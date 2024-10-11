#ifndef ZINDEXFILE_CPP
#define ZINDEXFILE_CPP
#include <zindexedfile/zindexfile.h>
//#include <zindexedfile/zifgeneric.h>
#include <zrandomfile/zrfcollection.h>
using namespace zbs ;


//----------zstatistic---------------------------------
/**
@addtogroup ZMFSTATS

@{
*/

/**
 * @brief zstatistics::operator - Operator overload : subtracts two zstatistics
 *  @note ZTimer itself remains unchanged : only DeltaTime is changed
 *
 * @param pPMSIn        second term of operator
 * @return a zstatistics structure
 */
zstatistics
zstatistics::operator - (zstatistics &pPMSIn)
    {
    zstatistics wzstatistics1;
    wzstatistics1 = *this;
    wzstatistics1.Iterations -= pPMSIn.Iterations;
    wzstatistics1.Reads -= pPMSIn.Reads;
    wzstatistics1.Delta -= pPMSIn.Delta;


// ZRFPMS base
    wzstatistics1.HFHReads -= pPMSIn.HFHReads;
    wzstatistics1.HFHWrites -= pPMSIn.HFHWrites;
    wzstatistics1.HReservedReads -= pPMSIn.HReservedReads;
    wzstatistics1.HReservedWrites -= pPMSIn.HReservedWrites;
    wzstatistics1.HFDReads -= pPMSIn.HFDReads;
    wzstatistics1.HFDWrites -= pPMSIn.HFDWrites;
    wzstatistics1.CBHReads -= pPMSIn.CBHReads;
    wzstatistics1.CBHReadBytesSize -= pPMSIn.CBHReadBytesSize;
    wzstatistics1.CBHWrites -= pPMSIn.CBHWrites;
    wzstatistics1.CBHWriteBytesSize -= pPMSIn.CBHWriteBytesSize;
    wzstatistics1.UserReads -= pPMSIn.UserReads;
    wzstatistics1.UserWrites -= pPMSIn.UserWrites;
    wzstatistics1.UserWriteSize -= pPMSIn.UserWriteSize;
    wzstatistics1.UserReadSize -= pPMSIn.UserReadSize;
    wzstatistics1.HighWaterWrites -= pPMSIn.HighWaterWrites;
    wzstatistics1.HighWaterBytesSize -= pPMSIn.HighWaterBytesSize;
    wzstatistics1.ExtentWrites -= pPMSIn.ExtentWrites;
    wzstatistics1.ExtentSize -= pPMSIn.ExtentSize;
    wzstatistics1.FreeMatches -= pPMSIn.FreeMatches;
    return wzstatistics1 ;
 }

/**
 * @brief zstatistics::init Initializes statistical monitoring : starts a timer
 */
void
zstatistics::init (void)
{
    Timer.start();
    ZRFPMS::clear();
    if (PMSBase!=nullptr)
        PMSStart = *PMSBase;
    return;
}// init
/**
 * @brief zstatistics::end End monitoring session. Stops the timer and compute delta time.
 * @return a reference to current zstatistics object
 */
zstatistics &
zstatistics::end (void)
    {
    ZRFPMS wzstatistics1;
    Timer.end();
    if (PMSBase==nullptr)
                return *this;
    wzstatistics1 = *PMSBase;

// ZRFPMS base
    *this = wzstatistics1 - PMSStart ;
    return *this ;
 }
/**
 * @brief zstatistics::operator -=  Operator overload for zstatistics
 *  @note ZTimer itself remains unchanged : only DeltaTime is changed
 *
 * @param pPMSIn
 * @return
 */
zstatistics&
zstatistics::operator -= (zstatistics &pPMSIn)
    {
    Iterations -= pPMSIn.Iterations;
    Reads -= pPMSIn.Reads;
    Delta -= pPMSIn.Delta;


// ZRFPMS base
    HFHReads -= pPMSIn.HFHReads;
    HFHWrites -= pPMSIn.HFHWrites;
    HReservedReads -= pPMSIn.HReservedReads;
    HReservedWrites -= pPMSIn.HReservedWrites;
    HFDReads -= pPMSIn.HFDReads;
    HFDWrites -= pPMSIn.HFDWrites;
    CBHReads -= pPMSIn.CBHReads;
    CBHReadBytesSize -= pPMSIn.CBHReadBytesSize;
    CBHWrites -= pPMSIn.CBHWrites;
    CBHWriteBytesSize -= pPMSIn.CBHWriteBytesSize;
    UserReads -= pPMSIn.UserReads;
    UserWrites -= pPMSIn.UserWrites;
    UserWriteSize -= pPMSIn.UserWriteSize;
    UserReadSize -= pPMSIn.UserReadSize;
    HighWaterWrites -= pPMSIn.HighWaterWrites;
    HighWaterBytesSize -= pPMSIn.HighWaterBytesSize;
    ExtentWrites -= pPMSIn.ExtentWrites;
    ExtentSize -= pPMSIn.ExtentSize;
    FreeMatches -= pPMSIn.FreeMatches;
    return *this ;
 }

/**
 * @brief zstatistics::operator + operator overload : adds two zstatistics
 *  @note ZTimer itself remains unchanged : only DeltaTime is changed
 * @param pPMSIn
 * @return
 */
zstatistics
zstatistics::operator + (zstatistics &pPMSIn)
    {
    zstatistics wzstatistics1;
    wzstatistics1 = *this;
    wzstatistics1.Iterations += pPMSIn.Iterations;
    wzstatistics1.Reads += pPMSIn.Reads;
    wzstatistics1.Delta += pPMSIn.Delta;

// ZRFPMS base
    wzstatistics1.HFHReads += pPMSIn.HFHReads;
    wzstatistics1.HFHWrites += pPMSIn.HFHWrites;
    wzstatistics1.HReservedReads += pPMSIn.HReservedReads;
    wzstatistics1.HReservedWrites += pPMSIn.HReservedWrites;
    wzstatistics1.HFDReads += pPMSIn.HFDReads;
    wzstatistics1.HFDWrites += pPMSIn.HFDWrites;
    wzstatistics1.CBHReads += pPMSIn.CBHReads;
    wzstatistics1.CBHReadBytesSize += pPMSIn.CBHReadBytesSize;
    wzstatistics1.CBHWrites += pPMSIn.CBHWrites;
    wzstatistics1.CBHWriteBytesSize += pPMSIn.CBHWriteBytesSize;
    wzstatistics1.UserReads += pPMSIn.UserReads;
    wzstatistics1.UserWrites += pPMSIn.UserWrites;
    wzstatistics1.UserWriteSize += pPMSIn.UserWriteSize;
    wzstatistics1.UserReadSize += pPMSIn.UserReadSize;
    wzstatistics1.HighWaterWrites += pPMSIn.HighWaterWrites;
    wzstatistics1.HighWaterBytesSize += pPMSIn.HighWaterBytesSize;
    wzstatistics1.ExtentWrites += pPMSIn.ExtentWrites;
    wzstatistics1.ExtentSize += pPMSIn.ExtentSize;
    wzstatistics1.FreeMatches += pPMSIn.FreeMatches;
    return wzstatistics1 ;
 }

/**
 * @brief zstatistics::operator +=  Operator overload for zstatistics
 *  @note ZTimer itself remains unchanged : only DeltaTime is changed
 *
 * @param pPMSIn
 * @return
 */
zstatistics&
zstatistics::operator += (zstatistics &pPMSIn)
    {
    Iterations += pPMSIn.Iterations;
    Reads += pPMSIn.Reads;
    Delta += pPMSIn.Delta;


// ZRFPMS base
    HFHReads += pPMSIn.HFHReads;
    HFHWrites += pPMSIn.HFHWrites;
    HReservedReads += pPMSIn.HReservedReads;
    HReservedWrites += pPMSIn.HReservedWrites;
    HFDReads += pPMSIn.HFDReads;
    HFDWrites += pPMSIn.HFDWrites;
    CBHReads += pPMSIn.CBHReads;
    CBHReadBytesSize += pPMSIn.CBHReadBytesSize;
    CBHWrites += pPMSIn.CBHWrites;
    CBHWriteBytesSize += pPMSIn.CBHWriteBytesSize;
    UserReads += pPMSIn.UserReads;
    UserWrites += pPMSIn.UserWrites;
    UserWriteSize += pPMSIn.UserWriteSize;
    UserReadSize += pPMSIn.UserReadSize;
    HighWaterWrites += pPMSIn.HighWaterWrites;
    HighWaterBytesSize += pPMSIn.HighWaterBytesSize;
    ExtentWrites += pPMSIn.ExtentWrites;
    ExtentSize += pPMSIn.ExtentSize;
    FreeMatches += pPMSIn.FreeMatches;
    return *this ;
 }

/**
 * @brief zstatistics::reportShort displays to pOuput the content of statistical counters
 *
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
zstatistics::reportShort(FILE* pOutput)
{
    fprintf(pOutput,
            "________________________________________________\n"
            "               Statistics\n"
            " Iterations %ld\n"
            " Reads      %ld\n"
            " Time        %s\n",
            Iterations,
            Reads,
            Timer.reportDeltaTime().toString());

    return;
}
/**
 * @brief zstatistics::reportFull displays to pOuput the content of statistical counters plus base ZRFPMS data
 *
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
zstatistics::reportFull(FILE* pOutput)
{
    reportShort(pOutput);
    reportDetails(pOutput);
    return;
}

/** @} */ //  ZMFSTATS


/** @addtogroup ZIndex
*   @{
*/

//---------ZIndexItem_struct routines-------------------------------------------------------
//
/**
 * @brief ZIndexItem_struct::toFileKey converts the memory content of the ZIndexItem Key to a flat stream of bytes to write on Index File
 * @note ZOp Operation field is not stored on Index File
 *
 * @return a reference to a ZDataBuffer containing the flat content of ZIndexItem being processed
 */
ZDataBuffer&
ZIndexItem::toFileKey(void)
{
    Exchange.setData(&ZMFaddress,sizeof(zaddress_type));
    Exchange.appendData(KeyContent);
    return Exchange;
}

/**
 * @brief ZIndexItem_struct::fromFileKey loads a ZIndexItem_struct from an Index file record contained in a ZDataBuffer structure (pFileKey)
 * @note ZOp Operation field is not stored on Index File, and therefore is not subject to be loaded
 *
 * @param[in] pFileKey ZIndexFile record content to load into current ZIndexItem
 * @return a reference to current ZIndexItem being processed
 */
ZIndexItem&
ZIndexItem::fromFileKey (ZDataBuffer &pFileKey)
{
size_t wOffset =0;
size_t wSize;

    clear();
    memmove (&ZMFaddress,pFileKey.Data+wOffset,sizeof(zaddress_type));
    wOffset += sizeof(ZMFaddress);
    wSize = pFileKey.Size - wOffset ;
    KeyContent.setData(pFileKey.Data+wOffset,wSize);
    return *this;
}


//---------ZIndexControlBlock routines----------------------------------------------------

/**
 * @brief ZIndexControlBlock::_exportICB exports ZIndexControlBlock content to a flat ZDataBuffer.
 * @return a ZDataBuffer containing the flat content of ZIndexControlBlock
 */
ZDataBuffer&
ZIndexControlBlock::_exportICB(ZDataBuffer &pICBContent)
{
ZDataBuffer     wDB;

 //       pICBContent.clear();  // Exchange is a ZDataBuffer belonging to ZIndexControlBlock and dedicated to export/import ops

        pICBContent.setData(this,sizeof(ZICBOwnData));  // move all data before ZKDic of ICBExchange ZDataBuffer
        ZKDic->_export(wDB);             // export key dictionary
        pICBContent.appendData(wDB);    // append it to ICBExchange ZDataBuffer

        ZICBOwnData* wICB = (ZICBOwnData*) pICBContent.Data;
        wICB->ICBSize = pICBContent.Size;  // update size of exported ICB into ICBExchange ZDataBuffer

        if (ZVerbose)
            printf ("---%s------------\n",_GET_FUNCTION_NAME_);
/*        ZDataBuffer wHexa;
        ZDataBuffer wAscii;
        ICBExchange.dumpHexa(0L,ICBExchange.Size,wHexa,wAscii);

        fprintf (stderr,"\n _export ICB\n %s\n %s\n=============\n",wHexa.DataChar,wAscii.DataChar);
*/
        return pICBContent;
}// _export

/**
 * @brief ZIndexControlBlock::_importICB Imports (load) current ZIndexControlBlock from a ZDataBuffer containing
 * a flat content (coming from Reserved Header space)
 *
 * @param[in] pRawICB flat content to load  in a ZDataBuffer
 * @param[out] Imported data size : note this represents the amount of space imported from pRawICB starting from Offset. pRawICB may contain more than one ZICB.
 * @param[in] pOffset Offset from the beginning of ZDataBuffer pRawICB to start import (pRawICB may contain more than one ZICB)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexControlBlock::_importICB (ZDataBuffer& pRawICB, ssize_t &pImportedSize, size_t pOffset)
{
//-------ICB Integrity controls--------------

ZICBOwnData *wICB = (ZICBOwnData*)pRawICB.Data+pOffset;

    if (wICB->BlockID != ZBID_ICB)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_BADICB,
                               Severity_Error,
                               "Error Index Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
                               wICB->BlockID);
        return(ZS_BADICB);
    }
    if (wICB->StartSign != cst_ZSTART)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_BADFILEHEADER,
                          Severity_Error,
                          "Error Index Control Block  : Index header appears to be corrupted - invalid ICB StartBlock");
    return(ZS_BADFILEHEADER);
    }
    if (wICB->ZMFVersion!= __ZMF_VERSION__)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_BADFILEVERSION,
                          Severity_Error,
                          "Error Index Control Block   : Found version <%ld> while current ZMF version is <%ld>",
                          wICB->ZMFVersion,
                          __ZMF_VERSION__);
    return(ZS_BADFILEVERSION);
    }

    clear();  // reset ZIndexControlBlock and reset Key Dictionary

    memmove (this,pRawICB.Data+pOffset,sizeof(ZICBOwnData));


    pImportedSize = sizeof(ZICBOwnData);
    pImportedSize += ZKDic->_import((unsigned char*)pRawICB.Data+sizeof(ZICBOwnData));

    ZKDic->_reComputeSize(); // whole key sizes are not saved
    return ZS_SUCCESS;
}// _import from a ZDataBuffer

/**
 * @brief ZIndexControlBlock::_importICB Imports an ZIndexControlBlock from its flats structure(unsigned char*) and load it to current ZICB
 * @note pBuffer pointer is not freed by this routine
 * @param[in] pBuffer
 * @param[out] Imported data size : note this represents the amount of space imported from pBuffer since its beginning. pBuffer may contain more than one ZICB.
 * @return
 */
ZStatus
ZIndexControlBlock::_importICB (unsigned char* pBuffer,ssize_t &pImportedSize)
{
//-------ICB Integrity controls--------------
// What if an index is added or suppressed by another process during a ZMasterFile open session ??? Not possible: File must be open in Exclusive mode (Mandatory)

ZICBOwnData *wICB = (ZICBOwnData*)pBuffer;

    if (wICB->BlockID != ZBID_ICB)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_BADICB,
                               Severity_Error,
                               "Error Index Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
                               wICB->BlockID);
        return(ZS_BADICB);
    }
    if (wICB->StartSign != cst_ZSTART)
    {
   ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_BADFILEHEADER,
                          Severity_Error,
                          "Error Index Control Block  : Index header appears to be corrupted - invalid ICB StartBlock");
   return(ZS_BADFILEHEADER);
    }
    if (wICB->ZMFVersion!= __ZMF_VERSION__)
    {
   ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_BADFILEVERSION,
                          Severity_Error,
                          "Error Index Control Block   : Found version <%ld> while current ZMF version is <%ld>",
                          wICB->ZMFVersion,
                          __ZMF_VERSION__);
   return(ZS_BADFILEVERSION);
    }

    clear();  // reset ZIndexControlBlock and reset Key Dictionary

    memmove (this,pBuffer,sizeof(ZICBOwnData));
    pImportedSize = sizeof(ZICBOwnData);
    pImportedSize += ZKDic->_import(pBuffer+sizeof(ZICBOwnData));

    ZKDic->_reComputeSize(); // whole key sizes are not saved

    return ZS_SUCCESS;
}// _import from pointer



