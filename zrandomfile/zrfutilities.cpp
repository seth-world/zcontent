
#include "zrfutilities.h"
#include <fcntl.h>
#include <ztoolset/zexceptionmin.h>
#include <zio/zioutils.h>

const long cst_FileNudge = 100;


ZFileUtils::~ZFileUtils()
{
  if (_isOpen) {
    rawClose(Fd);
    rawClose(FdHeader);
  }
}

ZStatus
ZFileUtils::rebuildHeader(uriString * pURIHeader) {
  ZStatus wSt;
  ZBlockPool wZBAT;
  ZBlockPool wZFBT;
  ZBlockPool wZHOT;
  bool wHeaderLoaded=false;


  ZHeaderControlBlock_Export wHCBe;
  ZHeaderControlBlock wHCB;
  ZFileControlBlock wZFCB;
  ZFCB_Export wZFCBe;
  ZDataBuffer wZReserved;

  int wHoleNb=0 , wBANb=0 , wFBNb = 0 , wDelNb=0;
  size_t wBlockMeanSize=0;

  FileSize = URI.getFileSize();

  _print("ZFileUtils::rebuildHeader-I-PROCFIL Processing file <%s>",URI.toString());
  if (!pURIHeader) {
    wSt= generateURIHeader (URI,URIHeader);
    if (wSt!=ZS_SUCCESS)
    {return  wSt;}
  }
  _print("ZFileUtils::rebuildHeader-I-HEADFIL Header file is set to <%s>",URIHeader.toString());
  if (URIHeader.exists()) {
    _print("ZFileUtils::rebuildHeader-I-REPLACE Header file exists and will be replaced.");
    ZDataBuffer wHeaderContent;
    wSt=URIHeader.loadContent(wHeaderContent);
    _print("ZFileUtils::rebuildHeader-I-GETCONT Getting header content.");
    URIHeader.renameBck();

    const unsigned char* wPtr=(const unsigned char*)wHeaderContent.Data;
    wHCB._import(wPtr);
    wHeaderLoaded=true;
    _print(" File type is %s\n"
           " Reserved data size %ld\n",
            decode_ZFile_type(wHCB.FileType),
            wHCB.SizeReserved);
    if (wHCB.SizeReserved > 0) {
      wZReserved.setData(wPtr+wHCB.OffsetReserved,wHCB.SizeReserved);
      _print("Keeping Reserved data.");
    }
    else
      _print("No header data.");
    wPtr += wHCB.OffsetFCB;
    wZFCB._import(wPtr);

  }

  _print( "File control block data\n"
          "Start of data          %ld\n"
          "Block target size      %ld  <%s>\n"
          "Grab free space        %s\n"
          "Highwater marking      %s\n",
      wZFCB.StartOfData,
      wZFCB.BlockTargetSize,wHeaderLoaded?"Will be recomputed":"To be computed",
      wZFCB.GrabFreeSpace?"Set":"Not set",
      wZFCB.HighwaterMarking?"Set":"Not set");

  wZFCB.AllocatedSize = FileSize;
  _print("Allocated size set/reset to %ld\n",wZFCB.AllocatedSize);


  wSt=_openContent();
  if (wSt!=ZS_SUCCESS) {
    _print(ZException.last().formatUtf8());
    return wSt;
  }

  zaddress_type wAdCur=0L;
  ZBlockDescriptor_Export wBEx ,wBEx1;
  ZBlockDescriptor wBD;

  wSt=searchNextBlock(wBEx,wAdCur);
  if (wSt!=ZS_SUCCESS) {
    _print(ZException.last().formatUtf8());
    return wSt;
  }

  while ((wSt==ZS_SUCCESS) && (wBEx.Address < FileSize)) {

    wBD.Address=wBEx.Address;
    wBD.BlockSize = wBEx.BlockSize;
    wBD.Pid = wBEx.Pid;
    wBD.Lock = wBEx.Lock;
    wBD.State = wBEx.State;

    switch (wBD.State) {
    case ZBS_Used:
      wBANb++;
      wZBAT.push(wBD);
      wBlockMeanSize += wBD.BlockSize;
      _print("Used block found address %ld size %ld",wBD.Address,wBD.BlockSize);
      break;
    case ZBS_Deleted:
      wDelNb++;
      wBlockMeanSize += wBD.BlockSize;
      wZFBT._addSorted(wBD);
      _print("Deleted block found address %ld size %ld",wBD.Address,wBD.BlockSize);
      break;
    case ZBS_Free:
      wFBNb++;
      wZFBT._addSorted(wBD);
      _print("Free block found address %ld size %ld",wBD.Address,wBD.BlockSize);
      break;
    }// switch

    wAdCur = wBEx.Address + wBEx.BlockSize ;

    wSt=searchNextBlock(wBEx1,wAdCur);
    if (wSt!=ZS_SUCCESS) {
      if ((wSt==ZS_OUTBOUNDHIGH)||(wSt==ZS_EOF))
        break;
      _print(ZException.last().formatUtf8());
      return wSt;
    }
    /* is there a hole ? */
    if (wBEx1.Address > wAdCur) {
      wBD.Address = wAdCur;
      wBD.State = ZBS_Hole;
      wBD.BlockSize =  wBEx1.Address - wAdCur  ;
      wBD.Pid = 0;
      wBD.Lock = 0;
      wHoleNb++;
      wZHOT._addSorted(wBD);
      _print("Hole found address %ld size %ld",wBD.Address,wBD.BlockSize);
    }
    wBEx=wBEx1;
  } // while

  _print ("End of file address %lld\n",wAdCur);
  _print ("_________________________");
  _print ("Used blocks    %ld\n"
          "Free blocks    %ld\n"
          "Deleted blocks %ld\n"
          "Holes          %ld",
          wBANb,wFBNb,wDelNb,wHoleNb);
  _print ("_________________________\n");

  _print ("Formatting and writing header file.");

  wZFCB.BlockTargetSize = wBlockMeanSize / size_t(wBANb + wDelNb ) ;

  _print("BlockTargetSize computed to be %ld",wZFCB.BlockTargetSize);

  wSt=writeHeaderFromPool(URIHeader,wHCB,wZFCB,wZReserved,&wZBAT,&wZFBT,&wZHOT);
  if (wSt!=ZS_SUCCESS){
    _print("An error occurred while writing header file <%s>",URIHeader.toString());
    _print(ZException.last().formatFullUserMessage().toString());
    return wSt;
  }
  _print("Header file <%s> has been successfully written.",URIHeader.toString());
} // ZFileUtils::rebuildHeader

