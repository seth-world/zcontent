#ifndef ZINDEXTYPE_H
#define ZINDEXTYPE_H
#include <zconfig.h>
#include <type_traits>
#include <ztoolset/zerror.h>
#include <ztoolset/zbasedatatypes.h>
#include <ztoolset/zexceptionmin.h>
#include <zrandomfile/zrandomfiletypes.h>
#include <cstdint>
#include <ztoolset/zarray.h>

#include <zindexedfile/zmetadic.h>

#define __ZMF_VERSION__ 2000
#define __ZIF_VERSION__ 2000

#include <zindexedfile/zdatatype.h>

/**
    @addtogroup ZIndexGroup
    @{ */

//                   ZType_type see <zindexedfile/zdatatype.h>
#ifdef __COMMENT__
/**
 * @brief The ZType_type enum  ZIndex key data type (component of ZIndexItem_struct )
 *
 * This enum qualifies any data type that will be processed to build key dictionaries.
 *
 *@image latex ztype_type.jpg "ZRandomFile physical layer" width=10cm
 *@image html ztype_type.jpg "ZRandomFile physical layer" width=10cm
 *
 * Hereafter are the possible values of ZType_type enum.
 */
/** @verbatim

                            +--------> type of storage : Atomic (01), Pointer(02), Array (04), enum (08), ByteSeq (10), Class (20), String (40)
                            |
                            | +------> subject to Endian conversion Yes(1) No(0) [if and only if system is Little Endian]
                            | |+-----> signed( 1)/ unsigned (0)
                            | ||+----> Varying length (1) or Fixed length (0)
                            | ||| +--> Elementary value type (what data is composed of, i. e. Unsigned byte for example)
                            | ||| |
                            --|-|___
   ZType_type(Hexadecimal)  XXxXxxxx
 @endverbatim
*/
enum ZType_type : uint32_t
{
    ZType_Nothing        = 0,           //!< NOP

    ZType_Atomic         = 0x01000000,  //!< data type is elementary
    ZType_Endian         = 0x00100000,  //!< data type is subject to little endian conversion  see: @ref ZIndexDataConversion
    ZType_Signed         = 0x00010000,  //!< data type is signed. Sign impacts of data conversion see: @ref ZIndexDataConversion
    ZType_Pointer        = 0x02000000,  //!< data type is a pointer
    ZType_Array          = 0x04000000,  //!< data type is an array (content has to be further described}
//    ZType_Enum           = 0x08000000,  //!< data type is enum
    ZType_ByteSeq        = 0x10000000,  //!< data type is a sequence of bytes regardless any type, only defined by a start address and a length. Warning data type is a pointer

    ZType_AtomicMask     = 0x00000FFF,
    ZType_VaryingMask    = 0x0000F000,
    ZType_SignedMask     = 0x000F0000,
    ZType_EndianMask     = 0x00F00000, //!< used to negate any preceeding data structure indication and keep atomic data type.
    ZType_NegateStructure= 0xFF000000, //!< used to negate any preceeding data structure indication and keep atomic data type.
    ZType_StructureMask  = 0xFF000000,

    ZType_Class          = 0x20000000, //!< Field is a class or a struct (NON POLYMORPHIC class):  NO CONTENT CONVERSION of the underneeth data will be made. Use it carefully.

    ZType_VaryingLength  = 0x00001000,

    ZType_String         = 0x40000000, //!< generic string type

    ZType_StdString      = 0x40001FFF,  //!< Field is a std::string class varying length : not used for storage, only for conversion
    ZType_CString        = 0x40001001,  //!< data type is a C string : varying length of char ended with '\0'
    ZType_FixedString    = 0x40000001,  //!< a fixed length string containing a c string : example descString

    ZType_WCString       = 0x40111008,  //!< WCHAR varying string
    ZType_WCUString      = 0x40101008,  //!< WCUCHAR varying string

    ZType_FWCString      = 0x40110008,  //!< Fixed WCHAR varying string
    ZType_FWCUString     = 0x40100008,  //!< Fixed WCUCHAR varying string

    ZType_Blob           = 0x10001000, //! blob : a varying sized content of bytes : See ByteSeq
//
// elementary data types - is combined (ored) with other attributes to give the full data type
//
    ZType_UChar          = 0x00000002,    //!< unsigned char - mostly used for encoding/decoding
    ZType_Char           = 0x00000001,    //!< char - a single ascii character - NOT SUBJECT to leading byte despite its signed status- use int8_t if a one byte numeric value is required

    ZType_U8             = 0x00000004,    //!< arithmetic byte
    ZType_S8             = 0x00010004,    //!< arithmetic signed byte
    ZType_U16            = 0x00100008,    //!< arithmetic 2 bytes value unsigned (no leading sign byte)
    ZType_S16            = 0x00110008,    //!< arithmetic 2 bytes value signed (with leading sign byte)
    ZType_U32            = 0x00100010,    //!< arithmetic 4 bytes value unsigned (no leading sign byte)
    ZType_S32            = 0x00110010,    //!< arithmetic 4 bytes value signed (with leading sign byte)
    ZType_U64            = 0x00100020,    //!< arithmetic 8 bytes value unsigned (no leading sign byte)
    ZType_S64            = 0x00110020,    //!< arithmetic 8 bytes value signed (with leading sign byte)

    ZType_Float          = 0x00110040,    //!< floating point (therefore signed)
    ZType_Double         = 0x00110080,    //!< floating point double (therefore signed)
    ZType_LDouble        = 0x00110100,    //!< floating point long double (therefore signed)
//
// Fully qualified data types : for usefulness purpose
//
    ZType_AtomicUChar    = 0x01000002,    //!< unsigned char - mostly used for encoding/decoding
    ZType_AtomicChar     = 0x01000001,    //!< char - a single ascii character - NOT SUBJECT to leading byte despite its signed status- use int8_t if a one byte numeric value is required

    ZType_AtomicWUChar   = 0x01100002,    //!< unsigned char - mostly used for encoding/decoding
    ZType_AtomicWChar    = 0x01100001,    //!< char - a single ascii character - NOT SUBJECT to leading byte despite its signed status- use int8_t if a one byte numeric value is required


    ZType_AtomicU8       = 0x01000004,    //!< arithmetic byte      no Endian
    ZType_AtomicS8       = 0x01010004,    //!< arithmetic signed byte no Endian
    ZType_AtomicU16      = 0x01100008,    //!< arithmetic 2 bytes value unsigned (no leading sign byte)
    ZType_AtomicS16      = 0x01110008,    //!< arithmetic 2 bytes value signed (with leading sign byte)
    ZType_AtomicU32      = 0x01100010,    //!< arithmetic 4 bytes value unsigned (no leading sign byte)
    ZType_AtomicS32      = 0x01110010,    //!< arithmetic 4 bytes value signed (with leading sign byte)

    ZType_AtomicU64      = 0x01100020,    //!< arithmetic 8 bytes value unsigned (no leading sign byte)
    ZType_AtomicS64      = 0x01110020,    //!< arithmetic 8 bytes value signed (with leading sign byte)

