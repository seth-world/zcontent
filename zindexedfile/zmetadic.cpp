#ifndef ZMETADIC_CPP
#define ZMETADIC_CPP

#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zdataconversion.h> // for _getURFHeaderSize

#include <ztoolset/zutfstrings.h>
#include <zxml/zxmlprimitives.h>

using namespace zbs;

ZMetaDic::ZMetaDic() :  ZArray<ZFieldDescription>(cst_ZRF_default_allocation,cst_ZRF_default_extentquota) ,
                        DicName("no name") { }
/**
 * @brief ZMetaDic::addField Main routine for adding dictionary field. All parameters are input.
 * @param[in] pFieldName
 * @param[in] pType
 * @param[in] pNaturalSize
 * @param[in] pUniversalSize
 * @param[in] pArrayCount
 * @return a ZStatus
 */
ZStatus
ZMetaDic::addField (const utf8String& pFieldName,
                   const ZTypeBase pType,
                   const size_t pNaturalSize,
                   const size_t pUniversalSize,
                   const URF_Array_Count_type pArrayCount)
{
    ZFieldDescription wField;
    if (pFieldName==nullptr)
      {
      fprintf (stderr,"ZMetaDic::addField-S-NULNAM Severe error field name is null.\n");
      return ZS_NULLPTR;
      }

  wField.setFieldName(pFieldName);

  wField.ZType=pType;
  wField.HeaderSize = getURFHeaderSize(wField.ZType);
  if (wField.ZType&ZType_VaryingLength)
        {
        wField.NaturalSize=0;
        wField.UniversalSize=0;
        wField.Capacity=0;
        wField.KeyEligible=false;
        }
        else        // store natural and universal size only if type is fixed length
        {
        wField.NaturalSize=pNaturalSize;
        wField.UniversalSize=pUniversalSize;
        wField.Capacity=pArrayCount;
        wField.KeyEligible=true;
        }
  wField.computeMd5();
  push(wField);
  return ZS_SUCCESS;
}//addField


 #ifdef __COMMENT__
/**
 * @brief Natural2UniversalStorage converts a data from its internal representation usable by local computer (Natural)
 *                                  to a ZSMetafile format (Universal)
 *
 *  - whenever necessary atomic data : is converted (sign , endian)
 *  - a header is created with ZType_type
 *  - Arrays or Blobs :
 *        + Size of data is added to header
 *  - then converted data is copied
 *
 * @param pNatural
 * @param pUniversal
 * @param pFieldDesc
 * @return
 */
ZDataBuffer_long* Natural2UniversalStorage(ZDataBuffer_long *pNatural, ZDataBuffer_long *pUniversal,fieldDesc_struct* pFieldDesc)
{
    ZIndexField_struct wFieldDef;
    wFieldDef.ZType =pFieldDesc->Type;
    ZDataBuffer wUniversal;
    pUniversal->setData(&pFieldDesc->Type,sizeof(pFieldDesc->Type));

    CType wType(pFieldDesc->Type);
    switch (wType.getStructure())
    {
    case ZType_Atomic :
        {
        _getAtomicFromRecord(*pNatural,wUniversal,wFieldDef);
        pUniversal->appendData(wUniversal); // no size, only type and content
        }
    case ZType_Array :
        {
        FieldValue.setData(pRecord.Data+Tab[pRank].Offset,_Base::Tab[pRank].Size);
        }
    case ZType_ByteSeq:
        {
        pUniversal->appendData(&pNatural->Size,sizeof(pNatural->Size)); // put data size after the type
        pUniversal->appendData(pNatural); // then the content
        return(pUniversal);
        }
    }//switch
}
ZDataBuffer_long* UniversalStorage2Natural(ZDataBuffer_long *pRecord, const long pRank, ZDataBuffer_long *pFieldValue)
{

}

