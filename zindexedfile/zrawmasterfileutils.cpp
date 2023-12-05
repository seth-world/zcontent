
#include <zindexedfile/zrawmasterfileutils.h>

#include <zcontent/zindexedfile/zmasterfile.h>

/**
  @ingroup ZMFPhysical
  @{ */



/**
 * @brief generateIndexRootName generates a ZIndexFile root name from its father ZSMasterFile's root name
 *
 * ZIndexFile name generation rule
 *@verbatim
 *          <master root name>[<index name>]-<index rank>
 *@endverbatim
 *
 * @param[in] pMasterRootName   a descString containing the father ZSMasterFile's root name
 * @param[in] pRank             Index rank
 * @param[in] pIndexName        Index user name : could be empty
 * @return an utf8VaryingString with the appropriate ZIndexFile root name
 */
utf8VaryingString generateIndexRootName(const utf8VaryingString &pMasterRootName,
                                        const utf8VaryingString &pIndexName)
{
  utf8VaryingString wIndexRootName;
  wIndexRootName = pMasterRootName;
  wIndexRootName.addUtfUnit('-');
  if (pIndexName.isEmpty())
  {
    wIndexRootName+="index";
  }
  else
  {
    wIndexRootName += pIndexName.toCChar();
  }
  wIndexRootName.eliminateChar(' ');
  wIndexRootName = wIndexRootName.toLower();
//  wIndexRootName += __ZINDEX_FILEEXTENSION__;
  return wIndexRootName;
} // generateIndexRootName

utf8VaryingString generateIndexBaseName(const utf8VaryingString &pMasterRootName,
    const utf8VaryingString &pIndexName)
{
  utf8VaryingString wIndexRootName = generateIndexRootName(pMasterRootName,pIndexName);
  wIndexRootName += __ZINDEX_FILEEXTENSION__;
  return wIndexRootName;
} // generateIndexBaseName

/**
 * @brief generateIndexURI
 *              generates the index uri full base name (including directory path) but without any extension (and without '.' char)
 *
 *  If no directory is mentionned in pMasterName path, then the current directory is taken.
 *  returns a ZS_INVNAME status if ZSMasterFile's extension is __ZINDEX_FILEEXTENSION__
 *  returns ZS_SUCCESS otherwise.
 *
 *  For base name generation @see generateIndexRootName
 *
 * @param[out] pZIndexFileUri  Resulting ZIndexFile name
 * @param[in]  pMasterFileUri   Base name for ZSMasterFile to create ZIndexFile name for
 * @param[in]  pPathDir         Directory path to create the ZIndexFile in
 * @param [in] pRank            Index rank for the ZSMasterFile
 * @param[in]  pIndexName       Index name (given by user) for the Index
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
generateIndexURI( uriString &pIndexFileUri,
                  const uriString& pMasterFileUri,
                  const uriString& pDirectory,
                  const utf8VaryingString& pIndexName)
{
  uriString  wPath_Uri;
  utf8VaryingString wMasterRoot;
  utf8VaryingString wMasterExt;


  if (pDirectory.isEmpty())
    wPath_Uri = pMasterFileUri.getDirectoryPath();
  else
    wPath_Uri = pDirectory;

//  QUrl wUrl(wPath_Uri.toCChar());
//  pIndexFileUri.fromQString(wUrl.toString(QUrl::PreferLocalFile));
  pIndexFileUri = wPath_Uri;
  pIndexFileUri.addConditionalDirectoryDelimiter() ;

  wMasterRoot = pMasterFileUri.getRootname();
  wMasterExt=pMasterFileUri.getFileExtension().toCChar();
  const utf8_t* wExt=(const utf8_t*)__ZINDEX_FILEEXTENSION__;
  wExt++;                             // skip the '.' char
  if (wMasterExt==wExt)
  {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_INVNAME,
        Severity_Error,
        " Invalid ZSMasterFile name extension %s for name %s",
        wMasterExt.toCChar(),
        pMasterFileUri.toCChar()
        );
    return ZS_INVNAME;
  }

  utf8VaryingString wM;

  wM=generateIndexBaseName(wMasterRoot,pIndexName);
  pIndexFileUri += wM.toString();

  return(ZS_SUCCESS);
} //generateIndexURI

/** @} */ // ingroup ZMFPhysical


