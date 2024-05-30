#ifndef ZSEARCHJOINADDRESS_H
#define ZSEARCHJOINADDRESS_H

#include "zmf_limits.h"
namespace zbs {


class ZSearchJoinAddress {
public:
    ZSearchJoinAddress() ;
    ZSearchJoinAddress(const ZSearchJoinAddress& pIn) { _copyFrom(pIn);}

    ZSearchJoinAddress(zaddress_type pMAddress,zaddress_type pSAddress);
    ZSearchJoinAddress& _copyFrom(const ZSearchJoinAddress& pIn);

    ZSearchJoinAddress& operator = (const ZSearchJoinAddress& pIn) { return _copyFrom(pIn);}

    void setInvalid() { MasterAddress=-1 ; SlaveAddress=-1; }

    bool isValid() { return (MasterAddress > -1) && (SlaveAddress > -1) ; }

    zaddress_type MasterAddress=-1;
    zaddress_type SlaveAddress=-1;
};
} // namespace zbs
#endif // ZSEARCHJOINADDRESS_H
