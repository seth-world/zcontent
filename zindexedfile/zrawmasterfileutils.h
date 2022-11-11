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
zrebuildRawIndex(ZRawIndexFile& pIF,bool pStat=false, FILE*pOutput=stdout)
{
  ZStatus         wSt = ZS_SUCCESS;
  //ZRawRecord *wRecord = static_cast<ZRawMasterFile *>(ZMFFather)->getRawRecord();
//  ZRawRecord *wRecord =pIF.getRawMasterFile()->generateRawRecord();
  ZDataBuffer     wRecord;
  zrank_type      wZMFRank = 0;
  zaddress_type   wZMFAddress=0;
  long            wIndexRank;

  long            wIndexCount=0;

  if ((((ZRandomFile&)pIF).getOpenMode()==ZRF_NotOpen )||!(((ZRandomFile&)pIF).getOpenMode() & ZRF_Exclusive)||((((ZRandomFile&)pIF).getOpenMode() & ZRF_All )!=ZRF_All))
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Error,
        "Request to rebuild file <%s> while open mode is invalid <%s>. Must be (ZRF_Exclusive | ZRF_All)",
        pIF.getURIContent().toCChar(),
        decode_ZRFMode(pIF.getOpenMode()));
    return  ZS_MODEINVALID;
  }

  if (pStat)
    pIF.ZPMSStats.init();
  fprintf (pOutput,
      "______________Rebuilding Index <%s>_______________\n"
      " File is %s \n",
      pIF.IndexName.toCChar(),
      pIF.getURIContent().toCChar());

  zsize_type wFatherSize = pIF.getRawMasterFile()->getSize();
  zsize_type wSize = pIF.IndexRecordSize() * wFatherSize ;
  if (ZVerbose)
  {
    fprintf (pOutput,
        " Index file size is computed to be %ld\n",
        wSize);

    fprintf (pOutput,"Clearing index file\n");
  }

  wSt=pIF.zclearFile(wSize);  // clearing file with a free block space equals to the whole index

  if (pIF.getRawMasterFile()->isEmpty())
  {
    fprintf(pOutput,
        " ------------No record in ZMasterFile <%s> : no index to rebuild..........\n",
        pIF.getRawMasterFile()->getURIContent().toString());


    return  ZS_SUCCESS;
  }


  wSt=pIF.getRawMasterFile()->zgetWAddress(wRecord,wZMFRank,wZMFAddress);
  for (zsize_type wi=0;(wSt==ZS_SUCCESS)&&(wi <wFatherSize);wi++ )
  {
    //           wZMFRank = wFather->zgetCurrentRank();
    //           wZMFAddress=wFather->zgetCurrentLogicalPosition();
    wIndexCount++;
    wSt=pIF._addRawKeyValue<_Tp>(wRecord,wIndexRank,wZMFAddress);
    if (wSt!= ZS_SUCCESS)
      break;
    wSt=pIF.getRawMasterFile()->zgetNextWAddress(wRecord,wZMFRank,wZMFAddress) ;
  } // for
  if ((wSt==ZS_EOF)||(wSt==ZS_OUTBOUNDHIGH))
    wSt=ZS_SUCCESS;

  if (pStat)
  {
    pIF.ZPMSStats.end();
    pIF.ZPMSStats.reportFull(pOutput);
  }
  fprintf (pOutput,"\n   %ld index keys added to index \n", wIndexCount);

  if (wSt!=ZS_SUCCESS)
  {
    fprintf (pOutput," ----- index rebuild ended with error --------\n");
    ZException.printUserMessage();
    return  wSt;
  }
  fprintf (pOutput," ---------Successfull end rebuilding process for Index <%s>------------\n",
      pIF.IndexName.toCChar());
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
