#ifndef ZSEARCHFILEENTITY_H
#define ZSEARCHFILEENTITY_H

#include <memory> // for shared pointers
#include <ztoolset/zstatus.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zdatabuffer.h>

#include "zmasterfile.h"

#include "zsearchtoken.h"

#include <zcontentcommon/urfparser.h>

#include "zsearchentitycontext.h"

//using namespace zbs;

namespace zbs {

/* a file entity must have an associated master file and an URFParser */

/* Class used to holds and manage a single Entity context for a shared MasterFile
 * This class is -and must stay- local to an entity while MasterFile object is unique and shared between all entities thru this class
 */

class ZSearchMasterFile : protected std::shared_ptr <ZMasterFile>
{
    typedef std::shared_ptr <ZMasterFile> _Base;
public:
    ZSearchMasterFile() {}
    ZSearchMasterFile(ZSearchMasterFile&)=delete;
    ZSearchMasterFile(std::shared_ptr<ZMasterFile> pZMF,const utf8VaryingString& pName)
    {
        this->std::shared_ptr<ZMasterFile>::operator =(pZMF);
        Name=pName;
/*
        if (pZMF->isOpen()) {
            _URFParser.setDictionary((*this)->Dictionary->getMetaDic());
        }
*/
    }
    ~ZSearchMasterFile()
    {
        if ((*this)->isOpen())
            (*this)->zclose();
    }

    std::shared_ptr <ZMasterFile> getMasterFile() { return *this; }


//    ZSearchEntityContext newEntityContext() ;


    ZStatus set(const uriString& pPath);

    std::shared_ptr<ZMasterFile> getBaseZMFPtr() {return *this;}     /* increase count */

    const ZMetaDic* getDictionary() { return (*this)->Dictionary->getMetaDic(); }

    ZStatus openReadOnly(const uriString& pURI)
    {
        ZStatus wSt =  (*this)->zopen(pURI,ZRF_Read_Only);
        if (wSt!=ZS_SUCCESS)
            return wSt;
//        _URFParser.setDictionary((*this)->Dictionary->getMetaDic());
        return wSt;
    }
    ZStatus openModify(const uriString& pURI)
    {
        ZStatus wSt = (*this)->zopen(pURI,ZRF_Modify);
        if (wSt!=ZS_SUCCESS)
            return wSt;
//        _URFParser.setDictionary(getMetaDic());
        return wSt;
    }

    const ZMetaDic* getMetaDic() const
    {
        return (*this)->Dictionary->getMetaDic() ;
    }

    ZFieldDescription getFieldByRank(long pRank) {return (*this)->Dictionary->TabConst(pRank);}



    const ZArray<ZKeyDictionary*>& getKeyDictionary() const
    {
        return (*this)->Dictionary->KeyDic;
    }
    ZStatus close()
    {
        return (*this)->zclose();
    }

    size_t getRecordCount()
    {
        return (*this)->getRecordCount();
    }

    ZaiErrors* _getErrorLog();
    void _setErrorLog(ZaiErrors* pErrorLog) {ErrorLog=pErrorLog;}

    ZStatus  openReadOnly()
    {
        return (*this)->zopen(ZRF_Read_Only);
    }
    ZStatus openModify()
    {
        return (*this)->zopen(ZRF_Modify);
    }

    bool    isOpen()
    {
        return (*this)->isOpen();
    }
    bool    isOpenModify()
    {
        return ((*this)->getOpenMode() & ZRF_Modify)== ZRF_Modify;
    }
    bool    isOpenAtLeastReadOnly()
    {
        return ((*this)->getOpenMode() & ZRF_Read_Only)== ZRF_Read_Only;
    }
    zmode_type getOpenMode(){
        return (*this)->getOpenMode() ;
    }

    /* get operations
     * NB: Current rank is updated only if operation is totally successfull */

    ZStatus getFirst(ZSearchEntityContext &pSEC, zaddress_type &pAddress);

    ZStatus getNext(ZSearchEntityContext &pSEC, zaddress_type &pAddress);

    ZStatus getByRank(ZSearchEntityContext &pSEC, const long pRank, zaddress_type &pAddress);

    ZStatus getByAddress(ZSearchEntityContext &pSEC, const zaddress_type pAddress);

    uriString getPath() {return (*this)->getURIContent();}

    utf8VaryingString getName() {return Name;}

//    URFParser                   _URFParser;
    long                        CurrentRank=0;
    zaddress_type               LastAddress=-1;
    utf8VaryingString           Name;  /* name used to declare file to query (see query clause <set file>) */
    ZaiErrors*                  ErrorLog=nullptr;
//    ZMasterFile                 _MasterFile;
};


class  ZEntitySymbol
{
public:
    ZEntitySymbol()=default;

    ZEntitySymbol(const ZEntitySymbol& pIn) {_copyFrom(pIn);}

    ZEntitySymbol& _copyFrom(const ZEntitySymbol& pIn) {
        Symbol=pIn.Symbol;
        Value=pIn.Value;
        return *this;
    }

    ZEntitySymbol& operator=(const ZEntitySymbol& pIn) {return _copyFrom(pIn);}

    utf8VaryingString Symbol;
    long              Value;
};


} // namespace zbs

#endif // ZSEARCHFILEENTITY_H
