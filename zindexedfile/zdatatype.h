#ifndef ZDATATYPE_H
#define ZDATATYPE_H

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

// see <zindexedfile/znaturalfromurf.cpp>




ZStatus getAtomicZType_Sizes(ZTypeBase pType, uint64_t& pNaturalSize, uint64_t& pUniversalSize);


//ZStatus _castAtomicValue_A (auto pValue,const ZType_type pTargetType,ZDataBuffer &pOutValue);

template <class _Tp>
ZTypeBase
_getAtomicZTypeFull(const size_t pTypeHashCode, uint64_t& pNaturalSize, uint64_t& pUniversalSize);

template <class _Tp>
ZTypeBase
_getClassZTypeFull( _Tp*  pValue,               /* value is required to get much info on it when it is class */
                    const size_t wTypeHashCode,
                    size_t &pNaturalSize,
                    size_t &pUniversalSize,
                    URF_Array_Count_type &pUnitCount);



/** @} */

using namespace zbs ;

typedef utf8String ZFieldName_type ;



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

//-------- Data type constant values-----------------------------

static const size_t CharType = typeid(char).hash_code();
static const size_t UCharType = typeid(unsigned char).hash_code();
/* not to be used anymore
static const size_t WCharType = typeid(wchar_t).hash_code();
static const size_t WUCharType = typeid(unsigned wchar_t).hash_code();
*/
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
static const size_t md5Type= typeid(md5).hash_code();
static const size_t bitsetType= typeid(ZBitset).hash_code();

static const size_t zdatabufferType= typeid(ZDataBuffer).hash_code();

static const size_t stdStringType= typeid(std::string).hash_code();
static const size_t stdWStringType= typeid(std::wstring).hash_code();

static const size_t zdateType= typeid(ZDate).hash_code();
static const size_t zdateFullType= typeid(ZDateFull).hash_code();

//---------- Data type acquisition ----------------------------
//

template <class _Tp>
ZTypeBase _getAtomicZType(const size_t pTypeHashCode);
template <class _Tp>
ZTypeBase _getClassZType(size_t pTypeHashCode);


ZStatus _getFixedStringType(void*pValue, ZTypeBase &pType,size_t &pNaturalSize ,size_t &pUniversalSize ,URF_Array_Count_type &pArrayCount);
//ZStatus _getFixedWStringType(void*pValue, ZTypeBase &pType, size_t &pNaturalSize , size_t &pUniversalSize , URF_Array_Count_type &pArrayCount);

ZStatus _getBlobType(void*pValue,ZTypeBase &pType,size_t &pNaturalSize , size_t &pUniversalSize ,URF_Array_Count_type &pArrayCount);

ZStatus _getBitsetTypeFull(void*pValue, ZTypeBase &pType, uint64_t &pNaturalSize , uint64_t&pUniversalSize , URF_Array_Count_type &pArrayCount);


template <class _Tp> // IMPORTANT: must be template because called by conditional templates
/**
 * @brief _getAtomicZTypeFull template to analyze a type hashcode and deduce ZType_Type and natural and internal sizes for an atomic data
 * This function must be a template because called by template routines
 *
 * @param[in] pTypeHashCode  type hashcode obtained using typeid(VAR).hash_code(). VAR must be atomic.
 * @param[out] pType         deduced ZType_type
 * @param[out] pNaturalSize  Resulting natural size of the variable
 * @param[out] pKeyFieldSize Resulting internal size of the variable
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZTypeBase
_getAtomicZTypeFull(const size_t pTypeHashCode,  size_t& pNaturalSize, size_t& pUniversalSize)
{
  ZTypeBase wType=0;
    while (true)
    {
    if (pTypeHashCode==UCharType)
                    {
                    wType |= ZType_UChar ;
                    pNaturalSize=sizeof(unsigned char);
                    break;
                    }
    if (pTypeHashCode==CharType)
                    {
                    wType |= ZType_Char ;
                    pNaturalSize=sizeof(char);
                    pUniversalSize = pNaturalSize;  // array of char is a special case because no sign byte is added
                    break;
                    }
/* not to be used anymore
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
*/
    if (pTypeHashCode==U8Type)
                    {
                    wType |= ZType_U8 ;
                    pNaturalSize=sizeof(uint8_t);
                    break;
                    }
    if (pTypeHashCode==S8Type)
                    {
                    wType |= ZType_S8 ;
                    pNaturalSize=sizeof(int8_t);
                    break;
                    }
    if (pTypeHashCode==S16Type)
                    {
                    wType |= ZType_S16 ;
                    pNaturalSize=sizeof(int16_t);
                    break;
                    }
    if (pTypeHashCode==U16Type)
                    {
                    wType |= ZType_U16 ;
                    pNaturalSize=sizeof(uint16_t);
                    break;
                    }
    if (pTypeHashCode==S32Type)
                    {
                    wType |= ZType_S32 ;
                    pNaturalSize=sizeof(int32_t);
                    break;
                    }
    if (pTypeHashCode==U32Type)
                    {
                    wType |= ZType_U32 ;
                    pNaturalSize=sizeof(uint32_t);
                    break;
                    }
    if (pTypeHashCode==S64Type)
                    {
                    wType |= ZType_S64 ;
                    pNaturalSize=sizeof(int64_t);
                    break;
                    }
    if (pTypeHashCode==U64Type)
                    {
                    wType |= ZType_U64 ;
                    pNaturalSize=sizeof(uint64_t);
                    break;
                    }

