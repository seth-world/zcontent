#ifndef ZSINDEXCOLLECTION_CPP
#define ZSINDEXCOLLECTION_CPP


#include <zindexedfile/zrawindexfile.h>
#include <zindexedfile/zsmasterfile.h>
#include <zindexedfile/zsindexcollection.h>
#include <zindexedfile/zsjournal.h>
#include <zindexedfile/zsjournalcontrolblock.h>


using namespace zbs;




//----------------ZSIndexCollection-------------------------------------------------

void
ZSIndexCollection::clear(void)
{
    reset();
    this->_Base::clear();
    FCollection = false;
    if (InputCollection!=nullptr)
                delete InputCollection;
    InputCollection = new ZArray<ZSIndexResult>();
    Context.clear();
}// clear


ZSIndexCollection::ZSIndexCollection(ZRawIndexFile *pZIFFile)
{
    clear();
    ZIFFile=pZIFFile;
    return;
}

ZSIndexCollection::ZSIndexCollection(ZSMasterFile &pZMFFile,const long pIndexRank)
{

    ZSIndexCollection(pZMFFile.IndexTable[pIndexRank]);
    return;
}


/**
 * @brief ZSIndexCollection::clear Clears and resets anything within collection excepted Collection (ZArray<ssize_t> & bool)
 * in order to preserve recursivity of calls : ZSIndexCollection could be itself given as a collection to refine with other search arguments.
 */
void
ZSIndexCollection::reset(void)
{

    Context.reset();
}
/**
 * @brief ZSIndexCollection::initSearch Initialize a search on the given ZRandomFile (pZRFFile) using optional pCollection of ranks.
 * @param[in] pZRFFile  a pointer to a ZRandomFile structure that is an opened ZRandomFile to process the search on
 * @param pCollection   Optionally, an existing collection of records that may result from a previous search operation
 * @return
 */
ZStatus
ZSIndexCollection::initSearch(ZArray<ZSIndexResult> *pCollection)
{
    reset();
    if (!ZIFFile->isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FILENOTOPEN,
                                Severity_Error,
                                "Reference ZIndexFile <%s> must be open to use a ZSIndexCollection.",
                                ZIFFile->getURIContent().toString());
        return ZS_FILENOTOPEN;
        }

    if (pCollection!=nullptr)
        {
        FCollection=true;
        if (InputCollection == nullptr)
                InputCollection = new ZArray<ZSIndexResult>();
        InputCollection->_copyFrom(*pCollection);
        }
        else
        {
        if (InputCollection!=nullptr)
                    delete InputCollection;
        InputCollection=nullptr;
        FCollection=false;
        }
    // Now we can clear _Base ZArray

    _Base::clear();

    Context.clear();
    Context.FInitSearch=true;
    return (ZS_SUCCESS);
}// initSearch

/**
 * @brief ZSIndexCollection::copy Safely copies a collection given in input to current ZSIndexCollection. It will further be the base of raw ranks.
 * @param pCollection collection to copy
 */
void
ZSIndexCollection::copy(ZSIndexCollection &pCollection)
{
    clear();
    _Base::_copyFrom(pCollection);
    for (long wi=0;wi<pCollection.size();wi++)
            {
            newBlankElement();
            Argument.last().SizeToCollect =pCollection.Argument[wi].SizeToCollect;
            Argument.last().FieldLength=pCollection.Argument[wi].FieldLength;
            Argument.last().FieldOffset=pCollection.Argument[wi].FieldOffset;
            Argument.last().FCaseRegardless=pCollection.Argument[wi].FCaseRegardless;
            Argument.last().FString=pCollection.Argument[wi].FString;
            Argument.last().FTrimSpace=pCollection.Argument[wi].FTrimSpace;
            Argument.last().SearchType=pCollection.Argument[wi].SearchType;
            Argument.last().SequenceOffset=pCollection.Argument[wi].SequenceOffset;
            Argument.last().SequenceSize=pCollection.Argument[wi].SequenceSize;
            Argument.last().SearchSequence=pCollection.Argument[wi].SearchSequence;
            }
}// copy
//--------------------- Raw Indexes --------------------------------------------------
/**
 * @brief ZSIndexCollection::getZIRfromZIF
 * @param[out] pZIR      A ZSIndexResult corresponding to pIndexRank
 * @param[in] pIndexRank
 * @return a ZStatus
 * - ZS_OUTBOUNDLOW or ZS_OUTBOUNDHIGH if pIndexRank points to a non existing low or high relative rank
 * - ZS_SUCCESS if Index has been validly accessed and returned ZSIndexResult reflects the content of ZIndexFile record
 * - File error status if an error occurs. In this case ZException is set with appropriate error message.
 */
