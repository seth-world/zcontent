#include "zdictionaryfile.h"
using namespace zbs;

#include <zcontent/zindexedfile/zmfdictionary.h>

#include <ztoolset/zfunctions.h>

ZDictionaryFile::ZDictionaryFile() : ZRandomFile(ZFT_DictionaryFile)
{

}

ZStatus ZDictionaryFile::zinitalize(const uriString &pFilename, bool pBackup)
{
  ZDataBuffer wDicExport;
  CreationDate = ZDateFull::currentDateTime();
  ModificationDate = ZDateFull::currentDateTime();

  _exportAppend(wDicExport);

  ZStatus wSt=zcreate(pFilename,wDicExport.Size,pBackup,true);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wSt= zadd(wDicExport);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  return zclose();
}//zinitalize

ZStatus ZDictionaryFile::zcreate(const uriString &pFilename,size_t pInitialSize, bool pBackup,bool pLeaveOpen)
{
  ZStatus wSt;
  if ((wSt=setPath(pFilename))!=ZS_SUCCESS) {
    return  wSt;
  }
  setCreateMinimum( pInitialSize);
  return _create(pInitialSize,ZFT_DictionaryFile,pBackup,pLeaveOpen);

}//zcreate

ZStatus ZDictionaryFile::zcreate(const uriString &pFilename,size_t pInitialSize,
    const long pAllocatedBlocks,
    const long pBlockExtentQuota,
    const long pBlockTargetSize,
    const bool pHighwaterMarking,
    const bool pGrabFreeSpace,bool pBackup,bool pLeaveOpen)
{
  ZStatus wSt;
  if ((wSt=setPath(pFilename))!=ZS_SUCCESS) {
    return  wSt;
  }
  setCreateMaximum( pInitialSize,
                    pAllocatedBlocks,
                    pBlockExtentQuota,
                    pBlockTargetSize,
                    pHighwaterMarking,
                    pGrabFreeSpace);
  return _create(pInitialSize,ZFT_DictionaryFile,pBackup,pLeaveOpen);

}//zcreate
ZStatus ZDictionaryFile::zopen(const uriString &pFilename,const zmode_type pMode)
{
  ZStatus wSt;
  if ((wSt=setPath(pFilename))!=ZS_SUCCESS)
    {    return  wSt;}
  wSt= _ZRFopen(pMode,ZFT_DictionaryFile,true);
  if (wSt== ZS_SUCCESS)
    ZRFPool->addOpenFile(this);
  return wSt;
}//zopen

ZStatus ZDictionaryFile::zopen(const zmode_type pMode)
{
  return _ZRFopen(pMode,ZFT_DictionaryFile,true);
}//zopen


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

ZStatus ZDictionaryFile::getDictionaryHeader(ZMFDicExportHeader& wHeader,long pRank )
{
  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,pRank);
  if (wSt != ZS_SUCCESS)
    return wSt;
  const unsigned char* wPtrIn=wRecord.getData();
  return  wHeader._import(wPtrIn);
}//ZDictionaryFile::loadDictionaryHeader

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
  long wRank;
  ZStatus wSt=searchActiveDictionary(wRank);
  if (wRank < 0) {
    return wSt;
  }
  return loadDictionaryByRank(wRank);
}//ZDictionaryFile::loadActiveDictionary

ZStatus ZDictionaryFile::searchActiveDictionary(long &pRank)
{
  ZMFDicExportHeader wHeader;
  ZDataBuffer wRecord;
  pRank=-1;
  ZStatus wSt=zget(wRecord,0);
  const unsigned char* wPtrIn=wRecord.getData();
  wSt= wHeader._import(wPtrIn);
  if ((wSt==ZS_SUCCESS)&&(wHeader.Active)) {
    pRank = CurrentRank;
    return wSt;
  }
  while (wSt==ZS_SUCCESS) {
    wSt=zgetNext(wRecord);
    if (wSt==ZS_SUCCESS) {
      wPtrIn=wRecord.getData();
      wSt= wHeader._import(wPtrIn);
      if ((wSt==ZS_SUCCESS)&&(wHeader.Active)) {
        pRank=CurrentRank;
        return wSt;
      }
    }
  }
  if (wSt==ZS_EOF) {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_NOTFOUND,
        Severity_Error,
        "No active dictionary found out of %ld records in dictionary file <%s>.",
        getRecordCount(),
        URIContent.toString());
    return ZS_NOTFOUND;
  }
  return wSt;  /* here io error : return status - ZException is filled appropriately */
}//findActiveDictionary

