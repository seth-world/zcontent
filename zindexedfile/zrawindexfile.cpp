#ifndef ZRAWINDEXFILE_CPP
#define ZRAWINDEXFILE_CPP
#include <zindexedfile/zrawindexfile.h>
//#include <zindexedfile/zifgeneric.h>
//#include <zindexedfile/zmasterfile.h>
#include <zrandomfile/zrfcollection.h>

#include <zcontentcommon/zresult.h>
#include <zindexedfile/zindexcontrolblock.h>
//#include <zindexedfile/zrecord.h>
//#include <zindexedfile/zindexitem.h>

#include <zindexedfile/zrawmasterfile.h>
#include <zindexedfile/zmasterfile.h>

#include <zxml/zxmlprimitives.h>

#include <zcontentcommon/urfparser.h>


using namespace zbs ;





/** @addtogroup ZIndex
*   @{
*/

uint8_t ZRawIndexFile::getRunMode() {return ZMFFather->EngineMode;}

bool ZRawIndexFile::isSearchDycho() {return ZMFFather->EngineMode & ZIXM_SearchDycho;}
bool ZRawIndexFile::isDebug() {return ZMFFather->EngineMode & ZIXM_Debug;}
bool ZRawIndexFile::isUpdateHeader() {return ZMFFather->EngineMode & ZIXM_UpdateHeader;}

/*
void ZRawIndexFile::setEngineMode(uint8_t pOnOff) {
  EngineMode=pOnOff;
  if (EngineMode & ZIXM_UpdateHeader)
    setUpdateHeader(true);
}
*/

void ZRawIndexFile::showRunMode() {
  if (isDebug())
    _DBGPRINT("ZRawIndexFile-I-DBGSET Debug mode is set.\n")
  else
    _DBGPRINT("ZRawIndexFile-I-DBGRST Debug mode is off.\n")
  if (isSearchDycho()) {
    _DBGPRINT("ZRawIndexFile-I-DYCHSET Search mode is set to dychotomic search.\n")
  }
  else
    _DBGPRINT("ZRawIndexFile-I-SEQSET Search mode is set to sequential search.\n")

  if (isUpdateHeader()) {
    _DBGPRINT("ZRawIndexFile-I-UPDTSET Search mode is set to update header at high frequence.\n")
  }
  else {
    _DBGPRINT("ZRawIndexFile-I-UPDTSET Search mode is set to update header at low frequence.\n")
  }
}

//---------ZIndexControlBlock routines----------------------------------------------------



/**
 * @brief ZIndexControlBlock::_importICB Imports an ZIndexControlBlock from its flats structure(unsigned char*) and load it to current ZICB
 * @note pBuffer pointer is not freed by this routine
 * @param[in] pBuffer
 * @param[out] Imported data size : note this represents the amount of space imported from pBuffer since its beginning. pBuffer may contain more than one ZICB.
 * @return
 */



#ifdef __COMMENT__
/**           see ZRecord::_extractKeys()
 * @brief ZSIndexControlBlock::zKeyValueExtraction Extracts the Key value from ZMasterFile record data using dictionnary CZKeyDictionary fields definition
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
ZSIndexControlBlock::zkeyValueExtraction (ZRecord *pRecord, ZDataBuffer& pKey)
{

 //   return  _keyValueExtraction(this->ZKDic,pRecord,pKey);
    return     _keyValueExtraction(this->ZKDic,pRecord,pKey);

}
#endif // __COMMENT__


//----------ZRawIndexFile-----------------------------------------------


ZRawIndexFile::ZRawIndexFile  (ZRawMasterFile *pFather): ZRandomFile()
{

//    setEngineMode(pRunMode);

    ZMFFather=pFather;
    if (pFather==nullptr) {
      if (!isDebug()) {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVADDRESS,
                                    Severity_Fatal,
                                    "ZRawIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
            this->~ZRawIndexFile();
            ZException.exit_abort();
      }
    }
    return ;
}// ZIF CTOR


ZRawIndexFile::ZRawIndexFile  (ZRawMasterFile *pFather, ZIndexControlBlock &pZICB): ZRandomFile(),ZIndexControlBlock(pZICB)
{

    ZMFFather=pFather;

    if (pFather==nullptr) {
      if (!isDebug()) {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVADDRESS,
                                        Severity_Fatal,
                                        "ZRawIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
                this->~ZRawIndexFile();
                ZException.exit_abort();
      }
    }

}// ZIF CTOR 2 w

ZRawIndexFile::ZRawIndexFile  (ZRawMasterFile *pFather, int pKeyUniversalsize, const utf8String &pIndexName , ZSort_Type pDuplicates): ZRandomFile(),ZIndexControlBlock()
{

  ZMFFather=pFather;

  if (pFather==nullptr) {
    if (!isDebug()) {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVADDRESS,
        Severity_Fatal,
        "ZRawIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
    this->~ZRawIndexFile();
    ZException.exit_abort();
    }
  }

  KeyUniversalSize=pKeyUniversalsize;
  IndexName=pIndexName;
  Duplicates=pDuplicates;
}

ZStatus
ZRawIndexFile::setIndexURI  (uriString &pURI)
{
    URIIndex=pURI;
    return ZRandomFile::setPath(pURI);
}

void
ZRawIndexFile::setIndexName  (utf8String &pName)
{
    IndexName = pName;
    return ;
}


//#include <zindexedfile/zmasterfile.h>



/**
 * @brief ZRawIndexFile::removeIndexFiles Removes all files composing current index (before index remove at ZMasterFile level)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawIndexFile::removeIndexFiles(ZaiErrors *pErrorLog)
{

//    return  ZRandomFile::zremoveFile();
return  ZRandomFile::_removeFile(pErrorLog);
}

/**
 * @brief ZRawIndexFile::zcreateIndex creates a new index file corresponding to the given specification ICB and ZRF parameters
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
ZRawIndexFile::zcreateIndexFile(ZIndexControlBlock &pICB,
                                 uriString &pIndexUri,
                                 long pAllocatedBlocks,
                                 long pBlockExtentQuota,
                                 zsize_type pInitialSize,
                                 bool pHighwaterMarking,
                                 bool pGrabFreeSpace,
                                 bool pBackup,
                                 bool pLeaveOpen)
{
ZStatus wSt;


//  setEngineMode(pRunMode);

  ZIndexControlBlock::_copyFrom(pICB);
  wSt=setIndexURI(pIndexUri);
  if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
//  ZIndexFile Record size is KeySize (sum of fields lengths) plus size of a zaddress_type (pointer to Master File record)

    ZRandomFile::setCreateMaximum (pInitialSize,
                             pAllocatedBlocks,
                             pBlockExtentQuota,
                             pICB.KeyUniversalSize + sizeof(zaddress_type),
                             pHighwaterMarking,
                             pGrabFreeSpace);
    wSt= ZRandomFile::_create(pInitialSize,ZFT_ZIndexFile,pBackup,false);             // Do not leave it open after file creation : ZRF_Exclusive | ZRF_All

    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
    wSt=ZRandomFile::_ZRFopen((ZRF_Exclusive | ZRF_All),ZFT_ZIndexFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
//
// now need to write the updated ICB to Index Header
//
    wSt=writeIndexControlBlock ();
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }

    if (!pLeaveOpen)
//            return   ZRandomFile::zclose();
            return   ZRandomFile::zclose();
//    return  ZS_SUCCESS;
return  ZS_SUCCESS;
}//zcreateIndexFile


ZStatus
ZRawIndexFile::zcreateIndexFile(ZIndexControlBlock &pICB,
                                uriString &pIndexUri,
                                long pAllocatedBlocks,
                                long pBlockExtentQuota,
                                zsize_type pInitialSize,
                                long pBlockTargetSize,
                                bool pHighwaterMarking,
                                bool pGrabFreeSpace,
                                bool pBackup,
                                bool pLeaveOpen)
{
  ZStatus wSt;


  ZIndexControlBlock::_copyFrom(pICB);
  wSt=setIndexURI(pIndexUri);
  if (wSt!=ZS_SUCCESS) {
    ZException.addToLast(" While Creating ZIndexFile %s\n",
        pIndexUri.toString());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }
  //  ZRawIndexFile Record size is KeySize (sum of fields lengths) plus size of a zaddress_type (pointer to Master File record)

  ZRandomFile::setCreateMaximum ( pAllocatedBlocks,
                            pBlockExtentQuota,
                            pBlockTargetSize,
                            pInitialSize,
                            pHighwaterMarking,
                            pGrabFreeSpace);
  wSt= ZRandomFile::_create(pInitialSize,ZFT_ZIndexFile,pBackup,false);             // Do not leave it open after file creation : ZRF_Exclusive | ZRF_All

  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" While Creating ZRawIndexFile %s\n",
        pIndexUri.toString());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }
  wSt=ZRandomFile::_ZRFopen((ZRF_Exclusive | ZRF_All),ZFT_ZIndexFile);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" While Creating ZRawIndexFile %s\n",
        pIndexUri.toString());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }
  //
  // now need to write the updated ICB to Index Header
  //
  wSt=writeIndexControlBlock ();
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" While Creating ZRawIndexFile %s\n",
        pIndexUri.toString());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }

  if (!pLeaveOpen)
    //            return   ZRandomFile::zclose();
    return   ZRandomFile::zclose();
  //    return  ZS_SUCCESS;
  return  ZS_SUCCESS;
}//zcreateIndexFile

ZStatus
ZRawIndexFile::openIndexFile(uriString &pIndexUri,long pIndexRank,const int pMode)
{
ZStatus wSt;

    URIIndex = pIndexUri;

    IndexRank = pIndexRank;

    wSt=ZRandomFile::setPath(URIIndex);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" setting path for ZIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
    wSt=ZRandomFile::_ZRFopen(pMode,ZFT_ZIndexFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.last().Severity=Severity_Severe;
                ZException.addToLast(" Openning ZIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
// --------- Need to read ICB and check whether it is ok with the given ICB --------------

/*     Compare Index Control Block read from Index File to the one stored in ZSMasterFile MCB  */

//    ZDataBuffer wRawICB;

//    wSt=ZRandomFile::getReservedBlock(wRawICB,true);  // get ICB on local ZIndex file
    wSt=ZRandomFile::_getReservedHeader(true);  // get ICB on local ZIndex file

    if (wSt!=ZS_SUCCESS) {
      ZException.addToLast( " Reading Reserved header zone ZIndexFile %s",
                            pIndexUri.toString());
                            ZException.setLastSeverity(Severity_Severe);
                            return  wSt;
    }

/*    if (CheckSum!=nullptr)
    {
    checkSum* wLocalCheckSum = wRawICB.newcheckSum();// compute checksum on it


    if ((*CheckSum)!=(*wLocalCheckSum))   // compare with ZMF Father's checkSum
        {
        ZDataBuffer wMasterICBContent;
        delete wLocalCheckSum;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADCHECKSUM,
                              Severity_Error,
                              "Index Control Block checksum does not mach his related MasterFile ICB checksum for index file is %s",
                              URIIndex.toString()
                             );
        fprintf(stderr,
                "%s>> InvChecksum  Index Control Blocks checksum difference detected.\n Dumping ICBs and Performing unitary check.\n",
                _GET_FUNCTION_NAME_);


        size_t wRet=ZIndexControlBlock::_exportAppend(wMasterICBContent);

        fprintf(stderr,"%s>> Father zmasterfile index block dump \n",_GET_FUNCTION_NAME_);
        wMasterICBContent.Dump();
        fprintf(stderr,"%s>> local zindexfile index block dump \n",_GET_FUNCTION_NAME_);
        wRawICB.Dump();

        bool wComp=true;
        size_t wi=0;

        if (wMasterICBContent.Size!=wRawICB.Size)
                                    wComp=false;
        for (wi=0;(wi<wRawICB.Size)&&wComp;wi++)
                        if (wRawICB.Data[wi]!=wMasterICBContent.Data[wi])
                                                {
                                                wComp=false;
                                                }
     //   if (memcmp(wRawICB.Data,ZICB->_exportICB(wICBContent).Data,wRawICB.Size)!=0) // compare it with the export of Master's ICB
        if (!wComp)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADICB,
                              Severity_Error,
                              "Index Control Block does not mach given ICB at position <%ld>- Index file is %s",
                              wi,
                              URIIndex.toString()
                              );
        ZException.setComplement ("Index file found is not aligned with corresponding ZSMasterFile file index definition");
        ZException.setLastSeverity(Severity_Severe);
        return  ZS_BADICB;
        }
        else
            fprintf (stdout,"%s>> ICB unitary control check passed. Checksums were wrong for one reason but ICB content is aligned\n",_GET_FUNCTION_NAME_);
        }// checkSum compared
// zstatistic intitialization
    delete wLocalCheckSum;
    }//if (CheckSum!=nullptr)
*/

  const unsigned char* wPtr=  ZReserved.Data;
  wSt=ZIndexControlBlock::_import(wPtr) ;
  if (wSt!=ZS_SUCCESS) {
    ZException.addToLast( " ZIndexFile %s", pIndexUri.toString());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }
  if (ZVerbose & ZVB_FileEngine) {
    _DBGPRINT(" ZRawIndexFile::openIndexFile Index <%s> Duplicates <%d> <%s> file <%s>\n",
        IndexName.toString(), int(Duplicates), Duplicates ? "Yes":"No",
        pIndexUri.toString())
  }
  ZPMSStats = ZPMS ;
  return  ZS_SUCCESS ;
}//ZRawIndexFile::openIndexFile



ZStatus
ZRawIndexFile::closeIndexFile(void)
{

ZStatus wSt;
ZDataBuffer wICBContent;
//
// flush ICB to Index file
//
  if (getMode()==ZRF_NotOpen)
    return ZS_SUCCESS;

  size_t wRet=ZIndexControlBlock::_exportAppend(ZReserved);
//  setReservedContent(wICBContent);
/*  wSt=ZRandomFile::updateReservedBlock(ZIndexControlBlock::_exportAppend(wICBContent),true);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast( " Writing Reserved header for ZIndexFile %s",URIIndex.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
*/
    return  ZRandomFile::zclose();/* zclose updates file's blocks (header - index file control block etc.)*/
}//closeIndexFile



/**
 * @brief ZIndexFile::writeIndexControlBlock
 * @param pCheckSum a pointer to a pointer on checkSum field.
 * If nullptr, no checkSum is returned
 * if not nullptr, new checkSum on IndexControlBlock raw data is computed and pointer is returned.
 * @return ZStatus
 */
