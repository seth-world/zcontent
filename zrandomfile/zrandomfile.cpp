#ifndef ZRANDOMFILE_CPP
#define ZRANDOMFILE_CPP

#include <zcontentcommon/zcontentconstants.h>

#include <zrandomfile/zrandomfile.h>
#include <zrandomfile/zrfcollection.h>
#include <ztoolset/zdatabuffer.h>

#include <ztoolset/charman.h>

#include <stdio.h>
#include <unistd.h>
//===================== Open ZRandomFiles management====================================================
namespace zbs { // see <ztoolset/zmodulestack.h> for global/external conditional definition  (__USE_ZRANDOMFILE__ must be defined in zconfig.h)

//OpenZRFPool GZRFPool;
//ZOpenZRFPool* ZRFPool=&GZRFPool;

ZStatus
ZOpenZRFPool::removeFileByFd(int pFd)
{
ZStatus wSt=ZS_NOTFOUND;

    for (long wi=0;wi<size();wi++)
    {
        wSt=ZS_FILENOTOPEN;
        if (pFd==Tab[wi]->ContentFd)
        {
        if (Tab[wi]->_isOpen)
            {
            wSt=ZRandomFile::_close(*Tab[wi]);
            }
        erase(wi,1);
        break;
        }
    }// for

    return wSt;
}// removeFileByFd

ZStatus
ZOpenZRFPool::removeFileByObject(zbs::ZFileDescriptor *pZRF)
{
ZStatus wSt=ZS_NOTFOUND;

    for (long wi=0;wi<size();wi++)
    {
        wSt=ZS_FILENOTOPEN;
        if (pZRF==Tab[wi])
        {
        if (Tab[wi]->_isOpen)
            {
            wSt=ZRandomFile::_close(*Tab[wi]);
            }
        erase(wi,1);
        break;
        }
    }// for

    return wSt;
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
        ZFileDescriptor* wZRF=popRP(&wZRF);
        if (wZRF->_isOpen)
            {
//            if (__ZRFVERBOSE__)
//                {
                fprintf(stdout,"Closing file <%s>...\n",
                        wZRF->getURIContent().toString());
//                }//__ZRFVERBOSE__
            wSt=ZRandomFile::_close(*wZRF);
            wNB++;
//            if (__ZRFVERBOSE__)
//                {
            if (wSt==ZS_SUCCESS)
                    fprintf(stdout," Successfully closed.\n");
                else
                    fprintf(stdout," error <%s> on close\n",decode_ZStatus(wSt));
//                }//    __ZRFVERBOSE__
            }// wZRF->isOpen()

    }// while size()
//    if (__ZRFVERBOSE__)
        fprintf (stdout,"Number of ZRandomFiles processed <%ld>.\n",wNB);

    return;
}


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
|          - ZSIndexFile :    ZIndexControlBlock_Export
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



//----------ZRFPMS---------------------------------

/**
 * @brief ZRFPMS::operator - Operator overload : subtracts two ZRFPMS
 * @param pPMSIn
 * @return
 */
ZRFPMS
ZRFPMS::operator - (ZRFPMS &pPMSIn)
    {
    ZRFPMS wZRFPMS1;
    wZRFPMS1 = *this;
    wZRFPMS1.HFHReads -= pPMSIn.HFHReads;
    wZRFPMS1.HFHWrites -= pPMSIn.HFHWrites;
    wZRFPMS1.HReservedReads -= pPMSIn.HReservedReads;
    wZRFPMS1.HReservedWrites -= pPMSIn.HReservedWrites;
    wZRFPMS1.HFDReads -= pPMSIn.HFDReads;
    wZRFPMS1.HFDWrites -= pPMSIn.HFDWrites;
    wZRFPMS1.CBHReads -= pPMSIn.CBHReads;
    wZRFPMS1.CBHReadBytesSize -= pPMSIn.CBHReadBytesSize;
    wZRFPMS1.CBHWrites -= pPMSIn.CBHWrites;
    wZRFPMS1.CBHWriteBytesSize -= pPMSIn.CBHWriteBytesSize;
    wZRFPMS1.UserReads -= pPMSIn.UserReads;
    wZRFPMS1.UserWrites -= pPMSIn.UserWrites;
    wZRFPMS1.UserWriteSize -= pPMSIn.UserWriteSize;
    wZRFPMS1.UserReadSize -= pPMSIn.UserReadSize;
    wZRFPMS1.FieldReads -= pPMSIn.FieldReads;
    wZRFPMS1.FieldWrites -= pPMSIn.FieldWrites;
    wZRFPMS1.FieldWriteSize -= pPMSIn.FieldWriteSize;
    wZRFPMS1.FieldReadSize -= pPMSIn.FieldReadSize;
    wZRFPMS1.HighWaterWrites -= pPMSIn.HighWaterWrites;
    wZRFPMS1.HighWaterBytesSize -= pPMSIn.HighWaterBytesSize;
    wZRFPMS1.ExtentWrites -= pPMSIn.ExtentWrites;
    wZRFPMS1.ExtentSize -= pPMSIn.ExtentSize;
    wZRFPMS1.FreeMatches -= pPMSIn.FreeMatches;
    return wZRFPMS1 ;
 }
/**
 * @brief ZRFPMS::operator + operator overload : adds two ZRFPMS
 * @param pPMSIn
 * @return
 */
ZRFPMS
ZRFPMS::operator + (ZRFPMS &pPMSIn)
    {
    ZRFPMS wZRFPMS1;
    wZRFPMS1 = *this;
    wZRFPMS1.HFHReads += pPMSIn.HFHReads;
    wZRFPMS1.HFHWrites += pPMSIn.HFHWrites;
    wZRFPMS1.HReservedReads += pPMSIn.HReservedReads;
    wZRFPMS1.HReservedWrites += pPMSIn.HReservedWrites;
    wZRFPMS1.HFDReads += pPMSIn.HFDReads;
    wZRFPMS1.HFDWrites += pPMSIn.HFDWrites;
    wZRFPMS1.CBHReads += pPMSIn.CBHReads;
    wZRFPMS1.CBHReadBytesSize += pPMSIn.CBHReadBytesSize;
    wZRFPMS1.CBHWrites += pPMSIn.CBHWrites;
    wZRFPMS1.CBHWriteBytesSize += pPMSIn.CBHWriteBytesSize;
    wZRFPMS1.UserReads += pPMSIn.UserReads;
    wZRFPMS1.UserWrites += pPMSIn.UserWrites;
    wZRFPMS1.UserWriteSize += pPMSIn.UserWriteSize;
    wZRFPMS1.UserReadSize += pPMSIn.UserReadSize;
    wZRFPMS1.FieldReads += pPMSIn.FieldReads;
    wZRFPMS1.FieldWrites += pPMSIn.FieldWrites;
    wZRFPMS1.FieldWriteSize += pPMSIn.FieldWriteSize;
    wZRFPMS1.FieldReadSize += pPMSIn.FieldReadSize;
    wZRFPMS1.HighWaterWrites += pPMSIn.HighWaterWrites;
    wZRFPMS1.HighWaterBytesSize += pPMSIn.HighWaterBytesSize;
    wZRFPMS1.ExtentWrites += pPMSIn.ExtentWrites;
    wZRFPMS1.ExtentSize += pPMSIn.ExtentSize;
    wZRFPMS1.FreeMatches += pPMSIn.FreeMatches;
    return wZRFPMS1 ;
 }

/**
 * @brief ZRFPMS::reportDetails detailed report of PMS data
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZRFPMS::reportDetails (FILE*pOutput)
{
    long long wAverageUserWrite;
    if ((UserWrites)==0)
                wAverageUserWrite = 0;
        else
        wAverageUserWrite=(long long)((float)UserWriteSize / (float)UserWrites);
    long long wAverageUserRead;
    if ((UserReads)==0)
                wAverageUserRead = 0;
        else
        wAverageUserRead=(long long)((float)UserReadSize / (float)UserReads);
    long long wAverageExtent;
    if ((ExtentWrites)==0)
                wAverageExtent = 0;
        else
        wAverageExtent=(long long)((float)ExtentSize / (float)ExtentWrites);

    long long wAverageFieldRead;
    if (FieldReads  ==0)
                wAverageFieldRead = 0;
        else
        wAverageFieldRead=(long long)((float)FieldReadSize / (float)FieldReads);

    long long wAverageFieldWrite;
    if (FieldWrites  ==0)
                wAverageFieldWrite = 0;
        else
        wAverageFieldWrite=(long long)((float)FieldWriteSize / (float)FieldWrites);

    fprintf (pOutput,
             "________________________________________________\n"
             " Header\n"
             "   File header reads          %10ld\n"
             "               writes         %10ld\n"
             "   File lock info read        %10ld\n"
             "                  write       %10ld\n"
             "   File descriptor reads      %10ld\n"
             "                   writes     %10ld\n"
             "   Reserved block reads       %10ld\n"
             "                  writes      %10ld\n"
             " Content\n"
             "   Block header reads         %10ld\n"
             "                size          %10lld\n"
             "                writes        %10ld\n"
             "                size          %10lld\n"
             "   User content reads         %10ld\n"
             "                size          %10lld\n"
             "             average/read     %10lld\n"

             "   User content writes        %10ld\n"
             "                size          %10lld\n"
             "             average/write    %10lld\n"

             "   Field content reads        %10ld\n"
             "                size          %10lld\n"
             "             average/read     %10lld\n"

             "   Field content writes       %10ld\n"
             "                size          %10lld\n"
             "             average/write    %10lld\n"

             "   Highwater marking writes   %10ld\n"

             "                     size     %10lld\n"

             "   Free pool matches          %10ld\n"
             "   Free pool mismatches :\n"
             "   Extent writes              %10ld\n"
             "          size                %10lld\n"
             "      extent average/write    %10lld\n"

             "________________________________________________\n",

             HFHReads,
             HFHWrites,
             LockReads,
             LockWrites,
             HFDReads,
             HFDWrites,
             HReservedReads,
             HReservedWrites,

             CBHReads,
             CBHReadBytesSize,
             CBHWrites,
             CBHWriteBytesSize,

             UserReads,
             UserReadSize,
             wAverageUserRead,

             UserWrites,
             UserWriteSize,
             wAverageUserWrite,

             FieldReads,
             FieldReadSize,
             wAverageFieldRead,

             FieldWrites,
             FieldWriteSize,
             wAverageFieldWrite,

             HighWaterWrites,
//             pDescriptor.ZFCB->HighwaterMarking?"<On>":"<Off>",
             HighWaterBytesSize,

             FreeMatches,
             ExtentWrites,
             ExtentSize,
             wAverageExtent
             );

    return ;
}

//! @cond Development

void
ZRFPMS::PMSCounterRead(ZPMSCounter_type pC, const zsize_type pSize)
{
    switch (pC)
    {
    case ZPMS_BlockHeader :
            {
            CBHReads += 1;
            CBHReadBytesSize += pSize;
            return;
            }
    case ZPMS_User :
            {
            UserReads += 1;
            UserReadSize += pSize;
            return;
            }
    case ZPMS_Field :
            {
            FieldReads += 1;
            FieldReadSize += pSize;
            return;
            }
    default:
    {
        return;
    }

     }
} //PMSCounterRead
void
ZRFPMS::PMSCounterWrite(ZPMSCounter_type pC, const zsize_type pSize)
{
    switch (pC)
    {
    case ZPMS_BlockHeader :
            {
            CBHWrites += 1;
            CBHWriteBytesSize += pSize;
            return;
            }
    case ZPMS_User :
            {
            UserWrites += 1;
            UserWriteSize += pSize;
            return;
            }
    case ZPMS_Field :
            {
            FieldWrites += 1;
            FieldWriteSize += pSize;
            return;
            }
    default:
    {
        return;
    }
     }
} //PMSCounterWrite

//! @endcond

//----------End ZRFPMS---------------------------------
//================================= ZBlockPool =====================

/**
 * @brief ZBlockPool::_exportPool DO NOT USE ZArray Standard import/export facilities for Block pools
 * @param pZDBExport
 * @return
 */
ZDataBuffer&
ZBlockPool::_exportPool(ZDataBuffer&pZDBExport)
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

    ZAE.CurrentSize  = ZAE.NbElements*sizeof(ZBlockDescriptor_Export);     // taking the CurrentSize
    ZAE.AllocatedSize = ZAE.AllocatedElements*sizeof(ZBlockDescriptor_Export); // get the total allocated size

    ZAE.DataSize = ZAE.CurrentSize; // change to allocatedsize if required

    ZAE.FullSize = ZAE.DataSize+sizeof(ZAExport)+1;

    pZDBExport.allocateBZero(ZAE.FullSize);

    ZAE.DataOffset = sizeof(ZAExport);

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

    _Base::convertZAExport_I(ZAE);
    pZDBExport.setData(&ZAE,sizeof(ZAExport));
    ZBlockDescriptor_Export wBlkExp;
    for (long wi=0;wi<_Base::size();wi++)
        {
        _Base::Tab[wi]._exportConvert(_Base::Tab[wi],&wBlkExp);
        pZDBExport.appendData(&wBlkExp,sizeof(wBlkExp));
        }
    ZAExport* wZAE=(ZAExport*)pZDBExport.Data;
    wZAE->FullSize = reverseByteOrder_Conditional<size_t> (pZDBExport.Size);

#ifdef __USE_ZTHREAD__
    _Base::unlock();
#endif

    return pZDBExport;

}// _exportPool
/**
 * @brief ZBlockPool::_importPool  DO NOT USE ZArray Standard import/export facilities for Block pools
 * @param pBuffer
 * @return
 */
size_t
ZBlockPool::_importPool(unsigned char *pBuffer)
{

//    return _ZAimport((ZArray<ZBlockDescriptor>*)this,pBuffer,&ZBlockDescriptor::_importConvert);

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
 _Base::lock();
#endif

ZAExport ZAE = _Base::ZAEimport(pBuffer);
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
         ZAE.AllocatedSize,
         ZAE.CurrentSize,
         ZAE.DataSize,
         ZAE.FullSize,
         ZAE.AllocatedElements,
         ZAE.ExtentQuota,
         ZAE.InitialAllocation,
         ZAE.NbElements,
         ZAE.DataOffset);
#endif// __REPORT_POOLS__

_Base::setAllocation(ZAE.AllocatedElements,false);  // no lock
_Base::bzero(0,-1,false);// no lock
_Base::setQuota(ZAE.ExtentQuota);
_Base::setInitialAllocation(ZAE.InitialAllocation,false); // no lock
_Base::clear(false); // no lock

if (ZAE.NbElements>(ssize_t)ZAE.AllocatedElements)
    {
    _Base::setAllocation(ZAE.NbElements,false); // false:no lock
    }
if (ZAE.NbElements>0)
    {
    _Base::newBlankElement(ZAE.NbElements,false); // no use of pClear : can memset ZBlockDescriptor
    ZBlockDescriptor_Export* wEltPtr_In=(ZBlockDescriptor_Export*)(pBuffer+ZAE.DataOffset);

    for (long wi=0;wi<ZAE.NbElements;wi++)
        {
        ZBlockDescriptor::_importConvert(Tab[wi],&wEltPtr_In[wi]);
        }// for

    } //(ZAE.NbElements>0)
#ifdef __USE_ZTHREAD__
_Base::unlock();
#endif
return ZAE.FullSize;

}// _importPool

//======================= ZHeaderControlBlock ================================================

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


//=========================== ZFileControlBlock Export import==========================================

ZDataBuffer&
ZFileControlBlock::_export(ZDataBuffer& pZDBExport)
{
    ZFileControlBlock_Export wFCB;
    wFCB.StartSign=StartSign;// don't care reversing start sign or end sign : same as reversed
    wFCB.BlockID=BlockID;

    wFCB.StartOfData=reverseByteOrder_Conditional<zaddress_type>(StartOfData);
    wFCB.AllocatedBlocks=reverseByteOrder_Conditional<unsigned long>(AllocatedBlocks);
    wFCB.BlockExtentQuota=reverseByteOrder_Conditional<unsigned long>(BlockExtentQuota);
    wFCB.ZBAT_DataOffset=reverseByteOrder_Conditional<size_t>(ZBAT_DataOffset);
    wFCB.ZBAT_ExportSize=reverseByteOrder_Conditional<size_t>(ZBAT_ExportSize);
    wFCB.ZFBT_DataOffset=reverseByteOrder_Conditional<size_t>(ZFBT_DataOffset);
    wFCB.ZFBT_ExportSize=reverseByteOrder_Conditional<size_t>(ZFBT_ExportSize);
    wFCB.ZDBT_DataOffset=reverseByteOrder_Conditional<size_t>(ZDBT_DataOffset);
    wFCB.ZDBT_ExportSize=reverseByteOrder_Conditional<size_t>(ZDBT_ExportSize);
    wFCB.ZReserved_DataOffset=reverseByteOrder_Conditional<size_t>(ZReserved_DataOffset);
    wFCB.ZReserved_ExportSize=reverseByteOrder_Conditional<size_t>(ZReserved_ExportSize);
    wFCB.InitialSize=reverseByteOrder_Conditional<zsize_type>(InitialSize);
    wFCB.AllocatedSize=reverseByteOrder_Conditional<zsize_type>(AllocatedSize);
    wFCB.UsedSize=reverseByteOrder_Conditional<zsize_type>(UsedSize);
    wFCB.MinSize=reverseByteOrder_Conditional<size_t>(MinSize);
    wFCB.MaxSize=reverseByteOrder_Conditional<size_t>(MaxSize);
    wFCB.BlockTargetSize=reverseByteOrder_Conditional<size_t>(BlockTargetSize);
    wFCB.HighwaterMarking=HighwaterMarking;
    wFCB.GrabFreeSpace=GrabFreeSpace;
    wFCB.EndSign=EndSign;// don't care reversing start sign or end sign : same as reversed
    pZDBExport.setData(&wFCB,sizeof(wFCB));

    return pZDBExport;
}// ZFileControlBlock::_export

ZFileControlBlock&
ZFileControlBlock::_import(unsigned char* pZDBImport_Ptr)
{
    ZFileControlBlock_Export* wFCB=(ZFileControlBlock_Export*) pZDBImport_Ptr;

//    StartSign=_reverseByteOrder_T<uint32_t>(wFCB->StartSign);
    StartSign=wFCB->StartSign;

    BlockID=wFCB->BlockID;

    StartOfData=wFCB->StartOfData;// don't care reversing start sign or end sign : same as reversed
    AllocatedBlocks=reverseByteOrder_Conditional<unsigned long>(wFCB->AllocatedBlocks);
    BlockExtentQuota=reverseByteOrder_Conditional<unsigned long>(wFCB->BlockExtentQuota);
    ZBAT_DataOffset=reverseByteOrder_Conditional<size_t>(wFCB->ZBAT_DataOffset);
    ZBAT_ExportSize=reverseByteOrder_Conditional<size_t>(wFCB->ZBAT_ExportSize);
    ZFBT_DataOffset=reverseByteOrder_Conditional<size_t>(wFCB->ZFBT_DataOffset);
    ZFBT_ExportSize=reverseByteOrder_Conditional<size_t>(wFCB->ZFBT_ExportSize);
    ZDBT_DataOffset=reverseByteOrder_Conditional<size_t>(wFCB->ZDBT_DataOffset);
    ZDBT_ExportSize=reverseByteOrder_Conditional<size_t>(wFCB->ZDBT_ExportSize);
    ZReserved_DataOffset=reverseByteOrder_Conditional<size_t>(wFCB->ZReserved_DataOffset);
    ZReserved_ExportSize=reverseByteOrder_Conditional<size_t>(wFCB->ZReserved_ExportSize);
    InitialSize=reverseByteOrder_Conditional<zsize_type>(wFCB->InitialSize);
    AllocatedSize=reverseByteOrder_Conditional<zsize_type>(wFCB->AllocatedSize);
    UsedSize=reverseByteOrder_Conditional<zsize_type>(wFCB->UsedSize);
    MinSize=reverseByteOrder_Conditional<size_t>(wFCB->MinSize);
    MaxSize=reverseByteOrder_Conditional<size_t>(wFCB->MaxSize);
    BlockTargetSize=reverseByteOrder_Conditional<size_t>(wFCB->BlockTargetSize);
    HighwaterMarking=wFCB->HighwaterMarking;
    GrabFreeSpace=wFCB->GrabFreeSpace;
//    EndSign=_reverseByteOrder_T<uint32_t>(wFCB->EndSign);
    EndSign=wFCB->EndSign;// don't care reversing start sign or end sign : same as reversed

    return *this;
}// ZFileControlBlock::_import

//=========================== ZFileControlBlock Export import==========================================
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
ZFileDescriptor::_importFCB(unsigned char* pFileControlBlock_Ptr)
{
    printf ("ZFileDescriptor::_importFCB>>\n");
    ZFileControlBlock* wFileControlBlock=( ZFileControlBlock* )pFileControlBlock_Ptr; // for debug
    setupFCB();
    ZFCB->_import(pFileControlBlock_Ptr);

    ZBAT->_importPool(pFileControlBlock_Ptr+ZFCB->ZBAT_DataOffset);

    ZFBT->_importPool(pFileControlBlock_Ptr+ZFCB->ZFBT_DataOffset);

    ZDBT->_importPool(pFileControlBlock_Ptr+ZFCB->ZDBT_DataOffset);

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

inline void
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

/**
 * @brief ZFileDescriptor::setPath for a ZRandomFile, set up the different pathes environment using a given uriString.
 *  This uriString will name the content file (main data file) @see ZRFPhysical
 * @param[in] pURIPath an uriString containing the path of the file
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZFileDescriptor::setPath (uriString &pURIPath)
{
    if (_isOpen)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Error,
                                    "File <%s> is already open.Cannot change to path <%s>",
                                    URIContent.toString(),
                                    pURIPath.toString());
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
                                            pURIPath.toString());
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



ZRandomFile::ZRandomFile(uriString pURI)
{
    if (setPath(pURI)!=ZS_SUCCESS)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                       ZS_MALFORMED,
                                       Severity_Severe,
                                       " invalid extension for file name <%s> - Cannot use <%s> reserved extension name",
                                       pURI.toString(),
                                       __HEADER_FILEEXTENSION__);
                 }
    return ;
}


//----------------Set parameters------------------------------------

/**
 * @brief ZRandomFile::setPath Sets main content file pathname to pURIPath
 * @param pURIPath pathname to set main content file with
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::setPath (uriString pURIPath)
{
    return (ZDescriptor.setPath(pURIPath));
}
//------------Setting parameters on current ZRandomFile------------------------

ZStatus
ZRandomFile::_setParameters (ZFileDescriptor &pDescriptor,
                             ZFile_type pFileType,
                             const bool pGrabFreeSpace,
                             const bool pHighwaterMarking,
                             const ssize_t pBlockTargetSize)
{
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
    if (!ZDescriptor._isOpen)
            {
            wasOpen=false;
            wSt = _open(pDescriptor,ZRF_Exclusive|ZRF_Write,pFileType);
            if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
        else
            {
            if ((pDescriptor.getMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
                    {
                    wFormerMode = getMode();
                    zclose();
                    wSt = _open(pDescriptor,ZRF_Exclusive|ZRF_Write,pFileType);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast(" while setHighwaterMarking");
                        return wSt;
                        }
                    }
            }// else

    pDescriptor.ZFCB->HighwaterMarking=pHighwaterMarking;
    pDescriptor.ZFCB->GrabFreeSpace=pGrabFreeSpace;
    pDescriptor.ZFCB->BlockTargetSize=pBlockTargetSize;

    wSt=_writeFileDescriptor(pDescriptor,true);
    if (!wasOpen)
                return _close(pDescriptor);

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
    if (!ZDescriptor._isOpen)
            {
            wasOpen=false;
            wSt = zopen(ZRF_Exclusive|ZRF_Write);
            if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
        else
            {
            if ((getMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
                    {
                    wFormerMode = getMode();
                    zclose();
                    wSt = zopen(ZRF_Exclusive|ZRF_Write);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast(" while setHighwaterMarking");
                        return wSt;
                        }
                    }
            }// else

    ZDescriptor.ZFCB->HighwaterMarking=pHighwaterMarking;
    wSt=_writeFileDescriptor(ZDescriptor,true);
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
    if (!ZDescriptor._isOpen)
            {
            wasOpen=false;
            wSt = zopen(ZRF_Exclusive|ZRF_Write);
            if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
        else
            {
            if ((getMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
                    {
                    wFormerMode = getMode();
                    zclose();
                    wSt = zopen(ZRF_Exclusive|ZRF_Write);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast(" while setHighwaterMarking");
                        return wSt;
                        }
                    }
            }// else

        ZDescriptor.ZFCB->GrabFreeSpace=pGrabFreeSpace;
        wSt=_writeFileDescriptor(ZDescriptor,true);
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
    if (!ZDescriptor._isOpen)
            {
            wasOpen=false;
            wSt = zopen(ZRF_Exclusive|ZRF_Write);
            if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
        else
            {
            if ((getMode()&(ZRF_Exclusive|ZRF_Write))!=(ZRF_Exclusive|ZRF_Write))
                    {
                    wFormerMode = getMode();
                    zclose();
                    wSt = zopen(ZRF_Exclusive|ZRF_Write);
                    if (wSt!=ZS_SUCCESS)
                        {
                        ZException.addToLast(" while setHighwaterMarking");
                        return wSt;
                        }
                    }
            }// else
    ZDescriptor.ZFCB->BlockTargetSize=pBlockTargetSize;
    wSt=_writeFileDescriptor(ZDescriptor,true);
    wSt=_writeFileDescriptor(ZDescriptor,true);
    if (!wasOpen)
                return zclose();

    if (wFormerMode!=ZRF_Nothing)
        {
        zclose();
        return zopen(wFormerMode);
        }
    return wSt;
}//setBlockTargetSize


//----------------End Set parameters------------------------------------
//#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
/**
 * @brief ZRandomFile::zremoveFile definitively remove ZRandomFile structure (content and header files).
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zremoveFile(void)
{
    if (isOpen())
            zclose();
    if (remove(ZDescriptor.URIHeader.toCString_Strait())!=0)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_FILEERROR,
                         Severity_Severe,
                         " Cannot remove file <%s>",
                         ZDescriptor.URIHeader.toString());
        return ZS_FILEERROR;
        }
    if (remove(ZDescriptor.URIContent.toCString_Strait())!=0)
        {
        ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_FILEERROR,
                         Severity_Severe,
                         " Cannot remove file <%s>",
                         ZDescriptor.URIContent.toString());
        return ZS_FILEERROR;
        }
    return ZS_SUCCESS;
} //zremoveFile

//---------------ZMasterFile creation--------------------------------------
/**
 * @brief ZRandomFile::setCreateMinimum sets minimum ZRandomFile parameters and leave the rest to their default.
 * @param[in] pInitialSize
 */
void
ZRandomFile::setCreateMinimum(const zsize_type pInitialSize)
{
    ZDescriptor.setupFCB();

    ZDescriptor.ZFCB->AllocatedSize      = pInitialSize;
    ZDescriptor.ZFCB->BlockTargetSize=(long) pInitialSize ;  // base of computation for an average user record size

/*    ZDescriptor.ZFCB->Journaling = false;
    ZDescriptor.ZFCB->Autocommit = false;
    ZDescriptor.ZFCB->History = false;*/

    ZDescriptor.ZFCB->HighwaterMarking = false;
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
        ZDescriptor.setupFCB();
    /*
        ZDescriptor.ZFCB->Journaling = pJournaling;
        ZDescriptor.ZFCB->Autocommit = pAutocommit;
        ZDescriptor.ZFCB->History = pHistory;
      */
        ZDescriptor.ZFCB->HighwaterMarking = pHighwaterMarking;
        ZDescriptor.ZFCB->GrabFreeSpace = pGrabFreeSpace;

        ZDescriptor.ZFCB->AllocatedBlocks =pAllocatedBlocks;  // ZFBT+ZBAT allocated elements
        ZDescriptor.ZFCB->BlockExtentQuota=pBlockExtentQuota; // ZBAT+ZFBT  extension quota (number of elements) and base of computation for file extension size

        ZDescriptor.ZFCB->AllocatedSize = pInitialSize;       // initial allocated space in file
        ZDescriptor.ZFCB->InitialSize = pInitialSize;       // initial allocated space in file

    //    ZDescriptor.ZFCB->ExtentSizeQuota = pExtentSizeQuota;   // extension quota size for file : no more used

    //    ZDescriptor.ZFCB->ZFBTNbEntries = 0;                // ZFBT current elements
    //    ZDescriptor.ZFCB->ZBATNbEntries=0;                  // ZBAT current elements

        ZDescriptor.ZFCB->BlockTargetSize=pBlockTargetSize;    // base of computation for an average user record size

        ZDescriptor.ZBAT->setAllocation(pAllocatedBlocks); // number of elements in table and pool
        ZDescriptor.ZBAT->setQuota(pBlockExtentQuota);     // number of elements table and pool will extend at each time it is needed
        ZDescriptor.ZFBT->setAllocation(pAllocatedBlocks);
        ZDescriptor.ZFBT->setQuota(pBlockExtentQuota);
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
    return (_create(ZDescriptor,pInitialSize,ZFT_ZRandomFile,pBackup, pLeaveOpen));


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
    uriString wFilename;
        wFilename = (const utf8_t*)pFilename;
        wSt=setPath ((const utf8_t*)pFilename);
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
    wFilename=(const utf8_t*)pFilename;
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
    wSt=_create(ZDescriptor,pInitialSize,ZFT_ZRandomFile,pBackup,pLeaveOpen);
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
ZRandomFile::_create(ZFileDescriptor &pDescriptor,const zsize_type pInitialSize,ZFile_type pFileType,bool pBackup,bool pLeaveOpen)
{
ZStatus wSt;
ZBlockDescriptor wFreeBlock;
ZBlockDescriptor_Export wFreeBlockExp;
ZDataBuffer wFileSpace;
int wS;

uriString wFormerContentURI;
uriString wFormerHeaderURI;
long      wFormerNumber=1;
int wRet;

    printf ("ZRandomFile::_create \n");

    if (pInitialSize<=0)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVSIZE,
                              Severity_Severe,
                              "Invalid file size <%ld> given for creating file <%s>\n",
                              pInitialSize,
                              pDescriptor.URIContent.toString());
        return (ZS_INVSIZE);
        }

    if (pBackup)
    {
    if ((pDescriptor.URIContent.exists())||(pDescriptor.URIHeader.exists()))
        {
        wFormerContentURI=pDescriptor.URIContent;
        wFormerHeaderURI = pDescriptor.URIHeader;
        while ((wFormerContentURI.exists())||(wFormerHeaderURI.exists()))
        {
        wFormerNumber ++;
        wFormerContentURI=pDescriptor.URIContent;
        wFormerContentURI.addsprintf("_bck%02ld",wFormerNumber);
        wFormerHeaderURI=pDescriptor.URIHeader;
        wFormerHeaderURI.addsprintf("_bck%02ld",wFormerNumber);
        }
        wRet= rename(pDescriptor.URIContent.toCString_Strait(),wFormerContentURI.toCString_Strait());
        if (wRet!=0)
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_ERROPEN,
                            Severity_Severe,
                            "During file creation : Error renaming existing file <%s> to <%s>\n",
                            pDescriptor.URIContent.toString(),
                             wFormerContentURI.toString());
            }
            else
            if (__ZRFVERBOSE__)
                fprintf (stdout,
                         "Renamed existing file <%s> to <%s> \n",
                         pDescriptor.URIContent.toCString_Strait(),
                          wFormerContentURI.toCString_Strait());
        wRet= rename(pDescriptor.URIHeader.toCString_Strait(),wFormerHeaderURI.toCString_Strait());
        if (wRet!=0)
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_ERROPEN,
                            Severity_Severe,
                            "During file creation : Error renaming existing file <%s> to <%s>\n",
                            pDescriptor.URIHeader.toString(),
                             wFormerHeaderURI.toString());
            ZException.printLastUserMessage();
            }
        else
        if (__ZRFVERBOSE__)
            fprintf (stdout,
                     "Renamed existing file <%s> to <%s> \n",
                     pDescriptor.URIHeader.toString(),
                      wFormerHeaderURI.toString());
        }// if exist
    }// pBackup

    mode_t wPosixMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    pDescriptor.ContentFd = open(pDescriptor.URIContent.toCString_Strait(),O_WRONLY|O_CREAT|O_TRUNC,wPosixMode);  // create or reset the file if exists
    if (pDescriptor.ContentFd < 0)
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_ERROPEN,
                            Severity_Severe,
                            "Error opening file for creation <%s> ",
                            pDescriptor.URIContent.toString());
            ZException.printLastUserMessage();
            }


    pDescriptor.HeaderFd = open(pDescriptor.URIHeader.toCString_Strait(),O_WRONLY|O_CREAT|O_TRUNC,wPosixMode);
    if (pDescriptor.HeaderFd<0)
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_ERROPEN,
                            Severity_Severe,
                            "Error opening header file for creation <%s> ",
                            pDescriptor.URIHeader.toString());
            close(pDescriptor.ContentFd);
            return(ZS_ERROPEN);
            }

    pDescriptor._isOpen=true;
    ZRFPool->addOpenFile(&pDescriptor);

