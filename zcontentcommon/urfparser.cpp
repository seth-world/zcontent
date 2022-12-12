#include "urfparser.h"
#include <zcontentcommon/zresource.h>

ZStatus URFParser::set(const ZDataBuffer* pRecord) {
  Record = pRecord;
  URFFieldList.clear();
  URFDataSize = 0;
  ZTypeBase wZType;

  AllFieldsPresent=false;
  URFDataSize=0;

  wPtr=Record->Data;
  wPtrEnd = Record->Data + Record->Size;

  _importAtomic<ZTypeBase>(wZType,wPtr);
  while (true) {
    if (wZType==ZType_bitset) {
      wPtr -= sizeof(ZTypeBase);
      ssize_t wS = Presence._importURF(wPtr);
      if (wS < 0) {
        ZException.setMessage (_GET_FUNCTION_NAME_,
            ZS_CORRUPTED,
            Severity_Severe,
            "Record appears to be malformed.");
        return  ZS_CORRUPTED;
      }
      break;
    } // if (wZType==ZType_bitset)

    /* bitset full is ok : nothing to do */
    if (wZType==ZType_bitsetFull) {
      AllFieldsPresent=true;
      break;
    } // if (wZType!=ZType_bitsetFull)
    /* if neither bitset nor bitsetfull is found then record is not properly formed */
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_CORRUPTED,
        Severity_Severe,
        "Record appears to be malformed : neither ZBitset nor ZBitsetFull has been found.");
    return  ZS_CORRUPTED;
  }// while true

  /* second get user URF data size */
  _importAtomic<uint64_t>(URFDataSize,wPtr);
  return ZS_SUCCESS;  /* done */
}// set

ZStatus
URFParser::appendURFFieldByRank (long pRank,ZDataBuffer pBuffer){
  ZStatus   wSt = ZS_NOTFOUND;

  long wRank=0;
  URFField wField;

  if (wRank < URFFieldList.count()) {
    if (URFFieldList[wRank].Present) {
      pBuffer.appendData(URFFieldList[wRank].Ptr,URFFieldList[wRank].Size);
      return ZS_SUCCESS;
    }

    return ZS_FIELDMISSING;
  }// if (wRank < URFFieldList.count())

  while ((wPtr < wPtrEnd )&&(wRank <= pRank)) {

    if ( (!AllFieldsPresent) && !(Presence.test(size_t(wRank)) ) ) {
      wField.Ptr = wPtr ;
      wField.Size = 0 ;
      wField.Present = false;

      URFFieldList << wField;
      if (wRank == pRank) {
        return ZS_FIELDMISSING;
      }
      wRank++;
      continue;
    } // field not present

    /* here field is present or all fields are declared to be present (ZBitsetFull) */
    wField.Ptr = wPtr;
    wField.Present = true;
    wField.Size = getURFFieldSize(wPtr);
    URFFieldList << wField;
    if (wRank == pRank){
      pBuffer.appendData(URFFieldList[wRank].Ptr,URFFieldList[wRank].Size);
      return ZS_SUCCESS;
    }
    wRank++;
    if (wField.Size < 0) {  /* invalid type */
      /* if ZType is unknown, then field is stored with size = -1, pointer as it is.
       * pointer is then searched for next known ZType (Found) or until the end of record (not Found).
       */
      wSt=searchNextValidZType(wPtr,wPtrEnd);
      if (wSt==ZS_NOTFOUND)
        break;
      continue;
    }
    wPtr += wField.Size;
    continue;
  } // while

  return ZS_SUCCESS;
}


