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

typedef utfdescString ZFieldName_type ;

#pragma pack(push)
#pragma pack(0)
struct ZSIndexField_strOut
{
    size_t      MDicRank;       //!< reference to Metadictionary row
    uint64_t    NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
    uint64_t    UniversalSize;  //!< length of the field when stored into Key (Field dictionary internal format size)
    uint32_t    ArrayCount;      //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.
    ZTypeBase   ZType;           //!< Type mask of the Field @see ZType_type

    ZDataBuffer& _export(ZDataBuffer& pZDBExport);

};
#pragma pack(pop)

/**

 * @brief ZSIndexField_struct Dictionary single field definition  :
 *              one key may be composed of many single fields not necessarily contiguous in the master file's data structure

    ZType : ZType is a mask of ZType_type describing the data field type (Atomic data type, signed or not, storage type (Atomic, Array, Class, Pointer), etc.)

    @note  Field size requires two different values :
    - NaturalSize represents data type size as it is stored originally by computer data type (natural format). It is also called External format.
    - KeySize represents data type size as it is stored within ZIndexFile key (key format). It is also called Internal format.

    1. Atomic, Array and Class : Field size fields represent the overall size of the data field : for an array, for instance this is the sum of the array element sizes.
    2. Pointers : ZType mask is set to the atomic data Pointer points to.
        - for pointers to char and uchar : NaturalSize and KeySize are set to the string size obtained with strlen.
          This implies that any pointer to char or uchar MUST BE zero terminated.
        - for pointers to other types : it is considered as pointer to Atomic data.
          So that, Natural size and KeySize are set accordingly to the values corresponding to ZType_type of Atomic data pointer points to.

 */

struct ZSIndexField_struct
{
    size_t      MDicRank;       //!< reference to Metadictionary row
    uint64_t    NaturalSize;    //!< Length of the Field to extract from the record : natural size is the canonical data size and not the size of the data once it has been reprocessed for key usage. @note Special case for char and uchar : Sizes are equal to Cstring size (obtained with strlen).
    uint64_t    UniversalSize;  //!< length of the field when stored into Key (Field dictionary internal format size)
    uint32_t    ArrayCount;     //!< in case of ZType is ZType_Array : number of elements in the Array(Atomic size can be computed then using NaturalSize / ArraySize or KeySize / ArraySize ). For other storage type, this field is set to 1.
    ZTypeBase   ZType;          //!< Type mask of the Field @see ZType_type
    uint64_t    KeyOffset;      //!< Offset of the Field from the beginning of Key record
//    ZFieldName_type  Name;      //!< short string (max 15 char) containing the name of the field (for readability purpose, not necessary for key processing)

/*    bool isAtomic(void) {return ZType & ZType_Atomic ;}
    bool isArray(void) {return ZType & ZType_Array ;}
    bool isSigned(void) {return ZType & ZType_Signed ;}
    bool isEndian(void) {return ZType & ZType_Endian ;}
    bool isUnknown(void){return (ZType==ZType_Unknown);}*/

    ZDataBuffer& _export(ZDataBuffer& pZDBExport);

    static ZSIndexField_strOut _exportConvert(ZSIndexField_struct& pIn, ZSIndexField_strOut *pOut);
    static ZSIndexField_struct _importConvert(ZSIndexField_struct& pOut, ZSIndexField_strOut *pIn);

    ZSIndexField_struct& _import(unsigned char* pZDBImport_Ptr);

};



#ifndef __KEYDICTYPE__
#define __KEYDICTYPE__
enum ZKeyDic_type {
    ZKD_Nothing         = 0,
    ZKD_NaturalSize     = 1,
    ZKD_UniversalSize   = 2,
    ZKD_KeyOffset       = 4,
    ZKD_RecordOffset    = 8,
    ZKD_ArraySize       = 0x10,
    ZKD_ZType           = 0x20,
    ZKD_Name            = 0x40
};
#endif //__KEYDICTYPE__
/**
 * @brief The ZSKeyDictionary class store ZIndex index key fields definition for one key (and therefore one ZIndexFile)
 *
 * ZSKeyDictionary is only a set of pointers to meta dictionary rank.
 * However, as Fields may have varying length it is necessary to use not the meta dictionary itself but the record dictionary
 *
 *
 * Nota Bene: when exporting, only ZArray of fields definition are exported in universal format
 *            when importing, total Natural size and Universal size are computed.
 *
 */