ZStatus ZDictionaryFile::searchDictionary(long &pRank,const utf8VaryingString& pDicName ,unsigned long pVersion)
{
  ZMFDicExportHeader wHeader;
  ZDataBuffer wRecord;
  pRank=-1;
  ZStatus wSt=zget(wRecord,0);
  const unsigned char* wPtrIn=wRecord.getData();
  wSt= wHeader._import(wPtrIn);
  if ((wSt==ZS_SUCCESS)&&(wHeader.DicName==pDicName)&&(wHeader.Version==pVersion)) {
    pRank = CurrentRank;
    return ZS_FOUND ;
  }
  while (wSt==ZS_SUCCESS) {
    wSt=zgetNext(wRecord);
    if (wSt==ZS_SUCCESS) {
      wPtrIn=wRecord.getData();
      wSt= wHeader._import(wPtrIn);
      if ((wSt==ZS_SUCCESS)&&(wHeader.DicName==pDicName)&&(wHeader.Version==pVersion)) {
        pRank=CurrentRank;
        return ZS_FOUND ;
      }
    }
  }
  if (wSt==ZS_EOF) {
    return ZS_NOTFOUND;
  }
  return wSt;  /* here io error : return status - ZException is filled appropriately by underlying routines */
}//searchDictionary

/**
 * @brief ZDictionaryFile::searchAndWrite
 *          search for (DicName , Version) in file :
            not exists -> append
            exists -> replace

      if dictionary to write is active, then reset all other active dictionaries to not active.
 * @param pDicName
 * @param pVersion
 * @return
 */
ZStatus
ZDictionaryFile::searchAndWrite() {
  ZDataBuffer wDicExport;

  ZDataBuffer         wDicRecord;
  ZMFDicExportHeader  wHeader;
  bool  wDicReplace=false;
  long wRank=0;
  ZStatus wSt=zget(wDicRecord,wRank);

  while (wSt==ZS_SUCCESS){
    const unsigned char* wPtrIn=wDicRecord.getData();
    wSt= wHeader._import(wPtrIn);
    if (wSt != ZS_SUCCESS){
      break;
    }
    if ((wSt==ZS_SUCCESS)&&(wHeader.DicName==DicName)&&(wHeader.Version==Version)) {
      wDicReplace=true;
      wSt=zreplace(wDicRecord,wRank);
      if (wSt!=ZS_SUCCESS)
        break;
    }
    else {
      if (Active && wHeader.Active) { /* if current is active and another is active, set this another to inactive */
        ZMFDictionary       wAnotherDic;
        const unsigned char* wPtrIn=wDicRecord.getData();
        wSt= wAnotherDic._import(wPtrIn);
        if (wSt != ZS_SUCCESS) {
          break;
        }
        wAnotherDic.Active=false;
        wAnotherDic.ModificationDate = ZDateFull::currentDateTime();
        wDicRecord.clear();
        wAnotherDic._exportAppend(wDicRecord);
        wSt=zreplace(wDicRecord,wRank);
        if (wSt!=ZS_SUCCESS)
          break;
      } // if (Active && wHeader.Active)
      /* NB: if dictionary to save is not active, we do not care about changing active status of other dics */
    }//else
    wRank++;
    wSt=zget(wDicRecord,wRank);
  }// while

  if (wSt!=ZS_OUTBOUNDHIGH)
    return wSt;
  if (wDicReplace)
    return ZS_REPLACED;

  ModificationDate = ZDateFull::currentDateTime();
  _exportAppend(wDicExport);
  wSt=zadd(wDicExport);
  if (wSt==ZS_SUCCESS) {
    return ZS_CREATED;
  }
  return wSt;
}//searchAndWrite

/** @brief save save embedded content to a valid, existing dictionary file.

Process
  -------------------Not handled by this routine------------------------
  dictionary file does not exist         create and don't care active (zinitialize)
  ----------------------------------------------------------------------

  dictionary file exits

  Dic content to write is not active :
      [searchAndWrite] search for (DicName , Version) in file :
        not exists -> append
        exists -> replace

  Dic content to write is active :
      search for active in file :
      active not exists :
        [searchAndWrite] search for (DicName , Version) in file :
          not exists -> append
          exists -> replace
      active exists :
        (DicName , Version) are the same : replace.
        (DicName , Version) are not the same :
          found in Dictionary set to not Active, and replace
          [searchAndWrite] search for (DicName , Version) in file :
            not exists -> append
            exists -> replace

*/
ZStatus
ZDictionaryFile::save(){
  ZStatus wSt;
  zmode_type wSvMode = getOpenMode();

  if (!(wSvMode & ZRF_Write_Only)) {
    if (wSvMode != ZRF_NotOpen)
      zclose();
    wSt=zopen( ZRF_Write_Only);
    if (wSt!=ZS_SUCCESS)
      return wSt;
  }

  if (CreationDate.isInvalid())
    CreationDate = ZDateFull::currentDateTime();

//  ModificationDate = ZDateFull::currentDateTime();

  wSt= searchAndWrite();
  if (wSvMode == ZRF_NotOpen)
    zclose();
  return wSt;
}//save

