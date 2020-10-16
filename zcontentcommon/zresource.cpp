#ifndef ZRESOURCE_CPP
#define ZRESOURCE_CPP

#include <zcontentcommon/zresource.h>
#include <zxml/zxmlprimitives.h>
#include <zindexedfile/zdatatype.h>
#include <zxml/zxml.h>



Resourceid_type ResourceId = 0;

Resourceid_type getUniqueResourceId() {return ResourceId++;}

utf8String ZResource::toXml(int pLevel)
{
    utf8String wContent = fmtXMLnode("zresource", pLevel);

    wContent += fmtXMLint64("id", id, pLevel+1);
    wContent += fmtXMLint64("entity", Entity, pLevel+1);
//    wContent += fmtXMLint64("datarank", DataRank, pLevel);
    wContent += fmtXMLendnode("zresource", pLevel);
    return wContent;
}
int ZResource::fromXml(zxmlElement *pRootNode, const char *pChildName, ZaiErrors *pErrorlog)
{
    zxmlElement*wChild=nullptr;
    ZStatus wSt=pRootNode->getChildByName((zxmlNode*&)wChild,pChildName);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorlog->logZStatus(ZAIES_Error,wSt,"ZResource::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pChildName,
            decode_ZStatus(wSt));
        return -1;
    }
    wSt=wChild->getChildByName((zxmlNode*&)wChild,"zresource");
    if (wSt!=ZS_SUCCESS)
    {
        pErrorlog->logZStatus(ZAIES_Error,wSt,"ZResource::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pChildName,
            decode_ZStatus(wSt));
        return -1;
    }
    if (XMLgetChildUInt64Hexa(pRootNode, "entity", Entity, pErrorlog) < 0) {
        pErrorlog->errorLog("ZResource::fromXML-E-CANTGETTXT Cannot get child <entity> as "
                            "hexadecimal text for node <%s>",
            pRootNode->getName().toCChar(),
            "zresource");
        return -1;
    }
    if (XMLgetChildInt64Hexa(pRootNode, "entity", id, pErrorlog) < 0) {
        pErrorlog->errorLog("ZResource::fromXML-E-CANTGETTXT Cannot get child <id> as "
                            "hexadecimal text for node <%s>",
            pRootNode->getName().toCChar(),
            "zresource");
        return -1;
    }
    return 0;
}

int ZResource::fromXmltoHexa(zxmlElement *pRootNode,const char* pChildName,utfcodeString& wCode, ZaiErrors *pErrorlog)
{
    utf8String wValue;
//    utfcodeString wCode;

    zxmlElement*wChild=nullptr;
    ZStatus wSt=pRootNode->getChildByName((zxmlNode*&)wChild,pChildName);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorlog->logZStatus(ZAIES_Error,wSt,"ZResource::fromXmltoHexa-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pChildName,
            decode_ZStatus(wSt));
        return -1;
    }
    wSt=wChild->getChildByName((zxmlNode*&)wChild,"zresource");
    if (wSt!=ZS_SUCCESS)
    {
        pErrorlog->logZStatus(ZAIES_Error,wSt,"ZResource::fromXmltoHexa-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pChildName,
            decode_ZStatus(wSt));
        return -1;
    }

    if (XMLgetChildText(wChild, "entity", wValue, pErrorlog) < 0) {
        pErrorlog->errorLog("ZResource::fromXML-E-CANTGETTXT Cannot get child <entity> as "
                            "hexadecimal text for node <%s>",
            pRootNode->getName().toCChar(),
            "zresource");
        return -1;
    }
    wCode = wValue.toCChar();
    if (XMLgetChildText(pRootNode, "id", wValue, pErrorlog) < 0) {
        pErrorlog->errorLog("ZResource::fromXML-E-CANTGETTXT Cannot get child <id> as hexadecimal "
                            "text for node <%s>",
            pRootNode->getName().toCChar(),
            "zresource");
        return -1;
    }

    wCode += "#";
    wCode += wValue.toCChar();

    return 0;
}//fromXmltoHexa

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
