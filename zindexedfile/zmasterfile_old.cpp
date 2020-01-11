#ifndef _ZMASTERFILE_CPP_
#define _ZMASTERFILE_CPP_

#include <zindexedfile/zmasterfile.h>
#ifdef __COMMENT__
ZMasterFile::ZMasterFile(bool pHistory) //: ZRandomFile(_cst_default_allocation,_cst_realloc_quota)
{

CommitUserFunction = nullptr;
CommitUserArguments=nullptr;
ZJournalAction.commit   = std::bind(&ZMasterFile::commit, this,_1,_2);
ZJournalAction.rollback = std::bind(&ZMasterFile::_rollback, this,_1);

ZJournaling = new ZCJournaling(&ZJournalAction,_cst_zmf_journaling_allocation,_cst_zmf_journaling_reallocquota);

setDescriptorQuota(_cst_zmf_descriptor_allocation,_cst_zmf_descriptor_reallocquota);
if (pHistory)
           setHistoryOn();
           else
           setHistoryOff();
return;
}
#endif // __COMMENT__
//!----------- ZMasterFile Index methods----------------
//!

void
ZMasterFile::_addIndexField (ZMasterControlBlock &pMCB ,utfcodeString& pName, size_t pOffset, size_t pLength)
{
    ZIndexField wField;
    wField.Name = pName;
    wField.Offset = pOffset;
    wField.Length = pLength;
    pMCB.Index.(wField);
}

ZStatus
ZMasterFile::_createIndex ( ZMasterControlBlock &pMCB ,
                            utfcodeString pName,
                            ZST_DUPLICATES pDuplicates,
                            pCount,
                            ...)
{
    ZIndexFile *wZIF = new ZIndexFile(this,pDuplicates,pCount,...);
    ZIndexList.push (new ZIndex(this,pName));
    return (ZIndexList.last()->_createKey(pZIFKey,pDuplicates));

}

ZStatus
ZMasterFile::_openIndex (  utfcodeString pName,
                      ZST_DUPLICATES pDuplicates)
{
    ZIndexList.push (new ZIndex(this,pName));
    return (ZIndexList.last()->_createKey(pZIFKey,pDuplicates));
};


//!
//!-------------ZMasterFile overloaded base expanded methods----------------------------------
//!
//!
//!
//!     ZMF Update routines
//!


