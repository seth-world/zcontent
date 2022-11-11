#ifndef ZINDEXCOLLECTION_H
#define ZINDEXCOLLECTION_H

//#include <zrandomfile/zrfcollection.h>
#include <zrandomfile/zrandomfiletypes.h>
#include <zrandomfile/zsearchargument.h>

#include <ztoolset/zarray.h>

//


namespace zbs {

class ZRawMasterFile;
class ZRawIndexFile;

int ZKeyCompareBinary (const ZDataBuffer &pKey1, ZDataBuffer &pKey2, ssize_t pSize);

int ZKeyCompareAlpha (const ZDataBuffer &pKey1,ZDataBuffer &pKey2,ssize_t pSize);

typedef int  (*ZIFCompare) (const ZDataBuffer &pKey1,ZDataBuffer &pKey2,ssize_t pSize);

//class ZRawIndexFile;

//class ZIndexCollection;

/**
 * @brief The ZCollection_Op_type enum describes the last operation done on Collection
 */
#ifndef ZCOLLECTION_OP
#define ZCOLLECTION_OP
enum ZCollection_Op_type : uint16_t {
    ZCOP_Nothing     =   0,
//    ZCOP_Get         =   0x0001,    //!< simple search operation for KeyContent (not used as is: masked with other op codes)
    ZCOP_Interval    =   0x0001,    //!< interval search operation  [KeyContent;KeyHigh] ( not used as is : masked with other op codes)
    ZCOP_Exclude     =   0x0002,    /**< during an interval search : exclude the given key values (high and low) from the collection :
                                    by default interval key values low and high are considered to be selected.*/
    ZCOP_GetFirst    =   0x0010,      //!< initialize collection, gets the first selected ZMasterFile rank in key order and enrich the collection
    ZCOP_GetNext     =   0x0020,      //!< gets the next current selected ZMasterFile rank in key order and enrich the collection
    ZCOP_GetAll      =   0x0040,      //!< gets all selected ZMasterFile ranks in key order and delivers the collection
    ZCOP_GetAllFirst =   ZCOP_GetFirst | ZCOP_GetAll,   //!< a getFirst have been invoked in a getAll context
    ZCOP_GetAllNext  =   ZCOP_GetNext | ZCOP_GetAll,    //!< a getNext have been invoked in a getAll context

    ZCOP_RemoveAll   =   0x1000,    //!< removes all ZMasterFile selected ranks from collection
    ZCOP_LockAll     =   0x2000     //!< locks all ZMasterFile selected ranks from collection
};



/**
 * @brief The ZMatchSize_type enum Option for defining the size of comparison while matching index records
 */
enum ZMatchSize_type
{
    ZMS_Nothing         = 0,
    ZMS_MatchIndexSize  = 1,  //!< Index key full size is the comparison size : exact match between Index key content and search key
    ZMS_MatchKeySize    = 2   //!< Size of the search key buffer is the comparison size : Partial key search may use this
};
#endif // ZCOLLECTION_OP
const char *decode_ZCOP (uint16_t pZCOP);
/**
 * @brief The ZSIndexResult class single element resulting of a search operation using indexes.
 * It gives the Index relative position (rank) and its corresponding ZMasterFile record address (Index is not sensible to ZMasterFile ranks, but only addresses).
 */
class ZIndexResult
{
public:
  zrank_type      IndexRank=-1; //!< ZIndexFile rank
  zaddress_type   ZMFAddress=-1;//!< Corresponding ZMasterFile address

  ZIndexResult(void) = default;
  ZIndexResult(zaddress_type pZMFAddress,zrank_type pIndexRank) ;
  ZIndexResult(const ZIndexResult &pIn) ;
  ~ZIndexResult(void) {}

  ZIndexResult&  _copyFrom(const ZIndexResult &pIn) ;

  static ZIndexResult create(zaddress_type pZMFAddress,zrank_type pIndexRank);
  /**
   * @brief reset resets ZSIndexResult to is 'null' value {-1 ; -1 }
   */
  void reset (void) {ZMFAddress=-1; IndexRank=-1;}
  bool isNull (void) const {return IndexRank<0;}
  bool isInvalid(void) const {return ZMFAddress<0;}

