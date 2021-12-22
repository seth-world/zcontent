#include "zkeydictionary.h"

#include <zindexedfile/zmetadic.h>
#include <zxml/zxmlprimitives.h>

using namespace zbs;

ZSKeyDictionary::ZSKeyDictionary(ZMetaDic*pMDic)
{
MetaDic=pMDic;
}
ZSKeyDictionary::ZSKeyDictionary(ZSKeyDictionary* pIn)
{
  _copyFrom(*pIn);
}
ZSKeyDictionary::ZSKeyDictionary(ZSKeyDictionary& pIn)
{
  _copyFrom(pIn);
}

ZTypeBase ZSKeyDictionary::getType(const long pKFieldRank)
{
  long wRank =Tab[pKFieldRank].MDicRank;
  return (MetaDic->Tab[wRank].ZType);
}
uint64_t ZSKeyDictionary::getUniversalSize(const long pKFieldRank)
{
  long wRank =Tab[pKFieldRank].MDicRank;
  return (MetaDic->Tab[wRank].UniversalSize);
}
uint64_t ZSKeyDictionary::getNaturalSize(const long pKFieldRank)
{
  long wRank =Tab[pKFieldRank].MDicRank;
  return (MetaDic->Tab[wRank].NaturalSize);
}
uint32_t ZSKeyDictionary::computeKeyUniversalSize()
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
ZSKeyDictionary::_reComputeKeySize (void)
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
  Recomputed=true;
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
ZSKeyDictionary::_exportAppend(ZDataBuffer& pZDBExport)
{
// exporting each rank of dictionnary : each field definition

  unsigned char* wBuffer;
  size_t wBufferSize;
  ZAexportCurrent((_Base*)this,
      wBuffer,
      wBufferSize,
      &ZSIndexField::_exportConvert);
  pZDBExport.appendData(wBuffer,wBufferSize);
  free(wBuffer);
  return pZDBExport;
}
ZDataBuffer
ZSKeyDictionary::_export()
{
  // exporting each rank of dictionnary : each field definition
  ZDataBuffer wZDB;
  return _exportAppend(wZDB);
}
size_t
ZSKeyDictionary::_import(unsigned char* &pZDBImport_Ptr)
{
  size_t wDicSize=ZAimport((_Base*)this,
                          pZDBImport_Ptr,                   /* pointer is updated */
                          &ZSIndexField::_importConvert);

  _reComputeKeySize();

  return wDicSize;
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

utf8String ZSKeyDictionary::toXml(int pLevel,int pRank, bool pComment)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("key",pLevel);
  wLevel++;
//  wReturn+=fmtXMLuint64("kdicsize",KDicSize,wLevel);    // KDicSize is export size : dont care
//  wReturn+=fmtXMLuint64("keynaturalsize",KeyNaturalSize,wLevel);
//  wReturn+=fmtXMLuint64("keyuniversalsize",KeyUniversalSize,wLevel);  // KeyUniversalSize is stored within ICB

  wReturn += fmtXMLchar("indexname",DicKeyName.toCChar(), wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," link to index control block indexname field ");

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
    wReturn +=fmtXMLchar("name",MetaDic->Tab[wI].getName().toCChar(),wLevel+2);
    wReturn +=fmtXMLuint32("ztype",MetaDic->Tab[wI].ZType,wLevel+2);    /* ZTypeBase = uint32_t*/
    utf8String wZTStr;
    wZTStr.sprintf(" ZType_type <%s> converted to its value number",decode_ZType(MetaDic->Tab[wI].ZType));
    fmtXMLaddInlineComment(wReturn,wZTStr.toCChar());
    wReturn +=fmtXMLuint64("universalsize",MetaDic->Tab[wI].UniversalSize,wLevel+2);

    wReturn += fmtXMLendnode("field",wLevel+1);
    }

  wReturn += fmtXMLendnode("keyfields",wLevel);

  wReturn += fmtXMLendnode("key",pLevel);
  return wReturn;
} // ZSKeyDictionary::toXml

