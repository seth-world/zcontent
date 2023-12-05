#include "zsearchentitycontext.h"
#include <ztoolset/zaierrors.h>
#include "zsearchoperand.h"
#include "zsearchentity.h"
#include "zsearchfield.h"
#include "zsearchentity.h"


namespace zbs {
ZSearchEntityContext::ZSearchEntityContext() {}

ZSearchEntityContext
ZSearchEntityContext::newEntityContext(std::shared_ptr<ZSearchEntity> pEntity)
{
    ZSearchEntityContext wCtx ;
    wCtx.Entity = pEntity;
    wCtx._URFParser.setDictionary(&pEntity->LocalMetaDic);
    wCtx.ErrorLog = pEntity->ErrorLog;
    if (pEntity->isFile()) {
        wCtx.BaseContext = new ZSearchEntityContext(ZSearchEntityContext::newEntityContext(pEntity->_FileEntity));
    }
    if (pEntity->isCollection()) {
        wCtx.BaseContext = new ZSearchEntityContext(ZSearchEntityContext::newEntityContext(pEntity->_BaseEntity));
    }
    if (pEntity->isJoin()) {
        wCtx.BaseContext = new ZSearchEntityContext(ZSearchEntityContext::newEntityContext(pEntity->_JoinList[0]));
        wCtx.SlaveContext = new ZSearchEntityContext(ZSearchEntityContext::newEntityContext(pEntity->_JoinList[1]));
    }
    return wCtx;
}
ZSearchEntityContext
ZSearchEntityContext::newEntityContext(ZSearchMasterFile* pFileEntity)
{
    ZSearchEntityContext wCtx ;
    wCtx.FileEntity = pFileEntity;
    wCtx._URFParser.setDictionary(pFileEntity->getMetaDic());
    wCtx.ErrorLog = pFileEntity->ErrorLog;
/*
    if (pEntity->isFile()) {
        wCtx.BaseContext = new ZSearchEntityContext(ZSearchEntityContext::newEntityContext(pEntity->_FileEntity));
    }
    if (pEntity->isCollection()) {
        wCtx.BaseContext = new ZSearchEntityContext(ZSearchEntityContext::newEntityContext(pEntity->_BaseEntity));
    }
    if (pEntity->isJoin()) {
        wCtx.BaseContext = new ZSearchEntityContext(ZSearchEntityContext::newEntityContext(pEntity->_JoinList[0]));
        wCtx.SlaveContext = new ZSearchEntityContext(ZSearchEntityContext::newEntityContext(pEntity->_JoinList[1]));
    }
*/
    return wCtx;
}

ZSearchEntityContext&
ZSearchEntityContext::_copyFrom(const ZSearchEntityContext& pIn)
{
    Entity = pIn.Entity;
    /* NO: ZSearchEntity owns the management of File entity (ZSearchMasterFile)
     *      Only a copy is made and used in ZSearchEntityContext
    if (FileEntity!=nullptr) {
        zdelete(FileEntity);
    }
    if (pIn.FileEntity!=nullptr) {
        FileEntity = new ZSearchMasterFile(*pIn.FileEntity);
    }
    */
    FileEntity = pIn.FileEntity ; /* this is correct */
    Status = pIn.Status;
    CurrentRank = pIn.CurrentRank;
//    FetchState = pIn.FetchState;
    _URFParser = pIn._URFParser;
    CaptureTime = pIn.CaptureTime;
    ProcessTi = pIn.ProcessTi;
    if (pIn.BaseContext!=nullptr)
        BaseContext = new ZSearchEntityContext(*pIn.BaseContext);
    else
        BaseContext = nullptr;
    if (pIn.SlaveContext!=nullptr)
        SlaveContext = new ZSearchEntityContext(*pIn.SlaveContext);
    else
        SlaveContext = nullptr;
    ErrorLog = pIn.ErrorLog;
    return *this;
}

URFParser*
ZSearchEntityContext::getBaseParserFromField(ZSearchField& pField)
{
    if ((Entity==nullptr)&&(FileEntity==nullptr)) {
        _DBGPRINT("ZSearchEntityContext::getBaseParserFromField Entity context has no Entity neither File defined or is corrupted.");
        Status = ZS_NULLPTR;
        abort();
    }
    if (BaseContext==nullptr) {
        _DBGPRINT("ZSearchEntityContext::getBaseParserFromField Entity context has no base context defined or is corrupted.");
        Status = ZS_NULLPTR;
        abort();
    }
    if ((Entity!=nullptr)&&(Entity->isJoin())) {
        if (SlaveContext==nullptr) {
            _DBGPRINT("ZSearchEntityContext::getBaseParserFromField Entity context (join entity) has no slave context defined or is corrupted.");
            Status = ZS_NULLPTR;
            abort();
        }
        if ((BaseContext->Entity!=nullptr)&&(pField.getEntity()==BaseContext->Entity)) {
            if (BaseContext->_URFParser.URFPS!=URFPS_TotallyParsed) {
                if (BaseContext->Entity==nullptr)
                    _DBGPRINT("ZSearchEntityContext::getBaseParserFromField-E-NOTPARSED File <%s> has URF parser not totally parsed.\n",
                              BaseContext->FileEntity->getName().toString())
                else
                    _DBGPRINT("ZSearchEntityContext::getBaseParserFromField-E-NOTPARSED Entity <%s> has URF parser not totally parsed.\n",
                              BaseContext->Entity->getEntityName().toString())
            }
            return &BaseContext->_URFParser;
        }
        if ((SlaveContext->Entity!=nullptr)&&(pField.getEntity()==SlaveContext->Entity)) {
            if (SlaveContext->Entity==nullptr)
                _DBGPRINT("ZSearchEntityContext::getBaseParserFromField-E-NOTPARSED File <%s> has URF parser not totally parsed.\n",
                          SlaveContext->FileEntity->getName().toString())
            else
                _DBGPRINT("ZSearchEntityContext::getBaseParserFromField-E-NOTPARSED Entity <%s> has URF parser not totally parsed.\n",
                          SlaveContext->Entity->getEntityName().toString())

            return &SlaveContext->_URFParser;
        }
    }
    return &BaseContext->_URFParser;
}

URFParser *
ZSearchEntityContext::getParserFromEntity(std::shared_ptr<ZSearchEntity> pEntity)
{
    URFParser* wParser=nullptr;
    if ((Entity==nullptr)&&(FileEntity==nullptr)) {
        _DBGPRINT("ZSearchEntityContext::getBaseParserFromEntity Entity context has no Entity neither File defined or is corrupted.");
        Status = ZS_NULLPTR;
        abort();
    }
    while (true) {
        if (pEntity==Entity) {
            wParser = &_URFParser;
            break;
        }
        if (BaseContext==nullptr) {
            _DBGPRINT("ZSearchEntityContext::getBaseParserFromEntity Entity context has no base context defined or is corrupted.");
            Status = ZS_NULLPTR;
            abort();
        }
        if ((SlaveContext!=nullptr)&&(SlaveContext->Entity==pEntity)) {
            wParser = &SlaveContext->_URFParser;
            break;
        }
        if ((BaseContext!=nullptr)&&(BaseContext->Entity==pEntity))  {
            wParser = &BaseContext->_URFParser;
            break;
        }
        _DBGPRINT("ZSearchEntityContext::getParserFromEntity Entity <%s> has not been found in current context and its sub-contextes.",
                  pEntity->getEntityName().toCChar());
        Status = ZS_NOTFOUND;
        abort();
    } // while true

    if (wParser->URFPS < URFPS_TotallyParsed) {
        _DBGPRINT("extractOperandContent-E-NOTPARSED Given parser is not up to date while parsing field from entity <%s>.\n",
                  pEntity->getEntityName().toCChar())
        abort();
    }
    return wParser;
}//ZSearchEntityContext::getParserFromEntity

} // namespace zbs
