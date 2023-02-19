#include "zfilecontrolblock.h"

#include <zxml/zxmlprimitives.h>

ZFileControlBlock::ZFileControlBlock()
{
  clear();
  return;
}
void ZFileControlBlock::clear(void)
{
  memset(this,0,sizeof(ZFileControlBlock));
//  StartSign=cst_ZSTART;
  StartOfData = 0L;
//  BlockId = ZBID_FCB;
//  EndSign=cst_ZEND;
  return;
}

ZFileControlBlock& ZFileControlBlock::_copyFrom(const ZFileControlBlock& pIn)
{
  StartOfData=pIn.StartOfData;
  AllocatedBlocks=pIn.AllocatedBlocks;
  BlockExtentQuota=pIn.BlockExtentQuota;
  ZBAT_DataOffset=pIn.ZBAT_DataOffset;
  ZBAT_ExportSize=pIn.ZBAT_ExportSize;

  ZFBT_DataOffset=pIn.ZFBT_DataOffset;
  ZFBT_ExportSize=pIn.ZFBT_ExportSize;

  ZHOT_DataOffset=pIn.ZHOT_DataOffset;
  ZHOT_ExportSize=pIn.ZHOT_ExportSize;

/* // Deprecated
  ZDBT_DataOffset=pIn.ZDBT_DataOffset;
  ZDBT_ExportSize=pIn.ZDBT_ExportSize;
*/
  ZReserved_DataOffset=pIn.ZReserved_DataOffset;
  ZReserved_ExportSize=pIn.ZReserved_ExportSize;

  InitialSize=pIn.InitialSize;
  AllocatedSize=pIn.AllocatedSize;
  UsedSize=pIn.UsedSize;

  MinSize=pIn.MinSize;
  MaxSize=pIn.MaxSize;
  BlockTargetSize=pIn.BlockTargetSize;

  HighwaterMarking=pIn.HighwaterMarking;
  GrabFreeSpace=pIn.GrabFreeSpace;
//  EndSign=pIn.EndSign;
  return *this;
}



FCBParams
ZFileControlBlock::getUseableParams()
{
  FCBParams wReturn;
  wReturn.AllocatedBlocks = AllocatedBlocks;
  wReturn.BlockExtentQuota = BlockExtentQuota;
  wReturn.InitialSize = InitialSize;
  wReturn.BlockTargetSize = BlockTargetSize;
  wReturn.HighwaterMarking = HighwaterMarking;
  wReturn.GrabFreeSpace = GrabFreeSpace;
  return wReturn;
}

void
ZFileControlBlock::setUseableParams(const FCBParams& pIn)
{
  AllocatedBlocks = pIn.AllocatedBlocks;
  BlockExtentQuota = pIn.BlockExtentQuota;
  InitialSize = pIn.InitialSize;
  BlockTargetSize = pIn.BlockTargetSize;
  HighwaterMarking = pIn.HighwaterMarking;
  GrabFreeSpace = pIn.GrabFreeSpace;
}




//=========================== ZFileControlBlock Export import==========================================

size_t
ZFileControlBlock::_exportAppend(ZDataBuffer& pZDBExport)
{
  ZFCB_Export wFCB;
  wFCB.set(*this);
  wFCB.serialize();

  pZDBExport.appendData(&wFCB,sizeof(wFCB));

  return sizeof(ZFCB_Export);
}// ZFileControlBlock::_exportAppend

ZDataBuffer
ZFileControlBlock::_export()
{
  ZFCB_Export wFCB;
  wFCB.set(*this);
  wFCB.serialize();

  return ZDataBuffer(&wFCB,sizeof(wFCB));
}// ZFileControlBlock::_exportAppend