ZStatus
//ZRawIndexFile::writeIndexControlBlock(checkSum **pCheckSum)
ZRawIndexFile::writeIndexControlBlock() {
ZStatus wSt;
ZDataBuffer wICBContent;
//
// flush ICB to Index file and set ZFile_type to ZFT_IndexFile
//
    ZHeader.FileType = ZFT_ZIndexFile;     // setting ZFile_type

    size_t wRet=ZIndexControlBlock::_exportAppend(wICBContent);

    if (IdxKeyDic!=nullptr)
    {
      wICBContent.appendData(IdxKeyDic->_export());
    }

    ZReserved.setData(wICBContent);
    wSt=ZRandomFile::_writeAllFileHeader();
 //   wSt=ZRandomFile::updateReservedBlock(ZICB->_exportICB());
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast (" Writing Reserved header zone ZRawIndexFile %s",URIIndex.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
/*    if (pCheckSum==nullptr)
        {
        return  ZS_SUCCESS;        // no checkSum requested
        }
   (pCheckSum) = wICBContent.newcheckSum();  // else compute checksum and return pointer to it (pointer to pointer)
*/
  return  ZS_SUCCESS;
}//writeIndexControlBlock





#ifdef __DEPRECATED__
ZStatus getURFTypeAndSize( const unsigned char* pPtrIn , ZTypeBase& pType , size_t &pSize ) {

  const unsigned char* wPtrIn=pPtrIn;
    _importAtomic<ZTypeBase>(pType,wPtrIn) ;

    pSize=sizeof(ZTypeBase);

    /* URF size for atomic fields */
    if (pType & ZType_Atomic) {
      pSize +=  getAtomicUniversalSize(pType) ;

    }

    /* URF size of known objects : what is the size after ZTypeBase ?*/
    switch (pType) {

    case ZType_Utf8VaryingString: {/* see <ztoolset/utfvtemplatestring.cpp/_exportURF()> */
      URF_Varying_Size_type wSize;
      _importAtomic<URF_Varying_Size_type>(wSize,wPtrIn);
      pSize += sizeof(URF_Varying_Size_type)+size_t(wSize);
      break;
    }
    case ZType_Utf16VaryingString: {
      URF_Varying_Size_type wSize;
      _importAtomic<URF_Varying_Size_type>(wSize,wPtrIn);
      pSize += sizeof(URF_Varying_Size_type)+ (size_t(wSize)* sizeof(utf16_t));
      break;
    }
    case ZType_Utf32VaryingString: {
      URF_Varying_Size_type wSize;
      _importAtomic<URF_Varying_Size_type>(wSize,wPtrIn);
      pSize += sizeof(URF_Varying_Size_type)+ (size_t(wSize)* sizeof(utf32_t));
      break;
    }

    case ZType_Utf8FixedString: /* see <ztoolset/utftemplatestring.cpp/_exportURF()> */
    case ZType_Utf16FixedString:
    case ZType_Utf32FixedString: {
      URF_Fixed_Size_type wSize;
      wPtrIn += sizeof (URF_Capacity_type);
      _importAtomic<URF_Fixed_Size_type>(wSize,wPtrIn);
      pSize += sizeof(URF_Capacity_type) + sizeof (URF_Capacity_type) + size_t(wSize);
      return ZS_SUCCESS;
    }

    case ZType_Resource: /* see <zcontent/zcontentcommon/zresource.cpp> */
      pSize += sizeof(ZEntity_type) + sizeof(Resourceid_type);
      return ZS_SUCCESS;

    case ZType_ZDate:       /* see <ztoolset/zdate.cpp> */
      pSize += sizeof(uint32_t);
      return ZS_SUCCESS;
    case ZType_ZDateFull:   /* see <ztoolset/zdate.cpp> */
      pSize += sizeof(uint64_t);
      return ZS_SUCCESS;

    case ZType_CheckSum:  /* see <zcrypt/checksum.cpp> */
      pSize += cst_checksum ;
      return ZS_SUCCESS;
    case ZType_MD5: /* see <zcrypt/md5.cpp> */
      pSize += cst_md5 ;
      return ZS_SUCCESS;

    case ZType_bitsetFull: /* see <ztoolset/zbiset.cpp> */
      return ZS_SUCCESS;  /* ZType_bitsetFull has no data */
    case ZType_bitset: { /* see <ztoolset/zbiset.cpp> */
      uint16_t wBSSize;
      _importAtomic<uint16_t>(wBSSize,pPtrIn); /* effective byte size of biset object */
      pSize += sizeof(uint16_t); /* effective bit size of bitset object */
      pSize += size_t(wBSSize); /* size of data */
      return ZS_SUCCESS;
    }

    }// switch
  return ZS_SUCCESS;
}
#endif // __DEPRECATED__

ZStatus skipURFUntilPosition( const unsigned char* &pPtrIn,
                              const long pPosition,
                              const ZBitset& pFieldPresence) {

  if (!pFieldPresence.test(pPosition))
    return ZS_OMITTED;

  long wPos = 0;
  ZTypeBase wType ;
  ssize_t wSize;
  ZStatus wSt;
  /* search existing field offset */
  while (wPos < pPosition) {
    if (!pFieldPresence.test(wPos)){
      wPos++;
      continue;
    }
    wSt=URFParser::getURFTypeAndSize(pPtrIn,wType,wSize);  /* pPtrIn is not updated */
    if (wSt!=ZS_SUCCESS)
      return wSt;
    pPtrIn += wSize;
    wPos++;
  }// while (wPos < pPosition)

  /* extract urf field value */
  if (wPos != pPosition)
    return ZS_OUTBOUND;
  return ZS_SUCCESS;
}

ZStatus extractURF_Append( ZDataBuffer& pURFOut,const unsigned char* &pPtrIn) {
  size_t wByteSize;
  ZTypeBase wType ;

  /* extract field URF value according its ZType */

  _importAtomic<ZTypeBase>(wType,pPtrIn) ;

  /* URF size for atomic fields */
  if (wType & ZType_Atomic) {
    size_t wSize = getAtomicUniversalSize(wType)+sizeof(ZTypeBase);
    pPtrIn -= sizeof(ZTypeBase);
    pURFOut.appendData(pPtrIn,wSize);
    return ZS_SUCCESS;
  }

    switch (wType) {

    case ZType_Utf8VaryingString: /* see <ztoolset/utfvtemplatestring.cpp/_exportURF()> */
    {
      URF_UnitCount_type wSize1;
      _importAtomic<URF_UnitCount_type>(wSize1,pPtrIn);
      wByteSize=size_t(wSize1) * sizeof(utf8_t);
      pPtrIn -= sizeof(ZTypeBase) + sizeof(URF_UnitCount_type);
      wByteSize += sizeof(ZTypeBase) + sizeof(URF_UnitCount_type);
      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;
      return ZS_SUCCESS;
    }
    case ZType_Utf16VaryingString:
    {
      URF_UnitCount_type wSize1;
      _importAtomic<URF_UnitCount_type>(wSize1,pPtrIn);
      wByteSize=size_t(wSize1) * sizeof(utf16_t);
      pPtrIn -= sizeof(ZTypeBase) + sizeof(URF_UnitCount_type);
      wByteSize += sizeof(ZTypeBase) + sizeof(URF_UnitCount_type);
      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;
      return ZS_SUCCESS;
    }
    case ZType_Utf32VaryingString: {
      URF_UnitCount_type wSize1;
      _importAtomic<URF_UnitCount_type>(wSize1,pPtrIn);
      wByteSize=size_t(wSize1) * sizeof(utf32_t);
      pPtrIn -= sizeof(ZTypeBase) + sizeof(URF_UnitCount_type);
      wByteSize += sizeof(ZTypeBase) + sizeof(URF_UnitCount_type);
      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;
      return ZS_SUCCESS;
    }

    case ZType_Utf8FixedString: /* see <ztoolset/utftemplatestring.cpp/_exportURF()> */
    {
      URF_UnitCount_type wSize1;
      pPtrIn += sizeof(URF_Capacity_type);
      _importAtomic<URF_UnitCount_type>(wSize1,pPtrIn);
      wByteSize=size_t(wSize1);

      /* reset pointer to begining of URF data */
      pPtrIn -= sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
      wByteSize += sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + (sizeof(URF_UnitCount_type) * sizeof(utf8_t));
      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;
      return ZS_SUCCESS;
    }
    case ZType_Utf16FixedString:
    {
      URF_UnitCount_type wSize1;
      pPtrIn += sizeof(URF_Capacity_type);
      _importAtomic<URF_UnitCount_type>(wSize1,pPtrIn);
      wByteSize=size_t(wSize1);

      /* reset pointer to begining of URF data */
      pPtrIn -= sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
      wByteSize += sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + (sizeof(URF_UnitCount_type) * sizeof(utf16_t));
      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;
      return ZS_SUCCESS;
    }
    case ZType_Utf32FixedString: {
      URF_UnitCount_type wSize1;
      pPtrIn += sizeof(URF_Capacity_type);
      _importAtomic<URF_UnitCount_type>(wSize1,pPtrIn);
      wByteSize=size_t(wSize1);

      /* reset pointer to begining of URF data */
      pPtrIn -= sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
      wByteSize += sizeof(ZTypeBase)+sizeof(URF_Capacity_type) + (sizeof(URF_UnitCount_type) * sizeof(utf32_t));
      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;
      return ZS_SUCCESS;
    }

    case ZType_Resource: { /* see <zcontent/zcontentcommon/zresource.cpp> */

      wByteSize = sizeof(ZTypeBase) + sizeof(ZEntity_type) + sizeof(Resourceid_type);
      pPtrIn -= sizeof(ZTypeBase);

      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;

      return ZS_SUCCESS;
    }
    case ZType_ZDate: {      /* see <ztoolset/zdate.cpp> */
      wByteSize = sizeof(ZTypeBase) + sizeof(uint32_t);
      pPtrIn -= sizeof(ZTypeBase);

      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;

      return ZS_SUCCESS;
    }
    case ZType_ZDateFull: {  /* see <ztoolset/zdate.cpp> */
      wByteSize = sizeof(ZTypeBase) + sizeof(uint64_t);
      pPtrIn -= sizeof(ZTypeBase);

      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;

      return ZS_SUCCESS;
    }

    case ZType_CheckSum:  {/* see <zcrypt/checksum.cpp> */
      wByteSize = sizeof(ZTypeBase) + size_t(cst_checksum);
      pPtrIn -= sizeof(ZTypeBase);

      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;
      return ZS_SUCCESS;
    }
    case ZType_MD5: /* see <zcrypt/md5.cpp> */
      wByteSize = sizeof(ZTypeBase) + size_t(cst_md5);
      pPtrIn -= sizeof(ZTypeBase);

      pURFOut.appendData(pPtrIn,wByteSize);
      pPtrIn += wByteSize;
      return ZS_SUCCESS;

      /* other types cannot be part of a key */
    default:
      ZException.setMessage("extractURF",ZS_INVTYPE,Severity_Fatal, "Encountered data type <%s> that cannot be part of a key.\n",
          decode_ZType(wType));
      return ZS_INVTYPE;

    }// switch
    return ZS_INVTYPE;
}



ZStatus
ZRawIndexFile::extractAllURFKeyFields (ZDataBuffer& pKeyContent,
                              const unsigned char* &pPtrIn,
                              const ZArray<long> pPosition,
                              const ZBitset& pFieldPresence)
{
  ZStatus wSt = ZS_SUCCESS ;
  const unsigned char* wPtrIn=pPtrIn;
  pKeyContent.clear();
  for (long wi=0 ; (wi < pPosition.count()) && (wSt == ZS_SUCCESS) ; wi++) {
  ZStatus wSt=skipURFUntilPosition(wPtrIn,pPosition[wi],pFieldPresence);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wSt= extractURF_Append(pKeyContent,pPtrIn);
  }
  return wSt;
}


/**

        Has dictionary :                               No Dictionary

    ZRawRecord + Dictionary

    - key values extraction from record content        Application provides raw content and keys content
      -> keys raw content setup


                                ZRawRecord
                          has record content AND keys content

                                Keys insert

                                -> Key by key
                        - key prepare from record raw content :
                              seek for existing key value and check duplicates
                              reserve space within file

                                If successfull
                                -> Key by key

                        - key commit
                              Effective write


                                If errored
                                -> Key by key

                        - key rollback
                               Release reserved space


*/
/**
 * @brief ZRawIndexFile::_extractKeys extracts all defined keys from pRecordContent using pDictionary givin pKeysContent as a result.
 * @param pRecordContent
 * @param pDictionary
 * @param pKeysContent
 * @return
 */
ZStatus
ZRawIndexFile::_extractRawKey(ZDataBuffer& pRawRecord,   ZDataBuffer& pKeyContent)
{
  if (!isDebug()) {
    _DBGPRINT("ZRawIndexFile::_extractRawKey-F-DBGMODE Cannot extract key in Debug mode.\n")
    abort();
  }

  if (ZMFFather->extractRawKey_func==nullptr)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,ZS_NULLPTR,Severity_Severe,
        "Key extraction function has not been declared for index <%s> rank <%ld>.",IndexName.toCChar(),IndexRank);
    return ZS_NULLPTR;
    }
  return ZMFFather->extractRawKey_func (pRawRecord,this,&pKeyContent);
}//_extractRawKey

#ifdef __DEPRECATED__

/**
 * @brief ZRawIndexFile::_addKeyValue Adds a key value from a ZRawMasterFile record to the current registrated ZRawIndexFile instance.
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
ZRawIndexFile::_addRawKeyValue(ZRawRecord* pZMFRecord,  zrank_type& pIndexRank, zaddress_type pZMFAddress)
{

ZStatus wSt;

ZIndexItem* wIndexItem = new ZIndexItem ;

zrank_type wIndexIdxCommit;

    wIndexItem->ZMFaddress=pZMFAddress;
    wIndexItem->Operation=ZO_Push;

    wSt=_extractRawKey(pZMFRecord,wIndexItem->KeyContent);
    if (wSt!=ZS_SUCCESS)
      {  return  wSt;}// Beware return  is multiple instructions in debug mode
    wSt=_addRawKeyValue_Prepare(*wIndexItem,wIndexIdxCommit,pZMFAddress);
    if (wSt!=ZS_SUCCESS)
                  {  return  wSt;}// Beware return  is multiple instructions in debug mode
    wSt= _addRawKeyValue_Commit(wIndexItem,wIndexIdxCommit);
    delete wIndexItem;
    return  wSt;
}// _addKeyValue

#endif // __DEPRECATED__


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
                if ((wSt=ZRandomFile::_insert(ZRandomFile::ZDescriptor,wZI.toFileKey(),0L,wAddress))!=ZS_SUCCESS)  //! equivalent to push_front
                                        {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                        _Mtx.unlock();
#endif
                                        return (wSt); // error is managed at ZMF level
                                        }
                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                wZIXOp=ZO_Push ;
//                ZJoinIndex=this->size();
                if ((wSt=ZRandomFile::_add(ZRandomFile::ZDescriptor,wZI.toFileKey(),wAddress))!=ZS_SUCCESS)  //! equivalent to push
                                        {

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                        _Mtx.unlock();
#endif
                                        return (wSt); // error is managed at ZMF level
                                        }
                break;
                }
            case (ZS_NOTFOUND):
                {
                wZIXOp=ZO_Insert ;
                if ((wSt=ZRandomFile::_insert(ZRandomFile::ZDescriptor,wZI.toFileKey(),wRes.ZIdx,wAddress))!=ZS_SUCCESS)     // insert at position returned by seekGeneric
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return (wSt); // error is managed at ZMF level
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
                            return  (ZS_DUPLICATEKEY);
                            }
                wZIXOp=ZO_Insert ;
                if ((wSt=ZRandomFile::_insert(ZRandomFile::ZDescriptor,wZI.toFileKey(),wRes.ZIdx,wAddress))!=ZS_SUCCESS)     // insert at position returned by seekGeneric
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                             _Mtx.unlock();
#endif
                            return (wSt); // error is managed at ZAM level
                            }
//                ZJoinIndex=wRes.ZIdx;
                break;
                }
            default :
                {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                 _Mtx.unlock();
#endif
                return (ZS_INVOP);
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
    return (ZS_SUCCESS);

}//ZIndexFile::_addKeyValue
#endif // __COMMENT__
//------------------Add sequence-----------------------------------------


/**
 * @brief ZRawIndexFile::_addKeyValue_Prepare prepare to add an index rank. This will be committed or rolled back later using _addKeyValue_Commit or _Rollback (or HardRollback)

    Key data must have been extracted when calling _addKeyValue_Prepare and has to be available within ZSIndexItem::KeyContent

@par keyValueExtraction :
   - extracts fields values according their definition in ZKeyFieldList class (ZType, offset, length)
   - formats data : puts data field content into appropriate key format taking into account :
    + signed data (leading byte)
    + reverse byte order if required (if and only if system is little endian compliant)
   - concatenate data in fields order given by their position within CZKeyFieldList array ( rank 0 is first )
     . seek for extracted key value : reject if duplicate collision
     . prepare (ZRandomFile) to insert key appropriately (push, push_front or insert at given rank)



 * @param[in] pZMFRecord user's record content to extract key value from
 * @param[out] pIndexItem a pointer to a generated ZIndexItem that will contain the informations (ZMFAddress, IndexRank, Operation)
 * @param[in] pZMFAddress The ZMF block address to set the index record with
 * @return  a ZStatus. In case of error,
 * - Duplicate key not allowed : ZS_DUPLICATEKEY is returned without setting any ZException content,
 * - If another error is encountered, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawIndexFile::_addRawKeyValue_Prepare( ZIndexItem* &pOutIndexItem,
                                        ZDataBuffer& pKeyContent,
                                        const zaddress_type pZMFAddress)
{

ZStatus wSt=ZS_SUCCESS;
//ZIndexResult wZIR;
//zaddress_type wIndexAddress; /* returned index record address is not used */

