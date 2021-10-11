#ifndef ZSMASTERFILE_H
#define ZSMASTERFILE_H
#include <zindexedfile/zmfconfig.h>
#include <cstdarg>
#include <zindexedfile/zmf_limits.h>
#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zsindexfile.h>
#include <zindexedfile/zskey.h>
#include <znet/zbasenet/znetcommon.h>
#include <zindexedfile/zmetadic.h>

#include <ztoolset/zutfstrings.h>

#include <ztoolset/zaierrors.h>
#include <zxml/zxml.h>

#include <zindexedfile/zsmastercontrolblock.h>
#include <zindexedfile/zindexcontrolblock.h>

#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zkeydictionary.h>

#include <zindexedfile/zrawmasterfile.h>

/*#ifdef QT_CORE_LIB
//#include <zxml/qxmlutilities.h>
#endif // QT_CORE_LIB
*/
//------------------Generic Functions--------------------------


utf8String generateIndexRootName(utf8String &pMasterRootName, const long pRank, const utf8String &pIndexName);

ZStatus generateIndexURI(uriString pMasterUri, uriString &pDirectory, uriString &pZIndexFileUri, const long pRank, const utf8String &pIndexName);


namespace zbs //========================================================================
{





/**
 * @brief The ZSMasterFile class Structured Master file : a dictionarized version of ZRawMasterFile
 *
 *  - has a metadictionary describing its content (mandatory)
 *  - data is stored using Universal Record Format (URF)
 *  - fields may be of varying length (strings, byte sequences or blobs,...)
 *  - fields may be present or omitted :
 *      . each record has a bitfield indicating each field presence
 *  - May have any indexes gathering one or more of its fields (optional)
 *      . a key dictionary is setup mentionning fields to integrate in the index:
 *          the order of key dictionary defines the order of fields within the key sorting order
 */



/**
 * @brief The ZSMasterFile class This is the master object derived from ZRandomFile that allows to extend it to index management
 *
 *  ZRecord : operations are done using dictionary (meta dictionary and key dictionaries )
 *  As a result, dictionary processing gives a ZRawRecord or uses as input a ZRawRecord.
 *
 *  ZRawRecord : operations are done without dictionaries, record and keys contents are directly fed by application.
 *  Manages low level operations using key buffers and record buffer.
 *
 *
 *
 *
 * @see @ref ZSMasterFileGroup
 *
 */
//class ZSMasterFile : protected ZRandomFile  // cannot publicly access to ZRandomFile data & methods
class ZSMasterFile : public ZRawMasterFile
{

public:
    friend class ZSKey;
    friend class ZSIndexCollection;
    friend class ZSJournal;

typedef ZRawMasterFile _Base ;

    ZSMasterFile(void) ;
//    ZSMasterFile(uriString pURI);
    ZSMasterFile(bool pHistory) ;
    ~ZSMasterFile(void);


    ZSKey* createZKeyByName (const char* pKeyName);
    ZSKey* createZKey (const long pKeyNumber);

// what we can externally use of ZRandomFile methods (included from ZIndexFile class)

    using ZRandomFile::lastIdx;
    using ZRandomFile::UsedSize;

    using ZRandomFile::getSize;
    using ZRandomFile::isEmpty;

    using ZRandomFile::freepoolSize;
    using ZRandomFile::last;


    using _Base::zgetCurrentRank;
    using _Base::zgetCurrentLogicalPosition ;
    using _Base::setLogicalFromPhysical ;
    using _Base::zremove;

    using _Base::zget;
    using _Base::zgetByAddress;

    using _Base::zgetBlockDescriptor;

    using _Base::getURIContent;
    using _Base::getURIHeader;

    using _Base::zgetWAddress;
    using _Base::zgetNextWAddress;
    using _Base::zgetLastWAddress;
    using _Base::zgetPreviousWAddress;


//----------------Statistical-------------------------------
    using _Base::zfullDump;
    using _Base::zheaderDump;
    using _Base::zcontentDump;