ZStatus
ZMasterFile::push(const ZDataBuffer &pElement)
{
ZStatus wSt;
size_t widx;
ZMFJournaling_struct wZJ;

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
      for (widx=0; widx<ZIFDescriptor.size();widx++)
               {
            if((wSt = ZIFDescriptor[widx]->_add((pElement,ZO_Push,wIdxRef))!=ZS_SUCCESS)
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
                                            ZIFDescriptor[wj]->_erase(wIdxRef);

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
 * @brief ZMasterFile::popRP_front
 *                  Returns a COPY of the last record of ZAM before removing it.
 * @param pElement
 * @return
 */

ZDataBuffer &
ZMasterFile::popRP(ZDataBuffer& pElement)
{
size_t widx;
ZStatus wSt;
ssize_t wIdxRef= lastIdx();

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
            ZAMlock();
#endif
    if (ZJournaling->ZFJournaling)
            {
            if((wSt=ZJournaling->journalize(-1,             // RFFU: not an index operation
                                    ZO_Pop,
                                    ZAMMarkedDelete,
                                    &this->Tab[this->lastIdx()],
                                    sizeof(_Type),
                                    this->lastIdx(),
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
            } // if ZFJournaling


    try {
      for (widx=0; widx<ZIFDescriptor.size();widx++)
               {
            if((wSt = ZIFDescriptor[widx]->_erase(this->lastIdx()))!=ZS_SUCCESS)
                                {
                                if (ZJournaling->ZFJournaling)
                                        {
//                                    wSt=(ZStatus)(wSt | ZS_SECONDARY);
                                    if (ZHistoryOn)
                                        ZHistory->_historizeErroredLastJournalEvent(ZJournaling,
                                                                            wSt,
                                                                            ZAMErrDelete ,
                                                                            widx,
                                                                            -1);
                                        }
                                fprintf(stderr,"ZMasterFile::popRP-F-ERR Fatal error while deleting ZIndexFile id <%ld> for ZAM rank <%ld> using popRP operation status is %s.\n",
                                        widx,
                                        wIdxRef,
                                        decode_ZStatus(wSt));  // fatal message is always printed on stderr
#ifdef __USE_ZTHREAD__
                                ZMFunlock();
#endif
                                this->~ZMasterFile();
                                abort();            // no rollback
                                }
               }// for
        }//try
        catch (const std::bad_function_call& err)
                            {
                            fprintf(stderr,"ZMasterFile::insert-F-std::function Fatal error while calling ZIndexFile id <%ld> _erase function within std library call \n"
                                            " error is  %s\n",
                                    widx,
                                    err.what());
#ifdef __USE_ZTHREAD__
                            ZMFunlock();
#endif
                            this->~ZMasterFile();
                            abort();
                            }//catch
// do the popRP on ZAM

    _Type &wReturn=_Base::popRP(pElement);

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
            ZMFunlock();
#endif
    if (ZJournaling->ZFAutoCommit)
            {
            commit(nullptr,-1);
            }

    return(wReturn);
}//ZAM::_popRP ZAM overload

/**
 * @brief ZMasterFile::popRP_front
 *                  Returns a COPY of the first record of ZAM before removing it.
 * @param pElement
 * @return
 */

ZDataBuffer
ZMasterFile::popRP_front(ZDataBuffer& pElement)
{
size_t widx;
ZStatus wSt;
size_t wIdxRef= 0;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
            ZAMlock();
#endif
    if (ZJournaling->ZFJournaling)
            {

            if((wSt=ZJournaling->journalize(-1,             // RFFU: not an index operation

                                ZO_Pop_front,
                                ZAMMarkedDelete,
                                &this->Tab[0],
                                sizeof(_Type),
                                0,
                                ZS_SUCCESS))!=ZS_SUCCESS)
                                {
                                fprintf(stderr,"ZAM::popRP_front-F-CANTJOURNALIZE Fatal error while journalizing row operation. Status is %s \n"
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
      for (widx=0; widx<ZIFDescriptor.size();widx++)
               {
            if((wSt = ZIFDescriptor[widx]->_erase(0))!=ZS_SUCCESS)
                                {
                                if (ZJournaling->ZFJournaling)
                                        {
 //                                       wSt=(ZStatus)(wSt | ZS_SECONDARY);
                                        if (ZHistoryOn)
                                            ZHistory->_historizeErroredLastJournalEvent(ZJournaling,
                                                                                wSt,
                                                                                ZAMErrDelete ,
                                                                                widx,
                                                                                -1);
                                            }
                                    fprintf(stderr,"ZAM::popRP-F-ERR Fatal error while deleting ZIndexFile id <%ld> for ZAM row rank <%ld> using popRP_front operation status is %s.\n",
                                            widx,
                                            wIdxRef,
                                            decode_ZStatus(wSt));  // fatal message is always printed on stderr
#ifdef __USE_ZTHREAD__
                                    ZMFunlock();
#endif
                                    this->~ZMasterFile();
                                    abort();            // no rollback
                                }
               }// for
        }//try
        catch (const std::bad_function_call& err)
                            {
                            fprintf(stderr,"ZAM::insert-F-std::function Fatal error while calling ZIndexFile id <%ld> _erase function within std library call \n"
                                            " error is %s\n",
                                    widx,
                                    err.what());
#ifdef __USE_ZTHREAD__
                            ZMFunlock();
#endif
                            this->~ZMasterFile();
                            abort();
                            }//catch
// do the popRP on ZAM

    if ((wSt=zremoveR(0L,pElement))!=ZS_SUCCESS)
                            ZException.exit_abort();
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFunlock();
#endif
    if (ZJournaling->ZFAutoCommit)
            {
            commit(nullptr,-1);
            }

    return(pElement);
}//_popRP ZAM overload


ZStatus
ZMasterFile::insert(const ZDataBuffer &pElement,const size_t pIdx)
{
size_t widx ;
ZStatus wSt ;

    size_t wIdxRef = _Base::insert(pElement,pIdx) ;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZAMlock();
#endif

    if (ZJournaling->ZFJournaling)
            {
            if((wSt=ZJournaling->journalize(-1,             // RFFU: not an index operation
                                    ZO_Insert,
                                    ZAMInserted,
                                    (void *)&pElement,
                                    sizeof(_Type),
                                    pIdx,
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

            } // if ZFJournaling
    try {
      for (widx=0; widx<ZIFDescriptor.size();widx++)
               {
            if((wSt = ZIFDescriptor[widx]->_add((_Type*)&pElement,ZO_Insert,wIdxRef))!=ZS_SUCCESS)
                                {
                                if (ZJournaling->ZFJournaling)
                                        {
  //                                  wSt=(ZStatus)(wSt | ZS_SECONDARY);
                                    if (ZHistoryOn)
                                        ZHistory->_historizeErroredLastJournalEvent(ZJournaling,
                                                                            wSt,
                                                                            ZAMErrInsert ,
                                                                            widx,
                                                                            -1);
                                        }
#if __DEBUG_LEVEL__ > __ERRORREPORT__
                                fprintf(stderr,"ZMasterFile::insert-E-ERR Error while updating ZIndexFile id <%ld> with ZAM row rank <%ld> during insert operation. Status is %s.\n"
                                        " Rolling back indexes that may have been updated.\n",
                                        widx,
                                        wIdxRef,
                                        decode_ZStatus(wSt));
#endif

                                //! rollback indexes from error
                                //!
                                //!
                                ZJOption ZJO = ZJournaling->_getOption();
                                ZJournaling->setJournalingRollback(ZJDisable);

                                for (long wj=widx-1;wj>-1;wj--)
                                                ZIFDescriptor[wj]->_erase(wIdxRef);

                                _Base::erase(pIdx); // then erase the ZAM row inserted

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
                            fprintf(stderr,"ZMasterFile::insert-F-std::function Fatal error while calling ZIndexFile id <%ld> _add function within std library call \n"
                                            " error is #%d : %s\n",
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
            commit(nullptr,-1); // commit with no arguments
            }

    return(ZS_SUCCESS);
}//ZAM::insert



ZStatus ZMasterFile::insert(const ZDataBuffer& pElement, const size_t pIdx, const size_t pNumber)
{
    ZStatus wSt;
    size_t wIdx = pIdx;
    size_t wLimit=pIdx + pNumber;
    while ((wIdx<wLimit)&&(wSt==ZS_SUCCESS))
    {
        wSt=insert(pElement,wIdx);
        wIdx++ ;
    }
    return(wSt);
} // ZAM::insert overload



ZStatus
ZMasterFile::erase(const size_t pIdx)
{
size_t widx;
ZStatus wSt ;

    long wIdxRef =pIdx;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
            ZAMlock();
#endif
    if (ZJournaling->ZFJournaling)
            {
            if((wSt=ZJournaling->journalize(-1,             // RFFU: not an index operation
                                    ZO_Erase,
                                    ZAMMarkedDelete,
                                    &Tab[pIdx],
                                    sizeof(_Type),
                                    wIdxRef,
                                    ZS_SUCCESS))!=ZS_SUCCESS)
                                {
                            fprintf(stderr,"ZAM::erase-F-CANTJOURNALIZE Fatal error while journalizing row operation. Status is %s \n"
                                    "Aborting ",
                                    decode_ZStatus(wSt));    // need to be updated later
                            this->~ZMasterFile();
                            abort();
                                }

            } // if ZFJournaling

    wIdxRef = _Base::erase(pIdx) ;  // wIdxRef is pIdx
    if (wIdxRef<0)
            {
        if (ZHistoryOn)
            ZHistory->_historizeErroredLastJournalEvent(ZJournaling,
                                                ZS_OUTBOUND,
                                                ZAMErrDelete ,
                                                -1,
                                                -1);
#ifdef __USE_ZTHREAD__
            ZMFunlock();
#endif
            return (ZS_OUTBOUND); // Return Error, Do not commit
            }


    try {
    for (size_t widx=0; widx<ZIFDescriptor.size();widx++)
                {
                if((wSt = ZIFDescriptor[widx]->_erase (wIdxRef))!=ZS_SUCCESS)
                                {
 //                               wSt=(ZStatus)(wSt | ZS_SECONDARY);
                                if (ZHistoryOn)
                                    ZHistory->_historizeErroredLastJournalEvent(ZJournaling,
                                                                        wSt,
                                                                        ZAMErrDelete ,
                                                                        widx,
                                                                        -1);

                                fprintf(stderr,"ZAM::erase-F-ERR Fatal error while updating ZIndexFile id <%ld> with ZAM row rank <%ld> during erase operation. Status is %s.\n"
                                        " Aborting .\n",
                                        widx,
                                        wIdxRef,
                                        decode_ZStatus(wSt));  // fatal message is always printed on stderr
#ifdef __USE_ZTHREAD__
                                ZMFunlock();
#endif

                                this->~ZMasterFile();
                                abort();  //! an Erase operation on indexes must be successful whatever the ZAM and ZIndexFile contents are
                                } // if ZIFDescriptor
                } // for
            }//try
            catch (const std::bad_function_call& err)
                                {
                                fprintf(stderr,"ZAM::erase-F-std::function Fatal error while calling ZIndexFile id <%ld> _erase function within std library call \n"
                                                " error is : %s\n",
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
            commit(nullptr,-1); // commit with no arguments
            }
    return(ZS_SUCCESS);
}//ZAM::erase


ZStatus ZMasterFile::erase(const size_t pIdx,const size_t pNumber)
{
ZStatus wSt ;

    size_t wIdx = pIdx;
    size_t wLimit=pIdx + pNumber;
    long wRet = 0;
    while ((wSt==ZS_SUCCESS)&&(wIdx<wLimit))
            {
            wSt= erase(wIdx);
            wIdx++;
            }
    return(wSt);
}//ZAM::erase



ZStatus
ZMasterFile::replace(ZDataBuffer &pElement, const size_t pIdx)
{
size_t widx;
ZStatus wSt, wSt1 ;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                    ZAMlock();
#endif

    if (ZJournaling->ZFJournaling)
            {
            if((wSt1=ZJournaling->journalize(-1,             // RFFU: not an index operation
                                    ZO_Replace,
                                    ZAMReplaced,
                                    &Tab[pIdx],
                                    sizeof(_Type),
                                    pIdx,
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
            } // if ZFJournaling
    try {
    for (widx=0; widx<ZIFDescriptor.size();widx++)
                {
                if((wSt = ZIFDescriptor[widx]->_replace (pElement,pIdx))!=ZS_SUCCESS)
                                {
                                if (ZJournaling->ZFJournaling)
                                        {
                                    if (ZHistoryOn)
                                        ZHistory->_historizeErroredLastJournalEvent(ZJournaling,
                                                                            wSt,
                                                                            ZAMErrReplace ,
                                                                            widx,
                                                                            -1);
                                        }
#if __DEBUG_LEVEL__ > __ERRORREPORT__
                                fprintf(stderr,"ZAM::replace-E-ERR Error while updating ZIndexFile id <%ld> with ZAM row rank <%ld> with a _replace operation. Status is %s.\n"
                                        " Roll back indexes that have may have been updated to previous values.\n",
                                        widx,
                                        pIdx,
                                        decode_ZStatus(wSt));
#endif// __DEBUG_LEVEL__
//!      must rollback only indexes that have been modified till index error with actual ZAM row value
//!      must clear Journaling
//!      must mention this in History
//!
                                ZJOption ZJO = ZJournaling->_getOption();
                                ZJournaling->setJournalingRollback(ZJDisable);

                                for (long wj=widx-1;wj>-1;wj--)
                                           ZIFDescriptor[wj]->_replace(_Base::Tab[pIdx],pIdx);

                                ZJournaling->setJournalingRollback(ZJO);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                ZMFunlock();
#endif
                                 return(wSt);
                                } // if ZIFDescriptor
                } // for
    }//try
    catch (const std::bad_function_call& err)
                        {
                        fprintf(stderr,"ZAM::replace-F-std::function Fatal error while calling ZIndexFile id <%ld> _replace function within std library call \n"
                                        " error is : %s\n",
                                widx,
                                err.what());

#ifdef __USE_ZTHREAD__
                        ZMFunlock();
#endif
                        this->~ZMasterFile();
                        abort();
                        }//catch

    memmove(&_Base::Tab[pIdx],&pElement ,sizeof(_Type)) ;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFunlock();
#endif
    if (ZJournaling->ZFAutoCommit)
            {
            commit(nullptr,-1); // commit with no arguments
            }
    return(ZS_SUCCESS);
}//replace


//!
//!----------------ZMasterFile Journaling expanded methods---------------------------
//!


void
ZMasterFile::setJournalingQuota (size_t pAlloc,size_t pQuota)
{
ZJournaling->setQuota(pQuota);       // set Quota first because of integrity control on Allocation vs Quota
ZJournaling->setAllocation(pAlloc);
return;
} // setJournalingQuota




/**
 * @brief ZMasterFile::commit commits journalized events
 * @param pCommitArguments
 * @param pZAMMark  ZAMMark_type value pointing to a journal index until which events will be committed
 * @return
 */
ZStatus
ZMasterFile::commit (void *pCommitArguments,ZAMMark_type pZAMMark) //! CommitArguments are arguments passed to user defined commit fumction
{

    if (!ZJournaling->ZFJournaling)
                return(ZS_INVOP);           // journaling must be on

    ZAMMark_type wZAMMark= (pZAMMark>0) ? pZAMMark : 0 ;  // commit will be done until this journal index
/**
  * for each journaling row reverse from last to ZAMMark:
  *         for each index descriptor
  *                 -call user routine
  *                     do appropriate actions (next journal row, continue, return error, abort, skip)
  *                     if OK
  *                         call commit routine for index
  *         commit ZAM
  *
  *
  */
#ifdef __USE_ZTHREAD__      // commit is a global lock whatever option chosen
                    ZAMlock();
#endif
    long wjnl= ZJournaling->lastIdx() ;
    while ((ZJournaling->lastIdx()>=0)&& !(ZJournaling->lastIdx()<wZAMMark) )
        {



        //!
        //!             call commit user routine here
        //!
        //!             First argument is always a reference to ZAM row being committed
        //!             Second argument is ZOp code for ZAM row
        //!             Third argument is optional and is a pointer to users data as a void* pointer
        //!
        //!
        //                    if (ZIFDescriptor[wdsc]->CommitUserFunction!=nullptr)
                        if (CommitUserFunction!=nullptr)
                                     {

                                    ZCommitStatus wZCS =   CommitUserFunction             //! call user function
                                                            ((const _Type&)_Base::Tab[ZJournaling->Tab[wjnl]->Content.Index],    //! with row currently being committed given by journal
                                                             ZJournaling->Tab[wjnl]->Content.Operation,
                                                             CommitUserArguments);                              //! and pass user's arguments block
                                    switch (wZCS)                                   //! analyze return from user's function
                                            {
                                            case ZCS_Fatal :
                                                    {
        #ifdef __USE_ZTHREAD__      // commit is a global lock whatever option chosen
                                                    ZMFunlock();
        #endif
                                                    fprintf(stderr,"ZAM::_commit-F-USERFATAL commit operation received fatal error signal from user routine\n");
                                                    this->~ZMasterFile();
                                                    abort();
                                                    }
                                            case ZSC_Skip:
                                                    {
                                                    continue;
                                                    }
                                            case ZCS_Error:
                                                    {
        #ifdef __USE_ZTHREAD__      // commit is a global lock whatever option chosen
                                                    ZMFunlock();
        #endif
                                                    return(ZS_USERERROR);
                                                    }
                                            }// switch
                                    }// if ZIFDescriptor

        //!
        //!             if returned status from user is OK, continue
        //!




//! commit operations on ZAM take place here
//!

    if (ZHistoryOn)
            {
            ZHistory->_historizeLastJournalEvent(ZJournaling,false);
            }
        else
            {
            ZJournaling->pop(); // destroying pop() for ZJournaling Element
            }
     wjnl= ZJournaling->lastIdx();
    }// while ZJournaling

#ifdef __USE_ZTHREAD__      // commit is a global lock whatever option chosen
                    ZMFunlock();
#endif
    return(ZS_SUCCESS);
}// ZMF::commit




ZIndexFile::ZIndexFile  (ZMasterFile *pFather,ZSort_Type pDuplicates,int pCount,...): ZRandomFile()
{
    va_list args;
    va_start (args, pCount);

    bool pAutoRebuild=true;

    ZICB.AutoRebuild = pAutoRebuild;
    ZICB.ZIDuplicates = pDuplicates;
    ZMFFather=pFather;
    if (pFather==nullptr)
                {
            fprintf(stderr,"ZIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
            this->~ZIndexFile();
            abort();
                }

    defineKeyFields(pCount,args);

    _register();
    if (pAutoRebuild)
            {
        ZStatus wSt = _rebuild();
    if (wSt != ZS_SUCCESS)
            {

        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Fatal,
                        "Fatal error while building or rebuilding ZIndexFile from ZIX constuctor. Status is <%s> \n",
                                decode_ZStatus(wSt));
        ZException.exit_abort();
            } // if _rebuild()
            } // if pAutoRebuild
    return;
}

void
ZIndexFile::defineKeyFields (long pCount,...)
{
ssize_t wSize = 0;
int wCount = pCount;
va_list args;
va_start (args, pCount);
ZIFKey.clear();
ZIndexField wZIFField;
while(wCount>0)
  {
    wZIFField.offset = va_arg(args , long );
    wCount--;
    if (wCount==0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP ,
                                    Severity_Fatal,
                                    "Invalid number of arguments in index key fields definition \n");
            ZException.exit_abort();
            }

    wZIFField.length = va_arg(args, long );
    wCount--;
    ZIFKey.Fields.push(wZIFField);
    ZIFKey.KeySize += wZIFField.length;
  }

    va_end (args);
return;
}

/**
 * @brief getZJournaling  returns a ZJournaling_struct (meaning with a Row content include and casted as a _Type) corresponding to pIdx Journal index order
 * @param pJournaling
 * @param pOutJournaling
 * @param pIdx
 * @return
 */
ZJournaling_struct<_Type>& getZJournaling(ZCJournaling *pJournaling,ZJournaling_struct<_Type>* pOutJournaling, size_t pIdx)
{
pOutJournaling->Index=pJournaling->Tab[pIdx]->Content.Index;
pOutJournaling->Operation=pJournaling->Tab[pIdx]->Content.Operation;
pOutJournaling->State=pJournaling->Tab[pIdx]->Content.State;
pOutJournaling->Status=pJournaling->Tab[pIdx]->Content.Status;
pOutJournaling->ZIndexFileId=pJournaling->Tab[pIdx]->Content.ZIndexFileId;
if (sizeof(_Type)!=pJournaling->Tab[pIdx]->Content.DataSize)
                            {
                            fprintf (stderr, "getJournaling-W-WRONGTYPESIZE  Output type size <%ld> is not aligned with stored data size within journal <%ld>. Using <%ld> \n",
                                     sizeof(_Type),
                                     pJournaling->Tab[pIdx]->Content.DataSize,
                                    _Min((long)sizeof(_Type),pJournaling->Tab[pIdx]->Content.DataSize));
                            }
memmove(&pOutJournaling->Row,pJournaling->Tab[pIdx]->Content.Data, _Min((long)sizeof(_Type),pJournaling->Tab[pIdx]->Content.DataSize));
return(*pOutJournaling);
}




/**
 * @brief popZJournaling  use getZJournaling to pop a journal event and return it under a ZJournal format (includes Row data)
 * @param pJournaling
 * @param pOutJournaling
 * @return
 */
ZJournaling_struct& popZJournaling(ZCJournaling *pJournaling,ZJournaling_struct* pOutJournaling)
{
    getZJournaling<_Type>(pJournaling, pOutJournaling,pJournaling->lastIdx());
    pJournaling->pop();
    return(*pOutJournaling);
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
//! reverse the journalized operation
//! when journalized    rollback operation
//!     erase           insert saved Row at saved Index position
//!     insert          erase saved Index position
//!     Replace         replace Index position with saved Row
//!


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

//! rollback journaling element processing
//!
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

    for (long  wdsc=0; wdsc<ZIFDescriptor.size();wdsc++)
            {

            switch (wZJ.Operation)
                {
                case (ZO_Add) :
                case (ZO_Insert):
                    {
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
                printf("ZAM::rollback-I-ZIX rollback _erase on ZIX <%ld> ZMA rank %ld\n",wdsc, wZJ.Index);
#endif // __DEBUG_LEVEL__
                if ((wSt=ZIFDescriptor[wdsc]->_erase (wZJ.Index))!=ZS_SUCCESS)
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
                    if ((wSt=ZIFDescriptor[wdsc]->_add ((_Type*)wZJ.Data,ZO_Add,wZJ.Index))!=ZS_SUCCESS)
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
                if ((wSt=ZIFDescriptor[wdsc]->_replace ((*wT),wZJ.Index))!=ZS_SUCCESS)
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


ZStatus
ZMasterFile::setJournaling (ZJOption pZJO)
{
#ifdef __USE_ZTHREAD__
    ZAMlock();
#endif


    ZStatus wSt=ZJournaling->setJournaling(pZJO) ;
    if (pZJO==ZJDisable)
            {
            setHistoryOff();
            }

#ifdef __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    return(wSt);
}//ZAM::setJournaling


                    //!----------------End ZAM journaling  expanded methods---------------------------



#endif // _ZMASTERFILE_CPP_
