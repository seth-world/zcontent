#ifndef ZSIndexFile_H
#define ZSIndexFile_H

#include <zindexedfile/zmfconfig.h>

#include <cstdarg>
#include <zrandomfile/zrandomfile.h>
#include <ztoolset/ztimer.h> // to get ZTimer

//#include <ztoolset/zsacommon.h> // for types and getDataType

//#include <zam/zam_include.h> // for ZOp

#include <zindexedfile/zsindextype.h>

#include <zindexedfile/zsindexcollection.h>

#include <zcontentcommon/zoperation.h>

/*
#ifdef ZVerbose
extern ZVerbose_type ZVerbose;
extern FILE* ZVerboseOutput;
#endif
*/
#ifndef ZSMASTERFILE_CPP
    extern bool ZMFStatistics;

#endif

    void    setZVerbose(ZVerbose_type pVerbose) ;
    void    setZMFStatistics (bool pStatistics) ;

namespace zbs {

/**
* @addtogroup ZMFSTATS
 * @brief The zstatistics struct This object is used to store statistic on performance during searches on Indexes for a ZMasterFile
 *

 *
 *  Each high level operation (zsearch, zadd, etc...) gives its own zstatistics values, specific to the operation,
 * whatever the result is, even in case of failure.
 *
 *
 * As it accesses ZRandomFile, it collects also the whole ZRFPMS data set for the operation.
 *
 *
 */
#ifndef ZMFSTATS
#define ZMFSTATS
struct zstatistics : ZRFPMS
{
    long    Iterations;     //!< Number of iterations a seek operation needed
    //
    long    Reads;          //!< Number of ZRandomFile read accesses : zget
    long    Writes;         //!< Number of ZRandomFile write accesses : zwrite
    long    Removes;        //!< Number of ZRandomFile remove accesses : zremove
    ZTimer  Timer;          //!< Timer set for the operation
    ZTime   Delta;          //!< Elapsed
    ZRFPMS  *PMSBase;       //!< pointer to a ZRandomFile PMS base
    ZRFPMS  PMSStart;

    zstatistics(void) {clear();  return;}

    void clear (void) {memset(this,0,sizeof(zstatistics)); return;}

    void setPMSBase (ZRFPMS  *pZPMS) {PMSBase=pZPMS; return;}


    void init (void) ;
    zstatistics & end(void);

    void reportShort(FILE* pOutput=stdout);
    void reportFull(FILE* pOutput=stdout);

    zstatistics & operator = (ZRFPMS pPMSIn) { memmove (this,&pPMSIn,sizeof(ZRFPMS)); return *this;}
    zstatistics & operator = (zstatistics pStatIn) { memmove (this,&pStatIn,sizeof(zstatistics)); return *this;}

    zstatistics  operator - (zstatistics &pStatIn);
    zstatistics  operator + (zstatistics &pStatIn);

    zstatistics& operator -= (zstatistics &pPMSIn);
    zstatistics& operator += (zstatistics &pPMSIn);

};

#endif // ZMFSTATS


//------- Index & key data structure definitions--------------

/** @addtogroup ZIndexGroup
 *
 * @{
 *
*/





/**
 @brief The ZIndexItem class  Main Input/Ouput for indexes. It relates to ZRecord.
The key effective content as it will be stored using ZSIndexFile object (and not its Fields structure definition).

*/
class ZSIndexItem : public ZDataBuffer
{
public:
        zaddress_type ZMFaddress;    //!< Master file block record address to link index key with
        ZOp           Operation;     //!< this is NOT stored on index file (see toFileKey() method) but only for history & journaling purpose
        ZDataBuffer   KeyContent;    //!< extracted key content from user record according key extraction rules. Size of content is fixed and value is ZIndexControlBlock::KeySize.



       void clear (void) {KeyContent.clearData(); ZMFaddress=0L; Operation = ZO_Nothing;
                          //State = ZAMNothing;
                          return;}
       ZDataBuffer& toFileKey(void);
       ZSIndexItem&  fromFileKey (ZDataBuffer &pFileKey);

private:
//        ZDataBuffer Exchange;
};

class ZSIndexItemList : public ZArray<ZSIndexItem*>
{
typedef ZArray<ZSIndexItem*> _Base;
public:
    ~ZSIndexItemList(void)
        {
        clear();
        }

