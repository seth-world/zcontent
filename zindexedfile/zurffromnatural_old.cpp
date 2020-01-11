#ifndef ZURFFROMNATURAL_CPP
#define ZURFFROMNATURAL_CPP

#include <zindexedfile/zurffromnatural.h>
#include <ztoolset/zatomicconvert.h>

ZStatus
setFieldURFfBlob(void* pNatural,
                ZDataBuffer* pURFData,
                ZTypeBase &pSourceType,
                 uint64_t &pSourceNSize,
                 uint64_t &pSourceUSize,
                 uint16_t &pSourceArrayCount,
                ZTypeBase &pTargetType,
                uint16_t &pTargetArrayCount)
{
    if (pTargetType!=ZType_Blob)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target type  <%s> given to convert from source type <%s>. Only type allowed is ZType_Blob.",
                              decode_ZType(pTargetType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    ZBlob* wZDB=static_cast<ZBlob*>(pNatural);
    wZDB->_exportURF(pURFData);
    return ZS_SUCCESS;

}// setFieldURFfBlob

ZStatus
setFieldURFfZDate(void* pNatural,
                  ZDataBuffer *pURFData,
                  ZTypeBase &pSourceType,
                  uint64_t &pSourceNSize,
                  uint64_t &pSourceUSize,
                  uint16_t &pSourceArrayCount,
                  ZTypeBase &pTargetType,
                  uint16_t &pTargetArrayCount)
{
    if (pTargetType!=ZType_ZDate)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target type  <%s> given to convert from source type <%s>. Only type allowed is ZType_ZDate.",
                              decode_ZType(pTargetType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    ZDate* wZDate=static_cast<ZDate*>(pNatural);
    wZDate->_exportURF(pURFData);
    return ZS_SUCCESS;

}// setFieldURFfZDate

ZStatus
setFieldURFfZDateFull(void* pNatural,
                      ZDataBuffer* pURFData,
                      ZTypeBase &pSourceType,
                      uint64_t &pSourceNSize,
                      uint64_t &pSourceUSize,
                      uint16_t &pSourceArrayCount,
                      ZTypeBase &pTargetType,
                      uint16_t &pTargetArrayCount)
{
    if (pTargetType!=ZType_ZDateFull)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target type  <%s> given to convert from source type <%s>. Only type allowed is ZType_ZDate.",
                              decode_ZType(pTargetType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    ZDateFull* wZDate=static_cast<ZDateFull*>(pNatural);
    wZDate->_exportURF(pURFData);
    return ZS_SUCCESS;

}// setFieldURFfZDateFull
ZStatus
setFieldURFfCheckSum(void* pNatural,
                      ZDataBuffer* pURFData,
                      ZTypeBase &pSourceType,
                      ZTypeBase &pTargetType)
{
    if (pTargetType!=ZType_CheckSum)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target type  <%s> given to convert from source type <%s>. Only type allowed is ZType_ZDate.",
                              decode_ZType(pTargetType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    checkSum* wCheckSum=static_cast<checkSum*>(pNatural);
    wCheckSum->_exportURF(pURFData);
    return ZS_SUCCESS;

}// setFieldURFfCheckSum
/**
 * @brief setFieldURFfChar converts/endian process pNatural value a ZType_Char | ZType_Pointer to any available data struture
 *          and returns its URF (universal record format) data
 *
 *          Using _exportURF facilities of each class used
 *          For Natural Arrays (char or wchar_t), URF header is generated.
 *
 * @param pNatural          pointer to data (char*) as a void* pointer
 * @param pURFData          out URF value as a ZDataBuffer
 * @param pSourceType       Mandatory
 * @param pSourceSize       Mandatory
 * @param pSourceArrayCount Mandatory
 * @param pTargetType       Mandatory
 * @param pTargetArrayCount Mandatory
 * @return
 */

ZStatus
setFieldURFfChar(void* pNatural,
                 ZDataBuffer* pURFData,
                 ZTypeBase &pSourceType,
                 uint64_t &pSourceNSize,
                 uint64_t &pSourceUSize,
                 uint16_t &pSourceArrayCount,
                 ZTypeBase &pTargetType,
                 uint16_t &pTargetArrayCount)
{
ZTypeBase wTargetAtomicType=pTargetType&ZType_AtomicMask;
ZTypeBase wSourceAtomicType=pSourceType&ZType_AtomicMask;


ZStatus wSt;
    if (pTargetType==ZType_Blob)  // blob capture anything
        {
        ZBlob wB;
        wB.setData(pNatural,pSourceNSize);
        wB._exportURF(pURFData);
        return ZS_SUCCESS;
        }

    if (wSourceAtomicType!=ZType_Char)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid structure type  <%s> given to convert. Only ZType_Char char* and ZType_WChar allowed there.",
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }

    switch (pTargetType)
        {
        case ZType_Utf8FixedString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                utfcodeString wString;
                wString.strset((utf8_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                utfdescString wString;
                wString.strset((utf8_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                utfmessageString wString;
                wString.strset((utf8_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                utfkeywordString wString;
                wString.strset((utf8_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_urilen+1:
                {
                uriString wString;
                wString.strset((utf8_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                utfidentityString wString;
                wString.strset((utf8_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedCString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)

            }// case ZType_FixedCString
/*        case ZType_FixedWString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                codeWString wString;
                wString.fromCString_Ptr((char*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                descWString wString;
                wString.fromCString_Ptr((char*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                messageWString wString;
                wString.fromCString_Ptr((char*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                keywordWString wString;
                wString.fromCString_Ptr((char*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                identityWString wString;
                wString.fromCString_Ptr((char*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedWString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)
            }// case ZType_FixedWString
*/
 /*       case ZType_VaryingCString:
            {
            varyingCString wString;
            wString.setString((char*)pNatural);
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }

        case ZType_VaryingWString:
            {
            varyingWString wString;
            wString.fromCString_PtrCount((char*)pNatural,strlen((char*)pNatural)+1);
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }*/
        //======================= Target is an Array of char or wchar_t==============================
        case ZType_Array:
            {
            if (wTargetAtomicType==ZType_Char)
                {
                ZTypeBase wType = ZType_Char|ZType_Array ;
                int32_t wArrayCountRef=pTargetArrayCount;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount!=pTargetArrayCount)
                    {
                    if (pSourceArrayCount>pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        wSt=ZS_FIELDCAPAOVFLW;
                        }
                    if (pSourceArrayCount<pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        }
                    }//pSourceArrayCount!=pTargetArrayCount
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));

                size_t wUniversalSize=wArrayCountRef;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));

                pURFData->appendData(pNatural,wArrayCountRef); // wArrayCountRef == wUniversalSize, by the way

                pURFData->extendBZero((ssize_t)pTargetArrayCount-wArrayCountRef);
                return wSt;
                }// if (wTargetAtomicType==ZType_Char)

/*            if (wTargetAtomicType==ZType_WChar)
                {
                ZTypeBase wType = ZType_WChar|ZType_Array ;
                size_t wDataByteSize= (size_t)pTargetArrayCount*sizeof(wchar_t);
                int32_t wArrayCountRef=pTargetArrayCount;
                wchar_t* wPtr;
                ZDataBuffer wZDB;

                wSt=ZS_SUCCESS;
                if (pSourceArrayCount>pTargetArrayCount)
                    {
                    wArrayCountRef = pTargetArrayCount;
                    wSt=ZS_FIELDCAPAOVFLW;
                    }
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));
                size_t wUniversalSize=wDataByteSize;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));// for array : size_t with arraycount*atomic size

                wPtr=(wchar_t*)pURFData->extendBZero(wDataByteSize); // get space for wchar_t array content
                wZDB.allocateBZero(wDataByteSize); // and for temporary storage too

                wSt=CStringtoWString(wZDB.WDataChar,pTargetArrayCount,(char*)pNatural,pSourceArrayCount); // convert from C to W

                for (long wi=0;wi<pTargetArrayCount;wi++)  // address Endian conversion
                    {
                    wPtr[wi]=reverseByteOrder_Conditional<wchar_t>(wZDB.WDataChar[wi]);
                    }
                return wSt;
                }// if (wTargetAtomicType==ZType_WChar)
*/
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  "Invalid target atomic data type given for moving pointer of type <%s>: Requested types are <ZType_Char> or <ZType_WChar>.",
                                  decode_ZType(pSourceType));
            return ZS_INVTYPE;

            }//case ZType_Array:
        default:
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target type  <%s> given to convert from source type <%s>",
                              decode_ZType(pTargetType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    }// switch wTargetStructure


}//setFieldURFfChar


#ifdef __COMMENT__
/**
 * @brief setFieldURFfWChar template that converts/endian process pNatural value a ZType_WChar | ZType_Pointer to any available data struture
 *          and returns its URF (universal record format) data
 * @param pNatural
 * @param pURFData
 * @param pSourceType
 * @param pSourceSize
 * @param pSourceArrayCount
 * @param pTargetType
 * @param pTargetArrayCount
 * @return
 */

ZStatus
setFieldURFfWChar(void* pNatural,
                  ZDataBuffer* pURFData,
                  ZTypeBase &pSourceType,
                  uint64_t &pSourceNSize,
                  uint64_t &pSourceUSize,
                  uint16_t &pSourceArrayCount,
                  ZTypeBase &pTargetType,
                  uint16_t &pTargetArrayCount)
{
ZTypeBase wTargetAtomicType=pTargetType&ZType_AtomicMask;
ZTypeBase wSourceAtomicType=pSourceType&ZType_AtomicMask;
ZTypeBase wTargetStructType=pTargetType&ZType_StructureMask;
ZTypeBase wSourceStructType=pSourceType&ZType_StructureMask;


ZStatus wSt;
    if (pTargetType==ZType_Blob)  // blob capture anything
        {
        ZBlob wB;
        wchar_t* wPtr=(wchar_t*)pNatural;
        pSourceArrayCount=(uint64_t)wcslen((wchar_t*)pNatural);
        pSourceNSize=pSourceUSize=pSourceArrayCount*sizeof(wchar_t);
        // @todo have to _reverseByteOrder the blob content
        wB.allocate(pSourceNSize);
        for (size_t wi=0;wi<pSourceArrayCount;wi++)
                wB.WDataChar[wi]= reverseByteOrder_Conditional<wchar_t>(wPtr[wi]);
//        wB.setData(pNatural,pSourceNSize);
        wB._exportURF(pURFData);
        }

    if (wSourceAtomicType!=ZType_WChar)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid structure type  <%s> given to convert. Only ZType_Char char* and ZType_WChar allowed there.",
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }

    switch (pTargetType)
        {
        case ZType_FixedCString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                utfutfcodeString wString;
                wString.fromWString_PtrCount((wchar_t*)pNatural,pSourceArrayCount);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                utfdescString wString;
                wString.fromWString_PtrCount((wchar_t*)pNatural,pSourceArrayCount);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                utfmessageString wString;
                wString.fromWString_PtrCount((wchar_t*)pNatural,pSourceArrayCount);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                keywordString wString;
                wString.fromWString_PtrCount((wchar_t*)pNatural,pSourceArrayCount);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_urilen+1:
                {
                uriString wString;
                wString.fromWString_PtrCount((wchar_t*)pNatural,pSourceArrayCount);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                identityString wString;
                wString.fromWString_PtrCount((wchar_t*)pNatural,pSourceArrayCount);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedCString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)

            }// case ZType_FixedCString
        case ZType_FixedWString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                codeWString wString;
                wString.fromString((wchar_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                descWString wString;
                wString.fromString((wchar_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                messageWString wString;
                wString.fromString((wchar_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                keywordWString wString;
                wString.fromString((wchar_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                identityWString wString;
                wString.fromString((wchar_t*)pNatural);
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedWString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)
            }// case ZType_FixedWString
        case ZType_Utf8VaryingString:
            {
            varyingCString wString;
            wString.fromWString_PtrCount((wchar_t*)pNatural,wcslen((wchar_t*)pNatural));
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }
    case ZType_Utf8VaryingString:
        {
        varyingCString wString;
        wString.fromWString_PtrCount((wchar_t*)pNatural,wcslen((wchar_t*)pNatural));
        wString._exportURF(pURFData);
        return ZS_SUCCESS;
        }
        case ZType_VaryingWString:
            {
            varyingWString wString;
            wString.fromWString((wchar_t*)pNatural);
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }
        //======================= Target is an Array of char or wchar_t==============================
        case ZType_Array:
            {
            if (wTargetAtomicType==ZType_Char)
                {
                ZTypeBase wType = ZType_Char|ZType_Array ;
                int32_t wArrayCountRef=pTargetArrayCount;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount!=pTargetArrayCount)
                    {
                    if (pSourceArrayCount>pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        wSt=ZS_FIELDCAPAOVFLW;
                        }
                    if (pSourceArrayCount<pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        }
                    }//pSourceArrayCount!=pTargetArrayCount
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));

                size_t wUniversalSize=wArrayCountRef;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));

                char*wPtr=(char*)pURFData->extendBZero((ssize_t)pTargetArrayCount);

                wSt=WStringtoCString(wPtr,pTargetArrayCount,(wchar_t*)pNatural,pSourceArrayCount); // convert from C to W

                pURFData->extendBZero((ssize_t)pTargetArrayCount-wArrayCountRef);
                return wSt;
                }// if (wTargetAtomicType==ZType_Char)

            if (wTargetAtomicType==ZType_WChar)
                {
                ZTypeBase wType = ZType_WChar+ZType_Array ;
                size_t wDataByteSize= (ssize_t)pTargetArrayCount*getAtomicUniversalSize(ZType_WChar);
                int32_t wArrayCountRef=pTargetArrayCount;
                wchar_t* wPtr;
                ZDataBuffer wZDB;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount>pTargetArrayCount)
                    {
                    wArrayCountRef = pTargetArrayCount;
                    wSt=ZS_FIELDCAPAOVFLW;
                    }
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));
                size_t wUniversalSize=wDataByteSize;
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize)); // for array : size_t with arraycount*atomic size

                wPtr=(wchar_t*)pURFData->extendBZero(wDataByteSize); // get space for wchar_t array content

                wZDB.setData(pNatural,pSourceArrayCount*sizeof(wchar_t));

                for (long wi=0;wi<pSourceArrayCount;wi++)  // address Endian conversion (wchar_t is treated as unsigned)
                    {
                    wPtr[wi]=reverseByteOrder_Conditional<wchar_t>(wZDB.WDataChar[wi]);
                    }
                return wSt;
                }// if (wTargetAtomicType==ZType_WChar)

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  "Invalid target atomic data type given for moving pointer of type <%s>: Requested types are <ZType_Char> or <ZType_WChar>.",
                                  decode_ZType(pSourceType));
            return ZS_INVTYPE;

            }//case ZType_Array:
        default:
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target structure type  <%s> given to convert from source type <%s>",
                              decode_ZType(wTargetStructType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    }// switch wTargetStructure



}//setFieldURFfWChar
#endif // __COMMENT__

#ifdef __DEPRECATED_FIELD__
/**
 * @brief setFieldURFfFixedC takes a derived of templateString (utfdescString,...) as source
 *                  and converts it to target structure URF format (target structure given by pTargetType)
 * @param pNatural
 * @param pURFData
 * @param pSourceType
 * @param pSourceSize
 * @param pSourceArrayCount
 * @param pTargetType
 * @param pTargetArrayCount
 * @return
 */
ZStatus
setFieldURFfFixedC(void* pNatural,
                   ZDataBuffer* pURFData,
                   ZTypeBase &pSourceType,
                   uint64_t &pSourceNSize,
                   uint64_t &pSourceUSize,
                   uint16_t &pSourceArrayCount,
                   ZTypeBase &pTargetType,
                   uint16_t &pTargetArrayCount)
{
ZTypeBase wTargetAtomicType=pTargetType&ZType_AtomicMask;
ZTypeBase wSourceAtomicType=pSourceType&ZType_AtomicMask;
ZTypeBase wTargetStructType=pTargetType&ZType_StructureMask;
ZTypeBase wSourceStructType=pSourceType&ZType_StructureMask;


ZStatus wSt;


    if (pSourceType!=ZType_FixedCString)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid structure type  <%s> given to convert. Only ZType_FixedCString allowed there.",
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }

    utfdescString* wPString=static_cast<utfdescString*>(pNatural);
    if (pTargetType==ZType_Blob)  // blob captures anything
        {
        ZBlob wB;
        wB.setData(wPString->toString(),wPString->size());
        wB._exportURF(pURFData);
        return ZS_SUCCESS;
        }

    if ((pTargetType&ZType_String)&& !(pTargetType&ZType_VaryingLength)) // fixed string
    {
        switch(pTargetType&ZType_AtomicMask)
        {
        case ZType_U8 :  // fixed
        {

        }
        }

    }
    switch (pTargetType)
        {
        case ZType_Utf8FixedString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                utfutfcodeString wString;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                utfdescString wString;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                utfmessageString wString;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_urilen+1:
                {
                uriString wString;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                keywordString wString;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                identityString wString;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedCString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)

            }// case ZType_FixedCString
        case ZType_FixedWString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                codeWString wString;
                wString.fromCString_PtrCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                descWString wString;
                wString.fromCString_PtrCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                messageWString wString;
                wString.fromCString_PtrCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                keywordWString wString;
                wString.fromCString_PtrCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                identityWString wString;
                wString.fromCString_PtrCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedWString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)
            }// case ZType_FixedWString
        case ZType_VaryingCString:
            {
            varyingCString wString;
            wString.fromCString_PtrCount( wPString->toString(),wPString->size());
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }

        case ZType_VaryingWString:
            {
            varyingWString wString;
            wString.fromCString_PtrCount(wPString->toString(),wPString->size()+1);
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }
        //======================= Target is an Array of char or wchar_t==============================
        case ZType_Array:
            {
            if (wTargetAtomicType==ZType_Char)
                {
                ZTypeBase wType = ZType_Char|ZType_Array ;
                int32_t wArrayCountRef=pTargetArrayCount;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount!=pTargetArrayCount)
                    {
                    if (pSourceArrayCount>pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        wSt=ZS_FIELDLOOSEPREC;
                        }
                    if (pSourceArrayCount<pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        }
                    }//pSourceArrayCount!=pTargetArrayCount
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));
                size_t wUniversalSize=wArrayCountRef;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));// for array : size_t with arraycount*atomic size

                pURFData->appendData(&wArrayCountRef,sizeof(wArrayCountRef));// wArrayCountRef == wUniversalSize, by the way

                pURFData->appendData(wPString->toString(),wArrayCountRef);

                pURFData->extendBZero((ssize_t)pTargetArrayCount-wArrayCountRef);
                return wSt;
                }// if (wTargetAtomicType==ZType_Char)

            if (wTargetAtomicType==ZType_WChar)
                {
                ZTypeBase wType = ZType_WChar|ZType_Array ;
                size_t wDataByteSize= (ssize_t)pTargetArrayCount*getAtomicUniversalSize(ZType_WChar);
                int32_t wArrayCountRef=pTargetArrayCount;
                wchar_t* wPtr;
                ZDataBuffer wZDB;

                wSt=ZS_SUCCESS;
                if (pSourceArrayCount>pTargetArrayCount)
                    {
                    wArrayCountRef = pTargetArrayCount;
                    wSt=ZS_FIELDCAPAOVFLW;
                    }
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));
                size_t wUniversalSize=wDataByteSize;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));// for array : size_t with arraycount*atomic size

                  wPtr=(wchar_t*)pURFData->extendBZero(wDataByteSize); // get space for wchar_t array content
                wZDB.allocateBZero(wDataByteSize); // and for temporary storage too

                wSt=CStringtoWString(wZDB.WDataChar,pTargetArrayCount,(char*)wPString->toString(),pSourceArrayCount); // convert from C to W

                for (long wi=0;wi<pTargetArrayCount;wi++)  // address Endian conversion
                    {
                    wPtr[wi]=reverseByteOrder_Conditional<wchar_t>(wZDB.WDataChar[wi]);
                    }
                return wSt;
                }// if (wTargetAtomicType==ZType_WChar)

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  "Invalid target atomic data type given for moving pointer of type <%s>: Requested types are <ZType_Char> or <ZType_WChar>.",
                                  decode_ZType(pSourceType));
            return ZS_INVTYPE;

            }//case ZType_Array:
        default:
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target structure type  <%s> given to convert from source type <%s>",
                              decode_ZType(wTargetStructType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    }// switch wTargetStructure


}//setFieldURFfFixedC


