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
             unsigned char* pURFInData,
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
    ZBlob* wZDB=static_cast<ZBlob*>(pNatural);
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
ZStatus _getURFHeaderData(unsigned char* pURF_Ptr,
                                 ZTypeBase &pZType,
                                 uint64_t &pUniversalSize,
                                 uint64_t &pNaturalSize,
                                 uint16_t &pCapacity,       // only for strings and arrays. Otherwise is set to 1
                                 uint16_t &pEffectiveUSize, // only for strings and arrays. Otherwise is set to 1
                                 uint64_t &pHeaderSize,
                                 unsigned char** pURFdDataPtr)
{


URF_Fixed_Size_type     wFUniversalSize;
URF_Varying_Size_type   wVUniversalSize;

ZTypeBase wStructType;
uint64_t wUSize=0;
unsigned char* wURFDataPtr;

    wURFDataPtr=getURFBufferValue<ZTypeBase>(&pZType,pURF_Ptr);

    switch (pZType)
    {
    case ZType_Blob:  // header is
        pHeaderSize=sizeof(ZTypeBase)+sizeof(URF_Varying_Size_type);

        wURFDataPtr=getURFBufferValue<URF_Varying_Size_type>(&wUSize,wURFDataPtr);
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
        {
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
    case ZType_FixedWString: // fixed Wide strings : ZType + canonical = nb of WChars (uint16_t) + byte size (uint16_t)
        pHeaderSize=sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t);

        wURFDataPtr=getURFBufferValue<URF_Capacity_type>(&pCapacity,wURFDataPtr);
        wURFDataPtr=getURFBufferValue<URF_Fixed_Size_type>(&wFUniversalSize,wURFDataPtr);
        pNaturalSize= (uint64_t)pCapacity;
        pUniversalSize=(uint64_t)wFUniversalSize;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;

        return  ZS_SUCCESS;
    case ZType_CharFixedString: // fixed strings
    case ZType_Utf8FixedString:
    case ZType_Utf16FixedString:
    case ZType_Utf32FixedString:
        pHeaderSize=sizeof(ZTypeBase)+sizeof(URF_Fixed_Size_type)+sizeof(URF_Capacity_type);

        wURFDataPtr=getURFBufferValue<URF_Capacity_type>(&pCapacity,wURFDataPtr);
        wURFDataPtr=getURFBufferValue<URF_Fixed_Size_type>(&wFUniversalSize,wURFDataPtr);
        pNaturalSize= (uint64_t)pCapacity;
        pUniversalSize=(uint64_t)wFUniversalSize;
        if (pURFdDataPtr)
                *pURFdDataPtr=wURFDataPtr;
        return  ZS_SUCCESS;

    case ZType_CharVaryingString: // varying strings
    case ZType_Utf8VaryingString:
    case ZType_Utf16VaryingString:
    case ZType_Utf32VaryingString:
        pHeaderSize=sizeof(ZTypeBase)+sizeof(URF_Varying_Size_type);

        wURFDataPtr=getURFBufferValue<URF_Varying_Size_type>(&pUniversalSize,wURFDataPtr);
        pNaturalSize= (uint64_t)pUniversalSize;
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

    default:    // Other types : data length is on uint16_t, canonical (array) count is systematically set to pUniversalSize
        {
        fprintf(stderr,"%s> unrecognized object type 0x%X %s\n",
                _GET_FUNCTION_NAME_,
                pZType,
                decode_ZType(pZType));
        return  ZS_INVTYPE;
/*        uint16_t wUSize_16;
        pHeaderSize=sizeof(ZTypeBase)+sizeof(uint16_t);

        wURFDataPtr=getURFBufferValue<uint16_t>(&wUSize_16,wURFDataPtr);

        pUniversalSize=(uint64_t) wUSize_16 ;
        pEffectiveUSize=pUniversalSize ;
        pCapacity=wUSize_16;

        break;*/
        }
    }// switch

    return  ZS_INVOP; // not understood - by the way not possible too.
}//_getURFHeaderData

