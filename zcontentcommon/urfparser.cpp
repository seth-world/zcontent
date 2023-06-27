#include "urfparser.h"
#include <zcontentcommon/zresource.h>
#include <zcontentutils/zentity.h>
//#include <ztoolset/zdate.h>
#include <ztoolset/zdatefull.h>
#include <ztoolset/zdatabuffer.h>

#include <ztoolset/zcharset.h>
#include <zcontent/zcontentcommon/zresource.h>

ZStatus URFParser::set(const ZDataBuffer* pRecord) {
  Record = pRecord;
  URFFieldList.clear();
  URFDataSize = 0;
  ZTypeBase wZType;

  AllFieldsPresent=false;
  URFDataSize=0;

  Ptr=Record->Data;
  PtrEnd = Record->Data + Record->Size;

  _importAtomic<ZTypeBase>(wZType,Ptr);
  while (true) {
    if (wZType==ZType_bitset) {
      Ptr -= sizeof(ZTypeBase);
      ssize_t wS = Presence._importURF(Ptr);
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
 // _importAtomic<uint64_t>(URFDataSize,wPtr); // Deprecated

  return ZS_SUCCESS;  /* done */
}// set


ZStatus URFParser::parse(const ZDataBuffer &pRecord, ZArray<URFField> &pFieldList) {

  pFieldList.clear();

  ZTypeBase wZType;
  int wRank=0;
  bool    wAllFieldsPresent=false;
  ZBitset wPresence;


  const unsigned char* wPtr=pRecord.Data;
  const unsigned char* wPtrEnd = pRecord.Data + pRecord.Size;

  _importAtomic<ZTypeBase>(wZType,wPtr);
  while (true) {
    if (wZType==ZType_bitset) {
      wPtr -= sizeof(ZTypeBase);
      ssize_t wS = wPresence._importURF(wPtr);
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
      wAllFieldsPresent=true;
      break;
    } // if (wZType!=ZType_bitsetFull)
    /* if neither bitset nor bitsetfull is found then record is not properly formed */
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_CORRUPTED,
        Severity_Severe,
        "Record appears to be malformed : neither ZBitset nor ZBitsetFull has been found.");
    return  ZS_CORRUPTED;
  }// while true


  URFField wField;
  while (wPtr < wPtrEnd) {
    wField.Ptr = wPtr ;
    if (!wPresence.test(wRank)) {
      wField.Present = false;
      wField.Size = 0 ;
    }
    else {
    wField.Present = true;
    wField.setFromPtr(wPtr);
    }
    pFieldList.push(wField);
    wRank++;
  } // while

  return ZS_SUCCESS;  /* done */
}// parse


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

  while ((Ptr < PtrEnd )&&(wRank <= pRank)) {

    if ( (!AllFieldsPresent) && !(Presence.test(size_t(wRank)) ) ) {
      wField.Ptr = Ptr ;
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
    wField.Ptr = Ptr;
    wField.Present = true;
    wField.Size = getURFFieldSize(Ptr);
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
      wSt=searchNextValidZType(Ptr,PtrEnd);
      if (wSt==ZS_NOTFOUND)
        break;
      continue;
    }
    Ptr += wField.Size;
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

  while ((Ptr < PtrEnd )&&(wRank <= pRank)) {

    if ( (!AllFieldsPresent) && !(Presence.test(size_t(wRank)) ) ) {
        wField.Ptr = Ptr ;
        wField.Size = 0 ;
        wField.Present = false;
        URFFieldList << wField;
        if (wRank == pRank)
          break;
        wRank++;
        continue;
    }

    /* here field is present or all fields are declared to be present */
    wField.Ptr = Ptr;
    wField.Present = true;
    size_t wS=getURFFieldSize(Ptr);

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
    wSt=searchNextValidZType(Ptr,PtrEnd);
    if (wRank == pRank)
      break;
    wRank++;
  } // while

  if (wRank == pRank) {
      wSt=getKeyFieldValue(Ptr,wFieldValue);

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

} // ZTypeExists /home/gerard/Development/zbasetools/zcontent/zcontentcommon/urfparser.cpp

ZStatus URFParser::getURFTypeAndSize (const unsigned char *& pPtrIn,ZTypeBase& pType,ssize_t & pSize) {
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

    return sizeof(ZTypeBase)+ sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type)+ wByteSize;
  }
  case ZType_Utf16FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf16_t)));

    return sizeof(ZTypeBase)+ sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type)+ wByteSize;
  }
  case ZType_Utf32FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf32_t)));

    return sizeof(ZTypeBase)+ sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type)+ wByteSize;
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
    fprintf(stderr,"ZURFParser::getURFFieldSize-S-INVTYP Found invalid ZType <%X>. URF Format may be corrupted\n",
        wType);
    return -1;
  }

  }// switch
} // getURFSize

