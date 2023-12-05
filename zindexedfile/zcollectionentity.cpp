#include "zcollectionentity.h"

#include "zsearchlogicalterm.h"
//#include "zsearchcontext.h"
#include "zsearchtoken.h"
#include "zsearchentity.h"



//ZCollectionEntity::ZCollectionEntity() {}
namespace zbs {



void
ZCollectionEntity::setLogicalTerm(ZSearchLogicalTerm* pTerm)
{
    if (LogicalTerm!=nullptr)
        delete LogicalTerm;
    if (pTerm==nullptr)
        LogicalTerm = nullptr;
    else
        LogicalTerm = new ZSearchLogicalTerm(pTerm);
}

ZStatus
ZCollectionEntity::evaluateRecord(bool &pOutResult,const ZDataBuffer& pRecord)
{
    _URFParser->set(pRecord);
    /* if there is no selection clause then all records are selected */
    if (LogicalTerm==nullptr) {
        pOutResult = true;
        return ZS_EMPTY;
    }
    return LogicalTerm->evaluate(*_URFParser , pOutResult);
    //  return ZS_SUCCESS;
    // return evaluateFormula(pOutResult,Formula,pRecord,0);
}


ZStatus
ZCollectionEntity::get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress)
{
    if (pRank >= AddressList.count()) {
        return ZS_OUTBOUNDHIGH;
    }
    if (pRank < 0) {
        return ZS_INVINDEX;
    }
    ZSearchMasterFile* _BaseFile = _BaseEntity->getRootFileEntity();
    pAddress = AddressList[pRank];
    return _BaseFile->getByAddress(pRecord,AddressList[pRank]);
}


ZaiErrors*
ZCollectionEntity::_getErrorLog() {
    return _BaseEntity->_getErrorLog();
}

size_t
ZCollectionEntity::_baseGetCount()
{
    return _BaseEntity->_baseGetCount();
}
size_t
ZCollectionEntity::getCount()
{
    return AddressList.count();
}
ZCollectionEntity& ZCollectionEntity::_copyFrom(const ZCollectionEntity& pIn)
{
    _BaseEntity = pIn._BaseEntity->getSharedPtr();
    AddressList.clear();
    for (int wi=0;wi < pIn.AddressList.count();wi++)
        AddressList.push (pIn.AddressList[wi]);

    if (LogicalTerm)
        delete LogicalTerm;
    LogicalTerm = nullptr;
    if (pIn.LogicalTerm!=nullptr)
        LogicalTerm = new ZSearchLogicalTerm(*pIn.LogicalTerm);

    _URFParser = pIn._URFParser;

    return *this;
}




} // namespace zbs
