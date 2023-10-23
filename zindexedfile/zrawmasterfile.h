#ifndef ZRAWMASTERFILE_H
#define ZRAWMASTERFILE_H
#include <zindexedfile/zmfconfig.h>
#include <cstdarg>
#include <zindexedfile/zmf_limits.h>
#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zrawindexfile.h>

#include <znet/zbasenet/znetcommon.h>

#include <ztoolset/zutfstrings.h>

#include <ztoolset/zaierrors.h>
#include <zxml/zxml.h>

#include <zindexedfile/zmastercontrolblock.h>
#include <zindexedfile/zindexcontrolblock.h>

#include <zindexedfile/bckelement.h>

//#include <zindexedfile/zindexitem.h>

/** @brief RawDataDescription Raw data storage detailed description
 * Raw data :
 * if first uint_32_t is ZType_bitsetFull, then no bitset, all fields are reputed to be present,
 * no dictionary is available (pure ZRawMasterRecord)
 * else
 * a bitset is present and must be read.
 *
 *
 * @verbatim
                      Raw Record bulk structure on file :

RawMasterFile                   Master File
    (no dictionary no presence)       with master dictionary


    uint32_t        ZType_bitsetFull               ZType_bitset
                    All fields present [...]    Zbitset content
                               \                   /
                                \                 /
    uint64_t                  record content size

    ...                       RECORD EFFECTIVE CONTENT
    URF format

    uint32_t                    Number of key contents

    uint32_t                      Key 0 size
    ...                           KEY 0 CONTENT

    uint32_t                      Key 1 size
    ...                           KEY 1 CONTENT

    ....

    uint32_t                      Key n size
    ...                           KEY n CONTENT


    uint32_t                      cst_ZEND  : end of record marker


*/

/* define functor for key extraction */

typedef ZStatus (*extractRawKey_type) (ZDataBuffer &pRawRecord,ZRawIndexFile* pZIF,ZDataBuffer* pKeyContent);

/*
 * Behavior mode for index key search
 */
enum ZIXMode : uint8_t {
  ZIXM_Nothing      = 0,
  ZIXM_SearchDycho  = 1,  /* use dychotomic search engine for key searches. By default, search is sequential. */
  ZIXM_Debug        = 2,  /* allows an index file not to have a father master file (only for debug purpose) */
  ZIXM_UpdateHeader = 4   /* systematically update header files after each operation */
};



namespace zbs //========================================================================
{

class ZKey;
class ZIndexItem;
class ZIndexItemList;

class ZRawMasterFile: protected ZRandomFile, public ZMasterControlBlock
{
  friend class ZIndexCollection;
  friend void zupgradeZRFtoZMF (const uriString& pZRFPath,FILE* pOutput);
  friend void zdowngradeZMFtoZRF (const uriString &pZMFPath, FILE* pOutput);

  friend class ZMasterFile;

  typedef ZRandomFile _Base ;
protected:  ZRawMasterFile(ZFile_type pType);
public:
  ZRawMasterFile();
//  ZRawMasterFile(uriString pURI);
  ~ZRawMasterFile(void);

/*  friend ::ZStatus applyXmltoFile(const char* pXMLPath, const char *pContentFilePath,bool pRealRun,const char* pLogfile);
  friend ::ZStatus createMasterFileFromXml(const char* pXMLPath,const char *pContentFilePath,bool pRealRun,bool pReplace,const char* pLogfile);
*/
  using ZRandomFile::getFileType;
  using ZRandomFile::getMode;
  using ZRandomFile::setPath;
  using ZRandomFile::getFCB;

  using ZRandomFile::getSize;
  using ZRandomFile::isEmpty;
  using ZRandomFile::getURIContent;
  using ZRandomFile::getURIHeader;
  using ZRandomFile::zgetByAddress;
  using ZRandomFile::zgetWAddress;
  using ZRandomFile::zgetNextWAddress;

  using ZRandomFile::zgetFirst;

  
  using ZRandomFile::isOpen;
  using ZRandomFile::getOpenMode;

  using ZRandomFile::getBlockTargetSize;
  using ZRandomFile::getAllocatedBlocks;
  using ZRandomFile::getBlockExtentQuota;
  using ZRandomFile::getHighwaterMarking;
  using ZRandomFile::getInitialSize;
  using ZRandomFile::getFileSize;