size_t  _getURFHeaderSize (ZTypeBase &pZType)
{

ZTypeBase wType,wStructType;
size_t wHeaderSize;
    wType=pZType;
    while (true)
    {
    if (wType==ZType_Blob)
        {
        wHeaderSize=sizeof(ZTypeBase)+sizeof(uint64_t);
        break;
        }
    if (wType==ZType_ZDate)
        {
        wHeaderSize= sizeof(ZTypeBase);
        break;
        }
    if (wType==ZType_ZDateFull)
        {
        wHeaderSize= sizeof(ZTypeBase);
        break;
        }
    if (wType==ZType_CheckSum)
        {
        wHeaderSize= sizeof(ZTypeBase);
        break;
        }
    if (wType&ZType_VaryingMask) // varying strings & blobs
        {
        wHeaderSize=sizeof(ZTypeBase)+sizeof(uint64_t);
        break;
        }
    if (wType&ZType_String) // Fixed strings
        {
        wHeaderSize=sizeof(ZTypeBase)+sizeof(URF_Capacity_type)+sizeof(URF_Fixed_Size_type);
        break;
        }
    wStructType= wType & ZType_StructureMask;
    switch (wStructType)
    {
    case ZType_Atomic:
        {
        wHeaderSize= sizeof(ZTypeBase);
        break;
        }
    case ZType_Array:
        {
        wHeaderSize=sizeof(ZTypeBase)+sizeof(uint32_t);
        break;
        }
    case ZType_String: // can only be a fixed string : checkSum has been processed before (checkSum is both a ZType_String & a ZType_Class)
        {
        wHeaderSize=sizeof(ZTypeBase)+(sizeof(uint16_t)*2);
        break;
        }
    default:    // Other types : data length is on uint32_t, canonical (array) count is systematically set to 1
        {
        wHeaderSize=sizeof(ZTypeBase)+sizeof(uint32_t);
        break;
        }
    }// switch
    break;
    }// while true
    return wHeaderSize;
}//_getURFHeaderSize

/**
 *  One to one conversion routines for =====object classes======
 *  Remark: Necessary to have a separate routine witn void* value statically casted to appropriate class to avoid compilation issues
 *
*/
ZStatus
get_121_BlobNfURF(void* pValue,unsigned char* pURFData)
{
        return static_cast<ZBlob*>(pValue)->_importURF(pURFData);
}
ZStatus
get_121_ZDateFullNfURF(void* pValue,unsigned char* pURFData)
{
    return static_cast<ZDateFull*>(pValue)->_importURF(pURFData);
}
ZStatus
get_121_ZDateNfURF(void* pValue,unsigned char* pURFData)
{
    return static_cast<ZDate*>(pValue)->_importURF(pURFData);
}
ZStatus
get_121_CheckSumNfURF(void* pValue,unsigned char* pURFData)
{
    return static_cast<checkSum*>(pValue)->_importURF(pURFData);
}

