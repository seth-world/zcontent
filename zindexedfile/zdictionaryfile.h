#ifndef ZDICTIONARYFILE_H
#define ZDICTIONARYFILE_H

#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zmfdictionary.h>

#include <stdint.h>

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

  using ZMFDictionary::push;

  ZStatus zcreate(const uriString &pFilename, size_t pInitialSize, bool pBackup=true, bool pLeaveOpen=false);
  ZStatus zcreate(const uriString &pFilename, size_t pInitialSize,
                  const long pAllocatedBlocks,
                  const long pBlockExtentQuota,
                  const long pBlockTargetSize,
                  const bool pHighwaterMarking,
                  const bool pGrabFreeSpace, bool pBackup, bool pLeaveOpen);

  ZStatus zinitalize(const uriString &pFilename, bool pBackup=true);

  ZStatus zopen(const uriString &pFilename,const zmode_type pMode);
  ZStatus zopen(const zmode_type pMode);

  ZStatus loadDictionaryByVersion(unsigned long pVersion );
  ZStatus loadDictionaryByRank(long pRank );
  ZStatus loadActiveDictionary();


  ZStatus getDictionaryHeader(ZMFDicExportHeader& wHeader,long pRank );

  ZStatus searchActiveDictionary(long &pRank);

  ZStatus searchDictionary(long &pRank,const utf8VaryingString& pDicName ,unsigned long pVersion);

  ZStatus searchAndWrite();

  ZStatus save();
//  ZStatus saveDictionary();

  void setDictionary(const ZMFDictionary& pDictionary) {ZMFDictionary::_copyFrom( pDictionary);}
  ZMFDictionary& getDictionary() {return *this;}

  long findDictionaryByVersion(unsigned long pVersion );
  long findDictionaryByName(const utf8VaryingString& pName );

  utf8VaryingString exportToXmlString(bool pComment);
  ZStatus exportToXmlFile(const uriString &pXmlFile,bool pComment);

  ZStatus importFromXmlString(const utf8VaryingString& pXmlContent);
  ZStatus importFromXmlFile(const uriString &pXmlFile);

  ZStatus getAllDicHeaders(ZDicHeaderList& wDicHeaderList);
  ZStatus getAllDictionaries(ZDicList& pDiclist);

  ZStatus generateAndSetFileName(const uriString& pURIContent);
  static utf8VaryingString generateFileName(const uriString& pURIContent);
//  ZMFDictionary Dictionary ;
};
} // namespace zbs
#endif // ZDICTIONARYFILE_H
