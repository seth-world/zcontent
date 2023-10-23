#include "zindexfield.h"
#include <zxml/zxmlprimitives.h>
#include <zindexedfile/zmfdictionary.h>

using namespace zbs;

ZIndexField& ZIndexField::_copyFrom(const ZIndexField &pIn)
{
  MDicRank=pIn.MDicRank;
  Hash=pIn.Hash;
  KeyOffset=pIn.KeyOffset;
  KeyDic=pIn.KeyDic;
  return *this;
}
void ZIndexField::clear()
{
  MDicRank=0;
  Hash.clear();
  KeyOffset=0;
  return;
}

void ZIndexField_Exp::_convert()
{
  if (is_big_endian())
    return ;
  EndianCheck = reverseByteOrder_Conditional<uint16_t>(EndianCheck);
  MDicRank=reverseByteOrder_Conditional<uint32_t>(MDicRank);
  KeyOffset=reverseByteOrder_Conditional<uint32_t>(KeyOffset);
  /* Hash does not need to be serialized */
}

void
ZIndexField_Exp::setFromPtr(const unsigned char*& pPtrIn)
{
  memmove(this,pPtrIn,sizeof(ZIndexField_Exp));
  pPtrIn += sizeof(ZIndexField_Exp);
}
void
ZIndexField_Exp::toZIF(ZIndexField &pZIF)
{
  pZIF.MDicRank=MDicRank;
  pZIF.KeyOffset=KeyOffset;
  for (size_t wi=0;wi<cst_md5;wi++)
    pZIF.Hash.content[wi]=Hash[wi];
}
void
ZIndexField_Exp::set(const ZIndexField& pIn)
{
  MDicRank=pIn.MDicRank;
  KeyOffset=pIn.KeyOffset;
  for (size_t wi=0;wi<cst_md5;wi++)
    Hash[wi]=pIn.Hash.content[wi];
}
void
ZIndexField_Exp::serialize()
{
  if (is_big_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZKeyDictionary_Exp::serialize-W-ALRDY ZHeaderControlBlock already serialized. \n");
    return;
  }
  _convert();

}
void
ZIndexField_Exp::deserialize()
{
  if (is_big_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZKeyDictionary_Exp::deserialize-W-ALRDY ZHeaderControlBlock already deserialized. \n");
    return;
  }
  _convert();

}



ZIndexField_Exp
ZIndexField::_exportConvert(ZIndexField& pIn,ZIndexField_Exp* pOut)
{
  ZIndexField_Exp wIFS;
  pOut->set(pIn);
  pOut->serialize();
  return *pOut;
}
ZIndexField
ZIndexField::_importConvert(ZIndexField& pOut, ZIndexField_Exp *pIn)
{
  ZIndexField_Exp wIFS;
  const unsigned char* wPtrIn=(const unsigned char*)pIn;
  wIFS.setFromPtr(wPtrIn);
  wIFS.toZIF(pOut);
/*
  pOut.KeyOffset=reverseByteOrder_Conditional<uint32_t>(pIn->KeyOffset);
  pOut.MDicRank=reverseByteOrder_Conditional<uint32_t>(pIn->MDicRank);
  for (int wi=0;wi < cst_md5;wi++)
    pOut.Hash.content[wi]= pIn->Hash[wi];
*/
  return pOut;
}

ZDataBuffer&
ZIndexField::_exportAppend(ZDataBuffer& pZDBExport) const
{
  ZIndexField_Exp wIFS;
  wIFS.set(*this);
  wIFS.serialize();
  pZDBExport.append_T(wIFS);
  return pZDBExport;
}
ZDataBuffer
ZIndexField::_export() const
{
  ZDataBuffer wReturn;
  return _exportAppend(wReturn);
}

/* this routine does not change input data */
void
ZIndexField::_import(const unsigned char* &pPtrIn)
{
  ZIndexField_Exp wIFSe;
  wIFSe.setFromPtr(pPtrIn);

  wIFSe.deserialize();

  wIFSe.toZIF(*this);
}



KeyField_Pack&
KeyField_Pack::_copyFrom(const KeyField_Pack &pIn)
{
  FieldDesc_Pack::_copyFrom(pIn);
  KeyNumber=pIn.KeyNumber;
  KeyOffset=pIn.KeyOffset;
  return *this;
}


KeyField_Pack::KeyField_Pack() {}

KeyField_Pack& KeyField_Pack::set(ZMFDictionary& pDic,long pKeyRank,long pKeyFieldRank)
{
  long wMDicRank=pDic.KeyDic[pKeyRank]->Tab(pKeyFieldRank).MDicRank;

  KeyOffset=pDic.KeyDic[pKeyRank]->Tab(pKeyFieldRank).KeyOffset;
  KeyFieldRank=pKeyFieldRank;
  KeyNumber=pKeyRank;

  FieldDesc_Pack::set(pDic[wMDicRank]);
  return *this;
}
KeyField_Pack& KeyField_Pack::set(ZFieldDescription& pFDesc,long pKeyRank)
{
  FieldDesc_Pack::set(pFDesc);
  KeyOffset=-1;
  KeyFieldRank=-1;
  KeyNumber=pKeyRank;

  return *this;
}
KeyField_Pack& KeyField_Pack::set(const FieldDesc_Pack& pFD_Pack,long pKeyRank)
{
  FieldDesc_Pack::_copyFrom(pFD_Pack);
  KeyOffset=-1;
  KeyFieldRank=-1;
  KeyNumber=pKeyRank;

  return *this;
}

QDataStream& KeyField_Pack::write(QDataStream &dataStream)
{
  dataStream.writeBytes((const char *)this,sizeof(KeyField_Pack)) ;
  return(dataStream);
}

QDataStream& KeyField_Pack::read(QDataStream &dataStream)
{
  char *Buf;
  uint wsize;
  dataStream.readBytes (Buf,wsize );
  if (wsize<sizeof(KeyField_Pack))
    memmove(this,Buf,wsize);
  else
    memmove(this,(const void*)Buf,sizeof(KeyField_Pack));
  delete Buf;
  return(dataStream);
}