/**
 * @brief ZIndexControlBlock::zKeyValueExtraction Extracts the Key value from ZMasterFile record data using dictionnary CZKeyDictionary fields definition
 *
 * Returns the concaneted key value in pKey ZDataBuffer.
 * - Key fields are extracted from the ZMasterFile user record .
 * - They are converted appropriately whenever required using base internal conversion routines according Dictionary data type ( ZType_type ):
 *    + atomic fields _getAtomicFromRecord()
 *    + arrays _getArrayFromRecord()
 *    + for data type Class (ZType_type) : data is simply mass-moved to key without any conversion
 *
 * @note As we are processing variable length records, if a defined key field points outside the record length,
 *       then its returning key value is set to binary zero on the corresponding length of the field within returned Key value.
 *
 *
 *
 * @param[in] pZKDic  ZIndex dictionary (part of ZIndexControlBlock) for the index to extract key for
 * @param[in pRecord  ZMasterFile user record to extract key from
 * @param[out] pKey  Resulting concatenated key content
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexControlBlock::zkeyValueExtraction (ZDataBuffer &pRecord, ZDataBuffer& pKey)
{
    return _keyValueExtraction(*this->ZKDic,pRecord,pKey);
}



//----------ZIndexFile-----------------------------------------------


ZIndexFile::ZIndexFile  (ZMasterFile *pFather): ZRandomFile()
{

    ZMFFather=pFather;
    if (pFather==nullptr)
                {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVADDRESS,
                                    Severity_Fatal,
                                    "ZIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
            this->~ZIndexFile();
            ZException.exit_abort();
                }
    return;

}// ZIF CTOR


ZIndexFile::ZIndexFile  (ZMasterFile *pFather, ZIndexControlBlock* pZICB): ZRandomFile()
{
    ZMFFather=pFather;

    if (pFather==nullptr)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVADDRESS,
                                        Severity_Fatal,
                                        "ZIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
                this->~ZIndexFile();
                ZException.exit_abort();
                }
        ZICB = pZICB;
        if (pZICB==nullptr)
                    {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVADDRESS,
                                        Severity_Fatal,
                                        "ZIndexFile::CTOR-F-IVZICB Index Control Block pointer is invalid (null value) \n");
                this->~ZIndexFile();
                ZException.exit_abort();
                    }
    return;

}// ZIF CTOR 2 w


ZStatus
ZIndexFile::setIndexURI  (uriString &pURI)
{
    IndexUri=pURI;
    return _Base::setPath(pURI);
}

void
ZIndexFile::setIndexName  (utfdescString &pName)
{
    ZICB->Name = pName;
    return;
}

void
ZIndexFile::setICB (ZIndexControlBlock *pICB)
{
 ZICB = pICB;
 return;
}
#include <zindexedfile/zmasterfile.h>

/**
 * @brief ZIndexFile::zrebuildIndex rebuilds the current index
 *
 * - Clears the file using ZRandomFile::Clear()
 * - Re-create each index rank from father's records content
 * - Optionally displays statistical information
 *
 * @param[in] pStat a flag mentionning if statistics will be produced (true) or not (false) during index rebuild
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexFile::zrebuildIndex(bool pStat, FILE*pOutput)
{
ZStatus         wSt = ZS_SUCCESS;
ZDataBuffer     wRecord;
long            wZMFRank = 0;
zaddress_type   wZMFAddress=0;
long            wIndexRank;

long            wIndexCount=0;

    if (!(ZDescriptor.Mode & ZRF_Exclusive)||((ZDescriptor.Mode & ZRF_All )!=ZRF_All))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_MODEINVALID,
                                    Severity_Error,
                                    "Request to rebuild file <%s> while open mode is invalid <%s>. Must be (ZRF_Exclusive | ZRF_All)",
                                    ZDescriptor.URIContent.toString(),
                                    decode_ZRFMode(ZDescriptor.Mode));
            return ZS_MODEINVALID;
            }
    if (pStat)
            ZPMSStats.init();
    fprintf (pOutput,
             "______________Rebuilding Index <%s>_______________\n"
               " File is %s \n",
               ZICB->Name.toString(),
               ZDescriptor.URIContent.toString());
    ZMasterFile* wFather = static_cast <ZMasterFile*> (ZMFFather);
    zsize_type wFatherSize = wFather->getRecordCount();
    zsize_type wSize = ZICB->IndexRecordSize() * wFather->size() ;
    if (ZVerbose)
            {
            fprintf (pOutput,
                     " Index file size is computed to be %lld\n",
                     wSize);

            fprintf (pOutput,"Clearing index file\n");
            }

    wSt=_Base::zclearFile(wSize);  // clearing file with a free block space equals to the whole index

    if (wFather->isEmpty())
            {
            fprintf(pOutput,
                    " ------------No record in ZMasterFile <%s> : no index to rebuild..........\n",
                    wFather->ZDescriptor.URIContent.toString());


            return ZS_SUCCESS;
            }


    wSt=wFather->zgetWAddress(wRecord,wZMFRank,wZMFAddress);
    for (zsize_type wi=0;(wSt==ZS_SUCCESS)&&(wi <wFatherSize);wi++ )
            {
 //           wZMFRank = wFather->zgetCurrentRank();
 //           wZMFAddress=wFather->zgetCurrentLogicalPosition();
            wIndexCount++;
            wSt=_addKeyValue(wRecord,wIndexRank,wZMFAddress);
            if (wSt!= ZS_SUCCESS)
                                break;
            wSt=wFather->zgetNextWAddress(wRecord,wZMFRank,wZMFAddress) ;
            } // for
    if ((wSt==ZS_EOF)||(wSt==ZS_OUTBOUNDHIGH))
            wSt=ZS_SUCCESS;

    if (pStat)
            {
            ZPMSStats.end();
            ZPMSStats.reportFull(pOutput);
            }
    fprintf (pOutput,"\n   %ld index keys added to index \n", wIndexCount);

    if (wSt!=ZS_SUCCESS)
            {
            fprintf (pOutput," ----- index rebuild ended with error --------\n");
            ZException.printUserMessage();
            return wSt;
            }
    fprintf (pOutput," ---------Successfull end rebuilding process for Index <%s>------------\n",
               ZICB->Name.toString());
    return wSt;
}//zrebuildIndex


/**
 * @brief ZIndexFile::removeIndexFiles Removes all files composing current index (before index remove at ZMasterFile level)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexFile::removeIndexFiles(void)
{
    return _Base::zremoveFile();
}

/**
 * @brief ZIndexFile::zcreateIndex creates a new index file corresponding to the given specification ICB and ZRF parameters
 *
 *  @note This could be NOT a good idea to set GrabFreeSpace option : Indexes are fixed length then search in Free Pool are only made with the same size.
 *
 * @param[in] pICB              pointer to ZIndexControlBlock of indexfile to create
 * @param[in] pIndexUri         uriString with index file name
 * @param[in] pAllocatedBlocks
 * @param[in] pBlockExtentQuota
 * @param[in] pInitialSize
 * @param[in] pHighwaterMarking
 * @param[in] pGrabFreeSpace
 * @param[in] pLeaveOpen        Option : true leave the index file open as (ZRF_Exclusive | ZRF_All) false: close index file right after creation
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexFile::zcreateIndex(ZIndexControlBlock *pICB,
                         uriString &pIndexUri,
                         long pAllocatedBlocks,
                         long pBlockExtentQuota,
                         zsize_type pInitialSize,
                         bool pHighwaterMarking,
                         bool pGrabFreeSpace,
                         bool pReplace,
                         bool pLeaveOpen)
{
ZStatus wSt;

    setICB(pICB);
    wSt=setIndexURI(pIndexUri);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZIndexFile %s\n",
                                         pIndexUri.toString());
                return wSt;
                }
//  ZIndexFile Record size is KeySize (sum of fields lengths) plus size of a zaddress_type (pointer to Master File record)

    _Base::setCreateMaximum (pAllocatedBlocks,
                             pBlockExtentQuota,
                             pICB->IndexRecordSize(),
                             pInitialSize,
                             pHighwaterMarking,
                             pGrabFreeSpace);
    wSt= _Base::_create(ZDescriptor,pInitialSize,ZFT_ZIndexFile,pReplace,false);             // Do not leave it open after file creation : ZRF_Exclusive | ZRF_All

    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZIndexFile %s\n",
                                         pIndexUri.toString());
                return wSt;
                }
    wSt=_Base::_open(ZDescriptor,(ZRF_Exclusive | ZRF_All),ZFT_ZIndexFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZIndexFile %s\n",
                                         pIndexUri.toString());
                return wSt;
                }
//
// now need to write the updated ICB to Index Header
//
    wSt=writeIndexControlBlock ();
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZIndexFile %s\n",
                                         pIndexUri.toString());
                return wSt;
                }

    if (!pLeaveOpen)
            return  _Base::zclose();
    return ZS_SUCCESS;
}//zcreateIndexFile

ZStatus
ZIndexFile::openIndexFile(uriString &pIndexUri,const int pMode)
{
ZStatus wSt;
ZDataBuffer wICBContent;
    IndexUri = pIndexUri;

    wSt=_Base::setPath(IndexUri);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" setting path for ZIndexFile %s\n",
                                         pIndexUri.toString());
                return wSt;
                }
    wSt=_Base::_open(ZDescriptor,pMode,ZFT_ZIndexFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" Openning ZIndexFile %s\n",
                                         pIndexUri.toString());
                return wSt;
                }
// --------- Need to read ICB and check whether it is ok with the given ICB --------------
//
    ZDataBuffer wRawICB;
    ssize_t wImportSize;
    wSt=_Base::getReservedBlock(wRawICB,true);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast( " Reading Reserved header zone ZIndexFile %s",
                                        pIndexUri.toString());
                return wSt;
                }


    ZICB->_importICB(wRawICB,wImportSize,0L);

    if (memcmp(wRawICB.Data,ZICB->_exportICB(wICBContent).Data,wRawICB.Size)!=0)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADICB,
                                    Severity_Severe,
                                    " Index Control Block does not mach given ICB - Index file is %s",
                                    IndexUri.toString()
                                    );
            ZException.setComplement ("Index files found are not aligned with Master file index definition");
            return ZS_BADICB;
            }

// zstatistic intitialization

    ZPMSStats.PMSBase= &ZDescriptor.ZPMS ;

    return ZS_SUCCESS ;
}//zopenIndexFile



ZStatus
ZIndexFile::closeIndexFile(void)
{
ZStatus wSt;
ZDataBuffer wICBContent;
//
// flush ICB to Index file
//
    wSt=_Base::updateReservedBlock(ZICB->_exportICB(wICBContent),true);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast( " Writing Reserved header for ZIndexFile %s",IndexUri.toString());
                return wSt;
                }

    return _Base::zclose();
}//closeIndexFile

ZStatus
ZIndexFile::writeIndexControlBlock(void)
{
ZStatus wSt;
ZDataBuffer wICBContent;
//
// flush ICB to Index file and set ZFile_type to ZFT_IndexFile
//
    ZDescriptor.ZHeader.FileType = ZFT_ZIndexFile;     // setting ZFile_type
    _Base::setReservedContent(ZICB->_exportICB(wICBContent));
    wSt=_Base::_writeFullFileHeader(ZDescriptor,true);
 //   wSt=_Base::updateReservedBlock(ZICB->_exportICB());
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast (" Writing Reserved header zone ZIndexFile %s",IndexUri.toString());
                return wSt;
                }
    return ZS_SUCCESS;
}//writeIndexControlBlock




/*
ZStatus
ZIndexFile::removeRollback         (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress)
{
    return (addIndexValue(pRecord,pZMFRank,pAddress));
}
ZStatus
ZIndexFile::insertRollback         (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress)
{
    return (removeIndexValue(pRecord,pZMFRank,pAddress));
}
*/
ZStatus
ZIndexFile::removeIndexValue        (const ZDataBuffer& pKey , zaddress_type &pAddress)
{
ZStatus         wSt;

ZIndexCollection wZIRList(this);
//zaddress_type   wZMFAddress;
//long            wIndexRank;
ZIndexResult wZIR;

    if (ZICB->Duplicates == ZST_DUPLICATES)
            {
            wSt=_searchAll(pKey,*this,wZIRList,ZMS_MatchIndexSize);
            if (wSt!=ZS_FOUND)
                        return wSt;
            wZIR.IndexRank = -1;
            if (pAddress==-1)       // if no ZMF address specified get the first one to be removed
                        {
 //                       wZMFAddress= ZMFAddressList[0];
 //                       wIndexRank = wIndexRankList[0];
                        wZIR = wZIRList[0]  ;
                        }
                else
                    {
                wZIR.IndexRank = -1;
            for (long wi=0;wi<wZIRList.size(); wi++)
                if (wZIRList[wi].ZMFAddress == pAddress )
                            {
                                            wZIR= wZIRList[wi];
                                            break;
                            }
            if (wZIR.IndexRank<0)
                        {
                        ZException.setMessage(_GET_FUNCTION_NAME_,
                                                ZS_INVADDRESS,
                                                Severity_Error,
                                                " given MasterFile's Address %ld has not been matched in index <%s>",
                                                pAddress,
                                                ZICB->Name.toString()
                                                );
                            return (ZS_INVADDRESS); // pAddress has not been matched
                        }
                    } // else
            }// if duplicates
            else
            {   // meaning No duplicates
//        wSt=_search(pKey,*this,wZIR,ZMS_MatchIndexSize);
        wSt=_search(pKey,*this,wZIR);
        if (wSt!=ZS_FOUND)
                    return wSt;
            }
// At this stage we have one ZIR with the IndexRank to remove within ZIndexFile

    return (_Base::zremove(wZIR.IndexRank));
}




/**
 * @brief ZIndexFile::_addKeyValue Adds a key value from a ZMasterFile record to the current registrated ZIndexFile instance.
 *
       - Prepare
       - Commit

@warning not to be used in a normal mode


 * @param pZMFRecord
 * @param pIndexRank
 * @param pZMFAddress
 * @param pStats
 * @return
 */
ZStatus
ZIndexFile::_addKeyValue(ZDataBuffer &pZMFRecord,  long& pIndexRank, zaddress_type pZMFAddress)
{
long ZJoinIndex;
ZStatus wSt;

ZOp wZIndexOp;  // for journaling & history purpose

ZIndexItem* wIndexItem = new ZIndexItem ;

long wIndexIdxCommit;

    wSt=_addKeyValue_Prepare(pZMFRecord,wIndexItem,wIndexIdxCommit,pZMFAddress);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    wSt= _addKeyValue_Commit(wIndexItem,wIndexIdxCommit);
    delete wIndexItem;
    return wSt;
}// _addKeyValue





