#ifndef ZSEARCHFILEENTITY_H
#define ZSEARCHFILEENTITY_H

#include <memory> // for shared pointers
#include <ztoolset/zstatus.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zdatabuffer.h>

#include "zmasterfile.h"

#include "zsearchtoken.h"

using namespace zbs;

namespace zbs {

class ZSearchMasterFile : public std::enable_shared_from_this <ZSearchMasterFile>
{
public:
    ZSearchMasterFile()=default;
    ZSearchMasterFile(ZSearchMasterFile&)=delete;

    ~ZSearchMasterFile()
    {
        if (_MasterFile.isOpen())
            _MasterFile.zclose();
    }

    ZStatus set(const uriString& pPath);

    std::shared_ptr<ZSearchMasterFile> getSharedPtr() {return shared_from_this();}     /* increase count */
    ZStatus openReadOnly(const uriString& pURI)
    {
        return _MasterFile.zopen(pURI,ZRF_Read_Only);
    }
    ZStatus openModify(const uriString& pURI)
    {
        return _MasterFile.zopen(pURI,ZRF_Modify);
    }

    const ZMetaDic* getMetaDic() const
    {
        return _MasterFile.Dictionary->getMetaDic() ;
    }

    const ZDictionaryFile& getFieldDictionary() const
    {
        return *_MasterFile.Dictionary;
    }
    const ZDictionaryFile* getFieldDictionaryPtr() const
    {
        return _MasterFile.Dictionary;
    }
    const ZArray<ZKeyDictionary*>& getKeyDictionary() const
    {
        return _MasterFile.Dictionary->KeyDic;
    }
    ZStatus close()
    {
        return _MasterFile.zclose();
    }

    size_t getRecordCount()
    {
        return _MasterFile.getRecordCount();
    }

    ZStatus  openReadOnly()
    {
        return _MasterFile.zopen(ZRF_Read_Only);
    }
    ZStatus openModify()
    {
        return _MasterFile.zopen(ZRF_Modify);
    }

    bool    isOpen()
    {
        return _MasterFile.isOpen();
    }
    bool    isOpenModify()
    {
        return (_MasterFile.getOpenMode() & ZRF_Modify)== ZRF_Modify;
    }
    bool    isOpenAtLeastReadOnly()
    {
        return (_MasterFile.getOpenMode() & ZRF_Read_Only)== ZRF_Read_Only;
    }
    zmode_type getOpenMode(){
        return _MasterFile.getOpenMode() ;
    }

    ZStatus get(ZDataBuffer& pRecord,const long pRank,zaddress_type &pAddress) {
        return _MasterFile.zgetWAddress(pRecord,pRank,pAddress);
    }

    ZStatus getByAddress(ZDataBuffer& pRecord,const zaddress_type pAddress) {
        return _MasterFile.zgetByAddress(pRecord,pAddress);
    }

    uriString getPath() {return _MasterFile.getURIContent();}

    ZMasterFile _MasterFile;
};

//class ZSearchToken;

/* a file entity must have an associated master file */
class _BaseFileEntity : public std::enable_shared_from_this <_BaseFileEntity>
{
public:
    //  ZSearchEntity() {}
    /* std::shared_ptr<ZSearchMasterFil is passed by value to increase usage count */
    _BaseFileEntity( std::shared_ptr<ZSearchMasterFile> pMasterPtr,ZSearchToken* pToken)
    {
        _MasterFile=pMasterPtr->getSharedPtr();
        setToken(pToken);
    }
    _BaseFileEntity(const _BaseFileEntity& pIn) {_copyFrom(pIn);}

    ~_BaseFileEntity() { }

    _BaseFileEntity& _copyFrom (const _BaseFileEntity& pIn) ;

    _BaseFileEntity& operator = (const _BaseFileEntity& pIn) {return _copyFrom(pIn);}

    std::shared_ptr<_BaseFileEntity> getSharedPtr() {return shared_from_this();}

    static std::shared_ptr<_BaseFileEntity> construct( std::shared_ptr<ZSearchMasterFile> pMasterPtr,ZSearchToken* pToken)
    {
        return std::make_shared<_BaseFileEntity>(pMasterPtr,pToken);
    }

    bool isValid() {return (_MasterFile!=nullptr )&& (!Name.isEmpty()); }

    /**
   * @brief set sets up Descriptor from current tokenizer array position and updates token text and type, row column and offset as well as token number
   */
    void setToken(ZSearchToken *pToken);
    void set(std::shared_ptr<ZSearchMasterFile> pMasterFile, ZSearchToken* pToken) ;
    utf8VaryingString getName( ) const ;

    bool hasFile() {return _MasterFile!=nullptr;}
    long getFieldRankbyName(const utf8VaryingString &pFieldName) const ;

    ZStatus setFile(std::shared_ptr<ZSearchMasterFile> pMasterFile);

    const ZMetaDic* getMetaDic() const
    {
        return _MasterFile->getMetaDic() ;
    }

    /*
  const ZDictionaryFile& getFieldDictionary() const;
  const ZDictionaryFile *getFieldDictionaryPtr() const;
  const ZArray<ZKeyDictionary*>& getKeyDictionary() const;
*/


    ZFieldDescription         getFieldByRank(long pRank) const;

    size_t          getRecordCount();
    ZStatus get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress);
    ZStatus getAt(ZDataBuffer& pRecord,zaddress_type pAddress);

    utf8VaryingString         Name;
    uriString                 Path;
    std::shared_ptr <ZSearchMasterFile> _MasterFile = nullptr;
};



class  ZSearchFileZEntity
{
public:
    ZSearchFileZEntity()=default;

    ZSearchFileZEntity(const ZSearchFileZEntity& pIn) {_copyFrom(pIn);}

    ZSearchFileZEntity& _copyFrom(const ZSearchFileZEntity& pIn) {
        Symbol=pIn.Symbol;
        Value=pIn.Value;
        return *this;
    }

    ZSearchFileZEntity& operator=(const ZSearchFileZEntity& pIn) {return _copyFrom(pIn);}

    utf8VaryingString Symbol;
    long              Value;
};


} // namespace zbs

#endif // ZSEARCHFILEENTITY_H
