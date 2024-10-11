#include "zdomainbroker.h"

namespace zbs {

class ZDomainBroker DomainBroker ;

ZDomainBroker::ZDomainBroker()
{
    DomainRoot = new ZDomainPath(ZDOM_Root) ;
}

} // namespace zbs