    ZType_AtomicFloat    = 0x01110040,    //!< floating point (therefore signed)
    ZType_AtomicDouble   = 0x01110080,    //!< floating point double (therefore signed)
    ZType_AtomicLDouble  = 0x01110100,    //!< floating point long double (therefore signed)

    ZType_PointerChar    = 0x02000001,    //!< this is the C string
    ZType_PointerUChar   = 0x02010002,    //!< special string specifically used by encoding

    ZType_ArrayChar      = 0x04000001,    //!< this is a C string stored as a key value (CString must be fixed length, with variable length content)
    ZType_ArrayUChar     = 0x04010002,    //!< and also could be used by encoding (same as above, but may be used for encoding/decoding)

    ZType_EnumByte       = 0x08110004,    //!< This is enum uses a long
    ZType_EnumWord       = 0x08110008,    //!< This is enum uses a long long
    ZType_EnumLong       = 0x08110010,    //!< This is enum uses a long
    ZType_EnumLongLong   = 0x08110020,    //!< This is enum uses a long long

    ZType_Unknown        = 0xF00FFFFF,    //! Unmanaged data type : generally an error
} ;

const char *decode_ZType (long pType) ;
struct ZType_struct{
    ZType_type Type;
};

ZStatus getAtomicZType_Sizes(int32_t pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize);

class CType : public ZType_struct
{
public:
    typedef ZType_struct _Base;
    CType() {}
    CType(ZType_type pType) {_Base::Type=pType;}

    bool isAtomic() {return (Type&ZType_Atomic);}
    bool isString() {return (Type&ZType_String);}
    bool isVarying() {return (Type&ZType_VaryingLength);}
    bool isEndian() {return (Type&ZType_Endian);}


    int32_t getStructure() {return Type&ZType_StructureMask; }
    int32_t getAtomicType() {return Type&ZType_AtomicMask;}

    size_t getAtomicUniversalSize()
    {
        ssize_t wNaturalSize,wUniversalSize;

        if (getAtomicZType_Sizes(getAtomicType(),wNaturalSize,wUniversalSize)!=ZS_SUCCESS)
                                                                ZException.exit_abort();
        return wUniversalSize;
    }
    size_t getAtomicNaturalSize()
    {
        ssize_t wNaturalSize,wUniversalSize;

        if (getAtomicZType_Sizes(getAtomicType(),wNaturalSize,wUniversalSize)!=ZS_SUCCESS)
                                                                ZException.exit_abort();
        return wNaturalSize;
    }

    CType* extract (ZDataBuffer_long* pZDB,int64_t Offset)
    {
        memmove (&Type,pZDB->Data+Offset,sizeof(Type));
        return this;
    }
};

ZStatus
getZTypeFromString (long &pZType , ZDataBuffer &pString);

#endif // __COMMENT__


using namespace zbs ;

typedef utfdescString ZFieldName_type ;
class ZIndexField;
#pragma pack(push)
#pragma pack(0)
/**
 * @brief The fieldDesc_strOut struct  this struct is a flat class requested to export a ZIndexField
 */
class Indexfield_Export
{
    Indexfield_Export &_copyFrom(Indexfield_Export &pIn);
public:
    Indexfield_Export() = default;
    Indexfield_Export(Indexfield_Export& pIn) { _copyFrom(pIn); }
    Indexfield_Export(Indexfield_Export&& pIn) { _copyFrom(pIn); }

    Indexfield_Export& operator = (Indexfield_Export& pIn) {return _copyFrom(pIn); }
    Indexfield_Export& operator = (Indexfield_Export&& pIn) {return _copyFrom(pIn); }

    ZTypeBase    ZType;          //!< Type mask of the Field @see ZType_type
    uint64_t     NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
    uint64_t     UniversalSize;   //!< length of the field when stored into Key (Field dictionary internal format size)
    URF_Capacity_type     Capacity ;      //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArrayCount or KeySize / ArrayCount ). For other storage type, this field is set to 1.
    uint64_t     RecordOffset;   //!< Offset of the Field from the beginning of record
    char        Name[cst_fieldnamelen+1+sizeof(uint16_t)];;      //!< short string (max 15 char) containing the name of the field (for readability purpose, not necessary for key processing)

};//indexfield_strOut
#pragma pack(pop)


/**

 * @brief ZIndexField_struct Dictionary single field definition  : one key may be composed of many single fields not necessarily contiguous

    ZType : ZType is a mask of ZType_type describing the data field type (Atomic data type, signed or not, storage type (Atomic, Array, Class, Pointer), etc.)

    @note  Field size requires two different values :
    - NaturalSize represents data type size as it is stored originally by computer data type (natural format). It is also called External format.
    - KeySize represents data type size as it is stored within ZIndexFile key (key format). It is also called Internal format.

    1. Atomic, Array and Class : Field size fields represent the overall size of the data field : for an array, for instance this is the sum of the array element sizes.
    2. Pointers : ZType mask is set to the atomic data Pointer points to.
        - for pointers to char and uchar : NaturalSize and KeySize are set to the string size obtained with strlen.
          This implies that any pointer to char or uchar MUST BE zero terminated.
        - for pointers to other types : it is considered as pointer to Atomic data.
          So that, Natural size and KeySize are set accordingly to the values corresponding to ZType_type of Atomic data pointer points to.

 */
class ZIndexField
{
public:
    uint64_t
        NaturalSize; //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
    uint64_t
        UniversalSize; //!< length of the field when stored into Key (Field dictionary internal format size)
    URF_Capacity_type
        Capacity; //!< replaces ArrayCount : if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if an atomic
//    uint32_t
//        ArrayCount; //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArrayCount or KeySize / ArrayCount ). For other storage type, this field is set to 1.
    ZTypeBase ZType; //!< Type mask of the Field @see ZType_type
    uint64_t RecordOffset; //!< Offset of the Field from the beginning of record
    ZFieldName_type
        Name; //!< short string (max 15 char) containing the name of the field (for readability purpose, not necessary for key processing)

    ZIndexField() = default;
    ZIndexField(ZIndexField &pIn) { _copyFrom(pIn); }
    ZIndexField(ZIndexField &&pIn) { _copyFrom(pIn); }

    ZIndexField &_copyFrom(ZIndexField &pIn);

    ZIndexField &operator=(ZIndexField &pIn) { return _copyFrom(pIn); }
    ZIndexField &operator=(ZIndexField &&pIn) { return _copyFrom(pIn); }

    bool isAtomic(void) {return ZType & ZType_Atomic ;}
    bool isSigned(void) {return ZType & ZType_Signed ;}
    bool isEndian(void) {return ZType & ZType_Endian ;}
    bool isUnknown(void) { return (ZType == ZType_Unknown); }

    /**
     * @brief _exportConvert static routine used by ZArray::export facilities and corresponding to ZAexportCurrent requirements
     * @param pIn
     * @param wOut
     * @return
     */
    static Indexfield_Export _exportConvert(ZIndexField &pIn, Indexfield_Export* wOut);
    static ZIndexField _importConvert(ZIndexField& pOut,Indexfield_Export *pIn);

