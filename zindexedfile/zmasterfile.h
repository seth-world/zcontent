#ifndef ZMASTERFILE_H
#define ZMASTERFILE_H

#include "zrawmasterfile.h"
#include "zmasterfile_utilities.h"

#include <ztoolset/zlimit.h>

#include <cstdarg>

class URFField;
namespace zbs {

class ZMasterFile : public ZRawMasterFile
{
public:
  ZMasterFile();
  ~ZMasterFile();

  ZStatus zopen       (const uriString& pURI, zmode_type pMode=ZRF_All); // shadows ZRawMasterFile::zopen
  ZStatus zopen       (zmode_type pMode=ZRF_All) {return (zopen(getURIContent(),pMode));}

  ZStatus loadDictionary ();
  ZStatus setDictionary(const ZMFDictionary& pDictionary);
  ZStatus setExternDictionary(const uriString& pDicPath);

  utf8VaryingString getDictionaryName() ;

//  int _getAvailableBckNum(const uriString& pBackupPath,const utf8VaryingString &pBckExt);

//  ZStatus backupAll (const uriString &pBackupPath=uriString(), const utf8VaryingString &pBckExt="bck") ;

  /**
   * @brief rebuildIndex  For index key of rank pIndexRank:<br>
   * Clears index file. Re-create sequentially from master file content, record per record, index key records using extractKeyValues() routine.
   * @param pIndexRank rank of index to rebuild
   * @param pRank       Optional (omitted if nullptr) : if mentionned, currently processed ZMF rank is updated there.
   * @return a ZStatus : ZS_SUCCESS if OK, otherwise the faulty status is returned and ZException is set with appropriate message data.
   */
  ZStatus rebuildIndex(long pIndexRank, long *pRank=nullptr) ;
  ZStatus extractKeyValues(const ZDataBuffer& pRecord, ZDataBuffer& pKeyContent, long pIndexRank);
};

}// namespace zbs

URFField getURFField(const unsigned char* &pPtrIn);

#endif // ZMASTERFILE_H
