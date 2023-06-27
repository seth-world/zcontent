#ifndef ZRANDOMLOCKFILE_CPP
#define ZRANDOMLOCKFILE_CPP
#include <zrandomfile/zrandomlockfile.h>
#include <ztoolset/charman.h>

ZRandomLockFile::ZRandomLockFile()
{

}

ZStatus
ZRandomLockFile::_getLockStatusByAddress(const zaddress_type pAddress, zlockmask_type &pLock,pid_t &pPid)
{
ZBlockHeader wBlockHeader;
ZStatus wSt;
    wSt=_getBlockHeader(pAddress,wBlockHeader);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    pLock= wBlockHeader.Lock;
    pPid=wBlockHeader.Pid;
/*    if (pLock!=ZLock_Nolock)
            if (pPid!=pDescriptor.Pid)
                {
                return _getStatusFromLock(pLock);
                }
*/
    return _getStatusFromLock(pLock);
}

inline ZStatus ZRandomLockFile::_lockZBAT(const zaddress_type pAddress,const zlockmask_type pLock)
{
    ZStatus wSt;
    ZBlockHeader wBlockHeader;
    //  wSt= _getBlockHeader(pDescriptor,pDescriptor.ZBAT[pRank].Address,wBlockHeader);
    wSt= _getBlockHeader(pAddress,wBlockHeader);
    if (wSt!=ZS_SUCCESS)
    {
        return wSt;
    }
    if (wBlockHeader.Lock!=ZLock_Nolock)
        if (wBlockHeader.Pid!=Pid)
        {
            wSt=(ZStatus)_getStatusFromLock(pLock);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  wSt,
                                  Severity_Error,
                                  "Cannot lock record. Already locked by another process lock mask is <%s>",
                                  decode_ZLockMask(wBlockHeader.Lock).toCChar());
            return wSt;
        }
    ZLock_struct wLock;
    wLock.Lock = pLock;
    //    wLock.Address = pDescriptor.ZBAT[pRank].Address;
    wLock.Address = pAddress;
    ZBlockLock.push(wLock);

    long wRank = getRankFromAddress(pAddress);
    ZBAT[wRank].Lock = pLock;
    ZBAT[wRank].Pid = Pid;

    wBlockHeader.Lock = pLock;
    wBlockHeader.Pid = Pid;

    return _writeBlockHeader(wBlockHeader,pAddress);
}//ZRandomLockFile::_lockZBAT
/*
long
ZRandomLockFile::getRankFromAddress(const zaddress_type pAddress)
{
  for (long wi=0;wi<ZBAT.size();wi++)
    if (ZBAT.Tab(wi).Address==pAddress)
      return wi;
  ZException.setMessage(_GET_FUNCTION_NAME_,
      ZS_INVADDRESS,
      Severity_Error,
      " Address <%lld> does not correspond to a valid block address ",
      pAddress);
  return (-1) ;
}
*/

ZStatus ZRandomLockFile::_lock(const zaddress_type pAddress,
                                      const zlockmask_type pLock,
                                      bool pForceWrite)
{
    ZStatusBase wSt;
    ZBlockHeader wBlockHeader;
    ZLock_struct wlockS;
    for (long wi = 0; wi < ZBlockLock.size(); wi++) {
        if (ZBlockLock[wi].Address == pAddress) {
            ZBlockLock.erase((size_t) wi); // if a lock already exist remove it
            break;
        }
    }

    wSt = _getBlockHeader(pAddress, wBlockHeader);
    if (wSt != ZS_SUCCESS) {
        return (ZStatus) wSt;
    }
    if (wBlockHeader.Lock != ZLock_Nolock) {
        if (wBlockHeader.Pid != Pid) {
            wSt = ZS_LOCKED;
            if (wBlockHeader.Lock & ZLock_Read)
                wSt |= (ZStatusBase) ZS_LOCKREAD;
            if (wBlockHeader.Lock & ZLock_Write)
                wSt |= (ZStatusBase) ZS_LOCKWRITE;
            if (wBlockHeader.Lock & ZLock_Delete)
                wSt |= (ZStatusBase) ZS_LOCKDELETE;
            ZException.setMessage(
                _GET_FUNCTION_NAME_,
                (ZStatus) wSt,
                Severity_Error,
                "Cannot lock record. Already locked by another process lock mask is <%s>",
                decode_ZLockMask(wBlockHeader.Lock).toCChar());
            return (ZStatus) wSt;
        }
    }
    wBlockHeader.Lock = pLock;
    wBlockHeader.Pid = Pid;

    wSt = _writeBlockHeader(wBlockHeader, pAddress);
    return (ZStatus) wSt;
    if (wSt != ZS_SUCCESS)

        wSt = _lockZBAT( pAddress, pLock);
    if (wSt != ZS_SUCCESS)
        return (ZStatus) wSt;

    wlockS.Lock = pLock;

    wlockS.Address = pAddress;
    ZBlockLock.push(wlockS);
    return (_writeFCB(pForceWrite));
}//_lock


#endif // ZRANDOMLOCKFILE_CPP
