#ifndef ZRFUTILITIES_H
#define ZRFUTILITIES_H

#include <zcontent/zrandomfile/zrandomfile.h>
#include <config/zconfig.h>
#include <zio/zioutils.h>

extern const long cst_FileNudge;


enum ZBlockExistence : uint16_t {
  ZBEX_Correct          =      0,
  ZBEX_SysBadAddress    =      1,  /* cannot position or read at given address on file */
  ZBEX_Orphan           =      2,  /* cannot find <cst_ZFILEBLOCKSTART> at given address */
  ZBEX_WrngAddr         =      4,
  ZBEX_Start            =      8,
  ZBEX_Size             =   0x10,  /* block size in pool is different from block size on content file block header */
  ZBEX_PoolZeroSize     =   0x20,  /* block size in pool has zero value */
  ZBEX_ContentZeroSize  =   0x40,  /* block size on content file has zero value */
  ZBEX_MustBeUsed       =   0x80,  /* invalid block state : must be ZBS_Used */
  ZBEX_MustBeFreeOrDeleted       = 0x0100,  /* invalid block state : must be ZBS_Free Or ZBS_Deleted*/
  //  ZBEX_MustBeDeleted    = 0x0200,  /* invalid block state : must be ZBS_Deleted */
};

enum ZPoolType : uint8_t {
  ZPTP_ZBAT     = 0 ,
  ZPTP_ZFBT     = 1 ,
  ZPTP_ZHOT     = 2 ,
  //  ZPTP_ZDBT     = 2 ,  // Deprecated
  ZPTP_Unknown
};


class ZFileUtils {
public:
  ZFileUtils() = default;
  ZFileUtils(uriString & pURI) {setUri(pURI);}

  ~ZFileUtils() ;

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


  void getPrevAddrVal(zaddress_type &pAddress, long &pNudge, long &pBucket);

  void _print(const char* pFormat,...);
  void _print(const utf8VaryingString& pOut) ;

  void set_Output(FILE* pOutput) {Output=pOutput;}
  void set_displayCallBack(__DISPLAYCALLBACK__(pDCB)) {_displayCallback=pDCB;}
  __DISPLAYCALLBACK__(_displayCallback)=nullptr;

  __FILEHANDLE__ Fd=-1;
  __FILEHANDLE__ FdHeader=-1;
  off_t FileOffset=0;
  size_t FileSize=0;
  uriString URI;
  uriString URIHeader;
  long FileNudge=cst_FileNudge;
  bool _isOpen=false;
  ZBlockDescriptor_Export BlockCur;

  ZFileControlBlock   FCB;
  ZHeaderControlBlock HCB;

  FILE* Output;
};

/**
 * @brief rawSearchNextStartSign searches cst_ZFILEBLOCKSTART mark
 * within a file described by its descriptor pFd (must be open) of total size pFileSize,
 * starting at address pStartAddress.
 * At first found, returns pOutAddress, address of first byte of the found sequence.
 * Each access to file is done using pNudge bytes.
 * @param pFd       a valid open file descriptor
 * @param pFileSize file size
 * @param pNudge    quantum of byte to load for each file access
 * @param pStartAddress file offset to start search
 * @param pOutAddress   returned address (file offset) when found. set to -1 if not found
 * @return a ZStatus
 * ZS_SUCCESS if sequence is successfully found
 * ZS_EOF or ZS_OUTBOUNDHIGH if end of file is reached
 * All possible status from rawReadAt and rawSeekToPosition
 */
ZStatus
rawSearchNextStartSign(__FILEHANDLE__ pFd,size_t pFileSize,long pNudge,
                    zaddress_type pStartAddress, zaddress_type & pOutAddress);

/**
 * @brief rawSearchNextSequence searches a byte sequence pSequence of length pSeqLen
 * within a file described by its descriptor pFd (must be open) of total size pFileSize,
 * starting at address pStartAddress.
 * At first found, returns pOutAddress, address of first byte of the found sequence.
 * Each access to file is done using pNudge bytes.
 * @param pFd       a valid open file descriptor
 * @param pFileSize file size
 * @param pNudge    quantum of byte to load for each file access
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
rawSearchNextSequence(__FILEHANDLE__ pFd,size_t pFileSize,long pNudge,
                      const unsigned char* pSequence,size_t pSeqLen,
                      zaddress_type pStartAddress, zaddress_type & pOutAddress);


ZStatus
rawGetBlockDescriptor(__FILEHANDLE__ pFdContent,ZBlockDescriptor& pBDOut,zaddress_type pAddress);

uint16_t
rawCheckContentBlock(int pPoolId,int pFdContent,ZBlockDescriptor& pBlockDesc);

ZStatus rebuildZRFHeader(uriString& pURIContent);


utf8VaryingString decode_ZBEx(uint16_t pBEx);

#endif // ZRFUTILITIES_H
