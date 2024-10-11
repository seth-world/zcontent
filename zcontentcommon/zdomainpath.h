#ifndef ZDOMAINPATH_H
#define ZDOMAINPATH_H

#include "zdomaintype.h"


namespace zbs {

class ZDomainPathRoot;
/*
    Domain suppression : a ZDomain is always suppressed (and must be always suppressed) by its father
 */
class ZDomainPath
{
    friend class ZDomainObject;
    friend class ZDomainBroker;
public:
    ZDomainPath() ;

    ZDomainPath(uint32_t pType) { Type=pType; }
    ZDomainPath(uint32_t pType,ZDomainPath* pFather) { Type=pType; setFather(pFather); }
    ZDomainPath(uint32_t pType,const utf8VaryingString& pName, ZDomainPath* pFather, int pPos=-1) ;
    ZDomainPath(ZDomainPath& pIn) { _copyFrom(pIn); }

    ~ZDomainPath()
    {
        while (Inferiors.count()>0)
            delete (Inferiors.popR());
/*
         if (Father!=nullptr) {
            for (int wi=0; wi < Father->childrenCount() ; wi++) {
                if ( Name==Father->Inferiors[wi]->Name) {
                    Father->removeChild(wi);
                }
            }
        }
*/
    }

    ZDomainPath& _copyFrom(const ZDomainPath& pIn) ;

    utf8VaryingString   getHierarchy() ;

    bool isFirstLevel() {
        if (Father==nullptr)      /* root */
            return true;
        if (Father->Father==nullptr) /* or direct root's child  */
            return true;

        return false ;
    }

    ZDomainPathRoot* getRoot() ;

    void setHasChanged(bool pOnOff=true) ;
    bool hasChanged();
    void clear();
    int  CurrentIndex=0;

    utf8VaryingString& getName()
    {
/*        if ((Father==nullptr)&&(Name.isEmpty()))
            return "root";
*/
        return Name;
    }

    ZStatus setName(const utf8VaryingString& pName)
    {
        if ((Father==nullptr)&&(Name.isEmpty())) {
            return ZS_INVOP;
        }
        for (int wi=0; wi < Father->Inferiors.count(); wi++) {
            if ( Father->Inferiors.Tab(wi)->Name==pName)
                return ZS_DUPVIOLATION;
        }
        Name = pName;
        return ZS_SUCCESS;
    }
    void forceSetName(const utf8VaryingString& pName)
    {
        Name = pName;
    }

    ZDomainPath* search(const utf8VaryingString& pDPName);
    ZDomainPath* searchInferiorPathByName(const utf8VaryingString& pDPName);

    ZDomainPath* _searchPath(const utf8VaryingString& pPath);

    bool isRoot() const { return ( Father==nullptr ) && ( Name.isEmpty() ) ; }
    bool isNull() const { return isRoot() && (Inferiors.count()==0) && (Type == ZDOM_Nothing) ; }
    bool isValid() const { return !isNull();}

    bool removeInferior(ZDomainPath*pToRemove);

    ZDomain_type getObjectType() { return Type & ZDOM_ObjectMask; }

    uriString constructFullPhysicalPath();

    /** @brief constructPath builds a path from domain chain root to current domain with the following rules :
     *  if domain node is not a ZDOM_Path then it is not taken into account.
     * - relative domain : content -if not empty- is aggregated with conditional directory sign
     * - absolute domain : content replaces all previous path.
     *   If content is empty, then content is cleared until next domain in chain.
     */
    void constructPath(uriString &pOutPath, utf8VaryingString &pOutObject);

    bool isSelectedForDisplay ( ZDomain_type pObjectSelection) {
        ZDomain_type wType = Type & ZDOM_ObjectMask ;
        if ( (wType & ZDOM_Path) == ZDOM_Path )
            return true;
        if (wType & pObjectSelection)
            return true;
        return false;
    }
    bool isSelected ( ZDomain_type pObjectSelection) {
        ZDomain_type wType = Type & ZDOM_ObjectMask ;
        if (wType & pObjectSelection)
            return true;
        return false ;
    }

    utf8VaryingString toXml(int pLevel, ZaiErrors *pErrorLog);

    bool isViewable() {
        ZDomain_type wType = Type & ZDOM_ObjectMask;
        return (wType != ZDOM_Other ) &&  ( wType < ZDOM_End )  && !Content.isEmpty();
    }
    bool isEditable() {
        ZDomain_type wType = Type & ZDOM_ObjectMask;
        return (wType != ZDOM_Other) && (wType != ZDOM_Path ) && ( wType < ZDOM_End )  && !Content.isEmpty();
    }
    bool isTruePath() {
        ZDomain_type wType = Type & ZDOM_ObjectMask;
        return (wType == ZDOM_Path) && !Content.isEmpty() ;
    }
    bool isPath() {
        ZDomain_type wType = Type & ZDOM_ObjectMask;
        return (wType == ZDOM_Path)  ;
    }
    bool isRelative() {
        return !(Type & ZDOM_Absolute)  ;
    }
    bool isAbsolute() {
        return (Type & ZDOM_Absolute)  ;
    }
    bool isRemote() {
        return (Type & ZDOM_Remote)  ;
    }
    bool isLocal() {
        return !(Type & ZDOM_Remote)  ;
    }
    bool hasViewer() ;
    bool hasEditor() ;

