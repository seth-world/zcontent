#ifndef ZMASTERFILE_H
#define ZMASTERFILE_H
#include <zindexedfile/zmfconfig.h>
#include <cstdarg>
#include <zindexedfile/zmf_limits.h>
#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zindexfile.h>
#include <zindexedfile/zskey.h>
#include <znet/zbasenet/znetcommon.h>
#include <zindexedfile/zmetadic.h>

#include <ztoolset/zutfstrings.h>

#include <ztoolset/zaierrors.h>
#include <zxml/zxml.h>

#include <zindexedfile/zmastercontrolblock.h>
#include <zindexedfile/zindexcontrolblock.h>

#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zkeydictionary.h>

#include <zindexedfile/zrawmasterfile.h>

#include <zindexedfile/zdictionaryfile.h>

/*#ifdef QT_CORE_LIB
//#include <zxml/qxmlutilities.h>
#endif // QT_CORE_LIB
*/
//------------------Generic Functions--------------------------



namespace zbs //========================================================================
{





/**
 * @brief The ZMasterFile class Structured Master file : a dictionarized version of ZRawMasterFile
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
 * @brief The ZMasterFile class This is the master object derived from ZRandomFile that allows to extend it to index management
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
 * @see @ref ZMasterFileGroup
 *
 */
//class ZMasterFile : protected ZRandomFile  // cannot publicly access to ZRandomFile data & methods
class ZMasterFile : public ZRawMasterFile
{

public:
    friend class ZSKey;
    friend class ZSIndexCollection;
    friend class ZSJournal;

    friend class ZContentVisuMain;

typedef ZRawMasterFile _Base ;

    ZMasterFile(void) ;
//    ZMasterFile(uriString pURI);
    ZMasterFile(bool pHistory) ;
    ~ZMasterFile(void);


    ZSKey* createZKeyByName (const char* pKeyName);
    ZSKey* createZKey (const long pKeyNumber);

// what we can externally use of ZRandomFile methods (included from ZIndexFile class)

    using ZRandomFile::lastIdx;
    using ZRandomFile::UsedSize;

    using ZRandomFile::getSize;
    using ZRandomFile::isEmpty;

    using ZRandomFile::freepoolSize;
    using ZRandomFile::last;


    using _Base::getCurrentRank;
    using _Base::getLogicalPosition ;
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

    using _Base::getFileDescriptor;


//----------------Statistical-------------------------------
    using _Base::_fullDump;
    using _Base::_headerDump;
    using _Base::_contentDump;

    using _Base::_surfaceScan;

//    using _Base::zclearFile;  // for tests only : must be suppressed imperatively

//--------------Setting parameters------------------------------------
    ZStatus  setFCBParameters (const bool pGrabFreeSpace,
                              const bool pHighwaterMarking,
                              const size_t pBlockTargetSize,
                              const size_t pBlockExtentQuota)
    { return _Base::_setParameters(ZFT_ZMasterFile,
                                   pGrabFreeSpace,
                                   pHighwaterMarking,
                                   pBlockTargetSize,
                                   pBlockExtentQuota);}

    ZStatus setIndexFilesDirectoryPath (uriString &pPath);
    ZStatus setJournalLocalDirectoryPath (uriString &pPath);

//    ZStatus setPath(uriString &pPath) { return _Base::setPath(pPath); }  // defined in ZRandomFile base class
//----------------End setting parameters--------------------
// ZMasterFile creation
#ifdef __COMMENT__
    void _defineKeyFieldsList (ZSIndexControlBlock pZICB, long pCount,...);

    void _addIndexField (ZArray<ZSIndexField_struct>& pZIFField, descString &pName, size_t pOffset, size_t pLength);

    void _addIndexKeyDefinition (ZIndexControlBlock* pZICB,
                                  ZSKeyDictionary &pZKDic,
                                  utf8String &pIndexName,
                                  uint32_t pKeyUniversalSize,
//RFFU                 bool pAutoRebuild,
                                  ZSort_Type pDuplicates);
#endif // __COMMENT__

//    void setDictionary (const ZMFDictionary& pDictionary);
    ZStatus addKeyToDic(ZKeyDictionary* pKeyDic, long &pOutKeyRank);

