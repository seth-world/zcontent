#include "zsearchjoinaddress.h"
namespace zbs {
ZSearchJoinAddress::ZSearchJoinAddress() {}

ZSearchJoinAddress::ZSearchJoinAddress(zaddress_type pMAddress,zaddress_type pSAddress)
{
    MasterAddress = pMAddress;
    SlaveAddress = pSAddress;
}

ZSearchJoinAddress&
ZSearchJoinAddress::_copyFrom(const ZSearchJoinAddress& pIn)
{
    MasterAddress=pIn.MasterAddress;
    SlaveAddress=pIn.SlaveAddress;
    return *this;
}

} // namespace zbs {
