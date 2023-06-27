#include "zmasterfile.h"

#include <zcontent/zcontentcommon/urfparser.h>

using namespace  zbs;

ZMasterFile::ZMasterFile() : ZRawMasterFile(ZFT_ZDicMasterFile)
{

}

ZMasterFile::~ZMasterFile() {

}

ZStatus
ZMasterFile::zopen  (const uriString &pURI, zmode_type pMode)
{
  ZStatus wSt = ZRawMasterFile::zopen(pURI,pMode);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  if (getFileType() == ZFT_ZRawMasterFile)
    return ZS_FILETYPEWARN ;
  return loadDictionary();
}// zopen

utf8VaryingString
ZMasterFile::getDictionaryName() {
  if (Dictionary==nullptr) {
    return "<No dictionary>";
  }
  return Dictionary->DicName;
}


ZStatus
ZMasterFile::loadDictionary()
{
  if (!isOpen()) {
    ZException.setMessage("ZMasterFile::loadDictionary",ZS_FILENOTOPEN,Severity_Severe,
        "File <%s> must be open before loading dictionary.",getURIContent().toString());
    return ZS_FILENOTOPEN;
  }
  if (Dictionary!=nullptr) {
    delete Dictionary;
  }
  Dictionary=new ZDictionaryFile;
  utf8VaryingString wDicFileName = Dictionary->generateDicFileName(getURIContent());
  return Dictionary->loadDictionary(wDicFileName);
}// zFullOpen

ZStatus
ZMasterFile::setDictionary(const ZMFDictionary& pDictionary)
{
  uriString wURIdic;
  if (Dictionary==nullptr) {
    Dictionary=new ZDictionaryFile; 
  }
  Dictionary->setDictionary(pDictionary);
  wURIdic = ZDictionaryFile::generateDicFileName(getURIContent());
  return Dictionary->saveToDicFile(wURIdic);
}

ZStatus
ZMasterFile::setExternDictionary(const uriString& pDicPath)
{
  if (!pDicPath.exists()) {
    ZException.setMessage("ZMasterFile::setDictionary",ZS_FILENOTEXIST,Severity_Error,"Dictionary file %s does not exist.",pDicPath.toString());
    return ZS_FILENOTEXIST;
  }

  if (Dictionary==nullptr) {
    Dictionary=new ZDictionaryFile;
  }
  DictionaryPath = pDicPath;
  Dictionary->URIDictionary = pDicPath;
  return Dictionary->load();
}

ZStatus
ZMasterFile::rebuildIndex(long pIndexRank,long* pRank) {
  ZDataBuffer wRecord, wKeyRecord;
  zaddress_type wZMFAddress;
  ZIndexItem* wIndexItem=nullptr;
  long wRank=0L;
  if (getMode()!=ZRF_All) {
    ZException.setMessage("ZMasterFile::rebuildIndex",ZS_FILENOTOPEN,Severity_Error,
        "File is open in mode %s. It must be open with access mode ZRF_All.",decode_ZRFMode(getMode()));
    return ZS_MODEINVALID;
  }

  if (pIndexRank >= IndexTable.count()) {
    ZException.setMessage("ZMasterFile::rebuildIndex",ZS_OUTBOUND,Severity_Error,
        "Invalid index rank <%ld> while expecting one of [0,%ld[.",pIndexRank,IndexTable.count());
    return ZS_INVTYPE;
  }
  ZStatus wSt;
/*  if (!IndexTable[pIndexRank]->isOpen()) {
    IndexTable[pIndexRank]->zclose();
  }
  wSt=zopenIndexFile(pIndexRank,ZRF_All);
*/
  wSt=IndexTable[pIndexRank]->zclearFile(-1);
  if (wSt!=ZS_SUCCESS){
    return wSt;
  }

  /* browse all master file records, extract key and feed index file */
  wSt=zgetWAddress(wRecord,0L,wZMFAddress);
  if (wSt!=ZS_SUCCESS){
    return wSt;
  }

  while (wSt==ZS_SUCCESS) {
    wSt=extractKeyValues(wRecord,wKeyRecord,pIndexRank);
    if (wSt!=ZS_SUCCESS)
      goto rebuildIndexError;
    wSt=IndexTable[pIndexRank]->_addRawKeyValue_Prepare(wIndexItem,wKeyRecord,wZMFAddress);
    if (wSt!=ZS_SUCCESS)
      goto rebuildIndexError;
    wSt=IndexTable[pIndexRank]->_rawKeyValue_Commit(wIndexItem);
    if (wSt!=ZS_SUCCESS)
      goto rebuildIndexError;
    wSt=zgetNextWAddress(wRecord,wRank,wZMFAddress);
    if (pRank!=nullptr)
      *pRank=wRank;
  }

  IndexTable[pIndexRank]->zclose();
  if (wSt==ZS_EOF)
    wSt=ZS_SUCCESS;
  return wSt;
rebuildIndexError:
  if (ZException.count()==0){
    ZException.setMessage("ZMasterFile::rebuildIndex",wSt,Severity_Error,"Error while rebuilding index");
  }
  else {
    ZException.addToLast(" calling module <ZMasterFile::rebuildIndex>");
  }
  return wSt;
}


