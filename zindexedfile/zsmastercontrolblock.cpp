#ifndef ZSMASTERCONTROLBLOCK_CPP
#define ZSMASTERCONTROLBLOCK_CPP

#include "zsmastercontrolblock.h"
#include <zindexedfile/zsjournalcontrolblock.h>
#include <zindexedfile/zmfdictionary.h>
#include <zxml/zxmlprimitives.h>

#include <zindexedfile/zindexdata.h>

using namespace zbs;

ZSMasterControlBlock::ZSMasterControlBlock() {clear();}


// ----------ZMasterControlBlock ----------
//

ZSMasterControlBlock::~ZSMasterControlBlock(void)
{
  /*    if (MDicCheckSum!=nullptr)   // No MDicCheckSum object is deleted while deleting MetaDic object
                delete MDicCheckSum;*/
  if (ZJCB!=nullptr)
    delete ZJCB;
  if (MasterDic!=nullptr)
    delete MasterDic;
  while (IndexTable.size()>0)
  {
    IndexTable.pop();
  }
  return;
}


long ZSMasterControlBlock::popIndex(void)
{
  if (IndexTable.pop()<0)
  {
    return -1;
  }
  IndexCount=IndexTable.size();
  return IndexTable.size();
}


void ZSMasterControlBlock::clear(void)
{
  IndexCount=0;
  //    HistoryOn=false;
  //    JournalingOn = false;
  MCBSize = 0;
  //    BlockID=ZBID_MCB;                 // only used within _Export structures
  //    ZMFVersion= __ZMF_VERSION__;
  //    StartSign=cst_ZSTART;
  while (IndexTable.size() > 0)
    popIndex();
  IndexTable.clear();
  IndexFilePath.clear();
  return;
}
#ifdef __COMMENT__
/**
 * @brief ZMasterControlBlock::pushICBtoIndex creates a new rank in ZMCB.IndexTable rank and stores ZIndexControlblock in it
 * @param pICB
 */
void
ZSMasterControlBlock::pushICBtoIndexTable(ZSIndexControlBlock* pICB)
{
  //    ZIndexField_struct wField;

  /*    Index.newBlankElement();
    Index.last()->clear(pMetaDic);  // create ZICB instance -  Key Dictionnary

  IndexCount= Index.size();
  long wi = Index.lastIdx();

  memmove (&Index[wi],pICB,sizeof(ZSICBOwnData));
  long wj = 0 ;
  while (wj<pICB.ZKDic->size())
  {
    Index[wi]->ZKDic->push(pICB.ZKDic->Tab[wj]);
    wj++;
  }*/
      Index.push(pICB);
  return;
}

void
ZSMasterControlBlock::removeICBfromIndexTable(const long pRank)
{
  //   delete Index[pRank].ZKDic; - done in erase overloaded routine
  Index.erase(pRank);
  IndexTable.erase(pRank);
  IndexCount= Index.size();
  return ;
}
#endif // __COMMENT__

ZSMCBOwnData_Export& ZSMCBOwnData_Export::_copyFrom(const ZSMCBOwnData_Export& pIn)
{
  StartSign=pIn.StartSign;
  BlockID=pIn.BlockID;
  ZMFVersion=pIn.ZMFVersion;
  MCBSize=pIn.MCBSize;
  MDicOffset=pIn.MDicOffset;
  MDicSize=pIn.MDicSize;
  IndexCount=pIn.IndexCount;
  ICBOffset=pIn.ICBOffset;
  ICBOffset=pIn.ICBOffset;
  ICBSize=pIn.ICBSize;
  JCBOffset=pIn.JCBOffset;
  JCBSize=pIn.JCBSize;
  HistoryOn=pIn.HistoryOn;
  return *this;
}

ZSMCBOwnData_Export& ZSMCBOwnData_Export::_copyFrom(const ZSMCBOwnData& pIn)
{

  StartSign=cst_ZSTART;
  BlockID=ZBID_MCB;
  ZMFVersion=__ZMF_VERSION__;

  MCBSize=pIn.MCBSize;
  MDicOffset=pIn.MDicOffset;
  MDicSize=pIn.MDicSize;
  IndexCount=pIn.IndexCount;
  ICBOffset=pIn.ICBOffset;
  ICBSize=pIn.ICBSize;
  JCBOffset=pIn.JCBOffset;
  JCBSize=pIn.JCBSize;
  HistoryOn=pIn.HistoryOn;
  return *this;
}

