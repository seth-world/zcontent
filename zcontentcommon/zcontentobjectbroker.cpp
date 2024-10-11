#include "zcontentobjectbroker.h"

#include <zxml/zxmlprimitives.h>
#include <QIcon>
#include <zqt/zqtwidget/zqlabel.h>

namespace zbs {

class ZDomainBroker DomainBroker ;
class ZContentObjectBroker ContentObjectBroker;


ZContentObjectBroker::ZContentObjectBroker() {}


ZQLabel*
ZContentObjectBroker::labelFactory(const utf8VaryingString& pImageDomainSpec, ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pImageDomainSpec,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return nullptr;

    uriString wIconPath = wObject.getPath(pErrorLog);
     QPixmap wPix(wIconPath.toCChar());
    if (wPix.isNull()) {
        pErrorLog->errorLog("ZContentObjectBroker::labelFactory Cannot create pixmap from image <%s> \n"
                            "             resulting path <%s> ",
                            pImageDomainSpec.toCChar(),wIconPath.toCChar());
        if (!wIconPath.exists()) {
            pErrorLog->errorLog("         image file does not exists");
        }
    }
    ZQLabel* wLabel=new ZQLabel;
    wLabel->setPixmap(wPix);

    return wLabel;
}

QIcon
ZContentObjectBroker::iconFactory(const utf8VaryingString& pDomain,  ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pDomain,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return QIcon();

    uriString wIconPath = wObject.getPath(pErrorLog);

 /*   ZStatus wSt = DomainBroker.constructPath(pDomain,wIconPath,pErrorLog);
    wIconPath.addWithLeadingCondDirDelim(pImageFileName);
*/
    QIcon pIcon = QIcon(wIconPath.toCChar());
    if (pIcon.isNull()) {
        pErrorLog->errorLog("ZContentObjectBroker::iconFactory Cannot create icon from image\n"
                            "             domain <%s> name <%s>\n"
                            "             resulting path <%s> ",
                            pDomain.toCChar(),wObject.Name.toCChar(),wIconPath.toCChar());
        if (!wIconPath.exists()) {
            pErrorLog->errorLog("         image file does not exists");
        }
    }

    return pIcon;
}
/*
QIcon
ZContentObjectBroker::iconFactory(const utf8VaryingString& pDomain,  ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.construct(pDomain,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return QIcon();

    uriString wIconPath = wObject.getPath(pErrorLog);

    QIcon pIcon = QIcon(wIconPath.toCChar());
    if (pIcon.isNull()) {
        pErrorLog->errorLog("ZContentObjectBroker::iconFactory Cannot create icon from image\n"
                            "             domain <%s> name <%s>\n"
                            "             resulting path <%s> ",
                            pDomain.toCChar(),wObject.Name.toCChar(),wIconPath.toCChar());
        if (!wIconPath.exists()) {
            pErrorLog->errorLog("         image file does not exists");
        }
    }

    return pIcon;
}
*/
/*
QIcon
ZContentObjectBroker::iconFactory(const utf8VaryingString& pImageDomainSpec, ZaiErrors* pErrorLog)
{
    QIcon wIcn;
    iconFactory( pImageDomainSpec,wIcn, pErrorLog);
    return wIcn;
}
*/
QImage
ZContentObjectBroker::imageFactory(const utf8VaryingString& pDomain, ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pDomain,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return QImage();

    uriString wIconPath = wObject.getPath(pErrorLog);

    /*   ZStatus wSt = DomainBroker.constructPath(pDomain,wIconPath,pErrorLog);
    wIconPath.addWithLeadingCondDirDelim(pImageFileName);
*/
    QImage pIcon = QImage(wIconPath.toCChar());
    if (pIcon.isNull()) {
        pErrorLog->errorLog("ZContentObjectBroker::iconFactory Cannot create image from image file\n"
                            "             domain <%s> name <%s>\n"
                            "             resulting path <%s> ",
                            pDomain.toCChar(),wObject.Name.toCChar(),wIconPath.toCChar());
        if (!wIconPath.exists()) {
            pErrorLog->errorLog("         image file does not exists");
        }
    }

    return pIcon;
}
/*
ZStatus
ZContentObjectBroker::imageFactory(const utf8VaryingString& pDomain, const utf8VaryingString& pImageFileName, QImage& pImage, ZaiErrors* pErrorLog)
{
    uriString wIconPath;
    ZStatus wSt = DomainBroker.constructPath(pDomain,wIconPath,pErrorLog);
    wIconPath.addWithLeadingCondDirDelim(pImageFileName);
    pIcon = QImage(wIconPath.toCChar());
    if (pIcon.isNull()) {
        pErrorLog->errorLog("ZContentObjectBroker::imageFactory Cannot create image from image file\n"
                            "             domain <%s> name <%s>\n"
                            "             resulting path <%s> ",
                            pDomain.toCChar(),pImageFileName.toCChar(),wIconPath.toCChar());
        if (!wIconPath.exists()) {
            pErrorLog->errorLog("         image file does not exists");
        }
    }

    return ZS_SUCCESS;
}

QImage
ZContentObjectBroker::imageFactory(const utf8VaryingString& pImageDomainSpec, ZaiErrors* pErrorLog)
{
    QImage wImg;
    imageFactory( pImageDomainSpec,wImg, pErrorLog);
    return wImg;
}

ZStatus
ZContentObjectBroker::iconFactory(const utf8VaryingString& pImageDomainSpec,QIcon& pIcon, ZaiErrors* pErrorLog)
{
    uriString wIconPath;
    ZStatus wSt = DomainBroker.constructPath(pImageDomainSpec,wIconPath,pErrorLog);
    pIcon = QIcon(wIconPath.toCChar());
    if (pIcon.isNull()) {
        pErrorLog->errorLog("ZContentObjectBroker::iconFactory Cannot create icon from image\n"
                            "             domain spec <%s>\n"
                            "             resulting path <%s> ",
                            pImageDomainSpec.toCChar(),wIconPath.toCChar());
        if (!wIconPath.exists()) {
            pErrorLog->errorLog("         image file does not exists");
        }
    }

    return ZS_SUCCESS;
}
*/

ZStatus
ZContentObjectBroker::loadContent(const utf8VaryingString& pDomainFileSpec, ZDataBuffer& pContent, ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pDomainFileSpec,pErrorLog);
    if (wSt != ZS_SUCCESS)
        return wSt ;