    void clear()
    {
        Name.clear();
        NaturalSize = 0;
        UniversalSize = 0;
        Capacity = 0;
        ZType = 0;
        RecordOffset = 0;
    }



};

#ifndef __KEYDICTYPE__
#define __KEYDICTYPE__
enum ZKeyDic_type {
    ZKD_Nothing         = 0,
    ZKD_NaturalSize     = 1,
    ZKD_UniversalSize    = 2,
    ZKD_KeyOffset       = 4,
    ZKD_RecordOffset    = 8,
    ZKD_ArrayCount       = 0x10,
    ZKD_ZType           = 0x20,
    ZKD_Name            = 0x40
};
#endif //__KEYDICTYPE__
/**
 * @brief The ZKeyDictionary class store ZIndex index key fields definition for one key (and therefore one ZIndexFile)
 */

class ZKeyDictionary : public zbs::ZArray<ZIndexField>
{
public:
    ssize_t NaturalSize ;   //!< overall size of the key (sum of keyfields NATURAL (total) length).
    ssize_t UniversalSize ;  //!< total of key size with internal format
/* not exported */
    checkSum *CheckSum=nullptr;
    zbs::ZMetaDic *MetaDic=nullptr;


    ZKeyDictionary(zbs::ZMetaDic * pMetaDic) { clear(pMetaDic); }
    ~ZKeyDictionary() {if (CheckSum!=nullptr)delete CheckSum;}



    ssize_t fieldKeyOffset (const long pRank);
    ssize_t fieldRecordOffset (const long pRank);

    ZStatus addFieldToZDic (const utf8_t *pFieldName);
    ZStatus addFieldToZDic (const long pMDicRank);

    void print (FILE* pOutput=stdout);

    void _reComputeSize (void) ;
    template <class _Tp>
    ZStatus zaddField (const char *pFieldName,const size_t pZMFOffset);
    template <class _Tp>
    ZStatus zaddField (utfdescString &pFieldName,const size_t pZMFOffset);
    ZStatus zremoveField (const char *pFieldName);
    ZStatus zremoveField (const long pFieldRank);
    template <class _Tp>
    ZStatus zsetField (const long pFieldRank,ZKeyDic_type pZKD,_Tp pValue);

    long zsearchFieldByName(const char* pFieldName) ;
    long zsearchFieldByName(utfdescString &pFieldName) ;

    void clear(zbs::ZMetaDic *pMetaDic)
    {
        MetaDic = pMetaDic;
        NaturalSize = 0;
        UniversalSize = 0;
        ZArray<ZIndexField>::clear();
        return;
    }

    ZDataBuffer& _export(ZDataBuffer& pZDBExport) ;
    size_t _import (unsigned char* pZDBImport_Ptr);



} ;


/** @cond Development */

template <class _Tp>
ZStatus
_addFieldToZDic (const char *pFieldName,const size_t pOffset, ZKeyDictionary &pZKDic);

template <class _Tp>
ZStatus ZKeyDictionary::zaddField (const char *pFieldName,size_t pZMFOffset)
{
    return(_addFieldToZDic<_Tp>(pFieldName,pZMFOffset,*this));
}

//---------- for data conversion to and from universal format see <zindexfile/datatype.h>---------

#ifdef __COMMENT__
//---------------Index Key Data Fields Format and conversions-----------------
//

//------------Demangling (according platform)-------------------------
#ifdef __GNUG__
#include <memory>
#include <cxxabi.h>

#endif// __GNUG__
#ifdef __USE_WINDOWS__

#include <Windows.h>
#include <Dbghelp.h>

#endif // __USE_WINDOWS__
ZStatus
typeDemangler(const char*pName,descString &pOutName);

//-------- Data type constant values-----------------------------

static const size_t CharType = typeid(char).hash_code();
static const size_t UCharType = typeid(unsigned char).hash_code();

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
_getAtomicZTypeT(const size_t pTypeHashCode,ZTypeBase& pType,ssize_t& pNaturalSize,ssize_t& pUniversalSize)
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
}// _getAtomicZTypeT generic - template


//-----------------Key Dictionary definition--------------------------------
// --------------- Key field type ------------------------------------------

// following is no more used see conditional template _getZTypeT function
//static inline
//ZStatus _getZType(auto pValue, ZType_type &pType, ssize_t &pNaturalSize, ssize_t pKeySize,bool pFromRecord=true);

auto            _negate(auto &pValue);
unsigned char*  _negate(unsigned char* pValue, ssize_t pSize);

template <typename _Tp>
ZStatus
_getZTypeStdString (const void *pValue,ZTypeBase &pType,ssize_t &pNaturalSize,ssize_t &pKeySize,long &pArrayCount)
{
const std::string* wString = static_cast<const std::string*>(pValue);
  pType = ZType_StdString ;

  pArrayCount = wString->size();
  pNaturalSize = pArrayCount;
  pKeySize= pArrayCount;
  return ZS_SUCCESS;
}
template <typename _Tp>
ZStatus
_getZTypeCString (const void *pValue,ZTypeBase &pType,ssize_t &pNaturalSize,ssize_t &pKeySize,long &pArrayCount)
{
const char* wString = static_cast<const char*>(pValue);
  pType = ZType_PointerChar ;

  pArrayCount = strlen(wString);
  pNaturalSize = pArrayCount;
  pKeySize= pArrayCount;
  return ZS_SUCCESS;
}

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
 * @param[out] pArrayCount   Number of occurrences withing the array if data type is an array or 1 if atomic data or Class
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
_getZTypeT(typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
           ZTypeBase &pType,
           ssize_t &pNaturalSize,
           ssize_t &pKeyFieldSize,
           long &pArrayCount)
{
    pType = ZType_Nothing ; // set pType (ZType_type mask) to zero (ZType_Nothing)
    pArrayCount = 1;
    const size_t wTypeHashCode = typeid(pValue).hash_code();
    if (wTypeHashCode==typeid(void).hash_code())
                {
                ZException.setMessage  (_GET_FUNCTION_NAME_,
                                          ZS_INVTYPE,
                                          Severity_Severe,
                                          "Invalid data type requested <void> ");

                return ZS_INVTYPE ;
                }
    if (std::is_pointer<_Tp>::value&&(pValue==0))  //! this is std::nullptr_t  (allowed as fundamental but has to be rejected)
                {
                ZException.setMessage  (_GET_FUNCTION_NAME_,
                                          ZS_INVTYPE,
                                          Severity_Severe,
                                          "Invalid data type requested <std::nullptr_t> ");

                return ZS_INVTYPE ;
                }

    return _getZTypeT<_Tp>(pValue,pType,pNaturalSize,pKeyFieldSize,pArrayCount);

}// _getZTypeT integral + floating point (fundamental excepted std::nullptr_t and void data types)

