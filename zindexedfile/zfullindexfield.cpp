#include "zfullindexfield.h"
#include <zindexedfile/zsindexfile.h>
#include <zindexedfile/zkeydictionary.h>
#include <zindexedfile/zsmasterfile.h>
#include <zindexedfile/zmfdictionary.h>

using namespace zbs;

ZFullIndexField::ZFullIndexField()
{

}

ZFullIndexField& ZFullIndexField::set(ZSIndexFile* pZIF,long pKeyFieldRank)
{
  ZSIndexField::_copyFrom(pZIF->KeyDic->Tab[pKeyFieldRank]);

  ZSMasterFile* wFather = pZIF->getMasterFile();

  ZFieldDescription::_copyFrom(wFather->MasterDic->Tab[MDicRank]);
  return *this;
}
