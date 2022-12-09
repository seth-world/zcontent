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

#include <zxml/zxmlprimitives.h>

#include <zcontentcommon/urfparser.h>


using namespace zbs ;





/** @addtogroup ZIndex
*   @{
*/


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

    ZMFFather=pFather;
    if (pFather==nullptr)
                {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVADDRESS,
                                    Severity_Fatal,
                                    "ZRawIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
            this->~ZRawIndexFile();
            ZException.exit_abort();
                }
    return ;
}// ZIF CTOR


ZRawIndexFile::ZRawIndexFile  (ZRawMasterFile *pFather, ZIndexControlBlock &pZICB): ZRandomFile(),ZIndexControlBlock(pZICB)
{

    ZMFFather=pFather;

    if (pFather==nullptr)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVADDRESS,
                                        Severity_Fatal,
                                        "ZIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
                this->~ZRawIndexFile();
                ZException.exit_abort();
                }

}// ZIF CTOR 2 w

ZRawIndexFile::ZRawIndexFile  (ZRawMasterFile *pFather, int pKeyUniversalsize, const utf8String &pIndexName , ZSort_Type pDuplicates): ZRandomFile(),ZIndexControlBlock()
{

  ZMFFather=pFather;

  if (pFather==nullptr)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVADDRESS,
        Severity_Fatal,
        "ZRawIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
    this->~ZRawIndexFile();
    ZException.exit_abort();
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


#include <zindexedfile/zmasterfile.h>



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

    ZDataBuffer wRawICB;

    wSt=ZRandomFile::getReservedBlock(wRawICB,true);  // get ICB on local ZIndex file

    if (wSt!=ZS_SUCCESS)
                {
                 ZException.addToLast( " Reading Reserved header zone ZIndexFile %s",
                                        pIndexUri.toString());
                 ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
    if (CheckSum!=nullptr)
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

    ZPMSStats = ZPMS ;
    return  ZS_SUCCESS ;
}//zopenIndexFile



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
  size_t wRet=ZIndexControlBlock::_exportAppend(wICBContent);
  setReservedContent(wICBContent); /* zclose updates file's blocks (header - index file control block etc.)*/
/*  wSt=ZRandomFile::updateReservedBlock(ZIndexControlBlock::_exportAppend(wICBContent),true);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast( " Writing Reserved header for ZIndexFile %s",URIIndex.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
*/
    return  ZRandomFile::zclose();
}//closeIndexFile
/**
 * @brief ZIndexFile::writeIndexControlBlock
 * @param pCheckSum a pointer to a pointer on checkSum field.
 * If nullptr, no checkSum is returned
 * if not nullptr, new checkSum on IndexControlBlock raw data is computed and pointer is returned.
 * @return ZStatus
 */
ZStatus
ZRawIndexFile::writeIndexControlBlock(checkSum **pCheckSum)
{

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
    if (pCheckSum==nullptr)
        {
        return  ZS_SUCCESS;        // no checkSum requested
        }
   *(pCheckSum) = wICBContent.newcheckSum();  // else compute checksum and return pointer to it (pointer to pointer)
    return  ZS_SUCCESS;
}//writeIndexControlBlock




/*
ZStatus
ZIndexFile::removeRollback         (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress)
{
    return  (addIndexValue(pRecord,pZMFRank,pAddress));
}
ZStatus
ZIndexFile::insertRollback         (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress)
{
    return  (removeIndexValue(pRecord,pZMFRank,pAddress));
}
*/
ZStatus
ZRawIndexFile::removeIndexValue        (const ZDataBuffer& pKey , zaddress_type &pAddress)
{

ZStatus         wSt;

ZIndexCollection wZIRList(this);
//zaddress_type   wZMFAddress;
//long            wIndexRank;
ZIndexResult wZIR;

    if (Duplicates == ZST_DUPLICATES)
            {
//            wSt=_RawsearchAll(pKey,*this,wZIRList,ZMS_MatchIndexSize);
            wSt = _URFsearchAll(pKey,wZIRList,ZMS_MatchIndexSize);
            if (wSt!=ZS_FOUND)
                        {return  wSt;}
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
                                                IndexName.toCChar()
                                                );
                        ZException.setLastSeverity(Severity_Severe);
                        return  (ZS_INVADDRESS); // pAddress has not been matched
                        }
                    } // else
            }// if duplicates
            else
            {   // meaning No duplicates
//        wSt=_Rawsearch(pKey,*this,wZIR,ZMS_MatchIndexSize);
//        wSt=_Rawsearch(pKey,*this,wZIR);
        wSt = _URFsearch(pKey,wZIR,ZLock_Nolock);
        if (wSt!=ZS_FOUND)
                    {  return  wSt;}
            }
