#ifndef ZRAWINDEXFILE_H
#define ZRAWINDEXFILE_H

#include <zindexedfile/zmfconfig.h>

#include <cstdarg>
#include <zrandomfile/zrandomfile.h>
#include <ztoolset/ztimer.h> // to get ZTimer

//#include <ztoolset/zsacommon.h> // for types and getDataType
//#include <zam/zam_include.h> // for ZOp

#include <zindexedfile/zindextype.h>
#include <zindexedfile/zindexcollection.h>
#include <zcontentcommon/zoperation.h>
#include <zindexedfile/zmfstats.h>
#include <zindexedfile/zindexcontrolblock.h>
#include <zindexedfile/zindexdata.h>

#include <zindexedfile/zindextable.h>

#include <zindexedfile/zindexitem.h>

/*
#ifdef ZVerbose
extern ZVerbose_type ZVerbose;
extern FILE* ZVerboseOutput;
#endif
*/
#ifndef ZMasterFile_CPP
    extern bool ZMFStatistics;
#endif

    void    setZVerbose(ZVerbose_type pVerbose) ;
    void    setZMFStatistics (bool pStatistics) ;



//------- Index & key data structure definitions--------------


//--------------Generic Functions--------------------------------------------------


//ZStatus _keyValueExtraction(ZSKeyDictionary* pZKDic, ZRecord &pRecord, ZDataBuffer& pKeyOut);




//=================================ZIndexFile===============================

namespace zbs {

/** @addtogroup ZIndexGroup
 *
 * @{
 *
*/


/** @brief The ZIndexFile class This object holds and manages at run-time an index file associated with a ZMasterFile object ( Father ).

ZIndexFile record is composed of :
  - address : leading address - This is address of master file record this index points to
  - index key content

ZIndexFile does not own the key definitions : Key definitions (dictionary) are local to ZMasterFile for which key has been defined.
ZIndexFile index definition is stored in a ZIndexControlBlock (ZICB) that gives all necessary information about how to extract and format key fields from a record coming from its father (ZMasterFile).
ZIndexFile manages
 - key values insertion / suppression. For doing this, it extracts and formats appropriate data from its father record using its ZICB.
 - searches on its index data.

Integrity controls are done to garanty an alignment with its ZMasterFile father.

@note this class is transparently created and used from ZMasterFile class object using appropriate ZMF routines.

 */
//class ZSIndexControlBlock;
class ZIndexItem;
class ZRawRecord;
//class ZRawRecord;
class ZRawMasterFile;

class ZRawIndexFile : protected ZRandomFile, public ZIndexControlBlock
{
friend class ZIndexCollection;
friend class ZRawMasterFile;
friend class ZIndexTable;
friend class ZIndexFile;


public:
    friend class ZMasterFile;

/* for new version of zrebuildRawIndex() see zrawmasterfileutils.h (template) */
    friend ZStatus zrebuildRawIndex(ZRawIndexFile& pIF,bool pStat, FILE*pOutput);

//    ZSIndexControlBlock   *ZICB=nullptr;    //!< ZICB pointer to ZMF father's ZICB content
//    ZSIndexControlBlock   ZICB ;    //!< ZICB content

    ZMFStats              ZPMSStats;        //!< statistical structure
/**
 * @brief ZIndexFile first constructor version : the common one.
 *          It sets up the ZIndexFile parameters AND rebuild the index if pAutoRebuild is set to true (default value is true).
 *          If ZIX rebuild is done, there must not be any ZS_DUPLICATEKEY during the rebuild if ZIndexFile duplicates option is set to ZST_NODUPLICATES.
 *          If so, abort() will be called (after having appropriately destroyed objects on father ZAM side).
 *
 * @note if you want to create a ZIX without knowing in advance if there will be duplicates on key or not : you should set pDuplicates to ZST_DUPLICATES.
 *      Do not create ZIndexFilees with rejected key values : you will have holes into your index tables that will induce an impredictable result as soon as you will update any part of the hierarchy (ZAM and other dependant ZIXs).
 *
 * @param[in] pFather ZAM to which the ZIX refers
 * @param[in] pDuplicates ZSort_Type defining how duplicates will be managed. (set to ZST_DUPLICATES by default)
 */
    ZRawIndexFile  (ZRawMasterFile *pFather);
    ZRawIndexFile  (ZRawMasterFile *pFather,ZIndexControlBlock& pZICB);
    ZRawIndexFile  (ZRawMasterFile *pFather,int pKeyUniversalsize,const utf8String &pIndexName ,ZSort_Type pDuplicates=ZST_NODUPLICATES);