    bool isMasterFile() { return (Type & ZDOM_MasterFile)==ZDOM_MasterFile; }

//    ZDomainPath* getFirstChild() {return ChildChain;}
    bool hasChildren() { return Inferiors.count()>0 ;}

    ZStatus addChild(ZDomainPath* pChild)
    {
        if (Name==pChild->Name) {
            _ERRPRINT("ZDomainPath::addChild-E-INVNAME Error trying to add conflicting domain name <&s> as inferior of domain <%s>\n",
                      pChild->Name.toCChar(),Name.toCChar())
            return ZS_INVNAME;
        }
        for (int wi=0; wi < Inferiors.count();wi++) {
        if (Inferiors.Tab(wi)->Name==pChild->Name) {
            _ERRPRINT("ZDomainPath::addChild-E-DUPVIOL Error trying to add duplicate domain name <%s> as inferior of domain <%s>\n",
                      pChild->Name.toCChar(),Name.toCChar())
            return ZS_DUPVIOLATION;
        }
        }// for
        pChild->Father = this;
        Inferiors.push(pChild);
        setHasChanged(true);
        return ZS_SUCCESS;
    }
    ZStatus insertChild(ZDomainPath* pChild,int wi)
    {
        if (Name==pChild->Name) {
            _ERRPRINT("ZDomainPath::insertChild-E-INVNAME Error trying to insert conflicting domain name <&s> as inferior of domain <%s>\n",
                      pChild->Name.toCChar(),Name.toCChar())
            return ZS_INVNAME;
        }
        for (int wi=0; wi < Inferiors.count();wi++) {
            if (Inferiors.Tab(wi)->Name==pChild->Name) {
                _ERRPRINT("ZDomainPath-E-DUPVIOL Error trying to insert duplicate domain name <&s> as inferior of domain <%s>\n",
                          pChild->Name.toCChar(),Name.toCChar())
                return ZS_DUPVIOLATION;
            }
        }// for
        Inferiors.insert(pChild,wi);
        setHasChanged(true);
        return ZS_SUCCESS;
    }

    void removeChild(size_t wi)
    {
        Inferiors.erase(wi);
        setHasChanged(true);
    }
    ZStatus removeChildByName(const utf8VaryingString& pName)
    {
        int wi=0;
        for ( ; wi < Inferiors.count() ; wi++ ) {
            if (pName == Inferiors[wi]->Name)
                break;
        }
        if (wi == Inferiors.count())
            return ZS_NOTFOUND;
        removeChild(wi);
        return ZS_SUCCESS;
    }
    ZStatus removeChildByContent(ZDomainPath* pChildToRemove)
    {
        int wi=0;
        for ( ; wi < Inferiors.count() ; wi++ ) {
            if (pChildToRemove == Inferiors[wi])
                break;
        }
        if (wi == Inferiors.count())
            return ZS_NOTFOUND;
        removeChild(wi);
        return ZS_SUCCESS;
    }
    void removeAllChildren()
    {
        Inferiors.erase(0,size_t(Inferiors.count()));
        setHasChanged(true);
    }

    size_t childrenCount() { return Inferiors.count(); }

    ZDomainPath* child(int wi) { return Inferiors.Tab(wi); }
    ZDomainPath* firstChild()
    {
        CurrentIndex=0;
        return Inferiors.first();
    }
    ZDomainPath* lastChild()
    {
        CurrentIndex=Inferiors.lastIdx();
        return Inferiors.last();
    }
    ZDomainPath* nextChild()
    {
        if (CurrentIndex==Inferiors.lastIdx())
            return nullptr;
        return Inferiors.Tab(++CurrentIndex);
    }

    ZDomainPath* father() { return Father; }

    utf8VaryingString getFatherName( )
    {
        if (Father==nullptr) {
            return "null";
        }
        if (Father->Name.isEmpty())
            return "null";
        return Father->getName();
    }

    bool hasFather() { return Father!=nullptr; }

    void setFather(ZDomainPath* pFather)
    {
        Father=pFather;
        Father->Inferiors.push(this);
        setHasChanged(true);
    }

    utf8VaryingString   Content;
    utf8VaryingString   ToolTip;
    bool                Temporary=false;
    ZDomain_type        Type = ZDOM_Nothing; // see ZDomain_type

private:
    utf8VaryingString       Name;
    ZDomainPath*        Father=nullptr;
 //   ZDomainPath*        Sibling=nullptr;

    ZArray<ZDomainPath*>    Inferiors;
//    ZDomainPath* ChildChain=nullptr;
};

class ZDomainPathRoot : public ZDomainPath
{
public:
    ZDomainPathRoot () : ZDomainPath(ZDOM_Root) { }
    ZDomainPathRoot (const ZDomainPathRoot& pIn) { _copyFrom(pIn); }
    ZDomainPathRoot& _copyFrom(const ZDomainPathRoot& pIn)
    {
        HasChanged = pIn.HasChanged;
        ZDomainPath::_copyFrom(pIn);
        return *this;
    }
    utf8VaryingString toXml(int pLevel, ZaiErrors *pErrorLog)
    {
        ZDomainPath::toXml(pLevel,pErrorLog);
        HasChanged=false;
    }

    bool                HasChanged=false;
};


} // namespace zbs

#endif // ZDOMAINPATH_H
