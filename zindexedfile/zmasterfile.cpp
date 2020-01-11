#ifndef _ZMASTERFILE_CPP_
#define _ZMASTERFILE_CPP_

#include <zindexedfile/zmasterfile.h>
#include <zrandomfile/zrfcollection.h>
#include <zindexedfile/zjournal.h>
#include <QUrl>



bool ZMFStatistics = false;

/** @addtogroup ZBSOptions Setting Options for runtime
 * We can set on or off options in order to change runtime behavior of product
@{
*/

/**
 * @brief setStatistics Set the option ZMFStatistics on (true) or off (false)
 * If ZMFStatistics is set then statistics are made and could be reported
 */
void setZMFStatistics (bool pStatistics) {ZMFStatistics=pStatistics;}
/** @} */
using namespace zbs ;

//namespace zbs {

/**
  @addtogroup ZMasterFileGroup
  @{
  */


//------------ZIndexObjectTable-------------------

long ZIndexObjectTable::pop (void)
{
    if (size()<0)
        return -1;
    last()->~ZIndexFile();
    if (last()!=nullptr)
            {
            delete last();
            last()=nullptr;
            }
    return _Base::pop();
} // pop

long ZIndexObjectTable::erase (long pRank)
{
    if (pRank>lastIdx())
                return -1;
    Tab[pRank]->~ZIndexFile();

    if (Tab[pRank]!=nullptr)
            delete Tab[pRank];   // appearingly it puts the mess
    Tab[pRank]=nullptr; // just in case for avoiding double free
    return _Base::erase(pRank);
} // erase

void ZIndexObjectTable::clear(void)
{
    while (size()>0)
                    pop();
    _Base::clear();
}// clear

//------------------End ZIndexObjectTable ---------------------

//----------------ZIndexControlTable--------------------------

long ZIndexControlTable::erase(const long pIdx)
{
    if (Tab[pIdx].ZKDic!=nullptr)
                        delete Tab[pIdx].ZKDic;
    Tab[pIdx].ZKDic=nullptr;
    return _Base::erase(pIdx);
}
long ZIndexControlTable::push (ZIndexControlBlock &pICB)
{
    newBlankElement();
    last().clear();  // set up ZICB and its Key Dictionnary

    memmove (&last(),&pICB,sizeof(ZICBOwnData));
    long wj = 0 ;
    while (wj<pICB.ZKDic->size())
          {
          last().ZKDic->push(pICB.ZKDic->Tab[wj]);
          wj++;
          }
    return lastIdx();
}//push

long ZIndexControlTable::pop (void)
{
    if (size()<1) return -1;
    if (last().ZKDic!=nullptr)
            {
            delete last().ZKDic ;
            last().ZKDic=nullptr;
            }
    return _Base::pop();
}
void ZIndexControlTable::clear (void)
{
    while (size()>0)
                    pop();
    return;
}

long ZIndexControlTable::zsearchIndexByName (const char* pName)
{
    for (long wi =0;wi<size();wi++)
            {
            if (Tab[wi].Name==pName)
                        return wi;
            }

    return -1;
}//zsearchIndexByName
long ZIndexControlTable::zsearchIndexByName (descString pName)
{
    for (long wi =0;wi<size();wi++)
            {
            if (Tab[wi].Name==pName)
                        return wi;
            }

    return -1;
}//zsearchIndexByName

//-----------End ZIndexControlTable --------------------

// ----------ZMasterControlBlock ----------
//

ZMasterControlBlock::~ZMasterControlBlock(void)
{
    return;
}


long ZMasterControlBlock::popICB(void)
    {
    if (Index.pop()<0)
                return -1;
    IndexCount=Index.size();
    return Index.size();
    }


void ZMasterControlBlock::clear(void)
{
    IndexCount=0;
    Index.clear();
//    HistoryOn=false;
//    JournalingOn = false;
    MCBSize = 0;
    BlockID=ZBID_MCB;
    ZMFVersion= __ZMF_VERSION__;
    StartSign=cst_ZSTART;
    while (Index.size()>0)
                Index.pop();
    IndexObjects.clear();
    IndexFilePath.clear();
    return;
}

/**
 * @brief ZMasterControlBlock::pushICBtoIndex creates a new rank in ZMCB.Index rank and stores ZIndexControlblock in it
 * @param pICB
 */
void
ZMasterControlBlock::pushICBtoIndex(ZIndexControlBlock&pICB)
{
//    ZIndexField_struct wField;

    Index.newBlankElement();
    Index.last().clear();  // create ZICB instance -  Key Dictionnary

    IndexCount= Index.size();
    long wi = Index.lastIdx();

    memmove (&Index[wi],&pICB,sizeof(ZICBOwnData));
    long wj = 0 ;
    while (wj<pICB.ZKDic->size())
        {
        Index[wi].ZKDic->push(pICB.ZKDic->Tab[wj]);
        wj++;
        }
    return;
}

void
ZMasterControlBlock::removeICB(const long pRank)
{
 //   delete Index[pRank].ZKDic; - done in erase overloaded routine
    Index.erase(pRank);
    IndexObjects.erase(pRank);
    IndexCount= Index.size();
    return;
}

/**
 * @brief ZMasterFileControlBlock::_exportMCB   exports ZMasterFileControlBlock content to a flat ZDataBuffer.
 * Updates values (offset and size) for ZJournalingControlBlock if ever journaling exists.
 * Both values are set to -1 if journaling does not exist.
 * @return a ZDataBuffer containing the flat raw data exported from ZMCB
 */
ZDataBuffer& ZMasterControlBlock::_exportMCB(ZDataBuffer &pMCBContent)
{
ZDataBuffer wICBContent;
// set / reset IndexCount
    IndexCount = Index.size(); // by security

    pMCBContent.setData(this,sizeof(ZMCBOwnData)); // set Own MCB Data to ZDataBuffer
    for (long wi = 0;wi < Index.size(); wi++)
        {
        pMCBContent.appendData(Index[wi]._exportICB(wICBContent));  // export only index definition not object pointer
        }
    ZMCBOwnData* wMCB = (ZMCBOwnData*) pMCBContent.Data ;
    wMCB->MCBSize = pMCBContent.Size;          // update size of exported MCB in exported buffer

    //---------ZJournalingControlBlock data-----------------------

    if (ZJCB==nullptr)
            {
            wMCB->JCBOffset = -1 ;
            wMCB->JCBSize = -1 ;
            }
        else        // if journaling defined : export ZJournalControlBlock
            {
            wMCB->JCBOffset = wMCB->MCBSize;

            ZDataBuffer wJCB;
            if (ZJCB!=nullptr)
                    pMCBContent.appendData(ZJCB->_exportJCB(wJCB)); //
            wMCB->JCBSize = wJCB.Size;
            }
    return pMCBContent;
}// _exportMCB

/**
 * @brief ZMasterFileControlBlock::_importMCB imports (rebuild) a ZMasterFileControlBlock from a ZDataBuffer containing flat raw data to import
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterControlBlock::_importMCB(ZDataBuffer& pBuffer)
{
ZStatus wSt;
//long wIndexCountsv ;

ZMCBOwnData* wMCB =(ZMCBOwnData*) pBuffer.Data;
    if (ZVerbose)
            fprintf (stdout,
                     "_importMCB-I- Importing MCB\n");
    if (wMCB->BlockID != ZBID_MCB)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_BADICB,
                               Severity_Error,
                               "Error Master Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
                               wMCB->BlockID);
        return(ZS_BADICB);
    }
    if (wMCB->StartSign != cst_ZSTART)
    {
   ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_BADFILEHEADER,
                          Severity_Error,
                          "Error Master Control Block  : file header appears to be corrupted - invalid MCB StartBlock");
   return(ZS_BADFILEHEADER);
    }
    if (wMCB->ZMFVersion!= __ZMF_VERSION__)
    {
   ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_BADFILEVERSION,
                          Severity_Error,
                          "Error Master Control Block   : Found version <%ld> while current ZMF version is <%ld>",
                          wMCB->ZMFVersion,
                          __ZMF_VERSION__);
   return(ZS_BADFILEVERSION);
    }

    clear();
    memmove(this,pBuffer.Data,sizeof(ZMCBOwnData));

//-----------Import defined IndexControlBlocks for the file

//    wIndexCountsv = IndexCount;
    if (ZVerbose)
            fprintf (stdout,
                     "_importMCB-I- Importing ICBs\n");
    unsigned char* wPtr;
    ssize_t wInSize;
    wPtr = pBuffer.Data + sizeof(ZMCBOwnData);
    for (long wi = 0;wi < IndexCount; wi++)  // IndexCount has been updated from memmove
    {

        Index.newBlankElement();
        wSt=Index.last()._importICB(wPtr,wInSize);
        if (wSt!=ZS_SUCCESS)
                        return wSt;
        wPtr = wPtr + wInSize ;    // adjust wPtr to next ZIndexControlBlock
//        if (wPtr >= (pBuffer.Data+pBuffer.Size))
//                        break;
        if (wPtr >= (pBuffer.Data + JCBOffset))
                                            break;
    }// for

//------------Import Journaling Control block if defined must be done else where--------

    if (ZVerbose)
            fprintf (stdout,
                     "_importMCB-I- End import MCB + ICBs\n");

    return ZS_SUCCESS;
}//_importMCB

/**
 * @brief ZMasterControlBlock::print Reports the whole content of ZMCB : indexes definitions and dictionaries
 */
void
ZMasterControlBlock::report(FILE*pOutput)
{
    fprintf (pOutput,
             "________________ZMasterControlBlock Content________________________________\n");
    for (long wi=0;wi < Index.size();wi++)
        {
         fprintf (pOutput,
                  "Index Rank <%2ld> <%20s>\n"
                 "          <%15s> <%15s> Index Record size <%ld>\n",
                 wi,
                 Index[wi].Name.toString(),
                 Index[wi].Duplicates==ZST_DUPLICATES?"Duplicates":"No Duplicates",
                 Index[wi].AutoRebuild?"Autorebuild":"NoAutorebuild",
                 Index[wi].IndexRecordSize());
         fprintf (pOutput,
                  "   Fields %17s %5s %5s %12s %s\n",
                 "Field Name",
                 "Natural",
                 "Internal",
                 "Offset",
                 "ZType");
        for (long wj=0; wj < Index[wi].ZKDic->size();wj++)
        {
         fprintf (pOutput,
                 "    <%ld> <%15s>  %5ld %5ld %12ld %s\n",
                 wj,
                 Index[wi].ZKDic->Tab[wj].Name.toString(),
                 Index[wi].ZKDic->Tab[wj].NaturalSize,
                 Index[wi].ZKDic->Tab[wj].UniversalSize,
                 Index[wi].ZKDic->Tab[wj].RecordOffset,
                 decode_ZType(Index[wi].ZKDic->Tab[wj].ZType));
        }
        }
    fprintf (pOutput,
             "___________________________________________________________________________\n");
    return;
} // print
#ifndef ZJOURNALCONTROLBLOCK_CPP
#define ZJOURNALCONTROLBLOCK_CPP
//-----------------ZJournalControlBlock-----------------------------

ZJournalControlBlock::~ZJournalControlBlock(void)
{
    if (Journal!=nullptr)
                delete Journal;
    return;
}


void ZJournalControlBlock::clear(void)
{
    memset(this,0,sizeof(ZJournalControlBlock));
    BlockID=ZBID_MCB;
    ZMFVersion= __ZMF_VERSION__;
    StartSign=cst_ZSTART;
    JournalLocalDirectoryPath.clear();
    return;
}

/**
 * @brief ZJournalControlBlock::_exportJCB   exports ZJournalControlBlock content to a flat ZDataBuffer.
 * @return a ZDataBuffer containing the flat raw data exported from ZJCB
 */
ZDataBuffer& ZJournalControlBlock::_exportJCB(ZDataBuffer &pJCBContent)
{
ZDataBuffer wJCBContent;


    pJCBContent.setData(this,sizeof(ZJCBOwnData)); // set Own JCB Data to ZDataBuffer

    ZJCBOwnData* wJCB = (ZJCBOwnData*) pJCBContent.Data ;
    wJCB->JCBSize = pJCBContent.Size;          // update size of exported JCB in exported buffer

    return pJCBContent;
}// _exportJCB

ssize_t
ZJournalControlBlock::_getExportSize()
{
    return (sizeof(ZJCBOwnData));
}

/**
 * @brief ZJournalControlBlock::_importJCB imports (rebuild) a ZJournalControlBlock from a ZDataBuffer containing flat raw data to import
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZJournalControlBlock::_importJCB(ZDataBuffer& pJCBContent)
{
//ZStatus wSt;
//long wIndexCountsv ;

ZJCBOwnData* wJCB =(ZJCBOwnData*) pJCBContent.Data;

    if (wJCB->BlockID != ZBID_JCB)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_BADICB,
                               Severity_Error,
                               "Error Journal Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
                               wJCB->BlockID);
        return(ZS_BADICB);
    }
    if (wJCB->StartSign != cst_ZSTART)
    {
   ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_BADFILEHEADER,
                          Severity_Error,
                          "Error Journal Control Block  : Index header appears to be corrupted - invalid ICB StartBlock");
   return(ZS_BADFILEHEADER);
    }
    if (wJCB->ZMFVersion!= __ZMF_VERSION__)
    {
   ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_BADFILEVERSION,
                          Severity_Error,
                          "Error Journal Control Block   : Found version <%ld> while current ZMF version is <%ld>",
                          wJCB->ZMFVersion,
                          __ZMF_VERSION__);
   return(ZS_BADFILEVERSION);
    }

    clear();
    memmove(this,pJCBContent.Data,sizeof(ZJCBOwnData));

    return ZS_SUCCESS;
}//_importJCB

ZStatus
ZJournalControlBlock::purge(const zrank_type pKeepRanks)
{
    if (Journal==nullptr)
                return ZS_INVOP;
    return Journal->purge(pKeepRanks);
}
/**
 * @brief ZJournalControlBlock::setParameters sets the local journaling parameters (actually only JournalDirectoryPath)
 * @param pJournalPath
 */
void ZJournalControlBlock::setParameters (uriString &pJournalPath)
{
    JournalLocalDirectoryPath=pJournalPath;
    return;
}
void
ZJournalControlBlock::setRemoteMirroring (int8_t pProtocol,
                                          char* pHost,
                                          char* pPort,
                                          char* pAuthenticate,
                                          char* pSSLKey,
                                          char* pSSLVector)
{
    if (Remote!=nullptr)
                delete Remote;
    Remote = new ZRemoteMirroring;
    Remote->Protocol = pProtocol;
    strncpy(Remote->Host,pHost,sizeof(Remote->Host));
    strncpy(Remote->Port,pPort,sizeof(Remote->Port));
    strncpy(Remote->Authenticate,pAuthenticate,sizeof(Remote->Authenticate));
    strncpy(Remote->SSLKey,pSSLKey,sizeof(Remote->SSLKey));
    strncpy(Remote->SSLVector,pSSLVector,sizeof(Remote->SSLVector));

}
#endif // ZJOURNALCONTROLBLOCK_CPP

//------------------------------------ZMasterFile-------------------
//
ZMasterFile::ZMasterFile(void) //: ZRandomFile(_cst_default_allocation,_cst_realloc_quota)
{
// ZMCB.JournalingOn=pJournaling;
 ZMCB.ZJCB=nullptr;
// ZMCB.JournalingOn = false;
return;
}

ZMasterFile::ZMasterFile(uriString pURI) : ZRandomFile(pURI)
{
    ZStatus wSt=setPath(pURI);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
//    ZMCB.JournalingOn=false;
    ZMCB.ZJCB=nullptr;
//    Journal->init(this);
    return;
}


ZMasterFile:: ~ZMasterFile(void)
{
    if (ZDescriptor._isOpen)
                        zclose();
     while (ZMCB.IndexObjects.size() >0)
             {
             if (ZMCB.IndexObjects.last()->ZDescriptor._isOpen)
                                            ZMCB.IndexObjects.last()->closeIndexFile();
             ZMCB.IndexObjects.pop(); // deletes the contained object
             }

      while (ZMCB.Index.size() >0)
             {
             ZMCB.Index.pop();
             }

      setJournalingOff();
}// DTOR

//------------Journaling--------------------------------

ZStatus
ZMasterFile::initJournaling (uriString &pJournalPath)
{
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
// this does not touch integrity of files

    if (!ZDescriptor._isOpen)
            {

                wasOpen=false;
                wSt = zopen(ZRF_Exclusive|ZRF_Write);
                if (wSt!=ZS_SUCCESS)
                        return wSt;
            }
        else
        {
        if (ZDescriptor.Mode!=(ZRF_Exclusive|ZRF_Write))
            {
            wFormerMode = ZDescriptor.Mode;
            zclose();
            zopen(ZRF_Exclusive|ZRF_Write);
            }

        }// else


    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOn-I Starting journaling for file %s\n",
                     getURIContent().toString());
    if (ZMCB.ZJCB==nullptr)
            {
            ZMCB.ZJCB = new ZJournalControlBlock;
            ZMCB.ZJCB->Journal = new ZJournal(this);
            }
        else
    {
    if (ZMCB.ZJCB!=nullptr)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot start journaling for file %s : already started.\n",
                     getURIContent().toString());
            if (ZMCB.ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZMCB.ZJCB->Journal->getURIContent().toString());
                        return ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Journaling for file %s is set to started while journal file is marked as not open.",
                                    getURIContent().toString());
            ZException.exit_abort();
//            return ZS_FILENOTOPEN;
            }
        } // else
    wSt=ZMCB.ZJCB->Journal->init();
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While initializing journaling");
        return wSt;
        }

//    ZMCB.JournalingOn=true;
    ZMCB.ZJCB->JournalLocalDirectoryPath = pJournalPath;
    wSt=writeControlBlocks();

    if (wSt!=ZS_SUCCESS)
                    return wSt;
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-JCB Journaling set on within JournalingControlBlock\n");

    if (!wasOpen)  // if was not open close it
            return zclose();

    if (wFormerMode!=ZRF_Nothing) // if was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open with former mode
            }
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    wSt=ZMCB.ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    }
    else
        {
        ZException.addToLast("While starting journaling thread for journaling file %s",
                             ZMCB.ZJCB->Journal->getURIContent().toString());
        ZException.printUserMessage(stderr);

        }// else
    return wSt;
} // initJournaling

ZStatus
ZMasterFile::setJournalingOn (void)
{
ZStatus wSt;
//bool wasOpen=true;
//zmode_type wFormerMode=ZRF_Nothing;
// this does not touch integrity of files

if (!ZDescriptor._isOpen)
        {
        fprintf (stderr,
                 "setJournalingOn-E File <%s> must be open to start/restart journaling.\n",
                 getURIContent().toString());
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                "File <%s> must be open to start/restart journaling.",
                                getURIContent().toString());
        return ZS_INVOP;
        }

    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOn-I Starting/restarting journaling on for file %s\n",
                     getURIContent().toString());
    if (ZMCB.ZJCB->Journal==nullptr)
                {
                fprintf (stderr,
                         "setJournalingOn-E Journaling has not be defined for file %s. Cannot start/restart journaling.\n",
                         getURIContent().toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVOP,
                                        Severity_Error,
                                        "Journaling has not be defined for file %s. Cannot start/restart journaling.",
                                        getURIContent().toString());
                return ZS_INVOP;
                }

    if (ZMCB.ZJCB->Journal->JThread.getState() > ZTHS_Nothing)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot start journaling for file %s : journaling already started. Thread id is %ld\n",
                     getURIContent().toString(),
                     ZMCB.ZJCB->Journal->JThread.getId());
            if (ZMCB.ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZMCB.ZJCB->Journal->getURIContent().toString());
                        return ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Setting journaling on : Journaling for file %s is started while journal file is marked as not open.",
                                    getURIContent().toString());
            return ZS_FILENOTOPEN;
            }

    wSt=ZMCB.ZJCB->Journal->init(true); // Journal file must exist
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While initializing journaling");
        return wSt;
        }


 //   ZMCB.JournalingOn=true;
 //   ZMCB.ZJCB->JournalLocalDirectoryPath = pJournalPath;
 //   wSt=writeControlBlocks();

//    if (wSt!=ZS_SUCCESS)
//                    return wSt;

    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    wSt=ZMCB.ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    }
    else
    {
        if (ZVerbose)
            {
                fprintf(stdout,
                        "setJournalingOn-E-Failure Journaling on file %s has not started.See ZException stack dump (following) to get information\n",
                        ZMCB.ZJCB->Journal->getURIContent().toString());
                ZException.printUserMessage();
            }
    }// else

/*    if (!wasOpen)  // if ZMasterFile was not open close it
            return zclose();

    if (wFormerMode!=ZRF_Nothing) // if ZMasterFile was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open ZMasterFile with former mode
            }*/
    return wSt;
} // setJournalingOn

#ifdef __COMMENT__
/**
 * @brief ZMasterFile::setJournalingOn starts Journaling process for the given file.
 * If journaling is already started, nothing happens.
 * ZMasterFile must be open to start journaling, otherwise a ZS_INVOP status is returned.
 *
 * @return a ZStatus set to ZS_SUCCESS if everything went well
 */
ZStatus
ZMasterFile::setJournalingLOn()
{
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
// this does not touch integrity of files

if (!ZDescriptor._isOpen)
                return (ZS_INVOP);

    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOn-I Setting journaling on for file %s\n",
                     getURIContent().toString());
    if (ZMCB.ZJCB->Journal==nullptr)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                " Journaling parameters have not be defined for file <%s>. Cannot start/restart journaling.",
                                getURIContent().toString());
    }

        else

//    if (ZMCB.JournalingOn)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot set journaling on for file %s : journaling already started.\n",
                     getURIContent().toString());
            if (ZMCB.ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZMCB.ZJCB->Journal->getURIContent().toString());
                        return ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Setting journaling on : Journaling for file %s is started while journal file is marked as not open.",
                                    getURIContent().toString());
            return ZS_FILENOTOPEN;
            }
    wSt=ZMCB.ZJCB->Journal->init(true); // journaling file must have been created elsewhere and must exist
    if (wSt!=ZS_SUCCESS)
        {

        ZException.addToLast("While initializing journaling");
        return wSt;
        }

/*
 //   ZMCB.JournalingOn=true;
 //   ZMCB.ZJCB->JournalLocalDirectoryPath = pJournalPath;
    wSt=writeControlBlocks();

    if (wSt!=ZS_SUCCESS)
                    return wSt;
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-MCB Journaling set on within MCB header \n");

    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    wSt=ZMCB.ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    }
    else
    {
        if (ZVerbose)
            {
                fprintf(stdout,
                        "setJournalingOn-E-Failure Journaling on file %s has not started.See ZException stack dump to get information\n",
                        ZMCB.ZJCB->Journal->getURIContent().toString());
                ZException.printUserMessage();
            }
    }// else

    if (!wasOpen)  // if ZMasterFile was not open close it
            return zclose();

    if (wFormerMode!=ZRF_Nothing) // if ZMasterFile was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open ZMasterFile with former mode
            }
    return wSt;*/
} // setJournalingOn
#endif // __COMMENT__

ZStatus
ZMasterFile::setJournalingOff (void)
{
//    if (!ZMCB.JournalingOn)
//                    return ZS_SUCCESS;
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
    // this does not touch integrity of files

    if (!ZDescriptor._isOpen)
            {

                wasOpen=false;
                wSt = zopen(ZRF_Exclusive|ZRF_Write);
                if (wSt!=ZS_SUCCESS)
                        return wSt;
            }
        else
        {
        if (ZDescriptor.Mode!=(ZRF_Exclusive|ZRF_Write))
            {
            wFormerMode = ZDescriptor.Mode;
            zclose();
            zopen(ZRF_Exclusive|ZRF_Write);
            }

        }// else


    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOff-I Stopping journaling for file %s\n",
                     getURIContent().toString());
    if (ZMCB.ZJCB->Journal==nullptr)
                return ZS_SUCCESS;

/*    if (!ZMCB.JournalingOn)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Journaling for file %s already stopped\n",
                     getURIContent().toString());
            if (ZMCB.ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning Journaling stopped but journal file %s is open. closing file\n",
                                 Journal->getURIContent().toString());
                        return Journal->zclose();
                        }
            delete Journal;
            return ZS_SUCCESS;
            }*/

    delete ZMCB.ZJCB;  // deletes Journaling Control Block  AND ZJournal instance : send a ZJOP_Close to journal thread



//    ZMCB.JournalingOn=false;
    wSt=writeControlBlocks();
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-MCB Journaling for file %s has been set off within MCB header \n",
                    getURIContent());


    if (!wasOpen)  // if was not open close it
            return zclose();

    if (wFormerMode!=ZRF_Nothing) // if was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open with former mode
            }
    return wSt;
} // setJournalingOff

//----------- ZMasterFile Index methods----------------
//


/**
 * @brief ZMasterFile::setIndexFilesDirectoryPath  sets a fixed Directory path into which index files will be stored for this master file
 *
 *  General case is that index files are stored in the same given path that the Master File.
 *
 *  But it may be interesting to store index files on a different device / directory from the one containing Master File for performance / security purpose.
 *
 *  Error cases
 *
 *  - File must not be open to set IndexFilesDirectoryPath.
 *  - Mentionned path must be a valid, existing directory.
 *
 * @note operation is not usual as index files could be moved while file is already indexed.
 *
 * @param[in] pPath an uriString containing the path where ZIndexFiles will be located
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::setIndexFilesDirectoryPath (uriString &pPath)
{
ZStatus wSt;
ZDataBuffer wReserved;
    if (isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Error,
                                    " Cannot modify IndexFilesDirectoryPath while file is already open for file  <%s>",
                                    ZDescriptor.URIContent.toString());
                    return ZS_INVOP;
            }
    if (!pPath.isDirectory())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_NOTDIRECTORY,
                                    Severity_Error,
                                    " Cannot set IndexFilesDirectoryPath : path is not a valid directory. Content file is <%s>",
                                    ZDescriptor.URIContent.toString());
                    return ZS_NOTDIRECTORY;
            }
    wSt=_Base::_open(ZDescriptor,ZRF_Exclusive|ZRF_All,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While openning for setting IndexFileDirectoryPath. File <%s>",
                               ZDescriptor.URIContent.toString());
        return wSt;
        }
    ZMCB.IndexFilePath = pPath;

    _Base::setReservedContent(ZMCB._exportMCB(wReserved));
    wSt=_Base::_writeReservedHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot set IndexFilesDirectoryPath / cannot write Reserved header. Content file is <%s>",
                                ZDescriptor.URIContent.toString());
        }
    _Base::_close(ZDescriptor);
    return wSt;
}//setIndexFilesDirectoryPath

ZStatus
ZMasterFile::setJournalLocalDirectoryPath (uriString &pPath)
{
ZStatus wSt;
ZDataBuffer wReserved;
    if (isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Error,
                                    " Cannot modify JournalingLocalDirectoryPath while Journaling is started for file  <%s>",
                                    getURIContent().toString());
                    return ZS_INVOP;
            }
    if (!pPath.isDirectory())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_NOTDIRECTORY,
                                    Severity_Error,
                                    " Cannot set JournalingLocalDirectoryPath : path is not a valid directory. Content file is <%s>",
                                    ZDescriptor.URIContent.toString());
                    return ZS_NOTDIRECTORY;
            }
    wSt=_Base::_open(ZDescriptor,ZRF_Exclusive|ZRF_All,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While openning for setting JournalingLocalDirectoryPath. File <%s>",
                               getURIContent().toString());
        return wSt;
        }
    ZMCB.ZJCB->JournalLocalDirectoryPath = pPath;

    _Base::setReservedContent(ZMCB._exportMCB(wReserved));
    wSt=_Base::_writeReservedHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot set IndexFilesDirectoryPath / cannot write Reserved header. Content file is <%s>",
                                getURIContent().toString());
        }
    _Base::_close(ZDescriptor);
    return wSt;
}//setJournalingLocalDirectoryPath


//------------------File Header : updates (read - write) ZMasterControlBlock AND ZJournalControlBlock if exists
/**
 * @brief ZMasterFile::zgetJCBfromHeader Accesses File header and updates JournalingControlBlock if any.
 * There is no reverse operation : writting JCB to Header is done using writing whole Control blocks to header file
 * using ZMasterFile::writeControlBlocks()
 *
 * @return
 */
ZStatus
ZMasterFile::readJCBfromHeader(void)
{
ZStatus wSt;
ZDataBuffer wRawMCB;
    wSt=_Base::getReservedBlock(wRawMCB,true);
    if (wSt!=ZS_SUCCESS)
            {
            return wSt;
            }
    return _getJCBfromReserved();
}
/**
 * @brief ZMasterFile::_getJCBfromReserved updates Journaling control block if any with its content from ZReserved from Filedescriptor.
 *  ZReserved have to be up to date with an already done getReservedBlock().
 * @return
 */
ZStatus
ZMasterFile::_getJCBfromReserved(void)
{
    ZMasterControlBlock* wMCB = (ZMasterControlBlock*)ZDescriptor.ZReserved.Data;

    if (wMCB->JCBSize<1)  // no journaling
                return ZS_SUCCESS;
    if (ZMCB.ZJCB==nullptr)
                {
                ZMCB.ZJCB=new ZJournalControlBlock;
                }
    ZDataBuffer wJCBContent;
    wJCBContent.setData(ZDescriptor.ZReserved.Data+wMCB->JCBOffset,wMCB->JCBSize);
    return ZMCB.ZJCB->_importJCB(wJCBContent);
}



/**
 * @brief ZMasterFile::writeMasterControlBlock updates ZMasterControlBlock AND ZJournalControlBlock (if exists)
 * for current ZMasterFile to ZReserved Header within header file.
 *
 * see @ref ZMasterControlBlock::_exportMCB()
 * see @ref ZJournalControlBlock::_exportJCB()
 *
 * @return
 */
ZStatus
ZMasterFile::writeControlBlocks(void)
{
ZStatus wSt;
ZDataBuffer wReserved;

    ZMCB._exportMCB(wReserved);
    _Base::setReservedContent(wReserved);
    wSt=_Base::_writeReservedHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot write Reserved Header (ZMasterControlBlock+ZJournalControlBlock) to file header. Content file is <%s>",
                                getURIContent().toString());
        }
    return wSt;
}//writeControlBlocks

/**
 * @brief ZMasterFile::readControlBlocks reads ZMasterControlBlock AND ZJournalControlBlock is exists
 * see @ref ZMasterControlBlock::_importMCB()
 * see @ref ZJournalControlBlock::_importJCB()
 * @return
 */
ZStatus
ZMasterFile::readControlBlocks(void)
{
ZStatus wSt;
ZDataBuffer wReserved;
//ZDataBuffer wRawJCB;

    wSt=_Base::getReservedBlock(wReserved,true);
    if (wSt!=ZS_SUCCESS)
            {
            return wSt;
            }

    wSt=ZMCB._importMCB(wReserved);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While importing ZMCB");
            return wSt;
            }
    if (ZMCB.JCBOffset<1)
            {
            if (ZMCB.ZJCB!=nullptr)
                    delete ZMCB.ZJCB;
             return wSt;
            }
    ZDataBuffer wReservedJCB;
    wReservedJCB.setData(wReserved.Data+ZMCB.JCBOffset,ZMCB.JCBSize);
    return ZMCB.ZJCB->_importJCB(wReservedJCB);

}// readControlBlocks



//  To add a new index :
//
//       _addIndexField:
//         create all fields definition in key sequence order
//
//       _addIndexKey
//         Create the index block (ZICB)
//
//       _ZIndexFileCreate
//

void
ZMasterFile::_addIndexField (ZArray<ZIndexField>& pZIFField,descString& pName, size_t pOffset, size_t pLength)
{
    ZIndexField wField;
    wField.Name = pName;
    wField.RecordOffset = pOffset;
    wField.NaturalSize = pLength;
    pZIFField.push(wField);
}

