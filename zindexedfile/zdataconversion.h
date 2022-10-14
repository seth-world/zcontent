#ifndef ZDATACONVERSION_H
#define ZDATACONVERSION_H
#include <zconfig.h>
#include <cstdint>
#include <type_traits>
#include <ztoolset/zerror.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zbasedatatypes.h>
#include <ztoolset/zexceptionmin.h>
#include <zrandomfile/zrandomfiletypes.h>
#include <cstdint>
#include <ztoolset/zatomicconvert.h>

#include <zindexedfile/zdatatype.h>



//#include <ztoolset/zwstrings.h>

template <class _Tp>
size_t getAtomicURFSize(ZTypeBase pType) ;
size_t  _getURFHeaderSize (ZTypeBase &pZType);

ZStatus _getURFHeaderData(const unsigned char* pURF_Ptr,
    ZTypeBase &pZType,
    uint64_t &pUniversalSize,
    uint64_t &pNaturalSize,
    uint16_t &pCanonical,
    uint16_t &pEffectiveUSize,
    uint64_t &pHeaderSize,
    const unsigned char **pDataPtr);

ZStatus _getAtomicUfN(ZDataBuffer &pInData,ZDataBuffer &pOutData,const ZType_type pType);

//===================== Natural to Universal data and URF conversion ============================

/** @addtogroup DATAFORMATGROUP
 *
 * @weakgroup LLRTC Low level routines : templates for conversion to and from Universal format ========================

@par Standards

    - by data types
          . _getAtomicxxx : concerns atomic data conversion
          . _getArrayxxx : concerns arrays conversion
    - suffixes :
          . UfN  Universal from Natural converts a natural data to its universal format
          . NfU  Natural from Universal converts a universal format to natural format

              When routine is a template, then suffix <_T> is added to routine name
        When routine is NOT a template but uses 'auto' clause, then suffix <_A> is added to routine name

            */
        /**




 * @brief _getAtomicUfN suite  Template: get Atomic Universal value (URF header excepted) From Natural value according its data type (ZType_type) given by pZType
 *
Converts a Atomic Natural field to an Universal format value
reverse byte order if necessary (Endian)
and returns
  - a ZDataBuffer with the formatted field content ready to be used as a natural data type in the given ZDataBuffer pOutData
  - the atomic value data field as well as a return value

        Size of the resulting field is the size of natural data type (ex : int8_t is 1 ), and Size field of ZDataBuffer pOutData is set to correct length.

            *
                * @param[in] pInData       Atomic value to format field from
                    * @param[out] pOutData     Universal value written to ZDataBuffer, eventually extended according routine
            * @param[in] pZType        Type of data ( ZType_type )
        */


template <class _Tp>
void _getAtomicUfN(_Tp pInData, ZDataBuffer &pUniversalData,const ZTypeBase pType ) {

  _Tp wValue=pInData , wValue2;

  if (pType & ZType_Signed) {

    unsigned char* wPtr=pUniversalData.allocate(sizeof(_Tp)+1);   // unsigned means size + sign byte
    if (wValue < 0)  {
      wValue2 = -wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wValue2 = _negate (wValue2); // mandatory otherwise -120 is greater than -110
      wPtr[0] = 0; // sign byte is set to Zero meaning negative value in URF
    }
    else {
      wValue2 = wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wPtr[0] = 1; // sign byte is set to 1 meaning positive value in URF
    }
    memmove(wPtr+1,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
    return ;
  } // if (pType & ZType_Signed)

  // up to here : unsigned data type
  unsigned char* wPtr=pUniversalData.allocate(sizeof(_Tp));
  wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
  memmove(wPtr,&wValue2,sizeof(_Tp));
  return ;
} // _getAtomicUfN

template <class _Tp>
static inline
    void _getAtomicUfN_Append(_Tp pInData, ZDataBuffer &pUniversalOut,const ZTypeBase pType ) {

  _Tp wValue=pInData , wValue2;

  if (pType & ZType_Signed) {

    unsigned char* wPtr=pUniversalOut.extend(sizeof(_Tp)+1);   // unsigned means size + sign byte
    if (wValue < 0)  {
      wValue2 = -wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wValue2 = _negate (wValue2); // mandatory otherwise -120 is greater than -110
      wPtr[0] = 0; // sign byte is set to Zero meaning negative value in URF
    }
    else {
      wValue2 = wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wPtr[0] = 1; // sign byte is set to 1 meaning positive value in URF
    }
    memmove(wPtr+1,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
    return ;
  } // if (pType & ZType_Signed)

  // up to here : unsigned data type
  unsigned char* wPtr=pUniversalOut.extend(sizeof(_Tp));
  wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
  memmove(wPtr,&wValue2,sizeof(_Tp));
  return ;
} // _getAtomicUfN_Append

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
ZStatus _getArrayUfN_T_Ptr(_Tp* pInData_Ptr,
 ZDataBuffer &pOutData,
 const ZType_type pType,
 const URF_Array_Count_type pArrayCount)  // mandatory input
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
    _getAtomicUfN(pInData_Ptr[wi],wDBElt,wZType) ;
    pOutData.appendData(wDBElt);
    wEltOffsetIn+=wEltNSize;
  }
  return ZS_SUCCESS;
}//_getArrayUfN_T_Ptr


template <class _Tp>
static inline
    void _getAtomicURF_Append(_Tp pInData, ZDataBuffer &pURFOut,const ZTypeBase pType ) {

  _Tp wValue=pInData , wValue2;

  ZTypeBase wType =   reverseByteOrder_Conditional<ZTypeBase>(pType);

  if (pType & ZType_Signed) {
    // signed urf size means data size + sign byte +ZTypeBase
    unsigned char* wPtr=pURFOut.extend(sizeof(_Tp) + sizeof(ZTypeBase) + 1);

    if (wValue < 0)  {
      wValue2 = -wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wValue2 = _negate (wValue2); // mandatory otherwise -120 is greater than -110
      wPtr[sizeof(ZTypeBase)] = 0; // sign byte is set to Zero meaning negative value in URF
    }
    else {
      unsigned char* wPtr=pURFOut.extend(sizeof(_Tp) + sizeof(ZTypeBase) + 1);
      wValue2 = wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wPtr[sizeof(ZTypeBase)] = 1; // sign byte is set to 1 meaning positive value in URF
    }
    memmove(wPtr+sizeof(ZTypeBase)+1,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
    return ;
  } // if (pType & ZType_Signed)

  // unsigned urf size means size + ZTypeBase
  unsigned char* wPtr=pURFOut.extend(sizeof(_Tp) + sizeof(ZTypeBase) );
  wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);

  memmove(wPtr,&wType,sizeof(ZTypeBase));
  memmove(wPtr+sizeof(ZTypeBase),&wValue2,sizeof(_Tp));
  return ;
} // _getAtomicURF_Append