ZStatus
ZFileControlBlock::_import(const unsigned char *&pZDBImport_Ptr)
{
  ZFCB_Export* wFCBe=(ZFCB_Export*) pZDBImport_Ptr;

  //    StartSign=_reverseByteOrder_T<uint32_t>(wFCB->StartSign);
  if (wFCBe->StartSign!=cst_ZBLOCKSTART)
      {
      ZException.setMessage("ZFileControlBlock::_import",
          ZS_CORRUPTED,
          Severity_Severe,
          "File Control Block appears to be corrupted : invalid start block sign.");
      return  (ZS_CORRUPTED);
      }
  if (wFCBe->EndSign!=cst_ZBLOCKEND)
      {
        ZException.setMessage("ZFileControlBlock::_import",
            ZS_CORRUPTED,
            Severity_Severe,
            "File Control Block appears to be corrupted : invalid end block sign.");
        return  (ZS_CORRUPTED);
      }
  if (wFCBe->BlockId!=ZBID_FCB)
      {
        ZException.setMessage("ZFileControlBlock::_import",
            ZS_INVTYPE,
            Severity_Error,
            "File Control Block appears to be corrupted : invalid block id.");
        return  (ZS_INVTYPE);
      }

  wFCBe->deserialize();
  wFCBe->toFCB(*this);
   pZDBImport_Ptr += sizeof(ZFCB_Export);
  return ZS_SUCCESS;
}// ZFileControlBlock::_import

ZFCB_Export& ZFCB_Export::_copyFrom(const ZFCB_Export& pIn)
{
  StartSign=pIn.StartSign;
  BlockId=pIn.BlockId;
  StartOfData=pIn.StartOfData;
  AllocatedBlocks=pIn.AllocatedBlocks;
  BlockExtentQuota=pIn.BlockExtentQuota;
  ZBAT_DataOffset=pIn.ZBAT_DataOffset;
  ZBAT_ExportSize=pIn.ZBAT_ExportSize;

  ZFBT_DataOffset=pIn.ZFBT_DataOffset;
  ZFBT_ExportSize=pIn.ZFBT_ExportSize;

  ZHOT_DataOffset=pIn.ZHOT_DataOffset;
  ZHOT_ExportSize=pIn.ZHOT_ExportSize;
/*  Deprecated
  ZDBT_DataOffset=pIn.ZDBT_DataOffset;
  ZDBT_ExportSize=pIn.ZDBT_ExportSize;
*/
/*  ZReserved_DataOffset=pIn.ZReserved_DataOffset;
  ZReserved_ExportSize=pIn.ZReserved_ExportSize;
*/
  InitialSize=pIn.InitialSize;
  AllocatedSize=pIn.AllocatedSize;
  UsedSize=pIn.UsedSize;

  MinSize=pIn.MinSize;
  MaxSize=pIn.MaxSize;
  BlockTargetSize=pIn.BlockTargetSize;

  HighwaterMarking=pIn.HighwaterMarking;
  GrabFreeSpace=pIn.GrabFreeSpace;
  EndSign=pIn.EndSign;
  return *this;
}

ZFCB_Export& ZFCB_Export::set(const ZFileControlBlock& pIn)
{
  StartSign=cst_ZBLOCKSTART;
  BlockId=ZBID_FCB;
  EndianCheck=cst_EndianCheck_Normal;

  StartOfData=pIn.StartOfData;
  AllocatedBlocks=pIn.AllocatedBlocks;
  BlockExtentQuota=pIn.BlockExtentQuota;
  ZBAT_DataOffset=pIn.ZBAT_DataOffset;
  ZBAT_ExportSize=pIn.ZBAT_ExportSize;

  ZFBT_DataOffset=pIn.ZFBT_DataOffset;
  ZFBT_ExportSize=pIn.ZFBT_ExportSize;

  ZHOT_DataOffset=pIn.ZHOT_DataOffset;
  ZHOT_ExportSize=pIn.ZHOT_ExportSize;

  /* // Deprecated
  ZDBT_DataOffset=pIn.ZDBT_DataOffset;
  ZDBT_ExportSize=pIn.ZDBT_ExportSize;
*/
/*  ZReserved_DataOffset=pIn.ZReserved_DataOffset;
  ZReserved_ExportSize=pIn.ZReserved_ExportSize;
*/
  InitialSize=pIn.InitialSize;
  AllocatedSize=pIn.AllocatedSize;
  UsedSize=pIn.UsedSize;

  MinSize=pIn.MinSize;
  MaxSize=pIn.MaxSize;
  BlockTargetSize=pIn.BlockTargetSize;

  HighwaterMarking=pIn.HighwaterMarking;
  GrabFreeSpace=pIn.GrabFreeSpace;
  EndSign=cst_ZBLOCKEND;
  return *this;
}