ZStatus
ZSIndexCollection::getZIRfromZIF(ZSIndexResult& pZIR,const zrank_type pIndexRank)
{

ZDataBuffer wIndexRecord;
ZSIndexItem wIndexItem ;
    if (pIndexRank<0)
            {
            Context.ZIFLast.reset();
            Context.ZSt=ZS_OUTBOUNDLOW;
            Context.CurrentZIFrank= -1;
            pZIR.reset();
            return ZS_OUTBOUNDLOW;
            }
    if (pIndexRank > ZIFFile->lastIdx())
            {
            Context.ZIFLast.reset();
            Context.ZSt=ZS_OUTBOUNDHIGH;
            Context.CurrentZIFrank= -1;
            pZIR.reset();
            return ZS_OUTBOUNDHIGH;
            }
    Context.CurrentZIFrank= pIndexRank;
    Context.ZSt=ZIFFile->zget(wIndexRecord,pIndexRank);
    if (Context.ZSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" While getting Index result.");
        Context.ZIFLast.reset();
        pZIR.reset();
        return Context.ZSt;
        }

    wIndexItem.fromFileKey(wIndexRecord);
    Context.ZIFLast.IndexRank = pIndexRank;
    Context.ZIFLast.ZMFAddress = wIndexItem.ZMFaddress ;
    pZIR=Context.ZIFLast;
    return Context.ZSt;
}//getZIRfromZIF

/**
 * @brief ZSIndexCollection::getFirstRawRank Delivers the first raw ZIndexFile rank as a ZSIndexResult structure,
 * either the native rank or through the saved collection rank - collection (a ZArray<ssize_t>) mentionned during initSearch.
 * This is given without having evaluated the test conditions.
 *
 * @note the returned rank is not yet evaluated, and the resulting field read from ZRandomFile must be evaluated using testSequence() routine.
 *
 * @param[out] pZIR the ZSIndexResult rank to read or isNull() ({-1,-1}) if end of collection or end of file.
 * @return a ZStatus set to
 * - ZS_SUCCESS if a valid first rank has been found
 * - ZS_EOF if getNextRawIndex is called while there is an input collection and current input collection index is at already last collection rank (input collection is empty-or-Index file is empty)
 * - error status from ZSIndexCollection::getZIRfromZIF()
 *
 */

ZStatus ZSIndexCollection::getFirstRawRank(ZSIndexResult & pZIR)
{
    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVINDEX,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }
    if (InputCollection!=nullptr)    // there is a collection to refine
        {
        Context.InputCollectionIndex=0;
        if (Context.InputCollectionIndex >= InputCollection->size())
            {
            Context.InputCollectionIndex = -1;
            Context.ZIFLast.reset();
            pZIR.reset();
            Context.ZSt=ZS_EOF;
            return ZS_EOF;
            }
        Context.ZSt = ZS_SUCCESS;
        Context.ZIFLast=InputCollection->Tab[Context.InputCollectionIndex];
        pZIR = InputCollection->Tab[Context.InputCollectionIndex];
        return ZS_SUCCESS;
        }
// direct access to ZRandomFile : no collection to refine
    Context.InputCollectionIndex=0;
    if (Context.InputCollectionIndex >= ZIFFile->getSize())
                {
                Context.ZIFLast.reset();
                Context.ZSt=ZS_EOF;
                return ZS_EOF;
                }
     return getZIRfromZIF(Context.ZIFLast,Context.InputCollectionIndex);
}//getFirstRawIndex
/**
 * @brief ZSIndexCollection::getNextRawRank Delivers the next raw ZRandomFile rank to read,
 * either the native rank or through the saved collection rank - collection (a ZArray<ssize_t>) mentionned during initSearch.
 * This is given without having evaluated the test conditions.
 *
 * @note the returned rank is not yet evaluated, and the resulting field read from ZRandomFile must be evaluated using testSequence() routine.
 *
 * ZSt is positionned to ZS_EOF if getNextRawIndex is called while Index is at already at last record rank or last collection rank.
 *
 * @note the returned rank is not yet evaluated, and the resulting field read from ZRandomFile must be evaluated using testSequence() routine.
 *
 * @param[out] pZIR the ZSIndexResult rank to read or isNull() ({-1,-1}) if end of collection / end of file or in case of file error (including record locked)
 * @return a ZStatus set to
 * - ZS_SUCCESS if a valid first rank has been found
 * - ZS_EOF if getNextRawIndex is called while there is an input collection and current input collection index is at already last collection rank (input collection is empty)
 * - error status from ZSIndexCollection::getZIRfromZIF()
 */
