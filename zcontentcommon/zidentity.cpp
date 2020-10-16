#include <zcontentcommon/zidentity.h>
#include <ztoolset/charman.h>

#include <zxml/zxmlprimitives.h>
#include <zindexedfile/zdatatype.h>
#include <zxml/zxml.h>

#ifdef QT_CORE_LIB
QString     ZIdentity::toQString(void)
{
    char Buffer[15];
    sprintf(Buffer,"%06ld",id);
    return(QString(Buffer));
}
QString     ZIdentity::toQStringShort(void)
{
    char Buffer[15];
    sprintf(Buffer,"%06ld",id);
    return(QString(Buffer));
}
QString     ZIdentity::toQStringLong(void)
{
    char Buffer[15];
    sprintf(Buffer,"%012ld",id);
    return(QString(Buffer));
}

ZIdentity& ZIdentity::fromQString (const QString pInput)
{
    bool ConvError;
    id = pInput.toInt(&ConvError);
    if (ConvError)
        fprintf(stderr,"%s-E-CONVERROR There were error converting QString %s to Docid_struct\n",
                _GET_FUNCTION_NAME_,
                pInput.toStdString().c_str());
    return(*this);
}
#endif // QT_CORE_LIB
CharMan ZIdentity::toStr()
{
    CharMan wId;
    sprintf(wId.content, "%ld", id);
    return wId;
}
CharMan ZIdentity::toHexa()
{
    CharMan wId;
    sprintf(wId.content, "%lXd", id);
    return wId;
}

utf8String ZIdentity::toXml(const char* pName,int pLevel)
{
    utf8String wContent = fmtXMLnode(pName, pLevel);

    wContent += fmtXMLint64Hexa("identityid", id, pLevel+1);
    wContent += fmtXMLendnode(pName, pLevel);
    return wContent;
}

int ZIdentity::fromXml(zxmlElement *pRootNode, const char *pChildName, ZaiErrors *pErrorlog)
{
    zxmlElement*wChild=nullptr;
    ZStatus wSt=pRootNode->getChildByName((zxmlNode*&)wChild,pChildName);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorlog->logZStatus(ZAIES_Error,wSt,"ZIdentity::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                              pChildName,
                              decode_ZStatus(wSt));
        return -1;
    }

    if (XMLgetChildInt64Hexa(pRootNode, "identityid", id, pErrorlog) < 0) {
        pErrorlog->errorLog("ZIdentity::fromXML-E-CANTGETTXT Cannot get child <id> as "
                            "hexadecimal text for node <%s>",
                            pRootNode->getName().toCChar(),
                            "identityid");
        return -1;
    }
    return 0;
}


