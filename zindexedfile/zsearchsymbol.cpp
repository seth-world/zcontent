#include "zsearchsymbol.h"
#include <zxml/zxmlprimitives.h>

namespace zbs {
ZSearchSymbol::ZSearchSymbol():ZSearchLiteral(ZSTO_Symbol) { }

/*
        <symbolitem>
            <type>ZSTO_UriString</type>
            <content>/home/gerard/Development/zmftest/zdocphysical.zmf</content>
            <name>zdocphysical</name>
        </symbolitem>
        <symbolitem>
            <type>ZSTO_Resource</type>
            <content>
                <zresource>
                <id>1</id>
                <entity>4097</entity>
                </zresource>
            </content>
            <name>wResource</name>
        </symbolitem>

*/

utf8VaryingString ZSearchSymbol::toXml(int pLevel)
{
    utf8VaryingString wXmlReturn, wZSTOText;
    int wLevel = pLevel;
    wXmlReturn = fmtXMLnode("symbolitem", pLevel);

    wLevel++;
    wXmlReturn += fmtXMLstring("name", FullFieldName, wLevel);

    wXmlReturn += fmtXMLintHexa("type", ZSTO, wLevel);
    wZSTOText.sprintf(" ZSTO value %X-%s ",ZSTO,decode_ZSTO(ZSTO).toString());
    fmtXMLaddInlineComment(wXmlReturn,wZSTOText) ;


    ZSearchOperandType_type wZSTO_Base = ZSTO & ZSTO_BaseMask;
    switch(wZSTO_Base)
    {
    case ZSTO_UriString:
        wXmlReturn += fmtXMLstring( "content",getURI(),wLevel);
        break;
    case ZSTO_String:
        wXmlReturn += fmtXMLstring( "content",getString(),wLevel);
        break;
    case ZSTO_Integer:
        wXmlReturn += fmtXMLlong( "content",getInteger(),wLevel);
        break;
    case ZSTO_Float:
        wXmlReturn += fmtXMLdouble("content",getFloat(),wLevel);
        break;
    case ZSTO_Bool:
        wXmlReturn += fmtXMLbool("content",getBool(),wLevel);
        break;
    case ZSTO_Date: {
        wXmlReturn += fmtXMLdatefull("content",getDate(),wLevel);
        break;
    }
    case ZSTO_Resource: {
        wXmlReturn += fmtXMLnode("content", wLevel);
        wXmlReturn += getResource().toXml(wLevel+1);
        wXmlReturn += fmtXMLendnode("content", wLevel);
        break;
    }
    case ZSTO_Checksum: {
        wXmlReturn += fmtXMLcheckSum("content",getChecksum(),wLevel);
        break;
    }
    default:
        wXmlReturn.addsprintf("<!-- invalid ZTO base value %X -->",wZSTO_Base);
        break;
    } // switch

    wLevel--;
    wXmlReturn += fmtXMLendnode("symbolitem", pLevel);

    return wXmlReturn;
} //ZSearchSymbol::toXml

ZStatus ZSearchSymbol::fromXml(zxmlElement*pSymbolRootNode,const utf8VaryingString& pRootName,ZaiErrors* pErrorLog)
{
    utf8VaryingString wSymbolName , wZSTOString,wContent;
    ZSearchOperandType_type wZSTO_Base = ZSTO_Nothing;
    if (pSymbolRootNode->getName()!=pRootName) {
        if (pErrorLog==nullptr) {
            _DBGPRINT("ZSearchSymbol::fromXml-E-INVNAME Invalid root node name %s while expecting %s\n",
                      pSymbolRootNode->getName().toCChar(),
                      pRootName.toCChar())
            return ZS_INVNAME;
        }
    }
    ZStatus wSt=XMLgetChildText( pSymbolRootNode,"name",wSymbolName,pErrorLog);
    FullFieldName = wSymbolName;
    int wZSTO;
    wSt=XMLgetChildIntHexa( pSymbolRootNode,"type",wZSTO,pErrorLog);

    ZSTO = ZSearchOperandType_type(wZSTO);
//    ZSTO = OperandZSTO  = encode_ZSTO(wZSTOString);
    OperandZSTO = ZSTO & ZSTO_BaseMask;
    wZSTO_Base = ZSTO & ZSTO_BaseMask;
    switch(wZSTO_Base)
    {
    case ZSTO_UriString:
    {
//        uriString wURI;
        wSt=XMLgetChildText( pSymbolRootNode,"content",wContent,pErrorLog);
        setURI(wContent);
        break;
    }
    case ZSTO_String:
        wSt=XMLgetChildText( pSymbolRootNode,"content",wContent,pErrorLog);
        setString(wContent);
        break;
    case ZSTO_Integer:
    {
        wSt=XMLgetChildText( pSymbolRootNode,"content",wContent,pErrorLog);
        setInteger(wContent.toLong());
        break;
    }
    case ZSTO_Float:
        wSt=XMLgetChildText( pSymbolRootNode,"content",wContent,pErrorLog);
        setFloat(wContent.toDouble());
        break;
    case ZSTO_Bool:
    {
        bool wBool;
        wSt=XMLgetChildBool( pSymbolRootNode,"content",wBool,pErrorLog);
        setBool(wBool);
        break;
    }
    case ZSTO_Date:
    {
        ZDateFull wD;
        wSt=XMLgetChildZDateFull( pSymbolRootNode,"content",wD,pErrorLog);
        setDate(wD);
        break;
    }
    case ZSTO_Resource: {
        ZResource wR;
        wR.fromXml(pSymbolRootNode,"content",pErrorLog,ZAIES_Error);
        setResource(wR);
        break;
    }
    case ZSTO_Checksum:
    {
        checkSum wR;
        wSt=XMLgetChildCheckSum(pSymbolRootNode,"content",wR,pErrorLog);
        setChecksum(wR);
        break;
    }
    default:
        if (pErrorLog!=nullptr) {
            pErrorLog->errorLog("ZSearchSymbol::fromXml-E-INVTYP Invalid base ZSTO %X-%s",wZSTO_Base,decode_ZSTO(wZSTO_Base).toString());
        }
        else {
            _DBGPRINT("ZSearchSymbol::fromXml-E-INVTYP Invalid base ZSTO %X-%s",wZSTO_Base,decode_ZSTO(wZSTO_Base).toString());
        }
        return ZS_INVTYPE;
    } // switch

    return ZS_SUCCESS;
} // ZSearchSymbol::fromXml

/*
<?xml version='1.0' encoding='UTF-8'?>
 <zsearchparsersymbol version = "'1.0-0'">
    <!--  Symbols table : defines symbols that points to a valid full file path -->
    <symboltable>
        <symbolitem>
            <type>0x10040</type> <!-- ZSTO_Symbol | ZSTO_UriString -->
            <content>/home/gerard/Development/zmftest/zdocphysical.zmf</content>
            <name>zdocphysical</name>
        </symbolitem>
        <symbolitem>
            <type>0x10020</type> <!-- ZSTO_Symbol | ZSTO_Resource -->
            <content>
                <zresource>
                <id>1</id>
                <entity>4097</entity>
                </zresource>
            </content>
            <name>wResource</name>
        </symbolitem>
    </symboltable>

</zsearchparsersymbol>
*/
utf8VaryingString
ZSearchSymbolList::toXml()
{
    utf8VaryingString wXmlReturn;

    wXmlReturn=fmtXMLdeclaration();

    utf8VaryingString wCom;
    wCom.sprintf("Symbol table predefines symbols that may be later on used in a request\n"
                 "This file has been processed by ZSearchSymbolList::toXml on %s",ZDateFull::currentDateTime().toDMYhms().toString());
    wXmlReturn += fmtXMLcomment(wCom,0);

    wXmlReturn += fmtXMLversion("zsearchparsersymbol",getVersionNum("1.0-0"),0);

    int wLevel=1;
    wXmlReturn += fmtXMLnode("symboltable",wLevel);
    wLevel++;
    for (int wi=0; wi < count(); wi++) {
        wXmlReturn += Tab(wi).toXml(wLevel);
    }
    wLevel--;
    wXmlReturn += fmtXMLendnode("symboltable",wLevel);

    wXmlReturn += fmtXMLendnode("zsearchparsersymbol",0);
    return wXmlReturn;
}// ZSearchSymbolList::toXml

ZStatus
ZSearchSymbolList::fromXml(const utf8VaryingString &pXmlContent, ZaiErrors *pErrorLog)
{
    ZStatus wSt;

    zxmlDoc     *wDoc = nullptr;
    zxmlElement *wRoot = nullptr;
    zxmlElement *wParamRootNode=nullptr;
    zxmlElement *wSymbolListNode=nullptr;

    zxmlElement *wSymbolKeyword=nullptr;
    zxmlElement *wSwapNode=nullptr;

    utf8VaryingString wKeyword;

    utf8VaryingString     wSkipIdentifier;
    utf8VaryingString     wSkipEndToken;



    wDoc = new zxmlDoc;
    wSt = wDoc->ParseXMLDocFromMemory(pXmlContent.toCChar(), pXmlContent.getUnitCount(), nullptr, 0);
    if (wSt != ZS_SUCCESS) {
        pErrorLog->logZExceptionLast();
        pErrorLog->errorLog(
            "ZSearchParser::loadXmlSearchParserSymbols-E-PARSERR Xml parsing error for string <%s> ",
            pXmlContent.subString(0, 25).toUtf());
        return wSt;
    }

    wSt = wDoc->getRootElement(wRoot);
    if (wSt != ZS_SUCCESS) {
        pErrorLog->logZExceptionLast();
        return wSt;
    }
    if (!(wRoot->getName() == "zsearchparsersymbol")) {
        pErrorLog->errorLog(
            "ZSearchParser::loadXmlSearchParserSymbols-E-INVROOT Invalid root node name <%s> expected <zsearchparsersymbol>",
            wRoot->getName().toCChar());
        return ZS_XMLINVROOTNAME;
    }

    /*------------------ Symbol table -----------------------*/

    while (true) {
        wSt=wRoot->getChildByName((zxmlNode*&)wSymbolListNode,"symboltable");
        if (wSt!=ZS_SUCCESS) {
            pErrorLog->logZStatus(
                ZAIES_Error,
                wSt,
                "ZSearchParser::loadXmlSearchParserSymbols-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                "symboltable",
                decode_ZStatus(wSt));
            break;
        }


        wSt=wSymbolListNode->getFirstChild((zxmlNode*&)wSymbolKeyword);

        ZSearchSymbol wSymbolElt;

        while (wSt==ZS_SUCCESS) {
            wSt=wSymbolElt.fromXml(wSymbolKeyword);
            push(wSymbolElt);
            wSt=wSymbolKeyword->getNextNode((zxmlNode*&)wSwapNode);
            XMLderegister(wSymbolKeyword);
            wSymbolKeyword=wSwapNode;
        }// while (wSt==ZS_SUCCESS)

        pErrorLog->textLog("_________________Search Parser symbols table load ____________________\n"
                " %ld symbols loaded.\n", count());
        pErrorLog->textLog("%3s- %27s %s\n","rnk","Symbol","Content");
        for (long wi=0;wi < count();wi++) {
            pErrorLog->textLog("%3ld- <%25s> <%s>\n",wi+1,
                               Tab(wi).FullFieldName.toString(),
                               Tab(wi).display().toString()
                    );
        }
        pErrorLog->textLog("________________________________________________________________\n");
        XMLderegister(wSymbolListNode);
        break;
    } // while (true)


    XMLderegister((zxmlNode *&) wParamRootNode);
    XMLderegister((zxmlNode *&) wRoot);

    if (wSt==ZS_EOF)
        return ZS_SUCCESS;
    return wSt;
}//ZSearchSymbolList::fromXml

ZStatus ZSearchSymbolList::XmlLoad(const uriString &pXmlFile, ZaiErrors *pErrorLog)
{
    utf8VaryingString wXmlString;
    ZStatus wSt;

    pErrorLog->setAutoPrintOn(ZAIES_Text);

    pErrorLog->textLog(" Loading symbol table <%s>",pXmlFile.toString());

    if (!pXmlFile.exists())  {
        pErrorLog->errorLog("ZSearchSymbolList::XmlLoad-E-FILNFND Symbol file <%s> has not been found.",pXmlFile.toCChar());
        return ZS_FILENOTEXIST;
    }

    uriString wXmlFile=pXmlFile;

    if ((wSt=wXmlFile.loadUtf8(wXmlString))!=ZS_SUCCESS) {
        pErrorLog->logZExceptionLast();
        return wSt;
    }
    return fromXml(wXmlString,pErrorLog);
} //ZSearchSymbolList::XmlLoad

ZStatus
ZSearchSymbolList::XmlSave(const uriString& pXmlFile, ZaiErrors *pErrorLog)
{

    utf8VaryingString wXmlString=toXml();
    if (pXmlFile.exists())
        pErrorLog->textLog("ZSearchSymbolList::XmlSave Replacing file <%s>",pXmlFile.toString());
    return pXmlFile.writeContent(wXmlString);
}


} //namespace zbs
