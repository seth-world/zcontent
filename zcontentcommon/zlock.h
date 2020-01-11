#ifndef ZLOCK_H
#define ZLOCK_H
/** @file zlock.h concerns general lock management : independant from application */
#include <stdint.h>
#include <zcontentcommon/zidentity.h>
#include <ztoolset/zutfstrings.h>
#include <zcontentcommon/zresource.h>

/*
 *                  Identity generic format
 *                      |
 *      +---------------+---------------+------------+-----------+--
 *      |                               |            |           |
 *  ZResource  <--lock by resource id->lock        userid      groupid
 * (any managed object)
 *      |
 *      |
 *      |
 *  ZDocument
 *
 */


typedef Identity_type ZLockid_type;
typedef uint8_t zLockmask_type;
//typedef int64_t Docid_type;


typedef Identity_type UserId_type;
//typedef int64_t Resourceid_type;


const ZLockid_type      cst_LockInvalid = INT64_MIN;
const ZLockid_type      cst_LockPending = -0x01;


struct ZLockid
{
public:
    ZLockid_type id;

    ZLockid(void) { id = 0; }
    ZLockid(ZLockid_type pId) { id = pId; }
    ZLockid(const ZLockid &pIn) { id = pIn.id; }
    ZLockid(const ZLockid &&pIn) { id = pIn.id; }


    bool isValid(void) const { return (!(id < 0)); }
    bool isInvalid(void) const { return (id == cst_LockInvalid); }
    bool isPending(void) const { return (id == cst_LockPending); }


    void setInvalid(void) { id = cst_LockInvalid; }
    void setPending(void) { id = cst_LockPending; }
    void clear(void) { id = cst_LockInvalid; }

    ZLockid getNext(void)
    {
        id++;
        return (*this);
    }

    ZLockid clearid(void)
    {
        ZLockid wid;
        wid.id = -1;
        return wid;
    }    
    static ZLockid getInvalid(void)
    {
        ZLockid wid;
        wid.id = cst_LockInvalid;
        return wid;
    }

    ZLockid &operator=(ZLockid &pIn)
    {
        id = pIn.id;
        return (*this);
    }

    ZLockid &operator=(ZLockid &&pIn)
    {
        id = pIn.id;
        return (*this);
    }

    ZLockid &operator=(long pId)
    {
        id = pId;
        return (*this);
    }
    ZLockid &operator=(const ZLockid &pId)
    {
        id = pId.id;
        return (*this);
    }

    int compare(ZLockid &pIn) const { return int(id - pIn.id); }

    bool operator==(const ZLockid &pId) { return (id == pId.id); }
    bool operator!=(const ZLockid &pId) { return !(id == pId.id); }
};



enum ZLock_Reason : uint8_t {
    ZReason_Nothing = 0,
    ZReason_Read = 1,
    ZReason_Modify = 2,
    ZReason_Delete = 4,
    ZReason_Create = 8,
    ZReason_Other  = 0x10
};

typedef uint8_t         zlockmask_type ;

enum ZLockMask : zlockmask_type
{
    ZLock_Nothing   = 0,            //!< NOP
    ZLock_Nolock    = 0,            //!< no lock has been requested for this resource
    ZLock_Read      = 0x02,         //!< cannot read the resource
    ZLock_Modify    = 0x04,         //!< cannot write/modify the resource
    ZLock_Write     = ZLock_Modify, //!< idem for easy naming use
    ZLock_Delete    = 0x08,         //!< cannot delete resource. Can read everything and modify -except indeed the resource identification-
    ZLock_All       = ZLock_Read|ZLock_Write|ZLock_Delete,          //!< cannot do anything
    ZLock_Exclusive = 0x10,         //!< for compatibility with ZRF_Exclusive
    ZLock_ReadWrite = ZLock_Read | ZLock_Modify,  //!< self explainatory
    ZLock_WriteDelete = ZLock_Write | ZLock_Delete,  //!< cannot write or delete record or file : only read is allowed
    ZLock_Omitted   = 0xFF         //!< Lock is deliberately omitted as an argument

};
class CharMan;
CharMan decode_ZLockMask(zlockmask_type pLock);


class ZLock
{
    ZLock &_copyFrom(const ZLock &pIn)
    {
        Resourceid = pIn.Resourceid;
        Lockid = pIn.Lockid;
        Mask = pIn.Mask;
        Owner = pIn.Owner;
        Reason = pIn.Reason;
        DataRank = pIn.DataRank;
    }
public:
    ZResource       Resourceid;
    ZLockid         Lockid;
    zlockmask_type  Mask;
    UserId_type     Owner;
    ZLock_Reason    Reason;
    DataRank_type   DataRank;

    ZLock (void) {clear();}
    ZLock(ZLock &pIn) { _copyFrom(pIn); }
    ZLock(ZLock &&pIn) { _copyFrom(pIn); }

    void clear(void)
    {
        Resourceid.clear();
        Lockid.clear();
        Mask=ZLock_Nothing;
        Owner=cst_IdentityInvalid;
        Reason=ZReason_Nothing;
        DataRank = cst_DataRankInvalid;
    }

    ZLock &operator=(const ZLock &pIn) { return _copyFrom(pIn); }
    ZLock &operator=(const ZLock &&pIn) { return _copyFrom(pIn); }
};


const char *
decode_ZLockReason(ZLock_Reason pReason);


#endif // ZLOCK_H

