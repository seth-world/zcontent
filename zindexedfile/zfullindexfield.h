#ifndef ZFULLINDEXFIELD_H
#define ZFULLINDEXFIELD_H

#include <zindexedfile/zfielddescription.h>
#include <zindexedfile/zindexfield.h>
#include <zindexedfile/zrawindexfile.h>

namespace zbs {

class ZSIndexFile;

class ZFullIndexField: public ZIndexField, public ZFieldDescription
{
public:
  ZFullIndexField();
  ZFullIndexField(ZFullIndexField & pIn) {_copyFrom(pIn);}

  ZFullIndexField & _copyFrom(ZFullIndexField & pIn)
  {
    ZIndexField::_copyFrom(pIn);
    ZFieldDescription::_copyFrom(pIn);
    return *this;
  }

  ZFullIndexField & operator=(ZFullIndexField & pIn) {return _copyFrom(pIn);}

  ZFullIndexField &set(ZRawIndexFile *pZIF, long pKeyFieldRank);
};

} // namespace zbs

#endif // ZFULLINDEXFIELD_H
