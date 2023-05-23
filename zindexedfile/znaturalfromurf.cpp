#ifndef ZNATURALFROMURF_CPP
#define ZNATURALFROMURF_CPP
#include <zindexedfile/znaturalfromurf.h>

#include <ztoolset/utfvtemplatestring.h>

#include <ztoolset/charfixedstring.h>
#include <ztoolset/utffixedstring.h>

#include <ztoolset/charvaryingstring.h>
#include <ztoolset/utfvaryingstring.h>

ZStatus
_getBlobNfURF(void* pNatural,
             const unsigned char* pURFInData,
             ZTypeBase pTargetType)
{
    if (pTargetType!=ZType_Blob)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target type  <%s> given to convert from source type <%s>. Only type allowed is ZType_Blob.",
                              decode_ZType(pTargetType),
                              decode_ZType(ZType_Blob));
        return ZS_INVTYPE;
        }
    ZDataBuffer* wZDB=static_cast<ZDataBuffer*>(pNatural);
    wZDB->_importURF(pURFInData);
    return ZS_SUCCESS;

}// setBlobfURF





/**
  @defgroup DATA_FORMAT Natural vs Universal vs UVF vs URF

  Natural format :

    Data format internally stored on computer. Subject to Endianness (if system is little endian) an Sign (if data is signed)

  Universal format :

    Data format as it is stored as a key field : it may be sorted

    Data is stored independantly of Endianness, with a preceeding byte sign if data is signed
    Universal format must be mandatorily coupled with
        - ZType_type  : enough for ZType_Atomic and ZType_Class (ZType_Date, ZType_Checksum,etc.) : size is deduced from type
        - if not atomic or class (ZType_Array and ZType_Strings) effective array count

  Universal Varying Format (UVF) : concerns fixed strings (and only fixed strings)

    Data format as it is stored within exported blocks in files headers (ZRF, ZMF, ZIF)
    string content is preceeded with an int16_t mentionning length of the string


 Universal Record Format (URF)

    Data format as it is stored within a file.
    Data is preceeded with a header describing the data.
    This header changes according ZType_type of data.


  ZType_Type vs URF Header sizes

  ZType_Atomic          ZTypeBase
  ------------          data size is deduced from atomic type size
                        see : getAtomicZType_Sizes() function <zindexedfile/zdatatype.h>

  ZType_Class
  -----------
  ZType_Date            ZTypeBase
                        data size is deduced from object class size.
                        size is sizeof (uint32_t)

  ZType_DateFull        ZTypeBase
                        data size is deduced from object class size
                        size is sizeof (uint64_t)

  ZType_CheckSum        ZTypeBase
                        data size is deduced from object class size
                        size is sizeof (cst_checksum)
  ZType_String
  ------------
  ZType_utfxxFixedString    ZTypeBase : gives the object type and the size of character unit i.e. ZType_Char, ZType_U8, ZType_U16, ZType_U32
                            uint16_t :  canonical size (capacity) : capacity of string class (maximum number of character units)
                            uint16_t : Effective Universal size in bytes : universal size in bytes of stored data (excluding URF header)
                                    NB: this size differs from canonical size representing the capacity of the string in character units

                        NB: Maximum number of bytes is <65'534>. Maximum number of characters depends on atomic type.

  ZType_FixedWString    ZTypeBase
                        uint16_t : canonical size : capacity of string class (number of characters)
                        uint16_t : Effective Universal size : universal size in bytes of stored data
                                    NB: this size differs from canonical size representing the capacity of the string in char

                        NB: Maximum number of characters is <32'765>.

  ZType_utfxxVaryingString  ZTypeBase  : gives the size of character unit i.e. ZType_U8, ZType_U16, ZType_U32
                            uint64_t : data byte size
                            warning :  data byte size WITHOUT Header Size

  ZType_VaryingWString  ZTypeBase
                        uint64_t : data byte size
                        warning :   Number of characters is <data byte size / sizeof(wchar_t)>
                                    data byte size WITHOUT Header Size

  ZType_Array           ZTypeBase  : gives the size of character unit i.e. ZType_U8, ZType_U16, ZType_U32
  -----------           uint32_t : Canonical (Array) count
                        warning : number of bytes is (Array Count * atomic data size) WITHOUT Header Size

  ZType_Blob            ZTypeBase
  ----------            uint64_t : data byte size WITHOUT Header Size

  */

