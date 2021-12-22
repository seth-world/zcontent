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

#include <zindexedfile/zsmastercontrolblock.h>
#include <zindexedfile/zindexcontrolblock.h>

#include <zindexedfile/zsindexitem.h>



/* define functor for key extraction */

typedef ZStatus (*extractRawKey_type) (ZDataBuffer &pRawRecord,ZRawIndexFile* pZIF,ZDataBuffer* pKeyContent);


namespace zbs //========================================================================
{

class ZRawMasterFile: protected ZRandomFile, public ZSMasterControlBlock
{
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
  using ZRandomFile::setPath;
  using ZRandomFile::getFCB;

  using ZRandomFile::getSize;
  using ZRandomFile::isEmpty;
  using ZRandomFile::getURIContent;
  using ZRandomFile::zgetWAddress;
  using ZRandomFile::zgetNextWAddress;
  
  using ZRandomFile::isOpen;
  using ZRandomFile::getOpenMode;

  using ZRandomFile::getBlockTargetSize;
  using ZRandomFile::getAllocatedBlocks;
  using ZRandomFile::getBlockExtentQuota;
  using ZRandomFile::getHighwaterMarking;
  using ZRandomFile::getInitialSize;

  

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

  ZStatus zcreate ( const uriString pURI, const zsize_type pInitialSize, bool pBackup=false, bool pLeaveOpen=false);

//  ZStatus zcreate (const char* pPathName, const zsize_type pInitialSize, bool pBackup=false, bool pLeaveOpen=false);
  /**
 * @brief ZRawMasterFile::zcreateRawIndex Generates a new index from a description (meaning a new ZRandomFile data + header).
 *
 * This routine will create a new index with the files structures necessary to hold and manage it : a ZSIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZSIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 *
 *@note
 * 1. ZSIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZSIndexFile file pathname is not stored but is a computed data from actual ZRawMasterFile file pathname.
 * 3. Master file must be open in mode <ZRF_Exclusive | ZRF_All>
 * 4. Master file and created index file are not closed and remain open after this operation
 *
 * @param[in] pIndexName        User name of the index key as a utfdescString
 * @param[in] pKeyUniversalSize Fixed length index key size
 * @param[in] pDuplicates       Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
 * @param[in] pBackup           if set to true (default) then index file with the same name will be backup copied.
 *                              if set to false, index file with the same name will be replaced during creation operation.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *  ZS_MODEINVALID ZRawMasterFile must be closed when calling zcreateRawIndex. If not, this status is returned.
 *
 */
  ZStatus zcreateRawIndex  (ZRawIndexFile *&pIndexObjectOut,  /* resulting created index object if successful, nullptr  if not*/
                            const utf8String &pIndexName,
                            uint32_t pKeyUniversalSize,
                            ZSort_Type pDuplicates,
                            bool pBackup=true);

  ZStatus zcreateRawIndexDetailed ( const utf8String &pIndexName, /*-----ICB------*/
                                    uint32_t pKeyUniversalSize,
                                    ZSort_Type pDuplicates,
                                    long pAllocatedBlocks,        /* ---FCB (for index ZRandomFile)---- */
                                    long pBlockExtentQuota,
                                    zsize_type pInitialSize,
                                    bool pHighwaterMarking,
                                    bool pGrabFreeSpace,
                                    bool pReplace);

  ZStatus _createRawIndexDet (const utf8String &pIndexName, /*-----ICB------*/
                              uint32_t pKeyUniversalSize,
                              ZSort_Type pDuplicates,
                              long pAllocatedBlocks,      /* ---FCB (for index ZRandomFile)---- */
                              long pBlockExtentQuota,
                              zsize_type pInitialSize,
                              bool pHighwaterMarking,
                              bool pGrabFreeSpace,
                              bool pReplace);

  void setDictionary(const ZMFDictionary& pDictionary);


  bool hasDictionary() {return MasterDic!=nullptr;}
  bool hasIndexTable() {return IndexTable.count()>0;}

  ZStatus zopen       (const uriString& pURI, const int pMode=ZRF_All); // superseeds ZRandomfile zopen
  ZStatus zopen       (const int pMode=ZRF_All) {return (zopen(getURIContent(),pMode));}
  //    ZStatus zopen       (const int pMode=ZRF_All) {return zopen(ZDescriptor.URIContent,pMode);}

  ZStatus zclose (void);