#ifdef __COMMENT__
ZStatus
ZIndexFile::_addKeyValue(ZDataBuffer &pElement,  long& pIndexRank, zaddress_type pZMFAddress)
{
long ZJoinIndex;
ZStatus wSt;
ZResult wRes;
ZOp wZIXOp;
ZIndexItem_struct wZI ;
zaddress_type wAddress;

/**
  * get with seekGeneric the ZIndexFile row position to insert
  *
  *  1-insert accordingly (push_front, push , insert)
  *
  *  2- shift all ZAM indexes references according pZAMIdx value : NB We do not care about that : only addresses are stored, not the order
  *
  */
    wZI.ZMFaddress = pZMFAddress ;
    wZI.State = ZAMInserted ;

    _keyValueExtraction(ZICB,pElement,wZI.KeyContent);

    wRes= _seekZIFGeneric (wZI.KeyContent,*this,ZICB,ZKeyCompareBinary);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
    switch (wRes.ZSt)
            {
            case (ZS_OUTBOUNDLOW):
                {
                wZIXOp=ZO_Push_front ;
//                ZJoinIndex=0;
                if ((wSt=_Base::_insert(_Base::ZDescriptor,wZI.toFileKey(),0L,wAddress))!=ZS_SUCCESS)  //! equivalent to push_front
                                        {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                        _Mtx.unlock();
#endif
                                        return(wSt); // error is managed at ZMF level
                                        }
                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                wZIXOp=ZO_Push ;
//                ZJoinIndex=this->size();
                if ((wSt=_Base::_add(_Base::ZDescriptor,wZI.toFileKey(),wAddress))!=ZS_SUCCESS)  //! equivalent to push
                                        {

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                        _Mtx.unlock();
#endif
                                        return(wSt); // error is managed at ZMF level
                                        }
                break;
                }
            case (ZS_NOTFOUND):
                {
                wZIXOp=ZO_Insert ;
                if ((wSt=_Base::_insert(_Base::ZDescriptor,wZI.toFileKey(),wRes.ZIdx,wAddress))!=ZS_SUCCESS)     // insert at position returned by seekGeneric
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return(wSt); // error is managed at ZMF level
                            }
//                ZJoinIndex=wRes.ZIdx;
                break;
                }
            case (ZS_FOUND):
                {
                if (ZICB->Duplicates==ZST_NODUPLICATES)
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return (ZS_DUPLICATEKEY);
                            }
                wZIXOp=ZO_Insert ;
                if ((wSt=_Base::_insert(_Base::ZDescriptor,wZI.toFileKey(),wRes.ZIdx,wAddress))!=ZS_SUCCESS)     // insert at position returned by seekGeneric
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                             _Mtx.unlock();
#endif
                            return(wSt); // error is managed at ZAM level
                            }
//                ZJoinIndex=wRes.ZIdx;
                break;
                }
            default :
                {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                 _Mtx.unlock();
#endif
                return(ZS_INVOP);
                }
            }// switch

/*
    if (FConnectedJoin)
    {
    for (long wi=0; wi< ZJDDescriptor.size();wi++)
                    {
                    ZJDDescriptor[wi]->_add(ZJoinIndex);  //! gives the pointer to the key value within the ZArray
                    }
     }

*/
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    return(ZS_SUCCESS);

}//ZIndexFile::_addKeyValue
#endif //__COMMENT__

//------------------Add sequence-----------------------------------------


/**
 * @brief ZIndexFile::_addKeyValue_Prepare prepare to add an index rank. This will be committed or rolled back later using _addKeyValue_Commit or _Rollback (or HardRollback)

@par keyValueExtraction :
   - extracts fields values according their definition in CZKeyFieldList class (ZType, offset, length)
   - formats data : puts data field content into appropriate key format taking into account :
    + signed data (leading byte)
    + reverse byte order if required (if and only if system is little endian compliant)
   - concatenate data in fields order given by their position within CZKeyFieldList array ( rank 0 is first )
     . seek for extracted key value : reject if duplicate collision
     . prepare (ZRandomFile) to insert key appropriately (push, push_front or insert at given rank)



 * @param[in] pZMFRecord user's record content to extract key value from
 * @param[out] pIndexItem a pointer to ZIndexItem struct that will contain the Index element to be added
 * @param[out] pIndexIdxCommit
 * @param[in] pZMFAddress The ZMF block address to set the index rank with
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexFile::_addKeyValue_Prepare(ZDataBuffer &pZMFRecord,
                                 ZIndexItem *&pIndexItem,
                                 long& pIndexCommit,
                                 const zaddress_type pZMFAddress)
{
long ZJoinIndex;
ZStatus wSt;
//zaddress_type wZMFAddress;
//zaddress_type wIndexAddress; // not used but necessary for base ZRandomFile class calls
ZIndexResult wZIR;

ZIFCompare wZIFCompare = ZKeyCompareBinary;


//-----------Comparison routine selection------------------------------------

    if (ZICB->ZKDic->size()==1)           // if only one field
        {
        if (ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison


// get with seekGeneric the ZIndexFile row position to insert

//  1-insert accordingly (push_front, push , insert)

//  2- shift all ZAM indexes references according pZAMIdx value :
//  NB We do not care about that : only addresses are stored, not the order

    pIndexItem->clear();
    pIndexItem->Operation = ZO_Add;
    pIndexItem->ZMFaddress = pZMFAddress ;  // store address to ZMF Block
//    pKeyValue.State = ZAMInserted ;

    _keyValueExtraction(*ZICB->ZKDic,pZMFRecord,pIndexItem->KeyContent);        // create key value content from user record

//    wSt= _search (pIndexItem->KeyContent,*this,wZIR,ZMS_MatchIndexSize,wZIFCompare);  // will not use wStats except for special seek mesure on add
    wSt= _search (pIndexItem->KeyContent,*this,wZIR,ZLock_Nolock);  // will not use wStats except for special seek mesure on add

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
    if (ZVerbose)
            {
            _DBGPRINT ("_addKeyValue_Prepare : _search return status <%s> rank <%ld>\n", decode_ZStatus(wSt),wZIR.IndexRank);
            }

    switch (wSt)
            {
            case (ZS_OUTBOUNDLOW):
                {
                pIndexItem->Operation=ZO_Push_front ;
//                ZJoinIndex=0;
                wSt=_insert2PhasesCommit_Prepare (_Base::ZDescriptor,pIndexItem->toFileKey(),0L,pIndexCommit,wZIR.ZMFAddress);//! equivalent to push_front
                if (ZVerbose)
                        _DBGPRINT ("Index Push_Front  (index rank 0L )\n");

                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                pIndexItem->Operation=ZO_Push ;
//                ZJoinIndex=this->size();
                wSt=_Base::_add2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),pIndexCommit,wZIR.ZMFAddress);//! equivalent to push
                if (ZVerbose)
                        _DBGPRINT ("Index Push\n");
                break;
                }
            case (ZS_NOTFOUND):
                {
                pIndexItem->Operation=ZO_Insert ;
                wSt=_Base::_insert2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),wZIR.IndexRank,pIndexCommit,wZIR.ZMFAddress);// insert at position returned by seekGeneric
//                ZJoinIndex=wRes.ZIdx;
                if (ZVerbose)
                        _DBGPRINT ("Index insert at rank <%ld>\n", wZIR.IndexRank);
                break;
                }
            case (ZS_FOUND):
                {
                if (ZICB->Duplicates==ZST_NODUPLICATES)
                            {
                            if (ZVerbose)
                                    _DBGPRINT("***Index Duplicate key exception at rank <%ld>\n", wZIR.IndexRank);
                            ZException.setMessage(_GET_FUNCTION_NAME_,
                                                    ZS_DUPLICATEKEY,
                                                    Severity_Error,
                                                    " Duplicate value on key set to ZST_NODUPLICATES index rank <%ld>",wZIR.IndexRank);
                            wSt=ZS_DUPLICATEKEY;
                            break;
                            }
                pIndexItem->Operation=ZO_Insert ;

                if (ZVerbose)
                        _DBGPRINT ("Index Duplicate key insert at rank <%ld>\n", wZIR.IndexRank);

                wSt=_Base::_insert2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),wZIR.IndexRank,pIndexCommit,wZIR.ZMFAddress); // insert at position returned by seekGeneric

//                ZJoinIndex=wRes.ZIdx;
                break;
                }
            default :// there was an error or a lock on file during seek operation on given key
                {
//                wSt=ZS_INVOP;
                break;
                }
            }// switch

    if (wSt!=ZS_SUCCESS)
                {
                goto _addKeyValuePrepareReturn;     // not necessary for the moment but RFFU
                return(wSt); // error is managed at ZMF level
                }

/*
    if (FConnectedJoin)
    {
    for (long wi=0; wi< ZJDDescriptor.size();wi++)
                    {
                    ZJDDescriptor[wi]->_add(ZJoinIndex);  //! gives the pointer to the key value within the ZArray
                    }
     }

*/
_addKeyValuePrepareReturn:
    if (wSt!=ZS_SUCCESS)
        if (!ZException.stackIsEmpty())
            ZException.addToLast(" during Index _addKeyValue_Prepare on index <%s> ",
                                                 ZICB->Name.toString());
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                _Mtx.unlock();
#endif

// statistical data is managed outside

/*    pStats.Timer.end();
    pStats.Timer.getDeltaTime();
    pStats = ZDescriptor.ZPMS- wPMS ;  // just give the delta : ZRFPMS is a base for zstatistics
    ZPMSStats += pStats;*/
    return (wSt) ;

}//_addKeyValue_Prepare


ZStatus
ZIndexFile::_addKeyValue_Commit(ZIndexItem *pIndexItem,const long pIndexCommit)
{
ZStatus wSt;
zaddress_type wAddress; // local index address : of no use there

    wSt=_Base::_add2PhasesCommit_Commit(_Base::ZDescriptor,pIndexItem->toFileKey(),pIndexCommit,wAddress);
    if (wSt!=ZS_SUCCESS)
            ZException.addToLast(" during Index _addKeyValue_Commit on index <%s> rank <%02ld> ",
                                                 ZICB->Name.toString(),
                                                 pIndexCommit);
// history and journaling take place here

    return wSt;
} // _addKeyValue_Commit

ZStatus
ZIndexFile::_addKeyValue_Rollback(const long pIndexCommit)
{
ZStatus wSt;
    wSt=_Base::_add2PhasesCommit_Rollback (_Base::ZDescriptor,pIndexCommit);
    if (wSt!=ZS_SUCCESS)
            ZException.addToLast(" during Index _addKeyValue_Rollback (Soft rollback) on index <%s> rank <%02ld> ",
                                                 ZICB->Name.toString(),
                                                 pIndexCommit);

// No history and no journaling for indexes

    return wSt;
} // _addKeyValue_Rollback

/**
 * @brief ZIndexFile::_addKeyValue_HardRollback routine used when index key add operation has been fully committed and a problem occurred on Master file

    So there is need to remove key Index rank at pIndexCommit to restore situation as aligned with Master file.


 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZIndexFile::_addKeyValue_HardRollback(const long pIndexCommit)
{
    if (ZVerbose)
            fprintf (stdout,"Index addKeyValue Hard rollback : removing index <%s> rank <%ld>\n",
                     ZICB->Name.toString(),
                    pIndexCommit);

ZStatus wSt =_Base::_remove(_Base::ZDescriptor,pIndexCommit);
    if (wSt!=ZS_SUCCESS)
            ZException.addToLast(" during Index _addKeyValue_HardRollback (hard rollback) on index <%s> rank <%02ld> ",
                                                 ZICB->Name.toString(),
                                                 pIndexCommit);

   return wSt;
} // _addKeyValue_HardRollback

//------------------End Add sequence-----------------------------------------

//------------------Remove sequence-----------------------------------------


/**
 * @brief ZIndexFile::_removeKeyValue_Prepare  Prepares to remove an index rank corresponding to given pKey (ZDataBuffer)
 *
 * Returns
 * - Index key relative position within ZIndexFile if key content is found
 * - the corresponding Address within Master file if found
 * - a ZIndexItem (pointer to) that is generated inside the module
 *
 * Returns following status
 *  - ZS_SUCCESS if operation is successfull
 *  - ZS_NOTFOUND or appropriate not found status if key is not found
 *  - lock status if index rank has been locked
 *  - other internal status in case of IO error
 *@see ZStatus
 *
 *  In all error cases, ZException is set with an appropriate message describing error.
 *
 * @param[in] pKey key content with a ZIndex format. @see _keyValueExtraction()
 * @param[out] pIndexItem a pointer to a ZIndexItem : this object is generated using new ZIndexItem
 * @param[out] pIndexRank logical position within ZIndexFile for the key content
 * @param[out] pZMFAddress Physical address within ZMasterFile for the block corresponding to key value
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZIndexFile::_removeKeyValue_Prepare(ZDataBuffer & pKey,
                                    ZIndexItem* &pIndexItem,
                                    long& pIndexRank,
                                    zaddress_type &pZMFAddress)
{
long ZJoinIndex;
ZStatus wSt;
ZResult wRes;
//zaddress_type wIndexAddress;
ZIndexResult wZIR;

//ZIFCompare         wZIFCompare = ZKeyCompareBinary;

//-----------Comparison routine selection------------------------------------


//    wSt=_search(pKey, *this,wZIR,ZMS_MatchIndexSize,wZIFCompare);
    wSt=_search(pKey, *this,wZIR,ZLock_Nolock);
    if (wSt!=ZS_FOUND)  // return status is either not found, record lock or other error
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    wRes.ZSt,
                                    Severity_Error,
                                    "During remove operation : Index value not found on index name <%s>",
                                     ZICB->Name.toString());
            return wSt;
            } // wSt!=ZS_FOUND

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif

    pIndexRank=wZIR.IndexRank ;

    pIndexItem = new ZIndexItem ;
    pIndexItem->Operation=ZO_Erase ;
    pIndexItem->KeyContent = pKey;
    pIndexItem->ZMFaddress = 0 ; // dont know yet

    wSt=_Base::_remove_Prepare(_Base::ZDescriptor,wZIR.IndexRank,wZIR.ZMFAddress);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                        _Mtx.unlock();
#endif

    return wSt;
}//_removeKeyValue_Prepare


ZStatus
ZIndexFile::_removeKeyValue_Commit(const long pIndexCommit)
{
long ZJoinIndex;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif


    wSt=_Base::_remove_Commit(_Base::ZDescriptor,pIndexCommit);
    if (wSt!=ZS_SUCCESS)
        ZException.addToLast(" during removeKeyValue_Commit on index <%s> number <%02ld> ",
                                             ZICB->Name.toString(),
                                             pIndexCommit);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                        _Mtx.unlock();
#endif
     return(wSt);

}//ZIndexFile::_removeKeyValue_Prepare
/**
 * @brief ZIndexFile::_removeKeyValue_Rollback
 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexFile::_removeKeyValue_Rollback(const long pIndexCommit)
{
long ZJoinIndex;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif


    wSt=_Base::_remove_Rollback(_Base::ZDescriptor,pIndexCommit);
    if (wSt!=ZS_SUCCESS)
        ZException.addToLast(" during removeKeyValue_Rollback (soft rollback) on index <%s> number <%02ld> ",
                                             ZICB->Name.toString(),
                                             pIndexCommit);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                        _Mtx.unlock();
#endif
     return wSt;

}//_removeKeyValue_Rollback

/**
 * @brief ZIndexFile::_removeKeyValue_HardRollback
 * @param pIndexItem
 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexFile::_removeKeyValue_HardRollback(ZIndexItem* pIndexItem, const long pIndexCommit)
{
long ZJoinIndex;
zaddress_type wAddress;  // not used : only for compatibility purpose

ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif

    wSt= _Base::_insert(_Base::ZDescriptor,pIndexItem->toFileKey(),pIndexCommit,wAddress);
    if (wSt!=ZS_SUCCESS)
        ZException.addToLast(" during _removeKeyValue_HardRollback (hard rollback) on index <%s> number <%02ld> ",
                                             ZICB->Name.toString(),
                                             pIndexCommit);

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                        _Mtx.unlock();
#endif
     return wSt ;

}//_removeKeyValue_HardRollback

#ifdef __FORMER__

//!
//!----------------ZIndexFile expanded methods---------------------------------------
//!


void
ZIndexFile::_register (void)
{
/**
  * Nota Bene: ZIXDescriptor must remain WITHIN ZIndexFile object and used from outside when needed thru pointer  (otherwise: SIGSEGV exception thrown).
  */
    ZIFDescriptor._add      = std::bind(&ZIndexFile::_add, this,_1,_2,_3);
    ZIFDescriptor._erase    = std::bind(&ZIndexFile::_erase, this,_1);
    ZIFDescriptor._replace  = std::bind(&ZIndexFile::_replace, this,_1,_2);
    ZIFDescriptor._askMe    = std::bind(&ZIndexFile::_askMe, this);
    ZIFDescriptor._ask      = std::bind(&ZIndexFile::_ask, this,_1,_2,_3);
    ZIFDescriptor._reset    = std::bind(&ZIndexFile::_reset, this,_1);
    ZIFDescriptor.CommitUserFunction =nullptr;
    ZIFDescriptor.CommitArguments =nullptr;

    ZIndexFileId    =   ZMFFather->_register(&ZIFDescriptor);

    if (ZIndexFileId<0)
            {
            fprintf(stderr,"ZIndexFile::_register-F-IVZIndexFileIDX Invalid ZIndexFileIdx value returned by ZAM.\n");
            ZIXAbort();
            }

    return;
}//register


