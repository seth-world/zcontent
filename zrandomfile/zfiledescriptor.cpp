#include "zfiledescriptor.h"
#include <zcontentcommon/zcontentconstants.h>

using namespace zbs;

//=========================== ZFileDescriptor Export import==========================================
/**
 * @brief ZFileDescriptor::_exportFCB Exports the operational data of file descriptor :
 *                      File control block (FCB)
 *                      pools : ZBAT   active blocks pool
 *                              ZFBT   free blocks pool
 *                              ZDBT   deleted blocks pool
 * @param pZDBExport
 * @return
 */
ZDataBuffer&
ZFileDescriptor::_exportFCB(ZDataBuffer& pZDBExport)
{
  printf ("ZFileDescriptor::_exportFCB>>\n");
  ZFileControlBlock wFileControlBlock;

  wFileControlBlock.ZBAT_DataOffset =  sizeof(ZFileControlBlock_Export);  // ZBAT data Pool is stored first just after ZFCB

  wFileControlBlock.AllocatedBlocks =ZBAT->getAllocation();
  wFileControlBlock.BlockExtentQuota = ZBAT->getQuota();

  ZDataBuffer wZBAT_export;
  ZBAT->_exportPool(wZBAT_export);
  wFileControlBlock.ZBAT_ExportSize = wZBAT_export.getByteSize();

  wFileControlBlock.ZFBT_DataOffset = (zaddress_type)(ZFCB->ZBAT_DataOffset
                                                      +ZFCB->ZBAT_ExportSize); // then ZFBT
  //    wFileControlBlock.ZFBT_ExportSize = ZFBT->_getExportAllocatedSize();
  ZDataBuffer wZFBT_export;
  ZFBT->_exportPool(wZFBT_export);
  wFileControlBlock.ZFBT_ExportSize = wZFBT_export.getByteSize();

  wFileControlBlock.ZDBT_DataOffset = (zaddress_type)(ZFCB->ZFBT_DataOffset
                                                      +ZFCB->ZFBT_ExportSize); // then ZDBT
  //    wFileControlBlock.ZDBT_ExportSize = ZDBT->_getExportAllocatedSize();
  ZDataBuffer wZDBT_export;
  ZFBT->_exportPool(wZDBT_export);
  wFileControlBlock.ZDBT_ExportSize = wZDBT_export.getByteSize();

  wFileControlBlock._export(pZDBExport);

  pZDBExport.appendData(wZBAT_export);
  pZDBExport.appendData(wZFBT_export);
  pZDBExport.appendData(wZDBT_export);
  return pZDBExport;
}// ZFileDescriptor::_exportFCB

ZFileDescriptor&
ZFileDescriptor::_importFCB(unsigned char* pPtrIn)
{
  printf ("ZFileDescriptor::_importFCB>>\n");
  ZFileControlBlock* wFileControlBlock=( ZFileControlBlock* )pPtrIn; // for debug
  setupFCB();
  unsigned char* wPtrIn=pPtrIn;
  ZFCB->_import(wPtrIn);

  ZBAT->_importPool(wPtrIn);

  ZFBT->_importPool(wPtrIn);

  ZDBT->_importPool(wPtrIn);

  return *this;
}// ZFileDescriptor::_importFCB

//=================ZFileDescriptor===============================================
void
ZFileDescriptor::setupFCB (void)
{
  if (ZFCB!=nullptr)
    delete ZFCB;
  ZFCB=new ZFileControlBlock;
  if (ZBAT!=nullptr)
    delete ZBAT;
  ZBAT=new ZBlockAccessTable;
  if (ZFBT!=nullptr)
    delete ZFBT;
  ZFBT=new ZFreeBlockPool;
  if (ZDBT!=nullptr)
    delete ZDBT;
  ZDBT=new ZDeletedBlockPool;
  return;
  //           ZReserved.clear();  // Reserved header is fully managed by derived classes
}


