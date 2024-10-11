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
    utf8VaryingString wFileName;

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
        /*
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
            if (Path[wi]->Type & ZDOM_Absolute) {
                wPath = Path[wi]->Content;
            }
            else {
                wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
            }
            break;
        }
        if (Path[wi]->Type & ZDOM_Image) {
            if (Path[wi]->Type & ZDOM_Absolute) {
                wPath = Path[wi]->Content;
            }
            else {
                wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
            }
            break;
        }
        if (Path[wi]->Type & ZDOM_Executable) {
            if (Path[wi]->Type & ZDOM_Absolute) {
                wPath = Path[wi]->Content;
            }
            else {
                wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
            }
            break;
        }
        if (Path[wi]->Type & ZDOM_PdfFile) {
            if (Path[wi]->Type & ZDOM_Absolute) {
                wPath = Path[wi]->Content;
            }
            else {
                wPath.addWithLeadingCondDirDelim(Path[wi]->Content);
            }
            break;
        }
*/
    }// for


    if (wFileName.isEmpty()&& (Path.count())) {
        ZDomainPath::_copyFrom(*Path.last());
        Path.pop();
        //        Type = Path.last()->Type;
        return ZS_SUCCESS;
    }


    Name = Content = wFileName ;
    Type = ZDOM_File;

    return ZS_SUCCESS ;

} // ZDomainObject::constructFromString


ZStatus
ZDomainObject::loadContent( ZDataBuffer& pContent, ZaiErrors* pErrorLog) const
{

    uriString wFilePath = getFullPath(pErrorLog);
    return wFilePath.loadContent(pContent);
}

ZStatus
ZDomainObject::loadUtf8(utf8VaryingString& pOutText,ZaiErrors* pErrorLog)
{
    uriString wFilePath = getFullPath(pErrorLog);
    return wFilePath.loadUtf8(pOutText);
}


} // namespace zbs
