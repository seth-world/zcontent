#ifndef ZDOMAINOBJECT_H
#define ZDOMAINOBJECT_H

#include "zdomainpath.h"

namespace zbs {

class ZDomainObject : public ZDomainPath
{
public:
    ZDomainObject();
    ZDomainObject(const ZDomainObject& pIn) { _copyFrom(pIn); }
    ZDomainObject(ZDomainPath* pDP,ZaiErrors* pErrorLog) {constructFromDP(pDP,pErrorLog);}

    ZDomainObject& _copyFrom(const ZDomainObject& pIn);

    ZDomainObject& operator = (const ZDomainObject& pIn) { return _copyFrom(pIn); }

    ZArray<ZDomainPath*> Path;

    void clear()
    {
        while (Path.count())
            Path.pop();
        ZDomainPath::clear();
    }

    bool isValid() {return ZDomainPath::isValid();}

    ZStatus constructFromString(const utf8VaryingString& pDomainPath, ZaiErrors* pErrorLog);
    void constructFromDP(ZDomainPath* pDomainPath, ZaiErrors* pErrorLog);
    uriString getPathOnly(ZaiErrors* pErrorLog);
    uriString getFullPath(ZaiErrors *pErrorLog) const;

    ZStatus loadUtf8(utf8VaryingString& pOutText,ZaiErrors* pErrorLog);
    ZStatus loadContent(ZDataBuffer& pOutData,ZaiErrors* pErrorLog) const;

}; // ZDomainObject


} // namespace zbs

#endif // ZDOMAINOBJECT_H