  using ZRandomFile::_reorgFileInternals;

  using ZRandomFile::getRecordCount;

//  ZFileControlBlock& getZFCB() {return ZFCB;}

  void setTypeRaw() { ZRandomFile::setFileType(ZFT_ZRawMasterFile); }
  void setTypeMasterFile() { ZRandomFile::setFileType(ZFT_ZMasterFile); }

  void setEngineMode(uint8_t pMode) {EngineMode=pMode;}

  /**
   * @brief createDictionary  generates an embedded dictionary file and stores pDic (ZMFDictionary) into it.
   * File type is changed to be ZFT_ZMasterFile
   * After successfull completion, pDic is loaded as valid, active dictionary for the file.
   * @return a ZStatus see Errors from ZDictionaryFile::saveToDicFile()
   */
  ZStatus createDictionary(const ZMFDictionary& pDic) ;
  /**
   * @brief createExternalDictionary set pDicPath as dictionary for master file.
   * File type is changed to be ZFT_ZMasterFile
   * After successfull completion, dictionary contained in pDicPath is loaded as valid, active dictionary for the file.
   * @return a ZStatus see Errors from ZDictionaryFile::Dictionary->load()
   */
  ZStatus createExternalDictionary(const uriString& pDicPath) ;

  /**
   * @brief zclearAll clears content of master file and of all defined indexes of master file.
   * @return a ZStatus
   */
  ZStatus zclearAll();


//    using _Base::zclearFile;  // for tests only : must be suppressed imperatively

  //--------------Setting parameters------------------------------------
  ZStatus  setFCBParameters (ZFile_type pFileType,
                          const bool pGrabFreeSpace,
                          const bool pHighwaterMarking,
                          const ssize_t pBlockTargetSize,
                          const size_t pBlockExtentQuota)
  { return _Base::_setParameters( pFileType,
                                  pGrabFreeSpace,
                                  pHighwaterMarking,
                                  pBlockTargetSize,
                                  pBlockExtentQuota);
  }

  void setFileType(ZFile_type pFileType) {return _Base::setFileType(pFileType);}

  ZStatus setIndexFilesDirectoryPath (uriString &pPath);
  ZStatus setJournalLocalDirectoryPath (uriString &pPath);

  ZRawIndexFile* zgetIndexObject(const zrank_type pIndexRank) {return IndexTable[pIndexRank];}

  ZStatus zcreateRawMasterFile(const uriString pURI,
                                long pAllocatedBlocks,
                                long pBlockExtentQuota,
                                long pBlockTargetSize,
                                const zsize_type pInitialSize,
                                bool pHighwaterMarking,
                                bool pGrabFreeSpace,
                                bool pJournaling,
                                bool pBackup=false,
                                bool pLeaveOpen=false);

  ZStatus zcreateRawMasterFile (const uriString pURI,
                                const zsize_type pInitialSize,
                                bool pBackup=false,
                                bool pLeaveOpen=false);

//  ZStatus zcreate (const char* pPathName, const zsize_type pInitialSize, bool pBackup=false, bool pLeaveOpen=false);
  /**
 * @brief ZRawMasterFile::zcreateRawIndex Generates a new index from a description (meaning a new ZRandomFile data + header).
 *
 * This routine will create a new index with the files structures necessary to hold and manage it :
 *  a ZIndexFile object will be instantiated.
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
 * 2. ZIndexFile file pathname is not stored but is a computed data from actual ZRawMasterFile file pathname.
 * 3. Master file must be open in mode <ZRF_Exclusive | ZRF_All>
 * 4. Master file and created index file are not closed and remain open after this operation
 *
 * @param[in] pIndexName        User name of the index key as a utfdescString
 * @param[in] pkeyguessedsize Fixed length index key size
 * @param[in] pDuplicates       Type of key : Allowing duplicates (ZST_Duplicates) or not (ZST_NODUPLICATES)
 * @param[in] pBackup           if set to true (default) then index file with the same name will be backup copied.
 *                              if set to false, index file with the same name will be replaced during creation operation.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *  ZS_MODEINVALID ZRawMasterFile must be closed when calling zcreateRawIndex. If not, this status is returned.
 *
 */
  ZStatus zcreateRawIndex  (ZRawIndexFile *&pIndexObjectOut,  /* resulting created index object if successful, nullptr  if not*/
                            const utf8String &pIndexName,
                            uint32_t pkeyguessedsize,
                            ZSort_Type pDuplicates,
                            long &pOutIndexRank,
                            bool pBackup=true);