template <class _Tp>
static inline
    void getAtomicURF_Append(_Tp pInData, ZDataBuffer &pURFOut,const ZTypeBase pType ) {

  _Tp wValue=pInData , wValue2;

  ZTypeBase wType =   reverseByteOrder_Conditional<ZTypeBase>(pType);

  if (pType & ZType_Signed) {
    // signed urf size means data size + sign byte +ZTypeBase
    unsigned char* wPtr=pURFOut.extend(sizeof(_Tp) + sizeof(ZTypeBase) + 1);

    if (wValue < 0)  {
      wValue2 = -wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wValue2 = _negate (wValue2); // mandatory otherwise -120 is greater than -110
      wPtr[sizeof(ZTypeBase)] = 0; // sign byte is set to Zero meaning negative value in URF
    }
    else {
      unsigned char* wPtr=pURFOut.extend(sizeof(_Tp) + sizeof(ZTypeBase) + 1);
      wValue2 = wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wPtr[sizeof(ZTypeBase)] = 1; // sign byte is set to 1 meaning positive value in URF
    }
    memmove(wPtr+sizeof(ZTypeBase)+1,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
    return ;
  } // if (pType & ZType_Signed)

  // unsigned urf size means size + ZTypeBase
  unsigned char* wPtr=pURFOut.extend(sizeof(_Tp) + sizeof(ZTypeBase) );
  wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);

  memmove(wPtr,&wType,sizeof(ZTypeBase));
  memmove(wPtr+sizeof(ZTypeBase),&wValue2,sizeof(_Tp));
  return ;
} // getAtomicURF_Append


