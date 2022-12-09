#ifndef ZSKEY_CPP
#define ZSKEY_CPP

#include "zkey.h"

#include <zconfig.h>
#include <type_traits>
#include <ztoolset/zerror.h>
#include <ztoolset/zbasedatatypes.h>
#include <zindexedfile/zdatatype.h>


#include <zindexedfile/zmasterfile.h>
//#include <zindexedfile/zrecord.h>

#include <zindexedfile/zfullindexfield.h>

using namespace zbs;

//--------------------ZKey routines---------------------------------------------------

//-------ZKey routines-------------------------------------------
/** @addtogroup ZKeyGroup
 * @{
*/

//-----------ZKey From record---------------------------------------------------
/**
 * @brief ZKey::KeyValueExtraction Extracts the raw key value from a ZMF record (pRecord) using Index Control Block definition (ZICB) fields definition list (ZICB::Index)
 * to feed the ZKey content :  returns the concaneted fields key value in pKey ZDataBuffer (base)
 *
 * @note: As we are processing variable length records, if a defined key field points outside the record length,
 *                      then its returning key value is set to binary zero on the corresponding length of the field within returned Key value.
 *
 * @see _keyValueExtraction()
 *
 * @param[in] pRecord the record to extract the key value from
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
/*ZStatus
ZSKey::keyValueExtraction( ZRecord* pRecord)
{
    return _keyValueExtraction(ZICB->ZKDic,pRecord,*this);
}//KeyValueExtraction
*/

ZKey::ZKey(ZRawMasterFile *pZMF, const long pKeyRank)
{
    IndexNumber=pKeyRank;
    ZMF = pZMF;
    ZIF = pZMF->IndexTable[pKeyRank];
    if (pKeyRank>ZMF->IndexTable.size())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_OUTBOUND,
                                        Severity_Severe,
                                        " Index <%ld> is out of index range for given ZSMasterFile",
                                        IndexNumber
                                        );
                ZException.exit_abort();
                }
  if (ZIF->IdxKeyDic==nullptr)
      {
        ZException.setMessage(_GET_FUNCTION_NAME_,
            ZS_BADDIC,
            Severity_Severe,
            " Index <%ld> has no defined key dictionary for Master File <%s>",
            IndexNumber,
            pZMF->getURIContent().toCChar()
            );
        ZException.exit_abort();
      }
//    ZIF=pZMF->IndexTable[IndexNumber];

    if (FieldPresence.allocateCurrentElements(ZIF->IdxKeyDic->count())) // alloc necessary elements and zero it
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Fatal,
                                    " Key dictionary for index <%s> contains no elements",
                                    ZIF->IndexName.toCChar()
                                    );
            ZException.exit_abort();
            }

    ZIF->IdxKeyDic->_reComputeKeySize();
//    ZICB->KeyType=ZIF_Nothing;
    allocate(ZIF->KeyUniversalSize);
    clearData();
    return;
}// ZKey CTor


/**
 * @brief ZKey::clearFieldSpace sets the key space reserved to field given by pFieldRank to binary zero
 * Extends the key buffer whenever necessary.
 * @param[in] pFieldRank field rank in key dictionary
 */
void
ZKey::clearFieldSpace (const long pFieldRank)
{
  ZFullIndexField wField;

  wField.set((ZIndexFile*)ZIF,pFieldRank);
    long wOffset=wField.KeyOffset;
    if ((wField.UniversalSize+wOffset)>Size)
        allocate(wField.UniversalSize+wOffset);
    memset (Data+wOffset,0,wField.UniversalSize);
    FieldPresence[pFieldRank]=false;                                    // Field is absent
}

/**
 * @brief setFieldRawValue sets the key section corresponding to mentioned field with raw data given by a pValue and pSize
 *
 *   Data must have been converted to Key internal format whenever necessary
 *   Field zone is padded with binary zero in data size is less than field internal format size
 *   Data is truncated to field internal format size if given data size exceeds
 *
 * @param[in] pValue a pointer to raw data value converted in Key format if necessary
 * @param[in] pSize  size of the data to set the field with
 * @param[in] pFieldRank field rank in key dictionary
 * @return a reference to current ZKey object
 *
 */
