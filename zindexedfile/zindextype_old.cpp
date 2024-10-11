#ifndef ZINDEXTYPE_CPP
#define ZINDEXTYPE_CPP

#include <zindexedfile/zindextype.h>
#include <zindexedfile/zindexfile.h>
#include <zindexedfile/zmasterfile.h>

Indexfield_Export &Indexfield_Export::_copyFrom(Indexfield_Export &pIn)
{
    NaturalSize = pIn.NaturalSize;
    UniversalSize = pIn.UniversalSize;
    Capacity = pIn.Capacity;
    ZType = pIn.ZType;
    RecordOffset = pIn.RecordOffset;
    memmove(Name, pIn.Name, sizeof(Name));
    return *this;
}

ZIndexField & ZIndexField::_copyFrom(ZIndexField &pIn)
{
    NaturalSize = pIn.NaturalSize;
    UniversalSize = pIn.UniversalSize;
    Capacity = pIn.Capacity;
    ZType = pIn.ZType;
    RecordOffset = pIn.RecordOffset;
    Name = pIn.Name;
}

Indexfield_Export
ZIndexField::_exportConvert(ZIndexField& pIn,Indexfield_Export* wOut)
{
    ZDataBuffer wZDBName;
//    indexfield_Export wOut;
    wOut->ZType = reverseByteOrder_Conditional<ZTypeBase>(pIn.ZType);
    wOut->Capacity = reverseByteOrder_Conditional<URF_Capacity_type>(pIn.Capacity);
    wOut->RecordOffset = reverseByteOrder_Conditional<uint64_t>(pIn.RecordOffset);
    wOut->UniversalSize = reverseByteOrder_Conditional<uint64_t>(pIn.UniversalSize);
    wOut->NaturalSize = reverseByteOrder_Conditional<uint64_t>(pIn.NaturalSize);
    pIn.Name._exportUVF(&wZDBName);
    memmove(wOut->Name, wZDBName.Data, wZDBName.Size);
    return *wOut;
}
ZIndexField
ZIndexField::_importConvert(ZIndexField& pOut, Indexfield_Export *pIn)
{
    pOut.clear();
    pOut.ZType = reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
    pOut.Capacity = reverseByteOrder_Conditional<URF_Capacity_type>(pIn->Capacity);
    pOut.RecordOffset = reverseByteOrder_Conditional<uint64_t>(pIn->RecordOffset);
    pOut.UniversalSize = reverseByteOrder_Conditional<uint64_t>(pIn->UniversalSize);
    pOut.NaturalSize = reverseByteOrder_Conditional<uint64_t>(pIn->NaturalSize);

    pOut.Name._importUVF((unsigned char *) pIn->Name);
    return pOut;
}
ZDataBuffer &
ZKeyDictionary::_export(ZDataBuffer &pZDBExport)
{
    ZDataBuffer wDB;
    ssize_t wNaturalSizeConv = reverseByteOrder_Conditional<ssize_t>(NaturalSize);
    ssize_t wUniversalSizeConv = reverseByteOrder_Conditional<ssize_t>(UniversalSize);

/*    wDB.setData(&wNaturalSizeConv, sizeof(ssize_t));
    wDB.appendData(&wUniversalSizeConv, sizeof(ssize_t));
*/
    wDB.allocate(sizeof(ssize_t) * 2);
    unsigned char *wDataPtr = wDB.Data;
    memmove(wDataPtr, &wNaturalSizeConv, sizeof(ssize_t));
    wDataPtr += sizeof(ssize_t);
    memmove(wDataPtr, &wUniversalSizeConv, sizeof(ssize_t));

    unsigned char*wBuf=nullptr;
    size_t wBufSize=0;
    ZAexportCurrent<ZIndexField,Indexfield_Export>((ZArray <ZIndexField>*)this,
                                                         wBuf,
                                                         wBufSize,
                                                         &ZIndexField::_exportConvert);
    pZDBExport.appendData(wBuf,wBufSize);
    free(wBuf);             // mandatory : release allocated memory
    return pZDBExport;
} // ZMetaDic::_export

struct ZKDImport_struct
{
    ssize_t NaturalSize;
    ssize_t UniversalSize;
    ZAExport ZAE;
};

size_t
ZKeyDictionary::_import(unsigned char* pZDBImport_Ptr)
{
    ZKDImport_struct *wZKDI = (ZKDImport_struct *)pZDBImport_Ptr;
    ZDataBuffer wDB;
    NaturalSize = reverseByteOrder_Conditional<ssize_t>(wZKDI->NaturalSize);
    UniversalSize = reverseByteOrder_Conditional<ssize_t>(wZKDI->UniversalSize);
    unsigned char* wZDBImport_Ptr = pZDBImport_Ptr + (sizeof(ssize_t) * 2);
    ZAExport wZAE;

    ZArray <ZIndexField>::clear();

    // import ZArray content
    size_t wSize= ZAimport<Indexfield_Export,ZIndexField>
        ((ZArray <ZIndexField>*)this,
         pZDBImport_Ptr,
         &ZIndexField::_importConvert,
         &wZAE);

    size_t wImportSize=wZAE.FullSize+(sizeof(ssize_t)*2);

    // get checkSum for key dictionary and store it
    ZDataBuffer wZDB(pZDBImport_Ptr,wImportSize);
    if (CheckSum!=nullptr)
    {
        delete CheckSum;
        CheckSum=nullptr;
    }
    CheckSum = wZDB.newcheckSum();

              // mandatory : release allocated memory
    return wImportSize;
} // ZKeyDictionary::_import



/** @addtogroup ZIndexFileGroup

@{ */

/** @brief _addFieldToZDic : Adds a Field definition to a Key dictionary (fields description list).
 *
 * - Analyzes the type of data using zgetZType() function.
 * - Deduces the data type (ZType_type) and true natural data length as well as internal data length.
 *
 * @note template is required here because 'auto' type hides arrays (char [n] is converted to char*).
 *
 * @param[in] pFieldName user name of the field. Only for readability.
 * @param[in] pMetaDic   Meta dictionary from which field may be added to index dictionary
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 *
 */

ZStatus
ZKeyDictionary::addFieldToZDic (const utf8_t* pFieldName)
{

ZIndexField wField;
    if (MetaDic==nullptr)
            {
            return ZS_INVVALUE;
            }
    long wMRank=MetaDic->searchFieldByName(pFieldName);
    if (wMRank<0)
            return ZS_NOTFOUND;
    wField.ZType=MetaDic->Tab[wMRank].ZType;
    wField.UniversalSize=MetaDic->Tab[wMRank].UniversalSize;
    wField.NaturalSize=MetaDic->Tab[wMRank].NaturalSize;
    wField.ArrayCount=MetaDic->Tab[wMRank].ArrayCount;
    push(wField);

    _reComputeSize();
    return ZS_SUCCESS;
}//addFieldToZDic

ZStatus
ZKeyDictionary::addFieldToZDic (const long pMDicRank)
{
ZStatus wSt;
ZIndexField wField;
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
                                  MetaDic->Tab[pMDicRank].Name);
            return ZS_INVINDEX;
            }
    wField.ZType=MetaDic->Tab[pMDicRank].ZType;
    wField.UniversalSize=MetaDic->Tab[pMDicRank].UniversalSize;
    wField.NaturalSize=MetaDic->Tab[pMDicRank].NaturalSize;
    wField.ArrayCount=MetaDic->Tab[pMDicRank].Capacity;
    wField.MDicRank = pMDicRank;
    push(wField);

    _reComputeSize();
    return ZS_SUCCESS;
}
/**
 * @brief CZKeyDictionary::zgetFieldRank gets a field position (rank) in the key dictionary using its field name
 *
 * @param[in] pFieldName a Cstring that qualifies the name of the field. This name is given while creating the index dictionary.
 * @return the field position (rank) in dictionary. returns -1 if field name has not been found.
 */
long
ZKeyDictionary::zsearchFieldByName(const char* pFieldName)
{
    for (long wi=0;wi<size();wi++)
            if (MetaDic->Tab[Tab[wi].MDicRank].Name==pFieldName)
                                    return wi;
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVNAME,
                            Severity_Error,
                            " Field name <%s> does not exist within ZDictionary",
                            pFieldName);

    return -1;
}
/**
 * @brief CZKeyDictionary::zgetFieldRank gets a field position (rank) in the key dictionary using its field name
 *
 * @param[in] pFieldName a Cstring that qualifies the name of the field. This name is given while creating the index dictionary.
 * @return the field position (rank) in dictionary. returns -1 if field name has not been found.
 */
long
ZKeyDictionary::zsearchFieldByName(utfdescString &pFieldName)
{
    for (long wi=0;wi<size();wi++)
            if (MetaDic->Tab[Tab[wi].MDicRank].Name==pFieldName.toString())
                                    return wi;
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVNAME,
                            Severity_Error,
                            " Field name <%s> does not exist within ZDictionary",
                            pFieldName.toString());

    return -1;
}
/**
 * @brief CZKeyDictionary::print Reports the content of CZKeyDictionary for all fields
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void ZKeyDictionary ::print (FILE* pOutput)
{
    fprintf (pOutput,
             "-----------------------ZKeyFieldList content-------------------\n"
             " %4s %8s %8s %8s %8s %17s %s\n",
             "rank",
             "MDicRank",
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
             Tab[wi].MDicRank,
             MetaDic->Tab[Tab[wi].MDicRank].NaturalSize,
             MetaDic->Tab[Tab[wi].MDicRank].UniversalSize,
             MetaDic->Tab[Tab[wi].MDicRank].ArrayCount,
             MetaDic->Tab[Tab[wi].MDicRank].Name.toString(),
             decode_ZType( MetaDic->Tab[Tab[wi].MDicRank].ZType));
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
             "---------------------------------------------------------------\n");
    return;
}

/**
 * @brief CZKeyDictionary::fieldKeyOffset this routine gives the offset from the beginning of the Key for the field at Key Dictionary rank pRank.
 * @param[in] pRank relative position (rank) of the field within key dictionary
 * @return the offset from the beginning of the Key for the field at Key Dictionary rank pRank. Returns -1 if field rank is out of dictionary boundaries.
 */