void ZFCB_Export::setFromPtr(const unsigned char * &pPtrIn)
{
  ZFCB_Export* pIn=(ZFCB_Export*)pPtrIn;
  StartSign=pIn->StartSign;
  BlockId=pIn->BlockId;
  EndianCheck=pIn->EndianCheck;

  StartOfData=pIn->StartOfData;
  AllocatedBlocks=pIn->AllocatedBlocks;
  BlockExtentQuota=pIn->BlockExtentQuota;
  ZBAT_DataOffset=pIn->ZBAT_DataOffset;
  ZBAT_ExportSize=pIn->ZBAT_ExportSize;

  ZFBT_DataOffset=pIn->ZFBT_DataOffset;
  ZFBT_ExportSize=pIn->ZFBT_ExportSize;

  ZHOT_DataOffset=pIn->ZHOT_DataOffset;
  ZHOT_ExportSize=pIn->ZHOT_ExportSize;

  /* // Deprecated

  ZDBT_DataOffset=pIn->ZDBT_DataOffset;
  ZDBT_ExportSize=pIn->ZDBT_ExportSize;
*/
/*  ZReserved_DataOffset=pIn->ZReserved_DataOffset;
  ZReserved_ExportSize=pIn->ZReserved_ExportSize;
*/
  InitialSize=pIn->InitialSize;
  AllocatedSize=pIn->AllocatedSize;
  UsedSize=pIn->UsedSize;

  MinSize=pIn->MinSize;
  MaxSize=pIn->MaxSize;
  BlockTargetSize=pIn->BlockTargetSize;

  HighwaterMarking=pIn->HighwaterMarking;
  GrabFreeSpace=pIn->GrabFreeSpace;
  EndSign=pIn->EndSign;

  pPtrIn += sizeof(ZFCB_Export);
}

ZFileControlBlock& ZFCB_Export::toFCB(ZFileControlBlock& pOut)
{
  if (isReversed())
    fprintf(stderr,"ZFCB_Export-W-NOTREV File control block is updated with a reversed(serialized) values.\n");
  pOut.StartOfData=StartOfData;
  pOut.AllocatedBlocks=AllocatedBlocks;
  pOut.BlockExtentQuota=BlockExtentQuota;
  pOut.ZBAT_DataOffset=ZBAT_DataOffset;
  pOut.ZBAT_ExportSize=ZBAT_ExportSize;

  pOut.ZFBT_DataOffset=ZFBT_DataOffset;
  pOut.ZFBT_ExportSize=ZFBT_ExportSize;

  pOut.ZHOT_DataOffset=ZHOT_DataOffset;
  pOut.ZHOT_ExportSize=ZHOT_ExportSize;

  /*// Deprecated
  pOut.ZDBT_DataOffset=ZDBT_DataOffset;
  pOut.ZDBT_ExportSize=ZDBT_ExportSize;
*/
/* pOut.ZReserved_DataOffset=ZReserved_DataOffset;
  pOut.ZReserved_ExportSize=ZReserved_ExportSize;
*/
  pOut.InitialSize=InitialSize;
  pOut.AllocatedSize=AllocatedSize;
  pOut.UsedSize=UsedSize;

  pOut.MinSize=MinSize;
  pOut.MaxSize=MaxSize;
  pOut.BlockTargetSize=BlockTargetSize;

  pOut.HighwaterMarking=HighwaterMarking;
  pOut.GrabFreeSpace=GrabFreeSpace;
  return pOut;
}


