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
  static ZStatus getURFTypeAndSize (const unsigned char *pPtrIn,ZTypeBase& pType,ssize_t & pSize); /* pPtrIn is NOT updated */
  ZStatus getURFFieldValue (const unsigned char* &wPtr, ZDataBuffer& pValue);
  ZStatus getKeyFieldValue (const unsigned char* &wPtr, ZDataBuffer& pValue);

  bool                  AllFieldsPresent=false;
  const ZDataBuffer*    Record=nullptr;
  ZArray<URFField>      URFFieldList;
  ZBitset               Presence;
  size_t                URFDataSize=0;
  const unsigned char*  wPtr=nullptr;
  const unsigned char*  wPtrEnd=nullptr;
};



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