    ZIndexResult& operator = (ZIndexResult &pZIR) {   memmove (this,&pZIR,sizeof(ZIndexResult)); return *this;}

    bool operator == (ZIndexResult &pZIR) {  return  memcmp (this,&pZIR,sizeof(ZIndexResult)); }

    bool operator > (ZIndexResult &pZIR) {  return  (memcmp (this,&pZIR,sizeof(ZIndexResult))>0); }
    bool operator < (ZIndexResult &pZIR) {  return  (memcmp (this,&pZIR,sizeof(ZIndexResult))<0); }

};



/**
 * @brief The ZSIndexCollectionContext class Maintain the context of a collection :
 * - various type of searches and other operations
 * - lock mask to be applied to records
 * - last operation code
 * - last operation status
 * - compare size if search operation
 * - comparison routine to use for key match (defaulted to ZKeyCompareBinary)
 * - Lowest key content to search (unique key content if simple search)
 * - Highest key content to search (in case of searching for Interval values)
 */
class ZIndexCollectionContext
{
public:
    ZIndexCollectionContext(void) { clear();}

    void setup ( const ZDataBuffer &pKeyLow,
                 const ZDataBuffer* pKeyHigh,
                 ZIFCompare    pZIFCompare,
                 const ssize_t pCompareSize);


    ZStatus setStatus(ZStatus pSt) {
      ZSt=pSt;
//                                    if (Collection!=nullptr)
//                                            Collection->ZSt = pSt;
      return ZSt;
    }
    ZStatus getStatus (void) {return ZSt;}
//    zlock_type getLock(void) {return Lock;}

    void reset (void) ;
    void clear();

    ZDataBuffer         KeyContent;                 //!< In case of simple search : key content value to search, or lowest key value to search in case of Interval search
    ZDataBuffer         KeyHigh;                    //!< In case of Interval search : highest value to search for
//    ZIndexFile*         ZIFFile=nullptr;            //!< ZIndexFile to refer to
    ssize_t             CurrentZIFrank=-1;            //!< Current ZIndexFile rank during search operations.
//    ZMatchSize_type     ZMS=ZMS_Nothing;            //!< Matching type
    ZIFCompare          Compare=ZKeyCompareBinary;  //!< Comparison routine : deduced from key data type
    ssize_t             CompareSize=-1;             //!< Size of key content to compare : deduced from ZMatchSize_type
//    zlock_type          Lock=ZLock_Nolock;          //!< Last lock mask applied or tried to be applied (in case of error)
    uint16_t            Op;                         //!< Last operation on collection see @ref ZCollection_Op_type

    ZIndexResult        ZIFLast;                    //!< last ZIndexFile rank content accessed ; either ZMasterFile rank & address if no collection - or collection content if there is a collection
    long                InputCollectionIndex=-1;    //!< either ZIndexFile record rank or InputCollection rank. In case of there is already a collection : current collection index . If no collection : ZIndexFile rank
    long                BaseIndex=-1;               //!< current base collection index meaning rank for ZArray<ZSIndexResult> defined as Base for ZSIndexCollection.

    ZStatus ZSt=ZS_NOTHING;                         //!< last current status
    bool isInit=false;                              //!< Have collection been initialized (true)? or not (false)
    bool FInitSearch = false;                       //!< has a sequential search operation been initialized (true)? or not (false)
};//ZIndexCollectionContext



class ZIndexCollection : public ZArray<ZIndexResult>  // Ranks of the corresponding index - & address of corresponding ZMasterFile block
{
    typedef ZArray<ZIndexResult> _Base;


    friend class ZMasterFile;
    friend class ZIndexFile;
    friend class ZRawMasterFile;
    friend class ZRawIndexFile;

public:
    ZIndexCollection(void) {
//      ZIFFile=nullptr;
    }
    ZIndexCollection(const ZIndexCollection& pIn) { _copyFrom(pIn); }

