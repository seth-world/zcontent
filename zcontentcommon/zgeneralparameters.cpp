#include "zgeneralparameters.h"
#include <ztoolset/zaierrors.h>
#include <zxml/zxmlprimitives.h>
#include <ztoolset/zexceptionmin.h>

#include <zcontent/zindexedfile/zmf_limits.h>

namespace zbs {
ZGeneralParameters GeneralParameters;
}

using namespace zbs;



ZGeneralParameters::ZGeneralParameters()
{
    BaseParameters = _BaseParameters = new ZBaseParameters;
}

ZGeneralParameters::ZGeneralParameters(int argc, char *argv[])
{
    BaseParameters = _BaseParameters = new ZBaseParameters;
    setFromArg(argc, argv);
}

ZStatus
ZGeneralParameters::setFromArg(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf (stderr,"ZGeneralParameters::setFromArg-E-NOARG no argument given.");
        return ZS_NOTFOUND;
    }
    uriString wURIGeneralParams;
    wURIGeneralParams = argv[1];
    if (!wURIGeneralParams.exists()) {
        fprintf (stderr,"ZGeneralParameters::setFromArg-E-FILENOTEXIST given argument <%s> as general parameter file does not exist.",
                argv[1]);
        return ZS_FILENOTEXIST;
    }
    return XmlLoad(wURIGeneralParams,nullptr);
}

/*
 <?xml version='1.0' encoding='UTF-8'?>
 <zmasterfileparameters version = "'2.30-0'">
   <generalparameters>
        <defaultworkdirectory> </defaultworkdirectory>
        <defaultparamdirectory> </defaultparamdirectory>
        <defaulticondirectory> </defaulticondirectory>
        <fixedfont> </fixedfont>
    </generalparameters>
 </zmasterfileparameters>
*/
ZStatus ZGeneralParameters::XmlSave(uriString& pXmlFile,ZaiErrors* pErrorLog)
{
    ZStatus wSt;
    int wLevel=0;
    if (pErrorLog!=nullptr)
        pErrorLog->setAutoPrintOn(ZAIES_Text);

    utf8VaryingString wReturn = fmtXMLdeclaration();
    wReturn += fmtXMLmainVersion("zmasterfileparameters",__ZMF_VERSION__,0);
    wLevel=1;
    wReturn += fmtXMLnode("generalparameters",wLevel);
    wLevel++;
    wReturn += fmtXMLchar("verbose",decode_Verbose(getVerbose()),wLevel);
    wReturn += fmtXMLchar("defaultworkdirectory",getWorkDirectory(),wLevel);
    wReturn += fmtXMLchar("defaultparamdirectory",getParamDirectory(),wLevel);
    wReturn += fmtXMLchar("defaulticondirectory",getIconDirectory(),wLevel);
    if (!FixedFont.isEmpty())
        wReturn += fmtXMLchar("fixedfont",getFixedFont(),wLevel);
    wLevel--;
    wReturn += fmtXMLendnode("generalparameters",wLevel);
    wReturn += fmtXMLendnode("zmasterfileparameters",0);

    return pXmlFile.writeContent(wReturn);
}//  XmlSave

