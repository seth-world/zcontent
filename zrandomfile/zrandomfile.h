#ifndef ZRANDOMFILE_H
#define ZRANDOMFILE_H

#include <zrandomfile/zrfconfig.h>

#ifdef __USE_WINDOWS__
#include <io.h>
#else
#include <pwd.h>     // for pwd user data structure
#include <unistd.h>  // for lseek
#endif

#include <ztoolset/zerror.h>
#include <ztoolset/zfunctions.h>
#include <ztoolset/zbasedatatypes.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/uristring.h>
#include <zrandomfile/zrandomfiletypes.h>
#include <ztoolset/zuser.h>

#ifndef ZRANDOMFILE_CPP

    extern bool ZRFStatistics;
#endif
    void setZRFStatistics (bool pStatistics) ;
/** @} */


typedef long zrank_type;


namespace zbs {



/**
 * @brief The ZRFPMS class Performance monitoring system : collects data about read / write operations on content and header during a session.

    ZRandomFile collects information about its activity.
    Data is stored within ZRFPMS class associated to ZFileDescriptor.
    This data has a life duration limitated to a session.

    @Note a session starts from the ZRandomFile::_open method call until ZRandomFile::_close call

    It could be reported at any time using ZRandomFile performance reporting method.
    @see ZRandomFile::ZRFPMSReport

 */
class ZRFPMS
{
public:
    long        HFHReads;       /**< file header reads (pure header block) */
    long        HFHWrites;      /**< file header writes (pure header block) */

    long        LockReads;       /**< file header lock infos reads (pure header block) */
    long        LockWrites;      /**< file header lock infos writes (pure header block) */

    long        HReservedReads;     /**< reserved block reads */
    long        HReservedWrites;    /**< reverved block writes */

    long        HFDReads;       /**< file descriptor reads */
    long        HFDWrites;      /**< file descriptor writes */

    long        CBHReads;           /**< content block header reads */
    zsize_type  CBHReadBytesSize;   /**< content block header bytes read */

    long        CBHWrites;      /**< content block header writes */
    zsize_type  CBHWriteBytesSize;  /**< content block header bytes written */

    long        UserReads;      /**< user content reads */
    zsize_type  UserReadSize;   /**< total of user content reads in bytes */
    long        UserWrites;     /**< user content writes */
    zsize_type  UserWriteSize;  /**< total of user content writes in bytes */

    long        HighWaterWrites;    /**< total of highwater marking writes */
    zsize_type  HighWaterBytesSize; /**< total bytes written while highwatermarking writting */

    long        ExtentWrites;   /**< number of times file has been extended (faults on the free block pool) */
    zsize_type  ExtentSize;     /**< total size of file has been extended. */
    long        FreeMatches;    /**< number of times a block has been found and allocated from free pool. */

    long        FieldReads;     /**< single field content reads */
    zsize_type  FieldReadSize;  /**< total of single field content reads in bytes */
    long        FieldWrites;     /**< single field  content writes */
    zsize_type  FieldWriteSize;  /**< total of single field content writes in bytes */


    ZRFPMS & operator = (ZRFPMS &pPMSIn) {memmove (this,&pPMSIn,sizeof(ZRFPMS)); return *this;}
    ZRFPMS  operator - (ZRFPMS &pPMSIn);
    ZRFPMS  operator + (ZRFPMS &pPMSIn);

    void clear(void) {memset(this,0,sizeof(ZRFPMS)); return;}
    void reportDetails (FILE*pOutput=stdout);

    void PMSCounterRead(ZPMSCounter_type pC, const zsize_type pSize=0);
    void PMSCounterWrite(ZPMSCounter_type pC, const zsize_type pSize=0);


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
/** @cond Development */

/**
  @defgroup ZRFHeader ZRandomFile file header structure
  Each ZRandomFile content file has a header file which it associated to him.

 This header file contains critical information for its management and usage.

 The file header infradata is written and maintained within header file from memory.
 ZFileDescriptor class owns in memory the whole infradata.

 @note file header, globally or one of its component (ZFileControlBlock) is updated to file as soon as there is a modification in one of its components.
 This is a way to maintain file's integrity as well as to share the file's infradata with other local/remote processes.
 However, there is a way to speed up application execution in minimizing file access in some conditions :
 @note
 Exclusive access to file is set at open time with a zmode mask mentionning ZRF_Exclusive.
 Then IN CASE OF EXCLUSIVE ACCESS, and by default, headers components are loaded once when file is opened, and written once, when file is closed.
 At any time, it is possible to force  header component to be written on file in positionning pForceRead or pForceWrite boolean to true in the appropriate routine call argument.
 By default, these arguments are set to false.
 @note This argument is of no impact when file is not opened with ZRF_Exclusive mask set to zmode.

 @note
  Concerned routines are
  - getting header component
    + ZRandomFile::_getFullFileHeader()
    + ZRandomFile::_getFileHeader()
    + ZRandomFile::_getReservedHeader()
    + ZRandomFile::_getFileDescriptor()
  - writing header component
    + ZRandomFile::_writeFullFileHeader()
    + ZRandomFile::_writeFileHeader()
    + ZRandomFile::_writeReservedHeader()
    + ZRandomFile::_writeFileDescriptor()

@par ZRandomFile Header simplified map
@verbatim
OL                                                                                                         expansion direction
!--------------------------!-----------------------------------!-----------------------------------!-------!------!---------->
 ZHeaderControlBlock       |    ZReserved                      |   ZFileControlBlock               | ZBAT  |ZFBT  | ZDBT
                           |    (used by derived classes       |     ZBAT_Offset (relative to FCB)-+ pool  |pool  | pool
     ZReserved_Offset------+     as ZMasterFile & ZIndexFile)  |     ZFBT_Offset (relative to FCB)---------+      |
                                                               |     ZDBT_Offset (relative to FCB)----------------+
     ZFCB_Offset-----------------------------------------------+

@endverbatim

@{

*/


class ZBlockPool:public ZArray<ZBlockDescriptor>
{
public:
    typedef ZArray<ZBlockDescriptor> _Base;


    ZDataBuffer& _exportPool(ZDataBuffer&pZDBExport);
    size_t _importPool(unsigned char *pBuffer);
};

typedef ZBlockPool ZBlockAccessTable;     //!< Blocks access table pool : contains references to any used block in file (Primary pool)
typedef ZBlockPool ZFreeBlockPool;        //!< Free blocks pool : contains references to any free space in file  (Primary pool)
typedef ZBlockPool ZDeletedBlockPool;     //!< Deleted blocks pool : keep references to dead blocks included into free blocks (Secondary pool)

struct ZLock_struct{
    zlockmask_type          Lock;
    zaddress_type       Address;  //< only address is non volatile - record rank is dependant from deletion
};
typedef ZArray<ZLock_struct> ZLockPool;     //< Lock pool : this pool stays local to the current process (not written on file) and keeps reference to lock set to ZBAT blocks

typedef void  (*ZFCBgetReserved) (ZDataBuffer &) ;
#pragma pack(push)
#pragma pack(0)
/**
 * @brief The ZHeaderControlBlock_Export class
 *
 *  Nota Bene : file lock infos are positionned at the block beginning right after StartSign to be quickly read/write accessed within file
 *
 *
 */
class ZHeaderControlBlock_Export
{
public:
    uint32_t                StartSign   = cst_ZSTART;       /**< check for block start */

    zlockmask_type              Lock    = ZLock_Nolock ;        /**< Lock mask (int32_t) at file header level (Exclusive lock) @see ZLockMask_type one lock at a time is authorized */
    pid_t                   LockOwner = 0L;                 /**< Owner process for the lock */

