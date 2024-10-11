#ifndef ZMASTERFILE_H
#define ZMASTERFILE_H

#include <ztoolset/zam.h>
#include <zrandomfile/zrandomfile.h>
#include <memory>
using namespace std::placeholders ; //! for std::bind
#include <functional>               //! for std::function<>

#include <cstdarg>

using std::function ;
using std::bind;
using std::bad_function_call;

using std::mem_fn ;
using std::mem_fun ;

using std::ref ;
using std::cref;

#include <bits/stl_function.h>  //!for std::mem_fn

#include <ctime>

//! Programer's help : in place of letting compiler issue an error that may not be quickly understood when it's late in the night...
#define _METHOD_NOT_ALLOWED__ {fprintf(stderr,"%s-F-METHODNOTALLOWED Fatal error : this method is not allowed with this kind of object.\n",\
                                _GET_FUNCTION_NAME_ );\
                                abort(); };

#include <type_traits>


namespace zbs //========================================================================
{





/**
 * @dev ZMasterFile - ZIndexFile -----------private content------------------------------------------------
 *
 *
 *  @public
 * @addtogroup ZAM ZArray Master Index or ZMasterFile (also named ZAM), and ZIndexFile-es
 *  Concept :
 *
 *  ZMasterFile is an extension of ZArray, with all implemented routines needed to manage ZIndexFile-es.
 *  Virtually, one may have as many indexes as one wants.
 *  A ZIndexFile is a particular order of ZMasterFile according a Key value.
 *  This key must be defined within a _Bulk (template parameter) structure, in order to position the key field named mandatorily _Key.
 *
 *  ZMasterFile itself -by difference with ZSortedArray- is not ordered, and rows within it are stored in the creation/deletion order.
 *  ZIndexFile is dependant from one and only one ZMasterFile.
 *  ZIndexFile need to be registrated to one ZMasterFile.
 *  This registration is made right at the beginning of ZIndexFile instantiation (CTOR).
 *  Deregistration is made within the class destructor.
 *
 *  However, ZIndexFile is not a child class of ZMasterFile and is an "independant" class in the meaning that there is no inheritence between ZMasterFile and ZIndexFile classes.
 *
 *  Accessing ZAM content :
 *      Since modifying ZAM row could possibly modify a Key field value without updating corresponding indexes IT IS NOT POSSIBLE to access directly ZAM rows content for modification.
 *      General case :
 *          operator '[]' : returns A COPY of corresponding ZAM rank (while ZArray operator [] returns a REFERENCE).
 *          operator  '=' : is forbidden.
 *      Getting a reference to ZAM rank :
 *          It is possible to get a reference to ZAM row content corresponding to an rank order value using getRef method.
 *          A 'const _Type&' reference is then returned.
 *
 *  Performance matters :
 *      each time a single operation occurs on ZMasterFile, it will be cascade-impacted on ALL defined ZIndexFile indexes.
 *      The more numerous ZIndexFile a ZMasterFile may have, the slower the response time could be, and the more time consumption will result in each operation.
 *
 * update :
 *      either call ZAM () :
 *          overloaded base methods are used :
 *              - replace for modifying Row content. Propagates to all indexes
 *              - erase for suppressing a Row and corresponding ZIndexFile(es) element(s)
 *              - insert for creating a new Row in the middle of ZMasterFile
 *              - push methods suite , pop methods suite  for adding removing Row at top or bottom of array and maintaining the related ZIndexFile(es)
 *      or call external update method of ZIndexFile
 *              - remove : removes the first ZAM Row with a corresponding Key value in ZIndexFile
 *              - removeAll : removes all ZAM rows with the Key values found in ZIndexFile
 * search :
 *      ZAM :
 *
 *          free search  for a void* pointer and a size_t size within ZAM for a binary sequense
 *                  until HighValue (End of ZArray).
 *                  Returns ZReturn containing ZAM row index if found.
 *
 *
 *      search functions using Key values use ZIndexFile
 *      - seek      returns ZReturn
 *      - seekAll   returns ZMulti_Result passed as pointer in arguments
 *
 * THREADS:
 *  Need to implement ZThreads : one ZThread by index -> build new classes ZMasterFile_thr & ZIndexFile_thr
 *  ZIndexFile thread update method is threaded and is always waiting for ZMutex to be unlocked by
 *---------------------------------------------------------------------------------------------
 *  JOURNALING : Commit / Rollback
 *---------------------------------------------------------------------------------------------
 *  Are not concerned by journaling : seek - seekAll
 *
 * Journaling enabling/disabling
 * -----------------------------
 * Journaling can be enabled (by default) or disabled using the appropriate methods :
 *  setJournaling (true/false) : true enables journaling process- false disable journaling process(and all overhead caused by journaling activity)
 *  Journaling state (on or off, meaning true or false) can be obtained from ZMasterFile using appropriate method :
 *  bool getJournaling ()
 *
 *  setting off journaling :
 *      When journaling is set off, every journalized change is committed automatically BEFORE journaling is set off, then journaling is reset-ed
 *
 *  Journaling data fields/structures :
 *   @ZMasterFile :
 *      bool ZFJournal
 *      ZArray<ZJournaling_struct> ZAJournaling  // should be shared while using ZThreads : need to add ZMutex on it
 *
 *
 *
 *  at ZMasterFile level:
 *      ZArray<ZJournaling_struct> is created & maintained by ZMasterFile :
 *          ZJournaling_struct :
 *              - ZOp
 *              - journaling state
 *              - ZMasterFile row index befor action or after action if add
 *              - _Type content (former content for replace & erase, new content for insert/ for _Key value otherwise)
 *              - ZStatus
 *
 *  at ZMasterFile level :
 *      ZArray<operation_struct> is created and maintained :
 *              - ZOp
 *              - _Type content
 *              - ZStatus
 *              - row index before action or after action if add
 *
 *  ZAMState values :
 *      - nothing
 *      - inserted
 *      - marked to deletion
 *      - marked for modification (journal content is _Type if ZMasterFile or _Bulk if ZIndexFile former value, )
 *LATER
 *      special value
 *      - beginmark (journaling reference index is ZAMMarkNumber a sequential number incremented)
 *      - endmark   (journaling reference index  is ZAMMarkNumber)
 *LATEREND
 *
 *  commit is done at ZMain level :
 *      commits all registered ZIndexFile-es until ZAMMark transaction number (long). Default is all transactions
 *      commit ZMasterFile :
 *          apply changes if needed (modification does not matter because already taken into account)
 *              until ZAMMark transaction number (long). Default is all transactions.
 *
 *          reset ZAState at row level
 *          clear Journal
 *
 *  rollback is done at ZMain level :
 *      rollback all registered ZIndexFile-es  from ZAMMark transaction number till beginning. Default is all transactions.
 *      rollback detail :
  * -----------------------+-----------------------------------------
  *         ZOperation     |  rollback action
  * -----------------------+-----------------------------------------
  *         insert              erase (row index)
  *         replace              restore former _Bulk content at row index
  *         erase               insert _Bulk content at row index
  *
  *
  * CommitUserFunction :
  *---------------------
  *         This is a user function that is called via a pointer to this function and returns a ZCommitStatus status value.
  *         The returned status may have an impact on commit operation according its value.
  *         This function is called at the row level, BEFORE commit operation is done for the row.
  *     CUF Arguments:
  *     --------------
  *         This function has two main arguments :
  *             - the first one is a reference to the ZAM row being processed during the commit operation.
  *                when ZBS calls this function it passes the current row as a constant with a type definition as _Type (parameter class).
  *                 As it is a constant, it may not and cannot be modified (for integrity purposes)
  *                 If you need to replace this row, use the ZAM methods facilities.
  *
  *             - the second one is a pointer to optional user's argument block.
  *                 As it is passed as a (void *) pointer, et is up to the user to define the structure and the content of this data,
  *                 and therefore to maintain its integrity.
  *     CUF returned status
  *     -------------------
  *         ZCS_Success,          //! commit is OK
  *         ZSC_Skip,             //! journal element must be skipped without interrupting the commit process, next journal element will be processed
  *         ZCS_Error,            //! commit is not to be continued but control is given back to caller with error.  ZS_USERERROR status is then returned.
  *         ZCS_Fatal             //! commit is errored. Processing is to be interrupted immediately with signal (abort()
  *
  *
  * @endpublic
 *
 * -----------END JOURNALING--------------------------------
 *
 *  WARNING: ZIndexFile & ZMasterFile needs to be together there within ZIndexFile.h for compiler limitations reason
 *
 * @endpublic
 *
 *  ZMasterFile
 *=========
 *  Create a ZMasterFile class :  ZArray <ZAMContent(_Type),ZAMState>  with
 *
 *  ZArray <Index descriptor>  : list of registered indexes / joins
 *      where Index descriptor contains ZIndexFileId and the pointers to the following std::function<>
 *
 *                           ZIFDescriptor_struct
 *------------------------------+-------------+-------------------------------------------------------------------
 *     function                 |return       |      arguments
 *------------------------------+-------------+-------------------------------------------------------------------
 * journaling
 *      - ZIndexFile::commit          ZStatus        user function pointer  (long)ZAMMark - defaulted to -1 for All
 *      - ZIndexFile::rollback        ZStatus        (long)ZAMMark - defaulted to -1 for All
 * update
 *      - ZIndexFile::add            ZStatus        (_Type &)Content,(size_t)ZAMRow
 *      - ZIndexFile::remove         ZStatus        (size_t) ZAMRow
 *      - ZIndexFile::replace         ZStatus        (_Type &)Content,(size_t)ZAMRow
 * search
 *      - ZIndexFile::seek           ZResult        (_KeyType) Key                                                      _Type is argument:_Key needs to be extracted from _Type
 *      - ZIndexFile::seekAll        void           (_KeyType) Key, (ZMulti_Result*)Result -as output                   _Type is argument:_Key needs to be extracted from _Type
 *      - ZIndexFile::seekAllPartial void           (_KeyType) Key,(size_t)Size, (ZMulti_Result*) Result -as output     _Type is argument:_Key needs to be extracted from _Type
 *
 *     Index identifier : is the unique number that identifies the ZIndexFile and is given back by ZMasterFile
 *
 *
 *
 *
 *                             ------------------------
 * Methods :
 * --------
 *
 *  overloaded '=' method :
 *          _Type <- ZMasterFile[index]
 *          ZMasterFile[index] = something : use replace method
 *
 *  Methods to add :
 *    Available to users :
 *      - seek  ( KeyContent, Keynumber=0)
 *      - seekAll (KeyContent, Keynumber=0)
 *
 *      - replace  (replaces partially = method) : replace content (assignes _Type, ZAMState is set to ZAMReplace
 *
 *
 *    Internal :
 *      - register      gives back a long with ZMasterFile.size() : empty = -1, other : need to rebuild
 *      - deregister
 *
 *      - commit        (see journaling)
 *      - rollback
 *
 *  overload of methods :
 *
 *      Local ZMasterFile method             ZIndexFile Method
 * ---------------------------------+-------------------------
 *      - push , push_front         : -> ZIndexFile::add
 *      - popRP, pop, popRP_front   : -> ZIndexFile::remove
 *      - insert                    : -> ZKeyIndex::add
 *      - erase                     : -> ZKeyIndex::remove
 *
 *  for including call to appropriate ZKeyIndex method
 *
 *  ZIndexFile registering data structure  :
 *      std::function & std::bind
 *
 *
 *
 * CTOR:
 *      Initialize Journaling ZArray
 *      Initialize ZIndexFile registration ZArray
 * DTOR :
 *      deregistrate every registrated ZIndexFile
 *      commit and destroy journaling if journaling is enabled
 *
 *--------------------------------------------------------------
 * ZIndexFile
 * ======
 *  ZIndexFile : <_Type,_Bulk,_KeyType,_Compare>
 *
 *  CBulk implemented<_Bulk,_KeyType,_Compare>
 *
 *              - orders Key
 *
 *
 * ZIndexFile methods naming convention:
 * ---------------------------------
 *              routines prefixed with '_' are internal and may be called by ZMasterFile
 *              all routines from _Base (ZArray) are private or overloaded :
 *              examples :  size() gives the size of ZAM.
 *                          push suite, pop suite are private
 *
 *              other routines are external and may be called by user.
 *
 *              General convention:
 *                  insert() routine, inserts a row in an ZArray (even if it impacts on ZIndexFilees) while add is a ZIndexFile operation
 *                  erase()  suppresses a row from a ZArray while remove and removeAll suppress the ZAM/ZIndexFile elements corresponding to a Key value searched within a ZIndexFile.
 *                  replace() changes the ZArray row value
 *
 *  ZIndexFile main data structures:
 * -----------------------------
 *              template <typename _KType>
 *              struct ZIndexFile_struct {
 *                          size_t      ZAMIdx ;            //index giving ZAM row
 *                          _KType      _Key;
 *                          ZAMState    State;              // @see ZAMState
 *                                  } ;
 *
 *              zreverse : reverse index  order by ZAM index giving ZIndexFile row index.
 *
 *  CTOR : ( pointer to ZMasterFile )
 *      -> register ZIndexFile within ZMasterFile
 *      -> rebuild index
 *
 * External methods (available to programer user):
 *      add (Key content, container row index)  giving ZStatus
 *      remove (row index)                      giving ZStatus
 *      seek (Key content)                      giving ZResult          (only one index result + status)
 *      seekAll (Key content, Operator)         giving ZMulti_Result    (all results + final status)
 *      change (row index)                      giving ZStatus
 *
 * Internal methods :
 *      -register (std::function<> add, remove, seek,change ..)
 *                       send a registration request to ZMasterFile giving ZIndexFileId in return
 *                       (long - unique identifier for ZIndexFile registrered on ZMasterFile.
 *                        ZMasterFile returns -1 if an error occurred.
 *
 *      _deregister      : sends a suppression request to ZMasterFile using its ZIndexFileId.
 *                        returns ZStatus : ZS_SUCCESS or ZS_NOTFOUND if ZIndexFileId has not been found or is incorrect
 *
 *      _build index     : build / rebuild defined index against ZMasterFile
 *
 *      commit          (see journaling)
 *      rollback        (see journaling)
 *
 *
 *operators
 * --------
 *
 *      [ ]  returns the content of ZMasterFile  at index given by ZIndexFile order   (i. e. ZAM[ZIndexFile[index].ZAMIdx]  )
 *
 *
 *---------------------------------------------------------------------
 *  @public
 *  ZAM search actions - update actions
 *  -----------------------------------
 *      Search actions (seek, seekAll, seekAllPartial) have to be made using ZIndexFile (and from ZIndexFile).
 *      Update actions may be made equally on ZMA or on ZIndexFile, with the same semantic and the same result :
 *
 *      if an action is made on ZMA, (ZMA source) :
 *          - direct action on container data will be made (and journalized if required)
 *          - action will be propagated to ZIndexFile-es
 *
 *      if action is made on ZIndexFile (ZIndexFile source) :
 *          - no direct action is made, but corresponding action is called to ZMA.
 *
 *
 *    Possible actions on ZMA
 *
 *      -push
 *              _Base::push()
 *              journalize
 *              update ZIndexFilees
 *
 *      -erase
 *              _Base::erase()
 *              journalize
 *              updateZIndexFilees
 *
 *      -replace
 *              _Base::Tab[index] = value
 *              journalize
 *              updateZIndexFilees
 *
 *
 * @publicend
 *
 *
 *
 * @devend
 */

/**
 * constants : journaling - index descriptors - journaling history
 */
const size_t _cst_zmf_journaling_allocation     = 20; // 20 operations in journaling before claiming for new allocation
const size_t _cst_zmf_journaling_reallocquota   = 10; // 10 operations block for space reallocation

const size_t _cst_zmf_descriptor_allocation     = 5 ;  // 5 indexes allocation space
const size_t _cst_zmf_descriptor_reallocquota   = 2 ;  // 2 indexes incremental number

const size_t _cst_zmf_history_allocation = 20 ;
const size_t _cst_zmf_history_reallocquota = 5 ;







#ifdef __COMMENT__

//! Journaling options
//!
enum ZJOption { ZJDisable,
                ZJManualCommit,
                ZJAutoCommit
};

struct ZJournalAction_struct
{
    std::function<ZStatus(void*,ZAMMark_type)>                  commit;
    std::function<ZStatus(ZAMMark_type)>                        rollback;
};

template <class _Type>
struct ZJournaling_struct
{
    long            ZIndexFileId    ;   //! ZIndexFile identification RFFU
    ZOp             Operation   ;   //! journalized ZOperation
    ZAMState_type   State       ;   //! state of the Row withing the ZAM
    long            DataSize    ;   //! size of Data
    void            *Data       ;   //! Journalized data content (if ever)
    size_t          Index       ;   //! reference ZAM index for journalized object
    ZStatus         Status      ;   //! last ZStatus if an error occurred
};

struct ZCJournaling_struct {
long            ZIndexFileId    ;   //! ZIndexFile identification RFFU
ZOp             Operation   ;   //! journalized ZOperation
ZAMState_type   State       ;   //! state of the Row withing the ZAM
long            DataSize    ;   //! size of Data
void            *Data       ;   //! Journalized data content (if ever)
long            Index       ;   //! reference ZAM index for journalized object
ZStatus         Status      ;   //! last ZStatus if an error occurred
};


class ZJournalingElement
{

public:
    ZJournalingElement(long pZIndexFileId,
                       const ZOp pOperation,
                       const ZAMState_type pState,
                       void *pData,
                       const long pDataSize,
                       const long pIndex,
                       const ZStatus pStatus) ;
    ZJournalingElement(const ZCJournaling_struct &pZCJ);

