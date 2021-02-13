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
#include <zindexedfile/zskey.h>
#include <ztoolset/zutfstrings.h>

#include <ztoolset/zaierrors.h>

/*#ifdef QT_CORE_LIB
//#include <zxml/qxmlutilities.h>
#endif // QT_CORE_LIB
*/
//------------------Generic Functions--------------------------


utf8String generateIndexRootName(utf8String &pMasterRootName, const long pRank, utf8String &pIndexName);

ZStatus generateIndexURI(uriString pMasterUri, uriString &pDirectory, uriString &pZIndexFileUri, const long pRank, utf8String &pIndexName);


namespace zbs //========================================================================
{



#include <zindexedfile/zmetadic.h>
/**
 * @brief The ZSMasterFile class Structured Master file or ZSMF
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

//-----------------Indexes ----------------------------------

class ZSIndexObjectTable :  private ZArray<ZSIndexFile*>
{
typedef ZArray<ZSIndexFile*> _Base ;
public:
    ZSIndexObjectTable() {}
    ~ZSIndexObjectTable() {}// just to call the base destructor
    using _Base::push;
    using _Base::size;
    using _Base::last;
    using _Base::lastIdx;
    using _Base::newBlankElement;
    using _Base::operator [];

    long pop (void);
    long erase (long pRank);
    void clear(void);
} ;

class ZSKeyDictionaryTable : private ZArray<ZSKeyDictionary>

{
    typedef ZArray<ZSKeyDictionary> _Base ;
public:
//    using _Base::push;  // push is overloaded
    using _Base::size;
    using _Base::last;
    using _Base::lastIdx;
    using _Base::newBlankElement;
    using _Base::operator [];
    //using ZIndexControlBlock::clear;

    long erase(const long pIdx) ;
    long push (ZSKeyDictionary &pICB);
    long pop (void);
    void clear (void) ;

}; // ZIndexControlTable
class ZSIndexControlTable : private ZArray<ZSIndexControlBlock*>

{
    typedef ZArray<ZSIndexControlBlock*> _Base ;
public:
    ZSIndexControlTable() {}
    ~ZSIndexControlTable() {}
//    using _Base::push;  // push is overloaded
    using _Base::newBlankElement;
    using _Base::size;
    using _Base::last;
    using _Base::lastIdx;
    using _Base::operator [];
    //using ZIndexControlBlock::clear;

    long erase(const long pIdx) ;
    long push (ZSIndexControlBlock *pICB);
    long pop (void);
    void clear (void) ;

    long zsearchIndexByName (const char* pName);

}; // ZIndexControlTable

#ifndef ZREMOTEMIRRORING
#define ZREMOTEMIRRORING
class ZRemoteMirroring
{
public:
    int8_t              Protocol;
    ZHAT                AddressType;
    char Host[256]="";
    char Port[25]="";
    char Authenticate[512]="";
    char SSLKey [128]="";
    char SSLVector[128]="";
};

#endif //ZREMOTEMIRRORING

/**
 * @brief The ZSJournalControlBlock class this object is part of file header and is present (not nullptr) when ZMasterFile has journaling option on.
 *
 */


struct ZSJCBOwnData{                         // will be the first block of data for ZJCB
    uint32_t                StartSign ;
    ZBlockID                BlockID;
    unsigned long           ZMFVersion;
    size_t                  JCBSize;
    uint8_t                 JournalingOn=false;         //!< Journaling is started (true) or not (false)
    uriString               JournalLocalDirectoryPath;  //!< Directory path for journal file. If empty, then directory path of main content file is taken
    uint8_t                 Keep=false;                 // uint8_t used as bool

    ZDataBuffer& _export(ZDataBuffer& pZDBExport);

    ZSJCBOwnData& _import(unsigned char* pZDBImport_Ptr);

};
class ZSJournal;
class ZRemoteMirroring;
class ZSJournalControlBlock : public ZSJCBOwnData
{
public:
    friend class ZSJournal;
    ZSJournalControlBlock (void) {clear();}
    ~ZSJournalControlBlock(void) ;

    void clear(void);

    ZDataBuffer& _exportJCB(ZDataBuffer &pJCBContent);
    size_t _getExportSize();
    ZStatus _importJCB (ZDataBuffer &pJCBContent);

    void report(FILE *pOutput=stdout);

    void setParameters (uriString &pJournalPath);
    void setRemoteMirroring (int8_t pProtocol,
                             char* pHost,
                             char* pPort,
                             char* pAuthenticate,
                             char* pSSLKey,
                             char* pSSLVector);