    ZBlockID                BlockID     = ZBID_FileHeader;  /**< identification is file header */
    unsigned long           ZRFVersion  = __ZRF_VERSION__;  /**< software version */
    ZFile_type              FileType    = ZFT_Nothing ;     /**< File type see ref ZFile_type */
    zaddress_type           OffsetFCB;                      /**< offset to ZFileControlBlock when written in file */
    zaddress_type           OffsetReserved;                 /**< offset to ZReserved section when written in file */
    zsize_type              SizeReserved = 0L;              /**< Size of ZReserved section in file */

    uint32_t                EndSign     = cst_ZEND;         /**< check for block end */
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

    zlockmask_type              Lock    = ZLock_Nolock ;        /**< Lock mask (int32_t) at file header level (Exclusive lock) @see ZLockMask_type one lock at a time is authorized */
    pid_t                   LockOwner = 0L;                 /**< Owner process for the lock */

//    ZBlockID                BlockID     = ZBID_FileHeader;  /**< identification is file header */
//    unsigned long           ZRFVersion  = __ZRF_VERSION__;  /**< software version */
    ZFile_type              FileType    = ZFT_Nothing ;     /**< File type see ref ZFile_type */
    zaddress_type           OffsetFCB;                      /**< offset to ZFileControlBlock when written in file */
    zaddress_type           OffsetReserved;                 /**< offset to ZReserved section when written in file */
    zsize_type              SizeReserved = 0L;              /**< Size of ZReserved section in file */
//    uint32_t                EndSign     = cst_ZEND;         /**< check for block end */

    ZHeaderControlBlock (void)          {clear();}

    void clear(void) ;
    ZDataBuffer& _export(ZDataBuffer& pZDBExport);
    ZStatus      _import(unsigned char* pZDBImport_Ptr);
};
struct lockPack_struct
{
    zlockmask_type              Lock    = ZLock_Nolock ;        /**< Lock mask (int32_t) at file header level (Exclusive lock) @see ZLockMask_type one lock at a time is authorized */
    pid_t                   LockOwner = 0L;                 /**< Owner process for the lock */
};
#pragma pack(push)
#pragma pack(0)
class ZFileControlBlock_Export
{
public:
    uint32_t        StartSign=cst_ZSTART ;         /**< StartSign word that mark start of data */
    ZBlockID        BlockID;            /**< Block id is set to ZBID_FCB */
    zaddress_type   StartOfData;        /**< offset where Data storage starts : 0L */
    unsigned long   AllocatedBlocks;            /**< for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT */
    unsigned long   BlockExtentQuota;            /**< for ZBAT & ZFBT : initial extension quota */

    size_t          ZBAT_DataOffset;            /**< Written on file header : Offset to Blocks Access Table array since begining of ZFCB */
    size_t          ZBAT_ExportSize;            /**<  Written on file header : size in bytes of ZBAT : to be written on file. This size is the global ZArray size in bytes */

    size_t          ZFBT_DataOffset;           /**< offset to Free Blocks Table array since begining of ZFCB */
    size_t          ZFBT_ExportSize;           /**< size in bytes of ZFBT : to be written on file */

    size_t          ZDBT_DataOffset;           /**< offset to Deleted Blocks Table array since begining of ZFCB */
    size_t          ZDBT_ExportSize;           /**< size in bytes of ZDBT : to be written on file */

    size_t          ZReserved_DataOffset;      /**<  Written on file header : Reserved space address . Must be 0L */
    size_t          ZReserved_ExportSize;      /**<  given by _getReservedSize */

//    void             (*_getReserved) (ZDataBuffer &) ;// routine to load zreserved from derived class

    zsize_type    InitialSize;                  /**< Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk */
    zsize_type    AllocatedSize;              /**< Total current allocated size in bytes for file */
    zsize_type    UsedSize;                   /**< Total of currently used size within file in bytes */
//    zsize_type    ExtentSizeQuota;            // extent quota size in bytes for file : no more used

    size_t   MinSize;                    /**< statistical value : minimum length of block record in file  (existing statistic) */
    size_t   MaxSize;                    /**< statistical value : maximum length of block record in file (existing statistic ) */
    size_t   BlockTargetSize;           /**< Block target size (user defined value) Foreseen medium size of blocks in a varying block context. */

/*    bool            History;
    bool            Autocommit;
    bool            Journaling;*/  // ZRandomFile Does NOT have journaling, history, autocommit : see ZMasterFile instead

    uint8_t         HighwaterMarking;           /**< mark to zero the whole deleted block content when removed */
    uint8_t         GrabFreeSpace;              /**< attempts to grab free space and holes at each block free operation */
    uint32_t        EndSign=cst_ZEND;           /**< EndSign word that marks end of data */

};
#pragma pack(pop)

/**
 * @brief The ZFileControlBlock class contains all operational information of a ZRandomFile.

This block is written in header file.

It contains in particular the 3 block pools :
   - ZBlockAccessTable (ZBAT): block pool that gives for a used block its address within the file using its rank number
   - ZFreeBlockPool (ZFBT): block pool of free/deleted blocks available for usage in record creation process
   - ZDeletedBlockPool (ZDBT): pool of deleted block before they could have been grabbed by grab mechanism

@par ZFileDescriptor simplified map
@verbatim


        each pool size is calculated using ZArray<>::getExportSize() method

0L                                                                                              Expansion
+-------Variables---------------+-----------------------+--------------------------+-------------------------->

xxxx @ZBAT  @ZFBT xxxxxxxxxxxxxxx......ZBAT content......,,,,,,,,,,ZFBT content.....,,,,,,,,,,ZDBT content.....

@endverbatim

@par BlockTargetSize
    Foreseen medium size of blocks in a varying block context.
    This value is a important value used for allocation of free blocks as well as searches for physical block headers
    @see ZRandomFile::_getFreeBlock() ZRandomFile::_searchNextPhysicalBlock() ZRandomFile::_searchPreviousPhysicalBlock() ZRandomFile::_surfaceScan()

@par StartOfData
    Offset from the beginning of the content file where data blocks start to be stored.
    This value cannot be changed by user

@par AllocatedBlocks
    for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT

@par BlockExtentQuota
    for ZBAT & ZFBT : initial extension quota

@par InitialSize
    This is the initial space the file has been created with.
    This value is used when ZRandomFile::zclear() : file size is kept at this minimum.

@par AllocatedSize
    Total current allocated size in bytes for file.

@par UsedSize
    Total of currently used size within file in bytes (Meaning : sum of user record sizes for ZBlockAccessTable pool )

@par Statistical values

   - MinSize minimum length of block record in file  (existing statistic)
   - MaxSize maximum length of block record in file  (existing statistic)

    These values are not dynamic values. They are updated via different tools and are not systematically accurate.
    @see ZRandomFile::ZRFstat ZRandomFile::zreorgFile

@par HighwaterMarking (Option)
    When this option is set, freed blocks are marked to binary zero.

@par GrabFreeSpace (Option)
    When this option is set, ZRandomFile will try to gather adjacent free blocks and holes together when a _freeBlock is invoked.



*/
class ZFileControlBlock
{
friend class ZFileDescriptor;
friend class ZRandomFile;
friend class ZMasterFile;
friend class ZIndexFile;
public:
    ZFileControlBlock (void) {clear(); return;}

    uint32_t       StartSign=cst_ZSTART ;         /**< StartSign word that mark start of data */
    ZBlockID       BlockID;            /**< Block id is set to ZBID_FCB */
private:
    zaddress_type  StartOfData;        /**< offset where Data storage starts : 0L */


public:
    unsigned long   AllocatedBlocks;            /**< for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT */
    unsigned long   BlockExtentQuota;            /**< for ZBAT & ZFBT : initial extension quota */