ZKey&
ZKey::setFieldRawValue (const void* pValue,const ssize_t pSize,const long pFieldRank)
{
  ZFullIndexField wField;
  wField.set((ZIndexFile*)ZIF,pFieldRank);
  long wOffset=wField.KeyOffset;
  ssize_t wSize = pSize;
    if (wSize > wField.UniversalSize)
                                wSize = wField.UniversalSize;
     if ((wField.UniversalSize+wOffset)>Size)
                 allocate(wField.UniversalSize+wOffset);

    memset (Data+wOffset,0,wField.UniversalSize);
    memmove(Data+wOffset,pValue,pSize);
return *this;
}//setFieldRawValue
/**
 * @brief setFieldRawValue sets the key section corresponding to mentioned field with raw data given by a ZDataBuffer pValue
 *
 *   Data must have been converted to Key internal format whenever necessary
 *   Field zone is padded with binary zero in data size is less than field internal format size
 *   Data is truncated to field internal format size if given data size exceeds
 *
 * @param[in] pValue a ZDataBuffer containing raw data value converted in Key format if necessary
 * @param[in] pFieldRank field rank in key dictionary
 * @return a reference to current ZKey object
 *
 */
ZKey&
ZKey::setFieldRawValue (ZDataBuffer& pValue,const long pFieldRank)
{
    return setFieldRawValue(pValue.Data,pValue.Size,pFieldRank);
}

/**
 * @brief ZKey::setKeyPartial
 * @param pFieldName
 * @param pLength
 * @return
 */
ZStatus
ZKey::setKeyPartial (const ssize_t pFieldRank ,const ssize_t pLength)
{
    if ((pFieldRank<0)||(pFieldRank > ZIF->IdxKeyDic->size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Error,
                                    " Invalid given field rank <%ld> while it must be [0,<%ld>]",
                                    pFieldRank,
                                    ZIF->IdxKeyDic->lastIdx());
            return ZS_OUTBOUND;
            }
  ZFullIndexField wField;
  wField.set ((ZIndexFile*)ZIF,pFieldRank);
// compute size up until current field
    PartialLength=0;
    for (long wi=0;wi<pFieldRank;wi++)
                        PartialLength += wField.UniversalSize;
    PartialLength += pLength;
    FPartialKey = true;

    _Base::Size = PartialLength;
    return ZS_SUCCESS;
}//setKeyPartial

/**
 * @brief ZKey::setKeyPartial
 * @param pFieldName
 * @param pLength
 * @return
 */
ZStatus
ZKey::setKeyPartial (const utf8_t* pFieldName,ssize_t pLength)
{
    long wFieldRank=ZIF->IdxKeyDic->zsearchFieldByName((const utf8_t*)pFieldName);
    if (wFieldRank<0)
            {
            ZException.addToLast(" While setting partial key length");
            return ZException.getLastStatus();
            }
    return setKeyPartial(wFieldRank,pLength);
}