/**
 * @brief ZMasterFile::_addIndexKeyDefinition creates a new ZICB from pZIFFields field list definition and push it to pZICB
 *
 *
 * @param[out] pZICB     ZIndexControlBlock to populate with index definition (Key dictionary)
 * @param[in] pZIFField     Key fields dictionary to add
 * @param[in] pIndexName    User name of the index key as a descString (for description only)
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
 */

void
ZMasterFile::_addIndexKeyDefinition (ZIndexControlBlock& pZICB,
                          ZKeyDictionary& pZIFField,
                          descString pIndexName,
                          ZSort_Type pDuplicates)
{
//        ZIndexControlBlock ZICB;
    pZICB.clear();
    pZICB.Name = pIndexName;
 //   pZICB.AutoRebuild = pAutoRebuild ;
    //pZICB.KeyType = pKeyType;
    pZICB.Duplicates = pDuplicates ;

    for (long wi = 0; wi<pZIFField.size();wi++)
        {
        pZICB.ZKDic->push(pZIFField[wi]);
//        wKeySize += pZIFField[wi].NaturalSize ;
        }
//    pZICB.ExternalKeySize = wKeySize;
        pZICB.ZKDic->_reComputeSize();

//        pZICB.push(ZICB);
}


/**
 * @brief ZMasterFile::zcreateIndexFile Generates a new index from a description (meaning a new ZRandomFile data + header).
 *
 * This routine will create a new index with the files structures necessary to hold and manage it : a ZIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 *
 *@note
 * 1. ZIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZIndexFile file pathname is not stored but is a computed data from actual ZMasterFile file pathname.
 *
 * @param[in] pZIFField     Key fields dictionary to create the index with
 * @param[in] pIndexName    User name of the index key as a descString
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zcreateIndex (ZKeyDictionary &pZIFField,  // contains the description of index key to add (multi fields)
                           utfdescString &pIndexName,
//                          bool pAutorebuild,
                           ZSort_Type pDuplicates,
                           bool pBackup)
{
ZStatus wSt;
ZIndexControlBlock wZICB;
bool wIsOpen = isOpen();
long wi;
zsize_type wIndexAllocatedSize=0;
uriString wIndexURI;
uriString wIndexFileDirectoryPath;
ZIndexFile *wIndexObject;

        if (!isOpen())
                {
                wSt=zopen(ZRF_Exclusive|ZRF_All);
                }
            else
        {
        if ((getMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All))
                {
                ZException.setMessage (_GET_FUNCTION_NAME_,
                                         ZS_MODEINVALID,
                                         Severity_Error,
                                         " ZMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                                         getURIContent().toString());
                return ZS_MODEINVALID;
                }
        }//else

// Control index name ambiguity

    if ((wi=ZMCB.Index.zsearchIndexByName(pIndexName))>-1)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVNAME,
                                        Severity_Error,
                                        " Ambiguous index name <%s>. Index name already exist at Index rank <%ld>. Please use zremoveIndex first.\n",
                                        pIndexName.toString(),
                                        wi);
                wSt= ZS_INVNAME;
                goto zcreateIndexEnd ;
                }


    _addIndexKeyDefinition ( wZICB,
                             pZIFField,
                             pIndexName,
//                             pKeyType,
//                             pAutorebuild,
                             pDuplicates);



    ZMCB.pushICBtoIndex(wZICB);  // since here any creation error will induce a desctruction of ZMCB.Index.lastIdx()

// instantiation of the ZIndexFile new structure :
// it gives a pointer to the actual ICB stored in Index vector
//
    wIndexObject = new ZIndexFile(this,&ZMCB.Index.last() );

    ZMCB.IndexObjects.push(wIndexObject); // since here any creation error will induce a desctruction of ZMCB.IndexObjects.lastIdx()

// ---------compute index file name-------------------

// Define IndexFileDirectoryPath
//     if mentionned then take it
//     if not then take the directory from Master File URI

    if (ZMCB.IndexFilePath.isEmpty())
        {
        wIndexFileDirectoryPath=getURIContent().getDirectoryPath().toCChar();
        }
        else
        {
        wIndexFileDirectoryPath=ZMCB.IndexFilePath;
        }

    wSt=generateIndexURI(getURIContent(),
                         wIndexFileDirectoryPath,
                         wIndexURI,
                         ZMCB.Index.lastIdx(),
                         pIndexName);
    if (wSt!=ZS_SUCCESS)
                    return wSt;

// compute the allocated size

    if (_Base::getBlockTargetSize()>0)
                if (_Base::getAllocatedBlocks()>0)
                    wIndexAllocatedSize =  _Base::getAllocatedBlocks() * ZMCB.Index.last().IndexRecordSize();
//
// Nota Bene : there is no History and Journaling processing for Index Files
//

    wSt =  wIndexObject->zcreateIndex(&ZMCB.Index.last(),      // pointer to index control block because ZIndexFile stores pointer to Father's ICB
                                      wIndexURI,
                                      _Base::getAllocatedBlocks(),
                                      _Base::getBlockExtentQuota(),
                                      wIndexAllocatedSize,
                                      _Base::getHighwaterMarking(),
//                                      _Base::getGrabFreeSpace(),
                                      false,        // grabfreespace is set to false
                                      pBackup,
                                      false          // do not leave it open
                                      );
    if (wSt!=ZS_SUCCESS)
                {
                goto zcreateIndexError;
                }

// update MCB to Reserved block in RandomFile header (_Base)
// then write updated Master Control Block to Master Header
//
    wSt= wIndexObject->openIndexFile(wIndexURI,(ZRF_Exclusive | ZRF_All));
    if (wSt!=ZS_SUCCESS)
            {
            goto zcreateIndexError;
            }
    wSt=wIndexObject->zrebuildIndex(ZMFStatistics ,stderr);
    if (wSt!=ZS_SUCCESS)
            {
            goto zcreateIndexError;
            }

zcreateIndexEnd:
    if (!wIsOpen)
            zclose();  // close everything and therefore update MCB in file

    // else All dependent files are open with mode (ZRF_Exclusive | ZRF_All) when returning.

    return wSt;
zcreateIndexError:

    ZMCB.IndexObjects.last()->zclose();
    ZMCB.IndexObjects.pop() ; // destroy the ZIndexFile object
    ZMCB.popICB(); // destroy created ICB
    ZException.addToLast(" While creating Index <%ld> for MasterFile <%s>",
                             ZMCB.Index.lastIdx(),
                             ZDescriptor.URIContent.toString());
    return wSt;

}//zcreateIndex

/**
 * @brief ZMasterFile::zcreateIndexFile overload calling zcreateIndexFile primary method
 *
 * This routine will create a new index with the files structures necessary to hold and manage it : a ZIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 * - rebuilds the index from current ZMasterFile's content
 *
 *@note
 * 1. ZIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZIndexFile file pathname is not stored but is a computed data from actual ZMasterFile file pathname.
 *
 * @param[in] pZIFField     Key fields dictionary to create the index with
 * @param[in] pIndexName    User name of the index key as a const char *
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zcreateIndex (ZKeyDictionary& pZIFField,
                           const char* pIndexName,
                           ZSort_Type pDuplicates,
                           bool pBackup)
{
    utfdescString wIndexName;
    wIndexName = pIndexName;
    return  zcreateIndex (pZIFField,
                          wIndexName,
                          pDuplicates,
                          pBackup);
} // zcreateIndex



#include <stdio.h>
/**
 * @brief ZMasterFile::zremoveIndex Destroys an Index definition and its files' content on storage
 *
 *  Removes an index.
 *
 *      renames all index files of rank greater than the current on to make them comply with ZIndexFile naming rules.
 *
 *
 * @param pIndexRank Index rank to remove from ZMasterFile
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zremoveIndex (const long pIndexRank)
{
ZStatus wSt;
zmode_type wMode = ZDescriptor.Mode;
uriString FormerIndexContent;
uriString FormerIndexHeader;
uriString NewIndexContent;
uriString NewIndexHeader;

int wRet;
    if ((getMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All))
            {
            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_MODEINVALID,
                                     Severity_Error,
                                     " ZMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                                     getURIContent().toString());
            return ZS_MODEINVALID;
            }
    if ((pIndexRank<0)||(pIndexRank>ZMCB.Index.size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Severe,
                                    " Out of indexes boundaries: rank <%ld>  boundaries [0,%ld] : File <%s>",
                                    pIndexRank,
                                    ZMCB.Index.lastIdx(),
                                    ZDescriptor.URIContent.toString());
            return ZS_OUTBOUND;
            }

    wSt= ZMCB.IndexObjects[pIndexRank]->zclose();  // close ZIndexFile files to delete
    if (wSt!=ZS_SUCCESS)
                    return wSt;

    wSt=ZMCB.IndexObjects[pIndexRank]->zremoveFile();  // remove the files
    if (wSt!=ZS_SUCCESS)
                    return wSt;

 //   ZMCB.removeICB(pIndexRank); // removes Index stuff IndexObjects suff and deletes appropriately

    ZMCB.Index.erase(pIndexRank);
    ZMCB.IndexObjects.erase(pIndexRank);
    ZMCB.IndexCount= ZMCB.Index.size();

    for (long wi = pIndexRank;wi<ZMCB.Index.size();wi++)
        {

        FormerIndexContent = ZMCB.IndexObjects[wi]->ZDescriptor.URIContent;
        FormerIndexHeader = ZMCB.IndexObjects[wi]->ZDescriptor.URIHeader;

        ZMCB.IndexObjects[wi]->zclose();// close index files before renaming its files

        wSt=generateIndexURI(getURIContent(),ZMCB.IndexFilePath,NewIndexContent,wi,ZMCB.Index[wi].Name);
        if (wSt!=ZS_SUCCESS)
                {
                return wSt;
                }
        wSt=generateURIHeader(NewIndexContent,NewIndexHeader);
        if (wSt!=ZS_SUCCESS)
                {
                return wSt;
                }
        ZMCB.IndexObjects[wi]->ZDescriptor.URIContent = NewIndexContent;
        ZMCB.IndexObjects[wi]->ZDescriptor.URIHeader = NewIndexHeader;

        wRet=rename(FormerIndexContent.toString(),NewIndexContent.toString());
        if (wRet)
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_FILEERROR,
                             Severity_Severe,
                             "Cannot rename index content file <%s> to <%s>",
                             FormerIndexContent.toString(),
                             NewIndexContent.toString());
        else
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_SUCCESS,
                                    Severity_Information,
                                    "Index content file <%s> has been renamed to <%s>",
                                    FormerIndexContent.toString(),
                                    NewIndexContent.toString());
            if (ZVerbose)
                    ZException.printLastUserMessage(stderr);
        }

        wRet=rename(FormerIndexHeader.toString(),NewIndexHeader.toString());
        if (wRet)
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_FILEERROR,
                             Severity_Severe,
                             "Cannot rename index header file <%s> to <%s>",
                             FormerIndexHeader.toString(),
                             NewIndexHeader.toString());
        else
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_SUCCESS,
                                    Severity_Information,
                                    "Index content file <%s> has been renamed to <%s>",
                                    FormerIndexHeader.toString(),
                                    NewIndexHeader.toString());
            if (ZVerbose)
                    ZException.printLastUserMessage(stderr);
        }
        ZMCB.IndexObjects[wi]->openIndexFile( NewIndexContent,wMode);
        }// for

    ZDataBuffer wMCBContent;
    return  _Base::updateReservedBlock(ZMCB._exportMCB(wMCBContent),true);
}//zremoveIndex

//---------------------------------Utilities-----------------------------------------------------

/**  * @addtogroup ZMFUtilities
 * @{ */

/**
 * @brief ZMasterFile::zclearMCB Sets the current ZMasterFile's ZMCB to its minimum value. Destroys all existing indexes, and their related files.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.upgradelog
 * @return
 */
ZStatus
ZMasterFile::zclearMCB (FILE* pOutput)
{
ZStatus wSt;
ZDataBuffer wMCBContent;

FILE* wOutput=nullptr;
bool FOutput=false;
descString wBase;


   wOutput=pOutput;
   if (pOutput==nullptr)
       {
       wBase=ZDescriptor.URIContent.getBasename();
       wBase+=".downgradelog";
       wOutput=fopen(wBase.toString(),"w");
       if (wOutput==nullptr)
                   {
           wOutput=stdout;
           fprintf(wOutput,
                   "%s>>  cannot open file <%s> redirected to stdout\n",
                   _GET_FUNCTION_NAME_,
                   wBase.toString());
                   }
       else
           {
           FOutput=true;
           }
       } // if nullptr

    if ((getMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All))
            {
            fprintf(wOutput,
                    "%s>>  ZMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                     _GET_FUNCTION_NAME_,
                    getURIContent().toString());

            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_MODEINVALID,
                                     Severity_Error,
                                     " ZMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                                     ZDescriptor.URIContent.toString());
            return ZS_MODEINVALID;
            }


    if (ZVerbose)
        {
        fprintf (wOutput,
                 "%s>>      Clearing ZMasterControlBlock of file <%s>\n"
                 "              Actual content\n",
                 _GET_FUNCTION_NAME_,
                 ZDescriptor.URIContent.toString());
        ZMCB.report(pOutput);
        }

    while (ZMCB.Index.size()>0)
            {
            wSt=zremoveIndex(ZMCB.Index.lastIdx());
            if (wSt!=ZS_SUCCESS)
                {
                if (ZVerbose)
                    {
                    fprintf (wOutput,
                             "%s>> ****Error: removing index rank <%ld> status <%s> clearing ZMasterControlBlock of file <%s>\n"
                             "              Actual content\n",
                             _GET_FUNCTION_NAME_,
                             ZMCB.IndexObjects.lastIdx(),
                             decode_ZStatus(wSt),
                             ZDescriptor.URIContent.toString());
                    }
                ZException.addToLast(" Index rank <%ld>. Clearing ZMasterControlBlock of file <%s>.",
                                       ZMCB.IndexObjects.lastIdx(),
                                       ZDescriptor.URIContent.toString());
                return wSt;
                }// not ZS_SUCCESS
            if (ZVerbose)
                {
                fprintf (pOutput,
                         "%s>>      Index successfully removed\n",
                         _GET_FUNCTION_NAME_);
                ZMCB.report(wOutput);
                }
            }//while

    if (FOutput)
            fclose(wOutput);
    ZMCB.clear();
    return  _Base::updateReservedBlock(ZMCB._exportMCB(wMCBContent),true);
}//zclearMCB

/**
 * @brief ZMasterFile::zdowngradeZMFtoZRF downgrades a ZMasterFile structure to a ZRandomFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * Former dependant index files are left to themselves and not destroyed by this method.
 *
 * @param[in] pZMFPath      file path of the ZMasterFile to convert
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.downgradelog
 */
void
ZMasterFile::zdowngradeZMFtoZRF (const char* pZMFPath,FILE* pOutput)
{
ZStatus wSt;
ZMasterFile wMasterFile;
ZRandomFile wMasterZRF;

uriString   wURIContent;

ZDataBuffer wReservedBlock;

FILE* wOutput=nullptr;
bool FOutput=false;
descString wBase;

   wURIContent = pZMFPath;

   wOutput=pOutput;
   if (pOutput==nullptr)
       {
       wBase=wURIContent.getBasename();
       wBase+=".downgradelog";
       wOutput=fopen(wBase.toString(),"w");
       if (wOutput==nullptr)
                   {
           wOutput=stdout;
           fprintf(wOutput,
                   "%s>>  cannot open file <%s> redirected to stdout\n",
                   _GET_FUNCTION_NAME_,
                   wBase.toString());
                   }
       else
           {
           FOutput=true;
           }
       } // if nullptr

   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   fprintf (wOutput,"%s>> starting downgrading ZMasterFile to ZRandomFile file path <%s>  \n",
            _GET_FUNCTION_NAME_,
            pZMFPath);


    wSt=wMasterFile.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));

    fprintf (wOutput,"%s>> getting ZReservedBlock content and load ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_);


   fprintf (wOutput,
            "%s>> clearing ZMCB\n"
            "            <%ld>  defined index(es) in ZMasterControlBlock. Destroying all index files & definitions from ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_,
            wMasterFile.ZMCB.Index.size());

    wSt=wMasterFile.zclearMCB(wOutput);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }
    fprintf (wOutput,"%s>>  ZMCB cleared successfully\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();

    fprintf (wOutput,"%s>>  Converting to ZRandomFile\n",
            _GET_FUNCTION_NAME_);
    wSt=wMasterZRF.setPath(wURIContent);
    if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }

   wSt=wMasterZRF._open (wMasterZRF.ZDescriptor,ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile);  // open ZMF using ZRandomFile routines
   if (wSt!=ZS_SUCCESS)
          {
          ZException.exit_abort();
          }

    wMasterZRF.ZDescriptor.ZHeader.FileType = ZFT_ZRandomFile;
    wReservedBlock.clear();
    wMasterZRF.setReservedContent(wReservedBlock);

    fprintf (wOutput,"%s>>  Writing new header to file\n",
            _GET_FUNCTION_NAME_);
    wSt=wMasterZRF._writeFullFileHeader(wMasterZRF.ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }
   wMasterZRF.zclose();
   fprintf (wOutput,"%s>>  File <%s> has been successfully converted from ZMasterFile to ZRandomFile\n",
           _GET_FUNCTION_NAME_,
            wURIContent.toString());
   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   ZException.printUserMessage(wOutput);
   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
//   if (wSt==ZS_SUCCESS)
           ZException.clearStack();
   if (FOutput)
          fclose(wOutput);
   return;
}//zdowngradeZMFtoZRF

/**
 * @brief ZMasterFile::zupgradeZRFtoZMF upgrades a ZRandomFile structure to an empty ZMasterFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * @param pZRFPath file path of the ZRandomFile to upgrade
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.upgradelog
 */
void
ZMasterFile::zupgradeZRFtoZMF (const char* pZRFPath,FILE* pOutput)
{
ZStatus wSt;
ZMasterFile wMasterFile;
ZRandomFile wMasterZRF;

uriString   wURIContent;

ZDataBuffer wReservedBlock;

FILE* wOutput=nullptr;
bool FOutput=false;
descString wBase;

   wURIContent = pZRFPath;

   wOutput=pOutput;
   if (pOutput==nullptr)
       {
       wBase=wURIContent.getBasename();
       wBase+=".upgradelog";
       wOutput=fopen(wBase.toString(),"w");
       if (wOutput==nullptr)
                   {
           wOutput=stdout;
           fprintf(wOutput,
                   "%s>>  cannot open file <%s> redirected to stdout\n",
                   _GET_FUNCTION_NAME_,
                   wBase.toString());
                   }
       else
           {
           FOutput=true;
           }
       } // if nullptr

   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   fprintf (wOutput,"%s>> starting upgrading ZRandomFile to ZMasterFile file path <%s>  \n",
            _GET_FUNCTION_NAME_,
            pZRFPath);


    wSt=wMasterZRF.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }
    fprintf (wOutput,"%s>> creating ZReservedBlock content and write ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_);

    wSt=wMasterFile.setPath(wURIContent);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }
    wMasterFile.ZMCB._exportMCB(wReservedBlock);

    wMasterZRF.setReservedContent(wReservedBlock);
    wMasterZRF.ZDescriptor.ZHeader.FileType = ZFT_ZMasterFile;
    wSt=wMasterZRF._writeFullFileHeader(wMasterZRF.ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }

   wMasterZRF.zclose();
   fprintf (wOutput,"%s>>  File <%s> has been successfully converted from ZRandomFile to ZMasterFile.\n",
           _GET_FUNCTION_NAME_,
            wURIContent.toString());
   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   ZException.printUserMessage(wOutput);
   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
 //  if (wSt==ZS_SUCCESS)
           ZException.clearStack();
   if (FOutput)
           fclose(wOutput);
   return;
}//zupgradeZRFtoZMF


/**
 *
 * @brief ZMasterFile::zrepairIndexes Scans and Repairs indexes of a ZMasterFile
 *
 *  zrepairIndexes unlock the file in case it has been left open and locked.
 *
 *  For any defined index of the given ZMasterFile, it tests index file presence and controls ZIndexControlBlock integrity.
 *
 *  It repairs damaged indexes if pRepair option is set to true (and rebuilds them).
 *
 *  It rebuilds all indexes if pRebuildAll is set to true
 *
 *  This routine may be used to test wether a file structure is still correct.
 *
 *  In addition, it may be used to regularly rebuild and reorder indexes if necessary.
 *
 * @warning Be sure to use zrepairIndexes in a standalone context, as it opens the file regardless the possible locks.
 *
 * @param[in] pZMFPath      a const char* with the ZMasterFile main content file path.
 * @param[in] pRepair       if set this option will try to repair/ rebuilt damaged or missing indexes. If not a report is emitted.
 * @param[in] pRebuildAll   Option if set (true) then all indexes are rebuilt, even if they are healthy indexes.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.repairlog
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zrepairIndexes (const char *pZMFPath,
                             bool pRepair,
                             bool pRebuildAll,
                             FILE* pOutput)
{
ZStatus wSt;
ZMasterFile wMasterFile;
ZRandomFile wMasterZRF;

ZIndexControlBlock wZICB;
ZIndexFile  wIndexFile(&wMasterZRF);
ZRandomFile wIndexZRF;
uriString   wURIContent;
uriString   wIndexUri;
ZDataBuffer wReservedBlock;
ssize_t     wImportSize;
zsize_type wIndexAllocatedSize;
ZDataBuffer wICBContent;

long wi=0, IndexRank=0;

FILE* wOutput=nullptr;
bool FOutput=false;
descString wBase;

ZArray<char> IndexPresence ; // 0 : Index to be deleted     1 : Index present but not to be rebuilt    2 : Index to be built or rebuilt

long wIndexProcessed = 0,wMissIndexFile = 0, wCorruptZICB = 0, wCreatedIndex = 0, wRebuiltIndex = 0 ;


   wURIContent = pZMFPath;

   wOutput=pOutput;
   if (pOutput==nullptr)
       {
       wBase=wURIContent.getBasename();
       wBase+=".repairlog";
       wOutput=fopen(wBase.toString(),"w");
       if (wOutput==nullptr)
                   {
           wOutput=stdout;
           fprintf(wOutput,
                   "%s>>  cannot open file <%s> redirected to stdout\n",
                   _GET_FUNCTION_NAME_,
                   wBase.toString());
                   }
       else
           {
           FOutput=true;
           }
       } // if nullptr

   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   fprintf (wOutput,"%s>> starting repairing indexes for ZMasterFile <%s>  \n"
                    ,
            _GET_FUNCTION_NAME_,
            pZMFPath);

   
   wSt=wMasterZRF.setPath(wURIContent);
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }
   wSt=wMasterZRF._open (wMasterZRF.ZDescriptor,ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile,true);  // open ZMF using ZRandomFile routines
   if (wSt!=ZS_SUCCESS)
           {
            goto ErrorRepairIndexes;
           }
   //-------------Must be a ZFT_ZMasterFile----------------

   if (wMasterZRF.ZDescriptor.ZHeader.FileType!=ZFT_ZMasterFile)
           {

           fprintf (wOutput,"%s>> **** Fatal error : file is not of mandatory type ZFT_ZMasterFile but is <%s> ******\n",
                    _GET_FUNCTION_NAME_,
                    decode_ZFile_type( wMasterZRF.ZDescriptor.ZHeader.FileType));
           goto ErrorRepairIndexes;
           }

   //wMasterFile.ZMFURI = wMasterZRF.getURIContent();     // align uris: ZMFURI is getting redundant. Only using ZRandomFile URIContent

   fprintf (wOutput,"%s>> getting ZReservedBlock content and load ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_);

   wSt=wMasterZRF.getReservedBlock(wReservedBlock,true);     // get reserved block content
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }
   wSt=wMasterFile.ZMCB._importMCB(wReservedBlock);     // load ZMCB from reserved block content
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }

   fprintf (wOutput,
            "%s>> existing ZMasterFile index(es)\n"
            "            <%ld>  defined index(es) in ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_,
            wMasterFile.ZMCB.Index.size());

   wMasterFile.ZMCBreport();

/*
 *  for each index
*/
   IndexPresence.allocateCurrentElements(wMasterFile.ZMCB.Index.size());
   IndexPresence.bzero();
   IndexRank=0;
   for (IndexRank=0;IndexRank<wMasterFile.ZMCB.Index.size();IndexRank++)
   {
       wSt=generateIndexURI(wMasterFile.getURIContent(),
                            wMasterFile.ZMCB.IndexFilePath,
                            wIndexUri,
                            IndexRank,
                            wMasterFile.ZMCB.Index[IndexRank].Name);
       if (wSt!=ZS_SUCCESS)
               {
               return wSt;
               }
       fprintf (wOutput,
                "%s>>   .....rank <%ld> processing index file <%s> \n",
                _GET_FUNCTION_NAME_,
                IndexRank,
                wIndexUri.toString());
        if (wIndexUri.exists())
                fprintf(wOutput,

                        "%s>>       Index file has been found\n",
                        _GET_FUNCTION_NAME_);
            else
            {
            fprintf(wOutput,
                    "\n%s>>  ****Error Index file <%s> is missing ****\n"
                    "                        Index will be created then rebuilt\n\n",
                    _GET_FUNCTION_NAME_,
                    wIndexUri.toString());

            wMissIndexFile++;

            IndexPresence[IndexRank]= 1;
            if (!pRepair)
                        continue;
            //----------------Create a new ZIndexFile-------------------------


            fprintf (wOutput,
                     "%s>> creating index file\n",
                     _GET_FUNCTION_NAME_);

            wIndexAllocatedSize=0;
            if (wMasterZRF.getBlockTargetSize()>0)
                        if (wMasterZRF.getAllocatedBlocks()>0)
                            wIndexAllocatedSize =  wMasterZRF.getAllocatedBlocks() * wMasterFile.ZMCB.Index[wi].IndexRecordSize();

            wSt =  wIndexFile.zcreateIndex(&wMasterFile.ZMCB.Index[wi],  // pointer to index control block because ZIndexFile stores pointer to Father's ICB
                                              wIndexUri,
                                              wMasterZRF.getAllocatedBlocks(),
                                              wMasterZRF.getBlockExtentQuota(),
                                              wIndexAllocatedSize,
                                              wMasterZRF.getHighwaterMarking(),
                                              false,        // grabfreespace is set to false : not necessary for an index
                                              true,         // replace existing file
                                              false         // do not leave it open
                                              );
            if (wSt!=ZS_SUCCESS)
                {
                goto ErrorRepairIndexes;
                }
            fprintf (wOutput,
                     "%s>> index file has been created\n",
                     _GET_FUNCTION_NAME_);
            wCreatedIndex++;

            fprintf (wOutput,
                     "%s>> ......rebuilding created index file\n",
                     _GET_FUNCTION_NAME_);

            wSt = wIndexFile.openIndexFile(wIndexUri,ZRF_Exclusive| ZRF_All);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>  ****Error: Unexpected Fatal Error while opening ZIndexFile index rank <%ld> path <%s> ****\n",
                         _GET_FUNCTION_NAME_,
                         IndexRank,
                         wIndexUri.toString());
                 wIndexFile.zclose();
                 goto ErrorRepairIndexes;
                }
            wSt = wIndexFile.zrebuildIndex(ZMFStatistics,wOutput);
            if (wSt!=ZS_SUCCESS)
                    {
                    fprintf (wOutput,
                             "%s>>  ****Unexpected Fatal Error while rebuilding ZIndexFile index rank <%ld> path <%s> *******\n",
                             _GET_FUNCTION_NAME_,
                             IndexRank,
                             wIndexUri.toString());
                    wIndexFile.zclose();
                    goto ErrorRepairIndexes;
                    } // ! ZS_SUCCESS

            wIndexFile.zclose();
            fprintf (wOutput,
                     "%s>>  Index file has been rebuilt successfully\n",
                     _GET_FUNCTION_NAME_);

            IndexPresence[IndexRank] = 1; // Index file is now present and does not need to be rebuilt
            wRebuiltIndex ++;

            //----------------End Create a new ZIndexFile-------------------------
            continue;
            }  // IndexUri does not exist

//---------- wIndexUri exists-----------------
        fprintf (wOutput,"%s>>  Opening ZIndexFile\n",
                 _GET_FUNCTION_NAME_);


        wSt=wIndexZRF.setPath(wIndexUri);
        if (wSt!=ZS_SUCCESS)
                {
                goto ErrorRepairIndexes;
                }
        wSt=wIndexZRF._open(wIndexZRF.ZDescriptor,ZRF_Exclusive | ZRF_All,ZFT_ZIndexFile,true);
        if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                     "%s>>   ******Error: rank <%ld> cannot open index file <%s>.\n"
                     "                  Status is <%s> Check <IndexFileDirectoryPath> parameter\n"
                      "       ...continuing...\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     decode_ZStatus(wSt),
                     wIndexUri.toString());
                if (pRepair)
                        {
                        fprintf (wOutput,
                        "                  Repare option has been chosen "
                        "                  Trying to delete file and reprocess it as missing file.\n");

                        wIndexZRF.zremoveFile(); // may be not necessary : to be checked

                        IndexRank--;
                        continue;
                        }
                }

        wReservedBlock.clear();

        fprintf (wOutput,"%s>>  getting ZReservedBlock content and load ZICB\n",
                 _GET_FUNCTION_NAME_);

        wSt=wIndexZRF.getReservedBlock(wReservedBlock,true);     // get reserved block content
        if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>   ******Error: Index file rank <%ld> file  <%s>.\n"
                         "                  Status is <%s> Cannot get ZReservedBlock from header file. \n"
                         "       ...continuing...\n",
                         _GET_FUNCTION_NAME_,

                         IndexRank,
                         wIndexUri.toString(),
                         decode_ZStatus(wSt));

                if (pRepair)
                        {
                        fprintf (wOutput,
                        "                  Repare option has been chosen \n"
                        "                  Trying to delete file and reprocess it as missing file.\n");

                        wIndexZRF.zremoveFile(); // may be not necessary : to be checked

                        IndexRank--;
                        continue;
                        }
                }
        wSt=wZICB._importICB(wReservedBlock,wImportSize,0L);  // load ZICB from reserved block content
        if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>   ******Error: Index file rank <%ld> file  <%s>.\n"
                         "                  Status is <%s> Cannot import ZIndexControlBlock \n"
                         "       ...continuing...\n",
                         _GET_FUNCTION_NAME_,
                         IndexRank,
                         wIndexUri.toString(),
                         decode_ZStatus(wSt));

                if (pRepair)
                        {
                        fprintf (wOutput,
                        "                  Repare option has been chosen \n"
                        "                  Trying to delete file and reprocess it as missing file.\n");

                        wIndexZRF.zremoveFile(); // may be not necessary : to be checked

                        IndexRank--;
                        continue;
                        }
                }
        fprintf (wOutput,"%s>>  checking ZICB content alignment with ZMasterFile\n",
                 _GET_FUNCTION_NAME_);

        if (memcmp(wReservedBlock.Data,wMasterFile.ZMCB.Index[IndexRank]._exportICB(wICBContent).Data,wReservedBlock.Size)==0)
            {
            fprintf (wOutput,
                     "%s>>  ZICB content is aligned with its ZMasterFile for index rank <%ld> path <%s>\n"
                     "             To rebuild ZIndexFile content (zrebuid) use Option rebuildAll\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     wIndexUri.toString());

            if (pRebuildAll)
                {
                fprintf (wOutput,
                         "%s>>  Option <RebuildAll> : healthy index file is marked for rebuilt\n",
                         _GET_FUNCTION_NAME_);

                IndexPresence[IndexRank]=2;
                }

            }
            else
            {
            fprintf (wOutput,
                     "%s>>  ****Error: ZICB content is NOT aligned with its ZMasterFile index rank <%ld> path <%s>\n"
                     "             Need to be realigned and rebuilt rebuildAll\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     wIndexUri.toString());

            wCorruptZICB ++;

            IndexPresence[IndexRank]= 2;  // need to be either realigned (ZICB) and rebuilt OR destroyed . recreated and rebuilt.

            //--------------------------------------------------------------------------

            if(pRepair)
            {
                fprintf (wOutput,
                         "%s>> removing corrupted index file\n",
                         _GET_FUNCTION_NAME_);

                 wIndexZRF.zremoveFile();
                 fprintf (wOutput,
                          "%s>> reprocessing index as missing index file\n",
                          _GET_FUNCTION_NAME_);
                 IndexRank --;
                 continue;

            }// if pRepare

            //----------------------------------------------------------------------


            }// else


            wIndexZRF.zclose();
   }// main for loop

   wIndexProcessed= IndexRank;
//----------------Post processing check index files to be rebuilt-------------------------

   wMasterZRF._close(wMasterZRF.ZDescriptor);

   wMasterFile.zopen(ZRF_Exclusive | ZRF_All);

    fprintf (wOutput,"%s>>   Rebuilding indexes to be rebuilt \n",
             _GET_FUNCTION_NAME_);

    for (wi=0;wi<IndexPresence.size();wi ++)
        {
        if (IndexPresence[wi]==2)
                {
                fprintf (wOutput,"%s>>   Rebuilding index rank <%ld> <%s> \n",
                         _GET_FUNCTION_NAME_,
                         wi,
                         wMasterFile.ZMCB.Index[wi].Name.toString());

                wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput);
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf (wOutput,
                             "%s>>   ****Error while rebuilding index rank <%ld> <%s> \n"
                             "          Status is <%s>\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             wMasterFile.ZMCB.Index[wi].Name.toString(),
                             decode_ZStatus(wSt));
                    goto ErrorRepairIndexes;
                    }
                fprintf (wOutput,"      rebuilt done\n");
                wRebuiltIndex ++;
                }// if IndexPresence == 2
        }// for