    virtual ~ZJournalingElement (void) ;

    struct ZCJournaling_struct Content;
    char *CData=(char*)Content.Data;

    void* store (long pZIndexFileId,
                 ZOp pOperation,
                 ZAMState_type pState,
                 void *pData,
                 const long pDataSize,
                 const long pIndex,
                 const ZStatus pStatus) ;

    void updateStatus (long pZIndexFileId,
                 ZAMState_type pState,
                 const ZStatus pStatus);



    void *getData(void) {return Content.Data;}

    size_t  getPrintSize (void)  { return ((Content.DataSize*4)+250);}

    const char* _sprint (char *pOBuf, char *pHeader1, char *pHeader2, bool pRowDump=false);

private:
};//ZjournalingElement


class ZCJournaling : public ZArray<ZJournalingElement*>
{
    typedef ZArray<ZJournalingElement*> _Base;
public:
    ZCJournaling (ZJournalAction_struct* pZMAActions,
                  size_t pAlloc=_cst_zmf_journaling_allocation,
                  size_t pQuota=_cst_zmf_journaling_reallocquota)
    {
       ZMAActions=pZMAActions;
       Allocation=pAlloc;
       ReallocQuota=pQuota;
       Option=ZJAutoCommit ;
    };

    virtual ~ZCJournaling ()
    {
        for (long wi = 0;wi<size();wi++)
                                delete Tab[wi];

    }

    using _Base::size ;
    using _Base::last ;
    using _Base::lastIdx ;
    /**
     * @brief _getOption gives back the current journaling option(s) as a ZJOption value
     */
    ZJOption _getOption(void) {return (Option);}
    ZStatus setJournalingRollback(ZJOption pZJournalingOption);
    ZStatus setJournaling(ZJOption pZJournalingOption);
    void setQuota (size_t pAlloc,size_t pQuota=_cst_zmf_journaling_reallocquota) {Allocation=pAlloc; ReallocQuota=pQuota;}

    ZStatus journalize(const long pZIndexFileId,
                       const ZOp pOperation,
                       const ZAMState_type pState,
                       void *pData,
                       const long pDataSize,
                       const long pIndex,
                       const ZStatus pOperationStatus)
    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
        ZJMovement = new ZJournalingElement(pZIndexFileId,
                                            pOperation,
                                            pState,
                                            pData,
                                            pDataSize,
                                            pIndex,
                                            pOperationStatus);
        if (push(ZJMovement)<0)
                    return(ZS_ERROR);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
#endif
    return(ZS_SUCCESS);
    };


/**
 * @brief setLastStatus  set the journal ZStatus to the given value for the last journalized operation
 * @param pOperationStatus
 * @return
 */
ZStatus setLastStatus(const ZStatus pOperationStatus)
    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
    if (isEmpty())
            {
            _Mtx.unlock();
            return (ZS_OUTBOUNDLOW);
            }
    Tab[lastIdx()]->Content.Status = pOperationStatus;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
#endif
    return(ZS_SUCCESS);
    };

ZStatus
setErrorStatus(const ZAMState_type pZAMState,const ZStatus pZStatus,const long pIndexId=-1,const long pIdx=-1)
{


#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
    if (isEmpty())
            {
            _Mtx.unlock();
            return (ZS_OUTBOUNDLOW);
            }
    if (pIdx>lastIdx())
            {
            _Mtx.unlock();
            return(ZS_OUTBOUNDHIGH);
            }
    long wIdxRef = (pIdx<0)?lastIdx():pIdx ;  // if -1 then lastIdx() if not :


    Tab[wIdxRef]->updateStatus(pIndexId,pZAMState,pZStatus);

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
#endif
    return(ZS_SUCCESS);
};

/**
 * @brief setLastStatus  set the journal ZStatus to the given value for the last journalized operation
 * @param pOperationStatus
 * @return
 */
ZStatus setLastZAMState(const ZAMState_type pZAMState,const long pIndexId=-1)
    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
    if (isEmpty())
            {
            _Mtx.unlock();
            return (ZS_OUTBOUNDLOW);
            }
    Tab[lastIdx()]->Content.State = pZAMState;
    Tab[lastIdx()]->Content.ZIndexFileId = pIndexId;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
#endif
    return(ZS_SUCCESS);
    };

    void printJournal(ZAMMark_type pZAMmark,bool pRowDump,FILE *pOutput);

//!
//! operators overload
//!
     ZCJournaling_struct& operator [] (const long pIndex) { return (_Base::Tab[pIndex]->Content);}   // we must be able to access to this operator function from the inherited class




//! Overload of base pop / popRP routines
//!
    void pop()
    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
//        memmove (pOutput,_Base::last(),sizeof(ZJournalingElement));
        delete (_Base::last());
        _Base::pop();
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
#endif
        return ;
    };

//! Non destructive pop() to keep the memory allocated for history
//!
    void popNonDestructive()
    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
        _Base::pop();
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
#endif
        return ;
    };
    ZCJournaling_struct &popRP(ZCJournaling_struct *pOutput)
    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
        memmove (pOutput,_Base::last(),sizeof(ZJournalingElement));
        delete (_Base::last());
        _Base::pop();
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
#endif
        return (*pOutput);
    };

    ZCJournaling_struct &popRP(ZCJournaling_struct &pOutput)
    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
        memmove (&pOutput,&_Base::last(),sizeof(ZJournalingElement));
        _Base::pop();
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
#endif
        return (pOutput);
    };

    friend ZCJournaling_struct& operator << (ZCJournaling_struct &pOutput,ZCJournaling *pZCJ)
         {
         return (pZCJ->popRP(&pOutput));
         };

    friend ZCJournaling_struct& operator << (ZCJournaling_struct &pOutput,ZCJournaling &pZCJ)
         {
         return (pZCJ.popRP(&pOutput));
         };

