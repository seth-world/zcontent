#ifndef ZSINDEXTYPE_CPP
#define ZSINDEXTYPE_CPP

#include <zindexedfile/zsindextype.h>
#include <zindexedfile/zmetadic.h>
/** @addtogroup ZSIndexFileGroup

@{ */

/**
 * @brief ZKeyDictionary::zgetFieldRank gets a field position (rank) in the key dictionary using its field name
 *
 * @param[in] pFieldName a Cstring that qualifies the name of the field. This name is given while creating the index dictionary.
 * @return the field position (rank) in dictionary. returns -1 if field name has not been found.
 */
long
ZSKeyDictionary::zsearchFieldByName(const utf8_t *pFieldName)
{
    long wMDicRank=0;
        for (long wi=0;wi<size();wi++)
                {
                wMDicRank= Tab[wi].MDicRank;
                if (MetaDic->Tab[wMDicRank].Name==pFieldName)
                                                    return wi;
                }
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
ZSKeyDictionary::zsearchFieldByName(utfdescString &pFieldName)
{
long wMDicRank=0;
    for (long wi=0;wi<size();wi++)
            {
            wMDicRank= Tab[wi].MDicRank;
            if (MetaDic->Tab[wMDicRank].Name==pFieldName.toCChar())
                                                return wi;
            }
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVNAME,
                            Severity_Error,
                            " Field name <%s> does not exist within ZDictionary",
                            pFieldName.toCChar());

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

ZSIndexField_struct wField;
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

ZSIndexField_struct wField;
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
                            &ZSIndexField_struct::_exportConvert);
     pZDBExport.setData(wBuffer,wBufferSize);
     free(wBuffer);
     return pZDBExport;
}
size_t
ZSKeyDictionary::_import(unsigned char* pZDBImport_Ptr)
{
    size_t wDicSize=ZAimport((_Base*)this,
                               pZDBImport_Ptr,
                               &ZSIndexField_struct::_importConvert);

    _reComputeSize();
    return wDicSize;
}



/** @endcond */




ZSIndexControlBlock::ZSIndexControlBlock (ZMetaDic *pMetaDic)
{
    clear(pMetaDic);
    return;
}
ZSIndexControlBlock::~ZSIndexControlBlock (void)
{
/*    if (ZKDic!=nullptr)
                {
                delete ZKDic ;
                ZKDic=nullptr;
                }*/
    if (CheckSum!=nullptr)
                delete CheckSum;
    MetaDic=nullptr; //MetaDic refers to ZSMasterFile::MetaDic
    return;
}
/**
 * @brief ZIndexControlBlock::clear Resets ZIndexControlBlock to initial data - reset Key Dictionnary
 */
void
ZSIndexControlBlock::clear(ZMetaDic*pMetaDic)
{

    ZSICBOwnData::clear();
    MetaDic = pMetaDic;
    if (ZKDic!=nullptr)
                {
                delete ZKDic ;
                ZKDic = nullptr;
                }
    ZKDic = new ZSKeyDictionary(pMetaDic) ;
    return;
}//clear

ZSIndexField_strOut
ZSIndexField_struct::_exportConvert(ZSIndexField_struct& pIn,ZSIndexField_strOut* pOut)
{

    memset(pOut,0,sizeof(ZSIndexField_strOut));
    pOut->MDicRank=reverseByteOrder_Conditional<size_t>(pIn.MDicRank);
    pOut->NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn.NaturalSize);
    pOut->UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn.UniversalSize);
    pOut->ArrayCount=reverseByteOrder_Conditional<uint32_t>(pIn.ArrayCount);
    pOut->ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn.ZType);
    return *pOut;
}
ZSIndexField_struct
ZSIndexField_struct::_importConvert(ZSIndexField_struct& pOut,ZSIndexField_strOut* pIn)
{

    memset(&pOut,0,sizeof(ZSIndexField_struct));
    pOut.MDicRank=reverseByteOrder_Conditional<size_t>(pIn->MDicRank);
    pOut.NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn->NaturalSize);
    pOut.UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn->UniversalSize);
    pOut.ArrayCount=reverseByteOrder_Conditional<uint32_t>(pIn->ArrayCount);
    pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
    return pOut;
}