    uriString wFilePath = wObject.getPath(pErrorLog);

    return wFilePath.loadContent(pContent);
}

ZStatus
ZContentObjectBroker::loadUtf8(const utf8VaryingString& pDomainFileSpec, utf8VaryingString& pUtfContent, ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pDomainFileSpec,pErrorLog);
    if (wSt != ZS_SUCCESS)
        return wSt ;

    uriString wFilePath = wObject.getPath(pErrorLog);
    return wFilePath.loadUtf8(pUtfContent);
}

void
ZDomainBroker::breakDomainPath(const utf8VaryingString& pDomainPath,ZArray<utf8VaryingString>& pDomainList)
{
    pDomainList.clear();
    utf8_t* wPtr = utfStrdup(pDomainPath.Data) ;
    utf8_t* wToBeFreed=wPtr;
    utf8_t* wPtrStart=wPtr;
    bool wFileNameFollows = false ;
    int wSize=0;
    while (*wPtr && (*wPtr != '/') ) {
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( (*wPtr=='.') ||(*wPtr != '/') ) {
            if (*wPtr == '/') {
                wFileNameFollows = true ;
            }
            *wPtr='\0';
            pDomainList.push(wPtrStart);
            wPtr++;
            wPtrStart=wPtr;
            wSize=0;
        }
    }
    if (wSize > 0)
        pDomainList.push(wPtrStart);
    free(wToBeFreed);
}// ZDomainBroker::breakDomainPath

void
ZDomainBroker::breakDomainPath(const utf8VaryingString& pDomainPath,ZArray<utf8VaryingString>& pDomainList,utf8VaryingString& pFileName)
{
    pDomainList.clear();
    pFileName.clear();
    utf8_t* wPtr = utfStrdup(pDomainPath.Data) ;
    utf8_t* wToBeFreed=wPtr;
    utf8_t* wPtrStart=wPtr;
    bool wFileNameFollows = false ;
    int wSize=0;
    while (*wPtr && (*wPtr != '/') && !wFileNameFollows ) {
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( (*wPtr=='.') ||(*wPtr == '/') ) {
            if (*wPtr == '/') {
                wFileNameFollows = true ;
            }
            *wPtr='\0';
            utf8VaryingString wDom = wPtrStart; // debug
            pDomainList.push(wPtrStart);
            wPtr++;
            wPtrStart=wPtr;
            wSize=0;
        }
    }
    if (wFileNameFollows)
        pFileName = wPtrStart;
    else {
        if (*wPtrStart != '\0') {
           pDomainList.push(wPtrStart);
        }
    }

    free(wToBeFreed);
}// ZDomainBroker::breakDomainPath