ZStatus
ZSIndexCollection::getNextRawRank (ZSIndexResult &pZIR)
{
    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVINDEX,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }

    Context.InputCollectionIndex ++;
    if (InputCollection!=nullptr)    // there is a collection to refine
        {

        if (Context.InputCollectionIndex > InputCollection->lastIdx())
            {
            Context.InputCollectionIndex = -1;
            Context.ZIFLast.reset();
            pZIR.reset();
            return setStatus(ZS_EOF);
            }
        Context.ZSt = ZS_SUCCESS;
        Context.ZIFLast=InputCollection->Tab[Context.InputCollectionIndex];
        return ZS_SUCCESS;
        }
// direct access to ZRandomFile : no collection to refine

    if (Context.InputCollectionIndex >= ZIFFile->getSize())
                {
                Context.ZIFLast.reset();
                pZIR.reset();
                return setStatus(ZS_EOF);
                }
    return getZIRfromZIF(pZIR,Context.InputCollectionIndex);
}//getNextRawIndex
/**
 * @brief ZSIndexCollection::getPreviousRawRank Delivers the previous raw ZRandomFile rank to read,
 * either the native flat rank or through the saved collection rank - collection (a ZArray<ssize_t>) mentionned during initSearch
 * This is given without having evaluated the test conditions.
 *
 * Context status (ZSt) is positionned to ZS_OUTBOUNDLOW if getPreviousIndex is called while Index is at 0.
 *
 * @return the ZRandomFile to read or -1 if beginning of collection or beginning of file
 */

ZStatus ZSIndexCollection::getPreviousRawRank(ZSIndexResult&pZIR)
{
    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }
    Context.InputCollectionIndex --;
    if (Context.InputCollectionIndex<0)
            {
            Context.InputCollectionIndex = -1;
            Context.ZIFLast.reset();
            pZIR.reset();
            Context.ZSt=ZS_OUTBOUNDLOW;
            return ZS_OUTBOUNDLOW;
            }
    if (InputCollection!=nullptr) // if there is a collection to refine
        {
        Context.ZSt = ZS_SUCCESS;
        pZIR = InputCollection->Tab[Context.InputCollectionIndex];
        Context.ZIFLast = InputCollection->Tab[Context.InputCollectionIndex];
        return ZS_SUCCESS;
        }
    // no collection to refine
    return getZIRfromZIF(pZIR,Context.InputCollectionIndex);
}//getPreviousRawIndex
/**
 * @brief ZSIndexCollection::getCurrentRawRank Delivers the current raw ZRandomFile rank to read,
 * either from stored collection of ranks or directly from ZRandomFile rank if input collection is omitted.
 * This is given without having evaluated the test conditions.
 *
 * @return current Raw rank to read
 */

ZStatus ZSIndexCollection::getCurrentRawRank(ZSIndexResult&pZIR)
{
    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }
    if (Context.InputCollectionIndex<0)
            {
            Context.ZIFLast.reset();
            pZIR.reset();
            Context.ZSt = ZS_OUTBOUNDLOW;
            return ZS_OUTBOUNDLOW;
            }
    if (InputCollection!=nullptr)
        {
        if (Context.InputCollectionIndex>InputCollection->size())
            {
            Context.ZSt=ZS_EOF;
            Context.ZIFLast.reset();
            pZIR.reset();
            return ZS_EOF;
            }

        pZIR= InputCollection->Tab[Context.InputCollectionIndex];
        return setStatus(ZS_SUCCESS);
        }
    if (Context.InputCollectionIndex>ZIFFile->getSize())
        {
        Context.ZSt=ZS_EOF;
        Context.ZIFLast.reset();
        pZIR.reset();
        return ZS_EOF;
        }
    Context.ZSt=ZS_SUCCESS;
    pZIR=Tab[Context.BaseIndex];
    return ZS_SUCCESS;
}// getCurrentRawIndex
//-------------End Raw indexes-------------------------------------------

//--------------Selected indexes-----------------------------------------

/**
 * @brief ZSIndexCollection::getFirstSelectedRank gets and returns the first ZSMasterFile record rank matching field selection criterias
 * This selection uses the sequential adhoc field selection mechanism.
 *
 * @return
 */