/**
  * @brief ZIndexFile::zprintKeyFieldsValues     prints the key fields values in a human readable format from a ZIndexFile
  *
  * Prints the actual key content to pOutput after having converted back all composing field using ZIndex Dictionary.
  *
  * In case of ZType_Class field, then the content of data is dumped using ZDataBuffer::Dump().
  *
  * @param[in] pHeader  if set to true then key fields description is printed. False means only values are printed.
  * @param[in] pKeyDump if set to true then index key record content is dumped after the list of its fields values. False means only fields values are printed.
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
  */
 void
 ZKey::zprintKeyFieldsValues (bool pHeader,bool pKeyDump,FILE*pOutput)
 {
  ZDataBuffer* wKeyContent=this;
  _printKeyFieldsValues (wKeyContent,(ZIndexFile*) ZIF, pHeader,pKeyDump,pOutput);
     return;
/*
ZDataBuffer wPrintableField;
ZDataBuffer *wKeyContent;
long wMDicRank=0;

    wKeyContent = (ZDataBuffer*)this;

ZRecord *wRecord = ZMF->generateZRecord();

// header : index name then fields dictionary definition
     if (pHeader)
     {
     fprintf (pOutput,
              "_______________________________________________________________________________________\n"
              " Index name %s\n",
              ZICB->Name.toCChar());
     for (long wi=0;wi<ZICB->ZKDic->size();wi++)
     {
     wMDicRank= ZICB->ZKDic->Tab[wi].MDicRank;
     fprintf (pOutput,
              " Field order %ld  name <%s> Data type <%s> \n",
              wi,
              wRecord->RDic->Tab[wMDicRank].MDicRank->Name.toCChar(),
              decode_ZType( wRecord->RDic->Tab[wMDicRank].MDicRank->ZType));
     }// for
     fprintf (pOutput,
              "_______________________________________________________________________________________\n");
     }// if pHeader

// then fields values

size_t wKeyFieldOffset = 0;

     for (long wi=0;wi<ZICB->ZKDic->size();wi++)
     {

     while (true)
     {
     wMDicRank= ZICB->ZKDic->Tab[wi].MDicRank;
     fprintf (pOutput,
              "      field order <%ld>  <%s> value ",
              wi,
              wRecord->RDic->Tab[wMDicRank].MDicRank->Name.toCChar());
     if (!FieldPresence[wi])
            {
            fprintf(pOutput,
                    "**No value**\n");
            break;
            }
     while (true)
     {
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Class)  // if class : simple dump
                {
         switch (ZICB->ZKDic->Tab[wi].ZType)
         {
         case ZType_ZDate:
            {
             ZDate wDate;
             wDate.getValueFromUniversal(wKeyContent->Data+wKeyFieldOffset);
            wPrintableField.fromString()
            }
         }// switch ZType


             wPrintableField.setData(wKeyContent.DataChar +wKeyFieldOffset, ZICB->ZKDic->Tab[wi].UniversalSize);
//             wPrintableField.setData(wBlock.Content.DataChar +wKeyOffset, ZICB->ZKDic->Tab[wi].KeyFieldSize);
//         _printAtomicValueFromKey(wBlock.Content,wPrintableField,wi,ZICB->ZKDic);
                fprintf (pOutput,
                         "\n");
                wPrintableField.Dump(16,pOutput);
                break;
                }
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Atomic)
     {
         _printAtomicValueFromKey(wKeyContent,wPrintableField,wi,*ZICB->ZKDic);
         fprintf (pOutput,
                  "<%s> ",
                  wPrintableField.DataChar);
         break;
     }
     if (ZICB->ZKDic->Tab[wi].ZType == ZType_ArrayChar)
     {
         ssize_t wKeyOffset = ZICB->ZKDic->fieldKeyOffset(wi);

         wPrintableField.setData(wKeyContent.DataChar +wKeyOffset, ZICB->ZKDic->Tab[wi].UniversalSize);
//         _printAtomicValueFromKey(wBlock.Content,wPrintableField,wi,ZICB->ZKDic);
         fprintf (pOutput,
                  "<%s> ",
                  wPrintableField.DataChar);
         break;
     }
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Array)
     {
         _printArrayValueFromKey(wKeyContent,wPrintableField,wi,*ZICB->ZKDic);
         fprintf (pOutput,
                  "%s ",
                  wPrintableField.DataChar);
         break;
     }
     fprintf (pOutput,
              "**unknown type*** ");
     break;
     }// while true

     fprintf (pOutput,
              "\n");
     } // while true

     wKeyFieldOffset += ZICB->ZKDic->Tab[wi].UniversalSize;
     }// for

     if (pKeyDump)
                 Dump();

       fprintf (pOutput,
                "_______________________________________________________________________________________\n");
     return ;*/


 }//zprintKeyFieldsValues




/** @cond Development */

 // of no use for the moment : should be suppressed
/**
 * @brief testKeyTypes test compatibility of input type (pTypeIn) vs key field type (pKeyType)
 * @param[in] pTypeIn
 * @param[in] pKeyType
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
testKeyTypes (const ZTypeBase pTypeIn,const long pKeyType)
{
ZTypeBase wTypeIn = pTypeIn ;
ZTypeBase wKeyType = pKeyType ;

    if (pTypeIn & ZType_Pointer)  // if pointer : test compatibility of atomic data underneeth
                    {
                   wTypeIn = wTypeIn &(~ZType_Pointer) ;
                   if (pKeyType & ZType_Array)
                            {
                            wKeyType = pKeyType &(~ZType_Array);
                            }
                   if (wTypeIn!=wKeyType)
                   {
                   ZException.setMessage(_GET_FUNCTION_NAME_,
                                           ZS_INVTYPE,
                                           Severity_Warning,
                                           " Pointer vs Array : Invalid Atomic type : expecting <%s> - having <%s> ",
                                           decode_ZType(wKeyType),
                                           decode_ZType(wTypeIn));
                   ZException.printUserMessage(stderr);
                   }
                   return ZS_SUCCESS;
                    }// if pointer
    if (pTypeIn & ZType_StdString)
            {
            return ZS_SUCCESS;
            }


    return ZS_SUCCESS;
} // testKeyTypes

/** @endcond */



