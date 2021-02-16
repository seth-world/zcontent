#ifndef ZSINDEXTYPE_H
#define ZSINDEXTYPE_H
#include <zconfig.h>

#include <ztoolset/zerror.h>
#include <ztoolset/zbasedatatypes.h>
#include <ztoolset/zexceptionmin.h>
#include <zrandomfile/zrandomfiletypes.h>
#include <cstdint>


#define __ZMF_VERSION__ 3000UL
#define __ZIF_VERSION__ 3000UL

#include <zindexedfile/zdatatype.h>
#include <zindexedfile/zrecord.h>

/**
    @addtogroup ZSIndexGroup
    @{ */

//                   ZType_type see <zindexedfile/zdatatype.h>


using namespace zbs ;

/** @cond Development */
/*
template <class _Tp>
ZStatus
_addFieldToZSKDic (const utf8_t *pFieldName,const size_t pOffset, ZSKeyDictionary &pZKDic);
*/
/*
template <class _Tp>
ZStatus ZSKeyDictionary::zaddField (const char *pFieldName,size_t pZMFOffset)
{
    return(_addFieldToZSKDic<_Tp>(pFieldName,pZMFOffset,*this));
}
*/
//---------- for data conversion to and from universal format see <zindexfile/datatype.h>---------



//----------------------ZDictionary routines---------------------------------------------------


/** @brief _addFieldToZDic : Adds a Field definition to a Key dictionary (fields description list).
 *
 * - Analyzes the type of data using zgetZType() function.
 * - Deduces the data type (ZType_type) and true natural data length as well as internal data length.
 *
 * @note template is required here because 'auto' type hides arrays (char [n] is converted to char*).
 *
 * @param[in] pFieldName user name of the field. Only for readability.
 * @param[in] pOffset    offset of the field IN THE USER RECORD (beginning = 0)
 * @param[in-out] pZKDic a ZDictionay : ZArray of ZIndexField_struct to which field definition will be added
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 *
 */
#ifdef __COMMENT__   // ZKeyDictionary is only a pointer to MetaDic
template <class _Tp> // template needs to be expanded here
static inline
ZStatus
_addFieldToZSKDic (const char *pFieldName,const size_t pOffset, ZSKeyDictionary &pZKDic)
{
ZStatus wSt;

    ZSIndexField_struct wField;
    if (pFieldName==nullptr)
            wField.Name.content[0]='\0';
        else
            wField.Name = pFieldName;

    wField.RecordOffset = pOffset;

    wSt=zgetZType_T<_Tp>(wField.ZType,wField.NaturalSize,wField.UniversalSize,wField.ArrayCount,true);
    if (wSt!=ZS_SUCCESS)
            return wSt;

    pZKDic.push(wField);
    pZKDic._reComputeSize();
    return ZS_SUCCESS;
}//_addFieldToZDic
/** @brief _addFieldToZDic : Adds a Field definition to a Key dictionary (fields description list).
 *
 * - Analyzes the type of data using zgetZType() function.
 * - Deduces the data type (ZType_type) and true natural data length as well as internal data length.
 *
 * @note template is required here because 'auto' type hides arrays (char [n] is converted to char*).
 *
 * @param[in] pFieldName user name of the field. Only for readability.
 * @param[in] pOffset    offset of the field IN THE USER RECORD (beginning = 0)
 * @param[in-out] pZKDic a ZDictionay : ZArray of ZIndexField_struct to which field definition will be added
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 *
 */

template <class _Tp> // template needs to be expanded here
static inline
ZStatus
_addFieldToZSKDic (descString &pFieldName,const size_t pOffset, ZSKeyDictionary &pZKDic)
{
ZStatus wSt;

    ZSIndexField_struct wField;
    if (pFieldName==nullptr)
            wField.Name.content[0]='\0';
        else
            wField.Name = pFieldName;

    wField.RecordOffset = pOffset;

    wSt=zgetZType_T<_Tp>(wField.ZType,wField.NaturalSize,wField.UniversalSize,wField.ArrayCount);
    if (wSt!=ZS_SUCCESS)
            return wSt;

    pZKDic.push(wField);
    pZKDic._reComputeSize();
    return ZS_SUCCESS;
}//_addFieldToZDic
//-----------------------Get Type of data --------------------------------------------------
#endif // __COMMENT__

//-------- Unitary (Atomic) Key fields definition-------------------------
class ZSIndexField;
class ZSKeyDictionary;

template <class _Tp>
ZStatus
_getAtomicZTypeT(const size_t pTypeHashCode,ZTypeBase& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize);

//------------Gets the effective fields values from a ZMF user record---------------------------------


ZStatus _getAtomicFromRecord(ZDataBuffer &pInData, ZDataBuffer &pOutData, ZSIndexField & pField);
ZDataBuffer& _getArrayFromRecord(ZDataBuffer &pInData, ZDataBuffer &pOutData, ZSIndexField & pField);

void _getValueFromStdString (void *pIndata, ZDataBuffer &pOutData);

template <class _Tp>
ZStatus _getByteSequenceFromRecord (_Tp pInData,const ssize_t pSize,ZDataBuffer &pOutData, ZSIndexField& pField);


//----------Gets a natural value from a key content----------------------------------

ZStatus _getFieldValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZSKeyDictionary & pFieldList);
inline
ZStatus _getAtomicValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZSKeyDictionary & pFieldList);
inline
ZStatus _getArrayValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZSKeyDictionary & pFieldList);
inline
ZStatus _getClassValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &AVFKValue,const long pRank, ZSKeyDictionary & pFieldList);


//-------------Functions----------------------------------
//!
bool is_little_endian(void);

ZDataBuffer& _printAtomicValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &pOutValue,const long pRank, ZSKeyDictionary & pFieldList);
ZDataBuffer& _printArrayValueFromKey(ZDataBuffer &pKeyData, ZDataBuffer &pOutValue, const long pRank, ZSKeyDictionary & pKFieldList);

class ZSKey;

template <class _Tp>
ZStatus
zsetKeyFieldValue (_Tp pValue, ZSKey & pZKey,const long pFieldRank,ssize_t pArraySize);

template <class _Tp>
static inline
ZStatus
_setKeyFieldValue ( ZSKey &pZKey,const long pFieldRank,_Tp pValue,const long pArrayNumber);

//
//  Key fields description
//

#ifndef ZIFKEYTYPE
#define ZIFKEYTYPE
enum ZIFKeyType_type : uint8_t
{
    ZKT_Nothing     =0 ,
//    ZKT_USASCII     =0x20,     //
    ZKT_UTF8        =1,         // by default key is UTF8
    ZKT_UTF16       =2,         // Warning UTF16 is subject to LE/BE conversion - USC2, UTF16
    ZKT_UTF32       =4,         // Warning UTF32 is subject to LE/BE conversion - USC4, UTF32
    ZKT_Binary      =8,         // subject to Little Endian/Big Endian conversion according data type
    ZKT_Compound    =0x10
};

#endif // ZIFKEYTYPE



#ifndef ZSINDEXCONTROLBLOCK
#define ZSINDEXCONTROLBLOCK

#endif // ZSINDEXCONTROLBLOCK
/** @}*/ //ZSIndexTypeGroup







//------------Functions-----------------------------------------
template <class _Tp>
ZStatus
//getAtomicZType(const size_t pTypeHashCode,long& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize);
getAtomicZType(_Tp pValue, ZTypeBase& pType, uint64_t &pNaturalSize, uint64_t &pUniversalSize);



#endif // ZSINDEXTYPE_H
