#ifndef ZRFCOLLECTION_CPP
#define ZRFCOLLECTION_CPP
#include <zrandomfile/zrfcollection.h>

#include <ztoolset/utfvaryingstring.h>

using namespace zbs;
//-----------------ZRFCollection------------------------------------------------------
/**
 * @brief ZRFCollection::clear Clears and resets anything within collection excepted Collection (ZArray<zrank_type>)
 * in order to preserve recursivity of calls : ZRFCollection could be itself given as a collection to refine with other search arguments.
 */
void
ZRFCollection::reset(void)
{
    ZRFRank           = -1;
    InputCollectionIndex = -1 ;
    BaseCollectionIndex       = -1;
    FInitSearch     = false;
}

void
ZRFCollection::clear(void)
{
    reset();
    this->_Base::clear();
    if (InputCollection!=nullptr)
                delete InputCollection;
    InputCollection = new ZRFCollection(nullptr);
    Argument.clear();
}
/**
 * @brief ZRFCollection::copy Safely copies a collection given in input to current ZRFCollection. It will further be the base of raw ranks.
 * @param pCollection collection to copy
 */
void
ZRFCollection::copy(ZRFCollection &pCollection)
{
    clear();
    _Base::_copyFrom (pCollection);
    Argument._copyFrom(pCollection.Argument);

/*     for (long wi=0;wi<pCollection.size();wi++)
            {
            newBlankElement();
            memmove(&Argument.last(),&pCollection.Argument[wi],sizeof(ZSearchArgument));
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
*/
    ZRFRank = pCollection.ZRFRank;
    InputCollectionIndex= pCollection.InputCollectionIndex;
    BaseCollectionIndex= pCollection.BaseCollectionIndex;
    FInitSearch = pCollection.FInitSearch;
    ZRFFile = pCollection.ZRFFile;
    Lock = pCollection.Lock ;
    InputCollection = pCollection.InputCollection;
    ZSt = ZS_NOTHING;

}// copy

/**
 * @brief ZRFCollection::initSearch Initialize a search on the given ZRandomFile (pZRFFile) using optional pCollection of ranks.
 *
 * @param[in] pZRFFile a pointer to ZRandomFile object to which collection will refer. File must be appropriately open.
 * @param[in] pLock lock mask to be applied to all matched records. defaulted to ZLock_omitted.
 * If equals ZLock_Omitted :
 * - And pCollection points to an existing input collection, then input collection lock is taken
 * - Input Collection is omitted (nullptr) then ZLock_Nolock is taken
 *
 * @param[in] pInputCollection Optionally (defaulted to nullptr), an existing collection of records that may result from a previous search operation

 * @return a ZStatus
 * - ZS_SUCCESS if everything went well
 * - ZS_FILENOTOPEN if relative ZRandomFile is mentionned but not open.
 */
ZStatus
ZRFCollection::initSearch(ZRandomFile* pZRFFile,const  zlockmask_type pLock,ZRFCollection *pInputCollection)
{
    reset();
    ZRFFile=pZRFFile;
    if (!ZRFFile->isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FILENOTOPEN,
                                Severity_Error,
                                "Reference ZRandomFile must be open to use a ZRFCollection.");
        return ZS_FILENOTOPEN;
        }

    if (pInputCollection!=nullptr)
                {
                if (InputCollection != nullptr)
                                delete InputCollection;
                InputCollection = new ZRFCollection(nullptr);
                InputCollection->copy(*pInputCollection);
                if (Lock == ZLock_Omitted)
                                Lock=InputCollection->Lock;
                        else
                                Lock = pLock;
                }
        else
                {
                if (InputCollection!=nullptr)
                            delete InputCollection;
                InputCollection=nullptr;
                if (pLock==ZLock_Omitted)
                            Lock=ZLock_Nolock;
                }
    // Now we can clear _Base ZArray (only now for recursive search on same ZRFCollection)
    _Base::clear();

    ZSt = ZS_NOTFOUND;

    FInitSearch=true;
    return (ZS_SUCCESS);
}// initSearch


