#ifndef ZDATATYPE_CPP
#define ZDATATYPE_CPP

#include <zindexedfile/zsindextype.h>
#include <zindexedfile/zsindexfile.h>

inline
size_t getAtomicUniversalSize(const ZTypeBase pType)
{
ZTypeBase wZType = pType&ZType_AtomicMask;

    switch (wZType)
    {
       case ZType_Char:    // char MUST not be converted with leading sign byte : this is a C string.
       case ZType_UChar:
       case ZType_U8 :
                {
                return sizeof(uint8_t);
                }
       case ZType_S8 :
               {
                return sizeof(int8_t)+1;
               }

        case ZType_U16 :
                 {
                 return sizeof(uint16_t);
                 }
        case ZType_S16 :
                {
                return sizeof(int16_t)+1;
                }
        case ZType_U32 :
                 {
                return sizeof(uint32_t);
                 }
        case ZType_S32 :
                {
                return sizeof(int32_t)+1;
                }
        case ZType_U64 :
                 {
                return sizeof(uint64_t);
                 }
        case ZType_S64 :
                {
                return sizeof(int64_t)+1;
                }
       case ZType_Float :
                    {
                return sizeof(float)+1;
                    }
        case ZType_Double :
                     {
                return sizeof(double)+1;
                     }
        case ZType_LDouble :
                     {
                return sizeof(long double)+1;
                     }

           default:
                    {
                        ZException.setMessage(_GET_FUNCTION_NAME_,
                                                ZS_INVTYPE,
                                                Severity_Fatal,
                                                "Invalid atomic ZType <%lX> <%s> encountered while processing record data",
                                               pType,
                                                decode_ZType(pType));
                        ZException.exit_abort();
                    }

       }//switch

}//getUniversalSize
/**
 * @brief getAtomicNaturalSize returns the Atomic natural size of the type.
 * If array, only the element atomic size is returned.
 * @param pType
 * @return
 */
inline
size_t getAtomicNaturalSize(const ZTypeBase pType)
{
ZTypeBase wZType = pType;
    wZType &= ZType_AtomicMask;
    switch (wZType)
    {
       case ZType_Char:    // char MUST not be converted with leading sign byte : this is a C string.
       case ZType_UChar:
       case ZType_U8 :
                {
                return sizeof(uint8_t);
                }
       case ZType_S8 :
               {
                return sizeof(int8_t);
               }

        case ZType_U16 :
                 {
                 return sizeof(uint16_t);
                 }
        case ZType_S16 :
                {
                return sizeof(int16_t);
                }
        case ZType_U32 :
                 {
                return sizeof(uint32_t);
                 }
        case ZType_S32 :
                {
                return sizeof(int32_t);
                }
        case ZType_U64 :
                 {
                return sizeof(uint64_t);
                 }
        case ZType_S64 :
                {
                return sizeof(int64_t);
                }
       case ZType_Float :
                    {
                return sizeof(float);
                    }
        case ZType_Double :
                     {
                return sizeof(double);
                     }
        case ZType_LDouble :
                     {
                return sizeof(long double);
                     }

           default:
                    {
                        ZException.setMessage(_GET_FUNCTION_NAME_,
                                                ZS_INVTYPE,
                                                Severity_Fatal,
                                                "Invalid atomic ZType <%lX> <%s> encountered while processing record data",
                                               pType,
                                                decode_ZType(pType));
                        ZException.exit_abort();
                    }

       }//switch

}//getNaturalSize


//



//------------------Extract Key field from record------------------------------------------------------------
/**
 * @brief _getValueFromStdString takes data from a pointer to a std::string and returns its content into a ZDataBuffer
 * @param pIndata
 * @param pOutData
 */
void
_getValueFromStdString (void *pIndata, ZDataBuffer &pOutData)
{
std::string*wString=    static_cast<std::string*>(pIndata) ;

    pOutData.setData((char*)wString->c_str(),wString->size());
    return;
}

template  <class _Tp>
ZStatus _getByteSequenceFromRecord (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp>  pInData, const ssize_t pSize, ZDataBuffer &pOutData, ZSIndexField& pField)
{
    pOutData.setData(pInData,pSize);
    return ZS_SUCCESS;
}

/**
 * @brief _getArrayUfN Converts an array of Natural atomic values to  a ZDataBuffer containing a Universal

Uses _getArrayUfN template routine to pack field according system constraints (big / little endian) and leading sign byte.

Size of the returned ZDataBuffer content is

- size of Atomic data + 1 byte (leading sign byte) if signed value type,
- size of Atomic data if unsigned value type.

 * @param pInData   ZDataBuffer containing the user record to extract field from.
 * @param pOutData  ZDataBuffer containing as a return the packed/extracted field value converted as ready to be used as a key field
 * @param pType     ZType_type of the data to convert
 * @param pArrayCount      Optional output argument : computed number of array ranks
 * @param pUniversalSize  Optional output argument : universal data format size
 * @return          A ZStatus
 */
template <class _Tp>
static inline
ZStatus _getArrayUfN_T_Ptr(unsigned char* pInData_Ptr,
                           ZDataBuffer &pOutData,
                           const ZType_type pType,
                           const URF_Array_Count_type pArrayCount)  //! mandatory input
{
 ZTypeBase wZType = pType;

 if (!(wZType & ZType_Array))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Severe,
                                 "Invalid ZType <%lX> <%s> encountered while processing record data. Type is NOT ARRAY.",
                                 pType,
                                 decode_ZType(pType));
         return ZS_INVTYPE;
        }
 wZType = wZType & ZType_AtomicMask ;  // negate ZType_Array : get the atomic data type



 long wEltOffsetIn=0;
 size_t wEltNSize = getAtomicNaturalSize(pType);
 ZDataBuffer wDBElt;

    pOutData.clear();
    for (long wi=0;wi <  (long)pArrayCount ;wi++)
        {
        _getAtomicUfN_T<_Tp>(pInData_Ptr+ wEltOffsetIn,wDBElt,pType) ;
        pOutData.appendData(wDBElt);
        wEltOffsetIn+=wEltNSize;
        }
    return ZS_SUCCESS;
}//_getArrayUfN_T_Ptr