ssize_t ZKeyDictionary ::fieldKeyOffset (const long pRank)
{
// compute offset of requested Field within Key

ssize_t wKeyOffset = 0;

for (long wi=0;wi<size();wi++)
           {
           if (wi==pRank)
                    return wKeyOffset;
           wKeyOffset += Tab[wi].UniversalSize; //! KeySize contains the overall size of data field stored in key whatever its type could be
           } // for
return (-1); // pRank is out of dictionary boundaries
} //fieldOffset
/**
 * @brief CZKeyDictionary::fieldRecordOffset this routine gives the offset from the beginning of the ZMasterFile user's record for the field at Key Dictionary rank pRank.
 * @param[in] pRank relative position (rank) of the field within key dictionary
 * @return the offset from the beginning of the ZMasterFile user's record for the field at Key Dictionary rank pRank. Returns -1 if field rank is out of dictionary boundaries.
 */
ssize_t ZKeyDictionary ::fieldRecordOffset (const long pRank)
{

    return Tab[pRank].RecordOffset;

} //fieldOffset

/**
 * @brief CZKeyDictionary::zremoveField removes a field which name corresponds to pFieldName from the current key dictionary
 * @param[in] pFieldName user name for the field to be removed from dictionary
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZKeyDictionary::zremoveField (const char *pFieldName)
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
template <class _Tp>
ZStatus
ZKeyDictionary::zsetField (const long pFieldRank,ZKeyDic_type pZKD,_Tp pValue)
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
    case ZKD_ArrayCount :
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
/*    case ZKD_RecordOffset :
    {
        Tab[pFieldRank].RecordOffset = pValue;
        return ZS_SUCCESS;
    }
*/
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


/** @cond Development
 * @brief CZKeyDictionary::_reComputeSize computes the total sizes : Natural size and Internal size for the whole key
 */
void
ZKeyDictionary::_reComputeSize (void)
{
    NaturalSize=0;
    UniversalSize=0;
    for (long wi=0;wi<size(); wi++)
                                        {
                                        NaturalSize += Tab[wi].NaturalSize ;
                                        UniversalSize += Tab[wi].UniversalSize ;
                                        }
    return;
}


/** @endcond */




ZIndexControlBlock::ZIndexControlBlock ()
{
    clear();
    return;
}
ZIndexControlBlock::~ZIndexControlBlock (void)
{
/*    if (ZKDic!=nullptr)
                {
                delete ZKDic ;
                ZKDic=nullptr;
                }*/
    return;
}
/**
 * @brief ZIndexControlBlock::clear Resets ZIndexControlBlock to initial data - reset Key Dictionnary
 */
void
ZIndexControlBlock::clear(ZMetaDic *pMetaDic, ZMasterFile *pZMFFather)
{
    BlockID=ZBID_ICB;
    ZMFVersion =  __ZMF_VERSION__ ;
    Name.clear();
    BlockID = ZBID_ICB;
    StartSign=cst_ZSTART;
    Duplicates = ZST_NODUPLICATES;
    AutoRebuild = false;
    if (ZKDic!=nullptr)
                {
                delete ZKDic ;
                ZKDic = nullptr;
                }
    ZKDic = new ZKeyDictionary(pMetaDic) ;

    MetaDic = pMetaDic;
    ZMFFather = pZMFFather;
    return;
}//clear

//------------- End CZKeyDictionary---------------------------------------







//

//--------------- Get data from Record----------------------------------------------

static inline
unsigned char* _reverseByteOrder(unsigned char* pValue, ssize_t pSize)
{
ZDataBuffer wValue;
    wValue.setData( pValue,pSize);

    int wj=0;
    int wi=pSize-1;

    unsigned char* wPtr = (unsigned char*)pValue;
    unsigned char* wPtr1 = (unsigned char*)wValue.Data;
    while(wi >= 0)
            {
            wPtr1[wj] =wPtr[wi];
            wj++;
            wi--;
            }

    memmove(pValue,wValue.Data,pSize);
    return pValue;
}

template <class _Tp>
/**
 * @brief _getKeyAFR  get Key Atomic (field) From Record
 *
Extracts an ZType_Atomic field from an Record field value  (ZDataBuffer).
reverse byte order if necessary (Endian) to set it back to its natural data type
and returns
  - a ZDataBuffer with the formatted field content ready to be used as a natural data type in the given ZDataBuffer pOutData
  - the atomic value data field as well as a return value

Size of the resulting field is the size of natural data type (ex : int8_t is 1 ), and Size field of ZDataBuffer pOutData is set to correct length.

 *
 * @param[in] pInData       Record buffer to extract field from
 * @param[out] pOutData     Field content extracted from key and formatted back to Natural (computer) data type
 * @param[in] pZType        Type mask of data ( ZType_type )
 * @return              The pure natural value of the field converted from Key field content (a reference to pOutData ).
 */
