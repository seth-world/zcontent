#ifndef ZKEYDICTIONARY_H
#define ZKEYDICTIONARY_H

#include <ztoolset/zarray.h>

#include <zcontent/zindexedfile/zindextype.h>
#include <zcontent/zindexedfile/zindexfield.h>
#include <zcontent/zcontentcommon/zcontentconstants.h>


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
  ZSort_Type  Duplicates=ZST_Nothing;   // Allow duplicates = 1 otherwise do not allow duplicates
  uint8_t     Forced=0;       // guessed size is forced = 1 otherwise guessed size is computed
  uint32_t    KeyGuessedSize=0;
  ZAExport    ZAE;            // base ZArray export data

  void setFromPtr(const unsigned char *&pPtrIn);

  void set(const ZKeyDictionary& pIn);

  void _convert();
  void serialize();
  void deserialize();

  bool isReversed() const
  {
    if (EndianCheck==cst_EndianCheck_Reversed) return true;
    return false;
  }
  bool isNotReversed() const
  {
    if (EndianCheck==cst_EndianCheck_Normal) return true;
    return false;
  }

  //  uint64_t    NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
  //  uint64_t    UniversalSize;  //!< length of the field when stored into Key (Field dictionary internal format size)
  //  uint32_t    ArrayCount;     //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.
  //  ZTypeBase   ZType;          //!< Type mask of the Field @see ZType_type
};


/*
class KeyDic_Pack
{
public:
  utf8_t  Name[cst_fieldnamelen];
  long    KeyGuessedSize;
  uint8_t Duplicates;
  KeyDic_Pack()=default;
  KeyDic_Pack(const KeyDic_Pack& pIn) {_copyFrom(pIn);}
  KeyDic_Pack(ZKeyDictionary* pIn) {set(*pIn);}
  KeyDic_Pack& _copyFrom(const KeyDic_Pack& pIn);

  KeyDic_Pack& operator = (const KeyDic_Pack& pIn) {return _copyFrom(pIn);}
  KeyDic_Pack& operator = (const KeyDic_Pack&& pIn) {return _copyFrom(pIn);}

  KeyDic_Pack& set(ZKeyDictionary &pIn);

  void setName(const utf8_t* pName);
  utf8String getName();
};
*/
#pragma pack(pop)


class ZKeyDictionary : public ZArray<ZIndexField>
{
typedef ZArray<ZIndexField> _Base;

public:
  ZKeyDictionary(ZMFDictionary*pMDic) ;
  ZKeyDictionary(const utf8String& pName,ZMFDictionary*pMDic) {setName(pName);Dictionary=pMDic;}
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


  ZSort_Type        Duplicates=ZST_Nothing ;
  utf8String        DicKeyName;       // refers to ZICB::IndexName
  utf8VaryingString ToolTip;         //!< help describing the key
  ZMFDictionary*    Dictionary=nullptr;  // Record Dictionary to which Key Dictionary refers : WARNING : not store in xml <keydictionary>

  uint32_t          KeyGuessedSize=0;       // used to force or guess an average key size. Example : key has a varying string component.
  bool              Forced = false;
  uint8_t           Status=0;             // for Xml matching and actions. refers to ZPRES values set.
  /** @brief computeKeyOffsets (re)compute key fields offset, field by field, and returns the key universal size
     */
  uint32_t computeKeyOffsets();
  uint32_t computekeyguessedsize()const;

  ZTypeBase getType(long pKFieldRank)const ;
  uint64_t getUniversalSize(const long pKFieldRank) const;
  uint64_t getNaturalSize(const long pKFieldRank)const ;


  ZStatus addFieldToZKeyByName (const char *pFieldName);
  ZStatus addFieldToZKeyByRank (const zrank_type pMDicRank);

  void report (FILE* pOutput=stdout);

  /* computes the total universal size of the key according its definition and returns this total size */
  uint32_t _reComputeKeySize(void) ;

  ZStatus zremoveField (const long pKDicRank);

  long zsearchFieldByName(const utf8_t* pFieldName) const ;
  long zsearchFieldByName(const utf8String &pFieldName) const ;

  void clear (void);


    utf8VaryingString toXml(int pLevel, int pRank, bool pComment=false);
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


} //namespace zbs


#endif // ZKEYDICTIONARY_H