EndRepairIndexes:

   fprintf (wOutput,"%s>>  Closing ZMasterFile\n",
            _GET_FUNCTION_NAME_);
   wMasterZRF.zclose();

   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");

    fprintf (wOutput,
             "%s>>  Report\n"
             "          Index(es) processed         %ld\n"
             "          Index file(s) missing       %ld\n"
             "          Index ZICB corrupted        %ld\n\n"
             "          Index(es) rebuilt           %ld\n",
             _GET_FUNCTION_NAME_,
             wIndexProcessed,
             wMissIndexFile,
             wCorruptZICB,
             wRebuiltIndex);
    fprintf (wOutput,
             "_____________________________________________________________________________________________\n");
    ZException.printUserMessage(wOutput);
    fprintf (wOutput,
             "_____________________________________________________________________________________________\n");
    if (wSt==ZS_SUCCESS)
            ZException.clearStack();
    if (FOutput)
            fclose(wOutput);
    return wSt;

ErrorRepairIndexes:
    fprintf (wOutput,"%s>>  **** Index repair ended with error ***\n",
             _GET_FUNCTION_NAME_);
    goto EndRepairIndexes;
}//zrepairIndexes

/** @ */ // ZMFUtilities


/**
 * @brief ZMasterFile::zcreate  ZMasterFile creation with a full definition with a file path that will name main content file.
 * Other file names will be deduced from this name.
 * @note At this stage, no indexes are created for ZMasterFile.
 *
 * Main file content and file header are created with appropriate parameters as given in parameters.
 * ZMasterFile infradata structure is created within header file.
 *
 * @param[in] pURI  uriString containing the path of the future ZMasterFile main content file.
 *          Other file names will be deduced from this main name.
 * @param[in] pAllocatedBlocks  number of initial elements in ZBAT pool and other pools(pInitialAlloc) see: @ref ZArrayParameters
 * @param[in] pBlockExtentQuota extension quota for pools (pReallocQuota) see: @ref ZArrayParameters
 * @param[in] pBlockTargetSize  approximation of best record size. see: @ref ZRFBlockTargetSize
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pHistory          RFFU History option true : on ; false : off
 * @param[in] pAutocommit       RFFU Autocommit option true : on ; false : off
 * @param[in] pJournaling       RFFU Journaling option true : on ; false : off
 * @param[in] pHighwaterMarking HighWaterMarking option true : on ; false : off see: @ref ZRFHighWaterMarking
 * @param[in] pGrabFreeSpace    GrabFreespace option true : on ; false : off see: @ref ZRFGrabFreeSpace
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zcreate(const uriString pURI,
                     long pAllocatedBlocks,
                     long pBlockExtentQuota,
                     long pBlockTargetSize,
                     const zsize_type pInitialSize,
                     bool pHighwaterMarking,
                     bool pGrabFreeSpace,
                     bool pJournaling,
                     bool pBackup,
                     bool pLeaveOpen)
{
ZStatus wSt;
ZDataBuffer wMCBContent;
//    ZMFURI=pURI;
    wSt=_Base::setPath(pURI);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return wSt;
            }

    _Base::setCreateMaximum (pAllocatedBlocks,
                             pBlockExtentQuota,
                             pBlockTargetSize,
                             pInitialSize,
                             pHighwaterMarking,
                             pGrabFreeSpace);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return wSt;
            }
    wSt=_Base::_create(ZDescriptor,pInitialSize,ZFT_ZMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return wSt;
            }
/*    wSt=_Base::zopen(ZRF_Write);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     ZMFURI.toString());
            return wSt;
            }*/
    ZDescriptor.ZHeader.FileType = ZFT_ZMasterFile;     // setting ZFile_type



    _Base::setReservedContent(ZMCB._exportMCB(wMCBContent));
//    wSt=_Base::updateReservedBlock(ZMCB._exportMCB());
    wSt=_Base::_writeFullFileHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
            {
            _Base::_close(ZDescriptor);
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return wSt;
            }

// Manage journaling for the being created file
// - set option to MCB
// - if journaling enabled : create journaling file

//    ZMCB.JournalingOn = pJournaling; // update journaling MCB option for the file
    if (pJournaling)
        {
        ZMCB.ZJCB->Journal=new ZJournal(this);
        wSt=ZMCB.ZJCB->Journal->createFile();
        if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" while creating ZMasterFile %s",
                                       getURIContent().toString());
                return wSt;
                }
        }

    if (pLeaveOpen)
                return wSt;

    zclose();
    return wSt;
}// zcreate

/**
 * @brief ZMasterFile::zcreate Creates the raw content file and its header as a ZRandomFile with a structure capable of creating indexes.
 * @param[in] pURI  uriString containing the path of the future ZMasterFile main content file. Other file names will be deduced from this main name.
 * @param[in] pInitialSize Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pBackup    If set to true file will be replaced if it already exists. If false (default value), existing file will be renamed according renaming rules.
 * @param[in] pLeaveOpen   If set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zcreate (const uriString pURI, const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{
ZStatus wSt;
ZDataBuffer wMCBContent;
//    ZMFURI=pURI;
    wSt=_Base::setPath (pURI);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    _Base::setCreateMinimum(pInitialSize);
    wSt=_Base::_create(ZDescriptor,pInitialSize,ZFT_ZMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return wSt;
            }
    /* ----------File is left open
    wSt=_Base::_open(ZDescriptor,ZRF_Exclusive | ZRF_Write,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     ZMFURI.toString());
            return wSt;
            }
            */
    ZDescriptor.ZHeader.FileType = ZFT_ZMasterFile;     // setting ZFile_type (Already done in _create routine)
    _Base::setReservedContent(ZMCB._exportMCB(wMCBContent));
//    wSt=_Base::updateReservedBlock(ZMCB._exportMCB());
    wSt=_Base::_writeFullFileHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return wSt;
            }
    if (pLeaveOpen)
                return wSt;

    zclose();
    return wSt;
}//zcreate

/**
 * @brief ZMasterFile::zcreate Creates the raw content file and its header as a ZRandomFile with a structure capable of creating indexes.
 *  @note if a file of the same name already exists (either content file or header file)
 *        then content and header file will be renamed to  <base file name>.<extension>_bck<nn>
 *        where <nn> is a version number
 *
 * @param[in] pPathHame  a C string (const char*) containing the path of the future ZMasterFile main content file. Other file names will be deduced from this main name.
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zcreate (const char* pPathName, const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{
uriString wURI(pPathName);
//    ZMFURI=pPathName;
    return (zcreate(wURI,pInitialSize,pBackup,pLeaveOpen));
}//zcreate


//----------------End zcreate--------------------

void
ZMasterFile::_defineKeyFieldsList (ZIndexControlBlock pZICB,long pCount,...)
{
int wCount = pCount;
va_list args;
va_start (args, pCount);

pZICB.ZKDic->clear();
ZIndexField wKeyField;
while(wCount>0)
  {
    wKeyField.RecordOffset = va_arg(args , long );
    wCount--;
    if (wCount==0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP ,
                                    Severity_Fatal,
                                    "Invalid number of arguments in index key fields definition \n");
            ZException.exit_abort();
            }

    wKeyField.NaturalSize = va_arg(args, long );
    wCount--;
    wKeyField.Name = va_arg(args,ZFieldName_type);
    wCount--;
    pZICB.ZKDic->push(wKeyField);
    pZICB.ZKDic->_reComputeSize();

  }

    va_end (args);
return;
}//_defineKeyFieldsList


/**
 * @brief ZMasterFile::createZKeyByName Creates a ZKey objects for the index corresponding to given user index key name.
 *
 * @note If index name is not recognized within ZMasterFile's ZIndexControlBlock, routine aborts.
 *
 * @param[in] pKeyName User given name of the index to create the ZKey for
 * @return a pointer to a ZKey structure. ZKey object is instantiated by 'new' instruction and must be deleted by calling procedure.
 *
 */
ZKey*
ZMasterFile::createZKeyByName (const char* pKeyName)
    {
    long wi=0;
    for (;wi<ZMCB.Index.size();wi++)
                    if (ZMCB.Index[wi].Name == pKeyName)
                                    break;

    if (wi==this->ZMCB.Index.size())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVOP,
                                        Severity_Fatal,
                                        " Invalid key name while creating Key structure. given name is %s", pKeyName);
                ZException.exit_abort();
                }
        return (createZKey(wi));
    }
/**
 * @brief ZMasterFile::createZKey Creates a ZKey objects for the index corresponding to given index key rank.
 *
 * @note If index rank is out of ZMasterFile's ZIndexControlBlock boundaries, routine aborts.
 *
 * @param[in] pKeyNumber index key ZICB rank to create the ZKey for
 * @return a pointer to a ZKey structure. ZKey object is instantiated by 'new' instruction and must be deleted by calling procedure.

 */
ZKey*
ZMasterFile::createZKey (const long pKeyNumber)
    {
    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " File must be open to created a ZKey object from file <%s>",
                                ZDescriptor.URIContent.isEmpty()?"Unknown":ZDescriptor.URIContent.toString());
        return nullptr;
        }
    if (pKeyNumber> this->ZMCB.Index.size())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Invalid key number while creating Key structure. given value is %ld", pKeyNumber);
        return nullptr;
        }
        ZKey* wKey = new ZKey(&ZMCB.Index[pKeyNumber],pKeyNumber);
        return (wKey);
    }


/**
 * @brief ZMasterFile::zprintIndexFieldValues prints the whole key description and content for ZIndex pIndex and for its rank pIdxRank
 *
 *  pIdxRank is then the key value relative position within the index (as a ZRandomFile)
 *  pIndex is the index number within ZMCB
 *
 * @see ZIndexFile::zprintKeyFieldsValues()
 *
 * @param[in] pIndex    Number of the index for the ZMasterFile
 * @param[in] pIdxRank  Logical rank of key record within ZIndexFile
  * @param[in] pHeader  if set to true then key fields description is printed. False means only values are printed.
  * @param[in] pKeyDump if set to true then index key record content is dumped after the list of its fields values. False means only values are printed.
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zprintIndexFieldValues (const long pIndex,const long pIdxRank,bool pHeader,bool pKeyDump,FILE *pOutput)
{
    if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.lastIdx()))
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_INVOP,
                                            Severity_Error,
                                            " invalid index number %ld. Number of ZIndexes is %ld",
                                            ZMCB.IndexObjects.lastIdx());
                    return ZS_INVOP;
                    }
    return (ZMCB.IndexObjects[pIndex]->zprintKeyFieldsValues(pIdxRank,pHeader,pKeyDump,pOutput));
} //zprintIndexFieldValues

/**
 * @brief ZMasterFile::zopen opens the ZMasterFile and all its associated ZIndexFiles with a mode set to defined mask pMode
 * @param pURI  MasterFile file path to open
 * @param pMode Open mode (mask)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
ZMasterFile::zopen  (const uriString pURI, const int pMode)
{
ZStatus wSt;
ZDataBuffer wRawMCB;

    wSt=_Base::setPath(pURI);
    wSt=_Base::_open (ZDescriptor,pMode,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
            {
            return wSt;
            }

//    ZMFURI = pURI;

    wSt=_Base::getReservedBlock(wRawMCB,true);
    if (wSt!=ZS_SUCCESS)
            {
            return wSt;
            }

    wSt=ZMCB._importMCB(wRawMCB);
    if (wSt!=ZS_SUCCESS)
                    return wSt;

// MCB is loaded
//----------Journaling----------------------

//    if (ZMCB.JournalingOn)
    if (ZMCB.JCBSize>0)     // if journaling requested
        {
        if (ZMCB.ZJCB->Journal==nullptr) // if no journaling : create one
                {
                ZMCB.ZJCB->Journal=new ZJournal(this);
                wSt=ZMCB.ZJCB->Journal->init();
                if (wSt!=ZS_SUCCESS)
                        return wSt;
                ZMCB.ZJCB->Journal->start();
                }
            else
                {
                if (!ZMCB.ZJCB->Journal->isOpen()) // if journal file is closed then need to re-open and restart
                        {
                        if (ZMCB.ZJCB->Journal->JThread.Created)  // journal file is not open but journaling thread is active
                                {
                                ZMCB.ZJCB->Journal->JThread.kill();
                                }
                        }
                }
        ZDataBuffer wJCBContent;

        wJCBContent.setData(ZDescriptor.ZReserved.Data+ZMCB.JCBOffset,ZMCB.JCBSize);
        ZMCB.ZJCB->_importJCB(wJCBContent);
        ZMCB.ZJCB->Journal->init();
        ZMCB.ZJCB->Journal->start();
        } // ZMCB.JournalingOn
        else // no journaling requested
        {
           if (ZMCB.ZJCB->Journal!=nullptr)
                        delete ZMCB.ZJCB->Journal;
        }// else
//--------------End journaling----------------------------

//     Need to create ZIndexFile object instances and open corresponding ZIndexFiles for each ZMCB.Index list rank
//
    ZMCB.IndexObjects.clear();
    uriString wIndexUri;
long wi;

    for (wi=0;wi < ZMCB.Index.size();wi++)
            {
            ZIndexFile* wIndex = new ZIndexFile (this,&ZMCB.Index[wi]);
            ZMCB.IndexObjects.push(wIndex);
            wIndexUri.clear();

            wSt=generateIndexURI(getURIContent(),ZMCB.IndexFilePath,wIndexUri,wi,ZMCB.Index[wi].Name);
            if (wSt!=ZS_SUCCESS)
                    {
                    return wSt;
                    }
            wSt=wIndex->openIndexFile(wIndexUri,pMode);
            if (wSt!=ZS_SUCCESS)
                    {
                    ZException.addToLast(" while opening index rank <%ld>", wi);
// In case of open index error : close any already opened index file
//              Then close master content file before returning
                    ZStatus wSvSt = wSt;
                    long wj;
                    for (wj = 0;wj < wi;wj++)
                                    ZMCB.IndexObjects[wj]->zclose();  // use the base ZRandomFile zclose routine
                     _Base::_close(ZDescriptor);

                    return wSt;
                    }
            }// for


    return ZS_SUCCESS;
}// zopen

/**
 * @brief ZMasterFile::zclose close the ZMasterFile, and all dependent ZIndexFiles
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zclose(void)
{
ZStatus wSt;
ZStatus SavedSt=ZS_SUCCESS;
ZDataBuffer wMCBContent;

// closing index files

    if (!ZDescriptor._isOpen)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                " file is not open : cannot close it.");
        return ZS_INVOP;
    }
    for (long wi=0;wi < ZMCB.IndexObjects.size();wi++)
            {
            if ((wSt=ZMCB.IndexObjects[wi]->closeIndexFile())!=ZS_SUCCESS)
                {
                ZException.printUserMessage(stderr);  // error on close is not blocking because we need to close ALL files
                SavedSt = wSt;
                }
            }// for

// flush MCB to file

    wSt=_Base::updateReservedBlock(ZMCB._exportMCB(wMCBContent),true);// force to write

    if (wSt!=ZS_SUCCESS)
                {

                ZException.addToLast( " Writing Reserved header for ZMasterFile %s",
                                        getURIContent().toString());
                _Base::zclose();
                return wSt;
                }

// releasing index resources

    while (ZMCB.IndexObjects.size()>0)
                            ZMCB.IndexObjects.pop();

    while (ZMCB.Index.size()>0)
                            ZMCB.Index.pop();

// ending journaling process

//    setJournalingOff();  // nope

// closing main ZMF content file

    wSt=_Base::zclose();
    if (wSt!=ZS_SUCCESS)
            {
            return wSt;
            }
    if (SavedSt!=ZS_SUCCESS)
                return SavedSt;
    return ZS_SUCCESS;
}// zclose

//----------------Insert sequence------------------------------------------------------

/**
 * @brief ZMasterFile::zinsert Inserts a new record given by pRecord at position pZMFRank within Master File and updates all indexes

Add operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
If an error occur, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
If all indexes are successfully updated, then update on Master File is committed .

 if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically removed from index file.

* @param[in] pRecord    user record content to insert in a ZDataBuffer
* @param[in] pZMFRank   relative position within the ZMasterFile to insert the record (@see ZRandomFile
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError


 */
ZStatus
ZMasterFile::zinsert       (ZDataBuffer& pRecord, const long pZMFRank)
{
ZStatus wSt;

long            wZMFIdxCommit;
zaddress_type   wZMFAddress;

ZArray <long>       IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZIndexItemList      IndexItemList;          // stores keys description per index processed

ZIndexItem           *wIndexItem;
long wIndex_Rank;
long wi = 0;

//    ZExceptionStack.clear();
//    ZException.getLastStatus() = ZS_SUCCESS;

    wSt=_Base::_insert2PhasesCommit_Prepare(_Base::ZDescriptor,pRecord,pZMFRank,wZMFIdxCommit,wZMFAddress);  //! prepare the add on Master File, reserve appropriate space, get entry in pool
    if (wSt!=ZS_SUCCESS)
            {
            goto zinsert_error;
            }
//
// update all Indexes
//

    for (wi=0;wi< ZMCB.IndexObjects.size();wi++)
    {
        if (ZVerbose)
        {
            fprintf (stdout,"Index number <%ld>\n",wi);
        }
        wIndexItem = new ZIndexItem;
        wSt=ZMCB.IndexObjects[wi]->_addKeyValue_Prepare(pRecord,wIndexItem,wIndex_Rank, wZMFAddress);// for indexes don't care about insert, this is an add key value
        if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast("During zinsert operation on index number <%ld>",wi);
                delete wIndexItem;
// on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
                _add_RollbackIndexes (ZMCB, IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
// on error reset ZMF in its original state
                _Base::_add2PhasesCommit_Rollback(_Base::ZDescriptor,wZMFIdxCommit); // do not accept update on Master File and free resources
                goto zinsert_error;
                }

     IndexRankProcessed.push(wIndex_Rank) ;     // this index has been added to this rank
     IndexItemList.push(wIndexItem);          // with that key content

//        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);
    }// main for

// so far everything when well
//     commit Indexes changes
//      if an error occurs during index commit :
//          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
// Nb: Exception(s) is(are) pushed on stack
    wSt=_add_CommitIndexes (ZMCB,IndexItemList,IndexRankProcessed) ;
    if (wSt!=ZS_SUCCESS)
        {
        // Soft rollback master update regardless returned ZStatus
        // Nb: Exception is pushed on stack. ZException keeps the last status.
        _Base::_add2PhasesCommit_Rollback(_Base::ZDescriptor,wZMFIdxCommit);

        goto zinsert_error;
        }

// at this stage all indexes have been committed
//         commit for Master file data must be done now
//
    wSt = _Base::_insert2PhasesCommit_Commit(_Base::ZDescriptor,pRecord,wZMFIdxCommit);//! accept insert update on Master File
    if (wSt!=ZS_SUCCESS)    //! and if then an error occur : hard rollback all indexes and signal exception
            {
            _add_HardRollbackIndexes (ZMCB, IndexRankProcessed); // indexes are already committed so use hardRollback to counter pass
            }                                                    // don't care about the status . In case of error exception stack will trace it
zinsert_return:
    if (getJournalingStatus())
        {
        ZMCB.ZJCB->Journal->enqueue(ZJOP_Insert,pRecord);
        }

zinsert_error:
    _Base::_unlockFile (_Base::ZDescriptor) ; // set Master file unlocked
    IndexItemList.clear();
    return wSt;
}// zadd
//------------End insert sequence----------------------------------------------------

//----------------Add sequence------------------------------------------------------

/**
 * @brief ZMasterFile::zadd Adds a new record within Master File and updates all indexes
 *
 *      Add operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
 *      If an error occur, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
 *      If all indexes are successfully updated, then update on Master File is committed .
 *
 *      if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically removed from index file.
 *
 * @param[in] pRecord   user record content to add in a ZDataBuffer.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zadd       (ZDataBuffer& pRecord)
{
ZStatus wSt;
//    wSt=_Base::zaddWithAddress (pRecord,wAddress);      // record must stay locked until successfull commit for indexes

long            wZMFIdxCommit;
zaddress_type   wZMFAddress;

ZArray <long>         IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZIndexItemList        IndexItemList;      // stores keys description per index processed

ZIndexItem           *wIndexItem;
long wIndex_Rank;
long wi = 0;

//    ZExceptionStack.clear();
//    ZException.getLastStatus() = ZS_SUCCESS;

    wSt=_Base::_add2PhasesCommit_Prepare(_Base::ZDescriptor,pRecord,wZMFIdxCommit,wZMFAddress);  // prepare the add on Master File, reserve appropriate space, get entry in pool, lock it
    if (wSt!=ZS_SUCCESS)
            {
            goto zadd_error;
            }

// update all Indexes

    IndexRankProcessed.clear();

    for (wi=0;wi< ZMCB.IndexObjects.size();wi++)
    {
        if (ZVerbose)
        {
            fprintf (stdout,"Index number <%ld>\n",wi);
        }
        wIndexItem=new ZIndexItem;
        wSt=ZMCB.IndexObjects[wi]->_addKeyValue_Prepare(pRecord,wIndexItem,wIndex_Rank, wZMFAddress);
        if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast("During zadd operation on index number <%ld>",wi);
                delete wIndexItem;
// on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
                _add_RollbackIndexes (ZMCB, IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
// on error reset ZMF in its original state
                _Base::_add2PhasesCommit_Rollback(_Base::ZDescriptor,wZMFIdxCommit); // do not accept update on Master File and free resources
                goto zadd_error;
                }

     IndexRankProcessed.push(wIndex_Rank) ;     // this index has been added to this rank
     IndexItemList.push(wIndexItem);            // with that key content

//        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);

    }// main for

// so far everything when well
//     commit Indexes changes
//      if an error occurs during index commit :
//          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
// Nb: Exception(s) is(are) pushed on stack

    wSt=_add_CommitIndexes (ZMCB,IndexItemList,IndexRankProcessed) ;
    if (wSt!=ZS_SUCCESS)
        {
        // Soft rollback master update regardless returned ZStatus
        // Nb: Exception is pushed on stack. ZException keeps the last status.
        _Base::_add2PhasesCommit_Rollback(_Base::ZDescriptor,wZMFIdxCommit);

        goto zadd_error;
        }

// at this stage all indexes have been committed
//         commit for Master file data must be done now
//
    wSt = _Base::_add2PhasesCommit_Commit(_Base::ZDescriptor,pRecord,wZMFIdxCommit,wZMFAddress);// accept update on Master File
    if (wSt!=ZS_SUCCESS)    // and if then an error occur : hard rollback all indexes
            {
            _add_HardRollbackIndexes (ZMCB, IndexRankProcessed);// indexes are already committed so use hardRollback regardless returned ZStatus
            }                                                   // don't care about the status . In case of error exception stack will trace it
zadd_return:
    if (getJournalingStatus())
        {
        ZMCB.ZJCB->Journal->enqueue(ZJOP_Add,pRecord);
        }
zadd_error:
    _Base::_unlockFile (_Base::ZDescriptor) ; // set Master file unlocked
    IndexItemList.clear();
//    while (IndexItemList.size()>0)
//                delete IndexItemList.popR();

    return wSt;
}// zadd

/** @cond Development */

ZStatus
ZMasterFile::_add_CommitIndexes (ZMasterControlBlock& pZMCB,ZArray <ZIndexItem*>  &pIndexItemList, ZArray<long> &pIndexRankProcessed)
{
ZStatus wSt;
long wj = 0;

 /*   if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_CORRUPTED,
                                Severity_Fatal,
                                " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                pZMCB.Index.size(),
                                pZMCB.IndexObjects.size());
        ZException.exit_abort();
    }
*/
//    ZException.clear();
//    ZException.getLastStatus() = ZS_SUCCESS;
    wj=0;

    for (wj=0;wj<pIndexRankProcessed.size();wj++)
            {
//            wSt=pZMCB.IndexObjects[wj]->_addRollBackIndex(pIndexRankProcessed.popR()); //! rollback update on each index concerned

             wSt=pZMCB.IndexObjects[wj]->_addKeyValue_Commit(pIndexItemList[wj],pIndexRankProcessed[wj]);
             if (wSt!=ZS_SUCCESS)
                 {
                 ZException.addToLast("While committing add operation on index(es)");
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to pZMCB.IndexObjects.size() soft rollback

                    for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
                    {
                    pZMCB.IndexObjects[wR]->_addKeyValue_HardRollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                            // regardless ZStatus (exception is on stack)
                    } // for

                    for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
                    {
                    pZMCB.IndexObjects[wR]->_addKeyValue_Rollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                        // regardless ZStatus (exception is on stack)
                    } //for

                    return ZException.getLastStatus(); // return the very last status encountered
                }//wSt
            } // for
   return ZS_SUCCESS;
} // _add_CommitIndexes

/**
 * @brief ZMasterFile::_add_RollbackIndexes Soft rollback indexes using a ZArray containing index rank per index processed
 *      Soft rollback means : free memory resources in Pools that have been allocated (ZBAT) and locked with status ZBS_Allocated
 *
 * @param[in] pZMCB ZMasterControlBlock containing indexes definitions
 * @param[in] pIndexRankProcessed rank of this array gives the Index rank - content gives the Index Rank processed within the Index
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::_add_RollbackIndexes (ZMasterControlBlock& pZMCB, ZArray<long> &pIndexRankProcessed)
{
ZStatus wSt;
long wi = 0;
/*    if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                        pZMCB.Index.size(),
                                        pZMCB.IndexObjects.size());
                ZException.exit_abort();
            }*/

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_add_RollbackIndexes Soft Rollback of indexes on add \n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexObjects[wi]->_addKeyValue_Rollback(pIndexRankProcessed[wi]); // rollback add on each index concerned
            }//for                                                                  // don't care about ZStatus: exception stack will track

   return ZException.getLastStatus();
} // _add_RollbackIndexes

/**
 * @brief ZMasterFile::_add_HardRollbackIndexes Hard rollback
 * @param pZMCB
 * @param pIndexRankProcessed
 * @return
 */
ZStatus
ZMasterFile::_add_HardRollbackIndexes (ZMasterControlBlock& pZMCB, ZArray<long> &pIndexRankProcessed)
{
ZStatus wSt;
long wi = 0;
//    ZException.getLastStatus() = ZS_SUCCESS;
/*    if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                        pZMCB.Index.size(),
                                        pZMCB.IndexObjects.size());
                ZException.exit_abort();
            }*/
    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on add operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexObjects[wi]->_addKeyValue_HardRollback(pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            }// for                                                                     // don't care about ZStatus: exception stack will track

   return ZException.getLastStatus();
} // _add_HardRollbackIndexes

