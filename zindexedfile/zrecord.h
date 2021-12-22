#ifndef ZRECORD_H
#define ZRECORD_H

#include <stdlib.h> //for atexit()

#include <zindexedfile/zmfconfig.h>
//#include <ztoolset/zerror.h>
#include <zindexedfile/zmetadic.h>
#include <ztoolset/zbitset.h>
#include <zindexedfile/zdatatype.h>
#include <zindexedfile/zurffromnatural.h>
#include <zindexedfile/znaturalfromurf.h>
#include <zxml/zxml.h>

#include <zindexedfile/zsindexfile.h>

#include <zindexedfile/zrawrecord.h>
#include <zindexedfile/zmfdictionary.h>

namespace zbs {
/** @addtogroup ZSMASTERFILEGROUP @{
 *
 * @weakgroup ZSMASTERFILERECORD zmasterfile record structure
 *
 * ZRecord structure is the memory representation of current's zmasterfile physical record.<br>
 *
@verbatim

            +---------+                                                 |           Physical record (Universal values)
            |         |                                                 +---------------------------------------------->
            |         |                                                  /                       ^
            | MetaDic | ----------------------------------------+      zget                      |
            |         |                                         |      /                         |
            |         |                                         |     /                          |
            |         |       +------------------------------ split : split physical record      |
            +---------+       |                               into field content vs metadic      |
                              |                                                                  |
                              |  +-------------------------------------------------------------zwrite : agregate fields contents to
                              V  |                                                             physical record according metadic
                        +-----------+   Field presence
                        |           |
                        | ZRecord   |     yes          ZDataBuffer (field URF value content)
    get/set  field      |           |     no           null
    value               |           |     yes          ZDataBuffer (field URF value content)
                            ....                        .....
                        |           |     yes          ZDataBuffer (field URF value content)
                        +-----------+
 @endverbatim
 *
 */

class recordFieldDesc
{
public:
  recordFieldDesc()=default;
  recordFieldDesc(recordFieldDesc& pIn) {_copyFrom(pIn);}
  ~recordFieldDesc()
  {
    clearURFData();
  }
  recordFieldDesc&  _copyFrom(const recordFieldDesc& pIn)
  {
    URFOffset=pIn.URFOffset;
    DataOffset=pIn.DataOffset;
    URFSize=pIn.URFSize;
    EffectiveUSize=pIn.EffectiveUSize;
    EffectiveNSize=pIn.EffectiveNSize;
    Capacity=pIn.Capacity;
    MDicField=pIn.MDicField;
    if (pIn.URFData)
      URFData=new ZDataBuffer(*pIn.URFData);
    else
      URFData=nullptr;
    return *this;
  }
  recordFieldDesc& operator =(const recordFieldDesc& pIn) {return _copyFrom(pIn);}

    uint64_t            URFOffset;              // Raw offset from beginning of record
    uint64_t            DataOffset;             // Data offset from beginning of record
    uint64_t            URFSize;                // Universal Record Format size (including field header)
    ZDataBuffer*        URFData=nullptr;        // Pointer to effective current URF data in memory if field has been loaded
//    unsigned char*        URFData=nullptr;        //!< Pointer to effective current URF data in memory if field has been loaded
    uint64_t            EffectiveUSize;         // Real Universal size for the record (only fixed length data could be anticipated)
    uint64_t            EffectiveNSize;         // Real Natural size for the record (only fixed length data could be anticipated)
    /* capacity is stored within metadic because is a stable data for the field */
    URF_Capacity_type   Capacity;               // Capacity or Real Array Count for the record (only fixed length data could be anticipated)
    ZFieldDescription* MDicField=nullptr;       // Pointer to MDicRank table content
    void clear()
    {
        MDicField=nullptr;
        URFOffset=0;
        DataOffset=0;
        URFSize=0;
        if (URFData!=nullptr)
            {
             URFData->clear();
             URFData=nullptr;
            }

        URFData=nullptr;
        EffectiveUSize=0;
        EffectiveNSize=0;
        Capacity=0;
    }
    void clearURFData()
    {
      zdelete (URFData);
    }
};

class ZMFDictionary;
/**
 * @brief The ZRecordDic class Record dictionary
 * According the fact that each record may have varying sized field and that fields may be present or not record per record,
 * It is necessary to set up a particular record dictionary.
 */
class ZRecordDic : public ZArray <recordFieldDesc>
{
public:
int64_t EffectiveRecordSize=0;
int64_t TheoricalSize=0;

