#ifndef ZJOURNAL_CPP
#define ZJOURNAL_CPP

#include <zindexedfile/zjournal.h>
#include <zindexedfile/zsmasterfile.h>
#include <zindexedfile/zmasterfile.h>

using namespace zbs;

//! @addtogroup JournalingGroup {

static inline
void packFieldValues(ZDataBuffer &pBuffer,ZDataBuffer &pFieldBefore,ZDataBuffer &pFieldAfter)
{
    pBuffer = pFieldBefore.Size;
    pBuffer += pFieldBefore;
    pBuffer += pFieldAfter.Size;
    pBuffer += pFieldAfter;
}
static inline
void unpackFieldValues(ZDataBuffer &pBuffer,ZDataBuffer &pFieldBefore,ZDataBuffer &pFieldAfter)
{
unsigned char* wPtr = pBuffer.Data;
    pFieldBefore.setData(wPtr+sizeof(ssize_t),(ssize_t)*wPtr);
    wPtr+= sizeof(ssize_t);
    wPtr+= pFieldBefore.Size;
    pFieldAfter.setData(wPtr+sizeof(ssize_t),(ssize_t)*wPtr);
}


//---------------ZJEvent--------------------------

void
ZJEvent::getEventFromRecord(ZDataBuffer &pRecord)
{
  clear();
  memmove (&Header,pRecord.Data,sizeof(ZJE_Header));
  setData(pRecord.Data+sizeof(Header),pRecord.Size-sizeof(Header));
}

void
ZJEvent::getRecordFromEvent(ZDataBuffer &pRecord)
    {
    pRecord.setData(&Header,sizeof(Header));
    pRecord.appendData(*this);
    }
void
ZJEvent::getFieldChange(ZDataBuffer &pFieldBefore,ZDataBuffer&pFieldAfter)
    {
    unpackFieldValues(*this,pFieldBefore,pFieldAfter);
    return;
    }



ZStatus
ZJEventQueue::pop()
    {
    if (isEmpty())
            return ZS_EMPTY;
    Mtx.lock();
    delete Tab[lastIdx()] ;
    _Base::pop();
    Mtx.unlock();
    return ZS_SUCCESS;
    }//pop

ZStatus
ZJEventQueue::erase (const zrank_type pRank)
{
    if ((pRank<0)||(pRank>lastIdx()))
                        return ZS_OUTBOUND;
    delete Tab[pRank];
    _Base::erase(pRank);
    return ZS_SUCCESS;
}// erase

void
ZJEventQueue::enqueue (const ZJOperation &pZJOP,
                       pid_t pPid,
                       ZUserId pUid,
                       descString &pUsername,
                       ZDataBuffer &pRecord,
                       const zrank_type pRank,
                       const zaddress_type pAddress,
                       const ssize_t pOffset)
{
    ZJEvent *wEvent = new ZJEvent (pZJOP,pPid,pUid,pUsername,pRecord,pRank,pAddress,pOffset);
/*    wEvent->setData(pRecord);
    wEvent->Username = pUsername;
    wEvent->Pid = pPid;
    wEvent->Operation = pZJOP;
    wEvent->DateTime = ZDateFull::currentDateTime();*/
    Mtx.lock();
    _Base::push(wEvent);
    Mtx.unlock();
    return;
}// enqueue

void
ZJEventQueue::dequeue(ZJEvent &pEvent)
{
    if (isEmpty())
            return ;
    Mtx.lock();
    pEvent.setData(*Tab[0]);
    pEvent.Header.Pid = Tab[0]->Header.Pid;
    pEvent.Header.DateTime = Tab[0]->Header.DateTime;
    pEvent.Header.Uid = Tab[0]->Header.Uid;
    pEvent.Header.Username = Tab[0]->Header.Username;
    pEvent.Header.Rank= Tab[0]->Header.Rank;
    pEvent.Header.Address= Tab[0]->Header.Address;

    delete Tab[0];
    _Base::pop_front();
    Mtx.unlock();
    return ;
} // dequeue (pop_front)