template <class _Tp>
static inline
ZStatus _getArrayUfN_T(ZDataBuffer &pInData,
                       ZDataBuffer &pOutData,
                       const ZType_type pType,
                       URF_Array_Count_type &pArrayCount)
{

    pArrayCount  = pInData.Size/getAtomicNaturalSize((ZType_type)pType);
    return _getArrayUfN_T_Ptr<_Tp>(pInData.Data,pOutData,pType,pArrayCount);
}
template <class _Tp>
static inline
ZStatus _getByteSeqUfN_T_Ptr(unsigned char* pInData,
                         ZDataBuffer     &pOutData,
                         const ZType_type pType,
                         URF_Varying_Size_type &pSize)
{
uint64_t wSize;

    if (pType & ZType_Blob)
    {
                ZDataBuffer*wZDB=(ZDataBuffer*)pInData;
                wSize = (URF_Varying_Size_type)wZDB->Size;
                pOutData.setData(wZDB->Data,wSize);
                return ZS_SUCCESS;
    }
    if (pType & ZType_bitset)
    {
                ZBitset*wZDB=(ZBitset*)pInData;
                wSize = (URF_Varying_Size_type)(wZDB->Size*wZDB->UnitByteSize);
                pOutData.setData(wZDB->bit,wSize);
                return ZS_SUCCESS;
    }
    return ZS_INVTYPE;
} // _getByteSeqUfN_T_Ptr
/*
template <class _Tp>
static inline
ZStatus _getArrayUfN_A(auto &pInData,
                     ZDataBuffer &pOutData,
                     const ZType_type pType,
                     URF_Array_Count_type &pArrayCount,
                     long &pUniversalSize)
{
ZType_type wType;
ssize_t wNaturalSize,wUniversalSize;
ZStatus wSt=zgetZType_T<decltype(pInData)>(wType,wNaturalSize,wUniversalSize,pArrayCount,false);
    if (wSt!=ZS_SUCCESS)
            return wSt;
    return _getArrayUfN_T_Ptr<_Tp>((unsigned char*)&pInData,pOutData,pType,pArrayCount);
}
*/
/**
 * @brief _getAtomicUfN_A
 * @param pInData
 * @param pOutData
 * @param pType
 * @param pArrayCount
 * @param pUniversalSize
 * @return
 */
/*
ZStatus _getAtomicUfN_A (auto pInData,
                        ZDataBuffer *pOutData)
{
ZType_type wType;
ssize_t wNaturalSize, wUniversalSize;
uint16_t wArrayCount;
ZStatus wSt = zgetZType_T<decltype(pInData)>(wType,wNaturalSize,wUniversalSize,wArrayCount,true);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    ZDataBuffer wDB;
    wDB.setData(pInData,sizeof(pInData));

    return _getAtomicUfN_T<decltype(pInData)>(wDB.Data,pOutData,wType);
}//_getAtomicUfN_A
*/
/**
 * @brief _getArrayUfN_A Converts an array to Universal data format from Natural data
 * @param pInData
 * @param pOutData
 * @param pType
 * @param pArrayCount
 * @param pUniversalSize
 * @return
 */
/*
ZStatus _getArrayUfN_A (auto pInData,
                        ZDataBuffer *pOutData,
                        const ZType_type pType,
                        URF_Array_Count_type &pArrayCount)
{
    ZStatus wSt;
    ssize_t wNSize,wUSize,wArrayCount;
    ZDataBuffer wDB;
    wDB.setData(pInData,sizeof(pInData));
    ZType_type wType;
    if (wSt=zgetZType_T<decltype(pInData)>(wType,wNSize,wUSize,wArrayCount)!= ZS_SUCCESS)
                                                            return wSt;
    return _getArrayUfN_T<decltype(pInData)>(pInData,pOutData,wType,pArrayCount);
}
*/
/**
 * @brief _getAtomicUfN Obtains a ZDataBuffer content with a Universal format from a ZDataBuffer with Natural atomic value

  Uses _getAtomicUfN_T template routine to pack field according system constraints (big / little endian) and leading sign byte.

  - Size of the returned ZDataBuffer content is
      + size of Atomic data + 1 byte (leading sign byte) if signed value type,
      + size of Atomic data if unsigned value type.

  - Byte order is reversed if system is little Endian

  ZIndexField_struct details reminder
  - ssize_t     Offset;         < Offset of the Field from the beginning of record
  - ssize_t     NaturalSize;    < Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
  - ssize_t     KeyFieldSize;   < length of the field when stored into Key (Field dictionary internal format size)
  - long        ZType;          < Type mask of the Field @see ZType_type
  - URF_Array_Count_type        ArraySize;      < in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.

 * @param[in] pInData   ZDataBuffer containing the user record to extract field from.
 * @param[out] pOutData  ZDataBuffer containing as a return the packed/extracted field value converted as ready to be used as a key field
 * @param[in] pField    ZIndex Key dictionary rank (ZIndexField_struct) describing the data field to extract.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus _getAtomicUfN(ZDataBuffer &pInData,ZDataBuffer &pOutData,const ZType_type pType)
{

 long wZType = pType;

 if (!(wZType & ZType_Atomic))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%lX> <%s> encountered while processing record data. Type is NOT ATOMIC.",
                                 pType,
                                 decode_ZType(pType));
         return ZS_INVTYPE;
        }
 wZType = wZType &ZType_AtomicMask ;  // Keep only Atomic flags

 switch (wZType)
 {
case ZType_U8 :
         {
          _getAtomicUfN_T<uint8_t>(pInData.Data,pOutData,pType);
          return ZS_SUCCESS;
         }
case ZType_S8 :
        {
        _getAtomicUfN_T<int8_t>(pInData.Data,pOutData,pType);
        return ZS_SUCCESS;
        }

 case ZType_U16 :
          {
          _getAtomicUfN_T<uint16_t>(pInData.Data,pOutData,pType);
          return ZS_SUCCESS;
          }
 case ZType_S16 :
         {
         _getAtomicUfN_T<int16_t>(pInData.Data,pOutData,pType);;
         return ZS_SUCCESS;
         }
 case ZType_U32 :
          {
           _getAtomicUfN_T<uint32_t>(pInData.Data,pOutData,pType);
           return ZS_SUCCESS;
          }
 case ZType_S32 :
         {
         _getAtomicUfN_T<int32_t>(pInData.Data,pOutData,pType);
         return ZS_SUCCESS;
         }
 case ZType_U64 :
          {
          _getAtomicUfN_T<uint64_t>(pInData.Data,pOutData,pType);
          return ZS_SUCCESS;
          }
 case ZType_S64 :
         {
         _getAtomicUfN_T<int64_t>(pInData.Data,pOutData,pType);
         return ZS_SUCCESS;
         }
case ZType_Float :
             {
              _getAtomicUfN_T<float>(pInData.Data,pOutData,pType);
              return ZS_SUCCESS;
             }
 case ZType_Double :
              {
              _getAtomicUfN_T<double>(pInData.Data,pOutData,pType);
              return ZS_SUCCESS;
              }
 case ZType_LDouble :
              {
               _getAtomicUfN_T<long double>(pInData.Data,pOutData,pType);
               return ZS_SUCCESS;
              }

    default:
             {
                 ZException.setMessage(_GET_FUNCTION_NAME_,
                                         ZS_INVTYPE,
                                         Severity_Fatal,
                                         "Invalid ZType <%lX> <%s> encountered while processing record data",
                                         pType,
                                         decode_ZType(pType));
                 return ZS_INVTYPE;
             }

 }//switch

}//_getAtomicUfN


//------------------------------- Conversion back from key to natural value--------------------------------------------------------------


/**
 * @brief _getAtomicNfU Obtains a single Atomic field value from a Universal content (pUniversal) .
 *
 * @param[out] pUniversal raw key content of the universal formatted value (excepted address)
 * @param[in] pNatural a ZDataBuffer containing field value in natural format ready to be used by application program
 * @param[in] pType ZType_type of the field to be converted to natural value
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorSError
 */