ssize_t
URFParser::getURFFieldAllSizes (const unsigned char*& pPtr, ZTypeBase& pType, size_t &pURFHeaderSize, size_t &pDataSize) {

  _importAtomic<ZTypeBase>(pType,pPtr);
  switch (pType) {
    /* atomic data :
     *  - ZTypeBase           (Raw)
     *  - Fixed size Value    (Raw)
     */
  case ZType_AtomicUChar:
  case ZType_UChar: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(unsigned char);
    return sizeof(ZTypeBase)+sizeof(unsigned char);
  }
  case ZType_AtomicChar:
  case ZType_Char: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(char);
    return sizeof(ZTypeBase)+sizeof(char);
  }
  case ZType_U8:
  case ZType_AtomicU8: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(uint8_t);
    return sizeof(ZTypeBase)+sizeof(uint8_t);
  }
  case ZType_AtomicS8:
  case ZType_S8: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(int8_t)+1;
    return sizeof(ZTypeBase)+sizeof(int8_t)+1;
  }
  case ZType_AtomicU16:
  case ZType_U16:{
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(uint16_t);
    return sizeof(ZTypeBase)+sizeof(uint16_t);
  }
  case ZType_AtomicS16:
  case ZType_S16: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(int16_t)+1;
    return sizeof(ZTypeBase)+sizeof(int16_t)+1;
  }
  case ZType_AtomicU32:
  case ZType_U32:{
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(uint32_t);
    return sizeof(ZTypeBase)+sizeof(uint32_t);
  }
  case ZType_AtomicS32:
  case ZType_S32: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(int32_t)+1;
    return sizeof(ZTypeBase)+sizeof(int32_t)+1;
  }
  case ZType_AtomicU64:
  case ZType_U64: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(uint64_t);
    return sizeof(ZTypeBase)+sizeof(uint64_t);
  }
  case ZType_AtomicS64:
  case ZType_S64: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(int64_t)+1;
    return sizeof(ZTypeBase)+sizeof(int64_t)+1;
  }
  case ZType_AtomicFloat:
  case ZType_Float: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(float)+1;
    return sizeof(ZTypeBase)+sizeof(float)+1;
  }
  case ZType_AtomicDouble:
  case ZType_Double: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(double)+1;
    return sizeof(ZTypeBase)+sizeof(double)+1;
  }
  case ZType_AtomicLDouble:
  case ZType_LDouble: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(long double)+1;
    return sizeof(ZTypeBase)+sizeof(long double)+1;
  }

    /* Non atomic data : fixed length structures */

  case ZType_ZDate: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(uint32_t);
    return sizeof(ZTypeBase)+sizeof(uint32_t);
  }

  case ZType_ZDateFull: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(uint64_t);
    return sizeof(ZTypeBase)+sizeof(uint64_t);
  }

  case ZType_CheckSum: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = cst_checksum;
    return sizeof(ZTypeBase)+cst_checksum;
  }

  case ZType_MD5: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = cst_md5;
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

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr); /* pPtr is updated */
    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf8_t)));

    pURFHeaderSize=sizeof(ZTypeBase)+sizeof(URF_UnitCount_type);
    pDataSize = wByteSize;

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ size_t(wByteSize);
  }

  case ZType_Utf16VaryingString:{
    URF_UnitCount_type wUnitsCount;

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr); /* pPtrIn is updated */
    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf16_t)));

    pURFHeaderSize=sizeof(ZTypeBase)+sizeof(URF_UnitCount_type);
    pDataSize = wByteSize;

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ size_t(wByteSize);
  }
  case ZType_Utf32VaryingString:{
    URF_UnitCount_type wUnitsCount;

    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr); /* pPtrIn is updated */
    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf32_t)));


    pURFHeaderSize=sizeof(ZTypeBase)+sizeof(URF_UnitCount_type);
    pDataSize = wByteSize;

    return sizeof(ZTypeBase)+ sizeof(URF_UnitCount_type)+ size_t(wByteSize);
  }

    /* for fixed string URF header is different */

  case ZType_Utf8FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);     /* pPtr is updated */
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf8_t)));


    pURFHeaderSize=sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
    pDataSize = wByteSize;

    return sizeof(ZTypeBase)+ sizeof(URF_Capacity_type)+ sizeof(URF_UnitCount_type)+ wByteSize;
  }
  case ZType_Utf16FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf16_t)));

    pURFHeaderSize=sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
    pDataSize = wByteSize;

    return sizeof(ZTypeBase)+ sizeof(URF_Capacity_type)+ sizeof(URF_UnitCount_type)+ wByteSize;
  }
  case ZType_Utf32FixedString:
  {
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    size_t wByteSize = size_t(wUnitsCount * URF_UnitCount_type(sizeof(utf32_t)));

    pURFHeaderSize=sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
    pDataSize = wByteSize;

    return sizeof(ZTypeBase)+ sizeof(URF_Capacity_type)+ sizeof(URF_UnitCount_type)+ wByteSize;
  }

    /* NB neither bitset nor bitsetfull are relevant for a key field */
  case ZType_bitset: {
    URF_UnitCount_type  wByteSize;
    uint16_t wByteSize1, wEBitSize;
    _importAtomic<uint16_t>(wByteSize1,pPtr);
    _importAtomic<uint16_t>(wEBitSize,pPtr);

    wByteSize = wByteSize1;

    pURFHeaderSize=sizeof(ZTypeBase)+sizeof(uint16_t) + sizeof(uint16_t);
    pDataSize = wByteSize;

    return sizeof(ZTypeBase)+ sizeof(uint16_t)+ sizeof(uint16_t) +size_t(wByteSize);
  }

  case ZType_bitsetFull: {
    /* no value */
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = 0;
    return sizeof(ZTypeBase);
  }

  case ZType_Resource: {
    pURFHeaderSize=sizeof(ZTypeBase);
    pDataSize = sizeof(ZEntity_type) + sizeof(Resourceid_type);

    return sizeof(ZTypeBase) + sizeof(ZEntity_type) + sizeof(Resourceid_type);
  }

  default: {
    ZException.setMessage("URFParser::getURFFieldAllSizes",ZS_INVTYPE,Severity_Error,"Invalid/unknown given type %X %d",pType);
    return -1;
  }

  }// switch
} // URFParser::getURFFieldAllSizes



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



