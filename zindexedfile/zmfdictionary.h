#ifndef ZMFDICTIONARY_H
#define ZMFDICTIONARY_H


#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zkeydictionary.h>
#include <ztoolset/zarray.h>

namespace  zbs {

class ZMFDictionary : public ZMetaDic
{
  ZMFDictionary& _copyFrom(const ZMFDictionary &pIn);
public:
  ZMFDictionary();
  ZMFDictionary(const utf8String& pName):ZMetaDic(pName) {}
  ZMFDictionary(const ZMFDictionary& pIn);
  ZMFDictionary(const ZMFDictionary &&pIn);

  ZMFDictionary(const ZMetaDic& pIn);

  ~ZMFDictionary()
    {
    while (KeyDic.count()>0)
      delete KeyDic.popR();
    }

  ZArray<ZKeyDictionary*> KeyDic;

  ZKeyDictionary* searchKey(const utf8String& pKeyName)
  {
    for (long wi=0;wi<KeyDic.count();wi++)
      if (KeyDic[wi]->DicKeyName==pKeyName)
        return KeyDic[wi];
    return nullptr;
  }
  ZKeyDictionary* searchKeyCase(const utf8String& pKeyName)
  {
    for (long wi=0;wi<KeyDic.count();wi++)
      if (KeyDic[wi]->DicKeyName.isEqualCase(pKeyName))
        return KeyDic[wi];
    return nullptr;
  }
  long searchKeyRank(const utf8String& pKeyName)
  {
    for (long wi=0;wi<KeyDic.count();wi++)
      if (KeyDic[wi]->DicKeyName==pKeyName)
        return wi;
    return -1;
  }
 long searchKeyRankCase(const utf8String& pKeyName)
  {
    for (long wi=0;wi<KeyDic.count();wi++)
      if (KeyDic[wi]->DicKeyName.isEqualCase(pKeyName))
        return wi;
    return -1;
  }

  /**
   * @brief addKey add index key definition pIn to current ZMFDictionary.
   * ZSKeyDictionary::DicKeyName is given by pIn DicKeyName.
   * @return Key rank or -1 if errored
   */
  ZStatus addKey(ZKeyDictionary*pIn, long &pOutKeyRank);
  /**
   * @brief addKey add index key definition pIn to current ZMFDictionary.
   * ZSKeyDictionary::DicKeyName is replaced with pKeyName.
   * @return Key rank or -1 if errored
   */
  ZStatus addKey(ZKeyDictionary*pIn, const utf8String& pKeyName, long &pOutKeyRank);

  long removeKey(const utf8String& pName)
    {
    for (long wi=0;wi<KeyDic.count();wi++)
      if (KeyDic[wi]->DicKeyName==pName)
        return KeyDic.erase(wi);
    return -1;
    }

  long removeKey(const long pKeyRank)
    {
    if ((pKeyRank<0)||(pKeyRank>KeyDic.count()))
        return -1;
    return KeyDic.erase(pKeyRank);
    }

  long removeKey(const ZKeyDictionary* pKey)
    {
    for (long wi=0;wi<KeyDic.count();wi++)
      if (KeyDic[wi]==pKey)
        return KeyDic.erase(wi);
    return -1;
    }


  ZDataBuffer& _exportAppend(ZDataBuffer& pZDB);
  ZStatus _import(const unsigned char *&pPtrIn);

  /** @brief XmlSaveToString  saves the whole master dictionary (ZMFDictionary) including its defined keys */
  utf8VaryingString XmlSaveToString(bool pComment);
  utf8VaryingString toXml(int pLevel,bool pComment);

  /** @brief XmlSaveToString  loads the whole master dictionary (ZMFDictionary) including its defined keys */
  ZStatus XmlLoadFromString(const utf8String &pXmlString, ZaiErrors* pErrorlog);
  ZStatus fromXml(zxmlNode* pZmfDicNode, ZaiErrors* pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

};

#pragma pack(push)
#pragma pack(1)
class ZMFDicExportHeader
{
public:
  ZMFDicExportHeader() {}
  void set(const ZMFDictionary *pDic);
  uint32_t  StartSign=cst_ZBLOCKSTART; /* StartSign word that mark start of data */
  uint8_t   BlockId=ZBID_MDIC;
  uint32_t  DicKeyCount=0;
  ZDataBuffer& _exportAppend(ZDataBuffer& pZDB);
  ZStatus _import(const unsigned char *&pPtrIn);
};
#pragma pack(pop)
} // namespace zbs
#endif // ZMFDICTIONARY_H