ZStatus _getAtomicNfU(ZDataBuffer &pUniversal, ZDataBuffer &pNatural,const ZType_type pType)
{

    ZTypeBase wZType = pType;

    if (!(wZType & ZType_Atomic))
           {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVTYPE,
                                    Severity_Fatal,
                                    "Invalid ZType <%ld> <%s> encountered while processing universal data. Type is NOT ATOMIC.",
                                    pType,
                                    decode_ZType(pType));
            return ZS_INVTYPE;
           }
    wZType = wZType &ZType_AtomicMask ;  // Keep only Atomic flags

 switch (wZType)
 {
case ZType_U8 :
         {

         uint8_t wValue= _getAtomicNfU_T_Ptr<uint8_t>(wValue,pUniversal.Data,wZType);
         pNatural.setData( &wValue, sizeof(wValue));
         break;
         }
case ZType_S8 :
        {
         int8_t wValue= _getAtomicNfU_T_Ptr<int8_t>(wValue,pUniversal.Data,wZType);
        pNatural.setData( &wValue, sizeof(wValue));
         break;
        }

 case ZType_U16 :
          {
          uint16_t wValue= _getAtomicNfU_T_Ptr<uint16_t>(wValue,pUniversal.Data,wZType);
          pNatural.setData( &wValue, sizeof(wValue));
          break;
          }
 case ZType_S16 :
         {
         int16_t wValue= _getAtomicNfU_T_Ptr<int16_t>(wValue,pUniversal.Data,wZType);
         pNatural.setData( &wValue, sizeof(wValue));
         break;
         }
 case ZType_U32 :
          {
         uint32_t wValue= _getAtomicNfU_T_Ptr<uint32_t>(wValue,pUniversal.Data,wZType);
         pNatural.setData( &wValue, sizeof(wValue));
         break;
          }
 case ZType_S32 :
         {
         int32_t wValue= _getAtomicNfU_T_Ptr<int32_t>(wValue,pUniversal.Data,wZType);
         pNatural.setData( &wValue, sizeof(wValue));
         break;
         }
 case ZType_U64 :
          {
         uint64_t wValue= _getAtomicNfU_T_Ptr<uint64_t>(wValue,pUniversal.Data,wZType);
         pNatural.setData( &wValue, sizeof(wValue));
         break;
          }
 case ZType_S64 :
         {
         int64_t wValue= _getAtomicNfU_T_Ptr<int64_t>(wValue,pUniversal.Data,wZType);
         pNatural.setData( &wValue, sizeof(wValue));
         break;
         }
case ZType_Float :
             {
             float wValue= _getAtomicNfU_T_Ptr<float>(wValue,pUniversal.Data,wZType);
             pNatural.setData( &wValue, sizeof(wValue));
             break;
             }
 case ZType_Double :
              {
             double wValue= _getAtomicNfU_T_Ptr<double>(wValue,pUniversal.Data,wZType);
             pNatural.setData( &wValue, sizeof(wValue));
             break;
              }
 case ZType_LDouble :
              {
             long double wValue= _getAtomicNfU_T_Ptr<long double>(wValue,pUniversal.Data,wZType);
             pNatural.setData( &wValue, sizeof(wValue));
             break;
              }

    default:
             {
//     auto wValue= _getAtomicNfU_T<int8_t>(pKeyData,WZType);
//     AVFKValue.setData( &wValue, sizeof(wValue));
             pNatural = "####" ;
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                     ZS_INVTYPE,
                                     Severity_Fatal,
                                     "Invalid ZType <%ld> <%s> encountered while processing key data",
                                     pType,
                                     decode_ZType(pType));
             return ZS_INVTYPE;
             }

 }//switch
    return ZS_SUCCESS;
}//_getAtomicNfU
/**
 * @brief _getAtomicNfU_A gets the natural type value as pValue from its universal value format
 * @param pValue
 * @param pUniversal
 * @param pType
 * @return
 */