    ZRecordDic(ZMFDictionary* pMasterDic);
    ~ZRecordDic();

    long getFieldByName(const utf8String& pName) const;
//    ZType_type getType(const long pRank) {return Tab[pRank].ZType;}

    void clearURFData(void);
    bool testCheckSum(void)
    {
        if (MasterDic==nullptr)
                return false;
        if (memcmp(CheckSum->content,MasterDic->CheckSum->content,sizeof(CheckSum->content)))
                        return false;
        return true;
    }

    ZMFDictionary   *MasterDic=nullptr;
    checkSum        *CheckSum=nullptr;
};


/**
@brief ZRecord structure

@par Structure of a ZRecord



         +---------> bitset: fields presence indicator (result from ZBitset::_exportURF()
         |                              +-----------------------> Field blocks
         |     _________________________|_________________________
    |.........|-----------------|----------------------------|---->
    0         n= size of bitset on bytes


@par Field blocks


               +--------------------> ZType_type (ZTypeBase-int32_t)
               |    +---------------> Data
               |    |
               |    |
    Atomic  |----|---->
            0


     Universal size is computed (USize=Array count*(universal atomic size))

               +--------------------> ZType_type (ZTypeBase-int32_t)
               |    +---------------> Unit/Array count (uint32_t)
               |    |      +--------> Data
               |    |      |
    Array   |----|----|---------->
            0    4    8

     Fixed string

               +------------------> ZType_type (ZTypeBase-uint32_t)
               |  +---------------> Canonical count (uint16_t) : capacity in units count
               |  |   +-----------> Byte size   (uint16_t) : data effective byte size
               |  |   |   +-------> Data : content
               |  |   |   |
Fixed string|----|--|--|-------->
            0    4  6  8
     Varying string

               +----------------------> ZType_type (ZTypeBase-uint32_t)
               |      +---------------> Content byte size (uint64_t)
               |      |      +--------> Data : content
               |      |      |
Varying Strg|----|--------|---------->
            0    4        12


 classes and Blob : templateString - templateWString - varyingCString - varyingWString - Blob
                     derived           derived

               +----------------------> ZType_type (ZTypeBase-uint32_t)
               |      +---------------> Sequence size (uint64_t)
               |      |      +--------> Data
               |      |      |
    ByteSeq |----|--------|---------->
            0    4        12


known fixed classes : Ex ZResource -
Known classes have a defined, known fixed size
                                                                                            derived           derived

          +----------------------> ZType_type (ZTypeBase-uint32_t)
          |
          |      +--------> Data (fields in UVF format)
          |      |
class  |----|---------->
       0    4


*/


/**
 * @brief The ZRecord Class used to set fields for a ZSMasterFile record.
 *                   Fields must have been defined within a ZMetaDic.
 *
 * - setting fields value :
 *      + setFieldValuebyRank
 *      + setFieldValuebyName (much longer)
 *
 *  Once the fields value have been set you may write ZSMasterFile record (ZRecord is a ZDataBuffer)
 *
 * - getting fields value :
 *      + getFieldbyName
 *
 *
 * @todo : add a checksum to ZMetadic to compare when using with a ZMasterFile
 *
 *
 */
class ZSMasterControlBlock;

class ZRecord : public ZRawRecord
{
public:
    ZRecord(ZSMasterFile *pMCB);
    ZRecord& operator = (const ZDataBuffer &pDataBuffer)
    {
      Content.reset();
      Content.setData(pDataBuffer);
      return(*this);
    }
    ~ZRecord() ;


