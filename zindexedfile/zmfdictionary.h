#ifndef ZMFDICTIONARY_H
#define ZMFDICTIONARY_H


#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zkeydictionary.h>
#include <ztoolset/zarray.h>

namespace  zbs {

class ZMFDictionary : public ZMetaDic
{
public:
  ZMFDictionary& _copyFrom(const ZMFDictionary &pIn);
public:
  ZMFDictionary();
//  ZMFDictionary(const utf8String& pName):ZMetaDic(pName) {}
  ZMFDictionary(const ZMFDictionary& pIn);
  ZMFDictionary(const ZMFDictionary &&pIn);

  ZMFDictionary(const ZMetaDic& pIn);

  ~ZMFDictionary() {
    clearData();
    }

  bool Active=false;

  unsigned long     Version = 1000000UL;
  ZDateFull         CreationDate;
  ZDateFull         ModificationDate;

  utf8VaryingString DicName;         /* name of the entity described by dic */
  checkSum *        CheckSum=nullptr;/* to check if meta dictionary has changed or not */

  ZMFDictionary& operator = (const ZMFDictionary& pIn) {return _copyFrom(pIn);}

  ZArray<ZKeyDictionary*> KeyDic;

  void setModified ();

  void clearData() {
    zdelete(CheckSum);
    while (KeyDic.count()>0) {
      ZKeyDictionary* wKDic=nullptr;
      KeyDic.popRP(&wKDic);
      zdelete (wKDic);
    }
    ZMetaDic::clear();
  }

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

  ZIndexField* searchKeyFieldCase(const utf8String& pKeyFieldName)
  {
    long wR=-1;
    for (long wi=0;wi<KeyDic.count();wi++)
      if ((wR=KeyDic[wi]->hasFieldNameCase(pKeyFieldName)) > -1 ) {
        return &KeyDic[wi]->Tab(wR);
      }
    return nullptr;
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

  void setDicName(const utf8String& pName) {DicName=pName;}
  void setVersion(unsigned int pVersion,unsigned int pMajor,unsigned int pMinor)
    {
    Version=pVersion*1000000;
    Version+=pMajor*1000;
    Version+=pMinor;
    }

  checkSum* getCheckSum(void) {return CheckSum;}
  void generateCheckSum (void)
    {
    ZDataBuffer wMetaDic;
    _exportAppend(wMetaDic);
    if (CheckSum!=nullptr)
      delete CheckSum;
    CheckSum=wMetaDic.newcheckSum();
    return;
    }

  ZDataBuffer& _exportAppend(ZDataBuffer& pZDB);
  ZStatus _import(const unsigned char *&pPtrIn);


  /** @brief XmlSaveToString  saves the whole master dictionary (ZMFDictionary) including its defined keys */
  utf8VaryingString XmlSaveToString(bool pComment);
  utf8VaryingString toXml(int pLevel,bool pComment);

  /** @brief XmlSaveToString  loads the whole master dictionary (ZMFDictionary) including its defined keys */
  ZStatus XmlLoadFromString(const utf8String &pXmlString,bool pCheckHash, ZaiErrors* pErrorlog);
  ZStatus fromXml(zxmlNode* pZmfDicNode, bool pCheckHash,ZaiErrors* pErrorlog);

};
/*                ----------------------------
                    Dictionary export schema
                  ----------------------------

_________________________Beginning of export data _________________________

  ZMFDicExportHeader structure (no start block)
  ...
  DicName (UVF format)
  _________Meta dictionary _exportAppendMetaDicFlat()______________________

                    |   cst_ZMSTART (0xF6F6F6F6)
                    |
  ZAExport          |   header data for fields ZArray
                    |

                          |   cst_ZFIELDSTART (0xF4F4F4F4)
                          |
        FieldDesc_Export  |
                          |
        Name (field name) UVF format
        ToolTip           UVF format

                          |   cst_ZFIELDSTART (0xF4F4F4F4)
                          |
        FieldDesc_Export  |
                          |
        Name (field name) UVF format
        ToolTip           UVF format
          ...

  cst_ZBUFFEREND (0xFBFBFBFB)
  _______________End of meta dictionary_____________

  ___________KeyDictionary _exportAppendFlat()_______
        -- Key 0 --
                      | cst_ZMSTART (0xF6F6F6F6)
                      | uint16_t endian check
  ZKeyDictionary_Exp  | uint32_t KeyDicSize
                      | uint32_t fieldNb
                      |
                      |               | cst_ZMSTART (0xF6F6F6F6)
                      | ZAExport      | uint16_t endian check
                      |               | size_t    FullSize
                      |               | size_t    AllocatedSize
                      |               | size_t    CurrentSize
                      |               | size_t    AllocatedElements
                      |               | size_t    FullSize
                      |               | size_t    FullSize
                      |               | ... see ZAExport
                      |               | ssize_t   NbElements
     DicKeyName (UVF)
                   | uint8_t char unit size
                   | uint16_t (UVF_Size_type) unit count
                   | ... string content...

     ZIndexField_Exp - key field content
     ...

-- Key 1 --
    ...
cst_ZBUFFEREND (0xFBFBFBFB)
______________________End of export__________________________________

*/
#pragma pack(push)
#pragma pack(1)
class ZMFDicExportHeader
{
public:
  ZMFDicExportHeader() {}
  void set(ZMFDictionary *pDic);

  /** updates pDic with header data */
  void get(ZMFDictionary& pDic);

  uint8_t       Active=0;
  uint32_t      DicKeyCount=0;
  unsigned long Version=0UL;
  ZDateFull     CreationDate;
  ZDateFull     ModificationDate;
  utf8VaryingString DicName;

  ZDataBuffer& _exportAppend(ZDataBuffer& pZDB);
  ZStatus _import(const unsigned char *&pPtrIn);
};
#pragma pack(pop)
} // namespace zbs
#endif // ZMFDICTIONARY_H