/**
 * @brief ZSIndexCollection::getFirstSelectedRank
 * @param pRecordContent
 * @param[out] pZIR First ZRandomFile record rank  corresponding to selection criterias, or -1 if no match found till end of file
 * @param{in] pLock lock mask to set the found record if any
 * @return a ZStatus with
 * - ZS_FOUND when a first record corresponding to selection has been found
 * - ZS_NOTFOUND if no record have been found
 * - a base file error status in other case
 *
 */
ZStatus
ZSIndexCollection::getFirstSelectedRank (ZDataBuffer &pRecordContent,ZSIndexResult &pZIR)
{
ZStatus wSt;
//ZSIndexResult wZIR;
ZSMasterFile *wMasterFile ;

    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVINDEX,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }

//    Context.Lock = pLock;   // store current lock mask into collection
    if (!isEmpty())
            _Base::clear();  // clear current collection

    setStatus( getFirstRawRank(pZIR));
    if (getStatus()!=ZS_SUCCESS)
            {
            Context.BaseIndex=-1;
            return getStatus();
            }
    wMasterFile = static_cast<ZSMasterFile*>(ZIFFile->ZMFFather);
    Context.ZSt=ZS_NOTFOUND;
    while (Context.ZSt==ZS_NOTFOUND)
        {
// here warm restart with a get to wZIR.ZMFAddress
        Context.ZSt=wMasterFile->zgetByAddress(pRecordContent,pZIR.ZMFAddress);
        if (Context.ZSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While getting first selected Index");
    //        BaseIndex = -1;  // in case of status = ZS_LOCKED, BaseIndex contains the current rank on ZSMasterFile or ZCollection to process
            Context.ZIFLast.reset();
            return Context.ZSt;
            }

        if (Argument.evaluate(pRecordContent)) // evaluate rules on the given record
                    {
 /*                   if (pLock!=ZLock_Nolock)
                        {
                        Context.ZSt=wMasterFile->zlockByAddress(pZIR.ZMFAddress,pLock);
                        if (Context.ZSt!=ZS_SUCCESS)
                                    {
                                    pZIR.reset();
                                    return getStatus();
                                    }
                        }*/
                    Context.ZSt= ZS_FOUND;
                    break;
                    }
                else
                    {
// up to here, record has not been selected. If record was from an existing collection, AND lock mask was NOT ZLock_Nolock,
//  need to release lock on former selected record
/*                    wSt=wMasterFile->zunlockByAddress(pZIR.ZMFAddress);
                    if (wSt!=ZS_SUCCESS)
                            {
                            ZException.addToLast("while unlocking ZMF record during getFirstSelected operation");
                            return setStatus(wSt);
                            }*/
                    pRecordContent.clear();
                    Context.ZSt= ZS_NOTFOUND;
                    }


        wSt= getNextRawRank(pZIR);
        if (wSt!=ZS_SUCCESS)
                {
                Context.BaseIndex = -1;
                pZIR.reset();
                pRecordContent.clear();
                return wSt;
                }
        }// while

    push(pZIR);
    Context.BaseIndex = lastIdx();
    return ZS_FOUND;
}//getFirstSelectedIndex

/**
 * @brief ZSIndexCollection::getNextSelectedRank gets and returns next ZRandomFile record rank matching field selection criterias
 *
 * ZSt is set to ZS_EOF if getNextSelectedIndex is called while Index is at already at last record rank or last collection rank
 *   or if no other record could match selection criterias until end of ZRandomFile.
 *
 * @return Next ZRandomFile record rank  corresponding to selection criterias, or -1 if no match found till end of file
 */

ZStatus
ZSIndexCollection::getNextSelectedRank(ZDataBuffer &pRecordContent,ZSIndexResult&pZIR)
{
ZStatus wSt;
//ZDataBuffer wRecordContent;

    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }

    wSt= getNextRawRank(pZIR);
    if (wSt!=ZS_SUCCESS)
            {
            Context.BaseIndex=-1;
            return wSt;
            }


    return _getRetry(pRecordContent,pZIR);
}//getNextSelectedRank