ZDataBuffer&
ZSIndexField_struct::_export(ZDataBuffer& pZDBExport)
{
    ZSIndexField_struct wIFS;
    memset(&wIFS,0,sizeof(ZSIndexField_struct));
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
ZSIndexField_struct&
ZSIndexField_struct::_import(unsigned char* pZDBImport_Ptr)
{
    ZSIndexField_struct* wIFS=(ZSIndexField_struct*) pZDBImport_Ptr;
    memset(this,0,sizeof(ZSIndexField_struct));
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



//------------- End CZKeyDictionary---------------------------------------
void
ZSICBOwnData_Export::clear(void)
{
    BlockID=ZBID_ICB;
    ZMFVersion =  __ZMF_VERSION__ ;
//    Name.clear();   // index name is not defined in ICB export structure : got from ICB memory structure and exported as Universal string
    BlockID = ZBID_ICB;
    StartSign=cst_ZSTART;
    Duplicates = ZST_NODUPLICATES;
    AutoRebuild = false;
    ICBTotalSize =0;
    ZKDicOffset=0;
//    Name=nullptr;
    return;
}//clear
ZDataBuffer&
ZSICBOwnData::_exportICBOwn(ZDataBuffer& pZDBExport)
{
ZDataBuffer wIndexName;
ZSICBOwnData_Export wICB;

    memset(&wICB,0,sizeof(ZSICBOwnData_Export));

    wIndexName=Name._exportUVF();
    ICBTotalSize = wIndexName.Size+sizeof(ZSICBOwnData_Export);
    ZKDicOffset = ICBTotalSize;
    wICB.clear();

    wICB.BlockID=ZBID_ICB ;   // uint8_t
    wICB.ZMFVersion=reverseByteOrder_Conditional<unsigned long>(__ZMF_VERSION__);
    wICB.ICBTotalSize=reverseByteOrder_Conditional<size_t>(ICBTotalSize);
    wICB.ZKDicOffset=reverseByteOrder_Conditional<size_t>(ZKDicOffset);

    wICB.AutoRebuild=AutoRebuild; // uint8_t
    wICB.Duplicates=Duplicates; // uint8_t
    wICB.KeyType=KeyType; // uint8_t
//    wICB.EndSign=EndSign;
    pZDBExport.setData(&wICB,sizeof(wICB));

    pZDBExport.appendData(wIndexName);
    return pZDBExport;
}// ZSICBOwnData::_exportICBOwn
ZStatus
ZSICBOwnData::_importICBOwn(unsigned char* pZDBImport_Ptr)
{


    ZSICBOwnData_Export* wICBOwn_Import=(ZSICBOwnData_Export*)( pZDBImport_Ptr);

    clear();
    if (wICBOwn_Import->BlockID != ZBID_ICB) // has not to be reversed
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_BADICB,
                                   Severity_Severe,
                                   "Error Index Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
                                   wICBOwn_Import->BlockID);
            return (ZS_BADICB);
            }
    if (wICBOwn_Import->StartSign != cst_ZSTART)    // has not to be reversed
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_BADFILEHEADER,
                                  Severity_Severe,
                                  "Error Index Control Block  : Index header appears to be corrupted - invalid ICB StartBlock");
            return (ZS_BADFILEHEADER);
            }

    if (reverseByteOrder_Conditional<unsigned long>(wICBOwn_Import->ZMFVersion) != __ZMF_VERSION__)// HAS to be reversed
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_BADFILEVERSION,
                                  Severity_Severe,
                                  "Error Index Control Block   : Found version <%ld> while current ZMF version is <%ld>",
                                  wICBOwn_Import->ZMFVersion,
                                  __ZMF_VERSION__);
            return (ZS_BADFILEVERSION);
            }



    ICBTotalSize=reverseByteOrder_Conditional<size_t>(wICBOwn_Import->ICBTotalSize);
    ZKDicOffset=reverseByteOrder_Conditional<size_t>(wICBOwn_Import->ZKDicOffset);

    AutoRebuild=wICBOwn_Import->AutoRebuild;  // uint8_t
    Duplicates=wICBOwn_Import->Duplicates; // uint8_t
    KeyType=wICBOwn_Import->KeyType; // uint8_t

    unsigned char* wPtr=pZDBImport_Ptr+sizeof(ZSICBOwnData_Export); // index Name is stored just after ZSICBOwnData_Export structure
                                                                    // format is Universal format with leading size (uint16_t)

    size_t wSize=Name._importUVF(wPtr);  // Name is stored after ZSICBOwnData_Export as a varying number of byte (uint16_t is leading string size)

