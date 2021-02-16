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
  StartSign=cst_ZSTART;
  StartOfData = 0L;
  BlockID = ZBID_FCB;
  EndSign=cst_ZEND;
  return;
}

ZFileControlBlock& ZFileControlBlock::_copyFrom(const ZFileControlBlock& pIn)
{
  StartSign=pIn.StartSign;
  BlockID=pIn.BlockID;
  StartOfData=pIn.StartOfData;
  AllocatedBlocks=pIn.AllocatedBlocks;
  BlockExtentQuota=pIn.BlockExtentQuota;
  ZBAT_DataOffset=pIn.ZBAT_DataOffset;
  ZBAT_ExportSize=pIn.ZBAT_ExportSize;

  ZFBT_DataOffset=pIn.ZFBT_DataOffset;
  ZFBT_ExportSize=pIn.ZFBT_ExportSize;

  ZDBT_DataOffset=pIn.ZDBT_DataOffset;
  ZDBT_ExportSize=pIn.ZDBT_ExportSize;

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
  EndSign=pIn.EndSign;
  return *this;
}





//=========================== ZFileControlBlock Export import==========================================

ZDataBuffer&
ZFileControlBlock::_export(ZDataBuffer& pZDBExport)
{
  ZFileControlBlock_Export wFCB;
  wFCB.StartSign=StartSign;// don't care reversing start sign or end sign : same as reversed
  wFCB.BlockID=BlockID;

  wFCB.StartOfData=reverseByteOrder_Conditional<zaddress_type>(StartOfData);
  wFCB.AllocatedBlocks=reverseByteOrder_Conditional<unsigned long>(AllocatedBlocks);
  wFCB.BlockExtentQuota=reverseByteOrder_Conditional<unsigned long>(BlockExtentQuota);
  wFCB.ZBAT_DataOffset=reverseByteOrder_Conditional<size_t>(ZBAT_DataOffset);
  wFCB.ZBAT_ExportSize=reverseByteOrder_Conditional<size_t>(ZBAT_ExportSize);
  wFCB.ZFBT_DataOffset=reverseByteOrder_Conditional<size_t>(ZFBT_DataOffset);
  wFCB.ZFBT_ExportSize=reverseByteOrder_Conditional<size_t>(ZFBT_ExportSize);
  wFCB.ZDBT_DataOffset=reverseByteOrder_Conditional<size_t>(ZDBT_DataOffset);
  wFCB.ZDBT_ExportSize=reverseByteOrder_Conditional<size_t>(ZDBT_ExportSize);
  wFCB.ZReserved_DataOffset=reverseByteOrder_Conditional<size_t>(ZReserved_DataOffset);
  wFCB.ZReserved_ExportSize=reverseByteOrder_Conditional<size_t>(ZReserved_ExportSize);
  wFCB.InitialSize=reverseByteOrder_Conditional<zsize_type>(InitialSize);
  wFCB.AllocatedSize=reverseByteOrder_Conditional<zsize_type>(AllocatedSize);
  wFCB.UsedSize=reverseByteOrder_Conditional<zsize_type>(UsedSize);
  wFCB.MinSize=reverseByteOrder_Conditional<size_t>(MinSize);
  wFCB.MaxSize=reverseByteOrder_Conditional<size_t>(MaxSize);
  wFCB.BlockTargetSize=reverseByteOrder_Conditional<size_t>(BlockTargetSize);
  wFCB.HighwaterMarking=HighwaterMarking;
  wFCB.GrabFreeSpace=GrabFreeSpace;
  wFCB.EndSign=EndSign;// don't care reversing start sign or end sign : same as reversed
  pZDBExport.setData(&wFCB,sizeof(wFCB));

  return pZDBExport;
}// ZFileControlBlock::_export

ZFileControlBlock&
ZFileControlBlock::_import(unsigned char* pZDBImport_Ptr)
{
  ZFileControlBlock_Export* wFCB=(ZFileControlBlock_Export*) pZDBImport_Ptr;

  //    StartSign=_reverseByteOrder_T<uint32_t>(wFCB->StartSign);
  StartSign=wFCB->StartSign;

  BlockID=wFCB->BlockID;

  StartOfData=wFCB->StartOfData;// don't care reversing start sign or end sign : same as reversed
  AllocatedBlocks=reverseByteOrder_Conditional<unsigned long>(wFCB->AllocatedBlocks);
  BlockExtentQuota=reverseByteOrder_Conditional<unsigned long>(wFCB->BlockExtentQuota);
  ZBAT_DataOffset=reverseByteOrder_Conditional<size_t>(wFCB->ZBAT_DataOffset);
  ZBAT_ExportSize=reverseByteOrder_Conditional<size_t>(wFCB->ZBAT_ExportSize);
  ZFBT_DataOffset=reverseByteOrder_Conditional<size_t>(wFCB->ZFBT_DataOffset);
  ZFBT_ExportSize=reverseByteOrder_Conditional<size_t>(wFCB->ZFBT_ExportSize);
  ZDBT_DataOffset=reverseByteOrder_Conditional<size_t>(wFCB->ZDBT_DataOffset);
  ZDBT_ExportSize=reverseByteOrder_Conditional<size_t>(wFCB->ZDBT_ExportSize);
  ZReserved_DataOffset=reverseByteOrder_Conditional<size_t>(wFCB->ZReserved_DataOffset);
  ZReserved_ExportSize=reverseByteOrder_Conditional<size_t>(wFCB->ZReserved_ExportSize);
  InitialSize=reverseByteOrder_Conditional<zsize_type>(wFCB->InitialSize);
  AllocatedSize=reverseByteOrder_Conditional<zsize_type>(wFCB->AllocatedSize);
  UsedSize=reverseByteOrder_Conditional<zsize_type>(wFCB->UsedSize);
  MinSize=reverseByteOrder_Conditional<size_t>(wFCB->MinSize);
  MaxSize=reverseByteOrder_Conditional<size_t>(wFCB->MaxSize);
  BlockTargetSize=reverseByteOrder_Conditional<size_t>(wFCB->BlockTargetSize);
  HighwaterMarking=wFCB->HighwaterMarking;
  GrabFreeSpace=wFCB->GrabFreeSpace;
  //    EndSign=_reverseByteOrder_T<uint32_t>(wFCB->EndSign);
  EndSign=wFCB->EndSign;// don't care reversing start sign or end sign : same as reversed

  return *this;
}// ZFileControlBlock::_import


