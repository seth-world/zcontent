#ifndef ZSEARCHCOLLECTION_H
#define ZSEARCHCOLLECTION_H

#include <memory>

#include <ztoolset/zarray.h>
#include <zcontent/zrandomfile/zrandomfiletypes.h>



namespace zbs {

class ZSearchEntity;
class ZSearchFormula;

class ZSearchCollection
{
public:
  ZSearchCollection()=default;
  ZSearchCollection(const ZSearchCollection& pIn) { _copyFrom(pIn);}
  ~ZSearchCollection();

  ZSearchCollection&  _copyFrom (const ZSearchCollection& pIn);

  ZSearchCollection&  operator = (const ZSearchCollection& pIn) { return _copyFrom(pIn);}

  size_t getRecordsToProcess();

  ZSearchFormula*                 Formula=nullptr;
  std::shared_ptr<ZSearchEntity>  Entity=nullptr;
  ZArray<zaddress_type>           AddressList;
};


}// namespace zbs

#endif // ZSEARCHCOLLECTION_H
