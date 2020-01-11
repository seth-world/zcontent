#ifndef ZSRFCOLLECTION_H
#define ZSRFCOLLECTION_H
#include <zrandomfile/zrandomfile.h>
#include <zrandomfile/zssearchargument.h>

namespace zbs {

/**
 * @brief The ZSRFCollection class contains in its base class (ZArray<ssize_t>)the resulting ranks set of a query operation
 *                  on an ZRandomFile or ZMasterFile thru various queries and specifically free search by field.
 *
 *  This objects holds and manages
 *  - the query resulting rows for relative ZRandomFile
 *  - the search rules
 *   + adhoc field definition
 *      . field offset from beginning of record (start 0) : may be obtained with offsetof(<struct>, <fieldname>),
 *      . field length in bytes : may be obtained with sizeof(<field datatype>);
 *   + sequence of bytes to search as a ZDataBuffer
 *   + query mask as a uint16_t with values corresponding to @ref ZRFSearch_type
 *   + a flag (boolean) mentionning if the rule result must be ORed(true) or ANDed with the agglomerated boolean result.
 *
 * @note there may be as much rules as required. But it is to be noticed that all rules are processed for any read record
 * and a huge amount of rules may impact on global search performance.
 *
 *
 *
 */
class ZSRFCollection : public ZArray<zrank_type>  // Ranks of the corresponding queried blocks/records
{
    typedef ZArray<zrank_type> _Base;
    friend class ZRandomFile;
public:


    ZSRFCollection(ZRandomFile* pZRFFile) {clear();  ZRFFile = pZRFFile;}
    ~ZSRFCollection() { if (InputCollection!=nullptr)
                                                delete InputCollection;
                          }  // And call _Base::ZArray destructor

    ZStatus initSearch(ZRandomFile* pZRFFile, const zlockmask_type pLock=ZLock_Omitted,ZSRFCollection *pInputCollection=nullptr);

//    ZStatus  testSequence(ZDataBuffer &pFieldContent);

    ZStatus evaluate(ZDataBuffer&wRecordContent) {return (Argument.evaluate(wRecordContent)?ZS_FOUND:ZS_NOTFOUND);}

    void reset(void);
    void clear(void);
    void copy(ZSRFCollection &pCollection);

    ZStatus addArgument(const ssize_t pFieldOffset,const ssize_t pFieldLength,ZDataBuffer &pSequence,const uint16_t pSearchType, bool pOR=false)
    {
        return Argument.add(pFieldOffset,pFieldLength,pSequence,pSearchType,pOR);
    }

    void reportArgument(FILE*pOutput=stdout) {Argument.report(pOutput);}
    void clearArgument(void) {Argument.clear(); return;}

    ZStatus getFirstRawRank(zrank_type &pZRFRank);
    ZStatus getNextRawRank(zrank_type &pZRFRank) ;
    ZStatus getPreviousRawRank(zrank_type &pZRFRank) ;
    ZStatus getCurrentRawRank (zrank_type& pZRFRank);

    ZStatus getFirstSelectedRank(ZDataBuffer &pRecordContent, zrank_type &pZRFRank, const zlockmask_type pLock=ZLock_Nolock);
    ZStatus getCurrentSelectedRank(zrank_type &pZRFRank);


    ZStatus getNextSelectedRank(ZDataBuffer &pRecordContent, zrank_type &pZRFRank);
    ZStatus getPreviousSelectedRank(ZDataBuffer &pRecordContent, zrank_type &pZRFRank, const zlockmask_type pLock=ZLock_Nolock);
    ZStatus getAllSelected(const zlockmask_type pLock=ZLock_Nolock);

    ZStatus _getRetry(ZDataBuffer&pRecordContent, zrank_type& pZRFRank);
//    ZStatus zgetNextRetry(ZDataBuffer &pRecordContent,zrank_type &pZRFRank,long pWaitMs,int16_t pTimes);

    bool isInit(void) {return FInitSearch;}

    ZStatus getStatus(void) {return ZSt;}
    ZStatus setStatus (ZStatus pSt) {ZSt=pSt; return ZSt;}

private:
    ZStatus ZSt ;                        //!< the status of the query operation
    ZSSearchArgument Argument;            //!< stores the evaluation conditions for the current collection

    zrank_type  ZRFRank=-1;              //!< last index accessed ; either ZRandomFile rank if no collection - or collection index if there is a collection */
    zrank_type  InputCollectionIndex=-1; //!< In case of there is already a collection : current collection index
    zrank_type  BaseCollectionIndex=-1;  //!< current base collection content index
    bool        FInitSearch = false;
    ZRandomFile *ZRFFile=nullptr;        //!< ZRF file to work with
    zlockmask_type  Lock=ZLock_Nolock ;      //!< lock mask used for setting lock on matched records (and only on matched records)

    ZSRFCollection *InputCollection=nullptr; //!< stores the collection as Input to work with for evaluating ranks
                                            //! If nullptr the whole set of ZRF ranks is taken see @ref ZSRFCollection::getNextRawRank
};// ZSRFCollection

} // namespace zbs



#endif // ZSRFCollection_H
