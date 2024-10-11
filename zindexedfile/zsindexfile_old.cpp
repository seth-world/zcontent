#ifndef ZSIndexFile_CPP
#define ZSIndexFile_CPP
#include <zindexedfile/zsindexfile.h>
//#include <zindexedfile/zifgeneric.h>
//#include <zindexedfile/zsmasterfile.h>
#include <zrandomfile/zrfcollection.h>

#include <zcontentcommon/zresult.h>
#include <zindexedfile/zindexcontrolblock.h>
#include <zindexedfile/zrecord.h>
#include <zindexedfile/zsindexitem.h>

#include <zindexedfile/zrawmasterfile.h>

using namespace zbs ;





/** @addtogroup ZIndex
*   @{
*/


//---------ZIndexControlBlock routines----------------------------------------------------


ZDataBuffer
ZSIndexControlBlock::_export()
{
 ZDataBuffer     wZDB;
 return _exportAppend(wZDB);
}

/**
 * @brief ZIndexControlBlock::_exportICB exports ZIndexControlBlock content to a flat ZDataBuffer.
 * @return a ZDataBuffer containing the flat content of ZIndexControlBlock
 */
ZDataBuffer &
ZSIndexControlBlock::_exportAppend(ZDataBuffer &pICBContent)
{
ZSICBOwnData_Export* wICBE;
int32_t  wDicOffset=0;

  ZSICBOwnData::_exportAppend(pICBContent);// export ICB own data in universal format

  wDicOffset=int32_t(pICBContent.Size);
/* no dictionary */
#ifdef __COMMENT__
  /* if Key dictionary does not exist */

  if (ZKDic==nullptr) // update ZDataBuffer with ICB size and dictionary offset to -1 (no dictionary)
    {
    wICBE=(ZSICBOwnData_Export*)pICBContent.Data;
    wICBE->ICBTotalSize = reverseByteOrder_Conditional<uint32_t>(uint32_t(pICBContent.Size)) ;
    wICBE->ZKDicOffset = reverseByteOrder_Conditional<int32_t>(-1) ;
    return pICBContent;
    }
  /* key dictionary exists */

//  ZDataBuffer     wZDB;

  ZKDic->_exportAppend(pICBContent);       // export key dictionary
#endif // #endif // __COMMENT__

  // update ZDataBuffer with ICB size and dictionary offset to effective offset to key dictionary
  wICBE=(ZSICBOwnData_Export*)pICBContent.Data;
  wICBE->ICBTotalSize = reverseByteOrder_Conditional<uint32_t>(uint32_t(pICBContent.Size)) ;
  wICBE->ZKDicOffset = reverseByteOrder_Conditional<int32_t>(int32_t(wDicOffset)) ;

  return  pICBContent ;
}// _exportICBAppend


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


//----------ZSIndexFile-----------------------------------------------


ZSIndexFile::ZSIndexFile  (ZRawMasterFile *pFather): ZRandomFile()
{

    ZMFFather=pFather;
    if (pFather==nullptr)
                {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVADDRESS,
                                    Severity_Fatal,
                                    "ZSIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
            this->~ZSIndexFile();
            ZException.exit_abort();
                }
    return ;
}// ZIF CTOR


ZSIndexFile::ZSIndexFile  (ZRawMasterFile *pFather, ZSIndexControlBlock &pZICB): ZRandomFile(),ZSIndexControlBlock(pZICB)
{

    ZMFFather=pFather;

    if (pFather==nullptr)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVADDRESS,
                                        Severity_Fatal,
                                        "ZSIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
                this->~ZSIndexFile();
                ZException.exit_abort();
                }

}// ZIF CTOR 2 w

ZSIndexFile::ZSIndexFile  (ZRawMasterFile *pFather, int pKeyUniversalsize, const utf8String &pIndexName , ZSort_Type pDuplicates): ZRandomFile(),ZSIndexControlBlock()
{

  ZMFFather=pFather;

  if (pFather==nullptr)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVADDRESS,
        Severity_Fatal,
        "ZSIndexFile::CTOR-F-IVFATHER ZMasterFile pointer is invalid (null value) \n");
    this->~ZSIndexFile();
    ZException.exit_abort();
  }

  KeyUniversalSize=pKeyUniversalsize;
  IndexName=pIndexName;
  Duplicates=pDuplicates;
}

ZStatus
ZSIndexFile::setIndexURI  (uriString &pURI)
{
    IndexUri=pURI;
    return _Base::setPath(pURI);
}

void
ZSIndexFile::setIndexName  (utf8String &pName)
{
    IndexName = pName;
    return ;
}


#include <zindexedfile/zsmasterfile.h>

/**
 * @brief ZSIndexFile::zrebuildIndex rebuilds the current index
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
ZSIndexFile::zrebuildIndex(bool pStat, FILE*pOutput)
{

ZStatus         wSt = ZS_SUCCESS;
//ZRawRecord *wRecord = static_cast<ZRawMasterFile *>(ZMFFather)->getRawRecord();
ZRawRecord *wRecord = ZMFFather->generateRawRecord();
zrank_type      wZMFRank = 0;
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
            return  ZS_MODEINVALID;
            }
    if (pStat)
            ZPMSStats.init();
    fprintf (pOutput,
             "______________Rebuilding Index <%s>_______________\n"
               " File is %s \n",
               IndexName.toCChar(),
               ZDescriptor.URIContent.toString());

    zsize_type wFatherSize = ZMFFather->getSize();
    zsize_type wSize = IndexRecordSize() * ZMFFather->getSize() ;
    if (ZVerbose)
            {
            fprintf (pOutput,
                     " Index file size is computed to be %ld\n",
                     wSize);

            fprintf (pOutput,"Clearing index file\n");
            }

    wSt=_Base::zclearFile(wSize);  // clearing file with a free block space equals to the whole index

    if (ZMFFather->isEmpty())
            {
            fprintf(pOutput,
                    " ------------No record in ZMasterFile <%s> : no index to rebuild..........\n",
                    ZMFFather->getURIContent().toString());


            return  ZS_SUCCESS;
            }


    wSt=ZMFFather->zgetWAddress(wRecord->Content,wZMFRank,wZMFAddress);
    for (zsize_type wi=0;(wSt==ZS_SUCCESS)&&(wi <wFatherSize);wi++ )
            {
 //           wZMFRank = wFather->zgetCurrentRank();
 //           wZMFAddress=wFather->zgetCurrentLogicalPosition();
            wIndexCount++;
            wSt=_addKeyValue(wRecord,wIndexRank,wZMFAddress);
            if (wSt!= ZS_SUCCESS)
                                break;
            wSt=ZMFFather->zgetNextWAddress(wRecord->Content,wZMFRank,wZMFAddress) ;
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
            return  wSt;
            }
    fprintf (pOutput," ---------Successfull end rebuilding process for Index <%s>------------\n",
                      IndexName.toCChar());
//    return  wSt;
    return  wSt;

}//zrebuildIndex


/**
 * @brief ZSIndexFile::removeIndexFiles Removes all files composing current index (before index remove at ZMasterFile level)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSIndexFile::removeIndexFiles(ZaiErrors *pErrorLog)
{

//    return  _Base::zremoveFile();
return  _Base::_removeFile(pErrorLog);
}

/**
 * @brief ZSIndexFile::zcreateIndex creates a new index file corresponding to the given specification ICB and ZRF parameters
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
ZSIndexFile::zcreateIndex(ZSIndexControlBlock &pICB,
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

    ZSIndexControlBlock::_copyFrom(pICB);
    wSt=setIndexURI(pIndexUri);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZSIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
//  ZSIndexFile Record size is KeySize (sum of fields lengths) plus size of a zaddress_type (pointer to Master File record)

    _Base::setCreateMaximum (pInitialSize,
                             pAllocatedBlocks,
                             pBlockExtentQuota,
                             pICB.KeyUniversalSize + sizeof(zaddress_type),
                             pHighwaterMarking,
                             pGrabFreeSpace);
    wSt= _Base::_create(ZDescriptor,pInitialSize,ZFT_ZSIndexFile,pReplace,false);             // Do not leave it open after file creation : ZRF_Exclusive | ZRF_All

    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZSIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
    wSt=_Base::_open(ZDescriptor,(ZRF_Exclusive | ZRF_All),ZFT_ZSIndexFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" While Creating ZSIndexFile %s\n",
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
                ZException.addToLast(" While Creating ZSIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }

    if (!pLeaveOpen)
//            return   _Base::zclose();
            return   _Base::zclose();
//    return  ZS_SUCCESS;
return  ZS_SUCCESS;
}//zcreateIndexFile


ZStatus
ZSIndexFile::zcreateIndex(ZSIndexControlBlock &pICB,
                          uriString &pIndexUri,
                          long pAllocatedBlocks,
                          long pBlockExtentQuota,
                          zsize_type pInitialSize,
                          long pBlockTargetSize,
                          bool pHighwaterMarking,
                          bool pGrabFreeSpace,
                          bool pReplace,
                          bool pLeaveOpen)
{
  ZStatus wSt;

  ZSIndexControlBlock::_copyFrom(pICB);
  wSt=setIndexURI(pIndexUri);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" While Creating ZSIndexFile %s\n",
        pIndexUri.toString());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }
  //  ZSIndexFile Record size is KeySize (sum of fields lengths) plus size of a zaddress_type (pointer to Master File record)

  _Base::setCreateMaximum ( pAllocatedBlocks,
                            pBlockExtentQuota,
                            pBlockTargetSize,
                            pInitialSize,
                            pHighwaterMarking,
                            pGrabFreeSpace);
  wSt= _Base::_create(ZDescriptor,pInitialSize,ZFT_ZSIndexFile,pReplace,false);             // Do not leave it open after file creation : ZRF_Exclusive | ZRF_All

  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" While Creating ZSIndexFile %s\n",
        pIndexUri.toString());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }
  wSt=_Base::_open(ZDescriptor,(ZRF_Exclusive | ZRF_All),ZFT_ZSIndexFile);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" While Creating ZSIndexFile %s\n",
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
    ZException.addToLast(" While Creating ZSIndexFile %s\n",
        pIndexUri.toString());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }

  if (!pLeaveOpen)
    //            return   _Base::zclose();
    return   _Base::zclose();
  //    return  ZS_SUCCESS;
  return  ZS_SUCCESS;
}//zcreateIndexFile

ZStatus
ZSIndexFile::openIndexFile(uriString &pIndexUri,const int pMode)
{

ZStatus wSt;

    IndexUri = pIndexUri;

    wSt=_Base::setPath(IndexUri);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" setting path for ZSIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
    wSt=_Base::_open(ZDescriptor,pMode,ZFT_ZSIndexFile);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.last().Severity=Severity_Severe;
                ZException.addToLast(" Openning ZSIndexFile %s\n",
                                         pIndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
// --------- Need to read ICB and check whether it is ok with the given ICB --------------

/*     Compare Index Control Block read from Index File to the one stored in ZSMasterFile MCB  */

    ZDataBuffer wRawICB;

    wSt=_Base::getReservedBlock(wRawICB,true);  // get ICB on local ZIndex file

    if (wSt!=ZS_SUCCESS)
                {
                 ZException.addToLast( " Reading Reserved header zone ZSIndexFile %s",
                                        pIndexUri.toString());
                 ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }
    checkSum* wLocalCheckSum = wRawICB.newcheckSum();// compute checksum on it
    if ((*CheckSum)!=(*wLocalCheckSum))   // compare with ZMF Father's checkSum
        {
        ZDataBuffer wMasterICBContent;
        delete wLocalCheckSum;
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADCHECKSUM,
                              Severity_Error,
                              "Index Control Block checksum does not mach his related MasterFile ICB checksum for index file is %s",
                              IndexUri.toString()
                             );
        fprintf(stderr,
                "%s>> InvChecksum  Index Control Blocks checksum difference detected.\n Dumping ICBs and Performing unitary check.\n",
                _GET_FUNCTION_NAME_);


        ZSIndexControlBlock::_exportAppend(wMasterICBContent);

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
                              IndexUri.toString()
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

    ZPMSStats = ZDescriptor.ZPMS ;
    return  ZS_SUCCESS ;
}//zopenIndexFile