ZDataBuffer
URFParser::getURFFieldByRank (long pRank){
  ZStatus   wSt = ZS_NOTFOUND;
  ZDataBuffer wFieldValue;

//  const unsigned char* wPtr=pData.Data;
//  const unsigned char* wPtrEnd=pData.Data+pData.Size;
  long wRank=0;
  URFField wField;

  if (wRank < URFFieldList.count()) {
    if (URFFieldList[wRank].Present) {
      wFieldValue.setData(URFFieldList[wRank].Ptr,URFFieldList[wRank].Size);
      return wFieldValue;
    }
    wFieldValue.clear();
    return wFieldValue;
  }// if (wRank < URFFieldList.count())

  while ((wPtr < wPtrEnd )&&(wRank <= pRank)) {

    if ( (!AllFieldsPresent) && !(Presence.test(size_t(wRank)) ) ) {
        wField.Ptr = wPtr ;
        wField.Size = 0 ;
        wField.Present = false;
        URFFieldList << wField;
        if (wRank == pRank)
          break;
        wRank++;
        continue;
    }

    /* here field is present or all fields are declared to be present */
    wField.Ptr = wPtr;
    wField.Present = true;
    size_t wS=getURFFieldSize(wPtr);

 /*   wSt=getURFFieldValue(wPtr,wFieldValue);
    if (wSt == ZS_SUCCESS) {
        wField.Size = wFieldValue.Size;
        URFFieldList << wField;
        if (wRank == pRank)
          break;
        wRank++;
        continue;
    }
*/
      /* if ZType is unknown, then field is stored with size = 0, pointer as it is.
       * pointer is then searched for next known ZType (Found) or until the end of record (not Found).
       */

    wField.Size = 0;
    URFFieldList << wField;
    wSt=searchNextValidZType(wPtr,wPtrEnd);
    if (wRank == pRank)
      break;
    wRank++;
  } // while

  if (wRank == pRank) {
      wSt=getKeyFieldValue(wPtr,wFieldValue);

      return wFieldValue;
  }

  return ZDataBuffer();
} // getURFFieldByRank


ZStatus searchNextValidZType( const unsigned char* &pPtr,const unsigned char* wPtrEnd)
{
  ZStatus wSt=ZS_SUCCESS;

  ZTypeBase wZType = reverseByteOrder_Ptr<ZTypeBase>(pPtr);

  while (!ZTypeExists(wZType) && (pPtr < wPtrEnd)) {
    pPtr++;
    wZType=reverseByteOrder_Ptr<ZTypeBase>(pPtr);
  }

  if (ZTypeExists(wZType)) {
    return ZS_FOUND;
  }
  return ZS_NOTFOUND;
}// searchNextValidZType


bool
ZTypeExists(ZTypeBase pType) {
  if (pType & ZType_Atomic)
    pType &= ~ZType_Atomic;

  switch (pType) {
  case ZType_Char:
  case ZType_UChar:
  case ZType_U8:
  case ZType_S8:
  case ZType_U16:
  case ZType_S16:

  case ZType_U32:
  case ZType_S32:
  case ZType_U64:
  case ZType_S64:
  case ZType_Float:
  case ZType_Double:
  case ZType_LDouble:
    return true;
    /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */

  case ZType_ZDate:
  case ZType_ZDateFull:

  case ZType_Utf8VaryingString:

  case ZType_Utf16VaryingString:
  case ZType_Utf32VaryingString:

  case ZType_Utf8FixedString:

  case ZType_Utf16FixedString:

  case ZType_Utf32FixedString:

  case ZType_CheckSum:
  case ZType_MD5:

  case ZType_Resource:

  case ZType_bitset:
  case ZType_bitsetFull:
    return true;

  default:
    return false;

  }// switch

} // ZTypeExists

ZStatus URFParser::getURFTypeAndSize (const unsigned char *pPtrIn,ZTypeBase& pType,ssize_t & pSize) {
  pSize = getURFFieldSize (pPtrIn);
  _importAtomic<ZTypeBase>(pType,pPtrIn);
  if (pSize < 0)
    return ZS_INVTYPE;
  return ZS_SUCCESS;
}


