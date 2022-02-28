#ifndef ZFILEDESCRIPTOR_H
#define ZFILEDESCRIPTOR_H

#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/zsystemuser.h>

#include <zrandomfile/zrandomfiletypes.h>

#include <zrandomfile/zblock.h>

#include <zrandomfile/zheadercontrolblock.h>
#include <zrandomfile/zfilecontrolblock.h>

#include <zrandomfile/zrfpms.h>


namespace zbs {
class ZLock_struct
{
public:
  zlockmask_type      Lock;
  zaddress_type       Address;  //< only address is non volatile - record rank is dependant from deletion
  ZLock_struct()=default;
  ZLock_struct(const ZLock_struct& pIn) {_copyFrom(pIn);}
  ZLock_struct(const ZLock_struct&& pIn) {_copyFrom(pIn);}

  ZLock_struct& operator = (const ZLock_struct& pIn) {_copyFrom(pIn);}

  ZLock_struct& _copyFrom(const ZLock_struct& pIn)
  {
    Lock=pIn.Lock;
    Address=pIn.Address;
    return *this;
  }

};

//typedef ZArray<ZLock_struct> ZLockPool;     //< Lock pool : this pool stays local to the current process (not written on file) and keeps reference to lock set to ZBAT blocks
class ZLockPool : public ZArray<ZLock_struct>
{
public:
  ZLockPool()=default;
  ZLockPool(const ZLockPool& pIn) {_copyFrom(pIn);}
  ZLockPool(const ZLockPool&& pIn) {_copyFrom(pIn);}

  ZLockPool& operator = (const ZLockPool& pIn) {_copyFrom(pIn);}

  ZLockPool& _copyFrom(const ZLockPool& pIn)
  {
    clear();
    for (long wi=0;wi < pIn.count();wi++)
      push((ZLock_struct)pIn[wi]);

    return *this;
  }
};


class ZBlockPool:public ZArray<ZBlockDescriptor>
{
public:
  typedef ZArray<ZBlockDescriptor> _Base;
  ZBlockPool():ZArray<ZBlockDescriptor>() {}
  ZBlockPool(ZBlockPool& pIn):ZArray<ZBlockDescriptor>(pIn) {}
  ZBlockPool(ZBlockPool&& pIn):ZArray<ZBlockDescriptor>(pIn) {}

  ZBlockPool& operator = (ZBlockPool& pIn){return (ZBlockPool&)ZArray<ZBlockDescriptor>::_copyFrom(pIn);}