  ZStatus zinsertRawIndex  (long pIndexRank,
                            ZRawIndexFile *&pIndexObjectOut,  /* resulting created index object if successful, nullptr  if not*/
                            const utf8String &pIndexName,
                            uint32_t pkeyguessedsize,
                            ZSort_Type pDuplicates,
                            long &pOutIndexRank,
                            bool pBackup=true);
#ifdef __COMMENT__
  ZStatus zcreateRawIndexDetailed ( const utf8String &pIndexName, /*-----ICB------*/
                                    uint32_t pkeyguessedsize,
                                    ZSort_Type pDuplicates,
                                    long pAllocatedBlocks,        /* ---FCB (for index ZRandomFile)---- */
                                    long pBlockExtentQuota,
                                    zsize_type pInitialSize,
                                    bool pHighwaterMarking,
                                    bool pGrabFreeSpace,
                                    bool pReplace);
#endif // __COMMENT__
  /**
   * @brief _createRawIndexDet
   *  @warning it is necessary to run zrebuildRawIndex() afterwards
   * @param pIndexName
   * @param pkeyguessedsize
   * @param pDuplicates
   * @param pAllocatedBlocks
   * @param pBlockExtentQuota
   * @param pInitialSize
   * @param pHighwaterMarking
   * @param pGrabFreeSpace
   * @param pReplace
   * @return
   */
  ZStatus _createRawIndexDet (long &pOutRank,
                              const utf8String &pIndexName, /*-----ICB------*/
                              uint32_t pkeyguessedsize,
                              ZSort_Type pDuplicates,
                              long pAllocatedBlocks,      /* ---FCB (for index ZRandomFile)---- */
                              long pBlockExtentQuota,
                              zsize_type pInitialSize,
                              bool pHighwaterMarking,
                              bool pGrabFreeSpace,
                              bool pReplace,
                              ZaiErrors* pErrorLog);
  /**
   * @brief _insertRawIndexDet
   *  @warning it is necessary to run zrebuildRawIndex() afterwards
   * @param pIndexRank
   * @param pIndexName
   * @param pkeyguessedsize
   * @param pDuplicates
   * @param pAllocatedBlocks
   * @param pBlockExtentQuota
   * @param pInitialSize
   * @param pHighwaterMarking
   * @param pGrabFreeSpace
   * @param pReplace
   * @param pErrorLog
   * @return
   */
  ZStatus _insertRawIndexDet (long pInputIndexRank,
                              const utf8String &pIndexName, /*-----ICB------*/
                              uint32_t pkeyguessedsize,
                              ZSort_Type pDuplicates,
                              long pAllocatedBlocks,      /* ---FCB (for index ZRandomFile)---- */
                              long pBlockExtentQuota,
                              zsize_type pInitialSize,
                              bool pHighwaterMarking,
                              bool pGrabFreeSpace,
                              bool pReplace,
                              ZaiErrors* pErrorLog);

  int _getAvailableBckNum(const uriString& pBackupPath,const utf8VaryingString &pBckExt);
/**
 * @brief ZRawMasterFile::backupAll backs up all files, dictionary file (if any) and index files, composing master file
 *
 *
 * <pBackupPath><master root name>.<actual master file extension>_<bck ext>99
 * <pBackupPath><master root name>.<__HEADER_FILEEXTENSION__>_<bck ext>99
 *
 * for indexes :
 *   <pBackupPath><master root name><index name>.<__ZINDEX_FILEEXTENSION__>_<bck ext>99
 *   <pBackupPath><master root name><index name>.<__HEADER_FILEEXTENSION__>_<bck ext>99
 *
 *  where 99 means a 2 digits with leading zero backup version value [ 01 up to 99 ]
 *
 * @param pBackupPath if omitted then master file directory path is used
 * @param pBckExt defaulted to "bck"
 * @return
 */
  ZStatus backupAll(const uriString& pBackupPath,const utf8VaryingString &pBackupSetName);