/**
 * @brief ZSIndexCollection::_getRetry gets the current index rank pointed by pZIR content test the corresponding ZMF record according selection rule using Argument::evaluate()
 * if a match occurs, returns
 * - a ZSIndexResult with Index Rank & ZMF record address matching the Argument rules
 * - corresponding ZMF record content
 * if no match, get next input collection or index record (if no input collection) until match, end of collection, record locked or other file error)
 *
 *  If a match occurs, then corresponding ZMF record is locked with Context::Lock mask
 *  If no match :
 * - if there is an input collection : non matched record is unlocked before going to next
 * - if there is no input collection, no unlock.
 *
 * @param pRecordContent ZSMasterFile record content for the found record matching Argument rules
 * @param[in,out] pZIR  a ZSIndexResult
 * - input : index record and ZMF address to start matching,
 * - output : ZSIndexResult with references of Index record (Index rank) and ZMF address for the matching record. If no match, then set to {-1;-1} (isNull()).
 * @return a ZStatus
 * - ZS_FOUND a valid record has been found
 * - ZS_EOF no more matching record has been found since input pZIR till end of collection or file
 * - ZS_LOCKED status mask (ZS_LOCKREAD, etc.) if one ZMF record has been found locked
 * - Other file error if another file error occurred.
 */
ZStatus
ZSIndexCollection::_getRetry(ZDataBuffer &pRecordContent,ZSIndexResult&pZIR)
{
ZStatus wSt;
ZSMasterFile *wMasterFile ;

    pZIR=Context.ZIFLast ;// ZIFLast is updated by getFirstRawRank and getNexRawRank
    wMasterFile = static_cast<ZSMasterFile*>(ZIFFile->ZMFFather);
    setStatus(ZS_NOTFOUND);
    while (getStatus()==ZS_NOTFOUND)
        {
        wSt=wMasterFile->zget(pRecordContent,pZIR.IndexRank);
        if (wSt!=ZS_SUCCESS)
            {
    // here case of locked record

            ZException.addToLast(" While getting next selected Index");
            pZIR.reset();
            return setStatus(wSt);
            }
        if (Argument.evaluate(pRecordContent))
            {
/*            if (Context.Lock!=ZLock_Nolock)  // lock ZMF record only if it makes sense
                {
                wSt=wMasterFile->zlockByAddress(pZIR.ZMFAddress,getLock());
                if (wSt!=ZS_SUCCESS)
                    {
                    ZException.addToLast(" While locking record in get next selected Index");
                    pZIR.reset();
                    return setStatus(wSt);
                    }
                }*/
            push(pZIR);
            Context.BaseIndex = lastIdx();
            return setStatus(ZS_FOUND);
            }
        pRecordContent.clear();

// up to here, record has not been selected. If record was from an existing collection, AND lock mask was NOT ZLock_Nolock,
//  need to release lock on former selected record

/*        if (InputCollection!=nullptr)
            {
            wSt=wMasterFile->zunlockByAddress(pZIR.ZMFAddress);
            if (wSt!=ZS_SUCCESS)
                return setStatus(wSt);
            }*/
        wSt= getNextRawRank(pZIR);        // positions ZSt to ZS_EOF if no more records to find
        if (wSt!=ZS_SUCCESS)
                {
                pZIR.reset();
                return wSt; // could be ZS_EOF or ZS_LOCKED
                }
        setStatus( ZS_NOTFOUND);
        }// while

    Context.BaseIndex = -1;
    Context.InputCollectionIndex = -1;
    pZIR.reset();
    return getStatus();

}//_getRetry
#ifdef __COMMENT__
/**
 * @brief ZSIndexCollection::zgetNextRetry retry getting next selected record (matching Argument rules) starting from last accessed element (ZRFRank from context).
 * @param[out] pRecordContent ZSMasterFile matching record content if successfull
 * @param[out] pZIR ZIndexRank for the matching record
 * @param pWaitMs time to wait in milliseconds between two retry
 * @param pTimes number of times to retry operation if not successfull before returning the wrong status
 * @return ZStatus returned by ZSIndexCollection::_getRetry()
 */
ZStatus
ZSIndexCollection::zgetNextRetry(ZDataBuffer &pRecordContent,ZSIndexResult& pZIR,long pWaitMs,int16_t pTimes)
{

int16_t wTimes=(pTimes>0?pTimes:1), wretryTimes = 0;

    pZIR=Context.ZIFLast;
//    if (Context.BaseIndex == lastIdx())
//                return Context.ZIFLast;

    while (wretryTimes<wTimes)
    {
        setStatus(_getRetry(pRecordContent,pZIR));
        if (!(Context.ZSt&ZS_LOCKED))       // return if success or status is not ZS_LOCKED series
                        return getStatus();
        usleep(pWaitMs);
        wretryTimes ++;
    }// while
    return getStatus();
}//zgetNextRetry
#endif //__COMMENT__
/**
 * @brief ZSIndexCollection::getPreviousSelectedRank returns the previous ZRandomFile selected index from Base ZArray<ssize_t>
 *      In case of success, ZSt is set to ZS_SUCCESS. In case of error, ZSt is set to ZS_OUTBOUNDLOW.
 * @return ZRandomFile record rank for the previous record corresponding to selection criterias, or -1 if out of boundaries.
 */
