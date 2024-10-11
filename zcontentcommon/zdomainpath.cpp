#include "zdomainpath.h"

ZDomainPath::ZDomainPath() {}
ZDomainPath::ZDomainPath(uint32_t pType, const utf8VaryingString &pName, ZDomainPath* pFather, int pPos)
{
    Type=pType;
    Name = pName;
    Father=pFather;
    if (pPos<0)
        Father->Inferiors.push(this);
    else
        Father->Inferiors.insert(this,size_t(pPos));
}

void ZDomainPath::clear()
{
    while (Inferiors.count()>0)
        delete (Inferiors.popR());
    Name.clear();
    Content.clear();
    Type = ZDOM_Nothing;
}

ZDomainPath&
ZDomainPath::_copyFrom(const ZDomainPath& pIn)
{
    clear();

    Name = pIn.Name;
    Content = pIn.Content;
    ToolTip = pIn.ToolTip;
    Type = pIn.Type;

    for (int wi=0; wi < pIn.Inferiors.count() ; wi++ )  {
        ZDomainPath* wToPush = new ZDomainPath(*pIn.Inferiors[wi]);
        wToPush->Father = this;
        Inferiors.push(wToPush);
    }
    return *this;
}

ZDomainPath*
ZDomainPath::search(const utf8VaryingString& pDPName)
{
    if (pDPName == Name)
        return this;
    for (int wi=0; wi < Inferiors.count() ; wi++) {
        return Inferiors[wi]->search(pDPName);
    }
    return nullptr;
}

ZDomainPath*
ZDomainPath::searchInferiorPathByName(const utf8VaryingString& pDPName)
{
    for (int wi=0; wi < Inferiors.count() ; wi++) {
        if ( Inferiors[wi]->Name == pDPName )
            return Inferiors[wi];
    }
    return nullptr;
}

ZDomainPath*
ZDomainPath::_searchPath(const utf8VaryingString& pDPName)
{
    if (Name == pDPName )
        return this;
    for (int wi = 0; (wi < Inferiors.count()) && (Inferiors[wi]->_searchPath(pDPName)) ; wi++ )
        if ( Inferiors[wi]->Name == pDPName )
            return Inferiors[wi];
    return nullptr;
}


bool
ZDomainPath::removeInferior(ZDomainPath* pToRemove)
{
    for (int wi=0 ; wi < Inferiors.count() ; wi++ ) {
        if (pToRemove == Inferiors[wi]) {
            Inferiors.erase(wi);
            return true;
        }
    }
    return false;
}

