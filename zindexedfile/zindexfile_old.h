#ifndef ZINDEXFILE_H
#define ZINDEXFILE_H

#include <zindexedfile/zmfconfig.h>

#include <cstdarg>
#include <zrandomfile/zrandomfile.h>
#include <ztoolset/zdate.h> // to get ZTimer

//#include <ztoolset/zsacommon.h> // for types and getDataType

#include <zam/zam_include.h> // for ZOp

#include <zindexedfile/zindextype.h>

#include <zindexedfile/zindexcollection.h>


#ifdef ZVerbose
extern ZVerbose_type ZVerbose;
extern FILE* ZVerboseOutput;
#endif

#ifndef ZMASTERFILE_CPP
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

//
//  Key fields description
//

#ifndef ZIFKEYTYPE
#define ZIFKEYTYPE
enum ZIFKeyType_type : unsigned char
{
    ZIF_Nothing,
    ZIF_Alpha ,
    ZIF_Binary,
    ZIF_Other
};

#endif // ZIFKEYTYPE




/**
 @brief The ZIndexItem class  The key effective content as it will be stored using ZIndexFile object (and not its Fields structure definition).

*/
class ZIndexItem {
public:
        zaddress_type ZMFaddress;    //!< Master file block record address to link index key with
        ZOp           Operation;     //!< this is NOT stored on index file (see toFileKey() method) but only for history & journaling purpose
        ZDataBuffer   KeyContent;    //!< extracted key content from user record according key extraction rules. Size of content is fixed and value is ZIndexControlBlock::KeySize.



       void clear (void) {KeyContent.clearData(); ZMFaddress=0L; Operation = ZO_Nothing;
                          //State = ZAMNothing;
                          return;}
       ZDataBuffer& toFileKey(void);
       ZIndexItem&  fromFileKey (ZDataBuffer &pFileKey);
private:
        ZDataBuffer Exchange;
};

class ZIndexItemList : public ZArray<ZIndexItem*>
{
typedef ZArray<ZIndexItem*> _Base;
public:
    ~ZIndexItemList(void)
        {
        clear();
        }

    void clear(void)
        {
        while (size()>0)
        delete popRP(&wItem);
        _Base::clear();
        }

ZIndexItem* wItem;
};



#ifndef __ZINDEXCONTROLBLOCK_DEFINED__
#define __ZINDEXCONTROLBLOCK_DEFINED__
/**
 * @brief The ZIndexControlBlock class  Defines the operational data that describes one index.
 *
 * This infraData is stored in the ZIndexFile Header AND in the ZMasterFile index collection within its reserved Header block (ZMasterControlBlock)
 */
class ZIndexControlBlock {
public:
    int32_t                 StartSign ;         //!< ZICB block start marker
    ZBlockID                BlockID;            //!< must be ZBID_ICB
    long                    ZMFVersion;         //!< Self explainatory
    ssize_t                 ICBSize;            //!< ICB size when written in file header (ZReserved header field)
    descString              Name;               //!< Name for the index is deduced from Master File name + index rank
    ZIFKeyType_type         KeyType;            //!< RFFU
    bool                    AutoRebuild;        //!< RFFU
    ZSort_Type              Duplicates;         //!< Index key is allowing duplicates (ZST_DUPLICATES) or is not allowing duplicates (ZST_NODUPLICATES)
    CZKeyDictionary         ZKDic;              //!< Fields dictionary for the key
    void clear(void)
    {
        BlockID=ZBID_ICB;
        StartSign=cst_ZSTART;
        KeyType=ZIF_Nothing;
        ZKDic.clear();
        ZMFVersion = __ZMF_VERSION__;
        return;
    };
    ZDataBuffer &           _exportICB (void) ;
    ZStatus     _importICB (ZDataBuffer &pRawICB) ;
    ZStatus     _importICB (unsigned char* pBuffer);

    ZStatus     zkeyValueExtraction (ZDataBuffer &pRecord, ZDataBuffer& pKey);