//class ZSKeyDictionary : public zbs::ZArray<ZSIndexField_struct>
class ZSKeyDictionary : public ZArray<ZSIndexField_struct>
{
typedef zbs::ZArray<ZSIndexField_struct> _Base;


public:
    ZSKeyDictionary(ZMetaDic*pMDic) {MetaDic=pMDic;}
    ~ZSKeyDictionary() { } // just to call the base destructor
    ZMetaDic* MetaDic=nullptr; //!< Record Dictionary to which Key Dictionary refers
    size_t KDicSize;          //!< Size of the dictionary when exported (other fields are not exported) this field comes first
    size_t KeyNaturalSize ;      //!< overall size of the key (sum of keyfields NATURAL (total) length).
    size_t KeyUniversalSize ;    //!< total of key size with internal format
    bool   Recomputed=false;
//    using _Base::newBlankElement;

    ssize_t fieldKeyOffset (const long pRank);      //< compute offset from beginning of key record for a given field rank with KDic
    ssize_t fieldRecordOffset (const long pRank);

    ZTypeBase getType(const long pKFieldRank)
        {
        long wRank =Tab[pKFieldRank].MDicRank;
        return (MetaDic->Tab[wRank].ZType);
        }
    uint64_t getUniversalSize(const long pKFieldRank)
        {
        long wRank =Tab[pKFieldRank].MDicRank;
        return (MetaDic->Tab[wRank].UniversalSize);
        }
    uint64_t getNaturalSize(const long pKFieldRank)
        {
        long wRank =Tab[pKFieldRank].MDicRank;
        return (MetaDic->Tab[wRank].NaturalSize);
        }


    ZStatus addFieldToZDicByName (const utf8_t *pFieldName);
    ZStatus addFieldToZDicByRank (const zrank_type pMDicRank);

//    ~CZKeyDictionary() {this->_Base::~ZArray<ZIndexField_struct>(); }

    void print (FILE* pOutput=stdout);

    void _reComputeSize (void) ;

    ZStatus zremoveField (const long pKDicRank);


/*    template <class _Tp>
    ZStatus zaddField (const char *pFieldName,const size_t pZMFOffset);
    template <class _Tp>
    ZStatus zaddField (descString &pFieldName,const size_t pZMFOffset);
    ZStatus zremoveField (const char *pFieldName);
    ZStatus zremoveField (const long pFieldRank);
    ZStatus zsetField (const long pFieldRank,ZKeyDic_type pZKD,auto pValue);
*/
    long zsearchFieldByName(const utf8_t* pFieldName) ;
    long zsearchFieldByName(utfdescString &pFieldName) ;

    void clear (void) {KeyNaturalSize=0; KeyUniversalSize=0; _Base::clear(); return;}

    ZDataBuffer& _export(ZDataBuffer& pZDBExport);
    size_t _import(unsigned char* pZDBImport_Ptr);

} ;


/** @cond Development */

template <class _Tp>
ZStatus
_addFieldToZSKDic (const utf8_t *pFieldName,const size_t pOffset, ZSKeyDictionary &pZKDic);
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

template <class _Tp>
ZStatus
_getAtomicZTypeT(const size_t pTypeHashCode,ZTypeBase& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize);

//------------Gets the effective fields values from a ZMF user record---------------------------------


ZStatus _getAtomicFromRecord(ZDataBuffer &pInData, ZDataBuffer &pOutData, ZSIndexField_struct & pField);
ZDataBuffer& _getArrayFromRecord(ZDataBuffer &pInData, ZDataBuffer &pOutData, ZSIndexField_struct & pField);

void _getValueFromStdString (void *pIndata, ZDataBuffer &pOutData);

template <class _Tp>
ZStatus _getByteSequenceFromRecord (_Tp pInData,const ssize_t pSize,ZDataBuffer &pOutData, ZSIndexField_struct& pField);


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
/**
 * @brief The ZIndexControlBlock class  Defines the operational data that describes one index.
 *
 * This infraData is stored in the ZSIndexFile Header AND in the ZSMasterFile index collection within its reserved Header block (ZMasterControlBlock)
 */