ZStatus
ZDomainBroker::_breakDomain(const utf8VaryingString& pInPath,
                                 ZArray<ZDomainPath*>& pDPList,
                                 utf8VaryingString& pFileName,
                                 ZaiErrors* pErrorLog)
{
    pDPList.clear();
    pFileName.clear();
    utf8_t* wToBeFreed = utfStrdup(pInPath.Data) ;
    utf8_t* wPtr = wToBeFreed ;
    utf8_t* wPtrStart=wPtr;

    int wSize=0;
    ZDomainPath* wDPCurrent = DomainRoot;

    bool wEndOfString = false;
    bool wFileNameFollows = false, wFileNameWillFollow = false ;
    while (!wEndOfString  ) {
        wPtrStart = wPtr;
        if (wFileNameFollows) {
            pFileName += wPtrStart ; /* take all remaining string after slash sign */
            break ;
        }
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( *wPtr == '\0' )
            wEndOfString = true;

        if (*wPtr == '/')
                wFileNameFollows = true ;

        *wPtr='\0';

        utf8VaryingString wDom = wPtrStart; // debug
        bool wFound = false;
        for (int wi=0; wi < wDPCurrent->Inferiors.count(); wi++) {
                if (wDPCurrent->Inferiors[wi]->Name == wDom) {
                    pDPList.push(wDPCurrent->Inferiors[wi]);
                    wPtr++;
                    wPtrStart=wPtr;
                    wSize=0;
                    wDPCurrent = wDPCurrent->Inferiors[wi];
                    wFound=true;
                    break ;
                }
        } // for
        if (wFound)
            continue;
        {
                pErrorLog->errorLog("ZDomainBroker::_breakDomain-E-INVNAME Domain name <%s> cannot be found within domain hierarchy <%s>\n"
                                    "                           Invalid domain path %s",
                                    wDom.toCChar(),wDPCurrent->getHierarchy().toCChar(),pInPath.toCChar());
                free(wToBeFreed);
                return ZS_INVNAME;
        }
    } //while (*wPtr && (*wPtr != '/') && !wFileNameFollows )

    /*
    if (wFileNameFollows)
        pFileName = wPtrStart;
    else {
        if (*wPtrStart != '\0') {
            utf8VaryingString wDom = wPtrStart; // debug
            bool wDPIdx = false;
            for (int wi=0; wi < wDPCurrent->Inferiors.count(); wi++) {
                if (wDPCurrent->Inferiors[wi]->Name == wDom) {
                    pDPList.push(wDPCurrent->Inferiors[wi]);
                    wPtr++;
                    wPtrStart=wPtr;
                    wSize=0;
                    wDPCurrent = wDPCurrent->Inferiors[wi];
                    wDPIdx=true;
                    break;
                }
            }
            if (!wDPIdx) {
                pErrorLog->errorLog("ZDomainBroker::_breakDomain-E-INVNAME Domain name <%s> cannot be found within domain hierarchy <%s>\n"
                                    "                           Invalid domain path %s",
                                    wDom.toCChar(),wDPCurrent->getHierarchy().toCChar(),pInPath.toCChar());
                free(wToBeFreed);
                return ZS_INVNAME;
            }
        }
    }
*/

    free(wToBeFreed);
    return ZS_SUCCESS;
}// ZDomainBroker::breakDomainPath