    size_t          ZBAT_DataOffset;            /**< Written on file header : Offset to Blocks Access Table array since begining of ZFCB */
    size_t          ZBAT_ExportSize;            /**<  Written on file header : size in bytes of ZBAT : to be written on file. This size is the global ZArray size in bytes */

    size_t          ZFBT_DataOffset;           /**< offset to Free Blocks Table array since begining of ZFCB */
    size_t          ZFBT_ExportSize;           /**< size in bytes of ZFBT : to be written on file */

    size_t          ZDBT_DataOffset;           /**< offset to Deleted Blocks Table array since begining of ZFCB */
    size_t          ZDBT_ExportSize;           /**< size in bytes of ZDBT : to be written on file */

    size_t          ZReserved_DataOffset;      /**<  Written on file header : Reserved space address . Must be 0L */
    size_t          ZReserved_ExportSize;      /**<  given by _getReservedSize */

//    void             (*_getReserved) (ZDataBuffer &) ;// routine to load zreserved from derived class

    zsize_type    InitialSize;                  /**< Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk */
    zsize_type    AllocatedSize;              /**< Total current allocated size in bytes for file */
    zsize_type    UsedSize;                   /**< Total of currently used size within file in bytes */
//    zsize_type    ExtentSizeQuota;            // extent quota size in bytes for file : no more used

    size_t   MinSize;                    /**< statistical value : minimum length of block record in file  (existing statistic) */
    size_t   MaxSize;                    /**< statistical value : maximum length of block record in file (existing statistic ) */
    size_t   BlockTargetSize;           /**< Block target size (user defined value) Foreseen medium size of blocks in a varying block context. */

/*    bool            History;
    bool            Autocommit;
    bool            Journaling;*/  // ZRandomFile Does NOT have journaling, history, autocommit : see ZMasterFile instead

    uint8_t         HighwaterMarking;           /**< mark to zero the whole deleted block content when removed */
    uint8_t         GrabFreeSpace;              /**< attempts to grab free space and holes at each block free operation */
    uint32_t        EndSign=cst_ZEND;           /**< EndSign word that marks end of data */


    void clear(void) {memset(this,0,sizeof(ZFileControlBlock));
                      StartSign=cst_ZSTART;
                      StartOfData = 0L;
                      BlockID = ZBID_FCB;
                      EndSign=cst_ZEND;
                      return;}

    ZDataBuffer& _export(ZDataBuffer& pZDBExport);
    ZFileControlBlock& _import(unsigned char* pZDBImport_Ptr);
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
struct ZFDOwnData {             // following is not saved on file and therefore doesn't need to be exported
    FILE*   FContent=nullptr;
    int     ContentFd=0L;
    FILE*   FHeader=nullptr;
    int     HeaderFd=0L;
    pid_t   Pid;                        /**< pid of current process owning ZFileDescriptor instance (set at object instantiation )  : other processes are collaborative processes sharing info with it */
//    uid_t   Uid;                        /**< uid of current process owning ZFileDescriptor instance (set at object instantiation ) */
    ZUserId Uid;                        /**< uid of current process owning ZFileDescriptor instance (set at object instantiation ) */
    utfcodeString Username;                /**< current system username */
    bool    _isOpen = false;            /**< True when file is open , false if closed */
    zmode_type   Mode    = ZRF_Nothing; /**< Mode mask (int32_t) the file has been openned for see: @ref ZRFMode_type */
    ZHeaderControlBlock ZHeader;
//    zaddress_type       OffsetFCB=0L;  /**< offset to ZFCB : OL if no derived class infradata space allocation. Else gives the size of reserved space.
    ZFileControlBlock*  ZFCB=nullptr;
    ZBlockAccessTable*  ZBAT=nullptr;
    ZFreeBlockPool*     ZFBT=nullptr;
    ZDeletedBlockPool*  ZDBT=nullptr;
    ZDataBuffer         ZReserved;   /**< used by derived classes to store infradata. The first info MUST BE sized to reserved infradata (equals to offsetFCB): gives the offset to effective ZFileDescriptor data.
                                             @ref ZRandomFile::setReservedContent and @ref ZRandomFile::getReservedContent */

    ZLockPool           ZBlockLock;  /**< Locks Pool.Used by ZRandomLock. This pool is NOT Stored on file but stay resident into memory */

    zaddress_type PhysicalPosition;  /**< current physical offset from beginning of file: updated by any read / write operation done on ZRandomFile. Not updated by remove operation (set to -1) */
    zaddress_type LogicalPosition;   /**< current offset since beginning of data : updated by any read / write operation done on ZRandomFile. Not updated by remove operation (set to -1) */
    long        CurrentRank;        /**< current ZBAT rank. set to -1 if no current rank */
};//ZFDOwnData
class ZFileDescriptor: public ZFDOwnData
{
    friend class ZRandomFile;
    friend class ZMasterFile;
    friend class ZIndexFile;
    friend class ZSMasterFile;
    friend class ZSIndexFile;

    friend void _cleanDeletedBlocks(ZFileDescriptor &pDescriptor,ZBlockDescriptor &pBD);
public:
    ZFileDescriptor (void)  {  setupFCB(); }
    ~ZFileDescriptor (void) { }
// Data should stay here
public:
    ZRFPMS          ZPMS;           /**< performance monitoring system */

//------------------uriStrings------------------------------------
    uriString& getURIContent(void) {return URIContent;}
protected:
    uriString URIContent;
    uriString URIHeader;
    uriString URIDirectoryPath;
//------------End Data-----------------------

    zmode_type       getMode (void) {return Mode;} /**< @brief getMode returns the file's open mode as a zmode */

    inline
    zaddress_type    getPhysicalPosition(void) {return PhysicalPosition;} /**< @brief returns the current physical position */

    inline
    zaddress_type    getLogicalPosition(void) {return LogicalPosition;} /**< @brief returns the current logical position */

    inline
    zsize_type       getAllocatedSize(void) { return lseek(ContentFd,0L,SEEK_END);} /**< returns the current allocated size (content physical file size) */

    inline
    long             getCurrentRank(void) {return CurrentRank;} /**< @brief returns the current relative rank */

    inline
    zaddress_type    getCurrentPhysicalAddress(void)    /**< returns the current physical address corresponding to current rank */
                                {if (CurrentRank<0)
                                                return -1;
                                 return (ZBAT->Tab[CurrentRank].Address);}
    inline
    zaddress_type    getCurrentLogicalAddress(void)
                                {if (CurrentRank<0)
                                                return -1;
                                 return (ZBAT->Tab[CurrentRank].Address-ZFCB->StartOfData);}

    inline
    zaddress_type    setLogicalFromPhysical (zaddress_type pPhysical) {if (pPhysical<0) return -1; return (pPhysical-ZFCB->StartOfData);}

    inline
    long  incrementRank(void) {if (CurrentRank>=ZBAT->lastIdx())
                                                        return -1;
                                              CurrentRank++;
                                              return CurrentRank;}
    inline
    long  decrementRank(void) {if (CurrentRank<=0)
                                                        return -1;
                                              CurrentRank--;
                                              return CurrentRank;}
    inline
    void   incrementPhysicalPosition(const zsize_type pIncrement)
                    {PhysicalPosition += pIncrement; LogicalPosition += pIncrement; return;}

    /**
     * @brief resetPosition get the start of data physical address as Physical Position and align logicalPosition
     * @return
     */
    inline
    zaddress_type resetPosition(void) {PhysicalPosition=ZFCB->StartOfData; setLogicalFromPhysical(PhysicalPosition); return PhysicalPosition;}

