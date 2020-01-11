#ifndef ZMASTERFILE_H
#define ZMASTERFILE_H

#include <zindexedfile/zmfconfig.h>
#include <ztoolset/zerror.h>

#include <cstdarg>

#include <zindexedfile/zmf_limits.h>

#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zindexfile.h>
#include <zindexedfile/zkey.h>
#include <znet/zbasenet/znetcommon.h>

#ifdef QT_CORE_LIB
#include <zxml/qxmlutilities.h>
#endif // QT_CORE_LIB

//------------------Generic Functions--------------------------


utfdescString generateIndexRootName(utfdescString &pMasterRootName, const long pRank, utffieldNameString &pIndexName);

ZStatus generateIndexURI(uriString pMasterUri, uriString &pDirectory, uriString &pZIndexFileUri, const long pRank, utffieldNameString &pIndexName);


namespace zbs //========================================================================
{


/**
  @addtogroup ZMasterFileGroup
  @{
  */


class ZIndexCollection;
class ZJournal;
//--------------------Journaling ---------------------------------------------------



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
 * @brief The ZJournalControlBlock class this object is part of file header and is present (not nullptr) when ZMasterFile has journaling option on.
 *
 */


struct ZJCBOwnData{                         // will be the first block of data for ZJCB
    int32_t                 StartSign ;
    ZBlockID                BlockID;
    long                    ZMFVersion;
    ssize_t                 JCBSize;
    bool                    JournalingOn=false;         //!< Journaling is started (true) or not (false)
    uriString               JournalLocalDirectoryPath;  //!< Directory path for journal file. If empty, then directory path of main content file is taken
    long                    Keep=-1;
};
class ZJournalControlBlock : public ZJCBOwnData
{
public:
    friend class ZJournal;
    ZJournalControlBlock (void) {clear();}
    ~ZJournalControlBlock(void) ;

    void clear(void);

    ZDataBuffer& _exportJCB(ZDataBuffer &pJCBContent);
    ssize_t _getExportSize();
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
    ZJournal            * Journal=nullptr;
    ZRemoteMirroring    *Remote=nullptr;
} ;


//-----------------Indexes ----------------------------------

class ZIndexObjectTable :  private ZArray<ZIndexFile*>
{
typedef ZArray<ZIndexFile*> _Base ;
public:
    ZIndexObjectTable() {}
    ~ZIndexObjectTable() {}// just to call the base destructor
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

class ZKeyDictionaryTable : private ZArray<ZKeyDictionary>

{
    typedef ZArray<ZKeyDictionary> _Base ;
public:
//    using _Base::push;  // push is overloaded
    using _Base::size;
    using _Base::last;
    using _Base::lastIdx;
    using _Base::newBlankElement;
    using _Base::operator [];
    //using ZIndexControlBlock::clear;

    long erase(const long pIdx) ;
    long push (ZKeyDictionary &pICB);
    long pop (void);
    void clear (void) ;

}; // ZIndexControlTable
class ZIndexControlTable : private ZArray<ZIndexControlBlock>

{
    typedef ZArray<ZIndexControlBlock> _Base ;
public:
    ZIndexControlTable() {}
    ~ZIndexControlTable() {}
//    using _Base::push;  // push is overloaded
    using _Base::newBlankElement;
    using _Base::size;
    using _Base::last;
    using _Base::lastIdx;
    using _Base::operator [];
    //using ZIndexControlBlock::clear;

    long erase(const long pIdx) ;
    long push (ZIndexControlBlock &pICB);
    long pop (void);
    void clear (void) ;

    long zsearchIndexByName (const char* pName);
    long zsearchIndexByName (utfdescString pName);

}; // ZIndexControlTable

struct ZMCBOwnData{                         // will be the first block of data for ZMCB
    int32_t                 StartSign ;
    ZBlockID                BlockID;
    long                    ZMFVersion;
    ssize_t                 MCBSize;
    ssize_t                 JCBOffset;
    ssize_t                 JCBSize;
    long                    IndexCount;

