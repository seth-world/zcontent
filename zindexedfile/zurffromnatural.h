#ifndef ZURFFROMNATURAL_H
#define ZURFFROMNATURAL_H
/**

  Conversion routines from natural format (internal usable data structure) to Universal Record Format

  This header file holds the conversion routines from natural data type format to universal record format type

  Data Types control :



  */
#include <ztoolset/zerror.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/ztypetype.h>
#include <zindexedfile/zdatatype.h>
#include <ztoolset/zatomicconvert.h>
#include <zindexedfile/zdataconversion.h>

ZStatus
setFieldURFfBlob(void* pSourceNatural,
                 ZDataBuffer &pTargetURFData,
                 ZTypeBase &pSourceType,
                 ZTypeBase &pTargetType);
ZStatus
setFieldURFfZDate(void* pSourceNatural,
                  ZDataBuffer& pTargetURFData,
                  ZTypeBase &pSourceType,
                  ZTypeBase &pTargetType);
ZStatus
setFieldURFfZDateFull(void* pSourceNatural,
                      ZDataBuffer &pTargetURFData,
                      ZTypeBase &pSourceType,
                      ZTypeBase &pTargetType);
ZStatus
setFieldURFfCheckSum(void* pSourceNatural,
                      ZDataBuffer& pTargetURFData,
                      ZTypeBase &pSourceType,
                      ZTypeBase &pTargetType);


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
setFieldURFfChar(void* pSourceNatural,
                 ZDataBuffer& pTargetURFData,
                 ZTypeBase &pSourceType,
                 uint64_t &pSourceNSize,
                 URF_Capacity_type &pSourceCapacity,
                 ZTypeBase &pTargetType,
                 URF_Capacity_type &pTargetCapacity);




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
/*
ZStatus
setFieldURFfWChar(void* pNatural,
                  ZDataBuffer *pURFData,
                  ZTypeBase &pSourceType,
                  uint64_t &pSourceNSize,
                  uint64_t &pSourceUSize,
                  URF_Capacity_type &pSourceArrayCount,
                  ZTypeBase &pTargetType,
                  URF_Capacity_type &pTargetArrayCount);

*/



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
                   ZDataBuffer *pURFData,
                   ZTypeBase &pSourceType,
                   uint64_t pSourceNSize,
                   uint64_t pSourceUSize,
                   uint16_t &pSourceArrayCount,
                   ZTypeBase &pTargetType,
                   uint16_t &pTargetArrayCount);


ZStatus
setFieldURFfVaryingC(void* pNatural,
                     ZDataBuffer* pURFData,
                     ZTypeBase &pSourceType,
                     uint64_t pSourceNSize,
                     uint64_t pSourceUSize,
                     uint16_t &pSourceArrayCount,
                     ZTypeBase &pTargetType,
                     uint16_t &pTargetArrayCount);


ZStatus
setFieldURFfVaryingW(void* pNatural,
                     ZDataBuffer *pURFData,
                     ZTypeBase &pSourceType,
                     uint64_t pSourceNSize,
                     uint64_t pSourceUSize,
                     uint16_t &pSourceArrayCount,
                     ZTypeBase &pTargetType,
                     uint16_t &pTargetArrayCount);


//==========================Main entry points / data type for setFieldURF_T====================================

