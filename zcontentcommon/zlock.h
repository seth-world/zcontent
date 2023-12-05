#ifndef ZLOCK_H
#define ZLOCK_H
/** @file zlock.h concerns general lock management : independant from application */
#include <stdint.h>
#include <zcontentcommon/zidentity.h>
#include <ztoolset/utfvaryingstring.h>
#include <zcontentcommon/zresource.h>

typedef long            zrank_type;     //!< Block record rank data type NB: could be negative if unknown
typedef int64_t         zaddress_type;  //!< ZRandomFile address data type : nb : could be negative if unknown
typedef uint64_t        zsize_type;     //!< ZRandomFile size data type

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
    ZLockid_type  id;
    ZResource     Owner;

    ZLockid(void) { id = 0; Owner.setInvalid();}
    ZLockid(ZLockid_type pId,const ZResource& pOwner) { id = pId; Owner=pOwner;}
    ZLockid(const ZLockid &pIn) { _copyFrom(pIn); }
    ZLockid(const ZLockid &&pIn) { _copyFrom(pIn); }

    ZLockid &_copyFrom(const ZLockid &pIn);

    bool isValid(void) const { return (!(id < 0)); }
    bool isInvalid(void) const { return (id == cst_LockInvalid); }
    bool isPending(void) const { return (id == cst_LockPending); }


    void setInvalid(void) { id = cst_LockInvalid; Owner.setInvalid();}
    void setPending(void) { id = cst_LockPending; }
    void clear(void) { id = cst_LockInvalid; Owner.setInvalid();}

    ZLockid getNext(void)
    {
        id++;
        return (*this);
    }
    ZLockid getNext(const ZResource& pOwner)
    {
      id++;
      return ZLockid(id,pOwner);
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

    ZLockid &operator=(const ZLockid &pIn) {return _copyFrom(pIn);}
    ZLockid &operator=(const ZLockid &&pIn){return _copyFrom(pIn);}

    int compare(const ZLockid &pIn) const { return int(id - pIn.id); }

    bool operator==(const ZLockid &pId) { return (id == pId.id); }
    bool operator!=(const ZLockid &pId) { return !(id == pId.id); }

    ZDataBuffer _export() const ;
    size_t      _import(const unsigned char *&pUniversalPtr);

};


/*
enum ZLock_Reason : uint8_t {
    ZReason_Nothing = 0,
    ZReason_Read = 1,
    ZReason_Modify = 2,
    ZReason_Delete = 4,
    ZReason_Create = 8,
    ZReason_Other  = 0x10
};
*/
typedef uint8_t         zlockmask_type ;

enum ZLockMask : zlockmask_type
{
    ZLock_Nothing   = 0,            //!< NOP
    ZLock_Nolock    = 0,            //!< no lock has been requested for this resource
    ZLock_Delete    = 0x01,         //!< cannot delete resource. Can read everything and modify -except indeed the resource identification-
    ZLock__Modify_  = 0x02,         //!< cannot write/modify the resource
    ZLock_Modify    = 0x03,         //!< cannot modify , so cannot delete : only read is allowed
    ZLock__Read_    = 0x04,        //!< cannot read the resource
    ZLock_Read      = 0x07,         //!< cannot read the resource, so cannot write and delete also
    ZLock_Write     = ZLock_Modify, //!< idem for easy naming use
    ZLock_All       = ZLock_Read|ZLock_Write|ZLock_Delete,          //!< cannot do anything
    ZLock_Exclusive = 0x10,         //!< for compatibility with ZRF_Exclusive
    ZLock_ReadWrite = ZLock_Read | ZLock_Modify,  //!< self explainatory
    ZLock_WriteDelete = ZLock_Write | ZLock_Delete,  //!< cannot write or delete record or file : only read is allowed
    ZLock_Omitted   = 0xFF         //!< Lock is deliberately omitted as an argument

};

utf8VaryingString decode_ZLockMask(zlockmask_type pLock);


class ZLock
{
    ZLock &_copyFrom(const ZLock &pIn)
    {
        Resourceid = pIn.Resourceid;
        Lockid = pIn.Lockid;
        Mask = pIn.Mask;
//        Owner = pIn.Owner;
//        Reason = pIn.Reason;
        DataRank = pIn.DataRank;
        return *this;
    }
public:
    ZResource       Resourceid;
    ZLockid         Lockid;
    zlockmask_type  Mask;
//    ZResource       Owner;  /* Owner is set within ZLockid */
//    ZLock_Reason    Reason;
    DataRank_type   DataRank;

    ZLock (void) {clear();}
    ZLock(ZLock &pIn) { _copyFrom(pIn); }
    ZLock(ZLock &&pIn) { _copyFrom(pIn); }

    void clear(void)
    {
        Resourceid.clear();
        Lockid.clear();
        Mask=ZLock_Nothing;
//        Owner=cst_IdentityInvalid;
//        Reason=ZReason_Nothing;
        DataRank = cst_DataRankInvalid;
    }

    ZLock &operator=(const ZLock &pIn) { return _copyFrom(pIn); }
    ZLock &operator=(const ZLock &&pIn) { return _copyFrom(pIn); }
};

/*
const char *
decode_ZLockReason(ZLock_Reason pReason);
*/

#endif // ZLOCK_H