//-----------------ZJournal--------------------
ZJournal::~ZJournal(void)
{
ZStatus wSt;
    if (isOpen())
    {
        end();
        JThread.join();
    }
    return;
}
/**
 * @brief ZJournal::init initialize journaling process for the ZMF file (pointed by Journaling::Father).
 * - Adequate parameters must have been set within Father's ZMCB :
 *  + JournalingOn : must be set to true for journaling to start
 *  + JournalPath : pointing to a valid local directory or empty if omitted
 * - Actions
 *  + opens journaling file.
 *  + initialize parameters of a thread in charge of processing the journaling event queue
 *
 *  Journal file must exist prior to execute this routine. see @ref ZJournal::createFile()
 * @param
 * @return  a ZStatus
 */
ZStatus
ZJournal::init(const bool pmustExist)
{
uriString   wURIJournal;
ZStatus     wSt;
    Pid=getpid();
/*    struct passwd *wUserDesc;
    Uid=getuid();

    wUserDesc = getpwuid(Uid);
    if (wUserDesc==nullptr)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_SYSTEMERROR,
                         Severity_Fatal,
                         "Fatal error while calling getpwduid for current process");
        ZException.exit_abort();
        }
    Username = wUserDesc->pw_name ;*/
    Username = ZUser().setToCurrentUser().getName().toString();

    if ((wSt=setJournalLocalDirectoryPath())!=ZS_SUCCESS)
                            {
                            ZException.addToLast(" While getting path for journal file");
                            return wSt;
                            }

    if (!wURIJournal.exists())  // if journal file does not exist : create it
        {
        if (pmustExist) // requested to exist
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTEXIST,
                                    Severity_Error,
                                    " Cannot start journaling. Journaling file %s is requested to exist.",
                                    wURIJournal.toString());
            return ZS_FILENOTEXIST;
            }
        wSt=createFile();
        if (wSt!=ZS_SUCCESS)
                    return wSt;
        }// !wURIJournal.exist()



    fprintf (stderr,
             " Starting journaling thread for file %s\n",
             Father->getURIContent().toString());
//             wMasterFile->getURIContent.toString());
//    JThread.init();
    return ZS_SUCCESS;
}// init


ZStatus
ZJournal::setJournalLocalDirectoryPath(void)
{
ZStatus wSt;
uriString   wURIFather;
uriString   wURIJournal;

    wURIFather=Father->getURIContent();

    wSt=generateJournalName(wURIFather,Father->ZMCB.ZJCB->JournalLocalDirectoryPath,wURIJournal);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    if ((wSt=_Base::setPath(wURIJournal))!=ZS_SUCCESS)
                            {
                            ZException.addToLast(" While setting path for journal file");
                            return wSt;
                            }
    return ZS_SUCCESS;
}// setPath

/**
 * @brief ZJournal::createFile creates or replaces journaling file.
 * JournalPath within Father's ZMCB must be either empty or pointing to a valid local directory.
 * @return a ZStatus. in case of error ZException is set with appropriate message.
 */
ZStatus
ZJournal::createFile(void)
{
ZStatus wSt;
uriString   wURIFather;
uriString   wURIJournal;

    wSt=setJournalLocalDirectoryPath();
    if (wSt!=ZS_SUCCESS)
            return wSt;
    wURIJournal = getURIContent();
    if (!wURIJournal.exists())
        {
        fprintf(stderr,
                "ZJournal::init-I-CREATE Journal file does not exist. Creating journal file %s\n",
                wURIJournal.toString());
        }
    else
        {
        fprintf(stderr,
                "ZJournal::init-I-CREATE Journal file exists. Replacing journal file %s\n",
                wURIJournal.toString());
        }
long wBlockTargetSize ;
long wInitialSize;

    wBlockTargetSize = Father->getBlockTargetSize() + sizeof(ZJEvent::Header);
    if (Father->getBlockTargetSize()>0)
            wInitialSize = wBlockTargetSize * (Father->getInitialSize()/Father->getBlockTargetSize());
        else
            wInitialSize = wBlockTargetSize * 10;
    wSt=zcreate(wURIJournal,
                wInitialSize,
                Father->getAllocatedBlocks(), // same allocated blocks as main file (Pool allocation)
                Father->getBlockExtentQuota(), // idem for extent quota (Pool extent quota)
                wBlockTargetSize,
                Father->getHighwaterMarking(), // Assuming that if main file is highwatermarking then will be journaling file
                true,   // GrabFreeSpace is set to true
                true,   // backup file in case of replace
                false); // No leave open  option

    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" while creating journal file for ZMasterFile %s",
                                   wURIFather.toString());

            }
    return wSt;
}//createFile