template <typename _Tp>
ZStatus
_getZTypeT(typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
           ZTypeBase &pType,
           ssize_t &pNaturalSize,
           ssize_t &pKeyFieldSize,
           long &pArrayCount)
{
ZStatus wSt;

    pType = ZType_Nothing ; // set pType (ZType_type mask) to zero (ZType_Nothing)

    pType |= ZType_Pointer;
    if (typeid(std::string).hash_code()==typeid(*pValue).hash_code())  // address pointer to std::string
    {
    return _getZTypeStdString<_Tp>(pValue,pType,pNaturalSize,pKeyFieldSize,pArrayCount) ;
    }
    ssize_t wAtomicNaturalSize;
    ssize_t wAtomicKeyFieldSize;
    wSt=_getAtomicZTypeT<_Tp>(typeid(*pValue).hash_code(),pType,wAtomicNaturalSize,wAtomicKeyFieldSize); //! analyze first array element's Atomic data type
    if (wSt!=ZS_SUCCESS)
                return wSt;


    if (pType & ZType_Char)
                {
                return _getZTypeCString<_Tp>(pValue,pType,pNaturalSize,pKeyFieldSize,pArrayCount);
//                return ZS_SUCCESS;
                }
    pNaturalSize = wAtomicNaturalSize;
//    pArrayCount = 1 ;
    pKeyFieldSize =  wAtomicKeyFieldSize ;

    return ZS_SUCCESS;


} // _getZTypeT for Pointer



template <typename _Tp>
/**
 * @brief _getZTypeT get ZType template for Array. That far we are processing an Array.
 *    So we will get of what is made this array.
 *    Length of the field will be length of array * unitary size*
 * @param[in]  pValue the value of the data field to be analyzed
 * @param[out] pType resulting ZType_type data type mask
 * @param[out] pNaturalSize resulting natural total size of the array
 * @param[out] pKeyFieldSize  resulting (Key) internal total size of the array
 * @param[out] pArrayCount   Number of occurrences withing the array
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
_getZTypeT(typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue,
           ZTypeBase &pType,             //!< Field type mask using ZType_type
           ssize_t &pNaturalSize,   //!< total field natural size (External size)
           ssize_t &pKeyFieldSize,  //!< total field internal format size ( may differ from natural size according sign byte )
           long &pArrayCount)        //!< Number or array elements
{
ZStatus wSt;
 //   long wNaturalSize = 0;  //! total field natural size
 //   long wKeySize = 0;      //! unary element size within Key ( may differ according sign byte )
    pType = ZType_Nothing ; // set pType (ZType_type mask) to zero (ZType_Nothing)
    ssize_t wAtomicNaturalSize;
    ssize_t wAtomicKeyFieldSize;
    wSt=_getAtomicZTypeT<_Tp>(typeid(pValue[0]).hash_code(),pType,wAtomicNaturalSize,wAtomicKeyFieldSize); // analyze first array element's Atomic data type
    if (wSt!=ZS_SUCCESS)
                return wSt;
    pNaturalSize = sizeof (_Tp);
    pArrayCount = (ssize_t ) ((float)pNaturalSize / (float)wAtomicNaturalSize );
    pKeyFieldSize = pArrayCount * wAtomicKeyFieldSize ;
    pType |= ZType_Array ;
    return(ZS_SUCCESS);

}// _getZTypeT for Array
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
 * @param[out] pArrayCount   Number of occurrences withing the array
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
_getZTypeT(typename std::enable_if_t<std::is_enum<_Tp>::value,_Tp> &pValue,
           ZTypeBase &pType,             //!< Field type mask using ZType_type
           ssize_t &pNaturalSize,   //!< total field natural size (External size)
           ssize_t &pKeyFieldSize,  //!< total field internal format size ( may differ from natural size according sign byte )
           long &pArrayCount)        //!< Number or array elements
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

    pArrayCount=1;
    return(ZS_SUCCESS);

/*
    long wValue=0;    // this is a long

    wSt=_getAtomicZTypeT<_Tp>(typeid(wValue).hash_code(),pType,pNaturalSize,pKeyFieldSize); // analyze first array element's Atomic data type
    if (wSt!=ZS_SUCCESS)
                return wSt;

*/
}// _getZTypeT for Enum

#endif // __COMMENT__

/** @brief _getZTypeT get ZType template for Class type ( class or struct ).
 *  It does nothing : it is there to insure std::enable_if <> coherence while compiling
 */
template <typename _Tp>
ZStatus
_getZTypeT(typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
           ZTypeBase &pType,
           ssize_t &pNaturalSize,
           ssize_t &pKeySize,
           long &pArrayCount)
{
    if (typeid(std::string).hash_code()==typeid(pValue).hash_code())
        {
        return _getZTypeStdString<_Tp>(&pValue,pType,pNaturalSize,pKeySize,pArrayCount) ;
        }
    pType |= ZType_Class ;
    pNaturalSize = sizeof(_Tp);
    pKeySize = sizeof(_Tp);
    pArrayCount = 1;
    return(ZS_SUCCESS);
}// _getZTypeT for class (class & struct)

/** @endcond */ // Development

template <class _Tp>
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
 *  - pArrayCount is supposed to contain the number of atomic elements the pointer points to as input
 *  - by default, or if this value is omitted (less than 1), pointer is considered to point to one unique data
 *
 * @param[out] pType        ZType_type mask as a result of the analysis
 * @param[out] pNaturalSize size of the data as to be processed by computer ( sizeof() )
 * @param[out] pKeySize     size of the data when it is stored as a key field
 * @param[in-out] pArrayCount
 *              - Input
 *                  + If type is pointer other than char* then it MUST contain in INPUT the number of atomic data pointer points to.
 *                  Otherwise (< 1) it is considered to be a pointer to a single data type value, and it is set to 1.
 *              - Output
 *                  + Number of occurrences in an array or 1 if the data type is not an array : this is an output data in this case.
 *
 * @param[in] pFromRecord   boolean mentionning if this data type should be validated for being a field from record extraction
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
zgetZType( ZTypeBase &pType, ssize_t &pNaturalSize,ssize_t &pUniversalSize, ssize_t &pArrayCount=0,bool pFromRecord=true)
{
ZStatus wSt;
//    const size_t wTypeHashCode = typeid(_Tp).hash_code();

    if (pArrayCount<1)
                pArrayCount=1;
    pType = ZType_Nothing ; // set pType (ZType_type mask) to zero (ZType_Nothing)

    _Tp wValue;
    //-----First data structure analysis-----------
    //

    wSt=  _getZTypeT<_Tp>(wValue,pType,pNaturalSize,pUniversalSize,pArrayCount);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    if (pFromRecord)
            {
            if (pType&ZType_Pointer)
                {
                 descString wOutName;
                 typeDemangler(typeid(_Tp).name(),wOutName);
                 ZException.setMessage (  _GET_FUNCTION_NAME_,
                                            ZS_INVTYPE,
                                            Severity_Fatal,
                                            " Invalid field data type  <%s>(before demangling <%s>).A pointer is not usable as a valid key field while extracting from a record (Key definition)",
                                            wOutName.toString(),
                                            typeid(_Tp).name() );
                 return ZS_INVTYPE;
                }
            }

    return wSt;

}// zgetZType
#endif // __COMMENT__

//----------------------ZDictionary routines---------------------------------------------------