utf8VaryingString ZStringBuffer2;
const char *decode_ZCOP (uint16_t pZCOP)
{
  if (pZCOP==ZCOP_Nothing)
    return "ZCOP_Nothing" ;

  ZStringBuffer2.clear();

  if (pZCOP & ZCOP_Interval)
  {
    ZStringBuffer2=(const utf8_t*)"ZCOP_Interval |" ;

  }
  if (pZCOP & ZCOP_Exclude)
  {
    ZStringBuffer2 += (const utf8_t*)" ZCOP_Exclude |" ;
  }

  if (pZCOP & ZCOP_GetFirst)
    ZStringBuffer2 += (const utf8_t*)" ZCOP_GetFirst";
  if (pZCOP & ZCOP_GetNext)
    ZStringBuffer2 += (const utf8_t*)" ZCOP_GetNext";
  if (pZCOP & ZCOP_GetAll)
    ZStringBuffer2 += (const utf8_t*)" ZCOP_GetAll";
  if (pZCOP & ZCOP_RemoveAll)
    ZStringBuffer2 += (const utf8_t*)" ZCOP_RemoveAll";
  if (pZCOP & ZCOP_LockAll)
    ZStringBuffer2 += (const utf8_t*)" ZCOP_LockAll";


  return ZStringBuffer2.toCString_Strait() ;
} // decode_ZCOP

/**  * @addtogroup ZMFUtilities
 * @{ */

/**
 *
 * @brief ZRawMasterFile::zrepairIndexes Scans and Repairs indexes of a ZSMasterFile
 *
 *  zrepairIndexes unlock the file in case it has been left open and locked.
 *
 *  For any defined index of the given ZSMasterFile, it tests index file presence and controls ZIndexControlBlock integrity.
 *
 *  It repairs damaged indexes if pRepair option is set to true (and rebuilds them).
 *
 *  It rebuilds all indexes if pRebuildAll is set to true
 *
 *  This routine may be used to test wether a file structure is still correct.
 *
 *  In addition, it may be used to regularly rebuild and reorder indexes if necessary.
 *
 * @warning Be sure to use zrepairIndexes in a standalone context, as it opens the file regardless the possible locks.
 *
 * @param[in] pZMFPath      a const char* with the ZSMasterFile main content file path.
 * @param[in] pRepair       if set this option will try to repair/ rebuilt damaged or missing indexes. If not a report is emitted.
 * @param[in] pRebuildAll   Option if set (true) then all indexes are rebuilt, even if they are healthy indexes.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.repairlog
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus zrepairIndexes (const char *pZMFPath,
                        bool pRepair,
                        bool pRebuildAll,
                        ZaiErrors* pErrorLog)
{
/*
    pErrorLog->setAutoPrintOn(ZAIES_Text);
    pErrorLog->setStoreMinSeverity(ZAIES_Warning);
*/
    ZStatus wSt;
  ZRawMasterFile  wMasterFile;
  ZRandomFile     wMasterZRF;

  ZIndexControlBlock* wZICB=nullptr;
  ZRawIndexFile wIndexFile(&wMasterFile);
  ZRandomFile wIndexZRF;
  uriString   wURIContent;
  uriString   wIndexUri;
  ZDataBuffer wReservedBlock;
  size_t      wImportSize;
  zsize_type  wIndexAllocatedSize;
  ZDataBuffer wICBContent;

  long wi=0, IndexRank=0;