ZStatus
ZFileUtils::_openContent(uriString& pURIContent) {
  ZStatus wSt=rawOpen(Fd,pURIContent.toCChar(),O_RDWR);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  URI=pURIContent;
  return ZS_SUCCESS;
}

ZStatus
ZFileUtils::_openHeader(uriString *pURIHeader) {

  ZStatus wSt=ZS_SUCCESS;
  if (pURIHeader==nullptr) {
    wSt= generateURIHeader (URI,URIHeader);
    if (wSt!=ZS_SUCCESS)
    {return  wSt;}
  }
  else
    URIHeader=pURIHeader->toString();

  wSt=rawOpen(FdHeader,URIHeader.toCChar(),O_RDWR);
  if (wSt!=ZS_SUCCESS)
    rawClose(Fd);

  return wSt;
}

ZStatus
ZFileUtils::seekAndGet(ZDataBuffer& pOut, ssize_t &pSize, size_t pAddress) {

  ZStatus wSt=rawSeek(Fd,(off_t &)pAddress,SEEK_SET);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  if ((pSize + FileOffset) > FileSize) {
    pSize= FileSize -  FileOffset;
    if (pSize <= 0) {
      return ZS_EOF;
    }
  }

  wSt=rawRead(Fd,pOut,pSize);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  return ZS_SUCCESS;
} //seekAndGet