//--------------------- Raw Ranks --------------------------------------------------
/**
 * @brief ZRFCollection::getFirstRawRank Delivers the first raw ZRandomFile rank to read,
 * either the native rank or through the saved collection rank - collection (a ZArray<ssize_t>) mentionned during initSearch.
 * This is given without having evaluated the test conditions.
 *
 * @note the returned rank is not yet evaluated, and the resulting field read from ZRandomFile must be evaluated using testSequence() routine.
 *
 * ZSt is positionned to ZS_EOF if getNextRawRank is called while Rank is at already at last record rank or last collection rank.
 *
 * @param pZRFRank the ZRandomFile rank to read or -1 if end of collection or end of file.
 * @return a ZStatus
 * - ZS_EOF if there is an input collection and collection is empty - or no input collection and ZRandomFile is empty
 * - ZS_SUCCESS if first ZRandomFile record rank is found (either from first input collection rank or first ZRF file rank)
 */
 ZStatus ZRFCollection::getFirstRawRank(zrank_type& pZRFRank)
{

    if (InputCollection!=nullptr)    // there is a collection to refine
        {
        InputCollectionIndex=0;
        if (InputCollectionIndex >= InputCollection->size())
            {
            InputCollectionIndex = -1;
            pZRFRank=-1;
            ZRFRank=-1;
            ZSt=ZS_EOF;
            return ZS_EOF;
            }
        ZSt = ZS_SUCCESS;
        pZRFRank=InputCollection->Tab[InputCollectionIndex];
        ZRFRank=InputCollection->Tab[InputCollectionIndex];
        return ZS_SUCCESS ;
        }
// direct access to ZRandomFile : no collection to refine, first record rank is 0L
    ZRFRank=0;
    pZRFRank=0;
    if (ZRFRank >= ZRFFile->getSize())
                {
                ZRFRank = -1;
                pZRFRank = -1;
                ZSt=ZS_EOF;
                return ZS_EOF;
                }
    ZSt = ZS_SUCCESS;
    return ZS_SUCCESS;
}//getFirstRawRank
/**
 * @brief ZRFCollection::getNextRawRank Delivers the next raw ZRandomFile rank to read,
 * either the native rank or through the saved collection rank - collection (a ZArray<ssize_t>) mentionned during initSearch.
 * This is given without having evaluated the test conditions.
 *
 * @note the returned rank is not yet evaluated, and the resulting field read from ZRandomFile must be evaluated using testSequence() routine.
 *
 * ZSt is positionned to ZS_EOF if getNextRawRank is called while Rank is at already at last record rank or last collection rank.
 *
 * @param pZRFRank the ZRandomFile previous rank or -1 if end of input collection or end of file.
 * @return a ZStatus
 * - ZS_EOF if getPreviousRank gets outside the input collection range if input collection or outside the ZRF file range if not.
 * - ZS_SUCCESS if a previous raw ZRF record rank has been found.
 */
 ZStatus ZRFCollection::getNextRawRank(zrank_type& pZRFRank)
{

    if (InputCollection!=nullptr)    // there is a collection to refine
        {
        InputCollectionIndex ++;
        if (InputCollectionIndex > InputCollection->lastIdx())
            {
            InputCollectionIndex = -1;
            ZRFRank=-1;
            pZRFRank=-1;
            ZSt=ZS_EOF;
            return ZS_EOF;
            }

        ZRFRank=InputCollection->Tab[InputCollectionIndex];
        return setStatus(ZS_SUCCESS);
        }
// direct access to ZRandomFile : no collection to refine
    ZRFRank++;
    pZRFRank=ZRFRank;
    if (ZRFRank >= ZRFFile->getSize())
                {
                ZRFRank = -1;
                ZRFRank=-1;
                ZSt=ZS_EOF;
                return ZS_EOF;
                }
    ZSt = ZS_SUCCESS;
    return ZS_SUCCESS;
}//getNextRawRank
/**
 * @brief ZRFCollection::getPreviousRawRank Delivers the previous raw ZRandomFile rank to read,
 * either the native flat rank or through the saved collection rank - collection (a ZArray<ssize_t>) mentionned during initSearch
 * This is given without having evaluated the test conditions.
 *
 * ZSt is positionned to ZS_OUTBOUNDLOW if getPreviousRank is called while Rank is at 0.
 *
 * @param pZRFRank the ZRandomFile previous rank or -1 if beginning of collection or beginning of file
 * @return a ZStatus
 * - ZS_OUTBOUNDLOW if getPreviousRank gets outside the input collection range if input collection or outside the ZRF file range if not.
 * - ZS_SUCCESS if a previous raw ZRF record rank has been found.
 */
