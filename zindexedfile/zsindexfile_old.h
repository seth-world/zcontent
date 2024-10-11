#ifndef ZSINDEXFILE_H
#define ZSINDEXFILE_H

#include <zindexedfile/zmfconfig.h>

#include <cstdarg>
#include <zrandomfile/zrandomfile.h>
#include <ztoolset/ztimer.h> // to get ZTimer

//#include <ztoolset/zsacommon.h> // for types and getDataType

//#include <zam/zam_include.h> // for ZOp

#include <zindexedfile/zsindextype.h>

#include <zindexedfile/zsindexcollection.h>

#include <zcontentcommon/zoperation.h>

#include <zindexedfile/zmfstats.h>

#include <zindexedfile/zindexcontrolblock.h>


typedef ZStatus (*ZRawExtractKey) (void* pEntity,ZDataBuffer& pKeyContent);

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




#ifndef __INDEXDATA__
#define __INDEXDATA__

#ifndef __ZPRES_DEF__
#define __ZPRES_DEF__

    /* IndexPresence values meaning
   * 0 : Index to be deleted
   * 1 : Index present but not to be rebuilt
   * 2 : Index to be built or rebuilt -
   * 3 : Index created (therefore rebuilt)
   * 4 : Index errored
   * 5 : missing and not created
   */
    enum ZPRES : uint8_t
    {
      ZPRES_Nothing       = 0,
      ZPRES_ToBeDeleted   = 1,  /**<  to be deleted : 1 */
      ZPRES_Unchanged     = 2,  /**<  present but not to be rebuilt - or not to be modified if not index : 2 */
      ZPRES_ToChange      = 0x13,  /**<  present to be built or rebuilt - or to be modified if not index : 3 */
      ZPRES_ToCreate      = 0x14,  /**<  created (therefore to be built ) : 4 */
      ZPRES_Errored       = 5,  /**<  errored  : 5 */
      ZPRES_MissNotCre    = 6,   /**<  missing but not to be created  : 6 */
      ZPRES_ToRebuild     =0x10
    };

#endif // __ZPRES_DEF__

struct IndexData_st
    {
      IndexData_st()=default;
      IndexData_st(const IndexData_st& pIn) {_copyFrom(pIn);}

      IndexData_st& _copyFrom(const IndexData_st& pIn)
      {
        Status = pIn.Status;
        IndexName = pIn.IndexName;
        Duplicates = pIn.Duplicates;
        KeyUniversalSize = pIn.KeyUniversalSize;
        FCB=pIn.FCB;
        return *this;
      }

      ZPRES       Status=ZPRES_Nothing;
      utf8String  IndexName;
      ZSort_Type  Duplicates=ZST_NODUPLICATES;
      uint32_t    KeyUniversalSize=0;
      FCBParams   FCB;
    };
#endif//__INDEXDATA__


//------- Index & key data structure definitions--------------


//--------------Generic Functions--------------------------------------------------


//ZStatus _keyValueExtraction(ZSKeyDictionary* pZKDic, ZRecord &pRecord, ZDataBuffer& pKeyOut);




//=================================ZSIndexFile===============================

namespace zbs {

/** @addtogroup ZIndexGroup
 *
 * @{
 *
*/


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
//class ZSIndexControlBlock;
class ZSIndexItem;
class ZRecord;
class ZRawRecord;
class ZRawMasterFile;

class ZSIndexFile : protected ZRandomFile, public ZSIndexControlBlock
{
friend class ZSIndexCollection;
friend class ZRawMasterFile;

protected:
        typedef ZRandomFile                   _Base   ;
public:
    friend class ZSMasterFile;
//    ZSIndexControlBlock   *ZICB=nullptr;    //!< ZICB pointer to ZMF father's ZICB content
//    ZSIndexControlBlock   ZICB ;    //!< ZICB content

    ZMFStats              ZPMSStats;        //!< statistical structure
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
    ZSIndexFile  (ZRawMasterFile *pFather);
    ZSIndexFile  (ZRawMasterFile *pFather,ZSIndexControlBlock& pZICB);
    ZSIndexFile  (ZRawMasterFile *pFather,int pKeyUniversalsize,const utf8String &pIndexName ,ZSort_Type pDuplicates=ZST_NODUPLICATES);


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

    using _Base::getFCB;
    using _Base::getFileType;


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


    ZStatus setIndexURI(uriString &pUri);
    void setIndexName (utf8String &pName);

    ZStatus removeIndexFiles(ZaiErrors *pErrorLog=nullptr);

    ZFileDescriptor& getIndexFileDescriptor(void) {return _Base::ZDescriptor;}

    ZStatus  setParameters (const bool pGrabFreeSpace,
                            const bool pHighwaterMarking,
                            const ssize_t pBlockTargetSize,
                            const size_t pBlockExtentQuota)
    { return _Base::_setParameters( ZFT_ZIndexFile,
                                    pGrabFreeSpace,
                                    pHighwaterMarking,
                                    pBlockTargetSize,
                                    pBlockExtentQuota);
    }