ZIndexItem wIndexItem;

    pOutIndexItem=new ZIndexItem;
    pOutIndexItem->Operation = ZO_Add;
    pOutIndexItem->ZMFAddress = pZMFAddress ;  // store address to ZMF Block
    pOutIndexItem->setBuffer(pKeyContent) ;

//    long wIndexRank;
//    zaddress_type wIndexAddress,wZMFAddress;

    if (isSearchDycho())
      wSt = _URFsearchDychoUnique(pKeyContent,wIndexItem, ZLock_Nolock);
    else
      wSt = _URFsearchUnique(pKeyContent,wIndexItem, ZLock_Nolock);


#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mtx.lock();
#endif
  if (ZVerbose & ZVB_FileEngine)
      {
      _DBGPRINT ("_addKeyValue_Prepare : _Rawsearch Index <%s> Duplicates <%d> <%s> return status <%s> rank <%ld> \n",
        IndexName.toString(), int(Duplicates), Duplicates ? "Yes":"No",
        decode_ZStatus(wSt),wIndexItem.IndexRank)
      }

  switch (wSt){
    case (ZS_OUTBOUNDLOW):
      pOutIndexItem->Operation=ZO_Insert ;
      pOutIndexItem->IndexRank=0L;
//    ZJoinIndex=0;
      wSt=_insert2Phases_Prepare (pOutIndexItem->toFileKey(),pOutIndexItem->IndexRank,pOutIndexItem->IndexAddress);// equivalent to push_front
      if (ZVerbose & ZVB_FileEngine)
        _DBGPRINT ("_addKeyValue_Prepare : Index ZO_Insert (push front)  Index key  <%s> rank %ld index address %ld \n",
            IndexName.toCChar(),
            pOutIndexItem->IndexRank,pOutIndexItem->IndexAddress)
      break;

    case (ZS_OUTBOUNDHIGH):
      pOutIndexItem->Operation=ZO_Push ;
//    ZJoinIndex=this->size();
      wSt=ZRandomFile::_add2Phases_Prepare(pOutIndexItem->toFileKey(),pOutIndexItem->IndexRank,pOutIndexItem->IndexAddress);// equivalent to push
      if (ZVerbose & ZVB_FileEngine) {
        _DBGPRINT ("_addKeyValue_Prepare : Index Push (last) Index key  <%s> rank %ld index address %ld \n",
            IndexName.toCChar(),
            pOutIndexItem->IndexRank,pOutIndexItem->IndexAddress)
      }
      break;

    case (ZS_NOTFOUND):
      pOutIndexItem->Operation=ZO_Insert ;
      pOutIndexItem->IndexRank = wIndexItem.IndexRank ;
      wSt=ZRandomFile::_insert2Phases_Prepare(pOutIndexItem->toFileKey(),wIndexItem.IndexRank,pOutIndexItem->IndexAddress);// insert at position returned by seekGeneric
      if (ZVerbose & ZVB_FileEngine) {
        _DBGPRINT ("_addKeyValue_Prepare : Index ZO_Insert  Index key <%s> index rank %ld index address %ld \n",
            IndexName.toCChar(),
            pOutIndexItem->IndexRank,pOutIndexItem->IndexAddress)
      }
      break;

    case (ZS_FOUND):
      pOutIndexItem->IndexRank = wIndexItem.IndexRank;
      if (Duplicates==ZST_NODUPLICATES) {
        if (ZVerbose & ZVB_FileEngine)
          _DBGPRINT("_addKeyValue_Prepare : ***Index Duplicate Index key <%s> Duplicates <%d> <%s>  exception at index rank <%ld>\n",
              IndexName.toCChar(), int(Duplicates), Duplicates ? "Yes":"No",
              wIndexItem.IndexRank)
 /*       ZException.setMessage(_GET_FUNCTION_NAME_,
                                                    ZS_DUPLICATEKEY,
                                                    Severity_Error,
                                                    " Duplicate value on key set to ZST_NODUPLICATES index rank <%ld>",pOutIndexItem->IndexRank);
  */
        wSt=ZS_DUPLICATEKEY;
        break;
      } //  if no duplicates allowed
      pOutIndexItem->Operation=ZO_Insert ;


    /* if duplicates allowed */
      if (ZVerbose & ZVB_FileEngine) {
        _DBGPRINT ("_addKeyValue_Prepare : Index Index key <%s> Duplicate key valid insert at index rank <%ld>\n",
            IndexName.toCChar(),
            pOutIndexItem->IndexRank)
      }
//      pOutIndexItem->IndexRank=pOutIndexItem->IndexRank;
      wSt=ZRandomFile::_insert2Phases_Prepare(pOutIndexItem->toFileKey(),pOutIndexItem->IndexRank,pOutIndexItem->IndexAddress); // insert at position returned by seekGeneric

//    ZJoinIndex=wRes.ZIdx;
      break;

    default :// there was an error or a lock on file during seek operation on given key
//                wSt=ZS_INVOP;
      break;

  }// switch

    if (wSt!=ZS_SUCCESS)
        if (!ZException.stackIsEmpty())
            ZException.addToLast(" during Index _addKeyValue_Prepare on index <%s> ",
                                                 IndexName.toCChar());
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif

// statistical data is managed outside

/*    pStats.Timer.end();
    pStats.Timer.getDeltaTime();
    pStats = ZPMS- wPMS ;  // just give the delta : ZRFPMS is a base for zstatistics
    ZPMSStats += pStats;*/
    return  (wSt) ;

}//_addRawKeyValue_Prepare

ZStatus
//ZRawIndexFile::_rawKeyValue_Commit(ZIndexItem *pIndexItem, const zrank_type pZBATIndex)
ZRawIndexFile::_rawKeyValue_Commit(ZIndexItem *pIndexItem)

{
  ZStatus wSt;
  const char* wAction=nullptr;
  zaddress_type wAddress; // local index address : of no use there
  if (ZVerbose & ZVB_FileEngine)
    _DBGPRINT("ZRawIndexFile::_rawKeyValue_Commit  Commit index <%s> Operation is <%s> index rank <%ld> index address %ld zmf address %ld \n",
        IndexName.toCChar(),
        decode_ZOperation(pIndexItem->Operation).toCChar(),
        pIndexItem->IndexRank,
        pIndexItem->IndexAddress,
        pIndexItem->ZMFAddress)

  switch (pIndexItem->Operation) {
  case ZO_Insert :
    wSt=ZRandomFile::_insert2Phases_Commit(pIndexItem->toFileKey(),pIndexItem->IndexRank,wAddress);
    wAction="_insert2Phases_Commit";
    break;
  case ZO_Push :
  case ZO_Add :
    wSt=ZRandomFile::_add2Phases_Commit(pIndexItem->toFileKey(),pIndexItem->IndexRank,wAddress);
    wAction="_add2Phases_Commit";
    break;
  case ZO_Erase :
    wSt=ZRandomFile::_remove_Commit(pIndexItem->IndexRank);
    wAction="_remove_Commit";
    break;
  default:
    if (ZVerbose & ZVB_FileEngine)
      _DBGPRINT("ZRawIndexFile::_rawKeyValue_Commit-E-INVOP  Invalid commit operation code <%s> for index key <%s>",
          decode_ZOperation(pIndexItem->Operation).toCChar(),IndexName.toCChar())
    ZException.setMessage("ZRawIndexFile::_rawKeyValue_Commit",ZS_INVOP,Severity_Severe,
        "Invalid commit operation code <%s> for index key <%s>",
        decode_ZOperation(pIndexItem->Operation).toCChar(),IndexName.toCChar());
    return ZS_INVOP;
  }//switch

  if (ZVerbose & ZVB_FileEngine)
    _DBGPRINT("ZRawIndexFile::_rawKeyValue_Commit  index key <%s> commit operation <%s> done. status is <%s> \n",
        IndexName.toCChar(),wAction,decode_ZStatus(wSt))

  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" during Index commit operation <%s> action <%s> on index <%s> rank <%ld> ",
        wAction,
        decode_ZOperation(pIndexItem->Operation).toCChar(),
        IndexName.toCChar(),
        pIndexItem->IndexRank);
    ZException.setLastSeverity(Severity_Severe);
  }
  else
    pIndexItem->Operation |= ZO_Processed ;

  // history and journaling for indexes should take place here

  return  wSt;
} // _rawKeyValue_Commit


ZStatus
ZRawIndexFile::_rawKeyValue_Rollback(ZIndexItem *pIndexItem)
{
  ZStatus wSt;
  const char* wAction=nullptr;

  if (ZVerbose & ZVB_FileEngine)
    _DBGPRINT("ZRawIndexFile::_rawKeyValue_Rollback  index key <%s> rollback operation <%s>.\n",
        IndexName.toCChar(),decode_ZOperation(pIndexItem->Operation).toString())

  if (pIndexItem->Operation & ZO_Processed)
    return _rawKeyValue_HardRollback(pIndexItem);

  ZOp_type wOp = pIndexItem->Operation & ZO_OpMask;

  switch (wOp) {
  case ZO_Add :

    wSt=ZRandomFile::_add2Phases_Rollback (pIndexItem->IndexRank);
    wAction="_add2Phases_Rollback";
    break;
  case ZO_Insert :
    wSt=ZRandomFile::_insert2Phases_Rollback(pIndexItem->IndexRank);
    wAction="_insert2Phases_Rollback";
    break;
  case ZO_Erase :
    wSt=ZRandomFile::_remove_Rollback(pIndexItem->IndexRank);
    wAction="_remove_Rollback";
    break;
  default:
    ZException.setMessage("ZRawIndexFile::_rawKeyValue_Rollback",ZS_INVOP,Severity_Severe,
        "Invalid rollback operation code <%s> for index <%s>",decode_ZOperation(pIndexItem->Operation).toCChar(),IndexName.toCChar());
    return ZS_INVOP;
  }//switch

  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" during Index rollback operation <%s> action <%s> on index <%s> rank <%ld> ",
        wAction,
        decode_ZOperation(pIndexItem->Operation).toCChar(),
        IndexName.toCChar(),
        pIndexItem->IndexRank);
    ZException.setLastSeverity(Severity_Severe);
  }
  // history and journaling take place here
  pIndexItem->Operation |= ZO_RolledBack ;

  if (ZVerbose & ZVB_FileEngine)
    _DBGPRINT("ZRawIndexFile::_rawKeyValue_Rollback  index key <%s> rollback done. status is <%s> \n",
        IndexName.toCChar(),decode_ZStatus(wSt))

  return  wSt;
} // _rawKeyValue_Commit



/**
 * @brief ZRawIndexFile::_addKeyValue_HardRollback routine used when
 *        - index key add operation has been fully committed
 *    and
 *        - a problem occurred on Master file
 *
    So there is need to remove key Index rank at pIndexCommit to restore situation as aligned with Master file.

* @param pIndexCommit
     * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
                                                                                 */

ZStatus
ZRawIndexFile::_rawKeyValue_HardRollback(ZIndexItem *pIndexItem)
{
  const char* wAction=nullptr;
  ZStatus wSt;
  zaddress_type wAddress;

  ZOp_type wOp = pIndexItem->Operation & ZO_OpMask ;

  if (ZVerbose & ZVB_FileEngine)
    _DBGPRINT("ZRawIndexFile::_rawKeyValue_HardRollback  index key <%s> hard rollback operation <%s> requested.\n",
        IndexName.toCChar(),decode_ZOperation(wOp).toString())

  switch (wOp) {
  case ZO_Add :
    wAction="remove";
    wSt=ZRandomFile::_remove (pIndexItem->IndexRank);
    break;
  case ZO_Insert :
    wAction="remove";
    wSt=ZRandomFile::_remove (pIndexItem->IndexRank);
    break;
  case ZO_Erase :
    wAction="insert";
    wSt=ZRandomFile::_insert(pIndexItem->toFileKey(),pIndexItem->IndexRank,wAddress);
    break;
  default:
    ZException.setMessage("ZRawIndexFile::_rawKeyValue_HardRollback",ZS_INVOP,Severity_Severe,
        "Invalid hard rollback operation code <%s> for index <%s>",
        decode_ZOperation(pIndexItem->Operation).toCChar(),IndexName.toCChar());
    return ZS_INVOP;
  }//switch


  if (ZVerbose & ZVB_FileEngine)
    _DBGPRINT( "ZRawIndexFile::_rawKeyValue_HardRollback Index  key <%s> raw Key Value Hard rollback of op <%s> action <%s>  index rank <%ld>\n",
                IndexName.toCChar(),
                decode_ZOperation(pIndexItem->Operation).toCChar(),
                wAction,
                pIndexItem->IndexRank)

  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" during Index raw Key Value Hard rollback of op <%s> action <%s>  on index rank <%s> rank <%ld> ",
        decode_ZOperation(pIndexItem->Operation).toCChar(),
        wAction,
        IndexName.toCChar(),
        pIndexItem->IndexRank);
    ZException.setLastSeverity(Severity_Severe);
  }

  pIndexItem->Operation |= ZO_HardRolledBack ;

  return  wSt;
} // _addKeyValue_HardRollback

#ifdef __DEPRECATED__
/**
 * @brief ZIndexFile::_addKeyValue_Rollback used when a problem occurred when
 *    - index file ZBAT has been reserved for index key at pIndexCommit
 *    - it must be released to available pool
 * @param pIndexCommit
 * @return
 */
ZStatus
ZRawIndexFile::_addRawKeyValue_Rollback(const zrank_type pIndexCommit)
{
ZStatus wSt;
    wSt=ZRandomFile::_add2Phases_Rollback (pIndexCommit);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" during Index _addKeyValue_Rollback (Soft rollback) on index <%s> rank <%02ld> ",
                                                 IndexName.toCChar(),
                                                 pIndexCommit);
            ZException.setLastSeverity(Severity_Severe);
            }

// No history and no journaling for indexes

    return  wSt;
} // _addKeyValue_Rollback

/**
 * @brief ZRawIndexFile::_addKeyValue_HardRollback routine used when
 *        - index key add operation has been fully committed
 *    and
 *        - a problem occurred on Master file
 *
    So there is need to remove key Index rank at pIndexCommit to restore situation as aligned with Master file.

 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRawIndexFile::_addKeyValue_HardRollback(const zrank_type pIndexCommit)
{

  if (ZVerbose)
            fprintf (stdout,"Index addKeyValue Hard rollback : removing index <%s> rank <%ld>\n",
                     IndexName.toCChar(),
                    pIndexCommit);

  ZStatus wSt =ZRandomFile::_remove(pIndexCommit);
  if (wSt!=ZS_SUCCESS)
    {
    ZException.addToLast(" during Index _addKeyValue_HardRollback (hard rollback) on index <%s> rank <%02ld> ",
                         IndexName.toCChar(),
                         pIndexCommit);
    ZException.setLastSeverity(Severity_Severe);
    }
  return  wSt;
} // _addKeyValue_HardRollback
#endif // __DEPRECATED__

#ifdef __OLD_VERSION__
ZStatus
ZRawIndexFile::_addRawKeyValue_Prepare(ZIndexItem *&pIndexItem,
    zrank_type &pZBATIndex,
    const zaddress_type pZMFAddress)
{

  ZStatus wSt=ZS_SUCCESS;
  //zaddress_type wZMFAddress;
  //zaddress_type wIndexAddress; // not used but necessary for base ZRandomFile class calls
  ZIndexResult wZIR;

  ZIFCompare wZIFCompare = ZKeyCompareBinary;


  //-----------Comparison routine selection------------------------------------

  if (IdxKeyDic->size()==1)           // if only one field
  {
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

  /* key data is supposed to have been extracted when coming here */
  //    _keyValueExtraction(ZICB->ZKDic,pZMFRecord,pIndexItem->KeyContent);        // create key value content from user record

  //    wSt= _Rawsearch (pIndexItem->KeyContent,*this,wZIR,ZMS_MatchIndexSize,wZIFCompare);  // will not use wStats except for special seek mesure on add
  wSt= _Rawsearch (pIndexItem->KeyContent,*this,wZIR,ZLock_Nolock);  // will not use wStats except for special seek mesure on add

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mtx.lock();
#endif
  if (ZVerbose)
  {
    _DBGPRINT ("_addKeyValue_Prepare : _Rawsearch return status <%s> rank <%ld>\n", decode_ZStatus(wSt),wZIR.IndexRank);
  }

  switch (wSt)
  {
  case (ZS_OUTBOUNDLOW):
  {
    pIndexItem->Operation=ZO_Push_front ;
    //                ZJoinIndex=0;
    pZBATIndex=0L;
    wSt=_insert2Phases_Prepare (pIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress);// equivalent to push_front
    if (ZVerbose)
      _DBGPRINT ("Index Push_Front  (index rank 0L )\n");

    break;
  }
  case (ZS_OUTBOUNDHIGH):
  {
    pIndexItem->Operation=ZO_Push ;
    //                ZJoinIndex=this->size();
    wSt=ZRandomFile::_add2Phases_Prepare(pIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress);// equivalent to push
    if (ZVerbose)
      _DBGPRINT ("Index Push\n");
    break;
  }
  case (ZS_NOTFOUND):
  {
    pIndexItem->Operation=ZO_Insert ;
    pZBATIndex=wZIR.IndexRank;
    wSt=ZRandomFile::_insert2Phases_Prepare(pIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress);// insert at position returned by seekGeneric
    //                ZJoinIndex=wRes.ZIdx;
    if (ZVerbose)
      _DBGPRINT ("Index insert at rank <%ld>\n", wZIR.IndexRank);
    break;
  }
  case (ZS_FOUND):
  {
    if (Duplicates==ZST_NODUPLICATES)
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
      _DBGPRINT ("Index Duplicate key insert at rank <%ld>\n", wZIR.IndexRank)
    pZBATIndex=wZIR.IndexRank;
    wSt=ZRandomFile::_insert2Phases_Prepare(pIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress); // insert at position returned by seekGeneric

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
                  IndexName.toCChar());
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mtx.unlock();
#endif

  // statistical data is managed outside

  /*    pStats.Timer.end();
    pStats.Timer.getDeltaTime();
    pStats = ZPMS- wPMS ;  // just give the delta : ZRFPMS is a base for zstatistics
    ZPMSStats += pStats;*/
  return  (wSt) ;

}//_addRawKeyValue_Prepare

