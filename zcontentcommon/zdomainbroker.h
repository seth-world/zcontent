#ifndef ZDOMAINBROKER_H
#define ZDOMAINBROKER_H

#include "zdomainpath.h"
#include "zdomainobject.h"


namespace zbs {

/*
    Domain chain

    A domain chain is a hierarchical suite of domain nodes, each of them having its own attributes.
    A domain node has a father and may have one or more children.
    within a domain chain, domain nodes are separated with a dot sign.


    Syntax :

    <domain node 1>.<domain node 2>....<domain node x>/<file name including extension>

    <domain chain>.<object name>

    Examples :

    gabu.dgabu.detail/image.png

    <gabu.dgabu.detail> are all paths objects

    translated value

    /content gabu/content dgabu/content detail/image.png

    using following rules
        - gabu is absolute or null
        - dgabu , detail are relative
            if one of the later above is absolute, then absolute content replaces already computed path


    gabu.dgabu.detail.icon
    <gabu.dgabu.detail> are all paths objects

    <icon> is an object (possibly of type icon and icon'content will contain an image file name)

    /content gabu/content dgabu/content detail/content icon


    Parsing process :

    break domain into parts

*/
//#define __DOMAIN_NOT_TO_BE_SAVED__ "local"


/*
 Domain and file name

 <domain hierarchy>/<file name>

<domain hierachy>
    <highest level domain node name>.<dependent domain>.< ...>...

<file name>
    file base name (including file name extension)

Example of valid domain file specification

    generalparameters.iconfactory.icon.QuitIcon  with type ZDOM_Icon
        is an icon object path and points to the file /home/gerard/Development/zbasetools/zqt/icons/system-shutdown.png
    generalparameters.iconfactory/system-shutown.png with type ZDOM_Path
        is a file path and points to the same file /home/gerard/Development/zbasetools/zqt/icons/system-shutdown.png
*/

class ZDomainBroker
{
public:
    ZDomainBroker();

    ZDomainBroker(const ZDomainBroker&pIn) { _copyFrom(pIn); }
    ZDomainBroker& _copyFrom(const ZDomainBroker&pIn) ;
    ~ZDomainBroker() {
        if (DomainRoot!=nullptr)
            delete DomainRoot;
    }

    ZDomainBroker& operator = (const ZDomainBroker&pIn) { return _copyFrom(pIn);}

    void clear() ;

    bool isNull()  { return DomainRoot->Inferiors.count() == 0 ; }

    ZStatus XmlLoad(uriString& pXmlFile,ZaiErrors* pErrorLog);
    ZStatus XmlLoadString(utf8VaryingString& pXmlString, ZaiErrors *pErrorLog);

    //   utf8VaryingString toXmlDomainItem(ZDomainPath *pDomain, int pLevel, ZaiErrors *pErrorLog);  // Deprecated
    utf8VaryingString toXml(int pLevel, ZaiErrors *pErrorLog);

    //    ZStatus constructPath(const utf8VaryingString &pDomainPath, uriString &pOutPath, ZDomainPath *pObject, ZaiErrors *pErrorLog);

    ZDomainObject constructFromPath(const utf8VaryingString& pDomainPath,ZaiErrors* pErrorLog);

    ZStatus constructPath(const utf8VaryingString& pDomainPath,ZDomainObject& pObject, ZaiErrors* pErrorLog);

    /* returns a pointer to father domain of pPtrToSearch (pointing to a valid domain)
     * returns nullptr if pPtrToSearch points to a first rank domain */
    //    ZDomainPath* findFatherDomain(ZDomainPath* pPtrToSearch);

    ZDomainPath* whoHasInferior(ZDomainPath* pPtrToSearch);
    ZDomainPath* _whoHasInferior(ZDomainPath* pDomain,ZDomainPath* pPtrToSearch);


    ZDomainPath* searchLevelRegardless(const utf8VaryingString& pDPName);

    ZDomainPath* searchPath(const utf8VaryingString& pPath, ZDomainPath *pFather);



    //    ZArray<ZDomainPath*> DomainList;

    ZStatus _breakDomain(const utf8VaryingString& pInPath,
                         ZArray<ZDomainPath*>& pDPList,
                         utf8VaryingString& pFileName,
                         ZaiErrors* pErrorLog);

    ZDomainPath *_breakDomainPathName(const utf8VaryingString &pInPath, ZaiErrors *pErrorLog);

    ZDomainPath* getRoot() {return DomainRoot;}


private:
    ZDomainPath* XmlGetDomain(zxmlElement* pDomainNode, ZDomainPath *pFather, int pLevel, ZaiErrors *pErrorLog);
    void breakDomainPath(const utf8VaryingString &pDomainPath, ZArray<utf8VaryingString>& pDomainList);
    void breakDomainPath(const utf8VaryingString &pDomainPath, ZArray<utf8VaryingString>& pDomainList,utf8VaryingString& pFileName);


    ZDomainPath*        DomainRoot=nullptr;
}  ;


extern class ZDomainBroker DomainBroker ;

}//namespace zbs

#endif // ZDOMAINBROKER_H
