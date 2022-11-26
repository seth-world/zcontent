#include "zkeydictionary.h"

#include <ztoolset/zlimit.h>

#include <zindexedfile/zmfdictionary.h>
#include <zxml/zxmlprimitives.h>

using namespace zbs;

KeyDic_Pack& KeyDic_Pack::_copyFrom(const KeyDic_Pack& pIn)
{
  memset(this,0,sizeof(KeyDic_Pack));
  memmove(Name,pIn.Name,cst_fieldnamelen);
  KeyUniversalSize=pIn.KeyUniversalSize;
  Duplicates=pIn.Duplicates;
  return *this;
}

KeyDic_Pack& KeyDic_Pack::set(const ZKeyDictionary &pIn)
{
  memset(this,0,sizeof(KeyDic_Pack));
  pIn.DicKeyName._exportUVFPtr(Name,cst_fieldnamelen);
  KeyUniversalSize=pIn._reComputeKeySize();
  return *this;
}

void
KeyDic_Pack::setName(const utf8_t* pName)
{
  if (pName==nullptr)
    return;
  utf8String wName=pName;
  wName._exportUVFPtr(Name,cst_fieldnamelen);
}
utf8String
KeyDic_Pack::getName()
{
  utf8String wName;
  const unsigned char* wPtr=Name;
  wName._importUVF(wPtr);
  return wName;
}

ZKeyDictionary::ZKeyDictionary(ZMFDictionary*pMDic)
{
Dictionary=pMDic;
}
ZKeyDictionary::ZKeyDictionary(const ZKeyDictionary *pIn)
{
  _copyFrom(*pIn);
}
ZKeyDictionary::ZKeyDictionary(const ZKeyDictionary& pIn)
{
  _copyFrom(pIn);
}

ZTypeBase ZKeyDictionary::getType(const long pKFieldRank) const
{
  long wRank =Tab[pKFieldRank].MDicRank;
  return (Dictionary->Tab[wRank].ZType);
}
uint64_t ZKeyDictionary::getUniversalSize(const long pKFieldRank) const
{
  long wRank =Tab[pKFieldRank].MDicRank;
  return (Dictionary->Tab[wRank].UniversalSize);
}
uint64_t ZKeyDictionary::getNaturalSize(const long pKFieldRank) const
{
  long wRank =Tab[pKFieldRank].MDicRank;
  return (Dictionary->Tab[wRank].NaturalSize);
}
uint32_t ZKeyDictionary::computeKeyUniversalSize() const
{
  uint64_t wUSize=0;
  for (long wi = 0; wi < count(); wi++)
            wUSize += getUniversalSize(wi);
  return uint32_t(wUSize);
}

/** @cond Development
 * @brief ZSKeyDictionary::_reComputeSize computes Universal key size for the index key. Returns computed size.
 */
uint32_t
ZKeyDictionary::_reComputeKeySize (void) const
{
//  KeyNaturalSize=0;
  uint32_t wKeyUniversalSize=0;

  for (long wi=0;wi<size(); wi++)
  { // only ZKDic at record level may be a certain value for field sizes
    Tab[wi].KeyOffset= wKeyUniversalSize;

//    KeyNaturalSize +=(size_t) getNaturalSize(wi) ; don't care about natural size
    wKeyUniversalSize += (uint32_t)getUniversalSize(wi) ;

    /*KeyNaturalSize += Tab[wi].NaturalSize;
                KeyUniversalSize += Tab[wi].UniversalSize;*/
  }
  return wKeyUniversalSize;
}// _reComputeSize

#ifdef __COMMENT__
/**
 * @brief CZKeyDictionary::fieldRecordOffset this routine gives the offset from the beginning of the ZMasterFile user's record for the field at Key Dictionary rank pRank.
 * @param[in] pRank relative position (rank) of the field within key dictionary
 * @return the offset from the beginning of the ZMasterFile user's record for the field at Key Dictionary rank pRank. Returns -1 if field rank is out of dictionary boundaries.
 */