    inline
    long    setRank(zrank_type pRank) {CurrentRank=pRank;  setPhysicalPosition(ZBAT->Tab[pRank].Address);  return CurrentRank;}

    inline
    long    resetRank(void) {CurrentRank=0; return CurrentRank;}

    inline
    ZStatus testRank(zrank_type pRank, const char *pModule);


    inline
    void incrementPosition (ssize_t pIncrement) {   PhysicalPosition += pIncrement ;
                                                    LogicalPosition += pIncrement ;}
    inline
    void setPhysicalPosition (zaddress_type pPosition) { PhysicalPosition = pPosition ;
                                                         LogicalPosition = pPosition - ZFCB->StartOfData ;}

    inline
    void clear (void) { clearFCB();
                        ZBlockLock.clear();
                        ZReserved.clear();
                        memset (this,0,sizeof(ZFDOwnData));
                        CurrentRank=-1;
                        PhysicalPosition = -1;
                        LogicalPosition = -1;
                        _isOpen = false ;
                        Pid= getpid();  // get current pid for ZFileDescriptor
                        Uid.current();
                        ZUser wUser;
                        Username = wUser.setToCurrentUser().getName().toString();
                        return;
                      }
    /**
     * @brief clearPartial reset (set to zero) partially until uriString section ZFileControlBlock in order to keep uriStrings pathes  for file.
     *      This routine is used by ZRandomFile::_close method in order to offer the possibility to re-open the file without specifying again pathname.
     */
    inline
    void clearPartial (void)
                        { clearFCB();

                        memset (this,0,(sizeof(ZFDOwnData)));
                        CurrentRank=-1;
                        PhysicalPosition = -1;
                        LogicalPosition = -1;
                        _isOpen = false ;
                        Pid= getpid();  // get current pid for ZFileDescriptor
                             }




    virtual ZStatus setPath (uriString &pURIPath);
    void setupFCB (void);
    void clearFCB (void);

    ZDataBuffer& _exportFCB(ZDataBuffer& pZDBExport);
    ZFileDescriptor& _importFCB(unsigned char* pFileControlBlock_Ptr);

    ZFileDescriptor& operator = (const ZFileDescriptor &pDesc)  {clear(); memmove(this,&pDesc,sizeof(ZFileDescriptor)); return *this;}
};//ZFileDescriptor
/** @endcond */


/** @} */  // ZHeaderGroup

class ZRFCollection;

/**
 * @brief The ZRandomFile class This class holds the tools to manage the whole ZRandomFile structure
 *
 * Instantiating this class allow to benefit from any underneeth objects.
 * There is one ZRandomFile object per ZRandomFile physical structure.
 * @see ZRFPhysical
 *
 */
class ZRandomFile
{
 friend class ZFileDescriptor ;
 friend class ZIndexFile;
 friend class ZMasterFile;

 friend class ZSIndexFile;
 friend class ZSMasterFile;

 friend class ZOpenZRFPool;
public:

// ZBlockDescriptor CurrentBlockDescriptor;

 ZDataBuffer CurrentRecord;

 //-----------Shared section----------------

 FILE* FHistory;
 uriString URIHistory;

 //--------------End Shared section -----------------
protected:

 ZFileDescriptor ZDescriptor;

public:

    ZRandomFile() {    }
    ZRandomFile(uriString pURI) ;
    ~ZRandomFile(void)
    {
        if (ZDescriptor._isOpen)
             {
             _close(ZDescriptor);
            }
    }

//! @cond Test
    void putTheMess (void);
//! @endcond

    ZStatus setPath (uriString pURIPath);

//----------------Set parameters------------------------------------

    ZStatus setHighwaterMarking (const bool pHighwaterMarking);
    ZStatus setGrabFreeSpace (const bool pGrabFreeSpace) ;
    ZStatus setBlockTargetSize (const ssize_t pBlockTargetSize) ;

    ZStatus _setParameters (ZFileDescriptor &pDescriptor,
                            ZFile_type pFileType,
                            const bool pGrabFreeSpace,
                            const bool pHighwaterMarking,
                            const ssize_t pBlockTargetSize);
//----------------Get parameters------------------------------------

    /**
     * @brief getAllocatedBlocks Returns the current parameter AllocatedBlocks from file descriptor in memory (no access to file header)
     * @return AllocatedBlocks parameter
     */
    long getAllocatedBlocks(void) {return ZDescriptor.ZFCB->AllocatedBlocks;}
    /**
     * @brief getBlockExtentQuota  Returns the current parameter BlockExtentQuota from file descriptor in memory (no access to file header)
     * @return BlockExtentQuota
     */
    long getBlockExtentQuota(void) {return ZDescriptor.ZFCB->BlockExtentQuota;}

//    zsize_type getAllocatedSize(void) {return ZDescriptor.ZFCB->AllocatedSize;}
    /**
     * @brief getAllocatedSize Returns the current effective content file size (AllocatedSize) for the file descriptor (content file is accessed).
     * @return AllocatedSize
     */
    zsize_type getAllocatedSize(void) {return ZDescriptor.getAllocatedSize();}
    /**
     * @brief getBlockTargetSize  Returns the current parameter BlockTargetSize from file descriptor in memory (no access to file header)
     * @return BlockTargetSize
     */
    zsize_type getBlockTargetSize(void) {return ZDescriptor.ZFCB->BlockTargetSize;}
    /**
     * @brief getInitialSize  Returns parameter InitialSize from file descriptor in memory (no access to file header)
     * @return BlockTargetSize
     */
    zsize_type getInitialSize(void) {return ZDescriptor.ZFCB->InitialSize;}
    /**
     * @brief getHighwaterMarking Returns the current option HighwaterMarking from file descriptor in memory (no access to file header)
     * @return true if option is set and false if not
     */
    bool getHighwaterMarking(void) {return ZDescriptor.ZFCB->HighwaterMarking;}
    /**
     * @brief getGrabFreeSpace Returns the current option HighwaterMarking from file descriptor in memory (no access to file header)
     * @return  true if option is set and false if not
     */
    bool getGrabFreeSpace(void) {return ZDescriptor.ZFCB->GrabFreeSpace;}


//-------------Lock management----------------------------
//  Lock must be here

//----------------File Lock/unlock-------------------------
//
/**
 * @brief zlockFile Locks the whole file with a lock mask given by pLock see @ref ZLockMask_type
 * @param[in] pLock the lock mask
 * @param[in] pForceWrite force to write file's header in any case
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
    ZStatus zlockFile(const zlockmask_type pLock,bool pForceWrite=true) {return _lockFile(ZDescriptor,pLock,pForceWrite);}
/**
 * @brief zunlockFile Unlocks the whole file see @ref ZLockMask_type
 * @param[in] pForceWrite force to write file's header in any case
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

    ZStatus zunlockFile(bool pForceWrite=true) {return _unlockFile(ZDescriptor,pForceWrite);}

static
    ZStatus _unlockFile (ZFileDescriptor &pDescriptor, bool pForceWrite=true);

static
    ZStatus _lockFile (ZFileDescriptor &pDescriptor, const zlockmask_type pLock, bool pForceWrite=true);

static
    bool _isFileLocked (ZFileDescriptor &pDescriptor, bool pForceRead=true);

    static
    ZStatus _writeFileLock(ZFileDescriptor &pDescriptor, lockPack_struct &pLockPack);
    static
    ZStatus _readFileLock(ZFileDescriptor &pDescriptor, lockPack_struct &pLockPack);




 void setCreateMinimum(const zsize_type pInitialSize);

 void setCreateMaximum (const zsize_type pInitialSize,
                        const long pAllocatedBlocks,
                        const long pBlockExtentQuota,
                        const long pBlockTargetSize,
                        const bool pHighwaterMarking,
                        const bool pGrabFreeSpace);

 ZStatus zcreate(const uriString & pFilename,
                 const zsize_type pInitialSize,
                 long pAllocatedBlocks,
                 long pBlockExtentQuota,
                 long pBlockTargetSize,
                 bool pHighwaterMarking=false,
                 bool pGrabFreeSpace=true,
                 bool pBackup=false,
                 bool pLeaveOpen=false);

 ZStatus zcreate(const char* pFilename,
                 const zsize_type pInitialSize,
                 long pAllocatedBlocks,
                 long pBlockExtentQuota,
                 long pBlockTargetSize,
                 bool pHighwaterMarking=false,
                 bool pGrabFreeSpace=true,
                 bool pBackup=false,
                 bool pLeaveOpen=false);

    ZStatus zcreate(const zsize_type pInitialSize,
                    long pAllocatedBlocks,
                    long pBlockExtentQuota,
                    long pBlockTargetSize,
                    bool pHighwaterMarking=false,
                    bool pGrabFreeSpace=true,
                    bool pBackup=false,
                    bool pLeaveOpen=false);



    ZStatus zcreate (const uriString & pFilename,
                     const zsize_type pInitialSize,
                     bool pBackup=false,
                     bool pLeaveOpen=false);

    ZStatus zcreate (const char * pFilename,
                     const zsize_type pInitialSize,
                     bool pBackup=false,
                     bool pLeaveOpen=false);

    ZStatus zcreate (const zsize_type pInitialSize,
                     bool pBackup=false,
                     bool pLeaveOpen=false) ;

    ZStatus zremoveFile (void) ;

    /**
     * @brief isOpen test whether file is open or not
     * @return true if file is open false if not
     */
    bool            isOpen(void)  {return ZDescriptor._isOpen;}
    /**
     * @brief getMode returns the open mode of the file as a zmode_type
     */
    zmode_type    getMode(void) { return (zmode_type)ZDescriptor.Mode;}
    ZStatus zopen(const zmode_type pMode);
    ZStatus zopen(const char *pFilename,const zmode_type pMode);
    ZStatus zopen(const uriString &pFilename,const zmode_type pMode);

