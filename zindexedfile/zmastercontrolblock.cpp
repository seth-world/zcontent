#ifndef ZMASTERCONTROLBLOCK_CPP
#define ZMASTERCONTROLBLOCK_CPP

#include "zmastercontrolblock.h"
#include <zindexedfile/zsjournalcontrolblock.h>
#include <zindexedfile/zmfdictionary.h>
#include <zxml/zxmlprimitives.h>

#include <zindexedfile/zindexdata.h>
#include <ztoolset/zarray.h>

#include <zindexedfile/zrawindexfile.h>


using namespace zbs;

ZMasterControlBlock::ZMasterControlBlock(ZRawMasterFile* pMasterFile)
{
  clear();
  RawMasterFile=pMasterFile;
}


// ----------ZMasterControlBlock ----------
//

ZMasterControlBlock::~ZMasterControlBlock(void)
{
  /*    if (MDicCheckSum!=nullptr)   // No MDicCheckSum object is deleted while deleting MetaDic object
                delete MDicCheckSum;*/
  if (ZJCB!=nullptr)
    delete ZJCB;
  if (Dictionary!=nullptr)
    delete Dictionary;
  while (IndexTable.size()>0)
  {
    IndexTable.pop();
  }
  return;
}


long ZMasterControlBlock::popIndex(void)
{
  if (IndexTable.pop()<0)
    return -1;

  return IndexTable.size();
}


void ZMasterControlBlock::clear(void)
{

  HistoryOn=false;

  delete Dictionary;
//  DictionaryName.clear();
  DictionaryPath.clear();
  while (IndexTable.size() > 0)
    popIndex();
  IndexTable.clear();
  IndexFilePath.clear();
  return;
}


ZMCB_Export& ZMCB_Export::_copyFrom(const ZMCB_Export& pIn)
{
  StartSign=pIn.StartSign;
  BlockId=pIn.BlockId;
  ZMFVersion=pIn.ZMFVersion;
  MCBSize=pIn.MCBSize;
//  MDicOffset=pIn.MDicOffset;
//  MDicSize=pIn.MDicSize;
//  IndexCount=pIn.IndexCount;
  ICBOffset=pIn.ICBOffset;
  ICBOffset=pIn.ICBOffset;
  ICBSize=pIn.ICBSize;
  JCBOffset=pIn.JCBOffset;
  JCBSize=pIn.JCBSize;
  HistoryOn=pIn.HistoryOn;
  return *this;
}

/* export order ;
 *
 ZMCB_Export
 - index file path
 - dictionary Path
 - dictionary Name

 IndexCount
 - index table :
    ZICBs
 - JCB


 */

size_t ZMCB_Export::_exportAppend(ZDataBuffer& pReturn)
{
  StartSign = cst_ZBLOCKSTART;
  BlockId   = ZBID_MCB;
  ZMFVersion =  __ZMF_VERSION__;

  serialize();

  pReturn.appendData(this,sizeof(ZMCB_Export));
  return sizeof(ZMCB_Export);
}

ZMCB_Export& ZMCB_Export::setFromPtr(const unsigned char*& pPtrIn)
{
  memmove (this,pPtrIn,sizeof(ZMCB_Export));
  pPtrIn += sizeof(ZMCB_Export);
  return *this;
}
ZMCB_Export& ZMCB_Export::_import( const unsigned char*& pPtrIn)
{
  memmove (this,pPtrIn,sizeof(ZMCB_Export));
  deserialize();
  pPtrIn += sizeof(ZMCB_Export);

  return *this;
} // _import


void ZMCB_Export::_convert()
{
  if (!is_little_endian())
    return ;

  EndianCheck=reverseByteOrder_Conditional<uint16_t>(EndianCheck);

//  StartSign=reverseByteOrder_Conditional<uint32_t>(StartSign);
  ZMFVersion=reverseByteOrder_Conditional<unsigned long>(ZMFVersion);
  MCBSize=reverseByteOrder_Conditional<uint32_t>(MCBSize);
//  MDicOffset=reverseByteOrder_Conditional<uint32_t>(MDicOffset);
//  MDicSize=reverseByteOrder_Conditional<int32_t>(MDicSize);
  ICBOffset=reverseByteOrder_Conditional<uint32_t>(ICBOffset);
  ICBSize=reverseByteOrder_Conditional<uint32_t>(ICBSize);
  JCBOffset=reverseByteOrder_Conditional<uint32_t>(JCBOffset);
  JCBSize=reverseByteOrder_Conditional<int32_t>(JCBSize);
//  IndexCount=reverseByteOrder_Conditional<uint32_t>(IndexCount);
  return ;
}