ssize_t ZSKeyDictionary ::fieldRecordOffset (const long pRank)
{

  return MDic->Tab[pRank].DataOffset;

} //fieldOffset
#endif //__COMMENT__
#ifdef __COMMENT__
/**
 * @brief CZKeyDictionary::zremoveField removes a field which name corresponds to pFieldName from the current key dictionary
 * @param[in] pFieldName user name for the field to be removed from dictionary
 * @return  a ZStatunamespace zbss. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSKeyDictionary::zremoveField (const char *pFieldName)
{

  long wRank = zsearchFieldByName(pFieldName);
  if (wRank<0)
    return ZS_INVNAME;

  erase(wRank);
  _reComputeSize();
  return ZS_SUCCESS;namespace zbs
}//zremoveField

/**
 * @brief CZKeyDictionary::zsetField For a key dictionary field given by its rank pFieldRank sets its attribute given by pZKD to pValue
 * @param[in] pFieldRank Key field rank to modify attribute
 * @param[in] pZKD       a ZKeyDic_type describing the attribute to modify
 * @param[in] pValue     Key field attribute value
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */namespace zbs
ZStatus
ZSKeyDictionary::zsetField (const long pFieldRank,ZKeyDic_type pZKD,auto pValue)
{
  ZStatus wSt = ZS_SUCCESS;
  if ((pFieldRank<0)||(pFieldRank>lastIdx()))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_OUTBOUND,
        Severity_Error,
        "Given field rank <%ld> is out of Key dictionary boundaries [0,%ld]",
        pFieldRank,
        lastIdx());
    return (ZS_OUTBOUND);
  }
  switch (pZKD)
  {
  case ZKD_ArraySize :
  {
    Tab[pFieldRank].ArrayCount = pValue;
    return ZS_SUCCESS;
  }
  case ZKD_UniversalSize :
  {
    Tab[pFieldRank].UniversalSize = pValue;
    return ZS_SUCCESS;
  }
  case ZKD_NaturalSize :
  {
    Tab[pFieldRank].NaturalSize = pValue;
    return ZS_SUCCESS;
  }
  case ZKD_RecordOffset :
  {
    Tab[pFieldRank].RecordOffset = pValue;
    return ZS_SUCCESS;
  }
  case ZKD_ZType :
  {
    Tab[pFieldRank].ZType = pValue;
    if (!(Tab[pFieldRank].ZType&ZType_NegateStructure)) // atomic value ?
    {
      Tab[pFieldRank].ArrayCount = 1;
      wSt=getAtomicZType_Sizes(Tab[pFieldRank].ZType,Tab[pFieldRank].NaturalSize,Tab[pFieldRank].UniversalSize);
      if (wSt!=ZS_SUCCESS)
        return wSt;
    }
    return ZS_SUCCESS;
  }
  default:
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Error,
        " Invalid ZKeyDic_type <%ld> ",
        pZKD);
    return ZS_INVTYPE;
  }
  }//case
}//zsetField
#endif // __COMMENT__

ZDataBuffer&
ZKeyDictionary::_exportAppend(ZDataBuffer& pZDBExport)
{
// exporting each rank of dictionnary : each field definition

  unsigned char* wBuffer;
  size_t wBufferSize;
  ZAexport<ZIndexField,ZIndexField_Exp>((_Base*)this,
                                          wBuffer,
                                          wBufferSize,
                                          &ZIndexField::_exportConvert);
  pZDBExport.appendData(wBuffer,wBufferSize);
  free(wBuffer);
  DicKeyName._exportAppendUVF(pZDBExport);
  return pZDBExport;
}





void
ZKeyDictionary_Exp::setFromPtr(const unsigned char* &pPtrIn)
{
  memmove (this,pPtrIn,sizeof(ZKeyDictionary_Exp));
  pPtrIn += sizeof(ZKeyDictionary_Exp);
}