ZStatus ZGeneralParameters::XmlLoad(uriString& pXmlFile,ZaiErrors* pErrorLog)
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

    utf8VaryingString wVerbose;
    zxmlDoc     *wDoc = nullptr;
    zxmlElement *wRoot = nullptr;
    zxmlElement *wParamRootNode=nullptr;
    zxmlElement *wGenParamsNode=nullptr;
    zxmlElement *wObjectNode=nullptr;
    zxmlElement *wSwapNode=nullptr;

    //  zxmlElement *wTypeNode=nullptr;
    zxmlElement *wIncludeFileNode=nullptr;

    utf8VaryingString wKeyword;

    ZTypeBase             wZType;

    utf8VaryingString     wIncludeFile;

    wDoc = new zxmlDoc;
    wSt = wDoc->ParseXMLDocFromMemory(wXmlString.toCChar(), wXmlString.getUnitCount(), nullptr, 0);
    if (wSt != ZS_SUCCESS) {
        if (pErrorLog!=nullptr) {
            pErrorLog->logZExceptionLast();
            pErrorLog->errorLog(
                "ZGeneralParameters::XMLLoad-E-PARSERR Xml parsing error for string <%s> ",
                wXmlString.subString(0, 25).toString());
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
        ZException.setMessage("ZGeneralParameters::XMLLoad",
                              ZS_XMLINVROOTNAME,
                              Severity_Error,
                              "Invalid root name <%s> expected <zmasterfileparameters> - file <%s>.",
                              wRoot->getName().toString(),
                              pXmlFile.toCChar());
        if (pErrorLog!=nullptr)
            pErrorLog->errorLog(
                "ZGeneralParameters::XMLLoad-E-INVROOT Invalid root node name <%s> expected <zmasterfileparameters> - file <%s>",
                wRoot->getName().toString(),pXmlFile.toCChar());
        return ZS_XMLINVROOTNAME;
    }
    /*------------------ various parameters -----------------------*/
    wSt=wRoot->getChildByName((zxmlNode*&)wGenParamsNode,"generalparameters");
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->logZStatus(
            ZAIES_Error,
            wSt,
            "ZGeneralParameters::loadGenerateParameters-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            "generalparameters",
            decode_ZStatus(wSt));
        return wSt;
    }
    wSt=XMLgetChildText( wGenParamsNode,"verbose",wVerbose,pErrorLog,ZAIES_Warning);
    if (wSt!=ZS_SUCCESS)
        _DBGPRINT("ZGeneralParameters::XmlLoad-W-NOVERBOSE No verbose parameters have been found.\n")
    else {
        _BaseParameters->setVerbose(encode_Verbose(wVerbose));
        _DBGPRINT("ZGeneralParameters::XmlLoad-I-VERBOSE Verbose is set to %s.\n", decode_Verbose(getVerbose()).toCChar())
    }

    wSt=XMLgetChildText( wGenParamsNode,"defaultworkdirectory",WorkDirectory,pErrorLog,ZAIES_Warning);
    wSt=XMLgetChildText( wGenParamsNode,"defaultparamdirectory",ParamDirectory,pErrorLog,ZAIES_Error);
    wSt=XMLgetChildText( wGenParamsNode,"defaulticondirectory",IconDirectory,pErrorLog,ZAIES_Error);
    wSt=XMLgetChildText( wGenParamsNode,"fixedfont",FixedFont,pErrorLog,ZAIES_Warning);

    XMLderegister(wGenParamsNode);

    XMLderegister((zxmlNode *&) wParamRootNode);

    currentXml = pXmlFile ;

    if (wSt==ZS_EOF) {
        Init=true;
        return ZS_SUCCESS;
    }
    if (wSt==ZS_SUCCESS)
        Init = true;

    return wSt;
}//  XmlLoad



ZGeneralParameters&
ZGeneralParameters::_copyFrom (const ZGeneralParameters& pIn)
{
    WorkDirectory = pIn.WorkDirectory;
    ParamDirectory = pIn.ParamDirectory;
    IconDirectory = pIn.IconDirectory;
    FixedFont = pIn.FixedFont;
    currentXml= pIn.currentXml;
    Init = pIn.Init;

    _BaseParameters->Verbose = pIn._BaseParameters->Verbose;

    return *this;
}

uriString
ZGeneralParameters::getWorkDirectory() {

    if (WorkDirectory.isEmpty()) {
        const char* wWD=getenv(__PARSER_WORK_DIRECTORY__);
        if (!wWD) {
            WorkDirectory.setToWorkingDir();
            fprintf(stderr,"getParserWorkDirectory-W-MISSYMB Missing environment symbol <%s> working directory is set to <%s> \n",
                    __PARSER_WORK_DIRECTORY__,WorkDirectory.toCChar());
        }
        else
            WorkDirectory=wWD;
    }
    return WorkDirectory;
}
uriString
ZGeneralParameters::getParamDirectory() {

    if (ParamDirectory.isEmpty()) {
        const char* wWD=getenv(__PARSER_PARAM_DIRECTORY__);
        if (!wWD) {
            ParamDirectory.setToWorkingDir();
            fprintf(stderr,"getParserWorkDirectory-W-MISSYMB Missing environment symbol <%s> parameters directory is set to <%s> \n",
                    __PARSER_PARAM_DIRECTORY__,ParamDirectory.toCChar());
        }
        else
            ParamDirectory=wWD;
    }
    return ParamDirectory;
}