template <class _Tp>
ZStatus _getAtomicNfU_A(_Tp &pValue, ZDataBuffer &pUniversal,const ZType_type pType)
{
    ZTypeBase wZType = pType;

    if (!(wZType & ZType_Atomic))
           {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVTYPE,
                                    Severity_Fatal,
                                    "Invalid ZType <%ld> <%s> encountered while processing universal data. Type is NOT ATOMIC.",
                                    pType,
                                    decode_ZType(pType));
            return ZS_INVTYPE;
           }
    wZType = wZType &ZType_AtomicMask ;  // Keep only Atomic flags

 switch (wZType)
 {
case ZType_U8 :
         {

         pValue= _getAtomicNfU_T_Ptr<uint8_t>(pValue,pUniversal.Data,wZType);
         break;
         }
case ZType_S8 :
        {
         pValue= _getAtomicNfU_T_Ptr<int8_t>(pValue,pUniversal.Data,wZType);
         break;
        }

 case ZType_U16 :
          {
          pValue= _getAtomicNfU_T_Ptr<uint16_t>(pValue,pUniversal.Data,wZType);
          break;
          }
 case ZType_S16 :
         {
         pValue= _getAtomicNfU_T_Ptr<int16_t>(pValue,pUniversal.Data,wZType);
         break;
         }
 case ZType_U32 :
          {
         pValue= _getAtomicNfU_T_Ptr<uint32_t>(pValue,pUniversal.Data,wZType);
         break;
          }
 case ZType_S32 :
         {
         pValue= _getAtomicNfU_T_Ptr<int32_t>(pValue,pUniversal.Data,wZType);
         break;
         }
 case ZType_U64 :
          {
         pValue= _getAtomicNfU_T_Ptr<uint64_t>(pValue,pUniversal.Data,wZType);
         break;
          }
 case ZType_S64 :
         {
         pValue= _getAtomicNfU_T_Ptr<int64_t>(pValue,pUniversal.Data,wZType);
         break;
         }
case ZType_Float :
             {
             pValue= _getAtomicNfU_T_Ptr<float>(pValue,pUniversal.Data,wZType);
             break;
             }
 case ZType_Double :
              {
             pValue= _getAtomicNfU_T_Ptr<double>(pValue,pUniversal.Data,wZType);
             break;
              }
 case ZType_LDouble :
              {
             pValue= _getAtomicNfU_T_Ptr<long double>(pValue,pUniversal.Data,wZType);
             break;
              }

    default:
             {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                     ZS_INVTYPE,
                                     Severity_Fatal,
                                     "Invalid ZType <%ld> <%s> encountered while processing key data",
                                     pType,
                                     decode_ZType(pType));
             return ZS_INVTYPE;
             }

 }//switch
    return ZS_SUCCESS;
}//_getAtomicNfU_A

/**
 * @brief _getArrayUfN Template: converts an array with Natural values into Universal values
 *
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] AVFKValue field value in natural format ready to be used by application program
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pFieldList Index dictionary as a CZKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorSError
 */
ZStatus _getArrayUfN(ZDataBuffer &pInData,
                     ZDataBuffer &pOutData,
                     const ZType_type pType,
                     URF_Array_Count_type &pArrayCount,
                     long *pUniversalSize)
{

    ZTypeBase wZType = pType;
     wZType = wZType & ZType_AtomicMask ;  // negate ZType_Array : get the atomic data type


ZDataBuffer wDBIn;

//    wDBIn.setData((pInData.Data+wOffset),pFieldList[pRank].InternalSize);


     if (wZType&ZType_Char)
            {
            pOutData.setData(wDBIn);
            return ZS_SUCCESS;
            }
     switch (wZType)
     {
        case ZType_Char:    // char MUST not be converted with leading sign byte : this is a C string.
        case ZType_UChar:
        case ZType_U8 :
                 {
                 return _getArrayUfN_T<uint8_t>(pInData,pOutData,pType,pArrayCount);
                 }
        case ZType_S8 :
                {
                return _getArrayUfN_T<int8_t>(pInData,pOutData,pType,pArrayCount);
                }
         case ZType_WChar :
         case ZType_U16 :
                  {
                  return _getArrayUfN_T<uint16_t>(pInData,pOutData,pType,pArrayCount);
                  }
         case ZType_S16 :
                 {
                return _getArrayUfN_T<int16_t>(pInData,pOutData,pType,pArrayCount);
                 }
         case ZType_U32 :
                  {
                return _getArrayUfN_T<uint32_t>(pInData,pOutData,pType,pArrayCount);
                  }
         case ZType_S32 :
                 {
                return _getArrayUfN_T<int32_t>(pInData,pOutData,pType,pArrayCount);
                 }
         case ZType_U64 :
                  {
                return _getArrayUfN_T<uint64_t>(pInData,pOutData,pType,pArrayCount);
                  }
         case ZType_S64 :
                 {
                return _getArrayUfN_T<int64_t>(pInData,pOutData,pType,pArrayCount);
                 }
        case ZType_Float :
                     {
         return _getArrayUfN_T<float>(pInData,pOutData,pType,pArrayCount);
                     }
         case ZType_Double :
                      {
         return _getArrayUfN_T<double>(pInData,pOutData,pType,pArrayCount);
                      }
         case ZType_LDouble :
                      {
                return _getArrayUfN_T<long double>(pInData,pOutData,pType,pArrayCount);
                      }

            default:
                     {
                         ZException.setMessage(_GET_FUNCTION_NAME_,
                                                 ZS_INVTYPE,
                                                 Severity_Fatal,
                                                 "Invalid ZType <%ld> <%s> encountered while processing natural data",
                                                 pType,
                                                 decode_ZType(pType));
                         return ZS_INVTYPE;
                     }

        }//switch
 return ZS_SUCCESS;
}//_getArrayUfN

#ifdef __COMMENT__
/**
 * @brief _getArrayUfN Template: converts an array with Natural values into Universal values
 *
 * @param[in] pKeyData raw key content of the key value (excepted address)
 * @param[out] AVFKValue field value in natural format ready to be used by application program
 * @param[in] pRank      field order (rank) in the index dictionary
 * @param[in] pFieldList Index dictionary as a CZKeyDictionary object. It is necessary to have here the whole dictionary and not only field definition.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorSError
 */
ZStatus _getArrayUfN(ZDataBuffer &pInData,ZDataBuffer &pOutData,const long pRank,ZKeyDictionary & pFieldList)
{

 long wZType = pFieldList[pRank].ZType;

    if (!(wZType & ZType_Array))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Fatal,
                                 "Invalid ZType <%lX> <%s> encountered while processing record data. Type is NOT ARRAY.",
                                 pFieldList[pRank].ZType,
                                 decode_ZType(pFieldList[pRank].ZType));
         return ZS_INVTYPE;
        }
    wZType = wZType & ZType_AtomicMask ;  // negate ZType_Array : get the atomic data type

// long wEltOffsetKey=pFieldList.fieldKeyOffset(pRank);
 long wEltOffset= 0;

ZDataBuffer wDBIn;

