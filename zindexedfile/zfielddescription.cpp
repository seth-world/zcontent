#include <zindexedfile/zfielddescription.h>
#include <zxml/zxml.h>
#include <zxml/zxmlprimitives.h>

#include <zindexedfile/zindexfield.h> // for hash comparizon (KeyField_Pack)

void ZFieldDescription::clear()
{
  Name.clear();
  ZType=ZType_Unknown;
  HeaderSize=0;
  Capacity=0;
  UniversalSize=0;
  NaturalSize=0;
  KeyEligible=false;
  Hash.clear();
  ToolTip.clear();
}

ZFieldDescription&
ZFieldDescription::_copyFrom(const ZFieldDescription& pIn)
{
  ZType=pIn.ZType;
  Capacity=pIn.Capacity;
  HeaderSize=pIn.HeaderSize;
  UniversalSize=pIn.UniversalSize;
  NaturalSize=pIn.NaturalSize;
  KeyEligible=pIn.KeyEligible;
  Name=pIn.Name;
  Hash = pIn.Hash;
  ToolTip=pIn.ToolTip;
  return *this;
}

void ZFieldDescription::setFieldName(const utf8String& pName)
{
  if (pName.strlen()>=cst_fieldnamelen)
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
} // setFieldName

bool ZFieldDescription::hasName(const utf8VaryingString& pName) const { return Name==pName.toCChar(); }

bool ZFieldDescription::hasHashCode (const md5& pHashcode) const {return (Hash==pHashcode);}

bool ZFieldDescription::checkHashcode()
{
  if (Hash.isInvalid())
    return false;

  md5 wNewHash = _computeMd5();

  for (int wi=0;wi < cst_md5 ; wi++)
    if (wNewHash.content[wi]!=Hash.content[wi])
      return false;
  return true;
}//checkHashcode

md5 ZFieldDescription::computeMd5()
{
  FieldDesc_Check wFDCheck;
  /*
  fprintf (stderr,"ZFieldDescription::computeMd5-I-  name <%s>\n"
                  "ZType <%08X>  header <%ld> Natural <%ld> Universal <%ld>.\n",
                  getName().toCChar(),ZType,HeaderSize,NaturalSize,UniversalSize);
*/
  wFDCheck.set(*this);
  Hash.clear();
  Hash.compute((unsigned char*)&wFDCheck,sizeof(FieldDesc_Check));
  fprintf (stderr,"ZFieldDescription::computeMd5-I-  md5 <%s>\n",Hash.toHexa().toCChar());
  std::cerr.flush();
  return Hash;
} // computeMd5

md5 ZFieldDescription::_computeMd5()
{
  FieldDesc_Check wFDCheck;
  wFDCheck.set(*this);
  md5 wHash;
  wHash.compute((unsigned char*)&wFDCheck,sizeof(FieldDesc_Check));
  return wHash;
} // _computeMd5

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

utf8VaryingString ZFieldDescription::toXml(int pLevel, bool pComment)
{
  int wLevel=pLevel;
  utf8VaryingString wReturn;

  wReturn = fmtXMLnode("field",pLevel);
  wLevel++;
  if (Name.isEmpty())
    wReturn += fmtXMLcomment("/name/ is missing",wLevel);
  else
    wReturn += fmtXMLchar("name",Name.toCChar(),wLevel);
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
  if (ToolTip.isEmpty())
    wReturn += fmtXMLcomment("/tooltip/ is missing",wLevel);
  else {
    wReturn += fmtXMLchar("tooltip",ToolTip.toCChar(),wLevel);
    if (pComment)
      fmtXMLaddInlineComment(wReturn,"Custom help");
  }
  wReturn += fmtXMLendnode("field",pLevel);
  return wReturn;
}//toXml