static inline
_Tp _getKeyAFR(unsigned char* pInData,/*const ssize_t pSize,*/ ZDataBuffer &pOutData,const ZTypeBase pZType )
{
    _Tp wValue , wValue2;

//    memmove(&wValue,(pInData.Data+pField.Offset),sizeof(wValue));
    memmove(&wValue,pInData,sizeof(wValue));

    if (wValue < 0)
                wValue2 = -wValue;
            else
                wValue2 = wValue;

    if (is_little_endian())             // only if system is little endian
          if (pZType & ZType_Endian)    // and data type is subject to endian reverse byte conversion
                        wValue2= _reverseByteOrder<_Tp>(wValue2);

    if (pZType & ZType_Signed)
            {

            pOutData.allocate(sizeof(_Tp)+1);   // unsigned means size + sign byte
            if (wValue<0)  // if negative value
                    {
                    pOutData.Data[0]=0; // sign byte is set to Zero
                    _negate (wValue2);
                    }
                else
                    pOutData.Data[0]=1;

            memmove(pOutData.Data+1,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
            return wValue;
            }
// up to here : unsigned data type
//
//    pOutData.allocate(sizeof(_Tp));
    pOutData.setData(&wValue,sizeof(_Tp));// unsigned means same size as input data type
    return wValue;
} // _getKeyAFR

/**
 * @brief _negate Complements any Atomic data value (not operation, byte per byte)
 * @param pValue
 */
auto _negate(auto &pValue)
{
    unsigned char *wValuePtr=(unsigned char*)&pValue;
    unsigned char wValueUChar;
//                       _Tp wANDFF ;
//                       memset (&wANDFF,0xFF,sizeof(_Tp));
//                       wValue = wValue & wANDFF;         //! absolute value of data is ANDED with 0xFF bytes on the size of the data type
//                        wValue = ~ wValue;          //! not (complement to 0xFF )
    for (size_t wi=0;wi<sizeof(pValue);wi++)
            {
                wValueUChar = wValuePtr[wi];
                wValuePtr[wi] = ~ wValueUChar;
            }
    return (pValue);
}// _negate
#ifdef __COMMENT__
/**
 * @brief _negate Complements any Atomic data value (not operation, byte per byte)
 * @param pValue
 */
unsigned char* _negate(unsigned char* pValue, ssize_t pSize)
{
    unsigned char *wValuePtr=pValue;
    unsigned char wValueUChar;
//                       _Tp wANDFF ;
//                       memset (&wANDFF,0xFF,sizeof(_Tp));
//                       wValue = wValue & wANDFF;         //! absolute value of data is ANDED with 0xFF bytes on the size of the data type
//                        wValue = ~ wValue;          //! not (complement to 0xFF )
    for (size_t wi=0;wi<pSize;wi++)
            {
                wValueUChar = wValuePtr[wi];
                wValuePtr[wi] = ~ wValueUChar;
            }
    return (pValue);
}// _negate
#endif // __COMMENT__
template <class _Tp>
/**
 * @brief _getValueAFK  get Atomic ( value ) From  Key
 *
Extracts an ZType_Atomic field from a ZDataBuffer (Key content) :

- from its offset since beginning of key using routine ZKeyDictionary::fieldOffset
- using the length deduced from its originary type.

Example : For a field stored in a ZIndex key that has an originary data type (ZType_type) mentionning an int32_t,
its Key length is computed to 5 ( 4 bytes plus 1 sign byte ).

The size of the result is the size of the originary data type of the field (In the above example : 4 bytes).
- further processing :
Data value byte order is reversed according Endian convention IF NECESSARY (system is little Endian).

 *
 * @param pKeyData       Key buffer to extract field from
 * @param pOutData      Field content extracted and formatted up to be used as key field
 * @param pField        Key Dictionary rank content
 * @return The obtained value with _Tp template parameter type.
 */
static inline
_Tp _getValueAFK (ZDataBuffer &pKeyData,const long pRank,ZKeyDictionary & pFieldList)
{
    _Tp wValue;
    ZDataBuffer wDBV;
    ssize_t wFieldOffset = pFieldList.fieldKeyOffset(pRank);

    if (wFieldOffset<0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Fatal,
                                    " Field rank is out of key dictionary boundaries while computing field offset given rank <%ld> vs dicionary size <%ld>",
                                    pRank,
                                    pFieldList.size());
            ZException.exit_abort();
            }

    wDBV.setData(pKeyData.Data+wFieldOffset,pFieldList[pRank].UniversalSize);
    if (pFieldList[pRank].ZType & ZType_Signed)
            {
            if (wDBV.Data[0]==0)  //! signed negative value
                    {
                    _negate(wDBV.Data+1,wDBV.Size-1);
                    if (is_little_endian())             //! only if system is little endian
                          if (pFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                                        _reverseByteOrder(wDBV.Data+1,(ssize_t)(wDBV.Size-1));
                    memmove(&wValue,(wDBV.Data+1),sizeof(_Tp));
                    wValue = -wValue;
                    return wValue;
                    }
                else //! it is a positive value
                {
                if (is_little_endian())             //! only if system is little endian
                      if (pFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                                    _reverseByteOrder(wDBV.Data+1,wDBV.Size-1);
                memmove(&wValue,(wDBV.Data+1),sizeof(_Tp));
                return wValue;
                }
            }
 // not signed : means no byte sign

    memmove(&wValue,(pKeyData.Data+wFieldOffset),sizeof(wValue));
    if (is_little_endian())             //! only if system is little endian
          if (pFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                         _reverseByteOrder(wDBV.Data,sizeof(_Tp));

    wValue = static_cast<_Tp>(*wDBV.Data);
    return wValue;
} // _getValueAFK
/**
 * @brief _getValueAAFK  get Array Atomic ( value ) From  Key
 *
Extracts an ZType_Atomic field from a ZDataBuffer (Key content) :

- from its offset since beginning of key using routine ZKeyDictionary::fieldOffset
- using the length deduced from its originary type.

Example: For a field stored in a ZIndex key that has an originary data type (ZType_type) mentionning an int32_t,
its Key length is computed to 5 ( 4 bytes plus 1 sign byte ).

The size of the result is the size of the originary data type of the field (In the above example : 4 bytes).
- further processing :
Data value byte order is reversed according Endian convention IF NECESSARY (system is little Endian).

 *
 * @param pKeyData      Key buffer to extract field from
 * @param pIndex        Index of the array to get value from
 * @param pRank         Rank of field within dictionary
 * @param pFieldList    The key field dictionary
 * @return The obtained value with _Tp template parameter type : Field content extracted and formatted up to be used as key field
 */
template <class _Tp>
static inline
_Tp _getValueAAFK (ZDataBuffer &pKeyData,const long pIndex,const long pRank,ZKeyDictionary & pFieldList)
{
    _Tp wValue;
    ZDataBuffer wDBV;
//    ssize_t wFieldOffset = pFieldList.fieldKeyOffset(pRank);
    ssize_t wElementSize = pFieldList[pRank].UniversalSize / pFieldList[pRank].ArrayCount; //! unary element size within array
    ssize_t wFieldOffset = pFieldList.fieldKeyOffset(pRank) + (wElementSize*pIndex) ;


    if ((wFieldOffset<0)||(pIndex>pFieldList[pRank].ArrayCount))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Fatal,
                                    " Field rank is out of key dictionary boundaries while computing field offset given rank <%ld> vs dicionary size <%ld>",
                                    pRank,
                                    pFieldList.size());
            ZException.exit_abort();
            }

//    wDBV.setData(pKeyData.Data+wFieldOffset,pFieldList[pRank].KeySize);
    wDBV.setData(pKeyData.Data+wFieldOffset,wElementSize);

    if (pFieldList[pRank].ZType & ZType_Signed)
            {
            if (wDBV.Data[0]==0)  //! signed negative value
                    {
                    _negate(wDBV.Data+1,wDBV.Size-1);
                    if (is_little_endian())             //! only if system is little endian
                          if (pFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                                        _reverseByteOrder(wDBV.Data+1,(ssize_t)(wDBV.Size-1));
                    memmove(&wValue,(wDBV.Data+1),sizeof(_Tp));
                    wValue = -wValue;
                    return wValue;
                    }
                else //! it is a positive value
                {
                if (is_little_endian())             //! only if system is little endian
                      if (pFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                                    _reverseByteOrder(wDBV.Data+1,wDBV.Size-1);
                memmove(&wValue,(wDBV.Data+1),sizeof(_Tp));
                return wValue;
                }
            }
 // not signed : means no byte sign

    memmove(&wValue,(pKeyData.Data+wFieldOffset),sizeof(wValue));
    if (is_little_endian())             //! only if system is little endian
          if (pFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                         _reverseByteOrder(wDBV.Data,sizeof(_Tp));

    wValue = static_cast<_Tp>(*wDBV.Data);
    return wValue;
} // _getValueAAFK  for Arrays


//------------------Extract Key field from record------------------------------------------------------------
#ifdef __COMMENT__
void
_getValueFromStdString (void *pIndata, ZDataBuffer &pOutData)
{
std::string*wString=    static_cast<std::string*>(pIndata) ;

    pOutData.setData((char*)wString->c_str(),wString->size());
    return;
}
#endif // __COMMENT__
template  <class _Tp>
ZStatus _getByteSequenceFromRecord (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp>  pInData, const ssize_t pSize, ZDataBuffer &pOutData, ZIndexField& pField)
{
    pOutData.setData(pInData,pSize);
    return ZS_SUCCESS;
}

/**
 * @brief _getArrayFromRecord Obtains a ZDataBuffer content ready to be used as a Key (concatenated to any other field if ever)

Uses _getKeyAFR template routine to pack field according system constraints (big / little endian) and leading sign byte.

Size of the returned ZDataBuffer content is

- size of Atomic data + 1 byte (leading sign byte) if signed value type,
- size of Atomic data if unsigned value type.

 * @param pInData   ZDataBuffer containing the user record to extract field from.
 * @param pOutData  ZDataBuffer containing as a return the packed/extracted field value converted as ready to be used as a key field
 * @param pField    ZIndex Key dictionary rank describing the data to extract.
 * @return          A reference to ZDataBuffer containing the packed data field.
 */
ZStatus _getArrayFromRecord(ZDataBuffer &pInData,ZDataBuffer &pOutData,ZIndexField & pField)
{

 ZTypeBase wZType = pField.ZType;

 if (!(wZType & ZType_Array))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%ld> <%s> encountered while processing record data. Type is NOT ARRAY.",
                                 pField.ZType,
                                 decode_ZType(pField.ZType));
        return ZS_INVTYPE;
        }
 wZType = wZType &(~ZType_StructureMask) ;  // negate ZType_Array : do not need it anymore


// long wEltOffsetIn=pField.RecordOffset;
long wEltOffsetIn=0;
ZDataBuffer wDBElt;
//ZDataBuffer wDBIn;
ZIndexField wField;
//    wField.RecordOffset = 0 ;
//    wDBIn.setData(pInData.Data+pField.Offset,pField.NaturalSize);

 if (!(wZType&ZType_Signed)&&(!is_little_endian()||!(wZType&ZType_Endian)))
                                        {
//                                    pOutData.setData(pInData.Data+pField.RecordOffset,pField.NaturalSize);
                                    pOutData.setData(pInData.Data,pField.NaturalSize);
                                    return pOutData;
                                        }

 pOutData.clear();
 for (long wi=0;wi<pField.ArrayCount;wi++)
 {
 switch (wZType)
 {
    case ZType_Char:    // char MUST not be converted with leading sign byte : this is a C string.
    case ZType_UChar:
    case ZType_U8 :
             {
             _getKeyAFR<uint8_t>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
             wEltOffsetIn += sizeof(uint8_t);
             break;
             }
    case ZType_S8 :
            {
             _getKeyAFR<int8_t>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
            wEltOffsetIn += sizeof(int8_t);
            break;
            }

     case ZType_U16 :
              {
             _getKeyAFR<uint16_t>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
              wEltOffsetIn += sizeof(uint16_t);
              break;
              }
     case ZType_S16 :
             {
             _getKeyAFR<int16_t>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
             wEltOffsetIn += sizeof(int16_t);
             break;
             }
     case ZType_U32 :
              {
             _getKeyAFR<uint32_t>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
             wEltOffsetIn += sizeof(uint32_t);
             break;
              }
     case ZType_S32 :
             {
             _getKeyAFR<int32_t>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
             wEltOffsetIn += sizeof(int32_t);
             break;
             }
     case ZType_U64 :
              {
            _getKeyAFR<uint64_t>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt );
             wEltOffsetIn += sizeof(uint64_t);
             break;
              }
     case ZType_S64 :
             {
            _getKeyAFR<int64_t>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData( wDBElt);
             wEltOffsetIn += sizeof(int64_t);
             break;
             }
    case ZType_Float :
                 {
             _getKeyAFR<float>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
             wEltOffsetIn += sizeof(float);
             break;
                 }
     case ZType_Double :
                  {
             _getKeyAFR<double>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
             wEltOffsetIn += sizeof(double);
             break;
                  }
     case ZType_LDouble :
                  {
             _getKeyAFR<long double>(pInData.Data+wEltOffsetIn,wDBElt,pField.ZType);
             pOutData.appendData(wDBElt);
             wEltOffsetIn += sizeof(long double);
             break;
                  }

        default:
                 {
                     ZException.setMessage(_GET_FUNCTION_NAME_,
                                             ZS_INVTYPE,
                                             Severity_Fatal,
                                             "Invalid atomic ZType <%ld> <%s> encountered while processing record data",
                                             wZType,
                                             decode_ZType(wZType));
//                     pField.ZType,
//                     decode_ZType(pField.ZType));
                     ZException.exit_abort();
                 }

    }//switch

    }// for
 return wDBElt;
}//_getArrayFromRecord



/**
 * @brief _getAtomicFromRecord Obtains a ZDataBuffer content ready to be used as a Key (concatenated to any other field if ever)

  Uses _getKeyAFR template routine to pack field according system constraints (big / little endian) and leading sign byte.

  - Size of the returned ZDataBuffer content is
      + size of Atomic data + 1 byte (leading sign byte) if signed value type,
      + size of Atomic data if unsigned value type.

  - Byte order is reversed if system is little Endian

  ZIndexField_struct details reminder
  - ssize_t     Offset;         < Offset of the Field from the beginning of record
  - ssize_t     NaturalSize;    < Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
  - ssize_t     KeyFieldSize;   < length of the field when stored into Key (Field dictionary internal format size)
  - long        ZType;          < Type mask of the Field @see ZType_type
  - long        ArrayCount;      < in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArrayCount or KeySize / ArrayCount ). For other storage type, this field is set to 1.

 * @param[in] pInData   ZDataBuffer containing the user record to extract field from.
 * @param[out] pOutData  ZDataBuffer containing as a return the packed/extracted field value converted as ready to be used as a key field
 * @param[in] pField    ZIndex Key dictionary rank (ZIndexField_struct) describing the data field to extract.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus _getAtomicFromRecord(ZDataBuffer &pInData,ZDataBuffer &pOutData,ZIndexField & pField)
{

 ZTypeBase wZType = pField.ZType;
 long wOffset=0;

 if (!(wZType & ZType_Atomic))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%ld> <%s> encountered while processing record data. Type is NOT ATOMIC.",
                                 pField.ZType,
                                 decode_ZType(pField.ZType));
         return ZS_INVTYPE;
        }
 wZType = wZType &(~ZType_Atomic) ;  // negate ZType_Atomic : do not need it anymore

 switch (wZType)
 {
case ZType_U8 :
         {
          _getKeyAFR<uint8_t>(pInData.Data+wOffset,pOutData,pField.ZType);
          return ZS_SUCCESS;
         }
case ZType_S8 :
        {
        _getKeyAFR<int8_t>(pInData.Data+wOffset,pOutData,pField.ZType);
        return ZS_SUCCESS;
        }

 case ZType_U16 :
          {
          _getKeyAFR<uint16_t>(pInData.Data+wOffset,pOutData,pField.ZType);
          return ZS_SUCCESS;
          }
 case ZType_S16 :
         {
         _getKeyAFR<int16_t>(pInData.Data+wOffset,pOutData,pField.ZType);
         return ZS_SUCCESS;
         }
 case ZType_U32 :
          {
           _getKeyAFR<uint32_t>(pInData.Data+wOffset,pOutData,pField.ZType);
           return ZS_SUCCESS;
          }
 case ZType_S32 :
         {
         _getKeyAFR<int32_t>(pInData.Data+wOffset,pOutData,pField.ZType);
         return ZS_SUCCESS;
         }
 case ZType_U64 :
          {
          _getKeyAFR<uint64_t>(pInData.Data+wOffset,pOutData,pField.ZType);
          return ZS_SUCCESS;
          }
 case ZType_S64 :
         {
         _getKeyAFR<int64_t>(pInData.Data+wOffset,pOutData,pField.ZType);
         return ZS_SUCCESS;
         }
case ZType_Float :
             {
              _getKeyAFR<float>(pInData.Data+wOffset,pOutData,pField.ZType);
              return ZS_SUCCESS;
             }
 case ZType_Double :
              {
              _getKeyAFR<double>(pInData.Data+wOffset,pOutData,pField.ZType);
              return ZS_SUCCESS;
              }
 case ZType_LDouble :
              {
               _getKeyAFR<long double>(pInData.Data+wOffset,pOutData,pField.ZType);
               return ZS_SUCCESS;
              }

    default:
             {
                 ZException.setMessage(_GET_FUNCTION_NAME_,
                                         ZS_INVTYPE,
                                         Severity_Fatal,
                                         "Invalid ZType <%ld> <%s> encountered while processing record data",
                                         pField.ZType,
                                         decode_ZType(pField.ZType));
                 return ZS_INVTYPE;
             }

 }//switch

}//_getAtomicFromRecord

//------------------------------- Conversion back from key to natural value--------------------------------------------------------------

/**
 * @brief _getFieldValueFromKey Obtains a single field value from a Key content (pKeyData) using its key dictionary definition (CZKeyDictionary and pRank).
 *                      Single field value is returned in a ZDataBuffer AVFKValue containing the natural field value with appropriate natural data type size.
 *  @note only proper key index content is taken : it means that ZMF address must be omitted from the data.
 *
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] AVFKValue field value in natural format ready to be used by application program
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pFieldList Index dictionary as a CZKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus _getFieldValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZKeyDictionary & pFieldList)
{
    ZTypeBase wZType = pFieldList[pRank].ZType;

    if (wZType & ZType_Atomic)
            return _getAtomicValueFromKey(pKeyData,AVFKValue,pRank,pFieldList);
    if (wZType & ZType_Array)
            return _getArrayValueFromKey(pKeyData,AVFKValue,pRank,pFieldList);
    if (wZType & ZType_Class)
            return _getClassValueFromKey(pKeyData,AVFKValue,pRank,pFieldList);

}//_getValueFromKey


/**
 * @brief _getAtomicValueFromKey Obtains a single Atomic field value from a Key content (pKeyData) using its key dictionary definition (CZKeyDictionary and pRank).
 *
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] AVFKValue field value in natural format ready to be used by application program
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pFieldList Index dictionary as a CZKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorSError
 */

ZStatus _getAtomicValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZKeyDictionary & pFieldList)
{

 ZTypeBase wZType = pFieldList[pRank].ZType;

 if (!(wZType & ZType_Atomic))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%ld> <%s> encountered while processing key data. Type is NOT ATOMIC.",
                                 pFieldList[pRank].ZType,
                                 decode_ZType(pFieldList[pRank].ZType));
         return ZS_INVTYPE;
        }
 wZType = wZType &(~ZType_Atomic) ;  //! negate ZType_Atomic : do not need it anymore



 switch (wZType)
 {
case ZType_U8 :
         {
         uint8_t wValue= _getValueAFK<uint8_t>(pKeyData,pRank,pFieldList);
         AVFKValue.setData( &wValue, sizeof(wValue));
         break;
         }
case ZType_S8 :
        {
         int8_t wValue= _getValueAFK<int8_t>(pKeyData,pRank,pFieldList);
        AVFKValue.setData( &wValue, sizeof(wValue));
         break;
        }

 case ZType_U16 :
          {
          uint16_t wValue= _getValueAFK<uint16_t>(pKeyData,pRank,pFieldList);
          AVFKValue.setData( &wValue, sizeof(wValue));
          break;
          }
 case ZType_S16 :
         {
         int16_t wValue= _getValueAFK<int16_t>(pKeyData,pRank,pFieldList);
         AVFKValue.setData( &wValue, sizeof(wValue));
         break;
         }
 case ZType_U32 :
          {
         uint32_t wValue= _getValueAFK<uint32_t>(pKeyData,pRank,pFieldList);
         AVFKValue.setData( &wValue, sizeof(wValue));
         break;
          }
 case ZType_S32 :
         {
         int32_t wValue= _getValueAFK<int32_t>(pKeyData,pRank,pFieldList);
         AVFKValue.setData( &wValue, sizeof(wValue));
         break;
         }
 case ZType_U64 :
          {
         uint64_t wValue= _getValueAFK<uint64_t>(pKeyData,pRank,pFieldList);
         AVFKValue.setData( &wValue, sizeof(wValue));
         break;
          }
 case ZType_S64 :
         {
         int64_t wValue= _getValueAFK<int64_t>(pKeyData,pRank,pFieldList);
         AVFKValue.setData( &wValue, sizeof(wValue));
         break;
         }
case ZType_Float :
             {
             float wValue= _getValueAFK<float>(pKeyData,pRank,pFieldList);
             AVFKValue.setData( &wValue, sizeof(wValue));
             break;
             }
 case ZType_Double :
              {
             double wValue= _getValueAFK<double>(pKeyData,pRank,pFieldList);
             AVFKValue.setData( &wValue, sizeof(wValue));
             break;
              }
 case ZType_LDouble :
              {
             long double wValue= _getValueAFK<long double>(pKeyData,pRank,pFieldList);
             AVFKValue.setData( &wValue, sizeof(wValue));
             break;
              }

    default:
             {
//     auto wValue= _getValueAFK<int8_t>(pKeyData,pRank,pFieldList);
//     AVFKValue.setData( &wValue, sizeof(wValue));
             AVFKValue = "####" ;
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                     ZS_INVTYPE,
                                     Severity_Fatal,
                                     "Invalid ZType <%ld> <%s> encountered while processing key data",
                                     pFieldList[pRank].ZType,
                                     decode_ZType(pFieldList[pRank].ZType));
             return ZS_INVTYPE;
             }

 }//switch
    return ZS_SUCCESS;
}//_getAtomicValueFromKey