ZDataBuffer_long *
ZMetaDic::getFieldValue(ZDataBuffer_long *pRecord, const long pRank, ZDataBuffer_long *pFieldValue)
    {
    if ((pRank<0)||(pRank>_Base::size()))
            {
            errno=EINVAL;
            return nullptr;
            }
    _Tp wValue;
    int64_t wOffset=Tab[pRank].Offset;
    int64_t wDataOffset=Tab[pRank].DataOffset;
    int64_t wSize ;

    pFieldValue.setData(pRecord.Data+Tab[pRank].Offset,_Base::Tab[pRank].Size);
    CType wType(Tab[pRank].Type);
    memmove (&wType.Type,pRecord.Data+Tab[pRank].Offset,sizeof(ZType_type));

    switch (wType.getStructure())
    {
    case ZType_Atomic :
        {
        Tab[pRank].Size=wType.getAtomicUniversalSize();
//        FieldValue.setData(pRecord.Data+Tab[pRank].Offset,_Base::Tab[pRank].Size);
        FieldValue.setData(pRecord.Data+Tab[pRank].Offset,Tab[pRank].Size);
        }
    case ZType_Array :
        {
        FieldValue.setData(pRecord.Data+Tab[pRank].Offset,_Base::Tab[pRank].Size);
        }
    case ZType_String:
        {

        }
    }

}// getFieldValue






/**
 * for Atomic values
 */


template <class _Tp>
/**
 * @brief getURFfN_T get Universal Record Format from Natural
 *
 * converts the Natural value into Universal Record Format
 *
 *          - value is casted to desired ZType_type whenever necessary
 *          - value is converted to Universal format
 *          - value is prefixed with Field header
 *
 * @param pValue
 * @param pURF
 * @param pTargetType
 * @param pArrayCount not used for Atomic data
 * @return
 */
inline
ZStatus getURFfN_T (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
                    ZDataBuffer &pURF, // universal record packed field value
                    const ZType_type pTargetType,
                    const long pArrayCount)
{
ZStatus wSt;
ZType_type wSourceType;
ssize_t wNaturalSize,wUniversalSize;
ZDataBuffer wDBV;
ZDataBuffer wDBV1;
    wSt=zgetZType_T<decltype(pValue)>(wSourceType,wNaturalSize,wUniversalSize,true);
    if (wSt!=ZS_SUCCESS)
            return wSt;
    if (wSourceType!=pTargetType)
        {
        wSt=_castAtomicValue_T<decltype(pValue)>(pValue,pTargetType,wDBV);
        if (wSt!=ZS_SUCCESS)
                return wSt;
        }
        else
        {
        wDBV.setData(&pValue,sizeof(pValue));
        }
    wSt=_getAtomicUfN(wDBV,wDBV1,pTargetType);
    // building URF : Header
    pURF.setData(&pTargetType,sizeof(ZType_type)); // for atomic data only ZType_type
    pURF.appendData(wDBV1);   // then put Universal formatted data
    return ZS_SUCCESS;
}
#endif // __COMMENT__
/** @addtogroup ZMetaDicGroup

@{ */

/**
 * @brief ZMetaDic::zsearchFieldByName gets a field position (rank) in the dictionary using its field name
 *
 * @param[in] pFieldName a Cstring that qualifies the name of the field. This name is given while creating the index dictionary.
 * @return the field position (rank) in dictionary. returns -1 if field name has not been found.
 */
zrank_type
ZMetaDic::searchFieldByName(const utf8String& pFieldName)
{
    if (pFieldName.isEmpty())
            return (zrank_type)-1;

    for (long wi=0;wi<size();wi++)
        {
        if (Tab[wi].getName()==pFieldName)
            return (zrank_type)wi;
        }
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVNAME,
                            Severity_Error,
                            " Field name <%s> does not exist within Dictionary named <%s>",
                            pFieldName.toCChar(), DicName.toCChar());

    return (zrank_type)-1;
}//zsearchFieldByName

zrank_type
ZMetaDic::searchFieldByHash(const md5& pHash)
{
  for (long wi=0;wi<size();wi++)
    {
    if (Tab[wi].Hash==pHash)
      return wi;
    }
  ZException.setMessage(_GET_FUNCTION_NAME_,
      ZS_INVNAME,
      Severity_Error,
      " Field with hascode <%s> does not exist within Dictionary named <%s>",
      pHash.toHexa().toChar(), DicName.toCChar());
  return (zrank_type)-1;
}