friend ZCJournaling_struct& getZCJournaling(ZCJournaling *pJournaling,ZCJournaling_struct* pOutJournaling, size_t pIdx)
{
memmove(&pOutJournaling,&pJournaling->Tab[pIdx]->Content, sizeof(ZCJournaling_struct));
return(*pOutJournaling);
}

friend ZCJournaling_struct& popZCJournaling(ZCJournaling_struct& pOutJournaling,ZCJournaling &pJournaling)
{
    memmove(&pOutJournaling,&pJournaling.Tab[pJournaling.lastIdx()]->Content, sizeof(ZCJournaling_struct));
    delete (pJournaling.last());
    pJournaling.pop();
    return((ZCJournaling_struct &) pOutJournaling);
}

    bool ZFJournaling=true;
    bool ZFAutoCommit=true;

#ifdef __USE_ZTHREAD__
    ZMutex _Mtx;
#endif
private:

   size_t Allocation;
   size_t ReallocQuota;

   ZJOption Option;

    ZJournalingElement *ZJMovement ;


    ZJournalAction_struct* ZMAActions ;

private:


};//ZCJournaling

//!=====================End ZCJournaling=========================


class ZHistoryElement
 {
public:
    ZHistoryElement(ZJournalingElement*pElement,bool pRolledBack=false)
    {
        Element=pElement;
        timecommit= std::time(nullptr);
        RolledBack=pRolledBack;
        return;
    } ;
    virtual ~ZHistoryElement ()
        {
        if (Element!=nullptr)
                        {
                        delete Element;
                        Element=nullptr;
                        }
        return;
        };

    const char* _sprint (char *pOBuf,char* pHeader1,char* pHeader2,bool pRowDump);

    size_t getPrintSize(void) {return(Element->getPrintSize()+50);};
private:
    bool RolledBack = false;
    std::time_t timecommit ;
    ZJournalingElement *Element=nullptr;
};


class ZCHistory : public ZArray <ZHistoryElement*>
{
public:
    typedef ZArray <ZHistoryElement*> _Base;
    ZCHistory (size_t pAlloc=_cst_zmf_history_allocation,size_t pReallocQuota=_cst_zmf_history_reallocquota):ZArray(pAlloc,pReallocQuota)
        {    }

    virtual ~ZCHistory ()
    {
        while (lastIdx()>=0)
                    {
                    delete (this->popR());
                    }
        return;
    };



    void store (ZJournalingElement * pZElt,bool pRolledBack=false)
    {
#ifdef __USE_ZTHREAD__
    _Mtx.lock();
#endif

     ZHistoryElement *wHE = new ZHistoryElement(pZElt,pRolledBack);
     if (push (wHE)<0)
            {
            fprintf(stderr,"ZCHistory::store-F-CANTPUSH push operation on ZCHistory has been errored. History size is %ld\n",
                    size());
#ifdef __USE_ZTHREAD__
            _Mtx.unlock();
#endif
            abort();
            }
#ifdef __USE_ZTHREAD__
     _Mtx.unlock();
#endif
     return;
    };
    //!
    //! \brief _historizeLastJournalEvent for historize committed operation just use with ZCJournaling
    //!                                     for historizing a rolledback operation, mention in addition 'true';
    //!                     Nota Bene: historizing a journalized operation DESTROY it from the journal.
    //!                                It is so further impossible to commit this operation afterwards.
    //!
    void _historizeLastJournalEvent (ZCJournaling *pZJ,bool pRolledBack=false)
        {
            store(pZJ->last(),pRolledBack);
            pZJ->popNonDestructive();  // pop ZCJournaling and keep the ZJournalElement alive for ZHistory
            return;
        };

    void _historizeErroredLastJournalEvent (ZCJournaling *pZJ,
                                        ZStatus pZStatus,
                                        const ZAMState_type pZAMState,
                                        long pIndex=-1,
                                        long pIdx=-1)
        {
        pZJ->setErrorStatus(pZAMState,pZStatus,pIndex,pIdx);
        store(pZJ->last(),false);

        pZJ->popNonDestructive();  // pop ZCJournaling and destroy
        return;
        };

    void _historizeRolledBackJournalEvent (ZCJournaling *pZJ)
        {
            store(pZJ->last(),true);
            pZJ->popNonDestructive();  // pop ZCJournaling and keep the ZJournalElement alive for ZHistory
            return;
        }
//!
//! same as _historizeLastJournalEvent but with << operator
//!
    friend ZCHistory & operator << (ZCHistory  &pZCH,ZCJournaling *pZJ)
         {
         pZCH.store(pZJ->last()) ;  // thread safe method
         pZJ->pop();
         return(pZCH);
         }


    void printHistory(ZAMMark_type pZAMmark,bool pRowDump,FILE *pOutput);


#ifdef __USE_ZTHREAD__
    ZMutex _Mtx;
#endif


};//class ZCHistory




//!============================End ZCHistory==============================
#endif __COMMENT__



template <class _KeyType>
struct ZIndexFile_struct            //!< Index core structure : container of ZArray ZIndexFile.
{
    _KeyType        _Key;       //! Key content
    size_t          ZAMIdx;     //! ZMasterFile row index
    size_t          ZJIdx;      //! left for dynamic ZIXJoins
    ZAMState_type   State;      //!  see ZAMState : journaling state for the index/table rank   No -> create a separate ZArray<ZAMState> called ZState
                                //!  leave ZArray<_Type> as _Base (much more fluent)
                                //!  maintain the separate ZArray ZState in appropriate base overloaded functions
};


//!@dev
//! IRON RULE : std::function<> must be defined (std::bind) and must stay within Object instance where the methods are
//!             pass pointer to this std::function<> to use it in an other object
//!             OR redefine it using std::bind within the calling object.
//!             But DO NOT Pass it by value or reference: Invoker will bullshit the pointers to arguments...
//!             This is how it is. No discussion.
//!
//! SECOND :    Decision has been taken not to use std::ref and std::cref. Pointers are use in place of reference (especifically for _Type Element)
//!
//!@devend
//!



struct ZIFCallbackVector_struct                                          //! for managing interoperability ZMF ZIF
{
std::function<ZStatus(ZDataBuffer&,ZOp,size_t)>         _add;
std::function<ZStatus(size_t)>                          _erase;
std::function<ZStatus(ZDataBuffer&,size_t)>             _replace;
std::function<void* (void)>                             _askMe;
std::function<void (int)>                               _reset;   //! if  arg == 0   -> reset if arg != 0 -> clear
std::function<bool(ZData_Type_struct&,size_t,size_t)>   _ask;
ZCommitStatus (*CommitUserFunction) (ZDataBuffer&,ZOp,void *) ;//! pointer to a function that is activated when commit is done. Optional : nullptr if omitted
void * CommitArguments ;   //! pointer to Arguments for commitUserFunction. Optional : nullptr if no arguments or if ComitUserFunction is omitted
};

//template <class _KType>
struct ZMFJDescriptor_struct                                  //! for managing interoperability ZJD ZIX
{
std::function<void(size_t)>                             _add;
std::function<void(size_t)>                             _erase;
std::function<void(int)>                                _reset;
std::function<void(void)>                               _deregister;  //! ZIX tells to ZJD I am dead, so are you.
};




    typedef  ZCommitStatus (*CUF) (const ZDataBuffer&,ZOp,void *) ;  //! commit user function



descString
generateIndexRootName (descString &pMasterRootName,long pRank,descString* pIndexName=nullptr)
{
    descString wIndexName;
    wIndexName = pMasterRootName;
    wIndexName += "-";
    if (pIndexName==nullptr)
                {
                wIndexName+="index";
                }
            else
                {
                wIndexName += *pIndexName;
                }
    wIndexName += "-";
    sprintf(&wIndexName.content[wIndexName.size()],"%4ld",pRank);
    return wIndexName;
} // generateIndexRootName
/**
 * @brief generateIndexURI
 *              generates the index uri full base name (including directory path) but without any extension (and without '.' char)
 *
 * @param pMasterName
 * @param pPathDir
 * @param pRank
 * @param pIndexName
 * @return
 */
uriString
generateIndexURI(descString &pMasterName,uriString &pPathDir, long pRank,descString* pIndexName=nullptr)
{
uriString wURI;

    QUrl wUrl(pPathDir.toString());
    wURI=wUrl.toString(QUrl::PreferLocalFile);
    wURI.addConditionalDirectoryDelimiter() ;

    wURI += generateIndexRootName(pMasterName,pRank,pIndexName);

    return(wURI);
} //generateURI



//!------- Hereafter the complements to File Headers according the file type (ZMasterFile ZMCB or ZIndexFile ZICB)

//!
//!  Key fields description
//!
enum ZIFKeyType_type : unsigned char {
    ZIF_Nothing,
    ZIF_Alpha ,
    ZIF_Binary,
    ZIF_Other
};

class ZIndexField
{
    ZIndexField & _copyFrom(ZIndexField & pIn)  { Offset = pIn.Offset; Length = pIn.Length; Name = pIn.Name;}
public:
    ZIndexField() = default;
    ZIndexField(ZIndexField &pIn) { _copyFrom(pIn); }
    ZIndexField(ZIndexField &&pIn) { _copyFrom(pIn);  }

    ZIndexField &operator=(ZIndexField &pIn) { return _copyFrom(pIn);  }
    ZIndexField &operator=(ZIndexField &&pIn) { return _copyFrom(pIn);  }



    ssize_t     Offset;
    ssize_t     Length;
    utfcodeString  Name;
};



class ZIndexControlBlock {
public:
    int32_t                 StartSign ;
    ZBlockID                BlockID;
    ssize_t                 ICBSize;
    utfdescString           Name;           //! Name for the index is deduced from Master File name + index rank
    ssize_t                 KeySize;        //! overall size of the key (sum of keyfields size)
    ZIFKeyType_type         ZIFKeyType;
    bool                    AutoRebuild;
    ZSort_Type              ZIDuplicates;
    ZArray<ZIndexField> Fields;         //! fields positionning details  : there could be more than one field composing a key, even not adjacent



    void clear(void)
    {
        StartSign=cst_ZSTART;
        BlockID=ZBID_ICB;
        KeySize=0;
        ZIFKeyType=ZIF_Nothing;
        Fields.clear();
        return;
    }
    ZDataBuffer &           _exportICB (void) ;
    ZIndexControlBlock&     _importICB (ZDataBuffer &pBuffer) ;
    ZIndexControlBlock&     _importICB (unsigned char* pBuffer);
private:
    ZDataBuffer     Exchange;
};

/**
 * @brief ZIndexControlBlock::_exportICB exports ZIndexControlBlock content to a flat ZDataBuffer.
 * @return
 */
ZDataBuffer&
ZIndexControlBlock::_exportICB(void)
{
unsigned char*  wBuffer;
ssize_t         wBufferSize;
        Exchange.clear();
#if __DEBUG_LEVEL__ > 3
        size_t wS1 = offsetof(ZIndexControlBlock,Fields) ;  // for Debug
#endif
        Exchange.setData(this,offsetof(ZIndexControlBlock,Fields));
        Fields._export(wBuffer,wBufferSize);
        Exchange.appendData(wBuffer,wBufferSize);
        free(wBuffer);
        ZIndexControlBlock* wICB = (ZIndexControlBlock*) Exchange.Data;
        wICB->ICBSize = Exchange.Size;  //! update size of exported ICB
        return Exchange;
}// _export

ZIndexControlBlock&
ZIndexControlBlock::_importICB (ZDataBuffer& pBuffer)
{
    memmove (this,pBuffer.Data,offsetof(ZIndexControlBlock,Fields));
    Fields._import((unsigned char*)pBuffer.DataChar+offsetof(ZIndexControlBlock,Fields));
    return *this;
}// _import

ZIndexControlBlock&
ZIndexControlBlock::_importICB (unsigned char* pBuffer)
{
    memmove (this,pBuffer,offsetof(ZIndexControlBlock,Fields));
    Fields._import(pBuffer+offsetof(ZIndexControlBlock,Fields));
    return *this;
}// _import