ZStatus
ZFileUtils::searchNextStartSign(zaddress_type pStartAddress, zaddress_type & pOutAddress) {


  if ((pStartAddress < 0)||(pStartAddress>FileSize)) {
    utf8VaryingString wStr;
    wStr.sprintf("Invalid address to search <%lld>.",pStartAddress);
    _print(wStr.toString());
    return ZS_OUTBOUND;
  }

  ZStatus wSt=ZS_SUCCESS;
  long wNudge = FileNudge;

  ZDataBuffer wRecord;
  if ( long(pStartAddress) - long(FileSize) < wNudge)
    wNudge = long(FileSize) - long(pStartAddress) ;
  else
    pStartAddress += wNudge;

  wSt=seekAndGet(wRecord,wNudge,pStartAddress);

  unsigned char* wPtr = wRecord.Data ;
  unsigned char* wPtrEnd = wRecord.Data + wRecord.Size;

  uint32_t* wStartSign = (uint32_t*)wPtr;
  if (*wStartSign== cst_ZFILEBLOCKSTART) {
    pOutAddress = pStartAddress;
    FileOffset = off_t(pStartAddress);
    return ZS_SUCCESS;
  }


  while ((pStartAddress < zaddress_type(FileSize)) && (wSt==ZS_SUCCESS)) {

    while ((*wStartSign!=cst_ZFILEBLOCKSTART) && (wPtr < wPtrEnd)) {
      wStartSign = (uint32_t*)wPtr;
      pStartAddress ++;
      FileOffset++;
      wPtr++;
    }
    if (*wStartSign==cst_ZFILEBLOCKSTART)
      break;

    if (pStartAddress >= zaddress_type(FileSize)) {
      _print("No start block mark found. File surface exhausted.");
      return ZS_OUTBOUNDHIGH;
    }
    pStartAddress -= 3;
    FileOffset -= 3;
    wSt=seekAndGet(wRecord,wNudge,pStartAddress);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wPtr = wRecord.Data;
    wPtrEnd = wRecord.Data + wRecord.Size ;
  } // while wAddress


  pOutAddress = pStartAddress;
  FileOffset = off_t(pStartAddress);

  return ZS_SUCCESS;
}// searchNextStartSign

void ZFileUtils::_print(const char* pFormat,...) {
  utf8VaryingString wOut;
  va_list ap;
  va_start(ap, pFormat);
  wOut.vsnprintf(500,pFormat,ap);
  va_end(ap);
  _print(wOut);
}

void ZFileUtils::_print(const utf8VaryingString& pOut) {
  if (_displayCallback==nullptr) {
    if (Output==nullptr)
      Output=stdout;
    fprintf(Output,pOut.toCChar());
    fprintf(Output,"\n");
    std::cout.flush();
  }
  else
    _displayCallback(pOut);
}