template < class _Type,class _Bulk,class _KType,class _KCompare>
ZStatus
ZIndexFile:: _rebuild(void)
{
ZStatus wSt=ZS_SUCCESS;

    this->setQuota(ZMFFather->getQuota());
    this->setAllocation(ZMFFather->getAllocation());

    this->reset();

#ifdef __USE_ZTHREAD__
    ZMFFather->ZAMlock(); // during an Index rebuild lock ZAM whatever ZThread mode is if using ZThreads
#endif

    for (size_t wi=0;wi<ZMFFather->size();wi++)
                    {
                    wSt = _addRebuild(ZMFFather->Tab[wi],wi);
#ifdef __USE_ZTHREAD__
                    ZMFFather->ZMFunlock();
#endif
                    if(wSt!=ZS_SUCCESS)
                            {
#if __DEBUG_LEVEL__ > __SEVEREREPORT__
                            _KType wKey ;
                            char wBuf[100];
                            memmove(&wKey,&((_Bulk &)ZMFFather->Tab[wi])._Key ,sizeof(_KType));

                            fprintf(stderr,"ZIX::_rebuild-S-ADDBADSTAT Severe error while building or rebuilding ZIndexFile id <%ld> at ZAM row id <%ld>.\n"
                                    "status is <%s> Key content is <%s>\n",
                                    ZIndexFileId,
                                    wi,
                                    decode_ZStatus(wSt),
                                    _printFieldValue<_KType>(wKey,wBuf));
#endif
                            return(wSt);
                            }

                     }
 //   _rebuildReverse();
    return(wSt);

}//ZIX::_rebuild

//
//-------------ZIndexFile overloaded base methods----------------------------------
//


//
//-------------ZIndexFile update----------------------------------
//



//
//-------------journaling----------------------------------
//

//
//-------------ZIndexFile expanded methods----------------------------------
//
//
//


ZStatus ZIndexFile::_addRebuild(ZDataBuffer &pElement,size_t pZAMIdx)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;
/**
  * get with seekGeneric the ZIndexFile row position to insert
  *
  *  1-insert accordingly (push_front, push , insert)
  *
  *  2- shift all ZAM indexes references according pZAMIdx value
  *
  */
    wZI.ZAMIdx = pZAMIdx ;
    wZI.State = ZAMInserted ;
    size_t wZI_Idx;
    memmove(&wZI._Key,&((_Bulk &)pElement)._Key ,sizeof(_KType));

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (wZI._Key,*this);
    wZI_Idx=wRes.ZIdx;
    switch (wRes.ZSt)
            {
            case (ZS_OUTBOUNDLOW):
                {
                if (_Base::push_front(wZI)<0)
                            return (ZS_OUTBOUNDHIGH);
                wZI_Idx=0;
                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                if (_Base::push(wZI)<0)
                            return (ZS_OUTBOUNDHIGH);
                wZI_Idx=this->lastIdx();
                break;
                }
            case (ZS_NOTFOUND):
                {
                if (_Base::insert(wZI,wRes.ZIdx)<0)     // insert at position returned by seekGeneric
                            {
                            return (ZS_NOTFOUND);
                            }
                wZI_Idx=wRes.ZIdx;
                break;
                }
            case (ZS_FOUND):
                {
                if (ZIDuplicates==ZST_NODUPLICATES)
                            {
                            return (ZS_DUPVIOLATION);
                            }
                if (_Base::insert(wZI,wRes.ZIdx)<0)
//                            return ((ZStatus)(ZS_ERROR|ZS_SECONDARY));
                            return (ZS_OUTBOUND);
                wZI_Idx=wRes.ZIdx;
                break;
                }
            default :
                {
//                return((ZStatus)(ZS_INVOP|ZS_SECONDARY));
                return(ZS_INVOP);
                }
            }// switch


    return(ZS_SUCCESS);
}//_addRebuild



ZStatus ZIndexFile::_erase (size_t pZAMIdx)
{
    ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.lock();
#endif
    long wZIXIdx = _lookupZAMIdx(pZAMIdx);

    if (wZIXIdx<0)
                    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                    _Mtx.unlock();
#endif
                    return(ZS_OUTBOUNDHIGH);      // error is managed at ZAM level
                    }
    if ((wSt=_alignZIndexFile(ZO_Erase,ZO_Erase,pZAMIdx,wZIXIdx))!=ZS_SUCCESS)
                        {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.unlock();
#endif
                        return(wSt);      // error is managed at ZAM level
                        }

    if (_Base::erase(wZIXIdx)<0)
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return(ZS_OUTBOUNDHIGH);      // error is managed at ZAM level
                            }
    if (FConnectedJoin)
    {
    for (long wi=0; wi< ZJDDescriptor.size();wi++)
                    {
                    ZJDDescriptor[wi]->_erase(wZIXIdx);
                    }
     }
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    return(ZS_SUCCESS);
}//ZIX::_erase


long ZIndexFile::_lookupZAMIdx (size_t pZAMIdx)
{
    for (long wi=0;wi<this->size();wi++)
                    {
                    if (pZAMIdx==this->Tab[wi].ZAMIdx)
                                return (wi);
                    }
    return(-1);
}


ZStatus ZIndexFile::_replace (ZDataBuffer &pElement,size_t pZAMIdx)
{

    long wZIXIdx = _lookupZAMIdx(pZAMIdx);
    if (wZIXIdx<0)
            {
            fprintf (stderr,"ZIX::_replace-F-ZIXCORR Fatal error Index id <%ld> corrupted  at ZAM rank <%ld> \n",
                     ZIndexFileId, pZAMIdx);
            ZIXAbort();
            }

    //!
    //! if key value remains identical : no need of change
    //!
    //!
    _KType *wKey =&((_Bulk&)pElement)._Key;
    if (_KCompare::_compare(wKey,                           // get Key from _Bulk recasted from _Type
                            &this->Tab[ wZIXIdx]._Key       // get key value corresponding to reverse index search
                            )  ==0 )                        //could be equal

                               return(ZS_SUCCESS);                 // if so, return everything is done.

ZStatus wSt;

// if not : erase corresponding ZIX using _erase method and insert using _add method

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif

    ZIndexFile_struct<_KType> wZIX_bck ;
    memmove(&wZIX_bck,&this->Tab[wZIXIdx], sizeof(wZIX_bck));
    long w1=_Base::erase(wZIXIdx);
/*    wSt=_erase (pZAMIdx);
    if (wSt!=ZS_SUCCESS)
*/
    if (w1<0)
            {
                        wSt=ZS_OUTBOUND;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.unlock();
#endif
                        return (wSt);
                }
    wSt=_add(&pElement,ZO_Insert, pZAMIdx);
    if (wSt!=ZS_SUCCESS)
                {           // manual restore value
                w1=_Base::insert(wZIX_bck,wZIXIdx);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                _Mtx.unlock();
#endif
                if (w1<0)
                        {
                        char wBuf[200];
                        fprintf(stderr,"ZIX::_replace-F-RBCKOUTBOUND  Fatal error : out of boundaries while rolling back ZIX rank (_Base::insert) ZIX id <%ld>.\n"
                                       "Index rank <%ld>  key value <%s>\n",
                                ZIndexFileId,
                                wZIXIdx,
                                _printFieldValue<_KType>(wZIX_bck._Key,wBuf));
                        ZIXAbort();
                        }
                return (wSt);
                }

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.unlock();
#endif
    return (wSt);
}//ZIX::_replace



ZStatus ZIndexFile::_add(ZDataBuffer &pElement, ZOp ZAMOp, size_t pZAMIdx)
{
long ZJoinIndex;
ZResult wRes;
ZOp wZIXOp;
ZIndexFile_struct<_KType> wZI ;
/**
  * get with seekGeneric the ZIndexFile row position to insert
  *
  *  1-insert accordingly (push_front, push , insert)
  *
  *  2- shift all ZAM indexes references according pZAMIdx value
  *
  */
    wZI.ZAMIdx = pZAMIdx ;
    wZI.State = ZAMInserted ;
    memmove(&wZI._Key,&((_Bulk*)pElement)->_Key ,sizeof(_KType));

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (wZI._Key,*this);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
    switch (wRes.ZSt)
            {
            case (ZS_OUTBOUNDLOW):
                {
                wZIXOp=ZO_Push_front ;
                ZJoinIndex=0;
                if ((wRes.ZSt=_alignZIndexFile(ZAMOp,wZIXOp,pZAMIdx,wRes.ZIdx))!=ZS_SUCCESS)
                                                                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                                                            _Mtx.unlock();
#endif
                                                                            return(wRes.ZSt);//! possible ZS_INVOP
                                                                            }
                _Base::push_front(wZI) ; // push_front has no error (or memory violation SIGSEGV)

                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                wZIXOp=ZO_Push ;
                ZJoinIndex=this->size();
                if ((wRes.ZSt=_alignZIndexFile(ZAMOp,wZIXOp,pZAMIdx,wRes.ZIdx))!= ZS_SUCCESS)
                                                {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                                _Mtx.unlock();
#endif
                                                return(wRes.ZSt);//! possible ZS_INVOP
                                                }
                _Base::push(wZI);  // ZArray::push has no error case except internal memory violation SIGSEGV
                break;
                }
            case (ZS_NOTFOUND):
                {
                wZIXOp=ZO_Insert ;
                if ((wRes.ZSt=_alignZIndexFile(ZAMOp,wZIXOp,pZAMIdx,wRes.ZIdx))!= ZS_SUCCESS)
                                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                            _Mtx.unlock();
#endif
                                            return(wRes.ZSt);//! possible ZS_INVOP
                                            }
                if (_Base::insert(wZI,wRes.ZIdx)<0)     // insert at position returned by seekGeneric
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return(ZS_OUTBOUNDHIGH); // error is managed at ZAM level
                            }
                ZJoinIndex=wRes.ZIdx;
                break;
                }
            case (ZS_FOUND):
                {
                if (ZIDuplicates==ZST_NODUPLICATES)
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return (ZS_DUPLICATEKEY);
                            }
                wZIXOp=ZO_Insert ;
                if ((wRes.ZSt=_alignZIndexFile(ZAMOp,wZIXOp,pZAMIdx,wRes.ZIdx))!= ZS_SUCCESS)
                                                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                                            _Mtx.unlock();
#endif
                                                            return(wRes.ZSt);//! possible ZS_INVOP
                                                            }
                if (_Base::insert(wZI,wRes.ZIdx)<0)
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                             _Mtx.unlock();
#endif
                            return(ZS_OUTBOUNDHIGH); // error is managed at ZAM level
                            }
                ZJoinIndex=wRes.ZIdx;
                break;
                }
            default :
                {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                 _Mtx.unlock();
#endif
                return(ZS_INVOP);
                }
            }// switch


    if (FConnectedJoin)
    {
    for (long wi=0; wi< ZJDDescriptor.size();wi++)
                    {
                    ZJDDescriptor[wi]->_add(ZJoinIndex);  //! gives the pointer to the key value within the ZArray
                    }
     }


#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    return(ZS_SUCCESS);

}//ZIX::_add



ZStatus
ZIndexFile::_alignZIndexFile(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pZIXIdx)
{
//! NB: ZO_Replace has no impact on ZReverse : ZAM index remains the same NO NO NO  Replace may impact on Key : _erase then _add

    switch (pZAMOp)
       {
        case (ZO_Push):
            {
            break;
            }
    case (ZO_Push_front):
            {
            for (long wi=0;wi<this->size();wi++)
                            this->Tab[wi].ZAMIdx++;
            break;
            }
     case (ZO_Add):
     case (ZO_Insert):
            {
        for (long wi=0;wi<this->size();wi++)
                    {
                    if (this->Tab[wi].ZAMIdx>=pZAMIdx)
                                this->Tab[wi].ZAMIdx++;
                    }
            }
    case (ZO_Pop):
            {
            break;
            }
    case (ZO_Pop_front):
            {
            for (long wi=0;wi<this->size();wi++)
                            this->Tab[wi].ZAMIdx--;
            break;
            }
    case (ZO_Erase):
           {
       for (long wi=0;wi<this->size();wi++)
                   {
                   if (this->Tab[wi].ZAMIdx>=pZAMIdx)
                                    this->Tab[wi].ZAMIdx--;
                   }
       break;
           }
    default:
            {
            return(ZS_INVOP);
            }
       }//switch
    return(ZS_SUCCESS);
} //_alignZIndexFile



//! ------------external ZIndexFile expanded methods----------------------------
//!
//!         These methods are at disposal of user
//!
//!
//!

//! @brief add external routine to add a row within ZAM father using ZIndexFile : it calls ZAM::push method
//!
ZStatus ZIndexFile::add(_Type &pElement)
{
    return (ZMFFather->push(pElement));
} // add



ZStatus ZIndexFile::remove(_KType &pKey)
{
ZResult wRes;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFFather->ZAMlock();
#endif

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    if (wRes.ZSt!= ZS_FOUND)
               {
//        wRes.ZSt=(ZStatus)(wRes.ZSt | ZS_SECONDARY);
        if (ZMFFather->getHistoryStatus())
                {
        if((wSt=ZMFFather->ZJournaling->journalize(ZIndexFileId,             //  index operation
                                ZO_Erase,
                                ZAMErrDelete,
                                nullptr,  // no row to store
                                -1,       // no size of row
                                wRes.ZIdx,
                                wRes.ZSt))!=ZS_SUCCESS)
                                {
                                fprintf(stderr,"ZMasterFile::erase-F-CANTJOURNALIZE Fatal error while journalizing row operation. Status is %s \n"
                                        "Aborting ",
                                        decode_ZStatus(wSt));    // need to be updated later
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                ZMFFather->ZMFunlock();
#endif
                               ZIXAbort();
                                } // journalize

                ZMFFather->ZHistory->_historizeErroredLastJournalEvent(
                                                        ZMFFather->ZJournaling,
                                                        wRes.ZSt,
                                                        ZAMErrDelete ,
                                                        ZIndexFileId,
                                                        -1);
                     } // if  HistoryOn
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                ZMFFather->ZMFunlock();
#endif
               return (wRes.ZSt);
               } // if  error

    wRes.ZSt=ZMFFather->erase(this->Tab[wRes.ZIdx].ZAMIdx);


#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFFather->ZMFunlock();
#endif
    return(wRes.ZSt);
}// ZIX::remove


