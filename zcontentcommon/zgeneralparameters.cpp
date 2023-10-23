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
}

ZGeneralParameters::ZGeneralParameters(int argc, char *argv[])
{
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

    utf8String wReturn = fmtXMLdeclaration();
    wReturn += fmtXMLmainVersion("zmasterfileparameters",__ZMF_VERSION__,0);
    wLevel=1;
    wReturn += fmtXMLnode("generalparameters",wLevel);
    wLevel++;
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
            pErrorLog->logZException();
        }
        return wSt;
    }

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
            pErrorLog->logZException();
            pErrorLog->errorLog(
                "ZGeneralParameters::XMLLoad-E-PARSERR Xml parsing error for string <%s> ",
                wXmlString.subString(0, 25).toString());
        }
        return wSt;
    }

    wSt = wDoc->getRootElement(wRoot);
    if (wSt != ZS_SUCCESS) {
        if (pErrorLog!=nullptr)
            pErrorLog->logZException();
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
            "DicEdit::loadGenerateParameters-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            "generalparameters",
            decode_ZStatus(wSt));
        return wSt;
    }
    wSt=XMLgetChildText( wGenParamsNode,"defaultworkdirectory",WorkDirectory,pErrorLog,ZAIES_Warning);
    wSt=XMLgetChildText( wGenParamsNode,"defaultparamdirectory",ParamDirectory,pErrorLog,ZAIES_Error);
    wSt=XMLgetChildText( wGenParamsNode,"defaulticondirectory",IconDirectory,pErrorLog,ZAIES_Error);
    wSt=XMLgetChildText( wGenParamsNode,"fixedfont",FixedFont,pErrorLog,ZAIES_Warning);

    XMLderegister(wGenParamsNode);

    XMLderegister((zxmlNode *&) wParamRootNode);
    if (wSt==ZS_EOF) {
        Init=true;
        return ZS_SUCCESS;
    }
    if (wSt==ZS_SUCCESS)
        Init = true;
    currentXml = pXmlFile;
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

