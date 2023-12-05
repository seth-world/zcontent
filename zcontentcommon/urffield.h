#ifndef URFFIELD_H
#define URFFIELD_H

#include <stddef.h>
#include <ztoolset/zstatus.h>
#include <ztoolset/ztypetype.h>

#include "zcontentconstants.h"
using namespace zbs;

class utf8VaryingString;
class zxmlElement;
class ZaiErrors;
class URFParser;
#ifndef __HASH_TIME__
#define __HASH_TIME__
#include <ztoolset/zdatabuffer.h>
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

    void set(const ZDataBuffer& pIn) {
        Value = (unsigned long)pIn.Data;
    }

    ZHash& operator = (const ZHash& pIn) {return _copyFrom(pIn); }

    bool operator == (const ZHash& pIn) {return (Value==pIn.Value)  ; }

    bool operator != (const ZHash& pIn) {return !(Value==pIn.Value)  ; }

    bool operator == (const ZDataBuffer& pIn) {return (Value == (unsigned long)pIn.Data);}

    unsigned long Value=0;
};

#endif //  __HASH_TIME__

class URFField {
public:
  URFField() ;
  URFField(const URFField& pIn) {_copyFrom(pIn);}
  URFField&_copyFrom(const URFField& pIn) ;

  URFField& operator = (const URFField& pIn) { return _copyFrom(pIn);}

  void clear() ;

  bool                  Present=false;
  const unsigned char*  FieldPtr=nullptr; /* points to the first byte of ZTypeBase for field */
  ZHash                 HashTime;
  size_t                Size=0;
  ZTypeBase             ZType=ZType_Nothing;
  URFParser*            _URFParser=nullptr;

  ZStatus setFromPtr(const ZDataBuffer& pRecord,
                     const unsigned char* &pPtrIn,
                     ZaiErrors* pErrorLog);

  static ZStatus searchNextValidZType(const ZDataBuffer& pRecord,
                                       const unsigned char* &pPtr,
                                       ZaiErrors* pErrorLog);
  /* formats field content for screen display */
  utf8VaryingString display();
  utf8VaryingString displayFmt(ZCFMT_Type pCellFormat = 0);

  /* formats field content for storing as text data  */
  utf8VaryingString stdDisplay() ;

  utf8VaryingString toXml(int pLevel) ;
  ZStatus fromXml(zxmlElement* pFieldNode, ZDataBuffer& pURFContent, ZaiErrors *pErrorLog);

  /* pField must contain a valid ZType */
  template <class _Tp>
  void getURFfromAtomicValue (_Tp pValue,ZDataBuffer& pURFContent)
  {
    pURFContent.allocate(sizeof(ZTypeBase)+sizeof(_Tp));
    const unsigned char* wPtr= FieldPtr = pURFContent.Data ;
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

URFField getURFField(const ZDataBuffer& pRecord,
                     const unsigned char* &pPtrIn,
                     ZaiErrors* pErrorLog);
#endif // URFFIELD_H
