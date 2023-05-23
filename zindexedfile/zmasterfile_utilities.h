#ifndef ZMASTERFILE_UTILITIES_H
#define ZMASTERFILE_UTILITIES_H



#include <ztoolset/zutfstrings.h>
#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zindexdata.h>

/*
#include <QtXml>
*/


namespace zbs
{
class ZMFDictionary;
class ZKeyDictionary;
}


namespace zbs {
class ZRawMasterFile;
class ZMasterFile;
}

utf8String generateIndexRootName(const utf8String &pMasterRootName,
                                 const utf8String &pIndexName);
/* see zrawmasterfileutils.h
ZStatus
generateIndexURI(uriString &pIndexFileUri,
                const uriString pMasterFileUri,
                const uriString &pDirectory,
//                const long pRank,
                const utf8String& pIndexName);
*/
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
                                const char *pFilePath,
                                const char *pZFile_type,
                                uriString &pURIContent,
                                FILE* pOutput,
                                ZaiErrors* pErrorLog);


ZStatus _XMLLoadAndControl(const char *pFilePath,
                          zxmlDoc* &    pXmlDoc,
                          zxmlElement* &pRootElement,
                          const char *pRootName,
                          const char *pVersion,
                          ZaiErrors*   pErrorLog,
                          FILE*        pOutput);



/**
 * @brief createFromXml loads and controls an Xml definition for all Master File components and then create a Master File
 * (Raw master file or Master file depending on dictionary existence or not).
 *
 * if Master dictionary cannot be loaded then file type is forced to be Raw master file whatever file type mentionned in xml parameters.
 *
 * @param[in] pXMLPath         a valid file path pointing to an xml file full definition
 * @param[in] pContentFilePath the ZRawMasterFile file path to generate the definition for. if omitted (nullptr) then the field <URIContent> from XML definition file will be taken as file path.
 * @param[in] pRealRun         if set to true, then file creation using xml file will be effective.\n
 *                             if set to false, then only a test run with report will be done to evaluate the possible result.\n
 *                             All tests and control will be done on Xml definition.
 * @param[in] pReplace  if set to true, pContentFilePath will be renamed and replace if exists.
 * @param[in] pLogfile  defaulted to nullptr.\nIf set to nullptr, then an xml file is generated with name <directory path><base name>.xmllog
 * @return a ZStatus
 *  - ZS_FILEEXIST file to be created already exists and option pReplace is set to false.
 *  - ZS_FILEERROR ZStatus returned by ZRandomFile::renameBck()
 *
 *
 */

ZStatus
createMasterFileFromXml(const char* pXMLPath,
    const char *pContentFilePath,
    bool pRealRun,
    bool pReplace, bool pCheckHash,
    const char* pLogfile);

#ifdef __DEPRECATED__
/**
 * @brief ZRawMasterFile::zapplyXMLFileDefinition Tests or applies an xml file give by pFilePath to change parameters of an <b>existing</b> file
 * @param[in] pXMLPath         a valid file path pointing to an xml file full definition
 * @param[in] pContentFilePath the ZRawMasterFile file path to generate the definition for. if omitted (nullptr) then the field <URIContent> from XML definition file will be taken as file path.
 * @param[in] pRealRun         if set to true, then modifications from xml file will be applied to the file.\n
 *                             if set to false, then only a test run with report will be done to evaluate the possible changes.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.\n
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.xmllog
 * @return
 */
ZStatus
applyXMLFileDefinition(const char* pXMLPath,
                        const char *pContentFilePath,
                        bool pRealRun,
                        const char *pLogfile=nullptr);

ZStatus
applyXmltoFile(const char* pXMLPath,
              const char *pContentFilePath,
              bool pRealRun,
              const char* pLogfile=nullptr);


ZStatus
applyXMLDictionaryChange(ZRawMasterFile* pMasterFile, zxmlElement* pRoot,bool pRealRun,ZaiErrors* pMessageLog);
#endif //__DEPRECATED__
/**
 * @brief createZRandomFileFromXml loads an Xml definition, checks it, and if valid, creates or replaces(depending on pReplace) a ZRandomFile with loaded parameters.
 * @param pXMLPath
 * @param pContentFilePath
 * @param pRealRun
 * @param pLogfile
 * @return
 */