    bool testCheckSum(void)
    {

        if(RDic==nullptr)
                {
                return false;
                }
        return(RDic->testCheckSum());
    }


    template <class _Tp>
    ZStatus getFieldPtrValuebyRank_T (_Tp &pNatural, const long pRank,const uint32_t pArrayCount) ;
    template <class _Tp>
    ZStatus getFieldValuebyRank_T (_Tp &pTargetNatural, const long pRank) ;
    template <class _Tp>
    ZStatus getFieldPtrValuebyName_T (_Tp &pNatural, const char* pName,const uint32_t pArrayCount) ;
    template <class _Tp>
    ZStatus getFieldValuebyName_T (_Tp &pNatural, const char* pName) ;
 /*   ZStatus getFieldValuebyRank (auto &pNatural, const long pRank,const uint32_t pArrayCount) ;
    ZStatus getFieldValuebyName (auto &pNatural, const char* pName,const uint32_t pArrayCount);
    ZStatus getFieldValuebyName (auto &pNatural, const char* pName);
*/
    unsigned char* getURFFieldFromRawRecord (unsigned char* pDataPtr,ZDataBuffer &pURFData);

    ZStatus getURFbyRank(ZDataBuffer &pOutValue, const long pRank);
    ZStatus getUniversalbyRank (ZDataBuffer &pOutValue, const long pRank, bool pTruncate=false);
#ifdef __COMMENT__
   ZStatus setFieldValuebyName (auto pValue, const char*pName) ;
    ZStatus setFieldPtrValuebyName (auto &pValue, const char*pName, uint32_t pArrayCount) ;
    ZStatus setFieldValuebyRank (auto &pValue, const long pRank);
    ZStatus setFieldPtrValuebyRank (auto &pValue, const long pRank, uint32_t pArrayCount) ;
#endif // __COMMENT__

/*    ZStatus setFirstFieldValue (auto pValue,const long pRank) {}
    ZStatus setNextFieldValue (auto pValue,const long pRank) {}
*/
    template<class _Tp>
    ZStatus setFieldPtrValuebyRank_T (_Tp& pValue,const long pRank,uint32_t pArrayCount=1);
    template<class _Tp>
    ZStatus setFieldPtrValuebyName_T (_Tp& pValue,const char* pName,uint32_t pArrayCount=1);
    template<class _Tp>
    ZStatus setFieldValuebyRank_T (_Tp& pValue,const long pRank);
    template<class _Tp>
    ZStatus setFieldValuebyName_T (_Tp& pValue,const char* pName);
/*    template<class _Tp>
    ZStatus setFieldBlobbyRank_T  (_Tp& pBlob,const long pRank);
*/

    /**
     * @brief prepareForFeed prepares ZRecord data structures for feeding fields content
     * @return a ZStatus
     */
    ZStatus prepareForFeed();

    bool testFieldPresenceByName(const utf8VaryingString& pName)
    {
      return RDic->searchFieldByName(pName);
    }
    bool testFieldPresenceByRank(const long pRank)
    {
      if (pRank<0)
        return false;
      return FieldPresence.test(pRank);
    }

    /**
     * @brief _setupRecordData extracts data fields contents from a record just been read from file.
     *  Raw data is contained within ZRawRecord
     */
    void _setupRecordData(void) ;
    /**
    * @brief _aggregate  concatenate fields into base ZDataBuffer in order to be written on file
    */
    ZStatus _aggregate(void);
    /**
       @brief _split Extracts fields from raw record according MetaDic after a ZSMasterFile get operation
     * @return
     */
    ZStatus _split(const ZDataBuffer &pContent);

    void _extractAllKeys();

    void printRecordData(FILE *pOuput=stdout);

    ZStatus RecordCheckAndMap(const ZDataBuffer& pRawContent,FILE *pOuput=stdout);

    utf8String displayRDicField(long pIdx);

    ZStatus createXMLRecord(zxmlElementPtr &wRecord);
    void writeXML(FILE* pOutput=stdout);

