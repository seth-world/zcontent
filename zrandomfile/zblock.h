#ifndef ZBLOCK_H
#define ZBLOCK_H

#include <zrandomfile/zrandomfiletypes.h>

class ZBlock;
class ZBlockDescriptor;

#pragma pack(push)
#pragma pack(1)
class ZBlockHeader_Export
{
public:
  uint32_t            StartBlock=cst_ZBLOCKSTART ;    // Start marker
  ZBlockID            BlockId=ZBID_Data;         // Block identification : always ZBID_Data here
  zsize_type          BlockSize;      // total size of the physical block, ZBH_Export size+user content size
  ZBlockState_type    State;          // state of the block see @ref ZBlockState_type
  zlockmask_type      Lock;           // relates to ZLockMask_type (zlockmanager.h)
  //    ZLock_Reason        LockReason;     // RFFU (zlockmanager.h)
  pid_t               Pid;            // process identification that locked the block
};
class ZBlockDescriptor_Export: public ZBlockHeader_Export
{
public:
  zaddress_type        Address;      // Offset from begining of file : Physical Address (starting 0)
};

#pragma pack(pop)

/**
 * @brief The ZBlockHeader class  : ZBlockHeader is heading any user data and stored in file before user record content as a header for each data block
 *  This header is effectively what is written on file before any user data record.
 *
 */
class ZBlockHeader
{
public:

  //    uint32_t            StartBlock ;    //!< Start sign
  //    ZBlockID            BlockID;        // Block identification
  zsize_type          BlockSize;      // total size of the physical block on file  including ZBlockHeader_Export size
  //                                       ZBlockHeader_Export plus user content size
  ZBlockState_type    State;          // state of the block see @ref ZBlockState_type
  zlockmask_type      Lock;           // relates to ZLockMask_type (zlockmanager.h)
  //    ZLock_Reason        LockReason;     // RFFU (zlockmanager.h)
  pid_t               Pid;            // process identification that locked the block


  ZBlockHeader(void) {clear(); return;}
  ZBlockHeader(const ZBlockHeader& pIn) {_copyFrom(pIn);}
  ZBlockHeader(const ZBlockHeader&& pIn) {_copyFrom(pIn);}

  void clear(void) {memset(this,0,sizeof(ZBlockHeader)); return;}

  ZBlockHeader& _copyFrom(const ZBlockHeader& pIn);

  ZBlockHeader& operator = (const ZBlockHeader &pIn) {return _copyFrom(pIn);}
  ZBlockHeader& operator = (const ZBlockDescriptor& pBlockDescriptor) {memmove(this,&pBlockDescriptor,sizeof(ZBlockHeader)); return *this;}
  ZBlockHeader& operator = (const ZBlock& pBlock) {memmove(this,&pBlock,sizeof(ZBlockHeader));  return *this;}
  /*    ZDataBuffer& _export(ZDataBuffer& pZDBExport)
    {
        ZBlockHeader wExport;
        wExport.StartBlock = _reverseByteOrder_T<int32_t>(StartBlock);
        wExport.BlockID= BlockID;   // unsigned char
        wExport.BlockSize=_reverseByteOrder_T<zsize_type>(BlockSize);
        wExport.State= State;   // unsigned char
        wExport.Lock=_reverseByteOrder_T<zlock_type>(Lock);
        wExport.LockReason=LockReason; // unsigned char
        wExport.Pid=_reverseByteOrder_T<pid_t>(Pid);
        pZDBExport.setData(&wExport,sizeof(wExport));
        return pZDBExport;
    }//_export
    ZBlockHeader& _import(unsigned char* pZDBImport_Ptr)
    {
        ZBlockHeader* pImport=(ZBlockHeader*)(pZDBImport_Ptr);
        StartBlock = _reverseByteOrder_T<int32_t>(pImport->StartBlock);
        BlockID= pImport->BlockID;   // unsigned char
        BlockSize=_reverseByteOrder_T<zsize_type>(pImport->BlockSize);
        State= pImport->State;   // unsigned char
        Lock=_reverseByteOrder_T<zlock_type>(pImport->Lock);
        LockReason=pImport->LockReason; // unsigned char
        Pid=_reverseByteOrder_T<pid_t>(pImport->Pid);
        return *this;
    }//_import*/
  static ZBlockHeader_Export& _exportConvert(ZBlockHeader& pIn,ZBlockHeader_Export* pOut);
  static ZStatus _importConvert(ZBlockHeader& pOut,ZBlockHeader_Export* pIn);

};

/**
 * @brief The ZBlockDescriptor class This class enhance ZBlockHeader with other fields (Address).
 * This is the main entry for describing file blocks within pools (ZBlockAccessTable, ZFreeBlockPool,...).
 */
class ZBlockDescriptor : public ZBlockHeader
{
public:
  zaddress_type        Address;  //!< Offset from begining of file : Physical Address

  ZBlockDescriptor& _copyFrom(const ZBlockDescriptor& pIn);


  ZBlockDescriptor(void) {ZBlockHeader::clear(); Address=0; return;}
  ZBlockDescriptor(const ZBlockDescriptor& pIn) {ZBlockHeader::_copyFrom(pIn); Address=pIn.Address;}
  ZBlockDescriptor(const ZBlockDescriptor&& pIn) {ZBlockHeader::_copyFrom(pIn);Address=pIn.Address;}

  void clear(void) {memset(this,0,sizeof(ZBlockDescriptor));return;}

  ZBlockDescriptor & operator = (const ZBlockDescriptor &pIn) {    return _copyFrom(pIn); }
  ZBlockDescriptor& operator = (const ZBlockHeader& pBlockHeader) { ZBlockHeader::_copyFrom(pBlockHeader); return *this;}
  ZBlockDescriptor& operator = (const ZBlock &pBlock) {ZBlockHeader::_copyFrom((const ZBlockHeader&)pBlock); return *this;}

  static ZBlockDescriptor_Export _exportConvert(ZBlockDescriptor& pIn,ZBlockDescriptor_Export* pOut);

  static ZStatus _importConvert(ZBlockDescriptor& pOut,ZBlockDescriptor_Export* pIn);



};

/**
 * @brief The ZBlockMin_struct struct Intermediate simplified structure to manage addresses and content size.
 */
struct ZBlockMin_struct
{
  zaddress_type Address;
  zsize_type   BlockSize;

  ZBlockMin_struct& operator = (ZBlockDescriptor &pBlock)
  {Address=pBlock.Address; BlockSize=pBlock.BlockSize; return *this;}
};


/**
 * @brief The ZBlock class  This object contains all the informations of a user block :
 * - ZBlockHeader : record control block
 * - User record content in a ZDataBuffer
 *
 */
class ZBlock : public ZBlockHeader
{
public:
  ZDataBuffer           Content;    //!< user data content

  ZBlock (void) {clear();}

  void clear(void) {memset(this,0,sizeof(ZBlock)); ZBlockHeader::clear();}
  void resetData(void) {Content.reset();}

  unsigned char* allocate (size_t pSize) {return(Content.allocate(pSize));}
  unsigned char* allocateBZero (size_t pSize) {return(Content.allocateBZero(pSize));}

  size_t DataSize(void) {return(Content.Size);}
  char * DataChar(void) {return(Content.DataChar);}

  ZBlock& operator = (const ZBlockHeader &pHeader) {memmove(this,&pHeader,sizeof(ZBlockHeader)); return *this;}
  ZBlock& operator = (const ZBlock& pBlock) {memmove(this,&pBlock,sizeof(ZBlock)); return *this;}

};
#endif // ZBLOCK_H