ssize_t KeyDataSize = (ssize_t)((float)pFieldList[pRank].UniversalSize / (float)pFieldList[pRank].ArraySize) ; //! compute data size in key format

    ssize_t wOffset = 0 ;
    wDBIn.setData((pInData.Data+wOffset),pFieldList[pRank].UniversalSize);


     if (wZType&ZType_Char)
            {
            pOutData.setData(wDBIn);
            return ZS_SUCCESS;
            }
     switch (wZType)
     {
        case ZType_Char:    // char MUST not be converted with leading sign byte : this is a C string.
        case ZType_UChar:
        case ZType_U8 :
                 {
                 uint8_t wValue = _getArrayUfN_T<uint8_t>(wDBIn,wi,WZType);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffset += KeyDataSize;
                 break;
                 }
        case ZType_S8 :
                {
                 int8_t wValue = _getArrayUfN_T<int8_t>(wDBIn,wi,WZType);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffset += KeyDataSize;
                 break;
                }

         case ZType_U16 :
                  {
                 uint16_t wValue = _getValueAAFK<uint16_t>(wDBIn,wi,WZType);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffset += KeyDataSize;
                 break;
                  }
         case ZType_S16 :
                 {
                 int16_t wValue = _getValueAAFK<int16_t>(wDBIn,wi,WZType);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffset += KeyDataSize;
                 break;
                 }
         case ZType_U32 :
                  {
                 uint32_t wValue = _getValueAAFK<uint32_t>(wDBIn,wi,WZType);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffset += KeyDataSize;
                 break;
                  }
         case ZType_S32 :
                 {
                 int32_t wValue = _getValueAAFK<int32_t>(wDBIn,wi,WZType);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffset += KeyDataSize;
                 break;
                 }
         case ZType_U64 :
                  {
                 uint64_t wValue = _getValueAAFK<uint64_t>(wDBIn,wi,WZType);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffset += KeyDataSize;
                 break;
                  }
         case ZType_S64 :
                 {
                 int64_t wValue = _getValueAAFK<int64_t>(wDBIn,wi,WZType);
                 pOutData.appendData(&wValue,sizeof(wValue));
                 wEltOffset += KeyDataSize;
                 break;
                 }
        case ZType_Float :
                     {
                     float wValue = _getValueAAFK<float>(wDBIn,wi,WZType);
                     pOutData.appendData(&wValue,sizeof(wValue));
                     wEltOffset += KeyDataSize;
                     break;
                     }
         case ZType_Double :
                      {
                     double wValue = _getValueAAFK<double>(wDBIn,wi,WZType);
                     pOutData.appendData(&wValue,sizeof(wValue));
                     wEltOffset += KeyDataSize;
                     break;
                      }
         case ZType_LDouble :
                      {
                     long double wValue = _getValueAAFK<long double>(wDBIn,wi,WZType);
                     pOutData.appendData(&wValue,sizeof(wValue));
                     wEltOffset += KeyDataSize;
                     break;
                      }

            default:
                     {
                         ZException.setMessage(_GET_FUNCTION_NAME_,
                                                 ZS_INVTYPE,
                                                 Severity_Fatal,
                                                 "Invalid ZType <%lX> <%s> encountered while processing record data",
                                                 pFieldList[pRank].ZType,
                                                 decode_ZType(pFieldList[pRank].ZType));
                         return ZS_INVTYPE;
                     }

        }//switch
        }// for
 return ZS_SUCCESS;
}//_getArrayFromKey


ZStatus zgetUfN_A(auto pNatural,ZDataBuffer &pUniversal,ZType_type& pType,long &pArrayCount)
{
ZStatus wSt;
ssize_t wNaturalSize, wUniversalSize;
ZTypeBase wType ;
    wSt= zgetZType_T<decltype(pNatural)>(pType,wNaturalSize,wUniversalSize,pArrayCount,true);

    wType= pType&ZType_StructureMask;
    switch (wType)
    {
    case ZType_Atomic:
        return _getAtomicUfN_T<decltype(pNatural)>(pNatural,pUniversal,pType);
    case ZType_Array:
        return _getArrayUfN_T<decltype(pNatural)>(pNatural,pUniversal,pType,pArrayCount);
 //   case ZType_ByteSeq:
 //       return _getByteSeqUfN_T<decltype(pNatural)>(pNatural,pUniversal,pType,pSize);

    }
    return ZS_INVTYPE;
}




static inline
ZDataBuffer&
_convertAtomicEdian(ZDataBuffer& pData,auto &pValue, ZIndexField_struct & pField)
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
_convertAtomicNOEndian(ZDataBuffer& pData,auto pValue, ZIndexField_struct & pField)
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
_convert(typename std::enable_if<std::is_integral<_Tp>::value, _Tp>::type &pValue,ZIndexField_struct pField,ZDataBuffer &pData) {
  // an implementation for integral types (int, char, unsigned, etc.)
    if (is_little_endian()) // if system uses little endian integral internal representation
            {
            return _convertAtomicEdian(pData,pValue,pField);
            }
    return _convertAtomicNOEndian(pData,pValue,pField);  //! or not
}