#ifdef __COMMENT__
/** @brief _addFieldToZDic : Adds a Field definition to a Key dictionary (fields description list).
 *
 * - Analyzes the type of data using zgetZType() function.
 * - Deduces the data type (ZType_type) and true natural data length as well as internal data length.
 *
 * @note template is required here because 'auto' type hides arrays (char [n] is converted to char*).
 *
 * @param[in] pFieldName user name of the field. Only for readability.
 * @param[in] pOffset    offset of the field IN THE USER RECORD (beginning = 0)
 * @param[in-out] pZKDic a ZDictionay : ZArray of ZIndexField_struct to which field definition will be added
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 *
 */

template <class _Tp> // template needs to be expanded here
static inline
ZStatus
_addFieldToZDic (const char *pFieldName,const size_t pOffset, ZKeyDictionary &pZKDic)
{
ZStatus wSt;

    ZIndexField_struct wField;
    if (pFieldName==nullptr)
            wField.Name.content[0]='\0';
        else
            wField.Name = pFieldName;

    wOffset = pOffset;

    wSt=zgetZType_T<_Tp>(wField.ZType,wField.NaturalSize,wField.UniversalSize,wField.ArrayCount,true);
    if (wSt!=ZS_SUCCESS)
            return wSt;

    pZKDic.push(wField);
    pZKDic._reComputeSize();
    return ZS_SUCCESS;
}//_addFieldToZDic
#endif // __COMMENT__

//-----------------------Get Type of data --------------------------------------------------


//-------- Unitary (Atomic) Key fields definition-------------------------

template <class _Tp>
ZStatus
_getAtomicZTypeT(const size_t pTypeHashCode,ZTypeBase& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize);

//------------Gets the effective fields values from a ZMF user record---------------------------------


ZStatus _getAtomicFromRecord(ZDataBuffer &pInData, ZDataBuffer &pOutData, ZIndexField & pField);
ZStatus _getArrayFromRecord(ZDataBuffer &pInData, ZDataBuffer &pOutData, ZIndexField & pField);



template <class _Tp>
ZStatus _getByteSequenceFromRecord (_Tp pInData,const ssize_t pSize,ZDataBuffer &pOutData, ZIndexField& pField);


//----------Gets a natural value from a key content----------------------------------

ZStatus _getFieldValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZKeyDictionary & pFieldList);
inline
ZStatus _getAtomicValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZKeyDictionary & pFieldList);
inline
ZStatus _getArrayValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZKeyDictionary & pFieldList);
inline
ZStatus _getClassValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZKeyDictionary & pFieldList);


//-------------Functions----------------------------------
//!
bool is_little_endian(void);

ZDataBuffer& _printAtomicValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &pOutValue,const long pRank, ZKeyDictionary & pFieldList);
ZDataBuffer& _printArrayValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &pOutValue,const long pRank, ZKeyDictionary & pFieldList);

class ZKey;

template <class _Tp>
ZStatus
zsetKeyFieldValue (_Tp pValue, ZKey & pZKey,const long pFieldRank,ssize_t pArrayCount);

template <class _Tp>
static inline
ZStatus
_setKeyFieldValue ( ZKey &pZKey,const long pFieldRank,_Tp pValue,const long pArrayNumber);


#ifndef __ZINDEXCONTROLBLOCK_DEFINED__
#define __ZINDEXCONTROLBLOCK_DEFINED__
/**
 * @brief The ZIndexControlBlock class  Defines the operational data that describes one index.
 *
 * This infraData is stored in the ZIndexFile Header AND in the ZMasterFile index collection within its reserved Header block (ZMasterControlBlock)
 */

struct ZICBOwnData{
    int32_t                 StartSign ;         //!< ZICB block start marker
    ZBlockID                BlockID;            //!< must be ZBID_ICB
    long                    ZMFVersion;         //!< Self explainatory
    ssize_t                 ICBSize;            //!< ICB size when written in file header (ZReserved header field)
    utfdescString           Name;               //!< Name for the index
    //ZIFKeyType_type         KeyType;            //!< RFFU
    bool                    AutoRebuild;        //!< RFFU
    ZSort_Type              Duplicates;         //!< Index key is allowing duplicates (ZST_DUPLICATES) or is not allowing duplicates (ZST_NODUPLICATES)
};
class ZMasterFile;
class ZIndexControlBlock : public ZICBOwnData
{
public:
    ZIndexControlBlock() ;
    ~ZIndexControlBlock(void);

    ZKeyDictionary         *ZKDic = nullptr ;  //!< Fields dictionary for the key :
                                                //!for memory management reasons it is a pointer that is instantiated by new and destroyed when ZICB rank is removed
    void clear(ZMetaDic* pMetaDic,ZMasterFile* pZMFFather);

    ZDataBuffer&_exportICB (ZDataBuffer &pICBContent) ;
    ZStatus     _importICB (ZDataBuffer &pRawICB, ssize_t &pImportedSize, size_t pOffset=0) ;
    ZStatus     _importICB (unsigned char* pBuffer, ssize_t &pImportedSize);

    ZStatus     zkeyValueExtraction (ZDataBuffer &pRecord, ZDataBuffer& pKey);

    template <class _Tp>

    ZStatus     zaddFieldToZKDic (const char *pFieldName,const size_t pZMFOffset)
    {
        ZStatus wSt;
        wSt=ZKDic->zaddField<_Tp>(pFieldName,pZMFOffset);
        if (wSt!=ZS_SUCCESS)
                return wSt;
        return wSt;
    }

   ssize_t IndexRecordSize (void)   //!< computes and return the effective size of a ZIndex key record
            {   ZKDic->_reComputeSize();
                return (ZKDic->UniversalSize + sizeof(zaddress_type)); }

private:
    ZDataBuffer     ICBExchange;
    ZMetaDic*       MetaDic=nullptr;
    ZMasterFile *ZMFFather = nullptr;
};

#endif // __ZINDEXCONTROLBLOCK_DEFINED__
/** @}*/ //ZIndexTypeGroup

/**
 * @addtogroup ZKeyGroup
 * @{ */
//---------------Adhoc key creation : Build Key from Value routines----------------------------------------------------
/**
@weakgroup ZKeyCorrespondence Using ZKey : Correspondence between data types

Setting a ZKey object with values implies a control of data types : data type of value to set vs data type of key field.

@par Corresponding sizes of input value vs key field size
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
You must mention pArrayCount parameter defining how much atomic values pointer points to.

@par Atomic input data type conversion

There should be a correspondance between atomic data type for the key field and for the value to set the field with.
If atomic data types are not the same (value to set and key field type) then a conversion (cast) is done to the destination keyfield type using _convertAtomicKeyFieldValue() routine.
- for atomic data : type conversion is done
- for arrays : any single atomic data value of the array is converted
- for pointers : any single atomic data value the pointer points to is converted
- for classes : there is no conversion at all, and raw data is moved "as is" to index key.

  */

// ZKey class definition must stay here in this file because of the use of template routines in ZIndextype

