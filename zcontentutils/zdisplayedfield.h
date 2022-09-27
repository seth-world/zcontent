#ifndef ZDISPLAYEDFIELD_H
#define ZDISPLAYEDFIELD_H

#include <zcontent/zindexedfile/zmfdictionary.h>
#include <zcontent/zindexedfile/zkeydictionary.h>
#include <QStandardItem>

namespace zbs {
/* reference of that to be stored as data within QStandardItem */
/* usage of dictionary is deprecated */
class ZKeyHeaderRow //: public ZKeyDictionary
{
public:
//  ZKeyHeaderRow(ZMFDictionary*pMDic) : ZKeyDictionary(pMDic) {}
//  ZKeyHeaderRow(ZKeyDictionary* pKeyDic) : ZKeyDictionary(pKeyDic) {set(pKeyDic);}
  ZKeyHeaderRow() {}
  ZKeyHeaderRow(const ZKeyHeaderRow& pIn)  {ZKeyHeaderRow::_copyFrom(pIn);}
  ZKeyHeaderRow(const ZKeyHeaderRow* pIn)  {ZKeyHeaderRow::_copyFrom(*pIn);}



  ZKeyHeaderRow& _copyFrom(const ZKeyHeaderRow& pIn) {
//    ZKeyDictionary::_copyFrom(pIn);
    DicKeyName=pIn.DicKeyName;
    ToolTip=pIn.ToolTip;
    KeyUniversalSize=pIn.KeyUniversalSize;
    Duplicates=pIn.Duplicates;
    return *this;
  }

  ZKeyHeaderRow& operator = (const ZKeyHeaderRow& pIn) {return _copyFrom(pIn); }

  void set(const ZKeyDictionary& pKeyDic) ;
  void set(const ZKeyDictionary* pKeyDic) ;
  ZKeyDictionary get(ZMFDictionary *pDic) ;

  uint32_t          KeyUniversalSize=0;
  uint8_t           Duplicates=0;
  utf8String        DicKeyName;       // refers to ZICB::IndexName
  utf8VaryingString ToolTip;         //!< help describing the key
};
/**
 * @brief The ZKeyFieldRow class usage is to be displayed within DicEdit conversation from fieldTBv table view
 */
class ZKeyFieldRow //: public ZIndexField
{
public:
  ZKeyFieldRow()=default;
//  ZKeyFieldRow(const ZIndexField& pKeyField):ZIndexField(pKeyField) {set(pKeyField);}
  ZKeyFieldRow(QStandardItem* pField) {setFromQItem(pField);}
  ZKeyFieldRow(const ZKeyFieldRow& pIn) {_copyFrom(pIn);}

  ZTypeBase         ZType=ZType_Nothing;
  utf8VaryingString Name;
  uint32_t          UniversalSize=0;
  uint32_t          KeyOffset=0;      // Offset of the Field from the beginning of Key record computed using universal formats
  md5               Hash;             // unique reference to meta dictionary field definition (stored in XML)
  //  uint32_t      MDicRank=0;       // reference to Metadictionary row : not stored in XML
  //  uint32_t      KeyOffset=0;      // Offset of the Field from the beginning of Key record computed using universal formats
  //  md5           Hash;             // unique reference to meta dictionary field definition (stored in XML)

  QStandardItem*    FieldItem=nullptr;

  ZKeyFieldRow& _copyFrom(const ZKeyFieldRow& pIn) {
 //   ZIndexField::_copyFrom(pIn);
    //    MDicRank=pIn.MDicRank;
    //    KeyOffset=pIn.KeyOffset;
    //    Hash=pIn.Hash;
    UniversalSize=pIn.UniversalSize;
    Name =pIn.Name;
    ZType = pIn.ZType;
    KeyOffset = pIn.KeyOffset;
    Hash = pIn.Hash;
    FieldItem=pIn.FieldItem;
    return *this;
  }

  ZKeyFieldRow& operator = (const ZKeyFieldRow& pIn) {return _copyFrom(pIn); }

  bool setFromQItem(QStandardItem* pField);
  void setFromKeyDic(ZIndexField *pField);
};


} // namespace zbs

#endif // ZDISPLAYEDFIELD_H
