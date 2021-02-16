#include "zkeydictionary.h"

ZSKeyDictionary::ZSKeyDictionary(ZMetaDic*pMDic)
{
MetaDic=pMDic;
}

/** @cond Development
 * @brief CZKeyDictionary::_reComputeSize computes the total sizes : Natural size and Internal size for the whole key
 */
void
ZSKeyDictionary::_reComputeSize (void)
{
  KeyNaturalSize=0;
  KeyUniversalSize=0;

  for (long wi=0;wi<size(); wi++)
  { // only ZKDic at record level may be a certain value for field sizes
    Tab[wi].KeyOffset= KeyUniversalSize;

    KeyNaturalSize +=(size_t) getNaturalSize(wi) ;
    KeyUniversalSize += (size_t)getUniversalSize(wi) ;

    /*KeyNaturalSize += Tab[wi].NaturalSize;
                KeyUniversalSize += Tab[wi].UniversalSize;*/
  }
  Recomputed=true;
  return;
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
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSKeyDictionary::zremoveField (const char *pFieldName)
{

  long wRank = zsearchFieldByName(pFieldName);
  if (wRank<0)
    return ZS_INVNAME;

  erase(wRank);
  _reComputeSize();
  return ZS_SUCCESS;
}//zremoveField

/**
 * @brief CZKeyDictionary::zsetField For a key dictionary field given by its rank pFieldRank sets its attribute given by pZKD to pValue
 * @param[in] pFieldRank Key field rank to modify attribute
 * @param[in] pZKD       a ZKeyDic_type describing the attribute to modify
 * @param[in] pValue     Key field attribute value
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
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
ZSKeyDictionary::_export(ZDataBuffer& pZDBExport)
{
  ZDataBuffer wZDB;       // exporting each rank of dictionnary : each field definition
  /*size_t wSize=_reverseByteOrder_T<size_t>(KDicSize);

  pZDBExport.setData(&wSize,sizeof(wSize)); // reserve a size_t first (DicSize) and disregard the other values (computed)

  for (long wi=0;wi<size();wi++)
  {
    pZDBExport.appendData(Tab[wi]._export(wZDB));
  }
  */
      unsigned char* wBuffer;
  size_t wBufferSize;
  ZAexportCurrent((_Base*)this,
      wBuffer,
      wBufferSize,
      &ZSIndexField::_exportConvert);
  pZDBExport.setData(wBuffer,wBufferSize);
  free(wBuffer);
  return pZDBExport;
}
size_t
ZSKeyDictionary::_import(unsigned char* pZDBImport_Ptr)
{
  size_t wDicSize=ZAimport((_Base*)this,
      pZDBImport_Ptr,
      &ZSIndexField::_importConvert);

  _reComputeSize();
  return wDicSize;
}

utf8String ZSKeyDictionary::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("keydictionary",pLevel);
  wLevel++;
  wReturn+=fmtXMLuint64("kdicsize",KDicSize,wLevel);
  wReturn+=fmtXMLuint64("keynaturalsize",KeyNaturalSize,wLevel);
  wReturn+=fmtXMLuint64("keyuniversalsize",KeyUniversalSize,wLevel);

  wReturn = fmtXMLnode("keyfields",wLevel);
  /* key fields */
  for (long wi=0;wi < count();wi++)
    wReturn += Tab[wi].toXml(wLevel+1);
  wReturn = fmtXMLendnode("keyfields",wLevel);

  wReturn += fmtXMLendnode("keydictionary",pLevel);
  return wReturn;
} // ZSKeyDictionary::toXml