//    size_t wZKDicOffset = wSize+sizeof(ZSICBOwnData_Export); // ZKDic starts after varying string Index Name
//    EndSign=wICB->EndSign;

    return  ZS_SUCCESS;
}// ZSICBOwnData::_importICBOwn





//

//--------------- Get data from Record----------------------------------------------

static inline
unsigned char* _reverseByteOrder_Ptr(unsigned char* pValue, size_t pSize)
{
ZDataBuffer wValue;
    wValue.setData( pValue,pSize);

    int wj=0;
    ssize_t wi=pSize>1?pSize-1:0;

    uint8_t * wPtr = (uint8_t*)pValue;
    uint8_t* wPtr1 = (uint8_t*)wValue.Data;
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
                        wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);

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
_Tp _getValueAFK (ZDataBuffer &pKeyData,const long pRank,ZSKeyDictionary & pKFieldList)
{
    _Tp wValue;
    ZDataBuffer wDBV;
    ssize_t wFieldOffset = pKFieldList.fieldKeyOffset(pRank);

    if (wFieldOffset<0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Fatal,
                                    " Field rank is out of key dictionary boundaries while computing field offset given rank <%ld> vs dicionary size <%ld>",
                                    pRank,
                                    pKFieldList.size());
            ZException.exit_abort();
            }

    wDBV.setData(pKeyData.Data+wFieldOffset,pKFieldList[pRank].UniversalSize);

//    wDBV.Dump();
    if (pKFieldList[pRank].ZType & ZType_Signed)
            {
            memmove(&wValue,(wDBV.Data+1),sizeof(_Tp));
            if (pKFieldList[pRank].ZType & ZType_Endian)   // and data type is subject to endian reverse byte conversion
//                if (is_little_endian())             // only if system is little endian
                              wValue=reverseByteOrder_Conditional<_Tp>(wValue);
            if (wDBV.Data[0]==0)  // signed negative value
                    {
//                    _negate(wDBV.Data+1,wDBV.Size-1);
                    _negate((unsigned char*)&wValue,sizeof(wValue));

//                                        _reverseByteOrder_Ptr(wDBV.Data+1,(ssize_t)(wDBV.Size-1));
//                    memmove(&wValue,(wDBV.Data+1),sizeof(_Tp));
                    wValue = -wValue;
//                    return wValue;
                    }
            return wValue;
/*                else // it is a positive value
                {
                if (is_little_endian())             // only if system is little endian
                      if (pKFieldList[pRank].ZType & ZType_Endian)   // and data type is subject to endian reverse byte conversion
                                    _reverseByteOrder_Ptr(wDBV.Data+1,wDBV.Size-1);

                return wValue;
                }*/
            }
 // not signed : means no byte sign

    memmove(&wValue,(pKeyData.Data+wFieldOffset),sizeof(wValue));