/**
 * @brief ZMetaDic::print Reports the content of ZMetaDic for all fields
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void ZMetaDic ::print (FILE* pOutput)
{
    fprintf (pOutput,
             "--------------------------------------Meta dictionary content----------------------------------\n"
             " %4s %25s %12s"
             " %9s %9s %9s %9s %s\n",
             "Rank",
             "Name",
             "Key Eligible",
             "Header size",
             "Array Count",
             "Natural",
             "Universal",
             "ZType");
    for (long wi=0;wi<size();wi++)
    {
/*    if (Tab[wi].ZType & ZType_Array) */
/*        const char* wName=(const char*)Tab[wi].Name.toCChar();
        printf ("%s 0x%X \n",wName,Tab[wi].ZType);
*/
        fprintf (pOutput,
                 " <%2ld> %25s %12s"
                 " %9ld %9d %9ld %9ld <%s>\n",
                 wi,
                 Tab[wi].getName().toCChar(),
                 Tab[wi].KeyEligible==true?"Yes":"No",
                 Tab[wi].HeaderSize,
                 Tab[wi].Capacity,
                 Tab[wi].NaturalSize,
                 Tab[wi].UniversalSize,
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
             "----------------------------------------------------------------------------------------------\n");
    return;
}
/**
 * @brief ZMetaDic::zremoveField removes a field which name corresponds to pFieldName from the current key dictionary
 * @param[in] pFieldName user name for the field to be removed from dictionary
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMetaDic::removeFieldByName (const utf8String & pFieldName)
{

    long wRank = searchFieldByName(pFieldName);
    if (wRank<0)
            return ZS_INVNAME;

    erase(wRank);
    return ZS_SUCCESS;
}//removeFieldByName

ZStatus
ZMetaDic::removeFieldByRank (const long pFieldRank)
{

    if ((pFieldRank<0)||(pFieldRank>size()))
                                return ZS_OUTBOUND;

    erase(pFieldRank);
    return ZS_SUCCESS;
}//removeFieldByRank


ZDataBuffer&
ZMetaDic::_exportAppendMetaDicFlat(ZDataBuffer& pZDBExport)
{
  // exporting each rank of key dictionnary : each field definition
  ZDataBuffer wDB;
  FieldDesc_Export wKDExp;
  ZAExport wZAE;
  wZAE = getZAExport();
  wZAE.serialize();
  pZDBExport.append_T<ZAExport>(wZAE);

  for (long wi=0;wi < count(); wi++)
  {
    wKDExp.set(Tab[wi]);            /* export field data  */
    wKDExp.serialize();
    pZDBExport.append_T<FieldDesc_Export>(wKDExp);
    Tab[wi].getName()._exportAppendUVF(pZDBExport);  /* then append field name */
    Tab[wi].ToolTip._exportAppendUVF(pZDBExport);  /* then append Tooltip (if exists) */
  }

  pZDBExport.append_T<uint32_t>(cst_ZBUFFEREND);
  return pZDBExport;
}//_exportAppendFlat

ZStatus
ZMetaDic::_importMetaDicFlat(const unsigned char* &pPtrIn)
{
  FieldDesc_Export wKDExp;
  ZAExport wZAE;
  wZAE.setFromPtr(pPtrIn);
  wZAE.deserialize();
  if (wZAE.StartSign!=cst_ZMSTART) {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_CORRUPTED,
        Severity_Error,
        "ZMetaDic::_importMetaDicFlat Meta dictionary <%s> : start block mark 0xF6F6F6F6 has not been found.",DicName.toCChar());
    return ZS_CORRUPTED;
  }

  clear();
  uint32_t wCheckEnd=0;
  setAllocation(wZAE.AllocatedElements,false);  // no lock
  bzero(0,-1,false);// no lock
  setQuota(wZAE.ExtentQuota);
  setInitialAllocation(wZAE.InitialAllocation,false); // no lock
  if (wZAE.NbElements>(ssize_t)wZAE.AllocatedElements)
    {
    setAllocation(wZAE.NbElements,false);
    }