ZStatus
ZRFCollection::getPreviousRawRank (zrank_type& pZRFRank)
{
    if (InputCollection!=nullptr) // if there is a collection to refine
        {
        InputCollectionIndex --;
        if (InputCollectionIndex<0)
                {
                InputCollectionIndex = -1;
                ZRFRank=-1;
                pZRFRank=-1;
                ZSt=ZS_OUTBOUNDLOW;
                return ZS_OUTBOUNDLOW;
                }
        ZRFRank = InputCollection->Tab[InputCollectionIndex];
        pZRFRank=ZRFRank;
        ZSt = ZS_SUCCESS;
        return ZS_SUCCESS;
        }
    // no collection to refine
    ZRFRank --;
    pZRFRank= ZRFRank;
    if (ZRFRank<0)
            {
            ZRFRank=-1;
            ZSt=ZS_OUTBOUNDLOW;
            return ZS_OUTBOUNDLOW;
            }
    ZSt = ZS_SUCCESS;
    return ZS_SUCCESS;
}//getPreviousRawRank
/**
 * @brief ZRFCollection::getCurrentRawRank Delivers the current raw ZRandomFile rank to read,
 * either from stored collection of ranks or directly from ZRandomFile rank if input collection is omitted.
 * This is given without having evaluated the test conditions.
 *
 * @param pZRFRank the ZRandomFile previous rank or -1 if rank is errored (out of boundaries)
 * @return a ZStatus
 * - ZS_OUTBOUNDLOW if InputCollectionIndex gets outside the input collection range if input collection or outside the ZRF file range if not.
 * - ZS_OUTBOUNDHIGH if InputCollectionIndex gets outside the input collection range if input collection or outside the ZRF file range if not.
 * - ZS_SUCCESS if a previous raw ZRF record rank has been found.
 */
ZStatus
ZRFCollection::getCurrentRawRank(zrank_type &pZRFRank)
{
    if (InputCollection!=nullptr)
        {
        if (InputCollectionIndex<0)
                {
                ZSt = ZS_OUTBOUNDLOW;
                pZRFRank = -1;
                return ZS_OUTBOUNDLOW;
                }
        if (InputCollectionIndex > InputCollection->size())
                {
                pZRFRank=-1;
                ZSt = ZS_OUTBOUNDHIGH;
                return ZS_OUTBOUNDHIGH;
                }

        pZRFRank=InputCollection->Tab[InputCollectionIndex];
        return setStatus(ZS_SUCCESS);
        }
    if (InputCollectionIndex>ZRFFile->getSize())
            {
            ZSt = ZS_OUTBOUNDHIGH;
            pZRFRank=-1;
            return ZS_OUTBOUNDHIGH;
            }
    if (ZRFRank<0)
            {
            ZSt=ZS_OUTBOUNDLOW;
            pZRFRank=-1;
            return ZS_OUTBOUNDLOW;
            }
    ZSt=ZS_SUCCESS;
    pZRFRank=ZRFRank++;
    return ZS_SUCCESS;
}// getCurrentRawRank
//-------------End Raw Ranks-------------------------------------------