ZStatus
ZJournal::start(void)
{
        if (!JThread._start(&ZJournal::process,this))
                    return ZException.getLastStatus();
        return ZS_SUCCESS;
}


ZStatus
ZJournal::reset(ZMasterFile* pFather)
{
//uriString   wURIFather;
uriString   wURIJournal;
ZStatus     wSt;

    Father = pFather;
    wSt=setJournalLocalDirectoryPath();
    if (wSt!=ZS_SUCCESS)
                    return wSt;
//    wURIFather=Father->getURIContent();
//    wURIJournal=generateJournalName(wURIFather);
    wURIJournal=getURIContent();
    if (!wURIJournal.exists())
        {
        fprintf(stderr,
                "ZJournal::reset-I-DONTEXIST journal file %s does not exist\n",
                wURIJournal.toString());
        return ZS_SUCCESS;
        }

    fprintf(stderr,
            "ZJournal::reset-I-REMOVE removing journal file %s\n",
            wURIJournal.toString());
    int wRet=remove(wURIJournal.toString());
    if (wRet!=0)
        {
        ZException.getErrno(wRet,
                         _GET_FUNCTION_NAME_,
                         ZS_FILEERROR,
                         Severity_Severe,
                         " while trying to remove journal file %s",
                         wURIJournal.toString());
        return ZS_FILEERROR;
        }
    return ZS_SUCCESS;
}// reset


void
ZJournal::end(void)
{
    if (JThread.Created)    // if journaling thread active
        {
        ZDataBuffer wRecord;
        EvtQueue.enqueue(ZJOP_Close,Pid,Uid,Username,wRecord);  // enqueue termination message
        }
        else
        {
        if (isOpen())  // if journaling thread not active but journal file is open (may be journal thread aborted)
            {
            zclose();  // close journal file
            }// isOpen()
        }// else
//    EvtQueue.clear();
//    return zclose();
}
/**
 * @brief ZJournal::enqueue enqueues a new journaling event to be processed by journaling thread and written to journaling file
 * @note journaling thread is based on static routine ZJournal::process()
 * @param[in] pOperation journaling operation code to journalize see @ref ZJOP
 * @param[in] pRecord record content for journalized operation. Could be empty in case of operation ZJOP_Close
 * @param[in] pRank rank for record operation. Significant for remove by rank (ZJOP_RemoveByRank). defaulted to -1 if omitted.
 * @param[in] pAddress address for record operation. Significant for remove by address (ZJOP_RemoveByAddress). defaulted to -1 if omitted.
 */
void
ZJournal::enqueue (const ZJOperation pOperation,
                   ZDataBuffer &pRecord,
                   const zrank_type pRank,
                   const zaddress_type pAddress,
                   const ssize_t pOffset)
{
    EvtQueue.enqueue(pOperation,Pid,Uid,Username,pRecord,pRank,pAddress,pOffset);
    CMtx.signal();
}

/**
 * @brief ZJournal::enqueueSetFieldValue
 * @param pOperation
 * @param pFieldBefore
 * @param pFieldAfter
 * @param pRank
 * @param pAddress
 * @param pOffset
 */
void
ZJournal::enqueueSetFieldValue (const ZJOperation pOperation,
                                ZDataBuffer &pFieldBefore,
                                ZDataBuffer &pFieldAfter,
                                const zrank_type pRank,
                                const zaddress_type pAddress,
                                const ssize_t pOffset)
{
ZDataBuffer pBuffer;
    packFieldValues(pBuffer,pFieldBefore,pFieldAfter);
    EvtQueue.enqueue(pOperation,Pid,Uid,Username,pBuffer,pRank,pAddress,pOffset);
    CMtx.signal();
}