utf8VaryingString
URFParser::displayOneURFField(const unsigned char* &wPtr,bool pShowZType) {
  utf8VaryingString wReturn;
  ZTypeBase wZType;

  ZStatus wSt=ZS_SUCCESS;

  utf8VaryingString wStr;
  _importAtomic<ZTypeBase>(wZType,wPtr);


  /* for atomic URF data, value is just following ZType. For other types, use _importURF function that implies ZType */
  if (!(wZType & ZType_Atomic))
    wPtr -= sizeof(ZTypeBase);
  else  {
    wZType &= ~ZType_Atomic;
  }
  switch (wZType) {
  case ZType_UChar:
  case ZType_U8: {
    uint8_t wValue;

    wValue=convertAtomicBack<uint8_t> (ZType_U8,wPtr);
    if (pShowZType)
      wReturn.sprintf("uint8_t[%d]",wValue);
    else
      wReturn.sprintf("%d",wValue);
    return wReturn;
  }
  case ZType_Char:
  case ZType_S8: {
    int8_t wValue;
    wValue=convertAtomicBack<int8_t> (ZType_S8,wPtr);
    if (pShowZType)
      wReturn.sprintf("int8_t[%d]",wValue);
    else
      wReturn.sprintf("%d",wValue);
    return wReturn;
  }
  case ZType_U16:{
    uint16_t wValue;
    wValue=convertAtomicBack<uint16_t> (ZType_U16,wPtr);
    if (pShowZType)
      wReturn.sprintf("uint16_t[%d]",wValue);
    else
      wReturn.sprintf("%d",wValue);
    return wReturn;
  }
  case ZType_S16: {
    int16_t wValue;
    wValue=convertAtomicBack<int16_t> (ZType_S16,wPtr);
    if (pShowZType)
      wReturn.sprintf("int16_t[%d]",wValue);
    else
      wReturn.sprintf("%d",wValue);
    return wReturn;
  }

  case ZType_U32:{
    uint32_t wValue;
    wValue=convertAtomicBack<uint32_t> (ZType_U32,wPtr);
    if (pShowZType)
      wReturn.sprintf("uint32_t[%d]",wValue);
    else
      wReturn.sprintf("%d",wValue);
    return wReturn;
  }
  case ZType_S32: {
    int32_t wValue;
    wValue=convertAtomicBack<int32_t> (ZType_S32,wPtr);
    if (pShowZType)
      wReturn.sprintf("int32_t[%d]",wValue);
    else
      wReturn.sprintf("%d",wValue);
    return wReturn;
  }
  case ZType_U64: {
    uint64_t wValue;
    wValue=convertAtomicBack<uint64_t> (ZType_U64,wPtr);
    if (pShowZType)
      wReturn.sprintf("uint64_t[%ld]",wValue);
    else
      wReturn.sprintf("%ld",wValue);
    return wReturn;
  }
  case ZType_S64: {
    int64_t wValue;
    wValue=convertAtomicBack<int64_t> (ZType_S64,wPtr);
    if (pShowZType)
      wReturn.sprintf("int64_t[%ld]",wValue);
    else
      wReturn.sprintf("%ld",wValue);
    return wReturn;
  }
  case ZType_Float: {
    float wValue;
    wValue=convertAtomicBack<float> (ZType_Float,wPtr);
    if (pShowZType)
      wReturn.sprintf("float[%g]",double(wValue));
    else
      wReturn.sprintf("%g",double(wValue));
    return wReturn;
  }

  case ZType_Double: {
    double wValue;
    wValue=convertAtomicBack<double> (ZType_Double,wPtr);
    if (pShowZType)
      wReturn.sprintf("double[%g]",wValue);
    else
      wReturn.sprintf("%g",wValue);
    return wReturn;
  }

  case ZType_LDouble: {
    long double wValue;
    wValue=convertAtomicBack<long double> (ZType_LDouble,wPtr);
    if (pShowZType)
      wReturn.sprintf("long double[%lf]",wValue);
    else
      wReturn.sprintf("%lf",wValue);
    return wReturn;
  }

    /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */
/* Deprecated
  case ZType_ZDate: {
    ssize_t wSize;
    ZDate wZDate;
    if ((wSize = wZDate._importURF(wPtr)) < 0) {
      if (pShowZType)
        wReturn = "ZDate[**Invalid value**]" ;
      else
        wReturn = "**Invalid value**" ;
      return wReturn;
    }
    if (!pShowZType)
      return wZDate.toLocale();
    wReturn.sprintf("ZDate[%s]",wZDate.toLocale().toCChar());
    return wReturn;
  }
*/
  case ZType_ZDateFull: {
    ssize_t wSize;
    ZDateFull wZDateFull;

    if ((wSize = wZDateFull._importURF(wPtr)) < 0) {
      if (pShowZType)
        wReturn = "ZDateFull[**Invalid value**]" ;
      else
        wReturn = "**Invalid value**" ;
      return wReturn;
    }
    if (!pShowZType)
      return wZDateFull.toLocale();
    wReturn.sprintf("ZDateFull[%s]",wZDateFull.toLocale().toCChar());
    return wReturn;
  }

  case ZType_URIString:{
    uriString wString;
    ssize_t wSize = wString._importURF(wPtr);
    if (!pShowZType)
      return wString.toCChar();
    wReturn.sprintf("uriString[%s]",wString.toCChar());
    return wReturn;
  }
  case ZType_Utf8VaryingString: {
    utf8VaryingString wString;
    ssize_t wSize = wString._importURF(wPtr);
    if (!pShowZType)
      return wString.toCChar();
    wReturn.sprintf("utf8VaryingString[%s]",wString.toCChar());
    return wReturn;
  }


  case ZType_Utf16VaryingString:{
    utf16VaryingString wString;

    ssize_t wSize = wString._importURF(wPtr);
    if (!pShowZType)
      return wString.toUtf8().toCChar();

    wReturn.sprintf("utf16VaryingString[%s]",wString.toUtf8().toCChar());
    return wReturn;
  }
  case ZType_Utf32VaryingString:{
    utf32VaryingString wString;
    utf16VaryingString wAdd;

    ssize_t wSize = wString._importURF(wPtr);
    if (!pShowZType)
      return wString.toUtf8().toCChar();
    wReturn.sprintf("utf32VaryingString[%s]",wString.toUtf8().toCChar());
    return wReturn;
  }

  case ZType_Utf8FixedString:{
    utf8VaryingString wString;

    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wStringByteSize;

    wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

    wStringByteSize = size_t (wUnitsCount) * sizeof(utf8_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf8_t* wPtrOut = (utf8_t*)wString.Data;
    utf8_t* wPtrIn = (utf8_t*)wPtr;
    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wPtr = (unsigned char*) wPtrIn;
    if (!pShowZType)
      return wString.toCChar();
    wReturn.sprintf("utf8FixedString[%s]",wString.toCChar());
    return wReturn;
  }

    /* for fixed string URF header is different */

  case ZType_Utf16FixedString:{
    utf16VaryingString wString;

    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;


    wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf16_t);

    /* the whole string must be imported, then possibly truncated afterwards to maximum displayable */

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf16_t* wPtrOut = (utf16_t*)wString.Data;
    utf16_t* wPtrIn = (utf16_t*)wPtr;
    while ( wI-- && *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wPtr = (unsigned char*) wPtrIn;
    if (!pShowZType)
      return wString.toUtf8().toCChar();

    wReturn.sprintf("utf16FixedString[%s]",wString.toUtf8().toCChar());
    return wReturn;
  }

  case ZType_Utf32FixedString:{
    utf32VaryingString wString;
    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;

    wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf32_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf32_t* wPtrOut = (utf32_t*)wString.Data;
    utf32_t* wPtrIn = (utf32_t*)wPtr;

    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wPtr = (unsigned char*) wPtrIn;

    if (!pShowZType)
      return wString.toUtf8().toCChar();
    wReturn.sprintf("utf32FixedString[%s]",wString.toUtf8().toCChar());
    return wReturn;
  }

  case ZType_CheckSum: {
    checkSum wCheckSum;

    wCheckSum._importURF(wPtr);

    if (!pShowZType)
      return wCheckSum.toHexa().toCChar();
    wReturn.sprintf("checksum[%s]",wCheckSum.toHexa().toCChar());
    return wReturn;
  }

  case ZType_MD5: {
    md5 wCheckSum;

    wCheckSum._importURF(wPtr);

    if (!pShowZType)
      return wCheckSum.toHexa().toCChar();

    wReturn.sprintf("md5[%s]",wCheckSum.toHexa().toCChar());
    return wReturn;
  }

  case ZType_Blob: {
    ZDataBuffer wBlob;
    ssize_t wSize=wBlob._importURF(wPtr);

    wReturn.sprintf("blob[%s]",wBlob.DumpS(16,60).toCChar());
    return wReturn;
  }

  case ZType_bitset: {
    ZBitset wBitset;

    ssize_t wSize=wBitset._importURF(wPtr);
    if (!pShowZType)
      return wBitset.toString();

    wReturn.sprintf("ZBitset[%s]",wBitset.toString().toCChar());
    return wReturn;
  }

  case ZType_bitsetFull: {
    wReturn = "ZBitsetFull[]";
    return wReturn;
  }

  case ZType_Resource: {
    ZResource wValue;
    ssize_t wSize=wValue._importURF(wPtr);
    if (pShowZType)
      wReturn.sprintf("ZResource[<%s>-<%ld>]",decode_ZEntity(wValue.Entity).toCChar(),wValue.id);
    else
      wReturn.sprintf("<%s>-<%ld>",decode_ZEntity(wValue.Entity).toCChar(),wValue.id);
    return wReturn;
  }

  default: {
    wReturn.sprintf("Unknown data type[----]");
    return wReturn;
  }

  }// switch

} // display urf field




/** @brief URFCompare  Compare two buffers composed each of one or many URF fields, each field potentially of variable length.
 *  returns 0 when both buffers are equal
 *          1 or positive value if pKey1 is greater than pKey2
 *          -1 or negative value if pKey1 is less than pKey2
 *
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
  size_t    wSize1,wSize2,wURFHSize1,wURFHSize2;
  const unsigned char* wURF1 = pKey1;
  const unsigned char* wEnd1 = pKey1 + pSize1;
  const unsigned char* wURF2 = pKey2 ;
  const unsigned char* wEnd2 = pKey2 + pSize2;

  int wRet=0;
//  wSt=URFParser::getURFTypeAndSize(pKey1,wType,wSize1);
  /* pKey1 is updated and points to true field data - wSize1 is the effective data byte size */
  ssize_t wParserRet=URFParser::getURFFieldAllSizes(pKey1,wType,wURFHSize1,wSize1); /* pKey1 is updated and points to true field data */
  if (wParserRet < 0) {
    ZException.setMessage("URFCompare",wSt,Severity_Fatal,"Error while comparing key values. Key1 type %X %s size %ld decoded type %s",
                            wType,wType,wSize1,decode_ZType(wType));
    ZException.exit_abort();
  }

//  wSt=URFParser::getURFTypeAndSize(pKey2,wType,wSize2);
  /* pKey2 is updated and points to true field data - wSize2 is the effective data byte size */
  wParserRet=URFParser::getURFFieldAllSizes(pKey2,wType,wURFHSize2,wSize2);
  if (wParserRet < 0) {
    ZException.setMessage("URFCompare",wSt,Severity_Fatal,"Error while comparing key values. Key2 type %X %s size %ld decoded type %s",
        wType,wType,wSize2,decode_ZType(wType));
    ZException.exit_abort();
  }

  /* pKey1 and pKey2 are updated to point outside key content at the end of compare routine */
  if (wType & ZType_U8) {
    wRet = UTFCompare<utf8_t>(pKey1,wSize1,pKey2,wSize2);
  } else if (wType & ZType_U16) {
    wRet = UTFCompare<utf16_t>(pKey1,wSize1,pKey2,wSize2);
  } else if (wType & ZType_U32) {
    wRet = UTFCompare<utf32_t>(pKey1,wSize1,pKey2,wSize2);
  }
  else
    wRet = URFCompareValues (pKey1,wSize1,pKey2,wSize2);

  while ( (wRet==0) && (wSt == ZS_SUCCESS ) && (pKey1 < wEnd1) && (pKey2 < wEnd2) ) {

    wParserRet=URFParser::getURFFieldAllSizes(pKey1,wType,wURFHSize1,wSize1); /* pKey1 is updated and points to true field data */
    if (wParserRet < 0) {
      ZException.setMessage("URFCompare",wSt,Severity_Fatal,"Error while comparing key values. Key1 type %X %s size %ld decoded type %s",
          wType,wType,wSize1,decode_ZType(wType));
      ZException.exit_abort();
    }

    wParserRet=URFParser::getURFFieldAllSizes(pKey2,wType,wURFHSize2,wSize2); /* pKey2 is updated and points to true field data */
    if (wParserRet < 0) {
      ZException.setMessage("URFCompare",wSt,Severity_Fatal,"Error while comparing key values. Key2 type %X %s size %ld decoded type %s",
          wType,wType,wSize2,decode_ZType(wType));
      ZException.exit_abort();
    }
    /* pKey1 and pKey2 are updated to point outside key content at the end of compare routine */
    if (wType & ZType_U8) {
      wRet = UTFCompare<utf8_t>(pKey1,wSize1,pKey2,wSize2);
    } else if (wType & ZType_U16) {
      wRet = UTFCompare<utf16_t>(pKey1,wSize1,pKey2,wSize2);
    } else if (wType & ZType_U32) {
      wRet = UTFCompare<utf32_t>(pKey1,wSize1,pKey2,wSize2);
    }
    else
      wRet = URFCompareValues (pKey1,wSize1,pKey2,wSize2);
  }// while
  return wRet;
} // URFCompare