void
_constructPath(ZDomainPath* wDomainPath,uriString& pOutPath)
{
    if (pOutPath.isEmpty())
        pOutPath = wDomainPath->Content;
    else {
        if (wDomainPath->Type & ZDOM_Absolute) {
            pOutPath = wDomainPath->Content;
        }
        else {
            pOutPath.addConditionalDirectoryDelimiter();
            pOutPath += wDomainPath->Content;
        } // else
    }// else
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



void ZDomainBroker::clear()
{
//    while (DomainList.count()>0)
//        delete (DomainList.popR());
    if (DomainRoot!=nullptr)
        delete DomainRoot;
}


ZDomainBroker&
ZDomainBroker::_copyFrom(const ZDomainBroker& pIn)
{
 //   DomainList.clear();
 //   for (int wi=0 ; wi < pIn.DomainList.count() ; wi++)
 //       DomainList.push(new ZDomainPath(*pIn.DomainList[wi]));
    if (DomainRoot!=nullptr)
     delete DomainRoot;
    DomainRoot=nullptr;
//    if (pIn.DomainRoot!=nullptr)
    DomainRoot = new ZDomainPath(*pIn.DomainRoot);
    return *this;
}
#ifdef __COMMENT__
ZDomainPath*
ZDomainBroker::findFatherDomain(ZDomainPath* pPtrToSearch)
{
    for (int wi=0; wi < DomainList.count(); wi++) {
        if (pPtrToSearch==DomainList[wi])
            return nullptr ;  // if pPtrToSearch is found within first rank domains then no superior
    }
    return whoHasInferior(pPtrToSearch);
}

ZDomainPath*
ZDomainBroker::whoHasInferior(ZDomainPath* pPtrToSearch)
{
    for (int wi=0; wi < DomainList.count(); wi++) {
        for (int wj=0 ; wj < DomainList[wi]->Inferiors.count(); wj++) {
            if (pPtrToSearch==DomainList[wi]->Inferiors[wj]) {
                return DomainList[wi];
            }
        }
    }
    return nullptr;
}
#endif // __COMMENT__

ZDomainPath*
ZDomainBroker::_whoHasInferior(ZDomainPath* pDomain,ZDomainPath* pPtrToSearch)
{
    ZDomainPath* wReturn=nullptr;
    for (int wi=0 ; wi < pDomain->Inferiors.count(); wi++) {
        if (pPtrToSearch==pDomain->Inferiors[wi]) {
            return pDomain;
        }
        wReturn=_whoHasInferior(pDomain->Inferiors[wi],pPtrToSearch);
        if (wReturn !=nullptr)
            return wReturn;
    }
    return nullptr;
}// ZDomainBroker::_whoHasInferior


utf8VaryingString
ZDomainPath::getHierarchy()
{
    if (Father==nullptr)
        return utf8VaryingString();

    utf8VaryingString wReturn ;

    wReturn = Father->getHierarchy();
/*    if (wReturn.isEmpty())
        wReturn = "<root>";
*/
    if (wReturn.isEmpty())
        return Name;
    wReturn.addChar( '.' );
//    wReturn += "." ;
    wReturn += Name ;
    return wReturn ;
}


void
ZDomainPath::getPathPart(uriString& pOutPath,ZaiErrors* pErrorLog)
{
    while (true)
    {
        if (pOutPath.isEmpty()) {
            pOutPath = Content;
            return;
        }
        if (Type & (ZDOM_Absolute|ZDOM_Path)) {
            pOutPath.setChar(Delimiter);
            pOutPath += Content;
            return;
        }
        if (Type & (ZDOM_Relative|ZDOM_Path)) {
            pOutPath.addChar(Delimiter);
            pOutPath += Content;
            return;
        }
        if (Type & (ZDOM_Absolute|ZDOM_Icon)) {
            pOutPath += Content;
            return;
        }
        if (Type & (ZDOM_Relative|ZDOM_Icon)) {
            pOutPath.addConditionalDirectoryDelimiter();
            pOutPath += Content;
            return;
        }
        pErrorLog->errorLog("ZDomainPath::getPathPart-E-INVTYP Invalid/unrecognized Domain type <%X>",Type);
        break;
    }// while true
}




ZDomainPath*
ZDomainBroker::whoHasInferior(ZDomainPath* pPtrToSearch)
{
    return _whoHasInferior(DomainRoot,pPtrToSearch);
}

#ifdef __DEPRECATED__
ZStatus
ZDomainBroker::constructPath(const utf8VaryingString& pDomainPath,uriString& pOutPath,ZaiErrors* pErrorLog)
{
    ZArray<utf8VaryingString> wInDomainList;
    utf8VaryingString wFileName;

    breakDomainPath(pDomainPath,wInDomainList,wFileName);

    ZDomainPath* wDomainPath=nullptr;
    pOutPath.clear();
/*
    for (int wi=0; wi < DomainList.count() ; wi++) {
        if ( DomainList[wi]->Name==wInDomainList[0] ) {
            wDomainPath = DomainList[wi] ;
            break;
        }
    }
*/
    for (int wi=0; wi < DomainRoot->Inferiors.count() ; wi++) {
        if ( DomainRoot->Inferiors[wi]->Name==wInDomainList[0] ) {
            wDomainPath = DomainRoot->Inferiors[wi] ;
            break;
        }
    }

    if (wDomainPath==nullptr) {
        pErrorLog->errorLog("ZDomainBroker::getPath-E-NOTFOUND Domain named <%s> has not been found in declared domains.",wInDomainList[0].toCChar());
        return ZS_NOTFOUND;
    }

 //   _constructPath(wDomainPath,pOutPath);
    wDomainPath->getPathPart(pOutPath,pErrorLog);
    for ( int wInDomainIdx = 1 ; wInDomainIdx < wInDomainList.count()  ; wInDomainIdx++) {


        if (wDomainPath->Inferiors.count()==0) {  /* no more sub domains in domain list ? */

            if (wInDomainIdx < (wInDomainList.count() - 1 ) ) { /* input sub domain list not exhausted ? */
                utf8VaryingString wNotProcessed = wInDomainList[wInDomainIdx] ;
                wInDomainIdx++;
                while (wInDomainIdx < wInDomainList.count()) {
                    wNotProcessed += ".";
                    wNotProcessed += wInDomainList[wInDomainIdx] ;
                    wInDomainIdx++;
                } // while
                pErrorLog->errorLog("ZDomainBroker::getPath-E-NOTFOUND Subdomain(s) <%s> are out of boundaries of declared domains.",wNotProcessed.toCChar());
                return ZS_OUTBOUND;
            }
            return ZS_SUCCESS ;
        }

        int wIdx=-1;
        for (int wi=0 ; wi < wDomainPath->Inferiors.count() ; wi++) {
            if ( wDomainPath->Inferiors[wi]->Name == wInDomainList[wInDomainIdx] ) {
                wDomainPath = wDomainPath->Inferiors[wi] ;
                wDomainPath->getPathPart(pOutPath,pErrorLog);
 //               _constructPath(wDomainPath,pOutPath);
                wIdx = wi;
                break;
            }
        } // for
        if (wIdx < 0) {
            pErrorLog->errorLog("ZDomainBroker::getPath-E-NOTFOUND Subdomain named <%s> has not been found in declared domains.",wInDomainList[wInDomainIdx].toCChar());
            return ZS_NOTFOUND;
        }
    } // for

//    if (wFileName.isEmpty())
//        return ZS_SUCCESS;

    pOutPath.addWithLeadingCondDirDelim(wFileName);
    return ZS_SUCCESS ;
} // ZDomainBroker::getPath
#endif

ZDomainObject
ZDomainBroker::constructFromPath(const utf8VaryingString& pDomainPath,ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pDomainPath,pErrorLog);
    if (wSt != ZS_SUCCESS)
        return ZDomainObject() ;
    return wObject;
}