ZStatus ZIndexFile::_remove(_KType &pKey)
{
ZResult wRes;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFFather->ZAMlock();
#endif

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = wRes.ZSt;
    if (wRes.ZSt!= ZS_FOUND)
               {
//        wRes.ZSt=(ZStatus)(wRes.ZSt | ZS_SECONDARY);
        if (ZMFFather->getHistoryStatus())
                {
        if((wSt=ZMFFather->_ZIXHistorizeError(ZIndexFileId,             //  index operation
                                ZO_Erase,
                                ZAMErrDelete,
                                nullptr,  // no row to store
                                -1,       // no size of row
                                wRes.ZIdx,
                                wRes.ZSt))!=ZS_SUCCESS)
                                {
                                fprintf(stderr,"ZMasterFile::erase-F-CANTJOURNALIZE Fatal error while journalizing row operation. Status is %s \n"
                                        " Please check if Journaling and Historization are correctly set to ON. Aborting... ",
                                        decode_ZStatus(wSt));    // need to be updated later
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                ZMFFather->ZMFunlock();
#endif
                                ZIXAbort();;
                                } // journalize

                     } // if  HistoryOn
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                ZMFFather->ZMFunlock();
#endif
               return (wRes.ZSt);
               } // if  error

    wRes.ZSt=ZMFFather->erase(this->Tab[wRes.ZIdx].ZAMIdx);


#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFFather->ZMFunlock();
#endif
    return(wRes.ZSt);
}// ZIX::_remove



/**
 * @brief ZIndexFile::removeAll
 *
 *                  Secondary index adjustement is treated from ZAM during erase processing
 *                  see ZIndexFile::_erase() method
 * @param pKey
 * @return
 */
ZStatus ZIndexFile::removeAll(const _KType &pKey)
{
    ZAMulti_Result wMRes;
    ZStatus wSt;
    #if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
    #endif
        _seekAllGenericZA<_KType,ZIndexFile_struct<_KType>,_KCompare> ((_KType&)pKey,*this,wMRes);
        PersistentStatus = wMRes.ZSt;
        if (wMRes.ZSt!= ZS_FOUND)
                   {
//                   wMRes.ZSt=_setSecondary(wMRes.ZSt);
    #if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                    _Mtx.unlock();
    #endif
                   return (wMRes.ZSt);
                   }
        while (wMRes.size()>0)
                {
                if ((wSt=ZMFFather->erase(this->Tab[wMRes.last()].ZAMIdx))!=ZS_SUCCESS)
                        {
    #if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.unlock();
    #endif
                        return(wSt);
                        }
                  for (long wi=0;wi<wMRes.size();wi++)
                                    if (wMRes[wi]>wMRes.last())
                                                    wMRes[wi]--;
                  wMRes.pop();
                }// while
    #if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
    #endif
        return(ZS_SUCCESS);
}//  ZIX::removeAll


ZStatus ZIndexFile::_removeAll(_KType &pKey)
{
ZMulti_Result wMRes;
ZStatus wSt;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.lock();
#endif
    _seekAllGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this,&wMRes);
    PersistentStatus = wMRes.ZSt;
    if (wMRes.ZSt!= ZS_FOUND)
               {
//               wMRes.ZSt=_setSecondary(wMRes.ZSt);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                _Mtx.unlock();
#endif
               return (wMRes.ZSt);
               }
    while (wMRes.ZTabIdx->size()>0)
            {
            if ((wSt=ZMFFather->erase(this->Tab[wMRes.ZTabIdx->Tab[0]].ZAMIdx))!=ZS_SUCCESS)
                    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                    _Mtx.unlock();
#endif
                    return(wSt);
                    }
             wMRes.ZTabIdx->pop_front();

             if (wMRes.ZTabIdx->size()>0)                               //! Caution : to be validated : NO CONCURRENT ACCESS during removeALL
                        wMRes.ZTabIdx->Tab[0]--; //! whole Index must be LOCKED : this is not done

            }// while
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    return(ZS_SUCCESS);
}//  ZIX::_removeAll



/**
 * @brief ZIndexFile::seek returns the first index of ZAM corresponding to the first value matching secondary Key value pKey
 * @param pKey
 * @return
 */
ZResult ZIndexFile::seek(_KType &pKey)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
    wRes.ZIdx=this->Tab[wRes.ZIdx].ZAMIdx;
    PersistentStatus = wRes.ZSt;
    return(wRes);
}//  ZIX::seek
/**
 * @brief ZIndexFile::seek returns the first index of ZIX corresponding to the first value matching secondary Key value pKey
 * @param pKey
 * @return
 */

ZResult ZIndexFile::_seekFirst(_KType &pKey)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = wRes.ZSt;
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
//    wRes.ZIdx=this->Tab[wRes.ZIdx].ZAMIdx;
    return(wRes);
}//  ZIX::_seekFirst

/**
 * @brief ZIndexFile::seek returns the first found ZAM row index  corresponding to the first value matching secondary Key value pKey within the ZIX index
 * @param pKey
 * @return
 */

ZResult ZIndexFile::seekFirst(_KType &pKey)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = wRes.ZSt;
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
    wRes.ZIdx=this->Tab[wRes.ZIdx].ZAMIdx;
    return(wRes);
}//  ZIX::_seekFirs



/**
 * @brief ZIndexFile::seek returns the first (ZIX) rank order of ZIndexFile corresponding to the first value matching secondary Key value pKey
 * @param pKey
 * @return
 */
ZResult ZIndexFile::_seek(ZDataBuffer &pKey)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = wRes.ZSt;
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
    return(wRes);
}// ZIX:: _seek


ZStatus ZIndexFile::seekAll(_KType pKey, ZMulti_Result &pMRes)
{

    _seekAllGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this,&pMRes);
    PersistentStatus = pMRes.ZSt;
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               return (pMRes.ZSt);
               }
    for (long wi =0;wi <pMRes.ZTabIdx->size();wi ++)
                pMRes.ZTabIdx->Tab[wi]= this->Tab[pMRes.ZTabIdx->Tab[wi]].ZAMIdx;

    return(pMRes.ZSt);
}//  ZIX::seekAll



/** ------------------------Uses ZAMulti_Result--------------------------------------------
 *
 *
 * @brief ZIndexFile::seekAll
 *              returns a collection of ZAM row ranks corresponding to all key values matching secondary Key values pKey
 * @param pKey
 * @return
 */

ZStatus ZIndexFile::seekAll(_KType pKey, ZAMulti_Result &pMRes)
{

    _seekAllGenericZA<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this,pMRes);
    PersistentStatus = pMRes.ZSt;
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               return (pMRes.ZSt);
               }
    for (long wi =0;wi <pMRes.size();wi ++)
                pMRes[wi]= this->Tab[pMRes[wi]].ZAMIdx;

    return(pMRes.ZSt);
}//  ZIX::seekAll



/**
 * @brief ZIX::seekAllJoinDyn returns a collection of raw ZIX row ranks corresponding to all key values matching secondary Key values pKey
 *             This is dedicated to ZJoinDyn usage
 * @param pKey
 * @return
 */
ZStatus ZIndexFile::_seekAllZJoinDyn(_KType &pKey, ZMulti_Result *pMRes)
{

    _seekAllGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this,pMRes);
    PersistentStatus = pMRes->ZSt;
    return(pMRes->ZSt);
}//  ZIX::seekAllJoinDyn



/**
 * @brief ZIndexFile::seekAllPartial Using ZAMulti_Result
 *          returns a collection of ZAM row ranks corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */
ZStatus ZIndexFile::seekAllPartial(_KType &pKey,size_t pSize, ZAMulti_Result &pMRes)
{
    size_t wj;
    _seekAllPartialGenericZA<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this,pMRes);
    PersistentStatus = pMRes.ZSt;
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               return (pMRes.ZSt);
               }
    for (long wi =0;wi <pMRes.size();wi ++)
                    {
                wj=pMRes[wi];
                pMRes.Tab[wi]= this->Tab[wj].ZAMIdx;
                    }
    return(pMRes.ZSt);
}// ZIX::seekAllPartial - using ZAMulti_Result


/**
 * @brief ZIndexFile::seekAllPartial returns a collection of ZAM row ranks corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */

ZStatus ZIndexFile::seekAllPartial(_KType pKey, size_t pSize, ZMulti_Result &pMRes)
{
    size_t wj;
    _seekAllPartialGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this,&pMRes);
    PersistentStatus = pMRes.ZSt;
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               return (pMRes.ZSt);
               }
    for (long wi =0;wi <pMRes.ZTabIdx->size();wi ++)
                    {
                wj=pMRes.ZTabIdx->Tab[wi];
                pMRes.ZTabIdx->Tab[wi]= this->Data.Tab[wj].ZAMIdx;
                    }
    return(pMRes.ZSt);
}// ZIX::seekAllPartial

/**
 *   Record operations : these ZIX functions return ZAM row content corresponding to ZIX index search for a Key (partial or full)
 */


/**
 * @brief seekRecord
 *                  Seeks ZIX index for pKey content
 *                  if search is successfull, returns :
 *                      - a pointer to ZAM row content
 *                      - the pointer to ZStatus variable is updated with the actual status value if this value has been given in input (not nullptr)
 *
 *                  if search is NOT successful :
 *                      - a nullptr value as return
 *                      - the pointer to ZStatus variable if given in input (different from nullptr) is updated with the current status
 * @param pKey
 * @param pRecord
 * @param pSt
 * @return
 */


const _Type *
ZIndexFile:: seekRecord (_KType pKey,      //! key content to search for
                                                      _Type &pRecord,    //! row buffer that will be returned (or not in case of error)
                                                      ZResult &pRes)      //!Index result structure if given (different from nullptr)
  {
    memset (&pRecord,0,sizeof(_Type));
    pRes =  _seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = pRes.ZSt;
    if (pRes.ZSt!=ZS_FOUND)
                {
//                *pRes=wRes;
                return nullptr;
                }

    memmove (&pRecord,&ZMFFather->Tab[pRes.ZIdx],sizeof(_Type));  //! copy rank to buffer given in input
    return (&ZMFFather->Tab[pRes.ZIdx]);        //! return pointer to this rank
  }//seekRecord



/**
 * @brief seekPartialRecord
 *                  Seeks ZIX index for pKey content
 *                  if search is successfull, returns :
 *                      - a pointer to ZAM row content
 *                      - the pointer to ZStatus variable is updated with the actual status value if this value has been given in input (not nullptr)
 *
 *                  if search is NOT successful :
 *                      - a nullptr value as return
 *                      - the pointer to ZStatus variable if given in input (different from nullptr) is updated with the current status
 * @param pKey
 * @param pRecord
 * @param pSt
 * @return
 */

const _Type *
ZIndexFile:: seekPartialRecord (_KType pKey,      //! key content to search for
                                                             size_t pSize,       //! size of the key
                                                             _Type &pRecord,    //! row buffer that will be returned (or not in case of error)
                                                             ZResult &pRes)      //! status if given (different from nullptr)
  {
    memset (&pRecord,0,sizeof(_Type));
    pRes =  _seekFirstPartialGeneric(pKey,pSize,*this);
    PersistentStatus = pRes.ZSt;
    if (pRes.ZSt!=ZS_FOUND)
                {
//                *pRes=wRes;
                return nullptr;
                }

    memmove (pRecord,&ZMFFather->Tab[pRes.ZIdx],sizeof(_Type));  //! copy rank to buffer given in input
    return (&ZMFFather->Tab[pRes.ZIdx]);        //! return pointer to this rank
  }//seekRecord




/**
 * @brief ZIndexFile::-seekAllPartial returns a collection of ZIX row ranks corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 *                                                  returned values are row ranks within the index (and not within ZAM)
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */
ZStatus ZIndexFile::_seekAllPartial(_KType pKey, size_t pSize, ZMulti_Result &pMRes)
{
    _seekAllPartialGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this,pMRes);
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               }
    return(pMRes.ZSt);
}// ZIX::_seekAllPartial





/**
 * @brief ZIndexFile::-seekFirstPartial returns the FIRST found ZIX row rank corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 *                                                  returned values are row ranks within the ZIX index (and not within ZAM)
 *
 *                                                  Warning : returns w ZIX rank and NOT ZAM rank
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */
ZResult ZIndexFile::_seekFirstPartial(_KType pKey,size_t pSize)
{

    return(_seekFirstPartialGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this));

}// ZIX::_seekFirstPartial



/**
 * @brief ZIndexFile::-seekFirstPartial returns the FIRST found corresponding ZAM row rank corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 *                                                  returned values are ZAM row ranks
 *
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */
ZResult ZIndexFile::seekFirstPartial(_KType pKey, size_t pSize)
{
    ZResult wRes=_seekFirstPartialGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this);
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
    wRes.ZIdx = getZAMIdx(wRes.ZIdx);
    return(wRes);
}// ZIX::seekFirstPartial


//!----------------End external ZIndexFile expanded methods----------------------------
//!
#endif // __FORMER__


//-------------------Generic Functions---------------------------------------

namespace zbs{

/** @cond Development
 * @brief _KeyValueExtraction Extracts the Key value from ZMasterFile record data using dictionnary CZKeyDictionary fields definition
 *
 * Returns the concaneted key value in pKey ZDataBuffer.
 * - Key fields are extracted from the ZMasterFile user record .
 * - They are converted appropriately whenever required using base internal conversion routines according Dictionary data type ( ZType_type ):
 *    + atomic fields _getAtomicFromRecord()
 *    + arrays _getArrayFromRecord()
 *    + for data type Class (ZType_type) : data is simply mass-moved to key without any conversion
 *
 * @note As we are processing variable length records, if a defined key field points outside the record length,
 *       then its returning key value is set to binary zero on the corresponding length of the field within returned Key value.
 *
 *
 *
 * @param[in] pZKDic  ZIndex dictionary (part of ZIndexControlBlock) for the index to extract key for
 * @param[in pRecord  ZMasterFile user record to extract key from
 * @param[out] pKey  Resulting concatenated key content
 * @return
 */
inline
ZStatus _keyValueExtraction(ZKeyDictionary& pZKDic, ZDataBuffer &pRecord, ZDataBuffer& pKey)
{
ZStatus wSt;
size_t wKeyOffset = 0;
size_t wSize ;

    pKey.allocate(pZKDic.NaturalSize+1);
    pKey.clearData();

    if (pZKDic.isEmpty())
            {
            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_BADICB,
                                     Severity_Severe,
                                     " Index Control Block appears to be malformed. Fields list is empty");
            return ZS_BADICB;
            }
    for (long wi=0;wi<pZKDic.size();wi++)
        {
// here put extraction rules. RFFU : Extraction could be complex. To be investigated and implemented

        wSize= pZKDic.Tab[wi].NaturalSize;
        if ((pZKDic.Tab[wi].RecordOffset+pZKDic.Tab[wi].NaturalSize)>pRecord.Size)  //! if key field points somewhere to end of record
        {
        if (pZKDic.Tab[wi].RecordOffset >= pRecord.Size) //! as we are processing variable length records : if Field is outside : key corresponding value is binary zero
                {
                wKeyOffset += pZKDic.Tab[wi].UniversalSize;
                wSt= ZS_KEYFIELDOUTBOUND;
                continue;
                } // complete outside
        wSize = pRecord.Size - ( pZKDic.Tab[wi].RecordOffset+pZKDic.Tab[wi].NaturalSize );  //! almost outside the record : take the remaining size
        wSt= ZS_KEYFIELDPARTIAL;
        }// partial outside
 /*       pKey.insertData(pRecord.Data+pICB->Fields[wi].Offset,
                   wSize,
                   wKeyOffset);
*/
        ZDataBuffer wDBField;
        ZIndexField wField;
        wField = pZKDic.Tab[wi] ;
        wField.NaturalSize = wSize;
        wField.UniversalSize = pZKDic.Tab[wi].UniversalSize;
        if (wField.ZType & ZType_Atomic)
                            _getAtomicFromRecord(pRecord,wDBField,wField);  //! Extract & pack unary Atomic Field ready for Key usage
                else
        if (wField.ZType & ZType_Class)
                            wDBField.setData(pRecord.Data+wField.RecordOffset,wField.UniversalSize); //! Extract the raw class structure
                else
        if (wField.ZType & ZType_Array)
                            {
                            _getArrayFromRecord(pRecord,wDBField,wField);  //! Extract & pack Array of Atomic Fields ready for Key usage
                            }
        pKey.changeData(wDBField,wKeyOffset);

        wKeyOffset += pZKDic.Tab[wi].UniversalSize;
        } // for
return ZS_SUCCESS;
}//zKeyValueExtraction