    ZDataBuffer& getRecordContent(void) {return (ZDataBuffer &) Content;}
    ZRecord& setRecordContent(ZDataBuffer& pZDB) { Content.setData(pZDB);  return  *this;}


    void checkFields();
    void verifyFields();

//protected:
    ZRecordDic*             RDic=nullptr;
};

ZStatus
getUniversalbyField (ZDataBuffer &pValue,bool pTruncate);



ZStatus  setFieldURFfN (void* &pSourceNatural,
                          ZDataBuffer *pTargetURFData,        // out data in URF format (out)
                          ZTypeBase& pSourceType,       // source natural type (out)
                          uint64_t &pSourceNSize,       // source natural size(out)
                          uint64_t &pSourceUSize,       // source universal size (URF size minus header size)(out)
                          uint16_t &pSourceCapacity,  // source capacity (out)
                          ZTypeBase& pTargetType,       // target type (given by RDic)
                          URF_Capacity_type &pTargetCapacity);  // target units count or array count (given by RDic)


template<class _Tp>
ZStatus
ZRecord::setFieldValuebyRank_T (_Tp &pValue, const long pRank)
{
ZStatus wSt;
ZTypeBase wSourceType;
ZDataBuffer* wURFData_Ptr=nullptr;


    if ((pRank<0)||(pRank > RDic->size()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_OUTBOUND,
                              Severity_Error,
                              "Field rank is out of record dictionary bounds");
        return ZS_OUTBOUND;
        }
    if  ((std::is_pointer<_Tp>::value))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Source array capacity value is required while requesting to convert a pointer to URF. Use <setFieldPtrValuebyRank_T> routine.");
        return ZS_INVOP;
        }
    wURFData_Ptr=new ZDataBuffer;
    wSt= setFieldURFfN_T<_Tp>(pValue,
                              wURFData_Ptr,
                              wSourceType,
                              RDic->Tab[pRank].EffectiveNSize,
                              RDic->Tab[pRank].EffectiveUSize,
                              RDic->Tab[pRank].Capacity,
                              RDic->Tab[pRank].MDicField->ZType,
                              RDic->Tab[pRank].MDicField->Capacity);
    if (wSt!=ZS_SUCCESS)
            {

            if (wSt!=ZS_FIELDCAPAOVFLW)
                                {
                                delete wURFData_Ptr;
                                return wSt;
                                }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_FIELDCAPAOVFLW,
                                  Severity_Warning,
                                  "field conversion shows a capacity overflow : source type <%s> target type <%s>.",
                                  decode_ZType(wSourceType),
                                  decode_ZType(RDic->Tab[pRank].MDicField->ZType));

            }//if (wSt!=ZS_SUCCESS)
    RDic->Tab[pRank].URFData=wURFData_Ptr;
    RDic->Tab[pRank].URFSize = wURFData_Ptr->Size;
    RDic->Tab[pRank].EffectiveUSize = wURFData_Ptr->Size - RDic->Tab[pRank].MDicField->HeaderSize;


    if (ZVerbose)
      {
      if (FieldPresence.test(pRank))
          {
          fprintf(stdout,"%s-I Field %ld %s content has been replaced  \n",
                  _GET_FUNCTION_NAME_,
                  pRank,
                (char*)RDic->Tab[pRank].MDicField->getName().toUtf());
          }
      else
          {
              fprintf (stdout,"%s-I Field %ld %s is created and declared present.\n",
                      _GET_FUNCTION_NAME_,
                      pRank,
                      (char*)RDic->Tab[pRank].MDicField->getName().toUtf());
          }
      }//if (ZVerbose)

  FieldPresence.set(pRank);

  fprintf (stdout,"%s-I After presence set Field %ld %s is declared <%s> and <%s>.\n",
          _GET_FUNCTION_NAME_,
          pRank,
          (char*)RDic->Tab[pRank].MDicField->getName().toUtf(),
          FieldPresence.test(pRank)?"present":"missing",
          RDic->Tab[pRank].URFData==nullptr?"is nullptr":"has a value");


    return ZS_SUCCESS;
}//setFieldValuebyRank



