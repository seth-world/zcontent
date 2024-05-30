#include "zsearchfileentity.h"

//#include "zsearchtoken.h"

using namespace zbs;

namespace zbs {

using namespace std;
/*
ZSearchEntityContext
ZSearchMasterFile::newEntityContext() {
    ZSearchEntityContext wCtx ;
    wCtx._URFParser.setDictionary(getMetaDic());
    wCtx.FileEntity = this;
    wCtx.ErrorLog = ErrorLog;
    return wCtx;
}
*/



ZStatus
ZSearchMasterFile::set(const uriString& pPath)
{
    return (*this)->setPath(pPath);
}

ZStatus
ZSearchMasterFile::getFirst(ZSearchEntityContext& pSEC,zaddress_type &pAddress)
{
    pSEC.Status=(*this)->zgetWAddress(pSEC._URFParser.Record,0L,pAddress);
    if (pSEC.Status!=ZS_SUCCESS) {
        return pSEC.Status;
    }
    pSEC._URFParser._setupRecord();
    pSEC.Status=pSEC._URFParser.parse(ErrorLog);
    if (pSEC.Status==ZS_SUCCESS) {
        pSEC.CurrentRank=CurrentRank=0L;
        pSEC.LastAddress = LastAddress=pAddress;
    }
    return pSEC.Status;
}

ZStatus
ZSearchMasterFile::getNext(ZSearchEntityContext& pSEC,zaddress_type &pAddress)
{
    int pOutRank = CurrentRank + 1;
    pSEC.Status=(*this)->zgetWAddress(pSEC._URFParser.Record,pOutRank,pAddress);
    if (pSEC.Status!=ZS_SUCCESS) {
        return pSEC.Status;
    }
    pSEC._URFParser._setupRecord();
    pSEC.Status = pSEC._URFParser.parse(ErrorLog);
    pSEC.CurrentRank=CurrentRank=pOutRank;
    pSEC.LastAddress = LastAddress=pAddress;

    return pSEC.Status;
}



ZStatus
ZSearchMasterFile::getByRank(ZSearchEntityContext& pSEC,const long pRank,zaddress_type &pAddress) {
    pSEC.Status=(*this)->zgetWAddress(pSEC._URFParser.Record,pRank,pAddress);
    if (pSEC.Status!=ZS_SUCCESS) {
        return pSEC.Status;
    }
    pSEC.CurrentRank = CurrentRank = pRank;
    pSEC.LastAddress = LastAddress = pAddress;
    pSEC._URFParser._setupRecord();
    return pSEC.Status = pSEC._URFParser.parse(ErrorLog);
}

ZStatus
ZSearchMasterFile::getByAddress(ZSearchEntityContext& pSEC,const zaddress_type pAddress) {
    pSEC.Status=(*this)->zgetByAddress(pSEC._URFParser.Record,pAddress);
    if (pSEC.Status!=ZS_SUCCESS) {
        return pSEC.Status;
    }
    CurrentRank=pSEC.CurrentRank = -1 ;
    pSEC.LastAddress = LastAddress = pAddress;
    pSEC._URFParser._setupRecord();
    return pSEC.Status = pSEC._URFParser.parse(ErrorLog);
}



ZaiErrors*
ZSearchMasterFile::_getErrorLog()
{
    if (ErrorLog!=nullptr)
        return ErrorLog;

    return ErrorLog = &(*this)->ErrorLog;
}



} //namespace zbs