    ZIndexCollection(ZRawIndexFile *pZIFFile) ;
    ZIndexCollection(ZRawMasterFile &pZMFFile, const long pIndexRank) ;

    ~ZIndexCollection(void) {
      if (InputCollection!=nullptr)
        delete InputCollection;
      }  // And call _Base::ZArray destructor

    ZStatus initSearch(ZArray<ZIndexResult> *pCollection);

    void setup         (const ZDataBuffer &pKeyLow,
                        const ZDataBuffer* pKeyHigh,
                        ZIFCompare    pZIFCompare,
                        const ssize_t pCompareSize,
                        const zlockmask_type pLock) {
             Context.setup(pKeyLow,
                           pKeyHigh,
                           pZIFCompare,
                           pCompareSize);
    }

    ZStatus evaluate(ZDataBuffer&wRecordContent) {return (Argument.evaluate(wRecordContent)?ZS_FOUND:ZS_NOTFOUND);}
    void reset(void);
    void clear(void);

    ZIndexCollection& _copyFrom(const ZIndexCollection& pIn);

    ZStatus getZIRfromZIF(ZIndexResult &pZIR, const zrank_type pIndexRank);

    ZStatus getFirstRawRank(ZIndexResult &pZIR);
    ZStatus getNextRawRank(ZIndexResult &pZIR) ;
    ZStatus getPreviousRawRank(ZIndexResult &pZIR) ;
    ZStatus getCurrentRawRank(ZIndexResult &pZIR);


    ZIndexResult getCurrentSelectedRank(void);
    ZStatus getFirstSelectedRank(ZDataBuffer &pRecordContent, ZIndexResult& pZIR);

    ZStatus getNextSelectedRank(ZDataBuffer &pRecordContent, ZIndexResult &pZIR);
//    ZStatus zgetNextRetry(ZDataBuffer &pRecordContent, ZSIndexResult &pZIR, long pWaitMs, int16_t pTimes);
    ZStatus _getRetry(ZDataBuffer &pRecordContent, ZIndexResult &pZIR);

    ZIndexResult getPreviousSelectedRank (const zlockmask_type pLock);
    ZStatus zgetAllSelected(const zlockmask_type pLock);
//    ZStatus _getAllRetry(void);
//    ZStatus zgetAllRetry(long pWaitMs,int16_t pTimes);

//    ZStatus zlockAll(const zlock_type pLock);
//    ZStatus zlockAllRetry  (long pWaitMs,int16_t pTimes);
//    ZStatus zunlockAll(void);

    ZStatus zremoveAll (void);
    ZStatus _removeAllRetry (void);
    ZStatus zremoveAllRetry (long pWaitMs,int16_t pTimes);

    ZStatus zsetFieldValue(const ssize_t pOffset, ZDataBuffer &pFieldValue);

    bool isInit(void) {return Context.isInit;}

    ZIndexCollectionContext& getContext(void) {return Context;}

    ZStatus getStatus(void)             {return Context.ZSt;}
    ZStatus setStatus(ZStatus pStatus)  {Context.ZSt=pStatus; return Context.ZSt;} // sets the current context status and returns it
//    zlock_type getLock(void)            {return Context.Lock;}
//    void setLock(zlock_type pLock)      {Context.Lock = pLock;}

private:
    ZRawIndexFile *ZIFFile=nullptr;    //!< ZIndexFile file to work with
    ZSearchArgument Argument;       //!< stores the evaluation conditions for the current collection : one collection has one search argument data structure

    ZArray<ZIndexResult> *InputCollection=nullptr; //!< stores the collection as Input to work with for evaluating ranks. If nullptr the whole set of ZRF ranks is taken
    bool        FCollection=false ;  //!< is there any input collection given (true) or should we address directly ZIndexFile ranks (false)

    ZIndexCollectionContext Context;    //!< current search context for
    ZMatchSize_type         ZMS;

};//ZSIndexCollection



} // namespace zbs
#endif // ZINDEXCOLLECTION_H