//  newBlankElement(wZAE.NbElements);
  int wCount=wZAE.NbElements;
  while ( wCount-- && (wCheckEnd!=cst_ZBUFFEREND)) {
      wKDExp.setFromPtr(pPtrIn);
      wKDExp.deserialize();
      ZFieldDescription wFD= wKDExp.toFieldDescription();
      wFD.getName()._importUVF(pPtrIn);  /* import field name */
      wFD.ToolTip._importUVF(pPtrIn); /* then import tooltip */
      push(wFD);
      memmove(&wCheckEnd,pPtrIn,sizeof(uint32_t));
  }
  memmove(&wCheckEnd,pPtrIn,sizeof(uint32_t));
  if (wCheckEnd!=cst_ZBUFFEREND) {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_CORRUPTED,
        Severity_Error,
        " Meta dictionary <%s> : end block mark 0xFBFBFBFB has not been found.",DicName.toCChar());
    return ZS_CORRUPTED;
  }
  pPtrIn += sizeof(uint32_t);
  return ZS_SUCCESS;
}//_importFlat




void
ZMetaDic::writeXML(FILE* pOutput)
{
    fprintf (pOutput,
             "           <ZMetaDic>\n"
             );
     for (long wd=0;wd<size();wd++) // dictionary detail
             {
    fprintf (pOutput,
             "              <Field>\n"
             "                <Rank>%ld</Rank>  <!-- not modifiable : only field position in ZKDic is taken -->\n"
             "                <Name>%s</Name>\n"
             "                <ArrayCount>%d</ArrayCount>\n"
             "                <HeaderSize>%ld</HeaderSize>\n"
             "                <UniversalSize>%ld</UniversalSize>\n"
             "                <NaturalSize>%ld</NaturalSize>\n"
             "                <KeyEligible>%s</KeyEligible>  <!-- not modifiable : defined by ZType -->\n"
             "                <ZType>%s</ZType>   <!-- see ZType_type definition : beware the typos -->\n"
             "              </Field>\n"
             ,
             wd,
             Tab[wd].getName().toCChar() ,
             Tab[wd].Capacity,
             Tab[wd].HeaderSize,
             Tab[wd].UniversalSize,
             Tab[wd].NaturalSize,
             Tab[wd].KeyEligible?"Yes":"No",
             decode_ZType( Tab[wd].ZType)
             );
             }// for
    fprintf (pOutput,
              "          </ZMetaDic>\n");
    return;

}//_writeXML_KDic

/** @endcond */


ZMetaDic&
ZMetaDic::_copyFrom( const ZMetaDic& pIn)
{
  if (CheckSum!=nullptr)
    {
    delete CheckSum;
    CheckSum=nullptr;
    }
  if (pIn.CheckSum!=nullptr)
    CheckSum = new checkSum(*pIn.CheckSum);

  _Base::clear();
  for (long wi=0;wi<pIn.count();wi++)
    push(ZFieldDescription(pIn.Tab[wi]));
  DicName = pIn.DicName;
  Version = pIn.Version;
  CreationDate=pIn.CreationDate;
  ModificationDate=pIn.ModificationDate;
  return *this;
}//_copyFrom

/* ------- C interfaces -------------------*/
zbs::ZArray<ZMetaDic*> ZMetaDicList;

CFUNCTOR void deleteZMetaDicAll();

static bool ZMetaDicInit=false;
bool isZMetaDicInit() {return ZMetaDicInit;}
void initZMetaDic()
{
    ZMetaDicInit=true;
    atexit(&deleteZMetaDicAll);
}

CFUNCTOR void deleteZMetaDicAll()
{
    while (ZMetaDicList.size())
    {
        ZMetaDic* wZRecord= static_cast <ZMetaDic*> (ZMetaDicList.popR());
        delete wZRecord;
    }
    return;
}//deleteZMetaDicAll

APICEXPORT
void* createZMetaDic(void* pMetaDic)
{
    ZMetaDic* wMetaDic = new ZMetaDic();
    if (!isZMetaDicInit())
                initZMetaDic();
    ZMetaDicList.push (wMetaDic);
    return wMetaDic;
}//createZMetaDic