template <class _Tp>
static inline
ZStatus  setFieldURFfN_T (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> &pValue,
                          ZDataBuffer* pURFData,        // out data in URF format
                          ZTypeBase& pSourceType,
                          uint64_t &pSourceNSize,       // source natural size(out)
                          uint64_t &pSourceUSize,       // source universal size (URF size minus header size)(out)
                          URF_Capacity_type &pSourceCapaciy,  // source array count
                          ZTypeBase& pTargetType,      // target type (given by RDic)
                          URF_Capacity_type &pTargetCapacity)        // target array count (given by RDic)
{
ZStatus wSt;
uint16_t wArrayCount;
ZDataBuffer wUValue;

    // create URF header : for atomic data only ZTypeBase data type
    wSt=_getZTypeFull_T<_Tp>(pValue,pSourceType,pSourceNSize,pSourceUSize,wArrayCount);
    if (wSt!=ZS_SUCCESS)
            return wSt;

    printf ("%s-Atomic> assigning field value from source type <%X> <%s> to target type <%X><%s>\n",
            _GET_FUNCTION_NAME_,
            pSourceType,
            decode_ZType(pSourceType),
            pTargetType,
            decode_ZType(pTargetType));

//    wTargetType=reverseByteOrder_Conditional<ZTypeBase>(pTargetType);
//    pURFData->setData(&wTargetType,sizeof(wTargetType));
/*
    if (pSourceType!=pTargetType)
        {
        wSt=_castAtomicToUfN_T<_Tp>(pValue,wUValue,pTargetType);
        }
        else
        _getAtomicUfN_T<_Tp>((unsigned char*) &pValue,wUValue,pTargetType);
    if (wSt!=ZS_SUCCESS)
                return wSt;
*/
    unsigned char* wURFPtr=pURFData->allocateBZero(wUValue.Size+sizeof(pTargetType));

    wURFPtr=setURFBufferValue<ZTypeBase>(wURFPtr,pTargetType);
    memmove(wURFPtr,wUValue.Data,wUValue.Size);

    pTargetCapacity=pSourceCapaciy=1;
    return ZS_SUCCESS;
} //setFieldURF_T for Atomic

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

/**
 *
 *      Input data type : pointer
 *      -------------------------
 *
 *   pArrayCount is input mandatory
 *
 *   size control p
 *
 *
 *
 *  Atomic type      \             Blob FixedCString FixedWString VaryingCString VaryingWString Array   Array   Array
 *                    \                                                                          Char   wchar_t Other
 *                     \
 *  Ztype_Char                     x         x          conv        x               conv         x       conv    --
 *
 *  ZType_WChar                    x       conv         x           conv            x           conv      x      --
 *
 *  Other Atomic                   x        --          --          --              --           --      --      conv(*)
 *  Data
 *
 * DEPRECATED
 * (*) cast to new format if necessary using  <_castAtomicToUfN_T>
 *
 *
 */

template <class _Tp>
static inline
ZStatus  setFieldURFfN_T (typename std::enable_if_t<std::is_pointer<_Tp>::value,_Tp> pNatural,
                          ZDataBuffer* pURFData,        // out data in URF format (out)
                          ZTypeBase& pSourceType,      // source natural type (out)
                          uint64_t &pSourceNSize,       // source natural size(out)
                          uint64_t &pSourceUSize,       // source universal size (URF size minus header size)(out)
                          uint32_t &pSourceArrayCount,  // source array count MANDATORY for a pointer
                          ZTypeBase& pTargetType,      // target type (given by RDic)
                          uint32_t &pTargetArrayCount)  // target array count (given by RDic)
{
ZStatus wSt=ZS_SUCCESS;
ZDataBuffer wValue;
ZTypeBase wTargetStructType,wTargetAtomicType,wSourceStructType,wSourceAtomicType ;
uint32_t  wRetainedArrayCount;
ZDataBuffer wUValue;
    if (pSourceArrayCount<1)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Severe,
                              "Source array count value <%d> is invalid while requesting to convert a pointer to URF",
                              pSourceArrayCount);
        return ZS_INVVALUE;
        }
    // create URF header : for atomic data only ZTypeBase data type
    pSourceType=_getZTypeFull_T<_Tp>(pNatural,pSourceNSize,pSourceUSize,pSourceArrayCount);
    if (pSourceType==ZType_Unknown)
                    return ZS_INVTYPE;
    printf ("%s-Pointer> assigning field value from source type <%X> <%s> to target type <%X><%s>\n",
            _GET_FUNCTION_NAME_,
            pSourceType,
            decode_ZType(pSourceType),
            pTargetType,
            decode_ZType(pTargetType));

    pURFData->setData(&pTargetType,sizeof(pTargetType));     // ZTypeBase    full ZType_type for target data (RDic)
    pURFData->appendData(&pTargetArrayCount,sizeof(pTargetArrayCount));  // uint32_t     Array count for target data (RDic)

    wTargetStructType = pTargetType&ZType_StructureMask;
    wSourceStructType = pSourceType&ZType_StructureMask;
    wTargetAtomicType = pTargetType&ZType_AtomicMask;
    wSourceAtomicType = pSourceType&ZType_AtomicMask;

    if (wSourceAtomicType&ZType_Char)
        {
        return setFieldURFfChar(pNatural,
                                pURFData,
                                pSourceType,
                                pSourceNSize,
                                pSourceUSize,
                                pSourceArrayCount,
                                pTargetType,
                                pTargetArrayCount);
        }
    else