/**
 * @brief The ZKey class  this object manages one key (given by KeyNumber) for a ZMasterFile using its ZIndexControlBlock (ZICB)
 *
 *  ZKey class is used to format a key content for being used with ZMasterFile in queries.
 *
 *  The objective is to offer the whole set of functionalities to create a key,
 *  either a search key or even a key content from a ZMF record.
 *
 *  ZKey imbeds all appropriate data type analyse and conversion routines.
 *
 *  A ZKey object is obtained "ready to use" from a ZMasterFile using
 * - ZMasterFile::createZKey() : create a ZKey corresponding to given ZMF index's rank
 * - ZMasterFile::createZKeyByName() : create a ZKey corresponding to given ZMF index's name
 *
 * @note As these routines deliver instantiated object with 'new' clause,
 *  this is of the user's responsibility to delete the object once it is no more in use.
 *
 *  Both routines give back a pointer to a ZKey object loaded with the appropriate key dictionary.
 *  Once it has been created, this object may be loaded with value using :
 * - setFieldValue() when loading key with litterals or data for search
 * - keyValueExtraction() for extracting a key content from a ZMasterFile user record.
 *
 *
 *  Key value may be searched with
 * - EXACT value correspondance : this means that both content and size must strictly correspond between search key content and index kex content
 * - PARTIAL value correspondance : Key content represents partial content of index key content to search for (starting from beginning of field).
 *
 *  Partial key option is set with ZKey::setKeyPartial() routine.
 *
 */

class
        ZKey : public ZDataBuffer
{
    friend class ZMasterFile;
    friend class ZIndexFile;

    typedef ZDataBuffer _Base;
public:

    ZKey(ZIndexControlBlock *pZICB,const long pKeyRank) ;
    ZIndexControlBlock *ZICB;     //!<  pointer to ZMasterFile ZICB element (also stored within ZIndexFile )
    long                IndexNumber;  //!<  ZMasterFile Index number (rank)


   ZArray<bool> FieldPresence;

   bool FPartialKey=false;  //!< Flag partial key : if partial key then
   ssize_t PartialLength=-1;

   template <class _Tp>
   friend
   ZStatus
   zsetKeyFieldValue (_Tp pValue, ZKey & pZKey,const long pFieldRank,ssize_t pArrayNumber);

   long getSize(void)  {return(long) Size;}

//   using ZDataBuffer::allocate;
   using ZDataBuffer::clearData;
   using ZDataBuffer::Dump;

   ZStatus keyValueExtraction( ZDataBuffer &pRecord) ;
   template <class _Tp>
   ZStatus setFieldValue (const long pFieldRank,_Tp pValue, const long pArrayCount=-1);
   template <class _Tp>
   ZStatus setFieldValue (const char* pFieldName, _Tp pValue, const long pArrayCount=-1);

   ZStatus setKeyPartial (const ssize_t pFieldRank ,const ssize_t pLength);
   ZStatus setKeyPartial (const char* pFieldName,ssize_t pLength);

   template <class _Tp>
   ZStatus setFieldValueT (const long pFieldRank,_Tp pValue, const long pArrayCount=-1);

   template <class _Tp>
   ZStatus setFieldValueT (const char* pFieldName,_Tp pValue, const long pArrayCount=-1);

    ZStatus setString (const long pFieldRank,std::string &pString );
    ZStatus setString (const long pFieldRank, const char* pString);

    void zprintKeyFieldsValues (bool pHeader=true,bool pKeyDump=false,FILE*pOutput=stdout);

   void clearFieldSpace (const long pFieldRank);

   ZKey& setFieldRawValue (const void* pValue,const ssize_t pSize,const long pFieldRank);
   ZKey& setFieldRawValue (ZDataBuffer& pValue,const long pFieldRank);


   void clearData(void)
    {
       _Base::clearData();      // set ZDataBuffer to zero
       FieldPresence.bzero();
       FPartialKey = false;
       PartialLength = -1;
       return;
   }

};


/** @cond Development */

template <class _Tp>
ZStatus
//_setKeyFieldValueStdString (std::enable_if_t<std::is_class<_Tp>::value, _Tp> pString,ZKey & pZKey, const long pFieldRank)
_setKeyFieldValueStdString (const void* pString,ZKey & pZKey, const long pFieldRank)

{
const std::string*  wString= static_cast<const std::string*>(pString);
ssize_t wSize = wString->size() ;
//ssize_t wOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);

    if (pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType!=ZType_ArrayChar)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Error,
                                " While trying to set up a Key field value : destination field->expecting array of char - having %s ",
                                decode_ZType(pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType));
        return ZS_INVTYPE;
        }

    if (wSize> pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize)
                                wSize = pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize;

/*    memset (pZKey.Data+wOffset,0,pZKey.ZICB->ZKDic->Tab[pFieldRank].KeyFieldSize);

    memset (pZKey.Data+wOffset,0,pZKey.ZICB->ZKDic->Tab[pFieldRank].NaturalSize);
    strncpy(pZKey.DataChar+wOffset,wString.c_str(),wSize);
    */
    pZKey.setFieldRawValue((void*)wString->c_str(),wSize,pFieldRank);
    return ZS_SUCCESS;
}// _setKeyFieldValueStdString

template <class _Tp>
ZStatus
_setKeyFieldValueCString (const void* pString,ZKey & pZKey, const long pFieldRank)

{
const char*  wString= static_cast<const char*>(pString);
ssize_t wSize = strlen(wString) ;
//ssize_t wOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);

    if (pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType!=ZType_ArrayChar)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Error,
                                " While trying to set up a Key field value : destination field->expecting array of char - having %s ",
                                decode_ZType(pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType));
        return ZS_INVTYPE;
        }

    if (wSize> pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize)
                                wSize = pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize;

/*    memset (pZKey.Data+wOffset,0,pZKey.ZICB->ZKDic->Tab[pFieldRank].KeyFieldSize);

    memset (pZKey.Data+wOffset,0,pZKey.ZICB->ZKDic->Tab[pFieldRank].NaturalSize);
    strncpy(pZKey.DataChar+wOffset,wString.c_str(),wSize);
    */
    pZKey.setFieldRawValue((void*)wString,wSize,pFieldRank);
    return ZS_SUCCESS;
}


ZStatus
testKeyTypes (const long pTypeIn,const long pKeyType);




