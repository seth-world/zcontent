#include "masterfilevalues.h"

MasterFileValues::MasterFileValues()
{
}
MasterFileValues& MasterFileValues::_copyFrom(MasterFileValues& pIn) {
  TargetDirectory=pIn.TargetDirectory;
  IndexDirectory=pIn.IndexDirectory;
  RootName=pIn.RootName;
  MeanRecordSize=pIn.MeanRecordSize;
  AllocatedBlocks=pIn.AllocatedBlocks;
  AllocatedSize=pIn.AllocatedSize;
  ExtentQuota=pIn.ExtentQuota;
  ExtentQuotaSize=pIn.ExtentQuotaSize;
  InitialBlocks=pIn.InitialBlocks;
  InitialSize=pIn.InitialSize;

  GrabFreeSpace=pIn.GrabFreeSpace;
  HighWaterMarking=pIn.HighWaterMarking;
  Journaling=pIn.Journaling;

  if (pIn.KeyValues==nullptr) {
    if (KeyValues!=nullptr) {
      delete KeyValues;
      KeyValues=nullptr;
    }
    return *this;
  }// if (pIn.KeyValues==nullptr)
  if (KeyValues==nullptr)
    KeyValues = new zbs::ZArray<KeyData>;
  else
    KeyValues->clear();
  for (long wi=0;wi < pIn.KeyValues->count();wi++) {
    KeyValues->push(pIn.KeyValues->Tab[wi]);
  } // for
  return *this;
} // _copyFrom