    ZStatus zclose(void) ;
// -------User routines------------------------------

    ZStatus zget(void* pRecord, size_t &pSize, const zrank_type pRank);
    ZStatus zget(ZDataBuffer &pRecordContent, const zrank_type pRank);

    ZStatus zgetNext (void* pUserRecord, size_t& pSize);
    ZStatus zgetNext (ZDataBuffer &pUserRecord);

    void    zrewind (void) { ZDescriptor.setRank(0L); }

    ZStatus zgetByAddress (ZDataBuffer&pRecord, zaddress_type pAddress);

    ZStatus zgetWAddress(ZDataBuffer &pRecord, const zrank_type pRank, zaddress_type &pAddress);
    ZStatus zgetNextWAddress(ZDataBuffer &pRecord, zrank_type &pRank, zaddress_type &pAddress);
    ZStatus zgetLastWAddress(ZDataBuffer &pRecord, zrank_type &pRank,zaddress_type &pAddress);
    ZStatus zgetPreviousWAddress(ZDataBuffer &pRecord, zrank_type &pRank, zaddress_type &pAddress);

    zrank_type searchBlockRankByAddress (ZFileDescriptor &pDescriptor, zaddress_type pAddress);


    ZStatus zaddWithAddress (ZDataBuffer&pRecord, zaddress_type &pAddress);

    ZStatus zgetLast (ZDataBuffer &pRecord, zrank_type &pRank);

    ZStatus zgetPrevious (ZDataBuffer &pRecord,const int pLock=0);


//-----------By Field operations--------------------------


    ZStatus zgetSingleField (ZDataBuffer& pFieldContent,
                             const zrank_type pRank,
                             const ssize_t pOffset,
                             const ssize_t &pLength,
                             const int pLock);

    ZStatus zwriteSingleField (ZDataBuffer& pFieldContent,
                               const zrank_type pRank,
                               const ssize_t pOffset,
                               const ssize_t &pLength,
                               int &pLock);

// ---------------Fields search-----------------------


        ZStatus
        zsearchFieldAllCollection (ZRFCollection &pCollection,
                                   const zlockmask_type pLock=ZLock_Omitted,
                                   ZRFCollection *pInputCollection=nullptr);

        ZStatus
        zsearchFieldFirstCollection(ZDataBuffer &pRecordContent,
                                    zrank_type &pZRFRank,
                                    ZRFCollection &pCollection,
                                    const zlockmask_type pLock=ZLock_Omitted,
                                    ZRFCollection *pInputCollection=nullptr);
        ZStatus
        zsearchFieldNextCollection(ZDataBuffer &pRecordContent,
                                   zrank_type &pZRFRank,
                                   ZRFCollection &pCollection);

//--------------------End field operations--------------------------------------

    ZStatus zadd(ZDataBuffer &pUserBuffer);

    ZStatus zinsert(const char* pRecord,size_t pSize,zrank_type pRank);
    ZStatus zinsert(const ZDataBuffer &pRecord,zrank_type pRank);

    ZStatus zremove(long pRank);
    ZStatus zremoveR(long pRank,ZDataBuffer& pRecord);
    ZStatus zremoveByAddress (zaddress_type pAddress);


    // ----------File space management------------------------------
     inline
    ZStatus _moveBlock (ZFileDescriptor &pDescriptor,
                        const zaddress_type pFrom,
                        const zaddress_type pTo,
                        bool pCreateZFBTEntry=true);

    ZStatus zclearFile(const zsize_type pSize=-1) ;


    ZStatus zcloneFile (ZFileDescriptor &pDescriptor,const zsize_type pFreeSpace=-1,FILE*pOutput=stdout) ; // will clone the whole ZRandomFile and leave pFreeSpace free byte allocation

    ZStatus zcloneFile (const zsize_type pFreeSpace=-1,FILE*pOutput=stdout) ; // will clone the whole ZRandomFile and leave pFreeSpace free byte allocation


    ZStatus ztruncateFile (ZFileDescriptor &pDescriptor,const zsize_type pFreeSpace=-1,FILE*pOutput=stdout) ; // will truncate the whole ZRandomFile to pSize byte allocation

    ZStatus zextendFile (ZFileDescriptor &pDescriptor, const zsize_type pFreeSpace) ;

    ZStatus zreorgFile (FILE *pOutput=stdout) ; // reorganize the current file

    ZStatus zreorgUriFile(uriString &pURI,bool pDump=false,FILE *pOutput=stdout); // will reorganize ZRandomFile mentionned in pURI


    ZStatus zheaderRebuild(uriString pContentURI, bool pDump=false, FILE*pOutput=stdout);

    ZStatus zheaderRebuild(const char * pContentURI, bool pDump=false, FILE*pOutput=stdout)
    {
        uriString wUri (pContentURI);
        return zheaderRebuild(wUri,pDump,pOutput);
    }

protected:
     inline
    ZStatus _reorgFile (ZFileDescriptor &pDescriptor, bool pDump=false, FILE *pOutput=stdout) ;

    ZStatus _reorgFileInternals(ZFileDescriptor& pDescriptor,bool pDump,FILE*pOutput);
public:
    //-----------------get routines-----------------------------------
    /**
     * @brief getURIContent Returns by value the uriString for the content file with its current full path
     * @return
     */