ZStatus
ZFileUtils::searchNextBlock(ZBlockDescriptor_Export& pBlock, zaddress_type pStartAddress) {
  ZBlockHeader_Export wBExp;
  ZDataBuffer wRecord;
  ssize_t wSize= ssize_t(sizeof(ZBlockHeader_Export));

  zaddress_type pOutAddress;

  ZStatus wSt=searchNextStartSign(pStartAddress,pOutAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wSt=seekAndGet(wRecord,wSize,pOutAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  memmove(&wBExp,wRecord.Data,sizeof(ZBlockHeader_Export));
  wBExp.deserialize();
  pBlock.State = wBExp.State;
  pBlock.Pid = wBExp.Pid;
  pBlock.Lock = wBExp.Lock;
  pBlock.BlockSize = wBExp.BlockSize;
  pBlock.Address = pOutAddress;

  return ZS_SUCCESS;
}
ZStatus
ZFileUtils::searchPreviousBlock(ZBlockDescriptor_Export& pBlock, zaddress_type pStartAddress) {
  ZDataBuffer wRecord;
  zaddress_type pOutAddress;
  ZBlockHeader_Export wBExp;
  ssize_t wSize= ssize_t(sizeof(ZBlockHeader_Export));
  ZStatus wSt=searchPreviousStartSign(pStartAddress,pOutAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wSt=seekAndGet(wRecord,wSize,pOutAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  memmove(&wBExp,wRecord.Data,sizeof(ZBlockHeader_Export));
  wBExp.deserialize();
  pBlock.State = wBExp.State;
  pBlock.Pid = wBExp.Pid;
  pBlock.Lock = wBExp.Lock;
  pBlock.BlockSize = wBExp.BlockSize;
  pBlock.Address = pOutAddress;

  BlockCur = pBlock;

  return ZS_SUCCESS;
} //searchPreviousBlock

void ZFileUtils::getPrevAddrVal(zaddress_type &pAddress, long &pNudge, long &pBucket){
  pNudge = FileNudge ;
  if (pAddress < pNudge) {
    pNudge = pBucket =  pAddress ;
    pAddress = 0L;
    return;
  }

  pAddress -= pNudge;

  if ((pAddress + FileNudge) > FileSize){
    pBucket = FileSize - pAddress ;
  }
  else
    pBucket = FileNudge + sizeof(cst_ZFILEBLOCKSTART) -1 ;
}

/* search previous start block starting at pAddress and decreasing to beginning of file */
ZStatus
ZFileUtils::searchPreviousStartSign(zaddress_type pStartAddress, zaddress_type & pOutAddress ) {

  if ((pStartAddress < 0)||(pStartAddress>FileSize)) {
    utf8VaryingString wStr;
    wStr.sprintf("Invalid address to search <%lld>.",pStartAddress);
    _print(wStr.toString());
    return ZS_OUTBOUND;
  }

  ZStatus wSt=ZS_SUCCESS;

  long wNudge = FileNudge ;
  long wBucket = wNudge + sizeof(cst_ZFILEBLOCKSTART) -1; /* just to take a truncated start sign into account */

  FileOffset = off_t(pStartAddress);

  ZDataBuffer wRecord;
  zaddress_type wAddress=pStartAddress;

  unsigned char* wPtr ;
  unsigned char* wPtrEnd ;
  uint32_t* wStartSign ;

  getPrevAddrVal(wAddress,wNudge,wBucket);

  wSt=seekAndGet(wRecord,wBucket,wAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wPtrEnd = wRecord.Data ;
  wPtr =  wRecord.Data + wRecord.Size - sizeof(cst_ZFILEBLOCKSTART) ;
  pStartAddress = wAddress + wRecord.Size - sizeof(cst_ZFILEBLOCKSTART);
  wStartSign=(uint32_t*)wPtr;
  while ((pStartAddress >= zaddress_type(0L)) && (wSt==ZS_SUCCESS)) {
    while ((*wStartSign!=cst_ZFILEBLOCKSTART) && (wPtr >= wPtrEnd)) {
      wStartSign = (uint32_t*)wPtr;
      //      pStartAddress --;
      //      FileOffset --;
      wPtr--;
    }
    if (*wStartSign==cst_ZFILEBLOCKSTART)
      break;

    /* here wPtr == wRecord.Data */

    getPrevAddrVal(wAddress,wNudge,wBucket);

    wSt=seekAndGet(wRecord,wBucket,wAddress);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    wPtrEnd = wRecord.Data  ;
    wPtr =  wRecord.Data + wRecord.Size - sizeof(cst_ZFILEBLOCKSTART);
    wStartSign=(uint32_t*)wPtr;
  } // while wAddress

  size_t wOffset = (wPtr-wRecord.Data) + 1;
  pOutAddress = wAddress + wOffset ;
  FileOffset = off_t(pOutAddress);
  return ZS_SUCCESS;
}// searchPreviousStartSign


ZStatus
ZFileUtils::writeHeaderFromPool(const uriString& pURIHeader,
    ZHeaderControlBlock& pHCB, ZFileControlBlock& pFCB, ZDataBuffer& pReserved,
    ZBlockPool* pZBAT,ZBlockPool* pZFBT,ZBlockPool* pZHOT)
{
  ZDataBuffer wHeaderContent,wNewHeaderContent;


  pHCB.OffsetReserved = sizeof(ZHeaderControlBlock_Export) ;
  pHCB.SizeReserved = pReserved.Size ;
  pHCB.OffsetFCB =    pHCB.OffsetReserved + pReserved.Size ;

  size_t wRet=pHCB._exportAppend(wHeaderContent);
  /* reserved : As it is */

  wHeaderContent.appendData(pReserved);

  /* FCB and Pool */

  // ==================Update FCB with offsets and sizes then export it===================================================

  pFCB.ZBAT_DataOffset =  sizeof(ZFCB_Export);  // ZBAT data Pool is stored first just after ZFCB

  pFCB.AllocatedBlocks = pZBAT->getAllocation();
  pFCB.BlockExtentQuota = pZBAT->getQuota();
  pFCB.ZBAT_ExportSize = pZBAT->getPoolExportSize();


  pFCB.ZFBT_DataOffset = (zaddress_type)(pFCB.ZBAT_DataOffset + pFCB.ZBAT_ExportSize);// then ZFBT
  //    ZFCB.ZFBT_ExportSize = ZFBT._getExportAllocatedSize();
  pFCB.ZFBT_ExportSize = pZFBT->getPoolExportSize();

  pFCB.ZHOT_DataOffset = (zaddress_type)(pFCB.ZFBT_DataOffset + pFCB.ZFBT_ExportSize);// then ZHOT
  pFCB.ZHOT_ExportSize = pZHOT->getPoolExportSize() ;

  wRet=pFCB._exportAppend(wHeaderContent);

  pZBAT->_exportAppendPool(wHeaderContent);  /* first block access table */
  pZFBT->_exportAppendPool(wHeaderContent);  /* second free blocks table */
  pZHOT->_exportAppendPool(wHeaderContent);  /* third Holes table */

  //  ZDataBuffer WReserved;

  /*
  ZHeader.OffsetReserved = sizeof(ZHeaderControlBlock_Export) ;
  ZHeader.SizeReserved = ZReserved.Size ;
  ZHeader.OffsetFCB =    ZHeader.OffsetReserved + ZReserved.Size ;
   *
   *  blocks positionning
   *
   * Header
   * Reserved
   * Fcb
   * ZBAT
   * ZFBT
   * ZDBT
   */


  return pURIHeader.writeContent(wHeaderContent);
} //updateHeaderFromPool



ZStatus
rawSearchNextStartSign(__FILEHANDLE__ pFd,size_t pFileSize,long pNudge,zaddress_type pStartAddress, zaddress_type & pOutAddress) {

  if ((pStartAddress < 0)||(pStartAddress>pFileSize)) {
    return ZS_OUTBOUND;
  }

  ZStatus wSt=ZS_SUCCESS;
  long wNudge = pNudge;

  ZDataBuffer wRecord;

  if ((wNudge + pStartAddress) > pFileSize) {
    wNudge = pFileSize -  pStartAddress;
    if (wNudge <= 0) {
      return ZS_EOF;
    }
  }

  wSt=rawReadAt(pFd,wRecord,wNudge,pStartAddress);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        wSt,
        Severity_Severe,
        "Error positionning at address <%ld> for file <%s> ",
        pStartAddress,
        getNameFromFd(pFd).toCChar());
    return wSt;
  }

  const unsigned char* wPtr = wRecord.Data ;
  const unsigned char* wPtrEnd = wRecord.Data + wRecord.Size;

  uint32_t* wStartSign = (uint32_t*)wPtr;
  if (*wStartSign == cst_ZFILEBLOCKSTART) {
    pOutAddress = pStartAddress;
    return ZS_SUCCESS;
  }

  while ((pStartAddress < zaddress_type(pFileSize)) && (wSt==ZS_SUCCESS)) {

    while ((*wStartSign!=cst_ZFILEBLOCKSTART) && (wPtr < wPtrEnd)) {
      wStartSign = (uint32_t*)wPtr;
      pStartAddress ++;
      wPtr++;
    }
    if (*wStartSign==cst_ZFILEBLOCKSTART)
      break;

    if (pStartAddress >= zaddress_type(pFileSize)) {
      return ZS_OUTBOUNDHIGH;
    }
    pStartAddress -= 3;

    wSt=rawReadAt(pFd,wRecord,wNudge,pStartAddress);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wPtr = wRecord.Data;
    wPtrEnd = wRecord.Data + wRecord.Size ;
  } // while wAddress


  pOutAddress = pStartAddress;
  return ZS_SUCCESS;
}// searchNextStartSign

bool testSequence (const unsigned char* pSequence,size_t pSeqLen,
    const unsigned char* pToCompare)
{
  while (pSeqLen) {
    if (*pSequence != *pToCompare)
      return false;
    pSequence++;
    pToCompare++;
    pSeqLen--;
  }
  return true;
}

ZStatus
searchNextSequence(__FILEHANDLE__ pFd,size_t pFileSize,long pNudge,
                    const unsigned char* pSequence,size_t pSeqLen,
                    zaddress_type pStartAddress, zaddress_type & pOutAddress)
{

  if ((pStartAddress < 0)||(pStartAddress>pFileSize)) {
    return ZS_OUTBOUND;
  }

  ZStatus wSt=ZS_SUCCESS;
  long wNudge = pNudge;

  ZDataBuffer wRecord;

  if ((wNudge + pStartAddress) > pFileSize) {
    wNudge = pFileSize -  pStartAddress;
    if (wNudge <= 0) {
      return ZS_EOF;
    }
  }

  wSt=rawReadAt(pFd,wRecord,wNudge,pStartAddress);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        wSt,
        Severity_Severe,
        "Error positionning at address <%ld> for file <%s> ",
        pStartAddress,
        getNameFromFd(pFd).toCChar());
    return wSt;
  }

  const unsigned char* wPtr = wRecord.Data ;
  const unsigned char* wPtrEnd = wRecord.Data + wRecord.Size;


  if (testSequence(pSequence,pSeqLen,wPtr)) {
    pOutAddress = pStartAddress;
    return ZS_SUCCESS;
  }


  while ((pStartAddress < zaddress_type(pFileSize)) && (wSt==ZS_SUCCESS)) {

    while (!testSequence(pSequence,pSeqLen,wPtr) && (wPtr < wPtrEnd)) {
      pStartAddress ++;
      wPtr++;
    }
    if (testSequence(pSequence,pSeqLen,wPtr))
      break;

    if (pStartAddress >= zaddress_type(pFileSize)) {
      return ZS_OUTBOUNDHIGH;
    }
    pStartAddress -= (pSeqLen-1);

    wSt=rawReadAt(pFd,wRecord,wNudge,pStartAddress);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wPtr = wRecord.Data;
    wPtrEnd = wRecord.Data + wRecord.Size ;
  } // while wAddress


  pOutAddress = pStartAddress;
  return ZS_SUCCESS;
}// searchNextSequence

ZStatus
rawGetBlockDescriptor(__FILEHANDLE__ pFdContent,ZBlockDescriptor& pBDOut,zaddress_type pAddress) {
  ZDataBuffer wBlock;

  ZStatus wSt=rawReadAt(pFdContent,wBlock,sizeof(ZBlockHeader_Export),pAddress);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  ZBlockHeader_Export* wBlockE=(ZBlockHeader_Export* )wBlock.Data;
  if (wBlockE->StartSign != cst_ZFILEBLOCKSTART) {
    return ZS_INVBLOCKADDR;
  }

  wBlockE->deserialize();

  pBDOut.Address = pAddress;
  pBDOut.BlockSize = wBlockE->BlockSize ;
  pBDOut.State = wBlockE->State  ;
  pBDOut.Lock = wBlockE->Lock  ;
  pBDOut.Pid = wBlockE->Pid   ;

  return ZS_SUCCESS;
} // rawGetBlockDescriptor

uint16_t
rawCheckContentBlock(int pPoolId,int pFdContent,ZBlockDescriptor& pBlockDesc) {
  ZDataBuffer wBlock;
  uint16_t  wRet = ZBEX_Correct;
  off_t wFileOffset = lseek(pFdContent,off_t(pBlockDesc.Address),SEEK_SET);
  if (wFileOffset < 0){
    wRet |= ZBEX_SysBadAddress;
    return wRet;
  }
  wBlock.allocate(sizeof(ZBlockHeader_Export));
  ssize_t wSize=::read(pFdContent,wBlock.Data,sizeof(ZBlockHeader_Export));
  if (wSize < 0) {
    wRet |= ZBEX_SysBadAddress;
    return wRet;
  }
  if (pBlockDesc.BlockSize==0) {
    wRet |= ZBEX_PoolZeroSize;
  }
  ZBlockHeader_Export* wBlockE=(ZBlockHeader_Export* )wBlock.Data;
  if (wBlockE->StartSign != cst_ZFILEBLOCKSTART) {
    wRet |= ZBEX_Orphan;
  }
  zsize_type wSize1 = reverseByteOrder_Conditional<zsize_type>(wBlockE->BlockSize);
  if (wSize1==0) {
    wRet |= ZBEX_ContentZeroSize;
  }
  if (wSize1!=pBlockDesc.BlockSize) {
    wRet |= ZBEX_Size;
  }

  if ((pPoolId==ZPTP_ZBAT)&&(pBlockDesc.State!=ZBS_Used)) {
    wRet |= ZBEX_MustBeUsed ;
  }
  if ((pPoolId==ZPTP_ZFBT)&&(pBlockDesc.State!=ZBS_Free) && (pBlockDesc.State!=ZBS_Deleted)) {
    wRet |= ZBEX_MustBeFreeOrDeleted;
  }
  /*  if ((pPoolId==ZPTP_ZDBT)&&(pBlockDesc.State!=ZBS_Deleted)) {
    wRet |= ZBEX_MustBeDeleted;
  }
*/
  return wRet;
} // checkContentBlock

utf8VaryingString
decode_ZBEx(uint16_t pBEx) {

  /* first preemptive statuses : one of these two cannot be combined with others */
  if (pBEx==ZBEX_Correct)
    return "<ZBEX_Correct> Pool block is correct";

  if (pBEx==ZBEX_SysBadAddress)
    return "<ZBEX_SysBadAddress> Cannot seed/read at given address on content file";

  /* then cumulative statuses : check following priority */

  if (pBEx & ZBEX_Orphan)
    return "<ZBEX_Orphan> Orphan block:<cst_ZFILEBLOCKSTART> missing";

  if (pBEx & ZBEX_PoolZeroSize)
    return "<ZBEX_PoolZeroSize> Block size in pool has zero value";

  if (pBEx & ZBEX_ContentZeroSize)
    return "<ZBEX_ContentZeroSize> Block size on content file has zero value ";

  /* these following are mutually exclusive */
  if (pBEx & ZBEX_MustBeUsed)
    return "<ZBEX_MustBeUsed> Block state must be ZBS_Used";
  if (pBEx & ZBEX_MustBeFreeOrDeleted)
    return "<ZBEX_MustBeFree> Block state must be ZBS_Free or ZBS_Deleted";
  //  if (pBEx & ZBEX_MustBeDeleted)
  //    return "<ZBEX_MustBeDeleted> Block state must be ZBS_Deleted";

  if (pBEx & ZBEX_Size)
    return "<ZBEX_Size> Block size in pool differs with block size on content file.";

  return "<?????> Unknown block check error ";

}