// At this stage we have one ZIR with the IndexRank to remove within ZIndexFile

    return  (ZRandomFile::zremove(wZIR.IndexRank));
}// removeIndexValue


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
 * @param[in-out] pIndexItem a pointer to ZIndexItem struct that will contain the Index element to be added
 * @param[out] pIndexIdxCommit
 * @param[in] pZMFAddress The ZMF block address to set the index rank with
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawIndexFile::_addRawKeyValue_Prepare( ZIndexItem* &pOutIndexItem,
                                        zrank_type &pZBATIndex,
                                        ZDataBuffer& pKeyContent,
                                        const zaddress_type pZMFAddress)
{

ZStatus wSt=ZS_SUCCESS;
//zaddress_type wZMFAddress;
//zaddress_type wIndexAddress; // not used but necessary for base ZRandomFile class calls
ZIndexResult wZIR;

//ZIFCompare wZIFCompare = ZKeyCompareBinary;


//-----------Comparison routine selection------------------------------------
/*
    if (IdxKeyDic->size()==1)           // if only one field
        {
            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison
*/

// get with seekGeneric the ZIndexFile row position to insert

//  1-insert accordingly (push_front, push , insert)

//  2- shift all ZAM indexes references according pZAMIdx value :
//  NB We do not care about that : only addresses are stored, not the order

    pOutIndexItem=new ZIndexItem(pKeyContent);
//    pOutIndexItem.set(pKeyContent);
    pOutIndexItem->Operation = ZO_Add;
    pOutIndexItem->ZMFaddress = pZMFAddress ;  // store address to ZMF Block
    pOutIndexItem->KeyContent = pKeyContent ;
//    pKeyValue.State = ZAMInserted ;

/* key data is supposed to have been extracted when coming here */
//    _keyValueExtraction(ZICB->ZKDic,pZMFRecord,pIndexItem->KeyContent);        // create key value content from user record

//    wSt= _Rawsearch (pIndexItem->KeyContent,*this,wZIR,ZMS_MatchIndexSize,wZIFCompare);  // will not use wStats except for special seek mesure on add
//    wSt= _Rawsearch (pIndexItem.KeyContent,*this,wZIR,ZLock_Nolock);  // will not use wStats except for special seek mesure on add
    wSt = _URFsearch(pKeyContent,wZIR,ZLock_Nolock);

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
                pOutIndexItem->Operation=ZO_Push_front ;
//                ZJoinIndex=0;
                pZBATIndex=0L;
                wSt=_insert2Phases_Prepare (pOutIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress);// equivalent to push_front
                if (ZVerbose)
                        _DBGPRINT ("Index Push_Front  (index rank 0L )\n")

                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                pOutIndexItem->Operation=ZO_Push ;
//                ZJoinIndex=this->size();
                wSt=ZRandomFile::_add2Phases_Prepare(pOutIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress);// equivalent to push
                if (ZVerbose)
                        _DBGPRINT ("Index Push\n")
                break;
                }
            case (ZS_NOTFOUND):
                {
                pOutIndexItem->Operation=ZO_Insert ;
                pZBATIndex=wZIR.IndexRank;
                wSt=ZRandomFile::_insert2Phases_Prepare(pOutIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress);// insert at position returned by seekGeneric
//                ZJoinIndex=wRes.ZIdx;
                if (ZVerbose)
                  _DBGPRINT ("Index insert at rank <%ld>\n", wZIR.IndexRank)
                break;
                }
            case (ZS_FOUND):
                {
                if (Duplicates==ZST_NODUPLICATES)
                            {
                            if (ZVerbose)
                                    _DBGPRINT("***Index Duplicate key exception at rank <%ld>\n", wZIR.IndexRank)
                            ZException.setMessage(_GET_FUNCTION_NAME_,
                                                    ZS_DUPLICATEKEY,
                                                    Severity_Error,
                                                    " Duplicate value on key set to ZST_NODUPLICATES index rank <%ld>",wZIR.IndexRank);
                            wSt=ZS_DUPLICATEKEY;
                            break;
                            }
                pOutIndexItem->Operation=ZO_Insert ;

                if (ZVerbose)
                        _DBGPRINT ("Index Duplicate key insert at rank <%ld>\n", wZIR.IndexRank)
                pZBATIndex=wZIR.IndexRank;
                wSt=ZRandomFile::_insert2Phases_Prepare(pOutIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress); // insert at position returned by seekGeneric

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
 *  In all error cases, ZException is set with an appropriate message describing error.
 *
 * @param[in] pKey key content with a ZIndex format. @see _keyValueExtraction()
 * @param[out] pIndexItem a pointer to a ZIndexItem : this object is generated using new ZIndexItem
 * @param[out] pIndexRank logical position within ZIndexFile for the key content
 * @param[out] pZMFAddress Physical address within ZMasterFile for the block corresponding to key value
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRawIndexFile::_removeKeyValue_Prepare(ZDataBuffer & pKey,
                                    ZIndexItem* &pIndexItem,
                                    long& pIndexRank,
                                    zaddress_type &pZMFAddress)
{

ZStatus wSt;
ZResult wRes;
//zaddress_type wIndexAddress;
ZIndexResult wZIResult;

//ZIFCompare         wZIFCompare = ZKeyCompareBinary;

//-----------Comparison routine selection------------------------------------


//    wSt=_Rawsearch(pKey, *this,wZIR,ZMS_MatchIndexSize,wZIFCompare);
//    wSt=_Rawsearch(pKey, *this,wZIResult,ZLock_Nolock);
    wSt = _URFsearch(pKey,wZIResult,ZLock_Nolock);
    if (wSt!=ZS_FOUND)  // Return status is either not found, record lock or other error
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    wRes.ZSt,
                                    Severity_Error,
                                    "During remove operation : Index value not found on index name <%s>",
                                     IndexName.toCChar());
            ZException.setLastSeverity(Severity_Severe);
            return  wSt;
            } // wSt!=ZS_FOUND

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.lock();
#endif

    pIndexRank=wZIResult.IndexRank ;

    pIndexItem = new ZIndexItem ;
    pIndexItem->Operation=ZO_Erase ;
    pIndexItem->KeyContent = pKey;
    pIndexItem->ZMFaddress = 0 ; // dont know yet

    wSt=ZRandomFile::_remove_Prepare(wZIResult.IndexRank,wZIResult.ZMFAddress);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif

    return  wSt;
}//_removeKeyValue_Prepare


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
  wSt = _URFsearch(pIndexItem.KeyContent,wZIResult,ZLock_Nolock);
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

        wRDicRank=pZIF->IdxKeyDic->Tab[wi].MDicRank;
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
    return (strncmp(pKey1.DataChar,pKey2.DataChar,wSize));
}// ZKeyCompareAlpha


} // namespace zbs