template <typename _Tp> // no conversion for pointer
ZDataBuffer &
_convert(typename std::enable_if<std::is_pointer<_Tp>::value, _Tp>::type &pValue,ZIndexField_struct pField,ZDataBuffer &pData) {


}
template <typename _Tp>
ZDataBuffer &
_convert(typename std::enable_if<std::is_class<_Tp>::value, _Tp>::type &pValue,ZIndexField_struct pField,ZDataBuffer &pData) {

}
#endif // __COMMENT__

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
getAtomicZType_Sizes(int32_t pType,ssize_t& pNaturalSize,ssize_t& pUniversalSize) // not a template
{

    switch (pType)
    {
    case ZType_UChar :
    case ZType_Char : // array of char is a special case because no sign byte is added
    case ZType_U8 :
        {
        pNaturalSize=sizeof(uint8_t);
        pUniversalSize = pNaturalSize;
        return ZS_SUCCESS;
        }
    case ZType_S8 :
        {
        pNaturalSize=sizeof(uint8_t);
        pUniversalSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_S16 :
        {
        pNaturalSize=sizeof(int16_t);
        pUniversalSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_U16 :
        {
        pNaturalSize=sizeof(uint16_t);
        pUniversalSize = pNaturalSize;
        return ZS_SUCCESS;
        }
    case ZType_S32 :
        {
        pNaturalSize=sizeof(int32_t);
        pUniversalSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_U32 :
        {
        pNaturalSize=sizeof(uint32_t);
        pUniversalSize = pNaturalSize;
        return ZS_SUCCESS;
        }
    case ZType_S64 :
        {
        pNaturalSize=sizeof(int64_t);
        pUniversalSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_U64 :
        {
        pNaturalSize=sizeof(uint64_t);
        pUniversalSize = pNaturalSize;
        return ZS_SUCCESS;
        }
    case ZType_Float :
        {
        pNaturalSize=sizeof(float);
        pUniversalSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_Double :
        {
        pNaturalSize=sizeof(double);
        pUniversalSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    case ZType_LDouble :
        {
        pNaturalSize=sizeof(long double);
        pUniversalSize = pNaturalSize+1;
        return ZS_SUCCESS;
        }
    default:
        {
        pNaturalSize = -1;
        pUniversalSize = -1;
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
getAtomicZType(_Tp pValue,ZTypeBase& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize)
{
pType=ZType_Nothing;
const size_t pTypeHashCode = typeid(pValue).hash_code();
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


//--------------------Routines---------------------------------------------------



 // of no use for the moment : should be suppressed
/**
 * @brief testUniversalTypes test compatibility of input type (pTypeIn) vs desired field type (pDesiredType)
 * @param[in] pTypeIn
 * @param[in] pDesiredType
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
checkFieldsTypes (const ZTypeBase pTypeIn, const ZTypeBase pTargetType)
{
ZTypeBase wStructTypeIn, wAtomicTypeIn ;
ZTypeBase wTargetStructType , wTargetAtomicType ;

    wStructTypeIn = pTypeIn &ZType_StructureMask;
    wAtomicTypeIn = pTypeIn &ZType_AtomicMask;

    wTargetStructType = pTargetType &ZType_StructureMask;
    wTargetAtomicType = wTargetAtomicType &ZType_AtomicMask;

    if (wStructTypeIn & ZType_Pointer)  // if pointer : test compatibility of atomic data underneeth
            {
           if (!(wTargetAtomicType & ZType_Array))
                {
               ZException.setMessage(_GET_FUNCTION_NAME_,
                                       ZS_INVTYPE,
                                       Severity_Warning,
                                       "Invalid source type : Pointers are only allowed for target type = Array. Target <%s> - while source is <%s> ",
                                       decode_ZType(wTargetStructType),
                                       decode_ZType(wStructTypeIn));
               ZException.printLastUserMessage(stderr);
               return ZS_INVTYPE;
                }
           if (wAtomicTypeIn!=wTargetAtomicType)
           {
           if (wAtomicTypeIn>wTargetAtomicType)
               {
               ZException.setMessage(_GET_FUNCTION_NAME_,
                                     ZS_INVTYPE,
                                     Severity_Warning,
                                     "May loose precision : Invalid Atomic type : expecting less or equal to <%s> - having <%s> ",
                                     decode_ZType(wTargetAtomicType),
                                     decode_ZType(wAtomicTypeIn));
               ZException.printLastUserMessage(stderr);
               return ZS_INVTYPE;
                }
           }
       return ZS_SUCCESS;
        }// if pointer

    if (wStructTypeIn&ZType_String)
        {

        }
    if (pTypeIn & ZType_StdString)
            {
            return ZS_SUCCESS;
            }


    return ZS_SUCCESS;
} // testKeyTypes




ZStatus _getBlobType(void*pValue, ZTypeBase &pType, size_t &pNaturalSize , size_t &pUniversalSize , URF_Array_Count_type &pArrayCount)
{
    pType=ZType_Blob;
    ZDataBuffer* wZDB = static_cast<ZDataBuffer*>(pValue);
    pNaturalSize = pUniversalSize =wZDB->Size;
    pArrayCount=1;
    return ZS_SUCCESS;
}

ZStatus _getFixedStringType(void*pValue,
                            ZTypeBase &pType,
                            size_t &pNaturalSize ,
                            size_t &pUniversalSize ,
                            URF_Array_Count_type &pArrayCount)
{
    utfdescString* wT = static_cast<utfdescString*>(pValue);
    pType=ZType_FixedCString;
    pNaturalSize = pUniversalSize =wT->getCapacity();
    pArrayCount=(uint16_t)wT->getCapacity();
    return ZS_SUCCESS;
}
/*
ZStatus _getFixedWStringType(void*pValue, ZTypeBase &pType, size_t &pNaturalSize , size_t &pUniversalSize , URF_Array_Count_type &pArrayCount)
{
    templatePWString* wT = static_cast<templatePWString*>(pValue);
    pType=ZType_FixedWString;
    pNaturalSize = pUniversalSize =wT->getByteCapacity();
    pArrayCount=(URF_Array_Count_type)wT->getByteCapacity();
    return ZS_SUCCESS;
}
*/
#include <ztoolset/zbitset.h>
ZStatus _getBitsetType(void*pValue, ZTypeBase &pType, uint64_t &pNaturalSize , uint64_t&pUniversalSize , URF_Array_Count_type &pArrayCount)
{
    pType=ZType_bitset;
    ZBitset* wBS = static_cast<ZBitset*>(pValue);
    pNaturalSize = pUniversalSize =(size_t) (wBS->Size*wBS->UnitByteSize);
    pArrayCount=(URF_Array_Count_type)wBS->Size;
    return ZS_SUCCESS;
}


/* signed integer 16 */

US16& US16::_copyFrom(const US16& pIn)
{
  Sign=pIn.Sign;
  Value=pIn.Value;
}
US16& US16::fromNatural(int16_t pN)
{
  if (pN<0)  // if negative value
  {
    Sign=0; // sign byte is set to Zero
    Value=-pN;
    Value=_negate (Value); // mandatory otherwise -120 is greater than -110
    Value = reverseByteOrder_Conditional(Value);
    return *this;
  }

  /* positive value */
  Sign=1;
  Value=pN;
  Value = reverseByteOrder_Conditional(Value);
  return *this;
}
int16_t US16::toNatural()
{
  int16_t wValue = int16_t(reverseByteOrder_Conditional(Value));
  if (Sign > 0)
    return wValue;

  wValue=_negate (wValue);
  return - wValue;
}
ZDataBuffer US16::_export()
{
  return ZDataBuffer(this,getSize());
}
size_t US16::_import(unsigned char* &pIn)
{
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize();
}
ZDataBuffer US16::_exportURF()
{
  ZDataBuffer pOut;
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S16;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}
ZDataBuffer& US16::_exportURF(ZDataBuffer& pOut)
{
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S16;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}

ZDataBuffer US16::_exportURF(int16_t pValue)
{
  ZDataBuffer pBuf;
  US32 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S16;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}
ZDataBuffer& US16::_exportURF(int16_t pValue,ZDataBuffer& pBuf)
{
  US32 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S16;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}

ssize_t US16::_importURF(unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_S16)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize()+sizeof(ZTypeBase);
}
ssize_t US16::_importURF(int16_t& pValue,unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_S16)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (&pValue,pIn,sizeof(US16));
  pIn += sizeof(US16);
  return sizeof(US16)+sizeof(ZTypeBase);
}


/* unsigned integer 16 */

UU16& UU16::_copyFrom(const UU16& pIn)
{
  Value=pIn.Value;
}
UU16& UU16::fromNatural(uint16_t pN)
{
  /* positive value */
  Value=pN;
  Value = reverseByteOrder_Conditional(Value);
  return *this;
}

ZDataBuffer UU16::_export()
{
  return ZDataBuffer(this,getSize());
}
size_t UU16::_import(unsigned char* &pIn)
{
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize();
}
ZDataBuffer UU16::_exportURF()
{
  ZDataBuffer pOut;
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U16;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}
ZDataBuffer& UU16::_exportURF(ZDataBuffer& pOut)
{
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U16;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}

ZDataBuffer UU16::_exportURF(uint16_t pValue)
{
  ZDataBuffer pBuf;
  UU32 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U16;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}
ZDataBuffer& UU16::_exportURF(uint16_t pValue,ZDataBuffer& pBuf)
{
  UU32 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U16;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}

ssize_t UU16::_importURF(unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_U16)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize()+sizeof(ZTypeBase);
}
ssize_t UU16::_importURF(uint16_t& pValue,unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_U16)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (&pValue,pIn,sizeof(UU16));
  pIn += sizeof(UU16);
  return sizeof(UU16)+sizeof(ZTypeBase);
}

/* signed integer 32 */

US32& US32::_copyFrom(const US32& pIn)
{
  Sign=pIn.Sign;
  Value=pIn.Value;
}
US32& US32::fromNatural(int32_t pN)
{
  if (pN<0)  // if negative value
  {
    Sign=0; // sign byte is set to Zero
    Value=-pN;
    Value=_negate (Value); // mandatory otherwise -120 is greater than -110
    Value = reverseByteOrder_Conditional(Value);
    return *this;
  }

  /* positive value */
  Sign=1;
  Value=pN;
  Value = reverseByteOrder_Conditional(Value);
  return *this;
}
int32_t US32::toNatural()
{
  int32_t wValue = int32_t(reverseByteOrder_Conditional(Value));
  if (Sign > 0)
    return wValue;

  wValue=_negate (wValue);
  return - wValue;
}
ZDataBuffer US32::_export()
{
  return ZDataBuffer(this,getSize());
}
size_t US32::_import(unsigned char* &pIn)
{
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize();
}
ZDataBuffer US32::_exportURF()
{
  ZDataBuffer pOut;
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S32;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}
ZDataBuffer& US32::_exportURF(ZDataBuffer& pOut)
{
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S32;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}

ZDataBuffer US32::_exportURF(int32_t pValue)
{
  ZDataBuffer pBuf;
  US32 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S32;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}
ZDataBuffer& US32::_exportURF(int32_t pValue,ZDataBuffer& pBuf)
{
  US32 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S32;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}

ssize_t US32::_importURF(unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_S32)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize()+sizeof(ZTypeBase);
}
ssize_t US32::_importURF(int32_t& pValue,unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_S32)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (&pValue,pIn,sizeof(US32));
  pIn += sizeof(US32);
  return sizeof(US32)+sizeof(ZTypeBase);
}


/* unsigned integer 32 */

UU32& UU32::_copyFrom(const UU32& pIn)
{
  Value=pIn.Value;
}
UU32& UU32::fromNatural(uint32_t pN)
{
  /* positive value */
  Value=pN;
  Value = reverseByteOrder_Conditional(Value);
  return *this;
}

ZDataBuffer UU32::_export()
{
  return ZDataBuffer(this,getSize());
}
size_t UU32::_import(unsigned char* &pIn)
{
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize();
}
ZDataBuffer UU32::_exportURF()
{
  ZDataBuffer pOut;
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U32;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}
ZDataBuffer& UU32::_exportURF(ZDataBuffer& pOut)
{
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U32;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}

ZDataBuffer UU32::_exportURF(uint32_t pValue)
{
  ZDataBuffer pBuf;
  UU32 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U32;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}
ZDataBuffer& UU32::_exportURF(uint32_t pValue,ZDataBuffer& pBuf)
{
  UU32 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U32;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}

ssize_t UU32::_importURF(unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_U32)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize()+sizeof(ZTypeBase);
}
ssize_t UU32::_importURF(uint32_t& pValue,unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_U32)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (&pValue,pIn,sizeof(UU32));
  pIn += sizeof(UU32);
  return sizeof(UU32)+sizeof(ZTypeBase);
}



/* signed integer 64 */

US64& US64::_copyFrom(const US64& pIn)
{
  Sign=pIn.Sign;
  Value=pIn.Value;
}
US64& US64::fromNatural(int64_t pN)
{
  if (pN<0)  // if negative value
  {
    Sign=0; // sign byte is set to Zero
    Value=-pN;
    Value=_negate (Value); // mandatory otherwise -120 is greater than -110
    Value = reverseByteOrder_Conditional(Value);
    return *this;
  }

  /* positive value */
  Sign=1;
  Value=pN;
  Value = reverseByteOrder_Conditional(Value);
  return *this;
}
int64_t US64::toNatural()
{
  int64_t wValue = int64_t(reverseByteOrder_Conditional(Value));
  if (Sign > 0)
    return wValue;

  wValue=_negate (wValue);
  return - wValue;
}
ZDataBuffer US64::_export()
{
  return ZDataBuffer(this,getSize());
}
size_t US64::_import(unsigned char* &pIn)
{
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize();
}
ZDataBuffer US64::_exportURF()
{
  ZDataBuffer pOut;
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S64;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}
ZDataBuffer& US64::_exportURF(ZDataBuffer& pOut)
{
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S64;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}

ZDataBuffer US64::_exportURF(int64_t pValue)
{
  ZDataBuffer pBuf;
  US64 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S64;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}
ZDataBuffer& US64::_exportURF(int64_t pValue,ZDataBuffer& pBuf)
{
  US64 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_S64;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}

ssize_t US64::_importURF(unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_S64)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize()+sizeof(ZTypeBase);
}
ssize_t US64::_importURF(int64_t& pValue,unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_S64)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (&pValue,pIn,sizeof(US64));
  pIn += sizeof(US64);
  return sizeof(US64)+sizeof(ZTypeBase);
}