template <class _Tp>
/**
 * @brief _castAtomicKeyFieldValue template function that converts any atomic value type to key field atomic value given by pFieldRank within Key dictionary of pKey
 * @param[in] pValue    Data to convert
 * @param[in] pZDic     Key Dictionary
 * @param[in] pZType    ZType_Type of Data to convert
 * @param[in] pFieldRank    Dictionary key field rank to convert the value to
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
//_castAtomicKeyFieldValue (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> pValue,
_castAtomicKeyFieldValue (_Tp pValue,
                             ZKeyDictionary & pZDic,
                             const ZTypeBase pZType,
                             const long pFieldRank,
                             ZDataBuffer &pOutValue)
{


long wType = pZDic[pFieldRank].ZType & ~(ZType_StructureMask);


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
                                             pZDic[pFieldRank].ZType,
                                             decode_ZType(pZDic[pFieldRank].ZType));
                     return ZS_INVTYPE;
                 }
    }// switch
}// _convertAtomicKeyFieldValue



// for atomic

template <class _Tp>
ZStatus
_setKeyFieldValue (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
                   ZKey &pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long pArrayCount) // pArrayNumber is given for pointers others than char pointers : how much atomic data it points to
{
ZStatus wSt;
//ssize_t wOffset = pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);
ssize_t wOffset = 0;

    ZDataBuffer wNaturalValue;
    ZDataBuffer wKeyValue;

    if (pZType != pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType )
            {
            wSt=_castAtomicKeyFieldValue<_Tp>(pValue,pZKey.ZICB->ZKDic,pZType,pFieldRank,wNaturalValue);
            }
        else
            wNaturalValue.setData(&pValue,sizeof(pValue));

    ZIndexField wField;
    wField = pZKey.ZICB->ZKDic->Tab[pFieldRank] ;
    wOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0

    wSt= _getAtomicFromRecord(wNaturalValue,wKeyValue,wField);  // Extract & pack unary Atomic Field ready for Key usage
    if (wSt!=ZS_SUCCESS)
            return wSt;
    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value
    return wSt;
} // _setKeyFieldValue for atomic data type


template <class _Tp>
ZStatus
_getValueFromPointer (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
                   ZKey &pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long pArrayCount)
{
ZStatus wSt;
ssize_t wOffset = pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);
ssize_t wArrayCount = pArrayCount;
    ZDataBuffer wNaturalValue;
    ZDataBuffer wANaturalValue;
    ZDataBuffer wKeyValue;

    if (pArrayCount<1)
            {
            wArrayCount= sizeof(pValue)/sizeof(pValue[0]);
            }
    if (wArrayCount>pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity)  // if array has more elements than key field : truncate to key field
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVSIZE,
                                    Severity_Warning,
                                    " given array size for pointer <%ld>  exceeds size of key field array size<%ld> (field rank <%ld>). Array size truncated to key field array size",
                                    wArrayCount,
                                    pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity,
                                    pFieldRank);
            ZException.printUserMessage(stderr);
            wArrayCount = pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity;
        }
    wNaturalValue.clear();

    if (pZType != pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType )
            {
            for (long wi=0;wi<wArrayCount;wi++)
            {
            wSt=_castAtomicKeyFieldValue<decltype(*pValue)>(pValue[wi],*pZKey.ZICB->ZKDic,pZType,pFieldRank,wANaturalValue);
            wNaturalValue.appendData(wANaturalValue);
             }
            }
        else
            wNaturalValue.appendData(&pValue[0],sizeof(pValue));

    ZIndexField wField;
    wField = pZKey.ZICB->ZKDic->Tab[pFieldRank] ;
    wOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0
    wField.Capacity = wArrayCount;      // cheating also for number of array occurence
    _getArrayFromRecord(wNaturalValue,wKeyValue,wField);  // Extract & pack Array of Atomic Fields ready for Key usage
    if (wSt!=ZS_SUCCESS)
            return wSt;

    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value

    return wSt;
} // _setKeyFieldValue for array data type
template <class _Tp>
ZStatus
_setKeyFieldValue (typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue,
                   ZKey &pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long pArrayCount)
{
ZStatus wSt;
ssize_t wOffset = pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);
ssize_t wArrayCount ;
    ZDataBuffer wNaturalValue;
    ZDataBuffer wANaturalValue;
    ZDataBuffer wKeyValue;

    if (pArrayCount<1)
            {
            wArrayCount= sizeof(pValue)/sizeof(pValue[0]);
            }
    if (wArrayCount>pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity)  // if array has more elements than key field : truncate to key field
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVSIZE,
                                    Severity_Warning,
                                    " given array size <%ld>  exceeds size of key field array size<%ld> (field rank <%ld>). Array size truncated to key field array size",
                                    wArrayCount,
                                    pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity,
                                    pFieldRank);
            ZException.printUserMessage(stderr);
            wArrayCount = pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity;
        }
    wNaturalValue.clear();

    if (pZType != pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType )
            {
            for (long wi=0;wi<wArrayCount;wi++)
            {
            _castAtomicKeyFieldValue<_Tp>(pValue[wi],pZKey.ZICB->ZKDic,pZType,pFieldRank,wANaturalValue);
            wNaturalValue.appendData(wANaturalValue);
             }
            }
        else
            wNaturalValue.appendData(&pValue[0],sizeof(pValue));

    ZIndexField wField;
    wField = pZKey.ZICB->ZKDic->Tab[pFieldRank] ;
    wOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0
    wField.Capacity = wArrayCount;      // cheating also for number of array occurence
    wSt=_getArrayFromRecord(wNaturalValue,wKeyValue,wField);  // Extract & pack Array of Atomic Fields ready for Key usage
    if (wSt!=ZS_SUCCESS)
            return wSt;

    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value

    return wSt;
} // _setKeyFieldValue for array data type

template <class _Tp>
ZStatus
_setKeyFieldValue (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
                   ZKey & pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long pArrayCount) // array size represents the number of value occurrences the pointer points to
{
ZStatus wSt;
long wType=pZType;
//long wOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);
long wOffset=0;
ssize_t wArrayCount=pArrayCount<1?1:pArrayCount;

ZDataBuffer wKeyValue;


    if (wArrayCount>pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVSIZE,
                                Severity_Warning,
                                " given array size <%ld> for pointer exceeds size of key field array size<%ld> (field rank <%ld>). Array size truncated to key field array size",
                                wArrayCount,
                                pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity,
                                pFieldRank);
        ZException.printUserMessage(stderr);
        wArrayCount = pZKey.ZICB->ZKDic->Tab[pFieldRank].Capacity;
    }

    if (wType == ZType_ByteSeq)
        {
        pZKey.setFieldRawValue(pValue,wArrayCount,pFieldRank); // set the key value using pArrayCount as Length
        return ZS_SUCCESS;
        }

    if (wType == ZType_CString)
                        {
                        return _setKeyFieldValueCString<_Tp>(pValue,pZKey,pFieldRank);
                        }

    wType = wType & ~(ZType_Pointer);



    if ((pZType&ZType_StdString)==ZType_StdString) // pointer to std::string : use appropriate routine
            {
            return _setKeyFieldValueStdString<_Tp>(pValue,pZKey,pFieldRank);
            }


//    wNaturalValue.setData(pValue,sizeof(pArrayCount));  // pValue is a pointer and pArrayCount the number of elements

    ZIndexField wField;
    wField = pZKey.ZICB->ZKDic->Tab[pFieldRank] ;
    wOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0

    wSt=_getValueFromPointer<_Tp>(pValue,pZKey,pZType,pFieldRank,wArrayCount);  // Extract & pack values pointed by pValue to get Fields ready for Key usage

//    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value
    return wSt;
} // _setKeyFieldValue for pointers


template <class _Tp>
ZStatus
_setKeyFieldValue (typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
                   ZKey & pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long  pArrayNumber)
{
ssize_t wSize = sizeof(pValue);
// now extracting without conversion
    if ((pZType&ZType_StdString)==ZType_StdString)
            {
            return _setKeyFieldValueStdString<_Tp>(&pValue,pZKey,pFieldRank);
            }

    if (sizeof(pValue)>pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVSIZE,
                                Severity_Warning,
                                " given class size <%ld> exceeds size of key field size<%ld> (field rank <%ld>). Array size truncated to key field array size",
                                wSize,
                                pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize,
                                pFieldRank);
        ZException.printUserMessage(stderr);
        wSize = pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize;
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
 * @param[in] pArrayCount    For a field with type pointer : number of atomic data elements pointer points to. For an array, pArrayCount is deduced.
 *                          Defaulted to -1. For all other data types, and if pArrayCount value is < 1, this value is set to 1.
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
zsetKeyFieldValue (_Tp pValue, ZKey & pZKey,const long pFieldRank,ssize_t pArrayCount)
{
ZStatus wSt;

long wType;
ssize_t wNaturalSize, wUniversalSize,wArrayCount;

    if ((pFieldRank<0)||(pFieldRank> pZKey.ZICB->ZKDic->lastIdx()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_OUTBOUND,
                                Severity_Severe,
                                " Field rank is out of key dictionary boundaries. given value is <%ld> while dictionary limits are <0> to <%s>",
                                pFieldRank,
                                pZKey.ZICB->ZKDic->lastIdx());
        return ZS_OUTBOUND;
        }

//first get ZType_type of input data and test its compatibility with target key field

    wSt=zgetZType_T<_Tp>(wType,wNaturalSize,wUniversalSize,pArrayCount,false) ; // with tolerance to pointers
    if (wSt!=ZS_SUCCESS)
                return wSt;
    wSt=testKeyTypes(wType, pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType);
    if (wSt!=ZS_SUCCESS)
        {
        return wSt;
        }

    if (pZKey.Size < pZKey.ZICB->ZKDic->UniversalSize)    // control size of key vs dictionary key internal size : allocate if not enough
        {
        pZKey.allocate(pZKey.ZICB->ZKDic->UniversalSize);
        }

// now extracting and converting

    if ((wType & ZType_StdString)==ZType_StdString)
            {
            wSt= _setKeyFieldValueStdString<_Tp>(&pValue,pZKey,pFieldRank); // addresses the direct reference and the pointer to std::string
            }
        else
            wSt=_setKeyFieldValue<_Tp>(pValue,pZKey,wType,pFieldRank,pArrayCount);

    if (wSt!=ZS_SUCCESS)
                    return wSt;

    pZKey.FieldPresence[pFieldRank]=true;
} //zsetKeyFieldValue

/** @endcond */

