#include "zsearchcontext.h"
#include "zsearchentity.h"

//using namespace zbs;
#include "zsearchhistory.h"
#include "zsearchtokenizer.h"
#include "zsearchparser.h"

#include "zsearchentitycontext.h"

namespace zbs {

//ZSearchContext::ZSearchContext() {}

ZSearchContext::~ZSearchContext()
{
/*
     if (SEC!=nullptr)
        delete SEC;
*/
}


ZSearchContext&
ZSearchContext::_copyFrom(const ZSearchContext&pIn)
{
    clearEntities();
//    for (int wi=0; wi < pIn.CurEntities.count();wi++)
//        CurEntities.push(pIn.CurEntities[wi]) ;
    SourceEntity = pIn.SourceEntity;
    InstructionType=pIn.InstructionType;
    Number=pIn.Number;
    Store = pIn.Store;
    Phrase = pIn.Phrase;
    TokenStart = pIn.TokenStart;
    HasInstruction = pIn.HasInstruction;
    MainPhrase = pIn.MainPhrase;
    Status = pIn.Status;

    CurrentToken = pIn.CurrentToken;
    Index = pIn.Index;
    Parser = pIn.Parser;


    SEC = pIn.SEC;

    return *this;
}

void ZSearchContext::setIndex(int pIndex){
    IndexStack.push(Index);
    Index=pIndex;
    if (pIndex >= CurrentTokenList.count() )
        return;
    CurrentToken = CurrentTokenList.Tab(pIndex);
}
void ZSearchContext::pushIndex(){
    IndexStack.push(Index);
    CurrentToken = CurrentTokenList.Tab(Index);
}
void ZSearchContext::popIndex(){
    if (IndexStack.count()==0) {
        fprintf(stderr,"ZSearchContext::popIndex-F-EMPTY Index stack is empty while trying to pop.");
        abort();
    }
    Index=IndexStack.popR();
    if (Index >= CurrentTokenList.count() )
        return;
    CurrentToken = CurrentTokenList.Tab(Index);
}

bool ZSearchContext::notEOF()
{
    return (Index < CurrentTokenList.count());
    if (Index < CurrentTokenList.count())
        if (CurrentTokenList[Index]->Type != ZSRCH_SEMICOLON)
        return true ;
    return false;
}
bool ZSearchContext::isEOF()
{
    if (Index >= CurrentTokenList.count())
        return true ;
    if (CurrentTokenList[Index]->Type == ZSRCH_SEMICOLON)
        return true ;
    return false;
}

bool ZSearchContext::isNestedField()
{
    if (isEOF())
        return false;
    if (CurrentToken->Type!=ZSRCH_IDENTIFIER)
        return false;
    if (Index+1 > CurrentTokenList.count())
        return false;
    if (CurrentTokenList.Tab(Index+1)->Type != __SEARCH_ENTITY_SEPARATOR__)
        return false;
    if (CurrentTokenList.Tab(Index+2)->Type != ZSRCH_IDENTIFIER )
        return false;
    return true;
}
bool ZSearchContext::isNestedAll()
{
    if (isEOF())
        return false;
    if (CurrentToken->Type!=ZSRCH_IDENTIFIER)
        return false;
    if (Index+1 > CurrentTokenList.count())
        return false;
    if (CurrentTokenList.Tab(Index+1)->Type != __SEARCH_ENTITY_SEPARATOR__)
        return false;
    if (CurrentTokenList.Tab(Index+2)->Type != ZSRCH_WILDCARD )
        return false;
    return true;
}
bool ZSearchContext::isNested()
{
    if (isEOF())
        return false;
    if (CurrentToken->Type!=ZSRCH_IDENTIFIER)
        return false;
    if (Index+1 > CurrentTokenList.count())
        return false;
    if (CurrentTokenList.Tab(Index+1)->Type != __SEARCH_ENTITY_SEPARATOR__)
        return false;
    return true;
}

bool
ZSearchContext::hasCurrent(ZSearchTokentype_type pType)
{
    return CurrentToken->Type == pType;
}

void
ZSearchContext::clear()
{
    clearEntities();
//    clearMessage();

    InstructionType = ZSITP_Nothing;
    Store=ZSearchHistory::Forget;
    HasInstruction=false;
    TokenStart=nullptr;
    MainPhrase=nullptr;
    CurrentToken=nullptr;
    Status=ZS_SUCCESS;

    TokenIdentifier=nullptr;
    Parser->clear();
    CurrentTokenList.clear();
//    SelClause.clear();
}
void
ZSearchContext::clearEntities()
{
/*
    for (int wi=0; wi < CurEntities.count();wi++)
        if (CurEntities[wi] != nullptr ) {
            CurEntities[wi].reset();
            CurEntities[wi] = nullptr;
        }
    while (CurEntities.count())
        CurEntities.pop();
  */
    SourceEntity.reset();
    SourceEntity = nullptr;
    TargetEntity.reset();
    TargetEntity  = nullptr;
}
#ifdef __DEPRECATED__
void
ZSearchContext::begin()
{
    if (CurrentTokenList.count()==0) {
        Parser->ErrorLog.errorLog("No token to parse in instruction line.");
        Status = ZS_EOF;
        return;
    }
    Index=0;
    TokenStart = CurrentToken = CurrentTokenList.Tab(Index);
    LastErroredToken=nullptr;
    return;
}


bool ZSearchContext::testSemiColon()
{
    if (Index >= CurrentTokenList.count()) {
        if (CurrentTokenList.count()==0)
            Parser->ErrorLog.errorLog("No valid token parsed.");
        Status = ZS_EOF;
        return false;
    }

    if (CurrentTokenList.Tab(Index)->Type != ZSRCH_SEMICOLON) {
        Status = ZS_MISS_PUNCTSIGN ;
        return false;
    }

    return true;
}
#endif // __DEPRECATED__

void
ZSearchContext::beginContext()
{
    if (CurrentTokenList.count()==0) {
        Parser->ErrorLog.errorLog("No token to parse.");
        Status = ZS_EOF;
        return;
    }
    Index=0;
    TokenStart = CurrentToken = CurrentTokenList.Tab(Index);

    while (notEOF()&&(CurrentToken->Type==ZSRCH_SPACE))
        advanceIndex();

    LastErroredToken=nullptr;
    return;
}

void ZSearchContext::TokenReport() {
    for(long wi=0; wi < CurrentTokenList.count(); wi++) {
        CurrentTokenList[wi]->displayNoindex();
    }
}


bool ZSearchContext::advanceIndex(bool pSignalEOF) {

    if (CurrentTokenList.count()==0) {
        Parser->ErrorLog.errorLog("No token to parse for this instruction line.");
        Status = ZS_EOF;
        return false;
    }

    Index++;

    if (Index >= CurrentTokenList.count()) {
        if (pSignalEOF) {
            Parser->ErrorLog.errorLog("Syntax error : End of token while expecting more. Last token <%s> line %d column %d.",
                                      CurrentTokenList.last()->Text.toString(),
                                      CurrentTokenList.last()->TokenLine,CurrentTokenList.last()->TokenColumn
                                      );
            LastErroredToken = CurrentTokenList.last();
        }
        Status = ZS_EOF;
        return false;
    }

    while (notEOF()) {

        CurrentToken=CurrentTokenList.Tab(Index);

        if (CurrentToken->Type == ZSRCH_SPACE) {
            Index++;
            continue;
        }
        if ((CurrentToken->Type & ZSRCH_MAYBE_COMMENT) == ZSRCH_MAYBE_COMMENT) {
            Index++;
            continue;
        }
        break;
    }// while (pIdx < ZSearchTokenizer::count())
    if (isEOF()) {
        Status=ZS_EOF;
        return false;
    }

    return true;
} // ZSearchContext::advanceIndex


ZSearchToken* ZSearchContext::nextToken()
{
    if (Index >= CurrentTokenList.count())
        return nullptr;
    return CurrentTokenList[Index+1];
}

ZSearchToken* ZSearchContext::previousToken()
{
    if (Index == 0)
        return nullptr;
    return CurrentTokenList[Index-1];
}

ZSearchToken* ZSearchContext::lastToken()
{
    if (CurrentTokenList.count() == 0)
        return nullptr;
    return CurrentTokenList.last();
}

utf8VaryingString decode_ZSITP(ZSearchInstruction_type pZSITP)
{
    utf8VaryingString wReturn;
    if (pZSITP==ZSITP_Nothing)
        return "ZSITP_Nothing";


    if (pZSITP & ZSITP_For)
        wReturn.addConditionalOR("ZSITP_Find");

    if (pZSITP & ZSITP_For)
        wReturn.addConditionalOR("ZSITP_For");

    if (pZSITP & ZSITP_Display)
        wReturn.addConditionalOR("ZSITP_Display");

    if (pZSITP & ZSITP_Fetch)
        wReturn.addConditionalOR("ZSITP_Fetch");



    if (pZSITP & ZSITP_File)
        wReturn.addConditionalOR("ZSITP_File");

    if (pZSITP & ZSITP_Symbol)
        wReturn.addConditionalOR("ZSITP_Symbol");

    if (pZSITP & ZSITP_SingleEntity)
        wReturn.addConditionalOR("ZSITP_SingleEntity");

    if (pZSITP & ZSITP_Join)
        wReturn.addConditionalOR("ZSITP_Join");



    if (pZSITP & ZSITP_First)
        wReturn.addConditionalOR("ZSITP_First");

    if (pZSITP & ZSITP_Next)
        wReturn.addConditionalOR("ZSITP_Next");

    if (pZSITP & ZSITP_All)
        wReturn.addConditionalOR("ZSITP_All");


    if (pZSITP & ZSITP_ReadOnly)
        wReturn.addConditionalOR("ZSITP_ReadOnly");
    if (pZSITP & ZSITP_Modify)
        wReturn.addConditionalOR("ZSITP_Modify");



    if (pZSITP & ZSITP_Declare)
        wReturn.addConditionalOR("ZSITP_Declare");
    if (pZSITP & ZSITP_Set)
        wReturn.addConditionalOR("ZSITP_Set");
    if (pZSITP & ZSITP_Clear)
        wReturn.addConditionalOR("ZSITP_Clear");
    if (pZSITP & ZSITP_Show)
        wReturn.addConditionalOR("ZSITP_Show");
    if (pZSITP & ZSITP_Save)
        wReturn.addConditionalOR("ZSITP_Save");
    if (pZSITP & ZSITP_Finish)
        wReturn.addConditionalOR("ZSITP_Finish");

    if (wReturn.isEmpty()) {
        wReturn.sprintf("Unknown ZSITP %X ",pZSITP);
    }
    return wReturn;
}
} // namespace zbs