    uriString &getURIContent(void) {return (ZDescriptor.URIContent);}
    /**
     * @brief getURIHeader returns by value the uriString for the header file with its current full path
     * @return
     */
    uriString getURIHeader(void) {return (ZDescriptor.URIHeader);}
    /**
     * @brief getFileDescriptor returns a reference to the current ZFileDescriptor
     * @return
     */

    ZFileDescriptor & getFileDescriptor(void) {return ZDescriptor;}

    /**
     * @brief zgetUsedSize returns the used space of the file
     *      Only space allocated to ZBlockAccessTable is returned here
     */

    zsize_type zgetUsedSize(void)       { if (ZDescriptor.ZFCB == nullptr ) return -1; return (ZDescriptor.ZFCB->UsedSize);}
    /**
     * @brief zgetAllocatedSize returns the total physical space taken by the ZRandomFile content file.
     * @return
     */

    zsize_type zgetAllocatedSize(void)  { if (ZDescriptor.ZFCB == nullptr ) return -1; return (ZDescriptor.getAllocatedSize());}
    /**
     * @brief zgetCurrentRank returns the current rank in ZBlockAccessTable
     *  Each read, write or delete affects this value.
     *
     * @return the current rank in ZBlockAccessTable
     */

    zrank_type          zgetCurrentRank(void)  {return ZDescriptor.getCurrentRank(); }
    /**
     * @brief zgetCurrentLogicalPosition returns the current logical address within the file
     *  Each read, write or delete affects this value.
     * @return
     */

    zaddress_type zgetCurrentLogicalPosition (void) {return (ZDescriptor.getCurrentLogicalAddress()) ; }
    /**
     * @brief setLogicalFromPhysical converts a physical to a logical address
     * @param pPhysical physical address to convert
     * @return  the logical address
     */

    zaddress_type setLogicalFromPhysical (zaddress_type pPhysical) {return (ZDescriptor.setLogicalFromPhysical(pPhysical)) ; }
    /**
     * @brief setPhysicalFromLogical converts a physical to a logical address
     * @param pLogical logical address to convert
     * @return the physical address duly converted
     */

    zaddress_type setPhysicalFromLogical (zaddress_type pLogical) {return (ZDescriptor.setLogicalFromPhysical(pLogical)) ;}
    /**
     * @brief getAddressFromRank  returns the physical address within file of the relative position of record given by pRank
     * @param pRank  Record's relative position within ZBlockAccessTable (ZBAT)
     * @return      the physical address of the record (beginning of block)
     */

    zaddress_type getAddressFromRank(const zrank_type pRank) {return (ZDescriptor.ZBAT->Tab[pRank].Address) ;}
    /**
     * @brief getRankFromAddress  returns the relative position of record (rank) coresponding to the given physical address within ZRandomFile file
     * @param pAddress    the physical address of the record (beginning of block)
     * @return      Record's relative position within ZBlockAccessTable (ZBAT) if found or -1 if address does not correspond to a valid block address.
     */
    static
    long getRankFromAddress(ZFileDescriptor *pDescriptor,const zaddress_type pAddress)
                {
                for (long wi=0;wi<pDescriptor->ZBAT->size();wi++)
                            if (pDescriptor->ZBAT->Tab[wi].Address==pAddress)
                                            return wi;
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVADDRESS,
                                        Severity_Error,
                                        " Address <%lld> does not correspond to a valid block address ",
                                        pAddress);
                return (-1) ;}
/**
 * @brief zgetCurrentPosition return the current logical address to which is set the current ZRandomFile address.
            This value is updated by any read or write operation and points to the actual block having last been accessed.
            ZRandomFile::zremove operation sets logical position to -1, so that return value of zgetCurrentPosition is not usable in that case.
 * @return
 */
    zaddress_type zgetCurrentPosition(void)  {return ZDescriptor.LogicalPosition; }



//
//---------------ZArray emulation-----------------------
//

    //! @brief lastIdx gets the rank of last used record in pool
    zrank_type lastIdx(void)           {return (ZDescriptor.ZBAT->lastIdx()); }
    //!@brief UsedSize  gets the total used space in file (does not take into account free blocks space)
    zsize_type UsedSize(void)    { if (ZDescriptor.ZFCB == nullptr ) return -1; return (ZDescriptor.ZFCB->UsedSize);}
    //! @brief size  gets the number of used records in used blocks pool
    long getSize(void)              {return (ZDescriptor.ZBAT->size()); }
    long getRecordCount(void)       {return (ZDescriptor.ZBAT->size()); }
    //!@brief isEmpty Returns true if ZRandomFile has no record in used blocks pool
    bool isEmpty(void)           {return (ZDescriptor.ZBAT->size()==0); }
    //! @brief freepoolSize gets the number of free blocks in pool
    ssize_t freepoolSize(void)      {return (ZDescriptor.ZFBT->size()); }



    /**
     * @brief last gets the last record of the file
     * @warning not usable in a multi threading context
     * @return a ZDataBuffer with the last user's record content (last rank)
     */
    ZDataBuffer& last(void)
                {
                zrank_type wLastIdx;
                zgetLast(CurrentRecord,wLastIdx);
                return (CurrentRecord);
                }

    ZStatus push(ZDataBuffer&pRecord);
    ZStatus push_front(ZDataBuffer&pRecord);
    ZStatus popRP(ZDataBuffer &pUserBuffer);
 //   long move (size_t pDest, size_t pOrig,size_t pNumber=1)   ; // move is not allowed
    zrank_type swap (const size_t pDest, const size_t pOrig,  const size_t pNumber=1)  ;

//--------------------operators-----------------------------------------

    /**
     * @brief operator [] delivers the record with logical rank pRank
     * @warning not usable in a multi threading context
     * @param pRank logical record position within the file
     * @return a ZDataBuffer with user's record content corresponding to given rank
     */
    const ZDataBuffer& operator [] (const long pRank) {ZStatus wSt; wSt=zget(CurrentRecord,pRank);
                                                       if (wSt!=ZS_SUCCESS)
                                                                    ZException.exit_abort();
                                                       return CurrentRecord;}
    /**
     * @brief operator << equivalent to push: adds a record as last logical record of the file
     * @param pRecord ZDataBuffer containing users record to add
     * @return a reference to the ZRandomFile
     */
    ZRandomFile & operator << (ZDataBuffer &pRecord) {ZStatus wSt;
                                                      wSt=zadd(pRecord);
                                                      if (wSt!=ZS_SUCCESS)
                                                                      ZException.exit_abort();
                                                      return *this;}


//----------------------------XML Routines-------------------------------------


    void zwriteXML_FileHeader(const char* pFilePath, FILE *pOutput);

    void zwriteXML_FileHeader(FILE *pOutput);

    void _writeXML_ZRandomFileHeader(ZFileDescriptor &pDescriptor, FILE *pOutput);



//--------------------- statistical  functions----------------------------------


    void    ZRFPMSReport( FILE *pOutput=stdout);

    ZStatus ZRFstat(const uriString& pFilename,FILE* pOutput=stdout);
    ZStatus ZRFstat (const char *pFilePath, FILE* pOutput=stdout)
        {
        uriString wFilePath;
        wFilePath=(const utf8_t*)pFilePath;
        return ZRFstat(wFilePath,pOutput);
        }


    ZStatus zgetBlockDescriptor (const zrank_type pRank, ZBlockDescriptor &pBlockDescriptor);
    ZStatus zgetFreeBlockDescriptor (const zrank_type pRank, ZBlockDescriptor &pBlockDescriptor);

    ZStatus zrecover (const zrank_type pRank, long &pZBATIdx);

// -----------------------Dump current ZRF- non static methods------------------
//
    void zfullDump(const int pColumn=16,FILE* pOutput=stdout);
    void zheaderDump(FILE* pOutput=stdout);
    void zcontentDump(const int pColumn=16,FILE* pOutput=stdout);