ZStatus ZFieldDescription::fromXml(zxmlNode* pFieldRootNode, bool pCheckHash,int &pErrored,int &pWarned, ZaiErrors* pErrorlog)
{
  bool        wErrored=false, wWarned=false;
  bool        wHashismissing=false;
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utf8String wName;
  bool wBool;
  unsigned int wInt;

  if (pFieldRootNode->getName()!="field")
    {
    pErrorlog->logZStatus(ZAIES_Error,
                          ZS_XMLINVROOTNAME,
                          "FieldDescription::fromXml-E-CNTFINDPAR Cannot find root node <%s>. Stopping xml parsing.",
                          "field");
    pErrored++;
    return ZS_XMLINVROOTNAME;
    }

  wRootNode = (zxmlElement *)pFieldRootNode;

  if (XMLgetChildText(wRootNode, "name", wName, pErrorlog) < 0) {
    pErrorlog->logZStatus(ZAIES_Error,ZS_XMLMISSREQ,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find field %s .",
        "name");
    wErrored=true;
    }
  else
    Name=wName.toCChar();
    /*optional */
    if (XMLgetChildText(wRootNode, "tooltip", wName, pErrorlog,ZAIES_Warning) < 0) {
        pErrorlog->warningLog(
          "FieldDescription::fromXml-W-MISSTOOLTIP node <Tooltip> is missing for field named %s.",
          Name.toCChar());
        wWarned=true;
    }
    else
      ToolTip=wName.toCChar();
/* optional */
  if (XMLgetChildMd5(wRootNode, "hash", Hash, pErrorlog,ZAIES_Warning) < 0)
    {
      pErrorlog->warningLog(
        "FieldDescription::fromXml-W-CNTFINDHASH Cannot find node <hash> for field named <%s>. Hashcode will be recomputed.",
        Name.toCChar());
      wHashismissing=true;
      wWarned=true;
    }

  if (XMLgetChildUInt(wRootNode, "ztype", wInt, pErrorlog,ZAIES_Error)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDNOD Cannot find node <%s>.",
        "ztype");
    wErrored=true;
  }
  else
    ZType = (ZTypeBase)wInt;

  if (XMLgetChildUInt(wRootNode, "capacity", wInt, pErrorlog,ZAIES_Error)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDNOD Cannot find node <%s>.",
        "capacity");
    wErrored=true;
  }
  else
    Capacity = (uint16_t)wInt;


  if (XMLgetChildULong(wRootNode, "headersize", HeaderSize, pErrorlog,ZAIES_Error)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDNOD Cannot find node <%s>.",
        "headersize");
    wErrored=true;
  }
  if (XMLgetChildULong(wRootNode, "universalsize", UniversalSize, pErrorlog,ZAIES_Error)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s.",
        "universalsize");
  }
  if (XMLgetChildULong(wRootNode, "naturalsize", NaturalSize, pErrorlog,ZAIES_Error)< 0) {
    pErrorlog->errorLog(
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s.",
        "naturalsize");
    wErrored=true;
  }

/* optional */
  if (XMLgetChildBool(wRootNode, "keyelibible", wBool, pErrorlog,ZAIES_Warning)< 0) {
    pErrorlog->warningLog(
        "FieldDescription::fromXml-W-CNTFINDPAR Cannot find parameter %s. ",
        "keyelibible");
    wWarned=true;
  }
  else
    KeyEligible = wBool;

  if ((!wErrored) && wHashismissing)
    {
    computeMd5();
    pErrorlog->infoLog(
        "FieldDescription::fromXml-I-RECOMPHASH Field <%s> : recomputing field's hashcode : value is <%s>.",
        Name.toCChar(),
        Hash.toHexa().toCChar());
    }
    else if ((!wErrored) && pCheckHash)
    {
      ZFieldDescription wFComp(*this);
      wFComp.computeMd5();
      if (wFComp.Hash != Hash)
      {
        pErrorlog->warningLog(
            "FieldDescription::fromXml-W-CHECKHASH Field <%s> : recomputed field's hashcode <%s> is not what has been loaded <%s>.",
            Name.toCChar(),
            wFComp.Hash.toHexa().toCChar(),
            Hash.toHexa().toCChar());
      }
    }//else if
  if (wErrored) {
    pErrored++;
    return ZS_XMLMISSREQ;
  }
  if (wWarned) {
    pWarned++;
    return ZS_XMLWARNING;
  }


  return ZS_SUCCESS;
}//fromXml

/** @cond Development
 * @brief fieldDesc_struct::_export exports a field description (a rank of ZKeyDictionary) to a out structure
 */
FieldDesc_Export
ZFieldDescription::_exportConvert(ZFieldDescription&pIn,FieldDesc_Export* pOut)
{
//  ZDataBuffer wZDBName;

  pOut->ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn.ZType);
  pOut->Capacity=reverseByteOrder_Conditional<URF_Capacity_type>(pIn.Capacity);
  pOut->HeaderSize=uint16_t(reverseByteOrder_Conditional<uint64_t>(pIn.HeaderSize));
  pOut->UniversalSize=uint32_t(reverseByteOrder_Conditional<uint64_t>(pIn.UniversalSize));
  pOut->NaturalSize=uint32_t(reverseByteOrder_Conditional<uint64_t>(pIn.NaturalSize));
 /*
  pIn.Name._exportUVFPtr((unsigned char*)pOut->Name,cst_FieldNameCapacity);

  memset (pOut->Name,0,cst_fieldnamelen+1);
  wZDBName=pIn.Name._exportUVF();
  memmove(pOut->Name,wZDBName.Data,wZDBName.Size);
*/
  pOut->KeyEligible = pIn.KeyEligible;
  return *pOut;
}//fieldDesc_struct::_exportConvert



