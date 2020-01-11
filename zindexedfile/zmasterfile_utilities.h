#ifndef ZMASTERFILE_UTILITIES_H
#define ZMASTERFILE_UTILITIES_H
#include <QtXml>
#include <ztoolset/zutfstrings.h>
#include <zrandomfile/zrandomfile.h>

utfdescString& generateIndexRootName(utfdescString &pMasterRootName,
                              const long pRank,
                              utfdescString &pIndexName);
ZStatus
generateIndexURI(uriString pMasterFileUri,
                 uriString &pDirectory,
                 uriString &pZSIndexFileUri,
                 const long pRank,
                 utfdescString& pIndexName);

ZStatus getChildElementValue(QDomNode pNodeWork,const char*pTagName,QString &pContent, bool pMandatory);

ZStatus getChildElementValue(QDomNode pNodeWork,const utf8_t*pTagName,QString &pContent, bool pMandatory);

ZStatus
_testXMLZFileControlBlock(ZFileControlBlock *pFCB,
                          QDomNode pFirstNode,
                          long &pMissingTags,
                          bool pRealRun,
                          FILE*pOutput);

ZStatus _testXMLZFileDescriptor (QDomNode pFistNode,
                                 const utf8_t *pFilePath,
                                 const utf8_t*pZFile_type,
                                 uriString &pURIContent,
                                 FILE* pOutput)
;
const char *decode_ZCOP (uint16_t pZCOP);
#endif // ZMASTERFILE_UTILITIES_H