  void getBackupSet(ZArray<BckElement>& pFileList);

  ZStatus XmlSaveBackupset(uriString& pXmlFile, ZArray<BckElement>& pFileList, bool pComment);
  ZStatus XmlLoadBackupset(const uriString& pXmlFile, ZArray<BckElement>& pFileList, ZDateFull& pBackupDate);

  /* Deprecated */
  /**
 * @brief ZRawMasterFile::backupAll DEPRECATED backs up all files dictionary file and index files composing master file
 *
 *
 * <pBackupPath><master root name>.<actual master file extension>_<bck ext>99
 * <pBackupPath><master root name>.<__HEADER_FILEEXTENSION__>_<bck ext>99
 *
 * for indexes :
 *   <pBackupPath><master root name><index name>.<__ZINDEX_FILEEXTENSION__>_<bck ext>99
 *   <pBackupPath><master root name><index name>.<__HEADER_FILEEXTENSION__>_<bck ext>99
 *
 *  where 99 means a 2 digits with leading zero backup version value [ 01 up to 99 ]
 *
 * @param pBackupPath if omitted then master file directory path is used
 * @param pBckExt defaulted to "bck"
 * @return
 */
  ZStatus backupAll_old(const uriString& pBackupPath,const utf8VaryingString &pBckExt="bck");

  ZStatus extractBasenameBack(const uriString& pBckFile, utf8VaryingString &pBasename, utf8VaryingString &pBckExt, int &pNum);
  /**
   * @brief zremoveAll removes all records from master file as well as from all index files.
   */
  ZStatus zremoveAll();

  void _testZReserved();

  bool hasDictionary()  {return Dictionary!=nullptr;}
  bool hasIndex()       {return IndexTable.count()>0;}
  bool hasJournal()     {return ZJCB != nullptr;}

  ZStatus zopen       (const uriString& pURI, const int pMode=ZRF_All); // superseeds ZRandomfile zopen
  ZStatus zopen       (const int pMode=ZRF_All) {return (zopen(getURIContent(),pMode));}

  ZStatus zopenIndexFile(long pRank, const int pMode);

  ZStatus zclose (void);

  ZStatus _writeAllHeaders();

  /** @brief _removeFile   local method that removes all files component of ZRawMasterFile @see removeFile */
  ZStatus _removeFile(const uriString &pContentPath, ZaiErrors *pErrorLog=nullptr);
  /**
   * @brief removeFile   static function that physically removes all files component of ZRawMasterFile
   * whose Content file is pointed by pContentPath :
   * for main file : ZRandomFile content + header
   * for each index : ZRandomFile content + header
   *
   * @return  a ZStatus  for errors @see ZRandomFile::_removeFile
   */
  static ZStatus removeMasterFile (const uriString &pContentPath, ZaiErrors *pErrorLog=nullptr);

  uriString getURIIndex(long pIndexRank) ;

  /**
   * @brief _renameBck   local method that renames all component files for ZRawMasterFile whose content file is pointed by pContentPath
   * to a new name with a special extension suffix given by pBckExt plus a incremental 2 digit value :
   *
   * <base filename>.<extension>_<pBckExt><nn>
   *
   * pBckExt is defaulted to string "bck".
   *
   * Components are :
   * for main file : ZRandomFile content + header
   * for each index : ZRandomFile content + header
   *
   * @return  a ZStatus for error codes @see ZRandomFile::_renameBck
   */
  ZStatus _renameBck(const char* pContentPath, ZaiErrors* pErrorLog=nullptr,const char* pBckExt="bck");

  /**
   * @brief renameBck   static function that renames all component files for ZRawMasterFile whose content file is pointed by pContentPath
   * to a new name with a special extension suffix given by pBckExt plus a incremental 2 digit value :
   *
   * <base filename>.<extension>_<pBckExt><nn>
   *
   *
   * pBckExt is defaulted to string "bck".
   *
   *
   * Components are :
   * for main file : ZRandomFile content + header
   * for each index : ZRandomFile content + header
   * @return  a ZStatus for error codes @see ZRandomFile::_renameBck
   */
  static ZStatus renameBck (const char* pContentPath, ZaiErrors* pErrorLog=nullptr, const char* pBckExt="bck");