/*
  FILE* wOutput=nullptr;
  bool FOutput=false;
  utfdescString wBase;
*/
  ZArray<char> IndexPresence ; // 0 : Index to be deleted     1 : Index present but not to be rebuilt    2 : Index to be built or rebuilt

  long wIndexProcessed = 0,wMissIndexFile = 0, wCorruptZICB = 0, wCreatedIndex = 0, wRebuiltIndex = 0 ;

  ZMasterControlBlock wMCB(wMasterFile);
  const unsigned char* wPtrIn=nullptr;


  wURIContent = pZMFPath;
/*
  wOutput=pOutput;
  if (pOutput==nullptr)
  {
    //       utfdescString wDInfo;
    wBase=wURIContent.getBasename().toCChar();
    wBase+=".repairlog";
    wOutput=fopen(wBase.toCChar(),"w");
    if (wOutput==nullptr)
    {
      wOutput=stdout;
      fprintf(wOutput,
          "%s>>  cannot open file <%s> redirected to stdout\n",
          _GET_FUNCTION_NAME_,
          wBase.toString());
    }
    else
    {
      FOutput=true;
    }
  } // if nullptr
*/
  pErrorLog->textLog( "_____________________________________________________________________________________________");
  pErrorLog->textLog(" starting repairing indexes for ZSMasterFile <%s>", pZMFPath);


  wSt=wMasterZRF.setPath(wURIContent);
  if (wSt!=ZS_SUCCESS) {
      pErrorLog->logZExceptionLast("zrepairIndexes");
      return wSt;
  }
  wSt=wMasterZRF._ZRFopen (ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile,true);  // open ZMF using ZRandomFile routines
  if (wSt!=ZS_SUCCESS)
    goto ErrorRepairIndexes;
  //  Must be a ZFT_ZRawMasterFile

  if ((wMasterZRF.getFileType()!=ZFT_ZRawMasterFile)||(wMasterZRF.getFileType()!=ZFT_ZMasterFile)) {
    pErrorLog->textLog(" **** Fatal error : file is not of mandatory type Master file (either raw or with dictionary) but is <%s> ******",
        decode_ZFile_type( wMasterZRF.getFileType()));
      wSt=ZS_INVTYPE;
    goto ErrorRepairIndexes;
  }

  //wMasterFile.ZMFURI = wMasterZRF.getURIContent();     // align uris: ZMFURI is getting redundant. Only using ZRandomFile URIContent

  pErrorLog->textLog(" getting ZReservedBlock content and load ZMasterControlBlock");

  wSt=wMasterZRF._getReservedHeader(true);     // get reserved block content
  if (wSt!=ZS_SUCCESS)
  {
    ZException.exit_abort();
  }
  wPtrIn= wMasterZRF.getFileDescriptor().ZReserved.Data;
  wSt=wMasterFile._import(wPtrIn);     // load ZMCB from reserved block content
  if (wSt!=ZS_SUCCESS)
  {
    ZException.exit_abort();
  }

  pErrorLog->textLog(
      " existing ZSMasterFile index(es)\n"
      "            <%ld>  defined index(es) in ZMasterControlBlock",
      wMasterFile.IndexTable.size());

  wMasterFile.MCBreport(pErrorLog);

  /*
 *  for each index
*/
  IndexPresence.setAllocation(wMasterFile.IndexTable.size());
  for (long wi=0; wi < wMasterFile.IndexTable.size();wi++) {
    IndexPresence[wi]=0;
  }
