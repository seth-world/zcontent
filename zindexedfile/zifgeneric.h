 #ifndef ZIFGENERIC_H
#define ZIFGENERIC_H
/**
  *
  *  zsageneric.h
  *  Contains the generic c-like functions for managing ZSA keys, abstracting from templates.
  *
  *
  *
  *
  */


#include <ztoolset/zsacommon.h> // for types and getDataType


#include <ztoolset/zerror.h>
#include <ztoolset/zbasedatatypes.h>
#include <zindexedfile/zmasterfile.h>


namespace zbs //========================================================================
{
typedef ZStatus  (*addMethod) (size_t,void*);
typedef ZStatus  (*removeMethod) (size_t);

/*

char* _printDataValue (ZDataBuffer &pKey,char *pBuf)
{
    ZData_Type_struct wDT = _get_Data_Type<_KeyType>();
    if (wDT.isAtomic)
            {
            if (wDT.Type & Zchar)
                    {
                    sprintf(pBuf,"%c",pKey);
                    return (pBuf);
                    }
            if ((wDT.Type & Zfloat)==Zfloat)
                    {
                    sprintf (pBuf,"%f",pKey);
                    return (pBuf);
                    }
            if ((wDT.Type & Zlong)==Zlong)
                    {
                    sprintf(pBuf,"%ld",pKey);
                    return (pBuf);
                    }
            if ((wDT.Type & Zint)==Zint)
                    {
                    sprintf(pBuf,"%d",pKey);
                    return (pBuf);
                    }
            if ((wDT.Type & Zdouble)==Zdouble)
                    {
                    sprintf(pBuf,"%G",pKey);
                    return (pBuf);
                    }
            }  // IsAtomic


    if (wDT.isCString)
             {
            sprintf(pBuf,"%s",pKey);
            return (pBuf);
            }
    if (wDT.isPointer)
            {
            sprintf (pBuf,"%p",pKey);
            return (pBuf);
            }
    if (wDT.isArray )
                {
        if (wDT.Type&Zchar)
                sprintf(pBuf,"%s",pKey);
                else
                   {
                sprintf (pBuf,"<array -%s- >",wDT.TypeName);
                    }
                return (pBuf);
                }
    if (wDT.isCompound)
                {
                return ((char *)dumpSequence(&pKey,sizeof(_KeyType),pBuf));
                }

}// printDataValue



template <typename _KKeyType,
          typename _Bulk>

void  _printZArrayContext (long pIdx,ZArray<_Bulk > &pZTab,FILE *pOutput=stdout)
{
char wBuf[_KEY_BUFFER_MAXSIZE];
long wIdx=pIdx ;


    fprintf(pOutput, "_printZArrayContext prints for rank <%ld>\n",pIdx);
    if ((pIdx<0)||(pIdx>pZTab.size()))
            {
            fprintf (pOutput,"               ****Index <%ld> is Out of Boundaries (size is %ld)\n",pIdx,pZTab.size());
            return ;
            }
    if (pIdx>0)
        {
        wIdx=pIdx-1;
        fprintf (pOutput,"                previous  %3ld > %-20s \n", wIdx,_printDataValue<_KKeyType>(pZTab[wIdx]._Key,wBuf));
        }
        else
        {
        fprintf (pOutput,"                Noprevious index\n");
        }
        wIdx=pIdx;
        fprintf (pOutput,"              >>current   %3ld > %-20s \n", wIdx,_printDataValue<_KKeyType>(pZTab[wIdx]._Key,wBuf));

    if (pIdx>=(pZTab.lastIdx()))
            {
            fprintf (pOutput,"                No next index -reached boundary\n");
            return ;
            }
    wIdx ++ ;
    fprintf (pOutput,"                next      %3ld > %-20s \n", wIdx,_printDataValue<_KKeyType>(pZTab[wIdx]._Key,wBuf));
    return;

}//printContext

*/
/**
 * @brief ZKeyExtractCompareBinary Extracts from a record pRecord a key content value, formats it to key internal format and compares with a Key content pKey
 * @param pKey
 * @param pRecord
 * @param pZKDic
 * @return
 */
int
ZKeyExtractCompareBinary (ZDataBuffer &pKey,ZDataBuffer &pRecord,ZKeyDictionary *pZKDic)
{
    ZDataBuffer pKey2;
    pKey2=_keyValueExtraction(pZKDic,pRecord,pKey2);
    return (memcmp(pKey.Data,_keyValueExtraction(*pZKDic,pRecord,pKey2).Data,pKey.Size));
}
int
ZKeyExtractCompareAlpha (const ZDataBuffer &pKey,ZDataBuffer &pRecord,ZKeyDictionary *pZKDic)
{
    ZDataBuffer pKey2;
    return (strncmp(pKey.DataChar,_keyValueExtraction(*pZKDic,pRecord,pKey2).DataChar,pKey.Size));
}

typedef int  (*ZIFExtractCompare) (const ZDataBuffer &pKey,const ZDataBuffer &pKey2,ZKeyDictionary *pZKDic);

ZStatus _keyValueExtraction(ZKeyDictionary &pZKDic, ZDataBuffer &pContent, ZDataBuffer& pKey);


static inline
ZResult
_seekZIFGeneric(const ZDataBuffer &pKey, ZRandomFile &pZRF,ZIndexControlBlock* pZICB,ZIFCompare pZIFCompare)
{
ZDataBuffer wCurrent;

ZResult ZRes;
ZStatus wSt;
//_MODULEINIT_  ;
ZRes.ZIterations = 0;
ZRes.ZSt = ZS_NOTFOUND ;
ZRes.ZIdx = 0 ;

int wR;

//    long wi = 0 ;
    long whigh = 0;
    long wlow = 0;
    long wpivot = 0;

    if (pZRF.size()==0)
                    {
                    ZRes.ZSt= ZS_OUTBOUNDHIGH ;
                    return(ZRes);
                    }

        ZRes.ZIdx =pZRF.lastIdx() ;

//        wR= _cp((_KeyType *)&pKey,getKeyPointer(ZCurrentNb-1));
        if ((ZRes.ZSt=pZRF.zgetLast(wCurrent,0L))!=ZS_SUCCESS)
                                                    return ZRes;


        wR= pZIFCompare(pKey,wCurrent,pZICB);
        if (wR > 0)
//        if ( pKey > KeyValue(ZCurrentNb-1))
                        {

                        ZRes.ZSt=ZS_OUTBOUNDHIGH ;
                        return (ZRes) ;
                        }
        if (wR==0)
//            if (KeyValue(ZCurrentNb-1) == pKey)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        return (ZRes) ;
                        }