ZStatus
ZRawIndexFile::_addRawKeyValue_Commit(ZIndexItem *pIndexItem, const zrank_type pZBATIndex)
{

  ZStatus wSt;
  zaddress_type wAddress; // local index address : of no use there

  wSt=ZRandomFile::_add2Phases_Commit(pIndexItem->toFileKey(),pZBATIndex,wAddress);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" during Index _addKeyValue_Commit on index <%s> rank <%02ld> ",
        IndexName.toCChar(),
        pZBATIndex);
    ZException.setLastSeverity(Severity_Severe);
  }
  // history and journaling take place here

  return  wSt;
} // _addKeyValue_Commit


/**
 * @brief ZIndexFile::_addKeyValue_Rollback used when a problem occurred when
 *    - index file ZBAT has been reserved for index key at pIndexCommit
 *    - it must be released to available pool
 * @param pIndexCommit
 * @return
 */
ZStatus
ZRawIndexFile::_addRawKeyValue_Rollback(const zrank_type pIndexCommit)
{

  ZStatus wSt;
  wSt=ZRandomFile::_add2Phases_Rollback (pIndexCommit);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" during Index _addKeyValue_Rollback (Soft rollback) on index <%s> rank <%02ld> ",
        IndexName.toCChar(),
        pIndexCommit);
    ZException.setLastSeverity(Severity_Severe);
  }

  // No history and no journaling for indexes

  return  wSt;
} // _addKeyValue_Rollback

/**
 * @brief ZRawIndexFile::_addKeyValue_HardRollback routine used when
 *        - index key add operation has been fully committed
 *    and
 *        - a problem occurred on Master file
 *
    So there is need to remove key Index rank at pIndexCommit to restore situation as aligned with Master file.

* @param pIndexCommit
     * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
                                                                                 */

        ZStatus
        ZRawIndexFile::_addKeyValue_HardRollback(const zrank_type pIndexCommit)
{

  if (ZVerbose)
    fprintf (stdout,"Index addKeyValue Hard rollback : removing index <%s> rank <%ld>\n",
        IndexName.toCChar(),
        pIndexCommit);

  ZStatus wSt =ZRandomFile::_remove(pIndexCommit);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" during Index _addKeyValue_HardRollback (hard rollback) on index <%s> rank <%02ld> ",
        IndexName.toCChar(),
        pIndexCommit);
    ZException.setLastSeverity(Severity_Severe);
  }
  return  wSt;
} // _addKeyValue_HardRollback
#endif // __OLD_VERSION__

//------------------End Add sequence-----------------------------------------

//------------------Remove sequence-----------------------------------------


/**
 * @brief ZRawIndexFile::_removeKeyValue_Prepare  Prepares to remove an index rank corresponding to given pKey (ZDataBuffer)
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
 * NB: Not found status got on index remove operation is an invalid status and must be signalled as an error
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
ZRawIndexFile::_removeRawKeyValue_Prepare(ZIndexItem* &pOutIndexItem,
                                        zrank_type &pIndexRank,
                                        ZDataBuffer & pKey,
                                        zaddress_type &pIndexAddress)
{

ZStatus wSt;
ZResult wRes;
zaddress_type wIndexAddress,wZMFAddress;
//ZIndexResult wZIResult;
ZIndexItem wIndexItem;
  pOutIndexItem= new ZIndexItem;

//  wSt = _URFsearchDychoUnique(pKey,pIndexRank,wIndexAddress,wZMFAddress,ZLock_Nolock);
  wSt = _URFsearchUnique(pKey,wIndexItem,ZLock_Nolock);
  /* only status valid is ZS_FOUND */
  if (wSt!=ZS_FOUND)  // Return status is either not found, record lock or other error
  {
    ZException.setMessage(  "ZRawIndexFile::_removeKeyValue_Prepare",
                            wRes.ZSt,
                            Severity_Severe,
                            "During remove operation : Index value not found on index name <%s>",
                            IndexName.toCChar());
    return  wSt;
  } // wSt!=ZS_FOUND

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.lock();
#endif

  pOutIndexItem->_copyFrom(wIndexItem);

  pOutIndexItem->Operation=ZO_Erase ;


  // Do not confuse ZMFAddress and index record address that will be suppressed

//  pIndexAddress = pOutIndexItem->IndexAddress  ;

  wSt=ZRandomFile::_remove_Prepare(pOutIndexItem->IndexRank,pOutIndexItem->IndexAddress);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif

  return  wSt;
}//_removeKeyValue_Prepare

#ifdef __DEPRECATED__
/**
 * @brief ZRawIndexFile::_removeIndexItem_Prepare  ZSIndexItem::KeyContent must content key value to erase in input.
 * @param pIndexItem
 * @param pIndexRank
 * @return
 */
ZStatus
ZRawIndexFile::_removeIndexItem_Prepare(ZIndexItem &pIndexItem,long & pIndexRank)
{
  ZStatus wSt;
  ZResult wRes;
  //zaddress_type wIndexAddress;
  ZIndexResult wZIResult;

  //ZIFCompare         wZIFCompare = ZKeyCompareBinary;

  //-----------Comparison routine selection------------------------------------


  //    wSt=_Rawsearch(pKey, *this,wZIR,ZMS_MatchIndexSize,wZIFCompare);
//  wSt=_Rawsearch(pIndexItem.KeyContent, *this,wZIResult,ZLock_Nolock);
  wSt = _URFsearch(pIndexItem,wZIResult,ZLock_Nolock);
  if (wSt!=ZS_FOUND)  // Return status is either not found, record lock or other error
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        wRes.ZSt,
        Severity_Error,
        "During remove operation : Index value not found on index name <%s> ",
        IndexName.toCChar());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  } // wSt!=ZS_FOUND

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mtx.lock();
#endif

  pIndexRank=wZIResult.IndexRank ;

  pIndexItem.Operation=ZO_Erase ;
  pIndexItem.ZMFaddress = 0 ; // dont know yet

  wSt=ZRandomFile::_remove_Prepare(wZIResult.IndexRank,wZIResult.ZMFAddress);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mtx.unlock();
#endif

  return  wSt;
}//_removeIndexItem_Prepare



ZStatus
ZRawIndexFile::_removeKeyValue_Commit(const zrank_type pIndexCommit)
{

zrank_type ZJoinIndex;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif


    wSt=ZRandomFile::_remove_Commit(pIndexCommit);

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" during removeKeyValue_Commit on index <%s> number <%02ld> ",
                                             IndexName.toCChar(),
                                             pIndexCommit);
        ZException.setLastSeverity(Severity_Severe);
        }

     return (wSt);

}//ZIndexFile::_removeKeyValue_Prepare



/**
 * @brief ZIndexFile::_removeKeyValue_Rollback
 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawIndexFile::_removeKeyValue_Rollback(const zrank_type pIndexCommit)
{
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif


    wSt=ZRandomFile::_remove_Rollback(pIndexCommit);

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" during removeKeyValue_Rollback (soft rollback) on index <%s> number <%02ld> ",
                             IndexName.toCChar(),
                             pIndexCommit);
        ZException.last().Severity=Severity_Severe;
        }

    return  wSt;
}//_removeKeyValue_Rollback

/**
 * @brief ZIndexFile::_removeKeyValue_HardRollback
 * @param pIndexItem
 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawIndexFile::_removeKeyValue_HardRollback(ZIndexItem* pIndexItem, const zrank_type pIndexCommit)
{


zrank_type ZJoinIndex;
zaddress_type wAddress;  // not used : only for compatibility purpose

ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif

    wSt= ZRandomFile::_insert(pIndexItem->toFileKey(),pIndexCommit,wAddress);

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
     _Mtx.unlock();
#endif

    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" during _removeKeyValue_HardRollback (hard rollback) on index <%s> number <%02ld> ",
                             IndexName.toCChar(),
                             pIndexCommit);
        ZException.setLastSeverity(Severity_Severe);
        }

     return  wSt ;

}//_removeKeyValue_HardRollback

//#endif // __COMMENT__
#endif //__DEPRECATED__
//-------------------Generic Functions---------------------------------------

namespace zbs{

/** @cond Development */
//#ifdef __COMMENT__
/**
 * @brief _KeyValueExtraction Extracts the Key value from ZMasterFile record data using dictionnary CZKeyDictionary fields definition
 *
 * return s the concaneted key value in pKey ZDataBuffer.
 * - Key fields are extracted from the ZMasterFile user record .
 * - They are converted appropriately whenever required using base internal conversion routines according Dictionary data type ( ZType_type ):
 *    + atomic fields _getAtomicFromRecord()
 *    + arrays _getArrayFromRecord()
 *    + for data type Class (ZType_type) : data is simply mass-moved to key without any conversion
 *
 * @note As we are processing variable length records, if a defined key field points outside the record length,
 *       then its return ing key value is set to binary zero on the corresponding length of the field within returned Key value.
 *
 *
 *
 * @param[in] pZKDic  ZIndex dictionary (part of ZIndexControlBlock) for the index to extract key for
 * @param[in pRecord  ZMasterFile user record to extract key from
 * @param[out] pKey  Resulting concatenated key content
 * @return
 */
#ifdef __DEPRECATED__
inline
ZStatus _keyValueExtraction(ZIndexFile* pZIF, ZRecord &pRecord, ZDataBuffer& pKeyOut)
{
ZStatus wSt;
size_t wKeyOffset = 0;
size_t wSize =0,wRecordOffset=0;
long wRDicRank=0;
ZDataBuffer wFieldUValue;

// at this stage _recomputeSize should have been done and total sizes should be OK.



    if (pZIF->IdxKeyDic->isEmpty())
            {
            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_BADDIC,
                                     Severity_Severe,
                                     " Index Control Block appears to be malformed. Key dictionary fields list is empty");
            return  ZS_BADDIC;
            }

    if (!pZIF->IdxKeyDic->Recomputed)
            pZIF->IdxKeyDic->computeKeyUniversalSize();

    pKeyOut.allocateBZero(pZIF->KeyUniversalSize +1);

    for (long wi=0;wi<pZIF->IdxKeyDic->count();wi++)
        {
// here put extraction rules. RFFU : Extraction could be complex. To be investigated and implemented

        wRDicRank=pZIF->IdxKeyDic->Tab(wi).MDicRank;
        pRecord.getUniversalbyRank(wFieldUValue,wRDicRank);
        pKeyOut.changeData(wFieldUValue,wKeyOffset);

        wKeyOffset += pRecord.RDic->Tab[wRDicRank].MDicField->UniversalSize;
        }//for



return  ZS_SUCCESS;
}//zKeyValueExtraction
#endif // __DEPRECATED__


//#endif // __COMMENT__

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
  if ((wSize < 0) || (pSize > pKey1.Size))
    wSize=pKey1.Size;
  return (memcmp(pKey1.Data,pKey2.Data,wSize));
}// ZKeyCompareBinary

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
    return (strncmp((char*)pKey1.Data,(char*)pKey2.Data,wSize));
}// ZKeyCompareAlpha


} // namespace zbs

//-----------------------Index Search routines------------------------------------------



/**
 * @brief URFCompare  Compare two buffers composed each of one or many URF fields, each field potentially of variable length.
 */
/*
 * int URFCompare(const ZDataBuffer &pKey1, ZDataBuffer &pKey2) {

  return URFComparePtr(pKey1.Data,pKey1.Size,pKey2.Data,pKey2.Size);
} // URFCompare
*/

void displayURFCompare(long pRank,int pR,unsigned char* pPtr1, unsigned char* pPtr2) {
  utf8VaryingString wValue1 , wValue2;
  const unsigned char* wPtr1=pPtr1, * wPtr2=pPtr2;
  wValue1 = URFParser::displayOneURFField(wPtr1);
  wValue2 = URFParser::displayOneURFField(wPtr2);

  if (pR == 0) {
    _DBGPRINT("rank <%ld> %s is equal to %s.\n",pRank,wValue1.toCChar(),wValue2.toCChar())
    std::cout.flush();
    return;
  }
  if (pR > 0) {
    _DBGPRINT("rank <%ld> %s greater than %s.\n",pRank,wValue1.toCChar(),wValue2.toCChar())
    std::cout.flush();
    return;
  }
  _DBGPRINT("rank <%ld> %s less than %s.\n",pRank,wValue1.toCChar(),wValue2.toCChar())
  std::cout.flush();
  return;
}


/**
 * @brief ZRawIndexFile::_URFsearchUnique Searches for a UNIQUE value for a given index without duplicates and in any cases
 *  sets pOutIndexItem with accurate values :
 * - key content,
 * - master file address ,
 * - last accessed index record address,
 * - last accessed index record rank
 *
 *  This search may be EXACT or PARTIAL.
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
 * Returns a ZSIndexResult structure for the unique index reference if found : Index record rank ; ZMasterFile corresponding address
 * see @ref ZSIndexResult.
 *
 * @param[in] pKeyToSearch key content to find : one or more URF formatted fields
 *
 * @param[out] pOutIndexItem a ZIndexItem object with ZIndexFile relative key position (rank within index file) corresponding to key content if found
 *                  associated to ZMasterFile corresponding record (block) address and many other informations.
 * @param[in] pLock a zlock_type mask build using ZLockMask_type containing the lock mask to apply onto the found records.
 * @note this lock mask applies to Master File record. Indexes are never locked.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
* - ZS_FOUND in case of success (And NOT ZS_SUCCESS). pOutIndexItem is filled with found index record values.
* - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value. pOutIndexItem is filled with last processed index record values.
* - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value. pOutIndexItem is filled with last processed index record values.
* - ZS_NOTFOUND if key value is not found in the middle of index values set.
*       pOutIndexItem is filled with last processed index record values i. e. exact index to insert key if necessary.
* other status : any status that may be given by underneeth ZRandowFile access routines. These are severe errors.
 */