    ~ZRawIndexFile() {}
    //~ZIndexFile() {if (ZMFFather!=nullptr)
    //                             _deregister();}

    using ZRandomFile::getSize ;
    using ZRandomFile::getAllocatedBlocks;
    using ZRandomFile::getAllocatedSize;
    using ZRandomFile::getBlockExtentQuota;
    using ZRandomFile::getGrabFreeSpace;
    using ZRandomFile::getHighwaterMarking;
    using ZRandomFile::getBlockTargetSize;

    using ZRandomFile::setGrabFreeSpace;
    using ZRandomFile::setHighwaterMarking;

    using ZRandomFile::getURIContent;
    using ZRandomFile::getURIHeader;
    using ZRandomFile::getFileDescriptor;

    using ZRandomFile::isOpen;

    using ZRandomFile::getFCB;
    using ZRandomFile::getFileType;

    using ZRandomFile::lastIdx;
    using ZRandomFile::last;

    using ZRandomFile::zget;
    using ZRandomFile::getOpenMode;
    using ZRandomFile::zclearFile;


    IndexData_st getIndexData()
    {
      IndexData_st wReturn;
      wReturn.FCB = getFCB()->getUseableParams();
      wReturn.Status = ZPRES_Nothing;
      wReturn.IndexName = IndexName;
      wReturn.Duplicates = Duplicates;
      wReturn.KeyUniversalSize = KeyUniversalSize;
      return wReturn;
    }

    /**
     * @brief searchURFField searches for a record field in pPtrIn, extracts its URF value and appends it to pKeyContent.
     *  record field is searched using its position given by pPosition.
     *  A control is made with pFieldPresence : field must be present. If not, ZS_OMITTED is returned.
     *  When found and set, ZS_SUCCESS is returned.
     */

    ZStatus extractAllURFKeyFields (ZDataBuffer& pKeyContent,
                            const unsigned char *&pPtrIn,
                            const ZArray<long> pPosition,
                            const ZBitset& pFieldPresence) ;

    ZStatus setIndexURI(uriString &pUri);
    void setIndexName (utf8String &pName);

    ZStatus removeIndexFiles(ZaiErrors *pErrorLog=nullptr);

    ZFileDescriptor& getIndexFileDescriptor(void) {return (ZFileDescriptor&)*this;}

    ZStatus  setParameters (const bool pGrabFreeSpace,
                            const bool pHighwaterMarking,
                            const ssize_t pBlockTargetSize,
                            const size_t pBlockExtentQuota)
    { return ZRandomFile::_setParameters( ZFT_ZIndexFile,
                                    pGrabFreeSpace,
                                    pHighwaterMarking,
                                    pBlockTargetSize,
                                    pBlockExtentQuota);
    }