/**
 * @brief _getURFHeaderData  Gets the information from URF header data of a field
 *
 *      see <zindexedfile/znaturalfromurf> <ZType_Type vs URF Header sizes>
 *
 *
 * @param[in pURF_Ptr           Pointer to data with URF format (URF header leading data content) : first byte of URF header
 * @param[out] pZType           ZTypeBase describing data
 * @param[out] pUniversalSize   Universal size in bytes without Header
 * @param[out] pNaturalSize     Natural size in bytes without Header
 * @param[out] pCanonical       ONLY FOR FIXED STRINGS AND ARRAYS :
 *                              FIXED STRINGS:  Canonical size : number of element of array or capacity of a fixed string
 *                              ARRAYS: array count
 *                              set to 1 otherwise
 * @param[out] pEffectiveUSize  ONLY FOR FIXED STRINGS effective size in bytes of stored data URF header EXCLUDED
 *                              set to 1 otherwise
 * @param[out] pHeaderSize      size in bytes of URF header
 * @return pointer to start of effective field data as unsigned char* (first byte after data header)
 */
ZStatus _getURFHeaderData(const unsigned char* pURF_Ptr,
                                 ZTypeBase &pZType,
                                 uint64_t &pUniversalSize,
                                 uint64_t &pNaturalSize,
                                 uint16_t &pCapacity,       // only for strings and arrays. Otherwise is set to 1
                                 uint16_t &pEffectiveUSize, // only for strings and arrays. Otherwise is set to 1
                                 uint64_t &pHeaderSize,
                                 const unsigned char** pURFdDataPtr)
{


URF_UnitCount_type  wFUniversalSize;
URF_UnitCount_type  wVUniversalSize;

ZTypeBase           wStructType;
URF_UnitCount_type  wUSize=0;
const unsigned char* wURFDataPtr;

    wURFDataPtr=getURFBufferValue<ZTypeBase>(&pZType,pURF_Ptr);

    switch (pZType)
    {
    case ZType_Blob:  // header is
        pHeaderSize=sizeof(ZTypeBase)+sizeof(URF_UnitCount_type);

        wURFDataPtr=getURFBufferValue<URF_UnitCount_type>(&wUSize,wURFDataPtr);
        pNaturalSize=pUniversalSize=wUSize;

        pEffectiveUSize=pCapacity=1;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;

    case ZType_ZDate:
        pHeaderSize= sizeof(ZTypeBase);
        pUniversalSize=pNaturalSize=sizeof(uint32_t);
        pCapacity=pEffectiveUSize=1;
        wURFDataPtr=pURF_Ptr+pHeaderSize;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;
    case ZType_ZDateFull:
        pHeaderSize= sizeof(ZTypeBase);
        pUniversalSize=pNaturalSize=sizeof(uint64_t);
        pCapacity=pEffectiveUSize=1;
        wURFDataPtr=pURF_Ptr+pHeaderSize;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;
    case ZType_CheckSum:
        pHeaderSize= sizeof(ZTypeBase);
        pUniversalSize=pNaturalSize=sizeof(checkSum::content);
        pCapacity=pEffectiveUSize=1;
        wURFDataPtr=pURF_Ptr+pHeaderSize;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;
/*    case ZType_VaryingCString: // varying strings & blobs  : only ZType_type plus uint64_t : effective size
        {pURFData
        pHeaderSize=sizeof(ZTypeBase)+sizeof(uint64_t);
        memmove (&wUSize,(pURF_Ptr+sizeof(ZTypeBase)),sizeof(uint64_t));
        pEffectiveUSize=pCanonical=1;
        pNaturalSize=pUniversalSize=reverseByteOrder_Conditional<uint64_t>(wUSize);
        break;
        }
    case ZType_VaryingWString: // varying strings & blobs
        varyingWString wS;
        pHeaderSize=sizeof(ZTypeBase)+sizeof(uint64_t);
        memmove (&wUSize,(pURF_Ptr+sizeof(ZTypeBase)),sizeof(uint64_t));
        pCanonical=pEffectiveUSize=1;
        pNaturalSize=pUniversalSize=reverseByteOrder_Conditional<uint64_t>(wUSize);
        pEffectiveDataPtr=pURF_Ptr+pHeaderSize;
        return  pEffectiveDataPtr;*/
/*    case ZType_FixedCString: // fixed C strings : ZType + canonical size (uint16_t)+ effective size (uint16_t)
        {
        pHeaderSize=sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t);
        memmove (&wUSize,(pURF_Ptr+sizeof(ZTypeBase)),sizeof(uint16_t));
        pNaturalSize= pCanonical=reverseByteOrder_Conditional<uint16_t>(wUSize);
        memmove (&wUSize,(pURF_Ptr+sizeof(ZTypeBase)+sizeof(uint16_t)),sizeof(uint16_t));
        pEffectiveUSize=pUniversalSize=reverseByteOrder_Conditional<uint16_t>(wUSize);
        break;
        }*/
/*    case ZType_FixedWString: // fixed Wide strings : ZType + canonical = nb of WChars (uint16_t) + byte size (uint16_t)
        pHeaderSize=sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t);

        wURFDataPtr=getURFBufferValue<URF_Capacity_type>(&pCapacity,wURFDataPtr);
        wURFDataPtr=getURFBufferValue<URF_Fixed_Size_type>(&wFUniversalSize,wURFDataPtr);
        pNaturalSize= (uint64_t)pCapacity;
        pUniversalSize=(uint64_t)wFUniversalSize;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;

        return  ZS_SUCCESS;
*/
    case ZType_CharFixedString: // fixed strings
    case ZType_Utf8FixedString:
    case ZType_Utf16FixedString:
    case ZType_Utf32FixedString:
        pHeaderSize=sizeof(ZTypeBase)+sizeof(URF_UnitCount_type)+sizeof(URF_Capacity_type);

        wURFDataPtr=getURFBufferValue<URF_Capacity_type>(&pCapacity,wURFDataPtr);
        wURFDataPtr=getURFBufferValue<URF_UnitCount_type>(&wFUniversalSize,wURFDataPtr);
        pNaturalSize= (uint64_t)pCapacity;
        pUniversalSize=(uint64_t)wFUniversalSize;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;

    case ZType_CharVaryingString: // varying strings
    case ZType_Utf8VaryingString:
    case ZType_Utf16VaryingString:
    case ZType_Utf32VaryingString:
    case ZType_URIString:
        pHeaderSize=sizeof(ZTypeBase)+sizeof(URF_UnitCount_type);

        wURFDataPtr=getURFBufferValue<URF_UnitCount_type>(&wFUniversalSize,wURFDataPtr);
        pNaturalSize= pUniversalSize=(uint64_t)wFUniversalSize;
        pCapacity = 0;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;
    default: // varying strings & blobs
        break;
    }// switch

    wStructType= pZType & ZType_StructureMask;
    switch (pZType & ZType_StructureMask)
    {
    case ZType_Atomic:
        pHeaderSize= sizeof(ZTypeBase);
        getAtomicZType_Sizes((pZType & ZType_AtomicMask),pNaturalSize,pUniversalSize);
        pCapacity=1;
        pEffectiveUSize = pUniversalSize;
//        *pURFDataPtr += pHeaderSize; // already done
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;

    case ZType_Array: /* for array capacity contains the full byte size of array */
        pHeaderSize=sizeof(ZTypeBase)+sizeof(URF_Capacity_type);

        wURFDataPtr=getURFBufferValue<URF_Capacity_type>(&pCapacity,wURFDataPtr);
        pNaturalSize= pEffectiveUSize =(uint64_t)pCapacity;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;

    case cst_ZBLOCKEND:
    case cst_ZBUFFEREND:
      return ZS_EOF;

    default:    // Other types : data length is on uint16_t, canonical (array) count is systematically set to pUniversalSize
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,ZS_INVTYPE,Severity_Severe,"unrecognized object type 0x%X %s",
                              pZType,decode_ZType(pZType));
        fprintf(stderr,"%s> unrecognized object type 0x%X %s\n",
                _GET_FUNCTION_NAME_,
                pZType,
                decode_ZType(pZType));
        return  ZS_INVTYPE;

        }
    }// switch

    return  ZS_INVOP; // not understood - by the way not possible too.
}//_getURFHeaderData