ZStatus ZSKeyDictionary::fromXml(zxmlNode* pKeyDicNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode=nullptr;
  zxmlElement *wFieldsRootNode=nullptr;
  zxmlElement *wSingleFieldNode=nullptr;
  zxmlElement *wSwapNode=nullptr;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  ZSIndexField wIFld;
  bool wBool;
  unsigned int wInt;

  ZStatus wSt = pKeyDicNode->getChildByName((zxmlNode *&) wRootNode, "keydictionary");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZSKeyDictionary::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
        "keydictionary",
        decode_ZStatus(wSt));
    return wSt;
    }

    if (XMLgetChildText(wSingleFieldNode,"indexname",DicKeyName,pErrorlog,ZAIES_Error)<0)
      {
      pErrorlog->logZStatus(ZAIES_Error, ZS_XMLMISSREQ,"ZSKeyDictionary::fromXml-E-MISSREQFLD Missing mandatory field <indexname>");
      return ZS_XMLMISSREQ;
      }



    wSt = wRootNode->getChildByName((zxmlNode *&) wFieldsRootNode, "keyfields");
    if (wSt != ZS_SUCCESS) {
      pErrorlog->logZStatus(
          ZAIES_Error,
          wSt,
          "ZSKeyDictionary::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
          "<%s>",
          "keyfields",
          decode_ZStatus(wSt));
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
        XMLgetChildMd5(wSingleFieldNode,"hash",wIFld.Hash,pErrorlog,ZAIES_Error);
        XMLgetChildUInt(wSingleFieldNode,"keyoffset",wIFld.KeyOffset,pErrorlog,ZAIES_Error);
        /* search within Meta Dic for hash code */
        bool wF=false;
        for (long wi=0;wi < MetaDic->count();wi++)
          {
          if (MetaDic->Tab[wi].Hash==wIFld.Hash)
            {
            wF=true;
            wIFld.MDicRank=uint32_t(wi);
            break;
            }
          }//for

          if (!wF) /* hash not successfull :try by name */
          {
            XMLgetChildText(wSingleFieldNode,"name",wName,pErrorlog,ZAIES_Error);
            for (long wi=0;wi < MetaDic->count();wi++)
            {
              if (MetaDic->Tab[wi].getName()==wName)
              {
                wF=true;
                wIFld.MDicRank=uint32_t(wi);
                break;
              }
            }//for
          }

          if (!wF) /* name not successfull : error */
          {
            XMLgetChildUInt(wSingleFieldNode,"mdicrank",wIFld.MDicRank,pErrorlog,ZAIES_Error);
            pErrorlog->logZStatus(ZAIES_Error,ZS_NOTFOUND,
                "ZSKeyDictionary::fromXml-E-NTFND Key field name <%s> hash <%s> declared meta dic rank <%s> has not been found within meta dictionary",
                wName.toCChar(),
                wIFld.Hash.toHexa().toChar(),
                wIFld.MDicRank);
          }
          else
            push(wIFld);

        }//if (wSingleFieldNode->getName()=="field")

      wSwapNode=nullptr;
      wSt=wSingleFieldNode->getNextElementSibling(wSwapNode);
      XMLderegister(wSingleFieldNode);
      wSingleFieldNode=wSwapNode;
      }//while (wSt==ZS_SUCCESS)

  XMLderegister(wRootNode);
  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//ZSKeyDictionary::fromXml


/**
 * @brief ZKeyDictionary::zgetFieldRank gets a field position (rank) in the key dictionary using its field name
 *
 * @param[in] pFieldName a Cstring that qualifies the name of the field. This name is given while creating the index dictionary.
 * @return the field position (rank) in dictionary. returns -1 if field name has not been found.
 */
long
ZSKeyDictionary::zsearchFieldByName(const utf8_t *pFieldName) const
{
  long wMDicRank=0;
  for (long wi=0;wi<size();wi++)
  {
    wMDicRank= Tab[wi].MDicRank;
    if (MetaDic->Tab[wMDicRank].getName()==pFieldName)
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
ZSKeyDictionary::zsearchFieldByName(const utf8String &pFieldName) const
{
  long wMDicRank=0;
  for (long wi=0;wi<size();wi++)
  {
    wMDicRank= Tab[wi].MDicRank;
    if (MetaDic->Tab[wMDicRank].getName()==pFieldName)
      return wi;
  }
  return -1;
}
#ifdef __COMMENT__
/**
 * @brief CZKeyDictionary::print Reports the content of CZKeyDictionary for all fields
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void ZSKeyDictionary ::print (FILE* pOutput)
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
    /*    if (Tab[wi].ZType & ZType_Array) */
    fprintf (pOutput,
        " <%2ld> %8ld %8ld %8ld %8d <%15s> <%s>\n",
        wi,
        Tab[wi].RecordOffset,
        Tab[wi].NaturalSize,
        Tab[wi].UniversalSize,
        Tab[wi].ArrayCount,
        Tab[wi].Name.toCChar(),
        decode_ZType( Tab[wi].ZType));
    /*    else
        fprintf (pOutput,
             " <%2ld> %8ld %8ld %8s <%15s> <%s>\n",
             wi,
             Tab[wi].Offset,
             Tab[wi].Length,
             "--",
             Tab[wi].Name.toCChar(),
             decode_ZType( Tab[wi].ZType));*/
  }// for
  fprintf (pOutput,
      "---------------------------------------------------------------\n");
  return;
}
#endif // __COMMENT__