    using _Base::zsurfaceScan;

//    using _Base::zclearFile;  // for tests only : must be suppressed imperatively

//--------------Setting parameters------------------------------------
    ZStatus  setFCBParameters (const bool pGrabFreeSpace,
                              const bool pHighwaterMarking,
                              const size_t pBlockTargetSize,
                              const size_t pBlockExtentQuota)
    { return _Base::_setParameters(ZFT_ZSMasterFile,
                                   pGrabFreeSpace,
                                   pHighwaterMarking,
                                   pBlockTargetSize,
                                   pBlockExtentQuota);}

    ZStatus setIndexFilesDirectoryPath (uriString &pPath);
    ZStatus setJournalLocalDirectoryPath (uriString &pPath);

//    ZStatus setPath(uriString &pPath) { return _Base::setPath(pPath); }  // defined as virtual in ZRandomFile base class
//----------------End setting parameters--------------------
// ZSMasterFile creation
#ifdef __COMMENT__
    void _defineKeyFieldsList (ZSIndexControlBlock pZICB, long pCount,...);

    void _addIndexField (ZArray<ZSIndexField_struct>& pZIFField, descString &pName, size_t pOffset, size_t pLength);
#endif // __COMMENT__
    void _addIndexKeyDefinition (ZSIndexControlBlock* pZICB,
                                  ZSKeyDictionary &pZKDic,
                                  utf8String &pIndexName,
                                  uint32_t pKeyUniversalSize,
//RFFU                 bool pAutoRebuild,
                                  ZSort_Type pDuplicates);

    ZStatus zprintIndexFieldValues (const zrank_type pIndex, const zrank_type pIdxRank, bool pHeader=true, bool pKeyDump=false,FILE *pOutput=stdout);
    /**
     * @brief zgetIndexSize returns the size (number of records) of the index number pIndex
     * @param pIndex Index number within ZSMasterFile to return the size for
     * @return
     */
    ssize_t    zgetIndexCount(const zrank_type pIndexRank) {return ZMCB.IndexTable[pIndexRank]->getRecordCount();}
 //   ZSKeyDictionary * zgetKeyDictionary(const zrank_type pIndexRank) {return ZMCB.IndexTable[pIndexRank]->ZKDic ;}

    ZRawIndexFile* zgetIndexFile(const zrank_type pIndexRank) {return ZMCB.IndexTable[pIndexRank];}

    ZStatus zcreate(ZMetaDic *pMetaDic,
                    const uriString pURI,
                    long pAllocatedBlocks,
                    long pBlockExtentQuota,
                    long pBlockTargetSize,
                    const zsize_type pInitialSize,
                    bool pHighwaterMarking,
                    bool pGrabFreeSpace,
                    bool pJournaling,
                    bool pBackup=false,
                    bool pLeaveOpen=false);

    ZStatus zcreate (ZMetaDic *pMetaDic, const uriString pURI, const zsize_type pInitialSize, bool pBackup=false, bool pLeaveOpen=false);

    ZStatus zcreate (ZMetaDic *pMetaDic,const char* pPathName, const zsize_type pInitialSize, bool pBackup=false, bool pLeaveOpen=false);

    ZStatus zcreateIndex (ZSKeyDictionary* pKeyDic,
                          const utf8String &pIndexName,
// RFFU                   bool pAutorebuild,
                          ZSort_Type pDuplicates,
                          bool pBackup);

// overload of previous function
#ifdef __COMMENT__
    ZStatus zcreateIndex (ZSKeyDictionary &pZKDic,
                          const utf8_t *pIndexName,
// RFFU                   bool pAutorebuild,
                          ZSort_Type pDuplicates,
                          bool pBackup=true);
#endif // __COMMENT__

    ZStatus zopen       (const uriString pURI, const int pMode=ZRF_All); // superseeds ZRandomfile zopen
    ZStatus zopen       (const int pMode=ZRF_All) {return (zopen(getURIContent(),pMode));}
//    ZStatus zopen       (const int pMode=ZRF_All) {return zopen(ZDescriptor.URIContent,pMode);}