//--------------Selected Ranks-----------------------------------------

/**
 * @brief ZRFCollection::getFirstSelectedRank gets and returns the first ZRandomFile record rank matching field selection criterias
 *
 * - If found returns pZRFRank with matched ZRandomFile rank and its corresponding record content in pRecordContent
 * - or it returns -1 and a status of ZS_EOF (all ZRF file ranks have been exhausted )
 *
 * if a lock request has been set in ZRFCollection context (Lock is !=ZLock_Nothing) then Records are locked with mentionned lock mask
 * if an input collection is mentionned with a lock request then ZRandomFile record is unlocked when deselected.
 *
 * @param[out] pRecordContent content of first ZRandomFile record (in relative rank order) that matches Argument conditions.
 * @param[out] pZRFRank ZRandomFile record rank  corresponding to selection criterias, or -1 if no match found till end of file
 * @param[in]  pLock Lock mask to apply to selected ZRandomFile ranks for the whole collection.
 * This lock mask is stored in collection context.
 * @return a ZStatus with the following possible values
 * - ZS_FOUND a matching record has been found, its record content is returned in pRecordContent and its rank is returned in pZRFRank
 * - ZS_EOF  no matching records have been found till the end of ZRandomFile. pRecordContent is cleared and pZRFRank is set to -1
 *
 * - ZS_LOCKED mask (ZS_LOCKREAD, etc.) if accessed ZRandomFile record is locked (see ZRFCollection::zgetNextRetry() )
 * - Other file errors in case of severe file error
 */
ZStatus
ZRFCollection::getFirstSelectedRank (ZDataBuffer&pRecordContent, zrank_type &pZRFRank,const zlockmask_type pLock)
{
ZStatus wSt;
    Lock = pLock;   // store lock mask in search context
    if (!isEmpty())
            _Base::clear();
    BaseCollectionIndex=-1;
    wSt= getFirstRawRank(ZRFRank);
    if (wSt!=ZS_SUCCESS)
            {
            if (wSt==ZS_OUTBOUNDHIGH)
                    wSt=ZS_EOF;
            return setStatus(wSt);
            }
    return _getRetry(pRecordContent,pZRFRank);

}//getFirstSelectedRank

/**
 * @brief ZRFCollection::getNextSelectedRank gets and returns next ZRandomFile record rank matching field selection criterias
 *
 * ZSt is set to ZS_EOF if getNextSelectedRank is called while Rank is at already at last record rank or last collection rank
 *   or if no other record could match selection criterias until end of ZRandomFile.
 *@note Lock mask applied to matched record is stored in context field Lock
 *
 * @copydetails ZRFCollection::_getRetry()
 */
ZStatus
ZRFCollection::getNextSelectedRank (ZDataBuffer&pRecordContent,zrank_type& pZRFRank)
{
ZStatus wSt;

    wSt= getNextRawRank(pZRFRank);
    if (wSt!=ZS_SUCCESS)
            {
            if (wSt==ZS_OUTBOUNDHIGH)
                    wSt=ZS_EOF;
            BaseCollectionIndex=-1;
            return wSt;
            }
    return _getRetry(pRecordContent,pZRFRank);
}//getNextSelectedRank