#pragma pack(push)
#pragma pack(0)
struct ZSICBOwnData_Export{
    uint32_t                StartSign=cst_ZSTART ;         //!< ZICB block start marker
    ZBlockID                BlockID;            //!< must be ZBID_ICB
    unsigned long           ZMFVersion;         //!< Self explainatory
    size_t                  ICBTotalSize;       //!< ICB total size when written in file header (ZReserved header field size)
    size_t                  ZKDicOffset;        //!< Offset to ZKey dictionary
    ZIFKeyType_type         KeyType;            //!< RFFU
    uint8_t                 AutoRebuild=false;  //!< RFFU
    ZSort_Type              Duplicates;         //!< Index key is allowing duplicates (ZST_DUPLICATES) or is not allowing duplicates (ZST_NODUPLICATES)
//    char*                    Name=nullptr;      //!< Name for the index
//    uint32_t                EndSign ;        //!< ZICB block end marker --NO--
    void clear();
};
#pragma pack(pop)

struct ZSICBOwnData{
//    uint32_t                StartSign=cst_ZSTART;//!< ZICB block start marker
//    ZBlockID                BlockID;            //!< must be ZBID_ICB
//    unsigned long           ZMFVersion;         //!< Self explainatory
    size_t                  ICBTotalSize;      //!< ICB (ZSICBOwnData_Export+ Name varying string length + ZKDic export size) size when written in file header (ZReserved header field)
    size_t                  ZKDicOffset;       //!< offset to ZDictionary (taking varying sized Index Name into account)

    ZIFKeyType_type         KeyType;            //!< RFFU
    uint8_t                 AutoRebuild=false;  //!< RFFU
    ZSort_Type              Duplicates;         //!< Index key is allowing duplicates (ZST_DUPLICATES) or is not allowing duplicates (ZST_NODUPLICATES)
    utffieldNameString      Name;               //!< Name for the index
//    uint32_t                EndSign=cst_ZEND ;  //!< ZICB block end marker

    ZDataBuffer& _exportICBOwn(ZDataBuffer& pZDBExport);
    ZStatus _importICBOwn(unsigned char* pZDBImport_Ptr);
    void clear() {ICBTotalSize=0; ZKDicOffset=0;AutoRebuild=false; Duplicates= ZST_NODUPLICATES; Name.clear();}
};

class ZSIndexControlBlock : public ZSICBOwnData
{
public:
    ZSIndexControlBlock(ZMetaDic *pMetaDic) ;
    ~ZSIndexControlBlock(void);

    ZSKeyDictionary         *ZKDic = nullptr ;  //< Fields dictionary for the key :
                                                //for memory management reasons it is a pointer that is instantiated by new and destroyed when ZICB rank is removed
    checkSum*               CheckSum=nullptr;
    ZMetaDic*               MetaDic;
    void clear(ZMetaDic *pMetaDic);

    ZDataBuffer&_exportICB (ZDataBuffer &pICBContent) ;
    ZStatus     _importICB (ZMetaDic* pMetaDic,ZDataBuffer &pRawICB, size_t &pImportedSize, size_t pOffset=0) ;
    ZStatus     _importICB (ZMetaDic* pMetaDic,unsigned char* pBuffer, size_t &pImportedSize);

    ZStatus     zkeyValueExtraction (ZRecord *pRecord, ZDataBuffer& pKey);

    // generates checksum for Index Control block on ZSMasterFile side

    void generateCheckSum(void) {ZDataBuffer ICBExport; CheckSum=_exportICB(ICBExport).newcheckSum(); return;}

/*    template <class _Tp>

    ZStatus     zaddFieldToZKDic (const char *pFieldName)
    {
        ZStatus wSt;
        wSt=ZKDic->zaddField<_Tp>(pFieldName);
        if (wSt!=ZS_SUCCESS)
                return wSt;
        return wSt;
    }
*/
   ssize_t IndexRecordSize (void)   //< computes and return the effective size of a ZIndex key record
                                    // variable fields length impose to compute record size at record level
            {   ZKDic->_reComputeSize();
                return (ZKDic->KeyUniversalSize + sizeof(zaddress_type)); }

};

#endif // ZSINDEXCONTROLBLOCK
/** @}*/ //ZSIndexTypeGroup







//------------Functions-----------------------------------------
template <class _Tp>
ZStatus
//getAtomicZType(const size_t pTypeHashCode,long& pType,ssize_t& pNaturalSize,ssize_t& pKeyFieldSize);
getAtomicZType(_Tp pValue, ZTypeBase& pType, uint64_t &pNaturalSize, uint64_t &pUniversalSize);



#endif // ZSINDEXTYPE_H
