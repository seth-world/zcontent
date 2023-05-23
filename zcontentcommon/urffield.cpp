#include "urffield.h"
#include <ztoolset/ztypetype.h>

#include <ztoolset/zdatabuffer.h>
#include <zcontent/zcontentutils/zentity.h>
#include <zcontent/zcontentcommon/zresource.h>

#include <ztoolset/zexceptionmin.h>

#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/utffixedstring.h>

//#include <ztoolset/zdate.h>
#include <ztoolset/zdatefull.h>

#include <ztoolset/zbitset.h>

#include <zcrypt/checksum.h>
#include <zcrypt/md5.h>



URFField::URFField()
{

}

ZStatus URFField::setFromPtr(const unsigned char* &pPtrIn) {
  Ptr=pPtrIn;
  Size = sizeof(ZTypeBase);

  _importAtomic<ZTypeBase>(ZType,pPtrIn);
  switch (ZType){
  case ZType_UChar:
  case ZType_AtomicUChar:
  case ZType_U8:
  case ZType_AtomicU8: {
    Size += sizeof(uint8_t);
    pPtrIn += sizeof(uint8_t);
    break;
  }
  case ZType_Char:
  case ZType_AtomicChar:
  case ZType_S8:
  case ZType_AtomicS8: {
    Size += sizeof(int8_t)+1;
    pPtrIn += sizeof(int8_t)+1;
    break;
  }
  case ZType_U16:
  case ZType_AtomicU16:{
    Size += sizeof(uint16_t);
    pPtrIn += sizeof(uint16_t);
    break;
  }
  case ZType_S16:
  case ZType_AtomicS16: {
    Size += sizeof(int16_t)+1;
    pPtrIn += sizeof(int16_t)+1;
    break;
  }

  case ZType_U32:
  case ZType_AtomicU32:{
    Size += sizeof(uint32_t);
    pPtrIn += sizeof(uint32_t);
    break;
  }
  case ZType_S32:
  case ZType_AtomicS32: {
    Size += sizeof(int32_t)+1;
    pPtrIn += sizeof(int32_t)+1;
    break;
  }
  case ZType_U64:
  case ZType_AtomicU64: {
    Size += sizeof(uint64_t);
    pPtrIn += sizeof(uint64_t);
    break;
  }
  case ZType_S64:
  case ZType_AtomicS64: {
    Size += sizeof(int64_t)+1;
    pPtrIn += sizeof(int64_t)+1;
    break;
  }
  case ZType_Float:
  case ZType_AtomicFloat: {
    Size += sizeof(float)+1;
    pPtrIn += sizeof(float)+1;
    break;
  }
  case ZType_Double:
  case ZType_AtomicDouble: {
    Size += sizeof(double)+1;
    pPtrIn += sizeof(double)+1;
    break;
  }

  case ZType_LDouble:
  case ZType_AtomicLDouble: {
    Size += sizeof(long double)+1;
    pPtrIn += sizeof(long double)+1;
    break;
  }

    /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */

  case ZType_ZDate: {
    Size += sizeof(uint32_t);
    pPtrIn += sizeof(uint32_t);
    break;
  }
  case ZType_ZDateFull: {
    Size += sizeof(uint64_t);
    pPtrIn += sizeof(uint64_t);
    break;
  }

  case ZType_URIString:{
    URF_UnitCount_type wUnitCount;
    Size += _importAtomic<URF_UnitCount_type>(wUnitCount,pPtrIn);
    Size += wUnitCount * sizeof(utf8_t);
    pPtrIn += wUnitCount * sizeof(utf8_t);
    break;
  }
  case ZType_Utf8VaryingString: {
    URF_UnitCount_type wUnitCount;
    Size += _importAtomic<URF_UnitCount_type>(wUnitCount,pPtrIn);
    Size += wUnitCount * sizeof(utf8_t);
    pPtrIn += wUnitCount * sizeof(utf8_t);
    break;
  }

  case ZType_Utf16VaryingString:{
    URF_UnitCount_type wUnitCount;
    Size += _importAtomic<URF_UnitCount_type>(wUnitCount,pPtrIn);
    Size += wUnitCount * sizeof(utf16_t);
    pPtrIn += wUnitCount * sizeof(utf16_t);
    break;
  }
  case ZType_Utf32VaryingString:{
    URF_UnitCount_type wUnitCount;
    Size += _importAtomic<URF_UnitCount_type>(wUnitCount,pPtrIn);
    Size += wUnitCount * sizeof(utf32_t);
    pPtrIn += wUnitCount * sizeof(utf32_t);
    break;
  }

  case ZType_Utf8FixedString:{
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    Size +=_importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    Size +=_importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    Size += size_t (wUnitsCount) * sizeof(utf8_t);
    pPtrIn += size_t (wUnitsCount) * sizeof(utf8_t);
    break;
  }

    /* for fixed string URF header is different */

  case ZType_Utf16FixedString:{
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    Size +=_importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    Size +=_importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    Size += size_t (wUnitsCount) * sizeof(utf16_t);
    pPtrIn += size_t (wUnitsCount) * sizeof(utf16_t);
    break;
  }

  case ZType_Utf32FixedString:{
    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;

    Size +=_importAtomic<URF_Capacity_type>(wCapacity,pPtrIn);
    Size +=_importAtomic<URF_UnitCount_type>(wUnitsCount,pPtrIn);

    Size += size_t (wUnitsCount) * sizeof(utf32_t);
    pPtrIn += size_t (wUnitsCount) * sizeof(utf32_t);
    break;
  }

  case ZType_CheckSum: {
    Size += cst_checksum ;
    pPtrIn += cst_checksum;
    break;
  }

  case ZType_MD5: {
    Size += cst_md5 ;
    break;
  }

  case ZType_Blob: {
    uint64_t wDataSize;
    Size += _importAtomic(wDataSize,pPtrIn);
    Size += wDataSize;
    pPtrIn += wDataSize;
    break;
  }

  case ZType_bitset: {
    uint16_t    wByteSize,wEffectiveBitSize;
    Size += _importAtomic<uint16_t>(wByteSize,pPtrIn);
    Size += _importAtomic<uint16_t>(wEffectiveBitSize,pPtrIn);
    Size += size_t(wByteSize);
    pPtrIn += size_t(wByteSize);
    break;
  }

  case ZType_bitsetFull: {
    break;
  }

  case ZType_Resource: {
    Size += sizeof(ZEntity_type)+sizeof(Resourceid_type);
    pPtrIn += sizeof(ZEntity_type)+sizeof(Resourceid_type);
    break;
  }

  default: {
    ZException.setMessage("getURFField",ZS_INVTYPE,Severity_Error,"Invalid ZType found %X ",ZType);
    Size=0;
    Ptr=nullptr;
    Present = false;
    return ZS_INVTYPE;
  }
  } // switch
  Present = true;
  return ZS_SUCCESS;
}// fromPtr

