#include "zfullindexfield.h"
#include <zindexedfile/zindexfile.h>
#include <zindexedfile/zkeydictionary.h>
#include <zindexedfile/zmasterfile.h>
#include <zindexedfile/zmfdictionary.h>

using namespace zbs;

ZFullIndexField::ZFullIndexField()
{

}

ZFullIndexField& ZFullIndexField::set(ZIndexFile* pZIF,long pKeyFieldRank)
{
  ZIndexField::_copyFrom(pZIF->IdxKeyDic->Tab[pKeyFieldRank]);

  ZMasterFile* wFather = pZIF->getMasterFile();

  ZFieldDescription::_copyFrom(wFather->MasterDic->Tab[MDicRank]);
  return *this;
}
