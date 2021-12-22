#include "zblock.h"

ZBlockHeader& ZBlockHeader::_copyFrom(const ZBlockHeader& pIn)
{
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

ZBlockDescriptor_Export ZBlockDescriptor:: _exportConvert(ZBlockDescriptor& pIn,ZBlockDescriptor_Export* pOut)
{
  memset(pOut,0,sizeof(ZBlockDescriptor_Export));
  pOut->StartBlock = cst_ZBLOCKSTART;
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
  if ((pIn->StartBlock!=cst_ZBLOCKSTART)||(pIn->BlockId!=ZBID_Data))
  {
    ZException.setMessage("ZBlockDescriptor::_importConvert",
        ZS_INVBLOCKADDR,
        Severity_Severe,
        "Invalid block format: invalid marker  Startblock<%X> or BlockId<%X>",
        pIn->StartBlock,
        pIn->BlockId);
    return  ZS_INVBLOCKADDR;
  }

  pOut.clear();
  //        pOut.StartBlock = _reverseByteOrder_T<int32_t>(pIn->StartBlock);
  //        pOut.BlockID= pIn->BlockID;   // unsigned char
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
  pOut->StartBlock = cst_ZBLOCKSTART;
  //        pOut->BlockID= pIn.BlockID;   // uint8_t
  pOut->BlockId= ZBID_Data;   // uint8_t
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
  if ((pIn->StartBlock!=cst_ZBLOCKSTART)||(pIn->BlockId!=ZBID_Data))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVBLOCKADDR,
        Severity_Severe,
        "Invalid block format: invalid marker  Startblock<%X> or BlockId<%X>",
        pIn->StartBlock,
        pIn->BlockId);
    return  ZS_INVBLOCKADDR;
  }
  //        pOut.StartBlock = pIn->StartBlock;
  //        pOut.BlockID= pIn->BlockID;   // unsigned char
  pOut.BlockSize=reverseByteOrder_Conditional<zsize_type>(pIn->BlockSize);
  pOut.State= pIn->State;   // unsigned char
  pOut.Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn->Lock);
  //        pOut.LockReason=pIn->LockReason; // unsigned char
  pOut.Pid=reverseByteOrder_Conditional<pid_t>(pIn->Pid);

  return  ZS_SUCCESS;
}//_importConvert