/*    if (pSourceType&ZType_WChar)
        {
        return setFieldURFfWChar(pNatural,
                                 pURFData,
                                 pSourceType,
                                 pSourceNSize,
                                 pSourceUSize,
                                 pSourceArrayCount,
                                 pTargetType,
                                 pTargetArrayCount);
        }
*/
    if (pSourceArrayCount!=pTargetArrayCount)
        {
        fprintf (stderr,"%s>Warning array counts differ from natural input data <%d> to target data <%d>\n",
                 _GET_FUNCTION_NAME_,
                 pSourceArrayCount,
                 pTargetArrayCount);
        if (pSourceArrayCount<pTargetArrayCount)
            wRetainedArrayCount=pSourceArrayCount;
         else
            {
            fprintf (stderr,"%s>Warning input array will be truncated to target array count <%d>\n",
                     _GET_FUNCTION_NAME_,
                     pTargetArrayCount);
            wRetainedArrayCount=pTargetArrayCount;
            }
        }
    pURFData->clear();
/*    casting is deprecated
    if (pSourceType!=pTargetType)
        {
        for (long wi=0;(wi<wRetainedArrayCount)&&(wSt==ZS_SUCCESS);wi++)
            {
              wSt=_castAtomicToUfN_T<typeof(pNatural[0])>(pNatural[wi],pTargetType,wValue);
             if (wSt==ZS_SUCCESS)
                    pURFData->appendData(wValue);
            }// for

        }//if (pOriginType!=pTargetType)
        else
        {
*/
        for (long wi=0;(wi<pTargetArrayCount)&&(wSt==ZS_SUCCESS);wi++)
            {
            wSt= _getAtomicUfN<typeof(pNatural[0])>(pNatural[wi],wValue,pTargetType);
             if (wSt==ZS_SUCCESS)
                    pURFData->appendData(wValue);
            }// for
//        }
// padding to 0 wether necessary

decltype(pNatural[0]) wZeroValue=0;
    for (long wi=wRetainedArrayCount;(wi<pTargetArrayCount)&&(wSt==ZS_SUCCESS);wi++)
            {
            wSt= _getAtomicUfN<typeof(pNatural[0])>( &wZeroValue,wValue,pTargetType);
            if (wSt==ZS_SUCCESS)
                       pURFData->appendData(wValue);
            }
    return wSt;

}// setFieldURF_T for Pointers

/**
 *
 *      input data type : Array
 *      ------------------------
 *                     \           Target structure
 *                      \
 *  Atomic type          \         Blob FixedCString FixedWString VaryingCString VaryingWString Array   Array   Array
 *                                                                                               Char   wchar_t Other
 *
 *  Ztype_Char                     x         x          conv        x               conv         x       conv    --
 *
 *  ZType_WChar                    x       conv         x           conv            x           conv      x      --
 *
 *  Other Atomic                   x        --          --          --              --           --      --      conv(*)
 *
 *
 * (*) cast to new format if necessary using  <_castAtomicToUfN_T>
 *
 *
 */