uriString
ZGeneralParameters::getIconDirectory() {

    if (IconDirectory.isEmpty()) {
        const char* wWD=getenv(__PARSER_ICON_DIRECTORY__);
        if (!wWD) {
            IconDirectory.setToWorkingDir();
            fprintf(stderr,"getParserWorkDirectory-W-MISSYMB Missing environment symbol <%s> parameters directory is set to <%s> \n",
                    __PARSER_ICON_DIRECTORY__,IconDirectory.toCChar());
        }
        else
            IconDirectory=wWD;
    }
    return IconDirectory;
}

utf8VaryingString
ZGeneralParameters::getFixedFont() {
    return FixedFont;
}


utf8VaryingString
decode_Verbose(ZVerbose_Base pVerbose)
{
    utf8VaryingString wReturn;
    if (pVerbose & ZVB_Basic) {
        wReturn.addConditionalOR( "ZVB_Basic");
    }
    if (pVerbose & ZVB_Thread) {
        wReturn.addConditionalOR( "ZVB_Thread");
    }
    if (pVerbose & ZVB_Net) {
        wReturn.addConditionalOR( "ZVB_Net");
    }
    if (pVerbose & ZVB_NetStats) {
        wReturn.addConditionalOR( "ZVB_NetStats");
    }
    if (pVerbose & ZVB_Mutex) {
        wReturn.addConditionalOR( "ZVB_Mutex");
    }
    if (pVerbose & ZVB_Xml) {
        wReturn.addConditionalOR( "ZVB_Xml");
    }

    if (pVerbose & ZVB_ZRF) {
        wReturn.addConditionalOR( "ZVB_ZRF");
    }
    if (pVerbose & ZVB_MemEngine) {
        wReturn.addConditionalOR( "ZVB_MemEngine");
    }
    if (pVerbose & ZVB_FileEngine) {
        wReturn.addConditionalOR( "ZVB_FileEngine");
    }
    if (pVerbose & ZVB_SearchEngine) {
        wReturn.addConditionalOR( "ZVB_SearchEngine");
    }
    if (pVerbose & ZVB_ZMF) {
        wReturn.addConditionalOR( "ZVB_ZMF");
    }
    if (pVerbose & ZVB_ZIF) {
        wReturn.addConditionalOR( "ZVB_ZIF");
    }
    if (pVerbose & ZVB_Net) {
        wReturn.addConditionalOR( "ZVB_Net");
    }

    if (wReturn.isEmpty())
        wReturn = "ZVB_NoVerbose";

    return wReturn;
}

ZVerbose_Base
encode_Verbose(const utf8VaryingString& pVerboseString)
{
    ZVerbose_Base wV=ZVB_NoVerbose;
    if (pVerboseString.hasToken((const utf8_t*)"ZVB_Basic"))
        wV |= ZVB_Basic;
    if (pVerboseString.hasToken((const utf8_t*)"ZVB_Thread"))
        wV |= ZVB_Thread;
    if (pVerboseString.hasToken((const utf8_t*)"ZVB_Net"))
        wV |= ZVB_Net;
    if (pVerboseString.hasToken((const utf8_t*)"ZVB_NetStats"))
        wV |= ZVB_NetStats;
    if (pVerboseString.hasToken((const utf8_t*)"ZVB_Mutex"))
        wV |= ZVB_Mutex;

    if (pVerboseString.hasToken((const utf8_t*)"ZVB_Xml"))
        wV |= ZVB_Xml;

    if (pVerboseString.hasToken((const utf8_t*)"ZVB_ZRF"))
        wV |= ZVB_ZRF;
    if (pVerboseString.hasToken((const utf8_t*)"ZVB_ZMF"))
        wV |= ZVB_ZMF;

    if (pVerboseString.hasToken((const utf8_t*)"ZVB_MemEngine"))
        wV |= ZVB_MemEngine;
    if (pVerboseString.hasToken((const utf8_t*)"ZVB_FileEngine"))
        wV |= ZVB_FileEngine;
    if (pVerboseString.hasToken((const utf8_t*)"ZVB_SearchEngine"))
        wV |= ZVB_SearchEngine;
    return wV;
}