    void clear(void)
        {
        while (size()>0)
        delete popRP(&wItem);
        _Base::clear();
        }

ZSIndexItem* wItem;
};





//--------------Generic Functions--------------------------------------------------


ZStatus _keyValueExtraction(ZSKeyDictionary* pZKDic, ZRecord *pRecord, ZDataBuffer& pKey);


//=================================ZSIndexFile===============================



/**
 * @brief The ZSIndexFile class This object holds and manages at run-time an index file associated with a ZSMasterFile object ( Father ).

ZSIndexFile does not own the key definitions : Key definitions (dictionary) are local to ZMasterFile for which key has been defined.
ZSIndexFile index definition is stored in a ZIndexControlBlock (ZICB) that gives all necessary information about how to extract and format key fields from a record coming from its father (ZMasterFile).
ZSIndexFile manages
 - key values insertion / suppression. For doing this, it extracts and formats appropriate data from its father record using its ZICB.
 - searches on its index data.

Integrity controls are done to garanty an alignment with its ZMasterFile father.

@note this class is transparently created and used from ZMasterFile class object using appropriate ZMF routines.

 */

class ZSIndexFile : protected ZRandomFile
{
friend class ZSIndexCollection;
protected:
        typedef ZRandomFile                   _Base   ;
public:
    friend class ZSMasterFile;
    ZSIndexControlBlock                    *ZICB=nullptr;    //!< ZICB pointer to ZMF father's ZICB content
    zstatistics                            ZPMSStats;       //!< statistical structure
/**
 * @brief ZSIndexFile first constructor version : the common one.
 *          It sets up the ZSIndexFile parameters AND rebuild the index if pAutoRebuild is set to true (default value is true).
 *          If ZIX rebuild is done, there must not be any ZS_DUPLICATEKEY during the rebuild if ZSIndexFile duplicates option is set to ZST_NODUPLICATES.
 *          If so, abort() will be called (after having appropriately destroyed objects on father ZAM side).
 *
 * @note if you want to create a ZIX without knowing in advance if there will be duplicates on key or not : you should set pDuplicates to ZST_DUPLICATES.
 *      Do not create ZSIndexFilees with rejected key values : you will have holes into your index tables that will induce an impredictable result as soon as you will update any part of the hierarchy (ZAM and other dependant ZIXs).
 *
 * @param[in] pFather ZAM to which the ZIX refers
 * @param[in] pDuplicates ZSort_Type defining how duplicates will be managed. (set to ZST_DUPLICATES by default)
 */
    ZSIndexFile  (void *pFather);
    ZSIndexFile  (void *pFather,ZSIndexControlBlock* pZICB);

    ~ZSIndexFile() {}
    //~ZSIndexFile() {if (ZMFFather!=nullptr)
    //                             _deregister();}

    using _Base::getSize ;
    using _Base::getAllocatedBlocks;
    using _Base::getAllocatedSize;
    using _Base::getBlockExtentQuota;
    using _Base::getGrabFreeSpace;
    using _Base::getHighwaterMarking;
    using _Base::getBlockTargetSize;

    using _Base::setGrabFreeSpace;
    using _Base::setHighwaterMarking;

    using _Base::getURIContent;
    using _Base::getURIHeader;
    using _Base::getFileDescriptor;

    using _Base::isOpen;

    ZStatus setIndexURI(uriString &pUri);
    void setIndexName (utffieldNameString &pName);
    void setICB (ZSIndexControlBlock *pICB);

    ZStatus removeIndexFiles(void);

    ZFileDescriptor& getIndexFileDescriptor(void) {return _Base::ZDescriptor;}

    zrank_type getCurrentRank (void) {return ZDescriptor.CurrentRank;}

    ZStatus zcreateIndex(ZSIndexControlBlock *pICB,
                         uriString &pIndexUri,
                         long pAllocatedBlocks,
                         long pBlockExtentQuota,
                         zsize_type pInitialSize,
                         bool pHighwaterMarking=false,
                         bool pGrabFreeSpace=false,
                         bool pReplace=false,
                         bool pLeaveOpen=true);

    ZStatus zrebuildIndex (bool pStat=false, FILE *pOutput=stdout) ;

    ZStatus openIndexFile (uriString &pIndexUri, const int pMode);
    ZStatus closeIndexFile (void);

    ZStatus writeIndexControlBlock(checkSum **pCheckSum=nullptr);

    ZStatus removeIndexValue    (const ZDataBuffer& pKey , zaddress_type &pAddress);
//    ZStatus insertIndexValue    (ZDataBuffer& pRecord ,zaddress_type &pAddress, long &pZMFRank);

/*    ZStatus addRollback         (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress);
    ZStatus removeRollback      (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress);
    ZStatus insertRollback      (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress);
*/
    ZStatus searchIndex     (ZDataBuffer& KeyValue);

    ZStatus clearIndexFile  (void);
/** @cond Development */
    ZStatus _addKeyValue(ZRecord *pZMFRecord, zrank_type &pIndexRank, const zaddress_type pZMFAddress);