//-------Floating point----------------------------
//
    if (pTypeHashCode==FloatType)
                    {
                    wType |= ZType_Float ;
                    pNaturalSize=sizeof(float);
                    break;
                    }
    if (pTypeHashCode==DoubleType)
                    {
                    wType |= ZType_Double ;
                    pNaturalSize=sizeof(double);
                    break;
                    }
    if (pTypeHashCode==LDoubleType)
                    {
                    wType |= ZType_LDouble ;
                    pNaturalSize=sizeof(long double);
                    break;
                    }


    wType = ZType_Unknown;
    pNaturalSize = -1;
    pUniversalSize = -1;
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_INVTYPE,
                          Severity_Severe,
                          "Atomic type of data hashcode <%lX> cannot not be recognized",pTypeHashCode);
    return (wType);

    }// while
    pUniversalSize = pNaturalSize;
    if (wType & ZType_Signed)
                pUniversalSize = pUniversalSize+ 1; // take care of sign byte

    return(wType);
}// _getAtomicZTypeFull_T generic - template

template <class _Tp>
ZTypeBase
_getAtomicZType(const size_t pTypeHashCode)
{

    if (pTypeHashCode==UCharType)
    {
      return ZType_UChar ;
    }
    if (pTypeHashCode==CharType)
    {
      return ZType_Char ;
    }
    /* not to be used anymore
    if (pTypeHashCode==WCharType)
                    {
                    return ZType_WChar ;
                    }
    if (pTypeHashCode==WUCharType)
                    {
                    return ZType_WUChar ;
                    }
*/
    if (pTypeHashCode==U8Type)
    {
      return ZType_U8 ;
    }
    if (pTypeHashCode==S8Type)
    {
      return ZType_S8 ;
    }
    if (pTypeHashCode==S16Type)
    {
      return ZType_S16 ;
    }
    if (pTypeHashCode==U16Type)
    {
      return ZType_U16 ;
    }
    if (pTypeHashCode==S32Type)
    {
      return ZType_S32 ;
    }
    if (pTypeHashCode==U32Type)
      return ZType_U32 ;

    if (pTypeHashCode==S64Type)
      return ZType_S64 ;

    if (pTypeHashCode==U64Type)
      return ZType_U64 ;


    //-------Floating point----------------------------
    //
    if (pTypeHashCode==FloatType)
      return ZType_Float ;

    if (pTypeHashCode==DoubleType)
      return ZType_Double ;

    if (pTypeHashCode==LDoubleType)
      return ZType_LDouble ;


    return (ZType_Unknown);
}// _getAtomicZType_T generic - template


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

/**================================_getZType templates===========================================================
 *    Warning : Enum is not allowed
 */
template <typename _Tp>
/**
 * @brief _getZTypeFull_T template function : from a given value (pValue) with template type _Tp analyzes the type of data and :
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
 * @param[out] pType
 * @param[out] pNaturalSize resulting natural total size of the array
 * @param[out] pUniversalSize  resulting (Key) internal total size of the data field
 * @param[out] pCapacity   Number of occurrences withing the array if data type is an array or 1 if atomic data or Class
 * @return  a ZTypeBase : resulting ZType_type data type mask. If not recognized, set to ZType_Unknown and ZException is set appropriately.
 */