#ifdef __DEPRECATED__
ZStatus
getUniversalFromURF (ZDataBuffer &pValue,const unsigned char* pDataPtr,bool pTruncate,const unsigned char** pDataPtrOut)
{

  if (pDataPtr==nullptr)
    return ZS_NULLPTR;
  ZTypeBase wType;
  memmove(&wType,pDataPtr,sizeof(ZTypeBase));
  wType=reverseByteOrder_Conditional<ZTypeBase>(wType);

  switch (wType)
  {
  case ZType_Utf8FixedString:
  {
    if (pTruncate)
      return utfStringHeader::getUniversalFromURF(ZType_Utf8FixedString,pDataPtr,pValue,pDataPtrOut);
    else
      return utf8FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf8FixedString,pDataPtr,pValue,pDataPtrOut);
  }//ZType_Utf8FixedString

  case ZType_Utf16FixedString:
  {
    if (pTruncate)
      return utf16FixedString<cst_desclen>::getUniversalFromURF_Truncated(ZType_Utf16FixedString,pDataPtr,pValue,pDataPtrOut);
    else
      return utf16FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf16FixedString,pDataPtr,pValue,pDataPtrOut);
  }//ZType_Utf16FixedString
  case ZType_Utf32FixedString:
  {
    if (pTruncate)
      return utf32FixedString<cst_desclen>::getUniversalFromURF_Truncated(ZType_Utf32FixedString,pDataPtr,pValue,pDataPtrOut);
    else
      return utf32FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf16FixedString,pDataPtr,pValue,pDataPtrOut);
  }//ZType_Utf32FixedString
  case ZType_Utf8VaryingString:
  {
    return utfStringHeader::getUniversalFromURF(ZType_Utf8VaryingString,pDataPtr,pValue,pDataPtrOut);

  }//ZType_Utf8VaryingString
  case ZType_Utf16VaryingString:
  {
    return utf16VaryingString::getUniversalFromURF(ZType_Utf16VaryingString,pDataPtr,pValue,pDataPtrOut);
  }//ZType_Utf16VaryingString
  case ZType_Utf32VaryingString:
  {
    return utf32VaryingString::getUniversalFromURF(ZType_Utf32VaryingString,pDataPtr,pValue,pDataPtrOut);
  }//ZType_Utf32VaryingString
    /*
    case ZType_FixedCString:
        {
        if (pTruncate)
            return templateString<cst_desclen>::getUniversalFromURF_Truncated(RDic->Tab[pRank].URFData->Data,pValue);
        else
            return templateString<cst_desclen>::getUniversalFromURF(RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_FixedCString

  case ZType_FixedWString:
  {
    if (pTruncate)
      return templateWString<cst_desclen>::getUniversalFromURF_Truncated(RDic->Tab[pRank].URFData->Data,pValue);
    else
      return templateWString<cst_desclen>::getUniversalFromURF(RDic->Tab[pRank].URFData->Data,pValue);
  }//ZType_FixedCWtring

  case ZType_VaryingCString:
  {
    return varyingCString::getUniversalFromURF(wDataPtr,pValue);
  }//ZType_VaryingCString

  case ZType_VaryingWString:
  {
    return varyingWString::getUniversalFromURF(wDataPtr,pValue);
  }//ZType_VaryingWString
    */

  case ZType_ZDate:
    {
      return ZDate::getUniversalFromURF(pDataPtr,pValue,pDataPtrOut);
    }
  case ZType_ZDateFull:
  {
    return ZDateFull::getUniversalFromURF(pDataPtr,pValue,pDataPtrOut);
  }
  case ZType_CheckSum:
  {
    return checkSum::getUniversalFromURF(pDataPtr,pValue,pDataPtrOut);
  }

  case ZType_Blob:
  {
    abort();
//    return ZBlob::getUniversalFromURF(pDataPtr,pValue,pDataPtrOut);
  }

  }// switch (wType)



  if (wType&ZType_Atomic)
  {
    ZTypeBase wTypeAtomic=wType&ZType_AtomicMask;
    size_t wUSize=getAtomicUniversalSize(wTypeAtomic);
    pValue.setData(pDataPtr+sizeof(ZTypeBase),wUSize);

    if (pDataPtrOut)
    {
      *pDataPtrOut = pDataPtr +sizeof(ZTypeBase)+ wUSize;
    }
    return ZS_SUCCESS;
  }

  if (wType&ZType_Array)
  {
    ZTypeBase wTypeAtomic=wType&ZType_AtomicMask;
    size_t wUSize=getAtomicUniversalSize(wTypeAtomic);
    pDataPtr += sizeof (ZTypeBase);
    uint16_t wArrayCount;
    memmove(&wArrayCount,pDataPtr,sizeof(wArrayCount));
    wArrayCount=reverseByteOrder_Conditional<uint16_t>(wArrayCount);
    wUSize=wUSize*wArrayCount;
    pDataPtr += sizeof (uint16_t);
    pValue.setData(pDataPtr,wUSize);

    if (pDataPtrOut)
    {
      *pDataPtrOut = pDataPtr + wUSize;
    }

    return ZS_SUCCESS;
  }

  return ZS_INVTYPE;
}//getUniversalbyField