        ZRes.ZIdx = 0 ;
//        wR= memcmp(pKey,KeyReference(0) ,pSize);

        if ((ZRes.ZSt=pZRF.zget(wCurrent,0L,0L))!=ZS_SUCCESS)
                                                    return ZRes;

        wR= pZIFCompare(pKey,wCurrent,pZICB);
//        wR= _Compare::_compare(&pKey,&pZTab[0]._Key );
        if (wR < 0)
//            if (pKey < KeyValue(0) )
                        {
                        ZRes.ZSt=ZS_OUTBOUNDLOW ;
                        return (ZRes) ;
                        }

        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        return (ZRes) ;
                        }
        whigh = pZRF.lastIdx();  // last element : highest boundary
        wlow = 0;               // first element : lowest boundary

        if ((whigh-wlow)==1)    // if only 2 elements remaining : return whigh as last index and ZS_NOTFOUND
                {
                ZRes.ZIdx=ZS_NOTFOUND;
                ZRes.ZIdx=whigh ;
                return (ZRes);
                }

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)
        {
            ZRes.ZIdx = wlow ;

            if ((ZRes.ZSt=pZRF.zget(wCurrent,wlow,0L))!=ZS_SUCCESS)
                                                        return ZRes;
            wR= pZIFCompare(pKey,wCurrent,pZICB);

//            wR= _Compare::_compare(&pKey,&pZTab[wlow]._Key );
            if (wR==0)
//              if (KeyValue(wlow) == pKey)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = wlow ;
                        return (ZRes) ;
                        }
         ZRes.ZIdx = whigh ;

         if ((ZRes.ZSt=pZRF.zget(wCurrent,whigh,0L))!=ZS_SUCCESS)
                                                     return ZRes;
         wR= pZIFCompare(pKey,wCurrent,pZICB);

//         wR= _Compare::_compare(&pKey,&pZTab[whigh]._Key) ;
         if (wR==0)
//         if (KeyValue(whigh) == pKey)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = whigh ;
                        return (ZRes) ;
                        }
        ZRes.ZIdx = wpivot ;
        if ((ZRes.ZSt=pZRF.zget(wCurrent,wpivot,0L))!=ZS_SUCCESS)
                                                    return ZRes;
        wR= pZIFCompare(pKey,wCurrent,pZICB);

//        wR= _Compare::_compare(&pKey,&pZTab[wpivot]._Key) ;
        if (wR==0)
//        if (pKey==KeyValue(wpivot))
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        return (ZRes) ;
                        }

        if (wR>0)
//        if (pKey> KeyValue(wpivot) )
                {
                wlow = wpivot ;
                }
                else
                {
        if (wR<0)
//          if ( pKey < KeyValue(wpivot) )
                        {
                        whigh = wpivot ;
                        }
                } // else

        if ((whigh-wlow)==1)
                {
                ZRes.ZSt= ZS_NOTFOUND;
                return (ZRes) ;
                }


        wpivot = ((whigh-wlow)/2) + wlow ;
        ZRes.ZIdx = wpivot ;
        ZRes.ZIterations++;
        } // while (whigh-wlow)>2

        /*
         *
         *  Process the remaining collection
         */

    wpivot = wlow;

//    while ((wpivot<whigh)&&(pKey > KeyValue(wpivot)))
    if ((ZRes.ZSt=pZRF.zget(wCurrent,wpivot,0L))!=ZS_SUCCESS)
                                                return ZRes;
    wR= pZIFCompare(pKey,wCurrent,pZICB);

//        while ((wpivot<whigh)&&(_Compare::_compare(&pKey,&pZTab[wpivot]._Key) > 0))
    while ((wpivot<whigh)&& (wR > 0))
                        {
                        wpivot ++;
                        if ((ZRes.ZSt=pZRF.zget(wCurrent,wpivot,0L))!=ZS_SUCCESS)
                                                                    return ZRes;
                        wR= pZIFCompare(pKey,wCurrent,pZICB);
                         }
//    if (KeyValue(wpivot) == pKey)
//        if (_Compare::_compare(&pKey,&pZTab[wpivot]._Key) ==0)
            if (wR==0)
                {
                ZRes.ZIdx = wpivot ;
                ZRes.ZSt=ZS_FOUND ;
                }
               else
                {
                ZRes.ZIdx = wpivot ; // because only 2 slots between wlow and whigh remain and we should insert before whigh
                ZRes.ZSt=ZS_NOTFOUND ;
                }
    return (ZRes) ;

}// _seek template




/**
 * @brief _seekAllGeneric  this is the generic seek method for seeking ALL key values described by pKey (_Key field) and contained in _K0Bulk class
 * @param pKey key value to seek passed by reference
 * @return a ZMulti_Result class containing the indexes values if any. If not found, ZIdx index is positionned on last index seeked subject to error
 */
template <  typename _KeyType ,
            typename _Bulk,
            typename _Compare>