    template <class _Tp>
    /**
     * @brief zaddFieldToZKDic
    * - Analyzes and gets the type of data using zgetZType() function.
    * - Deduces from the data type (ZType_type) its true natural data length as well as its (Key) internal data length.
    *
    * @note template is required here because 'auto' type hides arrays (char [n] is converted to char*).
    *
    * @param[in] pFieldName user name of the field. Only for readability.
    * @param[in] pZMFOffset    offset of the field IN THE USER RECORD (beginning = 0)
    * @param[in-out] pZKDic a ZDictionay : ZArray of ZIndexField_struct to which field definition will be added
    * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
    */
    ZStatus     zaddFieldToZKDic (const char *pFieldName,const size_t pZMFOffset)
    {
        ZStatus wSt;
        wSt=ZKDic.zaddField<_Tp>(pFieldName,pZMFOffset);
        if (wSt!=ZS_SUCCESS)
                return wSt;
        return wSt;
    }

   ssize_t IndexRecordSize (void)   //!< computes the size of a ZIndex key record
            { return (ZKDic.InternalSize + sizeof(zaddress_type)); }

private:
    ZDataBuffer     Exchange;
};

#endif // __ZINDEXCONTROLBLOCK_DEFINED__

//--------------Generic Functions--------------------------------------------------


ZStatus _keyValueExtraction(ZKeyDictionary& pZKDic, ZDataBuffer &pRecord, ZDataBuffer& pKey);


//=================================ZIndexFile===============================



/**
 * @brief The ZIndexFile class This object holds and manages at run-time an index file associated with a ZMasterFile object ( Father ).

ZIndexFile does not own the key definitions : Key definitions (dictionary) are local to ZMasterFile for which key has been defined.
ZIndexFile index definition is stored in a ZIndexControlBlock (ZICB) that gives all necessary information about how to extract and format key fields from a record coming from its father (ZMasterFile).
ZIndexFile manages
 - key values insertion / suppression. For doing this, it extracts and formats appropriate data from its father record using its ZICB.
 - searches on its index data.

Integrity controls are done to garanty an alignment with its ZMasterFile father.

@note this class is transparently created and used from ZMasterFile class object using appropriate ZMF routines.

 */
class ZMasterFile;

class ZIndexFile : protected ZRandomFile
{
friend class ZIndexCollection;
friend class ZMasterFile;
protected:
        typedef ZRandomFile                   _Base   ;
public:
    friend class ZMasterFile;
    ZIndexControlBlock                    *ZICB=nullptr;    //!< ZICB pointer to ZMF father's ZICB content
    zstatistics                            ZPMSStats;       //!< statistical structure
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
    ZIndexFile  (ZMasterFile *pFather);
    ZIndexFile  (ZMasterFile *pFather,ZIndexControlBlock* pZICB);

    ~ZIndexFile() {}
    //~ZIndexFile() {if (ZMFFather!=nullptr)
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
    void setIndexName (utfdescString &pName);
    void setICB (ZIndexControlBlock *pICB);

    ZStatus removeIndexFiles(void);

    ZFileDescriptor& getIndexFileDescriptor(void) {return _Base::ZDescriptor;}

    zrank_type getCurrentRank (void) {return ZDescriptor.CurrentRank;}