template <class _Tp>
void exportAtomicURF_Ptr(_Tp pInData, unsigned char* &pPtrOut,const ZTypeBase pType ) {

  _Tp wValue=pInData , wValue2;

  _exportAtomicPtr<ZTypeBase>(pType,pPtrOut);

  if (pType & ZType_Signed) {
    // signed urf size means data size + sign byte +ZTypeBase
    //    unsigned char* pPtrOut=pURFOut.extend(sizeof(_Tp) + sizeof(ZTypeBase) + 1);

    if (wValue < 0)  {
      wValue2 = -wValue;
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      wValue2 = _negate (wValue2); // mandatory otherwise -120 is greater than -110
      pPtrOut[sizeof(ZTypeBase)] = 0; // sign byte is set to Zero meaning negative value in URF
      pPtrOut++;
      memmove(pPtrOut,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
      pPtrOut += sizeof(wValue2);
    }
    else {
      pPtrOut[sizeof(ZTypeBase)] = 1; // sign byte is set to 1 meaning positive value in URF
      pPtrOut++;
      _exportAtomicPtr<_Tp>(wValue,pPtrOut);
    }
    memmove(pPtrOut+sizeof(ZTypeBase)+1,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
    return ;
  } // if (pType & ZType_Signed)

  /* data type is unsigned */

  _exportAtomicPtr<_Tp>(wValue,pPtrOut);
  return ;
} // getAtomicURF_Ptr

template <class _Tp>
void exportAtomicURF_Ptr(_Tp pInData, unsigned char* &pPtrOut ) {
  size_t                wNaturalSize;
  size_t                wUniversalSize;
  URF_Array_Count_type  wUnitCount;
  ZTypeBase             wType;
  wType=_getZTypeFull_T<_Tp>(pInData,wNaturalSize,wUniversalSize,wUnitCount);
  exportAtomicURF_Ptr<_Tp>(pInData,pPtrOut,wType);
}

/**
 * @brief _getArrayUfN_T  Template: gets an Array of Universal values from an array of Natural values
 *
 *
 * @param pKeyData      Key buffer to extract field from
 * @param pType         Full ZType_type for the array
 * @return The obtained value with _Tp template parameter type : Field content extracted and formatted up to be used as key field
 */
template <class _Tp>
ZStatus _getArrayUfN_T (_Tp &pValue,
        ZDataBuffer &pUniversal,
        const ZType_type pType,
        URF_Array_Count_type &pArrayCount)
{
  ZStatus wSt;
  ZDataBuffer wDBV;

  ZTypeBase wType = pType & ZType_AtomicMask;  // get the atomic data type
  // Compute array ranks
  long wAtomicType = pType & ZType_AtomicMask;
  long wAtomicUSize = getAtomicUniversalSize(wAtomicType);

  pArrayCount = (uint16_t)(sizeof(pValue)/wAtomicUSize);

  // following data formats MUST not be converted with leading sign byte : just move them.

  if ((wType == ZType_Char)||(wType == ZType_UChar)||(wType == ZType_U8))
  {
    pUniversal.setData(&pValue,sizeof(pValue));
    return ZS_SUCCESS;
  }

  for (long wi=0;wi<pArrayCount;wi++)
    _getAtomicUfN_Append<_Tp>(pValue[wi],pUniversal,wType);

  return ZS_SUCCESS;
} // _getArrayUfN_T

//========== Conversion from Universal to Natural data format======================================

/**
 * @brief _getAtomicNfU_T_Ptr  Template: get Atomic Natural value From  Universal format
 *
Extracts an ZType_Atomic field from a ZDataBuffer (Universal format according ZType_type) :

- using the length deduced from its originary type.

Data value byte order is reversed according Endian convention IF NECESSARY (if system is little Endian).

*
* @param pUniversal ZDataBuffer containing Atomic data in universal format
* @param pType      ZType_type of data to be converted
* @return The obtained value with _Tp template parameter type.
*/

template <class _Tp>  // type of _Tp must correspond strictly to pSourceType
_Tp _getAtomicNfU_T_Ptr ( typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
                          const unsigned char* pUniversal_Ptr,
                          const ZTypeBase pSourceType)
{
  //    _Tp pValue;

  size_t wUniversalSize = getAtomicUniversalSize(pSourceType);
  size_t wNaturalSize = getAtomicNaturalSize(pSourceType);


  if (pSourceType & ZType_Signed)
  {
    while (true)
    {
      if (is_little_endian()) // only if system is little endian
        if (pSourceType & ZType_Endian)   // and data type is subject to endian reverse byte conversion
        {
          pValue=reverseByteOrder_Ptr<_Tp>(pUniversal_Ptr+1);
          break;
        }
      memmove(&pValue,(pUniversal_Ptr+1),wUniversalSize);
      break;
    }// while
    if (pUniversal_Ptr[0]==0)  // signed negative value
    {
      pValue=_negate(pValue); // mandatory otherwise -120 is greater than -110
      return -pValue;
    }
    else // it is a positive value
    {
      return pValue;
    }
  }
  // not signed : means no byte sign
  while (true)
  {
    if (is_little_endian()) // only if system is little endian
    {
      if (pSourceType & ZType_Endian)   // and data type is subject to endian reverse byte conversion
      {
        pValue=reverseByteOrder_Ptr<_Tp>(pUniversal_Ptr);
        break;
      }
    }
    memmove(&pValue,pUniversal_Ptr,wNaturalSize);
    break;
  }// while
  return pValue;
} // _getAtomicNfU_T_Ptr



/**
 * @brief _getArrayNfU_T  Template: gets an Array of Natural values from an array of Universal values
 *
 *
 * @param pKeyData      Key buffer to extract field from
 * @param pType         Full ZType_type for the array
 * @return The obtained value with _Tp template parameter type : Field content extracted and formatted up to be used as key field
 */
template <class _Tp>
_Tp _getArrayNfU_T (typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue,
        ZDataBuffer &pUniversal,
        const ZType_type pType)
{
  size_t wAtomicOffset =0 ;
  int32_t wType = pType & ZType_AtomicMask;  // get the atomic data type
  // Compute array ranks
  long wAtomicUSize = getAtomicUniversalSize(wType);


  size_t wArrayCount = pUniversal.Size / wAtomicUSize;
  size_t wTargetArrayCount = sizeof(pValue)/wAtomicUSize;  // must be equal to wArrayCount

  if (wArrayCount!=wTargetArrayCount)
  {
    fprintf(stderr,"%s-W Warning target array count <%ld> is not equal to source <%ld>\n",
        _GET_FUNCTION_NAME_,
        wTargetArrayCount,
        wArrayCount);
  }
  // following data formats MUST not be converted with leading sign byte : just move them.

  if ((wType == ZType_Char)||(wType == ZType_UChar)||(wType == ZType_U8))
  {
    memmove(pValue,pUniversal.Data,pUniversal.Size);
    return pValue;
  }

  for (long wi=0;wi<wArrayCount;wi++)
  {
    //        wDBV.setData(pUniversal.Data+wAtomicOffset,wAtomicUSize);
    switch (wType)
    {
    case ZType_S8 :
    {
      int8_t wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<int8_t>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
    }
      //            case ZType_WChar:  // WChar is treated as an unsigned uint16_t but subject to endian conversion
      //            case ZType_WUChar:  // WUChar is treated as an unsigned uint16_t but subject to endian conversion
    case ZType_U16 :
    {
      uint16_t wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<uint16_t>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }
    case ZType_S16 :
    {
      int16_t wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<int16_t>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }
    case ZType_U32 :
    {
      uint32_t wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<uint32_t>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }
    case ZType_S32 :
    {
      int32_t wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<int32_t>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }
    case ZType_U64 :
    {
      uint64_t wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<uint64_t>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }
    case ZType_S64 :
    {
      int64_t wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<int64_t>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }
    case ZType_Float :
    {
      float wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<float>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }
    case ZType_Double :
    {
      double wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<double>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }
    case ZType_LDouble :
    {
      long double wValue;
      pValue[wi]=_getAtomicNfU_T_Ptr<long double>(wValue,pUniversal.Data+wAtomicOffset,(ZType_type)wType);
      break;
    }

    default:
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_INVTYPE,
          Severity_Fatal,
          "Invalid atomic ZType <%ld> <%s> encountered while processing record data",
          pType,
          decode_ZType(pType));
      ZException.exit_abort();
    }

    }//switch

    wAtomicOffset += wAtomicUSize;
  }
  return pValue;
} // _getArrayNfU_T_Ptr

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





template <class _Tp>
size_t importAtomicURF(_Tp &pOutData, const unsigned char* &pURFin ) {
  size_t                wNaturalSize;
  size_t                wUniversalSize;
  URF_Array_Count_type  wUnitCount;
  ZTypeBase             wType, wOutType;

  _Tp wValue , wValue2;

  /* get urf header */
  wOutType=_getZTypeFull_T<_Tp>(pOutData,wNaturalSize,wUniversalSize,wUnitCount);
  _importAtomic<ZTypeBase>(wType,pURFin);
  if (wType!=wOutType) {
    fprintf (stderr,"utfVaryingString::_importURF-E-IVTYP Invalid URF type <%s> while requested <%s>.\n",
        decode_ZType(wType),
        decode_ZType(wOutType));
    errno=EPERM;
    return 0;
  }

  if (wType & ZType_Signed) {
    if (pURFin[0]==0) {
      pURFin++;
      /* signed negative */
      memmove(&wValue2,pURFin,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
      wValue2 = _negate (wValue2);
      wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);
      pOutData = -wValue2;
      pURFin += sizeof(wValue2);
      return sizeof(ZTypeBase) + 1 + sizeof(_Tp);
    }
    /* signed positive */
    pURFin++;
    _importAtomic<_Tp>(pOutData,pURFin);
    return sizeof(ZTypeBase) + 1 + sizeof(_Tp);
  }//if (wType & ZType_Signed)

  /* unsigned */
  _importAtomic<_Tp>(pOutData,pURFin);
  return sizeof(ZTypeBase)+ sizeof(_Tp) ;
} // importAtomicURF