/**
 * @brief _getClassValueFromKey  Nothing is done here : class are returned without conversion from key offset on key field length
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] AVFKValue field value in natural format ready to be used by application program
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pFieldList Index dictionary as a CZKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorSError
 */
ZStatus _getClassValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZKeyDictionary & pFieldList)
{
    if (!(pFieldList[pRank].ZType & ZType_Class))
           {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVTYPE,
                                    Severity_Fatal,
                                    "Invalid ZType <%ld> <%s> encountered while processing key data. Type is NOT CLASS.",
                                    pFieldList[pRank].ZType,
                                    decode_ZType(pFieldList[pRank].ZType));
            return ZS_INVTYPE;
           }

    AVFKValue.setData(&pKeyData.Data[pFieldList.fieldKeyOffset(pRank)],pFieldList[pRank].UniversalSize);
    return ZS_SUCCESS;
}//_getClassValueFromKey

/**
 * @brief _getArrayValueFromKey Obtains a field natural value of an Array field from it Key internal value
 *
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] AVFKValue field value in natural format ready to be used by application program
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pFieldList Index dictionary as a CZKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorSError
 */
ZStatus _getArrayValueFromKey(ZDataBuffer &pInData,ZDataBuffer &pOutData,const long pRank,ZKeyDictionary & pFieldList)
{

 ZTypeBase wZType = pFieldList[pRank].ZType;

 if (!(wZType & ZType_Array))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%ld> <%s> encountered while processing record data. Type is NOT ARRAY.",
                                 pFieldList[pRank].ZType,
                                 decode_ZType(pFieldList[pRank].ZType));
         return ZS_INVTYPE;
        }
 wZType = wZType &(~ZType_Array) ;  // negate ZType_Array : do not need it anymore