template<class _Tp>
ZStatus
ZRecord::setFieldPtrValuebyRank_T (_Tp &pValue, const long pRank, uint32_t pArrayCount)
{
ZStatus wSt;
uint64_t wUSize=0, wNSize=0;
uint32_t wArrayCount=0;
ZTypeBase wSourceType,wTargetType;
ZDataBuffer* wURFData_Ptr=nullptr;
    printf("%s>> assigning value to field rank <%ld>\n",
           _GET_FUNCTION_NAME_,
           pRank);
    if ((pRank<0)||(pRank > RDic->size()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_OUTBOUND,
                              Severity_Error,
                              "Field rank is out of record dictionary bounds");
        return ZS_OUTBOUND;
        }
    if  ((std::is_pointer<_Tp>::value)&&(pArrayCount<1))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Severe,
                              "Source array count value <%d> is invalid while requesting to convert a pointer to URF",
                              pArrayCount);
        return ZS_INVVALUE;
        }
    wURFData_Ptr=new ZDataBuffer;
    wSt= setFieldURFfN_T<_Tp>(pValue,
                              wURFData_Ptr,
                              wSourceType,
                              RDic->Tab[pRank].EffectiveNSize,
                              RDic->Tab[pRank].EffectiveUSize,
                              RDic->Tab[pRank].Capacity,
                              RDic->Tab[pRank].MDicField->ZType,
                              RDic->Tab[pRank].MDicField->Capacity);
    if (wSt!=ZS_SUCCESS)
            {

            if (wSt!=ZS_FIELDCAPAOVFLW)
                                {
                                delete wURFData_Ptr;
                                return wSt;
                                }
            fprintf (stderr,
                     "%s> Warning: field conversion shows a capacity overflow : source type <%s> target type <%s>\n",
                     _GET_FUNCTION_NAME_,
                     decode_ZType(wSourceType),
                     decode_ZType(RDic->Tab[pRank].MDicField->ZType));
            }
    RDic->Tab[pRank].URFData=wURFData_Ptr;
    RDic->Tab[pRank].URFSize = wURFData_Ptr->Size;
    RDic->Tab[pRank].EffectiveUSize = wURFData_Ptr->Size - RDic->Tab[pRank].MDicField->HeaderSize;
    if (ZVerbose)
        if (FieldPresence.test(pRank))
        {
        fprintf(stdout,"%s-W Field content has been replaced : rank %ld \n",
                _GET_FUNCTION_NAME_,
                pRank);
        }

    FieldPresence.set(pRank);
    return ZS_SUCCESS;
}//setFieldValuebyRank


template<class _Tp>
ZStatus
ZRecord::setFieldPtrValuebyName_T (_Tp& pValue, const char* pName,uint32_t pArrayCount)
{
    printf("%s>> assigning value to field name <%ls>\n",
           _GET_FUNCTION_NAME_,
           pName);
    long wRank=RDic->getFieldByName(pName);
    if (wRank<0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_NOTFOUND,
                                  Severity_Error,
                                  "Field name <%s> has not been found within record dictionary");
            return ZS_NOTFOUND;
            }
    return setFieldPtrValuebyRank_T<_Tp>(pValue,wRank,pArrayCount);

}//setFieldPtrValuebyName

template<class _Tp>
ZStatus
ZRecord::setFieldValuebyName_T (_Tp& pValue, const char* pName)
{
    if (ZVerbose)
            printf("%s>> assigning value to field <%s>\n",
           _GET_FUNCTION_NAME_,
           pName);
    long wRank=RDic->getFieldByName(pName);
    if (wRank<0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_NOTFOUND,
                                  Severity_Error,
                                  "Field name <%s> has not been found within record dictionary",
                                  pName);
            return ZS_NOTFOUND;
            }
    return setFieldValuebyRank_T<_Tp>(pValue,wRank);

}//setFieldValuebyName
#ifdef __COMMENT__
ZStatus
ZRecord::setFieldValuebyRank (auto &pValue, const long pRank)
{
  return setFieldValuebyRank_T<typeof(pValue)>(&pValue,pRank);
}
/**
 * @brief ZRecord::setFieldPtrValuebyRank sets the field value for a pointer
 * @param pValue
 * @param pRank
 * @param pArrayCount
 * @return
 */