ZStatus
ZSIndexFile::closeIndexFile(void)
{

ZStatus wSt;
ZDataBuffer wICBContent;
//
// flush ICB to Index file
//
    wSt=_Base::updateReservedBlock(ZICB->_exportICBAppend(wICBContent),true);
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast( " Writing Reserved header for ZSIndexFile %s",IndexUri.toString());
                ZException.setLastSeverity(Severity_Severe);
                return  wSt;
                }

    return  _Base::zclose();
}//closeIndexFile
/**
 * @brief ZSIndexFile::writeIndexControlBlock
 * @param pCheckSum a pointer to a pointer on checkSum field.
 * If nullptr, no checkSum is returned
 * if not nullptr, new checkSum on IndexControlBlock raw data is computed and pointer is returned.
 * @return ZStatus
 */
ZStatus
ZSIndexFile::writeIndexControlBlock(checkSum **pCheckSum)
{

ZStatus wSt;
ZDataBuffer wICBContent;
//
// flush ICB to Index file and set ZFile_type to ZFT_IndexFile
//
    ZDescriptor.ZHeader.FileType = ZFT_ZSIndexFile;     // setting ZFile_type
    _Base::setReservedContent(ZICB->_exportICBAppend(wICBContent));
    wSt=_Base::_writeFullFileHeader(ZDescriptor,true);
 //   wSt=_Base::updateReservedBlock(ZICB->_exportICB());
    if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast (" Writing Reserved header zone ZSIndexFile %s",IndexUri.toString());
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
ZSIndexFile::removeRollback         (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress)
{
    return  (addIndexValue(pRecord,pZMFRank,pAddress));
}
ZStatus
ZSIndexFile::insertRollback         (ZDataBuffer& pRecord , long &pZMFRank,zaddress_type &pAddress)
{
    return  (removeIndexValue(pRecord,pZMFRank,pAddress));
}
*/
ZStatus
ZSIndexFile::removeIndexValue        (const ZDataBuffer& pKey , zaddress_type &pAddress)
{

ZStatus         wSt;

ZSIndexCollection wZIRList(this);
//zaddress_type   wZMFAddress;
//long            wIndexRank;
ZSIndexResult wZIR;

    if (ZICB->Duplicates == ZST_DUPLICATES)
            {
            wSt=_searchAll(pKey,*this,wZIRList,ZMS_MatchIndexSize);
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
                                                ZICB->IndexName.toCChar()
                                                );
                        ZException.setLastSeverity(Severity_Severe);
                        return  (ZS_INVADDRESS); // pAddress has not been matched
                        }
                    } // else
            }// if duplicates
            else
            {   // meaning No duplicates
//        wSt=_search(pKey,*this,wZIR,ZMS_MatchIndexSize);
        wSt=_search(pKey,*this,wZIR);
        if (wSt!=ZS_FOUND)
                    {  return  wSt;}
            }
// At this stage we have one ZIR with the IndexRank to remove within ZSIndexFile

    return  (_Base::zremove(wZIR.IndexRank));
}// removeIndexValue


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
 * @brief ZSIndexFile::_extractKeys extracts all defined keys from pRecordContent using pMasterDic givin pKeysContent as a result.
 * @param pRecordContent
 * @param pMasterDic
 * @param pKeysContent
 * @return
 */
ZStatus
ZSIndexFile::_extractKey(ZRawRecord& pRawRecord,  long pKeyRank, ZDataBuffer& pKeyContent)
{
  ZStatus wSt;
  size_t wKeyOffset = 0;
  size_t wSize =0,wRecordOffset=0;
  long wRDicRank=0;
  ZDataBuffer wFieldUValue;

  ZDataBuffer* wKeyValue=nullptr;

  if ((ZMFFather->ZMCB.MasterDic==nullptr)||(ZMFFather->ZMCB.MasterDic->isEmpty()))
    {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_BADDIC,
        Severity_Severe,
        " Dictionary is null or empty");
    return  ZS_BADDIC;
    }

  pKeyContent.allocateBZero(KeyUniversalSize+1);

  ZSKeyDictionary* wKeyDic = ZMFFather->ZMCB.MasterDic->KeyDic[pKeyRank];
  for (long wi=0 ; wi < wKeyDic->size() ; wi++)
  {
    // here put extraction rules. RFFU : Extraction could be complex. To be investigated and implemented

    wRDicRank=wKeyDic->Tab[wi].MDicRank;
    pRawRecord.getUniversalbyRank(wFieldUValue,wRDicRank);
    pKeyContent.changeData(wFieldUValue,wKeyOffset);

    wKeyOffset += pRecord.RDic->Tab[wRDicRank].MDicRank->UniversalSize;
  }//for



  pKeyOut.allocateBZero(pZKDic->KeyUniversalSize+1);

  for (long wi=0;wi<pZKDic->size();wi++)
  {
    // here put extraction rules. RFFU : Extraction could be complex. To be investigated and implemented

    wRDicRank=pZKDic->Tab[wi].MDicRank;
    pRecord.getUniversalbyRank(wFieldUValue,wRDicRank);
    pKeyOut.changeData(wFieldUValue,wKeyOffset);

    wKeyOffset += pRecord.RDic->Tab[wRDicRank].MDicRank->UniversalSize;
  }//for


}


