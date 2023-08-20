#ifndef ZSEARCH_H
#define ZSEARCH_H

#include <stdint.h>
#include <ztoolset/zdatabuffer.h>
#include <zrandomfile/zrandomfiletypes.h> // for zaddress_type

#include "zsearchparser.h"

namespace zbs {

class ZMasterFile;





class ZSearch
{
public:
  ZSearch(ZMasterFile& pMasterFile) {}

  ZStatus parse(const utf8VaryingString& pString);

  ZStatus search();

  ZStatus search(const utf8VaryingString& pString);
private:
  ZStatus _search();

  bool _evaluateRecord(const ZDataBuffer& pRecord);
  bool _evaluateFormulaTerm(const ZDataBuffer& pRecord,const ZSearchLogicalOperand& pElt) ;

  ZSearchParser* pParser=nullptr;

};

} // namespace zbs


#endif // ZSEARCH_H
