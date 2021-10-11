#ifndef ZRECORD_CPP
#define ZRECORD_CPP
#include <stdio.h>
#include <zindexedfile/zrecord.h>
#include <ztoolset/zexceptionmin.h>
#include <zindexedfile/zdatatype.h>
#include <ztoolset/utfsprintf.h>

#include <ztoolset/utfstringcommon.h> // for routines setURFxxx getURFxxx

#include <zindexedfile/zsmasterfile.h> // for ZSMasterControlBlock

#include <zindexedfile/zmfdictionary.h>

/** @addtogroup ZSMASTERFILEGROUP @{ */


using namespace  zbs;

ZRecordDic::ZRecordDic(ZMFDictionary *pMasterDic)
{
recordFieldDesc_struct wField;
  MasterDic=pMasterDic;

  if (pMasterDic==nullptr)
      {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADCHECKSUM,
                              Severity_Fatal,
                              "Given Meta Dictionary is nullptr while initializing a ZRecord Dictionary.");
        ZException.exit_abort();
      }
  if (pMasterDic->CheckSum==nullptr)
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_BADCHECKSUM,
                            Severity_Fatal,
                            " Meta Dictionary checksum is nullptr while initializing a ZRecord Dictionary.");
      ZException.exit_abort();
    }
  CheckSum=new checkSum;
  memmove (CheckSum->content ,pMasterDic->CheckSum->content,sizeof(CheckSum->content));
  for (long wi=0;wi <pMasterDic->size();wi++)
        {
        wField.clear();
        wField.MDicRank=&pMasterDic->Tab[wi];
        push(wField);
        }


}// ZRecordDic

ZRecordDic::~ZRecordDic()
{
while (size()>0)
            {
            if (last().URFData!=nullptr)
                            delete last().URFData;
            pop();
            }
if (CheckSum!=nullptr)
            delete CheckSum;
}// ~ZRecordDic

long
ZRecordDic::getFieldByName(const char*pName) const
{
    for(long wi=0;wi<size();wi++)
        {
      if (Tab[wi].MDicRank->hasName(pName))
                      return wi;
        }
    return -1;
}//getFieldByName


void
ZRecordDic::reset(void)
{
    for (long wi=0;wi<size();wi++)
        {
        if (Tab[wi].URFData!=nullptr)
                {
                delete Tab[wi].URFData;
                Tab[wi].URFData=nullptr;
                }
        }
}// reset

ZRecord::ZRecord(ZSMasterFile *pZMF) : ZRawRecord(pZMF)
{
  if (pZMF->ZMCB.MasterDic==nullptr)
    {
    ZException.setMessage("ZRecord::ZRecord",
          ZS_BADDIC,
        Severity_Fatal,
        "Usage of ZRecord is conditionned to Master dictionary existence.");
    ZException.exit_abort();
    }
  ZRawRecord::setup();
  RDic=new ZRecordDic(pZMF->ZMCB.MasterDic);
  RDic->reset();
  MetaDicCheckSum = pZMF->ZMCB.MasterDic->getCheckSum();
}
ZRecord::~ZRecord()
{
    if (FieldPresence!=nullptr)
                    delete FieldPresence;
    if (RDic!=nullptr)
                    delete RDic;
    if (MetaDicCheckSum!=nullptr)
                    delete MetaDicCheckSum;
}

/**
 * @brief _setupRecordData extract data type (ZType_type)
 *  extract data actual size if necessary
 *
 *  - atomic types : universal size is deduced from type
 *   NB: size is atomic natural size in bytes + 1 byte if signed
 *   NB2: for atomic data type
 *
 *  - blob : size is stored on a uint64_t (unsigned) size is stored as atomic universal size (Endian free)
 *  - String array : size is stored on uint32_t  (unsigned) size is stored as atomic universal size(Endian free)
 *  - WCHAR string array : effective size in bytes is stored on uint32_t.
 *      Effective size is NOT the number of characters (that will take 2 bytes) but the size in bytes.
 *      Number of characters is deduced from effective size / 2 (size of WCHAR).
@verbatim

 Storage format for data file content


 - atomic :
    Type  (uint32_t) Data (size = size of atomic data type)
    xxxx......
    012345---n

    In addition, atomic types are subject to endian conversion to have a universal representation of data.


 - blob :
     +----------------------------------> Type (uint32_t)
     |     +----------------------------> Size (uint64_t)
     |     |             +--------------> Data (....)
    ----___|____ ........|.......
    01234567890123--------------n
    0         1

 - Other structures :
     +----------------------------------> Type (uint32_t)
     |   +----------------------------> Size (uint32_t)
     |   |           +--------------> Data (....)
    ----_|__ ........|.......
    01234567890123--------------n
    0         1

@endverbatim

* @param pMetaDic
 * @param pZDB
 * @param pOffsets
 */
void
ZRecord::_setupRecordData(void)
{
/*
    if (MCB==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Fatal,
                              "Reference to Master Control Block has not been set while accessing ZRecord");
        ZException.exit_abort();
        }
        */
  if (RawMasterFile->ZMCB.MasterDic==nullptr)    /* ZRecord usage implies that meta dictionary has been defined */
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_INVOP,
          Severity_Fatal,
          "No meta dictionary has been defined for this file while accessing ZRecord.Use ZRawRecord instead.");
      ZException.exit_abort();
    }

  if (RDic==nullptr)
        {
        RDic=new ZRecordDic(RawMasterFile->ZMCB.MasterDic);
        }
    Rank=0;
    
    uint64_t wOffset=0;
    ssize_t wFieldRank =0;
    uint16_t wCanonicalCount ;
    uint64_t wTotalRecordSize=0;
    uint64_t wSizeLL;
    uint16_t wSizeW;
    unsigned char* wPtr =nullptr;
    ZTypeBase wTypeStructure;

    RDic->TotalRecordSize=Content.Size;

    if (FieldPresence!=nullptr)
                delete FieldPresence;
    FieldPresence=new ZBitset();
    wPtr=FieldPresence->_importURF(Content.Data);

    wOffset = FieldPresence->getExportSize();