ZStatus
ZSIndexFile::getUniversalbyRank (ZDataBuffer &pOutValue,
                                 ZBitset* pFieldPresence,
                                 const long pKeyRank,
                                 const long pFieldRank,
                                 bool pTruncate)
{

  if (ZMFFather->ZMCB.MasterDic==nullptr)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_BADDIC,
                            Severity_Error,
                            "No dictionary has been set (nullptr).");
    return ZS_BADDIC;
    }

  ZSKeyDictionary* wKeyDic = ZMFFather->ZMCB.MasterDic->KeyDic[pKeyRank];
  if ((pFieldRank<0)||(pFieldRank > wKeyDic->size()))
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_OUTBOUND,
        Severity_Severe,
        "trying to access field rank out of record dictionary boundaries");
    return ZS_OUTBOUND;
    }
  /* if presence bit set is nullptr then all fields are reputated present */
  if (pFieldPresence!=nullptr)
    {
    if ((pFieldRank >= pFieldPresence->EffectiveBitSize)||(!pFieldPresence->test(pFieldRank)))
      return ZS_FIELDMISSING;
    }


  unsigned char*wDataPtr= ZMFFather->ZMCB.MasterDic->Tab[wKeyDic->Tab[pFieldRank].MDicRank].offset

  unsigned char*wDataPtr=RDic->Tab[pRank].URFData->Data;
  if (wDataPtr==nullptr)
    return ZS_FIELDMISSING;
  ZTypeBase wType;
  memmove(&wType,wDataPtr,sizeof(ZTypeBase));
  wType=reverseByteOrder_Conditional<ZTypeBase>(wType);

  switch (wType)
  {
  case ZType_Utf8FixedString:
  {
    if (pTruncate)
      return utfStringHeader::getUniversalFromURF(ZType_Utf8FixedString,RDic->Tab[pRank].URFData->Data,pValue);
    else
      return utf8FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf8FixedString,RDic->Tab[pRank].URFData->Data,pValue);
  }//ZType_Utf8FixedString
    {
    case ZType_Utf16FixedString:
      if (pTruncate)
        return utf16FixedString<cst_desclen>::getUniversalFromURF_Truncated(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
      else
        return utf16FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
    }//ZType_Utf16FixedString
  case ZType_Utf32FixedString:
  {ZStatus
    ZRecord::getUniversalbyRank (ZDataBuffer &pValue,const long pRank,bool pTruncate)
    {
      if ((pRank<0)||(pRank>RDic->size()))
      {
        ZException.setMessage(_GET_FUNCTION_NAME_,
            ZS_OUTBOUND,
            Severity_Severe,
            "trying to access field rank out of record dictionary boundaries");
        return ZS_OUTBOUND;
      }
      if ((pRank>=FieldPresence->EffectiveBitSize)||(!FieldPresence->test(pRank)))
        return ZS_FIELDMISSING;

      unsigned char*wDataPtr=RDic->Tab[pRank].URFData->Data;
      if (wDataPtr==nullptr)
        return ZS_FIELDMISSING;
      ZTypeBase wType;
      memmove(&wType,wDataPtr,sizeof(ZTypeBase));
      wType=reverseByteOrder_Conditional<ZTypeBase>(wType);

      switch (wType)
      {
      case ZType_Utf8FixedString:
      {
        if (pTruncate)
          return utfStringHeader::getUniversalFromURF(ZType_Utf8FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        else
          return utf8FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf8FixedString,RDic->Tab[pRank].URFData->Data,pValue);
      }//ZType_Utf8FixedString
        {
        case ZType_Utf16FixedString:
          if (pTruncate)
            return utf16FixedString<cst_desclen>::getUniversalFromURF_Truncated(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
          else
            return utf16FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_Utf16FixedString
      case ZType_Utf32FixedString:
      {
        if (pTruncate)
          return utf32FixedString<cst_desclen>::getUniversalFromURF_Truncated(ZType_Utf32FixedString,RDic->Tab[pRank].URFData->Data,pValue);
        else
          return utf32FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
      }//ZType_Utf32FixedString
      case ZType_Utf8VaryingString:
      {
        return utfStringHeader::getUniversalFromURF(ZType_Utf8VaryingString,RDic->Tab[pRank].URFData->Data,pValue);

      }//ZType_Utf8VaryingString
      case ZType_Utf16VaryingString:
      {
        return utf16VaryingString::getUniversalFromURF(ZType_Utf16VaryingString,RDic->Tab[pRank].URFData->Data,pValue);
      }//ZType_Utf16VaryingString
      case ZType_Utf32VaryingString:
      {
        return utf32VaryingString::getUniversalFromURF(ZType_Utf32VaryingString,RDic->Tab[pRank].URFData->Data,pValue);
      }//ZType_Utf32VaryingString
        /*
    case ZType_FixedCString:
        {
        if (pTruncate)
            return templateString<cst_desclen>::getUniversalFromURF_Truncated(RDic->Tab[pRank].URFData->Data,pValue);
        else
            return templateString<cst_desclen>::getUniversalFromURF(RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_FixedCString

      case ZType_FixedWString:
      {
        if (pTruncate)
          return templateWString<cst_desclen>::getUniversalFromURF_Truncated(RDic->Tab[pRank].URFData->Data,pValue);
        else
          return templateWString<cst_desclen>::getUniversalFromURF(RDic->Tab[pRank].URFData->Data,pValue);
      }//ZType_FixedCWtring

      case ZType_VaryingCString:
      {
        return varyingCString::getUniversalFromURF(wDataPtr,pValue);
      }//ZType_VaryingCString

      case ZType_VaryingWString:
      {
        return varyingWString::getUniversalFromURF(wDataPtr,pValue);
      }//ZType_VaryingWString
        */

            case ZType_ZDate:
        {
          return ZDate::getUniversalFromURF(wDataPtr,pValue);
        }
      case ZType_ZDateFull:
      {
        return ZDateFull::getUniversalFromURF(wDataPtr,pValue);
      }
      case ZType_CheckSum:
      {
        return checkSum::getUniversalFromURF(wDataPtr,pValue);
      }

      case ZType_Blob:
      {
        return ZBlob::getUniversalFromURF(wDataPtr,pValue);
      }

      }// switch (wType)



      if (wType&ZType_Atomic)
      {
        ZTypeBase wTypeAtomic=wType&ZType_AtomicMask;
        size_t wUSize=getAtomicUniversalSize(wTypeAtomic);
        pValue.setData(wDataPtr+sizeof(ZTypeBase),wUSize);
        return ZS_SUCCESS;
      }

      if (wType&ZType_Array)
      {
        ZTypeBase wTypeAtomic=wType&ZType_AtomicMask;
        size_t wUSize=getAtomicUniversalSize(wTypeAtomic);
        wDataPtr += sizeof (ZTypeBase);
        uint16_t wArrayCount;
        memmove(&wArrayCount,wDataPtr,sizeof(wArrayCount));
        wArrayCount=reverseByteOrder_Conditional<uint16_t>(wArrayCount);
        wUSize=wUSize*wArrayCount;
        wDataPtr += sizeof (uint16_t);
        pValue.setData(wDataPtr,wUSize);

        return ZS_SUCCESS;
      }


      wDataPtr += RDic->Tab[pRank].MDicRank->HeaderSize;

      if (RDic->Tab[pRank].EffectiveUSize==0)
        RDic->Tab[pRank].EffectiveUSize=RDic->Tab[pRank].URFSize-RDic->Tab[pRank].MDicRank->HeaderSize;
      pValue.setData(wDataPtr,RDic->Tab[pRank].EffectiveUSize);
      return ZS_SUCCESS;
    }//getUniversalbyRank
    if (pTruncate)
      return utf32FixedString<cst_desclen>::getUniversalFromURF_Truncated(ZType_Utf32FixedString,RDic->Tab[pRank].URFData->Data,pValue);
    else
      return utf32FixedString<cst_desclen>::getUniversalFromURF(ZType_Utf16FixedString,RDic->Tab[pRank].URFData->Data,pValue);
  }//ZType_Utf32FixedString
  case ZType_Utf8VaryingString:
  {
    return utfStringHeader::getUniversalFromURF(ZType_Utf8VaryingString,RDic->Tab[pRank].URFData->Data,pValue);

  }//ZType_Utf8VaryingString
  case ZType_Utf16VaryingString:
  {
    return utf16VaryingString::getUniversalFromURF(ZType_Utf16VaryingString,RDic->Tab[pRank].URFData->Data,pValue);
  }//ZType_Utf16VaryingString
  case ZType_Utf32VaryingString:
  {
    return utf32VaryingString::getUniversalFromURF(ZType_Utf32VaryingString,RDic->Tab[pRank].URFData->Data,pValue);
  }//ZType_Utf32VaryingString
    /*
    case ZType_FixedCString:
        {
        if (pTruncate)
            return templateString<cst_desclen>::getUniversalFromURF_Truncated(RDic->Tab[pRank].URFData->Data,pValue);
        else
            return templateString<cst_desclen>::getUniversalFromURF(RDic->Tab[pRank].URFData->Data,pValue);
        }//ZType_FixedCString

  case ZType_FixedWString:
  {
    if (pTruncate)
      return templateWString<cst_desclen>::getUniversalFromURF_Truncated(RDic->Tab[pRank].URFData->Data,pValue);
    else
      return templateWString<cst_desclen>::getUniversalFromURF(RDic->Tab[pRank].URFData->Data,pValue);
  }//ZType_FixedCWtring

  case ZType_VaryingCString:
  {
    return varyingCString::getUniversalFromURF(wDataPtr,pValue);
  }//ZType_VaryingCString

  case ZType_VaryingWString:
  {
    return varyingWString::getUniversalFromURF(wDataPtr,pValue);
  }//ZType_VaryingWString
    */

        case ZType_ZDate:
    {
      return ZDate::getUniversalFromURF(wDataPtr,pValue);
    }
  case ZType_ZDateFull:
  {
    return ZDateFull::getUniversalFromURF(wDataPtr,pValue);
  }
  case ZType_CheckSum:
  {
    return checkSum::getUniversalFromURF(wDataPtr,pValue);
  }

  case ZType_Blob:
  {
    return ZBlob::getUniversalFromURF(wDataPtr,pValue);
  }

  }// switch (wType)



  if (wType&ZType_Atomic)
  {
    ZTypeBase wTypeAtomic=wType&ZType_AtomicMask;
    size_t wUSize=getAtomicUniversalSize(wTypeAtomic);
    pValue.setData(wDataPtr+sizeof(ZTypeBase),wUSize);
    return ZS_SUCCESS;
  }

  if (wType&ZType_Array)
  {
    ZTypeBase wTypeAtomic=wType&ZType_AtomicMask;
    size_t wUSize=getAtomicUniversalSize(wTypeAtomic);
    wDataPtr += sizeof (ZTypeBase);
    uint16_t wArrayCount;
    memmove(&wArrayCount,wDataPtr,sizeof(wArrayCount));
    wArrayCount=reverseByteOrder_Conditional<uint16_t>(wArrayCount);
    wUSize=wUSize*wArrayCount;
    wDataPtr += sizeof (uint16_t);
    pValue.setData(wDataPtr,wUSize);

    return ZS_SUCCESS;
  }


  wDataPtr += RDic->Tab[pRank].MDicRank->HeaderSize;

  if (RDic->Tab[pRank].EffectiveUSize==0)
    RDic->Tab[pRank].EffectiveUSize=RDic->Tab[pRank].URFSize-RDic->Tab[pRank].MDicRank->HeaderSize;
  pValue.setData(wDataPtr,RDic->Tab[pRank].EffectiveUSize);
  return ZS_SUCCESS;
}//getUniversalbyRank


/**
 * @brief ZSIndexFile::_addKeyValue Adds a key value from a ZMasterFile record to the current registrated ZSIndexFile instance.
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
ZSIndexFile::_addKeyValue(ZRawRecord* pZMFRecord,  zrank_type& pIndexRank, zaddress_type pZMFAddress)
{

long ZJoinIndex;
ZStatus wSt;

ZOp wZIndexOp;  // for journaling & history purpose

ZSIndexItem* wIndexItem = new ZSIndexItem ;

zrank_type wIndexIdxCommit;

    wSt=_addKeyValue_Prepare(pZMFRecord,wIndexItem,wIndexIdxCommit,pZMFAddress);
    if (wSt!=ZS_SUCCESS)
                  {  return  wSt;}// Beware return  is multiple instructions in debug mode
    wSt= _addKeyValue_Commit(wIndexItem,wIndexIdxCommit);
    delete wIndexItem;
    return  wSt;
}// _addKeyValue





#ifdef __COMMENT__
ZStatus
ZSIndexFile::_addKeyValue(ZDataBuffer &pElement,  long& pIndexRank, zaddress_type pZMFAddress)
{


long ZJoinIndex;
ZStatus wSt;
ZResult wRes;
ZOp wZIXOp;
ZIndexItem_struct wZI ;
zaddress_type wAddress;

/**
  * get with seekGeneric the ZSIndexFile row position to insert
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
                                        return (wSt); // error is managed at ZMF level
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
                                        return (wSt); // error is managed at ZMF level
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
                if ((wSt=_Base::_insert(_Base::ZDescriptor,wZI.toFileKey(),wRes.ZIdx,wAddress))!=ZS_SUCCESS)     // insert at position returned by seekGeneric
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

}//ZSIndexFile::_addKeyValue
#endif //__COMMENT__

//------------------Add sequence-----------------------------------------


/**
 * @brief ZSIndexFile::_addKeyValue_Prepare prepare to add an index rank. This will be committed or rolled back later using _addKeyValue_Commit or _Rollback (or HardRollback)

@par keyValueExtraction :
   - extracts fields values according their definition in ZKeyFieldList class (ZType, offset, length)
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
ZSIndexFile::_addKeyValue_Prepare(ZRawRecord *pZMFRecord,
                                 ZSIndexItem *&pIndexItem,
                                 zrank_type &pZBATIndex,
                                 const zaddress_type pZMFAddress)
{

ZStatus wSt;
//zaddress_type wZMFAddress;
//zaddress_type wIndexAddress; // not used but necessary for base ZRandomFile class calls
ZSIndexResult wZIR;

ZIFCompare wZIFCompare = ZKeyCompareBinary;


//-----------Comparison routine selection------------------------------------

    if (ZICB->ZKDic->size()==1)           // if only one field
        {
        if (ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison


// get with seekGeneric the ZSIndexFile row position to insert

//  1-insert accordingly (push_front, push , insert)

//  2- shift all ZAM indexes references according pZAMIdx value :
//  NB We do not care about that : only addresses are stored, not the order

    pIndexItem->clear();
    pIndexItem->Operation = ZO_Add;
    pIndexItem->ZMFaddress = pZMFAddress ;  // store address to ZMF Block
//    pKeyValue.State = ZAMInserted ;

/* key data is supposed to have been extracted when coming here */
//    _keyValueExtraction(ZICB->ZKDic,pZMFRecord,pIndexItem->KeyContent);        // create key value content from user record

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
                wSt=_insert2PhasesCommit_Prepare (_Base::ZDescriptor,pIndexItem->toFileKey(),0L,pZBATIndex,wZIR.ZMFAddress);// equivalent to push_front
                if (ZVerbose)
                        _DBGPRINT ("Index Push_Front  (index rank 0L )\n");

                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                pIndexItem->Operation=ZO_Push ;
//                ZJoinIndex=this->size();
                wSt=_Base::_add2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress);// equivalent to push
                if (ZVerbose)
                        _DBGPRINT ("Index Push\n");
                break;
                }
            case (ZS_NOTFOUND):
                {
                pIndexItem->Operation=ZO_Insert ;
                wSt=_Base::_insert2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),wZIR.IndexRank,pZBATIndex,wZIR.ZMFAddress);// insert at position returned by seekGeneric
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
                        _DBGPRINT ("Index Duplicate key insert at rank <%ld>\n", wZIR.IndexRank);

                wSt=_Base::_insert2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),wZIR.IndexRank,pZBATIndex,wZIR.ZMFAddress); // insert at position returned by seekGeneric

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
                                                 ZICB->IndexName.toCChar());
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif

