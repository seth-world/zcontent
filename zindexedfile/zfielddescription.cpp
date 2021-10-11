#include <zindexedfile/zfielddescription.h>
#include <zxml/zxml.h>
#include <zxml/zxmlprimitives.h>


ZFieldDescription&
ZFieldDescription::_copyFrom(ZFieldDescription& pIn)
{
  ZType=pIn.ZType;
  Capacity=pIn.Capacity;
  HeaderSize=pIn.HeaderSize;
  UniversalSize=pIn.UniversalSize;
  NaturalSize=pIn.NaturalSize;
  KeyEligible=pIn.KeyEligible;
  Name=pIn.Name;
  Hash = pIn.Hash;
  return *this;
}

void ZFieldDescription::setFieldName(const utf8String& pName)
{
  if (pName.strlen()>cst_fieldnamelen)
  {
    Name=pName.Left(cst_fieldnamelen-1);

    fprintf (stderr,"ZFieldDescription::setFieldName-W-ERRLEN field name <%s> is too long for meta dictionary naming standard.\n"
                    "Field name truncated to <%s>",
        _GET_FUNCTION_NAME_,
        pName.toCChar(),
        Name.toCChar());
  }
  else
    Name= pName;
  Hash= pName.getMD5();
}
/*
      <field>
        <name>%s</name>
        <!-- see ZType_type definition : converted to its value number -->
        <ztype>%s</ztype>
<!-- if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if atomic -->
        <capacity>%d</capacity>
        <headersize>%ld</headersize>
        <universalsize>%ld</universalsize>
        <naturalsize>%ld</naturalsize>
        <keyelibible>true</keyelibible>
        <hash>%s</hash>

      </field>

*/

utf8String ZFieldDescription::toXml(int pLevel, bool pComment)
{
  int wLevel=pLevel;
  utf8String wReturn;

  wReturn = fmtXMLnode("field",pLevel);
  wLevel++;
  wReturn+=fmtXMLchar("name",Name.toCChar(),wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," Name of the dictionary field : it must be unique.");

  wReturn+=fmtXMLuint32("ztype",  ZType,wLevel);
  if (pComment)
    {
    utf8String wZTStr;
    wZTStr.sprintf(" ZType_type <%s> converted to its value number",decode_ZType(ZType));
    fmtXMLaddInlineComment(wReturn,wZTStr.toCChar());
    }
  wReturn+=fmtXMLuint("capacity",Capacity,wLevel);  /* uint16 must be casted */
  if (pComment)
    fmtXMLaddInlineComment(wReturn," if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if atomic");
  wReturn+=fmtXMLuint64("headersize",HeaderSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," size of field header");
  wReturn+=fmtXMLuint64("universalsize",UniversalSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," Only if ZType is fixed length. Otherwise set to 0");
  wReturn+=fmtXMLuint64("naturalsize",NaturalSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," Only if ZType is fixed length. Otherwise set to 0");
  wReturn+=fmtXMLbool("keyelibible",KeyEligible,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," May be used as Key field (true) or not (false)");
  wReturn+=fmtXMLmd5("hash",Hash,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," unique hashcode value for the field.");

  wReturn += fmtXMLendnode("field",pLevel);
  return wReturn;
}//toXml

ZStatus ZFieldDescription::fromXml(zxmlNode* pRootNode,ZaiErrors* pErrorlog,ZaiE_Severity pSeverity)
{
  bool        wErrored=false;
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utf8String wName;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pRootNode->getChildByName((zxmlNode *&) wRootNode, "field");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        pSeverity,
        ZS_XMLINVROOTNAME,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find root node <%s>. Stopping xml parsing.",
        "field");
    return ZS_XMLINVROOTNAME;
    }
  if (XMLgetChildText(wRootNode, "name", wName, pErrorlog) < 0) {
    pErrorlog->logZStatus(ZAIES_Error,ZS_XMLMISSREQ,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find field %s .",
        "name");
    wErrored=true;
    }
  else
    Name=wName.toCChar();

  if (XMLgetChildMd5(wRootNode, "hash", Hash, pErrorlog) < 0) {
    if (wName.isEmpty())
    {
      pErrorlog->logZStatus(ZAIES_Error,ZS_XMLMISSREQ,
          "FieldDescription::fromXml-E-CNTFINDPAR Cannot find node <hash> while node <name> is missing.");
      return ZS_XMLMISSREQ;
    }
    else
      {
      Hash=wName.getMD5();
      pErrorlog->warningLog(
          "FieldDescription::fromXml-W-CNTFINDNOD Cannot find node <hash>. Recomputing name's hash : value is <%s>.",
          Hash.toHexa().toChar());
      }
    }


  if (XMLgetChildUInt(wRootNode, "ztype", wInt, pErrorlog)< 0) {
    pErrorlog->warningLog(
        "FieldDescription::fromXml-E-CNTFINDNOD Cannot find node <%s>.",
        "ztype");
  }
  else
    ZType = (ZTypeBase)wInt;

  if (XMLgetChildUInt(wRootNode, "capacity", wInt, pErrorlog)< 0) {
    pErrorlog->warningLog(
        "FieldDescription::fromXml-E-CNTFINDNOD Cannot find node <%s>.",
        "capacity");
  }
  else
    Capacity = (uint16_t)wInt;


  if (XMLgetChildULong(wRootNode, "headersize", HeaderSize, pErrorlog)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDNOD Cannot find node <%s>.",
        "headersize");
    wErrored=true;
  }
  if (XMLgetChildULong(wRootNode, "universalsize", UniversalSize, pErrorlog)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s.",
        "universalsize");
    wErrored=true;
  }
  if (XMLgetChildULong(wRootNode, "naturalsize", NaturalSize, pErrorlog)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s.",
        "naturalsize");
    wErrored=true;
  }


  if (XMLgetChildBool(wRootNode, "keyelibible", wBool, pErrorlog,ZAIES_Error)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. ",
        "keyelibible");
    wErrored=true;
  }
  else
    KeyEligible = wBool;


  return wErrored?ZS_XMLMISSREQ:ZS_SUCCESS;
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