/** @cond Development */

template <class _Tp>
ZStatus
//_setKeyFieldValueStdString (std::enable_if_t<std::is_class<_Tp>::value, _Tp> pString,ZKey & pZKey, const long pFieldRank)
_setKeyFieldValueStdString (const void* pString,ZKey & pZKey, const long pFieldRank)

{
  const std::string*  wString= static_cast<const std::string*>(pString);
  ssize_t wSize = wString->size() ;
  ZFullIndexField wField;
  wField.set ((ZIndexFile*)pZKey.ZIF,pFieldRank);

//ssize_t wOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);

    if (wField.ZType!=ZType_ArrayChar)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Error,
                                " While trying to set up a Key field value : destination field->expecting array of char - having %s ",
                                decode_ZType(wField.ZType));
        return ZS_INVTYPE;
        }

    if (wSize> wField.UniversalSize)
                                wSize = wField.UniversalSize;

/*    memset (pZKey.Data+wOffset,0,pZKey.ZICB->ZKDic->Tab[pFieldRank].KeyFieldSize);

    memset (pZKey.Data+wOffset,0,pZKey.ZICB->ZKDic->Tab[pFieldRank].NaturalSize);
    strncpy(pZKey.DataChar+wOffset,wString.c_str(),wSize);
    */
    pZKey.setFieldRawValue((void*)wString->c_str(),wSize,pFieldRank);
    return ZS_SUCCESS;
}// _setKeyFieldValueStdString
 #ifdef __COMMENT__
template <class _Tp>
ZStatus
_setKeyFieldValueCString (const void* pString,ZSKey & pZKey, const long pFieldRank)

{
const char*  wString= static_cast<const char*>(pString);
ssize_t wSize = strlen(wString) ;
//ssize_t wOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);

    if (pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType!=ZType_ArrayChar)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Error,
                                " While trying to set up a Key field value : destination field->expecting array of char - having %s ",
                                decode_ZType(pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType));
        return ZS_INVTYPE;
        }

    if (wSize> pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize)
                                wSize = pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize;

/*    memset (pZKey.Data+wOffset,0,pZKey.ZICB->ZKDic->Tab[pFieldRank].KeyFieldSize);

    memset (pZKey.Data+wOffset,0,pZKey.ZICB->ZKDic->Tab[pFieldRank].NaturalSize);
    strncpy(pZKey.DataChar+wOffset,wString.c_str(),wSize);
    */
    pZKey.setFieldRawValue((void*)wString,wSize,pFieldRank);
    return ZS_SUCCESS;
}


ZStatus
testKeyTypes (const long pTypeIn,const long pKeyType);