ZStatus
get_ZStringNfURF(void* pValue, ZTypeBase pType, unsigned char* &pURFData)
{

/*
ZTypeBase wZTypeSource, wZTypeTarget;
uint64_t wUniversalSize,wNaturalSize, wHeaderSize;
uint16_t wCanonical, wEffectiveUSize;
    utfStringHeader* wStringPtr=static_cast<utfStringHeader*>(pValue);

    unsigned char* wPtr=_getURFHeaderData(pURFData,
                                          wZTypeSource,
                                          wUniversalSize,
                                          wNaturalSize,
                                          wCanonical,
                                          wEffectiveUSize,
                                          wHeaderSize,
                                          wPtr);

    if (!wZTypeSource&ZType_String)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Error,
                              "Invalid fixed string object type <%X> <%s> while expecting one of String object.\n",
                              wZTypeTarget,
                              decode_ZType(wZTypeTarget));
        return  ZS_INVTYPE;
        
        }

*/
    if (pType&ZType_VaryingLength)
    {
    switch (pType&ZType_AtomicMask)
    {
        case ZType_Char:
        case ZType_UChar:
          if (static_cast<utfVaryingString<char>*>(pValue)->_importURF(pURFData)==0)
            return ZS_ERROR;
          return ZS_SUCCESS;
        case ZType_U8:
        case ZType_S8:
          if (static_cast<utfVaryingString<utf8_t>*>(pValue)->_importURF(pURFData)==0)
            return ZS_ERROR;
          return ZS_SUCCESS;
        case ZType_U16:
        case ZType_S16:
          if (static_cast<utfVaryingString<utf16_t>*>(pValue)->_importURF(pURFData)==0)
            return ZS_ERROR;
          return ZS_SUCCESS;
        case ZType_U32:
        case ZType_S32:
          if (static_cast<utfVaryingString<utf32_t>*>(pValue)->_importURF(pURFData)==0)
            return ZS_ERROR;
          return ZS_SUCCESS;
    default:
        break;
        }
    }
    return  static_cast<utfStringHeader*>(pValue)->_importURFGeneric(pURFData);
}// get_ZStringNfURF
/*
ZStatus
get_121_charFixedStringNfURF(void* pValue, unsigned char* pURFData)
{
    return static_cast<charFixedString*>(pValue)->_importURF(pURFData);
}
ZStatus
get_121_utf8FixedStringNfURF(void* pValue, unsigned char* pURFData)
{
    return static_cast<utfStringHeader*>(pValue)->_importURFGeneric(pURFData);
}
ZStatus
get_121_utf16FixedStringNfURF(void* pValue, unsigned char* pURFData)
{
    return static_cast<utf16FixedString*>(pValue)->_importURF(pURFData);
}
 ZStatus
get_121_utf32FixedStringNfURF(void* pValue, unsigned char* pURFData)
{
    return static_cast<utf32FixedString*>(pValue)->_importURF(pURFData);
}
 ZStatus
get_121_charVaryingStringNfURF(void* pValue, unsigned char* pURFData)
{
    return static_cast<charVaryingString*>(pValue)->_importURF(pURFData);
}
 ZStatus
get_121_utf8VaryingStringNfURF(void* pValue, unsigned char* pURFData)
{
    return static_cast<utf8VaryingString*>(pValue)->_importURF(pURFData);
}
 ZStatus
get_121_utf16VaryingStringNfURF(void* pValue, unsigned char* pURFData)
{
     
    return static_cast<utf16VaryingString*>(pValue)->_importURF(pURFData);
}
ZStatus
get_121_utf32VaryingStringNfURF(void* pValue, unsigned char* pURFData)
{
    return static_cast<utf32VaryingString*>(pValue)->_importURF(pURFData);
}
*/


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
_getZDateFullNfURF(void* pValue,unsigned char* pURFData,ZTypeBase pTargetType,uint64_t pTargetNSize,uint32_t pTargetArrayCount)
{


ZDateFull wDate;
    wDate._importURF(pURFData);
    switch(pTargetType)
    {
    case ZType_ZDateFull:
    {
    memmove(pValue,&wDate,sizeof(wDate));
    return  ZS_SUCCESS;
    }
    case ZType_ZDate:
    {
     ZDate *wValue=static_cast<ZDate*>(pValue);
     wValue->fromZDateFull(wDate);
     return  ZS_SUCCESS;
    }
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


ZStatus
_getZDateNfURF(void* pValue,unsigned char* pURFData,ZTypeBase pTargetType,uint64_t pTargetNSize,uint32_t pTargetArrayCount)
{


ZDate wDate;
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
        utf8_t* wPtrIn=wDate.toFormatted().toString();
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

ZStatus
_getCheckSumNfURF(void* pValue,unsigned char* pURFData,ZTypeBase pTargetType)
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

#ifdef __DEPRECATED_FIELD_CONVERSION__
ZStatus
_getFixedCStringNfURF(void* pValue, unsigned char* pURFData, ZTypeBase pTargetType, uint64_t pSourceNSize)
{
ZStatus wSt=ZS_SUCCESS;
templatePString* wPSourceString=nullptr ;

    switch (pSourceNSize)
    {
    case cst_codelen+1:
    {
        codeString* wSourceString=new codeString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePString*)wSourceString;
        wPSourceString->content =wSourceString->content;
        break;
    }
    case cst_desclen+1:
    {
        descString* wSourceString=new descString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePString*)wSourceString;
        wPSourceString->content =wSourceString->content;
        break;

    }
    case cst_messagelen+1:
    {
        messageString* wSourceString=new messageString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePString*)wSourceString;
        wPSourceString->content =wSourceString->content;
        break;

    }
    case cst_urilen+1:
    {
        uriString* wSourceString=new uriString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePString*)wSourceString;
        wPSourceString->content =wSourceString->content;
        break;
    }
    case cst_keywordlen+1:
    {
        keywordString* wSourceString=new keywordString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePString*)wSourceString;
        wPSourceString->content =wSourceString->content;
        break;
    }

    case cst_identitylen+1:
    {
        identityString* wSourceString=new identityString;
        wSourceString->_importURF(pURFData);
        wPSourceString==(templatePString*)wSourceString;
        wPSourceString->content =wSourceString->content;
        break;
    }
    default:
         {
        if (wPSourceString!=nullptr)
                            delete wPSourceString;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVSIZE,
                              Severity_Severe,
                              "Unknown source fixed string capacity of <%ld> : cannot convert from fixedCString to <%s>.",
                              pSourceNSize,
                              decode_ZType(pTargetType));
        return ZS_INVSIZE;
         }
    }// switch


  switch (pTargetType)
  {
  case ZType_FixedCString:
    {
      templatePString* wTargetPStr=static_cast<templatePString*> (pValue);
      size_t wStrSize=wPSourceString->size();
      if (wStrSize>(wTargetPStr->capacity()-1))
        {
          ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FIELDCAPAOVFLW,
                                Severity_Warning,
                                "Field type <%s> capacity <%ld> has been moved with <%ld> bytes and has been truncated to max capacity.",
                                decode_ZType(pTargetType),
                                wTargetPStr->capacity(),
                                wStrSize);
          wSt=ZS_FIELDCAPAOVFLW;
          wStrSize=(wTargetPStr->capacity()-1);
        }
      strncpy(wTargetPStr->content,wPSourceString->content,wStrSize);
      return (wSt);
    } // ZType_FixedCString