//-----------------------Index Search routines------------------------------------------




int URFCompareValues(const unsigned char* &pURF1,const unsigned char* pURF1_End,const unsigned char* &pURF2,const unsigned char* pURF2_End) {
  int wRet=0;

    wRet=0;
    while ((wRet==0) && (pURF1 < pURF1_End) && (pURF2 < pURF2_End)){
      wRet = (*pURF1++)-(*pURF2++);
    }
    if (wRet==0) {
      if (pURF1 > pURF1_End) {
        return 1; /* key1 is greater than key2 */
      }
      if (pURF2 > pURF2_End) {
        return -1; /* key1 is less than key2 */
      }
      /* equality in size and in values */
      return 0;
    }
    return wRet;
}
/**
 * @brief URFCompare  Compare two buffers composed each of one or many URF fields, each field potentially of variable length.
 */
int URFCompare(const ZDataBuffer &pKey1, ZDataBuffer &pKey2) {
  if (pKey1.isEmpty()) {
    if (pKey2.isEmpty())
      return 0;   /* both keys are empty : equality */
    return - 1;   /* key2 greater than key1 : negative value */
  }
  if (pKey2.isEmpty()) {
    return 1 ;    /* key1 greater than key2 : positive value */
  }

  ZDataBuffer wValue1,wValue2;
  ZStatus wSt;
  ZTypeBase wType;
  ssize_t    wSize;
  const unsigned char* wURF1 = pKey1.Data;
  const unsigned char* wURF1_End = pKey1.Data + pKey1.Size;
  const unsigned char* wEnd1 = pKey1.Data + pKey1.Size;
  const unsigned char* wURF2 = pKey2.Data;
  const unsigned char* wURF2_End = pKey2.Data + pKey2.Size;
  const unsigned char* wEnd2 = pKey2.Data + pKey2.Size;

  int wRet=0;
  wSt=URFParser::getURFTypeAndSize(wURF1,wType,wSize);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("URFCompare",wSt,Severity_Fatal,"Error while comparing key values.");
    ZException.exit_abort();
  }
  wURF1_End = wURF1 + wSize;

  wSt=URFParser::getURFTypeAndSize(wURF2,wType,wSize);
  if (wSt!=ZS_SUCCESS){
    ZException.setMessage("URFCompare",wSt,Severity_Fatal,"Error while comparing key values.");
    ZException.exit_abort();
  }
  wURF2_End = wURF2 + wSize;

  while ( (wRet==0) && (wSt == ZS_SUCCESS ) && (wURF1 < wEnd1) && (wURF2 < wEnd2) ) {

    wRet = URFCompareValues (wURF1,wURF1_End,wURF2,wURF2_End);  /* pURF1 and pURF2 are updated */

    wSt=URFParser::getURFTypeAndSize(wURF1,wType,wSize);
    if (wSt!=ZS_SUCCESS)
      break;
    wURF1_End = wURF1 + wSize;
    wSt=URFParser::getURFTypeAndSize(wURF2,wType,wSize);
    if (wSt!=ZS_SUCCESS)
      break;
    wURF2_End = wURF2 + wSize;
  }// while
  return wRet;
} // URFCompare