template <class _Tp>
size_t
getAtomicURFSize(_Tp pValue) {
  ZTypeBase wType;        //!< Field type mask using ZType_type
  size_t wNaturalSize;    //!< total field natural size (External size)
  size_t wUniversalSize;  //!< total field internal format size ( may differ from natural size according sign byte )
  URF_Array_Count_type wArrayCount;
  wType = _getZTypeFull_T<_Tp>(pValue,wNaturalSize,wUniversalSize,wArrayCount);
  if (wType & ZType_Atomic)
    return    sizeof(ZTypeBase) + getAtomicUniversalSize(wType);
  return    sizeof(ZTypeBase) + getAtomicUniversalSize(wType);
}


template <class _Tp>
ZStatus
_setUniversalFromStdString (const void* pString,ZDataBuffer & pUniversal, const ZType_type pDesiredType, const ssize_t pDesiredArraySize)
{
  const std::string*  wString= static_cast<const std::string*>(pString);
  ssize_t wSize = wString->size()+1 ;
  //ssize_t wOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);

  if (pDesiredType!=ZType_ArrayChar)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Error,
        " While trying to set up a Key field value : destination field->expecting array of char - having %s ",
        decode_ZType(pDesiredType));
    return ZS_INVTYPE;
  }

  if (wSize>= pDesiredArraySize)
    wSize = pDesiredArraySize-1;

  pUniversal.setData((void*)wString->c_str(),wSize);
  pUniversal.Data[wSize]='\0';
  return ZS_SUCCESS;
}// _setKeyFieldValueStdString


template <class _Tp>
/**
 * @brief _setUniversalValueCString Converts a CString (char*) to an array of char of pDesiredSize maximum length
 * @param pString
 * @param pUniversal
 * @param pDesiredType
 * @param pDesiredSize
 * @return
 */
ZStatus
_setUniversalFromCString (const void* pString,ZDataBuffer & pUniversal, const ZType_type pDesiredType, const ssize_t pDesiredSize)
{
  const char*  wString= static_cast<const char*>(pString);
  ssize_t wSize = strlen(wString)+1 ;
  //ssize_t wOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);

  if (pDesiredType!=ZType_ArrayChar)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Error,
        " While trying to set up a universal field value : destination field->expecting array of char - having %s ",
        pDesiredType);
    return ZS_INVTYPE;
  }

  if (wSize> pDesiredSize)
    wSize = pDesiredSize-1;

  pUniversal.setData((void*)wString,wSize);
  pUniversal.DataChar[wSize]='\0';
  return ZS_SUCCESS;
}

template <class _Tp>
/**
 * @brief _setUniversalValueWString Converts a WString (wchar_t*) to an array of wchar_t of pDesiredSize maximum length
 * @param pString
 * @param pUniversal
 * @param pDesiredType
 * @param pDesiredSize
 * @return
 */
ZStatus
_setUniversalFromWString (const void* pString,ZDataBuffer & pUniversal, const ZType_type pDesiredType, const ssize_t pDesiredSize)
{
  const char*  wString= static_cast<const char*>(pString);
  ssize_t wSize = strlen(wString)+1 ;
  //ssize_t wOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);

  if (pDesiredType!=ZType_ArrayChar)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Error,
        " While trying to set up a universal field value : destination field->expecting array of char - having %s ",
        pDesiredType);
    return ZS_INVTYPE;
  }

  if (wSize> pDesiredSize)
    wSize = pDesiredSize-1;

  pUniversal.setData((void*)wString,wSize);
  pUniversal.DataChar[wSize]='\0';
  return ZS_SUCCESS;
}






/** @cond Development */

#ifdef __COMMENT__

template <class _TpTarget>
/**
 * @brief _castAtomicToNfU_T template routine that delivers a natural value with casted data from an input type to another
 *      Types must be compatible
 * @param pValue Ouput data
 * @param pUniversal
 * @param pDesiredType
 * @return
 */