ZSIndexResult
ZSIndexCollection::getPreviousSelectedRank (const zlockmask_type pLock)
{
    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }
    Context.BaseIndex --;
    if (Context.BaseIndex<0)
            {
            Context.ZSt= ZS_OUTBOUNDLOW;
            Context.ZIFLast.reset();
            Context.BaseIndex=-1;
            return Context.ZIFLast;
            }
    Context.ZSt=ZS_SUCCESS;
    Context.ZIFLast= Tab[Context.BaseIndex];
    return Context.ZIFLast;
}//getNextSelectedIndex
/**
 * @brief ZSIndexCollection::getCurrentSelectedRank Return the current ZRandomFile rank corresponding to selection criterias (Last accessed).
 *      If no current record rank is available (no selected record or criterias not initialized) a value of -1 is returned.
 * @return  last selected ZRandomFile record rank or -1 in case of empty selection
 */
ZSIndexResult
ZSIndexCollection::getCurrentSelectedRank(void)
{
    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVINDEX,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }
    if (Context.BaseIndex<0)
        {
        Context.ZSt=ZS_OUTBOUNDLOW;
        Context.ZIFLast.reset();
        Context.BaseIndex = -1;
        return Context.ZIFLast;
        }
    if (Context.BaseIndex > lastIdx())
        {
        Context.ZSt=ZS_OUTBOUNDHIGH;
        Context.ZIFLast.reset();
        return Context.ZIFLast;
        }
    Context.ZIFLast= Tab[Context.BaseIndex];
    Context.ZSt=ZS_SUCCESS;
    return (Tab[Context.BaseIndex]);
}//getCurrentSelectedIndex

/**
 * @brief ZSIndexCollection::getAllSelected creates the whole Collection, browsing the whole collection (or the whole ZSMasterFile sequentially in record relative order if no collection),
 *              and selects ZRandomFile record rank according selection criterias, populates its Base ZArray<ssize_t> collection.
 * @return a ZStatus set to ZS_FOUND if at least one rank has been selected - ZS_NOTFOUND if no record rank has been selected.
 *          In case of other error, the corresponding status is returned and ZException is set appropriately.
 */
ZStatus
ZSIndexCollection::zgetAllSelected (const zlockmask_type pLock)
{
ZSIndexResult wZIR;
ZDataBuffer wRecordContent;
    if (ZIFFile==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Fatal,
                                "No ZIndexFile is mentionned while trying to access a ZSMasterFile index.");
        ZException.exit_abort();
        }

    Context.ZSt== getFirstSelectedRank(wRecordContent,wZIR);

    while (Context.ZSt==ZS_FOUND)
        {
        setStatus(getNextSelectedRank(wRecordContent,wZIR));
        }// while

    if (Context.ZSt==ZS_EOF)
        {
            if (!isEmpty())
                    Context.ZSt=ZS_FOUND;
              else
                    Context.ZSt=ZS_NOTFOUND;
        }
    return Context.ZSt;
}//getNextSelectedIndex

#ifdef __COMMENT__
ZStatus
ZSIndexCollection::_getAllRetry(void)
{
ZSIndexResult wZIR;
ZDataBuffer wRecordContent ;

    setStatus(_getRetry(wRecordContent,wZIR));
    while (getStatus()==ZS_FOUND)
        {
        getNextSelectedRank(wRecordContent,wZIR);
        }// while

    if (Context.ZSt==ZS_EOF)
        {
            if (!isEmpty())
                    Context.ZSt=ZS_FOUND;
              else
                   Context.ZSt=ZS_NOTFOUND;
        }
    return Context.ZSt;
} // getAllRetry


/**
 * @brief ZSIndexCollection::zgetAllRetry
 * @param pWaitMs number of micro seconds to wait between each retry operation retrying
 * @param pTimes number of time we have to retry operation before it failss
 * @return
 */
ZStatus
ZSIndexCollection::zgetAllRetry (long pWaitMs,int16_t pTimes)
{

int16_t wTimes=(pTimes>0?pTimes:1), wretryTimes = 0;

    if (Context.BaseIndex == lastIdx())
                return ZS_SUCCESS;
    if (!(Context.ZSt & ZS_LOCKED)) // retry only in case of lock(whatever lock it is), and disregard the other operations
                {
                    return Context.ZSt;
                }
    while (wretryTimes<wTimes)
    {
        Context.ZSt=_getAllRetry();
        if (!(Context.ZSt&ZS_LOCKED))       // return if success or status is not ZS_LOCKED series
                    return Context.ZSt;
        usleep(pWaitMs);
        wretryTimes ++;
    }// while
    return Context.ZSt;
}//zgetAllRetry