ZStatus
ZRawIndexFile::_URFsearchDychoUnique(  const ZDataBuffer &pKeyToSearch,
                                        ZIndexItem& pOutIndexItem,
                                        const zlockmask_type pLock)
{
  ZStatus     wSt= ZS_NOTFOUND;
//  ZIndexItem  wIndexItem;

  long              wPreviousRank;
  zaddress_type     wPreviousAddress;

  pOutIndexItem.IndexRank=0L;

  ZDataBuffer wIndexRecord;

  ZIndexItem wPrevious;

  int wR=0;


  long whigh = 0;
  long wlow = 0;
  long wpivot = 0;


  if (getSize() == 0L) {
    wSt= ZS_OUTBOUNDHIGH ;
    goto _URFsearch_Return;
  }

  whigh = lastIdx();  // last element : highest boundary
  wlow = 0;           // first element : lowest boundary


  wSt=_URFtestRank(pKeyToSearch,wlow,pOutIndexItem,wR,pLock);
  if (wSt!=ZS_SUCCESS)
    goto _URFsearch_Return;

  if (wR==0) {
    wSt=ZS_FOUND ;
    if (ZVerbose & ZVB_SearchEngine)
      _DBGPRINT("ZRawIndexFile::_URFsearchDychoUnique  Match index at rank %ld index address %ld\n",pOutIndexItem.IndexRank,pOutIndexItem.IndexAddress)
    goto _URFsearch_Return;
  }
  if (wR<0) {
    if (ZVerbose & ZVB_SearchEngine)
      _DBGPRINT("ZRawIndexFile::_URFsearchDychoUnique ZS_OUTBOUNDLOW rank %ld\n",pOutIndexItem.IndexRank)
    wSt =  ZS_OUTBOUNDLOW;
    goto _URFsearch_Return;
  }

/* there wR > 0 : key to search is greater than low boundary */

  wSt=_URFtestRank(pKeyToSearch,whigh,pOutIndexItem,wR,pLock);
  if (wSt!=ZS_SUCCESS)
    goto _URFsearch_Return;

  if (wR==0) {
    wSt=ZS_FOUND ;
    if (ZVerbose & ZVB_SearchEngine)
      _DBGPRINT("ZRawIndexFile::_URFsearchUnique  Match index at rank %ld index address %ld zmf address %ld\n",pOutIndexItem.IndexRank,pOutIndexItem.IndexAddress,pOutIndexItem.ZMFAddress)
    wSt=ZS_FOUND ;
    return wSt ;
  }

  if (wR>0) {
    if (ZVerbose & ZVB_SearchEngine)
      _DBGPRINT("ZRawIndexFile::_URFsearchDychoUnique ZS_OUTBOUNDHIGH rank %ld\n",pOutIndexItem.IndexRank)
    return  ZS_OUTBOUNDHIGH;
    }

  /* key to search is less than high boundary */

//  wpivot = ((whigh-wlow)/2) + wlow ;
  wpivot = ((whigh-wlow)/2) ;

  while ((whigh-wlow) > 4)// ---------------Main loop around pivot----------------------
  {
    wSt=_URFtestRank(pKeyToSearch,wpivot,pOutIndexItem,wR,pLock);
    if (wSt!=ZS_SUCCESS)
      goto _URFsearch_Return;

    if (wR==0) {
      if (ZVerbose & ZVB_SearchEngine)
        _DBGPRINT("ZRawIndexFile::_URFsearchDychoUnique  Match index at rank %ld index address %ld zmf address %ld\n",pOutIndexItem.IndexRank,pOutIndexItem.IndexAddress,pOutIndexItem.ZMFAddress)
      wSt=ZS_FOUND ;
      return wSt ;
    }

    if (wR > 0)
      wlow = wpivot ;
    else
      whigh = wpivot ;
/*
    if ((whigh-wlow) < 3) {
      break;
    }
*/
    wpivot = ((whigh - wlow) / 2) + wlow ;
    ZPMSStats.Iterations ++;
  }// while (whigh-wlow)>2---------------Main loop around pivot----------------------

  /*
   *
   *  Process the remaining collection : here at maximum, we have whigh - wlow equals 2 or less
   */

  wpivot = wlow;
  wPreviousRank =  pOutIndexItem.IndexRank = wpivot;
  wPreviousAddress = pOutIndexItem.IndexAddress ;

  wSt=_URFtestRank(pKeyToSearch,wpivot,pOutIndexItem,wR,pLock);
  if (wSt!=ZS_SUCCESS)
    goto _URFsearch_Return;

  while ((wpivot <= whigh)&& (wR > 0))
  {
    wPreviousRank = pOutIndexItem.IndexRank;
    wPreviousAddress = pOutIndexItem.IndexAddress;

    wpivot ++;

    wSt=_URFtestRank(pKeyToSearch,wpivot,pOutIndexItem,wR,pLock);
    if (wSt!=ZS_SUCCESS)
      goto _URFsearch_Return;
  }

  pOutIndexItem.IndexRank = wpivot ;

  if (wR==0) {
    if (ZVerbose & ZVB_SearchEngine)
      _DBGPRINT("ZRawIndexFile::_URFsearchDychoUnique  Match index at rank %ld index address %ld zmf address %ld\n",
          pOutIndexItem.IndexRank,pOutIndexItem.IndexAddress,pOutIndexItem.ZMFAddress)
    wSt=ZS_FOUND;
    goto _URFsearch_Return;
  }
  /* here search key is greater than compare key */
  /* get the rank before */
//  pOutIndexItem->_copyFrom( wPrevious );

//  pIndexRank = wPreviousRank;
  if (ZVerbose & ZVB_SearchEngine)
    _DBGPRINT("ZRawIndexFile::_URFsearchDychoUnique ZS_NOTFOUND last index rank %ld last index address %ld last zmf address %ld\\n",
        pOutIndexItem.IndexRank,pOutIndexItem.IndexAddress,pOutIndexItem.ZMFAddress)
  return  ZS_NOTFOUND ;

_URFsearch_Return:
  /*    if ((wSt=ZS_FOUND)&&(pLock != ZLock_Nolock ))
            {
            return  static_cast<ZMasterFile*>(pZIF.ZMFFather)->zlockByAddress(wIndexItem.ZMFaddress,pLock); // lock corresponding ZMasterFile address with given lock mask
            }*/
  return  (wSt) ;
}// _URFsearch

ZStatus
ZRawIndexFile::_URFtestRank(const ZDataBuffer &pKeyToSearch,
                            const long        pIndexRank,
                            ZIndexItem        &pIndexItem ,
                            int               &pReturn,
                            const zlockmask_type pLock)
{
ZDataBuffer wIndexRecord;
ZStatus wSt;

  pIndexItem.IndexRank = pIndexRank;

  if (getSize() == 0L) {
    return  ZS_OUTBOUNDHIGH;
  }

  if (pIndexRank > lastIdx()) {
    return  ZS_OUTBOUNDHIGH;
  }

  ZPMSStats.Reads ++;
  if ((wSt=zgetWAddress(wIndexRecord,pIndexItem.IndexRank,pIndexItem.IndexAddress)) != ZS_SUCCESS) {
    return wSt;
  }

  if (wIndexRecord.isEmpty()) {
    ZException.setMessage("ZRawIndexFile::_URFtestRank",ZS_EMPTY,Severity_Severe,"Index file <%s> : index record rank %ld is empty.",
                            pIndexRank,
                            getURIContent().toString());
    return ZS_EMPTY;
  }

  pIndexItem.fromFileKey(wIndexRecord);

  /* NB: ZMF address is given by ZIndexItem::fromFileKey() routine */

  pReturn = URFComparePtr(pKeyToSearch.Data,pKeyToSearch.Size,pIndexItem.Data,pIndexItem.Size);
  if (ZVerbose & ZVB_SearchEngine)
    displayURFCompare(pIndexRank,pReturn,pKeyToSearch.Data,pIndexItem.Data);

  return ZS_SUCCESS ;
}

ZStatus
ZRawIndexFile::_URFsearchUnique(  const ZDataBuffer &pKeyToSearch,
                                  ZIndexItem        &pIndexItem,
                                  const zlockmask_type pLock)
{
  ZStatus     wSt= ZS_NOTFOUND;
  int wR;
  long wIndexRank=0L;
  pIndexItem.IndexAddress = -1;

  wSt=_URFtestRank(pKeyToSearch,wIndexRank,pIndexItem,wR,pLock);

  /* while search key is greater than found item */

  while  (( wR > 0 ) && (wSt==ZS_SUCCESS) && (wIndexRank<= lastIdx()))  {
    wIndexRank++;
    wSt=_URFtestRank(pKeyToSearch,wIndexRank,pIndexItem,wR,pLock);
  }// while ( wR > 0 )

  if (wSt!=ZS_SUCCESS) {
    goto _URFsearch_Return;
  }
  if (wR==0) {
    wSt=ZS_FOUND ;
    pIndexItem.IndexRank=wIndexRank;
    pIndexItem.IndexAddress=wIndexRank;
    goto _URFsearch_Return;
  }

  if (wIndexRank > lastIdx()) {
    wSt=ZS_OUTBOUNDHIGH;
    goto _URFsearch_Return;
  }

  /* here wR is less than 0 */

  wSt=ZS_NOTFOUND;
  pIndexItem.IndexRank=wIndexRank;
//  goto _URFsearch_Return;


_URFsearch_Return:
  if (ZVerbose & ZVB_SearchEngine)
    _DBGPRINT("ZRawIndexFile::_URFsearchUnique  Index key <%s> status <%s> last index rank %ld last index address %ld last zmf address %ld \n",
              IndexName.toCChar(),
              decode_ZStatus(wSt),
              pIndexItem.IndexRank,pIndexItem.IndexAddress,pIndexItem.ZMFAddress)
  /*    if ((wSt==ZS_FOUND)&&(pLock != ZLock_Nolock ))
            {
            return  static_cast<ZMasterFile*>(pZIF.ZMFFather)->zlockByAddress(wIndexItem.ZMFaddress,pLock); // lock corresponding ZMasterFile address with given lock mask
            }*/
  return  (wSt) ;
}// _URFsearch


ZStatus
ZRawIndexFile::_URFsearchAll( const ZDataBuffer     &pKeyToSearch,
                                ZIndexCollection      &pIndexCollection,
                                const zlockmask_type   pLock)
{
  ZStatus     wSt= ZS_NOTFOUND;
  int wR;
  long wIndexRank=0L;
  ZIndexItem pIndexItem;
  pIndexItem.IndexAddress = -1;
  pIndexItem.ZMFAddress = -1;
  pIndexItem.IndexRank = -1;

  pIndexCollection.clear();


  wSt=_URFtestRank(pKeyToSearch,wIndexRank,pIndexItem,wR,pLock);

  /* while search key is greater than found item */

  while  (( wR > 0 ) && (wSt==ZS_SUCCESS) && (wIndexRank<= lastIdx()))  {
    wIndexRank++;
    wSt=_URFtestRank(pKeyToSearch,wIndexRank,pIndexItem,wR,pLock);
  }// while ( wR > 0 )

  if (wSt!=ZS_SUCCESS) {
    goto _URFsearch_Return;
  }
  if (wR==0) { /* if found loop while still equal */
    while (( wR == 0 ) && (wSt==ZS_SUCCESS) && (wIndexRank<= lastIdx())) {
      pIndexCollection.push(ZIndexResult(pIndexItem.ZMFAddress,pIndexItem.IndexAddress));
      wIndexRank++;
      wSt=_URFtestRank(pKeyToSearch,wIndexRank,pIndexItem,wR,pLock);
    }
    wSt=ZS_FOUND ;
    goto _URFsearch_Return;
  }

  if (wIndexRank > lastIdx()) {
    wSt=ZS_OUTBOUNDHIGH;
    goto _URFsearch_Return;
  }

  /* here wR is less than 0 */

  wSt=ZS_NOTFOUND;
//  goto _URFsearch_Return;


_URFsearch_Return:
  if (ZVerbose & ZVB_SearchEngine)
    _DBGPRINT("ZRawIndexFile::_URFsearchUnique  Index key <%s> status <%s> last index rank %ld last index address %ld last zmf address %ld \n",
        IndexName.toCChar(),
        decode_ZStatus(wSt),
        pIndexItem.IndexRank,pIndexItem.IndexAddress,pIndexItem.ZMFAddress)
  /*    if ((wSt==ZS_FOUND)&&(pLock != ZLock_Nolock ))
            {
            return  static_cast<ZMasterFile*>(pZIF.ZMFFather)->zlockByAddress(wIndexItem.ZMFaddress,pLock); // lock corresponding ZMasterFile address with given lock mask
            }*/
  return  (wSt) ;
}// _URFsearch

/**
 * @brief ZRawIndexFile::_Rawsearch Searches for a FIRST or UNIQUE value for a given index. This search may be EXACT or PARTIAL.
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
 * Returns a ZSIndexResult structure for the unique index reference if found : Index record rank ; ZMasterFile corresponding address
 * see @ref ZSIndexResult.
 *
 * @param[in] pKey key content to find. Key has to be in Key internal format to be compared : formatted using _formatKeyContent() routine
 * @param[in] pZIF ZIndexFile object to search on
 *
 * @param[out] pZIR a ZSIndexResult object with ZIndexFile relative key position (rank within index file) corresponding to key content if found
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
#ifdef __DEPRECATED__
ZStatus
ZRawIndexFile::_Rawsearch( const ZDataBuffer &pKey,
                      ZRawIndexFile &pZIF,
                      ZIndexResult &pZIR,
                      const zlockmask_type pLock)
{
ZStatus     wSt= ZS_NOTFOUND;

pZIR.IndexRank = 0;
ZDataBuffer wIndexRecord;
ZIndexItem wIndexItem ;

ssize_t wCompareSize=0;

ZIFCompare    wZIFCompare=ZKeyCompareBinary; /* use binary compare */

int wR;

long whigh = 0;
long wlow = 0;
long wpivot = 0;

//  ;

    if (pZIF.getSize()==0)
                    {
                    wSt= ZS_OUTBOUNDHIGH ;
                    goto _Rawsearch_Return;
                    }
// Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

     wCompareSize = pKey.Size;
    if (wCompareSize > pZIF.KeyUniversalSize)
                wCompareSize=pZIF.KeyUniversalSize;


// Choice of comparison routine : no choice - no dictionary
  /*
    if (pZIF.ZMFFather->MetaDictionary)
    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison

*/
        whigh = pZIF.lastIdx();  // last element : highest boundary
        wlow = 0;               // first element : lowest boundary

        wSt=ZS_NOTFOUND;

