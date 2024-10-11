#include "zdomainobject.h"
#include "zdomainbroker.h"

#include <ztoolset/uristring.h>


namespace zbs {


ZDomainObject::ZDomainObject() {}

ZDomainObject&
ZDomainObject::_copyFrom(const ZDomainObject& pIn)
{
    Path.clear();
    for (long wi = 0 ; wi < pIn.Path.count() ; wi ++ ) {
        Path.push(pIn.Path[wi]);
    }
    ZDomainPath::_copyFrom(pIn);
    return *this;
}

ZStatus
ZDomainObject::constructFromString(const utf8VaryingString& pDomainPath, ZaiErrors* pErrorLog)
{
    uriString wPath;
    clear() ;
    uriString wFileName;

    ZStatus wSt=DomainBroker._breakDomain(pDomainPath,Path,wFileName,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return wSt;


    /* for a local path */

    for (int wi=0; wi < Path.count();wi++) {
        if (Path[wi]->Type & ZDOM_Absolute) {
            wPath = Path[wi]->Content;
        }
        else {
            wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
        }
    }// for


    if (wFileName.isEmpty()&& (Path.count())) {
        ZDomainPath::_copyFrom(*Path.last());
        Path.pop();
        return ZS_SUCCESS;
    }

    Name = Content = wFileName ;
    Type = deducePossibleType(wFileName);

    return ZS_SUCCESS ;

} // ZDomainObject::constructFromString



ZStatus
ZDomainObject::loadContent( ZDataBuffer& pContent, ZaiErrors* pErrorLog) const
{

    uriString wFilePath = getFullPath(pErrorLog);
    return wFilePath.loadContent(pContent);
}

ZStatus
ZDomainObject::loadUtf8(utf8VaryingString& pOutText,ZaiErrors* pErrorLog) const
{
    uriString wFilePath = getFullPath(pErrorLog);
    return wFilePath.loadUtf8(pOutText);
}

void
ZDomainObject::constructFromDP(ZDomainPath* pDomainPath, ZaiErrors* pErrorLog)
{
    if (pDomainPath->Type & ZDOM_ObjectMask) {
        ZDomainPath::_copyFrom(*pDomainPath) ;
    }
    else {
        Path.push_front(pDomainPath);
    }
    ZDomainPath* wDP=pDomainPath->Father;
    while (wDP) {
        Path.push_front(wDP);
        wDP = wDP->Father;
    }
}


/** @brief constructFromString constructs a domain object from an input string pDomainPath using domain path base.
 *   Notation :
 *
 *  1-  <domain node1>.<domain node2>.../<file path spec>
 *
 *      leading domain node list before '/' sign mentions the actual path according rules concerning absolute / relative appliying to domain nodes
 *      Pointers to each of these domain node will be stored locally to current object in order to further construct its physical path.
 *
 *      Trailing substring after '/' mentions the actual file specification (including its extension)
 *      resulting object with embed a domain path whose name field will be identical to its content field i.e. <file path spec>
 *
 *      With this notation, resulting object has Type of ZDOM_File.
 *
 *  2-  <domain node1>.<domain node2>....<domain nodex>
 *      leading domain node list until <domain nodex> will be used to build path according rules concerning absolute / relative appliying to domain nodes
 *      last node <domain nodex>
 *
 *      With this notation, resulting object has Type inherited from <domain nodex>.
 *
 */

uriString
ZDomainObject::getPathOnly(ZaiErrors* pErrorLog)
{
    uriString wPath;

    /* for a local path */
    for (int wi=0; wi < Path.count();wi++) {
        if (Path[wi]->Type & ZDOM_Path) {
            if (Path[wi]->Type & ZDOM_Absolute) {
                wPath = Path[wi]->Content;
            }
            else {
                wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
            }
            continue;
        }
        if (Path[wi]->Type & ZDOM_Icon) {
            wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
            break;
        }
        if (Path[wi]->Type & ZDOM_Image) {
            wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
            break;
        }
    }// for

    return wPath;

} // ZDomainBroker::getPath

uriString
ZDomainObject::getFullPath(ZaiErrors* pErrorLog) const
{
    uriString wPath;

    /* if path is absolute : no need to build another path. Use it */
    if ((ZDomainPath::Type & ZDOM_Absolute) &&(!ZDomainPath::Name.isEmpty())){
        return ZDomainPath::Content;
    }


    /* build local path */
    for (int wi=0; wi < Path.count();wi++) {
        if (Path[wi]->Type & ZDOM_Absolute) {
            wPath = Path[wi]->Content;
        }
        else {
            wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
        }

    }// for

    if (ZDomainPath::isValid()&& !Content.isEmpty()) {
        /* path is relative and needs to be contructed */
        wPath.addWithLeadingCondDirDelim(ZDomainPath::Content);
    }

    return wPath;

} // ZDomainObject::getPath


ZDomain_type deducePossibleType(const uriString& pFileName)
{

    if (pFileName.isEmpty())
        return ZDOM_Nothing;


    if (pFileName.possiblyIconSource())
        return ZDOM_Icon;

    if (pFileName.possiblyText())
        return ZDOM_TextFile;
    if (pFileName.possiblyHtml())
        return ZDOM_HtmlFile;
    if (pFileName.possiblyPdf())
        return ZDOM_PdfFile;
    if (pFileName.possiblyXml())
        return ZDOM_XmlFile;
    if (pFileName.possiblyOdf())
        return ZDOM_OdfFile;
    if (pFileName.possiblyZMF())
        return ZDOM_MasterFile;
    if (pFileName.possiblyZRH())
        return ZDOM_HeaderFile;

    return ZDOM_Other;

}

ZFileSelection
inducePossibleType(ZDomain_type pType )
{
    ZDomain_type wType = pType & ZDOM_ObjectMask;
//    ZArray<utf8VaryingString> wPossibleExt;
    ZFileSelection wPossibleExt;
    if (wType == ZDOM_ObjectMask) {
        return wPossibleExt << "*.*";
    }

    switch (pType & ZDOM_ObjectMask) {
    case ZDOM_Icon:
    case ZDOM_Image:
        return wPossibleExt << "*.png" << "*.jpeg" << "*.jpg" << "*.gif";
    case ZDOM_TextFile:
        return wPossibleExt << "*.txt" << "*";
    case ZDOM_HtmlFile:
        return wPossibleExt << "*.html" << "*.htm";
    case ZDOM_XmlFile:
        return wPossibleExt << "*.xml";
    case ZDOM_OdfFile:
        return wPossibleExt << "*.odf" <<  "*.odg" <<  "*.ods" << "*.odp";
    case ZDOM_MasterFile:
        return wPossibleExt << "*.zmf";
    case ZDOM_HeaderFile:
        return wPossibleExt << "*.zrh";
    case ZDOM_Executable:
        return wPossibleExt << "*." << "*.exe" << "*.com" ;
    default:
        return wPossibleExt << "*.*";
    }

}


} // namespace zbs