static inline ZStatus
_seekAllGeneric(_KeyType &pKey, ZArray<_Bulk> &pZTab,ZMulti_Result *ZRes)
{
_KeyType wKey ;

    if (ZRes==NULL)
            {
//            ZRes=new ZMulti_Result();
            fprintf(stderr,"_seekAllGeneric-F-NULLVALUE Fatal error : ZMulti_Result cannot be a null value pointer\n");
            _ABORT_ ;
            }
    size_t sK = sizeof(_KeyType);
    ZRes->ZIterations = 0;
    ZRes->ZSt = ZS_NOTFOUND ;
    ZRes->ZIdx = 0 ;
    ZRes->ZTabIdx->reset();

    int wR;

    long whigh ;
    long wlow ;
    long wpivot;

    if (pZTab.size()==0)

                    {
                    ZRes->ZSt= ZS_OUTBOUNDHIGH ;
//                    return(*ZRes);
                    return(ZRes->ZSt);
                    }

        ZRes->ZIdx =pZTab.lastIdx() ;
#if __DEBUG_LEVEL__ > 1
        memmove(&wKey,&pZTab.last()._Key,sizeof(_KeyType));
#endif
        wR= _Compare::_compare(&pKey,&pZTab.last()._Key);
        if (wR > 0)
                        {
                        ZRes->ZSt=ZS_OUTBOUNDHIGH ;
//                        return (*ZRes) ;
                        return(ZRes->ZSt);
                        }
        if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        wpivot=pZTab.lastIdx();
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes->ZTabIdx->push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes->ZSt);
                        }

        ZRes->ZIdx = 0 ;
        memmove(&wKey,&pZTab[0]._Key,sizeof(_KeyType));
        wR= _Compare::_compare(&pKey,&pZTab[0]._Key );
        if (wR < 0)
                        {
                        ZRes->ZSt=ZS_OUTBOUNDLOW ;
//                        return (*ZRes) ;
                        return(ZRes->ZSt);
                        }

        if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        wpivot=0;
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes->ZTabIdx->push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes->ZSt);
                        }
        whigh = pZTab.lastIdx();  // last element : highest boundary
        wlow = 0;               // first element : lowest boundary

        if ((whigh-wlow)==1)    // if only 2 elements remaining : return whigh as last index and ZS_NOTFOUND
                {
                ZRes->ZIdx=ZS_NOTFOUND;
                ZRes->ZIdx=whigh ;
                return(ZRes->ZSt);
                }

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)
        {
            ZRes->ZIdx = wlow ;
            memmove(&wKey,&pZTab[wlow]._Key,sizeof(_KeyType));

            wR= _Compare::_compare(&pKey,&pZTab[wlow]._Key );
            if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        ZRes->ZIdx = wlow ;
                        wpivot=wlow;
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes->ZTabIdx->push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes->ZSt);
                        }
         ZRes->ZIdx = whigh ;
#if __DEBUG_LEVEL__ > 1
         memmove(&wKey,&pZTab[whigh]._Key,sizeof(_KeyType));
#endif
         wR= _Compare::_compare(&pKey,&pZTab[whigh]._Key) ;
         if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        ZRes->ZIdx = whigh ;
                        wpivot=whigh;
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes->ZTabIdx->push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes->ZSt);
                        }

        ZRes->ZIdx = wpivot ;
#if __DEBUG_LEVEL__ > 1
         memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
#endif
        wR= _Compare::_compare(&pKey,&pZTab[wpivot]._Key) ;

        if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes->ZTabIdx->push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes->ZSt);
                        }

        if (wR>0)
                {
                wlow = wpivot ;
                }
                else
                {
        if (wR<0)
                        {
                        whigh = wpivot ;
                        }
                } // else

        if ((whigh-wlow)==1)
                {
                ZRes->ZSt= ZS_NOTFOUND;
//                        return (*ZRes) ;
                return(ZRes->ZSt);
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        ZRes->ZIdx = wpivot ;
        ZRes->ZIterations++;
        } // while (whigh-wlow)>2

        /*
         *
         *  Process the remaining collection
         */

        wpivot = wlow;
        ZRes->ZSt=ZS_NOTFOUND ;
#if __DEBUG_LEVEL__ > 1
        memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
#endif
        while ((wpivot<whigh)&&((wR=_Compare::_compare(&pKey,&pZTab[wpivot]._Key)) > 0))
                        {
                        wpivot ++;
#if __DEBUG_LEVEL__ > 1
                        memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
#endif
                        } // while wpivot
#if __DEBUG_LEVEL__ > 1
        memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
#endif
        while ((wR=_Compare::_compare(&pKey,&pZTab[wpivot]._Key)) ==0)
                        {
                        ZRes->ZTabIdx->push((size_t&)wpivot);
                        ZRes->ZSt=ZS_FOUND ;
                        wpivot ++;
                        memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
                        }
    return(ZRes->ZSt);
}// _seekAllGeneric template

/**
 * @brief _seekAllPartialGeneric  this is the generic seek method for seeking ALL key values on a PARTIAL value described by pKey (_Key field) over a size (pSize and contained in _K0Bulk class
 * @param pKey key value to seek passed by reference
 * @return a ZMulti_Result class containing the indexes values if any. If not found, ZIdx index is positionned on last index seeked subject to error
 */
template <  typename _KeyType ,
            typename _Bulk,
            typename _Compare>