//  IndexPresence.bzero();
  IndexRank=0;
  for (IndexRank=0;IndexRank<wMasterFile.IndexTable.size();IndexRank++)
  {
    wSt=generateIndexURI(wIndexUri,
        wMasterFile.getURIContent(),
        wMasterFile.IndexFilePath,
//        IndexRank,
        wMasterFile.IndexTable[IndexRank]->IndexName);
    if (wSt!=ZS_SUCCESS)
    {
      return  wSt;// Beware return  is multiple instructions in debug mode
    }
   pErrorLog->textLog(
        "   .....rank <%ld> processing index file <%s> ",
        IndexRank,
        wIndexUri.toString());
    if (wIndexUri.exists())
      pErrorLog->textLog("       Index file has been found");
    else
    {
      pErrorLog->textLog(
          "\n  ****Error Index file <%s> is missing ****\n"
          "                        Index will be created then rebuilt\n",
          wIndexUri.toString());

      wMissIndexFile++;

      IndexPresence[IndexRank]= 1;
      if (!pRepair)
        continue;
      //---------------Create a new ZIndexFile-------------------------


      pErrorLog->textLog(" creating index file");

      wIndexAllocatedSize=0;
      if (wMasterZRF.getBlockTargetSize()>0)
        if (wMasterZRF.getAllocatedBlocks()>0)
          wIndexAllocatedSize =  wMasterZRF.getAllocatedBlocks() * wMasterFile.IndexTable[wi]->IndexRecordSize();

      wSt =  wIndexFile.zcreateIndexFile((ZIndexControlBlock&)*wMasterFile.IndexTable[IndexRank],  // pointer to index control block because ZIndexFile stores pointer to Father's ICB
          wIndexUri,
          wMasterZRF.getAllocatedBlocks(),
          wMasterZRF.getBlockExtentQuota(),
          wIndexAllocatedSize,
          wMasterZRF.getHighwaterMarking(),
          false,        // grabfreespace is set to false : not necessary for an index
          true,         // replace existing file
          false         // do not leave it open
          );
      if (wSt!=ZS_SUCCESS)
      {
        goto ErrorRepairIndexes;
      }
      pErrorLog->textLog("index file has been created");
      wCreatedIndex++;

      pErrorLog->textLog(" ......rebuilding created index file");

      wSt = wIndexFile.openIndexFile(wIndexUri,IndexRank,ZRF_Exclusive| ZRF_All);
      if (wSt!=ZS_SUCCESS)
      {
          pErrorLog->logZExceptionLast("RepairIndexes");
        pErrorLog->textLog(
            "  ****Error: Unexpected Fatal Error while opening ZIndexFile index rank <%ld> path <%s> ****",
            IndexRank,
            wIndexUri.toString());
        wIndexFile.zclose();
        goto ErrorRepairIndexes;
      }

      /*                 wSt = wIndexFile.zrebuildRawIndex(ZMFStatistics,wOutput);*/
      if (wSt!=ZS_SUCCESS)
      {
          pErrorLog->logZExceptionLast("RepairIndexes");
        pErrorLog->textLog(
            "  ****Unexpected Fatal Error while rebuilding ZIndexFile index rank <%ld> path <%s> *******\n",
            IndexRank,
            wIndexUri.toString());
        wIndexFile.zclose();
        goto ErrorRepairIndexes;
      } // ! ZS_SUCCESS

      wIndexFile.zclose();
      pErrorLog->textLog("  Index file has been rebuilt successfully");

      IndexPresence[IndexRank] = 1; // Index file is now present and does not need to be rebuilt
      wRebuiltIndex ++;

      //----------------End Create a new ZIndexFile-------------------------
      continue;
    }  // IndexUri does not exist

    //---------- wIndexUri exists-----------------
    pErrorLog->textLog("  Opening ZIndexFile");


    wSt=wIndexZRF.setPath(wIndexUri);
    if (wSt!=ZS_SUCCESS)
    {
      goto ErrorRepairIndexes;
    }
    wSt=wIndexZRF._ZRFopen(ZRF_Exclusive | ZRF_All,ZFT_ZIndexFile,true);
    if (wSt!=ZS_SUCCESS)
    {
      pErrorLog->textLog(
          "   ******Error: rank <%ld> cannot open index file <%s>.\n"
          "                  Status is <%s> Check <IndexFileDirectoryPath> parameter\n"
          "       ...continuing...",
          IndexRank,
          decode_ZStatus(wSt),
          wIndexUri.toString());
      if (pRepair)
      {
        pErrorLog->textLog(
            "                  Repare option has been chosen "
            "                  Trying to delete file and reprocess it as missing file.");

        wIndexZRF._removeFile(true,&wMasterFile.ErrorLog); // may be not necessary : to be checked

        IndexRank--;
        continue;
      }
    }

    wReservedBlock.clear();

    pErrorLog->textLog("  getting ZReservedBlock content and load ZICB\n");

    wSt=wIndexZRF.getReservedBlock(wReservedBlock,true);     // get reserved block content
    if (wSt!=ZS_SUCCESS)
    {
      pErrorLog->textLog(
          "   ******Error: Index file rank <%ld> file  <%s>.\n"
          "                  Status is <%s> Cannot get ZReservedBlock from header file. \n"
          "       ...continuing...\n",
          IndexRank,
          wIndexUri.toString(),
          decode_ZStatus(wSt));

      if (pRepair)
      {
        pErrorLog->textLog(
            "                  Repare option has been chosen \n"
            "                  Trying to delete file and reprocess it as missing file.");

        wIndexZRF._removeFile(true,&wMasterFile.ErrorLog); // may be not necessary : to be checked

        IndexRank--;
        continue;
      }
    }

    wPtrIn=wReservedBlock.Data;
    wMCB._import(wPtrIn);

    //        wSt=wZICB->_importICB(&wMasterFile.MetaDic,wReservedBlock,wMCBOwn.ICBSize,wMCBOwn.ICBOffset);  // load ZICB from reserved block content

    wZICB=new ZIndexControlBlock;
    wSt=wZICB->_import(wPtrIn);  // load ZICB from reserved block content

    if (wSt!=ZS_SUCCESS)
    {
      pErrorLog->textLog(
          "   ******Error: Index file rank <%ld> file  <%s>.\n"
          "                  Status is <%s> Cannot import ZIndexControlBlock \n"
          "       ...continuing...",
          IndexRank,
          wIndexUri.toString(),
          decode_ZStatus(wSt));

      if (pRepair)
      {
        pErrorLog->textLog(
            "                  Repare option has been chosen \n"
            "                  Trying to delete file and reprocess it as missing file.");

        wIndexZRF._removeFile(true,&wMasterFile.ErrorLog); // may be not necessary : to be checked

        IndexRank--;
        continue;
      }
    }
    pErrorLog->textLog("  checking ZICB content alignment with master file");

    size_t wRet = wMasterFile.IndexTable[IndexRank]->_exportAppend(wICBContent);
    if (memcmp(wReservedBlock.Data,wICBContent.Data,wReservedBlock.Size)==0)
    {
      pErrorLog->textLog(
          "  ZICB content is aligned with its ZSMasterFile for index rank <%ld> path <%s>\n"
          "             To rebuild ZIndexFile content (zrebuid) use Option rebuildAll",
          IndexRank,
          wIndexUri.toString());

      if (pRebuildAll)
      {
        pErrorLog->textLog(
            "  Option <RebuildAll> : healthy index file is marked for rebuilt");

        IndexPresence[IndexRank]=2;
      }

    }
    else
    {
      pErrorLog->textLog(
          "  ****Error: ZICB content is NOT aligned with its ZSMasterFile index rank <%ld> path <%s>\n"
          "             Need to be realigned and rebuilt rebuildAll",
          IndexRank,
          wIndexUri.toString());

      wCorruptZICB ++;

      IndexPresence[IndexRank]= 2;  // need to be either realigned (ZICB) and rebuilt OR destroyed . recreated and rebuilt.

      //--------------------------------------------------------------------------

      if(pRepair)
      {
        pErrorLog->textLog(
            " removing corrupted index file.");

        wIndexZRF._removeFile(true,&wMasterFile.ErrorLog);
        pErrorLog->textLog(
            " reprocessing index as missing index file");
        IndexRank --;
        continue;

      }// if pRepare

      //----------------------------------------------------------------------


    }// else


    wIndexZRF.zclose();
  }// main for loop

  wIndexProcessed= IndexRank;
  //----------------Post processing check index files to be rebuilt-------------------------

  wMasterZRF.zclose();

  wMasterFile.zopen(ZRF_Exclusive | ZRF_All);

  pErrorLog->textLog("   Rebuilding indexes to be rebuilt ");

  for (wi=0;wi<IndexPresence.size();wi ++)
  {
    if (IndexPresence[wi]==2)
    {
      pErrorLog->textLog("   Rebuilding index rank <%ld> <%s> ",
          wi,
          wMasterFile.IndexTable[wi]->IndexName.toCChar());

      /*                          wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput); */
      if (wSt!=ZS_SUCCESS)
      {
        pErrorLog->textLog(
            "  ****Error while rebuilding index rank <%ld> <%s> \n"
            "          Status is <%s>",
            wi,
            wMasterFile.IndexTable[wi]->IndexName.toCChar(),
            decode_ZStatus(wSt));
        goto ErrorRepairIndexes;
      }
      pErrorLog->textLog("      rebuilt done\n");
      wRebuiltIndex ++;
    }// if IndexPresence == 2
  }// for

