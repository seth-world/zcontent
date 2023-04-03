#ifndef MASTERFILEVALUES_H
#define MASTERFILEVALUES_H

#include <stdint.h>

#include <ztoolset/uristring.h>

#include "keydata.h"
#include "zchangerecord.h"

class MasterFileValues {
public:
  MasterFileValues();
  ~MasterFileValues() {
    if (KeyValues!=nullptr)
      delete KeyValues;
    if (DeletedKeyValues!=nullptr)
      delete DeletedKeyValues;
  }
  MasterFileValues(MasterFileValues& pIn) {_copyFrom(pIn);}
  MasterFileValues& _copyFrom(MasterFileValues& pIn) ;

  MasterFileValues& operator=(MasterFileValues& pIn) {return _copyFrom(pIn);}

  uriString TargetDirectory;
  uriString IndexDirectory;
  utf8VaryingString RootName;
  size_t MeanRecordSize=0;
  size_t AllocatedBlocks = 0, AllocatedSize = 0;
  size_t ExtentQuota = 0, ExtentQuotaSize = 0;
  size_t InitialBlocks = 0, InitialSize = 0;
  bool HighWaterMarking = false ;
  bool GrabFreeSpace = true;
  bool Journaling = false;

  zbs::ZArray<KeyData>* KeyValues=nullptr;
  zbs::ZArray<KeyData>* DeletedKeyValues=nullptr;

  zbs::ZArray<ZChangeRecord> ChangeLog;
};


#endif // MASTERFILEVALUES_H
