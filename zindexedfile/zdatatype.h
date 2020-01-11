#ifndef ZDATATYPE_H
#define ZDATATYPE_H

#include <zconfig.h>
#include <type_traits>
#include <ztoolset/zerror.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zbasedatatypes.h>
#include <ztoolset/zexceptionmin.h>
#include <zrandomfile/zrandomfiletypes.h>
#include <cstdint>
#include <ztoolset/zatomicconvert.h>

#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/utffixedstring.h>

#include <ztoolset/uristring.h>
#include <zcrypt/checksum.h>

#include <ztoolset/charman.h>

namespace zbs
{
/**
@defgroup DATAFORMATGROUP Data types and conversions for file record storage and indexing
@{

  @weakgroup Data conversion from natural to universal format et from universal format to natural

  @par data formats : Natural vs Universal

  Because of Endian conversion, because of sign bit, numeric data cannot be sorted widely.
  It has been necessary to define a universal data format that gets rid of those elements

  - Natural : format of data used by local computer for the object.
            Subject to endian. Not capable of being binary sorted appropriately.

  - Universal : format used to store data on file and index in an independant way.
            Not subject to endian : Universal format is big endian regardless the endianness of the current platform.
            May be sorted even as an agregate of data.  <-- Important
            Is used for index field(s).


  - Universal Record Format : format used to store fields content
        It is composed of an URF header describing the data, followed by data in natural format.

  - Universal Varying Format : Used for storing Fixed strings (and only strings). (see utftemplatestring.h _exportUVF
            This is a simplified URF format.
            It is used to save memory and faster the usage.
        Universal Varying  Format stores fixed string data into a varying length string container excluding '\0' character terminator
        leaded by an uint16_t mentionning the number of character units of the string that follows.


   for more see @DATA_FORMAT  (file  znaturalfromurf.h )


  @par Data conversion

  1-Get the type (ZType_type) of data to convert:
    1.1- from natural data to universal
        Type is deduced from data using specific routines :
        main entry point is template zgetZType

    1.2- from universal to natural
        Type must be given.

  2- Conversion

  2.1- adHoc conversion : target ZType_type [and Array Count] is reputed to be same as source data to be converted.
  Source = Target

  2.2- conversion against a dictionary (key dictionary, record dictionary)

  -source data type has to be casted to target type if data types are different.
  -For arrays (and adhoc pointers):
    Source array count is compared to target array count and eventually truncated.
    Unary data is casted to target type whenever necessary (see preceeding)


  B/ Universal Storage format

  In addition to preceeding, file record data is stored using an appropriate header that mentions

  - ZType_type in any case
  - Eventually (data is not atomic) its total size :
    . for an array : array count is deduced from total size : array count = total size/sizeof(atomic data)

  - for a blob (byteseq), size is int64_t



  Conversion routines

  Main entry points

  1- Adhoc conversion

  zgetUniversalFromNatural  : converts a natural data into Universal format

  zgetNaturalfromUniversal  : converts back a Universal data to its natural format according its ZType_type

  2- With dictionary constraints

  zgetUniversalFromNatural_W

  zgetNaturalfromUniversal_W
  */
} // namespace zbs
// see <zindexedfile/znaturalfromurf.cpp>
ZStatus _getURFHeaderData(unsigned char* pURF_Ptr,
                                 ZTypeBase &pZType,
                                 uint64_t &pUniversalSize,
                                 uint64_t &pNaturalSize,
                                 uint16_t &pCanonical,
                                 uint16_t &pEffectiveUSize,
                                 uint64_t &pHeaderSize,
                                 unsigned char **pDataPtr);
// see <zindexedfile/znaturalfromurf.cpp>
size_t  _getURFHeaderSize (ZTypeBase &pZType);

ZStatus checkFieldsTypes (const ZTypeBase pTypeIn, const ZTypeBase pTargetType);

ZStatus getAtomicZType_Sizes(ZTypeBase pType, uint64_t& pNaturalSize, uint64_t& pUniversalSize);

size_t getAtomicNaturalSize(const ZTypeBase pType);
size_t getAtomicUniversalSize(const ZTypeBase pType);

//ZStatus _castAtomicValue_A (auto pValue,const ZType_type pTargetType,ZDataBuffer &pOutValue);


ZStatus _getAtomicUfN(ZDataBuffer &pInData,ZDataBuffer &pOutData,const ZType_type pType);

struct ZType_struct{
    ZType_type Type;
};

class CType : public ZType_struct
{
public:
    typedef ZType_struct _Base;
    CType() {}
    CType(ZType_type pType) {_Base::Type=pType;}

    bool isAtomic() {return (Type&ZType_Atomic);}
    bool isByteSeq() {return (Type&ZType_ByteSeq);}
    bool isString() {return (Type&ZType_String);}
    bool isVarying() {return (Type&ZType_VaryingLength);}
    bool isEndian() {return (Type&ZType_Endian);}


    int32_t getStructure() {return Type&ZType_StructureMask; }
    int32_t getAtomicType() {return Type&ZType_AtomicMask;}

    uint64_t getAtomicUniversalSize()
    {
        uint64_t wNaturalSize,wUniversalSize;

        if (getAtomicZType_Sizes(getAtomicType(),wNaturalSize,wUniversalSize)!=ZS_SUCCESS)
                                                                ZException.exit_abort();
        return wUniversalSize;
    }
    uint64_t getAtomicNaturalSize()
    {
        uint64_t wNaturalSize,wUniversalSize;

        if (getAtomicZType_Sizes(getAtomicType(),wNaturalSize,wUniversalSize)!=ZS_SUCCESS)
                                                                ZException.exit_abort();
        return wNaturalSize;
    }

    CType* extract (unsigned char* pPtr)
    {
        memmove (&Type,pPtr,sizeof(ZTypeBase));
        return this;
    }
};

/** @} */



using namespace zbs ;

typedef utfdescString ZFieldName_type ;



//--------------- Data conversions-----------------
//
//------------Demangling (according platform)-------------------------

/* for demangling see ztypetype.h */

#ifdef __COMMENT__
#ifdef __GNUG__
#include <memory>
#include <cxxabi.h>

#endif// __GNUG__
#ifdef __USE_WINDOWS__

#include <Windows.h>
#include <Dbghelp.h>

