#include "zfiledescriptor.h"
#include <zcontentcommon/zcontentconstants.h>
#include "zrandomfiletypes.h"
#include "zrandomfile.h"
#include <zxml/zxmlprimitives.h>

#include <zio/zioutils.h>

using namespace zbs;



//=========================== ZFileDescriptor Export import==========================================
/**
 * @brief ZFileDescriptor::_exportFCB Exports the operational data of file descriptor :
 *                      File control block (FCB)
 *                      pools : ZBAT   active blocks pool
 *                              ZFBT   free blocks pool
 *                              ZHOT   holes table
 *                              (ZDBT   deleted blocks pool --DEPRECATED--)
 * @param pZDBExport
 * @return
 */
ZDataBuffer&
ZFileDescriptor::_exportFCB(ZDataBuffer& pZDBExport)
{
  _DBGPRINT ("ZFileDescriptor::_exportFCB>>\n")
  ZFileControlBlock wFileControlBlock;

  wFileControlBlock.ZBAT_DataOffset =  sizeof(ZFCB_Export);  // ZBAT data Pool is stored first just after ZFCB

  wFileControlBlock.AllocatedBlocks =ZBAT.getAllocation();
  wFileControlBlock.BlockExtentQuota = ZBAT.getQuota();

  ZDataBuffer wZBAT_export;
  size_t wExpSize=ZBAT._exportAppendPool(wZBAT_export);
  wFileControlBlock.ZBAT_ExportSize = wExpSize ;

  wFileControlBlock.ZFBT_DataOffset = (zaddress_type)(ZFCB.ZBAT_DataOffset + ZFCB.ZBAT_ExportSize); // then ZFBT
  //    wFileControlBlock.ZFBT_ExportSize = ZFBT._getExportAllocatedSize();
  ZDataBuffer wZFBT_export;
  wExpSize = ZFBT._exportAppendPool(wZFBT_export);
  wFileControlBlock.ZFBT_ExportSize = wExpSize ;

  wFileControlBlock.ZHOT_DataOffset = (zaddress_type)(ZFCB.ZFBT_DataOffset + ZFCB.ZFBT_ExportSize); // then ZDBT
  ZDataBuffer wZHOT_export;
  wExpSize = ZHOT._exportAppendPool(wZHOT_export);
  wFileControlBlock.ZHOT_ExportSize = wExpSize ;

#ifdef __DEPRECATED__
  wFileControlBlock.ZDBT_DataOffset = (zaddress_type)(ZFCB.ZFBT_DataOffset + ZFCB.ZFBT_ExportSize); // then ZDBT
  //    wFileControlBlock.ZDBT_ExportSize = ZDBT._getExportAllocatedSize();
  ZDataBuffer wZDBT_export;
  ZFBT._exportAppendPool(wZDBT_export);
  wFileControlBlock.ZDBT_ExportSize = wExpSize ;
#endif // __DEPRECATED__

  wExpSize = wFileControlBlock._exportAppend(pZDBExport);

  pZDBExport.appendData(wZBAT_export);
  pZDBExport.appendData(wZFBT_export);
  pZDBExport.appendData(wZHOT_export);
//  pZDBExport.appendData(wZDBT_export);
  return pZDBExport;
}// ZFileDescriptor::_exportFCB

ZFileDescriptor&
ZFileDescriptor::_importFCB(const unsigned char* pPtrIn)
{
  printf ("ZFileDescriptor::_importFCB>>\n");

  const unsigned char* wPtrIn=pPtrIn;
  ZFCB._import(wPtrIn);/* wPtrIn is updated by ZFileControlBlock::_import */

  ZBAT._importPool(wPtrIn);  /* wPtrIn is updated by ZBlockPool::_importPool */

  ZFBT._importPool(wPtrIn);

  ZHOT._importPool(wPtrIn);

//  ZDBT._importPool(wPtrIn);

  return *this;
}// ZFileDescriptor::_importFCB

//=================ZFileDescriptor===============================================
/*void
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
*/