/**
 * @brief ZSIndexCollection::zlockAll Locks the whole selected items contained in ZSIndexCollection object with lockmask given by pLock.
 *  This means that lock mask is set to pLock at record level and lock owner Pid is set to current pid.
 * @param[in] pLock lock mask see @ref ZLockMask_type
 * @return a ZStatus
 */
ZStatus
ZSIndexCollection::zlockAll (const zlock_type pLock)
{

    setLock (pLock);
    for (Context.BaseIndex=0;Context.BaseIndex < size();Context.BaseIndex++)// lock corresponding ZSMasterFile address with given lock mask
        {
        Context.ZSt=static_cast<ZSMasterFile*>(ZIFFile->ZMFFather)->zlockByAddress(Tab[Context.BaseIndex].ZMFAddress,pLock);
        if (Context.ZSt!=ZS_SUCCESS) // if not successfull : unlock what has been done and return status
            {
            for (long wj=0;wj<Context.BaseIndex;wj++)
                {
               static_cast<ZSMasterFile*>(ZIFFile->ZMFFather)->zunlockByAddress(Tab[wj].ZMFAddress);
                }
            } // !ZS_SUCCESS
        } // for

    return getStatus();
}// zlockAll
/**
 * @brief ZSIndexCollection::zlockAllRetry
 * @param pWaitMs number of micro seconds to wait between each retry operation retrying
 * @param pTimes number of time we have to retry operation before it fails
 * @return
 */
ZStatus
ZSIndexCollection::zlockAllRetry (long pWaitMs,int16_t pTimes)
{
ZStatus wSt;
int16_t wTimes=(pTimes>0?pTimes:1), wretryTimes = 0;

    if (!(Context.ZSt & ZS_LOCKED)) // retry only in case of lock(whatever lock it is), and disregard the other operations
                {
                    return Context.ZSt;
                }
    while (wretryTimes<wTimes)
    {
        wSt=zlockAll(getLock());
        if (!(wSt&ZS_LOCKED))       // return if success or status is not ZS_LOCKED series
                    return setStatus(wSt);
        usleep(pWaitMs);
        wretryTimes ++;
    }// while
    return setStatus(wSt);
}//zremoveAllRetry


/**
 * @brief ZSIndexCollection::zunlockAll unlocks the whole selected items contained in ZSIndexCollection object.
 * It means that lock mask is set to ZLock_Nolock and pid is set to O at record level.
 * @return a ZStatus
 */
