#ifndef ZRFUTILITIES_H
#define ZRFUTILITIES_H

#include <config/zconfig.h>
#include "zrandomfile.h"
#include <zio/zioutils.h>

extern const long cst_FilePayload;


enum ZBlockExistence : uint16_t {
  ZBEX_Correct          =      0,
  ZBEX_SysBadAddress    =      1,  /* cannot position or read at given address on file */
  ZBEX_Orphan           =      2,  /* cannot find <cst_ZFILEBLOCKSTART> at given address */
  ZBEX_WrngAddr         =      4,
  ZBEX_Start            =      8,
  ZBEX_PoolZeroSize     =   0x10,  /* block size in pool has zero value */
  ZBEX_ContentZeroSize  =   0x20,  /* block size on content file has zero value */
  ZBEX_MustBeUsed       =   0x40,  /* invalid block state : must be ZBS_Used */
  ZBEX_MustBeFreeOrDeleted       = 0x0100,  /* invalid block state : must be ZBS_Free Or ZBS_Deleted*/
  ZBEX_SizeNotSame      = 0x0200, /* block size in pool is different from block size on content file block header */
  ZBEX_StateNotSame     = 0x0400, /* State in pool is different from content file block header */
  ZBEX_LockNotSame      = 0x0800, /* Lock mask in pool is different from content file block header */
  ZBEX_PidNotSame       = 0x1000  /* pid in pool is different from content file block header */
  //  ZBEX_MustBeDeleted    = 0x0200,  /* invalid block state : must be ZBS_Deleted */
};

enum ZPoolType : uint8_t {
  ZPTP_ZBAT     = 0 ,
  ZPTP_ZFBT     = 1 ,
  ZPTP_ZHOT     = 2 ,
  //  ZPTP_ZDBT     = 2 ,  // Deprecated
  ZPTP_Unknown
};


class ZRandomFileUtils {
public:
  ZRandomFileUtils() = default;
  ZRandomFileUtils(uriString & pURI) {setUri(pURI);}

  ~ZRandomFileUtils() ;

  void setUri(uriString& pURI) {URI=pURI;}


  ZStatus rebuildHeader(uriString * pURIHeader);

  ZStatus seekAndGet(ZDataBuffer& pOut, ssize_t &pSize, size_t pAddress);

  /** @brief searchPreviousStartSign Searches file surface for start sign beginning at pStartAddress down to beginning of file
   *                                and return its file address in pOutAddress with a status set to ZS_SUCCESS.
   *  if pStartAddress points to a valid start sign then pStartAddress is returned
   *  if no start sign is found before beginning of file ZS_OUTBOUNDLOW is returned
   *
   * @return
   * ZS_SUCCESS : start block has been found. pOutAddress contains start sign address on file
   * ZS_OUTBOUND : pStartAddress is out of file's boundaries
   * ZS_OUTBOUNDLOW : no start sign has been found while beginning of file surface has been reached
   *  Any other status that may be emitted by low level routines.
   */
  ZStatus searchPreviousStartSign(zaddress_type pStartAddress, zaddress_type &pOutAddress) ;
  ZStatus searchNextStartSign(zaddress_type pStartAddress, zaddress_type & pOutAddress);
  ZStatus searchNextBlock(ZBlockDescriptor_Export& pBlock, zaddress_type pStartAddress) ;
  ZStatus searchPreviousBlock(ZBlockDescriptor_Export& pBlock, zaddress_type pStartAddress);

  ZStatus _openContent(uriString& pURIContent) ;
  ZStatus _openContent() {return _openContent(URI);}
  ZStatus _openHeader(uriString* pURIHeader=nullptr) ;

  ZStatus writeHeaderFromPool(const uriString& pURIHeader,
              ZHeaderControlBlock &pHCB, ZFileControlBlock &pFCB, ZDataBuffer &pReserved,
              ZBlockPool* pZBAT, ZBlockPool* pZFBT, ZBlockPool* pZHOT);


  void getPrevAddrVal(zaddress_type &pAddress, long &pPayload, long &pBucket);

  void _print(const char* pFormat,...);
  void _print(const utf8VaryingString& pOut) ;

  void set_Output(FILE* pOutput) {Output=pOutput;}
  void set_displayCallBack(__DISPLAYCALLBACK__(pDCB)) {_displayCallback=pDCB;}

  __DISPLAYCALLBACK__(_displayCallback)=nullptr;
  __progressCallBack__(_progressCallBack)=nullptr;
  __progressSetupCallBack__(_progressSetupCallBack)=nullptr;

  __FILEHANDLE__ Fd=-1;
  __FILEHANDLE__ FdHeader=-1;
  off_t FileOffset=0;
  size_t FileSize=0;
  uriString URI;
  uriString URIHeader;
  long FileNudge=cst_FilePayload;
  bool _isOpen=false;
  ZBlockDescriptor_Export BlockCur;

  ZFileControlBlock   FCB;
  ZHeaderControlBlock HCB;

  FILE* Output;
}; // ZRandomFileUtils

