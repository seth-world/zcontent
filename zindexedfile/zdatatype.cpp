#ifndef ZDATATYPE_CPP
#define ZDATATYPE_CPP

#include <zindexedfile/zindextype.h>
#include <zindexedfile/zrawindexfile.h>
#include <zindexedfile/zdataconversion.h>

/*
 *  get urf size from ZType
 *
 *  get urf value from natural value
 *
 *  get natural value from urf
 *
 *
 */



size_t getAtomicUniversalSize(const ZTypeBase pType)
{
ZTypeBase wZType = pType & ZType_AtomicMask;

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
}//getAtomicUniversalSize


/**
 * @brief getAtomicNaturalSize returns the Atomic natural size of the type.
 * If array, only the element atomic size is returned.
 * @param pType
 * @return
 */

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
return 0;
}//getNaturalSize




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
ZStatus _getByteSequenceFromRecord (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp>  pInData, const ssize_t pSize, ZDataBuffer &pOutData, ZIndexField& pField)
{
    pOutData.setData(pInData,pSize);
    return ZS_SUCCESS;
}


template <class _Tp>
ZStatus _getByteSeqUfN_T_Ptr(unsigned char* pInData,
                         ZDataBuffer     &pOutData,
                         const ZType_type pType,
                         URF_UnitCount_type &pSize)
{
uint64_t wSize;

    if (pType & ZType_Blob)
    {
                ZDataBuffer*wZDB=(ZDataBuffer*)pInData;
                wSize = (URF_UnitCount_type)wZDB->Size;
                pOutData.setData(wZDB->Data,wSize);
                return ZS_SUCCESS;
    }
    if (pType & ZType_bitset)
    {
                ZBitset*wZDB=(ZBitset*)pInData;
                wSize = (URF_UnitCount_type)(wZDB->Size*wZDB->UnitByteSize);
                pOutData.setData(wZDB->bit,wSize);
                return ZS_SUCCESS;
    }
    return ZS_INVTYPE;
} // _getByteSeqUfN_T_Ptr


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
getAtomicZType_Sizes(ZTypeBase pType,ssize_t& pNaturalSize,ssize_t& pUniversalSize) // not a template
{
    switch ( pType & ZType_AtomicMask)
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






ZStatus _getBlobType(void*pValue, ZTypeBase &pType, size_t &pNaturalSize , size_t &pUniversalSize , URF_Array_Count_type &pArrayCount)
{
    pType=ZType_Blob;
    ZDataBuffer* wZDB = static_cast<ZDataBuffer*>(pValue);
    pNaturalSize = pUniversalSize =wZDB->Size;
    pArrayCount=1;
    return ZS_SUCCESS;
}
/*
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
*/
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
ZStatus _getBitsetTypeFull(void*pValue, ZTypeBase &pType, uint64_t &pNaturalSize , uint64_t&pUniversalSize , URF_Array_Count_type &pArrayCount)
{
    pType=ZType_bitset;
    ZBitset* wBS = static_cast<ZBitset*>(pValue);
    pNaturalSize = pUniversalSize =(size_t) (wBS->Size*wBS->UnitByteSize);
    pArrayCount=(URF_Array_Count_type)wBS->Size;
    return ZS_SUCCESS;
}

size_t
getAtomicURFSize(ZTypeBase pType) {
  if (pType & ZType_Atomic)
    return    sizeof(ZTypeBase) + getAtomicUniversalSize(pType);
  return    sizeof(ZTypeBase) + getAtomicUniversalSize(pType);
}
#endif // ZDATATYPE_CPP