template <class _Tp>
static inline
ZStatus  setFieldURFfN_T (typename std::enable_if_t<std::is_array<_Tp>::value,_Tp> pNatural,
                          ZDataBuffer*pURFData,        // out data in URF format (out)
                          ZTypeBase& pSourceType,      // source natural type (out)
                          URF_Capacity_type &pSourceArrayCount,  // source array count
                          ZTypeBase& pTargetType,      // target type (given by RDic)
                          URF_Capacity_type &pTargetArrayCount)  // target array count (given by RDic)
{
ZStatus wSt=ZS_SUCCESS;
ZDataBuffer wValue;
ZTypeBase wTargetStructType,wTargetAtomicType,wSourceStructType,wSourceAtomicType ;
size_t wNSize, wUSize;
uint32_t  wRetainedArrayCount;
//ZDataBuffer wUValue;

    wSt=_getZTypeFull_T<_Tp>(pNatural,pSourceType,wNSize,wUSize,pSourceArrayCount);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    printf ("%s-Array>> assigning field value from source type <%X> <%s> to target type <%X><%s>\n",
            _GET_FUNCTION_NAME_,
            pSourceType,
            decode_ZType(pSourceType),
            pTargetType,
            decode_ZType(pTargetType));

    wTargetStructType = pTargetType&ZType_StructureMask;
    wSourceStructType = pSourceType&ZType_StructureMask;
    wTargetAtomicType = pTargetType&ZType_AtomicMask;
    wSourceAtomicType = pSourceType&ZType_AtomicMask;

    if (wSourceAtomicType&ZType_Char)
        {
        return setFieldURFfChar(pNatural,pURFData,pSourceType,wNSize,pSourceArrayCount,pTargetType,pTargetArrayCount);
        }
/*    else
    if (pSourceType&ZType_WChar)
        {
        return setFieldURFfWChar(pNatural,pURFData,pSourceType,wNSize,pSourceArrayCount,pTargetType,pTargetArrayCount);
        }
        */
//================Other than char[] or wchar_t[]==========================================
    if (pSourceArrayCount!=pTargetArrayCount)
        {
        fprintf (stderr,"%s>Warning array counts differ from natural input data <%d> to target data <%d>\n",
                 _GET_FUNCTION_NAME_,
                 pSourceArrayCount,
                 pTargetArrayCount);
        if (pSourceArrayCount<pTargetArrayCount)
            wRetainedArrayCount=pSourceArrayCount;
         else
            {
            fprintf (stderr,"%s>Warning input array will be truncated to target array count <%d>\n",
                     _GET_FUNCTION_NAME_,
                     pTargetArrayCount);
            wRetainedArrayCount=pTargetArrayCount;
            }
        }
    pURFData->clear();

/*    if (pSourceType!=pTargetType)
        {
        for (long wi=0;(wi<wRetainedArrayCount)&&(wSt==ZS_SUCCESS);wi++)
            {
              wSt=_castAtomicToUfN_T<decltype(pNatural[0])>(pNatural[wi],pTargetType,wValue);
             if (wSt==ZS_SUCCESS)
                    pURFData->appendData(wValue);
            }// for

        }//if (pOriginType!=pTargetType)
        else
        {*/
        for (long wi=0;(wi<pTargetArrayCount)&&(wSt==ZS_SUCCESS);wi++)
            {
            wSt= _getAtomicUfN<decltype(pNatural[0])>(pNatural[wi],wValue,pTargetType);
             if (wSt==ZS_SUCCESS)
                    pURFData->appendData(wValue);
            }// for
//        }
// padding to 0 wether necessary

decltype(pNatural[0]) wZeroValue=0;
    for (long wi=wRetainedArrayCount;(wi<pTargetArrayCount)&&(wSt==ZS_SUCCESS);wi++)
            {
            wSt= _getAtomicUfN<decltype(pNatural[0])>(wZeroValue,wValue,pTargetType);
            if (wSt==ZS_SUCCESS)
                       pURFData->appendData(wValue);
            }
    return wSt;

}// setFieldURF_T for Pointer



ZStatus  setFieldURFfStdString (void* pNatural,
                                ZDataBuffer &pURFData,
                                uint64_t &pSourceNSize,
                                uint64_t &pSourceUSize,        // out data in URF format (out)
                                URF_Capacity_type &pSourceCapacity,
                                ZTypeBase &pTargetType,      // target type (given by RDic)
                                URF_Capacity_type &pTargetCapacity);  // target array count (given by RDic)