  ZDataBuffer&  _exportAppendPool(ZDataBuffer&pZDBExport);
  size_t        getPoolExportSize();
  size_t        _importPool(const unsigned char *&pPtrIn);
};
/*
typedef ZBlockPool ZBlockAccessTable;     //!< Blocks access table pool : contains references to any used block in file (Primary pool)
typedef ZBlockPool ZFreeBlockPool;        //!< Free blocks pool : contains references to any free space in file  (Primary pool)
typedef ZBlockPool ZDeletedBlockPool;     //!< Deleted blocks pool : keep references to dead blocks included into free blocks (Secondary pool)
*/
enum ZHeaderAccess_type : uint8_t
{
  ZHAC_Nothing = 0,
  ZHAC_HCB = 1,
  ZHAC_FCB = 2,
  ZHAC_RESERVED= 4
};

/**
 * @brief The ZFileDescriptor class ZFileDescriptor (further abbreviated ZFD) owns operational infradata IN PROCESS MEMORY to manage behavior of ZRandomFile.
 *
 * ZFileDescriptor is a pure memory data structure used to manage File Header components.
 * The components of ZFileDescriptor are synchronized with the header file content.
 * @note when open exclusive mode (ZRF_Exclusive) is made, then header file content is synchronized once at open time and again at close time.
 *
 * In non exclusivee mode, ZFD components are written in file header and updated as soon as an update operation occurs on file.
 * It is preceeded with a Reserved block that is used by derived classes to store permanent infradata :
 * ZMasterFile and ZIndexFile uses this reserved space to store their own infradata structure.
 *
 * As it is less susceptible to change than operational ZRF infradata, it is stored BEFORE ZFileDescriptor data itself,
 * So that, ZRF header update will not induce rewrite on this portion of file.
 *
 * Size of the reserved infradata area is set with OffsetFCB field.
 *
 *          it is mostly composed of
    - ZHeaderControlBlock : header for the file
        + offset to Reserved
        + offset
    - ZFileControlBlock   : file's operational infradata
 *
 */
class ZFDOwnData  // following is not saved on file and therefore doesn't need to be exported
{
public:
  FILE*   FContent=nullptr;
  int     ContentFd=0;
  FILE*   FHeader=nullptr;
  int     HeaderFd=0;
  pid_t   Pid;                      /* pid of current process owning ZFileDescriptor instance (set at object instantiation )  : other processes are collaborative processes sharing info with it */
  ZSystemUser     Uid;              /* uid of current process owning ZFileDescriptor instance (set at object instantiation ) */
  bool            _isOpen = false;  /* True when file is open , false if closed */
  uint8_t HeaderAccessed=ZHAC_Nothing;  /*  has not been accessed : set to false by close operation - set to true at first access to header data */
  zmode_type   Mode  = ZRF_Nothing; /* Mode mask (int32_t) the file has been openned for see: @ref ZRFMode_type */
  ZHeaderControlBlock ZHeader;
  //    zaddress_type       OffsetFCB=0L;  /**< offset to ZFCB : OL if no derived class infradata space allocation. Else gives the size of reserved space.
  ZFileControlBlock ZFCB;
  ZBlockPool        ZBAT; /** Blocks access table pool : contains references to any used block in file (Primary pool)*/
  ZBlockPool        ZFBT; /** Free blocks pool : contains references to any free space in file  (Primary pool)*/
  ZBlockPool        ZDBT; /** Deleted blocks pool : keep references to dead blocks included into free blocks (Secondary pool)*/
  ZDataBuffer       ZReserved;   /** used by derived classes to store infradata. The first info MUST BE sized to reserved infradata (equals to offsetFCB): gives the offset to effective ZFileDescriptor data.
                                             @ref ZRandomFile::setReservedContent and @ref ZRandomFile::getReservedContent */

  ZLockPool     ZBlockLock;  /**< Locks Pool.Used by ZRandomLock. This pool is NOT Stored on file but stay resident into memory */

  zaddress_type PhysicalPosition=0;  /**< current physical offset from beginning of file: updated by any read / write operation done on ZRandomFile. Not updated by remove operation (set to -1) */
  zaddress_type LogicalPosition=0;   /**< current offset since beginning of data : updated by any read / write operation done on ZRandomFile. Not updated by remove operation (set to -1) */
  long          CurrentRank=0;        /**< current ZBAT rank. set to -1 if no current rank */

  ZFDOwnData() =default;
  ZFDOwnData(const ZFDOwnData& pIn) {_copyFrom(pIn);}
  ZFDOwnData(const ZFDOwnData&& pIn) {_copyFrom(pIn);}

  ZFDOwnData& _copyFrom(const ZFDOwnData& pIn);

  utf8String toXml(int pLevel);
  /**
   * @brief fromXml loads header control block from its xml definition and return 0 when successfull.
   * When errors returns <>0 and pErrlog contains appropriate error messages.
   * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
   */
  int fromXml(zxmlNode* pFDBRootNode, ZaiErrors* pErrorlog);

};//ZFDOwnData


class ZFileDescriptor : public ZFDOwnData
{
  friend class ZRandomFile;
  friend class ZMasterFile;
  friend class ZRawMasterFile;
  friend class ZIndexFile;
  friend class ZMasterFile;
  friend class ZIndexFile;

  friend void _cleanDeletedBlocks(ZFileDescriptor &pDescriptor,ZBlockDescriptor &pBD);
protected:
  uriString URIContent;
  uriString URIHeader;
  uriString URIDirectoryPath;
  // Data should stay here
public:
  ZRFPMS          ZPMS;           /**< performance monitoring system */
  //------------End Data-----------------------
public:
  ZFileDescriptor (void)  {  clear(); }
  ~ZFileDescriptor (void) { }

