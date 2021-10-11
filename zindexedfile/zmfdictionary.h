#ifndef ZMFDICTIONARY_H
#define ZMFDICTIONARY_H


#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zkeydictionary.h>
#include <ztoolset/zarray.h>

namespace  zbs {

class ZMFDictionary : public ZMetaDic
{
  ZMFDictionary& _copyFrom(ZMFDictionary& pIn);
public:
  ZMFDictionary();
  ZMFDictionary(ZMFDictionary& pIn);
  ZMFDictionary(ZMFDictionary&& pIn);
  ~ZMFDictionary()
  {
    while (KeyDic.count()>0)
      delete KeyDic.popR();
  }
  ZArray<ZSKeyDictionary*> KeyDic;


  long searchKey(const utf8String& pKeyName)
  {
    for (long wi=0;wi<KeyDic.count();wi++)
      if (KeyDic[wi]->IndexName==pKeyName)
        return wi;
    return -1;
  }
  long searchKeyCase(const utf8String& pKeyName)
  {
    for (long wi=0;wi<KeyDic.count();wi++)
      if (KeyDic[wi]->IndexName.isEqualCase(pKeyName))
        return wi;
    return -1;
  }

  long addKey(ZSKeyDictionary*pIn)
  {
    ZSKeyDictionary*wIn=new ZSKeyDictionary(pIn);
    wIn->MetaDic=this;
    return KeyDic.push(wIn);
  }

  ZDataBuffer& _exportAppend(ZDataBuffer& pZDB);
  size_t _import(unsigned char* &pPtrIn);

  utf8String toXml(int pLevel);
  ZStatus fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

};


class ZMFDicExportHeader
{
public:
  ZMFDicExportHeader() {}
  void set(const ZMFDictionary *pDic);
  uint8_t   BlockId=ZBID_MDIC;
  uint32_t  IndexCount=0;
  ZDataBuffer& _exportAppend(ZDataBuffer& pZDB);
  size_t _import(unsigned char* &pPtrIn);
};

} // namespace zbs
#endif // ZMFDICTIONARY_H