    zrank_type getCurrentRank (void) {return ZDescriptor.CurrentRank;}
/**
   * @brief ZSIndexFile::zcreateIndex creates a new index file corresponding to the given specification ICB and ZRF parameters
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
    ZStatus zcreateIndex(ZSIndexControlBlock &pICB,
                         uriString &pIndexUri,
                         long pAllocatedBlocks,
                         long pBlockExtentQuota,
                         zsize_type pInitialSize,
                         bool pHighwaterMarking=false,
                         bool pGrabFreeSpace=false,
                         bool pReplace=false,
                         bool pLeaveOpen=true);
  /**
   * @brief ZSIndexFile::zcreateIndex creates a new index file corresponding to the given specification ICB and ZRF parameters
   *      same as previous but with explicit pBlockTargetSize
   */
    ZStatus zcreateIndex(ZSIndexControlBlock &pICB,
                          uriString &pIndexUri,
                          long pAllocatedBlocks,
                          long pBlockExtentQuota,
                          zsize_type pInitialSize,
                          long pBlockTargetSize,
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

    ZStatus _extractKey(ZRawRecord &pRawRecord,  long pKeyRank, ZDataBuffer& pKeyContent);

    ZStatus _addKeyValue(ZRawRecord *pZMFRecord, zrank_type &pIndexRank, const zaddress_type pZMFAddress);

    /** reserves space for key in ZBAT pool at pZBATIndex address pZMFAddress */
    ZStatus _addRawKeyValue_Prepare(ZRawRecord* pElement, ZSIndexItem *&pIndexItem, zrank_type &pZBATIndex, const zaddress_type pZMFAddress);

    /** reserves space for key in ZBAT pool at pZBATIndex address pZMFAddress */
    ZStatus _addKeyValue_Prepare(ZRawRecord *pElement, ZSIndexItem *&pIndexItem, zrank_type &pZBATIndex, const zaddress_type pZMFAddress);
    /** effective write of key content in reserved record */
    ZStatus _addKeyValue_Commit(ZSIndexItem* pIndexItem, const zrank_type pZBATIndex);
    /** index key space has been reserved in ZBAT pool and needs to be released */
    ZStatus _addKeyValue_Rollback(const zrank_type pIndexCommit);
    /** index key has been created in file and needs to be suppressed */
    ZStatus _addKeyValue_HardRollback(const zrank_type pIndexCommit);

    ZStatus _removeKeyValue_Prepare (ZDataBuffer &pKey, ZSIndexItem *&pIndexItem, long &pIndexRank, zaddress_type &pZMFAddress);
    ZStatus _removeIndexItem_Prepare(ZSIndexItem &pIndexItem, long &pIndexRank);

    ZStatus _removeKeyValue_Commit  (const zrank_type pIndexCommit);
    ZStatus _removeKeyValue_Rollback( const zrank_type pIndexCommit);
    ZStatus _removeKeyValue_HardRollback(ZSIndexItem *pIndexItem, const zrank_type pIndexCommit);
/** @endcond */

    ZStatus getKeyIndexFields(ZDataBuffer &pIndexContent,ZDataBuffer& pKeyValue);
    ZStatus zprintKeyFieldsValues (const zrank_type pRank, bool pHeader=true, bool pKeyDump=false, FILE*pOutput=stdout) ;



    ZStatus
    _search(const ZDataBuffer &pKey,
            ZSIndexResult &pZIR,
            //            ZIFCompare pZIFCompare = ZKeyCompareBinary,
            const zlockmask_type pLock=ZLock_Nolock);


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
   ZRawMasterFile*              ZMFFather=nullptr;   // pointer to ZMasterFile that instantiated the ZSIndexFile object
   uriString                    IndexUri;    // current Index File uri to be passed to ZRandomFile

#ifdef __USE_ZTHREAD__
    ZMutex _Mtx;
#endif

    ZStatus _addRebuild(ZDataBuffer &pElement, size_t pZAMIdx);
    ZStatus _rebuildReverse(void);
    ZStatus _updateZReverse(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pIdx);
    long _lookupZMFIdx (size_t pZMFIdx);

    ZStatus _alignZSIndexFile(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pZIXIdx);


public: utf8String toXml(int pLevel,bool pComment);


private:
    long            IndexCommitRank;
    zaddress_type   ZMFAddress;
    ZDataBuffer     CurrentKeyContent;
};// class ZSIndexFile

#ifdef __COMMENT__

class ZIndexTable :  private ZArray<ZSIndexFile*>
{
  typedef ZArray<ZSIndexFile*> _Base ;
public:
  ZIndexTable() {}
  ~ZIndexTable() {}// just to call the base destructor
  using _Base::push;
  using _Base::size;
  using _Base::count;
  using _Base::last;
  using _Base::lastIdx;
  using _Base::newBlankElement;
  using _Base::operator [];

  long pop (void);
  long erase (long pRank);
  void clear(void);

  ZStatus removeIndex(const long pIdx);

  long searchIndexByName (const char* pName);
  long searchCaseIndexByName (const char* pName);
  long searchIndexByName (const utf8String& pName);
  long searchCaseIndexByName (const utf8String& pName);

  utf8String toXml(int pLevel,bool pComment=true);
  ZStatus fromXml(zxmlNode* pRoot,ZaiErrors*pErrorlog);

} ;

#endif // __COMMENT__


/** @} */ // ZIndexGroup

//--------------Functions------------------------


ZStatus _printKeyFieldsValues (ZDataBuffer *wKeyContent,ZSIndexControlBlock* ZICB, bool pHeader,bool pKeyDump,FILE*pOutput);


} // namespace zbs


#endif  //ZSINDEXFILE_H