int URFCompareValues( const unsigned char* &pURF1,size_t pSize1,
                      const unsigned char* &pURF2,size_t pSize2) {
  int wRet=0;
  size_t wSize1=pSize1,wSize2=pSize2;
  const unsigned char* wURF1=pURF1;
  const unsigned char* wURF2=pURF2;

  if (pURF1==nullptr) {
    if (pURF2==nullptr)
      return 0;
    else {
      pURF2 += pSize2;
      return -1;
    }
  }
  if (pURF2==nullptr) {
    pURF1 += pSize1;
    return 1;
  }

  wRet=0;
  while ((wRet==0) && (wSize1 > 0) && (wSize2 > 0)){
    wRet = (*wURF1++) - (*wURF2++);
    wSize1--;
    wSize2--;
  }
  pURF1 += pSize1;
  pURF2 += pSize2;
  if (wRet){
    return wRet;
  }
  /* one of the two or both keys are at the end and so far they are equal */

  /* if equals and both length are equal */
  if ((!wSize1)&&(!wSize2)) {
    return wRet;
  }

  if (wSize1) { /* but Key1 has a greater size than key2 */
    wURF1 += wSize1;
    return 1; /* key1 is greater than key2 */
  }
  if (wSize2) { /* Key2 has a greater size than key 1 */
    wURF2 += wSize2;
    return -1; /* key1 is less than key2 */
  }
    /* both are equal in values and sizes (sizes are exhausted) */
  return 0;
}// URFCompareValues