static inline ZStatus
_seekAllPartialGeneric(_KeyType &pKey, size_t pSize,ZArray<_Bulk> &pZTab, ZMulti_Result *ZRes)
{


    if (ZRes==NULL)
            {
//            ZRes=new ZMulti_Result();
            fprintf(stderr,"_seekAllGeneric-F-NULLVALUE Fatal error : ZMulti_Result cannot be a null value pointer\n");
            _ABORT_ ;
            }

    ZRes->ZIterations = 0;
    ZRes->ZSt = ZS_NOTFOUND ;
    ZRes->ZIdx = 0 ;
    ZRes->ZTabIdx->reset();

    int wR;

    long whigh ;
    long wlow ;
    long wpivot;

    if (pZTab.size()==0)

                    {
                    ZRes->ZSt= ZS_OUTBOUNDHIGH ;
                    return(ZRes->ZSt);
                    }

        ZRes->ZIdx =pZTab.lastIdx() ;
        whigh=pZTab.lastIdx() ;
        wlow = 0;
        wpivot=whigh;

        wR= _Compare::_comparePartial(&pKey,&pZTab.last()._Key,pSize);
        if (wR > 0)
                        {
                        ZRes->ZSt=ZS_OUTBOUNDHIGH ;
                        return(ZRes->ZSt);
                        }
        if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot<pZTab.size()))
                                {
                        ZRes->ZTabIdx->push(wpivot);
                                wpivot++;
                                }
                        return(ZRes->ZSt);

                        }

        ZRes->ZIdx = 0 ;
        wpivot=0;
        wR= _Compare::_comparePartial(&pKey,&pZTab[0]._Key ,pSize);
        if (wR < 0)
                        {
                        ZRes->ZSt=ZS_OUTBOUNDLOW ;
                        return(ZRes->ZSt);
                        }

        if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        ZRes->ZIdx = wpivot ;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot<pZTab.size()))
                                {
                        ZRes->ZTabIdx->push(wpivot);
                                wpivot++;
                                }
                        return(ZRes->ZSt);
                        }
        whigh = pZTab.lastIdx();  // last element : highest boundary
        wlow = 0;               // first element : lowest boundary

        if ((whigh-wlow)==1)    // if only 2 elements remaining : return whigh as last index and ZS_NOTFOUND
                {
                ZRes->ZIdx=ZS_NOTFOUND;
                ZRes->ZIdx=whigh ;
                 return(ZRes->ZSt);
                }

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)
        {
            ZRes->ZIdx = wlow ;
            wR= _Compare::_comparePartial(&pKey,&pZTab[wlow]._Key,pSize );
            if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        ZRes->ZIdx = wlow ;
                        wpivot=wlow ;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                       while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot<pZTab.size()))
                               {
                       ZRes->ZTabIdx->push(wpivot);
                               wpivot++;
                               }
                        }
         ZRes->ZIdx = whigh ;
         wR= _Compare::_comparePartial(&pKey,&pZTab[whigh]._Key,pSize) ;
         if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        ZRes->ZIdx = whigh ;
                        wpivot=whigh;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while (_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0&&(wpivot<pZTab.size()))
                                {
                        ZRes->ZTabIdx->push(wpivot);
                                wpivot++;
                                }
                        return(ZRes->ZSt);
                        }
        ZRes->ZIdx = wpivot ;

        wR= _Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) ;
        if (wR==0)
                        {
                        ZRes->ZSt=ZS_FOUND ;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while (_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0&&(wpivot<pZTab.size()))
                                {
                        ZRes->ZTabIdx->push((size_t &)wpivot);
                                wpivot++;
                                }

                        return(ZRes->ZSt);
                        }

        if (wR>0)
                {
                wlow = wpivot ;
                }
                else
                {
        if (wR<0)
                        {
                        whigh = wpivot ;
                        }
                } // else

        if ((whigh-wlow)==1)
                {
                ZRes->ZSt= ZS_NOTFOUND;
                return(ZRes->ZSt);
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        ZRes->ZIdx = wpivot ;
        ZRes->ZIterations++;
        } // while (whigh-wlow)>2

        /*
         *
         *  Process the remaining collection
         */

        wpivot = wlow;
        ZRes->ZSt=ZS_NOTFOUND ;
        while ((wpivot<whigh)&&(_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) > 0))
                        wpivot ++;

        while (_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) ==0)
                        {
                        ZRes->ZTabIdx->push((size_t&)wpivot);
                        ZRes->ZSt=ZS_FOUND ;
                        wpivot ++;
                        }
    return (ZRes->ZSt);
}// _seekAllPartialGeneric template
//!
//!-----------seek all and seek All partial with new ZAMulti_Result
//!
/**
 * @brief _seekAllGeneric  this is the generic seek method for seeking ALL key values described by pKey (_Key field) and contained in _K0Bulk class
 * @param pKey key value to seek passed by reference
 * @return a ZMulti_Result class containing the indexes values if any. If not found, ZIdx index is positionned on last index seeked subject to error
 */
template <  typename _KeyType ,
            typename _Bulk,
            typename _Compare>

static inline ZStatus
_seekAllGenericZA(_KeyType &pKey, ZArray<_Bulk> &pZTab,ZAMulti_Result &ZRes)
{
_KeyType wKey ;

    ZRes.clear();
    ZRes.ZSt = ZS_NOTFOUND ;

    int wR;

    long whigh ;
    long wlow ;
    long wpivot;

    if (pZTab.size()==0)

                    {
                    ZRes.ZSt= ZS_OUTBOUNDHIGH ;
                    return(ZRes.ZSt);
                    }

        ZRes.ZIdx =pZTab.lastIdx() ;
#if __DEBUG_LEVEL__ > 1
        memmove(&wKey,&pZTab.last()._Key,sizeof(_KeyType));
#endif
        wR= _Compare::_compare(&pKey,&pZTab.last()._Key);
        if (wR > 0)
                        {
                        ZRes.ZSt=ZS_OUTBOUNDHIGH ;
                        return(ZRes.ZSt);
                        }
        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        wpivot=pZTab.lastIdx();
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes.push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes.ZSt);
                        }

        ZRes.ZIdx = 0 ;
        memmove(&wKey,&pZTab[0]._Key,sizeof(_KeyType));
        wR= _Compare::_compare(&pKey,&pZTab[0]._Key );
        if (wR < 0)
                        {
                        ZRes.ZSt=ZS_OUTBOUNDLOW ;
                        return(ZRes.ZSt);
                        }

        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        wpivot=0;
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes.push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes.ZSt);
                        }
        whigh = pZTab.lastIdx();  // last element : highest boundary
        wlow = 0;               // first element : lowest boundary

        if ((whigh-wlow)==1)    // if only 2 elements remaining : return whigh as last index and ZS_NOTFOUND
                {
                ZRes.ZIdx=ZS_NOTFOUND;
                ZRes.ZIdx=whigh ;
                return(ZRes.ZSt);
                }

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)
        {
            ZRes.ZIdx = wlow ;
            memmove(&wKey,&pZTab[wlow]._Key,sizeof(_KeyType));

            wR= _Compare::_compare(&pKey,&pZTab[wlow]._Key );
            if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = wlow ;
                        wpivot=wlow;
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes.push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes.ZSt);
                        }
         ZRes.ZIdx = whigh ;