int ZSKeyDictionary::fromXml(zxmlNode* pIndexRankNode, ZaiErrors* pErrorlog)
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
  ZStatus wSt = pIndexRankNode->getChildByName((zxmlNode *&) wRootNode, "keydictionary");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZSKeyDictionary::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "keydictionary",
        decode_ZStatus(wSt));
    return -1;
  }
  if (XMLgetChildULong(wRootNode, "kdicsize", KDicSize, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "kdicsize");
  }
  if (XMLgetChildULong(wRootNode, "keynaturalsize", KeyNaturalSize, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "keynaturalsize");
  }
  if (XMLgetChildULong(wRootNode, "keyuniversalsize", KeyUniversalSize, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "keyuniversalsize");
  }

  wSt=wRootNode->getChildByName((zxmlNode*&)wFieldsRootNode,"keyfields");
  if (wSt!=ZS_SUCCESS)
  {
    pErrorlog->errorLog("ZSKeyDictionary::fromXml-E-NDNTFND Node <keyfields> not found.");
    return -1;
  }
  clear(); /* reset all index field definitions */
  wSt=wFieldsRootNode->getFirstChild((zxmlNode*&)wSingleFieldNode);
  long wi=0;
  while (wSt==ZS_SUCCESS)
  {
    wIFld.clear();
    if (wIFld.fromXml(wSingleFieldNode,pErrorlog)==0)
      push(wIFld);
    wSt=wSingleFieldNode->getNextNode((zxmlNode*&)wSwapNode);
    XMLderegister(wSingleFieldNode);
    wSingleFieldNode=wSwapNode;
  }
  XMLderegister(wRootNode);
  return (int)pErrorlog->hasError();
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
    if (MetaDic->Tab[wMDicRank].Name==pFieldName)
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
    if (MetaDic->Tab[wMDicRank].Name==pFieldName)
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
/**
 * @brief CZKeyDictionary::fieldKeyOffset this routine gives the offset from the beginning of the Key record for the field at Key Dictionary rank pRank.
 * @param[in] pRank relative position (rank) of the field within key dictionary
 * @return the offset from the beginning of the Key for the field at Key Dictionary rank pRank. Returns -1 if field rank is out of dictionary boundaries.
 */
ssize_t ZSKeyDictionary ::fieldKeyOffset (const long pRank)
{
  // compute offset of requested Field within Key

  ssize_t wKeyOffset = 0;

  for (long wi=0;wi<size();wi++)
  {
    if (wi==pRank)
      return wKeyOffset;
    wKeyOffset += Tab[wi].UniversalSize; // KeySize contains the overall size of data field stored in key whatever its type could be
  } // for
  return (-1); // pRank is out of dictionary boundaries
} //fieldOffset


ZStatus
ZSKeyDictionary::addFieldToZDicByName (const utf8_t* pFieldName)
{

  ZSIndexField wField;
  if (MetaDic==nullptr)
  {
    return ZS_INVVALUE;
  }
  zrank_type wMRank=MetaDic->searchFieldByName(pFieldName);
  if (wMRank<0)
    return ZS_NOTFOUND;

  return addFieldToZDicByRank (wMRank);
}//addFieldToZDicByName

ZStatus
ZSKeyDictionary::addFieldToZDicByRank (const zrank_type pMDicRank)
{

  ZSIndexField wField;
  if (MetaDic==nullptr)
  {
    return ZS_INVVALUE;
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
  {

    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVINDEX,
        Severity_Error,
        "Field rank <%ld> <%s> is not eligible to be a key field",
        pMDicRank,
        MetaDic->Tab[pMDicRank].Name.toCChar());
    return ZS_INVINDEX;
  }
  wField.ZType=MetaDic->Tab[pMDicRank].ZType;
  wField.UniversalSize=MetaDic->Tab[pMDicRank].UniversalSize;
  wField.NaturalSize=MetaDic->Tab[pMDicRank].NaturalSize;
  wField.ArrayCount=MetaDic->Tab[pMDicRank].Capacity;
  wField.MDicRank = pMDicRank;
  push(wField);
  Recomputed=false;
  return ZS_SUCCESS;
}// addFieldToZDicByRank


/** @endcond */

