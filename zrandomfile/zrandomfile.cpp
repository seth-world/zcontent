#ifndef ZRANDOMFILE_CPP
#define ZRANDOMFILE_CPP

//#define __REPORT_POOLS__ 1
#define __ZRFVERBOSE__

#include <zrandomfile/zrandomfile.h>

#include <zrandomfile/zrandomfiletypes.h>

#include <zcontentcommon/zcontentconstants.h>
#include <zrandomfile/zrfcollection.h>
#include <ztoolset/zdatabuffer.h>

#include <ztoolset/charman.h>

#include <stdio.h>
#include <unistd.h>

#include <zxml/zxmlprimitives.h>

#include <limits.h>


ZOpenZRFPool GZRFPool;
ZOpenZRFPool* ZRFPool=&GZRFPool;

using namespace  zbs;


ZStatus
ZOpenZRFPool::removeFileByFd(int pFd)
{
  for (long wi=0;wi<size();wi++)
  {
    if (pFd==Tab[wi]->getFileDescriptor().ContentFd)
    {
      erase(wi,1);
      return ZS_SUCCESS;
    }
  }// for

  return ZS_NOTFOUND;
}// removeFileByFd

ZStatus
ZOpenZRFPool::removeFileByObject(ZRandomFile *pZRF)
{
  ZStatus wSt=ZS_NOTFOUND;

  for (long wi=0;wi<size();wi++)
  {
    wSt=ZS_FILENOTOPEN;
    if (pZRF==Tab[wi])
    {
      if (Tab[wi]->isOpen())
        fprintf(stderr,"ZOpenZRFPool::removeFileByObject-W-ISOPEN file <%s> is open while removing its descriptor from open pool.\n", Tab[wi]->getURIContent().toCChar());
      erase(wi,1);
      return ZS_SUCCESS;
    }
  }// for

  return ZS_NOTFOUND;
}// removeFileByObject

void
ZOpenZRFPool::closeAll()
{

  long wNB=0;
  ZStatus wSt;

  ZException.setAbortOnSeverity(Severity_Highest);
  ZException.setThrowOnSeverity(Severity_Highest);

  //    if (__ZRFVERBOSE__)
  fprintf (stdout,"...Unlocking and closing all remaining open ZRandomFiles (if any)...\n");

  while (size())
  {
    zbs::ZRandomFile* wZRF=popRP(&wZRF);
    if (wZRF->isOpen())
    {
      fprintf(stderr,"ZOpenZRFPool::closeAll-I-CLOSING closing file <%s>....\n", wZRF->getURIContent().toCChar());
      wSt=wZRF->zclose();
      if (wSt!=ZS_SUCCESS)
        fprintf(stderr,"ZOpenZRFPool::closeAll-E-ERRCLOSE error <%s> while closing file <%s>.\n",decode_ZStatus(wSt), wZRF->getURIContent().toCChar());
      else
        fprintf(stderr,"ZOpenZRFPool::closeAll-I-DONE.\n");
      wNB++;
    }// wZRF->isOpen()

  }// while size()

  return;
}




//===================== Open ZRandomFiles management====================================================
namespace zbs { // see <ztoolset/zmodulestack.h> for global/external conditional definition  (__USE_ZRANDOMFILE__ must be defined in zconfig.h)


// for ZOpenZRFPool methods see at beginning of <zrandomfile/zrandomfile.cpp>


/*

 <!-- zrandomfile descriptor -->

 <zfiledescriptor>
  <uricontent> </uricontent>
  <uriheader> </uriheader>
  <uridirectorypath> </uridirectorypath>

  <zfdowndata> <!--see if it is required in xml -->
    <physicalposition> </physicalposition>
    <logicalposition> </logicalposition>
    <currentrank> </currentrank>
  </zfdowndata> <!-- en see if it is required in xml -->

  <zheadercontrolblock>
    <filetype> </filetype>
    <offsetfcb> </offsetfcb>
    <offsetreserved> </offsetreserved>
    <sizereserved> </sizereserved>
  </zheadercontrolblock>

  <zfilecontrolblock>
    <startofdata> </startofdata>
    <allocatedblocks> </allocatedblocks>
    <blockextentquota> </blockextentquota>
    <zbatdataoffset> </zbatdataoffset>
    <zbatexportsize> </zbatexportsize>
    <zfbtdataoffset> </zfbtdataoffset>
    <zfbtexportsize> </zfbtexportsize>
    <zdbtdataoffset> </zdbtdataoffset>
    <zdbtexportsize> </zdbtexportsize>
    <zreserveddataoffset> </zreserveddataoffset>
    <zreservedexportsize> </zreservedexportsize>
    <initialsize> </initialsize>
    <allocatedsize> </allocatedsize>
    <usedsize> </usedsize>
    <maxsize> </maxsize>
    <blocktargetsize> </blocktargetsize>
    <highwatermarking> </highwatermarking>
    <grabfreespace> </grabfreespace>

  </zfilecontrolblock>

</zfiledescriptor>


*/


}// namespace zbs
//===================== End Open ZRandomFiles management====================================================


bool ZRFStatistics = false;




/** @defgroup ZBSOptions Setting Options for runtime
 * We can set on or off options in order to change runtime behavior of product
@{
*/

/**
 * @brief setStatistics Set the option ZRFStatistics on (true) or off (false)
 * If ZRFStatistics is set then statistics are made and could be reported
 */
void setZRFStatistics (bool pStatistics) {ZRFStatistics=pStatistics;}
/** @} */



/**
File
offset

0L
|
|
|    ZHeaderControlBlock_Export
|
|
|
|
|
+--> sizeof(ZHeaderControlBlock_Export)
|
|
|
|
|    Reserved block :
|          - ZSMasterFile :
|               + MasterControlBlock_Export ,
|               + ZMetaDictionary_Export,
|               + IndexControlBlock_Export (1 per index)
|
|          - ZIndexFile :    ZIndexControlBlock_Export
|
|
|
|
|
+--> size of ZReserved Header
|
|
|
|
|    ZFileControlBlock_Export
|
|
|
|
|
|
+--> sizeof(ZFileControlBlock_Export)
|                          ----------Blocks pools--------------
| ZAExport
|                       ZBAT (exported format)
|-->sizeof(ZAExport)
|
|
| ZBAT flat content
|
|
|
|
|
+--> size of Export of ZBAT
|
| ZAExport
|                       ZFBT (exported format)
|-->sizeof(ZAExport)
|
|
| ZFBT flat content
|
|
|
|
|
+--> size of Export of ZDBT
|
| ZAExport
|                   ZBAT (exported format)
|-->sizeof(ZAExport)
|
|
| ZDBT flat content
|
|
|
|
|
+--> size of Export of ZBAT
|
|
+---> End of Header file







 */





using namespace std;









namespace zbs {

/** @addtogroup ZRandomFileGroup
 @{  */



//================================= ZBlockPool =====================

/**
 * @brief ZBlockPool::_exportPool DO NOT USE ZArray Standard import/export facilities for Block pools
 * @param pZDBExport
 * @return
 */
size_t
ZBlockPool::_exportAppendPool(ZDataBuffer&pZDBExport)
{

#ifdef __USE_ZTHREAD__
    _Base::lock();
#endif
    ZAExport ZAE;

    ZAE.AllocatedSize = _Base::getAllocatedSize();   // get the total allocated size
    ZAE.CurrentSize = _Base::getCurrentSize();     // get the current size corresponding to currently stored elements

    ZAE.AllocatedElements = _Base::getAllocation();
    ZAE.InitialAllocation = _Base::getInitalAllocation();
    ZAE.ExtentQuota = _Base::getQuota();
    ZAE.NbElements = _Base::ZCurrentNb;

    ZAE.DataSize = ZAE.NbElements * sizeof(ZBlockDescriptor_Export);

    ZAE.FullSize = getPoolExportSize();


#ifdef __REPORT_POOLS__
    fprintf (stdout,
         " -----Pool export report :\n"
         "    Allocated size      %ld\n"
         "    Current size        %ld\n"
         "    Data export size    %ld\n"
         "    Full export size    %ld\n"
         "    Allocated elemts    %ld\n"
         "    Extent quota        %ld\n"
         "    Initial allocation  %ld\n"
         "    Nb elements         %ld\n"
         "    Data offset         %ld\n",
         ZAE.AllocatedSize,
         ZAE.CurrentSize,
         ZAE.DataSize,
         ZAE.FullSize,
         ZAE.AllocatedElements,
         ZAE.ExtentQuota,
         ZAE.InitialAllocation,
         ZAE.NbElements,
         ZAE.DataOffset);
#endif // __REPORT_POOLS__

//    _Base::convertZAExport_I(ZAE);


    unsigned char* wPtr=pZDBExport.extendBZero(ZAE.FullSize);
    ZAE.serialize();
    memmove(wPtr,&ZAE,sizeof(ZAExport));



    wPtr += sizeof(ZAExport);

    ZBlockDescriptor_Export* wBlkExp =(ZBlockDescriptor_Export*) wPtr;
    for (long wi=0;wi<_Base::size();wi++)
        {
        _Base::Tab[wi]._exportConvert(_Base::Tab[wi],wBlkExp);
        wBlkExp++;
        }

#ifdef __USE_ZTHREAD__
    _Base::unlock();
#endif
    return ZAE.FullSize ;
}// _exportPool

size_t ZBlockPool::getPoolExportSize()
{
  return sizeof(ZAExport) + (sizeof(ZBlockDescriptor_Export) * count());
}


/**
 * @brief ZBlockPool::_importPool  DO NOT USE ZArray Standard import/export facilities for Block pools
 * @param pBuffer
 * @return
 */
size_t
ZBlockPool::_importPool(const unsigned char *& pPtrIn)
{

  ZAExport wZAE;
  wZAE.setFromPtr(pPtrIn); /* pPtrIn is updated to point after ZAExport */
  wZAE.deserialize();

//    return _ZAimport((ZArray<ZBlockDescriptor>*)this,pBuffer,&ZBlockDescriptor::_importConvert);

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
 _Base::lock();
#endif

//ZAExport ZAE = _Base::ZAEimport(pBuffer);

#ifdef __REPORT_POOLS__
fprintf (stdout,
         " +++++Pool import report :\n"
         "    Allocated size      %ld\n"
         "    Current size        %ld\n"
         "    Data export size    %ld\n"
         "    Full export size    %ld\n"
         "    Allocated elemts    %ld\n"
         "    Extent quota        %ld\n"
         "    Initial allocation  %ld\n"
         "    Nb elements         %ld\n"
         "    Data offset         %ld\n",
         wZAE.AllocatedSize,
         wZAE.CurrentSize,
         wZAE.DataSize,
         wZAE.FullSize,
         wZAE.AllocatedElements,
         wZAE.ExtentQuota,
         wZAE.InitialAllocation,
         wZAE.NbElements,
         wZAE.DataOffset);
#endif// __REPORT_POOLS__

_Base::setInitialAllocation(wZAE.InitialAllocation,false);  // no lock
_Base::setQuota(wZAE.ExtentQuota);                          // set reallocation quota
if (wZAE.NbElements > (ssize_t)wZAE.AllocatedElements)
    _Base::setAllocation(wZAE.NbElements,false);           // no lock : allocate space as number of elements
else
    _Base::setAllocation(wZAE.AllocatedElements,false);     // no lock : allocate space as allocated elements parameter
_Base::bzero(0,-1,false);                                   // no lock : set to zero

if (wZAE.NbElements>0)
    {
    _Base::newBlankElement(wZAE.NbElements,false); // no use of pClear : can memset ZBlockDescriptor
    ZBlockDescriptor_Export* wEltPtr_In=(ZBlockDescriptor_Export*)(pPtrIn);
    for (long wi=0;wi<wZAE.NbElements;wi++)
        {
        ZBlockDescriptor::_importConvert(Tab[wi],&wEltPtr_In[wi]);
        }// for

    } //(ZAE.NbElements>0)
#ifdef __USE_ZTHREAD__
_Base::unlock();
#endif
  pPtrIn += wZAE.DataSize;
return wZAE.FullSize;

}// _importPool


//----------------Set parameters------------------------------------


//------------Setting parameters on current ZRandomFile------------------------

ZStatus
ZRandomFile::_setParameters (ZFile_type pFileType,
                             const bool pGrabFreeSpace,
                             const bool pHighwaterMarking,
                             const size_t pBlockTargetSize,
                             const size_t pBlockExtentQuota)
{
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
    if (!_isOpen)
            {
            wasOpen=false;
            wSt = _ZRFopen(ZRF_Exclusive|ZRF_Write,pFileType);
            if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
        else
            {
            if ((getMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
                    {
                    wFormerMode = getMode();
                    zclose();
                    wSt = _ZRFopen(ZRF_Exclusive|ZRF_Write,pFileType);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast(" while setHighwaterMarking");
                        return wSt;
                        }
                    }
            }// else

    ZFCB.HighwaterMarking=pHighwaterMarking;
    ZFCB.GrabFreeSpace=pGrabFreeSpace;
    if (pBlockTargetSize>0)
      ZFCB.BlockTargetSize=pBlockTargetSize;
    if (pBlockExtentQuota>0)
      ZFCB.BlockExtentQuota=pBlockExtentQuota;

    Changed = true;
    wSt=_writeFCB(true);
    if (!wasOpen)
                return zclose();

    if (wFormerMode!=ZRF_Nothing)
        {
        zclose();
        return zopen(wFormerMode);
        }
    return wSt;
}//setParameters


/**
 * @brief setHighwaterMarking positions the higwater marking option to on (true) or off (false)
 *
 *  Subsequent operations on the file will be subject to this option.
 *  @note This option will be written to file header
 *  @see ZRandomFileGroup
 *
 * @param[in] pHighwaterMarking a boolean mentionning the option on (true) or off (false)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::setHighwaterMarking (const bool pHighwaterMarking)
{
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
    if (!_isOpen)
            {
            wasOpen=false;
            wSt = zopen(ZRF_Exclusive|ZRF_Write);
            if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
        else
            {
            if ((getOpenMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
                    {
                    wFormerMode = getOpenMode();
                    zclose();
                    wSt = zopen(ZRF_Exclusive|ZRF_Write);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast(" while setHighwaterMarking");
                        return wSt;
                        }
                    }
            }// else

    ZFCB.HighwaterMarking=pHighwaterMarking;
    wSt=_writeFCB(true);
    if (!wasOpen)
                return zclose();

    if (wFormerMode!=ZRF_Nothing)
        {
        zclose();
        return zopen(wFormerMode);
        }
    return wSt;
}//setHighwaterMarking

/**
 * @brief setGrabFreeSpace positions the GrabFreeSpace option to on (true) or off (false)
 *
 *  Subsequent operations on the file will be subject to this option.
 *  @note This option will be written to file header
 *  @see ZRandomFileGroup
 *
 * @param[in] pGrabFreeSpace  a boolean mentionning the option on (true) or off (false)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::setGrabFreeSpace (const bool pGrabFreeSpace)
{
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
    if (!_isOpen)
            {
            wasOpen=false;
            wSt = zopen(ZRF_Exclusive|ZRF_Write);
            if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
        else
            {
            if ((getOpenMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
                    {
                    wFormerMode = getOpenMode();
                    zclose();
                    wSt = zopen(ZRF_Exclusive|ZRF_Write);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast(" while setGrabFreeSpace");
                        return wSt;
                        }
                    }
            }// else

        ZFCB.GrabFreeSpace=pGrabFreeSpace;
        wSt=_writeFCB(true);
        if (!wasOpen)
                    return zclose();

        if (wFormerMode!=ZRF_Nothing)
            {
            zclose();
            return zopen(wFormerMode);
            }
        return wSt;
}//setGrabFreeSpace
/**
 * @brief setBlockTargetSize set the parameter BlockTargetSize to the given value et updates file header
 * @param[in] pBlockTargetSize new parameter value
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::setBlockTargetSize (const ssize_t pBlockTargetSize)
{
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
    if (!_isOpen)
            {
            wasOpen=false;
            wSt = zopen(ZRF_Exclusive|ZRF_Write);
            if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
        else
            {
            if ((getOpenMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
                    {
                    wFormerMode = getOpenMode();
                    zclose();
                    wSt = zopen(ZRF_Exclusive|ZRF_Write);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast(" while setBlockTargetSize");
                        return wSt;
                        }
                    }
            }// else
    ZFCB.BlockTargetSize=pBlockTargetSize;
    wSt=_writeFCB(true);
    if (!wasOpen)
                return zclose();

    if (wFormerMode!=ZRF_Nothing)
        {
        zclose();
        return zopen(wFormerMode);
        }
    return wSt;
}//setBlockTargetSize
/**
 * @brief setBlockExtentQuota set the parameter BlockExtentQuota to the given value et updates file header
 * @param[in] pBlockExtentQuota new parameter value
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::setBlockExtentQuota (const size_t pBlockExtentQuota)
{
  ZStatus wSt;
  bool wasOpen=true;
  zmode_type wFormerMode=ZRF_Nothing;
  if (!_isOpen)
  {
    wasOpen=false;
    wSt = zopen(ZRF_Exclusive|ZRF_Write);
    if (wSt!=ZS_SUCCESS)
      return wSt;
  }
  else
  {
    if ((getOpenMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
    {
      wFormerMode = getOpenMode();
      zclose();
      wSt = zopen(ZRF_Exclusive|ZRF_Write);
      if (wSt!=ZS_SUCCESS)
      {
        ZException.addToLast(" while setBlockTargetSize");
        return wSt;
      }
    }
  }// else
  ZFCB.BlockExtentQuota=pBlockExtentQuota;
  wSt=_writeFCB(true);
  if (!wasOpen)
    return zclose();

  if (wFormerMode!=ZRF_Nothing)
  {
    zclose();
    return zopen(wFormerMode);
  }
  return wSt;
}//setBlockExtentQuota

//----------------End Set parameters------------------------------------
//#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

ZStatus
ZRandomFile::_removeFile(const char* pContentPath,bool pBackup, ZaiErrors *pErrorLog)
{

  ZStatus wSt=zopen(pContentPath,ZRF_All);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  return _removeFile(pBackup,pErrorLog);
}

/**
 * @brief ZRandomFile::_removeFile if pBackup not set to true definitively remove ZRandomFile structure (content and header files).
 *                                  if set , files (content and header) are renamed to
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * @errors :
 *    ZS_INVOP :        file is already open when trying to open it
 *    ZS_FILENOTEXIST : file does not exist while it must exist for requested open mode
 *    ZS_ERROPEN :    low level file I/O error : errno is captured and system message is provided.
 *    ZS_LOCKED :     file is locked
 *    ZS_BADFILETYPE : file type is not what expected.
 *    ZS_MODEINVALID : open mode is incompatible with requested operation or open mode is not allowed by file lock.
 *
 *    ZS_FILEERROR  : cannot physically remove file (either content or header) ZException is loaded with errno and appropriate error explainations.
 */
ZStatus
ZRandomFile::_removeFile(bool pBackup,ZaiErrors *pErrorLog)
{
  ZStatus wSt;
  /* if not open : must open exclusive to load header data and to insure no user is using it */
  if (!isOpen())
    if ((wSt=zopen(ZRF_All))!=ZS_SUCCESS)
      {
        if (pErrorLog!=nullptr)
          pErrorLog->logZException();
        return wSt;
      }
  zclose();

  if (pBackup) {
    wSt=URIHeader.renameBck("bck");
    if (wSt!=ZS_SUCCESS) {
      if (pErrorLog!=nullptr)
        pErrorLog->logZException();
      return wSt;
    }

  } else
      if ((wSt=URIHeader.remove())!=ZS_SUCCESS) {
        if (pErrorLog!=nullptr)
          pErrorLog->logZException();
      return wSt;
  }

  if (pErrorLog!=nullptr)
      pErrorLog->infoLog("removed file <%s>",URIHeader.toCChar());


  if (pBackup) {
    wSt=URIContent.renameBck("bck");
    if (wSt!=ZS_SUCCESS) {
      if (pErrorLog!=nullptr)
        pErrorLog->logZException();
      return wSt;
    }

  } else
      if ((wSt=URIContent.remove())!=ZS_SUCCESS) {
        if (pErrorLog!=nullptr)
            pErrorLog->logZException();
        return wSt;
      }
  if (pErrorLog!=nullptr)
          pErrorLog->infoLog("removed file <%s>",URIContent.toCChar());
  return ZS_SUCCESS;
} //_removeFile

ZStatus
ZRandomFile::_renameBck(ZaiErrors* pErrorLog,const char* pBckExt)
{
  ZStatus wSt;
  /* if not open : must open exclusive to load header data and to insure no user is using it */
  if (!isOpen())
    {
    if ((wSt=zopen(ZRF_All))!=ZS_SUCCESS)
      {
      if (pErrorLog!=nullptr)
          pErrorLog->logZException();
      return wSt;
      }
    }
  zclose();
  int wFormerNumber = 1;
  uriString  wFormerContentURI;
  uriString  wFormerHeaderURI ;

  do {
      wFormerNumber ++;
      wFormerContentURI=URIContent;
      wFormerContentURI.addsprintf("_%s%02d",pBckExt,wFormerNumber);
      wFormerHeaderURI=URIHeader;
      wFormerHeaderURI.addsprintf("_%s%02d",pBckExt,wFormerNumber);
  } while ((wFormerContentURI.exists())||(wFormerHeaderURI.exists()));

  int wRet= rename(URIContent.toCChar(),wFormerContentURI.toCChar());
  if (wRet!=0)
      {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_FILEERROR,
          Severity_Severe,
          "During file creation : Error renaming existing file <%s> to <%s>\n",
          URIContent.toCChar(),
          wFormerContentURI.toCChar());

      if (pErrorLog!=nullptr)
        pErrorLog->logZException();
      return ZS_FILEERROR;
      }
  if (pErrorLog!=nullptr)
        pErrorLog->textLog("%s>> renamed existing file <%s> to <%s> \n",
                          _GET_FUNCTION_NAME_,
                          URIContent.toCChar(),
                          wFormerContentURI.toCChar());

  wRet= rename(URIHeader.toCChar(),wFormerHeaderURI.toCChar());
  if (wRet!=0)
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_FILEERROR,
          Severity_Severe,
          "During file creation : Error renaming existing file <%s> to <%s>\n",
          URIHeader.toCChar(),
          wFormerHeaderURI.toCChar());

      if (pErrorLog!=nullptr)
          pErrorLog->logZException();
      return ZS_FILEERROR;
    }

    if (pErrorLog!=nullptr)
      pErrorLog->textLog("%s>> renamed existing file <%s> to <%s> \n",
          _GET_FUNCTION_NAME_,
          URIHeader.toCChar(),
          wFormerHeaderURI.toCChar());

  return ZS_SUCCESS;
} //_renameBck

ZStatus
ZRandomFile::renameBck(const char* pContentPath, ZaiErrors *pErrorLog, const char *pBckExt)
{
  ZRandomFile wZRF;
  ZStatus wSt=wZRF.zopen(pContentPath,ZRF_Exclusive);
  if (wSt!=ZS_SUCCESS)
    {
    if (pErrorLog!=nullptr)
      pErrorLog->logZException();
    return wSt;
    }
  return wZRF._renameBck(pErrorLog,pBckExt);
}


//---------------ZMasterFile creation--------------------------------------
/**
 * @brief ZRandomFile::setCreateMinimum sets minimum ZRandomFile parameters and leave the rest to their default.
 * @param[in] pInitialSize
 */
void
ZRandomFile::setCreateMinimum(const zsize_type pInitialSize)
{
    ZFCB.clear();

    ZFCB.AllocatedSize      = pInitialSize;
    ZFCB.BlockTargetSize=(long) pInitialSize ;  // base of computation for an average user record size

/*    ZFCB.Journaling = false;
    ZFCB.Autocommit = false;
    ZFCB.History = false;*/

    ZFCB.HighwaterMarking = false;
}
/**
 * @brief ZRandomFile::setCreateMaximum sets whole ZRandomFile parameters see @ref ZRandomFile
 * @param[in] pAllocatedBlocks
 * @param[in] pBlockExtentQuota
 * @param[in] pBlockTargetSize
 * @param[in] pInitialSize
 * @param[in] pHighwaterMarking
 * @param[in] pGrabFreeSpace
 */
void
ZRandomFile::setCreateMaximum (const zsize_type pInitialSize,
                               const long pAllocatedBlocks,
                               const long pBlockExtentQuota,
                               const long pBlockTargetSize,
                               const bool pHighwaterMarking,
                               const bool pGrabFreeSpace)
{
        ZFCB.clear();
    /*
        ZFCB.Journaling = pJournaling;
        ZFCB.Autocommit = pAutocommit;
        ZFCB.History = pHistory;
      */
        ZFCB.HighwaterMarking = pHighwaterMarking;
        ZFCB.GrabFreeSpace = pGrabFreeSpace;

        ZFCB.AllocatedBlocks =pAllocatedBlocks;  // ZFBT+ZBAT allocated elements
        ZFCB.BlockExtentQuota=pBlockExtentQuota; // ZBAT+ZFBT  extension quota (number of elements) and base of computation for file extension size

        ZFCB.AllocatedSize = pInitialSize;       // initial allocated space in file
        ZFCB.InitialSize = pInitialSize;       // initial allocated space in file

    //    ZFCB.ExtentSizeQuota = pExtentSizeQuota;   // extension quota size for file : no more used

    //    ZFCB.ZFBTNbEntries = 0;                // ZFBT current elements
    //    ZFCB.ZBATNbEntries=0;                  // ZBAT current elements

        ZFCB.BlockTargetSize=pBlockTargetSize;    // base of computation for an average user record size

        ZBAT.setAllocation(pAllocatedBlocks); // number of elements in table and pool
        ZBAT.setQuota(pBlockExtentQuota);     // number of elements table and pool will extend at each time it is needed
        ZFBT.setAllocation(pAllocatedBlocks);
        ZFBT.setQuota(pBlockExtentQuota);
}

/**
 * @brief ZRandomFile::zcreate
 *
 * File creation with only 1 value : Initial  size . Other parameters are taken as their default value.
 *
 * File path must have been set previously with ZRandomFile::setPath().
 *
 * Initial size represents the amount of space initially allocated to the file at creation time.
 *
 * This space will be placed in free blocks pool (ZFreeBlockPool) and will be immediately available for use to be allocated as data blocks.
 *
 * @param[in] pInitialSize Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
 ZStatus
ZRandomFile::zcreate (const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{
    setCreateMinimum(pInitialSize);
    return (_create(pInitialSize,ZFT_ZRandomFile,pBackup, pLeaveOpen));
}// zcreate with no filename

/**
 * @brief ZRandomFile::zcreate ZRandomFile creation with a very simplified definition. The given file path that will name main content file.
 *
 * Main file content and file header are created with default parameters.
 *
 * @param[in] pURI  uriString containing the path of the future ZMasterFile main content file.
 *          Other file names will be deduced from this main name.
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pBackup      decides wether existing found files have to replaced or backuped with a version number.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zcreate (const uriString & pFilename,
                      const zsize_type pInitialSize,
                      bool pBackup,
                      bool pLeaveOpen)
{
ZStatus wSt;
        wSt=setPath (pFilename);
        if (wSt!=ZS_SUCCESS)
                    return wSt;
        return zcreate(pInitialSize,pBackup, pLeaveOpen);
}
/**
 * @brief ZRandomFile::zcreate ZRandomFile creation with a very simplified definition. The given file path that will name main content file.
 *
 * Main file content and file header are created with default parameters.
 *
 * @param[in] pFilename  a C string (char *) containing the path of the future ZMasterFile main content file.
 *          Other file names will be deduced from this main name.
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pBackup      decides wether existing found files have to replaced or backuped with a version number.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zcreate (const char * pFilename,
                      const zsize_type pInitialSize,
                      bool pBackup,
                      bool pLeaveOpen)
{
ZStatus wSt;
//    uriString wFilename;
//        wFilename = pFilename;
        wSt=setPath (pFilename);
        if (wSt!=ZS_SUCCESS)
                    return wSt;
        return zcreate(pInitialSize,pBackup,pLeaveOpen);
}

/**
 * @brief ZRandomFile::zcreate ZRandomFile creation with a full definition with a file path that will name main content file.
 * Main file content and file header are created with appropriate parameters as given in parameters.
 *
 * @param[in] pURI  uriString containing the path of the future ZMasterFile main content file.
 *          Other file names will be deduced from this main name.
 * @param[in] pAllocatedBlocks  number of initial elements in ZBAT pool and other pools(pInitialAlloc) see: @ref ZArrayParameters
 * @param[in] pBlockExtentQuota extension quota for pools (pReallocQuota) see: @ref ZArrayParameters
 * @param[in] pBlockTargetSize  approximation of best record size. see: @ref ZRFBlockTargetSize
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pHistory          RFFU History option true : on ; false : off
 * @param[in] pAutocommit       RFFU Autocommit option true : on ; false : off
 * @param[in] pJournaling       RFFU Journaling option true : on ; false : off
 * @param[in] pHighwaterMarking HighWaterMarking option true : on ; false : off  see: @ref ZRFHighWaterMarking
 * @param[in] pGrabFreeSpace    GrabFreespace option true : on ; false : off see: @ref ZRFGrabFreeSpace
 * @param[in] pBackup      decides wether existing found files have to replaced or backuped with a version number.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zcreate(const uriString &pFilename,
                     const zsize_type pInitialSize,
                     long pAllocatedBlocks,
                     long pBlockExtentQuota,
                     long pBlockTargetSize,
                     bool pHighwaterMarking,
                     bool pGrabFreeSpace,
                     bool pBackup,
                     bool pLeaveOpen)
{
ZStatus wSt;

    wSt=setPath(pFilename);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return (zcreate(pInitialSize,
                    pAllocatedBlocks,
                    pBlockExtentQuota,
                    pBlockTargetSize,
                    pHighwaterMarking,
                    pGrabFreeSpace,
                    pBackup,
                    pLeaveOpen));
}// zcreate with filename as uriString

/**
 * @brief ZRandomFile::zcreate ZRandomFile creation with a full definition with a file path that will name main content file.
 * Main file content and file header are created with appropriate parameters as given in parameters.
 *
 * @param[in] pFilename  a C string (char *) containing the path of the future ZMasterFile main contentfile.
 *          Other file names will be deduced from this main name.
 * @param[in] pAllocatedBlocks  number of initial elements in ZBAT pool and other pools(pInitialAlloc) see: @ref ZArrayParameters
 * @param[in] pBlockExtentQuota extension quota for pools (pReallocQuota) see: @ref ZArrayParameters
 * @param[in] pBlockTargetSize  approximation of best record size. see: @ref ZRFBlockTargetSize
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pHistory          RFFU History option true : on ; false : off
 * @param[in] pAutocommit       RFFU Autocommit option true : on ; false : off
 * @param[in] pJournaling       RFFU Journaling option true : on ; false : off
 * @param[in] pHighwaterMarking HighWaterMarking option true : on ; false : off  see: @ref ZRFHighWaterMarking
 * @param[in] pGrabFreeSpace    GrabFreespace option true : on ; false : off see: @ref ZRFGrabFreeSpace
 * @param[in] pBackup      decides wether existing found files have to replaced or backuped with a version number.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zcreate(const char* pFilename,
                     const zsize_type pInitialSize,
                     long pAllocatedBlocks,
                     long pBlockExtentQuota,
                     long pBlockTargetSize,
                     bool pHighwaterMarking,
                     bool pGrabFreeSpace,
                     bool pBackup,
                     bool pLeaveOpen)
{
uriString wFilename;
    wFilename=pFilename;
    return (zcreate(wFilename,
                    pInitialSize,
                    pAllocatedBlocks,
                    pBlockExtentQuota,
                    pBlockTargetSize,
                    pHighwaterMarking,
                    pGrabFreeSpace,
                    pBackup,
                    pLeaveOpen));
}// zcreate with filename as const char*

/**
 * @brief ZRandomFile::zcreate ZRandomFile creation with a full definition without a file path that should have been set before.
 * File path should have been set with ZRandomFile::setPath() method.
 * Main file content and file header are created with appropriate parameters as given in parameters.
 *
 * @param[in] pAllocatedBlocks  number of initial elements in ZBAT pool and other pools(pInitialAlloc) see: @ref ZArrayParameters
 * @param[in] pBlockExtentQuota extension quota for pools (pReallocQuota) see: @ref ZArrayParameters
 * @param[in] pBlockTargetSize  approximation of best record size. see: @ref ZRFBlockTargetSize
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pHistory          RFFU History option true : on ; false : off
 * @param[in] pAutocommit       RFFU Autocommit option true : on ; false : off
 * @param[in] pJournaling       RFFU Journaling option true : on ; false : off
 * @param[in] pHighwaterMarking HighWaterMarking option true : on ; false : off  (defaulted to false) see: @ref ZRFHighWaterMarking
 * @param[in] pGrabFreeSpace    GrabFreespace option true : on ; false : off (defaulted to true) see: @ref ZRFGrabFreeSpace.
 * @param[in] pJournaling       Journaling  option true : on ; false : off (defaulted to fale) see: @ref ZRFGrabFreeSpace.
 * @param[in] pBackup      decides wether existing found files have to replaced or backuped with a version number.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zcreate(const zsize_type pInitialSize,
                     long pAllocatedBlocks,
                     long pBlockExtentQuota,
                     long pBlockTargetSize,
                     bool pHighwaterMarking,
                     bool pGrabFreeSpace,
                     bool pBackup,
                     bool pLeaveOpen)
{
ZStatus wSt;
    setCreateMaximum (pInitialSize,
                      pAllocatedBlocks,
                      pBlockExtentQuota,
                      pBlockTargetSize,
                      pHighwaterMarking,
                      pGrabFreeSpace);
    wSt=_create(pInitialSize,ZFT_ZRandomFile,pBackup,pLeaveOpen);
    if (wSt!=ZS_SUCCESS)
                return wSt;


    return (wSt);

 } // zcreate

//!   @cond Development
/**
 * @brief ZRandomFile::_create Main internal file creation routine : all parameters must have been defined within ZFCB.
 *
 * @warning All parameters must have been set previously to calling this method.
 *
 * This routine does the creation job :
 * - Main file content and header file are created (reset to zero if exist depending pBackup value) and set appropriately.
 * - Internal file data structure is set up and cleared.
 *
 * If a file of same name already exist, then either :
 *
 *  - pBackup is set to false : the file's data is destroyed (content file + header file) and replaced by the created one.
 *
 *  - pBackup is set to true
 *    + content file name is renamed to <base name>.<extension>_BCK<nn>
 *    + header file name is renamed to <base name>.<__HEADER_FILEEXTENSION__>_BCK<nn>
 *
 *  Where <nn> represents the file version number
 *
 *  And <__HEADER_FILEEXTENSION__> represents the defined header file extension see @file zrandomfiletypes.h
 *
 * @param[in] pDescriptor   the file descriptor
 * @param[in] pInitialSize  Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pFileType     file type to create. defined by ZFile_type value.
 * @param[in] pBackup      decides wether existing found files have to replaced or backuped with a version number.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_create(const zsize_type pInitialSize,ZFile_type pFileType,bool pBackup,bool pLeaveOpen)
{
ZStatus wSt;
ZBlockDescriptor wFreeBlock;
ZBlockDescriptor_Export wFreeBlockExp;
ZDataBuffer wFileSpace;
int wS;

uriString wFormerContentURI;
uriString wFormerHeaderURI;

    printf ("ZRandomFile::_create \n");

    if (pInitialSize<=0) {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVSIZE,
                              Severity_Severe,
                              "Invalid file size <%ld> given for creating file <%s>\n",
                              pInitialSize,
                              URIContent.toString());
        return (ZS_INVSIZE);
        }

  if (URIContent.exists())
    {
    if (pBackup)
        URIContent.renameBck("bck");
      else
        remove(URIContent.toCChar());
    }
  if (URIHeader.exists())
    {
      if (pBackup)
        URIHeader.renameBck("bck");
      else
        remove(URIHeader.toCChar());
    }
  mode_t wPosixMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  ContentFd = open(URIContent.toCChar(),O_WRONLY|O_CREAT|O_TRUNC,wPosixMode);  // create or reset the file if exists
  if (ContentFd < 0)
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_ERROPEN,
                            Severity_Severe,
                            "Error opening file for creation <%s> ",
                            URIContent.toString());
            ZException.printLastUserMessage();
            }


  HeaderFd = open(URIHeader.toCChar(),O_WRONLY|O_CREAT|O_TRUNC,wPosixMode);
  if (HeaderFd<0)
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_ERROPEN,
                            Severity_Severe,
                            "Error opening header file for creation <%s> ",
                            URIHeader.toString());
            close(ContentFd);
            return(ZS_ERROPEN);
            }

  _isOpen=true;
  ZRFPool->addOpenFile(this);

// header
//
//         allocate header recordtable with pRecordAlloc + pExtentQuota
//         allocate freetable with pRecordAlloc + pExtentQuota
//


  if (ZFCB.AllocatedBlocks > 0)
      {
      ZBAT.setAllocation(ZFCB.AllocatedBlocks);
      ZFBT.setAllocation(ZFCB.AllocatedBlocks);
      ZDBT.setAllocation(ZFCB.AllocatedBlocks);
      }
      else
      ZFCB.AllocatedBlocks = ZBAT.getAllocation();  // get the value from default actual values

  if (ZFCB.BlockExtentQuota > 0)
      {
      ZBAT.setQuota(ZFCB.BlockExtentQuota);
      ZFBT.setQuota(ZFCB.BlockExtentQuota);
      ZDBT.setQuota(ZFCB.BlockExtentQuota);
      }
      else
      ZFCB.BlockExtentQuota = ZBAT.getQuota();  // get the value from default actual values



  if (ZFCB.BlockTargetSize==0)
      {
//    ZFCB.BlockTargetSize = (long)((float)ZFCB.AllocatedSize / (float)ZFCB.AllocatedBlocks) ;
      if (ZFCB.AllocatedBlocks>0)
      ZFCB.BlockTargetSize = (long)((float)pInitialSize / (float)ZFCB.AllocatedBlocks) ;
      else
      ZFCB.BlockTargetSize = pInitialSize ;
      }
  ZBAT.reset();       // reset number of elements to zero but keep allocation
  ZFBT.reset();
  ZDBT.reset();


//------------Extending file (content file) to its initial allocation-------------------

//    ZFCB.ZFBTNbEntries = ZFBT.size() ;              //! size of ZFBT is initially 1 : one block containing the whole file space
  zaddress_type wNewOffset ;
  if ((wNewOffset=(zaddress_type)lseek(ContentFd,0L,SEEK_SET))<0)    // position to beginning of file and get position in return
      {
      ZException.getErrno(errno,
                          _GET_FUNCTION_NAME_,
                          ZS_FILEPOSERR,
                          Severity_Severe,
                          " Severe error while positionning to beginning of file %s",
                          URIContent.toString());

      wSt=ZS_FILEPOSERR;
      goto _createEnd;
      }

  wS=    posix_fallocate(ContentFd,0L,(off_t)pInitialSize);
  if (wS!=0)
      {
      ZException.getErrno(wS,  // no errno is set by posix_fallocate : returned value is the status : ENOSPC means no space
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEERROR,
                          Severity_Severe,
                          " Error during creation of file <%s> cannot allocate disk space",
                          URIContent.toString());
      wSt=ZS_WRITEERROR;
      goto _createEnd;
      }

    /* create one block as entry in ZFreeBlockPool
      with initial file size as free space           */

    wFreeBlock.Address = ZFCB.StartOfData ;
    wFreeBlock.State   = ZBS_Free;
    wFreeBlock.BlockSize =pInitialSize;

    wSt=_writeBlockHeader(wFreeBlock,ZFCB.StartOfData);

    if (wSt!=ZS_SUCCESS)
                {
                goto _createEnd;
                }

//    create free block content in file

    wFileSpace.allocateBZero(pInitialSize-sizeof(ZBlockHeader_Export));  // ZBlockHeader is written on File - ZBlockDescriptor is the representation in memory

    if (write (ContentFd,wFileSpace.Data,wFileSpace.Size)<wFileSpace.Size)      // write extent zeroed
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_WRITEERROR,
                                 Severity_Severe,
                                 " Severe error while extending file %s",
                                 URIContent.toString());
                wSt=ZS_WRITEERROR;
                goto _createEnd;
                }

    fdatasync(ContentFd);

    // create entry in ZFreeBlockPool with initial file size as free space

     ZFBT.push(wFreeBlock);

//--------write header----------------------
//
    Mode = ZRF_Nothing;
    if (pLeaveOpen)
             {
             Mode = ZRF_Exclusive | ZRF_All ;
             _isOpen = true;
             }

    ZHeader.FileType = pFileType; // ZFile_type is here

    ZFCB.InitialSize = pInitialSize;
    ZFCB.AllocatedSize = pInitialSize;

    ZHeader.FileType = pFileType ;

//    wSt=_writeFullFileHeader(true);
    wSt=_writeAllFileHeader();

_createEnd:
    if (pLeaveOpen)
            {
            return wSt;
            }

    _isOpen = false;
    close(HeaderFd);
    close(ContentFd);

    ZRFPool->removeFileByObject(this);

    return(wSt);
}//_create



/**
 * @brief ZRandomFile::_writeFileHeader  writes the header block for file : uses ZFileDescriptor::ZReserved data to deduce appropriate offsets
 *
 * So, ZFileDescriptor::ZReserved MUST BE up-to-date when calling this method.
 *
 * @param[in] pDescriptor current file descriptor
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_writeFileHeader( bool pForceWrite)
{

  printf ("ZRandomFile::_writeFileHeader\n");
  std::cout.flush();
/*  if (Mode & ZRF_Exclusive)
     {
       if ((!pForceWrite)&&(HeaderAccessed & ZHAC_HCB))
       {  return  ZS_SUCCESS;}
     }
*/
  ZHeader.OffsetReserved = sizeof(ZHeaderControlBlock_Export) ;
  ZHeader.SizeReserved = ZReserved.Size ;
  ZHeader.OffsetFCB =    ZHeader.OffsetReserved + ZReserved.Size ;

 // NB: ZHeader.Offset is set by clear() method

 if (lseek(HeaderFd,0L,SEEK_SET) < 0)
             {
             ZException.getErrno(errno,
                              _GET_FUNCTION_NAME_,
                              ZS_FILEPOSERR,
                              Severity_Severe,
                              "Error positionning at Header address <%ld> of header file  <%s>",
                              0L,
                              URIHeader.toString());
             return(ZS_FILEPOSERR);
             }
 // write ZFileHeader block
 //
 ZPMS.HFHWrites ++;
 //ZHeader.BlockId = ZBID_FileHeader ;

 ZDataBuffer wZDBHeaderExport;
 ZHeader._exportAppend(wZDBHeaderExport);
// ssize_t wSWrite =write(HeaderFd,(char *)&ZHeader,sizeof(ZHeaderControlBlock)); // write file header infos
 ssize_t wSWrite =write(HeaderFd,(char *)wZDBHeaderExport.DataChar,wZDBHeaderExport.Size); // write file header infos
 if (wSWrite<0)
         {
         ZException.getErrno(errno,
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEERROR,
                          Severity_Error,
                          "Error while writing File Header block at address 0L to file header %s",
                          URIHeader.toString());
         return (ZS_WRITEERROR);
         }
  HeaderAccessed |= ZHAC_HCB;
 return ZS_SUCCESS;
}//_writeFileHeaderBlock

#ifdef __COMMENT__
ZStatus
ZRandomFile::_readFileLock( lockPack &pLockPack)
{

  printf ("ZRandomFile::_readFileLock\n");

  size_t wLockInfoOffset = cst_HeaderOffset + offsetof(ZHeaderControlBlock_Export, Lock);

  if (lseek(HeaderFd,wLockInfoOffset,SEEK_SET) < 0)
                 {
                 ZException.getErrno(errno,
                                  _GET_FUNCTION_NAME_,
                                  ZS_FILEPOSERR,
                                  Severity_Severe,
                                  "Error positionning at Header address <%ld> of header file  <%s>",
                                  wLockInfoOffset,
                                  URIHeader.toString());
                 return  (ZS_FILEPOSERR);
                 }

    ssize_t wSRead =read(HeaderFd,(char *)&pLockPack,sizeof(lockPack));  // read lock infos with packed structure

    if (wSRead<0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILEHEADER,
                                 Severity_Error,
                                 "Error reading  Header control block address <%lld> file  <%s>",
                                 0L,
                                 URIHeader.toString());
                return  (ZS_BADFILEHEADER);
                }

//    pLockPack.Lock=pLockPack.Lock;
    pLockPack.LockOwner=reverseByteOrder_Conditional<pid_t>(pLockPack.LockOwner);

    ZHeader.Lock =        pLockPack.Lock;
    ZHeader.LockOwner =   pLockPack.LockOwner;

    return  ZS_SUCCESS;

}//_readFileLock

ZStatus
ZRandomFile::_writeFileLock(lockPack &pLockPack)
{
  printf ("ZRandomFile::_writeFileLock\n");
  ZHeader.Lock = pLockPack.Lock;
  ZHeader.LockOwner = pLockPack.LockOwner;

  lockPack wLockPack;
  wLockPack.Lock =pLockPack.Lock;
  wLockPack.LockOwner = reverseByteOrder_Conditional<pid_t>(pLockPack.LockOwner);

  size_t wLockInfoOffset = cst_HeaderOffset + offsetof(ZHeaderControlBlock_Export, Lock);

  if (lseek(HeaderFd,(off_t)wLockInfoOffset,SEEK_SET) < 0)
                 {
                 ZException.getErrno(errno,
                                  _GET_FUNCTION_NAME_,
                                  ZS_FILEPOSERR,
                                  Severity_Severe,
                                  "Error positionning at Header address <%ld> of header file  <%s>",
                                  wLockInfoOffset,
                                  URIHeader.toString());
                 return  (ZS_FILEPOSERR);
                 }
  ssize_t wSWrite =write(HeaderFd,(char *)&wLockPack,sizeof(lockPack)); // write lock infos to file header s
  if (wSWrite<0)
             {
             ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_WRITEERROR,
                                 Severity_Error,
                                 "Error while writing File Header block at address <%ld> to file header %s",
                                 wLockInfoOffset,
                                 URIHeader.toString());
             return  (ZS_WRITEERROR);
             }
     return  ZS_SUCCESS;
}//_writeFileLock


ZStatus
ZRandomFile::_readFileLockOld( lockPack &pLockPack)
{

  printf ("ZRandomFile::_readFileLock\n");
  ZHeaderControlBlock_Export wHCB_Export;

  ZStatus wSt=_getFileHeader_Export(&wHCB_Export);
  if (wSt != ZS_SUCCESS)
    return wSt;

  wHCB_Export._convert();

  pLockPack.Lock=wHCB_Export.Lock;
  pLockPack.LockOwner=wHCB_Export.LockOwner;
  return ZS_SUCCESS;

  pLockPack.Lock=reverseByteOrder_Conditional<zlockmask_type>(pLockPack.Lock);
  pLockPack.LockOwner=reverseByteOrder_Conditional<pid_t>(pLockPack.LockOwner);

  size_t wLockInfoOffset = offsetof(ZHeaderControlBlock_Export, Lock);

  if (lseek(HeaderFd,wLockInfoOffset,SEEK_SET) < 0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning at Header address <%lld> of header file  <%s>",
        wLockInfoOffset,
        URIHeader.toString());
    return  (ZS_FILEPOSERR);
  }

  ssize_t wSRead =read(HeaderFd,(char *)&pLockPack,sizeof(lockPack));  // read lock infos with packed structure

  if (wSRead<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Error,
        "Error reading  Header control block address <%lld> file  <%s>",
        0L,
        URIHeader.toString());
    return  (ZS_BADFILEHEADER);
  }

  pLockPack.Lock=reverseByteOrder_Conditional<zlockmask_type>(pLockPack.Lock);
  pLockPack.LockOwner=reverseByteOrder_Conditional<pid_t>(pLockPack.LockOwner);
  return  ZS_SUCCESS;

}//_readFileLock

ZStatus
ZRandomFile::_writeFileLockOld(lockPack &pLockPack)
{
  printf ("ZRandomFile::_writeFileLock\n");
  ZHeader.Lock = pLockPack.Lock;
  ZHeader.LockOwner = pLockPack.LockOwner;

  return _writeFileHeader(true);

  lockPack wLockPack;
  wLockPack.Lock = reverseByteOrder_Conditional<zlockmask_type>(pLockPack.Lock);
  wLockPack.LockOwner = reverseByteOrder_Conditional<pid_t>(pLockPack.LockOwner);

  size_t wLockInfoOffset = offsetof(ZHeaderControlBlock_Export, Lock);

  if (lseek(HeaderFd,(off_t)wLockInfoOffset,SEEK_SET) < 0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning at Header address <%lld> of header file  <%s>",
        wLockInfoOffset,
        URIHeader.toString());
    return  (ZS_FILEPOSERR);
  }
  ssize_t wSWrite =write(HeaderFd,(char *)&wLockPack,sizeof(lockPack)); // write lock infos to file header s
  if (wSWrite<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_WRITEERROR,
        Severity_Error,
        "Error while writing File Header block at address <%ld> to file header %s",
        wLockInfoOffset,
        URIHeader.toString());
    return  (ZS_WRITEERROR);
  }
  return  ZS_SUCCESS;
}//_writeFileLock

/**
 * @brief ZRandomFile::_writeReservedHeader  writes the entire file header to disk. This method is the only way to update reserved infradata when its size has changed.
 * @param[in] pDescriptor   File descriptor for wich Reserved infra data block will be written
 * @param[out] pForceWrite  if true : will write any time. if false and file is opened in exclusive mode : will not write
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_writeAllHeaders(bool pForceWrite)
{
ZStatus wSt;
ssize_t wSWrite;

/*    if (Mode & ZRF_Exclusive)
           {
             if ((!pForceWrite)&&(HeaderAccessed & ZHAC_RESERVED))
             {  return  ZS_SUCCESS;}
           }
*/
    //             ZReserved + ZFCB  + ZBAT content + ZFBT content + ZReserved
    //
    if (ZReserved.Size==0)   // if there is a Reserved space
          {  return  ZS_SUCCESS;}


    if ((wSt=_writeFileHeader(pForceWrite))!=ZS_SUCCESS)
                                    {  return  wSt;}

    ZPMS.HReservedWrites ++;
    wSWrite =write(HeaderFd,ZReserved.DataChar,ZReserved.Size); // write Reserved infradata
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Error,
                             "Error while writing Reserved infra data to file header %s",
                             URIHeader.toString());
            return  (ZS_WRITEERROR);
            }






    return  _writeFileDescriptor(pForceWrite); //! this is absolutely necessary because size of Reserved block may vary. A future optimization could test if Reserved block size has changed and will save to this occasion this access.
}// _writeReservedHeader
#endif // __COMMENT__

/**
 * @brief ZRandomFile::_writeFCB writes to disk File Control Block and Pool (ZBAT, ZFBT, ZDBT)
 *
 * @param[out] pForceWrite  if true : will write any time. if false and file is opened in exclusive mode : will not write
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * @errors
 *  - ZS_FILEPOSERR bad positionning during lseek operation ZException is set
 *  - ZS_WRITEERROR error writing FCB + block pools to header file ZException is set
 */
ZStatus
ZRandomFile::_writeFCB(bool pForceWrite)
{
// Data offsets are computed from Beginning of FCB that may vary is Header size and/or Reserved Data Size Change
//

  if (!pForceWrite && !HeaderAccessed)
    return ZS_SUCCESS;
// ==================Update FCB with offsets and sizes then export it===================================================

    ZFCB.ZBAT_DataOffset =  sizeof(ZFileControlBlock);  // ZBAT data Pool is stored first just after ZFCB

    ZFCB.AllocatedBlocks = ZBAT.getAllocation();
    ZFCB.BlockExtentQuota = ZBAT.getQuota();
//    ZFCB.ZBAT_ExportSize = ZBAT._getExportAllocatedSize();
    ZFCB.ZBAT_ExportSize = ZBAT.getPoolExportSize();


    ZFCB.ZFBT_DataOffset = (zaddress_type)(ZFCB.ZBAT_DataOffset + ZFCB.ZBAT_ExportSize);// then ZFBT
//    ZFCB.ZFBT_ExportSize = ZFBT._getExportAllocatedSize();
    ZFCB.ZFBT_ExportSize = ZFBT.getPoolExportSize();

    ZFCB.ZDBT_DataOffset = (zaddress_type)(ZFCB.ZFBT_DataOffset + ZFCB.ZFBT_ExportSize); // then ZDBT
//    ZFCB.ZDBT_ExportSize = ZDBT._getExportAllocatedSize();
    ZFCB.ZDBT_ExportSize = ZDBT.getPoolExportSize();

//====================Export updated FCB and write it to file===============================

    ZDataBuffer wZDBFCB;
    ZFCB._exportAppend(wZDBFCB);

    ZBAT._exportAppendPool(wZDBFCB);
    ZFBT._exportAppendPool(wZDBFCB);
    ZDBT._exportAppendPool(wZDBFCB);

    if (lseek(HeaderFd,ZHeader.OffsetFCB,SEEK_SET) < 0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning at FileDescriptor address <%lld> of header file  <%s>",
                                 ZHeader.OffsetFCB,
                                 URIHeader.toString());
                return  (ZS_FILEPOSERR);
                }

    ZPMS.HFCBWrites ++;
    ssize_t wSWrite =write(HeaderFd,wZDBFCB.DataChar,wZDBFCB.Size); // write the whole set :ZFCB + block pools
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Error,
                             "Error while writing FCB + block pools to file header %s",
                             URIHeader.toString());
            return  (ZS_WRITEERROR);
            }


//=============================================================================================

    fdatasync(HeaderFd);

    HeaderAccessed |= ZHAC_FCB;

    return  (ZS_SUCCESS);
}// _writeFileControlBlock




ZStatus
ZRandomFile::_writeFCB(zaddress_type pOffsetFCB)
{
  // Data offsets are computed from Beginning of FCB that may vary is Header size and/or Reserved Data Size Change
  //
  ZDataBuffer wZDBZBATExport;
  ZDataBuffer wZDBZFBTExport;
  ZDataBuffer wZDBZDBTExport;
  ssize_t wSWrite;

  //    printf("ZRandomFile::_writeFileDescriptor\n");
  /*    if (Mode & ZRF_Exclusive)
    {
      if ((!pForceWrite)&&(HeaderAccessed & ZHAC_FCB))
      {  return  ZS_SUCCESS;}
    }
*/
  /*
 *  Export first the pools to ZDataBuffers
 *
 *  Update sizes and offsets within FCB
 *
 *  Export FCB
 *
 *  write FCB to file
 *
 *  write ZBAT , then ZFBT, then ZDBT to file
 *
 */
  ZBAT._exportAppendPool(wZDBZBATExport);
  ZFBT._exportAppendPool(wZDBZFBTExport);
  ZDBT._exportAppendPool(wZDBZDBTExport);

  // ==================Update FCB with offsets and sizes then export it===================================================

  ZFCB.ZBAT_DataOffset =  sizeof(ZFileControlBlock);  // ZBAT data Pool is stored first just after ZFCB

  ZFCB.AllocatedBlocks = ZBAT.getAllocation();
  ZFCB.BlockExtentQuota = ZBAT.getQuota();
  //    ZFCB.ZBAT_ExportSize = ZBAT._getExportAllocatedSize();
  ZFCB.ZBAT_ExportSize = wZDBZBATExport.Size;


  ZFCB.ZFBT_DataOffset = (zaddress_type)(ZFCB.ZBAT_DataOffset + ZFCB.ZBAT_ExportSize);// then ZFBT
  //    ZFCB.ZFBT_ExportSize = ZFBT._getExportAllocatedSize();
  ZFCB.ZFBT_ExportSize = wZDBZFBTExport.Size;

  ZFCB.ZDBT_DataOffset = (zaddress_type)(ZFCB.ZFBT_DataOffset + ZFCB.ZFBT_ExportSize); // then ZDBT
  //    ZFCB.ZDBT_ExportSize = ZDBT._getExportAllocatedSize();
  ZFCB.ZDBT_ExportSize = wZDBZDBTExport.Size;

  //====================Export updated FCB and write it to file===============================

  ZDataBuffer wZDBFCB;
  ZFCB._exportAppend(wZDBFCB);

  wZDBFCB.appendData(wZDBZBATExport);
  wZDBFCB.appendData(wZDBZFBTExport);
  wZDBFCB.appendData(wZDBZDBTExport);

  if (lseek(HeaderFd,pOffsetFCB,SEEK_SET) < 0)
  {
    ZException.getErrno(errno,
                      _GET_FUNCTION_NAME_,
                      ZS_FILEPOSERR,
                      Severity_Severe,
                      "Error positionning at FileDescriptor address <%lld> of header file  <%s>",
                      pOffsetFCB,
                      URIHeader.toString());
    return  (ZS_FILEPOSERR);
  }

  ZPMS.HFCBWrites ++;
  wSWrite =write(HeaderFd,wZDBFCB.DataChar,wZDBFCB.Size); // write the whole set :ZFCB + block pools
  if (wSWrite<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_WRITEERROR,
        Severity_Error,
        "Error while writing FCB + block pools to file header %s",
        URIHeader.toString());
    return  (ZS_WRITEERROR);
  }


  //=============================================================================================

  fdatasync(HeaderFd);

  HeaderAccessed |= ZHAC_FCB;

  return  (ZS_SUCCESS);
}



ZStatus
ZRandomFile::_writeReserved(zaddress_type pOffsetReserved)
{

  if (lseek(HeaderFd,pOffsetReserved,SEEK_SET) < 0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning at address <%lld> of header file  <%s>",
        pOffsetReserved,
        URIHeader.toString());
    return  (ZS_FILEPOSERR);
  }

  ZPMS.HReservedWrites ++;
  ssize_t wSWrite =write(HeaderFd,ZReserved.DataChar,ZReserved.Size); // write the whole set :ZFCB + block pools
  if (wSWrite<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_WRITEERROR,
        Severity_Error,
        "Error while writing Reserved content to file header %s",
        URIHeader.toString());
    return  (ZS_WRITEERROR);
  }


  //=============================================================================================

  fdatasync(HeaderFd);

  HeaderAccessed |= ZHAC_FCB;

  return  (ZS_SUCCESS);
}



ZStatus
ZRandomFile::_writeAllFileHeader()
{
  ZDataBuffer wZDB;
  if (__ZRFVERBOSE__)
    _DBGPRINT("ZRandomFile::_writeAllFileHeader \n") // debug

  std::cout.flush();

  ZHeader.OffsetReserved = sizeof(ZHeaderControlBlock_Export) ;
  ZHeader.SizeReserved = ZReserved.Size ;
  ZHeader.OffsetFCB =    ZHeader.OffsetReserved + ZReserved.Size ;
  // NB: ZHeader.Offset is set by clear() method

  size_t wRet=ZHeader._exportAppend(wZDB);

  /* reserved : As it is */

  wZDB.appendData(ZReserved);

  /* FCB and Pool */

  // ==================Update FCB with offsets and sizes then export it===================================================

  ZFCB.ZBAT_DataOffset =  sizeof(ZFCB_Export);  // ZBAT data Pool is stored first just after ZFCB

  ZFCB.AllocatedBlocks = ZBAT.getAllocation();
  ZFCB.BlockExtentQuota = ZBAT.getQuota();
  //    ZFCB.ZBAT_ExportSize = ZBAT._getExportAllocatedSize();
  ZFCB.ZBAT_ExportSize = ZBAT.getPoolExportSize();


  ZFCB.ZFBT_DataOffset = (zaddress_type)(ZFCB.ZBAT_DataOffset + ZFCB.ZBAT_ExportSize);// then ZFBT
  //    ZFCB.ZFBT_ExportSize = ZFBT._getExportAllocatedSize();
  ZFCB.ZFBT_ExportSize = ZFBT.getPoolExportSize();

  ZFCB.ZDBT_DataOffset = (zaddress_type)(ZFCB.ZFBT_DataOffset + ZFCB.ZFBT_ExportSize); // then ZDBT
  //    ZFCB.ZDBT_ExportSize = ZDBT._getExportAllocatedSize();
  ZFCB.ZDBT_ExportSize = ZDBT.getPoolExportSize();

  //====================Export updated FCB and write it to file===============================


  wRet=ZFCB._exportAppend(wZDB);

  ZBAT._exportAppendPool(wZDB);  /* first block access table */
  ZFBT._exportAppendPool(wZDB);  /* second free blocks table */
  ZDBT._exportAppendPool(wZDB);  /* third deleted blocks table */


  if (lseek(HeaderFd,0L,SEEK_SET) < 0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning at Header address <%lld> of header file  <%s>",
        0L,
        URIHeader.toString());
    return(ZS_FILEPOSERR);
  }
  ssize_t wSWrite =write(HeaderFd,wZDB.DataChar,wZDB.Size);
  if (wSWrite<0)
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_WRITEERROR,
          Severity_Error,
          "Error while writing all content to file header %s",
          URIHeader.toString());
      return  (ZS_WRITEERROR);
    }

  ZPMS.HFHWrites ++;
  ZPMS.HReservedWrites ++;
  ZPMS.HFCBWrites ++;

  HeaderAccessed |= ZHAC_HCB | ZHAC_FCB | ZHAC_RESERVED ;
  return ZS_SUCCESS;
}

ZStatus
ZRandomFile::_importAllFileHeader()
{
  ZDataBuffer wZDB;

  if (__ZRFVERBOSE__)
    _DBGPRINT("ZRandomFile::_importAllFileHeader \n") // debug


  /*
      Upon successful completion, lseek() returns the resulting offset
       location as measured in bytes from the beginning of the file.  On
       error, the value (off_t) -1 is returned and errno is set to
       indicate the error.
  */
  /* get the size of the file */
  off_t wOff = lseek(HeaderFd,(off_t)0L,SEEK_END);
  if (wOff<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Fatal,
        "lseek error positionning header file  at end of file file <%s>",
        URIHeader.toCChar()
        );
    return (ZS_FILEPOSERR);
  }

  wZDB.allocate((size_t)wOff);
  wOff = lseek(HeaderFd,(off_t)0L,SEEK_SET);
  if (wOff<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Fatal,
        "Error positionning header at beginning of file <%s>",
        URIHeader.toCChar()
        );
    return (ZS_FILEPOSERR);
  }

  ZPMS.HFHReads ++;

  //    ssize_t wSRead =read(HeaderFd,(char *)&ZHeader,sizeof(ZHeaderControlBlock));  //! read at first Header control block

  //    ssize_t wSRead =read(HeaderFd,(char *)&wZHeaderExport,sizeof(ZHeaderControlBlock_Export));  // read at first Header control block
  ssize_t wSRead =read(HeaderFd,wZDB.DataChar,wZDB.Size);  // load the whole file in wZDB
  if (wSRead<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Fatal,
        "Error loading  header file  <%s>",
        URIHeader.toCChar());
    return  (ZS_BADFILEHEADER);
  }

  if (wSRead < sizeof(ZHeaderControlBlock_Export))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Fatal,
        "Invalid/corrupted header file  <%s> size read %ld while expected at least %ld",
        URIHeader.toCChar(),
        wSRead, sizeof(ZHeaderControlBlock_Export));
    return  (ZS_BADFILEHEADER);
  }
  const unsigned char* wPtr=wZDB.Data;

  ZHeaderControlBlock_Export wZHCBe;
  wZHCBe.setFromPtr(wPtr);
  wZHCBe.deserialize();

  wZHCBe.toHCB(ZHeader);


  ZReserved.setData(wPtr,wZHCBe.SizeReserved);

  wPtr += wZHCBe.SizeReserved;


  ZFCB_Export wFCBe;
  wFCBe.setFromPtr(wPtr);
  wFCBe.deserialize();
  wFCBe.toFCB(ZFCB);


  ZBAT._importPool(wPtr);
  ZFBT._importPool(wPtr);
  ZDBT._importPool(wPtr);

  HeaderAccessed |= ZHAC_HCB | ZHAC_FCB | ZHAC_RESERVED ;
  return ZS_SUCCESS;
}



/**
 * @brief ZRandomFile::_writeFullFileHeader  Here only for logical purpose because it de facto an alias of _writeReservedHeader
 * @param[in] pDescriptor   File descriptor for wich Header block including Reserved block infra data will be written
 * @param[out] pForceWrite  if true : will write any time. if false and file is opened in exclusive mode : will not write
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorr
 */
#ifdef __DEPRECATED__
ZStatus
ZRandomFile::_writeFullFileHeader( bool pForceWrite)
{
ZStatus wSt;

    if (__ZRFVERBOSE__)
      _DBGPRINT("ZRandomFile::_writeFullFileHeader \n") // debug

    if ((wSt=_writeFileHeader(pForceWrite))!=ZS_SUCCESS)
                            { return  wSt;}
// if ZReserved contains something
    if (ZReserved.Size > 0)
      {
/*      if (Mode & ZRF_Exclusive)
        {
        if ((!pForceWrite)&&(HeaderAccessed & ZHAC_RESERVED))
            {  return  ZS_SUCCESS;}
        }
*/
        ZPMS.HReservedWrites += 1;
        ssize_t wSWrite =write(HeaderFd,ZReserved.DataChar,ZReserved.Size); // write Reserved infradata
        if (wSWrite<0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_WRITEERROR,
                                 Severity_Error,
                                 "Error while writing Reserved infra data to file header during creation. File %s",
                                 URIHeader.toString());
                return  (ZS_WRITEERROR);
                }
        HeaderAccessed |= ZHAC_RESERVED;
        fdatasync(HeaderFd); // better than flush : writes only data and not metadata to file
      } //if (ZReserved.Size > 0)


    return  (_writeFCB(pForceWrite));
}//_writeFullFileHeader
#endif // __DEPRECATED__

/**
 * @brief ZRandomFile::updateFileDescriptor  updates only the File descriptor zone in file header (without updating Reserved data)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::updateFileDescriptor(bool pForceWrite)
{
    return(_writeFCB(pForceWrite));
}

/**
 * @brief ZRandomFile::updateReservedHeader updates File Header with the give Reserved content given in pReserved, updates this content in memory, and updates the File descriptor in Header zone
 * @param pReserved
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::updateReservedBlock(const ZDataBuffer &pReserved,bool pForceWrite)
{
    setReservedContent(pReserved);
//    return(_writeAllHeaders(pForceWrite));
    return _writeAllFileHeader();
}


ZStatus
ZRandomFile::_loadHeaderFile(ZDataBuffer &pHeader)
{
  ZStatus wSt=ZS_SUCCESS;

  /* get the size of the file */
  off_t wOff = lseek(HeaderFd,(off_t)0L,SEEK_END);
  if (wOff<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Fatal,
        "lseek error positionning header file  at end of file file <%s>",
        URIHeader.toCChar()
        );
    return (ZS_FILEPOSERR);
  }

  pHeader.allocate((size_t)wOff);
  wOff = lseek(HeaderFd,(off_t)0L,SEEK_SET);
  if (wOff<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Fatal,
        "Error positionning header at beginning of file <%s>",
        URIHeader.toCChar()
        );
    return (ZS_FILEPOSERR);
  }

  ZPMS.HFHReads ++;

  //    ssize_t wSRead =read(HeaderFd,(char *)&ZHeader,sizeof(ZHeaderControlBlock));  //! read at first Header control block

  //    ssize_t wSRead =read(HeaderFd,(char *)&wZHeaderExport,sizeof(ZHeaderControlBlock_Export));  // read at first Header control block
  ssize_t wSRead =read(HeaderFd,pHeader.DataChar,pHeader.Size);  // load the whole file in wZDB
  if (wSRead<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Fatal,
        "Error loading  header file  <%s>",
        URIHeader.toCChar());
    return  (ZS_BADFILEHEADER);
  }

  if (wSRead < sizeof(ZHeaderControlBlock_Export))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Fatal,
        "Invalid/corrupted header file  <%s> size read %ld while expected at least %ld",
        URIHeader.toCChar(),
        wSRead, sizeof(ZHeaderControlBlock_Export));
    return  (ZS_BADFILEHEADER);
  }
  return  wSt;
}//_loadHeader

/**
 * @brief ZRandomFile::_getFileHeader gets/refrehes the file header block into memory.
 * This routine tests file header block validity
 * @param pDescriptor
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_getHeaderControlBlock(bool pForceRead)
{
ZStatus wSt=ZS_SUCCESS;
ZDataBuffer wZDB;

  if (Mode & ZRF_Exclusive)
     {
     if (!pForceRead)
             {return  ZS_SUCCESS;}
     }


  off_t wOff = lseek(HeaderFd,(off_t)0L,SEEK_SET);
  if (wOff<0)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_FILEPOSERR,
                         Severity_Fatal,
                         "Error positionning header at beginning of file <%s>",
                         URIHeader.toCChar()
                         );
        return (ZS_FILEPOSERR);
        }

    ZPMS.HFHReads ++;
    ZHeaderControlBlock_Export wZHeaderExport;
//    ssize_t wSRead =read(HeaderFd,(char *)&ZHeader,sizeof(ZHeaderControlBlock));  //! read at first Header control block

    ssize_t wSRead =read(HeaderFd,(char *)&wZHeaderExport,sizeof(ZHeaderControlBlock_Export));  // read at first Header control block
//    ssize_t wSRead =read(HeaderFd,wZDB.DataChar,wZDB.Size);  // load the whole file in wZDB
    if (wSRead<0)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_BADFILEHEADER,
                         Severity_Fatal,
                         "Error loading  header file  <%s>",
                         URIHeader.toCChar());
        return  (ZS_BADFILEHEADER);
        }

  if (wSRead < sizeof(ZHeaderControlBlock_Export))
        {
          ZException.setMessage(_GET_FUNCTION_NAME_,
              ZS_BADFILEHEADER,
              Severity_Fatal,
              "Invalid/corrupted header file  <%s> size read %ld while expected at least %ld",
              URIHeader.toCChar(),
              wSRead, sizeof(ZHeaderControlBlock_Export));
          return  (ZS_BADFILEHEADER);
        }

    // converts and controls about ZHeader integrity
  const unsigned char *wPtrIn=(unsigned char *)&wZHeaderExport;
  wSt=ZHeader._import(wPtrIn);

   return  wSt;
}//_getFileHeader
ZStatus
ZRandomFile::_getFileHeader_Export(ZHeaderControlBlock_Export* pHCB_Export)
{
ZStatus wSt=ZS_SUCCESS;
ZDataBuffer wZDB;


    off_t wOff = lseek(HeaderFd,(off_t)0L,SEEK_SET);
    if (wOff<0)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_FILEPOSERR,
                         Severity_Fatal,
                         "Error positionning header at offset <%lld> file <%s>",
                         0L,
                         URIHeader.toCChar()
                         );
        return (ZS_FILEPOSERR);
        }
    ZPMS.HFHReads ++;
    ssize_t wSRead =read(HeaderFd,(char *)pHCB_Export,sizeof(ZHeaderControlBlock_Export));  // read at first Header control block

    if (wSRead<0)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_BADFILEHEADER,
                         Severity_Fatal,
                         "Error reading  Header control block address <%lld> file  <%s>",
                         0L,
                         URIHeader.toCChar());
        return  (ZS_BADFILEHEADER);
        }

    // Controls about ZHeader integrity
    return  wSt;
}//_getFileHeader_Export
/**
 * @brief ZRandomFile::_getFullFileHeader gets/refreshes the whole file header content into memory_order
 * File header is composed of
 *  - header itself
 *  - reserved space
 *  - File Control Block himself composed of 3 pools :
 *      + Block access table
 *      + Free blocks pool
 *      + Deleted blocks pool
 *
 * @param pDescriptor
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_getFullFileHeader(bool pForceRead)
{
ZStatus wSt;
ZDataBuffer wHeaderContent;

    if (__ZRFVERBOSE__)
      _DBGPRINT("ZRandomFile::_getFullFileHeader \n") // debug

    if (getOpenMode() & ZRF_Exclusive)
           {
           if (!pForceRead)
                       {  return  ZS_SUCCESS;  }
           }

    if ((wSt=_loadHeaderFile(wHeaderContent))!=ZS_SUCCESS)
              {return   wSt;}



    const unsigned char* wPtr=wHeaderContent.Data;

    if ((wSt=ZHeader._import(wPtr))!=ZS_SUCCESS)
            {return   wSt;}

    HeaderAccessed |= ZHAC_HCB;

    if ((wSt=_getReservedHeader (pForceRead))!=ZS_SUCCESS)
                            {return   wSt;}
    return  _getFileControlBlock(pForceRead);

} // _getFullFileHeader
/**
 * @brief ZRandomFile::getReservedBlock gets/refreshes the file header reserved block in memory
 *  Reserved block is available in ZFileDescriptor::ZReserved data structure
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::getReservedBlock(bool pForceRead)
{
   return _getReservedHeader(pForceRead);
}
/**
 * @brief ZRandomFile::getReservedBlock gets the reserved block from file header and returns it within a ZDataBuffer
 * @param pReserved
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::getReservedBlock(ZDataBuffer& pReserved,bool pForceRead)
{
ZStatus wSt;
    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FILENOTOPEN,
                                Severity_Severe,
                                " File %s is not open while trying to get reserved header",
                                getURIContent().toCChar());
        return  ZS_FILENOTOPEN ;
        }
    pReserved.clearData();
    wSt=_getReservedHeader(pForceRead);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}
    pReserved = ZReserved ;
   return  wSt;
}

ZStatus
ZRandomFile::_getReservedHeader( bool pForceRead)
{
/*    if (getOpenMode() & ZRF_Exclusive)
           {
            if (!pForceRead  &&  (HeaderAccessed & ZHAC_RESERVED))
                    { return  ZS_SUCCESS;}
           }
*/
    off_t wOff = lseek(HeaderFd,(off_t)ZHeader.OffsetReserved,SEEK_SET);
    if (wOff<0)
          {
          ZException.getErrno(errno,
                           _GET_FUNCTION_NAME_,
                           ZS_FILEPOSERR,
                           Severity_Severe,
                           "Error positionning header at offset <%lld> file <%s>",
                           ZHeader.OffsetReserved,
                           URIHeader.toString()
                           );
          return  (ZS_FILEPOSERR);
          }
    ZPMS.HReservedReads += 1;
    ZReserved.allocateBZero(ZHeader.SizeReserved);
    ssize_t wSRead =read(HeaderFd,ZReserved.DataChar,ZHeader.SizeReserved);  //! then read reserved infradata
    if (wSRead<0)
          {
          ZException.getErrno(errno,
                           _GET_FUNCTION_NAME_,
                           ZS_BADFILERESERVED,
                           Severity_Error,
                           "Error reading  Header reserved infradata file  <%s>",
                           URIHeader.toString());
          return  (ZS_BADFILERESERVED);
          }
    HeaderAccessed|=ZHAC_RESERVED;
    return  ZS_SUCCESS;
} //_getReservedHeader

/**
 * @brief ZRandomFile::_getFileDescriptor returns the file descriptor with updated accurate infradata from file (header file)
 *
 *  File descriptor address (offset) MUST BE present and valid within ZFileHeader (OffsetFCB).
 *
 *  OffsetFCB allows to skip Reserved infradata stored at the beginning of file header (reputated more stable data than FCB data itself).
 *
 * @param pDescriptor
 * @param pOffsetFCB
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_getFileControlBlock ( bool pForceRead)
{
  if (getOpenMode() & ZRF_Exclusive)
           {
             if (!pForceRead  &&  (HeaderAccessed & ZHAC_FCB))
             { return  ZS_SUCCESS;}
           }
    off_t wOff = lseek(HeaderFd,ZHeader.OffsetFCB,SEEK_SET);
    if (wOff<0)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_FILEPOSERR,
                         Severity_Severe,
                         "Error positionning header at offset <%lld> file <%s>",
                         0L,
                         URIHeader.toString()
                         );
        return (ZS_FILEPOSERR);
        }

//    setupFCB();
    // get file control block
    //

    ZDataBuffer wZDB;
    wZDB.allocateBZero(sizeof(ZFCB_Export));
    ZPMS.HFCBReads ++;
    ssize_t wSRead =read(HeaderFd,wZDB.DataChar,sizeof(ZFCB_Export));  //! read at first Header control block
    if (wSRead<0)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_BADFILEHEADER,
                         Severity_Error,
                         "Error reading  Header control block address <%lld> file  <%s>",
                         0L,
                         URIHeader.toString());
        return  (ZS_BADFILEHEADER);
        }

    const unsigned char* wPtr=wZDB.Data;
    ZFCB._import(wPtr);/* beware wPtr is updated by _import */

    size_t wPoolSize = ZFCB.ZBAT_ExportSize + ZFCB.ZFBT_ExportSize + ZFCB.ZDBT_ExportSize;

    ZDataBuffer wBuffer;
    wBuffer.allocateBZero( wPoolSize+1);
/*
 * file is positioned to first byte after  ZFCB_Export
 *
   wOff = lseek(HeaderFd,(off_t)( ZFCB.ZBAT_DataOffset+ZHeader.OffsetFCB),SEEK_SET);
    if (wOff<0)
          {
          ZException.getErrno(errno,
                           _GET_FUNCTION_NAME_,
                           ZS_FILEPOSERR,
                           Severity_Severe,
                           "Error positionning file header at offset <%lld> file <%s>",
                           ZFCB.ZBAT_DataOffset,
                           URIHeader.toString()
                           );
          return  (ZS_FILEPOSERR);
          }
*/
    wSRead =read(HeaderFd,wBuffer.DataChar,wPoolSize);
//    if ((wSRead<64)||(wSRead !=ZFCB.ZBAT_ExportSize))
    if (wSRead != wPoolSize)
          {
          ZException.getErrno(errno,
                              _GET_FUNCTION_NAME_,
                              ZS_BADFILEHEADER,
                              Severity_Error,
                              "Error reading Block Access Table offset <%lld> file  <%s>. Pool size <%ld> has not been read in totality (<%ld> read).",
                              ZFCB.ZBAT_DataOffset,
                              URIHeader.toString(),
                              wPoolSize,
                              wSRead);
          return  (ZS_BADFILEHEADER);
          }


#ifdef __REPORT_POOLS__
          fprintf (stdout," import ZBAT pool \n");
#endif

    wPtr=wBuffer.Data;
    ZBAT._importPool(wPtr);  /* beware wPtr is updated by _importPool */

#ifdef __REPORT_POOLS__
  fprintf (stdout," import ZFBT pool \n");
#endif
    ZFBT._importPool(wPtr);/* beware wPtr is updated by _importPool */


#ifdef __REPORT_POOLS__
  fprintf (stdout," import ZDBT pool \n");
#endif
    ZDBT._importPool(wPtr);

    HeaderAccessed|=ZHAC_FCB;
    return  ZS_SUCCESS;
}// _getFileControlBlock



ZStatus
ZRandomFile::_updateFileControlBlock ( )
{
  off_t wOff = lseek(HeaderFd,ZHeader.OffsetFCB,SEEK_SET);
  if (wOff<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning header at offset <%lld> file <%s>",
        0L,
        URIHeader.toString()
        );
    return (ZS_FILEPOSERR);
  }

//  setupFCB();
  // get file control block
  //

  ZDataBuffer wZDB;
  wZDB.allocateBZero(sizeof(ZFCB_Export));
  ZPMS.HFCBReads ++;
  ssize_t wSRead =read(HeaderFd,wZDB.DataChar,sizeof(ZFCB_Export));  //! read at first Header control block
  if (wSRead<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Error,
        "Error reading  Header control block address <%lld> file  <%s>",
        0L,
        URIHeader.toString());
    return  (ZS_BADFILEHEADER);
  }
  const unsigned char* wPtr=wZDB.getData();
  ZFCB._import(wPtr);

  ZDataBuffer wBuffer;
  wBuffer.allocateBZero( ZFCB.ZBAT_ExportSize);
  wOff = lseek(HeaderFd,(off_t)( ZFCB.ZBAT_DataOffset+ZHeader.OffsetFCB),SEEK_SET);
  if (wOff<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning file header at offset <%lld> file <%s>",
        ZFCB.ZBAT_DataOffset,
        URIHeader.toString()
        );
    return  (ZS_FILEPOSERR);
  }
  wSRead =read(HeaderFd,wBuffer.DataChar,ZFCB.ZBAT_ExportSize);
  //    if ((wSRead<64)||(wSRead !=ZFCB.ZBAT_ExportSize))
  if (wSRead !=ZFCB.ZBAT_ExportSize)// no export header (64)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Error,
        "Error reading Block Access Table offset <%lld> file  <%s>. Pool size <%ld> has not been read in totality (<%ld> read).",
        ZFCB.ZBAT_DataOffset,
        URIHeader.toString(),
        ZFCB.ZBAT_ExportSize,
        wSRead);
    return  (ZS_BADFILEHEADER);
  }
  wPtr=wZDB.Data;
  ZBAT._importPool(wPtr);

  wBuffer.allocateBZero(ZFCB.ZFBT_ExportSize);
  wSRead =read(HeaderFd,wBuffer.DataChar,ZFCB.ZFBT_ExportSize);
  //    if ((wSRead<64)||(wSRead !=ZFCB.ZFBT_ExportSize))
  if (wSRead !=ZFCB.ZFBT_ExportSize) // no export header (64)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Error,
        "Error reading free block pool offset <%lld> file  <%s>. Pool size <%ld> has not been read in totality (<%ld> read).",
        ZFCB.ZFBT_DataOffset,
        URIHeader.toString(),
        ZFCB.ZFBT_ExportSize,
        wSRead);
    return  (ZS_BADFILEHEADER);
  }
  wPtr=wZDB.getData();
  ZFBT._importPool(wPtr);

  wBuffer.allocateBZero(ZFCB.ZDBT_ExportSize);
  wSRead =read(HeaderFd,wBuffer.DataChar,ZFCB.ZDBT_ExportSize);
  //    if ((wSRead<64)||(wSRead !=ZFCB.ZDBT_ExportSize))
  if (wSRead !=ZFCB.ZDBT_ExportSize) // no export header (64)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Error,
        "Error reading deleted block pool offset <%lld> file  <%s>. Pool size <%ld> has not been read in totality (<%ld> read).",
        ZFCB.ZDBT_DataOffset,
        URIHeader.toString(),
        ZFCB.ZDBT_ExportSize,
        wSRead);
    return  (ZS_BADFILEHEADER);
  }
  wPtr=wZDB.getData();
  ZDBT._importPool(wPtr);
  return  ZS_SUCCESS;
}// _getFileDescriptor

/**
 * @brief ZRandomFile::_getBlockHeader gets from file the block header at address pAddress and returns it in pBlockHeader
 * @param pDescriptor
 * @param pAddress
 * @param pBlockHeader
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_getBlockHeader(  zaddress_type pAddress, ZBlockHeader &pBlockHeader)
{
ssize_t wSRead;
ZBlockHeader_Export wBlockHeadExp;
// get position to block : block header is the first block element

    off_t wOff = lseek(ContentFd,(off_t)( pAddress),SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning file  at logical address <%lld> file <%s>",
                                 pAddress,
                                 URIContent.toString()
                                 );
                return  (ZS_FILEPOSERR);
                }

    setPhysicalPosition((zaddress_type)wOff);

    ZPMS.CBHReads ++;

    if ((wSRead=read(ContentFd,&wBlockHeadExp,sizeof(ZBlockHeader_Export)))<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_READERROR,
                             Severity_Error,
                             "Error reading block header :  address <%lld> for file <%s>\n",
                             pAddress,
                             URIContent.toString());
            return  (ZS_READERROR);
            }
    ZPMS.CBHReadBytesSize += wSRead ;

    ZBlockHeader::_importConvert(pBlockHeader,&wBlockHeadExp);

    incrementPosition(wSRead);

    if (wSRead!=sizeof(ZBlockHeader_Export))
            {
            ZException.setMessage(
                             _GET_FUNCTION_NAME_,
                             ZS_READPARTIAL,
                             Severity_Error,
                             "Error reading block header :  address <%lld> for file <%s>\n"
                             "Block header has not been entirely read and has been truncated.",
                             pAddress,
                             URIContent.toString());
            return  (ZS_READPARTIAL);
            }

    return  (ZS_SUCCESS);
} // _getBlockHeader


//!   @endcond


/**
 *
 *  @brief ZRandomFile::zopen opens the file in the mode given by pMode
 *      - loads ZFileControlBlock with appropriate information concerning ZRandomFile.
 *      - clears any block pointers. So, a sequential read (zgetnext) will read the first logical block record.
 * @note with this version of zopen method, it is supposed that file path has already been set with setPath() method.
 * During open phase, file header is loaded and set-up to memory.
 *
 * @par open actions
 *   - sets up ZFileDescriptor
 *      + associates ZFCB, ZBAT and ZFBT to ZFileDescriptor
 *      + clears block pointers
 *      + clears CurrentBlockHeader
 *   - loads ZFileHeader
 *   - loads ZReserved block
 *   - loads ZFileControlBlock
 *
 *   - controls file lock state
*
 * @param[in] pMode a zmode to define the kind of accesses to the file
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::zopen(const zmode_type pMode)
{
    return  _ZRFopen(pMode,ZHeader.FileType);

}//zopen
/**
 * @brief ZRandomFile::zopen open the file which path is given by pFilename in mode pMode
 * @param[in] pFilename an uriString containing the path for main content ZRandomFile. Header file name is deduced.
 * @param[in] pMode open mode mask @see zmode
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zopen(const uriString &pFilename, const zmode_type pMode)
{
ZStatus wSt;


    if ((wSt=setPath(pFilename))!=ZS_SUCCESS)
                                {    return  wSt;}


    return  (zopen(pMode));
}//zopen

/** @cond Development */
/**
 * @brief _cleanDeletedBlocks Deletes blocks in free blocks pool that are included in the area covered by block pBD.
 *  @note When a free block is allocated to ZBAT all deleted blocks within it are no more recoverable.
 *  This means that their references are destroyed from ZDBT table.
 * @param[In,Out]   pDescriptor File descriptor. File descriptor is modified (ZDBT) by this operation
 * @param[in]       pBD   block containing the deleted blocks to remove
 */
void
ZRandomFile::_cleanDeletedBlocks(ZBlockDescriptor &pBD)
{
    zaddress_type wAddMin = pBD.Address;
    zaddress_type wAddMax = pBD.Address+pBD.BlockSize;

    long wi = 0;
    while (wi<ZDBT.size()) {
      if ((ZDBT.Tab[wi].Address >= wAddMin)&&(ZDBT.Tab[wi].Address < wAddMax)) {
        if (__ZRFVERBOSE__)
          _DBGPRINT( " Removing deleted block index %ld from recoverable block pool address %lld is included in field of allocated block address %lld size %lld\n",
                     wi,
                     ZDBT.Tab[wi].Address,
                     pBD.Address,
                     pBD.BlockSize
                     )
        ZDBT.erase(wi);
        continue;
      }
      wi++;
    } // while
    return;
} // _cleanDeletedBlocks

/**
 * @brief ZRandomFile::_allocateFreeBlock
 * --------------------------------------------------------------
 *  block must previously been allocated by getFreeBlock routine.
 * --------------------------------------------------------------
 *  Takes freeblock pointed by pZFBTIndex index within free blocks table
 *  Creates an entry into Block Access Table ZBAT with address given by Free Table and size given by pSize
 *  if all space is used for freeblock : suppress Free Block Table entry
 *  if not Modify Free Block Table entry : adjust free space with remaining space , adjust free block address
 *
 *  block entry in ZBAT is marked as ZBS_Allocated (allocated by not used)
 *  new block entry in ZBAT is created at pZBATIndex rank or by push (add last) if pZBATIndex is -1
 *
 *  returns an Index corresponding to the newly created entry of Block Access Table (ZBAT)
 *  or -1 if an error occurred (invalid pZBTIndex or invalid pSize value).
 *  In this case, ZRException is set with explanaitions information
 *
 *
 * @param[in] pZFBTRank     Free block rank to be moved to ZBAT pool
 * @param[in] pSize         Total size of the block to be allocated (user data + block header )
 * @param[in] pFlag         ZBATAllocate_type :  ZBATA_Create created in ZBAT
 * @param[in] pZBATIndex    ZBAT rank to insert the allocated block. if -1, the new block is pushed on ZBAT
 * @return  rank within ZBAT (ZBlockAccessTable) of the allocated block : this is the relative position within RandomFile.
 */

long
ZRandomFile::_allocateFreeBlock (zrank_type pZFBTRank,
                                 zsize_type pSize,
                                 int        pFlag,  /* ZBATA_xxxx */
                                 long pZBATRank)

{
zrank_type wZBATRank=pZBATRank;

    ZPMS.FreeMatches ++;

    if (__ZRFVERBOSE__)
        _DBGPRINT(" allocateFreeBlock ; allocate size is %ld \n",pSize) // debug

    if ((pZFBTRank<0 )||(pZFBTRank > ZFBT.lastIdx()))
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                           ZS_INVOP,
                                           Severity_Severe,
                                           "Invalid index for Free Block Table (ZFBT) %ld ",pZFBTRank);
                    return  -1;
                    }
    if (pSize > ZFBT.Tab[pZFBTRank].BlockSize)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                           ZS_INVOP,
                                           Severity_Severe,
                                           "Invalid Free Block to be allocated : size %lld while requested size is %lld",
                                           ZFBT.Tab[pZFBTRank].BlockSize,
                                           pSize);
                    return  -1;
                    }
    ZBlockDescriptor wBD;

    wBD.clear();
    wBD             = ZFBT.Tab[pZFBTRank];

    wBD.BlockSize   = pSize;
    wBD.State       = ZBS_Allocated;    // block is allocated BUT NOT USED
//    wBD.Lock        = ZLock_All ;       // Block locked for all operations
//    wBD.LockReason  = ZReason_Create;   // Block locked for creation
    wBD.Pid         = Pid ;


// create / update ZBAT entry with free block data
    ZStatus wSt;
    switch (pFlag) {
    case ZBATA_Create: /* create */
      ZBAT.push(wBD);
      wZBATRank = ZBAT.lastIdx();
      break;
    case ZBATA_Insert:
      ZBAT.insert(wBD,pZBATRank); // ZBlockHeader is not modified on file and ZBAT is
      break;
    case ZBATA_Replace:/* replace involves to  send existing block to delete pool before setting ZBAT rank with allocated block data */
      /* send former block to deleted pool */
      ZBAT[pZBATRank].State |= ZBS_Deleted;
      /* free block without removing its position in ZBAT pool (pReplace set to true) */
      if ((wSt=_freeBlock_Commit(pZBATRank,true))!=ZS_SUCCESS)
        return wSt;
      /* replace block data at pZBATRank with allocated block data */
      ZBAT[pZBATRank]=wBD;
      ZBAT[pZBATRank].State=ZBS_Used;
      break;
    }

    zsize_type wRemainingSize = ZFBT.Tab[pZFBTRank].BlockSize - pSize;


    // debug
    //
    if(__ZRFVERBOSE__)
        _DBGPRINT(
                 " Allocating free block index %ld size %ld at address %ld for requested size %ld remaining size %ld \n",
                 pZFBTRank,
                 ZFBT.Tab[pZFBTRank].BlockSize,
                 ZFBT.Tab[pZFBTRank].Address,
                 pSize,
                 wRemainingSize
                 );




    if (wRemainingSize==0)                      // sizes are equal : reuse this Free Table Entry as it is and transfer it to ZBAT
            {
              ZFBT.erase(pZFBTRank);      // just suppress ZFBT entry : it is transferred to ZBAT

              _cleanDeletedBlocks(wBD);     // remove deleted block included in free block from Recovery Pool

              return  (wZBATRank);              // return ZBAT entry index created or reused
            } // wS==0 : sizes are equal


// otherwise :
//        block must be divided :
//             - the new one with appropriate required size : new entry created in ZFBT
//
//             if remaining size is greater than sizeof of ZBlockHeader_Export :
//                 create a ZBlockHeader_Export on file with appropriate information
//                 Modify ZFBT value to take this into account (new offset, new size)
//
//             if not : leave remaining space as a hole
//

    ZBlockDescriptor wRemainingFreeBlock;

    wRemainingFreeBlock.clear();

    wRemainingFreeBlock = ZFBT.Tab[pZFBTRank];
    wRemainingFreeBlock.Address += pSize;               // shift offset to new block header
    wRemainingFreeBlock.BlockSize -= pSize;             // subtract used size
    wRemainingFreeBlock.State = ZBS_Free ;


    ZFBT.erase(pZFBTRank);    //!

     if (wRemainingSize > sizeof(ZBlockHeader_Export)) // if and only if remaining space can contain more than a block header (otherwise leave as hole)
            {
            // must create a new free block entry in memory(ZBlockHeader) and on file (ZBlockHeader_Export)

            ZFBT.push(wRemainingFreeBlock) ;

//      need to write block header here at new address : export is made within _writeBlockHeader

             if ( _writeBlockHeader((ZBlockHeader&)wRemainingFreeBlock,wRemainingFreeBlock.Address)!=ZS_SUCCESS)
                                                         {  return  -1;}

            if(__ZRFVERBOSE__)
                 _DBGPRINT(
                          " Creating new free block size %ld at address %ld  index in Pool %ld \n",
                          wRemainingFreeBlock.BlockSize,
                          wRemainingFreeBlock.Address,
                          ZFBT.lastIdx()
                          );
            } // wRemainingSize > sizeof(ZBlockHeader_Export)
     // debug
     //
     else
     {
     if(__ZRFVERBOSE__)
             _DBGPRINT(
                      " Remaining space is left as a hole : size %ld is less than size of a block header\n",
                      wRemainingSize
                      );
     }

    // end debug

    _cleanDeletedBlocks(wBD);     //! remove deleted block included in free block from Recovery Pool

// need to update file control block too : take account of modified pools
 //    if ( _writeFileDescriptor(pDescriptor)!=ZS_SUCCESS)   // File descriptor will be written once commit has been done
 //                                                                           return -1;
    return  (wZBATRank);  //! returns the newly created or reused ZBAT entry allocated but not used
}//_allocateFreeBlock




/**
 * @brief ZRandomFile::_seek Physical positionning of system file pointer to a physical address
 *      position the file pointer to pAddress.
 *      ZFileDescriptor::PhysicalPosition and ZFileDescriptor::LogicalPosition are updated according the new position.
 *
 *
 * @param[in] pDescriptor   ZFileDescriptor of the file to read from
 * @param[in] pAddress      Address within file to position next physical read
 * @param[in] pModule       calling module reference
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_seek(zaddress_type pAddress)
{
    off_t wOff = lseek(ContentFd,(off_t)( pAddress+ZFCB.StartOfData),SEEK_SET);
    if (wOff<0) {
        ZException.getErrno(errno,
                        _GET_FUNCTION_NAME_,
                        ZS_FILEPOSERR,
                        Severity_Severe,
                        "Error while positionning file <%s> at address <%lld>",
                        URIContent.toString(),
                        pAddress);
        return  (ZS_FILEPOSERR);
    }
    setPhysicalPosition((zaddress_type)wOff);
    return  ZS_SUCCESS;
}//_seek

/**
  @brief ZRandomFile::_read
    reads at current physical position a file given by it ZFileDescriptor
    reads at maximum pSize bytes from file.

The number of bytes effectively read is returned in pSizeRead.
If End of file is reached, ZStatus is positionned as :
 - ZS_READPARTIAL : reads some bytes and returns it before reaching End of File.
 - ZS_EOF : nothing has been read

    ZFileDescriptor::PhysicalPosition and ZFileDescriptor::LogicalPosition are updated to their actual values AFTER read operation.
 *
 * @param[in] pDescriptor   ZFileDescriptor of the file to read from
 * @param[out] pBuffer      a pointer to a memory allocated
 * @param[in] pSize        Size to read from file.
 * @param[Out] pSizeRead   Size effectively read from file
 * @param[in] pZPMSType     gives the type of PMS counters to update
 * @param[in] pModule   a C string describing the calling module
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *  - ZS_SUCCESS if success
 *  - ZS_EOF        if end of file has been reached
 *  - ZS_READPARTIAL if returned buffer has not the requested size
 *  - ZS_READERROR if an error occurred
 */
ZStatus
ZRandomFile::_read(void* pBuffer,
                   const size_t pSize,
                   ssize_t &pSizeRead,
                   ZPMSCounter_type pZPMSType)
{


//    ZPMS.UserReads += 1;

    pSizeRead= read(ContentFd,pBuffer,pSize);

//    ZPMS.UserReadSize += pSizeRead;
    if (pSizeRead<0)
            {
            ZPMS.PMSCounterRead(pZPMSType,0);
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_READERROR,
                             Severity_Severe,
                             "Error while reading file <%s>",
                             URIContent.toString());
            return  (ZS_READERROR);
            }
    ZPMS.PMSCounterRead(pZPMSType,pSizeRead);
    setPhysicalPosition((zaddress_type)lseek(ContentFd,0L,SEEK_CUR));// get current address
//    PhysicalPosition = lseek(ContentFd,0L,SEEK_CUR);  //! get current address
//    LogicalPosition = PhysicalPosition - ZFCB.StartOfData ;

    if (pSizeRead==0)
            {
            return  ZS_EOF;
            }
    if (pSizeRead<pSize)
                {return  ZS_READPARTIAL;} // not an error : expected size is not returned. EOF may have been found

return  ZS_SUCCESS;
}//_read

/**
 * @brief ZRandomFile::_read
       Reads at the current physical position a file given by its pDescriptor ZFileDescriptor in pBuffer for reading at maximum pSizeRead bytes.
       The number of bytes effectively read is returned in pBuffer and the allocation of ZDataBuffer is adjusted accordingly to match the bytes effectively read.
  No mention of address is made : data is read from file from the current physical position within the file.

  In case of partial read, the number of byte is loaded into ZDataBuffer and its allocation is adjusted to size of read data.
  If End of file is reached, ZStatus is positionned as :
           ZS_READPARTIAL : reads some bytes and returns it before reaching End of File.
           ZS_EOF : nothing has been read. ZDataBuffer REMAINS UNCHANGED (same allocation, same initial data it had before the call)

  Current files addresses are updated in File Descriptor :
  ZFileDescriptor::PhysicalPosition and ZFileDescriptor::LogicalPosition are updated to their actual values AFTER read operation.

 * @param[in] pDescriptor   ZFileDescriptor of the file to read from
 * @param[out] pBuffer       A ZDataBuffer that will contain the data read. Its size is the data effectively read on file
 * @param[in] pSizeToRead   Contains the size expected to be read in input- Size effectively read is given by ZBufferData size
 * @param[in] pZPMSType     gives the type of PMS counters to update
 * @param[in] pModule   a C string describing the calling module
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *
 */

ZStatus
ZRandomFile::_read(ZDataBuffer& pBuffer,
                   const ssize_t pSizeToRead,
                   ZPMSCounter_type pZPMSType)
{

ssize_t wSRead=0;

//    ZPMS.UserReads += 1;

    pBuffer.allocate(pSizeToRead);                                    // allocate the maximum size to read from file
    wSRead= read(ContentFd,pBuffer.Data,pBuffer.Size);


    if (wSRead<0)
            {
            ZPMS.PMSCounterRead(pZPMSType,0);
            ZException.getErrno(errno,
                                _GET_FUNCTION_NAME_,
                                ZS_READERROR,
                                Severity_Severe,
                                "Error while reading file <%s>",
                                URIContent.toString());
            return(ZS_READERROR);
            }

//    ZPMS.UserReadSize += wSRead;
    ZPMS.PMSCounterRead(pZPMSType,wSRead);
    if (wSRead==0)
            {
            ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_EOF,
                         Severity_Severe,
                         "Read size is zero length. Either EOF has been encountered or User content record is empty.On file <%s>",
                         URIContent.toString());
//             pBuffer.clear();  // pBuffer remains unchanged
            return ZS_EOF;
            }
    if (wSRead<pSizeToRead)
            {
            pBuffer.allocate(wSRead); // reallocate appropriate size
            return ZS_READPARTIAL; //! not an error : expected size is not returned. EOF may have been found
            }
    incrementPosition( wSRead );
return ZS_SUCCESS;
}//_read

/**
 * @brief ZRandomFile::_readAt
 *      reads a file at offset pAddress given by its pDescriptor -File Descriptor- in pBuffer at maximum pSize bytes.
 *      The number of bytes effectively read is returned in pSizeRead.
 *
 * If End of file is reached, ZStatus is positionned as :
 *
 *  - ZS_READPARTIAL : reads some bytes and returns it before reaching End of File.
 *  - ZS_EOF : nothing has been read
 *
 * ZFileDescriptor::PhysicalPosition and ZFileDescriptor::LogicalPosition are updated to their actual values AFTER read operation.
 *
 *
 *
 * @param[in] pDescriptor   ZFileDescriptor of the file to read from
 * @param[out] pBuffer      a pointer to a memory allocated
 * @param[in] pSize        Size to read from file.
 * @param[Out] pSizeRead   Size effectively read from file
 * @param[in] pAddress      Address within file to read data
 * @param[in] pZPMSType     gives the type of PMS counters to update
 * @param[in] pModule   a C string describing the calling module
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *  see @ref ZRandomFile::_read() for returned status
 */

ZStatus
ZRandomFile::_readAt(void *pBuffer,
                     size_t pSize,
                     ssize_t &pSizeRead,
                     zaddress_type pAddress,
                     ZPMSCounter_type pZPMSType)
{


    ZStatus wSt = _seek(pAddress);
    if (wSt!=ZS_SUCCESS)
                {   return  wSt;   }
    return  _read(pBuffer,pSize,pSizeRead,pZPMSType);
}//_readAt


/**
 * @brief ZRandomFile::_readAt Reads raw data at address pAddress and returns a ZDataBuffer containing this data
 *
 *   ZDataBuffer will contain the read data.
 *   ZDataBuffer allocation with necessary space is done by routine.
 *   ZDataBuffer allocation defines the quantity of bytes effectively read.
 *   Read is done with at maximum pSizeToRead bytes.
 *
 *
 * @param[in] pDescriptor   ZFileDescriptor of the file to read from
 * @param[out] pBuffer      ZDataBuffer containing the raw data read at given address (pAddress).
 *      Its allocated size in return gives how much bytes have been effectively read.
 * @param[in] pSizeToRead   Size to read from file.
 * @param[in] pAddress      Address within file to read data
 * @param[in] pZPMSType     gives the type of PMS counters to update
 * @param[in] pModule   a C string describing the calling module
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */


ZStatus
ZRandomFile::_readAt(ZDataBuffer& pBuffer,
                     ssize_t &pSizeToRead,
                     zaddress_type pAddress,
                     ZPMSCounter_type pZPMSType)
{
    ZStatus wSt = _seek(pAddress);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return _read(pBuffer,pSizeToRead,pZPMSType);
}//_readAt

/**
 * @brief ZRandomFile::_readBlockAt
        Reads a whole block from file given by pDescriptor :
 - BlockHeader
 - then user content

 returns the whole block with a ZBlock structure
 @note PMS counters are incremented accordingly

 * @param[in] pDescriptor   ZFileDescriptor of the file to read from
 * @param[out] pBlock        ZBlock to be returned
 * @param[in] pAddress      Physical address to read the block
 * @param[in] pModule   a C string describing the calling module
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *  - ZS_SUCCESS in case of success
 *  - ZS_INVBLOCKADDR Block does not correspond to effective begining of a well formed data block
 *  - Other low level errors : see @ref ZRandomFile::_read() for possible status
 */
ZStatus
ZRandomFile::_readBlockAt(ZBlock &pBlock,
                     const zaddress_type pAddress) {

ZStatus wSt;
ssize_t wSizeRead;
ssize_t wUserRecordSize;

ZBlockHeader_Export wBlockHeadExp;

//    ZPMS.CBHReads ++;

    wSt= _readAt(&wBlockHeadExp,sizeof(ZBlockHeader_Export),wSizeRead,pAddress,ZPMS_BlockHeader);
    if (wSt!=ZS_SUCCESS)
                {   return  wSt;   }

    wSt=ZBlockHeader::_importConvert(pBlock,&wBlockHeadExp);

    if ((pBlock.BlockSize<1)||(pBlock.BlockSize>UINT_MAX)) {
      ZException.setMessage("ZRandomFile::_readBlockAt",ZS_INVSIZE,Severity_Error,
          "ZRandomFile::_readBlockAt-E-OUTBOUND invalid read block size %ld address %ld for file <%s> \n",
          pBlock.BlockSize,pAddress,URIContent.toCChar());
      fprintf(stderr,"ZRandomFile::_readBlockAt-E-OUTBOUND invalid read block size %ld address %ld for file <%s>.\n",
          pBlock.BlockSize,pAddress,URIContent.toCChar());
      return ZS_INVSIZE;
    }

// Test wether given address correspond to effective beginning of a block with correct identification (Data block)

    if (wSt!=ZS_SUCCESS)
                    {   return  wSt;   }


//    ZPMS.CBHReadBytesSize +=wSizeRead ;

    wUserRecordSize = pBlock.BlockSize - sizeof(ZBlockHeader_Export);
    pBlock.allocate(wUserRecordSize);
    return  _read(pBlock.Content,wUserRecordSize,ZPMS_User); // PMS counter are NOT updated within _read for user content values

}//_readBlockAt

/**
 * @brief ZRandomFile::searchBlockRankByAddress searches an active block (in ZBlockAccessTable) by its address and returns its rank in pool
 * @param[in] pDescriptor File descriptor
 * @param[in] pAddress starting block Address of the record to find the rank
 * @return  rank of the record whose block starts at address pAddress
 *      or -1 if given address is not the start of an active block in ZBAT pool
 */
zrank_type
ZRandomFile::searchBlockRankByAddress(zaddress_type pAddress)
{
    for (long wi = 0; wi< ZBAT.size();wi++)
                {
                if (ZBAT.Tab[wi].Address==pAddress)
                                {
                                return wi;
                                }
                }
    return -1;  // not found
} //searchBlockByAddress



/**
 * @brief ZRandomFile::_freeBlock_Prepare      Prepares to delete an entry of ZBlockAccessTable pool.
 *  Entry in ZBAT is marked ZBS_BeingDeleted, and cannot be accessed any more by others.
 *
 *  If HighwaterMarking option is set : the file block region will be set to zero during commit phase.
 *
 * @param[in] pDescriptor File descriptor
 * @param[in] pIndex ZBAT index to be freed (record rank)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

void
ZRandomFile::_freeBlock_Prepare(zrank_type pRank )
{
    ZBAT.Tab[pRank].State = ZBS_Deleted;
    ZBAT.Tab[pRank].Pid   = Pid;
    return;
}

/**
 * @brief ZRandomFile::_freeBlock_Rollback Invalidate freeBlock operation and sets the ZBAT block again to ZBS_Used, so that it can be accessed normally.
 * @param[in] pDescriptor File descriptor
 * @param[in] pIndex ZBAT index to be freed (record rank)
 */
void
ZRandomFile::_freeBlock_Rollback (zrank_type pRank )
{
    ZFBT.Tab[pRank].State = ZBS_Used ;
    ZBAT.Tab[pRank].Pid   = 0L;
    return;
}

/**
 * @brief ZRandomFile::_freeBlock_Commit Deletes (Frees) definitively an entry of ZBlockAccessTable pool
 *
 * Frees an entry of ZBlockAccessTable pool : Deletes it from ZBAT.
 * Create a corresponding entry wihin ZFreeBlockTable pool.
 *
 * If HighwaterMarking option is set : set the file block region to zero.
 *
 * @param[in] pIndex ZBAT index to be freed (record rank)
 * @param[in] pReplace if set to false (default) : ZBAT entry is deleted. if set to true ZBAT entry is kept available.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_freeBlock_Commit(zrank_type pRank, bool pKeepZBATElement) {
ZStatus wSt;
ZBlockDescriptor wBS;

    wBS   = ZBAT.Tab[pRank];

    if (ZFCB.GrabFreeSpace) {
      if ((wSt=_grabFreeSpaceLogical(pRank,wBS))!=ZS_SUCCESS)
        return wSt;
    }

    /* mark block on disk as free */
    wBS.State       =  ZBS_Deleted;
    wSt=_writeBlockHeader((ZBlockHeader&)wBS,wBS.Address);
    if (wSt!=ZS_SUCCESS)
                return wSt;


    if (ZFCB.HighwaterMarking)      // if highwatermarking then set user content file region to binary zero
            {
            wSt=_highwaterMark_Block(wBS.BlockSize-sizeof(ZBlockHeader_Export));
            if (wSt!=ZS_SUCCESS)
                        {
                        return wSt;
                        }

            } // if Highwater

    ZFBT.push(wBS);     // create entry into free blocks pool
    ZDBT.push(wBS);     // create entry into deleted pool

    ZFCB.UsedSize -= wBS.BlockSize ;

    if (!pKeepZBATElement) {
      ZBAT.erase(pRank);  // remove entry from ZBAT
      return(_writeFCB(true));
    }
    return ZS_SUCCESS;
}//_freeBlock_commit

/**
 * @brief ZRandomFile::_freeBlock former freeblock routine
 * @param[in] pDescriptor File descriptor
 * @param[in] pRank ZBAT rank to be freed (record rank)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_freeBlock(zrank_type pRank)
{

ZStatus wSt;
ZBlockDescriptor wBS;
ZBlockDescriptor_Export wBS_Exp;

    wBS   = ZBAT.Tab[pRank];
//    wBS.State       = ZBS_BeingDeleted;
    wBS.State       = ZBS_Deleted;
//    wBS.Lock        = ZLock_All;
//    wBS.LockReason  = ZReason_Delete ;
    wBS.Pid         = Pid;      // warning get the current pid

    if (ZFCB.GrabFreeSpace)
            {
            if ((wSt=_grabFreeSpaceLogical(pRank,wBS))!=ZS_SUCCESS)
                { return  wSt;}

            }


//    ZFCB.ZBATNbEntries --;

    ZBlock wBlock;
    wBlock.clear();

    wBlock.allocate(wBS.BlockSize-sizeof(ZBlockHeader_Export));
    wBlock.resetData();
    wBlock.State=ZBS_Deleted;
    wBlock.BlockSize = wBS.BlockSize;
//    wBlock.BlockId =ZBID_Nothing;

    wSt=_writeBlockHeader((ZBlockHeader&)wBlock,wBS.Address);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}


    if (ZFCB.HighwaterMarking) {     // if highwatermarking then set the file region to zero
            ZPMS.HighWaterWrites ++;

            ssize_t wSWrite=write (ContentFd,wBlock.DataChar(), wBlock.DataSize());
            if (wSWrite<0)
                        {
                        ZException.getErrno(errno,
                                         _GET_FUNCTION_NAME_,
                                         ZS_WRITEERROR,
                                         Severity_Severe,
                                         "Severe Error while high water marking freed block for file %s at address %lld",
                                         URIContent.toString(),
                                         getPhysicalPosition());
                        return  (ZS_WRITEERROR);
                        }

            ZPMS.HighWaterBytesSize += wSWrite;
            if (wSWrite!=wBlock.DataSize())
                        {
                        ZException.getErrno(errno,
                                         _GET_FUNCTION_NAME_,
                                         ZS_WRITEPARTIAL,
                                         Severity_Error,
                                         "Error: data partially written to file while high water marking freed block for file %s at address %lld requested %ld written %ld",
                                         URIContent.toString(),
                                         getPhysicalPosition(),
                                         wBlock.DataSize(),
                                         wSWrite);
                        return  (ZS_WRITEPARTIAL);
                        }
 //           incrementPosition( wSWrite );

            fdatasync(ContentFd); // better than flush : writes only data and not metadata to file
    } // if Highwater

    wBS.State = ZBS_Free;
    ZFBT.push(wBS);      // create entry into ZFBT for freed Block
    ZBAT.erase(pRank);  // remove entry from ZBAT

    ZFCB.UsedSize -= wBS.BlockSize ;

// current index and positions are invalid after removing of the block from ZBAT

    CurrentRank = -1;
    LogicalPosition = -1;
    PhysicalPosition = -1;

    return (_writeFCB(true));
}//_freeBlock


/**
 * @brief ZRandomFile::_grabFreeSpace collects free space adjacent to block given by its rank pIndex
 *
 * For a block to be freed given by pIndex pointing to a block in pZBAT,
 * _grabFreeSpace identifies and collects adjacent free space and/or holes,
 * and aggregate them to block to be freed.
 *
 * Free blocks and holes must be physically adjacent in file (and not in ZFBT pool table) : search is made in physical file.
 *
 * _grabFreeSpace() removes concerned freed blocks in Free blocks pool (ZFBT).
 *
 * a ZBlockMin_struct is returned with the references of the new created block from agregate space.
 *
 *
 * @param[in] pDescriptor File descriptor
 * @param[in] pIndex ZBAT index to be freed (record rank)
 * @param[out] pBS   free block to be returned as a ZBlockDescriptor with all space and aggregated free blocks found
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_grabFreeSpacePhysical(zrank_type pRank,
                                    ZBlockDescriptor &pBS)
{

ZStatus wSt;
zaddress_type wStartFreeBlockAddress ;

zsize_type wSizeFreeSpace;

    pBS.Address     = ZBAT.Tab[pRank].Address;
    pBS.BlockSize   = ZBAT.Tab[pRank].BlockSize;

    wStartFreeBlockAddress = ZBAT.Tab[pRank].Address;
    wSizeFreeSpace =  ZBAT.Tab[pRank].BlockSize;

    //-----------Gather free blocks and grab holes---------------
    // search for adjacent free blocks and holes that might be agregated to the one to be freed

    // On the file :
    //
    //   if beginning of next block (start of this block + blocksize) is not a Start block sign : it is a 'Hole' : get this space to be aggregated.
    //             find next start sign
    //     if beginning of next block is a start sign : search for ZBlock_State : if free then it may be aggregated to this one.
    //     if not : stop : no aggregation in this direction (except for found 'hole')
    //
    //   Search for block previous to the current :
    //         search for start sign :
    //             once found  test if this is a free block.
    //
    //                 if free block aggregate since begining of this found block
    //                 if not : test if there is no 'hole' between : start of previous block + blocksize must be equal to start of this block
    //

    // find previous block


ZBlockHeader wPreviousBlockHeader;
zaddress_type wPreviousAddress;



    wStartFreeBlockAddress = ZBAT.Tab[pRank].Address;

    wSt=_searchPreviousPhysicalBlock(ZBAT.Tab[pRank].Address,wPreviousAddress,wPreviousBlockHeader);

    if (wSt==ZS_OUTBOUNDLOW)
      // just get from Start of Data - returned by _searchPreviousBlock
            wStartFreeBlockAddress = wPreviousAddress ;
    else {
       // if previous block is available : Keep Address of previous block to group with current block
      if (wPreviousBlockHeader.State & (ZBS_Free | ZBS_Deleted) )
                                                // Address is : address of reading + wOffset1
                                                // Additional Length has to be computed
                                                // Think to erase current block header
      {
        wStartFreeBlockAddress=wPreviousAddress;            // grab this space !
        if(__ZRFVERBOSE__)
            _DBGPRINT("Grabbing previous free block : address %ld size %ld\n",
                      wPreviousAddress,
                      wPreviousBlockHeader.BlockSize)

// get free block in ZFBT free block pool and suppress it

        long wi;
        for (wi=0;wi<ZFBT.size();wi++) {
          if (ZFBT.Tab[wi].Address == wPreviousAddress) {
            ZFBT.erase(wi);  // suppress this block from free block pool
            break;
          }
        }// for
        /* if block to aggregate was a deleted block : remove it from deleted blocks pool */
        if (wPreviousBlockHeader.State & ZBS_Deleted ){
          for (wi=0;wi<ZDBT.size();wi++) {
            if (ZDBT.Tab[wi].Address == wPreviousAddress) {
                ZDBT.erase(wi);  // suppress this block from free block pool
                break;
              }
          }// for
        } //  if (wPreviousBlockHeader.State & ZBS_Deleted )
      } // if (wPreviousBlockHeader.State & (ZBS_Free | ZBS_Deleted) )

    else {

// previous physical block in file is Not ZBS_Free,
// so we must test wether there is a hole between previous block and freed block
//  previous block start address + blockSize + 1 < freed block start address : there is a hole

      zaddress_type wHoleAddress = (zaddress_type) wPreviousAddress+ wPreviousBlockHeader.BlockSize + 1;
      if (wHoleAddress < ZBAT.Tab[pRank].Address)   {   // yes there is a hole there
                                                                    // hole starts at wHoleAddress : grab it !
        wStartFreeBlockAddress = wHoleAddress;  // grab hole address (between previous used block and current address)
        if(__ZRFVERBOSE__)
          _DBGPRINT("Grabbing hole : address %ld\n", wHoleAddress)
                        }
        // else no Hole no Previous Free block : use current block address - as initialized
      } // if (wHoleAddress < ZBAT.Tab[pRank].Address)

    }  // else wSt==ZS_OUTBOUNDLOW


// ---------let's investigate for NEXT BLOCK to get the size ---------------
//
//     find start sign after freed block :
//
    ZBlockHeader wNextBlockHeader;
//    ZBlockHeader_Export wNextBlockHeader_Export;  // not used, not necessary
    zaddress_type wNextAddress;
    zaddress_type wEndingAddress;

    zaddress_type wStartAddress = ZBAT.Tab[pRank].Address + ZBAT.Tab[pRank].BlockSize;  // the byte after block to search next


    wSt= _searchNextPhysicalBlock(wStartAddress,wNextAddress,wNextBlockHeader);
    if (wSt<0)
            return wSt; // in case of error return status
    wEndingAddress = wNextAddress;
    if (wSt==ZS_FOUND)
      // if next block is free : grab its space and remove this block from freeblock pool
      if (wNextBlockHeader.State & (ZBS_Free & ZBS_Deleted) )
                        {
                        wEndingAddress = wNextAddress + wNextBlockHeader.BlockSize ;
                        long wi;
                        for (wi=0;wi<ZFBT.size();wi++)
                                        if (ZFBT.Tab[wi].Address == wNextAddress)
                                                        {
                                                        ZFBT.erase(wi);  // suppress this block from free block pool
                                                        break;
                                                        }
                        }


    wSizeFreeSpace = (zaddress_type)  (wEndingAddress  - wStartFreeBlockAddress);

    pBS.Address     = wStartFreeBlockAddress;
    pBS.BlockSize   = wSizeFreeSpace;

//------End Gather free blocks and grab holes-----------------

    if (pBS.Address!=ZBAT.Tab[pRank].Address)
                        {
                        ZBAT.Tab[pRank].State = ZBS_Deleted;
                        ZDBT.push(ZBAT.Tab[pRank]);
                        return (_markBlockAsDeleted (pRank));
                        }
    return ZS_SUCCESS;
}// _grabFreeSpacePhysical



/**
 * @brief ZRandomFile::_grabFreeSpaceLogical collects free blocks and holes adjacent to the block in ZBAT given by its rank and creates a new consolidated block
 *
 *      for a block to be freed given by pIndex pointing to a block in pZBAT,
 *      _grabFreeSpace identifies and collects adjacent free space and/or holes,
 *      and aggregate them to block to be freed.
 *
 *      Free blocks and holes must be physical addresses adjacent in file : search is made in ZFreeBlockPool table.
 *
 *      _grabFreeSpace removes concerned freed blocks in Free blocks pool (ZFBT).
 *
 *      a ZBlockMin_struct is returned with the references of the new created block from agregate space.
 * @param pDescriptor
 * @param pIndex
 * @param pBS       a ZBlockMin_struct containing the Address and the Size of the newly agglomerated block
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_grabFreeSpaceLogical (zrank_type pRank,       // ZBAT index to be freed
                                   ZBlockDescriptor &pBS)   // free block to be returned
{
ZStatus wSt;
zaddress_type wStartFreeBlockAddress ;
long wgrab;

zsize_type wSizeFreeSpace;

    pBS     = ZBAT.Tab[pRank];

    wStartFreeBlockAddress = ZBAT.Tab[pRank].Address;
    wSizeFreeSpace =  ZBAT.Tab[pRank].BlockSize;


/* Grab before */

// finds a previous block in free pool so that address + blocksize = ZBAT.Tab[pIndex].Address;
    if (wStartFreeBlockAddress > ZFCB.StartOfData) {

      for (wgrab=0 ; wgrab < ZFBT.size() ; wgrab++) {
        if (wStartFreeBlockAddress == (ZFBT.Tab[wgrab].Address+ZFBT.Tab[wgrab].BlockSize)) {
          // if block is not free or deleted (meaning allocated) in the pool : cannot grab it
          if (!(ZFBT.Tab[wgrab].State & (ZBS_Free | ZBS_Deleted))) {
//          if ((ZFBT.Tab[wgrab].State != ZBS_Free) && (ZFBT.Tab[wgrab].State != ZBS_Deleted))  {
            if(__ZRFVERBOSE__) {
              _DBGPRINT("ZRandomFile::_grabFreeSpaceLogical Found block to grab but it is allocated : address %ld size %ld - state %s .\n",
                  ZFBT.Tab[wgrab].Address,
                  ZFBT.Tab[wgrab].BlockSize,
                  decode_ZBS( ZFBT.Tab[wgrab].State))
            }// __ZRFVERBOSE
            break ;
          } //  State != ZBS_Free

        pBS = ZFBT.Tab[wgrab];
        wSizeFreeSpace += ZFBT.Tab[wgrab].BlockSize ;
        pBS.BlockSize = wSizeFreeSpace ;

        if(__ZRFVERBOSE__) {
          _DBGPRINT("ZRandomFile::_grabFreeSpaceLogical  Grabbing previous free block : address %ld size %ld\n",
              ZFBT.Tab[wgrab].Address, ZFBT.Tab[wgrab].BlockSize)
        }

        // mark ZTAB block header as deleted on file (new active header is ZFBT one)

        ZBAT.Tab[pRank].State |= ZBS_Deleted ;
        if ((wSt=_markBlockAsDeleted(pRank))!=ZS_SUCCESS) {
          return wSt;
        }
        ZDBT.push(ZBAT.Tab[pRank]); // add entry into deleted blocks : it will no more be the blockheader

        ZFBT.erase(wgrab);  // suppress this block from free block pool
        break;
                            }
      }// for

    }// if (wStartFreeBlockAddress > ZFCB.StartOfData)


  /* Grab after */

// Nota Bene : we have to compute the whole size with address + block size in order to take into account possible holes between blocks
//

  // compute theorical beginning address of following block

 zaddress_type wEndAddress = ZBAT.Tab[pRank].Address + ZBAT.Tab[pRank].BlockSize;
 if (wEndAddress != ZFCB.AllocatedSize) {

   for (wgrab=0; wgrab<ZFBT.size();wgrab++) {
     if (wEndAddress == (ZFBT.Tab[wgrab].Address)) {

       // if block is not free or deleted (meaning allocated) in the pool : cannot grab it
       if ((ZFBT.Tab[wgrab].State != ZBS_Free) && (ZFBT.Tab[wgrab].State != ZBS_Deleted))  {
          if(__ZRFVERBOSE__) {
           _DBGPRINT("ZRandomFile::_grabFreeSpaceLogical  Found block to grab but it is locked : address %ld size %ld - sate %s locked mask %uc reason %s \n",
                          ZFBT.Tab[wgrab].Address,
                          ZFBT.Tab[wgrab].BlockSize,
                          decode_ZBS( ZFBT.Tab[wgrab].State) )
          } //_ZRFVERBOSE

        break ;
       }//  if (ZFBT.Tab[wgrab].State != ZBS_Free)

       wEndAddress = ZFBT.Tab[wgrab].Address + ZFBT.Tab[wgrab].BlockSize;

       if(__ZRFVERBOSE__)
          _DBGPRINT("ZRandomFile::_grabFreeSpaceLogical Grabbing following free block : address %ld size %ld\n",
                                   ZFBT.Tab[wgrab].Address,
                                   ZFBT.Tab[wgrab].BlockSize)

        // mark grabbed following free ZFBT block as deleted (grabbed to current)

        if ((wSt=_markFreeBlockAsDeleted(wgrab))!=ZS_SUCCESS)
          return wSt;
        ZFBT.Tab[wgrab].State = ZBS_Deleted;
        ZDBT.push(ZFBT.Tab[wgrab]);  //! add entry into deleted blocks
        ZFBT.erase(wgrab);                        //! suppress ZFBT block from free block pool
        break;
      }// if (wEndAddress == (ZFBT.Tab[wgrab].Address))if
    } // for (wgrab=0; wgrab<ZFBT.size();wgrab++)

    pBS.BlockSize = wEndAddress - pBS.Address;
  }//    if (wEndAddress != ZFCB.AllocatedSize)
  return ZS_SUCCESS;
}// _grabFreeSpaceLogical



/**
 * @brief ZRandomFile::_markBlockAsDeleted gets a block header at given address pAddress sets its State flag to ZBS_Deleted and write it to disk
 *
 * @param pDescriptor
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_markBlockAsDeleted (zrank_type pRank)
{
    if(__ZRFVERBOSE__)
        _DBGPRINT("ZRandomFile::_markBlockAsDeleted Marking block from Block Access Table as ZBS_Deleted- address %ld size %ld\n",
                  ZBAT.Tab[pRank].Address,
                  ZBAT.Tab[pRank].BlockSize)

    ZBAT.Tab[pRank].State = ZBS_Deleted;
//    ZDBT.push(ZBAT.Tab[pIndex]);  //! move the block header to deleted pool
    return _writeBlockHeader((ZBlockHeader&) ZBAT.Tab[pRank],
                             ZBAT.Tab[pRank].Address);

}//_markBlockAsDeleted

/**
 * @brief ZRandomFile::_markFreeBlockAsDeleted Marks a free block from ZFBT as deleted on disk and moves it to deleted pool
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_markFreeBlockAsDeleted (zrank_type pRank)
{
    if(__ZRFVERBOSE__)
        _DBGPRINT("ZRandomFile::_markFreeBlockAsDeleted  Marking block from Free Pool as ZBS_Deleted address %ld size %ld\n",
                  ZFBT.Tab[pRank].Address,
                  ZFBT.Tab[pRank].BlockSize)

    ZFBT.Tab[pRank].State = ZBS_Deleted;
    ZDBT.push(ZFBT.Tab[pRank]);  // move the block header to deleted pool
    return _writeBlockHeader(
                             (ZBlockHeader&) ZFBT.Tab[pRank],
                             ZFBT.Tab[pRank].Address);

}//_markBlockAsDeleted
/**
 * @brief ZRandomFile::_markDeletedBlockAsFree marks a deleted block header from deleted pool as free to disk
 * It does not modify free blocks pool.
 *
 * @param pDescriptor
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_markDeletedBlockAsFree (zrank_type pRank)
{
    if(__ZRFVERBOSE__)
        _DBGPRINT("Marking deleted block from Recovery Pool as ZBS_Free address %ld size %ld\n",
                  ZDBT.Tab[pRank].Address,
                  ZDBT.Tab[pRank].BlockSize)

    ZDBT.Tab[pRank].State = ZBS_Free;

    return _writeBlockHeader((ZBlockHeader&) ZDBT.Tab[pRank],
                             ZDBT.Tab[pRank].Address);

}//_markDeletedBlockAsFree

/**
 * @brief ZRandomFile::_markFreeBlockAsUsed gets a block header at given address pAddress sets its State flag to ZBS_Used and write it to disk
 *
 * @param pDescriptor
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_markFreeBlockAsUsed (zrank_type pRank)
{
    if(__ZRFVERBOSE__)
        _DBGPRINT("Marking block from Free Pool as ZBS_Used address %ld size %ld\n",
                  ZFBT.Tab[pRank].Address,
                  ZFBT.Tab[pRank].BlockSize);

    ZFBT.Tab[pRank].State = ZBS_Used;
    return _writeBlockHeader(
                             (ZBlockHeader&) ZFBT.Tab[pRank],
                             ZFBT.Tab[pRank].Address);

}//_markFreeBlockAsUsed


/**
 * @brief ZRandomFile::_recoverFreeBlock
 *      recover a Free Block to Block Access Table (ZBAT)
 *      after having previously ungrabbed possible deleted blocks from Recovery Pool (ZDBT)
 *
 *      recovered block is set as ZBS_Used in ZBAT and on file surface
 *      ungrabbed blocks are set to ZBS_Free in ZFBT and on file surface
 *
 * @param pDescriptor
 * @param pIndex
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_recoverFreeBlock (zrank_type pRank)
{
ZStatus wSt;
    if(__ZRFVERBOSE__)
            _DBGPRINT("Recovering Free block from Free block Pool  address %ld size %ld\n",
                      ZFBT.Tab[pRank].Address,
                      ZFBT.Tab[pRank].BlockSize);

// ungrab free block---------------------------------------------------------------------------------
//     search for any deleted block (grabbed blocks) inside free block  (from Deleted Block Pool)
//     for nearest deleted block : create an entry into Free Block Pool
//                                 mark block as ZBS_Free on file surface
//
    zaddress_type wAddMin = ZFBT.Tab[pRank].Address;
    zaddress_type wAddMax = ZFBT.Tab[pRank].Address + ZFBT.Tab[pRank].BlockSize;

    long widx = 0;
    ZArray <long> wB;
    while (widx < ZDBT.size())
    {
        wAddMin = ZFBT.Tab[pRank].Address;
        wAddMax = ZFBT.Tab[pRank].Address + ZFBT.Tab[pRank].BlockSize;
        if ((ZDBT.Tab[widx].Address > wAddMin)&&(ZDBT.Tab[widx].Address < wAddMax)) // it belongs to this free block
                    {
                    wB.push(widx);
                    }
        widx ++;
    }// while

    if (!wB.isEmpty())
            {
            zaddress_type wAddM1 = wAddMax;
            long wIdxMin = 0;
            for (long wi = 0 ; wi < wB.size();wi++)
                    {
                        if (ZDBT.Tab[wB[wi]].Address<wAddM1)
                                    {
                                    wAddM1 = ZDBT.Tab[wB[wi]].Address;
                                    wIdxMin = wB[wi];
                                    }
                    }
            //! compute the former size of the block before it has been agglomerated by grabbing process
            //!
            ZFBT.Tab[pRank].BlockSize = ZDBT.Tab[wIdxMin].Address-ZFBT.Tab[pRank].Address ;
            ZDBT.Tab[wIdxMin].State = ZBS_Free;
            ZFBT.push(ZDBT.Tab[wIdxMin]); //! set the ungrabbed block to Free Pool as new free block
            _markDeletedBlockAsFree(wIdxMin);
            ZDBT.erase(wIdxMin);                       //! then remove it from recovery pool
            }

//  then   move free block from Free Pool to Block Access Table
//         mark it as ZBS_Used on file surface
//         remove it from Free Pool
//
    ZFBT.Tab[pRank].State = ZBS_Used;
    ZBAT.push(ZFBT.Tab[pRank]);
    if ((wSt=_markFreeBlockAsUsed(pRank))!=ZS_SUCCESS)
                            return wSt;
    ZFBT.erase(pRank);

    return _writeFCB(true) ;
}//_recoverFreeBlock


/**
 * @brief ZRandomFile::_searchPreviousBlock Overload with ZBAT index as argument
 * @param pDescriptor
 * @param pIndex
 * @param pPreviousAddress
 * @param pBlockHeader
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_searchPreviousBlock (zrank_type pRank,
                                   zaddress_type &pPreviousAddress,
                                   ZBlockHeader &pBlockHeader)
{
  return _searchPreviousPhysicalBlock(ZBAT.Tab[pRank].Address,pPreviousAddress,pBlockHeader);
}

/**
 * @brief ZRandomFile::_searchPreviousPhysicalBlock
 *
//!   Search for block previous to the current :
//!         search for start sign :
//!             once found  test if this is a free block.
//!
//!                 if free block aggregate since begining of this found block
//!                 if not : test if there is no 'hole' between : start of previous block + blocksize must be equal to start of this block
//!
//!
//!    returns status to ZS_FOUND       : the address (offset) of previous block found is loaded into pPreviousAddress
//!                                       and pBlockHeader is loaded with ZBlockHeader content
//!
//!                      ZS_OUTBOUNDLOW : either wCurrentAddress is the first address (   pPreviousAddres set to ZFCB.StartOfData )
//!                                       or no block start sign has been found (   pPreviousAddres set to ZFCB.StartOfData )
//!                                         In this last case, this is to be considered as a 'Hole'
//!                                         pBlockHeader IS NOT LOADED WITH ANY DATA
//!

//!    rewind quota is arbitrarily defined as : ZBlockHeader size + (block target size * 2)
//!
 *
 *
 * @param pDescriptor
 * @param pCurrentAddress  Offset in file to search from
 * @param pPreviousAddress Address (starting offset) of previous block just before current address or start of data in file if not found
 * @param pBlockHeader      if found the ZBlockHeader found
 * @return  ZS_FOUND        : previous block has been found and loaded into pBlockHeader
 *          ZS_OUTBOUNDLOW  : no previous block has been found and the start of data offset is returned as previous block address
 *                          In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_searchPreviousPhysicalBlock (zaddress_type pCurrentAddress,
                                   zaddress_type &pPreviousAddress,
                                   ZBlockHeader &pBlockHeader) {

ZStatus         wSt;
ssize_t          wSizeToRead=0;
zaddress_type    wAddress;
//zoffset_type    wRealAddress;
ZDataBuffer     wBuffer;

    if(__ZRFVERBOSE__)
        _DBGPRINT("%s : scanning file surface for previous physical block for address %lld \n",_GET_FUNCTION_NAME_, pCurrentAddress);

ZBlockHeader_Export*   wBlockHeader_Export=nullptr;
ssize_t wRewindQuota ;

    if (ZFCB.BlockTargetSize>0)
            wRewindQuota= sizeof(ZBlockHeader_Export) + (ZFCB.BlockTargetSize * 2); //! user content target size * 2 must be OK
        else
            wRewindQuota= ZFCB.InitialSize ;

    uint8_t wStartSign [5]={cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE , 0 };

    if (pCurrentAddress==ZFCB.StartOfData) // Only if it is not the first block for the file because 0L is the start address for File effective Data blocks
        {
        pPreviousAddress = ZFCB.StartOfData ;
        if(__ZRFVERBOSE__)
                _DBGPRINT("%s : reaching beginning of data returning address %lld \n",_GET_FUNCTION_NAME_, pPreviousAddress);
        return (ZS_OUTBOUNDLOW);
        }
// position to previous block address and load the whole stuff (wSizeToRead size) into wBuffer
//
//     rewind wAdvanceQuota until finding a block start sign  (a block size is varying and may be larger than wAdvanceQuota)
//
//     once found : search to be sure it is the LAST block BEFORE wCurrentAddress
//
    long wOffset = -1;

// we have to make sure that the last byte is the start of current block for finding previous block

    if ((pCurrentAddress-wRewindQuota)<ZFCB.StartOfData )
                        wRewindQuota = pCurrentAddress-ZFCB.StartOfData;
    wSizeToRead = wRewindQuota ;
    wBuffer.allocate(wSizeToRead);
    wAddress = ZFCB.StartOfData + 1; // just to initiate the loop


    while (wAddress >= ZFCB.StartOfData)
    {
    wAddress = pCurrentAddress - wSizeToRead;
    if (wAddress < ZFCB.StartOfData)           // we are before start of Data
                    {
                    wAddress = ZFCB.StartOfData;
                    }
    if ((wSt=_readAt(wBuffer,wSizeToRead,wAddress,ZPMS_Other))!=ZS_SUCCESS)
                        {
                        if (wSt!=ZS_READPARTIAL)
                                        {   return (wSt);}
                        }
// search for start of block sign : binary search on Start sign byte sequence
//         searches until the beginning of this block to get the block just before the current one
//
    wOffset=wBuffer.breverseSearch(wStartSign,4L,wOffset);
    if (wOffset < 0)  // if not found
        {
        if (wAddress == ZFCB.StartOfData)  //! no start of block found till start of data : this is a hole since the beginning till current address
                    {
                    pPreviousAddress = ZFCB.StartOfData ;
                    if(__ZRFVERBOSE__)
                            _DBGPRINT("%s : reaching beginning of data returning address %lld \n",_GET_FUNCTION_NAME_, pPreviousAddress);
                    return  (ZS_OUTBOUNDLOW);
                    }
        wSizeToRead += wRewindQuota ;  // rewind again
        continue;
        }
    wBlockHeader_Export = (ZBlockHeader_Export*)&wBuffer[wOffset];  // set Block Header position
    if (wBlockHeader_Export->State != ZBS_Deleted)
                                                break;

    }// while



//     memmove(&pBlockHeader_Export,wBlockHeader_Export,sizeof(ZBlockHeader_Export));
    wSt=ZBlockHeader::_importConvert(pBlockHeader,wBlockHeader_Export);
    if (wSt!=ZS_SUCCESS)
            {return  wSt;}
     pPreviousAddress = wAddress + wOffset;
    if(__ZRFVERBOSE__)
        _DBGPRINT("%s : found previous not deleted block returning address block header at address %lld \n",_GET_FUNCTION_NAME_, pPreviousAddress);

    return  (ZS_FOUND) ;
}//_searchPreviousPhysicalBlock


ZStatus
ZRandomFile::_searchNextBlock (zrank_type pRank,                     // ZBAT index to search for next block
                               zaddress_type &pNextAddress,
                               ZBlockHeader &pBlockHeader)
{
    pNextAddress = ZBAT.Tab[pRank].Address + ZBAT.Tab[pRank].BlockSize + 1;  // the byte after block to search next
    return (_searchNextPhysicalBlock (pRank,                     // ZBAT index to search for next block
                              pNextAddress,
                              pBlockHeader));

}//_searchNextBlock
/**
 * @brief ZRandomFile::_searchNextPhysicalBlock
 *
 *      searches in physical file surface for next block header starting at pAddress offset in file and returns its address as well as its block header.
 *
 *   Conversion of  ZBlockHeader_Export to ZBlockHeader is made
 *
 *
 *      returns ZS_FOUND if everything went OK and next block header has been found and is returned
 *
 *              ZS_EOF if no more block header
 *                                  in this case, pNextAddress is set to the last byte of the file, NO block header is returned
 *              returns errored status and ZException in case of other errors
 *
 *
 * @param pDescriptor
 * @param pIndex
 * @param pCurrentAddress
 * @param pNextAddress
 * @param pBlockHeader
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_searchNextPhysicalBlock (zaddress_type pAddress,      // Address to start searching for for next block
                                       zaddress_type &pNextAddress,
                                       ZBlockHeader &pBlockHeader)  // ZBlockDescriptor is a ZBlockHeader + Address of block as an offset in file
{

ZStatus         wSt;
ssize_t         wSizeToRead=0;
size_t          wAdvanceQuota;
zaddress_type    wAddress;
ZDataBuffer     wBuffer;

  off_t wFileSize = lseek(ContentFd,0,SEEK_END);  /* get file total size */

  wAdvanceQuota = sizeof(ZBlockHeader_Export) + (ZFCB.BlockTargetSize * 2); // user content target size * 2 must be OK
  wSizeToRead = sizeof(ZBlockHeader_Export) + (ZFCB.BlockTargetSize * 2); // user content target size * 2 must be OK
  wAddress = pAddress;
  if (off_t (wAddress + ZFCB.StartOfData + wSizeToRead) > wFileSize ) {
    wSizeToRead = wFileSize - (wAddress + ZFCB.StartOfData) ;
  }
  unsigned char wStartSign [5]={cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE , 0 };

    wBuffer.allocate(wSizeToRead);
    zaddress_type wOffset= -1;

  while (wOffset==-1) {
    if (wAddress >= wFileSize) {
      pAddress = wAddress;
      return ZS_EOF;
    }
    if (off_t (wAddress + ZFCB.StartOfData + wSizeToRead) > wFileSize ) {
      wSizeToRead = wFileSize - (wAddress + ZFCB.StartOfData) ;
      wBuffer.allocate(wSizeToRead);
    }

    if ((wSt=_readAt(wBuffer,wSizeToRead,wAddress,ZPMS_Other))!=ZS_SUCCESS) {
        if (wSt==ZS_EOF) {
          pNextAddress = wAddress;
          return  (ZS_EOF);
        }
        if (wSt==ZS_READPARTIAL)
           {
            wOffset=wBuffer.bsearch(wStartSign,4L,0L);
            if (wOffset<0)
                {
              /* assume that wStartSign is truncated at the end of wBuffer (max will be 3 bytes) */
                pNextAddress = wBuffer.Size + wAddress - 3 ;
                return  (ZS_EOF);
                }
                else
                {
                 pNextAddress = wAddress + wOffset;
                 wSt=pBlockHeader._importConvert(pBlockHeader,(ZBlockHeader_Export*)&wBuffer.DataChar[wOffset]);
                 if (wSt!=ZS_SUCCESS)
                             { return (wSt); }
                 return  (ZS_FOUND) ;
                 }
           }// ZS_READPARTIAL
        return  wSt;  // file error to be returned
        }

      wOffset=wBuffer.bsearch(wStartSign,4L,0L);
      if (wOffset==-1) {
        if (off_t (wAddress + ZFCB.StartOfData + wAdvanceQuota) > wFileSize ) {
          wAdvanceQuota = wFileSize - (wAddress + ZFCB.StartOfData) ;
          if (wAdvanceQuota == 0) {
            pNextAddress = wAddress;
            return  (ZS_EOF);
          }

        }
        wAddress += wAdvanceQuota ;
      }
  }// while (wOffset==-1)

     pNextAddress = wAddress + wOffset;
     wSt=pBlockHeader._importConvert(pBlockHeader,(ZBlockHeader_Export*)&wBuffer.DataChar[wOffset]);
     if (wSt!=ZS_SUCCESS)
                 { return (wSt); }
    return  (ZS_FOUND) ;
}//_searchNextPhysicalBlock
#ifdef __DEPRECATED__

/** ========== DEPRECATED - NO MORE USED =============
 * @brief ZRandomFile::_getFreeBlock This is the block allocation routine for ZRandomFile : it selects a free block in free block pool if some fits to demanded size or extends appropriately the file to get some required space.

@par   gets a new block of pSize
   - searches within Free Block Table for a block with a size >= requested size.
   - the free block must have an address greater than pBaseAddress to be selected (required for zreorg)
   - if no block can be found :
        extend the file of ExtentQuota * (BlockTargetSize+size of ZFileControlBlock)
                        add this entry into Free Block Table and reprocess
@par when found :
   - Allocates a block of pSize.
   - if pSize represents the totality of Free block size : remove entry from Free Block Table
   - if not modify Free Block Table element :
       + change Block Address to be former address + pSize
       + change Block size to be Free block size - pSize


@par calls _allocateFreeBlock :
    - creates an entry within ZBAT of appropriate pSize size from free block
    - marks this ZBAT block with a State of ZBS_Allocated

pIndex gives the rank where the new block descriptor will be inserted within ZBAT
if pIndex is -1. then the block is simply added as last ZBAT pool entry (push)

@par updates the file control block ZFCB in file

@par   returns
   - the index to ZBAT with a block marked as ZBS_Allocated (allocated by not used)
   - ZBlockMin_struct { Address, Size } with the address and size of the newly allocated block in ZBAT

 * @param[in] pSize         Required size to find
 * @param[out] pBlock       ZBlockMin_struct :  returned address AND size of the file space allocated wherever it is
 * @param[in] pFlag         ZBATAllocate_type  value (creation, insert, replace)
 * @param[in] pZBATRank     if creation, any value- else ZBAT index rank to insert or replace
 * @param[in] pBaseAddress  Base address for allocating a Free bock(used by _allocateFreeBlock())
 * @return  ZBAT rank       Insert or replace : equal to pZBATRank.
 *                          Creation : contains index of created ZBAT rank (last index of ZBAT after push()).
 *                          In case of error -1 is returned and ZException is set appropriately
 */
long
ZRandomFile::_getFreeBlock (  const size_t pSize,
                              ZBlockMin& pBlock,
                              int pFlag,              // one of ZBATAllocate_type
                              zrank_type pZBATRank,
                              const zaddress_type pBaseAddress)
{

  if ((pFlag != ZBATA_Create)&&(pFlag!=ZBATA_Insert)) {
    ZException.setMessage(_GET_FUNCTION_NAME_,ZS_INVPARAMS,Severity_Severe,"Invalid pFlag value. Expecting one of <ZBATA_Create>,<ZBATA_Insert>.");
    return -1;
  }

  zrank_type wRank=_getFreeBlockEngine(pSize,pBaseAddress);
  if (wRank<0)
    return -1;

  switch (pFlag) {
      case ZBATA_Create:
        ZBAT.push(ZFBT[wRank]);
        ZFBT.erase(wRank);
        ZBAT[wRank].State=ZBS_Allocated;
        ZBAT[wRank].Pid=getpid();
        break;
      case ZBATA_Insert:
        ZBAT.insert(ZFBT[wRank],pZBATRank);
        ZFBT.erase(wRank);
        ZBAT[wRank].State=ZBS_Allocated;
        ZBAT[wRank].Pid=getpid();
        break;

      default: /* ZBATA_Replace is managed by other means */
        ZException.setMessage(_GET_FUNCTION_NAME_,ZS_INVPARAMS,Severity_Severe,"Invalid pFlag value. Expecting one of <ZBATA_Create>,<ZBATA_Insert>.");
        fprintf(stdout,"ZRandomFile::_getFreeBlock-E-IVFLGVAL Invalid Flag value. Expecting one of <ZBATA_Create>,<ZBATA_Insert>.");
        return -1;
    }

    pBlock = ZBAT[wRank];
    return wRank;

    /*----------end-----------*/

zrank_type wIdx;
// debug
//
    if(__ZRFVERBOSE__)
            _DBGPRINT("request for free block size %ld for ZBAT index %ld\n", pSize,pZBATRank);

    for (long wi=0;wi<ZFBT.size();wi ++) {
      if (pSize <= ZFBT.Tab[wi].BlockSize)  {               // Sizes match
        if (ZFBT.Tab[wi].Address<=pBaseAddress)  // must match base address condition
          continue;
      if(__ZRFVERBOSE__)
        _DBGPRINT(" found block in pool index %ld size of free block %ld - requested size %ld \n",
                  wi,ZFBT.Tab[wi].BlockSize,pSize);

      wIdx= _allocateFreeBlock(wi,pSize,pFlag,pZBATRank);
      if (wIdx<0) {
        return  (wIdx);
      }

//---------File descriptor will be updated once commit has been done
//

        ZBAT.Tab[wIdx].State = ZBS_Allocated;
        ZBAT.Tab[wIdx].Pid = Pid;
        pBlock = ZBAT.Tab[wIdx];

        ZPMS.FreeMatches ++;
        return  wIdx;
        }// if pSize
    }// for

    if(__ZRFVERBOSE__)
            _DBGPRINT( " not found block in pool : extending \n");
//
// up to here we haven't found a free block in the pool with a size that fits into the requested size : need to extend the file
//
//     - compute needed ExtentSizeQuota times to extend the file (ExtentSizeQuota is the allocation unit to extend the file)
//     - extend the file and add an entry in Free blocks pool
//     - allocate this entry to Block Array table

ZBlockDescriptor wBlock;
ZBlockDescriptor wBlockMan;
ZStatus wSt;

zaddress_type wNewOffset;
zsize_type   wNeededExtent = pSize;

// position to end of file and get position in return

  if ((wNewOffset=(zaddress_type)lseek(ContentFd,0L,SEEK_END))<0)   {
      ZException.getErrno(errno,
                      _GET_FUNCTION_NAME_,
                      ZS_FILEPOSERR,
                      Severity_Severe,
                      " Severe error while positionning to end of file %s",
                      URIContent.toString());

      return  (ZS_FILEPOSERR);
  }

    wBlock.Address = wNewOffset;
    wBlock.State = ZBS_Free;
    wBlock.BlockSize = pSize;

// find previous logical free block in ZFBT (just before end of file)
//
    bool wFreeBlockGrabbed = false ;
    long wgrab;
    for (wgrab=0; wgrab<ZFBT.size();wgrab++) {

      if (wNewOffset == (ZFBT.Tab[wgrab].Address+ZFBT.Tab[wgrab].BlockSize)) {
          wBlock = ZFBT.Tab[wgrab] ;                          // grab this block
          wNeededExtent = wNeededExtent - wBlock.BlockSize ;   // extend but only for difference
          wFreeBlockGrabbed=true;
          break;
      }
    }// for wgrab


    if (wNeededExtent>0) {
      wSt=_getExtent(wBlockMan,wNeededExtent); // request an extension of at least pSize space
      if (wSt!=ZS_SUCCESS)
                { return  -1; }
    }
    long wZBATIndex = pZBATRank;

    wBlock.BlockSize = pSize;
    wBlock.State     = ZBS_Allocated;
    wBlock.Pid       = Pid;

//        ZPMS.FreeMatches ++;  //! this is not a free pool match but an extent

    if (pZBATRank<0) {
      wZBATIndex =ZBAT.push(wBlock) ;
    }
    else {
      wZBATIndex=ZBAT.insert(wBlock,pZBATRank);
    }
    if (wFreeBlockGrabbed)                      // if a free block has been grabbed : remove it from ZFBT
                ZFBT.erase(wgrab);
//    pBlock = ZBAT.Tab[ZBAT.lastIdx()];
    pBlock = ZBAT.Tab[wZBATIndex];

    if ( _writeBlockHeader((ZBlockHeader&)wBlock,wBlock.Address)!=ZS_SUCCESS)     // write it to file
      return wSt;
// recompute BlockTargetSize
//
    if (!ZBAT.isEmpty())
            ZFCB.BlockTargetSize = (long) ((float)ZFCB.AllocatedSize)/((float)ZBAT.size());//! must be AllocatedSize / ZBAT.size() after block has been allocated

//---------File descriptor will be updated only once commit has been done
//
//   if (_writeFileDescriptor(pDescriptor) != ZS_SUCCESS)      // write back FCB to header file
//            {
//            { return  (-1);}
//            }

    return  (wZBATIndex);  // if OK : return index of free block allocated to ZBAT : either lastIdx() or the result of inserted block index
} // _getFreeBlock

long
ZRandomFile::_getFreeBlockOld (  const size_t pSize,
                              ZBlockMin& pBlock,
                              int pFlag,              // one of ZBATAllocate_type
                              zrank_type pZBATRank,
                              const zaddress_type pBaseAddress)
{
  zrank_type wIdx;
  // debug
  //
  if(__ZRFVERBOSE__)
    _DBGPRINT("request for free block size %ld for ZBAT index %ld\n", pSize,pZBATRank);

  for (long wi=0;wi<ZFBT.size();wi ++) {
    if (pSize <= ZFBT.Tab[wi].BlockSize)  {               // Sizes match
      if (ZFBT.Tab[wi].Address<=pBaseAddress)  // must match base address condition
        continue;
      if(__ZRFVERBOSE__)
        _DBGPRINT(" found block in pool index %ld size of free block %lld - requested size %ld \n",
            wi,
            ZFBT.Tab[wi].BlockSize,
            pSize);

      wIdx= _allocateFreeBlock(wi,pSize,pFlag,pZBATRank);
      if (wIdx<0)
      {  return  (wIdx);}

      //---------File descriptor will be updated once commit has been done
      //

      ZBAT.Tab[wIdx].State = ZBS_Allocated;
      ZBAT.Tab[wIdx].Pid = Pid;
      pBlock = ZBAT.Tab[wIdx];

      ZPMS.FreeMatches ++;
      return  wIdx;
    }// if pSize
  }

  if(__ZRFVERBOSE__)
    _DBGPRINT( " not found block in pool : extending \n");
  //
  // up to here we haven't found a free block in the pool with a size that fits into the requested size : need to extend the file
  //
  //     - compute needed ExtentSizeQuota times to extend the file (ExtentSizeQuota is the allocation unit to extend the file)
  //     - extend the file and add an entry in Free blocks pool
  //     - allocate this entry to Block Array table

  ZBlockDescriptor wBlock;
  ZBlockDescriptor wBlockMan;
  ZStatus wSt;

  zaddress_type wNewOffset;
  zsize_type   wNeededExtent = pSize;


  if ((wNewOffset=(zaddress_type)lseek(ContentFd,0L,SEEK_END))<0)    // position to end of file and get position in return
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        " Severe error while positionning to end of file %s",
        URIContent.toString());

    return  (ZS_FILEPOSERR);
  }

  wBlock.Address = wNewOffset;
  wBlock.State = ZBS_Free;
  wBlock.BlockSize = pSize;

  // find previous logical free block in ZFBT (just before end of file)
  //
  bool wFreeBlockGrabbed = false ;
  long wgrab;
  for (wgrab=0; wgrab<ZFBT.size();wgrab++)
  {
    if (wNewOffset == (ZFBT.Tab[wgrab].Address+ZFBT.Tab[wgrab].BlockSize))
    {
      wBlock = ZFBT.Tab[wgrab] ;              // grab this block
      wNeededExtent = wNeededExtent - wBlock.BlockSize ;   // extend only for difference
      wFreeBlockGrabbed=true;
      break;
    }
  }// for


  if (wNeededExtent>0) {
    wSt=_getExtent(wBlockMan,wNeededExtent); // request an extension of at least pSize space
    if (wSt!=ZS_SUCCESS)
    { return  -1; }
  }
  long wZBATIndex = pZBATRank;

  wBlock.BlockSize = pSize;
  wBlock.State     = ZBS_Allocated;
  wBlock.Pid       = Pid;

  //        ZPMS.FreeMatches ++;  //! this is not a free pool match but an extent

  if (pZBATRank<0)
  {
    wZBATIndex =ZBAT.push(wBlock) ;
  }
  else
  {
    wZBATIndex=ZBAT.insert(wBlock,pZBATRank);
  }
  if (wFreeBlockGrabbed)                      // if a free block has been grabbed : remove it from ZFBT
    ZFBT.erase(wgrab);
  //    pBlock = ZBAT.Tab[ZBAT.lastIdx()];
  pBlock = ZBAT.Tab[wZBATIndex];

  if ( _writeBlockHeader((ZBlockHeader&)wBlock,wBlock.Address)!=ZS_SUCCESS)     // write it to file
    return wSt;
  // recompute BlockTargetSize
  //
  if (!ZBAT.isEmpty())
    ZFCB.BlockTargetSize = (long) ((float)ZFCB.AllocatedSize)/((float)ZBAT.size());//! must be AllocatedSize / ZBAT.size() after block has been allocated

  //---------File descriptor will be updated only once commit has been done
  //
  //   if (_writeFileDescriptor(pDescriptor) != ZS_SUCCESS)      // write back FCB to header file
  //            {
  //            { return  (-1);}
  //            }

  return  (wZBATIndex);  // if OK : return index of free block allocated to ZBAT : either lastIdx() or the result of inserted block index
} // _getFreeBlockOld

#endif // __DEPRECATED__

/**
 * @brief ZRandomFile::_getFreeBlockEngine Delivers a free block of size pRequestedSize.
 *
 *  Obtains a free block of size pRequestedSize searching in file at minimum address pBaseAddress,
 *  Extends file whenever required, creates an entry in free blocks pool
 *  Returns corresponding block rank in free blocks pool
 *
@par   gets a new block of pRequestedSize
   - searches within Free Block Table for a block with a size >= requested size.
   - the free block must have an address greater than pBaseAddress to be selected (required for zreorg)
   - if no corresponding block can be found :
        . extend the file using _getExtent() :
              file surface is extended using following formula
              ExtentQuota * (BlockTargetSize)+size of ZFileControlBlock -> if this value is greater than requested size,
              Or -> requested size if not.
        . change last free block of the file's surface
        . add this entry into Free Block Table

    @Note:  Since base address is used by zreorg(), it is admitted that when extending file,
            Base address condition does not apply.

@par when found or extended see checkSplitFreeBlock() :
   - if pRequestedSize (requested block size) equals Free block size : leave it as is and return its rank in free block table
   - if the free block size is greater than requested size pSize, modify free blocks pool element :
      + save block address and size , save rank as returned rank
      + append a new free block in free blocks table as follows :
       ° change block address to be former address + pSize
       ° change block size to be Free block size - pSize
       ° update block on file and create free block on file

@par updates the file control block ZFCB in file

@par   returns
   - the index within Free block pool with a block marked as ZBS_Free
 *
 * @param pRequestedSize  requested size for free block to obtain
 * @param pBaseAddress    base address to search for existing free blocks in file
 * @return index/rank in Free blocks pool (ZFBT) for block or -1 if errored, and in this latter case, ZException is set appropriately.
 */

long
ZRandomFile::_getFreeBlockEngine (const size_t pRequestedSize, const zaddress_type pBaseAddress)
{
  if(__ZRFVERBOSE__)
    _DBGPRINT("ZRandomFile::_getFreeBlockEngine Request for free block of minimum size %ld \n", pRequestedSize)

  for (long wi=0;wi<ZFBT.size();wi ++) {

    if (ZFBT.Tab[wi].Address <= pBaseAddress)  // must match base address condition
      continue;

    // if block is not free or deleted (meaning it is allocated) in the pool : skip it
    if ((ZFBT.Tab[wi].State != ZBS_Free) && (ZFBT.Tab[wi].State != ZBS_Deleted))
      continue;

    if (pRequestedSize <= ZFBT.Tab[wi].BlockSize)  {               // Sizes match
      if(__ZRFVERBOSE__)
        _DBGPRINT("ZRandomFile::_getFreeBlockEngine found block in pool index %ld address %ld state %s size %ld - requested size %ld \n",
            wi,
            ZFBT.Tab[wi].Address,
            decode_ZBS(ZFBT.Tab[wi].State),
            ZFBT.Tab[wi].BlockSize,
            pRequestedSize
            )
      ZPMS.FreeMatches ++;
      return checkSplitFreeBlock(wi,pRequestedSize);
    }// if pSize
  }// for

  if(__ZRFVERBOSE__)
    _DBGPRINT( "ZRandomFile::_getFreeBlockEngine not found block in pool : expanding file \n");

  /* -----------------------------------------------------------------------------------------------------------
   * Note - Base address :    Since base address is used by zreorg(), it is admitted that when extending file,
   *                          Base address condition does not apply.
   * -----------------------------------------------------------------------------------------------------------*/
  //
  // up to here we haven't found a free block in the pool with a size that fits into the requested size : need to extend the file
  //
  //     - compute needed ExtentSizeQuota times to extend the file (ExtentSizeQuota is the allocation unit to extend the file)
  //     - extend the file and add an entry in Free blocks pool
  //     - allocate this entry to Block Array table

  ZBlockDescriptor wBlock;
  ZBlockDescriptor wBlockMan;
  ZStatus wSt;

  zaddress_type wLastFileOffset;
  zsize_type   wNeededExtent = pRequestedSize;

   // position to end of file and get position in return
  if ((wLastFileOffset=(zaddress_type)lseek(ContentFd,0L,SEEK_END))<0)  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        " Severe error while positionning to end of file %s",
        URIContent.toString());

    return  -1;
  }

  wBlock.Address = wLastFileOffset;
  wBlock.State |= ZBS_Free;
  wBlock.BlockSize = pRequestedSize;

  /*
   *  find previous logical free block in ZFBT (just before end of file)
  */
  bool wFreeBlockGrabbed = false ;
  long wgrab;
  for (wgrab=0; wgrab < ZFBT.count();wgrab++) {
    if (wLastFileOffset == zaddress_type(ZFBT.Tab[wgrab].Address+ZFBT.Tab[wgrab].BlockSize)) /* if grabbed free block is last block of the file */
    {
      wBlock = ZFBT.Tab[wgrab] ; // grab this block
      wNeededExtent = wNeededExtent - wBlock.BlockSize ;    // request an file extension only for difference
      wFreeBlockGrabbed=true;
      break;
    }
  }// for

  if (wNeededExtent > 0) {
    wSt=_getExtent(wBlockMan,wNeededExtent); // request an extension of at least wNeededExtent space
    if (wSt!=ZS_SUCCESS)
          { return  -1; }
    if (wFreeBlockGrabbed) {
      /* now we have a huge free block including extent quota space */
      ZFBT.Tab[wgrab].BlockSize = ZFBT.Tab[wgrab].BlockSize + wBlockMan.BlockSize; /* extend last free block with extended space */
      return checkSplitFreeBlock(wgrab,pRequestedSize);
    }
  /* we don't have grabbed the last free block :
   *  all space is coming from _getExtent() and is neither marked with header nor stored in ZFBT (see _getExtent())
  */

  /* create a ZFBT entry for this space using returned ZBlockDescriptor (wBlockMan) */

    wBlockMan.State = ZBS_Free;
    ZFBT.push(wBlockMan);

  /* then take what space is appropriate :
   * IMPORTANT REMARK : checkSplitFreeBlock() split free space appropriately, and creates block headers on file, */

    return checkSplitFreeBlock(ZFBT.lastIdx(),pRequestedSize);
  }//if (wNeededExtent>0)

  wBlock.BlockSize = pRequestedSize;
  wBlock.State     = ZBS_Allocated;
  wBlock.Pid       = Pid;

//        ZPMS.FreeMatches ++;  //! this is not a free pool match but an extent

  if ( _writeBlockHeader((ZBlockHeader&)wBlock,wBlock.Address)!=ZS_SUCCESS){     // write extended free block to file
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_CANTALLOCSPACE,
                            Severity_Severe,
                            "Severe error while writing allocated free block header at address %ld file %s",
                            wBlock.Address,
                            URIContent.toString());

    return -1;
  }

// recompute BlockTargetSize

  ZFCB.BlockTargetSize = (long) ((float)ZFCB.AllocatedSize)/((float)ZBAT.size()+1.0f);// must be AllocatedSize / ZBAT.size() after block has been allocated


//---------File descriptor will be updated only once commit has been done ----
//
//   if (_writeFileDescriptor(pDescriptor) != ZS_SUCCESS)      // write back FCB to header file
//            {
//            { return  (-1);}
//            }

//  return  (wZBATIndex);  // if OK : return index of free block allocated to ZBAT : either lastIdx() or the result of inserted block index

  ZFBT.push(wBlock);
  return ZFBT.lastIdx();
} // _getFreeBlockEngine

/**
 * @brief ZRandomFile::_deleteZBAT
 *  Removes a ZBAT rank and moves it to free block pool.
 *  If GrabFreeSpace is set, tries to grab free space (see routine _grabFreeSpaceLogical())
 *  Updates block header on disc.
 *
 * @param pRank
 * @return
 */
long
ZRandomFile::_deleteZBAT (const size_t pRank)
{
  ZStatus wSt;
  ZBlockDescriptor wBS;

  wBS   = ZBAT.Tab[pRank];
  wBS.State       = ZBS_Free;

  if (ZFCB.GrabFreeSpace) {
    if ((wSt=_grabFreeSpaceLogical(pRank,wBS))!=ZS_SUCCESS)
      return wSt;
  }


  long wFBTRank=ZFBT.push(ZBAT[pRank]);
  if ( _writeBlockHeader((ZBlockHeader&)ZFBT.last(),ZFBT.last().Address)!=ZS_SUCCESS){     // mark block as free to file
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_CANTALLOCSPACE,
        Severity_Severe,
        "Severe error while writing free block header at address %ld file %s",
        ZFBT.last().Address,
        URIContent.toString());

    return -1;
  }
  ZBAT.erase(pRank);
  return wFBTRank;

} // _deleteZBAT

/**
 * @brief ZRandomFile::checkSplitFreeBlock
   - if pSize (requested block size) equals Free block size : leave it as is and return its rank in free block table
   - if the free block size is greater than requested size pSize, modify free blocks pool element :
      + save block address and size , save rank as returned rank
      + append a new free block in free blocks table as follows :
       ° change block address to be former address + pSize
       ° change block size to be Free block size - pSize
       ° update block on file and create free block on file
 * @param pRank
 * @param pSize
 * @return
 */
long
ZRandomFile::checkSplitFreeBlock (long pZFBTRank, size_t pRequestedSize){

  if (pRequestedSize==0) {  /* just to debug */
    abort();
  }

  if (ZFBT.Tab[pZFBTRank].State & ZBS_Deleted) {
    for (long wi=0; wi < ZDBT.count();wi++) {
      if (ZDBT[wi].Address==ZFBT.Tab[pZFBTRank].Address) {
        ZDBT.erase(wi);
        if(__ZRFVERBOSE__)
          _DBGPRINT( "ZRandomFile::checkSplitFreeBlock  Removed block from deleted pool rank %ld address %ld size %ld.\n",wi, ZDBT.Tab[wi].Address,ZDBT.Tab[wi].BlockSize)
        break;
      }
    }// for
  } // if (ZFBT.Tab[pZFBTRank].State & ZBS_Deleted)

  if(__ZRFVERBOSE__)
    _DBGPRINT( "ZRandomFile::checkSplitFreeBlock  Requested size %ld - given free block size %ld address %ld.\n",
        pRequestedSize,ZFBT.Tab[pZFBTRank].BlockSize,ZFBT.Tab[pZFBTRank].Address)

  /* see <when found or extended> found : must adjust sizes. Free block size must correspond exactly to requested size */
  if (pRequestedSize==ZFBT.Tab[pZFBTRank].BlockSize) {

    if(__ZRFVERBOSE__)
      _DBGPRINT( "ZRandomFile::checkSplitFreeBlock  Sizes match. No split.\n")
    ZFBT.Tab[pZFBTRank].State = (uint8_t)ZFBT.Tab[pZFBTRank].State  | ZBS_Allocated ;
    return pZFBTRank;
  }
  /* here free block size is greater than requested size */
  /* split the block and create a free block pool entry with remaining size */

  ZBlockDescriptor wNewBlock ;
  wNewBlock.BlockSize = ZFBT.Tab[pZFBTRank].BlockSize - pRequestedSize ;
  wNewBlock.Address = ZFBT.Tab[pZFBTRank].Address + pRequestedSize;
  wNewBlock.State = ZBS_Free;
  wNewBlock.Pid = wNewBlock.Lock = 0;

  ZFBT.push(wNewBlock);   /* create new entry within pool */

  if(__ZRFVERBOSE__)
    _DBGPRINT( "ZRandomFile::checkSplitFreeBlock  Creating new free block entry in ZFBT pool of size %ld address %ld.\n",wNewBlock.BlockSize,wNewBlock.Address)

  /* adjust block to its required size */

  ZBlockDescriptor wRetBlock = ZFBT.Tab[pZFBTRank] ;
  wRetBlock.BlockSize = pRequestedSize;
//  wRetBlock.Address = ZFBT.Tab[pZFBTRank].Address;
  wRetBlock.State = ZBS_Allocated;
  wRetBlock.Pid = getpid();
  wRetBlock.Lock = 0;

  ZFBT[pZFBTRank] = wRetBlock; /* replace adjusted block in free blocks pool at given rank */
  ZStatus wSt;
  /* write both block headers to file */
  if(__ZRFVERBOSE__)
    _DBGPRINT( "ZRandomFile::checkSplitFreeBlock  Writing to file split block header at address %ld.\n",wRetBlock.Address)

  if ( (wSt=_writeBlockHeader((ZBlockHeader&)wRetBlock,wRetBlock.Address))!=ZS_SUCCESS) { // write it to file
    ZException.setMessage(_GET_FUNCTION_NAME_, ZS_CANTALLOCSPACE , Severity_Severe,
        "Cannot write free block header address %ld for file %s. This severe error may require a reorganization of file.",
        wRetBlock.Address,URIContent.toString());
    return -1;
  }
  if(__ZRFVERBOSE__)
    _DBGPRINT( "ZRandomFile::checkSplitFreeBlock  Writing to file new free block header at address %ld.\n",wNewBlock.Address)

  if ( (wSt=_writeBlockHeader((ZBlockHeader&)wNewBlock,wNewBlock.Address))!=ZS_SUCCESS) { // write it to file
    ZException.setMessage(_GET_FUNCTION_NAME_, ZS_CANTALLOCSPACE , Severity_Severe,
        "Cannot write free block header address %ld for file %s. This severe error may require a reorganization of file.",
        wRetBlock.Address,URIContent.toString());
    return -1;
  }

  return pZFBTRank;  /* return rank from free block pool for selected block */
} //checkSplitFreeBlock



/**
 * @brief ZRandomFile::_getExtent raw extension of file to current file size + pSize bytes. extension space is set to binary zero.
 *
 * request to extend the file to get pSize bytes available :
 *
  - extend the file :
      file surface is extended using following formula
      ExtentQuota * (BlockTargetSize)+size of ZFileControlBlock -> if this value is greater than requested size,
      Or -> requested size if not.

 * - use posix_fallocate to get more disk space
 * - extend the file with a zeroed set of bytes
 *
 * - return a ZBlockDescriptor containing offset and size (size is >= pSize )
 *
 *  WARNING :  _getExtent does not write a block header but only adds free space at the end of file
 *              ------------------------------------------------------------------------------------
 *              Block header(s) has(have) to be written by callee routine
 *
 * @param pDescriptor
 * @param pBlockMin A ZBlockDescriptor newly returned with offset and size
 * @param pSize
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_getExtent(ZBlockDescriptor &pBlockDesc, const size_t pExtentSize)
{
zsize_type      wExtentSize ;
zaddress_type    wNewOffset;

/* computation of size for extension */

  wExtentSize = ZFCB.BlockTargetSize * ZFCB.BlockExtentQuota ;
  if (wExtentSize < pExtentSize)
    wExtentSize = pExtentSize;

  if ((wNewOffset=(zaddress_type)lseek(ContentFd,0L,SEEK_END))<0)    // position to end of file and get position in return
                    {
                    ZException.getErrno(errno,
                                    _GET_FUNCTION_NAME_,
                                    ZS_FILEPOSERR,
                                    Severity_Severe,
                                    " Severe error while positionning to end of file %s",
                                    URIContent.toString());


                    return   (ZS_FILEPOSERR);
                    }

  int wS = posix_fallocate(ContentFd,wNewOffset,wExtentSize);
  if (wS<0) {
  ZException.getErrno(errno,
                    _GET_FUNCTION_NAME_,
                    ZS_CANTALLOCSPACE,
                    Severity_Severe,
                    " Severe error while allocating (posix_fallocate) disk space size %ld to end of file %s",
                    URIContent.toString());
    return  (ZS_CANTALLOCSPACE);
    }
//    wExtentSize=pSize;

    ZDataBuffer wBuf;
    wBuf.allocate(wExtentSize);
    wBuf.clearData();

    ZPMS.ExtentWrites ++;
    ZPMS.ExtentSize += wBuf.Size ;

    if (write (ContentFd,wBuf.Data,wBuf.Size)<wBuf.Size)      // write extent zeroed
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_WRITEERROR,
                                 Severity_Severe,
                                 " Severe error while extending file %s",
                                 URIContent.toString());
                return  (ZS_WRITEERROR);
                }

    fdatasync(ContentFd);

    ZFCB.AllocatedSize += wExtentSize ;

    pBlockDesc.Address    = wNewOffset ;
    pBlockDesc.BlockSize  = wExtentSize ;

    if(__ZRFVERBOSE__)
        _DBGPRINT(" extended file from offset %ld for size %ld for requested size %ld\n", wNewOffset, wExtentSize, pExtentSize);
    return   ZS_SUCCESS;
}//_getExtent

//! @endcond // Development

/**
 * @brief ZRandomFile::zadd Adds a new record (pUserBuffer) to the logical end of file : the last one in terms of rank.
 * @note : the added block may be physically anywhere within the file.
 * It depends on where ZRandomFile management algorithms will find appropriate free space (or not).
 * @param[in] pUserBuffer record to add
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zadd(ZDataBuffer &pUserBuffer)
{
    zaddress_type wAddress;
    return(_add(pUserBuffer,wAddress));
}//zadd


//---------ZArray emulation-------------------------
//
//
/**
 * @brief ZRandomFile::push (same as ZRandomFile::zadd ) Adds a new record (pUserBuffer) to the logical end of file : the last one in terms of rank.
 * @param[in] pUserBuffer record to add
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::push(ZDataBuffer &pUserBuffer)
{
    zaddress_type wAddress;
    return(_add(pUserBuffer,wAddress));
}//zpush
/**
 * @brief ZRandomFile::push_front inserts a new record (pUserBuffer) to the logical beginning of the file : the first one in terms of rank.
 * @param pUserBuffer record to add
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::push_front(ZDataBuffer &pUserBuffer)
{
    zaddress_type wAddress;
    return(_insert (pUserBuffer,0L,wAddress));
}//zpush

/**
 * @brief ZRandomFile::popRP pop with return : suppresses the last record in ZBlockAccessTable pool and returns its user record as a ZDataBuffer
 * @param[out] pUserBuffer record being deleted by popR
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::popRP(ZDataBuffer &pUserBuffer)
{
zaddress_type wAddress;
ZStatus wSt;
ZBlock  wBlock;


long wRank = ZBAT.lastIdx();
    wSt=_getByRank(  wBlock,wRank,wAddress);
    pUserBuffer=wBlock.Content;
    if (wSt!=ZS_SUCCESS)
            return wSt;
    return(_freeBlock(wRank));
}//popRP

/*
long
ZRandomFile::move(size_t pDest, size_t pOrig,size_t pNumber)
{
ZStatus wSt;


    long wi=ZBAT.move(pDest,pOrig,pNumber);
    wSt=_writeFileDescriptor(ZDescriptor);
    if (wSt!=ZS_SUCCESS)
                return -1;
    return(wi);
}//move
*/

/**
 * @brief ZRandomFile::swap Exchange the number pNumber of records (default 1) from rank pOrig to rank pDest
 * @note : physical position of blocks does not change. Only ZBlockAccessTable data is changed.
 *
 * @param[in] pDest     destination rank to received exchanged blocks (pNumber blocks)
 * @param[in] pOrig     source for the ranks to be swapped
 * @param[in] pNumber   number of records to swap. Defaulted to 1
 * @return      pDest is returned
 */
zrank_type ZRandomFile::swap(const size_t pDest, const size_t pOrig,const size_t pNumber)
{
ZStatus wSt;
    long wi=ZBAT.swap(pDest,pOrig,pNumber);
    wSt=_writeFCB(true);
    if (wSt!=ZS_SUCCESS)
                return -1;
    return(wi);
}//swap

//---------------- Add sequence ---------------------------------------------------------

ZStatus
ZRandomFile::_add(const ZDataBuffer &pUserBuffer, zaddress_type &pAddress)
{


ZStatus         wSt;
long            wIdxCommit;

  wSt= _add2Phases_Prepare(pUserBuffer,wIdxCommit,pAddress);
  if (wSt!=ZS_SUCCESS)
              {  return  wSt;  }
  return  _add2Phases_Commit(pUserBuffer,wIdxCommit,pAddress);
}//_add



/**
 * @brief ZRandomFile::_add2PhasesCommit_Prepare Prepare to add a record.
 * Allocates a block in pool.
 * Returns a valid ZBAT rank and an Address.
 *
 *      Newly allocated block is marked ZBS_Allocated in ZBlockAccessTable pool (Meaning it is not usable for any other operation than commit)
 *
 *
 * @param[in]   pUserContent    User record to add to file.
 * @param[out]  pZBATIndex      Returned current index within ZBAT pool of block allocated (currently being created)
 * @param[out]  pLogicalAddress Returned logical address (file offset since start of data) for current block being created.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_add2Phases_Prepare(const ZDataBuffer &pUserContent,
                                       zrank_type &pZBATIndex,
                                       zaddress_type &pLogicalAddress)
{

//------ Lock file for adding record-----------------
//
//
/*    wSt = _lockFile(ZLock_ReadWrite);
    if (wSt!=ZS_SUCCESS)
                { return   wSt;}
*/
  // open mode allows to write ?
  if (!(Mode & ZRF_Write_Only))  {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_MODEINVALID,
                              Severity_Error,
                              " File open mode <%s> does not allow record write to file %s",
                              decode_ZRFMode(Mode),
                              URIContent.toString());
      return ZS_MODEINVALID;
  }
  size_t wNeededSize = pUserContent.Size + sizeof(ZBlockHeader_Export);

  long wFreeBlockRank = _getFreeBlockEngine (wNeededSize,-1);
  if (wFreeBlockRank <0) {
    return  ZException.getLastStatus();
  }
  /* if free block comes from a deleted block in free pool mark it specially (recover option with rollback) */
  if (ZFBT[wFreeBlockRank].State==ZBS_Deleted){
    ZFBT[wFreeBlockRank].State=ZBS_AllocFromDelete;
  }
  else
    ZFBT[wFreeBlockRank].State=ZBS_Allocated;

  pZBATIndex=ZBAT.push(ZFBT[wFreeBlockRank]);

    // if block is in deleted pool remove it from deleted pool

  ZFBT.erase(wFreeBlockRank);   /* remove allocated block from free blocks pool */

  ZBAT.Tab[pZBATIndex].Pid       = Pid;
  ZBAT.Tab[pZBATIndex].Lock      = 0;



//    CurrentRecord = pUserBuffer;
  if(__ZRFVERBOSE__)
    _DBGPRINT("ZRandomFile::_add2Phases_Prepare prepare to add block at %ld size %ld state %s to ZBAT rank %ld\n",
        ZBAT.Tab[pZBATIndex].Address,
        ZBAT.Tab[pZBATIndex].BlockSize,
        decode_ZBS(ZBAT.Tab[pZBATIndex].State),
        pZBATIndex)


    pLogicalAddress = setLogicalFromPhysical( ZBAT.Tab[pZBATIndex].Address) ;
    CurrentRank = pZBATIndex;

    return   ZS_SUCCESS;
}//_add2PhasesCommit_prepare

/**
 * @brief ZRandomFile::_add2PhasesCommit_Commit Physically writes the record (and block) content to content file using information given from add_Prepare

    creates the block containing user record (pUserBuffer).
    Block is then marked in ZBAT pool as ZBS_Used.
    Writes everything on file
    Updates file control block on file.

 * @param[in] pUserContent  User record to add to file.
 * @param[in] pIdxCommit    Current index within ZBAT pool of created block to be committed (created by _add2PhasesCommit_Prepare() )
 * @param pLogicalAddress   Return : logical address (file offset since start of data) for current block having be created.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_add2Phases_Commit(const ZDataBuffer &pUserContent,
                                      const zrank_type pZBATIndex,
                                      zaddress_type &pLogicalAddress)
{
ZStatus wSt;
ZBlock  wBlock;

    ZBAT.Tab[pZBATIndex].State = ZBS_Used;
    ZBAT.Tab[pZBATIndex].Pid   = 0L;

    wBlock = ZBAT.Tab[pZBATIndex];
    wBlock.State = ZBS_Used ;
    wBlock.Content=pUserContent;
    if ((wSt=_writeBlockAt(wBlock,ZBAT.Tab[pZBATIndex].Address))!=ZS_SUCCESS) {
      ZException.last().setComplement("during _add2Phases_Commit");
      return  wSt;
    }

    if(__ZRFVERBOSE__)
      _DBGPRINT("ZRandomFile::_add2Phases_Commit-- commit appending block rank %ld @ %ld size %ld \n",
          pZBATIndex,
          ZBAT.Tab[pZBATIndex].Address,
          ZBAT.Tab[pZBATIndex].BlockSize)

    ZFCB.UsedSize += wBlock.BlockSize ;

    if (ZBAT.Tab[pZBATIndex].State & ZBS_Deleted) { /* if free block comes from a deleted block */
      long wi=0 ;
      /* search block by address in deleted pool */
      for ( ; wi < ZDBT.count() ; wi++) {
        if (ZDBT.Tab[wi].Address==ZBAT.Tab[pZBATIndex].Address){
          break;
        }
      }// for
      if (ZDBT.Tab[wi].Address==ZBAT.Tab[pZBATIndex].Address){
        /* if found : remove it from pool */
        ZDBT.erase(wi);
      }
      else { /* this is totally abnormal and must be signalled in any case */
        fprintf(stderr,"ZRandomFile::_insert2Phases_Commit Allocated free block from deleted block address %ld has not been found in deleted block pool.\n", ZBAT.Tab[pZBATIndex].Address);
        std::cout.flush();
      }

    } //if (ZBAT.Tab[pZBATIndex].State == ZBS_AllocFromDelete)
    ZBAT.Tab[pZBATIndex].State = ZBS_Used ;

    pLogicalAddress = setLogicalFromPhysical( ZBAT.Tab[pZBATIndex].Address);

    wSt=_writeFCB(true);
    return wSt;
/*    if (wSt!=ZS_SUCCESS)
        {
        _unlockFile();
        return  wSt;
        }

    return  _unlockFile();*/

}//_add2Phases_commit

/**
 * @brief ZRandomFile::_add2PhasesCommit_Rollback Undo add_Prepare operation. Frees the allocated block : makes it available in Free block pool.
 * @param pDescriptor
 * @param[in] pZBATIndex Current index within ZBAT pool of created block to be suppressed by rollback operation
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_add2Phases_Rollback(const zrank_type pZBATIndex)
{
ZStatus wSt;
ZBlockDescriptor wBS;

  wBS   = ZBAT.Tab[pZBATIndex];

/*  if (ZFCB.GrabFreeSpace)
      if ((wSt=_grabFreeSpaceLogical(pZBATIndex,wBS))!=ZS_SUCCESS)
        return wSt;
*/
/* restore block status ex-ante */
  if (wBS.State == ZBS_AllocFromDelete)
    wBS.State = ZBS_Deleted;
  else
    wBS.State = ZBS_Free;

  ZFBT.push(wBS);            // create entry into ZFBT for freed Block
  ZBAT.erase(pZBATIndex);    // remove entry from ZBAT

  if ( _writeBlockHeader((ZBlockHeader&)wBS,wBS.Address)!=ZS_SUCCESS){     // write free block header to file
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_WRITEERROR,
        Severity_Severe,
        "Severe error while writing free block header at address %ld file %s",
        wBS.Address,
        URIContent.toString());

    return ZS_WRITEERROR;
  }


  CurrentRank = -1;          // no current index

  return _writeFCB(false);
//  return ZS_SUCCESS ;

//    return _unlockFile();
}//_add2Phases_rollback

//----------End Add sequence ---------------------------------------------------------------




//-----------------Remove sequence----------------------------------------------------------
//

/**
 * @brief ZRandomFile::zremove
 *              removes a block pointed by its rank order (pRank).
        @note
        Space (and data) is put in ZFreeBlockPool pool.
        If highwater marking option has been set on, the whole data block is first zeroed and all data is lost.
 * @param pRank     rank of the record to remove            (Input)
 * @param pRecord   user content of the deleted record      (Output)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zremove(long pRank)
{

    return(_remove(pRank));

}// zremove
/**
 * @brief ZRandomFile::zremoveByAddress
 *              removes a block pointed by its physical address.
        @note
        Space (and data) is put in ZFreeBlockPool pool.
        If highwater marking option has been set on, the whole data block is first zeroed and all data is lost.
 * @param pAddress Physical address of the block to be removed  (Input)
 * @param pRecord   user content of the deleted record          (Output)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zremoveByAddress (zaddress_type pAddress)
{

    long wRank = searchBlockRankByAddress(pAddress);
    if (wRank<0)
            return ZS_INVADDRESS;
    return _remove(wRank);
}//zremoveByAddress


/**
 * @brief ZRandomFile::zremoveR
 *          remove record with return.
        removes the block pointed by its rank order (pRank) and returns its user content within a ZDataBuffer structure (pRecord)
        @note
        Space (and data) is put in ZFreeBlockPool pool.
        If highwater marking option has been set on, the whole data block is first zeroed and all data is lost.
 *
 * @param pRank     rank of the record to remove            (Input)
 * @param pRecord   user content of the deleted record      (Output)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zremoveR(long pRank,ZDataBuffer &pRecord)
{
        return(_removeR(pRecord,pRank));
}// zremoveR

//----------Remove By Rank stuff---------------------------------------------

ZStatus
ZRandomFile::_remove(const zrank_type pRank)
{
ZStatus wSt;
zaddress_type wAddress;
    wSt=_remove_Prepare(pRank,wAddress);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return _remove_Commit(pRank);
}// _remove

ssize_t
ZRandomFile::getFileSize() {
  ssize_t wRet = lseek(ContentFd,0L,SEEK_END);
  if (wRet < 0)
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        " Severe error while positionning to end of file %s",
        URIContent.toString());
  return wRet;
}


//! @cond development
/**
 * @brief ZRandomFile::_remove_Prepare prepares to remove a block pointed by its rank (within ZBlockAccessTable pool) and returns its Address
 *
 *  Locks the file
 *  Block (in ZBAT pool) is marked ZBS_BeingDeleted.
 *
 *
 * @param[in]   pDescriptor
 * @param[in]   pZBATRank     Input : rank of the block to delete (ZBAT pool index)
 * @param[out]  pLogicalAddress Return: the logical address of the block being deleted
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_remove_Prepare(const zrank_type pZBATRank, zaddress_type &pLogicalAddress)
{
ZStatus wSt;

/*    wSt = _lockFile (ZLock_All ) ; // lock Master File for update after having tested if already locked

    if (wSt!=ZS_SUCCESS)        // either file is already locked or a Severe IO error
                return wSt;
*/
    wSt=testRank(pZBATRank,_GET_FUNCTION_NAME_);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    pLogicalAddress=setLogicalFromPhysical( ZBAT.Tab[pZBATRank].Address);
//    _freeBlock_Prepare(pZBATRank);
    ZBAT.Tab[pZBATRank].State = ZBS_BeingDeleted;
    ZBAT.Tab[pZBATRank].Pid   = Pid;
    CurrentRank = pZBATRank ;

    return ZS_SUCCESS;
}// _remove_Prepare



//----------Remove By Rank with Return  : returning user buffer (record)---------------------------------------------
/**
 * @brief ZRandomFile::_removeR Remove By Rank with Return  : returns user buffer (record) after having removed the block (put in Free blocks pool)
 * @param[in] pDescriptor
 * @param[out] pUserBuffer
 * @param[in] pRank
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_removeR(ZDataBuffer &pUserBuffer, const zrank_type pRank)
{
ZStatus wSt=ZS_SUCCESS;
zaddress_type wAddress ;
    wSt=_removeR_Prepare(pUserBuffer,pRank,wAddress);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return _remove_Commit(pRank);
}// _removeR

ZStatus
ZRandomFile::_removeR_Prepare( ZDataBuffer &pUserBuffer, const zrank_type pRank, zaddress_type &pAddress)
{
ZStatus wSt=ZS_SUCCESS ;
ZBlock wBlock;

//    wSt = _lockFile (ZLock_All ) ; // lock Master File for update after having tested if already locked
//    if (wSt!=ZS_SUCCESS)        // either file is already locked or a Severe IO error
//                return wSt;

    wSt=_getByRank (wBlock,pRank,pAddress); // lock record RFFU may be replaced by LockRecord method
    if (wSt!=ZS_SUCCESS)
                return wSt;
    pUserBuffer = wBlock.Content ;

    _freeBlock_Prepare(pRank);

    return ZS_SUCCESS;
}// _removeR_Prepare


//---------- remove By Address stuff---------------------------------------------
/**
 * @brief ZRandomFile::_removeByAddress removes a block using its address.
 * @param pDescriptor
 * @param pUserBuffer
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_removeByAddress(const zaddress_type &pAddress)
{
ZStatus wSt;
long wIdxCommit;
    wSt=_removeByAddress_Prepare(wIdxCommit,pAddress);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return _remove_Commit(wIdxCommit);
}// _removeByAddress

/**
 * @brief ZRandomFile::_removeByAddress_Prepare  Prepares to remove a block at address given by pAddress and returns its rank in ZBlockAccessTable pool.
 * @param pDescriptor
 * @param pIdxCommit
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_removeByAddress_Prepare( zrank_type &pIdxCommit, const zaddress_type pAddress)
{
ZStatus wSt;

//    wSt = _lockFile (ZLock_All ) ; //! lock Master File for update after having tested if already locked

    if (wSt!=ZS_SUCCESS)        //! either file is already locked or a Severe IO error
                return wSt;

    for (long wi = 0; wi < ZBAT.size();wi++)
    {
            if (ZBAT.Tab[wi].Address == pAddress)
            {
                pIdxCommit=wi;
                _freeBlock_Prepare(pIdxCommit);
                CurrentRank = wi ;
                return ZS_SUCCESS;
            }
    }//for
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVADDRESS,
                            Severity_Severe,
                            "Given address %lld does not correspond to a valid start of block address in pool ZBAT for file %s",
                            pAddress,
                            URIContent.toCChar());
    return ZS_INVADDRESS;

}// _removeByAddress_Prepare


/**
 * @brief ZRandomFile::_removeRByAddress  remove by address with return (record content).

    Returns the rank and the record content of the record corresponding to address pAddress and prepares to remove it.

* @param pDescriptor
 * @param pUserBuffer
 * @param pIdxCommit
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_removeRByAddress( ZDataBuffer &pUserBuffer, zrank_type &pIdxCommit, const zaddress_type pAddress)
{
ZStatus wSt;

    wSt=_removeRByAddress_Prepare(pUserBuffer,pIdxCommit,pAddress);
    if (wSt!=ZS_SUCCESS)
            return (wSt);
    _remove_Commit(pIdxCommit);
    return ZS_SUCCESS;
}// _removeRByAddress
/**
 * @brief ZRandomFile::_removeRByAddress_Prepare  remove by address with return (record content).

    Returns the rank and the record content of the record corresponding to address pAddress and prepares to remove it.

* @param pDescriptor
 * @param pUserBuffer
 * @param pIdxCommit
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_removeRByAddress_Prepare( ZDataBuffer &pUserBuffer, zrank_type &pIdxCommit, const zaddress_type pAddress)
{
ZStatus wSt;
ZBlock wBlock;
zaddress_type wAddress;

//    wSt = _lockFile (ZLock_All ) ; //! lock Master File for update after having tested if already locked

//    if (wSt!=ZS_SUCCESS)        //! either file is already locked or a Severe IO error
//                return wSt;
    for (long wi = 0; wi < ZBAT.size();wi++)
    {
            if (ZBAT.Tab[wi].Address == pAddress)
            {
                pIdxCommit=wi;
                wSt=_getByRank (wBlock,pIdxCommit,wAddress);
                if (wSt!=ZS_SUCCESS)
                        return wSt;
                pUserBuffer = wBlock.Content ;

                _freeBlock_Prepare(pIdxCommit);
                CurrentRank = wi ;
                return ZS_SUCCESS;
            }// if
    }//for
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVADDRESS,
                            Severity_Severe,
                            "Given address %lld does not correspond to a valid start of block address in pool ZBAT for file %s",
                            pAddress,
                            URIContent.toString());
    return ZS_INVADDRESS;

}// _removeRByAddress_Prepare


/**
 * @brief ZRandomFile::_remove_Commit   common to all remove methods for accepting remove command

    block being deleted is transferred to free block pool.
    Its status (ZBS_State) is set to ZBS_Free.
    Block is then available to be re-used as a valid ZBAT pool entry.

 * @param pDescriptor
 * @param pIdxCommit    Input : ZBAT entry to remove
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_remove_Commit( const zrank_type pIdxCommit)
{
ZStatus wSt, wSt1;

    CurrentRank = -1 ;
    wSt=_freeBlock_Commit(pIdxCommit,false);

//    wSt1 = _unlockFile () ; // lock Master File for update after having tested if already locked

//    if (wSt1!=ZS_SUCCESS)        // either file is already locked or a Severe IO error
//                return wSt1;
    return wSt;
}// _remove_Commit

/**
 * @brief ZRandomFile::_remove_Rollback common to all remove methods for rejecting remove command and restoring situation
 * @param pDescriptor
 * @param pIdxCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_remove_Rollback(const zrank_type pIdxCommit)
{
     _freeBlock_Rollback(pIdxCommit);  // NB: Current index stays set because ZBAT rank is still active and last accessed
     return ZS_SUCCESS;
//     return _unlockFile ( );
}// _remove_Rollback


//---------------- Insert sequence ---------------------------------------------------------
//

ZStatus
ZRandomFile::_insert( const ZDataBuffer &pUserBuffer, zrank_type pRank, zaddress_type &pLogicalAddress)
{
ZStatus wSt;
//zrank_type wZBATIndex ;
zaddress_type wLogicalAddress=0;
    if ((wSt=_insert2Phases_Prepare(pUserBuffer,pRank,pLogicalAddress))!=ZS_SUCCESS)
                                return wSt;
    return _insert2Phases_Commit(pUserBuffer,pRank,wLogicalAddress);
}//_insert

ZStatus
ZRandomFile::_replace( const ZDataBuffer &pUserContent, const zrank_type pRank, zaddress_type &pAddress)
{
  ZDataBuffer wFormerRecord;
  ZStatus wSt = _removeR(wFormerRecord,pRank);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wSt=_insert(pUserContent,pRank,pAddress);
  if (wSt==ZS_SUCCESS) {
    return wSt;
  }
  /* if error, first rollback then return errored status */
  ZException.setMessage("ZRandomFile::_replace",wSt,Severity_Severe,"Cannot insert record at rank %ld file <%s>",
      pRank,URIContent.getBasename().toCChar());


  ZStatus wSt1= _insert(wFormerRecord,pRank,pAddress);
  if (wSt1!=ZS_SUCCESS) {
    utf8VaryingString wPrevious = ZException.popR()->formatUtf8();
    ZException.setMessage("ZRandomFile::_replace",wSt,Severity_Severe,"Cannot rollback (after errored insert) record at rank %ld file <%s>\n See first insert error in complement.",
        pRank,URIContent.getBasename().toCChar());
    ZException.last().setComplement(wPrevious);
    return wSt1;
  }
  return wSt;

}//_replace

ZStatus
ZRandomFile::_replaceErrored( const ZDataBuffer &pUserContent, const zrank_type pRank, zaddress_type &pAddress)
{
  ZBlock wBlock;

  /* some controls */

  if ((pRank > ZBAT.size())||(pRank < 0 )) {
    ZException.setMessage("ZRandomFile::_replace",ZS_OUTBOUND,Severity_Severe,"Record rank to replace <%ld> is out of ZBAT boundaries (size %ld).",
        pRank, ZBAT.size());
    return ZS_OUTBOUND;
  }

  /* search ZBAT for address */

  pAddress=  ZBAT[pRank].Address;

  /* if user content has same size, just replace and write as it is */
  if ((ZBAT[pRank].BlockSize-sizeof(ZBlockDescriptor_Export)) == pUserContent.Size) {
    wBlock.Content = pUserContent;
    return _writeBlockAt(wBlock,pAddress);
  }
  /* size is changed */

  /* request from free block pool without deleting ZBAT rank for a new block corresponding to BlockHeader size + user content size
   * delete former block : mark block as deleted send it to deleted blocks pool
   * use new block as current rank and write to file it with user content
   *
   */

  size_t wNeededSize = pUserContent.Size + sizeof(ZBlockHeader_Export);

  //  ZBlockDescriptor  wFormerBlock;

  long wFBTRank;

  /* prepare deleting former block */

  _freeBlock_Prepare(pRank);

  //  wFormerBlock = ZBAT[pRank];  /* save former block */


  wFBTRank=_getFreeBlockEngine(wNeededSize,-1);/* request a free block with exact wNeededSize size */

  if(wFBTRank < 0) {
    ZException.setMessage("ZRandomFile::_replace",ZS_CANTALLOCSPACE,Severity_Error,"Cannot get free space. Requested size %ld file %s",
        wNeededSize,URIContent.toCChar());

    _freeBlock_Rollback(pRank); /* restore from deleting block */
    return ZS_CANTALLOCSPACE;
  }

  _freeBlock_Commit(pRank,true) ; /* effective delete of former block : keep ZBAT entry available (do not delete entry in ZBAT Pool */

  ZBAT[pRank]=ZFBT[wFBTRank]; /* move allocated free block to former ZBAT element */
  ZFBT.erase(wFBTRank);       /* remove it from free block pool */

  ZBAT[pRank].State=ZBS_Used; /* set up block with appropriate infos */
  ZBAT[pRank].Pid=getpid();

  /* write new data to file */
  wBlock=ZBAT[pRank];
  wBlock.Content=pUserContent;
  return _writeBlockAt(wBlock,pAddress);

}//_replace

/**
 * @brief ZRandomFile::_insert2PhasesCommit_Prepare  Prepares to insert a record contained in pUserBuffer at rank pRank
            Insertion will be finalized after _insert2PhasesCommit_Commit.
            Returns the logical address of block being inserted.
            Block is marked as ZBS_Allocated (meaning locked in creation process).




 * @param pDescriptor
 * @param pUserBuffer
 * @param pRank           Wished rank to insert into
 * @param pZBATIndex      Internal commit index : rank of ZRandom file ZBAT table : effective allocated rank
 * @param pLogicalAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_insert2Phases_Prepare(const ZDataBuffer &pUserBuffer,
                                          zrank_type &pZBATIndex,
                                          zaddress_type &pLogicalAddress)
{
size_t wNeededSize = pUserBuffer.Size + sizeof(ZBlockHeader_Export);

  // open mode allows to write ?
  if (!(Mode & ZRF_Write_Only))  {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_MODEINVALID,
                                    Severity_Error,
                                    " File open mode <%s> does not allow record insertion file %s",
                                    decode_ZRFMode(Mode),
                                    URIContent.toString());
            return ZS_MODEINVALID;
  }

/*    wSt = _lockFile (ZLock_ReadWrite ) ; // lock Master File for update after having tested if already locked
    if (wSt!=ZS_SUCCESS)        // either file is already locked or a Severe IO error
                return wSt;
*/
    /* scan FreeTable for available block in holes,
     * or give the first address that corresponds to size
     * or extend the file according extentquota to get available space
     * and create ZBAT entry at pRank */

  long pFreeBlockRank =_getFreeBlockEngine (wNeededSize,-1);
  if (pFreeBlockRank <0)
    {return  ZException.getLastStatus();}

  /* if free block comes from a deleted block in free pool mark it specially (recover option with rollback) */
  if (ZFBT[pFreeBlockRank].State==ZBS_Deleted){
    ZFBT[pFreeBlockRank].State=ZBS_AllocFromDelete;
  }
  else
    ZFBT[pFreeBlockRank].State=ZBS_Allocated;

  ZBAT.insert(ZFBT[pFreeBlockRank],pZBATIndex);
//  ZBAT.Tab[pZBATIndex].State     = ZBS_Allocated ;
  ZBAT.Tab[pZBATIndex].Pid       = Pid;
  ZBAT.Tab[pZBATIndex].Lock      = 0;

  ZFBT.erase(pFreeBlockRank) ;

  if(__ZRFVERBOSE__)
    _DBGPRINT("ZRandomFile::_insert2Phases_Prepare prepare to insert block at %ld size %ld state %s to ZBAT rank %ld\n",
        ZBAT.Tab[pZBATIndex].Address,
        ZBAT.Tab[pZBATIndex].BlockSize,
        decode_ZBS(ZBAT.Tab[pZBATIndex].State),
        pZBATIndex)


  pLogicalAddress = setLogicalFromPhysical (ZBAT.Tab[pZBATIndex].Address) ;      //! gives back logical from physical address

  CurrentRank = pZBATIndex;

  return ZS_SUCCESS;
}//_insert2Phases_Prepare

/**
 * @brief ZRandomFile::_insert2PhasesCommit_Commit  update physically the file with block to insert and updates the file header accordingly
 * @param pDescriptor
 * @param pUserBuffer
 * @param pRank
 * @param pIdxCommit
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_insert2Phases_Commit( const ZDataBuffer &pUserBuffer, const zrank_type pZBATIndex,zaddress_type &pLogicalAddress)
{
ZStatus wSt;
ZBlock  wBlock;


//    ZBAT.Tab[pZBATIndex].State = ZBS_Used ;
    wBlock = ZBAT.Tab[pZBATIndex];
    wBlock.State = ZBS_Used ;
    wBlock.Content=pUserBuffer;
    if ((wSt=_writeBlockAt(wBlock,ZBAT.Tab[pZBATIndex].Address))!=ZS_SUCCESS) {
      ZException.last().setComplement("during _insert2Phases_Commit");
      return  wSt;
    }
    if(__ZRFVERBOSE__)
      _DBGPRINT("ZRandomFile::_insert2Phases_Commit-- commit inserting block rank %ld @ %ld size %ld \n",
          pZBATIndex,
          ZBAT.Tab[pZBATIndex].Address,
          ZBAT.Tab[pZBATIndex].BlockSize)

    ZFCB.UsedSize += wBlock.BlockSize ;

    if (ZBAT.Tab[pZBATIndex].State == ZBS_AllocFromDelete) { /* if free block comes from a deleted block */
      long wi=0 ;
      /* search block by address in deleted pool */
      for ( ; wi < ZDBT.count() ; wi++) {
        if (ZDBT.Tab[wi].Address==ZBAT.Tab[pZBATIndex].Address){
          break;
        }
      }// for
      if (ZDBT.Tab[wi].Address==ZBAT.Tab[pZBATIndex].Address){
        /* if found : remove it from pool */
        ZDBT.erase(wi);
      }
      else { /* this is totally abnormal and must be signalled in any case */
        fprintf(stderr,"ZRandomFile::_insert2Phases_Commit Allocated free block from deleted block address %ld has not been found in deleted block pool.\n", ZBAT.Tab[pZBATIndex].Address);
        std::cout.flush();
      }

    } //if (ZBAT.Tab[pZBATIndex].State == ZBS_AllocFromDelete)
    ZBAT.Tab[pZBATIndex].State = ZBS_Used ;


    pLogicalAddress = setLogicalFromPhysical( ZBAT.Tab[pZBATIndex].Address);
//    pAddress = ZBAT.Tab[pIdxCommit].Address;

    wSt=_writeFCB(true);
    return wSt;
 /*
    wSt=_writeFileDescriptor(true);
    if (wSt!=ZS_SUCCESS)
    {
        _unlockFile();
        return wSt;
    }
    return(_unlockFile());*/

}//_insert2Phases_Commit

ZStatus
ZRandomFile::_insert2Phases_Rollback( const zrank_type pZBATIndex)
{
ZStatus wSt;
ZBlockDescriptor wBS;

    wBS   = ZBAT.Tab[pZBATIndex];
    wBS.State       = ZBS_Free;
/*
    if (ZFCB.GrabFreeSpace) {
      if ((wSt=_grabFreeSpaceLogical(pZBATIndex,wBS))!=ZS_SUCCESS)
        return wSt;
    }
*/
    /* restore block status ex-ante */
    if (wBS.State == ZBS_AllocFromDelete)
      wBS.State = ZBS_Deleted;
    else
      wBS.State = ZBS_Free;


    ZFBT.push(wBS);            // create entry into ZFBT for freed Block
    ZBAT.erase(pZBATIndex);    // remove entry from ZBAT

    if ( _writeBlockHeader((ZBlockHeader&)wBS,wBS.Address)!=ZS_SUCCESS){     // write free block header to file
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_WRITEERROR,
          Severity_Severe,
          "Severe error while writing free block header at address %ld file %s",
          wBS.Address,
          URIContent.toString());

      return ZS_WRITEERROR;
    }


//    ZFCB.UsedSize -= wBS.BlockSize ;
    CurrentRank = -1;          // No more current index available

    return _writeFCB(false);
 //   return ZS_SUCCESS;
//    return _unlockFile();
}//_add2Phases_Rollback

//-----------End Insert sequence -----------------------------------
/**
 * @brief ZRandomFile::_writeBlockAt writes a whole block (Block descriptor AND user record content) at given address pAddress
 *
 *  converts ZBlockHeader to ZBlockHeader_Export and recomputes block sizes vs user content size + ZBlockHeader_Export size
 *
 * @param[inout] pDescriptor File descriptor
 * @param[in] pBlock    Full block content to write
 * @param[in] pAddress physical address to write the block at
 * @param[in] pModule a constant string that qualifies the calling module for error reporting purpose
 * @return
 */
ZStatus
ZRandomFile::_writeBlockAt(ZBlock &pBlock, const zaddress_type pAddress)
{
ZStatus wSt;

    pBlock.BlockSize = pBlock.Content.Size + sizeof(ZBlockHeader_Export);
    wSt=_writeBlockHeader((ZBlockHeader&)pBlock,pAddress);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

    ssize_t wSWrite= write(ContentFd,pBlock.Content.DataChar,pBlock.Content.Size);// then write block user content
    if (wSWrite <0 )
        {
        ZPMS.PMSCounterWrite(ZPMS_User,0);
        ZException.getErrno (errno,             // NB: write function sets errno in case of error
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEERROR,
                          Severity_Severe,
                          "Error while writing block user content (Data). Address %lld size %ld",
                          PhysicalPosition,
                          pBlock.BlockSize);

         return  wSt;
        }
    ZPMS.PMSCounterWrite(ZPMS_User,wSWrite);
    incrementPosition(wSWrite);

    fdatasync(ContentFd); // better than flush

    if (wSWrite < pBlock.Content.Size)
        {
        ZException.getErrno (errno,
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEPARTIAL,
                          Severity_Error,
                          "Block user content has been partially written on file : content address %lld size requested %ld size written %lld",
                          (pAddress+sizeof(ZBlockHeader_Export)),
                          pBlock.BlockSize,
                          wSWrite);

         return  ZS_WRITEPARTIAL;
        }

    return (ZS_SUCCESS);
}//_writeBlockAt


/**
 * @brief ZRandomFile::_writeBlockHeader for a physical record (block) writes the Header corresponding to this block at the beginning of the block
 *
 *
 * @param pDescriptor
 * @param pBlockHeader
 * @param pAddress
 * @param pModule
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_writeBlockHeader(ZBlockHeader &pBlockHeader,
                               const zaddress_type pAddress)
{

    ZStatus wSt = _seek(pAddress);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}

    ZBlockHeader_Export wBlockExp;

    ZBlockHeader::_exportConvert(pBlockHeader,&wBlockExp);

    ssize_t wSWrite= write(ContentFd,&wBlockExp,sizeof(ZBlockHeader_Export));

    if (wSWrite <0 )
        {
        ZPMS.PMSCounterWrite(ZPMS_BlockHeader,0);
        ZException.getErrno (errno,             // NB: write function sets errno in case of error
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEERROR,
                          Severity_Severe,
                          "Error while writing block header address %lld file <%s>",
                         pAddress,
                         URIContent.toString());
         return  ZS_WRITEERROR;
        }

    ZPMS.PMSCounterWrite(ZPMS_BlockHeader,wSWrite);

    PhysicalPosition += wSWrite;
    LogicalPosition += wSWrite;

    fdatasync(ContentFd); //! better than flush
    if (wSWrite<sizeof(ZBlockHeader_Export))
        {
        ZException.getErrno (errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEPARTIAL,
                             Severity_Error,
                             "Block header has been partially written on file : header address %lld size requested %ld size written %ld for file <%s>",
                             pAddress,
                             sizeof(ZBlockHeader_Export),
                             wSWrite,
                             URIContent.toString());
        return  ZS_WRITEPARTIAL;
        }

    return  ZS_SUCCESS;
}//_writeBlockHeader

//! @endcond

/**
 * @brief ZRandomFile::zgetBlockDescriptor gets the Block descriptor of the block pointed by its given rank pRank in the ZBlockAccessTable pool.
 *
 *              gets all physical information about the record given by pRank.
 *              returns a ZBlockdescriptor (ZBlockHeader plus Address) of a block given by pRank.
 *
 * @param[in] pRank
 * @param[out] pBlockDescriptor
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::zgetBlockDescriptor (const zrank_type pRank, ZBlockDescriptor &pBlockDescriptor)
{


ZStatus wSt;
    if ((wSt=testRank(pRank,_GET_FUNCTION_NAME_))!=ZS_SUCCESS)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   wSt,
                                   Severity_Error,
                                   "Invalid block rank value %ld. Out of file boundaries for file %s",
                                   pRank,
                                   URIContent.toString());
            return  (wSt);
            }

// got to manage locking there

//    setIndex(pRank);

    pBlockDescriptor = ZBAT.Tab[pRank];
    return  ZS_SUCCESS;

}// zgetBlockDescriptor

/**
 * @brief ZRandomFile::zgetFreeBlockDescriptor gets the Block descriptor of the Free block pointed by its given rank pRank in the ZFreeBlockPool pool.
 *
 *  Gets all physical information about the free block given by pRank.
 *  Returns a ZBlockdescriptor (ZBlockHeader plus Address) of a block given by pRank.
 *
 * @param[in] pRank
 * @param[out] pBlockDescriptor
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zgetFreeBlockDescriptor (const zrank_type pRank, ZBlockDescriptor &pBlockDescriptor)
{

ZStatus wSt =ZS_SUCCESS ;

         if (pRank<0)
                            wSt= ZS_OUTBOUNDLOW;
        if (pRank>ZFBT.lastIdx())
                            wSt= ZS_OUTBOUNDHIGH;

    if (wSt != ZS_SUCCESS)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   wSt,
                                   Severity_Error,
                                   "Invalid free block rank value %ld. Out of boundaries for file %s",
                                   pRank,
                                   URIContent.toString());
            return (wSt);
            }

//===== got to manage locking there ==================

//    setIndex(pRank);

    pBlockDescriptor = ZFBT.Tab[pRank];
    return  ZS_SUCCESS;


}// zgetFreeBlockDescriptor
/**
 * @brief ZRandomFile::zrecover  Recovers the deleted block in Free Pool pointed by pRank,
                                 creates an entry into Active Table ZBAT with all preserved content,
                                 ungrabs agglomerated free blocks if any
                                 returns the newly created index in active pool ZBAT in ZBATIdx.


 * @param pRank
 * @param pZBATIdx
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zrecover (const zrank_type pRank,  long &pZBATIdx)
{


ZStatus wSt =ZS_SUCCESS ;

    if (pRank<0)
              wSt= ZS_OUTBOUNDLOW;
    if (pRank>ZBAT.lastIdx())
              wSt= ZS_OUTBOUNDHIGH;

    if (wSt != ZS_SUCCESS)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   wSt,
                                   Severity_Error,
                                   "Invalid free block rank value %ld. Out of boundaries for file %s",
                                   pRank,
                                   URIContent.toString());
            return  (wSt);
            }

//========== got to manage locking there =============================

    if ((wSt=_recoverFreeBlock (pRank))!=ZS_SUCCESS)
                                   { return  wSt;}//

    pZBATIdx=ZBAT.lastIdx();
    return  ZS_SUCCESS;
}// zrecover

ZStatus
ZRandomFile::zget(void* pRecord, size_t &pSize, const zrank_type pRank)
{
ZStatus wSt;
ZBlock wBlock;
zaddress_type wAddress;

    if ((wSt=_getByRank(
                  wBlock,
                  pRank,
                  wAddress))!=ZS_SUCCESS)
                                    return wSt;
    memmove (pRecord,wBlock.DataChar(),wBlock.DataSize());
    pSize=wBlock.DataSize();
    return ZS_SUCCESS;
}// zget
/**
 * @brief ZRandomFile::zget returns a user content record corresponding to its rank given by pRank.
        Eventually locks the block read using pLock lock mask
            @see _get
                  Returns a ZStatus :
       - ZS_SUCCESS if everything went well
       - ZS_BOUNDHIGH if pRank is outside (high) the active block pool index set. (corresponds to EOF)
       - ZS-BOUNDLOW if pRank is outside (low) the active block pool index set
       - Other low level errors : see @ref ZRandomFile::_read() for other possible status
 * @param[out] pRecord a ZDataBuffer that will contain the record content in return
 * @param[in] pRank File record relative position (rank)
 * @param pLock ZLockMask_type that will be applied to record (default is ZLock_Nolock). In case of lock by another process/user, Current lock mask is returned
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zget(ZDataBuffer &pRecordContent, const zrank_type pRank)
{
zaddress_type wAddress;
    ZStatus wSt;
    ZBlock wBlock;
    if ((wSt=_getByRank(
                  wBlock,
                  pRank,
                  wAddress))!=ZS_SUCCESS)
                {
                return wSt;
                }
    pRecordContent=wBlock.Content;

    return(ZS_SUCCESS);
}// zget
/**
 * @brief ZRandomFile::zgetWAddress gets user's record at rank pRank, and returns block address
 * @param[out] pRecord  User Record
 * @param[in] pRank     rank of record to get
 * @param[out] pAddress address of block for record
 * @param [in] pLock    Lock mask
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zgetWAddress(ZDataBuffer &pRecord, const zrank_type pRank, zaddress_type &pAddress)
{
ZStatus wSt;
ZBlock wBlock;
    if ((wSt=_getByRank(
                  wBlock,
                  pRank,
                  pAddress))!=ZS_SUCCESS)
                {
                return wSt;
                }
    pRecord=wBlock.Content;
    return(ZS_SUCCESS);
}// zgetWAddress

ZStatus
ZRandomFile::zgetNext (ZDataBuffer &pUserRecord)
{
ZStatus         wSt;
ZBlock          wBlock;
zaddress_type   wAddress;
zrank_type      wRank;

    if ((wSt=_getNext(
                      wBlock,
                      wRank,
                      wAddress))!=ZS_SUCCESS)
                                        return wSt;
    pUserRecord=wBlock.Content;
    return(ZS_SUCCESS);
}//zgetNext
/**
 * @brief ZRandomFile::zgetNextWAddress gets the next record from current position and returns user's content.
 *  @note memory is allocated to pUserRecord in order to hold user's record content. It is up to caller to deallocate memory.
 *
 * @param[in] pDescriptor file descriptor
 * @param[out] pUserRecord   user record returned
 * @param[out] pSize    block returned
 * @param[out] pRank    rank of the block read
 * @param[in] pLock     Locks the block read with this mask.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zgetNext (void* pUserRecord, size_t &pSize)
{


ZStatus         wSt;
ZBlock          wBlock;
zaddress_type   wAddress;
zrank_type      wRank;

    if ((wSt=_getNext(wBlock,
                      wRank,
                      wAddress))!=ZS_SUCCESS)
                                       { return  wSt; }//
    if ((pUserRecord=malloc (wBlock.DataSize()))==nullptr)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_MEMERROR,
                         Severity_Fatal,
                         "Cannot allocate memory to record buffer");
        ZException.exit_abort();
        }
    memmove(pUserRecord,wBlock.DataChar(),wBlock.DataSize());
    pSize=wBlock.DataSize();

    return  (ZS_SUCCESS);
}//zgetNext
/**
 * @brief ZRandomFile::zgetNextWAddress gets the next record from current position and returns user's content as well as its address.
 * @param[inout] pDescriptor file descriptor
 * @param[out] pBlock   block returned
 * @param[out] pRank    rank of the block read
 * @param[out] pAddress address of the block read
 * @param[in] pLock     Locks the block read with this mask.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zgetNextWAddress(ZDataBuffer &pRecord, zrank_type &pRank, zaddress_type &pAddress)
{
ZStatus wSt;
ZBlock wBlock;
    if ((wSt=_getNext(wBlock,
                      pRank,
                      pAddress))!=ZS_SUCCESS)
                        {
                        return wSt;
                        }
    pRecord=wBlock.Content;
    return(ZS_SUCCESS);
}// zgetNextWAddress



ZStatus
ZRandomFile::zgetPreviousWAddress(ZDataBuffer &pRecord, zrank_type &pRank,zaddress_type &pAddress)
{
ZStatus wSt;
ZBlock wBlock;
    pRank=decrementRank();
    if (pRank<0)
            return ZS_OUTBOUNDLOW;
    if ((wSt=_getByRank(
                  wBlock,
                  pRank,
                  pAddress))!=ZS_SUCCESS)
                {
                return wSt;
                }
    pRecord=wBlock.Content;
    return(ZS_SUCCESS);
}// zgetPreviousWAddress

/**
 * @brief ZRandomFile::_getNext obtains the record following current ZDescriptor record. If already at last record rank, returns ZS_EOF.
 * @param[inout] pDescriptor file descriptor
 * @param[out] pBlock   block returned
 * @param[out] pRank    rank of the block read
 * @param[out] pAddress address of the block read
 * @param[in] pLock     Locks the block read with this mask.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *          ZS_EOF if trying to read next record after last file's record.
 */
ZStatus
ZRandomFile::_getNext(ZBlock &pBlock,
                      zrank_type &pRank,
                      zaddress_type &pAddress)
{

    if (incrementRank()<0)
      return ZS_EOF;
    pAddress = getCurrentPhysicalAddress();
    pRank = getCurrentRank();
    return _readBlockAt(pBlock,pAddress);
} // _getNext


/**
 * @brief ZRandomFile::zgetByAddress gets a user record content by its block logical address
 * @param pRecord
 * @param pAddress
 * @param pLock
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zgetByAddress (ZDataBuffer&pRecord, zaddress_type pAddress)
{
ZStatus wSt;
ZBlock  wBlock;
//zaddress_type wLogical = setPhysicalFromLogical(pAddress);
    if ((wSt=_readBlockAt(
                          wBlock,
                          pAddress))!=ZS_SUCCESS)
                                return wSt;

    pRecord = wBlock.Content ;
    return ZS_SUCCESS ;
}//zgetByAddress


/**
 * @brief ZRandomFile::zgetSingleField gets a portion of user record content by its offset from beginning of user record and length
 *
 * @param pFieldContent     ZDataBuffer containing the field data   (Returned)
 * @param pRank             record rank in used pool (ZBAT)         (Input)
 * @param pOffset           Field offset from beginning of record   (Input)
 * @param pLength           Field length                            (Input/output)
 * @param pLock             Lock mask                               (Input)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zgetSingleField (ZDataBuffer& pFieldContent, const zrank_type pRank, const ssize_t pOffset, const ssize_t &pLength, const int pLock)
{

    if ((pRank>ZBAT.size())||(pRank<0))
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_OUTBOUND,
                                Severity_Severe,
                                "Invalid relative record number <%ld> while Used pool size is [0,<%ld>]",
                                pRank,
                                ZBAT.lastIdx());
        return  ZS_OUTBOUND;
    }

//     zaddress_type wLogical = setPhysicalFromLogical(pAddress);

    zaddress_type wFieldAddress = ZBAT.Tab[pRank].Address + sizeof(ZBlockHeader_Export) + pOffset ;

    off_t wOff = lseek(ContentFd,(off_t)( wFieldAddress),SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning file  at logical address <%lld> file <%s>",
                                 wFieldAddress,
                                 URIContent.toString()
                                 );
                return (ZS_FILEPOSERR);
                }

    // then just read from file the segment of data on field length
    return  _read(           // allocate pLength for field buffer, reads then manage errors if ever - updates stats
                 pFieldContent,
                 pLength,
                 ZPMS_Field);

} // zgetSingleField


/**
 * @brief ZRandomFile::zwriteSingleField Writes a portion of user record content by its offset from beginning of user record and length
 *
 * @param pFieldContent     ZDataBuffer containing the field data   (input)
 * @param pRank             record rank in used pool (ZBAT)         (Input)
 * @param pOffset           Field offset from beginning of record   (Input)
 * @param pLength           Field length                            (Input/output)
 * @param pLock             Lock mask                               (Input/output)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zwriteSingleField (ZDataBuffer& pFieldContent,
                                const zrank_type pRank,
                                const ssize_t pOffset,
                                const ssize_t &pLength,
                                int &pLock)
{

    if ((pRank>ZBAT.size())||(pRank<0))
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVADDRESS,
                                Severity_Severe,
                                "Invalid relative record number <%ld> while Used pool size is <%ld>",
                                pRank,
                                ZBAT.size());
        return  ZS_INVADDRESS;
    }

//     zaddress_type wLogical = setPhysicalFromLogical(pAddress);

    zaddress_type wFieldAddress = ZBAT.Tab[pRank].Address + sizeof(ZBlockHeader_Export) + pOffset ;

    off_t wOff = lseek(ContentFd,(off_t)( wFieldAddress),SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning file  at logical address <%lld> file <%s>",
                                 wFieldAddress,
                                 URIContent.toString()
                                 );
                return (ZS_FILEPOSERR);
                }

    // then just read from file the segment of data on field length
    return  _read(           // allocate pLength for field buffer, reads then manage errors if ever - updates stats
                 pFieldContent,
                 pLength,
                 ZPMS_Field);

} // zwriteSingleField







/**
 * @brief ZRandomFile::zaddWithAddress adds the user record contained in pRecord in ZRF and returns its address  in given zaddress_type pAddress field.
 * @param[in] pRecord a ZDataBuffer containing user's record flat data
 * @param[out] pAddress returning address for the added record if successful
 * @param[in] pLock lock mask
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zaddWithAddress (ZDataBuffer&pRecord, zaddress_type &pAddress)
{
    return(_add(pRecord,pAddress));;
}



/**
 * @brief ZRandomFile::zgetLast returns the record at the last logical position (rank) within the file
 * @note after the operation file descriptor's current index is set to the last rank
 * @param[out] pRecord returned user's record
 * @param[out] pRank   returned rank of the last record
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zgetLast (ZDataBuffer &pRecord,zrank_type &pRank)
{


    if (ZBAT.isEmpty())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Error,
                                    " File is empty. Cannot get last record for file <%s>",
                                    URIContent.toString());
             return  ZS_OUTBOUND;
             }
     CurrentRank = ZBAT.lastIdx();
     pRank= CurrentRank;
     return  zget(pRecord,ZBAT.lastIdx());
}//zgetLast
/**
 * @brief ZRandomFile::zgetLast returns the record at the last logical position (rank) within the file
 * @note after the operation file descriptor's current index is set to the last rank
 * @param[out] pRecord  returned user's record
 * @param[out] pRank    Rank of the last record in used block pool (returned)
 * @param[out] pAddress Address of the last block in used block pool (returned)
 * @return  a ZStatus.  In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zgetLastWAddress (ZDataBuffer &pRecord, zrank_type &pRank, zaddress_type& pAddress)
{

    if (ZBAT.isEmpty())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Error,
                                    " File is empty. Cannot get last record for file <%s>",
                                    URIContent.toString());
             return  ZS_OUTBOUND;
             }
     CurrentRank = ZBAT.lastIdx();
     pRank = CurrentRank;
     return  zgetWAddress(pRecord,CurrentRank,pAddress);
}//zgetLast
/**
 * @brief ZRandomFile::zgetPrevious returns the previous record from the current logical position (rank) within the file
 * @note after the operation file descriptor's current index is set to the previous record's rank
 * @param[out] pRecord returned user's record
 * @param[in] pLock Lock mask
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *          ZS_OUTBOUNDLOW if trying to read previous record while already at first file's record.
 */
ZStatus
ZRandomFile::zgetPrevious (ZDataBuffer &pRecord,const int pLock)
{

    if (CurrentRank==0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUNDLOW,
                                    Severity_Error,
                                    "Already at first record : cannot get Previous record for file <%s>",
                                    URIContent.toString());
             return  ZS_OUTBOUNDLOW;
             }
    CurrentRank--;
    return  zget(pRecord,CurrentRank);
}//zgetPrevious


/**
 * @brief ZRandomFile::_get returns a block record corresponding to its rank given by pRank.
 *                          Eventually locks the block read using pLock lock mask
 *
 *                 Returns a ZStatus :
 *      - ZS_SUCCESS if everything went well
 *      - ZS_BOUNDHIGH if pRank is outside (high) the active block pool index set. (corresponds to EOF)
 *      - ZS-BOUNDLOW if pRank is outside (low) the active block pool index set
 *
 * @param[in] pDescriptor   File descriptor
 * @param[out] pBlock       Block content returned
 * @param[in] pRank         rank of the block to read from file (given rank)
 * @param[out] pAddress     physical address of the block in file (returned)
 * @param[in] pLock         RFFU lock mask
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_getByRank(ZBlock &pBlock,
                  const long pRank,
                  zaddress_type &pAddress)
{


ZStatus wSt;
    if ((wSt=testRank(pRank,_GET_FUNCTION_NAME_))!=ZS_SUCCESS)
                {
                ZException.addToLast("| Wrong status getting block rank %ld status %s for file %s",
                                       pRank,
                                       decode_ZStatus(wSt),
                                       URIContent.toString());

                return (wSt); // can be out of boundaries (program issue) OR locked for deletion - locked for creation (concurrent access)
                }

    setRank(pRank);
// should test lock here
/*    if (pLock!=ZLock_Nolock)
                _lock(pRank,pLock,false);
*/
    pAddress = ZBAT.Tab[pRank].Address;
    return  (_readBlockAt(pBlock,
                        ZBAT.Tab[pRank].Address));

}// _getByRank

/**
 * @brief ZRandomFile::_getByAddress  search by Address pAddress a block and returns the whole block content.
 *
 * Returns ZS_NOTFOUND if address has not been found or Appropriate status if an error occurred or if there was a lock on the block.
 *
 * @param[in] pDescriptor
 * @param[out] pBlock
 * @param[in] pAddress
 * @return
 */
ZStatus
ZRandomFile::_getByAddress (ZBlock &pBlock,const zaddress_type pAddress)
{
 /*
long wi;

   for (wi=0;wi<ZBAT.size();wi++)
            {
            if (ZBAT.Tab[wi].Address == pAddress)
                    {
                    pRank = wi;
                    break;
                    }
            }// for
    if (wi == ZBAT.size())
                            return ZS_NOTFOUND;
*/
return(_readBlockAt(pBlock,
                    pAddress));
}// _getByAddress




ZStatus
ZRandomFile::zinsert(const ZDataBuffer &pRecord, zrank_type pRank)
{

zaddress_type wAddress;

    return _insert(pRecord,pRank,wAddress);

}// zinsert

ZStatus
ZRandomFile::zreplace(const ZDataBuffer &pRecord, zrank_type pRank)
{

  zaddress_type wAddress;

  return _replace(pRecord,pRank,wAddress);

}// zreplace


//! @cond Development
/**
 * @brief ZRandomFile::_ZRFopen Low level open file routine
 *  open mode : open mode
 *              lock mode
 *                  with recordlock
 * @par open actions
 *   - sets up ZFileDescriptor
 *      + associates ZFCB, ZBAT and ZFBT to ZFileDescriptor
 *      + clears block pointers
 *      + clears CurrentBlockHeader
 *   - loads ZFileHeader
 *   - loads ZReserved block
 *   - loads ZFileControlBlock
 *
 *   - controls file lock state
 *
 * @see ZRFLockGroup
 *
 *
 * @param[in] pMode          a ZRFMode_type defining the open mode
 * @param[in] pFileType      a ZFile_type defining the type of file to open.
 *  ZFT_Any allows to open as a ZRandomFile any file with type [ZFT_ZRandomFile,ZFT_ZMasterFile,ZFT_ZIndexFile].
 *  if file type is not one of these types, ZS_BADFILETYPE is returned and file is not open.
 *
 * @return  a ZStatus. In case of error or warning,
 *  ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *
 *errors :
 *    ZS_INVOP :        file is already open when trying to open it
 *    ZS_FILENOTEXIST : file does not exist while it must exist for requested open mode
 *    ZS_ERROPEN :    low level file I/O error : errno is captured and system message is provided.
 *    ZS_LOCKED :     file is locked
 *    ZS_BADFILETYPE : file type is not what expected in pFileType
 *    ZS_MODEINVALID : open mode is incompatible with requested operation or open mode is not allowed by file lock.
 *
 *warning :
 *     ZS_FILETYPEWARN :open mode is not the requested one for  operation but open mode is forced.
 *
*/
ZStatus
ZRandomFile::_ZRFopen(const zmode_type pMode, const ZFile_type pFileType, bool pLockRegardless)
{
ZStatus wSt=ZS_SUCCESS;

    if (__ZRFVERBOSE__)
      _DBGPRINT("ZRandomFile::_open-I-OPENNING Openning file <%s>.\n",URIContent.toCChar()) // debug

    if (_isOpen) {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Error,
                              "Cannot open <%s>.It is already open with mode <%s>",
                              URIContent.toString(),
                              decode_ZRFMode(Mode));
      return  (ZS_INVOP);
    }
    CurrentRank = -1;

    ZPMS.clear();                   // reset performance data collection

    if (!URIContent.exists()) {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_FILENOTEXIST,
                               Severity_Error,
                               "File <%s> does not exist. It must exist to be opened. Use zcreate with appropriate options.",
                               URIContent.toString());
        return  (ZS_FILENOTEXIST);
    }

    ContentFd = open(URIContent.toCChar(),O_RDWR);

   if (ContentFd<0) {
       ZException.getErrno(errno,
                       _GET_FUNCTION_NAME_,
                       ZS_ERROPEN,
                       Severity_Severe,
                       "Error opening file <%s> ",
                       URIContent.toCChar());
       return  (ZS_ERROPEN);
   }

    wSt = generateURIHeader (URIContent,URIHeader);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}
    HeaderFd = open(URIHeader.toCChar(),O_RDWR);
    if (HeaderFd < 0) {
      ZException.getErrno(errno,
                      _GET_FUNCTION_NAME_,
                      ZS_ERROPEN,
                      Severity_Severe,
                      "Error opening header file <%s>. ZRF file has not been opened.",
                      URIHeader.toString());
      _isOpen=false;
      HeaderAccessed = ZHAC_Nothing;
      close(ContentFd);

      return  (ZS_ERROPEN);
    }

//    setupFCB();  // update pDescriptor
  wSt=_importAllFileHeader();  // get header and force read pForceRead = true, whatever the open mode is
  if (wSt!=ZS_SUCCESS)
    {return  wSt;}
    if (!pLockRegardless)
      {
        if (ZHeader.Lock & ZRF_Exclusive)
        {
          _forceClose();
          ZException.setMessage(_GET_FUNCTION_NAME_,
              ZS_LOCKED,
              Severity_Error,
              "file %s is locked. lock mask is %s. File has not been opened.",
              URIContent.toString(),
              decode_ZLockMask (ZHeader.Lock).toChar()
              );
          return  ZS_LOCKED;
        }
    } //if (!pLockRegardless)
//-----------------------------File type test------------------------------------

    if (pFileType == ZFT_Nothing)
        {
        _forceClose();

        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_BADFILETYPE,
                                Severity_Error,
                                "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                URIContent.toString(),
                                decode_ZFile_type (pFileType),
                                decode_ZFile_type (ZHeader.FileType));
        return  ZS_BADFILETYPE;
        }

  if (pFileType == ZFT_Any)
    {
      switch (ZHeader.FileType)
      {
        case ZFT_ZRandomFile :
        case ZFT_ZMasterFile :
        case ZFT_ZRawMasterFile :
        case ZFT_ZIndexFile :
        case ZFT_DictionaryFile :
          break;
        default:
          _forceClose();
          ZException.setMessage(_GET_FUNCTION_NAME_,
              ZS_BADFILETYPE,
              Severity_Error,
              "file %s cannot be opened : invalid/unknown file type <%X %s> while expected one of  [ZFT_ZRandomFile,ZFT_ZMasterFile,ZFT_ZIndexFile]",
              URIContent.toCChar(),
              pFileType,
              decode_ZFile_type (pFileType));
          return ZS_BADFILETYPE;
      }// switch
    }
    else
    {
    switch (ZHeader.FileType)
      {
      case ZFT_ZRandomFile :
            {
            if (pFileType==ZFT_ZRandomFile)
                                {
                                wSt=ZS_SUCCESS;
                                break; // everything OK
                                }
            if ((pFileType==ZFT_ZMasterFile))   // file will be upgraded must be opened is ZRF_Exclusive
                {
                if (!(pMode & ZRF_Exclusive)) // Only ZRF_Exclusive for opening a ZMasterFile as ZRandomFile
                    {
                    _forceClose();
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_MODEINVALID,
                                            Severity_Error,
                                            "file %s is of type <%s> and cannot be opened as type <%s> for upgrade whitout being opened in mode ZRF_Exclusive ",
                                            URIContent.toString(),
                                            decode_ZFile_type (ZHeader.FileType),
                                            decode_ZFile_type (pFileType));
                    return  ZS_MODEINVALID;
                    }
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_FILETYPEWARN,
                                        Severity_Warning,
                                        "file %s has been opened but given type is <%s> while expected <%s>",
                                        URIContent.toString(),
                                        decode_ZFile_type (pFileType),
                                        decode_ZFile_type (ZHeader.FileType));
                wSt=ZS_FILETYPEWARN;// OK Open but emit a Warning
                break;
                }
            // all other cases are errored
            _forceClose();
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                    URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (ZHeader.FileType));
            return  ZS_BADFILETYPE;

            }// case ZFT_ZRandomFile

        case ZFT_ZRawMasterFile :
        case ZFT_ZMasterFile : {
          if ((pFileType==ZFT_ZMasterFile)||(pFileType==ZFT_ZRawMasterFile)) {
            wSt=ZS_SUCCESS;
            break; // everything in line
          }
          if (pFileType==ZFT_ZRandomFile) {
            if ((pMode & ZRF_Write_Only)||(pMode & ZRF_Delete_Only)) { // Only ZRF_Read_Only - delete and write mode excluded
              _forceClose();
              ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_MODEINVALID,
                                            Severity_Error,
                                            "file %s is of type <%s> and cannot be opened as type <%s> with mode<%s>",
                                            URIContent.toString(),
                                            decode_ZFile_type (ZHeader.FileType),
                                            decode_ZFile_type (pFileType),
              decode_ZRFMode(pMode));
              return  ZS_MODEINVALID;
            }
            // up to here file is opened as ZRF_Read_Only
            wSt=ZS_SUCCESS;
            break;
          } // if (pFileType==ZFT_ZRandomFile)
          // all other cases are errors
          _forceClose();
          ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                    URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (ZHeader.FileType));
            return  ZS_BADFILETYPE;
        }// ZFT_ZMasterFile or ZFT_ZRawMasterFile

        case ZFT_DictionaryFile : {
          if (pFileType==ZFT_DictionaryFile) {
            wSt=ZS_SUCCESS;
            break; // everything in line
          }
          if (pFileType==ZFT_ZRandomFile) {
            if ((pMode & ZRF_Write_Only)||(pMode & ZRF_Delete_Only)) { // Only ZRF_Read_Only - delete and write mode excluded
              _forceClose();
              ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_MODEINVALID,
                                            Severity_Error,
                                            "file %s is of type <%s> and cannot be opened as type <%s> with mode<%s>",
                                            URIContent.toString(),
                                            decode_ZFile_type (ZHeader.FileType),
                                            decode_ZFile_type (pFileType),
                                            decode_ZRFMode(pMode));
              return  ZS_MODEINVALID;
            }
            // up to here file is opened as ZRF_Read_Only
            break;
          }
          // all other cases are errors
          _forceClose();
          ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                    URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (ZHeader.FileType));
          return  ZS_BADFILETYPE;
          }// ZFT_DictionaryFile
        case ZFT_ZIndexFile : {
              if (pFileType==ZFT_ZIndexFile) {
                wSt=ZS_SUCCESS;
                break; // everything in line
              }
              if (pFileType==ZFT_ZRandomFile) {
                if ((pMode & ZRF_Write_Only)||(pMode & ZRF_Delete_Only)) // Only ZRF_Read_Only - delete and write mode excluded
                {
                  _forceClose();
                  ZException.setMessage(_GET_FUNCTION_NAME_,
                      ZS_MODEINVALID,
                      Severity_Error,
                      "file %s is of type <%s> and cannot be opened as type <%s> with mode<%s>",
                      URIContent.toString(),
                      decode_ZFile_type (ZHeader.FileType),
                      decode_ZFile_type (pFileType),
                      decode_ZRFMode(pMode));
                  return  ZS_MODEINVALID;
                }
                // up to here file is opened as ZRF_Read_Only

                ZException.setMessage(_GET_FUNCTION_NAME_,
                    ZS_FILETYPEWARN,
                    Severity_Warning,
                    "file %s has been opened but given type is <%s> while expected <%s>",
                    URIContent.toString(),
                    decode_ZFile_type (pFileType),
                    decode_ZFile_type (ZHeader.FileType));
                wSt=ZS_FILETYPEWARN; // OK Open but emit a Warning
                break;
              }
              // all other cases are errors
              _forceClose();
              ZException.setMessage(_GET_FUNCTION_NAME_,
                  ZS_BADFILETYPE,
                  Severity_Error,
                  "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                  URIContent.toString(),
                  decode_ZFile_type (pFileType),
                  decode_ZFile_type (ZHeader.FileType));
              return  ZS_BADFILETYPE;
            }// ZFT_ZIndexFile
    default:
        {
        // all other cases are errors
        _forceClose();
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_BADFILETYPE,
                                Severity_Error,
                                "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                URIContent.toString(),
                                decode_ZFile_type (pFileType),
                                decode_ZFile_type (ZHeader.FileType));
        return  ZS_BADFILETYPE;
        }
    }//switch
  }//if (pFileType != ZFT_Any)

//-----------------------------End File type test------------------------------------

    _isOpen = true ;
    Mode = pMode;
/*
    if (pMode & ZRF_Exclusive)
            {
            wSt=_lockFile(ZRF_Exclusive,true);
            }
*/
//=======Open pool management=========================

    ZRFPool->addOpenFile(this);

    return  wSt;  // do not write file descriptor
//    return(_writeFileDescriptor(true));  // force to write file descriptor whatever open mode is (pForceRead = true)
}//_open

ZStatus
ZRandomFile::_baseOpen(const zmode_type pMode, const ZFile_type pFileType, bool pLockRegardless)
{
  ZStatus wSt=ZS_SUCCESS;
  if (_isOpen)
  {
    ZException.setMessage("ZRandomFile::_baseOpen",
        ZS_INVOP,
        Severity_Error,
        "Cannot open <%s>.It is already open with mode <%s>",
        URIContent.toString(),
        decode_ZRFMode(Mode));
    return  (ZS_INVOP);
  }
  CurrentRank = -1;

  ZPMS.clear();                   // reset performance data collection

  if (!URIContent.exists())
  {
    ZException.setMessage("ZRandomFile::_baseOpen",
        ZS_FILENOTEXIST,
        Severity_Error,
        "File <%s> does not exist. It must exist to be opened. Use zcreate with appropriate options.",
        URIContent.toString());
    return  (ZS_FILENOTEXIST);
  }

  ContentFd = open(URIContent.toCChar(),O_RDWR);

  if (ContentFd<0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        "Error opening file <%s> ",
        URIContent.toCChar());
    return  (ZS_ERROPEN);
  }

  wSt = generateURIHeader (URIContent,URIHeader);
  if (wSt!=ZS_SUCCESS)
  {return  wSt;}
  HeaderFd = open(URIHeader.toCChar(),O_RDWR);
  if (HeaderFd < 0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        "Error opening header file <%s>. ZRF file has not been opened.",
        URIHeader.toString());
    _isOpen=false;
    HeaderAccessed = ZHAC_Nothing;
    close(ContentFd);

    return  (ZS_ERROPEN);
  }

  //    setupFCB();  // update pDescriptor
  wSt=_importAllFileHeader();  // get header and force read pForceRead = true, whatever the open mode is
  if (wSt!=ZS_SUCCESS)
  {return  wSt;}
  if (!pLockRegardless)
  {
    if (ZHeader.Lock & ZRF_Exclusive)
    {
      _forceClose();
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_LOCKED,
          Severity_Error,
          "file %s is locked. lock mask is %s. File has not been opened.",
          URIContent.toString(),
          decode_ZLockMask (ZHeader.Lock).toChar()
          );
      return  ZS_LOCKED;
    }
  } //if (!pLockRegardless)

  _isOpen = true ;
  Mode = pMode;
  /*
    if (pMode & ZRF_Exclusive)
            {
            wSt=_lockFile(ZRF_Exclusive,true);
            }
*/
  //=======Open pool management=========================

  ZRFPool->addOpenFile(this);

  return  wSt;  // do not write file descriptor
  //    return(_writeFileDescriptor(true));  // force to write file descriptor whatever open mode is (pForceRead = true)
}//_baseOpen


ZStatus
ZRandomFile::zclose()
{
ZStatus wSt;

  if (!_isOpen)
      {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Error,
                              " File is not open while requesting to close. File <%s>",
                              URIContent.toString());
      return ZS_INVOP;
      }

//  wSt=_unlockFile(false);
/*
  ZHeader.Lock = ZLock_Nolock;
  Mode = ZRF_Nothing;
*/
//    _unlockAll(false);



  wSt = _writeAllFileHeader();
  if (wSt!=ZS_SUCCESS)
                return (wSt);

    wSt= ZFileDescriptor::_close();
/*
    close(ContentFd);
    close(HeaderFd);
    _isOpen = false;

//=====================File pool management======================

    ZRFPool->removeFileByFd(ContentFd);

    clearPartial();
*/
    return  wSt;
}//_close

/** !@endcond  */ // Development

ZStatus
ZFileDescriptor::testRank(zrank_type pRank, const char* pModule)
{
    if (pRank<0)
        {
        ZException.setMessage(pModule,
                                ZS_OUTBOUNDLOW,
                                Severity_Error,
                                " invalid block rank number <%d> . Boundaries are [0,<%ld>]",
                                pRank,
                                ZBAT.lastIdx());
         return ZS_OUTBOUNDLOW;
         }
    if (pRank>ZBAT.lastIdx())
        {
        ZException.setMessage(pModule,
                                ZS_OUTBOUNDHIGH,
                                Severity_Error,
                                " invalid block rank number <%d> . Boundaries are [0,<%ld>]",
                                pRank,
                                ZBAT.lastIdx());
         return ZS_OUTBOUNDHIGH;
         }
   if (ZBAT.Tab[pRank].State==ZBS_Allocated)
        {
          ZException.setMessage(pModule,
                                  ZS_LOCKCREATE,
                                  Severity_Error,
                                  " Block record is currently being locked for creation by another stream");
          return ZS_LOCKCREATE ;
         }

    return ZS_SUCCESS;
}

//
//--------------Lock management------------------
//
#ifdef __COMMENT__
ZStatus
ZRandomFile::_lockFile (const  zlockmask_type pLock, bool pForceWrite)
{
ZStatus wSt;

    if ((!pForceWrite)&&(Mode&ZRF_Exclusive))
        {
        ZHeader.Lock = pLock;
        ZHeader.LockOwner = Pid;
        return  ZS_SUCCESS;
        }

lockPack wLockInfos;

//    wSt= _getFileHeader(pForceWrite); // pForceWrite = pForceRead
    wSt=_readFileLock(wLockInfos);
    if (wSt!=ZS_SUCCESS)
                    { return  wSt;}
    ZHeader.Lock = wLockInfos.Lock;
    ZHeader.LockOwner = wLockInfos.LockOwner;

    if (ZHeader.Lock != ZLock_Nolock)
    {
        if (ZHeader.LockOwner!=Pid)
        {
          ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_LOCKED,
                                  Severity_Error,
                                  " File is locked by another pid : <%X %d>  (current pid <%X %d>) while trying to lock it. File <%s>  lock mask is <%X %s>",
                                  ZHeader.LockOwner,ZHeader.LockOwner,
                                  Pid,Pid,
                                  URIHeader.toString(),
                                  ZHeader.Lock,
                                                    decode_ZLockMask (ZHeader.Lock).toChar());
        return  ZS_LOCKED;
        }
    }
    ZHeader.Lock = wLockInfos.Lock = pLock;
    ZHeader.LockOwner=wLockInfos.LockOwner= Pid;

    return  _writeFileHeader(pForceWrite);
}// _lockFile

#endif// __COMMENT__

ZStatus
ZRandomFile::zutilityUnlockZRF (const uriString& pContentFile)
{
  if (!pContentFile.exists())
  {
    ZException.setMessage("ZRandomFile::zutilityUnlockZRF",ZS_FILENOTEXIST,Severity_Error,
        "Given file path <%s> does not exist,",pContentFile.toCChar());
    return ZS_FILENOTEXIST;
  }

  uriString wURIHeader;
  ZStatus wSt= generateURIHeader(pContentFile, wURIHeader);
  if (wSt!=ZS_SUCCESS) return wSt;

  return zutilityUnlockHeaderFile(wURIHeader);
}

ZStatus
ZRandomFile::zutilityUnlockHeaderFile (const uriString& pHeaderFile)
{
  if (!pHeaderFile.exists())
    {
    ZException.setMessage("ZRandomFile::zutilityUnlockHeaderFile",ZS_FILENOTEXIST,Severity_Error,
                          "Given file path <%s> does not exist,",pHeaderFile.toCChar());
    return ZS_FILENOTEXIST;
    }
  if (!pHeaderFile.getFileExtension().compareV<char>(__HEADER_FILEEXTENSION__))
      fprintf (stderr,"ZRandomFile::zutilityUnlockHeaderFile-W-FILEXT Warning file <%s> has not header file extension <%s>. Proceeding anyway.",pHeaderFile.toCChar(),__HEADER_FILEEXTENSION__);

//  ZDataBuffer wHeaderContent;
//  pHeaderFile.loadContent(wHeaderContent);

  int wHeaderFd = open(pHeaderFile.toCChar(),O_RDWR);
  if (wHeaderFd < 0)
    {
      ZException.getErrno(errno,"ZRandomFile::zutilityUnlockHeaderFile",
          ZS_ERROPEN,
          Severity_Severe,
          "Error opening header file <%s>. ZRF file has not been opened.",
          pHeaderFile.toCChar());

      return  (ZS_ERROPEN);
    }
  off_t wOff = lseek(wHeaderFd,(off_t)cst_HeaderOffset,SEEK_SET);
  if (wOff<0)
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_FILEPOSERR,
          Severity_Severe,
          "lseek error positionning header file  at beginning of header address <%ld> for file <%s>",
          cst_HeaderOffset,
          pHeaderFile.toCChar()
          );
      return (ZS_FILEPOSERR);
    }
    /* get header control block */
  ZHeaderControlBlock_Export wHCB;
  ssize_t wSRead =read(wHeaderFd,&wHCB,sizeof(ZHeaderControlBlock_Export));
  if ((wSRead<0) || (wSRead != sizeof(ZHeaderControlBlock_Export)))
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_BADFILEHEADER,
          Severity_Severe,
          "Error loading  header control block for file  <%s>",
          pHeaderFile.toCChar());
      return  (ZS_BADFILEHEADER);
    }
  /* check this is a true Header file : start sign is a palyndroma and block id is one byte : no need of endian conversion */
  if (( wHCB.StartSign != cst_ZBLOCKSTART)||(wHCB.BlockId != ZBID_HCB))
    {
      ZException.setMessage("ZRandomFile::zutilityUnlockHeaderFile",ZS_BADFILEHEADER,Severity_Error,
          "Invalid or corruped header content : start sign read <%X> expected <%X> block id read <%2X> expected <%2X> for file <%s>",
          wHCB.StartSign,cst_ZBLOCKSTART,
          wHCB.BlockId, wHCB.BlockId,
          pHeaderFile.toCChar()
          );
      return (ZS_BADFILEHEADER);
    }
  /* reset values : no need to convert */
  wHCB.Lock = ZLock_Nothing;  /* this is 0 */
  wHCB.LockOwner=0L;

  /* rewrite header block  */
  wOff = lseek(wHeaderFd,(off_t)cst_HeaderOffset,SEEK_SET);
  wOff= write(wHeaderFd,&wHCB,sizeof(ZHeaderControlBlock_Export));


  ::close(wHeaderFd);
  return ZS_SUCCESS;
}

#ifdef __COMMENT__
ZStatus
ZRandomFile::_unlockFile (bool pForceWrite)
{
ZStatus wSt;

    printf ("ZRandomFile::_unlockFile\n");

/*    if ((!pForceWrite)&&(Mode & ZRF_Exclusive)&&(HeaderAccessed & ZHAC_HCB))
        {
        ZHeader.Lock = ZLock_Nolock;
        ZHeader.LockOwner =(pid_t)0;
        return  ZS_SUCCESS;
        }
*/
lockPack wLockInfos;

//    wSt= _getFileHeader(pForceWrite); // pForceWrite = pForceRead
    wSt=_readFileLock(wLockInfos);
    if (wSt!=ZS_SUCCESS)
                {   return  wSt;}
    ZHeader.Lock = wLockInfos.Lock;
    ZHeader.LockOwner = wLockInfos.LockOwner;

    if (ZHeader.Lock != ZLock_Nolock)
    {
        if (ZHeader.LockOwner!=Pid)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_LOCKED,
                                Severity_Error,
                                " File is already locked by another pid : <%X %d>  (current pid <%X %d>) while trying to lock it. File <%s>  lock mask is <%X %s>",
                                ZHeader.LockOwner,ZHeader.LockOwner,
                                Pid,Pid,
                                URIHeader.toString(),
                                 ZHeader.Lock,
                                decode_ZLockMask (ZHeader.Lock).toChar());
        return  ZS_LOCKED;
        }
    }
    ZHeader.Lock = wLockInfos.Lock = ZLock_Nolock;
    ZHeader.LockOwner=wLockInfos.LockOwner= (pid_t)0;

//  return  _writeFileHeader(pForceWrite);

    return  _writeFileLock(wLockInfos);

}//_unlockFile
#endif // __COMMENT__

/**
 * @brief ZRandomFile::_isLocked tests whether the whole file is locked or not
 *  returns the lock mask for the whole file in pLock
 * re
 * @param[inout] pDescriptor  file descriptor. Nota Bene ZHeader is updated from file.
 * @return true if file is locked - false if file is not locked
 */
bool
ZRandomFile::_isFileLocked (bool pForceRead)
{
ZStatus wSt;
  if (HeaderAccessed & ZHAC_HCB)
  {
  if (getOpenMode()!=ZRF_Exclusive)
    {
    wSt= _getHeaderControlBlock(pForceRead);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" while testing lock on file ");
                ZException.exit_abort();
                }
    }
    }
    return (!(ZHeader.Lock==ZLock_Nolock));
}//_isLocked
//----------End Lock sequence------------------------------------------

//! @}    ZRandomFile

//----------------------XML Routines----------------------------------------

utf8String ZRandomFile::toXml(int pLevel,bool pComment)
{
  utf8String wReturn = fmtXMLnode("file",pLevel);

  wReturn += fmtXMLint("filetype",int(ZFT_ZRandomFile),pLevel+1);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," File type is ZFT_ZMasterFile i. e. ZRandomFile ");

  wReturn += ZFileDescriptor::toXml(pLevel+1,pComment);

  wReturn += fmtXMLendnode("file", pLevel);
  return wReturn;
} // ZRandomFile:toXml

utf8String ZRandomFile::XmlSaveToString(bool pComment)
{
utf8String wReturn = fmtXMLdeclaration();
  wReturn += fmtXMLnodeWithAttributes("zicm","version",__ZRF_VERSION_CHAR__,0);

  wReturn += toXml(0,pComment);

  wReturn += fmtXMLendnode("zicm",0);
  return wReturn;
}

ZStatus ZRandomFile::XmlSaveToFile(uriString &pXmlFile,bool pComment)
{
  bool wExist=pXmlFile.exists();
  ZStatus wSt;
  ZDataBuffer wOutContent;
  utf8String wContent = XmlSaveToString(pComment);

  wOutContent.setData(wContent.toCChar(), wContent.ByteSize);

  return pXmlFile.writeContent(wOutContent);
}// ZRandomFile::XmlSaveToFile




/**
 * @brief ZRandomFile::XmlWriteFileDefinition Static function : Generates the xml definition for a ZRandomFile header given by it path name pFilePath
 * @note the ZMasterFile is opened for read only ZRF_Read_Only then closed.
 *
 * @param[in] pFilePath points to a valid file to generate the definition from
 * @param[in] pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
ZStatus
ZRandomFile::XmlWriteFileDefinition(const char* pZRandomFileName,ZaiErrors & pErrorlog)
{
ZRandomFile wZMF;
ZStatus wSt;

//FILE* wOutput=pErrorlog;

  pErrorlog.setContext("XmlWriteFileDefinition");
  pErrorlog.setAutoPrintOn(ZAIES_Text);

    uriString uriOutput;
    utf8String wOutPath;
    utf8String wLogPath;
    utf8String wOutBase;
    uriOutput= pZRandomFileName;
    wOutBase = uriOutput.getBasename();
    wOutPath = uriOutput.getDirectoryPath();
    wOutPath += wOutBase;
    wLogPath = wOutPath + ".log";
    wOutPath += ".xml";
    uriOutput = wOutPath.toCChar();

    pErrorlog.setOutput(wLogPath);

    wSt = wZMF.zopen(pZRandomFileName,ZRF_Read_Only);
    if (wSt!=ZS_SUCCESS)
      {
      pErrorlog.logZStatus(ZAIES_Error,wSt,"%s-E-ERROPEN Cannot open file <%s> in ZRF_Read_Only mode status <%s>.",
          _GET_FUNCTION_NAME_,
          pZRandomFileName,
          decode_ZStatus(wSt));
      return wSt;
      }

    return wZMF.XmlSaveToFile(uriOutput,true);
/*
    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=\"2.00\">\n");
    fprintf (wOutput,
             "<file>\n"
             " <filetype>ZRandomFile</filetype>\n");
    wZMF._writeXML_ZRandomFileHeader(wZMF.wOutput);
    fprintf (wOutput,
             "</file>\n");
    wZMF.zclose();
    if (pErrorlog==nullptr)
                fclose(wOutput);
*/

}  // static zwriteXML_FileHeader


/**
 * @brief ZRandomFile::zwriteXML_FileHeader Generates the xml definition for current opened ZRandomFile's header
 * @param pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
ZStatus
ZRandomFile::XmlWriteFileDefinition (FILE *pOutput)
{
    FILE* wOutput=pOutput;
    uriString uriOutput;
        if (pOutput==nullptr)
        {

            utf8String OutPath;
            utf8String OutBase;

            uriOutput = URIContent.toString();
            OutBase = uriOutput.getBasename();
            OutPath = uriOutput.getDirectoryPath();
            OutPath += OutBase;
            OutPath += ".xml";
            wOutput = fopen(OutPath.toCString_Strait(),"w");
            if (wOutput==nullptr)
                    {
                    ZException.getErrno(errno,
                                     _GET_FUNCTION_NAME_,
                                     ZS_ERROPEN,
                                     Severity_Severe,
                                     " cannot open file %s for output",
                                     OutBase.toString());
                    ZException.exit_abort();
                    }
        }
        return XmlSaveToFile(uriOutput,true);
}
/*
             "  <ZFileDescriptor>\n"
             "      <URIContent>%s</URIContent>\n"
             "      <URIHeader>%s</URIHeader> <!-- not modifiable generated by ZRandomFile-->\n"
             "      <URIDirectoryPath>%s</URIDirectoryPath> <!-- not modifiable generated by ZRandomFile-->\n"
             "      <ZHeaderControlBlock> <!-- not modifiable -->\n"
             "          <FileType>%s</FileType>  <!-- do not modify : could cause data loss  see documentation-->\n"
             "          <ZRFVersion>%ld</ZRFVersion> <!-- not modifiable -->\n"
             "          <OffsetFCB>%ld</OffsetFCB> <!-- not modifiable -->\n"
             "          <OffsetReserved>%ld</OffsetReserved> <!-- not modifiable -->\n"
             "          <SizeReserved>%ld</SizeReserved> <!-- not modifiable -->\n"
             "      </ZHeaderControlBlock>\n"
*/




//------------------End Xml Routines-----------------------------------
//

//-------------------Dump & statistic routines-------------------------
//


/**
@addtogroup ZRFSTATS
        @{

  */

/**
 * @brief ZRandomFile::ZRFPMSReport  Reports the performance counters of the current ZRandomFile. (file must be openned and active).
 *  @see ZRFPMS
 * PMS counters report input / output activity for each element of ZRandomFile.
 * Data is collected and cumulated for a whole session.
 * @note A ZRFPMS session starts when the file is opened, and ends when file is closed.
 *
 *
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZRandomFile::ZRFPMSReport(FILE* pOutput)
{
    if (!_isOpen)
            {
            _print( "ZRFPMSStats:: File is closed- no data available" );
            return;
            }
    _print(
             "\nZRandomFile Activity Report on file <%s>\n"
             "                 session summary\n"
             "________________________________________________\n"
             "   Header file <%s>\n"
             "   File open mode         %s\n"
             "________________________________________________",
             URIContent.toString(),
             URIHeader.toString(),
             decode_ZRFMode( Mode)
             );

    ZPMS.reportDetails(pOutput);
    return;

}// _ZRFPMSStats

/**
 * @brief ZRandomFile::ZRFstat static method that provides statistics on how information is stored for a given ZRandomFile.

    It provides information from ZFileHeader file header (pools) and some statistical information on block sizes.
    It may be used to adjust ZRandomFile specific parameters in order to increase its usage performance.

   @note This is a static method.
  It opens for reading and close appropriately the given ZRandomFile.
  If this method is used from an object that holds an already opened ZRandomFile, then it reopens it for reading.
  Using it from an active object instance might induce lock problems.

    @note May not be confused with performance reporting @see ZRFPMS .

 * @param pFilename an uriString containing the ZRandomFile name to be opened and analyzed.
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::ZRFstat(const uriString& pFilename, FILE *pOutput)
{
ZRandomFile wZRF;
ZStatus wSt;

if (wZRF.Output==nullptr)
  pOutput=stdout;

    wSt= wZRF.setPath((uriString &)pFilename);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    wSt=wZRF._ZRFopen( ZRF_Read_Only,ZFT_ZRandomFile);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    wSt=wZRF._getFileControlBlock(true);
    if (wSt!=ZS_SUCCESS)
                {
                wZRF.zclose ();
                return wSt;
                }

    double wD = 0;
    double wD1 = 0;
    double wM = 0;

    zsize_type wMin=wZRF.ZBAT.Tab[0].BlockSize;
    zsize_type wMax=wZRF.ZBAT.Tab[0].BlockSize;

    for (long wi =0; wi < wZRF.ZBAT.size();wi++)
            {
            wM += (double) wZRF.ZBAT.Tab[wi].BlockSize;
            if (wZRF.ZBAT.Tab[wi].BlockSize>wMax)
                            wMax=wZRF.ZBAT.Tab[wi].BlockSize;
            if (wZRF.ZBAT.Tab[wi].BlockSize<wMin)
                            wMin=wZRF.ZBAT.Tab[wi].BlockSize;
            }// for
    if (wZRF.ZBAT.size()>0)
            wM= wM/(double)wZRF.ZBAT.size();

    wZRF.ZFCB.MinSize = wMin;
    wZRF.ZFCB.MaxSize = wMax;


    // standard deviation
    for (long wi =0; wi < wZRF.ZBAT.size();wi++)
            {
            wD1 = (((double) wZRF.ZBAT.Tab[wi].BlockSize )- wM) ;
            if (wD1<0)
                    wD += - wD1 ;
                else
                    wD += wD1;
            }
    if (wZRF.ZBAT.size()>0)
                 wD = wD/wZRF.ZBAT.size();

    wZRF._print(
             "Statistics on ZRandom File <%s>\n"
             "________________________________________________\n"
             "   Header file <%s>\n"
             "   Content file size      %10lld\n"
             "   Header file size       %10lld\n"
             "   File open mode         %s\n"
             "________________________________________________\n"
             "      Allocated blocks    %10ld\n"
             "      Allocated size      %10ld\n"
             "      used blocks         %10ld\n"
             "      free pool           %10ld\n"
             "       max block size     %10ld\n"
             "       min block size     %10ld\n"
             "       average block size %10ld\n"
             "       standard deviation %10ld\n"
             "________________________________________________\n",
              wZRF.getURIContent().toString(),
              wZRF.getURIHeader().toString(),
              wZRF.getURIContent().getFileSize(),
              wZRF.getURIHeader().getFileSize(),
              decode_ZRFMode( wZRF.getOpenMode()),
             wZRF.ZFCB.AllocatedBlocks,
             wZRF.ZFCB.AllocatedSize,
             wZRF.ZBAT.size(),
             wZRF.ZFBT.size(),
             wMax,
             wMin,
             (long)wM,
             (long )wD
             );

    return wZRF.zclose ();

}// _ZRFstat



void
ZRandomFile::zfullDump(const int pColumn,FILE* pOutput)       // full dump current ZRF
{
    zfullDump(URIContent,pColumn,pOutput);
    return;
}




/**
 * @brief ZRandomFile::zblockDump  static method that will dump for a give uri of a ZRandomFile a block given by its rank
 *
 *  For a ZRandomFile corresponding to a given uri (pURIContent),
 *  for a block given by its rank,
 *  gives the full details of its content :
 *      - block header content values
 *      - user record content (Ascii + Hexadecimal Dump)
 *  @see recordranks
 *
   @note This is a static method.
  It opens for reading and close appropriately the given ZRandomFile.
  If this method is used from an object that holds an already opened ZRandomFile, then it reopens it for reading.
  Using it from an active object instance might induce lock problems.
 *
 * @param pURIContent
 * @param pBlockNum relative position or rank of the block to dump @see recordranks
 * @param pColumn   the number of bytes used to present ascii and hexa dump @see ZDataBuffer::Dump
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
void ZRandomFile::zblockDump (const uriString &pURIContent,
                              const long pRank,
                              const int pColumn,
                              FILE* pOutput)
{
ZStatus wSt;

ZRandomFile wZRF;
  if (pOutput!=nullptr)
    wZRF.set_Output(pOutput);
  wSt=wZRF.setPath(pURIContent);  // generates also Header file structures within ZFileDescriptor

  wSt=wZRF._ZRFopen(ZRF_Read_Only,ZFT_ZRandomFile);
  if (wSt!=ZS_SUCCESS) {
                ZException.exit_abort();
  }
  wZRF._blockDump(pRank,pColumn) ;

  wZRF.zclose();
  return;
} // zblockDump


void ZRandomFile::_blockDump (const long pRank,const int pColumn)
{
  ZStatus wSt;


  if ((wSt=_getFileControlBlock(true))!=ZS_SUCCESS)
  {
    ZException.addToLast(" header file is <%s> for content file <%s>",getURIHeader().toUtf(),getURIContent().toUtf());
    _print(ZException.last().formatFullUserMessage(false));
    return ;
  }

  _print(
      "        ***********************************************************\n"
      "        *  Block dump for file  %30s     *\n"
      "        ***********************************************************",
      getURIContent().toUtf());


  utf8String wRulerHexa;
  utf8String wRulerAscii;
  ZBlock wBlock;
  zaddress_type wAddress;

  int wColumn=zmemRulerSetup (wRulerHexa, wRulerAscii,pColumn);

  wSt=_getByRank(wBlock, pRank,wAddress);     // get first block of the file
  _print(
      "\n                 == Block number %4ld address %ld ==",
      pRank,
      wAddress);

  _dumpOneDataBlock(wBlock,wRulerHexa,wRulerAscii,wColumn);

  if (wSt!=ZS_SUCCESS) {
    _print(ZException.last().formatFullUserMessage(false));
  }

  return;
} // _blockDump


ZStatus ZRandomFile::zsurfaceScan(const uriString pURIContent, FILE *pOutput)
{
ZStatus wSt;

ZRandomFile wZRF;

    wSt=wZRF.setPath(pURIContent);  // generates also Header file structures within ZFileDescriptor
    if (wSt!=ZS_SUCCESS)
      {
      return wSt;
      }

    wZRF.set_Output(pOutput);
    wSt=wZRF._ZRFopen(ZRF_Read_Only,ZFT_Any);
    if (wSt!=ZS_SUCCESS)
                {
                return wSt;
                }

    if ((wSt=wZRF._getFileControlBlock(true))!=ZS_SUCCESS)
                {
                ZException.addToLast("Error while getting header file %s for ZRF file %s",
                wZRF.getURIHeader().toCChar(),
                wZRF.getURIContent().toCChar());
                return wSt;
                }

    wZRF._print(
            "        --------------ZRandomFile Surface Analysis Utility------------------\n"
            "         File %s\n"
            "        --------------------------------------------------------------------\n",
                    wZRF.getURIContent().toString());


    wZRF._headerDump();
    wZRF._surfaceScan();
    return wZRF.zclose();

}//zsurfaceScan

//! @cond Development

ZStatus
ZRandomFile::_surfaceScan()
 {
ZStatus wSt;

  _print("\n___________________________Content Data blocks in physical order___________________________________");

  wSt=ZS_SUCCESS;

ZBlockHeader wBlockHeader;

zaddress_type wOldAddress = 0;
zsize_type    wOldSize = 0;
zaddress_type wAddressNext = 0;
zaddress_type wTheoricAddress = 0;

    _print(
             "Physical address    |   State       | Block size         | User content size  | Theorical next Add\n"
             "____________________|_______________|____________________|____________________|____________________"
             );

    wSt=_seek(0L);
    while (true)
        {
        wSt=_searchNextPhysicalBlock(wAddressNext,wAddressNext,wBlockHeader);     // get first physical block of the file
        if (wSt!=ZS_FOUND)
                        break;

        if (wBlockHeader.State==ZBS_Deleted) {
          _print( "--%18ld|%15s|%20ld|%20ld|%20s",
                    wAddressNext,
                    decode_ZBS( wBlockHeader.State),
                    wBlockHeader.BlockSize,
                    (wBlockHeader.BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
                    "--deleted block--");
                     wAddressNext += sizeof(ZBlockHeader_Export);
        }
            else
                break;

           }// while true

    while (wSt==ZS_FOUND) {
        wTheoricAddress = wOldAddress+wOldSize;
        if (wAddressNext!=wTheoricAddress) {
          _print( "%20ld|%15s|%20ld|                    |",
                            wTheoricAddress,
                            "--- <Hole> ---",
                            wAddressNext-wTheoricAddress
                            );
        }
        _print( "%20ld|%15s|%20ld|%20ld|%20ld",
                wAddressNext,
                decode_ZBS( wBlockHeader.State),
                wBlockHeader.BlockSize,
                (wBlockHeader.BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
                wAddressNext+wBlockHeader.BlockSize);


        wOldAddress=wAddressNext;
        wOldSize = wBlockHeader.BlockSize ;
        if (wBlockHeader.State == ZBS_Used)
            wAddressNext += wBlockHeader.BlockSize;
        else
            wAddressNext += sizeof(ZBlockHeader_Export);   // just to grab deleted Blocks

        while (true) {
            wSt=_searchNextPhysicalBlock(wAddressNext,wAddressNext,wBlockHeader);     //! get first physical block of the file
            if (wSt!=ZS_FOUND)
                        break;

            if (wBlockHeader.State==ZBS_Deleted) {
              _print("--%18lld|%15s|%20lld|%20lld|%20s",
                        wAddressNext,
                        decode_ZBS( wBlockHeader.State),
                        wBlockHeader.BlockSize,
                        (wBlockHeader.BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
                        "--deleted block--");

                         wAddressNext += sizeof(ZBlockHeader_Export);
            }
            else break;
        }// while true
    }// while ZS_FOUND

    if (wSt==ZS_EOF) {
        _print("\n       **** End of File reached at address <%lld> *****",
                wAddressNext);
    }
    else {
      _print(ZException.last().formatFullUserMessage(false));
    }
    return wSt;
} // _physicalcontentDump






//! @endcond
/* static method */
void
ZRandomFile::zfullDump(const uriString &pURIContent, const int pColumn, FILE* pOutput) // dump another ZRF
{
ZStatus wSt;
ZRandomFile wZRF;


    wSt=wZRF.setPath(pURIContent);  // generates also Header file structures within ZFileDescriptor

    wSt=wZRF._ZRFopen(ZRF_Read_Only,ZFT_Any);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.printUserMessage(stderr);
                abort();
                }

    if ((wSt=wZRF._getFileControlBlock(true))!=ZS_SUCCESS)
                {
                ZException.addToLast("Error while getting header %s for ZRF file %s",
                                            wZRF.getURIHeader().toString(),
                                            wZRF.getURIContent().toString());
                ZException.printUserMessage(pOutput);
                return ;
                }

    fprintf(pOutput,
            "        ***********************************************************\n"
            "        * Logical Full Dump of file %30s *\n"
            "        ***********************************************************\n",
            wZRF.getURIContent().toString());

    wZRF._headerDump();
    wZRF._fullcontentDump(pColumn);
    wZRF.zclose();
    return;

}//zfullDump

/* local method */
ZStatus
ZRandomFile::_fullDump( const int pColumn)
{
  ZStatus wSt;

  if ((wSt=_getFileControlBlock(true))!=ZS_SUCCESS)
  {
    ZException.addToLast("Error while getting header %s for ZRF file %s",
        getURIHeader().toUtf(),
        getURIContent().toUtf());
    _print(ZException.last().formatFullUserMessage());
    return wSt;
  }

  _print(
      "        ***********************************************************\n"
      "        * Logical Full Dump of file %30s *\n"
      "        ***********************************************************",
      getURIContent().toUtf());

  _headerDump();
  return _fullcontentDump(pColumn);

}//zfullDump

/**
 * @brief ZRandomFile::zheaderDump Dumps the file header data for the ZRandomFile corresponding to pURIContent
 *
 *  reports the content of ZHeaderControlBlock file header to pOutput.
 *
   @note This is a static method.
  It opens for reading and close appropriately the given ZRandomFile.
  If this method is used from an object that holds an already opened ZRandomFile, then it reopens it for reading.
  Using it from an active object instance might induce lock problems.

 *
 * @param pURIContent uriString mentionning the ZRandomFile name to be dumped
 * @param pOutput   a FILE* pointer where the reporting will be made. By default, set to stdout.
 */
void
ZRandomFile::zheaderDumpS(const uriString &pURIContent, FILE* pOutput)
{
ZStatus wSt;
ZRandomFile wZRF;

    wSt=wZRF.setPath(pURIContent);  // generates also Header file structures within ZFileDescriptor

    wZRF.set_Output(pOutput);
    wSt=wZRF._ZRFopen(ZRF_Read_Only,ZFT_ZRandomFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.exit_abort();
                }

    if ((wSt=wZRF._getFullFileHeader(true))!=ZS_SUCCESS)
          {
          ZException.addToLast("Error while getting header %s for ZRF file %s",
                                wZRF.getURIHeader().toUtf(),
                                wZRF.getURIContent().toUtf());
          ZException.exit_abort();
          }

    fprintf(pOutput,
            "        *******************************************************\n"
            "        *  Dump of Header-file %30s *\n"
            "        *******************************************************\n",
              wZRF.getURIContent().toUtf());

    wZRF._headerDump();
    wZRF.zclose();
    return ;

}//zheaderDumpS


/**
 * @brief ZRandomFile::zcontentDump dumps to pOutput the content of a ZRandomFile.
 *
   @note This is a static method.
  It opens for reading and close appropriately the given ZRandomFile.
  If this method is used from an object that holds an already opened ZRandomFile, then it reopens it for reading.
  Using it from an active object instance might induce lock problems.

 *
 * @param pURIContent uriString mentionning the ZRandomFile name to be dumped
 * @param pColumn   Number of bytes displayed both in ascii and hexadecimal per row
 * @param pOutput   a FILE* pointer where the reporting will be made. By default, set to stdout.
 */
void
ZRandomFile::zcontentDumpS(uriString pURIContent,int pColumn,FILE* pOutput)
{
ZStatus wSt;
ZRandomFile wZRF;


    wSt=wZRF.setPath(pURIContent);  // generates also Header file structures within ZFileDescriptor

    wSt=wZRF._ZRFopen(ZRF_Read_Only,ZFT_Any,false);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.printUserMessage(stderr);
                abort();
                }
    wSt=wZRF._contentDump(pColumn);
    wZRF.zclose();
    return;

}//zcontentDump

/**
 * @brief ZRandomFile::zcontentDump reports the whole content of the current ZRandomFile
 *
 * @param pColumn   the number of bytes used to present ascii and hexa dump @see ZDataBuffer::Dump
 */
ZStatus
ZRandomFile::_contentDump(int pColumn)
{
  ZStatus wSt;


  if ((wSt=_getFullFileHeader(true))!=ZS_SUCCESS)
  {
    ZException.addToLast("Error while getting header %s for ZRF file %s",
        getURIHeader().toString(),
        getURIContent().toString());
    _print(ZException.last().formatFullUserMessage());
    return wSt;
  }

  _print(
      "        ***********ZRandomFile Content Dump Utility******************\n"
      "        File %s\n"
      "        *************************************************************",
      getURIContent().toUtf());

  _fullcontentDump(pColumn);

}//zcontentDump local


/**
 * @brief ZRandomFile::_fullcontentDump dump the full content of current ZRandomFile given by pDescriptor
 * ZRF must be opened for reading
 *
 * @param pDescriptor ZFileDescriptor of the ZRandonFile to dump
 * @param pColumn   Number of bytes displayed both in ascii and hexadecimal per row
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
ZStatus
ZRandomFile::_fullcontentDump(const int pWidth)
 {
ZStatus wSt;

  _print("\n....................Content Data blocks..........................");

wSt=ZS_SUCCESS;

ZBlock          wBlock;
utf8String     wRulerHexa;
utf8String     wRulerAscii;
zaddress_type   wAddress;
zrank_type      wRank;


    int wColumn=zmemRulerSetup (wRulerHexa, wRulerAscii,pWidth);
    zaddress_type wBN = 0;

    wRank = 0L;
    wSt=_getByRank(wBlock,wRank,wAddress);     // get first block of the file
    while (wSt==ZS_SUCCESS) {
      _print("\n                 == Block number %4ld - Physical address %ld ==",
             wRank,
             wAddress);
//             getCurrentRank(),
//             getCurrentPhysicalAddress());

        _dumpOneDataBlock(wBlock,wRulerHexa,wRulerAscii,wColumn);

        wSt=_getNext( wBlock,wRank,wAddress);
    }// while

    if (wSt==ZS_EOF) {
        _print( "\n       ***** End of File reached at block %d *****", wBN);
    }
    else {
          _print(ZException.last().formatFullUserMessage());
    }
    return wSt;
} // _contentDump


//! @cond Development

/**
 * @brief ZRandomFile::_dumpOneDataBlock dumps one given block to pOutput
 * @param pBlock    Block content to dump
 * @param pRulerHexa    Ruler for Hexadecimal
 * @param pRulerAscii   Ruler for Ascii
 * @param pColumn   Number of bytes displayed both in ascii and hexadecimal per row
 */
void ZRandomFile::_dumpOneDataBlock( ZBlock &pBlock, utf8String &pRulerHexa, utf8String &pRulerAscii,
                                const int pWidth)
{
    utf8String wLineChar;
    utf8String wLineHexa;
    const int wGroupFactor=4;
    int wGroup=pWidth/wGroupFactor;

    _print(
            " Block size  <%10ld> User Content size <%10ld>\n"
            " State       <%10s> Lockmask <0x%X %s> lock owner <0x%X>",
            pBlock.BlockSize,
            pBlock.DataSize(),
            decode_ZBS( pBlock.State),
            pBlock.Lock,decode_ZLockMask( pBlock.Lock).toChar(),
            pBlock.Pid);

    _print("____Offset____ %s %s",
             pRulerHexa.toCChar(),
             pRulerAscii.toCChar());

    long wOffset=0;
    int wL=0;
    while ((wOffset+pWidth)<pBlock.DataSize())
            {
            zmemDumpHexa(pBlock.Content.Data+wOffset,pWidth,wLineHexa,wLineChar);

            _print("%6X-%6d |%s |%s|",wL,wL,wLineHexa.toCChar(),wLineChar.toCChar());
            wL+=pWidth;
            wOffset+=pWidth;
            }


    char wFormat [50];

    sprintf (wFormat,"#6X-#6d |#-%ds |#-%ds|",(pWidth*3)+wGroup,pWidth);
    wFormat[0]='%';
    for (int wi=0;wFormat [wi]!='\0';wi++)
                if (wFormat[wi]=='#')
                            wFormat[wi]='%';

    zmemDumpHexa(pBlock.Content.Data+wOffset,(pBlock.Content.Size-wOffset),wLineHexa,wLineChar);
 //   pBlock.Content.dumpHexa(wOffset,(pBlock.Content.Size-wOffset),wLineHexa,wLineChar);

    _print(wFormat,wL,wL,wLineHexa.toCChar(),wLineChar.toCChar());
    return;
} //_dumpOneDataBlock

//! @endcond


/**
 * @brief ZRandomFile::zheaderDump reports the header data of the currently opened ZRandomFile
 */
void
ZRandomFile::_headerDump()
{
ZStatus wSt;
ZHeaderControlBlock_Export wHeaderExp;

  if ((wSt=_getFullFileHeader(true))!=ZS_SUCCESS) {
    ZException.addToLast("Error while getting header %s for ZRandom file %s",
        getURIHeader().toUtf(),
        getURIContent().toUtf());
    ZException.exit_abort();
  }

  wSt=_getFileHeader_Export(&wHeaderExp);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();
  wHeaderExp.deserialize();


  _print(
      "        +-----------------------------------------------------+\n"
      "        |  Dump of Header-file %30s |\n"
      "        +-----------------------------------------------------+",
      getURIContent().toUtf());

/*  _print(
    " Header file name     <%s>\n"
    " Header file block \n"
    " Identification       %s\n"
    " Offset to FCB        %10ld\n"
    " Reserved block size  %10ld\n"
    " Version              %10ld",
        getURIHeader().toString(),
    decode_ZBID(  wHeaderExp.BlockId),
    reverseByteOrder_Conditional<zaddress_type>(wHeaderExp.OffsetFCB),
    reverseByteOrder_Conditional<zsize_type>(wHeaderExp.SizeReserved),
    reverseByteOrder_Conditional<unsigned long>( wHeaderExp.ZRFVersion));
*/
  _print(
      " Header file name     <%s>\n"
      " Header file block \n"
      " Identification       %s\n"
      " Offset to FCB        %10ld\n"
      " Reserved block size  %10ld\n"
      " Version              %s",
      getURIHeader().toString(),
      decode_ZBID(  wHeaderExp.BlockId),
      wHeaderExp.OffsetFCB,
      wHeaderExp.SizeReserved,
      getVersionStr( wHeaderExp.ZRFVersion).toString() );

  _print(
        " File Control Block\n"
        " Start of data        %10ld\n"
        " File Allocated size  %10ld\n"
        " File Used size       %10ld\n"
        " Allocated blocks     %10ld\n"
        " Block extent quota   %10ld\n"
        " Used blocks          %10ld\n"
        " Free blocks          %10ld\n"
        " Deleted blocks       %10ld\n"
        " Block target  size   %10ld\n"
        " Highwater Marking    %10s\n"
        " GrabFreeSpace        %10s",

        ZFCB.StartOfData,
        getAllocatedSize(),
        ZFCB.UsedSize,
        ZFCB.AllocatedBlocks,
        ZFCB.BlockExtentQuota,
        ZBAT.size(),
        ZFBT.size(),
        ZDBT.size(),
        ZFCB.BlockTargetSize,
        ZFCB.HighwaterMarking?"On":"Off",
        ZFCB.GrabFreeSpace?"On":"Off");

  _print(
        " Block Access Table (ZBAT) - Active blocks are stored here\n"
        "------+------------+------+--------------------+--------------------+\n"
        " Rank |      State |      |    Address         |          Block size|\n"
        "------+------------+------+--------------------+--------------------+");

  for (long wi=0; wi<ZBAT.size();wi++)
    _print(
        "%5ld |%12s|%6s|%20ld|%20ld|",
        wi,
        decode_ZBS(ZBAT.Tab[wi].State),
        " ",
        ZBAT.Tab[wi].Address,
        ZBAT.Tab[wi].BlockSize );
 /*       fprintf(pOutput,
        "------+------------+------+--------------------+--------------------+\n");
        fprintf(pOutput,
        " Free Blocks Pool (ZFBT)\n"
        "------+--------------------+--------------------+\n"
        " Rank |    Address         |          Block size|\n"
        "------+--------------------+--------------------+\n");

        for (long wi=0; wi<ZFBT.size();wi++)
        fprintf(pOutput,
        "%5ld |%20lld|%20lld|\n",
        wi,
        ZFBT.Tab[wi].Address,
        ZFBT.Tab[wi].BlockSize);
*/
  if (ZBAT.size()>0)
    _print(
                "------+------------+------+--------------------+--------------------+");
  _print(
        " Free Blocks Pool(ZFBT) Available blocks are stored here\n"
        "------+------------+------+--------------------+--------------------+\n"
        " Rank |      State |      |    Address         |          Block size|\n"
        "------+------------+------+--------------------+--------------------+");

        for (long wi=0; wi<ZFBT.size();wi++)
  _print(
        "%5ld |%12s|%6s|%20ld|%20ld|",
        wi,
        decode_ZBS(ZFBT.Tab[wi].State),
        " ",
        ZFBT.Tab[wi].Address,
        ZFBT.Tab[wi].BlockSize );

  if (ZFBT.size()>0)
    _print("------+------------+------+--------------------+--------------------+");
  _print(
        " Deleted Blocks Pool(ZDBT) - Recovery Pool\n"
        "------+------------+------+--------------------+--------------------+\n"
        " Rank |      State |      |    Address         |          Block size|\n"
        "------+------------+------+--------------------+--------------------+");

  for (long wi=0; wi<ZDBT.size();wi++)
    _print(
        "%5ld |%12s|%6s|%20ld|%20ld|",
        wi,
        decode_ZBS(ZDBT.Tab[wi].State),
        " ",
        ZDBT.Tab[wi].Address,
        ZDBT.Tab[wi].BlockSize );

  if (ZDBT.size()>0)
    _print( "------+------------+------+--------------------+--------------------+");

  return;
}// _headerDump

//----------------End Dump routines--------------------------------------
/** @} ZRFDUMPSTAT */


//---------------------Surface Utilities----------------------------

//! @cond Development

/**
 * @brief ZRandomFile::_moveBlock moves A block physically from address pFrom to address pTo and creates an entry in ZFBT for moved block with state ZBS_Free.
 *
 *  _moveBlock Stores first the block to move from its source address pFrom, then move it to its destination address pTo,
 *  so that destination address may overlap source address without problem.
 * However, this has to be managed by calling routine.
 *
 * @param[in] pDescriptor   The current file descriptor
 * @param[in] pFrom      Address of the beginning of the block to move
 * @param[in] pTo       Address of the beginning of the block to be moved in. Existing data will be suppressed
 * @param[in] pModule   a C string describing the calling module
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_moveBlock (const zaddress_type pFrom,
                         const zaddress_type pTo,
                         bool pCreateZFBTEntry)
{
ZStatus wSt;
ZBlock wBlock;
ZBlockDescriptor wBD;
    wSt=_readBlockAt(wBlock,pFrom); // read the block to move
    if (wSt!=ZS_SUCCESS)
            { return  wSt;}

    wBD = wBlock;
    wBD.Address = pFrom;
    if (__ZRFVERBOSE__)
            fprintf(stdout,
                ".... moving Block. Former block Address %ld Size %ld",
                pFrom,
                wBD.BlockSize);
    if (pCreateZFBTEntry) {
      wBD.State = ZBS_Free;
      ZFBT.push(wBD);
      if (__ZRFVERBOSE__)
                    fprintf (stdout,
                         " put to free blocks pool ZFBT. Pool rank %ld\n ",
                         ZFBT.lastIdx());
      }
      else {
        wBD.State = ZBS_Deleted ;
        if (__ZRFVERBOSE__)
                    fprintf (stdout,
                         " deleted (ZBS_Deleted)\n");
      }

    wSt=_writeBlockHeader(wBD,pFrom); // mark it as deleted
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

    return  _writeBlockAt(wBlock,pTo);  // move it to its new address

}//_moveBlock

//! @endcond

/** @addtogroup ZRFUtilities
 * @{
  */

/**
 * @brief ZRandomFile::zheaderRebuild This static function rebuilds ZRandomFile header from an existing ZRandomFile content.
 *
 * If Header is damaged or lost, ZRandomFile content cannot be accessed anymore.
 * zheaderRebuild tries to create a new header from ZRandomFile content, and populate the 3 pools from existing blocks found :
 *  - ZBlockAccessTable : references all found blocks with State field as ZBS_Used
 *  - ZFreeBlockPool    : references all found blocks with State field as ZBS_Free
 *  - ZDeletedBlockPool : references all block headers found with state as ZBS_Deleted
 *
 *  The previous relative order of record in absolutely not garanteed, as ZBAT is populated in the physical block order.
 *
 *  At the end of the process, ZRandomFile header is saved to file to create the new file header.
 *  Both content file and header file are closed.
 *
 *
 * @param[in] pContentURI path of the ZRandomFile file's content to rebuild the header for
 * @param[in] pDump       Option if set (true) a surface scan of the file will be made before processing the file
 * @param[in] pOutput     a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zheaderRebuild(uriString pContentURI,bool pDump, FILE*pOutput)
{

ZStatus wSt;
ZFileDescriptor wDescriptor;
int wS;
ZBlockDescriptor wBlockDescriptor;

zaddress_type wAddress, wAddressNext,wOldAddress,wTheoricAddress;
zsize_type wOldSize;

    wDescriptor.setPath(pContentURI);


    _print(
             " Rebuilding header for ZRandomFile content file <%s>\n"
             "     Header file will be <%s>\n"
             "   **************Surface scan of content file*****************\n",
             wDescriptor.URIContent.toString(),
             wDescriptor.URIHeader.toString());
    if (!wDescriptor.URIContent.exists())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_FILENOTEXIST,
                                   Severity_Error,
                                   "File <%s> does not exist. It must exist to be opened.",
                                   wDescriptor.URIContent.toString());
            ZException.printUserMessage(pOutput);
            return  (ZS_FILENOTEXIST);
            }
    wDescriptor.ContentFd = open(wDescriptor.URIContent.toCChar(),O_RDONLY);       // open content file for read only
    if (wDescriptor.ContentFd<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_ERROPEN,
                             Severity_Severe,
                             " Error opening ZRandomFile content file %s ",
                             wDescriptor.URIContent.toString());
            return  ZS_ERROPEN;
            }
    if (pDump)
        _surfaceScan();

    mode_t wPosixMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    wDescriptor.HeaderFd = open(wDescriptor.URIHeader.toCChar(),O_RDWR|O_CREAT|O_TRUNC,wPosixMode); // open header file read write

    ssize_t wHeaderSize = sizeof (ZHeaderControlBlock_Export) + sizeof (ZFCB_Export) ;

    wS=    posix_fallocate(wDescriptor.HeaderFd,0L,(off_t)wHeaderSize);
    if (wS!=0)
            {
            ZException.getErrno(wS,  // no errno is set by posix_fallocate : returned value is the status : ENOSPC means no space
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Severe,
                             " Error during creation of file <%s> cannot allocate disk space",
                             wDescriptor.URIHeader.toCChar());
            wSt=ZS_WRITEERROR;
            goto end_zheaderRebuild;
            }
    wAddress=0L;
    wS=lseek(wDescriptor.ContentFd,wAddress,SEEK_SET);
    if (wS<0)
        {
        ZException.getErrno(errno,
                        _GET_FUNCTION_NAME_,
                        ZS_FILEPOSERR,
                        Severity_Severe,
                        "Error while positionning file <%s> at address <%lld>",
                        wDescriptor.URIContent.toCChar(),
                        wAddress);
        return  (ZS_FILEPOSERR);
        }
    while (true)
        {
        wSt=_searchNextPhysicalBlock(wAddress,wAddressNext,wBlockDescriptor);     //! get first physical block of the file
        if (wSt!=ZS_FOUND)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_EMPTYFILE,
                                        Severity_Severe,
                                        " No valid block has been found in content file %s",
                                        wDescriptor.URIContent.toCChar());
                goto error_zheaderRebuild;
                }
        wBlockDescriptor.Address = wAddressNext;


        // first block
        wDescriptor.ZFCB.StartOfData = wBlockDescriptor.Address;
        if (wBlockDescriptor.State==ZBS_Deleted)
                        {
            if (__ZRFVERBOSE__)
                    fprintf(pOutput,
                            "--%18lld|%15s|%20lld|%20lld|%20s\n",
                            wAddressNext,
                            decode_ZBS( wBlockDescriptor.State),
                            wBlockDescriptor.BlockSize,
                            (wBlockDescriptor.BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
                            "--deleted block--");

                     wAddressNext += sizeof(ZBlockHeader_Export);

                     wDescriptor.ZDBT.push(wBlockDescriptor);
                        }
            else
                    break;
           }// while true

    while (wSt==ZS_FOUND)
        {
        wTheoricAddress = wOldAddress+wOldSize;
        if (wAddressNext!=wTheoricAddress)
                    {
            if (__ZRFVERBOSE__)
                    fprintf(pOutput,
                            "%20lld|%15s|%20lld|                    |\n",
                            wTheoricAddress,
                            "--- <Hole> ---",
                            wAddressNext-wTheoricAddress
                            );
                    }
        if (__ZRFVERBOSE__)
            fprintf(pOutput,
                "%20lld|%15s|%20lld|%20lld|%20lld\n",
                wAddressNext,
                decode_ZBS( wBlockDescriptor.State),
                wBlockDescriptor.BlockSize,
                (wBlockDescriptor.BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
                wAddressNext+wBlockDescriptor.BlockSize);

        wBlockDescriptor.Address = wAddressNext;
        wOldAddress=wAddressNext;
        wOldSize = wBlockDescriptor.BlockSize ;
        if (wBlockDescriptor.State == ZBS_Used)
        {
            wDescriptor.ZBAT.push(wBlockDescriptor);

            wDescriptor.ZFCB.UsedSize += wBlockDescriptor.BlockSize ;  // sum used size and recompute block target size
            wDescriptor.ZFCB.BlockTargetSize = (long)((float)wDescriptor.ZFCB.UsedSize/(float)wDescriptor.ZBAT.size());

            wAddressNext += wBlockDescriptor.BlockSize;
        }
        else
        {
            if (wBlockDescriptor.State == ZBS_Free)
                    wDescriptor.ZFBT.push(wBlockDescriptor);
//                else
//                    wDescriptor.ZDBT.push(wBlockHeader);
            wAddressNext += sizeof(ZBlockHeader_Export);   // just to grab deleted Blocks
        }
        while (true)
            {
            wSt=_searchNextPhysicalBlock(wAddressNext,wAddressNext,wBlockDescriptor); // get first physical block of the file
            if (wSt!=ZS_FOUND)
                        break;

            wBlockDescriptor.Address = wAddressNext;
            if (wBlockDescriptor.State==ZBS_Deleted)
                            {
                            if (__ZRFVERBOSE__)
                                fprintf(pOutput,
                                    "--%18ld|%15s|%20ld|%20ld|%20s\n",
                                    wAddressNext,
                                    decode_ZBS( wBlockDescriptor.State),
                                    wBlockDescriptor.BlockSize,
                                    (wBlockDescriptor.BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
                                    "--deleted block--");

                            wDescriptor.ZDBT.push(wBlockDescriptor);
                            wAddressNext += sizeof(ZBlockHeader_Export);
                            }
                        else
                            break;
               }// while true
        }// while ZS_FOUND

    if (wSt==ZS_EOF)
        {
        if (__ZRFVERBOSE__)
            fprintf(pOutput,
                "\n       **** End of File reached at address <%lld> *****\n",
                wAddressNext);
        }
        else
        {
        ZException.printUserMessage(pOutput);
        }


    _writeFileHeader(true);

    _print(
             " *********Header rebuild results**************\n");

    _headerDump();

end_zheaderRebuild:
     _writeFileHeader(true);
    close (wDescriptor.HeaderFd);
    close (wDescriptor.ContentFd);
    return  wSt;
error_zheaderRebuild:
  _print(ZException.last().formatFullUserMessage());
  goto end_zheaderRebuild;
}// zheaderRebuild



//! @brief __CLONE_BASEEXTENSION__ Preprocessor symbol : base name extension for creating cloned file name
#define __CLONE_BASEEXTENSION__ "_cln"


/**
 * @brief ZRandomFile::zcloneFile Clones the current ZRandomFile : duplicates anything concerning its file header and copies its data
 *
 *  @par Clone name generation
 *  name is composed as <path><basename><__CLONE_BASEEXTENSION__>.<extension>
 * __CLONE_BASEEXTENSION__ is a preprocessor parameter that is added to ZRandomFile base name to clone.
 * By default, __CLONE_BASEEXTENSION__ is equal to <_clone>.
 * This base name extension is located right before the file name extension (before <.>).
 *
 *  @par Clone process
 * Duplicates the file structure and the data
 * - same structure, same parameters
 * - however copy is done in reorganizing blocks in the order of relative ranks
 * - at creation time :
 *  size is set using : used size + pFreeBlock (fallocate)
 *   Only one free block of size pFreeBlock is added at the end of physical file
 *  if pFreeBlocks = -1 (default) then BlockTargetSize is taken.
 * @par Free blocks
 * As a result, all free blocks (and therefore all deleted blocks) from the source ZRF file will be eliminated for the cloned file surface.
 * Only one free block, with a size equal to pFreeSpace bytes, will remain at the very top of physical address space.
 *
 * @param[in] pDescriptor   The current file descriptor read-only
 * @param[in] pFreeSpace    defaulted to -1 : free space to add at the top of file addresses when all used ranks are copied
 * @param[in] pOutput       a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zcloneFile(const zsize_type pFreeSpace,  FILE*pOutput)
{
ZStatus wSt;
long wi=0, wj=0;
uriString wCloneContent;
zmode_type wMode = ZRF_Nothing ;
ZFile_type wFileType;
ZBlock wBlock;
zsize_type wFreeSpace ;
zaddress_type wAddress;
zaddress_type wAddress1;

ZFileDescriptor wDescriptor_Clone;

ZRandomFile wZRF;


  wMode=getOpenMode();
  if (wMode!=ZRF_NotOpen)
  {
    zclose();
  }
  wSt=zopen(ZRF_Exclusive|ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS)
    goto error_zcloneFile;


  wCloneContent = wZRF.getURIContent().getDirectoryPath().toCChar();
  wCloneContent.addConditionalDirectoryDelimiter();
  wCloneContent += wZRF.getURIContent().getBasename().toCChar();
  wCloneContent += __CLONE_BASEEXTENSION__ ;
  wCloneContent +=".";
  wCloneContent += wZRF.getURIContent().getFileExtension().toCChar();

  _print(
        " preparing to clone file <%s> into <%s>\n ...Setting path and duplicating header \n",
        getURIContent().toCChar(),
        wCloneContent.toCChar());


  wZRF._copyFrom((ZFileDescriptor&)*this);
//  if (wZRF.ZFCB)
//    delete wZRF.ZFCB;

  wZRF.ZFCB._copyFrom(ZFCB);

  wFreeSpace = (pFreeSpace>0)?pFreeSpace:ZFCB.BlockTargetSize ;
// composing name

// setting up file header
     wSt = wDescriptor_Clone.setPath(wCloneContent);
     if (wSt!=ZS_SUCCESS)
                goto end_zcloneFile;
     // duplicating FCB parameters
 //    memmove(wDescriptor_Clone.ZFCB,ZFCB, sizeof(ZFileControlBlock));

     wDescriptor_Clone.Mode = ZBS_Nothing;
     wDescriptor_Clone.ZHeader.Lock =ZLock_Nolock ;
     wDescriptor_Clone.ZFCB.UsedSize = 0;
    if (__ZRFVERBOSE__)
        _print(
              " creating clone file <%s>. \n"
              " source used size is %ld - requested additional free size is %ld <%s>\n"
              " Total cloned file size will be %ld\n"
              "__________Cloned file parameters ____________\n"
              "  InitialSize      %ld\n"
              "  AllocatedBlocks  %ld\n"
              "  BlockExtentQuota %ld\n"
              "  BlockTargetSize  %ld\n"
              "  HighwaterMarking %s\n"
              "  GrabFreeSpace    %s\n"
              "______________________________________________\n"
          ,
              wCloneContent.toCChar(),
              ZFCB.UsedSize,
              wFreeSpace,
              (pFreeSpace==0)?"Defaulted to BlockTargetSize":"Given value",
              ZFCB.UsedSize+wFreeSpace,

              ZFCB.UsedSize+wFreeSpace,
              ZFCB.AllocatedBlocks,
              ZFCB.BlockExtentQuota,
              ZFCB.BlockTargetSize,
              ZFCB.HighwaterMarking?"true":"false",
              ZFCB.GrabFreeSpace?"true":"false");

     // create file and allocate UsedSize + BlockTargetSize file space
    wZRF.setPath(wCloneContent);


    wZRF.setCreateMaximum(ZFCB.UsedSize+wFreeSpace,
                  ZFCB.AllocatedBlocks,
                  ZFCB.BlockExtentQuota,
                  ZFCB.BlockTargetSize,
                  ZFCB.HighwaterMarking,
                  ZFCB.GrabFreeSpace);

    wSt=wZRF._create(wZRF.ZFCB.UsedSize+wFreeSpace,(ZFile_type) getFileType(),false,false);
    if (wSt!=ZS_SUCCESS)
      goto error_zcloneFile;
/*
     wSt=_create(wDescriptor_Clone,ZFCB.UsedSize+wFreeSpace,wFileType,false,false);
     if (wSt!=ZS_SUCCESS)
            goto end_zcloneFile;

     wSt=_open(wDescriptor_Clone,ZRF_Exclusive,wFileType);
*/

    wSt=wZRF.zopen(ZRF_Exclusive|ZRF_Write);
    if (wSt!=ZS_SUCCESS)
      goto error_zcloneFile;

    _print("...Populating file with source file with active blocks (%ld)\n",ZBAT.size());
     for (wi=0;wi<ZBAT.size();wi++)
         {
          wSt=_getByRank(wBlock,wi,wAddress1);        // read block from file
          if (wSt!=ZS_SUCCESS)
                    goto error_zcloneFile;
          wSt=wZRF._add(wBlock.Content,wAddress);  // write block to clone
          if (wSt!=ZS_SUCCESS)
                    goto error_zcloneFile;
          if (wj > 10)
          {
            wj=0;
            fprintf(pOutput,"%ld/%ld\n",wi,ZBAT.size());
          }
          wj++;
         } // for
    fprintf(pOutput,"%ld/%ld\n",wi,ZBAT.size());

    _print( "...Populated %ld blocks from source file\n", wi);
     wZRF.zclose();

end_zcloneFile:
  zclose();
  if (wMode!=ZRF_NotOpen)
        zopen(wMode);
  return wSt;

error_zcloneFile:
    wZRF.zclose();
    ZException.printUserMessage();
    goto end_zcloneFile;
}// zcloneFile

/**
 * @brief ZRandomFile::zcloneFile overload method for zcloneFile using current openned file
  * @return
 */
/*ZStatus
ZRandomFile::zcloneFile(const zsize_type pFreeSpace, FILE*pOutput)
{
    return (zcloneFile(pFreeSpace,pOutput));
}
*/
/**
 * @brief ZRandomFile::zextendFile will extend the existing file space with pFreeSpace additional bytes.
 *
 * ZRandomFile doe not need to have a manual management of its disk space, as it dynamically allocate new disk space using its extension algorithm.
 * However, it could be smart to allocate so additional free space according the file operation we intend to make, in order to speed up the space allocation process.
 *
 * Additional space will be created as a Free block in free block pool, and will be available for being allocated to ZBAT.
 * Disk space is physically allocated using posix_fallocate, that will insure -best try- to get contiguous disk space.
 * @see ZRFPools
 *  @note the file region that is extended is set to binary zero.
 *
 * @param[in,out] pDescriptor the current file descriptor for file to be extended
 * @param[in] pFreeSpace    additional space to extend the file with
 * @param pOutput
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zextendFile(const zsize_type pFreeSpace)
{
ZBlockDescriptor wHighBlock;
ZStatus wSt;
    wSt=_getExtent(wHighBlock,pFreeSpace);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    wHighBlock.State = ZBS_Free;
    ZFBT.push(wHighBlock);

    return (_writeFCB(true));
}

/**
 * @brief ZRandomFile::ztruncateFile will truncate the file pointed by pDescriptor that must be a ZRandomFile opened file to leave a free block of pFreeSpace at the physical end of the file.
 *
 * @note ztruncateFile may only reduce the amount of file space that is declared as "free", that mean : space allocated to
 *  - free block in FreeBlockPool
 *  - the highest address of the file
 * In other words, it should be the last physical block in the file, and this block must be in the Free pool.
 * @see ZRFPools
 *
 * Before truncating file, it could be required to :
 *  - use zreorgFile to free as much file space as possible and to put it at the highest address in a free block
 *  - OR to use zclearFile : Warning : In this case (using zclearFile), all records are lost.
 *
 * If you mention a desired free space equal to -1, then ztruncateFile will take a last block equal to ZFileControlBlock::BlockTargetSize,
 * under the condition that this value is greater than size of a ZBlockHeader.
 *
 * @param[in] pFreeSpace Minimum amount of free space to be left in a last free block at the physical end of the file.
 * If a value of -1 is mentionned (default value) then a space of ZFileControlBlock::BlockTargetSize will be allocated.
 * @param[in] pOutput
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::ztruncateFile(const zsize_type pFreeSpace, FILE*pOutput)
{
ZStatus wSt;
// search for highest address block within both pools
zaddress_type wHighAddressStart = -1;
ZBlockDescriptor wHighBlock;
long wHighBlockIndex;
zsize_type wFreeSpace = (pFreeSpace < 0)?ZFCB.BlockTargetSize : pFreeSpace ;

    if (wFreeSpace<(sizeof(ZBlockHeader_Export)+2))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVSIZE,
                                    Severity_Severe,
                                    " Requested free block size to truncate file has an invalid size %lld. It must be at least %ld",
                                    wFreeSpace,
                                    (sizeof(ZBlockHeader_Export)+2));
            return  ZS_INVSIZE;
            }

    for (long wi=0 ;wi<(ZBAT.size()-1);wi++)
        if (ZBAT.Tab[wi].Address >= wHighAddressStart)
                    {
                                wHighAddressStart = ZBAT.Tab[wi].Address;
                                wHighBlock= ZBAT.Tab[wi];
                    }

    for (long wi=0;wi < ZFBT.size();wi++)
    if (ZFBT.Tab[wi].Address >= wHighAddressStart)
                        {
                        wHighAddressStart = ZFBT.Tab[wi].Address;
                        wHighBlock= ZFBT.Tab[wi];
                        wHighBlockIndex= wi;
                        }

   if (wHighBlock.State != ZBS_Free )
        {
        _print(
                 "  Warning : highest block is at %ld (physical address) size %ld : this block is in use %s \n"
                 "  In order to get free blocks at the end of the file : \n"
                 "        - use zreorgFile : relative records rank will NOT be modified \n"
                 "        - use zclearFile : Warning : this suppresses ALL records in file\n",
                 wHighBlock.Address,
                 wHighBlock.BlockSize,
                 decode_ZBS(wHighBlock.State));
         return  ZS_SUCCESS;
        }
// We've got a candidate for being the last block to truncate
    if (wHighBlock.BlockSize < wFreeSpace)
            {
            _print(
                     "  Warning : highest block is at %ld (physical address) size %ld : this block has state %s \n"
                     "  This block has a size less than requested size %ld. File is left as it is.\n"
                     "  If you do need more free space at the end of the file \n"
                     "        - use zextend : relative records rank will NOT be modified \n",
                     wHighBlock.Address,
                     wHighBlock.BlockSize,
                     decode_ZBS(wHighBlock.State),
                     pFreeSpace);
            return  ZS_SUCCESS;
            }



    zsize_type wNewSize = wHighBlock.Address ;
    wNewSize += wFreeSpace;

    int wR=ftruncate(ContentFd,(off_t)wNewSize);
    if (wR<0)
            {
            ZException.getFileError(FContent,
                                      _GET_FUNCTION_NAME_,
                                      ZS_FILEERROR,
                                      Severity_Fatal,
                                      "Fatal error while truncating file %s at size %ld",
                                      URIContent.toString(),
                                      wNewSize);
            ZException.exit_abort();
            }
    wHighBlock.BlockSize = wFreeSpace;
    ZFBT.Tab[wHighBlockIndex].BlockSize = wFreeSpace;

    wSt=_writeBlockHeader(wHighBlock,wHighBlock.Address);
    if (wSt!=ZS_SUCCESS)
            {
            return  wSt;
            }

    _print(
             "  File %s has been truncated to %ld bytes\n"
             "  highest free block is at %ld (physical address) size %ld \n",
             URIContent.toString(),
             wNewSize,
             wHighBlock.Address,
             wHighBlock.BlockSize);

    ZFCB.AllocatedSize = wNewSize;

    CurrentRank = -1;
    LogicalPosition = -1;
    PhysicalPosition = -1;

    return  (_writeFCB(true));
} //ztruncateFile


/**
 * @brief zclearFile Clearing file content for the current opened ZRandomFile
 *
 * zclear will consider any block and user record within the ZRandomFile as deleted and unexisting anymore (even in the ZDeletedBlockPool).
 *
 * For doing this, it will
 *  - reset ZBlockAccessTable pool (no more used blocks)
 *  - create one unique free block in ZFreeBlockPool  with a size corresponding to
 * Size of the Content File - size of a BlockHeader - start of data
 *  - this block will be written as the first block of the content file (address start of data).
 *
 *  If highwater marking option has been selected, the freed space (size of free block as computed before) will be binary zeroed according highwater marking algorithm.
 * @see ZRandomFile::_highwaterMark_Block
 *
 *    If ZRandomFile contains records these records will be lost.
 * Current Logical address will be 0L, meaning start of the file.
   If highWaterMarking option has been selected and is on, the existing used file space will be marked to binary zero.
   File space remains allocated and is fully available as one free block with all available space to be used.

   File must be correctly setup (with appropriate path in various uriStrings ).
   If file is open then it will be closed before engaging the process.
   Once the process and been finished, will be re-opened for a later use with its previous open mode, if it was open, or left close if it was close while calling the method.

   @note The whole file space remains allocated to ZRandomFile content file.
   If pSize mentions a value less than current file space, the file is not truncated and file space remains unchanged.
   if pSize is -1, then the existing space is kept.
   if pSize mentions a value greater than actual file space, then file is extended using posix_fallocate and file size is adjusted to the given size.

   In order to reduce the amount of allocated disk space, it is required to use, after having used zclearFile, ZRandomFile::ztruncateFile.

 *@param[in] pSize      number of bytes to be allocated to file. If -1, then existing size is kept.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zclearFile(const zsize_type pSize)
{


ZStatus wSt;
long wi;
zmode_type wMode = ZRF_Nothing ;
ZFile_type wZFT = ZFT_Nothing;
bool FOpen = false;


   if (isOpen())
            {
            FOpen=true;
            wMode=getMode();
            wZFT = ZHeader.FileType;
            zclose();
            }
        else
            {
            if (ZHeader.FileType==ZFT_Nothing)
                        {
                        wSt=ZS_BADFILETYPE;
                        ZException.setMessage(_GET_FUNCTION_NAME_,
                                                ZS_FILETYPEWARN,
                                                Severity_Warning,
                                                " ZFile_type == ZFT_Nothing : it must be set either by a previous zopen or by setting ZHeader::FileType. Set to ZFT_ZRandomFile");
                        wZFT=ZFT_ZRandomFile;
                        }
                else
                    wZFT = ZHeader.FileType;
            }// else
    if ((wSt=_ZRFopen(ZRF_Exclusive|ZRF_All,wZFT))!=ZS_SUCCESS)
                                                return wSt;
    for (wi=0;(wi<ZBAT.size())&&(wSt==ZS_SUCCESS);wi++)
                    {
                    wSt=_freeBlock_Commit(wi,false);
                    }
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

//  Computing size of the mega free block
//          size of content file  - ZFCB.StartOfData
// Computing size of user content for mega free block
//          computed size for mega free block - sizeof(ZBlockHeader_Export)

    zsize_type wFreeBlockSize = 0;
    zsize_type wFreeUserSize = 0;

    // get file size

    if ((wFreeBlockSize=(zsize_type)lseek(ContentFd,0L,SEEK_END))<0)// get the physical file size
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEPOSERR,
                            Severity_Severe,
                            " Severe error while positionning to end of file %s",
                            URIContent.toString());


            wSt=ZS_FILEPOSERR;
            zclose();
            return  wSt;
            }

    // user requested pSize bytes

    wFreeBlockSize -= ZFCB.StartOfData ;
    wFreeUserSize = wFreeBlockSize - sizeof(ZBlockHeader_Export);

    if (wFreeBlockSize < pSize)
        {
        int wS=posix_fallocate(ContentFd,(off_t)wFreeBlockSize,(off_t)(pSize-wFreeBlockSize));
        if (wS!=0)
                {
                ZException.getErrno(wS,
                                _GET_FUNCTION_NAME_,
                                ZS_WRITEERROR,
                                Severity_Severe,
                                " Severe error while extending file space to end of file %s",
                                URIContent.toString());
                wSt= ZS_WRITEERROR;
                zclose();
                return  wSt;
                }
        }
// create one block with all the available space
// write the block descriptor to content file

    ZBlockDescriptor wBS;

    wBS.Address = resetPosition() ; // get the start of data physical address and align logicalPosition

    wBS.BlockSize = wFreeBlockSize;
//    wBS.BlockId = ZBID_Data ;
    wBS.State = ZBS_Free;
//    wBS.Pid   = Pid;  // warning get the current pid

// write this block header to content file at start of data

    wSt=_writeBlockHeader((ZBlockHeader&)wBS,wBS.Address);
    if (wSt!=ZS_SUCCESS)
                {
                zclose();
                return  wSt;
                }
// Then reset all pools
//

    ZDBT.reset();
    ZBAT.reset();
    ZFBT.reset();
// up to here nothing in the file
    ZFBT.push(wBS);  // put block in free block pool
// just the mega free block in free block pool

// Now take care of HighwaterMarking : if no HWM : job is done
    if (ZFCB.HighwaterMarking)      // if highwatermarking then set the file region to zero
        {
        wSt=_highwaterMark_Block(wFreeUserSize);
        if (wSt!=ZS_SUCCESS)
                    {
                    zclose();
                    return  wSt;
                    }
        }// if HighWaterMarking

    fdatasync(ContentFd); // quicker than flush : writes only data and not metadata to file
    ZFCB.UsedSize = 0 ;
//
// current index and positions are invalid after removing of the block from ZBAT

    CurrentRank = -1;
    LogicalPosition = -1;
    PhysicalPosition = -1;
    zclose();                           // nb zclose write the whole file header

    if (FOpen)     // if file was open re-open in same mode
            { return  _ZRFopen(wMode,wZFT);}
    return  ZS_SUCCESS;
}// zclearFile

/**
 * @brief ZRandomFile::zreorgUriFile Will reorganize ZRandomFile's file surface for the file corresponding to the path mentionned in pURI : eliminate remaining holes between holes and sort physical blocks according to their rank (logical position)
 *
 * The file mentionned in pURI is appropriately opened and closed.
 * Current ZRandomFile context (hold by ZFileDescriptor) is not impacted by this operation.
 *
 *   @note This is a static method.
  It opens for reading and close appropriately the given ZRandomFile.
  If this method is used from an object that holds an already opened ZRandomFile, then it reopens it for reading.
  Using it from an active object instance might induce lock problems.

 *
 * using this routine, content file is reorganized internally to the existing file using ZRandomFile capabilities :
 *  - existing freeblocks allocations
 *  - content file space temporary extensions
 *
 * @par Result
 * As a result to this routine :
 *  - holes will be eliminated
 *  - no free block AND no holes will physically remain between used blocks
 *  - only one free block gathering all available space will take place physically at the end of the file
 *  - rank of active records is not changed.
 *
 * So that, at the end of the process, file may be used as it was used previously.
 *
 * @par File size and extensions
 *  During reorganization process, content file will most probably be extended.
 *  To this purpose, there will be a need of disk space during extension :
 *  - depending to the file structure and specifically to the maximum blocksize (ZFileControlBlock::MaxSize) that could be evaluated with routines like ZRandomFile::ZRFstat,
 *    we can however state that one third of the file space will be used as an extension.
 *  - this disk space will be freed at the end of the reorganization process, and content file size will be readjusted to the size it had before the start of the process.
 *
 * @warning this routine cannot be used in a ZMasterFile / ZIndexedFile context. If such a processing is done, integrity of Key indexes is totally lost.
 * Why? Because the main link between index keys and record content is block addresses. zreorgFile changes addresses for blocks, then it will totally mess up index key orders.
 *
 * @param[in] an uriString containing the path for the file to be reorganized
 * @param[in] a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return    a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
ZRandomFile::zreorgUriFile (uriString &pURI, bool pDump, FILE *pOutput)
{


ZStatus wSt;

    wSt=setPath(pURI);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

    return  _reorgFile(pDump,pOutput);
}

/**
 * @brief ZRandomFile::zreorgFile Will reorganize the current ZRandomFile's file surface : eliminate remaining holes between holes and sort physical blocks according to their rank (logical position)
 *
 * using this routine, content file is reorganized internally to the existing file using ZRandomFile capabilities :
 *  - existing freeblocks allocations
 *  - content file space temporary extensions
 *
 * @par Result
 * As a result to this routine :
 *  - holes will be eliminated
 *  - no free block AND no holes will physically remain between used blocks
 *  - only one free block gathering all available space will take place physically at the end of the file
 *  - rank of active records is not changed.
 *
 * So that, at the end of the process, file may be used as it was used previously.
 *
 * @par File size and extensions
 *  During reorganization process, content file will most probably be extended.
 *  To this purpose, there will be a need of disk space during extension :
 *  - depending to the file structure and specifically to the maximum blocksize (ZFileControlBlock::MaxSize) that could be evaluated with routines like ZRandomFile::ZRFstat,
 *    we can however state that one third of the file space will be used as an extension.
 *  - this disk space will be freed at the end of the reorganization process, and content file size will be readjusted to the size it had before the start of the process.
 *
 * @warning this routine cannot be used in a ZMasterFile / ZIndexedFile context. If such a processing is done, integrity of Key indexes is totally lost.
 * Why? Because the main link between index keys and record content is block addresses. zreorgFile changes addresses for blocks, then it will totally mess up index key orders.
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zreorgFile(FILE*pOutput)
{

    return(_reorgFile(pOutput));
}

/**
 * @brief ZRandomFile::_reorgFile Will reorganize the file surface : eliminate remaining holes between holes and sort physical blocks according to their rank (logical position)
 *
 * using this routine, content file is reorganized internally to the existing file using ZRandomFile capabilities :
 *  - existing freeblocks allocations
 *  - content file space temporary extensions
 *
 * @par Result
 * As a result to this routine :
 *  - holes will be eliminated
 *  - no free block AND no holes will physically remain between used blocks
 *  - only one free block gathering all available space will take place physically at the end of the file
 *  - rank of active records is not changed.
 *
 * So that, at the end of the process, file may be used as it was used previously.
 *
 * @par File size and extensions
 *  During reorganization process, content file will most probably be extended.
 *  To this purpose, there will be a need of disk space during extension :
 *  - depending to the file structure and specifically to the maximum blocksize (ZFileControlBlock::MaxSize) that could be evaluated with routines like ZRandomFile::ZRFstat,
 *    we can however state that one third of the file space will be used as an extension.
 *  - this disk space will be freed at the end of the reorganization process, and content file size will be readjusted to the size it had before the start of the process.
 *
 * @warning this routine cannot be used in a ZMasterFile / ZIndexedFile context. If such a processing is done, integrity of Key indexes is totally lost.
 *
 * @param[in] pDescriptor The current ZRandomFile to be reorganized.
 * @param[in] pDump this boolean sets (true) the option of having surfaceScan during the progression of reorganization. Omitted by default (false)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_reorgFile(bool pDump,FILE*pOutput)
{
ZStatus wSt;
bool wgrabFreeSpaceSet = false ;
bool wasOpen=false;
ZFile_type wFileType = ZFT_ZRandomFile;

// Open - reopen the file in exclusive mode

    zmode_type wMode = ZRF_Nothing ;
    if (_isOpen)
             {
             wMode=getMode();
             wFileType = ZHeader.FileType;
             zclose();
             wasOpen=true;
             }
            else
            {
            if ((wSt=_ZRFopen(ZRF_Read_Only,ZFT_ZRandomFile))!=ZS_SUCCESS) // open to get ZFile_type
                             return wSt;
            wFileType = ZHeader.FileType;
            zclose();
            }
    if ((wSt=_ZRFopen(ZRF_Exclusive|ZRF_Read_Only,wFileType))!=ZS_SUCCESS) // open with appropriate ZFile_type
                                                        return wSt;
    if (!ZFCB.GrabFreeSpace)        // activate grabFreeSpace if it has been set on
                {
                ZFCB.GrabFreeSpace=true;
                wgrabFreeSpaceSet = true;
                }

    wSt= _reorgFileInternals(pDump,pOutput);
    if (wSt!=ZS_SUCCESS)
            ZException.printUserMessage(pOutput);

    if (wgrabFreeSpaceSet)        // restore grabFreeSpace if it was off and has been set on
                 {
                 ZFCB.GrabFreeSpace=false;
                 }
    zclose ();
    if (wasOpen)
            _ZRFopen(wMode,wFileType);
    return  (wSt);
} // _reorgFile

ZStatus
ZRandomFile::_reorgFileInternals(bool pDump,FILE*pOutput)
{
ZStatus wSt;
zrank_type    wZBATIdx = 0;
zaddress_type wCurrentAddress  ;
zsize_type    wNeedOfSpace = 0;
ZBlockDescriptor wCurrentBlock;
ZArray<zrank_type> wZAIdx;               // collection of all ranks for blocks to move regardless wether it is ZBAT or ZFBT blocks
ZArray<ZBlockDescriptor> wZABlocksMove ; // descriptors of blocks to move : State field will determine if this is a ZBAT or a ZFBT block

zaddress_type wStartAddress ;
zaddress_type wEndAddress ;
ZBlockDescriptor wBlockReminder;
zaddress_type wMaxAddress= 0;
zaddress_type wMaxLastAddress= 0;

zsize_type wSumUsedSize  = 0;
zsize_type wSumBlockSize = 0;
zsize_type wMinBlockSize ;
zsize_type wMaxBlockSize = 0;
long wOld;

long wi;


_print("ZRandomFile::_reorgFileInternals Starting at %s reorganization of file %s", ZDateFull::currentDateTime().toFormatted().toUtf(),URIContent.toCChar());
// pDescriptor MUST point to an opened ZRandomFile in ZRF_Exclusive move

    wCurrentAddress = ZFCB.StartOfData ;

zsize_type wFileSize = getAllocatedSize(); // get initial allocated size

    // first add a last element in ZBAT that will contain remaining space at each time
    // in order for free space not to be grabbed by _getFreeBlock allocation

    wBlockReminder.clear();
    wMaxAddress= -1;
    wMaxLastAddress= -1;
    wMinBlockSize = ZBAT.Tab[0].BlockSize ;
    wMaxBlockSize = 0;

    wBlockReminder.State = ZBS_Control ; // special ZBS
//    ZBAT.push(wBlockReminder); // created

     while (wZBATIdx<ZBAT.size())
     {

     wCurrentBlock = ZBAT.Tab[wZBATIdx];

     wSumUsedSize += (wCurrentBlock.BlockSize - sizeof(ZBlockHeader_Export));  //  compute sum of users size
     wSumBlockSize += wCurrentBlock.BlockSize ;     // compute sum of BlockSize

     if (wMinBlockSize > ZBAT.Tab[wZBATIdx].BlockSize)
                                    wMinBlockSize = ZBAT.Tab[wZBATIdx].BlockSize;
     if (wMaxBlockSize < ZBAT.Tab[wZBATIdx].BlockSize)
                                    wMaxBlockSize = ZBAT.Tab[wZBATIdx].BlockSize;

     if (ZBAT.Tab[wZBATIdx].Address == wCurrentAddress) // is block already at correct place ?
                    {
                    wCurrentAddress += ZBAT.Tab[wZBATIdx].BlockSize ;    // ready for next block

                    wBlockReminder.Address = wCurrentAddress;
                    wBlockReminder.BlockSize = 0;
//                    ZBAT.last().Address = wCurrentAddress;
//                    ZBAT.last().BlockSize = 0;
                    wZBATIdx ++;
                    continue ;
                    }


     wMaxAddress = -1;
     wMaxLastAddress = -1;
     wZABlocksMove.reset();
     wZAIdx.reset();

     wCurrentBlock = ZBAT.Tab[wZBATIdx] ;
     wNeedOfSpace = wCurrentBlock.BlockSize ;

     wStartAddress = wCurrentAddress;                       // we will implant
     wEndAddress = wStartAddress + wCurrentBlock.BlockSize + 1 ;   // up to here
     if (__ZRFVERBOSE__)
       _print(
              "---------ZBAT rank %ld \n"
              "  Source address %ld block size %ld \n"
              "  Start address %ld End Address %ld  Size needed %ld------\n"
              "%s\n"
              "   ...searching...",
              wZBATIdx,
              wCurrentBlock.Address,
              wCurrentBlock.BlockSize,
              wCurrentAddress,
              wEndAddress,
              wCurrentBlock.BlockSize,
              (wCurrentBlock.Address < wEndAddress) ? "   Warning : destination address overlaps source\n":"\n");
// find all blocks that are present on wCurrentAddress , wNeedOfSpace



// if wBlockReminder has Not enough room to put the current block : search for space
//     if (ZBAT.last().BlockSize < ZBAT.Tab[wZBATIdx].BlockSize)
    if (wBlockReminder.BlockSize < ZBAT.Tab[wZBATIdx].BlockSize) {
        for (long wi=wZBATIdx + 1 ;wi<(ZBAT.size()-1);wi++)
            {
            if ((ZBAT.Tab[wi].Address >= wStartAddress)&&(ZBAT.Tab[wi].Address < wEndAddress))
                        {
                        if (ZBAT.Tab[wi].Address>wMaxAddress)
                                    {
                                    wMaxAddress = ZBAT.Tab[wi].Address;
                                    wMaxLastAddress = wMaxAddress + ZBAT.Tab[wi].BlockSize;
                                    }
                        wZAIdx.push(wi);
                        wZABlocksMove.push(ZBAT.Tab[wi]);
                        if (__ZRFVERBOSE__)
                            _print(
                                 " Selecting for move ZBAT block rank %ld address %ld size %ld ",
                                 wi,
                                 ZBAT.Tab[wi].Address,
                                 ZBAT.Tab[wi].BlockSize);
                        }

            }// for

    for (long wi=0;wi < ZFBT.size();wi++)
            {
    if ((ZFBT.Tab[wi].Address >= wStartAddress)&&(ZFBT.Tab[wi].Address < wEndAddress))
                        {
                        if (ZFBT.Tab[wi].Address>wMaxAddress)
                                        {
                                        wMaxAddress = ZFBT.Tab[wi].Address;
                                        wMaxLastAddress = wMaxAddress + ZFBT.Tab[wi].BlockSize;
                                        }
                        wZAIdx.push(wi);
                        wZABlocksMove.push(ZFBT.Tab[wi]);
                        if (__ZRFVERBOSE__)
                            _print(
                                 " Selecting for deletion ZFBT block rank %ld address %ld size %ld",
                                 wi,
                                 ZFBT.Tab[wi].Address,
                                 ZFBT.Tab[wi].BlockSize);
                        }
            }// for

// up to here we have
// the list of index from ZFBT and ZBAT concerned
// the address to move the ZBAT block wCurrentAddress
// the highest address wMaxLastAddress

    ZBlockMin wBMin;

// delete all ZFBT blocks (State==ZBS_Free)
    wi = 0;
    while (wi<wZABlocksMove.size()) {
      if (wZABlocksMove[wi].State!=ZBS_Free) {
          wi ++;
          continue;
      }
      wZABlocksMove[wi].State = ZBS_Deleted;
      wSt=_writeBlockHeader(wZABlocksMove[wi],wZABlocksMove[wi].Address);
      if (wSt!=ZS_SUCCESS)
                      {return  wSt;}
      ZFBT.erase(wZAIdx[wi]);     // this is now a hole
      wZABlocksMove.erase(wi);    // and forget it
      wZAIdx.erase(wi);           // its index too
    }// while

/* we have to move the ZBAT blocks to another further location either a using existing free block ZFBT or extend the file :
 use standard function _getFreeBlock with ZBATA_Create as pFlag -1 as rank argument : it will return a new ZBAT element that will will suppress later
 Remark : using another index that -1 with INSERT a new ZBAT element at given rank */
    long wi=0;
    while (wi < wZABlocksMove.size()) {
        // move only ZBAT used blocks - not free block ZFBT (will be deleted)

      if (wZABlocksMove[wi].State!=ZBS_Used) {
        wi ++;
        continue;
      }
      if (__ZRFVERBOSE__)
        _print(" ----Moving ZBAT block rank %ld----- \n    Getting free space size %ld",
                    wZAIdx[wi],
                    wZABlocksMove[wi].BlockSize);
      long wFBTRank=0;
      if ((wFBTRank=_getFreeBlockEngine(wZABlocksMove[wi].BlockSize, wEndAddress)) <0) {
        ZException.setMessage(_GET_FUNCTION_NAME_,ZS_CANTALLOCSPACE,Severity_Fatal,
            "Cannot get free block under constraint of address %ld file %s.\n"
            "File reorganization is interrupted. This may cause file structure problems.",wEndAddress,URIContent.toCChar());
        return    ZS_CANTALLOCSPACE;
      }
      wBMin.Address=ZFBT[wFBTRank].Address;
      wBMin.BlockSize=ZFBT[wFBTRank].BlockSize;

      ZFBT.erase(wFBTRank); /* no more needed in free block pool */

      // moving block to make room
      ZBAT.Tab[wZAIdx[wi]].State = ZBS_Used;         // state is still ZBS_Used
      ZBAT.Tab[wZAIdx[wi]].Address = wBMin.Address;  // new address for moved block
      _print(
               " Making room : Moving block ZBAT index %ld from address %ld to address %ld size is %ld",
                wZAIdx[wi],
               wZABlocksMove[wi].Address,
               ZBAT.Tab[wZAIdx[wi]].Address,
               ZBAT.Tab[wZAIdx[wi]].BlockSize);

        wSt=_moveBlock(wZABlocksMove[wi].Address,
                       wBMin.Address,
                       false); // move block without creating an entry in ZFBT - Note : block on file is marked ZBS_Free
        wZABlocksMove.erase(wi);
        wZAIdx.erase(wi);           // its rank too
        if (wSt!=ZS_SUCCESS)
                        {    return  wSt;}
    }//while
    // space for writing block is now free
    // move the block to its new position

    } // if wBlockReminder has not enough space


    if (wCurrentBlock.Address < wEndAddress)
                {
        _print(" Warning : Block Overlap Detected-----------");
        if (pDump)
                    {
            _headerDump();
            _surfaceScan();
                    }
                }
    if (__ZRFVERBOSE__)
        _print(
             " -- Moving block ZBAT index %ld from address %ld to its sorted address %ld  size of block is %ld",
              wZBATIdx,
             ZBAT.Tab[wZBATIdx].Address,
             wCurrentAddress,
             ZBAT.Tab[wZBATIdx].BlockSize);

    wSt=_moveBlock(
                   ZBAT.Tab[wZBATIdx].Address,
                   wCurrentAddress,
                   true);      // move and create a ZFBT entry for the target address

// create a free block in pool for the former block address
// TODO see later highwaterMarking impact

//   ZFBT.push(ZBAT.Tab[wZBATIdx]);
//   ZFBT.last().State =ZBS_Free; // with correct State

    ZBAT.Tab[wZBATIdx].Address = wCurrentAddress;  // update pool with the new address
    if (wSt!=ZS_SUCCESS)
                    { return  wSt;}

    wCurrentAddress += ZBAT.Tab[wZBATIdx].BlockSize ;    // ready for next block

    if (wMaxLastAddress>0) // if there was some blocks moved (ZBAT) or deleted (ZFBT)
            {
//    ZBAT.last().Address = wCurrentAddress;
//    ZBAT.last().BlockSize = wMaxLastAddress-wCurrentAddress;
    wBlockReminder.Address = wCurrentAddress;
    wBlockReminder.BlockSize = wMaxLastAddress-wCurrentAddress;
            }
        else    // if not
            {
            wBlockReminder.BlockSize -= wCurrentBlock.BlockSize ;
            wBlockReminder.Address = wCurrentAddress;
            }
    _print("   Reminder address %ld size %ld    - wMaxAddress %ld wMaxLastAddress  %ld  wCurrentAddress %ld",
             wBlockReminder.Address,
             wBlockReminder.BlockSize,
             wMaxAddress,
             wMaxLastAddress,
             wCurrentAddress);
    if (pDump)
                {
        _headerDump();
        _surfaceScan();
                }


    //_writeFileDescriptor(pDescriptor) ; // needed because we read the descriptor in _scanSurface

    wZBATIdx ++;
     }// while

//
//  Cleaning Free blocks pool
//
  _print(".....Deleting Free blocks....");
  for (wi=0;wi < ZFBT.size();wi++) {
         ZFBT.Tab[wi].State = ZBS_Deleted;
         wSt=_writeBlockHeader(ZFBT.Tab[wi],ZFBT.Tab[wi].Address);
         if (wSt!=ZS_SUCCESS)
                     { return  wSt;}
  }
  ZFBT.clear();
  _print(" <%ld> free blocks deleted\n",wi);

// compute now free block is what remains after the last address
// at this stage, remaining free blocks will be at the upper stack of the file
// Only one free block gathering the whole space should be set.

    wCurrentBlock.clear();
    wCurrentBlock.Address = wCurrentAddress ;
    wCurrentBlock.State = ZBS_Free;

    if ((wCurrentBlock.BlockSize=(zsize_type)lseek(ContentFd,0L,SEEK_END))<0)    // position to end of file and get position in return as file size
                {
                ZException.getErrno(errno,
                                _GET_FUNCTION_NAME_,
                                ZS_FILEPOSERR,
                                Severity_Severe,
                                " Severe error while positionning to end of file %s",
                                URIContent.toString());


                wSt=ZS_FILEPOSERR;
                return  wSt;
                }
//
    wCurrentBlock.BlockSize -= wCurrentAddress;
    ZFBT.push(wCurrentBlock);
    wSt=_writeBlockHeader(wCurrentBlock,wCurrentAddress);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}
    if (__ZRFVERBOSE__)
        _print(
             " creating one free block gathering all available space\n"
             "    free block is address %ld blocksize %ld",
             wCurrentBlock.Address,
             wCurrentBlock.BlockSize);

    wSt=_writeFileHeader(true);
    if (wSt!=ZS_SUCCESS)
                {  return  wSt;}

    _print("  Physical file surface reorganization finished successfully at %s",ZDateFull::currentDateTime().toFormatted().toUtf());

// compute Block Target Size
    _print(" ----------adjusting file size to initial value %ld",wFileSize);

    if (getAllocatedSize()<=wFileSize) {
      _print(" No need to readjust file size.");
            }
    else {
      zsize_type wS=wFileSize-wCurrentBlock.Address;
      if (wS>0) {
        wSt=ztruncateFile(wS,pOutput);
        if (wSt!=ZS_SUCCESS)
                    {  return  wSt;}
      }
    }// else

    _print(" -----------------Adjusting values -----------------------");

    wOld = ZFCB.BlockTargetSize ;
    ZFCB.BlockTargetSize  = (long)((float)wSumBlockSize / (float)ZBAT.size());
    _print(
             " BlockTargetSize former value %ld  new value %ld.",
             wOld,
             ZFCB.BlockTargetSize);

    wOld = ZFCB.UsedSize ;
    ZFCB.UsedSize  = (long)((float)wSumUsedSize / (float)ZBAT.size());
    _print(
             " UsedSize former value %ld  new value %ld.",
             wOld,
             ZFCB.UsedSize);


    _print(
             " MinSize- Minimum block size found : former value %ld  new value %ld.",
             ZFCB.MinSize,
             wMinBlockSize
             );
    ZFCB.MinSize=wMinBlockSize;
    _print(
             " MaxSize- Maximum block size found : former value %ld  new value %ld.",
             ZFCB.MaxSize,
             wMaxBlockSize
             );


    ZFCB.MaxSize=wMaxBlockSize;

    CurrentRank = -1;
    LogicalPosition = -1;
    PhysicalPosition = -1;
    return  wSt;
} // _reorgFileInternals



void ZRandomFile::_print(const char* pFormat,...) {
  utf8VaryingString wOut;
  va_list ap;
  va_start(ap, pFormat);
  wOut.vsnprintf(500,pFormat,ap);
  va_end(ap);
  _print(wOut);
}

void ZRandomFile::_print(const utf8VaryingString& pOut) {
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



/** @cond Test

 * @brief ZRandomFile::putTheMess  For test purposes changes blocks allocations
 */
void
ZRandomFile::putTheMess (void)
{

    ZBAT.swap(1,3) ;
    ZBAT.swap(0,5) ;
    ZBAT.swap(2,7) ;
    _writeFCB(true);
    return;
}
//! @endcond

//!@var __HIGHWATER_MODULO__
//! @{
//! highwater write is done modulo 2^25 : meaning 33'554'432 bytes
//! this value could be changed by changing __HIGHWATER_MODULO__ preprocessor parameter
//!
#define __HIGHWATER_MODULO__  pow(2,25)
//!@}

/**
  @cond Development
 * @brief ZRandomFile::_highwaterMark_Block Marks the file region to binary zero- File must be EXACTLY positionned at the beginning of region to mark
 * @param [in] pDescriptor
 * @param [in] pFreeUserSize
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_highwaterMark_Block (const zsize_type pFreeUserSize)
{
ZDataBuffer wHWBuffer;
ssize_t     wSWrite;
zsize_type  wFreeUserSize=pFreeUserSize;
// HighwaterMarking process : if space to mark is greater than __HIGHWATER_MODULO__

//    zsize_type wModulo = __HIGHWATER_MODULO__ ;  // for debug
    if (pFreeUserSize >__HIGHWATER_MODULO__)
        {
        wHWBuffer.allocate(__HIGHWATER_MODULO__);
        wHWBuffer.clearData();                              // set buffer to binary zero
        while (wFreeUserSize >__HIGHWATER_MODULO__)
                {
                ZPMS.HighWaterWrites ++;

                wSWrite=write (ContentFd,wHWBuffer.DataChar, wHWBuffer.Size);
                if (wSWrite<0)
                        {
                        ZException.getErrno(errno,
                                         _GET_FUNCTION_NAME_,
                                         ZS_WRITEERROR,
                                         Severity_Severe,
                                         "Severe Error while high water marking freed block for file %s at address %lld",
                                         URIContent.toString(),
                                         getPhysicalPosition());
                         return  (ZS_WRITEERROR);
                        }

                incrementPhysicalPosition(wSWrite);
                wFreeUserSize -= __HIGHWATER_MODULO__;
                } // while
        } // if (wFreeUserSize >__HIGHWATER_MODULO__)
    wHWBuffer.allocate(pFreeUserSize);
    wHWBuffer.clearData();
    ZPMS.HighWaterWrites ++;

    wSWrite=write (ContentFd,wHWBuffer.DataChar, wHWBuffer.Size);
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Severe,
                             "Severe Error while high water marking freed block for file %s at address %lld",
                             URIContent.toString(),
                             getPhysicalPosition());
            return  (ZS_WRITEERROR);
            }
    incrementPhysicalPosition(wSWrite);
    return  ZS_SUCCESS;
}

/** @endcond  */
/*
int ZRandomFile::fromXml(zxmlNode* pIndexRankNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pIndexRankNode->getChildByName((zxmlNode *&) wRootNode, "icbowndata");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "FieldDescription::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "icbowndata",
        decode_ZStatus(wSt));
    return -1;
  }
  if (XMLgetChildText(wRootNode, "name", Name, pErrorlog) < 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "name");
  }



  if (XMLgetChildULong(wRootNode, "icbtotalsize", ICBTotalSize, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "icbtotalsize");
  }
  if (XMLgetChildULong(wRootNode, "zkdicoffset", ZKDicOffset, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "zkdicoffset");
  }


  if (XMLgetChildUInt(wRootNode, "keytype", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "keytype");
  }
  else
    KeyType = (ZIFKeyType_type)wInt;

  if (XMLgetChildUInt(wRootNode, "autorebuild", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "autorebuild");
  }
  else
    AutoRebuild = (uint8_t)wInt;

  if (XMLgetChildUInt(wRootNode, "duplicates", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "duplicates");
  }
  else
    Duplicates = (ZSort_Type)wInt;

  return (int)pErrorlog->hasError();
}//ZRandomFile::fromXml

*/

/** @} ZRFUtilities */


/** @addtogroup ZRFFreeSearch
@{
*/
/**
 * @brief ZRandomFile::zsearchFieldAllCollection Sequential search on a given collection of records (All records of the collection)
 * for a given sequence (pSearch) into a field defined by its offset from beginning of record
 * If pCollection is omitted, then the search is made on all active records of the ZRandomFile (ZBlockAccessTable - ZBAT)
 *
 * You should first defines a field (a segment of user's record) from beginning of user's record (offset)
 * and its length in bytes for searching in the entire file
 *
 * Search is made on a collection of records ranks given by pCollection (ZArray<ssize_t>).
 *
 * As Search can be made recursively using each time ZRFCollection as collection of records ranks to find.
 * (A safe copy is made in the routine)
 *
 * Then searches if the content of Sequence matches somewhere in this field for the set or selected records.
 *
 * Returns a ZRFCollection containing
 * - the status of the operation
 * - a ZArray with the ranks of found records
 *
 * @note If pSearch as a sequence is a string
 * - Either keep in mind that the ending character '\0´ must be excluded.
 * Example
 * @code
 * ZDataBuffer wSearch;
 * wSearch = "string to search";
 * wSearch.Size -- ; // mandatory because must exclude trailing '\0' char, while operator '=' includes it into ZDataBuffer's size.
 * @endcode
 * - or you should use ZRFString as a search mask for @ref pSearchType. In this case, trailing '\0' char will be automatically omitted.
 *
 * Additionally, trailing '\0' from fields read from file will also be omitted when comparing key to record field extraction.
 *
 * Example :
 * @code
 * uchar wSearchType = ZRFEndsWith | ZRFString
 * @endcode
 *
 *
 * @warning As it is a sequential search for records, it may induce performance issues
 *
 * @param[in] pOffset     offset of field from start of user record to search within
 * @param[in] pLength     length of field to search within
 *
 * @param[in] pSequence   ZDataBuffer containing the binary sequence or the string sequence to find (depending on pSearchType).

 * @param[out] pMRes      a pointer to a newly created ZRFCollection object that will contain in return the result of the search.
 *
 * @param[in] pSearchType a ZRFSearch_type that indicates how to search within field (defaulted to ZStartsWith)

 * @param[in] pCollection a pointer to a ZArray<ssize_t> (Base of ZRFCollection) containing the ranks of records to search for value in defined field.
 * Defaulted to nullptr. If omitted (nullptr), then the whole active records set for the ZRandomFile is taken as input for testing fields values.
 *

 *
 * @return  a ZStatus that gives the result of the operation (equals to ZRFCollection::ZSt)
 *          if at least one record is found, this status is set to ZS_FOUND.
 *          In case of error(mostly file error), this status is set to appropriate status while ZException contains the error exception messages.
 *
 */
ZStatus
ZRandomFile::zsearchFieldAllCollection (ZRFCollection &pCollection,
                                        const zlockmask_type pLock,
                                        ZRFCollection* pInputCollection)
{
ZStatus wSt;
    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FILENOTOPEN,
                                Severity_Severe,
                                " File <%s> must be open to perform requested operation",
                                URIContent.toString());
        return ZS_FILENOTOPEN;
        }
//    wSt=pMRes.initSearch(this,pOffset,pLength,pSequence,pSearchType,pCollection);
    wSt=pCollection.initSearch(this,pLock,pInputCollection);

    if (wSt!=ZS_SUCCESS)
                    return wSt;


    return pCollection.getAllSelected(pLock);;
} //zsearchFieldCollection

/**
 * @brief ZRandomFile::zsearchFieldFirstCollection
 * @param[out] pMRes        Collection of ranks to build
 * @param[in] pCollection   Collection of ranks in input to base the search on
 * @return
 */
ZStatus
ZRandomFile::zsearchFieldFirstCollection(ZDataBuffer &pRecordContent,
                                         zrank_type &pZRFRank,
                                         ZRFCollection &pCollection,
                                         const zlockmask_type pLock,
                                         ZRFCollection *pInputCollection)
{
ZStatus wSt;
    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FILENOTOPEN,
                                Severity_Severe,
                                " File <%s> must be open to perform requested operation",
                                URIContent.toString());
        return ZS_FILENOTOPEN;
        }
    pCollection.reset();
    wSt=pCollection.initSearch(this,pLock,pInputCollection);
    if (wSt!=ZS_SUCCESS)
                    return wSt;

    return zsearchFieldNextCollection (pRecordContent,
                                       pZRFRank,
                                       pCollection);
}// zsearchFieldFirstCollection

/**
 * @brief ZRandomFile::zsearchFieldNextCollection
 * @param[out] pMRes        Collection of ranks to build
 * @param[in] pCollection   Collection of ranks in input to base the search on
 * @return
 */
/**
 * @brief ZRandomFile::zsearchFieldNextCollection gets next selected ZRandomFile record and rank using store Argument rules. Populates collection with found record.
 * @param[out] pRecordContent matched ZRandomFile record content if successfull. clear() if not
 * @param[out] pZRFRank matched ZRandomFile rank if successfull or -1 if not
 * @param[out] pCollection     a pointer to the collection to feed with next matched rank
 * @return a ZStatus with the following possible values
 * - ZS_FOUND a matching record has been found, its record content is returned in pRecordContent and its rank is returned in pZRFRank
 * - ZS_EOF  no matching records have been found till the end of ZRandomFile. pRecordContent is cleared and pZRFRank is set to -1
 * - ZS_LOCKED mask (ZS_LOCKREAD, etc.) if accessed ZRandomFile record is locked (see ZRFCollection::zgetNextRetry() )
 * - Other file errors in case of severe file error. ZException is set with appropriate message.
 */
ZStatus
ZRandomFile::zsearchFieldNextCollection(ZDataBuffer &pRecordContent,
                                        zrank_type &pZRFRank,
                                        ZRFCollection &pCollection)
{
ZStatus wSt;
    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_FILENOTOPEN,
                                Severity_Severe,
                                " File <%s> must be open to perform requested operation",
                                URIContent.toString());
        return ZS_FILENOTOPEN;
        }

    if (!pCollection.isInit())
        {
        wSt=pCollection.initSearch(this,ZLock_Nolock,nullptr);
        if (wSt!=ZS_SUCCESS)
                    return wSt;
        }

    return pCollection.getNextSelectedRank(pRecordContent,pZRFRank);

} //zsearchFieldNextCollection

//----------------End Search sequence----------------------------------------------------

/** @} */ // ZRFFreeSearch


/** @} */ // ZRandomFileGroup





} // namespace zbs


//--------Static functions------------------------
//

/**
 * @ingroup ZRFPhysical
 * @brief generateURIHeader Generates the appropriate ZRandomFile Header file name from the content file path name (user name)
 *
 *  Preprocessor parameter __HEADER_FILEEXTENSION__ gives the extension name of all header files.
 *  This parameter is set to <.zrh> by default.
 *
 *  This parameter is defined in file zrandomfiletypes.h
 *
 *  @warning It is then forbidden to give a ZRandomFile such an extension.
 * If so, an error will occur when trying to create the ZRF file,  ZS_INVNAME will be returned and ZException will be setup appropriately.
 * In other cases, ZS_SUCCES will be returned
 * @param [in]  pURIPath      uriString containing the user named ZRandomFile path    (Input)
 * @param [out] pURIHeader    uriString containing the generated header file name     (Output)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
generateURIHeader (uriString pURIPath,uriString &pURIHeader)
{
    utf8String wExt=pURIPath.getFileExtension();

//    const utf8_t* wExtension=(const utf8_t*)__HEADER_FILEEXTENSION__ + 1;
//     if (wExtension[0]=='.')
//             wExtension++;       // +1 because we should miss the '.' char
//     if (!wExt.compare(wExtension))
  if (wExt==(const utf8_t*)__HEADER_FILEEXTENSION__ + 1)  // +1 because we should miss the '.' char
       {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_INVNAME,
                               Severity_Error,
                               "File name is malformed. Extension <%s> is reserved to header file while given file name is <%s>",
                               __HEADER_FILEEXTENSION__,
                               pURIPath.toString());
       return ZS_INVNAME;
       }

    pURIHeader=pURIPath;
    utf8_t *wPtr=pURIHeader.strrchr((utf8_t)'.');
    if (wPtr) // '.' is found cut the string at this position and pad with zeroes
            {
            wPtr[0]='\0';
            }

    pURIHeader.add(__HEADER_FILEEXTENSION__);
    return ZS_SUCCESS;
}


/**
 * @brief decode_ZBS gives the key word content of a ZBlockState_type using its code
 * @param pZBS  the code to decode
 * @return      a constant string with the keyword
 */
utf8VaryingString ZBSOut;
const char *
decode_ZBS (uint8_t pZBS)
    {
    switch (pZBS) {
        case ZBS_Nothing :
          return ("ZBS_Nothing");
        case ZBS_Used :
          return ("ZBS_Used");
        case ZBS_Free :
          return ("ZBS_Free");
        case ZBS_Deleted :
          return ("ZBS_Deleted");
        case ZBS_BeingDeleted :
          return ("ZBS_BeingDel");

        case ZBS_Allocated :
          return ("ZBS_Alloctd");
        case ZBS_AllocFromDelete :
          return ("ZBS_AllFDel");

        case ZBS_Control :
          return ("ZBS_Control");

        case ZBS_Split :
          return ("ZBS_Split");

        case ZBS_Invalid :
          return ("ZBS_Invalid");
/*        case ZBS_SplitPrev :
          return ("ZBS_SpltPrv");
        case ZBS_SplitNext :
          return ("ZBS_SpltNxt");
*/
        default : {
          ZBSOut.clear();
          if (pZBS & ZBS_Used)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_Used");
          if (pZBS & ZBS_Free)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_Free");
          if (pZBS & ZBS_Deleted)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_Deleted");
          if (pZBS & ZBS_Used)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_BeingDel");
          if (pZBS & ZBS_Allocated)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_Allocated");
          if (pZBS & ZBS_AllocFromDelete)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_AllFDel");
          if (pZBS & ZBS_Control)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_Control");
          if (pZBS & ZBS_Split)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_Split");
          if (pZBS & ZBS_Invalid)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_Invalid");
/*          if (pZBS & ZBS_SplitPrev)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_SpltPrv");
          if (pZBS & ZBS_SplitNext)
            ZBSOut.addConditionalOR((utf8_t*)"ZBS_SpltNxt");
*/
          return ZBSOut.toCChar();
        }

}//switch
}//decode_ZBS

const char *
decode_ZBID (ZBlockId pZBID)
    {
    switch (pZBID)
            {
            case ZBID_Nothing :
                    {
                        return ("ZBID_Nothing");
                    }
            case ZBID_HCB :
                    {
                        return ("ZBID_FileHeader");
                    }

            case ZBID_FCB :
                {
                    return ("ZBID_FCB");
                }
    case ZBID_MCB :
        {
            return ("ZBID_MCB");
        }

        case ZBID_ICB :
            {
                return ("ZBID_ICB");
            }
        case ZBID_Data :
            {
                return ("ZBID_Data");
            }
    default :
            {
                return ("Unknownn ZBlockId");
            }
}//switch
}//decode_ZBS

utf8String wMode;

const char *
decode_ZRFMode (zmode_type pZRF)
{

  if (pZRF==ZRF_NotOpen)
    return "ZRF_NotOpen";

    if (pZRF&ZRF_Exclusive)
      wMode= (const utf8_t*)"ZRF_Exclusive";

    if (pZRF & ZRF_ManualLock)
      wMode.addConditionalOR( (utf8_t*)"ZRF_ManualLock");
    if (pZRF&ZRF_Read_Only)
      wMode.addConditionalOR( (utf8_t*)"ZRF_Read");
    if (pZRF&ZRF_Write_Only)
      wMode.addConditionalOR( (utf8_t*)"ZRF_Write");
    if (pZRF&ZRF_Delete_Only)
      wMode.addConditionalOR( (utf8_t*)"ZRF_Delete");
    if (wMode.isEmpty())
                return ("Unknownn ZRFMode_type");
    wMode+=(utf8_t)'\0';
    return wMode.toCChar();
}//decode_ZRFMode

zmode_type
encode_ZRFMode (char *pZRF)
{
zmode_type wRet = ZRF_Nothing;

  if (strstr(pZRF,"ZRF_NotOpen" )!=nullptr)
      return ZRF_NotOpen;

    if (strstr(pZRF,"ZRF_Exclusive" )!=nullptr)
                            wRet |= ZRF_Exclusive;
    if (strstr(pZRF,"ZRF_ManualLock" )!=nullptr)
                            wRet |= ZRF_ManualLock;

    if (strstr(pZRF,"ZRF_Read_Only" )!=nullptr)
                            wRet |= ZRF_Read_Only;
    if (strstr(pZRF,"ZRF_Write_Only" )!=nullptr)
                            wRet |= ZRF_Write_Only;
    if (strstr(pZRF,"ZRF_Delete_Only" )!=nullptr)
                            wRet |= ZRF_Delete_Only;
    if (strstr(pZRF,"ZRF_All" )!=nullptr)
                            wRet |= ZRF_All;
    if (strstr(pZRF,"ZRF_Modify" )!=nullptr)
                            wRet |= ZRF_Modify;
    if (strstr(pZRF,"ZRF_Write" )!=nullptr)
                            wRet |= ZRF_Write;
    if (strstr(pZRF,"ZRF_Delete" )!=nullptr)
                            wRet |= ZRF_Delete;
    return wRet;
}//encode_ZRFMode

const char *
decode_ZFile_type (uint8_t pType)
    {
    switch (pType)
            {
            case ZFT_Nothing :
                    {
                        return ("ZFT_Nothing");
                    }
            case ZFT_ZRandomFile :
                    {
                        return ("ZFT_ZRandomFile");
                    }

            case ZFT_ZRawMasterFile :
                {
                    return ("ZFT_ZRawMasterFile");
                }
            case ZFT_ZIndexFile :
                {
                    return ("ZFT_ZIndexFile");
                }
        case ZFT_ZMasterFile :
            {
                return ("ZFT_ZMasterFile");
            }
        case ZFT_DictionaryFile :
            {
              return ("ZFT_DictionaryFile");
            }
    default :
            {
                return ("Unknownn ZFile_type");
            }
    }//switch
}//encode_ZFile_type

ZFile_type
encode_ZFile_type (char* pType)
{

    if (strcmp(pType,"ZFT_ZRandomFile"))
                    return ZFT_ZRandomFile;
    if (strcmp(pType,"ZFT_ZMasterFile"))
                    return ZFT_ZMasterFile;
    if (strcmp(pType,"ZFT_ZIndexFile"))
                    return ZFT_ZIndexFile;
    return ZFT_Nothing;

}//encode_ZFile_type


#endif //ZRANDOMFILE_CPP
