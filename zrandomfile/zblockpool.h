#ifndef ZBLOCKPOOL_H
#define ZBLOCKPOOL_H

#include <stdint.h>
#include <ztoolset/zarray.h>
#include <zcontent/zrandomfile/zblock.h>


class ZBlockPool:public ZArray<ZBlockDescriptor>
{
public:
  typedef ZArray<ZBlockDescriptor> _Base;
  ZBlockPool();
  ZBlockPool(ZBlockPool& pIn):ZArray<ZBlockDescriptor>(pIn) {}
  ZBlockPool(ZBlockPool&& pIn):ZArray<ZBlockDescriptor>(pIn) {}

  ZBlockPool& operator = (ZBlockPool& pIn){return (ZBlockPool&)ZArray<ZBlockDescriptor>::_copyFrom(pIn);}

  inline long _addSorted(ZBlockDescriptor& pBlockDescriptor) {
    if (count()==0)
      return _Base::push(pBlockDescriptor);
    zrank_type wi=0;
    while ((wi < count()) && (Tab[wi].Address < pBlockDescriptor.Address)) {
      wi++;
    }
    /* in the same pool we cannot have 2 blocks with the same address */
    if (Tab[wi].Address==pBlockDescriptor.Address) {
      _DBGPRINT("ZBlockPool::_addSorted-F-IVADDR Address collision in free block pool.\n")
      _ABORT_
    }
    if (wi==count())
      return _Base::push(pBlockDescriptor);
    return _Base::insert(pBlockDescriptor,wi);
  }

  size_t        _exportAppendPool(ZDataBuffer&pZDBExport);
  size_t        getPoolExportSize();
  size_t        _importPool(const unsigned char *&pPtrIn);
};

#endif // ZBLOCKPOOL_H