/*    if (is_little_endian())             // only if system is little endian
          if (pKFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                         _reverseByteOrder_Ptr(wDBV.Data,sizeof(_Tp));
*/
    if (pKFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                    wValue=reverseByteOrder_Conditional<_Tp>(wValue);
//    wValue = static_cast<_Tp>(*wDBV.Data);
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
_Tp _getValueAAFK (ZDataBuffer &pKeyData,const long pIndex,const long pRank,ZSKeyDictionary & pKFieldList)
{
    _Tp wValue;
    ZDataBuffer wDBV;
//    ssize_t wFieldOffset = pFieldList.fieldKeyOffset(pRank);
    ssize_t wElementSize = pKFieldList[pRank].UniversalSize / pKFieldList[pRank].ArrayCount; // unary element size within array
    ssize_t wFieldOffset = pKFieldList.fieldKeyOffset(pRank) + (wElementSize*pIndex) ;


    if ((wFieldOffset<0)||(pIndex>pKFieldList[pRank].ArrayCount))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Fatal,
                                    " Field rank is out of key dictionary boundaries while computing field offset given rank <%ld> vs dicionary size <%ld>",
                                    pRank,
                                    pKFieldList.size());
            ZException.exit_abort();
            }

//    wDBV.setData(pKeyData.Data+wFieldOffset,pFieldList[pRank].KeySize);
    wDBV.setData(pKeyData.Data+wFieldOffset,wElementSize);

    if (pKFieldList[pRank].ZType & ZType_Signed)
            {
            if (wDBV.Data[0]==0)  // signed negative value
                    {
                    _negate(wDBV.Data+1,wDBV.Size-1);
                    if (is_little_endian())             // only if system is little endian
                          if (pKFieldList[pRank].ZType & ZType_Endian)   // and data type is subject to endian reverse byte conversion
                                        _reverseByteOrder_Ptr(wDBV.Data+1,(ssize_t)(wDBV.Size-1));
                    memmove(&wValue,(wDBV.Data+1),sizeof(_Tp));
                    wValue = -wValue;
                    return wValue;
                    }
                else // it is a positive value
                {
                if (is_little_endian())             // only if system is little endian
                      if (pKFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                                    _reverseByteOrder_Ptr(wDBV.Data+1,wDBV.Size-1);
                memmove(&wValue,(wDBV.Data+1),sizeof(_Tp));
                return wValue;
                }
            }
 // not signed : means no byte sign

    memmove(&wValue,(pKeyData.Data+wFieldOffset),sizeof(wValue));
    if (is_little_endian())             //! only if system is little endian
          if (pKFieldList[pRank].ZType & ZType_Endian)   //! and data type is subject to endian reverse byte conversion
                         _reverseByteOrder_Ptr(wDBV.Data,sizeof(_Tp));

    wValue = static_cast<_Tp>(*wDBV.Data);
    return wValue;
} // _getValueAAFK  for Arrays


//------------------Extract Key field from Key record------------------------------------------------------------



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
ZDataBuffer &_getArrayFromRecord(ZDataBuffer &pInData,ZDataBuffer &pOutData,ZSIndexField_struct & pField)
{
size_t wOffset=0;
 ZTypeBase wZType = pField.ZType;

 if (!(wZType & ZType_Array))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%ld> <%s> encountered while processing record data. Type is NOT ARRAY.",
                                 pField.ZType,
                                 decode_ZType(pField.ZType));
         ZException.exit_abort();
        }
 wZType = wZType &(~ZType_StructureMask) ;  // negate ZType_Array : do not need it anymore


 long wEltOffsetIn=wOffset;
ZDataBuffer wDBElt;
//ZDataBuffer wDBIn;
ZSIndexField_struct wField;

//    wDBIn.setData(pInData.Data+pField.Offset,pField.NaturalSize);

 if (!(wZType&ZType_Signed)&&(!is_little_endian()||!(wZType&ZType_Endian)))
                                        {
                                    pOutData.setData(pInData.Data+wOffset,pField.NaturalSize);
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
  - long        ArraySize;      < in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.

 * @param[in] pInData   ZDataBuffer containing the user record to extract field from.
 * @param[out] pOutData  ZDataBuffer containing as a return the packed/extracted field value converted as ready to be used as a key field
 * @param[in] pField    ZIndex Key dictionary rank (ZIndexField_struct) describing the data field to extract.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus _getAtomicFromRecord(ZDataBuffer &pInData,ZDataBuffer &pOutData,ZSIndexField_struct & pField)
{
size_t wOffset=0;
 ZTypeBase wZType = pField.ZType;

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
ZStatus _getFieldValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZSKeyDictionary & pFieldList)
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

ZStatus _getAtomicValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZSKeyDictionary & pFieldList)
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
ZStatus _getClassValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZSKeyDictionary & pFieldList)
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
ZStatus _getArrayValueFromKey(ZDataBuffer &pInData,ZDataBuffer &pOutData,const long pRank,ZSKeyDictionary & pFieldList)
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