//-------------xml export import ----------------------------

utf8String ZFileControlBlock::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("zfilecontrolblock",pLevel);
  wLevel++;

  /* NB: StartSign and BlockID are not exported to xml */
  wReturn+=fmtXMLint64("startofdata",StartOfData,wLevel);

  wReturn+=fmtXMLulong("allocatedblocks",AllocatedBlocks,wLevel);
  wReturn+=fmtXMLulong("blockextentquota",BlockExtentQuota,wLevel);

  wReturn+=fmtXMLulong("zbatdataoffset",ZBAT_DataOffset,wLevel);
  wReturn+=fmtXMLulong("zbatexportsize",ZBAT_ExportSize,wLevel);

  wReturn+=fmtXMLulong("zfbtdataoffset",ZFBT_DataOffset,wLevel);
  wReturn+=fmtXMLulong("zfbtexportsize",ZFBT_ExportSize,wLevel);

  wReturn+=fmtXMLulong("zdbtdataoffset",ZDBT_DataOffset,wLevel);
  wReturn+=fmtXMLulong("zdbtexportsize",ZDBT_ExportSize,wLevel);

  wReturn+=fmtXMLulong("zreserveddataoffset",ZReserved_DataOffset,wLevel);
  wReturn+=fmtXMLulong("zreservedexportsize",ZReserved_ExportSize,wLevel);

  wReturn+=fmtXMLlong("initialsize",InitialSize,wLevel);
  wReturn+=fmtXMLlong("allocatedsize",AllocatedSize,wLevel);
  wReturn+=fmtXMLlong("usedsize",UsedSize,wLevel);

  wReturn+=fmtXMLulong("minsize",MinSize,wLevel);
  wReturn+=fmtXMLulong("maxsize",MaxSize,wLevel);
  wReturn+=fmtXMLulong("blocktargetsize",BlockTargetSize,wLevel);

  wReturn+=fmtXMLbool("highwatermarking",(bool)HighwaterMarking,wLevel);
  wReturn+=fmtXMLbool("grabfreespace",(bool)GrabFreeSpace,wLevel);

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
  unsigned int wInt;
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

  if (XMLgetChildInt64(wRootNode, "startofdata", StartOfData, pErrorlog)< 0) {
    fprintf(stderr,
        "ZFileControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "startofdata");
    }
  if (XMLgetChildULong(wRootNode, "blockextentquota", BlockExtentQuota, pErrorlog)< 0) {
    fprintf(stderr,
        "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "blockextentquota");
    }

  if (XMLgetChildULong(wRootNode, "zbatdataoffset", ZBAT_DataOffset, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "zbatdataoffset");
    }
  if (XMLgetChildULong(wRootNode, "zbatexportsize", ZBAT_ExportSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "zbatexportsize");
    }
  if (XMLgetChildULong(wRootNode, "zfbtdataoffset", ZFBT_DataOffset, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "zfbtdataoffset");
    }
  if (XMLgetChildULong(wRootNode, "zfbtexportsize", ZFBT_ExportSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "zfbtexportsize");
    }
  if (XMLgetChildULong(wRootNode, "zdbtdataoffset", ZDBT_DataOffset, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "zdbtdataoffset");
    }
  if (XMLgetChildULong(wRootNode, "zdbtexportsize", ZDBT_ExportSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "zdbtexportsize");
    }
  if (XMLgetChildULong(wRootNode, "zreserveddataoffset", ZReserved_DataOffset, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "zreserveddataoffset");
    }
  if (XMLgetChildULong(wRootNode, "zreservedexportsize", ZReserved_ExportSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "zreservedexportsize");
    }
  if (XMLgetChildULong(wRootNode, "initialsize", InitialSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "initialsize");
    }
  if (XMLgetChildULong(wRootNode, "allocatedsize", AllocatedSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "allocatedsize");
    }
  if (XMLgetChildULong(wRootNode, "usedsize", UsedSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "usedsize");
    }
    if (XMLgetChildULong(wRootNode, "minsize", MinSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "minsize");
    }
    if (XMLgetChildULong(wRootNode, "maxsize", MaxSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "maxsize");
    }
    if (XMLgetChildULong(wRootNode, "blocktargetsize", BlockTargetSize, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "blocktargetsize");
    }

  if (XMLgetChildBool(wRootNode, "highwatermarking", wBool, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "highwatermarking");
      }
      else
        HighwaterMarking =(uint8_t)wBool;

    if (XMLgetChildBool(wRootNode, "grabfreespace", wBool, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "grabfreespace");
      }
      else
        GrabFreeSpace =(uint8_t)wBool;

  return (int)pErrorlog->hasError();
}//ZHeaderControlBlock::fromXml

