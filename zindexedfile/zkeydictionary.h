#ifndef ZKEYDICTIONARY_H
#define ZKEYDICTIONARY_H

#include <ztoolset/zarray.h>

#include <zindexedfile/zsindextype.h>
#include <zindexedfile/zindexfield.h>
#include <QDataStream> /* for Q_DECLARE_METATYPE */

#ifndef __KEYDICTYPE__
#define __KEYDIdCTYPE__
enum ZKeyDic_type {
    ZKD_Nothing         = 0,
    ZKD_NaturalSize     = 1,
    ZKD_UniversalSize   = 2,
    ZKD_KeyOffset       = 4,
    ZKD_RecordOffset    = 8,
    ZKD_ArraySize       = 0x10,
    ZKD_ZType           = 0x20,
    ZKD_Name            = 0x40
};
#endif //__KEYDICTYPE__

namespace zbs {

/**
 * @brief The ZSKeyDictionary class store ZIndex index key fields definition for one key (and therefore one ZIndexFile)
 *
 * ZSKeyDictionary is only a set of pointers to meta dictionary rank.
 * However, as Fields may have varying length it is necessary to use not the meta dictionary itself but the record dictionary
 *
 *
 * Nota Bene: when exporting, only ZArray of fields definition are exported in universal format
 *            when importing, total Natural size and Universal size are computed.
 *
 */

class ZMetaDic;
class ZMFDictionary;


#pragma pack(push)
#pragma pack(1)
class ZKeyDictionary_Exp
{
public:

  ZKeyDictionary_Exp()=default;


  uint32_t    StartSign=cst_ZMSTART;
  uint16_t    EndianCheck=cst_EndianCheck_Normal;
  uint32_t    KeyDicSize=0;   // overall size of exported key dictionary
  uint32_t    FieldNb=0;      // number of key fields
  ZAExport    ZAE;            // base ZArray export data

  void setFromPtr(const unsigned char *&pPtrIn);

  void set(const ZKeyDictionary& pIn);

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

class KeyDic_Pack
{
public:
  utf8_t  Name[cst_fieldnamelen];
  long    KeyUniversalSize;
  uint8_t Duplicates;
  KeyDic_Pack()=default;
  KeyDic_Pack(const KeyDic_Pack& pIn) {_copyFrom(pIn);}
  KeyDic_Pack(const ZKeyDictionary* pIn) {set(*pIn);}
  KeyDic_Pack& _copyFrom(const KeyDic_Pack& pIn);

  KeyDic_Pack& operator = (const KeyDic_Pack& pIn) {return _copyFrom(pIn);}
  KeyDic_Pack& operator = (const KeyDic_Pack&& pIn) {return _copyFrom(pIn);}

  KeyDic_Pack& set(const ZKeyDictionary& pIn);

  void setName(const utf8_t* pName);
  utf8String getName();
};

#pragma pack(pop)


class ZKeyDictionary : public ZArray<ZIndexField>
{
typedef ZArray<ZIndexField> _Base;

public:
  ZKeyDictionary(ZMFDictionary*pMDic) ;
  ZKeyDictionary(const utf8String& pName,ZMFDictionary*pMDic) {setName(pName);MasterDic=pMDic;}
  ZKeyDictionary(const ZKeyDictionary* pIn);
  ZKeyDictionary(const ZKeyDictionary& pIn);
  ~ZKeyDictionary() { } // just to call the base destructor

  ZKeyDictionary& _copyFrom( const ZKeyDictionary& pIn);

  ZKeyDictionary& operator=( ZKeyDictionary& pIn) {return _copyFrom(pIn);}

  void setName(const utf8String& pName) {DicKeyName=pName;}

  bool hasSameContentAs(ZKeyDictionary*pKey);

  long  hasFieldNameCase(const utf8VaryingString& pName);

  long push(ZIndexField& pField) {
    pField.KeyDic = this;
    return _Base::push(pField);
  }
  long insert(ZIndexField& pField, long pIdx) {
    pField.KeyDic = this;
    return _Base::insert(pField, pIdx);
  }

  long insert(ZIndexField* pFieldArray, long pIdx,long pNb) {
    for (long wi = 0; wi < pNb; wi++)
      pFieldArray[wi].KeyDic = this;
    return _Base::insert(pFieldArray, pIdx,pNb);
  }


  uint8_t           Duplicates=0;
  utf8String        DicKeyName;       // refers to ZICB::IndexName
  utf8VaryingString ToolTip;         //!< help describing the key
  ZMFDictionary*    MasterDic=nullptr;  // Record Dictionary to which Key Dictionary refers : WARNING : not store in xml <keydictionary>
                                //  it is stored apart in dedicated <metadic> xml node
//    uint32_t KDicSize;          //!< Size of the dictionary when exported (other fields are not exported) this field comes first
        // nb: KDicSize is already given by ZAExport structure.
//    uint32_t KeyNaturalSize ;     //!< overall size of the key (sum of keyfields NATURAL (total) length). not necessary
//    uint32_t KeyUniversalSize ;   //!< total of key size with internal format MOVED TO ZSICBOwnData
  // WARNING: fields below are not stored (xml) nor exported (icb)
    bool    Recomputed=false;
    uint8_t Status=0;             // for Xml matching and actions. refers to ZPRES values set.
    /** @brief computeKeyOffsets (re)compute key fields offset, field by field, and returns the key universal size
     */
    uint32_t computeKeyOffsets();
    uint32_t computeKeyUniversalSize()const;