ZStatus
createZRandomFileFromXml(const char* pXMLPath,
                          const char *pContentFilePath,
                          bool pRealRun,
                          const char *pLogfile=nullptr);



ZStatus
createDicFromXmlDefinition( ZRawMasterFile& pMasterFile,
                            ZMFDictionary* wDictionary,
                            ZArray<IndexData_st>* wIndexData,
                            bool pRealRun,
                            ZaiErrors* pErrorLog);

/**
 * @brief validateXmlDicDefinition checks wether ICB indexes are aligned with key dictionary
 * @param[in] pDictionary the whole master dictionary structure to check
 * @param[in] pIndexData  ICB Index table
 * @param pErrorLog
 * @return a ZStatus
 * ZS_SUCCESS       : Key dictionary is aligned with ICB indexes.
 *                    This routine returns only a ZStatus, and does not change or allocate data structures.
 *
 * ZS_NULLPTR       : pDictionary is nullptr-> no master dictionary defined
 * ZS_BADDIC        : Number of indexes does not match nubmer of defined dictionary keys
 * ZS_DUPLICATEKEY  : Two dictionary keys match the same ICB index name. Corrupted key dictionary.
 * ZS_INVSIZE       : Dictionary key universal size does not match index size. Key dictionary is malformed or ICB index is malformed.
 * ZS_NOTFOUND      : ICB index name not found within key dictionary. Key dictionary is incomplete.
 *
 */
ZStatus validateXmlDicDefinition(ZMFDictionary* wDictionary, ZArray<IndexData_st> *wIndexData, ZaiErrors* pErrorLog);
/**
 * @brief loadXMLDictionary reads all element from a DOM element root
 * @param wMCBNode
 * @param pDictionary
 * @param pErrorLog
 * @return a ZStatus
 * ZS_SUCCESS   : Meta dictionary (fields definition) and Key dictionary has been correctly loaded into pDictionary.
 *                in this case, pDictionary is returned as a structure allocated by new, and MUST BE DEALLOCATED by callee.
 *
 * ZS_INVVALUE  : pDictionary has not be initialized to nullptr and may cause a memory problem. Nothing is done.
 * ZS_NOTFOUND  : No Master Dictionary has been found under given xml root node. pDictionary is set to nullptr.
 * ZS_BADDIC    : No Meta dictionary node has been found under Master Dictionary node. XML structure is corrupted.
 * ZS_XMLERROR  : Meta dictionary has been found but an xml problem occurred during loading of fields definition.
 *
 * ZS_NOTFOUND  : No key dictionary found. pDictionary exists with a valid fields definition
 *                but KeyDic has count() set to zero (no key).
 * ZS_XMLEMPTY  : Key dictionary is malformed. No key definition found.pDictionary exists with a valid fields definition
 *                but KeyDic has count() set to zero (no key).
 *
 * In addition to returned ZStatus :
 *
 *  When Meta Dictionary is errored or does not exist, pDictionary is set to nullptr
 *  When KeyDictionary is errored or does not exist, pDictionary->KeyDic.count() returns zero
 *
 */
ZStatus loadXMLDictionaryForCreate(zxmlElement* pRoot, ZMFDictionary *& pDictionary, bool pCheckHash, ZaiErrors* pErrorLog);

ZStatus zmuXMLgetChild(zxmlElement* pRootNode, const char* pName, bool& pValue,  ZaiErrors* pErrorlog, bool pMandatory,ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,int& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,uint32_t& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,unsigned long& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,long& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity=ZAIES_Error);


ZStatus zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,md5& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,utf8String& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity=ZAIES_Error);


void displayKeyDicElement(ZMFDictionary* pDictionary,long pIdx,ZaiErrors* pMessageLog);
void displayKeyDicElement(ZKeyDictionary* pKeyDic,ZaiErrors* pMessageLog);

const char *decode_ZCOP (uint16_t pZCOP);
#endif // ZMASTERFILE_UTILITIES_H