    zrank_type getCurrentRank (void) {return CurrentRank;}
/**
   * @brief ZIndexFile::zcreateIndex creates a new index file corresponding to the given specification ICB and ZRF parameters
   *
   *  @note This could be NOT a good idea to set GrabFreeSpace option : Indexes are fixed length then search in Free Pool are only made with the same size.
   *
   * BlockTargetSize is computed with
   *
   * @param[in] pICB              pointer to ZIndexControlBlock of indexfile to create
   * @param[in] pIndexUri         uriString with index file name
   * @param[in] pAllocatedBlocks
   * @param[in] pBlockExtentQuota
   * @param[in] pInitialSize
   * @param[in] pHighwaterMarking
   * @param[in] pGrabFreeSpace
   * @param[in] pLeaveOpen        Option : true leave the index file open as (ZRF_Exclusive | ZRF_All) false: close index file right after creation
   * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
   */
    ZStatus zcreateIndexFile(ZIndexControlBlock &pICB,
                         uriString &pIndexUri,
                         long pAllocatedBlocks,
                         long pBlockExtentQuota,
                         zsize_type pInitialSize,
                         bool pHighwaterMarking=false,
                         bool pGrabFreeSpace=false,
                         bool pBackup=false,
                         bool pLeaveOpen=true);
  /**
   * @brief ZIndexFile::zcreateIndex creates a new index file corresponding to the given specification ICB and ZRF parameters
   *      same as previous but with explicit pBlockTargetSize
   */
    ZStatus zcreateIndexFile(ZIndexControlBlock &pICB,
                          uriString &pIndexUri,
                          long pAllocatedBlocks,
                          long pBlockExtentQuota,
                          zsize_type pInitialSize,
                          long pBlockTargetSize,
                          bool pHighwaterMarking=false,
                          bool pGrabFreeSpace=false,
                          bool pBackup=false,
                          bool pLeaveOpen=true);


#ifdef __DEPRECATED__

/**
 * @brief ZIndexFile::zrebuildIndex rebuilds the current index
 *
 * NB: this routine does not open nor close the files
 *
 * - Clears the index file using ZRandomFile::Clear()
 * - Re-create each index rank from father's records content
 * - Optionally displays statistical information when pStat is set to true and pOutput is assigned to a valid FILE* value
 *
 * @param[in] pStat a flag mentionning if statistics will be produced (true) or not (false) during index rebuild
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
    template <class _Tp>
    ZStatus zrebuildRawIndex (bool pStat=false, FILE *pOutput=stdout) ;

#endif
    ZStatus openIndexFile (uriString &pIndexUri, long pIndexRank, const int pMode);
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

    ZStatus _extractRawKey(ZDataBuffer &pRawRecord, ZDataBuffer& pKeyContent);

    ZStatus _extractKeyLevel1(ZDataBuffer *pRawRecord, ZDataBuffer& pKeyContent);
#ifdef __DEPRECATED__
    ZStatus _addRawKeyValue(ZRawRecord *pZMFRecord, zrank_type &pIndexRank, const zaddress_type pZMFAddress);
#endif

    /** reserves space for key in ZBAT pool at pZBATIndex address pZMFAddress */
#ifdef __OLD_VERSION__
    ZStatus _addRawKeyValue_Prepare(ZIndexItem *&pIndexItem, zrank_type &pZBATIndex, const zaddress_type pZMFAddress);
#endif
    ZStatus _addRawKeyValue_Prepare(ZIndexItem *&pOutIndexItem,
                                    zrank_type &pZBATIndex,
                                    ZDataBuffer &pKeyContent,
                                    const zaddress_type pZMFAddress);
    /** effective write of key content in reserved record */
    ZStatus _addRawKeyValue_Commit(ZIndexItem* pIndexItem, const zrank_type pZBATIndex);
    /** index key space has been reserved in ZBAT pool and needs to be released */
    ZStatus _addRawKeyValue_Rollback(const zrank_type pIndexCommit);
    /** index key has been created in file and needs to be suppressed */
    ZStatus _addKeyValue_HardRollback(const zrank_type pIndexCommit);

    ZStatus _removeKeyValue_Prepare (ZDataBuffer &pKey, ZIndexItem *&pIndexItem, long &pIndexRank, zaddress_type &pZMFAddress);
    ZStatus _removeIndexItem_Prepare(ZIndexItem &pIndexItem, long &pIndexRank);

    ZStatus _removeKeyValue_Commit  (const zrank_type pIndexCommit);
    ZStatus _removeKeyValue_Rollback( const zrank_type pIndexCommit);
    ZStatus _removeKeyValue_HardRollback(ZIndexItem *pIndexItem, const zrank_type pIndexCommit);
/** @endcond */

  //  ZStatus getKeyIndexFields(ZDataBuffer &pIndexContent,ZDataBuffer& pKeyValue);
  //  ZStatus zprintKeyFieldsValues (const zrank_type pRank, bool pHeader=true, bool pKeyDump=false, FILE*pOutput=stdout) ;