#if __DEBUG_LEVEL__ > 1
         memmove(&wKey,&pZTab[whigh]._Key,sizeof(_KeyType));
#endif
         wR= _Compare::_compare(&pKey,&pZTab[whigh]._Key) ;
         if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = whigh ;
                        wpivot=whigh;
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes.push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes.ZSt);
                        }

        ZRes.ZIdx = wpivot ;
#if __DEBUG_LEVEL__ > 1
         memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
#endif
        wR= _Compare::_compare(&pKey,&pZTab[wpivot]._Key) ;

        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_compare(&pKey,&pZTab[wpivot]._Key)==0)&&(wpivot<pZTab.size()))  // take them all
                                {
                        ZRes.push((size_t &)wpivot);
                                wpivot++;
                                }
                        return(ZRes.ZSt);
                        }

        if (wR>0)
                {
                wlow = wpivot ;
                }
                else
                {
        if (wR<0)
                        {
                        whigh = wpivot ;
                        }
                } // else

        if ((whigh-wlow)==1)
                {
                ZRes.ZSt= ZS_NOTFOUND;
//                        return (*ZRes) ;
                return(ZRes.ZSt);
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        ZRes.ZIdx = wpivot ;
        ZRes.ZIterations++;
        } // while (whigh-wlow)>2

        /*
         *
         *  Process the remaining collection
         */

        wpivot = wlow;
        ZRes.ZSt=ZS_NOTFOUND ;
#if __DEBUG_LEVEL__ > 1
        memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
#endif
        while ((wpivot<whigh)&&((wR=_Compare::_compare(&pKey,&pZTab[wpivot]._Key)) > 0))
                        {
                        wpivot ++;
#if __DEBUG_LEVEL__ > 1
                        memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
#endif
                        } // while wpivot
#if __DEBUG_LEVEL__ > 1
        memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
#endif
        while ((wR=_Compare::_compare(&pKey,&pZTab[wpivot]._Key)) ==0)
                        {
                        ZRes.push((size_t&)wpivot);
                        ZRes.ZSt=ZS_FOUND ;
                        wpivot ++;
                        memmove(&wKey,&pZTab[wpivot]._Key,sizeof(_KeyType));
                        }
    return(ZRes.ZSt);
}// _seekAllGeneric template

/**
 * @brief _seekAllPartialGeneric  this is the generic seek method for seeking ALL key values on a PARTIAL value described by pKey (_Key field) over a size (pSize and contained in _K0Bulk class
 * @param pKey key value to seek passed by reference
 * @return a ZMulti_Result class containing the indexes values if any. If not found, ZIdx index is positionned on last index seeked subject to error
 */
template <  typename _KeyType ,
            typename _Bulk,
            typename _Compare>

static inline ZStatus
_seekAllPartialGenericZA(_KeyType &pKey, size_t pSize,ZArray<_Bulk> &pZTab, ZAMulti_Result &ZRes)
{
    ZRes.ZIterations = 0;
    ZRes.ZSt = ZS_NOTFOUND ;
    ZRes.ZIdx = 0 ;
    ZRes.reset();

    int wR;

    long whigh ;
    long wlow ;
    long wpivot;

    if (pZTab.size()==0)

                    {
                    ZRes.ZSt= ZS_OUTBOUNDHIGH ;
                    return(ZRes.ZSt);
                    }

        ZRes.ZIdx =pZTab.lastIdx() ;
        whigh=pZTab.lastIdx() ;
        wlow = 0;
        wpivot=whigh;

        wR= _Compare::_comparePartial(&pKey,&pZTab.last()._Key,pSize);
        if (wR > 0)
                        {
                        ZRes.ZSt=ZS_OUTBOUNDHIGH ;
                        return(ZRes.ZSt);
                        }
        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot<pZTab.size()))
                                {
                        ZRes.push(wpivot);
                                wpivot++;
                                }
                        return(ZRes.ZSt);

                        }

        ZRes.ZIdx = 0 ;
        wpivot=0;
        wR= _Compare::_comparePartial(&pKey,&pZTab[0]._Key ,pSize);
        if (wR < 0)
                        {
                        ZRes.ZSt=ZS_OUTBOUNDLOW ;
                        return(ZRes.ZSt);
                        }

        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = wpivot ;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot<pZTab.size()))
                                {
                        ZRes.push(wpivot);
                                wpivot++;
                                }
                        return(ZRes.ZSt);
                        }
        whigh = pZTab.lastIdx();  // last element : highest boundary
        wlow = 0;               // first element : lowest boundary

        if ((whigh-wlow)==1)    // if only 2 elements remaining : return whigh as last index and ZS_NOTFOUND
                {
                ZRes.ZIdx=ZS_NOTFOUND;
                ZRes.ZIdx=whigh ;
                 return(ZRes.ZSt);
                }

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)
        {
            ZRes.ZIdx = wlow ;
            wR= _Compare::_comparePartial(&pKey,&pZTab[wlow]._Key,pSize );
            if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = wlow ;
                        wpivot=wlow ;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                       while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot<pZTab.size()))
                               {
                       ZRes.push(wpivot);
                               wpivot++;
                               }
                        }
         ZRes.ZIdx = whigh ;
         wR= _Compare::_comparePartial(&pKey,&pZTab[whigh]._Key,pSize) ;
         if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = whigh ;
                        wpivot=whigh;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while (_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0&&(wpivot<pZTab.size()))
                                {
                        ZRes.push(wpivot);
                                wpivot++;
                                }
                        return(ZRes.ZSt);
                        }
        ZRes.ZIdx = wpivot ;

        wR= _Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) ;
        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        while ((_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0)&&(wpivot>=0))
                                                                wpivot--;  // find the first one among possible duplicates
                        wpivot++; // just to the begining of the collection
                        while (_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize)==0&&(wpivot<pZTab.size()))
                                {
                        ZRes.push((size_t &)wpivot);
                                wpivot++;
                                }

                        return(ZRes.ZSt);
                        }

        if (wR>0)
                {
                wlow = wpivot ;
                }
                else
                {
        if (wR<0)
                        {
                        whigh = wpivot ;
                        }
                } // else

        if ((whigh-wlow)==1)
                {
                ZRes.ZSt= ZS_NOTFOUND;
                return(ZRes.ZSt);
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        ZRes.ZIdx = wpivot ;
        ZRes.ZIterations++;
        } // while (whigh-wlow)>2

        /*
         *
         *  Process the remaining collection
         */

        wpivot = wlow;
        ZRes.ZSt=ZS_NOTFOUND ;
        while ((wpivot<whigh)&&(_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) > 0))
                        wpivot ++;

        while (_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) ==0)
                        {
                        ZRes.push((size_t&)wpivot);
                        ZRes.ZSt=ZS_FOUND ;
                        wpivot ++;
                        }
    return (ZRes.ZSt);
}// _seekAllPartialGeneric template


