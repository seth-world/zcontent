#ifndef ZGENERALPARAMETERS_H
#define ZGENERALPARAMETERS_H
#include <ztoolset/zstatus.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/zarray.h>
#include <ztoolset/uristring.h>

#define __PARSER_WORK_DIRECTORY__  "zparserworkdir"
#define __PARSER_PARAM_DIRECTORY__  "zparserparamdir"
#define __PARSER_ICON_DIRECTORY__ "zparsericondir"

#define __WORK_DIRECTORY__  "zparserworkdir"
#define __PARAM_DIRECTORY__  "zparserparamdir"
#define __ICON_DIRECTORY__ "zparsericondir

#define __GENERAL_PARAMETERS_FILE__ "generalparameters.xml"

class ZaiErrors;

namespace zbs {
class ZCppGenerate;

class ZGeneralParameters
{
public:
    ZGeneralParameters();
    ZGeneralParameters(int argc, char *argv[]);
    ZGeneralParameters(const ZGeneralParameters& pIn) { _copyFrom(pIn);}

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


    const char* getWorkDirectoryChar() {return WorkDirectory.toCChar();}
    const char* getParamDirectoryChar() {return ParamDirectory.toCChar();}
    const char* getIconDirectoryChar() {return IconDirectory.toCChar();}

    uriString currentXml;
    uriString WorkDirectory;
    uriString ParamDirectory;
    uriString IconDirectory;

    utf8VaryingString FixedFont;
    bool    Init=false;
};

extern ZGeneralParameters GeneralParameters;

} // namespace zbs

/*
const char* getWorkDirectory();
const char* getParamDirectory();
const char* getIconDirectory();
*/

#endif // ZGENERALPARAMETERS_H
