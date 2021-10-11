#ifndef ZINDEXFIELD_H
#define ZINDEXFIELD_H

#include <zindexedfile/zsindextype.h>
#include <zcrypt/checksum.h>
#include <zindexedfile/zfielddescription.h>

#pragma pack(push)
#pragma pack(0)
/* no MDic rank is stored */
struct ZSIndexField_Out
{
//    size_t      MDicRank;       //!< reference to Metadictionary row
//    uint64_t    NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
//    uint64_t    UniversalSize;  //!< length of the field when stored into Key (Field dictionary internal format size)
//    uint32_t    ArrayCount;      //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.
//    ZTypeBase   ZType;           //!< Type mask of the Field @see ZType_type


    uint32_t    MDicRank;
    uint32_t    KeyOffset;      //!< Offset of the Field from the beginning of Key record
    char        Hash[cst_md5];

    void clear()
    {
      memset(Hash,0,sizeof(Hash));
      KeyOffset=0;
      MDicRank=0;
    }
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

namespace zbs {

#pragma pack(push)
#pragma pack(0)
class ZSIndexField_exp
{
public:

  ZSIndexField_exp()
  {
    MDicRank=0;
    KeyOffset=0;
    memset(Hash,0,cst_md5);
  }
  uint32_t    MDicRank=0;     //!< reference to Metadictionary row
  uint32_t    KeyOffset=0;    //!< Offset of the Field from the beginning of Key record
  char        Hash[cst_md5];  //!< reference to Metadic field
//  uint64_t    NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
//  uint64_t    UniversalSize;  //!< length of the field when stored into Key (Field dictionary internal format size)
//  uint32_t    ArrayCount;     //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.
//  ZTypeBase   ZType;          //!< Type mask of the Field @see ZType_type
};
#pragma pack(pop)

class ZSIndexField
{
public:
  ZSIndexField& _copyFrom(const ZSIndexField& pIn);
  ZSIndexField()=default;
  ZSIndexField(const ZSIndexField& pIn) {_copyFrom(pIn);}
  ZSIndexField(const ZSIndexField&& pIn) {_copyFrom(pIn);}

  ZSIndexField& operator = (const ZSIndexField& pIn) {return _copyFrom(pIn);}


    void clear();

/* to and from Xml are managed within zkeydictionary
    utf8String toXml(int pLevel);
    int fromXml(zxmlElement* pFieldNode, ZaiErrors* pErrorlog);
*/
    ZDataBuffer& _export(ZDataBuffer& pZDBExport) const;
    ZDataBuffer _export() const;

    static ZSIndexField_Out _exportConvert(ZSIndexField& pIn, ZSIndexField_Out *pOut);
    static ZSIndexField _importConvert(ZSIndexField& pOut, ZSIndexField_Out *pIn);

    size_t _import(unsigned char *&pPtrIn);


    uint32_t    MDicRank=0;       //!< reference to Metadictionary row : not stored in XML
    uint32_t    KeyOffset=0;      //!< Offset of the Field from the beginning of Key record computed using universal formats
    md5         Hash;             //!< unique reference to meta dictionary field definition (stored in XML)
//    uint64_t    NaturalSize=0;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
//    uint64_t    UniversalSize=0;  //!< length of the field when stored into Key (Field dictionary internal format size)
//    ZTypeBase   ZType=0;          //!< Type mask of the Field @see ZType_type

};


} // namespace zbs
#endif // ZINDEXFIELD_H