ZSMCBOwnData& ZSMCBOwnData_Export::_toMCBOwnData(ZSMCBOwnData& pOut)
{
  pOut.MCBSize=MCBSize;
  pOut.MDicOffset=MDicOffset;
  pOut.IndexCount=IndexCount;
  pOut.ICBOffset=ICBOffset;
  pOut.ICBSize=ICBSize;
  pOut.JCBOffset=JCBOffset;
  pOut.JCBSize=JCBSize;
  pOut.HistoryOn=HistoryOn;

  return pOut;
}


ZSMCBOwnData_Export& ZSMCBOwnData_Export::reverseConditional()
{
  if (!is_little_endian())
    return *this;
  StartSign=reverseByteOrder_Conditional<uint32_t>(StartSign);
  ZMFVersion=reverseByteOrder_Conditional<uint32_t>(ZMFVersion);
  MCBSize=reverseByteOrder_Conditional<uint32_t>(MCBSize);
  MDicOffset=reverseByteOrder_Conditional<uint32_t>(MDicOffset);
  MDicSize=reverseByteOrder_Conditional<int32_t>(MDicSize);
  ICBOffset=reverseByteOrder_Conditional<uint32_t>(ICBOffset);
  ICBSize=reverseByteOrder_Conditional<uint32_t>(ICBSize);
  JCBOffset=reverseByteOrder_Conditional<uint32_t>(JCBOffset);
  JCBSize=reverseByteOrder_Conditional<int32_t>(JCBSize);
  IndexCount=reverseByteOrder_Conditional<uint32_t>(IndexCount);
  return *this;
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

ZDataBuffer&
ZSMCBOwnData::_exportAppend(ZDataBuffer& pZDBExport)
{

  ZSMCBOwnData_Export wMCBE;

 /* wMCB.StartSign=cst_ZSTART ;
  wMCB.BlockID=ZBID_MCB;
  wMCB.ZMFVersion=reverseByteOrder_Conditional<unsigned long>(__ZMF_VERSION__);

  MDicOffset = MCBSize = sizeof(ZSMCBOwnData_Export) + IndexFilePath._getexportUVFSize() ;

  wMCB.MCBSize=reverseByteOrder_Conditional<uint32_t>(MCBSize);
  wMCB.MDicOffset=reverseByteOrder_Conditional<uint32_t>(MDicOffset);
  wMCB.MDicSize=reverseByteOrder_Conditional<int32_t>(MDicSize);
  wMCB.IndexCount=reverseByteOrder_Conditional<uint32_t>(IndexCount);
  wMCB.ICBOffset=reverseByteOrder_Conditional<uint32_t>(ICBOffset);
  wMCB.ICBSize=reverseByteOrder_Conditional<uint32_t>(ICBSize);
  wMCB.JCBOffset=reverseByteOrder_Conditional<uint32_t>(JCBOffset);
  wMCB.JCBSize=reverseByteOrder_Conditional<int32_t>(JCBSize);

  wMCB.HistoryOn=HistoryOn;  // uint8_t used as boolean
*/
  wMCBE._copyFrom(*this);
  wMCBE.reverseConditional();
  pZDBExport.appendData(&wMCBE,sizeof(ZSMCBOwnData_Export));  // move all up until IndexFilePath
      // append IndexFilePath without '\0' end character : NO END SIGN MARKER
  IndexFilePath._exportAppendUVF(pZDBExport);
//  pZDBExport.appendData(wIndexPath);

  return pZDBExport;
}//ZSMCBOwnData::_exportAppend


ZDataBuffer
ZSMCBOwnData::_export()
{
  ZDataBuffer wExp;
  return _exportAppend(wExp);
}//ZSMCBOwnData::_export

ZStatus
ZSMCBOwnData::_import(unsigned char* &pZDBImport_Ptr)
{
  ZStatus wSt;
  ZSMCBOwnData_Export* wMCB=(ZSMCBOwnData_Export*) pZDBImport_Ptr;

  if (wMCB->StartSign != cst_ZSTART)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Severe,
        "Error Master Control Block  : file header appears to be corrupted - invalid MCB StartBlock");
    return (ZS_BADFILEHEADER);
    }
  if (wMCB->BlockID != ZBID_MCB)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADICB,
        Severity_Severe,
        "Error Master Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
        wMCB->BlockID);
    return  (ZS_BADICB);
    }
  if (reverseByteOrder_Conditional<unsigned long>(wMCB->ZMFVersion)!= __ZMF_VERSION__)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEVERSION,
        Severity_Error,
        "Error Master Control Block   : Found version <%ld> while current ZMF version is <%ld>",
        reverseByteOrder_Conditional<unsigned long>(wMCB->ZMFVersion),
        __ZMF_VERSION__);
    return (ZS_BADFILEVERSION);
    }