ZStatus
ZJournal::purge(const zrank_type pKeepRanks)
{
    ZDataBuffer wJournalRecord;
    ZJEvent wEvent;
    ZStatus wSt;
    if (pKeepRanks!=-1)
        if ((wSt=zget(wJournalRecord,0L))!=ZS_SUCCESS)
                                {
                                ZException.addToLast(" while purging journal file");
                                return wSt;
                                }
        while (size()>pKeepRanks)
        {

        wEvent.getEventFromRecord(wJournalRecord);
        while (true)
            {
        if (Father->ZMCB.ZJCB->Remote!=nullptr)     // if a remote mirroring is requested
                if (!wEvent.Header.State.is(ZJS_Exported)) // must be exported to remote host before being purged
                                                    break;

            break;
            }
        if ((wSt=zgetNext(wJournalRecord))!=ZS_SUCCESS)
                        {
                        if (wSt==ZS_EOF)
                                        break;
                        ZException.addToLast(" while purging journal file");
                        return wSt;
                        }
        }// size()>pKeepRanks
return ZS_SUCCESS;
} // purge




/**
 * @brief ZJournal::process processes the journaling event queue. Infinite loop waiting for an enqueue operation.
 * - opens the journaling file (file path must have been set previously with ZRandomFile::setPath() routine
 * - infinite loop until receive ZJOP_Close Operation code in event queue
 * - waits for conditional mutex
 * - to be waked up using signal or broadcast
 * - dequeue event queue till empty
 * - write each event into journal file
 * - in the end : close journal file
 *
 * @param pJournal
 * @return
 */
void*
ZJournal::process(void* pJournal) // client thread
{
ZStatus wSt;
ZJEvent wEvent;
ZDataBuffer wJournalRecord;
    ZJournal* wJournal = static_cast<ZJournal*> (pJournal);
    fprintf (stderr,
             " ZJournal::process-I- Starting journal processing. File is %s\n",
             wJournal->getURIContent().toString());
    wSt=wJournal->zopen(ZRF_All); // uri for journal file must have been set up using setPath
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Fatal,
                                " while opening journaling file for ZMasterFile %s",
                                wJournal->Father->getURIContent().toString());
        ZException.Thread_exit_abort();
        }
    while(true)
    {
        wJournal->CMtx.wait();
        fprintf (stderr,
                 " ZJournal::process-I-Wakeup wakeup : processing journaling event queue\n");
        while (wJournal->EvtQueue.size()>0)
        {
           wJournal->EvtQueue.dequeue(wEvent);
           if (wEvent.Header.Operation == ZJOP_Close) // request to shutdown
                                                   break;
           wJournalRecord.clear();

           wJournalRecord.setData(&wEvent.Header,sizeof(wEvent.Header));
           wJournalRecord.appendData(wEvent);
           wJournal->ThreadStatus=wJournal->zadd(wJournalRecord);
           if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast("While writing journal event within dedicated thread");
                ZException.Thread_exit_abort();
                }
           fprintf (stderr,
                    " ZJournal::process-I-write  1 record added to journaling file\n");
        }// while size()>0

    }// while

    fprintf (stderr,
             " ZJournal::process-I-End  closing journaling file %s\n",
             wJournal->getURIContent().toString());

    wJournal->ThreadStatus=wJournal->zclose();
    wJournal->JThread.exitThread( wJournal->ThreadStatus);
}// process

ZStatus
ZJournal::getEvent(zrank_type pRank,ZJEvent pEvent)
{
ZDataBuffer wBuffer;
ZStatus wSt;
    wBuffer.clear();

}

