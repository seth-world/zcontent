#include "zmfobject.h"

namespace zbs {
ZMFObject::ZMFObject() {}

ZMFObject&
ZMFObject::_copyFrom(const ZMFObject& pIn)
{
    ZDomainObject::_copyFrom(pIn);
    return *this;
}


}