    ZRawMasterFile* getMasterFile() {return (ZRawMasterFile*)ZMFFather;}
    ZRawMasterFile* getRawMasterFile() {return ZMFFather;}


    ZStatus _URFsearch( const ZDataBuffer &pKey,
                        ZIndexResult &pZIR,
                        const zlockmask_type pLock=ZLock_Nolock);

    ZStatus _URFsearchAll(const ZDataBuffer &pKey,
                          ZIndexCollection &pCollection,
                          const ZMatchSize_type pZMS= ZMS_MatchIndexSize);

    ZStatus _URFsearchFirst ( const ZDataBuffer        &pKeyContent,
                              ZIndexCollection           *pCollection,
                              ZIndexResult               &pZIR,
                              const ZMatchSize_type pZMS) ;

    ZStatus _URFsearchNext (ZIndexResult &pZIR, ZIndexCollection *pCollection) ;

#ifdef __DEPRECATED__
    ZStatus
    _RawsearchNext(const ZDataBuffer &pKey,
        ZRawIndexFile &pZIF,
        ZIndexCollection &pCollection,
        const ZMatchSize_type pZSC );
    ZStatus
    _Rawsearch( const ZDataBuffer &pKey,
                ZIndexResult &pZIR,
                //            ZIFCompare pZIFCompare = ZKeyCompareBinary,
                const zlockmask_type pLock=ZLock_Nolock);




    static inline
    ZStatus
    _Rawsearch(const ZDataBuffer &pKey,
              ZRawIndexFile &pZIF,
              ZIndexResult &pZIR,
  //            ZIFCompare pZIFCompare = ZKeyCompareBinary,
              const zlockmask_type pLock=ZLock_Nolock);

    static
    ZStatus
    _RawsearchAll(const ZDataBuffer &pKey,
                   ZRawIndexFile &pZIF,
                   ZIndexCollection &pCollection,
                   const ZMatchSize_type pZMS= ZMS_MatchIndexSize);

    static
    ZStatus
    _RawsearchNext(const ZDataBuffer &pKey,
                ZRawIndexFile &pZIF,
                ZIndexCollection &pCollection,
                const ZMatchSize_type pZSC );

    static
    ZStatus _RawsearchFirst (const ZDataBuffer        &pKeyContent,     // key content to find out in index
                          ZRawIndexFile               &pZIF,
                          ZIndexCollection           *pCollection,
                          ZIndexResult               &pZIR,
                          const ZMatchSize_type pZMS) ;
    static
    ZStatus _RawsearchNext (ZIndexResult             &pZIR,
                         ZIndexCollection *pCollection) ;
#endif // __DEPRECATED__
    static
    ZStatus _RawsearchIntervalFirst (const ZDataBuffer        &pKeyLow,     // lowest value of key content to find out in index
                                  const ZDataBuffer        &pKeyHigh,    // highest value of key content to find out in index
                                  ZRawIndexFile               &pZIF,       // ZIndexFile object to search on
                                  ZIndexCollection         *pCollection, // collection and context
                                  ZIndexResult             &pZIR,       // First (in key order) index rank & ZMF addresses found matching key value
                                  const bool               pExclude); // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)


    static ZStatus _RawsearchIntervalAll  (const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                                        const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                                        ZRawIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                                        ZIndexCollection       *pCollection,   // enriched collection of reference (ZIndexFile rank, ZMasterFile record address)
                                        const bool             pExclude);


    static
    ZStatus _RawsearchIntervalNext (ZIndexResult       &pZIR,
                                 ZIndexCollection   *pCollection) ;



protected:
   ZRawMasterFile*              ZMFFather=nullptr;   // pointer to ZMasterFile that instantiated the ZIndexFile object
//   uriString                    IndexUri;    // current Index File uri to be passed to ZRandomFile

#ifdef __USE_ZTHREAD__
    ZMutex _Mtx;
#endif

    ZStatus _addRebuild(ZDataBuffer &pElement, size_t pZAMIdx);
    ZStatus _rebuildReverse(void);
    ZStatus _updateZReverse(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pIdx);
    long _lookupZMFIdx (size_t pZMFIdx);