// get fields (that are present within the record) and setup record dictionary
    ZTypeBase wType;

    // Nota Bene : if fields have been added, FieldPresence is not extended,
    //              So additional fields will be considered as missing (upper limit is FieldPresence->CurrentBitSize)
    while ((wFieldRank<RDic->size())&&(wOffset< Content.Size )&&(wFieldRank < (ssize_t)FieldPresence->EffectiveBitSize))
        {
        if (!FieldPresence->test(wFieldRank))
                        {
                        wFieldRank++;
                        continue ;
                        }
        RDic->Tab[wFieldRank].URFOffset=wOffset;

        wPtr=(Content.Data + wOffset);

        memmove(&wType,wPtr,sizeof(ZTypeBase));
        wType=reverseByteOrder_Conditional<ZTypeBase>(wType);


        wPtr+= sizeof(ZTypeBase);
        while (true)
        {
        if (wType==ZType_FixedCString)
            {
            uint16_t wCanonSize;
            uint16_t wUSize;
            memmove(&wCanonSize,wPtr,sizeof(wCanonSize));
            wCanonSize=reverseByteOrder_Conditional<uint16_t>(wCanonSize);
            wPtr += sizeof(uint16_t);
            memmove(&wUSize,wPtr,sizeof(wUSize));
            wUSize=reverseByteOrder_Conditional<uint16_t>(wUSize);
            RDic->Tab[wFieldRank].EffectiveUSize=wUSize;

            RDic->Tab[wFieldRank].URFSize=wUSize+sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t); // URFSize includes header size + size of Universal data

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t);
            RDic->Tab[wFieldRank].Capacity=wUSize;
            break;
            }
        if (wType==ZType_FixedWString)
            {
            uint16_t wCanonSize;
            uint16_t wUSize;
            memmove(&wCanonSize,wPtr,sizeof(wCanonSize));
            wCanonSize=reverseByteOrder_Conditional<uint16_t>(wCanonSize);
            RDic->Tab[wFieldRank].Capacity=wCanonSize;

            wPtr += sizeof(uint16_t);
            memmove(&wUSize,wPtr,sizeof(wUSize));
            wUSize=reverseByteOrder_Conditional<uint16_t>(wUSize);
            RDic->Tab[wFieldRank].EffectiveUSize=wUSize;

            RDic->Tab[wFieldRank].URFSize=wUSize+sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t); // URFSize includes header size + size of Universal data

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t);

            break;
            }
        if (wType==ZType_VaryingWString)
            {
            uint64_t wUSize;
            memmove(&wUSize,wPtr,sizeof(wUSize));
            wUSize=reverseByteOrder_Conditional<uint64_t>(wUSize);
            RDic->Tab[wFieldRank].EffectiveUSize=wUSize;
            RDic->Tab[wFieldRank].Capacity=1;

            RDic->Tab[wFieldRank].URFSize=wUSize+sizeof(ZTypeBase)+sizeof(uint64_t); // URFSize includes header size + size of Universal data

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase)+sizeof(uint64_t);

            break;
            }

        // for varying strings see further : switch on structure type

        if (wType==ZType_CheckSum)
            {
            RDic->Tab[wFieldRank].Capacity=1;
            RDic->Tab[wFieldRank].EffectiveUSize=sizeof (checkSum::content);
            RDic->Tab[wFieldRank].EffectiveNSize=sizeof (checkSum::content);
            RDic->Tab[wFieldRank].URFSize=sizeof (checkSum::content)+sizeof(ZTypeBase); // URFSize includes header size + size of Universal data

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase);

            break;
            }
        if (wType==ZType_ZDate)
            {
            RDic->Tab[wFieldRank].Capacity=1;
            RDic->Tab[wFieldRank].EffectiveUSize=sizeof (uint32_t);
            RDic->Tab[wFieldRank].EffectiveNSize=sizeof (uint32_t);
            RDic->Tab[wFieldRank].URFSize=sizeof (uint32_t)+sizeof(ZTypeBase); // URFSize includes header size + size of Universal data

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase);

            break;
            }
        if (wType==ZType_ZDateFull)
            {
            RDic->Tab[wFieldRank].Capacity=1;
            RDic->Tab[wFieldRank].EffectiveUSize=sizeof (uint64_t);
            RDic->Tab[wFieldRank].EffectiveNSize=sizeof (uint64_t);
            RDic->Tab[wFieldRank].URFSize=sizeof (uint64_t)+sizeof(ZTypeBase); // URFSize includes header size + size of Universal data

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase);

            break;
            }
        if (wType==ZType_bitset)
            {
            uint16_t wCanonSize;
            uint16_t wUSize;
            memmove(&wCanonSize,wPtr,sizeof(wCanonSize));
            wCanonSize=reverseByteOrder_Conditional<uint16_t>(wCanonSize);
            RDic->Tab[wFieldRank].Capacity=wCanonSize;

            wPtr += sizeof(uint16_t);
            memmove(&wUSize,wPtr,sizeof(wUSize));
            wUSize=reverseByteOrder_Conditional<uint16_t>(wUSize);
            RDic->Tab[wFieldRank].EffectiveUSize=wUSize;

            RDic->Tab[wFieldRank].URFSize=wUSize+sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t); // URFSize includes header size + size of Universal data

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t);
            break;
            } // ZType_bitset

        wTypeStructure= wType&ZType_StructureMask;
        switch (wTypeStructure)
        {
        case (ZType_Atomic):
            {
            RDic->Tab[wFieldRank].URFSize=getAtomicUniversalSize(wType)+sizeof(ZTypeBase);// atomic URF size is deduced from type+header size
            RDic->Tab[wFieldRank].EffectiveNSize=getAtomicNaturalSize(wType);
            RDic->Tab[wFieldRank].EffectiveUSize=getAtomicUniversalSize(wType);
            RDic->Tab[wFieldRank].URFOffset= wOffset;

            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase);
            RDic->Tab[wFieldRank].Capacity=1;
            break;
            }
        case (ZType_Array):
            {
            uint16_t wURFSize;
            memmove(&wURFSize,wPtr,sizeof(wURFSize));
            wURFSize=reverseByteOrder_Conditional<uint16_t>(wURFSize);
            RDic->Tab[wFieldRank].EffectiveUSize=wURFSize;

            RDic->Tab[wFieldRank].URFSize=wURFSize+sizeof(ZTypeBase)+sizeof(uint16_t);

//            RDic->Tab[wFieldRank].EffectiveUSize=wURFSize-RDic->Tab[wFieldRank].MDicRank->HeaderSize; // universal size is urf size minus header size

            wCanonicalCount=RDic->Tab[wFieldRank].EffectiveUSize/getAtomicUniversalSize(wType);
            RDic->Tab[wFieldRank].Capacity=wCanonicalCount;
            RDic->Tab[wFieldRank].EffectiveNSize=getAtomicNaturalSize(wType)*wCanonicalCount;
            RDic->Tab[wFieldRank].URFOffset= wOffset;
//            RDic->Tab[wFieldRank].DataOffset= wOffset+RDic->Tab[wFieldRank].MDicRank->HeaderSize;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase)+sizeof(uint16_t);
            break;
            }
        case (ZType_Blob): // blob : size is stored as a unsigned long long (uint64_t) itself subject to Endian conversion
            {
            uint64_t wURFSize;
            memmove(&wURFSize,wPtr,sizeof(wURFSize));
            wURFSize=reverseByteOrder_Conditional<uint64_t>(wURFSize);

            RDic->Tab[wFieldRank].EffectiveNSize=wURFSize;

            RDic->Tab[wFieldRank].URFSize=wURFSize+sizeof(ZTypeBase)+sizeof(int64_t); // URFSize includes header size + size of Universal data

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase)+sizeof(int64_t);
            RDic->Tab[wFieldRank].Capacity=1;  // for a Blob : Effective Array count is 1
            break;
            }
        case (ZType_VaryingMask) : // for varying length structures (class, strings) size is stored on a unsigned int 64 bytes (uint64_t)
            {
            uint64_t wURFSize;
            memmove(&wURFSize,wPtr,sizeof(wURFSize));
            wURFSize=reverseByteOrder_Conditional<uint64_t>(wURFSize);

            RDic->Tab[wFieldRank].EffectiveUSize=wURFSize;
            RDic->Tab[wFieldRank].URFSize=wURFSize+sizeof(ZTypeBase)+sizeof(uint64_t);

            RDic->Tab[wFieldRank].Capacity=RDic->Tab[wFieldRank].EffectiveUSize/getAtomicUniversalSize(wType);

            RDic->Tab[wFieldRank].EffectiveNSize=RDic->Tab[wFieldRank].Capacity*getAtomicNaturalSize(wType);

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase)+sizeof(uint64_t);
            break;
            }


        default : // for other fixed structures (class, strings) size is stored on a unsigned int 16 bytes (uint32_t)
            {
            uint16_t wURFSize;
            memmove(&wURFSize,wPtr,sizeof(wURFSize));
            wURFSize=reverseByteOrder_Conditional<uint32_t>(wURFSize);

            RDic->Tab[wFieldRank].URFSize=wURFSize;// atomic : size is deduced from type
            RDic->Tab[wFieldRank].EffectiveUSize=RDic->Tab[wFieldRank].URFSize-(sizeof(ZTypeBase)+sizeof(int32_t));

            RDic->Tab[wFieldRank].Capacity=RDic->Tab[wFieldRank].EffectiveUSize/getAtomicUniversalSize(wType);

            RDic->Tab[wFieldRank].EffectiveNSize=RDic->Tab[wFieldRank].Capacity*getAtomicNaturalSize(wType);

            RDic->Tab[wFieldRank].URFOffset= wOffset;
            RDic->Tab[wFieldRank].DataOffset= wOffset+sizeof(ZTypeBase)+sizeof(uint32_t);
            break;
            }
        }// switch
        break;  // for switch
        }// while true

        wOffset += RDic->Tab[wFieldRank].URFSize;
        wTotalRecordSize+=RDic->Tab[wFieldRank].URFSize ;
        wFieldRank++;
        }// while

    RDic->TotalRecordSize=wTotalRecordSize;
    return;
}// _setupRecordData