void
ZKeyDictionary_Exp::set(const ZKeyDictionary& pIn)
{
  ZAE=pIn.getZAExport();
  ZAE.DataSize = sizeof(ZKeyDictionary_Exp)*ZAE.NbElements;
  ZAE.FullSize = sizeof(ZAE)+ZAE.DataSize;

  FieldNb=(uint32_t)pIn.count();

  KeyDicSize=(uint32_t)(sizeof(ZKeyDictionary_Exp)+pIn.DicKeyName._getexportUVFSize()+(sizeof(ZIndexField_Exp)*pIn.count()));
}


void ZKeyDictionary_Exp::_convert()
{
  if (is_big_endian())
    return ;
  EndianCheck = reverseByteOrder_Conditional<uint16_t>(EndianCheck);
  FieldNb=reverseByteOrder_Conditional<uint32_t>(FieldNb);
  KeyDicSize=reverseByteOrder_Conditional<uint32_t>(KeyDicSize);
}
void
ZKeyDictionary_Exp::serialize()
{
  if (is_big_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZKeyDictionary_Exp::serialize-W-ALRDY ZHeaderControlBlock already serialized. \n");
    return;
  }
  _convert();
  ZAE.serialize();

}
void
ZKeyDictionary_Exp::deserialize()
{
  if (is_big_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZKeyDictionary_Exp::deserialize-W-ALRDY ZHeaderControlBlock already deserialized. \n");
    return;
  }
  _convert();
  ZAE.deserialize();

}


ZDataBuffer&
ZKeyDictionary::_exportAppendFlat(ZDataBuffer& pZDBExport)
{
  // exporting each rank of key dictionnary : each field definition
  ZDataBuffer wDB;

  ZKeyDictionary_Exp wKDExp;
  wKDExp.KeyDicSize=(uint32_t)(sizeof(ZKeyDictionary_Exp)+DicKeyName._getexportUVFSize()+(sizeof(ZIndexField_Exp)*count()));

  wKDExp.set(*this);
  wKDExp.serialize();

  wDB.setData(&wKDExp,sizeof(ZKeyDictionary_Exp));
  DicKeyName._exportAppendUVF(wDB);     /* Note : DicKeyName export data is between ZAExport and ZArray flat content */

  /* now export key field by keyfield */

  unsigned char* wPtrOut=wDB.extendBZero(sizeof(ZIndexField_Exp)*count());

  ZIndexField_Exp* wIFExp=(ZIndexField_Exp*)wPtrOut;
  for (long wi=0;wi < count();wi++)
    {
      wIFExp[wi].set(Tab[wi]);
      wIFExp[wi].serialize();
    }

  /* done */

  return pZDBExport.appendData(wDB);
}//_exportAppendFlat

ZStatus
ZKeyDictionary::_importFlat(const unsigned char* &pPtrIn)
{

  ZKeyDictionary_Exp wKDExp ;
  wKDExp.setFromPtr(pPtrIn);

  if (wKDExp.StartSign!=cst_ZMSTART) {
    ZException.setMessage(_GET_FUNCTION_NAME_,ZS_CORRUPTED,Severity_Error,
        "While importing Key Dictionary found start sign <%X> expected <%X>. Import data is corrupted.",wKDExp.StartSign,cst_ZMSTART);
    return ZS_CORRUPTED;
  }

  wKDExp.deserialize();


  if (wKDExp.FieldNb > __INVALID_INDEX__)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,ZS_CORRUPTED,Severity_Error,
        "While importing Key Dictionary found invalid index value <%ld>. Import data must be corrupted.",wKDExp.FieldNb);
    return ZS_CORRUPTED;
  }
  if (wKDExp.KeyDicSize > __INVALID_SIZE__)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,ZS_CORRUPTED,Severity_Error,
        "While importing Key Dictionary found invalid size <%ld>. Import data must be corrupted.",wKDExp.KeyDicSize);
    return ZS_CORRUPTED;
  }

  /* set parameters for current ZKeyDictionary ZArray with ZAE params */
  wKDExp.ZAE.setZArray(this);

  /* Note : DicKeyName export data is between ZAExport and ZArray flat content (key fields descriptions) */

  DicKeyName._importUVF(pPtrIn); /* pPtrIn is updated */

  /* now import key field by key field */
  //  ZSIndexField_exp* wIFExp=(ZSIndexField_exp*)pPtrIn;
  ZIndexField wKeyField;
  int wCount=wKDExp.FieldNb;
  while (wCount--)
  {
    wKeyField._import(pPtrIn);
    push(wKeyField);
  }

  _reComputeKeySize();
  return ZS_SUCCESS;
}//_importFlat




