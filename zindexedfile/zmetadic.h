#ifndef ZMETADIC_H
#define ZMETADIC_H
#include <zindexedfile/zdatatype.h>
#include <ztoolset/zarray.h>
#include <stdlib.h> //for atexit()

namespace zbs {

/*
struct atomicField_struct{
    ZType_type      Type;
    char            Name [cst_fieldnamelen+1];
    int16_t         Next;
    uint64_t        Offset;
    uint8_t         Size;

};
struct stringField_struct{
    ZType_type      Type;
    char            Name [cst_fieldnamelen+1];
    int16_t         Next;
    uint64_t        Offset;
    uint32_t        Size;

};
struct fieldDesc_struct{
    ZType_type      Type;
    char            Name [cst_fieldnamelen+1];
    int16_t         Next;
    uint64_t        Offset;
    uint64_t        Size;
};


union fieldDef
{
    atomicField_struct Atomic;
    stringField_struct String;
    blobField_struct   Blob;
};*/
#pragma pack(push)
#pragma pack(0)
struct FieldDesc_Export{
    ZTypeBase       ZType;
    uint32_t        ArrayCount; //< if data type is Array then count the rows 0 if not an array
    uint64_t        HeaderSize; //< Size of field header
    uint64_t        UniversalSize;  //< Only if ZType is fixed length. Otherwise set to 0
    uint64_t        NaturalSize;                    //< idem
    uint8_t         KeyEligible=false ;             //< could be set as Key field
    char            Name[cst_fieldnamelen+1+sizeof(uint16_t)];       //< Name of the field aligned to take care of utfdescString export

};
#pragma pack(pop)
/**
 * @brief The fieldDesc_struct struct Field definition for ZMetaDic
 *
 *  see ZIndexField_struct in <zindextype.h>
 *  see ZType_type
 *
 *  Nota Bene :
 *  As fields may have varying length, offet and lengths for each field are volatile data.
 *  It is available at ZRecord level within ZRecordDictionary.
 *
 *
 */
class FieldDescription
{
    FieldDescription& _copyFrom(FieldDescription& pIn)
    {
        ZType=pIn.ZType;
        Capacity=pIn.Capacity;
        HeaderSize=pIn.HeaderSize;
        UniversalSize=pIn.UniversalSize;
        NaturalSize=pIn.NaturalSize;
        KeyEligible=pIn.KeyEligible;
        Name=pIn.Name;
    }
public:
    FieldDescription() = default;
    FieldDescription (FieldDescription& pIn) { _copyFrom(pIn); }
    FieldDescription (FieldDescription&& pIn) { _copyFrom(pIn); }

    FieldDescription& operator = (FieldDescription& pIn) { _copyFrom(pIn); }
    FieldDescription& operator = (FieldDescription&& pIn) { _copyFrom(pIn); }

    ZTypeBase           ZType;
    URF_Capacity_type   Capacity; //!< if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if an atomic
    uint64_t            HeaderSize;         //!< Size of field header
    uint64_t            UniversalSize;      //!< Only if ZType is fixed length. Otherwise set to 0
    uint64_t            NaturalSize;        //!< Only if ZType is fixed length. Otherwise set to 0
    ZBool               KeyEligible=false ; //!< May be used as Key field
    utffieldNameString  Name;            //!< Name of the field



    bool isAtomic(void) {return ZType & ZType_Atomic ;}
    bool isArray(void) {return ZType & ZType_Array ;}
    bool isByteSeq(void) {return ZType & ZType_ByteSeq ;}
    bool isBlob(void) {return ZType & ZType_Blob ;}
    bool isErrored(void){return (ZType==ZType_Unknown)||(ZType&ZType_Class);}  // ZType_Class is not allowed
    bool isVarying() {return (ZType&ZType_VaryingLength);}
    bool isSigned(void) {return ZType & ZType_Signed ;}
    bool isEndian(void) {return ZType & ZType_Endian ;}

