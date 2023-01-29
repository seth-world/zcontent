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

int URFCompareValues( const unsigned char* &pURF1,size_t pSize1,
                      const unsigned char* &pURF2,size_t pSize2);



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

#endif // URFPARSER_H
