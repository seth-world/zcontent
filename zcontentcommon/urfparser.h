#ifndef URFPARSER_H
#define URFPARSER_H


#include <stdint.h>

#include <ztoolset/zstatus.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zbitset.h>
#include <ztoolset/zexceptionmin.h>


class URFField {
public:
  URFField() =default;
  URFField(const URFField& pIn) {_copyFrom(pIn);}
  URFField&_copyFrom(const URFField& pIn) {
    Present=pIn.Present;
    Ptr=pIn.Ptr;
    Size=pIn.Size;
    return *this;
  }
  bool                  Present=false;
  const unsigned char*  Ptr=nullptr;
  size_t                Size=0;
};


enum CompareOptions : uint8_t {
  COPT_Nothing = 0,
  COPT_DropAccute = 1,
  COPT_UpCase     = 2,
  COPT_Default    = COPT_DropAccute | COPT_UpCase
};

class URFParser {
public:
  URFParser(){  }

  URFParser(const URFParser&) = delete;
  URFParser& operator= (const URFParser&) = delete;

  ZStatus set(const ZDataBuffer * pRecord);
  /**
   * @brief appendURFFieldByRank parse record surface for urf fields and append to pBuffer found field as an URF field definition.
   * @param pRank
   * @param pBuffer
   * @return
   */
  ZStatus appendURFFieldByRank (long pRank,ZDataBuffer pBuffer);

  ZDataBuffer getURFFieldByRank (long pRank);

  static ssize_t getURFFieldSize (const unsigned char *pPtrIn); /* pPtrIn is NOT updated */

  /**
   *  */
  /**
   * @brief getURFFieldAllSizes gets ZType (pType) URF header size (pURFHeaderSize) and data size(pDataSize) without header
   *  from an URF field pointed by pPtr.
   *  pPtr is updated to point to first byte of field data : returns the size of the whole field including URF header
   */
  static ssize_t getURFFieldAllSizes (const unsigned char* &pPtr,ZTypeBase& pType,size_t &pURFHeaderSize,size_t &pDataSize);


  static ZStatus getURFTypeAndSize (const unsigned char *&pPtrIn, ZTypeBase& pType, ssize_t & pSize); /* pPtrIn is NOT updated */
  ZStatus getURFFieldValue (const unsigned char* &wPtr, ZDataBuffer& pValue);
  ZStatus getKeyFieldValue (const unsigned char* &wPtr, ZDataBuffer& pValue);

  static utf8VaryingString displayOneURFField(const unsigned char* &wPtr);



  bool                  AllFieldsPresent=false;
  const ZDataBuffer*    Record=nullptr;
  ZArray<URFField>      URFFieldList;
  ZBitset               Presence;
  uint64_t              URFDataSize=0;
  const unsigned char*  wPtr=nullptr;
  const unsigned char*  wPtrEnd=nullptr;
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

template <class _Tp>
_Tp
convertAtomicBack(ZType_type pType,const unsigned char* &pPtrIn) {
  _Tp wValue;
  uint8_t wSign=1;
  if (pType & ZType_Signed) {
    wSign = *pPtrIn;
    pPtrIn += sizeof(uint8_t);
  }
  memmove(&wValue,pPtrIn,sizeof(_Tp));
  pPtrIn += sizeof(_Tp);
  wValue = reverseByteOrder_Conditional(wValue);
  if (!wSign) {
    wValue = _negate(wValue);
    wValue = -wValue;
  }
  return wValue;
} // convertAtomicBack

bool ZTypeExists(ZTypeBase pType);
ZStatus searchNextValidZType( const unsigned char* &pPtr,const unsigned char* wPtrEnd);



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