ZStatus
_castAtomicToNfU_T (typename std::enable_if_t<std::is_integral<_TpTarget>::value|std::is_floating_point<_TpTarget>::value,_TpTarget> &pTargetValue,
    const unsigned char* pUniversal, // Source data
    ZTypeBase pSourceType)
{
  size_t wUniversalSize, wNaturalSize;
  uint16_t wArrayCount;
  ZTypeBase wTargetType;
  ZStatus wSt=ZS_SUCCESS;

  ZTypeBase wSourceType = pSourceType & ZType_AtomicMask;

  wSt=getAtomicZType(pTargetValue,wTargetType,wNaturalSize,wUniversalSize);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  // @todo : check type by type with max value

  switch (wSourceType)
  {
  case ZType_U8 :
  {

    uint8_t wValue= _getAtomicNfU_T_Ptr<uint8_t>(wValue,pUniversal,wSourceType); // no endian no sign

    _getAtomicNfU_T_Ptr<uint8_t>(wValue,pUniversal,wTargetType);
    return ZS_SUCCESS;
  }
  case ZType_S8 : // no endian - sign byte
  {
    int8_t wValue= _getAtomicNfU_T_Ptr<int8_t>(wValue,pUniversal,wSourceType);
    if ((wSt=checkMaxAtomicValue_T<int8_t>(wTargetType,wValue))!=ZS_SUCCESS)
      return wSt;
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }

  case ZType_U16 :
  {
    uint16_t wValue= _getAtomicNfU_T_Ptr<uint16_t>(wValue,pUniversal,wSourceType);
    if ((wSt=checkMaxAtomicValue_T<uint16_t>(wTargetType,wValue))!=ZS_SUCCESS)
      return wSt;
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_S16 :
  {
    int16_t wValue= _getAtomicNfU_T_Ptr<int16_t>(wValue,pUniversal,wSourceType);
    if ((wSt=checkMaxAtomicValue_T<int16_t>(wTargetType,wValue))!=ZS_SUCCESS)
      return wSt;
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_U32 :
  {
    uint32_t wValue= _getAtomicNfU_T_Ptr<uint32_t>(wValue,pUniversal,wSourceType);
    if ((wSt=checkMaxAtomicValue_T<uint32_t>(wTargetType,wValue))!=ZS_SUCCESS)
      return wSt;
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_S32 :
  {
    int32_t wValue= _getAtomicNfU_T_Ptr<int32_t>(wValue,pUniversal,wSourceType);
    if ((wSt=checkMaxAtomicValue_T<int32_t>(wTargetType,wValue))!=ZS_SUCCESS)
      return wSt;
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_U64 :
  {
    uint64_t wValue= _getAtomicNfU_T_Ptr<uint64_t>(wValue,pUniversal,wSourceType);
    if ((wSt=checkMaxAtomicValue_T<uint64_t>(wTargetType,wValue))!=ZS_SUCCESS)
      return wSt;
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_S64 :
  {
    int64_t wValue= _getAtomicNfU_T_Ptr<int64_t>(wValue,pUniversal,wSourceType);
    if ((wSt=checkMaxAtomicValue_T<int64_t>(wTargetType,wValue))!=ZS_SUCCESS)
      return wSt;
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_Float :
  {
    float wValue= _getAtomicNfU_T_Ptr<float>(wValue,pUniversal,wSourceType);
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_Double :
  {
    double wValue= _getAtomicNfU_T_Ptr<double>(wValue,pUniversal,wSourceType);
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_LDouble :
  {
    long double wValue= _getAtomicNfU_T_Ptr<long double>(wValue,pUniversal,wSourceType);
    pTargetValue=static_cast<_TpTarget>(wValue);
    return ZS_SUCCESS;
  }

  default:
  {
    _getZTypeFull_T<_TpTarget>(pTargetValue,wTargetType,wNaturalSize,wUniversalSize,wArrayCount);
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Severe,
        "Invalid Type code <%lX> [type <%s>] encountered while processing data conversion",
        wTargetType,
        decode_ZType(wTargetType));
    return ZS_INVTYPE;
  }
  }// switch*/
}// _castAtomicValue


/**
 * @brief _castAtomicValue template function that converts any atomic value type to Universal atomic value
 *                                  according pDesiredType
 *          This routine is used to change atomic datatype from one to another before converting it
 *
 * @param[in] pValue        Data to convert
 * @param[in] pDesiredType  ZType_Type of Data to convert
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
template <class _Tp>
ZStatus
_castAtomicValue_T (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
    const ZType_type pDesiredType,
    ZDataBuffer &pOutValue)
{

  if (sizeof(_Tp)>getAtomicNaturalSize(pDesiredType))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_FIELDLOOSEPREC,
        Severity_Error,
        "asked to convert to <%s>. Field may loose precision: Conversion rejected.",
        decode_ZType(pDesiredType));
    return ZS_FIELDLOOSEPREC;
  }
  ZTypeBase wType = pDesiredType & ZType_AtomicMask;

  switch (wType)
  {
  case ZType_U8 :
  {
    uint8_t wValue= static_cast<uint8_t>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_S8 :
  {
    int8_t wValue= static_cast<int8_t>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }

  case ZType_U16 :
  {
    uint16_t wValue= static_cast<uint16_t>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_S16 :
  {
    int16_t wValue= static_cast<int16_t>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_U32 :
  {
    uint32_t wValue= static_cast<uint32_t>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_S32 :
  {
    int32_t wValue= static_cast<int32_t>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_U64 :
  {
    uint64_t wValue= static_cast<uint64_t>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_S64 :
  {
    int64_t wValue= static_cast<int64_t>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_Float :
  {
    float wValue= static_cast<float>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_Double :
  {
    double wValue= static_cast<double>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }
  case ZType_LDouble :
  {
    long double wValue= static_cast<long double>(pValue);
    pOutValue.setData(&wValue,sizeof(wValue));
    return ZS_SUCCESS;
  }

  default:
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Fatal,
        "Invalid Type for key field to convert data to <%ld> <%s> encountered while processing data conversion",
        pDesiredType,
        decode_ZType(pDesiredType));
    return ZS_INVTYPE;
  }
  }// switch
}// _castAtomicValue




/**
 * @brief _castAtomicToNfU_T template function that converts any Universal atomic value of pSourceType
 *          to Natural atomic value of type _Tp
 *          This routine is used to convert to natural value and to change atomic datatype from source type to the requested one
 *
 * @param[in] pInValue      Data to convert in Universal format
 * @param[in] pSourceType   ZType_Type of Data to convert
 * @param[out] pOutValue    Value converted and eventually casted to appropriate type
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
template <class _Tp>
ZStatus
_castAtomicToNfU_T (ZDataBuffer &pInValue,
    const ZType_type pSourceType,
    typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pOutValue)
{
  size_t wSize=getAtomicNaturalSize(pSourceType);
  if (sizeof(_Tp) < wSize)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_FIELDLOOSEPREC,
        Severity_Error,
        "asked to convert from <%s>. Field may loose precision: Conversion rejected.",
        decode_ZType(pSourceType));
    return ZS_FIELDLOOSEPREC;
  }
  ZTypeBase wType =  pSourceType & ZType_AtomicMask;

  switch (wType)
  {
  case ZType_U8 :
  {
    uint8_t wValue;
    memmove(&wValue,pInValue.Data,sizeof(wValue));
    //              wValue=_reverseByteOrder_T<uint8_t>(wValue);
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_S8 :
  {
    int8_t wValue ;
    memmove(&wValue,pInValue.Data+1,sizeof(wValue));
    //            wValue=_reverseByteOrder_T<int8_t>(wValue);
    if (pInValue.Data[0]!=1) // if negative
    {
      _negate(wValue);
      wValue=-wValue;
    }
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }

  case ZType_U16 :
  {
    uint16_t wValue;
    memmove(&wValue,pInValue.Data,sizeof(wValue));
    wValue=_reverseByteOrder_T<uint16_t>(wValue);
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_S16 :
  {
    int16_t wValue ;
    memmove(&wValue,pInValue.Data+1,sizeof(wValue));
    wValue=_reverseByteOrder_T<int16_t>(wValue);
    if (pInValue.Data[0]!=1) // if negative
    {
      _negate(wValue);
      wValue=-wValue;
    }
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_U32 :
  {
    uint32_t wValue;
    memmove(&wValue,pInValue.Data,sizeof(wValue));
    wValue=_reverseByteOrder_T<uint32_t>(wValue);
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_S32 :
  {
    int32_t wValue ;
    memmove(&wValue,pInValue.Data+1,sizeof(int32_t));
    wValue=_reverseByteOrder_T<int32_t>(wValue);
    if (pInValue.Data[0]!=1) // if negative
    {
      _negate(wValue);
      wValue=-wValue;
    }
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_U64 :
  {
    uint64_t wValue;
    memmove(&wValue,pInValue.Data,sizeof(wValue));
    wValue=_reverseByteOrder_T<uint64_t>(wValue);
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_S64 :
  {
    int64_t wValue ;
    memmove(&wValue,pInValue.Data+1,sizeof(wValue));
    wValue=_reverseByteOrder_T<int64_t>(wValue);
    if (pInValue.Data[0]!=1) // if negative
    {
      _negate(wValue);
      wValue=-wValue;
    }
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_Float :
  {
    float wValue ;
    memmove(&wValue,pInValue.Data+1,sizeof(wValue));
    wValue=_reverseByteOrder_T<float>(wValue);
    if (pInValue.Data[0]!=1) // if negative
    {
      _negate(wValue);
      wValue=-wValue;
    }
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_Double :
  {
    double wValue ;
    memmove(&wValue,pInValue.Data+1,sizeof(wValue));
    wValue=_reverseByteOrder_T<double>(wValue);
    if (pInValue.Data[0]!=1) // if negative
    {
      _negate(wValue);
      wValue=-wValue;
    }
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }
  case ZType_LDouble :
  {
    long double wValue ;
    memmove(&wValue,pInValue.Data+1,sizeof(wValue));
    wValue=_reverseByteOrder_T<long double>(wValue);
    if (pInValue.Data[0]!=1) // if negative
    {
      _negate(wValue);
      wValue=-wValue;
    }
    pOutValue= static_cast<_Tp>(wValue);
    return ZS_SUCCESS;
  }

  default:
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Fatal,
        "Invalid Type for key field to convert data to <%ld> <%s> encountered while processing data conversion",
        wType,
        decode_ZType(wType));
    return ZS_INVTYPE;
  }
  }// switch
}// _castAtomicValue
#endif // __COMMENT__


#ifdef __COMMENT__
/**
 * @brief _getUniversalValue converts pValue from its Natural format to its Universal format
 * @param pValue        value to convert (here template for Pointers)
 * @param pUniversal
 * @param pTargetType
 * @param pSourceType
 * @param pArrayCount
 * @return
 */
template <class _Tp>
ZStatus
_getUniversalValue (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
    ZDataBuffer &pUniversal,
    const ZType_type pDesiredType,
    const long pDesiredArraySize,
    const ZType_type pZType,
    const long pArraySize)
{
  ZStatus wSt;
  ssize_t wArrayCount = pArraySize;
  ZDataBuffer wNaturalValue;
  ZDataBuffer wANaturalValue;
  ZDataBuffer wKeyValue;

  if (pArraySize<1)
  {
    wArrayCount= sizeof(pValue)/sizeof(pValue[0]);
  }
  if (wArrayCount>pDesiredArraySize)  // if array has more elements than key field : truncate to key field
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVSIZE,
        Severity_Warning,
        " given array size for pointer <%ld>  exceeds size of universal array size<%ld>. Array size truncated",
        wArrayCount,
        pDesiredArraySize);
    ZException.printUserMessage(stderr);
    wArrayCount = pDesiredArraySize;
  }
  wNaturalValue.clear();

  if (pZType != pDesiredType )
  {
    for (long wi=0;wi<wArrayCount;wi++)
    {
      wSt=_castAtomicValue_T<typeof(*pValue)>(pValue[wi],pDesiredType,wANaturalValue);
      wNaturalValue.appendData(wANaturalValue);
    }
  }
  else
    wNaturalValue.appendData(&pValue[0],sizeof(pValue));

  wField.RecordOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0
  wField.ArraySize = wArrayCount;      // cheating also for number of array occurence
  _getArrayUfN(wNaturalValue,wKeyValue);  // Extract & pack Array of Atomic Fields ready for Key usage
  if (wSt!=ZS_SUCCESS)
    return wSt;

  pUniversal.setData(wKeyValue); // set the key value

  return wSt;
} // _setKeyFieldValue for array data type
/**
 * @brief _getUniversalValue converts pValue from its Natural format to its Universal format
 * @param pValue        value to convert (here template for Arrays)
 * @param pUniversal
 * @param pTargetType
 * @param pSourceType
 * @param pArrayCount
 * @return
 */
template <class _Tp>
ZStatus
_getUniversalFromNatural (typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue,
    ZDataBuffer &pUniversal,
    const ZType_type pTargetType,
    const long pTargetArrayCount,
    const ZType_type pZType,
    const long pArraySize)
{
  ZStatus wSt;
  ssize_t wArrayCount;
  ZDataBuffer wNaturalValue;
  ZDataBuffer wANaturalValue;
  ZDataBuffer wKeyValue;

  if (pArraySize<1)
  {
    wArrayCount= sizeof(pValue)/sizeof(pValue[0]);
  }
  if (wArrayCount>pTargetArrayCount)  // if array has more elements than key field : truncate to key field
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVSIZE,
        Severity_Warning,
        " given array size <%ld>  exceeds size of universal desired array size<%ld>. Array size truncated.",
        wArrayCount,
        pTargetArrayCount);
    ZException.printUserMessage(stderr);
    wArrayCount = pTargetArrayCount;
  }
  wNaturalValue.clear();

  if (pZType != pTargetType )
  {
    for (long wi=0;wi<wArrayCount;wi++)
    {
      _castAtomicValue<_Tp>(pValue[wi],pTargetType,wANaturalValue);
      wNaturalValue.appendData(wANaturalValue);
    }
  }
  else
    wNaturalValue.appendData(&pValue[0],sizeof(pValue));

  ZIndexField_struct wField;
  wField = pUniversal.ZICB->ZKDic->Tab[pFieldRank] ;
  wField.RecordOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0
  wField.ArraySize = wArrayCount;      // cheating also for number of array occurence
  wSt=_getArrayUfN<>(wNaturalValue,wKeyValue,wField);  // Extract & pack Array of Atomic Fields ready for Key usage
  if (wSt!=ZS_SUCCESS)
    return wSt;

  pUniversal.setFieldRawValue(wKeyValue,pFieldRank); // set the key value

  return wSt;
} // _setKeyFieldValue for array data type



template <class _Tp>
ZStatus
_getUniversalFromNatural (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
    ZDataBuffer & pUniversal,
    const long pZType,
    const long pArrayCount) // array count represents the number of value occurrences the pointer points to
{
  ZStatus wSt;
  long wType=pZType;
  ssize_t wArrayCount=pArrayCount<1?1:pArrayCount;
  /*
ZDataBuffer wKeyValue;


  if (wArrayCount>pUniversal.ZICB->ZKDic->Tab[pFieldRank].ArraySize)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVSIZE,
        Severity_Warning,
        " given array size <%ld> for pointer exceeds size of key field array size<%ld> (field rank <%ld>). Array size truncated to key field array size",
        wArrayCount,
        pUniversal.ZICB->ZKDic->Tab[pFieldRank].ArraySize,
        pFieldRank);
    ZException.printUserMessage(stderr);
    wArrayCount = pUniversal.ZICB->ZKDic->Tab[pFieldRank].ArraySize;
  }
  */
      if (wType == ZType_ByteSeq)
  {
    pUniversal.setData(pValue,wArrayCount,pFieldRank); // set the key value using pArraySize as Length
    return ZS_SUCCESS;
  }

  if (wType == ZType_PointerChar)
  {
    return _setKeyFieldValueCString<_Tp>(pValue,pUniversal,pFieldRank);
    //                        pZKey.appendValue(pValue,pZKey.ZICB->ZKDic->Tab[pFieldRank].KeyFieldSize);  // Extract & pack pointer to String Fields ready for Key usage
    //                        return ZS_SUCCESS;
  }

  wType = wType & ~(ZType_Pointer);



  if ((pZType&ZType_StdString)==ZType_StdString) // pointer to std::string : use appropriate routine
  {
    return _getValueFromStdString<_Tp>(pValue,pUniversal,pFieldRank);
  }


  //    wNaturalValue.setData(pValue,sizeof(pArraySize));  // pValue is a pointer and pArraySize the number of elements

  ZIndexField_struct wField;
  wField = pUniversal.ZICB->ZKDic->Tab[pFieldRank] ;
  wField.RecordOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0

  wSt=_getValueFromPointer<_Tp>(pValue,pUniversal,pZType,pFieldRank,wArrayCount);  // Extract & pack values pointed by pValue to get Fields ready for Key usage

  //    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value
  return wSt;
} // _setKeyFieldValue for pointers


template <class _Tp>
ZStatus
_getUniversalFromNatural (typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
    ZKey & pZKey,
    const long pZType,
    const long pFieldRank,
    const long  &pArrayNumber)
{
  ssize_t wSize = sizeof(pValue);
  // now extracting without conversion
  if ((pZType&ZType_StdString)==ZType_StdString)
  {
    return _setKeyFieldValueStdString<_Tp>(&pValue,pZKey,pFieldRank);
  }

  if (sizeof(pValue)>pZKey.ZICB->ZKDic->Tab[pFieldRank].InternalSize)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVSIZE,
        Severity_Warning,
        " given class size <%ld> exceeds size of key field size<%ld> (field rank <%ld>). Array size truncated to key field array size",
        wSize,
        pZKey.ZICB->ZKDic->Tab[pFieldRank].InternalSize,
        pFieldRank);
    ZException.printUserMessage(stderr);
    wSize = pZKey.ZICB->ZKDic->Tab[pFieldRank].InternalSize;
  }

  //    pZKey.appendValue(&pValue,wSize,pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank)); // set the key value
  pZKey.setFieldRawValue(&pValue,wSize,pFieldRank);// set the key raw value
  return ZS_SUCCESS;
}

template <class _Tp>
/**
 * @brief zsetKeyFieldValue sets the Key field with pValue. Does all controls and calls specialised routines according ZType_type and size of input data
 * @param[in] pValue    input data value of type (_Tp) defined by template parameter
 * @param[in] pZKey     ZKey object owning the key dictionary that will get the key fields values converted to internal key format.
 * @param[in] pFieldRank    Key dictionary field rank. Could be obtained by field name @see CZKeyDictionary::zgetFieldRank()
 * @param[in] pArraySize    For a field with type pointer : number of atomic data elements pointer points to. For an array, pArraySize is deduced.
 *                          Defaulted to -1. For all other data types, and if pArraySize value is < 1, this value is set to 1.
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
zgetUniversalFromNatural_T (_Tp pValue, ZDataBuffer & pUniversal,const long pFieldRank,ssize_t pArraySize)
{
  ZStatus wSt;

  long wType;
  ssize_t wNaturalSize, wUniversalSize,wArraySize;

  /*    if ((pFieldRank<0)||(pFieldRank> pZKey.ZICB->ZKDic->lastIdx()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_OUTBOUND,
                                Severity_Severe,
                                " Field rank is out of key dictionary boundaries. given value is <%ld> while dictionary limits are <0> to <%s>",
                                pFieldRank,
                                pZKey.ZICB->ZKDic->lastIdx());
        return ZS_OUTBOUND;
        }
*/
  //first get ZType_type of input data and test its compatibility with target key field

  wSt=zgetZType_T<_Tp>(wType,wNaturalSize,wUniversalSize,pArraySize,false) ; // with tolerance to pointers
  if (wSt!=ZS_SUCCESS)
    return wSt;
  /*
    wSt=testUniversalTypes(wType, pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType);
    if (wSt!=ZS_SUCCESS)
        {
        return wSt;
        }
    */

  pUniversal.allocate(wUniversalSize);


  // now extracting and converting

  if ((wType & ZType_StdString)==ZType_StdString)
  {
    wSt= _setKeyFieldValueStdString<_Tp>(&pValue,pZKey,pFieldRank); // addresses the direct reference and the pointer to std::string
  }
  else
    wSt=_getUniversalFromNatural<_Tp>(pValue,pUniversal,wType,pFieldRank,pArraySize);

  if (wSt!=ZS_SUCCESS)
    return wSt;

} //zsetKeyFieldValue