void
ZMCB_Export::serialize()
{
  if (!is_little_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZSMCBOwnData_Export::serialize-W-ALRDY Master Control Block already serialized. \n");
    return;
  }
  _convert();
}

void
ZMCB_Export::deserialize()
{
  if (!is_little_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZSMCBOwnData_Export::deserialize-W-ALRDY Master Control Block already deserialized. \n");
    return;
  }
  _convert();
}

/**
 * @brief ZSMCBOwnData::_export
 *      Exports Master Control Block own data
 *      IndexFilePath, a fixed length uriString, is stored using its varying Universal Format (VUniversal)
 *      -> uint16_t size of string content WITHOUT ending '\0' char
 *      -> string content
 *      IndexFilePath is then added under this format at the end of MCB own data export format
 *
 *      Size of exported MCB is then sizeof(ZZSMCBOwnData_Export)+size of VUFormat of string
 *      This size is stored in MCBSize.
 *      Nota Bene : MDicOffset is equal to MCBSize.
 *
 * @param pZDBExport
 * @return
 */
uriString ZMasterControlBlock::getURIDictionary() {
  if (Dictionary==nullptr)
    return "<no dictionary>";

  return Dictionary->URIDictionary;
}

/*
<zmastercontrolblock>
    <indexfilepath> </indexfilepath> <!-- Directory path for index files. If empty, then directory path of main content file is taken -->
    <history> </history>
    <indexcount> </indexcount>
</zmastercontrolblock>
*/
utf8String
ZMasterControlBlock::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("zmastercontrolblock",pLevel);
  wLevel++;

//  wReturn += ZSMCBOwnData::toXml(wLevel);

  /*--------Master control block own data  ------------------*/

  wReturn+=fmtXMLchar("indexfilepath",  IndexFilePath.toCChar(),wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," Directory path for index files. If empty, then directory path of main content file is taken");

  wReturn+=fmtXMLbool("historyon",  HistoryOn,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," Reserved for future use : will allow historization of journalling events if set to true.");

  wReturn+=fmtXMLuint("indexcount",  IndexTable.count(),wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," FYI: Number of index keys for current file.");

  wReturn += IndexTable.toXml(pLevel,pComment);
  /* check if following is necessary in an xml definition */

  /*
  wReturn+=fmtXMLuint("mcbsize",  MCBSize,wLevel);
  wReturn+=fmtXMLuint("mdicoffset",  MDicOffset,wLevel);
  wReturn+=fmtXMLuint("mdicsize",  MDicSize,wLevel);


  wReturn+=fmtXMLuint64("icboffset",ICBOffset,wLevel);
  wReturn+=fmtXMLuint64("icbsize",ICBSize,wLevel);

  wReturn+=fmtXMLuint64("jcboffset",JCBOffset,wLevel);
  wReturn+=fmtXMLuint64("jcbsize",JCBSize,wLevel);
  */

  /*--------Index control block table ZSIndexControlTable ------------------*/


  /*--------Index file table ZIndexTable  ------------------*/

  /*--------Journal control block ------------*/
  if (pComment)
    wReturn +=fmtXMLcomment("optional zjournalcontrolblock : if exists then journaling has been defined for this file : if not no journaling",wLevel);
  if (ZJCB!=nullptr)
    wReturn += ZJCB->toXml(wLevel);

  /*--------dictionary ------------*/
  if (Dictionary!=nullptr)
    wReturn += static_cast<ZMFDictionary*>(Dictionary)->toXml(wLevel,pComment);

  wReturn += fmtXMLendnode("zmastercontrolblock",pLevel);
  return wReturn;
} // ZSMasterControlBlock::toXml