ZStatus
ZSIndexCollection::zunlockAll (void)
{
ZStatus wSt;


    for (Context.BaseIndex=0;Context.BaseIndex < size();Context.BaseIndex++)// lock corresponding ZSMasterFile address with given lock mask
        {
        wSt=static_cast<ZSMasterFile*>(ZIFFile->ZMFFather)->zunlockByAddress(Tab[Context.BaseIndex].ZMFAddress);
        if (wSt!=ZS_SUCCESS)
                {
                setStatus(wSt);
                }
        } // for

    return setStatus(wSt);
}// zunlockAll
#endif // __COMMENT__
/**
  * @brief ZSIndexCollection::zsetFieldValue Changes the value of one field for all records of the current collection
  * This collection routine allows to make massive change to the collection of records
  *
  * A check is made on field definition (offset and size) with all ZSMasterFile defined index key dictionaries, in order to prevent
  * modifying a field subject to being part of a key.
  * If such a violation occurs, then a ZS_INVADDRESS status is returned, without any modification made.
  *
  * @param[in] pOffset offset for the field to be changed
  * @param[in] pFieldValue a ZDataBuffer containing the value to change
  * @return
  */
 ZStatus
 ZSIndexCollection::zsetFieldValue(const ssize_t pOffset,ZDataBuffer &pFieldValue)
 {
ZStatus wSt;
ZSMasterFile* wMasterFile =  static_cast<ZSMasterFile*>(ZIFFile->ZMFFather);
ZBlock      wBlock;
ssize_t     wSize;
ZSKeyDictionary *wZKDic ;

ZDataBuffer wFormerFieldValue;

// here check if massive change will affect one of defined key fields for ZSMasterFile

/*    for (zrank_type wi=0;wi<wMasterFile->IndexTable.size();wi++)
        {
        wZKDic = wMasterFile->IndexTable[wi].ZKDic;
        for (long wj=0;wi<wZKDic->size();wj++)
                {
                if ((pOffset > wZKDic->fieldRecordOffset(wj))&&(pOffset < wZKDic->fieldRecordOffset(wj)))
                                    return ZS_INVADDRESS ;
                if (((pOffset+pFieldValue.Size) > wZKDic->fieldRecordOffset(wj))&&((pOffset+pFieldValue.Size) < wZKDic->fieldRecordOffset(wj)))
                                    return ZS_INVADDRESS ;
                }
        }// for*/
// up to here field definition is checked

   for (Context.BaseIndex=0;Context.BaseIndex < size();Context.BaseIndex++)
       {
       Context.ZSt=wMasterFile->_getByAddress(wBlock,Tab[Context.BaseIndex].ZMFAddress);
       if (Context.ZSt!=ZS_SUCCESS)
                   {
                   return getStatus();
                   } // !ZS_SUCCESS
       // check wether data size to change will not bypass the end of the record : if so, truncate it.
       // NB: we are in a record varying context

       if (wMasterFile->ZJCB!=nullptr)
           {
           wFormerFieldValue.setData(wBlock.Content.Data+pOffset,pFieldValue.Size);
           wMasterFile->ZJCB->Journal->enqueueSetFieldValue(ZJOP_ChgFld,
                                                                 wFormerFieldValue,
                                                                 pFieldValue,
                                                                 -1,
                                                                 Tab[Context.BaseIndex].ZMFAddress,
                                                                 pOffset);
           }


       wSize = pFieldValue.Size;
       if (wBlock.Content.Size<(pOffset+pFieldValue.Size))
                                    wSize=wBlock.Content.Size-pOffset;
       wBlock.Content.changeData(pFieldValue.Data,wSize,pOffset);

       wSt=wMasterFile->_writeBlockAt(wBlock,Tab[Context.BaseIndex].ZMFAddress);
       if (wSt!=ZS_SUCCESS)
                return wSt;



       } // for
   return wSt;
 }// zsetFieldValue

/**
 * @brief ZSIndexCollection::zremoveAll
 * @return
 */
ZStatus
ZSIndexCollection::zremoveAll (void)
{

    for (Context.BaseIndex=0;Context.BaseIndex < size();Context.BaseIndex++)// lock corresponding ZSMasterFile address with given lock mask
        {
        Context.ZSt=static_cast<ZSMasterFile*>(ZIFFile->ZMFFather)->zremoveByAddress(Tab[Context.BaseIndex].ZMFAddress);
        if (Context.ZSt!=ZS_SUCCESS)
                    {
                    return getStatus();
                    } // !ZS_SUCCESS
        } // for

    return getStatus();
}// zremoveAll

ZStatus
ZSIndexCollection::_removeAllRetry (void)
{

    for (long wi=Context.BaseIndex;wi < size();wi++)// lock corresponding ZSMasterFile address with given lock mask
        {
        setStatus(static_cast<ZSMasterFile*>(ZIFFile->ZMFFather)->zremoveByAddress(Tab[wi].ZMFAddress));
        if (Context.ZSt!=ZS_SUCCESS) // if not successfull : unlock what has been done and return status
                    {
                    Context.BaseIndex=wi;
                    return getStatus();
                    } // !ZS_SUCCESS
        } // for

    return getStatus();
}// _removeAllRetry
/**
 * @brief ZSIndexCollection::zremoveAllRetry
 * @param pWaitMs number of micro seconds to wait between each retry operation retrying
 * @param pTimes number of time we have to retry operation before it fails
 * @return
 */
ZStatus
ZSIndexCollection::zremoveAllRetry (long pWaitMs,int16_t pTimes)
{
ZStatus wSt;
int16_t wTimes=(pTimes>0?pTimes:1), wretryTimes = 0;

    if (Context.BaseIndex > lastIdx())
                            return ZS_SUCCESS;
    if (!(Context.ZSt & ZS_LOCKED)) // retry only in case of lock(whatever lock it is), and disregard the other operations
                {
                    return Context.ZSt;
                }
    while (wretryTimes<wTimes)
    {
        wSt=_removeAllRetry();
        if (!(wSt&ZS_LOCKED))       // return if success or status is not ZS_LOCKED series
                    return setStatus(wSt);
        usleep(pWaitMs);
        wretryTimes ++;
    }// while
    return setStatus(wSt);
}//zremoveAllRetry




//--------------End Selected indexes-----------------------------------------


//--------------End ZSIndexCollection------------------------------------------------


#endif // ZSIndexCollection_CPP