    bool                    HistoryOn=false;
//    bool                    JournalingOn=false;//!<  will define wether update or load ZJournalControlBlock from header while updating/reading ZMasterControlBlock
    uriString               IndexFilePath;  //!< Directory path for index files. If empty, then directory path of main content file is taken
//    uriString               JournalPath;    //!< see ZJCB - Directory path for journal file. If empty, then directory path of main content file is taken
};

//-----------------Master control block-------------------------------------

/**
 * @brief The ZMasterFileControlBlock class Master File Control Block contains all operational infradata necessary for a master file to operate.
 *
 *  ZMCB is stored within reserved block in Master File header.
 *
 *  As it is a permanent infradata, it is stored BEFORE ZFCB structure.
 *
 */

class ZMasterControlBlock : public ZMCBOwnData
{
public:
    friend class ZJournal;
//  Index list

    ZIndexControlTable              Index;
    ZIndexObjectTable               IndexObjects;
    ZJournalControlBlock*           ZJCB=nullptr;  //! journaling is defined here. If no journaling, stays to nullptr

    ZMasterControlBlock (void) {clear();}
    ~ZMasterControlBlock(void) ;

    void pushICBtoIndex(ZIndexControlBlock&pICB);
    void removeICB(const long pRank);

    long popICB(void);

    void clear(void);


    ZDataBuffer& _exportMCB(ZDataBuffer &pMCBContent);
    ZStatus _importMCB (ZDataBuffer& pBuffer);

    void report(FILE *pOutput=stdout);
} ;


//=================================ZMasterFile===============================
//


/**
 * @brief The ZMasterFile class This is the master object derived from ZRandomFile that allows to extend it to index management
 * @see @ref ZMasterFileGroup
 *
 */
class ZMasterFile : protected ZRandomFile  // cannot publicly access to ZRandomFile data & methods
{
    friend class ZKey;
    friend class ZIndexCollection;
    friend class ZJournal;
typedef ZRandomFile _Base ;
public:

    ZMasterFile(void) ;
    ZMasterFile(uriString pURI);
    ZMasterFile(bool pHistory) ;
    ~ZMasterFile(void);

    ZKey* createZKeyByName (const char* pKeyName);
    ZKey* createZKey (const long pKeyNumber);

// what we can externally use of ZMasterFile methods (included from ZIndexFile class)

//    using _Base::getURIContent;  // defined as virtual
//    using _Base::getURIHeader;

//    using _Base::putTheMess ;

//    using _Base::size ;
//    using _Base::getAllocatedBlocks;
//    using _Base::getAllocatedSize;
//    using _Base::getInitialSize;
//    using _Base::getBlockExtentQuota;
//    using _Base::getGrabFreeSpace;
//    using _Base::getHighwaterMarking;
//    using _Base::getBlockTargetSize;

 /*   using _Base::setGrabFreeSpace;
    using _Base::setHighwaterMarking;
    using _Base::setBlockTargetSize;
*/
    using _Base::zgetCurrentRank;
    using _Base::zgetCurrentLogicalPosition ;
    using _Base::setLogicalFromPhysical ;
    using _Base::zremove;

    using _Base::zget;
    using _Base::zgetByAddress;

    using _Base::zgetBlockDescriptor;

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
                                   ZFT_ZMasterFile,
                                   pGrabFreeSpace,
                                   pHighwaterMarking,
                                   pBlockTargetSize);}

    ZStatus setIndexFilesDirectoryPath (uriString &pPath);
    ZStatus setJournalLocalDirectoryPath (uriString &pPath);

//    ZStatus setPath(uriString &pPath) { return _Base::setPath(pPath); }  // defined as virtual in ZRandomFile base class
//----------------End setting parameters--------------------
// ZMasterFile creation

    void _defineKeyFieldsList (ZIndexControlBlock pZICB, long pCount,...);

    void _addIndexField(ZArray<ZIndexField> &pZIFField,
                        utfdescString &pName,
                        size_t pOffset,
                        size_t pLength);
    void _addIndexField(ZMasterControlBlock &pMCB, utfcodeString &pName, size_t pOffset, size_t pLength);

