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
  HighwaterMarking = pIn.HighwaterMarking;
  IndexRootName.clear();
  if (!pIn.IndexRootName.isEmpty())
    IndexRootName=pIn.IndexRootName;
  IndexName.clear();
  if (!pIn.IndexName.isEmpty())
    IndexName=pIn.IndexName;
  return *this;
}
