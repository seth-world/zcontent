#include "zsearchdictionary.h"
#include "zsearchentity.h"

ZSearchDictionary::ZSearchDictionary()  {}
/* Deprecated
void
ZSearchDictionary::setEqualTo(std::shared_ptr<ZSearchEntity>  pSourceEntity,const ZMetaDic* pMetaDic)
{
    clear();
    for (int wi=0; wi < pMetaDic->count(); wi++) {
        push(ZSearchField(pSourceEntity,pMetaDic,wi));
    }
    BaseMetaDic = pMetaDic;
}
*/
void
ZSearchDictionary::setEqualTo(std::shared_ptr<ZSearchEntity>  pSourceEntity)
{
    clear();
    for (int wi=0; wi < pSourceEntity->BuildDic.count(); wi++) {
        push(ZSearchField(pSourceEntity,&pSourceEntity->LocalMetaDic,pSourceEntity->BuildDic[wi].getMetaDicRank()));
    }
    BaseMetaDic = &pSourceEntity->LocalMetaDic;
}

/*
ZStatus ZSearchDictionary::addByRank(long pFieldRank)
{
    if (pFieldRank < 0)
        return ZS_OUTBOUNDLOW;
    if (pFieldRank >= BaseMetaDic->count())
        return ZS_OUTBOUNDHIGH;
    push (ZSearchField(BaseMetaDic,pFieldRank));
    return ZS_SUCCESS;
}
*/

long ZSearchDictionary::getFieldRankByName(const utf8VaryingString &pFieldName) const
{
    for (int wi=0;wi < count();wi++) {
        if (pFieldName == TabConst(wi).getFieldName()) {
            return long(wi);
        }
    }
    return -1;
}
/*
void ZSearchDictionary::addAll(const ZMetaDic* pMetaDic)
{
    for (zrank_type pRank = 0;pRank < pMetaDic->count();pRank++)
        push (ZSearchField(pMetaDic->TabConst(pRank),pRank));
    BaseMetaDic=pMetaDic;
}
*/

/*
ZStatus
ZSearchDictionary::addByList(ZArray<utf8VaryingString> pFldNameList)
{
    for (long wi=0;wi < pFldNameList.count();wi++) {
        addByName(pFldNameList[wi]);
    }
    return ZS_SUCCESS;
}
*/
