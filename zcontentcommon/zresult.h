#ifndef ZRESULT_H
#define ZRESULT_H

#include <stddef.h>  // for size_t
#include <ztoolset/zstatus.h>
#include <ztoolset/zarray.h>
#include <stdint.h> // for INT64_MIN

const long cst_InvalidRank = INT64_MIN;

enum ZSA_Action { ZSA_NoAction      =   0,
                  ZSA_Error         =   -1,
                  ZSA_Push          =   1,
                  ZSA_Push_Front    =   2,
                  ZSA_Insert        =   4,
                  ZSA_Remove        =   0x10};

class ZResult
{
public:

    ZStatus     ZSt=ZS_NOTHING ;
    ZSA_Action  ZA =ZSA_NoAction ;
    long        ZIdx=cst_InvalidRank ;
    int         ZIterations;
    ZResult() {clear();}
    ZResult(const ZResult& pIn) {_copyFrom(pIn);}
    ZResult(const ZResult&& pIn) {_copyFrom(pIn);}

    ZResult& _copyFrom(const ZResult& pIn);
    ZResult& operator = (const ZResult& pIn) {return _copyFrom(pIn);}
    ZResult& operator = (const ZResult&& pIn) {return _copyFrom(pIn);}

    void clear();

    bool isInvalid()  {return ZIdx == cst_InvalidRank;}
    bool isFound() { return ZSt == ZS_FOUND; }
    bool notFound() { return ZSt != ZS_FOUND; }
    bool isSuccess() { return (ZSt == ZS_SUCCESS)||(ZSt == ZS_FOUND); }
    bool notSuccess(){ return (ZSt != ZS_SUCCESS)&&(ZSt != ZS_FOUND); }
} ;



template <typename _KType>
struct Index_struct {
    size_t Idx ;           //index on ZSA structure
    _KType _Key;
} ;

class ZMulti_Result: public zbs::ZArray<size_t>
{
private:
    long CurrentIndex=cst_InvalidRank;
public:
    ZStatus ZSt = ZS_NOTHING;
    long ZIdx = cst_InvalidRank;
    int     ZIterations;


    ZMulti_Result(void) { clear(); }
    ~ZMulti_Result() {}

    ZMulti_Result(ZMulti_Result &pIn) { _copyFrom(pIn); }
    ZMulti_Result(ZMulti_Result &&pIn) { _copyFrom(pIn); }

    ZMulti_Result &_copyFrom(ZMulti_Result &pIn)
    {
        CurrentIndex = pIn.CurrentIndex;
        ZSt = pIn.ZSt;
        ZIdx = pIn.ZIdx;
        ZIterations = pIn.ZIterations;
        zbs::ZArray<size_t>::clear();
        for (long wi = 0; wi < pIn.count(); wi++)
            push(pIn.Tab[wi]);
        return *this;
    }
    ZMulti_Result &operator=(ZMulti_Result &pIn) { return _copyFrom(pIn); }
    ZMulti_Result &operator=(ZMulti_Result &&pIn) { return _copyFrom(pIn); }

    void clear()
    {
        zbs::ZArray<size_t>::clear();
        ZIdx = cst_InvalidRank;
        ZIterations = 0;
        ZSt = ZS_NOTHING;
    }
    long getFirst(void)
    {
        if (isEmpty()) return -1;
        CurrentIndex=0;
        return Tab[0];
    }
    long getLast(void)
    {
        if (isEmpty()) return -1;
        CurrentIndex=lastIdx();
        return CurrentIndex;
    }
    long getNext(void)
    {
        if (isEmpty() )return -1;
        if (CurrentIndex < 0)
                CurrentIndex = 0;
        if (CurrentIndex>lastIdx()) return -1;
        CurrentIndex++;
        return(Tab[CurrentIndex]);
    }

    bool isFound() { return ZSt == ZS_FOUND; }
    bool notFound() { return ZSt != ZS_FOUND; }
    bool isSuccess() { return (ZSt == ZS_SUCCESS)||(ZSt == ZS_FOUND); }
    bool notSuccess(){ return (ZSt != ZS_SUCCESS)&&(ZSt != ZS_FOUND); }
};

const char * decode_ZSA_Action (ZSA_Action &ZS);

#endif // ZRESULT_H