#endif // __USE_WINDOWS__
void
typeDemangler(const char*pName,utfdescString &pOutName);
void
typeDemangler(const char*pName, char **pOutName);
#endif // __COMMENT__
#include <ztoolset/zbitset.h>
//#include <ztoolset/zwstrings.h>

//-------- Data type constant values-----------------------------

static const size_t CharType = typeid(char).hash_code();
static const size_t WCharType = typeid(wchar_t).hash_code();
static const size_t UCharType = typeid(unsigned char).hash_code();
static const size_t WUCharType = typeid(unsigned wchar_t).hash_code();

static const size_t S8Type = typeid(int8_t).hash_code();
static const size_t U8Type= typeid(uint8_t).hash_code();
static const size_t S16Type= typeid(int16_t).hash_code();
static const size_t U16Type= typeid(uint16_t).hash_code();
static const size_t S32Type= typeid(int32_t).hash_code();
static const size_t U32Type= typeid(uint32_t).hash_code();
static const size_t S64Type= typeid(int64_t).hash_code();
static const size_t U64Type= typeid(uint64_t).hash_code();

static const size_t FloatType= typeid(float).hash_code();
static const size_t DoubleType= typeid(double).hash_code();
static const size_t LDoubleType= typeid(long double).hash_code();

static const size_t PtrCharType= typeid(char *).hash_code();
static const size_t PtrUCharType= typeid(unsigned char *).hash_code();

static const size_t PtrArrayCharType= typeid(char []).hash_code();
static const size_t PtrArrayUCharType= typeid(unsigned char []).hash_code();

static const size_t Prohibit= typeid(void*).hash_code();

static const size_t descStringType= typeid(utfdescString).hash_code();
static const size_t codeStringType= typeid(utfcodeString).hash_code();
static const size_t uriStringType= typeid(uriString).hash_code();
static const size_t messageStringType= typeid(utfmessageString).hash_code();
static const size_t keywordStringType= typeid(utfkeywordString).hash_code();
static const size_t identityStringType= typeid(utfidentityString).hash_code();

/*static const size_t descWStringType= typeid(descWString).hash_code();
static const size_t codeWStringType= typeid(codeWString).hash_code();
//static const size_t uriWStringType= typeid(uriString).hash_code();
static const size_t messageWStringType= typeid(messageWString).hash_code();
static const size_t keywordWStringType= typeid(keywordWString).hash_code();
static const size_t identityWStringType= typeid(identityWString).hash_code();*/

//static const size_t varyingCStringType= typeid(varyingCString).hash_code();
//static const size_t varyingWStringType= typeid(varyingWString).hash_code();

static const size_t utf8varyingStringType= typeid(utf8VaryingString).hash_code();
static const size_t utf16varyingStringType= typeid(utf16VaryingString).hash_code();
static const size_t utf32varyingStringType= typeid(utf32VaryingString).hash_code();

static const size_t checksumType= typeid(checkSum).hash_code();
static const size_t bitsetType= typeid(ZBitset).hash_code();

static const size_t zdatabufferType= typeid(ZDataBuffer).hash_code();

static const size_t stdStringType= typeid(std::string).hash_code();
static const size_t stdWStringType= typeid(std::wstring).hash_code();

static const size_t zdateType= typeid(ZDate).hash_code();
static const size_t zdateFullType= typeid(ZDateFull).hash_code();

//---------- Data type acquisition ----------------------------
//



template <class _Tp> // must be template because called by conditional templates
/**
 * @brief _getAtomicZTypeT template to analyze a type hashcode and deduce ZType_Type and natural and internal sizes for an atomic data
 * This function must be a template because called by template routines
 *
 * @param[in] pTypeHashCode  type hashcode obtained using typeid(VAR).hash_code(). VAR must be atomic.
 * @param[out] pType         deduced ZType_type
 * @param[out] pNaturalSize  Resulting natural size of the variable
 * @param[out] pKeyFieldSize Resulting internal size of the variable
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
_getAtomicZType_T(const size_t pTypeHashCode, ZTypeBase &pType, size_t& pNaturalSize, size_t& pUniversalSize)
{

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
                    pUniversalSize = pNaturalSize;  // array of char is a special case because no sign byte is added
                    break;
                    }
    if (pTypeHashCode==WCharType)
                    {
                    pType |= ZType_WChar ;
                    pNaturalSize=sizeof(wchar_t);
                    pUniversalSize=pNaturalSize;
                    break;
                    }
    if (pTypeHashCode==WUCharType)
                    {
                    pType |= ZType_WUChar ;
                    pNaturalSize=sizeof(unsigned wchar_t);
                    pUniversalSize=pNaturalSize;
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
    pUniversalSize = -1;
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_INVTYPE,
                          Severity_Severe,
                          "Atomic type of data hashcode <%lX> cannot not be recognized",pTypeHashCode);
    return (ZS_INVTYPE);

    }// while
    pUniversalSize = pNaturalSize;
    if (pType & ZType_Signed)
                pUniversalSize = pUniversalSize+ 1; // take care of sign byte

    return(ZS_SUCCESS);
}// _getAtomicZTypeT generic - template


//-----------------Key Dictionary definition--------------------------------
// --------------- Key field type ------------------------------------------

// following is no more used see conditional template _getZTypeT function
//static inline
//ZStatus _getZType(auto pValue, ZType_type &pType, ssize_t &pNaturalSize, ssize_t pKeySize,bool pFromRecord=true);



template <typename _Tp>
ZStatus
_getZTypeStdString (const void *pValue,
                    ZTypeBase &pType,
                    size_t &pNaturalSize,
                    size_t &pUniversalSize,
                    URF_Array_Count_type &pArrayCount)
{
const std::string* wString = static_cast<const std::string*>(pValue);
  pType = ZType_StdString ;

  pArrayCount = wString->size();
  pNaturalSize = pArrayCount;
  pUniversalSize= pArrayCount;
  return ZS_SUCCESS;
}
template <typename _Tp>
ZStatus
_getZTypeStdWString (const void *pValue,
                    ZTypeBase &pType,
                    size_t &pNaturalSize,
                    size_t &pUniversalSize,
                    URF_Array_Count_type &pArrayCount)
{
const std::wstring* wString = static_cast<const std::wstring*>(pValue);
  pType = ZType_StdWString ;

  pArrayCount = wString->size()+1;
  pNaturalSize = pArrayCount*sizeof(wchar_t);
  pUniversalSize= pNaturalSize;
  return ZS_SUCCESS;
}
template <typename _Tp>
ZStatus
_getZTypeCString (const void *pValue,
                  ZTypeBase &pType,
                  size_t &pNaturalSize,
                  size_t &pUniversalSize,
                  URF_Array_Count_type &pArraySize)
{
const char* wString = static_cast<const char*>(pValue);
  pType = ZType_CString ;

  pArraySize = strlen(wString);
  pNaturalSize = pArraySize;
  pUniversalSize= pArraySize;
  return ZS_SUCCESS;
}


template <typename _Tp>
ZStatus
_getZTypeFixedString (void *pValue,
                      ZTypeBase &pType,
                      size_t &pNaturalSize,
                      size_t &pUniversalSize,
                      URF_Array_Count_type &pArrayCount)
{

utfStringHeader* wString = static_cast<utfStringHeader*>(pValue);

//size_t wSize=wString->capacity();

  pType =  wString->ZType;

  pArrayCount = (URF_Array_Count_type)wString->getUnitCount();
  pNaturalSize = wString->getByteSize();
  pUniversalSize= wString->getByteSize();
  return ZS_SUCCESS;
}

template <typename _Tp>
ZStatus
_getUtfStringData (void *pValue,
                      ZTypeBase &pType,
                      size_t &pNaturalSize,
                      size_t &pUniversalSize,
                      URF_Array_Count_type &pUnitCount)
{

utfStringDescriptor* wString = static_cast<utfStringDescriptor*>(pValue);

  pType =wString->getZType();
  pUnitCount = wString->getUnitCount();
  pNaturalSize = wString->getByteSize();
  pUniversalSize= wString->getByteSize();
  return ZS_SUCCESS;
}
/*
template <typename _Tp>
ZStatus
_getZTypeVaryingWString (void *pValue,
                      ZTypeBase &pType,
                      size_t &pNaturalSize,
                      size_t &pUniversalSize,
                      uint16_t &pArrayCount)
{

varyingWString* wString = static_cast<varyingWString*>(pValue);

//size_t wSize=wString->capacity();
  pType = ZType_VaryingWString ;

  pArrayCount = wString->Size/sizeof(wchar_t);
  pNaturalSize = wString->Size;
  pUniversalSize= wString->Size;
  return ZS_SUCCESS;
}
*/
/**================================_getZType templates===========================================================
 *
 */
