#ifndef ZSEARCHCONTEXT_H
#define ZSEARCHCONTEXT_H

#include <ztoolset/zarray.h>
#include <ztoolset/utfvaryingstring.h>

#include "zsearchparsertype.h"
#include "zsearchhistory.h"

//#include "zselclause.h"
//#include "zsearchentity.h"

namespace zbs {
typedef uint64_t  ZSearchInstruction_type;
enum ZSearchInstruction_enum : ZSearchInstruction_type
{
    ZSITP_Nothing           = 0,

    /* execute */

    ZSITP_ToBeExecuted      = 0x0000000F,

    ZSITP_Find              = 0x00000002,

    ZSITP_For               = 0x00000004,
    ZSITP_Display           = 0x00000008,
    ZSITP_Fetch             = 0x00000010,

    /* execute what ?               FF00 */

    ZSITP_File              = 0x00000100,
    ZSITP_Symbol            = 0x00000200,

    ZSITP_SingleEntity      = 0x00000400,
    ZSITP_Join              = 0x00000800,


    /* execute how ?              FF0000 */

    ZSITP_First             = 0x00010000,
    ZSITP_Next              = 0x00020000,
    ZSITP_All               = 0x00040000,
    ZSITP_At                = 0x00080000,

    ZSITP_ReadOnly          = 0x00100000,
    ZSITP_Modify            = 0x00200000,

    /* not to be executed */

    ZSITP_Declare           = 0x01000000,
    ZSITP_Set               = 0x02000000,
    ZSITP_Clear             = 0x04000000,
    ZSITP_Show              = 0x08000000,
    ZSITP_Save              = 0x10000000,
    ZSITP_Finish            = 0x20000000,

//    ZSITP_Other             = 0x00100000,





};

utf8VaryingString decode_ZSITP(ZSearchInstruction_type pZSITP);

class ZSearchEntity ;
class ZSearchTokenizer ;
class ZSearchParser ;
class ZSearchEntityContext ;

class ZSearchContext
{
public:
    ZSearchContext() : Parser(nullptr) {}
    ZSearchContext(ZSearchParser* pParser) :  Parser(pParser) {}
    ZSearchContext(ZSearchParser* pParser, const utf8VaryingString* pMainPhrase) : MainPhrase(pMainPhrase) , Parser(pParser) {}
    ZSearchContext(const ZSearchContext&pIn) {_copyFrom(pIn);}

    ~ZSearchContext();

    ZSearchContext& _copyFrom(const ZSearchContext&pIn);

    void setMainPhrase(const utf8VaryingString* pMainPhrase) {MainPhrase=pMainPhrase;}
    void clear() ;
    void clearEntities();

    bool isNestedField();
    bool isNestedAll();
    bool isNested();

    bool isInvalid() {return Parser==nullptr;}

    bool hasCurrent(ZSearchTokentype_type pType);

    bool notEOF() ;
    bool isEOF();

//    bool testSemiColon();
#ifdef __DEPRECATED__
    void setMessage(const utf8VaryingString& pMessage,...)
    {
        va_list args;
        va_start (args, pMessage);
        Message.vsnprintf(size_t(cst_messagelen),pMessage.toCChar(), args);
        va_end(args);
 //       Message=pMessage;
    }
    bool hasMessage() { return !Message.isEmpty();}
    void clearMessage() {Message.clear();}
#endif // __DEPRECATED__

    void setEntity (std::shared_ptr<ZSearchEntity> pEntity) {SourceEntity=pEntity;}

    void setIndex(int pIndex);
    void pushIndex();
    void popIndex();
    bool advanceIndex(bool pSignalEOF=true);

    /* initializes tokens interpretation after having parsing with Tokenizer */
    void beginContext();

    void TokenReport() ;

    /* gives the token following current pointer without modifying token index
     * returns nullptr if index points to last token  */
    ZSearchToken* nextToken();

    /* gives the previous token of current pointer without modifying token index
     * returns nullptr if index points to first token  */
    ZSearchToken* previousToken();

    ZSearchToken* lastToken();

//    void setTokenizer(ZSearchTokenizer* pTokenizer) { Tokenizer=pTokenizer; }

//    ZArray<std::shared_ptr<ZSearchEntity>>  CurEntities;        /* current used entities within phrase */
    std::shared_ptr<ZSearchEntity> SourceEntity=nullptr;
    std::shared_ptr<ZSearchEntity> TargetEntity=nullptr;

    ZSearchEntityContext*       SEC=nullptr;

    int                         InstructionType = ZSITP_Nothing;
    int                         Number = -1;  /* default is all (-1) */

//    ZSearchTokenizer*           Tokenizer=nullptr;
    ZSearchParser*              Parser=nullptr;
    int                         Index=0;

    ZArray<ZSearchToken*>       CurrentTokenList;

    ZArray<int>                 IndexStack;

    InstructionFlag_type        Store=ZSearchHistory::Forget;
    bool                        HasInstruction=false;
    ZSearchToken*               TokenStart=nullptr;
    ZSearchToken*               CurrentToken=nullptr;
    ZSearchToken*               LastErroredToken=nullptr;
    ZSearchToken*               TokenIdentifier=nullptr;
    utf8VaryingString           Phrase;
    const utf8VaryingString*    MainPhrase=nullptr;
#ifdef __DEPRECATED__
    utf8VaryingString           Message;            /* not an error (given by errorLog) but a communication message */
#endif // __DEPRECATED__
    ZStatus                     Status=ZS_SUCCESS;
//    ZFldSelClause                  SelClause;
};  // ZSearchContext

} // namespace zbs
#endif // ZSEARCHCONTEXT_H