/** @endcond */
/**
 * @brief ZKeyCompareBinary This routine compares raw data that should have been already correctly packed in key internal format as binary
 * @param pKey1 Must be the key to match
 * @param pKey2 the index key to compare with
 * @param pSize Reference Size to compare : in case of partial
 * @return
 */
int
ZKeyCompareBinary (const ZDataBuffer &pKey1,ZDataBuffer &pKey2,ssize_t pSize)
{
ssize_t wSize = pSize;
    if (wSize<0)
            wSize=pKey1.Size;
    return (memcmp(pKey1.Data,pKey2.Data,wSize));
}

/**
 * @brief ZKeyCompareAlpha This routine compares raw data that should have been already correctly packed in key internal format as CString
 * @param pKey1 Must be the key to match
 * @param pKey2 the index key to compare with
 * @param pSize Reference Size to compare : in case of partial
 * @return
 */

int
ZKeyCompareAlpha (const ZDataBuffer &pKey1, ZDataBuffer &pKey2, ssize_t pSize)
{
ssize_t wSize = pSize;
    if (wSize<0)
            wSize=pKey1.Size;
    return (strncmp(pKey1.DataChar,pKey2.DataChar,wSize));
}


} // namespace zbs


//-----------------------Index Search routines------------------------------------------


/**
 * @brief ZIndexFile::_search Searches for a FIRST or UNIQUE value for a given index. This search may be EXACT or PARTIAL.
 *
 * Size of comparison is given by the Size field fo pKey (ZDataBuffer) : if partial key Size will be shorter than Index key size.
 * - Exact is when comparizon size is the whole index key internal size
 * - Partial is when  comparison is made on the size of the given key (may be a fragment of index key)
 * - Exact : means that both content (Key content and index content) much match exactly in terms of size AND content.
 * - First or Unique : means that a unique index rank is returned corresponding to the unique index content value if found
 * or the first index value matching in case of multiple values found in index (ZST_DUPLICATES).
 *
 * - Exact search with duplicate key index : the first matching value found is returned.
 * What is returned is not the first matching value in Index order but the first found value according the search algorithm.
 * - Partial search : the first matching value found is returned.
 * What is returned is not the first matching value in Index order but the first found value according the search algorithm.
 * - Exact search with no duplicate key index : the unique found value is returned
 *
 *
 *
 * Returns a ZIndexResult structure for the unique index reference if found : Index record rank ; ZMasterFile corresponding address
 * see @ref ZIndexResult.
 *
 * @param[in] pKey key content to find. Key has to be in Key internal format to be compared : formatted using _formatKeyContent() routine
 * @param[in] pZIF ZIndexFile object to search on
 *
 * @param[out] pZIR a ZIndexResult object with ZIndexFile relative key position (rank within index file) corresponding to key content if found
 *                  associated to ZMasterFile corresponding record (block) address.
 * @param[in] pZIFCompare comparizon routine. Either ZKeyCompareAlpha() or ZKeyCompareBinary().
 * @param[in] pLock a zlock_type mask build using ZLockMask_type containing the lock mask to apply onto the found records.
 * @note this lock mask applies to ZMasterFile records. Indexes are never locked.
 *
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
* - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
* - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
* - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
* - ZS_NOTFOUND if key value is not found in the middle of index values set
*
 */
ZStatus
ZIndexFile::_search(const ZDataBuffer &pKey,
                  ZIndexFile &pZIF,
                  ZIndexResult &pZIR,
                  const zlock_type pLock)
{

ZStatus     wSt= ZS_NOTFOUND;

pZIR.IndexRank = 0;
ZDataBuffer wIndexRecord;
ZIndexItem wIndexItem ;

ssize_t wCompareSize=0;

ZIFCompare    wZIFCompare=ZKeyCompareBinary;

int wR;

long whigh = 0;
long wlow = 0;
long wpivot = 0;

//_MODULEINIT_  ;

    if (pZIF.size()==0)
                    {
                    wSt= ZS_OUTBOUNDHIGH ;
                    goto _searchReturn;
                    }
// Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

     wCompareSize = pKey.Size;
    if (wCompareSize> pZIF.ZICB->ZKDic->UniversalSize)
                wCompareSize=pZIF.ZICB->ZKDic->UniversalSize;


// Choice of comparison routine

    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison


        whigh = pZIF.lastIdx();  // last element : highest boundary
        wlow = 0;               // first element : lowest boundary

        wSt=ZS_NOTFOUND;

//        while (true) // WHILE TOBE SUPPRESSED------------------Main loop------------------------
//            {
        pZIR.IndexRank = wlow ;

        pZIF.ZPMSStats.Reads ++;
        if ((wSt=pZIF.zget(wIndexRecord,0L))!=ZS_SUCCESS)
                                                    goto _searchReturn;
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

        if (wR==0)
                    {
                    wSt=ZS_FOUND ;
                    pZIR.IndexRank = wlow ;
                    goto _searchReturn;
                    }
        if (wR<0)
                return ZS_OUTBOUNDLOW;

         pZIR.IndexRank = whigh ;

         pZIF.ZPMSStats.Reads ++;
         if ((wSt=pZIF.zget(wIndexRecord,whigh))!=ZS_SUCCESS)
                                                     goto _searchReturn;
         wIndexItem.fromFileKey(wIndexRecord);
         wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

         if (wR==0)
                {
                wSt=ZS_FOUND ;
                pZIR.IndexRank = whigh ;
                goto _searchReturn;
                }
         if (wR>0)
            return ZS_OUTBOUNDHIGH;

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)// ---------------Main loop around pivot----------------------
            {
            pZIR.IndexRank = wpivot ;

            pZIF.ZPMSStats.Reads ++;
            if ((wSt=pZIF.zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
                                                        goto _searchReturn;
            wIndexItem.fromFileKey(wIndexRecord);
            wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

            if (wR==0)
                    {
                    wSt=ZS_FOUND ;
                    goto _searchReturn;
                    }

            if (wR>0)
                wlow = wpivot ;
                else
                whigh = wpivot ;

            if ((whigh-wlow)==1)
                    {
                    pZIR.IndexRank = wpivot ;
                    return ZS_NOTFOUND;
                    }

            wpivot = ((whigh-wlow)/2) + wlow ;
            pZIR.IndexRank = wpivot ;
            pZIF.ZPMSStats.Iterations ++;
            }// while (whigh-wlow)>2---------------Main loop around pivot----------------------

    /*
     *
     *  Process the remaining collection
     */

    wpivot = wlow;
    pZIR.IndexRank = wpivot;
    pZIF.ZPMSStats.Reads ++;
    if ((wSt=pZIF.zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
                                                    return wSt;
    wIndexItem.fromFileKey(wIndexRecord);
    wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

//        while ((wpivot<whigh)&&(_Compare::_compare(&pKey,&pZTab[wpivot]._Key) > 0))
    while ((wpivot<whigh)&& (wR > 0))
            {
            wpivot ++;
            pZIR.IndexRank=wpivot;

            pZIF.ZPMSStats.Reads ++;
            if ((wSt=pZIF.zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
                                                        return wSt;
            wIndexItem.fromFileKey(wIndexRecord);
            wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);
             }
    if (wR==0)
        {
        pZIR.IndexRank = wpivot ;
        wSt=ZS_FOUND;
        }
       else
        {
        pZIR.IndexRank = wpivot ; // because only 2 slots between wlow and whigh remain and we should insert before whigh
        return ZS_NOTFOUND ;
        }

_searchReturn:
    pZIR.ZMFAddress = wIndexItem.ZMFaddress ;
/*    if ((wSt=ZS_FOUND)&&(pLock != ZLock_Nolock ))
            {
            return static_cast<ZMasterFile*>(pZIF.ZMFFather)->zlockByAddress(wIndexItem.ZMFaddress,pLock); // lock corresponding ZMasterFile address with given lock mask
            }*/
    return (wSt) ;
}// _search

/**
 * @brief ZIndexFile::_searchAll  Search for ALL index ranks matching EXACTLY a certain key value. Key value cannot be partial.
 * The size of Index key value defines the length to compare with given key value pKey.
 *
 * @par Returns
 * - a ZIndexCollection gathering a collection of ZIndexResult_struct i. e. :
 *   + ZIndexFile ranks of indexes matching key value
 *   + ZMasterFile corresponding address
 *
 * @par Record lock (RFFU-not yet implemented)
 *
 * Record locking is done 'All at once'
 *
 *  - Key index values are first searched with no lock.
 *  - When collection has been found, then whole collection is locked if necessary (pLock != ZLock_Nolock)
 *  - Locks do not appy on indexes by on ZMasterFile main content
 *  - In case of error (may be already locked), already locked collection items are unlocked before returning errored ZStatus.
 *
 * @param[in] pKey key content to find. Key has to be formatted using _formatKeyContent() routine
 * @param[in] pZIF ZIndexFile object to search on
 * @param[out] pCollection  contains the resulting collection of ZIndexResult objects {Index rank ; ZMF blockaddress} for the matched elements
 *
 * @param[in] pZMS  defines whether comparison is Exact or Partial see @ref ZMatchSize_type. Defaulted to ZMS_MatchKeySize
 * - ZSC_MatchKeySize then comparizon is partial : comparizon size is made on Key size and key may be partial
 * - ZSC_MatchIndexSize then comparison is exact : comparison is made on the whole index key size
 *
 *
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
* - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
* - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
* - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
* - ZS_NOTFOUND if key value is not found in the middle of index values set
* - ZS_INVSIZE if search is requested on exact key and given key size is not equal to index key size
 */
ZStatus
ZIndexFile::_searchAll(const ZDataBuffer        &pKey,     // key content to find out in index
                       ZIndexFile               &pZIF,     // pointer to ZIndexControlBlock containing index description
                       ZIndexCollection         &pCollection,
                       const ZMatchSize_type    pZMS)
{
ZStatus     wSt=ZS_NOTFOUND;
//long        wIndexRank;
ZIndexResult wZIR;
ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

ZIFCompare wZIFCompare=ZKeyCompareBinary;

ssize_t wCompareSize= 0;

int wR;

long whigh ;
long wlow ;
long wpivot;
long wIndexFound=0;
//_MODULEINIT_


    pCollection.ZMS = pZMS;
    if (pZMS==ZMS_MatchIndexSize)
            {
            if (pKey.Size!=pZIF.ZICB->ZKDic->UniversalSize)
                        return ZS_INVSIZE ;
            }

//-----------Comparison routine selection------------------------------------

    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison

//-----------Comparison size definition------------------------------
// Size of comparison is given by pKey.Size : if partial key Size, given size will be shorter than Index key size.

     wCompareSize = pKey.Size;
    if (wCompareSize> pZIF.ZICB->ZKDic->UniversalSize)
                wCompareSize=pZIF.ZICB->ZKDic->UniversalSize;

    pCollection.clear();            // Always clearing the collection when zsearch
    pCollection.setStatus(ZS_NOTFOUND) ;
    wSt= ZS_NOTFOUND;

    if (pZIF.size()==0)
                    {
                    pCollection.setStatus(ZS_OUTBOUNDHIGH) ;
                    goto _searchAllReturn;
                    }


    whigh = pZIF.lastIdx();  // last element : highest boundary
    wlow = 0;               // first element : lowest boundary

    wpivot = ((whigh-wlow)/2) +wlow ;

// get lowest rank (0L)

        wZIR.IndexRank= wlow ;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection.setStatus(pZIF.zget(wIndexRecord,ZLock_Nolock))!=ZS_SUCCESS)
                                                goto _searchAllError;
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);

        if (wR==0)
            {
            pCollection.setStatus(ZS_FOUND) ;
            goto _searchAllBackProcess ;  // capture all collection around current wZIR.IndexRank
            }// if wR==0

        if (wR<0)
            {
            pCollection.setStatus(ZS_OUTBOUNDLOW) ;
            return ZS_OUTBOUNDLOW ;
            }// if wR<0

// get highest rank (lastIdx())
     wZIR.IndexRank = whigh ;
     pZIF.ZPMSStats.Reads ++;
     if (pCollection.setStatus(pZIF.zget(wIndexRecord,wZIR.IndexRank))!=ZS_SUCCESS)
                                             goto _searchAllError;
     wIndexItem.fromFileKey(wIndexRecord);
     wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);
//     wR= _Compare::_compare(&pKey,&pZTab[whigh]._Key) ;
     if (wR==0)
         {
         pCollection.setStatus(ZS_FOUND) ;
         goto _searchAllBackProcess ;  // capture all collection around current wZIR.IndexRank
         }// if wR==0

     if (wR>0)
         {
         pCollection.setStatus(ZS_OUTBOUNDHIGH) ;
         return ZS_OUTBOUNDHIGH ;
         }// if wR>0

     while ((whigh-wlow)>2) //---------------------Main loop around wpivot-----------
        {
        wZIR.IndexRank = wpivot ;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection.setStatus(pZIF.zget(wIndexRecord,wZIR.IndexRank))!=ZS_SUCCESS)
                                                goto _searchAllError;
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);
    //    wR= _Compare::_compare(&pKey,&pZTab[wpivot]._Key) ;

        if (wR==0)
            {
            pCollection.setStatus(ZS_FOUND) ;
            break ;  // capture all collection around current wZIR.IndexRank
            }// if wR==0

        if (wR>0)
                wlow = wpivot ;
            else
                {
                if (wR<0)
                        whigh = wpivot ;
                } // else

        if ((whigh-wlow)==1)
                {
                pCollection.setStatus(ZS_NOTFOUND);
                return ZS_NOTFOUND ;
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        wZIR.IndexRank = wpivot ;
        pZIF.ZPMSStats.Iterations++;
        } // while (whigh-wlow)>2 ---------------------Main loop around wpivot-----------

//--------------------------Collection Post processing--------------------
    if (pCollection.getStatus()!=ZS_FOUND)
        {
        return pCollection.getStatus();
        }
_searchAllBackProcess:
    /*
     *
     *  Process the remaining collection
     */
    wIndexFound = wZIR.IndexRank; // search for matches before and after wIndexFound
    pCollection.setStatus(ZS_FOUND) ;

    wZIR.ZMFAddress =wIndexItem.ZMFaddress;

    pCollection.push(wZIR);

    // need to capture anything equal before wIndexFound: push_front - then anything AFTER wIndexFound : push
    // search for lower range

    // Anything equal before wIndexFound : push_front
    pZIF.ZPMSStats.Reads ++;
    wSt=pZIF.zgetPrevious(wIndexRecord,ZLock_Nolock);
    wIndexItem.fromFileKey(wIndexRecord);
    while ((wSt==ZS_SUCCESS)&&(wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize)==0))
        {
        wZIR.IndexRank = pZIF.ZDescriptor.getCurrentRank();
        wZIR.ZMFAddress =wIndexItem.ZMFaddress;

        pCollection.push_front(wZIR); // next push : push_front to reorganize in the correct order
        pZIF.ZPMSStats.Reads ++;
        wSt=pZIF.zgetPrevious(wIndexRecord,ZLock_Nolock);
        if (wSt==ZS_SUCCESS)
            wIndexItem.fromFileKey(wIndexRecord);
        }// while
    if (wSt!=ZS_OUTBOUNDLOW)
            {
            pCollection.setStatus(wSt);
            goto _searchAllError;
            }