    ZStatus zprintIndexFieldValues (const zrank_type pIndex, const zrank_type pIdxRank, bool pHeader=true, bool pKeyDump=false,FILE *pOutput=stdout);
    /**
     * @brief zgetIndexSize returns the size (number of records) of the index number pIndex
     * @param pIndex Index number within ZMasterFile to return the size for
     * @return
     */
    ssize_t    zgetIndexCount(const zrank_type pIndexRank) {return IndexTable[pIndexRank]->getRecordCount();}
 //   ZSKeyDictionary * zgetKeyDictionary(const zrank_type pIndexRank) {return ZMCB.IndexTable[pIndexRank]->ZKDic ;}

    ZRawIndexFile* zgetIndexFile(const zrank_type pIndexRank) {return IndexTable[pIndexRank];}


    ZStatus zcreateMasterFile(uriString *pDictionary,
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

    ZStatus zcreateMasterFile ( uriString *pDictionary,
                      const uriString pURI,
                      const zsize_type pInitialSize,
                      bool pBackup=false,
                      bool pLeaveOpen=false);

 //   ZStatus zcreate (ZMetaDic *pMetaDic,const char* pPathName, const zsize_type pInitialSize, bool pBackup=false, bool pLeaveOpen=false);
/**
 * @brief ZMasterFile::zcreateIndexWithDefinition Generates a new raw index (meaning a new ZRandomFile data + header) from a given key definition.
 *
 * This routine will store the given key definition within Master dictionary and will
 * create a new index with the files structures necessary to hold and manage it : a ZIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 *
 *@note
 * 1. ZIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZIndexFile file pathname is not stored but is a computed data from actual ZMasterFile file pathname.
 *
 * @param[in] pKeyDic       Key fields dictionary to create the index with AND to add to Master dictionary
 * @param[in] pIndexName    User name of the index key as a utf8String. This name replaces ZSKeyDictionary::DicKeyName.
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
 * @param[in] pBackup   If set to true, then a backup copy of possible existing index files is made
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
    ZStatus zcreateIndexWithDefinition (ZKeyDictionary* pKeyDic,
                                        ZSort_Type pDuplicates,
                                        long &pOutKeyRank);

/**
 * @brief ZMasterFile::zcreateIndexFromDictionary Generates a new index from a key definition (meaning a new ZRandomFile data + header).
 *
 * This routine will search within Master dictionary for an Index key whose name corresponds to pDicKeyName.
 * Then it will create a new index from this definition with the files structures necessary to hold and manage it : a ZIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 *
 *@note
 * 1. ZIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZIndexFile file pathname is not stored but is a computed data from actual ZMasterFile file pathname.
 *
 * @param[in] pDicKeyName    User name of the index key as a utf8String.
 *                           This name is searched within ZSKeyDictionary::DicKeyName.
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
 * @param[out]pOutKeyRank   Gives back the index key rank for the created index. It is set to -1 in case of error(see below).
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
*   ZS_NULLPTR  Master dictionary has not been defined and is set to nullptr
*   ZS_ERROPEN  Master file has not been open in priviligiated mode while required for calling this routine
*   ZS_NOTFOUND given key name pDicKeyName has not been found within existing Master dictionary
*   ZS_DUPLICATEKEY given key name or definition already corresponds to an existing, effective key
*
 */
    ZStatus zcreateIndexFromDictionary (const utf8String &pDicKeyName,
                                        ZSort_Type pDuplicates,
                                        long &pOutKeyRank);


    ZStatus zopen       (const uriString pURI, const int pMode=ZRF_All); // superseeds ZRandomfile zopen
    ZStatus zopen       (const int pMode=ZRF_All) {return (zopen(getURIContent(),pMode));}


    ZStatus zadd      (ZRecord *pRecord );
    ZStatus _add(ZRecord* pRecord);

    ZStatus zinsert   (ZRecord *pRecord, const zrank_type pZMFRank);
    ZStatus zget      (ZRecord* pRecord, const zrank_type pZMFRank);

    ZStatus zaddRaw   (ZRawRecord &pRecord);
    ZStatus _insertRaw(ZRawRecord &pRecord, const zrank_type pZMFRank);
    ZStatus _getRaw   (ZRawRecord &pRecord, const zrank_type pZMFRank);

    ZStatus zremoveByRank     (const zrank_type pZMFRank) ;
    ZStatus zremoveByRankR    (ZRecord *pZMFRecord, const zrank_type pZMFRank);




//    ZStatus zclose (void);  // zclose is defined within ZRawMasterFile

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

    void _writeXML_ZRandomFileHeader(ZMasterFile& pZMF,FILE *pOutput);

    void _writeXML_Index(ZMasterFile& pZMF,const long pIndexRank,FILE* pOutput);

    void _writeXML_MetaDic(ZMetaDic* ZMDic,FILE* pOutput);

    void _writeXML_KDic(ZKeyDictionary* ZKDic, FILE* pOutput);

    ZStatus generateXML_IndexDefinition(const long pIndexRank, const char *pFilename);
    
    void XmlWriteFileDefinition(FILE *pOutput=nullptr) ;
    ZStatus generateXML_IndexDefinition(const long pIndexRank, FILE *pOutput=nullptr);
    static
    void XmlWriteFileDefinition(const char *pFilePath, FILE *pOutput=nullptr) ;


    static
    ZStatus _loadXMLKeyField(zxmlNode* &wNode, ZKeyDictionary*&pZKDic);
    static
    ZStatus _loadXMLDictionary(zxmlNode* &wNode, ZKeyDictionary *pZKDic);

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
                                ZArray<ZIndexControlBlock *> &pZICBList,
                                ZMetaDic *pMetaDic,
                                ZaiErrors* pErrorlog);
    static
    ZStatus zloadXML_Dictionary(const utf8_t* pFilePath, ZKeyDictionary &pZKDIC, ZMetaDic *pMetaDic);

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
                                zbs::ZArray<ZIndexControlBlock *> &pZICBList,
                                ZMetaDic *pMetaDic,
                                ZaiErrors *pErrorlog);
    static
    zxmlNode* _searchForChildTag(zxmlNode *pTopNode, const char *pTag);

