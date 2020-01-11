#ifndef ZMETADIC_CPP
#define ZMETADIC_CPP

#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zdatatype.h>

using namespace zbs;

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
ZMetaDic::addField (const utf8_t*pFieldName,
                   const ZTypeBase pType,
                   const size_t pNaturalSize,
                   const size_t pUniversalSize,
                   const URF_Array_Count_type pArrayCount)
{
    FieldDescription wField;
    if (pFieldName==nullptr)
            wField.Name.clear();
        else
        {
        wField.Name=pFieldName;
        if (strlen((const char*)pFieldName)>cst_fieldnamelen)
            {
            fprintf (stderr,"%s-W-ERRLEN field name <%s> is too long for meta dictionary naming standard.\n"
                     "Field name truncated to <%s>",
                     _GET_FUNCTION_NAME_,
                     pFieldName,
                     (char*)wField.Name.toUtf());
            }
        }

    wField.ZType=pType;
    wField.HeaderSize = _getURFHeaderSize(wField.ZType);
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
ZMetaDic::searchFieldByName(const utf8_t* pFieldName)
{
    if (!pFieldName)
            return (zrank_type)-1;
    if (!*pFieldName)
            return (zrank_type)-1;
    for (long wi=0;wi<size();wi++)
            {
        if (Tab[wi].Name==pFieldName)
//        if (!strcmp((char*)Tab[wi].Name.content,pFieldName))
                                            return (zrank_type)wi;
            }
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVNAME,
                            Severity_Error,
                            " Field name <%s> does not exist within Dictionary",
                            pFieldName);

    return (zrank_type)-1;
}//zsearchFieldByName

/**
 * @brief CZKeyDictionary::print Reports the content of CZKeyDictionary for all fields
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
/*        const char* wName=(const char*)Tab[wi].Name.toString();
        printf ("%s 0x%X \n",wName,Tab[wi].ZType);
*/
        fprintf (pOutput,
                 " <%2ld> %25s %12s"
                 " %9ld %9d %9ld %9ld <%s>\n",
                 wi,
                 (char*)Tab[wi].Name.toString(),
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
             Tab[wi].Name.toString(),
             decode_ZType( Tab[wi].ZType));*/
    }// for
    fprintf (pOutput,
             "----------------------------------------------------------------------------------------------\n");
    return;
}
/**
 * @brief CZKeyDictionary::zremoveField removes a field which name corresponds to pFieldName from the current key dictionary
 * @param[in] pFieldName user name for the field to be removed from dictionary
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMetaDic::removeFieldByName (const utf8_t *pFieldName)
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


/** @cond Development
 * @brief fieldDesc_struct::_export exports a field description (a rank of ZKeyDictionary) to a out structure
 */
FieldDesc_Export
FieldDescription::_exportConvert(FieldDescription&pIn,FieldDesc_Export* pOut)
{
ZDataBuffer wZDBName;

    pOut->ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn.ZType);
    pOut->ArrayCount=reverseByteOrder_Conditional<URF_Array_Count_type>(pIn.Capacity);
    pOut->HeaderSize=reverseByteOrder_Conditional<uint64_t>(pIn.HeaderSize);
    pOut->UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn.UniversalSize);
    pOut->NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn.NaturalSize);

    memset (pOut->Name,0,cst_fieldnamelen+1);
    pIn.Name._exportUVF(&wZDBName);
    memmove(pOut->Name,wZDBName.Data,wZDBName.Size);

    pOut->KeyEligible = pIn.KeyEligible;
    return *pOut;
}//fieldDesc_struct::_exportConvert
/**
 * @brief fieldDesc_struct::_import imports a field description (a rank of ZKeyDictionary) from a out structure
 * @param pOut
 * @return
 */
FieldDescription
FieldDescription::_importConvert(FieldDescription& pOut,FieldDesc_Export* pIn)
{
    pOut.clear();
    pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
    pOut.Capacity=reverseByteOrder_Conditional<URF_Array_Count_type>(pIn->ArrayCount);
    pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
    pOut.HeaderSize=reverseByteOrder_Conditional<uint64_t>(pIn->HeaderSize);
    pOut.NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn->NaturalSize);
    pOut.UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn->UniversalSize);

    pOut.Name._importUVF((unsigned char*)pIn->Name);

    pOut.KeyEligible = pIn->KeyEligible;
    return pOut;
}//fieldDesc_struct::_import


/**
 * @brief ZMetaDic::_export export Meta dictionary and returns a ZDataBuffer containing exported dictionary data
 * @param pZDBExport
 * @return
 */

ZDataBuffer&
ZMetaDic::_export(ZDataBuffer& pZDBExport)
{
    unsigned char*wBuf=nullptr;
    size_t wBufSize=0;
    ZAexportCurrent<FieldDescription,FieldDesc_Export>((ZArray <FieldDescription>*)this,
                                                        wBuf,
                                                        wBufSize,
                                                        &FieldDescription::_exportConvert);
    pZDBExport.setData(wBuf,wBufSize);
    free(wBuf);             // mandatory : release allocated memory
    return pZDBExport;
} // ZMetaDic::_export
/**
 * @brief ZMetaDic::_import import Meta dictionary and returns imported size
 * @param pZDBImport_Ptr
 * @return
 */
size_t
ZMetaDic::_import(unsigned char* pZDBImport_Ptr)
{
    ZAExport wZAE;
    // import dictionary content
    size_t wSize= ZAimport<FieldDesc_Export,FieldDescription>
                        ((ZArray <FieldDescription>*)this,
                         pZDBImport_Ptr,
                         &FieldDescription::_importConvert,
                         &wZAE);
// get checkSum for meta dictionary and store it
    ZDataBuffer wZDB(pZDBImport_Ptr,wZAE.FullSize);
    if (CheckSum!=nullptr)
                {
                delete CheckSum;
                CheckSum=nullptr;
                }
    CheckSum = wZDB.newcheckSum();
    return wSize;
}// _import


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
             Tab[wd].Name.toString() ,
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




#endif // ZMETADIC_CPP