template <class _Tp>
/**
 * @brief _castAtomicKeyFieldValue template function that converts any atomic value type to key field atomic value given by pFieldRank within Key dictionary of pKey
 * @param[in] pValue    Data to convert
 * @param[in] pZDic     Key Dictionary
 * @param[in] pZType    ZType_Type of Data to convert
 * @param[in] pFieldRank    Dictionary key field rank to convert the value to
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
//_castAtomicKeyFieldValue (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> pValue,
_castAtomicKeyFieldValue (_Tp pValue,
                             ZSKeyDictionary & pZDic,
                             const ZTypeBase pZType,
                             const long pFieldRank,
                             ZDataBuffer &pOutValue)
{


long wType = pZDic[pFieldRank].ZType & ~(ZType_NegateStructure);


    switch (wType)
    {
    case ZType_U8 :
             {
              uint8_t wValue= static_cast<uint8_t>(pValue);
              pOutValue.setData(&wValue,sizeof(wValue));
              return ZS_SUCCESS;
             }
    case ZType_S8 :
            {
        int8_t wValue= static_cast<int8_t>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
            return ZS_SUCCESS;
            }

     case ZType_U16 :
              {
        uint16_t wValue= static_cast<uint16_t>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
              return ZS_SUCCESS;
              }
     case ZType_S16 :
             {
        int16_t wValue= static_cast<int16_t>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
             return ZS_SUCCESS;
             }
     case ZType_U32 :
              {
        uint32_t wValue= static_cast<uint32_t>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
               return ZS_SUCCESS;
              }
     case ZType_S32 :
             {
        int32_t wValue= static_cast<int32_t>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
             return ZS_SUCCESS;
             }
     case ZType_U64 :
              {
        uint64_t wValue= static_cast<uint64_t>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
              return ZS_SUCCESS;
              }
     case ZType_S64 :
             {
        int64_t wValue= static_cast<int64_t>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
             return ZS_SUCCESS;
             }
    case ZType_Float :
                 {
        float wValue= static_cast<float>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
                  return ZS_SUCCESS;
                 }
     case ZType_Double :
                  {
        double wValue= static_cast<double>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
                  return ZS_SUCCESS;
                  }
     case ZType_LDouble :
                  {
        long double wValue= static_cast<long double>(pValue);
        pOutValue.setData(&wValue,sizeof(wValue));
                    return ZS_SUCCESS;
                  }

        default:
                 {
                     ZException.setMessage(_GET_FUNCTION_NAME_,
                                             ZS_INVTYPE,
                                             Severity_Fatal,
                                             "Invalid Type for key field to convert data to <%ld> <%s> encountered while processing data conversion",
                                             pZDic[pFieldRank].ZType,
                                             decode_ZType(pZDic[pFieldRank].ZType));
                     return ZS_INVTYPE;
                 }
    }// switch
}// _convertAtomicKeyFieldValue


// for atomic

template <class _Tp>
ZStatus
_setKeyFieldValue (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
                   ZSKey &pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long pArrayCount) // pArrayNumber is given for pointers others than char pointers : how much atomic data it points to
{
ZStatus wSt;
ssize_t wOffset = pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);
    ZDataBuffer wNaturalValue;
    ZDataBuffer wKeyValue;

    if (pZType != pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType )
            {
            wSt=_castAtomicKeyFieldValue<_Tp>(pValue,pZKey.ZICB->ZKDic,pZType,pFieldRank,wNaturalValue);
            }
        else
            wNaturalValue.setData(&pValue,sizeof(pValue));

    ZSIndexField_struct wField;
    wField = pZKey.ZICB->ZKDic->Tab[pFieldRank] ;
 //   wField.RecordOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0

    wSt= _getAtomicFromRecord(wNaturalValue,wKeyValue,wField);  // Extract & pack unary Atomic Field ready for Key usage
    if (wSt!=ZS_SUCCESS)
            return wSt;
    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value
    return wSt;
} // _setKeyFieldValue for atomic data type


template <class _Tp>
ZStatus
_getValueFromPointer (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
                   ZSKey &pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long pArrayCount)
{
ZStatus wSt;
ssize_t wOffset = pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);
ssize_t wArrayCount = pArrayCount;
    ZDataBuffer wNaturalValue;
    ZDataBuffer wANaturalValue;
    ZDataBuffer wKeyValue;

    if (pArrayCount<1)
            {
            wArrayCount= sizeof(pValue)/sizeof(pValue[0]);
            }
    if (wArrayCount>pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount)  // if array has more elements than key field : truncate to key field
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVSIZE,
                                    Severity_Warning,
                                    " given array size for pointer <%ld>  exceeds size of key field array size<%ld> (field rank <%ld>). Array size truncated to key field array size",
                                    wArrayCount,
                                    pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount,
                                    pFieldRank);
            ZException.printUserMessage(stderr);
            wArrayCount = pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount;
        }
    wNaturalValue.clear();

    if (pZType != pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType )
            {
            for (long wi=0;wi<wArrayCount;wi++)
            {
            wSt=_castAtomicKeyFieldValue<decltype(*pValue)>(pValue[wi],*pZKey.ZICB->ZKDic,pZType,pFieldRank,wANaturalValue);
            wNaturalValue.appendData(wANaturalValue);
             }
            }
        else
            wNaturalValue.appendData(&pValue[0],sizeof(pValue));

    ZSIndexField_struct wField;
    wField = pZKey.ZICB->ZKDic->Tab[pFieldRank] ;
//    wField.RecordOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0
    wField.ArrayCount = wArrayCount;      // cheating also for number of array occurence
    _getArrayFromRecord(wNaturalValue,wKeyValue,wField);  // Extract & pack Array of Atomic Fields ready for Key usage
    if (wSt!=ZS_SUCCESS)
            return wSt;

    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value

    return wSt;
} // _setKeyFieldValue for array data type
template <class _Tp>
ZStatus
_setKeyFieldValue (typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> &pValue,
                   ZSKey &pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long pArrayCount)
{
ZStatus wSt;
//ssize_t wOffset = pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);
ssize_t wArrayCount ;
    ZDataBuffer wNaturalValue;
    ZDataBuffer wANaturalValue;
    ZDataBuffer wKeyValue;

    if (pArrayCount<1)
            {
            wArrayCount= sizeof(pValue)/sizeof(pValue[0]);
            }
    if (wArrayCount>pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount)  // if array has more elements than key field : truncate to key field
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVSIZE,
                                    Severity_Warning,
                                    " given array size <%ld>  exceeds size of key field array size<%ld> (field rank <%ld>). Array size truncated to key field array size",
                                    wArrayCount,
                                    pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount,
                                    pFieldRank);
            ZException.printUserMessage(stderr);
            wArrayCount = pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount;
        }
    wNaturalValue.clear();

    if (pZType != pZKey.ZICB->ZKDic->Tab[pFieldRank].ZType )
            {
            for (long wi=0;wi<wArrayCount;wi++)
            {
            _castAtomicKeyFieldValue<_Tp>(pValue[wi],pZKey.ZICB->ZKDic,pZType,pFieldRank,wANaturalValue);
            wNaturalValue.appendData(wANaturalValue);
             }
            }
        else
            wNaturalValue.appendData(&pValue[0],sizeof(pValue));

    ZSIndexField_struct wField;
    wField = pZKey.ZICB->ZKDic->Tab[pFieldRank] ;
    wField.RecordOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0
    wField.ArrayCount = wArrayCount;      // cheating also for number of array occurence
    wSt=_getArrayFromRecord(wNaturalValue,wKeyValue,wField);  // Extract & pack Array of Atomic Fields ready for Key usage
    if (wSt!=ZS_SUCCESS)
            return wSt;

    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value

    return wSt;
} // _setKeyFieldValue for array data type

template <class _Tp>
ZStatus
_setKeyFieldValue (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> &pValue,
                   ZSKey & pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long pArrayCount) // array size represents the number of value occurrences the pointer points to
{
ZStatus wSt;
long wType=pZType;
long wKeyOffset= pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank);
ssize_t wArrayCount=pArrayCount<1?1:pArrayCount;

ZDataBuffer wKeyValue;


    if (wArrayCount>pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVSIZE,
                                Severity_Warning,
                                " given array size <%ld> for pointer exceeds size of key field array size<%ld> (field rank <%ld>). Array size truncated to key field array size",
                                wArrayCount,
                                pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount,
                                pFieldRank);
        ZException.printUserMessage(stderr);
        wArrayCount = pZKey.ZICB->ZKDic->Tab[pFieldRank].ArrayCount;
    }

    if (wType == ZType_ByteSeq)
        {
        pZKey.setFieldRawValue(pValue,wArrayCount,pFieldRank); // set the key value using pArrayCount as Length
        return ZS_SUCCESS;
        }

    if (wType == ZType_PointerChar)
                        {
                        return _setKeyFieldValueCString<_Tp>(pValue,pZKey,pFieldRank);
//                        pZKey.appendValue(pValue,pZKey.ZICB->ZKDic->Tab[pFieldRank].KeyFieldSize);  // Extract & pack pointer to String Fields ready for Key usage
//                        return ZS_SUCCESS;
                        }

    wType = wType & ~(ZType_Pointer);



    if ((pZType&ZType_StdString)==ZType_StdString) // pointer to std::string : use appropriate routine
            {
            return _setKeyFieldValueStdString<_Tp>(pValue,pZKey,pFieldRank);
            }


//    wNaturalValue.setData(pValue,sizeof(pArrayCount));  // pValue is a pointer and pArrayCount the number of elements

    ZSIndexField_struct wField;
    wField = pZKey.ZICB->ZKDic->Tab[pFieldRank] ;
    wField.RecordOffset = 0;                  // we are cheating key extraction mechanism saying its a record and field is at offset 0

    wSt=_getValueFromPointer<_Tp>(pValue,pZKey,pZType,pFieldRank,wArrayCount);  // Extract & pack values pointed by pValue to get Fields ready for Key usage

//    pZKey.setFieldRawValue(wKeyValue,pFieldRank); // set the key value
    return wSt;
} // _setKeyFieldValue for pointers


template <class _Tp>
ZStatus
_setKeyFieldValue (typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pValue,
                   ZSKey & pZKey,
                   const ZTypeBase pZType,
                   const long pFieldRank,
                   const long  pArrayNumber)
{
ssize_t wSize = sizeof(pValue);
// now extracting without conversion
    if ((pZType&ZType_StdString)==ZType_StdString)
            {
            return _setKeyFieldValueStdString<_Tp>(&pValue,pZKey,pFieldRank);
            }

    if (sizeof(pValue)>pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVSIZE,
                                Severity_Warning,
                                " given class size <%ld> exceeds size of key field size<%ld> (field rank <%ld>). Array size truncated to key field array size",
                                wSize,
                                pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize,
                                pFieldRank);
        ZException.printUserMessage(stderr);
        wSize = pZKey.ZICB->ZKDic->Tab[pFieldRank].UniversalSize;
    }

//    pZKey.appendValue(&pValue,wSize,pZKey.ZICB->ZKDic->fieldKeyOffset(pFieldRank)); // set the key value
    pZKey.setFieldRawValue(&pValue,wSize,pFieldRank);// set the key raw value
    return ZS_SUCCESS;
}
#endif // __COMMENT

template <class _Tp>
/**
 * @brief zsetKeyFieldValue sets the Key field with pValue. Does all controls and calls specialised routines according ZType_type and size of input data
 * @param[in] pValue    input data value of type (_Tp) defined by template parameter
 * @param[in] pZKey     ZKey object owning the key dictionary that will get the key fields values converted to internal key format.
 * @param[in] pFieldRank    Key dictionary field rank. Could be obtained by field name @see CZKeyDictionary::zgetFieldRank()
 * @param[in] pArrayCount    For a field with type pointer : number of atomic data elements pointer points to. For an array, pArrayCount is deduced.
 *                          Defaulted to -1. For all other data types, and if pArrayCount value is < 1, this value is set to 1.
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
zsetKeyFieldValue (_Tp pValue, ZKey & pZKey,const long pFieldRank,ssize_t pArrayCount)
{
ZStatus wSt;

ZTypeBase wType;
ssize_t wNaturalSize, wUniversalSize,wArrayCount;


  if ((pFieldRank<0)||(pFieldRank > pZKey.ZIF->IdxKeyDic->lastIdx()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_OUTBOUND,
                                Severity_Severe,
                                " Field rank is out of key dictionary boundaries. given value is <%ld> while dictionary limits are <0> to <%s>",
                                pFieldRank,
                                pZKey.ZIF->IdxKeyDic->lastIdx());
        return ZS_OUTBOUND;
        }

    ZFullIndexField wField;
    wField.set ((ZIndexFile*)pZKey.ZIF,pFieldRank);
//first get ZType_type of input data and test its compatibility with target key field

    wType =_getZTypeFull_T<_Tp>(wNaturalSize,wUniversalSize,pArrayCount,false) ; // with tolerance to pointers
    if (wType==ZType_Unknown)
                return ZS_INVTYPE;
    wSt=testKeyTypes(wType, wField.ZType);
    if (wSt!=ZS_SUCCESS)
        {
        return wSt;
        }

    if (pZKey.Size < pZKey.ZIF->KeyUniversalSize)    // control size of key vs dictionary key internal size : allocate if not enough
        {
        pZKey.allocate(pZKey.ZIF->KeyUniversalSize);
        }

// now extracting and converting

    if ((wType & ZType_StdString)==ZType_StdString)
            {
            wSt= _setKeyFieldValueStdString<_Tp>(&pValue,pZKey,pFieldRank); // addresses the direct reference and the pointer to std::string
            }
        else
            wSt=_setKeyFieldValue<_Tp>(pZKey,pFieldRank,pValue,pArrayCount);

    if (wSt!=ZS_SUCCESS)
                    return wSt;

    pZKey.FieldPresence[pFieldRank]=true;
} //zsetKeyFieldValue

/** @endcond */