/**
 * @brief _seekAllPartialGeneric    This is the generic seek method for seeking the FIRST found key value on a PARTIAL value described by pKey (_Key field)
 *                                  over a size (pSize and contained in _K0Bulk class
 * @param pKey key value to seek passed by reference
 * @return a ZMulti_Result class containing the indexes values if any. If not found, ZIdx index is positionned on last index seeked subject to error
 */
template <  typename _KeyType ,
            typename _Bulk,
            typename _Compare>

static inline ZResult
_seekFirstPartialGeneric(_KeyType &pKey, size_t pSize,ZArray<_Bulk> &pZTab)
{
ZResult ZRes;

    ZRes.ZIterations = 0;
    ZRes.ZSt = ZS_NOTFOUND ;
    ZRes.ZIdx = 0 ;

    int wR;

    long whigh ;
    long wlow ;
    long wpivot;

    if (pZTab.size()==0)

                    {
                    ZRes.ZSt= ZS_OUTBOUNDHIGH ;
                    return(ZRes);
                    }

        ZRes.ZIdx =pZTab.lastIdx() ;
        whigh=pZTab.lastIdx() ;
        wlow = 0;
        wpivot=whigh;

        wR= _Compare::_comparePartial(&pKey,&pZTab.last()._Key,pSize);
        if (wR > 0)
                        {
                        ZRes.ZSt=ZS_OUTBOUNDHIGH ;
                        return(ZRes);
                        }
        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        return (ZRes);
                        }

        ZRes.ZIdx = 0 ;
        wpivot=0;
        wR= _Compare::_comparePartial(&pKey,&pZTab[0]._Key ,pSize);
        if (wR < 0)
                        {
                        ZRes.ZSt=ZS_OUTBOUNDLOW ;
                        return(ZRes);
                        }

        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = wpivot ;

                        return(ZRes);
                        }
        whigh = pZTab.lastIdx();  // last element : highest boundary
        wlow = 0;               // first element : lowest boundary

        if ((whigh-wlow)==1)    // if only 2 elements remaining : return whigh as last index and ZS_NOTFOUND
                {
                ZRes.ZIdx=ZS_NOTFOUND;
                ZRes.ZIdx=whigh ;
                 return(ZRes);
                }

        wpivot = ((whigh-wlow)/2) +wlow ;

        while ((whigh-wlow)>2)
        {
            ZRes.ZIdx = wlow ;
            wR= _Compare::_comparePartial(&pKey,&pZTab[wlow]._Key,pSize );
            if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = wlow ;
                        return(ZRes);
                        }
         ZRes.ZIdx = whigh ;
         wR= _Compare::_comparePartial(&pKey,&pZTab[whigh]._Key,pSize) ;
         if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx = whigh ;
                        return(ZRes);
                        }
        ZRes.ZIdx = wpivot ;

        wR= _Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) ;
        if (wR==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        return(ZRes);
                        }

        if (wR>0)
                {
                wlow = wpivot ;
                }
                else
                {
        if (wR<0)
                        {
                        whigh = wpivot ;
                        }
                } // else

        if ((whigh-wlow)==1)
                {
                ZRes.ZSt= ZS_NOTFOUND;
                return(ZRes);
                }

        wpivot = ((whigh-wlow)/2) + wlow ;
        ZRes.ZIdx = wpivot ;
        ZRes.ZIterations++;
        } // while (whigh-wlow)>2

        /*
         *
         *  Process the remaining collection
         */

        wpivot = wlow;
        ZRes.ZSt=ZS_NOTFOUND ;
        while ((wpivot<whigh)&&(_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) > 0))
                        wpivot ++;

        if (_Compare::_comparePartial(&pKey,&pZTab[wpivot]._Key,pSize) ==0)
                        {
                        ZRes.ZSt=ZS_FOUND ;
                        ZRes.ZIdx=wpivot ;
                        }
    return (ZRes);
}// _seekFirstPartialGeneric template

#ifdef __FORMER__