ZStatus
ZJournal::report (FILE* pOutput)
{
long wRank;
ZJEvent wEvent;
ZDataBuffer wRecord;
ZDataBuffer wLineHexa;
ZDataBuffer wLineChar;
ZDataBuffer wLineHexa2;
ZDataBuffer wLineChar2;
ZDataBuffer wFieldBefore;
ZDataBuffer wFieldAfter;
//ZJEvent::ZJE_Header *wEventHeader;

    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FILENOTOPEN,
                                Severity_Error,
                                " Journal file %s is not open (Journaling process not active). Cannot report journal.");
        return ZS_FILENOTOPEN;
        }

    fprintf (pOutput,
             "Rank| %60s |  Content dump |\n"
             "    |   Pid   |   Uid  |  Username  |  Date and time | \n",
             "Header data");

    while (zgetNext(wRecord)==ZS_SUCCESS)
    {
    wEvent.getRecordFromEvent(wRecord);
    if (wEvent.Header.Operation == ZJOP_ChgFld)
            {
            wEvent.getFieldChange(wFieldBefore,wFieldAfter);
            wFieldBefore.dumpHexa(0,30,wLineHexa,wLineChar);
            wFieldAfter.dumpHexa(0,30,wLineHexa2,wLineChar2);
            fprintf (pOutput,
                     "%3ld|%6ld|%6ld|%20s|%17s|%10s|%3ld|%6lld|%6ld|%s %s\n",
                     wRank,
                     wEvent.Header.Pid,
                     wEvent.Header.Uid,
                     wEvent.Header.Username,
                     wEvent.Header.DateTime.toLocaleFormatted(),
                     wEvent.Header.DateTime,
                     decode_ZJOP(wEvent.Header.Operation),
                     wEvent.Header.Rank,
                     wEvent.Header.Address,
                     wEvent.Header.Offset,
                     wLineHexa.DataChar,
                     wLineChar.DataChar
                     );
            }
        else
            wEvent.getRecordFromEvent(wRecord);

    wEvent.dumpHexa(0,30,wLineHexa,wLineChar);
    fprintf (pOutput,
             "%3ld|%6ld|%6ld|%20s|%17s|%10s|%3ld|%6lld|%6ld|%s %s\n",
             wRank,
             wEvent.Header.Pid,
             wEvent.Header.Uid,
             wEvent.Header.Username,
             wEvent.Header.DateTime.toLocaleFormatted(),
             wEvent.Header.DateTime,
             decode_ZJOP(wEvent.Header.Operation),
             wEvent.Header.Rank,
             wEvent.Header.Address,
             wEvent.Header.Offset,
             wLineHexa.DataChar,
             wLineChar.DataChar
             );
    }
}// report

//------------Generic functions------------------------------------
ZStatus
generateJournalName(uriString &pZMFName,uriString &pJournalingPath, uriString& pJournalName)
{
//descString wBase;
//uriString wJournalName;
    if (!pJournalingPath.isEmpty())
        if (!pJournalingPath.isDirectory())
            {
             ZException.setMessage(_GET_FUNCTION_NAME_,
                                     ZS_NOTDIRECTORY,
                                     Severity_Severe,
                                     "Defined journaling directory path <%s> is not a valid local directory",
                                     pJournalingPath.toString());
             return ZS_NOTDIRECTORY;
            }
//    wBase=pZMFName.getRootBasename();
    if (pJournalingPath.isEmpty())
                pJournalName = pZMFName.getDirectoryPath();
        else
                pJournalName = pJournalingPath;
    pJournalName.addConditionalDirectoryDelimiter();
    pJournalName += pZMFName.getRootBasename();
    pJournalName += "_jnl.";
    pJournalName += __JOURNAL_EXT__;
    return ZS_SUCCESS;
}

char * decode_ZJOP (const ZJOperation pOperation)
{
    switch (pOperation)
    {
        case ZJOP_Nothing:
            {
             return "ZJOP_Nothing";
            }
    case ZJOP_Add:
        {
         return "ZJOP_Add";
        }
    case ZJOP_Insert:
        {
         return "ZJOP_Insert";
        }
    case ZJOP_RemoveByRank:
        {
         return "ZJOP_RemoveByRank";
        }
    case ZJOP_RemoveByAddress:
        {
         return "ZJOP_RemoveByAddress";
        }
    case ZJOP_ChgFld:
        {
         return "ZJOP_ChgFld";
        }
    case ZJOP_Close:
        {
         return "ZJOP_Close";
        }
    case ZJOP_RollBack:
        {
         return "ZJOP_RollBack";
        }
    default:
        {
        return "Unknown ZJOP";
        }

    }// switch
}// decode_ZJOP