#ifdef __DEPRECATED__
ZStatus
ZDomainBroker::constructPath(const utf8VaryingString& pDomainPath,ZDomainObject& pObject, ZaiErrors* pErrorLog)
{
    ZArray<ZDomainPath*> wDPList;
    utf8VaryingString wFileName;
    pObject.clear() ;

    ZStatus wSt=_breakDomain(pDomainPath,pObject.Path,wFileName,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return wSt;
    pObject.clear();



    /* for a local path */
    for (int wi=0; wi < pObject.Path.count();wi++) {
        if (pObject.Path[wi]->Type & ZDOM_Path) {
            if (pObject.Path[wi]->Type & ZDOM_Absolute) {

            }
        }
    }// for


    ZDomainPath* wDomainPath=nullptr;
    pOutPath.clear();
    /*
    for (int wi=0; wi < DomainList.count() ; wi++) {
        if ( DomainList[wi]->Name==wInDomainList[0] ) {
            wDomainPath = DomainList[wi] ;
            break;
        }
    }
*/
    for (int wi=0; wi < DomainRoot->Inferiors.count() ; wi++) {
        if ( DomainRoot->Inferiors[wi]->Name==wInDomainList[0] ) {
            wDomainPath = DomainRoot->Inferiors[wi] ;
            break;
        }
    }

    if (wDomainPath==nullptr) {
        pErrorLog->errorLog("ZDomainBroker::getPath-E-NOTFOUND Domain named <%s> has not been found in declared domains.",wInDomainList[0].toCChar());
        return ZS_NOTFOUND;
    }

    //   _constructPath(wDomainPath,pOutPath);
    wDomainPath->getPathPart(pOutPath,pErrorLog);
    for ( int wInDomainIdx = 1 ; wInDomainIdx < wInDomainList.count()  ; wInDomainIdx++) {


        if (wDomainPath->Inferiors.count()==0) {  /* no more sub domains in domain list ? */

            if (wInDomainIdx < (wInDomainList.count() - 1 ) ) { /* input sub domain list not exhausted ? */
                utf8VaryingString wNotProcessed = wInDomainList[wInDomainIdx] ;
                wInDomainIdx++;
                while (wInDomainIdx < wInDomainList.count()) {
                    wNotProcessed += ".";
                    wNotProcessed += wInDomainList[wInDomainIdx] ;
                    wInDomainIdx++;
                } // while
                pErrorLog->errorLog("ZDomainBroker::getPath-E-NOTFOUND Subdomain(s) <%s> are out of boundaries of declared domains.",wNotProcessed.toCChar());
                return ZS_OUTBOUND;
            }
            return ZS_SUCCESS ;
        }

        int wIdx=-1;
        for (int wi=0 ; wi < wDomainPath->Inferiors.count() ; wi++) {
            if ( wDomainPath->Inferiors[wi]->Name == wInDomainList[wInDomainIdx] ) {
                wDomainPath = wDomainPath->Inferiors[wi] ;
                wDomainPath->getPathPart(pOutPath,pErrorLog);
                //               _constructPath(wDomainPath,pOutPath);
                wIdx = wi;
                break;
            }
        } // for
        if (wIdx < 0) {
            pErrorLog->errorLog("ZDomainBroker::getPath-E-NOTFOUND Subdomain named <%s> has not been found in declared domains.",wInDomainList[wInDomainIdx].toCChar());
            return ZS_NOTFOUND;
        }
    } // for

    //    if (wFileName.isEmpty())
    //        return ZS_SUCCESS;

    pOutPath.addWithLeadingCondDirDelim(wFileName);
    return ZS_SUCCESS ;
} // ZDomainBroker::getPath
#endif // __DEPRECATED__
void
ZDomainObject::constructFromDP(ZDomainPath* pDomainPath, ZaiErrors* pErrorLog)
{
    if (pDomainPath->Type & ZDOM_ObjectMask) {
        ZDomainPath::_copyFrom(*pDomainPath) ;
    }
    else
        Path.push_front(pDomainPath);

    ZDomainPath* wDP=pDomainPath->Father;
    while (wDP) {
        Path.push_front(wDP);
        wDP = wDP->Father;
    }
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

    if (wFileName.isEmpty()) {
        Type = ZDOM_Path;
        return ZS_SUCCESS;
    }


    Name = Content = wFileName ;
    Type = ZDOM_File;

    return ZS_SUCCESS ;

} // ZDomainObject::construct

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
ZDomainObject::getPath(ZaiErrors* pErrorLog)
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

    if (ZDomainPath::isValid()&& !Content.isEmpty()) {
        wPath.addWithLeadingCondDirDelim(ZDomainPath::Content);
    }

    return wPath;

} // ZDomainBroker::getPath

