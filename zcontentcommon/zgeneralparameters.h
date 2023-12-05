#ifndef ZGENERALPARAMETERS_H
#define ZGENERALPARAMETERS_H

#include <ztoolset/zstatus.h>
#include <ztoolset/zbaseparameters.h>
//#include "zarray.h"
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/uristring.h>

/*
#define __PARSER_WORK_DIRECTORY__  "zparserworkdir"
#define __PARSER_PARAM_DIRECTORY__  "zparserparamdir"
#define __PARSER_ICON_DIRECTORY__ "zparsericondir"

#define __WORK_DIRECTORY__  "zparserworkdir"
#define __PARAM_DIRECTORY__  "zparserparamdir"
#define __ICON_DIRECTORY__ "zparsericondir"

#define __GENERAL_PARAMETERS_FILE__ "generalparameters.xml"
*/
class ZaiErrors;



namespace zbs {
//class ZCppGenerate;

class ZGeneralParameters
{
public:
    ZGeneralParameters();
    ZGeneralParameters(int argc, char *argv[]);
    ZGeneralParameters(const ZGeneralParameters& pIn) { _copyFrom(pIn);}
    ZGeneralParameters(ZBaseParameters* pBase) {_BaseParameters = pBase;}
    ZGeneralParameters& _copyFrom (const ZGeneralParameters& pIn) ;

    ZGeneralParameters& operator = (const ZGeneralParameters& pIn) {return _copyFrom(pIn);}

    ZStatus XmlLoad(uriString& pXmlFile, ZaiErrors *pErrorLog=nullptr);
    ZStatus XmlSave(uriString& pXmlFile,ZaiErrors* pErrorLog=nullptr);

    bool isInit() {return Init;}
    void setInit(bool pInit=true)  {Init=pInit;}

    ZStatus setFromArg(int argc, char *argv[]);

    uriString getWorkDirectory();
    uriString getParamDirectory() ;
    uriString getIconDirectory() ;

    utf8VaryingString getFixedFont();

    ZVerbose_Base getVerbose() {return _BaseParameters->Verbose;}

    const char* getWorkDirectoryChar() {return WorkDirectory.toCChar();}
    const char* getParamDirectoryChar() {return ParamDirectory.toCChar();}
    const char* getIconDirectoryChar() {return IconDirectory.toCChar();}


    void setVerbose (ZVerbose_Base pVerbose) {_BaseParameters->Verbose = pVerbose; }
    void addVerbose (ZVerbose_Base pVerbose) {_BaseParameters->Verbose |= pVerbose; }
    void removeVerbose (ZVerbose_Base pVerbose) {_BaseParameters->Verbose &= ~pVerbose ; }
    void clearVerbose (ZVerbose_Base pVerbose) {_BaseParameters->Verbose = ZVB_NoVerbose; }

    bool VerboseBasic () {return _BaseParameters->VerboseBasic();}
    bool VerboseStat () {return _BaseParameters->VerboseStats();}
    bool VerboseThread () {return _BaseParameters->VerboseThread();}
    bool VerboseNet () {return _BaseParameters->VerboseNet();}
    bool VerboseNetStats () {return _BaseParameters->VerboseNetStats();}
    bool VerboseMutex () {return _BaseParameters->VerboseMutex();}

    bool VerboseXml() {return _BaseParameters->VerboseXml();}

    bool VerboseZRF () {return _BaseParameters->VerboseZRF();}
    bool VerboseZMF () {return _BaseParameters->VerboseZMF();}
    bool VerboseZIF () {return _BaseParameters->VerboseZIF();}

    bool VerboseMemEngine () {return _BaseParameters->VerboseMemEngine();}
    bool VerboseFileEngine () {return _BaseParameters->VerboseFileEngine();}
    bool VerboseSearchEngine () {return _BaseParameters->VerboseSearchEngine();}

    uriString currentXml;
    uriString WorkDirectory;
    uriString ParamDirectory;
    uriString IconDirectory;


    utf8VaryingString FixedFont;
    bool    Init=false;

    ZBaseParameters* _BaseParameters=nullptr;
};

extern ZGeneralParameters GeneralParameters;

} // namespace zbs

utf8VaryingString decode_Verbose(ZVerbose_Base pVerbose);
ZVerbose_Base encode_Verbose(const utf8VaryingString& pVerboseString);


/*
const char* getWorkDirectory();
const char* getParamDirectory();
const char* getIconDirectory();
*/

#endif // ZGENERALPARAMETERS_H