  /** @brief _removeFile   local method that removes all files component of ZRawMasterFile @see removeFile */
  ZStatus _removeFile(const char* pContentPath, ZaiErrors *pErrorLog=nullptr);
  /**
   * @brief removeFile   static function that physically removes all files component of ZRawMasterFile
   * whose Content file is pointed by pContentPath :
   * for main file : ZRandomFile content + header
   * for each index : ZRandomFile content + header
   *
   * @return  a ZStatus  for errors @see ZRandomFile::_removeFile
   */
  static ZStatus removeFile (const char* pContentPath, ZaiErrors *pErrorLog=nullptr);


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
  ZStatus _addRaw   (ZRawRecord *pRecord);
  ZStatus _insertRaw(ZRawRecord *pRecord, const zrank_type pZMFRank);
  ZStatus _getRaw   (ZRawRecord *pRecord, const zrank_type pZMFRank);
  ZStatus _removeByRankR    (ZRawRecord* pZMFRecord, const zrank_type pZMFRank);


public:  ZStatus writeControlBlocks(void);
public:  ZStatus readControlBlocks(void);

protected:  ZStatus readJCBfromHeader(void);
protected:  ZStatus _getJCBfromReserved(void);

public:

  //---------search and get operations--------------------------------
  //

  //-------relative get  (use ZRandomFile base routines)------------------------
  //

  //    ZStatus zgenerateKeyValueList (ZDataBuffer &pKey , long pKeyNumber, int pCount,...);
  //    ZStatus zgenerateKeyValue (ZSIndexControlBlock& pICB, ZArray<void *> &pKeyValues, ZDataBuffer& pKey);

  ZStatus zsearch (ZDataBuffer &pRecord, ZDataBuffer &pKeyValue, const long pIndexNumber);
  ZStatus zsearch (ZDataBuffer &pRecord, ZSKey *pKey);

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
  void ZMCBreport(FILE *pOutput=stdout);
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

  ZStatus zremoveIndex (const long pIndexRank, ZaiErrors *pErrorLog=nullptr);
  //------------Surface utilities---------------------------------

  ZStatus zreorgFile(bool pDump=false, FILE *pOutput=stdout); // replaces the ZRandomFile::zreorgFile()
  ZStatus zindexRebuild (const long pIndexRank,bool pStat=false, FILE *pOutput=stdout);


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
  ZStatus getRawIndex(ZSIndexItem &pIndexItem, const zrank_type pIndexRank, const zrank_type pKeyNumber);

  // -------------------ZSMasterFile operators-------------------------

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
private:

  //------------Add sequence---------------------------------------
  /** @cond Development */
  ZStatus _add_RollbackIndexes (ZArray<zrank_type> &pIndexRankProcessed);
  ZStatus _add_HardRollbackIndexes (ZArray<zrank_type> &pIndexRankProcessed);

  ZStatus _add_CommitIndexes (ZArray <ZSIndexItem*>  &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed);

  //-----------End Add sequence------------------------------------

  // ----------Remove sequence-------------------------------------



  ZStatus _removeByRankRaw  (ZRawRecord *pRawRecord, const zrank_type pZMFRank);



  ZStatus _remove_RollbackIndexes (ZArray<zrank_type> &pIndexRankProcessed);

  ZStatus _remove_HardRollbackIndexes (ZArray<ZSIndexItem*> &pIndexItemList,ZArray<zrank_type> &pIndexRankProcessed);

  ZStatus _remove_CommitIndexes ( ZSIndexItemList &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed);
  /** @endcond */
  //-----------End Remove sequence------------------------------------



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


  ZRawRecord* generateRawRecord();


  void setExtractKeyRoutine(extractRawKey_type pExtract)
  {
    extractRawKey_func=pExtract;
  }

  // =======================Data fields======================================


protected:
  using _Base::operator = ;

#ifdef __USE_ZTHREAD__
  ZMutex    _Mtx;
#endif


protected:


  ZRawRecord*  RawRecord=nullptr;

public:

  // permanent public data
  //
//  ZSMasterControlBlock  ZMCB;      //< Master Control Block : all data is there
  ZMFStats              ZPMSStats; //< Statistical data

  extractRawKey_type    extractRawKey_func=nullptr;

  ZaiErrors             ErrorLog;
}; //--------------------end class ZSMasterFile-------------------------------


} // namespace zbs
#endif // ZRAWMASTERFILE_H
