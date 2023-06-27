#include "zdisplayedfield.h"
#include <zdatareference.h>


void ZKeyHeaderRow::set(ZKeyDictionary& pKeyDic) {
//  ZKeyDictionary::_copyFrom(pKeyDic);
  DicKeyName=pKeyDic.DicKeyName;
  ToolTip=pKeyDic.ToolTip;
  KeyUniversalSize=0;
  Duplicates=pKeyDic.Duplicates;
  KeyUniversalSize=pKeyDic._reComputeKeySize();
}
void ZKeyHeaderRow::set(ZKeyDictionary* pKeyDic) {
//  ZKeyDictionary::_copyFrom(pKeyDic);
  DicKeyName=pKeyDic->DicKeyName;
  ToolTip=pKeyDic->ToolTip;
  KeyUniversalSize=0;
  Duplicates=pKeyDic->Duplicates;
  KeyUniversalSize=pKeyDic->_reComputeKeySize();
}

ZKeyDictionary ZKeyHeaderRow::get(ZMFDictionary* pDic) {

  ZKeyDictionary wOutKey(pDic);
  wOutKey.DicKeyName =DicKeyName;
  wOutKey.Duplicates =Duplicates;
  wOutKey.ToolTip =ToolTip;
  return wOutKey;
}

/*
void ZKeyFieldRow::set(const ZMetaDic& pMetaDic, const ZIndexField& pKeyField) {
  MDicRank=pKeyField.MDicRank;
  KeyOffset=pKeyField.KeyOffset;
  Hash=pKeyField.Hash;
  UniversalSize=pMetaDic.Tab[MDicRank].UniversalSize;
  Name =pMetaDic.Tab[MDicRank].getName();
  ZType = pMetaDic.Tab[MDicRank].ZType;
}*/

bool ZKeyFieldRow::setFromQItem(QStandardItem *pField) {
  //  ZIndexField::_copyFrom(pKeyField);
  if (!pField)
    abort();
  QVariant wV;
  ZDataReference wDRef;
  wV=pField->data(ZQtDataReference);
  if (!wV.isValid())
    return false;
  wDRef=wV.value<ZDataReference>();
  long MDicRank=pField->row();

  ZFieldDescription* wFDesc=wDRef.getPtr<ZFieldDescription*>();

  KeyOffset=0;
  Hash=wFDesc->Hash;
  UniversalSize=wFDesc->UniversalSize;
  ZType=wFDesc->ZType;
  Name=wFDesc->getName();
  FieldItem=pField;
  return true;
}

void ZKeyFieldRow::setFromKeyDic(ZIndexField *pField) {
  //  ZIndexField::_copyFrom(pKeyField);
  if (!pField)
    abort();

  KeyOffset=pField->KeyOffset;
  Hash=pField->Hash;
  Name = pField->KeyDic->Dictionary->Tab(pField->MDicRank).getName().toCChar();

  return;
}