/**
 * @brief ZKey::setFieldValue loads the value (auto) pValue to the key field (defined in key dictionary) of rank pFieldRank
 *
 * @note GNU C++ compiler converts arrays to pointers when using 'auto' clause.
 * In case of arrays, use template ZKey::setFieldValueT
 * @note
 * pArrayCount is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArrayCount is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArrayCount is not necessary because deduced from size of string.
 *
 *@bug the default value of -1 for pArrayCount does not work with GNU compiler because of the use of 'auto' clause.
 * so that, pArrayCount should systematically be mentionned
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
 * @param[in] pArrayCount Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
template <class _Tp>
ZStatus
ZKey::setFieldValue (const long pFieldRank, _Tp pValue, const long pArrayCount)
{
    return zsetKeyFieldValue<decltype(pValue)>(pValue,*this,pFieldRank,pArrayCount);
}
/**
 * @brief ZKey::setFieldValue loads the value (auto) pValue to the key field (defined in key dictionary)
 * with a name that matches given string.
 *
 *@bug the default value of -1 for pArrayCount does not work with GNU compiler because of the use of 'auto' clause.
 * so that, pArrayCount should systematically be mentionned
 *
 * @note GNU C++ compiler converts arrays to pointers when using 'auto' clause.
 * In case of arrays, use template ZKey::setFieldValueT
 * @note
 * pArrayCount is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArrayCount is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArrayCount is not necessary because deduced from size of string.
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
 * @param[in] pArrayCount Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
template <class _Tp>
ZStatus
ZKey::setFieldValue (const char* pFieldName,_Tp pValue, const long pArrayCount)
{
    long wFieldRank=ZICB->ZKDic->zsearchFieldByName(pFieldName);
    if (wFieldRank<0)
            return ZException.getLastStatus();
    return zsetKeyFieldValue<decltype(pValue)>(pValue,*this,wFieldRank,pArrayCount);
}




template <class _Tp>
/**
 * @brief ZKey::setFieldValueT Template routine that loads the value (of type _Tp) pValue to the key field (defined in key dictionary) of rank pFieldRank
 * @note
 * Template is necessary in case of an array to keep the native data type.
 * @note
 * pArrayCount is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArrayCount is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArrayCount is not necessary because deduced from size of string.
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
 * @param[in] pArrayCount Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 * @return
 */
ZStatus
ZKey::setFieldValueT (const long pFieldRank,_Tp pValue, const long pArrayCount)
{
    return zsetKeyFieldValue<_Tp>(pValue,*this,pFieldRank,pArrayCount);
}
template <class _Tp>
/**
 * @brief ZKey::setFieldValueT Template routine that loads the value (of type _Tp) pValue to the key field (defined in key dictionary)
 *  with a name that matches given string.
 * @note
 * Template is necessary in case of an array to keep the native data type.
 * @note
 * pArrayCount is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArrayCount is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArrayCount is not necessary because deduced from size of string.
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
 * @param[in] pArrayCount Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 * @return
 */
ZStatus
ZKey::setFieldValueT (const char* pFieldName,_Tp pValue, const long pArrayCount)
{
    long wFieldRank=ZICB->ZKDic->zsearchFieldByName(pFieldName);
    if (wFieldRank<0)
            return ZException.getLastStatus();
    return zsetKeyFieldValue<_Tp>(pValue,*this,wFieldRank,pArrayCount);
}



//-------------------End Build Key from Value routines-----------------------------------


/** @} */ // end ZKeyGroup

//------------Functions-----------------------------------------
template <class _Tp>
ZStatus
//getAtomicZType(const size_t pTypeHashCode,long& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize);
getAtomicZType(_Tp pValue,ZTypeBase& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize);

#ifdef __COMMENT__
/**
 * @brief _reverseByteOrder reverse the data byte order of an atomic value of type _Tp and returns a _Tp data with the reversed value in it.
 * @param[in] pValue data value to reverse
 * @return
 */
template<class _Tp>
static inline
_Tp _reverseByteOrder(_Tp &pValue)
{
_Tp wValue = pValue;
    int wj=0;
    int wi=sizeof(pValue)-1;

    unsigned char* wPtr = (unsigned char*)&pValue;
    unsigned char* wPtr1 = (unsigned char*)&wValue;
    while(wi >= 0)
            {
            wPtr1[wj] =wPtr[wi];
            wj++;
            wi--;
            }
    return wValue;
}

#endif // __COMMENT__


#endif // ZINDEXTYPE_H
