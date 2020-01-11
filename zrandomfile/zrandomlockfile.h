#ifndef ZRANDOMLOCKFILE_H
#define ZRANDOMLOCKFILE_H

#include <zrandomfile/zrandomfile.h>

class ZRandomLockFile : protected ZRandomFile
{
public:
    ZRandomLockFile();

    ZStatus zget(void* pRecord, size_t &pSize, const zrank_type pRank,zlockmask_type pLock=ZLock_Nolock);
    ZStatus zget(ZDataBuffer &pRecord, const zrank_type pRank,zlockmask_type pLock=ZLock_Nolock);

    ZStatus zgetNext (void* pUserRecord,size_t& pSize,zlockmask_type pLock=ZLock_Nolock);
    ZStatus zgetNext (ZDataBuffer &pUserRecord,zlockmask_type pLock=ZLock_Nolock);

    ZStatus zgetByAddress (ZDataBuffer&pRecord, zaddress_type pAddress);

    ZStatus zgetWAddress(ZDataBuffer &pRecord, const zrank_type pRank, zaddress_type &pAddress, zlockmask_type pLock=ZLock_Nolock);
    ZStatus zgetNextWAddress(ZDataBuffer &pRecord, zrank_type &pRank, zaddress_type &pAddress, zlockmask_type pLock=ZLock_Nolock);
    ZStatus zgetLastWAddress(ZDataBuffer &pRecord, zrank_type &pRank,zaddress_type &pAddress, zlockmask_type pLock=ZLock_Nolock);
    ZStatus zgetPreviousWAddress(ZDataBuffer &pRecord, zrank_type &pRank, zaddress_type &pAddress, zlockmask_type pLock=ZLock_Nolock);

    long searchBlockByAddress (ZFileDescriptor &pDescriptor,
                                zaddress_type pAddress);


    ZStatus zaddWithAddress (ZDataBuffer&pRecord, zaddress_type &pAddress, int pLock=0);

    ZStatus zgetLast (ZDataBuffer &pRecord, const int pLock=0);

    ZStatus zgetPrevious (ZDataBuffer &pRecord,const int pLock=0);



//-----------Record (Block) lock and unlock----------------------------------
/**
 * @brief zlockByAddress locks a record pointed by its address with a lock mask given by pLock see @ref ZLockMask_type
 * File descriptor is updated in file header if file is not opened in ZRF_Exclusive mode
 * @param pAddress ZRandomFile record (block) address to unlock
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
/*    Only Address is non volative  : rank is subject to change (insertion, deletion)
 */
  ZStatus zlock (const long pRank,const zlockmask_type pLock)
    {
        return _lock(ZDescriptor,pRank,pLock,!(getMode()&ZRF_Exclusive));
    }

    ZStatus zlockByAddress (const zaddress_type pAddress,const zlockmask_type pLock)
    {
        long wRank = getRankFromAddress(&ZDescriptor,pAddress);
        if (wRank<0)
        {
            ZException.addToLast(" While trying to lock by address.");
            return ZException.getLastStatus();
        }

        return _lock(ZDescriptor,wRank,pLock,!(getMode()&ZRF_Exclusive)); // NB: Force header write if mode is not ZRF_Exclusive
    }// zlockByAddress

    bool _isLockedByAddress(ZFileDescriptor &pDescriptor,const zaddress_type pAddress);

    ZStatus _getLockStatusByAddress(ZFileDescriptor &pDescriptor,const zaddress_type pAddress, zlockmask_type &pLock,pid_t &pPid);

/**
 * @brief zunlockByAddress Unlocks a record pointed by its address.
 * File descriptor is updated in file header if file is not opened in ZRF_Exclusive mode
 * @param pAddress ZRandomFile record (block) address to unlock
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
    ZStatus zunlockByAddress (const zaddress_type pAddress)
    {
        long wRank = getRankFromAddress(&ZDescriptor,pAddress);
        if (wRank<0)
            {
            ZException.addToLast(" While trying to lock by address.");
            return ZException.getLastStatus();
            }

        return _unlock(ZDescriptor,wRank,!(getMode()&ZRF_Exclusive)); // NB: Force header write if mode is not ZRF_Exclusive
    } // zunlockByAddress

    ZStatus zunlock (const long pRank)
    {
        return _unlock(ZDescriptor,pRank,!(getMode()&ZRF_Exclusive));
    } // zunlock
/**
  * @brief zunlockAll Unlocks all locks for the process for the file
  * File descriptor is updated in file header if file is not opened in ZRF_Exclusive mode
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
  */
    ZStatus zunlockAll (void)
    {
        return _unlockAll(ZDescriptor,!(getMode()&ZRF_Exclusive));
    }// zunlockAll

/**
  * @brief _unlockAll Unlocks all locks for the process for the file pointed by pDescriptor
  * File descriptor is updated in file header.
  * @param[in] pDescriptor file descriptor
  * @param[in] pForceWrite if this option is set to true, then it forces File Descriptor to be written on file
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
  */
 static inline
  ZStatus _unlockAll(ZFileDescriptor &pDescriptor, bool pForceWrite=true )