/**
 * @brief setFieldURFZType_FixedW takes a derived of templateWString (descWString,...) as source
 *                                  and converts it to target structure URF format (target structure given by pTargetType)

 * @param pNatural
 * @param pURFData
 * @param pSourceType
 * @param pSourceSize
 * @param pSourceArrayCount
 * @param pTargetType
 * @param pTargetArrayCount
 * @return
 */
ZStatus
setFieldURFfFixedW(void* pNatural,
                           ZDataBuffer* pURFData,
                           ZTypeBase &pSourceType,
                   uint64_t pSourceNSize,
                   uint64_t  pSourceUSize,
                           uint16_t &pSourceArrayCount,
                           ZTypeBase &pTargetType,
                           uint16_t &pTargetArrayCount)
{
ZTypeBase wTargetAtomicType=pTargetType&ZType_AtomicMask;
ZTypeBase wSourceAtomicType=pSourceType&ZType_AtomicMask;
ZTypeBase wTargetStructType=pTargetType&ZType_StructureMask;
ZTypeBase wSourceStructType=pSourceType&ZType_StructureMask;


ZStatus wSt;


    if (pSourceType!=ZType_FixedWString)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid structure type  <%s> given to convert. Only ZType_FixedWString allowed there.",
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }

    descWString* wPString=static_cast<descWString*>(pNatural);
    if (pTargetType==ZType_Blob)  // blob captures anything
        {
        ZBlob wB;
        wB.setData(wPString->toWString(),wPString->getByteSize());
        wB._exportURF(pURFData);
        return ZS_SUCCESS;
        }

    switch (pTargetType)
        {
        case ZType_FixedCString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                utfutfcodeString wString;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                utfdescString wString;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                utfmessageString wString;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                keywordString wString;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                identityString wString;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedCString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)

            }// case ZType_FixedCString
        case ZType_FixedWString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                codeWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                descWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                messageWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                keywordWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                identityWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedWString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)
            }// case ZType_FixedWString
        case ZType_VaryingCString:
            {
            varyingCString wString;
            wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }

        case ZType_VaryingWString:
            {
            varyingWString wString;
            wString.fromWStringCount(wPString->toWString(),wPString->size());
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }
        //======================= Target is an Array of char or wchar_t==============================
        case ZType_Array:
            {
            if (wTargetAtomicType==ZType_Char)
                {
                ZTypeBase wType = ZType_Char|ZType_Array ;
                int32_t wArrayCountRef=pTargetArrayCount;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount!=pTargetArrayCount)
                    {
                    if (pSourceArrayCount>pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        wSt=ZS_FIELDLOOSEPREC;
                        }
                    if (pSourceArrayCount<pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        }
                    }//pSourceArrayCount!=pTargetArrayCount
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));

                size_t wUniversalSize=wArrayCountRef;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));

                char*wPtr=(char*)pURFData->extendBZero((ssize_t)pTargetArrayCount);

                wSt=WStringtoCString(wPtr,pTargetArrayCount,(wchar_t*)wPString->toWString(),pSourceArrayCount); // convert from C to W