/**
 * @brief ZKey::setFieldValue loads the value (auto) pValue to the key field (defined in key dictionary) of rank pFieldRank
 *
 * @note GNU C++ compiler converts arrays to pointers when using 'auto' clause.
 * In case of arrays, use template ZKey::setFieldValueT
 * @note
 * pArrayCount is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArrayCount is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArrayCount is not necessary because deduced from size of string.
 *
 *@bug the default value of -1 for pArrayCount does not work with GNU compiler because of the use of 'auto' clause.
 * so that, pArrayCount should systematically be mentionned
 *
 * This routine controls the coherence of given data.
 * - It makes the data conversion whenever required to have a dictionary defined data (natural data).
 * - It makes the conversion from natural field to internal key field value format.
 * - It addresses pointers vs arrays.
 * - It pads the key if the size of input data (specifically for arrays) is not large enough.
 * - It truncates the data if given value exceeds defined key field boundaries.
 *
 * @param[in] pFieldRank field order (rank) in key dictionary
 * @param[in] pValue  Value of undertermined type to set the field with
 * @param[in] pArrayCount Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
/*template <class _Tp>
ZStatus
ZSKey::setFieldValue (const long pFieldRank,_Tp pValue, const long pArrayCount)
{
    return zsetKeyFieldValue<decltype(pValue)>(pValue,*this,pFieldRank,pArrayCount);
}
*/
/**
 * @brief ZKey::setFieldValue loads the value (auto) pValue to the key field (defined in key dictionary)
 * with a name that matches given string.
 *
 *@bug the default value of -1 for pArrayCount does not work with GNU compiler because of the use of 'auto' clause.
 * so that, pArrayCount should systematically be mentionned
 *
 * @note GNU C++ compiler converts arrays to pointers when using 'auto' clause.
 * In case of arrays, use template ZKey::setFieldValueT
 * @note
 * pArrayCount is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArrayCount is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArrayCount is not necessary because deduced from size of string.
 *
 * This routine controls the coherence of given data.
 * - It makes the data conversion whenever required to have a dictionary defined data (natural data).
 * - It makes the conversion from natural field to internal key field value format.
 * - It addresses pointers vs arrays.
 * - It pads the key if the size of input data (specifically for arrays) is not large enough.
 * - It truncates the data if given value exceeds defined key field boundaries.
 *
 * @param[in] pFieldRank field order (rank) in key dictionary
 * @param[in] pValue  Value of undertermined type to set the field with
 * @param[in] pArrayCount Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
/*ZStatus
ZSKey::setFieldValue (const utf8_t *pFieldName, auto pValue, const long pArrayCount)
{
    long wFieldRank=ZICB->ZKDic->zsearchFieldByName((const utf8_t*)pFieldName);
    if (wFieldRank<0)
            return ZException.getLastStatus();
    return zsetKeyFieldValue<decltype(pValue)>(pValue,*this,wFieldRank,pArrayCount);
}
*/