ZDataBuffer&
getStringNfU (ZDataBuffer&pNatural,ZDataBuffer& pUniversal,ZTypeBase pType)
{
// 8 bit strings

    if ((pType&ZType_Char)||(pType&ZType_UChar))
                        {
                        pNatural=pUniversal;
                        return pNatural;
                        }
    if ((pType&ZType_WChar)||(pType&ZType_WUChar))
                        {
                        if (is_little_endian())
                            {
                            size_t wArrayCount = pUniversal.Size/sizeof(wchar_t);
                            unsigned char* wPtr=pUniversal.Data;
                            for (size_t wi=0;wi<wArrayCount;wi++)
                                    {
                                    size_t wASize=getAtomicNaturalSize(ZType_WChar);
                                    wchar_t wCh= reverseByteOrder_Ptr<wchar_t>(wPtr);
                                    pNatural.appendData(&wCh,sizeof(wCh));
                                    wPtr += wASize;
                                    }
                            }
                        else
                            pNatural=pUniversal;
                        return pNatural;
                        }
    if (pType&ZType_WUChar)
            {
            if (is_little_endian())
                {
                size_t wArrayCount = pUniversal.Size/sizeof(unsigned wchar_t);
                unsigned char* wPtr=pUniversal.Data;
                for (size_t wi=0;wi<wArrayCount;wi++)
                        {
                        size_t wASize=getAtomicNaturalSize(ZType_WChar);
                        unsigned wchar_t wCh= reverseByteOrder_Ptr<unsigned wchar_t>(wPtr);
                        pNatural.appendData(&wCh,sizeof(wCh));
                        wPtr += wASize;
                        }
                }
            else
                pNatural=pUniversal;
            return pNatural;
            }//if (pType&ZType_WUChar)
}//getStringNfU
/**
 * @brief ZRecord::getUniversalfromRaw gets universal value
 * @param pDataPtr
 * @param pValue
 * @param pRank
 * @param pTruncate
 * @return
 */
unsigned char*
ZRecord::getURFFieldFromRawRecord (unsigned char* pDataPtr,ZDataBuffer &pURFData)
{

    if (pDataPtr==nullptr)
                    return nullptr;

unsigned char* wURFDataPtr=pDataPtr;
ZTypeBase wType;
uint64_t wUniversalSize, wNaturalSize, wHeaderSize;
uint16_t wCapacity, wEffectiveUSize;
size_t wFieldSize;
ZStatus wSt;

    wSt=_getURFHeaderData(wURFDataPtr,
                                        wType,
                                        wUniversalSize,
                                        wNaturalSize,
                                        wCapacity,
                                        wEffectiveUSize,
                                        wHeaderSize,
                                        nullptr);
    if (wSt==ZS_SUCCESS)
            {
            wFieldSize=wHeaderSize+wUniversalSize;
            pURFData.setData(wURFDataPtr,wFieldSize);
            return (unsigned char*)(pDataPtr+wFieldSize);
            }
    return nullptr;
}// getURFDataFromRaw

/**
 * @brief ZRecord::getUniversalbyRank From an already loaded ZRecord (_split() routine has been executed for this ZRecord)
 *                                  returns the Universal Value (not the URF value) of a field by its rank in record dictionary
 *
 *
 *
 * @param pValue
 * @param pRank
 * @return
 */