ZSKeyDictionary& ZSKeyDictionary ::_copyFrom( ZSKeyDictionary& pIn)
{
 // KDicSize=pIn.KDicSize;
//  KeyNaturalSize=pIn.KeyNaturalSize;
//  KeyUniversalSize=pIn.KeyUniversalSize;
  Recomputed=pIn.Recomputed;
  Status = pIn.Status;
  MetaDic=pIn.MetaDic;
//  _Base::_copyFrom(pIn);
  _Base::setQuota(pIn.ZReallocQuota);
  _Base::allocate(pIn.ZAllocation);
  _Base::clear();
  for (long wi=0; wi<pIn.count();wi++)
    _Base::push(pIn.Tab[wi]);

  DicKeyName = pIn.DicKeyName;
  return *this;
} //fieldOffset

bool ZSKeyDictionary ::hasSameContentAs(ZSKeyDictionary*pKey)
{
  if (count()!=pKey->count())
    return false;

  for (long wi=0;wi<count();wi++)
    {
    long wMDicRank= Tab[wi].MDicRank;
    long wMDicRankIn= pKey->Tab[wi].MDicRank;
    if (MetaDic->Tab[wMDicRank].ZType!=pKey->MetaDic->Tab[wMDicRankIn].ZType)
      return false;
    if (MetaDic->Tab[wMDicRank].UniversalSize!=pKey->MetaDic->Tab[wMDicRankIn].UniversalSize)
      return false;
    if (MetaDic->Tab[wMDicRank].Capacity!=pKey->MetaDic->Tab[wMDicRankIn].Capacity)
      return false;
    if (Tab[wi].KeyOffset!=pKey->Tab[wi].KeyOffset)
      return false;
    }
  return true;
}


uint32_t ZSKeyDictionary::computeKeyOffsets()
{
  // compute offset of requested Field within Key

  uint32_t wKeyOffset = 0;
  long wi=0;

  while (wi < count())
    {
    Tab[wi].KeyOffset=wKeyOffset;
    wKeyOffset += MetaDic->Tab[Tab[wi].MDicRank].UniversalSize;
    wi++;
    }
  return wKeyOffset;  // computed key size
} //computeKeyOffsets

ZStatus
ZSKeyDictionary::addFieldToZKeyByName (const char* pFieldName)
{

  ZSIndexField wField;
  if (MetaDic==nullptr)
  {
    return ZS_INVVALUE;
  }
  zrank_type wMRank=MetaDic->searchFieldByName(pFieldName);
  if (wMRank<0)
    return ZS_NOTFOUND;

  return addFieldToZKeyByRank (wMRank);
}//addFieldToZDicByName

ZStatus
ZSKeyDictionary::addFieldToZKeyByRank (const zrank_type pMDicRank)
{
  ZSIndexField wField;
  if (MetaDic==nullptr)
  {
    return ZS_NULLPTR;
  }
  if ((pMDicRank>=MetaDic->size())||(pMDicRank<0))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_OUTBOUND,
        Severity_Error,
        "Meta dictionary rank <%ld> is not a valid index value",
        pMDicRank);
    return ZS_OUTBOUND;
  }

  if (!MetaDic->Tab[pMDicRank].KeyEligible)
    if (!isKeyEligible( MetaDic->Tab[pMDicRank].ZType))
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVINDEX,
        Severity_Error,
        "Field rank <%ld> <%s> is not eligible to be a key field",
        pMDicRank,
        MetaDic->Tab[pMDicRank].getName().toCChar());
    return ZS_INVTYPE;
    }
/*  wField.ZType=MetaDic->Tab[pMDicRank].ZType;
  wField.UniversalSize=MetaDic->Tab[pMDicRank].UniversalSize;
  wField.NaturalSize=MetaDic->Tab[pMDicRank].NaturalSize;
  wField.ArrayCount=MetaDic->Tab[pMDicRank].Capacity;*/
  wField.MDicRank = pMDicRank;
  wField.Hash = MetaDic->Tab[pMDicRank].Hash ;
  /* KeyOffset is left to Zero */
  push(wField);
  computeKeyOffsets();
  Recomputed=false;
  return ZS_SUCCESS;
}// addFieldToZDicByRank


/** @endcond */