ZTypeBase
_getZTypeFull_T(typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
           size_t &pNaturalSize,
           size_t &pUniversalSize,
           URF_Array_Count_type &pCapacity)
{
    ZTypeBase wType  ; // set pType (ZType_type mask) to zero (ZType_Nothing)
    pCapacity = 1;
    const size_t wTypeHashCode = typeid(pValue).hash_code();
    if (wTypeHashCode==typeid(void).hash_code())
                {
                ZException.setMessage  (_GET_FUNCTION_NAME_,
                                          ZS_INVTYPE,
                                          Severity_Severe,
                                          "Invalid data type requested <void> ");

                return ZS_INVTYPE ;
                }

    wType = _getAtomicZTypeFull<_Tp>(wTypeHashCode,pNaturalSize,pUniversalSize);
    if (wType!=ZType_Unknown )
      return wType | ZType_Atomic;

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
    return(wType);

}// _getZType_T integral + floating point (fundamental excepted std::nullptr_t and void data types)

/** @brief _getZTypeFull_T get ZType template for Class type ( class or struct ).
 */
template <typename _Tp>
ZTypeBase
_getZTypeFull_T(typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
    size_t &pNaturalSize,
    size_t &pUniversalSize,
    URF_Array_Count_type &pUnitCount)
{
  ZTypeBase pType ;
  const size_t wTypeHashCode=typeid(pValue).hash_code();
  pUnitCount = 1;

  pType = _getClassZTypeFull<_Tp>(&pValue,wTypeHashCode,pNaturalSize,pUniversalSize,pUnitCount);

  if (pType != ZType_Unknown)
    return ZS_SUCCESS;

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
}// _getZType_T for class (class & struct)

template <typename _Tp>
ZTypeBase
_getZTypeFull_T(typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
           size_t &pNaturalSize,
           size_t &pUniversalSize,
           URF_Array_Count_type &pCapacity)
{
ZStatus wSt;
ZTypeBase wType;

    const size_t wTypeHash = typeid(*pValue).hash_code();

    wType=_getAtomicZTypeFull<_Tp>(wTypeHash,pNaturalSize,pUniversalSize); // analyze first array element's Atomic data type
    if (wType!=ZType_Unknown){
      return wType | ZType_Pointer;
    }
    /* not found, try classes */
    wType=_getClassZTypeFull<_Tp>(wTypeHash,pNaturalSize,pUniversalSize,pCapacity); // analyze first array element's Atomic data type
    if (wType!=ZType_Unknown)
      return wType | ZType_Pointer;


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
    fprintf (stderr,ZException.last().formatFullUserMessage().toCChar());
    return wType | ZType_Pointer;

} // _getZTypeFull_T for Pointer



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
ZTypeBase
_getZTypeFull_T(typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue,
                size_t &pNaturalSize,
                size_t &pUniversalSize,
                URF_Array_Count_type &pCapacity)
{
  ZTypeBase wType;
  const size_t wTypeHash = typeid(pValue[0]).hash_code();

  wType=_getAtomicZTypeFull<_Tp>(wTypeHash,pNaturalSize,pUniversalSize); // analyze first array element's Atomic data type
  if (wType!=ZType_Unknown){
    return wType | ZType_Pointer;
  }
  /* not found, try classes */
  wType=_getClassZTypeFull<_Tp>(wTypeHash,pNaturalSize,pUniversalSize,pCapacity); // analyze first array element's Atomic data type
  if (wType!=ZType_Unknown)
    return wType | ZType_Array;


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
  fprintf (stderr,ZException.last().formatFullUserMessage().toCChar());
  return wType | ZType_Array;;

}// _getZType_T for Array

//--------------Enum will not be allowed anymore : deprecated and not useful----------------

//#include <ztoolset/zutfstrings.h>

/*============== simple get data type =================*/


template <typename _Tp>
/**
 * @brief _getZType_T template function : from a given value (pValue) with template type _Tp analyzes the type of data and :
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
 * @return  resulting ZType_type data type mask.
 * If type has not been found, then ZType_Unknown is returned and ZException is set with appropriate message.
 */