/** @endcond */
//----------------End Add sequence------------------------------------------------------
/**
 * @brief ZMasterFile::zreorgFile reorder the base file for ZMasterFile and rebuild all indexes
 *
 * superseeds ZRandomFile::zreorgFile as an method overload.
 *
 * As ZRandomFile::zreorgFile() changes physical block addresses in reorganizing file structure,
 * it is necessary to rebuild any defined index after having done a zreorgFile process.
 *
 * ZMasterFile::zreorgFile() does the whole stuff :
 * - base file reorganization
 * - rebuild for all defined indexes for the ZMasterFile @see ZIndexFile::zrebuildIndex()
 *
 * @param[in] pDump this boolean sets (true) the option of having surfaceScan during the progression of reorganization. Omitted by default (false)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zreorgFile (bool pDump,FILE *pOutput)
{
ZStatus wSt;
long wi = 0;
//ZExceptionMin ZException_sv;
bool wasOpen=false;
bool wgrabFreeSpaceSet = false;

zmode_type wMode = ZRF_Nothing ;
    if (ZDescriptor._isOpen)
             {
             wMode=ZDescriptor.getMode();
             zclose();
             wasOpen=true;
             }
     if ((wSt=zopen(ZDescriptor.URIContent,ZRF_Exclusive|ZRF_Write))!=ZS_SUCCESS)
                             return wSt;


     if (!ZDescriptor.ZFCB->GrabFreeSpace)        // activate grabFreeSpace if it has been set on
                 {
     ZDescriptor.ZFCB->GrabFreeSpace=true;
                 wgrabFreeSpaceSet = true;
                 }

    zstartPMSMonitoring();

    wSt = ZRandomFile::_reorgFileInternals(ZDescriptor,pDump,pOutput);

   while (wi < ZMCB.IndexObjects.size())
           {
//            wSt=pZMCB.IndexObjects[wj]->_addRollBackIndex(pIndexRankProcessed.popR()); // rollback update on each index concerned
       wSt=ZMCB.IndexObjects[wi]->zrebuildIndex(false,pOutput); // hard rollback update on each already committed index

       if (wSt!=ZS_SUCCESS)
               {
//               ZException_sv = ZException; // in case of error : store the exception but continue rolling back other indexes
               ZException.addToLast(" during Index rebuild on index <%s> number <%02ld> ",
                                           ZMCB.Index[wi].Name.toString(),
                                           wi);
               }

           wi++;
           }

    zendPMSMonitoring ();
    fprintf (pOutput,
             " ----------End of ZMasterFile reorganization process-------------\n");

    zreportPMSMonitoring(pOutput);

    if (ZException.getLastStatus()!=ZS_SUCCESS)
             {
//             ZException=ZException_sv;
             goto error_zreorgZMFFile;
             }

//    ZException.getLastStatus() = ZS_SUCCESS;

end_zreorgZMFFile:

    if (wgrabFreeSpaceSet)        // restore grabFreeSpace if it was off and has been set on
                 {
                 ZDescriptor.ZFCB->GrabFreeSpace=false;
                 }
    zclose ();
    if (wasOpen)
            zopen(ZDescriptor.URIContent,wMode);

    return ZException.getLastStatus();

error_zreorgZMFFile:
    ZException.printUserMessage(pOutput);
    goto end_zreorgZMFFile;

}// ZMasterFile::zreorgFile

/**
 * @brief ZMasterFile::zindexRebuild Rebuilds an index from scratch for a ZMasterFile
 * @warning ZMasterFile must be opened in mode ZRF_Exclusive | ZRF_All before calling this method. If not, the operation will be rejected with an error status of ZS_INVOP.
 *          ZException will be setup accordingly.
 *
 * @param pIndexRank    rank of the index for the ZMasterFile (ZMasterControlBlock)
 * @param pStat         option to request for statistical report (true: yes , false:no)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zindexRebuild (const long pIndexRank,bool pStat, FILE *pOutput)
{
    if (!isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Severe,
                                    " File <%s> must be opened to use this function",
                                    ZDescriptor.URIContent.toString());
            return ZS_INVOP;
            }
    if ((pIndexRank<0)||(pIndexRank>ZMCB.Index.size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Severe,
                                    " Out of indexes boundaries: rank <%ld>  boundaries [0,%ld] : File <%s>",
                                    pIndexRank,
                                    ZMCB.Index.lastIdx(),
                                    ZDescriptor.URIContent.toString());
            return ZS_OUTBOUND;
            }
 return ZMCB.IndexObjects[pIndexRank]->zrebuildIndex (pStat,pOutput);
}//zindexRebuild


//----------------Remove sequence------------------------------------------------------

ZStatus
ZMasterFile::zremoveByRank    (const long pZMFRank)
{
ZDataBuffer wZMFRecord;

    return _removeByRank (wZMFRecord,pZMFRank);
} // zremoveByRank

ZStatus
ZMasterFile::zremoveByRankR     (ZDataBuffer &pZMFRecord,const long pZMFRank)
{
    return _removeByRank (pZMFRecord,pZMFRank);
} // zremoveByRankR

/**
 * @brief ZMasterFile::_removeByRank Removes a record corresponding to logical position pZMFRank within Master File and updates all indexes
 *
 * Remove operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
 *
 * @note  If an error occurs during processing, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
 * If all indexes are successfully updated, then update on Master File is committed .
 * if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically recreated within index file.
 *
 * @param[in,out] pDescriptor ZMasterFile descriptor. It is updated during remove operation
 * @param[in] pZMCB ZMasterControlBlock of the file
 * @param[out] pZMFRecord the Record that is removed
 * @param[in] pZMFRank the logical position (rank) of the record to remove
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::_removeByRank  (ZDataBuffer &pZMFRecord,
                             const long pZMFRank)
{
ZStatus     wSt;

long                  wZMFIdxCommit;
zaddress_type         wZMFAddress;
ZArray <long>         IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZIndexItemList        IndexItemList;      // stores keys description per index processed (rollback purpose). Must stay as pointer (double free)
ZIndexItem            *wIndexItem;
ZDataBuffer           wKeyContent;

ZBlock          wBlock;

//---- get Master File header test for lock - if OK lock it

    wSt=_Base::_remove_Prepare(ZDescriptor,pZMFRank,wZMFAddress);  // prepare remove on Master File mark entry in pool as to be deleted
    if (wSt!=ZS_SUCCESS)
            {
            _Base::_remove_Rollback(ZDescriptor,pZMFRank);
            return wSt;
            }
    wZMFIdxCommit = pZMFRank;
    wSt=_Base::_readBlockAt(ZDescriptor,wBlock,wZMFAddress,_GET_FUNCTION_NAME_);  // we know that it is locked as ZBS_BeingDeleted
    if (wSt!=ZS_SUCCESS)
            {
            _Base::_remove_Rollback(ZDescriptor,pZMFRank);
            return wSt;
            }

    pZMFRecord = wBlock.Content;
//    pZMFRecord.setData(wBlock.Content.Data,wBlock.Content.Size);
    wBlock.Content.clear();
//
// update all Indexes
//
    IndexRankProcessed.clear();
    IndexItemList.clear();
    long wIndex_Rank;
    long wi;

    for (wi=0;wi< ZMCB.IndexObjects.size();wi++)
    {
        wSt = _keyValueExtraction(*ZMCB.Index[wi].ZKDic,pZMFRecord,wKeyContent);
        if (wSt!=ZS_SUCCESS)
                        goto _removeByRank_return;
            wIndexItem = new ZIndexItem ;
            wSt=ZMCB.IndexObjects[wi]->_removeKeyValue_Prepare(wKeyContent,
                                                                wIndexItem,
                                                                wIndex_Rank,
                                                                wZMFAddress);
            if (wSt!=ZS_SUCCESS)
                {
                delete wIndexItem; // free memory for errored key value
                // on error reset all already processed indexes in their original state
                // (IndexRankProcessed heap contains the Index ranks added)
                // An additional error during index rollback will be put on exception stack

                _remove_RollbackIndexes (ZMCB, IndexRankProcessed); // do not care about ZStatus : exception will be on stack

                // on error reset ZMF in its original state
                _Base::_remove_Rollback(ZDescriptor,wZMFIdxCommit); // do not accept update on Master File and free resources

//                if (ZMCB.HistoryOn)
//                      ZMFHistory.push (ZO_Add,wi,wAddress,wSt); //! journalize Error on index if journaling is enabled

               // _Base::_unlockFile (ZDescriptor) ; // unlock done in remove rollback

                goto _removeByRank_return; ;   // and send appropriate status
                }
     IndexRankProcessed.push(wIndex_Rank) ; // this index has been added to this rank
     IndexItemList.push(wIndexItem);        // with that key content
//        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);
    }// main for

// so far everything when well
//     commit changes

    wSt=_remove_CommitIndexes (ZMCB,IndexItemList,IndexRankProcessed) ;
    if (wSt!=ZS_SUCCESS)
                {
                // in case of error : appropriate indexes soft or hard rollback is made in Commit Indexes routine

                _Base::_remove_Rollback(ZDescriptor,wZMFIdxCommit); // an error occurred on indexes : rollback all MasterFile
               // _Base::_unlockFile (ZDescriptor) ; // unlock is done in rollback routine
                goto _removeByRank_return;// if an error occurs here : this is a severe error that only may be caused by an HW or IO subsystem failure.
                }

    wSt = _Base::_remove_Commit(ZDescriptor,wZMFIdxCommit);// accept update on Master File
    if (wSt!=ZS_SUCCESS)    // and if then an error occur : rollback all indexes and signal exception
            {
            _remove_HardRollbackIndexes (ZMCB, IndexItemList,IndexRankProcessed);    // indexes are already committed so use hardRollback to counter pass
            goto _removeByRank_return;
            }

    if (getJournalingStatus())
    {
        ZMCB.ZJCB->Journal->enqueue(ZJOP_RemoveByRank,pZMFRecord,pZMFRank,wZMFAddress);
    }

_removeByRank_return:
    _Base::_unlockFile (ZDescriptor) ;
/*
    while (IndexItemList.size()>0)
                  delete IndexItemList.popR();
*/
    IndexItemList.clear();
    return wSt;
}// _removeByRank



/**
 * @brief ZMasterFile::_remove_CommitIndexes For All ZIndexFile associated to current ZMasterFile commits the remove operation
 *
 *
 * @param[in] pZMCB             ZMasterControlBlock owning indexes definitions
 * @param[in] pIndexItemList        List of pointers to ZIndexItem to be removed (Index Key contents)- Necessary in case of Hard Rollback.
 * @param[in] pIndexRankProcessed   List of ranks for ZIndexFile to be removed
 * @return  a ZStatus value. ZException is set appropriately with error message content in case of error.
 */
ZStatus
ZMasterFile::_remove_CommitIndexes (ZMasterControlBlock& pZMCB, ZIndexItemList & pIndexItemList,ZArray<long> &pIndexRankProcessed)
{
ZStatus wSt;
long wj = 0;

//    ZException.getLastStatus() =ZS_SUCCESS;
    for (wj=0;wj<pIndexRankProcessed.size();wj++)
            {
            wSt=pZMCB.IndexObjects[wj]->_removeKeyValue_Commit(pIndexRankProcessed[wj]);
            if (wSt!=ZS_SUCCESS)
                {
//  hard roll back already processed indexes
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to pZMCB.IndexObjects.size() soft rollback

                for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
                {
                pZMCB.IndexObjects[wR]->_removeKeyValue_HardRollback(pIndexItemList[wR] ,pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                        // regardless ZStatus (exception is on stack)
                } // for

                for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
                {
                pZMCB.IndexObjects[wR]->_removeKeyValue_Rollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                    // regardless ZStatus (exception is on stack)
                }// for

            return ZException.getLastStatus(); // return the very last status encountered
            }//wSt
    } // Main for

   return ZException.getLastStatus();
} // _remove_CommitIndexes


ZStatus
ZMasterFile::_remove_RollbackIndexes (ZMasterControlBlock& pZMCB, ZArray<long> &pIndexRankProcessed)
{
ZStatus wSt;
long wi = 0;
        if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
                {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_CORRUPTED,
                                            Severity_Fatal,
                                            " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                            pZMCB.Index.size(),
                                            pZMCB.IndexObjects.size());
                    ZException.exit_abort();
                }

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_remove_RollbackIndexes Soft Rollback of indexes on remove\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexObjects[wi]->_removeKeyValue_Rollback(pIndexRankProcessed[wi]);  // rollback remove on each index concerned
            }// for                                                                     // don't care about ZStatus: exception stack will track

   return ZException.getLastStatus();
} // _remove_RollbackIndexes


ZStatus
ZMasterFile::_remove_HardRollbackIndexes (ZMasterControlBlock& pZMCB, ZArray<ZIndexItem*> &pIndexItemList, ZArray<long> &pIndexRankProcessed)
{
ZStatus wSt;
long wi = 0;

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                        pZMCB.Index.size(),
                                        pZMCB.IndexObjects.size());
                ZException.exit_abort();
            }
    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on remove operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexObjects[wi]->_removeKeyValue_HardRollback(pIndexItemList[wi],pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            } // for                                                    // don't care about ZStatus: exception stack will track

   return ZException.getLastStatus();
} // _remove_HardRollbackIndexes

//----------------End Remove sequence------------------------------------------------------


//----------------Search sequence-----------------------------------------------------------
//

ZStatus
ZMasterFile::zgenerateKeyValueList ( ZDataBuffer& pKey ,long pKeyNumber,int pCount,...)
{
va_list args;
va_start (args, pCount);
ZArray<void*> wKeyValues;

    for (int wi=0;wi<pCount;wi++)
    {
        wKeyValues.push(va_arg(args,void*));
    }
 va_end(args);
 return (zgenerateKeyValue(ZMCB.Index[pKeyNumber],wKeyValues,pKey));
} // zgenerateKeyValue


/**
 * @brief ZMasterFile::_generateKeyValue creates a ZDataBuffer with key values from a ZArray containing natural individual values
 * @param[in] pICB
 * @param[in] pKeyValues a ZArray with each element represents a key field value in key dictionary rank order
 * @param[out] pKey a ZDataBuffer with the key content in natural format
 * @return
 */
ZStatus
ZMasterFile::zgenerateKeyValue ( ZIndexControlBlock& pICB,ZArray<void*> &pKeyValues,ZDataBuffer& pKey)
{
ssize_t wKeyOffset = 0;

// As there might be partial key search, we do not test wether all key fields are processed but only on maximum number of requested fields
//
    if (pKeyValues.size()>pICB.ZKDic->size())
                  {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_INVOP,
                                            Severity_Fatal,
                                            " Number of requested fields for key is %ld while number of key fields is %ld",
                                            pKeyValues.size(),
                                            pICB.ZKDic->size());
                    return ZS_INVOP;
                   }

    pKey.clearData();
    pKey.allocate(pICB.IndexRecordSize()+1);
    memset (pKey.Data,0,pICB.IndexRecordSize());

    for (long wi=0;wi<pKeyValues.size();wi++)
        {
        memmove (pKey.Data+wKeyOffset,pKeyValues[wi],pICB.ZKDic->Tab[wi].NaturalSize);

        wKeyOffset += pICB.ZKDic->Tab[wi].NaturalSize;
        } // for

    return  ZS_SUCCESS;
}//_generateKeyValue

/**
 * @brief ZMasterFile::zsearch searches for a single key value using a ZKey Object
 *
 * if key has duplicates, the first found value according search algorithm is given
 *
 * @param pRecord
 * @param pKey
 * @return
 */
ZStatus
ZMasterFile::zsearch(ZDataBuffer &pRecord,ZKey *pKey)
{

    return (zsearch(pRecord,(ZDataBuffer &)*pKey,pKey->IndexNumber));
}
/**
 * @brief ZMasterFile::zsearch searches for a single key value using a ZDataBuffer containing key value to search
 * Key to search for has to be previously formatted to index internal format using appropriate routines
 * @copydetail ZIndexFile::_search()
 *
 * @param[out] pRecord
 * @param[in] pKeyValue
 * @param[in] pIndexNumber
 * @return
 */
ZStatus
ZMasterFile::zsearch (ZDataBuffer &pRecord,ZDataBuffer &pKeyValue,const long pIndexNumber)
{
ZStatus wSt;
//zaddress_type wAddress;
//long wIndexRank;
ZIndexResult wZIR;
    wSt = ZMCB.IndexObjects[pIndexNumber]->_search(pKeyValue,*ZMCB.IndexObjects[pIndexNumber],wZIR);
    if (wSt!=ZS_FOUND)
            return wSt;
    return zgetByAddress(pRecord,wZIR.ZMFAddress);
}

class ZIndexCollection;

/**
 * @brief ZMasterFile::zsearchAll search for all index rank using a ZDataBuffer containing key value to search.
 * Exact or Partial key search is defined by a ZMatchSize_type (pZMS) that defines wether search is exact or partial.
 *
 * Exact key value : comparizon is done on key content and key length. Both must be equal.
 * Partial key value : given key must have a length less or equal to key length
 *
 * @param[in] pKeyValue a ZDataBuffer containing the key content to search for. Key content must have been previously formatted to index internal format.
 * @param[in] pIndexNumber ZMasterFile index number to search
 * @param[out] pIndexCollection returned collection of index references
 * @return
 */
ZStatus
ZMasterFile::zsearchAll (ZDataBuffer &pKeyValue,const long pIndexNumber,ZIndexCollection& pIndexCollection, const ZMatchSize_type pZMS)
{
ZStatus wSt;

    wSt = ZMCB.IndexObjects[pIndexNumber]->_searchAll(pKeyValue,*ZMCB.IndexObjects[pIndexNumber],pIndexCollection,pZMS);
    return wSt;
}
/**
 * @brief ZMasterFile::zsearchAll search for all index rank using a ZKey object. Exact or Partial key search is set by ZKey
 *
 * Exact key value : comparizon is done on key content and key length. Both must be equal.
 * Partial key value : given key must have a length less or equal to key length
 *
 * @param[in] pZKey a ZKey containing the key content to search with all necessary index references
 * @param[out] pIndexCollection returned collection of index references
 * @return
 */
ZStatus
ZMasterFile::zsearchAll (ZKey &pZKey,ZIndexCollection& pIndexCollection)
{
ZStatus wSt;

    pIndexCollection.reset();
    ZMatchSize_type wZSC = ZMS_MatchIndexSize;

    pIndexCollection.ZIFFile = ZMCB.IndexObjects[pZKey.IndexNumber];// assign ZIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    if (pZKey.FPartialKey)
            wZSC=ZMS_MatchKeySize ;
    wSt = ZMCB.IndexObjects[pZKey.IndexNumber]->_searchAll(pZKey,*ZMCB.IndexObjects[pZKey.IndexNumber],pIndexCollection,wZSC);
    return wSt;
}//zsearchAll

/**
 * @brief ZMasterFile::zsearchFirst searches for first key value (in index key order) of pKey (see @ref ZKey) and returns its corresponding ZMasterFile record content.
 *
 * a Collection and its context ( see @ref ZIndexCollection and @ref ZIndexCollectionContext )
 * is created by 'new' instruction and needs to be mentionned when further calls to ZMasterFile::zsearchNext() are made.
 *
 * enriched during further zsearchNext accesses - and may be obtained to use Collection facitilies. see @ref Collection.
 *
 * Collection context is created and maintained within collection. This meta data describes the context of current search.
 *
 *  zsearchFirst uses ZIndexFile::_searchFirst().
 *
 * @copydoc ZIndexFile::_searchFirst()
 *
 * @param[in] pZKey     Key object. contains key content value to find and ZMasterFile index number.
 * @param[out] pRecord  Found record content if any match found.
 * @param[out] pCollection A pointer to the contextual meta-data from the search created by _searchFirst() routine.
 *
 * It contains
 * - a ZArray of ZIndexResult objects, enriched with successive _searchNext() calls
 * - Contextual meta-data for the search (an ZIndexCollectionContext embeded object holds this data)
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on
 *
 * @param[in] pLock  lock mask to apply to found record.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 */
ZStatus
ZMasterFile::zsearchFirst (ZKey &pZKey,
                           ZDataBuffer& pRecord,
                           ZIndexCollection *pCollection)
{
ZStatus wSt;
ZIndexResult wZIR;

    ZMatchSize_type wZMS = ZMS_MatchIndexSize;


    if (pZKey.FPartialKey)
            wZMS=ZMS_MatchKeySize ;
    wSt = ZMCB.IndexObjects[pZKey.IndexNumber]->_searchFirst(pZKey,
                                                             *ZMCB.IndexObjects[pZKey.IndexNumber],
                                                             pCollection,
                                                             wZIR,
                                                             wZMS);
    if (wSt!=ZS_FOUND)
                return wSt;

    wSt=zgetByAddress(pRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
            return wSt;
    return ZS_FOUND;
}// zsearchFirst
/**
 * @brief ZMasterFile::zsearchNext
 * @copydoc ZIndexFile::_searchNext()
 *
 * @param [in] pZKey  Key object. Contains key content value to find and ZMasterFile index number.
 * @param [in-out] pCollection A pointer to the contextual meta-data from the search created by _searchFirst() routine.
 *
 * It contains
 * - a ZArray of ZIndexResult objects, enriched with successive _searchNext calls
 * - Contextual meta-data for the search.
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on.
 * @param [out] pRecord Record content as a result of the _searchNext operation, if any.
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 */
ZStatus
ZMasterFile::zsearchNext (ZKey &pZKey,
                          ZDataBuffer & pRecord,
                          ZIndexCollection *pCollection)
{
ZStatus wSt;
ZIndexResult wZIR;

    if (pCollection==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                "Invalid collection/context : it must have been created using zsearchFirst");
        return ZS_INVOP;
        }

    //pIndexCollection.ZIFFile = ZMCB.IndexObjects[pIndexNumber];// assign ZIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    wSt = ZMCB.IndexObjects[pZKey.IndexNumber]->_searchNext(wZIR,pCollection);
    if (wSt!=ZS_FOUND)
                return wSt;

    wSt=zgetByAddress(pRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
            return wSt;
    return ZS_FOUND;
}// zsearchFirst





//----------------End Search sequence----------------------------------------------------


/**
 * @brief ZMasterFile::getKeyIndexFields returns natural field values from a ZIndex key at rank pIndexRank for ZIndex pIndexNumber (key number)
 * @note given rank is relative position of record within ZIndexFile file and NOT relative position of record within ZMasterFile file
 * @param[out] pKeyFieldValues   returns natural field values concatenated within a ZDataBuffer
 * @param[in] pIndexRank        Index key row
 * @param[in] pIndexNumber      ZIndex number (key number) - in definition order
 * @return a ZStatus
 */
ZStatus
ZMasterFile::getKeyIndexFields(ZDataBuffer &pKeyFieldValues, const long pIndexRank,const long pIndexNumber)
{
 ZStatus wSt;
 ZDataBuffer wIndexRawContent;
 ZIndexItem  wIndexContent;
    wSt=ZMCB.IndexObjects[pIndexNumber]->_Base::zget(wIndexRawContent,pIndexRank);
    if (wSt!=ZS_SUCCESS)
                     return wSt;
    wIndexContent.fromFileKey(wIndexRawContent);
    return ZMCB.IndexObjects[pIndexNumber]->getKeyIndexFields(pKeyFieldValues,wIndexContent.KeyContent);
}

/**
 * @brief getRawIndex   Gets a raw index item (without Endian conversion) corresponding to is IndexNumber and its rank
 * @param pIndexItem    A ZIndexItem with returned index content
 * @param pIndexRank    Rank for index
 * @param pIndexNumber  Index number
 * @return
 */
ZStatus
ZMasterFile::getRawIndex(ZIndexItem &pIndexItem,const long pIndexRank,const long pIndexNumber)
    {
    ZStatus wSt;
    ZDataBuffer wIndexContent;
    wSt=ZMCB.IndexObjects[pIndexNumber]->_Base::zget(wIndexContent,pIndexRank);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    pIndexItem.fromFileKey(wIndexContent);
    return wSt;
    }

/** @} */ // ZMasterFileGroup

//----------------------Reports---------------------------------------
/**
 * @brief ZMasterFile::ZMCBreport Reports the whole content of ZMasterControlBlock : indexes definitions and dictionaries
 */
void ZMasterFile::ZMCBreport(void)
{
    ZMCB.report();
    return;
}

//----------------------XML Reports and stuff-----------------------------------
/** addtogroup XMLGroup
 * @{ */

/** @cond Development
 *
 * @brief ZMasterFile::_writeXML_FileHeader Generates the xml definition for current opened ZMasterFile's ZFileDescriptor (header) plus ZReserved : ZMasterControlBlock
 * @param pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZMasterFile::_writeXML_ZRandomFileHeader(ZMasterFile& pZMF,FILE *pOutput)
{


   ZRandomFile::_writeXML_ZRandomFileHeader(pZMF.ZDescriptor,pOutput);

   fprintf (pOutput,

            "  <ZMasterControlBlock>\n"
            "         <HistoryOn>%s</HistoryOn>\n"
            "         <JournalingOn>%s</JournalingOn>\n"
            "         <IndexFileDirectoryPath>%s</IndexFileDirectoryPath>\n"
            ,

            pZMF.ZMCB.HistoryOn?"true":"false",
            pZMF.getJournalingStatus()?"true":"false",
            pZMF.ZMCB.IndexFilePath.toString()
            );

   fprintf (pOutput,
            "         <!-- Changing IndexRank position in a file description has no impact -->\n"
            "         <Index>\n");
   for (long wi=0;wi<pZMF.ZMCB.Index.size();wi++)
   {
   _writeXML_Index(pZMF,wi,pOutput);
   }//for wi
   fprintf (pOutput,
            "         </Index>\n");
   fprintf (pOutput,
            "  </ZMasterControlBlock>\n");

}//_writeXML_FileHeader

void
ZMasterFile::_writeXML_Index(ZMasterFile& pZMF,const long pIndexRank,FILE* pOutput)
{

    pZMF.ZMCB.Index[pIndexRank].ZKDic->_reComputeSize();
    fprintf (pOutput,
             "         <IndexRank>\n"
             "           <Rank>%ld</Rank> <!-- not significant -->\n"
             "           <Name>%s</Name> <!-- warning modification of this field implies index duplication see documentation -->\n"
             "           <Duplicates>%s</Duplicates> <!-- warning modification of this field must be cautiously done see documentation -->\n"
             "           <AutoRebuild>%s</AutoRebuild>\n",
             pIndexRank,
             pZMF.ZMCB.Index[pIndexRank].Name.toString(),
             pZMF.ZMCB.Index[pIndexRank].Duplicates==ZST_DUPLICATES?"ZST_DUPLICATES":"ZST_NODUPLICATES",
             pZMF.ZMCB.Index[pIndexRank].AutoRebuild?"true":"false"
             );

            _writeXML_KDic(pZMF.ZMCB.Index[pIndexRank].ZKDic,pOutput);

    fprintf (pOutput,
             "         </IndexRank>\n");


}//_writeXML_Index
void
ZMasterFile::_writeXML_KDic(ZKeyDictionary* ZKDic,FILE* pOutput)
{

    ZKDic->_reComputeSize();
    fprintf (pOutput,
             "           <ZKDic>\n"
             "               <NaturalSize>%ld</NaturalSize>   <!-- not modifiable : computed field -->\n"
             "               <UniversalSize>%ld</UniversalSize> <!-- not modifiable : computed field -->\n"
             ,
             ZKDic->NaturalSize,
             ZKDic->UniversalSize
             );
     for (long wd=0;wd<ZKDic->size();wd++) // dictionary detail
             {
    fprintf (pOutput,
             "              <KeyField>\n"
             "                <Rank>%ld</Rank>  <!-- not modifiable : only field position in ZKDic is taken -->\n"
             "                <Name>%s</Name>\n"
             "                <NaturalSize>%ld</NaturalSize>\n"
             "                <UniversalSize>%ld</UniversalSize>\n"
             "                <ArraySize>%ld</ArraySize>\n"
             "                <ZType>%s</ZType>   <!-- see ZType_type definition : beware the typos -->\n"
             "                <RecordOffset>%ld</RecordOffset>\n"
             "              </KeyField>\n"
             ,
             wd,
             ZKDic->Tab[wd].Name.toString() ,
             ZKDic->Tab[wd].NaturalSize,
             ZKDic->Tab[wd].UniversalSize,
             ZKDic->Tab[wd].ArraySize,
             decode_ZType( ZKDic->Tab[wd].ZType),
             ZKDic->Tab[wd].RecordOffset
             );
             }// for
    fprintf (pOutput,
              "          </ZKDic>\n");
    return;

}//_writeXML_KDic

/** @endcond */ // Development

/**
 * @brief ZMasterFile::zwriteXML_IndexDefinition Generates the xml definition for current opened ZMasterFile's Index of rank pIndexRank
 * @param[in]   Index rank to generate the XML definition for
 * @param[out] pOutput defaulted to nullptr. Could be stdout or stderr .
 *             If nullptr, then an xml file is generated named <directory path><base name>-<indexname><pIndexRank>.xml
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zwriteXML_IndexDefinition(const long pIndexRank, FILE *pOutput)
{
FILE* wOutput=pOutput;
descString OutPath;
    if ((pIndexRank<0)||(pIndexRank>ZMCB.IndexObjects.lastIdx()))
       {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_INVOP,
                               Severity_Severe,
                               "Invalid index number <%ld> requested while index range is [0,%ld]",
                               pIndexRank,
                               ZMCB.IndexObjects.lastIdx());
       return (ZS_INVOP);
       }
    if (pOutput==nullptr)
    {
        uriString uriOutput;
        descString OutBase;
        uriOutput= ZDescriptor.URIContent.toString();
        OutBase = uriOutput.getBasename();
        OutPath = uriOutput.getDirectoryPath();
        OutPath += OutBase;
        OutPath += "-";
        OutPath += ZMCB.Index[pIndexRank].Name.toString();
        OutPath.add("%02ld",pIndexRank);
        OutPath += ".xml";
        wOutput = fopen(OutPath.toString(),"w");
        if (wOutput==nullptr)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_ERROPEN,
                                 Severity_Severe,
                                 " cannot open file %s for output",
                                 OutBase.toString());
                ZException.exit_abort();
                }
    }
    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");

    _writeXML_Index(*this,pIndexRank,wOutput);

    fprintf (wOutput,
             "</zicm>");
    if (pOutput==nullptr)
                fclose(wOutput);

    fprintf (stderr,"Xml index definition file <%s> has been created.\n",
                     OutPath.toString());
    return ZS_SUCCESS;
}//zwriteXML_IndexDefinition

/**
 * @brief ZMasterFile::zwriteXML_IndexDictionary generates the xml definition of a Key dictionary for the mentionned index rank for current ZMF
 * @param[in]   Index rank to generate the XML definition for
 * @param[out] pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>-<indexname><pIndexRank>-kdic.xml
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zwriteXML_IndexDictionary(const long pIndexRank, FILE *pOutput)
{
FILE* wOutput=pOutput;
descString OutPath;
    if ((pIndexRank<0)||(pIndexRank>ZMCB.IndexObjects.lastIdx()))
       {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_INVOP,
                               Severity_Severe,
                               "Invalid index number <%ld> requested while index range is [0,%ld]",
                               pIndexRank,
                               ZMCB.IndexObjects.lastIdx());
       return (ZS_INVOP);
       }
    if (pOutput==nullptr)
    {
        uriString uriOutput;
        descString OutBase;
        uriOutput= ZDescriptor.URIContent.toString();
        OutBase = uriOutput.getBasename();
        OutPath = uriOutput.getDirectoryPath();
        OutPath += OutBase;
        OutPath += "-";
        OutPath += ZMCB.Index[pIndexRank].Name.toString();
        OutPath.add("%02ld",pIndexRank);
        OutPath.add("-kdic");
        OutPath += ".xml";
        wOutput = fopen(OutPath.toString(),"w");
        if (wOutput==nullptr)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_ERROPEN,
                                 Severity_Severe,
                                 " cannot open file %s for output",
                                 OutBase.toString());
                ZException.exit_abort();
                }
    }
    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");


    _writeXML_KDic(ZMCB.Index[pIndexRank].ZKDic,wOutput);

    fprintf (wOutput,
             "</zicm>");
    if (pOutput==nullptr)
                fclose(wOutput);

    fprintf (stderr,"Xml key dictionary definition file <%s> has been created.\n",
                     OutPath.toString());
    return ZS_SUCCESS;
}//zwriteXML_IndexDictionary


/**
 * @brief ZMasterFile::zwriteXML_FileHeader Generates the xml definition for the whole current opened ZMasterFile's header
 *  This is a non static routine
 *
 *  This routine generates an xml file containing :
 *      - the
 *
 * @param pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZMasterFile::zwriteXML_FileHeader(FILE *pOutput)
{
FILE* wOutput=pOutput;
descString OutPath;
    if (pOutput==nullptr)
    {
        uriString uriOutput;
        descString OutBase;
        uriOutput= ZDescriptor.URIContent.toString();
        OutBase = uriOutput.getBasename();
        OutPath = uriOutput.getDirectoryPath();
        OutPath += OutBase;
        OutPath += ".xml";
        wOutput = fopen(OutPath.toString(),"w");
        if (wOutput==nullptr)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_ERROPEN,
                                 Severity_Severe,
                                 " cannot open file %s for output",
                                 OutBase.toString());
                ZException.exit_abort();
                }
    }
    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");
    fprintf (wOutput,
             "<File>\n");
    _writeXML_ZRandomFileHeader(*this,wOutput);
    fprintf (wOutput,
             "</File>\n"
             "</zicm>");

    if (pOutput==nullptr)
                fclose(wOutput);


    fprintf (stderr,"Xml header definition file <%s> has been created.\n",
                     OutPath.toString());
    return;
}//zwriteXML_FileHeader
/**
 * @brief ZMasterFile::zwriteXML_FileHeader Static function : Generates the xml definition for a ZMasterFile's header given by it path name pFilePath
 * @note the ZMasterFile is opened for read only ZRF_Read_Only then closed.
 *
 * @param[in] pFilePath points to a valid file to generate the definition from
 * @param[in] pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZMasterFile::zwriteXML_FileHeader(const char* pFilePath,FILE *pOutput)
{
ZMasterFile wZMF;
ZStatus wSt;
FILE* wOutput=pOutput;
descString OutPath;
    wSt = wZMF.zopen(pFilePath,ZRF_Read_Only);
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();

    if (pOutput==nullptr)
    {
        uriString uriOutput;
        descString OutBase;
        uriOutput= pFilePath;
        OutBase = uriOutput.getBasename();
        OutPath = uriOutput.getDirectoryPath();
        OutPath += OutBase;
        OutPath += ".xml";
        wOutput = fopen(OutPath.toString(),"w");
        if (wOutput==nullptr)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_ERROPEN,
                                 Severity_Severe,
                                 " cannot open file %s for output",
                                 OutBase.toString());
                ZException.exit_abort();
                }
    }

    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");

    fprintf (wOutput,
             "<File>\n");
    wZMF._writeXML_ZRandomFileHeader(wZMF,wOutput);
    fprintf (wOutput,
             "</File>\n");
    fprintf (wOutput,
             "</zicm>");
    wZMF.zclose();
    if (pOutput==nullptr)
                fclose(wOutput);
    fprintf (stderr,"Xml definition file <%s> has been created.\n",
                     OutPath.toString());

}  // static zwriteXML_FileHeader

#ifdef QT_CORE_LIB

/*----------------------------------------------------------------------------------------------
 *  Warning : never use QDomNode::cloneNode (true) . It generates SIGEV after combined 2 usages
 * ---------------------------------------------------------------------------------------------
*/
/** @cond Development */
/**
 * @brief ZMasterFile::_XMLzicmControl Loads an XML document and Makes all XML controls to have an appropriate <zicm> document
 *
 *  Controls are made on
 *  - xml coherence
 *  - zicm version attribute
 *
 *  It delivers the FIRST child node ELEMENT AFTER root node element <zicm>, without controlling either tag name or content
 *
 * @param[in] pFilePath     a Cstring pointing to xml file to load and control
 * @param[out] XmlDoc       The XML document content loaded as a return
 * @param[out] pFirstNode   The First node to exploit as a return
 * @return
 */
ZStatus
ZMasterFile::_XMLzicmControl(const char* pFilePath,QDomDocument &XmlDoc,QDomNode &pFirstNode)
{
uriString wUriPath;
ZDataBuffer wXMLcontent;
ZStatus wSt;
    wUriPath = pFilePath;
//    wUriPath = "/home/gerard/uncryptedparams.xml";
    if (!wUriPath.exists())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTEXIST,
                                    Severity_Error,
                                    " File <%s> does not exist while trying to load it",
                                    pFilePath);
            return ZS_FILENOTEXIST;
            }
    wSt=wUriPath.loadContent(wXMLcontent);
    if (wSt!=ZS_SUCCESS)
            return wSt;