/**
 * @brief rawSearchNextStartSign searches cst_ZFILEBLOCKSTART mark
 * within a file described by its descriptor pFd (must be open) of total size pFileSize,
 * starting at address pStartAddress.
 * At first found, returns pOutAddress, address of first byte of the found sequence.
 * Each access to file is done using pPayload bytes.
 * @param pFd       a valid open file descriptor
 * @param pFileSize file size
 * @param pPayload    quantum of byte to load for each file access
 * @param pStartAddress file offset to start search : WARNING if it points to startsign then pStartAddress will be returned.
 * @param pOutAddress   returned address (file offset) when found. set to -1 if not found
 * @return a ZStatus
 * ZS_SUCCESS if sequence is successfully found
 * ZS_EOF or ZS_OUTBOUNDHIGH if end of file is reached
 * ZS_READPARTIAL is depending on payload but IS NOT RETURNED. Search is done normally and if found ZS_SUCCESS is returned.
 * All other possible status from rawReadAt and rawSeekToPosition
 */
ZStatus
rawSearchNextStartSign(__FILEHANDLE__ pFd,size_t pFileSize,long pPayload,
                    zaddress_type pStartAddress, zaddress_type & pOutAddress);

/**
 * @brief rawSearchNextSequence searches a byte sequence pSequence of length pSeqLen
 * within a file described by its descriptor pFd (must be open) of total size pFileSize,
 * starting at address pStartAddress.
 * At first found, returns pOutAddress, address of first byte of the found sequence.
 * Each access to file is done using pPayload bytes.
 * @param pFd       a valid open file descriptor
 * @param pFileSize file size
 * @param pPayload    quantum of byte to load for each file access
 * @param pSequence sequence to search for
 * @param pSeqLen   sequence length
 * @param pStartAddress file offset to start search
 * @param pOutAddress   returned address (file offset) when found. set to -1 if not found
 * @return a ZStatus
 * ZS_SUCCESS if sequence is successfully found
 * ZS_EOF or ZS_OUTBOUNDHIGH if end of file is reached
 * All possible status from rawReadAt and rawSeekToPosition
 */
ZStatus
rawSearchNextSequence(__FILEHANDLE__ pFd,size_t pFileSize,long pPayload,
                      const unsigned char* pSequence,size_t pSeqLen,
                      zaddress_type pStartAddress, zaddress_type & pOutAddress);


ZStatus
rawGetBlockDescriptor(__FILEHANDLE__ pFdContent,ZBlockDescriptor& pBDOut,zaddress_type pAddress);

uint16_t
rawCheckContentBlock(int pPoolId,__FILEHANDLE__ pFdContent,ZBlockDescriptor& pBlockDesc);

ZStatus rebuildZRFHeader(uriString& pURIContent);




/**
 * @brief _searchBlockStart scans file pointed by pContentFd since pBeginAddress for a start mark (cst_ZBLOCKSTART 0xF5F5F5F5)
 * and returns
 *  . address of found mark in pNextAddress (pointing to first byte of start mark),
 *  . ZDataBuffer containing the file space between two start mark or until end of file.
 * Each read access loads pPayload bytes from file. pPayload is adjusted progressively to block sizes.
 * When found pNextAddress points to the first byte of start mark.
 * @param pContentFd    File descriptor to search. File must be opened with READ capabilities (O_RDONLY)
 * @param pBeginAddress address to begin the search.
 * WARNING: if pBeginAddress points to a cst_ZBLOCKSTART block, then this address will be returned as pNextAddress.
 * @param pNextAddress  address of first byte of found cst_START
 * @param pPayload      number of bytes to read at each read operation
 * @param pFileSize     total number of bytes for the file to be scanned
 * @return  a ZStatus
 * - ZS_FOUND     start mark has been found.
 *                pNextAddress points to first byte of start mark.
 *                pBlockContent contains block data since pBeginAddress until next start mark (excluded).
 * - ZS_EOF       no more to read and start mark has not been found since pBeginAddress.
 *                pNextAddress is set to the last address processed.
 *                pBlockContent contains data since pBeginAddress until EndofFile.
 * - ZS_READERROR a low level error has been encountered. ZException is set with appropriate message.
 *                pNextAddress is set to the last address processed.
 * - ZS_FILEERROR a seek operation failed with a low level error.
 *                ZException is set with appropriate message.
 *                pNextAddress is set to the last address processed.
 */
ZStatus
_searchBlockStart (__FILEHANDLE__ pContentFd,
                  zaddress_type pBeginAddress,      // Address to start searching for start mark
                  zaddress_type &pNextAddress,
                  ZDataBuffer &pBlockContent,
                  ssize_t &pPayload,
                  int     &pCount,
                  size_t  &pFileSize,
                  uint32_t *pBeginContent=nullptr);
//bool testSequence (const unsigned char* pSequence,size_t pSeqLen, const unsigned char* pToCompare);



utf8VaryingString decode_ZBEx(uint16_t pBEx);

#endif // ZRFUTILITIES_H