/** @endcond */

/**
 * @brief ZKey::setFieldValue loads the value (auto) pValue to the key field (defined in key dictionary) of rank pFieldRank
 *
 * @note GNU C++ compiler converts arrays to pointers when using 'auto' clause.
 * In case of arrays, use template ZKey::setFieldValueT
 * @note
 * pArraySize is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArraysize is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArraysize is not necessary because deduced from size of string.
 *
 *@bug the default value of -1 for pArraySize does not work with GNU compiler because of the use of 'auto' clause.
 * so that, pArraySize should systematically be mentionned
 *
 * This routine controls the coherence of given data.
 * - It makes the data conversion whenever required to have a dictionary defined data (natural data).
 * - It makes the conversion from natural field to internal key field value format.
 * - It addresses pointers vs arrays.
 * - It pads the key if the size of input data (specifically for arrays) is not large enough.
 * - It truncates the data if given value exceeds defined key field boundaries.
 *
 * @param[in] pFieldRank field order (rank) in key dictionary
 * @param[in] pValue  Value of undertermined type to set the field with
 * @param[in] pArraySize Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
zgetUniversalFromNatural (auto pValue,ZDataBuffer& pUniversal ,long &pArraySize)
{
  return zgetUniversalFromNatural_T<typeof(pValue)>(pValue,pUniversal,pArraySize);
}
/**
 * @brief ZKey::setFieldValue loads the value (auto) pValue to the key field (defined in key dictionary)
 * with a name that matches given string.
 *
 *@bug the default value of -1 for pArraySize does not work with GNU compiler because of the use of 'auto' clause.
 * so that, pArraySize should systematically be mentionned
 *
 * @note GNU C++ compiler converts arrays to pointers when using 'auto' clause.
 * In case of arrays, use template ZKey::setFieldValueT
 * @note
 * pArraySize is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArraysize is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArraysize is not necessary because deduced from size of string.
 *
 * This routine controls the coherence of given data.
 * - It makes the data conversion whenever required to have a dictionary defined data (natural data).
 * - It makes the conversion from natural field to internal key field value format.
 * - It addresses pointers vs arrays.
 * - It pads the key if the size of input data (specifically for arrays) is not large enough.
 * - It truncates the data if given value exceeds defined key field boundaries.
 *
 * @param[in] pFieldRank field order (rank) in key dictionary
 * @param[in] pValue  Value of undertermined type to set the field with
 * @param[in] pArraySize Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
ZKey::setFieldValue (const char* pFieldName,auto pValue, const long pArraySize)
{
  long wFieldRank=ZICB->ZKDic->zsearchFieldByName(pFieldName);
  if (wFieldRank<0)
    return ZException.getLastStatus();
  return zsetKeyFieldValue<typeof(pValue)>(pValue,*this,wFieldRank,pArraySize);
}




template <class _Tp>
/**
 * @brief ZKey::setFieldValueT Template routine that loads the value (of type _Tp) pValue to the key field (defined in key dictionary) of rank pFieldRank
 * @note
 * Template is necessary in case of an array to keep the native data type.
 * @note
 * pArraySize is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArraysize is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArraysize is not necessary because deduced from size of string.
 *
 * This routine controls the coherence of given data.
 * - It makes the data conversion whenever required to have a dictionary defined data (natural data).
 * - It makes the conversion from natural field to internal key field value format.
 * - It addresses pointers vs arrays.
 * - It pads the key if the size of input data (specifically for arrays) is not large enough.
 * - It truncates the data if given value exceeds defined key field boundaries.
 *
 * @param[in] pFieldRank field order (rank) in key dictionary
 * @param[in] pValue  Value of undertermined type to set the field with
 * @param[in] pArraySize Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 * @return
 */