//                pURFData->extendBZero((ssize_t)pTargetArrayCount-wArrayCountRef);
                return wSt;
                }// if (wTargetAtomicType==ZType_Char)

            if (wTargetAtomicType==ZType_WChar)
                {
                ZTypeBase wType = ZType_WChar|ZType_Array ;
                size_t wDataByteSize= (ssize_t)pTargetArrayCount*getAtomicUniversalSize(ZType_WChar);
                int32_t wArrayCountRef=pTargetArrayCount;
                wchar_t* wPtr;
                ZDataBuffer wZDB;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount>pTargetArrayCount)
                    {
                    wArrayCountRef = pTargetArrayCount;
                    wSt=ZS_FIELDCAPAOVFLW;
                    }
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));

                size_t wUniversalSize=wDataByteSize;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));// for array : size_t with arraycount*atomic size

                wPtr=(wchar_t*)pURFData->extendBZero(wDataByteSize); // get space for wchar_t array content

                wZDB.setData(wPString->toWString(),pSourceArrayCount*sizeof(wchar_t));

                for (long wi=0;wi<pTargetArrayCount;wi++)  // address Endian conversion
                    {
                    wPtr[wi]=reverseByteOrder_Conditional<wchar_t>(wZDB.WDataChar[wi]);
                    }
                return wSt;
                }// if (wTargetAtomicType==ZType_WChar)

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  "Invalid target atomic data type given for moving pointer of type <%s>: Requested types are <ZType_Char> or <ZType_WChar>.",
                                  decode_ZType(pSourceType));
            return ZS_INVTYPE;

            }//case ZType_Array:
        default:
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target structure type  <%s> given to convert from source type <%s>",
                              decode_ZType(wTargetStructType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    }// switch wTargetStructure


}//setFieldURFfFixedW