    void _addIndexKeyDefinition (ZIndexControlBlock& pZICB,
                       ZKeyDictionary &pZIFField,
                       utfdescString pIndexName,
//RFFU                 bool pAutoRebuild,
                       ZSort_Type pDuplicates);

    ZStatus zprintIndexFieldValues (const long pIndex, const long pIdxRank, bool pHeader=true, bool pKeyDump=false,FILE *pOutput=stdout);
    /**
     * @brief zgetIndexSize returns the size (number of records) of the index number pIndex
     * @param pIndex Index number within ZMasterFile to return the size for
     * @return
     */
    long    zgetIndexSize(const long pIndexRank) {return ZMCB.IndexObjects[pIndexRank]->getSize();}
    ZIndexFile* zgetIndexObject(const long pIndexRank) {return ZMCB.IndexObjects[pIndexRank];}

    ZStatus zcreate(const uriString pURI,
                    long pAllocatedBlocks,
                    long pBlockExtentQuota,
                    long pBlockTargetSize,
                    const zsize_type pInitialSize,
                    bool pHighwaterMarking,
                    bool pGrabFreeSpace,
                    bool pJournaling,
                    bool pBackup=false,
                    bool pLeaveOpen=false);

    ZStatus zcreate (const uriString pURI, const zsize_type pInitialSize, bool pBackup=false, bool pLeaveOpen=false);

    ZStatus zcreate (const char* pPathName, const zsize_type pInitialSize, bool pBackup=false, bool pLeaveOpen=false);

    ZStatus zcreateIndex (ZKeyDictionary& pZIFField,
                          utfdescString &pIndexName,
// RFFU                   bool pAutorebuild,
                          ZSort_Type pDuplicates,
                          bool pBackup);

// overload of previous function

    ZStatus zcreateIndex (ZKeyDictionary &pZIFField,
                          const char* pIndexName,
// RFFU                   bool pAutorebuild,
                          ZSort_Type pDuplicates,
                          bool pBackup=true);


    ZStatus zopen       (const uriString pURI, const int pMode=ZRF_All); // superseeds ZRandomfile zopen
    ZStatus zopen       (const int pMode=ZRF_All) {return (zopen(getURIContent(),pMode));}
//    ZStatus zopen       (const int pMode=ZRF_All) {return zopen(ZDescriptor.URIContent,pMode);}

    ZStatus zadd         (ZDataBuffer& pRecord );
    ZStatus zinsert     (ZDataBuffer& pRecord, const long pZMFRank);

    ZStatus zremoveByRank     (const long pZMFRank) ;
    ZStatus zremoveByRankR    (ZDataBuffer &pZMFRecord,const long pZMFRank);


    ZStatus writeControlBlocks(void);
    ZStatus readControlBlocks(void);
    ZStatus readJCBfromHeader(void);
    ZStatus _getJCBfromReserved(void);

    ZStatus zclose (void);

//---------search and get operations--------------------------------
//

//-------relative get  (use ZRandomFile base routines)------------------------
//

    ZStatus zgenerateKeyValueList (ZDataBuffer &pKey , long pKeyNumber, int pCount,...);
    ZStatus zgenerateKeyValue (ZIndexControlBlock& pICB, ZArray<void *> &pKeyValues, ZDataBuffer& pKey);

    ZStatus zsearch (ZDataBuffer &pRecord, ZDataBuffer &pKeyValue, const long pIndexNumber=0);
    ZStatus zsearch (ZDataBuffer &pRecord,ZKey *pKey);

    ZStatus zsearchAll (ZDataBuffer &pKeyValue,
                        const long pIndexNumber,
                        ZIndexCollection &pIndexCollection,
                        const ZMatchSize_type pZMS=ZMS_MatchKeySize);

    ZStatus zsearchAll (ZKey &pZKey, ZIndexCollection& pIndexCollection);

    ZStatus zsearchFirst (ZKey &pZKey, ZDataBuffer &pRecord, ZIndexCollection *pCollection);
    ZStatus zsearchNext (ZKey &pZKey, ZDataBuffer &pRecord, ZIndexCollection *pCollection);