    ZTypeBase getType(const long pKFieldRank)const ;
    uint64_t getUniversalSize(const long pKFieldRank) const;
    uint64_t getNaturalSize(const long pKFieldRank)const ;


    ZStatus addFieldToZKeyByName (const char *pFieldName);
    ZStatus addFieldToZKeyByRank (const zrank_type pMDicRank);



    void report (FILE* pOutput=stdout);

    /* computes the total universal size of the key according its definition and returns this total size */
    uint32_t _reComputeKeySize(void) const ;

    ZStatus zremoveField (const long pKDicRank);


/*    template <class _Tp>
    ZStatus zaddField (const char *pFieldName,const size_t pZMFOffset);
    template <class _Tp>
    ZStatus zaddField (descString &pFieldName,const size_t pZMFOffset);
    ZStatus zremoveField (const char *pFieldName);
    ZStatus zremoveField (const long pFieldRank);
    ZStatus zsetField (const long pFieldRank,ZKeyDic_type pZKD,auto pValue);
*/
    long zsearchFieldByName(const utf8_t* pFieldName) const ;
    long zsearchFieldByName(const utf8String &pFieldName) const ;

    void clear (void)
    {
//      KeyNaturalSize=0;
//      KeyUniversalSize=0;
//      KDicSize=0;
//      Recomputed=false;
      _Base::clear();
      return;
    }

    utf8String toXml(int pLevel, int pRank, bool pComment=false);
    ZStatus fromXml(zxmlNode* pKeyDicNode, ZaiErrors* pErrorlog);

    /** @brief _export exports current key dictionary in a ZAExport normalized format into a new ZDataBuffer as result
     */
    ZDataBuffer _export();
    /**
     * @brief _exportAppend same as previous but appends to pZDBExport ZDataBuffer
     */
    ZDataBuffer& _exportAppend(ZDataBuffer& pZDBExport);
    ZStatus _import(const unsigned char *&pZDBImport_Ptr);


    /**
     * @brief _exportAppendFlat This routine does not use ZAExport because of varying elements due to varying string
     * @param pZDBExport
     * @return
     */
    ZDataBuffer& _exportAppendFlat(ZDataBuffer& pZDBExport);
    ZStatus _importFlat(const unsigned char *&pPtrIn);


} ;



/* reference of that to be stored within QStandardItem[0] */
class ZKeyHeaderRow : public ZKeyDictionary
{
public:
  ZKeyHeaderRow(ZMFDictionary*pMDic) : ZKeyDictionary(pMDic) {}
  ZKeyHeaderRow(ZKeyDictionary* pKeyDic) : ZKeyDictionary(pKeyDic) {set(pKeyDic);}

  ZKeyHeaderRow(const ZKeyHeaderRow& pIn) : ZKeyDictionary(pIn) {ZKeyHeaderRow::_copyFrom(pIn);}
  ZKeyHeaderRow(const ZKeyHeaderRow* pIn) : ZKeyDictionary(pIn) {ZKeyHeaderRow::_copyFrom(*pIn);}

  uint32_t          KeyUniversalSize=0;

  ZKeyHeaderRow& _copyFrom(const ZKeyHeaderRow& pIn) {
    ZKeyDictionary::_copyFrom(pIn);
    KeyUniversalSize=pIn.KeyUniversalSize;
    return *this;
  }

  ZKeyHeaderRow& operator = (const ZKeyHeaderRow& pIn) {return _copyFrom(pIn); }

  void set(const ZKeyDictionary& pKeyDic) ;
  void set(const ZKeyDictionary* pKeyDic) ;
  ZKeyDictionary get() ;
};

class ZKeyFieldRow : public ZIndexField
{
public:
  ZKeyFieldRow()=default;
  ZKeyFieldRow(const ZIndexField& pKeyField):ZIndexField(pKeyField) {set(pKeyField);}
  ZKeyFieldRow(const ZKeyFieldRow& pIn) {_copyFrom(pIn);}
  ZTypeBase         ZType=ZType_Nothing;
  utf8VaryingString Name;
  uint32_t          UniversalSize=0;
//  uint32_t      MDicRank=0;       // reference to Metadictionary row : not stored in XML
//  uint32_t      KeyOffset=0;      // Offset of the Field from the beginning of Key record computed using universal formats
//  md5           Hash;             // unique reference to meta dictionary field definition (stored in XML)

  ZKeyFieldRow& _copyFrom(const ZKeyFieldRow& pIn) {
    ZIndexField::_copyFrom(pIn);
//    MDicRank=pIn.MDicRank;
//    KeyOffset=pIn.KeyOffset;
//    Hash=pIn.Hash;
    UniversalSize=pIn.UniversalSize;
    Name =pIn.Name;
    ZType = pIn.ZType;
    return *this;
  }

  ZKeyFieldRow& operator = (const ZKeyFieldRow& pIn) {return _copyFrom(pIn); }

  //  void set(const ZMetaDic& pMetaDic, const ZIndexField& pKeyField) ;
  void set(const ZIndexField& pKeyField) ;
  ZIndexField get() ;
};




} //namespace zbs

Q_DECLARE_METATYPE(zbs::KeyDic_Pack);   // required for using such structure as variant
#endif // ZKEYDICTIONARY_H