ZTypeBase
_getZType_T(typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue)
{

    const size_t wTypeHashCode = typeid(pValue).hash_code();
    if (wTypeHashCode==typeid(void).hash_code()) {
        ZException.setMessage  (_GET_FUNCTION_NAME_,
                                          ZS_INVTYPE,
                                          Severity_Severe,
                                          "Invalid data type requested <void> ");

        fprintf (stderr,ZException.last().formatFullUserMessage().toCChar());
        return ZType_Unknown ;
    }

    ZTypeBase wType = _getAtomicZType<_Tp>(wTypeHashCode);

    if (wType!=ZType_Unknown){
      return wType | ZType_Atomic;
    }
    /* well, this is an unknown type */
    char wOutName[50];
    char*wOutNamePtr= (char*)wOutName;
    memset(wOutName,0,50);
    typeDemangler(typeid(_Tp).name(),wOutNamePtr,50);
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Severe,
        " Unrecognized data type  <%s> (before demangling <%s>) hascode <%lX.",
        wOutName,
        typeid(_Tp).name(),
        typeid(pValue).hash_code());
    fprintf (stderr,ZException.last().formatFullUserMessage().toCChar());
    return wType;
}// _getZType_T integral + floating point (fundamental excepted std::nullptr_t and void data types)