char*
ZJState::decode (void)
{

    if (State==ZJS_Nothing)
                return (Buf="ZJS_Nothing").content;

    if (State&ZJS_Created)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZJS_Created";
            }
    if (State&ZJS_Exported)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZJS_Exported";
            }
    if (State&ZJS_RolledBack)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZJS_RolledBack";
            }

    if (Buf.isEmpty())
                {
                return "Unknown ZJState";
                }
    return Buf.content;

}// decode
ZJState &
ZJState::encode (char *pString)
{
    State = ZJS_Nothing;
    if (pString==nullptr)
                {
                    return *this;
                }
    if (strlen(pString)==0)
                {
                    return *this;
                }
descString wBuf;
    wBuf.clear();
    wBuf = pString;
    wBuf.toUpper();
    if (wBuf=="ZJS_NOTHING")
            {
                return *this;
            }
    if (Buf.contains("ZJS_CREATED"))
                     State |= ZJS_Created;
    if (Buf.contains("ZJS_ROLLEDBACK"))
                     State |= ZJS_RolledBack;
    if (Buf.contains("ZJS_EXPORTED"))
                     State |= ZJS_Exported;

    return *this;

}// encode


char*
ZProtocol::decode (void)
{

    if (Protocol==ZP_Nothing)
                return (Buf="ZP_Nothing").content;

    if ((Protocol&ZP_Base)==ZP_Base)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZP_Base";
            }
    if (Protocol&ZP_RPC)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZP_RPC";
            }
    if (Protocol&ZP_Corba)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZP_Corba";
            }
    if (Protocol&ZP_DBus)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZP_DBus";
            }
    if (Protocol&ZP_RPC)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZP_RPC";
            }
    if (Protocol&ZP_XMLRPC)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZP_XMLRPC";
            }
    if (Protocol&ZP_SSLV5)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZP_SSLV5";
            }
    if (Protocol&ZP_B64)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZP_B64";
            }
    if (Buf.isEmpty())
                {
                return "Unknown ZJProtocol";
                }
    return Buf.content;

}// decode

ZProtocol &
ZProtocol::encode (const char *pString)
{
    Protocol = ZP_Nothing;
    if (pString==nullptr)
                {
                    return *this;
                }
    if (strlen(pString)==0)
                {
                    return *this;
                }
    Buf.clear();
    Buf = pString;
    Buf.toUpper();
    Buf.Trim();
    if (Buf=="ZJS_NOTHING")
            {
                return *this;
            }
    if (Buf.containsCase("ZP_Base"))
                     Protocol |= ZP_Base;
    if (Buf.containsCase("ZP_RPC"))
                     Protocol |= ZP_RPC;
    if (Buf.containsCase("ZP_Corba"))
                     Protocol |= ZP_Corba;
    if (Buf.containsCase("ZP_DBus"))
                     Protocol |= ZP_DBus;
    if (Buf.containsCase("ZP_XML-RPC"))
                     Protocol |= ZP_XMLRPC;
    if (Buf.containsCase("ZP_SSLV5"))
                     Protocol |= ZP_SSLV5;
    if (Buf.containsCase("ZP_B64"))
                     Protocol |= ZP_B64;

    return *this;

}// encode
 


char*
ZHAT::decode (void)
{
    Buf.clear();
    if (Type==ZHAT_Nothing)
                return (Buf="ZHAT_Nothing").content;

    if (Type&ZHAT_IPV6)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZHAT_IPV6";
            }
    if (Type&ZHAT_IPV4)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZHAT_IPV4";
            }
    if (Type&ZHAT_Default)
            {
            if (!Buf.isEmpty())
                        Buf += "|" ;
            Buf += "ZHAT_Default";
            }

    if (Buf.isEmpty())
                {
                return "Unknown ZJState";
                }
    return Buf.content;

}// decode
ZHAT &
ZHAT::encode (char *pString)
{
    Type = ZHAT_Nothing;
    if (pString==nullptr)
                {
                    return *this;
                }
    if (strlen(pString)==0)
                {
                    return *this;
                }
descString wBuf;
    wBuf.clear();
    wBuf = pString;
    wBuf.toUpper();
    if (wBuf=="ZHAT_Nothing")
            {
                return *this;
            }
    if (Buf.contains("ZHAT_IPV4"))
                     Type |= ZHAT_IPV4;
    if (Buf.contains("ZHAT_IPV6"))
                     Type |= ZHAT_IPV6;
    if (Buf.contains("ZHAT_Default"))
                     Type |= ZHAT_Default;

    return *this;

}// encode
//! @} JournalingGroup
#endif // ZJOURNAL_CPP