ssize_t
URFParser::getURFFieldSize (const unsigned char* pPtrIn){
  ZTypeBase wType;

//  pValue.setData(pPtrIn,sizeof(ZTypeBase));
  _importAtomic<ZTypeBase>(wType,pPtrIn);
  switch (wType) {
    /* atomic data :
     *  - ZTypeBase           (Raw)
     *  - Fixed size Value    (Raw)
     */
  case ZType_AtomicUChar:
  case ZType_UChar: {
    return sizeof(ZTypeBase)+sizeof(unsigned char);
  }
  case ZType_AtomicChar:
  case ZType_Char: {
    return sizeof(ZTypeBase)+sizeof(char);
  }
  case ZType_U8:
  case ZType_AtomicU8: {
    return sizeof(ZTypeBase)+sizeof(uint8_t);
  }
  case ZType_AtomicS8:
  case ZType_S8: {
    return sizeof(ZTypeBase)+sizeof(int8_t)+1;
  }
  case ZType_AtomicU16:
  case ZType_U16:{
    return sizeof(ZTypeBase)+sizeof(uint16_t);
  }
  case ZType_AtomicS16:
  case ZType_S16: {
    return sizeof(ZTypeBase)+sizeof(int16_t)+1;
  }
  case ZType_AtomicU32:
  case ZType_U32:{
    return sizeof(ZTypeBase)+sizeof(uint32_t);
  }
  case ZType_AtomicS32:
  case ZType_S32: {
    return sizeof(ZTypeBase)+sizeof(int32_t)+1;
  }
  case ZType_AtomicU64:
  case ZType_U64: {
    return sizeof(ZTypeBase)+sizeof(uint64_t);
  }
  case ZType_AtomicS64:
  case ZType_S64: {
    return sizeof(ZTypeBase)+sizeof(int64_t)+1;
  }
  case ZType_AtomicFloat:
  case ZType_Float: {
    return sizeof(ZTypeBase)+sizeof(float)+1;
  }
  case ZType_AtomicDouble:
  case ZType_Double: {
    return sizeof(ZTypeBase)+sizeof(double)+1;
  }
  case ZType_AtomicLDouble:
  case ZType_LDouble: {
    return sizeof(ZTypeBase)+sizeof(long double)+1;
  }

    /* Non atomic data : fixed length structures */

  case ZType_ZDate: {
    return sizeof(ZTypeBase)+sizeof(uint64_t);
  }

  case ZType_ZDateFull: {
    return sizeof(ZTypeBase)+sizeof(uint64_t);
  }

  case ZType_CheckSum: {
    return sizeof(ZTypeBase)+cst_checksum;
  }

  case ZType_MD5: {
    return sizeof(ZTypeBase)+cst_md5;
  }

    /* Varying length storage types
      ZTypeBase               (Raw)
      URF_Varying_Size_type   (Raw)  Effective byte size (Warning: Standard for string is units count)
      varying length data     (Raw)
  */

  case ZType_URIString:
  case ZType_Utf8VaryingString:{

    URF_UnitCount_type wUnitsCount;

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn); /* pPtrIn is updated */
    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf8_t)));

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ size_t(wByteSize);
  }

  case ZType_Utf16VaryingString:{
    URF_UnitCount_type wUnitsCount;

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn); /* pPtrIn is updated */
    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf16_t)));

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ size_t(wByteSize);
  }
  case ZType_Utf32VaryingString:{
    URF_UnitCount_type wUnitsCount;

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn); /* pPtrIn is updated */
    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf32_t)));

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ size_t(wByteSize);
  }

    /* for fixed string URF header is different */

  case ZType_Utf8FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf8_t)));

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize;
  }
  case ZType_Utf16FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf16_t)));

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize;
  }
  case ZType_Utf32FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf32_t)));

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize;
  }

    /* NB neither bitset nor bitsetfull are relevant for a key field */
  case ZType_bitset: {
    URF_UnitCount_type  wByteSize;
    uint16_t wByteSize1, wEBitSize;
    _importAtomic<uint16_t>(wByteSize1,pPtrIn);
    _importAtomic<uint16_t>(wEBitSize,pPtrIn);

    wByteSize = wByteSize1;
    return sizeof(ZTypeBase)+ sizeof(uint16_t)+ sizeof(uint16_t) +size_t(wByteSize);
  }

  case ZType_bitsetFull: {
    /* no value */
    return sizeof(ZTypeBase);
  }

  case ZType_Resource: {
    return sizeof(ZTypeBase) + sizeof(ZEntity_type) + sizeof(Resourceid_type);
  }

  default: {
    return -1;
  }

  }// switch
} // getURFSize


