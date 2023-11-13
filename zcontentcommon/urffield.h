#ifndef URFFIELD_H
#define URFFIELD_H

#include <stddef.h>
#include <ztoolset/zstatus.h>
#include <ztoolset/ztypetype.h>

class utf8VaryingString;
class zxmlElement;
class ZaiErrors;

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
  const unsigned char*  Ptr=nullptr; /* points to the first byte of ZTypeBase for field */
  size_t                Size=0;
  ZTypeBase             ZType=ZType_Nothing;

  ZStatus setFromPtr(const unsigned char* &pPtrIn);
  /* formats field content for screen display */
  utf8VaryingString display();

  /* formats field content for storing as text data  */
  utf8VaryingString stdDisplay() ;

  utf8VaryingString toXml(int pLevel) ;
  ZStatus fromXml(zxmlElement* pFieldNode, ZDataBuffer& pURFContent, ZaiErrors *pErrorLog);

  /* pField must contain a valid ZType */
  template <class _Tp>
  void getURFfromAtomicValue (_Tp pValue,ZDataBuffer& pURFContent)
  {
    pURFContent.allocate(sizeof(ZTypeBase)+sizeof(_Tp));
    const unsigned char* wPtr= Ptr = pURFContent.Data ;
    ZTypeBase* wTypPtr = (ZTypeBase*)wPtr;
    *wTypPtr = ZType ;
    wPtr += sizeof (ZTypeBase);
    _Tp* wValuePtr = nullptr;
    wValuePtr=(_Tp*)(wPtr);
    Size =  sizeof (ZTypeBase) + sizeof(_Tp);
    *wValuePtr = reverseByteOrder_Conditional<_Tp>(pValue) ;
  }

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

/* pField must contain a valid ZType */
/*
template <class _Tp>
void getURFfromAtomicValue (_Tp pValue,URFField& pField,ZDataBuffer& pURFContent)
{
  pURFContent.allocate(sizeof(ZTypeBase)+sizeof(_Tp));
  unsigned char* wPtr= URFField& pFieldPtr = pURFContent.Data;
  ZTypeBase* wTBPtr = (ZTypeBase*)wPtr;
  *wTBPtr = pField.ZType ;
  wPtr += sizeof (ZTypeBase);
  _Tp* wValuePtr = wPtr;
  pField.Size =
  *wValuePtr = wValue ;
}
*/


#endif // URFFIELD_H