// statistical data is managed outside

/*    pStats.Timer.end();
    pStats.Timer.getDeltaTime();
    pStats = ZDescriptor.ZPMS- wPMS ;  // just give the delta : ZRFPMS is a base for zstatistics
    ZPMSStats += pStats;*/
    return  (wSt) ;

}//_addKeyValue_Prepare

/**
 * @brief ZSIndexFile::_addKeyValue_Prepare prepare to add an index rank. This will be committed or rolled back later using _addKeyValue_Commit or _Rollback (or HardRollback)

@par keyValueExtraction :
   - extracts fields values according their definition in ZKeyFieldList class (ZType, offset, length)
   - formats data : puts data field content into appropriate key format taking into account :
    + signed data (leading byte)
    + reverse byte order if required (if and only if system is little endian compliant)
   - concatenate data in fields order given by their position within CZKeyFieldList array ( rank 0 is first )
     . seek for extracted key value : reject if duplicate collision
     . prepare (ZRandomFile) to insert key appropriately (push, push_front or insert at given rank)



 * @param[in] pZMFRecord user's record content to extract key value from
 * @param[out] pIndexItem a pointer to ZIndexItem struct that contains raw key value to be added (ZSIndexItem::KeyContent)
 * @param[out] pZBATIndex  Return : (Internal) Current index within ZBAT pool for created block. Needed for commit or rollback operation.
 * @param[in] pZMFAddress The ZMF block address to set the index rank with
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSIndexFile::_addRawKeyValue_Prepare(ZRawRecord* pZMFRecord,
                                     ZSIndexItem *&pIndexItem,
                                     zrank_type &pZBATIndex,
                                     const zaddress_type pZMFAddress)
{

ZStatus wSt;
//zaddress_type wZMFAddress;
//zaddress_type wIndexAddress; // not used but necessary for base ZRandomFile class calls
ZSIndexResult wZIR;

ZIFCompare wZIFCompare = ZKeyCompareBinary;


//-----------Comparison routine selection------------------------------------

/*   if (ZICB->ZKDic->size()==1)           // if only one field
        {
        if (ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison
*/

// get with seekGeneric the ZSIndexFile row position to insert

//  1-insert accordingly (push_front, push , insert)

//  2- shift all ZAM indexes references according pZAMIdx value :
//  NB We do not care about that : only addresses are stored, not the order

//    pIndexItem->clear();

/* pIndexItem-> KeyValue contains raw key value */
    pIndexItem->Operation = ZO_Add;
    pIndexItem->ZMFaddress = pZMFAddress ;  // store address to ZMF Block

/* key data is supposed to have been extracted when coming here */

    if (pIndexItem->KeyContent.isEmpty())  /* that must not be the case, but ... */
      {
      if (pZMFRecord->RawContent.isEmpty())/* that must not be the case, but ... */
        {
        ZException.setMessage("ZSIndexFile::_addRawKeyValue_Prepare",
                              ZS_EMPTY,
                              Severity_Error,
                              "Raw Record is empty while trying to add key.");
        return ZS_EMPTY;
        }
      pZMFRecord->getRawKeyContent((unsigned int)pZBATIndex,pIndexItem->KeyContent);
      }

//    wSt= _search (pIndexItem->KeyContent,*this,wZIR,ZMS_MatchIndexSize,wZIFCompare);  // will not use wStats except for special seek mesure on add
    wSt= _search (pIndexItem->KeyContent,*this,wZIR,ZLock_Nolock);  // will not use wStats except for special seek mesure on add

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
    if (ZVerbose)
            {
            _DBGPRINT ("_addRawKeyValue_Prepare : _search return status <%s> rank <%ld>\n", decode_ZStatus(wSt),wZIR.IndexRank);
            }

    switch (wSt)
            {
            case (ZS_OUTBOUNDLOW):
                {
                pIndexItem->Operation=ZO_Push_front ;
//                ZJoinIndex=0;
                wSt=_insert2PhasesCommit_Prepare (_Base::ZDescriptor,pIndexItem->toFileKey(),0L,pZBATIndex,wZIR.ZMFAddress);// equivalent to push_front
                if (ZVerbose)
                        _DBGPRINT ("_addRawKeyValue_Prepare: Index Push_Front  (index rank 0L )\n");

                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                pIndexItem->Operation=ZO_Push ;
//                ZJoinIndex=this->size();
                wSt=_Base::_add2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),pZBATIndex,wZIR.ZMFAddress);// equivalent to push
                if (ZVerbose)
                        _DBGPRINT ("Index Push\n");
                break;
                }
            case (ZS_NOTFOUND):
                {
                pIndexItem->Operation=ZO_Insert ;
                wSt=_Base::_insert2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),wZIR.IndexRank,pZBATIndex,wZIR.ZMFAddress);// insert at position returned by seekGeneric
