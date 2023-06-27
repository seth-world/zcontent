#ifndef ZDICTIONARYFILE_H
#define ZDICTIONARYFILE_H

#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zmfdictionary.h>

#include <stdint.h>

#include <ztoolset/uristring.h>

#define __DICTIONARY_EXTENSION__ "dic"

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
      ZMFDictionary* wI=new  ZMFDictionary(*pIn.at(wi));
      push(wI);
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
    while (count())
      pop();

    for (long wi=0;wi < count();wi++) {
      push(new ZMFDicExportHeader(*pIn.at(wi)));
    }
    return *this;
  }

  ZDicHeaderList& operator = (const ZDicHeaderList& pIn) {return _copyFrom(pIn);}
};

class ZDictionaryFile :  public ZMFDictionary
{
public:
  ZDictionaryFile();
  ZDictionaryFile(const ZDictionaryFile& pIn) { _copyFrom(pIn);}

  ZDictionaryFile& _copyFrom(const ZDictionaryFile& pIn) ;

  ZMFDictionary& getDictionary() { return *this; }

  ZStatus create(const uriString &pDicFilename,  bool pBackup=true);

  ZStatus load();
  ZStatus save(bool pBackup=true);

  /** @brief savetoDicFile updates URIDictionary and save current dictionary content to this file */
  ZStatus saveToDicFile (const uriString& pURIDicFile) {
    URIDictionary = pURIDicFile;
    return save();
  }

  ZStatus saveAsEmbedded(const uriString& pZMFURIContent) {
    URIDictionary = generateDicFileName(pZMFURIContent);
    return save();
  }

  void setDicFilename(const uriString& pDicFilename) { URIDictionary=pDicFilename; }
  void setDictionary(const ZMFDictionary& pDic) ;
  ZStatus loadDictionary(const uriString& pDicFilename);

  utf8VaryingString exportToXmlString(bool pComment);
  ZStatus exportToXmlFile(const uriString &pXmlFile,bool pComment);

  ZStatus importFromXmlString(const utf8VaryingString& pXmlContent);
  ZStatus importFromXmlFile(const uriString &pXmlFile);

  static utf8VaryingString generateDicFileName(const uriString& pURIContent);


  uriString URIDictionary;

//  ZMFDictionary Dictionary ;
};
} // namespace zbs
#endif // ZDICTIONARYFILE_H