ZSIndexField_struct wField;
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

ZDataBuffer& _printAtomicValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &pOutValue,const long pRank, ZSKeyDictionary & pFieldList)
{
utfdescString AVFKValue;
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
 wZType = wZType &(~ZType_Atomic) ;  // negate ZType_Atomic : do not need it anymore

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
             AVFKValue = (const utf8_t*)"#### " ;
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

#include <wchar.h>
/**
 * @brief _printArrayValueFromKey Obtains a user readable content of a field with ZType_Array
 *  Field values occurences are concanetated in pOutvalue, enclosed with < and > signs and separated by space
 *
 *
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] pOutValue field value in human readable format
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pKFieldList Index dictionary as a ZSKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return a reference to pOutValue ZDataBuffer
 */
ZDataBuffer& _printArrayValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &pOutValue,const long pRank, ZSKeyDictionary & pKFieldList)
{
utfdescString wEltValue;
 ZTypeBase wZType = pKFieldList[pRank].ZType;

 if (!(wZType & ZType_Array))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%ld> <%s> encountered while processing key data. Type is NOT AN ARRAY.",
                                 pKFieldList[pRank].ZType,
                                 decode_ZType(pKFieldList[pRank].ZType));
         ZException.exit_abort();
        }


 wZType = wZType &(~ZType_Array) ;  // negate ZType_Atomic : do not need it anymore

 pOutValue.clear();

 // treat C strings and wide C strings

 if (wZType==ZType_Char)
  {
      size_t wFieldArrayCount = pKFieldList[pRank].ArrayCount ;
      size_t wFieldOffset = pKFieldList.fieldKeyOffset(pRank)  ;
      snprintf(pOutValue.DataChar,wFieldArrayCount, "<%s>",(pKeyData.DataChar+wFieldOffset));
      return pOutValue;
  }
 if (wZType==ZType_WChar)
   {
       size_t wFieldArrayCount = pKFieldList[pRank].ArrayCount ;
       size_t wFieldOffset = pKFieldList.fieldKeyOffset(pRank)  ;
       swprintf(pOutValue.WDataChar,wFieldArrayCount,L"<%s>", (pKeyData.DataChar+wFieldOffset));
       return pOutValue;
   }

 // then other types