#endif // __DEPRECATED__

/**
 *  One to one conversion routines for =====object classes======
 *  Remark: Necessary to have a separate routine witn void* value statically casted to appropriate class to avoid compilation issues
 *
*/
ssize_t
get_121_BlobNfURF(void* pValue, const unsigned char *pURFData)
{
  static_cast<ZDataBuffer*>(pValue)->_importURF(pURFData);
  return ZS_SUCCESS;
}
ssize_t get_121_ZDateFullNfURF(void* pValue, ZDataBuffer *pURFData)
{
  const unsigned char* wPtr=pURFData->Data;
  return static_cast<ZDateFull*>(pValue)->_importURF(wPtr) ;

}
/* Deprecated
ssize_t
get_121_ZDateNfURF(void* pValue,ZDataBuffer * pURFData)
{
  const unsigned char* wPtr=pURFData->Data;
  return static_cast<ZDate*>(pValue)->_importURF(wPtr);
}
*/
ssize_t get_121_CheckSumNfURF(void* pValue, ZDataBuffer *pURFData)
{
  const unsigned char* wPtr=pURFData->Data;
  return static_cast<checkSum*>(pValue)->_importURF(wPtr);
}

ssize_t
get_ZStringNfURF(void* pValue, ZTypeBase pType, ZDataBuffer *&pURFData)
{
    if (pType&ZType_VaryingLength)
    {
    const unsigned char* wPtr=pURFData->Data;
    switch (pType&ZType_AtomicMask)
    {
        case ZType_Char:
        case ZType_UChar:
          return (static_cast<utfVaryingString<char>*>(pValue)->_importURF(wPtr));
        case ZType_U8:
        case ZType_S8:
              return static_cast<utfVaryingString<utf8_t>*>(pValue)->_importURF(wPtr);
        case ZType_U16:
        case ZType_S16:
              return static_cast<utfVaryingString<utf16_t>*>(pValue)->_importURF(wPtr);
          return ZS_SUCCESS;
        case ZType_U32:
        case ZType_S32:
          return static_cast<utfVaryingString<utf32_t>*>(pValue)->_importURF(wPtr);

    default:
        return -1;
        }
    }
  return -1;
//    return  static_cast<utfStringHeader*>(pValue)->_importURFGeneric(pURFData->Data);
}// get_ZStringNfURF


