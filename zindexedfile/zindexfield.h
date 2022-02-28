#ifndef ZINDEXFIELD_H
#define ZINDEXFIELD_H

#include <zindexedfile/zsindextype.h>
#include <zcrypt/checksum.h>
#include <zindexedfile/zfielddescription.h>

#ifdef __COMMENT__
#pragma pack(push)
#pragma pack(1)
/* no MDic rank is stored */
struct ZSIndexField_Exp
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
#endif // __COMMENT__

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
#pragma pack(1)
class ZIndexField_Exp
{
public:

  ZIndexField_Exp()
  {
    MDicRank=0;
    KeyOffset=0;
    memset(Hash,0,cst_md5);
  }
  uint16_t    EndianCheck=cst_EndianCheck_Normal;
  uint32_t    MDicRank=0;     //!< reference to Metadictionary row
  uint32_t    KeyOffset=0;    //!< Offset of the Field from the beginning of Key record
  char        Hash[cst_md5];  //!< reference to Metadic field - does not need to be reverted

  void set(const ZIndexField& pIn);
  void setFromPtr(const unsigned char*& pPtrIn);
  void toZIF(ZIndexField &pZIF);

  void _convert();
  void serialize();
  void deserialize();

  bool isReversed()
  {
    if (EndianCheck==cst_EndianCheck_Reversed) return true;
    return false;
  }
  bool isNotReversed()
  {
    if (EndianCheck==cst_EndianCheck_Normal) return true;
    return false;
  }

//  uint64_t    NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
//  uint64_t    UniversalSize;  //!< length of the field when stored into Key (Field dictionary internal format size)
//  uint32_t    ArrayCount;     //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.
//  ZTypeBase   ZType;          //!< Type mask of the Field @see ZType_type
};

class ZMFDictionary;

class KeyField_Pack : public FieldDesc_Pack
{
public:
  KeyField_Pack()=default;

  KeyField_Pack& _copyFrom(const KeyField_Pack& pIn);

  KeyField_Pack(const KeyField_Pack& pIn) {_copyFrom(pIn);}

  KeyField_Pack& set(ZMFDictionary& pDic,long pKeyRank,long pKeyFieldRank);
  KeyField_Pack& set(ZFieldDescription& pFDesc,long pKeyRank);
  KeyField_Pack& set(const FieldDesc_Pack& pFD_Pack,long pKeyRank);

  bool hasSameHash(const FieldDesc_Pack& pIn) {return memcmp(Hash,pIn.Hash,cst_md5)==0;}

  QDataStream& write(QDataStream &dataStream);
  QDataStream& read(QDataStream &dataStream);

  friend   QDataStream &operator << (QDataStream &out, const KeyField_Pack &myObj)
  {
    out.writeBytes((const char *)&myObj,sizeof(KeyField_Pack)) ;
    return(out);
  }


  long KeyNumber;
  long KeyFieldRank;
  long KeyOffset;
};

#pragma pack(pop)

class ZIndexField
{
public:
  ZIndexField& _copyFrom(const ZIndexField& pIn);
  ZIndexField()=default;
  ZIndexField(const ZIndexField& pIn) {_copyFrom(pIn);}
  ZIndexField(const ZIndexField&& pIn) {_copyFrom(pIn);}

  ZIndexField& operator = (const ZIndexField& pIn) {return _copyFrom(pIn);}


    void clear();

/* to and from Xml are managed within zkeydictionary
    utf8String toXml(int pLevel);
    int fromXml(zxmlElement* pFieldNode, ZaiErrors* pErrorlog);
*/
    ZDataBuffer& _exportAppend(ZDataBuffer& pZDBExport) const;
    ZDataBuffer _export() const;

    static ZIndexField_Exp _exportConvert(ZIndexField& pIn, ZIndexField_Exp *pOut);
    static ZIndexField _importConvert(ZIndexField& pOut, ZIndexField_Exp *pIn);

    /* this routine does not change input data - but updates pPtrIn*/
    void _import(const unsigned char *&pPtrIn);



    uint32_t    MDicRank=0;       // reference to Metadictionary row : not stored in XML
    uint32_t    KeyOffset=0;      // Offset of the Field from the beginning of Key record computed using universal formats
    md5         Hash;             // unique reference to meta dictionary field definition (stored in XML)
//    uint64_t    NaturalSize=0;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
//    uint64_t    UniversalSize=0;  //!< length of the field when stored into Key (Field dictionary internal format size)
//    ZTypeBase   ZType=0;          //!< Type mask of the Field @see ZType_type

};


} // namespace zbs

Q_DECLARE_METATYPE(zbs::KeyField_Pack);   // required for using such structure as variant

#endif // ZINDEXFIELD_H