ZStatus
ZRecord::setFieldPtrValuebyRank (auto &pValue, const long pRank, uint32_t pArrayCount)
{
  return setFieldPtrValuebyRank_T<typeof(pValue)>(&pValue,pRank,pArrayCount);
}
ZStatus
ZRecord::setFieldValuebyName (auto pValue,const char* pName)
{
  return setFieldValuebyName_T<decltype(pValue)>(pValue,pName);
}

ZStatus
ZRecord::setFieldPtrValuebyName (auto &pValue,const char* pName,uint32_t pArrayCount)
{
  return setFieldPtrValuebyName_T<typeof(pValue)>(pValue,pName,pArrayCount);
}
#endif//#ifdef __COMMENT__
//======================get field value : obtain the value of a field stored within a ZRecord========================================


template<class _Tp>
/**
 * @brief ZRecord::getFieldValuebyName_T gets the value of a record field defined by metadic from current record content.
 * @param pValue    variable that will receive the fields content
 * @param pName     name of the field. Name is searched within metadic to get field metadic definition.
 * @return
 */
ZStatus
ZRecord::getFieldValuebyName_T (_Tp& pValue, const char* pName)
{
    printf("%s>> getting value from field name <%s>\n",
           _GET_FUNCTION_NAME_,
           pName);
    long wRank=RDic->getFieldByName(pName);
    if (wRank<0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_NOTFOUND,
                                  Severity_Error,
                                  "Field name <%s> has not been found within record dictionary");
            return ZS_NOTFOUND;
            }
    return getFieldValuebyRank_T<_Tp>(pValue,wRank);

}//getFieldValuebyName_T

#ifdef __COMMENT__

ZStatus
ZRecord::getFieldValuebyName (auto pValue,const char* pName)
{

  return getFieldValuebyName_T<typeof(pValue)>(pValue,pName);
}

#endif //#ifdef __COMMENT__
template <class _Tp>
/**
 * @brief ZRecord::getFieldbyRank User routine to get the natural value of a field from a record into a given data.<br>
 *  output data type is checked to be compatible with source data<br>
 *  source data is eventually converted to output data format<br>
 *  endian/sign conversion is made
 *
 * @param pNatural
 * @param pRank
 * @return
 */
ZStatus
ZRecord::getFieldValuebyRank_T (_Tp& pTargetNatural, const long pRank)
{

    if ((pRank<0)||(pRank>RDic->size()))
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_OUTBOUND,
                                      Severity_Severe,
                                      "trying to access field rank out of record dictionary boundaries");
                return ZS_OUTBOUND;
                }
    if ((pRank>=FieldPresence.EffectiveBitSize)||(!FieldPresence.test(pRank)))
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
    printf("%s>> getting value from record for field rank <%ld> dicitionary name is <%s>\n",
           _GET_FUNCTION_NAME_,
           pRank,
           (const char*)RDic->Tab[pRank].MDicField->getName().toCChar());

uint64_t    wTargetNSize, wTargetUSize;
uint16_t    wTargetUnitsCount;
uint32_t    wArrayCount;

ZTypeBase   wTargetType,wSourceType,wTypeStruct,wTypeAtomic;
uint64_t    wOffset = RDic->Tab[pRank].URFOffset, wDataOffset = RDic->Tab[pRank].DataOffset;


    _getZType_T<_Tp>(pTargetNatural,wTargetType,wTargetNSize,wTargetUSize,wTargetUnitsCount);

    wSourceType = RDic->Tab[pRank].MDicField->ZType;

    wTypeStruct=wSourceType&ZType_StructureMask;
    wTypeAtomic=wSourceType&ZType_Atomic;



    return importFieldfURF_T<_Tp>(pTargetNatural,
                             RDic->Tab[pRank].URFData,
                             wTargetType,
                             wTargetNSize,
                             wTargetUSize,
                             wTargetUnitsCount);
}//getFieldbyRank