ZStatus ZMasterControlBlock::fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utf8String wValue;
  bool wBool;
  ZStatus wSt = pRootNode->getChildByName((zxmlNode *&) wRootNode, "zmastercontrolblock");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZSMCBOwnData::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "zmastercontrolblock",
        decode_ZStatus(wSt));
    return wSt;
  }
  if (XMLgetChildText(wRootNode, "indexfilepath", wValue, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay empty.",
        "indexfilepath");
    IndexFilePath.clear();
  }
  else
    IndexFilePath=wValue.toCChar();
  if (XMLgetChildText(wRootNode, "indexfilepath", wValue, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "offsetfcb");
  }
  else
    IndexFilePath=wValue.toCChar();

  if (XMLgetChildBool(wRootNode, "historyon", wBool, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to <false>.\n",
        "historyon");
  }
  else
    HistoryOn=wBool;

  uint32_t wIndexCount;
  if (XMLgetChildUInt32(wRootNode, "indexcount", wIndexCount, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "indexcount");
  }

  /*
  if (XMLgetChildUInt32(wRootNode, "mcbsize", MCBSize, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "mdicoffset");
  }
  if (XMLgetChildUInt32(wRootNode, "mdicoffset", MDicOffset, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "mdicoffset");
  }
  if (XMLgetChildInt32(wRootNode, "mdicsize", MDicSize, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "mdicsize");
  }

  if (XMLgetChildUInt32(wRootNode, "icboffset", ICBOffset, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "icboffset");
  }
  if (XMLgetChildUInt32(wRootNode, "icbsize", ICBSize, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "icbsize");
  }

  if (XMLgetChildUInt32(wRootNode, "jcboffset", JCBOffset, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "jcboffset");
  }
  if (XMLgetChildInt32(wRootNode, "jcbsize", JCBSize, pErrorlog)< 0) {
    fprintf(stderr,
        "ZSMCBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "jcbsize");
  }
  */


  return ZS_SUCCESS;
}//zmastercontrolblock::fromXml



/**
 * @brief ZSMasterFileControlBlock::_exportMCB   exports ZSMasterFileControlBlock content to a flat ZDataBuffer.
 * Updates values (offset and size) for ZJournalingControlBlock if ever journaling exists.
 * Both values are set to -1 if journaling does not exist.
 * @return a ZDataBuffer containing the flat raw data exported from ZMCB
 */


/*

  Header Reserved components for Master file  (Master Control Block export)

  Master Control Block    ZBID_MCB
       |  MCB Own Data : offsets to other "control blocks" and dictionary are here
       |
       |  IndexFilePath (UVF format)
       |  DictionaryName (UVF format)
       |  DictionaryPath (UVF format)
       |  IndexFilePath (UVF format)

    +------> if index count greater than 0
    |
    |  Index Control Blocks ZBID_ICB
    |      | ICB 0
    |      | IndexName (UVF format)
    |      | URIIndex (UVF format)
    |
    |      | ICB 1
    |      | IndexName (UVF format)
    |      | URIIndex (UVF format)
    |      ...
    |      | ICB n
    |      | IndexName (UVF format)
    |      | URIIndex (UVF format)
    +-------
    cst_ZBLOCKEND (0xFCFCFCFC)


  Journal Control Block ZBID_JCB


*/
ssize_t ZMasterControlBlock::_exportAppend(ZDataBuffer &pMCBContent)
{
  ZDataBuffer wJCB;
  ZDataBuffer wICB;

  ZMCB_Export wMCBExp;
  ZDataBuffer wZDBIndexes;

  /* NB: before is ZMCB_Export (computed later) */

  size_t wRet;

  if (IndexTable.count() > 0){
  /* first export indexes */
  for (long wi = 0;wi < IndexTable.size(); wi++) {
    wRet=IndexTable[wi]->_exportAppend(wZDBIndexes);
  }
  wMCBExp.ICBSize=(uint32_t)wZDBIndexes.Size;
  }
  else {
    wMCBExp.ICBOffset=0;
    wMCBExp.ICBSize=0;
  }
  _exportAtomic<uint32_t>(cst_ZBLOCKEND,wZDBIndexes);

  /* second export journalling : if journalling exists */
  ZDataBuffer wZDBJCB ;
  if (ZJCB!=nullptr) {
    ZJCB->_exportJCB(wZDBJCB);
    _exportAtomic<uint32_t>(cst_ZBLOCKEND,wZDBJCB);
    wMCBExp.JCBSize = (uint32_t)wZDBJCB.Size;
  }
  else {
    wMCBExp.JCBSize = 0;
    wMCBExp.JCBOffset = 0;
  }

//  wMCBExp.IndexCount=uint32_t(IndexTable.count());

  wMCBExp.JCBOffset=uint32_t(wMCBExp.ICBOffset + wMCBExp.ICBSize);

  wMCBExp.MCBSize=uint32_t(sizeof(ZMCB_Export)
                             + IndexFilePath._getexportUVFSize()
                             + DictionaryPath._getexportUVFSize());
//                             + DictionaryName._getexportUVFSize() );
  wMCBExp.ICBOffset=wMCBExp.MCBSize;

  wMCBExp.HistoryOn=HistoryOn;

  wRet=wMCBExp._exportAppend(pMCBContent);

  //DictionaryName._exportAppendUVF(pMCBContent);
  DictionaryPath._exportAppendUVF(pMCBContent);
  IndexFilePath._exportAppendUVF(pMCBContent);
  /*
  size_t      wMCBSize = wMCB.Size;

  ZMCB_Export* wMCB_Fin = (ZMCB_Export*)wMCB.Data;

  wMCB_Fin->MCBSize = reverseByteOrder_Conditional<uint32_t>(uint32_t(wMCB.Size));
  wMCB_Fin->ICBOffset = wMCB_Fin->MCBSize;

  wMCB_Fin->ICBSize = reverseByteOrder_Conditional<uint32_t>(uint32_t(wZDBIndexes.Size));
  wMCB_Fin->JCBOffset = reverseByteOrder_Conditional<uint32_t>(uint32_t (wZDBIndexes.Size+wMCB.Size));
*/
  pMCBContent.appendData(wZDBIndexes);

  pMCBContent.appendData(wZDBJCB);

  return pMCBContent.Size;
}// _exportMCBAppend

