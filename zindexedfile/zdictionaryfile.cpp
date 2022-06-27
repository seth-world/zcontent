#include "zdictionaryfile.h"
using namespace zbs;

#include <zcontent/zindexedfile/zmfdictionary.h>

#include <ztoolset/zfunctions.h>

ZDictionaryFile::ZDictionaryFile() : ZRandomFile(ZFT_DictionaryFile)
{

}
ZStatus ZDictionaryFile::zopen(const uriString &pFilename,const zmode_type pMode)
{
  ZStatus wSt= _ZRFopen(pMode,ZFT_DictionaryFile,true);
  ZRFPool->addOpenFile(this);
}




ZStatus ZDictionaryFile::loadDictionaryByVersion(unsigned long pVersion )
{
  ZMFDicExportHeader wHeader;
  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,0);
  const unsigned char* wPtrIn=wRecord.getData();
  wSt= wHeader._import(wPtrIn);
  while ((wSt==ZS_SUCCESS)&&(pVersion!=0UL)&&(wHeader.Version!=pVersion)) {
    wSt=zgetNext(wRecord);
    if (wSt==ZS_SUCCESS) {
      wPtrIn=wRecord.getData();
      wSt= wHeader._import(wPtrIn);
    }
  }

  if (wSt==ZS_SUCCESS) {
    if (wHeader.Version==pVersion) {
      wPtrIn=wRecord.getData();
      return ((ZMFDictionary*)this)->_import(wPtrIn);
    }
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_NOTFOUND,
        Severity_Error,
        "No dictionary found out of %ld records for requested version <%s> in dictionary file <%s>.",
        getRecordCount(),
        getVersionStr(pVersion).toCChar(),
        URIContent.toString()
        );
    return ZS_NOTFOUND;
  }
  return wSt;
}//ZDictionaryFile::loadDictionaryByVersion

ZStatus ZDictionaryFile::loadDictionaryByRank(long pRank )
{
  ZMFDicExportHeader wHeader;
  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,pRank);
  const unsigned char* wPtrIn=wRecord.getData();

  if (wSt==ZS_SUCCESS) {
      return ((ZMFDictionary*)this)->_import(wPtrIn);
    }
  return wSt;
}//ZDictionaryFile::loadDictionaryByRank

ZStatus ZDictionaryFile::loadActiveDictionary()
{
  ZMFDicExportHeader wHeader;
  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,0);
  const unsigned char* wPtrIn=wRecord.getData();
  wSt= wHeader._import(wPtrIn);
  while (!wHeader.Active&&(wSt==ZS_SUCCESS)) {
    wSt=zgetNext(wRecord);
    if (wSt==ZS_SUCCESS) {
      wPtrIn=wRecord.getData();
      wSt= wHeader._import(wPtrIn);
    }
  }

  if (wSt==ZS_SUCCESS) {
    if (wHeader.Active) {
      wPtrIn=wRecord.getData();
      return ZMFDictionary::_import(wPtrIn);
    }
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_NOTFOUND,
        Severity_Error,
        "No active dictionary found out of %ld records in dictionary file <%s>.",
        getRecordCount(),
        URIContent.toString());
    return ZS_NOTFOUND;
  }
  return wSt;  /* here io error : return status - ZException is filled appropriately */
}//ZDictionaryFile::loadActiveDictionary

ZDicList
ZDictionaryFile::getAllDictionaries()
{
  ZMFDictionary* wDic=nullptr;
  const unsigned char* wPtrIn=nullptr;
  ZDicList wListDic;
  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,0);


  while (wSt==ZS_SUCCESS) {
    wPtrIn=wRecord.getData();
    wDic = new ZMFDictionary;
    wSt=wDic->_import(wPtrIn);
    if (wSt==ZS_SUCCESS) {
      wListDic.push(wDic);
      wSt=zgetNext(wRecord);
    }
  }

  return wListDic;
}

ZDicHeaderList
ZDictionaryFile::getAllDicHeaders()
{
  ZMFDicExportHeader* wDicHeader=nullptr;
  const unsigned char* wPtrIn=nullptr;
  ZDicHeaderList wDicHeaderList;
  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,0);


  while (wSt==ZS_SUCCESS) {
    wPtrIn=wRecord.getData();
    wDicHeader = new ZMFDicExportHeader;
    wSt=wDicHeader->_import(wPtrIn);
    if (wSt==ZS_SUCCESS) {
      wDicHeaderList.push(wDicHeader);
      wSt=zgetNext(wRecord);
    }
  }

  return wDicHeaderList;
}

long ZDictionaryFile::findDictionaryByVersion(unsigned long pVersion )
{
  ZMFDicExportHeader wHeader;
  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,0);
  const unsigned char* wPtrIn=wRecord.getData();
  wSt= wHeader._import(wPtrIn);
  while ((wSt==ZS_SUCCESS) && (pVersion!=0UL) && (wHeader.Version!=pVersion)) {
    wSt=zgetNext(wRecord);
    if (wSt==ZS_SUCCESS) {
      wPtrIn=wRecord.getData();
      wSt= wHeader._import(wPtrIn);
      if (wHeader.Version==pVersion)
        return CurrentRank;
      continue;
    }
  }
  return -1;
}//ZDictionaryFile::findDictionaryByVersion

long ZDictionaryFile::findDictionaryByName(const utf8VaryingString &pName )
{

  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,0);
  const unsigned char* wPtrIn=wRecord.getData();
  wSt= ZMFDictionary::_import(wPtrIn);
  while ((ZMFDictionary::DicName!=pName)&&(wSt==ZS_SUCCESS)) {
    wSt=zgetNext(wRecord);
    if (wSt==ZS_SUCCESS) {
      wPtrIn=wRecord.getData();
      wSt= ZMFDictionary::_import(wPtrIn);
    }
  }
  if (wSt==ZS_SUCCESS) {
    if (ZMFDictionary::DicName==pName)
      return CurrentRank;

    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_NOTFOUND,
        Severity_Error,
        "No dictionary found out of %ld records for requested name <%s> in dictionary file <%s>.",
        getRecordCount(),
        pName.toCChar(),
        URIContent.toString()
        );
  }
  return -1;
}//ZDictionaryFile::findDictionaryByName

ZStatus ZDictionaryFile::saveDictionary(unsigned long pVersion )
{
  ZDataBuffer wRecord;
  ZStatus wSt;

  ZMFDictionary::Version = pVersion;
  ((ZMFDictionary*)this)->_exportAppend(wRecord);

  int wRank=findDictionaryByVersion(pVersion);
  if (wRank < 0) {
    return zadd(wRecord);
  }

  const unsigned char* wPtrIn=wRecord.getData();
  ((ZMFDictionary*)this)->_exportAppend(wRecord);



}

utf8VaryingString  ZDictionaryFile::exportToXmlString( bool pComment)
{
  return ((ZMFDictionary*)this)->XmlSaveToString(pComment);

}
ZStatus ZDictionaryFile::exportToXmlFile(const uriString &pXmlFile,bool pComment)
{
  utf8VaryingString wContent = ((ZMFDictionary*)this)->XmlSaveToString(pComment);
  return pXmlFile.writeContent(wContent);
}