//                ZJoinIndex=wRes.ZIdx;
                if (ZVerbose)
                        _DBGPRINT ("Index insert at rank <%ld>\n", wZIR.IndexRank);
                break;
                }
            case (ZS_FOUND):
                {
//                if (ZICB->Duplicates==ZST_NODUPLICATES)
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
                        _DBGPRINT ("Index Duplicate key insert at rank <%ld>\n", wZIR.IndexRank);

                wSt=_Base::_insert2PhasesCommit_Prepare(_Base::ZDescriptor,pIndexItem->toFileKey(),wZIR.IndexRank,pZBATIndex,wZIR.ZMFAddress); // insert at position returned by seekGeneric

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
                goto _addRawKeyValuePrepareReturn;     // not necessary for the moment but RFFU
 //               return (wSt); // error is managed at ZMF level
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
_addRawKeyValuePrepareReturn:
    if (wSt!=ZS_SUCCESS)
        if (!ZException.stackIsEmpty())
            ZException.addToLast(" during Index _addKeyValue_Prepare on key <%s> ",
                                                 IndexName.toCChar());
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif

// statistical data is managed outside

/*    pStats.Timer.end();
    pStats.Timer.getDeltaTime();
    pStats = ZDescriptor.ZPMS- wPMS ;  // just give the delta : ZRFPMS is a base for zstatistics
    ZPMSStats += pStats;*/
    return  (wSt) ;

}//_addRawKeyValue_Prepare