FieldDesc_Export
ZFieldDescription::getFDExp()
{
  FieldDesc_Export pOut;
//  ZDataBuffer wZDBName;

  pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(ZType);
  pOut.Capacity=reverseByteOrder_Conditional<URF_Capacity_type>(Capacity);
  pOut.HeaderSize=reverseByteOrder_Conditional(HeaderSize);
  pOut.UniversalSize=reverseByteOrder_Conditional(UniversalSize);
  pOut.NaturalSize=reverseByteOrder_Conditional(NaturalSize);
/*
  memset (pOut.Name,0,cst_fieldnamelen+1);
  wZDBName=Name._exportUVF();
  memmove(pOut.Name,wZDBName.Data,wZDBName.Size);
*/
  pOut.KeyEligible = KeyEligible;
  return pOut;
}//fieldDesc_struct::_exportConvert

void FieldDesc_Export::setFromPtr(const unsigned char* &pPtrIn)
{
  FieldDesc_Export* wFLDe=(FieldDesc_Export*)pPtrIn;

  StartSign= wFLDe->StartSign;
  EndianCheck= wFLDe->EndianCheck;
  ZType = wFLDe->ZType;

  Capacity = wFLDe->Capacity; /* FileType does not need to be reversed */
  HeaderSize = wFLDe->HeaderSize;
  NaturalSize = wFLDe->NaturalSize;
  NaturalSize = wFLDe->NaturalSize;
  UniversalSize = wFLDe->UniversalSize;
  KeyEligible = wFLDe->KeyEligible;
  memmove(Hash,wFLDe->Hash,cst_md5);
  pPtrIn += sizeof(FieldDesc_Export);
  return ;
}



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
  pOut.Capacity=reverseByteOrder_Conditional<URF_Capacity_type>(pIn->Capacity);
  pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
  pOut.HeaderSize=reverseByteOrder_Conditional<uint64_t>(pIn->HeaderSize);
  pOut.NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn->NaturalSize);
  pOut.UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn->UniversalSize);
  memset(pOut.Hash.content,0,sizeof (pOut.Hash.content));
  memmove(pOut.Hash.content,pIn->Hash,cst_md5);
/*  const unsigned char* wPtrName=(const unsigned char*)pIn->Name;
  pOut.Name._importUVF(wPtrName);
*/
  pOut.KeyEligible = pIn->KeyEligible;
  return pOut;
}//fieldDesc_struct::_import


FieldDesc_Export&
FieldDesc_Export::_copyFrom(const FieldDesc_Export &pIn)
{
/*  for (int wi=0; wi < cst_FieldNameCapacity;wi++)
    Name[wi]=pIn.Name[wi];
*/
  StartSign=pIn.StartSign;
  EndianCheck=pIn.EndianCheck;
  ZType=pIn.ZType;
  Capacity=pIn.Capacity;
  HeaderSize=pIn.HeaderSize;
  UniversalSize=pIn.UniversalSize;
  NaturalSize=pIn.NaturalSize;
  KeyEligible=pIn.KeyEligible;
  memmove(Hash,pIn.Hash,cst_md5);
  return *this;
}
ZFieldDescription FieldDesc_Export::toFieldDescription()
{
  ZFieldDescription pFD;
  pFD.ZType=ZType;
  pFD.Capacity=Capacity;
  pFD.HeaderSize=HeaderSize;
  pFD.UniversalSize=UniversalSize;
  pFD.NaturalSize=NaturalSize;
  pFD.KeyEligible=KeyEligible;
  for (int wi=0;wi < cst_md5;wi++)
    pFD.Hash.content[wi]=Hash[wi];
  return pFD;
}

FieldDesc_Export& FieldDesc_Export::set(ZFieldDescription& pIn)
{
  StartSign=cst_ZFIELDSTART;
  EndianCheck=cst_EndianCheck_Normal;
  ZType=pIn.ZType;
  Capacity=pIn.Capacity;
  HeaderSize=uint16_t(pIn.HeaderSize);
  UniversalSize=uint32_t(pIn.UniversalSize);
  NaturalSize=uint32_t(pIn.NaturalSize);
  KeyEligible=pIn.KeyEligible;
  for (int wi=0;wi < cst_md5;wi++)
    Hash[wi]=pIn.Hash.content[wi];
  return *this;
}

