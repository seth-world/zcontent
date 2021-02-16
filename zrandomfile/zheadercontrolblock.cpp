#include "zheadercontrolblock.h"

#include <zxml/zxmlprimitives.h>

ZHeaderControlBlock::ZHeaderControlBlock()
{
  clear();
}

void ZHeaderControlBlock::clear(void)
{
  memset(this,0,sizeof(ZHeaderControlBlock));
  //    StartSign=cst_ZSTART;
  //    BlockID = ZBID_FileHeader ;
  //    ZRFVersion=__ZRF_VERSION__;
  //    EndSign=cst_ZEND;
  OffsetReserved = sizeof(ZHeaderControlBlock_Export); // because Reserved block just starts after ZHeaderControlBlock
  return;
}

ZHeaderControlBlock& ZHeaderControlBlock::_copyFrom(const ZHeaderControlBlock& pIn)
{
  Lock=pIn.Lock;
  LockOwner=pIn.LockOwner;
  FileType=pIn.FileType;
  OffsetFCB=pIn.OffsetFCB;
  OffsetReserved=pIn.OffsetReserved;
  SizeReserved=pIn.SizeReserved;
  return *this;
}

ZDataBuffer&
ZHeaderControlBlock::_export(ZDataBuffer& pZDBExport)
{
  ZHeaderControlBlock_Export wHCB;
  wHCB.StartSign = cst_ZSTART;// don't care reversing start sign or end sign : same as reversed

  wHCB.Lock = reverseByteOrder_Conditional<zlockmask_type>(Lock);
  wHCB.LockOwner = reverseByteOrder_Conditional<pid_t>(LockOwner);

  wHCB.BlockID = ZBID_FileHeader;
  wHCB.ZRFVersion = reverseByteOrder_Conditional<unsigned long>(__ZRF_VERSION__);
  wHCB.FileType = FileType;
  wHCB.OffsetFCB = reverseByteOrder_Conditional<zaddress_type>(OffsetFCB);
  //    wHCB.OffsetReserved = _reverseByteOrder_T<zaddress_type>(OffsetReserved);
  wHCB.OffsetReserved = reverseByteOrder_Conditional<zaddress_type>(sizeof(ZHeaderControlBlock_Export));
  wHCB.SizeReserved = reverseByteOrder_Conditional<zsize_type>(SizeReserved);

  wHCB.EndSign = cst_ZEND;// don't care reversing start sign or end sign : same as reversed
  pZDBExport.setData(&wHCB,sizeof(wHCB));

  return pZDBExport;
}
ZStatus
ZHeaderControlBlock::_import(unsigned char* pZDBImport_Ptr)
{

  ZHeaderControlBlock_Export* wHCB=(ZHeaderControlBlock_Export*)(pZDBImport_Ptr);
  //        StartSign = _reverseByteOrder_T<uint32_t>(wHCB->StartSign);
  //    StartSign = wHCB->StartSign;  // don't care reversing start sign or end sign : same as reversed

  Lock = reverseByteOrder_Conditional<zlockmask_type>(wHCB->Lock);
  LockOwner = reverseByteOrder_Conditional<pid_t>(wHCB->LockOwner);
  if ((wHCB->BlockID!=ZBID_FileHeader)||(wHCB->StartSign!=cst_ZSTART))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Severe,
        "invalid header block content found Start marker <%X> ZBlockID <%X>. One of these is invalid (or both are).",
        wHCB->StartSign,
        wHCB->BlockID);
    return  ZS_BADFILEHEADER;
  }
  if (reverseByteOrder_Conditional<unsigned long>(wHCB->ZRFVersion)!=__ZRF_VERSION__)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Severe,
        "invalid header block version : found version <%ld> while current version is <%ld>.",
        wHCB->ZRFVersion,
        __ZRF_VERSION__);
    return  ZS_BADFILEHEADER;
  }

  //    BlockID = wHCB->BlockID;
  //    ZRFVersion = _reverseByteOrder_T<unsigned long>(wHCB->ZRFVersion);
  FileType = wHCB->FileType;
  OffsetFCB = reverseByteOrder_Conditional<zaddress_type>(wHCB->OffsetFCB);
  OffsetReserved = reverseByteOrder_Conditional<zaddress_type>(wHCB->OffsetReserved);
  SizeReserved = reverseByteOrder_Conditional<zsize_type>(wHCB->SizeReserved);

  //    EndSign = wHCB->EndSign; // don't care reversing start sign or end sign : same as reversed

  return  ZS_SUCCESS;
}

utf8String ZHeaderControlBlock::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("zheadercontrolblock",pLevel);
  wLevel++;

  /* NB: Lock and LockOwner are not exported to xml */

  wReturn+=fmtXMLuint("filetype",  FileType,wLevel);  /* uint8_t */
  wReturn+=fmtXMLuint64("offsetfcb",OffsetFCB,wLevel);
  wReturn+=fmtXMLuint64("offsetreserved",OffsetReserved,wLevel);

  wReturn+=fmtXMLuint64("sizereserved",SizeReserved,wLevel);  /* uint16 must be casted */

  wReturn += fmtXMLendnode("zheadercontrolblock",pLevel);
  return wReturn;
} // ZRandomFile::toXml

int ZHeaderControlBlock::fromXml(zxmlNode* pHCBRootNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pHCBRootNode->getChildByName((zxmlNode *&) wRootNode, "zheadercontrolblock");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZHeaderControlBlock::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "zheadercontrolblock",
        decode_ZStatus(wSt));
    return -1;
  }

  if (XMLgetChildInt64(wRootNode, "offsetfcb", OffsetFCB, pErrorlog)< 0) {
    fprintf(stderr,
        "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "offsetfcb");
    }
  if (XMLgetChildInt64(wRootNode, "offsetreserved", OffsetReserved, pErrorlog)< 0) {
      fprintf(stderr,
        "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "offsetreserved");
      }
    if (XMLgetChildUInt64(wRootNode, "sizereserved", SizeReserved, pErrorlog)< 0) {
      fprintf(stderr,
          "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
          "default.",
          "sizereserved");
      }

  if (XMLgetChildUInt(wRootNode, "filetype", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "ZHeaderControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "filetype");
      }
      else
        FileType = (ZFile_type)wInt;


  return (int)pErrorlog->hasError();
}//ZHeaderControlBlock::fromXml