  ZFileDescriptor(const ZFileDescriptor& pIn) {_copyFrom(pIn);}
  ZFileDescriptor(const ZFileDescriptor&& pIn) {_copyFrom(pIn);}

  ZFileDescriptor& _copyFrom(const ZFileDescriptor& pIn);


  ZFileDescriptor& operator = (const ZFileDescriptor &pIn)
  {
    clear();
    return _copyFrom(pIn);
  }

  ZStatus _close();
  void _forceClose();

  //------------------uriStrings------------------------------------
  uriString& getURIContent(void) {return URIContent;}


  zmode_type       getMode (void) {return Mode;} /**< @brief getMode returns the file's open mode as a zmode */

  /** @brief getPhysicalPosition returns the current physical position */
  inline
  zaddress_type    getPhysicalPosition(void) {return PhysicalPosition;}

  /** @brief getLogicalPosition returns the current logical position */
  inline
  zaddress_type    getLogicalPosition(void) {return LogicalPosition;}

  /** @brief getAllocatedSize returns the current allocated size (content physical file size) */
  inline
  zsize_type       getAllocatedSize(void) { return lseek(ContentFd,0L,SEEK_END);}
  inline
  long             getCurrentRank(void) {return CurrentRank;} /**< @brief returns the current relative rank */

  /** @brief getCurrentPhysicalAddress  returns the current physical address corresponding to current rank */
  inline
  zaddress_type    getCurrentPhysicalAddress(void)
  {
    if (CurrentRank<0)
      return -1;
    return (ZBAT.Tab[CurrentRank].Address);
  }

  /**
   * @brief getRankFromAddress  returns the relative position of record (rank) coresponding to the given physical address within ZRandomFile file
   * @param pAddress    the physical address of the record (beginning of block)
   * @return      Record's relative position within ZBlockAccessTable (ZBAT) if found or -1 if address does not correspond to a valid block address.
   */

  long getRankFromAddress(const zaddress_type pAddress);

  inline
  zaddress_type    getCurrentLogicalAddress(void)
  {
    if (CurrentRank<0)
      return -1;
    return (ZBAT.Tab[CurrentRank].Address-ZFCB.StartOfData);
  }

  /**
 * @brief getCurrentPosition return the current logical address to which is set the current ZRandomFile address.
            This value is updated by any read or write operation and points to the actual block having last been accessed.
            ZRandomFile::zremove operation sets logical position to -1, so that return value of zgetCurrentPosition is not usable in that case.
 * @return
 */
  zaddress_type getCurrentPosition(void)  {return LogicalPosition; }

  /**
     * @brief setPhysicalFromLogical converts a physical to a logical address
     * @param pLogical logical address to convert
     * @return the physical address duly converted
     */
  inline
  zaddress_type    setLogicalFromPhysical (zaddress_type pPhysical) {if (pPhysical<0) return -1; return (pPhysical-ZFCB.StartOfData);}

  /**
     * @brief getAddressFromRank  returns the physical address within file of the relative position of record given by pRank
     * @param pRank  Record's relative position within ZBlockAccessTable (ZBAT)
     * @return      the physical address of the record (beginning of block)
     */

  zaddress_type getAddressFromRank(const zrank_type pRank) {return (ZBAT.Tab[pRank].Address) ;}


  inline
  long  incrementRank(void)
        {
          if (CurrentRank>=ZBAT.lastIdx())
                return -1;
          CurrentRank++;
          return CurrentRank;
        }

  inline
  long  decrementRank(void)
        {
        if (CurrentRank<=0)
          return -1;
        CurrentRank--;
        return CurrentRank;
        }
  inline
  void   incrementPhysicalPosition(const zsize_type pIncrement)
          {
            PhysicalPosition += pIncrement;
            LogicalPosition += pIncrement;
          }

  /**
     * @brief resetPosition gefromXmlt the start of data physical address as Physical Position and align logicalPosition
     * @return
     */
  inline
      zaddress_type resetPosition(void) {PhysicalPosition=ZFCB.StartOfData; setLogicalFromPhysical(PhysicalPosition); return PhysicalPosition;}

