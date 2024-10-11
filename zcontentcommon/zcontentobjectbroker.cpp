#include "zcontentobjectbroker.h"
#include "zdomainbroker.h"

#include <zxml/zxmlprimitives.h>
#include <QIcon>
#include <zqt/zqtwidget/zqlabel.h>

namespace zbs {


class ZContentObjectBroker ContentObjectBroker;


ZContentObjectBroker::ZContentObjectBroker() {}


ZQLabel*
ZContentObjectBroker::labelFactory(const utf8VaryingString& pImageDomainSpec, ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pImageDomainSpec,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return nullptr;

    uriString wIconPath = wObject.getFullPath(pErrorLog);
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

    uriString wIconPath = wObject.getFullPath(pErrorLog);

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
}// ZContentObjectBroker::iconFactory

ZDomainObject
ZContentObjectBroker::getViewerFromType(uint32_t pType, ZaiErrors* pErrorLog)
{
    uint32_t wObjectType = pType & ZDOM_ObjectMask;

    utf8VaryingString wDomainChain = "general.viewer.linux.";
    int wi = 0;
    for (;  TypeDecodeTable[wi].Type != ZDOM_End ; wi ++)
        if (TypeDecodeTable[wi].Type == wObjectType)
            break;
    if (TypeDecodeTable[wi].Type == ZDOM_End)
        return ZDomainObject();

    wDomainChain += TypeDecodeTable[wi].TypeString;
    /*
    switch (wObjectType)
    {
    case ZDOM_Path:
    {
        ZDomainObject wPathViewer ;
        wPathViewer.constructFromString("general.viewer.ZDOM_Path",pErrorLog);
        return wPathViewer;
    }
    case ZDOM_Icon:
    case ZDOM_Image:
    case ZDOM_Executable:
    {
        ZDomainObject wPathViewer ;
        wPathViewer.constructFromString("general.viewer.ZDOM_Path",pErrorLog);
        return wPathViewer;
    }
    case ZDOM_TextFile:
    {
        ZDomainObject wPathViewer ;
        wPathViewer.constructFromString("general.viewer.ZDOM_TextFile",pErrorLog);
        return wPathViewer;
    }
    case ZDOM_PdfFile:
    {
        ZDomainObject wPathViewer ;
        wPathViewer.constructFromString("general.viewer.ZDOM_PdfFile",pErrorLog);
        return wPathViewer;
    }
    case ZDOM_OdfFile:
    default:
        break;

    }// switch
    return ZDomainObject();
    */
    ZDomainObject wPathViewer ;
    ZStatus wSt = wPathViewer.constructFromString(wDomainChain,pErrorLog);
    return wPathViewer;
} // ZContentObjectBroker::getViewerFromType

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

    uriString wIconPath = wObject.getFullPath(pErrorLog);

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

    uriString wFilePath = wObject.getFullPath(pErrorLog);

    return wFilePath.loadContent(pContent);
}