    static
    ZStatus zgetXMLIndexRank(ZMasterFile &wMasterFile,
                             ZIndexControlBlock &wZICB,
                             zxmlNode* &pNode,
                             long &wMissingTags,
                             long &wMissMandatoryTags,
                             char &IndexPresence,
                             long &IndexRank,
                             FILE*wOutput);


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

// -------------------ZMasterFile operators-------------------------

/**
 * @brief operator << Adds a new record (ZDataBuffer) at the logical end of the ZMasterFile. Equivalent to push
 * @param pZMF
 * @param pInput
 * @return
 */
friend ZStatus operator << (ZMasterFile &pZMF,ZRecord& pInput)
     {
     return (pZMF.push(pInput));
     }
/**
 * @brief operator -- Remove the last logical record of the ZMasterFile. Equivalent to pop()
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

    ZStatus _remove_RollbackIndexes (ZArray<zrank_type> &pIndexRankProcessed);

    ZStatus _remove_HardRollbackIndexes (ZArray<ZSIndexItem*> &pIndexItemList,
                                         ZArray<zrank_type> &pIndexRankProcessed);
    ZStatus _remove_CommitIndexes ( ZSIndexItemList &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed);
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

  ZStatus _seek (ZMasterControlBlock &pMCB, long pIndexNumber, const ZDataBuffer &pKey,ZDataBuffer &pUserRecord);


  ZRecord* generateRecord();


}; //--------------------end class ZMasterFile-------------------------------


/** @}  */  // end ZMasterFileGroup group

// =====================end ZMasterFile===============================

} // namespace zbs

#endif // ZMasterFile_H