ZFDOwnData& ZFDOwnData::_copyFrom(const ZFDOwnData& pIn)
{
  FContent=pIn.FContent;
  ContentFd=pIn.ContentFd;
//  FHeader=pIn.FHeader;
  HeaderFd=pIn.HeaderFd;
  Pid=pIn.Pid;
  Uid=pIn.Uid;
  _isOpen=pIn._isOpen;
  Mode=pIn.Mode;

  ZHeader=pIn.ZHeader;
  ZFCB=pIn.ZFCB;
  ZBAT=(ZBlockPool&)pIn.ZBAT;
  ZFBT=(ZBlockPool&)pIn.ZFBT;
  ZHOT=(ZBlockPool&)pIn.ZHOT;
//  ZDBT=(ZBlockPool&)pIn.ZDBT;  // Deprecated
  ZReserved=pIn.ZReserved;
  ZBlockLock=pIn.ZBlockLock;
  PhysicalPosition=pIn.PhysicalPosition;
  LogicalPosition=pIn.LogicalPosition;
  CurrentRank=pIn.CurrentRank;

  return *this;
}//_copyFrom

utf8String ZFDOwnData::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("zfdowndata",pLevel);
  wLevel++;
  /* see if it is really required */
  wReturn+=fmtXMLint64("physicalposition",  PhysicalPosition,wLevel);  /* uint8_t */
  wReturn+=fmtXMLint64("logicalposition",LogicalPosition,wLevel);
  wReturn+=fmtXMLlong("currentrank",CurrentRank,wLevel);
  /* end see if it is really required */
  wReturn += fmtXMLendnode("zfdowndata",pLevel);
  return wReturn;
}//ZFDOwnData::toXml


/**
 * @brief fromXml loads ZFDOwnData from its xml definition and return 0 when successfull.
 * When errors returns <>0 and pErrlog contains appropriate error messages.
 * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
 */