  {
      while (pDescriptor.ZBlockLock.size()>0)
                    {
                  _unlockZBAT(pDescriptor,pDescriptor.ZBlockLock.last().Address);
                  pDescriptor.ZBlockLock.pop();
                    }
     return (_writeFileDescriptor(pDescriptor,pForceWrite));
  }
/**
* @brief _unlock Releases the lock set to block of rank pRank
*   This routine writes the file descriptor to header
*
* @param[in,out] pDescriptor file descriptor
* @param[in] pRank rank of the record to be unlocked
* @param[in] pForceWrite if this option is set to true, then it forces File Descriptor to be written on file
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
*/
 static inline
  ZStatus _unlock(ZFileDescriptor &pDescriptor ,const zaddress_type pAddress,bool pForceWrite=true)
  {
 ZStatus wSt;
 long  wi;
 bool wFound = false;
    for (wi=0;wi<pDescriptor.ZBlockLock.size();wi++)
                    {
                if (pDescriptor.ZBlockLock[wi].Address==pAddress)
                                    {
                                    pDescriptor.ZBlockLock.erase(wi);
                                    wFound = true;
                                    break;
                                    }
                    }
    if (!wFound)
                            return ZS_SUCCESS;      // no lock found for this ZBAT rank;
    wSt=_unlockZBAT(pDescriptor,pAddress);
    if (wSt!= ZS_SUCCESS)
                return wSt;

    return (_writeFileDescriptor(pDescriptor,pForceWrite));

  }// _unlock
 /**
  * @brief _unlockZBAT  Low level unlock routine : mark ZBAT block on file to clear lock to ZLock_Nolock (release lock)
  * @warning this is not the appropriate user routine to unlock a block @see _unlock
  * @param[in] pDescriptor file descriptor
  * @param[in] pRank rank of block to unlock in ZBAT
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
  */
 static inline
  ZStatus _unlockZBAT(ZFileDescriptor &pDescriptor,const zaddress_type pAddress )
  {
    long wRank = getRankFromAddress(&pDescriptor,pAddress);
    if (wRank<0)
        {
        ZException.addToLast("While unlocking ZBAT (%s)",_GET_FUNCTION_NAME_);
        ZException.exit_abort();
        }
    pDescriptor.ZBAT->Tab[wRank].Lock = ZLock_Nothing;
    pDescriptor.ZBAT->Tab[wRank].Pid = 0L;
    return _writeBlockHeader(pDescriptor,pDescriptor.ZBAT->Tab[wRank],pDescriptor.ZBAT->Tab[wRank].Address);
  }

inline
ZStatus _getStatusFromLock (const zlockmask_type pLock)
 {
ZStatusBase wSt;

    if (!(pLock&ZS_LOCKED))
                            return ZS_SUCCESS;
     wSt= ZS_LOCKED ;
     if (pLock & ZLock_Read)
                     wSt |=(long)ZS_LOCKREAD ;
     if (pLock & ZLock_Write)
                     wSt |=(long)ZS_LOCKWRITE ;
     if (pLock & ZLock_Delete)
                     wSt |=(long)ZS_LOCKDELETE ;
     return (ZStatus)wSt;
 }

 /**
  * @brief _lockZBAT Low level lock routine : mark ZBAT block on file with lock pLock
  * Block header is written on file with lock information
  * @param[in] pDescriptor file descriptor
  * @param[in] pRank       rank of block to lock
  * @param[in] pLock      lock mask
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
  */
 inline ZStatus _lockZBAT(ZFileDescriptor &pDescriptor,
                          const zaddress_type pAddress,
                          const zlockmask_type pLock);