ZStatus  setFieldURFfStdWString (void* pNatural,
                                 ZDataBuffer *pURFData,        // out data in URF format (out)
                                 uint64_t &pSourceNSize,       // source natural size(out)
                                 uint64_t &pSourceUSize,       // source universal size (URF size minus header size)(out)
                                 URF_Capacity_type &pSourceArrayCount,
                                 ZTypeBase& pTargetType,      // target type (given by RDic)
                                 uint16_t &pTargetArrayCount);  // target array count (given by RDic)

/**
 *  input data type : Class
 *  ------------------------
 *
 *             \               target class type
 *  input type  \                                                                              | fixed arrays of chars  |
 *               \    FixedCString   VaryingCString    FixedWString       VaryingWString  Blob   char[]      wchar_t[]
 *
 *  std::string        capacity           X             conversion          conversion     X    capa cont.    conv
 *                     control                        capacity control                                       capa cont.
 *
 *  std::wstring       capacity          conv.                                   X         X      conv.
 *                     control                        capacity control                          capa cont.   capa cont.
 *
 *  FixedCString       capacity           X             conversion          conversion     X    capa cont.    conv
 *                     control                        capacity control                                       capa cont.
 *
 *  FixedWString       conversion       conversion                              X          X     conv.       capa cont.
 *                   capacity control                 capacity control                          capa cont.
 *
 *  VaryingCString     capacity           X             conversion          conversion     X    capa cont.    conv.
 *                     control                        capacity control                                       capa cont.
 *
 *  VaryingWString     conversion       conversion    capacity control          X          X      conv.      capa cont.
 *                   capacity control                                                           capa cont.
 *
 *  Blob                -No-             -No-               -No-               -No-        X       -No-         -No-
 *
 */
#ifdef __DEPRECATED__
template <class _Tp>
/**
 * @brief setFieldURFfN_T  Set value of a target field defined by its metadic definition from a source variable
 * @param pSourceNatural
 * @param pTargetURFData
 * @param pSourceType
 * @param pSourceNSize
 * @param pSourceUSize
 * @param pSourceArrayCount
 * @param pTargetType
 * @param pTargetArrayCount
 * @return
 */
static inline
ZStatus  setFieldURFfN_T (typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pSourceNatural,
                          ZDataBuffer *pTargetURFData,        // out data in URF format (out)
                          ZTypeBase& pSourceType,       // source natural type (out)
                          uint64_t &pSourceNSize,       // source natural size(out)
                          uint64_t &pSourceUSize,       // source universal size (URF size minus header size)(out)
                          uint16_t &pSourceCapacity,  // source capacity (out)
                          ZTypeBase& pTargetType,       // target type (given by RDic)
                          URF_Capacity_type &pTargetCapacity)  // target units count or array count (given by RDic)
{

ZStatus wSt=ZS_SUCCESS;
//ZDataBuffer wTargetURFData;
/* create URF header :
 *      for atomic data only ZTypeBase data type
 *      arrays & pointer (target type is array) :[ ZTypeBase + uint32_t (array count) reverse byte order if endian]
 *      Others : [ZTypeBase + size_t (size of Universal data) reverse byte order if endian]
 *
 */

// get ZType_type and sizes + arraycount


    wSt=_getZTypeFull_T<_Tp>(pSourceNatural,pSourceType,pSourceNSize,pSourceUSize,pSourceCapacity);
    if (wSt!=ZS_SUCCESS)
                   { return  wSt;}

    if (ZVerbose)
            printf ("%s-Class>> assigning field value from source type <%X> <%s> to target type <%X><%s>\n",
                    _GET_FUNCTION_NAME_,
                    pSourceType,
                    decode_ZType(pSourceType),
                    pTargetType,
                    decode_ZType(pTargetType));

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
            wSt=setFieldURFfStdString(&pSourceNatural,
                                      pTargetURFData,
                                      pSourceNSize,
                                      pSourceUSize,
                                      pSourceCapacity,
                                      pTargetType,
                                      pTargetCapacity);

            return  wSt;
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
        case ZType_URIString:
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

}// setFieldURF_T for Classes
#endif // __DEPRECATED__
#ifdef __FIELD_CONVERSION_DEPRECATED__
/**
 *  input data type : Class
 *  ------------------------
 *
 *             \               target class type
 *  input type  \                                                                              | fixed arrays of chars  |
 *               \    FixedCString   VaryingCString    FixedWString       VaryingWString  Blob   char[]      wchar_t[]
 *
 *  std::string        capacity           X             conversion          conversion     X    capa cont.    conv
 *                     control                        capacity control                                       capa cont.
 *
 *  std::wstring       capacity          conv.                                   X         X      conv.
 *                     control                        capacity control                          capa cont.   capa cont.
 *
 *  FixedCString       capacity           X             conversion          conversion     X    capa cont.    conv
 *                     control                        capacity control                                       capa cont.
 *
 *  FixedWString       conversion       conversion                              X          X     conv.       capa cont.
 *                   capacity control                 capacity control                          capa cont.
 *
 *  VaryingCString     capacity           X             conversion          conversion     X    capa cont.    conv.
 *                     control                        capacity control                                       capa cont.
 *
 *  VaryingWString     conversion       conversion    capacity control          X          X      conv.      capa cont.
 *                   capacity control                                                           capa cont.
 *
 *  Blob                -No-             -No-               -No-               -No-        X       -No-         -No-
 *
 */
