#ifndef ZRAWMASTERFILEUTILS_H
#define ZRAWMASTERFILEUTILS_H

#include <zindexedfile/zrawmasterfile.h>
#include <zindexedfile/zrawindexfile.h>

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

template <class _Tp>
ZStatus
zrebuildRawIndex(ZRawMasterFile& pMasterFile,const long pIndexRank,bool pStat=false, FILE*pOutput=stdout)
{
  ZStatus         wSt = ZS_SUCCESS;
  ZDataBuffer     wRecord;
  zrank_type      wZMFRank = 0;
  zrank_type      wIndexRank=0;
  zaddress_type   wZMFAddress=0;
  ZIndexItem      wIndexItem;

  ZRawIndexFile&  wIF = *pMasterFile.IndexTable[pIndexRank];
 // long            wIndexRank;

  _Tp             wClass;

  long            wIndexCount=0;

  if (((pMasterFile.getOpenMode()==ZRF_NotOpen )||!(pMasterFile.getOpenMode() & ZRF_Exclusive)||((pMasterFile).getOpenMode() & ZRF_All )!=ZRF_All))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Error,
        "Request to rebuild index for master file <%s> while open mode is invalid <%s>. Must be (ZRF_Exclusive | ZRF_All)",
        pMasterFile.getURIContent().toCChar(),
        decode_ZRFMode(pMasterFile.getOpenMode()));
    return  ZS_MODEINVALID;
  }

  if (pStat)
    wIF.ZPMSStats.init();
  fprintf (pOutput,
      "______________Rebuilding Index <%s>_______________\n"
      " File is %s \n",
      wIF.IndexName.toCChar(),
      wIF.getURIContent().toCChar());

  zsize_type wFatherSize = pMasterFile.getSize();
  zsize_type wSize = wIF.IndexRecordSize() * wFatherSize ;
  if (ZVerbose)
  {
    fprintf (pOutput,
        " Index file size is computed to be %ld\n",
        wSize);

    fprintf (pOutput,"Clearing index file\n");
  }

  wSt=wIF.zclearFile(wSize);  // clearing file with a free block space equals to the whole index

  if (wIF.getRawMasterFile()->isEmpty())
  {
    fprintf(pOutput,
        " ------------No record in ZMasterFile <%s> : no index to rebuild..........\n",
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
    wIF.ZPMSStats.reportFull(pOutput);
  }
  fprintf (pOutput,"\n   %ld index keys added to index \n", wIndexCount);

  if (wSt!=ZS_SUCCESS)
  {
    fprintf (pOutput," ----- index rebuild ended with error --------\n");
    ZException.printUserMessage();
    return  wSt;
  }
  fprintf (pOutput," ---------Successfull end rebuilding process for Index <%s>------------\n",
      wIF.IndexName.toCChar());
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
template<class _Tp>
ZStatus
zreorgRawMasterFile (ZRawMasterFile& pRawMF, bool pDump,FILE *pOutput)
{
  ZStatus wSt;
  long wi = 0;
  //ZExceptionMin ZException_sv;
  bool wasOpen=false;
  bool wgrabFreeSpaceSet = false;

  zmode_type wMode = ZRF_Nothing ;
  if (pRawMF.isOpen())
  {
    wMode=pRawMF.getMode();
    pRawMF.zclose();
    wasOpen=true;
  }
  if ((wSt=pRawMF.zopen(pRawMF.getURIContent(),ZRF_Exclusive|ZRF_Write))!=ZS_SUCCESS)
  {  return  wSt;}


  if (!pRawMF.getFCB()->GrabFreeSpace)        // activate grabFreeSpace if it has been set on
  {
    pRawMF.getFCB()->GrabFreeSpace=true;
    wgrabFreeSpaceSet = true;
  }

  pRawMF.zstartPMSMonitoring();

  wSt = pRawMF._reorgFileInternals(pDump,pOutput);

  while (wi < pRawMF.IndexTable.size()) {
    //      wSt=IndexTable[wi]->zrebuildRawIndex(false,pOutput); // hard rollback update on each already committed index
    wSt=zrebuildRawIndex<_Tp>(pRawMF.IndexTable[wi],false,pOutput); // hard rollback update on each already committed index

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
  fprintf (pOutput,
      " ----------End of ZRawMasterFile reorganization process-------------\n");

  pRawMF.zreportPMSMonitoring(pOutput);

  if (ZException.getLastStatus()!=ZS_SUCCESS)
  {
    //             ZException=ZException_sv;
    goto error_zreorgZMFFile;
  }

//    ZException.getLastStatus() = ZS_SUCCESS;

end_zreorgZMFFile:

  if (wgrabFreeSpaceSet)        // restore grabFreeSpace if it was off and has been set on
  {
    pRawMF.getFCB()->GrabFreeSpace=false;
  }
  pRawMF.zclose ();
  if (wasOpen)
    pRawMF.zopen(pRawMF.getURIContent(),wMode);

  return  ZException.getLastStatus();

error_zreorgZMFFile:
  ZException.printUserMessage(pOutput);
  goto end_zreorgZMFFile;

}// ZRawMasterFile::zreorgFile



#endif // ZRAWMASTERFILEUTILS_H