    ZStatus zadd      (ZRecord *pRecord );
    ZStatus zinsert   (ZRecord *pRecord, const zrank_type pZMFRank);
    ZStatus zget      (ZRecord* pRecord, const zrank_type pZMFRank);

    ZStatus zaddRaw   (ZRawRecord &pRecord);
    ZStatus _insertRaw(ZRawRecord &pRecord, const zrank_type pZMFRank);
    ZStatus _getRaw   (ZRawRecord &pRecord, const zrank_type pZMFRank);

    ZStatus zremoveByRank     (const zrank_type pZMFRank) ;
    ZStatus zremoveByRankR    (ZRecord *pZMFRecord, const zrank_type pZMFRank);


    ZStatus writeControlBlocks(void);
    ZStatus readControlBlocks(void);
    ZStatus readJCBfromHeader(void);
    ZStatus _getJCBfromReserved(void);

    ZStatus zclose (void);

//---------search and get operations--------------------------------
//

//-------relative get  (use ZRandomFile base routines)------------------------
//

//    ZStatus zgenerateKeyValueList (ZDataBuffer &pKey , long pKeyNumber, int pCount,...);
//    ZStatus zgenerateKeyValue (ZSIndexControlBlock& pICB, ZArray<void *> &pKeyValues, ZDataBuffer& pKey);

    ZStatus zsearch (ZDataBuffer &pRecord, ZDataBuffer &pKeyValue, const long pIndexNumber=0);
    ZStatus zsearch (ZDataBuffer &pRecord,ZSKey *pKey);

    ZStatus zsearchAll (ZDataBuffer &pKeyValue,
                        const long pIndexNumber,
                        ZSIndexCollection &pIndexCollection,
                        const ZMatchSize_type pZMS=ZMS_MatchKeySize);

    ZStatus zsearchAll (ZSKey &pZKey, ZSIndexCollection& pIndexCollection);

    ZStatus zsearchFirst (ZSKey &pZKey, ZDataBuffer &pRecord, ZSIndexCollection *pCollection);
    ZStatus zsearchNext (ZSKey &pZKey, ZDataBuffer &pRecord, ZSIndexCollection *pCollection);


    using ZRandomFile::zsearchFieldAllCollection;
    using ZRandomFile::zsearchFieldFirstCollection;
    using ZRandomFile::zsearchFieldNextCollection;


    ZStatus zsearchInterval (ZSKey &pZKeyLow, ZSKey &pZKeyHigh,const zrank_type pIndexNumber,ZSIndexCollectionContext *pSearchContext );


//-------------Reports------------------------------------
    void ZMCBreport(void);
//---------------------Utilities-----------------------------------
    static
    ZStatus zrepairIndexes (const char *pZMFPath,
                          bool pRepair=false,
                          bool pRebuildAll=false,
                          FILE *pOutput=nullptr);

    ZStatus zclearMCB (FILE *pOutput=nullptr);

    static
    void    zdowngradeZMFtoZRF (const char* pZMFPath,FILE* pOutput=nullptr);
    static
    void    zupgradeZRFtoZMF (const char* pZRFPath, FILE* pOutput=nullptr);

    ZStatus zremoveIndex (const long pIndexRank);
//------------Surface utilities---------------------------------

    ZStatus zreorgFile(bool pDump=false, FILE *pOutput=stdout); // replaces the ZRandomFile::zreorgFile()
    ZStatus zindexRebuild (const long pIndexRank,bool pStat=false, FILE *pOutput=stdout);


//--------------XML reports & utilities---------------------------------

// write XML routines do not require QT_CORE_LIB because no QT module is required

    void _writeXML_ZRandomFileHeader(ZSMasterFile& pZMF,FILE *pOutput);

    void _writeXML_Index(ZSMasterFile& pZMF,const long pIndexRank,FILE* pOutput);

    void _writeXML_MetaDic(ZMetaDic* ZMDic,FILE* pOutput);