void
FieldDesc_Export::_convert()
{
  if (is_big_endian())
    return ;
  /* BlockId (byte), StartSign EndSign(palyndromas) do not need to be reversed */

  EndianCheck=reverseByteOrder_Conditional(EndianCheck);
  ZType=reverseByteOrder_Conditional<ZTypeBase>(ZType);
  Capacity = reverseByteOrder_Conditional(Capacity);
  HeaderSize = reverseByteOrder_Conditional(HeaderSize);
  UniversalSize = reverseByteOrder_Conditional(UniversalSize);
  NaturalSize = reverseByteOrder_Conditional(NaturalSize);
// KeyEligible  is one byte
}
void
FieldDesc_Export::serialize()
{
  if (!is_little_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"FieldDesc_Export::serialize-W-ALRDY ZHeaderControlBlock already serialized. \n");
    return;
  }
  _convert();
}
void
FieldDesc_Export::deserialize()
{
  if (!is_little_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"FieldDesc_Export::deserialize-W-ALRDY ZHeaderControlBlock already deserialized. \n");
    return;
  }
  _convert();
}

FieldDesc_Check& FieldDesc_Check::set(ZFieldDescription& pIn)
{
  memset(this,0,sizeof(FieldDesc_Check));
  ZType=pIn.ZType;
  Capacity=pIn.Capacity;
  HeaderSize=uint16_t(pIn.HeaderSize);
  UniversalSize=uint32_t(pIn.UniversalSize);
  NaturalSize=uint32_t(pIn.NaturalSize);
  KeyEligible=pIn.KeyEligible;
//  memset(Name,0,cst_FieldNameCapacity);
  pIn.Name._exportUVFPtr(Name,cst_FieldNameCapacity);
  return *this;
}

FieldDesc_Check&
FieldDesc_Check::_copyFrom(FieldDesc_Check& pIn)
{
  for (int wi=0; wi < cst_FieldNameCapacity;wi++)
    Name[wi]=pIn.Name[wi];
  ZType=pIn.ZType;
  Capacity=pIn.Capacity;
  HeaderSize=pIn.HeaderSize;
  UniversalSize=pIn.UniversalSize;
  NaturalSize=pIn.NaturalSize;
  KeyEligible=pIn.KeyEligible;
//  memmove(Name,pIn.Name,cst_FieldNameCapacity);
  return *this;
}

FieldDesc_Pack&
FieldDesc_Pack::_copyFrom(const FieldDesc_Pack &pIn)
{
  ZType=pIn.ZType;
  Capacity=pIn.Capacity;
  HeaderSize=pIn.HeaderSize;
  UniversalSize=pIn.UniversalSize;
  NaturalSize=pIn.NaturalSize;
  KeyEligible=pIn.KeyEligible;
  memmove(Name,pIn.Name,cst_FieldNameCapacity);
  memmove(Hash,pIn.Hash,cst_md5);
  return *this;
}


FieldDesc_Pack& FieldDesc_Pack::set(ZFieldDescription& pIn)
{
  ZType=pIn.ZType;
  Capacity=pIn.Capacity;
  HeaderSize=uint16_t(pIn.HeaderSize);
  UniversalSize=uint32_t(pIn.UniversalSize);
  NaturalSize=uint32_t(pIn.NaturalSize);
  KeyEligible=pIn.KeyEligible;
  pIn.Name._exportUVFPtr(Name,cst_FieldNameCapacity);
  memmove(Hash,pIn.Hash.content,cst_md5);
  return *this;
}

ZFieldDescription FieldDesc_Pack::toFieldDescription()
{
  ZFieldDescription pFD;
  pFD.ZType=ZType;
  pFD.Capacity=Capacity;
  pFD.HeaderSize=HeaderSize;
  pFD.UniversalSize=UniversalSize;
  pFD.NaturalSize=NaturalSize;
  pFD.KeyEligible=KeyEligible;
  for (int wi=0;wi < cst_md5;wi++)
    pFD.Hash.content[wi]=Hash[wi];
  const unsigned char *wPtr=Name;
  pFD.Name._importUVF(wPtr);
  return pFD;
}


bool FieldDesc_Pack::hasSameHash(const KeyField_Pack& pIn)
{
  return memcmp(Hash,pIn.Hash,cst_md5)==0;
}
utf8VaryingString FieldDesc_Pack::getName()
{
  utf8VaryingString wRet;
  const unsigned char*wPtr=Name;
  wRet._importUVF(wPtr);
  return wRet;
}

void FieldDesc_Pack::setName(const utf8VaryingString& pName)
{
  pName._exportUVFPtr(Name,cst_fieldnamelen);
}

QDataStream& FieldDesc_Pack::write(QDataStream &dataStream)
{
  dataStream.writeBytes((const char *)this,sizeof(FieldDesc_Pack)) ;
  return(dataStream);
}

QDataStream& FieldDesc_Pack::read(QDataStream &dataStream)
{
  char *Buf;
  uint wsize;
  dataStream.readBytes (Buf,wsize );
  if (wsize<sizeof(FieldDesc_Pack))
    memmove(this,Buf,wsize);
  else
    memmove(this,(const void*)Buf,sizeof(FieldDesc_Pack));
  delete Buf;
  return(dataStream);
}