QDomNode wNode;
QString wN;
//QDomDocument XmlDoc;


QString ErrorMsg;
int ErrLine=0;
int ErrColumn=0;
bool Result = true;
bool NameSpaceProcessing = true; //option to be set at a higher level
    XmlDoc.clear();
    Result = XmlDoc.setContent(   wXMLcontent.toQByteArray(),
                                        NameSpaceProcessing,
                                        &ErrorMsg,
                                        &ErrLine,
                                        &ErrColumn) ;

    if (!Result)
                {
                ZException.setComplement(
                        "line %d Column %d Xml error >>",
                        ErrLine,
                        ErrColumn,
                        ErrorMsg.toUtf8());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Warning,
                                        " XML Error(s) have been reported : ");
                }


QDomNode wNodeRoot= XmlDoc.firstChildElement("zicm");

    if (wNodeRoot.isNull())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_EMPTYFILE,
                                        Severity_Error,
                                        "In file %s : xml document is empty or errored",
                                        pFilePath);
                return ZS_EMPTYFILE;
                }
QDomElement wElement;
//    wNodeRoot=wNodeRoot.nextSiblingElement(); // first node is never an element
    wElement=wNodeRoot.toElement();
    wN= wElement.tagName();
    if (wN!="zicm")
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "Bad xml document <%s>.Expected first element <zicm> not found. Found <%s>",
                                        pFilePath,
                                        wN.toStdString().c_str());
                return ZS_XMLERROR;
                }
QString wAttribute;
    wAttribute = wElement.attribute("version");
    if (wAttribute!=__ZRF_XMLVERSION_CONTROL__)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "Bad xml document <%s>.Bad zicm version.Expected <2.00>  Found <%s>",
                                pFilePath,
                                wAttribute.toStdString().c_str());
        return ZS_XMLERROR;
        }
    wNode = wNodeRoot.firstChildElement();
    wN= wNode.toElement().tagName(); // debug control of tag name

    if (wNode.isNull())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "Bad xml document <%s>.Empty document no child to <zicm> tag found ",
                                pFilePath);
        return ZS_XMLERROR;
        }


    pFirstNode = wNode;
    return ZS_SUCCESS;
} //_XMLzicmControl
/**
 * @brief ZMasterFile::_XMLLoadAndControl Loads an XML document and Makes all XML controls to have an appropriate <zicm> document
 *
 *  Controls are made on
 *  - xml coherence
 *  - Root node existence
 *  - Root node tag name validity using pRootName
 *  - Root node Element attribute. Optional control : if pRootAttrName is omitted (nullptr), no control is done.
 *
 *  It delivers the ROOT node ELEMENT after having done the controls
 *
 * @param[in] pFilePath     a Cstring pointing to xml file to load and control
 * @param[out] XmlDoc       The XML document content loaded as a return
 * @param[out] pNodeRoot   The First node under the root name to exploit as a return
 * @param[in] pRootName      Root node tag name we are searching for
 * @param[in] pRootAttrName Root node Element attribute name. If nullptr then no control is made.
 * @param[in] pRootAttrValue Root node Element attribute expected value
 * @return
 */
ZStatus
_XMLLoadAndControl(const char* pFilePath,
                   QDomDocument &XmlDoc,
                   QDomNode & wRootNode,
                   const char* pRootName,
                   const char* pRootAttrName,
                   const char* pRootAttrValue,
                   FILE* pOutput=nullptr)
{
uriString wUriPath;
ZDataBuffer wXMLcontent;
ZStatus wSt;

QDomElement wElement;

QString wN;
QString wAttribute;

    wUriPath = pFilePath;

    if (!wUriPath.exists())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTEXIST,
                                    Severity_Error,
                                    " File <%s> does not exist while trying to load it",
                                    pFilePath);
            fprintf(pOutput,
                    "%s>> File <%s> does not exist while trying to load it\n",
                    _GET_FUNCTION_NAME_,
                    pFilePath);
            return ZS_FILENOTEXIST;
            }
    if (pOutput)
            fprintf(pOutput,"%s>> loading xml file %s\n",
                    _GET_FUNCTION_NAME_,
                    pFilePath);

    wSt=wUriPath.loadContent(wXMLcontent);
    if (wSt!=ZS_SUCCESS)
            return wSt;

//QDomDocument XmlDoc;


QString ErrorMsg;
int ErrLine=0;
int ErrColumn=0;
bool Result = true;
bool NameSpaceProcessing = true; //option to be set at a higher level

    XmlDoc.clear();
    Result = XmlDoc.setContent(   wXMLcontent.toQByteArray(),
                                        NameSpaceProcessing,
                                        &ErrorMsg,
                                        &ErrLine,
                                        &ErrColumn) ;

    if (!Result)
                {
                ZException.setComplement(
                        "line %d Column %d Xml error >> %s",
                        ErrLine,
                        ErrColumn,
                        ErrorMsg.toStdString().c_str());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Warning,
                                        " XML Error(s) have been reported : ");
                if (pOutput)
                        ZException.printLastUserMessage(pOutput);
                }// !Result


    wRootNode= XmlDoc.firstChildElement(pRootName);

    if (wRootNode.isNull())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_EMPTYFILE,
                                        Severity_Error,
                                        "In file %s : xml document is empty or errored",
                                        pFilePath);
                return ZS_EMPTYFILE;
                }

    wElement=wRootNode.toElement();
    wN= wElement.tagName();
    if (wN!=pRootName)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "Bad xml document <%s>.Expected first element <%s> not found. Found <%s>",
                                        pFilePath,
                                        pRootName,
                                        wN.toStdString().c_str());
                return ZS_XMLERROR;
                }

    if (pRootAttrName==nullptr)         // if Optional attribute is omitted then no more control
                    return ZS_SUCCESS;

    wAttribute = wElement.attribute(pRootAttrName);
    if (wAttribute!=pRootAttrValue)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "Bad xml document <%s>.Bad root node attribute <%s>.Expected <%s>  Found <%s>",
                                pFilePath,
                                pRootAttrName,
                                pRootAttrValue,
                                wAttribute.toStdString().c_str());
        return ZS_XMLERROR;
        }
    if (pOutput)
            fprintf(pOutput,"%s>> load and control on xml file %s has been done successfully\n",
                    _GET_FUNCTION_NAME_,
                    pFilePath);
    return ZS_SUCCESS;
} //_XMLControl


ZStatus
ZMasterFile::_loadXML_Index(QDomNode &pIndexNode, ZIndexControlBlock* pZICB)
{
ZStatus wSt;
QDomNode wNode;
QString wN;
    wNode = pIndexNode.firstChildElement();
    if (wNode.isNull())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document is corrupted.Expected children to <IndexRank> tag : no child found.");
        return ZS_XMLERROR;
        }
    while (!wNode.isNull())
    {
        while (true)
        {
            wN=wNode.toElement().tagName();
            if (wN=="Rank")
            {
            break; // of no use
            }
            if (wN=="Name")
            {
              pZICB->Name=wNode.toElement().text();
              break;
            }
            if (wN=="Duplicates")
            {
              if (wNode.toElement().text()=="ZST_NODUPLICATES")
                        pZICB->Duplicates = ZST_NODUPLICATES ;
                    else
                        pZICB->Duplicates = ZST_DUPLICATES ;
              break;
            }
            if (wN=="AutoRebuild")
            {
            pZICB->AutoRebuild = (wNode.toElement().text()=="true");
            break;
            }
            if (wN=="ZKDic")
            {
            QDomNode wNodeField = wNode;
            pZICB->ZKDic=new ZKeyDictionary;
            wSt=_loadXMLDictionary(wNodeField,pZICB->ZKDic);
            if (wSt!=ZS_SUCCESS)
                        {
                        return wSt;
                        }
            break;
            }

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "XML error unknown tag name found %s  at line %d ",
                                    wN.toStdString().c_str(),
                                    wNode.lineNumber());
            return ZS_XMLERROR;
        }//while true
    wNode=wNode.nextSiblingElement();
    }// while wNode is not null
   return ZS_SUCCESS;
}// _loadXML_Index

/** @endcond */ // Development

/**
 * @brief ZMasterFile::zloadXML_Index creates a ZIndexControlBlock instance and loads it data from the xml file pointed by pFilePath
 *
 * Created ZIndex
 *
 * @param[in] pFilePath xml file to load
 * @param[out] pZICB the ZIndexControlBlock populated with data contained in xml file
 * @return
 */
ZStatus
ZMasterFile::zloadXML_Index(const char* pFilePath, ZIndexControlBlock &pZICB)
{
ZStatus wSt;
QString wN;
QDomDocument XmlDoc;
QDomNode wNode;
QDomNode wIndexNode;


    wSt=_XMLzicmControl(pFilePath,XmlDoc,wIndexNode);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    if (wIndexNode.toElement().tagName()!="IndexRank")
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_XMLERROR,
                            Severity_Error,
                            "XML error : unknown tag name found %s  at line %d. Tag <IndexRank> was expected",
                            wN.toStdString().c_str(),
                            wNode.lineNumber());
        return ZS_XMLERROR;
        } //  not IndexRank


//    pZICB= new ZIndexControlBlock;

//    while (!wIndexNode.isNull())
//    {
    wNode = wIndexNode;
    wSt=_loadXML_Index(wNode,&pZICB);
    if (wSt!=ZS_SUCCESS)
                {
//                delete pZICB;
                return wSt;
                }
//    wIndexNode=wIndexNode.nextSibling();
//    }// while wNode is not null


    return ZS_SUCCESS;

}//zloadXML_Index

//----------------------Dictionary and fields load---------------------------

/** @cond Development */
ZStatus
ZMasterFile::_loadXMLKeyField(QDomNode& wNode, ZKeyDictionary *&pZKDic)
{
ZStatus wSt;
QString wN;
QDomElement wElement;
    wNode = wNode.firstChildElement();

    if (wNode.isNull())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document is corrupted.Expected children to <KeyField> tag : no child found.");
        return ZS_XMLERROR;
        }


    while (!wNode.isNull())
    {
    while (true)
    {
        wElement=wNode.toElement();

        wN=wNode.toElement().tagName();
        if (wN=="Rank")
        {
        break; // of no use
        }
        if (wN=="Name")
        {
        pZKDic->last().Name = wElement.text();
          break;
        }
        if (wN=="NaturalSize")
        {
          getElementLong (pZKDic->last().NaturalSize ,wElement);
          break;
        }
        if (wN=="UniversalSize")
        {
          getElementLong (pZKDic->last().UniversalSize ,wElement);
          break;
        }
        if (wN=="ArraySize")
        {
          getElementLong (pZKDic->last().ArraySize ,wElement);
          break;
        }
        if (wN=="RecordOffset")
        {
          getElementLong (pZKDic->last().RecordOffset ,wElement);
          break;
        }
        if (wN=="ZType")
        {
            ZDataBuffer wS;
            wS=wElement.text();
          wSt=encodeZTypeFromString (pZKDic->last().ZType ,wS);
          if (wSt!=ZS_SUCCESS)
                    return wSt;

          break;
        }
        if (wN=="KeyField")
        {
        QDomNode wFieldNode;
        wFieldNode = wNode; // to keep sibling potential
        pZKDic->newBlankElement();
        wSt=_loadXMLKeyField(wFieldNode,pZKDic);
        if (wSt!=ZS_SUCCESS)
                {
                pZKDic->pop();
                return wSt;
                }
        break;
        }

        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error unknown tag name found %s  at line %d ",
                                wN.toStdString().c_str(),
                                wNode.lineNumber());
        break;
        }//while true

    wNode=wNode.nextSiblingElement();

    }// while wNode is not null



    return ZS_SUCCESS;
}

ZStatus
ZMasterFile::_loadXMLDictionary(QDomNode& wNode, ZKeyDictionary *pZKDic)
{
ZStatus wSt;
QString wN;
QDomElement wElement;
QDomNode wNodeField;
    wNode = wNode.firstChildElement();

    if (wNode.isNull())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document is corrupted.Expected children to <ZKDic> tag : no child found.");
        return ZS_XMLERROR;
        }


    while (!wNode.isNull())
    {
    while (true)
    {
        wElement=wNode.toElement();

        wN=wNode.toElement().tagName();
        if (wN=="NaturalSize")
        {
          getElementLong (pZKDic->NaturalSize ,wElement);
          break;
        }
        if (wN=="UniversalSize")
        {
          getElementLong (pZKDic->UniversalSize ,wElement);
          break;
        }
        if (wN=="KeyField")
        {
        wNodeField = wNode;
        pZKDic->newBlankElement();
        wSt=_loadXMLKeyField(wNodeField,pZKDic);
        if (wSt!=ZS_SUCCESS)
                {
                pZKDic->pop();
                return wSt;
                }
        break;
        }

        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error unknown tag name found %s  at line %d ",
                                wN.toStdString().c_str(),
                                wNode.lineNumber());
        break;
    }//while true

    wNode=wNode.nextSiblingElement();

    }// while wNode is not null


    return ZS_SUCCESS;
}// loadXMLDictionary

/** @endcond */ // Development

/**
 * @brief ZMasterFile::zloadXML_Dictionary creates a CZKeyDictionary instance and loads it data from the xml file pointed by pFilePath
 * @param[in] pFilePath xml file to load
 * @param[out] pZKDIC the CZKeyDictionary populated with data contained in xml file
 * @return
 */
ZStatus
ZMasterFile::zloadXML_Dictionary(const char* pFilePath, ZKeyDictionary &pZKDIC)
{
ZStatus wSt;
QString wN;
QDomDocument XmlDoc;
QDomNode wNode;
QDomNode wIndexNode;


    wSt=_XMLzicmControl(pFilePath,XmlDoc,wIndexNode);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    wN = wIndexNode.toElement().tagName();
    if (wIndexNode.toElement().tagName()!="ZKDic")
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_XMLERROR,
                            Severity_Error,
                            "XML error : unknown tag name found %s  at line %d. Tag <ZKDic> was expected",
                            wN.toStdString().c_str(),
                            wNode.lineNumber());
        return ZS_XMLERROR;
        } //  not IndexRank


//    pZKDIC= new CZKeyDictionary;

//    wIndexNode = wIndexNode.firstChildElement();       // one single index is loaded
//    while (!wIndexNode.isNull())
//    {
    wNode = wIndexNode; // to preserve wIndexNode context for potential next sibling
    wSt=_loadXMLDictionary(wNode,&pZKDIC);
    if (wSt!=ZS_SUCCESS)
                {
//                delete pZKDIC;
                return wSt;
                }
//    wIndexNode=wIndexNode.nextSibling();
//    }// while wNode is not null


    return ZS_SUCCESS;

}//zloadXML_Dictionary



/*
<?xml version='1.0' encoding='UTF-8'?>
<zicm version="2.00">
<File>
  <ZFileDescriptor>
      <URIContent>/home/gerard/testindexedpictures.zmf</URIContent>
      <URIHeader>/home/gerard/testindexedpictures.zrh</URIHeader> <!-- not modifiable generated by ZRandomFile-->
      <URIDirectoryPath>/home/gerard/</URIDirectoryPath> <!-- not modifiable generated by ZRandomFile-->
      <ZHeaderControlBlock> <!-- not modifiable -->
          <FileType>ZFT_ZMasterFile</FileType>  <!-- do not modify : could cause data loss  see documentation-->
          <ZRFVersion>2000</ZRFVersion> <!-- not modifiable -->
          <OffsetFCB>4698</OffsetFCB> <!-- not modifiable -->
          <OffsetReserved>64</OffsetReserved> <!-- not modifiable -->
          <SizeReserved>4634</SizeReserved> <!-- not modifiable -->
      </ZHeaderControlBlock>
   <ZFileControlBlock>
      <AllocatedBlocks>15</AllocatedBlocks> <!-- not modifiable -->
      <BlockExtentQuota>5</BlockExtentQuota>
      <InitialSize>100000</InitialSize>
      <BlockTargetSize>1</BlockTargetSize>
      <HighwaterMarking>false</HighwaterMarking>
      <GrabFreeSpace>true</GrabFreeSpace>
   </ZFileControlBlock>
  </ZFileDescriptor>
  <ZMasterControlBlock>
         <HistoryOn>false</HistoryOn>
         <JournalingOn>true</JournalingOn>
         <IndexFileDirectoryPath></IndexFileDirectoryPath>
         <!-- Changing IndexRank position in a file description will imply huge modifications -->
         <Index>
         <IndexRank>
           <Rank>0</Rank> <!-- not significant -->
           <Name>First Index</Name> <!-- warning modification of this field is dangerous see documentation -->
           <Duplicates>ZST_NODUPLICATES</Duplicates> <!-- warning modification of this field is dangerous see documentation -->
           <AutoRebuild>false</AutoRebuild>
           <ZKDic>
               <NaturalSize>62</NaturalSize>   <!-- not modifiable -->
               <UniversalSize>65</UniversalSize> <!-- not modifiable -->
              <KeyField>
                <Rank>0</Rank>  <!-- not significant -->
                <Name>String1</Name>
                <NaturalSize>50</NaturalSize>
                <UniversalSize>50</UniversalSize>
                <ArraySize>50</ArraySize>
                <ZType>ZType_Array | ZType_Char</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>0</RecordOffset>
              </KeyField>
              <KeyField>
                <Rank>1</Rank>  <!-- not significant -->
                <Name>NumericField</Name>
                <NaturalSize>12</NaturalSize>
                <UniversalSize>15</UniversalSize>
                <ArraySize>3</ArraySize>
                <ZType>ZType_Array | ZType_Signed | ZType_Endian | ZType_Float</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>52</RecordOffset>
              </KeyField>
          </ZKDic>
         </IndexRank>
         <IndexRank>
           <Rank>1</Rank> <!-- not significant -->
           <Name>Second Index</Name> <!-- warning modification of this field is dangerous see documentation -->
           <Duplicates>ZST_DUPLICATES</Duplicates> <!-- warning modification of this field is dangerous see documentation -->
           <AutoRebuild>false</AutoRebuild>
           <ZKDic>
               <NaturalSize>4</NaturalSize>   <!-- not modifiable -->
               <UniversalSize>5</UniversalSize> <!-- not modifiable -->
              <KeyField>
                <Rank>0</Rank>  <!-- not significant -->
                <Name>Type</Name>
                <NaturalSize>4</NaturalSize>
                <UniversalSize>5</UniversalSize>
                <ArraySize>1</ArraySize>
                <ZType>ZType_Enum| ZType_Signed | ZType_Endian | ZType_S32</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>64</RecordOffset>
              </KeyField>
          </ZKDic>
         </IndexRank>
         </Index>
  </ZMasterControlBlock>
</File>
</zicm>
  */


//} // namespace zbs
/** @cond Development */
/**
 * @brief getChildElementValue gets from Node pNodeWork the content into pContent of a child node Element with name pTagName
 * @param wNodeWork father node to get the child element from
 * @param wTagName tag name of the child node to get the content from
 * @param wContent QString to receive the content of the element
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
getChildElementValue(QDomNode pNodeWork,const char*pTagName,QString &pContent, bool pMandatory)
{
QDomNode wNodeDetail;
QDomElement wElement;
QString wName;

    wNodeDetail = pNodeWork.firstChildElement(pTagName);
    if (wNodeDetail.isNull())
        {
        if (pMandatory)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "xml document may be corrupted or incomplete.Expected <%s> child to <%s> tag : child not found.",
                                    pTagName,
                                    pNodeWork.toElement().tagName().toStdString().c_str());
            return ZS_XMLERROR;
            }
        else
            return ZS_NOTFOUND;
        }
    wElement=wNodeDetail.toElement();
    wName=wNodeDetail.toElement().tagName();
    pContent=wElement.text();
    if (pContent.isEmpty())
        {
        if (pMandatory)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "Warning: xml document may be incomplete.Expected <%s> child to <%s> tag : child content is empty.",
                                    pTagName,
                                    pNodeWork.toElement().tagName().toStdString().c_str());
            return ZS_XMLERROR;
            }
        else
            return ZS_NOTFOUND;
        }
   return ZS_SUCCESS;
} // getChildElementValue

/**
 * @brief _testXMLZFileControlBlock
 * @param pFCB
 * @param pFirstNode
 * @param pMissingTags
 * @param pRealRun
 * @param pOutput
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
_testXMLZFileControlBlock(ZFileControlBlock *pFCB,
                          QDomNode pFirstNode,
                          long &pMissingTags,
                          bool pRealRun,
                          FILE*pOutput)
{
ZStatus wSt;
QString wContent;

QDomNode wNodeWork;
QDomNode wSecondlevelNode;
messageString wMessage;
descString wTagName;
//--------------ZFileControlBlock modifiable fields----------------

        wSecondlevelNode = pFirstNode.firstChildElement("ZFileDescriptor");

        fprintf (pOutput,"%s>> processing <ZFileControlBlock>\n",
                 _GET_FUNCTION_NAME_);

       wNodeWork = wSecondlevelNode.firstChildElement("ZFileControlBlock");
       if (wNodeWork.isNull())
           {
           ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_XMLERROR,
                                   Severity_Error,
                                   "xml document may be corrupted or incomplete.Expected <ZFileControlBlock> child to <File> tag : no child found.");
           return ZS_XMLERROR; // we must explore all document before returning
           }


       wTagName = "BlockExtentQuota";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.clear();
       wMessage.add("       <%s>  ",
                    wTagName.toString());
       if (pFCB->BlockExtentQuota==wContent.toLong())
                   wMessage.add( " <%ld> ==unchanged== \n",
                                 pFCB->BlockExtentQuota);
               else
               {
               wMessage.add(" current <%ld> modified to <%ld>\n",
                            pFCB->BlockExtentQuota,
                            wContent.toLong());
               if (pRealRun)
                    {
                    pFCB->BlockExtentQuota = wContent.toLong();
                    }
               }
       fprintf(pOutput,wMessage.toString());
       }// else field----------------------------------------------------

       wTagName = "InitialSize";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.clear();
       wMessage.add("       <%s>  ",
                    wTagName.toString());
       if (pFCB->InitialSize==wContent.toLong())
                   wMessage.add( " <%ld> ==unchanged== \n",
                                 pFCB->InitialSize);
               else
                {
                   wMessage.add(" current <%ld> modified to <%ld>\n",
                                pFCB->InitialSize,
                                wContent.toLong());
               if (pRealRun)
                    {
                    pFCB->InitialSize = wContent.toLong();
                    }
                }
       fprintf(pOutput,wMessage.toString());
       }// else field----------------------------------------------------

       wTagName = "BlockTargetSize";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.clear();
       wMessage.add("       <%s>  ",
                    wTagName.toString());
       if (pFCB->BlockTargetSize==wContent.toLong())
                   wMessage.add( " <%ld> ==unchanged== \n",
                                 pFCB->BlockTargetSize);
               else
                {
                   wMessage.add(" current <%ld> modified to <%ld>\n",
                                pFCB->BlockTargetSize,
                                wContent.toLong());
               if (pRealRun)
                    {
                    pFCB->BlockTargetSize = wContent.toLong();
                    }
                }
       fprintf(pOutput,wMessage.toString());
       }// else field----------------------------------------------------

       wTagName = "HighwaterMarking";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.clear();
       wMessage.add("       parameter <%s>  ",
                    wTagName.toString());
       if (pFCB->HighwaterMarking==(wContent=="true"))
                   wMessage.add( " <%s> ==unchanged== \n",
                                 pFCB->HighwaterMarking?"true":"false");
               else
                    {
                   wMessage.add(" current <%s> modified to <%s>\n",
                                pFCB->HighwaterMarking?"true":"false",
                                wContent=="true"?"true":"false");
                   if (pRealRun)
                        {
                        pFCB->HighwaterMarking = (wContent=="true");
                        }
                    }
       fprintf(pOutput,wMessage.toString());
       }// else field----------------------------------------------------

       wTagName = "GrabFreeSpace";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.clear();
       wMessage.add("       parameter <%s>  ",
                    wTagName.toString());
       if (pFCB->GrabFreeSpace==(wContent=="true"))
                   wMessage.add( " <%s> ==unchanged== \n",
                                 pFCB->GrabFreeSpace?"true":"false");
               else
                    {
                   wMessage.add(" current <%s> modified to <%s>\n",
                                pFCB->GrabFreeSpace?"true":"false",
                                wContent=="true"?"true":"false");
                   if (pRealRun)
                        {
                        pFCB->GrabFreeSpace = (wContent=="true");
                        }
                    }
       fprintf(pOutput,wMessage.toString());
       }// else field----------------------------------------------------------

    //--------------End ZFileControlBlock modifiable fields----------------
   return ZS_SUCCESS;
}//_testXMLZFileControlBlock

/**
 * @brief _getXMLZFileDescriptor analyze ZFilecontrolBlock from xml definition, controls it, and return a pURIContent
 * @note an error here must end the processing
 *
 *
 * @param[in] pFistNode
 * @param[in] pFilePath     content file path. if omitted, the URIContent field from xml definition will be taken
 * @param[in] pZFile_type   type of file for validating xml definition
 * @param[out] pURIContent  selected content file path
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus _testXMLZFileDescriptor (QDomNode pFistNode,const char *pFilePath,const char*pZFile_type,uriString &pURIContent,FILE* pOutput)
{
ZStatus wSt;

QString wContent;

QDomNode wSecondlevelNode;
QDomNode wThirdlevelNode;

    fprintf (pOutput,"%s>> processing node <ZFileDescriptor>  \n",
             _GET_FUNCTION_NAME_);

    wSecondlevelNode = pFistNode.firstChildElement("ZFileDescriptor");
    if (wSecondlevelNode.isNull())
        {
        fprintf(pOutput,"   ****Error*** Missing <ZFileDescriptor> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <ZFileControlBlock> child to <File> tag : no child found.");
        return(ZS_XMLERROR);
        }// isNull

    wThirdlevelNode = wSecondlevelNode.firstChildElement("ZHeaderControlBlock");
    if (wThirdlevelNode.isNull())
        {
        fprintf(pOutput,"   ****Error*** Missing <ZHeaderControlBlock> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <ZHeaderControlBlock> child to <ZFileDescriptor> tag : no child found.");
        return(ZS_XMLERROR);
        }// isNull

    wSt=getChildElementValue(wThirdlevelNode,"FileType",wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(pOutput,"   ****Error*** Missing <FileType> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <FileType> child to <ZHeaderControlBlock> tag : no child found.");
        return(ZS_XMLERROR);
        }// !ZS_SUCCESS
    if (wContent!=pZFile_type)
        {
        fprintf(pOutput,"   ****Error*** Invalid <FileType> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "Invalid <FileType> must be <%s> found <%s>.",
                                pZFile_type,
                                wContent.toStdString().c_str());
        return(ZS_XMLERROR);
        }


    if (pFilePath==nullptr)
            {
            wSt=getChildElementValue(wSecondlevelNode,"URIContent",wContent);
            if (wSt!=ZS_SUCCESS)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "No file path has been specified and xml file does not contain a valid <URIContent> field.",
                                        wContent.toStdString().c_str());
                                        ZException.exit_abort();
                }
            pURIContent = wContent ;
            }
        else
            {
            pURIContent = pFilePath ;
            }
    return wSt;
}// _testXMLZFileDescriptor
/** @endcond */ // Development