#ifdef __OLD_VERSION__
template <class _Tp>
/**
 * @brief ZRecord::getFieldbyRank User routine to get the natural value of a field into a given data
 *  output data type is checked to be compatible with source data
 *  source data is eventually converted to output data format
 *  endian/sign conversion is made
 *
 * @param pNatural
 * @param pRank
 * @return
 */
ZStatus
ZRecord::getFieldValuebyRank_T (_Tp& pNatural, const long pRank)
{
    printf("%s>> getting value from field rank <%ld>\n",
           _GET_FUNCTION_NAME_,
           pRank);
    if ((pRank<0)||(pRank>RDic->size()))
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_OUTBOUND,
                                      Severity_Severe,
                                      "trying to access field rank out of record dictionary boundaries");
                return ZS_OUTBOUND;
                }
    if ((pRank>=FieldPresence.EffectiveBitSize)||(!FieldPresence.test(pRank)))
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

ZStatus     wSt;
uint64_t    wTargetNSize, wTargetUSize;
uint16_t    wTargetArrayCount;
ZDataBuffer wUValue;
ZTypeBase   wTargetType,wSourceType,wTypeStruct,wTypeAtomic;
uint64_t    wOffset = RDic->Tab[pRank].URFOffset, wDataOffset = RDic->Tab[pRank].DataOffset;


    wSourceType = RDic->Tab[pRank].MDicRank->ZType;

    wTypeStruct=wSourceType&ZType_StructureMask;
    wTypeAtomic=wSourceType&ZType_Atomic;


    return setFieldNfURF_T<_Tp>(pNatural,
                             RDic->Tab[pRank].URFData->Data,
                             wTargetType,
                             wTargetNSize,
                             wTargetUSize,
                             wTargetArrayCount);
}//getFieldbyRank
#endif // __OLD_VERSION__

#ifdef __COMMENT__
/*
* @param[in] pInData       Record buffer to extract field from
* @param[out] pOutData     Field content extracted from key and formatted back to Natural (computer) data type
* @param[in] pZType        Type mask of data ( ZType_type )
* @return              The pure natural value of the field converted from Key field content (a reference to pOutData ).
*/
template <class _Tp>
static inline
_Tp _setFieldAtomicUfN_T_Ptr(unsigned char* pInData, ZDataBuffer &pOutData,const ZTypeBase pType )
{
   _Tp wValue , wValue2;

   memmove(&wValue,pInData,sizeof(wValue));

   if (wValue < 0)
               wValue2 = -wValue;
           else
               wValue2 = wValue;

   if (is_little_endian())             // only if system is little endian
         if (pType & ZType_Endian)    // and data type is subject to endian reverse byte conversion
                       wValue2= _reverseByteOrder_T<_Tp>(wValue2);

   if (pType & ZType_Signed)
           {

           pOutData.allocate(sizeof(_Tp)+1);   // unsigned means size + sign byte
           if (wValue<0)  // if negative value
                   {
                   pOutData.Data[0]=0; // sign byte is set to Zero
                   _negate (wValue2); // mandatory otherwise -120 is greater than -110
                   }
               else
                   pOutData.Data[0]=1;

           memmove(pOutData.Data+1,&wValue2,sizeof(_Tp)); // skip the sign byte and store value (absolute value)
           return wValue;
           }
// up to here : unsigned data type

//    pOutData->setData(&wValue,sizeof(_Tp));// not deduced for sizeof(_Tp) because of Arrays
   pOutData.setData(&wValue,getAtomicUniversalSize(pType));// unsigned means same size as input data type
   return wValue;
} // _setFieldAtomicUfN_T


