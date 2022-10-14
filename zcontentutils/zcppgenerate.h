#ifndef ZCPPGENERATE_H
#define ZCPPGENERATE_H

#include <ztoolset/zstatus.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/uristring.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zarray.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zaierrors.h>



#define __GENERATE_PARAMETER_FILE__ "zcppgenerateparameters.xml"

namespace zbs {


class GenObj {
public:
  GenObj()=default;
  GenObj(ZTypeBase pZType,long pIncludeRank) {ZType=pZType; IncludeRank=pIncludeRank;}
  ZTypeBase   ZType=ZType_Nothing;
  long        IncludeRank=-1;
};

class GenInclude {
public:
  GenInclude()=default;
  GenInclude(const utf8VaryingString& pInclude) {Include=pInclude; Used=false;}
  utf8VaryingString Include;
  bool              Used = false;
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
                                ZArray<GenInclude> &pGenIncludeList,
                                ZaiErrors *pErrorLog);


  utf8VaryingString genIncludes(ZTypeBase pField,
      ZArray<GenObj> &pGenObjList,
      ZArray<GenInclude> &pGenIncludeList);

  utf8VaryingString genHeaderFields(utf8VaryingString& pFileIncludeList,ZArray<GenObj> &pGenObjList,ZArray<GenInclude> &pGenIncludeList);
  utf8VaryingString genCopyFrom(utf8VaryingString& pClassName);
  ZStatus generateInterface(const utf8VaryingString &pOutName, const utf8VaryingString &pClassName, const utf8VaryingString &pBrief);

  uriString getGenPath() {return GenPath;}

private:
  ZDictionaryFile* DictionaryFile=nullptr;
  ZArray<GenObj>      wGenObjList;
  ZArray<GenInclude>  wGenIncludeList;

  uriString           GenPath;
};

} // namespace zbs

#endif // ZCPPGENERATE_H