template <typename _Tp>
ZTypeBase
_getZType_T(typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue)
{
  ZTypeBase wType;
  const size_t wTypeHash = typeid(*pValue).hash_code();

  wType=_getAtomicZType<_Tp>(wTypeHash); // analyze first array element's Atomic data type
  if (wType!=ZType_Unknown){
    return wType | ZType_Pointer;
  }
  /* not found, try classes */
  wType=_getClassZType<_Tp>(wTypeHash); // analyze first array element's Atomic data type
  if (wType!=ZType_Unknown){
    return wType | ZType_Pointer;
  }
  /* well, this is an unknown type */
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
  fprintf (stderr,ZException.last().formatFullUserMessage().toCChar());
  return wType;

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
ZTypeBase
_getZType_T(typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue)   //!< Number or array elementsbool pRecordUse)
{
ZStatus wSt;
 //   long wNaturalSize = 0;  //! total field natural size
 //   long wKeySize = 0;      //! unary element size within Key ( may differ according sign byte )
ZTypeBase wType;
const size_t wTypeHash = typeid(pValue[0]).hash_code();

    wType=_getAtomicZType<_Tp>(wTypeHash); // analyze first array element's Atomic data type
    if (wType!=ZType_Unknown){
      return wType | ZType_Array;
    }
    /* not found, try classes */
    wType=_getClassZType<_Tp>(wTypeHash); // analyze first array element's Atomic data type
    if (wType!=ZType_Unknown){
      return wType | ZType_Array;
    }
    /* well, this is an unknown type */
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
    fprintf (stderr,ZException.last().formatFullUserMessage().toCChar());
    return wType;
}// _getZTypeT for Array



//--------------Enum will not be allowed anymore : deprecated and not useful----------------

//#include <ztoolset/zutfstrings.h>
ZStatus _getFixedStringType(void*pValue,
                            ZTypeBase &pType,
                            size_t &pNaturalSize ,
                            size_t &pUniversalSize ,
                            URF_Array_Count_type &pArrayCount);
//ZStatus _getFixedWStringType(void*pValue, ZTypeBase &pType, size_t &pNaturalSize , size_t &pUniversalSize , URF_Array_Count_type &pArrayCount);

ZStatus _getBlobType(void*pValue,ZTypeBase &pType,size_t &pNaturalSize , size_t &pUniversalSize ,URF_Array_Count_type &pArrayCount);

ZStatus _getBitsetTypeFull(void*pValue, ZTypeBase &pType, uint64_t &pNaturalSize , uint64_t&pUniversalSize , URF_Array_Count_type &pArrayCount);


/* single get for ztype (no other values) */


/** @brief _getZType_T get ZType template for Class type ( class or struct ).
 *  It does nothing : it is there to insure std::enable_if <> coherence while compiling
 */
template <typename _Tp>
ZTypeBase
_getZType_T(typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue)
{
    size_t wTypeHashCode=typeid(pValue).hash_code();

    ZTypeBase wType = _getClassZType<_Tp>(wTypeHashCode);

    if (wType!=ZType_Unknown)
      return wType;

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
    fprintf(stderr,ZException.formatFullUserMessage().toCChar());
    return(wType);
}// _getZType_T for class (class & struct)


template <class _Tp>
ZTypeBase
_getClassZType(size_t pTypeHashCode)
{
  if (pTypeHashCode==stdStringType)
    return ZType_StdString;
  if (pTypeHashCode==stdWStringType)
    return ZType_StdWString;

  //===============Fixed utf strings=================================

  if (pTypeHashCode==descStringType)
    return ZType_Utf8FixedString;
  if (pTypeHashCode==codeStringType)
    return ZType_Utf8FixedString;
  if (pTypeHashCode==messageStringType)
    return ZType_Utf8FixedString;
  if (pTypeHashCode==keywordStringType)
    return ZType_Utf8FixedString;
  if (pTypeHashCode==identityStringType)
    return ZType_Utf8FixedString;

  if (pTypeHashCode==uriStringType)
    return ZType_URIString;

  //===============Varying utf strings=================================

  if (pTypeHashCode==utf8varyingStringType)
    return ZType_Utf8VaryingString;
  if (pTypeHashCode==utf16varyingStringType)
    return ZType_Utf16VaryingString;
  if (pTypeHashCode==utf32varyingStringType)
    return ZType_Utf32VaryingString;

  if (pTypeHashCode==checksumType)
    return ZType_CheckSum;
  if (pTypeHashCode==md5Type)
    return ZType_MD5;

  if (pTypeHashCode==bitsetType)
    return ZType_bitset;

  if (pTypeHashCode==zdatabufferType)
    return ZType_Blob;

  if (pTypeHashCode==zdateType)
    return ZType_ZDate;
  if (pTypeHashCode==zdateFullType)
    return ZType_ZDateFull;


  return(ZType_Unknown);
}// _getClassZType for class (class & struct)


template <class _Tp>
ZTypeBase
_getClassZTypeFull( _Tp*  pValue,               /* value is required to get much info on it when it is class */
                    const size_t wTypeHashCode,
                    size_t &pNaturalSize,
                    size_t &pUniversalSize,
                    URF_Array_Count_type &pUnitCount)
{
  ZTypeBase pType = ZType_Class ;
  pUnitCount = 1;

  //===============Varying utf strings=================================

  if (wTypeHashCode==utf8varyingStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
  }
  if (wTypeHashCode==utf16varyingStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
  }
  if (wTypeHashCode==utf32varyingStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
  }

  //=============Crypting classes =======================

  if (wTypeHashCode==checksumType)
  {
    pType=ZType_CheckSum;
    pNaturalSize = pUniversalSize =pUnitCount = cst_checksum;
    return pType;
  }
  if (wTypeHashCode==md5Type)
  {
    pType=ZType_MD5;
    pNaturalSize = pUniversalSize =pUnitCount = cst_md5 ;
    return pType;
  }
  //============= bitset =======================

  if (wTypeHashCode==bitsetType)
  {
    pType=ZType_bitset;
    return _getBitsetTypeFull(pValue,pType,pNaturalSize , pUniversalSize ,pUnitCount);
    return pType;
  }

  //============= blob (ZDataBuffer) =======================

  if (wTypeHashCode==zdatabufferType)
  {
    pType=ZType_Blob;
    _getBlobType(pValue,pType,pNaturalSize , pUniversalSize ,pUnitCount);
    return pType;
  }

  //============= dates =======================

  if (wTypeHashCode==zdateType)
  {
    pType=ZType_ZDate;
    pNaturalSize = pUniversalSize = sizeof(uint32_t);
    pUnitCount =1;
    return pType;
  }
  if (wTypeHashCode==zdateFullType)
  {
    pType=ZType_ZDateFull;
    pNaturalSize = pUniversalSize = sizeof(uint64_t);
    pUnitCount = 1;
    return pType;
  }

  if (wTypeHashCode==stdStringType)
  {
    _getZTypeStdString<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
  }
  if (wTypeHashCode==stdWStringType)
  {
    _getZTypeStdWString<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
  }
  //===============Fixed size strings=================================

  if (wTypeHashCode==descStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
    /*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_desclen+1;
        return ZS_SUCCESS ;*/
  }
  if (wTypeHashCode==codeStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
    /*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_codelen+1;
        return ZS_SUCCESS ;*/
  }
  if (wTypeHashCode==uriStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
  }
  if (wTypeHashCode==messageStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
    /*        pType=ZType_Utf8FixedString;
        pNaturalSize = pUniversalSize =pUnitCount = cst_messagelen+1;
        return ZS_SUCCESS ;*/
  }
  if (wTypeHashCode==keywordStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
  }
  if (wTypeHashCode==identityStringType)
  {
    _getUtfStringData<_Tp>(pValue,pType,pNaturalSize,pUniversalSize,pUnitCount) ;
    return pType;
  }


  pType=ZType_Unknown;
  pNaturalSize = pUniversalSize = pUnitCount=0;


  return pType;
}// _getClassZType for class (class & struct)


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
ZTypeBase
zgetZType_T(
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
    ZTypeBase pType = ZType_Nothing ; // set pType (ZType_type mask) to zero (ZType_Nothing)


    //-----First data structure analysis-----------
    //

    pType=  _getZTypeFull_T<_Tp>(wValue,pNaturalSize,pUniversalSize,pArrayCount);
    if (pType==ZType_Unknown)
                    return pType;
    if (pRecordUse)
            {
            if (pType & ZType_Pointer)
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

    return pType;

}// zgetZType

//-----------------------End Get Type of data --------------------------------------------------


//-------- Unitary (Atomic) Key fields definition-------------------------

template <class _Tp>
ZStatus
_getAtomicZTypeFull_T(const size_t pTypeHashCode,size_t& pNaturalSize,size_t& pUniversalSize);

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
            wCheckType = _getZTypeFull_T<_TpCheck>(pValueCheck,wNaturalSize,wUniversalSize,wArrayCount);
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
            wCheckType=_getZTypeFull_T<_TpCheck>(pValueCheck,wNaturalSize,wUniversalSize,wArrayCount);
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
            wCheckType=_getZTypeFull_T<_TpCheck>(pValueCheck,wNaturalSize,wUniversalSize,wArrayCount);
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
            wCheckType=_getZTypeFull_T<_TpCheck>(pValueCheck,wNaturalSize,wUniversalSize,wArrayCount);
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
getAtomicZTypeFull(_Tp pValue, ZTypeBase& pType, uint64_t& pNaturalSize, uint64_t& pUniversalSize)
{
  const size_t wTypeHashCode = typeid(pValue).hash_code();
  pType=ZType_Nothing;
  while (true)
  {
    if (wTypeHashCode==UCharType)
    {
      pType |= ZType_UChar ;
      pNaturalSize=sizeof(unsigned char);
      break;
    }
    if (wTypeHashCode==CharType)
    {
      pType |= ZType_Char ;
      pNaturalSize=sizeof(char);
      pUniversalSize = pNaturalSize;  // array of char is a special case because no sign byte is added
      break;
    }
    if (wTypeHashCode==U8Type)
    {
      pType |= ZType_U8 ;
      pNaturalSize=sizeof(uint8_t);
      break;
    }
    if (wTypeHashCode==S8Type)
    {
      pType |= ZType_S8 ;
      pNaturalSize=sizeof(int8_t);
      break;
    }
    if (wTypeHashCode==S16Type)
    {
      pType |= ZType_S16 ;
      pNaturalSize=sizeof(int16_t);
      break;
    }
    if (wTypeHashCode==U16Type)
    {
      pType |= ZType_U16 ;
      pNaturalSize=sizeof(uint16_t);
      break;
    }
    if (wTypeHashCode==S32Type)
    {
      pType |= ZType_S32 ;
      pNaturalSize=sizeof(int32_t);
      break;
    }
    if (wTypeHashCode==U32Type)
    {
      pType |= ZType_U32 ;
      pNaturalSize=sizeof(uint32_t);
      break;
    }
    if (wTypeHashCode==S64Type)
    {
      pType |= ZType_S64 ;
      pNaturalSize=sizeof(int64_t);
      break;
    }
    if (wTypeHashCode==U64Type)
    {
      pType |= ZType_U64 ;
      pNaturalSize=sizeof(uint64_t);
      break;
    }

    //-------Floating point----------------------------
    //
    if (wTypeHashCode==FloatType)
    {
      pType |= ZType_Float ;
      pNaturalSize=sizeof(float);
      break;
    }
    if (wTypeHashCode==DoubleType)
    {
      pType |= ZType_Double ;
      pNaturalSize=sizeof(double);
      break;
    }
    if (wTypeHashCode==LDoubleType)
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


#endif // ZDATATYPE_H