ZResult _addGeneric(ZDataBuffer &pElement,ZRandomFile &pZTab,ZSort_Type pZST=ZST_NODUPLICATES)
    {

    ZResult ZRes;

    #if __GENERIC_DEBUG_LEVEL__ > 0
    char wBuf    [60];
    #endif
        _Type rElement=pElement;
        ZRes=_seekZIFGeneric(rElement._Key,pZTab);

    #if __GENERIC_DEBUG_LEVEL__ > 1
            printf ("_addGeneric seek:  Key <%-20s>  status <%-15s> iterations <%5d> ZIdx : <%5ld>\n",
                     _printDataValue<_KeyType>(pElement._Key,wBuf), decode_ZStatus(ZRes.ZSt),ZRes.ZIterations,ZRes.ZIdx);
    #endif
        if (ZRes.ZSt==ZS_FOUND)
                        {

            if (pZST==ZST_NODUPLICATES)
                                {
            #if __GENERIC_DEBUG_LEVEL__ > 1
                        printf("_addGeneric        ****ZST NO DUPLICATE Key violation : ZS_DUPLICATEKEY***** : Key is <%s>\n",_printDataValue(pElement,wBuf));
            #endif
                        ZRes.ZSt=(ZS_DUPLICATEKEY);
                        ZRes.ZA = ZSA_Error;
                        return (ZRes);
                                }
                        }
        if (ZRes.ZSt==ZS_OUTBOUNDHIGH)
                    {
                    pZTab.push((_Type &)pElement);
    #if __GENERIC_DEBUG_LEVEL__ > 2
                    printf ("_addGeneric        ***PUSH**** key is <%s>\n",_printDataValue<_KeyType>(pElement._Key,wBuf));
                    printf ("_addGeneric        size() is <%5ld> ----------------\n", pZTab.size());
    #endif
                    ZRes.ZSt=ZS_SUCCESS;
                    ZRes.ZA = ZSA_Push;
                    ZRes.ZIdx=pZTab.lastIdx();
                    return(ZRes);
                    }
        if (ZRes.ZSt==ZS_OUTBOUNDLOW)
                    {
            #if __GENERIC_DEBUG_LEVEL__ > 1
                        printf("_addGeneric        ****ZS_OUTOFBOUNDLOW***** : Key is <%s>\n",_printDataValue<_KeyType>(pElement._Key,wBuf));
            #endif
                    pZTab.push_front((_Type&)pElement);
    #if __GENERIC_DEBUG_LEVEL__ > 2

                    printf ("_addGeneric        ***PUSH_FRONT**** key is <%s>\n",_printDataValue<_KeyType>(pElement._Key,wBuf));
                    printf ("_addGeneric        size() is <%5ld> ----------------\n", pZTab.size());

    #endif
                    ZRes.ZSt=(ZS_SUCCESS);
                    ZRes.ZA = ZSA_Push_Front;
                    ZRes.ZIdx=0;
                    return (ZRes);
                    }
        pZTab.insert((_Type &)pElement,ZRes.ZIdx);
    #if __GENERIC_DEBUG_LEVEL__ > 2
        printf("_addGeneric         ***INSERT*** key is <%s> at position <%ld>\n", _printDataValue<_KeyType>(pElement._Key,wBuf),ZRes.ZIdx);
        printf ("_addGeneric        Array size <%5ld> ----------------\n", pZTab.size());

        printf ("_addGeneric         ---------------------------------------------\n");
        std::cout.flush();
    #endif
        ZRes.ZSt=(ZS_SUCCESS);
        ZRes.ZA = ZSA_Insert;
        return (ZRes);
} // _addGeneric









template <typename _Base ,          // ZSA type
          typename _KDesc,          // ZKeyDescriptor type
          typename _KType,        // main Key type or class
          typename _KBulk,           // _K0Bulk of the main ZSA access with _Key positionned as main key
          typename _KCompare>        // _Compare class of the main ZSA

