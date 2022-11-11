#ifndef ZINDEXFILE_H
#define ZINDEXFILE_H

#include <zindexedfile/zmfconfig.h>

#include <zindexedfile/zrawindexfile.h>


/* define functor for key extraction */

typedef ZStatus (*extractRawKey_type) (ZDataBuffer &pRawRecord,ZRawIndexFile* pZIF,ZDataBuffer* pKeyContent);


//=================================ZIndexFile===============================

namespace zbs {

/** @addtogroup ZIndexGroup
 *
 * @{
 *
*/


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
//class ZSIndexControlBlock;
class ZIndexItem;
class ZRecord;
class ZRawRecord;
class ZRawMasterFile;

class ZIndexFile : public ZRawIndexFile
{
friend class ZIndexCollection;
friend class ZRawMasterFile;
friend class ZMasterFile;


protected:
        typedef ZRawIndexFile                   _Base   ;
public:

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
    ZIndexFile  (ZRawMasterFile *pFather);
    ZIndexFile  (ZRawMasterFile *pFather,ZIndexControlBlock& pZICB);
    ZIndexFile  (ZRawMasterFile *pFather,ZKeyDictionary* pKDic, int pKeyUniversalsize,const utf8String &pIndexName ,ZSort_Type pDuplicates=ZST_NODUPLICATES);

    ZIndexFile  (ZRawMasterFile *pFather, int pKeyUniversalsize,const utf8String &pIndexName ,ZSort_Type pDuplicates=ZST_NODUPLICATES);

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

    using _Base::getFCB;
    using _Base::getFileType;




ZStatus _search( const ZDataBuffer &pKey,
                ZIndexFile &pZIF,
                ZIndexResult &pZIR,
                const zlockmask_type pLock) ;

ZStatus _searchFirst(const ZDataBuffer        &pKey,     // key content to find out in index
        ZRawIndexFile                         &pZIF,     // pointer to ZIndexControlBlock containing index description
        ZIndexCollection                     *pCollection,
        ZIndexResult                         &pZIR,
        const ZMatchSize_type                 pZMS);

ZStatus _searchNext (ZIndexResult       &pZIR,
                      ZIndexCollection*  pCollection);


ZStatus _searchAll(const ZDataBuffer        &pKey,     // key content to find out in index
                  ZRawIndexFile &pZIF,     // pointer to ZIndexControlBlock containing index description
                  ZIndexCollection &pCollection,
                  const ZMatchSize_type    pZMS);

ZStatus _searchIntervalFirst(const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                            const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                            ZIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                            ZIndexCollection       *pCollection,   // enriched collection of reference (ZIndexFile rank, ZMasterFile record address)
                            ZIndexResult           &pZIR,
                            const bool             pExclude);

ZStatus _searchIntervalNext (ZIndexResult       &pZIR,ZIndexCollection*  pCollection);


ZStatus _searchIntervalAll (const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                            const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                            ZIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                            ZIndexCollection       *pCollection,   // enriched collection of reference (ZIndexFile rank, ZMasterFile record address)
                            const bool             pExclude); // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)


ZStatus getUniversalbyRank (ZDataBuffer &pOutValue,
                            ZBitset* pFieldPresence,
                            const long pKeyRank,
                            const long pFieldRank,
                            bool pTruncate);

ZStatus addKeyValue(ZRecord *pZMFRecord, zaddress_type pZMFAddress);
ZStatus _addKeyValue_Prepare(ZIndexItem *&pIndexItem, zrank_type &pZBATIndex, const zaddress_type pZMFAddress);
ZStatus _addKeyValue_Commit(ZIndexItem *pIndexItem, const zrank_type pZBATIndex);
ZStatus _addKeyValue_Rollback(const zrank_type pIndexCommit);

ZStatus _addKeyValue_HardRollback(const zrank_type pIndexCommit);

ZStatus _removeKeyValue_Prepare(ZDataBuffer & pKey,
                                ZIndexItem* &pIndexItem,
                                long& pIndexRank,
                                zaddress_type &pZMFAddress);

ZStatus _removeKeyValue_Commit(const zrank_type pIndexCommit);
ZStatus _removeKeyValue_Rollback(const zrank_type pIndexCommit);
ZStatus _removeKeyValue_HardRollback(ZIndexItem* pIndexItem, const zrank_type pIndexCommit);

ZStatus _removeIndexItem_Prepare(ZIndexItem &pIndexItem,long & pIndexRank);
ZStatus removeIndexValue        (const ZDataBuffer& pKey , zaddress_type &pAddress);

ZStatus zrebuildIndex(bool pStat, FILE*pOutput);

/**
 * @brief _keyExtraction Extracts the Key value from ZMasterFile record data using dictionnary ZSKeyDictionary fields definition
 *
 * return s the concaneted key value in pKey ZDataBuffer.
 * - Key fields are extracted from the ZMasterFile user record .
 * - They are converted appropriately whenever required using base internal conversion routines according Dictionary data type ( ZType_type ):
 *    + atomic fields _getAtomicFromRecord()
 *    + arrays _getArrayFromRecord()
 *    + for data type Class (ZType_type) : data is simply mass-moved to key without any conversion
 *
 * @note As we are processing variable length records, if a defined key field points outside the record length,
 *       then its return ing key value is set to binary zero on the corresponding length of the field within returned Key value.
 *
 * @note At this stage _recomputeSize should have been done and total key sizes should be OK.
 *
 * @param[in] pZKDic  ZIndex dictionary (part of ZIndexControlBlock) for the index to extract key for
 * @param[in pRecord  ZMasterFile user record to extract key from
 * @param[out] pKey  Resulting concatenated key content
 * @return ZStatus
 */
ZStatus _keyExtraction(ZRecord *pRecord, ZDataBuffer& pKeyOut);

ZStatus getKeyIndexFields(ZDataBuffer &pIndexContent,ZDataBuffer& pKeyValue);
ZStatus zprintKeyFieldsValues (const zrank_type pRank,bool pHeader,bool pKeyDump,FILE*pOutput);

};// class ZIndexFile




} // namespace zbs



ZStatus
_printKeyFieldsValues (ZDataBuffer* wKeyContent,ZIndexFile* pZIF, bool pHeader,bool pKeyDump,FILE*pOutput);

#endif  //ZINDEXFILE_H