  inline
      long    setRank(zrank_type pRank) {CurrentRank=pRank;  setPhysicalPosition(ZBAT.Tab[pRank].Address);  return CurrentRank;}

  inline
      long    resetRank(void) {CurrentRank=0; return CurrentRank;}

  inline
      ZStatus testRank(zrank_type pRank, const char *pModule);


  inline
      void incrementPosition (ssize_t pIncrement) {   PhysicalPosition += pIncrement ;
    LogicalPosition += pIncrement ;}
  inline
      void setPhysicalPosition (zaddress_type pPosition) { PhysicalPosition = pPosition ;
    LogicalPosition = pPosition - ZFCB.StartOfData ;}

  //----------------Get parameters------------------------------------

  /**
     * @brief getAllocatedBlocks Returns the current parameter AllocatedBlocks from file descriptor in memory (no access to file header)
     * @return AllocatedBlocks parameter
     */
  long getAllocatedBlocks(void) {return ZFCB.AllocatedBlocks;}
  /**
     * @brief getBlockExtentQuota  Returns the current parameter BlockExtentQuota from file descriptor in memory (no access to file header)
     * @return BlockExtentQuota
     */
  long getBlockExtentQuota(void) {return ZFCB.BlockExtentQuota;}
  /**
     * @brief getBlockTargetSize  Returns the current parameter BlockTargetSize from file descriptor in memory (no access to file header)
     * @return BlockTargetSize
     */
  zsize_type getBlockTargetSize(void) {return ZFCB.BlockTargetSize;}
  /**
     * @brief getInitialSize  Returns parameter InitialSize from file descriptor in memory (no access to file header)
     * @return BlockTargetSize
     */
  zsize_type getInitialSize(void) {return ZFCB.InitialSize;}
  /**
     * @brief getHighwaterMarking Returns the current option HighwaterMarking from file descriptor in memory (no access to file header)
     * @return true if option is set and false if not
     */
  bool getHighwaterMarking(void) {return ZFCB.HighwaterMarking;}
  /**
     * @brief getGrabFreeSpace Returns the current option HighwaterMarking from file descriptor in memory (no access to file header)
     * @return  true if option is set and false if not
     */
  bool getGrabFreeSpace(void) {return ZFCB.GrabFreeSpace;}





    void clear (void)
      {
      clearFCB();
//      memset (this,0,sizeof(ZFDOwnData));
      FHeader=nullptr;
      HeaderFd=0;
      FContent=nullptr;
      ContentFd=0;

      Pid= getpid();  // get current pid for ZFileDescriptor
      Uid.setToCurrentUser();

      CurrentRank=-1;
      PhysicalPosition = -1;
      LogicalPosition = -1;

      _isOpen=false;
      HeaderAccessed=ZHAC_Nothing;
      Mode  = ZRF_Nothing ;

      ZHeader.clear();

      /* pools */
      ZBlockLock.clear();

      ZBAT.clear();
      ZFBT.clear();
      ZDBT.clear();
      ZReserved.clear();
    //                        ZSystemUser wUser;
    //                        Uid.Username = wUser.setToCurrentUser().getName().toString();
    return;
  }
  /**
     * @brief clearPartial reset (set to zero) partially until uriString section ZFileControlBlock in order to keep uriStrings pathes  for file.
     *      This routine is used by ZRandomFile::_close method in order to offer the possibility to re-open the file without specifying again pathname.
     */
  void clearPartial (void);


  ZStatus setPath (const uriString &pURIPath);
//  void setupFCB (void);
  void clearFCB (void);

  utf8String toXml(int pLevel, bool pComment=true);
  /**
     * @brief fromXml loads file descriptor from its xml definition and returns ZS_SUCCESS when successfull.
     * When errors returns <>0 and pErrlog contains appropriate error messages.
     * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
     */
  ZStatus fromXml(zxmlNode* pFDRootNode, ZaiErrors* pErrorlog);


  ZDataBuffer& _exportFCB(ZDataBuffer& pZDBExport);
  ZFileDescriptor& _importFCB(const unsigned char *pPtrIn);


};//ZFileDescriptor

} // namespace zbs

#endif // ZFILEDESCRIPTOR_H