  ZStatus zget      (ZDataBuffer &pRecordContent, const zrank_type pZMFRank);

  ZStatus zgetPerIndex (ZDataBuffer &pRecordContent, const long pIndexRank,long pKeyRank);
  ZStatus zgetFirstPerIndex (ZDataBuffer &pRecordContent, const long pIndexRank);
  ZStatus zgetNextPerIndex (ZDataBuffer &pRecordContent, const long pIndexRank);

  /** @brief zremoveByRank Remove by rank : removes record at position pZMFRank */
  ZStatus zremoveByRank     (const zrank_type pZMFRank) ;

  /** @brief zremoveByRankR Remove by rank with return : removes record at position pZMFRank
   *        and returns removed record content.
   *        Internal ZRawRecord structure is updated with removed record content.
   */
  ZStatus zremoveByRankR    (ZDataBuffer &pRecordContent, const zrank_type pZMFRank);

  /**
   * @brief zadd Adds a new record whose content is pRecordContent within a Raw Master File at the last logical position
   * and updates all indexes.
   *      As Raw Master File does not use any dictionary, user must extract and provide keys content (pKeys).
   */
  ZStatus zadd      (ZDataBuffer& pRecordContent, ZArray<ZDataBuffer> &pKeys );



  /**
   * @brief zinsert insert a new record whose content is pRecordContent within a Raw Master File at logical position pRank, and updates all indexes.
   *      As Raw Master File does not use any dictionary, user must extract and provide keys content (pKeys).
   */
  ZStatus zinsert   (ZDataBuffer& pRecordContent, ZArray<ZDataBuffer> &pKeys, const zrank_type pZMFRank);



protected:

//  ZStatus _addRaw   (ZDataBuffer& pRecord, ZArray<ZDataBuffer>& pKeysContent);
  ZStatus _addRaw   (ZDataBuffer& pRecord, ZArray<ZDataBuffer> &pKeysContent);

  ZStatus _insertRaw (const ZDataBuffer& pRecord, ZArray<ZDataBuffer>& pKeys, const zrank_type pZMFRank);
  ZStatus _removeByRankR  (ZDataBuffer &pRecord, const zrank_type pZMFRank);

  /* Deprecated
  ZStatus _addRawOld   (ZDataBuffer& pRecord, ZArray<ZDataBuffer>& pKeysContent);
  ZStatus _insertRawOld (const ZDataBuffer& pRecord, ZArray<ZDataBuffer>& pKeys, const zrank_type pZMFRank);
*/


  ZStatus _commitIndexes(ZArray <ZIndexItem*>  &pIndexItemList);
  ZStatus _rollbackIndexes (ZArray <ZIndexItem*>  &pIndexItemList);

  ZStatus _getRaw   (ZDataBuffer &pRecord, const zrank_type pRank);

  ZStatus _getFirst (ZDataBuffer &pRecord) ;
  ZStatus _getNext (ZDataBuffer &pRecord) ;

  ZStatus _getByKey (ZDataBuffer &pRecord, ZDataBuffer &pKeyValue, const zrank_type pKeyNumber,ZIndexItem* pOutIndexItem);

  /**
 * @brief ZRawMasterFile::writeMasterControlBlock updates ZMasterControlBlock AND ZSJournalControlBlock (if exists)
 * for current ZRawMasterFile to ZReserved Header within header file.
 *
 * see @ref ZMasterControlBlock::_exportMCB()
 * see @ref ZSJournalControlBlock::_exportJCB()
 *
 * @return
 */
public:  ZStatus writeControlBlocks(void);
  /**
 * @brief ZRawMasterFile::readControlBlocks reads ZMasterControlBlock AND ZSJournalControlBlock is exists
 * see @ref ZMasterControlBlock::_import()
 * see @ref ZSJournalControlBlock::_import()
 * @return
 */
public:  ZStatus readControlBlocks(void);

