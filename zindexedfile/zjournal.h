#ifndef ZJOURNAL_H
#define ZJOURNAL_H

#include <cstdarg>
#include <zrandomfile/zrandomfile.h>
#include <ztoolset/zbasedatatypes.h>
#include <znet/zbasenet/znetcommon.h>


#define __JOURNAL_EXT__                 "jnl"
#define __JOURNAL_ENDOFFILEPATH__       "_jnl.jnl"

enum ZJOperation:long {
    ZJOP_Nothing    =0x0,
    ZJOP_Add                =0x00000001,
    ZJOP_Insert             =0x00000002,
    ZJOP_RemoveByRank       =0x00000004,
    ZJOP_RemoveByAddress    =0x00000008,
    ZJOP_ChgFld             =0x00000100,
    ZJOP_RollBack           =0x00000010,
    ZJOP_Close              =0x0000ffff
};

const char *decode_ZJOP(const ZJOperation pOperation);

typedef uint8_t ZJState_base;

enum ZJState_type : ZJState_base {
    ZJS_Nothing     =0x0,
    ZJS_Created     =0x01,
    ZJS_RolledBack  =0x04,
    ZJS_Exported    =0x10
};

struct ZJState
{
//public:
    ZJState_base State;
    ZJState() {State = (ZJState_base)ZJS_Nothing;}
    const char *decode(void);
    ZJState &encode(char *pString) ;

    ZJState& operator = (ZJState_type pState) {State=(ZJState_base)pState; return *this;}
    ZJState& operator = (ZJState_base pState) {State=(ZJState_base)pState; return *this;}

    bool is(ZJState_type pState) {return (State&pState);}

    ZJState_base toInt(void) {return State;}
/*    friend
    int8_t operator << (int8_t pState,ZJState pZJState) {pState=pZJState.State; return pState;}
*/

private:
    utfdescString  Buf ;
};



#ifdef __COMMENT__   // see zremote/znetcommon.h
//descString* decode_ZJState (const uint8_t pState);
typedef uint16_t ZProtocol_base;

enum ZJProtocol_type : ZProtocol_base
{
    ZJP_Nothing     = 0,
    ZJP_Base        = 0x0001,
    ZJP_RPC         = 0x0002,
    ZJP_Corba       = 0x0004,
    ZJP_DBus        = 0x0008,
    ZJP_XMLRPC      = 0x0010,
    ZJP_SSLV5       = 0x0100,
    ZJP_B64         = 0x0200
};

char * decode_ZJP (const int16_t pOperation);

struct ZJProtocol
{
//public:
    ZProtocol_base Protocol;
    ZJProtocol() {Protocol = (ZProtocol_base)ZJS_Nothing;}
    char *decode(void);
    ZJProtocol &encode(const char *pString) ;

    ZJProtocol& operator = (ZJProtocol pProtocol) {Protocol=(ZProtocol_base)pProtocol.Protocol; return *this;}
    ZJProtocol& operator = (const ZProtocol_base pProtocol) {Protocol=(ZProtocol_base)pProtocol; return *this;}

    bool is(ZJProtocol_type pProtocol) {return (Protocol & pProtocol);}

    ZProtocol_base toInt(void) {return Protocol;}


private:
    descString  Buf ;
};
#endif // __COMMENT__


ZStatus generateJournalName(uriString &pZMFName, uriString &pJournalingPath, uriString &pJournalName);


namespace zbs{


class ZJEvent : public ZDataBuffer
{
public:
    ZJEvent(void) {}
    ZJEvent (const ZJOperation             pOperation,
             pid_t                  pPid,
             const ZSystemUserId&   pUid,
             const ZDataBuffer      &pRecord,
             const zrank_type       pRank,
             const zaddress_type    pAddress,
             const ssize_t          pOffset)
        {
        Header.DateTime = ZDateFull::currentDateTime();
        Header.Operation = pOperation;
        Header.Pid=pPid;
        Header.Uid=pUid;
//        Header.Username = pUsername;
        Header.Rank = pRank;
        Header.Address = pAddress;
        Header.Offset   = pOffset;
        Header.State    = ZJS_Created;
        setData(pRecord);
        return;
        }

    void getRecordFromEvent(ZDataBuffer &pRecord);
    void getEventFromRecord(ZDataBuffer &pRecord);
    void getFieldChange(ZDataBuffer &pFieldBefore,ZDataBuffer& pFieldAfter);


//-----------ZJEvent Data-------------------
    struct ZJE_Header {
    ZJOperation     Operation;
    ZJState         State; //!< define the state of the journal record
    pid_t           Pid;
    ZSystemUserId   Uid;
//    utfdescString   Username;
    ZDateFull       DateTime;
    zrank_type      Rank;
    zaddress_type   Address;
    ssize_t         Offset; //!< in case of setField operation (ZJOP_ChgFld)
    } Header;
};

class ZJEventQueue : protected ZArray <ZJEvent*>
{
    typedef ZArray<ZJEvent*> _Base;
public:
    ~ ZJEventQueue(void)
    {
    clear();
    }

    using _Base::size;

    void clear(void) { while (pop()!=ZS_EMPTY); }

    void enqueue (const ZJOperation &pZJOP,
                  pid_t pPid,
                  const ZSystemUserId &pUid,
                  const ZDataBuffer &pRecord,
                  const zrank_type pRank=-1,
                  const zaddress_type pAddress=-1,
                  const ssize_t pOffset=-1);

    void dequeue(ZJEvent &pEvent);

    // operator overload : return a reference to a const journal event content
    const ZJEvent& operator [](const zrank_type pRank) {return *Tab[pRank];}

    ZStatus erase (const zrank_type pRank);

    ZStatus pop();
//----------Queue Data-------------------
    ZMutex Mtx;
};

class ZRawMasterFile;
class ZSJournal :  protected ZRandomFile
{
typedef ZRandomFile _Base;
public:
    ZSJournal(ZRawMasterFile* pFather) {Father = pFather;}

//                                    CMtx.init(false);}

    ~ZSJournal(void);

    ZStatus setUp(uriString &pJournalPath);

    ZStatus init(const bool pmustExist=false);
    ZStatus reset(ZRawMasterFile* pFather);
    ZStatus start(void);
    void    end(void);

    ZStatus createFile(void);

    using ZRandomFile::getURIContent;
    using ZRandomFile::getURIHeader;
    using ZRandomFile::isOpen;
//    using ZRandomFile::setPath;
    inline
    ZStatus setJournalLocalDirectoryPath(void);

    void enqueue (const ZJOperation pOperation,
                  const ZDataBuffer &pRecord,
                  const zrank_type pRank=-1,
                  const zaddress_type pAddress=-1,
                  const ssize_t pOffset=-1);

    void enqueueSetFieldValue (const ZJOperation pOperation,
                               ZDataBuffer &pFieldBefore,
                               ZDataBuffer &pFieldAfter,
                               const zrank_type pRank=-1,
                               const zaddress_type pAddress=-1,
                               const ssize_t pOffset=-1);

    ZStatus purge(const zrank_type pKeepRanks=-1);

    ZStatus getEvent(zrank_type pRank,ZJEvent pEvent);

    static void* process(void* pJournal); // client thread


    ZStatus report(FILE* pOutput);


public:
    ZJEventQueue    EvtQueue;
    ZMutexCondition CMtx;
    ZThread_Base         JThread;
    ZStatus         ThreadStatus;

protected:
    pid_t       Pid;
    ZSystemUserId     Uid;
    utf8String    Username;
    ZRawMasterFile* Father;
//    uriString   URIJournal;
};


} //zbs
#endif // ZJOURNAL_H