// long wEltOffsetKey=pFieldList.fieldKeyOffset(pRank);
 long wEltOffsetKey= 0;

ZDataBuffer wDBIn;

ssize_t KeyDataSize = (ssize_t)((float)pFieldList[pRank].UniversalSize / (float)pFieldList[pRank].ArrayCount) ; //! compute data size in key format

ZIndexField wField;
    ssize_t wOffset = pFieldList.fieldKeyOffset(pRank) ;
    wDBIn.setData((pInData.Data+wOffset),pFieldList[pRank].UniversalSize);

 pOutData.clear();

 if (wZType&ZType_Char)
        {
        pOutData.setData(wDBIn);
        return ZS_SUCCESS;
        }

 for (long wi=0;wi<pFieldList[pRank].ArrayCount;wi++)
 {
 switch (wZType)
 {
    case ZType_Char:    // char MUST not be converted with leading sign byte : this is a C string.
    case ZType_UChar:
    case ZType_U8 :
             {
             uint8_t wValue = _getValueAAFK<uint8_t>(wDBIn,wi,pRank,pFieldList);
             pOutData.appendData(&wValue,sizeof(wValue));
             wEltOffsetKey += KeyDataSize;
             break;
             }
    case ZType_S8 :
            {
             int8_t wValue = _getValueAAFK<int8_t>(wDBIn,wi,pRank,pFieldList);
             pOutData.appendData(&wValue,sizeof(wValue));
             wEltOffsetKey += KeyDataSize;
             break;
            }

     case ZType_U16 :
              {
             uint16_t wValue = _getValueAAFK<uint16_t>(wDBIn,wi,pRank,pFieldList);
             pOutData.appendData(&wValue,sizeof(wValue));
             wEltOffsetKey += KeyDataSize;
             break;
              }
     case ZType_S16 :
             {
             int16_t wValue = _getValueAAFK<int16_t>(wDBIn,wi,pRank,pFieldList);
             pOutData.appendData(&wValue,sizeof(wValue));
             wEltOffsetKey += KeyDataSize;
             break;
             }
     case ZType_U32 :
              {
             uint32_t wValue = _getValueAAFK<uint32_t>(wDBIn,wi,pRank,pFieldList);
             pOutData.appendData(&wValue,sizeof(wValue));
             wEltOffsetKey += KeyDataSize;
             break;
              }
     case ZType_S32 :
             {
             int32_t wValue = _getValueAAFK<int32_t>(wDBIn,wi,pRank,pFieldList);
             pOutData.appendData(&wValue,sizeof(wValue));
             wEltOffsetKey += KeyDataSize;
             break;
             }
     case ZType_U64 :
              {
             uint64_t wValue = _getValueAAFK<uint64_t>(wDBIn,wi,pRank,pFieldList);
             pOutData.appendData(&wValue,sizeof(wValue));
             wEltOffsetKey += KeyDataSize;
             break;
              }
     case ZType_S64 :
             {
             int64_t wValue = _getValueAAFK<int64_t>(wDBIn,wi,pRank,pFieldList);
             pOutData.appendData(&wValue,sizeof(wValue));
             wEltOffsetKey += KeyDataSize;
             break;
             }
    case ZType_Float :
                 {
                 float wValue = _getValueAAFK<float>(wDBIn,wi,pRank,pFieldList);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffsetKey += KeyDataSize;
                 break;
                 }
     case ZType_Double :
                  {
                 double wValue = _getValueAAFK<double>(wDBIn,wi,pRank,pFieldList);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffsetKey += KeyDataSize;
                 break;
                  }
     case ZType_LDouble :
                  {
                 long double wValue = _getValueAAFK<long double>(wDBIn,wi,pRank,pFieldList);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffsetKey += KeyDataSize;
                 break;
                  }

        default:
                 {
                     ZException.setMessage(_GET_FUNCTION_NAME_,
                                             ZS_INVTYPE,
                                             Severity_Fatal,
                                             "Invalid ZType <%ld> <%s> encountered while processing record data",
                                             pFieldList[pRank].ZType,
                                             decode_ZType(pFieldList[pRank].ZType));
                     return ZS_INVTYPE;
                 }

    }//switch
    }// for
 return ZS_SUCCESS;
}//_getArrayFromKey



/**
 * @brief _printAtomicValueFromKey formats a ZDataBuffer with an atomic key field value converted to a human readable value from a key content
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] pOutValue field value in human readable format
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pFieldList Index dictionary as a CZKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return          A reference to ZDataBuffer containing the packed data field.
 */

ZDataBuffer& _printAtomicValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &pOutValue,const long pRank, ZKeyDictionary & pFieldList)
{
descString AVFKValue;
 ZTypeBase wZType = pFieldList[pRank].ZType;

 if (!(wZType & ZType_Atomic))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%ld> <%s> encountered while processing key data. Type is NOT ATOMIC.",
                                 pFieldList[pRank].ZType,
                                 decode_ZType(pFieldList[pRank].ZType));
         ZException.exit_abort();
        }
 wZType = wZType &(~ZType_Atomic) ;  //! negate ZType_Atomic : do not need it anymore

 switch (wZType)
 {
case ZType_U8 :
         {
         AVFKValue.sprintf("<%uc> ", _getValueAFK<uint8_t>(pKeyData,pRank,pFieldList));
         break;
         }
case ZType_S8 :
        {
         AVFKValue.sprintf("<%c> ",_getValueAFK<int8_t>(pKeyData,pRank,pFieldList));
         break;
        }

 case ZType_U16 :
          {
          AVFKValue.sprintf("<%ud> ",_getValueAFK<uint16_t>(pKeyData,pRank,pFieldList));
          break;
          }
 case ZType_S16 :
         {
         AVFKValue.sprintf("<%d> ",_getValueAFK<int16_t>(pKeyData,pRank,pFieldList));
         break;
         }
 case ZType_U32 :
          {
         AVFKValue.sprintf("<%uld> ",_getValueAFK<uint32_t>(pKeyData,pRank,pFieldList));
         break;
          }
 case ZType_S32 :
         {
         AVFKValue.sprintf("<%ld> ",_getValueAFK<int32_t>(pKeyData,pRank,pFieldList));
         break;
         }
 case ZType_U64 :
          {
         AVFKValue.sprintf("<%ulld> ",_getValueAFK<uint64_t>(pKeyData,pRank,pFieldList));
         break;
          }
 case ZType_S64 :
         {
         AVFKValue.sprintf("<%lld> ",_getValueAFK<int64_t>(pKeyData,pRank,pFieldList));
         break;
         }
case ZType_Float :
             {
             AVFKValue.sprintf("<%f> ",_getValueAFK<float>(pKeyData,pRank,pFieldList));
             break;
             }
 case ZType_Double :
              {
             AVFKValue.sprintf("<%g> ",_getValueAFK<double>(pKeyData,pRank,pFieldList));
             break;
              }
 case ZType_LDouble :
              {
             AVFKValue.sprintf("<%g> ",_getValueAFK<long double>(pKeyData,pRank,pFieldList));
             break;
              }

    default:
             {
             AVFKValue = "#### " ;
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                     ZS_INVTYPE,
                                     Severity_Fatal,
                                     "Invalid ZType <%ld> <%s> encountered while processing key data",
                                     pFieldList[pRank].ZType,
                                     decode_ZType(pFieldList[pRank].ZType));
             break;
             }

 }//switch
    pOutValue.setData(AVFKValue.content,AVFKValue.size()+1);
    pOutValue.DataChar[AVFKValue.size()]='\0';
    return pOutValue;
}//_printAtomicValueFromKey

/**
 * @brief _printArrayValueFromKey Obtains a user readable content of a field with ZType_Array
 *  Field values occurences are concanetated in pOutvalue, enclosed with < and > signs and separated by space
 *
 *
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] pOutValue field value in human readable format
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pFieldList Index dictionary as a CZKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return a reference to pOutValue ZDataBuffer
 */
ZDataBuffer& _printArrayValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &pOutValue,const long pRank, ZKeyDictionary & pFieldList)
{
descString wEltValue;
 ZTypeBase wZType = pFieldList[pRank].ZType;

 if (!(wZType & ZType_Array))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%ld> <%s> encountered while processing key data. Type is NOT AN ARRAY.",
                                 pFieldList[pRank].ZType,
                                 decode_ZType(pFieldList[pRank].ZType));
         ZException.exit_abort();
        }


 wZType = wZType &(~ZType_Array) ;  //! negate ZType_Atomic : do not need it anymore

 pOutValue.clear();