/**
 * @brief ZRFCollection::_getRetry base routine to get a matched record and rank from ZRandomFile starting from context field ZRFRank.
 * @note ZRFRank is a context field that is positionned by ZRFCollection::getFirstRawRank() and ZRFCollection::getNextRawRank()

 * - If found returns pZRFRank with matched ZRandomFile rank and its corresponding record content in pRecordContent
 * - or it returns -1 and a status of ZS_EOF (all ZRF file ranks have been exhausted )
 *
 * if a lock request has been set in ZRFCollection context (Lock is !=ZLock_Nothing) then Records are locked with mentionned lock mask
 * if an input collection is mentionned with a lock request then ZRandomFile record is unlocked when deselected.
 *
 * @param[out] pRecordContent matched ZRandomFile record content if successfull. clear() if not
 * @param[out] pZRFRank matched ZRandomFile rank if successfull or -1 if not
 * @return a ZStatus with the following possible values
 * - ZS_FOUND a matching record has been found, its record content is returned in pRecordContent and its rank is returned in pZRFRank
 * - ZS_EOF  no matching records have been found till the end of ZRandomFile. pRecordContent is cleared and pZRFRank is set to -1
 * - ZS_LOCKED mask (ZS_LOCKREAD, etc.) if accessed ZRandomFile record is locked (see ZRFCollection::zgetNextRetry() )
 * - Other file errors in case of severe file error
 */
ZStatus
ZRFCollection::_getRetry(ZDataBuffer&pRecordContent,zrank_type& pZRFRank)
{
ZStatus wSt;
    wSt=ZS_NOTFOUND;
    while (wSt==ZS_NOTFOUND)
    {

    wSt=ZRFFile->zget(pRecordContent,pZRFRank);
    if (wSt!=ZS_SUCCESS)    // could be locked
        {
        ZException.addToLast(" While getting first selected Rank");
        return wSt;
        }

    if (Argument.evaluate(pRecordContent))
                    {  // match
 /*                   if (Lock!=ZLock_Nolock)
                        {
                        wSt=ZRFFile->zlock(pZRFRank,Lock);
                        if (wSt!=ZS_SUCCESS)
                                        return setStatus(wSt);
                        }*/
                    ZRFRank = pZRFRank;
                    push(pZRFRank);
                    BaseCollectionIndex = lastIdx();

                    return setStatus(ZS_FOUND);
                    }
// up to here ZRF rank is not matched with Argument rules

/*    if (InputCollection!=nullptr)// if there is an input collection : unlock record that were previously locked and are currently not selected
            {
            if (InputCollection->Lock!=ZLock_Nolock)    // only if input collection locked records
                    {
                    wSt= ZRFFile->zunlock(pZRFRank);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast("While trying to unlock input collection rank");
                        return setStatus(wSt); //
                        }
                    }
            } // InputCollection!=nullptr
*/
    wSt= getNextRawRank(pZRFRank);
    if (wSt!=ZS_SUCCESS)
            {
            if (wSt==ZS_OUTBOUNDHIGH)
                    wSt=ZS_EOF;
            BaseCollectionIndex = -1;
            InputCollectionIndex=-1;
            ZRFRank=-1;
            pZRFRank=-1;
            pRecordContent.clear();
            return setStatus(wSt);
            }
    wSt= ZS_NOTFOUND;
    }// while

    ZRFRank=-1;
    pZRFRank=-1;
    BaseCollectionIndex = -1;
    InputCollectionIndex = -1;
    return setStatus(wSt);
}//_getRetry

/**
 * @brief ZRFCollection::getPreviousSelectedRank returns the previous ZRandomFile selected Rank from Base ZArray<ssize_t>
 *      In case of success, ZSt is set to ZS_SUCCESS. In case of error, ZSt is set to ZS_OUTBOUNDLOW.
 * @return ZRandomFile record rank for the previous record corresponding to selection criterias, or -1 if out of boundaries.
 */
