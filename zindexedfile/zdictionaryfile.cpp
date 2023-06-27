#include "zdictionaryfile.h"

using namespace zbs;

#include <zcontent/zindexedfile/zmfdictionary.h>

#include <ztoolset/zfunctions.h>
#include <zrandomfile/zrfutilities.h> // for removeFile()

ZDictionaryFile::ZDictionaryFile()
{

}

ZDictionaryFile& ZDictionaryFile::_copyFrom(const ZDictionaryFile& pIn) {
  URIDictionary = pIn.URIDictionary;
  ZMFDictionary::_copyFrom(pIn);
  return *this;
}


/** @brief save save embedded content to a valid, existing dictionary file.

Process
  -------------------Not handled by this routine------------------------
  dictionary file does not exist         create and don't care active (zinitialize)
  ----------------------------------------------------------------------

  dictionary file exits

  Dic content to write is not active :
      [searchAndWrite] search for (DicName , Version) in file :
        not exists -> append
        exists -> replace

  Dic content to write is active :
      search for active in file :
      active not exists :
        [searchAndWrite] search for (DicName , Version) in file :
          not exists -> append
          exists -> replace
      active exists :
        (DicName , Version) are the same : replace.
        (DicName , Version) are not the same :
          found in Dictionary set to not Active, and replace
          [searchAndWrite] search for (DicName , Version) in file :
            not exists -> append
            exists -> replace

*/
ZStatus
ZDictionaryFile::create(const uriString& pDicFilename,bool pBackup){

  URIDictionary = pDicFilename;
  return save();
}// create



ZStatus
ZDictionaryFile::save(bool pBackup){
  ZDataBuffer wDicExport;

  setModified();

  _exportAppend(wDicExport);

  if (pBackup && URIDictionary.exists())
    URIDictionary.renameBck("bck");
  else
    URIDictionary.remove();

  return URIDictionary.writeContent(wDicExport);
}//save

ZStatus
ZDictionaryFile::load(){
  ZDataBuffer wDicImport;
  ZStatus wSt=URIDictionary.loadContent(wDicImport);
  if (wSt != ZS_SUCCESS) {
    return wSt;
  }
  const unsigned char* wPtrIn=wDicImport.Data;
  return _import(wPtrIn);
}//load


void
ZDictionaryFile::setDictionary(const ZMFDictionary& pDic) {
  ZMFDictionary::clear();
  ZMFDictionary::_copyFrom(pDic);
}


ZStatus
ZDictionaryFile::loadDictionary(const uriString& pDicFilename) {
  clear();
  setDicFilename(pDicFilename);
  return load();
}

utf8VaryingString  ZDictionaryFile::exportToXmlString( bool pComment)
{
  return ZMFDictionary::XmlSaveToString(pComment);

}
ZStatus ZDictionaryFile::exportToXmlFile(const uriString &pXmlFile,bool pComment)
{
  utf8VaryingString wContent = ((ZMFDictionary*)this)->XmlSaveToString(pComment);
  return pXmlFile.writeContent(wContent);
}


utf8VaryingString
ZDictionaryFile::generateDicFileName(const uriString& pURIContent){
  uriString wDicPath;
  wDicPath = pURIContent.getDirectoryPath();
  wDicPath.addConditionalDirectoryDelimiter();
  wDicPath += pURIContent.getRootname();
  wDicPath += "-";
  wDicPath += "dic";
  wDicPath += ".";
  wDicPath += __DICTIONARY_EXTENSION__;

  return (utf8VaryingString)wDicPath;
}