ZDataBuffer
ZKeyDictionary::_export()
{
  // exporting each rank of dictionnary : each field definition
  ZDataBuffer wZDB;
  return _exportAppend(wZDB);
}
ZStatus
ZKeyDictionary::_import(const unsigned char* &pZDBImport_Ptr)
{
  ssize_t wSize=0;
  ZStatus wSt=ZAimport((_Base*)this,
                          pZDBImport_Ptr,wSize,                   /* pointer is updated */
                          &ZIndexField::_importConvert);
  if (wSt!=ZS_SUCCESS)
    {
    ZException.addToLast("while importing key dictionary ");
    return wSt;
    }

  DicKeyName._importUVF(pZDBImport_Ptr);

  _reComputeKeySize();

  return ZS_SUCCESS;
}

/*
 *    Key dictionary schema for one index key
 *
  <key>
    <rank>n</rank>
    <indexname> </indexname>  <!-- link to index control block indexname field -_>
    <keyfields>
      <field>
        <fieldrank>n<fieldrank>
                <name>name of the field from meta dic</name> <--! from metadic -->
                <ztype> </ztype> <--! from meta dic not used -->
                <universalsize> </universalsize>  <--! from meta dic not used -->
                <mdicrank> </mdicrank>
                <hash>hhhhhhhhhh</hash> <--! hexa value -->
      </field>
      <field>
                <name>name of the field from meta dic</name> <--! from metadic -->
                <ztype> </ztype> <--! from meta dic not used -->
                <universalsize> </universalsize>  <--! from meta dic not used -->
                <mdicrank> </mdicrank>
                <hash>hhhhhhhhhh</hash> <--! hexa value -->
      </field>
    </keyfields>
  </key>

 */

utf8VaryingString ZKeyDictionary::toXml(int pLevel,int pRank, bool pComment)
{
  int wLevel=pLevel;
  utf8VaryingString wReturn;
  wReturn = fmtXMLnode("key",pLevel);
  wLevel++;
//  wReturn+=fmtXMLuint64("kdicsize",KDicSize,wLevel);    // KDicSize is export size : dont care
//  wReturn+=fmtXMLuint64("keynaturalsize",KeyNaturalSize,wLevel);
//  wReturn+=fmtXMLuint64("keyuniversalsize",KeyUniversalSize,wLevel);  // KeyUniversalSize is stored within ICB

  if (DicKeyName.isEmpty())
    fmtXMLcomment("/keyname/ is missing",wLevel);
  else {
  wReturn += fmtXMLchar("keyname",DicKeyName.toCChar(), wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," linked to index control block indexname field ");
  }
  wReturn+=fmtXMLbool("duplicates",(bool)Duplicates,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," If set, key allows duplicates. if not - key must be unique ");

  if (!ToolTip.isEmpty())
    wReturn+=fmtXMLstring("tooltip",ToolTip,wLevel);

  wReturn += fmtXMLnode("keyfields",wLevel);
  /* key fields */
  for (long wi=0;wi < count();wi++)
    {
    wReturn += fmtXMLnode("field",wLevel+1);
    /* dump values from ZIndexField */
    wReturn +=fmtXMLlong("mdicrank",Tab[wi].MDicRank,wLevel+2);
    wReturn +=fmtXMLmd5("hash",Tab[wi].Hash,wLevel+2);
    wReturn +=fmtXMLuint32("keyoffset",Tab[wi].KeyOffset,wLevel+2);

    /* get infos from mdic concerning this field (not required) */
    if (pComment)
      wReturn += fmtXMLcomment(" hereafter optional fields from MetaDic describing key field ",wLevel+2);
    long wI = Tab[wi].MDicRank;
    wReturn +=fmtXMLchar("name",Dictionary->Tab[wI].getName().toCChar(),wLevel+2);
    wReturn +=fmtXMLuint32("ztype",Dictionary->Tab[wI].ZType,wLevel+2);    /* ZTypeBase = uint32_t*/
    utf8String wZTStr;
    wZTStr.sprintf(" ZType_type <%s> converted to its value number",decode_ZType(Dictionary->Tab[wI].ZType));
    fmtXMLaddInlineComment(wReturn,wZTStr.toCChar());
    wReturn +=fmtXMLuint64("universalsize",Dictionary->Tab[wI].UniversalSize,wLevel+2);

    wReturn += fmtXMLendnode("field",wLevel+1);
    }

  wReturn += fmtXMLendnode("keyfields",wLevel);

  wReturn += fmtXMLendnode("key",pLevel);
  return wReturn;
} // ZSKeyDictionary::toXml

