#include "zindexfield.h"
#include <zxml/zxmlprimitives.h>


using namespace zbs;

ZSIndexField& ZSIndexField::_copyFrom(const ZSIndexField &pIn)
{
  MDicRank=pIn.MDicRank;
  Hash=pIn.Hash;
  KeyOffset=pIn.KeyOffset;
  return *this;
}
void ZSIndexField::clear()
{
  MDicRank=0;
  Hash.clear();
  KeyOffset=0;
  return;
}

ZSIndexField_Out
ZSIndexField::_exportConvert(ZSIndexField& pIn,ZSIndexField_Out* pOut)
{

 /* memset(pOut,0,sizeof(ZSIndexField_Out));
  pOut->MDicRank=reverseByteOrder_Conditional<size_t>(pIn.MDicRank);
  pOut->NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn.NaturalSize);
  pOut->UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn.UniversalSize);
  pOut->ArrayCount=reverseByteOrder_Conditional<uint32_t>(pIn.ArrayCount);
  pOut->ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn.ZType);
  return *pOut;
  */
  pOut->clear();
  for (int wi=0;wi<cst_md5;wi++)
    pOut->Hash[wi]=pIn.Hash.content[wi];
  pOut->KeyOffset=reverseByteOrder_Conditional<uint32_t>(pIn.KeyOffset);
  pOut->MDicRank=reverseByteOrder_Conditional<uint32_t>(pIn.MDicRank);
  return *pOut;
}
ZSIndexField
ZSIndexField::_importConvert(ZSIndexField& pOut,ZSIndexField_Out* pIn)
{
/*
  memset(&pOut,0,sizeof(ZSIndexField));
  pOut.MDicRank=reverseByteOrder_Conditional<size_t>(pIn->MDicRank);
  pOut.NaturalSize=reverseByteOrder_Conditional<uint64_t>(pIn->NaturalSize);
  pOut.UniversalSize=reverseByteOrder_Conditional<uint64_t>(pIn->UniversalSize);
  pOut.ArrayCount=reverseByteOrder_Conditional<uint32_t>(pIn->ArrayCount);
  pOut.ZType=reverseByteOrder_Conditional<ZTypeBase>(pIn->ZType);
*/
  pOut.KeyOffset=reverseByteOrder_Conditional<uint32_t>(pIn->KeyOffset);
  pOut.MDicRank=reverseByteOrder_Conditional<uint32_t>(pIn->MDicRank);
  for (int wi=0;wi < cst_md5;wi++)
    pOut.Hash.content[wi]= pIn->Hash[wi];
  return pOut;
}

ZDataBuffer&
ZSIndexField::_export(ZDataBuffer& pZDBExport) const
{
  ZSIndexField_exp wIFS;
//  wIFS.MDicRank=reverseByteOrder_Conditional<size_t>(MDicRank);
//  wIFS.NaturalSize=reverseByteOrder_Conditional<uint64_t>(NaturalSize);
//  wIFS.UniversalSize=reverseByteOrder_Conditional<uint64_t>(UniversalSize);
  wIFS.KeyOffset=reverseByteOrder_Conditional<uint64_t>(KeyOffset);
  for (int wi=0;wi < cst_md5;wi++)
        wIFS.Hash[wi] = Hash.content[wi];
//  wIFS.ArrayCount=reverseByteOrder_Conditional<uint32_t>(ArrayCount);
//  wIFS.ZType=reverseByteOrder_Conditional<ZTypeBase>(ZType);
  //    wIFS.RecordOffset=_reverseByteOrder_T<uint64_t>(RecordOffset);
  //    wIFS.Name=Name;
  pZDBExport.setData(&wIFS,sizeof(wIFS));
  return pZDBExport;
}
ZDataBuffer
ZSIndexField::_export() const
{
  ZDataBuffer wReturn;
  return _export(wReturn);
}

size_t
ZSIndexField::_import(unsigned char* &pPtrIn)
{
  ZSIndexField_exp* wIFS=(ZSIndexField_exp*) pPtrIn;

  MDicRank=reverseByteOrder_Conditional<uint32_t>(wIFS->MDicRank);
//  NaturalSize=reverseByteOrder_Conditional<uint64_t>(wIFS->NaturalSize);
//  UniversalSize=reverseByteOrder_Conditional<uint64_t>(wIFS->UniversalSize);
  KeyOffset=reverseByteOrder_Conditional<uint32_t>(wIFS->KeyOffset);
//  ArrayCount=reverseByteOrder_Conditional<uint32_t>(wIFS->ArrayCount);
//  ZType=reverseByteOrder_Conditional<ZTypeBase>(wIFS->ZType);

  for (int wi=0;wi < cst_md5;wi++)
    Hash.content[wi]=wIFS->Hash[wi];

  //    RecordOffset=_reverseByteOrder_T<uint64_t>(wIFS->RecordOffset);
  //    Name=wIFS->Name;  // Name is a descString
  pPtrIn += sizeof(ZSIndexField_exp);
  return sizeof(ZSIndexField_exp);
}