    void _writeXML_KDic(ZSKeyDictionary* ZKDic, FILE* pOutput);

    ZStatus generateXML_IndexDefinition(const long pIndexRank, const char *pFilename);
    
    void XmlWriteFileDefinition(FILE *pOutput=nullptr) ;
    ZStatus generateXML_IndexDefinition(const long pIndexRank, FILE *pOutput=nullptr);
    static
    void XmlWriteFileDefinition(const char *pFilePath, FILE *pOutput=nullptr) ;


    static
    ZStatus _loadXMLKeyField(zxmlNode* &wNode, ZSKeyDictionary*&pZKDic);
    static
    ZStatus _loadXMLDictionary(zxmlNode* &wNode, ZSKeyDictionary *pZKDic);

    static
    ZStatus zapplyXMLFileDefinition(const utf8_t *pXMLPath, const utf8_t *pContentFilePath=nullptr, bool pRealRun=false, FILE *pOutput=nullptr);
    static
    ZStatus zapplyXMLFileDefinition_old(const utf8_t *pXMLPath, const utf8_t*pContentFilePath=nullptr, bool pRealRun=false, FILE *pOutput=nullptr);
    static
    int zextractXML_MetaDic(const char *pFilePath,
                            ZMetaDic* &pMetaDic,
                            ZaiErrors* pErrorlog);
    static
    int zextractXML_AllIndexes(const char *pFilePath,
                                ZArray<ZSIndexControlBlock *> &pZICBList,
                                ZMetaDic *pMetaDic,
                                ZaiErrors* pErrorlog);
    static
    ZStatus zloadXML_Dictionary(const utf8_t* pFilePath, ZSKeyDictionary &pZKDIC, ZMetaDic *pMetaDic);

    static
    ZStatus
    _XMLLoadAndControl(const char *pFilePath,
                       zxmlDoc*     &pXmlDoc,
                       zxmlElement *&pRootElement,
                       const utf8_t* pRootName,
                       const utf8_t* pRootAttrName,
                       const utf8_t* pRootAttrValue,
                       ZaiErrors *pErrorLog,
                       FILE* pOutput=nullptr);
    static
    ZStatus zloadXML_AllIndexes(const utf8_t *pFilePath,
                                zbs::ZArray<ZSIndexControlBlock *> &pZICBList,
                                ZMetaDic *pMetaDic,
                                ZaiErrors *pErrorlog);
    static
    zxmlNode* _searchForChildTag(zxmlNode *pTopNode, const char *pTag);

    static
    ZStatus zgetXMLIndexRank(ZSMasterFile &wMasterFile,
                             ZSIndexControlBlock &wZICB,
                             zxmlNode* &pNode,
                             long &wMissingTags,
                             long &wMissMandatoryTags,
                             char &IndexPresence,
                             long &IndexRank,
                             FILE*wOutput);
/*    static
    ZStatus zapplyXMLIndexRankDefinition(const char* pXMLPath,
                                              const char *pContentFilePath,
                                              bool pRealRun,
                                              FILE*pOutput);
*/
//#endif

//-------- Stats-----------------------------------------------


    using _Base::ZRFPMSReport;
    using _Base::ZRFstat;

    void    ZRFPMSIndexStats( const long pIndex,FILE *pOutput=stdout);

    ZStatus zstartIndexPMSMonitoring(const long pIndex);
    ZStatus zstopIndexPMSMonitoring (const long pIndex);
    ZStatus zreportIndexPMSMonitoring (const long pIndex, FILE* pOutput=stdout);

    void    zstartPMSMonitoring (void);
    void    zendPMSMonitoring (void);
    void    zreportPMSMonitoring (FILE* pOutput);


//---------End Stats-------------------------------------------

    ZStatus getRawIndex(ZSIndexItem &pIndexItem, const zrank_type pIndexRank, const zrank_type pIndexNumber);

