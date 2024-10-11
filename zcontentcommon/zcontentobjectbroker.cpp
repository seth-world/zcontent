#include "zcontentobjectbroker.h"
#include "zdomainbroker.h"

#include <zxml/zxmlprimitives.h>
#include <QIcon>
//#include <zqt/zqtwidget/zqlabel.h>

namespace zbs {


class ZContentObjectBroker ContentObjectBroker;


ZContentObjectBroker::ZContentObjectBroker() {}


bool ZContentObjectBroker::check(const utf8VaryingString& pDomainChain)
{
    return DomainBroker.domainChainExists(pDomainChain);
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
        pErrorLog->errorLog("ZContentObjectBroker::iconFactory Cannot create icon from image domain <%s> name <%s> resulting path <%s> ",
                            pDomain.toCChar(),wObject.getName().toCChar(),wIconPath.toCChar());
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

    ZDomainObject wViewer ;
    ZStatus wSt = wViewer.constructFromString(wDomainChain,pErrorLog);
    return wViewer;
} // ZContentObjectBroker::getViewerFromType

ZDomainObject
ZContentObjectBroker::getEditorFromType(uint32_t pType, ZaiErrors* pErrorLog)
{
    uint32_t wObjectType = pType & ZDOM_ObjectMask;

    utf8VaryingString wDomainChain = "general.editor.linux.";
    int wi = 0;
    for (;  TypeDecodeTable[wi].Type != ZDOM_End ; wi ++)
        if (TypeDecodeTable[wi].Type == wObjectType)
            break;
    if (TypeDecodeTable[wi].Type == ZDOM_End)
        return ZDomainObject();

    wDomainChain += TypeDecodeTable[wi].TypeString;

    ZDomainObject wEditor ;
    ZStatus wSt = wEditor.constructFromString(wDomainChain,pErrorLog);
    return wEditor;
} // ZContentObjectBroker::getViewerFromType

ZDomainObject
ZContentObjectBroker::getObjetFromDomain(const utf8VaryingString& pDomainChain,ZaiErrors* pErrorLog)
{
    ZDomainObject wObject ;
    ZStatus wSt= wObject.constructFromString(pDomainChain,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return ZDomainObject();
    return wObject;
}


QImage
ZContentObjectBroker::imageFactory(const utf8VaryingString& pDomain, ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pDomain,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return QImage();

    uriString wIconPath = wObject.getFullPath(pErrorLog);

    QImage pIcon = QImage(wIconPath.toCChar());
    if (pIcon.isNull()) {
        pErrorLog->errorLog("ZContentObjectBroker::iconFactory Cannot create image from image file\n"
                            "             domain <%s> name <%s>\n"
                            "             resulting path <%s> ",
                            pDomain.toCChar(),wObject.getName().toCChar(),wIconPath.toCChar());
        if (!wIconPath.exists()) {
            pErrorLog->errorLog("         image file does not exists");
        }
    }

    return pIcon;
}

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

    DomainRoot = new ZDomainPathRoot;

    while ((wSt==ZS_SUCCESS) && (wDomainNode!=nullptr)) {
        if (wDomainNode->getName()=="domain-item") {
            ZDomainPath* wDP=XmlGetDomain(wDomainNode,DomainRoot,0,pErrorLog);
            DomainRoot->addChild(wDP);
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
ZDomainBroker::toXml( int pLevel, ZaiErrors *pErrorLog)
{
    utf8VaryingString wReturn;

    fprintf(stdout,"ZDomainBroker::toXml  ...\n");
    wReturn = fmtXMLnode("domain",pLevel);

    for (int wi=0; wi < DomainRoot->childrenCount(); wi++) {
        if (DomainRoot->child(wi))
            wReturn += DomainRoot->child(wi)->toXml( pLevel+1,pErrorLog);
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
    utf8VaryingString wDName=wDomainPath->getName();

    ZStatus wSt=XMLgetChildText(pDomainNode,"name",wDomainPath->getName(),pErrorLog,ZAIES_Error);
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
                              "content",wDomainPath->getName().toString(),wDomainPath->getHierarchy().toString());
    }

    wSt=XMLgetChildText(pDomainNode,"tooltip",wDomainPath->ToolTip,pErrorLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->infoLog("ZDomainBroker::XmlGetDomain-E-MISSELT Required element <%s> has been omitted for domain name <%s> hierarchy <%s>.\n",
                              "tooltip",wDomainPath->getName().toString(),wDomainPath->getHierarchy().toString());
    }


    wSt=XMLgetChildUInt64Hexa(pDomainNode,"type",wDomainPath->Type,pErrorLog,ZAIES_Info);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->infoLog("ZDomainBroker::XmlGetDomain-I-OMIT Element <%s> cannot be found for domain name <%s> hierarchy <%s>.\n",
                           "type",wDomainPath->getName().toString(),wDomainPath->getHierarchy().toString());
        return nullptr;
    }


    _DBGPRINT("%*c<%s> <%s> <%s> father <%s>\n",
            (pLevel + 1) * 5 , ' ' ,
            wDomainPath->getName().toCChar(),
            wDomainPath->Content.toCChar(),
            decode_ZDomain_type(wDomainPath->Type).toCChar(),
            wDomainPath->getFatherName().toCChar());
    if (!wDomainPath->ToolTip.isEmpty())
        _DBGPRINT("%*cToolTip<%s>\n",
                (pLevel + 1) * 5 , ' ' ,
                wDomainPath->ToolTip.toCChar());

    wSt=pDomainNode->getFirstChild((zxmlNode*&)wSubDomainNode);
    while ((wSt==ZS_SUCCESS)&&(wSubDomainNode!=nullptr)) {
        ZDomainPath* wDP=XmlGetDomain(wSubDomainNode,wDomainPath,pLevel+1,pErrorLog);
        if (wDP) {
            wDomainPath->addChild(wDP);
        }
        wSwapNode = wSubDomainNode;
        wSt=wSwapNode->getNextNode((zxmlNode*&)wSubDomainNode);
        XMLderegister(wSwapNode);
    } // while


    return wDomainPath;
} // ZDomainBroker::getDomain



} // namespace zbs