ZFCB_Export& ZFCB_Export::operator = (const ZFileControlBlock& pIn)
{
  return set(pIn);
}


void ZFCB_Export::_convert()
{
  if (!is_little_endian())
    return ;
  EndianCheck=reverseByteOrder_Conditional<uint16_t>(EndianCheck);

  StartOfData=reverseByteOrder_Conditional<zaddress_type>(StartOfData);
  AllocatedBlocks=reverseByteOrder_Conditional<unsigned long>(AllocatedBlocks);
  BlockExtentQuota=reverseByteOrder_Conditional<unsigned long>(BlockExtentQuota);
  ZBAT_DataOffset=reverseByteOrder_Conditional<size_t>(ZBAT_DataOffset);
  ZBAT_ExportSize=reverseByteOrder_Conditional<size_t>(ZBAT_ExportSize);
  ZFBT_DataOffset=reverseByteOrder_Conditional<size_t>(ZFBT_DataOffset);
  ZFBT_ExportSize=reverseByteOrder_Conditional<size_t>(ZFBT_ExportSize);

  ZHOT_DataOffset=reverseByteOrder_Conditional<size_t>(ZHOT_DataOffset);
  ZHOT_ExportSize=reverseByteOrder_Conditional<size_t>(ZHOT_ExportSize);

/* Deprecated
  ZDBT_DataOffset=reverseByteOrder_Conditional<size_t>(ZDBT_DataOffset);
  ZDBT_ExportSize=reverseByteOrder_Conditional<size_t>(ZDBT_ExportSize);
*/
//  ZReserved_DataOffset=reverseByteOrder_Conditional<size_t>(ZReserved_DataOffset);
//  ZReserved_ExportSize=reverseByteOrder_Conditional<size_t>(ZReserved_ExportSize);
  InitialSize=reverseByteOrder_Conditional<zsize_type>(InitialSize);
  AllocatedSize=reverseByteOrder_Conditional<zsize_type>(AllocatedSize);
  UsedSize=reverseByteOrder_Conditional<zsize_type>(UsedSize);
  MinSize=reverseByteOrder_Conditional<size_t>(MinSize);
  MaxSize=reverseByteOrder_Conditional<size_t>(MaxSize);
  BlockTargetSize=reverseByteOrder_Conditional<size_t>(BlockTargetSize);
//HighwaterMarking=HighwaterMarking;
//GrabFreeSpace=GrabFreeSpace;
}


void
ZFCB_Export::serialize()
{
  if (!is_little_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZFCB_Export::serialize-W-ALRDY File Control Block already serialized.\n");
    return;
  }
  _convert();
}

void
ZFCB_Export::deserialize()
{
  if (!is_little_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZFCB_Export::deserialize-W-ALRDY File Control Block already deserialized. \n");
    return;
  }
  _convert();
}


//-------------xml export import ----------------------------

utf8String ZFileControlBlock::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("zfilecontrolblock",pLevel);
  wLevel++;

  /* NB: StartSign and BlockId are not exported to xml */
  wReturn+=fmtXMLint64("startofdata",StartOfData,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"FYI : offset where Data storage starts - cannot be modified.");

  if (pComment)
    wReturn+=fmtXMLcomment(" required fields for ZRandomFile creation : see ZRandomFile::setCreateMaximum() ",wLevel);

  wReturn+=fmtXMLulong("allocatedblocks",AllocatedBlocks,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," required at creation time : for ZBAT & ZFBT : available allocated slots in ZBAT and ZFBT");
  wReturn+=fmtXMLulong("blockextentquota",BlockExtentQuota,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"  required at creation time : :  for ZBAT & ZFBT : initial extension quota ");
  wReturn+=fmtXMLlong("initialsize",InitialSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"  required at creation time : Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk ");
  wReturn+=fmtXMLulong("blocktargetsize",BlockTargetSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," required at creation time : Block target size (user defined value) Foreseen medium size of blocks in a varying block context.");

  wReturn+=fmtXMLbool("highwatermarking",(bool)HighwaterMarking,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," required at creation time : highwater marking when blocks are deleted (overwritten with binary zeroes) ");
  wReturn+=fmtXMLbool("grabfreespace",(bool)GrabFreeSpace,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," required at creation time : option - when set forces to collect free space before block allocation ");

  wReturn+=fmtXMLlong("allocatedsize",AllocatedSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"  FYI: currently allocated space ");
  wReturn+=fmtXMLlong("usedsize",UsedSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"  FYI: currently used space  ");

  wReturn+=fmtXMLulong("minsize",MinSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," FYI:  stats : minimum block size ");
  wReturn+=fmtXMLulong("maxsize",MaxSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," FYI:  stats : maximum block size ");

  wReturn += fmtXMLendnode("zfilecontrolblock",pLevel);
  return wReturn;
} // ZRandomFile::toXml