ZStatus
ZRecord::getUniversalbyRank (ZDataBuffer &pValue,const long pRank,bool pTruncate)
{
    if ((pRank<0)||(pRank>RDic->size()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_OUTBOUND,
                              Severity_Severe,
                              "trying to access field rank out of record dictionary boundaries");
        return ZS_OUTBOUND;
        }

    /* test field presence : if FieldPresence is omitted then all fields are reputated present */
    if (FieldPresence!=nullptr)
      if ((pRank>=FieldPresence->EffectiveBitSize)||(!FieldPresence->test(pRank)))
                            return ZS_FIELDMISSING;

    unsigned char*wDataPtr=RDic->Tab[pRank].URFData->Data;
    if (wDataPtr==nullptr)
                    return ZS_FIELDMISSING;
    ZTypeBase wType;
    memmove(&wType,wDataPtr,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);

    switch (wType)
    {
    case ZType_Utf8FixedString:
        {
        if (pTruncate)
            return utfStringHeader::getUniversalFromURF(ZType_Utf8FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        else
            return utf8FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf8FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_Utf8FixedString

    case ZType_Utf16FixedString:
        {
        if (pTruncate)
            return utf16FixedString<cst_desclen>::getUniversalFromURF_Truncated(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        else
            return utf16FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_Utf16FixedString
    case ZType_Utf32FixedString:
        {
        if (pTruncate)
            return utf32FixedString<cst_desclen>::getUniversalFromURF_Truncated(ZType_Utf32FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        else
            return utf32FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_Utf32FixedString
    case ZType_Utf8VaryingString:
        {
        return utfStringHeader::getUniversalFromURF(ZType_Utf8VaryingString,RDic->Tab[pRank].URFData->Data,pValue);

        }//ZType_Utf8VaryingString
    case ZType_Utf16VaryingString:
        {
            return utf16VaryingString::getUniversalFromURF(ZType_Utf16VaryingString,RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_Utf16VaryingString
    case ZType_Utf32VaryingString:
        {
            return utf32VaryingString::getUniversalFromURF(ZType_Utf32VaryingString,RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_Utf32VaryingString
/*
    case ZType_FixedCString:
        {
        if (pTruncate)
            return templateString<cst_desclen>::getUniversalFromURF_Truncated(RDic->Tab[pRank].URFData->Data,pValue);
        else
            return templateString<cst_desclen>::getUniversalFromURF(RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_FixedCString

    case ZType_FixedWString:
        {
        if (pTruncate)
            return templateWString<cst_desclen>::getUniversalFromURF_Truncated(RDic->Tab[pRank].URFData->Data,pValue);
        else
            return templateWString<cst_desclen>::getUniversalFromURF(RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_FixedCWtring

   case ZType_VaryingCString:
        {
        return varyingCString::getUniversalFromURF(wDataPtr,pValue);
        }//ZType_VaryingCString

    case ZType_VaryingWString:
        {
        return varyingWString::getUniversalFromURF(wDataPtr,pValue);
        }//ZType_VaryingWString
*/

    case ZType_ZDate:
        {
        return ZDate::getUniversalFromURF(wDataPtr,pValue);
        }
    case ZType_ZDateFull:
        {
        return ZDateFull::getUniversalFromURF(wDataPtr,pValue);
        }
    case ZType_CheckSum:
        {
        return checkSum::getUniversalFromURF(wDataPtr,pValue);
        }

    case ZType_Blob:
        {
        return ZBlob::getUniversalFromURF(wDataPtr,pValue);
        }

    }// switch (wType)



    if (wType&ZType_Atomic)
        {
        ZTypeBase wTypeAtomic=wType&ZType_AtomicMask;
        size_t wUSize=getAtomicUniversalSize(wTypeAtomic);
        pValue.setData(wDataPtr+sizeof(ZTypeBase),wUSize);
        return ZS_SUCCESS;
        }

    if (wType&ZType_Array)
        {
        ZTypeBase wTypeAtomic=wType&ZType_AtomicMask;
        size_t wUSize=getAtomicUniversalSize(wTypeAtomic);
        wDataPtr += sizeof (ZTypeBase);
        uint16_t wArrayCount;
        memmove(&wArrayCount,wDataPtr,sizeof(wArrayCount));
        wArrayCount=reverseByteOrder_Conditional<uint16_t>(wArrayCount);
        wUSize=wUSize*wArrayCount;
        wDataPtr += sizeof (uint16_t);
        pValue.setData(wDataPtr,wUSize);

        return ZS_SUCCESS;
        }


    wDataPtr += RDic->Tab[pRank].MDicRank->HeaderSize;

    if (RDic->Tab[pRank].EffectiveUSize==0)
            RDic->Tab[pRank].EffectiveUSize=RDic->Tab[pRank].URFSize-RDic->Tab[pRank].MDicRank->HeaderSize;
    pValue.setData(wDataPtr,RDic->Tab[pRank].EffectiveUSize);
    return ZS_SUCCESS;
}//getUniversalbyRank
/**
 * @brief ZRecord::getURFbyRank  Returns the Universal Record Format value of a field by its rank in record dictionary
 *          Nota Bene : URF is Universal format with URF Header
 *
 * @param pValue
 * @param pRank
 * @return
 */
ZStatus
ZRecord::getURFbyRank (ZDataBuffer &pValue,const long pRank)
{
    if ((pRank<0)||(pRank>RDic->size()))
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_OUTBOUND,
                                      Severity_Severe,
                                      "trying to access field rank out of record dictionary boundaries");
                return ZS_OUTBOUND;
                }
    if ((pRank>=FieldPresence->EffectiveBitSize)||(!FieldPresence->test(pRank)))
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_FIELDMISSING,
                                      Severity_Warning,
                                      "field is missing within current record");
                return ZS_FIELDMISSING;
                }

    if (RDic->Tab[pRank].URFData==nullptr)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_FIELDMISSING,
                                      Severity_Error,
                                      "field value is missing within current record while it is mentionned as present");
                return ZS_FIELDMISSING;
                }
    pValue.setData(*RDic->Tab[pRank].URFData);
    return ZS_SUCCESS;
}//getURFbyRank

#include <zxml/zxml.h>

void
ZRecord::RecordMap(FILE*pOutput)
{
    ZStatus wSt;
    ZTypeBase wType;
    uint64_t wUniversalSize, wNaturalSize, wHeaderSize;
    uint16_t wCapacity, wEffectiveUSize;

    unsigned char* wURFDataPtr= Content.Data;
    unsigned char* wEndPtr= Content.Data + Content.Size;
    int wi=0;
    size_t wOffset=0;
    ZBitset wBitset;
    wURFDataPtr=wBitset._importURF(wURFDataPtr);

    wOffset = wURFDataPtr-Content.Data;
    fprintf(pOutput,
            "-------------------------------------------------------------------\n"
            "                         Record map\n"
            "-------------------------------------------------------------------\n"
            "Record full size %ld\n"
            " Presence bitset............\n",
            Content.Size);
    wBitset.print(pOutput);

    for (ssize_t wRank=0;wRank<RDic->lastIdx();wRank++)
        {
        fprintf (pOutput,
                 " Field # <%ld>  <%s> presence <%s>\n",
                 wRank,
                 RDic->Tab[wRank].MDicRank->getName().toCChar(),
                 wBitset.test(wRank)?"Yes":"No");
        }

    fprintf(pOutput,
            " Structure map............\n");

    while (wURFDataPtr <= wEndPtr)
    {
       wSt= _getURFHeaderData(wURFDataPtr,
                          wType,
                          wUniversalSize,
                          wNaturalSize,
                          wCapacity,
                          wEffectiveUSize,
                          wHeaderSize,
                              nullptr);
       if (wSt!=ZS_SUCCESS)
       {
       unsigned char* wPtr= wURFDataPtr;
       size_t wBSt;
       while ((wSt!=ZS_SUCCESS)&&(wPtr < wEndPtr))
           {
           wPtr++;
           wSt= _getURFHeaderData(wPtr,
                              wType,
                              wUniversalSize,
                              wNaturalSize,
                              wCapacity,
                              wEffectiveUSize,
                              wHeaderSize,
                                  nullptr);
           }
        wBSt= wPtr-wURFDataPtr;
        if (wSt==ZS_SUCCESS)
            {
            fprintf(stdout,"%s> Found uncoherent data size %ld from record offset %ld before first ZType 0x%X %s\n",
                    _GET_FUNCTION_NAME_,
                    wBSt,
                    wOffset,
                    wType,
                    decode_ZType(wType));
            wOffset += wBSt;
            wURFDataPtr=wPtr;
            }
            else /* record size exhausted */
            {
            fprintf(stdout,"%s> until end of record : found uncoherent data size %ld from record offset %ld. No valid ZType found before end of record surface\n",
                    _GET_FUNCTION_NAME_,
                    wBSt,
                    wOffset);
            break;
            }
        }//if (wSt!=ZS_SUCCESS)
        fprintf (pOutput,
                 "............Field serial order %d (not dictionary rank)\n"
                 "Offset      %ld \n"
                 "Univ Size   %ld\n"
                 "Header size %ld\n"
                 "ZType       0x%X %s\n"
                 "Capacity    %d\n",
                 wi,
                 wOffset,
                 wUniversalSize,
                 wHeaderSize,
                 wType, decode_ZType(wType),
                 wCapacity
                 );

        wURFDataPtr += wUniversalSize;
        wURFDataPtr += wHeaderSize;
        wOffset += wUniversalSize;
        wOffset += wHeaderSize;
        if (wURFDataPtr >= wEndPtr)
                                break;
        wi++;
        if (wi > RDic->size() )
                {
                fprintf (stdout,
                         " number of scanned field headers <%d> is greater than record dictionary fields number <%ld>\n"
                         " ...... <more data follows> ... uncoherent record surface....\n",
                         wi,RDic->size());
                break;
                }

    }// while
    fprintf (stdout,
             "-------------------------------------------------------------------\n"
             " Record surface map stopped at record offset <%ld>\n"
             " %d fields were found on record surface.\n"
             "-------------------------------------------------------------------\n",
             wOffset,
             wi);
    return;
}// RecordMap


void
ZRecord::printRecordData(FILE*pOutput)
{
    fprintf(pOutput,
            "Rank|  %25s> %25s\n"
            "     Size and offsets...<%10s> <%10s> <%10s> <%10s> <%10s>\n",
            "Field Name",
            "Data type(ZType_type)",
            "offset",
            "Array Cnt",
            "Universal",
            "Natural",
            "URF Size");
    for (long wi=0;wi<RDic->size();wi++)
    {
        if (FieldPresence->test(wi))
        fprintf(pOutput,
                "%4ld>  %25s %25s\n"
                "      Size and offsets...<%10ld> <%10d> <%10ld> <%10ld> <%10ld\n>",
                wi,
                RDic->Tab[wi].MDicRank->getName().toCChar(),
                decode_ZType(RDic->Tab[wi].MDicRank->ZType),
                RDic->Tab[wi].URFOffset,
                RDic->Tab[wi].MDicRank->Capacity,
                RDic->Tab[wi].MDicRank->UniversalSize,
                RDic->Tab[wi].MDicRank->NaturalSize,
                RDic->Tab[wi].URFSize);
        else
            fprintf(stdout,
                     "%ld>   %s %s\n"
                     "            <field absent from record>\n",
                     wi,
                     RDic->Tab[wi].MDicRank->getName().toCChar(),
                     decode_ZType(RDic->Tab[wi].MDicRank->ZType));
    }

}//printRecordData
#include <ztoolset/utfsprintf.h>
ZStatus
ZRecord::createXMLRecord(zxmlElementPtr &wRecord)
{
ZDataBuffer wZDB;
    wRecord =zxmlcreateElement("ZRecord");
    for (long wi=0;wi<RDic->size();wi++) // dictionary detail
            {

                zxmlElementPtr wField=zxmlcreateElement("Field");
                zxmlElementPtr wElement=nullptr;
                zxmlNodePtr wNode=nullptr;
                utf8VaryingString wText;
                wRecord->addChild((zxmlNodePtr)wField);
                wRecord->newElementTextsprintf(wElement,"Rank",nullptr,"%ld",wi);
                wElement->newComment(wNode, "not modifiable : only field position in ZKDic is taken");
                wField->newElementTextChild(wElement,"Name",(const char*)RDic->Tab[wi].MDicRank->getName().toCChar());
                wField->newElementTextChild(wElement,"Presence",(FieldPresence->test(wi)?"Yes":"No"));
                if (!FieldPresence->test(wi))
                    {
                    continue;
                    }
                wField->newElementTextsprintf(wElement,"ArrayCount",nullptr,"%ld",RDic->Tab[wi].MDicRank->Capacity);
                wField->newElementTextsprintf(wElement,"HeaderSize",nullptr,"%ld",RDic->Tab[wi].MDicRank->HeaderSize);
                wField->newElementTextsprintf(wElement,"UniversalSize",nullptr,"%ld",RDic->Tab[wi].MDicRank->UniversalSize);
                wField->newElementTextsprintf(wElement,"NaturalSize",nullptr,"%ld",RDic->Tab[wi].MDicRank->NaturalSize);
                wField->newElementTextsprintf(wElement,"URFOffset",nullptr,"%ld",RDic->Tab[wi].URFOffset);
                wField->newElementTextsprintf(wElement,"DataOffset",nullptr,"%ld",RDic->Tab[wi].DataOffset);
                wField->newElementTextChild(wElement,"KeyEligible",(RDic->Tab[wi].MDicRank->KeyEligible?"Yes":"No"));

                wElement->newComment(wNode, "not modifiable : defined by ZType");

                wField->newElementTextChild(wElement,"ZType",decode_ZType( RDic->Tab[wi].MDicRank->ZType));

                wElement->newComment(wNode, "see ZType_type definition : beware the typos");

//                wField->newElementTextChild(wElement,"ZTypeCode",toHex_A(RDic->Tab[wi].MDicRank->ZType).toString());
                wField->newElementTextsprintf(wElement,"ZTypeCode",nullptr,"%X",RDic->Tab[wi].MDicRank->ZType);

                getUniversalbyRank(wZDB,wi,true);

                if ((RDic->Tab[wi].MDicRank->ZType&(ZType_String))==(ZType_String))  // Any type of ZString
                        wField->newCDataElement(wElement,"Data",wZDB,nullptr); // no encoding
                        else
                        {
                        wZDB.encryptB64();   // B64 encoding
                        wField->newCDataElement(wElement,"Data",wZDB,nullptr);
                        }
    }// for
    return  ZS_SUCCESS;
}//createXMLRecord

void
ZRecord::writeXML(FILE* pOutput)
{

ZDataBuffer wZDB;
    fprintf (pOutput,
             "           <ZRecord>\n"
             );
     for (long wi=0;wi<RDic->size();wi++) // dictionary detail
             {
            std::cout.flush();
            if (FieldPresence!=nullptr)
            if (!FieldPresence->test(wi))
                {
                fprintf (pOutput,
                         "              <Field>\n"
                         "                <Rank>%ld</Rank>  <!-- not modifiable : only field position in ZKDic is taken -->\n"
                         "                <Name>%s</Name>\n"
                         "                <Presence>%s</Presence>\n",
                         wi,
                  RDic->Tab[wi].MDicRank->getName().toCChar() ,
                         "No");

                continue;
                }

            getUniversalbyRank(wZDB,wi,true);  // truncate fixed strings not to have trailing binary zeroes up to capacity

    fprintf (pOutput,
             "              <Field>\n"
             "                <Rank>%ld</Rank>  <!-- not modifiable : only field position in ZKDic is taken -->\n"
             "                <Name>%s</Name>\n"
             "                <Presence>%s</Presence>\n"
             "                <ArrayCount>%d</ArrayCount>\n"
             "                <HeaderSize>%ld</HeaderSize>\n"
             "                <UniversalSize>%ld</UniversalSize>\n"
             "                <NaturalSize>%ld</NaturalSize>\n"
             "                <URFOffset>%ld</URFOffset>\n"
             "                <DataOffset>%ld</DataOffset>\n"
             "                <KeyEligible>%s</KeyEligible>  <!-- not modifiable : defined by ZType -->\n"
             "                <ZType>%s</ZType>   <!-- see ZType_type definition : beware the typos -->\n"
             "                <ZTypeCode>%X</ZTypeCode>\n"
             "                <Data>"
             ,
             wi,
             RDic->Tab[wi].MDicRank->getName().toCChar() ,
             "Yes",
             RDic->Tab[wi].MDicRank->Capacity,
             RDic->Tab[wi].MDicRank->HeaderSize,
             RDic->Tab[wi].MDicRank->UniversalSize,
             RDic->Tab[wi].MDicRank->NaturalSize,
             RDic->Tab[wi].URFOffset,
             RDic->Tab[wi].DataOffset,
             RDic->Tab[wi].MDicRank->KeyEligible?"Yes":"No",
             decode_ZType( RDic->Tab[wi].MDicRank->ZType),
             RDic->Tab[wi].MDicRank->ZType
             );

            fprintf (stdout," field name <%s> \n",
                     RDic->Tab[wi].MDicRank->getName().toCChar());

            wZDB.Dump(20,500,stdout);
            /* Any utf8 string or char string : non encoding. in all other cases :B64 encoding (utf16 utf32 strings and blobs)*/

             if ((RDic->Tab[wi].MDicRank->ZType&(ZType_String)) /* if utf8 string or char string : clear text */
                     &&((RDic->Tab[wi].MDicRank->ZType & ZType_U8)||  (RDic->Tab[wi].MDicRank->ZType&ZType_Char)))
             {
                 fprintf (pOutput,
                          "<![CDATA[");

                 fwrite(wZDB.DataChar,1,wZDB.Size,pOutput);     // put string data without encoding
                 fprintf (pOutput,
                          "]]>");
             }
             else // any other case : B64 encoding
                {
                 fprintf (pOutput,
                          "<![CDATA[");
                 wZDB.encryptB64();                          // B64 encode
                 fwrite(wZDB.Data,1,wZDB.Size,pOutput);
                 fprintf (pOutput,
                          "]]>");
                 if (wZDB.Size<100000)
                         wZDB.Dump(20,-1,stdout);
                }
//             for (unsigned long wz=0;wz<wZDB.Size;wz++)
//                            fputc(wZDB.Data[wz],pOutput);
             fprintf (pOutput,
                      "</Data>\n"
                      "              </Field>\n");

             }// for
    fprintf (pOutput,
              "          </ZRecord>\n");
    return ;

}//writeXML


/**
 * @brief ZRecord::_split Extracts fields from raw record according MetaDic after a ZSMasterFile get operation
 * @return
 */
ZStatus
ZRecord::_split(void)
{

ZDataBuffer* wFieldURFData;

size_t wURFSize=0;
size_t wOffset=0;

int wTotalFields=0,wMissing=0;

ZStatus wSt;
ZTypeBase wType;
uint64_t wUniversalSize, wNaturalSize, wHeaderSize;
uint16_t wCanonical, wEffectiveUSize;
size_t wFieldSize;

    if (Content.isEmpty())
      return ZS_EMPTY;

    unsigned char*wURFDataPtr = Content.Data;
    if (RDic==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_EMPTY,
                              Severity_Fatal,
                              " Record dictionary is null, while trying to split record in fields.");
        ZException.exit_abort();
        }


    if (FieldPresence!=nullptr)
                    delete FieldPresence;
    FieldPresence=new ZBitset;

    if(FieldPresence->_importURF(wURFDataPtr)==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_FILEERROR,
                              Severity_Fatal,
                              " Cannot load Field presence bitset from file ");
        ZException.exit_abort();
        }

    wOffset=(size_t)(wURFDataPtr-Content.Data);
    for (size_t wi=0;wi<RDic->size();wi++)
        {
        wTotalFields ++;
        RDic->Tab[wi].URFOffset=wOffset;
        wURFSize=0;
        wHeaderSize=0;
        if (!FieldPresence->test(wi))
                {
                printf ("field index <%ld> name <%s> is missing.\n",
                        wi,
                        (char*)RDic->Tab[wi].MDicRank->getName().toCChar());
                wMissing++;
                continue;
                }

        fprintf (stdout," processing <%s> rank <%ld>\n",RDic->Tab[wi].MDicRank->getName().toCChar(),wi);
        RDic->Tab[wi].URFOffset=wOffset;
        wURFDataPtr= Content.Data+wOffset;

        wSt=_getURFHeaderData(wURFDataPtr,
                              wType,
                              wUniversalSize,
                              wNaturalSize,
                              wCanonical,
                              wEffectiveUSize,
                              wHeaderSize,
                              nullptr);
        if (wSt!=ZS_SUCCESS)
                        {return  wSt;}
        wFieldSize = wUniversalSize+wHeaderSize;
        wFieldURFData=new ZDataBuffer(wURFDataPtr,wFieldSize); // get the whole field header + universal data

        wFieldURFData->Dump(16,100);

        RDic->Tab[wi].URFSize=wFieldSize;
        RDic->Tab[wi].EffectiveUSize=wEffectiveUSize;
        RDic->Tab[wi].EffectiveNSize=wNaturalSize;
        RDic->Tab[wi].Capacity=wCanonical;
        RDic->Tab[wi].URFData=wFieldURFData;

        fprintf (stdout," got type %x <%s>  Header size <%ld> Natural size <%ld> Universal size <%ld> Canonical <%d> Effective <%d> \n",
                 wType,
                 decode_ZType(wType),
                 wHeaderSize,
                 wNaturalSize,
                 wUniversalSize,
                 wCanonical,
                 wEffectiveUSize);

        wOffset += wFieldSize;
        }// for
    fprintf (stdout," Total fields processed <%d> missing fields <%d>\n",
             wTotalFields,
             wMissing);
    return  ZS_SUCCESS;
}// _split



/**
 * @brief ZRecord::_aggregate  concatenate fields into base ZDataBuffer in order to be written on file
 *
 *  - bitset size
 *  - bitset content
 *  - fields in URF Universal Record Format(present fields only according to bitset)
 *      . field type
 *      [. field Universal Data length if not ZType_Atomic]
 *      . field data using Universal format
 *
 *
 * @return
 */
ZStatus
ZRecord::_aggregate(void)
{

// debug data
    ZStatus wSt;
    ZTypeBase wType;
    uint64_t wUniversalSize, wNaturalSize, wHeaderSize;
    uint16_t wCapacity, wEffectiveUSize;
    size_t wURFFieldSize;
// end debug data
/*
 *  export bit set as first record element
 *
 *  compute record size :
 *  for all fields with a presence bit set : sum of urf size (universal size + header size)
 *
 *  allocate space into record for fields URF data
 *
 *  for all fields with a presence bit set : move URF data (header + content)
 *
 */

size_t wOffset=0,wMaxOffset=0;           /* debug */
unsigned char* wURFDataPtr;
unsigned char* wURFEnd;
    if (RDic==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_EMPTY,
                              Severity_Fatal,
                              " Record dictionary is null, while trying to aggregate record.");
        ZException.exit_abort();
        }
    Content.clear(); /* reset main ZDataBuffer to null */

    Content.setData(FieldPresence->_exportURF(Content));  // export bitset as first record element

    wOffset= Content.getByteSize();

/*    compute record size : */
    uint64_t wURFDataSize=0;

    for (long wi=0;wi < RDic->size();wi++)
        {
        if (FieldPresence->test(wi))
            {
            if (RDic->Tab[wi].URFData==nullptr)
                {
                continue;
                }
            wURFDataSize += RDic->Tab[wi].URFSize;
            }
        }//for (long wi=0;wi < RDic->size();wi++)

    wURFDataPtr=Content.extendBZero(wURFDataSize + 1);
    wURFEnd=wURFDataPtr + wURFDataSize ;

    wMaxOffset=Content.getByteSize();



    for (long wi=0;(wi < RDic->size()) && (wURFDataPtr < wURFEnd);wi++)
        {
        RDic->Tab[wi].URFOffset = wOffset;
        if (FieldPresence->test(wi))
            {
            if (RDic->Tab[wi].URFData==nullptr)
                {
                if (ZVerbose)
                    fprintf(stderr,
                        "%s>> Warning: URF data for field #%ld %s is null, while its presence is set.\n",
                        _GET_FUNCTION_NAME_, wi,(char*)RDic->Tab[wi].MDicRank->getName().toCChar());

                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_FIELDMISSING,
                                      Severity_Fatal,
                                      " URF data for field #%ld %s is null, while its presence is set.",
                                      wi,(char*)RDic->Tab[wi].MDicRank->getName().toCChar());
                continue;
                }
            if (ZVerbose)
                fprintf(stdout ,"%s>>Field <%ld> <%s> setting content.\n",
                        _GET_FUNCTION_NAME_, wi,(char*)RDic->Tab[wi].MDicRank->getName().toCChar());

            wSt=_getURFHeaderData(RDic->Tab[wi].URFData->getData(),
                                  wType,
                                  wUniversalSize,
                                  wNaturalSize,
                                  wCapacity,
                                  wEffectiveUSize,
                                  wHeaderSize,
                                  nullptr);
            if (wSt!=ZS_SUCCESS)
                        {return  wSt;}

            wURFFieldSize= wHeaderSize + wUniversalSize;
            _ASSERT_((wOffset+wURFFieldSize) >= Content.getByteSize(),\
                     " Record buffer capacity overflow. Record size %ld - Offset %ld - Field URF size %ld",\
                     Content.getByteSize(),wOffset,wURFFieldSize)



            memmove(wURFDataPtr,RDic->Tab[wi].URFData->getData(),wURFFieldSize);

            printf (" header data \n"           /* debug */
                    "  Record offset  %ld\n"
                    "  ZType  0x%X <%s>\n"
                    "  Universal size %ld\n"
                    "  Natural size   %ld\n"
                    "  Header size    %ld\n"
                    "  Capacity       %d\n"
                    "  Field URF size %ld\n",
                    wOffset,
                    wType, decode_ZType(wType),
                    wUniversalSize,
                    wNaturalSize,
                    wHeaderSize,
                    wCapacity,
                    wURFFieldSize);

            RDic->Tab[wi].URFData->Dump(16,150);

            wOffset += wURFFieldSize;
            wURFDataPtr += wURFFieldSize;

            }//if (FieldPresence->test(wi))
            else
            if (ZVerbose)
                fprintf(stderr,
                    "%s>> field #%ld %s is not present.\n",
                    _GET_FUNCTION_NAME_, wi,(char*)RDic->Tab[wi].MDicRank->getName().toCChar());
        }// for (long wi=0;(wi < RDic->size()) && (wURFDataPtr < wURFEnd);wi++)

    if (ZVerbose)
            fprintf (stdout,"%s> Record : URF data size %ld Allocated size %ld\n",
                     _GET_FUNCTION_NAME_,
                     wOffset,
                     Content.Size);
    printf (" Record surface check\n");
    RecordMap();


    return  ZS_SUCCESS;
}// _aggregate