template <typename _Tp>
/**
 * @brief _getZTypeT template function : from a given value (pValue) with template type _Tp analyzes the type of data and :
   -  generates a ZType_type mask returned in pType (long),
   - returns
      + a Natural size (roughly sizeof(_Tp))
      + Key size for the data (depending on Sign byte etc.)
      + Array size (number of elements in array or 1 if atomic data or Class)

There is one function (conditional overloads) for
  - atomic data : integral and floating point
  - array : for array, analyzed type is done additionally for the atomic data type of contained elements
  - pointer: for pointer, analyzed type contains additionally the atomic data type to which the pointer points to.
  - class (non polymorphic class & struct)

 * @param[in]  pValue the value of the data field to be analyzed
 * @param[out] pType resulting ZType_type data type mask
 * @param[out] pNaturalSize resulting natural total size of the array
 * @param[out] pKeyFieldSize  resulting (Key) internal total size of the data field
 * @param[out] pArraySize   Number of occurrences withing the array if data type is an array or 1 if atomic data or Class
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
_getZType_T(typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
           ZTypeBase &pType,
           size_t &pNaturalSize,
           size_t &pKeyFieldSize,
           URF_Array_Count_type &pArraySize)
{
    pType = ZType_Atomic ; // set pType (ZType_type mask) to zero (ZType_Nothing)
    pArraySize = 1;
    const size_t wTypeHashCode = typeid(pValue).hash_code();
    if (wTypeHashCode==typeid(void).hash_code())
                {
                ZException.setMessage  (_GET_FUNCTION_NAME_,
                                          ZS_INVTYPE,
                                          Severity_Severe,
                                          "Invalid data type requested <void> ");

                return ZS_INVTYPE ;
                }

    return _getAtomicZType_T<_Tp>(wTypeHashCode,pType,pNaturalSize,pKeyFieldSize);

}// _getZType_T integral + floating point (fundamental excepted std::nullptr_t and void data types)

template <typename _Tp>
ZStatus
_getZType_T(typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
           ZTypeBase &pType,
           size_t &pNaturalSize,
           size_t &pUniversalSize,
           URF_Array_Count_type &pArrayCount)
{
ZStatus wSt;
ZTypeBase wType;

    pType = ZType_Pointer;
    if (typeid(std::string).hash_code()==typeid(*pValue).hash_code())  // address pointer to std::string
    {
    return _getZTypeStdString<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pArrayCount) ;
    }
    size_t wAtomicNaturalSize;
    size_t wAtomicUniversalSize;
    wSt=_getAtomicZType_T<_Tp>(typeid(*pValue).hash_code(),wType,wAtomicNaturalSize,wAtomicUniversalSize); // analyze first array element's Atomic data type
    if (wSt!=ZS_SUCCESS)
                return wSt;
    pType |= wType;

    if (wType & ZType_Char)
                {
                return _getZTypeCString<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pArrayCount);
//                return ZS_SUCCESS;
                }
    pNaturalSize = wAtomicNaturalSize;
//    pArraySize = 1 ;
    pUniversalSize =  wAtomicUniversalSize ;

    return ZS_SUCCESS;

} // _getZType_T for Pointer



template <typename _Tp>
/**
 * @brief _getZType_T get ZType template for Array. That far we are processing an Array.
 *    So we will get of what is made this array.
 *    Length of the field will be length of array * unitary size*
 * @param[in]  pValue the value of the data field to be analyzed
 * @param[out] pType resulting ZType_type data type mask
 * @param[out] pNaturalSize resulting natural total size of the array
 * @param[out] pKeyFieldSize  resulting (Key) internal total size of the array
 * @param[out] pArraySize   Number of occurrences withing the array
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
_getZType_T(typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue,
           ZTypeBase &pType,        //!< Field type mask using ZType_type
           size_t &pNaturalSize,    //!< total field natural size (External size)
           size_t &pUniversalSize,  //!< total field internal format size ( may differ from natural size according sign byte )
           URF_Array_Count_type &pArrayCount)   //!< Number or array elementsbool pRecordUse)
{
ZStatus wSt;
 //   long wNaturalSize = 0;  //! total field natural size
 //   long wKeySize = 0;      //! unary element size within Key ( may differ according sign byte )
ZTypeBase wType;
    pType = ZType_Array ; // set pType (ZType_type mask) to zero (ZType_Nothing)
    size_t wAtomicNaturalSize;
    size_t wAtomicUniversalSize;
    wSt=_getAtomicZType_T<_Tp>(typeid(pValue[0]).hash_code(),wType,wAtomicNaturalSize,wAtomicUniversalSize); // analyze first array element's Atomic data type
    if (wSt!=ZS_SUCCESS)
                return wSt;
    pNaturalSize = sizeof (_Tp);
    pArrayCount = (uint16_t ) ((float)pNaturalSize / (float)wAtomicNaturalSize );
    pUniversalSize = ((size_t)pArrayCount) * wAtomicUniversalSize ;
    pType |= wType ;
    return(ZS_SUCCESS);

}// _getZTypeT for Array

//--------------Enum will not be allowed anymore : deprecated and not useful----------------

#ifdef __COMMENT__
template <typename _Tp>
/**
 * @brief _getZTypeT get ZType template for Enum.
 *
 *  Enums are encoded in a long
 *
 * @param[in]  pValue the value of the data field to be analyzed
 * @param[out] pType resulting ZType_type data type mask
 * @param[out] pNaturalSize resulting natural total size of the array
 * @param[out] pKeyFieldSize  resulting (Key) internal total size of the array
 * @param[out] pArraySize   Number of occurrences withing the array
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
//_getZTypeT(typename std::enable_if_t<std::is_integral<_Tp>::value,_Tp> &pValue,long &pType_mask,ssize_t& pLength,bool pFromRecord)
_getZTypeT(typename std::enable_if_t<std::is_enum<_Tp>::value,_Tp> &pValue,
           ZTypeBase pType,             //!< Field type mask using ZType_type
           ssize_t &pNaturalSize,   //!< total field natural size (External size)
           ssize_t &pKeyFieldSize,  //!< total field internal format size ( may differ from natural size according sign byte )
           long &pArraySize)        //!< Number or array elements
{
ZStatus wSt;
    long wNaturalSize = 0;  // total field natural size
    long wKeyFieldSize = 0;      // unary element size within Key ( may differ according sign byte )
    pType = ZType_Enum ; // set pType (ZType_type mask) to zero (ZType_Nothing)


    ssize_t wSize = sizeof(pValue);

    while(true)
    {
    if (wSize== sizeof(int8_t))
                {
                pType |= ZType_S8&(~ZType_NegateStructure)  ;
                break ;
                }
    if (wSize== sizeof(int16_t))
                {
                pType |= ZType_S16&(~ZType_NegateStructure)  ;
                break ; ;
                }
    if (wSize== sizeof(int32_t))
                {
                pType |= ZType_S32 & (~ZType_NegateStructure)  ;  // ZType_EnumLong
                break ;
                }
    if (wSize== sizeof(int64_t))
                {
                pType |= ZType_S64 & (~ZType_NegateStructure)  ;
                break ;
                }

    pType = ZType_Enum|ZType_Unknown ;
    break;
    }//while true

    pNaturalSize = wSize;
    pKeyFieldSize = wSize + 1;

    pArraySize=1;
    return(ZS_SUCCESS);

/*
    long wValue=0;    // this is a long

    wSt=_getAtomicZTypeT<_Tp>(typeid(wValue).hash_code(),pType,pNaturalSize,pKeyFieldSize); // analyze first array element's Atomic data type
    if (wSt!=ZS_SUCCESS)
                return wSt;

*/
}// _getZTypeT for Enum

