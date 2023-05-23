#ifndef BCKELEMENT_H
#define BCKELEMENT_H

#include <ztoolset/uristring.h>
#include <stdint.h>
#include <ztoolset/zdatefull.h>
#include <ztoolset/zstatus.h>

class BckElement {
public:
  BckElement() ;
  BckElement(const BckElement& pIn) {_copyFrom(pIn);}
  BckElement& _copyFrom(const BckElement& pIn);
  uriString Source;
  size_t    Size=0;
  uriString Target;
  ZDateFull Created;        /* stat for the existing file before the moment of backup */
  ZDateFull LastModified;   /* stat for the existing file before the moment of backup */
  ZStatus   Status=ZS_NOTHING;
};

#endif // BCKELEMENT_H