ZStatus
URFParser::getKeyFieldValue (const unsigned char* &pPtrIn,ZDataBuffer& pValue){
  ZTypeBase wType;
  const unsigned char* wRawTypePtr=pPtrIn;
  unsigned char* wPtrOut=nullptr;

  pValue.setData(pPtrIn,sizeof(ZTypeBase));
  _importAtomic<ZTypeBase>(wType,pPtrIn);
  switch (wType) {
    /* atomic data :
     *  - ZTypeBase           (Raw)
     *  - Fixed size Value    (Raw)
     */
  case ZType_AtomicUChar:
  case ZType_UChar: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(unsigned char));
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(unsigned char));
    pPtrIn += sizeof(unsigned char);
    return ZS_SUCCESS;
  }
  case ZType_AtomicChar:
  case ZType_Char: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(char));
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(char));
    pPtrIn += sizeof(char);
    return ZS_SUCCESS;
  }
  case ZType_U8:
  case ZType_AtomicU8: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(uint8_t));
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(uint8_t));
    pPtrIn += sizeof(uint8_t);
    return ZS_SUCCESS;
  }
  case ZType_AtomicS8:
  case ZType_S8: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(int8_t)+1);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(int8_t)+1);
    pPtrIn += sizeof(int8_t)+1;
    return ZS_SUCCESS;
  }
  case ZType_AtomicU16:
  case ZType_U16:{
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(uint16_t));
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(uint16_t));
    pPtrIn += sizeof(uint16_t);
    return ZS_SUCCESS;
  }
  case ZType_AtomicS16:
  case ZType_S16: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(int16_t)+1);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(int16_t)+1);
    pPtrIn += sizeof(int16_t)+1;
    return ZS_SUCCESS;
  }
  case ZType_AtomicU32:
  case ZType_U32:{
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(uint32_t));
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(uint32_t));
    pPtrIn += sizeof(uint32_t);
    return ZS_SUCCESS;
  }
  case ZType_AtomicS32:
  case ZType_S32: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(int32_t)+1);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(int32_t)+1);
    pPtrIn += sizeof(int32_t)+1;
    return ZS_SUCCESS;
  }
  case ZType_AtomicU64:
  case ZType_U64: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(uint64_t));
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(uint64_t));
    pPtrIn += sizeof(uint64_t);
    return ZS_SUCCESS;
  }
  case ZType_AtomicS64:
  case ZType_S64: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(int64_t)+1);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(int64_t)+1);
    pPtrIn += sizeof(int64_t)+1;
    return ZS_SUCCESS;
  }
  case ZType_AtomicFloat:
  case ZType_Float: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(float)+1);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(float)+1);
    pPtrIn += sizeof(float)+1;
    return ZS_SUCCESS;
  }
  case ZType_AtomicDouble:
  case ZType_Double: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(double)+1);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(double)+1);
    pPtrIn += sizeof(double)+1;
    return ZS_SUCCESS;
  }
  case ZType_AtomicLDouble:
  case ZType_LDouble: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(long double)+1);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(long double)+1);
    pPtrIn += sizeof(long double)+1;
    return ZS_SUCCESS;
  }

    /* Non atomic data : fixed length structures */

  case ZType_ZDate: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(uint64_t));
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(uint32_t));
    pPtrIn += sizeof(uint32_t);
    return ZS_SUCCESS;
  }

  case ZType_ZDateFull: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+sizeof(uint64_t));
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,sizeof(uint64_t));
    pPtrIn += sizeof(uint64_t);
    return ZS_SUCCESS;
  }

  case ZType_CheckSum: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+cst_checksum);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,cst_checksum);
    pPtrIn += cst_checksum;
    return ZS_SUCCESS;
  }

  case ZType_MD5: {
    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+cst_md5);
    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    memmove(wPtrOut,pPtrIn,cst_md5);
    pPtrIn += cst_md5;
    return ZS_SUCCESS;
  }

  /* Varying length storage types
      ZTypeBase               (Raw)
      URF_Varying_Size_type   (Raw)  Effective byte size (Warning: Standard for string is units count)
      varying length data     (Raw)
  */

  case ZType_URIString:
  case ZType_Utf8VaryingString:{

    size_t              wByteSize;

    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn); /* pPtrIn is updated */
    wByteSize = size_t(wUnitsCount) * sizeof(utf8_t);

    wPtrOut=pValue.extend(sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize);

    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);

    _exportAtomicPtr<URF_UnitCount_type>(URF_UnitCount_type(wByteSize),wPtrOut);

    memmove(wPtrOut,pPtrIn,size_t(wByteSize));
    pPtrIn += size_t(wByteSize);
    return ZS_SUCCESS;

  }

  case ZType_Utf16VaryingString:{
    size_t              wByteSize;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn); /* pPtrIn is updated */
    wByteSize = size_t(wUnitsCount) * sizeof(utf16_t);

    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize);

    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);

    _exportAtomicPtr<URF_UnitCount_type>(URF_UnitCount_type(wByteSize),wPtrOut);

    memmove(wPtrOut,pPtrIn,size_t(wByteSize));
    pPtrIn += size_t(wByteSize);
    return ZS_SUCCESS;
  }
  case ZType_Utf32VaryingString:{
    size_t              wByteSize;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn); /* pPtrIn is updated */
    wByteSize = size_t(wUnitsCount) * sizeof(utf32_t);

    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize);

    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);

    _exportAtomicPtr<URF_UnitCount_type>(URF_UnitCount_type(wByteSize),wPtrOut);

    memmove(wPtrOut,pPtrIn,size_t(wByteSize));
    pPtrIn += size_t(wByteSize);
    return ZS_SUCCESS;
  }

    /* for fixed string URF header is different */

  case ZType_Utf8FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wByteSize;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    wByteSize = wUnitsCount * sizeof(utf8_t);

    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize);

    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);

    _exportAtomicPtr<URF_UnitCount_type>(URF_UnitCount_type(wByteSize),wPtrOut);

    memmove(wPtrOut,pPtrIn,size_t(wByteSize));
    pPtrIn += size_t(wByteSize);
    return ZS_SUCCESS;
  }
  case ZType_Utf16FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wByteSize;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    wByteSize = wUnitsCount * sizeof(utf16_t);

    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize);

    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);

    _exportAtomicPtr<URF_UnitCount_type>(URF_UnitCount_type(wByteSize),wPtrOut);

    memmove(wPtrOut,pPtrIn,size_t(wByteSize));
    pPtrIn += size_t(wByteSize);
    return ZS_SUCCESS;
  }
  case ZType_Utf32FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wByteSize;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    wByteSize = wUnitsCount * sizeof(utf32_t);

    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ wByteSize);

    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);

    _exportAtomicPtr<URF_UnitCount_type>(URF_UnitCount_type(wByteSize),wPtrOut);

    memmove(wPtrOut,pPtrIn,size_t(wByteSize));
    pPtrIn += size_t(wByteSize);
    return ZS_SUCCESS;
  }

    /* NB neither bitset nor bitsetfull are relevant for a key field */
  case ZType_bitset: {
    size_t wByteSize;
    uint16_t wByteSize1, wEBitSize;
    _importAtomic<uint16_t>(wByteSize1,pPtrIn);
    _importAtomic<uint16_t>(wEBitSize,pPtrIn);

    wByteSize = wByteSize1;

    wPtrOut=pValue.allocate(sizeof(ZTypeBase)+ sizeof(uint16_t)+ sizeof(uint16_t) + wByteSize);

    memmove(wPtrOut,wRawTypePtr,sizeof(ZTypeBase));
    wPtrOut += sizeof(ZTypeBase);
    _exportAtomicPtr<URF_UnitCount_type>(URF_UnitCount_type(wByteSize),wPtrOut);

    memmove(wPtrOut,pPtrIn, wByteSize);
    pPtrIn += wByteSize;
    return ZS_SUCCESS;
  }

  case ZType_bitsetFull: {
    /* no value */
    pValue.setData(wRawTypePtr,sizeof(ZTypeBase));
    return ZS_SUCCESS;
  }

  case ZType_Resource: {
    pValue.setData(wRawTypePtr,sizeof(ZTypeBase));
    pValue.appendData(pPtrIn,sizeof(ZEntity_type)+sizeof(Resourceid_type));
    pPtrIn += sizeof(ZEntity_type)+sizeof(Resourceid_type);
    return ZS_SUCCESS;
  }

  default: {
    pValue.setData(wRawTypePtr,sizeof(ZTypeBase));
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_INVTYPE,
        Severity_Error,
        "URF field type %X <%s> is not known from URFParser engine-",
        wType,decode_ZType(wType));
    return ZS_INVTYPE;
  }

  }// switch
  return ZS_SUCCESS;
}// getURFFieldValue