ZStatus
ZRFCollection::getPreviousSelectedRank (ZDataBuffer &pRecordContent,zrank_type& pZRFRank,const zlockmask_type pLock)
{

    BaseCollectionIndex --;
    if (BaseCollectionIndex<0)
            {
            ZSt= ZS_OUTBOUNDLOW;
            ZRFRank=-1;
            pZRFRank=-1;
            BaseCollectionIndex=-1;
            return ZS_OUTBOUNDLOW;
            }
    ZSt=ZS_FOUND;
    ZRFRank= Tab[BaseCollectionIndex];
    pZRFRank=ZRFRank;
    setStatus(ZRFFile->zget(pRecordContent,ZRFRank));
    if (getStatus()==ZS_SUCCESS)
                setStatus(ZS_FOUND);
    return getStatus();
}//getNextSelectedRank
#ifdef __COMMENT__
/**
 * @brief ZRFCollection::zgetNextRetry retry getting next selected record (matching Argument rules) starting from last accessed element (ZRFRank from context).
 * @param[out] pRecordContent ZRandomFile matching record content if successfull
 * @param[out] pZRFRank ZRandomFile record rank
 * @param[in] pWaitMs time to wait in milliseconds between two retry
 * @param[in] pTimes number of times to retry operation if not successfull before returning the wrong status
 * @return ZStatus returned by ZRFCollection::_getRetry()
 */
ZStatus
ZRFCollection::zgetNextRetry(ZDataBuffer &pRecordContent,zrank_type &pZRFRank,long pWaitMs,int16_t pTimes)
{

int16_t wTimes=(pTimes>0?pTimes:1), wretryTimes = 0;
 //   pZRFRank = ZRFRank; // don in _getRetry
    while (wretryTimes<wTimes)
    {
        ZSt=_getRetry(pRecordContent,pZRFRank);
        if (!(ZSt&ZS_LOCKED))       // return if success or status is not ZS_LOCKED series
                        return ZSt;
        usleep(pWaitMs);
        wretryTimes ++;
    }// while
    return ZSt ;
}//zgetNextRetry

#endif // __COMMENT__

/**
 * @brief ZRFCollection::getCurrentSelectedRank Return the current ZRandomFile rank corresponding to selection criterias (Last accessed).
 *      If no current record rank is available (no selected record or criterias not initialized) a value of -1 is returned.
 * @return  last selected ZRandomFile record rank or -1 in case of empty selection
 */
ZStatus ZRFCollection::getCurrentSelectedRank(zrank_type &pZRFRank)
{
    if (BaseCollectionIndex<0)
        {
        ZSt=ZS_OUTBOUNDLOW;
        return ZS_OUTBOUNDLOW;
        }
    if (BaseCollectionIndex>lastIdx())
        {
        ZSt=ZS_OUTBOUNDHIGH;
        return ZS_OUTBOUNDHIGH;
        }

    pZRFRank=Tab[BaseCollectionIndex];
    return setStatus(ZS_FOUND);
}//getCurrentSelectedRank

/**
 * @brief ZRFCollection::getAllSelected creates the whole Collection, browsing the whole collection (or the whole ZRandomFile if no collection),
 *              and selects ZRandomFile record rank according selection criterias, populates its Base ZArray<ssize_t> collection.
 * @return a ZStatus set to
 * - ZS_FOUND if at least one rank has been selected
 * - ZS_NOTFOUND if no record rank has been selected.
 * - ZS_LOCKED if a record is locked. In this case, the collection build process is stopped at the record rank that have been locked.
 * - In case of other error, the corresponding status is returned and ZException is set appropriately.
 */
ZStatus
ZRFCollection::getAllSelected (const zlockmask_type pLock)
{
ZStatus     wSt;
zrank_type  pRank;
ZDataBuffer wRecord;

   wSt = getFirstSelectedRank(wRecord,pRank,pLock);

    while (wSt==ZS_FOUND)
        {
        wSt=getNextSelectedRank(wRecord,pRank);
        }// while

    if (ZSt==ZS_EOF)
        {
            if (!isEmpty())
                    ZSt=ZS_FOUND;
              else
                    ZSt=ZS_NOTFOUND;
        }
    return ZSt;
}//getNextSelectedRank




//--------------End Selected Ranks-----------------------------------------

//----------------End ZRFCollection--------------------------------------------------



/** @cond Development */

/** @endcond */ // Development


#endif //ZRFCOLLECTION_CPP