#endif // __COMMENT__
//#include <ztoolset/zutfstrings.h>
ZStatus _getFixedStringType(void*pValue,
                            ZTypeBase &pType,
                            size_t &pNaturalSize ,
                            size_t &pUniversalSize ,
                            URF_Array_Count_type &pArrayCount);
//ZStatus _getFixedWStringType(void*pValue, ZTypeBase &pType, size_t &pNaturalSize , size_t &pUniversalSize , URF_Array_Count_type &pArrayCount);

ZStatus _getBlobType(void*pValue,ZTypeBase &pType,size_t &pNaturalSize , size_t &pUniversalSize ,URF_Array_Count_type &pArrayCount);

ZStatus _getBitsetType(void*pValue, ZTypeBase &pType, uint64_t &pNaturalSize , uint64_t&pUniversalSize , URF_Array_Count_type &pArrayCount);
/** @brief _getZTypeT get ZType template for Class type ( class or struct ).
 *  It does nothing : it is there to insure std::enable_if <> coherence while compiling
 */
template <typename _Tp>
ZStatus
_getZType_T(typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
            ZTypeBase &pType,
            size_t &pNaturalSize,
            size_t &pUniversalSize,
            URF_Array_Count_type &pUnitCount)
{

    pType = ZType_Class ;
    size_t wTypeHashCode=typeid(pValue).hash_code();
    pUnitCount = 1;
    if (wTypeHashCode==stdStringType)
        {
        return _getZTypeStdString<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
        }
    if (typeid(pValue).hash_code()==stdWStringType)
        {
        return _getZTypeStdWString<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
        }
//===============Fixed utf strings=================================

    if (wTypeHashCode==descStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
/*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_desclen+1;
        return ZS_SUCCESS ;*/
        }
    if (wTypeHashCode==codeStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
/*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_codelen+1;
        return ZS_SUCCESS ;*/
        }
    if (wTypeHashCode==uriStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
/*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_urilen+1;
        return ZS_SUCCESS ;*/
        }
    if (wTypeHashCode==messageStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
/*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_messagelen+1;
        return ZS_SUCCESS ;*/
        }
    if (wTypeHashCode==keywordStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
/*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_keywordlen+1;
        return ZS_SUCCESS ;*/
        }
    if (wTypeHashCode==identityStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
/*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_identitylen+1;
        return ZS_SUCCESS ;*/
        }
     //===============Varying utf strings=================================
    if (wTypeHashCode==utf8varyingStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
        }
    if (wTypeHashCode==utf16varyingStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
        }
    if (wTypeHashCode==utf32varyingStringType)
        {
        return _getUtfStringData<_Tp>(&pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
        }
    if (wTypeHashCode==checksumType)
        {
        pType=ZType_CheckSum;
        pNaturalSize = pUniversalSize =pUnitCount = cst_checksum;
        return ZS_SUCCESS ;
        }
    if (wTypeHashCode==bitsetType)
        {
        pType=ZType_bitset;
        return _getBitsetType(&pValue,pType,pNaturalSize , pUniversalSize ,pUnitCount);
        return ZS_SUCCESS ;
        }
    if (wTypeHashCode==zdatabufferType)
        {
        pType=ZType_Blob;
        return _getBlobType(&pValue,pType,pNaturalSize , pUniversalSize ,pUnitCount);
        }

    if (wTypeHashCode==zdateType)
        {
        pType=ZType_ZDate;
        pNaturalSize = pUniversalSize = sizeof(uint32_t);
        pUnitCount =1;
        return ZS_SUCCESS ;
        }
    if (wTypeHashCode==zdateFullType)
        {
        pType=ZType_ZDateFull;
        pNaturalSize = pUniversalSize = sizeof(uint64_t);
        pUnitCount = 1;
        return ZS_SUCCESS ;
        }

    pType=ZType_Unknown;
    pNaturalSize = pUniversalSize =pUnitCount=0;

    char wOutName[50];
    char*wOutNamePtr= (char*)wOutName;
    memset(wOutName,0,50);
    typeDemangler(typeid(_Tp).name(),wOutNamePtr,50);
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_INVTYPE,
                          Severity_Severe,
                          " Unrecognized field data type  <%s> (before demangling <%s>) hascode <%lX.",
                          wOutName,
                          typeid(_Tp).name(),
                          typeid(pValue).hash_code());
    return(ZS_INVTYPE);
}// _getZTypeT for class (class & struct)