for (long wi=0;wi<pFieldList[pRank].ArrayCount;wi++)
{
 switch (wZType)
    {
case ZType_Char:

case ZType_UChar:
case ZType_U8 :
         {
         wEltValue.sprintf("<%uc> ", _getValueAAFK<uint8_t>(pKeyData,wi,pRank,pFieldList));
         break;
         }
case ZType_S8 :
        {
         wEltValue.sprintf("<%c> ",_getValueAAFK<int8_t>(pKeyData,wi,pRank,pFieldList));
         break;
        }

 case ZType_U16 :
          {
          wEltValue.sprintf("<%ud> ",_getValueAAFK<uint16_t>(pKeyData,wi,pRank,pFieldList));
          break;
          }
 case ZType_S16 :
         {
         wEltValue.sprintf("<%d> ",_getValueAAFK<int16_t>(pKeyData,wi,pRank,pFieldList));
         break;
         }
 case ZType_U32 :
          {
         wEltValue.sprintf("<%uld> ",_getValueAAFK<uint32_t>(pKeyData,wi,pRank,pFieldList));
         break;
          }
 case ZType_S32 :
         {
         wEltValue.sprintf("<%ld> ",_getValueAAFK<int32_t>(pKeyData,wi,pRank,pFieldList));
         break;
         }
 case ZType_U64 :
          {
         wEltValue.sprintf("%ulld ",_getValueAAFK<uint64_t>(pKeyData,wi,pRank,pFieldList));
         break;
          }
 case ZType_S64 :
         {
         wEltValue.sprintf("<%lld> ",_getValueAAFK<int64_t>(pKeyData,wi,pRank,pFieldList));
         break;
         }
case ZType_Float :
             {
             wEltValue.sprintf("<%f> ",_getValueAAFK<float>(pKeyData,wi,pRank,pFieldList));
             break;
             }
 case ZType_Double :
              {
             wEltValue.sprintf("<%g> ",_getValueAAFK<double>(pKeyData,wi,pRank,pFieldList));
             break;
              }
 case ZType_LDouble :
              {
             wEltValue.sprintf("<%g> ",_getValueAAFK<long double>(pKeyData,wi,pRank,pFieldList));
             break;
              }

    default:
             {
                 ZException.setMessage(_GET_FUNCTION_NAME_,
                                         ZS_INVTYPE,
                                         Severity_Fatal,
                                         "Invalid ZType <%ld> <%s> encountered while processing key data",
                                         pFieldList[pRank].ZType,
                                         decode_ZType(pFieldList[pRank].ZType));
                 ZException.exit_abort();
             }

        }//switch

    pOutValue.appendData(wEltValue.content,wEltValue.size());
    }//for

return pOutValue;
}//_printArrayValueFromKey

static inline
ZDataBuffer&
_convertAtomicEdian(ZDataBuffer& pData,auto &pValue, ZIndexField & pField)
 {
decltype(pValue) wValue = pValue;

    pData.allocate(pField.NaturalSize+1);
    pData.clearData();

    if (wValue < 0 )
                wValue = - wValue  ;
        else
                pData[0] = 1 ;  // set positive sign (default is 0 : negative)

    int wj=1;
    int wi=sizeof(wValue)-1;

    unsigned char* wPtr = (unsigned char*)&wValue;
    while(wi >= 0)
            {
            pData.Data[wj] =wPtr[wi];
            wj++;
            wi--;
            }
    return pData;
}//_convertAtomic_Edian

ZDataBuffer&
_convertAtomicNOEndian(ZDataBuffer& pData,auto pValue, ZIndexField & pField)
 {
decltype(pValue) wValue = pValue;

    pData.allocate(sizeof(pValue)+1);
    pData.clearData();

    if (wValue < 0 )
                wValue = - wValue  ;
        else
                pData[0] = 1 ;  // set positive sign (default is 0 : negative)

    int wj=1;
    int wi=sizeof(wValue)-1;

    unsigned char* wPtr = (unsigned char*)&wValue;
    memmove (pData.Data+1,&pValue,sizeof(pValue));

    return pData;
}//_convertAtomic_NOEdian


template <typename _Tp>
ZDataBuffer &
_convert(typename std::enable_if<std::is_integral<_Tp>::value, _Tp>::type &pValue,ZIndexField pField,ZDataBuffer &pData) {
  // an implementation for integral types (int, char, unsigned, etc.)
    if (is_little_endian()) // if system uses little endian integral internal representation
            {
            return _convertAtomicEdian(pData,pValue,pField);
            }
    return _convertAtomicNOEndian(pData,pValue,pField);  //! or not
}

template <typename _Tp> //! no conversion for pointer
ZDataBuffer &
_convert(typename std::enable_if<std::is_pointer<_Tp>::value, _Tp>::type &pValue) {

}
template <typename _Tp>
ZDataBuffer &
_convert(typename std::enable_if<std::is_class<_Tp>::value, _Tp>::type &pValue) {

}

//---------------- building a key with fields----------------------
//












//-----------Functions-----------------------------------------------

#ifdef __COMMENT__
/**
* @brief is_little_endian  defines if system is using little Endian atomic data storage (reverse byte order)
* @return
*/
bool
is_little_endian(void)
{
  union {
       uint32_t i;
       int8_t c[4];
   } e = { 0x00000001 };

   return (e.c[0]==1);
} //is_little_endian
#endif // __COMMENT__
//----------Get sizes per Atomic ZType_Type-------------------------

/**
 * @brief _getAtomicZType_Sizes returns Natural and Internal sizes for an Atomic ZType_type given by pType
 * @param[in] pType
 * @param[out] pNaturalSize
 * @param[out] pKeyFieldSize
 * @return
 */
