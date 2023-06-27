
#include <zindexedfile/zrawmasterfileutils.h>

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
 * @return an utf8String with the appropriate ZIndexFile root name
 */
utf8VaryingString generateIndexRootName(const utf8String &pMasterRootName,
                                        const utf8String &pIndexName)
{
  utf8String wIndexRootName;
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

utf8VaryingString generateIndexBaseName(const utf8String &pMasterRootName,
    const utf8String &pIndexName)
{
  utf8String wIndexRootName = generateIndexRootName(pMasterRootName,pIndexName);
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
  utf8String wMasterRoot;
  utf8String wMasterExt;


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

  utf8String wM;

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
                        FILE* pOutput)
{

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

  FILE* wOutput=nullptr;
  bool FOutput=false;
  utfdescString wBase;

  ZArray<char> IndexPresence ; // 0 : Index to be deleted     1 : Index present but not to be rebuilt    2 : Index to be built or rebuilt

  long wIndexProcessed = 0,wMissIndexFile = 0, wCorruptZICB = 0, wCreatedIndex = 0, wRebuiltIndex = 0 ;

  ZMasterControlBlock wMCB(wMasterFile);
  const unsigned char* wPtrIn=nullptr;


  wURIContent = pZMFPath;

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

  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  fprintf (wOutput,"%s>> starting repairing indexes for ZSMasterFile <%s>  \n"
      ,
      _GET_FUNCTION_NAME_,
      pZMFPath);


  wSt=wMasterZRF.setPath(wURIContent);
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }
  wSt=wMasterZRF._ZRFopen (ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile,true);  // open ZMF using ZRandomFile routines
  if (wSt!=ZS_SUCCESS)
    goto ErrorRepairIndexes;
  //  Must be a ZFT_ZRawMasterFile

  if (wMasterZRF.getFileType()!=ZFT_ZRawMasterFile)
  {

    fprintf (wOutput,"%s>> **** Fatal error : file is not of mandatory type ZFT_ZSMasterFile but is <%s> ******\n",
        _GET_FUNCTION_NAME_,
        decode_ZFile_type( wMasterZRF.getFileType()));
    goto ErrorRepairIndexes;
  }

  //wMasterFile.ZMFURI = wMasterZRF.getURIContent();     // align uris: ZMFURI is getting redundant. Only using ZRandomFile URIContent

  fprintf (wOutput,"%s>> getting ZReservedBlock content and load ZMasterControlBlock\n",
      _GET_FUNCTION_NAME_);

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

  fprintf (wOutput,
      "%s>> existing ZSMasterFile index(es)\n"
      "            <%ld>  defined index(es) in ZMasterControlBlock\n",
      _GET_FUNCTION_NAME_,
      wMasterFile.IndexTable.size());

  wMasterFile.ZMCBreport();

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
    fprintf (wOutput,
        "%s>>   .....rank <%ld> processing index file <%s> \n",
        _GET_FUNCTION_NAME_,
        IndexRank,
        wIndexUri.toString());
    if (wIndexUri.exists())
      fprintf(wOutput,

          "%s>>       Index file has been found\n",
          _GET_FUNCTION_NAME_);
    else
    {
      fprintf(wOutput,
          "\n%s>>  ****Error Index file <%s> is missing ****\n"
          "                        Index will be created then rebuilt\n\n",
          _GET_FUNCTION_NAME_,
          wIndexUri.toString());

      wMissIndexFile++;

      IndexPresence[IndexRank]= 1;
      if (!pRepair)
        continue;
      //---------------Create a new ZIndexFile-------------------------


      fprintf (wOutput,
          "%s>> creating index file\n",
          _GET_FUNCTION_NAME_);

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
      fprintf (wOutput,
          "%s>> index file has been created\n",
          _GET_FUNCTION_NAME_);
      wCreatedIndex++;

      fprintf (wOutput,
          "%s>> ......rebuilding created index file\n",
          _GET_FUNCTION_NAME_);

      wSt = wIndexFile.openIndexFile(wIndexUri,IndexRank,ZRF_Exclusive| ZRF_All);
      if (wSt!=ZS_SUCCESS)
      {
        fprintf (wOutput,
            "%s>>  ****Error: Unexpected Fatal Error while opening ZIndexFile index rank <%ld> path <%s> ****\n",
            _GET_FUNCTION_NAME_,
            IndexRank,
            wIndexUri.toString());
        wIndexFile.zclose();
        goto ErrorRepairIndexes;
      }

      /*                 wSt = wIndexFile.zrebuildRawIndex(ZMFStatistics,wOutput);*/
      if (wSt!=ZS_SUCCESS)
      {
        fprintf (wOutput,
            "%s>>  ****Unexpected Fatal Error while rebuilding ZIndexFile index rank <%ld> path <%s> *******\n",
            _GET_FUNCTION_NAME_,
            IndexRank,
            wIndexUri.toString());
        wIndexFile.zclose();
        goto ErrorRepairIndexes;
      } // ! ZS_SUCCESS

      wIndexFile.zclose();
      fprintf (wOutput,
          "%s>>  Index file has been rebuilt successfully\n",
          _GET_FUNCTION_NAME_);

      IndexPresence[IndexRank] = 1; // Index file is now present and does not need to be rebuilt
      wRebuiltIndex ++;

      //----------------End Create a new ZIndexFile-------------------------
      continue;
    }  // IndexUri does not exist

    //---------- wIndexUri exists-----------------
    fprintf (wOutput,"%s>>  Opening ZIndexFile\n",
        _GET_FUNCTION_NAME_);


    wSt=wIndexZRF.setPath(wIndexUri);
    if (wSt!=ZS_SUCCESS)
    {
      goto ErrorRepairIndexes;
    }
    wSt=wIndexZRF._ZRFopen(ZRF_Exclusive | ZRF_All,ZFT_ZIndexFile,true);
    if (wSt!=ZS_SUCCESS)
    {
      fprintf (wOutput,
          "%s>>   ******Error: rank <%ld> cannot open index file <%s>.\n"
          "                  Status is <%s> Check <IndexFileDirectoryPath> parameter\n"
          "       ...continuing...\n",
          _GET_FUNCTION_NAME_,
          IndexRank,
          decode_ZStatus(wSt),
          wIndexUri.toString());
      if (pRepair)
      {
        fprintf (wOutput,
            "                  Repare option has been chosen "
            "                  Trying to delete file and reprocess it as missing file.\n");

        wIndexZRF._removeFile(true,&wMasterFile.ErrorLog); // may be not necessary : to be checked

        IndexRank--;
        continue;
      }
    }

    wReservedBlock.clear();

    fprintf (wOutput,"%s>>  getting ZReservedBlock content and load ZICB\n",
        _GET_FUNCTION_NAME_);

    wSt=wIndexZRF.getReservedBlock(wReservedBlock,true);     // get reserved block content
    if (wSt!=ZS_SUCCESS)
    {
      fprintf (wOutput,
          "%s>>   ******Error: Index file rank <%ld> file  <%s>.\n"
          "                  Status is <%s> Cannot get ZReservedBlock from header file. \n"
          "       ...continuing...\n",
          _GET_FUNCTION_NAME_,

          IndexRank,
          wIndexUri.toString(),
          decode_ZStatus(wSt));

      if (pRepair)
      {
        fprintf (wOutput,
            "                  Repare option has been chosen \n"
            "                  Trying to delete file and reprocess it as missing file.\n");

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
      fprintf (wOutput,
          "%s>>   ******Error: Index file rank <%ld> file  <%s>.\n"
          "                  Status is <%s> Cannot import ZIndexControlBlock \n"
          "       ...continuing...\n",
          _GET_FUNCTION_NAME_,
          IndexRank,
          wIndexUri.toString(),
          decode_ZStatus(wSt));

      if (pRepair)
      {
        fprintf (wOutput,
            "                  Repare option has been chosen \n"
            "                  Trying to delete file and reprocess it as missing file.\n");

        wIndexZRF._removeFile(true,&wMasterFile.ErrorLog); // may be not necessary : to be checked

        IndexRank--;
        continue;
      }
    }
    fprintf (wOutput,"%s>>  checking ZICB content alignment with ZSMasterFile\n",
        _GET_FUNCTION_NAME_);

    size_t wRet = wMasterFile.IndexTable[IndexRank]->_exportAppend(wICBContent);
    if (memcmp(wReservedBlock.Data,wICBContent.Data,wReservedBlock.Size)==0)
    {
      fprintf (wOutput,
          "%s>>  ZICB content is aligned with its ZSMasterFile for index rank <%ld> path <%s>\n"
          "             To rebuild ZIndexFile content (zrebuid) use Option rebuildAll\n",
          _GET_FUNCTION_NAME_,
          IndexRank,
          wIndexUri.toString());

      if (pRebuildAll)
      {
        fprintf (wOutput,
            "%s>>  Option <RebuildAll> : healthy index file is marked for rebuilt\n",
            _GET_FUNCTION_NAME_);

        IndexPresence[IndexRank]=2;
      }

    }
    else
    {
      fprintf (wOutput,
          "%s>>  ****Error: ZICB content is NOT aligned with its ZSMasterFile index rank <%ld> path <%s>\n"
          "             Need to be realigned and rebuilt rebuildAll\n",
          _GET_FUNCTION_NAME_,
          IndexRank,
          wIndexUri.toString());

      wCorruptZICB ++;

      IndexPresence[IndexRank]= 2;  // need to be either realigned (ZICB) and rebuilt OR destroyed . recreated and rebuilt.

      //--------------------------------------------------------------------------

      if(pRepair)
      {
        fprintf (wOutput,
            "%s>> removing corrupted index file\n",
            _GET_FUNCTION_NAME_);

        wIndexZRF._removeFile(true,&wMasterFile.ErrorLog);
        fprintf (wOutput,
            "%s>> reprocessing index as missing index file\n",
            _GET_FUNCTION_NAME_);
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

  fprintf (wOutput,"%s>>   Rebuilding indexes to be rebuilt \n",
      _GET_FUNCTION_NAME_);

  for (wi=0;wi<IndexPresence.size();wi ++)
  {
    if (IndexPresence[wi]==2)
    {
      fprintf (wOutput,"%s>>   Rebuilding index rank <%ld> <%s> \n",
          _GET_FUNCTION_NAME_,
          wi,
          wMasterFile.IndexTable[wi]->IndexName.toCChar());

      /*                          wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput); */
      if (wSt!=ZS_SUCCESS)
      {
        fprintf (wOutput,
            "%s>>   ****Error while rebuilding index rank <%ld> <%s> \n"
            "          Status is <%s>\n",
            _GET_FUNCTION_NAME_,
            wi,
            wMasterFile.IndexTable[wi]->IndexName.toCChar(),
            decode_ZStatus(wSt));
        goto ErrorRepairIndexes;
      }
      fprintf (wOutput,"      rebuilt done\n");
      wRebuiltIndex ++;
    }// if IndexPresence == 2
  }// for

EndRepairIndexes:

  fprintf (wOutput,"%s>>  Closing ZSMasterFile\n",
      _GET_FUNCTION_NAME_);
  wMasterZRF.zclose();

  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");

  fprintf (wOutput,
      "%s>>  Report\n"
      "          Index(es) processed         %ld\n"
      "          Index file(s) missing       %ld\n"
      "          Index ZICB corrupted        %ld\n\n"
      "          Index(es) rebuilt           %ld\n",
      _GET_FUNCTION_NAME_,
      wIndexProcessed,
      wMissIndexFile,
      wCorruptZICB,
      wRebuiltIndex);
  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  ZException.printUserMessage(wOutput);
  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  if (wSt==ZS_SUCCESS)
    ZException.clearStack();
  if (FOutput)
    fclose(wOutput);
  return  wSt;

ErrorRepairIndexes:
  fprintf (wOutput,"%s>>  **** Index repair ended with error ***\n",
      _GET_FUNCTION_NAME_);
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
void zdowngradeZMFtoZRF (const uriString &pZMFPath, FILE* pOutput)
{


  ZStatus wSt;
  ZRawMasterFile wMasterFile;
  ZRandomFile wMasterZRF;

  uriString   wURIContent;

  ZDataBuffer wReservedBlock;

  FILE* wOutput=nullptr;
  bool FOutput=false;
  utfdescString wBase;

  wURIContent = pZMFPath;

  wOutput=pOutput;
  if (pOutput==nullptr)
  {
    //       utfdescString wDInfo;
    wBase=wURIContent.getBasename().toCChar();
    wBase+=(const utf8_t*)".downgradelog";
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

  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  fprintf (wOutput,"%s>> starting downgrading ZRawMasterFile to ZRandomFile file path <%s>  \n",
      _GET_FUNCTION_NAME_,
      pZMFPath.toString());


  wSt=wMasterFile.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));

  fprintf (wOutput,"%s>> getting ZReservedBlock content and load ZMasterControlBlock\n",
      _GET_FUNCTION_NAME_);


  fprintf (wOutput,
      "%s>> clearing ZMCB\n"
      "            <%ld>  defined index(es) in ZMasterControlBlock. Destroying all index files & definitions from ZMasterControlBlock\n",
      _GET_FUNCTION_NAME_,
      wMasterFile.IndexTable.size());

  wSt=wMasterFile.zclearMCB(wOutput);
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }

  fprintf (wOutput,"%s>>  ZMCB cleared successfully\n",
      _GET_FUNCTION_NAME_);

  wMasterFile.zclose();

  fprintf (wOutput,"%s>>  Converting to ZRandomFile\n",
      _GET_FUNCTION_NAME_);
  wSt=wMasterZRF.setPath(wURIContent);
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }

  wSt=wMasterZRF._ZRFopen (ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile);  // open ZMF using ZRandomFile routines
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }

  wMasterZRF.setFileType ( ZFT_ZRandomFile);
  wReservedBlock.clear();
  wMasterZRF.setReservedContent(wReservedBlock);

  fprintf (wOutput,"%s>>  Writing new header to file\n",
      _GET_FUNCTION_NAME_);

  wSt=wMasterZRF._writeAllFileHeader();
  if (wSt!=ZS_SUCCESS)
  {
    ZException.exit_abort();
  }

  wMasterZRF.zclose();
  fprintf (wOutput,"%s>>  File <%s> has been successfully converted from ZRawMasterFile to ZRandomFile\n",
      _GET_FUNCTION_NAME_,
      wURIContent.toString());
  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  ZException.printUserMessage(wOutput);
  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  //   if (wSt==ZS_SUCCESS)
  ZException.clearStack();
  if (FOutput)
    fclose(wOutput);
  return ;
}//zdowngradeZMFtoZRF