/**
 * @brief _getZDateFullNfU gets a ZDateFull object from and URF data that could be
 *
 *      - ZDateFull (121)
 *      - ZDate     a conversion is needed
 *
 *
 * @param[out] pValue   a void pointer to ZDateFull object to feed. Pointer must be void.
 * @param[in]  pURFData URF data to feed ZDateFull with
 * @param[in]  pTargetType Obviously ZType_ZDateFull
 * @param[in]  pArrayCount  non significant
 * @return     a ZStatus
 */
ZStatus
_getZDateFullNfURF(void* pValue,const unsigned char* pURFData,ZTypeBase pTargetType,uint64_t pTargetNSize,uint32_t pTargetArrayCount)
{

ZDateFull wDate;
  pTargetArrayCount=1;
  wDate._importURF(pURFData);
  switch(pTargetType)
    {
    case ZType_ZDateFull:
    {
    memmove(pValue,&wDate,sizeof(wDate));
    return  ZS_SUCCESS;
    }
    /* Deprecated
    case ZType_ZDate:
    {
     ZDate *wValue=static_cast<ZDate*>(pValue);
     wValue->fromZDateFull(wDate);
     return  ZS_SUCCESS;
    }
    */
    case ZType_ArrayChar:
    {
       char* wNatural = static_cast<char*>(pValue);
       strncpy(wNatural,wDate.toFormatted().toCString_Strait(),pTargetNSize);
       return  ZS_SUCCESS;
    }
    case ZType_Utf8VaryingString:
    {
/*        utf8VaryingString* wValue=static_cast<utf8VaryingString*>(pValue);
        descString wDateS =wDate.toFormatted().toString();
        wValue->fromCString_PtrCount(wDateS.toString(),wDateS.size());
*/
        static_cast<utf8VaryingString*>(pValue)->strset(wDate.toFormatted().toString());
        return  ZS_SUCCESS;
    }
    case ZType_Utf16VaryingString:
    {
/*        utf8VaryingString* wValue=static_cast<utf8VaryingString*>(pValue);
        descString wDateS =wDate.toFormatted().toString();
        wValue->fromCString_PtrCount(wDateS.toString(),wDateS.size());
*/
        static_cast<utf16VaryingString*>(pValue)->fromUtf8(wDate.toFormatted().toString());
        return  ZS_SUCCESS;
    }
    case ZType_Utf32VaryingString:
    {
/*        utf8VaryingString* wValue=static_cast<utf8VaryingString*>(pValue);
        descString wDateS =wDate.toFormatted().toString();
        wValue->fromCString_PtrCount(wDateS.toString(),wDateS.size());
*/
        static_cast<utf32VaryingString*>(pValue)->fromUtf8(wDate.toFormatted().toString());
        return  ZS_SUCCESS;
    }
/*    case ZType_FixedCString:
    {

        templatePString* wValue=static_cast<templatePString*>(pValue);
        descString wDateS =wDate.toFormatted().toString();
        strncpy(wValue->content,wDateS.toString(),wValue->capacity());
        return ZS_SUCCESS;
    }*/
    default:
         {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Incompatible source and target data type : cannot convert from %x <%s> to %x <%s>.",
                              ZType_ZDateFull,
                              decode_ZType(ZType_ZDateFull),
                              pTargetType,
                              decode_ZType(pTargetType));
        return  ZS_INVTYPE;
         }
    }//switch
    return  ZS_SUCCESS;
}//_getZDateFullNfU