#ifdef __COMMENT__

int UTF8Compare(const unsigned char* &pKey1,size_t pSize1,const unsigned char* &pKey2,size_t pSize2) {
  utf8_t* wKey1 = (utf8_t* )pKey1;
  utf8_t* wKey2 = (utf8_t* )pKey2;
  if (pKey1==nullptr) {
    if (pKey2==nullptr)
      return 0;
    else {
      pKey2 += pSize2;
      return -1;
    }
  }
  if (pKey2==nullptr) {
    pKey1 += pSize1;
    return 1;
  }

  utf8_t pV1,pV2;

  size_t wI1=0, wI2=0;
  int wRet=0;

  //  while ((wRet==0)&&(wI1<pSize1) &&(wI2<pSize2) && pKey1[wI1] && pKey2[wI2]) {
  while ((wRet==0) && (wI1<pSize1) && (wI2<pSize2) ) {
    /*  uppercase no accute */
    pV1=KeyCharConvert(pKey1[wI1++]);
    pV2=KeyCharConvert(pKey2[wI2++]);

    wRet=pV1-pV2;
  }

  pKey1 += pSize1;
  pKey2 += pSize2;

  /* one of the two -or both- strings are at the end and so far they are equal */
  if (wI1==pSize1) { /* but Key1 has a greater size than key2 */
    return 1; /* key1 is greater than key2 */
  }
  if (wI2==pSize2) { /* Key2 has a greater size than key 1 */
    return -1; /* key1 is less than key2 */
  }
  /* both are equal in values and sizes (sizes are exhausted) */

  return wRet;
}//compareUtf8