//        while (true) // WHILE TOBE SUPPRESSED------------------Main loop------------------------
//            {
        pZIR.IndexRank = wlow ;

        pZIF.ZPMSStats.Reads ++;
        if ((wSt=pZIF.zget(wIndexRecord,0L))!=ZS_SUCCESS)
                                                    goto _Rawsearch_Return;
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

        if (wR==0)
                    {
                    wSt=ZS_FOUND ;
                    pZIR.IndexRank = wlow ;
                    goto _Rawsearch_Return;
                    }
        if (wR<0)
                {return  ZS_OUTBOUNDLOW;}

         pZIR.IndexRank = whigh ;

         pZIF.ZPMSStats.Reads ++;
         if ((wSt=pZIF.zget(wIndexRecord,whigh))!=ZS_SUCCESS)
                                                     goto _Rawsearch_Return;
         wIndexItem.fromFileKey(wIndexRecord);
         wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

         if (wR==0)
                {
                wSt=ZS_FOUND ;
                pZIR.IndexRank = whigh ;
                goto _Rawsearch_Return;
                }
         if (wR>0)
                 {return  ZS_OUTBOUNDHIGH;}

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)// ---------------Main loop around pivot----------------------
            {
            pZIR.IndexRank = wpivot ;

            pZIF.ZPMSStats.Reads ++;
            if ((wSt=pZIF.zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
                                                        goto _Rawsearch_Return;
            wIndexItem.fromFileKey(wIndexRecord);
            wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

            if (wR==0)
                    {
                    wSt=ZS_FOUND ;
                    goto _Rawsearch_Return;
                    }

            if (wR>0)
                wlow = wpivot ;
                else
                whigh = wpivot ;

            if ((whigh-wlow)==1)
                    {
                    pZIR.IndexRank = wpivot ;
                    return  ZS_NOTFOUND;
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
                                                { return  wSt;}
    wIndexItem.fromFileKey(wIndexRecord);
    wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

//        while ((wpivot<whigh)&&(_Compare::_compare(&pKey,&pZTab[wpivot]._Key) > 0))
    while ((wpivot<whigh)&& (wR > 0))
            {
            wpivot ++;
            pZIR.IndexRank=wpivot;

            pZIF.ZPMSStats.Reads ++;
            if ((wSt=pZIF.zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
                                                       { return  wSt;}
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
        return  ZS_NOTFOUND ;
        }

_Rawsearch_Return:
    pZIR.ZMFAddress = wIndexItem.ZMFaddress ;
/*    if ((wSt=ZS_FOUND)&&(pLock != ZLock_Nolock ))
            {
            return  static_cast<ZMasterFile*>(pZIF.ZMFFather)->zlockByAddress(wIndexItem.ZMFaddress,pLock); // lock corresponding ZMasterFile address with given lock mask
            }*/
    return  (wSt) ;
}// _Rawsearch
#endif // #ifdef __DEPRECATED__
/**
 * @brief ZRawIndexFile::_RawsearchAll  Search for ALL index ranks matching EXACTLY a certain key value. Key value cannot be partial.
 * The size of Index key value defines the length to compare with given key value pKey.
 *
 * @par return s
 * - a ZSIndexCollection gathering a collection of ZSIndexResult_struct i. e. :
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
 *  - In case of error (may be already locked), already locked collection items are unlocked before return ing errored ZStatus.
 *
 * @param[in] pKey key content to find. Key has to be formatted using _formatKeyContent() routine
 * @param[in] pZIF ZIndexFile object to search on
 * @param[out] pCollection  contains the resulting collection of ZSIndexResult objects {Index rank ; ZMF blockaddress} for the matched elements
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
ZRawIndexFile::_URFsearchAll( const ZDataBuffer        &pKey,     // key content to find out in index
                               ZIndexCollection &pCollection,
                               const ZMatchSize_type    pZMS)
{
ZStatus     wSt=ZS_NOTFOUND;
//long        wIndexRank;
ZIndexResult wZIR;
ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//ZIFCompare wZIFCompare=ZKeyCompareBinary;

//ssize_t wCompareSize= 0;

int wR;

zrank_type whigh ;
zrank_type wlow ;
zrank_type wpivot;
zrank_type wIndexFound=0;
//


    pCollection.ZMS = pZMS;
/*    if (pZMS==ZMS_MatchIndexSize)
            {
            if (pKey.Size!=pZIF.KeyUniversalSize)
                        {return  ZS_INVSIZE ;}//
            }
*/
//-----------Comparison routine selection  : Always ZKeyCompareBinary for Raw Index search------------------------------------

/*    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison
*/
//-----------Comparison size definition------------------------------
// Size of comparison is given by pKey.Size : if partial key Size, given size will be shorter than Index key size.
/*
     wCompareSize = pKey.Size;
    if (wCompareSize > pZIF.KeyUniversalSize)
                wCompareSize=pZIF.KeyUniversalSize;
*/
    pCollection.clear();            // Always clearing the collection when zsearch
    pCollection.setStatus(ZS_NOTFOUND) ;
    wSt= ZS_NOTFOUND;

    if (getSize()==0) {
      pCollection.setStatus(ZS_OUTBOUNDHIGH) ;
      goto _URFsearchAll_Return;
    }


    whigh = lastIdx();  // last element : highest boundary
    wlow = 0;               // first element : lowest boundary

    wpivot = ((whigh-wlow)/2) +wlow ;

// get lowest rank (0L)

        wZIR.IndexAddress= wlow ;
        ZPMSStats.Reads ++;
        if (pCollection.setStatus(zget(wIndexRecord,ZLock_Nolock))!=ZS_SUCCESS)
                                                goto _URFsearchAllError;
        wIndexItem.fromFileKey(wIndexRecord);
//        wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);
        wR = URFComparePtr(pKey.Data,pKey.Size,wIndexItem.Data,wIndexItem.Size);
        if (wR==0)
            {
            pCollection.setStatus(ZS_FOUND) ;
            goto _URFsearchAllBackProcess ;  // capture all collection around current wZIR.IndexRank
            }// if wR==0

        if (wR<0)
            {
            pCollection.setStatus(ZS_OUTBOUNDLOW) ;
            return  ZS_OUTBOUNDLOW ;
            }// if wR<0

// get highest rank (lastIdx())
     wZIR.IndexAddress = whigh ;
     ZPMSStats.Reads ++;
     if (pCollection.setStatus(zget(wIndexRecord,wZIR.IndexAddress))!=ZS_SUCCESS)
                                             goto _URFsearchAllError;
     wIndexItem.fromFileKey(wIndexRecord);
//     wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);
     wR = URFComparePtr(pKey.Data,pKey.Size,wIndexItem.Data,wIndexItem.Size);
     if (wR==0)
         {
         pCollection.setStatus(ZS_FOUND) ;
         goto _URFsearchAllBackProcess ;  // capture all collection around current wZIR.IndexRank
         }// if wR==0

     if (wR>0)
         {
         pCollection.setStatus(ZS_OUTBOUNDHIGH) ;
         return  ZS_OUTBOUNDHIGH ;
         }// if wR>0

     while ((whigh-wlow)>2) //---------------------Main loop around wpivot-----------
        {
        wZIR.IndexAddress = wpivot ;
        ZPMSStats.Reads ++;
        if (pCollection.setStatus(zget(wIndexRecord,wZIR.IndexAddress))!=ZS_SUCCESS)
                                                goto _URFsearchAllError;
        wIndexItem.fromFileKey(wIndexRecord);
//        wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);
        wR = URFComparePtr(pKey.Data,pKey.Size,wIndexItem.Data,wIndexItem.Size);
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
                return  ZS_NOTFOUND ;
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        wZIR.IndexAddress = wpivot ;
        ZPMSStats.Iterations++;
        } // while (whigh-wlow)>2 ---------------------Main loop around wpivot-----------

//--------------------------Collection Post processing--------------------
    if (pCollection.getStatus()!=ZS_FOUND)
        {
        return  pCollection.getStatus();
        }
_URFsearchAllBackProcess:
    /*
     *
     *  Process the remaining collection
     */
    wIndexFound = wZIR.IndexAddress; // search for matches before and after wIndexFound
    pCollection.setStatus(ZS_FOUND) ;

    wZIR.ZMFAddress =wIndexItem.ZMFAddress;

    pCollection.push(wZIR);

    // need to capture anything equal before wIndexFound: push_front - then anything AFTER wIndexFound : push
    // search for lower range

    // Anything equal before wIndexFound : push_front
    ZPMSStats.Reads ++;
    wSt=zgetPrevious(wIndexRecord,ZLock_Nolock);
    wIndexItem.fromFileKey(wIndexRecord);
//    while ((wSt==ZS_SUCCESS)&&(wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize)==0))
    while ((wSt==ZS_SUCCESS)&& (URFComparePtr(pKey.Data,pKey.Size,wIndexItem.Data,wIndexItem.Size)==0)) {
        wZIR.IndexAddress = getCurrentRank();
        wZIR.ZMFAddress =wIndexItem.ZMFAddress;

        pCollection.push_front(wZIR); // next push : push_front to reorganize in the correct order
        ZPMSStats.Reads ++;
        wSt=zgetPrevious(wIndexRecord,ZLock_Nolock);
        if (wSt==ZS_SUCCESS)
            wIndexItem.fromFileKey(wIndexRecord);
        }// while
    if (wSt!=ZS_OUTBOUNDLOW)
            {
            pCollection.setStatus(wSt);
            goto _URFsearchAllError;
            }
// up to here we have the first found index record in key order at pCollection[0]

    // Anything equal after wIndexFound : push
    ZPMSStats.Reads ++;
    wIndexFound++;
    wZIR.IndexAddress = wIndexFound;
    wSt=zget(wIndexRecord,wIndexFound);
    if (wSt==ZS_SUCCESS) {
        wIndexItem.fromFileKey(wIndexRecord);
        while ((wSt==ZS_SUCCESS)&& (URFComparePtr(pKey.Data,pKey.Size,wIndexItem.Data,wIndexItem.Size)==0)) {
//        while ((wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize)==0)&&(wSt==ZS_SUCCESS))
//            {
            wZIR.IndexAddress = getCurrentRank();
            wZIR.ZMFAddress =wIndexItem.ZMFAddress;

            pCollection.push(wZIR); // after Found index : push
            ZPMSStats.Reads ++;
            wSt=zgetNext(wIndexRecord);
            if (wSt==ZS_SUCCESS)
                wIndexItem.fromFileKey(wIndexRecord);
            if (wSt!=ZS_EOF)
                    {
                    pCollection.setStatus(wSt);
                    goto _URFsearchAllError;
                    }
            }
        }// if

_URFsearchAll_Return:
//    if (pLock != ZLock_Nolock )
//            pCollection.zlockAll(pLock);    // lock corresponding Collection with given lock mask if necessary
    return  (pCollection.getStatus()) ;

_URFsearchAllError:
//    if (pLock != ZLock_Nolock )
//            pCollection.zunlockAll();    // lock corresponding Collection with given lock mask if necessary

    return  (pCollection.getStatus());
}// URFearchAll using ZIndexFile

#ifdef __DEPRECATED__

ZStatus
ZRawIndexFile::_RawsearchAll(const ZDataBuffer        &pKey,     // key content to find out in index
    ZRawIndexFile               &pZIF,     // pointer to ZIndexControlBlock containing index description
    ZIndexCollection &pCollection,
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

  zrank_type whigh ;
  zrank_type wlow ;
  zrank_type wpivot;
  zrank_type wIndexFound=0;
  //


  pCollection.ZMS = pZMS;
  if (pZMS==ZMS_MatchIndexSize)
  {
    if (pKey.Size!=pZIF.KeyUniversalSize)
    {return  ZS_INVSIZE ;}//
  }

  //-----------Comparison routine selection  : Always ZKeyCompareBinary for Raw Index search------------------------------------

  /*    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison
*/
  //-----------Comparison size definition------------------------------
  // Size of comparison is given by pKey.Size : if partial key Size, given size will be shorter than Index key size.

  wCompareSize = pKey.Size;
  if (wCompareSize> pZIF.KeyUniversalSize)
    wCompareSize=pZIF.KeyUniversalSize;

  pCollection.clear();            // Always clearing the collection when zsearch
  pCollection.setStatus(ZS_NOTFOUND) ;
  wSt= ZS_NOTFOUND;

  if (pZIF.getSize()==0)
  {
    pCollection.setStatus(ZS_OUTBOUNDHIGH) ;
    goto _RawsearchAll_Return;
  }


  whigh = pZIF.lastIdx();  // last element : highest boundary
  wlow = 0;               // first element : lowest boundary

  wpivot = ((whigh-wlow)/2) +wlow ;

  // get lowest rank (0L)

  wZIR.IndexRank= wlow ;
  pZIF.ZPMSStats.Reads ++;
  if (pCollection.setStatus(pZIF.zget(wIndexRecord,ZLock_Nolock))!=ZS_SUCCESS)
    goto _RawsearchAllError;
  wIndexItem.fromFileKey(wIndexRecord);
  wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);

  if (wR==0)
  {
    pCollection.setStatus(ZS_FOUND) ;
    goto _RawsearchAllBackProcess ;  // capture all collection around current wZIR.IndexRank
  }// if wR==0

  if (wR<0)
  {
    pCollection.setStatus(ZS_OUTBOUNDLOW) ;
    return  ZS_OUTBOUNDLOW ;
  }// if wR<0

  // get highest rank (lastIdx())
  wZIR.IndexRank = whigh ;
  pZIF.ZPMSStats.Reads ++;
  if (pCollection.setStatus(pZIF.zget(wIndexRecord,wZIR.IndexRank))!=ZS_SUCCESS)
    goto _RawsearchAllError;
  wIndexItem.fromFileKey(wIndexRecord);
  wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);
  //     wR= _Compare::_compare(&pKey,&pZTab[whigh]._Key) ;
  if (wR==0)
  {
    pCollection.setStatus(ZS_FOUND) ;
    goto _RawsearchAllBackProcess ;  // capture all collection around current wZIR.IndexRank
  }// if wR==0

  if (wR>0)
  {
    pCollection.setStatus(ZS_OUTBOUNDHIGH) ;
    return  ZS_OUTBOUNDHIGH ;
  }// if wR>0

  while ((whigh-wlow)>2) //---------------------Main loop around wpivot-----------
  {
    wZIR.IndexRank = wpivot ;
    pZIF.ZPMSStats.Reads ++;
    if (pCollection.setStatus(pZIF.zget(wIndexRecord,wZIR.IndexRank))!=ZS_SUCCESS)
      goto _RawsearchAllError;
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
      return  ZS_NOTFOUND ;
    }

    wpivot = ((whigh-wlow)/2) + wlow ;
    wZIR.IndexRank = wpivot ;
    pZIF.ZPMSStats.Iterations++;
  } // while (whigh-wlow)>2 ---------------------Main loop around wpivot-----------

  //--------------------------Collection Post processing--------------------
  if (pCollection.getStatus()!=ZS_FOUND)
  {
    return  pCollection.getStatus();
  }
_RawsearchAllBackProcess:
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
    wZIR.IndexRank = pZIF.getCurrentRank();
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
    goto _RawsearchAllError;
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
      wZIR.IndexRank = pZIF.getCurrentRank();
      wZIR.ZMFAddress =wIndexItem.ZMFaddress;

      pCollection.push(wZIR); // after Found index : push
      pZIF.ZPMSStats.Reads ++;
      wSt=pZIF.zgetNext(wIndexRecord);
      if (wSt==ZS_SUCCESS)
        wIndexItem.fromFileKey(wIndexRecord);
      if (wSt!=ZS_EOF)
      {
        pCollection.setStatus(wSt);
        goto _RawsearchAllError;
      }
    }
  }// if

_RawsearchAll_Return:
  //    if (pLock != ZLock_Nolock )
  //            pCollection.zlockAll(pLock);    // lock corresponding Collection with given lock mask if necessary
  return  (pCollection.getStatus()) ;

_RawsearchAllError:
  //    if (pLock != ZLock_Nolock )
  //            pCollection.zunlockAll();    // lock corresponding Collection with given lock mask if necessary

  return  (pCollection.getStatus());
}// _RawsearchAll using ZIndexFile

#endif // __DEPRECATED__

/**
 * @brief ZRawIndexFile::_RawsearchFirst search ZIndexFile pZIF for a first match of pKey (first in key order) and return s a ZSIndexResult
 * - ZIndexFile rank : index file relative position of key found
 * - ZMasterFile corresponding record (block) address
 *
 * @par Accessing collection of selected records
 *
 *  ZIndexFile::_RawsearchFirst() and ZIndexFile::_RawsearchNext() works using a search context ( ZIndexSearchContext )that maintains a collection of found records (ZSIndexCollection)
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
 * @param[out] pCollection A pointer to the contextual meta-data from the search created by _RawsearchFirst() routine.
 *
 * It contains
 * - a ZArray of ZSIndexResult objects, enriched with successive _RawsearchNext() calls
 * - Contextual meta-data for the search (an ZSIndexCollectionContext embeded object holds this data)
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on
 *
 * @param[out] pZIR result of the _RawsearchNext operation, if any, as a ZSIndexResult with appropriate Index references (Index file rank ; ZMF record address)
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
ZRawIndexFile::_URFsearchFirst( const ZDataBuffer        &pKeyContent,     // key content to find out in index
                                 ZIndexCollection       *pCollection,
                                 ZIndexResult             &pZIR,
                                 const ZMatchSize_type    pZMS) {

ZStatus     wSt=ZS_NOTFOUND;
//long        wIndexRank;
ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//ZIFCompare wZIFCompare=ZKeyCompareBinary; // comparison routine is deduced from Dictionary Key field type

//ssize_t wCompareSize= 0;

int wR;

zrank_type whigh ;
zrank_type wlow ;
zrank_type wpivot;
//

//-----------Initialization Section---------------------------------

    if (pCollection==nullptr)
    {
//-----------Comparison routine selection : Always ZKeyCompareBinary for Raw Index search------------------------------------
 /*   if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison
*/
//-----------Comparison size definition------------------------------

// Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.
/*
     wCompareSize = pKeyContent.Size;
    if (wCompareSize> pZIF.KeyUniversalSize)
                wCompareSize=pZIF.KeyUniversalSize;
*/


    pCollection=new ZIndexCollection (this); // initialize all and create ZSIndexCollection instance
    pCollection->Context.setup (pKeyContent,nullptr);
    pCollection->Context.Op = ZCOP_GetFirst ;
    pCollection->setStatus(ZS_NOTFOUND) ;
    wSt= ZS_NOTFOUND;
    } // pCollection == nullptr

    pCollection->ZMS = pZMS;
    if (pZMS==ZMS_MatchIndexSize)
            {
            if (pKeyContent.Size!=KeyUniversalSize)
                                        {return  ZS_INVSIZE ;}//
            }
//-----------End Initialization Section---------------------------------

    if (getSize()==0)
                    {
                    pCollection->setStatus(ZS_OUTBOUNDHIGH );
                    return  ZS_OUTBOUNDHIGH;
                    }


    whigh = lastIdx();  // last element : highest boundary
    wlow = 0;               // first element : lowest boundary

    pZIR.IndexAddress= wlow ;
    ZPMSStats.Reads ++;
    if (pCollection->setStatus(zget(wIndexRecord,pZIR.IndexAddress))!=ZS_SUCCESS)
                                                            {return  pCollection->getStatus();}//
    wIndexItem.fromFileKey(wIndexRecord);
//    wR= wZIFCompare(pKeyContent,wIndexItem.KeyContent,wCompareSize);

    wR=URFComparePtr(pKeyContent.Data,pKeyContent.Size,wIndexItem.Data,wIndexItem.Size);

    if (wR==0)
        {
        wSt= ZS_FOUND;
        pCollection->setStatus(ZS_FOUND);
        goto _RawsearchFirstBackProcess ;  // capture all collection before current wZIR.IndexRank if any
        }// if wR==0
    if (wR<0)
        {
        pCollection->setStatus( ZS_OUTBOUNDLOW);
       return  ZS_OUTBOUNDLOW;
        }// if wR<0

     pZIR.IndexAddress = whigh ;
     ZPMSStats.Reads ++;
     if (pCollection->setStatus(zget(wIndexRecord,pZIR.IndexAddress))!=ZS_SUCCESS)
                                                         {return  pCollection->getStatus();}
     wIndexItem.fromFileKey(wIndexRecord);
//     wR= wZIFCompare((ZDataBuffer&)pKeyContent,wIndexItem.KeyContent,wCompareSize);
    wR=URFComparePtr(pKeyContent.Data,pKeyContent.Size,wIndexItem.Data,wIndexItem.Size);
     if (wR==0)
         {
         pCollection->setStatus( ZS_FOUND);
         goto _RawsearchFirstBackProcess ;  // capture all collection before current wZIR.IndexRank
         }// if wR==0
     if (wR>0)
         {
         pCollection->setStatus( ZS_OUTBOUNDHIGH);
        return  ZS_OUTBOUNDHIGH;
         }// if wR>0

    wpivot = ((whigh-wlow)/2) +wlow ;

    while ((whigh-wlow)>2) // --------Main loop around pivot------------------
         {
        pZIR.IndexAddress = wpivot ;
        ZPMSStats.Reads ++;
        if (pCollection->setStatus(zget(wIndexRecord,pZIR.IndexAddress))!=ZS_SUCCESS)
                                                            {return  pCollection->getStatus();}
        wIndexItem.fromFileKey(wIndexRecord);
        wR=URFComparePtr(pKeyContent.Data,pKeyContent.Size,wIndexItem.Data,wIndexItem.Size);
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
                return  ZS_NOTFOUND;
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        pZIR.IndexAddress = wpivot ;
        pCollection->Context.CurrentZIFrank = wpivot;
        ZPMSStats.Iterations++;
        } // while (whigh-wlow)>2--------Main loop around pivot------------------

    if (pCollection->getStatus()!=ZS_FOUND)
                        { return  pCollection->getStatus();}
_RawsearchFirstBackProcess:
/*
 *
 *  Search for the first key value in Index order
 */
    wIndexItem.fromFileKey(wIndexRecord);
    pZIR.IndexAddress = getCurrentRank();
    pZIR.ZMFAddress = wIndexItem.ZMFAddress;

    pCollection->Context.CurrentZIFrank = pZIR.IndexAddress; // search for matches before current indexrank to find the first key value in index order

// need to set the first record to First index rank for key
// search for lower range

    ZPMSStats.Reads ++;
    pCollection->setStatus(zgetPrevious(wIndexRecord,ZLock_Nolock));

    while ((pCollection->getStatus()==ZS_SUCCESS)&&(URFComparePtr(pKeyContent.Data,pKeyContent.Size,wIndexItem.Data,wIndexItem.Size)==0))
        {
        wIndexItem.fromFileKey(wIndexRecord);
        pZIR.IndexAddress = getCurrentRank();
        pZIR.ZMFAddress = wIndexItem.ZMFAddress;
        pCollection->Context.CurrentZIFrank= pZIR.IndexAddress;

//       pCollection.push_front(wZIR); // next push : push_front to reorganize in the correct order
        ZPMSStats.Reads ++;
        pCollection->Context.CurrentZIFrank--;
        if (pCollection->Context.CurrentZIFrank<0)
                                        break;
        pCollection->setStatus(zget(wIndexRecord,pCollection->Context.CurrentZIFrank));
        }// while


/*    if (pLock!=ZLock_Nolock) // lock record if requested
        {
        pCollection->setStatus(static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress,pLock));
        if (pCollection->getStatus()!=ZS_SUCCESS)
            {
            return  pCollection->getStatus();
            }
        }// !=ZLock_Nolock
*/
    pCollection->push(pZIR); // enrich Collection with found record.

//    ZException.clearStack();

    return  pCollection->setStatus(ZS_FOUND);

}// _URFsearchFirst

#ifdef _DEPRECATED__
ZStatus
ZRawIndexFile::_RawsearchFirst( const ZDataBuffer        &pKeyContent,     // key content to find out in index
    ZRawIndexFile           &pZIF,     // pointer to ZIndexControlBlock containing index description
    ZIndexCollection       *pCollection,
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

  zrank_type whigh ;
  zrank_type wlow ;
  zrank_type wpivot;
  //

  //-----------Initialization Section---------------------------------

  if (pCollection==nullptr)
  {
    //-----------Comparison routine selection : Always ZKeyCompareBinary for Raw Index search------------------------------------
    /*   if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison
*/
    //-----------Comparison size definition------------------------------

    // Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

    wCompareSize = pKeyContent.Size;
    if (wCompareSize> pZIF.KeyUniversalSize)
      wCompareSize=pZIF.KeyUniversalSize;



    pCollection=new ZIndexCollection (&pZIF); // initialize all and create ZSIndexCollection instance
    pCollection->Context.setup (pKeyContent,
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
    if (pKeyContent.Size!=pZIF.KeyUniversalSize)
    {return  ZS_INVSIZE ;}//
  }
  //-----------End Initialization Section---------------------------------

  if (pZIF.getSize()==0)
  {
    pCollection->setStatus(ZS_OUTBOUNDHIGH );
    return  ZS_OUTBOUNDHIGH;
  }


  whigh = pZIF.lastIdx();  // last element : highest boundary
  wlow = 0;               // first element : lowest boundary

  pZIR.IndexRank= wlow ;
  pZIF.ZPMSStats.Reads ++;
  if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
  {return  pCollection->getStatus();}//
  wIndexItem.fromFileKey(wIndexRecord);
  wR= wZIFCompare((ZDataBuffer&)pKeyContent,wIndexItem.KeyContent,wCompareSize);

  if (wR==0)
  {
    wSt= ZS_FOUND;
    pCollection->setStatus(ZS_FOUND);
    goto _RawsearchFirstBackProcess ;  // capture all collection before current wZIR.IndexRank if any
  }// if wR==0
  if (wR<0)
  {
    pCollection->setStatus( ZS_OUTBOUNDLOW);
    return  ZS_OUTBOUNDLOW;
  }// if wR<0

  pZIR.IndexRank = whigh ;
  pZIF.ZPMSStats.Reads ++;
  if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
  {return  pCollection->getStatus();}
  wIndexItem.fromFileKey(wIndexRecord);
  wR= wZIFCompare((ZDataBuffer&)pKeyContent,wIndexItem.KeyContent,wCompareSize);

  if (wR==0)
  {
    pCollection->setStatus( ZS_FOUND);
    goto _RawsearchFirstBackProcess ;  // capture all collection before current wZIR.IndexRank
  }// if wR==0
  if (wR>0)
  {
    pCollection->setStatus( ZS_OUTBOUNDHIGH);
    return  ZS_OUTBOUNDHIGH;
  }// if wR>0

  wpivot = ((whigh-wlow)/2) +wlow ;

  while ((whigh-wlow)>2) // --------Main loop around pivot------------------
  {
    pZIR.IndexRank = wpivot ;
    pZIF.ZPMSStats.Reads ++;
    if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
    {return  pCollection->getStatus();}
    wIndexItem.fromFileKey(wIndexRecord);
    wR= wZIFCompare((ZDataBuffer&)pKeyContent,wIndexItem.KeyContent,wCompareSize);
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
      return  ZS_NOTFOUND;
    }

    wpivot = ((whigh-wlow)/2) + wlow ;
    pZIR.IndexRank = wpivot ;
    pCollection->Context.CurrentZIFrank = wpivot;
    pZIF.ZPMSStats.Iterations++;
  } // while (whigh-wlow)>2--------Main loop around pivot------------------

  if (pCollection->getStatus()!=ZS_FOUND)
  { return  pCollection->getStatus();}
_RawsearchFirstBackProcess:
  /*
 *
 *  Search for the first key value in Index order
 */
  wIndexItem.fromFileKey(wIndexRecord);
  pZIR.IndexRank = pZIF.getCurrentRank();
  pZIR.ZMFAddress = wIndexItem.ZMFaddress;

  pCollection->Context.CurrentZIFrank = pZIR.IndexRank; // search for matches before current indexrank to find the first key value in index order

  // need to set the first record to First index rank for key
  // search for lower range

  pZIF.ZPMSStats.Reads ++;
  pCollection->setStatus(pZIF.zgetPrevious(wIndexRecord,ZLock_Nolock));

  while ((pCollection->getStatus()==ZS_SUCCESS)&&(wZIFCompare((ZDataBuffer&)pKeyContent,wIndexItem.KeyContent,wCompareSize)==0))
  {
    wIndexItem.fromFileKey(wIndexRecord);
    pZIR.IndexRank = pZIF.getCurrentRank();
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
            return  pCollection->getStatus();
            }
        }// !=ZLock_Nolock
*/
  pCollection->push(pZIR); // enrich Collection with found record.

  //    ZException.clearStack();

  return  pCollection->setStatus(ZS_FOUND);

}// _RawsearchFirst
#endif // _DEPRECATED__
/**
 * @brief ZIndexFile::_RawsearchNext
 * @param[in-out] pCollection A pointer to the contextual meta-data from the search created by _RawsearchFirst() routine.
 *
 * It contains
 * - a ZArray of ZSIndexResult objects, enriched with successive _RawsearchNext calls
 * - Contextual meta-data for the search.
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on.
 * @param[out] pZIR result of the _RawsearchNext operation, if any, as a ZSIndexResult with appropriate Index references (Index file rank ; ZMF record address)
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 */
ZStatus
ZRawIndexFile::_URFsearchNext ( ZIndexResult       &pZIR,
                                ZIndexCollection*  pCollection)
{

ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//

//-----------Initialization Section---------------------------------

    if (pCollection==nullptr)  // if no Context then errored
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Cannot invoke _RawsearchNext without having Collection / Context : invoke first _RawsearchFirst ");
        return  ZS_INVOP;
        } // pCollection == nullptr

//-----------End Initialization Section---------------------------------

    pCollection->Context.Op = ZCOP_GetNext ;
    if (pCollection->getStatus()!=ZS_FOUND)  // if nothing has been found or error : return  not found
                    {return  pCollection->getStatus();}



// up to here we have the first found index record in key order at pCollection[0]

    // Anything equal after wIndexFound : push

    pCollection->Context.CurrentZIFrank ++;
    if (pCollection->Context.CurrentZIFrank > pCollection->ZIFFile->lastIdx())
        {
        pCollection->Context.CurrentZIFrank=pCollection->ZIFFile->lastIdx();
        return  pCollection->setStatus(ZS_EOF);
        }
    pCollection->ZIFFile->ZPMSStats.Reads ++;
    pCollection->setStatus(pCollection->ZIFFile->zget(wIndexRecord,pCollection->Context.CurrentZIFrank));
    if (pCollection->getStatus()==ZS_SUCCESS)
        {
        wIndexItem.fromFileKey(wIndexRecord);
//        if ((pCollection->Context.Compare(pCollection->Context.KeyContent,wIndexItem,pCollection->Context.CompareSize)==0)&&(pCollection->getStatus()==ZS_SUCCESS))
        if ((URFComparePtr(pCollection->Context.KeyContent.Data,pCollection->Context.KeyContent.Size,
                            wIndexItem.Data, wIndexItem.Size)==0)&&(pCollection->getStatus()==ZS_SUCCESS))
            {
            pZIR.IndexAddress = pCollection->ZIFFile->getCurrentRank();
            pZIR.ZMFAddress =wIndexItem.ZMFAddress;
            pCollection->setStatus(ZS_FOUND);

            pCollection->push(pZIR); // after Found index : push
/*
            if (pCollection->getLock()!=ZLock_Nolock) // lock record if requested
                {
                pCollection->setStatus (static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress));
                if (pCollection->getStatus()!=ZS_SUCCESS)
                    {
                    return  pCollection->getStatus ();
                    }
                }// !=ZLock_Nolock
*/


            }// if compare
        }// if
        else
        pCollection->setStatus(ZS_OUTBOUNDHIGH);

    return  pCollection->getStatus();

}// _URFsearchNext

#ifdef __DEPRECATED__
ZStatus
ZRawIndexFile::_RawsearchNext (ZIndexResult       &pZIR,
    ZIndexCollection*  pCollection)
{

  ZIndexItem wIndexItem ;
  ZDataBuffer wIndexRecord;

  //

  //-----------Initialization Section---------------------------------

  if (pCollection==nullptr)  // if no Context then errored
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVOP,
        Severity_Severe,
        " Cannot invoke _RawsearchNext without having Collection / Context : invoke first _RawsearchFirst ");
    return  ZS_INVOP;
  } // pCollection == nullptr

  //-----------End Initialization Section---------------------------------

  pCollection->Context.Op = ZCOP_GetNext ;
  if (pCollection->getStatus()!=ZS_FOUND)  // if nothing has been found or error : return  not found
  {return  pCollection->getStatus();}



  // up to here we have the first found index record in key order at pCollection[0]

  // Anything equal after wIndexFound : push

  pCollection->Context.CurrentZIFrank ++;
  if (pCollection->Context.CurrentZIFrank > pCollection->ZIFFile->lastIdx())
  {
    pCollection->Context.CurrentZIFrank=pCollection->ZIFFile->lastIdx();
    return  pCollection->setStatus(ZS_EOF);
  }
  pCollection->ZIFFile->ZPMSStats.Reads ++;
  pCollection->setStatus(pCollection->ZIFFile->zget(wIndexRecord,pCollection->Context.CurrentZIFrank));
  if (pCollection->getStatus()==ZS_SUCCESS)
  {
    wIndexItem.fromFileKey(wIndexRecord);
    if ((pCollection->Context.Compare(pCollection->Context.KeyContent,wIndexItem.KeyContent,pCollection->Context.CompareSize)==0)&&(pCollection->getStatus()==ZS_SUCCESS))
    {
      pZIR.IndexRank = pCollection->ZIFFile->getCurrentRank();
      pZIR.ZMFAddress =wIndexItem.ZMFaddress;
      pCollection->setStatus(ZS_FOUND);

      pCollection->push(pZIR); // after Found index : push
      /*
            if (pCollection->getLock()!=ZLock_Nolock) // lock record if requested
                {
                pCollection->setStatus (static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress));
                if (pCollection->getStatus()!=ZS_SUCCESS)
                    {
                    return  pCollection->getStatus ();
                    }
                }// !=ZLock_Nolock
*/


    }// if compare
  }// if
  else
    pCollection->setStatus(ZS_OUTBOUNDHIGH);

  return  pCollection->getStatus();

}// _RawsearchNext
#endif // __DEPRECATED__