// header
//
//         allocate header recordtable with pRecordAlloc + pExtentQuota
//         allocate freetable with pRecordAlloc + pExtentQuota
//


    if (pDescriptor.ZFCB->AllocatedBlocks > 0)
                        {
                        pDescriptor.ZBAT->setAllocation(pDescriptor.ZFCB->AllocatedBlocks);
                        pDescriptor.ZFBT->setAllocation(pDescriptor.ZFCB->AllocatedBlocks);
                        pDescriptor.ZDBT->setAllocation(pDescriptor.ZFCB->AllocatedBlocks);
                        }
                    else
                        pDescriptor.ZFCB->AllocatedBlocks = pDescriptor.ZBAT->getAllocation();  // get the value from default actual values

    if (pDescriptor.ZFCB->BlockExtentQuota > 0)
                        {
                        pDescriptor.ZBAT->setQuota(pDescriptor.ZFCB->BlockExtentQuota);
                        pDescriptor.ZFBT->setQuota(pDescriptor.ZFCB->BlockExtentQuota);
                        pDescriptor.ZDBT->setQuota(pDescriptor.ZFCB->BlockExtentQuota);
                        }
                    else
                        pDescriptor.ZFCB->BlockExtentQuota = pDescriptor.ZBAT->getQuota();  // get the value from default actual values



    if (pDescriptor.ZFCB->BlockTargetSize==0)
                {
//        pDescriptor.ZFCB->BlockTargetSize = (long)((float)pDescriptor.ZFCB->AllocatedSize / (float)pDescriptor.ZFCB->AllocatedBlocks) ;
        if (pDescriptor.ZFCB->AllocatedBlocks>0)
                pDescriptor.ZFCB->BlockTargetSize = (long)((float)pInitialSize / (float)pDescriptor.ZFCB->AllocatedBlocks) ;
            else
                pDescriptor.ZFCB->BlockTargetSize = pInitialSize ;
                }
    pDescriptor.ZBAT->reset();       // reset number of elements to zero but keep allocation
    pDescriptor.ZFBT->reset();
    pDescriptor.ZDBT->reset();


//------------Extending file (content file) to its initial allocation-------------------

//    pDescriptor.ZFCB->ZFBTNbEntries = pDescriptor.ZFBT->size() ;              //! size of ZFBT is initially 1 : one block containing the whole file space
    zaddress_type wNewOffset ;
    if ((wNewOffset=(zaddress_type)lseek(pDescriptor.ContentFd,0L,SEEK_SET))<0)    //! position to beginning of file and get position in return
                    {
                    ZException.getErrno(errno,
                                    _GET_FUNCTION_NAME_,
                                    ZS_FILEPOSERR,
                                    Severity_Severe,
                                    " Severe error while positionning to beginning of file %s",
                                    pDescriptor.URIContent.toString());

                    wSt=ZS_FILEPOSERR;
                    goto _createEnd;
                    }

    wS=    posix_fallocate(pDescriptor.ContentFd,0L,(off_t)pInitialSize);
    if (wS!=0)
            {
            ZException.getErrno(wS,  // no errno is set by posix_fallocate : returned value is the status : ENOSPC means no space
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Severe,
                             " Error during creation of file <%s> cannot allocate disk space",
                             pDescriptor.URIContent.toString());
            wSt=ZS_WRITEERROR;
            goto _createEnd;
            }

    // create entry in ZFreeBlockPool with initial file size as free space

    wFreeBlock.Address = pDescriptor.ZFCB->StartOfData ;
 //   wFreeBlock.BlockID = ZBID_Data;
    wFreeBlock.State   = ZBS_Free;
    wFreeBlock.BlockSize =pInitialSize;

    wSt=_writeBlockHeader(pDescriptor,wFreeBlock,pDescriptor.ZFCB->StartOfData);

    if (wSt!=ZS_SUCCESS)
                {
                goto _createEnd;
                }

//    create free block content in file

    wFileSpace.allocateBZero(pInitialSize-sizeof(ZBlockHeader_Export));  // ZBlockHeader is written on File - ZBlockDescriptor is the representation in memory

    if (write (pDescriptor.ContentFd,wFileSpace.Data,wFileSpace.Size)<wFileSpace.Size)      // write extent zeroed
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_WRITEERROR,
                                 Severity_Severe,
                                 " Severe error while extending file %s",
                                 pDescriptor.URIContent.toString());
                wSt=ZS_WRITEERROR;
                goto _createEnd;
                }

    fdatasync(pDescriptor.ContentFd);

    // create entry in ZFreeBlockPool with initial file size as free space

     pDescriptor.ZFBT->push(wFreeBlock);

//--------write header----------------------
//
    pDescriptor.Mode = ZRF_Nothing;
    if (pLeaveOpen)
             {
             pDescriptor.Mode = ZRF_Exclusive | ZRF_All ;
             pDescriptor._isOpen = true;
             }

    pDescriptor.ZHeader.FileType = pFileType; // ZFile_type is here

    pDescriptor.ZFCB->InitialSize = pInitialSize;
    pDescriptor.ZFCB->AllocatedSize = pInitialSize;

    pDescriptor.ZHeader.FileType = pFileType ;

    wSt=_writeFullFileHeader(pDescriptor,true);

_createEnd:
    if (pLeaveOpen)
            {
            return wSt;
            }

    pDescriptor._isOpen = false;
    close(pDescriptor.HeaderFd);
    close(pDescriptor.ContentFd);

    ZRFPool->removeFileByObject(&pDescriptor);

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
ZRandomFile::_writeFileHeader(ZFileDescriptor &pDescriptor, bool pForceWrite)
{

     printf ("ZRandomFile::_writeFileHeader\n");

 if (pDescriptor.Mode & ZRF_Exclusive)
        {
        if (!pForceWrite)
                return ZS_SUCCESS;
        }
 pDescriptor.ZHeader.OffsetReserved = sizeof(ZHeaderControlBlock_Export) ;
 pDescriptor.ZHeader.SizeReserved = pDescriptor.ZReserved.Size ;
 pDescriptor.ZHeader.OffsetFCB =    pDescriptor.ZHeader.OffsetReserved + pDescriptor.ZReserved.Size ;

 // NB: ZHeader.Offset is set by clear() method

 if (lseek(pDescriptor.HeaderFd,0L,SEEK_SET) < 0)
             {
             ZException.getErrno(errno,
                              _GET_FUNCTION_NAME_,
                              ZS_FILEPOSERR,
                              Severity_Severe,
                              "Error positionning at Header address <%lld> of header file  <%s>",
                              0L,
                              pDescriptor.URIHeader.toString());
             return(ZS_FILEPOSERR);
             }
 // write ZFileHeader block
 //
 pDescriptor.ZPMS.HFHWrites ++;
 //pDescriptor.ZHeader.BlockID = ZBID_FileHeader ;

 ZDataBuffer wZDBHeaderExport;
 pDescriptor.ZHeader._export(wZDBHeaderExport);
// ssize_t wSWrite =write(pDescriptor.HeaderFd,(char *)&pDescriptor.ZHeader,sizeof(ZHeaderControlBlock)); // write file header infos
 ssize_t wSWrite =write(pDescriptor.HeaderFd,(char *)wZDBHeaderExport.DataChar,wZDBHeaderExport.Size); // write file header infos
 if (wSWrite<0)
         {
         ZException.getErrno(errno,
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEERROR,
                          Severity_Error,
                          "Error while writing File Header block at address 0L to file header %s",
                          pDescriptor.URIHeader.toString());
         return (ZS_WRITEERROR);
         }
 return ZS_SUCCESS;
}//_writeFileHeaderBlock

ZStatus
ZRandomFile::_readFileLock(ZFileDescriptor &pDescriptor, lockPack_struct &pLockPack)
{

    printf ("ZRandomFile::_readFileLock\n");

    size_t wLockInfoOffset = sizeof (ZHeaderControlBlock_Export::StartSign);

    if (lseek(pDescriptor.HeaderFd,wLockInfoOffset,SEEK_SET) < 0)
                 {
                 ZException.getErrno(errno,
                                  _GET_FUNCTION_NAME_,
                                  ZS_FILEPOSERR,
                                  Severity_Severe,
                                  "Error positionning at Header address <%lld> of header file  <%s>",
                                  wLockInfoOffset,
                                  pDescriptor.URIHeader.toString());
                 return  (ZS_FILEPOSERR);
                 }

    ssize_t wSRead =read(pDescriptor.HeaderFd,(char *)&pLockPack,sizeof(lockPack_struct));  // read lock infos with packed structure

    if (wSRead<0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILEHEADER,
                                 Severity_Error,
                                 "Error reading  Header control block address <%lld> file  <%s>",
                                 0L,
                                 pDescriptor.URIHeader.toString());
                return  (ZS_BADFILEHEADER);
                }

    pLockPack.Lock=reverseByteOrder_Conditional<zlockmask_type>(pLockPack.Lock);
    pLockPack.LockOwner=reverseByteOrder_Conditional<pid_t>(pLockPack.LockOwner);
    return  ZS_SUCCESS;

}//_readFileLock

ZStatus
ZRandomFile::_writeFileLock(ZFileDescriptor &pDescriptor, lockPack_struct &pLockPack)
{


    printf ("ZRandomFile::_writeFileLock\n");
    lockPack_struct wLockPack;
    wLockPack.Lock = reverseByteOrder_Conditional<zlockmask_type>(pLockPack.Lock);
    wLockPack.LockOwner = reverseByteOrder_Conditional<pid_t>(pLockPack.LockOwner);

    size_t wLockInfoOffset = sizeof(ZHeaderControlBlock_Export::StartSign); // lock info is just after start marker

    if (lseek(pDescriptor.HeaderFd,(off_t)wLockInfoOffset,SEEK_SET) < 0)
                 {
                 ZException.getErrno(errno,
                                  _GET_FUNCTION_NAME_,
                                  ZS_FILEPOSERR,
                                  Severity_Severe,
                                  "Error positionning at Header address <%lld> of header file  <%s>",
                                  wLockInfoOffset,
                                  pDescriptor.URIHeader.toString());
                 return  (ZS_FILEPOSERR);
                 }
    ssize_t wSWrite =write(pDescriptor.HeaderFd,(char *)&wLockPack,sizeof(lockPack_struct)); // write lock infos to file header s
     if (wSWrite<0)
             {
             ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_WRITEERROR,
                                 Severity_Error,
                                 "Error while writing File Header block at address <%ld> to file header %s",
                                 wLockInfoOffset,
                                 pDescriptor.URIHeader.toString());
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
ZRandomFile::_writeReservedHeader(ZFileDescriptor &pDescriptor,bool pForceWrite)
{


ZStatus wSt;
ssize_t wSWrite;

    if (pDescriptor.Mode & ZRF_Exclusive)
           {
           if (!pForceWrite)
                       {  return  wSt;}

           }

    //             ZReserved + ZFCB  + ZBAT content + ZFBT content + ZReserved
    //
    if (pDescriptor.ZReserved.Size==0)   // if there is a Reserved space
                                {  return  wSt;}


    if ((wSt=_writeFileHeader(pDescriptor,pForceWrite))!=ZS_SUCCESS)
                                    {  return  wSt;}

    pDescriptor.ZPMS.HReservedWrites ++;
    wSWrite =write(pDescriptor.HeaderFd,pDescriptor.ZReserved.DataChar,pDescriptor.ZReserved.Size); // write Reserved infradata
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Error,
                             "Error while writing Reserved infra data to file header %s",
                             pDescriptor.URIHeader.toString());
            return  (ZS_WRITEERROR);
            }

    return  _writeFileDescriptor(pDescriptor,pForceWrite); //! this is absolutely necessary because size of Reserved block may vary. A future optimization could test if Reserved block size has changed and will save to this occasion this access.
}// _writeReservedHeader

/**
 * @brief ZRandomFile::_writeFileDescriptor
 *
 *     writing header. For information :

               ZFileHeader + ZReserved + ZFCB  + ZBAT content + ZFBT content

               This information is written at offset OffsetFCB, in order to skip Reserved block.

 *
 * @param[in] pDescriptor   File descriptor for wich Reserved infra data block will be written
 * @param[out] pForceWrite  if true : will write any time. if false and file is opened in exclusive mode : will not write
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_writeFileDescriptor(ZFileDescriptor &pDescriptor, bool pForceWrite)
{


// Data offsets are computed from Beginning of FCB that may vary is Header size and/or Reserved Data Size Change
//
ZFileDescriptor* wFCBExport=nullptr; // added
ZDataBuffer wZDBZBATExport;
ZDataBuffer wZDBZFBTExport;
ZDataBuffer wZDBZDBTExport;
ssize_t wSWrite;

    printf("ZRandomFile::_writeFileDescriptor\n");

    if (pDescriptor.Mode & ZRF_Exclusive)
           {
           if (!pForceWrite)
                       { return  ZS_SUCCESS;}
           }
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
    fprintf (stdout," exporting ZBAT\n");
    pDescriptor.ZBAT->_exportPool(wZDBZBATExport);
    fprintf (stdout," exporting ZFBT\n");
    pDescriptor.ZFBT->_exportPool(wZDBZFBTExport);
    fprintf (stdout," exporting ZDBT\n");
    pDescriptor.ZDBT->_exportPool(wZDBZDBTExport);

// ==================Update FCB with offsets and sizes then export it===================================================

    pDescriptor.ZFCB->ZBAT_DataOffset =  sizeof(ZFileControlBlock);  // ZBAT data Pool is stored first just after ZFCB

    pDescriptor.ZFCB->AllocatedBlocks = pDescriptor.ZBAT->getAllocation();
    pDescriptor.ZFCB->BlockExtentQuota = pDescriptor.ZBAT->getQuota();
//    pDescriptor.ZFCB->ZBAT_ExportSize = pDescriptor.ZBAT->_getExportAllocatedSize();
    pDescriptor.ZFCB->ZBAT_ExportSize = wZDBZBATExport.Size;


    pDescriptor.ZFCB->ZFBT_DataOffset = (zaddress_type)(pDescriptor.ZFCB->ZBAT_DataOffset
                                                       +pDescriptor.ZFCB->ZBAT_ExportSize);// then ZFBT
//    pDescriptor.ZFCB->ZFBT_ExportSize = pDescriptor.ZFBT->_getExportAllocatedSize();
    pDescriptor.ZFCB->ZFBT_ExportSize = wZDBZFBTExport.Size;

    pDescriptor.ZFCB->ZDBT_DataOffset = (zaddress_type)(pDescriptor.ZFCB->ZFBT_DataOffset
                                                       +pDescriptor.ZFCB->ZFBT_ExportSize); // then ZDBT
//    pDescriptor.ZFCB->ZDBT_ExportSize = pDescriptor.ZDBT->_getExportAllocatedSize();
    pDescriptor.ZFCB->ZDBT_ExportSize = wZDBZDBTExport.Size;

//====================Export updated FCB and write it to file===============================

    ZDataBuffer wZDBFCB;
    pDescriptor.ZFCB->_export(wZDBFCB);

    wZDBFCB.appendData(wZDBZBATExport);
    wZDBFCB.appendData(wZDBZFBTExport);
    wZDBFCB.appendData(wZDBZDBTExport);

    if (lseek(pDescriptor.HeaderFd,pDescriptor.ZHeader.OffsetFCB,SEEK_SET) < 0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning at FileDescriptor address <%lld> of header file  <%s>",
                                 pDescriptor.ZHeader.OffsetFCB,
                                 pDescriptor.URIHeader.toString());
                return  (ZS_FILEPOSERR);
                }

    pDescriptor.ZPMS.HFDWrites ++;
    wSWrite =write(pDescriptor.HeaderFd,wZDBFCB.DataChar,wZDBFCB.Size); // write the whole set :ZFCB + block pools
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Error,
                             "Error while writing FCB + block pools to file header %s",
                             pDescriptor.URIHeader.toString());
            return  (ZS_WRITEERROR);
            }

//===============================================================================================================

/*    ssize_t wSWrite =write(pDescriptor.HeaderFd,(char*)pDescriptor.ZFCB,sizeof(ZFileControlBlock)); // write ZFCB
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Error,
                             "Error while writing FCB to file header %s",
                             pDescriptor.URIHeader.toString());
            return (ZS_WRITEERROR);
            }

    ssize_t wZASize;
    unsigned char*wZAContent;
    wZAContent=pDescriptor.ZBAT->_exportPool(wZAContent,wZASize);
    wSWrite = write(pDescriptor.HeaderFd,wZAContent,wZASize); // export ZBAT
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Error,
                             "Error while writing Block Access Table to file header %s",
                             pDescriptor.URIHeader.toString());
            return (ZS_WRITEERROR);
            }
    free (wZAContent);

    wZAContent=pDescriptor.ZFBT->_exportAllocated(wZAContent,wZASize);
    wSWrite = write(pDescriptor.HeaderFd,wZAContent,wZASize); //! export ZFBT
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Error,
                             "Error while writing Free Blocks Pool to file header %s",
                             pDescriptor.URIHeader.toString());
            return (ZS_WRITEERROR);
            }
    free(wZAContent);

    wZAContent=pDescriptor.ZDBT->_exportAllocated(wZAContent,wZASize);
    wSWrite = write(pDescriptor.HeaderFd,wZAContent,wZASize); //! export ZFBT
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Error,
                             "Error while writing Deleted Blocks Pool to file header %s",
                             pDescriptor.URIHeader.toString());
            return (ZS_WRITEERROR);
            }
    free(wZAContent);
*/
//=============================================================================================

    fdatasync(pDescriptor.HeaderFd);
    return  (ZS_SUCCESS);
}// _writeFileControlBlock

/**
 * @brief ZRandomFile::_writeFullFileHeader  Here only for logical purpose because it de facto an alias of _writeReservedHeader
 * @param[in] pDescriptor   File descriptor for wich Header block including Reserved block infra data will be written
 * @param[out] pForceWrite  if true : will write any time. if false and file is opened in exclusive mode : will not write
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorr
 */
ZStatus
ZRandomFile::_writeFullFileHeader(ZFileDescriptor &pDescriptor, bool pForceWrite)
{


ZStatus wSt;

    printf ("ZRandomFile::_writeFullFileHeader\n");

    if (pDescriptor.Mode & ZRF_Exclusive)
           {
           if (!pForceWrite)
                            {  return  ZS_SUCCESS;}
           }

    if ((wSt=_writeFileHeader(pDescriptor,pForceWrite))!=ZS_SUCCESS)
                            { return  wSt;}
// if ZReserved contains something
    if (pDescriptor.ZReserved.Size > 0)
            {
        pDescriptor.ZPMS.HReservedWrites += 1;
        ssize_t wSWrite =write(pDescriptor.HeaderFd,pDescriptor.ZReserved.DataChar,pDescriptor.ZReserved.Size); // write Reserved infradata
        if (wSWrite<0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_WRITEERROR,
                                 Severity_Error,
                                 "Error while writing Reserved infra data to file header during creation. File %s",
                                 pDescriptor.URIHeader.toString());
                return  (ZS_WRITEERROR);
                }
            }
    return  (_writeFileDescriptor(pDescriptor,pForceWrite));
}//_writeFullFileHeader


