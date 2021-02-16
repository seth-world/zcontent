#include "zindexfield.h"


ZSIndexField& ZSIndexField::_copyFrom(ZSIndexField& pIn)
{
  MDicRank=pIn.MDicRank;
  NaturalSize=pIn.NaturalSize;
  UniversalSize=pIn.UniversalSize;
  ArrayCount=pIn.ArrayCount;
  ZType=pIn.ZType;
  KeyOffset=pIn.KeyOffset;
  return *this;
}
void ZSIndexField::clear()
{
  MDicRank=0;
  NaturalSize=0;
  UniversalSize=0;
  ArrayCount=0;
  ZType=0;
  KeyOffset=0;
  return;
}

ZSIndexField_strOut
ZSIndexField::_exportConvert(ZSIndexField& pIn,ZSIndexField_strOut* pOut)
{

  memset(pOut,0,sizeof(ZSIndexField_strOut));
  pOut->MDicRank=reverseByteOrder_Conditional<size_t>(pIn.MDicRank);
  pOut->NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn.NaturalSize);
  pOut->UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn.UniversalSize);
  pOut->ArrayCount=reverseByteOrder_Conditional<uint32_t>(pIn.ArrayCount);
  pOut->ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn.ZType);
  return *pOut;
}
ZSIndexField
ZSIndexField::_importConvert(ZSIndexField& pOut,ZSIndexField_strOut* pIn)
{

  memset(&pOut,0,sizeof(ZSIndexField));
  pOut.MDicRank=reverseByteOrder_Conditional<size_t>(pIn->MDicRank);
  pOut.NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn->NaturalSize);
  pOut.UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn->UniversalSize);
  pOut.ArrayCount=reverseByteOrder_Conditional<uint32_t>(pIn->ArrayCount);
  pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
  return pOut;
}

ZDataBuffer&
ZSIndexField::_export(ZDataBuffer& pZDBExport)
{
  ZSIndexField wIFS;
  memset(&wIFS,0,sizeof(ZSIndexField));
  wIFS.MDicRank=reverseByteOrder_Conditional<size_t>(MDicRank);
  wIFS.NaturalSize=reverseByteOrder_Conditional<uint64_t>(NaturalSize);
  wIFS.UniversalSize=reverseByteOrder_Conditional<uint64_t>(UniversalSize);
  wIFS.KeyOffset=reverseByteOrder_Conditional<uint64_t>(KeyOffset);
  wIFS.ArrayCount=reverseByteOrder_Conditional<uint32_t>(ArrayCount);
  wIFS.ZType=reverseByteOrder_Conditional<ZTypeBase>(ZType);

  //    wIFS.RecordOffset=_reverseByteOrder_T<uint64_t>(RecordOffset);
  //    wIFS.Name=Name;
  pZDBExport.setData(&wIFS,sizeof(wIFS));
  return pZDBExport;
}
ZSIndexField&
ZSIndexField::_import(unsigned char* pZDBImport_Ptr)
{
  ZSIndexField* wIFS=(ZSIndexField*) pZDBImport_Ptr;
  memset(this,0,sizeof(ZSIndexField));
  MDicRank=reverseByteOrder_Conditional<size_t>(wIFS->MDicRank);
  NaturalSize=reverseByteOrder_Conditional<uint64_t>(wIFS->NaturalSize);
  UniversalSize=reverseByteOrder_Conditional<uint64_t>(wIFS->UniversalSize);
  KeyOffset=reverseByteOrder_Conditional<uint64_t>(wIFS->KeyOffset);
  ArrayCount=reverseByteOrder_Conditional<uint32_t>(wIFS->ArrayCount);
  ZType=reverseByteOrder_Conditional<ZTypeBase>(wIFS->ZType);
  //    RecordOffset=_reverseByteOrder_T<uint64_t>(wIFS->RecordOffset);
  //    Name=wIFS->Name;  // Name is a descString
  return *this;
}


utf8String ZSIndexField::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("zindexfield",pLevel);
  wLevel++;
  wReturn+=fmtXMLuint64("mdicrank",MDicRank,wLevel);
  wReturn+=fmtXMLuint64("naturalsize",NaturalSize,wLevel);
  wReturn+=fmtXMLuint64("universalsize",UniversalSize,wLevel);

  wReturn+=fmtXMLint("arraycount",ArrayCount,wLevel); /* uint32_t*/
  wReturn+=fmtXMLint("ztype",ZType,wLevel);           /* ZTypeBase = uint32_t*/
  wReturn+=fmtXMLuint64("keyoffset",KeyOffset,wLevel);

  wReturn += fmtXMLendnode("zindexfield",pLevel);
  return wReturn;
} // ZSIndexField_struct::toXml

int ZSIndexField::fromXml(zxmlElement* pFieldNode, ZaiErrors* pErrorlog)
{
  //  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  uint64_t wI64Value;
  bool wBool;
  unsigned int wInt;
  if (pFieldNode->getName()!="zindexfield")
  {
    pErrorlog->errorLog(
        "ZSIndexField::fromXml-E-CNTFINDND Error invalid root node element name <%s>, expected <%s>",
        pFieldNode->getName().toCChar(),
        "indexfield");
    return -1;
  }
  if (XMLgetChildULong(pFieldNode, "mdicrank", MDicRank, pErrorlog) < 0) {
    fprintf(stderr,
        "ZSIndexField::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default. Field skipped.",
        "mdicrank");
  }
  if (XMLgetChildULong(pFieldNode, "naturalsize", NaturalSize, pErrorlog) < 0) {
    fprintf(stderr,
        "ZSIndexField::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.Field skipped.",
        "naturalsize");
  }
  if (XMLgetChildULong(pFieldNode, "universalsize", UniversalSize, pErrorlog) < 0) {
    fprintf(stderr,
        "ZSIndexField::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.Field skipped.",
        "universalsize");
  }
  if (XMLgetChildUInt(pFieldNode, "arraycount", wInt, pErrorlog) < 0) {
    fprintf(stderr,
        "ZSIndexField::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.Field skipped.",
        "arraycount");
  }
  else
    ArrayCount=(uint32_t)wInt;

  if (XMLgetChildUInt(pFieldNode, "ztype", wInt, pErrorlog) < 0) {
    fprintf(stderr,
        "ZSIndexField::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.Field skipped.",
        "ztype");
  }
  else
    ZType=(ZTypeBase)wInt;

  if (XMLgetChildULong(pFieldNode, "keyoffset", KeyOffset, pErrorlog) < 0) {
    fprintf(stderr,
        "ZSIndexField::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.Field skipped.",
        "keyoffset");
  }

  return (int)pErrorlog->hasError();
}//ZSIndexField::fromXml
