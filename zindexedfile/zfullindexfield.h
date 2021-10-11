#ifndef ZFULLINDEXFIELD_H
#define ZFULLINDEXFIELD_H

#include <zindexedfile/zfielddescription.h>
#include <zindexedfile/zindexfield.h>

namespace zbs {

class ZSIndexFile;

class ZFullIndexField: public ZSIndexField, public ZFieldDescription
{
public:
  ZFullIndexField();
  ZFullIndexField(ZFullIndexField & pIn) {_copyFrom(pIn);}

  ZFullIndexField & _copyFrom(ZFullIndexField & pIn)
  {
    ZSIndexField::_copyFrom(pIn);
    ZFieldDescription::_copyFrom(pIn);
    return *this;
  }

  ZFullIndexField & operator=(ZFullIndexField & pIn) {return _copyFrom(pIn);}

  ZFullIndexField &set(ZSIndexFile* pZIF, long pKeyFieldRank);
};

} // namespace zbs

#endif // ZFULLINDEXFIELD_H