ZStatus
ZSIndexFile::_addKeyValue_Commit(ZSIndexItem *pIndexItem, const zrank_type pZBATIndex)
{

ZStatus wSt;
zaddress_type wAddress; // local index address : of no use there

    wSt=_Base::_add2PhasesCommit_Commit(_Base::ZDescriptor,pIndexItem->toFileKey(),pZBATIndex,wAddress);
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
 * @brief ZSIndexFile::_addKeyValue_Rollback used when a problem occurred when
 *    - index file ZBAT has been reserved for index key at pIndexCommit
 *    - it must be released to available pool
 * @param pIndexCommit
 * @return
 */
ZStatus
ZSIndexFile::_addKeyValue_Rollback(const zrank_type pIndexCommit)
{

ZStatus wSt;
    wSt=_Base::_add2PhasesCommit_Rollback (_Base::ZDescriptor,pIndexCommit);
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
 * @brief ZSIndexFile::_addKeyValue_HardRollback routine used when
 *        - index key add operation has been fully committed
 *    and
 *        - a problem occurred on Master file
 *
    So there is need to remove key Index rank at pIndexCommit to restore situation as aligned with Master file.

 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZSIndexFile::_addKeyValue_HardRollback(const zrank_type pIndexCommit)
{

  if (ZVerbose)
            fprintf (stdout,"Index addKeyValue Hard rollback : removing index <%s> rank <%ld>\n",
                     IndexName.toCChar(),
                    pIndexCommit);

  ZStatus wSt =_Base::_remove(_Base::ZDescriptor,pIndexCommit);
  if (wSt!=ZS_SUCCESS)
    {
    ZException.addToLast(" during Index _addKeyValue_HardRollback (hard rollback) on index <%s> rank <%02ld> ",
                         IndexName.toCChar(),
                         pIndexCommit);
    ZException.setLastSeverity(Severity_Severe);
    }
  return  wSt;
} // _addKeyValue_HardRollback

//------------------End Add sequence-----------------------------------------

//------------------Remove sequence-----------------------------------------


/**
 * @brief ZSIndexFile::_removeKeyValue_Prepare  Prepares to remove an index rank corresponding to given pKey (ZDataBuffer)
 *
 * Returns
 * - Index key relative position within ZSIndexFile if key content is found
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
 * @param[out] pIndexRank logical position within ZSIndexFile for the key content
 * @param[out] pZMFAddress Physical address within ZMasterFile for the block corresponding to key value
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZSIndexFile::_removeKeyValue_Prepare(ZDataBuffer & pKey,
                                    ZSIndexItem* &pIndexItem,
                                    long& pIndexRank,
                                    zaddress_type &pZMFAddress)
{

ZStatus wSt;
ZResult wRes;
//zaddress_type wIndexAddress;
ZSIndexResult wZIResult;

//ZIFCompare         wZIFCompare = ZKeyCompareBinary;

//-----------Comparison routine selection------------------------------------


//    wSt=_search(pKey, *this,wZIR,ZMS_MatchIndexSize,wZIFCompare);
    wSt=_search(pKey, *this,wZIResult,ZLock_Nolock);
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

    pIndexItem = new ZSIndexItem ;
    pIndexItem->Operation=ZO_Erase ;
    pIndexItem->KeyContent = pKey;
    pIndexItem->ZMFaddress = 0 ; // dont know yet

    wSt=_Base::_remove_Prepare(_Base::ZDescriptor,wZIResult.IndexRank,wZIResult.ZMFAddress);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif

    return  wSt;
}//_removeKeyValue_Prepare


/**
 * @brief ZSIndexFile::_removeIndexItem_Prepare  ZSIndexItem::KeyContent must content key value to erase in input.
 * @param pIndexItem
 * @param pIndexRank
 * @return
 */
ZStatus
ZSIndexFile::_removeIndexItem_Prepare(ZSIndexItem &pIndexItem,long & pIndexRank)
{

  ZStatus wSt;
  ZResult wRes;
  //zaddress_type wIndexAddress;
  ZSIndexResult wZIResult;

  //ZIFCompare         wZIFCompare = ZKeyCompareBinary;

  //-----------Comparison routine selection------------------------------------


  //    wSt=_search(pKey, *this,wZIR,ZMS_MatchIndexSize,wZIFCompare);
  wSt=_search(pIndexItem.KeyContent, *this,wZIResult,ZLock_Nolock);
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

  wSt=_Base::_remove_Prepare(_Base::ZDescriptor,wZIResult.IndexRank,wZIResult.ZMFAddress);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mtx.unlock();
#endif

  return  wSt;
}//_removeIndexItem_Prepare



ZStatus
ZSIndexFile::_removeKeyValue_Commit(const zrank_type pIndexCommit)
{

zrank_type ZJoinIndex;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif


    wSt=_Base::_remove_Commit(_Base::ZDescriptor,pIndexCommit);

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

}//ZSIndexFile::_removeKeyValue_Prepare
/**
 * @brief ZSIndexFile::_removeKeyValue_Rollback
 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSIndexFile::_removeKeyValue_Rollback(const zrank_type pIndexCommit)
{

zrank_type ZJoinIndex;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif


    wSt=_Base::_remove_Rollback(_Base::ZDescriptor,pIndexCommit);

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
 * @brief ZSIndexFile::_removeKeyValue_HardRollback
 * @param pIndexItem
 * @param pIndexCommit
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSIndexFile::_removeKeyValue_HardRollback(ZSIndexItem* pIndexItem, const zrank_type pIndexCommit)
{


zrank_type ZJoinIndex;
zaddress_type wAddress;  // not used : only for compatibility purpose

ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif

    wSt= _Base::_insert(_Base::ZDescriptor,pIndexItem->toFileKey(),pIndexCommit,wAddress);

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
inline
ZStatus _keyValueExtraction(ZSKeyDictionary* pZKDic, ZRecord &pRecord, ZDataBuffer& pKeyOut)
{
ZStatus wSt;
size_t wKeyOffset = 0;
size_t wSize =0,wRecordOffset=0;
long wRDicRank=0;
ZDataBuffer wFieldUValue;

// at this stage _recomputeSize should have been done and total sizes should be OK.



    if (pZKDic->isEmpty())
            {
            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_BADDIC,
                                     Severity_Severe,
                                     " Index Control Block appears to be malformed. Key dictionary fields list is empty");
            return  ZS_BADDIC;
            }

    if (!pZKDic->Recomputed)
            pZKDic->_reComputeSize();

    pKeyOut.allocateBZero(pZKDic->KeyUniversalSize+1);

    for (long wi=0;wi<pZKDic->size();wi++)
        {
// here put extraction rules. RFFU : Extraction could be complex. To be investigated and implemented

        wRDicRank=pZKDic->Tab[wi].MDicRank;
        pRecord.getUniversalbyRank(wFieldUValue,wRDicRank);
        pKeyOut.changeData(wFieldUValue,wKeyOffset);

        wKeyOffset += pRecord.RDic->Tab[wRDicRank].MDicRank->UniversalSize;
        }//for



return  ZS_SUCCESS;
}//zKeyValueExtraction

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


/**
 * @brief ZSIndexFile::_search Searches for a FIRST or UNIQUE value for a given index. This search may be EXACT or PARTIAL.
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
 * @param[in] pZIF ZSIndexFile object to search on
 *
 * @param[out] pZIR a ZSIndexResult object with ZSIndexFile relative key position (rank within index file) corresponding to key content if found
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
ZSIndexFile::_search( const ZDataBuffer &pKey,
                      ZSIndexFile &pZIF,
                      ZSIndexResult &pZIR,
                      const zlockmask_type pLock)
{
ZStatus     wSt= ZS_NOTFOUND;

pZIR.IndexRank = 0;
ZDataBuffer wIndexRecord;
ZSIndexItem wIndexItem ;

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
                    goto _search_Return;
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
                                                    goto _search_Return;
        wIndexItem.fromFileKey(wIndexRecord);
        wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

        if (wR==0)
                    {
                    wSt=ZS_FOUND ;
                    pZIR.IndexRank = wlow ;
                    goto _search_Return;
                    }
        if (wR<0)
                {return  ZS_OUTBOUNDLOW;}

         pZIR.IndexRank = whigh ;

         pZIF.ZPMSStats.Reads ++;
         if ((wSt=pZIF.zget(wIndexRecord,whigh))!=ZS_SUCCESS)
                                                     goto _search_Return;
         wIndexItem.fromFileKey(wIndexRecord);
         wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

         if (wR==0)
                {
                wSt=ZS_FOUND ;
                pZIR.IndexRank = whigh ;
                goto _search_Return;
                }
         if (wR>0)
                 {return  ZS_OUTBOUNDHIGH;}

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)// ---------------Main loop around pivot----------------------
            {
            pZIR.IndexRank = wpivot ;

            pZIF.ZPMSStats.Reads ++;
            if ((wSt=pZIF.zget(wIndexRecord,wpivot))!=ZS_SUCCESS)
                                                        goto _search_Return;
            wIndexItem.fromFileKey(wIndexRecord);
            wR= wZIFCompare(pKey,wIndexItem.KeyContent,wCompareSize);

            if (wR==0)
                    {
                    wSt=ZS_FOUND ;
                    goto _search_Return;
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

_search_Return:
    pZIR.ZMFAddress = wIndexItem.ZMFaddress ;
/*    if ((wSt=ZS_FOUND)&&(pLock != ZLock_Nolock ))
            {
            return  static_cast<ZMasterFile*>(pZIF.ZMFFather)->zlockByAddress(wIndexItem.ZMFaddress,pLock); // lock corresponding ZMasterFile address with given lock mask
            }*/
    return  (wSt) ;
}// _search

/**
 * @brief ZSIndexFile::_searchAll  Search for ALL index ranks matching EXACTLY a certain key value. Key value cannot be partial.
 * The size of Index key value defines the length to compare with given key value pKey.
 *
 * @par return s
 * - a ZSIndexCollection gathering a collection of ZSIndexResult_struct i. e. :
 *   + ZSIndexFile ranks of indexes matching key value
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
 * @param[in] pZIF ZSIndexFile object to search on
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
ZSIndexFile::_searchAll(const ZDataBuffer        &pKey,     // key content to find out in index
                       ZSIndexFile               &pZIF,     // pointer to ZIndexControlBlock containing index description
                       ZSIndexCollection &pCollection,
                       const ZMatchSize_type    pZMS)
{


ZStatus     wSt=ZS_NOTFOUND;
//long        wIndexRank;
ZSIndexResult wZIR;
ZSIndexItem wIndexItem ;
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
            if (pKey.Size!=pZIF.ZICB->ZKDic->KeyUniversalSize)
                        {return  ZS_INVSIZE ;}//
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
    if (wCompareSize> pZIF.ZICB->ZKDic->KeyUniversalSize)
                wCompareSize=pZIF.ZICB->ZKDic->KeyUniversalSize;

    pCollection.clear();            // Always clearing the collection when zsearch
    pCollection.setStatus(ZS_NOTFOUND) ;
    wSt= ZS_NOTFOUND;

    if (pZIF.getSize()==0)
                    {
                    pCollection.setStatus(ZS_OUTBOUNDHIGH) ;
                    goto _searchAll_Return;
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
            return  ZS_OUTBOUNDLOW ;
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
         return  ZS_OUTBOUNDHIGH ;
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

_searchAll_Return:
//    if (pLock != ZLock_Nolock )
//            pCollection.zlockAll(pLock);    // lock corresponding Collection with given lock mask if necessary
    return  (pCollection.getStatus()) ;

_searchAllError:
//    if (pLock != ZLock_Nolock )
//            pCollection.zunlockAll();    // lock corresponding Collection with given lock mask if necessary

    return  (pCollection.getStatus());
}// _searchAll using ZSIndexFile



/**
 * @brief ZSIndexFile::_searchFirst search ZSIndexFile pZIF for a first match of pKey (first in key order) and return s a ZSIndexResult
 * - ZSIndexFile rank : index file relative position of key found
 * - ZMasterFile corresponding record (block) address
 *
 * @par Accessing collection of selected records
 *
 *  ZSIndexFile::_searchFirst() and ZSIndexFile::_searchNext() works using a search context ( ZIndexSearchContext )that maintains a collection of found records (ZSIndexCollection)
 *
 *  To get access to this collection, you may use the following syntax  <search context name>->Collection-><function to use>
 *
 * You may then use collection to
 * - refine the search with sequential adhoc fields matches
 * - use mass operations (lockAll, unlockAll, removeAll)
 *
 * @note sequential adhoc field rules will apply on ZMasterFile's record content and NOT to ZSIndexFile Index key values.
 * This means that data to compare is RAW data, and NOT data formatted for index sorting.
 * see @ref ZSIndexFile::_addKeyValue_Prepare() for more on internal key data format vs natural record data format.
 *
 * @param[in] pKey a ZDataBuffer with key value to search for.                                  [stored in collection's context]
 * Key value could be partial or exact, depending on ZDataBuffer Size and pZMS value
 * @param[in] pZIF ZSIndexFile object to search on                                               [stored in collection]
 * @param[out] pCollection A pointer to the contextual meta-data from the search created by _searchFirst() routine.
 *
 * It contains
 * - a ZArray of ZSIndexResult objects, enriched with successive _searchNext() calls
 * - Contextual meta-data for the search (an ZSIndexCollectionContext embeded object holds this data)
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on
 *
 * @param[out] pZIR result of the _searchNext operation, if any, as a ZSIndexResult with appropriate Index references (Index file rank ; ZMF record address)
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
ZSIndexFile::_searchFirst(const ZDataBuffer        &pKey,     // key content to find out in index
                         ZSIndexFile               &pZIF,     // pointer to ZIndexControlBlock containing index description
                         ZSIndexCollection         *pCollection,
                         ZSIndexResult             &pZIR,
                         const ZMatchSize_type    pZMS)

{


ZStatus     wSt=ZS_NOTFOUND;
//long        wIndexRank;
ZSIndexItem wIndexItem ;
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
//-----------Comparison routine selection------------------------------------
    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison

//-----------Comparison size definition------------------------------

// Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

     wCompareSize = pKey.Size;
    if (wCompareSize> pZIF.ZICB->ZKDic->KeyUniversalSize)
                wCompareSize=pZIF.ZICB->ZKDic->KeyUniversalSize;



    pCollection=new ZSIndexCollection (&pZIF); // initialize all and create ZSIndexCollection instance
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
            if (pKey.Size!=pZIF.ZICB->ZKDic->KeyUniversalSize)
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
       return  ZS_OUTBOUNDLOW;
        }// if wR<0

     pZIR.IndexRank = whigh ;
     pZIF.ZPMSStats.Reads ++;
     if (pCollection->setStatus(pZIF.zget(wIndexRecord,pZIR.IndexRank))!=ZS_SUCCESS)
                                                         {return  pCollection->getStatus();}
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
                return  ZS_NOTFOUND;
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        pZIR.IndexRank = wpivot ;
        pCollection->Context.CurrentZIFrank = wpivot;
        pZIF.ZPMSStats.Iterations++;
        } // while (whigh-wlow)>2--------Main loop around pivot------------------

    if (pCollection->getStatus()!=ZS_FOUND)
                        { return  pCollection->getStatus();}
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
            return  pCollection->getStatus();
            }
        }// !=ZLock_Nolock
*/
    pCollection->push(pZIR); // enrich Collection with found record.

//    ZException.clearStack();

    return  pCollection->setStatus(ZS_FOUND);

}// _searchFirst
/**
 * @brief ZSIndexFile::_searchNext
 * @param[in-out] pCollection A pointer to the contextual meta-data from the search created by _searchFirst() routine.
 *
 * It contains
 * - a ZArray of ZSIndexResult objects, enriched with successive _searchNext calls
 * - Contextual meta-data for the search.
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on.
 * @param[out] pZIR result of the _searchNext operation, if any, as a ZSIndexResult with appropriate Index references (Index file rank ; ZMF record address)
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 */
ZStatus
ZSIndexFile::_searchNext (ZSIndexResult       &pZIR,
                         ZSIndexCollection*  pCollection)
{

ZSIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//

//-----------Initialization Section---------------------------------

    if (pCollection==nullptr)  // if no Context then errored
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Cannot invoke _searchNext without having Collection / Context : invoke first _searchFirst ");
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
                    return  pCollection->getStatus ();
                    }
                }// !=ZLock_Nolock
*/


            }// if compare
        }// if
        else
        pCollection->setStatus(ZS_OUTBOUNDHIGH);

    return  pCollection->getStatus();

}// _searchNext