    ZStatus purge(const zrank_type pKeepRanks=-1);
    ZSJournal            * Journal=nullptr;
    ZRemoteMirroring    *Remote=nullptr;
} ;
#pragma pack(push)
#pragma pack(0)
struct ZSMCBOwnData_Export{                         // will be the first block of data for ZSMCB
    uint32_t                StartSign ;
    ZBlockID                BlockID;
    unsigned long           ZMFVersion;
    size_t                  MCBSize;
    size_t                  MDicOffset;
    size_t                  MDicSize;
    size_t                  ICBOffset;
    size_t                  ICBSize;
    ssize_t                 JCBOffset;
    ssize_t                 JCBSize;
    size_t                  IndexCount;

    uint8_t                 HistoryOn=false;

// ===========IndexFilePath is exported as Universal value using uriString::_exportUniversal()=======
//                     leading uint16_t size then string content WITHOUT '\0' ending char
};
#pragma pack(pop)


struct ZSMCBOwnData{                         // will be the first block of data for ZSMCB
//    uint32_t                StartSign=cst_ZSTART ;
//    ZBlockID                BlockID;
//    unsigned long           ZMFVersion;
    size_t                  MCBSize;      // contains size of exported - imported  MCB
    //                                includes varying size of index path (varying string)
    size_t                  MDicOffset;
    size_t                  MDicSize;
    size_t                  ICBOffset;
    size_t                  ICBSize;
    ssize_t                 JCBOffset;
    ssize_t                 JCBSize;
    size_t                  IndexCount;

    uint8_t                 HistoryOn=false;
//    uint8_t                    JournalingOn=false; //  will define wether update or load ZSJournalControlBlock from header while updating/reading ZMasterControlBlock
    uriString               IndexFilePath;        // Directory path for index files. If empty, then directory path of main content file is taken
//    uriString               JournalPath;        // see ZJCB - Directory path for journal file. If empty, then directory path of main content file is taken
//    uint32_t                EndSign=cst_ZEND ;
    ZDataBuffer& _export(ZDataBuffer& pZDBExport);
    ZStatus _import(unsigned char* pZDBImport_Ptr);

};

//-----------------Master control block-------------------------------------

/**
 * @brief The ZSMasterFileControlBlock class Master File Control Block contains all operational infradata necessary for a master file to operate.
 *
 *  ZMCB is stored within reserved block in Master File header.
 *
 *  As it is a permanent infradata, it is stored BEFORE ZFCB structure.
 *
 */
class ZSMasterControlBlock : public ZSMCBOwnData
{
public:
    friend class ZSJournal;
//  Index list
    ZMetaDic                        MetaDic;        // Mandatory Meta Dictionary
//    checkSum*                       MDicCheckSum=nullptr;
    ZSIndexControlTable             Index;          // List of Indexes definition content
    ZSIndexObjectTable              IndexObjects;   // List of effective index objects that will manage physical ZSIndexFile
    ZSJournalControlBlock*          ZJCB=nullptr;  // journaling is defined here. If no journaling, stays to nullptr

    ZSMasterControlBlock (void) {clear();}
    ~ZSMasterControlBlock(void) ;

    void pushICBtoIndex(ZSIndexControlBlock *pICB);
    void removeICB(const long pRank);

    long popICB(void);

    void clear(void);


    ZDataBuffer& _exportMCB (ZDataBuffer &pMCBContent);
    ZStatus     _importMCB  (ZDataBuffer& pBuffer);

    void report(FILE *pOutput=stdout);
} ;


//=================================ZSMasterFile===============================
//
//class ZSKey;

/**
 * @brief The ZSMasterFile class This is the master object derived from ZRandomFile that allows to extend it to index management
 * @see @ref ZSMasterFileGroup
 *
 */
class ZSMasterFile : protected ZRandomFile  // cannot publicly access to ZRandomFile data & methods
{

public:
    friend class ZSKey;
    friend class ZSIndexCollection;
    friend class ZSJournal;
typedef ZRandomFile _Base ;

    ZSMasterFile(void) ;
    ZSMasterFile(uriString pURI);
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
    ZStatus  setParameters (const bool pGrabFreeSpace,
                            const bool pHighwaterMarking,
                            const ssize_t pBlockTargetSize)
    { return _Base::_setParameters(ZDescriptor,
                                   ZFT_ZSMasterFile,
                                   pGrabFreeSpace,
                                   pHighwaterMarking,
                                   pBlockTargetSize);}

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
//RFFU                 bool pAutoRebuild,
                       ZSort_Type pDuplicates);

