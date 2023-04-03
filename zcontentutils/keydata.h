#ifndef KEYDATA_H
#define KEYDATA_H

#include <stddef.h>
#include <ztoolset/utfvaryingstring.h>


class KeyData {
public:
  KeyData();
  KeyData(const KeyData& pIn) {_copyFrom(pIn);}
  KeyData& _copyFrom(const KeyData& pIn) ;
  KeyData& operator = (const KeyData& pIn) { return _copyFrom(pIn);}
  size_t  KeySize=0;
  size_t  Allocated=0;
  size_t  AllocatedSize=0;
  size_t  ExtentQuota=0;
  size_t  ExtentSize=0;
  bool    Duplicates=false;
  bool    GrabFreeSpace=false;
  bool    HighwaterMarking=false;
  utf8VaryingString IndexRootName;
  utf8VaryingString IndexName;
};

#endif // KEYDATA_H