    ZStatus getKeyIndexFields(ZDataBuffer &pKeyFieldValues, const zrank_type pIndexRank,const zrank_type pIndexNumber);

// -------------------ZSMasterFile operators-------------------------

/**
 * @brief operator << Adds a new record (ZDataBuffer) at the logical end of the ZSMasterFile. Equivalent to push
 * @param pZMF
 * @param pInput
 * @return
 */
friend ZStatus operator << (ZSMasterFile &pZMF,ZRecord& pInput)
     {
     return (pZMF.push(pInput));
     }
/**
 * @brief operator -- Remove the last logical record of the ZSMasterFile. Equivalent to pop()
 * @return
 */
    long operator -- (void)
     {
     ZStatus wSt;
     wSt=zremoveByRank(lastIdx());
     if (wSt!=ZS_SUCCESS)
                    return -1;
     return(lastIdx());
     }

//------------------end operators------------------------


//------------Add sequence---------------------------------------
/** @cond Development */
    ZStatus _add_RollbackIndexes ( ZArray<zrank_type> &pIndexRankProcessed);
    ZStatus _add_HardRollbackIndexes (ZArray<zrank_type> &pIndexRankProcessed);
    ZStatus _add_CommitIndexes (ZArray <ZSIndexItem*>  &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed);

//-----------End Add sequence------------------------------------

// ----------Remove sequence-------------------------------------


//    ZStatus _removeByRank  (ZFileDescriptor &pDescriptor, ZMasterControlBlock &pZMCB, ZDataBuffer &pZMFRecord, const long pZMFRank);
    ZStatus _removeByRank  (ZRecord *pZMFRecord, const zrank_type pZMFRank);

    ZStatus _removeByRankRaw  (ZRawRecord *pZMFRecord, const zrank_type pZMFRank);


    static inline
    ZStatus _remove_RollbackIndexes (ZSMasterControlBlock &pZMCB, ZArray<zrank_type> &pIndexRankProcessed);
    static inline
    ZStatus _remove_HardRollbackIndexes (ZSMasterControlBlock& pZMCB,
                                         ZArray<ZSIndexItem*> &pIndexItemList,
                                         ZArray<zrank_type> &pIndexRankProcessed);
    static inline
    ZStatus _remove_CommitIndexes (ZSMasterControlBlock& pZMCB, ZSIndexItemList &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed);
/** @endcond */
//-----------End Remove sequence------------------------------------

//
//-------------------base methods overload-----------------
//
    ZStatus push(ZRecord &pElement) ;
    ZStatus push_front (ZRecord *pElement) {return(zinsert (pElement,0L));}

//    long move (size_t pDest, size_t pOrig,size_t pNumber=1)     _METHOD_NOT_ALLOWED__  // forbidden  (because of possible Index corruption)
//    long swap (size_t pDest, size_t pOrig,  size_t pNumber=1)   _METHOD_NOT_ALLOWED__  // forbidden  (because of possible Index corruption)

    ZStatus insert (ZDataBuffer& pElement, const size_t pZAMIdx) ;                 // ZAM transmitted an insert operation: it shifts all ZAM indexes > pZAMIdx

    ZStatus erase(const size_t pIdx) ;
    ZStatus erase(const size_t pIdx,const size_t pNumber ) ;

    ZStatus replace(ZDataBuffer &pElement,const size_t pIdx);

    long pop(void)     ;
    long pop_front(void)             ;

//    ZDataBuffer &popR(void)                  _METHOD_NOT_ALLOWED__
//    ZDataBuffer &popR_front(void)            _METHOD_NOT_ALLOWED__
    ZDataBuffer &popRP(ZDataBuffer &pReturn) ;

    ZStatus _seek (ZSMasterControlBlock &pMCB, long pIndexNumber, const ZDataBuffer &pKey,ZDataBuffer &pUserRecord);


    ZRecord* generateRecord();


}; //--------------------end class ZSMasterFile-------------------------------


/** @}  */  // end ZSMasterFileGroup group

// =====================end ZSMasterFile===============================

} // namespace zbs

#endif // ZSMASTERFILE_H
