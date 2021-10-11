#ifndef ZSINDEXFILE_H
#define ZSINDEXFILE_H

#include <zindexedfile/zmfconfig.h>

#include <zindexedfile/zrawindexfile.h>




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

class ZSIndexFile : public ZRawIndexFile
{
friend class ZSIndexCollection;
friend class ZRawMasterFile;
friend class ZSMasterFile;

protected:
        typedef ZRawIndexFile                   _Base   ;
public:

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
    ZSIndexFile  (ZSMasterFile *pFather);
    ZSIndexFile  (ZSMasterFile *pFather,ZSIndexControlBlock& pZICB);
    ZSIndexFile  (ZSMasterFile *pFather,ZSKeyDictionary* pKDic, int pKeyUniversalsize,const utf8String &pIndexName ,ZSort_Type pDuplicates=ZST_NODUPLICATES);


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


    ZStatus _keyValueExtraction(ZRecord &pRecord, ZDataBuffer& pKeyOut);


ZStatus _search( const ZDataBuffer &pKey,
                ZSIndexFile &pZIF,
                ZSIndexResult &pZIR,
                const zlockmask_type pLock) ;

ZStatus _searchFirst(const ZDataBuffer        &pKey,     // key content to find out in index
        ZSIndexFile               &pZIF,     // pointer to ZIndexControlBlock containing index description
        ZSIndexCollection         *pCollection,
        ZSIndexResult             &pZIR,
        const ZMatchSize_type    pZMS);

ZStatus _searchNext (ZSIndexResult       &pZIR,
                      ZSIndexCollection*  pCollection);


ZStatus _searchAll(const ZDataBuffer        &pKey,     // key content to find out in index
                  ZSIndexFile               &pZIF,     // pointer to ZIndexControlBlock containing index description
                  ZSIndexCollection &pCollection,
                  const ZMatchSize_type    pZMS);

ZStatus _searchIntervalFirst(const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                            const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                            ZSIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                            ZSIndexCollection       *pCollection,   // enriched collection of reference (ZSIndexFile rank, ZMasterFile record address)
                            ZSIndexResult           &pZIR,
                            const bool             pExclude);

ZStatus _searchIntervalNext (ZSIndexResult       &pZIR,ZSIndexCollection*  pCollection);


ZStatus _searchIntervalAll (const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                            const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                            ZSIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                            ZSIndexCollection       *pCollection,   // enriched collection of reference (ZSIndexFile rank, ZMasterFile record address)
                            const bool             pExclude); // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)


ZStatus getUniversalbyRank (ZDataBuffer &pOutValue,
                            ZBitset* pFieldPresence,
                            const long pKeyRank,
                            const long pFieldRank,
                            bool pTruncate);

ZStatus addKeyValue(ZRecord *pZMFRecord, zaddress_type pZMFAddress);
ZStatus _addKeyValue_Prepare(ZSIndexItem *&pIndexItem, zrank_type &pZBATIndex, const zaddress_type pZMFAddress);
ZStatus _addKeyValue_Commit(ZSIndexItem *pIndexItem, const zrank_type pZBATIndex);
ZStatus _addKeyValue_Rollback(const zrank_type pIndexCommit);

ZStatus _addKeyValue_HardRollback(const zrank_type pIndexCommit);

ZStatus _removeKeyValue_Prepare(ZDataBuffer & pKey,
                                ZSIndexItem* &pIndexItem,
                                long& pIndexRank,
                                zaddress_type &pZMFAddress);

ZStatus _removeKeyValue_Commit(const zrank_type pIndexCommit);
ZStatus _removeKeyValue_Rollback(const zrank_type pIndexCommit);
ZStatus _removeKeyValue_HardRollback(ZSIndexItem* pIndexItem, const zrank_type pIndexCommit);

ZStatus _removeIndexItem_Prepare(ZSIndexItem &pIndexItem,long & pIndexRank);
ZStatus removeIndexValue        (const ZDataBuffer& pKey , zaddress_type &pAddress);

ZStatus zrebuildIndex(bool pStat, FILE*pOutput);

/**
 * @brief ZSIndexFile::_extractKeys extracts all defined keys from pRecordContent using pMasterDic givin pKeysContent as a result.
 * @param pRecordContent
 * @param pMasterDic
 * @param pKeysContent
 * @return
 */
ZStatus _extractKey(ZRecord *pRecord, ZDataBuffer& pKeyContent);


ZStatus getKeyIndexFields(ZDataBuffer &pIndexContent,ZDataBuffer& pKeyValue);
ZStatus zprintKeyFieldsValues (const zrank_type pRank,bool pHeader,bool pKeyDump,FILE*pOutput);

};// class ZSIndexFile




} // namespace zbs



ZStatus
_printKeyFieldsValues (ZDataBuffer* wKeyContent,ZSIndexFile* pZIF, bool pHeader,bool pKeyDump,FILE*pOutput);

#endif  //ZSINDEXFILE_H