/* unsigned integer 64 */

UU64& UU64::_copyFrom(const UU64& pIn)
{
  Value=pIn.Value;
}
UU64& UU64::fromNatural(uint64_t pN)
{
  /* positive value */
  Value=pN;
  Value = reverseByteOrder_Conditional(Value);
  return *this;
}

ZDataBuffer UU64::_export()
{
  return ZDataBuffer(this,getSize());
}
size_t UU64::_import(unsigned char* &pIn)
{
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize();
}
ZDataBuffer UU64::_exportURF()
{
  ZDataBuffer pOut;
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U64;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}
ZDataBuffer& UU64::_exportURF(ZDataBuffer& pOut)
{
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U64;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}

ZDataBuffer UU64::_exportURF(uint64_t pValue)
{
  ZDataBuffer pBuf;
  US64 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U64;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}
ZDataBuffer& UU64::_exportURF(uint64_t pValue,ZDataBuffer& pBuf)
{
  UU64 pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_U64;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}

ssize_t UU64::_importURF(unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_U64)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize()+sizeof(ZTypeBase);
}
ssize_t UU64::_importURF(uint64_t& pValue,unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_U64)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (&pValue,pIn,sizeof(UU64));
  pIn += sizeof(US64);
  return sizeof(US64)+sizeof(ZTypeBase);
}