ZStatus
ZKey::setFieldValueT (const long pFieldRank,_Tp pValue, const long pArraySize)
{
  return zsetKeyFieldValue<_Tp>(pValue,*this,pFieldRank,pArraySize);
}
template <class _Tp>
/**
 * @brief ZKey::setFieldValueT Template routine that loads the value (of type _Tp) pValue to the key field (defined in key dictionary)
 *  with a name that matches given string.
 * @note
 * Template is necessary in case of an array to keep the native data type.
 * @note
 * pArraySize is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArraysize is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArraysize is not necessary because deduced from size of string.
 *
 * This routine controls the coherence of given data.
 * - It makes the data conversion whenever required to have a dictionary defined data (natural data).
 * - It makes the conversion from natural field to internal key field value format.
 * - It addresses pointers vs arrays.
 * - It pads the key if the size of input data (specifically for arrays) is not large enough.
 * - It truncates the data if given value exceeds defined key field boundaries.
 *
 * @param[in] pFieldRank field order (rank) in key dictionary
 * @param[in] pValue  Value of undertermined type to set the field with
 * @param[in] pArraySize Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 * @return
 */
ZStatus
ZKey::setFieldValueT (const char* pFieldName,_Tp pValue, const long pArraySize)
{
  long wFieldRank=ZICB->ZKDic->zsearchFieldByName(pFieldName);
  if (wFieldRank<0)
    return ZException.getLastStatus();
  return zsetKeyFieldValue<_Tp>(pValue,*this,wFieldRank,pArraySize);
}



//-------------------End Build Key from Value routines-----------------------------------

#endif // __COMMENT__
/** @} */ // end Development



#endif // ZDATACONVERSION_H
