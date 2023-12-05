#ifndef URFPARSER_H
#define URFPARSER_H


#include <stdint.h>

#include <ztoolset/zstatus.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zbitset.h>
#include <ztoolset/zexceptionmin.h>

#include "urffield.h"


enum CompareOptions : uint8_t {
  COPT_Nothing = 0,
  COPT_DropAccute = 1,
  COPT_UpCase     = 2,
  COPT_Default    = COPT_DropAccute | COPT_UpCase
};


namespace zbs {
class ZMetaDic;
}

#ifndef __HASH_TIME__
#define __HASH_TIME__

class ZHash
{
public:
    ZHash() { clear(); }
    ZHash(const ZHash& pIn) {_copyFrom(pIn);}

    ZHash& _copyFrom(const ZHash& pIn)
    {
        Value = pIn.Value;
        return *this;
    }
    void clear()
    {
        Value=0;
    }

    ZHash& operator = (const ZHash& pIn) {return _copyFrom(pIn); }

    bool operator == (const ZHash& pIn) {return (Value==pIn.Value)  ; }

    unsigned long Value=0;
};

#endif //  __HASH_TIME__


enum URFParserState
{
    URFPS_Nothing = 0,
    URFPS_RecordSet = 1,
    URFPS_PartiallyParsed = 2,
    URFPS_TotallyParsed = 3
};
const char* decode_URFPS (URFParserState pURFPS) ;

class ZSearchDictionary;

class URFParser {
public:
  URFParser(){  }

  URFParser( const ZMetaDic* pMDic) {MetaDic=pMDic;}

  URFParser(const URFParser& pIn) { _copyFrom(pIn);}
  URFParser& operator= (const URFParser& pIn) {return _copyFrom(pIn);}

  URFParser& _copyFrom (const URFParser& pIn)
  {
      AllFieldsPresent=pIn.AllFieldsPresent;
      Record=pIn.Record;
      HashTime=pIn.HashTime;
      URFFieldList.clear();
      for (int wi=0; wi < pIn.URFFieldList.count() ; wi++ )
          URFFieldList.push(pIn.URFFieldList[wi]);
      Presence=pIn.Presence;
      URFDataSize=pIn.URFDataSize;
      ParserPtr=pIn.ParserPtr;
      ParserPtrEnd=pIn.ParserPtrEnd;
      URFPS=pIn.URFPS;
      MetaDic=pIn.MetaDic;
      return *this;
  }

  /**
   * @brief set makes pRecord current record to be parsed,
   *            Presence bit set is updated from record
   *            Pointers are initialized :
   *                - PtrEnd points to the first byte after record surface
   *                - Ptr points to the first byte of URFField (after bitset)
   *
   *  Nota Bene : NO LOCAL COPY of record is made in order to advantage performance so Record must stay available.
   *
   * @param pRecord
   * @return ZS_CORRUPTED if bitset is not present or is malformed : URF is not respected.
   *                      In this case, ZException is positionned with appropriate message
   *         ZS_SUCCES if OK
   */

//  ZStatus set(const ZDataBuffer& pRecord); // Deprecated

  void setDictionary(const ZMetaDic* pMetaDic) {MetaDic=pMetaDic;}

  /* save record and initializes pointers with saved record data */
  void setRecord(const ZDataBuffer& pRecord) ;
  void _setupRecord();


  ZDataBuffer& getRecord() {return Record;}

  void clear() ;
  /* reset pointers to record data (begin, end)*/
  void resetPtr() ;

  /**
   * @brief rawParse creates the URF field list from pRecord without any check with dictionary
   */
  ZStatus rawParse(const ZDataBuffer &pRecord, ZaiErrors *pErrorLog);

/* Deprecated
  ZStatus parse(const ZDataBuffer &pRecord, const zbs::ZMetaDic *pDic, ZaiErrors *pErrorLog);
*/
  /**
   * @brief parse creates the URF field list from pRecord and check fields validity vs URFParser meta dictionary
   */
  ZStatus parse(ZaiErrors *pErrorLog);

  /* static version to obtain dynamically field list. NB: URFPS is not positioned to URFPS_TotallyParsed */
  static ZStatus rawParse(const ZDataBuffer &pRecord,
                       ZArray<URFField> &pFieldList,
                       ZaiErrors *pErrorLog);

  /**
   * @brief appendURFFieldByRank parse record surface for urf fields and append to pBuffer found field as an URF field definition.
   * @param pRank
   * @param pBuffer
   * @return
   */
  ZStatus appendURFFieldByRank (long pRank,ZDataBuffer pBuffer);

  /* not to be used anymore */
  ZStatus getURFFieldByRankIncremental(long pRank, URFField &pField);

  ZDataBuffer getURFFieldByRank (long pRank);
  ZStatus _getURFFieldByRank (long pRank,URFField& pField);

  /** @brief getURFFieldSize returns size of URF field pointed by pPtrIn.
   * pPtrIn is NOT modified,
   * Returned size includes URF header size.
   * Return -1 if if ZType is unknown
   */
  static ssize_t getURFFieldSize (const unsigned char *pPtrIn); /* pPtrIn is NOT updated */


  /**
   * @brief getURFFieldAllSizes gets ZType (pType) URF header size (pURFHeaderSize) and data size(pDataSize) without header
   *  from an URF field pointed by pPtr.
   *  pPtr is updated to point to first byte of field data : returns the size of the whole field including URF header
   */
  static ssize_t getURFFieldAllSizes (const unsigned char* &pPtr,ZTypeBase& pType,size_t &pURFHeaderSize,size_t &pDataSize);


