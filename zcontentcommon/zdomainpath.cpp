#include "zdomainpath.h"
#include <zxml/zxmlprimitives.h>
#include "zdomainbroker.h"

namespace zbs {

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
    Father = pIn.Father;
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

uriString
ZDomainPath::constructFullPhysicalPath()
{
    ZArray<ZDomainPath*> wDPList;
    ZDomainPath* wDP = this;
    wDPList.push(wDP);
    while (wDP->Father!=nullptr) {
        wDP = wDP->Father;
        wDPList.push(wDP);
    }
    uriString wPhysicalPath = wDPList.popR()->Content;
    while (wDPList.count()>0) {
        ZDomainPath* wDP = wDPList.popR() ;
        if (wDP->isAbsolute())
            wPhysicalPath = wDP->Content;
        else
            wPhysicalPath.addWithLeadingCondDirDelim(wDP->Content);
    }
    return wPhysicalPath;
}
void
ZDomainPath::constructPath(uriString& pOutPath,utf8VaryingString& pOutObject)
{
    pOutObject.clear();
    pOutPath.clear();

    if (isAbsolute()) {
        if (isPath())
            pOutPath = Content;
        else {
            pOutObject = Content;
        }
        return;
    }


    ZArray<ZDomainPath*> wDPList;
    ZDomainPath* wDP = this;
    if (!wDP->isPath())
        pOutObject = wDP->Content;
    else {
        wDPList.push(wDP);
    }

    wDP = wDP->Father;
    while (!wDP->isRoot()) {
        wDPList.push(wDP);
        wDP = wDP->Father;
    }
    pOutPath = wDPList.popR()->Content;
    while (wDPList.count() > 0) {
        wDP = wDPList.popR();
        if (!wDP->Content.isEmpty() && wDP->isPath()) {
            if (wDP->isRelative())
                pOutPath.addWithLeadingCondDirDelim(wDP->Content);
            else
                pOutPath = wDP->Content;
        }
    }
    return;
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

utf8VaryingString
ZDomainPath::getHierarchy()
{
    if (Father==nullptr)
        return utf8VaryingString();

    utf8VaryingString wReturn ;

    wReturn = Father->getHierarchy();

    if (wReturn.isEmpty())
        return Name;
    wReturn.addChar( '.' );
    wReturn += Name ;
    return wReturn ;
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



bool
ZDomainPath::hasViewer() {

    uint32_t wObjectType = Type & ZDOM_ObjectMask;

    /* manage built in embedded viewers */
    if ( (wObjectType == ZDOM_Icon)
        || (wObjectType == ZDOM_Image)
        ||(wObjectType == ZDOM_TextFile)
        ||(wObjectType == ZDOM_HtmlFile)
        ||(wObjectType == ZDOM_XmlFile))
        return true;
    utf8VaryingString wDomainChain = "general.viewer.linux.";
    int wi = 0;
    for (;  TypeDecodeTable[wi].Type != ZDOM_End ; wi ++)
        if (TypeDecodeTable[wi].Type == wObjectType)
            break;
    if (TypeDecodeTable[wi].Type == ZDOM_End)
        return false ;

    wDomainChain += TypeDecodeTable[wi].TypeString;

    return DomainBroker.domainChainExists(wDomainChain);

} // ZDomainPath::hasViewer

bool
ZDomainPath::hasEditor() {

    uint32_t wObjectType = Type & ZDOM_ObjectMask;

    utf8VaryingString wDomainChain = "general.editor.linux.";
    int wi = 0;
    for (;  TypeDecodeTable[wi].Type != ZDOM_End ; wi ++)
        if (TypeDecodeTable[wi].Type == wObjectType)
            break;
    if (TypeDecodeTable[wi].Type == ZDOM_End)
        return false ;

    wDomainChain += TypeDecodeTable[wi].TypeString;

    return DomainBroker.domainChainExists(wDomainChain);

} // ZDomainPath::hasEditor


ZDomainPathRoot* ZDomainPath::getRoot()
{
    ZDomainPath* wDP = this;
    while (wDP->Father!=nullptr)
        wDP = wDP->Father;
    return static_cast<ZDomainPathRoot*>(wDP);
}

void
ZDomainPath::setHasChanged(bool pOnOff)
{
    getRoot()->HasChanged=pOnOff;
}
bool
ZDomainPath::hasChanged()
{
    return getRoot()->HasChanged ;
}

utf8VaryingString
ZDomainPath::toXml(int pLevel, ZaiErrors *pErrorLog)
{
    utf8VaryingString wReturn;
    if (Temporary)
        return wReturn;
    wReturn = fmtXMLnode("domain-item",pLevel);
    if (!Name.isEmpty())
        wReturn += fmtXMLchar("name",Name.toCChar(),pLevel+1);
    if (!Content.isEmpty())
        wReturn += fmtXMLchar("content",Content.toCChar(),pLevel+1);
    if (!ToolTip.isEmpty())
        wReturn += fmtXMLchar("tooltip",ToolTip.toCChar(),pLevel+1);

    //    wReturn += fmtXMLchar("type",decode_ZDomain_type( Type ),pLevel+1);

    wReturn += fmtXMLuint64Hexa("type",Type ,pLevel+1);
    fmtXMLaddInlineComment(wReturn,decode_ZDomain_type( Type ));

    for (int wi=0 ; wi < Inferiors.count() ; wi++ ) {
        wReturn += Inferiors[wi]->toXml(pLevel+1,pErrorLog);
    }
    wReturn += fmtXMLendnode("domain-item",pLevel);
    return wReturn;
} // ZDomainPath::toXmlDomainItem



} // namespace zbs