    void clear() {ZType=0; Capacity=0;HeaderSize=0;UniversalSize=0;NaturalSize=0; KeyEligible=false;Name.clear();}
    static FieldDesc_Export _exportConvert(FieldDescription&pIn,FieldDesc_Export* pOut);
    static FieldDescription _importConvert(FieldDescription& pOut,FieldDesc_Export* pIn);
};

/**
 * @brief The ZMetaDic class this class holds and manage the metadata definition for a ZSMasterFile.
 */
class ZMetaDic : public ZArray <FieldDescription>
{
public:
    ZMetaDic() {}
    ~ZMetaDic() {if (CheckSum!=nullptr)delete CheckSum;}

    checkSum *CheckSum=nullptr;

    checkSum* getCheckSum(void) {return CheckSum;}
    void generateCheckSum (void)
            {
            ZDataBuffer wMetaDic;
            _export(wMetaDic);
            if (CheckSum!=nullptr)
                    delete CheckSum;
            CheckSum=wMetaDic.newcheckSum();
            return;
            }

    void insertField(FieldDescription &pFieldDef,const long pRank) {insert(pFieldDef,pRank);}
    void addField(FieldDescription &pFieldDef){push(pFieldDef);}

    long CurrentRank=0;

    void print (FILE* pOutput=stdout);

    ZStatus addField (const utf8_t *pFieldName,
                     const ZTypeBase pType,
                     const size_t pNaturalSize,
                     const size_t pUniversalSize,
                     const URF_Array_Count_type pArrayCount);
    template <class _Tp>
    ZStatus addField_T (const utf8_t *pFieldName);
    template<class _Tp>
    ZStatus addField_T(const char *pFieldName)
    {
        return addField_T<_Tp>((const utf8_t *) pFieldName);
    }
//    ZStatus addField_A (auto pValue,const utf8_t *pFieldName);
    ZStatus removeFieldByName (const utf8_t *pFieldName);
    ZStatus removeFieldByName(const char *pFieldName)
    {
        return removeFieldByName((const utf8_t *) pFieldName);
    }
    ZStatus removeFieldByRank (const long pFieldRank);

    long searchFieldByName(const utf8_t* pFieldName) ;

    ZDataBuffer& _export(ZDataBuffer& pZDBExport) ;
    size_t _import (unsigned char* pZDBImport_Ptr);

    void clear (void) { clear(); return;}

    static size_t _computeFieldHeaderSize (const ZTypeBase pType);

    void writeXML (FILE* pOutput=stdout) ;
};


template <class _Tp> // template needs to be expanded here
ZStatus
ZMetaDic::addField_T (const utf8_t *pFieldName)
{
ZStatus wSt;
size_t wNaturalSize=0, wUniversalSize=0;
URF_Array_Count_type wArrayCount=0;
ZTypeBase wType;


    wSt=zgetZType_T<_Tp>(wType,wNaturalSize,wUniversalSize,wArrayCount);
    if (wSt!=ZS_SUCCESS)
            return wSt;

    return addField(pFieldName,wType,wNaturalSize,wUniversalSize,wArrayCount);
}//_addField
/*
ZStatus
ZMetaDic::addField_A (auto pValue, const utf8_t *pFieldName)
{
ZStatus wSt;
size_t wNaturalSize=0, wUniversalSize=0;
uint16_t wArrayCount=0;
ZTypeBase wType;

// beware typeof() may be not portable  : decltype keeps the reference and its not usable for templateStrings

    wSt=  _getZType_T<decltype(pValue)>(pValue,wType,wNaturalSize,wUniversalSize,wArrayCount);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" while getting type of field <%s>",pFieldName);
            return wSt;
            }
    return addField(pFieldName,wType,wNaturalSize,wUniversalSize,wArrayCount);
}
*/

/* ------------- C interfaces to ZMetadic -----------------------_*/


CFUNCTOR  void deleteZMetadicAll();

APICEXPORT void* createZMetaDic(void* pMetaDic);

APICEXPORT void deleteZMetaDic(void* pZRecord);

APICEXPORT ZStatus setFieldValueByRank(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);

APICEXPORT ZStatus setFieldValueByRank_Array(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);

APICEXPORT ZStatus setFieldValueByName(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);



} // namespace zbs
#endif // ZMETADIC_H