EndRepairIndexes:

    pErrorLog->textLog(" Closing ZSMasterFile\n");
  wMasterZRF.zclose();

  pErrorLog->textLog(
      "_____________________________________________________________________________________________");

  pErrorLog->textLog(
      "  Report\n"
      "          Index(es) processed         %ld\n"
      "          Index file(s) missing       %ld\n"
      "          Index ZICB corrupted        %ld\n\n"
      "          Index(es) rebuilt           %ld\n",
      wIndexProcessed,
      wMissIndexFile,
      wCorruptZICB,
      wRebuiltIndex);
  pErrorLog->textLog(
      "_____________________________________________________________________________________________");

  if (wSt==ZS_SUCCESS)
    ZException.clearStack();
  return  wSt;

ErrorRepairIndexes:
    pErrorLog->textLog("  **** Index repair ended with error ***");
  goto EndRepairIndexes;
}//zrepairIndexes



/**
 * @brief ZRawMasterFile::zdowngradeZMFtoZRF downgrades a ZRawMasterFile structure to a ZRandomFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * Former dependant index files are left to themselves and not destroyed by this method.
 *
 * @param[in] pZMFPath      file path of the ZRawMasterFile to convert
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.downgradelog
 */
ZStatus zdowngradeZMFtoZRF (const uriString &pZMFPath, ZaiErrors *pErrorLog)
{


  ZStatus wSt;
  ZRawMasterFile wMasterFile;
  ZRandomFile wMasterZRF;

  uriString   wURIContent;

  ZDataBuffer wReservedBlock;

  wURIContent = pZMFPath;

  pErrorLog->textLog("_____________________________________________________________________________________________");
  pErrorLog->textLog(" starting downgrading ZRawMasterFile to ZRandomFile <%s>", pZMFPath.toString());



  wSt=wMasterFile.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));

  pErrorLog->textLog(" getting ZReservedBlock content and load ZMasterControlBlock");


  pErrorLog->textLog(
      " clearing ZMCB\n"
      "            <%ld>  defined index(es) in ZMasterControlBlock. Destroying all index files & definitions from ZMasterControlBlock\n",
      _GET_FUNCTION_NAME_,
      wMasterFile.IndexTable.size());

  wSt=wMasterFile.zclearMCB(pErrorLog);
  if (wSt!=ZS_SUCCESS) {
      pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
      wMasterFile.zclose();
      return wSt;
  }

  pErrorLog->textLog("Master Control Block cleared successfully");

  wMasterFile.zclose();

  pErrorLog->textLog("  Converting to ZRandomFile\n" );
  wSt=wMasterZRF.setPath(wURIContent);
  if (wSt!=ZS_SUCCESS) {
      pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
      return wSt;
  }

  wSt=wMasterZRF._ZRFopen (ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile);  // open ZMF using ZRandomFile routines
  if (wSt!=ZS_SUCCESS) {
      pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
      return wSt;
  }

  wMasterZRF.setFileType ( ZFT_ZRandomFile);
  wReservedBlock.clear();
  wMasterZRF.setReservedContent(wReservedBlock);

  pErrorLog->textLog("  Writing new header to file");

  wSt=wMasterZRF._writeAllFileHeader();
  if (wSt!=ZS_SUCCESS)
  {
      pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
      wMasterZRF.zclose();
      return wSt;
  }

  wMasterZRF.zclose();
  pErrorLog->textLog(" File <%s> has been successfully converted from ZRawMasterFile to ZRandomFile", wURIContent.toString());
   pErrorLog->textLog(
      "_____________________________________________________________________________________________\n");

  ZException.clearStack();

  return wSt;
}//zdowngradeZMFtoZRF