ZStatus ZDomainBroker::XmlLoad(uriString& pXmlFile,ZaiErrors* pErrorLog)
{
    utf8VaryingString wXmlString;

    ZStatus wSt;
    if (pErrorLog!=nullptr)
        pErrorLog->setAutoPrintOn(ZAIES_Text);

    if (!pXmlFile.exists())  {
        ZException.setMessage("ZGeneralParameters::XMLLoad",ZS_FILENOTEXIST,Severity_Error,"Parameter file <%s> has not been found.",pXmlFile.toCChar());
        if (pErrorLog!=nullptr) {
            pErrorLog->errorLog("ZGeneralParameters::XMLLoad-E-FILNFND Parameter file <%s> has not been found.",pXmlFile.toCChar());
        }
        return ZS_FILENOTEXIST;
    }


    if ((wSt=pXmlFile.loadUtf8(wXmlString))!=ZS_SUCCESS) {
        if (pErrorLog!=nullptr) {
            pErrorLog->logZExceptionLast();
        }
        return wSt;
    }

    return  XmlLoadString(wXmlString,pErrorLog);
}

ZStatus
ZDomainBroker::XmlLoadString(utf8VaryingString& pXmlString, ZaiErrors *pErrorLog)
{

    pErrorLog->infoLog("ZDomainBroker::XmlLoadString-I-LOAD  Loading domains...\n");
    utf8VaryingString wVerbose;
    zxmlDoc*     wDoc=new zxmlDoc ;
    zxmlElement *wRoot = nullptr;
    zxmlElement *wMainDomainNode=nullptr;
    zxmlElement *wDomainNode=nullptr;
    zxmlElement *wSwapNode=nullptr;


    ZStatus wSt = wDoc->ParseXMLDocFromMemory(pXmlString.toCChar(), pXmlString.getUnitCount(), nullptr, 0);
    if (wSt != ZS_SUCCESS) {
        if (pErrorLog!=nullptr) {
            pErrorLog->logZExceptionLast();
            pErrorLog->errorLog(
                "ZDomainBroker::XMLLoad-E-PARSERR Xml parsing error for string <%s> ",
                pXmlString.subString(0, 25).toString());
        }
        return wSt;
    }

    wSt = wDoc->getRootElement(wRoot);
    if (wSt != ZS_SUCCESS) {
        if (pErrorLog!=nullptr)
            pErrorLog->logZExceptionLast();
        return wSt;
    }
    if (!(wRoot->getName() == "zmasterfileparameters")) {
        ZException.setMessage("ZDomainBroker::XMLLoadString",
                              ZS_XMLINVROOTNAME,
                              Severity_Error,
                              "Invalid root name <%s> expected <zmasterfileparameters>.",
                              wRoot->getName().toString());
        if (pErrorLog!=nullptr)
            pErrorLog->errorLog(
                "ZDomainBroker::XMLLoadString-E-INVROOT Invalid root node name <%s> expected <zmasterfileparameters> - file <%s>",
                wRoot->getName().toString());
        return ZS_XMLINVROOTNAME;
    }

    wSt=wRoot->getChildByName((zxmlNode*&)wMainDomainNode,"domain");
    if (wSt!=ZS_SUCCESS) {
        XMLderegister(wRoot);
        pErrorLog->logZStatus(
            ZAIES_Error,
            wSt,
            "ZDomainBroker::XMLLoadString-E-CNTFINDND Error cannot find node element with name <%s> (domain root element name) status <%s>",
            "domain",
            decode_ZStatus(wSt));
        return wSt;
    }

    wSt=wMainDomainNode->getFirstChild((zxmlNode*&)wDomainNode);
    if (wSt!=ZS_SUCCESS) {
        XMLderegister(wRoot);
        XMLderegister(wMainDomainNode);
        pErrorLog->logZStatus(
            ZAIES_Error,
            wSt,
            "ZDomainBroker::XMLLoadString-W-Empty Domain root node <%s> appears to be empty. No child node found.",
            "domain");
        return wSt;
    }

    if (DomainRoot!=nullptr)
        delete DomainRoot;

    DomainRoot = new ZDomainPath;

    while ((wSt==ZS_SUCCESS) && (wDomainNode!=nullptr)) {
        if (wDomainNode->getName()=="domain-item") {
            ZDomainPath* wDP=XmlGetDomain(wDomainNode,DomainRoot,0,pErrorLog);
            DomainRoot->Inferiors.push(wDP);
        }
        wSwapNode = wDomainNode;
        wSt=wSwapNode->getNextNode((zxmlNode*&)wDomainNode);
        XMLderegister(wSwapNode);
    }

    if (wSt == ZS_EOF )
        wSt = ZS_SUCCESS;
    XMLderegister(wMainDomainNode);
    XMLderegister(wRoot);
    XMLderegister(wDoc);
    return wSt;
} // ZDomainBroker::XmlLoadString