/*
  MCBSize=reverseByteOrder_Conditional<uint32_t>(wMCB->MCBSize);
  MDicOffset=reverseByteOrder_Conditional<uint32_t>(wMCB->MDicOffset);
  MDicSize=reverseByteOrder_Conditional<int32_t>(wMCB->MDicSize);
  ICBOffset=reverseByteOrder_Conditional<uint32_t>(wMCB->ICBOffset);
  ICBSize=reverseByteOrder_Conditional<uint32_t>(wMCB->ICBSize);
  JCBOffset=reverseByteOrder_Conditional<uint32_t>(wMCB->JCBOffset);
  JCBSize=reverseByteOrder_Conditional<int32_t>(wMCB->JCBSize);
  IndexCount=reverseByteOrder_Conditional<size_t>(wMCB->IndexCount);

  HistoryOn=wMCB->HistoryOn;  // uint8_t used as boolean
*/

  wMCB->reverseConditional();
  wMCB->_toMCBOwnData(*this);

  pZDBImport_Ptr+=sizeof(ZSMCBOwnData_Export);
  IndexFilePath._importUVF(pZDBImport_Ptr);

  /*    wMCB->IndexFilePathSize=_reverseByteOrder_T<uint16_t>(wMCB->IndexFilePathSize);
    wMCB->IndexFilePath=(char*) (wMCB + offsetof(ZSMCBOwnData_Export,IndexFilePath));
//size_t wSize = MCBSize - offsetof(ZSMCBOwnData_Export,IndexFilePath)-sizeof (uint32_t);
    IndexFilePath.clear();
    IndexFilePath.fromStringCount(wMCB->IndexFilePath,wMCB->IndexFilePathSize);*/

  return  ZS_SUCCESS;
}//ZSMCBOwnData::_import

/*
<zmastercontrolblock>
    <indexfilepath> </indexfilepath> <!-- Directory path for index files. If empty, then directory path of main content file is taken -->
    <history> </history>
    <indexcount> </indexcount>
</zmastercontrolblock>
*/
utf8String
ZSMasterControlBlock::toXml(int pLevel,bool pComment)
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

  wReturn+=fmtXMLuint("indexcount",  IndexCount,wLevel);
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
  if (MasterDic!=nullptr)
    wReturn += MasterDic->toXml(wLevel);

  wReturn += fmtXMLendnode("zmastercontrolblock",pLevel);
  return wReturn;
} // ZSMasterControlBlock::toXml