/** @cond Development
 * @brief _extractAllKeys Extracts all Keys value from ZMasterFile record data using dictionnary ZSKeyDictionary fields definition
 *
 * return s the concaneted key value in pKey ZDataBuffer.
 * - Key fields are extracted from the ZMasterFile user record .
 * - They are converted appropriately whenever required using base internal conversion routines according Dictionary data type ( ZType_type ):
 *    + atomic fields _getAtomicFromRecord()
 *    + arrays _getArrayFromRecord()
 *    + for data type Class (ZType_type) : data is simply mass-moved to key without any conversion
 *
 * @note As we are processing variable length records, if a defined key field points outside the record length,
 *       then its return ing key value is set to binary zero on the corresponding length of the field within returned Key value.
 *
 *  After routine completion, KeyValue[].KeyContent buffer are setup and ready to be written on index files.
 */
void
ZRecord::_extractAllKeys(void)
{
  ZStatus wSt=ZS_SUCCESS;
  long wRDicRank=0;
  size_t wKeyOffset = 0;
  ZSKeyDictionary* wZKDic;
  ZDataBuffer wFieldUValue;

  /* compute offset of all record fields : some fields may be of variable length */

  for (long wi=0 ; wi < RawMasterFile->ZMCB.IndexCount ; wi++)
  {
//   wZKDic=MCB->Index[wi]->ZKDic;

    wZKDic =  RawMasterFile->ZMCB.MasterDic->KeyDic[wi];

    KeyValue[wi]->KeyContent.reset();
//    KeyValue[wi]->KeyContent.allocateBZero(MCB->Index[wi]->KeyUniversalSize+1);
    KeyValue[wi]->KeyContent.allocateBZero(RawMasterFile->ZMCB.IndexTable[wi]->KeyUniversalSize+1);
    for (long wj=0;wj<wZKDic->count();wj++)
      {
      wRDicRank=wZKDic->Tab[wj].MDicRank;
      getUniversalbyRank(wFieldUValue,wRDicRank);
      KeyValue[wi]->KeyContent.changeData(wFieldUValue,wKeyOffset);
      wKeyOffset += RDic->Tab[wRDicRank].MDicRank->UniversalSize;
      }
  }
  return ;
}// _extractAllKeys