class ZIndexFile;

/**
 * @brief The ZMasterFileControlBlock class Master File Control Block
 *
 *
 */
class ZMasterControlBlock {
public:
    int32_t                 StartSign ;
    ZBlockID                BlockID;
    ssize_t                 MCBSize;
    long                    IndexCount;
    bool    ZHistory;
    bool    ZJournaling;
//! here other infra-data concerning Master File behavior

//!   Index list
//!
    ZArray<ZIndexControlBlock>     Index;
    ZArray<ZIndexFile*>            IndexObjects;


    ZMasterControlBlock (void) {clear();};

    void pushICB(ZIndexControlBlock&pICB) {IndexCount=Index.push(pICB);};
    ZIndexControlBlock& popRPICB(ZIndexControlBlock& pICB)
        {
        Index.popRP(&pICB);
        IndexCount=Index.size();
        return pICB;
        };
    long eraseICB(long pRank)
    {
    if (Index.erase(pRank)<0)
                    return -1;
    IndexCount=Index.size();
    return IndexCount;
    };

    void clear(void) {Index.clear(); ZHistory=false; ZJournaling = false; BlockID=ZBID_MCB; StartSign=cst_ZSTART; Index.clear(); return;};
    const ZDataBuffer& _exportMCB(void);
    void  _importMCB (ZDataBuffer& pBuffer);

private:
    ZDataBuffer                     Exchange ;
} ;


/**
 * @brief ZMasterFileControlBlock::_exportMCB   exports ZMasterControlBlock content to a flat ZDataBuffer.
 * @return
 */
const ZDataBuffer& ZMasterControlBlock::_exportMCB(void)
{

    Exchange.clearData();
    Exchange.setData(this,offsetof(ZMasterControlBlock,Index));
    for (long wi = 0;wi < Index.size(); wi++)
    {
        const ZDataBuffer& wIndexBuffer=Index[wi]._exportICB();
        Exchange.appendData(wIndexBuffer);  //! only index definition not object pointer
    }
    ZMasterControlBlock* wMCB = (ZMasterControlBlock*) Exchange.Data ;
    wMCB->MCBSize = Exchange.Size;  //! update size of exported MCB in exported buffer
    return Exchange;
}
/**
 * @brief ZMasterFileControlBlock::_importMCB
 * @return
 */
void ZMasterControlBlock::_importMCB(ZDataBuffer& pBuffer)
{
    clear();
    memmove(this,pBuffer.Data,offsetof(ZMasterControlBlock,Index));

    unsigned char* wPtr = pBuffer.Data + offsetof(ZMasterControlBlock,Index);
    for (long wi = 0;wi < IndexCount; wi++)  //! IndexCount has been updated from memmove
    {
        ZIndexControlBlock wICB ;

        wICB._importICB(wPtr);
        pushICB(wICB);
        wPtr+=wICB.ICBSize;
        if (wPtr >= (pBuffer.Data+pBuffer.Size))
                        break;
    }// for
    return ;
}



//!=================================ZIndexFile===============================
//!
//!

class ZMasterFile;


class ZIndexFile : private ZRandomFile
{
    friend class ZMasterFile;

public:
    typedef ZRandomFile                   _Base   ;
    ZIndexControlBlock                    ZICB;
/**
 * @brief ZIndexFile first constructor version : the common one.
 *          It sets up the ZIndexFile parameters AND rebuild the index if pAutoRebuild is set to true (default value is true).
 *          If ZIX rebuild is done, there must not be any ZS_DUPLICATEKEY during the rebuild if ZIndexFile duplicates option is set to ZST_NODUPLICATES.
 *          If so, abort() will be called (after having appropriately destroyed objects on father ZAM side).
 *
 *      - Nota Bene: if you want to create a ZIX without knowing in advance if there will be duplicates on key or not : you should set pDuplicates to ZST_DUPLICATES.
 *      Do not create ZIndexFilees with rejected key values : you will have holes into your index tables that will induce an impredictable result as soon as you will update any part of the hierarchy (ZAM and other dependant ZIXs).
 *
 * @param pFather ZAM to which the ZIX refers
 * @param pDuplicates ZSort_Type defining how duplicates will be managed. (set to ZST_DUPLICATES by default)
 */
    ZIndexFile  (ZMasterFile *pFather,ZSort_Type pDuplicates,int pCount,...);


    ~ZIndexFile() {if (ZMFFather!=nullptr)
                                 _deregister();}

    void defineKeyFields (long pCount,...);

    using _Base::size   ;

#ifdef __COMMENT__
//!--------------operators-------------------
//!
    /**
  * @brief operator [] delivers a reference to ZAM rank corresponding to ZIX index rank pIndex
  * @param pIndex
  * @return
  */
 ZDataBuf& operator [] (const long pIndex)
            {
            if ((Tab[pIndex].ZAMIdx>ZMFFather->lastIdx())||(Tab[pIndex].ZAMIdx<0))
                        {
                        fprintf(stderr,"ZIX::operator[]-F-INVINDEX for row index %ld ZAMIdx is corrupted or invalid value <%ld>. Index Rebuild will be done.\n"
                                " If this situation occurs frequently, this may be due to an internal bug.\n",
                                pIndex,
                                Tab[pIndex].ZAMIdx);
                        _rebuild();
                        }
//            long wZAMIdx=this->Tab[pIndex].ZAMIdx;
            return (ZMFFather->Tab[this->Tab[pIndex].ZAMIdx]);
            }


//! ----------end operators------------------------
//!
//!

    ZStatus _rebuild(void);//! _rebuild creates the ZIndexFile from ZAM using existing ZAM rows.
    void _register (void);

    void _deregister (void) {
                            ZStatus wSt=ZMFFather->_deregister(ZIndexFileId);
                            if (wSt!=ZS_SUCCESS)
                                    fprintf(stderr,"ZIndexFile::_deregister-F-ERR Fatal error while deregistering ZIndexFile status %s",
                                            decode_ZStatus(wSt));
                            return; };


//!
//!---------------ZIndexFile methods for ZJoins----------------
//!
//!  equivalent to ZIX::operator []
//!
 /**
 * @brief getZAMRow  returns a const reference to ZAM row corresponding to ZIX index order of pIdx rank
 * @param pIdx
 * @return
 */
const _Type& getZAMRow(long pIdx) {
    return(ZMFFather->Tab[this->Tab[pIdx].ZAMIdx]);}

long getZAMIdx(long pIdx)   { return(this->Tab[pIdx].ZAMIdx);}

//!
//!------------ journaling methods & data-------------
//!
//!  No journaling for ZIX : fully managed by ZAM
//!


//!
//!--------------- ZIndexFile update methods------------------------
//!
private:        // not allowed for public access (Only ZMAFather may access this)
    ZStatus _erase (size_t pZAMIdx) ;                       // called by ZMA
    ZStatus _replace (_Type &pElement, size_t pZAMIdx) ;    // called by ZMA


    ZStatus _add    (ZDataBuffer &pElement, ZOp ZAMOp, size_t pZAMIdx);     // called by ZMA replaces _insert method
    ZStatus _addOld    (_Type* pElement,ZOp ZAMOp, size_t pZAMIdx);     // called by ZMA replaces _insert method

    ZStatus _remove (_KType &pKey);         // internal routine
    ZStatus _removeAll (_KType &pKey);
    ZStatus _seekAll         (_KType pKey, ZMulti_Result &pMRes) ;
    ZStatus _seekAllPartial  (_KType pKey,size_t pSize, ZMulti_Result &pMRes);

    ZResult _seek (const _KType &pKey)  {_KType wKey; memmove(&wKey,&pKey,sizeof(_KType));return(seek((wKey)));}
public:
//! reserved to ZJoinDyn
//!
    ZStatus _seekAllZJoinDyn(_KType &pKey, ZMulti_Result *pMRes);
    ZResult _seek            (ZDataBuffer &pKey) ;                       // internal but must be used by ZJoinDyn

//! external update methods
//!
//!
//!

    ZStatus add(_Type &pElement);           //!< @brief ZIndexFile::add user method that calls ZAM::insert method

    ZStatus remove (_KType &pKey);          //!< @brief ZIndexFile::remove user method : erase a ZAM Row corresponding to _Key searched in ZIndexFile

    ZStatus remove (const _KType &pKey)      //!< @brief ZIndexFile::remove user method : erase a ZAM Row corresponding to _Key (a constant) searched in ZIndexFile
                    {
                    _KType wKey;
                    memmove(&wKey,&pKey,sizeof(_KType));
                    return(_remove(wKey));
                    }

    ZStatus removeAll (const _KType &pKey);       //! @brief ZIndexFile::removeAll user method : erase all ZAM Rows corresponding to _Key searched in ZIndexFile

//! external search methods


//! search for indexes
//!
    ZResult seek            (_KType &pKey) ;                                    // available to user
    ZResult seek            (const _KType &pKey)  {_KType wKey; memmove(&wKey,&pKey,sizeof(_KType));return(seek((wKey)));}

 //   ZStatus seekAll         (_KType pKey, ZMulti_Result *pMRes) ;              // available to user
    ZStatus seekAll         (_KType pKey, ZMulti_Result &pMRes) ;              // available to user

    ZStatus seekAll         (_KType pKey, ZAMulti_Result &pMRes) ;              // available to user : Using ZAMulti_Result


    ZResult _seekFirstPartial  (_KType pKey, size_t pSize);  // available to user : returns ZIX index
    ZResult seekFirstPartial  (_KType pKey,size_t pSize);  // available to user : returns ZAM index

//    ZStatus seekAllPartial  (_KType pKey,size_t pSize, ZMulti_Result *pMRes);  // available to user
    ZStatus seekAllPartial  (_KType pKey,size_t pSize, ZMulti_Result &pMRes);  // available to user

     ZStatus seekAllPartial  (_KType &pKey,size_t pSize, ZAMulti_Result &pMRes);  // available to user : Using ZAMulti_Result

    ZResult seekFirst         (_KType &pKey) ;
    ZResult _seekFirst         (_KType &pKey) ;

//! search for ZAM row content (Record)
//!

    const _Type *seekRecord (_KType pKey,_Type &pRecord,ZResult &pRes) ;                       //! returns the first ZAM Row content corresponding to full Key content within mentionned index
    const _Type *seekPartialRecord (_KType pKey,size_t pSize,_Type &pRecord,ZResult &pRes);    //! returns the first ZAM Row content corresponding to partial key


//! various methods
//!

//char *  printKeyValue(long pIdx,char *pBuf);

long _zjdregister (ZJDescriptor_struct* pZJD)
     { ZJDDescriptor.push(pZJD);
       FConnectedJoin=true;
       return(ZJDDescriptor.lastIdx());}

void _zjdderegister(long pZJoinID)
     {ZJDDescriptor.erase(pZJoinID);
      if (ZJDDescriptor.isEmpty())
                 FConnectedJoin=false;
     return; }

void _reset(int pArg) {

    for (long wi=0;wi<ZJDDescriptor.size();wi++)            //! resetting (or clear) all Dynamic Joins linked to this ZIX
                        ZJDDescriptor[wi]->_reset(pArg);
    if (pArg)                                               //! then properly resetting or clearing itself
                                _Base::clear();
                            else
                                _Base::reset();
                        return; }
#endif // __COMMENT__
private:

   long                         ZIndexFileId;
   ZMasterFile*                 ZMFFather;
#ifdef __COMMENT__

//! ZJoinDyn management section
//!
   bool FConnectedJoin = false;
   ZArray<ZJDescriptor_struct*>     ZJDDescriptor;  //! registering log for all ZJoinDyn connecting to this ZIX

   ZSort_Type ZIDuplicates;
   ZStatus PersistentStatus;


//! -------------end ZJoinDyn management section
//!
#endif //__COMMENT__

