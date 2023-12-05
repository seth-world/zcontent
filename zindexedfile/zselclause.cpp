#include "zselclause.h"


ZFldSelFld& ZFldSelFld::_copyFrom(const ZFldSelFld&pIn)
{
    Field = pIn.Field ;
    for (int wi=0;wi < pIn.TokenList.count();wi++)
        TokenList.push(pIn.TokenList[wi]);
    return *this;
}

void ZFldSelFld::clear()
{
    Field.clear();
    while (TokenList.count())
        TokenList.pop();
}


ZFldSelEntity& ZFldSelEntity::_copyFrom(const ZFldSelEntity&pIn)
{
    Entity = pIn.Entity ;
    AllFields = pIn.AllFields ;
    while (TokenList.count())
        TokenList.pop();
    for (int wi=0;wi < pIn.TokenList.count();wi++)
        TokenList.push(pIn.TokenList[wi]);
    while (count())
        pop();
    for (int wi=0;wi < pIn.count();wi++)
        push(pIn.TabConst(wi));
    return *this;
}

void ZFldSelEntity::clear()
{
    Entity.clear();
    AllFields=false;
    TokenList.clear();
    ZArray<ZFldSelFld>::clear();
}


ZFldSelClause& ZFldSelClause::_copyFrom(const ZFldSelClause& pIn)
{
    for (int wi=0;wi < pIn.count();wi++)
        push(pIn.TabConst(wi));
    return *this;
}
