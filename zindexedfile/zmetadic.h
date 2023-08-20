#ifndef ZMETADIC_H
#define ZMETADIC_H

#include <stdlib.h> //for atexit() and uintxx_t

#include <zindexedfile/zdatatype.h>
#include <ztoolset/zarray.h>

#include <zindexedfile/zfielddescription.h>

#include <zindexedfile/zkeydictionary.h>

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
 * @brief The ZMetaDic class this class holds and manage the data definition for a ZMasterFile.
 */
class ZMetaDic : public ZArray <ZFieldDescription>
{
public:
    typedef ZArray <ZFieldDescription> _Base ;

    utf8VaryingString DicName;         /* name of the entity described by meta dic */
    unsigned long     Version = 1000000UL;
    ZDateFull         CreationDate;
    ZDateFull         ModificationDate;
    checkSum *        CheckSum=nullptr;/* to check if meta dictionary has changed or not */   

    ZMetaDic() ;
    ~ZMetaDic() { zdelete (CheckSum) ;}

    ZMetaDic(const utf8VaryingString& pDicName) {setDicName(pDicName);}

    ZMetaDic& _copyFrom(const ZMetaDic& pIn) ;

    ZMetaDic(const ZMetaDic& pIn) {_copyFrom(pIn);}
    ZMetaDic(const ZMetaDic&& pIn) {_copyFrom(pIn);}
    ZMetaDic&  operator = (const ZMetaDic& pIn) { return _copyFrom(pIn); }


    void setDicName(const utf8String& pName) {DicName=pName;}

    void setVersion(unsigned int pVersion,unsigned int pMajor,unsigned int pMinor)
      {
      Version=pVersion*1000000;
      Version+=pMajor*1000;
      Version+=pMinor;
      }

    checkSum* getCheckSum(void) {return CheckSum;}
    void generateCheckSum (void)
            {
            ZDataBuffer wMetaDic;
            _exportAppendMetaDicFlat(wMetaDic);
            if (CheckSum!=nullptr)
                    delete CheckSum;
            CheckSum=wMetaDic.newcheckSum();
            return;
            }

    void insertField(ZFieldDescription &pFieldDef,const long pRank) {insert(pFieldDef,pRank);}
    void addField(ZFieldDescription &pFieldDef){push(pFieldDef);}

    void setModified();

    void print (FILE* pOutput=stdout);

    ZStatus addField (const utf8String &pFieldName,
                     const ZTypeBase pType,
                     const size_t pNaturalSize,
                     const size_t pUniversalSize,
                     const URF_Array_Count_type pArrayCount);
    template <class _Tp>
    ZStatus addField_T (const utf8String &pFieldName);
    ZStatus removeFieldByName (const utf8String &pFieldName);
/*    ZStatus removeFieldByName(const utf8_t *pFieldName)
    {
        return removeFieldByName((const char *) pFieldName);
    }*/
    ZStatus removeFieldByRank (const long pFieldRank);

    zrank_type searchFieldByName(const utf8String &pFieldName) const ;

    zrank_type searchFieldByHash(const md5& pHash) const ;

#define getFieldByHash searchFieldByHash
#define getFieldByName searchFieldByName

    /** @brief XmlSaveToString  saves the meta dictionary alone without defined keys */
    utf8String XmlSaveToString(bool pComment);

    utf8VaryingString toXml(int pLevel, bool pComment=false);

    ZStatus XmlLoadFromString(const utf8String &pXmlString, bool pCheckHash, ZaiErrors* pErrorLog);

    ZStatus fromXml(zxmlNode* pMetaDicRootNode, bool pCheckHash, ZaiErrors* pErrorlog);


    ZDataBuffer& _exportAppendMetaDicFlat(ZDataBuffer& pZDBExport);
    ZStatus _importMetaDicFlat(const unsigned char *&pPtrIn);

    void clear (void) { _Base::clear(); return;}

    static size_t _computeFieldHeaderSize (const ZTypeBase pType);

    void writeXML (FILE* pOutput=stdout) ;
}; // ZMetaDic


template <class _Tp> // template needs to be expanded here
ZStatus
ZMetaDic::addField_T (const utf8String& pFieldName)
{
ZStatus wSt;
size_t wNaturalSize=0, wUniversalSize=0;
URF_Array_Count_type wArrayCount=0;
ZTypeBase wType;


    wSt=_getZTypeFull_T<_Tp>(wType,wNaturalSize,wUniversalSize,wArrayCount);
    if (wSt!=ZS_SUCCESS)
            return wSt;

    return addField(pFieldName,wType,wNaturalSize,wUniversalSize,wArrayCount);
}//_addField

/* ------------- C interfaces to ZMetadic -----------------------_*/


CFUNCTOR  void deleteZMetadicAll();

APICEXPORT void* createZMetaDic(void* pMetaDic);

APICEXPORT void deleteZMetaDic(void* pZRecord);

APICEXPORT ZStatus setFieldValueByRank(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);

APICEXPORT ZStatus setFieldValueByRank_Array(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);

APICEXPORT ZStatus setFieldValueByName(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);



} // namespace zbs
#endif // ZMETADIC_H