   struct ZData_Type_struct _keyDType ;

#ifdef __USE_ZTHREAD__
    ZMutex _Mtx;
#endif
    ZStatus _addRebuild(ZDataBuffer &pElement, size_t pZAMIdx);
    ZStatus _rebuildReverse(void);
    ZStatus _updateZReverse(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pIdx);
    long _lookupZAMIdx (size_t pZAMIdx);

    ZStatus _alignZIndexFile(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pZIXIdx);


};// class ZIndexFile




//!=================================ZMasterFile===============================
//!
//!





/**
 * @brief The ZMasterFile class
 *
 *
 *
 */
class ZMasterFile : protected ZRandomFile  //! cannot publicly access to ZRandomFile data & methods
{
typedef ZRandomFile _Base ;
public:
    ZMasterControlBlock ZMCB ;

    ZMasterFile(bool pHistory=true) ;
    ZMasterFile(uriString pURI) : ZRandomFile(pURI) {};

   virtual ~ZMasterFile(void)
        {
        while (ZIFDescriptor.size()>0)
                            _deregister(ZIFDescriptor.lastIdx());

        if (ZJournaling!=nullptr)
                    {
                    delete ZJournaling;
                    ZJournaling = nullptr;
                    }
        if (ZHistory!=nullptr)
                     {
                    delete ZHistory;
                    ZHistory=nullptr;
                     }
        }

//! what we can externally use of ZMasterFile methods (included from ZIndexFile class)
//!
//    using _Base::Tab ;
    using _Base::size ;
    using _Base::lastIdx ;
    using _Base::last ;
    using _Base::zremoveR ;

//    using _Base::getAllocation ;
    using _Base::getURI ;

    using _Base::setPath;
    using _Base::setJournaling;
    using _Base::setHistory;
    using _Base::setHighwaterMarking;
    using _Base::setAutocommit;
    using _Base::setGrabFreeSpace;


    ZStatus zcreate (const char *pFilename,
                     long pAllocatedBlocks,
                     long pBlockExtentQuota,
                     long pRecordTargetSize,
                     zsize_type pAllocatedSize,
                     zsize_type pExtentSizeQuota,
                     bool pHistory,
                     bool pAutocommit,
                     bool pJournaling,
                     bool pHighwaterMarking,
                     bool pGrabFreeSpace);

    void _addIndexField (ZArray<ZIFKeyDescriptor>& pZIFKey,codeString& pName, size_t pOffset, size_t pLength);

    ZStatus _createIndex (ZMasterControlBlock &pMCB,
                            codeString pName,
                            ZST_DUPLICATES pDuplicates, pCount,...);
    ZStatus _openIndex (  codeString pName,
                          ZST_DUPLICATES pDuplicates);

    /**
     * @brief getRef returns a reference to ZAM row corresponding to pIndex
     * @param pIndex
     * @return
     */
//    const _Type& getRef (long pIndex) {return(_Base::Data.Tab[pIndex]);}
    /**
     * @brief getRef returns a COPY of the content of ZAM row corresponding to pIndex
     * @param pIndex
     * @return
     */

 //   _Type get (long pIndex) {return(_Base::Data.Tab[pIndex]);}
/** -------------------ZMasterFile operators-------------------------*/
    /**
     *  Operator = cannot be used : replace method will be used instead of it
     */

    /**
      * @brief operator [] Returns a reference to a (non modifiable) const_Type corresponding to ZAM rank pointed by pIndex,
      *                    while base ZArray operator  [] returns a reference to the rank.
      *                    ZAM rank cannot be modified (because of potential Keys modification): use replace() method instead.
      * @param pIndex
      * @return
      */
//! not usable in a multi threading context
//!
     ZDataBuffer&  operator [] (const long pIndex) { zget(CurrentRecord,pIndex,0L);return CurrentRecord;}   // resulting _Type cannot be modified : use replace() method instead


friend ZDataBuffer& operator << (ZDataBuffer &pRes,ZMasterFile &pZMF)
     {
     pZMF.zremoveR(pZMF.lastIdx(),pRes);
     return(pRes);
     }
friend ZStatus operator << (ZMasterFile &pZMF,const ZDataBuffer& pInput)
     {
     return (pZMF.push(pInput));
     }

    const long& operator -- (void)
     {
     return(pop() );
     }



//!------------------end ZAM operators------------------------



//! LATER for join implementation
/*
void*  askZIndexFile(size_t pOffset,size_t plength)
{
    return( ZIFDescriptor[0]->_askMe());
//    return (nullptr);

}// ask if there is an ZIndexFile key defined on ZAM at offset for length
                                                 // returns nullptr if no ZIndexFile defined, or pointer to the appropriate ZIndexFile instance if any

*/
//! LATEREND

//!
//!-------------------base methods overload-----------------
//!
    ZStatus push(const ZDataBuffer &pElement) ;
    ZStatus push_front (const ZDataBuffer &pElement) {return(zinsert (pElement,0));};

    long move (size_t pDest, size_t pOrig,size_t pNumber=1)     _METHOD_NOT_ALLOWED__  // forbidden  (because of possible Index corruption)
    long swap (size_t pDest, size_t pOrig,  size_t pNumber=1)   _METHOD_NOT_ALLOWED__  // forbidden  (because of possible Index corruption)

    ZStatus insert (const ZDataBuffer& pElement, const size_t pZAMIdx) ;                 // ZAM transmitted an insert operation: it shifts all ZAM indexes > pZAMIdx

    ZStatus erase(const size_t pIdx) ;
    ZStatus erase(const size_t pIdx,const size_t pNumber ) ;

    ZStatus replace(ZDataBuffer &pElement,const size_t pIdx);

    long pop(void)     ;
    long pop_front(void)             ;

    ZDataBuffer &popR(void)                  _METHOD_NOT_ALLOWED__
    ZDataBuffer &popR_front(void)            _METHOD_NOT_ALLOWED__
    ZDataBuffer &popRP(ZDataBuffer &pReturn) ;


    void reset() {        for (long wi=0;wi<ZIFDescriptor.size();wi++)
                                            ZIFDescriptor[wi]->_reset(0); }
    void clear() {        for (long wi=0;wi<ZIFDescriptor.size();wi++)
                                            ZIFDescriptor[wi]->_reset(1); }
//!
//!----------------ZAM Journaling methods---------------------------
//!



ZStatus
    declareCommitUserFunction (CUF pCommitUserFunction,void* pCommitUserArguments)
    {
        if (!ZJournaling->ZFJournaling)
                        return(ZS_INVOP);           // journaling must be active
        CommitUserFunction = pCommitUserFunction;
        CommitUserArguments = pCommitUserArguments;
        return (ZS_SUCCESS);
    };
ZStatus setJournaling(ZJOption pZJournalingOption=ZJAutoCommit) ;

void setJournalingQuota (size_t pAlloc,size_t pQuota=_cst_zmf_journaling_reallocquota);

ZStatus commit (void *pCommitArguments=nullptr, ZAMMark_type pZAMMark=-1);

ZStatus _rollback (ZAMMark_type pZAMMark);

ZStatus rollback (ZAMMark_type pZAMMark=-1) {return(_rollback(pZAMMark));}

ZStatus rollback (void) //! @brief rollback () rolls back all journaling events that have not been committed yet
            {return _rollback( - ZJournaling->lastIdx()); }

ZStatus _rollback_indexes (long pZAMIdx=-1);



ZStatus journalize(const long pZIndexFileId,
                   const ZOp pOperation,
                   const ZAMState_type pState,
                   void *pData,
                   const long pDataSize,
                   const long pIndex,
                   const ZStatus pOperationStatus)
{
    return(ZJournaling->journalize(pZIndexFileId,
                                        pOperation,
                                         pState,
                                        pData,
                                        pDataSize,
                                        pIndex,
                                        pOperationStatus));

};



ZStatus
_ZIFHistorizeError(const long pZIndexFileId,
                   const ZOp pOperation,
                   const ZAMState_type pState,
                   void *pData,
                   const long pDataSize,
                   const long pIndex,
                   const ZStatus pOperationStatus);


void printJournal(ZAMMark_type pZAMMark=-1,bool pRowDump=false,FILE *pOutput=stdout)
        {
        return(ZJournaling->printJournal(pZAMMark,pRowDump,pOutput));
        };

void printHistory(ZAMMark_type pZAMMark=-1,bool pRowDump=false,FILE *pOutput=stdout)
        {
        if (ZHistoryOn)
                return(ZHistory->printHistory(pZAMMark,pRowDump,pOutput));
            else
                fprintf(stderr,"ZHistory-E-HISTOFF History is set to OFF. Please set history on to have an historized even list\n");
        };


//!----------------End ZAM journaling methods----------------------------

//!
//!--------------------ZIndexFile management stuff-------------------------------------
//!
/**
 * @brief _register registers a new index for the ZMasterFile ZArray. Giving back the ZIndexFileId.
 * @param pDescriptor descriptor for index to add.
 * @return ZStatus : ZS_SUCCESS if successfull
 */
ssize_t _register(ZIFDescriptor_struct* pDescriptor)
{
    ZIFDescriptor.push(pDescriptor);
    return (ZIFDescriptor.lastIdx());
};


/**
 * @brief _deregister  suppresses the ZIndexFile pointed by ZIndexFileId from the Index base
 * @param pZIndexFileId index to suppress.
 * @return ZStatus : ZS_SUCCESS if successfull. ZS_NOTFOUND if ZIndexFileId is invalid.
 */
ZStatus _deregister(long pZIndexFileId)
{
    if(ZIFDescriptor.erase(pZIndexFileId)<0)
                {
                    fprintf (stderr,"ZMasterFile::_deregister-S-ZIndexFileIDNOTFOUND not found ZIndexFileId within registered ZIndexFile table\n");
                    return(ZS_NOTFOUND);
                }
    return (ZS_SUCCESS);
};

void setDescriptorQuota (size_t pAlloc,size_t pQuota=_cst_zmf_descriptor_reallocquota)
    {
    ZIFDescriptor.setAllocation(pAlloc);
    ZIFDescriptor.setQuota(pQuota);
    return;
    };
//!--------------------end ZIndexFile management stuff-------------------------------------
//!

//!------------- History management------------------------------
void setHistoryOn (size_t pAlloc=_cst_zmf_history_allocation,size_t pReallocQuota=_cst_zmf_history_reallocquota)
{
#ifdef __USE_ZTHREAD__
    _Mtx.lock();
#endif
    if (ZHistory==nullptr)
                {
                ZHistory=new ZCHistory(pAlloc,pReallocQuota);
                }
     ZHistoryOn=true;
     ZHistoryZIXOn=true;
     if (ZJournaling->_getOption()==ZJDisable)
                                  {
                                 ZJournaling->setJournaling(ZJAutoCommit );
#if __DEBUG_LEVEL__ > __WARNINGREPORT__
                                 fprintf(stderr,"ZAM::setJournaling-W-JRNLSETAUTO Setting history on : journaling is set to ZJAutocommit");
#endif
                                  }

#ifdef __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    return;
};


void setHistoryOff (void)
{
#ifdef __USE_ZTHREAD__
    ZAMlock();
#endif
     ZHistoryOn=false;
     ZHistoryZIXOn=false;
#ifdef __USE_ZTHREAD__
    ZMFunlock();
#endif
    return;
};

#ifdef __USE_ZTHREAD__
void ZAMlock(void)
{
        if (ZJournaling->ZFJournaling)
                    {
                    ZJournaling->_Mtx.lock();
                    }
       if (ZHistory!=nullptr)
                    ZHistory->_Mtx.lock();

        return(_Mtx.lock());
};


void ZMFunlock(void)
{
    if (ZJournaling->ZFJournaling)
                {
                ZJournaling->_Mtx.unlock();
                }
   if (ZHistory!=nullptr)
                ZHistory->_Mtx.unlock();

        return(_Mtx.unlock());
};
#endif





bool getHistoryStatus (void)
{
    return(ZHistoryOn);
}

bool getJournalingStatus (void)
{
    return(ZJournaling->ZFJournaling);
}

//!
//! =======================Data fields======================================
//!


private:
    using _Base::operator = ;
    ZCJournaling *  ZJournaling=nullptr;            //! must be public because accessed by ZIX
    ZCHistory    *  ZHistory = nullptr;             //! idem


//! call back vectors

