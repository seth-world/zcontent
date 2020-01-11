#ifndef ZRESOURCE_CPP
#define ZRESOURCE_CPP

#include <zcontentcommon/zresource.h>
#include <zxml/zxmlprimitives.h>
#include <zindexedfile/zdatatype.h>

Resourceid_type ResourceId = 0;

Resourceid_type getUniqueResourceId() {return ResourceId++;}

utf8String ZResource::toXML(int pLevel)
{
    utf8String wContent = fmtXMLnode("zresource", pLevel);

    wContent += fmtXMLint64("id", id, pLevel);
    wContent += fmtXMLint64("entity", Entity, pLevel);
//    wContent += fmtXMLint64("datarank", DataRank, pLevel);
    return wContent;
}

utf8String ZResource::toStr() const
{
    utf8String wContent;
//    wContent.sprintf("%ld#%ld#%ld", id, Entity, DataRank);
    wContent.sprintf("%ld#%ld", Entity, id);
    return wContent;
}
utf8String ZResource::toHexa() const
{
    utf8String wContent;
    //    wContent.sprintf("%ld#%ld#%ld", id, Entity, DataRank);
    wContent.sprintf("%l08Xu#%l08Xd", Entity, id);
    return wContent;
}

ZDataBuffer ZResource::toKey()
{
    ZDataBuffer wOutValue;
    _castAtomicValue_T<ZEntity_type>(Entity, ZType_U64, wOutValue);
    return wOutValue;
}

ZDataBuffer ZResource::toFullKey()
{
    ZDataBuffer wOutValue, wZDB;
    _castAtomicValue_T<ZEntity_type>(Entity, ZType_U64, wOutValue);
    _castAtomicValue_T<Resourceid_type>(id, ZType_S64, wZDB);
    wOutValue += wZDB;
//    _castAtomicValue_T<Identity_type>(DataRank, ZType_S64, wOutValue);
//    wOutValue += wZDB;
    return wOutValue;
}

#endif // ZRESOURCE_CPP