ZFileDescriptor& ZFileDescriptor::_copyFrom(const ZFileDescriptor& pIn)
{
  ZFDOwnData::_copyFrom((const ZFDOwnData&)pIn);
  URIContent = pIn.URIContent;
  URIHeader = pIn.URIHeader;
  URIDirectoryPath = pIn.URIDirectoryPath;
  return *this;
}

void
ZFileDescriptor::clearFCB (void)
{
  ZBlockLock.clear();
  ZReserved.clear();
  if (ZFCB!=nullptr)
    delete ZFCB;
  if (ZBAT!=nullptr)
    delete ZBAT;
  if (ZFBT!=nullptr)
    delete ZFBT;
  if (ZDBT!=nullptr)
    delete ZDBT;


  ZFCB = nullptr;
  ZBAT = nullptr;
  ZFBT = nullptr;
  ZDBT = nullptr;

  return;
}


void ZFileDescriptor::clearPartial (void)
{
  clearFCB();

//  memset (this,0,(sizeof(ZFDOwnData)));
  CurrentRank=-1;
  PhysicalPosition = -1;
  LogicalPosition = -1;
  _isOpen = false ;
  HeaderAccessed = false;

  Pid= getpid();  // get current pid for ZFileDescriptor
}


ZStatus
ZFileDescriptor::_close()
{
  if (close (ContentFd) < 0)
  {
    ZException.getErrno(errno,
        "ZFileDescriptor._close",
        ZS_FILEERROR,
        Severity_Severe,
        "System error closing file <%s>",URIContent.toCChar());
    return ZS_FILEERROR;
  }
  if (close (HeaderFd) < 0)
  {
    ZException.getErrno(errno,
        "ZFileDescriptor._close",
        ZS_FILEERROR,
        Severity_Severe,
        "System error closing file <%s>",URIHeader.toCChar());
    return ZS_FILEERROR;
  }
  _isOpen=false;
  HeaderAccessed = false;
  ZRFPool->removeFileByFd(ContentFd);
  clearPartial();
  return ZS_SUCCESS;
}

void
ZFileDescriptor::_forceClose()
{
  close (ContentFd);
  close (HeaderFd);
  _isOpen=false;
  HeaderAccessed = false;
  ZRFPool->removeFileByFd(ContentFd);
  clearPartial();
}

/**
 * @brief ZFileDescriptor::setPath for a ZRandomFile, set up the different pathes environment using a given uriString.
 *  This uriString will name the content file (main data file) @see ZRFPhysical
 * @param[in] pURIPath an uriString containing the path of the file
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZFileDescriptor::setPath (const uriString &pURIPath)
{
  if (_isOpen)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVOP,
        Severity_Error,
        "File <%s> is already open.Cannot change to path <%s>",
        URIContent.toCChar(),
        pURIPath.toCChar());
    return ZS_INVOP;
  }
  Pid = getpid();
  //    utfdescString wDInfo;
  const utf8_t* wExtension=(const utf8_t*)__HEADER_FILEEXTENSION__;
  if (wExtension[0]=='.')
    wExtension++;       // +1 because we should miss the '.' char
  //    if (!strcmp(wExtension,(char*)pURIPath.getFileExtension().toString()))
  if (pURIPath.getFileExtension()==wExtension)

  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVNAME,
        Severity_Error,
        "File name is malformed. Extension <%s> is reserved while given file name is <%s>",
        __HEADER_FILEEXTENSION__,
        pURIPath.toCChar());
    return ZS_INVNAME;
  }

  URIContent = pURIPath;

  URIDirectoryPath = URIContent.getDirectoryPath().toString();

  if (URIDirectoryPath.isEmpty())  // if directory path is not mentionned : then current working directory is taken
  {
    char wB[cst_filename_max];
    URIDirectoryPath = (const utf8_t*)getcwd(wB,cst_filename_max);
    URIContent.setDirectoryPath(URIDirectoryPath);
  }

  return generateURIHeader( pURIPath,URIHeader);
}// ZFileDescriptor::setPath