/**
 * @brief ZSIndexFile::_searchIntervalAll searches all index key value from ZSIndexFile pZIF corresponding to Interval given by pKeyLow as lowest value for range and pKeyHigh as highest value for range.
 *      pKeyLow and pKeyHigh may be included in range if pExclude is false, or exclude from range if pExclude is true.
 * @param[in] pKeyLow   Lowest key content value to find out in index           [stored in collection's context]
 * @param[in] pKeyHigh  Highest key content value to find out in index          [stored in collection's context]
 * @param[in] pZIF      ZSIndexFile object                                       [stored in collection]
 * @param[out] pCollection ZSIndexCollection object created by the routine. It stores the resulting set of reference to found ZSIndexFile-ZMasterFile records.
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
ZSIndexFile::_searchIntervalAll  (const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                                 const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                                 ZSIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                                 ZSIndexCollection       *pCollection,   // enriched collection of reference (ZSIndexFile rank, ZMasterFile record address)
                                 const bool             pExclude) // Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)
{


ZStatus      wSt;
ZSIndexResult wZIR;

    wSt=_searchIntervalFirst(pKeyLow,
                             pKeyHigh,
                             pZIF,
                             pCollection,
                             wZIR,
                             pExclude);
    if (wSt!=ZS_FOUND)
                { return  wSt;}
    while (wSt==ZS_FOUND)
        {
        wSt=_searchIntervalNext(wZIR,pCollection);
        }
    return  (wSt==ZS_EOF)?ZS_FOUND:wSt;

}// _searchIntervalAll

/**
 * @brief ZSIndexFile::_searchIntervalFirst
 * @param[in] pKeyLow   Lowest key content value to find out in index           [stored in collection's context]
 * @param[in] pKeyHigh  Highest key content value to find out in index          [stored in collection's context]
 * @param[in] pZIF      ZSIndexFile object                                       [stored in collection]
 * @param[out] pCollection ZSIndexCollection object created by the routine.
 * @param[out] pZIR ZSIndexResult giving the result of the first search if any
 * @param[in] pExclude Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)
 *          this option is stored within Collection context using operation code
 * @param[in] pLock Lock mask to apply to found records                         [stored in collection's context]
 * @return
 */
ZStatus
ZSIndexFile::_searchIntervalFirst(const ZDataBuffer      &pKeyLow,  // Lowest key content value to find out in index
                                 const ZDataBuffer      &pKeyHigh, // Highest key content value to find out in index
                                 ZSIndexFile             &pZIF,     // pointer to ZIndexControlBlock containing index description
                                 ZSIndexCollection       *pCollection,   // enriched collection of reference (ZSIndexFile rank, ZMasterFile record address)
                                 ZSIndexResult           &pZIR,
                                 const bool             pExclude)// Exclude KeyLow and KeyHigh value from selection (i. e. > pKeyLow and < pKeyHigh)

{


ZSIndexItem wIndexItem ;
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
//-----------Comparison routine selection------------------------------------
    if (pZIF.ZICB->ZKDic->size()==1)           // if only one field
        {
        if (pZIF.ZICB->ZKDic->Tab[0].ZType & ZType_Char)  // and this field has type Char (array of char)
                            wZIFCompare = ZKeyCompareAlpha; // use string comparison
        } // in all other cases, use binary comparison

//-----------Comparison size definition------------------------------
// Size of comparison is given by pKey.Size : if partial key Size will be shorter than Index key size.

     wCompareSize = pKeyLow.Size;
    if (wCompareSize> pZIF.ZICB->ZKDic->KeyUniversalSize)
                wCompareSize=pZIF.ZICB->ZKDic->KeyUniversalSize;

    pCollection=new ZSIndexCollection (&pZIF);

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
            goto _searchIntervalFirstBackProcess;  // capture all collection before current wZIR.IndexRank if any
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

}// _searchIntervalFirst

/**
 * @brief ZSIndexFile::_searchIntervalNext
 * @param[out]    pZIR
 * @param[in-out] pCollection
 * @return
 */
ZStatus
ZSIndexFile::_searchIntervalNext (ZSIndexResult       &pZIR,
                                 ZSIndexCollection*  pCollection)
{


ZSIndexItem wIndexItem ;
ZDataBuffer wIndexRecord;

//

//-----------Initialization Section---------------------------------

    if (pCollection==nullptr)  // if no Context then errored
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Cannot invoke _searchNext without having Collection / Context : invoke first _searchFirst ");
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
            pZIR.IndexRank = pCollection->ZIFFile->ZDescriptor.getCurrentRank();
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

}// _searchIntervalNext

//--------------------------End Search routines--------------------------------------

/**
 * @brief ZSIndexFile::getKeyIndexFields obtains natural fields values from a given ZIndex key rank content (pKeyValue) according ZKDic fields dictionary definitions
                     and returns a ZDataBuffer in pIndexContent containing concanetated natural fields values up to be used by computer as their origin data (external natural format).

                     @see ZIndexControlBlock::zkeyValueExtraction

 * @param pIndexContent     concanetated natural fields values
 * @param pKeyValue         ZIndex key in ZIndex internal data format
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSIndexFile::getKeyIndexFields(ZDataBuffer &pIndexContent,ZDataBuffer& pKeyValue)
{


ZStatus wSt=ZS_SUCCESS;
ZDataBuffer wFieldValue ;
    pIndexContent.clear();
    if (ZMFFather->ZMCB.MasterDic==nullptr)
      {
      ZException.setMessage("ZSIndexFile::getKeyIndexFields",
                            ZS_BADDIC,
                            Severity_Error,
                            "Master dictionary is missing (nullptr). File appears to be a ZRawMasterFile with no dictionary.\n");
      return ZS_BADDIC;
      }
    for (long wi=0; (ZMFFather->ZMCB.MasterDic->size())&&(wSt==ZS_SUCCESS);wi++)
      {
      wSt=_getFieldValueFromKey(pKeyValue,wFieldValue,wi,*ZICB->ZKDic);
      if (wSt==ZS_SUCCESS)
              pIndexContent.appendData(wFieldValue);
      }
    return  wSt;
}

/**
  * @brief ZSIndexFile::zprintKeyFieldsValues     prints the key fields values in a human readable format from a ZSIndexFile
  *
  * Gets a record from a ZSIndexFile corresponding to its relative position pRank.
  * Prints the index content to pOutput after having converted back all composing field using ZIndex Dictionary.
  *
  * In case of ZType_Class field, then the content of data is dumped using ZDataBuffer::Dump().
  *
  * @param[in] pRank       logical rank of key record within ZSIndexFile
  * @param[in] pHeader  if set to true then key fields description is printed. False means only values are printed.
  * @param[in] pKeyDump if set to true then index key record content is dumped after the list of its fields values. False means only fields values are printed.
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
  */
 ZStatus
 ZSIndexFile::zprintKeyFieldsValues (const zrank_type pRank,bool pHeader,bool pKeyDump,FILE*pOutput)
 {


ZStatus wSt;
ZBlock wBlock;

ZDataBuffer wKeyContent;
//zlock_type wLock = 0L;
zaddress_type wZMFAddress=0;
zaddress_type wIdxAddress;


     wSt=_getByRank(ZDescriptor,wBlock,pRank,wIdxAddress);
     if (wSt!=ZS_SUCCESS)
                 {return  wSt;}

     memmove(&wZMFAddress,wBlock.Content.Data,sizeof(zaddress_type));
     wZMFAddress = reverseByteOrder_Conditional<zaddress_type>(wZMFAddress);
     fprintf (pOutput,
              " Rank <%4ld>  ZMasterFile Address <%lld> \n",
              pRank,
              wZMFAddress);

// we have to skip zaddress_type field in from of key record (ZMF address) to isolate only Key content
     wKeyContent.setData(wBlock.Content.Data + sizeof(zaddress_type),wBlock.Content.Size-sizeof(zaddress_type));

    return    _printKeyFieldsValues(&wKeyContent,ZICB,pHeader,pKeyDump,pOutput);
}