// up to here we have the first found index record in key order at pCollection[0]

    // Anything equal after wIndexFound : push
    pZIF.ZPMSStats.Reads ++;
    wIndexFound++;
    wZIR.IndexRank = wIndexFound;
    wSt=pZIF.zget(wIndexRecord,wIndexFound);
    if (wSt==ZS_SUCCESS)
        {
        wIndexItem.fromFileKey(wIndexRecord);
        while ((wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize)==0)&&(wSt==ZS_SUCCESS))
            {
            wZIR.IndexRank = pZIF.ZDescriptor.getCurrentRank();
            wZIR.ZMFAddress =wIndexItem.ZMFaddress;

            pCollection.push(wZIR); // after Found index : push
            pZIF.ZPMSStats.Reads ++;
            wSt=pZIF.zgetNext(wIndexRecord);
            if (wSt==ZS_SUCCESS)
                wIndexItem.fromFileKey(wIndexRecord);
            if (wSt!=ZS_EOF)
                    {
                    pCollection.setStatus(wSt);
                    goto _searchAllError;
                    }
            }
        }// if

_searchAllReturn:
//    if (pLock != ZLock_Nolock )
//            pCollection.zlockAll(pLock);    // lock corresponding Collection with given lock mask if necessary
    return (pCollection.getStatus()) ;

_searchAllError:
//    if (pLock != ZLock_Nolock )
//            pCollection.zunlockAll();    // lock corresponding Collection with given lock mask if necessary

    return (pCollection.getStatus());
}// _searchAll using ZIndexFile



/**
 * @brief ZIndexFile::_searchFirst search ZIndexFile pZIF for a first match of pKey (first in key order) and returns a ZIndexResult
 * - ZIndexFile rank : index file relative position of key found
 * - ZMasterFile corresponding record (block) address
 *
 * @par Accessing collection of selected records
 *
 *  ZIndexFile::_searchFirst() and ZIndexFile::_searchNext() works using a search context ( ZIndexSearchContext )that maintains a collection of found records (ZIndexCollection)
 *
 *  To get access to this collection, you may use the following syntax  <search context name>->Collection-><function to use>
 *
 * You may then use collection to
 * - refine the search with sequential adhoc fields matches
 * - use mass operations (lockAll, unlockAll, removeAll)
 *
 * @note sequential adhoc field rules will apply on ZMasterFile's record content and NOT to ZIndexFile Index key values.
 * This means that data to compare is RAW data, and NOT data formatted for index sorting.
 * see @ref ZIndexFile::_addKeyValue_Prepare() for more on internal key data format vs natural record data format.
 *
 * @param[in] pKey a ZDataBuffer with key value to search for.                                  [stored in collection's context]
 * Key value could be partial or exact, depending on ZDataBuffer Size and pZMS value
 * @param[in] pZIF ZIndexFile object to search on                                               [stored in collection]
 * @param[out] pCollection A pointer to the contextual meta-data from the search created by _searchFirst() routine.
 *
 * It contains
 * - a ZArray of ZIndexResult objects, enriched with successive _searchNext() calls
 * - Contextual meta-data for the search (an ZIndexCollectionContext embeded object holds this data)
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on
 *
 * @param[out] pZIR result of the _searchNext operation, if any, as a ZIndexResult with appropriate Index references (Index file rank ; ZMF record address)
 *
 * @param[in] pZMS  defines whether comparison is Exact or Partial see @ref ZMatchSize_type. Defaulted to ZMS_MatchKeySize
 * - ZSC_MatchKeySize then comparizon is partial : comparizon size is made on Key size and key may be partial
 * - ZSC_MatchIndexSize then comparison is exact : comparison is made on the whole index key size
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 * - ZS_INVSIZE if search is requested on exact key and given key size is not equal to index key size
 */
ZStatus
ZIndexFile::_searchFirst(const ZDataBuffer        &pKey,     // key content to find out in index
                         ZIndexFile               &pZIF,     // pointer to ZIndexControlBlock containing index description
                         ZIndexCollection         *pCollection,
                         ZIndexResult             &pZIR,
                         const ZMatchSize_type    pZMS)

 {
ZStatus     wSt=ZS_NOTFOUND;
//long        wIndexRank;
ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

ZIFCompare wZIFCompare=ZKeyCompareBinary; // comparison routine is deduced from Dictionary Key field type

ssize_t wCompareSize= 0;

int wR;

long whigh ;
long wlow ;
long wpivot;
//_MODULEINIT_

//-----------Initialization Section---------------------------------

    if (pCollection==nullptr)
    {
//-----------Comparison routine selection------------------------------------
    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison

//-----------Comparison size definition------------------------------

// Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

     wCompareSize = pKey.Size;
    if (wCompareSize> pZIF.ZICB->ZKDic->UniversalSize)
                wCompareSize=pZIF.ZICB->ZKDic->UniversalSize;



    pCollection=new ZIndexCollection (&pZIF); // initialize all and create ZIndexCollection instance
    pCollection->Context.setup (pKey,
                                nullptr,
                                wZIFCompare,
                                wCompareSize);
    pCollection->Context.Op = ZCOP_GetFirst ;
    pCollection->setStatus(ZS_NOTFOUND) ;
    wSt= ZS_NOTFOUND;
    } // pCollection == nullptr

    pCollection->ZMS = pZMS;
    if (pZMS==ZMS_MatchIndexSize)
            {
            if (pKey.Size!=pZIF.ZICB->ZKDic->UniversalSize)
                        return ZS_INVSIZE ;
            }
//-----------End Initialization Section---------------------------------

    if (pZIF.size()==0)
                    {
                    pCollection->setStatus(ZS_OUTBOUNDHIGH );
                    return ZS_OUTBOUNDHIGH;
                    }


    whigh = pZIF.lastIdx();  // last element : highest boundary
    wlow = 0;               // first element : lowest boundary

    pZIR.IndexRank= wlow ;
    pZIF.ZPMSStats.Reads ++;
    if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                            return pCollection->getStatus();
    wIndexItem.fromFileKey(wIndexRecord);
    wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);

    if (wR==0)
        {
        wSt= ZS_FOUND;
        pCollection->setStatus(ZS_FOUND);
        goto _searchFirstBackProcess ;  // capture all collection before current wZIR.IndexRank if any
        }// if wR==0
    if (wR<0)
        {
        pCollection->setStatus( ZS_OUTBOUNDLOW);
       return ZS_OUTBOUNDLOW;
        }// if wR<0

     pZIR.IndexRank = whigh ;
     pZIF.ZPMSStats.Reads ++;
     if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                                        return pCollection->getStatus();
     wIndexItem.fromFileKey(wIndexRecord);
     wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);

     if (wR==0)
         {
         pCollection->setStatus( ZS_FOUND);
         goto _searchFirstBackProcess ;  // capture all collection before current wZIR.IndexRank
         }// if wR==0
     if (wR>0)
         {
         pCollection->setStatus( ZS_OUTBOUNDHIGH);
        return ZS_OUTBOUNDHIGH;
         }// if wR>0

    wpivot = ((whigh-wlow)/2) +wlow ;

    while ((whigh-wlow)>2) // --------Main loop around pivot------------------
         {
        pZIR.IndexRank = wpivot ;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                                            return pCollection->getStatus();
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);
        if (wR==0)
            {
            pCollection->setStatus( ZS_FOUND);
            break ;  // capture all collection before current wZIR.IndexRank
            }// if wR==0

        if (wR>0)
                wlow = wpivot ;
            else
                {
                if (wR<0)
                        whigh = wpivot ;
                } // else

        if ((whigh-wlow)==1)
                {
                pCollection->setStatus( ZS_NOTFOUND);
                return ZS_NOTFOUND;
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        pZIR.IndexRank = wpivot ;
        pCollection->Context.CurrentZIFrank = wpivot;
        pZIF.ZPMSStats.Iterations++;
        } // while (whigh-wlow)>2--------Main loop around pivot------------------

    if (pCollection->getStatus()!=ZS_FOUND)
                            return pCollection->getStatus();
_searchFirstBackProcess:
/*
 *
 *  Search for the first key value in Index order
 */
    wIndexItem.fromFileKey(wIndexRecord);
    pZIR.IndexRank = pZIF.ZDescriptor.getCurrentRank();
    pZIR.ZMFAddress = wIndexItem.ZMFaddress;

    pCollection->Context.CurrentZIFrank = pZIR.IndexRank; // search for matches before current indexrank to find the first key value in index order

// need to set the first record to First index rank for key
// search for lower range

    pZIF.ZPMSStats.Reads ++;
    pCollection->setStatus(pZIF.zgetPrevious(wIndexRecord,ZLock_Nolock));

    while ((pCollection->getStatus()==ZS_SUCCESS)&&(wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize)==0))
        {
        wIndexItem.fromFileKey(wIndexRecord);
        pZIR.IndexRank = pZIF.ZDescriptor.getCurrentRank();
        pZIR.ZMFAddress = wIndexItem.ZMFaddress;
        pCollection->Context.CurrentZIFrank= pZIR.IndexRank;

//       pCollection.push_front(wZIR); // next push : push_front to reorganize in the correct order
        pZIF.ZPMSStats.Reads ++;
        pCollection->Context.CurrentZIFrank--;
        if (pCollection->Context.CurrentZIFrank<0)
                                        break;
        pCollection->setStatus(pZIF.zget(wIndexRecord,pCollection->Context.CurrentZIFrank));
        }// while


/*    if (pLock!=ZLock_Nolock) // lock record if requested
        {
        pCollection->setStatus(static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress,pLock));
        if (pCollection->getStatus()!=ZS_SUCCESS)
            {
            return pCollection->getStatus();
            }
        }// !=ZLock_Nolock
*/
    pCollection->push(pZIR); // enrich Collection with found record.

    ZException.clearStack();

    return pCollection->setStatus(ZS_FOUND);

}// _searchFirst
/**
 * @brief ZIndexFile::_searchNext
 * @param[in-out] pCollection A pointer to the contextual meta-data from the search created by _searchFirst() routine.
 *
 * It contains
 * - a ZArray of ZIndexResult objects, enriched with successive _searchNext calls
 * - Contextual meta-data for the search.
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on.
 * @param[out] pZIR result of the _searchNext operation, if any, as a ZIndexResult with appropriate Index references (Index file rank ; ZMF record address)
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 */
ZStatus
ZIndexFile::_searchNext (ZIndexResult       &pZIR,
                         ZIndexCollection*  pCollection)
{

ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//_MODULEINIT_

//-----------Initialization Section---------------------------------

    if (pCollection==nullptr)  // if no Context then errored
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Cannot invoke _searchNext without having Collection / Context : invoke first _searchFirst ");
        return ZS_INVOP;
    } // pCollection == nullptr

//-----------End Initialization Section---------------------------------

    pCollection->Context.Op = ZCOP_GetNext ;
    if (pCollection->getStatus()!=ZS_FOUND)  // if nothing has been found or error : return not found
                    return pCollection->getStatus();



// up to here we have the first found index record in key order at pCollection[0]

    // Anything equal after wIndexFound : push

    pCollection->Context.CurrentZIFrank ++;
    if (pCollection->Context.CurrentZIFrank > pCollection->ZIFFile->lastIdx())
        {
        pCollection->Context.CurrentZIFrank=pCollection->ZIFFile->lastIdx();
        return pCollection->setStatus(ZS_EOF);
        }
    pCollection->ZIFFile->ZPMSStats.Reads ++;
    pCollection->setStatus(pCollection->ZIFFile->zget(wIndexRecord,pCollection->Context.CurrentZIFrank));
    if (pCollection->getStatus()==ZS_SUCCESS)
        {
        wIndexItem.fromFileKey(wIndexRecord);
        if ((pCollection->Context.Compare(pCollection->Context.KeyContent,wIndexItem.KeyContent,pCollection->Context.CompareSize)==0)&&(pCollection->getStatus()==ZS_SUCCESS))
            {
            pZIR.IndexRank = pCollection->ZIFFile->ZDescriptor.getCurrentRank();
            pZIR.ZMFAddress =wIndexItem.ZMFaddress;
            pCollection->setStatus(ZS_FOUND);

            pCollection->push(pZIR); // after Found index : push
/*
            if (pCollection->getLock()!=ZLock_Nolock) // lock record if requested
                {
                pCollection->setStatus (static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress));
                if (pCollection->getStatus()!=ZS_SUCCESS)
                    {
                    return pCollection->getStatus ();
                    }
                }// !=ZLock_Nolock
*/


            }// if compare
        }// if
        else
        pCollection->setStatus(ZS_OUTBOUNDHIGH);

    return pCollection->getStatus();

}// _searchNext




/**
 * @brief ZIndexFile::_searchIntervalAll searches all index key value from ZIndexFile pZIF corresponding to Interval given by pKeyLow as lowest value for range and pKeyHigh as highest value for range.
 *      pKeyLow and pKeyHigh may be included in range if pExclude is false, or exclude from range if pExclude is true.
 * @param[in] pKeyLow   Lowest key content value to find out in index           [stored in collection's context]
 * @param[in] pKeyHigh  Highest key content value to find out in index          [stored in collection's context]
 * @param[in] pZIF      ZIndexFile object                                       [stored in collection]
 * @param[out] pCollection ZIndexCollection object created by the routine. It stores the resulting set of reference to found ZIndexFile-ZMasterFile records.
 * @param[out] pZIR ZIndexResult giving the result of the first search if any
 * @param[in] pExclude Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)
 *          this option is stored within Collection context using operation code
 * @param[in] pLock Lock mask to apply to found records                         [stored in collection's context]
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 */
ZStatus
ZIndexFile::_searchIntervalAll  (const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                                 const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                                 ZIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                                 ZIndexCollection       *pCollection,   // enriched collection of reference (ZIndexFile rank, ZMasterFile record address)
                                 const bool             pExclude) // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)
{


ZStatus      wSt;
ZIndexResult wZIR;

//_MODULEINIT_


    wSt=_searchIntervalFirst(pKeyLow,
                             pKeyHigh,
                             pZIF,
                             pCollection,
                             wZIR,
                             pExclude);
    if (wSt!=ZS_FOUND)
                return wSt;
    while (wSt==ZS_FOUND)
        {
        wSt=_searchIntervalNext(wZIR,pCollection);
        }
    return (wSt==ZS_EOF)?ZS_FOUND:wSt;

}// _searchIntervalAll

/**
 * @brief ZIndexFile::_searchIntervalFirst
 * @param[in] pKeyLow   Lowest key content value to find out in index           [stored in collection's context]
 * @param[in] pKeyHigh  Highest key content value to find out in index          [stored in collection's context]
 * @param[in] pZIF      ZIndexFile object                                       [stored in collection]
 * @param[out] pCollection ZIndexCollection object created by the routine.
 * @param[out] pZIR ZIndexResult giving the result of the first search if any
 * @param[in] pExclude Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)
 *          this option is stored within Collection context using operation code
 * @param[in] pLock Lock mask to apply to found records                         [stored in collection's context]
 * @return
 */
ZStatus
ZIndexFile::_searchIntervalFirst(const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                                 const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                                 ZIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                                 ZIndexCollection       *pCollection,   // enriched collection of reference (ZIndexFile rank, ZMasterFile record address)
                                 ZIndexResult           &pZIR,
                                 const bool             pExclude)// Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)