/* ------- C interfaces -------------------*/
zbs::ZArray<ZRecord*> ZRecordList;



static bool ZRecordInit=false;
bool isZRecordInit() {return ZRecordInit;}
void initZRecord()
{
    ZRecordInit=true;
    atexit(&deleteZRecordAll);
}

CFUNCTOR void deleteZRecordAll()
{
    while (ZRecordList.size())
    {
        ZRecord* wZRecord= static_cast <ZRecord*> (ZRecordList.popR());
        delete wZRecord;
    }
    return;
}//deleteZRecordAll

APICEXPORT
void* createZRecord(void* pZMF)
{
    ZSMasterFile* wMasterFile= static_cast <ZSMasterFile*> (pZMF);
    ZRecord* wRecord = new ZRecord(wMasterFile);
    if (!isZRecordInit())
                    initZRecord();
    ZRecordList.push (wRecord);
    return wRecord;
}//createZRecord

APICEXPORT
void deleteZRecord(void* pZRecord)
{
    for (long wi=0;wi < ZRecordList.size(); wi++)
            if (pZRecord==ZRecordList[wi])
                {
                ZRecord* wZRecord= static_cast <ZRecord*> (pZRecord);
                delete wZRecord;
                ZRecordList.erase(wi);
                return;
                }
    fprintf (stderr,"%s-S-NotFound ZRecord has not been found while trying to delete it",_GET_FUNCTION_NAME_);
    return ;
}//deleteZRecord