template <class _Tp>
/**
 * @brief ZKey::setFieldValueT Template routine that loads the value (of type _Tp) pValue to the key field (defined in key dictionary) of rank pFieldRank
 * @note
 * Template is necessary in case of an array to keep the native data type.
 * @note
 * pArrayCount is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArrayCount is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArrayCount is not necessary because deduced from size of string.
 *
 * This routine controls the coherence of given data.
 * - It makes the data conversion whenever required to have a dictionary defined data (natural data).
 * - It makes the conversion from natural field to internal key field value format.
 * - It addresses pointers vs arrays.
 * - It pads the key if the size of input data (specifically for arrays) is not large enough.
 * - It truncates the data if given value exceeds defined key field boundaries.
 *
 * @param[in] pFieldRank field order (rank) in key dictionary
 * @param[in] pValue  Value of undertermined type to set the field with
 * @param[in] pArrayCount Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 * @return
 */
ZStatus
ZKey::setFieldValueT (const long pFieldRank,_Tp pValue, const long pArrayCount)
{
    return zsetKeyFieldValue<_Tp>(pValue,*this,pFieldRank,pArrayCount);
}


//-------------------End Build Key from Value routines-----------------------------------


/** @} */ // end ZSKeyGroup
#endif // ZSKEY_CPP