  /**
 * @brief ZRawMasterFile::readJCBfromHeader Accesses File header and updates JournalingControlBlock if any.
 * There is no reverse operation : writting JCB to Header is done using writing whole Control blocks to header file
 * using ZRawMasterFile::writeControlBlocks()
 *
 * @return
 */
protected:  ZStatus readJCBfromHeader(void);
/**
 * @brief ZRawMasterFile::_getJCBfromReserved updates Journaling control block if any with its content from ZReserved from Filedescriptor.
 *  ZReserved have to be up to date with an already done getReservedBlock().
 * @return
 */
protected:  ZStatus _getJCBfromReserved(void);

public:

  //---------search and get operations--------------------------------
  //

  //-------relative get  (use ZRandomFile base routines)------------------------
  //

  //    ZStatus zgenerateKeyValueList (ZDataBuffer &pKey , long pKeyNumber, int pCount,...);
  //    ZStatus zgenerateKeyValue (ZSIndexControlBlock& pICB, ZArray<void *> &pKeyValues, ZDataBuffer& pKey);

  ZStatus zsearch (ZDataBuffer &pRecord, ZDataBuffer &pKeyValue, const long pIndexNumber);
//  ZStatus zsearch (ZDataBuffer &pRecord, ZKey *pKey);

  ZStatus zsearchAll (ZDataBuffer &pKeyValue,
                      const long pIndexNumber,
                      ZIndexCollection &pIndexCollection,
                      const ZMatchSize_type pZMS=ZMS_MatchKeySize);
#ifdef __DEPRECATED__
  ZStatus zsearchAll (ZKey &pZKey, ZIndexCollection& pIndexCollection);

  ZStatus zsearchFirst (ZKey &pZKey, ZDataBuffer &pOutRecord, ZIndexCollection *pCollection);
  ZStatus zsearchNext (ZKey &pZKey, ZDataBuffer &pRecord, ZIndexCollection *pCollection);
#endif // __DEPRECATED__
  using ZRandomFile::zsearchFieldAllCollection;
  using ZRandomFile::zsearchFieldFirstCollection;
  using ZRandomFile::zsearchFieldNextCollection;

  using ZRandomFile::getFileDescriptor ;


  ZStatus zsearchInterval (ZKey &pZKeyLow, ZKey &pZKeyHigh,const zrank_type pIndexNumber,ZIndexCollectionContext *pSearchContext );


  //                  Reports
  void ZMCBreport(FILE *pOutput=stdout);
  //                  Utilities


  ZStatus zclearMCB (FILE *pOutput=nullptr);

  ZStatus zremoveIndex (const long pIndexRank, bool pBackup=false, ZaiErrors *pErrorLog=nullptr);

  ZStatus shiftIndexNameDown(long pStartRank,ZaiErrors* pErrorLog);
  ZStatus shiftIndexNameUp(long pStartRank,ZaiErrors* pErrorLog);

  //------------Surface utilities---------------------------------
  /* for the following routines : see zrawmasterfileutils.h -> using static templates */
#ifdef __DEPRECATED__
  ZStatus zreorgRawFile(bool pDump=false, FILE *pOutput=stdout); // replaces the ZRandomFile::zreorgFile()
  ZStatus zindexRebuild (const long pIndexRank,bool pStat=false, FILE *pOutput=stdout);
#endif

  //--------------XML reports & utilities---------------------------------

  /** @brief XmlSaveToFile() Exports ZRandomFile parameters to full Xml formatted file */
  ZStatus XmlSaveToFile(uriString &pXmlFile,bool pComment=true);
  /** @brief XmlSaveToFile() Exports all ZRandomFile parameters to full Xml formatted string */
  utf8String XmlSaveToString(bool pComment=true);
  /**
   * @brief toXml writes ZRawMasterFile definition as xml at level (indentation) pLevel.
   * If pComment is set to true, then available explainations are commented in output xml code.
   * @return an utf8String with xml definition
   */
  utf8String toXml(int pLevel,bool pComment=true);



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
  /** @brief getRawIndex
   * @param pIndexItem
   * @param pIndexRank
   * @param pKeyNumber
   * @return
   */
  ZStatus getRawIndex(ZIndexItem &pIndexItem, const zrank_type pIndexRank, const zrank_type pKeyNumber);

  // -------------------ZMasterFile operators-------------------------