/*

  first : presence bit set
  uint64_t user data size
  urf list


*/
ZStatus
ZMasterFile::extractKeyValues(const ZDataBuffer& pRecord,ZDataBuffer& pKeyContent,long pIndexRank) {
  ZTypeBase wZType;
  ZBitset wBitset;
  bool  wBitSetFull=false;
  uint64_t wURFDataSize=0;

  pKeyContent.clear();

  ZMFDictionary& wZMFDic=Dictionary->getDictionary();

  ZKeyDictionary* wKeyDic=wZMFDic.KeyDic[pIndexRank];

  const unsigned char* wPtrIn=pRecord.Data;
/*
  ZBitset wFieldPresence ;
  wFieldPresence._importURF(wPtrIn);
*/
  _importAtomic<ZTypeBase>(wZType,wPtrIn);

  if ((wZType != ZType_bitset) && (wZType != ZType_bitsetFull)) {
    ZException.setMessage("ZMasterFile::extractKey",ZS_INVTYPE,Severity_Error, "Invalid format. While expecting <ZType_bitset>, found <%6X> <%s>.",wZType,decode_ZType(wZType)
        );
    return ZS_INVTYPE;
  }
  if (wZType==ZType_bitset) {
    wPtrIn -= sizeof(ZTypeBase);
    ssize_t wSize=wBitset._importURF(wPtrIn);
  } // if (wZType==ZType_bitset)

  else if (wZType==ZType_bitsetFull) {
    wBitSetFull=true;
  } // if (wZType==ZType_bitsetFull)

  /* second get user URF data size */
//  _importAtomic<uint64_t>(wURFDataSize,wPtrIn);  // Deprecated

  const unsigned char* wFldBegPtr=wPtrIn;
//  const unsigned char* wFldPtr=wPtrIn;
  const unsigned char* wPtrEnd=wPtrIn + pRecord.Size;
  long wR=0;

  long wFldRank=-1;
  for (long wi=0;wi < wKeyDic->count();wi++) {
//    if (wKeyDic->Tab(wi).MDicRank < wFldRank)  { /* if next field has a greater rank : do not reset pointer */
//      wR=0;
//      wPtrIn=wFldBegPtr;
//    }

    wFldRank=wKeyDic->Tab(wi).MDicRank;

    /* find sequentially field of rank MDicRank */
    wR=0;
    while ((wR < wFldRank)&&(wPtrIn < wPtrEnd)) {
      if ((!wBitSetFull) && (!wBitset.test(wR)))  {
        wR++;
        continue;
      }

      URFField wF=getURFField(wPtrIn);
      wR++;
    }//while

    if (wR != wFldRank) {
      ZException.setMessage("ZMasterFile::extractKey",ZS_OUTBOUNDHIGH,Severity_Error,"Requested field position <%ld> is out of range",wFldRank);
      return ZS_OUTBOUNDHIGH;
    }
    if (!wBitset.test(wR)) {
      ZException.setMessage("ZMasterFile::extractKey",ZS_OMITTED,Severity_Error,"Requested field - position <%ld> name <%s> is omitted. Cannot build an index key with omitted field.",wFldRank);
      return ZS_OUTBOUNDHIGH;
    }
    URFField wF=getURFField(wPtrIn);
    if (wF.Ptr==nullptr) {
      return ZException.last().Status;
    }
    pKeyContent.appendData(wF.Ptr,wF.Size);
  }// for
  return ZS_SUCCESS;
} // extractKey