namespace zbs {

ZStatus
_printKeyFieldsValues (ZDataBuffer* wKeyContent,ZSIndexControlBlock* ZICB, bool pHeader,bool pKeyDump,FILE*pOutput)
{
zrank_type wMDicRank;
ZDataBuffer wPrintableField;
// header : index name then fields dictionary definition
     if (pHeader)
     {
     fprintf (pOutput,
              "------------------------------- Index description ------------------------------------\n"
              " Index name %s\n",
              ZICB->IndexName.toCChar());
     for (long wi=0;wi<ZICB->ZKDic->size();wi++)
     {
     wMDicRank=    ZICB->ZKDic->Tab[wi].MDicRank;
     fprintf (pOutput,
              " Field order <%ld> Metadic rank <%ld> name <%s> Data type <%s> \n",
              wi,
              wMDicRank,
              ZICB->MetaDic->Tab[wMDicRank].Name.toCChar(),
              decode_ZType( ZICB->MetaDic->Tab[wMDicRank].ZType));
     }// for
     fprintf (pOutput,
              "---------------------------------------------------------------------------------------\n");
     }// if pHeader

// then fields values



     for (long wi=0;wi<ZICB->ZKDic->size();wi++)
     {
     wMDicRank=    ZICB->ZKDic->Tab[wi].MDicRank;
     fprintf (pOutput,
              " <ZKDic>-<MDic> <%ld>-<%ld> name <%*s> value ",
              wi,
              wMDicRank,
              cst_fieldnamelen,
              ZICB->MetaDic->Tab[wMDicRank].Name.toCChar());

     while (true)
     {
     if (ZICB->MetaDic->Tab[wMDicRank].ZType & ZType_Class)  // if class : simple dump
        {
         ssize_t wKeyOffset = ZICB->ZKDic->fieldKeyOffset(wi);
         unsigned char* wPtr = (unsigned char*)(wKeyContent->DataChar +wKeyOffset);
         size_t wUSize = ZICB->ZKDic->Tab[wi].UniversalSize;
        switch (ZICB->MetaDic->Tab[wMDicRank].ZType)
            {
        case ZType_Utf8FixedString:
            {
            utf8VaryingString wUtf8;  // Yes : considered as varying as we don't know the canonical capacity
            wUtf8.strset((const utf8_t*)wPtr);
            fprintf(pOutput,wUtf8.toCString_Strait());
            break;
            }
        case ZType_Utf16FixedString:
            {
            utf8VaryingString wUtf8;// Yes : considered as varying as we don't know the canonical capacity
            wUtf8.fromUtf16((const utf16_t*)wPtr,nullptr);  // no byte size - endianness is current system endianness
            fprintf(pOutput,wUtf8.toCString_Strait());
            break;
            }
        case ZType_Utf32FixedString:
            {
            utf8VaryingString wUtf8;// Yes : considered as varying as we don't know the canonical capacity
            wUtf8.fromUtf32((const utf32_t*)wPtr, nullptr);// no byte size - endianness is current system endianness
            fprintf(pOutput,wUtf8.toCString_Strait());
            break;
            }
/*        case ZType_FixedCString:
            {
            varyingCString wString;  // Yes : considered as varying as we don't know the canonical capacity
            wString.fromCString_PtrCount((const char*)wPtr,wUSize);
            fprintf(pOutput,wString.toString());
            break;
            }
        case ZType_FixedWString:
            {
            decltype(varyingWString::WDataChar) wWPtr=(typeof(varyingWString::WDataChar))(wPtr);
            varyingWString wString;  // Yes : considered as varying as we don't know the canonical capacity
            wString.fromWStringCount(wWPtr,wUSize);
            for (long ws=0;ws<wString.size();ws++)
                {
                wString.WDataChar[ws]=reverseByteOrder_Conditional<typeof(wString.WDataChar[0])> (wWPtr[ws]);
                }
            fwprintf(pOutput,wString.toWString());
            break;
            }*/

        case ZType_CheckSum:
            {
            checkSum wChk(wKeyContent->Data+wKeyOffset,wUSize);
//            memmove(wChk.content,wKeyContent->DataChar +wKeyOffset,wUSize);
            fprintf (pOutput,wChk.toHexa());
            break;
            }
        case ZType_ZDate:
            {
            ZDate wDate;
            wDate._import(*(uint32_t*)(wKeyContent->DataChar +wKeyOffset));
            fprintf(pOutput,wDate.toFormatted().toCString_Strait());
            break;
            }
        case ZType_ZDateFull:
            {
            ZDateFull wDate;
            wDate._import(*(uint64_t*)(wKeyContent->DataChar +wKeyOffset));
            fprintf(pOutput,wDate.toFormatted().toCString_Strait());
            }
        case ZType_VaryingCString:  // this cannot be for an index field
        case ZType_VaryingWString:  // this cannot be for an index field
        case ZType_StdString: // this cannot be for an index field
        default:
            {
             wPrintableField.fromString("**Unknown/invalid Key Data Type**");
             break;
            }
            }// switch ZICB->MetaDic->Tab[wMDicRank].ZType
       break;
      }//(ZICB->MetaDic->Tab[wMDicRank].ZType & ZType_Class)
     if (ZICB->MetaDic->Tab[wMDicRank].ZType & ZType_Atomic)
     {
         _printAtomicValueFromKey(*wKeyContent,wPrintableField,wi,*ZICB->ZKDic);
         fprintf (pOutput,
                  "<%s> ",
                  wPrintableField.DataChar);
         break;
     }

     if (ZICB->MetaDic->Tab[wMDicRank].ZType & ZType_Array)  // char arrays and wchar arrays are addressed in _printArrayValueFromKey
        {
         _printArrayValueFromKey(*wKeyContent,wPrintableField,wi,*ZICB->ZKDic);

         break;
        }
     wPrintableField.fromString("**Not printable value**");
     break;
     }// while true

     fprintf (pOutput,
              "%s ",
              wPrintableField.DataChar);
     fprintf (pOutput,
              "\n");
     }// for

     if (pKeyDump)
                 wKeyContent->Dump();

     return  ZS_SUCCESS;
 }//_printKeyFieldsValues

}// namespace zbs



#ifdef __COMMENT__
//------------ZIndexTable-------------------


long ZIndexTable::pop (void)
{

  if (size()<0)
  {
    return  -1;// Beware return  is multiple instructions in debug mode
  }
  last()->~ZSIndexFile();
  if (last()!=nullptr)
  {
    delete last();
    last()=nullptr;
  }
  return  _Base::pop();
} // pop

long ZIndexTable::erase (long pRank)
{
  if (pRank>lastIdx())
  {
    return  -1;
  }
  Tab[pRank]->~ZSIndexFile();

  if (Tab[pRank]!=nullptr)
    delete Tab[pRank];   // appearingly it puts the mess
  Tab[pRank]=nullptr; // just in case for avoiding double free
  return  _Base::erase(pRank);
} // erase

void ZIndexTable::clear(void)
{
  while (size()>0)
    pop();
  _Base::clear();
}// clear



long ZIndexTable::searchIndexByName (const char* pName)
{
  for (long wi =0;wi<size();wi++)
  {
    if (Tab[wi]->IndexName==pName)
      return  wi;
  }

  return -1;
}//zsearchIndexByName

long ZIndexTable::searchCaseIndexByName (const char* pName)
{
  for (long wi =0;wi<size();wi++)
  {
    if (Tab[wi]->IndexName.isEqualCase((const utf8_t*)pName))
    {
      return  wi;
    }
  }

  return -1;
}//zsearchIndexByName

long ZIndexTable::searchIndexByName (const utf8String& pName)
{
  for (long wi =0;wi<size();wi++)
  {
    if (Tab[wi]->IndexName==pName)
      return  wi;
  }

  return -1;
}//zsearchIndexByName

long ZIndexTable::searchCaseIndexByName (const utf8String& pName)
{
  for (long wi =0;wi<size();wi++)
  {
    if (Tab[wi]->IndexName.isEqualCase(pName))
    {
      return  wi;
    }
  }

  return -1;
}//zsearchIndexByName

//------------------End ZIndexObjectTable ---------------------
#endif // __COMMENT__
//----------------XML---------------------------------------

/*
  <indextable>
    <index>
      <indexcontrolblock> <!-- no dictionary in index control block -->
          <indexname> </indexname>
          <keyuniversalsize> </keyuniversalsize>
          <duplicates> </duplicates>
      </indexcontrolblock>
      <file> <!-- from ZIndexTable >
         <zfiledescriptor>
            <URIContent>/home/gerard/testindexedpictures.zmf</URIContent>
            <URIHeader>/home/gerard/testindexedpictures.zrh</URIHeader> <!-- not modifiable generated by ZRandomFile-->
            <URIDirectoryPath>/home/gerard/</URIDirectoryPath> <!-- not modifiable generated by ZRandomFile-->
            <ZHeaderControlBlock> <!-- not modifiable -->
                <FileType>ZFT_ZSMasterFile</FileType>  <!-- do not modify : could cause data loss  see documentation-->
                <ZRFVersion>2000</ZRFVersion> <!-- not modifiable -->
                <OffsetFCB>4698</OffsetFCB> <!-- not modifiable -->
                <OffsetReserved>64</OffsetReserved> <!-- not modifiable -->
                <SizeReserved>4634</SizeReserved> <!-- not modifiable -->
            </ZHeaderControlBlock>
         <zfilecontrolblock>
            <AllocatedBlocks>15</AllocatedBlocks> <!-- not modifiable -->
            <BlockExtentQuota>5</BlockExtentQuota>ZSKeyDictionary
            <InitialSize>100000</InitialSize>
            <BlockTargetSize>1</BlockTargetSize>
            <HighwaterMarking>false</HighwaterMarking>
            <GrabFreeSpace>true</GrabFreeSpace>
         </zfilecontrolblock>
        </zfiledescriptor>
      </file>
    </index>
  </indextable>
*/

utf8String ZSIndexFile::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel+1;
  utf8String wReturn;
  wReturn = fmtXMLnode("index",pLevel);

  wReturn += ZSIndexControlBlock::toXml(wLevel,pComment);

  wReturn += ZRandomFile::toXml(wLevel,pComment);

  wReturn += fmtXMLendnode("index",pLevel);
  return wReturn;
} // toXml



utf8String ZIndexTable::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel+1;
  utf8String wReturn;
  wReturn = fmtXMLnode("indextable",pLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," no dictionary in index control block");

  for (long wi=0;wi<count();wi++)
  {
    Tab[wi]->toXml(wLevel+1,pComment);
  }
  wReturn += fmtXMLendnode("indextable",pLevel);
  return wReturn;
} // toXml

ZStatus ZIndexTable::fromXml(zxmlNode* pIndexNode, ZaiErrors* pErrorlog)
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

#endif //ZSIndexFile_CPP