    ZStatus _alignZIndexFile(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pZIXIdx);


public: utf8String toXml(int pLevel,bool pComment);

public: ZStatus  fromXml(zxmlNode* pIndexNode, ZaiErrors* pErrorlog);

/*
  template <class _Tp>
  ZStatus _addRawKeyValue(const ZDataBuffer& pZMFRecord,  zrank_type& pIndexRank, zaddress_type pZMFAddress) {
    _Tp wClass;
    ZStatus wSt;
    _Tp wRecord;
    wRecord.fromRecord(pZMFRecord);

    ZIndexItem* wIndexItem = new ZIndexItem (wRecord.getKey(pIndexRank)) ;

    zrank_type wIndexIdxCommit;

    wClass.fromRecord(pZMFRecord);

    wIndexItem->ZMFaddress=pZMFAddress;
    wIndexItem->Operation=ZO_Push;

//    wSt=_extractRawKey(pZMFRecord,wIndexItem->KeyContent);
    if (wSt!=ZS_SUCCESS)
    {  return  wSt;}// Beware return  is multiple instructions in debug mode
    wSt=_addRawKeyValue_Prepare(*wIndexItem,wIndexIdxCommit,pZMFAddress);
    if (wSt!=ZS_SUCCESS)
    {  return  wSt;}// Beware return  is multiple instructions in debug mode
    wSt= _addRawKeyValue_Commit(wIndexItem,wIndexIdxCommit);
    delete wIndexItem;
    return  wSt;
  }

*/
private:
    long                  IndexCommitRank;
    zaddress_type         ZMFAddress;
    ZDataBuffer           CurrentKeyContent;
};// class ZIndexFile


/*
class ZIndexTable :  private ZArray<ZRawIndexFile*>
{
  typedef ZArray<ZRawIndexFile*> ZRandomFile ;
public:
  ZIndexTable() {}
  ~ZIndexTable() {}// just to call the base destructor
  using ZRandomFile::push;
  using ZRandomFile::size;
  using ZRandomFile::count;
  using ZRandomFile::last;
  using ZRandomFile::lastIdx;
  using ZRandomFile::newBlankElement;
  using ZRandomFile::operator [];

  long pop (void);
  long erase (long pRank);
  long insert(ZIndexFile* pIndexFile, long pRank);
  void clear(void);


  long searchIndexByName (const char* pName);
  long searchCaseIndexByName (const char* pName);
  long searchIndexByName (const utf8String& pName);
  long searchCaseIndexByName (const utf8String& pName);

  utf8String toXml(int pLevel,bool pComment=true);
  ZStatus fromXml(zxmlNode* pRoot,ZaiErrors*pErrorlog);
} ;

*/


/** @} */ // ZIndexGroup

//--------------Functions------------------------


} // namespace zbs

/**
 * @brief URFCompare  Compare two buffers composed each of one or many URF fields, each field potentially of variable length.
 */
//int URFCompare(const ZDataBuffer &pKey1,ZDataBuffer &pKey2) ;

/**
 * @brief extractURF_Append
 * @return
 */
ZStatus extractURF_Append(ZDataBuffer& pURFOut, const unsigned char* &pPtrIn) ;
#ifdef __DEPRECATED__
ZStatus getURFTypeAndSize( const unsigned char* pPtrIn , ZTypeBase& pType , size_t &pSize ) ;
#endif // __DEPRECATED__

/**
 * @brief skipURFUntilPosition parse an URF formatted record pointed by pPtrIn
 *        and skip URF fields (present or not present vs pFieldPresence) until position pPosition.
 * @return
 */
ZStatus skipURFUntilPosition( const unsigned char* &pPtrIn,
                              const long pPosition,
                              const ZBitset& pFieldPresence);

int URFCompareValues( const unsigned char* &pURF1,const unsigned char* pURF1_End,
                      const unsigned char* &pURF2,const unsigned char* pURF2_End);

ZStatus _printKeyFieldsValues (ZDataBuffer *wKeyContent,ZIndexFile* pZIF, bool pHeader,bool pKeyDump,FILE*pOutput);

#endif  //ZRAWINDEXFILE_H