ZStatus
ZRawIndexFile::_URFsearch(  const ZDataBuffer &pKey,
                            ZIndexResult &pZIR,
                            const zlockmask_type pLock)
{
  ZStatus     wSt= ZS_NOTFOUND;

  pZIR.IndexRank = 0;
  ZDataBuffer wIndexRecord;
  ZIndexItem wIndexItem ;
  ssize_t wCompareSize=0;

 // ZIFCompare    wZIFCompare=ZKeyCompareBinary; /* use binary compare */

  int wR;

  long whigh = 0;
  long wlow = 0;
  long wpivot = 0;

  if (getSize()==0)
  {
    wSt= ZS_OUTBOUNDHIGH ;
    goto _URFsearch_Return;
  }
  // Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

//  wCompareSize = pKey.Size;

  whigh = lastIdx();  // last element : highest boundary
  wlow = 0;           // first element : lowest boundary

  wSt=ZS_NOTFOUND;

  pZIR.IndexRank = wlow ;

  ZPMSStats.Reads ++;
  if ((wSt=zget(wIndexRecord,0L))!=ZS_SUCCESS)
    goto _URFsearch_Return;
  if (wIndexRecord.isEmpty()) {
    fprintf(stderr,"ZRawIndexFile::_URFsearch-W-EMPTY Index file <%s> : index record rank 0 is empty.",
        getURIContent().toString());
  }

  wIndexItem.fromFileKey(wIndexRecord);

  if (wIndexItem.isEmpty()){
    fprintf(stderr,"ZRawIndexFile::_URFsearch-W-EMPTY Index file <%s> : index record rank 0 is empty.",
        getURIContent().toString());
  }

  wR = URFCompare(pKey,wIndexItem);
  if (wR==0)
  {
    wSt=ZS_FOUND ;
    pZIR.IndexRank = wlow ;
    goto _URFsearch_Return;
  }
  if (wR<0)
  {return  ZS_OUTBOUNDLOW;}

  pZIR.IndexRank = whigh ;

  ZPMSStats.Reads ++;
  if ((wSt=zget(wIndexRecord,whigh))!=ZS_SUCCESS)
    goto _URFsearch_Return;
  wIndexItem.fromFileKey(wIndexRecord);
  wR= URFCompare(pKey,wIndexItem.KeyContent);

  if (wR==0)
  {
    wSt=ZS_FOUND ;
    pZIR.IndexRank = whigh ;
    goto _URFsearch_Return;
  }
  if (wR>0)
  {return  ZS_OUTBOUNDHIGH;}

  wpivot = ((whigh-wlow)/2) +wlow ;

  while ((whigh-wlow)>2)// ---------------Main loop around pivot----------------------
  {
    pZIR.IndexRank = wpivot ;

    ZPMSStats.Reads ++;
    if ((wSt=zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
      goto _URFsearch_Return;
    wIndexItem.fromFileKey(wIndexRecord);
    wR= URFCompare(pKey,wIndexItem.KeyContent);

    if (wR==0)
    {
      wSt=ZS_FOUND ;
      goto _URFsearch_Return;
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
    ZPMSStats.Iterations ++;
  }// while (whigh-wlow)>2---------------Main loop around pivot----------------------

  /*
   *
   *  Process the remaining collection
   */

  wpivot = wlow;
  pZIR.IndexRank = wpivot;
  ZPMSStats.Reads ++;
  if ((wSt=zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
  { return  wSt;}
  wIndexItem.fromFileKey(wIndexRecord);
  wR= URFCompare(pKey,wIndexItem.KeyContent);

  //        while ((wpivot<whigh)&&(_Compare::_compare(&pKey,&pZTab[wpivot]._Key) > 0))
  while ((wpivot<whigh)&& (wR > 0))
  {
    wpivot ++;
    pZIR.IndexRank=wpivot;

    ZPMSStats.Reads ++;
    if ((wSt=zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
    { return  wSt;}
    wIndexItem.fromFileKey(wIndexRecord);
    wR= URFCompare(pKey,wIndexItem.KeyContent);
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

_URFsearch_Return:
  pZIR.ZMFAddress = wIndexItem.ZMFaddress ;
  /*    if ((wSt=ZS_FOUND)&&(pLock != ZLock_Nolock ))
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

        wZIR.IndexRank= wlow ;
        ZPMSStats.Reads ++;
        if (pCollection.setStatus(zget(wIndexRecord,ZLock_Nolock))!=ZS_SUCCESS)
                                                goto _URFsearchAllError;
        wIndexItem.fromFileKey(wIndexRecord);
//        wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);
        wR = URFCompare(pKey,wIndexItem.KeyContent);
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
     wZIR.IndexRank = whigh ;
     ZPMSStats.Reads ++;
     if (pCollection.setStatus(zget(wIndexRecord,wZIR.IndexRank))!=ZS_SUCCESS)
                                             goto _URFsearchAllError;
     wIndexItem.fromFileKey(wIndexRecord);
//     wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);
     wR = URFCompare(pKey,wIndexItem.KeyContent);
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
        wZIR.IndexRank = wpivot ;
        ZPMSStats.Reads ++;
        if (pCollection.setStatus(zget(wIndexRecord,wZIR.IndexRank))!=ZS_SUCCESS)
                                                goto _URFsearchAllError;
        wIndexItem.fromFileKey(wIndexRecord);
//        wR= wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize);
        wR = URFCompare(pKey,wIndexItem.KeyContent);
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
    wIndexFound = wZIR.IndexRank; // search for matches before and after wIndexFound
    pCollection.setStatus(ZS_FOUND) ;

    wZIR.ZMFAddress =wIndexItem.ZMFaddress;

    pCollection.push(wZIR);

    // need to capture anything equal before wIndexFound: push_front - then anything AFTER wIndexFound : push
    // search for lower range

    // Anything equal before wIndexFound : push_front
    ZPMSStats.Reads ++;
    wSt=zgetPrevious(wIndexRecord,ZLock_Nolock);
    wIndexItem.fromFileKey(wIndexRecord);
//    while ((wSt==ZS_SUCCESS)&&(wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize)==0))
    while ((wSt==ZS_SUCCESS)&& (URFCompare(pKey,wIndexItem.KeyContent)==0)) {
        wZIR.IndexRank = getCurrentRank();
        wZIR.ZMFAddress =wIndexItem.ZMFaddress;

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
    wZIR.IndexRank = wIndexFound;
    wSt=zget(wIndexRecord,wIndexFound);
    if (wSt==ZS_SUCCESS) {
        wIndexItem.fromFileKey(wIndexRecord);
        while ((wSt==ZS_SUCCESS)&& (URFCompare(pKey,wIndexItem.KeyContent)==0)) {
//        while ((wZIFCompare((ZDataBuffer&)pKey,wIndexItem.KeyContent,wCompareSize)==0)&&(wSt==ZS_SUCCESS))
//            {
            wZIR.IndexRank = getCurrentRank();
            wZIR.ZMFAddress =wIndexItem.ZMFaddress;

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
/*
     wCompareSize = pKeyContent.Size;
    if (wCompareSize> pZIF.KeyUniversalSize)
                wCompareSize=pZIF.KeyUniversalSize;
*/


    pCollection=new ZIndexCollection (this); // initialize all and create ZSIndexCollection instance
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

    pZIR.IndexRank= wlow ;
    ZPMSStats.Reads ++;
    if (pCollection->setStatus(zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                            {return  pCollection->getStatus();}//
    wIndexItem.fromFileKey(wIndexRecord);
    wR= wZIFCompare(pKeyContent,wIndexItem.KeyContent,wCompareSize);

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
     ZPMSStats.Reads ++;
     if (pCollection->setStatus(zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
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
        ZPMSStats.Reads ++;
        if (pCollection->setStatus(zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
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
    pZIR.IndexRank = getCurrentRank();
    pZIR.ZMFAddress = wIndexItem.ZMFaddress;

    pCollection->Context.CurrentZIFrank = pZIR.IndexRank; // search for matches before current indexrank to find the first key value in index order

// need to set the first record to First index rank for key
// search for lower range

    ZPMSStats.Reads ++;
    pCollection->setStatus(zgetPrevious(wIndexRecord,ZLock_Nolock));

    while ((pCollection->getStatus()==ZS_SUCCESS)&&(wZIFCompare((ZDataBuffer&)pKeyContent,wIndexItem.KeyContent,wCompareSize)==0))
        {
        wIndexItem.fromFileKey(wIndexRecord);
        pZIR.IndexRank = getCurrentRank();
        pZIR.ZMFAddress = wIndexItem.ZMFaddress;
        pCollection->Context.CurrentZIFrank= pZIR.IndexRank;

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

ZIFCompare wZIFCompare=ZKeyCompareBinary; // comparison routine is deduced from Dictionary Key field type

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

    pCollection->Context.setup (pKeyLow,&pKeyHigh,wZIFCompare,wCompareSize); // initialize all and create ZSIndexCollection instance
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
        pZIR.IndexRank= wlow ;
        pCollection->Context.CurrentZIFrank = wlow;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                            { return  pCollection->getStatus();}
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize);

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
     pZIR.IndexRank = whigh ;
     pCollection->Context.CurrentZIFrank = whigh;
     pZIF.ZPMSStats.Reads ++;
     if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                         {  return  pCollection->getStatus();}
     wIndexItem.fromFileKey(wIndexRecord);
     wR= wZIFCompare((ZDataBuffer&)pKeyLow,wIndexItem.KeyContent,wCompareSize);
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

        pZIR.IndexRank = wpivot ;
        pCollection->Context.CurrentZIFrank = wpivot;
        pZIF.ZPMSStats.Reads ++;
        if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                        { return  pCollection->getStatus();}
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

_RawsearchIntervalFirstBackProcess:
/*
 *
 *  Search for the first key value in Index order
 */
    if ((pCollection->getStatus()==ZS_FOUND)&&(pCollection->Context.Op & ZCOP_Exclude) )
                                                            pCollection->setStatus(ZS_NOTFOUND); // exclude the lowest key value found
                else
                {
                pZIR.IndexRank = pZIF.getCurrentRank();// include the lowest key value found as valid candidate (need to test highest key value)
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
       pZIR.IndexRank = pZIF.getCurrentRank();
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
                                   return  ZS_NOTFOUND;
                                   }
            }
              else
            {
           if (wZIFCompare((ZDataBuffer&)pKeyHigh,wIndexItem.KeyContent,wCompareSize)> 0) // Interval includes high key value
                                   {
                                   pCollection->setStatus(ZS_NOTFOUND);
                                   return  ZS_NOTFOUND;
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
        pCollection->Context.CurrentZIFrank=pCollection->ZIFFile->lastIdx();
        return  pCollection->setStatus(ZS_EOF);
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
                                       return  ZS_EOF;
                                       }
                }
                  else
                {
               if (pCollection->Context.Compare(pCollection->Context.KeyHigh,wIndexItem.KeyContent,pCollection->Context.CompareSize)>=0) // Interval includes high key value
                                       {
                                       pCollection->setStatus(ZS_EOF);
                                       return  ZS_EOF;
                                       }
                }
            pZIR.IndexRank = pCollection->ZIFFile->getCurrentRank();
            pZIR.ZMFAddress =wIndexItem.ZMFaddress;



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

        }// if

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