    ZArray<ZIFCallbackVector_struct*>        ZIFDescriptor ;
    ZJournalAction_struct                   ZJournalAction;

    ZCommitStatus (*CommitUserFunction ) (const ZDataBuffer&,ZOp,void *)     ;
    void *  CommitUserArguments;

#ifdef __USE_ZTHREAD__
    ZMutex    _Mtx;
#endif
protected:
//! permanent protected data
//!
    CZMCB ZMCB;  //! Master Control Block : all data is there

}; //--------------------end class ZMasterFile-------------------------------







//! =====================end ZMasterFile===============================



//!
//!----------------ZIndexFile expanded methods---------------------------------------
//!


void
ZIndexFile::_register (void)
{
/**
  * Nota Bene: ZIXDescriptor must remain WITHIN ZIndexFile object and used from outside when needed thru pointer  (otherwise: SIGSEGV exception thrown).
  */
    ZIFDescriptor._add      = std::bind(&ZIndexFile::_add, this,_1,_2,_3);
    ZIFDescriptor._erase    = std::bind(&ZIndexFile::_erase, this,_1);
    ZIFDescriptor._replace  = std::bind(&ZIndexFile::_replace, this,_1,_2);
    ZIFDescriptor._askMe    = std::bind(&ZIndexFile::_askMe, this);
    ZIFDescriptor._ask      = std::bind(&ZIndexFile::_ask, this,_1,_2,_3);
    ZIFDescriptor._reset    = std::bind(&ZIndexFile::_reset, this,_1);
    ZIFDescriptor.CommitUserFunction =nullptr;
    ZIFDescriptor.CommitArguments =nullptr;

    ZIndexFileId    =   ZMFFather->_register(&ZIFDescriptor);

    if (ZIndexFileId<0)
            {
            fprintf(stderr,"ZIndexFile::_register-F-IVZIndexFileIDX Invalid ZIndexFileIdx value returned by ZAM.\n");
            ZIXAbort();
            }

    return;
}//register


template < class _Type,class _Bulk,class _KType,class _KCompare>
ZStatus
ZIndexFile:: _rebuild(void)
{
ZStatus wSt=ZS_SUCCESS;

    this->setQuota(ZMFFather->getQuota());
    this->setAllocation(ZMFFather->getAllocation());

    this->reset();

#ifdef __USE_ZTHREAD__
    ZMFFather->ZAMlock(); // during an Index rebuild lock ZAM whatever ZThread mode is if using ZThreads
#endif

    for (size_t wi=0;wi<ZMFFather->size();wi++)
                    {
                    wSt = _addRebuild(ZMFFather->Tab[wi],wi);
#ifdef __USE_ZTHREAD__
                    ZMFFather->ZMFunlock();
#endif
                    if(wSt!=ZS_SUCCESS)
                            {
#if __DEBUG_LEVEL__ > __SEVEREREPORT__
                            _KType wKey ;
                            char wBuf[100];
                            memmove(&wKey,&((_Bulk &)ZMFFather->Tab[wi])._Key ,sizeof(_KType));

                            fprintf(stderr,"ZIX::_rebuild-S-ADDBADSTAT Severe error while building or rebuilding ZIndexFile id <%ld> at ZAM row id <%ld>.\n"
                                    "status is <%s> Key content is <%s>\n",
                                    ZIndexFileId,
                                    wi,
                                    decode_ZStatus(wSt),
                                    _printFieldValue<_KType>(wKey,wBuf));
#endif
                            return(wSt);
                            }

                     }
 //   _rebuildReverse();
    return(wSt);

}//ZIX::_rebuild

//!
//!-------------ZIndexFile overloaded base methods----------------------------------
//!


//!
//!-------------ZIndexFile update----------------------------------
//!



//!
//!-------------journaling----------------------------------
//!

//!
//!-------------ZIndexFile expanded methods----------------------------------
//!
//!
//!


ZStatus ZIndexFile::_addRebuild(ZDataBuffer &pElement,size_t pZAMIdx)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;
/**
  * get with seekGeneric the ZIndexFile row position to insert
  *
  *  1-insert accordingly (push_front, push , insert)
  *
  *  2- shift all ZAM indexes references according pZAMIdx value
  *
  */
    wZI.ZAMIdx = pZAMIdx ;
    wZI.State = ZAMInserted ;
    size_t wZI_Idx;
    memmove(&wZI._Key,&((_Bulk &)pElement)._Key ,sizeof(_KType));

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (wZI._Key,*this);
    wZI_Idx=wRes.ZIdx;
    switch (wRes.ZSt)
            {
            case (ZS_OUTBOUNDLOW):
                {
                if (_Base::push_front(wZI)<0)
                            return (ZS_OUTBOUNDHIGH);
                wZI_Idx=0;
                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                if (_Base::push(wZI)<0)
                            return (ZS_OUTBOUNDHIGH);
                wZI_Idx=this->lastIdx();
                break;
                }
            case (ZS_NOTFOUND):
                {
                if (_Base::insert(wZI,wRes.ZIdx)<0)     // insert at position returned by seekGeneric
                            {
                            return (ZS_NOTFOUND);
                            }
                wZI_Idx=wRes.ZIdx;
                break;
                }
            case (ZS_FOUND):
                {
                if (ZIDuplicates==ZST_NODUPLICATES)
                            {
                            return (ZS_DUPVIOLATION);
                            }
                if (_Base::insert(wZI,wRes.ZIdx)<0)
//                            return ((ZStatus)(ZS_ERROR|ZS_SECONDARY));
                            return (ZS_OUTBOUND);
                wZI_Idx=wRes.ZIdx;
                break;
                }
            default :
                {
//                return((ZStatus)(ZS_INVOP|ZS_SECONDARY));
                return(ZS_INVOP);
                }
            }// switch


    return(ZS_SUCCESS);
}//_addRebuild



ZStatus ZIndexFile::_erase (size_t pZAMIdx)
{
    ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.lock();
#endif
    long wZIXIdx = _lookupZAMIdx(pZAMIdx);

    if (wZIXIdx<0)
                    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                    _Mtx.unlock();
#endif
                    return(ZS_OUTBOUNDHIGH);      // error is managed at ZAM level
                    }
    if ((wSt=_alignZIndexFile(ZO_Erase,ZO_Erase,pZAMIdx,wZIXIdx))!=ZS_SUCCESS)
                        {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.unlock();
#endif
                        return(wSt);      // error is managed at ZAM level
                        }

    if (_Base::erase(wZIXIdx)<0)
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return(ZS_OUTBOUNDHIGH);      // error is managed at ZAM level
                            }
    if (FConnectedJoin)
    {
    for (long wi=0; wi< ZJDDescriptor.size();wi++)
                    {
                    ZJDDescriptor[wi]->_erase(wZIXIdx);
                    }
     }
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    return(ZS_SUCCESS);
}//ZIX::_erase


long ZIndexFile::_lookupZAMIdx (size_t pZAMIdx)
{
    for (long wi=0;wi<this->size();wi++)
                    {
                    if (pZAMIdx==this->Tab[wi].ZAMIdx)
                                return (wi);
                    }
    return(-1);
}


ZStatus ZIndexFile::_replace (ZDataBuffer &pElement,size_t pZAMIdx)
{

    long wZIXIdx = _lookupZAMIdx(pZAMIdx);
    if (wZIXIdx<0)
            {
            fprintf (stderr,"ZIX::_replace-F-ZIXCORR Fatal error Index id <%ld> corrupted  at ZAM rank <%ld> \n",
                     ZIndexFileId, pZAMIdx);
            ZIXAbort();
            }

    //!
    //! if key value remains identical : no need of change
    //!
    //!
    _KType *wKey =&((_Bulk&)pElement)._Key;
    if (_KCompare::_compare(wKey,                           // get Key from _Bulk recasted from _Type
                            &this->Tab[ wZIXIdx]._Key       // get key value corresponding to reverse index search
                            )  ==0 )                        //could be equal

                               return(ZS_SUCCESS);                 // if so, return everything is done.

ZStatus wSt;

// if not : erase corresponding ZIX using _erase method and insert using _add method

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif

    ZIndexFile_struct<_KType> wZIX_bck ;
    memmove(&wZIX_bck,&this->Tab[wZIXIdx], sizeof(wZIX_bck));
    long w1=_Base::erase(wZIXIdx);
/*    wSt=_erase (pZAMIdx);
    if (wSt!=ZS_SUCCESS)
*/
    if (w1<0)
            {
                        wSt=ZS_OUTBOUND;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.unlock();
#endif
                        return (wSt);
                }
    wSt=_add(&pElement,ZO_Insert, pZAMIdx);
    if (wSt!=ZS_SUCCESS)
                {           // manual restore value
                w1=_Base::insert(wZIX_bck,wZIXIdx);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                _Mtx.unlock();
#endif
                if (w1<0)
                        {
                        char wBuf[200];
                        fprintf(stderr,"ZIX::_replace-F-RBCKOUTBOUND  Fatal error : out of boundaries while rolling back ZIX rank (_Base::insert) ZIX id <%ld>.\n"
                                       "Index rank <%ld>  key value <%s>\n",
                                ZIndexFileId,
                                wZIXIdx,
                                _printFieldValue<_KType>(wZIX_bck._Key,wBuf));
                        ZIXAbort();
                        }
                return (wSt);
                }

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.unlock();
#endif
    return (wSt);
}//ZIX::_replace



ZStatus ZIndexFile::_add(ZDataBuffer &pElement, ZOp ZAMOp, size_t pZAMIdx)
{
long ZJoinIndex;
ZResult wRes;
ZOp wZIXOp;
ZIndexFile_struct<_KType> wZI ;
/**
  * get with seekGeneric the ZIndexFile row position to insert
  *
  *  1-insert accordingly (push_front, push , insert)
  *
  *  2- shift all ZAM indexes references according pZAMIdx value
  *
  */
    wZI.ZAMIdx = pZAMIdx ;
    wZI.State = ZAMInserted ;
    memmove(&wZI._Key,&((_Bulk*)pElement)->_Key ,sizeof(_KType));

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (wZI._Key,*this);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
#endif
    switch (wRes.ZSt)
            {
            case (ZS_OUTBOUNDLOW):
                {
                wZIXOp=ZO_Push_front ;
                ZJoinIndex=0;
                if ((wRes.ZSt=_alignZIndexFile(ZAMOp,wZIXOp,pZAMIdx,wRes.ZIdx))!=ZS_SUCCESS)
                                                                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                                                            _Mtx.unlock();
#endif
                                                                            return(wRes.ZSt);//! possible ZS_INVOP
                                                                            }
                _Base::push_front(wZI) ; // push_front has no error (or memory violation SIGSEGV)

                break;
                }
            case (ZS_OUTBOUNDHIGH):
                {
                wZIXOp=ZO_Push ;
                ZJoinIndex=this->size();
                if ((wRes.ZSt=_alignZIndexFile(ZAMOp,wZIXOp,pZAMIdx,wRes.ZIdx))!= ZS_SUCCESS)
                                                {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                                _Mtx.unlock();
#endif
                                                return(wRes.ZSt);//! possible ZS_INVOP
                                                }
                _Base::push(wZI);  // ZArray::push has no error case except internal memory violation SIGSEGV
                break;
                }
            case (ZS_NOTFOUND):
                {
                wZIXOp=ZO_Insert ;
                if ((wRes.ZSt=_alignZIndexFile(ZAMOp,wZIXOp,pZAMIdx,wRes.ZIdx))!= ZS_SUCCESS)
                                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                            _Mtx.unlock();
#endif
                                            return(wRes.ZSt);//! possible ZS_INVOP
                                            }
                if (_Base::insert(wZI,wRes.ZIdx)<0)     // insert at position returned by seekGeneric
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return(ZS_OUTBOUNDHIGH); // error is managed at ZAM level
                            }
                ZJoinIndex=wRes.ZIdx;
                break;
                }
            case (ZS_FOUND):
                {
                if (ZIDuplicates==ZST_NODUPLICATES)
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                            _Mtx.unlock();
#endif
                            return (ZS_DUPLICATEKEY);
                            }
                wZIXOp=ZO_Insert ;
                if ((wRes.ZSt=_alignZIndexFile(ZAMOp,wZIXOp,pZAMIdx,wRes.ZIdx))!= ZS_SUCCESS)
                                                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                                            _Mtx.unlock();