ZStatus
ZContentObjectBroker::loadUtf8(const utf8VaryingString& pDomainFileSpec, utf8VaryingString& pUtfContent, ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pDomainFileSpec,pErrorLog);
    if (wSt != ZS_SUCCESS)
        return wSt ;
    return wObject.loadUtf8(pUtfContent,pErrorLog);
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
ZDomainPath*
ZDomainBroker::_breakDomainPathName(const utf8VaryingString& pInPath,
                            ZaiErrors* pErrorLog)
{
//    pFileName.clear();
    utf8_t* wToBeFreed = utfStrdup(pInPath.Data) ;
    utf8_t* wPtr = wToBeFreed ;
    utf8_t* wPtrStart=wPtr;

    int wSize=0;
    ZDomainPath* wDPCurrent = DomainRoot;

    bool wEndOfString = false;
    bool wFileNameFollows = false;
    while (!wEndOfString  ) {
        wPtrStart = wPtr;
        if (wFileNameFollows) {
//            pFileName += wPtrStart ; /* take all remaining string after slash sign */
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
            pErrorLog->errorLog("breakDomainPathName::_breakDomain-E-INVNAME Domain node name <%s> cannot be found within domain hierarchy <%s>\n"
                                "                           Invalid domain path %s",
                                wDom.toCChar(),wDPCurrent->getHierarchy().toCChar(),pInPath.toCChar());
            free(wToBeFreed);
            return nullptr;
        }
    } //while (*wPtr && (*wPtr != '/') && !wFileNameFollows )
    free(wToBeFreed);
    return wDPCurrent;
}// ZDomainBroker::breakDomainPathName

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
    bool wFileNameFollows = false;
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
                pErrorLog->errorLog("ZDomainBroker::_breakDomain-E-INVNAME Domain node name <%s> cannot be found within domain hierarchy <%s>\n"
                                    "                           Invalid domain path <%s>",
                                    wDom.toCChar(),wDPCurrent->getHierarchy().toCChar(),pInPath.toCChar());
                free(wToBeFreed);
                return ZS_INVNAME;
        }
    } //while (*wPtr && (*wPtr != '/') && !wFileNameFollows )

    free(wToBeFreed);
    return ZS_SUCCESS;
}// ZDomainBroker::breakDomain


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

ZDomainPath*
ZDomainBroker::searchLevelRegardless(const utf8VaryingString& pDPName)
{
    return DomainRoot->search(pDPName);
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
ZDomainPath::toXml(int pLevel, ZaiErrors *pErrorLog)
{
    utf8VaryingString wReturn;
//    if (Name == __DOMAIN_NOT_TO_BE_SAVED__)
//        return wReturn;
    if (Name == Temporary)
        return wReturn;
    wReturn = fmtXMLnode("domain-item",pLevel);
    if (!Name.isEmpty())
        wReturn += fmtXMLchar("name",Name.toCChar(),pLevel+1);
    if (!Content.isEmpty())
        wReturn += fmtXMLchar("content",Content.toCChar(),pLevel+1);
    if (!ToolTip.isEmpty())
        wReturn += fmtXMLchar("tooltip",ToolTip.toCChar(),pLevel+1);

    //    wReturn += fmtXMLchar("type",decode_ZDomain_type( Type ),pLevel+1);

    wReturn += fmtXMLuint32Hexa("type",Type ,pLevel+1);
    fmtXMLaddInlineComment(wReturn,decode_ZDomain_type( Type ));

    for (int wi=0 ; wi < Inferiors.count() ; wi++ ) {
        wReturn += Inferiors[wi]->toXml(pLevel+1,pErrorLog);
    }
    wReturn += fmtXMLendnode("domain-item",pLevel);
    return wReturn;
} // ZDomainPath::toXmlDomainItem

/* Deprecated
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
//        wReturn += toXmlDomainItem(pDomain->Inferiors[wi], pLevel+1,pErrorLog);
        wReturn += pDomain->Inferiors[wi]->toXml( pLevel+1,pErrorLog);
    }
    wReturn += fmtXMLendnode("domain-item",pLevel);
    return wReturn;
} // ZDomainBroker::toXmlDomainItem
*/
utf8VaryingString
ZDomainBroker::toXml( int pLevel, ZaiErrors *pErrorLog)
{
    utf8VaryingString wReturn;

    fprintf(stdout,"ZDomainBroker::toXml  ...\n");
    wReturn = fmtXMLnode("domain",pLevel);

    for (int wi=0; wi < DomainRoot->Inferiors.count(); wi++) {
        if (DomainRoot->Inferiors[wi])
            wReturn += DomainRoot->Inferiors[wi]->toXml( pLevel+1,pErrorLog);
//            wReturn += toXmlDomainItem(DomainRoot->Inferiors[wi],pLevel+1,pErrorLog);
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



} // namespace zbs
