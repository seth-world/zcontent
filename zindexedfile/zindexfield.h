#ifndef ZINDEXFIELD_H
#define ZINDEXFIELD_H

#include <zindexedfile/zsindextype.h>


typedef utf8String ZFieldName_type ;

#pragma pack(push)
#pragma pack(0)
struct ZSIndexField_strOut
{
    size_t      MDicRank;       //!< reference to Metadictionary row
    uint64_t    NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
    uint64_t    UniversalSize;  //!< length of the field when stored into Key (Field dictionary internal format size)
    uint32_t    ArrayCount;      //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.
    ZTypeBase   ZType;           //!< Type mask of the Field @see ZType_type

    ZDataBuffer& _export(ZDataBuffer& pZDBExport);

};
#pragma pack(pop)

/**

 * @brief ZSIndexField_struct Dictionary single field definition  :
 *              one key may be composed of many single fields not necessarily contiguous in the master file's data structure

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

class ZSIndexField
{
public:
  ZSIndexField& _copyFrom(ZSIndexField& pIn);
  ZSIndexField()=default;
  ZSIndexField(ZSIndexField& pIn) {_copyFrom(pIn);}
  ZSIndexField(ZSIndexField&& pIn) {_copyFrom(pIn);}

  ZSIndexField& operator = (ZSIndexField& pIn) {return _copyFrom(pIn);}


    size_t      MDicRank;       //!< reference to Metadictionary row
    uint64_t    NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
    uint64_t    UniversalSize;  //!< length of the field when stored into Key (Field dictionary internal format size)
    uint32_t    ArrayCount;     //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.
    ZTypeBase   ZType;          //!< Type mask of the Field @see ZType_type
    uint64_t    KeyOffset;      //!< Offset of the Field from the beginning of Key record
//    ZFieldName_type  Name;      //!< short string (max 15 char) containing the name of the field (for readability purpose, not necessary for key processing)

/*    bool isAtomic(void) {return ZType & ZType_Atomic ;}
    bool isArray(void) {return ZType & ZType_Array ;}
    bool isSigned(void) {return ZType & ZType_Signed ;}
    bool isEndian(void) {return ZType & ZType_Endian ;}
    bool isUnknown(void){return (ZType==ZType_Unknown);}*/

    void clear();

    utf8String toXml(int pLevel);
    int fromXml(zxmlElement* pFieldNode, ZaiErrors* pErrorlog);

    ZDataBuffer& _export(ZDataBuffer& pZDBExport);

    static ZSIndexField_strOut _exportConvert(ZSIndexField& pIn, ZSIndexField_strOut *pOut);
    static ZSIndexField _importConvert(ZSIndexField& pOut, ZSIndexField_strOut *pIn);

    ZSIndexField& _import(unsigned char* pZDBImport_Ptr);

};


#endif // ZINDEXFIELD_H