int ZFileControlBlock::fromXml(zxmlNode* pFCBRootNode, ZaiErrors* pErrorlog)
{
  clear();
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  ZStatus wSt = pFCBRootNode->getChildByName((zxmlNode *&) wRootNode, "zfilecontrolblock");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZFileControlBlock::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "zfilecontrolblock",
        decode_ZStatus(wSt));
    return -1;
  }
/*
  if (XMLgetChildInt64(wRootNode, "startofdata", StartOfData, pErrorlog)< 0) {
    fprintf(stderr,
        "ZFileControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "current value.",
        "startofdata");
    }
*/
  if (XMLgetChildULong(wRootNode, "blockextentquota", BlockExtentQuota, pErrorlog)< 0) {
    fprintf(stderr,
        "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "current value.",
        "blockextentquota");
    }

/*
  if (XMLgetChildULong(wRootNode, "zbatdataoffset", ZBAT_DataOffset, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "zbatdataoffset");
    }
  if (XMLgetChildULong(wRootNode, "zbatexportsize", ZBAT_ExportSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "zbatexportsize");
    }
  if (XMLgetChildULong(wRootNode, "zfbtdataoffset", ZFBT_DataOffset, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "zfbtdataoffset");
    }
  if (XMLgetChildULong(wRootNode, "zfbtexportsize", ZFBT_ExportSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "zfbtexportsize");
    }
  if (XMLgetChildULong(wRootNode, "zdbtdataoffset", ZDBT_DataOffset, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "zdbtdataoffset");
    }
  if (XMLgetChildULong(wRootNode, "zdbtexportsize", ZDBT_ExportSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "zdbtexportsize");
    }
  if (XMLgetChildULong(wRootNode, "zreserveddataoffset", ZReserved_DataOffset, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "zreserveddataoffset");
    }
  if (XMLgetChildULong(wRootNode, "zreservedexportsize", ZReserved_ExportSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "zreservedexportsize");
    }
*/
  if (XMLgetChildULong(wRootNode, "initialsize", InitialSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "initialsize");
    }
  if (XMLgetChildULong(wRootNode, "allocatedsize", AllocatedSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "allocatedsize");
    }
  if (XMLgetChildULong(wRootNode, "usedsize", UsedSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "usedsize");
    }
/*
    if (XMLgetChildULong(wRootNode, "minsize", MinSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "minsize");
    }
    if (XMLgetChildULong(wRootNode, "maxsize", MaxSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "maxsize");
    }
*/
    if (XMLgetChildULong(wRootNode, "blocktargetsize", BlockTargetSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "blocktargetsize");
    }

  if (XMLgetChildBool(wRootNode, "highwatermarking", wBool, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "highwatermarking");
      }
      else
        HighwaterMarking =(uint8_t)wBool;

    if (XMLgetChildBool(wRootNode, "grabfreespace", wBool, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "current value.",
          "grabfreespace");
      }
      else
        GrabFreeSpace =(uint8_t)wBool;

  return (int)pErrorlog->hasError();
}//ZHeaderControlBlock::fromXml

