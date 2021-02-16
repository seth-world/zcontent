#ifndef ZLOCK_CPP
#define ZLOCK_CPP
#include <string.h>
#include <ztoolset/charman.h>
#include <zcontentcommon/zlock.h>
/*
const char *decode_ZLockReason(ZLock_Reason pReason)
{
    switch (pReason)
    {
    case ZReason_Nothing:
        return "ZReason_Nothing";
    case ZReason_Read:
        return "ZReason_Read";
    case ZReason_Modify:
        return "ZReason_Modify";
    case ZReason_Delete:
        return "ZReason_Delete";
    case ZReason_Create:
        return "ZReason_Create";
    case ZReason_Other:
        return "ZReason_Other";
    }
}
*/
ZLockid & ZLockid::_copyFrom(const ZLockid &pIn)
{
  id= pIn.id;
  Owner=pIn.Owner;
  return *this;
}

ZDataBuffer ZLockid::_export() const
{
  ZDataBuffer wReturn;
  ZLockid_type wId=reverseByteOrder_Conditional<ZLockid_type>(id);
  wReturn.setData(&wId,sizeof(ZLockid_type));
  wReturn+= Owner._export();
  return wReturn;
}

size_t     ZLockid::_import(unsigned char *&pUniversalPtr)
{
  unsigned char* wPtrIn=pUniversalPtr;
  ZLockid_type wId;
  memmove(&wId,wPtrIn,sizeof(ZLockid_type));
  id=reverseByteOrder_Conditional<ZLockid_type>(wId);
  pUniversalPtr += sizeof(ZLockid_type);
  size_t wLen=sizeof(ZLockid_type);
  wLen += Owner._import(pUniversalPtr);
  return wLen;
}


CharMan decode_ZLockMask(zlockmask_type pLock)
{
    CharMan wCh;
    if (pLock == ZLock_Nolock)
        return CharMan("ZLock_Nolock");
    if (pLock == ZLock_All)
        return "ZLock_All";

    if (pLock & ZLock_Exclusive) {
        wCh.strcat("ZLock_Exclusive ");
    }
    if (pLock & ZLock_Delete) {
        wCh.strcat( "ZLock_Delete ");
    }
    if (pLock & ZLock_Read) {
        wCh.strcat( "ZLock_Read ");
    }
    if (pLock & ZLock_Modify) {
        wCh.strcat("ZLock_Modify ");
    }
    if (pLock & ZLock_Read) {
        wCh.strcat( "ZLock_Read ");
    }
    if (wCh[0] == 0) {
        wCh.strcat( "No lock found");
        }
    return wCh;
} //decode_ZLockMask

#endif //ZLOCK_CPP