APICEXPORT
void deleteZMetaDic(void* pMetaDic)
{
    for (long wi=0;wi < ZMetaDicList.size(); wi++)
            if (pMetaDic==ZMetaDicList[wi])
                {
                ZMetaDic* wMetaDic= static_cast <ZMetaDic*> (pMetaDic);
                delete wMetaDic;
                ZMetaDicList.erase(wi);
                return;
                }
    fprintf (stderr,"%s-S-NotFound ZMetaDic has not been found while trying to delete it",_GET_FUNCTION_NAME_);
    return ;
}//deleteZMetaDic


/*
  <metadic>
    <dicfields>   <!-- dicfields is required for future use in case of adding parameters to metadic -->
      <field>
        <name>%s</name>
        <capacity>%d</capacity> <!-- if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if atomic -->
        <headersize>%ld</headersize>
        <naturalsize>%ld</naturalsize>
        <universalsize>%ld</universalsize>
        <ztype>%s</ztype>   <!-- see ZType_type definition : converted to its value number -->
        <hash>%s</hash>
      </field>
    </dicfields>
  </metadic>
*/

utf8String ZMetaDic::XmlSaveToString(bool pComment)
{
  utf8String wReturn = fmtXMLdeclaration();
  wReturn += fmtXMLmainVersion("zmetadictionary",__ZDIC_VERSION__,0);
  wReturn += toXml(1,pComment);
  wReturn += fmtXMLendnode("zmetadictionary",0);
  return wReturn;
}

utf8VaryingString ZMetaDic::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel;
  utf8String wReturn;
  ZDataBuffer wB64;
  wReturn = fmtXMLnode("metadictionary",wLevel);
  wLevel++;
  wReturn += fmtXMLversion("version",Version,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," <version> field is the local version to the dictionary data (not to be confused with software version).");
  wReturn += fmtXMLdatefull("creationdate",CreationDate,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," <creationdate>  when dictionary has been firstly created.");

  wReturn += fmtXMLdatefull("modificationdate",ModificationDate,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," <modificationdate>  when dictionary has lastly being saved.");

  if (DicName.isEmpty())
    wReturn += fmtXMLcomment("/dicname/ is missing",wLevel);
  else
    wReturn += fmtXMLchar("dicname",DicName.toCChar(),wLevel);

  wReturn += fmtXMLnode("dicfields",wLevel);
  /* key fields */
  wLevel++;
  for (long wi=0;wi < count();wi++)
    wReturn += Tab[wi].toXml(wLevel,pComment);
  wLevel--;
  wReturn += fmtXMLendnode("dicfields",wLevel);
  wReturn += fmtXMLendnode("metadictionary",pLevel);

  return wReturn;
} // ZMetaDic::toXml


