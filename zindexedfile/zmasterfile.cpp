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
}// zFullOpen

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
  _importAtomic<uint64_t>(wURFDataSize,wPtrIn);

  const unsigned char* wFldBegPtr=wPtrIn;
  const unsigned char* wFldPtr=wPtrIn;
  const unsigned char* wPtrEnd=wPtrIn + pRecord.Size;
  long wR=0;

  long wFldRank=-1;
  for (long wi=0;wi < wKeyDic->count();wi++) {
    if (wKeyDic->Tab[wi].MDicRank < wFldRank)  { /* if next field has a greater rank : do not reset pointer */
      wR=0;
      wPtrIn=wFldBegPtr;
    }
    wFldRank=wKeyDic->Tab[wi].MDicRank;

    /* find sequentially field of rank MDicRank */

    while ((wR < wFldRank)&&(wPtrIn < wPtrEnd)) {
      if ((!wBitSetFull) && (!wBitset.test(wR)))  {
        wR++;
      }
      URFField wF=getURFField(wFldPtr);
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
    URFField wF=getURFField(wFldPtr);
    if (wF.Ptr==nullptr) {
      return ZException.last().Status;
    }
    pKeyContent.appendData(wF.Ptr,wF.Size);
  }// for
  return ZS_SUCCESS;
} // extractKey

URFField
getURFField(const unsigned char* &pPtrIn) {
  URFField wF;

  wF.Ptr=pPtrIn;
  wF.Size = sizeof(ZTypeBase);
  ZTypeBase wType;

  _importAtomic<ZTypeBase>(wType,pPtrIn);

  switch (wType){
  case ZType_UChar:
  case ZType_U8: {
    wF.Size += sizeof(uint8_t);
    pPtrIn += sizeof(uint8_t);
    break;
  }
  case ZType_Char:
  case ZType_S8: {
    wF.Size += sizeof(int8_t)+1;
    pPtrIn += sizeof(int8_t)+1;
    break;
  }
  case ZType_U16:{
    wF.Size += sizeof(uint16_t);
    pPtrIn += sizeof(uint16_t);
    break;
  }
  case ZType_S16: {
    wF.Size += sizeof(int16_t)+1;
    pPtrIn += sizeof(int16_t)+1;
    break;
  }

  case ZType_U32:{
    wF.Size += sizeof(uint32_t);
    pPtrIn += sizeof(uint32_t);
    break;
  }
  case ZType_S32: {
    wF.Size += sizeof(int32_t)+1;
    pPtrIn += sizeof(int32_t)+1;
    break;
  }
  case ZType_U64: {
    wF.Size += sizeof(uint64_t);
    pPtrIn += sizeof(uint64_t);
    break;
  }
  case ZType_S64: {
    wF.Size += sizeof(int64_t)+1;
    pPtrIn += sizeof(int64_t)+1;
    break;
  }
  case ZType_Float: {
    wF.Size += sizeof(float)+1;
    pPtrIn += sizeof(float)+1;
    break;
  }

  case ZType_Double: {
    wF.Size += sizeof(double)+1;
    pPtrIn += sizeof(double)+1;
    break;
  }

  case ZType_LDouble: {
    wF.Size += sizeof(long double)+1;
    pPtrIn += sizeof(long double)+1;
    break;
  }

    /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */

  case ZType_ZDate: {
    wF.Size += sizeof(uint32_t);
    pPtrIn += sizeof(uint32_t);
    break;
  }
  case ZType_ZDateFull: {
    wF.Size += sizeof(uint64_t);
    pPtrIn += sizeof(uint64_t);
    break;
  }

  case ZType_URIString:{
    URF_UnitCount_type wUnitCount;
    wF.Size += _importAtomic<URF_UnitCount_type>(wUnitCount,pPtrIn);
    wF.Size += wUnitCount * sizeof(utf8_t);
    break;
  }
  case ZType_Utf8VaryingString: {
    URF_UnitCount_type wUnitCount;
    wF.Size += _importAtomic<URF_UnitCount_type>(wUnitCount,pPtrIn);
    wF.Size += wUnitCount * sizeof(utf8_t);
    break;
  }

  case ZType_Utf16VaryingString:{
    URF_UnitCount_type wUnitCount;
    wF.Size += _importAtomic<URF_UnitCount_type>(wUnitCount,pPtrIn);
    wF.Size += wUnitCount * sizeof(utf16_t);
    break;
  }
  case ZType_Utf32VaryingString:{
    URF_UnitCount_type wUnitCount;
    wF.Size += _importAtomic<URF_UnitCount_type>(wUnitCount,pPtrIn);
    wF.Size += wUnitCount * sizeof(utf32_t);
    break;
  }

  case ZType_Utf8FixedString:{
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    wF.Size +=_importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    wF.Size +=_importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    wF.Size += size_t (wUnitsCount) * sizeof(utf8_t);
    break;
  }

    /* for fixed string URF header is different */

  case ZType_Utf16FixedString:{
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    wF.Size +=_importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    wF.Size +=_importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    wF.Size += size_t (wUnitsCount) * sizeof(utf16_t);
    break;
  }

  case ZType_Utf32FixedString:{
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    wF.Size +=_importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    wF.Size +=_importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    wF.Size += size_t (wUnitsCount) * sizeof(utf32_t);
    break;
  }

  case ZType_CheckSum: {
    wF.Size += cst_checksum ;
    break;
  }

  case ZType_MD5: {
    wF.Size += cst_md5 ;
    break;
  }

  case ZType_Blob: {
    uint64_t wDataSize;
    wF.Size += _importAtomic(wDataSize,pPtrIn);
    wF.Size += wDataSize;
    break;
  }

  case ZType_bitset: {
    uint16_t    wByteSize,wEffectiveBitSize;
    wF.Size += _importAtomic<uint16_t>(wByteSize,pPtrIn);
    wF.Size += _importAtomic<uint16_t>(wEffectiveBitSize,pPtrIn);
    wF.Size += size_t(wByteSize);
    break;
  }

  case ZType_bitsetFull: {
    break;
  }

  case ZType_Resource: {
    wF.Size += sizeof(ZEntity_type)+sizeof(Resourceid_type);
    break;
  }

  default: {
    ZException.setMessage("getURFField",ZS_INVTYPE,Severity_Error,"Invalid ZType found %X ",wType);
    wF.Size=0;
    wF.Ptr=nullptr;
    wF.Present = false;
    return wF;
  }
  } // switch
  wF.Present = true;
  return wF;
} //getURFField
