#ifndef ZRESOURCE_H
#define ZRESOURCE_H

#include <stdint.h>

//#include <zcontentcommon/zidentity.h>
#include <ztoolset/zutfstrings.h> // for utfcodeString

typedef int64_t Resourceid_type;
typedef uint64_t ZEntity_type;
typedef int64_t DataRank_type;


const ZEntity_type cst_EntityInvalid=0;
const Resourceid_type cst_ResourceInvalid = INT64_MIN;
const DataRank_type cst_DataRankInvalid = INT64_MIN;

#ifndef ZRESOURCE_CPP
extern Resourceid_type ResourceId;
#endif // ZRESOURCE_CPP

Resourceid_type getUniqueResourceId();
/**
 * @brief The ZResource class Anything that could be searched for AND/OR locked is a ZResource and has a unique ZIdentity::id number.
 *  ZResource is characterized by Entity (Entity_type) application dependant code that refines what kind of resource is designated.
 *
 *  id and Entity are mandatory to fully define a ZResource.
 */
class ZResource
//    : public  ZIdentity /* unique resource id regardless entity type - initially set to invalid */
{
public:
    Resourceid_type id = cst_ResourceInvalid;
    ZEntity_type Entity = cst_EntityInvalid; /* entity type : defined by application */
 //   DataRank_type DataRank = -1; /* reference to data rank (either ZAM rank or MasterFile rank */

    ZResource() {clear();}
    ZResource(Resourceid_type pResourceid, ZEntity_type pEntity)
    {
        memset(this, 0, sizeof(ZResource));
        id = pResourceid;
        Entity = pEntity;
    }
    ZResource( ZEntity_type pEntity)
    {
        Entity = pEntity;
    }
    ZResource(const ZResource &pIn) { _copyFrom(pIn); }
    ZResource(const ZResource &&pIn) { _copyFrom(pIn); }

    ZResource &_copyFrom(const ZResource &pIn)
    {
        memset(this, 0, sizeof(ZResource));
        id = pIn.id;
        Entity = pIn.Entity;
//        DataRank = pIn.DataRank;
        return *this;
    }
    ZResource &_copyFrom(const ZResource &&pIn)
    {
        memset(this, 0, sizeof(ZResource));
        id = pIn.id;
        Entity = pIn.Entity;
 //       DataRank = pIn.DataRank;
        return *this;
    }
    ZResource &operator=(ZResource &pIn) { return _copyFrom(pIn); }
    //    ZResource(Entity_type pEntity, utfcodeString &pCode) { pEntity = pCode.toLong(); }

    friend class ZLockManager;
private:
    ZResource &getNew()
    {
        id = getUniqueResourceId();
        return *this;
    }

    static ZResource &getNew(ZEntity_type pEntity)
    {
        ZResource wResource;
        wResource.id = getUniqueResourceId();
        wResource.Entity = pEntity;
        return wResource;
    }
public:
    //    ZResourceid (ZEntity_type pEntity, Docid_type &pDocid) {fromDocid(pEntity,pDocid);}
    utfcodeString tocodeString (void)
        {
            utfcodeString pString ;
            pString.sprintf("%08lX-%08lX",Entity,id);
            return pString;
        }

    void clear(void)
    {
        memset(this, 0, sizeof(ZResource));
        id = cst_ResourceInvalid;
        Entity = 0;
//        DataRank = 0;
    }

    bool isValid() { return id != cst_ResourceInvalid;}
    bool isInvalid() { return id == cst_ResourceInvalid; }

    void setInvalid()
    {
        Entity = cst_EntityInvalid;
        id = cst_ResourceInvalid;
    }


    ZResource &operator=(const ZResource &pIn) { return _copyFrom(pIn); }
    ZResource &operator=(const ZResource &&pIn) { return _copyFrom(pIn); }

    bool operator==(const ZResource &pIn) { return compare(pIn)==0; }

    /** @brief compare() This routine is mandatory for ZResource to be used as ZMIndex key component */
    int compare(const ZResource& pKey2) const {
            if (id == pKey2.id) {
                if (Entity == pKey2.Entity)
                    return 0;
                if (Entity < pKey2.Entity)
                    return -1;
                return 1;
            }
            if (id < pKey2.id) {
                return -1;
            }
            return 1;
        }
/*
    static int compare(const ZResource& pKey1,const ZResource& pKey2)
        {
            if (pKey1.id == pKey2.id) {
                if (pKey1.Entity == pKey2.Entity)
                    return 0;
                if (pKey1.Entity < pKey2.Entity)
                    return -1;
                return 1;
            }
            if (pKey1.id < pKey2.id) {
                return -1;
            }
            return 1;
        }
*/

    utf8String toXML(int pLevel);
    utf8String toStr() const;
    utf8String toHexa() const;

    /** @brief toKey() gets only ZResource::id (int this order) converted to universal format (Nota Bene : resource id is unique)*/
    ZDataBuffer toKey();
    /** @brief  toFullKey() gets ZResource::Entity+ZResource::id (int this order) converted to universal format */
    ZDataBuffer toFullKey();
};

#endif // ZRESOURCE_H