/**
 * @brief ZMasterFile::zapplyXMLIndexRankDefinition gets an index rank (one key) definition from an xml file given by pXMLPath
 *                      and apply it to the ZMasterFile given by pContentFilePath.
 * This is a static method.
 *
 *  The index definition is parsed and validated.
 *
 *  The named index may be suppressed if the tag <Remove> following index name is set to value 'true'.
 *  If no index with the given <Name> tag does not exist within ZMasterFile ZMCB, then it will be created as an index and
 *
 * @param pXMLPath          Path of the xml file containing the index definition.
 * @param pContentFilePath  Path for main content file for the ZMasterFile to apply the modifications to.
 * @param pRealRun          If set to true, then modifications will be applied. If set to false, this is a test run with no modification.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.\n
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.xmllog
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zapplyXMLIndexRankDefinition(const char* pXMLPath,
                                          const char *pContentFilePath,
                                          bool pRealRun,
                                          FILE*pOutput)
{

ZStatus wSt;
uriString URIContent;
uriString wURIOutput;
descString wBase;
FILE* wOutput;
bool FOutput=false;
char    IndexPresence=0;
long    IndexRank=0,
        wAddedIndexes=0 ,
        wDestroyedIndexes = 0,
        wRebuiltIndexes = 0,
        wMissingTags=0 ,
        wMissMandatoryTags=0,
        wIndexProcessed=0;

QDomDocument XmlDoc;
QDomNode wRootNode;
QDomNode wFirstlevelNode;


// 0 : Index to be deleted     1 : Index present    2 : Index added  3 : Index created   4 : Index errored

ZIndexControlBlock  wZICB;
ZMasterFile         wMasterFile;


    wOutput=pOutput;
    if (pOutput==nullptr)
        {
        wURIOutput=pXMLPath;
        wBase=wURIOutput.getBasename();
        wBase+=".xmllog";
        wOutput=fopen(wBase.toString(),"w");
        if (wOutput==nullptr)
                    {
            wOutput=stdout;
            fprintf(wOutput," cannot open file <%s> redirected to stdout\n",wBase.toString());
                    }
        else
            {
             FOutput=true;
            }
        }

    fprintf (wOutput,
             "__________________________________________________________\n");
    fprintf (wOutput,
             "%s>> starting processing xml file <%s>  \n"
             "            Run options are \n"
             "                 Real run or test run <%s>\n"
             "                 File to apply        <%s>\n"
                     ,
             _GET_FUNCTION_NAME_,
             pXMLPath,
             pRealRun?"Real Run":"Test Run",
             pContentFilePath);

    if (pRealRun)
            fprintf(wOutput," Real Run : file structure is to be modified  \n");
        else
            fprintf(wOutput,"***test*** no update is done  \n");

    wSt=_XMLLoadAndControl(pXMLPath,XmlDoc,wRootNode,"zicm","version",__ZRF_XMLVERSION_CONTROL__,wOutput);
    if (wSt!=ZS_SUCCESS)
                {
                return wSt;
                }

    wFirstlevelNode=wRootNode.firstChildElement("IndexRank");

    if (wFirstlevelNode.isNull())
        {
        fprintf (wOutput,
                 "%s>> **** Error : XML error : corrupted or empty file <%s>. Mandatory tag <File> was expected and not found.****",
                 _GET_FUNCTION_NAME_,
                 pXMLPath);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error : corrupted or empty file <%s>. Mandatory tag <File> was expected and not found.",
                                pXMLPath);
            goto ErrorzapplyXMLIndex;
        }
    URIContent = pContentFilePath;

    if (!pRealRun)
        {
        fprintf (wOutput,"%s>> opening ZMasterFile in mode ZRF_Read_Only  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Read_Only);  // for test read is enough
        }
    else
        {
        fprintf (wOutput,"%s>> opening ZMasterFile in mode ZRF_Exclusive | ZRF_Write  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Exclusive | ZRF_All); // must be this way
        }
    if (wSt!=ZS_SUCCESS)
            goto ErrorzapplyXMLIndex;
//-------------------Indexes-----------------------------------------


    while (!wFirstlevelNode.isNull())
        {
//-------------here get index rank and dictionary details and test ------------------------
        fprintf(wOutput,
                "%s>>   getting Index definition\n",
                _GET_FUNCTION_NAME_);

        wIndexProcessed ++;
        wSt= zgetXMLIndexRank(wMasterFile,
                              wZICB,
                              wFirstlevelNode,
                              wMissingTags,
                              wMissMandatoryTags,
                              IndexPresence,
                              IndexRank,
                              wOutput);
        break;

        wFirstlevelNode=wFirstlevelNode.nextSiblingElement("IndexRank");
        IndexRank ++;
        }
BackProcess_zapplyXMLIndex:

    if (wSt!=ZS_SUCCESS)
            goto ErrorzapplyXMLIndex;
    fprintf(wOutput,
            "%s>>     -----------Checking for index to build/rebuild/suppressed-----------------------------\n",
            _GET_FUNCTION_NAME_);

    switch(IndexPresence)
    {
    case 0 :
    {
        fprintf (wOutput,
                 "%s>>          Suppressing Index rank <%ld> <%s> ...",
                 _GET_FUNCTION_NAME_,
                 IndexRank,
                 wZICB.Name.toString());
        if (pRealRun)
            {
             wSt=   wMasterFile.zremoveIndex(IndexRank);
             if (wSt!=ZS_SUCCESS)
                 {
                 fprintf (wOutput,
                          "%s>>  **** Error : an error occurred during index remove. Status is %s. please see error stack dump.",
                          _GET_FUNCTION_NAME_,
                          decode_ZStatus(wSt));
                 goto ErrorzapplyXMLIndex;
                 }
             fprintf (wOutput,
                      " Done\n"
                      "%s>>   Index rank <%ld> <%s> successfully removed.",
                      _GET_FUNCTION_NAME_,
                      IndexRank,
                      wZICB.Name.toString());
            }
            else
            fprintf(wOutput,"\n");
        break;
    }// 0
    case 1 :
    {
        fprintf (wOutput,
                 "%s>>          Nothing to do for Index rank <%ld> <%s>.\n",
                 _GET_FUNCTION_NAME_,
                 IndexRank,
                 wZICB.Name.toString());
        break;
    }
    case 2:
    {
    fprintf(wOutput,
            "%s>>           Index rank <%ld> Name <%s> is to be modified into Master file structure ...",
            _GET_FUNCTION_NAME_,
            IndexRank,
            wMasterFile.ZMCB.Index[IndexRank].Name.toString());

    if (pRealRun)
        {
        wMasterFile.ZMCB.Index[IndexRank].ZKDic->clear();
        wMasterFile.ZMCB.Index[IndexRank].ZKDic->setAllocation(wZICB.ZKDic->size());

        for (long wi = 0;wi< wZICB.ZKDic->size();wi++)
            {
            wMasterFile.ZMCB.Index[IndexRank].ZKDic->push(wZICB.ZKDic->Tab[wi]);
            }//for
        wMasterFile.ZMCB.Index[IndexRank].ZKDic->_reComputeSize();
        fprintf(wOutput, " Done.\n");
        }
        else
            fprintf(wOutput,"\n");

        fprintf(wOutput,
                "%s>>           Index rank <%ld> <%s> : Index control block is to be modified into index file structure ...",
                _GET_FUNCTION_NAME_,
                IndexRank,
                wMasterFile.ZMCB.Index[IndexRank].Name.toString());
        if (pRealRun)
            {
            wMasterFile.ZMCB.IndexObjects[IndexRank]->setICB(&wMasterFile.ZMCB.Index[IndexRank]);
            wSt=wMasterFile.ZMCB.IndexObjects[IndexRank]->writeIndexControlBlock();
            if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>  **** Error : an error occurred during index control block update. Status is %s. please see error stack dump.",
                         _GET_FUNCTION_NAME_,
                         decode_ZStatus(wSt));
                goto ErrorzapplyXMLIndex;
                }
            fprintf (wOutput," Done.\n");

            }
            else
            fprintf (wOutput,"\n");

    fprintf(wOutput,
            "%s>>           Index rank <%ld> Name <%s> is to be rebuilt\n",
            _GET_FUNCTION_NAME_,
            IndexRank,
            wMasterFile.ZMCB.Index[IndexRank].Name.toString());
    if (pRealRun)
        {
         wSt=wMasterFile.zindexRebuild(IndexRank,ZMFStatistics,wOutput);
         if (wSt!=ZS_SUCCESS)
             {
             fprintf(wOutput,
                     "%s>>     ***** Error occurred while rebuilding Index rank <%ld> status is <%s> Aborting\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     decode_ZStatus(wSt));
             goto ErrorzapplyXMLIndex;
             }// ! ZS_SUCCESS
        } // pRealRun
    wRebuiltIndexes ++;
    } // case 2

    case 3:
    {
    fprintf (wOutput,
             "%s>>          Index <%s> is to be created and rebuilt.\n",
             _GET_FUNCTION_NAME_,
             wZICB.Name.toString());
    if (pRealRun)
    {
    wZICB.ZKDic->_reComputeSize();
    wSt=wMasterFile.zcreateIndex(*wZICB.ZKDic,
                                 wZICB.Name.toString(),
                                 wZICB.Duplicates,
                                 false);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,
                "%s>> **** Index creation ended with error. Status is <%s>. Index has been removed from file. ****\n",
                 _GET_FUNCTION_NAME_,
                decode_ZStatus(wSt));
        break;
        }
    fprintf(wOutput,
            "%s>>           Index has been created and rebuilt sucessfully.\n",
             _GET_FUNCTION_NAME_);
    }
    wRebuiltIndexes ++;
    break;
    }// case 3

    case 4: // errored
    {
        break;
    }
    default :
        break;

    }// switch
    wMasterFile.zclose();

EndzapplyXMLIndex:

    //-----------------------Final report------------------------------

        fprintf(wOutput,
                "________________%s Report_______________\n"
                "  Missing tags                     %ld\n"
                "  Mandatory tags missing           %ld\n"
                "  Index(es) definition processed   %ld\n"
                "  Added Index(es)                  %ld\n"
                "  Suppressed Index(es)             %ld\n"
                "  Index(es) rebuilt                %ld\n"
                "______________________________________________________\n",
                _GET_FUNCTION_NAME_,
                wMissingTags,
                wMissMandatoryTags,
                wIndexProcessed,
                wAddedIndexes,
                wDestroyedIndexes,
                wRebuiltIndexes);

        ZException.printUserMessage(wOutput);
        ZException.clearStack();

        if (FOutput)
                fclose(wOutput);

    return wSt;

ErrorzapplyXMLIndex:
    fprintf(wOutput,
            "%s>>  *****process ended with errors*****************\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();
    goto EndzapplyXMLIndex;

}//zapplyXMLIndexRankDefinition

/** @cond Development */
ZStatus
ZMasterFile::zgetXMLIndexRank(ZMasterFile &wMasterFile,
                              ZIndexControlBlock &wZICB,
                              QDomNode pNode,
                              long &wMissingTags,
                              long &wMissMandatoryTags,
                              char &IndexPresence,  // 0 : Index to be deleted     1 : Index present but not changed and not to be rebuilt    2 : Index is changed and to be built or rebuilt   3 : Index is to be created  4 : Index is errored
                              long &IndexRank,
                              FILE *wOutput)
{
ZStatus wSt;
long    wKeyRank = 0;

QString wContent;
QDomNode wFifthlevelNode;
QDomNode wSixthlevelNode;
QDomNode wNodeWork;

messageString wMessage;
descString wTagName;

uint32_t    wZType;
ZDataBuffer wZTypeString;
ZIndexField  wField;
bool FRemove=false;
descString wTagValue;


    wZICB.clear();

    wNodeWork = pNode;

    wTagName = "Name";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s> - stopping process\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Fatal,
                                "xml document may be corrupted or incomplete.Expected mandatory field <Name> child to <IndexRank> tag : no child found.");
        wMissingTags++;
        IndexPresence = 4;
        return ZS_XMLERROR;
        }

//--------Search for index rank by its name

    IndexRank =  wMasterFile.ZMCB.Index.zsearchIndexByName(wContent.toStdString().c_str());

    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    wMessage.add(" value <%s>\n",
                 wContent.toStdString().c_str());
    fprintf(wOutput,wMessage.toString());
    wZICB.Name = wContent;

    if (IndexRank < 0) // not found
            {
            fprintf(wOutput,
                    "%s>>           Index name <%s> is not found and will be added to ZMasterFile index at rank <%ld>\n",
                    _GET_FUNCTION_NAME_,
                    wContent.toStdString().c_str(),
                    wMasterFile.ZMCB.Index.size());

            IndexPresence = 3;      // Index will be created(and rebuilt) and does not have to to be rebuilt - pushed on index presence list
            IndexRank=wMasterFile.ZMCB.Index.size();

            }
            else
            {
            IndexPresence = 1; // so far : Index to be kept as is (see later): need to check fields to suppress however
            wZICB.Name = wMasterFile.ZMCB.Index[IndexRank].Name;
            fprintf(wOutput,
                    "%s>>       Found index name <%s> to be modified at Index rank <%ld> \n",
                    _GET_FUNCTION_NAME_,
                    wZICB.Name.toString(),
                    IndexRank);
            }// else

    fprintf(wOutput,wMessage.toString());
//---------Search for tag Remove --------------------------------------
    wTagName = "Remove";
    wSt=getChildElementValue(pNode,wTagName.toString(),wContent,false); // not mandatory
    if (wSt==ZS_SUCCESS)
        {
        wTagValue = wContent;
        wMessage.clear();
        wMessage.add("%s>>        Found tag <%s> value <%s> \n",
                     _GET_FUNCTION_NAME_,
                     wTagName.toString(),
                     wTagValue.toString());

        FRemove = (wTagValue=="true");
        if (FRemove)
            {
            if (IndexPresence==1)
                {
                wMessage.add("%s>>        It is requested to remove index rank <%ld> <%s> \n"
                             "                  Index definition is no more parsed\n",
                             _GET_FUNCTION_NAME_,
                             IndexRank,
                             wZICB.Name.toString());
                IndexPresence = 0;
                fprintf (wOutput,wMessage.toString());
                return ZS_SUCCESS;
                }
            wMessage.add("%s>>   ****Error : It is requested to remove index rank <%ld> <%s> while this index does not exist in ZMasterFile ****\n"
                         "                  Index definition is no more parsed\n",
                         _GET_FUNCTION_NAME_,
                         IndexRank,
                         wZICB.Name.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Error,
                                    "It is requested to remove index rank <%ld> <%s> while this index does not exist in ZMasterFile\n",
                                    IndexRank,
                                    wZICB.Name.toString());
            fprintf (wOutput,wMessage.toString());
            IndexPresence = 4;
            return ZS_INVOP;
            }
        } // ZS_SUCCESS
//------------ End Tag Remove---------------------------------



//------------------End Index Name------------------------------------------

//--------------------Adding Index---------------------------------
        wTagName = "Duplicates";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> default value <%s> applied\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString(),
                    "ZST_DUPLICATES");
            wMissingTags++;
            }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());

            wZICB.Duplicates = encode_ZST(wContent.toStdString().c_str());

            wMessage.add(" value <%s>\n",
                         decode_ZST(wZICB.Duplicates));// Just to cross-check

            fprintf(wOutput,wMessage.toString());
            }// else---------------------------------------------
        wTagName = "AutoRebuild";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> default value <%s> applied \n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString(),
                    "false");
             wMissingTags++;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",  // field does not exist in file
                         wTagName.toString());
            wMessage.add(" value <%s>\n",
                         wContent.toStdString().c_str());
            fprintf(wOutput,wMessage.toString());
            }// else---------------------------------------------

//----------------ZKDic------------------------------

        wFifthlevelNode = pNode.firstChildElement("ZKDic");

        if (wFifthlevelNode.isNull())
            {
            fprintf(wOutput,
                    "%s>>   ****Error No Dictionary found in Index definition. Rank <%ld> name <%s>. Index definition is corrupted******\n",
                    _GET_FUNCTION_NAME_,
                    IndexRank,
                    wZICB.Name.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** No Dictionary found in Index definition. Rank <%ld> <%s>  **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    IndexRank,
                                    wZICB.Name.toString(),
                                    wTagName.toString()
                                    );
            wMissingTags++;
            wMissMandatoryTags ++;

            IndexPresence = 4;
            return ZS_XMLERROR;
            }// isNull

        wSixthlevelNode = wFifthlevelNode.firstChildElement("KeyField");  // an index must at least have one declared field

        if (wFifthlevelNode.isNull())
            {
            fprintf(wOutput,
                    "%s>>   ****Error No Key field found in Dictionary of Index number <%ld> definition . Index definition is corrupted******\n",
                    _GET_FUNCTION_NAME_,
                    IndexRank);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags ++;

            IndexPresence = 4;
            return ZS_XMLERROR;
            }// isNull

        while (!wSixthlevelNode.isNull())
        {
        memset(&wField,0,sizeof(wField));

        wNodeWork= wSixthlevelNode;

        wTagName = "Name";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>****Warning*** Failed to get parameter <%s>. Field rank <%ld> will miss its name.\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString(),
                    wKeyRank);
             wMissingTags++;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%s>\n",
                         wContent.toStdString().c_str());
            fprintf(wOutput,wMessage.toString());

            wField.Name = wContent;

            }// else-----------------------------------------------
        wTagName = "NaturalSize";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld>\n",
                         wContent.toLong());
            fprintf(wOutput,wMessage.toString());

            wField.NaturalSize = wContent.toLong();
            }// else-----------------------------------------------
        wTagName = "UniversalSize";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;

            IndexPresence = 4;
            return ZS_XMLERROR;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld>\n",
                         wContent.toLong());
            fprintf(wOutput,wMessage.toString());

            wField.UniversalSize = wContent.toLong();
            }// else-----------------------------------------------
        wTagName = "ArraySize";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;

            IndexPresence = 4;
            return ZS_XMLERROR;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld>\n",
                         wContent.toLong());
            fprintf(wOutput,wMessage.toString());

            wField.ArraySize = wContent.toLong();
            }// else-----------------------------------------------

        wTagName = "RecordOffset";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;

            IndexPresence = 4;
            return ZS_XMLERROR;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld>\n",
                         wContent.toLong());
            fprintf(wOutput,wMessage.toString());

            wField.RecordOffset = wContent.toLong();
            }// else-----------------------------------------------

        wTagName = "ZType";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;

            IndexPresence = 4;
            return ZS_XMLERROR;
             }
            else
            {
           //---------encode ZType to test it----------------
            wZTypeString= wContent;
            wSt = encodeZTypeFromString(wZType,wZTypeString);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** parameter <%s> is errored with status <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString(),
                        decode_ZStatus(wSt));
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        wSt,
                                        Severity_Error,
                                        "**** **** parameter <%s> is errored with status <%s> Index will not be built for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        decode_ZStatus(wSt),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;

                IndexPresence = 4;
                return ZS_XMLERROR;
                }
                else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld> <%s>\n",
                         wZType,
                         decode_ZType(wZType));
            fprintf(wOutput,wMessage.toString());

            wField.ZType = wZType;

            }// else
            }// else-----------------------------------------------

        wZICB.ZKDic->push(wField);

        wSixthlevelNode=wSixthlevelNode.nextSiblingElement("KeyField");
        }// ! wSixthlevelNode.isNull

//--------------- End ZKDic--------------------------------------------------------
//--------------------Index Add------------------------------------------------------

    if (IndexPresence == 3)     // Creation then return
                    return wSt;
 //--------Search for first difference in index

    if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->size()!=wZICB.ZKDic->size())
        {
        fprintf (wOutput,
                 "%s>>          Index has been modified and must be rebuilt\n",
                 _GET_FUNCTION_NAME_);
        IndexPresence = 2;
        return wSt;
        }
    for (long wi=0; wi < wZICB.ZKDic->size();wi++)
    {
        if (memcmp(&wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wi],
                   &wZICB.ZKDic->Tab[wi],
                   sizeof(ZIndexField))!=0)
                    {
                    fprintf (wOutput,
                             "%s>>          Index has been modified and must be rebuilt\n",
                             _GET_FUNCTION_NAME_);
                    IndexPresence =2;
                    return wSt;
                    }
    }//for
    fprintf (wOutput,
             "%s>>          Index has not been modified and do not need to be rebuilt\n",
             _GET_FUNCTION_NAME_);
    return wSt;
}//zgetXMLIndexRank
/** @endcond */ // Development

/**
 * @brief ZMasterFile::zapplyXMLFileDefinition Static method that tests or applies an xml file give by pFilePath to change parameters of an existing file
 * @param[in] pXMLPath         a valid file path pointing to an xml file full definition
 * @param[in] pContentFilePath the ZMasterFile file path to generate the definition for. if omitted (nullptr) then the field <URIContent> from XML definition file will be taken as file path.
 * @param[in] pRealRun         if set to true, then modifications from xml file will be applied to the file.\n
 *                             if set to false, then only a test run with report will be done to evaluate the possible changes.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.\n
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.xmllog
 * @return
 */
ZStatus
ZMasterFile::zapplyXMLFileDefinition(const char* pXMLPath,
                                    const char *pContentFilePath,
                                    bool pRealRun,
                                    FILE*pOutput)
{
ZStatus wSt;
uriString URIContent;
uriString wURIOutput;
descString wBase;
FILE* wOutput;
bool FOutput=false;

long    IndexRank=0,
        wAddedIndexes=0 ,
        wDestroyedIndexes = 0,
        wRebuiltIndexes = 0,
        wMissingTags=0 ,
        wMissMandatoryTags=0,
        wIndexProcessed=0;

QString wContent;
QDomDocument XmlDoc;
QDomNode wRootNode;
QDomNode wFirstlevelNode;
QDomNode wSecondlevelNode;
QDomNode wThirdlevelNode;
QDomNode wFourthlevelNode;
QDomNode wNodeWork;

messageString wMessage;
descString wTagName;

// 0 : Index to be deleted     1 : Index present but not to be rebuilt    2 : Index to be built or rebuilt -  3 : Index created (therefore rebuilt)
ZArray<char> IndexPresence ;
char wIndexPresence;

ZIndexControlBlock  wZICB;
ZKeyDictionary     wZKDic;
ZMasterFile         wMasterFile;


    wOutput=pOutput;
    if (pOutput==nullptr)
        {
        wURIOutput=pXMLPath;
        wBase=wURIOutput.getBasename();
        wBase+=".xmllog";
        wOutput=fopen(wBase.toString(),"w");
        if (wOutput==nullptr)
                    {
            wOutput=stdout;
            fprintf(wOutput," cannot open file <%s> redirected to stdout\n",wBase.toString());
                    }
        else
        {
            FOutput=true;
         }
        }

    fprintf (wOutput,
             "__________________________________________________________\n");
    fprintf (wOutput,
             "%s>> starting processing xml file <%s>  \n"
             "            Run options are \n"
             "                 Real run or test run <%s>\n"
             "                 File to apply        <%s>\n"
                     ,
             _GET_FUNCTION_NAME_,
             pXMLPath,
             pRealRun?"Real Run":"Test Run",
             pContentFilePath==nullptr?"to be found in xml file":pContentFilePath);

    if (pRealRun)
            fprintf(wOutput," Real Run : file structure is to be modified  \n");
        else
            fprintf(wOutput,"***test*** no update is done  \n");

    wSt=_XMLLoadAndControl(pXMLPath,XmlDoc,wRootNode,"zicm","version",__ZRF_XMLVERSION_CONTROL__,wOutput);
    if (wSt!=ZS_SUCCESS)
                {
                goto ErrorzapplyXMLFile;
                }

    wFirstlevelNode=wRootNode.firstChildElement("File");

    if (wFirstlevelNode.isNull())
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error : corrupted or empty file <%s>. Mandatory tag <File> was expected and not found.",
                                pXMLPath);
            goto ErrorzapplyXMLFile;
        }

//---------ZRandomFile Content (ZFileDescriptor)------------------------

    wSt=_testXMLZFileDescriptor(wFirstlevelNode,pContentFilePath,"ZFT_ZMasterFile",URIContent,wOutput);

    fprintf (wOutput,"%s>> starting processing ZMasterFile <%s>   \n",
             _GET_FUNCTION_NAME_,
             URIContent.toString());


    if (!pRealRun)
        {
        fprintf (wOutput,"%s>> opening ZMasterFile in mode ZRF_Read_Only  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Read_Only);  // for test read is enough
        }
    else
        {
        fprintf (wOutput,"%s>> opening ZMasterFile in mode ZRF_Exclusive | ZRF_Write  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Exclusive | ZRF_All); // must be this way
        }

     if (wSt!=ZS_SUCCESS)
             {
             ZException.addToLast(" While applying xml modifications.");
             goto EndzapplyXMLFile;
             }
     fprintf (wOutput,"%s>> file <%s> successfully opened in ZRF_Read_only mode\n",
              _GET_FUNCTION_NAME_,
              URIContent.toString());

//-------------Control FileType------------------------------------

    if (wMasterFile.ZDescriptor.ZHeader.FileType!=ZFT_ZMasterFile)
            {

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "File type is <%s> while expected <ZFT_ZMasterFile>.",
                                    decode_ZFile_type(wMasterFile.ZDescriptor.ZHeader.FileType));

            goto ErrorzapplyXMLFile;
            }

//--------------ZFileControlBlock modifiable fields----------------

    wSt=_testXMLZFileControlBlock(wMasterFile.ZDescriptor.ZFCB,wFirstlevelNode,wMissingTags,pRealRun, wOutput);
    if (wSt!=ZS_SUCCESS)
            goto ErrorzapplyXMLFile;

 //----------------ZMasterControlBlock modifiable fields----------------
    fprintf (wOutput,"%s>> processing node <ZMasterControlBlock>  \n",
             _GET_FUNCTION_NAME_);

    wSecondlevelNode = wFirstlevelNode.firstChildElement("ZMasterControlBlock");
    if (wSecondlevelNode.isNull())
        {
        wMasterFile.zclose();
        if (FOutput)
                fclose(wOutput);
        fprintf(wOutput,"   ****Error*** Missing <ZMasterControlBlock> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <ZMasterControlBlock> child to <File> tag : no child found.");
        goto ErrorzapplyXMLFile;
        }// isNull
    wNodeWork = wSecondlevelNode;

    wTagName = "HistoryOn";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. It will remain unchanged\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.HistoryOn==(wContent=="true"))
                wMessage.add( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.HistoryOn?"true":"false");
            else
                {
                wMessage.add(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.HistoryOn?"true":"false",
                             wContent=="true"?"true":"false");
                if (pRealRun)
                     {
                     wMasterFile.ZMCB.HistoryOn = (wContent=="true");
                     }
                }
    fprintf(wOutput,wMessage.toString());
    }// else field----------------------------------------------------

    wTagName = "IndexFileDirectoryPath";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent,false); // not mandatory tag
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>Warning: Failed to get optional parameter <%s>. It will remain unchanged.\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.IndexFilePath.toQString()==wContent)
                wMessage.add( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.IndexFilePath.toString());
            else
                {
                wMessage.add(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.IndexFilePath.toString(),
                             wContent.toStdString().c_str());
                if (pRealRun)
                     {
                     wMasterFile.ZMCB.IndexFilePath = wContent;
                     }
                }
    fprintf(wOutput,wMessage.toString());
    }// else field----------------------------------------------------

//---------------------------Journaling---------------------------------
/*
    <Journaling>
      <!--	   <JournalingOn>true</JournalingOn>  -->
      <JournalLocalDirectoryPath></JournalLocalDirectoryPath> <!--null: same directory as ZMasterFile's path  -->
      <JournalingMaxDepth>-1</JournalingMaxDepth> <!-- max number of journaling records to keep. -1 : infinite. in this case need to use purgeJournaling(keepranks) regularly -->
      <JournalRemoteMirroring>
        <JournalHostAddressType>ZHAT_IPV4</JournalHostAddressType> <!-- values ZHAT_IPV4 ZHAT_IPV6-->
        <JournalHostPort> 1156 </JournalHostPort>
        <JournalHostName>JournalServer</JournalHostName>
        <JournalProtocol>ZJP_RPC|ZJP_SSLV5</JournalProtocol> <!--   -->
        <JournalAuthenticate> <JournalAuthenticate>
        <JournalSSLKey>  </JournalSSLKey>
        <JournalSSLVector> </JournalSSLVector>
      </JournalRemoteMirroring>
    </Journaling>
*/
    fprintf (wOutput,"%s>> processing node <Journaling>  \n",
             _GET_FUNCTION_NAME_);


    if (wMasterFile.ZMCB.ZJCB!=nullptr)
            {
        fprintf (wOutput,"%s>> Journaling will be stopped for this file  \n",
                 _GET_FUNCTION_NAME_);
            }
        else
            {
            fprintf (wOutput,"%s>> Journaling data structures will be created for this file  \n",
                     _GET_FUNCTION_NAME_);
            }
    while (true)
    {
    wThirdlevelNode = wSecondlevelNode.firstChildElement("Journaling");
    if (wThirdlevelNode.isNull())
        {
        wMasterFile.zclose();
        if (FOutput)
                fclose(wOutput);
        fprintf(wOutput,"   ****Warning*** No <Journaling> definition for this file******\n Journaling is disabled.\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Warning,
                                "Expected <Journaling> child to <File> tag : no child found. Journaling will not be enabled -or disabled- for this file.");
        break; // out of loop while true
        }// isNull

    ZJCBOwnData wJCB;
    wNodeWork = wThirdlevelNode;

    wTagName = "JournalLocalDirectoryPath";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s>.JournalLocalDirectoryPath will be set to ZMasterFile's default directory path\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    wJCB.JournalLocalDirectoryPath=wContent;
    if (wMasterFile.ZMCB.ZJCB!=nullptr)
                {
        wMessage.add(" to be created with value <%s>\n",
                     wJCB.JournalLocalDirectoryPath.toString());
                }
        else
                {
    if (wMasterFile.ZMCB.ZJCB->JournalLocalDirectoryPath== wJCB.JournalLocalDirectoryPath)
                wMessage.add( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.ZJCB->JournalLocalDirectoryPath.toString());
            else
                {
                wMessage.add(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.ZJCB->JournalLocalDirectoryPath.toString(),
                             wJCB.JournalLocalDirectoryPath.toString());

                } // else unchanged
                } // else created
    fprintf(wOutput,wMessage.toString());
    }// else field----------------------------------------------------
    wTagName = "JournalKeep";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>Warning: Failed to get optional parameter <%s>. It will remain unchanged or will be set to its default value (-1).\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wJCB.Keep=wContent.toLong();
    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.ZJCB!=nullptr)
                {
        wMessage.add(" to be created with value <%ld>\n",
                     wJCB.Keep);
                }
        else
        {
    if (wMasterFile.ZMCB.ZJCB->Keep==wJCB.Keep)
                wMessage.add( " <%ld> ==unchanged== \n",
                              wMasterFile.ZMCB.ZJCB->Keep);
            else
                {
                wMessage.add(" current <%ld> modified to <%ld>\n",
                             wMasterFile.ZMCB.ZJCB->Keep,
                             wJCB.Keep);
                } // else unchanged
                } // else created
    fprintf(wOutput,wMessage.toString());
    }// else field----------------------------------------------------




//-----------------remote mirroring parameters-------------------------
    ZRemoteMirroring* wJRM=nullptr;
    fprintf (wOutput,"%s>> getting remote journaling parameters (if any)\n",
             _GET_FUNCTION_NAME_);

    while (true)
    {
    wFourthlevelNode = wThirdlevelNode.firstChildElement("JournalRemoteMirroring");
    if (wFourthlevelNode.isNull())
        {
        wMasterFile.zclose();
        if (FOutput)
                fclose(wOutput);
        fprintf(wOutput,"   ****Warning*** No <JournalRemoteMirroring> definition for this file******\n Remote Journaling is disabled.\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Warning,
                                "Expected <JournalRemoteMirroring> child to <File> tag : no child found. Remote Journaling will not be enabled -or disabled- for this file.");
        break; // out of loop while true
        }// isNull

    wJRM=new ZRemoteMirroring;

    wTagName = "JournalHostAddressType";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>Warning: Failed to get optional parameter <%s>. It will remain unchanged or will be set to its default value (-1).\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wJRM->AddressType.encode(wContent);
    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.ZJCB->Remote==nullptr)
                {
        wMessage.add(" to be created with value <%s>\n",
                     wJRM->AddressType.decode());
                }
        else
        {
    if (wMasterFile.ZMCB.ZJCB->Remote->AddressType==wJRM->AddressType)
                wMessage.add( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.ZJCB->Remote->AddressType.decode());
            else
                {
                wMessage.add(" current <%ld> modified to <%s>\n",
                             wMasterFile.ZMCB.ZJCB->Remote->AddressType.decode(),
                             wJRM->AddressType.decode());
                } // else unchanged
                } // else created
    fprintf(wOutput,wMessage.toString());
    }// else field----------------------------------------------------



    }// while true remote
//----------------end remote mirroring---------------------------------



//---------------Setting Local Journaling Values-----------------------

     if (pRealRun)
     {
     if (wMasterFile.ZMCB.ZJCB!=nullptr)
             {
         fprintf (wOutput,"%s>> stopping journaling for this file  \n",
                  _GET_FUNCTION_NAME_);
             wMasterFile.setJournalingOff();
 //            delete wMasterFile.ZMCB.ZJCB ;
             }
         else
             {
             fprintf (wOutput,"%s>> creating journaling data structures for this file  \n",
                      _GET_FUNCTION_NAME_);
             wMasterFile.ZMCB.ZJCB = new ZJournalControlBlock;
             wMasterFile.ZMCB.ZJCB->Journal = new ZJournal(&wMasterFile);
             }
     fprintf (wOutput,"%s>> setting journaling values  \n",
              _GET_FUNCTION_NAME_);

     memmove(wMasterFile.ZMCB.ZJCB,&wJCB,sizeof(wJCB));
     wSt=wMasterFile.writeControlBlocks();
     if (wSt!=ZS_SUCCESS)
             {
             fprintf (wOutput,"    ***** Error setting journaling values **** \n");
             ZException.addToLast("while writting journaling data in file header");
             goto ErrorzapplyXMLFile;
             }
     fprintf (wOutput,"%s>> starting local journaling \n",
              _GET_FUNCTION_NAME_);
     wSt= wMasterFile.setJournalingOn();
     if (wSt!=ZS_SUCCESS)
             {
             fprintf (wOutput,"    ***** Error starting journaling **** \n");
             ZException.addToLast("while starting journaling for file");
             goto ErrorzapplyXMLFile;
             }

     }// pRealRun

    break;
    }// while true journaling
