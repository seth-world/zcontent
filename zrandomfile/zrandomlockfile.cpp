#ifndef ZRANDOMLOCKFILE_CPP
#define ZRANDOMLOCKFILE_CPP
#include <zrandomfile/zrandomlockfile.h>
#include <ztoolset/charman.h>

ZRandomLockFile::ZRandomLockFile()
{

}

ZStatus
ZRandomLockFile::_getLockStatusByAddress(ZFileDescriptor &pDescriptor,const zaddress_type pAddress, zlockmask_type &pLock,pid_t &pPid)
{
ZBlockHeader wBlockHeader;
ZStatus wSt;
    wSt=_getBlockHeader(pDescriptor,pAddress,wBlockHeader);
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

inline ZStatus ZRandomLockFile::_lockZBAT(ZFileDescriptor &pDescriptor,const zaddress_type pAddress,const zlockmask_type pLock)
{
    ZStatus wSt;
    ZBlockHeader wBlockHeader;
    //  wSt= _getBlockHeader(pDescriptor,pDescriptor.ZBAT->Tab[pRank].Address,wBlockHeader);
    wSt= _getBlockHeader(pDescriptor,pAddress,wBlockHeader);
    if (wSt!=ZS_SUCCESS)
    {
        return wSt;
    }
    if (wBlockHeader.Lock!=ZLock_Nolock)
        if (wBlockHeader.Pid!=pDescriptor.Pid)
        {
            wSt=(ZStatus)_getStatusFromLock(pLock);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  wSt,
                                  Severity_Error,
                                  "Cannot lock record. Already locked by another process lock mask is <%s>",
                                  decode_ZLockMask(wBlockHeader.Lock).toChar());
            return wSt;
        }
    ZLock_struct wLock;
    wLock.Lock = pLock;
    //    wLock.Address = pDescriptor.ZBAT->Tab[pRank].Address;
    wLock.Address = pAddress;
    pDescriptor.ZBlockLock.push(wLock);

    long wRank = getRankFromAddress(&pDescriptor,pAddress);
    pDescriptor.ZBAT->Tab[wRank].Lock = pLock;
    pDescriptor.ZBAT->Tab[wRank].Pid = pDescriptor.Pid;

    wBlockHeader.Lock = pLock;
    wBlockHeader.Pid = pDescriptor.Pid;

    return _writeBlockHeader(pDescriptor,wBlockHeader,pAddress);
}//ZRandomLockFile::_lockZBAT

inline ZStatus ZRandomLockFile::_lock(ZFileDescriptor &pDescriptor,
                                      const zaddress_type pAddress,
                                      const zlockmask_type pLock,
                                      bool pForceWrite)
{
    ZStatusBase wSt;
    ZBlockHeader wBlockHeader;
    ZLock_struct wlockS;
    for (long wi = 0; wi < pDescriptor.ZBlockLock.size(); wi++) {
        if (pDescriptor.ZBlockLock[wi].Address == pAddress) {
            pDescriptor.ZBlockLock.erase((size_t) wi); // if a lock already exist remove it
            break;
        }
    }

    wSt = _getBlockHeader(pDescriptor, pAddress, wBlockHeader);
    if (wSt != ZS_SUCCESS) {
        return (ZStatus) wSt;
    }
    if (wBlockHeader.Lock != ZLock_Nolock)
        if (wBlockHeader.Pid != pDescriptor.Pid) {
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
                decode_ZLockMask(wBlockHeader.Lock));
            return (ZStatus) wSt;
        }

    wBlockHeader.Lock = pLock;
    wBlockHeader.Pid = pDescriptor.Pid;

    wSt = _writeBlockHeader(pDescriptor, wBlockHeader, pAddress);
    return (ZStatus) wSt;
    if (wSt != ZS_SUCCESS)

        wSt = _lockZBAT(pDescriptor, pAddress, pLock);
    if (wSt != ZS_SUCCESS)
        return (ZStatus) wSt;

    wlockS.Lock = pLock;
//    wlockS.Rank = pRank;
    wlockS.Address = pAddress;
    pDescriptor.ZBlockLock.push(wlockS);
    return (_writeFileDescriptor(pDescriptor,pForceWrite));
}//_lock


#endif // ZRANDOMLOCKFILE_CPP