#ifdef __COMMENT__

/**
 * @brief ZSMasterFileControlBlock::_importMCB imports (rebuild) a ZSMasterFileControlBlock from a ZDataBuffer containing flat raw data to import
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterControlBlock::_importMCB(ZDataBuffer& pBuffer)
{
  ZStatus wSt;
  //long wIndexCountsv ;
  ZArray<ZSIndexControlBlock*> wICBTable;
  unsigned char* wPtrIn=pBuffer.Data;

  wSt=ZSMCBOwnData::_import(wPtrIn);  // wPtrIn is updated
  if (wSt!=ZS_SUCCESS)
    return  wSt;

  unsigned char* wPtrEnd= pBuffer.Data;
  if (JCBOffset > 0)
    wPtrEnd += JCBOffset;
  else
    wPtrEnd += pBuffer.Size;

  //-----------Import defined IndexControlBlocks for the file

  //    wIndexCountsv = IndexCount;

  long wi =0;

  ZSIndexControlBlock wICB;




  while ((wi < IndexTable.size())&& (wi < IndexCount))
  {
    wSt=wICB._import(wPtrIn);


    IndexTable[wi++]->ZSIndexControlBlock::_import(wPtrIn);
  }




  size_t wInSize;

  ZSIndexControlBlock* wICB=nullptr;

  for (size_t wi = 0;wi < IndexCount; wi++)  // IndexCount has been updated from memmove
  {
    wICB= new ZSIndexControlBlock;

    wSt=wICB->_import(wPtrIn);
    wICBTable.push(wICB);
    if (wSt!=ZS_SUCCESS)
      return  wSt;

    if (wPtrIn >= wPtrEnd)   // JCBOffset may be negative
      break;
  }// for




  if (Dictionary!=nullptr)
    delete Dictionary;
  Dictionary=nullptr;
  if (MDicSize > 0)
    {
    Dictionary = new ZMFDictionary;
    wInSize = Dictionary->_import(wPtrIn);
    }


  if (ZJCB!=nullptr)
    delete ZJCB;
  ZJCB=nullptr;
  if (JCBSize > 0)
    {
    ZJCB = new ZSJournalControlBlock;
    wInSize = ZJCB->_import(wPtrIn);
    }


  //------------Import Journaling Control block if defined must be done elsewhere--------



  return  ZS_SUCCESS;
}//_importMCB

#endif // __COMMENT__

/* this routine is called only during open file session
 *
 */

