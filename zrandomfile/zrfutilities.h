#ifndef ZRFUTILITIES_H
#define ZRFUTILITIES_H

#include <zcontent/zrandomfile/zrandomfile.h>
#include <config/zconfig.h>


extern const long cst_FileNudge;

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


ZStatus rebuildZRFHeader(uriString& pURIContent);

#endif // ZRFUTILITIES_H