    void zsurfaceScan(uriString pURIContent, FILE *pOutput=stdout);
    void zsurfaceScan(const char* pFilename, FILE *pOutput=stdout)
    {
        uriString wFilename;
        wFilename = (const utf8_t*)pFilename;
        zsurfaceScan(wFilename,pOutput);
    }

protected:
    void _surfaceScan(ZFileDescriptor &pDescriptor, FILE *pOutput=stdout);

public:
//--------------Dump any ZRF given by its uriString pathname - static methods -------------------------
//

    void zblockDump (uriString pURIContent, const long pRank, const int pColumn=16, FILE* pOutput=stdout);
    void zblockDump (const char *pFilePath,const long pBlockNum,const int pColumn=16, FILE* pOutput=stdout)
        {
        uriString wFilePath;
        wFilePath=(const utf8_t*)pFilePath;
        zblockDump(wFilePath,pBlockNum,pColumn,pOutput);
        return;
        }

    void zfullDump(uriString pURIContent, const int pColumn=16, FILE* pOutput=stdout);
    void zfullDump (const char *pFilePath, const int pColumn=16, FILE* pOutput=stdout)
        {
        uriString wFilePath;
        wFilePath=(const utf8_t*)pFilePath;
        zfullDump(wFilePath,pColumn,pOutput);
        return;
        }


    void zheaderDump(uriString &pURIContent, FILE* pOutput=stdout);
    void zheaderDump (const char *pFilePath, FILE* pOutput=stdout)
        {
        uriString wFilePath;
        wFilePath=(const utf8_t*)pFilePath;
        zheaderDump(wFilePath,pOutput);
        return;
        }
    void zcontentDump(uriString pURIContent, int pColumn=16, FILE* pOutput=stdout);
    void zcontentDump (const char *pFilePath, int pColumn=16, FILE* pOutput=stdout)
        {
        uriString wFilePath;
        wFilePath=(const utf8_t*)pFilePath;
        zcontentDump(wFilePath,pColumn,pOutput);
        return;
        }
protected:

    void _headerDump(ZFileDescriptor &pDescriptor, FILE* pOutput=stdout);
    void _fullcontentDump(ZFileDescriptor &pDescriptor, const int pColumn=16, FILE* pOutput=stdout);
    void _dumpOneDataBlock(ZBlock &pBlock, ZDataBuffer &pRulerHexa, ZDataBuffer &pRulerAscii, const int pColumn=16, FILE *pOutput=stdout);

//-----------End Dump---------------------------------------------
//
protected:

    //  routines prefixed with '_' are low level routines
    //
    //

    void setReservedContent (const ZDataBuffer &pReserved)
    {
        ZDescriptor.ZReserved = pReserved ;
        return;
    }

    const ZDataBuffer& getReservedContent (void)
    {
        return ZDescriptor.ZReserved  ;
    }
    ZStatus getReservedBlock (bool pForceRead) ;
    ZStatus getReservedBlock (ZDataBuffer& pReserved, bool pForceRead);


    ZStatus updateFileDescriptor(bool pForceWrite);

    ZStatus updateReservedBlock(bool pForceWrite);
    ZStatus updateReservedBlock(const ZDataBuffer &pReserved, bool pForceWrite);


//static
    ZStatus _open(ZFileDescriptor &pDescriptor,
                  const zmode_type pMode,
                  const ZFile_type pFileType,
                  bool pLockRegardless=false);
static
    ZStatus _close(ZFileDescriptor &pDescriptor);
static
    ZStatus _getByRank(ZFileDescriptor & pDescriptor,
                 ZBlock &pBlock,
                 const long pRank,
                 zaddress_type &pAddress);

static
    ZStatus _getByAddress (ZFileDescriptor & pDescriptor,
                           ZBlock &pBlock,
                           const zaddress_type pAddress);
static
    ZStatus _add(ZFileDescriptor &pDescriptor, ZDataBuffer &pUserBuffer, zaddress_type &pAddress);

static
    ZStatus _getNext(ZFileDescriptor &pDescriptor, // updated
                     ZBlock &pBlock,               // write
                     zrank_type &pRank,
                     zaddress_type &pAddress);                 // read


//-----------Internal routines-------------------------------
static
    ZStatus _add2PhasesCommit_Prepare  (ZFileDescriptor &pDescriptor,
                                        const ZDataBuffer &pUserBuffer,
                                        zrank_type &pIdxCommit,
                                        zaddress_type &pLogicalAddress);
static
    ZStatus _add2PhasesCommit_Commit(ZFileDescriptor &pDescriptor,
                                      const ZDataBuffer &pUserBuffer,
                                      const zrank_type pIdxCommit,
                                      zaddress_type &pLogicalAddress);
static
    ZStatus _add2PhasesCommit_Rollback(ZFileDescriptor &pDescriptor,
                                       const zrank_type pIdxCommit);

static
    ZStatus _insert(ZFileDescriptor &pDescriptor,
                    const ZDataBuffer &pUserBuffer,
                    const zrank_type pRank,
                    zaddress_type &pLogicalAddress);

static
    ZStatus _insert2PhasesCommit_Prepare(ZFileDescriptor &pDescriptor,
                                        const ZDataBuffer &pUserBuffer,
                                        const zrank_type pRank,
                                        zrank_type &pIdxCommit,
                                        zaddress_type &pLogicalAddress);
static
    ZStatus _insert2PhasesCommit_Commit(ZFileDescriptor &pDescriptor,
                                       const ZDataBuffer &pUserBuffer,
                                       const zrank_type pIdxCommit);

static
    ZStatus _insert2PhasesCommit_Rollback(ZFileDescriptor &pDescriptor,
                                         const zrank_type pIdxCommit);


/*     inline
    ZStatus _removeByAddress_Commit(ZFileDescriptor &pDescriptor,ZDataBuffer &pUserBuffer,long &pIdxCommit,zaddress_type &pAddress);
     inline
    void    _removeByAddress_Rollback(ZFileDescriptor &pDescriptor,ZDataBuffer &pUserBuffer,long &pIdxCommit,zaddress_type &pAddress);
*/
static
    ZStatus _remove(ZFileDescriptor &pDescriptor, const zrank_type pRank);

static
    ZStatus _remove_Prepare(ZFileDescriptor &pDescriptor, const zrank_type pRank, zaddress_type &pLogicalAddress);
static
    ZStatus _removeR(ZFileDescriptor &pDescriptor,ZDataBuffer &pUserBuffer,const zrank_type pRank);

static
    ZStatus _removeR_Prepare(ZFileDescriptor &pDescriptor,ZDataBuffer &pUserBuffer,const zrank_type pRank,zaddress_type &pAddress);

static
    ZStatus _removeRByAddress(ZFileDescriptor &pDescriptor,ZDataBuffer &pUserBuffer,zrank_type &pIdxCommit,const zaddress_type pAddress);

static
    ZStatus _removeRByAddress_Prepare(ZFileDescriptor &pDescriptor,ZDataBuffer &pUserBuffer,zrank_type &pIdxCommit,const zaddress_type pAddress);

static
    ZStatus _removeByAddress(ZFileDescriptor &pDescriptor, const zaddress_type &pAddress);
static
    ZStatus _removeByAddress_Prepare(ZFileDescriptor &pDescriptor, zrank_type &pIdxCommit, const zaddress_type pAddress);


static
    ZStatus _remove_Commit(ZFileDescriptor &pDescriptor, const zrank_type pIdxCommit);
static
    ZStatus _remove_Rollback(ZFileDescriptor &pDescriptor, const zrank_type pIdxCommit);

static
    ZStatus _create (ZFileDescriptor &pDescriptor,
                     const zsize_type pInitialSize,
                     ZFile_type pFileType,
                     bool pBackup,
                     bool pLeaveOpen) ;
 //   ZStatus _extend (zoffset_type pSize) ;

static
    long _getFreeBlock(ZFileDescriptor &pDescriptor,
                       const size_t pSize,
                       ZBlockMin_struct &pBlock,
                       zrank_type pZBATRank=-1,
                       const zaddress_type pBaseAddress=-1);// obtains a free block of pSize  (within ZFBT) and moves it to ZBAT
static
    ZStatus _getExtent(ZFileDescriptor &pDescriptor,
                       ZBlockDescriptor &pBlockMin,
                       const size_t pSize);     //! get a free block extension with size greater or equal to pSize (according ExtentQuotaSize)
static
    long _allocateFreeBlock (ZFileDescriptor &pDescriptor,
                             zrank_type pZFBTRank,
                             zsize_type pSize,
                             long pZBATRank=-1);// allocates a free block to used block (from ZFBT to ZBAT) at rank pZBABRank, or by push (pZBABRank=-1)