/**
 * @brief ZRandomFile::updateFileDescriptor  updates only the File descriptor zone in file header (without updating Reserved data)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::updateFileDescriptor(bool pForceWrite)
{
    return(_writeFileDescriptor(ZDescriptor,pForceWrite));
}
/**
 * @brief ZRandomFile::updateReservedHeader  updates File Header with the Reserved content already set with setReservedContent and updates the File descriptor in Header zone
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::updateReservedBlock(bool pForceWrite)
{
    return(_writeReservedHeader(ZDescriptor,pForceWrite));
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
    return(_writeReservedHeader(ZDescriptor,pForceWrite));
}
/**
 * @brief ZRandomFile::_getFileHeader gets/refrehes the file header block into memory.
 * This routine tests file header block validity
 * @param pDescriptor
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_getFileHeader(ZFileDescriptor &pDescriptor,bool pForceRead)
{


ZStatus wSt=ZS_SUCCESS;
ZDataBuffer wZDB;

    if (pDescriptor.Mode & ZRF_Exclusive)
           {
           if (!pForceRead)
                   {return  ZS_SUCCESS;}//
           }


    off_t wOff = lseek(pDescriptor.HeaderFd,(off_t)0L,SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Fatal,
                                 "Error positionning header at offset <%lld> file <%s>",
                                 0L,
                                 pDescriptor.URIHeader.toString()
                                 );
                return (ZS_FILEPOSERR);
                }
    pDescriptor.ZPMS.HFHReads ++;
    ZHeaderControlBlock_Export wZHeaderExport;
//    ssize_t wSRead =read(pDescriptor.HeaderFd,(char *)&pDescriptor.ZHeader,sizeof(ZHeaderControlBlock));  //! read at first Header control block
    ssize_t wSRead =read(pDescriptor.HeaderFd,(char *)&wZHeaderExport,sizeof(ZHeaderControlBlock_Export));  // read at first Header control block

    if (wSRead<0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILEHEADER,
                                 Severity_Fatal,
                                 "Error reading  Header control block address <%lld> file  <%s>",
                                 0L,
                                 pDescriptor.URIHeader.toString());
                return  (ZS_BADFILEHEADER);
                }

    // Controls about ZHeader integrity

   wSt=pDescriptor.ZHeader._import((unsigned char *)&wZHeaderExport);

   return  wSt;
}//_getFileHeader
ZStatus
ZRandomFile::_getFileHeader_Export(ZFileDescriptor &pDescriptor,ZHeaderControlBlock_Export* pHCB_Export)
{


ZStatus wSt=ZS_SUCCESS;
ZDataBuffer wZDB;



    off_t wOff = lseek(pDescriptor.HeaderFd,(off_t)0L,SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Fatal,
                                 "Error positionning header at offset <%lld> file <%s>",
                                 0L,
                                 pDescriptor.URIHeader.toString()
                                 );
                return (ZS_FILEPOSERR);
                }
    pDescriptor.ZPMS.HFHReads ++;
    ssize_t wSRead =read(pDescriptor.HeaderFd,(char *)pHCB_Export,sizeof(ZHeaderControlBlock_Export));  // read at first Header control block

    if (wSRead<0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILEHEADER,
                                 Severity_Fatal,
                                 "Error reading  Header control block address <%lld> file  <%s>",
                                 0L,
                                 pDescriptor.URIHeader.toString());
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
ZRandomFile::_getFullFileHeader(ZFileDescriptor &pDescriptor, bool pForceRead)
{


ZStatus wSt;

    printf("ZRandomFile::_getFullFileHeader\n");

    if (pDescriptor.Mode & ZRF_Exclusive)
           {
           if (!pForceRead)
                       {  return  ZS_SUCCESS;  }
           }
    if ((wSt=_getFileHeader(pDescriptor,pForceRead))!=ZS_SUCCESS)
                            {return   wSt;}
    if ((wSt=_getReservedHeader (pDescriptor,pForceRead))!=ZS_SUCCESS)
                            {return   wSt;}
    return  _getFileControlBlock(pDescriptor,pForceRead);
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
   return _getReservedHeader(ZDescriptor,pForceRead);
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
    wSt=_getReservedHeader(ZDescriptor,pForceRead);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}
    pReserved = ZDescriptor.ZReserved ;
   return  wSt;
}

ZStatus
ZRandomFile::_getReservedHeader(ZFileDescriptor &pDescriptor, bool pForceRead)
{


    if (pDescriptor.Mode & ZRF_Exclusive)
           {
           if (!pForceRead)
                   { return  ZS_SUCCESS;}
           }
    off_t wOff = lseek(pDescriptor.HeaderFd,(off_t)pDescriptor.ZHeader.OffsetReserved,SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning header at offset <%lld> file <%s>",
                                 pDescriptor.ZHeader.OffsetReserved,
                                 pDescriptor.URIHeader.toString()
                                 );
                return  (ZS_FILEPOSERR);
                }
    pDescriptor.ZPMS.HReservedReads += 1;
    pDescriptor.ZReserved.allocateBZero(pDescriptor.ZHeader.SizeReserved);
    ssize_t wSRead =read(pDescriptor.HeaderFd,pDescriptor.ZReserved.DataChar,pDescriptor.ZHeader.SizeReserved);  //! then read reserved infradata
    if (wSRead<0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILERESERVED,
                                 Severity_Error,
                                 "Error reading  Header reserved infradata file  <%s>",
                                 pDescriptor.URIHeader.toString());
                return  (ZS_BADFILERESERVED);
                }

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
ZRandomFile::_getFileControlBlock (ZFileDescriptor &pDescriptor, bool pForceRead)
{


    if (pDescriptor.Mode & ZRF_Exclusive)
           {
           if (!pForceRead)
                   {  return  ZS_SUCCESS;}
           }

    off_t wOff = lseek(pDescriptor.HeaderFd,pDescriptor.ZHeader.OffsetFCB,SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning header at offset <%lld> file <%s>",
                                 0L,
                                 pDescriptor.URIHeader.toString()
                                 );
                return (ZS_FILEPOSERR);
                }

    pDescriptor.setupFCB();
    // get file control block
    //

    ZDataBuffer wZDB;
    wZDB.allocateBZero(sizeof(ZFileControlBlock_Export));
    pDescriptor.ZPMS.HFDReads ++;
    ssize_t wSRead =read(pDescriptor.HeaderFd,wZDB.DataChar,sizeof(ZFileControlBlock_Export));  //! read at first Header control block
    if (wSRead<0)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILEHEADER,
                                 Severity_Error,
                                 "Error reading  Header control block address <%lld> file  <%s>",
                                 0L,
                                 pDescriptor.URIHeader.toString());
                return  (ZS_BADFILEHEADER);
                }
    pDescriptor.ZFCB->_import(wZDB.Data);

    ZDataBuffer wBuffer;
    wBuffer.allocateBZero( pDescriptor.ZFCB->ZBAT_ExportSize);
    wOff = lseek(pDescriptor.HeaderFd,(off_t)( pDescriptor.ZFCB->ZBAT_DataOffset+pDescriptor.ZHeader.OffsetFCB),SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning file header at offset <%lld> file <%s>",
                                 pDescriptor.ZFCB->ZBAT_DataOffset,
                                 pDescriptor.URIHeader.toString()
                                 );
                return  (ZS_FILEPOSERR);
                }
    wSRead =read(pDescriptor.HeaderFd,wBuffer.DataChar,pDescriptor.ZFCB->ZBAT_ExportSize);
//    if ((wSRead<64)||(wSRead !=pDescriptor.ZFCB->ZBAT_ExportSize))
    if (wSRead !=pDescriptor.ZFCB->ZBAT_ExportSize)// no export header (64)
            {
            ZException.getErrno(errno,
                                _GET_FUNCTION_NAME_,
                                ZS_BADFILEHEADER,
                                Severity_Error,
                                "Error reading Block Access Table offset <%lld> file  <%s>. Pool size <%ld> has not been read in totality (<%ld> read).",
                                pDescriptor.ZFCB->ZBAT_DataOffset,
                                pDescriptor.URIHeader.toString(),
                                pDescriptor.ZFCB->ZBAT_ExportSize,
                                wSRead);
            return  (ZS_BADFILEHEADER);
            }

    pDescriptor.ZBAT->_importPool(wBuffer.Data);

    wBuffer.allocateBZero(pDescriptor.ZFCB->ZFBT_ExportSize);
    wSRead =read(pDescriptor.HeaderFd,wBuffer.DataChar,pDescriptor.ZFCB->ZFBT_ExportSize);
//    if ((wSRead<64)||(wSRead !=pDescriptor.ZFCB->ZFBT_ExportSize))
    if (wSRead !=pDescriptor.ZFCB->ZFBT_ExportSize) // no export header (64)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_BADFILEHEADER,
                             Severity_Error,
                             "Error reading free block pool offset <%lld> file  <%s>. Pool size <%ld> has not been read in totality (<%ld> read).",
                             pDescriptor.ZFCB->ZFBT_DataOffset,
                             pDescriptor.URIHeader.toString(),
                                pDescriptor.ZFCB->ZFBT_ExportSize,
                                wSRead);
            return  (ZS_BADFILEHEADER);
            }
    pDescriptor.ZFBT->_importPool(wBuffer.Data);

    wBuffer.allocateBZero(pDescriptor.ZFCB->ZDBT_ExportSize);
    wSRead =read(pDescriptor.HeaderFd,wBuffer.DataChar,pDescriptor.ZFCB->ZDBT_ExportSize);
//    if ((wSRead<64)||(wSRead !=pDescriptor.ZFCB->ZDBT_ExportSize))
    if (wSRead !=pDescriptor.ZFCB->ZDBT_ExportSize) // no export header (64)
            {
            ZException.getErrno(errno,
                                _GET_FUNCTION_NAME_,
                                ZS_BADFILEHEADER,
                                Severity_Error,
                                "Error reading deleted block pool offset <%lld> file  <%s>. Pool size <%ld> has not been read in totality (<%ld> read).",
                                pDescriptor.ZFCB->ZDBT_DataOffset,
                                pDescriptor.URIHeader.toString(),
                                pDescriptor.ZFCB->ZDBT_ExportSize,
                                wSRead);
            return  (ZS_BADFILEHEADER);
            }
    pDescriptor.ZDBT->_importPool(wBuffer.Data);
/*        ssize_t wSRead =read(pDescriptor.HeaderFd,(char *)pDescriptor.ZFCB,sizeof(ZFileControlBlock));  //! read at first Header control block
        if (wSRead<0)
                    {
                    ZException.getErrno(errno,
                                     _GET_FUNCTION_NAME_,
                                     ZS_BADFILEHEADER,
                                     Severity_Error,
                                     "Error reading  Header control block address <%lld> file  <%s>",
                                     0L,
                                     pDescriptor.URIHeader.toString());
                    return(ZS_BADFILEHEADER);
                    }
    ZDataBuffer wBuffer;
    wBuffer.allocate(pDescriptor.ZFCB->ZBAT_ExportSize);
    wOff = lseek(pDescriptor.HeaderFd,(off_t)( pDescriptor.ZFCB->ZBAT_DataOffset+pDescriptor.ZHeader.OffsetFCB),SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning file header at offset <%lld> file <%s>",
                                 pDescriptor.ZFCB->ZBAT_DataOffset,
                                 pDescriptor.URIHeader.toString()
                                 );
                return(ZS_FILEPOSERR);
                }
    wSRead =read(pDescriptor.HeaderFd,wBuffer.DataChar,pDescriptor.ZFCB->ZBAT_ExportSize);
    if ((wSRead<64)||(wSRead !=pDescriptor.ZFCB->ZBAT_ExportSize))
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILEHEADER,
                                 Severity_Error,
                                 "Error reading Block Access Table offset <%lld> file  <%s>",
                                 pDescriptor.ZFCB->ZBAT_DataOffset,
                                 pDescriptor.URIHeader.toString());
                return(ZS_BADFILEHEADER);
                }

    pDescriptor.ZBAT->_import(wBuffer.Data);

    wBuffer.allocate(pDescriptor.ZFCB->ZFBT_ExportSize);

    wSRead =read(pDescriptor.HeaderFd,wBuffer.DataChar,pDescriptor.ZFCB->ZFBT_ExportSize);
    if ((wSRead<0)||(wSRead<pDescriptor.ZFCB->ZFBT_ExportSize))        //! Load ZFBT
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILEHEADER,
                                 Severity_Error,
                                 "Error reading File Header for getting Free Blocks Pool.  file  <%s>",
                                 pDescriptor.URIHeader.toString());
                return(ZS_BADFILEHEADER);
                }
    pDescriptor.ZFBT->_import(wBuffer.Data);


    wBuffer.allocate(pDescriptor.ZFCB->ZDBT_ExportSize);

    wSRead =read(pDescriptor.HeaderFd,wBuffer.DataChar,pDescriptor.ZFCB->ZDBT_ExportSize);
    if ((wSRead<0)||(wSRead<pDescriptor.ZFCB->ZDBT_ExportSize))        //! Load ZDBT
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_BADFILEHEADER,
                                 Severity_Error,
                                 "Error reading File Header for getting ZDBT. file  <%s>",
                                 pDescriptor.URIHeader.toString());
                return(ZS_BADFILEHEADER);
                }
    pDescriptor.ZDBT->_import(wBuffer.Data);
*/

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
ZRandomFile::_getBlockHeader(ZFileDescriptor &pDescriptor,  zaddress_type pAddress, ZBlockHeader &pBlockHeader)
{


ssize_t wSRead;
ZBlockHeader_Export wBlockHeadExp;
// get position to block : block header is the first block element

    off_t wOff = lseek(pDescriptor.ContentFd,(off_t)( pAddress),SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning file  at logical address <%lld> file <%s>",
                                 pAddress,
                                 pDescriptor.URIContent.toString()
                                 );
                return  (ZS_FILEPOSERR);
                }

    pDescriptor.setPhysicalPosition((zaddress_type)wOff);

    pDescriptor.ZPMS.CBHReads ++;

    if ((wSRead=read(pDescriptor.ContentFd,&wBlockHeadExp,sizeof(ZBlockHeader_Export)))<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_READERROR,
                             Severity_Error,
                             "Error reading block header :  address <%lld> for file <%s>\n",
                             pAddress,
                             pDescriptor.URIContent.toString());
            return  (ZS_READERROR);
            }
    pDescriptor.ZPMS.CBHReadBytesSize += wSRead ;

    ZBlockHeader::_importConvert(pBlockHeader,&wBlockHeadExp);

    pDescriptor.incrementPosition(wSRead);

    if (wSRead!=sizeof(ZBlockHeader_Export))
            {
            ZException.setMessage(
                             _GET_FUNCTION_NAME_,
                             ZS_READPARTIAL,
                             Severity_Error,
                             "Error reading block header :  address <%lld> for file <%s>\n"
                             "Block header has not been entirely read and has been truncated.",
                             pAddress,
                             pDescriptor.URIContent.toString());
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


    return  _open(ZDescriptor,pMode,ZFT_ZRandomFile);

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
/**
 * @brief ZRandomFile::zopen open file which name is given by pFilename as a C string to the mode pMode
 * @param[in] pFilename a C string containing ZRandomFile main file name. Header file name is deduced.
 * @param[in] pMode open mode mask @see zmode
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::zopen(const char *pFilename, const zmode_type pMode)
{
uriString wFilename;
    wFilename = (const utf8_t*)pFilename;
    return(zopen(wFilename,pMode));
}//zopen

ZStatus
ZRandomFile::zclose(void)
{
    return (_close(ZDescriptor));

}//zclose

/** @cond Development */
/**
 * @brief _cleanDeletedBlocks Deletes blocks in free blocks pool that are included in the area covered by block pBD.
 *  @note When a free block is allocated to ZBAT all deleted blocks within it are no more recoverable.
 *  This means that their references are destroyed from ZDBT table.
 * @param[In,Out]   pDescriptor File descriptor. File descriptor is modified (ZDBT) by this operation
 * @param[in]       pBD   block containing the deleted blocks to remove
 */
void _cleanDeletedBlocks(ZFileDescriptor &pDescriptor,ZBlockDescriptor &pBD)
{
    zaddress_type wAddMin = pBD.Address;
    zaddress_type wAddMax = pBD.Address+pBD.BlockSize;

    long wi = 0;
    while (wi<pDescriptor.ZDBT->size())
            {
                if ((pDescriptor.ZDBT->Tab[wi].Address >= wAddMin)&&(pDescriptor.ZDBT->Tab[wi].Address < wAddMax))
                            {
                            if (__ZRFVERBOSE__)
                               _DBGPRINT(
                                     " Removing deleted block index %ld from recoverable block pool address %lld is included in field of allocated block address %lld size %lld\n",
                                     wi,
                                     pDescriptor.ZDBT->Tab[wi].Address,
                                     pBD.Address,
                                     pBD.BlockSize
                                     );
                              pDescriptor.ZDBT->erase(wi);
                              continue;
                            }
                wi++;
            }
    return;
} // _cleanDeletedBlocks


/**
 * @brief ZRandomFile::_allocateFreeBlock
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
 * @param[in] pDescriptor   ZFileDescriptor of the file
 * @param[in] pZFBTIndex    Free block rank to be moved to ZBAT pool
 * @param[in] pSize        Size of the block to be allocated
 * @param[in] pZBATIndex    ZBAT rank to insert the allocated block. if -1, the new block is pushed on ZBAT
 * @return  rank within ZBAT (ZBlockAccessTable) of the allocated block : this is the relative position within RandomFile.
 */

long
ZRandomFile::_allocateFreeBlock (ZFileDescriptor &pDescriptor,
                                 zrank_type pZFBTRank,
                                 zsize_type pSize,
                                 long pZBATRank)

{


zrank_type wZBATRank=pZBATRank;

    pDescriptor.ZPMS.FreeMatches ++;

// debug
    if (__ZRFVERBOSE__)
        _DBGPRINT(" allocateFreeBlock ; allocate size is %lld \n",pSize); // debug

    if ((pZFBTRank<0 )||(pZFBTRank > pDescriptor.ZFBT->lastIdx()))
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                           ZS_INVOP,
                                           Severity_Severe,
                                           "Invalid index for Free Block Table (ZFBT) %ld ",pZFBTRank);
                    return  -1;
                    }
    if (pSize > pDescriptor.ZFBT->Tab[pZFBTRank].BlockSize)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                           ZS_INVOP,
                                           Severity_Severe,
                                           "Invalid Free Block to be allocated : size %lld while requested size is %lld",
                                           pDescriptor.ZFBT->Tab[pZFBTRank].BlockSize,
                                           pSize);
                    return  -1;
                    }
    ZBlockDescriptor wBD;

    wBD.clear();
    wBD             = pDescriptor.ZFBT->Tab[pZFBTRank];

    wBD.BlockSize   = pSize;
    wBD.State       = ZBS_Allocated;    // block is allocated BUT NOT USED
//    wBD.Lock        = ZLock_All ;       // Block locked for all operations
//    wBD.LockReason  = ZReason_Create;   // Block locked for creation
    wBD.Pid         = pDescriptor.Pid ;


