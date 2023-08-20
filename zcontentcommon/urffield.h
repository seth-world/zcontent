#ifndef URFFIELD_H
#define URFFIELD_H

#include <stddef.h>
#include <ztoolset/zstatus.h>
#include <ztoolset/ztypetype.h>

class utf8VaryingString;

class URFField {
public:
  URFField() ;
  URFField(const URFField& pIn) {_copyFrom(pIn);}
  URFField&_copyFrom(const URFField& pIn) {
    Present=pIn.Present;
    Ptr=pIn.Ptr;
    Size=pIn.Size;
    ZType=pIn.ZType;
    return *this;
  }
  bool                  Present=false;
  const unsigned char*  Ptr=nullptr;
  size_t                Size=0;
  ZTypeBase             ZType=ZType_Nothing;

  ZStatus setFromPtr(const unsigned char* &pPtrIn);
  utf8VaryingString display();
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



#endif // URFFIELD_H