/**
 * @brief ZRawIndexFile::_RawsearchIntervalAll searches all index key value from ZIndexFile pZIF corresponding to Interval given by pKeyLow as lowest value for range and pKeyHigh as highest value for range.
 *      pKeyLow and pKeyHigh may be included in range if pExclude is false, or exclude from range if pExclude is true.
 * @param[in] pKeyLow   Lowest key content value to find out in index           [stored in collection's context]
 * @param[in] pKeyHigh  Highest key content value to find out in index          [stored in collection's context]
 * @param[in] pZIF      ZIndexFile object                                       [stored in collection]
 * @param[out] pCollection ZSIndexCollection object created by the routine. It stores the resulting set of reference to found ZIndexFile-ZMasterFile records.
 * @param[out] pZIR ZSIndexResult giving the result of the first search if any
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
ZRawIndexFile::_RawsearchIntervalAll  (const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                                 const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                                 ZRawIndexFile &pZIF,     // pointer to ZIndexControlBlock containing index description
                                 ZIndexCollection       *pCollection,   // enriched collection of reference (ZIndexFile rank, ZMasterFile record address)
                                 const bool             pExclude) // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)
{


ZStatus      wSt;
ZIndexResult wZIR;

    wSt=_RawsearchIntervalFirst(pKeyLow,
                             pKeyHigh,
                             pZIF,
                             pCollection,
                             wZIR,
                             pExclude);
    if (wSt!=ZS_FOUND)
                { return  wSt;}
    while (wSt==ZS_FOUND)
        {
        wSt=_RawsearchIntervalNext(wZIR,pCollection);
        }
    return  (wSt==ZS_EOF)?ZS_FOUND:wSt;

}// _RawsearchIntervalAll

/**
 * @brief ZIndexFile::_RawsearchIntervalFirst
 * @param[in] pKeyLow   Lowest key content value to find out in index           [stored in collection's context]
 * @param[in] pKeyHigh  Highest key content value to find out in index          [stored in collection's context]
 * @param[in] pZIF      ZIndexFile object                                       [stored in collection]
 * @param[out] pCollection ZSIndexCollection object created by the routine.
 * @param[out] pZIR ZSIndexResult giving the result of the first search if any
 * @param[in] pExclude Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)
 *          this option is stored within Collection context using operation code
 * @param[in] pLock Lock mask to apply to found records                         [stored in collection's context]
 * @return
 */
ZStatus
ZRawIndexFile::_RawsearchIntervalFirst(const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                                 const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                                 ZRawIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                                 ZIndexCollection       *pCollection,   // enriched collection of reference (ZIndexFile rank, ZMasterFile record address)
                                 ZIndexResult           &pZIR,
                                 const bool             pExclude)// Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)

{


ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//ZIFCompare wZIFCompare=ZKeyCompareBinary; // comparison routine is deduced from Dictionary Key field type

size_t wCompareSize= 0;

ZStatus wSt;

int wR;

zrank_type whigh ;
zrank_type wlow ;
zrank_type wpivot;
//

//-----------Initialization Section---------------------------------
    if (pCollection!=nullptr)
                    pCollection->clear();
    if (!pCollection->Context.FInitSearch)
    {
//-----------Comparison routine selection : Always ZKeyCompareBinary for Raw Index search------------------------------------
 /*   if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison
*/
//-----------Comparison size definition------------------------------
// Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

     wCompareSize = pKeyLow.Size;
    if (wCompareSize> pZIF.KeyUniversalSize)
                wCompareSize=pZIF.KeyUniversalSize;

    pCollection=new ZIndexCollection (&pZIF);

    pCollection->Context.setup (pKeyLow,&pKeyHigh); // initialize all and create ZSIndexCollection instance
    pCollection->setStatus(ZS_NOTFOUND) ;
    wSt= ZS_NOTFOUND;
    pCollection->Context.Op = ZCOP_Interval | ZCOP_GetFirst ;
    if (pExclude)
            pCollection->Context.Op |= ZCOP_Exclude ;
    } // pSearchContext == nullptr

//-----------End Initialization Section---------------------------------

    if (pZIF.getSize()==0)
                    {
                    pCollection->setStatus(ZS_OUTBOUNDHIGH );
                    return  ZS_OUTBOUNDHIGH;
                    }


//------------First half search---------------------------------

    whigh = pZIF.lastIdx();  // last element : highest boundary
    wlow = 0;               // first element : lowest boundary

// test low
        pZIR.IndexAddress= wlow ;
        pCollection->Context.CurrentZIFrank = wlow;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexAddress))!=ZS_SUCCESS)
                                                            { return  pCollection->getStatus();}
        wIndexItem.fromFileKey(wIndexRecord);
//        wR= wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize);
        wR=URFComparePtr(pKeyLow.Data,pKeyLow.Size,wIndexItem.Data,wIndexItem.Size);

        if (wR==0)
            {
            wSt= ZS_FOUND;
            pCollection->setStatus(ZS_FOUND);
            goto _RawsearchIntervalFirstBackProcess;  // capture all collection before current wZIR.IndexRank if any
            }// if wR==0
        if (wR<0)
            {
            pCollection->setStatus(ZS_OUTBOUNDLOW);
                                {return  ZS_OUTBOUNDLOW;}
            }// if wR==0
// test high
     pZIR.IndexAddress = whigh ;
     pCollection->Context.CurrentZIFrank = whigh;
     pZIF.ZPMSStats.Reads ++;
     if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexAddress))!=ZS_SUCCESS)
                                                         {  return  pCollection->getStatus();}
     wIndexItem.fromFileKey(wIndexRecord);
//     wR= wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize);
     wR=URFComparePtr(pKeyLow.Data,pKeyLow.Size,wIndexItem.Data,wIndexItem.Size);
//     wR= _Compare::_compare(&pKey,&pZTab[whigh]._Key) ;
     if (wR==0)
         {
         pCollection->setStatus( ZS_FOUND);
         goto _RawsearchIntervalFirstBackProcess ;  // capture all collection before current wZIR.IndexRank
         }// if wR==0
     if (wR > 0)
         {
         wSt= ZS_OUTBOUNDHIGH;
         pCollection->setStatus(ZS_OUTBOUNDHIGH);
         goto _RawsearchIntervalFirstBackProcess;
         }// if wR==0

    wpivot = ((whigh-wlow)/2) +wlow ;

     while ((whigh-wlow)>2) //------Main loop around wpivot---------------------
         {

        pZIR.IndexAddress = wpivot ;
        pCollection->Context.CurrentZIFrank = wpivot;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexAddress))!=ZS_SUCCESS)
                                                        { return  pCollection->getStatus();}
        wIndexItem.fromFileKey(wIndexRecord);
//        wR= wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize);
        wR=URFComparePtr(pKeyLow.Data,pKeyLow.Size,wIndexItem.Data,wIndexItem.Size);
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
        pZIR.IndexAddress = wpivot ;
        pCollection->Context.CurrentZIFrank = wpivot;
        pZIF.ZPMSStats.Iterations++;
        } // while (whigh-wlow)>2 ------End Main loop around wpivot---------------------

_RawsearchIntervalFirstBackProcess:
/*
 *
 *  Search for the first key value in Index order
 */
    if ((pCollection->getStatus()==ZS_FOUND)&&(pCollection->Context.Op & ZCOP_Exclude) )
                                                            pCollection->setStatus(ZS_NOTFOUND); // exclude the lowest key value found
                else
                {
                pZIR.IndexAddress = pZIF.getCurrentRank();// include the lowest key value found as valid candidate (need to test highest key value)
                pZIR.ZMFAddress = wIndexItem.ZMFAddress;
                }
    pCollection->Context.CurrentZIFrank = pZIR.IndexAddress; // search for matches before current indexrank to find the first key value in index order

// need to set the first record to First index rank for key
// search for lower range

    pZIF.ZPMSStats.Reads ++;
    wSt=pZIF.zgetPrevious(wIndexRecord,ZLock_Nolock);

    while (wSt==ZS_SUCCESS) {
      wIndexItem.fromFileKey(wIndexRecord);

      if (pCollection->Context.Op & ZCOP_Exclude) {
//        if (wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize)<= 0) // Interval excludes low key value
        if (URFComparePtr(pKeyLow.Data,pKeyLow.Size,wIndexItem.Data,wIndexItem.Size)<= 0)// Interval excludes low key value
          break;
        else
//          if (wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize) < 0) // Interval includes low key value
          if (URFComparePtr(pKeyLow.Data,pKeyLow.Size,wIndexItem.Data,wIndexItem.Size) < 0) // Interval includes low key value
                                           break;
          }
       pZIR.IndexAddress = pZIF.getCurrentRank();
       pZIR.ZMFAddress = wIndexItem.ZMFAddress;
       pCollection->Context.CurrentZIFrank= pZIR.IndexAddress;

        pZIF.ZPMSStats.Reads ++;
        pCollection->Context.CurrentZIFrank--;
        if (pCollection->Context.CurrentZIFrank < 0)
                                                break;
        wSt=pZIF.zget(wIndexRecord,pCollection->Context.CurrentZIFrank);
        }// while

// Now test the highest Key content value

   if (pCollection->Context.Op & ZCOP_Exclude) {
//     if (wZIFCompare((ZDataBuffer&)pKeyHigh,wIndexItem.KeyContent,wCompareSize) >= 0) { // Interval excludes high key value
     if (URFComparePtr(pKeyHigh.Data,pKeyHigh.Size,wIndexItem.Data,wIndexItem.Size) >= 0) { // Interval excludes high key value
        pCollection->setStatus(ZS_NOTFOUND);
        return  ZS_NOTFOUND;
     }
    }
    else {
//      if (wZIFCompare((ZDataBuffer&)pKeyHigh,wIndexItem.KeyContent,wCompareSize)> 0) {  // Interval includes high key value
      if (URFComparePtr(pKeyHigh.Data,pKeyHigh.Size,wIndexItem.Data,wIndexItem.Size)> 0) {  // Interval includes high key value
        pCollection->setStatus(ZS_NOTFOUND);
        return  ZS_NOTFOUND;
      }
    }
    pCollection->setStatus(ZS_FOUND);
    pCollection->Context.CurrentZIFrank = pZIR.IndexAddress ;

// OK : key is within the requested interval

/*    if (pLock!=ZLock_Nolock) // lock record if requested
        {
        pCollection->setStatus(static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress,pLock));
        if (pCollection->getStatus()!=ZS_SUCCESS)
            {
            return  pCollection->getStatus();
            }
        }// !=ZLock_Nolock
*/
    pCollection->push(pZIR); // enrich Collection with found record.

    ZException.clearStack();

    return  pCollection->setStatus(ZS_FOUND);

}// _RawsearchIntervalFirst

/**
 * @brief ZIndexFile::_RawsearchIntervalNext
 * @param[out]    pZIR
 * @param[in-out] pCollection
 * @return
 */
ZStatus
ZRawIndexFile::_RawsearchIntervalNext (ZIndexResult       &pZIR,
                                 ZIndexCollection*  pCollection)
{


ZIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//

//-----------Initialization Section---------------------------------

    if (pCollection==nullptr)  // if no Context then errored
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Cannot invoke _RawsearchNext without having Collection / Context : invoke first _RawsearchFirst ");
        return  ZS_INVOP;
        } // pCollection == nullptr

//-----------End Initialization Section---------------------------------

    if (pCollection->getStatus()!=ZS_FOUND)  // if nothing has been found or error : return  not found
                        { return  pCollection->getStatus();}


    pCollection->Context.Op = pCollection->Context.Op & ~ ZCOP_GetFirst ;   // clear ZCOP_GetFirst
    pCollection->Context.Op |= ZCOP_Interval | ZCOP_GetNext ;       // set ZCOP_GetNext (plus ZCOP_Interval if necessary)

// up to here we have the first found index record in key order at pCollection[0]

    // Anything equal after wIndexFound : push

    pCollection->Context.CurrentZIFrank ++;
    if (pCollection->Context.CurrentZIFrank > pCollection->ZIFFile->lastIdx())
        {
        pCollection->Context.CurrentZIFrank = pCollection->ZIFFile->lastIdx();
        return  pCollection->setStatus(ZS_EOF);
        }

    pCollection->ZIFFile->ZPMSStats.Reads ++;
    pCollection->setStatus(pCollection->ZIFFile->zget(wIndexRecord,pCollection->Context.CurrentZIFrank));
    if (pCollection->getStatus()==ZS_SUCCESS)
        {
        wIndexItem.fromFileKey(wIndexRecord);
        // For get next : only test the highest Key content value

       if (pCollection->Context.Op & ZCOP_Exclude) {
//         if (pCollection->Context.Compare(pCollection->Context.KeyHigh,wIndexItem.KeyContent,pCollection->Context.CompareSize)>0)  // Interval excludes high key value
         if (URFComparePtr( pCollection->Context.KeyHigh.Data,pCollection->Context.KeyHigh.Size,
                            wIndexItem.Data,wIndexItem.Size) > 0) { // Interval excludes high key value
            pCollection->setStatus(ZS_EOF);
            return  ZS_EOF;
         }
        }
        else {
//          if (pCollection->Context.Compare(pCollection->Context.KeyHigh,wIndexItem.KeyContent,pCollection->Context.CompareSize)>=0) // Interval includes high key value
          if (URFComparePtr(  pCollection->Context.KeyHigh.Data,pCollection->Context.KeyHigh.Size,
                              wIndexItem.Data,wIndexItem.Size) >= 0) { // Interval includes high key value
                pCollection->setStatus(ZS_EOF);
                return  ZS_EOF;
          }
        }
        pZIR.IndexAddress = pCollection->ZIFFile->getCurrentRank();
        pZIR.ZMFAddress =wIndexItem.ZMFAddress;

/*            if (pCollection->getLock()!=ZLock_Nolock) // lock record if requested
                {
                pCollection->setStatus (static_cast<ZMasterFile *>(pCollection->ZIFFile->ZMFFather)->zlockByAddress(pZIR.ZMFAddress,pCollection->getLock()));
                if (pCollection->getStatus()!=ZS_SUCCESS)
                    {
                    return  pCollection->getStatus ();
                    }
                }// !=ZLock_Nolock
*/
            pCollection->setStatus(ZS_FOUND);
            pCollection->push(pZIR); // after Found index : push

    }// if (pCollection->getStatus()==ZS_SUCCESS)

    return  pCollection->getStatus();

}// _RawsearchIntervalNext

//--------------------------End Search routines--------------------------------------


utf8String ZRawIndexFile::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel+1;
  utf8String wReturn;
  wReturn = fmtXMLnode("index",pLevel);

  wReturn += ZIndexControlBlock::toXml(wLevel,pComment);

  wReturn += ZRandomFile::toXml(wLevel,pComment);

  wReturn += fmtXMLendnode("index",pLevel);
  return wReturn;
} // toXml



ZStatus ZRawIndexFile::fromXml(zxmlNode* pIndexNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  uint32_t wInt=0;

  ZStatus wSt;
  if (!(pIndexNode->getName()=="indextable")) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        ZS_INVNAME,
        "ZSIndexControlBlock::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "indexcontrolblock",
        decode_ZStatus(wSt));
    return ZS_INVNAME;
  }
  //  if (ZSICBOwnData::fromXml(wRootNode, pErrorlog) != 0)
  //    return ZS_XMLERROR;

  if (XMLgetChildText(wRootNode, "indexname", IndexName, pErrorlog) < 0)
  {
    pErrorlog->errorLog("ZSIndexControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter <%s>.\n","indexname");
  }


  if (XMLgetChildUInt(wRootNode, "keyuniversalsize", KeyUniversalSize, pErrorlog)< 0)
  {
    pErrorlog->errorLog("ZSIndexControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter <%s>.\n","keyuniversalsize");
  }
  if (XMLgetChildUInt(wRootNode, "duplicates", wInt, pErrorlog)< 0)
  {
    pErrorlog->warningLog("ZSIndexControlBlock::fromXml-W-CNTFINDPAR Cannot find parameter <%s>. Will stay to its default.\n","duplicates");
    //      Duplicates = ZST_NODUPLICATES;
  }
  else
    Duplicates = (ZSort_Type)wInt;

  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//fromXml


/** @}
  */

//}// namespace zbs

#endif //ZRAWINDEXFILE_CPP