{

ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

ZIFCompare wZIFCompare=ZKeyCompareBinary; // comparison routine is deduced from Dictionary Key field type

ssize_t wCompareSize= 0;

ZStatus wSt;

int wR;

long whigh ;
long wlow ;
long wpivot;
//_MODULEINIT_

//-----------Initialization Section---------------------------------
    if (pCollection!=nullptr)
                    pCollection->clear();
    if (!pCollection->Context.FInitSearch)
    {
//-----------Comparison routine selection------------------------------------
    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison

//-----------Comparison size definition------------------------------
// Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

     wCompareSize = pKeyLow.Size;
    if (wCompareSize> pZIF.ZICB->ZKDic->UniversalSize)
                wCompareSize=pZIF.ZICB->ZKDic->UniversalSize;

    pCollection=new ZIndexCollection (&pZIF);

    pCollection->Context.setup (pKeyLow,&pKeyHigh,wZIFCompare,wCompareSize); // initialize all and create ZIndexCollection instance
    pCollection->setStatus(ZS_NOTFOUND) ;
    wSt= ZS_NOTFOUND;
    pCollection->Context.Op = ZCOP_Interval | ZCOP_GetFirst ;
    if (pExclude)
            pCollection->Context.Op |= ZCOP_Exclude ;
    } // pSearchContext == nullptr

//-----------End Initialization Section---------------------------------

    if (pZIF.size()==0)
                    {
                    pCollection->setStatus(ZS_OUTBOUNDHIGH );
                    return ZS_OUTBOUNDHIGH;
                    }


//------------First half search---------------------------------

    whigh = pZIF.lastIdx();  // last element : highest boundary
    wlow = 0;               // first element : lowest boundary

// test low
        pZIR.IndexRank= wlow ;
        pCollection->Context.CurrentZIFrank = wlow;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                                return pCollection->getStatus();
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize);

        if (wR==0)
            {
            wSt= ZS_FOUND;
            pCollection->setStatus(ZS_FOUND);
            goto _searchIntervalFirstBackProcess;  // capture all collection before current wZIR.IndexRank if any
            }// if wR==0
        if (wR<0)
            {
            pCollection->setStatus(ZS_OUTBOUNDLOW);
            return ZS_OUTBOUNDLOW;
            }// if wR==0
// test high
     pZIR.IndexRank = whigh ;
     pCollection->Context.CurrentZIFrank = whigh;
     pZIF.ZPMSStats.Reads ++;
     if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                                        return pCollection->getStatus();
     wIndexItem.fromFileKey(wIndexRecord);
     wR= wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize);
//     wR= _Compare::_compare(&pKey,&pZTab[whigh]._Key) ;
     if (wR==0)
         {
         pCollection->setStatus( ZS_FOUND);
         goto _searchIntervalFirstBackProcess ;  // capture all collection before current wZIR.IndexRank
         }// if wR==0
     if (wR > 0)
         {
         wSt= ZS_OUTBOUNDHIGH;
         pCollection->setStatus(ZS_OUTBOUNDHIGH);
         goto _searchIntervalFirstBackProcess;
         }// if wR==0

    wpivot = ((whigh-wlow)/2) +wlow ;

     while ((whigh-wlow)>2) //------Main loop around wpivot---------------------
         {

        pZIR.IndexRank = wpivot ;
        pCollection->Context.CurrentZIFrank = wpivot;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                                            return pCollection->getStatus();
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize);

        if (wR==0)
            {
            pCollection->setStatus( ZS_FOUND);
            break ;  // process collection before current wZIR.IndexRank
            }// if wR==0

        if (wR>0)
                wlow = wpivot ;
            else
                {
//                if (wR<0) // obvious but commented here for clearity
                        whigh = wpivot ;
                } // else

        if ((whigh-wlow)==1)
                {
                pCollection->setStatus( ZS_NOTFOUND);
                break;  // do not mind if we have not found the low key value : searching for an interval
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        pZIR.IndexRank = wpivot ;
        pCollection->Context.CurrentZIFrank = wpivot;
        pZIF.ZPMSStats.Iterations++;
        } // while (whigh-wlow)>2 ------End Main loop around wpivot---------------------

_searchIntervalFirstBackProcess:
/*
 *
 *  Search for the first key value in Index order
 */
    if ((pCollection->getStatus()==ZS_FOUND)&&(pCollection->Context.Op & ZCOP_Exclude) )
                                                            pCollection->setStatus(ZS_NOTFOUND); // exclude the lowest key value found
                else
                {
                pZIR.IndexRank = pZIF.ZDescriptor.getCurrentRank();// include the lowest key value found as valid candidate (need to test highest key value)
                pZIR.ZMFAddress = wIndexItem.ZMFaddress;
                }
    pCollection->Context.CurrentZIFrank = pZIR.IndexRank; // search for matches before current indexrank to find the first key value in index order

// need to set the first record to First index rank for key
// search for lower range

    pZIF.ZPMSStats.Reads ++;
    wSt=pZIF.zgetPrevious(wIndexRecord,ZLock_Nolock);

    while (wSt==ZS_SUCCESS)
        {
        wIndexItem.fromFileKey(wIndexRecord);

        if (pCollection->Context.Op & ZCOP_Exclude)
                {
                if (wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize)<= 0)  // Interval excludes low key value
                                            break;
                }
                else
                {
                if (wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize) < 0) // Interval includes low key value
                                           break;
                }
       pZIR.IndexRank = pZIF.ZDescriptor.getCurrentRank();
       pZIR.ZMFAddress = wIndexItem.ZMFaddress;
       pCollection->Context.CurrentZIFrank= pZIR.IndexRank;

        pZIF.ZPMSStats.Reads ++;
        pCollection->Context.CurrentZIFrank--;
        if (pCollection->Context.CurrentZIFrank < 0)
                                                break;
        wSt=pZIF.zget(wIndexRecord,pCollection->Context.CurrentZIFrank);
        }// while

// Now test the highest Key content value

   if (pCollection->Context.Op & ZCOP_Exclude)
            {
           if (wZIFCompare((ZDataBuffer&)pKeyHigh,wIndexItem.KeyContent,wCompareSize) >= 0)  // Interval excludes high key value
                                   {
                                   pCollection->setStatus(ZS_NOTFOUND);
                                   return ZS_NOTFOUND;
                                   }
            }
              else
            {
           if (wZIFCompare((ZDataBuffer&)pKeyHigh,wIndexItem.KeyContent,wCompareSize)> 0) // Interval includes high key value
                                   {
                                   pCollection->setStatus(ZS_NOTFOUND);
                                   return ZS_NOTFOUND;
                                   }
            }
    pCollection->setStatus(ZS_FOUND);
    pCollection->Context.CurrentZIFrank = pZIR.IndexRank ;

// OK : key is within the requested interval

/*    if (pLock!=ZLock_Nolock) // lock record if requested
        {
        pCollection->setStatus(static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress,pLock));
        if (pCollection->getStatus()!=ZS_SUCCESS)
            {
            return pCollection->getStatus();
            }
        }// !=ZLock_Nolock
*/
    pCollection->push(pZIR); // enrich Collection with found record.

    ZException.clearStack();

    return pCollection->setStatus(ZS_FOUND);

}// _searchIntervalFirst

/**
 * @brief ZIndexFile::_searchIntervalNext
 * @param[out]    pZIR
 * @param[in-out] pCollection
 * @return
 */
ZStatus
ZIndexFile::_searchIntervalNext (ZIndexResult       &pZIR,
                                 ZIndexCollection*  pCollection)
{

ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//_MODULEINIT_

//-----------Initialization Section---------------------------------

    if (pCollection==nullptr)  // if no Context then errored
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Cannot invoke _searchNext without having Collection / Context : invoke first _searchFirst ");
        return ZS_INVOP;
    } // pCollection == nullptr

//-----------End Initialization Section---------------------------------

    if (pCollection->getStatus()!=ZS_FOUND)  // if nothing has been found or error : return not found
                    return pCollection->getStatus();


    pCollection->Context.Op = pCollection->Context.Op & ~ ZCOP_GetFirst ;   // clear ZCOP_GetFirst
    pCollection->Context.Op |= ZCOP_Interval | ZCOP_GetNext ;       // set ZCOP_GetNext (plus ZCOP_Interval if necessary)

// up to here we have the first found index record in key order at pCollection[0]

    // Anything equal after wIndexFound : push

    pCollection->Context.CurrentZIFrank ++;
    if (pCollection->Context.CurrentZIFrank > pCollection->ZIFFile->lastIdx())
        {
        pCollection->Context.CurrentZIFrank=pCollection->ZIFFile->lastIdx();
        return pCollection->setStatus(ZS_EOF);
        }

    pCollection->ZIFFile->ZPMSStats.Reads ++;
    pCollection->setStatus(pCollection->ZIFFile->zget(wIndexRecord,pCollection->Context.CurrentZIFrank));
    if (pCollection->getStatus()==ZS_SUCCESS)
        {
        wIndexItem.fromFileKey(wIndexRecord);
        // For get next : only test the highest Key content value

       if (pCollection->Context.Op & ZCOP_Exclude)
                {
               if (pCollection->Context.Compare(pCollection->Context.KeyHigh,wIndexItem.KeyContent,pCollection->Context.CompareSize)>0)  // Interval excludes high key value
                                       {
                                       pCollection->setStatus(ZS_EOF);
                                       return ZS_EOF;
                                       }
                }
                  else
                {
               if (pCollection->Context.Compare(pCollection->Context.KeyHigh,wIndexItem.KeyContent,pCollection->Context.CompareSize)>=0) // Interval includes high key value
                                       {
                                       pCollection->setStatus(ZS_EOF);
                                       return ZS_EOF;
                                       }
                }
            pZIR.IndexRank = pCollection->ZIFFile->ZDescriptor.getCurrentRank();
            pZIR.ZMFAddress =wIndexItem.ZMFaddress;



/*            if (pCollection->getLock()!=ZLock_Nolock) // lock record if requested
                {
                pCollection->setStatus (static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress,pCollection->getLock()));
                if (pCollection->getStatus()!=ZS_SUCCESS)
                    {
                    return pCollection->getStatus ();
                    }
                }// !=ZLock_Nolock
*/
            pCollection->setStatus(ZS_FOUND);
            pCollection->push(pZIR); // after Found index : push

        }// if

    return pCollection->getStatus();

}// _searchIntervalNext

//--------------------------End Search routines--------------------------------------

/**
 * @brief ZIndexFile::getKeyIndexFields obtains natural fields values from a given ZIndex key rank content (pKeyValue) according ZKDic fields dictionary definitions
                     and returns a ZDataBuffer in pIndexContent containing concanetated natural fields values up to be used by computer as their origin data (external natural format).

                     @see ZIndexControlBlock::zkeyValueExtraction

 * @param pIndexContent     concanetated natural fields values
 * @param pKeyValue         ZIndex key in ZIndex internal data format
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZIndexFile::getKeyIndexFields(ZDataBuffer &pIndexContent,ZDataBuffer& pKeyValue)
{
ZStatus wSt=ZS_SUCCESS;
ZDataBuffer wFieldValue ;
    pIndexContent.clear();
    for (long wi=0; (wi<ZICB->ZKDic->size())&&(wSt==ZS_SUCCESS);wi++)
    {
    wSt=_getFieldValueFromKey(pKeyValue,wFieldValue,wi,*ZICB->ZKDic);
    if (wSt==ZS_SUCCESS)
            pIndexContent.appendData(wFieldValue);
    }
    return wSt;
}

/**
  * @brief ZIndexFile::zprintKeyFieldsValues     prints the key fields values in a human readable format from a ZIndexFile
  *
  * Gets a record from a ZIndexFile corresponding to its relative position pRank.
  * Prints the index content to pOutput after having converted back all composing field using ZIndex Dictionary.
  *
  * In case of ZType_Class field, then the content of data is dumped using ZDataBuffer::Dump().
  *
  * @param[in] pRank       logical rank of key record with ZIndexFile
  * @param[in] pHeader  if set to true then key fields description is printed. False means only values are printed.
  * @param[in] pKeyDump if set to true then index key record content is dumped after the list of its fields values. False means only fields values are printed.
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
  */
 ZStatus
 ZIndexFile::zprintKeyFieldsValues (const long pRank,bool pHeader,bool pKeyDump,FILE*pOutput)
 {
ZStatus wSt;
ZBlock wBlock;
ZDataBuffer wPrintableField;
ZDataBuffer wKeyContent;
//zlock_type wLock = 0L;
zaddress_type wZMFAddress=0;
zaddress_type wIdxAddress;

     wSt=_getByRank(ZDescriptor,wBlock,pRank,wIdxAddress);
     if (wSt!=ZS_SUCCESS)
                        return wSt;


// we have to skip zaddress_type field in from of key record (ZMF address) to isolate only Key content
     wKeyContent.setData(wBlock.Content.Data + sizeof(zaddress_type),wBlock.Content.Size-sizeof(zaddress_type));


// header : index name then fields dictionary definition
     if (pHeader)
     {
     fprintf (pOutput,
              "--------------------------------------------------------------------------------------\n"
              " Index name %s\n",
              ZICB->Name.content);
     for (long wi=0;wi<ZICB->ZKDic->size();wi++)
     {
     fprintf (pOutput,
              " Field order %ld  name <%s> Data type <%s> \n",
              wi,
              ZICB->ZKDic->Tab[wi].Name.content,
              decode_ZType( ZICB->ZKDic->Tab[wi].ZType));
     }// for
     fprintf (pOutput,
              "---------------------------------------------------------------------------------------\n");
     }// if pHeader

// then fields values

     memmove(&wZMFAddress,wBlock.Content.Data,sizeof(zaddress_type));

     fprintf (pOutput,
              " Rank <%4ld>  ZMasterFile Address <%lld> \n",
              pRank,
              wZMFAddress);

     for (long wi=0;wi<ZICB->ZKDic->size();wi++)
     {
     fprintf (pOutput,
              "      field order <%ld>  <%s> value ",
              wi,
              ZICB->ZKDic->Tab[wi].Name.content);

     while (true)
     {
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Class)  // if class : simple dump
                {
             ssize_t wKeyOffset = ZICB->ZKDic->fieldKeyOffset(wi);

             wPrintableField.setData(wKeyContent.DataChar +wKeyOffset, ZICB->ZKDic->Tab[wi].UniversalSize);
//             wPrintableField.setData(wBlock.Content.DataChar +wKeyOffset, ZICB->ZKDic->Tab[wi].KeyFieldSize);
//         _printAtomicValueFromKey(wBlock.Content,wPrintableField,wi,ZICB->ZKDic);
                fprintf (pOutput,
                         "\n");
                wPrintableField.Dump(16,pOutput);
                break;
                }
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Atomic)
     {
         _printAtomicValueFromKey(wKeyContent,wPrintableField,wi,*ZICB->ZKDic);
         fprintf (pOutput,
                  "<%s> ",
                  wPrintableField.DataChar);
         break;
     }
     if (ZICB->ZKDic->Tab[wi].ZType == ZType_ArrayChar)
     {
         ssize_t wKeyOffset = ZICB->ZKDic->fieldKeyOffset(wi);

         wPrintableField.setData(wKeyContent.DataChar +wKeyOffset, ZICB->ZKDic->Tab[wi].UniversalSize);
//         _printAtomicValueFromKey(wBlock.Content,wPrintableField,wi,ZICB->ZKDic);
         fprintf (pOutput,
                  "<%s> ",
                  wPrintableField.DataChar);
         break;
     }
     if (ZICB->ZKDic->Tab[wi].ZType & ZType_Array)
     {
         _printArrayValueFromKey(wKeyContent,wPrintableField,wi,*ZICB->ZKDic);
         fprintf (pOutput,
                  "%s ",
                  wPrintableField.DataChar);
         break;
     }
     fprintf (pOutput,
              "**unknown type*** ");
     break;
     }// while true

     fprintf (pOutput,
              "\n");
     }// for

     if (pKeyDump)
                 wBlock.Content.Dump();

     return ZS_SUCCESS;
 }//zprintKeyFieldsValues


/** @}
  */

//}// namespace zbs

#endif //ZINDEXFILE_CPP