#endif
                                                            return(wRes.ZSt);//! possible ZS_INVOP
                                                            }
                if (_Base::insert(wZI,wRes.ZIdx)<0)
                            {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                             _Mtx.unlock();
#endif
                            return(ZS_OUTBOUNDHIGH); // error is managed at ZAM level
                            }
                ZJoinIndex=wRes.ZIdx;
                break;
                }
            default :
                {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                 _Mtx.unlock();
#endif
                return(ZS_INVOP);
                }
            }// switch


    if (FConnectedJoin)
    {
    for (long wi=0; wi< ZJDDescriptor.size();wi++)
                    {
                    ZJDDescriptor[wi]->_add(ZJoinIndex);  //! gives the pointer to the key value within the ZArray
                    }
     }


#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    return(ZS_SUCCESS);

}//ZIX::_add



ZStatus
ZIndexFile::_alignZIndexFile(ZOp pZAMOp,ZOp pZIXOp, size_t pZAMIdx, size_t pZIXIdx)
{
//! NB: ZO_Replace has no impact on ZReverse : ZAM index remains the same NO NO NO  Replace may impact on Key : _erase then _add

    switch (pZAMOp)
       {
        case (ZO_Push):
            {
            break;
            }
    case (ZO_Push_front):
            {
            for (long wi=0;wi<this->size();wi++)
                            this->Tab[wi].ZAMIdx++;
            break;
            }
     case (ZO_Add):
     case (ZO_Insert):
            {
        for (long wi=0;wi<this->size();wi++)
                    {
                    if (this->Tab[wi].ZAMIdx>=pZAMIdx)
                                this->Tab[wi].ZAMIdx++;
                    }
            }
    case (ZO_Pop):
            {
            break;
            }
    case (ZO_Pop_front):
            {
            for (long wi=0;wi<this->size();wi++)
                            this->Tab[wi].ZAMIdx--;
            break;
            }
    case (ZO_Erase):
           {
       for (long wi=0;wi<this->size();wi++)
                   {
                   if (this->Tab[wi].ZAMIdx>=pZAMIdx)
                                    this->Tab[wi].ZAMIdx--;
                   }
       break;
           }
    default:
            {
            return(ZS_INVOP);
            }
       }//switch
    return(ZS_SUCCESS);
} //_alignZIndexFile



//! ------------external ZIndexFile expanded methods----------------------------
//!
//!         These methods are at disposal of user
//!
//!
//!

//! @brief add external routine to add a row within ZAM father using ZIndexFile : it calls ZAM::push method
//!
ZStatus ZIndexFile::add(_Type &pElement)
{
    return (ZMFFather->push(pElement));
} // add



ZStatus ZIndexFile::remove(_KType &pKey)
{
ZResult wRes;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFFather->ZAMlock();
#endif

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    if (wRes.ZSt!= ZS_FOUND)
               {
//        wRes.ZSt=(ZStatus)(wRes.ZSt | ZS_SECONDARY);
        if (ZMFFather->getHistoryStatus())
                {
        if((wSt=ZMFFather->ZJournaling->journalize(ZIndexFileId,             //  index operation
                                ZO_Erase,
                                ZAMErrDelete,
                                nullptr,  // no row to store
                                -1,       // no size of row
                                wRes.ZIdx,
                                wRes.ZSt))!=ZS_SUCCESS)
                                {
                                fprintf(stderr,"ZMasterFile::erase-F-CANTJOURNALIZE Fatal error while journalizing row operation. Status is %s \n"
                                        "Aborting ",
                                        decode_ZStatus(wSt));    // need to be updated later
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                ZMFFather->ZMFunlock();
#endif
                               ZIXAbort();
                                } // journalize

                ZMFFather->ZHistory->_historizeErroredLastJournalEvent(
                                                        ZMFFather->ZJournaling,
                                                        wRes.ZSt,
                                                        ZAMErrDelete ,
                                                        ZIndexFileId,
                                                        -1);
                     } // if  HistoryOn
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                ZMFFather->ZMFunlock();
#endif
               return (wRes.ZSt);
               } // if  error

    wRes.ZSt=ZMFFather->erase(this->Tab[wRes.ZIdx].ZAMIdx);


#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFFather->ZMFunlock();
#endif
    return(wRes.ZSt);
}// ZIX::remove


ZStatus ZIndexFile::_remove(_KType &pKey)
{
ZResult wRes;
ZStatus wSt;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFFather->ZAMlock();
#endif

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = wRes.ZSt;
    if (wRes.ZSt!= ZS_FOUND)
               {
//        wRes.ZSt=(ZStatus)(wRes.ZSt | ZS_SECONDARY);
        if (ZMFFather->getHistoryStatus())
                {
        if((wSt=ZMFFather->_ZIXHistorizeError(ZIndexFileId,             //  index operation
                                ZO_Erase,
                                ZAMErrDelete,
                                nullptr,  // no row to store
                                -1,       // no size of row
                                wRes.ZIdx,
                                wRes.ZSt))!=ZS_SUCCESS)
                                {
                                fprintf(stderr,"ZMasterFile::erase-F-CANTJOURNALIZE Fatal error while journalizing row operation. Status is %s \n"
                                        " Please check if Journaling and Historization are correctly set to ON. Aborting... ",
                                        decode_ZStatus(wSt));    // need to be updated later
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                                ZMFFather->ZMFunlock();
#endif
                                ZIXAbort();;
                                } // journalize

                     } // if  HistoryOn
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                ZMFFather->ZMFunlock();
#endif
               return (wRes.ZSt);
               } // if  error

    wRes.ZSt=ZMFFather->erase(this->Tab[wRes.ZIdx].ZAMIdx);


#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    ZMFFather->ZMFunlock();
#endif
    return(wRes.ZSt);
}// ZIX::_remove



/**
 * @brief ZIndexFile::removeAll
 *
 *                  Secondary index adjustement is treated from ZAM during erase processing
 *                  see ZIndexFile::_erase() method
 * @param pKey
 * @return
 */
ZStatus ZIndexFile::removeAll(const _KType &pKey)
{
    ZAMulti_Result wMRes;
    ZStatus wSt;
    #if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.lock();
    #endif
        _seekAllGenericZA<_KType,ZIndexFile_struct<_KType>,_KCompare> ((_KType&)pKey,*this,wMRes);
        PersistentStatus = wMRes.ZSt;
        if (wMRes.ZSt!= ZS_FOUND)
                   {
//                   wMRes.ZSt=_setSecondary(wMRes.ZSt);
    #if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                    _Mtx.unlock();
    #endif
                   return (wMRes.ZSt);
                   }
        while (wMRes.size()>0)
                {
                if ((wSt=ZMFFather->erase(this->Tab[wMRes.last()].ZAMIdx))!=ZS_SUCCESS)
                        {
    #if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                        _Mtx.unlock();
    #endif
                        return(wSt);
                        }
                  for (long wi=0;wi<wMRes.size();wi++)
                                    if (wMRes[wi]>wMRes.last())
                                                    wMRes[wi]--;
                  wMRes.pop();
                }// while
    #if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mtx.unlock();
    #endif
        return(ZS_SUCCESS);
}//  ZIX::removeAll


ZStatus ZIndexFile::_removeAll(_KType &pKey)
{
ZMulti_Result wMRes;
ZStatus wSt;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.lock();
#endif
    _seekAllGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this,&wMRes);
    PersistentStatus = wMRes.ZSt;
    if (wMRes.ZSt!= ZS_FOUND)
               {
//               wMRes.ZSt=_setSecondary(wMRes.ZSt);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                _Mtx.unlock();
#endif
               return (wMRes.ZSt);
               }
    while (wMRes.ZTabIdx->size()>0)
            {
            if ((wSt=ZMFFather->erase(this->Tab[wMRes.ZTabIdx->Tab[0]].ZAMIdx))!=ZS_SUCCESS)
                    {
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
                    _Mtx.unlock();
#endif
                    return(wSt);
                    }
             wMRes.ZTabIdx->pop_front();

             if (wMRes.ZTabIdx->size()>0)                               //! Caution : to be validated : NO CONCURRENT ACCESS during removeALL
                        wMRes.ZTabIdx->Tab[0]--; //! whole Index must be LOCKED : this is not done

            }// while
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
    _Mtx.unlock();
#endif
    return(ZS_SUCCESS);
}//  ZIX::_removeAll



/**
 * @brief ZIndexFile::seek returns the first index of ZAM corresponding to the first value matching secondary Key value pKey
 * @param pKey
 * @return
 */
ZResult ZIndexFile::seek(_KType &pKey)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
    wRes.ZIdx=this->Tab[wRes.ZIdx].ZAMIdx;
    PersistentStatus = wRes.ZSt;
    return(wRes);
}//  ZIX::seek
/**
 * @brief ZIndexFile::seek returns the first index of ZIX corresponding to the first value matching secondary Key value pKey
 * @param pKey
 * @return
 */

ZResult ZIndexFile::_seekFirst(_KType &pKey)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = wRes.ZSt;
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
//    wRes.ZIdx=this->Tab[wRes.ZIdx].ZAMIdx;
    return(wRes);
}//  ZIX::_seekFirst

/**
 * @brief ZIndexFile::seek returns the first found ZAM row index  corresponding to the first value matching secondary Key value pKey within the ZIX index
 * @param pKey
 * @return
 */

ZResult ZIndexFile::seekFirst(_KType &pKey)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = wRes.ZSt;
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
    wRes.ZIdx=this->Tab[wRes.ZIdx].ZAMIdx;
    return(wRes);
}//  ZIX::_seekFirs



/**
 * @brief ZIndexFile::seek returns the first (ZIX) rank order of ZIndexFile corresponding to the first value matching secondary Key value pKey
 * @param pKey
 * @return
 */
ZResult ZIndexFile::_seek(ZDataBuffer &pKey)
{
ZResult wRes;
ZIndexFile_struct<_KType> wZI ;

    wRes=_seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = wRes.ZSt;
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
    return(wRes);
}// ZIX:: _seek


ZStatus ZIndexFile::seekAll(_KType pKey, ZMulti_Result &pMRes)
{

    _seekAllGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this,&pMRes);
    PersistentStatus = pMRes.ZSt;
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               return (pMRes.ZSt);
               }
    for (long wi =0;wi <pMRes.ZTabIdx->size();wi ++)
                pMRes.ZTabIdx->Tab[wi]= this->Tab[pMRes.ZTabIdx->Tab[wi]].ZAMIdx;

    return(pMRes.ZSt);
}//  ZIX::seekAll



/** ------------------------Uses ZAMulti_Result--------------------------------------------
 *
 *
 * @brief ZIndexFile::seekAll
 *              returns a collection of ZAM row ranks corresponding to all key values matching secondary Key values pKey
 * @param pKey
 * @return
 */

