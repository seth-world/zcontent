#ifndef ZCPPGENERATE_H
#define ZCPPGENERATE_H

#include <ztoolset/zstatus.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/uristring.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zarray.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zaierrors.h>



#define __CPPGENERATE_PARAMETER_FILE__ "zcppgenerateparameters.xml"

#define __CPPGENERATE_VERSION__ "1.1-0"

namespace zbs {


class GenObj {
public:
  GenObj()=default;
  GenObj(ZTypeBase pZType,const utf8VaryingString& pInclude) {ZType=pZType; Include=pInclude; }
  GenObj(ZTypeBase pZType,long pIncludeRank) {ZType=pZType; IncludeRank=pIncludeRank;}
  ZTypeBase         ZType=ZType_Nothing;
  utf8VaryingString Include;
  bool              Used = false;
  long              IncludeRank=-1;
};

class GenInclude {
public:
  GenInclude()=default;
  GenInclude(const utf8VaryingString& pInclude) {Include=pInclude; Used=false;}
  utf8VaryingString Include;
  bool              Used = false;
};


class genReport {
public:
  genReport()=default;

  genReport(const genReport& pIn) {_copyFrom(pIn);}

  genReport& operator = (const genReport& pIn) {return _copyFrom(pIn);}

  genReport& _copyFrom(const genReport& pIn) {
    Fields = pIn.Fields;
    ErrFields = pIn.ErrFields;
    ErrInstructions = pIn.ErrInstructions;
    return *this;
  }

  int Fields=0;
  int ErrFields=0;
  int ErrInstructions=0;
};



class ZDictionaryFile;
/*
<?xml version='1.0' encoding='UTF-8'?>
 <zcppgenerate version = "'0.30-0'">
     <includefiles>
        <object>
            <ztype>ZType_Utf8FixedString</type>
            <file>ztoolset/utffixedstring.h</file>
         </object>
    </includefiles>
  </zcppgenerate>
*/
class ZCppGenerate
{
public:
  ZCppGenerate(ZDictionaryFile* pDictionaryFile);

  ZStatus loadGenerateParameters(const uriString& pXmlFile,ZaiErrors *pErrorLog);

  ZStatus loadXmlParameters(const uriString& pXmlFile,
                                ZArray<GenObj> &pGenObjList,
                                ZaiErrors *pErrorLog);


  int getOneIncludeFiles(ZTypeBase pType);
  void resetIncludeFiles();
  void setupIncludeFiles();

  utf8VaryingString genIncludes();

  utf8VaryingString genHeaderFields(utf8VaryingString& pFileIncludeList);

  ZStatus genHeader(const utf8VaryingString& pClassName,
                              const uriString &pHeaderFile,
                              const utf8VaryingString &pBrief);
  /* generates all keys declarative stuff */
  utf8VaryingString genHeaderKeys() ;
  /* generates all keys management body routines */
  utf8VaryingString genCppKeys(const utf8VaryingString& pClassName);
  /* generates the void clear() body method */
  utf8VaryingString genCppClear(  const utf8VaryingString& pClassName);
  ZStatus genCpp(const utf8VaryingString& pClassName, const uriString &pHeaderFile, const uriString& pCppFile);

  ZStatus generateInterface(const utf8VaryingString &pOutBase,
                            const utf8VaryingString &pClassName,
                            const utf8VaryingString &pBrief,
                            const uriString &pGenPath);

  uriString getGenPath() {return GenPath;}

  uriString getGenCppFile() {return GenCppFile;}
  uriString getGenHeaderFile() {return GenHeaderFile;}

  bool testErrored(long pRank);
  void setXmlDictionaryFile(const utf8VaryingString& pXmlDictionaryFile) {XmlDictionaryFile=pXmlDictionaryFile;}
private:
  ZDictionaryFile*    DictionaryFile=nullptr;
  uriString           XmlGenParamsFile;
  uriString           XmlDictionaryFile;
  ZArray<GenObj>      GenObjList;
 // ZArray<GenInclude>  GenIncludeList;

  ZArray<long> ErroredFields;

  uriString           GenCppFile;
  uriString           GenHeaderFile;

  uriString           GenPath;
  utf8VaryingString   CommentBanner;
  ZaiErrors           ErrorLog;
};

} // namespace zbs


ZStatus ZTypeToCTypeDefinition(ZTypeBase pType,
                                long pCapacity,
                                const utf8VaryingString& pName,
                                utf8VaryingString &pDeclaration,
                                ZaiErrors* pErrorLog);

#endif // ZCPPGENERATE_H
