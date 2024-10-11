#ifndef ZTYPETYPE_H
#define ZTYPETYPE_H
#include <stdint.h>
#include <ztoolset/zerror.h>
/**
 * @brief The ZType_type enum  ZIndex key data type (component of ZIndexItem_struct )
 *
 * This enum qualifies any data type that will be processed to build key dictionaries.
 *
 *@image latex ztype_type.jpg "Data Type bit mask" width=10cm
 *@image html ztype_type.jpg "Data Type bit mask" width=10cm
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
typedef uint32_t ZTypeBase;

enum ZType_type : ZTypeBase
{
    ZType_Nothing        = 0,           //!< NOP

    ZType_Atomic         = 0x01000000,  //!< data type is elementary
    ZType_Endian         = 0x00100000,  //!< data type is subject to little endian conversion  see: @ref ZIndexDataConversion
    ZType_Signed         = 0x00010000,  //!< data type is signed. Sign impacts of data conversion see: @ref ZIndexDataConversion
    ZType_Pointer        = 0x02000000,  //!< data type is a pointer
    ZType_Array          = 0x04000000,  //!< data type is an array (content has to be further described}
//    ZType_Enum           = 0x08000000,  //!< data type is enum
    ZType_ByteSeq        = 0x10000000,  //!< data type is a sequence of bytes regardless any type, only defined by a start address and a length. Warning data type is a pointer

    ZType_AtomicMask     = 0x00FF0FFF, // Elementary + signed + endian
    ZType_VaryingMask    = 0x0000F000,
    ZType_SignedMask     = 0x000F0000,
    ZType_EndianMask     = 0x00F00000, //!< used to negate any preceeding data structure indication and keep atomic data type.
    ZType_StructureMask  = 0xFF000000, //!< used to negate any preceeding data structure indication and keep atomic data type.

//    ZType_Huge           = 0x00000800,    //!< size must be on int64_t

    ZType_Class          = 0x20000000, //!< Field is a class or a struct (NON POLYMORPHIC class):
    //!            NO CONTENT CONVERSION of the underneeth data will be made excepted for ZDate, ZDateFull etc. Use it carefully.

    ZType_VaryingLength  = 0x00001000, //!< By default any data structure is fixed sized. Varying length means it is not. Used with strings and pointers.

    ZType_String         = 0x40000000, //!< generic string type

    ZType_StdString      = 0x40101FF1,  //!< Field is a std::string class varying length : not used for storage, only for conversion
    ZType_StdWString     = 0x40101FF8,  //!< Field is a std::wstring class varying length : not used for storage, only for conversion

    ZType_Blob           = 0x10001000, //! blob : a varying sized content of bytes : See ByteSeq. Corresponds to ZDataBuffer
//   ZType_LBlob          = 0x10001800, //! Long blob : corresponds to ZDataBuffer_long
//
// atomic data types - is combined (ored) with other attributes to give the full data type
//
    ZType_UChar          = 0x00000202,    //!< unsigned char - mostly used for encoding/decoding
    ZType_Char           = 0x00000201,    //!< char - a single ascii character - NOT SUBJECT to leading byte despite its signed status- use int8_t if a one byte numeric value is required

    ZType_U8             = 0x00000004,    //!< arithmetic byte
    ZType_S8             = 0x00010004,    //!< arithmetic signed byte
    ZType_U16            = 0x00100008,    //!< arithmetic 2 bytes value unsigned (no leading sign byte)

    ZType_WChar          = 0x00110208,    //!< WChar is a char string based on uint16_t wchar_t (no leading sign byte) but subject to endian conversion
    ZType_WUChar         = 0x00100208,    //!< WUChar is a char string based on uint16_t unsigned wchar_t subject to endian conversion

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

    ZType_AtomicWUChar   = 0x0110000A,    //!< unsigned wchar_t (is a char and an uint16_t
    ZType_AtomicWChar    = 0x01100009,    //!< wchar_t - a single double character - NOT SUBJECT to leading byte despite its signed status- use int8_t if a one byte numeric value is required


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
/*
    ZType_PointerChar    = 0x02000001,    //!< this is the C string
    ZType_PointerUChar   = 0x02010002,    //!< special string specifically used by encoding
*/
    ZType_ArrayChar      = 0x04000001,    //!< this is a C string stored as a key value (CString must be fixed length, with variable length content)
    ZType_ArrayUChar     = 0x04010002,    //!< and also could be used by encoding (same as above, but may be used for encoding/decoding)

    /**  Deprecated -- not to be used
    ZType_EnumByte       = 0x08110004,    //!< This is enum uses a long
    ZType_EnumWord       = 0x08110008,    //!< This is enum uses a long long
    ZType_EnumLong       = 0x08110010,    //!< This is enum uses a long
    ZType_EnumLongLong   = 0x08110020,    //!< This is enum uses a long long
    */
    ZType_bit            = 0x00000400,
    ZType_bitset         = 0x10000400,    //!< bitset is a ZType_ByteSeq with atomic data 'bit'

    ZType_CharVaryingString = ZType_String|ZType_VaryingLength|ZType_Char,

    ZType_Utf8VaryingString = ZType_String|ZType_VaryingLength|ZType_U8,
    ZType_Utf16VaryingString = ZType_String|ZType_VaryingLength|ZType_U16,
    ZType_Utf32VaryingString = ZType_String|ZType_VaryingLength|ZType_U32,

    ZType_CharFixedString   = ZType_String|ZType_Char,

    ZType_Utf8FixedString   = ZType_String|ZType_U8,
    ZType_Utf16FixedString  = ZType_String|ZType_U16,
    ZType_Utf32FixedString  = ZType_String|ZType_U32,


    // ==============deprecated=============================
    ZType_VaryingCString = ZType_String|ZType_VaryingLength|ZType_Char ,  //!< data type is a C string : varying length of char ended with '\0'
    ZType_VaryingWString = ZType_String|ZType_VaryingLength|ZType_WChar,  //!< data type is a W string : varying length of char ended with L'\0'
    ZType_FixedCString   = ZType_String|ZType_Char,  //!< a fixed length string containing a c string :templateWString and derived
    ZType_FixedWString   = ZType_String|ZType_WChar,  //!< a fixed length string containing a W string :templateWString and derived
    ZType_FixedWUString   = ZType_String|ZType_WUChar,  //!< a fixed length string containing a unsigned W string :templateWString and derived
    // ============== end deprecated=============================

    ZType_CString        = ZType_Pointer|ZType_VaryingLength|ZType_Char, //!< standard cstring (char*) ZType_Pointer | ZType_Char
    ZType_WString        = ZType_Pointer|ZType_VaryingLength|ZType_WChar, //!< standard cstring (wchar_t*) ZType_Pointer | ZType_WChar


//    ZType_ZDate          = 0x21100010,    //!< a struct giving uint32_t  is ZDate (export and import)
//    ZType_ZDateFull      = 0x21100020,    //!< a struct giving uint64_t  is ZDate (export and import)

    ZType_ZDate          = ZType_Class | ZType_U32,    // a struct giving uint32_t  is ZDate (export and import)
    ZType_ZDateFull      = ZType_Class | ZType_U64,    // a struct giving uint64_t  is ZDateFull (export and import)

    ZType_CheckSum       = ZType_Class | ZType_UChar, //!< a struct containing fixed array of unsigned char

    ZType_Unknown        = 0xF00FFFFF     //! Unmanaged data type : generally an error
} ;
class ZDataBuffer;
const char *decode_ZType (ZTypeBase pType) ;
ZStatus encodeZTypeFromString (ZTypeBase &pZType , ZDataBuffer &pString);

#endif // ZTYPETYPE_H
