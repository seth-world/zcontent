#ifndef ZRAWMASTERFILEUTILS_H
#define ZRAWMASTERFILEUTILS_H

#include <zindexedfile/zrawmasterfile.h>
#include <zindexedfile/zrawindexfile.h>

ZStatus zrepairIndexes(const char *pZMFPath,
                       bool pRepair ,
                       bool pRebuildAll ,
                       ZaiErrors *pErrorLog);

ZStatus zdowngradeZMFtoZRF(const uriString &pZMFPath, ZaiErrors *pErrorLog);

ZStatus zupgradeZRFtoZMF(const uriString &pZRFPath,
                         const uriString &pDictionaryFile,
                         ZaiErrors *pErrorLog);

/* returns index root name (without index file extension) */
utf8VaryingString generateIndexRootName(const utf8VaryingString &pMasterRootName,
                                        const utf8VaryingString &pIndexName);
/* return index base name : i. e. <index root name>.<index file extension> */
utf8VaryingString generateIndexBaseName(const utf8VaryingString &pMasterRootName,
                                        const utf8VaryingString &pIndexName);

ZStatus
generateIndexURI(uriString &pIndexFileUri,
                  const uriString &pMasterFileUri,
                  const uriString &pDirectory,
                  const utf8VaryingString& pIndexName);

const char *decode_ZCOP (uint16_t pZCOP);
/**
 * @brief ZIndexFile::zrebuildIndex rebuilds the current index
 *
 * NB: this routine does not open nor close the files
 *
 * - Clears the index file using ZRandomFile::Clear()
 * - Re-create each index rank from father's records content
 * - Optionally displays statistical information when pStat is set to true and pOutput is assigned to a valid FILE* value
 *
 * @param[in] pStat a flag mentionning if statistics will be produced (true) or not (false) during index rebuild
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

template <class _RecordClass>
ZStatus
//zrebuildRawIndex(ZRawMasterFile& pMasterFile,const long pIndexRank,bool pStat=false, FILE*pOutput=stdout)
zrebuildRawIndex(ZRawMasterFile& pMasterFile,const long pIndexRank,bool pStat,ZaiErrors* pErrorLog)
{
  ZStatus         wSt = ZS_SUCCESS;
  ZDataBuffer     wRecord;
  zrank_type      wZMFRank = 0;
  zrank_type      wIndexRank=0;
  zaddress_type   wZMFAddress=0;
  ZIndexItem      wIndexItem;

  ZRawIndexFile&  wIF = *pMasterFile.IndexTable[pIndexRank];
 // long            wIndexRank;

  _RecordClass    wClass;

  long            wIndexCount=0;

  if (((pMasterFile.getOpenMode()==ZRF_NotOpen )||!(pMasterFile.getOpenMode() & ZRF_Exclusive)||((pMasterFile).getOpenMode() & ZRF_All )!=ZRF_All))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Error,
        "Request to rebuild index for master file <%s> while open mode is invalid <%s>. Must be (ZRF_Exclusive | ZRF_All)",
        pMasterFile.getURIContent().toCChar(),
        decode_ZRFMode(pMasterFile.getOpenMode()));
      if (pErrorLog!=nullptr)
        pErrorLog->errorLog("Request to rebuild index for master file <%s> while open mode is invalid <%s>. Must be (ZRF_Exclusive | ZRF_All)",
                              pMasterFile.getURIContent().toCChar(),
                              decode_ZRFMode(pMasterFile.getOpenMode()));
    return  ZS_MODEINVALID;
  }

  if (pStat)
    wIF.ZPMSStats.init();

  pErrorLog->textLog("______________Rebuilding Index <%s>_______________\n"
                     " File is %s \n",
                     wIF.IndexName.toCChar(),
                     wIF.getURIContent().toCChar());

  zsize_type wFatherSize = pMasterFile.getSize();
  zsize_type wSize = wIF.IndexRecordSize() * wFatherSize ;

  pErrorLog->textLog(" Index file size is computed to be %ld", wSize);
  pErrorLog->textLog(" Clearing index file");


  wSt=wIF.zclearFile(wSize,pMasterFile.getFCB()->HighwaterMarking,pErrorLog);  // clearing file with a free block space equals to the whole index

  if (wIF.getRawMasterFile()->isEmpty())
  {
    pErrorLog->infoLog(" ------------No record in ZMasterFile <%s> : no index to rebuild..........\n",
                        pMasterFile.getURIContent().toString());
    return  ZS_SUCCESS;
  }


  wSt=pMasterFile.zgetWAddress(wRecord,wZMFRank,wZMFAddress);

  for (zsize_type wi=0;(wSt==ZS_SUCCESS)&&(wi < wFatherSize);wi++ )
  {
    wClass.fromRecord(wRecord);
    //           wZMFRank = wFather->zgetCurrentRank();
    //           wZMFAddress=wFather->zgetCurrentLogicalPosition();
    wIndexCount++;
    wSt=wIF._addRawKeyValue_Prepare(wIndexItem,wIndexRank,wClass.getKey(pIndexRank),wZMFAddress);
//    wSt=pIF._addRawKeyValue(wRecord,wIndexRank,wZMFAddress);
    if (wSt!= ZS_SUCCESS)
      break;
    wSt=pMasterFile.zgetNextWAddress(wRecord,wZMFRank,wZMFAddress) ;
  } // for
  if ((wSt==ZS_EOF)||(wSt==ZS_OUTBOUNDHIGH))
    wSt=ZS_SUCCESS;

  if (pStat)
  {
    wIF.ZPMSStats.end();
    wIF.ZPMSStats.reportFull(pErrorLog);
  }
  pErrorLog->infoLog("   %ld index key values added to index \n", wIndexCount);

  if (wSt!=ZS_SUCCESS) {
    pErrorLog->logZExceptionLast(" ----- index rebuild ended with error --------");
    return  wSt;
  }
  pErrorLog->infoLog (" ---------Successfull rebuilding process for Index <%s>------------\n", wIF.IndexName.toCChar());
  return  wSt;

}//zrebuildIndex



/**
 * @brief ZRawMasterFile::zreorgFile reorder the base file for ZRawMasterFile and rebuild all indexes
 *
 * superseeds ZRandomFile::zreorgFile as an method overload.
 *
 * As ZRandomFile::zreorgFile() changes physical block addresses in reorganizing file structure,
 * it is necessary to rebuild any defined index after having done a zreorgFile process.
 *
 * ZRawMasterFile::zreorgFile() does the whole stuff :
 * - base file reorganization
 * - rebuild for all defined indexes for the ZRawMasterFile @see ZIndexFile::zrebuildIndex()
 *
 * @param[in] pDump this boolean sets (true) the option of having surfaceScan during the progression of reorganization. Omitted by default (false)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */


template <class _RecordClass>
ZStatus
zreorgRawMasterFile (const uriString& pURIRawMF,
                    long pRequestedFreeBlocks,
                    //  bool pDump,
                    ZaiErrors* pErrorLog)
{
    ZStatus wSt;
    ZRawMasterFile pRawMF;
    long wi = 0;

    bool wgrabFreeSpaceSet = false;

    if ((wSt=pRawMF.zopen(pURIRawMF,ZRF_All))!=ZS_SUCCESS)
    {  return  wSt;}


    if (!pRawMF.getFCB()->GrabFreeSpace)        // activate grabFreeSpace if it has been set on
    {
        pRawMF.getFCB()->GrabFreeSpace=true;
        wgrabFreeSpaceSet = true;
    }

    pRawMF.zstartPMSMonitoring();

    wSt = pRawMF._reorgFileInternals(pRequestedFreeBlocks,pErrorLog);

    while (wi < pRawMF.IndexTable.size()) {
        wSt=zrebuildRawIndex<_RecordClass>(pRawMF.IndexTable[wi],wi,false,pErrorLog); // hard rollback update on each already committed index

        if (wSt!=ZS_SUCCESS)
        {
            //               ZException_sv = ZException; // in case of error : store the exception but continue rolling back other indexes
            ZException.addToLast(" during Index rebuild on index <%s> number <%02ld> ",
                                 pRawMF.IndexTable[wi]->IndexName.toCChar(),
                                 wi);
        }

        wi++;
    }

    pRawMF.zendPMSMonitoring ();
    pErrorLog->textLog(
        " ----------End of ZRawMasterFile reorganization process-------------\n");

    pRawMF.zreportPMSMonitoring(pErrorLog);

    if (ZException.getLastStatus()!=ZS_SUCCESS)
    {
        //             ZException=ZException_sv;
        goto error_zreorgRawMasterFile;
    }

//    ZException.getLastStatus() = ZS_SUCCESS;

end_zreorgRawMasterFile:

    if (wgrabFreeSpaceSet)        // restore grabFreeSpace if it was off and has been set on
    {
        pRawMF.getFCB()->GrabFreeSpace=false;
    }
    pRawMF.zclose ();
    return  wSt;

error_zreorgRawMasterFile:
    pErrorLog->logZExceptionLast("zreorgRawMasterFile");
    goto end_zreorgRawMasterFile;

}

ZStatus
zreorgMasterFile (const uriString& pURIRawMF,
                 long pRequestedFreeBlocks,
                 ZaiErrors* pErrorLog);


#endif // ZRAWMASTERFILEUTILS_H