    using ZRandomFile::zsearchFieldAllCollection;
    using ZRandomFile::zsearchFieldFirstCollection;
    using ZRandomFile::zsearchFieldNextCollection;


    ZStatus zsearchInterval (ZKey &pZKeyLow, ZKey &pZKeyHigh,const zrank_type pIndexNumber,ZIndexCollectionContext *pSearchContext );


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

    void _writeXML_KDic(ZKeyDictionary* ZKDic,FILE* pOutput);

    ZStatus zwriteXML_IndexDefinition(const long pIndexRank, FILE *pOutput=nullptr);
    void zwriteXML_FileHeader(FILE *pOutput=nullptr) ;
    ZStatus zwriteXML_IndexDictionary(const long pIndexRank, FILE *pOutput=nullptr);
    static
    void zwriteXML_FileHeader(const char *pFilePath, FILE *pOutput=nullptr) ;

#ifdef QT_CORE_LIB
    static
    ZStatus _loadXMLKeyField(QDomNode &wNode, ZKeyDictionary*&pZKDic);
    static
    ZStatus _loadXMLDictionary(QDomNode &wNode, ZKeyDictionary *pZKDic);

    static
    ZStatus zapplyXMLFileDefinition(const char* pXMLPath, const char*pContentFilePath=nullptr, bool pRealRun=false, FILE *pOutput=nullptr);
    static
    ZStatus zapplyXMLFileDefinition_old(const char* pXMLPath, const char*pContentFilePath=nullptr, bool pRealRun=false, FILE *pOutput=nullptr);
    static
    ZStatus zloadXML_Index(const char* pFilePath, ZIndexControlBlock &wZICB);
    static
    ZStatus zloadXML_Dictionary(const char* pFilePath, ZKeyDictionary &pZKDIC);
    static
    ZStatus _XMLzicmControl(const char* pFilePath,QDomDocument &XmlDoc,QDomNode &pFirstNode);