/**
 * @brief ZRawMasterFile::zupgradeZRFtoZMF upgrades a ZRandomFile structure to an empty ZRawMasterFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * @param pZRFPath file path of the ZRandomFile to upgrade
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.upgradelog
 */
ZStatus zupgradeZRFtoZMF(const uriString &pZRFPath,
//                         const uriString &pZMFPath,
                         const uriString& pDictionaryFile,
                         ZaiErrors *pErrorLog)
{

  ZStatus wSt;
  ZRawMasterFile wMasterFile;
  ZDictionaryFile wDictionary;
  ZRandomFile wMasterZRF;
  ZFile_type wFileType= ZFT_ZRawMasterFile;
  uriString   wURIContent;

  ZDataBuffer wReservedBlock;

  utf8VaryingString wBase;

  wURIContent = pZRFPath;


  pErrorLog->textLog("_____________________________________________________________________________________________");
  pErrorLog->textLog(" starting upgrading ZRandomFile <%s>", pZRFPath.toString());

  if (pDictionaryFile.isEmpty()) {
      pErrorLog->textLog(" ZRandomFile will be upgraded to a simple ZRawMasterFile with no embedded dictionary.");
  }
  else {
      pErrorLog->textLog(" ZRandomFile will be upgraded to ZMasterFile with embedded dictionary <%s>", pDictionaryFile.toString());
      wFileType = ZFT_ZMasterFile;
  }


  wSt=wMasterZRF.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));
  if (wSt!=ZS_SUCCESS) {
    pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
    wMasterZRF.zclose();
    return wSt;
  }
  if (wMasterZRF.getFileType() != ZFT_ZRandomFile) {
      pErrorLog->errorLog(" Input file is not a ZRandomFile. Its file type is  <%s>\n"
                          " Processing is interrupted. ", decode_ZFile_type( wMasterZRF.getFileType()));
      return ZS_INVTYPE;
  }

  pErrorLog->textLog(" Creating ZReservedBlock content ");

  wSt=wMasterFile.setPath(wURIContent);
  if (wSt!=ZS_SUCCESS) {
      pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
      wMasterZRF.zclose();
      return wSt;
  }

  if (!pDictionaryFile.isEmpty()) {
      pErrorLog->textLog(" Loading dictionary to embed <%s>.",pDictionaryFile.toString());
      wSt=wDictionary.loadDictionary(pDictionaryFile,pErrorLog);
      if (wSt!=ZS_SUCCESS) {
          pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
          wMasterZRF.zclose();
          wMasterFile.zclose();
          return wSt;
      }
      pErrorLog->textLog(" Embedding dictionary <%s>.",wDictionary.DicName.toString());
      /* see ZMasterFile::setDictionary */
      wMasterFile.Dictionary = new ZDictionaryFile;
      wMasterFile.Dictionary->setDictionary(wDictionary);
      uriString wURIdic = ZDictionaryFile::generateDicFileName(wMasterFile.getURIContent());

      wSt=wMasterFile.Dictionary->saveToDicFile(wURIdic);
      if (wSt!=ZS_SUCCESS) {
          pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
          wMasterZRF.zclose();
          wMasterFile.zclose();
          return wSt;
      }
      pErrorLog->infoLog(" Dictionary <%s> embedded.",wDictionary.DicName.toString());
  } // if (!pDictionaryFile.isEmpty())


  pErrorLog->textLog(" Writing ZReservedBlock content to file.");

  wMasterFile._exportAppend(wReservedBlock);
  wMasterZRF.setReservedContent(wReservedBlock);
  wMasterZRF.setFileType( wFileType);
  wSt=wMasterZRF._writeAllFileHeader();
  if (wSt!=ZS_SUCCESS) {
      pErrorLog->logZExceptionLast("zupgradeZRFtoZMF");
      wMasterZRF.zclose();
      return wSt;
  }

  wMasterZRF.zclose();
  pErrorLog->textLog("File <%s> has been successfully converted from ZRandomFile to %s.",
                     wURIContent.toString(), decode_ZFile_type( wMasterFile.getFileType()));
  pErrorLog->textLog(
      "_____________________________________________________________________________________________");

  //  if (wSt==ZS_SUCCESS)
  ZException.clearStack();

  return wSt;
}//zupgradeZRFtoZMF