    ZStatus zcreateIndex(ZIndexControlBlock *pICB,
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

    ZStatus writeIndexControlBlock(void);

    ZStatus removeIndexValue    (const ZDataBuffer& pKey , zaddress_type &pAddress);
//    ZStatus insertIndexValue    (ZDataBuffer& pRecord ,zaddress_type &pAddress, long &pZMFRank);

/*    ZStatus addRollback         (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress);
    ZStatus removeRollback      (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress);
    ZStatus insertRollback      (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress);
*/
    ZStatus searchIndex     (ZDataBuffer& KeyValue);

    ZStatus clearIndexFile  (void);
/** @cond Development */
    ZStatus _addKeyValue(ZDataBuffer &pZMFRecord, long &pIndexRank, const zaddress_type pZMFAddress);

    ZStatus _addKeyValue_Prepare(ZDataBuffer &pElement, ZIndexItem *&pIndexItem, long& pIndexCommit, const zaddress_type pZMFAddress);
    ZStatus _addKeyValue_Commit(ZIndexItem* pIndexItem, const long pIndexCommit);
    ZStatus _addKeyValue_Rollback(const long pIndexCommit);
    ZStatus _addKeyValue_HardRollback(const long pIndexCommit);

    ZStatus _removeKeyValue_Prepare (ZDataBuffer &pKey, ZIndexItem *&pIndexItem, long &pIndexRank, zaddress_type &pZMFAddress);
    ZStatus _removeKeyValue_Commit  (const long pIndexCommit);
    ZStatus _removeKeyValue_Rollback( const long pIndexCommit);
    ZStatus _removeKeyValue_HardRollback(ZIndexItem *pIndexItem, const long pIndexCommit);
/** @endcond */

    ZStatus getKeyIndexFields(ZDataBuffer &pIndexContent,ZDataBuffer& pKeyValue);
    ZStatus zprintKeyFieldsValues (const long pRank, bool pHeader=true, bool pKeyDump=false, FILE*pOutput=stdout) ;

    static inline
    ZStatus
    _search(const ZDataBuffer &pKey,
            ZIndexFile &pZIF,
            ZIndexResult &pZIR,
//            ZIFCompare pZIFCompare = ZKeyCompareBinary,
            const zlock_type pLock=ZLock_Nolock);

    static
    ZStatus
    _searchAll(const ZDataBuffer &pKey,
               ZIndexFile &pZIF,
               ZIndexCollection &pCollection,
               const ZMatchSize_type pZMS= ZMS_MatchIndexSize);
    static
    ZStatus
    _searchNext(const ZDataBuffer &pKey,
                ZIndexFile &pZIF,
                ZIndexCollection &pCollection,
                const ZMatchSize_type pZSC );

    static
    ZStatus _searchFirst (const ZDataBuffer        &pKey,     // key content to find out in index
                          ZIndexFile               &pZIF,
                          ZIndexCollection         *pCollection,
                          ZIndexResult             &pZIR, const ZMatchSize_type pZMS) ;
    static
    ZStatus _searchNext (ZIndexResult             &pZIR,
                         ZIndexCollection *pCollection) ;

    static
    ZStatus _searchIntervalFirst (const ZDataBuffer        &pKeyLow,     // lowest value of key content to find out in index
                                  const ZDataBuffer        &pKeyHigh,    // highest value of key content to find out in index
                                  ZIndexFile               &pZIF,       // ZIndexFile object to search on
                                  ZIndexCollection         *pCollection, // collection and context
                                  ZIndexResult             &pZIR,       // First (in key order) index rank & ZMF addresses found matching key value
                                  const bool               pExclude); // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)



    static
    ZStatus _searchIntervalNext (ZIndexResult       &pZIR,
                                 ZIndexCollection   *pCollection) ;

    static
    ZStatus _searchIntervalAll   (const ZDataBuffer        &pKeyLow,     // lowest value of key content to find out in index
                                  const ZDataBuffer        &pKeyHigh,    // highest value of key content to find out in index
                                  ZIndexFile               &pZIF,       // ZIndexFile object to search on
                                  ZIndexCollection         *pCollection, // collection and context
                                  const bool               pExclude); // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)


private:
   void*                        ZMFFather;   // pointer to ZMasterFile that instantiated the ZIndexFile object
   uriString                    IndexUri;    // current Index File uri to be passed to ZRandomFile

#ifdef __USE_ZTHREAD__
    ZMutex _Mtx;
#endif

    ZStatus _addRebuild(ZDataBuffer &pElement, size_t pZAMIdx);
    ZStatus _rebuildReverse(void);
    ZStatus _updateZReverse(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pIdx);
    long _lookupZMFIdx (size_t pZMFIdx);

    ZStatus _alignZIndexFile(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pZIXIdx);

private:
    long            IndexCommitRank;
    zaddress_type   ZMFAddress;
    ZDataBuffer     CurrentKeyContent;
};// class ZIndexFile

/** @} */ // ZIndexGroup

//!--------------Functions------------------------
//!
//!

} // namespace zbs


#endif  //ZINDEXFILE_H