    static
    ZStatus _loadXML_Index(QDomNode &pIndexNode, ZIndexControlBlock *pZICB);
    static
    ZStatus zgetXMLIndexRank(ZMasterFile &wMasterFile,
                             ZIndexControlBlock &wZICB,
                             QDomNode pNode,
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

#endif

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

    ZStatus getRawIndex(ZIndexItem &pIndexItem, const long pIndexRank, const long pIndexNumber);

    ZStatus getKeyIndexFields(ZDataBuffer &pKeyFieldValues, const long pIndexRank,const long pIndexNumber);

// -------------------ZMasterFile operators-------------------------

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


//friend ZDataBuffer& operator << (ZDataBuffer &pRes,ZMasterFile &pZMF);

//friend ZStatus operator << (ZMasterFile &pZMF,ZDataBuffer& pInput);
/**
 * @brief operator << Adds a new record (ZDataBuffer) at the logical end of the ZMasterFile. Equivalent to push
 * @param pZMF
 * @param pInput
 * @return
 */
friend ZStatus operator << (ZMasterFile &pZMF,ZDataBuffer& pInput)
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
    static inline
    ZStatus _add_RollbackIndexes (ZMasterControlBlock &pZMCB, ZArray<long> &pIndexRankProcessed);
    static inline
    ZStatus _add_HardRollbackIndexes (ZMasterControlBlock &pZMCB, ZArray<long> &pIndexRankProcessed);
    static inline
    ZStatus _add_CommitIndexes (ZMasterControlBlock& pZMCB, ZArray <ZIndexItem*>  &pIndexItemList, ZArray<long> &pIndexRankProcessed);

//-----------End Add sequence------------------------------------

// ----------Remove sequence-------------------------------------

    inline
//    ZStatus _removeByRank  (ZFileDescriptor &pDescriptor, ZMasterControlBlock &pZMCB, ZDataBuffer &pZMFRecord, const long pZMFRank);
    ZStatus _removeByRank  (ZDataBuffer &pZMFRecord, const long pZMFRank);
    static inline
    ZStatus _remove_RollbackIndexes (ZMasterControlBlock &pZMCB, ZArray<long> &pIndexRankProcessed);
    static inline
    ZStatus _remove_HardRollbackIndexes (ZMasterControlBlock& pZMCB, ZArray<ZIndexItem*> &pIndexItemList,ZArray<long> &pIndexRankProcessed);
    static inline
    ZStatus _remove_CommitIndexes (ZMasterControlBlock& pZMCB, ZIndexItemList &pIndexItemList, ZArray<long> &pIndexRankProcessed);
/** @endcond */
//-----------End Remove sequence------------------------------------

//
//-------------------base methods overload-----------------
//
    ZStatus push(ZDataBuffer &pElement) ;
    ZStatus push_front (ZDataBuffer &pElement) {return(zinsert (pElement,0L));}

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
  /*  if (ZJournaling->ZFJournaling)
                {
                ZJournaling->_Mtx.unlock();
                }
   if (ZHistory!=nullptr)
                ZHistory->_Mtx.unlock();*/
    _Mtx.unlock();
    return;
}
#endif
//------------- Journaling management------------------------------

#ifdef __COMMENT__

//------------- History management------------------------------
void setHistoryOn (size_t pAlloc=_cst_zmf_history_allocation,size_t pReallocQuota=_cst_zmf_history_reallocquota)
{
#ifdef __USE_ZTHREAD__
    _Mtx.lock();
#endif
 /*   if (ZMCB.ZHistory==nullptr)
                {
                ZHistory=new ZCHistory(pAlloc,pReallocQuota);
                }*/
     ZMCB.HistoryOn=true;
//     ZHistoryZIXOn=true;
 /*    if (ZJournaling->_getOption()==ZJDisable)
                                  {
                                 ZJournaling->setJournaling(ZJAutoCommit );
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
                                 fprintf(stderr,"ZAM::setJournaling-W-JRNLSETAUTO Setting history on : journaling is set to ZJAutocommit");
#endif
                                  }
*/

#ifdef __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    return;
}


void setHistoryOff (void)
{
#ifdef __USE_ZTHREAD__
    ZMFlock();
#endif
     ZMCB.HistoryOn=false;
#ifdef __USE_ZTHREAD__
    ZMFunlock();
#endif
    return;
}

bool getHistoryStatus (void)
{
    return(ZMCB.HistoryOn);
}
#endif //__COMMENT__
//======================Journaling=======================================

bool getJournalingStatus (void)
{
    return(ZMCB.ZJCB!=nullptr);
}

ZStatus journalingSetup (uriString &pJournalPath);

ZStatus setJournalingOn (void);

ZStatus initJournaling (uriString &pJournalPath);

ZStatus setJournalingOff (void);

// =======================Data fields======================================


private:
    using _Base::operator = ;

#ifdef __USE_ZTHREAD__
    ZMutex    _Mtx;
#endif
protected:
// permanent protected data
//
    ZMasterControlBlock ZMCB;           //!< Master Control Block : all data is there


    /* for below see into ZMasterControlBlock  */
//    ZJournalControlBlock* ZJCB=nullptr; //!< Journaling data : if exists, is part of ZMasterFile header

//    uriString           ZMFURI; //!< uriString with the current full path of ZMasterFile. redundant with ZRandomFile URIContent. Removed

//    ZJournal*           Journal=nullptr;  // see ZJCB
//    bool                FJournal=false;   // see ZJCB::JournalingOn
//    uriString           JournalPath;      // see ZJCB::JournalPath

public:
    zstatistics         ZPMSStats; //!< Statistical data

}; //--------------------end class ZMasterFile-------------------------------



/*ZDataBuffer& operator << (ZDataBuffer &pRes,ZMasterFile &pZMF)
     {
     pZMF.zremoveR(pRes,pZMF.lastIdx());
     return(pRes);
     }*/



/** @}  */  // end ZMasterFileGroup group

// =====================end ZMasterFile===============================

} // namespace zbs





#endif // ZMASTERFILE_H
