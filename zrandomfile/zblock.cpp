#include "zblock.h"


ZBlockHeader_Export& ZBlockHeader_Export::_copyFrom(const ZBlockHeader_Export& pIn)
{
  memset(this,0,sizeof(ZBlockHeader_Export));
  StartSign=pIn.StartSign;
  BlockId=pIn.BlockId;
  EndianCheck=pIn.EndianCheck;
  BlockSize=pIn.BlockSize;
  State=pIn.State;
  Lock=pIn.Lock;
  Pid=pIn.Pid;

  return *this;
}

ZBlockHeader_Export& ZBlockHeader_Export::set(const ZBlockHeader& pIn)
{

  BlockSize=pIn.BlockSize;
  State=pIn.State;
  Lock=pIn.Lock;
  Pid=pIn.Pid;

  return *this;
}

ZBlockHeader_Export& ZBlockHeader_Export::setFromPtr(const unsigned char *&pPtrIn)
{
  ZBlockHeader_Export* wIne=(ZBlockHeader_Export*)pPtrIn;
  StartSign=wIne->StartSign;
  BlockId=wIne->BlockId;
  EndianCheck=wIne->EndianCheck;
  BlockSize=wIne->BlockSize;
  State=wIne->State;
  Lock=wIne->Lock;
  Pid=wIne->Pid;
  pPtrIn += sizeof(ZBlockHeader_Export);
  return *this;
}

void ZBlockHeader_Export::_convert()
{
  if (!is_little_endian())
    return ;

  EndianCheck=reverseByteOrder_Conditional(EndianCheck);

  //  StartSign=reverseByteOrder_Conditional<uint32_t>(StartSign);
  BlockSize=reverseByteOrder_Conditional(BlockSize);
  Pid=reverseByteOrder_Conditional(Pid);
  return ;
}


void
ZBlockHeader_Export::serialize()
{
  if (!is_little_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZBlockHeader_Export::serialize-W-ALRDY Master Control Block already serialized. \n");
    return;
  }
  _convert();
}

void
ZBlockHeader_Export::deserialize()
{
  if (!is_little_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZBlockHeader_Export::deserialize-W-ALRDY Master Control Block already deserialized. \n");
    return;
  }
  _convert();
}



void ZBlockDescriptor_Export::set(const ZBlockDescriptor &pIn)
{
  ZBlockHeader_Export::set((const ZBlockHeader&)pIn);
  Address = pIn.Address;
  return ;
}

void ZBlockDescriptor_Export::setFromPtr(const unsigned char *&pPtrIn)
{
  ZBlockDescriptor_Export* wIne=(ZBlockDescriptor_Export*)pPtrIn;
  ZBlockHeader_Export::setFromPtr(pPtrIn);
  Address = wIne->Address;
  pPtrIn += sizeof(Address);
  return ;
}

void ZBlockDescriptor_Export::_convert()
{
  if (!is_little_endian())
    return ;
  ZBlockHeader_Export::_convert();
  Address=reverseByteOrder_Conditional(Address);
  return ;
}

void
ZBlockDescriptor_Export::serialize()
{
  if (!is_little_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZBlockDescriptor_Export::serialize-W-ALRDY Master Control Block already serialized. \n");
    return;
  }
  _convert();
}

void
ZBlockDescriptor_Export::deserialize()
{
  if (!is_little_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZBlockDescriptor_Export::deserialize-W-ALRDY Master Control Block already deserialized. \n");
    return;
  }
  _convert();
}





ZBlockHeader& ZBlockHeader::_copyFrom(const ZBlockHeader& pIn)
{
  memset(this,0,sizeof(ZBlockHeader));
  BlockSize=pIn.BlockSize;
  State=pIn.State;
  Lock=pIn.Lock;
  Pid=pIn.Pid;
  return *this;
}