ZStatus
ZMasterControlBlock::_import(const unsigned char*& pPtrIn) {

  ZStatus wSt=ZS_SUCCESS;

  ZMCB_Export wMCBExp;
  wMCBExp._import(pPtrIn);

//  DictionaryName._importUVF(pPtrIn);
  DictionaryPath._importUVF(pPtrIn);
  IndexFilePath._importUVF(pPtrIn);

  IndexTable.clear();

  ZRawIndexFile* wIFile= nullptr;
  uint32_t wIRank=0;
  uint32_t* wEndMark=(uint32_t*)pPtrIn;

  while((wSt==ZS_SUCCESS) && (*wEndMark !=  cst_ZBLOCKEND)) {
    wIFile=new ZRawIndexFile(RawMasterFile);
    wSt=wIFile->_import(pPtrIn);
    IndexTable.push(wIFile);
    wIRank++;
    wEndMark=(uint32_t*)pPtrIn;
  }// while
  if ((wSt!=ZS_EOF) && (wSt!=ZS_SUCCESS))
    return wSt;

  wSt=ZS_SUCCESS;
  if (ZJCB!=nullptr)
    delete ZJCB;

  ZJCB=nullptr;

  if (wMCBExp.JCBSize!=0) {
    ZJCB = new ZJournalControlBlock;
    ZJCB->_import(pPtrIn);
  }

  return ZS_SUCCESS;

#ifdef __COMMENT__
  //long wIndexCountsv ;
  //  ZArray<ZIndexControlBlock*> wICBTable;
  /* IndexPresence values meaning
   * 0 : Index to be deleted
   * 1 : Index present but not to be rebuilt
   * 2 : Index to be built or rebuilt -
   * 3 : Index created (therefore rebuilt)
   * 4 : Index errored
   */

/*  wSt=ZMCBExport_import(pPtrIn);  // wPtrIn is updated
  if (wSt!=ZS_SUCCESS)
    return  wSt;
*/

  //-----------Import defined IndexControlBlocks for the file

  //    wIndexCountsv = IndexCount;

  long wi =0;

  /* import all index control blocks at once */

  while (wi < wMCBExp.IndexCount)
    {
      pIndexPresence.push(ZPRES_Unchanged);
      ZRawIndexFile* wZRIF= new ZRawIndexFile((ZRawMasterFile *)pMaster);
      wSt=wZRIF->ZIndexControlBlock::_import(pPtrIn);
      if (wSt!=ZS_SUCCESS)
        {
        fprintf (stderr,"ZSMasterControlBlock::_import-E-CANTIMPORT Cannot import ZSIndexControlBlock.\n");
        delete wZRIF;
        return wSt;
        }
      IndexTable.push(wZRIF);
      wi++;
    }

  ssize_t wInSize=0;

#ifdef __OLD_COMMENT__
  if (Dictionary!=nullptr)
          delete Dictionary;
  Dictionary=nullptr;
  if ((MDicSize > 0) && (MDicOffset > 0))
    {
    Dictionary = new ZMFDictionary;
    wPtrDic=wPtrIn + MDicOffset ;
    wSt = Dictionary->_import(wPtrDic);
    if (wSt!=ZS_SUCCESS)
      return wSt;
*/
    /* Match Index rank with key dictionary rank and update KeyDic within index file
     * NB: if no dictionary present (RawMasterFile) then KeyDic will remain nullptr
     */
    for (long wi=0;wi < IndexTable.count();wi++ )
    {
      ZKeyDictionary* wKD=Dictionary->searchKeyCase(IndexTable[wi]->IndexName);
      if (!wKD)
      {
        ZException.setMessage(_GET_FUNCTION_NAME_,
            ZS_BADDIC,
            Severity_Severe,
            "Key dictionary corrupted or incomplete : Index name <%s> not found in master key dictionary.",IndexTable[wi]->IndexName.toCChar());
        return ZS_BADDIC;
      }
      IndexTable[wi]->IdxKeyDic=wKD;  /* store locally to index file a direct access (pointer) to key dictionary from master */
    }// for

    }//if (MDicSize > 0)
#endif // __OLD_COMMENT__


  if (ZJCB!=nullptr)
    delete ZJCB;
  ZJCB=nullptr;
  if (JCBSize > 0)
  {
    ZJCB = new ZJournalControlBlock;
    wSt= ZJCB->_import(pPtrIn);
  }

  return  wSt;
  #endif // __COMMENT__
}//_import
/*


ZStatus
ZMasterControlBlock::_import(const unsigned char* &pDataIn)
{
  ZArray<ZPRES> wIndexPresence;

  return _import(pMaster,pDataIn,wIndexPresence);
}//_import
*/
/**
 * @brief ZMasterControlBlock::print Reports the whole content of ZMCB : indexes definitions and dictionaries
 */