/**
 * @brief setFieldValueByRank sets the value of a given field described by its rank within Master Dictionary
 * with an input value described by its ZType_type (pType)
 * Input value may be any value excepted
 * @param pType
 * @param pValue
 * @param pRank
 * @return
 */
APICEXPORT ZStatus setFieldValueByRank(void* pRecordCTX,void* pValue,const long pRank,ZTypeBase pType)
{

}

/*APICEXPORT ZStatus setFieldValueByRank_Array(void* pRecordCTX,void* pValue,const long pRank,const size_t pCapacity,ZTypeBase pType)
{

}
*/
APICEXPORT ZStatus getFieldValueByRank(void* pRecordCTX,void* pValue,const long pRank,ZTypeBase pType)
{

}

APICEXPORT ZStatus getFieldValueByRank_Array(void* pRecordCTX,void* pValue,const long pRank,const size_t pCapacity,ZTypeBase pType)
{

}

APICEXPORT ZStatus  setFieldURFfN (void* &pSourceNatural,
                          ZDataBuffer *pTargetURFData,        // out data in URF format (out)
                          ZTypeBase& pSourceType,       // source natural type (out)
                          uint64_t &pSourceNSize,       // source natural size(out)
                          uint64_t &pSourceUSize,       // source universal size (URF size minus header size)(out)
                          uint16_t &pSourceCapacity,  // source capacity (out)
                          ZTypeBase& pTargetType,       // target type (given by RDic)
                          URF_Capacity_type &pTargetCapacity)  // target units count or array count (given by RDic)
{

ZStatus wSt=ZS_SUCCESS;

/* create URF header :
 *      for atomic data only ZTypeBase data type
 *      arrays & pointer (target type is array) :[ ZTypeBase + uint32_t (array count) reverse byte order if endian]
 *      Others : [ZTypeBase + size_t (size of Universal data) reverse byte order if endian]
 *
 */

// get ZType_type and sizes + arraycount

/*    wSt=_getZType_T<_Tp>(pSourceNatural,pSourceType,pSourceNSize,pSourceUSize,pSourceCapacity);
    if (wSt!=ZS_SUCCESS)
                   { return  wSt;}

    if (ZVerbose)
            printf ("%s-Class>> assigning field value from source type <%X> <%s> to target type <%X><%s>\n",
                    _GET_FUNCTION_NAME_,
                    pSourceType,
                    decode_ZType(pSourceType),
                    pTargetType,
                    decode_ZType(pTargetType));
*/
    _ASSERT_(pSourceType!=pTargetType," Source type <0X%X> <%s> must be equal to target type <0X%X> <%s>",pSourceType,decode_ZType(pSourceType),pTargetType,decode_ZType(pTargetType));

    switch (pSourceType)
        {
        case ZType_Blob:
            {
            return  (setFieldURFfBlob(&pSourceNatural,
                                     pTargetURFData,
                                     pSourceType,
                                     pTargetType));
            }//  case ZType_StdString:
        case ZType_StdString:
            {
            return  (setFieldURFfStdString(&pSourceNatural,
                                          pTargetURFData,
                                          pSourceNSize,
                                          pSourceUSize,
                                          pSourceCapacity,
                                          pTargetType,
                                          pTargetCapacity));
            }//  case ZType_StdString:
        case ZType_StdWString:
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  " Unsupported object type <ZType_StdWString> for source object type. Please utfxxxStrings in place.");
            return  ZS_INVTYPE;
            }
