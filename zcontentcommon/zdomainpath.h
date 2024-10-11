#ifndef ZDOMAINPATH_H
#define ZDOMAINPATH_H

#include "zdomaintype.h"


namespace zbs {

class ZDomainPath
{
public:
    ZDomainPath() ;
    ZDomainPath(uint32_t pType) { Type=pType; }
    ZDomainPath(uint32_t pType,const utf8VaryingString& pName, ZDomainPath* pFather, int pPos=-1) ;
    ZDomainPath(ZDomainPath& pIn) { _copyFrom(pIn); }

    ~ZDomainPath()
    {
        while (Inferiors.count()>0)
            delete (Inferiors.popR());
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

    void clear();

    bool hasViewer() {
        ZDomain_type wType = Type & ZDOM_ObjectMask;
        return (wType != ZDOM_File ) && ( wType < ZDOM_End ) ;
    }

    utf8VaryingString getName()
    {
        if ((Father==nullptr)&&(Name.isEmpty()))
            return "root";
        return Name;
    }


    ZDomainPath* search(const utf8VaryingString& pDPName);
    ZDomainPath* searchInferiorPathByName(const utf8VaryingString& pDPName);

    ZDomainPath* _searchPath(const utf8VaryingString& pPath);

    bool isRoot() const { return ( Father==nullptr ) && ( Name.isEmpty() ) ; }
    bool isNull() const { return isRoot() && (Inferiors.count()==0) && (Type == ZDOM_Nothing) ; }
    bool isValid() const { return !isNull();}

    bool removeInferior(ZDomainPath*pToRemove);

    utf8VaryingString toXml(int pLevel, ZaiErrors *pErrorLog);

    bool isViewable() {
        return (Type & ZDOM_Viewable) && !Content.isEmpty() ;
    }

    utf8VaryingString   Name;
    utf8VaryingString   Content;
    utf8VaryingString   ToolTip;
    bool                Temporary=false;
    ZDomain_type        Type = ZDOM_Nothing; // see ZDomain_type
    ZDomainPath*        Father=nullptr;

    ZArray<ZDomainPath*> Inferiors;
};

} // namespace zbs

#endif // ZDOMAINPATH_H