ZStatus ZMetaDic::XmlLoadFromString(const utf8String &pXmlString,bool pCheckHash, ZaiErrors* pErrorLog)
{
  ZStatus wSt;

  zxmlDoc *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;

  wDoc = new zxmlDoc;
  wSt = wDoc->ParseXMLDocFromMemory(pXmlString.toCChar(), pXmlString.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    pErrorLog->logZException();
    pErrorLog->errorLog(
        "ZMetaDic::XmlloadFromString-E-PARSERR Xml parsing error for string <%s> ",
        pXmlString.subString(0, 25).toUtf());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    pErrorLog->logZException();
    return wSt;
  }
  if (!(wRoot->getName() == "metadictionary")) {
    pErrorLog->errorLog(
        "ZMetaDic::XmlloadFromString-E-INVROOT Invalid root node name <%s> expected <metadic>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  wSt = fromXml(wRoot, pCheckHash, pErrorLog);

  XMLderegister((zxmlNode *&) wRoot);

  return wSt;
}//ZMetaDic::XmlLoadFromString

ZStatus ZMetaDic::fromXml(zxmlNode* pMetaDicRootNode, bool pCheckHash,ZaiErrors* pErrorlog)
{
  zxmlElement *wMetaDicNode=nullptr;
  zxmlElement *wFieldsRootNode=nullptr;
  zxmlElement *wSingleFieldNode=nullptr;
  zxmlElement *wSwapNode=nullptr;
  utfcodeString wXmlHexaId;
  ZFieldDescription wFD;
  utf8String wValue;
  utfcodeString wCValue;
  int wErroredFields=0,wWarnedFields=0;

//  ZStatus wSt = pDicRootNode->getChildByName((zxmlNode *&) wMetaDicNode, "metadic");
//  if (wSt != ZS_SUCCESS)

  wMetaDicNode=( zxmlElement *)pMetaDicRootNode;
  if( wMetaDicNode->getName() != "metadictionary")
    {
    pErrorlog->logZStatus(ZAIES_Fatal,
                          ZS_XMLINVROOTNAME,
                          "ZMetaDic::fromXml-F-INVROOTNAME Wrong name <%s> for given root node. Expected <metadictionary> ",wMetaDicNode->getName().toCChar());
    return ZS_XMLINVROOTNAME;
    }

  if (XMLgetChildText(wMetaDicNode,"dicname",DicName,pErrorlog,ZAIES_Warning) <0)
    {
    pErrorlog->logZStatus(ZAIES_Warning,
        ZS_XMLWARNING,
        "ZMetaDic::fromXml-W-CNTFINDND Warning: cannot find node element with name <dicname>. Dictionary has no name.");
    DicName.clear();
    }
  if (XMLgetChildVersion(wMetaDicNode,"version",Version,pErrorlog,ZAIES_Warning) <0)
    {
      pErrorlog->logZStatus(ZAIES_Warning,
          ZS_XMLWARNING,
          "ZMetaDic::fromXml-W-CNTFINDND Warning: cannot find node element with name <version>. Dictionary version set to <1.0.0>.");
      Version=1000000UL;
    }
  if (XMLgetChildZDateFull(wMetaDicNode,"creationdate",CreationDate,pErrorlog,ZAIES_Warning) <0)
    {
      pErrorlog->logZStatus(ZAIES_Warning,
          ZS_XMLWARNING,
          "ZMetaDic::fromXml-W-CNTFINDND Warning: cannot find node element with name <creationdate>. Dictionary version set to <1.0.0>.");
    }
  if (XMLgetChildZDateFull(wMetaDicNode,"modificationdate",ModificationDate,pErrorlog,ZAIES_Warning) <0)
    {
      pErrorlog->logZStatus(ZAIES_Warning,
          ZS_XMLWARNING,
          "ZMetaDic::fromXml-W-CNTFINDND Warning: cannot find node element with name <modificationdate>. Dictionary version set to <1.0.0>.");
    }
  ZStatus wSt=wMetaDicNode->getChildByName((zxmlNode*&)wFieldsRootNode,"dicfields");
  if (wSt!=ZS_SUCCESS)
  {
    pErrorlog->logZStatus(ZAIES_Error,
                          ZS_XMLMISSREQ,
                          "ZMetaDic::fromXml-E-CNTFINDND Error cannot find required node element with name <%s> status <%s>",
                          "dicfields",
                          decode_ZStatus(wSt));
    return ZS_XMLMISSREQ;
  }

  wSt=wFieldsRootNode->getFirstChild((zxmlNode*&)wSingleFieldNode);

  clear();  /* clear dictionary definitions */
  while (wSt==ZS_SUCCESS)
    {
    wFD.clear();
    wSt=wFD.fromXml(wSingleFieldNode,pCheckHash,pErrorlog);
    if (wSt > 0) /* ZS_SUCCESS or ZS_WARNING  -> field is oK */
      push(wFD);
    else
      wErroredFields ++;

    if (wSt==ZS_XMLWARNING)  /* for our accounting */
        wWarnedFields ++;

    wSt=wSingleFieldNode->getNextNode((zxmlNode*&)wSwapNode);
    XMLderegister(wSingleFieldNode);
    wSingleFieldNode=wSwapNode;
    }
  pErrorlog->textLog("ZMetaDic::fromXml___________Field definitions load report____________________\n"
                     " Dictionary name <%s>.\n"
                     " %ld loaded.\n"
                     " %d fields warned.\n"
                     " %d errored and not loaded.",
                      DicName.isEmpty()?"<no name>":DicName.toCChar(),
                      count(), wWarnedFields,
                      wErroredFields);
  XMLderegister(wMetaDicNode);
  XMLderegister(wFieldsRootNode);
  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//ZMetaDic::fromXml



#endif // ZMETADIC_CPP