void
ZMasterControlBlock::report(FILE*pOutput)
{
  fprintf (pOutput,
      "________________ZMasterControlBlock Content________________________________\n");
  fprintf(pOutput,
      "Master Dictionary\n"
      "-----------------\n");
  if (Dictionary==nullptr)
    fprintf(pOutput,"---------No master dictionary (file might be of type ZRawMasterFile)-----------\n");
  else
    {
    fprintf (pOutput,
        "   Fields %17s %10s %10s %8s %s\n",
        "Field Name",
        "Natural",
        "Universal",
        "Eligible",
        "ZType");

    for (long wi=0;wi < Dictionary->size();wi++)
    {
      fprintf (pOutput,
          "    <%2ld> <%15s> %10ld %10ld %8s %s\n",
          wi,
          Dictionary->Tab(wi).getName().toCChar(),
          Dictionary->Tab(wi).NaturalSize,
          Dictionary->Tab(wi).UniversalSize,
          Dictionary->Tab(wi).KeyEligible?"Yes":"No",
          decode_ZType(Dictionary->Tab(wi).ZType));
    }
    fprintf (pOutput,
        "___________________________________________________________________________\n");
  }// else


  fprintf(pOutput,
      "Index Control Blocks\n"
      "--------------------\n");
  for (long wi=0;wi < IndexTable.size();wi++)
  {

    fprintf (pOutput,
            "Index Rank <%2ld> <%20s> KeyUSize <%6d> <%s>\n",
            wi,
            IndexTable[wi]->IndexName.toCChar(),
            IndexTable[wi]->KeyGuessedSize,
            decode_ZST(IndexTable[wi]->Duplicates));
    if (Dictionary!=nullptr)
    {
    fprintf (pOutput,
        "   Fields %17s  %15s  %5s %12s %s\n",
        "Field Name",
        "Natural",
        "Internal",
        "Key Offset",
        "ZType");
    for (long wj=0; wj < Dictionary->KeyDic[wi]->size();wj++)
    {
      fprintf (pOutput,
          "    <%ld> <%15s> %5ld %5ld %12d %s\n",
          wj,
          Dictionary->Tab(Dictionary->KeyDic[wi]->Tab(wj).MDicRank).getName().toCChar(),
          Dictionary->Tab(Dictionary->KeyDic[wi]->Tab(wj).MDicRank).NaturalSize,
          Dictionary->Tab(Dictionary->KeyDic[wi]->Tab(wj).MDicRank).UniversalSize,
          Dictionary->KeyDic[wi]->Tab(wj).KeyOffset,
          decode_ZType(Dictionary->Tab(Dictionary->KeyDic[wi]->Tab(wj).MDicRank).ZType));
    }// for
    }// if (Dictionary!=nullptr)
  }// for (long wi=0;wi < IndexTable.size();wi++)

  fprintf (pOutput,
      "___________________________________________________________________________\n");
  return ;
} // report

ZStatus
ZMasterControlBlock::loadDictionary(ZaiErrors* pErrorlog){
  return Dictionary->load_xml(pErrorlog);
}
#ifdef __USE_BIN_DICTIONARY__
ZStatus
ZMasterControlBlock::loadDictionary_bin(){
  return Dictionary->load_bin();
}
#endif //__USE_BIN_DICTIONARY__

void ZMasterControlBlock::setEngineMode (uint8_t pSE)
{
  EngineMode = pSE;
/*  for (long wi=0;wi < IndexTable.count();wi++)
    if (IndexTable[wi]->isOpen()){
      IndexTable[wi]->setEngineMode(EngineMode);
    }
*/
}

#endif // ZSMASTERCONTROLBLOCK_CPP
