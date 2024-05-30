#include "urffield.h"
#include "urfparser.h"

using namespace zbs;

#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/utffixedstring.h>

#include <ztoolset/ztypetype.h>

#include <ztoolset/zexceptionmin.h>
#include <ztoolset/zdatabuffer.h>
#include <zcontent/zcontentutils/zentity.h>
#include <zcontent/zcontentcommon/zresource.h>

#include <zcontent/zindexedfile/zsearchparser.h>



#include <zxml/zxmlprimitives.h>


//#include <ztoolset/zdate.h>
#include <ztoolset/zdatefull.h>

#include <ztoolset/zbitset.h>

#include <zcrypt/checksum.h>
#include <zcrypt/md5.h>

//#include "urfparser.h"
#include <ztoolset/zaierrors.h>

#include <ztoolset/zarray.h>

#include <zcontent/zindexedfile/zsearchfileentity.h>

namespace zbs {
extern class ZSearchParser* GParser;
extern ZArray<ZEntitySymbol>  ZEntitySymbolList;
} // namespace zbs


URFField::URFField()
{

}

ZStatus
URFField::setFromPtr(const ZDataBuffer &pRecord,
                     const unsigned char *&pPtrIn,
                     ZaiErrors *pErrorLog)
{
    ZStatus wSt=ZS_SUCCESS;
    FieldPtr = pPtrIn;
    Size = sizeof(ZTypeBase);

    while (true) {
        _importAtomic<ZTypeBase>(ZType, pPtrIn);
        switch (ZType) {
        case ZType_UChar:
        case ZType_AtomicUChar:
        case ZType_U8:
        case ZType_AtomicU8: {
            Size += sizeof(uint8_t);
            pPtrIn += sizeof(uint8_t);
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_Char:
        case ZType_AtomicChar:
        case ZType_S8:
        case ZType_AtomicS8: {
            Size += sizeof(int8_t) + 1;
            pPtrIn += sizeof(int8_t) + 1;
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_U16:
        case ZType_AtomicU16:
        {
            Size += sizeof(uint16_t);
            pPtrIn += sizeof(uint16_t);
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_S16:
        case ZType_AtomicS16: {
            Size += sizeof(int16_t) + 1;
            pPtrIn += sizeof(int16_t) + 1;
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_U32:
        case ZType_AtomicU32: {
            Size += sizeof(uint32_t);
            pPtrIn += sizeof(uint32_t);
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_S32:
        case ZType_AtomicS32: {
            Size += sizeof(int32_t) + 1;
            pPtrIn += sizeof(int32_t) + 1;
            break;
        }
        case ZType_U64:
        case ZType_AtomicU64: {
            Size += sizeof(uint64_t);
            pPtrIn += sizeof(uint64_t);
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_S64:
        case ZType_AtomicS64: {
            Size += sizeof(int64_t) + 1;
            pPtrIn += sizeof(int64_t) + 1;
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_Float:
        case ZType_AtomicFloat: {
            Size += sizeof(float) + 1;
            pPtrIn += sizeof(float) + 1;
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_Double:
        case ZType_AtomicDouble: {
            Size += sizeof(double) + 1;
            pPtrIn += sizeof(double) + 1;
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_LDouble:
        case ZType_AtomicLDouble: {
            Size += sizeof(long double) + 1;
            pPtrIn += sizeof(long double) + 1;
            Present = true;
            return ZS_SUCCESS;
        }

            /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */

        case ZType_ZDate: {
            Size += sizeof(uint32_t);
            pPtrIn += sizeof(uint32_t);
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_ZDateFull: {
            Size += sizeof(uint64_t);
            pPtrIn += sizeof(uint64_t);
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_URIString: {
            URF_UnitCount_type wUnitCount;
            Size += _importAtomic<URF_UnitCount_type>(wUnitCount, pPtrIn);
            Size += wUnitCount * sizeof(utf8_t);
            pPtrIn += wUnitCount * sizeof(utf8_t);
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_Utf8VaryingString: {
            URF_UnitCount_type wUnitCount;
            Size += _importAtomic<URF_UnitCount_type>(wUnitCount, pPtrIn);
            Size += wUnitCount * sizeof(utf8_t);
            pPtrIn += wUnitCount * sizeof(utf8_t);
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_Utf16VaryingString: {
            URF_UnitCount_type wUnitCount;
            Size += _importAtomic<URF_UnitCount_type>(wUnitCount, pPtrIn);
            Size += wUnitCount * sizeof(utf16_t);
            pPtrIn += wUnitCount * sizeof(utf16_t);
            Present = true;
            return ZS_SUCCESS;
        }
        case ZType_Utf32VaryingString: {
            URF_UnitCount_type wUnitCount;
            Size += _importAtomic<URF_UnitCount_type>(wUnitCount, pPtrIn);
            Size += wUnitCount * sizeof(utf32_t);
            pPtrIn += wUnitCount * sizeof(utf32_t);
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_Utf8FixedString: {
            URF_Capacity_type wCapacity;
            URF_UnitCount_type wUnitsCount;

            Size += _importAtomic<URF_Capacity_type>(wCapacity, pPtrIn);
            Size += _importAtomic<URF_UnitCount_type>(wUnitsCount, pPtrIn);

            Size += size_t(wUnitsCount) * sizeof(utf8_t);
            pPtrIn += size_t(wUnitsCount) * sizeof(utf8_t);
            Present = true;
            return ZS_SUCCESS;
        }

            /* for fixed string URF header is different */

        case ZType_Utf16FixedString: {
            URF_Capacity_type wCapacity;
            URF_UnitCount_type wUnitsCount;

            Size += _importAtomic<URF_Capacity_type>(wCapacity, pPtrIn);
            Size += _importAtomic<URF_UnitCount_type>(wUnitsCount, pPtrIn);

            Size += size_t(wUnitsCount) * sizeof(utf16_t);
            pPtrIn += size_t(wUnitsCount) * sizeof(utf16_t);
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_Utf32FixedString: {
            URF_Capacity_type wCapacity;
            URF_UnitCount_type wUnitsCount;

            Size += _importAtomic<URF_Capacity_type>(wCapacity, pPtrIn);
            Size += _importAtomic<URF_UnitCount_type>(wUnitsCount, pPtrIn);

            Size += size_t(wUnitsCount) * sizeof(utf32_t);
            pPtrIn += size_t(wUnitsCount) * sizeof(utf32_t);
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_CheckSum: {
            Size += cst_checksum;
            pPtrIn += cst_checksum;
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_MD5: {
            Size += cst_md5;
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_Blob: {
            uint64_t wDataSize;
            Size += _importAtomic(wDataSize, pPtrIn);
            Size += wDataSize;
            pPtrIn += wDataSize;
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_bitset: {
            uint16_t wByteSize, wEffectiveBitSize;
            Size += _importAtomic<uint16_t>(wByteSize, pPtrIn);
            Size += _importAtomic<uint16_t>(wEffectiveBitSize, pPtrIn);
            Size += size_t(wByteSize);
            pPtrIn += size_t(wByteSize);
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_bitsetFull: {
            Present = true;
            return ZS_SUCCESS;
        }

        case ZType_Resource: {
            Size += sizeof(ZEntity_type) + sizeof(Resourceid_type);
            pPtrIn += sizeof(ZEntity_type) + sizeof(Resourceid_type);
            Present = true;
            return ZS_SUCCESS;
        }

        default: {
            ZException.setMessage("URFField",
                                  ZS_INVTYPE,
                                  Severity_Error,
                                  "Invalid ZType found %X  record offset %ld",
                                  ZType, pPtrIn - pRecord.Data);
            if (pErrorLog!=nullptr) {
                pErrorLog->logZExceptionLast("URFField::setFromPtr searching new ZType");
            }
            wSt=searchNextValidZType(pRecord,pPtrIn,pErrorLog);
            if (wSt!=ZS_FOUND) {
                ZException.setMessage("URFField",
                                      wSt,
                                      Severity_Error,
                                      "Cannot find a valid ZType till end of record");
                if (pErrorLog!=nullptr) {
                    pErrorLog->logZExceptionLast("URFField::setFromPtr");
                }
                return wSt;
            }
            break;
        } // default
        } // switch
    }// while true

    return ZS_SUCCESS;
} // setFromPtr

ZStatus
URFField::searchNextValidZType(const ZDataBuffer& pRecord,
                               const unsigned char* &pPtr,
                               ZaiErrors* pErrorLog)
{
    ZStatus wSt=ZS_SUCCESS;
    const unsigned char* wPtrSv=pPtr;
    const unsigned char* wPtrEnd = pRecord.Data + pRecord.Size ;

    ZTypeBase wZType = reverseByteOrder_Ptr<ZTypeBase>(pPtr);

    while (!ZTypeExists(wZType) && (pPtr < wPtrEnd)) {
        pPtr++;
        wZType=reverseByteOrder_Ptr<ZTypeBase>(pPtr);
    }

    if (ZTypeExists(wZType)) {
        if (pErrorLog!=nullptr)
            pErrorLog->infoLog("searchNextValidZType Found valid type 0x%X <%s> shifted %ld bytes right at record offset %ld as next valid data type.",
                               wZType,decode_ZType(wZType),pPtr-wPtrSv, pPtr-pRecord.Data);
        else
            _DBGPRINT("searchNextValidZType Found type %X <%s>  shifted %ld bytes right at record offset %ld as next valid data type.\n",
                      wZType,decode_ZType(wZType),pPtr-wPtrSv, pPtr-pRecord.Data);

        return ZS_FOUND;
    }
    if (pErrorLog!=nullptr)
        pErrorLog->infoLog("searchNextValidZType No valid ZType can be found until end of block.");
    else
        _DBGPRINT("searchNextValidZType No valid ZType can be found until end of block.");
    return ZS_NOTFOUND;
}// searchNextValidZType



utf8VaryingString
URFField::display() {
  if (!Present) {
    return "not present";
  }
  ZTypeBase wZType;
  const unsigned char* wPtr=FieldPtr;
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
} //display

URFField& URFField::_copyFrom(const URFField& pIn) {
    Present=pIn.Present;
    FieldPtr=pIn.FieldPtr;
    Size=pIn.Size;
    ZType=pIn.ZType;
    _URFParser = pIn._URFParser;
    HashTime = pIn.HashTime;
    return *this;
}

void URFField::clear()
{
    Present=false;
    FieldPtr=nullptr;
    HashTime.clear() ;
    Size=0;
    ZType=ZType_Nothing;
    _URFParser = nullptr;
    return;
}

utf8VaryingString
URFField::displayFmt(zbs::ZCFMT_Type pCellFormat)
{
    if (!Present) {
        return "not present";
    }
    ZTypeBase wZType;
    const unsigned char* wPtr=FieldPtr;
    utf8VaryingString wStr;
    Size = sizeof(ZTypeBase);
    _importAtomic_KeepPtr<ZTypeBase>(wZType,wPtr);

    if (HashTime!=_URFParser->HashTime) {
        _DBGPRINT("URFField::displayFmt-E-CORRUPT Hashtime values does not match.\n")
    }

    if (wZType!=ZType) {
        _DBGPRINT("URFField::displayFmt-E-INVTYP Effective ZType 0x%X %s does not correspond to parsed ZType 0x%X %s\n",
                  wZType,decode_ZType(wZType),ZType,decode_ZType(ZType))
    }
    /* for atomic URF data, value is just following ZType. For other types, use _importURF function that implies ZType */
    if (wZType & ZType_Atomic) {
         wPtr += sizeof(ZTypeBase);
//        wZType &= ~ZType_Atomic;
    }



    switch (wZType) {
    case ZType_AtomicUChar:
    case ZType_UChar:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
                        wStr = "UChar[";
        }
        uint8_t wValue;
        wValue=convertAtomicBack<uint8_t> (ZType_U8,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%u",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_AtomicU8:
    case ZType_U8:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
                wStr = "U8[";
        }
        uint8_t wValue;
        wValue=convertAtomicBack<uint8_t> (ZType_U8,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%u",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_Char:
    case ZType_AtomicChar:
    {
        int8_t wValue;
        if (pCellFormat & ZCFMT_PrefZType) {
                wStr = "Char[";
        }
        wValue=convertAtomicBack<int8_t> (ZType_S8,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%d",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_S8:

    case ZType_AtomicS8:
    {
        int8_t wValue;
        if (pCellFormat & ZCFMT_PrefZType) {
                wStr = "S8[";
        }
        wValue=convertAtomicBack<int8_t> (ZType_S8,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%d",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_AtomicU16:
    case ZType_U16:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "U16[";
        }
        uint16_t wValue;
        wValue=convertAtomicBack<uint16_t> (ZType_U16,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%u",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_AtomicS16:
    case ZType_S16: {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_S16[";
        }
        int16_t wValue;
        wValue=convertAtomicBack<int16_t> (ZType_S16,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%d",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    case ZType_AtomicU32:
    case ZType_U32:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "U32[";
        }
        uint32_t wValue;
        wValue=convertAtomicBack<uint32_t> (ZType_U32,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%u",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_AtomicS32:
    case ZType_S32:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "S32[";
        }
        int32_t wValue;
        wValue=convertAtomicBack<int32_t> (ZType_S32,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%d",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_AtomicU64:
    case ZType_U64:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "U64[";
        }
        uint64_t wValue;
        wValue=convertAtomicBack<uint64_t> (ZType_U64,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%0llu",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_AtomicS64:
    case ZType_S64:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "S64[";
        }
        int64_t wValue;
        wValue=convertAtomicBack<int64_t> (ZType_S64,wPtr);
        if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
            wStr.addsprintf("0x%X",wValue);
        else
            wStr.addsprintf("%lld",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_AtomicFloat:
    case ZType_Float:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "Float[";
        }
        float wValue;
        wValue=convertAtomicBack<float> (ZType_Float,wPtr);
        wStr.addsprintf("%g",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    case ZType_AtomicDouble:
    case ZType_Double:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "Double[";
        }
        double wValue;
        wValue=convertAtomicBack<double> (ZType_Double,wPtr);
        wStr.addsprintf("%g",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }
    case ZType_AtomicLDouble:
    case ZType_LDouble:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "LDouble[";
        }
        long double wValue;
        wValue=convertAtomicBack<long double> (ZType_LDouble,wPtr);
        wStr.addsprintf("%g",wValue);
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    /* classes */

    case ZType_ZDateFull: {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZDateFull[";
        }
        ssize_t wSize;
        ZDateFull wZDateFull;

        if ((wSize = wZDateFull._importURF(wPtr)) < 0) {
            wStr += "**value**";
        }
        else {
            ZCFMT_Type wFmt = pCellFormat & ZCFMT_DateMask;
            switch (wFmt)
            {
            case ZCFMT_DMY:
                wStr+=wZDateFull.toDMY();
                break;
            case ZCFMT_MDY:
                wStr+=wZDateFull.toMDY();
                break;
            case ZCFMT_MDYHMS:
                wStr+=wZDateFull.toMDYhms();
                break;
            case ZCFMT_DMYHMS:
                wStr+=wZDateFull.toDMYhms();
                break;
            case ZCFMT_DLocale:
                wStr+=wZDateFull.toLocale();
                break;
            case ZCFMT_DUTC:
                wStr+=wZDateFull.toUTCGMT();
                break;
            case ZCFMT_Nothing:
            default:
                wStr+=wZDateFull.toLocale();
                break;
            }
        }// else
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    case ZType_URIString:{
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "URIString[";
        }
        uriString wString;
        ssize_t wSize = wString._importURF(wPtr);
        if (wSize<0) {
            wString += "**errored**";
        }

        if (wString.strlen() > cst_StringDisplayMax){
            wStr += wString.Right(cst_StringDisplayMax).toString();
            wStr += cst_OverflowChar8;
        }
        else if (wString.isEmpty()) {
            wStr += "<empty>";
        }
        else {
            wStr += wString;
        }
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    } // ZType_URIString

    case ZType_Utf8VaryingString:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "Utf8VaryingString[";
        }
        utf8VaryingString wString;
        ssize_t wSize=wString._importURF(wPtr);
        if (wSize<0) {
            wString += "**errored**";
        }
        if (wString.isEmpty()) {
            wStr += "<empty>";
        }
        else {
            if (wString.strlen() > cst_StringDisplayMax){
                wStr += wString.Right(cst_StringDisplayMax).toString();
                wStr += cst_OverflowChar8;
            }
            else {
                wStr += wString;
            }
        }// else
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    } // ZType_Utf8VaryingString

    case ZType_Utf16VaryingString:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "Utf16VaryingString[";
        }
        utf16VaryingString wString;

        ssize_t wSize = wString._importURF(wPtr);
        if (wSize<0) {
            wString += "**errored**";
        }
        if (wString.isEmpty()) {
            wStr += "<empty>";
        }
        else {
            utf8VaryingString wString8;
            wString8.fromUtf16(wString);
            if (wString8.strlen() > cst_StringDisplayMax){
                wStr += wString8.Right(cst_StringDisplayMax).toString();
                wStr += cst_OverflowChar8;
            }
            else {
                wStr += wString8;
            }
        } // else

        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    } // ZType_Utf16VaryingString

    case ZType_Utf32VaryingString:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "Utf32VaryingString[";
        }
        utf32VaryingString wString;

        ssize_t wSize = wString._importURF(wPtr);
        if (wSize<0) {
            wString += "**errored**";
        }

        if (wString.isEmpty()) {
            wStr += "<empty>";
        }
        else {
            utf8VaryingString wString8;
            wString8.fromUtf32(wString);

            if (wString8.strlen() > cst_StringDisplayMax){
                utf8VaryingString w1 = cst_OverflowChar8 ;
                wStr += wString8.Right(cst_StringDisplayMax).toString();
                wStr += cst_OverflowChar8;
            }
            else {
                wStr += wString8;
            }
        } // else

        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    } // ZType_Utf32VaryingString

    case ZType_Utf8FixedString:
    {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "Utf8FixedString[";
        }

        URF_Capacity_type   wCapacity;
        URF_UnitCount_type  wUnitsCount;
        size_t              wStringByteSize;

        wPtr += sizeof(ZTypeBase);

        _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
        _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

        wStringByteSize = size_t (wUnitsCount) * sizeof(utf8_t);
        utf8_t* wPtrOut ;
        bool wSizeOverflow=false;
        if (wUnitsCount > cst_StringDisplayMax) {
            wUnitsCount = cst_StringDisplayMax ;
            wSizeOverflow=true;
            wPtrOut = wStr.extendUnitsBZero(size_t(wUnitsCount+2));
        }
        else
            wPtrOut = wStr.extendUnitsBZero(size_t(wUnitsCount+1));

        URF_Capacity_type wI = wUnitsCount;

        if (pCellFormat & ZCFMT_PrefZType) {
            wPtrOut--;  /* to skip leading '\0' */
        }

//        utf8_t* wPtrOut = (utf8_t*)wStr.Data;
        utf8_t* wPtrIn = (utf8_t*)wPtr;
        while (wI--&& *wPtrIn )
            *wPtrOut++ = *wPtrIn++;
        if (wSizeOverflow)
            wStr += cst_OverflowChar8 ;
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    } // ZType_Utf8FixedString

        /* for fixed string URF header is different */

    case ZType_Utf16FixedString:{
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_Utf16FixedString[";
        }
        utf16VaryingString wString;

        URF_Capacity_type wCapacity;
        URF_UnitCount_type  wUnitsCount;


        wPtr += sizeof(ZTypeBase);

        _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
        _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

        size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf16_t);

        /* the whole string must be imported, then possibly truncated afterwards to maximum displayable */

        utf16_t* wPtrOut ;
        URF_Capacity_type wI = wUnitsCount;
        bool wSizeOverflow=false;
        if (wUnitsCount > cst_StringDisplayMax) {
            wUnitsCount = cst_StringDisplayMax;
            wSizeOverflow=true;
        }

        wPtrOut = wString.allocateUnitsBZero(size_t(wUnitsCount+1));

        utf16_t* wPtrIn = (utf16_t*)wPtr;
        while ( wI-- && *wPtrIn )
            *wPtrOut++ = *wPtrIn++;

        wPtr = (unsigned char*) wPtrIn;

        utf8VaryingString wString8;

        wString8.fromUtf16(wString);
        if (wSizeOverflow)
            wString8 += cst_OverflowChar8 ;

        wString += wString8;

        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    } // ZType_Utf16FixedString

    case ZType_Utf32FixedString:{
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_Utf32FixedString[";
        }
        utf32VaryingString wString;
        URF_Capacity_type wCapacity;
        URF_UnitCount_type  wUnitsCount;

        wPtr += sizeof(ZTypeBase);

        _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
        _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

        size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf32_t);

        URF_Capacity_type wI = wUnitsCount;
        bool wSizeOverflow=false;
        if (wUnitsCount > cst_StringDisplayMax) {
            wUnitsCount = cst_StringDisplayMax;
            wSizeOverflow=true;
        }

        wString.allocateUnitsBZero(size_t(wUnitsCount+1));

        utf32_t* wPtrOut = (utf32_t*)wString.Data;
        utf32_t* wPtrIn = (utf32_t*)wPtr;

        while (wI--&& *wPtrIn )
            *wPtrOut++ = *wPtrIn++;

        utf8VaryingString wString8;

        wString8.fromUtf32(wString);
        if (wSizeOverflow)
            wString8 += cst_OverflowChar8 ;
        wStr=wString8;
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    } // ZType_Utf32FixedString

    case ZType_CheckSum: {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_CheckSum[";
        }
        checkSum wCheckSum;
        wCheckSum._importURF(wPtr);
        wStr += wCheckSum.toHexa();
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    case ZType_MD5: {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_MD5[";
        }
        md5 wCheckSum;
        wCheckSum._importURF(wPtr);
        wStr += wCheckSum.toHexa();
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    case ZType_Blob: {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_Blob[";
        }
        if (pCellFormat & ZCFMT_DumpBlob) {
            ZDataBuffer wZDB;
            if (wZDB._importURF(wPtr)<0) {
                wStr += "invalid URF blob";
            }
            else {
                wStr += wZDB.simpleDump(false,cst_StringDisplayMax/2);
            }
        } // ZCFMT_DumpBlob
        else
        {
            uint64_t wDataSize;
            wPtr += sizeof(ZTypeBase);
            _importAtomic(wDataSize,wPtr);
            wStr.addsprintf("<blob %lld bytes>",wDataSize);
        }
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    case ZType_bitset: {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_bitset[";
        }
        ZBitset wBitset;

        ssize_t wSize=wBitset._importURF(wPtr);
        wStr += "<";
        wStr += wBitset.toString();
        wStr += ">";
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    case ZType_bitsetFull: {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_bitsetFull[";
        }
        wStr += "<bitset full>";
        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    }

    case ZType_Resource: {
        if (pCellFormat & ZCFMT_PrefZType) {
            wStr = "ZType_Resource[";
        }
        ZResource wValue;
        ssize_t wSize=wValue._importURF(wPtr);
        if (!wValue.isValid()) {
            wStr += "<Invalid resource>";
        }
        else {
            ZCFMT_Type wFmt = pCellFormat & ZCFMT_ResMask;
            if (wFmt & ZCFMT_ResSymb) {
                utf8VaryingString  wZEntitySymbol ;

                int wi=0;
                for (; wi < ZEntitySymbolList.count();wi++) {
                    if (ZEntitySymbolList[wi].Value == wValue.Entity)
                        break;
                }

                if (wi < ZEntitySymbolList.count())
                    wZEntitySymbol = ZEntitySymbolList[wi].Symbol;
                else
                    wZEntitySymbol.addsprintf("%X",int(wValue.Entity));

                if (wFmt & ZCFMT_ResStd) {
                    wStr.addsprintf("[%s,%ld]",wZEntitySymbol.toCChar(),wValue.id);
                }
                else {
                    wStr.addsprintf("[%s,0x%X]",wZEntitySymbol.toCChar(),wValue.id);
                }
            } // ZCFMT_ResSymb
            else {
                if (wFmt & ZCFMT_ResStd) {
                    wStr.addsprintf("[%d,%ld]",wValue.Entity,wValue.id);
                }
                else {
                    wStr.addsprintf("[0x%X,0x%X]",wValue.Entity,wValue.id);
                } //ZCFMT_ResStd
            }// else
        }

        if (pCellFormat & ZCFMT_PrefZType)
            wStr += "]";
        return wStr;
    } //ZType_Resource

    default: {
        wStr.sprintf ("Unknown data type <%d> <%X>",wZType,wZType);
        return wStr;
    }

    }// switch

    return wStr;
} //displayFmt




utf8VaryingString
URFField::stdDisplay() {
  if (!Present) {
    return "not present";
  }
  ZTypeBase wZType;
  const unsigned char* wPtr=FieldPtr;
  utf8VaryingString wReturn;

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
    wReturn.sprintf("%u",wValue);
    return wReturn;
  }
  case ZType_Char:
  case ZType_S8: {
    int8_t wValue;
    wValue=convertAtomicBack<int8_t> (ZType_S8,wPtr);
    wReturn.sprintf("%d",wValue);
    return wReturn;
  }
  case ZType_U16:{
    uint16_t wValue;
    wValue=convertAtomicBack<uint16_t> (ZType_U16,wPtr);
    wReturn.sprintf("%u",wValue);
    return wReturn;
  }
  case ZType_S16: {
    int16_t wValue;
    wValue=convertAtomicBack<int16_t> (ZType_S16,wPtr);
    wReturn.sprintf("%d",wValue);
    return wReturn;
  }

  case ZType_U32:{
    uint32_t wValue;
    wValue=convertAtomicBack<uint32_t> (ZType_U32,wPtr);
    wReturn.sprintf("%u",wValue);
    return wReturn;
  }
  case ZType_S32: {
    int32_t wValue;
    wValue=convertAtomicBack<int32_t> (ZType_S32,wPtr);
    wReturn.sprintf("%d",wValue);
    return wReturn;
  }
  case ZType_U64: {
    uint64_t wValue;
    wValue=convertAtomicBack<uint64_t> (ZType_U64,wPtr);
    wReturn.sprintf("%llu",wValue);
    return wReturn;
  }
  case ZType_S64: {
    int64_t wValue;
    wValue=convertAtomicBack<int64_t> (ZType_S64,wPtr);
    wReturn.sprintf("%lld",wValue);
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
    ZDataBuffer wBlob;
    uint64_t wDataSize;
    wPtr += sizeof(ZTypeBase);
    _importAtomic(wDataSize,wPtr);
    wBlob.setData(FieldPtr,wDataSize);
    wBlob.encryptB64();
    wReturn=(utf8_t*)wBlob.Data;
    wPtr += size_t(wDataSize);
    return wReturn;
    break;
  }

  case ZType_bitset: {
    ZBitset wBitset;

    ssize_t wSize=wBitset._importURF(wPtr);
    wReturn = wBitset.toString();
    return wReturn;
  }

  case ZType_bitsetFull: {
    return "bitsetfull";
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

utf8VaryingString
URFField::toXml(int pLevel)
{
  if (!Present)
     return fmtXMLcomment(" Missing field (Presence flag set to false) ",pLevel);

  utf8VaryingString wXmlString , wXmlComment;
  wXmlString = fmtXMLZType("ztype",ZType,pLevel);
  wXmlComment.sprintf(" integer value of ZType <%s> ",decode_ZType(ZType));
  fmtXMLaddInlineComment(wXmlString,wXmlComment);


  ZTypeBase wZType = ZType ;
  if (ZType & ZType_Atomic )
     wZType = ZType & ~ ZType_Atomic ;
  const unsigned char* wPtrAtomic = FieldPtr + sizeof(ZTypeBase); /* for atomic data */
  const unsigned char* wPtr = FieldPtr ; /* for classes and others : need to point to ZTypeBase */

  switch (wZType)
  {
  case ZType_UChar:
  case ZType_U8: {
    uint8_t wValue;
    wValue=convertAtomicBack<uint8_t> (ZType_U8,wPtrAtomic);
    wXmlString += fmtXMLuint8("content",uint32_t(wValue),pLevel);
    break;
  }
  case ZType_Char:
  case ZType_S8: {
    int8_t wValue;
    wValue=convertAtomicBack<int8_t> (ZType_S8,wPtrAtomic);
    wXmlString += fmtXMLint8("content",int32_t(wValue),pLevel);
    break;
  }
  case ZType_U16:{
    uint16_t wValue;
    wValue=convertAtomicBack<uint16_t> (ZType_U16,wPtrAtomic);
    wXmlString += fmtXMLuint16("content",(unsigned int)(wValue),pLevel);
    break;
  }
  case ZType_S16: {
    int16_t wValue;
    wValue=convertAtomicBack<int16_t> (ZType_S16,wPtrAtomic);
    wXmlString += fmtXMLint16("content",int(wValue),pLevel);
    break;
  }

  case ZType_U32:{
    uint32_t wValue;
    wValue=convertAtomicBack<uint32_t> (ZType_U32,wPtrAtomic);
    wXmlString += fmtXMLuint32("content",wValue,pLevel);
    break;
  }
  case ZType_S32: {
    int32_t wValue;
    wValue=convertAtomicBack<int32_t> (ZType_S32,wPtrAtomic);
    wXmlString += fmtXMLint32("content",wValue,pLevel);
    break;
  }
  case ZType_U64: {
    uint64_t wValue;
    wValue=convertAtomicBack<uint64_t> (ZType_U64,wPtrAtomic);
    wXmlString += fmtXMLuint64("content",wValue,pLevel);
    break;
  }
  case ZType_S64: {
    int64_t wValue;
    wValue=convertAtomicBack<int64_t> (ZType_S64,wPtrAtomic);
    wXmlString += fmtXMLint64("content",wValue,pLevel);
    break;
  }
  case ZType_Float: {
    float wValue;
    wValue=convertAtomicBack<float> (ZType_Float,wPtrAtomic);
    wXmlString += fmtXMLfloat("content",wValue,pLevel);
    break;
  }

  case ZType_Double: {
    double wValue;
    wValue=convertAtomicBack<double> (ZType_Double,wPtrAtomic);
    wXmlString += fmtXMLdouble("content",wValue,pLevel);
    break;
  }

  case ZType_LDouble: {
    long double wValue;
    wValue=convertAtomicBack<long double> (ZType_LDouble,wPtrAtomic);
    wXmlString += fmtXMLdouble("content",double(wValue),pLevel);
    break;
  }

  case ZType_ZDateFull: {
    ssize_t wSize;
    ZDateFull wZDateFull;
    wZDateFull._importURF(wPtr);
    wXmlString += fmtXMLdatefull("content",wZDateFull,pLevel);
    break;
  }

  case ZType_URIString:{
    uriString wString;
    ssize_t wSize = wString._importURF(wPtr);
    wXmlString += fmtXMLchar("content",wString.toCChar(),pLevel);
    break;

  }
  case ZType_Utf8VaryingString: {
    utf8VaryingString wString;
    ssize_t wSize = wString._importURF(wPtr);
    wXmlString += fmtXMLchar("content",wString.toCChar(),pLevel);
    break;
  }

  case ZType_Utf16VaryingString:{
    utf16VaryingString wString ;
    utf8VaryingString wString1 ;

    ssize_t wSize = wString._importURF(wPtr);

    wString1.fromUtf16(wString);
    wXmlString += fmtXMLchar("content",wString1.toCChar(),pLevel);
    break;
  }
  case ZType_Utf32VaryingString:{
    utf32VaryingString wString ;
    utf8VaryingString wString1 ;
    ssize_t wSize = wString._importURF(wPtr);
    wString1.fromUtf32(wString);
    wXmlString += fmtXMLchar("content",wString1.toCChar(),pLevel);
    break;
  }

  case ZType_Utf8FixedString:{

    utf8VaryingString wString  ;

    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wStringByteSize;

    _importAtomic<URF_Capacity_type>(wCapacity,wPtrAtomic);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtrAtomic);

    wStringByteSize = size_t (wUnitsCount) * sizeof(utf8_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf8_t* wPtrOut = (utf8_t*)wString.Data;
    utf8_t* wPtrIn = (utf8_t*)wPtrAtomic;
    while ((wI-->0) && *wPtrIn )
      *wPtrOut++ = *wPtrIn++;
    wXmlString += fmtXMLuint("capacity",wCapacity,pLevel);
    wXmlString += fmtXMLchar("content",wString.toCChar(),pLevel);
    break;
  }

    /* for fixed string URF header is different */

  case ZType_Utf16FixedString:{
    utf16VaryingString wString;
    utf8VaryingString wString1 ;

    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;

 //   wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtrAtomic);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtrAtomic);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf16_t);

    /* the whole string must be imported, then possibly truncated afterwards to maximum displayable */

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf16_t* wPtrOut = (utf16_t*)wString.Data;
    utf16_t* wPtrIn = (utf16_t*)wPtrAtomic;
    while ( wI-- && *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wString1.fromUtf16(wString);
    wXmlString += fmtXMLuint("capacity",wCapacity,pLevel);
    wXmlString += fmtXMLchar("content",wString1.toCChar(),pLevel);
    break;
  }

  case ZType_Utf32FixedString:{
    utf32VaryingString wString;
    utf8VaryingString wString1 ;
    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;

    wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtrAtomic);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtrAtomic);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf32_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf32_t* wPtrOut = (utf32_t*)wString.Data;
    utf32_t* wPtrIn = (utf32_t*)wPtrAtomic;

    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wPtr = (unsigned char*) wPtrIn;

    wString1.fromUtf32(wString);
    wXmlString += fmtXMLuint("capacity",wCapacity,pLevel);
    wXmlString += fmtXMLchar("content",wString1.toCChar(),pLevel);
    break;
  }

  case ZType_CheckSum: {
    checkSum wCheckSum;
    wCheckSum._importURF(wPtr);
    wXmlString += fmtXMLcheckSum("content",wCheckSum,pLevel);
    break;
  }

  case ZType_MD5: {
    md5 wCheckSum;
    wCheckSum._importURF(wPtr);
    wXmlString += fmtXMLmd5("content",wCheckSum,pLevel);
    break;
  }



  case ZType_bitsetFull:
  case ZType_bitset: {
    fprintf(stderr,"exportContent-F-INVTYP ZType_bitset data type cannot be exported\n");
    ZBitset wBitset;
    ssize_t wSize=wBitset._importURF(wPtrAtomic);
    wXmlString += fmtXMLchar("content",wBitset.toString(),pLevel);
    fmtXMLaddInlineComment(wXmlString," exportContent-F-INVTYP ZType_bitset data type cannot be exported ");
    break;
  }
/*
  case ZType_bitsetFull: {
    wXmlString = fmtXMLchar("content","",pLevel);
    break;
  }
*/
  case ZType_Resource: {
    ZResource wValue;
    ssize_t wSize=wValue._importURF(wPtr);
    wXmlString += fmtXMLnode("content",pLevel);
    wXmlString += wValue.toXml(pLevel+1);
    wXmlString += fmtXMLendnode("content",pLevel);
    break;
  }
  case ZType_Blob: {
    ZDataBuffer wBlob;
    wBlob._importURF(wPtr);
    wXmlString += fmtXMLBlob("content",wBlob,pLevel);
/*
    uint64_t wDataSize;
    utf8VaryingString wBlobContent;
    //    wPtrAtomic += sizeof(ZTypeBase);
    wDataSize=convertAtomicBack<uint64_t>(ZType_U64,wPtrAtomic);
    wBlob.setData(wPtrAtomic,wDataSize);
    wBlob.encryptB64();
    wXmlString += "<content>";
    wXmlString.nadd((utf8_t*)wBlob.Data,wBlob.Size);
    wXmlString += fmtXMLendnode("content",0);
    break;
*/
  }
  default: {
    fprintf(stderr,"exportContent-F-INVTYP Unknown data type <%d> <%X>\n",wZType,wZType);
    wXmlComment.sprintf("Unknown/unmanaged data type <%d> <%X>",wZType,wZType);
    wXmlString += fmtXMLcomment(wXmlComment,pLevel);
    break;
  }
  }// switch

  return wXmlString;
} // toXml



ZStatus
URFField::fromXml(zxmlElement* pFieldNode,ZDataBuffer& pURFContent,ZaiErrors* pErrorLog)
{
  ZType = 0;

  ZTypeBase wZType=0,wZTypeB=0;
  ZStatus wSt=XMLgetChildZType(pFieldNode,"ztype",ZType,pErrorLog,ZAIES_Info);
  if (wSt!=ZS_SUCCESS) {
    Present = false;
    FieldPtr = nullptr;
    Size = 0 ;
    return ZS_SUCCESS;
  }

  Present = true;
  wZTypeB = ZType;
  if (wZTypeB & ZType_Atomic)
    wZTypeB = ZType & ~ ZType_Atomic;
  switch (wZTypeB)
  {
  case ZType_UChar:
  case ZType_U8: {
    unsigned int wValue;
    wSt=XMLgetChildUInt(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    uint8_t wValue8 = (uint8_t)wValue;
    getURFfromAtomicValue<uint8_t>(wValue8,pURFContent);
    return ZS_SUCCESS;
  }
  case ZType_Char:
  case ZType_S8: {
    int wValue;
    wSt=XMLgetChildInt(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<int8_t>(int8_t(wValue),pURFContent);
    return ZS_SUCCESS;
  }
  case ZType_U16:{
    unsigned int wValue;
    wSt=XMLgetChildUInt(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<uint16_t>(uint16_t(wValue),pURFContent);
    return ZS_SUCCESS;
  }
  case ZType_S16: {
    int wValue;
    wSt=XMLgetChildInt(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<int16_t>(int16_t(wValue),pURFContent);
    return ZS_SUCCESS;
  }

  case ZType_U32:{
    unsigned int wValue;
    wSt=XMLgetChildUInt(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<uint32_t>(uint32_t(wValue),pURFContent);
    return ZS_SUCCESS;
  }
  case ZType_S32: {
    int wValue;
    wSt=XMLgetChildInt(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<int32_t>(int32_t(wValue),pURFContent);
    return ZS_SUCCESS;
  }
  case ZType_U64: {
    uint64_t wValue;
    wSt=XMLgetChildUInt64(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<uint64_t>(wValue,pURFContent);
    return ZS_SUCCESS;
  }
  case ZType_S64: {
    int64_t wValue;
    wSt=XMLgetChildInt64(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<int64_t>(wValue,pURFContent);
    return ZS_SUCCESS;
  }
  case ZType_Float: {
    float wValue;
    wSt=XMLgetChildFloat(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<float>(wValue,pURFContent);
    return ZS_SUCCESS;
  }

  case ZType_Double: {
    double wValue;
    wSt=XMLgetChildDouble(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<double>(wValue,pURFContent);
    return ZS_SUCCESS;
  }

  case ZType_LDouble: {
    double wValue;
    wSt=XMLgetChildDouble(pFieldNode,"content",wValue,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    getURFfromAtomicValue<long double>((long double)(wValue),pURFContent);
    return ZS_SUCCESS;
  }

  case ZType_ZDateFull: {
    ssize_t wSize;
    ZDateFull wZDateFull;
    wSt=XMLgetChildZDateFull(pFieldNode,"content",wZDateFull,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    ssize_t wS=wZDateFull._exportURF(pURFContent);
    if (wS>0)
      return ZS_SUCCESS ;
    return ZS_ERROR;
  }

  case ZType_URIString:{
    uriString wString;
    wSt=XMLgetChildText(pFieldNode,"content",wString,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    ssize_t wS=wString._exportURF(pURFContent);
    if (wS<0)
      return ZS_ERROR;
    return ZS_SUCCESS ;
  }
  case ZType_Utf8VaryingString: {
    utf8VaryingString wString;
    wSt=XMLgetChildText(pFieldNode,"content",wString,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    ssize_t wS=wString._exportURF(pURFContent);
    if (wS<0)
      return ZS_ERROR;
    return ZS_SUCCESS ;
  }

  case ZType_Utf16VaryingString:{
    utf16VaryingString wString ;
    utf8VaryingString wString1 ;
    wSt=XMLgetChildText(pFieldNode,"content",wString1,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wString.fromUtf8(wString1.toUtf());
    ssize_t wS=wString._exportURF(pURFContent);
    if (wS<0)
      return ZS_ERROR;
    return ZS_SUCCESS ;
  }
  case ZType_Utf32VaryingString:{
    utf32VaryingString wString ;
    utf8VaryingString wString1 ;
    wSt=XMLgetChildText(pFieldNode,"content",wString1,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wString.fromUtf8(wString1.toUtf());
    ssize_t wS=wString._exportURF(pURFContent);
    if (wS<0)
      return ZS_ERROR;
    return ZS_SUCCESS ;
  }

  case ZType_Utf8FixedString:{

/* emulates an URF Fixed string
 *
 *  ZTypeBase           ZType
 *  URF_Capacity_type   Capacity
 *  URF_UnitCount_type  Unit Count
 *  unsigned char*      data
 */
    utf8VaryingString wString  ;
    unsigned int wCap1=0;
    URF_Capacity_type wCapacity;
    wSt=XMLgetChildUInt(pFieldNode,"capacity",wCap1,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wCapacity = URF_Capacity_type(wCap1);
    wSt=XMLgetChildText(pFieldNode,"content",wString,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    Size= sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + (sizeof(uint8_t)*wString.getUnitCount()) ;
    pURFContent.allocateBZero(Size);
    FieldPtr = pURFContent.Data;
    unsigned char* wPtrOut = pURFContent.Data;
    ZTypeBase* wPtrTB = (ZTypeBase*)wPtrOut;
    *wPtrTB = reverseByteOrder_Conditional<ZTypeBase>(ZType) ;
    wPtrOut += sizeof(ZTypeBase);
    URF_Capacity_type* wPtrCap = (URF_Capacity_type*)wPtrOut;
    *wPtrCap = reverseByteOrder_Conditional<URF_Capacity_type>(wCapacity) ;
    wPtrOut += sizeof(URF_Capacity_type);
    URF_UnitCount_type* wPtrUnit = (URF_UnitCount_type*)wPtrOut;
    URF_UnitCount_type wCount = URF_UnitCount_type(wString.getUnitCount());
    *wPtrUnit = reverseByteOrder_Conditional<URF_UnitCount_type>(wCount) ;
    wPtrOut += sizeof(URF_UnitCount_type);

    const unsigned char* wPtrIn = wString.Data;

    while (wCount-- > 0) {
      *wPtrOut++ = *wPtrIn++;
    }
    return ZS_SUCCESS ;
  } //ZType_Utf8FixedString

    /* for fixed string URF header is different */

  case ZType_Utf16FixedString:{
    utf8VaryingString wString  ;
    utf16VaryingString wString16;
    unsigned int wCap1=0;
    URF_Capacity_type wCapacity;
    wSt=XMLgetChildUInt(pFieldNode,"capacity",wCap1,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wCapacity = URF_Capacity_type(wCap1);
    wSt=XMLgetChildText(pFieldNode,"content",wString,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wString16.fromUtf8(wString.toUtf());

    Size= sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + (sizeof(uint16_t)*wString.getUnitCount()) ;
    pURFContent.allocateBZero(Size);
    FieldPtr = pURFContent.Data;
    unsigned char* wPtrOut = pURFContent.Data;
    ZTypeBase* wPtrTB = (ZTypeBase*)wPtrOut;
    *wPtrTB = reverseByteOrder_Conditional<ZTypeBase>(ZType) ;
    wPtrOut += sizeof(ZTypeBase);
    URF_Capacity_type* wPtrCap = (URF_Capacity_type*)wPtrOut;
    *wPtrCap = reverseByteOrder_Conditional<URF_Capacity_type>(wCapacity) ;
    wPtrOut += sizeof(URF_Capacity_type);
    URF_UnitCount_type* wPtrUnit = (URF_UnitCount_type*)wPtrOut;
    URF_UnitCount_type wCount = URF_UnitCount_type(wString.getUnitCount());
    *wPtrUnit = reverseByteOrder_Conditional<URF_UnitCount_type>(wCount) ;
    wPtrOut += sizeof(URF_UnitCount_type);

    utf16_t* wPtr16 = (utf16_t*)wPtrOut;

    utf16_t* wPtrIn = wString16.Data;

    while (wCount-- > 0) {
      *wPtr16++ = reverseByteOrder_Conditional<utf16_t>(*wPtrIn++);;
    }
    return ZS_SUCCESS ;
  } // ZType_Utf16FixedString

  case ZType_Utf32FixedString:
  {
    utf8VaryingString wString  ;
    utf32VaryingString wString32;
    unsigned int wCap1=0;
    URF_Capacity_type wCapacity;
    wSt=XMLgetChildUInt(pFieldNode,"capacity",wCap1,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wCapacity = URF_Capacity_type(wCap1);
    wSt=XMLgetChildText(pFieldNode,"content",wString,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wString32.fromUtf8(wString.toUtf());

    Size= sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + (sizeof(uint32_t)*wString.getUnitCount()) ;
    pURFContent.allocateBZero(Size);
    FieldPtr = pURFContent.Data;
    unsigned char* wPtrOut = pURFContent.Data;
    ZTypeBase* wPtrTB = (ZTypeBase*)wPtrOut;
    *wPtrTB = reverseByteOrder_Conditional<ZTypeBase>(ZType) ;
    wPtrOut += sizeof(ZTypeBase);
    URF_Capacity_type* wPtrCap = (URF_Capacity_type*)wPtrOut;
    *wPtrCap = reverseByteOrder_Conditional<URF_Capacity_type>(wCapacity) ;
    wPtrOut += sizeof(URF_Capacity_type);
    URF_UnitCount_type* wPtrUnit = (URF_UnitCount_type*)wPtrOut;
    URF_UnitCount_type wCount = URF_UnitCount_type(wString.getUnitCount());
    *wPtrUnit = reverseByteOrder_Conditional<URF_UnitCount_type>(wCount) ;
    wPtrOut += sizeof(URF_UnitCount_type);

    utf32_t* wPtr32 = (utf32_t*)wPtrOut;

    utf32_t* wPtrIn = wString32.Data;

    while (wCount-- > 0) {
      *wPtr32++ = reverseByteOrder_Conditional<utf32_t>(*wPtrIn++);;
    }
    return ZS_SUCCESS ;
  } // ZType_Utf32FixedString

  case ZType_CheckSum:
  {
    checkSum wCheckSum;
    wSt=XMLgetChildCheckSum(pFieldNode,"content",wCheckSum,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    wCheckSum._exportURF(pURFContent);
    return ZS_SUCCESS ;
  } // ZType_CheckSum

  case ZType_MD5:
  {
    md5 wCheckSum;
    wSt=XMLgetChildMd5(pFieldNode,"content",wCheckSum,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    wCheckSum._exportURF(pURFContent);
    return ZS_SUCCESS ;
  } // ZType_MD5

  case ZType_bitsetFull:
  case ZType_bitset: {
    utf8VaryingString wBitsetContent;
    pErrorLog->warningLog("exportContent-F-INVTYP ZType_bitset data type cannot be imported.");
    ZBitset wBitset;
    wSt=XMLgetChildText(pFieldNode,"content",wBitsetContent,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    utf8_t* wPtr=wBitsetContent.Data;
    wBitset._allocate(wBitsetContent.getByteSize());
    wBitset.clear();
    size_t wI=0;
    for (size_t wi=0; (wi < wBitsetContent.ByteSize) && (wPtr[wi]!=0) ; wi ++) {
      if (wPtr[wi]=='1')
        wBitset.set(wi);
    }
    return ZS_SUCCESS;
  } // ZType_bitset ZType_bitsetFull

  case ZType_Resource: {
     ZResource wValue;
    ZStatus wSt= wValue.fromXml(pFieldNode,"content",pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wValue._exportURF(pURFContent);
    return ZS_SUCCESS;
  }

  case ZType_Blob:
  {
    ZDataBuffer wBlob;
    wSt=XMLgetChildBlob(pFieldNode,"content",wBlob,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wBlob._exportURF(pURFContent);
    return ZS_SUCCESS ;
    /*
    uint64_t wDataSize;
    utf8VaryingString wBlobContent;

    wSt=XMLgetChildUInt64(pFieldNode,"size",wDataSize,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    wSt=XMLgetChildText(pFieldNode,"content",wBlobContent,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    ZDataBuffer wBlobZDB;
    wBlobZDB.setData(wBlobContent.Data,wBlobContent.ByteSize);
    wBlobZDB.uncryptB64();

    pURFContent.allocateBZero(sizeof(ZTypeBase) + sizeof(uint64_t) );

    unsigned char* wPtr = pURFContent.Data ;

    _exportAtomicPtr<ZTypeBase>(ZType,wPtr);
    _exportAtomicPtr<uint64_t>(wDataSize,wPtr);

    pURFContent.appendData(wBlobZDB);
    return ZS_SUCCESS ;
*/
  }

  default: {
    fprintf(stderr,"importContent-F-INVTYPE Unknown data type <%d> <%X>\n",ZType,ZType);
    pErrorLog->errorLog("importContent-F-INVTYPE Unknown/unmanaged data type <%d> <%X>",ZType,ZType);
    return ZS_INVTYPE;
  }
  }// switch

  return ZS_INVTYPE;
}


URFField getURFField(const ZDataBuffer& pRecord,
                     const unsigned char* &pPtrIn,
                     ZaiErrors* pErrorLog)
{
    URFField wReturn;
    ZStatus wSt=wReturn.setFromPtr(pRecord,pPtrIn,pErrorLog);
    if (wSt==ZS_NOTFOUND)
        return URFField();
    return wReturn;
}
