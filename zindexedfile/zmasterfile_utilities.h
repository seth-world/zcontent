#ifndef ZMASTERFILE_UTILITIES_H
#define ZMASTERFILE_UTILITIES_H

#include <ztoolset/zutfstrings.h>
#include <zrandomfile/zrandomfile.h>
#include <zxml/zxmlprimitives.h>
/*
#include <QtXml>
*/
utf8String generateIndexRootName(utf8String &pMasterRootName,
                                 const long pRank,
                                 utf8String &pIndexName);
ZStatus
generateIndexURI(uriString pMasterFileUri,
    uriString &pDirectory,
    uriString &pZSIndexFileUri,
    const long pRank,
    utf8String& pIndexName);
/*
ZStatus getChildElementValue(QDomNode pNodeWork,const char*pTagName,QString &pContent, bool pMandatory);

ZStatus getChildElementValue(QDomNode pNodeWork,const utf8_t*pTagName,QString &pContent, bool pMandatory);
*/
ZStatus
_testXMLZFileControlBlock(ZFileControlBlock *pFCB,
                          zxmlNode* pFileDescNode,
                          long &pMissingTags,
                          bool pRealRun,
                          FILE *pOutput,
                          ZaiErrors* pErrorLog);

ZStatus _testXMLZFileDescriptor (zxmlNode *pFileNode,
                                const utf8_t *pFilePath,
                                const utf8_t *pZFile_type,
                                uriString &pURIContent,
                                FILE* pOutput,
                                ZaiErrors* pErrorLog);

const char *decode_ZCOP (uint16_t pZCOP);
#endif // ZMASTERFILE_UTILITIES_H