    ZStatus zprintIndexFieldValues (const zrank_type pIndex, const zrank_type pIdxRank, bool pHeader=true, bool pKeyDump=false,FILE *pOutput=stdout);
    /**
     * @brief zgetIndexSize returns the size (number of records) of the index number pIndex
     * @param pIndex Index number within ZSMasterFile to return the size for
     * @return
     */
    ssize_t    zgetIndexSize(const zrank_type pIndexRank) {return ZMCB.IndexObjects[pIndexRank]->getSize();}
    ZSKeyDictionary * zgetKeyDictionary(const zrank_type pIndexRank) {return ZMCB.Index[pIndexRank]->ZKDic ;}


    ZSIndexFile* zgetIndexObject(const zrank_type pIndexRank) {return ZMCB.IndexObjects[pIndexRank];}

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

    ZStatus zcreateIndex (ZSKeyDictionary& pZIFField,
                          utf8String &pIndexName,
// RFFU                   bool pAutorebuild,
                          ZSort_Type pDuplicates,
                          bool pBackup);

// overload of previous function

    ZStatus zcreateIndex (ZSKeyDictionary &pZIFField,
                          const utf8_t *pIndexName,
// RFFU                   bool pAutorebuild,
                          ZSort_Type pDuplicates,
                          bool pBackup=true);


    ZStatus zopen       (const uriString pURI, const int pMode=ZRF_All); // superseeds ZRandomfile zopen
    ZStatus zopen       (const int pMode=ZRF_All) {return (zopen(getURIContent(),pMode));}
//    ZStatus zopen       (const int pMode=ZRF_All) {return zopen(ZDescriptor.URIContent,pMode);}

    ZStatus zadd         (ZRecord *pRecord );
    ZStatus zinsert     (ZRecord *pRecord, const zrank_type pZMFRank);

    ZStatus zget        (ZRecord* pRecord, const zrank_type pZMFRank);

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

    void _writeXML_KDic(ZSKeyDictionary* ZKDic,FILE* pOutput);

    ZStatus zwriteXML_IndexDefinition(const long pIndexRank, FILE *pOutput=nullptr);
    void zwriteXML_FileHeader(FILE *pOutput=nullptr) ;
    ZStatus zwriteXML_IndexDictionary(const long pIndexRank, FILE *pOutput=nullptr);
    static
    void zwriteXML_FileHeader(const char *pFilePath, FILE *pOutput=nullptr) ;

//#ifdef QT_CORE_LIB
    static
    ZStatus _loadXMLKeyField(zxmlNode* &wNode, ZSKeyDictionary*&pZKDic);
    static
    ZStatus _loadXMLDictionary(zxmlNode* &wNode, ZSKeyDictionary *pZKDic);

