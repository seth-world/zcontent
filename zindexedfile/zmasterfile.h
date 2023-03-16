#ifndef ZMASTERFILE_H
#define ZMASTERFILE_H

#include "zrawmasterfile.h"
#include "zmasterfile_utilities.h"

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

  ZStatus rebuildIndex(long pIndexRank) ;
  ZStatus extractKeyValues(const ZDataBuffer& pRecord, ZDataBuffer& pKeyContent, long pIndexRank);
};

}// namespace zbs

URFField getURFField(const unsigned char* &pPtrIn);

#endif // ZMASTERFILE_H