utf8VaryingString
ZDomainBroker::toXmlDomainItem(ZDomainPath* pDomain,int pLevel, ZaiErrors *pErrorLog)
{
    utf8VaryingString wReturn;
    wReturn = fmtXMLnode("domain-item",pLevel);
    if (!pDomain->Name.isEmpty())
        wReturn += fmtXMLchar("name",pDomain->Name.toCChar(),pLevel+1);
    if (!pDomain->Content.isEmpty())
        wReturn += fmtXMLchar("content",pDomain->Content.toCChar(),pLevel+1);
    if (!pDomain->ToolTip.isEmpty())
        wReturn += fmtXMLchar("tooltip",pDomain->ToolTip.toCChar(),pLevel+1);

//    wReturn += fmtXMLchar("type",decode_ZDomain_type( pDomain->Type ),pLevel+1);

    wReturn += fmtXMLuint32Hexa("type",pDomain->Type ,pLevel+1);
    fmtXMLaddInlineComment(wReturn,decode_ZDomain_type( pDomain->Type ));

    for (int wi=0 ; wi < pDomain->Inferiors.count() ; wi++ ) {
        wReturn += toXmlDomainItem(pDomain->Inferiors[wi], pLevel+1,pErrorLog);
    }
    wReturn += fmtXMLendnode("domain-item",pLevel);
    return wReturn;
} // ZDomainBroker::toXmlDomainItem

utf8VaryingString
ZDomainBroker::toXml( int pLevel, ZaiErrors *pErrorLog)
{
    utf8VaryingString wReturn;

    fprintf(stdout,"ZDomainBroker::toXml  ...\n");
    wReturn = fmtXMLnode("domain",pLevel);

    for (int wi=0; wi < DomainRoot->Inferiors.count(); wi++) {
        wReturn += toXmlDomainItem(DomainRoot->Inferiors[wi],pLevel+1,pErrorLog);
    }

    wReturn += fmtXMLendnode("domain",pLevel);


    return wReturn ;
} // ZDomainBroker::toXml