for (long wi=0;wi<pKFieldList[pRank].ArrayCount;wi++)
{
 switch (wZType)
    {
case ZType_UChar:
case ZType_U8 :
         {
         wEltValue.sprintf("<%uc> ", _getValueAAFK<uint8_t>(pKeyData,wi,pRank,pKFieldList));
         break;
         }
case ZType_S8 :
        {
         wEltValue.sprintf("<%c> ",_getValueAAFK<int8_t>(pKeyData,wi,pRank,pKFieldList));
         break;
        }

 case ZType_U16 :
          {
          wEltValue.sprintf("<%ud> ",_getValueAAFK<uint16_t>(pKeyData,wi,pRank,pKFieldList));
          break;
          }
 case ZType_S16 :
         {
         wEltValue.sprintf("<%d> ",_getValueAAFK<int16_t>(pKeyData,wi,pRank,pKFieldList));
         break;
         }
 case ZType_U32 :
          {
         wEltValue.sprintf("<%uld> ",_getValueAAFK<uint32_t>(pKeyData,wi,pRank,pKFieldList));
         break;
          }
 case ZType_S32 :
         {
         wEltValue.sprintf("<%ld> ",_getValueAAFK<int32_t>(pKeyData,wi,pRank,pKFieldList));
         break;
         }
 case ZType_U64 :
          {
         wEltValue.sprintf("%ulld ",_getValueAAFK<uint64_t>(pKeyData,wi,pRank,pKFieldList));
         break;
          }
 case ZType_S64 :
         {
         wEltValue.sprintf("<%lld> ",_getValueAAFK<int64_t>(pKeyData,wi,pRank,pKFieldList));
         break;
         }
case ZType_Float :
             {
             wEltValue.sprintf("<%f> ",_getValueAAFK<float>(pKeyData,wi,pRank,pKFieldList));
             break;
             }
 case ZType_Double :
              {
             wEltValue.sprintf("<%g> ",_getValueAAFK<double>(pKeyData,wi,pRank,pKFieldList));
             break;
              }
 case ZType_LDouble :
              {
             wEltValue.sprintf("<%g> ",_getValueAAFK<long double>(pKeyData,wi,pRank,pKFieldList));
             break;
              }

    default:
             {
                 ZException.setMessage(_GET_FUNCTION_NAME_,
                                         ZS_INVTYPE,
                                         Severity_Fatal,
                                         "Invalid ZType <%ld> <%s> encountered while processing key data",
                                         pKFieldList[pRank].ZType,
                                         decode_ZType(pKFieldList[pRank].ZType));
                 ZException.exit_abort();
             }

        }//switch

    pOutValue.appendData(wEltValue.content,wEltValue.size());
    }//for

return pOutValue;
}//_printArrayValueFromKey

template <class _Tp>
static inline
ZDataBuffer&
_convertAtomicEdian(ZDataBuffer& pData,_Tp &pValue, ZSIndexField_struct & pField)
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
template <class _Tp>
ZDataBuffer&
_convertAtomicNOEndian(ZDataBuffer& pData,_Tp pValue, ZSIndexField_struct & pField)
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
_convert(typename std::enable_if<std::is_integral<_Tp>::value, _Tp>::type &pValue,ZSIndexField_struct pField,ZDataBuffer &pData) {
  // an implementation for integral types (int, char, unsigned, etc.)
    if (is_little_endian()) // if system uses little endian integral internal representation
            {
            return _convertAtomicEdian(pData,pValue,pField);
            }
    return _convertAtomicNOEndian(pData,pValue,pField);  //! or not
}

template <typename _Tp> // no conversion for pointer
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


//----------Get sizes per Atomic ZType_Type-------------------------

/**
 * @brief _getAtomicZType_Sizes returns Natural and Internal sizes for an Atomic ZType_type given by pType
 * @param[in] pType
 * @param[out] pNaturalSize
 * @param[out] pKeyFieldSize
 * @return
 */