#ifdef __DEPRECATED__
ZStatus
_getZDateNfURF(void* pValue,const unsigned char* pURFData,ZTypeBase pTargetType,uint64_t pTargetNSize,uint32_t pTargetArrayCount)
{
ZDate wDate;
  pTargetArrayCount = 1;
  wDate._importURF(pURFData);


  switch(pTargetType)
    {
    case ZType_ZDate:
    {
    memmove(pValue,&wDate,sizeof(wDate));
    return  ZS_SUCCESS;
    }
    case ZType_ZDateFull:
    {
     ZDateFull *wValue=static_cast<ZDateFull*>(pValue);
     wValue->fromZDate(wDate);
     return  ZS_SUCCESS;
    }
    case ZType_ArrayChar:
    {
/*       char* wNatural = static_cast<char*>(pValue);
       descString wDateS =wDate.toFormatted().toString();*/
       strncpy((char*)pValue,wDate.toFormatted().toCString_Strait(),pTargetNSize);
       return  ZS_SUCCESS;
    }
    case ZType_Utf8FixedString:
    {
        utf8VaryingString* wValue=static_cast<utf8VaryingString*>(pValue);
        wValue->strset(wDate.toFormatted().toUtf());
        return  ZS_SUCCESS;
    }
    case ZType_Utf16FixedString:
    {
        utfStringHeader* wValue=static_cast<utfStringHeader*>(pValue);
        const utf8_t* wPtrIn=wDate.toFormatted().toString();
        utf16_t* wPtrOut= (utf16_t*)wValue->DataByte;
        size_t wi=wValue->getUnitCount();
        memset (wPtrOut,0,wi);
        while ((wi -- )&&(*wPtrIn))
            *wPtrOut++=(utf16_t)*wPtrIn++;
        return  ZS_SUCCESS;
    }
    case ZType_Utf32FixedString:
    {
        utfStringHeader* wValue=static_cast<utfStringHeader*>(pValue);
        const utf8_t* wPtrIn=wDate.toFormatted().toUtf();
        utf32_t* wPtrOut= (utf32_t*)wValue->DataByte;
        size_t wi=wValue->getUnitCount();
        memset (wPtrOut,0,wi);
        while ((wi -- )&&(*wPtrIn))
            *wPtrOut++=(utf32_t)*wPtrIn++;
        return  ZS_SUCCESS;
    }

    case ZType_Utf8VaryingString:
    {
        utf8VaryingString* wValue=static_cast<utf8VaryingString*>(pValue);
        wValue->strset(wDate.toFormatted().toUtf());
        return  ZS_SUCCESS;
    }
    case ZType_Utf16VaryingString:
    {
        utf16VaryingString* wValue=static_cast<utf16VaryingString*>(pValue);
        wValue->fromUtf8(wDate.toFormatted().toUtf());
        return  ZS_SUCCESS;
    }
    case ZType_Utf32VaryingString:
    {
        utf32VaryingString* wValue=static_cast<utf32VaryingString*>(pValue);
        wValue->fromUtf8(wDate.toFormatted().toUtf());
        return  ZS_SUCCESS;
    }
    default:
         {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Incompatible source and target data type : cannot convert from <%s> to <%s>.",
                              decode_ZType(ZType_ZDate),
                              decode_ZType(pTargetType));
        return  ZS_INVTYPE;
         }
    }//switch
}//_getZDateNfU
#endif // __DEPRECATED__
ZStatus
_getCheckSumNfURF(void* pValue,const unsigned char* pURFData,ZTypeBase pTargetType)
{
checkSum wCheckSum;
    if (pTargetType!=ZType_CheckSum)
        {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                             ZS_INVTYPE,
                             Severity_Severe,
                             "Incompatible source and target data type : cannot convert from <%s> to <%s>.",
                             decode_ZType(ZType_CheckSum),
                             decode_ZType(pTargetType));
       return ZS_INVTYPE;
        }
    wCheckSum._importURF(pURFData);
    return ZS_SUCCESS;
}//_getCheckSumNfU



#endif // ZNATURALFROMURF_CPP