  static ZStatus getURFTypeAndSize (const unsigned char *&pPtrIn, ZTypeBase& pType, ssize_t & pSize); /* pPtrIn is NOT updated */
  static ZStatus getURFFieldValue (const unsigned char* &Ptr, ZDataBuffer& pValue);
  static ZStatus getKeyFieldValue (const unsigned char* &Ptr, ZDataBuffer& pValue);

  static utf8VaryingString displayOneURFField(const unsigned char* &Ptr, bool pShowZType=true);

  bool hasRecord() {return (!Record.isEmpty()) && (URFPS >= URFPS_RecordSet );}


  bool                  AllFieldsPresent=false;
  ZDataBuffer           Record;             /* local copy of the record to parse */
  ZHash                 HashTime;
  ZArray<URFField>      URFFieldList;
  ZBitset               Presence;
  uint64_t              URFDataSize=0;
  const unsigned char*  ParserPtr=nullptr;
  const unsigned char*  ParserPtrEnd=nullptr;
  URFParserState        URFPS = URFPS_Nothing;
  const zbs::ZMetaDic*  MetaDic=nullptr;
};

/** @brief URFCompare  Compare two buffers composed each of one or many URF fields, each field potentially of variable length.
*/
int URFComparePtr(const unsigned char* pKey1, size_t pSize1, const unsigned char* pKey2, size_t pSize2);

/**
 * @brief URFCompareValues binary compares two contents
 * returns 0 equality in both content and size
 *         -1 if pURF1 content is less than pURF2
 *          1 if pURF1 content is greater than pURF2
 *
 * After routine ran, pURF1 and pURF2 points respectively to the end of their size
 * @return
 */
int URFCompareValues( const unsigned char* &pURF1,size_t pSize1,
                      const unsigned char* &pURF2,size_t pSize2);

/**
 * @brief compareUt8 compares two utf8 strings after having converted string to uppercase, droped accutes
 *                            Strings remains the same and are not affected by operation.
 * returns 0 equality in both content and size
 *         -1 if pKey1 content is less than pKey2
 *          1 if pKey1 content is greater than pKey2
 * After routine ran, pKey1 and pKey2 points respectively to the end of their string
 * @return
 */
int UTF8Compare(const unsigned char *&pKey1, size_t pSize1, const unsigned char *&pKey2, size_t pSize2) ;
int UTF16Compare(const unsigned char *&pKey1, size_t pSize1, const unsigned char *&pKey2, size_t pSize2)  ;
int UTF32Compare(const unsigned char* &pKey1,size_t pSize1,const unsigned char* &pKey2,size_t pSize2) ;

bool ZTypeExists(ZTypeBase pType);
ZStatus searchNextValidZType(const unsigned char *&pPtr, const unsigned char *pPtrEnd);

bool TypeExists(ZTypeBase pType);

template <class _Utf>
_Utf KeyCharConvert(_Utf pChar) {

  if (pChar > 256)  /* not a valid western character */
    return pChar;

  _Utf wV1=(_Utf)cst_ToUpper[(int)pChar]; /* convert to uppercase */
  wV1=(_Utf)cst_DropAcute[(int)wV1]; /* convert to no accute character */
  return wV1;
}

template <class _Utf>
int UTFCompare(const unsigned char* &pKey1,size_t pSize1,const unsigned char* &pKey2,size_t pSize2) {

  size_t wCount1=pSize1/sizeof(_Utf);
  size_t wCount2=pSize1/sizeof(_Utf);

  _Utf* wKey1 = (_Utf* )pKey1;
  _Utf* wKey2 = (_Utf* )pKey2;

  if (pKey1==nullptr) {
    if (pKey2==nullptr)
      return 0;
    else {
      pKey2 += pSize2;
      return -1;
    }
  }
  if (pKey2==nullptr) {
    pKey1 += pSize1;
    return 1;
  }

  _Utf pV1,pV2;

  size_t wI1=0, wI2=0;
  int wRet=0;

  //  while ((wRet==0)&&(wI1<pSize1) &&(wI2<pSize2) && pKey1[wI1] && pKey2[wI2]) {
  while ((wRet==0) && (wI1<wCount1) && (wI2<wCount2) ) {
    /*  uppercase no accute */
    pV1=KeyCharConvert<_Utf>(wKey1[wI1++]);
    pV2=KeyCharConvert<_Utf>(wKey2[wI2++]);

    wRet=pV1-pV2;
  }

  pKey1 += pSize1;
  pKey2 += pSize2;

  /* one of the two or both keys are at the end and so far they are equal */

  /* if equals and both length are equal /* one of the two or both keys are at the end and so far they are equal */
  if ((wI1==wI2)&&(wI1==wCount1)) {
    return wRet;
  }

  /* one of the two -or both- strings are at the end and so far they are equal */
  if (wI1==wCount1) { /* but Key1 has a greater size than key2 */
    return 1; /* key1 is greater than key2 */
  }
  if (wI2==wCount2) { /* Key2 has a greater size than key 1 */
    return -1; /* key1 is less than key2 */
  }
  /* both are equal in values and sizes (sizes are exhausted) */

  return wRet;
} // UTFCompare

#endif // URFPARSER_H