ZResult _updateAddSecondaryGeneric(_KBulk &pElement,size_t pZSAIdx,_Base* pBase,_KDesc* pKDesc)
{
char wBuf[50];
long wR ;

ZResult wRes;
ZResult wRes1;


size_t ZSAIdx ;
size_t  ZKey1Idx;

Index_struct<_KType> wIndex;
//printf ("==================_addSecondaryGeneric==================\n");

_KBulk* wK1 ;
wK1= (_KBulk*)&pElement ;
memmove(&wIndex._Key,&wK1->_Key,sizeof(_KType));

wIndex.Idx=wRes.ZIdx ; // index within the main ZSA
ZSAIdx=wRes.ZIdx;

ZKey1Idx =(long) _HighValue;



/**
*                         MAINTAIN ACTIONS ACCORDING THE CASES
*
*     Action      Action                  index        index
*     ZSA         ZKey1                    ZSA          ZKey1       ZReverse    (is an index)            ZKey1.Idx
*  and ZReverse
*
*     push        push                    last         last       --                                  --
*                 push_front              last         0          +1 ZReverse[0-size]                 --
*                 insert                  last         i          +1 ZReverse[ZKey1[i-size].Idx]       --
*
*     push_front  push                    0           last        --                                  +1 ZKey1[0-size]
*                 push_front              0           0           +1 ZReverse[0-size]                 +1 ZKey1[0-size]
*                 insert                  0           i           +1 ZReverse[ZKey1[i-size].Idx]       +1 ZKey1[0-size]
*
*     insert      push                    j           last        --                                  +1 ZKey1[ZReverse[j-size]]
*                 push_front              j           0           +1 ZReverse[0-size                  +1 ZKey1[ZReverse[j-size]]
*                 insert                  j           i           +1 ZReverse[ZKey1[i-size].Idx]       +1 ZKey1[ZReverse[j-size]]
*
*
*/


switch (wRes.ZA)
        {
        case ZSA_Push :  // no impact on ZSA index : only last ZSA index added
            {
            fprintf(stdout,"_updateAddSecondaryGeneric:      ZSA Push index<%4ld> key value <%s>\n",wIndex.Idx,_printDataValue<_KType> (wIndex._Key,wBuf) );

            ZKey1Idx = wRes1.ZIdx ;
            pKDesc->ZReverse.push (wRes1.ZIdx) ;

            printf ("   ZReverse.push ***\n");
            break;

            } // ZSA_Push on ZSA index

        case ZSA_Push_Front :
            {
                for (long wi=0;wi<pKDesc->size();wi++)
                                       {
//                                    if (wi!=ZKey1Idx)
                                            pKDesc->Tab[wi].Idx++;
                                       }
                pKDesc->ZReverse.push_front (ZKey1Idx);
                printf ("   ZReverse.push_front ***\n");
                break;
             }//ZSA_Push_Front on ZSA index

        case ZSA_Insert :
                {
                 for (long wi=ZSAIdx;wi<pKDesc->ZReverse.size();wi++)
                             {
                            if (pKDesc->ZReverse[wi]==(long)_HighValue)
                                                continue;
                           wR=pKDesc->ZReverse[wi];
                          pKDesc->Tab[pKDesc->ZReverse[wi]].Idx++;
                             }

/*                      for (long wi=0;wi<ZKey1.size();wi++)
                              {
//                                if ((ZKey1[wi].Idx>=ZSAIdx)&&(wi!=ZKey1Idx))
                              if (ZKey1[wi].Idx>=ZSAIdx)
                                                ZKey1[wi].Idx++;
                              }*/

                 printf ("   ZReverse.insert ***\n");
                 pKDesc->ZReverse.insert (ZKey1Idx,ZSAIdx,1);
                break;
                }//ZSA_Insert on ZSA index
        default :
            {
            wRes.ZSt = ZS_INVOP ;
            return (wRes);
            }
        } ;  // switch wRes for ZSA status

wRes1=_addGeneric<Index_struct<_KType>,_KType,Index_struct<_KType>,_KCompare> (wIndex,(*pKDesc),pKDesc->ZST);


if (wRes1.ZSt!=ZS_SUCCESS)
            {
            pBase->erase(wRes.ZIdx,1) ;  // secondary key is errored so roll-back ZSA

//            wRes1.ZSt = (ZStatus)((int) wRes1.ZSt |(int) ZS_SECONDARY) ;
            return (wRes1);
            }
ZKey1Idx=wRes1.ZIdx ;

switch (wRes1.ZA) // maintain ZReverse
        {
        case ZSA_Push : // no impact on ZKey1 index : only last modified
                {
                break;
                }
        case ZSA_Push_Front : // all ZReverse index references to ZKey1 are affected
                {
                for (long wi=0;wi<pKDesc->ZReverse.size();wi ++)
                                                pKDesc->ZReverse[wi]++;
                break;
                }
        case ZSA_Insert : // For ZReverse : all ZKey1 indexes are affected since ZRes1.Idx+1 till end of ZArray
                {
                for (long wi=ZKey1Idx+1;wi<pKDesc->size();wi ++)
                                                {
                                                if (pKDesc->ZReverse[wi]==(long)_HighValue)
                                                                        continue;
                                                pKDesc->ZReverse[pKDesc->Tab[wi].Idx]++;
                                                }
                  break;

                }
                default:
                     {
                        wRes.ZSt = ZS_INVOP ;
                        return (wRes);
                        break;
                     }
        }//switch wRes1.ZA

pKDesc->ZReverse[ZSAIdx]=ZKey1Idx ;

fprintf(stdout," _updateAddSecondaryGeneric:             _Base::_add Key <%15s> ZResult <%10s> <%10s> ZSA index <%4ld> \n "
               "                                          ZKey1---->   ZResult <%10s> <%10s> ZKey1Index <%4ld> \n",
        _printDataValue<_KType>(wIndex._Key,wBuf),
        decode_ZSA_Action(wRes.ZA),
        decode_ZStatus( wRes.ZSt),
        wRes.ZIdx,
        decode_ZSA_Action(wRes1.ZA),
        decode_ZStatus( wRes1.ZSt),
        wRes1.ZIdx            );
        printf ("...ZSA printAllKeys....\n");
        pBase->printAllKeys();
        printf ("...ZKey1 printAllKeys....\n");
        for (long wi=0;wi<pKDesc->size();wi++)
                printf ("%d >> %s\n",wi,_printDataValue<_KType>(pKDesc->Tab[wi]._Key,wBuf));





printf ("---ZKey1---\n");
for (long wi=0;wi<pKDesc->size();wi++)
            printf ("%3ld >> _Key <%s> Idx <%ld>\n",wi,_printDataValue<_KType>( pKDesc->Tab[wi]._Key,wBuf), pKDesc->Tab[wi].Idx);
printf ("---ZReverse---\n");
for (long wi=0;wi<pKDesc->ZReverse.size();wi++)
            printf ("%3ld >> Idx <%ld>\n",wi, pKDesc->ZReverse[wi]);
return (wRes);
return (wRes) ;
} // _updateAddSecondaryGeneric




template <typename _Base ,          // ZSA from which we build the index
          typename _KDesc,          // KeyDescriptor for the index to be built
          typename _KType,          // Key type or class
          typename _KBulk,          // _KBulk of the main ZSA access with _Key positionned as main key
          typename _KCompare>       // _Compare class of the key to build

ZStatus _buildSecondaryGeneric(_Base* pBase,_KDesc* pDesc,ZSort_Type pZST=ZST_DUPLICATES)
{
    size_t wi ;
    ZResult wRes;
    wRes.ZSt=ZS_SUCCESS;
    while ((wi<pBase->size())&&(wRes.ZSt==ZS_SUCCESS))
            {
                 wRes=_updateAddSecondaryGeneric<_Base,_KDesc,_KType,_KBulk,_KCompare>(pBase->Tab[wi],wi,pBase,pDesc);
                 wi ++;
            }
    return(wRes.ZSt);
} //_buildSecondaryGeneric


#endif // __FORMER__


}// namespace zbs



#endif // ZIFGENERIC_H