ZStatus
ZDictionaryFile::getAllDicHeaders(ZDicHeaderList& wDicHeaderList)
{
  ZMFDicExportHeader* wDicHeader=nullptr;
  const unsigned char* wPtrIn=nullptr;
  ZDataBuffer wRecord;
  zrank_type wRank=0;
  ZStatus wSt=zget(wRecord,wRank++);
  if (wSt==ZS_OUTBOUNDHIGH)
    return ZS_EMPTY;

  while (wSt==ZS_SUCCESS) {
    wPtrIn=wRecord.getData();
    wDicHeader = new ZMFDicExportHeader;
    wSt=wDicHeader->_import(wPtrIn);
    if (wSt==ZS_SUCCESS) {
      wDicHeaderList.push(wDicHeader);
      wSt=zget(wRecord,wRank++);
    }
  }

  return wSt;
}

ZStatus
ZDictionaryFile::getAllDictionaries(ZDicList& pDiclist)
{
  ZMFDictionary* wDic=nullptr;
  const unsigned char* wPtrIn=nullptr;
  ZDataBuffer wRecord;
  zrank_type wRank=0;
  ZStatus wSt=zget(wRecord,0L);
  if (wSt==ZS_OUTBOUNDHIGH)
    return ZS_EMPTY;

  while (wSt==ZS_SUCCESS) {
    wPtrIn=wRecord.getData();
    wDic = new ZMFDictionary;
    wSt=wDic->_import(wPtrIn);
    if (wSt==ZS_SUCCESS) {
      pDiclist.push(wDic);
      wSt=zgetNext(wRecord);
    }
  }
  return wSt;
}
long ZDictionaryFile::findDictionaryByVersion(unsigned long pVersion )
{
  ZMFDicExportHeader wHeader;
  ZDataBuffer wRecord;
  ZStatus wSt=zget(wRecord,0);
  if (wSt!=ZS_SUCCESS)
    return -1;
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
  if (wSt!=ZS_SUCCESS)
    return -1;
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
#ifdef __COMMENT__
ZStatus ZDictionaryFile::saveDictionary()
{
  ZDataBuffer wRecord;
  ZMFDictionary::_exportAppend(wRecord);

  zmode_type wSvMode = getOpenMode();
  ZStatus wSt;
  if (!(wSvMode & ZRF_Write_Only)) {
    if (wSvMode != ZRF_NotOpen)
          zclose();
    wSt=zopen( ZRF_Modify);
    if (wSt!=ZS_SUCCESS)
      return wSt;
  }
  if (!Active) {
    /* if current dictionary version is not active, write it and return */
  }
  ZDicHeaderList wDicHeaderList;
  wSt=getAllDicHeaders(wDicHeaderList);
  if (wSt==ZS_SUCCESS) {
    long wRank=0;
    bool wFound=false;
    /* search for active version */
    for ( ; wRank < wDicHeaderList.count(); wRank++) {
      if (wDicHeaderList[wRank]->Active) {
        wFound=true;
      }
    }
    if (wFound) {
      if ((wDicHeaderList[wRank]->DicName==DicName)&&(wDicHeaderList[wRank]->Version==Version)) {
        /* replace it */
        return zreplace(wRecord,wRank);
      }
      else {
        /* active version exists but this is not this version/name :
         * return ZS_
        */
        ZException.setMessage(_GET_FUNCTION_NAME_,ZS_BADFILEVERSION,Severity_Error,
            "Trying to write active ");
        return ZS_BADFILEVERSION;

      }
    }
  }


  long wRank;
  wSt=findActiveDictionary(wRank);
  if (wRank > -1) {

    }
  wRank=findDictionaryByVersion(Version);
  if (wRank < 0) {
    wSt= zadd(wRecord);
  }
  else {
  ZMFDictionary::_exportAppend(wRecord);

  wSt= zreplace(wRecord,wRank);
  }

  if (wSvMode == ZRF_NotOpen) {
    zclose();
  }
  return wSt;
}//saveDictionary
#endif // __COMMENT__
utf8VaryingString  ZDictionaryFile::exportToXmlString( bool pComment)
{
  return ZMFDictionary::XmlSaveToString(pComment);

}
ZStatus ZDictionaryFile::exportToXmlFile(const uriString &pXmlFile,bool pComment)
{
  utf8VaryingString wContent = ((ZMFDictionary*)this)->XmlSaveToString(pComment);
  return pXmlFile.writeContent(wContent);
}

ZStatus
ZDictionaryFile::generateAndSetFileName(const uriString& pURIContent){
  URIContent = generateFileName(pURIContent);
  if (!URIContent.exists())
    return ZS_FILENOTEXIST;
  return ZS_SUCCESS;
}

utf8VaryingString
ZDictionaryFile::generateFileName(const uriString& pURIContent){
  uriString wDicPath;
  wDicPath = pURIContent.getDirectoryPath();
  wDicPath.addConditionalDirectoryDelimiter();
  wDicPath += pURIContent.getBasename();
  wDicPath += ".";
  wDicPath += __DICTIONARY_EXTENSION__;

  return (utf8VaryingString)wDicPath;
}

