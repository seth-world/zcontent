#include "keydata.h"


KeyData::KeyData()
{

}
KeyData&
KeyData::_copyFrom(const KeyData& pIn) {
  KeySize=pIn.KeySize;
  Allocated=pIn.Allocated;
  AllocatedSize=pIn.AllocatedSize;
  ExtentQuota=pIn.ExtentQuota;
  ExtentSize=pIn.ExtentSize;
  Duplicates = pIn.Duplicates;
  GrabFreeSpace = pIn.GrabFreeSpace;
  Forced = pIn.Forced;
  HighwaterMarking = pIn.HighwaterMarking;
  IndexRootName.clear();
  if (!pIn.IndexRootName.isEmpty())
    IndexRootName=pIn.IndexRootName;
  IndexName.clear();
  if (!pIn.IndexName.isEmpty())
    IndexName=pIn.IndexName;
  return *this;
}
void
KeyData::clear() {
  KeySize=0;
  Allocated=0;
  AllocatedSize=0;
  ExtentQuota=0;
  ExtentSize=0;
  Duplicates =ZST_Nothing;
  GrabFreeSpace = false;
  Forced = false;
  HighwaterMarking = false;
  IndexRootName.clear();
  IndexName.clear();
  return ;
}