    ZStatus _addKeyValue_Prepare(ZRecord *pElement, ZSIndexItem *&pIndexItem, zrank_type &pIndexCommit, const zaddress_type pZMFAddress);
    ZStatus _addKeyValue_Commit(ZSIndexItem* pIndexItem, const zrank_type pIndexCommit);
    ZStatus _addKeyValue_Rollback(const zrank_type pIndexCommit);
    ZStatus _addKeyValue_HardRollback(const zrank_type pIndexCommit);

    ZStatus _removeKeyValue_Prepare (ZDataBuffer &pKey, ZSIndexItem *&pIndexItem, long &pIndexRank, zaddress_type &pZMFAddress);
    ZStatus _removeKeyValue_Commit  (const zrank_type pIndexCommit);
    ZStatus _removeKeyValue_Rollback( const zrank_type pIndexCommit);
    ZStatus _removeKeyValue_HardRollback(ZSIndexItem *pIndexItem, const zrank_type pIndexCommit);
/** @endcond */

    ZStatus getKeyIndexFields(ZDataBuffer &pIndexContent,ZDataBuffer& pKeyValue);
    ZStatus zprintKeyFieldsValues (const zrank_type pRank, bool pHeader=true, bool pKeyDump=false, FILE*pOutput=stdout) ;

    static inline
    ZStatus
    _search(const ZDataBuffer &pKey,
            ZSIndexFile &pZIF,
            ZSIndexResult &pZIR,
//            ZIFCompare pZIFCompare = ZKeyCompareBinary,
            const zlockmask_type pLock=ZLock_Nolock);

    static
    ZStatus
    _searchAll(const ZDataBuffer &pKey,
               ZSIndexFile &pZIF,
               ZSIndexCollection &pCollection,
               const ZMatchSize_type pZMS= ZMS_MatchIndexSize);
    static
    ZStatus
    _searchNext(const ZDataBuffer &pKey,
                ZSIndexFile &pZIF,
                ZSIndexCollection &pCollection,
                const ZMatchSize_type pZSC );

    static
    ZStatus _searchFirst (const ZDataBuffer        &pKey,     // key content to find out in index
                          ZSIndexFile               &pZIF,
                          ZSIndexCollection         *pCollection,
                          ZSIndexResult             &pZIR, const ZMatchSize_type pZMS) ;
    static
    ZStatus _searchNext (ZSIndexResult             &pZIR,
                         ZSIndexCollection *pCollection) ;

    static
    ZStatus _searchIntervalFirst (const ZDataBuffer        &pKeyLow,     // lowest value of key content to find out in index
                                  const ZDataBuffer        &pKeyHigh,    // highest value of key content to find out in index
                                  ZSIndexFile               &pZIF,       // ZSIndexFile object to search on
                                  ZSIndexCollection         *pCollection, // collection and context
                                  ZSIndexResult             &pZIR,       // First (in key order) index rank & ZMF addresses found matching key value
                                  const bool               pExclude); // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)



    static
    ZStatus _searchIntervalNext (ZSIndexResult       &pZIR,
                                 ZSIndexCollection   *pCollection) ;

    static
    ZStatus _searchIntervalAll   (const ZDataBuffer        &pKeyLow,     // lowest value of key content to find out in index
                                  const ZDataBuffer        &pKeyHigh,    // highest value of key content to find out in index
                                  ZSIndexFile               &pZIF,       // ZSIndexFile object to search on
                                  ZSIndexCollection         *pCollection, // collection and context
                                  const bool               pExclude); // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)


private:
   void*                        ZMFFather=nullptr;   // pointer to ZMasterFile that instantiated the ZSIndexFile object
   uriString                    IndexUri;    // current Index File uri to be passed to ZRandomFile

#ifdef __USE_ZTHREAD__
    ZMutex _Mtx;
#endif

    ZStatus _addRebuild(ZDataBuffer &pElement, size_t pZAMIdx);
    ZStatus _rebuildReverse(void);
    ZStatus _updateZReverse(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pIdx);
    long _lookupZMFIdx (size_t pZMFIdx);

    ZStatus _alignZSIndexFile(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pZIXIdx);

private:
    long            IndexCommitRank;
    zaddress_type   ZMFAddress;
    ZDataBuffer     CurrentKeyContent;
};// class ZSIndexFile

/** @} */ // ZIndexGroup

//--------------Functions------------------------


ZStatus _printKeyFieldsValues (ZDataBuffer *wKeyContent,ZSIndexControlBlock* ZICB, bool pHeader,bool pKeyDump,FILE*pOutput);


} // namespace zbs


#endif  //ZSIndexFile_H
