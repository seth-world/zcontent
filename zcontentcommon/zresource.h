#ifndef ZRESOURCE_H
#define ZRESOURCE_H

#include <stdint.h>

//#include <zcontentcommon/zidentity.h>
#include <ztoolset/zutfstrings.h> // for utfcodeString
#include <zxml/zxml.h>

#include <ztoolset/zaierrors.h>

typedef int64_t Resourceid_type;
typedef uint64_t ZEntity_type;
typedef int64_t DataRank_type;


const ZEntity_type cst_EntityInvalid=0;
const Resourceid_type cst_ResourceInvalid = INT64_MIN;
const Resourceid_type cst_PureEntity = -1;
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
/* no memory alignment to avoid undesired bytes with garbage between components
 * Remark : memset() cannot be used for ZResource object because it has virtual methods and will overwrite VTable content
 */
#pragma pack(push)
#pragma pack(0)         // no memory alignment
    ZEntity_type Entity = cst_EntityInvalid; /* entity type : defined by application */
    Resourceid_type id = cst_ResourceInvalid;
#pragma pack(pop) // end no memory alignment

 //   DataRank_type DataRank = -1; /* reference to data rank (either ZAM rank or MasterFile rank */

    ZResource() {clear();}
    virtual ~ZResource() {} /* mandatory because ZResource has virtual methods */

    ZResource(const Resourceid_type pResourceid, const ZEntity_type pEntity)
    {
// NO: Do not use   memset(this, 0, sizeof(ZResource));
        id = pResourceid;
        Entity = pEntity;
    }
    ZResource(const ZEntity_type pEntity)
    {
        Entity = pEntity;
    }
    ZResource(const ZResource &pIn) { _copyFrom(pIn); }
    ZResource(const ZResource &&pIn) { _copyFrom(pIn); }

    ZResource &_copyFrom(const ZResource &pIn)
    {
// NO: Do not use   memset(this, 0, sizeof(ZResource));
        id = pIn.id;
        Entity = pIn.Entity;
//        DataRank = pIn.DataRank;
        return *this;
    }
    ZResource &_copyFrom(const ZResource &&pIn)
    {
// NO: Do not use    memset(this, 0, sizeof(ZResource));
        id = pIn.id;
        Entity = pIn.Entity;
 //       DataRank = pIn.DataRank;
        return *this;
    }

    void setPureEntity(const ZEntity_type pEntity)
    {
        id = cst_PureEntity;
        Entity = pEntity;
    }
    bool isPureEntity(void) const { return (id == cst_PureEntity); }
    //    ZResource(Entity_type pEntity, utfcodeString &pCode) { pEntity = pCode.toLong(); }


protected:
    static ZResource getNew(const ZEntity_type pEntity)
    {
        ZResource wR;
        wR.id = getUniqueResourceId();
        wR.Entity=pEntity;
        return wR;
    }

public:
    //    ZResourceid (ZEntity_type pEntity, Docid_type &pDocid) {fromDocid(pEntity,pDocid);}
    utfcodeString tocodeString (void)
        {
            utfcodeString pString ;
            pString.sprintf("0x%08lX-0x%08lX",Entity,id);
            return pString;
        }

    void clear(void)
    {
//        memset(this, 0, sizeof(ZResource));
        id = cst_ResourceInvalid;
        Entity = 0;
//        DataRank = 0;
    }

    bool isValid() const { return id != cst_ResourceInvalid;}
    bool isInvalid() const  { return id == cst_ResourceInvalid; }

    void setInvalid()
    {
        Entity = cst_EntityInvalid;
        id = cst_ResourceInvalid;
    }


    ZResource &operator=(const ZResource &pIn) { return _copyFrom(pIn); }
    ZResource &operator=(const ZResource &&pIn) { return _copyFrom(pIn); }

    bool operator==(const ZResource &pIn) const { return compare(pIn)==0; }

    /** @brief compare() This routine is mandatory for ZResource to be used as ZMIndex key component
     *         Nota Bene : it has to be const
     */
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

    /* virtual methods : to be replaced by derived classes own methods */
    virtual utf8String toXml(int pLevel);
    virtual int fromXml(zxmlElement *pRootNode, const char *pChildName,ZaiErrors *pErrorlog);

    static int fromXmltoHexa(zxmlElement *pRootNode,
            const char *pChildName,
            utfcodeString &wCode,
            ZaiErrors *pErrorlog);

    virtual utf8String toStr() const;
    virtual utf8String toHexa() const;

    /** @brief toKey() gets only ZResource::id (int this order) converted to universal format (Nota Bene : resource id is unique)*/
    ZDataBuffer toKey();
    /** @brief  toFullKey() gets ZResource::Entity+ZResource::id (int this order) converted to universal format */
    ZDataBuffer toFullKey();

    unsigned char* _export(unsigned char*& pBuffer,size_t & pSize) const;
    ZDataBuffer _export() const;
    /** @brief _import imports from serialized data pUniversalPtr. Updates pointer to first byte after imported data*/
    size_t      _import(unsigned char* &pUniversalPtr);

    /**
     * @brief _exportURF
     *  URF format for ZResource :  size if fixed and known
     *
     *  <ZType_Resource>
     *  <ZEntity_type>
     *  <Resourceid_type>
     *
     * @param pBuffer
     * @param pSize
     * @return
     */
    unsigned char* _exportURF(unsigned char*& pBuffer,size_t & pSize) const;
    ZDataBuffer *_exportURF(ZDataBuffer *pReturn) const;
    /** @brief _import imports from serialized data pUniversalPtr. Updates pointer to first byte after imported data*/
    ssize_t      _importURF(unsigned char* &pUniversalPtr);
};

#endif // ZRESOURCE_H