ZStatus ZSMasterControlBlock::fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog)
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

  if (XMLgetChildUInt32(wRootNode, "indexcount", IndexCount, pErrorlog)< 0) {
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


/* MCB is the concatenation of following
 *
 *
    MCBOwn

    IndexFilePath (varying string UVF format)

    MetaDic : if exists

    ZSIndexControlTable ( list of Index control blocks )

    Journal Control Block : if exists

 *
 */

ZDataBuffer ZSMasterControlBlock::_exportMCB()
{
  ZDataBuffer wZDB;
  return _exportMCBAppend(wZDB);
}

ZDataBuffer& ZSMasterControlBlock::_exportMCBAppend(ZDataBuffer &pMCBContent)
{

  /* first export journalling : if journalling exists */

  ZDataBuffer wJCB;
  if (ZJCB!=nullptr)
    ZJCB->_exportJCB(wJCB);

  /* second export meta dic : if meta dic exists */
  ZDataBuffer wMDic;
//  if (MetaDic != nullptr)
//    MetaDic->_export(wMDic);
  if (MasterDic!=nullptr)
    MasterDic->_exportAppend(wMDic);

  /* third icb */

  ZDataBuffer wICB;
  wICB.reset();
  for (long wi = 0;wi < IndexTable.size(); wi++)
    {
    IndexTable[wi]->_exportAppend(wICB);
    }

  /* then start filling MCB owndata */


  uint32_t wOffset=sizeof(ZSMCBOwnData_Export)+IndexFilePath._getexportUVFSize();

  ZSMCBOwnData_Export wMCBE;

  wMCBE._copyFrom(*this);

  if (MasterDic == nullptr)
      {
        wMCBE.MDicSize = 0;
        wMCBE.MDicOffset = 0;
      }
      else
      {
        wMCBE.MDicSize =  uint32_t(wMDic.Size);
        wMCBE.MDicOffset = wOffset;
        wOffset += wMDic.Size;
      }

  wMCBE.ICBSize = (uint32_t)wICB.Size;
  wMCBE.ICBOffset = (uint32_t)wOffset;
  wOffset += wICB.Size;

  if  (ZJCB==nullptr)
    {
    wMCBE.JCBSize = 0;
    wMCBE.JCBOffset = 0;
    }
  else
    {
    wMCBE.JCBSize =  wJCB.Size;
    wMCBE.JCBOffset = wOffset;
    wOffset += wJCB.Size;
    }

  wMCBE.MCBSize = wOffset ; /* updated offset gives the total size for MCB */
  wMCBE.reverseConditional();

  pMCBContent.setData(&wMCBE,sizeof(ZSMCBOwnData_Export)); /* because always not appended to anything else */
  IndexFilePath._exportAppendUVF(pMCBContent);

  pMCBContent.appendData(wICB);

  if (!wMDic.isEmpty())
    pMCBContent.appendData(wMDic);



  if (!wJCB.isEmpty())
    pMCBContent.appendData(wJCB);

  return  pMCBContent;
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




  if (MasterDic!=nullptr)
    delete MasterDic;
  MasterDic=nullptr;
  if (MDicSize > 0)
    {
    MasterDic = new ZMFDictionary;
    wInSize = MasterDic->_import(wPtrIn);
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

ZStatus
ZSMasterControlBlock::_import(unsigned char*& pPtrIn,ZArray<ZPRES>& pIndexPresence)
{
  ZStatus wSt;
  //long wIndexCountsv ;
  ZArray<ZSIndexControlBlock*> wICBTable;
  /* IndexPresence values meaning
   * 0 : Index to be deleted
   * 1 : Index present but not to be rebuilt
   * 2 : Index to be built or rebuilt -
   * 3 : Index created (therefore rebuilt)
   * 4 : Index errored
   */

  wSt=ZSMCBOwnData::_import(pPtrIn);  // wPtrIn is updated
  if (wSt!=ZS_SUCCESS)
    return  wSt;


  //-----------Import defined IndexControlBlocks for the file

  //    wIndexCountsv = IndexCount;

  long wi =0;

  /* import all index control blocks at once */

  while  ((wi < IndexCount) && (wi < IndexTable.count()))
    {
    wSt=IndexTable[wi]->_import(pPtrIn);
    wi++;
    }

  while (wi < IndexCount)
    {
      pIndexPresence.push(ZPRES_Unchanged);
      ZRawIndexFile* wZRIF= new ZRawIndexFile((ZRawMasterFile *)this);
      wSt=wZRIF->ZSIndexControlBlock::_import(pPtrIn);
      if (wSt!=ZS_SUCCESS)
        {
        fprintf (stderr,"ZSMasterControlBlock::_import-E-CANTIMPORT Cannot import ZSIndexControlBlock.\n");
        delete wZRIF;
        return wSt;
        }
      IndexTable.push(wZRIF);
    }

  size_t wInSize=0;
  if (MasterDic!=nullptr)
          delete MasterDic;
  MasterDic=nullptr;
  if (MDicSize > 0)
    {
    MasterDic = new ZMFDictionary;
    wInSize = MasterDic->_import(pPtrIn);

    /* Match Index rank with key dictionary rank and update KeyDic within index file
     * NB: if no dictionary present (RawMasterFile) then KeyDic will remain nullptr
     */
    for (long wi=0;wi < IndexTable.count();wi++ )
    {
      long wh=MasterDic->searchKeyCase(IndexTable[wi]->IndexName);
      if (wh < 0)
      {
        ZException.setMessage(_GET_FUNCTION_NAME_,
            ZS_BADDIC,
            Severity_Severe,
            "Key dictionary corrupted or incomplete : Index name <%s> not found in key dictionary.",IndexTable[wi]->IndexName.toCChar());
        return ZS_BADDIC;
      }
      IndexTable[wi]->KeyDic=MasterDic->KeyDic[wh];  /* store locally to index file a direct access to key dictionary from master */
    }// for

    }//if (MDicSize > 0)




  if (ZJCB!=nullptr)
    delete ZJCB;
  ZJCB=nullptr;
  if (JCBSize > 0)
  {
    ZJCB = new ZSJournalControlBlock;
    wSt= ZJCB->_importJCB(pPtrIn);
  }

  return  wSt;
}//_import


ZStatus
ZSMasterControlBlock::_import(ZDataBuffer &pDataIn)
{

  ZArray<ZPRES> wIndexPresence;

  unsigned char* wPtrIn= pDataIn.Data;
  return _import(wPtrIn,wIndexPresence);
}//_import

/**
 * @brief ZMasterControlBlock::print Reports the whole content of ZMCB : indexes definitions and dictionaries
 */
void
ZSMasterControlBlock::report(FILE*pOutput)
{
  fprintf (pOutput,
      "________________ZMasterControlBlock Content________________________________\n");
  fprintf(pOutput,
      "Master Dictionary\n"
      "-----------------\n");
  if (MasterDic==nullptr)
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

    for (long wi=0;wi < MasterDic->size();wi++)
    {
      fprintf (pOutput,
          "    <%2ld> <%15s> %10ld %10ld %8s %s\n",
          wi,
          MasterDic->Tab[wi].getName().toCChar(),
          MasterDic->Tab[wi].NaturalSize,
          MasterDic->Tab[wi].UniversalSize,
          MasterDic->Tab[wi].KeyEligible?"Yes":"No",
          decode_ZType(MasterDic->Tab[wi].ZType));
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
            IndexTable[wi]->KeyUniversalSize,
            IndexTable[wi]->Duplicates==ZST_DUPLICATES?"Duplicates":"No Duplicates");
    if (MasterDic!=nullptr)
    {
    fprintf (pOutput,
        "   Fields %17s  %15s  %5s %12s %s\n",
        "Field Name",
        "Natural",
        "Internal",
        "Key Offset",
        "ZType");
    for (long wj=0; wj < MasterDic->KeyDic[wi]->size();wj++)
    {
      fprintf (pOutput,
          "    <%ld> <%15s> %5ld %5ld %12d %s\n",
          wj,
          MasterDic->Tab[MasterDic->KeyDic[wi]->Tab[wj].MDicRank].getName().toCChar(),
          MasterDic->Tab[MasterDic->KeyDic[wi]->Tab[wj].MDicRank].NaturalSize,
          MasterDic->Tab[MasterDic->KeyDic[wi]->Tab[wj].MDicRank].UniversalSize,
          MasterDic->KeyDic[wi]->Tab[wj].KeyOffset,
          decode_ZType(MasterDic->Tab[MasterDic->KeyDic[wi]->Tab[wj].MDicRank].ZType));
    }// for
    }// if (MasterDic!=nullptr)
  }// for (long wi=0;wi < IndexTable.size();wi++)

  fprintf (pOutput,
      "___________________________________________________________________________\n");
  return ;
} // report

#endif // ZSMASTERCONTROLBLOCK_CPP