int UTF32Compare(const unsigned char* &pKey1,size_t pSize1,const unsigned char* &pKey2,size_t pSize2) {

  size_t wCount1=pSize1/sizeof(utf32_t);
  size_t wCount2=pSize1/sizeof(utf32_t);

  utf32_t* wKey1 = (utf32_t* )pKey1;
  utf32_t* wKey2 = (utf32_t* )pKey2;

  if (pKey1==nullptr) {
    if (pKey2==nullptr)
      return 0;
    else {
      pKey2 += pSize2;
      return -1;
    }
  }
  if (pKey2==nullptr) {
    pKey1 += pSize1;
    return 1;
  }

  utf32_t pV1,pV2;

  size_t wI1=0, wI2=0;
  int wRet=0;

  //  while ((wRet==0)&&(wI1<pSize1) &&(wI2<pSize2) && pKey1[wI1] && pKey2[wI2]) {
  while ((wRet==0) && (wI1<wCount1) && (wI2<wCount2) ) {
    /*  uppercase no accute */
    pV1=convertChar<utf32_t>(wKey1[wI1++]);
    pV2=convertChar<utf32_t>(wKey2[wI2++]);

    wRet=pV1-pV2;
  }

  pKey1 += pSize1;
  pKey2 += pSize2;

  /* one of the two -or both- strings are at the end and so far they are equal */
  if (wI1==wCount1) { /* but Key1 has a greater size than key2 */
    return 1; /* key1 is greater than key2 */
  }
  if (wI2==wCount2) { /* Key2 has a greater size than key 1 */
    return -1; /* key1 is less than key2 */
  }
  /* both are equal in values and sizes (sizes are exhausted) */

  return wRet;
} // UTF32Compare
#endif // __COMMENT__