/**
 * @brief _getArrayUfN Converts an array of Natural atomic values to  a ZDataBuffer containing a Universal

Uses _getArrayUfN template routine to pack field according system constraints (big / little endian) and leading sign byte.

Size of the returned ZDataBuffer content is

- size of Atomic data + 1 byte (leading sign byte) if signed value type,
- size of Atomic data if unsigned value type.

 * @param pInData   ZDataBuffer containing the user record to extract field from.
 * @param pOutData  ZDataBuffer containing as a return the packed/extracted field value converted as ready to be used as a key field
 * @param pType     ZType_type of the data to convert
 * @param pArrayCount      Optional output argument : computed number of array ranks
 * @param pUniversalSize  Optional output argument : universal data format size
 * @return          A ZStatus
 */
template <class _Tp>
static inline
ZStatus _setFieldArrayUfN_T_Ptr(_Tp &pValue,
                                ZDataBuffer &pUniversal,
                                const ZType_type pType,
                                const long pArrayCount)  //! mandatory input
{

 ZTypeBase wZType = pType;

 if (!(wZType & ZType_Array))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Severe,
                                 "Invalid ZType <%ld> <%s> encountered while processing field data. Type is NOT ARRAY.",
                                 pType,
                                 decode_ZType(pType));
         return ZS_INVTYPE;
        }
 wZType = wZType & ZType_AtomicMask ;  // negate ZType_Array : get the atomic data type

 long wEltOffsetIn=0;
 size_t wEltNSize = getAtomicNaturalSize(pType);
 ZDataBuffer wDBElt;

    pUniversal.clear();
    for (long wi=0;wi <  pArrayCount ;wi++)
        {
        _getdAtomicUfN_T_Ptr<_Tp>(pInData_Ptr+ wEltOffsetIn,wDBElt,pType) ;
        pUniversal.appendData(wDBElt);
        wEltOffsetIn+=wEltNSize;
        }

  return ZS_SUCCESS;
}//_setFieldArrayUfN_T_Ptr


template <class _Tp>
static inline
ZStatus _setFieldByteSeqUfN_T(unsigned char* pInData_Ptr,
                                ZDataBuffer &pOutData,
                                const ZType_type pType)
{
 ZTypeBase wZType = pType;

 if (!(wZType & ZType_ByteSeq))
        {
         ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVTYPE,
                                 Severity_Severe,
                                 "Invalid ZType <%ld> <%s> encountered while processing field data. Type is NOT ZType_ByteSeq.",
                                 pType,
                                 decode_ZType(pType));
         return ZS_INVTYPE;
        }


    wZType = wZType & ZType_StructureMask ;  //

    if (pType & ZType_bitset)
        {
        ZBitset* wData=static_cast<ZBitset*>(pInData_Ptr);
        pOutData.setData(wData->bit,(int64_t)(wData->Size*wData->UnitByteSize));
        return ZS_SUCCESS;
        }
    if (pType & ZType_Blob)
        {
        ZDataBuffer* wData=static_cast<ZDataBuffer*>(pInData_Ptr);
        pOutData.setData(wData->Data,(int64_t)wData->Size);
        return ZS_SUCCESS;
        }

  return ZS_SUCCESS;
}//_setFieldByteSeqUfN_T_Ptr
#endif// __COMMENT__


//==========================Conversion From URF to Natural==========================================================



} // namespace zbs

/** @} */

/* --------------------------C interfaces to ZRecord------------------------------------
 */


CFUNCTOR  void deleteZRecordAll();

APICEXPORT void* createZRecord(void* pZMF);

APICEXPORT void deleteZRecord(void* pZRecord);

APICEXPORT ZStatus setFieldValueByRank(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);

APICEXPORT ZStatus setFieldValueByRank_Array(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);

APICEXPORT ZStatus setFieldValueByName(void* pRecordCTX,void* pValue,const long pRank,uint32_t pType);


#endif // ZRECORD_H