template <class _Tp>
static inline
ZStatus  setFieldURFfN_T (typename std::enable_if_t<std::is_class<_Tp>::value,_Tp> &pNatural,
                          ZDataBuffer *pURFData,        // out data in URF format (out)
                          ZTypeBase& pSourceType,       // source natural type (out)
                          uint64_t &pSourceNSize,       // source natural size(out)
                          uint64_t &pSourceUSize,       // source universal size (URF size minus header size)(out)
                          uint16_t &pSourceArrayCount,  // source array count(out)
                          ZTypeBase& pTargetType,       // target type (given by RDic)
                          uint16_t &pTargetArrayCount)  // target array count (given by RDic)
{
ZStatus wSt=ZS_SUCCESS;
ZTypeBase wTargetType,wTargetStructType,wTargetAtomicType,wSourceStructType,wSourceAtomicType ;
size_t wNSize, wUSize;
uint32_t  wRetainedArrayCount;
ZDataBuffer wUValue;


/* create URF header :
 *      for atomic data only ZTypeBase data type
 *      arrays & pointer (target type is array) :[ ZTypeBase + uint32_t (array count) reverse byte order if endian]
 *      Others : [ZTypeBase + size_t (size of Universal data) reverse byte order if endian]
 *
 */

// get ZType_type and sizes + arraycount

    wSt=_getZType_T<_Tp>(pNatural,pSourceType,pSourceNSize,pSourceUSize,pSourceArrayCount);
    if (wSt!=ZS_SUCCESS)
                    return wSt;

    printf ("%s-Class>> assigning field value from source type <%X> <%s> to target type <%X><%s>\n",
            _GET_FUNCTION_NAME_,
            pSourceType,
            decode_ZType(pSourceType),
            pTargetType,
            decode_ZType(pTargetType));

    switch (pSourceType)
        {
        case ZType_Blob:
            {
            return (setFieldURFfBlob(&pNatural,
                                     pURFData,
                                     pSourceType,
                                     pSourceNSize,
                                     pSourceUSize,
                                     pSourceArrayCount,
                                     pTargetType,
                                     pTargetArrayCount));
            }//  case ZType_StdString:
        case ZType_StdString:
            {
            return (setFieldURFfStdString(&pNatural,
                                          pURFData,
                                          pSourceNSize,
                                          pSourceUSize,
                                          pSourceArrayCount,
                                          pTargetType,
                                          pTargetArrayCount));
            }//  case ZType_StdString:
        case ZType_StdWString:
            {
            return (setFieldURFfStdWString(&pNatural,
                                           pURFData,
                                           pSourceNSize,
                                           pSourceUSize,
                                           pSourceArrayCount,
                                           pTargetType,
                                           pTargetArrayCount));

            }
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
        case ZType_ZDate:
            {
        return (setFieldURFfZDate(&pNatural,
                                  pURFData,
                                  pSourceType,
                                  pSourceNSize,
                                  pSourceUSize,
                                  pSourceArrayCount,
                                  pTargetType,
                                  pTargetArrayCount));
            }
        case ZType_ZDateFull:
            {
        return (setFieldURFfZDateFull(&pNatural,
                                      pURFData,
                                      pSourceType,
                                      pSourceNSize,
                                      pSourceUSize,
                                      pSourceArrayCount,
                                      pTargetType,
                                      pTargetArrayCount));
            }
        case ZType_CheckSum:
            {
        return (setFieldURFfCheckSum(&pNatural,
                                      pURFData,
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
            return ZS_INVTYPE;
            }
        }//switch (pSourceType)

    return wSt;

}// setFieldURF_T for Classes


ZStatus
setFieldURFfZString(void* pSourceNaturalPtr,
                    ZDataBuffer& pTargetURFData,
                    ZTypeBase pTargetType,
                    URF_Capacity_type pTargetCapacity);

#endif // __FIELD_CONVERSION_DEPRECATED__

/*ZStatus  setFieldURFfN_A (auto &pValue,
                          ZDataBuffer* pURFData,        // out data in URF format
                          ZTypeBase& pSourceType,      // [out]
                          uint64_t &pSourceNSize,       // source natural size(out)
                          uint64_t &pSourceUSize,       // source universal size (URF size minus header size)(out)
                          uint32_t &pSourceArrayCount,  // [in-out] mandatory[in] if pointer, [out] otherwise: source array count
                          ZTypeBase& pTargetType,      // {in] target type (given by RDic)
                          uint32_t &pArrayCount)        // [in] target array count (given by RDic)
{


    return setFieldURFfN_T<typeof(pValue)>(pValue,
                                           pURFData,
                                           pSourceType,
                                           pSourceNSize,
                                           pSourceUSize,
                                           pSourceArrayCount,
                                           pTargetType,
                                           pArrayCount);
}// setFieldURFfN_A
*/



template <class _Tp>
ZDataBuffer& _atomictoURF (typename std::enable_if_t<std::is_integral<_Tp>::value|std::is_floating_point<_Tp>::value,_Tp> pIn,ZDataBuffer& pOut)
{
  size_t wNaturalSize,wUniversalSize,wArrayCount;
  _Tp wValue , wValue2 ;

  unsigned char* wPtr=nullptr;

  ZTypeBase wType;
  bool wIsSigned=false;

  _getZTypeFull_T<_Tp>(pIn,wType,wNaturalSize,wUniversalSize,wArrayCount);

  wIsSigned = wType & ZType_Signed ;

  if ((wType & ZType_Endian)&&is_little_endian())    // only if system is little endian and data type is subject to endian reverse byte conversion
    wValue2= reverseByteOrder_Conditional<_Tp>(wValue2);

  wPtr=pOut.allocate(wUniversalSize+sizeof(ZTypeBase));   // unsigned means size + sign byte

  wType = reverseByteOrder_Conditional<ZTypeBase>(wType);
  memmove(wPtr,&wType,sizeof(ZTypeBase));
  wPtr += sizeof(ZTypeBase);

  wValue2 = pIn;

  if (wIsSigned)
    {
    if (pIn < 0)  // if negative value
      {
      wValue2 = -pIn;
      wPtr[0]=0; // sign byte is set to Zero
      wValue2=_negate (wValue2); // mandatory otherwise -120 is greater than -110
      }
    else
      {
      wPtr[0]=1;
      }
    wPtr++;
    }

  memmove(wPtr,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
  return pOut;
} //_atomictoURF





#endif // ZURFFROMNATURAL_H
