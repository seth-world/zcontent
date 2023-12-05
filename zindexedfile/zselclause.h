#ifndef ZSELCLAUSE_H
#define ZSELCLAUSE_H

#include <ztoolset/zarray.h>
#include <ztoolset/utfvaryingstring.h>

#include "zsearchparsertype.h"


/* selected field from one entity */
class ZFldSelFld
{
public:
    ZFldSelFld() {}
    ZFldSelFld(const ZFldSelFld& pIn) {_copyFrom(pIn);}

    ZFldSelFld& _copyFrom(const ZFldSelFld&pIn);

    ZFldSelFld& operator = (const ZFldSelFld&pIn) {return _copyFrom(pIn);}

    void clear();

    utf8VaryingString       Field;
    ZArray<ZSearchToken*>   TokenList;
};
/* Entity fields selection : list of selected fields dependent of one entity Entity */
class ZFldSelEntity : public ZArray<ZFldSelFld>
{
public:
    ZFldSelEntity() {}
    ZFldSelEntity(const ZFldSelEntity& pIn) {_copyFrom(pIn);}
    ~ZFldSelEntity()
    {
        while (count())
            pop();
        while (TokenList.count())
            TokenList.pop();
    }
    ZFldSelEntity& _copyFrom(const ZFldSelEntity&pIn) ;

    ZFldSelEntity& operator = (const ZFldSelEntity&pIn) {return _copyFrom(pIn);}
    void clear() ;

    bool                    AllFields=false;
    utf8VaryingString       Entity;
    ZArray<ZSearchToken*>   TokenList;
};

/* Clause : list of selection fields each of it dependent of one entity */
class ZFldSelClause : public ZArray<ZFldSelEntity>
{
public:
    ZFldSelClause() {}
    ZFldSelClause(const ZFldSelClause& pIn) {_copyFrom(pIn);}
    ~ZFldSelClause()
    {
        while (count())
            pop();
    }
    ZFldSelClause& _copyFrom(const ZFldSelClause& pIn) ;
    ZFldSelClause& operator = (const ZFldSelClause& pIn) {return _copyFrom(pIn);}
    void clear()
    {
        while (count())
            pop();
    }
};



#endif // ZSELCLAUSE_H
