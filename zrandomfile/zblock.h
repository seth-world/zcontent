#ifndef ZBLOCK_H
#define ZBLOCK_H

#include <ztoolset/zlimit.h>
#include <zrandomfile/zrandomfiletypes.h>

class ZBlock;
class ZBlockDescriptor;
class ZBlockHeader;
#pragma pack(push)
#pragma pack(1)
class ZBlockHeader_Export
{
public:
  ZBlockHeader_Export()=default;
  ZBlockHeader_Export(const ZBlockHeader_Export& pIn) {_copyFrom(pIn);}

  ZBlockHeader_Export& _copyFrom(const ZBlockHeader_Export& pIn);

  ZBlockHeader_Export& set(const ZBlockHeader &pIn);
  ZBlockHeader_Export& setFromPtr(unsigned char* &pIn);

  void _convert();
  void serialize();
  void deserialize();

  bool isReversed() {if (EndianCheck==cst_EndianCheck_Reversed) return true; return false;}
  bool isNotReversed() {if (EndianCheck==cst_EndianCheck_Normal) return true; return false;}

  uint32_t            StartSign=cst_ZBLOCKSTART ;    // Start marker
  ZBlockID            BlockId=ZBID_Data;         // Block identification : always ZBID_Data here
  uint16_t            EndianCheck=cst_EndianCheck_Normal;
  zsize_type          BlockSize;      // total size of the physical block, ZBH_Export size+user content size
  ZBlockState_type    State;          // state of the block see @ref ZBlockState_type
  zlockmask_type      Lock;           // relates to ZLockMask_type (zlockmanager.h)
  //    ZLock_Reason        LockReason;     // RFFU (zlockmanager.h)
  pid_t               Pid;            // process identification that locked the block
};
class ZBlockDescriptor_Export: public ZBlockHeader_Export
{
public:
  void set(const ZBlockDescriptor& pIn);
  void setFromPtr(unsigned char* &pPtrIn);

  void _convert();
  void serialize();
  void deserialize();

  bool isReversed() {if (EndianCheck==cst_EndianCheck_Reversed) return true; return false;}
  bool isNotReversed() {if (EndianCheck==cst_EndianCheck_Normal) return true; return false;}

  zaddress_type        Address;      // Offset from begining of file : Physical Address (starting 0)
};

#pragma pack(pop)

/**
 * @brief The ZBlockHeader class  : ZBlockHeader is heading any user data and written in file before user record content as a header for each data block
 *  This header is effectively what is written on file before any user data record.
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


  ZStatus _import(unsigned char *pPtrIn);
  ZDataBuffer& _exportAppend(ZDataBuffer &pZDB);

  /* required for exporting Pool with ZArray export facilities */
  static ZBlockHeader_Export& _exportConvert(ZBlockHeader& pIn,ZBlockHeader_Export* pOut);
  static ZStatus _importConvert(ZBlockHeader& pOut,ZBlockHeader_Export* pIn);

};

/**
 * @brief The ZBlockDescriptor class This class enhance ZBlockHeader with Address.
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

  ZStatus _import(unsigned char *pPtrIn);
  ZDataBuffer& _exportAppend(ZDataBuffer &pZDB);

  /* required for exporting Pool with ZArray export facilities */
  static ZBlockDescriptor_Export _exportConvert(ZBlockDescriptor& pIn,ZBlockDescriptor_Export* pOut);
  static ZStatus _importConvert(ZBlockDescriptor& pOut,ZBlockDescriptor_Export* pIn);

};

/**
 * @brief The ZBlockMin class Intermediate simplified structure to manage addresses and content size.
 */
class ZBlockMin
{
public:
  ZBlockMin()=default;
  ZBlockMin(zaddress_type pAddr,zsize_type pSize) {Address=pAddr; BlockSize=pSize;}
  ZBlockMin(const ZBlockMin& pIn) {_copyFrom(pIn);}
  zaddress_type Address=0L;
  zsize_type   BlockSize=0uL;

  ZBlockMin& operator = (ZBlockDescriptor &pBlock)
  {Address=pBlock.Address; BlockSize=pBlock.BlockSize; return *this;}
  ZBlockMin& operator = (const ZBlockMin& pIn) {return _copyFrom(pIn);}
  ZBlockMin& _copyFrom (const ZBlockMin& pIn) {Address=pIn.Address; BlockSize=pIn.BlockSize; return *this;}
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
