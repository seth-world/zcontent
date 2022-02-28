#ifndef ZHEADERCONTROLBLOCK_H
#define ZHEADERCONTROLBLOCK_H

#include <ztoolset/zlimit.h>
#include <zrandomfile/zrandomfiletypes.h>
#include <ztoolset/zdatabuffer.h>


typedef void  (*ZFCBgetReserved) (ZDataBuffer &) ;
#pragma pack(push)
#pragma pack(1)
/**
 * @brief The ZHeaderControlBlock_Export class
 *
 *  Nota Bene : file lock infos are positionned at the block beginning right after StartSign to be quickly read/write accessed within file
 *
 *
 */


class ZHeaderControlBlock;
class ZHeaderControlBlock_Export
{
public:
  uint32_t                StartSign   = cst_ZBLOCKSTART;  /**< check for block start */
  ZBlockID                BlockID     = ZBID_FileHeader;  /**< identification is file header */
  uint16_t                EndianCheck = cst_EndianCheck_Normal;
  unsigned long           ZRFVersion  = __ZRF_VERSION__;  /**< software version */

  ZFile_type              FileType    = ZFT_Nothing ;     /**< File type see ref ZFile_type */
  zlockmask_type          Lock        = ZLock_Nolock ;    /**< Lock mask (int32_t) at file header level (Exclusive lock) @see ZLockMask_type one lock at a time is authorized */
  pid_t                   LockOwner   = 0L;               /**< Owner process for the lock */

  zaddress_type           OffsetFCB;                      /**< offset to ZFileControlBlock when written in file */
  zaddress_type           OffsetReserved;                 /**< offset to ZReserved section when written in file */
  zsize_type              SizeReserved = 0L;              /**< Size of ZReserved section in file */

  uint32_t                EndSign     = cst_ZBLOCKEND;         /**< check for block end */


  ZHeaderControlBlock_Export()=default;
  ZHeaderControlBlock_Export(ZHeaderControlBlock_Export& pIn) {_copyFrom(pIn);}

  ZHeaderControlBlock_Export& operator=(ZHeaderControlBlock_Export& pIn) {return _copyFrom(pIn);}

  void    set(const ZHeaderControlBlock& pIn);
  void    setFromPtr(const unsigned char *&pPtrIn);
  ZHeaderControlBlock&          toHCB(ZHeaderControlBlock& pOut);

  void _convert();
  void serialize();
  void deserialize();

  ZHeaderControlBlock_Export& _copyFrom(ZHeaderControlBlock_Export& pIn);

  bool isReversed()
  {
    if (EndianCheck==cst_EndianCheck_Reversed) return true;
    return false;
  }
  bool isNotReversed()
  {
    if (EndianCheck==cst_EndianCheck_Normal) return true;
    return false;
  }

};

class lockPack
{
public:
  zlockmask_type          Lock      = ZLock_Nolock ;  /**< Lock mask (int32_t) at file header level (Exclusive lock) @see ZLockMask_type one lock at a time is authorized */
  pid_t                   LockOwner = 0L;             /**< Owner process for the lock */

};

#pragma pack(pop)

/**
 * @brief The ZHeaderControlBlock class contains the required information as a header for any ZRandomFile
 *  This block is the first of the file's header.
 *  It represents the signature of the file and contains the necessary set of addresses of all others header components.
 *
 *  This is purely a file data block (while ZFileDescriptor owns file's infradata in memory).
 *
 *  Nota Bene : file lock infos are positionned at the block beginning right after StartSign to be quickly read/write accessed within file
 *   So, to access lock infos (Lock and LockOwner), offset is sizeof(uint32_t)
 *
 */
class ZHeaderControlBlock
{
public:
  //    uint32_t                StartSign   = cst_ZSTART;       /**< check for block start */

  zlockmask_type          Lock    = ZLock_Nolock ;   /**< Lock mask (int32_t) at file header level (Exclusive lock) @see ZLockMask_type one lock at a time is authorized */
  pid_t                   LockOwner = 0L;            /**< Owner process for the lock */

  //    ZBlockID                BlockID     = ZBID_FileHeader;  /**< identification is file header */
  //    unsigned long           ZRFVersion  = __ZRF_VERSION__;  /**< software version */
  ZFile_type              FileType    = ZFT_Nothing ;     /**< File type see ref ZFile_type */
  zaddress_type           OffsetFCB;                      /**< offset to ZFileControlBlock when written in file */
  zaddress_type           OffsetReserved;                 /**< offset to ZReserved section when written in file */
  zsize_type              SizeReserved = 0L;              /**< Size of ZReserved section in file */
  //    uint32_t                EndSign     = cst_ZEND;         /**< check for block end */

  ZHeaderControlBlock (void);
  ZHeaderControlBlock(const ZHeaderControlBlock& pIn) {_copyFrom(pIn);}
  ZHeaderControlBlock(const ZHeaderControlBlock&& pIn) {_copyFrom(pIn);}

  ZHeaderControlBlock& _copyFrom(const ZHeaderControlBlock& pIn);
  ZHeaderControlBlock& operator = (const ZHeaderControlBlock& pIn){_copyFrom(pIn);}

  ZHeaderControlBlock& _fromHCBE(const ZHeaderControlBlock_Export *pIn);

  void clear(void) ;



  utf8String toXml(int pLevel);
  /**
     * @brief fromXml loads header control block from its xml definition and return 0 when successfull.
     * When errors returns <>0 and pErrlog contains appropriate error messages.
     * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
     */
  int fromXml(zxmlNode* pHCBRootNode,ZaiErrors* pErrorlog);

  ZDataBuffer& _exportAppend(ZDataBuffer& pZDBExport);
  ZStatus      _import(const unsigned char *&pPtrIn);
};
#endif // ZHEADERCONTROLBLOCK_H