ZStatus
getAtomicZType_Sizes(ZTypeBase pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize) // not a template
{

    switch (pType)
    {
    case ZType_UChar :
    case ZType_Char : // array of char is a special case because no sign byte is added
    case ZType_U8 :
        {
        pNaturalSize=sizeof(uint8_t);
        pKeyFieldSize = pNaturalSize;
        return ZS_SUCCESS;
        }
    case ZType_S8 :
        {
        pNaturalSize=sizeof(uint8_t);
        pKeyFieldSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_S16 :
        {
        pNaturalSize=sizeof(int16_t);
        pKeyFieldSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_U16 :
        {
        pNaturalSize=sizeof(uint16_t);
        pKeyFieldSize = pNaturalSize;
        return ZS_SUCCESS;
        }
    case ZType_S32 :
        {
        pNaturalSize=sizeof(int32_t);
        pKeyFieldSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_U32 :
        {
        pNaturalSize=sizeof(uint32_t);
        pKeyFieldSize = pNaturalSize;
        return ZS_SUCCESS;
        }
    case ZType_S64 :
        {
        pNaturalSize=sizeof(int64_t);
        pKeyFieldSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_U64 :
        {
        pNaturalSize=sizeof(uint64_t);
        pKeyFieldSize = pNaturalSize;
        return ZS_SUCCESS;
        }
    case ZType_Float :
        {
        pNaturalSize=sizeof(float);
        pKeyFieldSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_Double :
        {
        pNaturalSize=sizeof(double);
        pKeyFieldSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_LDouble :
        {
        pNaturalSize=sizeof(long double);
        pKeyFieldSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    default:
        {
        pNaturalSize = -1;
        pKeyFieldSize = -1;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Error,
                                " Invalid type <%ld> given type analysis is %s",
                                decode_ZType(pType));
        return ZS_INVTYPE;
        }
    }// case
return ZS_SUCCESS;
}//_getAtomicZType_Sizes


/**
 * @brief getAtomicZType template to analyze a type hashcode and deduce ZType_Type and natural and internal sizes for an atomic data
 * This function is NOT a template and cannot be called by template routines
 *
 * @param[in] pTypeHashCode  type hashcode obtained using typeid(VAR).hash_code(). VAR must be atomic.
 * @param[out] pType         deduced ZType_type
 * @param[out] pNaturalSize  Resulting natural size of the variable
 * @param[out] pKeyFieldSize Resulting internal size of the variable
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
template <class _Tp>
ZStatus
//getAtomicZType(const size_t pTypeHashCode,long& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize)
getAtomicZType(_Tp pValue, ZTypeBase& pType, ssize_t& pNaturalSize, ssize_t& pKeyFieldSize)
{
const size_t pTypeHashCode = typeid(pValue).hash_code();
    pType=ZType_Nothing;
    while (true)
    {
    if (pTypeHashCode==UCharType)
                    {
                    pType |= ZType_UChar ;
                    pNaturalSize=sizeof(unsigned char);
                    break;
                    }
    if (pTypeHashCode==CharType)
                    {
                    pType |= ZType_Char ;
                    pNaturalSize=sizeof(char);
                    pKeyFieldSize = pNaturalSize;  // array of char is a special case because no sign byte is added
                    break;
                    }
    if (pTypeHashCode==U8Type)
                    {
                    pType |= ZType_U8 ;
                    pNaturalSize=sizeof(uint8_t);
                    break;
                    }
    if (pTypeHashCode==S8Type)
                    {
                    pType |= ZType_S8 ;
                    pNaturalSize=sizeof(int8_t);
                    break;
                    }
    if (pTypeHashCode==S16Type)
                    {
                    pType |= ZType_S16 ;
                    pNaturalSize=sizeof(int16_t);
                    break;
                    }
    if (pTypeHashCode==U16Type)
                    {
                    pType |= ZType_U16 ;
                    pNaturalSize=sizeof(uint16_t);
                    break;
                    }
    if (pTypeHashCode==S32Type)
                    {
                    pType |= ZType_S32 ;
                    pNaturalSize=sizeof(int32_t);
                    break;
                    }
    if (pTypeHashCode==U32Type)
                    {
                    pType |= ZType_U32 ;
                    pNaturalSize=sizeof(uint32_t);
                    break;
                    }
    if (pTypeHashCode==S64Type)
                    {
                    pType |= ZType_S64 ;
                    pNaturalSize=sizeof(int64_t);
                    break;
                    }
    if (pTypeHashCode==U64Type)
                    {
                    pType |= ZType_U64 ;
                    pNaturalSize=sizeof(uint64_t);
                    break;
                    }

//-------Floating point----------------------------
//
    if (pTypeHashCode==FloatType)
                    {
                    pType |= ZType_Float ;
                    pNaturalSize=sizeof(float);
                    break;
                    }
    if (pTypeHashCode==DoubleType)
                    {
                    pType |= ZType_Double ;
                    pNaturalSize=sizeof(double);
                    break;
                    }
    if (pTypeHashCode==LDoubleType)
                    {
                    pType |= ZType_LDouble ;
                    pNaturalSize=sizeof(long double);
                    break;
                    }


    pType = ZType_Unknown;
    pNaturalSize = -1;
    pKeyFieldSize = -1;

    return (ZS_INVTYPE);

    }// while
    pKeyFieldSize = pNaturalSize;
    if (pType & ZType_Signed)
                pKeyFieldSize = pKeyFieldSize+ 1; //! take care of sign byte

    return(ZS_SUCCESS);
}// getAtomicZType generic - not template


/** @} */ //  End group ZIndexGroup



//--------------------ZKey routines---------------------------------------------------

//-------ZKey routines-------------------------------------------
/** @addtogroup ZKeyGroup
 * @{
*/

ZKey::ZKey(ZIndexControlBlock *pZICB,const long pKeyRank)
{
    ZICB=pZICB;
    IndexNumber=pKeyRank;
    if (FieldPresence.allocateCurrentElements(pZICB->ZKDic->size())) // alloc necessary elements and zero it
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Fatal,
                                    " Key dictionary for index <%s> contains no elements",
                                    ZICB->Name.toString()
                                    );
            ZException.exit_abort();
            }

    ZICB->ZKDic->_reComputeSize();
//    ZICB->KeyType=ZIF_Nothing;
    allocate(ZICB->ZKDic->UniversalSize);
    clearData();
    return;
}// ZKey CTor


/**
 * @brief ZKey::clearFieldSpace sets the key space reserved to field given by pFieldRank to binary zero
 * Extends the key buffer whenever necessary.
 * @param[in] pFieldRank field rank in key dictionary
 */
void
ZKey::clearFieldSpace (const long pFieldRank)
{
    long wOffset=ZICB->ZKDic->fieldKeyOffset(pFieldRank);
    if ((ZICB->ZKDic->Tab[pFieldRank].UniversalSize+wOffset)>Size)
                            allocate(ZICB->ZKDic->Tab[pFieldRank].UniversalSize+wOffset);
    memset (Data+wOffset,0,ZICB->ZKDic->Tab[pFieldRank].UniversalSize);
    FieldPresence[pFieldRank]=false;                                    // Field is absent
}

/**
 * @brief setFieldRawValue sets the key section corresponding to mentioned field with raw data given by a pValue and pSize
 *
 *   Data must have been converted to Key internal format whenever necessary
 *   Field zone is padded with binary zero in data size is less than field internal format size
 *   Data is truncated to field internal format size if given data size exceeds
 *
 * @param[in] pValue a pointer to raw data value converted in Key format if necessary
 * @param[in] pSize  size of the data to set the field with
 * @param[in] pFieldRank field rank in key dictionary
 * @return a reference to current ZKey object
 *
 */
ZKey&
ZKey::setFieldRawValue (const void* pValue,const ssize_t pSize,const long pFieldRank)
{
long wOffset=ZICB->ZKDic->fieldKeyOffset(pFieldRank);
ssize_t wSize = pSize;
    if (wSize> ZICB->ZKDic->Tab[pFieldRank].UniversalSize)
                                wSize = ZICB->ZKDic->Tab[pFieldRank].UniversalSize;
     if ((ZICB->ZKDic->Tab[pFieldRank].UniversalSize+wOffset)>Size)
                 allocate(ZICB->ZKDic->Tab[pFieldRank].UniversalSize+wOffset);

    memset (Data+wOffset,0,ZICB->ZKDic->Tab[pFieldRank].UniversalSize);
    memmove(Data+wOffset,pValue,pSize);
return *this;
}//setFieldRawValue
/**
 * @brief setFieldRawValue sets the key section corresponding to mentioned field with raw data given by a ZDataBuffer pValue
 *
 *   Data must have been converted to Key internal format whenever necessary
 *   Field zone is padded with binary zero in data size is less than field internal format size
 *   Data is truncated to field internal format size if given data size exceeds
 *
 * @param[in] pValue a ZDataBuffer containing raw data value converted in Key format if necessary
 * @param[in] pFieldRank field rank in key dictionary
 * @return a reference to current ZKey object
 *
 */
ZKey&
ZKey::setFieldRawValue (ZDataBuffer& pValue,const long pFieldRank)
{
    return setFieldRawValue(pValue.Data,pValue.Size,pFieldRank);
}

/**
 * @brief ZKey::setKeyPartial
 * @param pFieldName
 * @param pLength
 * @return
 */
ZStatus
ZKey::setKeyPartial (const ssize_t pFieldRank ,const ssize_t pLength)
{
    if ((pFieldRank<0)||(pFieldRank>ZICB->ZKDic->size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Error,
                                    " Invalid given field rank <%ld> while it must be [0,<%ld>]",
                                    pFieldRank,
                                    ZICB->ZKDic->lastIdx());
            return ZS_OUTBOUND;
            }
// compute size up until current field
    PartialLength=0;
    for (long wi=0;wi<pFieldRank;wi++)
                        PartialLength += ZICB->ZKDic->Tab[wi].UniversalSize;
    PartialLength += pLength;
    FPartialKey = true;

    _Base::Size = PartialLength;
    return ZS_SUCCESS;
}

/**
 * @brief ZKey::setKeyPartial
 * @param pFieldName
 * @param pLength
 * @return
 */
ZStatus
ZKey::setKeyPartial (const char* pFieldName,ssize_t pLength)
{
    long wFieldRank=ZICB->ZKDic->zsearchFieldByName(pFieldName);
    if (wFieldRank<0)
            {
            ZException.addToLast(" While setting partial key length");
            return ZException.getLastStatus();
            }
    return setKeyPartial(wFieldRank,pLength);
}



/**
  * @brief ZIndexFile::zprintKeyFieldsValues     prints the key fields values in a human readable format from a ZIndexFile
  *
  * Prints the actual key content to pOutput after having converted back all composing field using ZIndex Dictionary.
  *
  * In case of ZType_Class field, then the content of data is dumped using ZDataBuffer::Dump().
  *
  * @param[in] pHeader  if set to true then key fields description is printed. False means only values are printed.
  * @param[in] pKeyDump if set to true then index key record content is dumped after the list of its fields values. False means only fields values are printed.
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
  */
 void
 ZKey::zprintKeyFieldsValues (bool pHeader,bool pKeyDump,FILE*pOutput)
 {

ZDataBuffer wPrintableField;
ZDataBuffer wKeyContent;

    wKeyContent = (ZDataBuffer&)*this;

// header : index name then fields dictionary definition
     if (pHeader)
     {
     fprintf (pOutput,
              "_______________________________________________________________________________________\n"
              " Index name %s\n",
              ZICB->Name.content);
     for (long wi=0;wi<ZICB->ZKDic->size();wi++)
     {
     fprintf (pOutput,
              " Field order %ld  name <%s> Data type <%s> \n",
              wi,
//              ZICB->ZKDic->Tab[wi].Name.content,
              ZICB->ZKDic->MetaDic->Tab[ZICB->ZKDic->Tab[wi].MDicRank].Name.toString(),
              decode_ZType( ZICB->ZKDic->MetaDic->Tab[ZICB->ZKDic->Tab[wi].MDicRank].ZType));
     }// for
     fprintf (pOutput,
              "_______________________________________________________________________________________\n");
     }// if pHeader

// then fields values

     wKeyContent = (ZDataBuffer&)*this ;

     for (long wi=0;wi<ZICB->ZKDic->size();wi++)
     {
     fprintf (pOutput,
              "      field order <%ld>  <%s> value ",
              wi,
              ZICB->ZKDic->Tab[wi].Name.content);
     if (!FieldPresence[wi])
            {
            fprintf(pOutput,
                    "**No value**\n");
            continue;
            }
     while (true)
     {
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Class)  // if class : simple dump
                {
             ssize_t wKeyOffset = ZICB->ZKDic->fieldKeyOffset(wi);

             wPrintableField.setData(wKeyContent.DataChar +wKeyOffset, ZICB->ZKDic->Tab[wi].UniversalSize);
//             wPrintableField.setData(wBlock.Content.DataChar +wKeyOffset, ZICB->ZKDic->Tab[wi].KeyFieldSize);
//         _printAtomicValueFromKey(wBlock.Content,wPrintableField,wi,ZICB->ZKDic);
                fprintf (pOutput,
                         "\n");
                wPrintableField.Dump(16,pOutput);
                break;
                }
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Atomic)
     {
         _printAtomicValueFromKey(wKeyContent,wPrintableField,wi,*ZICB->ZKDic);
         fprintf (pOutput,
                  "<%s> ",
                  wPrintableField.DataChar);
         break;
     }
     if (ZICB->ZKDic->Tab[wi].ZType == ZType_ArrayChar)
     {
         ssize_t wKeyOffset = ZICB->ZKDic->fieldKeyOffset(wi);

         wPrintableField.setData(wKeyContent.DataChar +wKeyOffset, ZICB->ZKDic->Tab[wi].UniversalSize);
//         _printAtomicValueFromKey(wBlock.Content,wPrintableField,wi,ZICB->ZKDic);
         fprintf (pOutput,
                  "<%s> ",
                  wPrintableField.DataChar);
         break;
     }
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Array)
     {
         _printArrayValueFromKey(wKeyContent,wPrintableField,wi,*ZICB->ZKDic);
         fprintf (pOutput,
                  "%s ",
                  wPrintableField.DataChar);
         break;
     }
     fprintf (pOutput,
              "**unknown type*** ");
     break;
     }// while true

     fprintf (pOutput,
              "\n");
     }// for

     if (pKeyDump)
                 Dump();

       fprintf (pOutput,
                "_______________________________________________________________________________________\n");
     return ;
 }//zprintKeyFieldsValues




/** @cond Development */

 // of no use for the moment : should be suppressed
/**
 * @brief testKeyTypes test compatibility of input type (pTypeIn) vs key field type (pKeyType)
 * @param[in] pTypeIn
 * @param[in] pKeyType
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
testKeyTypes (const ZTypeBase pTypeIn,const long pKeyType)
{
ZTypeBase wTypeIn = pTypeIn ;
ZTypeBase wKeyType = pKeyType ;

    if (pTypeIn & ZType_Pointer)  // if pointer : test compatibility of atomic data underneeth
                    {
                   wTypeIn = wTypeIn &(~ZType_Pointer) ;
                   if (pKeyType & ZType_Array)
                            {
                            wKeyType = pKeyType &(~ZType_Array);
                            }
                   if (wTypeIn!=wKeyType)
                   {
                   ZException.setMessage(_GET_FUNCTION_NAME_,
                                           ZS_INVTYPE,
                                           Severity_Warning,
                                           " Pointer vs Array : Invalid Atomic type : expecting <%s> - having <%s> ",
                                           decode_ZType(wKeyType),
                                           decode_ZType(wTypeIn));
                   ZException.printUserMessage(stderr);
                   }
                   return ZS_SUCCESS;
                    }// if pointer
    if (pTypeIn & ZType_StdString)
            {
            return ZS_SUCCESS;
            }


    return ZS_SUCCESS;
} // testKeyTypes

/** @endcond */



//-----------ZKey From record---------------------------------------------------
/**
 * @brief ZKey::KeyValueExtraction Extracts the raw key value from a ZMF record (pRecord) using Index Control Block definition (ZICB) fields definition list (ZICB::Index)
 * to feed the ZKey content :  returns the concaneted fields key value in pKey ZDataBuffer (base)
 *
 * @note: As we are processing variable length records, if a defined key field points outside the record length,
 *                      then its returning key value is set to binary zero on the corresponding length of the field within returned Key value.
 *
 * @see _keyValueExtraction()
 *
 * @param[in] pRecord the record to extract the key value from
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
ZKey::keyValueExtraction( ZDataBuffer &pRecord)
{
    return _keyValueExtraction(*ZICB->ZKDic,pRecord,*this);
}//KeyValueExtraction



/** @} */ // ZKeyGroup

descString ZStringBuffer;

/**
 * @brief decode_ZType decode the ZType_type value given as a long into a human readable string
* @param[in] pType  ZType_type mask to decode
 * @return a C string
*/
const char *decode_ZType (long pType)
{
    if (pType==ZType_Nothing)
                return "ZType_Nothing" ;

    if (pType & ZType_Class)
    {
            return "ZType_Class";
    }
    if (pType & ZType_ByteSeq)
    {
            return "ZType_ByteSeq";
    }


    ZStringBuffer.clear();



    while (true)
    {
    if (pType & ZType_Atomic)
            {
            ZStringBuffer="ZType_Atomic " ;
            break;
            }
    if (pType & ZType_Pointer)
            {
            ZStringBuffer="ZType_Pointer " ;
            break;
            }
    if (pType & ZType_Array)
            {
            ZStringBuffer="ZType_Array " ;
            break;
            }
/*    if (pType & ZType_Enum)
            {
            ZStringBuffer = "ZType_Enum";
            break;
            }*/
    if (pType & ZType_StdString) // we may have a pointer to a std::string
    {
            ZStringBuffer="ZType_StdString";
            break;
    }
    ZStringBuffer="ZType_StructUnknown ";
    break;
    }
    if (pType & ZType_Signed)
            ZStringBuffer += "| ZType_Signed ";
    if (pType & ZType_Endian)
            ZStringBuffer += "| ZType_Endian ";

    while (true) // carefull to put first signed type otherwise detect unsigned while it is signed (same mask with sign byte )
    {
    if ((pType & ZType_Char)==ZType_Char)
            {
            ZStringBuffer+="| ZType_Char" ;
            break;
            }
    if ((pType & ZType_UChar)== ZType_UChar)
        {
        ZStringBuffer+="| ZType_UChar" ;
        break;
        }


    if ((pType & ZType_S8)==ZType_S8)
            {
            ZStringBuffer+="| ZType_S8" ;
            break;
            }
    if ((pType & ZType_U8)==ZType_U8)
            {
            ZStringBuffer+="| ZType_U8" ;
            break;
            }

    if ((pType & ZType_S16)==ZType_S16)
            {
            ZStringBuffer+="| ZType_S16" ;
            break;
            }
    if ((pType & ZType_U16)==ZType_U16)
            {
            ZStringBuffer+="| ZType_U16" ;
            break;
            }

    if ((pType & ZType_S32)==ZType_S32)
            {
            ZStringBuffer+="| ZType_S32" ;
            break;
            }
    if ((pType & ZType_U32)==ZType_U32)
            {
            ZStringBuffer+="| ZType_U32" ;
            break;
            }

    if ((pType & ZType_S64)==ZType_S64)
            {
            ZStringBuffer+="| ZType_S64" ;
            break;
            }
    if ((pType & ZType_U64)==ZType_U64)
            {
            ZStringBuffer+="| ZType_U64" ;
            break;
            }

    if ((pType & ZType_Float)==ZType_Float)
            {
            ZStringBuffer+="| ZType_Float" ;
            break;
            }
    if ((pType & ZType_Double)==ZType_Double)
            {
            ZStringBuffer+="| ZType_Double" ;
            break;
            }
    if ((pType & ZType_LDouble)==ZType_LDouble)
            {
            ZStringBuffer+="| ZType_LDouble" ;
            break;
            }

    if ((pType & ZType_Class)==ZType_Class)
            {
            ZStringBuffer+="| ZType_Compound" ;
            break;
            }
    ZStringBuffer="| ZType_DataUnknown";
    break;
    }
    return ZStringBuffer.content ;
} // decode_ZType

ZStatus
encodeZTypeFromString (long &pZType ,ZDataBuffer &pString)
{

    pZType=ZType_Nothing;

    if (pString.bsearch((void *)"ZType_Atomic",strlen("ZType_Atomic"))>-1)
                    pZType=ZType_Atomic;

    if (pString.bsearch((void *)"ZType_Array",strlen("ZType_Array"))>-1)
                    pZType=ZType_Array;

    if (pString.bsearch((void *)"ZType_Pointer",strlen("ZType_Pointer"))>-1)
                    pZType=ZType_Pointer;

/*    if (pString.bsearch((void *)"ZType_Enum",strlen("ZType_Enum"))>-1)
                    pZType=ZType_Enum;
*/
    if (pString.bsearch((void *)"ZType_ByteSeq",strlen("ZType_ByteSeq"))>-1)
                    pZType=ZType_ByteSeq;

    if (pString.bsearch((void *)"ZType_Class",strlen("ZType_Class"))>-1)
                    {
                    pZType=ZType_Class;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_StdString",strlen("ZType_StdString"))>-1)
                    {
                    pZType=ZType_StdString;
                    return ZS_SUCCESS;
                    }


    if (pString.bsearch((void *)"ZType_UChar",strlen("ZType_UChar"))>-1)
                    {
                    pZType|=ZType_UChar;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_Char",strlen("ZType_Char"))>-1)
                    {
                    pZType|=ZType_Char;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_U8",strlen("ZType_U8"))>-1)
                    {
                    pZType|=ZType_U8;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_S8",strlen("ZType_S8"))>-1)
                    {
                    pZType|=ZType_S8;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_U16",strlen("ZType_U16"))>-1)
                    {
                    pZType|=ZType_U16;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_S16",strlen("ZType_S16"))>-1)
                    {
                    pZType|=ZType_S16;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_U32",strlen("ZType_U32"))>-1)
                    {
                    pZType|=ZType_U32;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_S32",strlen("ZType_S32"))>-1)
                    {
                    pZType|=ZType_S32;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_U64",strlen("ZType_U64"))>-1)
                    {
                    pZType|=ZType_U64;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_S64",strlen("ZType_S64"))>-1)
                    {
                    pZType|=ZType_S64;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_Float",strlen("ZType_Float"))>-1)
                    {
                    pZType|=ZType_Float;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_Double",strlen("ZType_Double"))>-1)
                    {
                    pZType|=ZType_Double;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_LDouble",strlen("ZType_LDouble"))>-1)
                    {
                    pZType|=ZType_LDouble;
                    return ZS_SUCCESS;
                    }

    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVTYPE,
                            Severity_Error,
                            " Given type has an invalid or unknown atomic data type <%s>",
                            pString.DataChar);
    return ZS_INVTYPE;

}//getZTypeFromString


const char *decode_ZCOP (uint16_t pZCOP)
{
    if (pZCOP==ZCOP_Nothing)
                return "ZCOP_Nothing" ;

    ZStringBuffer.clear();

    if (pZCOP & ZCOP_Interval)
            {
            ZStringBuffer="ZCOP_Interval |" ;

            }
    if (pZCOP & ZCOP_Exclude)
            {
            ZStringBuffer += " ZCOP_Exclude |" ;
            }

    if (pZCOP & ZCOP_GetFirst)
            ZStringBuffer += " ZCOP_GetFirst";
    if (pZCOP & ZCOP_GetNext)
            ZStringBuffer += " ZCOP_GetNext";
    if (pZCOP & ZCOP_GetAll)
            ZStringBuffer += " ZCOP_GetAll";
    if (pZCOP & ZCOP_RemoveAll)
            ZStringBuffer += " ZCOP_RemoveAll";
    if (pZCOP & ZCOP_LockAll)
            ZStringBuffer += " ZCOP_LockAll";


    return ZStringBuffer.content ;
} // decode_ZCOP


#endif // ZINDEXTYPE_CPP