int ZFDOwnData::fromXml(zxmlNode* pFDBRootNode,ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pFDBRootNode->getChildByName((zxmlNode *&) wRootNode, "zfdowndata");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZFDOwnData::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "zfdowndata",
        decode_ZStatus(wSt));
    return -1;
  }

  if (ZHeader.fromXml(wRootNode,pErrorlog)!=0)
    return -1;

  //  if (ZFCB==nullptr)
  //      ZFCB=new ZFileControlBlock;

  if (ZFCB.fromXml(wRootNode,pErrorlog)!=0)
    return -1;
  /* see if it is really required */
  if (XMLgetChildInt64(wRootNode, "physicalposition", PhysicalPosition, pErrorlog,ZAIES_Warning) < 0) {
    fprintf(stderr,
        "ZFDOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "physicalposition");
  }
  if (XMLgetChildInt64(wRootNode, "logicalposition", LogicalPosition, pErrorlog,ZAIES_Warning) < 0) {
    fprintf(stderr,
        "ZFDOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "logicalposition");
  }
  if (XMLgetChildLong(wRootNode, "currentrank", CurrentRank, pErrorlog) < 0) {
    fprintf(stderr,
        "ZFDOwnData::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "currentrank");
  }
  /* end see if it is really required */

  return (int)pErrorlog->hasError();
}//ZFDOwnData::fromXml



ZStatus ZFileDescriptor::fromXml(zxmlNode* pFDRootNode,ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;

  ZStatus wSt = pFDRootNode->getChildByName((zxmlNode *&) wRootNode, "zfiledescriptor");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZFileDescriptor::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "zfiledescriptor",
        decode_ZStatus(wSt));
    return wSt;
  }

  if (ZFDOwnData::fromXml(wRootNode,pErrorlog)!=0)
    return ZS_XMLERROR;

  if (XMLgetChildText(wRootNode, "uricontent", wValue, pErrorlog) < 0) {
    fprintf(stderr,
        "ZFileDescriptor::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "uricontent");
  }
  else
    URIContent=wValue.toCChar();

  if (XMLgetChildText(wRootNode, "uriheader", wValue, pErrorlog) < 0) {
    fprintf(stderr,
        "ZFileDescriptor::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "uriheader");
  }
  else
    URIHeader=wValue.toCChar();

  if (XMLgetChildText(wRootNode, "uridirectorypath", wValue, pErrorlog) < 0) {
    fprintf(stderr,
        "ZFileDescriptor::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "uridirectorypath");
  }
  else
    URIDirectoryPath=wValue.toCChar();

  if (ZHeader.fromXml(wRootNode,pErrorlog)!=0)
    return ZS_XMLERROR;

  if (ZFCB.fromXml(wRootNode,pErrorlog)!=0)
    return ZS_XMLERROR;

  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//ZFDOwnData::fromXml



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
  ZFCB.clear();
  ZBAT.clear();
  ZFBT.clear();
  ZHOT.clear();
//  ZDBT.clear();  // Deprecated

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
  HeaderAccessed = ZHAC_Nothing;

  Pid= getpid();  // get current pid for ZFileDescriptor
}

bool
testOpenModeValue (zmode_type pMode) {
  bool wValidMode=false;
  if (pMode & ZRF_Exclusive)
    wValidMode = true;
  if (pMode & ZRF_Delete_Only)
    wValidMode = true;
  if (pMode & ZRF_Write_Only)
    wValidMode = true;
  if (pMode & ZRF_Read_Only)
    wValidMode = true;
  return wValidMode;
}


ZStatus
ZFileDescriptor::_changeOpenMode (zmode_type pNewMode) {
  if (!_isOpen)
    return ZS_INVOP;
  if (!testOpenModeValue(pNewMode)) {
    return ZS_INVVALUE;
  }

  /* TODO test if new mode is compatible with file situation (lock etc.) */

  Mode=pNewMode;
  return ZS_SUCCESS;
}

ZStatus
ZFileDescriptor::_close()
{
ZStatus wSt=ZS_SUCCESS;

  if (close (ContentFd) < 0)
  {
    ZException.getErrno(errno,
        "ZFileDescriptor._close",
        ZS_FILEERROR,
        Severity_Severe,
        "System error closing file <%s>",URIContent.toCChar());
    wSt= ZS_FILEERROR;
  }
  if (close (HeaderFd) < 0)
  {
    ZException.getErrno(errno,
        "ZFileDescriptor._close",
        ZS_FILEERROR,
        Severity_Severe,
        "System error closing file <%s>",URIHeader.toCChar());
    wSt= ZS_FILEERROR;
  }
  _isOpen=false;
  Mode=ZRF_NotOpen;
  HeaderAccessed = ZHAC_Nothing ;
  ZRFPool->removeFileByFd(ContentFd);
  clearPartial();
  ContentFd=0;
  HeaderFd=0;
  return wSt;
}

void
ZFileDescriptor::_forceClose()
{
  rawClose (ContentFd);
  rawClose (HeaderFd);
  _isOpen=false;
  HeaderAccessed = ZHAC_Nothing;
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
  if (pURIPath.isEmpty())
  {
    ZException.setMessage("ZFileDescriptor::setPath ",
        ZS_INVNAME,
        Severity_Error,
        "Invalid file name <empty>.");
    return ZS_INVNAME;
  }
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

  URIContent = pURIPath;

  URIDirectoryPath = URIContent.getDirectoryPath();

  if (URIDirectoryPath.isEmpty())  // if directory path is not mentionned : then current working directory is taken
  {
    URIDirectoryPath=uriString::currentWorkingDirectory();
  }

  return generateURIHeader( pURIPath,URIHeader);
}// ZFileDescriptor::setPath

long
ZFileDescriptor::getRankFromAddress(const zaddress_type pAddress)
{
  for (long wi=0;wi<ZBAT.size();wi++)
    if (ZBAT.Tab(wi).Address==pAddress)
      return wi;
  ZException.setMessage(_GET_FUNCTION_NAME_,
      ZS_INVADDRESS,
      Severity_Error,
      " Address <%lld> does not correspond to a valid block address ",
      pAddress);
  return (-1) ;
}


utf8String ZFileDescriptor::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("zfiledescriptor",pLevel);
  wLevel++;

  /* NB: Lock and LockOwner are not exported to xml */

  wReturn+=fmtXMLchar("uricontent",  URIContent.toCChar(),wLevel);  /* uint8_t */
  wReturn+=fmtXMLchar("uriheader",URIHeader.toCChar(),wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"FYI : not modifiable - generated by ZRandomFile from uricontent");
  wReturn+=fmtXMLchar("uridirectorypath",URIDirectoryPath.toCChar(),wLevel);

  // wReturn+=ZFDOwnData::toXml(wLevel);  // ZFDOwnData has only volatile data (positionning)
  if (pComment)
    wReturn += fmtXMLcomment(" all children fields of zheadercontrolblock are not modifiable ",wLevel);
  wReturn+=ZHeader.toXml(wLevel);
  wReturn+=ZFCB.toXml(wLevel,pComment);

  wReturn += fmtXMLendnode("zfiledescriptor",pLevel);
  return wReturn;
}//ZFileDescriptor::toXml