/**
 * @brief ZRawMasterFile::zupgradeZRFtoZMF upgrades a ZRandomFile structure to an empty ZRawMasterFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * @param pZRFPath file path of the ZRandomFile to upgrade
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.upgradelog
 */
void zupgradeZRFtoZMF (const uriString& pZRFPath,FILE* pOutput)
{


  ZStatus wSt;
  ZRawMasterFile wMasterFile;
  ZRandomFile wMasterZRF;

  uriString   wURIContent;

  ZDataBuffer wReservedBlock;

  FILE* wOutput=nullptr;
  bool FOutput=false;
  utfdescString wBase;

  wURIContent = pZRFPath;

  wOutput=pOutput;
  if (pOutput==nullptr)
  {
    //       utfdescString wDInfo;
    wBase=wURIContent.getBasename().toCChar();
    wBase+=".upgradelog";
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

  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  fprintf (wOutput,"%s>> starting upgrading ZRandomFile to ZRawMasterFile file path <%s>  \n",
      _GET_FUNCTION_NAME_,
      pZRFPath.toString());


  wSt=wMasterZRF.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));
  if (wSt!=ZS_SUCCESS)
  {
    ZException.exit_abort();
  }
  fprintf (wOutput,"%s>> creating ZReservedBlock content and write ZMasterControlBlock\n",
      _GET_FUNCTION_NAME_);

  wSt=wMasterFile.setPath(wURIContent);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.exit_abort();
  }
  wMasterFile._exportAppend(wReservedBlock);
  wMasterZRF.setReservedContent(wReservedBlock);
  wMasterZRF.setFileType( ZFT_ZMasterFile);
  wSt=wMasterZRF._writeAllFileHeader();
  if (wSt!=ZS_SUCCESS)
  {
    ZException.exit_abort();
  }

  wMasterZRF.zclose();
  fprintf (wOutput,"%s>>  File <%s> has been successfully converted from ZRandomFile to ZRawMasterFile.\n",
      _GET_FUNCTION_NAME_,
      wURIContent.toString());
  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  ZException.printUserMessage(wOutput);
  fprintf (wOutput,
      "_____________________________________________________________________________________________\n");
  //  if (wSt==ZS_SUCCESS)
  ZException.clearStack();
  if (FOutput)
    fclose(wOutput);
  return ;
}//zupgradeZRFtoZMF





/** @ */ // ZMFUtilities
