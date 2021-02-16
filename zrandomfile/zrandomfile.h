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
#include <ztoolset/zsystemuser.h>

#include <zrandomfile/zblock.h>

#include <zrandomfile/zheadercontrolblock.h>
#include <zrandomfile/zfilecontrolblock.h>

#include <zrandomfile/zfiledescriptor.h>

#ifndef ZRANDOMFILE_CPP

    extern bool ZRFStatistics;
#endif
    void setZRFStatistics (bool pStatistics) ;
/** @} */


typedef long zrank_type;


namespace zbs {



#ifdef __COMMENT__
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
#endif // __COMMENT__
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


struct lockPack_struct
{
    zlockmask_type          Lock    = ZLock_Nolock ;        /**< Lock mask (int32_t) at file header level (Exclusive lock) @see ZLockMask_type one lock at a time is authorized */
    pid_t                   LockOwner = 0L;                 /**< Owner process for the lock */
};


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