ZStatus
setFieldURFfVaryingC(void* pNatural,
                           ZDataBuffer* pURFData,
                           ZTypeBase &pSourceType,
                     uint64_t pSourceNSize,
                     uint64_t pSourceUSize,
                           uint16_t &pSourceArrayCount,
                           ZTypeBase &pTargetType,
                           uint16_t &pTargetArrayCount)
{
ZTypeBase wTargetAtomicType=pTargetType&ZType_AtomicMask;
ZTypeBase wSourceAtomicType=pSourceType&ZType_AtomicMask;
ZTypeBase wTargetStructType=pTargetType&ZType_StructureMask;
ZTypeBase wSourceStructType=pSourceType&ZType_StructureMask;


ZStatus wSt=ZS_SUCCESS;


    if (pSourceType!=ZType_VaryingCString)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid structure type  <%s> given to convert. Only ZType_VaryingCString allowed there.",
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }

    varyingCString* wPString=static_cast<varyingCString*>(pNatural);
    if (pTargetType==ZType_Blob)  // blob captures anything
        {
        ZBlob wB;
        wB.setData(wPString->Data,wPString->Size);
        wB._exportURF(pURFData);
        return ZS_SUCCESS;
        }

    switch (pTargetType)
        {
        case ZType_FixedCString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                utfutfcodeString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_desclen+1:
                {
                utfdescString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_messagelen+1:
                {
                utfmessageString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_keywordlen+1:
                {
                keywordString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_identitylen+1:
                {
                identityString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromUtfCount(wPString->toString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedCString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)

            }// case ZType_FixedCString
        case ZType_FixedWString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                codeWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_desclen+1:
                {
                descWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_messagelen+1:
                {
                messageWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_keywordlen+1:
                {
                keywordWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            case cst_identitylen+1:
                {
                identityWString wString;
                wString.fromString(wPString->toWString());
                wString._exportURF(pURFData);
                return ZS_SUCCESS;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedWString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)
            }// case ZType_FixedWString
        case ZType_VaryingCString:
            {
            wPString->_exportURF(pURFData);
            return ZS_SUCCESS;
            }

        case ZType_VaryingWString:
            {
            varyingWString wString;
            wString.fromWStringCount(wPString->toWString(),wPString->size());
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }
        //======================= Target is an Array of char or wchar_t==============================
        case ZType_Array:
            {
            if (wTargetAtomicType==ZType_Char)
                {
                ZTypeBase wType = ZType_Char|ZType_Array ;
                int32_t wArrayCountRef=pTargetArrayCount;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount!=pTargetArrayCount)
                    {
                    if (pSourceArrayCount>pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        wSt=ZS_FIELDCAPAOVFLW;
                        }
                    if (pSourceArrayCount<pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        }
                    }//pSourceArrayCount!=pTargetArrayCount
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));

                size_t wUniversalSize=wArrayCountRef;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));

                pURFData->appendData(wPString->toString(),wPString->size());

                pURFData->extendBZero((ssize_t)pTargetArrayCount-wArrayCountRef);
                return wSt;
                }// if (wTargetAtomicType==ZType_Char)

            if (wTargetAtomicType==ZType_WChar)
                {
                ZTypeBase wType = ZType_WChar+ZType_Array ;
                size_t wDataByteSize= (ssize_t)pTargetArrayCount*getAtomicUniversalSize(ZType_WChar);
                int32_t wArrayCountRef=pTargetArrayCount;
                wchar_t* wPtr;
                ZDataBuffer wZDB;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount>pTargetArrayCount)
                    {
                    wArrayCountRef = pTargetArrayCount;
                    wSt=ZS_FIELDCAPAOVFLW;
                    }
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));
                size_t wUniversalSize=wDataByteSize;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));// for array : size_t with arraycount*atomic size

                wPtr=(wchar_t*)pURFData->extendBZero(wDataByteSize); // get space for wchar_t array content
                wZDB.allocateBZero(wDataByteSize); // and for temporary storage too

                wSt=CStringtoWString(wZDB.WDataChar,pTargetArrayCount,wPString->toString(),pSourceArrayCount); // convert from C to W

                for (long wi=0;wi<pTargetArrayCount;wi++)  // address Endian conversion
                    {
                    wPtr[wi]=reverseByteOrder_Conditional<wchar_t>(wZDB.WDataChar[wi]);
                    }
                return wSt;
                }// if (wTargetAtomicType==ZType_WChar)

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  "Invalid target atomic data type given for moving pointer of type <%s>: Requested types are <ZType_Char> or <ZType_WChar>.",
                                  decode_ZType(pSourceType));
            return ZS_INVTYPE;

            }//case ZType_Array:
        default:
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target structure type  <%s> given to convert from source type <%s>",
                              decode_ZType(wTargetStructType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    }// switch wTargetStructure


}//setFieldURFfVaryingC