ZBlockHeader& ZBlockHeader::_FromZBlock(const ZBlock& pIn)
{
  memset(this,0,sizeof(ZBlockHeader));
  BlockSize=pIn.BlockSize;
  State=pIn.State;
  Lock=pIn.Lock;
  Pid=pIn.Pid;
  return *this;
}
ZBlockHeader& ZBlockHeader::_FromZBlockDescriptor(const ZBlockDescriptor& pIn)
{
  memset(this,0,sizeof(ZBlockHeader));
  BlockSize=pIn.BlockSize;
  State=pIn.State;
  Lock=pIn.Lock;
  Pid=pIn.Pid;
  return *this;
}


ZBlockDescriptor& ZBlockDescriptor::_copyFrom(const ZBlockDescriptor& pIn)
{
  ZBlockHeader::_copyFrom(pIn);
  Address=pIn.Address;
  return *this;
}


ZStatus
ZBlockHeader::_import(const unsigned char *pPtrIn)
{
  ZBlockHeader_Export wZBHe;
  wZBHe.setFromPtr(pPtrIn);
  wZBHe.deserialize();
  BlockSize = wZBHe.BlockSize;
  State=wZBHe.State;
  Lock=wZBHe.Lock;
  Pid=wZBHe.Pid;
}

ZStatus
ZBlockDescriptor::_import(const unsigned char *pPtrIn)
{
  ZBlockDescriptor_Export wZBDe;
  wZBDe.setFromPtr(pPtrIn);
  wZBDe.deserialize();
  BlockSize = wZBDe.BlockSize;
  State=wZBDe.State;
  Lock=wZBDe.Lock;
  Pid=wZBDe.Pid;
  Address = wZBDe.Address;
}

ZDataBuffer &
ZBlockHeader::_exportAppend(ZDataBuffer &pZDB)
{
  ZBlockHeader_Export wZBHe;
  wZBHe.set(*this);
  wZBHe.serialize();

  pZDB.append_T(wZBHe);
  return pZDB;
}

ZDataBuffer &
ZBlockDescriptor::_exportAppend(ZDataBuffer &pZDB)
{
  ZBlockDescriptor_Export wZBDe;
  wZBDe.set(*this);
  wZBDe.serialize();
  pZDB.append_T(wZBDe);
  return pZDB;
}


ZBlockDescriptor_Export ZBlockDescriptor:: _exportConvert(ZBlockDescriptor_Export* pOut,ZBlockDescriptor& pIn)
{
  memset(pOut,0,sizeof(ZBlockDescriptor_Export));
  pOut->StartSign = cst_ZFILEBLOCKSTART;
  pOut->EndianCheck = cst_EndianCheck_Reversed;
  pOut->BlockId= ZBID_Data ;   // uint8_t
  pOut->BlockSize=reverseByteOrder_Conditional<zsize_type>(pIn.BlockSize);
  pOut->State= pIn.State;   // uint8_t
  pOut->Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn.Lock);
  //        pOut->LockReason=pIn.LockReason; // unsigned char
  pOut->Pid=reverseByteOrder_Conditional<pid_t>(pIn.Pid);
  // specific to ZBlockDescriptor
  pOut->Address=reverseByteOrder_Conditional<zaddress_type>(pIn.Address);

  return *pOut;
}//_exportConvert


