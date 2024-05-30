#ifndef ZSEARCHENTITYCONTEXT_H
#define ZSEARCHENTITYCONTEXT_H
#include <memory.h>
#include "zmf_limits.h"
#include <ztoolset/ztimer.h>
#include <zcontentcommon/urfparser.h>
#include "zsearchjoinaddress.h"

class ZSearchField;

namespace zbs {

class ZSearchMasterFile;

typedef uint32_t  EFST_Type;

enum EntityFetchState : EFST_Type
{
    EFST_Nothing =      0,
    EFST_Partial =      1,
    EFST_Total   =      2,
    EFST_OutDated=      4

};

class ZSearchEntity;

/* entity context is generated by ZSearchEntity::newEntityContext() or ZSearchMasterFile::newEntityContext() */
class ZSearchEntityContext
{
public:
    ZSearchEntityContext() ;
    ZSearchEntityContext(const ZSearchEntityContext& pIn) {_copyFrom(pIn);}
    ~ZSearchEntityContext()
    {
        /*
        if (BaseContext!=nullptr)
            delete BaseContext;
        if (SlaveContext!=nullptr)
            delete SlaveContext;
        */
    }

    ZSearchEntityContext& _copyFrom(const ZSearchEntityContext& pIn) ;

    static std::shared_ptr <ZSearchEntityContext> newEntityContext(std::shared_ptr<ZSearchEntity> pEntity);
    static std::shared_ptr <ZSearchEntityContext> newEntityContext(ZSearchMasterFile *pFileEntity);

    void setCaptureTime(bool pOnOff) { CaptureTime=pOnOff; }

    URFParser* getBaseParserFromField(ZSearchField& pField) ;

    URFParser* getParserFromEntity(std::shared_ptr<ZSearchEntity> pEntity) ;

    bool isEOF()
    {
        if ((Status==ZS_EOF) || (Status== ZS_OUTBOUNDHIGH)) {
            _DBGPRINT("ZSearchEntityContext::isEOF  EOF reached.\n")
            Counter=0;
        }
        return (Status==ZS_EOF) || (Status== ZS_OUTBOUNDHIGH);
    }

    std::shared_ptr<ZSearchEntity> Entity=nullptr;
    ZSearchMasterFile*             FileEntity=nullptr;
    ZStatus                 Status=ZS_SUCCESS;
    int                     CurrentRank=-1;
    zaddress_type           LastAddress=-1;
    ZSearchJoinAddress      LastAddressJoin;
//    EFST_Type               FetchState=EFST_Nothing;
    URFParser               _URFParser;
    bool                    CaptureTime=true;
    ZTimer                  ProcessTi;
    std::shared_ptr <ZSearchEntityContext>   BaseContext=nullptr;
    std::shared_ptr <ZSearchEntityContext>   SlaveContext=nullptr;
    ZaiErrors*              ErrorLog=nullptr;
    long                    Counter=0;
}; // ZSearchEntityContext

class CSECList : public ZArray<std::shared_ptr <ZSearchEntityContext> >
{
public:
    CSECList() = default ;
    ~CSECList() {}

    std::shared_ptr <ZSearchEntityContext>
    getSEC(std::shared_ptr<ZSearchEntity> pSourceEntity) {
        for (int wi=0;wi < count(); wi++) {
            if (Tab(wi)->Entity == pSourceEntity) {
                return  Tab(wi);
            }
        }
        return nullptr;
    }

}  ;

extern CSECList SECList;



} // namespace zbs

#endif // ZSEARCHENTITYCONTEXT_H