ZStatus
setFieldURFfVaryingW(void* pNatural,
                     ZDataBuffer* pURFData,
                     ZTypeBase &pSourceType,
                     uint64_t pSourceNSize,
                     uint64_t pSourceUSize,
                     uint16_t &pSourceArrayCount,
                     ZTypeBase &pTargetType,
                     uint16_t &pTargetArrayCount)
{
ZTypeBase wTargetAtomicType=pTargetType&ZType_AtomicMask;
ZTypeBase wSourceAtomicType=pSourceType&ZType_AtomicMask;
ZTypeBase wTargetStructType=pTargetType&ZType_StructureMask;
ZTypeBase wSourceStructType=pSourceType&ZType_StructureMask;


ZStatus wSt=ZS_SUCCESS;


    if (pSourceType!=ZType_VaryingWString)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid structure type  <%s> given to convert. Only ZType_VaryingWString allowed there.",
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }

    varyingWString* wPString=static_cast<varyingWString*>(pNatural);
    if (pTargetType==ZType_Blob)  // blob captures anything
        {
        ZBlob wB;
        wB.setData(wPString->Data,wPString->Size);
        wB._exportURF(pURFData);
        return ZS_SUCCESS;
        }

    switch (pTargetType)
        {
        case ZType_FixedCString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                utfutfcodeString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_desclen+1:
                {
                utfdescString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_messagelen+1:
                {
                utfmessageString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_keywordlen+1:
                {
                keywordString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_identitylen+1:
                {
                identityString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedCString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)

            }// case ZType_FixedCString
        case ZType_FixedWString:
            {
            switch (pTargetArrayCount)
            {
            case cst_codelen+1:
                {
                codeWString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromStringCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_desclen+1:
                {
                descWString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromStringCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_messagelen+1:
                {
                messageWString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromStringCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_keywordlen+1:
                {
                keywordWString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromStringCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            case cst_identitylen+1:
                {
                identityWString wString;
                if (wPString->size()>wString.capacity())
                                            wSt=ZS_FIELDCAPAOVFLW;
                wString.fromStringCount(wPString->toWString(),wPString->size());
                wString._exportURF(pURFData);
                return wSt;
                }
            default :
                {
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVSIZE,
                                        Severity_Severe,
                                        "Requested to use a ZType_FixedWString of unknown capacity : <%ld>",
                                        pTargetArrayCount);
                  return ZS_INVSIZE;
                }
            }// switch (pTargetArrayCount)
            }// case ZType_FixedWString
        case ZType_VaryingCString:
            {
            varyingCString wString;
            wString.fromWString_PtrCount(wPString->toWString(),wPString->size());
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }

        case ZType_VaryingWString:
            {
            varyingWString wString;
            wString.fromWStringCount(wPString->toWString(),wPString->size());
            wString._exportURF(pURFData);
            return ZS_SUCCESS;
            }
        //======================= Target is an Array of char or wchar_t==============================
        case ZType_Array:
            {
            if (wTargetAtomicType==ZType_Char)
                {
                ZTypeBase wType = ZType_Char|ZType_Array ;
                int32_t wArrayCountRef=pTargetArrayCount;
                char*wPtrString=nullptr;

                wSt=ZS_SUCCESS;
                if (pSourceArrayCount!=pTargetArrayCount)
                    {
                    if (pSourceArrayCount>pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        wSt=ZS_FIELDCAPAOVFLW;
                        }
                    if (pSourceArrayCount<pTargetArrayCount)
                        {
                        wArrayCountRef = pTargetArrayCount;
                        }
                    }//pSourceArrayCount!=pTargetArrayCount
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));

                size_t wUniversalSize=wArrayCountRef;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));

                pURFData->appendData(wPString->toCString(wPtrString,wPString->size()),wPString->size());
                free(wPtrString); // mandatory when using varyingWString::toCstring()

                pURFData->extendBZero((ssize_t)pTargetArrayCount-wArrayCountRef);
                return wSt;
                }// if (wTargetAtomicType==ZType_Char)

            if (wTargetAtomicType==ZType_WChar)
                {
                ZTypeBase wType = ZType_WChar|ZType_Array ;
                size_t wDataByteSize= (ssize_t)pTargetArrayCount*getAtomicUniversalSize(ZType_WChar);
                int32_t wArrayCountRef=pTargetArrayCount;
                wchar_t* wPtr;
                ZDataBuffer wZDB;
                wSt=ZS_SUCCESS;
                if (pSourceArrayCount>pTargetArrayCount)
                    {
                    wArrayCountRef = pTargetArrayCount;
                    wSt=ZS_FIELDLOOSEPREC;
                    }
                // write URF header
                wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
                pURFData->setData(&wType,sizeof(wType));
                size_t wUniversalSize=wDataByteSize;
                wUniversalSize=reverseByteOrder_Conditional<size_t>(wUniversalSize);
                pURFData->appendData(&wUniversalSize,sizeof(wUniversalSize));// for array : size_t with arraycount*atomic size

                wPtr=(wchar_t*)pURFData->extendBZero(wDataByteSize); // get space for wchar_t array content
                wZDB.setData(wPString->toWString(),wPString->getByteSize());

                for (long wi=0;wi<pTargetArrayCount;wi++)  // address Endian conversion
                    {
                    wPtr[wi]=reverseByteOrder_Conditional<wchar_t>(wZDB.WDataChar[wi]);
                    }
                return wSt;
                }// if (wTargetAtomicType==ZType_WChar)

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Severe,
                                  "Invalid atomic data type given from pointer <%s>. Requested types are <ZType_Char> or <ZType_WChar>.",
                                  decode_ZType(pSourceType));
            return ZS_INVTYPE;

            }//case ZType_Array:
        default:
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid target structure type  <%s> given to convert from source type <%s>",
                              decode_ZType(wTargetStructType),
                              decode_ZType(pSourceType));
        return ZS_INVTYPE;
        }
    }// switch wTargetStructure


}//setFieldURFfVaryingW
#endif // __DEPRECATED_FIELD__

