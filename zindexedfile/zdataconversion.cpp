
#include <zindexedfile/zdataconversion.h>

#include <ztoolset/zexceptionmin.h>
#include <ztoolset/ztypetype.h>
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
          uint8_t wData = *((uint8_t*)pInData.Data);
          _getAtomicUfN<uint8_t>(wData,pOutData);
          return ZS_SUCCESS;
         }
  case ZType_S8 :
    {
    int8_t wData = *((int8_t*)pInData.Data);
    _getAtomicUfN<int8_t>(wData,pOutData);
    return ZS_SUCCESS;
    }

  case ZType_U16 :
      {
      uint16_t wData = *((uint16_t*)pInData.Data);
      _getAtomicUfN<uint16_t>(wData,pOutData);
      return ZS_SUCCESS;
      }
  case ZType_S16 :
     {
      int16_t wData = *((int16_t*)pInData.Data);
     _getAtomicUfN<int16_t>(wData,pOutData);
     return ZS_SUCCESS;
     }
  case ZType_U32 :
      {
      uint32_t wData = *((uint32_t*)pInData.Data);
       _getAtomicUfN<uint32_t>(wData,pOutData);
       return ZS_SUCCESS;
      }
  case ZType_S32 :
       {
        int32_t wData = *((int32_t*)pInData.Data);
       _getAtomicUfN<int32_t>(wData,pOutData);
       return ZS_SUCCESS;
       }
  case ZType_U64 :
      {
      uint64_t wData = *((uint64_t*)pInData.Data);
      _getAtomicUfN<uint64_t>(wData,pOutData);
      return ZS_SUCCESS;
      }
 case ZType_S64 :
     {
    int64_t wData = *((int64_t*)pInData.Data);
     _getAtomicUfN<int64_t>(wData,pOutData);
     return ZS_SUCCESS;
     }
  case ZType_Float :
    {
    float wData = *((float*)pInData.Data);
    _getAtomicUfN<float>(wData,pOutData);
    return ZS_SUCCESS;
    }
  case ZType_Double :
    {
    double wData = *((double*)pInData.Data);
    _getAtomicUfN<double>(wData,pOutData);
    return ZS_SUCCESS;
    }
  case ZType_LDouble :
    {
    long double wData = *((long double*)pInData.Data);
    _getAtomicUfN<long double>(wData,pOutData);
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


#ifdef __DEPRECATED__
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
 //        case ZType_WChar :
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
#endif // __DEPRECATED__
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

template <class _Tp>
inline
size_t getOtherURF(const ZTypeBase pType,_Tp &pData)
{
  ZTypeBase wZType = pType & ~(ZType_Array | ZType_Pointer);

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
  return 0;
}//getOtherURF