  /**
  * @brief operator [] Returns a ZDataBuffer containing the record pointed by pIndex rank in ZBlockAccessTable pool,
  *                    while base ZArray operator  [] returns a reference to the rank.
  *
  *                    As it uses ZRandomFile::CurrentRecord data structure this is NOT usable within a multi thread context.
  *
  *                    Master File rank cannot be modified (because of potential Keys modification): use replace() method instead.
  * @warning not usable in a multi threading context.
  *
  * @param pIndex
  * @return a ZDataBuffer with Record content
  */
  ZDataBuffer  operator [] (const long pIndex) { zget(CurrentRecord,pIndex);return CurrentRecord;}   // resulting _Type cannot be modified : use replace() method instead

  //------------------end operators------------------------

   /* ------------following routines are to be used with a generated class----------------*/
/*
  template <class _Tp>
  ZStatus zadd_T(_Tp& pClass) {
    ZDataBuffer wRecordContent=pClass.toRecord();
    ZArray<ZDataBuffer> wKeys = pClass.getAllKeys();
    return _addRaw(wRecordContent,wKeys);
  }
  */

  template <class _Tp>
  ZStatus zadd_T(_Tp& pClass) {
    ZDataBuffer wRecordContent=pClass.toRecord();
    ZArray<ZDataBuffer> wKeys = pClass.getAllKeys();
    return _addRaw(wRecordContent,wKeys);
  }
  template <class _Tp>
  ZStatus zinsert_T(_Tp& pClass, const zrank_type pZMFRank) {
    ZDataBuffer wRecordContent=pClass.toRecord();
    return zinsert(wRecordContent,pClass.getAllKeys(),pZMFRank);
  }

  template <class _Tp>
  ZStatus zget_T(_Tp& pClass, const zrank_type pZMFRank) {
    ZDataBuffer wRecordContent;
    ZStatus wSt=zget(wRecordContent,pZMFRank);
    if (wSt==ZS_SUCCESS)
      pClass.fromRecord(wRecordContent);
    return wSt;
  }

/* NB: zremoveByRankR() does not need to have a template here as it does not use record content */

  template <class _Tp>
  ZStatus zremoveByRankR_T(_Tp& pClass, const zrank_type pZMFRank) {
    ZDataBuffer wRecordContent;
    ZStatus wSt=zremoveByRankR(wRecordContent,pZMFRank);
    if (wSt==ZS_SUCCESS)
      pClass.fromRecord(wRecordContent);
    return wSt;
  }
   //---------search and get operations--------------------------------

  template <class _Tp>
  ZStatus zsearch_T(_Tp& pClass, const long pIndexNumber) {
    return zsearch(pClass.toRecord(),pClass.getAllKeys(),pIndexNumber);
  }

  template <class _Tp>
  ZStatus zsearchAll_T(_Tp& pClass, const long pIndexNumber,ZIndexCollection &pIndexCollection,
      const ZMatchSize_type pZMS=ZMS_MatchKeySize) {
    return zsearchAll(pClass.toRecord(),pClass.getAllKeys(),pIndexNumber);
  }


private:


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
    _Mutex.lock();

    return;
  }

  void ZMFunlock(void)
  {
    _Mutex.unlock();
    return;
  }
#endif // __USE_ZTHREAD__

public:
  //======================Journaling=======================================


  bool getJournalingStatus (void)
  {
    return(ZJCB!=nullptr);
  }

  ZStatus journalingSetup (uriString &pJournalPath);
  ZStatus setJournalingOn (void);
  ZStatus initJournaling (uriString &pJournalPath);
  ZStatus setJournalingOff (void);


//  ZRawRecord* generateRawRecord();


  void setExtractKeyRoutine(extractRawKey_type pExtract)
  {
    extractRawKey_func=pExtract;
  }

  // =======================Data fields======================================


protected:
  using _Base::operator = ;

#ifdef __USE_ZTHREAD__
  ZMutex    _Mutex;
#endif


protected:


//  ZRawRecord*  RawRecord=nullptr;

public:

  // permanent public data
  //
//  ZSMasterControlBlock  ZMCB;      //< Master Control Block : all data is there
  ZMFStats              ZPMSStats; //< Statistical data

  extractRawKey_type    extractRawKey_func=nullptr;

  ZaiErrors             ErrorLog;
}; //--------------------end class ZMasterFile-------------------------------




} // namespace zbs
#endif // ZRAWMASTERFILE_H