URFField
getURFField(const unsigned char* &pPtrIn) {
  URFField wF;
  wF.setFromPtr(pPtrIn);
  return wF;
} //getURFField


utf8VaryingString
URFField::display() {
  if (!Present) {
    return "not present";
  }
  ZTypeBase wZType;
  const unsigned char* wPtr=Ptr;
  utf8VaryingString wReturn;
  Size = sizeof(ZTypeBase);
  _importAtomic<ZTypeBase>(wZType,wPtr);
  /* for atomic URF data, value is just following ZType. For other types, use _importURF function that implies ZType */
  if (wZType & ZType_Atomic) {
    wZType &= ~ZType_Atomic;
  } else
    wPtr -= sizeof(ZTypeBase);


  switch (wZType) {
  case ZType_UChar:
  case ZType_U8: {
    uint8_t wValue;
    wValue=convertAtomicBack<uint8_t> (ZType_U8,wPtr);
    wReturn.sprintf("%u 0x%X",wValue,wValue);
    return wReturn;
  }
  case ZType_Char:
  case ZType_S8: {
    int8_t wValue;
    wValue=convertAtomicBack<int8_t> (ZType_S8,wPtr);
    wReturn.sprintf("%d 0x%X",wValue,wValue);
    return wReturn;
  }
  case ZType_U16:{
    uint16_t wValue;
    wValue=convertAtomicBack<uint16_t> (ZType_U16,wPtr);
    wReturn.sprintf("%u 0x%X",wValue,wValue);
    return wReturn;
  }
  case ZType_S16: {
    int16_t wValue;
    wValue=convertAtomicBack<int16_t> (ZType_S16,wPtr);
    wReturn.sprintf("%d 0x%X",wValue,wValue);
    return wReturn;
  }

  case ZType_U32:{
    uint32_t wValue;
    wValue=convertAtomicBack<uint32_t> (ZType_U32,wPtr);
    wReturn.sprintf("%u 0x%X",wValue,wValue);
    return wReturn;
  }
  case ZType_S32: {
    int32_t wValue;
    wValue=convertAtomicBack<int32_t> (ZType_S32,wPtr);
    wReturn.sprintf("%d 0x%X",wValue,wValue);
    return wReturn;
  }
  case ZType_U64: {
    uint64_t wValue;
    wValue=convertAtomicBack<uint64_t> (ZType_U64,wPtr);
    wReturn.sprintf("%llu 0x%X",wValue,wValue);
    return wReturn;
  }
  case ZType_S64: {
    int64_t wValue;
    wValue=convertAtomicBack<int64_t> (ZType_S64,wPtr);
    wReturn.sprintf("%lld 0x%X",wValue,wValue);
    return wReturn;
  }
  case ZType_Float: {
    float wValue;
    wValue=convertAtomicBack<float> (ZType_Float,wPtr);
    wReturn.sprintf("%g",wValue);
    return wReturn;
  }

  case ZType_Double: {
    double wValue;
    wValue=convertAtomicBack<double> (ZType_Double,wPtr);
    wReturn.sprintf("%g",wValue);
    return wReturn;
  }

  case ZType_LDouble: {
    long double wValue;
    wValue=convertAtomicBack<long double> (ZType_LDouble,wPtr);
    wReturn.sprintf("%g",wValue);
    return wReturn;
  }

    /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */
/* Deprecated
  case ZType_ZDate: {
    ssize_t wSize;
    ZDate wZDate;
    if ((wSize = wZDate._importURF(wPtr)) < 0) {
      wReturn = "**Invalid date value**";
      return wReturn;
    }
    return wZDate.toLocale();
  }
*/
  case ZType_ZDateFull: {
    ssize_t wSize;
    ZDateFull wZDateFull;

    if ((wSize = wZDateFull._importURF(wPtr)) < 0) {
      wReturn = "**Invalid date value**";
      return wReturn;
    }
    return wZDateFull.toLocale();
  }

  case ZType_URIString:{
    uriString wString;
    ssize_t wSize = wString._importURF(wPtr);
    wReturn = wString;
    return wReturn;
  }
  case ZType_Utf8VaryingString: {
    ssize_t wSize = wReturn._importURF(wPtr);
    return wReturn;
  }

  case ZType_Utf16VaryingString:{
    utf16VaryingString wString;

    ssize_t wSize = wString._importURF(wPtr);

    wReturn.fromUtf16(wString);
    return wReturn;
  }
  case ZType_Utf32VaryingString:{
    utf32VaryingString wString;
    ssize_t wSize = wString._importURF(wPtr);
    wReturn.fromUtf32(wString);
    return wReturn;
  }

  case ZType_Utf8FixedString:{

    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wStringByteSize;

    wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

    wStringByteSize = size_t (wUnitsCount) * sizeof(utf8_t);

    URF_Capacity_type wI = wUnitsCount;

    wReturn.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf8_t* wPtrOut = (utf8_t*)wReturn.Data;
    utf8_t* wPtrIn = (utf8_t*)wPtr;
    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wPtr = (unsigned char*) wPtrIn;

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

    wReturn.fromUtf16(wString);
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

    wReturn.fromUtf32(wString);
    return wReturn;
  }

  case ZType_CheckSum: {
    checkSum wCheckSum;
    wCheckSum._importURF(wPtr);
    return wCheckSum.toHexa();
  }

  case ZType_MD5: {
    md5 wCheckSum;
    wCheckSum._importURF(wPtr);
    return wCheckSum.toHexa();
  }

  case ZType_Blob: {
    uint64_t wDataSize;
    wPtr += sizeof(ZTypeBase);
    _importAtomic(wDataSize,wPtr);
    return "<blob content>";
    wPtr += size_t(wDataSize);
    break;
  }

  case ZType_bitset: {
    ZBitset wBitset;

    ssize_t wSize=wBitset._importURF(wPtr);
    wReturn = "<";
    wReturn += wBitset.toString();
    wReturn += ">";
    return wReturn;
  }

  case ZType_bitsetFull: {
    return "<bitset full>";
  }

  case ZType_Resource: {
    ZResource wValue;
    ssize_t wSize=wValue._importURF(wPtr);
    return wValue.toStr();
  }

  default: {
    wReturn.sprintf ("Unknown data type <%d> <%X>",wZType,wZType);
    return wReturn;
  }

  }// switch

  return wReturn;
}
