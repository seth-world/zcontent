#ifndef ZMETADIC_H
#define ZMETADIC_H

#include <stdlib.h> //for atexit() and uintxx_t

#include <zindexedfile/zdatatype.h>
#include <ztoolset/zarray.h>

#include <zfielddescription.h>


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


/**
 * @brief The ZMetaDic class this class holds and manage the data definition for a ZSMasterFile.
 */
class ZMetaDic : public ZArray <ZFieldDescription>
{
public:
    typedef ZArray <ZFieldDescription> _Base ;
    ZMetaDic& _copyFrom(const ZMetaDic& pIn) ;
    ZMetaDic() {}
    ~ZMetaDic() {if (CheckSum!=nullptr)delete CheckSum;}

    ZMetaDic(const ZMetaDic& pIn) {_copyFrom(pIn);}
    ZMetaDic(const ZMetaDic&& pIn) {_copyFrom(pIn);}
    ZMetaDic&  operator = (const ZMetaDic& pIn) { return _copyFrom(pIn);}


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

    void insertField(ZFieldDescription &pFieldDef,const long pRank) {insert(pFieldDef,pRank);}
    void addField(ZFieldDescription &pFieldDef){push(pFieldDef);}

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


    utf8String toXml(int pLevel);
    int fromXml(zxmlNode* pIndexRankNode,ZaiErrors* pErrorlog);

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