//-------------------End Journaling------------------------------------



//-------------------Indexes-----------------------------------------

    fprintf (wOutput,"%s>> processing node <%s>  \n",
             _GET_FUNCTION_NAME_,
             "Index");

    wZICB.clear();
    wZKDic.clear();

    IndexPresence.allocateCurrentElements(wMasterFile.ZMCB.Index.size());
    IndexPresence.bzero(); // equivalent to clearData

    wThirdlevelNode = wSecondlevelNode.firstChildElement("Index");

    if (wThirdlevelNode.isNull())  // this could be the case with 'Super ZRandomFile' (ZRandomFile + Journaling options)
        {
        fprintf(wOutput,"%s>>   ****Warning No Index found in definition ******\n",
                _GET_FUNCTION_NAME_);
        if (wMasterFile.ZMCB.Index.size()>0)
                {
         fprintf(wOutput,"%s>>    ****Found <%ld> existing indexes in file header : they will ALL be deleted  ******\n",
                 _GET_FUNCTION_NAME_,
                 wMasterFile.ZMCB.Index.size());
         wMissingTags++;
                }
        goto BackProcess_zapplyXMLFile;
        }// isNull

    IndexRank = 0; // xml index rank

    wFourthlevelNode = wThirdlevelNode.firstChildElement("IndexRank");

    if (wFourthlevelNode.isNull())
        {
        fprintf(wOutput,"%s>>   ****Error No Index rank found in Index number <%ld> definition . Index definition is corrupted******\n",
                _GET_FUNCTION_NAME_, IndexRank);
        wMissingTags++;
        wMissMandatoryTags ++;
        }// isNull


    while (!wFourthlevelNode.isNull())
        {
//-------------here get index rank and dictionary details and test ------------------------
        fprintf(wOutput,
                "%s>>   getting Index definition \n",
                _GET_FUNCTION_NAME_);

        wIndexProcessed++;

        wSt= zgetXMLIndexRank(wMasterFile,
                              wZICB,
                              wFourthlevelNode,
                              wMissingTags,
                              wMissMandatoryTags,
                              wIndexPresence,
                              IndexRank,
                              wOutput);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get index definition  - stopping process\n",
                    _GET_FUNCTION_NAME_);

            goto ErrorzapplyXMLFile;
            }

        if (wSt!=ZS_SUCCESS)
                goto ErrorzapplyXMLFile;
        fprintf(wOutput,
                "%s>>     -----------Checking for index to build/rebuild/suppressed-----------------------------\n",
                _GET_FUNCTION_NAME_);

        switch(wIndexPresence)
        {
        case 0 :
        {
            fprintf (wOutput,
                     "%s>>          Suppressing Index rank <%ld> <%s> ...",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     wZICB.Name.toString());
            if (pRealRun)
                {
                 wSt=   wMasterFile.zremoveIndex(IndexRank);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf (wOutput,
                              "%s>>  **** Error : an error occurred during index remove. Status is %s. please see error stack dump.",
                              _GET_FUNCTION_NAME_,
                              decode_ZStatus(wSt));
                     goto ErrorzapplyXMLFile;
                     }
                 fprintf (wOutput,
                          " Done\n"
                          "%s>>   Index rank <%ld> <%s> successfully removed.",
                          _GET_FUNCTION_NAME_,
                          IndexRank,
                          wZICB.Name.toString());

                 IndexPresence.erase(IndexRank);
                }
                else
                fprintf(wOutput,"\n");
            break;
        }// 0
        case 1 :
        {
            fprintf (wOutput,
                     "%s>>          Nothing to do for Index rank <%ld> <%s>.\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     wZICB.Name.toString());
            IndexPresence[IndexRank] = 1 ;
            break;
        }
        case 2:
        {
        fprintf(wOutput,
                "%s>>           Index rank <%ld> Name <%s> is to be modified into Master file structure ...",
                _GET_FUNCTION_NAME_,
                IndexRank,
                wMasterFile.ZMCB.Index[IndexRank].Name.toString());

        IndexPresence[IndexRank] = 2 ; // index modified to be rebuilt

        if (pRealRun)
            {
            wMasterFile.ZMCB.Index[IndexRank].ZKDic->clear();
            wMasterFile.ZMCB.Index[IndexRank].ZKDic->setAllocation(wZICB.ZKDic->size());

            for (long wi = 0;wi< wZICB.ZKDic->size();wi++)
                {
                wMasterFile.ZMCB.Index[IndexRank].ZKDic->push(wZICB.ZKDic->Tab[wi]);
                }//for
            wMasterFile.ZMCB.Index[IndexRank].ZKDic->_reComputeSize();
            fprintf(wOutput, " Done.\n");
            }
            else
                fprintf(wOutput,"\n");

            fprintf(wOutput,
                    "%s>>           Index rank <%ld> <%s> : Index control block is to be modified into index file structure ...",
                    _GET_FUNCTION_NAME_,
                    IndexRank,
                    wMasterFile.ZMCB.Index[IndexRank].Name.toString());
            if (pRealRun)
                {
                wMasterFile.ZMCB.IndexObjects[IndexRank]->setICB(&wMasterFile.ZMCB.Index[IndexRank]);
                wSt=wMasterFile.ZMCB.IndexObjects[IndexRank]->writeIndexControlBlock();
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf (wOutput,
                             "%s>>  **** Error : an error occurred during index control block update. Status is %s. please see error stack dump.",
                             _GET_FUNCTION_NAME_,
                             decode_ZStatus(wSt));
                    goto ErrorzapplyXMLFile;
                    }
                fprintf (wOutput," Done.\n");

                }
                else
                fprintf (wOutput,"\n");

        fprintf(wOutput,
                "%s>>           Index rank <%ld> Name <%s> is to be rebuilt\n",
                _GET_FUNCTION_NAME_,
                IndexRank,
                wMasterFile.ZMCB.Index[IndexRank].Name.toString());
        if (pRealRun)
            {
             wSt=wMasterFile.zindexRebuild(IndexRank,ZMFStatistics,wOutput);
             if (wSt!=ZS_SUCCESS)
                 {
                 fprintf(wOutput,
                         "%s>>     ***** Error occurred while rebuilding Index rank <%ld> status is <%s> Aborting\n",
                         _GET_FUNCTION_NAME_,
                         IndexRank,
                         decode_ZStatus(wSt));
                 goto ErrorzapplyXMLFile;
                 }// ! ZS_SUCCESS
            } // pRealRun
        wRebuiltIndexes ++;
        } // case 2

        case 3: // Index rank creation
        {
        fprintf (wOutput,
                 "%s>>          Index <%s> is to be created and rebuilt.\n",
                 _GET_FUNCTION_NAME_,
                 wZICB.Name.toString());


        IndexPresence.push(3);
        if (pRealRun)
        {
        wZICB.ZKDic->_reComputeSize();
        wSt=wMasterFile.zcreateIndex(*wZICB.ZKDic,
                                     wZICB.Name.toString(),
                                     wZICB.Duplicates,
                                     false);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,
                    "%s>> **** Index creation ended with error. Status is <%s>. Index has been removed from file. ****\n",
                     _GET_FUNCTION_NAME_,
                    decode_ZStatus(wSt));
            break;
            }
        fprintf(wOutput,
                "%s>>           Index has been created and rebuilt sucessfully.\n",
                 _GET_FUNCTION_NAME_);

        IndexPresence.last() = 1 ; // rebuilt is done nothing more to do there
        }// pRealRun

        wRebuiltIndexes ++;
        break;
        }// case 3

        case 4: // errored
        {
            fprintf(wOutput,
                    "%s>> **** Index definition for rank <%ld> is errored. Status is <%s>. Index definition is ignored. ****\n",
                     _GET_FUNCTION_NAME_,
                    IndexRank,
                    decode_ZStatus(wSt));
            break;
        }
        default :
            break;

        }// switch


        wFourthlevelNode=wFourthlevelNode.nextSiblingElement("IndexRank");
        }// while !isNull()
//----------------End ZMasterControlBlock modifiable fields----------------

BackProcess_zapplyXMLFile:
//-------------Checking for index to suppress----------------------
    fprintf(wOutput,
            "%s>>     -----------Checking for index to suppress-----------------------------\n",
            _GET_FUNCTION_NAME_);

    for (long wi = 0;wi<IndexPresence.size();wi++)
    {
        if (!IndexPresence[wi])
        {
            fprintf(wOutput,
                    "%s>>     Index rank <%ld> Name <%s> is to be deleted\n",
                    _GET_FUNCTION_NAME_,
                    wi,
                    wMasterFile.ZMCB.Index[wi].Name.toString());
            wDestroyedIndexes ++;

            if (pRealRun)
                {
                 wSt=wMasterFile.zremoveIndex(wi);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf(wOutput,
                             "%s>>     ***** Error occurred while removing Index rank <%ld> status is <%s>\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             decode_ZStatus(wSt));
                     }// ! ZS_SUCCESS
                 fprintf(wOutput,
                         "%s>>     Index rank <%ld> has been deleted\n",
                         _GET_FUNCTION_NAME_,
                         wi);
                 IndexPresence.erase(wi); // remove the index flag from table
                 wi--;
                } // pRealRun
        }// IndexPresence
    }// for


    fprintf(wOutput,
            "%s>>     -----------End processing index(es) to suppress------------------------\n",
            _GET_FUNCTION_NAME_);

    fprintf(wOutput,
            "%s>>     -----------Checking for index to build/rebuild-----------------------------\n",
            _GET_FUNCTION_NAME_);

    for (long wi = 0;wi<IndexPresence.size();wi++)
    {
        if (IndexPresence[wi] == 2)
        {
            fprintf(wOutput,
                    "%s>>     Index rank <%ld> Name <%s> is to be rebuilt\n",
                    _GET_FUNCTION_NAME_,
                    wi,
                    wMasterFile.ZMCB.Index[wi].Name.toString());
            if (pRealRun)
                {
                 wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf(wOutput,
                             "%s>>     ***** Error occurred while rebuilding Index rank <%ld> status is <%s> Aborting\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             decode_ZStatus(wSt));
                     goto ErrorzapplyXMLFile;
                     }// ! ZS_SUCCESS
                } // pRealRun
            wRebuiltIndexes ++;
        }
    }

    fprintf(wOutput,
            "%s>>     -----------End processing index(es) to rebuild------------------------\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();

EndzapplyXMLFile:

    //-----------------------Final report------------------------------

    fprintf(wOutput,
            "________________%s Report_______________\n"
            "  Missing tags                     %ld\n"
            "  Mandatory tags missing           %ld\n"
            "  Index(es) definition processed   %ld\n"
            "  Added Index(es)                  %ld\n"
            "  Suppressed Index(es)             %ld\n"
            "  Index(es) rebuilt                %ld\n"
            "______________________________________________________\n",
            _GET_FUNCTION_NAME_,
            wMissingTags,
            wMissMandatoryTags,
            wIndexProcessed,
            wAddedIndexes,
            wDestroyedIndexes,
            wRebuiltIndexes);

    ZException.printUserMessage(wOutput);
    ZException.clearStack();

    if (FOutput)
            fclose(wOutput);

    return wSt;

ErrorzapplyXMLFile:
    fprintf(wOutput,
            "%s>>  *****process ended with errors*****************\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();
    goto EndzapplyXMLFile;

}// zapplyXMLFileDefinition


/**
 * @brief ZMasterFile::zapplyXMLFileDefinition Static method that tests or applies an xml file give by pFilePath to change parameters of an existing file
 * @param[in] pXMLPath         a valid file path pointing to an xml file full definition
 * @param[in] pContentFilePath the ZMasterFile file path to generate the definition for. if omitted (nullptr) then the field <URIContent> from XML definition file will be taken as file path.
 * @param[in] pRealRun         if set to true, then modifications from xml file will be applied to the file.\n
 *                             if set to false, then only a test run with report will be done to evaluate the possible changes.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.\n
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.xmllog
 * @return
 */
ZStatus
ZMasterFile::zapplyXMLFileDefinition_old(const char* pXMLPath,
                                    const char *pContentFilePath,
                                    bool pRealRun,
                                    FILE*pOutput)
{
ZStatus wSt;
uriString URIContent;
uriString wURIOutput;
descString wBase;
FILE* wOutput;
bool FOutput=false;

long    IndexRank=0,
        wKeyRank=0,
        wAddedIndexes=0 ,
        wDestroyedIndexes = 0,
        wRebuiltIndexes = 0,
        wMissingTags=0 ,
        wMissMandatoryTags=0;

QString wContent;
QDomDocument XmlDoc;
QDomNode wRootNode;
QDomNode wFirstlevelNode;
QDomNode wSecondlevelNode;
QDomNode wThirdlevelNode;
QDomNode wFourthlevelNode;
QDomNode wFifthlevelNode;
QDomNode wSixthlevelNode;
QDomNode wNodeWork;

messageString wMessage;
descString wTagName;

// 0 : Index to be deleted     1 : Index present but not to be rebuilt    2 : Index to be built or rebuilt -  3 : Index created (therefore rebuilt)
ZArray<char> IndexPresence ;

bool        FAddIndex = false;
bool        FAddField = false;
bool        FIndexRebuild = false;
uint32_t    wZType;
ZDataBuffer wZTypeString;
ZIndexControlBlock  wZICB;
ZKeyDictionary     wZKDic;
ZIndexField  wField;
ZMasterFile         wMasterFile;


    wOutput=pOutput;
    if (pOutput==nullptr)
        {
        wURIOutput=pXMLPath;
        wBase=wURIOutput.getBasename();
        wBase+=".xmllog";
        wOutput=fopen(wBase.toString(),"w");
        if (wOutput==nullptr)
                    {
            wOutput=stdout;
            fprintf(wOutput," cannot open file <%s> redirected to stdout\n",wBase.toString());
                    }
        else
        {
            FOutput=true;
         }
        }

    fprintf (wOutput,
             "__________________________________________________________\n");
    fprintf (wOutput,
             "%s>> starting processing xml file <%s>  \n"
             "            Run options are \n"
             "                 Real run or test run <%s>\n"
             "                 File to apply        <%s>\n"
                     ,
             _GET_FUNCTION_NAME_,
             pXMLPath,
             pRealRun?"Real Run":"Test Run",
             pContentFilePath==nullptr?"to be found in xml file":pContentFilePath);

    if (pRealRun)
            fprintf(wOutput," Real Run : file structure is to be modified  \n");
        else
            fprintf(wOutput,"***test*** no update is done  \n");

    wSt=_XMLLoadAndControl(pXMLPath,XmlDoc,wRootNode,"zicm","version",__ZRF_XMLVERSION_CONTROL__,wOutput);
    if (wSt!=ZS_SUCCESS)
                {
                goto ErrorzapplyXMLFile;
                }

    wFirstlevelNode=wRootNode.firstChildElement("File");

    if (wFirstlevelNode.isNull())
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error : corrupted or empty file <%s>. Mandatory tag <File> was expected and not found.",
                                pXMLPath);
            goto ErrorzapplyXMLFile;
        }

//---------ZRandomFile Content (ZFileDescriptor)------------------------

    wSt=_testXMLZFileDescriptor(wFirstlevelNode,pContentFilePath,"ZFT_ZMasterFile",URIContent,wOutput);

    fprintf (wOutput,"%s>> starting processing ZMasterFile <%s>   \n",
             _GET_FUNCTION_NAME_,
             URIContent.toString());


    if (!pRealRun)
        {
        fprintf (wOutput,"%s>> opening ZMasterFile in mode ZRF_Read_Only  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Read_Only);  // for test read is enough
        }
    else
        {
        fprintf (wOutput,"%s>> opening ZMasterFile in mode ZRF_Exclusive | ZRF_Write  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Exclusive | ZRF_All); // must be this way
        }

     if (wSt!=ZS_SUCCESS)
             {
             ZException.addToLast(" While applying xml modifications.");
             goto EndzapplyXMLFile;
             }
     fprintf (wOutput,"%s>> file <%s> successfully opened in ZRF_Read_only mode\n",
              _GET_FUNCTION_NAME_,
              URIContent.toString());

//-------------Control FileType------------------------------------

    if (wMasterFile.ZDescriptor.ZHeader.FileType!=ZFT_ZMasterFile)
            {

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "File type is <%s> while expected <ZFT_ZMasterFile>.",
                                    decode_ZFile_type(wMasterFile.ZDescriptor.ZHeader.FileType));

            goto ErrorzapplyXMLFile;
            }

//--------------ZFileControlBlock modifiable fields----------------

    wSt=_testXMLZFileControlBlock(wMasterFile.ZDescriptor.ZFCB,wFirstlevelNode,wMissingTags,pRealRun, wOutput);
    if (wSt!=ZS_SUCCESS)
            goto ErrorzapplyXMLFile;

 //----------------ZMasterControlBlock modifiable fields----------------
    fprintf (wOutput,"%s>> processing node <ZMasterControlBlock>  \n",
             _GET_FUNCTION_NAME_);

    wSecondlevelNode = wFirstlevelNode.firstChildElement("ZMasterControlBlock");
    if (wSecondlevelNode.isNull())
        {
        wMasterFile.zclose();
        if (FOutput)
                fclose(wOutput);
        fprintf(wOutput,"   ****Error*** Missing <ZMasterControlBlock> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <ZMasterControlBlock> child to <File> tag : no child found.");
        goto ErrorzapplyXMLFile;
        }// isNull
    wNodeWork = wSecondlevelNode;

    wTagName = "HistoryOn";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. It will remain unchanged\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.HistoryOn==(wContent=="true"))
                wMessage.add( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.HistoryOn?"true":"false");
            else
                {
                wMessage.add(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.HistoryOn?"true":"false",
                             wContent=="true"?"true":"false");
                if (pRealRun)
                     {
                     wMasterFile.ZMCB.HistoryOn = (wContent=="true");
                     }
                }
    fprintf(wOutput,wMessage.toString());
    }// else field----------------------------------------------------

    wTagName = "JournalingOn";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s>.It will remain unchanged\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.getJournalingStatus()==(wContent=="true"))
                wMessage.add( " <%s> ==unchanged== \n",
                              wMasterFile.getJournalingStatus()?"true":"false");
            else
                {
                wMessage.add(" current <%s> modified to <%s>\n",
                             wMasterFile.getJournalingStatus()?"true":"false",
                             wContent=="true"?"true":"false");
                if (pRealRun)
                     {
//                     wMasterFile.ZMCB.JournalingOn = (wContent=="true");
                     }
                }
    fprintf(wOutput,wMessage.toString());
    }// else field----------------------------------------------------

    wTagName = "IndexFileDirectoryPath";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>Warning: Failed to get optional parameter <%s>. It will remain unchanged.\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.IndexFilePath.toQString()==wContent)
                wMessage.add( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.IndexFilePath.toString());
            else
                {
                wMessage.add(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.IndexFilePath.toString(),
                             wContent.toStdString().c_str());
                if (pRealRun)
                     {
                     wMasterFile.ZMCB.IndexFilePath = wContent;
                     }
                }
    fprintf(wOutput,wMessage.toString());
    }// else field----------------------------------------------------

//-------------------Indexes-----------------------------------------

    fprintf (wOutput,"%s>> processing node <%s>  \n",
             _GET_FUNCTION_NAME_,
             "Index");

    wZICB.clear();
    wZKDic.clear();

    IndexPresence.allocateCurrentElements(wMasterFile.ZMCB.Index.size());
    IndexPresence.bzero(); // equivalent to clearData

    wThirdlevelNode = wSecondlevelNode.firstChildElement("Index");

    if (wThirdlevelNode.isNull())  // this could be the case with 'Super ZRandomFile' (ZRandomFile + Journaling options)
        {
        fprintf(wOutput,"%s>>   ****Warning No Index found in definition ******\n",
                _GET_FUNCTION_NAME_);
        if (wMasterFile.ZMCB.Index.size()>0)
                {
         fprintf(wOutput,"%s>>    ****Found <%ld> existing indexes in file header : they will ALL be deleted  ******\n",
                 _GET_FUNCTION_NAME_,
                 wMasterFile.ZMCB.Index.size());
         wMissingTags++;
                }
        goto BackProcess_zapplyXMLFile;
        }// isNull

    IndexRank = 0; // xml index rank

    wFourthlevelNode = wThirdlevelNode.firstChildElement("IndexRank");

    if (wFourthlevelNode.isNull())
        {
        fprintf(wOutput,"%s>>   ****Error No Index rank found in Index number <%ld> definition . Index definition is corrupted******\n",
                _GET_FUNCTION_NAME_, IndexRank);
        wMissingTags++;
        wMissMandatoryTags ++;
        }// isNull


    while (!wFourthlevelNode.isNull())
        {
//-------------here get index rank and dictionary details and test ------------------------
        fprintf(wOutput,
                "%s>>   getting Index rank <%ld>-------\n",
                _GET_FUNCTION_NAME_,
                IndexRank);

        wNodeWork = wFourthlevelNode;

        wTagName = "Name";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> - stopping process\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Fatal,
                                    "xml document may be corrupted or incomplete.Expected mandatory field <Name> child to <IndexRank> tag : no child found.");
            wMissingTags++;
            goto ErrorzapplyXMLFile;
            }

//--------Search for index rank by its name

        IndexRank =  wMasterFile.ZMCB.Index.zsearchIndexByName(wContent.toStdString().c_str());

        if (IndexRank < 0) // not found
                {
                fprintf(wOutput,
                        "%s>> Index name <%s> will be added to Index at rank <%ld> \n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString(),
                        wMasterFile.ZMCB.Index.lastIdx());

                IndexPresence.push(1);      // Index will be created(and rebuilt) and does not have to to be rebuilt - pushed on index presence list
                IndexRank=IndexPresence.lastIdx();
                FAddIndex = true;
                wAddedIndexes ++;

                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.add(" adding value <%s>\n",
                             wContent.toStdString().c_str());
                wZICB.Name = wContent;
                }
                else
                {
                IndexPresence[IndexRank]=1; // so far : Index to be kept as is (see later)
                FAddIndex = false;

                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                 if (wMasterFile.ZMCB.Index[IndexRank].Name.toQString()==wContent)
                             wMessage.add( " <%s> ==unchanged== \n",
                                           wMasterFile.ZMCB.Index[IndexRank].Name.toString());
                        else
                            wMessage.add(" current <%s> modified to <%s>\n",
                                         wMasterFile.ZMCB.Index[IndexRank].Name.toString(),
                                         wContent.toStdString().c_str());

                }

        fprintf(wOutput,wMessage.toString());
//------------------End Index Name------------------------------------------


        if (FAddIndex)
        {
//--------------------Adding Index---------------------------------
            FIndexRebuild = true;

            wTagName = "Duplicates";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s> default value <%s> applied\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString(),
                        "ZST_DUPLICATES");
                wMissingTags++;
                }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());

                wZICB.Duplicates = encode_ZST(wContent.toStdString().c_str());

                wMessage.add(" adding value <%s>\n",
                             decode_ZST(wZICB.Duplicates));// Just to cross-check

                fprintf(wOutput,wMessage.toString());
                }// else---------------------------------------------
            wTagName = "AutoRebuild";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s> default value <%s> applied \n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString(),
                        "false");
                 wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",  // field does not exist in file
                             wTagName.toString());
                wMessage.add(" adding value <%s>\n",
                             wContent.toStdString().c_str());
                fprintf(wOutput,wMessage.toString());
                }// else---------------------------------------------

//----------------ZKDic------------------------------

            wFifthlevelNode = wFourthlevelNode.firstChildElement("ZKDic");

            if (wFifthlevelNode.isNull())
                {
                fprintf(wOutput,
                        "%s>>   ****Error No Dictionary found in Index number <%ld> definition . Index definition is corrupted******\n",
                        _GET_FUNCTION_NAME_,
                        IndexRank);
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** No Dictionary found in Index number <%ld> definition **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags ++;
                FIndexRebuild =false;
                goto ErrorzapplyXMLFile;
                }// isNull

            wSixthlevelNode = wFifthlevelNode.firstChildElement("KeyField");  // an index must at least have one declared field

            if (wFifthlevelNode.isNull())
                {
                fprintf(wOutput,
                        "%s>>   ****Error No Key field found in Dictionary of Index number <%ld> definition . Index definition is corrupted******\n",
                        _GET_FUNCTION_NAME_,
                        IndexRank);
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags ++;
                FIndexRebuild =false;
                goto ErrorzapplyXMLFile;
                }// isNull

            while (!wSixthlevelNode.isNull())
            {
            memset(&wField,0,sizeof(wField));

            wNodeWork= wSixthlevelNode;

            wTagName = "Name";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. It will remain unchanged.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                 wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.add(" adding value <%s>\n",
                             wContent.toStdString().c_str());
                fprintf(wOutput,wMessage.toString());

                wField.Name = wContent;

                }// else-----------------------------------------------
            wTagName = "NaturalSize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.add(" adding value <%ld>\n",
                             wContent.toLong());
                fprintf(wOutput,wMessage.toString());

                wField.NaturalSize = wContent.toLong();
                }// else-----------------------------------------------
            wTagName = "UniversalSize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.add(" adding value <%ld>\n",
                             wContent.toLong());
                fprintf(wOutput,wMessage.toString());

                wField.UniversalSize = wContent.toLong();
                }// else-----------------------------------------------
            wTagName = "ArraySize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.add(" adding value <%ld>\n",
                             wContent.toLong());
                fprintf(wOutput,wMessage.toString());

                wField.ArraySize = wContent.toLong();
                }// else-----------------------------------------------
            wTagName = "RecordOffset";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.add(" adding value <%ld>\n",
                             wContent.toLong());
                fprintf(wOutput,wMessage.toString());

                wField.RecordOffset = wContent.toLong();
                }// else-----------------------------------------------

            wTagName = "ZType";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                goto ErrorzapplyXMLFile;
                 }
                else
                {
               //---------encode ZType to test it----------------
                wZTypeString= wContent;
                wSt = encodeZTypeFromString(wZType,wZTypeString);
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf(wOutput,"%s>>**** parameter <%s> is errored with status <%s> Index will not be built\n",
                            _GET_FUNCTION_NAME_,
                            wTagName.toString(),
                            decode_ZStatus(wSt));
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            wSt,
                                            Severity_Error,
                                            "**** **** parameter <%s> is errored with status <%s> Index will not be built for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                            wTagName.toString(),
                                            decode_ZStatus(wSt),
                                            IndexRank
                                            );
                    wMissingTags++;
                    wMissMandatoryTags++;
                    FIndexRebuild =false;
                    }
                    else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.add(" adding value <%ld> <%s>\n",
                             wZType,
                             decode_ZType(wZType));
                fprintf(wOutput,wMessage.toString());

                wField.ZType = wZType;

                }// else
                }// else-----------------------------------------------

            wZKDic.push(wField);

            wSixthlevelNode=wSixthlevelNode.nextSiblingElement("KeyField");
            }// ! wSixthlevelNode.isNull

//--------------- End ZKDic--------------------------------------------------------
//--------------------Index Add------------------------------------------------------
            if (!FIndexRebuild)
            {
                fprintf(wOutput,"%s>>**** Index is errored and will not be added\n",
                        _GET_FUNCTION_NAME_);
                wAddedIndexes --;
                IndexPresence.pop();
            }
            else
            {
                fprintf(wOutput,"%s>>    Index <%s> rank <%ld> is to be created\n",
                        _GET_FUNCTION_NAME_,
                        wZICB.Name.toString(),
                        IndexRank
                        );

                if (pRealRun)
                        {
                fprintf(wOutput,"%s>>    Adding Index <%s>\n",
                        _GET_FUNCTION_NAME_,
                        wZICB.Name.toString());

                wSt =wMasterFile.zcreateIndex (wZKDic,  // contains the description of index key to add (multi fields)
                                               wZICB.Name,
                            //                 bool pAutorebuild,
                                               wZICB.Duplicates,
                                               false) ;
                if (wSt!=ZS_SUCCESS)
                            {
                    fprintf(wOutput,"%s>>    *********Fatal error creation of Index <%s> rank <%ld> ended with error\n",
                            _GET_FUNCTION_NAME_,
                            wZICB.Name.toString(),
                            IndexRank


                            );
                    IndexPresence[IndexRank]=1; // index exits now and does not have to be rebuilt
                    goto ErrorzapplyXMLFile;
                            }
                        }// pRealRun
            }// else FIndexRebuild
        } // FAddIndex

        else
//------------------Modifying existing index------------------------------
        {
        IndexPresence[IndexRank]=1 ;// not modified yet so not to be rebuilt

// Rule to change duplicate field : ZST_NODUPLICATES  -> ZST_DUPLICATES     : OK
//                                  ZST_DUPLICATES    -> ZST_NODUPLICATES   : Not allowed. delete index and recreate one
        wTagName = "Duplicates";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> parameter will remain unchanged\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            wMissingTags++;
            }
        else
        {
        wMessage.clear();
        wMessage.add("       parameter <%s>  ",
                     wTagName.toString());
        if (wMasterFile.ZMCB.Index[IndexRank].Duplicates==encode_ZST( wContent.toStdString().c_str()))
                                wMessage.add( " <%s> ==unchanged== \n",
                                              decode_ZST(wMasterFile.ZMCB.Index[IndexRank].Duplicates));
            else
                {
                wMessage.add(" current <%s> modified to <%s>\n",
                             decode_ZST(wMasterFile.ZMCB.Index[IndexRank].Duplicates),
                             wContent.toStdString().c_str());
                if (wMasterFile.ZMCB.Index[IndexRank].Duplicates==ZST_DUPLICATES) // if ZST_DUPLICATES must stay to this value
                    {
                    fprintf(wOutput,"%s>>**** Changing index <Duplicates> parameter to <%s> parameter is not allowed. It will remain unchanged\n",
                            _GET_FUNCTION_NAME_,
                            wContent.toStdString().c_str());
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            wSt,
                                            Severity_Error,
                                            "Changing index <%s> parameter to <%s> parameter is not allowed for index rank <%ld>. It will remain unchanged",
                                            wTagName.toString(),
                                            wContent.toStdString().c_str(),
                                            IndexRank);
                    }
                    else
                    {
                    if (pRealRun)
                        {
                        wMasterFile.ZMCB.Index[IndexRank].Duplicates = encode_ZST( wContent.toStdString().c_str());
                        }
                   // IndexPresence[IndexRank] = 2; // rebuild
                    }
                }// else
        fprintf(wOutput,wMessage.toString());
        }// else field----------------------------------------------------------

        wTagName = "AutoRebuild";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> parameter will remain unchanged\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            wMissingTags++;
            }
        else
        {
        wMessage.clear();
        wMessage.add("       parameter <%s>  ",
                     wTagName.toString());
        if (wMasterFile.ZMCB.Index[IndexRank].AutoRebuild==(wContent=="true"))
                    wMessage.add( " <%s> ==unchanged== \n",
                                  wMasterFile.ZMCB.Index[IndexRank].AutoRebuild?"true":"false");
                else
                {
                wMessage.add(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.Index[IndexRank].AutoRebuild?"true":"false",
                             wContent=="true"?"true":"false");
                if (pRealRun)
                    {
                    wMasterFile.ZMCB.Index[IndexRank].AutoRebuild = (wContent=="true");
                    }
                //IndexPresence[IndexRank] = 2; // rebuild
                }
        fprintf(wOutput,wMessage.toString());
        }// else field----------------------------------------------------------

