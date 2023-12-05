#ifndef ZSEARCHSYMBOL_H
#define ZSEARCHSYMBOL_H

#include <ztoolset/zarray.h>
#include <ztoolset/utfvaryingstring.h>
#include "zsearchoperand.h"
#include "zsearchparsertype.h"
#include "ztoolset/zstatus.h"

namespace zbs {
class  ZSearchSymbol : public ZSearchLiteral
{
public:
    ZSearchSymbol() ;

    ZSearchSymbol(const ZSearchSymbol& pIn) {_copyFrom(pIn);}

    ZSearchSymbol& _copyFrom(const ZSearchSymbol& pIn) {
        //      Symbol=pIn.Symbol;
        ZSearchLiteral::_copyFrom(pIn);
        //      Path=pIn.Path;
        return *this;
    }

    ZSearchOperandBase getOperandBase() {return *this;}

    utf8VaryingString getName() {return FullFieldName;}
    void setName(utf8VaryingString& pName)  {FullFieldName = pName;}

    ZSearchSymbol& operator=(const ZSearchSymbol& pIn) {return _copyFrom(pIn);}

    void setLiteral(ZSearchLiteral* pLit)
    {
        ZSearchLiteral::_copyFrom(*pLit);
    }
    void setLiteral(ZSearchLiteral& pLit)
    {
        ZSearchLiteral::_copyFrom(pLit);
    }

    utf8VaryingString toXml(int pLevel);
    ZStatus fromXml(zxmlElement *pSymbolRootNode,
                    const utf8VaryingString &pRootName = "symbolitem",
                    ZaiErrors *pErrorLog = nullptr);

    //    utf8VaryingString Symbol;
    //    uriString         Path;
};


class ZSearchSymbolList : public ZArray<ZSearchSymbol>
{
public:
    ZSearchSymbol getSymbol(const utf8VaryingString& pNameToSearch)
    {
        for (int wi=0;wi < count();wi++)
            if (pNameToSearch==Tab(wi).getName()) {
                return Tab(wi);
            }
        return ZSearchSymbol();
    }
    bool exists(const utf8VaryingString& pNameToSearch)
    {
        for (int wi=0;wi < count();wi++)
            if (pNameToSearch==Tab(wi).getName()) {
                return true ;
            }
        return false;
    }


    utf8VaryingString toXml() ;
    ZStatus fromXml(const utf8VaryingString &pXmlContent, ZaiErrors *pErrorLog);

    ZStatus XmlSave(const uriString &pXmlFile, ZaiErrors *pErrorLog);
    ZStatus XmlLoad(const uriString &pXmlFile, ZaiErrors *pErrorLog);
};

} // namespace zbs

#endif // ZSEARCHSYMBOL_H