/*  case ZType_FixedWString:
    {
      templatePWString* wTargetPWStr=static_cast<templatePWString*> (pValue);
      size_t wStrSize=wPSourceString->size();
      if (wStrSize>(wTargetPWStr->capacity()-1))
        {
          ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FIELDCAPAOVFLW,
                                Severity_Warning,
                                "Field type <%s> capacity <%ld> has been moved with <%ld> bytes and has been truncated to max capacity.",
                                decode_ZType(pTargetType),
                                wTargetPWStr->capacity(),
                                wStrSize);
          wSt=ZS_FIELDCAPAOVFLW;
          wStrSize=wTargetPWStr->capacity()-1;
        }
      wTargetPWStr->fromCString_PtrCount(wPSourceString->content,wStrSize);
      return (wSt);
    } // ZType_FixedWString*/

  case ZType_VaryingCString:
    {
      varyingCString* wTargetVStr=static_cast<varyingCString*> (pValue);
      wTargetVStr->setData(wPSourceString->content,wPSourceString->size()+1);
      wTargetVStr->addConditionalTermination();
      delete wPSourceString;
      return ZS_SUCCESS;
    } // ZType_VaryingCString
/*  case ZType_VaryingWString:
    {
      varyingWString* wTargetVStr=static_cast<varyingWString*> (pValue);
      wTargetVStr->fromCString_PtrCount(wPSourceString->content,wPSourceString->size()+1);
      wTargetVStr->addConditionalTermination();
      return ZS_SUCCESS;
    } // ZType_VaryingWString*/
  case ZType_Blob:
  {
      ZDataBuffer* wTargetBlob=static_cast<ZDataBuffer*>(pValue);
      wTargetBlob->setData(wPSourceString->content,wPSourceString->size()+1);
      delete wPSourceString;
      return ZS_SUCCESS;
  }// ZType_Blob
  default:
       {
      delete wPSourceString;
      ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVTYPE,
                            Severity_Severe,
                            "Incompatible source and target data type : cannot convert from <%s> to <%s>.",
                            decode_ZType(ZType_FixedCString),
                            decode_ZType(pTargetType));
      return ZS_INVTYPE;
       }
  }
}//ZType_FixedCString