ZStatus
getAtomicZType_Sizes(ZTypeBase pType,
                     uint64_t& pNaturalSize,
                     uint64_t& pUniversalSize) // not a template
{

    ZTypeBase wType = pType & ZType_AtomicMask;
    switch (wType)
    {
    case ZType_UChar :
    case ZType_Char : // array of char is a special case because no sign byte is added
    case ZType_U8 :
        {
        pNaturalSize=sizeof(uint8_t);
        pUniversalSize = pNaturalSize;
        return  ZS_SUCCESS;
        }
    case ZType_S8 :
        {
        pNaturalSize=sizeof(uint8_t);
        pUniversalSize = pNaturalSize+1;
        return  ZS_SUCCESS;
        }
    case ZType_S16 :
        {
        pNaturalSize=sizeof(int16_t);
        pUniversalSize = pNaturalSize+1;
        return  ZS_SUCCESS;
        }
    case ZType_U16 :
        {
        pNaturalSize=sizeof(uint16_t);
        pUniversalSize = pNaturalSize;
        return  ZS_SUCCESS;
        }
    case ZType_S32 :
        {
        pNaturalSize=sizeof(int32_t);
        pUniversalSize = pNaturalSize+1;
        return  ZS_SUCCESS;
        }
    case ZType_U32 :
        {
        pNaturalSize=sizeof(uint32_t);
        pUniversalSize = pNaturalSize;
        return  ZS_SUCCESS;
        }
    case ZType_S64 :
        {
        pNaturalSize=sizeof(int64_t);
        pUniversalSize = pNaturalSize+1;
        return  ZS_SUCCESS;
        }
    case ZType_U64 :
        {
        pNaturalSize=sizeof(uint64_t);
        pUniversalSize = pNaturalSize;
        return  ZS_SUCCESS;
        }
    case ZType_Float :
        {
        pNaturalSize=sizeof(float);
        pUniversalSize = pNaturalSize+1;
        return  ZS_SUCCESS;
        }
    case ZType_Double :
        {
        pNaturalSize=sizeof(double);
        pUniversalSize = pNaturalSize+1;
        return  ZS_SUCCESS;
        }
    case ZType_LDouble :
        {
        pNaturalSize=sizeof(long double);
        pUniversalSize = pNaturalSize+1;
        return  ZS_SUCCESS;
        }
    default:
        {
        pNaturalSize = -1;
        pUniversalSize = -1;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Error,
                                " Invalid type (hex)<%x> given type analysis is %s",
                                pType,
                                decode_ZType(pType));
        return  ZS_INVTYPE;
        }
    }// case
return ZS_SUCCESS;
}//_getAtomicZType_Sizes





/** @} */ //  End group ZIndexGroup






/** @} */ // ZKeyGroup

utfdescString ZStringBuffer;

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
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_Atomic ") ;
            break;
            }
    if (pType & ZType_Pointer)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_Pointer ") ;
            break;
            }
    if (pType & ZType_Array)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_Array ") ;
            break;
            }
/*    if (pType & ZType_Enum)
            {
            ZStringBuffer = "ZType_Enum";
            break;
            }*/
    if (pType & ZType_StdString) // we may have a pointer to a std::string
    {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_StdString");
            break;
    }
    ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_StructUnknown ");
    break;
    }
    if (pType & ZType_Signed)
            ZStringBuffer .addConditionalOR((const utf8_t*)"ZType_Signed ");
    if (pType & ZType_Endian)
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_Endian ");

    while (true) // carefull to put first signed type otherwise detect unsigned while it is signed (same mask with sign byte )
    {
    if ((pType & ZType_Char)==ZType_Char)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_Char") ;
            break;
            }
    if ((pType & ZType_UChar)== ZType_UChar)
        {
        ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_UChar") ;
        break;
        }


    if ((pType & ZType_S8)==ZType_S8)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_S8") ;
            break;
            }
    if ((pType & ZType_U8)==ZType_U8)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_U8") ;
            break;
            }

    if ((pType & ZType_S16)==ZType_S16)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_S16") ;
            break;
            }
    if ((pType & ZType_U16)==ZType_U16)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_U16") ;
            break;
            }

    if ((pType & ZType_S32)==ZType_S32)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_S32") ;
            break;
            }
    if ((pType & ZType_U32)==ZType_U32)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_U32") ;
            break;
            }

    if ((pType & ZType_S64)==ZType_S64)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_S64") ;
            break;
            }
    if ((pType & ZType_U64)==ZType_U64)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_U64") ;
            break;
            }

    if ((pType & ZType_Float)==ZType_Float)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_Float") ;
            break;
            }
    if ((pType & ZType_Double)==ZType_Double)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_Double") ;
            break;
            }
    if ((pType & ZType_LDouble)==ZType_LDouble)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_LDouble") ;
            break;
            }

    if ((pType & ZType_Class)==ZType_Class)
            {
            ZStringBuffer.addConditionalOR((const utf8_t*)"ZType_Compound") ;
            break;
            }
    ZStringBuffer=(const utf8_t*)"ZType_DataUnknown";
    break;
    }
    return ZStringBuffer.toCString_Strait() ;
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




#endif // ZSINDEXTYPE_CPP