ZStatus
zreorgMasterFile (const uriString& pURIRawMF,
                    long pRequestedFreeBlocks,
                    //  bool pDump,
                    ZaiErrors* pErrorLog)
{
    ZStatus wSt;
    ZMasterFile wZMF;
    long wi = 0;
    long wIndexRank=0;

    bool wgrabFreeSpaceSet = false;

    if ((wSt=wZMF.zopen(pURIRawMF,ZRF_All))!=ZS_SUCCESS)
    {  return  wSt;}


    if (!wZMF.getFCB()->GrabFreeSpace)        // activate grabFreeSpace if it has been set on
    {
        wZMF.getFCB()->GrabFreeSpace=true;
        wgrabFreeSpaceSet = true;
    }

    wZMF.zstartPMSMonitoring();

    wSt = wZMF._reorgFileInternals(pRequestedFreeBlocks,pErrorLog);

    while (wi < wZMF.IndexTable.size()) {

        wSt=wZMF.rebuildIndex(wi,&wIndexRank,pErrorLog);
        if (wSt!=ZS_SUCCESS)
        {
            //               ZException_sv = ZException; // in case of error : store the exception but continue rolling back other indexes
            ZException.addToLast(" during Index rebuild on index <%s> number <%02ld> ",
                                 wZMF.IndexTable[wi]->IndexName.toCChar(),
                                 wi);

            pErrorLog->logZExceptionLast("zreorgMasterFile");
            return wSt;
        }

        wi++;
    }

    wZMF.zendPMSMonitoring ();
    pErrorLog->textLog(
        " ----------End of ZRawMasterFile reorganization process-------------\n");

    wZMF.zreportPMSMonitoring(pErrorLog);

    if (ZException.getLastStatus()!=ZS_SUCCESS)
    {
        //             ZException=ZException_sv;
        goto error_zreorgMasterFile;
    }

//    ZException.getLastStatus() = ZS_SUCCESS;

end_zreorgMasterFile:

    if (wgrabFreeSpaceSet)        // restore grabFreeSpace if it was off and has been set on
    {
        wZMF.getFCB()->GrabFreeSpace=false;
    }
    wZMF.zclose ();
    return  wSt;

error_zreorgMasterFile:
    pErrorLog->logZExceptionLast("zreorgRawMasterFile");
    goto end_zreorgMasterFile;

}

/** @ */ // ZMFUtilities
