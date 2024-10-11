#include "zdomainbroker.h"
#include <ztoolset/zaierrors.h>

namespace zbs {

class ZDomainBroker DomainBroker ;

ZDomainBroker::ZDomainBroker()
{
    DomainRoot = new ZDomainPathRoot ;
}

ZDomainBroker&
ZDomainBroker::_copyFrom(const ZDomainBroker& pIn)
{
    //   DomainList.clear();
    //   for (int wi=0 ; wi < pIn.DomainList.count() ; wi++)
    //       DomainList.push(new ZDomainPath(*pIn.DomainList[wi]));
    if (DomainRoot!=nullptr)
        delete DomainRoot;
    DomainRoot=nullptr;
    //    if (pIn.DomainRoot!=nullptr)
    DomainRoot = new ZDomainPathRoot(*pIn.DomainRoot);
    return *this;
}

void ZDomainBroker::clear()
{
    //    while (DomainList.count()>0)
    //        delete (DomainList.popR());
    if (DomainRoot!=nullptr)
        delete DomainRoot;
}

void
ZDomainBroker::breakDomainPath(const utf8VaryingString& pDomainPath,ZArray<utf8VaryingString>& pDomainList)
{
    pDomainList.clear();
    utf8_t* wPtr = utfStrdup(pDomainPath.Data) ;
    utf8_t* wToBeFreed=wPtr;
    utf8_t* wPtrStart=wPtr;
    bool wFileNameFollows = false ;
    int wSize=0;
    while (*wPtr && (*wPtr != '/') ) {
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( (*wPtr=='.') ||(*wPtr != '/') ) {
            if (*wPtr == '/') {
                wFileNameFollows = true ;
            }
            *wPtr='\0';
            pDomainList.push(wPtrStart);
            wPtr++;
            wPtrStart=wPtr;
            wSize=0;
        }
    }
    if (wSize > 0)
        pDomainList.push(wPtrStart);
    free(wToBeFreed);
}// ZDomainBroker::breakDomainPath

void
ZDomainBroker::breakDomainPath(const utf8VaryingString& pDomainPath,ZArray<utf8VaryingString>& pDomainList,utf8VaryingString& pFileName)
{
    pDomainList.clear();
    pFileName.clear();
    utf8_t* wPtr = utfStrdup(pDomainPath.Data) ;
    utf8_t* wToBeFreed=wPtr;
    utf8_t* wPtrStart=wPtr;
    bool wFileNameFollows = false ;
    int wSize=0;
    while (*wPtr && (*wPtr != '/') && !wFileNameFollows ) {
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( (*wPtr=='.') ||(*wPtr == '/') ) {
            if (*wPtr == '/') {
                wFileNameFollows = true ;
            }
            *wPtr='\0';
            utf8VaryingString wDom = wPtrStart; // debug
            pDomainList.push(wPtrStart);
            wPtr++;
            wPtrStart=wPtr;
            wSize=0;
        }
    }
    if (wFileNameFollows)
        pFileName = wPtrStart;
    else {
        if (*wPtrStart != '\0') {
            pDomainList.push(wPtrStart);
        }
    }

    free(wToBeFreed);
}// ZDomainBroker::breakDomainPath
ZDomainPath*
ZDomainBroker::_breakDomainPathName(const utf8VaryingString& pInPath,
                                    ZaiErrors* pErrorLog)
{
    //    pFileName.clear();
    utf8_t* wToBeFreed = utfStrdup(pInPath.Data) ;
    utf8_t* wPtr = wToBeFreed ;
    utf8_t* wPtrStart=wPtr;

    int wSize=0;
    ZDomainPath* wDPCurrent = DomainRoot;

    bool wEndOfString = false;
    bool wFileNameFollows = false;
    while (!wEndOfString  ) {
        wPtrStart = wPtr;
        if (wFileNameFollows) {
            //            pFileName += wPtrStart ; /* take all remaining string after slash sign */
            break ;
        }
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( *wPtr == '\0' )
            wEndOfString = true;

        if (*wPtr == '/')
            wFileNameFollows = true ;

        *wPtr='\0';

        utf8VaryingString wDom = wPtrStart; // debug
        bool wFound = false;
        for (int wi=0; wi < wDPCurrent->childrenCount(); wi++) {
            if (wDPCurrent->child(wi)->getName() == wDom) {
                wPtr++;
                wPtrStart=wPtr;
                wSize=0;
                wDPCurrent = wDPCurrent->child(wi);
                wFound=true;
                break ;
            }
        } // for
        if (wFound)
            continue;
        {
            pErrorLog->errorLog("breakDomainPathName::_breakDomain-E-INVNAME Domain node name <%s> cannot be found within domain hierarchy <%s>\n"
                                "                           Invalid domain path %s",
                                wDom.toCChar(),wDPCurrent->getHierarchy().toCChar(),pInPath.toCChar());
            free(wToBeFreed);
            return nullptr;
        }
    } //while (*wPtr && (*wPtr != '/') && !wFileNameFollows )
    free(wToBeFreed);
    return wDPCurrent;
}// ZDomainBroker::breakDomainPathName


#ifdef __COMMENT__
ZDomainPath*
ZDomainBroker::findFatherDomain(ZDomainPath* pPtrToSearch)
{
    for (int wi=0; wi < DomainList.count(); wi++) {
        if (pPtrToSearch==DomainList[wi])
            return nullptr ;  // if pPtrToSearch is found within first rank domains then no superior
    }
    return whoHasInferior(pPtrToSearch);
}

ZDomainPath*
ZDomainBroker::whoHasInferior(ZDomainPath* pPtrToSearch)
{
    for (int wi=0; wi < DomainList.count(); wi++) {
        for (int wj=0 ; wj < DomainList[wi]->getChildrenCount(); wj++) {
            if (pPtrToSearch==DomainList[wi]->Inferiors[wj]) {
                return DomainList[wi];
            }
        }
    }
    return nullptr;
}
#endif // __COMMENT__

ZDomainPath*
ZDomainBroker::_whoHasInferior(ZDomainPath* pDomain,ZDomainPath* pPtrToSearch)
{
    ZDomainPath* wReturn=nullptr;
    for (int wi=0 ; wi < pDomain->childrenCount(); wi++) {
        if (pPtrToSearch==pDomain->child(wi)) {
            return pDomain;
        }
        wReturn=_whoHasInferior(pDomain->child(wi),pPtrToSearch);
        if (wReturn !=nullptr)
            return wReturn;
    }
    return nullptr;
}// ZDomainBroker::_whoHasInferior


ZDomainObject
ZDomainBroker::constructFromPath(const utf8VaryingString& pDomainPath,ZaiErrors* pErrorLog)
{
    ZDomainObject wObject;
    ZStatus wSt = wObject.constructFromString(pDomainPath,pErrorLog);
    if (wSt != ZS_SUCCESS)
        return ZDomainObject() ;
    return wObject;
}

ZDomainPath*
ZDomainBroker::searchLevelRegardless(const utf8VaryingString& pDPName)
{
    return DomainRoot->search(pDPName);
}

ZDomainPath*
ZDomainBroker::whoHasInferior(ZDomainPath* pPtrToSearch)
{
    return _whoHasInferior(DomainRoot,pPtrToSearch);
}

ZStatus
ZDomainBroker::_breakDomain(const utf8VaryingString& pInPath,
                            ZArray<ZDomainPath*>& pDPList,
                            utf8VaryingString& pFileName,
                            ZaiErrors* pErrorLog)
{
    pDPList.clear();
    pFileName.clear();
    utf8_t* wToBeFreed = utfStrdup(pInPath.Data) ;
    utf8_t* wPtr = wToBeFreed ;
    utf8_t* wPtrStart=wPtr;

    int wSize=0;
    ZDomainPath* wDPCurrent = DomainRoot;

    bool wEndOfString = false;
    bool wFileNameFollows = false;
    while (!wEndOfString  ) {
        wPtrStart = wPtr;
        if (wFileNameFollows) {
            pFileName += wPtrStart ; /* take all remaining string after slash sign */
            break ;
        }
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( *wPtr == '\0' )
            wEndOfString = true;

        if (*wPtr == '/')
            wFileNameFollows = true ;

        *wPtr='\0';

        utf8VaryingString wDom = wPtrStart;
        bool wFound = false;
        for (int wi=0; wi < wDPCurrent->childrenCount(); wi++) {
            if (wDPCurrent->child(wi)->getName() == wDom) {
                pDPList.push(wDPCurrent->child(wi));
                wPtr++;
                wPtrStart=wPtr;
                wSize=0;
                wDPCurrent = wDPCurrent->child(wi);
                wFound=true;
                break ;
            }
        } // for
        if (wFound)
            continue;
        {
            pErrorLog->errorLog("ZDomainBroker::_breakDomain-E-INVNAME Domain node name <%s> cannot be found within domain hierarchy <%s>\n"
                                "                           Invalid domain path <%s>",
                                wDom.toCChar(),wDPCurrent->getHierarchy().toCChar(),pInPath.toCChar());
            free(wToBeFreed);
            return ZS_INVNAME;
        }
    } //while (*wPtr && (*wPtr != '/') && !wFileNameFollows )

    free(wToBeFreed);
    return ZS_SUCCESS;
}// ZDomainBroker::breakDomain


bool
ZDomainBroker::domainChainExists(const utf8VaryingString& pDomainChain)
{
    utf8_t* wToBeFreed = utfStrdup(pDomainChain.Data) ;
    utf8_t* wPtr = wToBeFreed ;
    utf8_t* wPtrStart=wPtr;

    int wSize=0;
    ZDomainPath* wDPCurrent = DomainRoot;

    bool wEndOfString = false;
    bool wFileNameFollows = false;
    while (!wEndOfString  ) {
        wPtrStart = wPtr;
        if (wFileNameFollows) {
            free(wToBeFreed);
            return false;
//            break ;
        }
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( *wPtr == '\0' )
            wEndOfString = true;

        if (*wPtr == '/') {
            wFileNameFollows = true ;
        }
        *wPtr='\0';

        utf8VaryingString wDom = wPtrStart;
        bool wFound = false;
        for (int wi=0; wi < wDPCurrent->childrenCount(); wi++) {
            if (wDPCurrent->child(wi)->getName() == wDom) {
                wPtr++;
                wPtrStart=wPtr;
                wSize=0;
                wDPCurrent = wDPCurrent->child(wi);
                wFound=true;
                break ;
            }
        } // for
        if (wFound)
            continue;
        {
            free(wToBeFreed);
            return false;
        }
    } //while (*wPtr && (*wPtr != '/') && !wFileNameFollows )

    free(wToBeFreed);
    return true;
}// ZDomainBroker::domainChainExists

ZDomainPath*
ZDomainBroker::getFromDomainChain(const utf8VaryingString& pDomainChain)
{
    utf8_t* wToBeFreed = utfStrdup(pDomainChain.Data) ;
    utf8_t* wPtr = wToBeFreed ;
    utf8_t* wPtrStart=wPtr;

    int wSize=0;
    ZDomainPath* wDPCurrent = DomainRoot;

    bool wEndOfString = false;
    bool wFileNameFollows = false;
    while (!wEndOfString  ) {
        wPtrStart = wPtr;
        if (wFileNameFollows) {
            free(wToBeFreed);
            return nullptr;
            //            break ;
        }
        while(*wPtr && (*wPtr != '.') && (*wPtr != '/') ) {
            wPtr++;
            wSize++;
        }
        if ( *wPtr == '\0' )
            wEndOfString = true;

        if (*wPtr == '/') {
            wFileNameFollows = true ;
        }
        *wPtr='\0';

        utf8VaryingString wDom = wPtrStart;
        bool wFound = false;
        for (int wi=0; wi < wDPCurrent->childrenCount(); wi++) {
            if (wDPCurrent->child(wi)->getName() == wDom) {
                wPtr++;
                wPtrStart=wPtr;
                wSize=0;
                wDPCurrent = wDPCurrent->child(wi);
                wFound=true;
                break ;
            }
        } // for
        if (wFound)
            continue;
        {
            free(wToBeFreed);
            return nullptr;
        }
    } //while (*wPtr && (*wPtr != '/') && !wFileNameFollows )

    free(wToBeFreed);
    return wDPCurrent;
}//  ZDomainBroker::getFromDomainChain

} // namespace zbs