/**
 *   source type                        dest type     Endian Size/capacity
 *
 *      templateString                  templateString   No   may be truncated according to capacity
 *                          ZDataBuffer varying string   No   adjusted to precise strlen+1
 *                          ZDataBuffer varying string   No   adjusted to precise strlen+1
 *                                      templateWString  Yes  conversion to wchar_t then endian conversion
 *
 *
 *      ZDataBuffer Varying String      templateString
 *
 *      pointer to WC string            templateString  No   may be truncated according to capacity
 *                          ZDataBuffer varying string  No   adjusted to precise strlen+1
 *                                      templateWString Yes  may be truncated according to capacity then endian conversion
 *
 *      templateWString                 templateString   No   may be truncated according to capacity
 *                                      varying string   No   adjusted to precise strlen+1
 *                                      templateWString  Yes  may be truncated according to capacit then endian conversion
 *
 *      pointer to WC string            templateString  No   may be truncated according to capacity
 *                          ZDataBuffer  varying string  No   adjusted to precise strlen+1
 *                                      templateWString Yes  may be truncated according to capacity then endian conversion
 *
 *      std::string                     templateString  No    may be truncated according to capacity
 *                          ZDataBuffer varying string  No    adjusted to precise size()+1
 *                                      templateWString Yes   conversion to wchar_t then endian conversion then endian conversion
 *
 *
 *
 */





