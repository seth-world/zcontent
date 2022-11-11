#ifndef ZFILEGENERATE_H
#define ZFILEGENERATE_H

#include <ztoolset/zstatus.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/uristring.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zarray.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zaierrors.h>
#include <zcontent/zindexedfile/zmfdictionary.h>

#define __GENERATE_PARAMETER_FILE__ "zcppgenerateparameters.xml"

namespace zbs {

class ZDictionaryFile;

class ZFileGenerate
{
public:
  ZFileGenerate(ZDictionaryFile* pDictionaryFile);

  ZStatus toXmlFileDefinition(const ZMFDictionary& pDictionary,uriString& pXmlFile);

  ZStatus fileGenerate (const ZMFDictionary& pDictionary,const uriString& pOutMasterFile);
  ZStatus fileGenerate (const utf8VaryingString& pXmlFileDefinition,const uriString& pOutMasterFile);


private:
  ZDictionaryFile* DictionaryFile=nullptr;
};

} // namespace zbs
#endif // ZFILEGENERATE_H