/** @endcond */ // Development


/**
 * @brief zgetZType top level generic template function that calls underneeth detailed function per storage type. @see _getZTypeT
 *
 *  In this function, the real input is the template parameter equal to the type of data to define.
 *
 *  The objective is to define the data ZType_type to be used within the dictionary within ZIndexControlBlock.
 *  Doing that its controls whether data type will be suitable to be embeded into a ZIndexKey.
 *
 *  Pointer data type (including CString) are NOT allowed to be part of a key definition.
 *  However, pointers are allowed to be used for creating an addhoc search key. @see ZKey
 *
 *  This is the reason why you should mention the option pFromRecord.
 *  When this option is set (true) then data type is analyzed to be part of a ZIndexKey dictionary and will be used for extracting ZMasterFile record key values.
 *  In this case, pointers are NOT allowed.
 *  In this option is not set (false), then zgetZType is considered to be used for analyzing an adhoc key field, and pointers are allowed.
 *  By default, pFromRecord option is set to true.
 *
 * in case of pointer analysis :
 *  - pArraySize is supposed to contain the number of atomic elements the pointer points to as input
 *  - by default, or if this value is omitted (less than 1), pointer is considered to point to one unique data
 *
 * @param[out] pType        ZType_type mask as a result of the analysis
 * @param[out] pNaturalSize size of the data as to be processed by computer ( sizeof() )
 * @param[out] pUniversalSize     size of the data when it is stored as a Unversal data format
 * @param[in-out] pArrayCount
 *              - Input
 *                  + If type is pointer other than char* then it MUST contain in INPUT the number of atomic data pointer points to.
 *                  Otherwise (< 1) it is considered to be a pointer to a single data type value, and it is set to 1.
 *              - Output
 *                  + Number of occurrences in an array or 1 if the data type is not an array : this is an output data in this case.
 *
 * @param[in] pFromRecord   boolean mentionning if this data type should be validated for being a field to be used for record extraction or feeding
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
template <class _Tp>
ZStatus
zgetZType_T(ZTypeBase &pType,
            size_t &pNaturalSize,
            size_t &pUniversalSize,
            URF_Array_Count_type &pArrayCount,
            bool pRecordUse=true)
{
ZStatus wSt;
//    const size_t wTypeHashCode = typeid(_Tp).hash_code();
    _Tp wValue;
    if (pArrayCount<1)
                pArrayCount=1;
    pType = ZType_Nothing ; // set pType (ZType_type mask) to zero (ZType_Nothing)


    //-----First data structure analysis-----------
    //

    wSt=  _getZType_T<_Tp>(wValue,pType,pNaturalSize,pUniversalSize,pArrayCount);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    if (pRecordUse)
            {
            if (pType&ZType_Pointer)
                {
                 utfdescString wOutName;
                 typeDemangler(typeid(_Tp).name(),(char*)wOutName.content,wOutName.getByteSize());
                 ZException.setMessage (  _GET_FUNCTION_NAME_,
                                            ZS_INVTYPE,
                                            Severity_Fatal,
                                            " Invalid field data type  <%s>(before demangling <%s>).A pointer is not usable as a valid key field while extracting from a record (Key definition)",
                                            wOutName.toCString_Strait(),
                                            typeid(_Tp).name() );
                 return ZS_INVTYPE;
                }
            }

    return wSt;

}// zgetZType
/*
ZStatus
zgetZType(auto &pValue,
          ZTypeBase& pType,
          size_t &pNaturalSize,
          size_t &pUniversalSize,
          URF_Array_Count_type &pArrayCount,
          bool pRecordUse=true)
{
    return zgetZType_T<typeof(pValue)>(pValue,pType,pNaturalSize,pUniversalSize,pArrayCount);
}
*/
//-----------------------End Get Type of data --------------------------------------------------


//-------- Unitary (Atomic) Key fields definition-------------------------

template <class _Tp>
ZStatus
_getAtomicZType_T(const size_t pTypeHashCode,ZTypeBase& pType,size_t& pNaturalSize,size_t& pUniversalSize);

/** to be changed to templates */

//ZStatus _getAtomicUfN_A(auto pValue,ZDataBuffer& pUniversal,ZType_type& pType);
//ZStatus _getArrayUfN_A(auto pValue,ZDataBuffer& pUniversal,ZType_type& pType,long &pArrayCount);

//ZStatus _getAtomicNfU_A(auto &pValue,ZDataBuffer& pUniversal,ZType_type& pType);
//ZStatus _getArrayNfU_A(auto &pValue,ZDataBuffer& pUniversal,ZType_type& pType,long &pArrayCount);

/** End to be changed to templates */