//--------------Processing dictionary------------------------------------------
//----------------ZKDic------------------------------

            wFifthlevelNode = wFourthlevelNode.firstChildElement("ZKDic");

            if (wFifthlevelNode.isNull())
                {
                fprintf(wOutput,
                        "%s>>   ****Error No Dictionary found in Index number <%ld> definition . Index definition is corrupted******\n",
                        _GET_FUNCTION_NAME_,
                        IndexRank);
                wMissingTags++;
                wMissMandatoryTags++;

                goto ErrorzapplyXMLFile;
                }// isNull

            wSixthlevelNode = wFifthlevelNode.firstChildElement("KeyField");

            if (wFifthlevelNode.isNull())
                {
                fprintf(wOutput,
                        "%s>>   ****Error No Key field found in Dictionary of Index number <%ld> definition . Index definition is corrupted******\n",
                        _GET_FUNCTION_NAME_,
                        IndexRank);
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                goto ErrorzapplyXMLFile;
                }// isNull

            wKeyRank = 0;
            FAddField = false ;

            while (!wSixthlevelNode.isNull())
            {
            wNodeWork= wSixthlevelNode;

            if (wKeyRank > wMasterFile.ZMCB.Index[IndexRank].ZKDic->lastIdx())
                {
                fprintf(wOutput,"%s>>       Adding Field to index key dictionary for Index name <%s>.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                if (pRealRun)
                        {
                        wMasterFile.ZMCB.Index[IndexRank].ZKDic->newBlankElement();
                        IndexPresence[IndexRank] = 2;
                        }
                FAddField = true;
                } // wKeyRank > ZKDic size

            wTagName = "Name";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged Or omitted.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].Name== wContent.toStdString().c_str())
                        wMessage.add( " <%s> ==unchanged== \n",
                                      wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].Name.toString());
                    else
                    {
                    wMessage.add(" current <%s> modified to <%s>\n",
                                 wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].Name.toString(),
                                 wContent.toStdString().c_str());
                    if (pRealRun)
                        {
                        wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].Name = wContent ;
                        }

                    //IndexPresence[IndexRank] = 2 ; // rebuild Changing name of a field is neutral
                    }
                // changing field name will not induce an Index rebuilt. In case of Adding a field it even could be omitted
            fprintf(wOutput,wMessage.toString());
            }//else----------------------------------------------------------------------------

            wTagName = "NaturalSize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }

                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].NaturalSize== wContent.toLong())
                        wMessage.add( " <%ld> ==unchanged== \n",
                                      wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].NaturalSize);
                        else
                        {
                        wMessage.add(" current <%ld> modified to <%ld>\n",
                                     wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].NaturalSize,
                                     wContent.toLong());
                        if (pRealRun)
                            {
                            wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].NaturalSize = wContent.toLong() ;
                            }
                        IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                        }
                fprintf(wOutput,wMessage.toString());
                }// else-----------------------------------------------


            wTagName = "UniversalSize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].UniversalSize== wContent.toLong())
                            wMessage.add( " <%ld> ==unchanged== \n",
                                          wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].UniversalSize);
                        else
                            {
                            wMessage.add(" current <%ld> modified to <%ld>\n",
                                         wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].UniversalSize,
                                         wContent.toLong());
                            if (pRealRun)
                                {
                                wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].UniversalSize = wContent.toLong() ;
                                }
                            IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                            }
                fprintf(wOutput,wMessage.toString());
                }// else-----------------------------------------------

            wTagName = "ArraySize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ArraySize== wContent.toLong())
                            wMessage.add( " <%ld> ==unchanged== \n",
                                          wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ArraySize);
                        else
                            {
                            wMessage.add(" current <%ld> modified to <%ld>\n",
                                         wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ArraySize,
                                         wContent.toLong());
                            if (pRealRun)
                                {
                                wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ArraySize = wContent.toLong() ;
                                }
                            IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                            }
                fprintf(wOutput,wMessage.toString());
                }// else-----------------------------------------------

            wTagName = "RecordOffset";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].RecordOffset== wContent.toLong())
                            wMessage.add( " <%ld> ==unchanged== \n",
                                          wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].RecordOffset);
                        else
                            {
                            wMessage.add(" current <%ld> modified to <%ld>\n",
                                         wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].RecordOffset,
                                         wContent.toLong());
                            if (pRealRun)
                                {
                                wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].RecordOffset = wContent.toLong() ;
                                }
                            IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                            }
                fprintf(wOutput,wMessage.toString());

                }// else-----------------------------------------------

            wTagName = "ZType";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
               //---------encode ZType to test it----------------
                wZTypeString= wContent;
                wSt = encodeZTypeFromString(wZType,wZTypeString);
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf(wOutput,"%s>>**** parameter <%s> is errored with status <%s> Field will not be changed\n",
                            _GET_FUNCTION_NAME_,
                            wTagName.toString(),
                            decode_ZStatus(wSt));
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            wSt,
                                            Severity_Error,
                                            "**** **** parameter <%s> is errored with status <%s> Field will not be changed for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                            wTagName.toString(),
                                            decode_ZStatus(wSt),
                                            IndexRank
                                            );
                    wMissingTags++;
                    }
                    else
                    {
                    wMessage.clear();
                    wMessage.add("       parameter <%s>  ",
                                 wTagName.toString());

                    if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType== wZType)
                                wMessage.add( " <%s> ==unchanged== \n",
                                              decode_ZType(wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType));
                            else
                                {
                                wMessage.add(" current <%ld> <%s> ",
                                             wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType,
                                             decode_ZType(wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType));
                                wMessage.add("modified to <%ld> <%s>\n",
                                             wZType,
                                             decode_ZType(wZType));
                                if (pRealRun)
                                    {
                                    wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType = wZType ;
                                    }
                                IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                                }
                    fprintf(wOutput,wMessage.toString());
                    }// else
                }// else-----------------------------------------------

            wSixthlevelNode=wSixthlevelNode.nextSiblingElement("KeyField");
            wKeyRank++;
            } // while !wSixthlevelNode.isNull

        }// else FAddIndex


        wFourthlevelNode=wFourthlevelNode.nextSiblingElement("IndexRank");
        IndexRank ++;
        }// while !isNull()
//----------------End ZMasterControlBlock modifiable fields----------------

BackProcess_zapplyXMLFile:
//-------------Checking for index to suppress----------------------
    fprintf(wOutput,
            "%s>>     -----------Checking for index to suppress-----------------------------\n",
            _GET_FUNCTION_NAME_);

    for (long wi = 0;wi<IndexPresence.size();wi++)
    {
        if (!IndexPresence[wi])
        {
            fprintf(wOutput,
                    "%s>>     Index rank <%ld> Name <%s> is to be deleted\n",
                    _GET_FUNCTION_NAME_,
                    wi,
                    wMasterFile.ZMCB.Index[wi].Name.toString());
            wDestroyedIndexes ++;

            if (pRealRun)
                {
                 wSt=wMasterFile.zremoveIndex(wi);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf(wOutput,
                             "%s>>     ***** Error occurred while removing Index rank <%ld> status is <%s>\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             decode_ZStatus(wSt));
                     }// ! ZS_SUCCESS
                 fprintf(wOutput,
                         "%s>>     Index rank <%ld> has been deleted\n",
                         _GET_FUNCTION_NAME_,
                         wi);
                 IndexPresence.erase(wi); // remove the index flag from table
                 wi--;
                } // pRealRun
        }// IndexPresence
    }// for


    fprintf(wOutput,
            "%s>>     -----------End processing index(es) to suppress------------------------\n",
            _GET_FUNCTION_NAME_);

    fprintf(wOutput,
            "%s>>     -----------Checking for index to build/rebuild-----------------------------\n",
            _GET_FUNCTION_NAME_);

    for (long wi = 0;wi<IndexPresence.size();wi++)
    {
        if (IndexPresence[wi] == 2)
        {
            fprintf(wOutput,
                    "%s>>     Index rank <%ld> Name <%s> is to be rebuilt\n",
                    _GET_FUNCTION_NAME_,
                    wi,
                    wMasterFile.ZMCB.Index[wi].Name.toString());
            if (pRealRun)
                {
                 wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf(wOutput,
                             "%s>>     ***** Error occurred while rebuilding Index rank <%ld> status is <%s> Aborting\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             decode_ZStatus(wSt));
                     goto ErrorzapplyXMLFile;
                     }// ! ZS_SUCCESS
                } // pRealRun
            wRebuiltIndexes ++;
        }
    }

    fprintf(wOutput,
            "%s>>     -----------End processing index(es) to suppress------------------------\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();

EndzapplyXMLFile:

    //-----------------------Final report------------------------------

    fprintf(wOutput,
            "________________%s Report_______________\n"
            "  Missing tags                     %ld\n"
            "  Mandatory tags missing           %ld\n"
            "  Index(es) definition processed   %ld\n"
            "  Added Index(es)                  %ld\n"
            "  Suppressed Index(es)             %ld\n"
            "  Index(es) rebuilt                %ld\n"
            "______________________________________________________\n",
            _GET_FUNCTION_NAME_,
            wMissingTags,
            wMissMandatoryTags,
            IndexRank,
            wAddedIndexes,
            wDestroyedIndexes,
            wRebuiltIndexes);

    ZException.printUserMessage(wOutput);
    ZException.clearStack();

    if (FOutput)
            fclose(wOutput);

    return wSt;

ErrorzapplyXMLFile:
    fprintf(wOutput,
            "%s>>  *****process ended with errors*****************\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();
    goto EndzapplyXMLFile;

}// zapplyXMLFileDefinition


/** @} */ // group XMLGroup

#endif // QT_CORE_LIB



//-------------------Statistical functions-----------------------------------------

/**
@addtogroup ZMFSTATS ZMasterFile and ZIndexFile storage statistics and PMS session monitoring

@{
*/

/**
 * @brief ZMasterFile::ZRFPMSIndexStats gives storage statistical data for an Index given by its rank
 *
 *
 * @param pIndex    Index rank
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZMasterFile::ZRFPMSIndexStats(const long pIndex,FILE* pOutput)
{
    if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.size()))
    {
             fprintf(pOutput, " Performance Data report error for file <%s> : invalid index number <%ld>\n",
                     ZDescriptor.URIContent.toString(),
                     pIndex);
             return;
    }
    fprintf (pOutput,
             "\nPerformance report on index number <%ld> name <%s> for file <%s>\n"
             "         Index Content File <%s>\n"
             "                 session summary\n"
             "________________________________________________\n"
             "   Index Header file <%s>\n"
             "________________________________________________\n",
             pIndex,

             ZMCB.Index[pIndex].Name.toString(),
             ZDescriptor.URIHeader.toString(),
             ZMCB.IndexObjects[pIndex]->getURIContent().toString(),
             ZMCB.IndexObjects[pIndex]->getURIHeader().toString()
             );


    ZMCB.IndexObjects[pIndex]->ZDescriptor.ZPMS.reportDetails(pOutput);
    return;

}// ZRFPMSIndexStats

/**
* @brief ZMasterFile::zstartPMSMonitoring Starts PMS monitoring session for current ZMasterFile
* @note collected data concerns only ZMasterFile and NOT indexes
*   @ref  ZMasterFile::zstartIndexPMSMonitoring
*   @ref  ZMasterFile::zendIndexPMSMonitoring
*   @ref  ZMasterFile::zreportIndexPMSMonitoring
*/
 void
 ZMasterFile::zstartPMSMonitoring (void)
 {
     ZPMSStats.init();
 }
/**
* @brief ZMasterFile::zendIndexPMSMonitoring Ends current PMS monitoring session on Index given by its rank (pIndex)
* @note collected data concerns only ZMasterFile and NOT indexes
*   @ref  ZMasterFile::zstartIndexPMSMonitoring
*   @ref  ZMasterFile::zendIndexPMSMonitoring
*   @ref  ZMasterFile::zreportIndexPMSMonitoring
*/
  void
  ZMasterFile::zendPMSMonitoring (void)
  {
      ZPMSStats.end();
  }
/**
* @brief ZMasterFile::zreportPMSMonitoring Fully reports current PMS monitoring session on current ZMasterFile
* This report lists collected data :
*   - timer data
*   - ZRandomFile PMS data
* @note collected data concerns only ZMasterFile and NOT indexes
*   @ref  ZMasterFile::zstartIndexPMSMonitoring
*   @ref  ZMasterFile::zendIndexPMSMonitoring
*   @ref  ZMasterFile::zreportIndexPMSMonitoring
*
* @param pIndex rank of the index to monitor
* @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
*/
   void
   ZMasterFile::zreportPMSMonitoring (FILE* pOutput)
   {
       fprintf(pOutput,
               "________________________________________________\n"
               "   ZMasterFile <%s>\n"
               "   File open mode         %s\n"
               "________________________________________________\n",
               ZDescriptor.URIContent.toString(),
               decode_ZRFMode( ZDescriptor.Mode)
               );
       ZPMSStats.reportFull(pOutput);
   }



/**
  * @brief ZMasterFile::zstartIndexPMSMonitoring Starts PMS monitoring session on the specific index given by its rank (pIndex)
  * @param pIndex rank of the index to monitor
  */
 ZStatus
 ZMasterFile::zstartIndexPMSMonitoring (const long pIndex)
 {
     if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.lastIdx()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                "Invalid index number <%ld> requested while index range is [0,%ld]",
                                pIndex,
                                ZMCB.IndexObjects.lastIdx());
        return (ZS_INVOP);
        }
     ZMCB.IndexObjects[pIndex]->ZPMSStats.init();
     return ZS_SUCCESS;
 }//zstartIndexPMSMonitoring
 /**
   * @brief ZMasterFile::zendIndexPMSMonitoring Ends current PMS monitoring session on the specific index given by its rank (pIndex)
   * @param pIndex rank of the index to monitor
   */
ZStatus
ZMasterFile::zstopIndexPMSMonitoring(const long pIndex)
{
  if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.lastIdx()))
     {
     ZException.setMessage(_GET_FUNCTION_NAME_,
                             ZS_INVOP,
                             Severity_Severe,
                             "Invalid index number <%ld> requested while index range is [0,%ld]",
                             pIndex,
                             ZMCB.IndexObjects.lastIdx());
     return (ZS_INVOP);
     }
  ZMCB.IndexObjects[pIndex]->ZPMSStats.end();
  return ZS_SUCCESS;
}//zstopIndexPMSMonitoring

/**
* @brief ZMasterFile::zreportIndexPMSMonitoring Fully reports current PMS monitoring session on the specific index given by its rank (pIndex)
* @param pIndex rank of the index to monitor
* @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
*/
ZStatus
ZMasterFile::zreportIndexPMSMonitoring (const long pIndex, FILE* pOutput)
{
   if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.lastIdx()))
      {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Invalid index number <%ld> requested while index range is [0,%ld]",
                              pIndex,
                              ZMCB.IndexObjects.lastIdx());
      return (ZS_INVOP);
      }
   fprintf(pOutput,
           "________________________________________________\n"
           "   ZMasterFile <%s>\n"
           "   Index rank <%ld> <%s>\n"
           "   File open mode    %s\n",
           ZDescriptor.URIContent.toString(),
           pIndex,
           ZMCB.Index[pIndex].Name.toString(),
           decode_ZRFMode( ZDescriptor.Mode));
   ZMCB.IndexObjects[pIndex]->ZPMSStats.reportFull(pOutput);
   return ZS_SUCCESS;
}// zreportIndexPMSMonitoring

/** @}*/ // ZMFSTATS
//----------------End Stats---------------------------------------------------



//------------Generic Functions-----------------------------------------------------------
//




/**
  @ingroup ZMFPhysical
  @{ */

/**
 * @brief generateIndexRootName generates a ZIndexFile root name from its father ZMasterFile's root name
 *
 * ZIndexFile name generation rule
 *@verbatim
 *          <master root name>[<index name>]-<index rank>
 *@endverbatim
 *
 * @param[in] pMasterRootName   a descString containing the father ZMasterFile's root name
 * @param[in] pRank             Index rank
 * @param[in] pIndexName        Index user name : could be empty
 * @return a descString with the appropriate ZIndexFile root name
 */
descString DSRootName;
descString& generateIndexRootName(descString &pMasterRootName,
                              const long pRank,
                              descString &pIndexName)
{
    DSRootName = pMasterRootName;
    DSRootName += "-";
    if (pIndexName.isEmpty())
                {
                DSRootName+="index";
                }
            else
                {
                DSRootName += pIndexName;
                }
    DSRootName += "-";
    sprintf(&DSRootName.content[DSRootName.size()],"%02ld",pRank);

    DSRootName.eliminateChar(' ');
    DSRootName += __ZINDEX_FILEEXTENSION__;
    return DSRootName;
} // generateIndexRootName
/**
 * @brief generateIndexURI
 *              generates the index uri full base name (including directory path) but without any extension (and without '.' char)
 *
 *  If no directory is mentionned in pMasterName path, then the current directory is taken.
 *  returns a ZS_INVNAME status if ZMasterFile's extension is __ZINDEX_FILEEXTENSION__
 *  returns ZS_SUCCESS otherwise.
 *
 *  For base name generation @see generateIndexRootName
 *
 * @param[in]  pMasterFileUri   Base name for ZMasterFile to create ZIndexFile name for
 * @param[in]  pPathDir         Directory path to create the ZIndexFile in
 * @param[out] pZIndexFileUri   Resulting ZIndexFile name
 * @param [in] pRank            Index rank for the ZMasterFile
 * @param[in]  pIndexName       Index name (given by user) for the Index
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
generateIndexURI(uriString pMasterFileUri,
                 uriString &pDirectory,
                 uriString &pZIndexFileUri,
                 const long pRank,
                 descString& pIndexName)
{
uriString  wPath_Uri;
descString wMasterName;
descString wMasterExt;


    if (pDirectory.isEmpty())
            wPath_Uri = pMasterFileUri.getDirectoryPath();
        else
            wPath_Uri = pDirectory.toString();

    QUrl wUrl(wPath_Uri.toString());
    pZIndexFileUri=wUrl.toString(QUrl::PreferLocalFile);
    pZIndexFileUri.addConditionalDirectoryDelimiter() ;

    wMasterName = pMasterFileUri.getBasename();
    wMasterExt=pMasterFileUri.getFileExtension();

    if (wMasterExt==__ZINDEX_FILEEXTENSION__+1) // skip the '.' char
                {
                ZException.setMessage (_GET_FUNCTION_NAME_,
                                         ZS_INVNAME,
                                         Severity_Error,
                                         " Invalid ZMasterFile name extension %s for name %s",
                                         wMasterExt.toString(),
                                         pMasterFileUri.toString()
                                         );
                return ZS_INVNAME;
                }

    descString wM;
    wM=generateIndexRootName(wMasterName,pRank,pIndexName);
    pZIndexFileUri += wM;

    return(ZS_SUCCESS);
} //generateIndexURI
/** @} */ // ingroup ZMFPhysical


// -------------_Former stuff-----------------------------


#ifdef __FORMER__



//-------------ZMasterFile overloaded base expanded methods----------------------------------
//
//
//
//     ZMF Update routines
//


ZStatus
ZMasterFile::push(ZDataBuffer &pElement)
{
ZStatus wSt;
size_t widx;
//ZMFJournaling_struct wZJ;

    zadd(pElement);
    long wIdxRef=lastIdx();

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
            ZMFunlock();
#endif

    if (ZJournaling->ZFJournaling)
            {
            if((wSt=ZJournaling->journalize(-1,             // RFFU: not an index operation
                                   ZO_Push,   // Push operation
                                   ZAMInserted, // status accordingly
                                   (void *)pElement.Data,     // for a push or an insert, no backup of existing row is needed but it is there. RFFU : optimize out
                                   pElement.Size,          // size is not existing
                                   wIdxRef,     // ZAM index
                                    ZS_SUCCESS))!=ZS_SUCCESS)
                                    {
                                    fprintf(stderr,"ZMasterFile::erase-F-CANTJOURNALIZE Fatal error while journalizing row operation. Status is %s \n"
                                            "Aborting ",
                                            decode_ZStatus(wSt));    // need to be updated later
#ifdef __USE_ZTHREAD__
                                    ZMFunlock();
#endif
                                    this->~ZMasterFile();
                                    abort();
                                    }
                }// if ZFJournaling
    try {
      for (widx=0; widx<ZIFCallback.size();widx++)
               {
            if((wSt = ZIFCallback[widx]->_add((pElement,ZO_Push,wIdxRef))!=ZS_SUCCESS)
                                {
 //                               wSt = (ZStatus) (wSt|ZS_SECONDARY);
                                if (ZHistoryOn)
                                    ZHistory->_historizeErroredLastJournalEvent(ZJournaling,
                                                                        wSt,
                                                                        ZAMErrInsert,
                                                                        widx,
                                                                        -1);
#if __DEBUG_LEVEL__ > __ERRORREPORT__
                                fprintf(stderr,"ZMasterFile::push-E-ERR Error while updating ZIndexFile id <%ld> reference <%ld> during push.\n"
                                               " status is %s.  Rolling back already modified indexes.\n",
                                        widx,
                                        wIdxRef,
                                        decode_ZStatus(wSt));
#endif //__DEBUG_LEVEL__
                                ZJOption ZJO = ZJournaling->_getOption();
                                ZJournaling->setJournalingRollback(ZJDisable);

                                for (long wj=widx-1;wj>-1;wj--)                     //rollback concerned indexes with no journaling
                                            ZIFCallback[wj]->_erase(wIdxRef);

                                _Base::pop();  // then suppress the ZAM row

                                ZJournaling->setJournalingRollback(ZJO);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                ZMFunlock();
#endif
                                 return(wSt);
                                }
               }// for
        }//try
        catch (const std::bad_function_call& err)
                            {
                            fprintf(stderr,"ZMasterFile::push-F-std::function Fatal error while calling ZIndexFile id <%ld> _add function within std library call \n"
                                            " error is  %s\n",
                                    widx,
                                    err.what());
#ifdef __USE_ZTHREAD__
                            ZMFunlock();
#endif
                            this->~ZMasterFile();
                            abort();
                            }//catch

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
            ZMFunlock();
#endif
    if (ZJournaling->ZFAutoCommit)
            {
            commit(nullptr,-1);
            }

    return(ZS_SUCCESS);
}//ZAM::push


long
ZMasterFile::pop(void)
{
    if (size()>-1)
            zremove(0L);
    zremove(lastIdx);
    return(lastIdx());
}

long
ZMasterFile::pop_front(void)
{
    if (size()>-1)
            zremove(0L);
    return(size());
}



/**
 * @brief ZMasterFile::rollback Roll back journalized operation according pZAMMark value.
 *                       ZAMMark contains the journalized operation order  (0 the first operation ...)
 *                       if a negative value is mentionned, then the last operations mentionned are taken
 *                       example -3 indicates that the last 3 operations are to be rolled back (and no other preceeding operation), that then should be committed
 * @param pZAMMark
 * @return
 */
ZStatus
ZMasterFile::_rollback (ZAMMark_type pZAMMark)
{
  ZStatus  wSt;

 _Type *wElement;

 ZAMMark_type wZAMMark;

    if (!ZJournaling->ZFJournaling)
                    {
#if __DEBUG_LEVEL__ > __ERRORREPORT__
                    fprintf(stderr,"ZAM::_rollback-E-JRNLOFF  Error _rollback called while journaling is off\n");
#endif
                    return(ZS_INVOP);
                    }
    if (ZJournaling->isEmpty())
               return(ZS_SUCCESS);
    if (pZAMMark >= 0)
            wZAMMark = pZAMMark;
        else
            {
            wZAMMark=ZJournaling->lastIdx()+1+pZAMMark;
            if (wZAMMark<0)
                    {
#if __DEBUG_LEVEL__ > 0
                    fprintf(stderr,"ZMasterFile::rollback-E-INVZAMMARK negative value on journaling index ZAMMark <%ld> exceed actual journalized number of operations <%ld>.\n"
                            " Forced to zero : all available current operations will be rolled back.\n",
                            pZAMMark,
                            ZJournaling->lastIdx());
#endif // __DEBUG_LEVEL__
                    wZAMMark = 0;
                    }

            }// else
#ifdef __USE_ZTHREAD__      // rollback is always a total lock of ZAM & Journal & history
        ZAMlock();
#endif


/**
  * for each journaling row reverse from last to ZAMMark:
  *         for each index descriptor
  *                         call rollback routine for index
  *         rollback ZAM
  */
    ZAMMark_type wjnl= ZJournaling->lastIdx();
    while (  ZJournaling->lastIdx()>=wZAMMark )
        {
        ZCJournaling_struct wZJ = (*ZJournaling)[ZJournaling->lastIdx()];
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
        printf ("ZMA::rollback rolling back ZAM rank <%ld> Origin operation<%s> ZAM State <%s>\n",
                wZJ.Index,
                decode_ZOperation(wZJ.Operation),
                decode_ZAMState(wZJ.State));

#endif// __DEBUG_LEVEL__

        if ((wSt=_rollback_indexes(wjnl))!=ZS_SUCCESS)
                                            {
#ifdef __USE_ZTHREAD__
                                            ZMFunlock();
#endif
                                                return(wSt);
                                            }
// reverse the journalized operation
// when journalized    rollback operation
//     erase           insert saved Row at saved Index position
//     insert          erase saved Index position
//     Replace         replace Index position with saved Row



        switch (wZJ.Operation)
            {
            case (ZO_Add) :
            case (ZO_Insert):
                {
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
                printf("ZMA rollback Erase on ZMA rank %ld\n", wZJ.Index);
#endif
                ZJournaling->setLastZAMState(ZAMRBckErased) ;
                if(_Base::erase(wZJ.Index)<0)
                                    {
                                    ZJournaling->setLastStatus(ZS_COMROLLERROR);
                                    return(ZS_COMROLLERROR);
                                    }


                break;
                }
            case (ZO_Erase):
                {
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
                fprintf(stdout,"ZMA rollback Insert on ZMA rank %ld\n", wZJ.Index);
#endif

                ZJournaling->setLastZAMState(ZAMRBckInserted) ;
                wElement = (_Type*)wZJ.Data;
//                if (_Base::insert((_Type&)(*wZJ.Data),wZJ.Index)<0)
                if (_Base::insert((_Type*)(wElement),wZJ.Index)<0)
                                            {
                                            ZJournaling->setLastStatus(ZS_COMROLLERROR);
                                            return(ZS_COMROLLERROR);
                                            }
                break;
                }
            case (ZO_Replace):
                {
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
                printf("ZMA rollback Replace on ZMA rank %ld\n", wZJ.Index);
#endif
                ZJournaling->setLastZAMState(ZAMRBckReplaced) ;
                memmove(&_Base::Tab[wZJ.Index],&wZJ.Data,sizeof(_Type));

                break;
                }
            default:
                {
                return (ZS_INVOP);      //! invalid operation found  (who knows...)
                }
        }// switch

// rollback journaling element processing
//
 //       ZState[ZJournaling->Tab[wjnl]->Content.Index] = ZAMRolledBack ;  //! set status of row pointed by journal to rolled back

        if (ZHistoryOn)
                {
                ZHistory->_historizeRolledBackJournalEvent(ZJournaling);
//                ZJournaling->popNonDestructive();  // pop with no delete for the ZJournaling element pointer in there
                }
            else
                {
                ZJournaling->pop(); // destroying pop() for ZJournaling Element
                }

        wjnl= ZJournaling->lastIdx();

        }   // while ZJournaling

    return(ZS_SUCCESS);
}    // ZAM::rollback




ZStatus
ZMasterFile::_rollback_indexes (long pZAMMark)
{
ZStatus wSt;
ZCJournaling_struct wZJ;
//    memmove(&wZJ,&ZJournaling->Tab[pZAMMark]->Content,sizeof(ZCJournaling_struct)) ;
    if ((pZAMMark<0)||(pZAMMark>ZJournaling->lastIdx()))
                {
                fprintf (stderr,"ZMasterFile::_rollback_indexes-F-INVZAMMARK Fatal error : invalid journal index for rolling back indexes <%ld>, while Journaling element number is <%ld>\n",
                         pZAMMark,
                         ZJournaling->lastIdx());
                this->~ZMasterFile();
                abort();
                }
    wZJ = (*ZJournaling)[pZAMMark];

    for (long  wdsc=0; wdsc<ZIFCallback.size();wdsc++)
            {

            switch (wZJ.Operation)
                {
                case (ZO_Add) :
                case (ZO_Insert):
                    {
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
                printf("ZAM::rollback-I-ZIX rollback _erase on ZIX <%ld> ZMA rank %ld\n",wdsc, wZJ.Index);
#endif // __DEBUG_LEVEL__
                if ((wSt=ZIFCallback[wdsc]->_erase (wZJ.Index))!=ZS_SUCCESS)
                                {
                            fprintf(stderr,"ZAM::_rollback_indexes-F-ERRZIXRBERASE Fatal error while calling _rollback_indexes for _erase on ZIndexFile %ld  row index is %ld error is %s\n",
                                    wdsc,
                                    wZJ.Index,
                                    decode_ZStatus(wSt));
                            this->~ZMasterFile();
                            abort();
                                }// if

                    break;
                    }
                case (ZO_Erase):
                    {
                    printf("ZIX rollback _add on ZIX <%ld> ZMA rank %ld\n", wdsc,wZJ.Index);
                    if ((wSt=ZIFCallback[wdsc]->_add ((_Type*)wZJ.Data,ZO_Add,wZJ.Index))!=ZS_SUCCESS)
                                {
                            fprintf(stderr,"ZAM::_rollback_indexes-F-ERRZIXRBADD Fatal error while calling _rollback for _add on ZIndexFile %ld  row index is %ld error is %s\n",
                                    wdsc,
                                    wZJ.Index,
                                    decode_ZStatus(wSt));
                            this->~ZMasterFile();
                            abort();
                                }// if
                    break;
                    }
                case (ZO_Replace):
                    {
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
                printf("ZAM::rollback-I-ZIX rollback _replace on ZIX <%ld> ZMA rank %ld\n", wdsc,wZJ.Index);
#endif // __DEBUG_LEVEL_
                _Type *wT =(_Type*) wZJ.Data;
                if ((wSt=ZIFCallback[wdsc]->_replace ((*wT),wZJ.Index))!=ZS_SUCCESS)
                            {
                        fprintf(stderr,"ZAM::_rollback_indexes-F-ERRZIXRBREPLACE Fatal error while calling _rollback for _replace on ZIndexFile %ld  row index is %ld error is %s\n",
                                wdsc,
                                wZJ.Index,
                                decode_ZStatus(wSt));
                        this->~ZMasterFile();
                        abort();
                            }// if
                break;
                    break;
                    }
                default:
                    {
                    return (ZS_INVOP);      //! invalid operation found  (who knows...)
                    }
            }// switch



            } // for ZIFDescriptor
    return(ZS_SUCCESS);
}   // ZAM::_rollback_indexes

ZStatus
ZMasterFile::_ZIFHistorizeError(const long pZIndexFileId,
                   const ZOp pOperation,
                   const ZAMState_type pState,
                   void *pData,
                   const long pDataSize,
                   const long pIndex,
                   const ZStatus pOperationStatus)
{

if (!getHistoryStatus())
                return(ZS_INVOP);

ZStatus wSt;
if((wSt=journalize(pZIndexFileId,             //  index operation
                        pOperation,
                        pState,
                        pData,
                        pDataSize,
                        pIndex,
                        pOperationStatus))!=ZS_SUCCESS)
                        {
                        fprintf(stderr,"ZMasterFile::ZIXHistorizeError-F-CANTJOURNALIZE Fatal error while journalizing row operation. Status is %s \n"
                                "Aborting ",
                                decode_ZStatus(wSt));    // need to be updated later

                        this->~ZMasterFile();
                        abort();
                        } // journalize

        ZHistory->_historizeErroredLastJournalEvent(
                                                ZJournaling,
                                                 pOperationStatus,
                                                pState ,
                                                pZIndexFileId,
                                                pIndex);


    return(wSt);

}  // ZAM::_ZIXHistorizeError




//!----------------End ZMF journaling  expanded methods---------------------------

#endif// __FORMER__


/** @} */

//} // namespace zbs

#endif // _ZMASTERFILE_CPP_