ZStatus ZKeyDictionary::fromXml(zxmlNode* pKeyDicNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wKeyRootNode=nullptr;
  zxmlElement *wFieldsRootNode=nullptr;
  zxmlElement *wSingleFieldNode=nullptr;
  zxmlElement *wSwapNode=nullptr;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  ZIndexField wIFld;
  bool wBool;
  unsigned int wInt;
  ZStatus   wSt;

  pErrorlog->setContext("ZKeyDictionary::fromXml");
  int wErroredFields=0,wWarnedFields=0;

  if( pKeyDicNode->getName() != "key") {
    pErrorlog->logZStatus(ZAIES_Fatal,
        ZS_XMLINVROOTNAME,
        "ZKeyDictionary::fromXml-F-INVROOTNAME Wrong name <%s> for given root node. Expected <metadictionary> ",pKeyDicNode->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  wKeyRootNode=(zxmlElement *)pKeyDicNode;

  if (XMLgetChildText(wKeyRootNode,"keyname",DicKeyName,pErrorlog,ZAIES_Error)<0)
      {
      pErrorlog->logZStatus(ZAIES_Error, ZS_XMLMISSREQ,"ZSKeyDictionary::fromXml-E-MISSREQFLD Missing mandatory field <keyname>");
      return ZS_XMLMISSREQ;
      }

  if (XMLgetChildBool(wKeyRootNode,"duplicates",wBool,pErrorlog,ZAIES_Error)<0)
      {
        pErrorlog->logZStatus(ZAIES_Error, ZS_XMLMISSREQ,"ZSKeyDictionary::fromXml-E-MISSREQFLD Missing mandatory field <keyname>");
         wErroredFields ++;
        return ZS_XMLMISSREQ;
      }
      else
        Duplicates=(uint8_t)wBool;

  if (XMLgetChildText(wKeyRootNode,"tooltip",ToolTip,pErrorlog,ZAIES_Warning)<0){
    wWarnedFields++;
    pErrorlog->warningLog("ZSKeyDictionary::fromXml-W-OPTMISS Missing optional key field <tooltip> for key <%s>",
        DicKeyName.toString());
  }

    wSt = wKeyRootNode->getChildByName((zxmlNode *&) wFieldsRootNode, "keyfields");
    if (wSt != ZS_SUCCESS) {
      pErrorlog->logZStatus(
          ZAIES_Error,
          wSt,
          "ZSKeyDictionary::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s> for key <%s>",
          "keyfields",
          decode_ZStatus(wSt),DicKeyName.toString());
      pErrorlog->popContext();
      return wSt;
    }
    wSt=wFieldsRootNode->getFirstChildElement(wSwapNode);

    while (wSt==ZS_SUCCESS)
      {
      utf8String wName;
      wSingleFieldNode=wSwapNode;
      if (wSingleFieldNode->getName()=="field")
        {
        wIFld.clear();
        if (XMLgetChildMd5(wSingleFieldNode,"hash",wIFld.Hash,pErrorlog,ZAIES_Error) != ZS_SUCCESS)
          wErroredFields ++;
        if (XMLgetChildUInt(wSingleFieldNode,"keyoffset",wIFld.KeyOffset,pErrorlog,ZAIES_Error) != ZS_SUCCESS)
          wErroredFields ++;
        /* search within Meta Dic for hash code */
        bool wF=false;
        for (long wi=0;wi < Dictionary->count();wi++)
          {
          if (Dictionary->Tab[wi].Hash==wIFld.Hash)
            {
            wF=true;
            wIFld.MDicRank=uint32_t(wi);
            break;
            }
          }//for

          if (!wF) /* hash not successfull :try by name */
          {
            if (XMLgetChildText(wSingleFieldNode,"name",wName,pErrorlog,ZAIES_Error) != ZS_SUCCESS)
              wErroredFields ++;
            for (long wi=0;wi < Dictionary->count();wi++)
            {
              if (Dictionary->Tab[wi].getName()==wName)
              {
                wF=true;
                wIFld.MDicRank=uint32_t(wi);
                break;
              }
            }//for
          }

          if (!wF) /* name not successfull : error */
          {
            if (XMLgetChildUInt(wSingleFieldNode,"mdicrank",wIFld.MDicRank,pErrorlog,ZAIES_Error) != ZS_SUCCESS)
              wErroredFields ++;
            pErrorlog->logZStatus(ZAIES_Error,ZS_NOTFOUND,
                "ZSKeyDictionary::fromXml-E-NTFND Key field name <%s> hash <%s> declared meta dic rank <%s> has not been found within meta dictionary",
                wName.toCChar(),
                wIFld.Hash.toHexa().toCChar(),
                wIFld.MDicRank);
          }
          else
            push(wIFld);

        }//if (wSingleFieldNode->getName()=="field")

      wSwapNode=nullptr;
      wSt=wSingleFieldNode->getNextNode((zxmlNode*&)wSwapNode);
      XMLderegister(wSingleFieldNode);
      wSingleFieldNode=wSwapNode;
      }//while (wSt==ZS_SUCCESS)

    pErrorlog->textLog("ZKeyDictionary::fromXml___________Field definitions load report____________________\n"
                       " Key name <%s>.\n"
                       " %ld key fields loaded.\n"
                       " %d xml fields warned.\n"
                       " %d xml fields errored and not loaded.",
        DicKeyName.isEmpty()?"<no name>":DicKeyName.toCChar(),
        count(), wWarnedFields,
        wErroredFields);


  pErrorlog->popContext();
  XMLderegister(wKeyRootNode);
  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//ZSKeyDictionary::fromXml


/**
 * @brief ZKeyDictionary::zgetFieldRank gets a field position (rank) in the key dictionary using its field name
 *
 * @param[in] pFieldName a Cstring that qualifies the name of the field. This name is given while creating the index dictionary.
 * @return the field position (rank) in dictionary. returns -1 if field name has not been found.
 */
long
ZKeyDictionary::zsearchFieldByName(const utf8_t *pFieldName) const
{
  long wMDicRank=0;
  for (long wi=0;wi<size();wi++)
  {
    wMDicRank= Tab[wi].MDicRank;
    if (Dictionary->Tab[wMDicRank].getName()==pFieldName)
      return wi;
  }
  return -1;
}
/**
 * @brief CZKeyDictionary::zgetFieldRank gets a field position (rank) in the key dictionary using its field name
 *
 * @param[in] pFieldName a Cstring that qualifies the name of the field. This name is given while creating the index dictionary.
 * @return the field position (rank) in dictionary. returns -1 if field name has not been found.
 */
long
ZKeyDictionary::zsearchFieldByName(const utf8String &pFieldName) const
{
  long wMDicRank=0;
  for (long wi=0;wi<size();wi++)
  {
    wMDicRank= Tab[wi].MDicRank;
    if (Dictionary->Tab[wMDicRank].getName()==pFieldName)
      return wi;
  }
  return -1;
}

/**
 * @brief ZSKeyDictionary::print Reports the content of ZSKeyDictionary for all fields
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void ZKeyDictionary::report (FILE* pOutput)
{
  fprintf (pOutput,
      "-----------------------ZKeyFieldList content-------------------\n"
      " %4s %8s %8s %8s %8s %17s %5s\n",
      "rank",
      "Offset",
      "Natural",
      "Internal",
      "Array nb",
      "Name",
      "ZType");
  for (long wi=0;wi<size();wi++)
  {
    ZFieldDescription& wFD = Dictionary->Tab[Tab[wi].MDicRank];

    fprintf (pOutput,
        " <%2ld> %8d %8ld %8ld %8d <%15s> <%s>\n",
        wi,
        Tab[wi].KeyOffset,
        wFD.NaturalSize,
        wFD.UniversalSize,
        wFD.Capacity,
        wFD.getName().toCChar(),
        decode_ZType( wFD.ZType));
  }// for
  fprintf (pOutput,
      "---------------------------------------------------------------\n");
  return;
}


ZKeyDictionary& ZKeyDictionary ::_copyFrom(const ZKeyDictionary &pIn)
{
 // KDicSize=pIn.KDicSize;
//  KeyNaturalSize=pIn.KeyNaturalSize;
//  KeyUniversalSize=pIn.KeyUniversalSize;
  Recomputed=pIn.Recomputed;
  Status = pIn.Status;
  Dictionary=pIn.Dictionary;
//  _Base::_copyFrom(pIn);
  _Base::setQuota(pIn.ZReallocQuota);
  _Base::allocate(pIn.ZAllocation);
  _Base::clear();
  for (long wi=0; wi<pIn.count();wi++)
    _Base::push(pIn.Tab[wi]);
  ToolTip = pIn.ToolTip;
  DicKeyName = pIn.DicKeyName;
  Duplicates = pIn.Duplicates;
  return *this;
} //fieldOffset

long ZKeyDictionary ::hasFieldNameCase(const utf8VaryingString& pName) {
  for (long wi=0;wi < count();wi++) {
    if (Dictionary->Tab[Tab[wi].MDicRank].getName().isEqualCase(pName))
      return wi;
  }
  return -1;
}


bool ZKeyDictionary ::hasSameContentAs(ZKeyDictionary*pKey)
{
  if (count()!=pKey->count())
    return false;

  for (long wi=0;wi<count();wi++)
    {
    long wMDicRank= Tab[wi].MDicRank;
    long wMDicRankIn= pKey->Tab[wi].MDicRank;
    if (Dictionary->Tab[wMDicRank].ZType!=pKey->Dictionary->Tab[wMDicRankIn].ZType)
      return false;
    if (Dictionary->Tab[wMDicRank].UniversalSize!=pKey->Dictionary->Tab[wMDicRankIn].UniversalSize)
      return false;
    if (Dictionary->Tab[wMDicRank].Capacity!=pKey->Dictionary->Tab[wMDicRankIn].Capacity)
      return false;
    if (Tab[wi].KeyOffset!=pKey->Tab[wi].KeyOffset)
      return false;
    }
  return true;
}


uint32_t ZKeyDictionary::computeKeyOffsets()
{
  // compute offset of requested Field within Key

  uint32_t wKeyOffset = 0;
  long wi=0;

  while (wi < count())
    {
    Tab[wi].KeyOffset=wKeyOffset;
    wKeyOffset += Dictionary->Tab[Tab[wi].MDicRank].UniversalSize;
    wi++;
    }
  return wKeyOffset;  // computed key size
} //computeKeyOffsets

ZStatus
ZKeyDictionary::addFieldToZKeyByName (const char* pFieldName)
{

  ZIndexField wField;
  if (Dictionary==nullptr)
  {
    return ZS_INVVALUE;
  }
  zrank_type wMRank=Dictionary->searchFieldByName(pFieldName);
  if (wMRank<0)
    return ZS_NOTFOUND;

  return addFieldToZKeyByRank (wMRank);
}//addFieldToZDicByName

ZStatus
ZKeyDictionary::addFieldToZKeyByRank (const zrank_type pMDicRank)
{
  ZIndexField wField;
  if (Dictionary==nullptr)
  {
    return ZS_NULLPTR;
  }
  if ((pMDicRank>=Dictionary->size())||(pMDicRank<0))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_OUTBOUND,
        Severity_Error,
        "Meta dictionary rank <%ld> is not a valid index value",
        pMDicRank);
    return ZS_OUTBOUND;
  }

  if (!Dictionary->Tab[pMDicRank].KeyEligible)
    if (!isKeyEligible( Dictionary->Tab[pMDicRank].ZType))
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVINDEX,
        Severity_Error,
        "Field rank <%ld> <%s> is not eligible to be a key field",
        pMDicRank,
        Dictionary->Tab[pMDicRank].getName().toCChar());
    return ZS_INVTYPE;
    }
/*  wField.ZType=MetaDic->Tab[pMDicRank].ZType;
  wField.UniversalSize=MetaDic->Tab[pMDicRank].UniversalSize;
  wField.NaturalSize=MetaDic->Tab[pMDicRank].NaturalSize;
  wField.ArrayCount=MetaDic->Tab[pMDicRank].Capacity;*/
  wField.MDicRank = pMDicRank;
  wField.Hash = Dictionary->Tab[pMDicRank].Hash ;
  /* KeyOffset is left to Zero */
  push(wField);
  computeKeyOffsets();
  Recomputed=false;
  return ZS_SUCCESS;
}// addFieldToZDicByRank

#ifdef __COMMENT__

void ZKeyHeaderRow::set(const ZKeyDictionary& pKeyDic) {
  ZKeyDictionary::_copyFrom(pKeyDic);
  KeyUniversalSize=pKeyDic._reComputeKeySize();
}
void ZKeyHeaderRow::set(const ZKeyDictionary* pKeyDic) {
  ZKeyDictionary::_copyFrom(pKeyDic);
  KeyUniversalSize=pKeyDic->_reComputeKeySize();
}
ZKeyDictionary ZKeyHeaderRow::get() {
  return new ZKeyDictionary(this);
}
/*
void ZKeyFieldRow::set(const ZMetaDic& pMetaDic, const ZIndexField& pKeyField) {
  MDicRank=pKeyField.MDicRank;
  KeyOffset=pKeyField.KeyOffset;
  Hash=pKeyField.Hash;
  UniversalSize=pMetaDic.Tab[MDicRank].UniversalSize;
  Name =pMetaDic.Tab[MDicRank].getName();
  ZType = pMetaDic.Tab[MDicRank].ZType;
}*/
void ZKeyFieldRow::set( const ZIndexField& pKeyField) {
  ZIndexField::_copyFrom(pKeyField);
//  MDicRank=pKeyField.MDicRank;
//  KeyOffset=pKeyField.KeyOffset;
//  Hash=pKeyField.Hash;
  assert(pKeyField.KeyDic->Dictionary!=nullptr);
  ZMetaDic* wMetaDic=pKeyField.KeyDic->Dictionary;
  UniversalSize=wMetaDic->Tab[MDicRank].UniversalSize;
  Name =wMetaDic->Tab[MDicRank].getName();
  ZType = wMetaDic->Tab[MDicRank].ZType;
}
ZIndexField ZKeyFieldRow::get() {
  return ZIndexField (*this);
}

#endif //__COMMENT__

/** @endcond */