ZStatus
ZBlockDescriptor::_importConvert(ZBlockDescriptor& pOut, ZBlockDescriptor_Export *pIn)
{

  if (pIn->StartSign!=cst_ZFILEBLOCKSTART) {
    ZException.setMessage("ZBlockDescriptor::_importConvert",
        ZS_INVBLOCK,
        Severity_Severe,
        "Invalid block format: invalid marker  Startblock<0x%X>" ,
        pIn->StartSign);
    return  ZS_INVBLOCK;
  }
  if (pIn->BlockId!=ZBID_Data) {
    ZException.setMessage("ZBlockDescriptor::_importConvert",
        ZS_INVBLOCK,
        Severity_Severe,
        "Invalid block format: invalid block id <0x%X>",
        pIn->StartSign,
        pIn->BlockId);
    return  ZS_INVBLOCK;
  }
  if (pIn->EndianCheck != cst_EndianCheck_Reversed) {
    ZException.setMessage("ZBlockDescriptor::_importConvert",
        ZS_ENDIAN,
        Severity_Warning,
        "Block is not reversed (EndianCheck) while importing it.");
  }
  pOut.clear();

  //        pOut.StartBlock = _reverseByteOrder_T<int32_t>(pIn->StartBlock);
  //        pOut.BlockId= pIn->BlockId;   // unsigned char
  pOut.BlockSize=reverseByteOrder_Conditional<zsize_type>(pIn->BlockSize);
  pOut.State= pIn->State;   // unsigned char
  pOut.Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn->Lock);
  //        pOut.LockReason=pIn->LockReason; // unsigned char
  pOut.Pid=reverseByteOrder_Conditional<pid_t>(pIn->Pid);
  // specific to ZBlockDescriptor
  pOut.Address=reverseByteOrder_Conditional<zaddress_type>(pIn->Address);
  return  ZS_SUCCESS;
}//_importConvert

ZBlockHeader_Export&
ZBlockHeader::_exportConvert(ZBlockHeader& pIn,ZBlockHeader_Export* pOut)
{
  pOut->StartSign = cst_ZFILEBLOCKSTART;
  pOut->BlockId= ZBID_Data;   // uint8_t
  pOut->EndianCheck = reverseByteOrder_Conditional<uint16_t>(cst_EndianCheck_Normal);

  pOut->BlockSize=reverseByteOrder_Conditional<zsize_type>(pIn.BlockSize);
  pOut->State= pIn.State;   // unsigned char
  pOut->Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn.Lock);
  //        pOut->LockReason=pIn.LockReason; // unsigned char
  pOut->Pid=reverseByteOrder_Conditional<pid_t>(pIn.Pid);

  return *pOut;
}//_exportConvert

ZStatus
ZBlockHeader::_importConvert(ZBlockHeader& pOut,ZBlockHeader_Export* pIn)
{
  pOut.clear();
  if (pIn->StartSign!=cst_ZFILEBLOCKSTART) {
    ZException.setMessage("ZBlockHeader::_importConvert",
        ZS_INVBLOCK,
        Severity_Severe,
        "Invalid block format: invalid marker  Startblock<0x%X>" ,
        pIn->StartSign);
    return  ZS_INVBLOCK;
  }
  if (pIn->BlockId!=ZBID_Data) {
    ZException.setMessage("ZBlockHeader::_importConvert",
        ZS_INVBLOCK,
        Severity_Severe,
        "Invalid block format: invalid block id <0x%X>",
        pIn->StartSign,
        pIn->BlockId);
    return  ZS_INVBLOCK;
  }
  if (pIn->isNotReversed() ) {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_CORRUPTED,
        Severity_Severe,
        "Invalid block format: input is not serialized, EndianChek is not reversed.");
    return  ZS_CORRUPTED;
  }
  pOut.BlockSize=reverseByteOrder_Conditional<zsize_type>(pIn->BlockSize);
  pOut.State= pIn->State;   // unsigned char
  pOut.Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn->Lock);

  pOut.Pid=reverseByteOrder_Conditional<pid_t>(pIn->Pid);

  return  ZS_SUCCESS;
}//_importConvert



const char*
decode_BlockId(ZBlockId pBID)
{
  switch (pBID)
  {
  case ZBID_Nothing:
    return "ZBID_Nothing";
  case ZBID_HCB:
    return "ZBID_FileHeader";
  case ZBID_FCB:
    return "ZBID_FCB";
  case ZBID_MCB:
    return "ZBID_MCB";
  case ZBID_ICB:
    return "ZBID_ICB";
  case ZBID_JCB:
    return "ZBID_JCB";
  case ZBID_Data:
    return "ZBID_Data";
  case ZBID_Index:
    return "ZBID_Index";
  case ZBID_Master:
    return "ZBID_Master";
  case ZBID_MDIC:
    return "ZBID_MDIC";
  default:
    return "Unknown Block id";
  }
}