ZStatus
_getFixedWStringNfURF(void* pValue, unsigned char* pURFData, ZTypeBase pTargetType, uint64_t pSourceNSize)
{
ZStatus wSt=ZS_SUCCESS;
templatePWString* wPSourceString=nullptr ;



    switch (pSourceNSize)
    {
    case cst_codelen+1:
    {
        codeWString* wSourceString=new codeWString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePWString*)wSourceString;
        break;
    }
    case cst_desclen+1:
    {
        descWString* wSourceString=new descWString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePWString*)wSourceString;
        break;

    }
    case cst_messagelen+1:
    {
        messageWString* wSourceString=new messageWString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePWString*)wSourceString;
        break;

    }
/*    case cst_urilen+1:
    {
        uriString* wSourceString=new uriString;
        wSourceString->_importURF(pURFData,pURFData->Size);
        wPSourceString=(templatePString*)wSourceString;
        break;
    }*/
    case cst_keywordlen+1:
    {
        keywordWString* wSourceString=new keywordWString;
        wSourceString->_importURF(pURFData);
        wPSourceString=(templatePWString*)wSourceString;
        break;
    }

    case cst_identitylen+1:
    {
        identityWString* wSourceString=new identityWString;
        wSourceString->_importURF(pURFData);
        wPSourceString==(templatePWString*)wSourceString;
        break;
    }
    default:
         {
        if (wPSourceString!=nullptr)
                            delete wPSourceString;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVSIZE,
                              Severity_Severe,
                              "Unknown source fixed string capacity of <%ld> : cannot convert from fixedCString to <%s>.",
                              pSourceNSize,
                              decode_ZType(pTargetType));
        return ZS_INVSIZE;
         }
    }// switch


  switch (pTargetType)
  {
   case ZType_FixedWString:
    {
      templatePWString* wTargetPWStr=static_cast<templatePWString*> (pValue);
      size_t wStrSize=wPSourceString->size();
      if (wStrSize>(wTargetPWStr->capacity()-1))
        {
          ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FIELDCAPAOVFLW,
                                Severity_Warning,
                                "Field type <%s> capacity <%ld> has been moved with <%ld> bytes and has been truncated to max capacity.",
                                decode_ZType(pTargetType),
                                wTargetPWStr->capacity(),
                                wStrSize);
          wSt=ZS_FIELDCAPAOVFLW;
          wStrSize=wTargetPWStr->capacity()-1;
        }
      wcsncpy(wTargetPWStr->content,wPSourceString->content,wStrSize);
      return (wSt);
    } // ZType_FixedWString

  case ZType_VaryingWString:
    {
      varyingWString* wTargetVStr=static_cast<varyingWString*> (pValue);
      wTargetVStr->fromWStringCount(wPSourceString->content,wPSourceString->size()+1);
      wTargetVStr->addConditionalTermination();
      return ZS_SUCCESS;
    } // ZType_VaryingWString

  default:
       {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVTYPE,
                            Severity_Severe,
                            "Incompatible source and target data type : cannot convert from <%s> to <%s>.",
                            decode_ZType(ZType_FixedCString),
                            decode_ZType(pTargetType));
      return ZS_INVTYPE;
       }
  }
}//_getFixedWStringNfU
/**
 * @brief _getVaryingCStringNfURF  URF type is varyingCString : target type might be
 *  varyingCString
 *  fixedCString
 *  varyingWString
 *  fixedWString
 *  array of char
 *  array of wchar_t
 *
 * @param pValue
 * @param pURFData
 * @param pTargetType
 * @param pSourceSize
 * @return
 */