/* float */

UFloat& UFloat::_copyFrom(const UFloat& pIn)
{
  Sign=pIn.Sign;
  Value=pIn.Value;
}
UFloat& UFloat::fromNatural(float pN)
{
  if (pN<0)  // if negative value
  {
    Sign=0; // sign byte is set to Zero
    Value=-pN;
    Value=_negate (Value); // mandatory otherwise -120 is greater than -110
    Value = reverseByteOrder_Conditional(Value);
    return *this;
  }

  /* positive value */
  Sign=1;
  Value=pN;
  Value = reverseByteOrder_Conditional(Value);
  return *this;
}
float UFloat::toNatural()
{
  float wValue = float(reverseByteOrder_Conditional(Value));
  if (Sign > 0)
    return wValue;

  wValue=_negate (wValue);
  return - wValue;
}
ZDataBuffer UFloat::_export()
{
  return ZDataBuffer(this,getSize());
}
size_t UFloat::_import(unsigned char* &pIn)
{
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize();
}
ZDataBuffer UFloat::_exportURF()
{
  ZDataBuffer pOut;
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_Float;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}
ZDataBuffer& UFloat::_exportURF(ZDataBuffer& pOut)
{
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_Float;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}

ZDataBuffer UFloat::_exportURF(float pValue)
{
  ZDataBuffer pBuf;
  UFloat pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_Float;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}
ZDataBuffer& UFloat::_exportURF(float pValue,ZDataBuffer& pBuf)
{
  UFloat pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_Float;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}

ssize_t UFloat::_importURF(unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_Float)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize()+sizeof(ZTypeBase);
}
ssize_t UFloat::_importURF(float& pValue,unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_Float)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (&pValue,pIn,sizeof(US64));
  pIn += sizeof(US64);
  return sizeof(US64)+sizeof(ZTypeBase);
}

/* double */

UDouble& UDouble::_copyFrom(const UDouble& pIn)
{
  Sign=pIn.Sign;
  Value=pIn.Value;
}
UDouble& UDouble::fromNatural(double pN)
{
  if (pN<0)  // if negative value
  {
    Sign=0; // sign byte is set to Zero
    Value=-pN;
    Value=_negate (Value); // mandatory otherwise -120 is greater than -110
    Value = reverseByteOrder_Conditional(Value);
    return *this;
  }

  /* positive value */
  Sign=1;
  Value=pN;
  Value = reverseByteOrder_Conditional(Value);
  return *this;
}
double UDouble::toNatural()
{
  double wValue = double(reverseByteOrder_Conditional(Value));
  if (Sign > 0)
    return wValue;

  wValue=_negate (wValue);
  return - wValue;
}
ZDataBuffer UDouble::_export()
{
  return ZDataBuffer(this,getSize());
}
size_t UDouble::_import(unsigned char* &pIn)
{
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize();
}
ZDataBuffer UDouble::_exportURF()
{
  ZDataBuffer pOut;
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_Double;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}
ZDataBuffer& UDouble::_exportURF(ZDataBuffer& pOut)
{
  unsigned char* wPtr=pOut.extend(getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_Double;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,this,getSize());
  return pOut;
}

ZDataBuffer UDouble::_exportURF(double pValue)
{
  ZDataBuffer pBuf;
  UDouble pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_Double;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}
ZDataBuffer& UDouble::_exportURF(double pValue,ZDataBuffer& pBuf)
{
  UDouble pOut(pValue);
  unsigned char* wPtr=pBuf.extend(pOut.getSize()+sizeof(ZTypeBase));
  const ZTypeBase wT=ZType_Double;
  memmove(wPtr,&wT,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);
  memmove (wPtr,&pOut,pOut.getSize());
  return pBuf;
}

ssize_t UDouble::_importURF(unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_Double)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (this,pIn,getSize());
  pIn += getSize();
  return getSize()+sizeof(ZTypeBase);
}
ssize_t UDouble::_importURF(double& pValue,unsigned char* &pIn)
{
  ZTypeBase wT;
  memmove(&wT,pIn,sizeof(ZTypeBase));
  if (wT!=ZType_Double)
    return -1;
  pIn += sizeof(ZTypeBase);
  memmove (&pValue,pIn,sizeof(US64));
  pIn += sizeof(US64);
  return sizeof(US64)+sizeof(ZTypeBase);
}



#endif // ZDATATYPE_CPP
