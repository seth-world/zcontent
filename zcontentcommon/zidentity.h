#ifndef ZIDENTITY_H
#define ZIDENTITY_H
#include <limits.h>
#include <ztoolset/zutfstrings.h>

typedef int64_t      Identity_type;

const Identity_type cst_IdentityInvalid = INT64_MIN;
const Identity_type cst_IdentityNothing = -1;


class CharMan;
class ZIdentity
{
public:
    Identity_type id = cst_IdentityNothing;

    ZIdentity() = default;
    ZIdentity(Identity_type pId) { id = pId; }
    ZIdentity(ZIdentity &pIn) { id = pIn.id; }
    ZIdentity(ZIdentity &&pIn) { id = pIn.id; }

    Identity_type value() { return id; }
    void clear(void) { id = cst_IdentityNothing; }

    bool isInvalid(void) { return (id < 0); }
    bool isValid(void) { return (!(id < 0)); }

    ZIdentity getInvalid(void) { return ZIdentity(cst_IdentityInvalid); }
    void setInvalid(void) { id = INT_MIN; }

//    static ZIdentity getNew(void) {return ZIdentity(GlobalId++);}

    utfcodeString tocodeString (void)               {utfcodeString pString ; pString.sprintf("%08lX",id); return pString;}
    ZIdentity& fromString (const char *pString,int pBase=10)  {id =strtol (pString,nullptr,pBase); return *this;}

//    ZIdentity& getNext (void) { id++; return *this; }

    ZIdentity& operator= (ZIdentity& pIn) { id = pIn.id; return(*this);}
    ZIdentity& operator= (ZIdentity&& pIn) { id = pIn.id; return(*this);}

    inline ZIdentity &operator = (unsigned long pId) {id=pId; return(*this);}
    inline ZIdentity &operator = (const char* pId)
    {
        const char* wPtr=_firstNotinSet(pId,__DEFAULTDELIMITERSET__);
        if (wPtr==nullptr)
        {
            id=0;
            return *this;
        }
        if ((memcmp(wPtr,"0x",2)==0)||(memcmp(wPtr,"0X",2)==0))
        {
            id =strtol (pId,nullptr,16); return *this;
        }
        id =strtol (pId,nullptr,10);
        return *this;
    }
    ZIdentity &operator = (const utfcodeString& pId)
    {
        fromString (pId.toCChar(),10);
        return *this;
    }

    CharMan toStr();
    CharMan toHexa();

    int compare(ZIdentity &pIn) const { return id - pIn.id; }

    inline bool operator == ( ZIdentity &pId2) {return (id==pId2.id);}
    inline bool operator == ( unsigned long pId2) {return (id==pId2);}
    inline bool operator != ( ZIdentity &pId2) {return !(id==pId2.id);}
    inline bool operator != ( unsigned long pId2) {return !(id==pId2);}

#ifdef QT_CORE_LIB
    QString toQString(void);
    QString toQStringShort(void);
    QString toQStringLong(void);
    ZIdentity &fromQString(const QString pQString);
    ZIdentity &operator = (const QString pQString) {id = pQString.toLong(nullptr,16);return *this;}
#endif // QT_CORE_LIB

};

#endif // ZIDENTITY_H
