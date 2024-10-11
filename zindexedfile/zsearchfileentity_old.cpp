#include "zsearchfileentity.h"

//#include "zsearchtoken.h"

using namespace zbs;

namespace zbs {

using namespace std;

_BaseFileEntity&
_BaseFileEntity::_copyFrom (const _BaseFileEntity& pIn) {
    Name = pIn.Name;
    Path = pIn.Path;
    _MasterFile = pIn._MasterFile->getSharedPtr();
    return *this;
}

void _BaseFileEntity::setToken(ZSearchToken* pToken)
{
    Name = pToken->Text;
}

void _BaseFileEntity::set(std::shared_ptr<ZSearchMasterFile> pMasterFile, ZSearchToken* pToken)
{
    Name = pToken->Text;
    _MasterFile=pMasterFile;
    Path = pMasterFile->getPath();
}

ZStatus _BaseFileEntity::setFile(std::shared_ptr<ZSearchMasterFile> pMasterFile)
{
    _MasterFile=pMasterFile;
    Path = pMasterFile->getPath();
}




utf8VaryingString
_BaseFileEntity::getName( ) const
{
    return Name;
}
long
_BaseFileEntity::getFieldRankbyName(const utf8VaryingString& pFieldName) const {
    return _MasterFile->getFieldDictionary().searchFieldByName(pFieldName);
}

ZFieldDescription
_BaseFileEntity::getFieldByRank(long pRank) const {
    return _MasterFile->getFieldDictionary().TabConst(pRank);
}

size_t
_BaseFileEntity::getRecordCount()
{
    return _MasterFile->getRecordCount();
}

ZStatus
_BaseFileEntity::get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress)
{
    return _MasterFile->get(pRecord,pRank,pAddress);
}
ZStatus
_BaseFileEntity::getAt(ZDataBuffer& pRecord,zaddress_type pAddress)
{
    return _MasterFile->getByAddress(pRecord,pAddress);
}
/*
const ZDictionaryFile&
_BaseFileEntity::getFieldDictionary() const
{
  return _MasterFile->getFieldDictionary();
}
const ZDictionaryFile*
_BaseFileEntity::getFieldDictionaryPtr() const
{
  return _MasterFile->getFieldDictionaryPtr();
}
const ZArray<ZKeyDictionary*>&
_BaseFileEntity::getKeyDictionary() const
{
  return _MasterFile->getKeyDictionary();
}
*/

} //namespace zbs
