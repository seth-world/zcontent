#include "zfielddescription.h"


utf8String ZFieldDescription::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("fielddescription",pLevel);
  wLevel++;
  wReturn+=fmtXMLchar("name",Name.toCChar(),wLevel);

  wReturn+=fmtXMLuint32("ztype",  ZType,wLevel);
  wReturn+=fmtXMLuint("capacity",Capacity,wLevel);  /* uint16 must be casted */
  wReturn+=fmtXMLuint64("headersize",HeaderSize,wLevel);
  wReturn+=fmtXMLuint64("universalsize",UniversalSize,wLevel);
  wReturn+=fmtXMLuint64("naturalsize",NaturalSize,wLevel);
  wReturn+=fmtXMLbool("keyelibible",KeyEligible,wLevel);

  wReturn += fmtXMLendnode("fielddescription",pLevel);
  return wReturn;
}//toXml
int ZFieldDescription::fromXml(zxmlNode* pRootNode,ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pRootNode->getChildByName((zxmlNode *&) wRootNode, "fielddescription");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "FieldDescription::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>. Field skipped.",
        "fielddescription",
        decode_ZStatus(wSt));
    return -1;
  }
  if (XMLgetChildText(wRootNode, "name", wValue, pErrorlog) < 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default. Field skipped.",
        "name");
  }
  else
    Name=wValue.toCChar();

  if (XMLgetChildUInt(wRootNode, "ztype", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default. Field skipped.",
        "ztype");
  }
  else
    ZType = (ZTypeBase)wInt;

  if (XMLgetChildUInt(wRootNode, "capacity", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default. Field skipped.",
        "capacity");
  }
  else
    Capacity = (uint16_t)wInt;


  if (XMLgetChildULong(wRootNode, "headersize", HeaderSize, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default. Field skipped.",
        "headersize");
  }
  if (XMLgetChildULong(wRootNode, "universalsize", UniversalSize, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default. Field skipped.",
        "universalsize");
  }
  if (XMLgetChildULong(wRootNode, "naturalsize", NaturalSize, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default. Field skipped.",
        "naturalsize");
  }


  if (XMLgetChildBool(wRootNode, "keyelibible", wBool, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default. Field skipped.",
        "backupencrypted");
  }
  else
    KeyEligible = wBool;
  return (int)pErrorlog->hasError();
}//fromXml

/** @cond Development
 * @brief fieldDesc_struct::_export exports a field description (a rank of ZKeyDictionary) to a out structure
 */
FieldDesc_Export
ZFieldDescription::_exportConvert(ZFieldDescription&pIn,FieldDesc_Export* pOut)
{
  ZDataBuffer wZDBName;

  pOut->ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn.ZType);
  pOut->ArrayCount=reverseByteOrder_Conditional<URF_Array_Count_type>(pIn.Capacity);
  pOut->HeaderSize=reverseByteOrder_Conditional<uint64_t>(pIn.HeaderSize);
  pOut->UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn.UniversalSize);
  pOut->NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn.NaturalSize);

  memset (pOut->Name,0,cst_fieldnamelen+1);
  wZDBName=pIn.Name._exportUVF();
  memmove(pOut->Name,wZDBName.Data,wZDBName.Size);

  pOut->KeyEligible = pIn.KeyEligible;
  return *pOut;
}//fieldDesc_struct::_exportConvert
/**
 * @brief fieldDesc_struct::_import imports a field description (a rank of ZKeyDictionary) from a out structure
 * @param pOut
 * @return
 */
ZFieldDescription
ZFieldDescription::_importConvert(ZFieldDescription& pOut,FieldDesc_Export* pIn)
{
  pOut.clear();
  pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
  pOut.Capacity=reverseByteOrder_Conditional<URF_Array_Count_type>(pIn->ArrayCount);
  pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
  pOut.HeaderSize=reverseByteOrder_Conditional<uint64_t>(pIn->HeaderSize);
  pOut.NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn->NaturalSize);
  pOut.UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn->UniversalSize);
  unsigned char* wPtrName=(unsigned char*)pIn->Name;
  pOut.Name._importUVF(wPtrName);

  pOut.KeyEligible = pIn->KeyEligible;
  return pOut;
}//fieldDesc_struct::_import