/** @brief URFCompare  Compare two buffers composed each of one or many URF fields, each field potentially of variable length.
   */
int URFComparePtr(const unsigned char* pKey1, size_t pSize1, const unsigned char* pKey2, size_t pSize2) {
  if ((pKey1 == nullptr)||(pSize1==0)) {
    if ((pKey2 == nullptr)||(pSize2==0))
      return 0;   /* both keys are empty : equality */
    return - 1;   /* key2 greater than key1 : negative value */
  }
  if ((pKey2 == nullptr)||(pSize2==0)) {
    return 1 ;    /* key1 (whatever value) greater than key2 (empty) : positive value */
  }

  ZDataBuffer wValue1,wValue2;
  ZStatus wSt;
  ZTypeBase wType;
  ssize_t    wSize;
  const unsigned char* wURF1 = pKey1;
  const unsigned char* wURF1_End = pKey1 + pSize1;
  const unsigned char* wEnd1 = pKey1 + pSize1;
  const unsigned char* wURF2 = pKey2 ;
  const unsigned char* wURF2_End = pKey2 + pSize2;
  const unsigned char* wEnd2 = pKey2 + pSize2;

  int wRet=0;
  wSt=URFParser::getURFTypeAndSize(pKey1,wType,wSize);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("URFCompare",wSt,Severity_Fatal,"Error while comparing key values. Key1 type %X %s size %ld");
    ZException.exit_abort();
  }
  wURF1_End = wURF1 + wSize;

  wSt=URFParser::getURFTypeAndSize(pKey2,wType,wSize);
  if (wSt!=ZS_SUCCESS){
    ZException.setMessage("URFCompare",wSt,Severity_Fatal,"Error while comparing key values.");
    ZException.exit_abort();
  }
  wURF2_End = wURF2 + wSize;

  wRet = URFCompareValues (wURF1,wURF1_End,wURF2,wURF2_End);  /* pURF1 and pURF2 are updated */

  while ( (wRet==0) && (wSt == ZS_SUCCESS ) && (wURF1 < wEnd1) && (wURF2 < wEnd2) ) {
    wSt=URFParser::getURFTypeAndSize(wURF1,wType,wSize);
    if (wSt!=ZS_SUCCESS)
      break;
    wURF1_End = wURF1 + wSize;
    wSt=URFParser::getURFTypeAndSize(wURF2,wType,wSize);
    if (wSt!=ZS_SUCCESS)
      break;
    wURF2_End = wURF2 + wSize;
    wRet = URFCompareValues (wURF1,wURF1_End,wURF2,wURF2_End);  /* pURF1 and pURF2 are updated */
  }// while
  return wRet;
} // URFCompare





int URFCompareValues( const unsigned char* &pURF1,const unsigned char* pURF1_End,
                      const unsigned char* &pURF2,const unsigned char* pURF2_End) {
  int wRet=0;

  if (pURF1==nullptr) {
    if (pURF2==nullptr)
      return 0;
    else {
      pURF2 = pURF2_End;
      return -1;
    }
  }
  if (pURF2==nullptr)
    return 1;

  wRet=0;
  while ((wRet==0) && (pURF1 < pURF1_End) && (pURF2 < pURF2_End)){
    wRet = (*pURF1++)-(*pURF2++);
  }
  if (wRet==0) {
    if (pURF1 > pURF1_End) {
      pURF2 = pURF2_End;
      pURF1 = pURF1_End;
      return 1; /* key1 is greater than key2 */
    }
    if (pURF2 > pURF2_End) {
      pURF2 = pURF2_End;
      pURF1 = pURF1_End;
      return -1; /* key1 is less than key2 */
    }
    pURF2 = pURF2_End;
    pURF1 = pURF1_End;
    /* equality in size and in values */
    return 0;
  }
  return wRet;
}