//-------------Functions----------------------------------
//
bool is_little_endian(void);


/**
 * @addtogroup ZDATAFORMAT
 * @{ */
//---------------Adhoc key creation : Build Key from Value routines----------------------------------------------------
/**
@weakgroup ZKeyCorrespondence Using ZKey : Correspondence between data types

Setting a ZKey object with values implies a control of data types : data type of value to set vs data type of key field.

@par Corresponding sizes of Natural vs Universal size
Additionnally there is a control of size of value vs natural size of key field in case of array or pointer or class.

- Size principles :
    Value size must be less than or equal to key field size. If size of value is less than size of key field then key field is padded to binary zero.
    If size is greater (for instance sizeof (class)), then a warning message is emitted to stderr
    and size is adjusted to key field size without being rejected with an error status.

- special case : Strings.
    Strings are processed apart from other data field.
   A string MUST have mandatorily a fixed array of char as Key data type.
   But this key field can be set with many data types (see further).

@par  Correspondence Key field data type  vs input value data type

- array of char : valid input value types are
    + array of char : size of array is checked and value must be less or equal than key field
    + pointer to char (C string) : size is given by strlen(). if value size exceeds key field array size then value size is truncated without error status.
    + std::string : key field is loaded with std::string content.If value size exceeds key field array size then value size is truncated without error status.
    + pointer to std::string : key field is loaded with std::string content.If value size exceeds key field array size then value size is truncated without error status.

- Atomic data :  valid input value types are
    + same atomic data type (there could be a data conversion)
    + pointer to the same atomic data type (there could be a data conversion)

- Arrays : valid input value types are
    + arrays of same atomic data type (there could be a data conversion).
        size must less or equal to key field array size. if size of value is less, key field size is padded with binary zeroes
        @bug arrays as input value generates compilation error due to a mismatch of types during compilation.
    + pointer to same atomic data type (there could be a data conversion).
You must mention pArraySize parameter defining how much atomic values pointer points to.

@par Atomic input data type conversion

There should be a correspondance between atomic data type for the key field and for the value to set the field with.
If atomic data types are not the same (value to set and key field type) then a conversion (cast) is done to the destination keyfield type using _convertAtomicKeyFieldValue() routine.
- for atomic data : type conversion is done (reverse byte order if endian, sign byte)
- for arrays : any single atomic data value of the array is converted
- for pointers : any single atomic data value the pointer points to is converted
- for classes : there is no conversion at all, and raw data is moved "as is" to index key.

  */



/** @cond Development */


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


/*
ZStatus
_castAtomicValue_A (auto pValue,
                  const ZType_type pTargetType,
                  ZDataBuffer &pOutValue)
{
    return _castAtomicValue_T<typeof(pValue)>(pValue,pTargetType,pOutValue);
}
*/
#ifdef __COMMENT__
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

// for atomic
//ZStatus zgetUfN_A(auto &pNatural,ZDataBuffer &pUniversal,ZType_type& pType,long &pArrayCount);

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
/** @} */ // end ZKeyGroup

//------------Functions-----------------------------------------

//ZStatus
//getAtomicZType(auto pValue, ZTypeBase& pType, uint64_t &pNaturalSize, uint64_t &pUniversalSize);

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







//===================== Natural to Universal format data conversion ============================

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
 * @brief _getAtomicUfN_T  Template: get Atomic Universal From Natural value according its data type (ZType_type) given by pZType
 *
Converts a Atomic Natural field to an Universal format value
reverse byte order if necessary (Endian)
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
template <class _Tp>
static inline
_Tp _getAtomicUfN_T(unsigned char* pInData, ZDataBuffer &pUniversalData,const ZTypeBase pType )
{
    _Tp wValue , wValue2;

    memmove(&wValue,pInData,sizeof(wValue));

    if (wValue < 0)
                wValue2 = -wValue;
            else
                wValue2 = wValue;

    if ((pType & ZType_Endian)&&is_little_endian())    // only if system is little endian and data type is subject to endian reverse byte conversion
                        wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);

    if (pType & ZType_Signed)
            {

            pUniversalData.allocate(sizeof(_Tp)+1);   // unsigned means size + sign byte
            if (wValue<0)  // if negative value
                    {
                    pUniversalData.Data[0]=0; // sign byte is set to Zero
                    _negate (wValue2); // mandatory otherwise -120 is greater than -110
                    }
                else
                    pUniversalData.Data[0]=1;

            memmove(pUniversalData.Data+1,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
            return wValue;
            }
// up to here : unsigned data type

//    pOutData->setData(&wValue,sizeof(_Tp));// not deduced for sizeof(_Tp) because of Arrays
    pUniversalData.setData(&wValue,getAtomicUniversalSize(pType));// unsigned means same size as input data type
    return wValue;
} // _getAtomicUfN_T


/**
 * @brief _getArrayUfN_T  Template: gets an Array of Universal values from an array of Natural values
 *
 *
 * @param pKeyData      Key buffer to extract field from
 * @param pType         Full ZType_type for the array
 * @return The obtained value with _Tp template parameter type : Field content extracted and formatted up to be used as key field
 */
template <class _Tp>
static
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
        {
        switch (wType)
        {
           case ZType_S8 :
                   {
            _getAtomicUfN_T<int8_t>((unsigned char*) &pValue[wi],wDBV,wType);
            break;
                   }
            case ZType_WChar:  // WChar is treated as an unsigned uint16_t but subject to endian conversion
            case ZType_U16 :
                     {
            _getAtomicUfN_T<uint16_t>((unsigned char*) &pValue[wi],pUniversal,wType);
            break;
                     }
            case ZType_S16 :
                    {
            _getAtomicUfN_T<int16_t>((unsigned char*) &pValue[wi],pUniversal,wType);
            break;
                    }
            case ZType_U32 :
                     {
            _getAtomicUfN_T<uint32_t>((unsigned char*) &pValue[wi],pUniversal,wType);
            break;
                     }
            case ZType_S32 :
                    {
            _getAtomicUfN_T<int32_t>((unsigned char*) &pValue[wi],pUniversal,wType);
            break;
                    }
            case ZType_U64 :
                     {
            _getAtomicUfN_T<uint64_t>((unsigned char*) &pValue[wi],pUniversal,wType);
            break;
                     }
            case ZType_S64 :
                    {
            _getAtomicUfN_T<int64_t>((unsigned char*) &pValue[wi],pUniversal,wType);
            break;
                    }
           case ZType_Float :
                        {
            _getAtomicUfN_T<float>((unsigned char*) &pValue[wi],pUniversal,wType);
            break;
                        }
            case ZType_Double :
                         {
            _getAtomicUfN_T<double>((unsigned char*) &pValue[wi],pUniversal,wType);
            break;
                         }
            case ZType_LDouble :
                         {
            _getAtomicUfN_T<long double>((unsigned char*) &pValue[wi],pUniversal,wType);
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
                            return ZS_INVTYPE;
                        }

           }//switch
        if (wSt!=ZS_SUCCESS)
                    return wSt;
        pUniversal.appendData(wDBV);
        }// for
    return ZS_SUCCESS;
} // _getArrayUfN_T