 /**
  * @brief _getlock gets the lock status and mask (pLock) for the block pointed by its rank pRank.
  * @param[in] pDescriptor file descriptor
  * @param[in] pRank       rank of block to get lock information
  * @param[out] pLock      lock mask
  * @return ZS_SUCCESS if no lock found : pLock is then set to ZLock_Nolock. If a lock is found returns ZS_LOCKED and pLock is set with the lock mask found
  */
 static inline
  ZStatus _getlock(ZFileDescriptor &pDescriptor,const long pRank ,zlockmask_type &pLock)
  {
    if ((pLock= pDescriptor.ZBAT->Tab[pRank].Lock)!=ZLock_Nolock)
            if (pDescriptor.ZBAT->Tab[pRank].Pid!=pDescriptor.Pid)
                                                    return (ZS_LOCKED); // locked by another process
    return (ZS_SUCCESS); // no lock or locked by this pid
  }

#ifdef __COMMENT__
 /**
  * @brief _lock Locks the block of rank pRank with lock mask pLock.
  * @param[in-out] pDescriptor file descriptor
  * @param[in] pRank    rank of block to lock
  * @param[in] pLock    lock mask
  * @param pForceWrite if false : ZFileDescriptor is not updated to file. if true: force to update ZFileDescriptor to file.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message. see: @ref ZBSError
  */
inline
  ZStatus
 _lock(ZFileDescriptor &pDescriptor ,const long pRank,const zlock_type pLock,bool pForceWrite=true)
  {
  ZStatusBase wSt;
  ZBlockHeader wBlockHeader;
  ZLock_struct wlockS;
    for (long wi=0;wi<pDescriptor.ZBlockLock.size();wi++)
                    {
                if (pDescriptor.ZBlockLock[wi].Rank==pRank)
                                    {
                                    pDescriptor.ZBlockLock.erase(wi);    // if a lock already exist remove it
                                    break;
                                    }
                    }

    wSt= _getBlockHeader(pDescriptor,pDescriptor.ZBAT->Tab[pRank].Address,wBlockHeader);
    if (wSt!=ZS_SUCCESS)
        {
        return (ZStatus)wSt;
        }
    if (wBlockHeader.Lock!=ZLock_Nolock)
            if (wBlockHeader.Pid!=pDescriptor.Pid)
                    {
                    wSt= ZS_LOCKED ;
                    if (wBlockHeader.Lock & ZLock_Read)
                                    wSt |=(ZStatusBase)ZS_LOCKREAD ;
                    if (wBlockHeader.Lock & ZLock_Write)
                                    wSt |=(ZStatusBase)ZS_LOCKWRITE ;
                    if (wBlockHeader.Lock & ZLock_Delete)
                                    wSt |=(ZStatusBase)ZS_LOCKDELETE ;
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            wSt,
                                            Severity_Error,
                                            "Cannot lock record. Already locked by another process lock mask is <%s>",
                                            decode_ZLockMask(wBlockHeader.Lock));
                    return (ZStatus)wSt;
                    }

    wBlockHeader.Lock = pLock;
    wBlockHeader.Pid = pDescriptor.Pid;

    wSt=_writeBlockHeader(pDescriptor,wBlockHeader,pDescriptor.ZBAT->Tab[pRank].Address,_GET_FUNCTION_NAME_);
                    return (ZStatus)wSt;
    if (wSt!=ZS_SUCCESS)

    wSt=_lockZBAT(pDescriptor,pRank,pLock);
    if (wSt!= ZS_SUCCESS)
                return (ZStatus)wSt;

    wlockS.Lock=pLock;
    wlockS.Rank=pRank;
    pDescriptor.ZBlockLock.push(wlockS);
    return (_writeFileDescriptor(pDescriptor,pForceWrite));
  } //_lock

#endif // __COMMENT__

  /**
  * @brief _lock Locks the block of address pAddress with lock mask pLock.
  * @param[in-out] pDescriptor file descriptor
  * @param[in] pAddress    Address of block to lock
  * @param[in] pLock    lock mask
  * @param pForceWrite if false : ZFileDescriptor is not updated to file. if true: force to update ZFileDescriptor to file.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message. see: @ref ZBSError
 * @return
 */
ZStatus _lock(ZFileDescriptor &pDescriptor,
                const zaddress_type pAddress,
                const zlockmask_type pLock,
                bool pForceWrite = true);


//----------End lock Management-----------------------------

};

#endif // ZRANDOMLOCKFILE_H
