#ifndef ZDICTIONARYFILE_H
#define ZDICTIONARYFILE_H

#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zmfdictionary.h>

#include <stdint.h>

#define __DICTIONARY_EXTENSION__ ".dic"

const unsigned long cst_DictionaryVersionAny = 0xFFFFFFFFFF;

namespace zbs {

//class ZMFD

class ZDicList : public ZArray<ZMFDictionary*> {
public:
  ZDicList()=default;

  ~ZDicList() {
    while (count())
      delete popR();
  }

  ZDicList(const ZDicList& pIn) {_copyFrom(pIn);}


  ZDicList& _copyFrom(const ZDicList& pIn) {
    for (long wi=0;wi < count();wi++) {
      push(new ZMFDictionary(*pIn[wi]));
    }
    return *this;
  }

  ZDicList& operator = (const ZDicList& pIn) {return _copyFrom(pIn);}
};
class ZDicHeaderList : public ZArray<ZMFDicExportHeader*> {
public:
  ZDicHeaderList()=default;

  ~ZDicHeaderList() {
    while (count())
      delete popR();
  }

  ZDicHeaderList(const ZDicHeaderList& pIn) {_copyFrom(pIn);}


  ZDicHeaderList& _copyFrom(const ZDicHeaderList& pIn) {
    for (long wi=0;wi < count();wi++) {
      push(new ZMFDicExportHeader(*pIn[wi]));
    }
    return *this;
  }

  ZDicHeaderList& operator = (const ZDicHeaderList& pIn) {return _copyFrom(pIn);}
};

class ZDictionaryFile : public ZRandomFile , public ZMFDictionary
{
public:
  ZDictionaryFile();

  ZStatus zopen(const uriString &pFilename,const zmode_type pMode);

  ZStatus loadDictionaryByVersion(unsigned long pVersion );
  ZStatus loadDictionaryByRank(long pRank );
  ZStatus loadActiveDictionary();
  ZStatus saveDictionary(unsigned long pVersion);


  void setDictionary(ZMFDictionary& pMasterDic) {ZMFDictionary::_copyFrom( pMasterDic);}
  ZMFDictionary& getDictionary() {return *this;}

  long findDictionaryByVersion(unsigned long pVersion );
  long findDictionaryByName(const utf8VaryingString& pName );


  utf8VaryingString exportToXmlString(bool pComment);
  ZStatus exportToXmlFile(const uriString &pXmlFile,bool pComment);

  ZStatus importFromXmlString(const utf8VaryingString& pXmlContent);
  ZStatus importFromXmlFile(const uriString &pXmlFile);

  ZDicList getAllDictionaries();
  ZDicHeaderList getAllDicHeaders();

//  ZMFDictionary MasterDic ;
};
} // namespace zbs
#endif // ZDICTIONARYFILE_H