//========== Conversion from Universal to Natural data format======================================

/**
 * @brief _getAtomicNfU_T  Template: get Atomic Natural value From  Universal format
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
static inline
_Tp _getAtomicNfU_T_Ptr (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
                         unsigned char* pUniversal_Ptr,
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
                    _negate(pValue); // mandatory otherwise -120 is greater than -110
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

/*template <class _Tp>
static inline
_Tp _getAtomicNfU_T (_Tp &pValue,
                     ZDataBuffer &pUniversal,const ZTypeBase pType)
{
    return _getAtomicNfU_T_Ptr<_Tp>(pUniversal.Data,pType);
}*/

/**
 * @brief _getArrayNfU_T  Template: gets an Array of Natural values from an array of Universal values
 *
 *
 * @param pKeyData      Key buffer to extract field from
 * @param pType         Full ZType_type for the array
 * @return The obtained value with _Tp template parameter type : Field content extracted and formatted up to be used as key field
 */
template <class _Tp>
static inline
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
            case ZType_WChar:  // WChar is treated as an unsigned uint16_t but subject to endian conversion
            case ZType_WUChar:  // WUChar is treated as an unsigned uint16_t but subject to endian conversion
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

#ifdef __COMMENT__

template <class _Tp>
/**
 * @brief _getClassNfU_T Only those class types are allowed :
 *      templateString and derived,
 *      wtemplateString and derived (same as templateString but with wchar_t)
 *      std::string
 *      ZDataBuffer
 *
 * @param pValue
 * @param pUniversal
 * @param pType
 * @return
 */
static inline
_Tp* _getClassNfU_T (typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
                     ZDataBuffer &pUniversal,
                     const ZType_type pType)
{
size_t wAtomicOffset =0 ;
int32_t wType = pType & ZType_AtomicMask;  // get the atomic data type


    if (pType==ZType_Blob)
        {
        ZDataBuffer* wValue=static_cast<ZDataBuffer*>(&pValue);
        wValue->setData(pUniversal);
        return wValue;
        }
    if (pType==ZType_FixedCString)
        {
        templatePString* wValue=static_cast<templatePString*>(&pValue);
        size_t wCapacity=wValue->_capacity;
        }
    if (pType==ZType_FixedWString)
        {
        templatePWString* wValue=static_cast<templatePWString*>(&pValue);
        size_t wCapacity=wValue->_capacity;
        }

}
#endif // __COMMENT__
template <class _Tp>
/**
 * @brief _castAtomicToUfN_T template routine that delivers a ZDataBuffer with casted data from an input type to another
 *      Types must be compatible
 * @param pValue
 * @param pUniversal
 * @param pDesiredType
 * @return
 */