ZStatus
_getVaryingCStringNfURF(void* pValue,
                        unsigned char* pURFData,
                        ZTypeBase pTargetType,
                        uint64_t pSourceSize)
{

    if (pTargetType==ZType_VaryingCString)
    {
        static_cast<varyingCString*>(pValue)->_importURF(pURFData);
        return ZS_SUCCESS;
    }
ZStatus wSt=ZS_SUCCESS;
uint64_t wTargetCapacity;

    switch (pTargetType)
    {
        case ZType_FixedCString:
        {

        templatePString* wTargetPString= static_cast<templatePString*>(pValue);
            wTargetCapacity=static_cast<templatePString*>(pValue)->capacity();
        switch (wTargetCapacity)
        {
        case cst_codelen+1:
        {
            codeString* wTargetString=static_cast<codeString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_desclen+1:
        {
            descString* wTargetString=static_cast<descString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_messagelen+1:
        {
            messageString* wTargetString=static_cast<messageString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_urilen+1:
        {
            uriString* wTargetString=static_cast<uriString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_keywordlen+1:
        {
            keywordString* wTargetString=static_cast<keywordString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }

        case cst_identitylen+1:
        {
            identityString* wTargetString=static_cast<identityString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        default:
             {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVSIZE,
                                  Severity_Severe,
                                  "Unknown target fixed string capacity of <%ld> : cannot convert from varyingCString to <%s>.",
                                  wTargetCapacity,
                                  decode_ZType(pTargetType));
            return ZS_INVSIZE;
             }
        }// switch wTargetCapacity
    }//case Type_FixedCString

    case ZType_FixedWString:
    {
    varyingCString* wPSourceString=new varyingCString;
    wPSourceString->_importURF(pURFData);
    templatePWString* wTargetPString= static_cast<templatePWString*>(pValue);
    wTargetCapacity=static_cast<templatePString*>(pValue)->capacity();
    switch (wTargetCapacity)
    {
    case cst_codelen+1:
    {
        codeWString* wTargetString=static_cast<codeWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    case cst_desclen+1:
    {
        descWString* wTargetString=static_cast<descWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    case cst_messagelen+1:
    {
        messageWString* wTargetString=static_cast<messageWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }

    case cst_keywordlen+1:
    {
        keywordWString* wTargetString=static_cast<keywordWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }

    case cst_identitylen+1:
    {
        identityWString* wTargetString=static_cast<identityWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    default:
         {
        if (wPSourceString!=nullptr)
                            delete wPSourceString;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVSIZE,
                              Severity_Severe,
                              "Unknown target fixed string capacity of <%ld> : cannot convert from varyingCString to <%s>.",
                              wTargetCapacity,
                              decode_ZType(pTargetType));
        return ZS_INVSIZE;
         }
    }// switch wTargetCapacity
    }//case Type_FixedWString

    case ZType_VaryingWString:
        {
        varyingCString* wPSourceString=new varyingCString;
        wPSourceString->_importURF(pURFData);
        varyingWString* wTargetPString= static_cast<varyingWString*>(pValue);
        wTargetPString->fromVaryingCString(*wPSourceString);
        delete wPSourceString;
        return ZS_SUCCESS;
        }//case Type_VaryingWString

 default:
       {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVTYPE,
                            Severity_Severe,
                            "Incompatible source and target data type : cannot convert from <%s> to <%s>.",
                            decode_ZType(ZType_VaryingCString),
                            decode_ZType(pTargetType));
      return ZS_INVTYPE;
       }
  }// main switch (pTargetType)
}//ZType_VaryingCString


ZStatus
_getVaryingWStringNfURF(void* pValue,
                        unsigned char* pURFData,
                        ZTypeBase pTargetType,
                        uint64_t pSourceSize)
{

    if (pTargetType==ZType_VaryingWString)
        {
        static_cast<varyingWString*>(pValue)->_importURF(pURFData);
        return ZS_SUCCESS;
        }
ZStatus wSt=ZS_SUCCESS;
uint64_t wTargetCapacity;

    switch (pTargetType)
    {
        case ZType_FixedWString:
        {

        templatePWString* wTargetPString= static_cast<templatePWString*>(pValue);
            wTargetCapacity=static_cast<templatePWString*>(pValue)->capacity();
        switch (wTargetCapacity)
        {
        case cst_codelen+1:
        {
            codeWString* wTargetString=static_cast<codeWString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_desclen+1:
        {
            descWString* wTargetString=static_cast<descWString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_messagelen+1:
        {
            messageWString* wTargetString=static_cast<messageWString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
/*        case cst_urilen+1:
        {
            uriString* wTargetString=static_cast<uriString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }*/
        case cst_keywordlen+1:
        {
            keywordWString* wTargetString=static_cast<keywordWString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }

        case cst_identitylen+1:
        {
            identityWString* wTargetString=static_cast<identityWString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        default:
             {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVSIZE,
                                  Severity_Severe,
                                  "Unknown target fixed string capacity of <%ld> : cannot convert from varyingWString to <%s>.",
                                  wTargetCapacity,
                                  decode_ZType(pTargetType));
            return ZS_INVSIZE;
             }
        }// switch wTargetCapacity
    }//case Type_FixedWString

    case ZType_FixedCString:
    {
    varyingWString* wPSourceString=new varyingWString;
    wPSourceString->_importURF(pURFData);
    templatePString* wTargetPString= static_cast<templatePString*>(pValue);
    wTargetCapacity=static_cast<templatePString*>(pValue)->capacity();
    switch (wTargetCapacity)
    {
    case cst_codelen+1:
    {
        codeString* wTargetString=static_cast<codeString*>(pValue);
        wTargetString->fromWString_PtrCount(wPSourceString->toWString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    case cst_desclen+1:
    {
        descString* wTargetString=static_cast<descString*>(pValue);
        wTargetString->fromWString_PtrCount(wPSourceString->toWString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    case cst_messagelen+1:
    {
        messageString* wTargetString=static_cast<messageString*>(pValue);
        wTargetString->fromWString_PtrCount(wPSourceString->toWString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }

    case cst_keywordlen+1:
    {
        keywordString* wTargetString=static_cast<keywordString*>(pValue);
        wTargetString->fromWString_PtrCount(wPSourceString->toWString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }

    case cst_identitylen+1:
    {
        identityString* wTargetString=static_cast<identityString*>(pValue);
        wTargetString->fromWString_PtrCount(wPSourceString->toWString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    default:
         {
        if (wPSourceString!=nullptr)
                            delete wPSourceString;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVSIZE,
                              Severity_Severe,
                              "Unknown target fixed string capacity of <%ld> : cannot convert from varyingWString to <%s>.",
                              wTargetCapacity,
                              decode_ZType(pTargetType));
        return ZS_INVSIZE;
         }
    }// switch wTargetCapacity
    }//case Type_FixedWString

    case ZType_VaryingCString:
        {
        varyingWString* wPSourceString=new varyingWString;
        wPSourceString->_importURF(pURFData);
        varyingCString* wTargetPString= static_cast<varyingCString*>(pValue);
        wTargetPString->fromVaryingWString(*wPSourceString);
        delete wPSourceString;
        }//case Type_VaryingWString

 default:
       {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVTYPE,
                            Severity_Severe,
                            "Incompatible source and target data type : cannot convert from <%s> to <%s>.",
                            decode_ZType(ZType_VaryingWString),
                            decode_ZType(pTargetType));
      return ZS_INVTYPE;
       }
  }// main switch (pTargetType)
}//_getVaryingWStringNfURF


ZStatus
_getArrayNfURF(void* pValue,
             unsigned char* pURFData,
             ZTypeBase pTargetType,
             uint64_t pTargetNSize,
             uint32_t pTargetArrayCount)
{

/*   if (pTargetType==ZType_VaryingCString)
    {
        static_cast<varyingCString*>(pValue)->_importURF(pURFData);
        return ZS_SUCCESS;
    }
  */
    if (pTargetType&ZType_String)
    {
        return static_cast<utfStringHeader*>(pValue)->_importURF(pURFData);
    }
ZStatus wSt=ZS_SUCCESS;
uint64_t wTargetCapacity;

    switch (pTargetType)
    {
        case ZType_FixedCString:
        {

 //       templatePString* wTargetPString= static_cast<templatePString*>(pValue);
        wTargetCapacity=static_cast<templatePString*>(pValue)->capacity();
        switch (wTargetCapacity)
        {
        case cst_codelen+1:
        {
            codeString* wTargetString=static_cast<codeString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_desclen+1:
        {
            descString* wTargetString=static_cast<descString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_messagelen+1:
        {
            messageString* wTargetString=static_cast<messageString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_urilen+1:
        {
            uriString* wTargetString=static_cast<uriString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        case cst_keywordlen+1:
        {
            keywordString* wTargetString=static_cast<keywordString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }

        case cst_identitylen+1:
        {
            identityString* wTargetString=static_cast<identityString*>(pValue);
            wTargetString->_importURF(pURFData);
            return ZS_SUCCESS;
        }
        default:
             {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVSIZE,
                                  Severity_Severe,
                                  "Unknown target fixed string capacity of <%ld> : cannot convert from varyingCString to <%s>.",
                                  wTargetCapacity,
                                  decode_ZType(pTargetType));
            return ZS_INVSIZE;
             }
        }// switch wTargetCapacity
    }//case Type_FixedCString

    case ZType_FixedWString:
    {
    varyingCString* wPSourceString=new varyingCString;
    wPSourceString->_importURF(pURFData);
    templatePWString* wTargetPString= static_cast<templatePWString*>(pValue);
    wTargetCapacity=static_cast<templatePString*>(pValue)->capacity();
    switch (wTargetCapacity)
    {
    case cst_codelen+1:
    {
        codeWString* wTargetString=static_cast<codeWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    case cst_desclen+1:
    {
        descWString* wTargetString=static_cast<descWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    case cst_messagelen+1:
    {
        messageWString* wTargetString=static_cast<messageWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }

    case cst_keywordlen+1:
    {
        keywordWString* wTargetString=static_cast<keywordWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }

    case cst_identitylen+1:
    {
        identityWString* wTargetString=static_cast<identityWString*>(pValue);
        wTargetString->fromCString_PtrCount(wPSourceString->toString(),wPSourceString->size());
        delete wPSourceString;
        return ZS_SUCCESS;
    }
    default:
         {
        if (wPSourceString!=nullptr)
                            delete wPSourceString;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVSIZE,
                              Severity_Severe,
                              "Unknown target fixed string capacity of <%ld> : cannot convert from varyingCString to <%s>.",
                              wTargetCapacity,
                              decode_ZType(pTargetType));
        return ZS_INVSIZE;
         }
    }// switch wTargetCapacity
    }//case Type_FixedWString

    case ZType_VaryingWString:
        {
        varyingCString* wPSourceString=new varyingCString;
        wPSourceString->_importURF(pURFData);
        varyingWString* wTargetPString= static_cast<varyingWString*>(pValue);
        wTargetPString->fromVaryingCString(*wPSourceString);
        delete wPSourceString;
        }//case Type_VaryingWString

 default:
       {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVTYPE,
                            Severity_Severe,
                            "Incompatible source and target data type : cannot convert from <%s> to <%s>.",
                            decode_ZType(ZType_VaryingCString),
                            decode_ZType(pTargetType));
      return ZS_INVTYPE;
       }
  }// main switch (pTargetType)
}//ZType_Array
#endif // __DEPRECATED_FIELD_CONVERSION__


#endif // ZNATURALFROMURF_CPP