/*            return (setFieldURFfStdWString(&pNatural,
                                           pURFData,
                                           pSourceNSize,
                                           pSourceUSize,
                                           pSourceArrayCount,
                                           pTargetType,
                                           pTargetArrayCount));


            }*/
        case ZType_CharFixedString:
        case ZType_Utf8FixedString:
        case ZType_Utf16FixedString:
        case ZType_Utf32FixedString:
        case ZType_CharVaryingString:
        case ZType_Utf8VaryingString:
        case ZType_Utf16VaryingString:
        case ZType_Utf32VaryingString:
            {
//            utfStringHeader *wString=static_cast<utfStringHeader*>(wNaturalPtr);
//            return  wString->_exportURFGeneric(pTargetURFData);
             return  (setFieldURFfZString(&pSourceNatural,
                                           pTargetURFData,
                                           pTargetType,
                                           pTargetCapacity));
/*           return (setFieldURFfFixedC(&pNatural,
                                       pURFData,
                                       pSourceType,
                                       pSourceNSize,
                                       pSourceUSize,
                                       pSourceArrayCount,
                                       pTargetType,
                                       pTargetArrayCount));*/
            }
#ifdef __DEPRECATED_FIELD__
       case ZType_FixedCString:
            {
            return (setFieldURFfFixedC(&pNatural,
                                       pURFData,
                                       pSourceType,
                                       pSourceNSize,
                                       pSourceUSize,
                                       pSourceArrayCount,
                                       pTargetType,
                                       pTargetArrayCount));
            }
        case ZType_FixedWString:
            {
            return (setFieldURFfFixedW(&pNatural,
                                       pURFData,
                                       pSourceType,
                                       pSourceNSize,
                                       pSourceUSize,
                                       pSourceArrayCount,
                                       pTargetType,
                                       pTargetArrayCount));
            }
        case ZType_VaryingCString:
            {
        return (setFieldURFfVaryingC(&pNatural,
                                     pURFData,
                                     pSourceType,
                                     pSourceNSize,
                                     pSourceUSize,
                                     pSourceArrayCount,
                                     pTargetType,
                                     pTargetArrayCount));
            }
        case ZType_VaryingWString:
            {
        return (setFieldURFfVaryingW(&pNatural,
                                     pURFData,
                                     pSourceType,
                                     pSourceNSize,
                                     pSourceUSize,
                                     pSourceArrayCount,
                                     pTargetType,
                                     pTargetArrayCount));
            }
#endif //__DEPRECATED_FIELD__

        case ZType_ZDate:
            {
        return  (setFieldURFfZDate(&pSourceNatural,
                                    pTargetURFData,
                                    pSourceType,
                                    pTargetType));
            }
        case ZType_ZDateFull:
            {
        return  (setFieldURFfZDateFull(&pSourceNatural,
                                      pTargetURFData,
                                      pSourceType,
                                      pTargetType));
            }
        case ZType_CheckSum:
            {
        return  (setFieldURFfCheckSum(&pSourceNatural,
                                      pTargetURFData,
                                      pSourceType,
                                      pTargetType));
            }
        default:
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  "Invalid source data type  <%s> given to convert to target type <%s>",
                                  decode_ZType(pSourceType),
                                  decode_ZType(pTargetType));
            return  ZS_INVTYPE;
            }
        }//switch (pSourceType)

    return  wSt;

}// setFieldURFfN for Classes





/** @} */
#endif // ZRECORD_CPP