     static
    void    _freeBlock_Prepare (ZFileDescriptor &pDescriptor,
                                zrank_type pRank); // remove Block pointed by pRank in ZBAT and move it to ZFBT. Update File Header
     static
    void    _freeBlock_Rollback(ZFileDescriptor &pDescriptor,
                                zrank_type pRank); // remove Block pointed by pRank in ZBAT and move it to ZFBT. Update File Header
     static
    ZStatus _freeBlock_Commit  (ZFileDescriptor &pDescriptor,
                                zrank_type pRank); // remove Block pointed by pRank in ZBAT and move it to ZFBT. Update File Header
    static
    ZStatus _freeBlock(ZFileDescriptor &pDescriptor,
                       zrank_type pRank); // remove Block pointed by pRank in ZBAT and move it to ZFBT. Update File Header

     static
    ZStatus _grabFreeSpacePhysical(ZFileDescriptor &pDescriptor,
                                   zrank_type pRank,
                                   ZBlockDescriptor &pBS);   // reference to aggregated block to be freed : output
     static
    ZStatus _grabFreeSpaceLogical(ZFileDescriptor &pDescriptor,
                                  zrank_type pRank,
                                  ZBlockDescriptor &pBS);   // reference to aggregated block to be freed : output

     static
    ZStatus _searchPreviousPhysicalBlock (ZFileDescriptor &pDescriptor,
                                  zaddress_type pCurrentAddress,
                                  zaddress_type &pPreviousAddress,   // Previous physical block address found in file or Start of Data : output
                                  ZBlockHeader &pBlockHeader);      // block header : output - updated if any else left

     static
    ZStatus _searchPreviousBlock (ZFileDescriptor &pDescriptor,
                                   zrank_type pRank,
                                   zaddress_type &pPreviousAddress,
                                   ZBlockHeader &pBlockHeader);

     static
    ZStatus _searchNextBlock (ZFileDescriptor &pDescriptor,
                              zrank_type pRank,
                              zaddress_type &pNextAddress,   //! Next physical block address found in file or End of file : output
                              ZBlockHeader &pBlockHeader);  //! block header : output - updated if any else left


     static
    ZStatus _searchNextPhysicalBlock (ZFileDescriptor &pDescriptor,
                                      zaddress_type pAddress,                     //! Address to start searching for for next block
                                      zaddress_type &pNextAddress,
                                      ZBlockHeader &pBlockHeader) ;

     static
    ZStatus _getBlockHeader(ZFileDescriptor &pDescriptor,
                            zaddress_type pAddress,
                            ZBlockHeader &pBlockHeader);

     static ZStatus
     _getBlockHeader_Export(ZFileDescriptor &pDescriptor,
                            zaddress_type pAddress,
                            ZBlockHeader_Export &pBlockHeader_Export);

     static
    ZStatus _writeBlockAt(ZFileDescriptor &pDescriptor,
                        ZBlock &pBlock,
                        const zaddress_type pAddress);

    static ZStatus _writeBlockHeader(ZFileDescriptor &pDescriptor,
                              ZBlockHeader &pBlockHeader,
                              const zaddress_type pAddress);
     static
    ZStatus _markBlockAsDeleted (ZFileDescriptor &pDescriptor, zrank_type pRank);

     static
    ZStatus _markFreeBlockAsDeleted (ZFileDescriptor &pDescriptor, zrank_type pRank);
     static
    ZStatus _markDeletedBlockAsFree (ZFileDescriptor &pDescriptor, zrank_type pRank);
     static
    ZStatus _markFreeBlockAsUsed (ZFileDescriptor &pDescriptor, zrank_type pRank);

     static
    ZStatus _recoverFreeBlock (ZFileDescriptor &pDescriptor, zrank_type pRank);

     static
    ZStatus _highwaterMark_Block (ZFileDescriptor &pDescriptor,const zsize_type pFreeUserSize);

// ------------file header operations-----------------------


     static
    ZStatus _getFullFileHeader(ZFileDescriptor &pDescriptor, bool pForceRead=false);

     static
    ZStatus _getFileHeader(ZFileDescriptor &pDescriptor, bool pForceRead);

    static
    ZStatus _getFileHeader_Export(ZFileDescriptor &pDescriptor,ZHeaderControlBlock_Export* pHCB_Export);

     static
    ZStatus _getReservedHeader(ZFileDescriptor &pDescriptor, bool pForceRead);

    static ZStatus _getFileControlBlock(ZFileDescriptor &pDescriptor, bool pForceRead);



    static ZStatus _writeFullFileHeader(ZFileDescriptor &pDescriptor, bool pForceWrite);// should be a duplicate of _writeReservedHeader but must be kept for logic


    static ZStatus _writeFileHeader(ZFileDescriptor &pDescriptor, bool pForceWrite);

    static ZStatus _writeReservedHeader(ZFileDescriptor &pDescriptor,bool pForceWrite); // corresponds to a full header write because reserved header is positionned before file descriptor

    static ZStatus _writeFileDescriptor(ZFileDescriptor &pDescriptor, bool pForceWrite);




    static ZStatus _seek(ZFileDescriptor &pDescriptor, zaddress_type pAddress);


    static ZStatus _read(ZFileDescriptor &pDescriptor,
                  void* pBuffer,
                  const size_t pSize,
                  ssize_t& pSizeToRead,
                  ZPMSCounter_type pZPMSType);
    static
    ZStatus _read(ZFileDescriptor &pDescriptor,
                  ZDataBuffer& pBuffer,
                  const ssize_t pSizeToRead,
                  ZPMSCounter_type pZPMSType);

    static
    ZStatus _readAt(ZFileDescriptor &pDescriptor,
                    void* pBuffer,
                    size_t pSize,
                    ssize_t& pSizeRead,
                    zaddress_type pAddress,
                    ZPMSCounter_type pZPMSType);
     static
    ZStatus _readAt(ZFileDescriptor &pDescriptor,
                    ZDataBuffer& pBuffer,
                    ssize_t &pSizeRead,
                    zaddress_type pAddress,
                    ZPMSCounter_type pZPMSType);
     static
    ZStatus _readBlockAt(ZFileDescriptor &pDescriptor,
                         ZBlock &pBlock,
                         const zaddress_type pAddress);




}; // ZRandomFile


/** @}*/ //addtogroup ZRandomFileGroup

} // namespace zbs

const char *
decode_ZBID (ZBlockID pZBID);

ZStatus generateURIHeader(uriString pURIPath, uriString &pURIHeader);


#endif // ZRANDOMFILE_H