ZStatus
_castAtomicToUfN_T (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
                    ZDataBuffer &pUniversal,
                    ZTypeBase pDesiredType)
{
size_t wUniversalSize, wNaturalSize;
uint16_t wArrayCount;
ZTypeBase wType2;
_Tp wValue = pValue;
ZTypeBase wType = pDesiredType & ZType_AtomicMask;

    if (sizeof(_Tp) > getAtomicNaturalSize(pDesiredType))
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_FIELDLOOSEPREC,
                                      Severity_Error,
                                      "asked to convert to <%s>. Field may loose precision: Conversion rejected.",
                                      decode_ZType(pDesiredType));
                return ZS_FIELDLOOSEPREC;
                }


    if ((pValue<0)&&!(wType&ZType_Signed))
            {
            _getZType_T<_Tp>(pValue,wType2,wNaturalSize,wUniversalSize,wArrayCount);
             ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_FIELDCAPAOVFLW,
                                   Severity_Error,
                                   " request to cast a negative number of type <%s> to unsigned type <%s>",
                                   decode_ZType(wType2),
                                   decode_ZType(wType));
             return ZS_FIELDCAPAOVFLW;
            }

    switch (wType)
    {
    case ZType_U8 :
             {
              uint8_t wValue= static_cast<uint8_t>(pValue); // no endian no sign
              _getAtomicUfN_T<uint8_t>((unsigned char*)&wValue,pUniversal,wType);
              return ZS_SUCCESS;
             }
    case ZType_S8 : // no endian - sign byte
            {
            int8_t wValue= static_cast<int8_t>(pValue);
             _getAtomicUfN_T<int8_t>((unsigned char*)&wValue,pUniversal,wType);
            return ZS_SUCCESS;
            }

     case ZType_U16 :
              {
                uint16_t wValue= static_cast<uint16_t>(pValue);
                _getAtomicUfN_T<uint16_t>((unsigned char*)&wValue,pUniversal,wType);
                return ZS_SUCCESS;
              }
     case ZType_S16 :
             {
                int16_t wValue= static_cast<int16_t>(pValue);
                _getAtomicUfN_T<int16_t>((unsigned char*)&wValue,pUniversal,wType);
                return ZS_SUCCESS;
             }
     case ZType_U32 :
              {

            uint32_t wValue= static_cast<uint32_t>(pValue);
            _getAtomicUfN_T<uint32_t>((unsigned char*)&wValue,pUniversal,wType);
               return ZS_SUCCESS;
              }
     case ZType_S32 :
             {
            int32_t wValue= static_cast<int32_t>(pValue);
            _getAtomicUfN_T<int32_t>((unsigned char*)&wValue,pUniversal,wType);
             return ZS_SUCCESS;
             }
     case ZType_U64 :
              {
            uint64_t wValue= static_cast<uint64_t>(pValue);
            _getAtomicUfN_T<uint64_t>((unsigned char*)&wValue,pUniversal,wType);
              return ZS_SUCCESS;
              }
     case ZType_S64 :
             {
            int64_t wValue= static_cast<int64_t>(pValue);
            _getAtomicUfN_T<int64_t>((unsigned char*)&wValue,pUniversal,wType);
             return ZS_SUCCESS;
             }
    case ZType_Float :
                 {
                float wValue= static_cast<float>(pValue);
                _getAtomicUfN_T<float>((unsigned char*)&wValue,pUniversal,wType);
                 return ZS_SUCCESS;
                 }
     case ZType_Double :
                  {
                double wValue= static_cast<double>(pValue);
                _getAtomicUfN_T<double>((unsigned char*)&wValue,pUniversal,wType);
                  return ZS_SUCCESS;
                  }
     case ZType_LDouble :
                  {
                long double wValue= static_cast<long double>(pValue);
                _getAtomicUfN_T<long double>((unsigned char*)&wValue,pUniversal,wType);;
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
#include <cstdint>
template<class _TpCheck>
ZStatus checkMaxAtomicValue_T(ZTypeBase pAtomicType,_TpCheck pValueCheck)
{
size_t wUniversalSize, wNaturalSize;
uint16_t wArrayCount;
ZTypeBase wCheckType;

    switch (pAtomicType)
    {
    case ZType_U8:
        {
        if (pValueCheck>UINT8_MAX)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Severe,
                                  " Field capacity overflow trying for unsigned type <%s>",
                                  decode_ZType(pAtomicType));
            return ZS_FIELDCAPAOVFLW;
            }
        if (pValueCheck<0)
            {
            _getZType_T<_TpCheck>(pValueCheck,wCheckType,wNaturalSize,wUniversalSize,wArrayCount);
             ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_INVVALUE,
                                   Severity_Severe,
                                   " request to cast a negative number of type <%s> to unsigned type <%s>",
                                   decode_ZType(wCheckType),
                                   decode_ZType(pAtomicType));
             return ZS_INVVALUE;
            }
        return ZS_SUCCESS;
        }
    case ZType_S8 :
        {
        if (pValueCheck>INT8_MAX)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Severe,
                                  " Field capacity overflow trying for unsigned type <%s>",
                                  decode_ZType(pAtomicType));
            return ZS_FIELDCAPAOVFLW;
            }
        return ZS_SUCCESS;
        }

     case ZType_U16 :
        {
        if (pValueCheck>UINT16_MAX)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Severe,
                                  " Field capacity overflow trying for unsigned type <%s>",
                                  decode_ZType(pAtomicType));
            return ZS_FIELDCAPAOVFLW;
            }
        if (pValueCheck<0)
            {
            _getZType_T<_TpCheck>(pValueCheck,wCheckType,wNaturalSize,wUniversalSize,wArrayCount);
             ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_INVVALUE,
                                   Severity_Severe,
                                   " request to cast a negative number of type <%s> to unsigned type <%s>",
                                   decode_ZType(wCheckType),
                                   decode_ZType(pAtomicType));
             return ZS_INVVALUE;
            }
        return ZS_SUCCESS;
        }
     case ZType_S16 :
        {
        if (pValueCheck>INT16_MAX)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Severe,
                                  " Field capacity overflow trying for unsigned type <%s>",
                                  decode_ZType(pAtomicType));
            return ZS_FIELDCAPAOVFLW;
            }
        return ZS_SUCCESS;
        }
     case ZType_U32 :
        {
        if (pValueCheck>UINT32_MAX)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Severe,
                                  " Field capacity overflow trying for unsigned type <%s>",
                                  decode_ZType(pAtomicType));
            return ZS_FIELDCAPAOVFLW;
            }
        if (pValueCheck<0)
            {
            _getZType_T<_TpCheck>(pValueCheck,wCheckType,wNaturalSize,wUniversalSize,wArrayCount);
             ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_INVVALUE,
                                   Severity_Severe,
                                   " request to cast a negative number of type <%s> to unsigned type <%s>",
                                   decode_ZType(wCheckType),
                                   decode_ZType(pAtomicType));
             return ZS_INVVALUE;
            }
        return ZS_SUCCESS;
        }
     case ZType_S32 :
        {
        if (pValueCheck>INT32_MAX)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Severe,
                                  " Field capacity overflow trying for unsigned type <%s>",
                                  decode_ZType(pAtomicType));
            return ZS_FIELDCAPAOVFLW;
            }
        return ZS_SUCCESS;
        }
     case ZType_U64 :
        {
        if (pValueCheck>UINT64_MAX)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Severe,
                                  " Field capacity overflow trying for unsigned type <%s>",
                                  decode_ZType(pAtomicType));
            return ZS_FIELDCAPAOVFLW;
            }
        if (pValueCheck<0)
            {
            _getZType_T<_TpCheck>(pValueCheck,wCheckType,wNaturalSize,wUniversalSize,wArrayCount);
             ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_INVVALUE,
                                   Severity_Severe,
                                   " request to cast a negative number of type <%s> to unsigned type <%s>",
                                   decode_ZType(wCheckType),
                                   decode_ZType(pAtomicType));
             return ZS_INVVALUE;
            }
        return ZS_SUCCESS;
        }
     case ZType_S64 :
        {
        if (pValueCheck>INT64_MAX)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Severe,
                                  " Field capacity overflow trying for unsigned type <%s>",
                                  decode_ZType(pAtomicType));
            return ZS_FIELDCAPAOVFLW;
            }
        return ZS_SUCCESS;
        }

        default:
        {
         return ZS_SUCCESS;
        }
    }// switch*/
} // checkMaxAtomicValue_T
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
getAtomicZType(_Tp pValue, ZTypeBase& pType, uint64_t& pNaturalSize, uint64_t& pUniversalSize)
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
                    pUniversalSize = pNaturalSize;  // array of char is a special case because no sign byte is added
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
    pUniversalSize = -1;

    return (ZS_INVTYPE);

    }// while
    pUniversalSize = pNaturalSize;
    if (pType & ZType_Signed)
                pUniversalSize = pUniversalSize+ 1; //! take care of sign byte

    return(ZS_SUCCESS);
}// getAtomicZType generic - not template

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
                    unsigned char* pUniversal, // Source data
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
         _getZType_T<_TpTarget>(pTargetValue,wTargetType,wNaturalSize,wUniversalSize,wArrayCount);
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
#endif // ZDATATYPE_H