ZStatus ZIndexFile::seekAll(_KType pKey, ZAMulti_Result &pMRes)
{

    _seekAllGenericZA<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this,pMRes);
    PersistentStatus = pMRes.ZSt;
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               return (pMRes.ZSt);
               }
    for (long wi =0;wi <pMRes.size();wi ++)
                pMRes[wi]= this->Tab[pMRes[wi]].ZAMIdx;

    return(pMRes.ZSt);
}//  ZIX::seekAll



/**
 * @brief ZIX::seekAllJoinDyn returns a collection of raw ZIX row ranks corresponding to all key values matching secondary Key values pKey
 *             This is dedicated to ZJoinDyn usage
 * @param pKey
 * @return
 */
ZStatus ZIndexFile::_seekAllZJoinDyn(_KType &pKey, ZMulti_Result *pMRes)
{

    _seekAllGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this,pMRes);
    PersistentStatus = pMRes->ZSt;
    return(pMRes->ZSt);
}//  ZIX::seekAllJoinDyn



/**
 * @brief ZIndexFile::seekAllPartial Using ZAMulti_Result
 *          returns a collection of ZAM row ranks corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */
ZStatus ZIndexFile::seekAllPartial(_KType &pKey,size_t pSize, ZAMulti_Result &pMRes)
{
    size_t wj;
    _seekAllPartialGenericZA<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this,pMRes);
    PersistentStatus = pMRes.ZSt;
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               return (pMRes.ZSt);
               }
    for (long wi =0;wi <pMRes.size();wi ++)
                    {
                wj=pMRes[wi];
                pMRes.Tab[wi]= this->Tab[wj].ZAMIdx;
                    }
    return(pMRes.ZSt);
}// ZIX::seekAllPartial - using ZAMulti_Result


/**
 * @brief ZIndexFile::seekAllPartial returns a collection of ZAM row ranks corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */

ZStatus ZIndexFile::seekAllPartial(_KType pKey, size_t pSize, ZMulti_Result &pMRes)
{
    size_t wj;
    _seekAllPartialGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this,&pMRes);
    PersistentStatus = pMRes.ZSt;
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               return (pMRes.ZSt);
               }
    for (long wi =0;wi <pMRes.ZTabIdx->size();wi ++)
                    {
                wj=pMRes.ZTabIdx->Tab[wi];
                pMRes.ZTabIdx->Tab[wi]= this->Data.Tab[wj].ZAMIdx;
                    }
    return(pMRes.ZSt);
}// ZIX::seekAllPartial

/**
 *   Record operations : these ZIX functions return ZAM row content corresponding to ZIX index search for a Key (partial or full)
 */


/**
 * @brief seekRecord
 *                  Seeks ZIX index for pKey content
 *                  if search is successfull, returns :
 *                      - a pointer to ZAM row content
 *                      - the pointer to ZStatus variable is updated with the actual status value if this value has been given in input (not nullptr)
 *
 *                  if search is NOT successful :
 *                      - a nullptr value as return
 *                      - the pointer to ZStatus variable if given in input (different from nullptr) is updated with the current status
 * @param pKey
 * @param pRecord
 * @param pSt
 * @return
 */


const _Type *
ZIndexFile:: seekRecord (_KType pKey,      //! key content to search for
                                                      _Type &pRecord,    //! row buffer that will be returned (or not in case of error)
                                                      ZResult &pRes)      //!Index result structure if given (different from nullptr)
  {
    memset (&pRecord,0,sizeof(_Type));
    pRes =  _seekGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,*this);
    PersistentStatus = pRes.ZSt;
    if (pRes.ZSt!=ZS_FOUND)
                {
//                *pRes=wRes;
                return nullptr;
                }

    memmove (&pRecord,&ZMFFather->Tab[pRes.ZIdx],sizeof(_Type));  //! copy rank to buffer given in input
    return (&ZMFFather->Tab[pRes.ZIdx]);        //! return pointer to this rank
  }//seekRecord



/**
 * @brief seekPartialRecord
 *                  Seeks ZIX index for pKey content
 *                  if search is successfull, returns :
 *                      - a pointer to ZAM row content
 *                      - the pointer to ZStatus variable is updated with the actual status value if this value has been given in input (not nullptr)
 *
 *                  if search is NOT successful :
 *                      - a nullptr value as return
 *                      - the pointer to ZStatus variable if given in input (different from nullptr) is updated with the current status
 * @param pKey
 * @param pRecord
 * @param pSt
 * @return
 */

const _Type *
ZIndexFile:: seekPartialRecord (_KType pKey,      //! key content to search for
                                                             size_t pSize,       //! size of the key
                                                             _Type &pRecord,    //! row buffer that will be returned (or not in case of error)
                                                             ZResult &pRes)      //! status if given (different from nullptr)
  {
    memset (&pRecord,0,sizeof(_Type));
    pRes =  _seekFirstPartialGeneric(pKey,pSize,*this);
    PersistentStatus = pRes.ZSt;
    if (pRes.ZSt!=ZS_FOUND)
                {
//                *pRes=wRes;
                return nullptr;
                }

    memmove (pRecord,&ZMFFather->Tab[pRes.ZIdx],sizeof(_Type));  //! copy rank to buffer given in input
    return (&ZMFFather->Tab[pRes.ZIdx]);        //! return pointer to this rank
  }//seekRecord




/**
 * @brief ZIndexFile::-seekAllPartial returns a collection of ZIX row ranks corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 *                                                  returned values are row ranks within the index (and not within ZAM)
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */
ZStatus ZIndexFile::_seekAllPartial(_KType pKey, size_t pSize, ZMulti_Result &pMRes)
{
    _seekAllPartialGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this,pMRes);
    if (pMRes.ZSt!= ZS_FOUND)
               {
//               pMRes.ZSt=_setSecondary(pMRes.ZSt);
               }
    return(pMRes.ZSt);
}// ZIX::_seekAllPartial





/**
 * @brief ZIndexFile::-seekFirstPartial returns the FIRST found ZIX row rank corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 *                                                  returned values are row ranks within the ZIX index (and not within ZAM)
 *
 *                                                  Warning : returns w ZIX rank and NOT ZAM rank
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */
ZResult ZIndexFile::_seekFirstPartial(_KType pKey,size_t pSize)
{

    return(_seekFirstPartialGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this));

}// ZIX::_seekFirstPartial



/**
 * @brief ZIndexFile::-seekFirstPartial returns the FIRST found corresponding ZAM row rank corresponding to matching secondary Key values pKey on a PARTIAL comparizon on pSize length
 *                                                  returned values are ZAM row ranks
 *
 * @param pKey
 * @param ZMulti_Result : this is the returned data structure with all found elements ranks
 * @return
 */
ZResult ZIndexFile::seekFirstPartial(_KType pKey, size_t pSize)
{
    ZResult wRes=_seekFirstPartialGeneric<_KType,ZIndexFile_struct<_KType>,_KCompare> (pKey,pSize,*this);
    if (wRes.ZSt!= ZS_FOUND)
               {
//               wRes.ZSt=_setSecondary(wRes.ZSt);
               return (wRes);
               }
    wRes.ZIdx = getZAMIdx(wRes.ZIdx);
    return(wRes);
}// ZIX::seekFirstPartial

#ifdef __COMMENT__
template<typename _Tp>
/**
 * @brief _printFieldValue  print the value of any field according its DataType. It is linked to _printStdStringField used for printing transparently std::string and pointer to std::string
 * @param Field
 * @param pBuf
 * @return
 */
char *_printFieldValue(_Tp &Field,char *pBuf)
{
ZData_Type_struct _keyDType;

    _get_Data_Type<_Tp>(&_keyDType);

if (_keyDType.isAtomic)
        {
        if (_keyDType.Type & Zchar)
                {
                sprintf(pBuf,"%c",Field);
                return (pBuf);
                }
        if ((_keyDType.Type & Zfloat)==Zfloat)
                {
                sprintf(pBuf,"%F",Field);
                return (pBuf);
                }
        if ((_keyDType.Type & Zlong)==Zlong)
                {
                sprintf(pBuf,"%ld",Field);
                return (pBuf);
                }
        if ((_keyDType.Type & Zint)==Zint)
                {
                sprintf(pBuf,"%d",Field);
                return (pBuf);
                }
        if ((_keyDType.Type & Zdouble)==Zdouble)
                {
                sprintf(pBuf,"%G",Field);
                return (pBuf);
                }
        }  // IsAtomic

if (_keyDType.isCString)
         {

        sprintf(pBuf,"%s",Field);
        return (pBuf);
        }
if (_keyDType.isStdString)
         {

            return(_printStdStringField((void*)&Field,_keyDType.isPointer,pBuf));
//          fprintf (stderr,"Warning: std::string is not managed for printFieldValue ");
//          sprintf(pBuf,"not managed");
//          return(pBuf);
        }
if (_keyDType.isPointer)
        {
        sprintf (pBuf,"%p",Field);
        return (pBuf);
        }
if (_keyDType.isArray )
            {

    if (_keyDType.Type&Zchar)
            sprintf(pBuf,"%s",Field);
            else
               {
            sprintf (pBuf,"<array -%s- >",_keyDType.TypeName);
                }
            return (pBuf);
            }
if (_keyDType.isCompound)
            {
            return ((char *)dumpSequence((void *)&Field,sizeof(_Tp),pBuf));
            }

} // printFieldValue


template < class _Type,class _Bulk,class _KType,class _KCompare>
char *  ZIndexFile::printKeyValue(long pIdx,char *pBuf)
    {
_Bulk *pElement = (_Bulk *)&(this->Tab[pIdx]);

    if ((pIdx<0)||(pIdx>=size()))
    {
    fprintf (stderr, "ZIndexFile::printKeyValue-F-OUTOFBOUND Wrong index value out of ZArray boundaries <%ld> \n", pIdx);
    _ABORT_ ;
    }
//     wU.wPtr =( void *)&Tab[pIdx]+_KeyOffset ;

    if (_keyDType.isAtomic)
            {
            if (_keyDType.Type & Zchar)
                    {
                    sprintf(pBuf,"%c",pElement->_Key);
                    return (pBuf);
                    }
            if ((_keyDType.Type & Zfloat)==Zfloat)
                    {
                    sprintf(pBuf,"%F",pElement->_Key);
                    return (pBuf);
                    }
            if ((_keyDType.Type & Zlong)==Zlong)
                    {
                    sprintf(pBuf,"%ld",pElement->_Key);
                    return (pBuf);
                    }
            if ((_keyDType.Type & Zint)==Zint)
                    {
                    sprintf(pBuf,"%d",pElement->_Key);
                    return (pBuf);
                    }
            if ((_keyDType.Type & Zdouble)==Zdouble)
                    {
                    sprintf(pBuf,"%G",pElement->_Key);
                    return (pBuf);
                    }
            }  // IsAtomic

    if (_keyDType.isCString)
             {

            sprintf(pBuf,"%s",pElement->_Key);
            return (pBuf);
            }

    if (_keyDType.isPointer)
            {
            sprintf (pBuf,"%p",pElement->_Key);
            return (pBuf);
            }
    if (_keyDType.isArray )
                {

        if (_keyDType.Type&Zchar)
                sprintf(pBuf,"%s",pElement->_Key);
                else
                   {
                sprintf (pBuf,"<array -%s- >",_keyDType.TypeName);
                    }
                return (pBuf);
                }
    if (_keyDType.isCompound)
                {
                return ((char *)dumpSequence((void *)&pElement->_Key,sizeof(_KType),pBuf));
                }

    } // printKeyValue
#endif //__COMMENT__

//!----------------End external ZIndexFile expanded methods----------------------------
//!
//!


/** @}  */


#endif // ZMASTERFILE_H