    static
    ZStatus zapplyXMLFileDefinition(const utf8_t *pXMLPath, const utf8_t *pContentFilePath=nullptr, bool pRealRun=false, FILE *pOutput=nullptr);
    static
    ZStatus zapplyXMLFileDefinition_old(const utf8_t *pXMLPath, const utf8_t*pContentFilePath=nullptr, bool pRealRun=false, FILE *pOutput=nullptr);
    static
    ZStatus zloadXML_Index(const utf8_t* pFilePath, ZSIndexControlBlock &wZICB, ZMetaDic *pMetaDic);
    static
    ZStatus zloadXML_Dictionary(const utf8_t* pFilePath, ZSKeyDictionary &pZKDIC, ZMetaDic *pMetaDic);
    static
    ZStatus _XMLzicmControl(const utf8_t *pFilePath, zxmlDoc* &XmlDoc, zxmlNode* &pFirstNode);
    static
    ZStatus
    _XMLLoadAndControl(const utf8_t* pFilePath,
                       zxmlDoc* &XmlDoc,
                       zxmlNode* &wRootNode,
                       const utf8_t* pRootName,
                       const utf8_t* pRootAttrName,
                       const utf8_t* pRootAttrValue,
                       FILE* pOutput=nullptr);
    static
    ZStatus _loadXML_Index(zxmlNode* &pIndexNode, ZSIndexControlBlock *pZICB, ZMetaDic *pMetaDic);
    static
    ZStatus zgetXMLIndexRank(ZSMasterFile &wMasterFile,
                             ZSIndexControlBlock &wZICB,
                             zxmlNode* &pNode,
                             long &wMissingTags,
                             long &wMissMandatoryTags,
                             char &IndexPresence,
                             long &IndexRank,
                             FILE*wOutput);
    static
    ZStatus zapplyXMLIndexRankDefinition(const char* pXMLPath,
                                              const char *pContentFilePath,
                                              bool pRealRun,
                                              FILE*pOutput);

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
  * @brief operator [] Returns a reference to a ZDataBuffer containing the record pointed by pIndex rank in ZBlockAccessTable pool,
  *                    while base ZArray operator  [] returns a reference to the rank.
  *
  *                    As it uses ZRandomFile::CurrentRecord data structure this is NOT usable within a multi thread context.
  *
  *                    ZAM rank cannot be modified (because of potential Keys modification): use replace() method instead.
  * @warning not usable in a multi threading context.
  *
  * @param pIndex
  * @return a Reference to a ZDataBuffer (Field CurrentRecord)
  */
     ZDataBuffer&  operator [] (const long pIndex) { zget(CurrentRecord,pIndex);return CurrentRecord;}   // resulting _Type cannot be modified : use replace() method instead


//friend ZDataBuffer& operator << (ZDataBuffer &pRes,ZSMasterFile &pZMF);

//friend ZStatus operator << (ZSMasterFile &pZMF,ZDataBuffer& pInput);
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
    static inline
    ZStatus _add_RollbackIndexes (ZSMasterControlBlock &pZMCB, ZArray<zrank_type> &pIndexRankProcessed);
    static inline
    ZStatus _add_HardRollbackIndexes (ZSMasterControlBlock &pZMCB, ZArray<zrank_type> &pIndexRankProcessed);
    static inline
    ZStatus _add_CommitIndexes (ZSMasterControlBlock& pZMCB, ZArray <ZSIndexItem*>  &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed);

//-----------End Add sequence------------------------------------

// ----------Remove sequence-------------------------------------

    inline
//    ZStatus _removeByRank  (ZFileDescriptor &pDescriptor, ZMasterControlBlock &pZMCB, ZDataBuffer &pZMFRecord, const long pZMFRank);
    ZStatus _removeByRank  (ZRecord *pZMFRecord, const zrank_type pZMFRank);
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


//-------------Threads Lock management---------------------------

#ifdef __USE_ZTHREAD__
void ZMFlock(void)
{
/*        if (ZJournaling->ZFJournaling)
                    {
                    ZJournaling->_Mtx.lock();
                    }
       if (ZHistory!=nullptr)
                    ZHistory->_Mtx.lock();*/
        _Mtx.lock();

        return;
}

void ZMFunlock(void)
{
    _Mtx.unlock();
    return;
}
#endif // __USE_ZTHREAD__


//======================Journaling=======================================


bool getJournalingStatus (void)
{
    return(ZMCB.ZJCB!=nullptr);
}

ZStatus journalingSetup (uriString &pJournalPath);

ZStatus setJournalingOn (void);

ZStatus initJournaling (uriString &pJournalPath);

ZStatus setJournalingOff (void);

ZRecord * generateZRecord(void)
{
    return new ZRecord (&ZMCB.MetaDic);
}

// =======================Data fields======================================


private:
    using _Base::operator = ;

#ifdef __USE_ZTHREAD__
    ZMutex    _Mtx;
#endif


protected:

    /* for below see into ZMasterControlBlock  */
//    ZSJournalControlBlock* ZJCB=nullptr; //!< Journaling data : if exists, is part of ZSMasterFile header

//    uriString           ZMFURI; //!< uriString with the current full path of ZSMasterFile. redundant with ZRandomFile URIContent. Removed

//    ZJournal*           Journal=nullptr;  // see ZJCB
//    bool                FJournal=false;   // see ZJCB::JournalingOn
//    uriString           JournalPath;      // see ZJCB::JournalPath

public:

// permanent public data
//
    ZSMasterControlBlock ZMCB;      //< Master Control Block : all data is there
    zstatistics          ZPMSStats; //< Statistical data

    ZaiErrors           ErrorLog;
}; //--------------------end class ZSMasterFile-------------------------------


/** @}  */  // end ZSMasterFileGroup group

// =====================end ZSMasterFile===============================

} // namespace zbs
#endif // ZSMASTERFILE_H