// create / update ZBAT entry with free block data

    if (wZBATRank <0)        // wZBATRank = -1 : it is requested to add one entry - else it is requested to reuse given ZBAT entry
          {
          pDescriptor.ZBAT->push(wBD);
          wZBATRank = pDescriptor.ZBAT->lastIdx();
          }
      else
          pDescriptor.ZBAT->insert(wBD,pZBATRank); // ZBlockHeader is not modified on file and ZBAT is

    zsize_type wRemainingSize = pDescriptor.ZFBT->Tab[pZFBTRank].BlockSize - pSize;


    // debug
    //
    if(__ZRFVERBOSE__)
        _DBGPRINT(
                 " Allocating free block index %ld size %lld at address %lld for requested size %lld remaining size %lld \n",
                 pZFBTRank,
                 pDescriptor.ZFBT->Tab[pZFBTRank].BlockSize,
                 pDescriptor.ZFBT->Tab[pZFBTRank].Address,
                 pSize,
                 wRemainingSize
                 );




    if (wRemainingSize==0)                      //! sizes are equal : reuse this Free Table Entry as it is and transfer it to ZBAT
            {
              pDescriptor.ZFBT->erase(pZFBTRank);      //! just suppress ZFBT entry : it is transferred to ZBAT

              _cleanDeletedBlocks(pDescriptor,wBD);     //! remove deleted block included in free block from Recovery Pool

              return  (wZBATRank);              //! return ZBAT entry index created or reused
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

    wRemainingFreeBlock = pDescriptor.ZFBT->Tab[pZFBTRank];
    wRemainingFreeBlock.Address += pSize;               // shift offset to new block header
    wRemainingFreeBlock.BlockSize -= pSize;             // subtract used size
    wRemainingFreeBlock.State = ZBS_Free ;


    pDescriptor.ZFBT->erase(pZFBTRank);    //!

     if (wRemainingSize > sizeof(ZBlockHeader_Export)) // if and only if remaining space can contain more than a block header (otherwise leave as hole)
            {
            // must create a new free block entry in memory(ZBlockHeader) and on file (ZBlockHeader_Export)

            pDescriptor.ZFBT->push(wRemainingFreeBlock) ;

//      need to write block header here at new address : export is made within _writeBlockHeader

             if ( _writeBlockHeader(pDescriptor,(ZBlockHeader&)wRemainingFreeBlock,wRemainingFreeBlock.Address)!=ZS_SUCCESS)
                                                         {  return  -1;}

            if(__ZRFVERBOSE__)
                 _DBGPRINT(
                          " Creating new free block size %lld at address %lld  index in Pool %ld \n",
                          wRemainingFreeBlock.BlockSize,
                          wRemainingFreeBlock.Address,
                          pDescriptor.ZFBT->lastIdx()
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

    _cleanDeletedBlocks(pDescriptor,wBD);     //! remove deleted block included in free block from Recovery Pool

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
ZRandomFile::_seek(ZFileDescriptor &pDescriptor,zaddress_type pAddress)
{


    off_t wOff = lseek(pDescriptor.ContentFd,(off_t)( pAddress+pDescriptor.ZFCB->StartOfData),SEEK_SET);
    if (wOff<0)
                {
                ZException.getErrno(errno,
                                _GET_FUNCTION_NAME_,
                                ZS_FILEPOSERR,
                                Severity_Severe,
                                "Error while positionning file <%s> at address <%lld>",
                                pDescriptor.URIContent.toString(),
                                pAddress);
                return  (ZS_FILEPOSERR);
                }
    pDescriptor.setPhysicalPosition((zaddress_type)wOff);
    return  ZS_SUCCESS;
}//_seek

/**
  @brief ZRandomFile::_read
    reads at current physical position a file given by it ZFileDescriptor pDescriptor.
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
ZRandomFile::_read(ZFileDescriptor &pDescriptor,
                   void* pBuffer,
                   const size_t pSize,
                   ssize_t &pSizeRead,
                   ZPMSCounter_type pZPMSType)
{


//    pDescriptor.ZPMS.UserReads += 1;

    pSizeRead= read(pDescriptor.ContentFd,pBuffer,pSize);

//    pDescriptor.ZPMS.UserReadSize += pSizeRead;
    if (pSizeRead<0)
            {
            pDescriptor.ZPMS.PMSCounterRead(pZPMSType,0);
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_READERROR,
                             Severity_Severe,
                             "Error while reading file <%s>",
                             pDescriptor.URIContent.toString());
            return  (ZS_READERROR);
            }
    pDescriptor.ZPMS.PMSCounterRead(pZPMSType,pSizeRead);
    pDescriptor.setPhysicalPosition((zaddress_type)lseek(pDescriptor.ContentFd,0L,SEEK_CUR));//! get current address
//    pDescriptor.PhysicalPosition = lseek(pDescriptor.ContentFd,0L,SEEK_CUR);  //! get current address
//    pDescriptor.LogicalPosition = pDescriptor.PhysicalPosition - pDescriptor.ZFCB->StartOfData ;

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
ZRandomFile::_read(ZFileDescriptor &pDescriptor,
                   ZDataBuffer& pBuffer,
                   const ssize_t pSizeToRead,
                   ZPMSCounter_type pZPMSType)
{

ssize_t wSRead=0;

//    pDescriptor.ZPMS.UserReads += 1;

    pBuffer.allocate(pSizeToRead);                                    //! allocate the maximum size to read from file
    wSRead= read(pDescriptor.ContentFd,pBuffer.Data,pBuffer.Size);  //! read


    if (wSRead<0)
            {
            pDescriptor.ZPMS.PMSCounterRead(pZPMSType,0);
            ZException.getErrno(errno,
                                _GET_FUNCTION_NAME_,
                                ZS_READERROR,
                                Severity_Severe,
                                "Error while reading file <%s>",
                                pDescriptor.URIContent.toString());
            return(ZS_READERROR);
            }

//    pDescriptor.ZPMS.UserReadSize += wSRead;
    pDescriptor.ZPMS.PMSCounterRead(pZPMSType,wSRead);
    if (wSRead==0)
            {
            ZException.getErrno(errno,
                         _GET_FUNCTION_NAME_,
                         ZS_EOF,
                         Severity_Severe,
                         "Read size is zero length. Either EOF has been encountered or User content record is empty.On file <%s>",
                         pDescriptor.URIContent.toString());
//             pBuffer.clear();  // pBuffer remains unchanged
            return ZS_EOF;
            }
    if (wSRead<pSizeToRead)
            {
            pBuffer.allocate(wSRead); // reallocate appropriate size
            return ZS_READPARTIAL; //! not an error : expected size is not returned. EOF may have been found
            }
    pDescriptor.incrementPosition( wSRead );
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
ZRandomFile::_readAt(ZFileDescriptor &pDescriptor,
                     void *pBuffer,
                     size_t pSize,
                     ssize_t &pSizeRead,
                     zaddress_type pAddress,
                     ZPMSCounter_type pZPMSType)
{


    ZStatus wSt = _seek(pDescriptor,pAddress);
    if (wSt!=ZS_SUCCESS)
                {   return  wSt;   }
    return  _read(pDescriptor,pBuffer,pSize,pSizeRead,pZPMSType);
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
ZRandomFile::_readAt(ZFileDescriptor &pDescriptor,
                     ZDataBuffer& pBuffer,
                     ssize_t &pSizeToRead,
                     zaddress_type pAddress,
                     ZPMSCounter_type pZPMSType)
{
    ZStatus wSt = _seek(pDescriptor,pAddress);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return _read(pDescriptor,pBuffer,pSizeToRead,pZPMSType);
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
ZRandomFile::_readBlockAt(ZFileDescriptor &pDescriptor,
                     ZBlock &pBlock,
                     const zaddress_type pAddress)
{


ZStatus wSt;
ssize_t wSizeRead;
ssize_t wUserRecordSize;

ZBlockHeader_Export wBlockHeadExp;

//    pDescriptor.ZPMS.CBHReads ++;

    wSt= _readAt(pDescriptor,&wBlockHeadExp,sizeof(ZBlockHeader_Export),wSizeRead,pAddress,ZPMS_BlockHeader);
    if (wSt!=ZS_SUCCESS)
                {   return  wSt;   }

    wSt=ZBlockHeader::_importConvert(pBlock,&wBlockHeadExp);

// Test wether given address correspond to effective beginning of a block with correct identification (Data block)

    if (wSt!=ZS_SUCCESS)
                    {   return  wSt;   }


//    pDescriptor.ZPMS.CBHReadBytesSize +=wSizeRead ;

    wUserRecordSize = pBlock.BlockSize - sizeof(ZBlockHeader_Export);
    pBlock.allocate(wUserRecordSize);
    return  _read(pDescriptor,pBlock.Content,wUserRecordSize,ZPMS_User); // PMS counter are NOT updated within _read for user content values

}//_readBlockAt

/**
 * @brief ZRandomFile::searchBlockRankByAddress searches an active block (in ZBlockAccessTable) by its address and returns its rank in pool
 * @param[in] pDescriptor File descriptor
 * @param[in] pAddress starting block Address of the record to find the rank
 * @return  rank of the record whose block starts at address pAddress
 *      or -1 if given address is not the start of an active block in ZBAT pool
 */
zrank_type
ZRandomFile::searchBlockRankByAddress(ZFileDescriptor &pDescriptor,
                                      zaddress_type pAddress)
{
    for (long wi = 0; wi< pDescriptor.ZBAT->size();wi++)
                {
                if (pDescriptor.ZBAT->Tab[wi].Address==pAddress)
                                {
                                return wi;
                                }
                }
    return -1;  // not found
} //searchBlockByAddress



/**
 * @brief ZRandomFile::_freeBlock_Prepare      Prepares to free an entry of ZBlockAccessTable pool : prepares to deletes it from ZBAT.
 *  Entry in ZBAT is marked ZBS_BeingDeleted, and cannot be access more by others.
 *
 *  If HighwaterMarking option is set : the file block region will be set to zero during commit phase.
 *
 * @param[in] pDescriptor File descriptor
 * @param[in] pIndex ZBAT index to be freed (record rank)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

void
ZRandomFile::_freeBlock_Prepare(ZFileDescriptor &pDescriptor,
                                zrank_type pRank )
{
    pDescriptor.ZBAT->Tab[pRank].State = ZBS_BeingDeleted;
    pDescriptor.ZBAT->Tab[pRank].Pid   = pDescriptor.Pid;
    return;
}

/**
 * @brief ZRandomFile::_freeBlock_Rollback Invalidate freeBlock operation and sets the ZBAT block again to ZBS_Used, so that it can be accessed normally.
 * @param[in] pDescriptor File descriptor
 * @param[in] pIndex ZBAT index to be freed (record rank)
 */
void
ZRandomFile::_freeBlock_Rollback (ZFileDescriptor &pDescriptor,
                                  zrank_type pRank )
{
    pDescriptor.ZFBT->Tab[pRank].State = ZBS_Used ;
    pDescriptor.ZBAT->Tab[pRank].Pid   = 0L;
    return;
}

/**
 * @brief ZRandomFile::_freeBlock_Commit Frees definitively an entry of ZBlockAccessTable pool
 *
 * Frees an entry of ZBlockAccessTable pool : Deletes it from ZBAT.
 * Create a corresponding entry wihin ZFreeBlockTable pool.
 *
 * If HighwaterMarking option is set : set the file block region to zero.
 *
 * @param[in] pDescriptor File descriptor
 * @param[in] pIndex ZBAT index to be freed (record rank)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_freeBlock_Commit(ZFileDescriptor &pDescriptor,
                               zrank_type pRank)
{
ZStatus wSt;
ZBlockDescriptor wBS;

    wBS   = pDescriptor.ZBAT->Tab[pRank];


    if (pDescriptor.ZFCB->GrabFreeSpace)
            {
//            if ((wSt=_grabFreeSpacePhysical(pDescriptor,pIndex,wBS))!=ZS_SUCCESS)
//                                                                return wSt;
            if ((wSt=_grabFreeSpaceLogical(pDescriptor,pRank,wBS))!=ZS_SUCCESS)
                                                                return wSt;
            }

    wBS.State       = ZBS_Free;
    wSt=_writeBlockHeader(pDescriptor,(ZBlockHeader&)wBS,wBS.Address);
    if (wSt!=ZS_SUCCESS)
                return wSt;


    if (pDescriptor.ZFCB->HighwaterMarking)      // if highwatermarking then set user content file region to binary zero
            {
            wSt=_highwaterMark_Block(pDescriptor,wBS.BlockSize-sizeof(ZBlockHeader_Export));
            if (wSt!=ZS_SUCCESS)
                        {
                        return wSt;
                        }

            } // if Highwater

//    wBS.State = ZBS_Free;
    pDescriptor.ZFBT->push(wBS);      // create entry into ZFBT for freed Block
    pDescriptor.ZBAT->erase(pRank);  // remove entry from ZBAT

    pDescriptor.ZFCB->UsedSize -= wBS.BlockSize ;

    return(_writeFileDescriptor(pDescriptor,true));
}//_freeBlock_commit

/**
 * @brief ZRandomFile::_freeBlock former freeblock routine
 * @param[in] pDescriptor File descriptor
 * @param[in] pRank ZBAT rank to be freed (record rank)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_freeBlock(ZFileDescriptor &pDescriptor,
                        zrank_type pRank)
{

ZStatus wSt;
ZBlockDescriptor wBS;
ZBlockDescriptor_Export wBS_Exp;

    wBS   = pDescriptor.ZBAT->Tab[pRank];
    wBS.State       = ZBS_BeingDeleted;
//    wBS.Lock        = ZLock_All;
//    wBS.LockReason  = ZReason_Delete ;
    wBS.Pid         = pDescriptor.Pid;      // warning get the current pid

    if (pDescriptor.ZFCB->GrabFreeSpace)
            {
            if ((wSt=_grabFreeSpaceLogical(pDescriptor,pRank,wBS))!=ZS_SUCCESS)
                                                                        { return  wSt;}

            }



//    pDescriptor.ZFCB->ZBATNbEntries --;

    ZBlock wBlock;
    wBlock.clear();

    wBlock.allocate(wBS.BlockSize-sizeof(ZBlockHeader_Export));
    wBlock.resetData();
    wBlock.State=ZBS_Free;
    wBlock.BlockSize = wBS.BlockSize;
//    wBlock.BlockID =ZBID_Nothing;

    wSt=_writeBlockHeader(pDescriptor,(ZBlockHeader&)wBlock,wBS.Address);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}


    if (pDescriptor.ZFCB->HighwaterMarking)      //! if highwatermarking then set the file region to zero
            {
            pDescriptor.ZPMS.HighWaterWrites ++;

            ssize_t wSWrite=write (pDescriptor.ContentFd,wBlock.DataChar(), wBlock.DataSize());
            if (wSWrite<0)
                        {
                        ZException.getErrno(errno,
                                         _GET_FUNCTION_NAME_,
                                         ZS_WRITEERROR,
                                         Severity_Severe,
                                         "Severe Error while high water marking freed block for file %s at address %lld",
                                         pDescriptor.URIContent.toString(),
                                         pDescriptor.getPhysicalPosition());
                        return  (ZS_WRITEERROR);
                        }

            pDescriptor.ZPMS.HighWaterBytesSize += wSWrite;
            if (wSWrite!=wBlock.DataSize())
                        {
                        ZException.getErrno(errno,
                                         _GET_FUNCTION_NAME_,
                                         ZS_WRITEPARTIAL,
                                         Severity_Error,
                                         "Error: data partially written to file while high water marking freed block for file %s at address %lld requested %ld written %ld",
                                         pDescriptor.URIContent.toString(),
                                         pDescriptor.getPhysicalPosition(),
                                         wBlock.DataSize(),
                                         wSWrite);
                        return  (ZS_WRITEPARTIAL);
                        }
 //           pDescriptor.incrementPosition( wSWrite );

            fdatasync(pDescriptor.ContentFd); //! better than flush : writes only data and not metadata to file
            } // if Highwater

    wBS.State = ZBS_Free;
    pDescriptor.ZFBT->push(wBS);      // create entry into ZFBT for freed Block
    pDescriptor.ZBAT->erase(pRank);  // remove entry from ZBAT

    pDescriptor.ZFCB->UsedSize -= wBS.BlockSize ;

// current index and positions are invalid after removing of the block from ZBAT

    pDescriptor.CurrentRank = -1;
    pDescriptor.LogicalPosition = -1;
    pDescriptor.PhysicalPosition = -1;

    return (_writeFileDescriptor(pDescriptor,true));
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
ZRandomFile::_grabFreeSpacePhysical(ZFileDescriptor &pDescriptor,
                                    zrank_type pRank,
                                    ZBlockDescriptor &pBS)
{

ZStatus wSt;
zaddress_type wStartFreeBlockAddress ;

zsize_type wSizeFreeSpace;

    pBS.Address     = pDescriptor.ZBAT->Tab[pRank].Address;
    pBS.BlockSize   = pDescriptor.ZBAT->Tab[pRank].BlockSize;

    wStartFreeBlockAddress = pDescriptor.ZBAT->Tab[pRank].Address;
    wSizeFreeSpace =  pDescriptor.ZBAT->Tab[pRank].BlockSize;

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



    wStartFreeBlockAddress = pDescriptor.ZBAT->Tab[pRank].Address;

    wSt=_searchPreviousPhysicalBlock(pDescriptor,pDescriptor.ZBAT->Tab[pRank].Address,wPreviousAddress,wPreviousBlockHeader);

    if (wSt==ZS_OUTBOUNDLOW)
            wStartFreeBlockAddress = wPreviousAddress ; //! just get from Start of Data - returned by _searchPreviousBlock
        else //! wSt==ZS_FOUND
    {
     if (wPreviousBlockHeader.State==ZBS_Free) // Yes it is : Keep Address to group with current block
                                                // Address is : address of reading + wOffset1
                                                // Additional Length has to be computed
                                                // Think to erase current block header
        {
        wStartFreeBlockAddress=wPreviousAddress;            // grab this space !
        if(__ZRFVERBOSE__)
            _DBGPRINT("Grabbing previous free block : address %lld size %lld\n",
                      wPreviousAddress,
                      wPreviousBlockHeader.BlockSize);

// get free block in ZFBT free block pool and suppress it

        long wi;
        for (wi=0;wi<pDescriptor.ZFBT->size();wi++)
                        if (pDescriptor.ZFBT->Tab[wi].Address == wPreviousAddress)
                                        {
                                        pDescriptor.ZFBT->erase(wi);  //! suppress this block from free block pool
                                        break;
                                        }


        }
        else
// previous physical block in file is Not ZBS_Free,
// so we must test wether there is a hole between previous block and freed block
//  previous block start address + blockSize + 1 < freed block start address : there is a hole
        {
         zaddress_type wHoleAddress = (zaddress_type) wPreviousAddress+ wPreviousBlockHeader.BlockSize + 1;
        if (wHoleAddress<pDescriptor.ZBAT->Tab[pRank].Address)      // yes there is a hole there
                                                                    // hole starts at wHoleAddress : grab it !
                        {
                        wStartFreeBlockAddress = wHoleAddress;  //! grab hole address (between previous used block and current address)
                        if(__ZRFVERBOSE__)
                            _DBGPRINT("Grabbing hole : address %lld\n",
                                      wHoleAddress);
                        }
        //! else no Hole no Previous Free block : use current block address - as initialized
        }

        }  //! wSt==ZS_FOUND



// ---------let's investigate for next block to get the size ---------------
//
//     find start sign after freed block :
//
    ZBlockHeader wNextBlockHeader;
//    ZBlockHeader_Export wNextBlockHeader_Export;  // not used, not necessary
    zaddress_type wNextAddress;
    zaddress_type wEndingAddress;

    zaddress_type wStartAddress = pDescriptor.ZBAT->Tab[pRank].Address + pDescriptor.ZBAT->Tab[pRank].BlockSize;  //! the byte after block to search next


    wSt= _searchNextPhysicalBlock(pDescriptor,wStartAddress,wNextAddress,wNextBlockHeader);
    if (wSt<0)
            return wSt; //! in case of error return status
    wEndingAddress = wNextAddress;
    if (wSt==ZS_FOUND)
            if (wNextBlockHeader.State==ZBS_Free)  //! if next block is free : grab its space and remove this block from freeblock pool
                        {
                        wEndingAddress = wNextAddress + wNextBlockHeader.BlockSize ;
                        long wi;
                        for (wi=0;wi<pDescriptor.ZFBT->size();wi++)
                                        if (pDescriptor.ZFBT->Tab[wi].Address == wNextAddress)
                                                        {
                                                        pDescriptor.ZFBT->erase(wi);  //! suppress this block from free block pool
                                                        break;
                                                        }
                        }


    wSizeFreeSpace = (zaddress_type)  (wEndingAddress  - wStartFreeBlockAddress);

    pBS.Address     = wStartFreeBlockAddress;
    pBS.BlockSize   = wSizeFreeSpace;

//------End Gather free blocks and grab holes-----------------

    if (pBS.Address!=pDescriptor.ZBAT->Tab[pRank].Address)
                        {
                        pDescriptor.ZBAT->Tab[pRank].State = ZBS_Deleted;
                        pDescriptor.ZDBT->push(pDescriptor.ZBAT->Tab[pRank]);
                        return (_markBlockAsDeleted (pDescriptor,pRank));
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
ZRandomFile::_grabFreeSpaceLogical(ZFileDescriptor &pDescriptor,
                                   zrank_type pRank,            //! ZBAT index to be freed
                                   ZBlockDescriptor &pBS)  //! free block to be returned
{

ZStatus wSt;
zaddress_type wStartFreeBlockAddress ;
long wgrab;

zsize_type wSizeFreeSpace;

    pBS     = pDescriptor.ZBAT->Tab[pRank];

    wStartFreeBlockAddress = pDescriptor.ZBAT->Tab[pRank].Address;
    wSizeFreeSpace =  pDescriptor.ZBAT->Tab[pRank].BlockSize;



// finds a previous block in free pool so that address + blocksize = pDescriptor.ZBAT->Tab[pIndex].Address;
    if (wStartFreeBlockAddress>pDescriptor.ZFCB->StartOfData)
    {
    for (wgrab=0; wgrab<pDescriptor.ZFBT->size();wgrab++)
                    {
                if (wStartFreeBlockAddress == (pDescriptor.ZFBT->Tab[wgrab].Address+pDescriptor.ZFBT->Tab[wgrab].BlockSize))
                            {
                            if (pDescriptor.ZFBT->Tab[wgrab].State != ZBS_Free) //! if block is locked in the pool : leave it
                                                {
                                if(__ZRFVERBOSE__)
                                    _DBGPRINT("Found block to grab but it is locked : address %lld size %lld - sate %s locked mask %uc reason %s \n",
                                              pDescriptor.ZFBT->Tab[wgrab].Address,
                                              pDescriptor.ZFBT->Tab[wgrab].BlockSize,
                                              decode_ZBS( pDescriptor.ZFBT->Tab[wgrab].State)) ;

                                                 break ;
                                                }
                            pBS = pDescriptor.ZFBT->Tab[wgrab];
                            wSizeFreeSpace += pDescriptor.ZFBT->Tab[wgrab].BlockSize ;
                            pBS.BlockSize = wSizeFreeSpace ;

                            if(__ZRFVERBOSE__)
                                _DBGPRINT("Grabbing previous free block : address %lld size %lld\n",
                                          pDescriptor.ZFBT->Tab[wgrab].Address,
                                          pDescriptor.ZFBT->Tab[wgrab].BlockSize) ;

                            // mark ZTAB block header as deleted on file (new active header is ZFBT one)

                            pDescriptor.ZBAT->Tab[pRank].State = ZBS_Deleted ;
                            if ((wSt=_markBlockAsDeleted(pDescriptor,pRank))!=ZS_SUCCESS)
                                            {
                                                                                    return wSt;
                                            }
                            pDescriptor.ZDBT->push(pDescriptor.ZBAT->Tab[pRank]); //! add entry into deleted blocks : it will no more be the blockheader

                            pDescriptor.ZFBT->erase(wgrab);  //! suppress this block from free block pool
                            break;
                            }
                    }// for

    }// if

// Nota Bene : we have to compute the whole size with address + block size in order to take into account possible holes between blocks
//
 zaddress_type wEndAddress = pDescriptor.ZBAT->Tab[pRank].Address + pDescriptor.ZBAT->Tab[pRank].BlockSize; //! theorical beginning address of following block
    if (wEndAddress != pDescriptor.ZFCB->AllocatedSize)
    {

     for (wgrab=0; wgrab<pDescriptor.ZFBT->size();wgrab++)
                     if (wEndAddress == (pDescriptor.ZFBT->Tab[wgrab].Address))
                     {
                     if (pDescriptor.ZFBT->Tab[wgrab].State != ZBS_Free) //! if block is locked in the pool : leave it
                            {
                             if(__ZRFVERBOSE__)
                                _DBGPRINT("Found block to grab but it is locked : address %lld size %lld - sate %s locked mask %uc reason %s \n",
                                          pDescriptor.ZFBT->Tab[wgrab].Address,
                                          pDescriptor.ZFBT->Tab[wgrab].BlockSize,
                                          decode_ZBS( pDescriptor.ZFBT->Tab[wgrab].State) ) ;

                              break ;
                             }
                     wEndAddress = pDescriptor.ZFBT->Tab[wgrab].Address + pDescriptor.ZFBT->Tab[wgrab].BlockSize;

                     if(__ZRFVERBOSE__)
                         _DBGPRINT("Grabbing following free block : address %lld size %lld\n",
                                   pDescriptor.ZFBT->Tab[wgrab].Address,
                                   pDescriptor.ZFBT->Tab[wgrab].BlockSize);

                     // mark grabbed following free ZFBT block as deleted (grabbed to current)

                     if ((wSt=_markFreeBlockAsDeleted(pDescriptor,wgrab))!=ZS_SUCCESS)
                                                                             return wSt;
                     pDescriptor.ZFBT->Tab[wgrab].State = ZBS_Deleted;
                     pDescriptor.ZDBT->push(pDescriptor.ZFBT->Tab[wgrab]);  //! add entry into deleted blocks
                     pDescriptor.ZFBT->erase(wgrab);                        //! suppress ZFBT block from free block pool
                     break;
                     }
    }// if

    pBS.BlockSize = wEndAddress - pBS.Address;
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
ZRandomFile::_markBlockAsDeleted (ZFileDescriptor &pDescriptor, zrank_type pRank)
{
    if(__ZRFVERBOSE__)
        _DBGPRINT("Marking block from Block Access Table as ZBS_Deleted- address %lld size %lld\n",
                  pDescriptor.ZBAT->Tab[pRank].Address,
                  pDescriptor.ZBAT->Tab[pRank].BlockSize);

    pDescriptor.ZBAT->Tab[pRank].State = ZBS_Deleted;
//    pDescriptor.ZDBT->push(pDescriptor.ZBAT->Tab[pIndex]);  //! move the block header to deleted pool
    return _writeBlockHeader(pDescriptor,
                             (ZBlockHeader&) pDescriptor.ZBAT->Tab[pRank],
                             pDescriptor.ZBAT->Tab[pRank].Address);

}//_markBlockAsDeleted

/**
 * @brief ZRandomFile::_markFreeBlockAsDeleted gets a block header at given address pAddress sets its State flag to ZBS_Deleted and write it to disk
 *
 * @param pDescriptor
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_markFreeBlockAsDeleted (ZFileDescriptor &pDescriptor, zrank_type pRank)
{
    if(__ZRFVERBOSE__)
        _DBGPRINT("Marking block from Free Pool as ZBS_Deleted address %lld size %lld\n",
                  pDescriptor.ZFBT->Tab[pRank].Address,
                  pDescriptor.ZFBT->Tab[pRank].BlockSize);

    pDescriptor.ZFBT->Tab[pRank].State = ZBS_Deleted;
//    pDescriptor.ZDBT->push(pDescriptor.ZFBT->Tab[pIndex]);  //! move the block header to deleted pool
    return _writeBlockHeader(pDescriptor,
                             (ZBlockHeader&) pDescriptor.ZFBT->Tab[pRank],
                             pDescriptor.ZFBT->Tab[pRank].Address);

}//_markBlockAsDeleted
/**
 * @brief ZRandomFile::_markFreeBlockAsDeleted gets a block header at given address pAddress sets its State flag to ZBS_Deleted and write it to disk
 *
 * @param pDescriptor
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_markDeletedBlockAsFree (ZFileDescriptor &pDescriptor, zrank_type pRank)
{
    if(__ZRFVERBOSE__)
        _DBGPRINT("Marking deleted block from Recovery Pool as ZBS_Free address %lld size %lld\n",
                  pDescriptor.ZDBT->Tab[pRank].Address,
                  pDescriptor.ZDBT->Tab[pRank].BlockSize);

    pDescriptor.ZDBT->Tab[pRank].State = ZBS_Free;
    return _writeBlockHeader(pDescriptor,
                             (ZBlockHeader&) pDescriptor.ZDBT->Tab[pRank],
                             pDescriptor.ZDBT->Tab[pRank].Address);

}//_markDeletedBlockAsFree

/**
 * @brief ZRandomFile::_markFreeBlockAsUsed gets a block header at given address pAddress sets its State flag to ZBS_Used and write it to disk
 *
 * @param pDescriptor
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRandomFile::_markFreeBlockAsUsed (ZFileDescriptor &pDescriptor, zrank_type pRank)
{
    if(__ZRFVERBOSE__)
        _DBGPRINT("Marking block from Free Pool as ZBS_Used address %lld size %lld\n",
                  pDescriptor.ZFBT->Tab[pRank].Address,
                  pDescriptor.ZFBT->Tab[pRank].BlockSize);

    pDescriptor.ZFBT->Tab[pRank].State = ZBS_Used;
    return _writeBlockHeader(pDescriptor,
                             (ZBlockHeader&) pDescriptor.ZFBT->Tab[pRank],
                             pDescriptor.ZFBT->Tab[pRank].Address);

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
ZRandomFile::_recoverFreeBlock (ZFileDescriptor &pDescriptor, zrank_type pRank)
{
ZStatus wSt;
    if(__ZRFVERBOSE__)
            _DBGPRINT("Recovering Free block from Free block Pool  address %lld size %lld\n",
                      pDescriptor.ZFBT->Tab[pRank].Address,
                      pDescriptor.ZFBT->Tab[pRank].BlockSize);

// ungrab free block---------------------------------------------------------------------------------
//     search for any deleted block (grabbed blocks) inside free block  (from Deleted Block Pool)
//     for nearest deleted block : create an entry into Free Block Pool
//                                 mark block as ZBS_Free on file surface
//
    zaddress_type wAddMin = pDescriptor.ZFBT->Tab[pRank].Address;
    zaddress_type wAddMax = pDescriptor.ZFBT->Tab[pRank].Address + pDescriptor.ZFBT->Tab[pRank].BlockSize;

    long widx = 0;
    ZArray <long> wB;
    while (widx < pDescriptor.ZDBT->size())
    {
        wAddMin = pDescriptor.ZFBT->Tab[pRank].Address;
        wAddMax = pDescriptor.ZFBT->Tab[pRank].Address + pDescriptor.ZFBT->Tab[pRank].BlockSize;
        if ((pDescriptor.ZDBT->Tab[widx].Address > wAddMin)&&(pDescriptor.ZDBT->Tab[widx].Address < wAddMax)) //! it belongs to this free block
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
                        if (pDescriptor.ZDBT->Tab[wB[wi]].Address<wAddM1)
                                    {
                                    wAddM1 = pDescriptor.ZDBT->Tab[wB[wi]].Address;
                                    wIdxMin = wB[wi];
                                    }
                    }
            //! compute the former size of the block before it has been agglomerated by grabbing process
            //!
            pDescriptor.ZFBT->Tab[pRank].BlockSize = pDescriptor.ZDBT->Tab[wIdxMin].Address-pDescriptor.ZFBT->Tab[pRank].Address ;
            pDescriptor.ZDBT->Tab[wIdxMin].State = ZBS_Free;
            pDescriptor.ZFBT->push(pDescriptor.ZDBT->Tab[wIdxMin]); //! set the ungrabbed block to Free Pool as new free block
            _markDeletedBlockAsFree(pDescriptor,wIdxMin);
            pDescriptor.ZDBT->erase(wIdxMin);                       //! then remove it from recovery pool
            }

//  then   move free block from Free Pool to Block Access Table
//         mark it as ZBS_Used on file surface
//         remove it from Free Pool
//
    pDescriptor.ZFBT->Tab[pRank].State = ZBS_Used;
    pDescriptor.ZBAT->push(pDescriptor.ZFBT->Tab[pRank]);
    if ((wSt=_markFreeBlockAsUsed(pDescriptor,pRank))!=ZS_SUCCESS)
                            return wSt;
    pDescriptor.ZFBT->erase(pRank);

    return _writeFileDescriptor(pDescriptor,true) ;
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
ZRandomFile::_searchPreviousBlock (ZFileDescriptor &pDescriptor,
                                   zrank_type pRank,
                                   zaddress_type &pPreviousAddress,
                                   ZBlockHeader &pBlockHeader)
{
  return _searchPreviousPhysicalBlock(pDescriptor,pDescriptor.ZBAT->Tab[pRank].Address,pPreviousAddress,pBlockHeader);
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
//!                      ZS_OUTBOUNDLOW : either wCurrentAddress is the first address (   pPreviousAddres set to ZFCB->StartOfData )
//!                                       or no block start sign has been found (   pPreviousAddres set to ZFCB->StartOfData )
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
ZRandomFile::_searchPreviousPhysicalBlock (ZFileDescriptor &pDescriptor,
                                   zaddress_type pCurrentAddress,
                                   zaddress_type &pPreviousAddress,
                                   ZBlockHeader &pBlockHeader)
{


ZStatus         wSt;
ssize_t          wSizeToRead=0;
zaddress_type    wAddress;
//zoffset_type    wRealAddress;
ZDataBuffer     wBuffer;

    if(__ZRFVERBOSE__)
        _DBGPRINT("%s : scanning file surface for previous physical block for address %lld \n",_GET_FUNCTION_NAME_, pCurrentAddress);

ZBlockHeader_Export*   wBlockHeader_Export=nullptr;
ssize_t wRewindQuota ;

    if (pDescriptor.ZFCB->BlockTargetSize>0)
            wRewindQuota= sizeof(ZBlockHeader) + (pDescriptor.ZFCB->BlockTargetSize * 2); //! user content target size * 2 must be OK
        else
            wRewindQuota= pDescriptor.ZFCB->InitialSize ;


    uint8_t wStartSign [5]={cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE , 0 };



    if (pCurrentAddress==pDescriptor.ZFCB->StartOfData) //! Only if it is not the first block for the file because 0L is the start address for File effective Data blocks
        {
        pPreviousAddress = pDescriptor.ZFCB->StartOfData ;
        if(__ZRFVERBOSE__)
                _DBGPRINT("%s : reaching beginning of data returning address %lld \n",_GET_FUNCTION_NAME_, pPreviousAddress);
        return (ZS_OUTBOUNDLOW);        return (ZS_OUTBOUNDLOW);
        }
// position to previous block address and load the whole stuff (wSizeToRead size) into wBuffer
//
//     rewind wAdvanceQuota until finding a block start sign  (a block size is varying and may be larger than wAdvanceQuota)
//
//     once found : search to be sure it is the LAST block BEFORE wCurrentAddress
//
    long wOffset = -1;

// we have to make sure that the last byte is the start of current block for finding previous block

    if ((pCurrentAddress-wRewindQuota)<pDescriptor.ZFCB->StartOfData )
                        wRewindQuota = pCurrentAddress-pDescriptor.ZFCB->StartOfData;
    wSizeToRead = wRewindQuota ;
    wBuffer.allocate(wSizeToRead);
    wAddress = pDescriptor.ZFCB->StartOfData + 1; // just to initiate the loop


    while (wAddress >= pDescriptor.ZFCB->StartOfData)
    {
    wAddress = pCurrentAddress - wSizeToRead;
    if (wAddress < pDescriptor.ZFCB->StartOfData)           // we are before start of Data
                    {
                    wAddress = pDescriptor.ZFCB->StartOfData;
                    }
    if ((wSt=_readAt(pDescriptor,wBuffer,wSizeToRead,wAddress,ZPMS_Other))!=ZS_SUCCESS)
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
        if (wAddress == pDescriptor.ZFCB->StartOfData)  //! no start of block found till start of data : this is a hole since the beginning till current address
                    {
                    pPreviousAddress = pDescriptor.ZFCB->StartOfData ;
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
ZRandomFile::_searchNextBlock (ZFileDescriptor &pDescriptor,
                               zrank_type pRank,                     //! ZBAT index to search for next block
                               zaddress_type &pNextAddress,
                               ZBlockHeader &pBlockHeader)
{
    pNextAddress = pDescriptor.ZBAT->Tab[pRank].Address + pDescriptor.ZBAT->Tab[pRank].BlockSize + 1;  // the byte after block to search next
    return (_searchNextPhysicalBlock (pDescriptor,
                              pRank,                     // ZBAT index to search for next block
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
ZRandomFile::_searchNextPhysicalBlock (ZFileDescriptor &pDescriptor,
                                       zaddress_type pAddress,                     //! Address to start searching for for next block
                                       zaddress_type &pNextAddress,
                                       ZBlockHeader &pBlockHeader)  // ZBlockDescriptor is a ZBlockHeader + Address of block as an offset in file
{

ZStatus         wSt;
ssize_t         wSizeToRead=0;
size_t          wAdvanceQuota;
zaddress_type    wAddress;
ZDataBuffer     wBuffer;

    wAdvanceQuota = sizeof(ZBlockHeader_Export) + (pDescriptor.ZFCB->BlockTargetSize * 2); // user content target size * 2 must be OK
    wSizeToRead = sizeof(ZBlockHeader_Export) + (pDescriptor.ZFCB->BlockTargetSize * 2); // user content target size * 2 must be OK

    unsigned char wStartSign [5]={cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE , 0 };


    wAddress = pAddress;

    wBuffer.allocate(wSizeToRead);
    zaddress_type wOffset= -1;

     while (wOffset==-1)
     {

     if ((wSt=_readAt(pDescriptor,wBuffer,wSizeToRead,wAddress,ZPMS_Other))!=ZS_SUCCESS)
        {
        if (wSt==ZS_EOF)
            {
                pNextAddress = wAddress;
                return  (ZS_EOF);
            }
        if (wSt==ZS_READPARTIAL)
           {
            wOffset=wBuffer.bsearch(wStartSign,4L,0L);
            if (wOffset<0)
                {
                pNextAddress = wBuffer.Size + wAddress ;
                return  (ZS_EOF);
                }
                else
                {
                 pNextAddress = wAddress + wOffset;
                 wSt=pBlockHeader._importConvert(pBlockHeader,(ZBlockHeader_Export*)&wBuffer.DataChar[wOffset]);
                 if (wSt!=ZS_SUCCESS)
                             { return (wSt); }
//                           memmove(&pBlockHeader,&wBuffer.DataChar[wOffset],sizeof(ZBlockHeader));
                 return  (ZS_FOUND) ;
                 }
           }// ZS_READPARTIAL
        return  wSt;  // file error to be returned
        }

      wOffset=wBuffer.bsearch(wStartSign,4L,0L);
      if (wOffset==-1)
                {
                 wAddress += wAdvanceQuota ;
                }
     }// while

     pNextAddress = wAddress + wOffset;
     wSt=pBlockHeader._importConvert(pBlockHeader,(ZBlockHeader_Export*)&wBuffer.DataChar[wOffset]);
     if (wSt!=ZS_SUCCESS)
                 { return (wSt); }
//     memmove(&pBlockHeader,&wBuffer.DataChar[wOffset],sizeof(ZBlockHeader));
    return  (ZS_FOUND) ;
}//_searchNextPhysicalBlock


/**
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



 * @param[in] pDescriptor   File's descriptor
 * @param[in] pSize         Required size to find
 * @param[out] pBlock       ZBlockMin_struct :  returned address AND size of the file space allocated wherever it is
 * @param[in] pZBATIndex
 * @param[in] pBaseAddress  Base address for allocating a Free bock
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
long
ZRandomFile::_getFreeBlock (ZFileDescriptor &pDescriptor,
                            const size_t pSize,
                            ZBlockMin_struct& pBlock,
                            zrank_type pZBATRank,
                            const zaddress_type pBaseAddress)
{


zrank_type wIdx;
// debug
//
    if(__ZRFVERBOSE__)
            _DBGPRINT("request for free block size %ld for ZBAT index %ld\n", pSize,pZBATRank);

    for (long wi=0;wi<pDescriptor.ZFBT->size();wi ++)
            {
              if (pSize <= pDescriptor.ZFBT->Tab[wi].BlockSize)                 // Sizes match
                        {
                        if (pDescriptor.ZFBT->Tab[wi].Address<=pBaseAddress)  // must match base address condition
                                                                        continue;
                        if(__ZRFVERBOSE__)
                              _DBGPRINT(" found block in pool index %ld size of free block %lld - requested size %ld \n",
                                        wi,
                                        pDescriptor.ZFBT->Tab[wi].BlockSize,
                                        pSize);

                        wIdx= _allocateFreeBlock(pDescriptor, wi,pSize,pZBATRank);
                        if (wIdx<0)
                                {  return  (wIdx);}

//---------File descriptor will be updated once commit has been done
//

                        pDescriptor.ZBAT->Tab[wIdx].State = ZBS_Allocated;
                        pDescriptor.ZBAT->Tab[wIdx].Pid = pDescriptor.Pid;
                        pBlock = pDescriptor.ZBAT->Tab[wIdx];

                        pDescriptor.ZPMS.FreeMatches ++;
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


if ((wNewOffset=(zaddress_type)lseek(pDescriptor.ContentFd,0L,SEEK_END))<0)    // position to end of file and get position in return
                {
                ZException.getErrno(errno,
                                _GET_FUNCTION_NAME_,
                                ZS_FILEPOSERR,
                                Severity_Severe,
                                " Severe error while positionning to end of file %s",
                                pDescriptor.URIContent.toString());

                return  (ZS_FILEPOSERR);
                }

    wBlock.Address = wNewOffset;
    wBlock.State = ZBS_Free;
    wBlock.BlockSize = pSize;

// find previous logical free block in ZFBT (just before end of file)
//
    bool wFreeBlockGrabbed = false ;
    long wgrab;
    for (wgrab=0; wgrab<pDescriptor.ZFBT->size();wgrab++)
        {
            if (wNewOffset == (pDescriptor.ZFBT->Tab[wgrab].Address+pDescriptor.ZFBT->Tab[wgrab].BlockSize))
                        {
                        wBlock = pDescriptor.ZFBT->Tab[wgrab] ;              // grab this block
                        wNeededExtent = wNeededExtent - wBlock.BlockSize ;   // extend only for difference
                        wFreeBlockGrabbed=true;
                        break;
                        }
        }// for


    if (wNeededExtent>0)
    {
  wSt=_getExtent(pDescriptor,wBlockMan,wNeededExtent); // request an extension of at least pSize space
  if (wSt!=ZS_SUCCESS)
                { return  -1; }
    }
    long wZBATIndex = pZBATRank;

    wBlock.BlockSize = pSize;
    wBlock.State     = ZBS_Allocated;
    wBlock.Pid       = pDescriptor.Pid;

//        pDescriptor.ZPMS.FreeMatches ++;  //! this is not a free pool match but an extent

    if (pZBATRank<0)
            {
            wZBATIndex =pDescriptor.ZBAT->push(wBlock) ;
            }
        else
            {
            wZBATIndex=pDescriptor.ZBAT->insert(wBlock,pZBATRank);
            }
    if (wFreeBlockGrabbed)                      // if a free block has been grabbed : remove it from ZFBT
                pDescriptor.ZFBT->erase(wgrab);
//    pBlock = pDescriptor.ZBAT->Tab[pDescriptor.ZBAT->lastIdx()];
    pBlock = pDescriptor.ZBAT->Tab[wZBATIndex];

    if ( _writeBlockHeader(pDescriptor,(ZBlockHeader&)wBlock,wBlock.Address)!=ZS_SUCCESS)     // write it to file

// recompute BlockTargetSize
//
    if (!pDescriptor.ZBAT->isEmpty())
            pDescriptor.ZFCB->BlockTargetSize = (long) ((float)pDescriptor.ZFCB->AllocatedSize)/((float)pDescriptor.ZBAT->size());//! must be AllocatedSize / ZBAT.size() after block has been allocated

//---------File descriptor will be updated only once commit has been done
//
//   if (_writeFileDescriptor(pDescriptor) != ZS_SUCCESS)      // write back FCB to header file
//            {
//            { return  (-1);}
//            }

    return  (wZBATIndex);  // if OK : return index of free block allocated to ZBAT : either lastIdx() or the result of inserted block index
} // _getFreeBlock




/**
 * @brief ZRandomFile::_getExtent
 *
 * request to extend the file to get pSize bytes available :
 *
 *
 * - grab free space before end of file
 * - use posix_fallocate to get more disk space
 * - extend the file with a zeroed set of bytes
 *
 * - return a ZBlockDescriptor containing offset and size (size is >= pSize )
 *
 *  WARNING :  _getExtent does not write a block header but only free additional space on file
 *
 * @param pDescriptor
 * @param pBlockMin A ZBlockDescriptor newly returned with offset and size
 * @param pSize
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_getExtent(ZFileDescriptor &pDescriptor,
                        ZBlockDescriptor &pBlockMin,
                        const size_t pSize)
{


// test if needed space fits into one extent
//
zsize_type      wExtentSize ;
zaddress_type    wNewOffset;

    if ((wNewOffset=(zaddress_type)lseek(pDescriptor.ContentFd,0L,SEEK_END))<0)    // position to end of file and get position in return
                    {
                    ZException.getErrno(errno,
                                    _GET_FUNCTION_NAME_,
                                    ZS_FILEPOSERR,
                                    Severity_Severe,
                                    " Severe error while positionning to end of file %s",
                                    pDescriptor.URIContent.toString());


                    return   (ZS_FILEPOSERR);
                    }

    int wS = posix_fallocate(pDescriptor.ContentFd,wNewOffset,pSize);
    if (wS<0)
    {
    ZException.getErrno(errno,
                    _GET_FUNCTION_NAME_,
                    ZS_WRITEERROR,
                    Severity_Severe,
                    " Severe error while allocating (posix_fallocate) disk space size %ld to end of file %s",
                    pDescriptor.URIContent.toString());
    return  (ZS_WRITEERROR);
    }
    wExtentSize=pSize;

    ZDataBuffer wBuf;
    wBuf.allocate(wExtentSize);
    wBuf.clearData();

    pDescriptor.ZPMS.ExtentWrites ++;
    pDescriptor.ZPMS.ExtentSize += wBuf.Size ;

    if (write (pDescriptor.ContentFd,wBuf.Data,wBuf.Size)<wBuf.Size)      // write extent zeroed
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_WRITEERROR,
                                 Severity_Severe,
                                 " Severe error while extending file %s",
                                 pDescriptor.URIContent.toString());
                return  (ZS_WRITEERROR);
                }

    fdatasync(pDescriptor.ContentFd);

    pDescriptor.ZFCB->AllocatedSize += wExtentSize ;

    pBlockMin.Address    = wNewOffset;
    pBlockMin.BlockSize  = pSize;

    if(__ZRFVERBOSE__)
        _DBGPRINT(" extended file from offset %lld for size %lld for requested size %lld\n", wNewOffset, wExtentSize, pSize);
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
    return(_add(ZDescriptor,pUserBuffer,wAddress));
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
    return(_add(ZDescriptor,pUserBuffer,wAddress));
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
    return(_insert (ZDescriptor,pUserBuffer,0L,wAddress));
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


long wRank = ZDescriptor.ZBAT->lastIdx();
    wSt=_getByRank( ZDescriptor, wBlock,wRank,wAddress);
    if (wSt!=ZS_SUCCESS)
            return wSt;
    return(_freeBlock(ZDescriptor,wRank));
}//popRP

/*
long
ZRandomFile::move(size_t pDest, size_t pOrig,size_t pNumber)
{
ZStatus wSt;


    long wi=ZDescriptor.ZBAT->move(pDest,pOrig,pNumber);
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
    long wi=ZDescriptor.ZBAT->swap(pDest,pOrig,pNumber);
    wSt=_writeFileDescriptor(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
                return -1;
    return(wi);
}//swap

//---------------- Add sequence ---------------------------------------------------------

ZStatus
ZRandomFile::_add(ZFileDescriptor &pDescriptor,ZDataBuffer &pUserBuffer,zaddress_type &pAddress)
{


ZStatus         wSt;
long            wIdxCommit;

  wSt= _add2PhasesCommit_Prepare(pDescriptor,pUserBuffer,wIdxCommit,pAddress);
  if (wSt!=ZS_SUCCESS)
              {  return  wSt;  }
  return  _add2PhasesCommit_Commit(pDescriptor,pUserBuffer,wIdxCommit,pAddress);
}//_add



/**
 * @brief ZRandomFile::_add2PhasesCommit_Prepare Prepare to add a record.
 * Allocates a block in pool.
 * Returns a valid rank and an Address.
 *
 *      Newly allocated block is marked ZBS_Allocated in ZBlockAccessTable pool (Meaning it is not usable for any other operation than commit)
 *
 *
 * @param[in] pDescriptor     File descriptor : file descriptor is modified in its ZBAT component (ZFCB)
 * @param[in] pUserBuffer     Input: user record to add to file.
 * @param[out] pIdxCommit      Return : current index within ZBAT pool of block allocated (currently being created)
 * @param[out] pLogicalAddress Return : logical address (file offset since start of data) for current block being created.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_add2PhasesCommit_Prepare(ZFileDescriptor &pDescriptor,
                                       const ZDataBuffer &pUserBuffer,
                                       zrank_type &pIdxCommit,
                                       zaddress_type &pLogicalAddress)
{


ZStatus wSt;
//------ Lock file for adding record-----------------
//
//
    wSt = _lockFile( pDescriptor,ZLock_ReadWrite);
    if (wSt!=ZS_SUCCESS)
                { return   wSt;}

    size_t wNeededSize = pUserBuffer.Size + sizeof(ZBlockHeader_Export);

ZBlockMin_struct    wBlockMin;

    pIdxCommit=_getFreeBlock (pDescriptor,
                             wNeededSize,wBlockMin,-1); // scan FreeTable for available block in holes, or give the first address at the end or extend the file according extentquota to get available space
    if (pIdxCommit<0)
            {return  ZException.getLastStatus();}

    pDescriptor.ZBAT->Tab[pIdxCommit].State     = ZBS_Allocated ;
    pDescriptor.ZBAT->Tab[pIdxCommit].Pid       = pDescriptor.Pid;
    pDescriptor.ZBAT->Tab[pIdxCommit].BlockSize = wNeededSize;

//    CurrentRecord = pUserBuffer;
    if(__ZRFVERBOSE__)
            _DBGPRINT("-- _add-- adding block at %lld size %lld \n",
                      pDescriptor.ZBAT->Tab[pIdxCommit].Address,
                      pDescriptor.ZBAT->Tab[pIdxCommit].BlockSize);


    pLogicalAddress = pDescriptor.setLogicalFromPhysical( pDescriptor.ZBAT->Tab[pIdxCommit].Address) ;
    pDescriptor.CurrentRank = pIdxCommit;

    return   ZS_SUCCESS;
}//_add2PhasesCommit_prepare
/**
 * @brief ZRandomFile::_add2PhasesCommit_Commit Physically writes the record (and block) content to content file using information given from add_Prepare

    creates the block containing user record (pUserBuffer).
    Block is then marked in ZBAT pool as ZBS_Used.

 * @param pDescriptor
 * @param pUserBuffer     Input: user record to add to file.
 * @param pIdxCommit      Return : current index within ZBAT pool of block  (created)
 * @param pLogicalAddress Return : logical address (file offset since start of data) for current block having be created.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_add2PhasesCommit_Commit(ZFileDescriptor &pDescriptor,
                                      const ZDataBuffer &pUserBuffer,
                                      const zrank_type pIdxCommit,
                                      zaddress_type &pLogicalAddress)
{


ZStatus wSt;
ZBlock  wBlock;

    pDescriptor.ZBAT->Tab[pIdxCommit].State = ZBS_Used;
    pDescriptor.ZBAT->Tab[pIdxCommit].Pid   = 0L;

    wBlock = pDescriptor.ZBAT->Tab[pIdxCommit];
    wBlock.State = ZBS_Used ;
    wBlock.Content=pUserBuffer;
    if ((wSt=_writeBlockAt(pDescriptor,wBlock,pDescriptor.ZBAT->Tab[pIdxCommit].Address))!=ZS_SUCCESS)
                                {   return  wSt;}

    pDescriptor.ZFCB->UsedSize += wBlock.BlockSize ;

    pLogicalAddress = pDescriptor.setLogicalFromPhysical( pDescriptor.ZBAT->Tab[pIdxCommit].Address);

    wSt=_writeFileDescriptor(pDescriptor,true);
    if (wSt!=ZS_SUCCESS)
            {
             _unlockFile(pDescriptor);
            return  wSt;
            }

    return  _unlockFile(pDescriptor);

}//_add2PhasesCommit_commit

/**
 * @brief ZRandomFile::_add2PhasesCommit_Rollback Undo add_Prepare operation. Frees the allocated block : makes it available in Free block pool.
 * @param pDescriptor
 * @param pIdxCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_add2PhasesCommit_Rollback(ZFileDescriptor &pDescriptor, const zrank_type pIdxCommit)
{
ZStatus wSt;
ZBlockDescriptor wBS;

    wBS   = pDescriptor.ZBAT->Tab[pIdxCommit];

    if (pDescriptor.ZFCB->GrabFreeSpace)
            {
            if ((wSt=_grabFreeSpaceLogical(pDescriptor,pIdxCommit,wBS))!=ZS_SUCCESS)
                                                                return wSt;

            }

    wBS.State = ZBS_Free;
    pDescriptor.ZFBT->push(wBS);            // create entry into ZFBT for freed Block
    pDescriptor.ZBAT->erase(pIdxCommit);    // remove entry from ZBAT

    pDescriptor.CurrentRank = -1;          // no current index

    return _unlockFile(pDescriptor);
}//_add2PhasesCommit_rollback

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

    return(_remove(ZDescriptor,pRank));

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

    long wRank = searchBlockRankByAddress(ZDescriptor,pAddress);
    if (wRank<0)
            return ZS_INVADDRESS;
    return _remove(ZDescriptor,wRank);
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
        return(_removeR(ZDescriptor,pRecord,pRank));
}// zremoveR

//----------Remove By Rank stuff---------------------------------------------

ZStatus
ZRandomFile::_remove(ZFileDescriptor &pDescriptor, const zrank_type pRank)
{
ZStatus wSt;
zaddress_type wAddress;
    wSt=_remove_Prepare(pDescriptor,pRank,wAddress);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return _remove_Commit(pDescriptor,pRank);
}// _remove

//! @cond development
/**
 * @brief ZRandomFile::_remove_Prepare prepares to remove a block pointed by its rank (within ZBlockAccessTable pool) and returns its Address
 *
 *  Locks the file
 *  Block (in ZBAT pool) is marked ZBS_BeingDeleted.
 *
 *
 * @param[in]   pDescriptor
 * @param[in]   pRank           Input : rank of the block to delete (ZBAT pool index)
 * @param[out]  pLogicalAddress Return: the logical address of the block being deleted
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_remove_Prepare(ZFileDescriptor &pDescriptor, const zrank_type pRank, zaddress_type &pLogicalAddress)
{
ZStatus wSt;

    wSt = _lockFile (pDescriptor,ZLock_All ) ; // lock Master File for update after having tested if already locked

    if (wSt!=ZS_SUCCESS)        // either file is already locked or a Severe IO error
                return wSt;

    wSt=pDescriptor.testRank(pRank,_GET_FUNCTION_NAME_);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    pLogicalAddress=pDescriptor.setLogicalFromPhysical( pDescriptor.ZBAT->Tab[pRank].Address);
    _freeBlock_Prepare(pDescriptor,pRank);
    pDescriptor.CurrentRank = pRank ;

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
ZRandomFile::_removeR(ZFileDescriptor &pDescriptor, ZDataBuffer &pUserBuffer, const zrank_type pRank)
{
ZStatus wSt;
zaddress_type wAddress ;
    wSt=_removeR_Prepare(pDescriptor,pUserBuffer,pRank,wAddress);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return _remove_Commit(pDescriptor,pRank);
}// _removeR

ZStatus
ZRandomFile::_removeR_Prepare(ZFileDescriptor &pDescriptor, ZDataBuffer &pUserBuffer, const zrank_type pRank, zaddress_type &pAddress)
{
ZStatus wSt;
ZBlock wBlock;

    wSt = _lockFile (pDescriptor,ZLock_All ) ; // lock Master File for update after having tested if already locked

    if (wSt!=ZS_SUCCESS)        // either file is already locked or a Severe IO error
                return wSt;

    wSt=_getByRank (pDescriptor,wBlock,pRank,pAddress); // lock record RFFU may be replaced by LockRecord method
    if (wSt!=ZS_SUCCESS)
                return wSt;
    pUserBuffer = wBlock.Content ;

    _freeBlock_Prepare(pDescriptor,pRank);

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
ZRandomFile::_removeByAddress(ZFileDescriptor &pDescriptor,const zaddress_type &pAddress)
{
ZStatus wSt;
long wIdxCommit;
    wSt=_removeByAddress_Prepare(pDescriptor,wIdxCommit,pAddress);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return _remove_Commit(pDescriptor,wIdxCommit);
}// _removeByAddress

/**
 * @brief ZRandomFile::_removeByAddress_Prepare  Prepares to remove a block at address given by pAddress and returns its rank in ZBlockAccessTable pool.
 * @param pDescriptor
 * @param pIdxCommit
 * @param pAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_removeByAddress_Prepare(ZFileDescriptor &pDescriptor, zrank_type &pIdxCommit, const zaddress_type pAddress)
{
ZStatus wSt;

    wSt = _lockFile (pDescriptor,ZLock_All ) ; //! lock Master File for update after having tested if already locked

    if (wSt!=ZS_SUCCESS)        //! either file is already locked or a Severe IO error
                return wSt;

    for (long wi = 0; wi < pDescriptor.ZBAT->size();wi++)
    {
            if (pDescriptor.ZBAT->Tab[wi].Address == pAddress)
            {
                pIdxCommit=wi;
                _freeBlock_Prepare(pDescriptor,pIdxCommit);
                pDescriptor.CurrentRank = wi ;
                return ZS_SUCCESS;
            }
    }//for
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVADDRESS,
                            Severity_Severe,
                            "Given address %lld does not correspond to a valid start of block address in pool ZBAT for file %s",
                            pAddress,
                            pDescriptor.URIContent.toCChar());
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
ZRandomFile::_removeRByAddress(ZFileDescriptor &pDescriptor, ZDataBuffer &pUserBuffer, zrank_type &pIdxCommit, const zaddress_type pAddress)
{
ZStatus wSt;

    wSt=_removeRByAddress_Prepare(pDescriptor,pUserBuffer,pIdxCommit,pAddress);
    if (wSt!=ZS_SUCCESS)
            return (wSt);
    _remove_Commit(pDescriptor,pIdxCommit);
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
ZRandomFile::_removeRByAddress_Prepare(ZFileDescriptor &pDescriptor, ZDataBuffer &pUserBuffer, zrank_type &pIdxCommit, const zaddress_type pAddress)
{
ZStatus wSt;
ZBlock wBlock;
zaddress_type wAddress;

    wSt = _lockFile (pDescriptor,ZLock_All ) ; //! lock Master File for update after having tested if already locked

    if (wSt!=ZS_SUCCESS)        //! either file is already locked or a Severe IO error
                return wSt;
    for (long wi = 0; wi < pDescriptor.ZBAT->size();wi++)
    {
            if (pDescriptor.ZBAT->Tab[wi].Address == pAddress)
            {
                pIdxCommit=wi;
                wSt=_getByRank (pDescriptor,wBlock,pIdxCommit,wAddress);
                if (wSt!=ZS_SUCCESS)
                        return wSt;
                pUserBuffer = wBlock.Content ;

                _freeBlock_Prepare(pDescriptor,pIdxCommit);
                pDescriptor.CurrentRank = wi ;
                return ZS_SUCCESS;
            }// if
    }//for
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVADDRESS,
                            Severity_Severe,
                            "Given address %lld does not correspond to a valid start of block address in pool ZBAT for file %s",
                            pAddress,
                            pDescriptor.URIContent.toString());
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
ZRandomFile::_remove_Commit(ZFileDescriptor &pDescriptor, const zrank_type pIdxCommit)
{
ZStatus wSt, wSt1;

    pDescriptor.CurrentRank = -1 ;
    wSt=_freeBlock_Commit(pDescriptor,pIdxCommit);

    wSt1 = _unlockFile (pDescriptor ) ; // lock Master File for update after having tested if already locked

    if (wSt1!=ZS_SUCCESS)        // either file is already locked or a Severe IO error
                return wSt1;
    return wSt;
}// _remove_Commit

/**
 * @brief ZRandomFile::_remove_Rollback common to all remove methods for rejecting remove command and restoring situation
 * @param pDescriptor
 * @param pIdxCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_remove_Rollback(ZFileDescriptor &pDescriptor, const zrank_type pIdxCommit)
{
     _freeBlock_Rollback(pDescriptor,pIdxCommit);  //! NB: Current index stays set because ZBAT rank is still active and last accessed
     return _unlockFile (pDescriptor );
}// _remove_Rollback


//---------------- Insert sequence ---------------------------------------------------------
//

ZStatus
ZRandomFile::_insert(ZFileDescriptor &pDescriptor, const ZDataBuffer &pUserBuffer, const zrank_type pRank, zaddress_type &pLogicalAddress)
{
ZStatus wSt;
zrank_type wIdxCommit ;
    if ((wSt=_insert2PhasesCommit_Prepare(pDescriptor,pUserBuffer,pRank,wIdxCommit,pLogicalAddress))!=ZS_SUCCESS)
                                return wSt;
    return _insert2PhasesCommit_Commit(pDescriptor,pUserBuffer,wIdxCommit);
}//_insert

/**
 * @brief ZRandomFile::_insert2PhasesCommit_Prepare  Prepares to insert a record contained in pUserBuffer at rank pRank
            Insertion will be finalized after _insert2PhasesCommit_Commit.
            Returns the logical address of block being inserted.
            Block is marked as ZBS_Allocated (meaning locked in creation process).




 * @param pDescriptor
 * @param pUserBuffer
 * @param pRank
 * @param pIdxCommit
 * @param pLogicalAddress
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_insert2PhasesCommit_Prepare(ZFileDescriptor &pDescriptor,
                                          const ZDataBuffer &pUserBuffer,
                                          const zrank_type pRank,
                                          zrank_type &pIdxCommit,
                                          zaddress_type &pLogicalAddress)
{
ZStatus wSt;
size_t wNeededSize = pUserBuffer.Size + sizeof(ZBlockHeader);

ZBlockMin_struct    wBlockMin;

    if (!(pDescriptor.Mode & ZRF_Write_Only))  // open mode allows to write ?
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_MODEINVALID,
                                    Severity_Error,
                                    " File open mode <%s> does not allow record insertion file %s",
                                    decode_ZRFMode(pDescriptor.Mode),
                                    pDescriptor.URIContent.toString());
            return ZS_MODEINVALID;
            }

    wSt = _lockFile (pDescriptor,ZLock_ReadWrite ) ; // lock Master File for update after having tested if already locked
    if (wSt!=ZS_SUCCESS)        // either file is already locked or a Severe IO error
                return wSt;

     pIdxCommit=_getFreeBlock ( pDescriptor,
                                wNeededSize,
                                wBlockMin,
                                pRank); // scan FreeTable for available block in holes, or give the first address that corresponds to size or extend the file according extentquota to get available space
// and create ZBAT entry at pRank
//
if (pIdxCommit<0)  // wIdx must be equal to pRank or negative if an error has been encountered
        return  ZException.getLastStatus();

//pDescriptor.ZBAT->Tab[pIdxCommit].State = ZBS_Used ;
pDescriptor.ZBAT->Tab[pIdxCommit].BlockSize = wNeededSize;

//    CurrentRecord = pUserBuffer;
    if(__ZRFVERBOSE__)
        _DBGPRINT("-- _add-- adding block at %lld size %lld \n",
                  pDescriptor.ZBAT->Tab[pIdxCommit].Address,
                  pDescriptor.ZBAT->Tab[pIdxCommit].BlockSize);

    pLogicalAddress = pDescriptor.setLogicalFromPhysical (pDescriptor.ZBAT->Tab[pIdxCommit].Address) ;      //! gives back logical from physical address

    pDescriptor.CurrentRank = pIdxCommit;

    return ZS_SUCCESS;
}//_insert2PhasesCommit_Prepare


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
ZRandomFile::_insert2PhasesCommit_Commit(ZFileDescriptor &pDescriptor, const ZDataBuffer &pUserBuffer, const zrank_type pIdxCommit)
{
ZStatus wSt;
ZBlock  wBlock;
    pDescriptor.ZBAT->Tab[pIdxCommit].State = ZBS_Used ;
    wBlock = pDescriptor.ZBAT->Tab[pIdxCommit];
    wBlock.Content=pUserBuffer;
    if ((wSt=_writeBlockAt(pDescriptor,wBlock,pDescriptor.ZBAT->Tab[pIdxCommit].Address))!=ZS_SUCCESS)
                                        return wSt;

    pDescriptor.ZFCB->UsedSize += wBlock.BlockSize ;
//    pAddress = pDescriptor.ZBAT->Tab[pIdxCommit].Address;
    wSt=_writeFileDescriptor(pDescriptor,true);
    if (wSt!=ZS_SUCCESS)
    {
        _unlockFile(pDescriptor);
        return wSt;
    }
    return(_unlockFile(pDescriptor));
}//_insert2PhasesCommit_Commit

ZStatus
ZRandomFile::_insert2PhasesCommit_Rollback(ZFileDescriptor &pDescriptor, const zrank_type pIdxCommit)
{
ZStatus wSt;
ZBlockDescriptor wBS;

    wBS   = pDescriptor.ZBAT->Tab[pIdxCommit];
    wBS.State       = ZBS_Free;

    if (pDescriptor.ZFCB->GrabFreeSpace)
            {
//            if ((wSt=_grabFreeSpacePhysical(pDescriptor,pIndex,wBS))!=ZS_SUCCESS)
//                                                                return wSt;
            if ((wSt=_grabFreeSpaceLogical(pDescriptor,pIdxCommit,wBS))!=ZS_SUCCESS)
                                                                return wSt;

            }
    pDescriptor.ZFBT->push(wBS);            //! create entry into ZFBT for freed Block
    pDescriptor.ZBAT->erase(pIdxCommit);    //! remove entry from ZBAT

//    pDescriptor.ZFCB->UsedSize -= wBS.BlockSize ;
    pDescriptor.CurrentRank = -1;          //! No more current index available

    return _unlockFile(pDescriptor);
}//_add2PhasesCommit_Rollback

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
ZRandomFile::_writeBlockAt(ZFileDescriptor& pDescriptor,
                         ZBlock &pBlock,
                         const zaddress_type pAddress)
{


ZStatus wSt;

    pBlock.BlockSize = pBlock.Content.Size + sizeof(ZBlockHeader_Export);
    wSt=_writeBlockHeader(pDescriptor,(ZBlockHeader&)pBlock,pAddress);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

    ssize_t wSWrite= write(pDescriptor.ContentFd,pBlock.Content.DataChar,pBlock.Content.Size);// then write block user content
    if (wSWrite <0 )
        {
        pDescriptor.ZPMS.PMSCounterWrite(ZPMS_User,0);
        ZException.getErrno (errno,             // NB: write function sets errno in case of error
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEERROR,
                          Severity_Severe,
                          "Error while writing block user content (Data). Address %lld size %ld",
                          pDescriptor.PhysicalPosition,
                          pBlock.BlockSize);

         return  wSt;
        }
    pDescriptor.ZPMS.PMSCounterWrite(ZPMS_User,wSWrite);
    pDescriptor.incrementPosition(wSWrite);

    fdatasync(pDescriptor.ContentFd); // better than flush

    if (wSWrite < pBlock.Content.Size)
        {
        ZException.getErrno (errno,
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEPARTIAL,
                          Severity_Error,
                          "Block user content has been partially written on file : content address %lld size requested %ld size written %lld",
                          (pAddress+sizeof(ZBlockHeader)),
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
ZRandomFile::_writeBlockHeader(ZFileDescriptor& pDescriptor,
                               ZBlockHeader &pBlockHeader,
                               const zaddress_type pAddress)
{

    ZStatus wSt = _seek(pDescriptor,pAddress);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}

    ZBlockHeader_Export wBlockExp;

    ZBlockHeader::_exportConvert(pBlockHeader,&wBlockExp);

    ssize_t wSWrite= write(pDescriptor.ContentFd,&wBlockExp,sizeof(ZBlockHeader_Export));

    if (wSWrite <0 )
        {
        pDescriptor.ZPMS.PMSCounterWrite(ZPMS_BlockHeader,0);
        ZException.getErrno (errno,             // NB: write function sets errno in case of error
                          _GET_FUNCTION_NAME_,
                          ZS_WRITEERROR,
                          Severity_Severe,
                          "Error while writing block header address %lld file <%s>",
                         pAddress,
                         pDescriptor.URIContent.toString());
         return  ZS_WRITEERROR;
        }

//    pDescriptor.ZPMS.CBHWriteBytesSize += wSWrite;
    pDescriptor.ZPMS.PMSCounterWrite(ZPMS_BlockHeader,wSWrite);

    pDescriptor.PhysicalPosition += wSWrite;
    pDescriptor.LogicalPosition += wSWrite;

    fdatasync(pDescriptor.ContentFd); //! better than flush
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
                             pDescriptor.URIContent.toString());
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
    if ((wSt=ZDescriptor.testRank(pRank,_GET_FUNCTION_NAME_))!=ZS_SUCCESS)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   wSt,
                                   Severity_Error,
                                   "Invalid block rank value %ld. Out of file boundaries for file %s",
                                   pRank,
                                   ZDescriptor.URIContent.toString());
            return  (wSt);
            }

// got to manage locking there

//    ZDescriptor.setIndex(pRank);

    pBlockDescriptor = ZDescriptor.ZBAT->Tab[pRank];
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
        if (pRank>ZDescriptor.ZFBT->lastIdx())
                            wSt= ZS_OUTBOUNDHIGH;

    if (wSt != ZS_SUCCESS)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   wSt,
                                   Severity_Error,
                                   "Invalid free block rank value %ld. Out of boundaries for file %s",
                                   pRank,
                                   ZDescriptor.URIContent.toString());
            return (wSt);
            }

//===== got to manage locking there ==================

//    ZDescriptor.setIndex(pRank);

    pBlockDescriptor = ZDescriptor.ZFBT->Tab[pRank];
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
    if (pRank>ZDescriptor.ZBAT->lastIdx())
              wSt= ZS_OUTBOUNDHIGH;

    if (wSt != ZS_SUCCESS)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   wSt,
                                   Severity_Error,
                                   "Invalid free block rank value %ld. Out of boundaries for file %s",
                                   pRank,
                                   ZDescriptor.URIContent.toString());
            return  (wSt);
            }

//========== got to manage locking there =============================

    if ((wSt=_recoverFreeBlock (ZDescriptor,pRank))!=ZS_SUCCESS)
                                   { return  wSt;}//

    pZBATIdx=ZDescriptor.ZBAT->lastIdx();
    return  ZS_SUCCESS;
}// zrecover

ZStatus
ZRandomFile::zget(void* pRecord, size_t &pSize, const zrank_type pRank)
{
ZStatus wSt;
ZBlock wBlock;
zaddress_type wAddress;

    if ((wSt=_getByRank(ZDescriptor,
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
    if ((wSt=_getByRank(ZDescriptor,
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
    if ((wSt=_getByRank(ZDescriptor,
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

    if ((wSt=_getNext(ZDescriptor,
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

    if ((wSt=_getNext(ZDescriptor,
                      wBlock,
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
    if ((wSt=_getNext(ZDescriptor,
                      wBlock,
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
    pRank=ZDescriptor.decrementRank();
    if (pRank<0)
            return ZS_OUTBOUNDLOW;
    if ((wSt=_getByRank(ZDescriptor,
                  wBlock,
                  pRank,
                  pAddress))!=ZS_SUCCESS)
                {
                return wSt;
                }
    pRecord=wBlock.Content;
    return(ZS_SUCCESS);
}// zgetLastWAddress

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
ZRandomFile::_getNext(ZFileDescriptor & pDescriptor,
                      ZBlock &pBlock,
                      zrank_type &pRank,
                      zaddress_type &pAddress)
{

    if (pDescriptor.incrementRank()<0)
                                return ZS_EOF;
    pAddress = pDescriptor.getCurrentPhysicalAddress();
    pRank = pDescriptor.getCurrentRank();
    return _readBlockAt(pDescriptor,pBlock,pAddress);
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
    if ((wSt=_readBlockAt(ZDescriptor,
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

    if ((pRank>ZDescriptor.ZBAT->size())||(pRank<0))
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_OUTBOUND,
                                Severity_Severe,
                                "Invalid relative record number <%ld> while Used pool size is [0,<%ld>]",
                                pRank,
                                ZDescriptor.ZBAT->lastIdx());
        return  ZS_OUTBOUND;
    }

//     zaddress_type wLogical = setPhysicalFromLogical(pAddress);

    zaddress_type wFieldAddress = ZDescriptor.ZBAT->Tab[pRank].Address + sizeof(ZBlockHeader) + pOffset ;

    off_t wOff = lseek(ZDescriptor.ContentFd,(off_t)( wFieldAddress),SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning file  at logical address <%lld> file <%s>",
                                 wFieldAddress,
                                 ZDescriptor.URIContent.toString()
                                 );
                return (ZS_FILEPOSERR);
                }

    // then just read from file the segment of data on field length
    return  _read(ZDescriptor,           // allocate pLength for field buffer, reads then manage errors if ever - updates stats
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

    if ((pRank>ZDescriptor.ZBAT->size())||(pRank<0))
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVADDRESS,
                                Severity_Severe,
                                "Invalid relative record number <%ld> while Used pool size is <%ld>",
                                pRank,
                                ZDescriptor.ZBAT->size());
        return  ZS_INVADDRESS;
    }

//     zaddress_type wLogical = setPhysicalFromLogical(pAddress);

    zaddress_type wFieldAddress = ZDescriptor.ZBAT->Tab[pRank].Address + sizeof(ZBlockHeader) + pOffset ;

    off_t wOff = lseek(ZDescriptor.ContentFd,(off_t)( wFieldAddress),SEEK_SET);
    if (wOff<0)
                {

                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_FILEPOSERR,
                                 Severity_Severe,
                                 "Error positionning file  at logical address <%lld> file <%s>",
                                 wFieldAddress,
                                 ZDescriptor.URIContent.toString()
                                 );
                return (ZS_FILEPOSERR);
                }

    // then just read from file the segment of data on field length
    return  _read(ZDescriptor,           // allocate pLength for field buffer, reads then manage errors if ever - updates stats
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
    return(_add(ZDescriptor,pRecord,pAddress));;
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


    if (ZDescriptor.ZBAT->isEmpty())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Error,
                                    " File is empty. Cannot get last record for file <%s>",
                                    ZDescriptor.URIContent.toString());
             return  ZS_OUTBOUND;
             }
     ZDescriptor.CurrentRank = ZDescriptor.ZBAT->lastIdx();
     pRank= ZDescriptor.CurrentRank;
     return  zget(pRecord,ZDescriptor.ZBAT->lastIdx());
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

    if (ZDescriptor.ZBAT->isEmpty())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Error,
                                    " File is empty. Cannot get last record for file <%s>",
                                    ZDescriptor.URIContent.toString());
             return  ZS_OUTBOUND;
             }
     ZDescriptor.CurrentRank = ZDescriptor.ZBAT->lastIdx();
     pRank = ZDescriptor.CurrentRank;
     return  zgetWAddress(pRecord,ZDescriptor.CurrentRank,pAddress);
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

    if (ZDescriptor.CurrentRank==0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUNDLOW,
                                    Severity_Error,
                                    "Already at first record : cannot get Previous record for file <%s>",
                                    ZDescriptor.URIContent.toString());
             return  ZS_OUTBOUNDLOW;
             }
    ZDescriptor.CurrentRank--;
    return  zget(pRecord,ZDescriptor.CurrentRank);
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
ZRandomFile::_getByRank(ZFileDescriptor & pDescriptor,
                  ZBlock &pBlock,
                  const long pRank,
                  zaddress_type &pAddress)
{


ZStatus wSt;
    if ((wSt=pDescriptor.testRank(pRank,_GET_FUNCTION_NAME_))!=ZS_SUCCESS)
                {
                ZException.addToLast("| Wrong status getting block rank %ld status %s for file %s",
                                       pRank,
                                       decode_ZStatus(wSt),
                                       pDescriptor.URIContent.toString());

                return (wSt); // can be out of boundaries (program issue) OR locked for deletion - locked for creation (concurrent access)
                }

    pDescriptor.setRank(pRank);
// should test lock here
/*    if (pLock!=ZLock_Nolock)
                _lock(pDescriptor,pRank,pLock,false);
*/
    pAddress = pDescriptor.ZBAT->Tab[pRank].Address;
    return  (_readBlockAt(pDescriptor,
                        pBlock,
                        pDescriptor.ZBAT->Tab[pRank].Address));

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
ZRandomFile::_getByAddress (ZFileDescriptor & pDescriptor,
                            ZBlock &pBlock,
                            const zaddress_type pAddress)
{
 /*
long wi;

   for (wi=0;wi<pDescriptor.ZBAT->size();wi++)
            {
            if (pDescriptor.ZBAT->Tab[wi].Address == pAddress)
                    {
                    pRank = wi;
                    break;
                    }
            }// for
    if (wi == pDescriptor.ZBAT->size())
                            return ZS_NOTFOUND;
*/
return(_readBlockAt(pDescriptor,
                    pBlock,
                    pAddress));
}// _getByAddress




ZStatus
ZRandomFile::zinsert(const ZDataBuffer &pRecord, zrank_type pRank)
{

zaddress_type wAddress;

    return _insert(ZDescriptor,pRecord,pRank,wAddress);

}// zinsert


//! @cond Development
/**
 * @brief ZRandomFile::_open low level open file routine
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
 * @param[inout] pDescriptor the ZFileDescriptor to be associated with the file to open
 * @param[in] pMode          a ZRFMode_type defining the open mode
 * @param[in] pFileType      a ZFile_type defining the type of file to open
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRandomFile::_open(ZFileDescriptor &pDescriptor,
                   const zmode_type pMode, const ZFile_type pFileType, bool pLockRegardless)
{


ZStatus wSt=ZS_SUCCESS;

    if (pDescriptor._isOpen)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_ERROPEN,
                                Severity_Error,
                                "Cannot open <%s>.It is already open with mode <%s>",
                                pDescriptor.URIContent.toString(),
                                decode_ZRFMode(pDescriptor.Mode));
        return  (ZS_ERROPEN);
        }
    pDescriptor.CurrentRank = -1;

    pDescriptor.ZPMS.clear();                   // reset performance data collection

    if (!pDescriptor.URIContent.exists())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_FILENOTEXIST,
                                   Severity_Error,
                                   "File <%s> does not exist. It must exist to be opened. Use zcreate with appropriate options.",
                                   pDescriptor.URIContent.toString());
            return  (ZS_FILENOTEXIST);
            }

    pDescriptor.ContentFd = open(pDescriptor.URIContent.toCString_Strait(),O_RDWR);

   if (pDescriptor.ContentFd<0)
               {
               ZException.getErrno(errno,
                               _GET_FUNCTION_NAME_,
                               ZS_ERROPEN,
                               Severity_Severe,
                               "Error opening file <%s> ",
                               pDescriptor.URIContent.toString());
               return  (ZS_ERROPEN);
               }

    wSt = generateURIHeader (pDescriptor.URIContent,pDescriptor.URIHeader);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}
    pDescriptor.HeaderFd = open(pDescriptor.URIHeader.toCString_Strait(),O_RDWR);
    if (pDescriptor.HeaderFd < 0)
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_ERROPEN,
                            Severity_Severe,
                            "Error opening header file <%s>. ZRF file has not been opened.",
                            pDescriptor.URIHeader.toString());
            pDescriptor._isOpen=false;
            close(pDescriptor.ContentFd);

            return  (ZS_ERROPEN);
            }

    pDescriptor.setupFCB();  // update pDescriptor

    wSt=_getFullFileHeader(pDescriptor,true);  // get header and force read pForceRead = true, whatever the open mode is
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}
    if (!pLockRegardless)
    {
    if (pDescriptor.ZHeader.Lock & ZRF_Exclusive)
                        {
                        close (pDescriptor.ContentFd);
                        close(pDescriptor.HeaderFd);
                        pDescriptor._isOpen=false;

 //                       ZRFPool->removeFileByFd(pDescriptor.ContentFd);

                        ZException.setMessage(_GET_FUNCTION_NAME_,
                                                ZS_LOCKED,
                                                Severity_Error,
                                                "file %s is locked. lock mask is %s. File has not been opened.",
                                                pDescriptor.URIContent.toString(),
                                                decode_ZLockMask (pDescriptor.ZHeader.Lock).toChar()
                                                );
                        return  ZS_LOCKED;
                        }
    }
//-----------------------------File type test------------------------------------

    if (pFileType == ZFT_Nothing)
            {
            close (pDescriptor.ContentFd);
            close(pDescriptor.HeaderFd);
            pDescriptor._isOpen=false;

//            ZRFPool->removeFileByFd(pDescriptor.ContentFd);

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                    pDescriptor.URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (pDescriptor.ZHeader.FileType));
            return  ZS_BADFILETYPE;
            }
//    fprintf (stdout,"%s>> file type is <%d>  requested file type is <%d>\n",_GET_FUNCTION_NAME_,pDescriptor.ZHeader.FileType,pFileType);
    switch (pDescriptor.ZHeader.FileType)
    {
        case ZFT_ZRandomFile :
            {
            if (pFileType==ZFT_ZRandomFile)
                                {
                                wSt=ZS_SUCCESS;
                                break; // everything in line
                                }
            if ((pFileType==ZFT_ZMasterFile))   // file will be upgraded must be opened is ZRF_Exclusive
                {
                if (!(pMode & ZRF_Exclusive)) // Only ZRF_Exclusive for opening a ZMasterFile as ZRandomFile
                    {
                    close (pDescriptor.ContentFd);
                    close(pDescriptor.HeaderFd);
                    pDescriptor._isOpen=false;
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_MODEINVALID,
                                            Severity_Error,
                                            "file %s is of type <%s> and cannot be opened as type <%s> for upgrade whitout being opened in mode ZRF_Exclusive ",
                                            pDescriptor.URIContent.toString(),
                                            decode_ZFile_type (pDescriptor.ZHeader.FileType),
                                            decode_ZFile_type (pFileType));
                    return  ZS_MODEINVALID;
                    }
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_FILETYPEWARN,
                                        Severity_Warning,
                                        "file %s has been opened but given type is <%s> while expected <%s>",
                                        pDescriptor.URIContent.toString(),
                                        decode_ZFile_type (pFileType),
                                        decode_ZFile_type (pDescriptor.ZHeader.FileType));
                wSt=ZS_FILETYPEWARN;// OK Open but emit a Warning
                break;
                }
            // all other cases are errored
            close (pDescriptor.ContentFd);
            close(pDescriptor.HeaderFd);
            pDescriptor._isOpen=false;
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                    pDescriptor.URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (pDescriptor.ZHeader.FileType));
            return  ZS_BADFILETYPE;

            }// case ZFT_ZRandomFile

        case ZFT_ZMasterFile :
            {
            if (pFileType==ZFT_ZMasterFile)
                                {
                                wSt=ZS_SUCCESS;
                                break; // everything in line
                                }
            if (pFileType==ZFT_ZRandomFile)
                {
                if ((pMode & ZRF_Write_Only)||(pMode & ZRF_Delete_Only)) // Only ZRF_Read_Only - delete and write mode excluded
                    {
                    close (pDescriptor.ContentFd);
                    close(pDescriptor.HeaderFd);
                    pDescriptor._isOpen=false;
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_MODEINVALID,
                                            Severity_Error,
                                            "file %s is of type <%s> and cannot be opened as type <%s> with mode<%s>",
                                            pDescriptor.URIContent.toString(),
                                            decode_ZFile_type (pDescriptor.ZHeader.FileType),
                                            decode_ZFile_type (pFileType),
                                            decode_ZRFMode(pMode));
                    return  ZS_MODEINVALID;
                    }
                // up to here file is opened as ZRF_Read_Only

                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_FILETYPEWARN,
                                        Severity_Warning,
                                        "file %s has been opened but given type is <%s> while expected <%s>",
                                        pDescriptor.URIContent.toString(),
                                        decode_ZFile_type (pFileType),
                                        decode_ZFile_type (pDescriptor.ZHeader.FileType));
                wSt=ZS_FILETYPEWARN; // OK Open but emit a Warning
                break;
                }
            // all other cases are errors
            close (pDescriptor.ContentFd);
            close(pDescriptor.HeaderFd);
            pDescriptor._isOpen=false;
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                    pDescriptor.URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (pDescriptor.ZHeader.FileType));
            return  ZS_BADFILETYPE;
            }// ZFT_ZMasterFile

        case ZFT_ZIndexFile :
            {
            if (pFileType==ZFT_ZIndexFile)
                                {
                                wSt=ZS_SUCCESS;
                                break; // everything in line
                                }
            if (pFileType==ZFT_ZRandomFile)
                {
                if ((pMode & ZRF_Write_Only)||(pMode & ZRF_Delete_Only)) // Only ZRF_Read_Only - delete and write mode excluded
                    {
                    close (pDescriptor.ContentFd);
                    close(pDescriptor.HeaderFd);
                    pDescriptor._isOpen=false;
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_MODEINVALID,
                                            Severity_Error,
                                            "file %s is of type <%s> and cannot be opened as type <%s> with mode<%s>",
                                            pDescriptor.URIContent.toString(),
                                            decode_ZFile_type (pDescriptor.ZHeader.FileType),
                                            decode_ZFile_type (pFileType),
                                            decode_ZRFMode(pMode));
                    return  ZS_MODEINVALID;
                    }
                // up to here file is opened as ZRF_Read_Only

                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_FILETYPEWARN,
                                        Severity_Warning,
                                        "file %s has been opened but given type is <%s> while expected <%s>",
                                        pDescriptor.URIContent.toString(),
                                        decode_ZFile_type (pFileType),
                                        decode_ZFile_type (pDescriptor.ZHeader.FileType));
                wSt=ZS_FILETYPEWARN; // OK Open but emit a Warning
                break;
                }
            // all other cases are errors
            close (pDescriptor.ContentFd);
            close(pDescriptor.HeaderFd);
            pDescriptor._isOpen=false;
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                    pDescriptor.URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (pDescriptor.ZHeader.FileType));
            return  ZS_BADFILETYPE;
            }// ZFT_ZIndexFile
    case ZFT_ZSMasterFile :
        {
        if (pFileType==ZFT_ZSMasterFile)
                            {
                            wSt=ZS_SUCCESS;
                            break; // everything in line
                            }
        if (pFileType==ZFT_ZRandomFile)
            {
            if ((pMode & ZRF_Write_Only)||(pMode & ZRF_Delete_Only)) // Only ZRF_Read_Only - delete and write mode excluded
                {
                close (pDescriptor.ContentFd);
                close(pDescriptor.HeaderFd);
                pDescriptor._isOpen=false;
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_MODEINVALID,
                                        Severity_Error,
                                        "file %s is of type <%s> and cannot be opened as type <%s> with mode<%s>",
                                        pDescriptor.URIContent.toString(),
                                        decode_ZFile_type (pDescriptor.ZHeader.FileType),
                                        decode_ZFile_type (pFileType),
                                        decode_ZRFMode(pMode));
                return  ZS_MODEINVALID;
                }
            // up to here file is opened as ZRF_Read_Only

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILETYPEWARN,
                                    Severity_Warning,
                                    "file %s has been opened but given type is <%s> while expected <%s>",
                                    pDescriptor.URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (pDescriptor.ZHeader.FileType));
            wSt=ZS_FILETYPEWARN; // OK Open but emit a Warning
            break;
            }
        // all other cases are errors
        close (pDescriptor.ContentFd);
        close(pDescriptor.HeaderFd);
        pDescriptor._isOpen=false;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_BADFILETYPE,
                                Severity_Error,
                                "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                pDescriptor.URIContent.toString(),
                                decode_ZFile_type (pFileType),
                                decode_ZFile_type (pDescriptor.ZHeader.FileType));
        return  ZS_BADFILETYPE;
        }// ZFT_ZSMasterFile
    case ZFT_ZSIndexFile :
        {
        if (pFileType==ZFT_ZSIndexFile)
                            {
                            wSt=ZS_SUCCESS;
                            break; // everything in line
                            }
        if (pFileType==ZFT_ZRandomFile)
            {
            if ((pMode & ZRF_Write_Only)||(pMode & ZRF_Delete_Only)) // Only ZRF_Read_Only - delete and write mode excluded
                {
                close (pDescriptor.ContentFd);
                close(pDescriptor.HeaderFd);
                pDescriptor._isOpen=false;
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_MODEINVALID,
                                        Severity_Error,
                                        "file %s is of type <%s> and cannot be opened as type <%s> with mode<%s>",
                                        pDescriptor.URIContent.toString(),
                                        decode_ZFile_type (pDescriptor.ZHeader.FileType),
                                        decode_ZFile_type (pFileType),
                                        decode_ZRFMode(pMode));
                return  ZS_MODEINVALID;
                }
            // up to here file is opened as ZRF_Read_Only

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILETYPEWARN,
                                    Severity_Warning,
                                    "file %s has been opened but given type is <%s> while expected <%s>",
                                    pDescriptor.URIContent.toString(),
                                    decode_ZFile_type (pFileType),
                                    decode_ZFile_type (pDescriptor.ZHeader.FileType));
            wSt=ZS_FILETYPEWARN; // OK Open but emit a Warning
            break;
            }
        // all other cases are errors
        close (pDescriptor.ContentFd);
        close(pDescriptor.HeaderFd);
        pDescriptor._isOpen=false;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_BADFILETYPE,
                                Severity_Error,
                                "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                pDescriptor.URIContent.toString(),
                                decode_ZFile_type (pFileType),
                                decode_ZFile_type (pDescriptor.ZHeader.FileType));
        return  ZS_BADFILETYPE;
        }// ZFT_ZSIndexFile
    default:
        {
        // all other cases are errors
        close (pDescriptor.ContentFd);
        close(pDescriptor.HeaderFd);
        pDescriptor._isOpen=false;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_BADFILETYPE,
                                Severity_Error,
                                "file %s cannot be opened : invalid given type <%s> while expected <%s>",
                                pDescriptor.URIContent.toString(),
                                decode_ZFile_type (pFileType),
                                decode_ZFile_type (pDescriptor.ZHeader.FileType));
        return  ZS_BADFILETYPE;
        }

    }//switch

//-----------------------------End File type test------------------------------------

    pDescriptor._isOpen = true ;
    pDescriptor.Mode = pMode;

    if (pMode & ZRF_Exclusive)
            {
            wSt=_lockFile(pDescriptor,ZRF_Exclusive,true);
//            pDescriptor.ZHeader.Lock = ZRF_Exclusive ;
            }

//=======Open pool management=========================

    ZRFPool->addOpenFile(&pDescriptor);

    return  wSt;  // do not write file descriptor
//    return(_writeFileDescriptor(pDescriptor,true));  // force to write file descriptor whatever open mode is (pForceRead = true)
}//_open


ZStatus
ZRandomFile::_close(ZFileDescriptor &pDescriptor)
{

ZStatus wSt;


    if (!pDescriptor._isOpen)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                " File is not open while requesting to close. File <%s>",
                                pDescriptor.URIContent.toString());
        return ZS_INVOP;
        }

    pDescriptor.ZHeader.Lock = ZLock_Nolock;
    pDescriptor.Mode = ZRF_Nothing;

//    _unlockAll(pDescriptor,false);

    wSt=_writeFullFileHeader(pDescriptor,true); // write full header and force write pForceWrite=true whatever the open mode is
    if (wSt!=ZS_SUCCESS)
                return (wSt);
    close(pDescriptor.ContentFd);
    close(pDescriptor.HeaderFd);
    pDescriptor._isOpen = false;

//=====================File pool management======================

    ZRFPool->removeFileByFd(pDescriptor.ContentFd);

    pDescriptor.clearPartial();
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
                                ZBAT->lastIdx());
         return ZS_OUTBOUNDLOW;
         }
    if (pRank>ZBAT->lastIdx())
        {
        ZException.setMessage(pModule,
                                ZS_OUTBOUNDHIGH,
                                Severity_Error,
                                " invalid block rank number <%d> . Boundaries are [0,<%ld>]",
                                pRank,
                                ZBAT->lastIdx());
         return ZS_OUTBOUNDHIGH;
         }
   if (ZBAT->Tab[pRank].State==ZBS_Allocated)
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
ZStatus
ZRandomFile::_lockFile (ZFileDescriptor &pDescriptor,const  zlockmask_type pLock, bool pForceWrite)
{


ZStatus wSt;


    if ((!pForceWrite)&&(pDescriptor.Mode&ZRF_Exclusive))
        {
        pDescriptor.ZHeader.Lock = pLock;
        pDescriptor.ZHeader.LockOwner = pDescriptor.Pid;
        return  ZS_SUCCESS;
        }

lockPack_struct wLockInfos;

//    wSt= _getFileHeader(pDescriptor,pForceWrite); // pForceWrite = pForceRead
    wSt=_readFileLock(pDescriptor,wLockInfos);
    if (wSt!=ZS_SUCCESS)
                    { return  wSt;}
    pDescriptor.ZHeader.Lock = wLockInfos.Lock;
    pDescriptor.ZHeader.LockOwner = wLockInfos.LockOwner;

    if (pDescriptor.ZHeader.Lock != ZLock_Nolock)
    {
        if (pDescriptor.ZHeader.LockOwner!=pDescriptor.Pid)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_LOCKED,
                                Severity_Error,
                                " File is already locked by another pid : <%ld> while trying to lock it <%s> lock mask is <%s>",
                                pDescriptor.ZHeader.LockOwner,
                                pDescriptor.URIHeader.toString(),
                                  decode_ZLockMask (pDescriptor.ZHeader.Lock).toChar());
        return  ZS_LOCKED;
        }
    }
    pDescriptor.ZHeader.Lock = wLockInfos.Lock = pLock;
    pDescriptor.ZHeader.LockOwner=wLockInfos.LockOwner= pDescriptor.Pid;


    return  _writeFileLock(pDescriptor,wLockInfos);
}// _lockFile


ZStatus
ZRandomFile::_unlockFile (ZFileDescriptor &pDescriptor,bool pForceWrite)
{

ZStatus wSt;

    printf ("%s\n",_GET_FUNCTION_NAME_);

    if ((!pForceWrite)&&(pDescriptor.Mode&ZRF_Exclusive))
        {
        pDescriptor.ZHeader.Lock = ZLock_Nolock;
        pDescriptor.ZHeader.LockOwner =(pid_t)0;
        return  ZS_SUCCESS;
        }

lockPack_struct wLockInfos;

//    wSt= _getFileHeader(pDescriptor,pForceWrite); // pForceWrite = pForceRead
    wSt=_readFileLock(pDescriptor,wLockInfos);
    if (wSt!=ZS_SUCCESS)
                {   return  wSt;}
    pDescriptor.ZHeader.Lock = wLockInfos.Lock;
    pDescriptor.ZHeader.LockOwner = wLockInfos.LockOwner;

    if (pDescriptor.ZHeader.Lock != ZLock_Nolock)
    {
        if (pDescriptor.ZHeader.LockOwner!=pDescriptor.Pid)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_LOCKED,
                                Severity_Error,
                                " File is already locked by another pid : <%ld> while trying to lock it <%s> lock mask is <%s>",
                                pDescriptor.ZHeader.LockOwner,
                                pDescriptor.URIHeader.toString(),
                                decode_ZLockMask (pDescriptor.ZHeader.Lock).toChar());
        return  ZS_LOCKED;
        }
    }
    pDescriptor.ZHeader.Lock = wLockInfos.Lock = ZLock_Nolock;
    pDescriptor.ZHeader.LockOwner=wLockInfos.LockOwner= (pid_t)0;


    return  _writeFileLock(pDescriptor,wLockInfos);

}//_unlockFile

/**
 * @brief ZRandomFile::_isLocked tests whether the whole file is locked or not
 *  returns the lock mask for the whole file in pLock
 * re
 * @param[inout] pDescriptor  file descriptor. Nota Bene ZHeader is updated from file.
 * @return true if file is locked - false if file is not locked
 */
bool
ZRandomFile::_isFileLocked (ZFileDescriptor &pDescriptor,bool pForceRead)
{
ZStatus wSt;
    if (pDescriptor.Mode!=ZRF_Exclusive)
    {
    wSt= _getFileHeader(pDescriptor,pForceRead);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" while testing lock on file ");
                ZException.exit_abort();
                }
    }
    return (!(pDescriptor.ZHeader.Lock==ZLock_Nolock));
}//_isLocked
//----------End Lock sequence------------------------------------------

//! @}    ZRandomFile

//----------------------XML Routines----------------------------------------

//------------XML ------------------------------------------
/**
 * @brief ZMasterFile::zwriteXML_FileHeader Static function : Generates the xml definition for a ZMasterFile's header given by it path name pFilePath
 * @note the ZMasterFile is opened for read only ZRF_Read_Only then closed.
 *
 * @param[in] pFilePath points to a valid file to generate the definition from
 * @param[in] pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZRandomFile::zwriteXML_FileHeader(const char* pFilePath,FILE *pOutput)
{
ZRandomFile wZMF;
ZStatus wSt;
FILE* wOutput=pOutput;
    wSt = wZMF.zopen(pFilePath,ZRF_Read_Only);
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();

    if (pOutput==nullptr)
    {
        uriString uriOutput;
        utfdescString OutPath;
        utfdescString OutBase;
        uriOutput= (const utf8_t*)pFilePath;
        OutBase = uriOutput.getBasename();
        OutPath = uriOutput.getDirectoryPath();
        OutPath += OutBase;
        OutPath += (const utf8_t*)".xml";
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


    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=\"2.00\">\n");
    fprintf (wOutput,
             "<file>\n"
             " <filetype>ZRandomFile</filetype>\n");
    wZMF._writeXML_ZRandomFileHeader(wZMF.ZDescriptor,wOutput);
    fprintf (wOutput,
             "</file>\n");
    wZMF.zclose();
    if (pOutput==nullptr)
                fclose(wOutput);
}  // static zwriteXML_FileHeader


/**
 * @brief ZRandomFile::zwriteXML_FileHeader Generates the xml definition for current opened ZRandomFile's header
 * @param pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZRandomFile::zwriteXML_FileHeader (FILE *pOutput)
{
    FILE* wOutput=pOutput;

        if (pOutput==nullptr)
        {
            uriString uriOutput;
            utfdescString OutPath;
            utfdescString OutBase;

            uriOutput= ZDescriptor.URIContent.toString();
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
        fprintf (wOutput,
                 "<?xml version='1.0' encoding='UTF-8'?>\n"
                 "<zicm version=\"2.00\">\n");
        fprintf (wOutput,
                 "<file>\n"
                 " <filetype>ZRandomFile</filetype>\n");
        _writeXML_ZRandomFileHeader(ZDescriptor,wOutput);
        fprintf (wOutput,
                 "</file>\n");

        if (pOutput==nullptr)
                    fclose(wOutput);
        return;
}

/**
 * @brief ZRandomFile::_writeXML_FileHeader Generates the xml definition for current opened ZRandomFile's ZFileDescriptor (header)
 * @param pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZRandomFile::_writeXML_ZRandomFileHeader (ZFileDescriptor &pDescriptor,FILE *pOutput)
{


    fprintf (pOutput,
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
             "      </ZHeaderControlBlock>\n",

             pDescriptor.URIContent.toString(),
             pDescriptor.URIHeader.toString(),
             pDescriptor.URIDirectoryPath.toString(),

             decode_ZFile_type(pDescriptor.ZHeader.FileType),
             __ZRF_VERSION__,
             //pDescriptor.ZHeader.ZRFVersion,
             pDescriptor.ZHeader.OffsetFCB,
             pDescriptor.ZHeader.OffsetReserved,
             pDescriptor.ZHeader.SizeReserved
             );
    fprintf (pOutput,
             "   <ZFileControlBlock>\n"
             "      <AllocatedBlocks>%ld</AllocatedBlocks> <!-- not modifiable -->\n"
             "      <BlockExtentQuota>%ld</BlockExtentQuota>\n"
             "      <InitialSize>%ld</InitialSize>\n"
             "      <BlockTargetSize>%ld</BlockTargetSize>\n"
             "      <HighwaterMarking>%s</HighwaterMarking>\n"
             "      <GrabFreeSpace>%s</GrabFreeSpace>\n"
             "   </ZFileControlBlock>\n",

             pDescriptor.ZFCB->AllocatedBlocks,
             pDescriptor.ZFCB->BlockExtentQuota,
             pDescriptor.ZFCB->InitialSize,
             pDescriptor.ZFCB->BlockTargetSize,
             pDescriptor.ZFCB->HighwaterMarking?"true":"false",
             pDescriptor.ZFCB->GrabFreeSpace?"true":"false"
             );
    fprintf (pOutput,
             "  </ZFileDescriptor>\n");

} //_writeXML_Header


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
    if (!ZDescriptor._isOpen)
            {
            fprintf(pOutput, "ZRFPMSStats:: File is closed- no data available\n" );
            return;
            }
    fprintf (pOutput,
             "\nZRandomFile Activity Report on file <%s>\n"
             "                 session summary\n"
             "________________________________________________\n"
             "   Header file <%s>\n"
             "   File open mode         %s\n"
             "________________________________________________\n",
             ZDescriptor.URIContent.toString(),
             ZDescriptor.URIHeader.toString(),
             decode_ZRFMode( ZDescriptor.Mode)
             );

    ZDescriptor.ZPMS.reportDetails(pOutput);
    return;

}// _ZRFPMSStats

/**
 * @brief ZRandomFile::ZRFstat Provides statistics on how information is stored for a given ZRandomFile.

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
ZFileDescriptor wDescriptor;
ZStatus wSt;

    wSt= wDescriptor.setPath((uriString &)pFilename);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    wSt=_open(wDescriptor, ZRF_Read_Only,ZFT_ZRandomFile);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    wSt=_getFileControlBlock(wDescriptor,true);
    if (wSt!=ZS_SUCCESS)
                {
                _close (wDescriptor);
                return wSt;
                }

    double wD = 0;
    double wD1 = 0;
    double wM = 0;

    zsize_type wMin=wDescriptor.ZBAT->Tab[0].BlockSize;
    zsize_type wMax=wDescriptor.ZBAT->Tab[0].BlockSize;

    for (long wi =0; wi < wDescriptor.ZBAT->size();wi++)
            {
            wM += (double) wDescriptor.ZBAT->Tab[wi].BlockSize;
            if (wDescriptor.ZBAT->Tab[wi].BlockSize>wMax)
                            wMax=wDescriptor.ZBAT->Tab[wi].BlockSize;
            if (wDescriptor.ZBAT->Tab[wi].BlockSize<wMin)
                            wMin=wDescriptor.ZBAT->Tab[wi].BlockSize;
            }// for
    if (wDescriptor.ZBAT->size()>0)
            wM= wM/(double)wDescriptor.ZBAT->size();

    wDescriptor.ZFCB->MinSize = wMin;
    wDescriptor.ZFCB->MaxSize = wMax;


    // standard deviation
    for (long wi =0; wi < wDescriptor.ZBAT->size();wi++)
            {
            wD1 = (((double) wDescriptor.ZBAT->Tab[wi].BlockSize )- wM) ;
            if (wD1<0)
                    wD += - wD1 ;
                else
                    wD += wD1;
            }
    if (wDescriptor.ZBAT->size()>0)
                 wD = wD/wDescriptor.ZBAT->size();

    fprintf (pOutput,
             "Statistics on ZRandom File <%s>\n"
             "________________________________________________\n"
             "   Header file <%s>\n"
             "   Content file size      %10ld\n"
             "   Header file size       %10ld\n"
             "   File open mode         %s\n"
             "________________________________________________\n"
             "      Allocated blocks    %10lld\n"
             "      Allocated size      %10lld\n"
             "      used blocks         %10lld\n"
             "      free pool           %10lld\n"
             "       max block size     %10ld\n"
             "       min block size     %10ld\n"
             "       average block size %10ld\n"
             "       standard deviation %10ld\n"
             "________________________________________________\n",
             wDescriptor.URIContent.toString(),
             wDescriptor.URIHeader.toString(),
             wDescriptor.URIContent.getFileSize(),
             wDescriptor.URIHeader.getFileSize(),
             decode_ZRFMode( wDescriptor.Mode),
             wDescriptor.ZFCB->AllocatedBlocks,
             wDescriptor.ZFCB->AllocatedSize,
             wDescriptor.ZBAT->size(),
             wDescriptor.ZFBT->size(),
             wMax,
             wMin,
             (long long)wM,
             (long long )wD
             );

    return _close (wDescriptor);

}// _ZRFstat


/**
 * @brief ZRandomFile::zfullDump reports the full content of
 * @param pColumn   the number of bytes used to present ascii and hexa dump @see ZDataBuffer::Dump
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZRandomFile::zfullDump(const int pColumn,FILE* pOutput)       //! full dump current ZRF
{
    zfullDump(ZDescriptor.URIContent,pColumn,pOutput);
    return;
}
/**
 * @brief ZRandomFile::zheaderDump reports the header data of the currently opened ZRandomFile
 *
 * @param pOutput   a FILE* pointer where the reporting will be made. By default, set to stdout.
 */
void
ZRandomFile::zheaderDump(FILE* pOutput)                     //! dump current ZRF header
{
    zheaderDump(ZDescriptor.URIContent,pOutput);
    return;
}
/**
 * @brief ZRandomFile::zcontentDump reports the whole content of the current ZRandomFile
 *
 * @param pColumn   the number of bytes used to present ascii and hexa dump @see ZDataBuffer::Dump
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZRandomFile::zcontentDump(const int pColumn,FILE* pOutput)       //! dump current ZRF content
{
    zcontentDump(ZDescriptor.URIContent,pColumn,pOutput);
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
void ZRandomFile::zblockDump (uriString pURIContent,
                              const long pRank,
                              const int pColumn,
                              FILE* pOutput)
{
ZStatus wSt;
ZFileDescriptor wDescriptor;

    wSt=wDescriptor.setPath(pURIContent);  //! generates also Header file structures within ZFileDescriptor

    wSt=_open(wDescriptor,ZRF_Read_Only,ZFT_ZRandomFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.exit_abort();
                }

    if ((wSt=_getFileControlBlock(wDescriptor,true))!=ZS_SUCCESS)
                {
                ZException.addToLast(" header file is <%s> for ZRF file <%s>",
                                            wDescriptor.URIHeader.toString(),
                                            wDescriptor.URIContent.toString());
                ZException.printUserMessage(pOutput);

                return ;
                }

    fprintf(pOutput,
            "        ***********************************************************\n"
            "        *  Block dump for file  %30s     *\n"
            "        ***********************************************************\n",
            wDescriptor.URIContent.toString());


ZDataBuffer wRulerHexa;
ZDataBuffer wRulerAscii;
ZBlock wBlock;
zaddress_type wAddress;

    int wColumn=rulerSetup (wRulerHexa, wRulerAscii,pColumn);

    wSt=_getByRank(wDescriptor,wBlock, pRank,wAddress);     // get first block of the file
    fprintf (pOutput,
             "\n                 == Block number %4ld address %lld ==\n",
             pRank,
             wAddress);

        _dumpOneDataBlock(wBlock,wRulerHexa,wRulerAscii,wColumn,pOutput);

    if (wSt!=ZS_SUCCESS)
        {
        ZException.printUserMessage(pOutput);
        }

    _close(wDescriptor);
    return;
} // zblockDump


/**
 * @brief zsurfaceScan  Scans the physical file surface for a whole ZRandomFile corresponding to pURIContent

    Scans the file surface in physical order (not in logical block order given from the pools),
    physical block after physical block and gives the status  (Free or Used) and size of the encountered blocks.

    Detects whether there are holes between blocks.

   @note This is a static method.
  It opens for reading and close appropriately the given ZRandomFile.
  If this method is used from an object that holds an already opened ZRandomFile, then it reopens it for reading.
  Using it from an active object instance might induce lock problems.

 * @param pURIContent uriString giving the ZRandomFile path to scan
 * @param pOutput   a FILE* pointer where the reporting will be made. By default, set to stdout.
 */

void
ZRandomFile::zsurfaceScan(uriString pURIContent, FILE *pOutput) //! dump another ZRF
{


ZStatus wSt;

ZFileDescriptor wDescriptor;

    wSt=wDescriptor.setPath(pURIContent);  //! generates also Header file structures within ZFileDescriptor
    wSt=_open(wDescriptor,ZRF_Read_Only,ZFT_ZRandomFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.exit_abort();
                }

    if ((wSt=_getFileControlBlock(wDescriptor,true))!=ZS_SUCCESS)
                {
                ZException.addToLast("Error while getting header file %s for ZRF file %s",
                                            wDescriptor.URIHeader.toString(),
                                            wDescriptor.URIContent.toString());
                ZException.exit_abort();
                }

    fprintf(pOutput,
            "        **************ZRandomFile Surface Analysis Utility******************\n"
            "         File %s\n"
            "        ********************************************************************\n",
            wDescriptor.URIContent.toString());

    _headerDump(wDescriptor,pOutput);
    _surfaceScan(wDescriptor,pOutput);
    _close(wDescriptor);
    return ;

}//zsurfaceScan

//! @cond Development

void
ZRandomFile::_surfaceScan(ZFileDescriptor &pDescriptor,FILE* pOutput)
 {


ZStatus wSt;

fprintf (pOutput,
         "\n___________________________Content Data blocks in physical order___________________________________\n");

wSt=ZS_SUCCESS;

ZBlockHeader wBlockHeader;

zaddress_type wOldAddress = 0;
zsize_type    wOldSize = 0;
zaddress_type wAddressNext = 0;
zaddress_type wTheoricAddress = 0;

    fprintf (pOutput,
             "Physical address    |   State       | Block size         | User content size  | Theorical next Add\n"
             "____________________|_______________|____________________|____________________|____________________\n"
             );

    wSt=_seek(pDescriptor,0L);
    while (true)
        {
        wSt=_searchNextPhysicalBlock(pDescriptor,wAddressNext,wAddressNext,wBlockHeader);     //! get first physical block of the file
        if (wSt!=ZS_FOUND)
                        break;

        if (wBlockHeader.State==ZBS_Deleted)
                        {
            fprintf(pOutput,
                    "--%18lld|%15s|%20lld|%20lld|%20s\n",
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

    while (wSt==ZS_FOUND)
        {
        wTheoricAddress = wOldAddress+wOldSize;
        if (wAddressNext!=wTheoricAddress)
                    {
                    fprintf(pOutput,
                            "%20lld|%15s|%20lld|                    |\n",
                            wTheoricAddress,
                            "--- <Hole> ---",
                            wAddressNext-wTheoricAddress
                            );
                    }
        fprintf(pOutput,
                "%20lld|%15s|%20lld|%20lld|%20lld\n",
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
            wAddressNext += sizeof(ZBlockHeader_Export);   //! just to grab deleted Blocks

        while (true)
            {
            wSt=_searchNextPhysicalBlock(pDescriptor,wAddressNext,wAddressNext,wBlockHeader);     //! get first physical block of the file
            if (wSt!=ZS_FOUND)
                        break;

            if (wBlockHeader.State==ZBS_Deleted)
                            {
                fprintf(pOutput,
                        "--%18lld|%15s|%20lld|%20lld|%20s\n",
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
        }// while ZS_FOUND

    if (wSt==ZS_EOF)
        {

        fprintf(pOutput,
                "\n       **** End of File reached at address <%lld> *****\n",
                wAddressNext);
        }
        else
        {
        ZException.printUserMessage(pOutput);
        }
    return ;
} // _physicalcontentDump

//! @endcond


/**
 * @brief ZRandomFile::zfullDump     logical full dump of a whole ZRandomFile given by its uri (pURIContent).
 *
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
ZRandomFile::zfullDump(uriString pURIContent,const int pColumn,FILE* pOutput) // dump another ZRF
{
ZStatus wSt;
ZFileDescriptor wDescriptor;


    wSt=wDescriptor.setPath(pURIContent);  //! generates also Header file structures within ZFileDescriptor

    wSt=_open(wDescriptor,ZRF_Read_Only,ZFT_ZRandomFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.printUserMessage(stderr);
                abort();
                }

    if ((wSt=_getFileControlBlock(wDescriptor,true))!=ZS_SUCCESS)
                {
                ZException.addToLast("Error while getting header %s for ZRF file %s",
                                            wDescriptor.URIHeader.toString(),
                                            wDescriptor.URIContent.toString());
                ZException.printUserMessage(pOutput);
                return ;
                }

    fprintf(pOutput,
            "        ***********************************************************\n"
            "        * Logical Full Dump of file %30s *\n"
            "        ***********************************************************\n",
            wDescriptor.URIContent.toString());

    _headerDump(wDescriptor,pOutput);
    _fullcontentDump(wDescriptor,pColumn,pOutput);
    _close(wDescriptor);
    return;

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
ZRandomFile::zheaderDump(uriString &pURIContent, FILE* pOutput)
{

ZStatus wSt;

ZFileDescriptor wDescriptor;


    wSt=wDescriptor.setPath(pURIContent);  //! generates also Header file structures within ZFileDescriptor

    wSt=_open(wDescriptor,ZRF_Read_Only,ZFT_ZRandomFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.exit_abort();
                }

    if ((wSt=_getFullFileHeader(wDescriptor))!=ZS_SUCCESS)
                {
                ZException.addToLast("Error while getting header %s for ZRF file %s",
                                            wDescriptor.URIHeader.toString(),
                                            wDescriptor.URIContent.toString());
                ZException.exit_abort();
                }

    fprintf(pOutput,
            "        *******************************************************\n"
            "        *  Dump of Header-file %30s *\n"
            "        *******************************************************\n",
            wDescriptor.URIContent.toString());

    _headerDump(wDescriptor,pOutput);
    _close(wDescriptor);
    return ;

}//zheaderDump

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
ZRandomFile::zcontentDump(uriString pURIContent,int pColumn,FILE* pOutput)
{
ZStatus wSt;
ZFileControlBlock wFCB;
ZArray<ZBlockDescriptor> wZBAT;
ZArray<ZBlockMin_struct> wZFBT;
ZFileDescriptor wDescriptor;


    wSt=wDescriptor.setPath(pURIContent);  //! generates also Header file structures within ZFileDescriptor

    wSt=_open(wDescriptor,ZRF_Read_Only,ZFT_ZRandomFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.printUserMessage(stderr);
                abort();
                }

    if ((wSt=_getFullFileHeader(wDescriptor))!=ZS_SUCCESS)
                {
                ZException.addToLast("Error while getting header %s for ZRF file %s",
                                            wDescriptor.URIHeader.toString(),
                                            wDescriptor.URIContent.toString());
                ZException.printUserMessage(pOutput);
                return ;
                }

    fprintf(pOutput,
            "        ***********ZRandomFile Content Dump Utility******************\n"
            "        File %s\n"
            "        *************************************************************\n",
            wDescriptor.URIContent.toString());

    _fullcontentDump(wDescriptor,pColumn,pOutput);
    _close(wDescriptor);
    return;

}//zcontentDump



/**
 * @brief ZRandomFile::_fullcontentDump dump the full content of current ZRandomFile given by pDescriptor
 * ZRF must be opened for reading
 *
 * @param pDescriptor ZFileDescriptor of the ZRandonFile to dump
 * @param pColumn   Number of bytes displayed both in ascii and hexadecimal per row
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZRandomFile::_fullcontentDump(ZFileDescriptor &pDescriptor,
                          const int pColumn,
                          FILE* pOutput)
 {
ZStatus wSt;

fprintf (pOutput,
         "\n....................Content Data blocks..........................\n");

wSt=ZS_SUCCESS;

ZBlock          wBlock;
ZDataBuffer     wRulerHexa;
ZDataBuffer     wRulerAscii;
zaddress_type   wAddress;
zrank_type      wRank;


    int wColumn=rulerSetup (wRulerHexa, wRulerAscii,pColumn);
    zaddress_type wBN = 0;

    wRank = 0L;
    wSt=_getByRank(pDescriptor,wBlock,wRank,wAddress);     // get first block of the file
    while (wSt==ZS_SUCCESS)
        {
    fprintf (pOutput,
             "\n                 == Block number %4ld - Physical address %lld ==\n",
             wRank,
             wAddress);
//             pDescriptor.getCurrentRank(),
//             pDescriptor.getCurrentPhysicalAddress());

        _dumpOneDataBlock(wBlock,wRulerHexa,wRulerAscii,wColumn,pOutput);

        wSt=_getNext(pDescriptor, wBlock,wRank,wAddress);
        }// while

    if (wSt==ZS_EOF)
        {
        fprintf(pOutput,
                "\n       ***** End of File reached at block %d *****\n",
                wBN);
        }
        else
        {
        ZException.printUserMessage(pOutput);
        }
    return;
} // _contentDump


//! @cond Development

/**
 * @brief ZRandomFile::_dumpOneDataBlock dumps one given block to pOutput
 * @param pBlock    Block content to dump
 * @param pRulerHexa    Ruler for Hexadecimal
 * @param pRulerAscii   Ruler for Ascii
 * @param pColumn   Number of bytes displayed both in ascii and hexadecimal per row
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZRandomFile::_dumpOneDataBlock(ZBlock &pBlock,ZDataBuffer &pRulerHexa, ZDataBuffer &pRulerAscii,const int pColumn,FILE *pOutput)
{
    ZDataBuffer wLineChar;
    ZDataBuffer wLineHexa;

    fprintf(pOutput,
            " State <%6s> Block size <%10lld> User Content size <%10ld>\n",
            decode_ZBS( pBlock.State),
            pBlock.BlockSize,
            pBlock.DataSize());
    fprintf (pOutput,
             "Offset  %s  %s\n",
             pRulerHexa.DataChar,
             pRulerAscii.DataChar);

    long wOffset=0;
    int wL=0;
    while ((wOffset+pColumn)<pBlock.DataSize())
            {
            pBlock.Content.dumpHexa(wOffset,pColumn,wLineHexa,wLineChar);
            fprintf(pOutput,"%6d |%s |%s|\n",wL,wLineHexa.DataChar,wLineChar.DataChar);
            wL+=pColumn;
            wOffset+=(pColumn);
            }


    char wFormat [50];

    sprintf (wFormat,"#6d |#-%ds |#-%ds|\n",pColumn*3,pColumn);
    wFormat[0]='%';
    for (int wi=0;wFormat [wi]!='\0';wi++)
                if (wFormat[wi]=='#')
                            wFormat[wi]='%';

    pBlock.Content.dumpHexa(wOffset,(pBlock.Content.Size-wOffset),wLineHexa,wLineChar);

    fprintf(pOutput,wFormat,wL,wLineHexa.DataChar,wLineChar.DataChar);
    return;
} //_dumpOneDataBlock

//! @endcond

void
ZRandomFile::_headerDump(ZFileDescriptor &pDescriptor,
                         FILE* pOutput)
{
ZStatus wSt;
ZHeaderControlBlock_Export wHeaderExp;
    wSt=_getFileHeader_Export(pDescriptor,
                               &wHeaderExp);
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
    fprintf(pOutput,
    " Header file name     <%s>\n"
    " Header file block \n"
    " Identification       %s\n"
    " Offset to FCB        %10lld\n"
    " Reserved block size  %10lld\n"
    " Version              %10ld\n",
    pDescriptor.URIHeader.toString(),
    decode_ZBID(  wHeaderExp.BlockID),
    reverseByteOrder_Conditional<zaddress_type>(wHeaderExp.OffsetFCB),
    reverseByteOrder_Conditional<zsize_type>(wHeaderExp.SizeReserved),
    reverseByteOrder_Conditional<unsigned long>( wHeaderExp.ZRFVersion));



        fprintf(pOutput,

        " File Control Block\n"
        " Start of data        %10lld\n"
        " File Allocated size  %10lld\n"
        " File Used size       %10lld\n"
        " Allocated blocks     %10ld\n"
        " Block extent quota   %10ld\n"
        " Used blocks          %10ld\n"
        " Free blocks          %10ld\n"
        " Deleted blocks       %10ld\n"
        " Block target  size   %10ld\n"
        " Highwater Marking    %10s\n"
        " GrabFreeSpace        %10s\n",

        pDescriptor.ZFCB->StartOfData,
        pDescriptor.getAllocatedSize(),
        pDescriptor.ZFCB->UsedSize,
        pDescriptor.ZFCB->AllocatedBlocks,
        pDescriptor.ZFCB->BlockExtentQuota,
        pDescriptor.ZBAT->size(),
        pDescriptor.ZFBT->size(),
        pDescriptor.ZDBT->size(),
        pDescriptor.ZFCB->BlockTargetSize,
        pDescriptor.ZFCB->HighwaterMarking?"On":"Off",
        pDescriptor.ZFCB->GrabFreeSpace?"On":"Off");

        fprintf(pOutput,
        " Block Access Table (ZBAT) - Active blocks are stored here\n"
        "------+------------+------+--------------------+--------------------+\n"
        " Rank |      State |      |    Address         |          Block size|\n"
        "------+------------+------+--------------------+--------------------+\n");

        for (long wi=0; wi<pDescriptor.ZBAT->size();wi++)
        fprintf(pOutput,
        "%5ld |%12s|%6s|%20lld|%20lld|\n",
        wi,
        decode_ZBS(pDescriptor.ZBAT->Tab[wi].State),
        " ",
        pDescriptor.ZBAT->Tab[wi].Address,
        pDescriptor.ZBAT->Tab[wi].BlockSize );
 /*       fprintf(pOutput,
        "------+------------+------+--------------------+--------------------+\n");
        fprintf(pOutput,
        " Free Blocks Pool (ZFBT)\n"
        "------+--------------------+--------------------+\n"
        " Rank |    Address         |          Block size|\n"
        "------+--------------------+--------------------+\n");

        for (long wi=0; wi<pDescriptor.ZFBT->size();wi++)
        fprintf(pOutput,
        "%5ld |%20lld|%20lld|\n",
        wi,
        pDescriptor.ZFBT->Tab[wi].Address,
        pDescriptor.ZFBT->Tab[wi].BlockSize);
*/
        if (pDescriptor.ZBAT->size()>0)
        fprintf(pOutput,
                "------+------------+------+--------------------+--------------------+\n");
        fprintf(pOutput,
        " Free Blocks Pool(ZFBT) Available blocks are stored here\n"
        "------+------------+------+--------------------+--------------------+\n"
        " Rank |      State |      |    Address         |          Block size|\n"
        "------+------------+------+--------------------+--------------------+\n");

        for (long wi=0; wi<pDescriptor.ZFBT->size();wi++)
        fprintf(pOutput,
        "%5ld |%12s|%6s|%20lld|%20lld|\n",
        wi,
        decode_ZBS(pDescriptor.ZFBT->Tab[wi].State),
        " ",
        pDescriptor.ZFBT->Tab[wi].Address,
        pDescriptor.ZFBT->Tab[wi].BlockSize );

        if (pDescriptor.ZFBT->size()>0)
        fprintf(pOutput,
                "------+------------+------+--------------------+--------------------+\n");
        fprintf(pOutput,
        " Deleted Blocks Pool(ZDBT) - Recovery Pool\n"
        "------+------------+------+--------------------+--------------------+\n"
        " Rank |      State |      |    Address         |          Block size|\n"
        "------+------------+------+--------------------+--------------------+\n");

        for (long wi=0; wi<pDescriptor.ZDBT->size();wi++)
        fprintf(pOutput,
        "%5ld |%12s|%6s|%20lld|%20lld|\n",
        wi,
        decode_ZBS(pDescriptor.ZDBT->Tab[wi].State),
        " ",
        pDescriptor.ZDBT->Tab[wi].Address,
        pDescriptor.ZDBT->Tab[wi].BlockSize );

        if (pDescriptor.ZDBT->size()>0)
        fprintf(pOutput,
                "------+------------+------+--------------------+--------------------+\n");;

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
ZRandomFile::_moveBlock (ZFileDescriptor &pDescriptor,
                         const zaddress_type pFrom,
                         const zaddress_type pTo,
                         bool pCreateZFBTEntry)
{


ZStatus wSt;
ZBlock wBlock;
ZBlockDescriptor wBD;
    wSt=_readBlockAt(pDescriptor,wBlock,pFrom); // read the block to move
    if (wSt!=ZS_SUCCESS)
            { return  wSt;}

    wBD = wBlock;
    wBD.Address = pFrom;
    if (__ZRFVERBOSE__)
            fprintf(stdout,
                ".... moving Block. Former block Address %lld Size %lld",
                pFrom,
                wBD.BlockSize);
    if (pCreateZFBTEntry)
                {

                wBD.State = ZBS_Free;
                pDescriptor.ZFBT->push(wBD);
                if (__ZRFVERBOSE__)
                    fprintf (stdout,
                         " put to ZFBT pool. Pool rank %ld\n ",
                         pDescriptor.ZFBT->lastIdx());
                }
            else
                {
                wBD.State = ZBS_Deleted ;
                if (__ZRFVERBOSE__)
                    fprintf (stdout,
                         " deleted (ZBS_Deleted)\n");
                }
    wSt=_writeBlockHeader(pDescriptor,wBD,pFrom); // mark it as deleted
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

    return  _writeBlockAt(pDescriptor,wBlock,pTo);  // move it to its new address

}//_moveBlock

//! @endcond

/** @addtogroup ZRFUtilities
 * @{
  */

/**
 * @brief ZRandomFile::zheaderRebuild Rebuilds ZRandomFile header from an existing ZRandomFile content.
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


    fprintf (pOutput,
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
    wDescriptor.ContentFd = open(wDescriptor.URIContent.toCString_Strait(),O_RDONLY);       // open content file for read only
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
        _surfaceScan(wDescriptor,pOutput);

    mode_t wPosixMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    wDescriptor.HeaderFd = open(wDescriptor.URIHeader.toCString_Strait(),O_RDWR|O_CREAT|O_TRUNC,wPosixMode); // open header file read write

    ssize_t wHeaderSize = sizeof (ZHeaderControlBlock_Export) + sizeof (ZFileControlBlock_Export) ;

    wS=    posix_fallocate(wDescriptor.HeaderFd,0L,(off_t)wHeaderSize);
    if (wS!=0)
            {
            ZException.getErrno(wS,  // no errno is set by posix_fallocate : returned value is the status : ENOSPC means no space
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Severe,
                             " Error during creation of file <%s> cannot allocate disk space",
                             wDescriptor.URIHeader.toString());
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
                        wDescriptor.URIContent.toString(),
                        wAddress);
        return  (ZS_FILEPOSERR);
        }
    while (true)
        {
        wSt=_searchNextPhysicalBlock(wDescriptor,wAddress,wAddressNext,wBlockDescriptor);     //! get first physical block of the file
        if (wSt!=ZS_FOUND)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_EMPTYFILE,
                                        Severity_Severe,
                                        " No valid block has been found in content file %s",
                                        wDescriptor.URIContent.toString());
                goto error_zheaderRebuild;
                }
        wBlockDescriptor.Address = wAddressNext;


        // first block
        wDescriptor.ZFCB->StartOfData = wBlockDescriptor.Address;
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

                     wDescriptor.ZDBT->push(wBlockDescriptor);
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
            wDescriptor.ZBAT->push(wBlockDescriptor);

            wDescriptor.ZFCB->UsedSize += wBlockDescriptor.BlockSize ;  // sum used size and recompute block target size
            wDescriptor.ZFCB->BlockTargetSize = (long)((float)wDescriptor.ZFCB->UsedSize/(float)wDescriptor.ZBAT->size());

            wAddressNext += wBlockDescriptor.BlockSize;
        }
        else
        {
            if (wBlockDescriptor.State == ZBS_Free)
                    wDescriptor.ZFBT->push(wBlockDescriptor);
//                else
//                    wDescriptor.ZDBT->push(wBlockHeader);
            wAddressNext += sizeof(ZBlockHeader_Export);   // just to grab deleted Blocks
        }
        while (true)
            {
            wSt=_searchNextPhysicalBlock(wDescriptor,wAddressNext,wAddressNext,wBlockDescriptor); // get first physical block of the file
            if (wSt!=ZS_FOUND)
                        break;

            wBlockDescriptor.Address = wAddressNext;
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

                            wDescriptor.ZDBT->push(wBlockDescriptor);
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


    _writeFileHeader(wDescriptor,true);

    fprintf (pOutput,
             " *********Header rebuild results**************\n");

    _headerDump(wDescriptor,pOutput);

end_zheaderRebuild:
     _writeFileHeader(wDescriptor,true);
    close (wDescriptor.HeaderFd);
    close (wDescriptor.ContentFd);
    return  wSt;
error_zheaderRebuild:
    ZException.printUserMessage(pOutput);
    goto end_zheaderRebuild;
}// zheaderRebuild



//! @brief __CLONE_BASEEXTENSION__ Preprocessor symbol : base name extension for creating cloned file name
#define __CLONE_BASEEXTENSION__ "_clone"


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
ZRandomFile::zcloneFile(ZFileDescriptor &pDescriptor, const zsize_type pFreeSpace, FILE*pOutput)
{
ZStatus wSt;
long wi;
uriString wCloneContent;
zmode_type wMode = ZRF_Nothing ;
ZFile_type wFileType;
ZBlock wBlock;
zsize_type wFreeSpace ;
zaddress_type wAddress;
zaddress_type wAddress1;

ZFileDescriptor wDescriptor_Clone;

    if (pDescriptor._isOpen)
             {
             wMode=pDescriptor.getMode();
             wFileType = pDescriptor.ZHeader.FileType;
             _close(pDescriptor);
             }
     if ((wSt=_open(pDescriptor,ZRF_Exclusive|ZRF_Read_Only,wFileType))!=ZS_SUCCESS)
                             return wSt;

    wFreeSpace = (pFreeSpace>0)?pFreeSpace:pDescriptor.ZFCB->BlockTargetSize ;
// composing name
    utfdescString wDInfo;
     wCloneContent = pDescriptor.URIContent.getDirectoryPath().toCChar();
     wCloneContent.addConditionalDirectoryDelimiter();
     wCloneContent += pDescriptor.URIContent.getBasename().toCChar();
     wCloneContent += __CLONE_BASEEXTENSION__ ;
     wCloneContent +=".";
     wCloneContent += pDescriptor.URIContent.getFileExtension().toCChar();
     if (__ZRFVERBOSE__)
         fprintf (pOutput,
              " prepare cloning. Clone file uri is %s\n ...Setting path and duplicating header \n",
              wCloneContent.toString());
// setting up file header
     wSt = wDescriptor_Clone.setPath(wCloneContent);
     if (wSt!=ZS_SUCCESS)
                goto end_zcloneFile;
     // duplicating FCB parameters
     memmove(wDescriptor_Clone.ZFCB,pDescriptor.ZFCB, sizeof(ZFileControlBlock));

     wDescriptor_Clone.Mode = ZBS_Nothing;
     wDescriptor_Clone.ZHeader.Lock =ZLock_Nolock ;
     wDescriptor_Clone.ZFCB->UsedSize = 0;
    if (__ZRFVERBOSE__)
        fprintf (pOutput,
              " creating clone file. \n"
              "  source used size is %lld - requested free size is %lld <%s>\n"
              "  Total cloned file size will be %lld\n"
              "  Info: BlockTargetSize is %lld\n",
              pDescriptor.ZFCB->UsedSize,
              wFreeSpace,
              (pFreeSpace<0)?"Defaulted to BlockTargetSize":"Given value",
              pDescriptor.ZFCB->UsedSize+wFreeSpace,
              wDescriptor_Clone.ZFCB->BlockTargetSize);

     // create file and allocate UsedSize + BlockTargetSize file space
     wSt=_create(wDescriptor_Clone,pDescriptor.ZFCB->UsedSize+wFreeSpace,wFileType,false,false);
     if (wSt!=ZS_SUCCESS)
            goto end_zcloneFile;

     wSt=_open(wDescriptor_Clone,ZRF_Exclusive,wFileType);
     if (__ZRFVERBOSE__)
        fprintf (pOutput,
              "...Populating file with source file\n");
     for (wi=0;wi<pDescriptor.ZBAT->size();wi++)
         {
          wSt=_getByRank(pDescriptor,wBlock,wi,wAddress1);        // read block from file
          if (wSt!=ZS_SUCCESS)
                    goto error_zcloneFile;
          wSt=_add(wDescriptor_Clone,wBlock.Content,wAddress);  // write block to clone
          if (wSt!=ZS_SUCCESS)
                    goto error_zcloneFile;
         } // for
     if (__ZRFVERBOSE__)
         fprintf (pOutput,
              "...Populated %ld blocks withsource file\n",
              wi);



     _close(wDescriptor_Clone);
end_zcloneFile:
     _close(pDescriptor);
     _open(pDescriptor,wMode,wFileType);
     return wSt;

error_zcloneFile:
    _close(wDescriptor_Clone);
    ZException.printUserMessage();
    goto end_zcloneFile;
}// zcloneFile

/**
 * @brief ZRandomFile::zcloneFile overload method for zcloneFile using current openned file
  * @return
 */
ZStatus
ZRandomFile::zcloneFile(const zsize_type pFreeSpace, FILE*pOutput)
{
    return (zcloneFile(ZDescriptor,pFreeSpace,pOutput));
}

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
ZRandomFile::zextendFile(ZFileDescriptor &pDescriptor, const zsize_type pFreeSpace)
{
ZBlockDescriptor wHighBlock;
ZStatus wSt;
    wSt=_getExtent(pDescriptor,wHighBlock,pFreeSpace);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    wHighBlock.State = ZBS_Free;
    pDescriptor.ZFBT->push(wHighBlock);

    return (_writeFileDescriptor(pDescriptor,true));
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
ZRandomFile::ztruncateFile(ZFileDescriptor &pDescriptor, const zsize_type pFreeSpace, FILE*pOutput)
{


ZStatus wSt;
// search for highest address block within both pools
zaddress_type wHighAddressStart = -1;
ZBlockDescriptor wHighBlock;
long wHighBlockIndex;
zsize_type wFreeSpace = (pFreeSpace < 0)?pDescriptor.ZFCB->BlockTargetSize : pFreeSpace ;

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

    for (long wi=0 ;wi<(pDescriptor.ZBAT->size()-1);wi++)
        if (pDescriptor.ZBAT->Tab[wi].Address >= wHighAddressStart)
                    {
                                wHighAddressStart = pDescriptor.ZBAT->Tab[wi].Address;
                                wHighBlock= pDescriptor.ZBAT->Tab[wi];
                    }

    for (long wi=0;wi < pDescriptor.ZFBT->size();wi++)
    if (pDescriptor.ZFBT->Tab[wi].Address >= wHighAddressStart)
                        {
                        wHighAddressStart = pDescriptor.ZFBT->Tab[wi].Address;
                        wHighBlock= pDescriptor.ZFBT->Tab[wi];
                        wHighBlockIndex= wi;
                        }

   if (wHighBlock.State != ZBS_Free )
        {
        fprintf (pOutput,
                 "  Warning : highest block is at %lld (physical address) size %lld : this block is in use %s \n"
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
            fprintf (pOutput,
                     "  Warning : highest block is at %lld (physical address) size %lld : this block has state %s \n"
                     "  This block has a size less than requested size %lld. File is left as it is.\n"
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

    int wR=ftruncate(pDescriptor.ContentFd,(off_t)wNewSize);
    if (wR<0)
            {
            ZException.getFileError(pDescriptor.FContent,
                                      _GET_FUNCTION_NAME_,
                                      ZS_FILEERROR,
                                      Severity_Fatal,
                                      "Fatal error while truncating file %s at size %lld",
                                      pDescriptor.URIContent.toString(),
                                      wNewSize);
            ZException.exit_abort();
            }
    wHighBlock.BlockSize = wFreeSpace;
    pDescriptor.ZFBT->Tab[wHighBlockIndex].BlockSize = wFreeSpace;

    wSt=_writeBlockHeader(pDescriptor,wHighBlock,wHighBlock.Address);
    if (wSt!=ZS_SUCCESS)
            {
            return  wSt;
            }

    fprintf (pOutput,
             "  File %s has been truncated to %lld bytes\n"
             "  highest free block is at %lld (physical address) size %lld \n",
             pDescriptor.URIContent.toString(),
             wNewSize,
             wHighBlock.Address,
             wHighBlock.BlockSize);

    pDescriptor.ZFCB->AllocatedSize = wNewSize;

    pDescriptor.CurrentRank = -1;
    pDescriptor.LogicalPosition = -1;
    pDescriptor.PhysicalPosition = -1;

    return  (_writeFileDescriptor(pDescriptor,true));
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
            wMode=ZDescriptor.getMode();
            wZFT = ZDescriptor.ZHeader.FileType;
            zclose();
            }
        else
            {
            if (ZDescriptor.ZHeader.FileType==ZFT_Nothing)
                        {
                        wSt=ZS_BADFILETYPE;
                        ZException.setMessage(_GET_FUNCTION_NAME_,
                                                ZS_FILETYPEWARN,
                                                Severity_Warning,
                                                " ZFile_type == ZFT_Nothing : it must be set either by a previous zopen or by setting ZHeader::FileType. Set to ZFT_ZRandomFile");
                        wZFT=ZFT_ZRandomFile;
                        }
                else
                    wZFT = ZDescriptor.ZHeader.FileType;
            }// else
    if ((wSt=_open(ZDescriptor,ZRF_Exclusive|ZRF_All,wZFT))!=ZS_SUCCESS)
                                                return wSt;
    for (wi=0;(wi<ZDescriptor.ZBAT->size())&&(wSt==ZS_SUCCESS);wi++)
                    {
                    wSt=_freeBlock_Commit(ZDescriptor,wi);
                    }
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

//  Computing size of the mega free block
//          size of content file  - ZDescriptor.ZFCB->StartOfData
// Computing size of user content for mega free block
//          computed size for mega free block - sizeof(ZBlockHeader_Export)

    zsize_type wFreeBlockSize = 0;
    zsize_type wFreeUserSize = 0;

    // get file size

    if ((wFreeBlockSize=(zsize_type)lseek(ZDescriptor.ContentFd,0L,SEEK_END))<0)// get the physical file size
            {
            ZException.getErrno(errno,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEPOSERR,
                            Severity_Severe,
                            " Severe error while positionning to end of file %s",
                            ZDescriptor.URIContent.toString());


            wSt=ZS_FILEPOSERR;
            zclose();
            return  wSt;
            }

    // user requested pSize bytes

    wFreeBlockSize -= ZDescriptor.ZFCB->StartOfData ;
    wFreeUserSize = wFreeBlockSize - sizeof(ZBlockHeader_Export);

    if (wFreeBlockSize < pSize)
        {
        int wS=posix_fallocate(ZDescriptor.ContentFd,(off_t)wFreeBlockSize,(off_t)(pSize-wFreeBlockSize));
        if (wS!=0)
                {
                ZException.getErrno(wS,
                                _GET_FUNCTION_NAME_,
                                ZS_WRITEERROR,
                                Severity_Severe,
                                " Severe error while extending file space to end of file %s",
                                ZDescriptor.URIContent.toString());
                wSt= ZS_WRITEERROR;
                zclose();
                return  wSt;
                }
        }
// create one block with all the available space
// write the block descriptor to content file

    ZBlockDescriptor wBS;

    wBS.Address = ZDescriptor.resetPosition() ; // get the start of data physical address and align logicalPosition

    wBS.BlockSize = wFreeBlockSize;
//    wBS.BlockID = ZBID_Data ;
    wBS.State = ZBS_Free;
//    wBS.Pid   = ZDescriptor.Pid;  // warning get the current pid

// write this block header to content file at start of data

    wSt=_writeBlockHeader(ZDescriptor,(ZBlockHeader&)wBS,wBS.Address);
    if (wSt!=ZS_SUCCESS)
                {
                zclose();
                return  wSt;
                }
// Then reset all pools
//

    ZDescriptor.ZDBT->reset();
    ZDescriptor.ZBAT->reset();
    ZDescriptor.ZFBT->reset();
// up to here nothing in the file
    ZDescriptor.ZFBT->push(wBS);  // put block in free block pool
// just the mega free block in free block pool

// Now take care of HighwaterMarking : if no HWM : job is done
    if (ZDescriptor.ZFCB->HighwaterMarking)      // if highwatermarking then set the file region to zero
        {
        wSt=_highwaterMark_Block(ZDescriptor,wFreeUserSize);
        if (wSt!=ZS_SUCCESS)
                    {
                    zclose();
                    return  wSt;
                    }
        }// if HighWaterMarking

    fdatasync(ZDescriptor.ContentFd); // quicker than flush : writes only data and not metadata to file
    ZDescriptor.ZFCB->UsedSize = 0 ;
//
// current index and positions are invalid after removing of the block from ZBAT

    ZDescriptor.CurrentRank = -1;
    ZDescriptor.LogicalPosition = -1;
    ZDescriptor.PhysicalPosition = -1;
    zclose();                           // nb zclose write the whole file header

    if (FOpen)     // if file was open re-open in same mode
            { return  _open(ZDescriptor,wMode,wZFT);}
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
ZFileDescriptor wZDescriptor;

    wSt=wZDescriptor.setPath(pURI);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

    return  _reorgFile(wZDescriptor,pDump,pOutput);
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

    return(_reorgFile(ZDescriptor,pOutput));
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
ZRandomFile::_reorgFile(ZFileDescriptor& pDescriptor,bool pDump,FILE*pOutput)
{


ZStatus wSt;
bool wgrabFreeSpaceSet = false ;
bool wasOpen=false;
ZFile_type wFileType = ZFT_ZRandomFile;

// Open - reopen the file in exclusive mode

    zmode_type wMode = ZRF_Nothing ;
    if (pDescriptor._isOpen)
             {
             wMode=pDescriptor.getMode();
             wFileType = pDescriptor.ZHeader.FileType;
             _close(pDescriptor);
             wasOpen=true;
             }
            else
            {
            if ((wSt=_open(pDescriptor,ZRF_Read_Only,ZFT_ZRandomFile))!=ZS_SUCCESS) // open to get ZFile_type
                             return wSt;
            wFileType = pDescriptor.ZHeader.FileType;
            _close(pDescriptor);
            }
    if ((wSt=_open(pDescriptor,ZRF_Exclusive|ZRF_Read_Only,wFileType))!=ZS_SUCCESS) // open with appropriate ZFile_type
                                                        return wSt;
    if (!pDescriptor.ZFCB->GrabFreeSpace)        // activate grabFreeSpace if it has been set on
                {
                pDescriptor.ZFCB->GrabFreeSpace=true;
                wgrabFreeSpaceSet = true;
                }

    wSt= _reorgFileInternals(pDescriptor,pDump,pOutput);
    if (wSt!=ZS_SUCCESS)
            ZException.printUserMessage(pOutput);

    if (wgrabFreeSpaceSet)        // restore grabFreeSpace if it was off and has been set on
                 {
                 pDescriptor.ZFCB->GrabFreeSpace=false;
                 }
    _close (pDescriptor);
    if (wasOpen)
            _open(pDescriptor,wMode,wFileType);
    return  (wSt);
} // _reorgFile


ZStatus
ZRandomFile::_reorgFileInternals(ZFileDescriptor& pDescriptor,bool pDump,FILE*pOutput)
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


// pDescriptor MUST point to an opened ZRandomFile in ZRF_Exclusive move

    wCurrentAddress = pDescriptor.ZFCB->StartOfData ;

zsize_type wFileSize = pDescriptor.getAllocatedSize(); // get initial allocated size

    // first add a last element in ZBAT that will contain remaining space at each time
    // in order for free space not to be grabbed by _getFreeBlock allocation

    wBlockReminder.clear();
    wMaxAddress= -1;
    wMaxLastAddress= -1;
    wMinBlockSize = pDescriptor.ZBAT->Tab[0].BlockSize ;
    wMaxBlockSize = 0;

    wBlockReminder.State = ZBS_Control ; // special ZBS
//    pDescriptor.ZBAT->push(wBlockReminder); // created

     while (wZBATIdx<pDescriptor.ZBAT->size())
     {

     wCurrentBlock = pDescriptor.ZBAT->Tab[wZBATIdx];

     wSumUsedSize += (wCurrentBlock.BlockSize - sizeof(ZBlockHeader_Export));  //  compute sum of users size
     wSumBlockSize += wCurrentBlock.BlockSize ;     // compute sum of BlockSize

     if (wMinBlockSize > pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize)
                                    wMinBlockSize = pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize;
     if (wMaxBlockSize < pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize)
                                    wMaxBlockSize = pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize;

     if (pDescriptor.ZBAT->Tab[wZBATIdx].Address == wCurrentAddress) // is block already at correct place ?
                    {
                    wCurrentAddress += pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize ;    // ready for next block

                    wBlockReminder.Address = wCurrentAddress;
                    wBlockReminder.BlockSize = 0;
//                    pDescriptor.ZBAT->last().Address = wCurrentAddress;
//                    pDescriptor.ZBAT->last().BlockSize = 0;
                    wZBATIdx ++;
                    continue ;
                    }


     wMaxAddress = -1;
     wMaxLastAddress = -1;
     wZABlocksMove.reset();
     wZAIdx.reset();

     wCurrentBlock = pDescriptor.ZBAT->Tab[wZBATIdx] ;
     wNeedOfSpace = wCurrentBlock.BlockSize ;

     wStartAddress = wCurrentAddress;                       // we will implant
     wEndAddress = wStartAddress + wCurrentBlock.BlockSize + 1 ;   // up to here
     if (__ZRFVERBOSE__)
         fprintf (pOutput,
              "---------ZBAT rank %ld \n"
              "  Source address %lld block size %lld \n"
              "  Start address %lld End Address %lld  Size needed %lld------\n"
              "%s"
              "   ...searching...\n",
              wZBATIdx,
              wCurrentBlock.Address,
              wCurrentBlock.BlockSize,
              wCurrentAddress,
              wEndAddress,
              wCurrentBlock.BlockSize,
              (wCurrentBlock.Address < wEndAddress) ? "   Warning : destination address overlaps source\n":"");
// find all blocks that are present on wCurrentAddress , wNeedOfSpace



// if wBlockReminder has Not enough room to put the current block : search for space
//     if (pDescriptor.ZBAT->last().BlockSize < pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize)
    if (wBlockReminder.BlockSize < pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize)
           {
        for (long wi=wZBATIdx + 1 ;wi<(pDescriptor.ZBAT->size()-1);wi++)
            {
            if ((pDescriptor.ZBAT->Tab[wi].Address >= wStartAddress)&&(pDescriptor.ZBAT->Tab[wi].Address < wEndAddress))
                        {
                        if (pDescriptor.ZBAT->Tab[wi].Address>wMaxAddress)
                                    {
                                    wMaxAddress = pDescriptor.ZBAT->Tab[wi].Address;
                                    wMaxLastAddress = wMaxAddress + pDescriptor.ZBAT->Tab[wi].BlockSize;
                                    }
                        wZAIdx.push(wi);
                        wZABlocksMove.push(pDescriptor.ZBAT->Tab[wi]);
                        if (__ZRFVERBOSE__)
                            fprintf (stdout,
                                 " Selecting for move ZBAT block rank %ld address %lld size %lld \n",
                                 wi,
                                 pDescriptor.ZBAT->Tab[wi].Address,
                                 pDescriptor.ZBAT->Tab[wi].BlockSize);
                        }

            }// for

    for (long wi=0;wi < pDescriptor.ZFBT->size();wi++)
            {
    if ((pDescriptor.ZFBT->Tab[wi].Address >= wStartAddress)&&(pDescriptor.ZFBT->Tab[wi].Address < wEndAddress))
                        {
                        if (pDescriptor.ZFBT->Tab[wi].Address>wMaxAddress)
                                        {
                                        wMaxAddress = pDescriptor.ZFBT->Tab[wi].Address;
                                        wMaxLastAddress = wMaxAddress + pDescriptor.ZFBT->Tab[wi].BlockSize;
                                        }
                        wZAIdx.push(wi);
                        wZABlocksMove.push(pDescriptor.ZFBT->Tab[wi]);
                        if (__ZRFVERBOSE__)
                            fprintf (pOutput,
                                 " Selecting for deletion ZFBT block rank %ld address %lld size %lld\n",
                                 wi,
                                 pDescriptor.ZFBT->Tab[wi].Address,
                                 pDescriptor.ZFBT->Tab[wi].BlockSize);
                        }
            }// for

// up to here we have
// the list of index from ZFBT and ZBAT concerned
// the address to move the ZBAT block wCurrentAddress
// the highest address wMaxLastAddress

    ZBlockMin_struct wBMin;

// remove the ZFBT blocks (ZBS_Free)
    wi = 0;
    while (wi<wZABlocksMove.size())
    {
        if (wZABlocksMove[wi].State!=ZBS_Free)
                {
                wi ++;
                continue;
                }
    wZABlocksMove[wi].State = ZBS_Deleted;
    wSt=_writeBlockHeader(pDescriptor,wZABlocksMove[wi],wZABlocksMove[wi].Address);
    if (wSt!=ZS_SUCCESS)
                    {return  wSt;}
    pDescriptor.ZFBT->erase(wZAIdx[wi]); // this is now a hole
    wZABlocksMove.erase(wi);    // and forget it
    wZAIdx.erase(wi);           // its index too
    }// while

// we have to move the ZBAT blocks to another further location  either a using ZFBT or extend the file :
// use standard function _getFreeBlock with -1 as rank argument : it will return a new ZBAT element that will will suppress later
// Remark : using another index that -1 with INSERT a new ZBAT element at given rank
    long wi=0;
    while (wi<wZABlocksMove.size())
        {
        // move only ZBAT blocks - not ZFBT (will be deleted)
        if (wZABlocksMove[wi].State!=ZBS_Used)
                {
                wi ++;
                continue;
                }
                if (__ZRFVERBOSE__)
                  fprintf(pOutput,
                    " ----Moving ZBAT block rank %lld----- \n    Getting free space size %lld \n",
                    wZAIdx[wi],
                    wZABlocksMove[wi].BlockSize);
//            _getFreeBlock(pDescriptor,wZABlocksMove[wi].BlockSize,wBMin,wZAIdx[wi]) ; // find free block for ZBAT destination rank to reassign block
            _getFreeBlock(pDescriptor,
                          wZABlocksMove[wi].BlockSize,
                          wBMin,
                          -1,
                          wEndAddress) ;
            // find free block for ZBAT destination rank to reassign block on the stack, but with minimum of wEndAddress

        pDescriptor.ZBAT->pop();// immediately suppress this new ZBAT element : address and size are given in ZBlockMin wBMin as return

        // moving block to make room
        pDescriptor.ZBAT->Tab[wZAIdx[wi]].State = ZBS_Used;         // state is still ZBS_Used
        pDescriptor.ZBAT->Tab[wZAIdx[wi]].Address = wBMin.Address;  // new address for moved block
        if (__ZRFVERBOSE__)
            fprintf (pOutput,
                 " Making room : Moving block ZBAT index %ld from address %lld to address %lld size is %lld\n",
                  wZAIdx[wi],
                 wZABlocksMove[wi].Address,
                 pDescriptor.ZBAT->Tab[wZAIdx[wi]].Address,
                 pDescriptor.ZBAT->Tab[wZAIdx[wi]].BlockSize);

        wSt=_moveBlock(pDescriptor,
                       wZABlocksMove[wi].Address,
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
        fprintf (pOutput,
                 " Warning : Block Overlap Detected-----------\n");
        if (pDump)
                    {
            _headerDump(pDescriptor,pOutput);
            _surfaceScan(pDescriptor,pOutput);
                    }
                }
    if (__ZRFVERBOSE__)
        fprintf (pOutput,
             " -- Moving block ZBAT index %ld from address %lld to its sorted address %lld  size of block is %lld\n",
              wZBATIdx,
             pDescriptor.ZBAT->Tab[wZBATIdx].Address,
             wCurrentAddress,
             pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize);

    wSt=_moveBlock(pDescriptor,
                   pDescriptor.ZBAT->Tab[wZBATIdx].Address,
                   wCurrentAddress,
                   true);      // move and create a ZFBT entry for the target address

// create a free block in pool for the former block address
// TODO see later highwaterMarking impact

//   pDescriptor.ZFBT->push(pDescriptor.ZBAT->Tab[wZBATIdx]);
//   pDescriptor.ZFBT->last().State =ZBS_Free; // with correct State

    pDescriptor.ZBAT->Tab[wZBATIdx].Address = wCurrentAddress;  // update pool with the new address
    if (wSt!=ZS_SUCCESS)
                    { return  wSt;}

    wCurrentAddress += pDescriptor.ZBAT->Tab[wZBATIdx].BlockSize ;    // ready for next block

    if (wMaxLastAddress>0) // if there was some blocks moved (ZBAT) or deleted (ZFBT)
            {
//    pDescriptor.ZBAT->last().Address = wCurrentAddress;
//    pDescriptor.ZBAT->last().BlockSize = wMaxLastAddress-wCurrentAddress;
    wBlockReminder.Address = wCurrentAddress;
    wBlockReminder.BlockSize = wMaxLastAddress-wCurrentAddress;
            }
        else    // if not
            {
            wBlockReminder.BlockSize -= wCurrentBlock.BlockSize ;
            wBlockReminder.Address = wCurrentAddress;
            }
    if (__ZRFVERBOSE__)
        fprintf (stdout,
             "   Reminder address %lld size %lld    - wMaxAddress %lld wMaxLastAddress  %lld  wCurrentAddress %lld\n",
             wBlockReminder.Address,
             wBlockReminder.BlockSize,
             wMaxAddress,
             wMaxLastAddress,
             wCurrentAddress);
    if (pDump)
                {
        _headerDump(pDescriptor,pOutput);
        _surfaceScan(pDescriptor,pOutput);
                }


    //_writeFileDescriptor(pDescriptor) ; // needed because we read the descriptor in _scanSurface

    wZBATIdx ++;
     }// while

//
//  Cleaning Free blocks pool
//
    if (__ZRFVERBOSE__)
        fprintf(pOutput,
             ".....Deleting Free blocks....");
     for (wi=0;wi < pDescriptor.ZFBT->size();wi++)
     {
         pDescriptor.ZFBT->Tab[wi].State = ZBS_Deleted;
         wSt=_writeBlockHeader(pDescriptor,pDescriptor.ZFBT->Tab[wi],pDescriptor.ZFBT->Tab[wi].Address);
         if (wSt!=ZS_SUCCESS)
                     { return  wSt;}
     }
     pDescriptor.ZFBT->clear();
     if (__ZRFVERBOSE__)
        fprintf (pOutput,
             " <%ld> free blocks deleted\n",
             wi);

// compute now free block is what remains after the last address
// at this stage, remaining free blocks will be at the upper stack of the file
// Only one free block gathering the whole space should be set.

    wCurrentBlock.clear();
    wCurrentBlock.Address = wCurrentAddress ;
    wCurrentBlock.State = ZBS_Free;

    if ((wCurrentBlock.BlockSize=(zsize_type)lseek(pDescriptor.ContentFd,0L,SEEK_END))<0)    //! position to end of file and get position in return as file size
                {
                ZException.getErrno(errno,
                                _GET_FUNCTION_NAME_,
                                ZS_FILEPOSERR,
                                Severity_Severe,
                                " Severe error while positionning to end of file %s",
                                pDescriptor.URIContent.toString());


                wSt=ZS_FILEPOSERR;
                return  wSt;
                }
//
    wCurrentBlock.BlockSize -= wCurrentAddress;
    pDescriptor.ZFBT->push(wCurrentBlock);
    wSt=_writeBlockHeader(pDescriptor,wCurrentBlock,wCurrentAddress);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}
    if (__ZRFVERBOSE__)
        fprintf (pOutput,
             " creating one free block gathering all available space\n"
             "    free block is address %lld blocksize %lld\n",
             wCurrentBlock.Address,
             wCurrentBlock.BlockSize);

    wSt=_writeFileHeader(pDescriptor,true);
    if (wSt!=ZS_SUCCESS)
                {  return  wSt;}
    fprintf (pOutput,
             "  Physical file surface reorganization finished successfully\n");

// compute Block Target Size
    if (__ZRFVERBOSE__)
        fprintf (pOutput,
             " ----------adjusting file size to initial value %lld \n",
             wFileSize);

    if (pDescriptor.getAllocatedSize()<=wFileSize)
            {
                if (__ZRFVERBOSE__)
                    fprintf(pOutput,
                        " No need to readjust file size \n");
            }
        else
    {
    zsize_type wS=wFileSize-wCurrentBlock.Address;
    if (wS>0)
    {
        wSt=ztruncateFile(pDescriptor,wS,pOutput);
        if (wSt!=ZS_SUCCESS)
                    {  return  wSt;}
    }
    }

    if (__ZRFVERBOSE__)
        fprintf (pOutput,
             " -----------------Adjusting values -----------------------\n");

    wOld = pDescriptor.ZFCB->BlockTargetSize ;
    pDescriptor.ZFCB->BlockTargetSize  = (long)((float)wSumBlockSize / (float)pDescriptor.ZBAT->size());
    fprintf (pOutput,
             " BlockTargetSize former value %ld  new value %ld \n",
             wOld,
             pDescriptor.ZFCB->BlockTargetSize);

    wOld = pDescriptor.ZFCB->UsedSize ;
    pDescriptor.ZFCB->UsedSize  = (long)((float)wSumUsedSize / (float)pDescriptor.ZBAT->size());
    fprintf (pOutput,
             " UsedSize former value %ld  new value %ld \n",
             wOld,
             pDescriptor.ZFCB->UsedSize);


    fprintf (pOutput,
             " MinSize- Minimum block size found : former value %ld  new value %ld \n",
             pDescriptor.ZFCB->MinSize,
             wMinBlockSize
             );
    pDescriptor.ZFCB->MinSize=wMinBlockSize;
    fprintf (pOutput,
             " MaxSize- Maximum block size found : former value %ld  new value %ld \n",
             pDescriptor.ZFCB->MaxSize,
             wMaxBlockSize
             );


    pDescriptor.ZFCB->MaxSize=wMaxBlockSize;

    pDescriptor.CurrentRank = -1;
    pDescriptor.LogicalPosition = -1;
    pDescriptor.PhysicalPosition = -1;
    return  wSt;
} // _reorgFileInternals

/** @cond Test

 * @brief ZRandomFile::putTheMess  For test purposes changes blocks allocations
 */
void
ZRandomFile::putTheMess (void)
{

    ZDescriptor.ZBAT->swap(1,3) ;
    ZDescriptor.ZBAT->swap(0,5) ;
    ZDescriptor.ZBAT->swap(2,7) ;
    _writeFileDescriptor(ZDescriptor,true);
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
ZRandomFile::_highwaterMark_Block (ZFileDescriptor &pDescriptor,const zsize_type pFreeUserSize)
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
                pDescriptor.ZPMS.HighWaterWrites ++;

                wSWrite=write (pDescriptor.ContentFd,wHWBuffer.DataChar, wHWBuffer.Size);
                if (wSWrite<0)
                        {
                        ZException.getErrno(errno,
                                         _GET_FUNCTION_NAME_,
                                         ZS_WRITEERROR,
                                         Severity_Severe,
                                         "Severe Error while high water marking freed block for file %s at address %lld",
                                         pDescriptor.URIContent.toString(),
                                         pDescriptor.getPhysicalPosition());
                         return  (ZS_WRITEERROR);
                        }

                pDescriptor.incrementPhysicalPosition(wSWrite);
                wFreeUserSize -= __HIGHWATER_MODULO__;
                } // while
        } // if (wFreeUserSize >__HIGHWATER_MODULO__)
    wHWBuffer.allocate(pFreeUserSize);
    wHWBuffer.clearData();
    pDescriptor.ZPMS.HighWaterWrites ++;

    wSWrite=write (pDescriptor.ContentFd,wHWBuffer.DataChar, wHWBuffer.Size);
    if (wSWrite<0)
            {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_WRITEERROR,
                             Severity_Severe,
                             "Severe Error while high water marking freed block for file %s at address %lld",
                             pDescriptor.URIContent.toString(),
                             pDescriptor.getPhysicalPosition());
            return  (ZS_WRITEERROR);
            }
    pDescriptor.incrementPhysicalPosition(wSWrite);
    return  ZS_SUCCESS;
}

/** @endcond  */

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
                                ZDescriptor.URIContent.toString());
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
                                ZDescriptor.URIContent.toString());
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
                                ZDescriptor.URIContent.toString());
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
    utfdescString wExt;

    wExt=pURIPath.getFileExtension();
    const utf8_t* wExtension=(const utf8_t*)__HEADER_FILEEXTENSION__;
     if (wExtension[0]=='.')
             wExtension++;       // +1 because we should miss the '.' char
     if (!wExt.compare(wExtension))
                     {
                     ZException.setMessage(_GET_FUNCTION_NAME_,
                                             ZS_INVNAME,
                                             Severity_Error,
                                             "File name is malformed. Extension <%s> is reserved while given file name is <%s>",
                                             __HEADER_FILEEXTENSION__,
                                             pURIPath.toString());
                     return ZS_INVNAME;
                     }

    pURIHeader=pURIPath;
    utf8_t *wPtr=pURIHeader.strrchr((utf8_t)'.');
    if (wPtr) // '.' is found cut the string at this position
            {
            wPtr[0]='\0';
            }

    pURIHeader.add((utf8_t*)__HEADER_FILEEXTENSION__);
    return ZS_SUCCESS;
}


/**
 * @brief decode_ZBS gives the key word content of a ZBlockState_type using its code
 * @param pZBS  the code to decode
 * @return      a constant string with the keyword
 */
const char *
decode_ZBS (ZBlockState_type pZBS)
    {
    switch (pZBS)
            {
            case ZBS_Nothing :
                    {
                    return ("ZBS_Nothing");
                    }
            case ZBS_Used :
                    {
                    return ("ZBS_Used");
                    }

            case ZBS_Free :
                {
                    return ("ZBS_Free");
                }
            case ZBS_Deleted :
                {
                return ("ZBS_Deleted");
                }
            case ZBS_BeingDeleted :
                {
                return ("ZBS_BeingDel");
                }

        case ZBS_Control :
            {
                return ("ZBS_Control");
            }
        case ZBS_Allocated :
            {
                return ("ZBS_Alloctd");
            }
    default :
            {
                return ("Unknwn ZBSt");
            }
}//switch
}//decode_ZBS

const char *
decode_ZBID (ZBlockID pZBID)
    {
    switch (pZBID)
            {
            case ZBID_Nothing :
                    {
                        return ("ZBID_Nothing");
                    }
            case ZBID_FileHeader :
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
                return ("Unknownn ZBlockID");
            }
}//switch
}//decode_ZBS

utf8FixedString<50> wMode;

const char *
decode_ZRFMode (zmode_type pZRF)
{

    if (pZRF&ZRF_Exclusive)
//            wMode.fromCString( "ZRF_Exclusive | ");
            wMode= (const utf8_t*)"ZRF_Exclusive";

    if (pZRF & ZRF_ManualLock)
            wMode.addConditionalOR( (utf8_t*)"ZRF_ManualLock ");
    if (pZRF&ZRF_Read_Only)
            wMode += (const utf8_t*)"ZRF_Read | ";
    if (pZRF&ZRF_Write_Only)
            wMode += (const utf8_t*)"ZRF_Write  | ";
    if (pZRF&ZRF_Delete_Only)
            wMode += (const utf8_t*)"ZRF_Delete  | ";


    if (wMode.isEmpty())
            {
                return ("Unknownn ZRFMode_type");
            }

    wMode+=(utf8_t)'\0';
    return wMode.toCString_Strait();
}//decode_ZRFMode

zmode_type
encode_ZRFMode (char *pZRF)
{
zmode_type wRet = ZRF_Nothing;

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
decode_ZFile_type (ZFile_type pType)
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

            case ZFT_ZMasterFile :
                {
                    return ("ZFT_ZMasterFile");
                }
            case ZFT_ZIndexFile :
                {
                    return ("ZFT_ZIndexFile");
                }
        case ZFT_ZSMasterFile :
            {
                return ("ZFT_ZSMasterFile");
            }
        case ZFT_ZSIndexFile :
            {
                return ("ZFT_ZSIndexFile");
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