ZStatus  setFieldURFfStdString (void* pNatural,
                                ZDataBuffer*pURFData,        // out data in URF format (out)
                                uint64_t &pSourceNSize,
                                uint64_t &pSourceUSize,
                                uint16_t &pSourceArrayCount,
                                ZTypeBase &pTargetType,      // target type (given by RDic)
                                uint16_t &pTargetArrayCount)  // target array count (given by RDic)
{
ZTypeBase wSourceType = ZType_Pointer|ZType_Char;
uint16_t wSourceArrayCount;
   std::string *wStdString = static_cast<std::string*> (pNatural);
   pSourceArrayCount = (uint32_t)wStdString->size();
   pSourceNSize = pSourceUSize=(uint64_t)(wSourceArrayCount+1);

    return setFieldURFfChar((char*)wStdString->c_str(),
                            pURFData,
                            wSourceType,
                            pSourceNSize,
                            pSourceUSize,
                            wSourceArrayCount,
                            pTargetType,
                            pTargetArrayCount);
}//setFieldURFfStdString
/*

ZStatus  setFieldURFfStdWString (void* pNatural,
                                 ZDataBuffer* pURFData,        // out data in URF format (out)
                                 uint64_t &pSourceNSize,
                                 uint64_t &pSourceUSize,      // target type (given by RDic)
                                 uint16_t &pSourceArrayCount,
                                 ZTypeBase &pTargetType,
                                 uint16_t &pTargetArrayCount)  // target array count (given by RDic)
{
ZTypeBase wSourceType = ZType_Pointer|ZType_WChar;  // change ZType_type to correspond to a pointer on WChar

   std::wstring *wStdWString = static_cast<std::wstring*> (pNatural);
   pSourceArrayCount = (uint16_t)wStdWString->size();
   pSourceNSize = pSourceUSize=(uint64_t)((pSourceArrayCount+1)*(sizeof(wchar_t)));

    return setFieldURFfWChar((void*)wStdWString->c_str(),
                             pURFData,
                             wSourceType,
                             pSourceNSize,
                             pSourceUSize,
                             pSourceArrayCount,
                             pTargetType,
                             pTargetArrayCount);
}//setFieldURFfStdWString
*/

#endif // ZURFFROMNATURAL_CPP