ZDomainPath*
ZDomainBroker::XmlGetDomain(zxmlElement* pDomainNode, ZDomainPath* pFather, int pLevel, ZaiErrors *pErrorLog)
{
    if (pDomainNode->getName()!="domain-item")
        return nullptr;
    zxmlElement *wSubDomainNode=nullptr;
    zxmlElement *wSwapNode=nullptr;

    ZDomainPath* wDomainPath = new ZDomainPath;
    wDomainPath->Father = pFather;

    utf8VaryingString wContent;

    ZStatus wSt=XMLgetChildText(pDomainNode,"name",wDomainPath->Name,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->errorLog("ZDomainBroker::XmlGetDomain-E-MISSELT Required element <%s> cannot be found. Hierarchy <%s>.\n"
                            "             Xml document appears to be corrupted",
                            "name",wDomainPath->getHierarchy().toString());
        return nullptr;
    }
    wSt=XMLgetChildText(pDomainNode,"content",wDomainPath->Content,pErrorLog,ZAIES_Warning);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->warningLog("ZDomainBroker::XmlGetDomain-W-MISSELT Element <%s> cannot be found for domain name <%s> hierarchy <%s>.\n"
                              "Xml document might be corrupted",
                              "content",wDomainPath->Name.toString(),wDomainPath->getHierarchy().toString());
    }

    /*
    wSt=XMLgetChildText(pDomainNode,"type",wContent,pErrorLog,ZAIES_Warning);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->errorLog("ZDomainBroker::XmlGetDomain-W-MISSELT Mandatory element <%s> cannot be found for domain name <%s> hierarchy <%s>.\n"
                            "             Xml document appears to be corrupted",
                              "type",wDomainPath->Name.toString(),wDomainPath->getHierarchy().toString());
    }
    wDomainPath->Type = encode_ZDomain_type(wContent);
    */

    wSt=XMLgetChildText(pDomainNode,"tooltip",wDomainPath->ToolTip,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->infoLog("ZDomainBroker::XmlGetDomain-E-MISSELT Required element <%s> has been omitted for domain name <%s> hierarchy <%s>.\n",
                              "tooltip",wDomainPath->Name.toString(),wDomainPath->getHierarchy().toString());
    }


    wSt=XMLgetChildUInt32Hexa(pDomainNode,"type",wDomainPath->Type,pErrorLog,ZAIES_Info);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->infoLog("ZDomainBroker::XmlGetDomain-I-OMIT Element <%s> cannot be found for domain name <%s> hierarchy <%s>.\n",
                           "type",wDomainPath->Name.toString(),wDomainPath->getHierarchy().toString());
        return nullptr;
    }


    _DBGPRINT("%*c<%s> <%s> <%s> father <%s>\n",
            (pLevel + 1) * 5 , ' ' ,
            wDomainPath->Name.toCChar(),
            wDomainPath->Content.toCChar(),
            decode_ZDomain_type(wDomainPath->Type).toCChar(),
            wDomainPath->Father==nullptr?"nullptr":wDomainPath->Father->Name.toCChar());
    if (!wDomainPath->ToolTip.isEmpty())
        _DBGPRINT("%*cToolTip<%s>\n",
                (pLevel + 1) * 5 , ' ' ,
                wDomainPath->ToolTip.toCChar());

    wSt=pDomainNode->getFirstChild((zxmlNode*&)wSubDomainNode);
    while ((wSt==ZS_SUCCESS)&&(wSubDomainNode!=nullptr)) {
        ZDomainPath* wDP=XmlGetDomain(wSubDomainNode,wDomainPath,pLevel+1,pErrorLog);
        if (wDP) {
            wDomainPath->Inferiors.push(wDP);
        }
        wSwapNode = wSubDomainNode;
        wSt=wSwapNode->getNextNode((zxmlNode*&)wSubDomainNode);
        XMLderegister(wSwapNode);
    } // while


    return wDomainPath;
} // ZDomainBroker::getDomain




utf8VaryingString
decode_ZDomain_type(uint32_t pType)
{
    utf8VaryingString wReturn;

    if (pType & ZDOM_Root)
        wReturn.addConditionalOR("ZDOM_Root");

    if (pType & ZDOM_Path)
        wReturn.addConditionalOR("ZDOM_Path");
    if (pType & ZDOM_Icon)
        wReturn.addConditionalOR("ZDOM_Icon");
    if (pType & ZDOM_Remote)
        wReturn.addConditionalOR("ZDOM_Remote");
    if (pType & ZDOM_Absolute)
        wReturn.addConditionalOR("ZDOM_Absolute");
    if (pType & ZDOM_Relative)
        wReturn.addConditionalOR("ZDOM_Relative");
    if (wReturn.isEmpty())
        wReturn = "ZDOM_Nothing";
    return wReturn;
}
utf8VaryingString
decode_ZDomainTypeForDisplay(uint32_t pType)
{
    utf8VaryingString wReturn;

    if (pType & ZDOM_Root)
        wReturn.addConditionalOR("Root");

    if (pType & ZDOM_Remote)
        wReturn.addConditionalOR("Remote");
    else
        wReturn.addConditionalOR("Local");

    if (pType & ZDOM_Absolute)
        wReturn.addConditionalOR("Absolute");
    if (pType & ZDOM_Relative)
        wReturn.addConditionalOR("Relative");


    if (pType & ZDOM_Path)
        wReturn.addConditionalOR("Path");
    if (pType & ZDOM_Icon)
        wReturn.addConditionalOR("Icon");

    if (wReturn.isEmpty())
        wReturn = "Nothing";

    return wReturn;
}
uint32_t
encode_ZDomain_type(utf8VaryingString& pType)
{
    uint32_t wType=ZDOM_Nothing;
    if (pType.containsCase("ZDOM_Root"))
        wType |= ZDOM_Root;

    if (pType.containsCase("ZDOM_Remote"))
        wType |= ZDOM_Remote;
    if (pType.containsCase("ZDOM_Absolute"))
        wType |= ZDOM_Absolute;
    if (pType.containsCase("ZDOM_Relative"))
        wType |= ZDOM_Relative;
    if (pType.containsCase("ZDOM_Path"))
        wType |= ZDOM_Path;
    if (pType.containsCase("ZDOM_Icon"))
        wType |= ZDOM_Icon;
    return wType;
}
} // namespace zbs
