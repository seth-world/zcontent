#include "zsearchparser.h"

#include "zsearchparsertype.h"
#include "zsearchentity.h"
//#include "zsearchformula.h"
//#include "zsearchcollection.h"

#include <ztoolset/zexceptionmin.h>

#include "zmasterfile.h"

#include <zio/zioutils.h>
#include <fcntl.h>

#include <ztoolset/ztimer.h>

#include <zxml/zxmldoc.h>
#include <zxml/zxmlnode.h>
#include <zxml/zxmlprimitives.h>
//#include "zcpptypedef.h"

//#include "zsearchformula.h"
#include "zsearchparsertype.h"
#include "zsearchoperand.h"
#include "zsearchlogicalterm.h"
#include "zsearcharithmeticterm.h"

#include "zsearchcontext.h"
#include "zsearchentitycontext.h"

#include <ztoolset/utfutils.h>

#include <zcontent/zcontentcommon/urfparser.h>
#include <zcontent/zcontentcommon/zcontentconstants.h>

#include <zcontentcommon/zgeneralparameters.h>

using namespace zbs;

namespace zbs {

ZArray<ZEntitySymbol>  ZEntitySymbolList;

}

const char* ZSearchQueryHelp =
" help </b>"
" set file <path> [mode readonly,modify] as <entity name> ;  // by default mode is read only<br>"
    "                      <----------------selection phrase---------------> <br>";
/*
  help <br><br>
  declare <entity name> as file <path> [mode readonly,modify] ;  // by default mode is read only <br>"
<br>

Entities declarations

  declare <entity name> as file <path> [mode readonly,modify] [;]  // by default mode is read only

  creates an entity definition with name <entity name>.
  This entity refers to a master file described by <path> that must point to a valid, existing master file.
  File will be accessed according mode, either readonly (default) or modify.

  <entity name> must be a unique, non existing entity name.

  if mode clause is omitted then mode is set to readonly.

  Once successfully declared, <entity name> will be available to :
  - be accessed (see fetch, display instructions)
  - be used in other declare instructions (using its name, here <entity name>)


Single entity (Collection)

          ---------------------------Declare predicate--------------------------------------------------------------------------------------------------------
                            ----- field selection clause--------
  declare <entity name> as { <field name1>, <field name2> , ....}
                           { * }
                        --entity selection--
                        in <entity name>
                              ---------------------rank selection clause----------------------
                        with [<entity name>.]<field name> { = != < > <= >= } <field name1>,....  [;]




                               -----------fields selection clause (from both entities)--------
  declare <entity name> as join { <entity name1>.<field name1>, <entity name2>.<field name2>,... }
                             { <entity name1>.* , <entity name2>.* }
                             ----entities selection--------
                        in <entity name1> , <entity name2>
                             -------rank selection clause------
                        with <see rank selection clause syntax>
                                -------------------------join clause----------------------------------------------------------
                        using <entity name>.<field name>[.<modifier>] { = < > >= <= } <entity name2>.<field name2>[.<modifier>] [,...]

Join Entity

  declare <new entity name> as < master entity definition > join <slave entity definition> using <join clause>

                               -----------fields selection clause for first entity (master)----
  declare <entity name> as { [<entity name1>.]<field name1>, [<entity name1>.]<field name2>... }    nb: only fields from <entity name1>
                             { <entity name1>.*  }
                             --Master----
                        in <entity name1>
                             --ranks selection clause for master----
                        with <see rank selection clause syntax>
                                -----------fields selection clause for second entity (slave)----
                        join { [<entity name2>.]<field name1>, [<entity name2>.]<field name2>... }
                             { <entity name2>.* }
                             ---Slave-----
                        in  <entity name2>
                             --ranks selection clause for slave----
                        with <see rank selection clause syntax>
                                --------------------------------join clause----------------------------------------------------------
                        using <entity name>.<field name>[.<modifier>] { = < > >= <= } <entity name2>.<field name2>[.<modifier>] [,...]

  declare <new entity name> as <master entity name> join <slave entity name> using <join clause>

Symbol declaration

  declare <name> = { <literal> , <arithmetic expression> , <logical expression> }

Fetch instruction

  fetch <entity name>  [all]
  fetch <entity name> first [<count>]
  fetch <entity name> next [<count>]

  display <entity name>  [all]
  display <entity name> first [<count>]
  display <entity name> next [<count>]


  find [ {all , first [count] , next [count] } <---Declare predicate--->

    is equivalent to

        declare <declare clause content> ;
        fetch {all , first [count] , next [count] } ;
        display {all , first [count] , next [count] }


\n
                                                       <---------------------------- assignment phrase ------------------------------>
for <entity> with [all,first] <selection phrase > set [<entity name>.]<field name> = <value> , [<entity name>.]<field name> = <value> ... ;


set <symbol name> = { <literal value> , <arithmetic expression> , <logical expression> }  -> replace symbol content regardless symbol type



-------- Dictionary display -----------------------------------------

  show entities;                -> list all entities available
  show ZEntity;                 -> list all ZEntity symbols (used for ZResource)
  show symbols ;                -> list all symbols available : a symbol may be a shortcut to a filename
  show symbol <symbol name> ;   -> displays symbol content
  show <entity> state [;]       -> displays fetch state for entity <entity>
  show <entity>  fields ;       -> displays all fields for entity <entity>
  show <entity>  <fieldname> ;  -> displays exact field name details
  show <entity>  sss* ;         -> displays details for field whose name starts with 'sss'
  show <entity> *sss ;          -> displays details for field whose name ends with 'sss'
  show <entity> *sss* ;         -> displays details for field whose name ends with 'sss'



Instruction history

  set history maximum nn ;  -> set the maximum instruction lines to keep in history log. Default is 40

  show history ;  -> display all instruction lines in history log


  clear history ;       -> removes ALL instruction lines from the history. History is then empty.

  clear history first 5; -> removes the 5 FIRST instruction lines in the history or removes all the first lines until 3.

  clear history last 3;  -> removes the 3 LAST instruction lines in the history or removes all the last lines until 3.

  clear history at 3 ; -> removes the third instruction line in the history. If rank 3 does not exist in history, nothing is done.
                            NB: rank is counted from 1 (and not from 0)



  finish ;  // release resources flush and close open files


  field modifiers :

        a field may be designated using a field modifier.
        Then its type may change from the field type itself to the modifier result type.
        Example : ZDate(99/99/999).month will modify type ZDate to integer.

        string modifiers
          right(<value>)
          where <value> is a non negative integer
          retulting type : string
          extract the substring up to <value> number of characters from the end of the string.
          If string length is less than <value> then the whole string content is returned.

          left(<value>)
          where <value> is a non negative integer
          retulting type : string
          extract the substring up to <value> number of characters starting at the beginning of the string.
          If string length is less than <value> then the whole string content is returned.

          substring(<start>,<value>)
          where <start> and <value> are non negative integers
          retulting type : string
          extract the substring up from a string at offset <start> (starting 0) up to <value> number of characters.
          If string length is less than <start>+<value> then returned string length is adjusted to <input string length> - (<start>+<value>)

        date modifiers

        resource modifiers


        Particular case : translate modifier

        tranlate(<entity name>,<key field name>,<resulting field name>,<match operator>)

        This modifier will use current content value to
            - match within entity <entity name> for <key field name> using <match operator>.
            - return first match for the <resulting field name> field content value
        resulting type : type of <resulting field name> field within <entity name> entity




*/
using namespace std;

namespace zbs {

class ZSearchParser* GParser=nullptr;

ZSearchParser::ZSearchParser()
{
//Tokenizer = new ZSearchTokenizer;
}

ZSearchParser::~ZSearchParser() {

/*  if (Tokenizer)
    delete Tokenizer;
*/
  while (MasterFileList.count())
    MasterFileList.pop();
  while (EntityList.count())
    EntityList.pop();

}


ZStatus
ZSearchParser::setup(uriString& pXmlParserSymbol,
                    uriString& pXmlParserZEntity,
                    uriString &pXmlParserParams,
                    const uriString& pHistory,
                    uint32_t pOptions)
{

  setOptions(pOptions);

  ErrorLog.setAutoPrintOn(ZAIES_Text);

  ZStatus wSt=loadXmlSearchParserZEntity(pXmlParserZEntity);
  if ((wSt!=ZS_SUCCESS)&&(wSt!=ZS_EOF)) {
    ErrorLog.warningLog("Cannot load parser ZEntity xml file <%s>, status is <%s>.No application entity will be defined.",
        pXmlParserZEntity.toCChar(),decode_ZStatus(wSt));
  }
  wSt=loadXmlSearchParserSymbols(pXmlParserSymbol);
  if ((wSt!=ZS_SUCCESS)&&(wSt!=ZS_EOF))  {
    ErrorLog.warningLog("Cannot load parser symbol file <%s>, status is <%s>.No symbol files will be defined.",
        pXmlParserSymbol.toCChar(),decode_ZStatus(wSt));
  }

  wSt=History.setup(pHistory);
  if (wSt!=ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    ErrorLog.logZStatus(ZAIES_Error,wSt,"Cannot open history file <%s>.\n History will be disabled.",pHistory.toString());
  }

  ErrorLog.displayAllLogged(stdout);
  //clear();

  GParser = this;

  return ZS_SUCCESS;
}

//ZStatus ZSearchParser::parse(const utf8VaryingString& pContent, std::shared_ptr<ZSearchEntity> &pCollection)
ZStatus ZSearchParser::parse(const utf8VaryingString& pContent, ZSearchContext &pContext)
{

  if (pContext.isInvalid()) {
    ErrorLog.errorLog("ZSearchParser::parse-E-INVCTX Parse context is invalid. Use parser to generate a valid context.");
    return ZS_INVPARAMS ;
  }
  ZTimer wTi;
  clear();

  pContext.clear();

  pContext.setMainPhrase(&pContent);

  ErrorLog.textLog("Processing request %s",pContext.MainPhrase->toString());

  if (pContent.isEmpty()) {
    ErrorLog.errorLog("No text to parse.");
    return ZS_EMPTY;
  }

  wTi.start();

  ZSearchTokenizer::parse(pContent);  /* create token list for the whole instruction phrase */

  if (ZSearchTokenizer::count()==0) {
      ErrorLog.errorLog("No token to parse.");
      return ZS_EMPTY;
  }

  displayTokenList();

  /* search for keywords and replace with appropriate ZSRCH code */

  for (int wi=0; wi < ZSearchTokenizer::count();wi++) {
    if (ZSearchTokenizer::Tab(wi)->Type==ZSRCH_IDENTIFIER) {
      if (searchKeyword(ZSearchTokenizer::Tab(wi)))
        continue;
    }
  }// for

  /* split if multiple instruction lines (one line is delimited by end of text or semi colon */

//  ZArray<ZSearchToken*> wCurrentTokenList;

  int MainIndex=0;
  while (MainIndex < ZSearchTokenizer::count())
  {
      pContext.CurrentTokenList.clear();
      if (ZSearchTokenizer::Tab(MainIndex)->Type==ZSRCH_SEMICOLON) {
          MainIndex++;
          continue;
      }
      for (; (MainIndex < ZSearchTokenizer::count()) ; MainIndex++  ) {
          if (ZSearchTokenizer::Tab(MainIndex)->Type==ZSRCH_SEMICOLON)
              break;
          pContext.CurrentTokenList.push(ZSearchTokenizer::Tab(MainIndex));
      }

      if (Options & ZSRCHO_Report)
        pContext.TokenReport();

       _parseContext(pContext);

      if (pContext.Status!=ZS_SUCCESS)
          break ;
      if ((pContext.CurrentTokenList.count() > 0) && pContext.notEOF()) {
          if (pContext.CurrentToken != pContext.CurrentTokenList.last()) {
            int wLen = pContext.CurrentTokenList.last()->TokenOffset + pContext.CurrentTokenList.last()->Text.strlen()-pContext.CurrentToken->TokenOffset;
            ErrorLog.warningLog("Remaining of instruction line will not be processed.\n"
                                "Ignored instructions are <%s>",
                                pContext.MainPhrase->subString( pContext.CurrentToken->TokenOffset, wLen ).toString());
          }
      }
  }// while
/*
  if (Options & ZSRCHO_Report)
    ZSearchTokenizer::report();

  ZStatus wSt = _parse(pSearchContext);
*/
  wTi.end();
  ErrorLog.infoLog("Performance report : Parsing elapsed time %s",wTi.reportDeltaTime().toString());

  if (pContext.Store != ZSearchHistory::Forget)
    History.add(pContent);

  return pContext.Status ;
}//ZSearchParser::parse



bool  ZSearchParser::_parseComment (ZSearchToken* &pToken) {
  switch (pToken->Type)
  {

  case ZSRCH_COMMENT_BRIEF:
    return true;
/*  case ZSRCH_COMMENT_BRIEF_IDENTIFIER:
    TooltipType=pToken;
    return true;

  case ZSRCH_COMMENT_PREVIOUS:
    TooltipType=pToken;
    return true;
  case ZSRCH_COMMENT_TEXT:
    Tooltip=pToken;
    if (TooltipType!=nullptr)
      Tooltip->Type = TooltipType->Type;
    return true;
*/
  default:
    return false;
  }
  return false;
} // _parseComment

void
ZSearchParser::showHistory () {
  History.show(&ErrorLog);
}

void
ZSearchParser::showHistoryMaximum () {
  ErrorLog.textLog(" History maximum is set to %d ",History.Maximum);
}

ZStatus
ZSearchParser::setHistoryMaximum (int pMax)
{
  return History.setMaximum(pMax,&ErrorLog);
}

void
ZSearchParser::clearHistory ()
{
  History.List.clear();
  ErrorLog.textLog("            ---instruction log is cleared--.");
  return ;
}

ZStatus
ZSearchParser::clearHistoryFirst (int pLines)
{
  return History.removeFirst(pLines,&ErrorLog);
}

ZStatus
ZSearchParser::clearHistoryLast(int pLines)
{
  return History.removeLast(pLines,&ErrorLog);
}
ZStatus
ZSearchParser::clearHistoryRank (int pRank)
{
  return History.remove(pRank,&ErrorLog);
}


void
ZSearchParser::showSymbols ()
{
  ErrorLog.textLog("__________________Symbol list___________________");
  if (SymbolList.count()==0)
    ErrorLog.textLog("            ---no symbol to show--.");
  else
    for (int wi=0;wi < SymbolList.count();wi++) {
          ErrorLog.textLog("%25s %s",SymbolList[wi].getName().toString(),SymbolList[wi].display().toString());
  }
  ErrorLog.textLog("________________________________________________");
}

ZStatus
ZSearchParser::showSymbol (ZSearchContext& pContext,ZSearchToken*  pTokenName)
{
  if (SymbolList.count()==0) {
    ErrorLog.warningLog("showSymbol-W-EMPTY            ---no symbol to show--.");
    return ZS_EMPTY;
  }
  int wi=0;
  for (; (wi < SymbolList.count()) && (pTokenName->Text != SymbolList[wi].getName());wi++) ;

  if (wi == SymbolList.count()){
    ErrorLog.errorLog("showSymbol-E-NOTFND Symbol %s has not been found",pTokenName->Text.toString());
    pContext.LastErroredToken=pTokenName;
    /* suggest symbols */
    ZArray<utf8VaryingString> wSugArray ;
    /* add entities names to suggest */
    for (int wi=0;wi < SymbolList.count();wi++)
        wSugArray.push(SymbolList[wi].getName());

    /* try to suggest */
    utf8VaryingString wSuggest=searchAdHocWeighted (pTokenName->Text,wSugArray);
    if (!wSuggest.isEmpty())
        ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );
    return pContext.Status=ZS_INVNAME ;
  }
  ErrorLog.textLog("%25s %s",SymbolList[wi].getName().toString(),SymbolList[wi].display().toString());
  return pContext.Status=ZS_SUCCESS;
} // ZSearchParser::showSymbol

void
ZSearchParser::showDisplay ()
{
    ErrorLog.textLog("__________________Display parameters___________________");
    if (URIDisplay.isEmpty()) {
        ErrorLog.textLog("Output is <%s>",DisplayCurrent?"Application defined":"Default");
    }
    else
        ErrorLog.textLog("Output is <%s>",URIDisplay.toString());
    ErrorLog.textLog("Default display");
    ErrorLog.textLog("  maximum entity ranks [limit]   %d %s",DisplayLimit,DisplayLimit<0?"[no limit]":"");
    ErrorLog.textLog("  maximum column size [colmax]   %d",DisplayColMax);
    ErrorLog.textLog("  minimum column size [colmin]   %d",DisplayColMin);
    ErrorLog.textLog("  cell format                    %s",decode_ZCellFormat(DisplayCellFormat).toString());
}

void
ZSearchParser::showZEntities ()
{
  ErrorLog.textLog("__________________ZEntity symbol list___________________");
  if (ZEntitySymbolList.count()==0)
    ErrorLog.textLog("            ---no ZEntity to show--.");
  else
    for (int wi=0;wi < ZEntitySymbolList.count();wi++) {
      ErrorLog.textLog("%25s 0x%10lX",ZEntitySymbolList[wi].Symbol.toString(),ZEntitySymbolList[wi].Value);
    }
  ErrorLog.textLog("________________________________________________");
}

void
ZSearchParser::showMasterFiles ()
{
  ErrorLog.textLog("__________________Master files___________________");
  if (MasterFileList.count()==0)
    ErrorLog.textLog("           ---no Master file to show--.");
  else
    for (int wi=0;wi < MasterFileList.count();wi++) {
      ErrorLog.textLog("%30s %s",
          decode_ZRFMode( MasterFileList[wi]->getOpenMode()), MasterFileList[wi]->getURIContent().toString());
    }
  ErrorLog.textLog("________________________________________________");
}

void
ZSearchParser::showEntityList ()
{
  ErrorLog.textLog("_________________________________________Entity list____________________________________________________");
  if (EntityList.count()==0)
    ErrorLog.textLog("           ---no Entity to show--.");
  else
    for (int wi=0;wi < EntityList.count();wi++) {
      if (!EntityList[wi]->isValid()) {
        ErrorLog.textLog("<Invalid/malformed entity> at rank <%d>",wi);
        continue;
      }
      showEntityDetails(EntityList[wi],3);
    }

  ErrorLog.textLog("________________________________________________________________________________________________");
}



ZStatus
ZSearchParser::showEntity (ZSearchContext &pContext, ZSearchToken *pTokenName,int pShowType)
{
    if (EntityList.count()==0) {
      ErrorLog.warningLog("showEntity-W-EMPTY            ---no entity to show--.");
      return pContext.Status=ZS_EMPTY;
    }
    int wi=0;
    for (; (wi < EntityList.count()) && (pTokenName->Text != EntityList[wi]->getEntityName());wi++) ;

    if (wi == EntityList.count()){
      ErrorLog.errorLog("showEntity-E-NOTFND Entity <%s> has not been found",pTokenName->Text.toString());
      pContext.LastErroredToken=pTokenName;
      /* suggest symbols */
      ZArray<utf8VaryingString> wSugArray ;
      /* add entities names to suggest */
      for (int wi=0;wi < EntityList.count();wi++)
          wSugArray.push(EntityList[wi]->getEntityName());

      /* try to suggest */
      utf8VaryingString wSuggest=searchAdHocWeighted (pTokenName->Text,wSugArray);
      if (!wSuggest.isEmpty())
          ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );
      return pContext.Status=ZS_INVNAME ;
    }
    if (pShowType & SHENT_Brief) {
        showEntityDetailsBrief(EntityList[wi],pShowType);
    }
    else {
        showEntityDetails(EntityList[wi],pShowType);
    }
  ErrorLog.textLog("________________________________________________________________________________________________");

  return pContext.Status = ZS_SUCCESS;
} // showEntity

const char*
entityType(std::shared_ptr<ZSearchEntity> pEntity)
{
    const char* wEType="unknown entity type";
    if (pEntity->isFile())
        wEType = "File entity";
    else if (pEntity->isCollection())
        wEType = "Collection";
    else if (pEntity->isJoin())
        wEType = "Join";

    return wEType;
}

void
ZSearchParser::showEntityDetailsBrief (std::shared_ptr<ZSearchEntity> pEntity, int pShowType)
{
    ErrorLog.textLog("Entity %s %s %s",
                     pEntity->getEntityName().toString(),
                     pEntity->getEntityFullName().toString(),
                     entityType(pEntity));

    while (true) {
        if (pEntity->isJoin()) {
            ErrorLog.textLog("     Master entity %s %s %s",
                             pEntity->_JoinList[0]->getEntityName().toString(),
                             pEntity->_JoinList[0]->getEntityFullName().toString(),
                             entityType(pEntity->_JoinList[0]));
            ErrorLog.textLog("     slave entity %s %s %s",
                             pEntity->_JoinList[1]->getEntityName().toString(),
                             pEntity->_JoinList[1]->getEntityFullName().toString(),
                             entityType(pEntity->_JoinList[1]));
            break;
        }
        if (pEntity->isCollection()) {
            ErrorLog.textLog("     Base entity %s %s %s",
                             pEntity->_BaseEntity->getEntityName().toString(),
                             pEntity->_BaseEntity->getEntityFullName().toString(),
                             entityType(pEntity->_BaseEntity));
            break;
        }
        if (pEntity->isFile()) {
            ErrorLog.textLog("     File %s <%s>",
                             pEntity->_FileEntity->Name.toString(),
                             pEntity->_FileEntity->getPath().toString());
            break;
        }
        ErrorLog.textLog("Unknown entity type");
        break;
    } // while true

    ErrorLog.textLog("     Build dictionary %d fields\n Local dictionary (selected fields) %d fields",
                     pEntity->BuildDic.count(),pEntity->LocalMetaDic.count());

    utf8VaryingString wFieldList = "      Field selection clause\n";
    int wi = 0 ;
    if (pEntity->BuildDic.count()==0) {
      wFieldList += "<no field selected>";
    }
    else {
        wFieldList.addsprintf("%s ",pEntity->BuildDic[wi]._reportBrief(0).toString());
        for (wi = 1 ; wi < pEntity->BuildDic.count(); wi++) {
            wFieldList += ", ";
            wFieldList.addsprintf("%s ",pEntity->BuildDic[wi]._reportBrief(0).toString());
        }
    }
    ErrorLog.textLog(wFieldList.toCChar());

    if (pEntity->isCollection()) {
        if (pShowType & SHENT_With) {
            ErrorLog.textLog("      With clause");
            if (pEntity->LogicalTerm==nullptr)
                ErrorLog.textLog("         <nullptr>");
            else
                ErrorLog.textLog(pEntity->LogicalTerm->_reportFormula(false).toCChar());
        }
    }// Collection
    else if (pEntity->isJoin()) {
//        CountTerm wCT;
        if (pShowType & SHENT_With) {
            ErrorLog.textLog("     With clause Joined entity :\n"
                             "          Master entity");
            ErrorLog.textLog(pEntity->_JoinList[0]->LogicalTerm->_reportFormula(false).toCChar());
            ErrorLog.textLog("          Slave entity :");
            ErrorLog.textLog(pEntity->_JoinList[1]->LogicalTerm->_reportFormula(false).toCChar());
        }
        if (pShowType & SHENT_Using) {
            if (pEntity->_Using == nullptr)
                ErrorLog.errorLog("     Invalid <Using> clause for a join entity : <nullptr> ");
            else {
                ErrorLog.textLog("     Using clause ");
                ErrorLog.textLog(pEntity->_Using->_reportFormula(false).toCChar());
            }
        }
        /*
        _countLogicalTerm(pEntity->_JoinList[0]->LogicalTerm,wCT);
        ErrorLog.textLog("     master entity :  logical term(s):%d operands:%d operators:%d - arithmetic term(s):%d operands:%d operators:%d\n"
                         "                    end scan status: %s",
                         wCT.LogTerm,wCT.LogOperand,wCT.LogOperator,
                         wCT.ArithTerm,wCT.ArithOperand,wCT.ArithOperator,
                         decode_TSST(wCT.Status));
        wCT.clear();
        _countLogicalTerm(pEntity->_JoinList[1]->LogicalTerm,wCT);
        ErrorLog.textLog("     slave entity :  logical term(s):%d operands:%d operators:%d - arithmetic term(s):%d operands:%d operators:%d\n"
                         "                    end scan status: %s",
                         wCT.LogTerm,wCT.LogOperand,wCT.LogOperator,
                         wCT.ArithTerm,wCT.ArithOperand,wCT.ArithOperator,
                         decode_TSST(wCT.Status));

        wCT.clear();
        _countLogicalTerm(pEntity->_Using,wCT);
        */

    }// isJoin

    if (!pEntity->isJoin()){
        if (pEntity->_Using != nullptr)
            ErrorLog.errorLog("     Invalid <Using> clause : not allowed for a <%s> entity",
                              entityType(pEntity));
    }

//  ErrorLog.textLog("________________________________________________________________________________________________");
} // ZSearchParser::showEntityDetailsBrief

void
ZSearchParser::showEntityWithClause (std::shared_ptr<ZSearchEntity> pEntity,int pDetail)
{
    ErrorLog.textLog("______________________Ranks selection logical term (with clause)_______________________");
    if (pEntity->isJoin()) {
        ErrorLog.textLog("   Master :");
        if (pEntity->_JoinList[0]->hasFormula()) {
            if (pDetail & SHENT_Brief)
                ErrorLog.textLog(pEntity->_JoinList[0]->LogicalTerm->_reportFormula(false).toCChar());
            else
                ErrorLog.textLog(pEntity->_JoinList[0]->LogicalTerm->_reportDetailed(false).toCChar());
        }
        else {
            ErrorLog.textLog("   no with clause for master entity (all ranks selected).");
        }
        ErrorLog.textLog("   Slave :");
        if (pEntity->_JoinList[1]->hasFormula()) {
            if (pDetail & SHENT_Brief)
                ErrorLog.textLog(pEntity->_JoinList[1]->LogicalTerm->_reportFormula(false).toCChar());
            else
                ErrorLog.textLog(pEntity->_JoinList[1]->LogicalTerm->_reportDetailed(false).toCChar());
        }
        else {
            ErrorLog.textLog("   no with clause for slave entity (all ranks selected).");
        }
        return;
    } // isJoin()

    /* either collection or file entity */

    if (pEntity->hasFormula()) {
        if (pDetail & SHENT_Brief)
            ErrorLog.textLog(pEntity->LogicalTerm->_reportFormula(false).toCChar());
        else
            ErrorLog.textLog(pEntity->LogicalTerm->_reportDetailed(false).toCChar());
     }
    else
     ErrorLog.textLog("  with clause missing (all ranks selected)");
}
void
ZSearchParser::showEntityUsingClause (std::shared_ptr<ZSearchEntity> pEntity,int pDetail)
{
    if (pEntity->_Using != nullptr) {
        ErrorLog.textLog("_____________________________ join logical term (using clause) ______________________________");
        if (pDetail & SHENT_Brief)
            ErrorLog.textLog(pEntity->_Using->_reportFormula(false).toCChar());
        else
            ErrorLog.textLog(pEntity->_Using->_reportDetailed(false).toCChar());

        if (!pEntity->isJoin()) {
            ErrorLog.warningLog(" Entity is not a join entity and has an existing using clause");
        }
        return;
     }
    if (!pEntity->isJoin()) {
        return ;
    }

    ErrorLog.warningLog(" Entity is a join entity and has no existing using clause. This may induce disfunctions.");

}

void
ZSearchParser::showEntityBuildDic (std::shared_ptr<ZSearchEntity> pEntity,int pDetail)
{
    ErrorLog.textLog("__________________________ Build dictionary _______________________________");
    ErrorLog.textLog("   |          Field name     |            ZType        |MetaDic rank");
    for (long wj=0;wj < pEntity->BuildDic.count();wj++ ) {
        ErrorLog.textLog("%3d|%25s|%25s|%ld",wj,
                pEntity->BuildDic[wj].getFieldName().toString(),
                decode_ZType(pEntity->BuildDic[wj].getZType()),
                pEntity->BuildDic[wj].getMetaDicRank());
    }
}

void
ZSearchParser::showEntityLocalDic (std::shared_ptr<ZSearchEntity> pEntity,int pDetail)
{
    ErrorLog.textLog("__________________________  Local dictionary   _______________________________");
    ErrorLog.textLog("   |          Field name     |            ZType        ");
    for (long wj=0;wj < pEntity->LocalMetaDic.count();wj++ ) {
        ErrorLog.textLog("%3d|%25s|%25s|",wj,
                pEntity->LocalMetaDic[wj].getName().toString(),
                decode_ZType(pEntity->LocalMetaDic[wj].ZType));
    }
}
void
ZSearchParser::showEntityHeader (std::shared_ptr<ZSearchEntity> pEntity)
{
  const char* wETp ;
  if (pEntity->isFile())
      wETp = "File Entity";
  else if (pEntity->isJoin())
      wETp = "Join Entity";
  else if (pEntity->isCollection())
      wETp = "Collection Entity";
  else
      wETp = "Undefined Entity";

  ErrorLog.textLog("__________________Entity %s %s___________________",
            pEntity->getEntityName().toString(),
            wETp);

} // ZSearchParser::showEntityHeader
void
ZSearchParser::showEntityTrailer (std::shared_ptr<ZSearchEntity> pEntity)
{
  const char* wETp ;
  if (pEntity->isFile())
      wETp = "File Entity";
  else if (pEntity->isJoin())
      wETp = "Join Entity";
  else if (pEntity->isCollection())
      wETp = "Collection Entity";
  else
      wETp = "Undefined Entity";

  ErrorLog.textLog("__________________end of %s %s___________________",
            pEntity->getEntityName().toString(),
            wETp);

} // ZSearchParser::showEntityHeader

void
ZSearchParser::showEntityDetails (std::shared_ptr<ZSearchEntity> pEntity,int pShowType)
{
    if (pEntity->isJoin()) {
        showJoinDetails(pEntity, pShowType);
        return;
    }
    showCollectionDetails(pEntity, pShowType);
}

void
ZSearchParser::showJoinDetails (std::shared_ptr<ZSearchEntity> pEntity,int pShowType)
{
  if (!pEntity->isJoin()) {
      showCollectionDetails(pEntity, pShowType);
      return;
  }
    showEntityHeader(pEntity);

    ErrorLog.textLog("Join entity main data");

    ErrorLog.textLog("\nBuild dictionary\n");

    ErrorLog.textLog("rnk %15s ZSTO ","Field name");
    for (int wi=0; wi < pEntity->BuildDic.count() ; wi++) {
        ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(pEntity->BuildDic[wi].Operand);
        ErrorLog.textLog("%3d %15s %s",
                         wi,
                         wOpBase->FullFieldName.toString(),
                         decode_ZSTO(wOpBase->ZSTO).toString()
                         );

        if (wOpBase->ZSTO & ZSTO_Field) {
            ZSearchFieldOperand* wOp = static_cast<ZSearchFieldOperand*>(pEntity->BuildDic[wi].Operand);
            ErrorLog.textLog("         Entity <%15s> Local dictionary rank <%d> field name <%15s> ZType <%s>",
                            wOp->Entity->getEntityName().toString(),
                            wOp->MetaDicRank,
                            wOp->Entity->LocalMetaDic[wOp->MetaDicRank].getName().toString(),
                            decode_ZType(wOp->Entity->LocalMetaDic[wOp->MetaDicRank].ZType));
        }
        else if (wOpBase->ZSTO & ZSTO_Logical) {
            ZSearchLogicalOperand* wOp = static_cast<ZSearchLogicalOperand*>(pEntity->BuildDic[wi].Operand);
            ErrorLog.textLog("      Logical expression <%s>",wOp->_reportBrief(0).toString());
        }
        else if (wOpBase->ZSTO & ZSTO_Arithmetic) {
            ZSearchArithmeticTerm* wOp = static_cast<ZSearchArithmeticTerm*>(pEntity->BuildDic[wi].Operand);
            ErrorLog.textLog("      Arithmetic expression <%s>",wOp->_reportFormula(true).toString());
        }
    }// for
    ErrorLog.textLog("\nLocal meta dictionary\n");

    for (int wi=0; wi < pEntity->LocalMetaDic.count() ; wi++) {
        ErrorLog.textLog("rnk %15s ZType","Field name");
        ErrorLog.textLog("%3d %15s %s",
                         wi,
                         pEntity->LocalMetaDic[wi].getName().toString(),
                         decode_ZType(pEntity->LocalMetaDic[wi].ZType)
                         );

        ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(pEntity->BuildDic[wi].Operand);

        ErrorLog.textLog("rnk %15s ZSTO ","Field name");
        ErrorLog.textLog("%3d %15s %s",
                         wi,
                         wOpBase->FullFieldName.toString(),
                         decode_ZSTO(wOpBase->ZSTO).toString()
                         );

        if (wOpBase->ZSTO & ZSTO_Field) {
            ZSearchFieldOperand* wOp = static_cast<ZSearchFieldOperand*>(pEntity->BuildDic[wi].Operand);
            ErrorLog.textLog("         Entity <%15s> Local dictionary rank <%d> field name <%15s> ZType <%s>",
                            wOp->Entity->getEntityName().toString(),
                            wOp->MetaDicRank,
                            wOp->Entity->LocalMetaDic[wOp->MetaDicRank].getName().toString(),
                            decode_ZType(wOp->Entity->LocalMetaDic[wOp->MetaDicRank].ZType));
        }

    }// for


    ErrorLog.textLog("master entity <%s> <%s>\n",
                     pEntity->_JoinList[0]->getEntityName().toString(),entityType(pEntity->_JoinList[0]));


    if (pShowType & SHENT_With) {
        ErrorLog.textLog("______________________With clause_______________________________\n"
                         "Master");
        ErrorLog.textLog(pEntity->_JoinList[0]->LogicalTerm->_reportFormula(false).toCChar());
        ErrorLog.textLog("Slave");
        ErrorLog.textLog(pEntity->_JoinList[1]->LogicalTerm->_reportFormula(false).toCChar());
        ErrorLog.textLog("_________________________________________________________________");
    }
    if (pShowType & SHENT_Dictionaries) {
        showEntityLocalDic(pEntity->_JoinList[0],0);
        showEntityBuildDic(pEntity->_JoinList[0],0);

    }

    ErrorLog.textLog("slave  entity <%s> <%s>\n",
                     pEntity->_JoinList[1]->getEntityName().toString(),entityType(pEntity->_JoinList[1]));

    if (pShowType & SHENT_Dictionaries) {
        showEntityLocalDic(pEntity->_JoinList[1],0);
        showEntityBuildDic(pEntity->_JoinList[1],0);
    }

  showEntityTrailer(pEntity);
} // ZSearchParser::showEntityDetails

void
ZSearchParser::showCollectionDetails (std::shared_ptr<ZSearchEntity> pEntity,int pShowType)
{
  showEntityHeader(pEntity);

    if (pShowType & SHENT_With) {
       showEntityWithClause(pEntity,pShowType);
    }
    if (pShowType & SHENT_Dictionaries) {
        showEntityLocalDic(pEntity,pShowType);

        showEntityBuildDic(pEntity,pShowType);
    }

  showEntityTrailer(pEntity);
} // ZSearchParser::showEntityDetails

int
analyzeSearchField(const utf8VaryingString& pFieldName, utf8VaryingString& pToSearch)
{
    int wSType = 0 ; /* 0 : exact - 1 : starts with - 2 : ends with - 3 : contains - 4 : all - 8 : is like <~> sign */
/* analyze field name to search */

    utf8_t* wPtr=pFieldName.Data;
    utf8_t* wPtrStart ;
    utf8_t* wPtrEnd ;

    if (pFieldName.isEmpty()) {
        pToSearch.clear();
        return 4;
    }

    /* skip leading spaces and other possible bulshits */
    while ((*wPtr)&& !utfIsAsciiChar(*wPtr) && utfIsSpace(*wPtr))
        wPtr++;

    wPtrStart = wPtr;
    if (*wPtr=='~') {
        wSType = 8 ;    /* is like */
        wPtrStart ++;
        wPtrEnd = wPtrStart;
        while ((*wPtrEnd)&& utfIsAsciiChar(*wPtrEnd) && !utfIsSpace(*wPtrEnd))
            wPtrEnd++;

        goto analyzeSearchField_End;  /* extract search string and exit */
    }


    if (*wPtr=='*') {
        wSType = 2 ;    /* ends with : wild card is first character */
        wPtrStart ++ ;
        if (!*wPtr || utfIsSpace(*wPtr)) {
            pToSearch.clear();
            return 4;   /* no other character than wildcard : all */
        }
    }
    wPtr++;
    while ( *wPtr && (*wPtr!='*') && !utfIsSpace(*wPtr) && utfIsAsciiChar(*wPtr)  )
        wPtr++;

    wPtrEnd = wPtr;
    if (*wPtr=='*') {
        wSType += 1 ; /* starts with : wild card is last character ( or contains )*/
//        wPtrEnd -- ;
    }
analyzeSearchField_End:
    /* copy substring to search */

    utf8_t* wPtrOut = pToSearch.allocateUnitsBZero(wPtrEnd-wPtrStart+1);

    while (wPtrStart < wPtrEnd) {
        *wPtrOut = *wPtrStart;
        wPtrStart++;
        wPtrOut++;
    }

    return wSType;
} // analyzeSearchField

ZStatus
ZSearchParser::showEntityField (ZSearchContext& pContext,std::shared_ptr<ZSearchEntity> pEntity,const utf8VaryingString& pFieldName)
{
    ErrorLog.textLog("__________________Entity %s %s___________________",
            pEntity->getEntityName().toString(),
            pEntity->isFile()?"File entity":"Collection");

    if (pEntity==nullptr) {
        ErrorLog.errorLog("ZSearchParser::showEntityField-E-NULL Given Entity is invalid or null at line %d column %d.",
            pContext.previousToken()->TokenLine,pContext.previousToken()->TokenColumn);
            pContext.LastErroredToken = pContext.previousToken();
        return pContext.Status=ZS_NULLPTR;
    }

    if (pEntity->LocalMetaDic.count()==0) {
        ErrorLog.errorLog("ZSearchParser::showEntityField-E-EMPTY Given Entity <%s> has no local dictionary at line %d column %d.",
            pContext.previousToken()->Text.toString(), pContext.previousToken()->TokenLine,pContext.previousToken()->TokenColumn);
          pContext.LastErroredToken = pContext.previousToken();
        return pContext.Status=ZS_EMPTY;
    }

    /* 0 : exact - 1 : starts with - 2 : ends with - 3 : contains */
    utf8VaryingString wSearch;
    int wSType = analyzeSearchField(pFieldName,wSearch); /* analyze field name to search */

    switch (wSType)
    {
    case 0:  /* exact match */
    {
        int wi=0;
        for (;wi < pEntity->LocalMetaDic.count();wi++) {
            if (pEntity->LocalMetaDic[wi].getName()==pFieldName)
                break;
        }
        if (wi >= pEntity->LocalMetaDic.count()) {
            ErrorLog.errorLog("Entity %s has no field named %s",
                    pEntity->getEntityName().toString(),
                    pFieldName.toString());
            zbs::ZArray<utf8VaryingString> wFieldNameList;
            for (long wi=0; wi < pEntity->LocalMetaDic.count();wi++)
                wFieldNameList.push(pEntity->LocalMetaDic[wi].getName());

            utf8VaryingString wApprox = searchAdHocWeighted(pFieldName,wFieldNameList);
            if (!wApprox.isEmpty()) {
                ErrorLog.textLog("Do you mean <%s> ?", wApprox.toString() );
            }
            return pContext.Status=ZS_NOTFOUND;
        }
        _displayOneFieldDetail(pEntity->LocalMetaDic[wi]);
        return pContext.Status=ZS_SUCCESS;
    } // case 0

    case 1:  /* starts with */
    {
        bool wFound=false;
        int wi=0;
        for (;wi < pEntity->LocalMetaDic.count();wi++) {
            if (pEntity->LocalMetaDic[wi].getName().startsWithT<utf8VaryingString>(wSearch)) {
                _displayOneFieldDetail(pEntity->LocalMetaDic[wi]);
                wFound=true;
            }
        }
        if (!wFound) {
            ErrorLog.errorLog("Entity %s has no field whose name starts with %s",
                    pEntity->getEntityName().toString(),
                    wSearch.toString());
            zbs::ZArray<utf8VaryingString> wFieldNameList;
            for (long wi=0; wi < pEntity->LocalMetaDic.count();wi++)
                wFieldNameList.push(pEntity->LocalMetaDic[wi].getName());

            utf8VaryingString wApprox = searchAdHocWeighted(pFieldName,wFieldNameList);
            if (!wApprox.isEmpty()) {
                ErrorLog.textLog("Do you mean <%s> ?", wApprox.toString() );
            }
            pContext.LastErroredToken = pContext.TokenIdentifier;
            return pContext.Status=ZS_NOTFOUND;
        }

        return pContext.Status=ZS_SUCCESS;
    } // case 1

    case 2:  /* ends with */
    {
        bool wFound=false;
        int wi=0;
        for (;wi < pEntity->LocalMetaDic.count();wi++) {
            if (pEntity->LocalMetaDic[wi].getName().endsWithT<utf8VaryingString>(wSearch)) {
                _displayOneFieldDetail(pEntity->LocalMetaDic[wi]);
                wFound=true;
        }
        }
        if (!wFound) {
            ErrorLog.errorLog("Entity %s has no field whose name ends with <%s>",
                    pEntity->getEntityName().toString(),
                    wSearch.toString());
            zbs::ZArray<utf8VaryingString> wFieldNameList;
            for (long wi=0; wi < pEntity->LocalMetaDic.count();wi++)
                wFieldNameList.push(pEntity->LocalMetaDic[wi].getName());

            utf8VaryingString wApprox = searchAdHocWeighted(pFieldName,wFieldNameList);
            if (!wApprox.isEmpty()) {
                ErrorLog.textLog("Do you mean <%s> ?", wApprox.toString() );
            }
            return pContext.Status=ZS_NOTFOUND;
        }
        return pContext.Status=ZS_SUCCESS;
    } // case 2

    case 3:  /* contains */
    {
        bool wFound=false;
        int wi=0;
        for (;wi < pEntity->LocalMetaDic.count();wi++) {
            if (pEntity->LocalMetaDic[wi].getName().containsT<utf8VaryingString>(wSearch)) {
                _displayOneFieldDetail(pEntity->LocalMetaDic[wi]);
                wFound=true;
            }
        }
        if (!wFound) {
            ErrorLog.errorLog("Entity %s has no field whose name contains %s",
                    pEntity->getEntityName().toString(),
                    wSearch.toString());
            zbs::ZArray<utf8VaryingString> wFieldNameList;
            for (long wi=0; wi < pEntity->LocalMetaDic.count();wi++)
                wFieldNameList.push(pEntity->LocalMetaDic[wi].getName());

            utf8VaryingString wApprox = searchAdHocWeighted(pFieldName,wFieldNameList);
            if (!wApprox.isEmpty()) {
                ErrorLog.textLog("Do you mean <%s> ?", wApprox.toString() );
            }
            return pContext.Status=ZS_NOTFOUND;
        }
        return pContext.Status=ZS_SUCCESS;
    } // case 3

    case 4:  /* all */
    {
        int wi=0;
        for (;wi < pEntity->LocalMetaDic.count();wi++) {
            _displayOneFieldDetail(pEntity->LocalMetaDic[wi]);
        }

        return pContext.Status=ZS_SUCCESS;
    } // case 4

    case 8:  /* is like */
    {
        CWeightList wWeightList;
        CMatchResult wR;
        int wMatch=0;
        for (int wi=0;wi < pEntity->LocalMetaDic.count();wi++) {
            if(matchWeight(wR,pEntity->LocalMetaDic[wi].getName(),wSearch)) {
                wWeightList.add(wR,wi);
            }
        }
        if (wWeightList.count() == 0) {
            ErrorLog.errorLog("Entity %s has no field whose name is like <%s>",
                    pEntity->getEntityName().toString(),
                    wSearch.toString());
            pContext.LastErroredToken = pContext.TokenIdentifier;
            return pContext.Status=ZS_NOTFOUND;
        }

        for (int wi=0;wi < wWeightList.count();wi++) {
//            _displayOneFieldDetail(pEntity->LocalMetaDic[wWeightList[wi].Index]);
            ErrorLog.textLog("%25s %25s [match %d radix %d]",
                    pEntity->LocalMetaDic[wWeightList[wi].Index].getName().toString(),
                    decode_ZType( pEntity->LocalMetaDic[wWeightList[wi].Index].ZType),
                    wWeightList[wi].Match,wWeightList[wi].Radix);
            }

        return pContext.Status=ZS_SUCCESS;
    } // case 4
    } // switch

} // ZSearchParser::showEntityField



ZStatus
ZSearchParser::_finish()
{

/* release all created collections */
  return ZS_SUCCESS;
}

/* for <entity name> with < selection clause > set  <entity name>.<field name> = <value> , <entity name>.<field name> = <value> ; */

ZStatus
ZSearchParser::_parseFor(ZSearchContext& pContext)
{
  return ZS_SUCCESS;
}

/*
    save symbol ;
    save symbol to <xml file path> ;

    save instruction ;
*/
ZStatus
ZSearchParser::_parseSave(ZSearchContext& pContext)
{
    ZStatus wSt=ZS_SUCCESS;

    if (!pContext.advanceIndex()) /* skip save key word */
        return ZS_SYNTAX_ERROR;

    switch (pContext.CurrentToken->Type)
    {
    case ZSRCH_SYMBOL: {

        if ((!pContext.advanceIndex())||(pContext.CurrentToken->Type==ZSRCH_SEMICOLON)) {
            uriString wXmlPath = __SEARCHPARSER_SYMBOL_FILE__;
            bool wExists=wXmlPath.exists();
            wSt = SymbolList.XmlSave(wXmlPath, &ErrorLog);
            if (wSt==ZS_SUCCESS) {
                ErrorLog.textLog("Symbol file %s successfully %s",wXmlPath.toString(),wExists?"replaced":"saved");
                return wSt;
            }
            ErrorLog.errorLog("Cannot write symbol file %s status %s",wXmlPath.toString(),decode_ZStatus(wSt));
            return wSt;
        }
        if (pContext.CurrentToken->Type!=ZSRCH_TO) {
            ErrorLog.errorLog("Expected keyword TO. Found %s instead line %d column %d",
                     pContext.CurrentToken->Text.toString(), pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
            pContext.LastErroredToken=pContext.CurrentToken;
            return ZS_MISS_KEYWORD;
        }
        if (pContext.CurrentToken->Type!=ZSRCH_STRING_LITERAL) {
            ErrorLog.errorLog("Expected xml file path. Found %s instead line %d column %d",
                     pContext.CurrentToken->Text.toString(), pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
            pContext.LastErroredToken=pContext.CurrentToken;
            return ZS_MISS_KEYWORD;
        }
        uriString wXmlPath = pContext.CurrentToken->Text;
        bool wExists=wXmlPath.exists();
        wSt = SymbolList.XmlSave(wXmlPath, &ErrorLog);
        if (wSt==ZS_SUCCESS) {
            ErrorLog.textLog("Symbol file %s successfully %s",wXmlPath.toString(),wExists?"replaced":"saved");
            return wSt;
        }
        ErrorLog.errorLog("Cannot write symbol file %s status %s",wXmlPath.toString(),decode_ZStatus(wSt));
        return wSt;
    }//ZSRCH_SYMBOL
    default:
        ErrorLog.errorLog("Expected one of {symbol,... }. Found %s instead line %d column %d",
                 pContext.CurrentToken->Text.toString(), pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken=pContext.CurrentToken;
        return ZS_MISS_KEYWORD;
    }

    while ((pContext.Index < ZSearchTokenizer::count())&&(pContext.CurrentToken->Type!=ZSRCH_SEMICOLON)) {
//        CurrentToken=pContext.CurrentToken;
        while (true) {
            if(pContext.CurrentToken->Type==ZSRCH_STRING_LITERAL)
                break;
        }
    }
    return ZS_SUCCESS;
} // _parseSave

/*                  <---field selection clause ---->                         <-------record selection clause------------------->
                    [<entity name>.]<field name>,...
  find  [all,first]                                     in <entity name> with <entity name>.<field name> == <value> [and,or] ... [as <collection name>] ;
                    [ <entity name>.]*

    all = all records corresponding to selection clause
    first = first record corresponding to selection clause


field selection :
<field name>

<field name>.decode (<entity name 1>,<match field name>,<resulting field name>,<comparizon operator>)


decode field modifier :

search within already declared and stored <entity name 1>
        for value fo <field name 0> from current selection to match <field name 1> according <comparizon operator>
                then returns <field name 2>

 */

/* Field selection clause

extracts selected field identifier either under the form of

Global syntax
    <field selection>,<field selection>,...
Field selection
   <field>
   <field>.<modifier>

  <numeric literal>.<field>
  <numeric literal> must correspond to a valid rank of a selected entities + 1.

  <entity>.<field>

  <entity>.<field>.<modifier>
  <num lit>.<field>.<modifier>

  <modifer> itself and its possible arguments will be extracted by callee routine (i. e. _parseOperandField())
*/


ZStatus
ZSearchParser::_parseSelClause(ZSearchContext& pContext)
{

    ZStatus wSt=ZS_SUCCESS;

    if ((pContext.CurrentToken->Type != ZSRCH_IDENTIFIER)&&
        (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL)&&
        (pContext.CurrentToken->Type != ZSRCH_WILDCARD)) {
        ErrorLog.errorLog("Expected field name , entity identifier , literal or wild card. Found <%s> at line %d column %d.",
                          pContext.CurrentToken->Text.toString(),
                          pContext.CurrentToken->TokenLine,
                          pContext.CurrentToken->TokenColumn );
        pContext.LastErroredToken=pContext.CurrentToken;
        return pContext.Status=ZS_MISS_FIELD;
    }

    ZSearchField wFld;
    bool wHasAllFields=false;
    std::shared_ptr<ZSearchEntity> wSourceEntity=nullptr;

    while (pContext.notEOF() && !pContext.hasCurrent(ZSRCH_IN) )  // loop until ZSRCH_IN
    {
        wFld.clear();
        while (true) {
        /* if nested check entity name vs target entity _BaseEntity if collection or _JoinList entities referential */
        if (pContext.isNested()) {

            if (pContext.TargetEntity->isCollection()) {
                if (pContext.CurrentToken->Text != pContext.TargetEntity->_BaseEntity->getEntityName()) {
                    ErrorLog.errorLog("Entity identifier <%s> is invalid (entity name is <%s>) at line %d column %d.",
                                      pContext.CurrentToken->Text.toString(),
                                      pContext.TargetEntity->getEntityName(),
                                      pContext.CurrentToken->TokenLine,
                                      pContext.CurrentToken->TokenColumn );
                    pContext.LastErroredToken = pContext.CurrentToken;
                    return ZS_INV_ENTITY;
                }
                wSourceEntity = pContext.TargetEntity->_BaseEntity;
            }// collection
            else if (pContext.TargetEntity->isJoin()) {
                int wi=0;
                for (; wi < pContext.TargetEntity->_JoinList.count();wi++) {
                    if (pContext.CurrentToken->Text == pContext.TargetEntity->_JoinList[wi]->getEntityName()) {
                        wSourceEntity = pContext.TargetEntity->_JoinList[wi];
                        break;
                    }
                }// for
                if (wSourceEntity == nullptr) {
                    ErrorLog.errorLog("Entity identifier <%s> is not found in entity list of join entity <%s> at line %d column %d.",
                                      pContext.CurrentToken->Text.toString(),
                                      pContext.TargetEntity->getEntityName(),
                                      pContext.CurrentToken->TokenLine,
                                      pContext.CurrentToken->TokenColumn );
                    pContext.LastErroredToken = pContext.CurrentToken;
                    return ZS_INV_ENTITY;
                }
            }// join
            /* In case of file entity : entity name is target entity name */
            else if (pContext.TargetEntity->isFile()) {
                if (pContext.CurrentToken->Text != pContext.TargetEntity->getEntityName()) {
                    ErrorLog.errorLog("Entity identifier <%s> is invalid (entity name is <%s>) at line %d column %d.",
                                      pContext.CurrentToken->Text.toString(),
                                      pContext.TargetEntity->getEntityName(),
                                      pContext.CurrentToken->TokenLine,
                                      pContext.CurrentToken->TokenColumn );
                    pContext.LastErroredToken = pContext.CurrentToken;
                    return ZS_INV_ENTITY;
                }
                wSourceEntity = pContext.TargetEntity ;
            } // file

            pContext.advanceIndex(false);
            pContext.advanceIndex(false);  /* availability of tokens is checked by isNested() */

            if (pContext.CurrentToken->Type == ZSRCH_WILDCARD) {
                /* if all fields from source entity are selected, align both BuilDic and LocalMetaDic to source */
                pContext.TargetEntity->BuildDic.setEqualTo(wSourceEntity);

                for (int wi=0; wi < wSourceEntity->LocalMetaDic.count();wi++) {
                    pContext.TargetEntity->LocalMetaDic.push(wSourceEntity->LocalMetaDic[wi]);
                }
                break;
            }
            zrank_type wRank=wSourceEntity->LocalMetaDic.searchFieldByName(pContext.CurrentToken->Text);
            if (wRank < 0) {
                ErrorLog.errorLog("Field identifier <%s> is invalid within entity <%s> at line %d column %d.",
                                  pContext.CurrentToken->Text.toString(),
                                  wSourceEntity->getEntityName(),
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn );
                pContext.LastErroredToken = pContext.CurrentToken;
                return ZS_INV_FIELD;
            }
            pContext.TargetEntity->BuildDic.push(ZSearchField(wSourceEntity,&wSourceEntity->LocalMetaDic,wRank));
            pContext.TargetEntity->LocalMetaDic.push(wSourceEntity->LocalMetaDic[wRank]);
            ErrorLog.infoLog("Field <%s> from entity <%s> added to local meta dic of entity <%s> at line %d column %d.",
                              pContext.CurrentToken->Text.toString(),
                              pContext.TargetEntity->_BaseEntity->getEntityName(),
                             pContext.TargetEntity->getEntityName(),
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn );

            break;
        } // isNested

        /* here identifier is not a nested field descriptor */
        int wRank=-1;

        if (pContext.TargetEntity->isJoin()) {
            ErrorLog.warningLog("When parsing a joined entity as <%s> is, field descriptor like <%s> has to be nested as <entity name>::%s at line %d column %d.",
                                pContext.TargetEntity->getEntityName(),
                                pContext.CurrentToken->Text.toString(),
                                pContext.CurrentToken->Text.toString(),
                                pContext.CurrentToken->TokenLine,
                                pContext.CurrentToken->TokenColumn );
            if (pContext.hasCurrent(ZSRCH_WILDCARD)) {
                ErrorLog.errorLog("Entity <%s> is a join entity and wildcard cannot be used without nested description at line %d column %d.",
                                  pContext.TargetEntity->getEntityName(),
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn );
                pContext.LastErroredToken = pContext.CurrentToken;
                return ZS_INV_FIELD;
            }
            int wi=0;
            wSourceEntity = nullptr;
            for (; wi < pContext.TargetEntity->_JoinList.count();wi++) {
                for (int wj=0; wj < pContext.TargetEntity->_JoinList[wi]->LocalMetaDic.count(); wj++)
                    if (pContext.CurrentToken->Text == pContext.TargetEntity->_JoinList[wi]->LocalMetaDic[wj].getName()) {
                        wSourceEntity = pContext.TargetEntity->_JoinList[wi];
                        wRank=wj;
                        break;
                } // for
            } // for
            if (wRank < 0) {
                ErrorLog.errorLog("Field <%s> has not been found in any joined entity local dictionary of <%s> at line %d column %d.",
                                  pContext.CurrentToken->Text.toString(),
                                  pContext.TargetEntity->getEntityName(),
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn );
                pContext.LastErroredToken = pContext.CurrentToken;
                return ZS_INV_FIELD;
            }
            pContext.TargetEntity->BuildDic.push(ZSearchField(wSourceEntity,&wSourceEntity->LocalMetaDic,wRank));
            ErrorLog.infoLog("Field %s::%s added to local meta dic at line %d column %d.",
                             wSourceEntity->getEntityName(),
                             pContext.CurrentToken->Text.toString(),
                             pContext.CurrentToken->TokenLine,
                             pContext.CurrentToken->TokenColumn );
            break;

        } // is join

        /* here remains only Collection and File */

        if (pContext.TargetEntity->isCollection()) {
            wSourceEntity = pContext.TargetEntity->_BaseEntity;
        } // Collection
        else if (pContext.TargetEntity->isFile()) {
            wSourceEntity = pContext.TargetEntity ;
        } // file
        /* Nota bene : target entity type cannot be file entity (defined by DECLARE FILE clause) */

        if (pContext.hasCurrent(ZSRCH_WILDCARD)) {  /* Join is not allowed there : only Collection or File */
            pContext.TargetEntity->BuildDic.setEqualTo(wSourceEntity);            
            for (int wi = 0 ; wi < wSourceEntity->LocalMetaDic.count(); wi++) {
               pContext.TargetEntity->LocalMetaDic.push(wSourceEntity->LocalMetaDic[wi]);
            }

            pContext.advanceIndex(false);
            if (pContext.hasCurrent( ZSRCH_COMMA ))
                pContext.advanceIndex(false);
            wHasAllFields = true;

            ErrorLog.infoLog("All fields are selected from entity <%s>",wSourceEntity->getEntityName().toString());

            break ;
        } // ZSRCH_WILDCARD

        for (int wi=0; wi < wSourceEntity->LocalMetaDic.count(); wi++)
            if (pContext.CurrentToken->Text == wSourceEntity->LocalMetaDic[wi].getName()) {
                wRank=wi;
                break;
        } // for
        if (wRank < 0) {
            ErrorLog.errorLog("Field <%s> has not been found in local dictionary of entity <%s> at line %d column %d.",
                              pContext.CurrentToken->Text.toString(),
                              wSourceEntity->getEntityName(),
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn );
            pContext.LastErroredToken = pContext.CurrentToken;
            return ZS_INV_FIELD;
        }

        wFld = ZSearchField(wSourceEntity,&wSourceEntity->LocalMetaDic,wRank);
        ErrorLog.infoLog("Field %s::%s added to local meta dic of entity <%s> at line %d column %d.",
                         wSourceEntity->getEntityName(),
                         pContext.CurrentToken->Text.toString(),
                         pContext.TargetEntity->getEntityName(),
                         pContext.CurrentToken->TokenLine,
                         pContext.CurrentToken->TokenColumn );
        break;
        } // while true

        if (wHasAllFields)  /* if wild card parse possible literals or expressions */
            continue;

        ZSearchOperandBase* wFOp=static_cast<ZSearchOperandBase*>(wFld.Operand);
        if (wFOp->hasModifier()) {
            wSt = _parseModifier(wFOp , pContext);
            if (wSt!=ZS_SUCCESS) {
                return wSt;
            }
        }
        pContext.TargetEntity->BuildDic.push(wFld);
        pContext.TargetEntity->LocalMetaDic.push(wFld.buildFieldDescription());

        pContext.advanceIndex(false);
        if (pContext.hasCurrent( ZSRCH_COMMA ))
            pContext.advanceIndex(false);
    } // while not eof ....

    return ZS_SUCCESS;

} // ZSearchParser::_parseSelClause

/*
  extracts one field identifier and returns a ZFieldOperand

    Field syntax may be either under the form of

field expression

  <entity>.<field>.<modifier>

  <entity>.<field>

  <field>.<modifier>
  <field>

  NB: <modifer> itself and its possible arguments will be extracted by callee routine (i. e. _parseOperandField())

  <arithmetic expression>

        <literal> \          / <literal>
           or      <operator>    or
        <field>   /          \  <field>


  DECODE (<field expression>,<entity>,<key field name>, <resulting field>)

*/
ZStatus
ZSearchParser::_parseFieldIdentifier (ZSearchContext& pContext, ZSearchFieldOperand *pFOD)
{
 utf8VaryingString wEntityName;
 std::shared_ptr<ZSearchEntity> wSourceEntity=nullptr;
 int wRank=0;

    if (pContext.CurrentToken->Type != ZSRCH_IDENTIFIER) {
       ErrorLog.errorLog("Missing field or entity identifier. Found <%s> at line %d column %d.",
           pContext.CurrentToken->Text.toString(),
           pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
       pContext.LastErroredToken = pContext.CurrentToken;

       return pContext.Status=ZS_MISS_FIELD;
    }
    while (true) {
    /* if next is dot and then identifier , then previous is ZEntity name as prefix and field name follows dot  */
        if (pContext.isNestedField()) {
        /* check entity name is one of used in phrase
         * if multiple entities in pharse, scroll
         */
            while (true) {
                if (pContext.TargetEntity->isJoin()) {

                    for (int wi=0; wi < pContext.TargetEntity->_JoinList.count();wi++) {
                        utf8VaryingString wEName = pContext.TokenIdentifier->Text;
                        wEName += "$";
                        wEName += pContext.CurrentToken->Text ;
                        if (wEName == pContext.TargetEntity->_JoinList[wi]->getEntityName()) {
                            wSourceEntity = pContext.TargetEntity->_JoinList[wi];
                            break;
                        }
                    }// for
                    if (wSourceEntity==nullptr) {
                        ErrorLog.errorLog("Wrong entity identifier <%s> for join entity <%s> at line %d column %d.",
                                          pContext.CurrentToken->Text.toString(),
                                          pContext.TargetEntity->getEntityName(),
                                          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
                        pContext.LastErroredToken = pContext.CurrentToken;
                        return pContext.Status=ZS_INV_ENTITY;
                    }
                    break;
                } // join
                /* here entity is Collection */

                if (pContext.CurrentToken->Text != pContext.TargetEntity->_BaseEntity->getEntityName()) {
                    ErrorLog.errorLog("Wrong entity identifier. Found <%s> at line %d column %d.",
                             pContext.CurrentToken->Text.toString(),
                             pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
                    pContext.LastErroredToken = pContext.CurrentToken;
                    return pContext.Status=ZS_INV_ENTITY;
                }
                wSourceEntity = pContext.TargetEntity->_BaseEntity;
                break;
            } // while true
            /* selected entity is Source entity (of no use if only one entity) */

            /* advance twice to point to possible field identifier */
            if (!pContext.advanceIndex(true))/* skip entity identifier */
                return pContext.Status=ZS_SYNTAX_ERROR;
            if (!pContext.advanceIndex(true))/* skip entity separator sign to point to field identifier */
                return pContext.Status=ZS_SYNTAX_ERROR;
            break;
        } // nested

    /* not nested then target entity should be a Collection */
        /* if not nested but join entity : emit warning */
        if (pContext.TargetEntity->isJoin()) {
            ErrorLog.warningLog("When parsing a joined entity as <%s> is, field descriptor like <%s> has to be nested as <entity name>::%s at line %d column %d.",
                                pContext.TargetEntity->getEntityName(),
                                pContext.CurrentToken->Text.toString(),
                                pContext.CurrentToken->Text.toString(),
                                pContext.CurrentToken->TokenLine,
                                pContext.CurrentToken->TokenColumn );
            if (pContext.hasCurrent(ZSRCH_WILDCARD)) {
                ErrorLog.errorLog("Entity <%s> is a join entity and wildcard cannot be used without nested description at line %d column %d.",
                                  pContext.TargetEntity->getEntityName(),
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn );
                pContext.LastErroredToken = pContext.CurrentToken;
                return pContext.Status=ZS_INV_FIELD;
            }
            int wi=0;
            wRank=-1;
            wSourceEntity = nullptr;
            for (; wi < pContext.TargetEntity->_JoinList.count();wi++) {
                for (int wj=0; wj < pContext.TargetEntity->_JoinList[wi]->LocalMetaDic.count(); wj++)
                    if (pContext.CurrentToken->Text == pContext.TargetEntity->_JoinList[wi]->LocalMetaDic[wj].getName()) {
                        wSourceEntity = pContext.TargetEntity->_JoinList[wi];
                        wRank=wj;
                        break;
                } // for
            } // for
            if (wRank < 0) {
                ErrorLog.errorLog("Field <%s> has not been found in any joined entity local dictionary of <%s> at line %d column %d.",
                                  pContext.CurrentToken->Text.toString(),
                                  pContext.TargetEntity->getEntityName(),
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn );
                pContext.LastErroredToken = pContext.CurrentToken;
                return pContext.Status=ZS_INV_FIELD;
            }
            break;
        } // is join
        wRank = -1 ;
        if ( pContext.TargetEntity->isFile() )
            wSourceEntity = pContext.TargetEntity;
        else
        /* here remains only Collection  */
            wSourceEntity = pContext.TargetEntity->_BaseEntity;
        break;
    } // while true

     /* here this is a supposed valid field description : search it in current entity local dictionary (generated from field extraction using BuildDic) */


    pFOD->Entity = wSourceEntity;
    pFOD->MetaDicRank=-1;
    int wi=0;
    for (;wi < wSourceEntity->LocalMetaDic.count();wi++) {
         if (wSourceEntity->LocalMetaDic[wi].getName()==pContext.CurrentToken->Text) {
            pFOD->MetaDicRank=wi;
            break;
         }
    } // for

     if (pFOD->MetaDicRank < 0) {
       ErrorLog.errorLog("Wrong field name <%s> for entity <%s> (field not found in entity dictionary) at line %d column %d.",
           pContext.CurrentTokenList.Tab(pContext.Index+2)->Text.toString(),
           wSourceEntity->getEntityName().toString(),
           pContext.CurrentTokenList.Tab(pContext.Index+2)->TokenLine,pContext.CurrentTokenList.Tab(pContext.Index+2)->TokenColumn );
         pContext.LastErroredToken = pContext.CurrentToken;
       pFOD->MetaDicRank=-1;
       return pContext.Status=ZS_MISS_FIELD;
     }

    pFOD->MetaDicPtr = &wSourceEntity->LocalMetaDic; /* store a pointer to Local MetaDic object */

    ZSearchOperandType_type wZSTO = getZSTOFromZType(wSourceEntity->LocalMetaDic[pFOD->MetaDicRank].ZType);
    pFOD->setZSTO(wZSTO | ZSTO_Field);

//     pFOD->FieldDescription = pContext.SourceEntity->LocalMetaDic[pFOD->ZSEDicRank].getFieldDescriptionPtr();
     pFOD->FullFieldName = wSourceEntity->EntityFullName;
     pFOD->FullFieldName += __SEARCH_ENTITY_SEPARATOR_ASC__ ;
     pFOD->FullFieldName += wSourceEntity->LocalMetaDic[pFOD->MetaDicRank].getName();

     _DBGPRINT("ZSearchParser::_parseFieldIdentifier Found field operand <%s>\n"
               "        ZType <%s> ZSTO Type<%s>  Dictionary rank %ld \n",
               pFOD->FullFieldName.toString(),
               decode_ZType(pFOD->getZType()),
               decode_OperandType(pFOD->ZSTO), pFOD->MetaDicRank
               )
     pContext.advanceIndex(false);

     /* check if modifier */
     if (pContext.isEOF() || (pContext.CurrentToken->Type != __MODIFIER_SEPARATOR__)) {
         return pContext.Status=ZS_SUCCESS;  /* no modifier */
     }
     if (!pContext.advanceIndex(true)) {/* skip field identifier */
       return pContext.Status=ZS_SYNTAX_ERROR;
     }
     if ((pContext.CurrentToken->Type & ZSRCH_MODIFIER)!=ZSRCH_MODIFIER) {
         ErrorLog.errorLog("<%s> has not been found in any joined entity local dictionary of <%s> at line %d column %d.",
                           pContext.CurrentToken->Text.toString(),
                           pContext.TargetEntity->getEntityName(),
                           pContext.CurrentToken->TokenLine,
                           pContext.CurrentToken->TokenColumn );
         pContext.LastErroredToken = pContext.CurrentToken;
         return pContext.Status=ZS_INV_MODIFIER ;
     }

     return pContext.Status=_parseModifier(pFOD,pContext);
} // ZSearchParser::_parseFieldIdentifier



/**
 * @brief ZSearchParser::_parseModifier   pContext.Index must point after ZSRC_DOT token directly to Modifier token
 *
 * This routine changes the operand base type ZSTO according the result of modifier action.
 *
 */
ZStatus
ZSearchParser::_parseModifier(ZSearchOperandBase* pOB,ZSearchContext& pContext)
{
  if ((pContext.CurrentToken->Type & ZSRCH_MODIFIER)!=ZSRCH_MODIFIER) {
      ErrorLog.errorLog("Expecting modifier. Possibly there is a mismatch with entity,field name,modifier. Found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_SYNTAX_ERROR;
  }

  ZSearchOperandType_type wBaseZSTO = pOB->ZSTO & ZSTO_BaseMask;

  switch (pContext.CurrentToken->Type ) {
  /* <field name>.DECODE(<Entity name>,<key field name>,<target field name> [,<compare logical operator>=EQUAL] )
   * <Entity name> must be a current entity
   * <field name>   must have the same search operand type (ZSTO) given by pOB->ZSTO
   */
  case ZSRCH_TRANSLATE :
  {
      pOB->ModifierType = pContext.CurrentToken->Type;
      if (!pContext.advanceIndex()) {
          return pContext.Status=ZS_SYNTAX_ERROR;
      }
      if(pContext.CurrentToken->Type!=ZSRCH_OPENPARENTHESIS) {
          ErrorLog.errorLog("Wrong modifier syntax. Expecting open parenthesis found <%s> at line %d column %d ",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_MISS_PUNCTSIGN ;
      }
      if (!pContext.advanceIndex()) {
          return pContext.Status=ZS_SYNTAX_ERROR;
      }
      if(pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
          ErrorLog.errorLog("Missing entity identifier. Found <%s> at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_INV_ENTITY ;
      }

      Mod_Translate* wMod = new Mod_Translate;

      if ((wMod->Entity = EntityList.getEntityByName(pContext.CurrentToken->Text))==nullptr) {
          ErrorLog.errorLog("Entity identifier <%s> is not a valid, current entity. at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          delete wMod;
          return pContext.Status=ZS_INV_ENTITY ;
      }

      if (!pContext.advanceIndex()) {
          delete wMod;
          return pContext.Status=ZS_SYNTAX_ERROR;
      }
      if(pContext.CurrentToken->Type!=ZSRCH_COMMA) {
          ErrorLog.errorLog("Wrong modifier syntax. Expecting comma, found <%s> at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          delete wMod;
          return pContext.Status=ZS_MISS_PUNCTSIGN ;
      }

    /* get key field name,
     * check it is a valid entity field
     * check type compatibility with current field ZSTO
     */
      if(pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
          ErrorLog.errorLog("Missing field identifier. Found <%s> at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          delete wMod;
          return pContext.Status=ZS_MISS_FIELD ;
      }

      int wi=0 ;
      for (; wi < wMod->Entity->BuildDic.count();wi++) {
          if (pContext.CurrentToken->Text==wMod->Entity->BuildDic[wi].getFieldName()) {
              break;
          }
      }
      if (wi == wMod->Entity->BuildDic.count()) {
          ErrorLog.errorLog("Invalid field identifier <%s> for entity <%s>. at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   wMod->Entity->EntityName.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          delete wMod;
          return pContext.Status=ZS_INV_FIELD ;
      }

      wMod->KeyDicRank =  wi;

      if (!pContext.advanceIndex()) {
          delete wMod;
          return pContext.Status=ZS_SYNTAX_ERROR;
      }
      if(pContext.CurrentToken->Type!=ZSRCH_COMMA) {
          ErrorLog.errorLog("Wrong modifier syntax. Expecting comma, found <%s> at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_MISS_PUNCTSIGN ;
      }


     /* get result field name,
     * check it is a valid entity field
     * get Resulting field type : expression type will be resulting ZSTO
     */
      if(pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
          ErrorLog.errorLog("Missing field identifier. Found <%s> at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          delete wMod;
          return pContext.Status=ZS_MISS_FIELD ;
      }

      wi=0 ;
      for (; wi < wMod->Entity->LocalMetaDic.count();wi++) {
          if (pContext.CurrentToken->Text==wMod->Entity->LocalMetaDic[wi].getName()) {
              break;
          }
      }
      if (wi == wMod->Entity->LocalMetaDic.count()) {
          ErrorLog.errorLog("Invalid field identifier <%s> for entity <%s>. at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   wMod->Entity->EntityName.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          delete wMod;
          return pContext.Status=ZS_INV_FIELD ;
      }

      wMod->ResultDicRank =  wi;
      ZSearchOperandType_type wZSTO = pOB->ZSTO & ~ZSTO_BaseMask ;      /* get typology (field, literal, expression etc.) */
      ZSearchOperandType_type wZSTO1 = getZSTOFromZType(wMod->Entity->LocalMetaDic[wi].ZType);
      wZSTO = wZSTO | wZSTO1 ;                                          /* mix with resulting field base type */
      pOB->ZSTO = wZSTO;                                                /* replace type in operand */

      if (!pContext.advanceIndex()) {
          delete wMod;
          return pContext.Status=ZS_SYNTAX_ERROR;
      }

      if(pContext.CurrentToken->Type==ZSRCH_COMMA) {  /* if comma then operator argument is not omitted */

          if (!pContext.advanceIndex()) {
              delete wMod;
              return pContext.Status=ZS_SYNTAX_ERROR;
          }

          ZSearchOperator_type wOpTp=ZSOPV_EQUAL;
          /* here expecting compare operator */
          switch (pContext.CurrentToken->Type)
          {
          case ZSRCH_OPERATOR_EQUAL:
              wOpTp=ZSOPV_EQUAL;
              break;
          case ZSRCH_OPERATOR_LESS:
              wOpTp=ZSOPV_LESS;
              break;
          case ZSRCH_OPERATOR_GREATER:
              wOpTp=ZSOPV_LESS;
              break;
          case ZSRCH_OPERATOR_GREATEROREQUAL:
              wOpTp=ZSOPV_GREATEREQUAL;
              break;
          case ZSRCH_OPERATOR_LESSOREQUAL:
              wOpTp=ZSOPV_LESSEQUAL;
              break;
          default:
              ErrorLog.errorLog("Invalid operator. Found <%s> at line %d column %d ",
                       pContext.CurrentToken->Text.toString(),
                       pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
              pContext.LastErroredToken = pContext.CurrentToken;
              delete wMod;
              return pContext.Status=ZS_INV_OPERATOR ;
          }// switch
          if (!pContext.advanceIndex()) {
              delete wMod;
              return pContext.Status=ZS_SYNTAX_ERROR;
          }
      } // ZSRCH_COMMA

      if(pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
          ErrorLog.errorLog("Wrong modifier syntax. Expecting close parenthesis found <%s> at line %d column %d ",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_MISS_PUNCTSIGN ;
      }
      pOB->ModParams = wMod;
      break;
  } // ZSRCH_DECODE

  /* <field name>.SUBSTRING(<offset>,<length>) */
  case ZSRCH_SUBSTRING :
  {
    if ((wBaseZSTO != ZSTO_String)&&(wBaseZSTO != ZSTO_UriString)) {
      ErrorLog.errorLog("Modifier SUBSTRING is only possible with a string or an URI string. Possibly there is a mismatch with entity.field name.modifier. Found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_INV_MODIFIER;
    }
    pOB->ModifierType = pContext.CurrentToken->Type;
    if (!pContext.advanceIndex()) {
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    if(pContext.CurrentToken->Type!=ZSRCH_OPENPARENTHESIS) {
      ErrorLog.errorLog("Wrong modifier syntax. Expecting open parenthesis found <%s> at line %d column %d ",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }
    if (!pContext.advanceIndex()) {
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    if(pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
      ErrorLog.errorLog("Missing numeric literal. Found <%s> at line %d column %d",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_MISS_LITERAL ;
    }
    int wOffset = pContext.CurrentToken->Text.toInt();
    if (!pContext.advanceIndex()) {
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    if(pContext.CurrentToken->Type!=ZSRCH_COMMA) {
      ErrorLog.errorLog("Wrong modifier syntax. Expecting comma, found <%s> at line %d column %d",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }
    if (!pContext.advanceIndex()) {
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    if(pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
      ErrorLog.errorLog("Missing numeric literal. Found <%s> at line %d column %d ",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_MISS_LITERAL ;
    }
    int wLength = pContext.CurrentToken->Text.toInt();
    if (!pContext.advanceIndex()) {
        return pContext.Status=ZS_SYNTAX_ERROR;
    }
    if(pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
      ErrorLog.errorLog("Wrong modifier syntax. Expecting close parenthesis found <%s> at line %d column %d ",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }

    Mod_SubString* wMod = new Mod_SubString;

    wMod->Offset   = wOffset;
    wMod->Length   = wLength;
    pOB->ModParams = wMod;

    /* NB: pOB->ZSTO does not change - still string */

    break;
  } // ZSRCH_SUBSTRING

  case ZSRCH_SUBSTRINGRIGHT :
  case ZSRCH_SUBSTRINGLEFT :
  {
      if ((wBaseZSTO != ZSTO_String)&&(wBaseZSTO != ZSTO_UriString)) {
          ErrorLog.errorLog("Modifiers LEFT OR RIGHT are only compatible with a string or an URI string. Possibly there is a mismatch with entity.field name.modifier. Found <%s> at line %d column %d.",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_INV_MODIFIER;
      }
      pOB->ModifierType = pContext.CurrentToken->Type;
      if (!pContext.advanceIndex()) {
          return pContext.Status=ZS_SYNTAX_ERROR;
      }
      if(pContext.CurrentToken->Type!=ZSRCH_OPENPARENTHESIS) {
          ErrorLog.errorLog("Wrong modifier syntax. Expecting open parenthesis found <%s> at line %d column %d ",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_MISS_PUNCTSIGN ;
      }
      if (!pContext.advanceIndex()) {
          return ZS_SYNTAX_ERROR;
      }
      if(pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
          ErrorLog.errorLog("Missing numeric literal. Found <%s> at line %d column %d",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_MISS_LITERAL ;
      }

      int wLength = pContext.CurrentToken->Text.toInt();

      if (!pContext.advanceIndex()) {
          return pContext.Status=ZS_SYNTAX_ERROR;
      }
      if(pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
          ErrorLog.errorLog("Wrong modifier syntax. Expecting close parenthesis found <%s> at line %d column %d ",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_MISS_PUNCTSIGN ;
      }

      Mod_LeftRight* wMod = new Mod_LeftRight;
      wMod->Length   = wLength;
      pOB->ModParams = wMod;

      /* NB: pOB->ZSTO does not change - still string */

     break;
  } // ZSRCH_SUBSTRINGRIGHT or RIGHT


  case ZSRCH_PATH:
  case ZSRCH_EXTENSION:
  case ZSRCH_BASENAME:
  case ZSRCH_ROOTNAME: {
    if (wBaseZSTO != ZSTO_UriString) {
      ErrorLog.errorLog("Modifier <%s> is only possible with an URI string.at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_INV_MODIFIER;
    }

    pOB->ModifierType = pContext.CurrentToken->Type;
    /*
    if (!pContext.advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
    */
    /* NB: pOB->ZSTO changes from uriString to sting */
    ZSearchOperandType_type wZSTO = pOB->ZSTO & ~ZSTO_BaseMask ;      /* get typology (field, literal, expression etc.) */
    wZSTO = wZSTO | ZSTO_String  ;                                    /* mix with resulting field base type */
    pOB->ZSTO = wZSTO;                                                /* replace type in operand */

    break;
  }
  case ZSRCH_YEAR:
  case ZSRCH_MONTH:
  case ZSRCH_DAY:
  case ZSRCH_HOUR:
  case ZSRCH_MIN:
  case ZSRCH_SEC: {
    if (wBaseZSTO != ZSTO_Date) {
      ErrorLog.errorLog("Modifier <%s> is only possible with an Date.at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_INV_MODIFIER;
    }

    pOB->ModifierType = pContext.CurrentToken->Type;
    ZSearchOperandType_type wZSTO = pOB->ZSTO & ~ZSTO_BaseMask ;      /* get typology (field, literal, expression etc.) */
    wZSTO = wZSTO | ZSTO_Integer ;                                    /* mix with resulting field base type */
    pOB->ZSTO = wZSTO;
    break;
  }
  case ZSRCH_ZENTITY:
  case ZSRCH_ID:  {
      if (wBaseZSTO != ZSTO_Resource) {
        ErrorLog.errorLog("Modifier <%s> is only possible with an Resource.at line %d column %d.",
            pContext.CurrentToken->Text.toString(),
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_INV_MODIFIER;
      }
      pOB->ModifierType = pContext.CurrentToken->Type;
      ZSearchOperandType_type wZSTO = pOB->ZSTO & ~ZSTO_BaseMask ;      /* get typology (field, literal, expression etc.) */
      wZSTO = wZSTO | ZSTO_Integer ;                                    /* mix with resulting field base type */
      pOB->ZSTO = wZSTO;
      break;
  }
  default:
      ErrorLog.errorLog("Invalid modifier <%s> for field at line %d column %d ",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_INV_MODIFIER ;
  }// switch Type

  pContext.advanceIndex(false);
  return pContext.Status=ZS_SUCCESS;
}// _parseModifier

ZStatus
ZSearchParser::_parseOperandField(void* &    pTermOperand,ZSearchContext& pContext, ZSearchOperandType_type& pRequestedType)
{
  ZStatus wSt=ZS_SUCCESS;
  int wSvIndex=pContext.Index;
//  ZSearchFieldOperand  wFOD;

  int wEntityListIndex=0;

  pTermOperand=nullptr;
  ZSearchFieldOperand* wF1=new ZSearchFieldOperand;
  wSt=_parseFieldIdentifier(pContext,wF1);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  if (!OperandTypeCheck(pRequestedType,pContext,wF1, wSvIndex)) {
      delete wF1;
      ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
               pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
               pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
      return pContext.Status=ZS_INVTYPE ;
  }
/*
  if (!pContext.advanceIndex(true)) {
    delete wF1;
    return pContext.Status=ZS_SYNTAX_ERROR;
  }
*/
  pTermOperand = wF1;
  return pContext.Status=ZS_SUCCESS;


  ZSearchOperandType_type wZSTO = wF1->ZSTO & ZSTO_BaseMask;
  /* set up term */
  switch(wZSTO) {

  case ZSTO_FieldInteger:
  {

//    wF1->setZSTO(ZSTO_FieldInteger);

    /* no modifiers for numeric field */

    if (pContext.CurrentToken->Type == __MODIFIER_SEPARATOR__) {
        ErrorLog.errorLog("No modifier allowed to integer data. Found <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(pContext.Index+2)->Text.toString(),
                 pContext.CurrentTokenList.Tab(pContext.Index+2)->TokenLine,
                 pContext.CurrentTokenList.Tab(pContext.Index+2)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pContext.Index+2);
      return pContext.Status=ZS_SYNTAX_ERROR;
    }


    if (!OperandTypeCheck(pRequestedType,pContext,wF1, wSvIndex)) {
        delete wF1;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
        return pContext.Status=ZS_INVTYPE ;
    }
/*
    if (!pContext.advanceIndex()) {
      delete wF1;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
*/
    pTermOperand = wF1;
    return pContext.Status=ZS_SUCCESS;

  }// ZSTO_FieldInteger

  case ZSTO_FieldFloat:
  {

//    wF1->setZSTO (ZSTO_FieldFloat);

    /* no modifiers for numeric field */
    if (pContext.CurrentToken->Type == __MODIFIER_SEPARATOR__) {
        ErrorLog.errorLog("No modifier allowed to floating point data. Found <%s> at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,pContext,wF1, wSvIndex)) {
        delete wF1;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    if (!pContext.advanceIndex()) {
      delete wF1;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return pContext.Status=ZS_SUCCESS;
  } //  ZSTO_FieldFloat


  case ZSTO_FieldString:
  {

//    wF1->setZSTO (ZSTO_FieldString);

    if (pContext.CurrentToken->Type == __MODIFIER_SEPARATOR__) {
      if (!pContext.advanceIndex()) {  /* position to modifier token */
        delete wF1;
        return pContext.Status=ZS_SYNTAX_ERROR;
      }

      if ((wSt=_parseModifier(wF1,pContext))!=ZS_SUCCESS) {
        delete wF1;
        return wSt;
      }

    } // __MODIFIER_SEPARATOR__

    if (!OperandTypeCheck(pRequestedType,pContext,wF1, wSvIndex)) {
        delete wF1;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;

        return pContext.Status=ZS_INVTYPE ;
    }
    /*
    if (!pContext.advanceIndex()) {
      delete wF1;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    */
    pTermOperand = wF1;
    return pContext.Status=ZS_SUCCESS;
  } // case strings

  case ZSTO_FieldUriString: {

//      wF1->setZSTO (ZSTO_FieldUriString);

    if (pContext.CurrentToken->Type == __MODIFIER_SEPARATOR__) {
      if (!pContext.advanceIndex()) {  /* position to modifier token */
        delete wF1;
        return pContext.Status=ZS_SYNTAX_ERROR;
      }

      if ((wSt=_parseModifier(wF1,pContext))!=ZS_SUCCESS) {
        delete wF1;
        return wSt;
      }

    } // __MODIFIER_SEPARATOR__


    if (!OperandTypeCheck(pRequestedType,pContext,wF1, wSvIndex)) {
      delete wF1;
      pTermOperand = nullptr;
      ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
               pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
               pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
      return pContext.Status=ZS_INVTYPE ;
    }
    if (!pContext.advanceIndex()) {
      delete wF1;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    pTermOperand = wF1;
    return pContext.Status=ZS_SUCCESS;
  } // ZType_URIString

  case ZSTO_FieldDate: {

//    wF1->setZSTO(ZSTO_FieldDate);

    if (pContext.CurrentToken->Type == __MODIFIER_SEPARATOR__) {
      if (!pContext.advanceIndex()) {  /* position to modifier token */
        delete wF1;
        return pContext.Status=ZS_SYNTAX_ERROR;
      }

      if ((wSt=_parseModifier(wF1,pContext))!=ZS_SUCCESS) {
        delete wF1;
        return wSt;
      }

    } // __MODIFIER_SEPARATOR__

    if (!OperandTypeCheck(pRequestedType,pContext,wF1, wSvIndex)) {
      delete wF1;
      pTermOperand = nullptr;
      ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
               pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
               pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
      return pContext.Status=ZS_INVTYPE ;
    }

    if (!pContext.advanceIndex()) {
      delete wF1;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return pContext.Status=ZS_SUCCESS;
  } // ZType_ZDateFull

  case ZSTO_FieldChecksum: {

//    wF1->setZSTO(ZSTO_Checksum);

    if (!OperandTypeCheck(pRequestedType,pContext,wF1, wSvIndex)) {
      delete wF1;
      pTermOperand = nullptr;
      ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
               pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
               pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
      return pContext.Status=ZS_INVTYPE ;
    }

    if (!pContext.advanceIndex()) {
      delete wF1;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return pContext.Status=ZS_SUCCESS;
  }
  case ZSTO_FieldResource: {

    wF1->setZSTO(ZSTO_FieldResource);

    if (pContext.CurrentToken->Type == __MODIFIER_SEPARATOR__ ) {
      if (!pContext.advanceIndex()) {  /* position to modifier token */
        delete wF1;
        return pContext.Status=ZS_SYNTAX_ERROR;
      }

      if ((wSt=_parseModifier(wF1,pContext))!=ZS_SUCCESS) {
        delete wF1;
        return wSt; // pContext.Status is updated by _parseModifier
      }

    } // __MODIFIER_SEPARATOR__

    if (!OperandTypeCheck(pRequestedType,pContext,wF1, wSvIndex)) {
        delete wF1;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    if (!pContext.advanceIndex()) {
      delete wF1;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return pContext.Status=ZS_SUCCESS;
  } // ZType_Resource

  default:
    ErrorLog.errorLog("Dictionary field type <%s> ZSTO <%s> for field <%s>.<%s> is not supported at line %d column %d.",
               decode_ZType(wF1->getZType()),
               decode_OperandType(wF1->ZSTO),
        pContext.CurrentTokenList.Tab(pContext.Index-2)->Text.toString(),pContext.CurrentToken->Text.toString(),
               pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
    pContext.LastErroredToken = pContext.CurrentToken;
    return pContext.Status=ZS_INVTYPE ;
  }// switch

  return pContext.Status=ZS_SUCCESS;
} //_parseOperandField

bool ZSearchParser::_parseEntity(void *&pOperand,
                                 ZSearchContext &pContext,
                                 ZSearchOperandType_type &pRequestedType)
{
  if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER)
    return false;

  int wSvIndex=pContext.Index;
  pOperand = nullptr;

  /* check if ZEntity */

  for (int wi=0 ; wi < ZEntitySymbolList.count();wi++) {
    if (pContext.CurrentToken->Text.compare(ZEntitySymbolList[wi].Symbol)==0) {
      ZSearchLiteral *wLit = new ZSearchLiteral ;
      wLit->setZSTO(ZSTO_LiteralInteger);
      wLit->setInteger(ZEntitySymbolList[wi].Value);
      wLit->FullFieldName = ZEntitySymbolList[wi].Symbol;
//      wLit->Comment = ZEntitySymbolList[wi].Symbol;

      if (!pContext.advanceIndex()) {
        delete wLit;
        return false;
      }

      if (!OperandTypeCheck(pRequestedType,pContext,wLit, wSvIndex)) {
        delete wLit;
        return false ;
      }

      pOperand = wLit;
      return true;
    }
  }
  return false;
} //_parseZEntity

bool
ZSearchParser::_parseZEntity (void* &    pOperand, ZSearchContext& pContext, ZSearchOperandType_type &pRequestedType)
{
  if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER)
    return false;

  pOperand = nullptr;
  /* check if ZEntity */

  for (int wi=0 ; wi < ZEntitySymbolList.count();wi++) {
    if (pContext.CurrentToken->Text.compareCase(ZEntitySymbolList[wi].Symbol)==0) {

      ZSearchLiteral *wLit = new ZSearchLiteral ;
      wLit->setZSTO(ZSTO_LiteralInteger);
      wLit->setInteger( ZEntitySymbolList[wi].Value);
      wLit->FullFieldName = ZEntitySymbolList[wi].Symbol;
//      wLit->Comment = ZEntitySymbolList[wi].Symbol;

      if (!OperandTypeCheck(pRequestedType,pContext,wLit, pContext.Index)) {
        delete wLit;
        return false ;
      }

      if (!pContext.advanceIndex()) {
        delete wLit;
        return false;
      }
       pOperand = wLit;
      return true;
    }
  }
  return false;
} //_parseZEntity

bool ZSearchParser::_getSymbol(void *&pOperand,
                               ZSearchContext &pContext,
                               ZSearchOperandType_type &pRequestedType)
{
  if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER)
    return false;

  /* check if symbol */
  ZSearchSymbol wSymbol=SymbolList.getSymbol(pContext.CurrentToken->Text);
  if (wSymbol.isNull())
      return false;

  if (!OperandTypeCheck(pRequestedType,pContext,&wSymbol, pContext.Index)) {
      return false ;
  }
  if (!pContext.advanceIndex()) {
      pOperand = nullptr;
      return false;
  }
  pOperand = new ZSearchSymbol(wSymbol);
  return true;

#ifdef __DEPRECATED__
  for (int wi=0 ; wi < SymbolList.count();wi++) {
    if (pContext.CurrentToken->Text.compareCase(SymbolList[wi].Symbol)==0) {
      ZSearchLiteral *wLit = new ZSearchLiteral ;
      wLit->setZSTO(ZSTO_LiteralUriString);
      wLit->setURI( SymbolList[wi].Path);
      wLit->FullFieldName = SymbolList[wi].Symbol;
//      wLit->Comment = SymbolList[wi].Symbol;

      if (!OperandTypeCheck(pRequestedType,pContext,wLit, pContext.Index)) {
        delete wLit;
        return false ;
      }

      if (!pContext.advanceIndex()) {
        delete wLit;
        pOperand = nullptr;
        return false;
      }

      pOperand = wLit;

      return true;
    }
  }
  return false;
#endif // __DEPRECATED__

} //_parseSymbol


ZStatus
ZSearchParser::_parseLiteral(ZSearchLiteral* & pOperand,ZSearchContext& pContext, ZSearchOperandType_type &pRequestedType)
{
  ZStatus wSt;

  pOperand=nullptr;
  int wStartOperandIndex=pContext.Index;

  switch (pContext.CurrentToken->Type ) {

  case ZSRCH_STRING_LITERAL: {
    ZSearchLiteral *wLit=new ZSearchLiteral;

    wLit->setZSTO( ZSTO_LiteralString);
//    wLit->TokenList.push(pContext.CurrentToken);

/* Eliminate leading and trailing quotes if any */

    utf8_t * wContent = pContext.CurrentToken->Text.duplicate();
    utf8_t * wPtr = wContent;
    utf8_t * wHardEnd = wContent+pContext.CurrentToken->Text.UnitCount;

    if (wPtr[0]=='"')
      wPtr++;

    utf8_t * wEnd= wContent;
    while ((*wEnd != 0) && (wEnd < wHardEnd))
      wEnd++;

    wEnd--;
    if (wEnd[0]=='"')
      wEnd--;

    *wEnd = 0;

    wLit->setString(wPtr);

    zfree(wContent);

    /* done */

    _DBGPRINT("ZSearchParser::_parseLiteral Found string literal <%s>\n",wLit->getString().toString())

    if (!OperandTypeCheck(pRequestedType,pContext,wLit, wStartOperandIndex)) {
        delete wLit;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wStartOperandIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    pOperand = wLit;

    pContext.advanceIndex(false);

    return pContext.Status=ZS_SUCCESS;
  } //ZSRCH_STRING_LITERAL


  case ZSRCH_HEXA_LITERAL: {
/*
    if (pContext.CurrentToken->Text.UnitCount >= cst_checksumHexa) { // possible checksum litteral

    }
    else
        if (pContext.CurrentToken->Text.UnitCount >= cst_md5Hexa) { // possible md5 litteral

    }
*/
    ZSearchLiteral *wLit=new ZSearchLiteral;
    wLit->setZSTO( ZSTO_LiteralInteger);
//    wLit->TokenList.push(pContext.CurrentToken);

    /* getting hexa integer */

//    wLit->Content =  0;

    long wContent = 0;

    long wSign = 1;
    long w16 = 1;

    utf8_t* wPtr=pContext.CurrentToken->Text.Data;

    if (*wPtr=='-') /* leading sign */
      wSign=-1;

    if ((wPtr[0]=='0')&&((wPtr[1]=='x')||(wPtr[1]=='X')))
        wPtr += 2;


    while (*wPtr != 0) {
        if ((*wPtr>='0')&&((*wPtr<='9'))) {
            wContent *= 16;
            wContent += long(*wPtr - '0') ;
        }
      else {
            if ((*wPtr>='A')&&((*wPtr<='F'))) {
                wContent *= 16;
                wContent += long(*wPtr - 'A' + 10) ;
            }
        else
                if ((*wPtr>='a')&&((*wPtr<='f'))) {
                    wContent *= 16;
                    wContent += long(*wPtr - 'a' + 10);
                }

      }
      if (*wPtr=='-') { /* trailing sign */
          wSign=-1;
          break;
      }
      wPtr++;
      w16++;
    }
  /* got it */

    wContent *= wSign ;
    wLit->setInteger(wContent);

    _DBGPRINT("ZSearchParser::_parseLiteral Found hexa literal <%X>\n",wLit->getInteger())

    if (!OperandTypeCheck(pRequestedType,pContext,wLit, wStartOperandIndex)) {
        delete wLit;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wStartOperandIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    pOperand = wLit ;

    pContext.advanceIndex(false);

    return pContext.Status=ZS_SUCCESS;
  } //ZSRCH_HEXA_LITERAL

  case ZSRCH_NUMERIC_LITERAL:
  case ZSRCH_INTEGER_LITERAL:
  case ZSRCH_INTEGER_LONG_LITERAL:
  case ZSRCH_INTEGER_ULONG_LITERAL: {

    ZSearchLiteral *wLit=new ZSearchLiteral;
    wLit->setZSTO( ZSTO_LiteralInteger);

//    wLit->TokenList.push(pContext.CurrentToken);

    long wContent = 0;
    long wSign = 1;

    utf8_t* wPtr=pContext.CurrentToken->Text.Data;
    if (wPtr[0]=='-') { /* leading sign */
      wSign = -1;
      wPtr ++;
    }


    while (*wPtr != 0) {
      if ((*wPtr>='0')&&((*wPtr<='9'))) {
          wContent *= 10 ;
          wContent += long(*wPtr - '0')  ;
      }
      if (*wPtr=='-') { /* trailing sign */
        wSign = -1;
        break;
      }
      wPtr++;
    }

    wContent *= wSign ;

    wLit->setInteger( wContent) ;
    _DBGPRINT("ZSearchParser::_parseLiteral Found numeric literal <%ld>\n",wLit->getInteger())


    if (!OperandTypeCheck(pRequestedType,pContext,wLit, wStartOperandIndex)) {
        delete wLit;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wStartOperandIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    pOperand = wLit ;

    pContext.advanceIndex(false);

    return pContext.Status=ZS_SUCCESS;
  } // long integer unsigned long;

  case ZSRCH_FLOAT_LITERAL:
  case ZSRCH_DOUBLE_LITERAL: {

    ZSearchLiteral *wLit=new ZSearchLiteral;
    wLit->setZSTO( ZSTO_LiteralFloat);
//    wLit->TokenList.push(pContext.CurrentToken);

    wLit->setFloat(pContext.CurrentToken->Text.toDouble());

    if (!pContext.advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    _DBGPRINT("ZSearchParser::_parseLiteral Found fload literal <%g>\n",wLit->getFloat())

    if (!OperandTypeCheck(pRequestedType,pContext,wLit, wStartOperandIndex)) {
        delete wLit;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wStartOperandIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    pOperand = wLit;

    return pContext.Status=ZS_SUCCESS;
  } // long integer unsigned long;

      /* xx/xx/xx see ZSRCH_ZDATE_LITERAL for another safer format */
  case ZSRCH_DATE_LITERAL:
  {
    ZSearchLiteral *wLit=new ZSearchLiteral;
    wLit->setZSTO( ZSTO_LiteralDate);

//    wLit->TokenList.push(pContext.CurrentToken);

    /* allowed date format are
     *  dd/mm/yy
     *  dd/mm/yyyy
     *  mm/dd/yy
     *  mm/dd/yyyy
     *
     *
     * [d]d{/|-}[m]m{/|-}[yy]yy[-hh:mm:ss]
     *
     */
    uint8_t pFed=0;
    struct tm wTm;
    wSt=ZDateFull::checkDMY(pContext.CurrentToken->Text,wTm,pFed);
    if (wSt!=ZS_SUCCESS) {
      wSt=ZDateFull::checkMDY(pContext.CurrentToken->Text,wTm,pFed);
      if (wSt!=ZS_SUCCESS) {
        ErrorLog.errorLog("Invalid literal date <%s> at line %d column %d.",
            pContext.CurrentToken->Text.toString(),
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
        delete wLit;
        pOperand = nullptr;
        return ZS_INV_LITERAL;
      }
    }
    ZDateFull wD;
    wD._toInternal(wTm);
    wLit->setDate( wD);

    _DBGPRINT("ZSearchParser::_parseLiteral Found date literal <%s>\n",wD.toFormatted().toCChar())

    if (!pContext.advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,pContext,wLit, wStartOperandIndex)) {
        delete wLit;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wStartOperandIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    pOperand = wLit;
    return pContext.Status=ZS_SUCCESS;
  } // ZSRCH_DATE_LITERAL

      /* ZDATE(xx/xx/xx) */
  case ZSRCH_ZDATE_LITERAL: {
    ZSearchLiteral *wLit=new ZSearchLiteral;
    wLit->setZSTO( ZSTO_LiteralDate);

//    wLit->TokenList.push(pContext.CurrentToken);
    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    if(pContext.CurrentToken->Type!=ZSRCH_OPENPARENTHESIS) {
        ErrorLog.errorLog("ZDate literal : wrong syntax. Expecting open parenthesis. Found <%s> at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }
    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    int wSvIndex=pContext.Index;
    utf8VaryingString wDLContent;

/*    [d]d{/|-}[m]m{/|-}[yy]yy[-hh:mm:ss] */

    while (pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
      if ((pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL)
          && (pContext.CurrentToken->Type!=ZSRCH_OPERATOR_DIVIDEORSLASH)
          && (pContext.CurrentToken->Type!=ZSRCH_OPERATOR_MINUS)
          && (pContext.CurrentToken->Type!=ZSRCH_COLON))
      {
        ErrorLog.errorLog("ZDate literal : invalid date literal value. Expecting date literal with format [d]d{/|-}[m]m{/|-}[yy]yy[-hh[:mm:[ss]]]. Found <%s> at line %d column %d.",
            pContext.CurrentToken->Text.toString(),
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
        delete wLit;
        pOperand=nullptr;
        return pContext.Status=ZS_SYNTAX_ERROR;
      }

      wDLContent += pContext.CurrentToken->Text;
      if (!pContext.advanceIndex()) {
        delete wLit;
        pOperand=nullptr;
        return pContext.Status=ZS_SYNTAX_ERROR;
      }
    }// not ZSRCH_CLOSEPARENTHESIS

    uint8_t pFed=0;
    struct tm wTm;
    wSt=ZDateFull::checkDMY(wDLContent,wTm,pFed);
    if (wSt!=ZS_SUCCESS) {
      wSt=ZDateFull::checkMDY(wDLContent,wTm,pFed);
      if (wSt!=ZS_SUCCESS) {
        ErrorLog.errorLog("Invalid literal date <%s> at line %d column %d.",
            wDLContent.toString(),
            pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
        return pContext.Status=ZS_INV_LITERAL;
      }
    }
    ZDateFull wD;
    wD._toInternal(wTm);
    wLit->setDate(wD) ;

     _DBGPRINT("ZSearchParser::_parseLiteral Found ZDate literal <%s>\n",wD.toFormatted().toCChar())

    if (!pContext.advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,pContext,wLit->getOperandBase(), wSvIndex)) {
        delete wLit;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wSvIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wSvIndex)->TokenLine,pContext.CurrentTokenList.Tab(wSvIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wSvIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    pOperand = wLit ;
    return pContext.Status=ZS_SUCCESS;
  } //ZSRCH_ZDATE_LITERAL

  case ZSRCH_RESOURCE_LITERAL: {

    ZSearchLiteral *wLit=new ZSearchLiteral;
    wLit->setZSTO( ZSTO_LiteralResource );

//    wLit->TokenList.push(pContext.CurrentToken);

    if (!pContext.advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if(pContext.CurrentToken->Type!=ZSRCH_OPENPARENTHESIS) {
      ErrorLog.errorLog("ZResource literal : wrong syntax. Expecting open parenthesis. Found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }
//    wLit->TokenList.push(pContext.CurrentToken);
    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    if(pContext.CurrentToken->Type != ZSRCH_IDENTIFIER ) {
      ErrorLog.errorLog("Missing ZEntity identifier. Found <%s> at line %d column %d",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      pOperand=nullptr;
      return pContext.Status=ZS_MISS_LITERAL ;
    }

    ZEntity_type    wZEntity=0;

    if(pContext.CurrentToken->Type != ZSRCH_IDENTIFIER ) { /* if a symbol has been set here */

        int wi=0;
        for (;wi < ZEntitySymbolList.count();wi++) {
          if (pContext.CurrentToken->Text==ZEntitySymbolList[wi].Symbol) {
            wZEntity = ZEntitySymbolList[wi].Value ;
            break;
          }
        }
        if (wi==ZEntitySymbolList.count()) {
          ErrorLog.errorLog("Invalid ZEntity identifier (not found in ZEntityList). Token <%s> at line %d column %d",
              pContext.CurrentToken->Text.toString(),
              pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
            pContext.LastErroredToken = pContext.CurrentToken;
          delete wLit;
          pOperand=nullptr;
          return pContext.Status=ZS_MISS_LITERAL ;
        }
    } else if (pContext.CurrentToken->Type == ZSRCH_NUMERIC_LITERAL ) {
        wZEntity = pContext.CurrentToken->Text.toLong(10) ;
    } else if (pContext.CurrentToken->Type == ZSRCH_HEXA_LITERAL ) {
        wZEntity = pContext.CurrentToken->Text.toLong(16) ;
    } else {
        ErrorLog.errorLog("Missing ZEntity descriptor (ZEntity identifier, numeric / hexa literal). Found <%s> at line %d column %d",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
        delete wLit;
        pOperand=nullptr;
        return pContext.Status=ZS_MISS_LITERAL ;
    }
//    wLit->TokenList.push(pContext.CurrentToken);
    if (!pContext.advanceIndex())
      return pContext.Status=ZS_SYNTAX_ERROR;

    if(pContext.CurrentToken->Type!=ZSRCH_COMMA) {
      ErrorLog.errorLog("Wrong ZResource literal syntax. Expecting comma, found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      pOperand=nullptr;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }
//    wLit->TokenList.push(pContext.CurrentToken);

    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if(pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
      ErrorLog.errorLog("Wrong ZResource literal syntax. Missing numeric literal. Found <%s> at line %d column %d",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      return pContext.Status=ZS_MISS_LITERAL ;
    }

    Resourceid_type wId = pContext.CurrentToken->Text.toLong() ;
    wLit->setResource( ZResource(wId,wZEntity));


//    wLit->TokenList.push(pContext.CurrentToken);

    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if(pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
      ErrorLog.errorLog("ZResource literal : wrong syntax. Expecting close parenthesis. Found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }

//    wLit->TokenList.push(pContext.CurrentToken);
    _DBGPRINT("ZSearchParser::_parseLiteral Found ZResource literal <%s>\n",ZResource(wId,wZEntity).toStr().toCChar())

    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,pContext,wLit->getOperandBase(), wStartOperandIndex)) {
        delete wLit;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_INVTYPE ;
    }

    pOperand = wLit;

    return pContext.Status=ZS_SUCCESS;
  } // ZSRCH_RESOURCE_LITERAL

  case ZSRCH_CHECKSUM_LITERAL: {

    ZSearchLiteral *wLit=new ZSearchLiteral;
    wLit->setZSTO( ZSTO_LiteralChecksum );

//    wLit->TokenList.push(pContext.CurrentToken);

    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    if(pContext.CurrentToken->Type!=ZSRCH_OPENPARENTHESIS) {
      ErrorLog.errorLog("checkSum literal : wrong syntax. Expecting open parenthesis. Found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }

//    wLit->TokenList.push(pContext.CurrentToken);
    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if(pContext.CurrentToken->Type != ZSRCH_HEXA_LITERAL ) {
      ErrorLog.errorLog("Missing Hexadecimal literal (64 hexa characters). Found <%s> at line %d column %d",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      return pContext.Status=ZS_MISS_LITERAL ;
    }

    checkSum wCS;
    wSt=wCS.fromHexa(pContext.CurrentToken->Text);
    if (wSt!=ZS_SUCCESS) {
      ErrorLog.errorLog("Invalid checksum hexadecimal litteral : possibly invalid size of hash code. Token <%s> at line %d column %d",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      return pContext.Status=ZS_MISS_LITERAL ;
    }
    wLit->setChecksum(wCS);
//    wLit->TokenList.push(pContext.CurrentToken);

    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if(pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
      ErrorLog.errorLog("ZResource literal : wrong syntax. Expecting close parenthesis. Found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
      delete wLit;
      return pContext.Status=ZS_MISS_PUNCTSIGN ;
    }
//    wLit->TokenList.push(pContext.CurrentToken);

    _DBGPRINT("ZSearchParser::_parseLiteral Found checksum literal <%s>\n",wLit->getChecksum().toHexa().toCChar())


    if (!pContext.advanceIndex()) {
      delete wLit;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,pContext,wLit->getOperandBase(), wStartOperandIndex)) {
        delete wLit;
        ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                 pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(wStartOperandIndex);
        return pContext.Status=ZS_INVTYPE ;
    }

    pOperand = wLit;
    return pContext.Status=ZS_SUCCESS;
  } // ZSRCH_RESOURCE_LITERAL

  default:
    ErrorLog.errorLog("Wrong literal syntax. Expecting literal, found <%s> at line %d column %d.",
        pContext.CurrentToken->Text.toString(),
        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken = pContext.CurrentToken;
    return pContext.Status=ZS_MISS_LITERAL ;
  }//switch

  return pContext.Status=ZS_SUCCESS;
} //_parseLiteral
/** gets one element of a date either day, month year, hour minute seconds whenever mentionned (will define date precision).
 * Element must be immediately followed by a valid separator pEndSeparator or not if equals SRCH_Nothing.
 * Returs ZS_SUCCESS if OK, ZS_EOF if Closing parenthesis has been encountered AFTER the date element or ZS_SYNTAX_ERROR in other cases
 *
*/
ZStatus
ZSearchParser::_getZDateOnePiece(int &pValue,ZSearchContext& pContext,int pMaxDigits,ZSearchTokentype pEndSeparator)
{
  if (pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
    ErrorLog.errorLog("Date literal : invalid date literal value. Expecting date literal with format [d]d{/|-}[m]m{/|-}[yy]yy[-hh[:mm:[ss]]]. Found <%s> at line %d column %d.",
        pContext.CurrentToken->Text.toString(),
        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken = pContext.CurrentToken;
    return pContext.Status=ZS_SYNTAX_ERROR;
  }
  pValue=pContext.CurrentToken->Text.toInt();
  if (!pContext.advanceIndex()) {
    return pContext.Status=ZS_SYNTAX_ERROR;
  }
  if (pContext.CurrentToken->Type==ZSRCH_CLOSEPARENTHESIS)
    return pContext.Status=ZS_EOF;

  if (pContext.CurrentToken->Type==pEndSeparator) {
    if (!pContext.advanceIndex()) {
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
  }
  return pContext.Status=ZS_SUCCESS;
}

#ifdef __DEPRECATED__
/* collection is instantiated as soon as first entity name is known and validated to be a valid, stored entity */

ZStatus
ZSearchParser::_parseFind(ZSearchContext & pContext)
{
  ZStatus wSt=ZS_SUCCESS;
  Action = ZSPA_Find;
  pContext.InstructionType = ZSITP_Find;

//  int wSelNumber=-1 ; /* default set to ALL */
//  ZFldSelClause            wSelClause;
  ZSearchLogicalTerm*   wLogicalTerm=nullptr;

  pContext.clearEntities(); /* reset current entities being used (defined within phrase) */
  pContext.clearMessage();

  if (!pContext.advanceIndex())
    return ZS_SYNTAX_ERROR;

  switch (pContext.CurrentToken->Type) {
    case ZSRCH_ALL:
      Action |= ZSPA_All;
      if (!pContext.advanceIndex())
        return ZS_SYNTAX_ERROR;
      break;
    case ZSRCH_FIRST:
      Action |= ZSPA_First;
      pContext.InstructionType |= ZSITP_First;  /* Nb ; Equivalence for ZSITP_All is 0x0 as defaulted value */
      if (!pContext.advanceIndex())
        return ZS_SYNTAX_ERROR;

      if (pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
          ErrorLog.errorLog("Missing numeric literal after clause FIRST. set to default value : 1 - at line %d column %d. Found <%s>.",
                     pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
          pContext.LastErroredToken = pContext.CurrentToken;
          return ZS_MISS_LITERAL;
      }

      pContext.NumberToFind = pContext.CurrentToken->Text.toInt(10);
      if (!pContext.advanceIndex())
          return ZS_SYNTAX_ERROR;
      break;
    default:
      ErrorLog.warningLog("Missing keyword one of [<ALL>,<FIRST>] set to default : ALL - at line %d column %d. Found <%s>.",
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
      Action |= ZSPA_All;
      break;
    }// switch

    /* search for entity(ies) after keyword IN */

    int wFieldListBookMark = pContext.Index;  /* keep memory of fields selection clause beginning */

    while ((pContext.CurrentTokenList.count() > pContext.Index ) &&(pContext.CurrentToken->Type != ZSRCH_IN ))  /* search for beginning of entities */
    {
        if (!pContext.advanceIndex()) {
            ErrorLog.errorLog("Missing keyword <IN> in instruction.");
            return ZS_SYNTAX_ERROR;
        }
    }
    if (pContext.CurrentTokenList.count() == pContext.Index ) {
        ErrorLog.errorLog("Missing keyword <IN> in instruction.");
        return ZS_SYNTAX_ERROR;
    }
    /* here we are pointing on <IN> */
    if (!pContext.advanceIndex())
        return ZS_SYNTAX_ERROR;

    if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
        ErrorLog.errorLog("Expected active entity. Found <%s> line <%d> column <%d>",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken= pContext.CurrentToken;
        return ZS_INV_ENTITY;
    }

    while ((pContext.CurrentToken->Type != ZSRCH_WITH) && (pContext.CurrentToken->Type!=ZSRCH_AS)) {

        if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
            ErrorLog.errorLog("Expected entity identifier. Found <%s> at line %d column %d.",
                       pContext.CurrentToken->Text.toString(),
                       pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
            pContext.LastErroredToken=pContext.CurrentToken;
            return ZS_INV_ENTITY;
        }
        /* search for a valid open entity */
        std::shared_ptr<ZSearchEntity>          wEntity=nullptr;
        wEntity = EntityList.getEntityByName(pContext.CurrentToken->Text);
        if (wEntity==nullptr) {
            ErrorLog.errorLog("Entity identifier not found in declared entities list. Found <%s> at line %d column %d.",
              pContext.CurrentToken->Text.toString(),
              pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
            pContext.LastErroredToken=pContext.CurrentToken;
            return ZS_INV_ENTITY;
        }

        /* here active entity is set to pContext.CurrentToken->Text */

/* collection is instantiated as soon as first entity name is known and validated to be a valid, stored entity */
//        pContext.CurEntity=ZSearchEntity::constructWithZSearchEntity(wEntity,pContext.CurrentToken);

        pContext.SourceEntity = wEntity ;
        pContext.TargetEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(wEntity,pContext.CurrentToken));

        /* entity name will be allocated later after having parsed AS clause */

        if (!pContext.advanceIndex())
            return ZS_SYNTAX_ERROR;

        if (pContext.CurrentToken->Type == ZSRCH_COMMA) {
            if (!pContext.advanceIndex())
                return ZS_SYNTAX_ERROR;
            continue;
        }
    } //  while != ZSRCH_WITH && !=ZSRCH_AS

    if (pContext.SourceEntity==nullptr) {
        ErrorLog.errorLog("No valid entity has been mentionned. Found <%s> at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
        pContext.LastErroredToken=pContext.CurrentToken;
        return ZS_SYNTAX_ERROR;
    }



 //   int wWithBookMark = pContext.Index; // store pContext.Index pointing to first token after WITH Clause

    /* get back to parse selection */
//    setpContext.Index(wWithBookMark);  /* restore pContext.Index to first token after with clause */

    if (pContext.CurrentToken->Type == ZSRCH_WITH) {
        if (!pContext.advanceIndex())
          return ZS_SYNTAX_ERROR;

        wSt=_parseLogicalTerm(wLogicalTerm,pContext,0,pContext.Index);
        if (wSt!=ZS_SUCCESS)
          return wSt;
    } // ZSRCH_WITH
    else {
        ErrorLog.warningLog("Missing keyword <WITH> : all ranks of entity will be selected." );
    }
    /* here token must be 'AS' */

    if (pContext.CurrentToken->Type!=ZSRCH_AS) {
      ErrorLog.errorLog("Syntax error : Missing clause <AS> while found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
        pContext.LastErroredToken = pContext.CurrentToken;
      if (wLogicalTerm!=nullptr)
        delete wLogicalTerm;
      return ZS_MISS_KEYWORD;
    }

    if (!pContext.advanceIndex()) {
      if (wLogicalTerm!=nullptr)
        delete wLogicalTerm;
      return ZS_SYNTAX_ERROR;
    }

    /* here entity name as collection name */

 //   ZSearchToken* wEntityNameToken= pContext.CurrentToken;

    if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER ) {
        ErrorLog.errorLog("Syntax error : Missing collection identifier while found <%s> at line %d column %d.",
                  pContext.CurrentToken->Text.toString(),
                  pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
        pContext.LastErroredToken = pContext.CurrentToken;
        if (wLogicalTerm!=nullptr)
            delete wLogicalTerm;
        return ZS_MISS_FIELD;
    }
/* at this stage, collection is already built */

    pContext.TargetEntity->setNameByToken(pContext.CurrentToken);

    if (!pContext.advanceIndex()) {
      pContext.clearEntities();
      delete wLogicalTerm;
      return ZS_SYNTAX_ERROR;
    }
    if (!pContext.testSemiColon()){
      pContext.clearEntities();
      delete wLogicalTerm;
      return ZS_MISS_PUNCTSIGN ;
    }
    pContext.TargetEntity->setLogicalTerm(wLogicalTerm);

    /* in the end get back to parse selected fields */

    int wEndIndex = pContext.Index;
    pContext.setIndex(wFieldListBookMark); /* get back to field list clause : NB: we have entity(ies) field dictionary at disposal */

    wSt=_parseSelClause(pContext);
    if (wSt!=ZS_SUCCESS)
        return wSt;

    pContext.setIndex(wEndIndex);

    EntityList.push(pContext.TargetEntity);
    pContext.setMessage("Collection <%s> has been created and registered as a new search entity.",
                     pContext.TargetEntity->EntityFullName.toString());
    ErrorLog.infoLog("Collection <%s> has been created and registered as a new search entity.",
            pContext.TargetEntity->EntityFullName.toString()  );
  return ZS_SUCCESS;
}// ZSearchParser::_parseFind

#endif // __DEPRECATED__

/*
terms operands allowed conversions before logical operation :



Resource
    no modifier
    ZEntity (Integer)
        Id      (Integer)

    Date
    no modifier
    Month...(Integer)




    Checksum is only allowed to be compared with Checksum
*/


ZSearchOperandType_type getMainOperandType(ZSearchLogicalOperand* pOperand)
{
  if (pOperand->ZSTO==ZSTO_Logical)
    return ZSTO_Bool;             /* if operand is logical then result has type boolean */
  if (pOperand->ZSTO==ZSTO_Arithmetic)
    return pOperand->ZSTO;        /* if operand is arithmetic then result has literal type defined at operand level */


  /* in all other cases, main type is deduced from detailed operand type */

  ZSearchOperandType_type wType = ZSearchOperandType_type(pOperand->ZSTO & ZSTO_BaseMask);
  switch (wType)
  {
  case ZSTO_String:
    return ZSTO_String;
  case ZSTO_UriString:
    if (pOperand->ModifierType!=ZSRCH_NOTHING)
      return ZSTO_String;
    return ZSTO_UriString;
  case ZSTO_Integer:
    return ZSTO_Integer;
  case ZSTO_Float:
    return ZSTO_Float;
  case ZSTO_Resource:
    if (pOperand->ModifierType!=ZSRCH_NOTHING)
      return ZSTO_Integer;
    return ZSTO_Resource;
  case ZSTO_Date:
    if (pOperand->ModifierType!=ZSRCH_NOTHING)
      return ZSTO_Integer;
    return ZSTO_Resource;
  case ZSTO_Checksum:
    return ZSTO_Checksum;
  }//switch

}



/*
NB: type of a logical term is bool (in case of term operand is a logical term)

Allowed type correspondance

        to      string      integer   float   Date    checkSum      bool   ZResource     Allowed operators
  from

  string            v        toInt     toFloat                                            == != < > <= >=
    All modifiers
    are string
    (substrings)

  integer                     v        convert                                            == != < > <= >=

  float                     convert     v                                                 == != < > <= >=

  Date           check                          v                                         == != < > <= >=
    All modifiers
    are integers
  (year,month,...)

  checkSum                                                v                               ==

  bool                                                                v                   == != AND OR
  other operand
  must be bool


  ZResource                                                                   v
    All modifiers
    are integers
    (ZEntity,id)

*/
ZStatus ZSearchParser::_parseOneLogicalOperand(void *&pOperand,
                                               ZSearchContext &pContext,
                                               ZSearchOperandType_type &pMainType,
                                               int pParenthesisLevel)
{
  int wStartOperandIndex=pContext.Index;
  pOperand=nullptr;
  ZStatus wSt=ZS_SUCCESS;

  while (true) {

    if (pContext.CurrentToken->Type == ZSRCH_OPENPARENTHESIS) {
      if (!pContext.advanceIndex(true)) {
        return pContext.Status=ZS_SYNTAX_ERROR;
      }
      pParenthesisLevel++;
      wSt=_parseLogicalTerm((ZSearchLogicalTerm* &)pOperand,pContext,pParenthesisLevel+1);
      if (wSt!=ZS_SUCCESS) {
        return wSt;
      }
      pMainType = ZSTO_Bool;/* result of a logical term operand is a bool */

      if (pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
        ErrorLog.errorLog("Syntax error : Closed parenthesis expected. found <%s> - at line %d column %d",
            pContext.CurrentToken->Text.toString(),
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;

        return pContext.Status=ZS_SYNTAX_ERROR;
      }
      if (pParenthesisLevel<0) {
        ErrorLog.errorLog("Syntax error : Closed parenthesis without corresponding open parenthesis - at line %d column %d",
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;

        return pContext.Status=ZS_SYNTAX_ERROR;
      }

      if (!pContext.advanceIndex(true)) {
        return pContext.Status=ZS_SYNTAX_ERROR;
      }

      break;  /* Operand is set as a new logical formula */
    }// ZSRCH_OPENPARENTHESIS

    if (pContext.CurrentToken->Type==ZSRCH_IDENTIFIER) {

      /* check if symbol or ZEntity */

      if (_parseZEntity(pOperand,pContext,pMainType) ) {
        break;
      }
      if (_getSymbol(pOperand,pContext,pMainType) ) {
        /* Capture base type of operand */
        break;
      }

      wSt=_parseOperandField(pOperand,pContext,pMainType);
      if (wSt!=ZS_SUCCESS) {
        return wSt;
      }

      break;
    } // ZSRCH_IDENTIFIER

    if ((pContext.CurrentToken->Type & ZSRCH_LITERAL)==ZSRCH_LITERAL) {
      ZSearchLiteral* wOperand=nullptr;
      wSt=_parseLiteral(wOperand,pContext,pMainType);
      if (wSt!=ZS_SUCCESS) {
        return wSt;
      }
      pOperand=wOperand;
      //        wTerm->Operand1.Type = ZSTO_Literal;
      break;
    } // ZSRCH_LITERAL
    break;
  } // while true

  if (ZSearchOperator::isArithmeric(pContext.CurrentToken)) {
    bool wIsLiteral=true;
    clearOperand(pOperand);

    pContext.setIndex(wStartOperandIndex);
    wSt=_parseArithmeticTerm((ZSearchArithmeticTerm* &)pOperand,
                               pContext,
                               pParenthesisLevel,
//                               wStartOperandIndex,
                               pMainType,
                               wIsLiteral);
    if (wSt!=ZS_SUCCESS) {
      return wSt;
    }
    /* if all components of arithmetic operation are literal, then arithmetic term is once evaluated and its value is stored and never recomputed */

    if (wIsLiteral) {

      utf8VaryingString wFormula ;

      for (int wi=wStartOperandIndex; wi < pContext.Index; wi++) {
        wFormula += pContext.CurrentTokenList.Tab(wi)->Text;
        wFormula += " ";
      }

    ZSearchOperandType_type wType = ZSearchOperandType_type(static_cast<ZSearchArithmeticTerm*>(pOperand)->ZSTO & ZSTO_BaseMask);
    ErrorLog.infoLog("Arithmetic expression [%s] is a strict literal of type <%s> ", wFormula.toString(),decode_OperandType(wType));

    ZOperandContent wResult;
    ZSearchArithmeticTerm* wSLOP=static_cast<ZSearchArithmeticTerm*>(pOperand);
    wSt=computeArithmeticLiteral(wSLOP,wType,wResult);
    if ((wSt!=ZS_SUCCESS)&&(wSt!=ZS_EOF)) {
        ErrorLog.errorLog("Arithmetic expression [%s] is an invalid literal expression. Returned status is< %s>", wFormula.toString(),
                 decode_ZStatus(wSt));
        clearOperand(pOperand);
        return wSt;
    }

    switch (wResult.OperandZSTO)
    {
      case ZSTO_Integer:
      {
        ZSearchLiteral* wLit = new ZSearchLiteral;
        clearOperand(pOperand);
        wLit->setZSTO (ZSTO_LiteralInteger);
        wLit->setInteger( wResult.getInteger() );
        wLit->FullFieldName = wFormula;
//        wLit->Comment = wFormula;
        if (!OperandTypeCheck(pMainType,pContext,wLit,wStartOperandIndex)) {
            delete wLit;
            ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                     pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                     pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
            return pContext.Status=ZS_INVTYPE;
        }
        pOperand = wLit;
        return pContext.Status=ZS_SUCCESS;
      }
      case ZSTO_Float:
      {
        ZSearchLiteral* wLit = new ZSearchLiteral;
        wLit->setZSTO(  ZSTO_LiteralFloat);
        wLit->setFloat(wResult.getFloat());
        wLit->FullFieldName = wFormula;
//        wLit->Comment = wFormula;
        if (!OperandTypeCheck(pMainType,pContext,wLit,wStartOperandIndex)) {
            delete wLit;
            ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                     pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                     pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
            return pContext.Status=ZS_INVTYPE;
        }
        pOperand = wLit;
        return pContext.Status=ZS_SUCCESS;
      }
      case ZSTO_UriString:
      case ZSTO_String:
      {
        ZSearchLiteral* wLit = new ZSearchLiteral;
        wLit->setZSTO ( ZSTO_LiteralString);
        wLit->setString(wResult.getString());
        wLit->FullFieldName = wFormula;
//        wLit->Comment = wFormula;
        if (!OperandTypeCheck(pMainType,pContext,wLit,wStartOperandIndex)) {
            delete wLit;
            ErrorLog.errorLog("Operand type check failed at token <%s> at line %d column %d.",
                     pContext.CurrentTokenList.Tab(wStartOperandIndex)->Text.toString(),
                     pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenLine,pContext.CurrentTokenList.Tab(wStartOperandIndex)->TokenColumn );
            return pContext.Status=ZS_INVTYPE;
        }
        pOperand = wLit;
        return pContext.Status=ZS_SUCCESS;
      }

 /*   No date allowed in an arithmetic operation
     case ZSTO_Date:
      {
        ZSearchLiteral<ZDateFull>* wLit = new ZSearchLiteral<ZDateFull>;
        wLit->Content = wResult.Date;
        wLit->Comment = wFormula;
        break;
      }
*/
      default:
        _DBGPRINT("_parseOneLogicalOperand Arithmetic expression is a strict literal of type <%s>\n",decode_OperandType(wType))
        clearOperand(pOperand);
        return pContext.Status=ZS_INVOP;
      }// switch

   } // is a full literal

    /* arithmetic expression involving at least one field :
     * pOperand is already loaded with arithmetic terms chain */

    return pContext.Status=ZS_SUCCESS;
  } // operator is arithmetic


  return pContext.Status=ZS_SUCCESS;
} // _parseOneLogicalOperand

/*
ZStatus
ZSearchParser::_parseLogicalTerm(ZSearchLogicalTerm* & pTerm,
                                 ZSearchContext& pContext,
                                 int pParenthesisLevel,
                                 int pBookMark)
*/
ZStatus
ZSearchParser::_parseLogicalTerm(ZSearchLogicalTerm* & pTerm,
                                 ZSearchContext& pContext,
                                 int pParenthesisLevel)
{

  pTerm=nullptr;

//  ZSearchOperandType_type wOpType = ZSTO_Nothing;
  ZStatus wSt=ZS_SUCCESS;

  ZSearchLogicalTerm* wTerm = new ZSearchLogicalTerm;
  wTerm->ParenthesisLevel = pParenthesisLevel;
  //  wTerm->Collateral = pCollateral;

//  pContext.setIndex(pBookMark);


  if (pContext.CurrentToken->Type == ZSRCH_OPERATOR_NOT)  {
    wTerm->NotOperator.set(pContext.CurrentToken);
    if (!pContext.advanceIndex(true)) {
      delete wTerm;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
  }

  wTerm->MainType = ZSTO_Nothing;

  /* first operand acquisition */

  wSt = _parseOneLogicalOperand(wTerm->Operand1,pContext,wTerm->MainType,pParenthesisLevel);
  if (wSt!=ZS_SUCCESS) {
    delete wTerm;
    return wSt;
  }

  /* compare operator */

  if (pContext.isEOF() ||(!ZSearchOperator::isComparator(pContext.CurrentToken))) {
    ErrorLog.errorLog("Missing logical operator at line %d column %d. Found <%s>.",
                      pContext.CurrentToken->TokenLine,pContext.CurrentTokenList.Tab(pContext.Index+2)->TokenColumn,
                      pContext.CurrentToken->Text.toString());
    pContext.LastErroredToken = pContext.CurrentToken;
    delete wTerm;
    return pContext.Status=ZS_INV_OPERATOR;
  }// is not a comparator

  wTerm->CompareOperator.set(pContext.CurrentToken);

  _DBGPRINT ("ZSearchParser::_parseLogicalTerm Found operator <%s>\n",decode_ZSOPV(wTerm->CompareOperator.ZSOPV))

  if (!pContext.advanceIndex(true)) {
    delete wTerm;
    return pContext.Status=ZS_SYNTAX_ERROR;
  }

  /*----------Operand 2 ----------------*/
//  wSvIndex = pContext.Index;

  wSt = _parseOneLogicalOperand(wTerm->Operand2,pContext,wTerm->MainType,pParenthesisLevel);
  if (wSt!=ZS_SUCCESS) {
    delete wTerm;
    return wSt;
  }

  if (pContext.isEOF()) {
      pTerm=wTerm;
      return pContext.Status=ZS_SUCCESS;
  }

  if (ZSearchOperator::isAndOr(pContext.CurrentToken)) {
    wTerm->AndOrOperator.set(pContext.CurrentToken);
    if (!pContext.advanceIndex(true)) {
      delete wTerm;
      return pContext.Status=ZS_SYNTAX_ERROR;
    }
    wSt=_parseLogicalTerm(wTerm->NextTerm,pContext,pParenthesisLevel);
    if (wSt!=ZS_SUCCESS) {
      delete wTerm;
      return wSt;
    }
  }

  pTerm=wTerm;
  return pContext.Status=ZS_SUCCESS;

}//ZSearchParser::_parseLogicalTerm

bool ZSearchParser::_arithmeticTypeCheck(ZSearchOperandType_type &pMainType,
                                         ZSearchContext &pContext,
                                         ZSearchOperandBase *pOB,
                                         int pIndex)
{
  ZSearchOperandType_type wOpBaseType = ZSearchOperandType_type(pOB->ZSTO & ZSTO_BaseMask);
  switch (pMainType)
  {
  case ZSTO_Nothing:  /* set main type */

    switch (wOpBaseType)
    {
    case ZSTO_UriString:
    case ZSTO_String:  /* despite any modifier, type is string */
      pMainType=ZSTO_String;
      return true;
    case ZSTO_Integer:
    case ZSTO_Float:
      pMainType = wOpBaseType ;
      return true;
    case ZSTO_Date:
      if (pOB->hasModifier()) {
        pMainType = ZSTO_Integer;
        return true;
      }
      pMainType = ZSTO_Date;
      return true;
    case ZSTO_Resource:
      if (pOB->hasModifier()) {
        pMainType = ZSTO_Integer;
        return true;
      }
      pMainType = ZSTO_Resource;
      return true;
    case ZSTO_Checksum:
      pMainType = ZSTO_Checksum;
      return true;
    case ZSTO_Bool:
      ErrorLog.errorLog("Arithmetic operand type mismatch: invalid operand type Bool in arithmetic expression at line %d column %d.",
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }// switch

    /* ???? */
    pMainType = wOpBaseType ;
    return true;

  case ZSTO_UriString:
  case ZSTO_String:
    if (wOpBaseType!=ZSTO_String) {
      ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type string while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    break;
  case ZSTO_Integer:
  case ZSTO_Float:

    if ((wOpBaseType==ZSTO_Float) || (wOpBaseType==ZSTO_Integer))
      return true;

    if (wOpBaseType == ZSTO_Date) {
      if (pOB->hasModifier())
        return true;
      ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    if (wOpBaseType == ZSTO_Date) {
      if (pOB->hasModifier())
        return true;
      ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    if (wOpBaseType == ZSTO_Resource) {
      if (pOB->hasModifier())
        return true;
      ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
        decode_OperandType(pMainType),
        decode_OperandType(wOpBaseType),
        pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
        pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
    pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
    return false;

  case ZSTO_Date:
    /* here must be strictly date */
    if (wOpBaseType!=ZSTO_Date) {
      ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    return true;

  case ZSTO_Resource:
    /* here must be strictly resource */
    if (wOpBaseType!=ZSTO_Resource) {
      ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    return true;
  case ZSTO_Bool:
    ErrorLog.errorLog("Arithmetic operand type mismatch: invalid operand with type Bool in an arithmetic expression. Token  <%s> at line %d column %d.",
        pContext.CurrentToken->Text.toString(),
        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
    return false;
  case ZSTO_Checksum:
    if (wOpBaseType!=ZSTO_Checksum) {
      ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    return true;
  default:
    ErrorLog.errorLog("Arithmetic operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
        decode_OperandType(pMainType),
        decode_OperandType(wOpBaseType),
        pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
        pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
    return false;
  }//swich

} // ZSearchParser::_arithmeticTypeCheck


bool ZSearchParser::OperandTypeCheck(ZSearchOperandType_type& pMainType,ZSearchContext& pContext,ZSearchOperandBase* pOB, int pIndex)
{
  ZSearchOperandType_type wOpBaseType = ZSearchOperandType_type(pOB->ZSTO & ZSTO_BaseMask);
  switch (pMainType)
  {
  case ZSTO_Nothing:  /* set main type */

    switch (wOpBaseType)
    {
    case ZSTO_UriString:
    case ZSTO_String:  /* despite any modifier, type is string */
      pMainType=ZSTO_String;
      return true;
    case ZSTO_Integer:
    case ZSTO_Float:
      pMainType = wOpBaseType ;
      return true;
    case ZSTO_Date:
      if (pOB->hasModifier()) {
        pMainType = ZSTO_Integer;
        return true;
      }
      pMainType = ZSTO_Date;
      return true;
    case ZSTO_Resource:
      if (pOB->hasModifier()) {
        pMainType = ZSTO_Integer;
        return true;
      }
      pMainType = ZSTO_Resource;
      return true;
    case ZSTO_Checksum:
      pMainType = ZSTO_Checksum;
      return true;
    case ZSTO_Bool:
      pMainType = ZSTO_Bool;
      return true;
    }// switch

    /* ???? */
    pMainType = wOpBaseType ;
    return true;

  case ZSTO_UriString:
  case ZSTO_String:
    if (wOpBaseType!=ZSTO_String) {
        ErrorLog.errorLog("Operand type mismatch: expected operand with type string while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    break;
  case ZSTO_Integer:
  case ZSTO_Float:

    if ((wOpBaseType==ZSTO_Float) || (wOpBaseType==ZSTO_Integer))
      return true;

    if (wOpBaseType == ZSTO_Date) {
      if (pOB->hasModifier())
        return true;
      ErrorLog.errorLog("Operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    if (wOpBaseType == ZSTO_Date) {
      if (pOB->hasModifier())
        return true;
      ErrorLog.errorLog("Operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    if (wOpBaseType == ZSTO_Resource) {
      if (pOB->hasModifier())
        return true;
      ErrorLog.errorLog("Operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentToken;
      return false;
    }
    ErrorLog.errorLog("Operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
        decode_OperandType(pMainType),
        decode_OperandType(wOpBaseType),
        pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
        pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
    pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
    return false;

  case ZSTO_Date:
    /* here must be strictly date */
    if (wOpBaseType!=ZSTO_Date) {
      ErrorLog.errorLog("Operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    return true;

  case ZSTO_Resource:
    /* here must be strictly resource */
    if (wOpBaseType!=ZSTO_Resource) {
      ErrorLog.errorLog("Operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    return true;
  case ZSTO_Bool:
    ErrorLog.errorLog("Arithmetic operand type mismatch: invalid operand with type Bool in an arithmetic expression. Token  <%s> at line %d column %d.",
        pContext.CurrentToken->Text.toString(),
        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
    return false;
  case ZSTO_Checksum:
    if (wOpBaseType!=ZSTO_Checksum) {
      ErrorLog.errorLog("Operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
          pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
      return false;
    }
    return true;
  default:
    ErrorLog.errorLog("Operand type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
        decode_OperandType(pMainType),
        decode_OperandType(wOpBaseType),
        pContext.CurrentTokenList.Tab(pIndex)->Text.toString(),
        pContext.CurrentTokenList.Tab(pIndex)->TokenLine,pContext.CurrentTokenList.Tab(pIndex)->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentTokenList.Tab(pIndex);
    return false;
  }//swich

} // ZSearchParser::_logicalTypeCheck

ZStatus ZSearchParser::_parseArithmeticTerm(ZSearchArithmeticTerm *&pArithTerm,
                                            ZSearchContext &pContext,
                                            int pParenthesisLevel,
//                                            int pBookMark,
                                            ZSearchOperandType_type &pRequestedType,
                                            bool &pIsLiteral)
{
  ZStatus wSt=ZS_SUCCESS;

  pArithTerm=nullptr;

//  pContext.setIndex(pBookMark);

  int wStartOperandIndex=pContext.Index;

  ZSearchArithmeticTerm* wArithTerm = new ZSearchArithmeticTerm;
  wArithTerm->ParenthesisLevel = pParenthesisLevel;

  while (true) {

    if (pContext.CurrentToken->Type==ZSRCH_OPENPARENTHESIS) {
      if (!pContext.advanceIndex()) {
        delete wArithTerm;
        return pContext.Status=ZS_SYNTAX_ERROR;
      }
      wSt=_parseArithmeticTerm((ZSearchArithmeticTerm*&)wArithTerm->Operand,
                                 pContext,
                                 pParenthesisLevel+1,
//                                 pContext.Index,
                                 pRequestedType,
                                 pIsLiteral);
      if (wSt!=ZS_SUCCESS) {
        delete wArithTerm;
        return pContext.Status=wSt;
      }
      if (pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
        ErrorLog.errorLog("Syntax error: expected closing parenthesis while found <%s> at line %d column %d.",
            pContext.CurrentToken->Text.toString(),
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
          pContext.LastErroredToken = pContext.CurrentToken;
        delete wArithTerm;
        return pContext.Status=ZS_MISS_PUNCTSIGN;
      }
      break; /* first operand is stored as an arithmetic formula */
    } //ZSRCH_OPENPARENTHESIS


    if (pContext.CurrentToken->Type==ZSRCH_IDENTIFIER) {

      if (_parseZEntity(wArithTerm->Operand,pContext,pRequestedType) ) {
        break;
      }
      if (_getSymbol(wArithTerm->Operand,pContext,pRequestedType) ) {
        break;
      }

      wSt=_parseOperandField(wArithTerm->Operand,pContext,pRequestedType);
      if (wSt!=ZS_SUCCESS) {
        delete wArithTerm;
        return pContext.Status=wSt;
      }
      pIsLiteral = false;
      break;
    } // ZSRCH_IDENTIFIER

    if ((pContext.CurrentToken->Type & ZSRCH_LITERAL)==ZSRCH_LITERAL) {
      ZSearchLiteral* wOperand=nullptr;
      wSt=_parseLiteral(wOperand,pContext,pRequestedType);
      if (wSt!=ZS_SUCCESS) {
        delete wArithTerm;
        return pContext.Status=wSt;
      }
      wArithTerm->Operand=wOperand;
      break;
    } // ZSRCH_LITERAL

    ErrorLog.errorLog("Syntax error: expected either an identifier or a litteral while found <%s> at line %d column %d.",
        pContext.CurrentToken->Text.toString(),
        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
    pContext.LastErroredToken = pContext.CurrentToken;
    return pContext.Status=ZS_INVVALUE;
  }// while true

  ZSearchOperandBase* wOB=static_cast<ZSearchOperandBase*>(wArithTerm->Operand);
  if (!_arithmeticTypeCheck(pRequestedType,pContext,wOB,wStartOperandIndex)) {
      delete wArithTerm;
      return pContext.Status=ZS_INVTYPE;
  }

  wArithTerm->ZSTO = ZSearchOperandType_type( ZSTO_Arithmetic | (wOB->ZSTO & ZSTO_BaseMask));  /* extract the base type (string, integer,etc.) from operand */

  /* if not an arithmetic operator, then arithmetic expression is ended */
  if (!ZSearchOperator::isArithmeric(pContext.CurrentToken)) {
    wArithTerm->Operator.ZSOPV=ZSTO_Nothing;
    wArithTerm->OperandNext = nullptr;
    pArithTerm = wArithTerm;
    return pContext.Status=ZS_SUCCESS ;
  }

  wArithTerm->Operator.set(pContext.CurrentToken);

  if (!pContext.advanceIndex(true)) {
    delete wArithTerm;
    return pContext.Status=ZS_SYNTAX_ERROR;
  }
  /*  parse next operand */

  wSt=_parseArithmeticTerm((ZSearchArithmeticTerm*&)wArithTerm->OperandNext,
                           pContext,
                           pParenthesisLevel,
//                           pContext.Index,
                           pRequestedType,
                           pIsLiteral);
  if (wSt!=ZS_SUCCESS) {
    delete wArithTerm;
    return pContext.Status=wSt;
  }

  pArithTerm = wArithTerm;
  pContext.popIndex();
  return pContext.Status=wSt;
}


/*
 * Example of arithmetic expression and related storage structure
 *
   <field> + <literal> - ( <field> + <literal> / (<literal> + <field> )  ) - <literal>

  ZSearchArith
  -----------------
(op <field>)  (oper +)   (next ZSearchArith)

                        (op <literal>) (oper -)  (next ZSearchArith)

                                                  (op ZSearchArith)    (oper -)   (next ZSearchArith)

                                                                                      (op <literal>) (oper nothing) (next nullptr)

                                                  (op <literal>)  (oper +)  (next ZSearchArith)

                                                                              (op <literal> (oper /) (next ZSearchArith)

                                                                                                        (op <literal>)  (oper +) (next ZSearchArith)

                                                                                                                                  (op <field>) (oper nothing) (next nullptr)



*/





/*
  +--------------------------+--> leading link operator
  |                          |                      ^       +-> open parenthesis = new embedded formula    +--> end embedded formula
  |                          |                      |       |                                              |
[NOT] <field> ==  <literal> AND <field> < <literal> OR [NOT]( <field> == <literal> AND <field> > <literal> ) AS
                                                            <----term 31------->     <------term 32---->
     <-----term 1------->     <-------term 2---->           <-----Formula 21----> <-----Formula 22---->
                                                               ^              |    |
                                                               |              +----+
                                                             embedded(term is formula)
     <---formula 0-----------> <-------formula 1------> <-------formula 2---->
                            |    |                  |    |
                            +----+                  +----+
                           chained

some rules:

  a term is a tuple of 2 {field,literal} separated by a logical operator {==,<,<=,>,>=,contains,starts with,ends with}
    that gives a result true or false.

  a formula combines the boolean result of a term or an embedded list of formulas
  to the following one using its logical operator {AND,OR} and possibly NOT

  NOT must immediately be followed by open parenthesis

  each term (see definition before) is immediately followed by a log op {AND,OR}, or nothing if end of formula list

  open parenthesis generates an embedded formula


terms operands allowed conversions before logical operation :



    Resource
        no modifier
        ZEntity (Integer)
        Id      (Integer)

    Date
        no modifier
        Month...(Integer)

    Checksum is only allowed to be compared with Checksum


Term operand

  a term operand may be
    a field, prefixed or not according the number of current entities, with or without modifier
    a literal with or without modifiers

    a literal expression using literals linked by operators
    a field expression mixing fields and/or literals linked by operators


    an expression is in place when a valid operator is found :
      arithmetic operator
      string  operator

    As a field or a literal, an expression has a type (string, integer, float, date,etc.) according the nature of its result.

*/

#ifdef __COMMENT__
ZStatus ZSearchParser::_parseFormula(ZSearchFormula* &pFormula,int pParenthesisLevel)
{
  ZStatus wSt=ZS_SUCCESS;
  int wBookMark = pContext.Index;

  pFormula = new ZSearchFormula(ZFORT_Term,pParenthesisLevel);

  if (pContext.CurrentToken->Type == ZSRCH_OPERATOR_NOT)  {
    pFormula->LeadingOperator.add(pContext.CurrentToken);
    pFormula->LeadingOperator.Type |= ZSOPV_NOT ;
    if (!pContext.advanceIndex())
      return ZS_SYNTAX_ERROR;
  }

  if (pContext.CurrentToken->Type == ZSRCH_OPENPARENTHESIS)
    {
    if (!pContext.advanceIndex())
      return ZS_SYNTAX_ERROR;
      pParenthesisLevel++;
      pFormula->setType(ZFORT_Formula);
      ZSearchFormula* wFormulaPtr=nullptr;
      wSt=_parseFormula(wFormulaPtr,pParenthesisLevel);
      if (wSt!=ZS_SUCCESS)
        goto _parseFormulaError;

      pFormula->TermOrFormula = wFormulaPtr;

      if (pContext.CurrentToken->Type!=ZSRCH_CLOSEPARENTHESIS) {
        ErrorLog.errorLog("Syntax error : Closed parenthesis expected. found <%s> - at line %d column %d",
            pContext.CurrentToken->Text.toString(),
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);

        wSt = ZS_SYNTAX_ERROR ;
        goto _parseFormulaError;
      }
      if (pParenthesisLevel<0) {
          ErrorLog.errorLog("Syntax error : Closed parenthesis without corresponding open parenthesis - at line %d column %d",
              pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);

          wSt = ZS_SYNTAX_ERROR ;
          goto _parseFormulaError;
      }

      if (!pContext.advanceIndex())
        return ZS_SYNTAX_ERROR;

    }// ZSRCH_OPENPARENTHESIS

    if ((pContext.CurrentToken->Type == ZSRCH_IDENTIFIER) ||
        ((pContext.CurrentToken->Type & ZSRCH_LITERAL)==ZSRCH_LITERAL)) {

      ZSearchLogicalOperand* wTerm=nullptr;
      wSt=_parseLogical((void*)wTerm,0,0,pContext.Index);
      if (wSt!=ZS_SUCCESS)
        goto _parseFormulaError;

      pFormula->setType(ZFORT_Term);
      pFormula->TermOrFormula=wTerm;

//      advanceIndex(pContext);

    } else  {
      ErrorLog.errorLog("Syntax error : Expected either field identifier or literal at line %d column %d. Found <%s>.",
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,
          pContext.CurrentToken->Text.toString());

      wSt = ZS_SYNTAX_ERROR ;
      goto _parseFormulaError;
    }

  /* getting link logical operator that will link formula */



  if ((pContext.CurrentToken->Type == ZSRCH_OPERATOR_AND) ||
        (pContext.CurrentToken->Type == ZSRCH_OPERATOR_OR)) {
    pFormula->LeadingOperator.add(pContext.CurrentToken);
    if (!pContext.advanceIndex())
      return ZS_SYNTAX_ERROR;
  }
  else if (pContext.CurrentToken->Type != ZSRCH_AS) {
    ErrorLog.errorLog("Syntax error : Expected logical operator one of {AND,OR} at line %d column %d. Found <%s>.",
        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,
        pContext.CurrentToken->Text.toString());

    wSt = ZS_SYNTAX_ERROR ;
    goto _parseFormulaError;
  } // not = 'AS'

  if (pContext.CurrentToken->Type == ZSRCH_OPERATOR_NOT)  {

    pFormula->LeadingOperator.add(pContext.CurrentToken);
    if (!pContext.advanceIndex())
      return ZS_SYNTAX_ERROR;

    /* NB: NOT operand must be followed by expression within parenthesis */
    if (pContext.CurrentToken->Type != ZSRCH_OPENPARENTHESIS)
    {
      ErrorLog.errorLog("Syntax error : Logical operator NOT must be followed by parenthesis at line %d column %d. Found <%s>.",
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,
          pContext.CurrentToken->Text.toString());

      wSt = ZS_SYNTAX_ERROR ;
      goto _parseFormulaError;
    }

  } // ZSRCH_OPERATOR_NOT


  /* getting next formula */

  if ((ZSearchTokenizer::count() < pContext.Index) &&
      (pContext.CurrentToken->Type!= ZSRCH_AS) &&
      (pContext.CurrentToken->Type!= ZSRCH_SEMICOLON)) {

    ZSearchFormula* wNextFormula=nullptr;

    wSt=_parseFormula(wNextFormula,pParenthesisLevel);
    if (wSt!=ZS_SUCCESS)
      goto _parseFormulaError;

    pFormula->NextFormula = wNextFormula;
  }

  return ZS_SUCCESS;

_parseFormulaError:
  delete pFormula;
  pFormula=nullptr;
  return wSt ;
} // _parseFormula



ZStatus ZSearchParser::_parseSelectionClause(ZSearchFormula *&pOutFormula, const utf8VaryingString& pEntityName, const ZMetaDic* pDictionary)
{

  ZStatus wSt=ZS_SUCCESS;

    wSt=_parseFormula(pOutFormula,0);
    if (wSt!=ZS_SUCCESS)
      return wSt;

  return ZS_SUCCESS;
}

#endif // __COMMENT__


bool
ZSearchParser::searchKeyword(ZSearchToken* pToken) {
  for (int wi=0; wi < KeywordList.count();wi++) {
    if (KeywordList[wi].Text.compareCase(pToken->Text)==0) {
      pToken->Type = KeywordList[wi].Type;
      return true;
    }
  }
  return false;
}

#ifdef __DEPRECATED__
/*
ZStatus ZSearchParser::_parse(std::shared_ptr<ZSearchEntity> &pCollection,
                              bool &pStoreInstruction,
                              int &pInstructionType)
*/

ZStatus ZSearchParser::_parse(ZSearchContext& pContext)
{
//  pContext.clear();
  pContext.Status=ZS_SUCCESS;
  pContext.begin();

//  pContext.TokenStart = FirstToken = pContext.CurrentToken;
 pContext.CurrentToken = pContext.TokenStart = pContext.CurrentToken;

 // CurrentToken=nullptr;
  pContext.HasInstruction=false;

  while ((pContext.Status!=ZS_EOF)&&(pContext.Index < ZSearchTokenizer::count())) {

    while ((pContext.Index < ZSearchTokenizer::count())&&(pContext.CurrentToken->Type==ZSRCH_SPACE))
      if (!pContext.advanceIndex())
        return pContext.Status=ZS_SYNTAX_ERROR;

//    CurrentToken=pContext.CurrentToken;

    if ((pContext.CurrentToken->Type & ZSRCH_INSTRUCTION_MASK)!=ZSRCH_INSTRUCTION_MASK) {

     ZArray<utf8VaryingString> wKeyWords = { "DECLARE","SET","FIND","FOR" };

      ErrorLog.errorLog("Missing instruction at line %d column %d one of {DECLARE,SET,FIND,FOR}. Found <%s>.",
                        pContext.CurrentToken->TokenLine,
                        pContext.CurrentToken->TokenColumn,
                        pContext.CurrentToken->Text.toString() );
      pContext.LastErroredToken = pContext.CurrentToken;
//      utf8VaryingString wApprox = searchKeywordWeighted(pContext.CurrentToken->Text);
      utf8VaryingString wApprox = searchAdHocWeighted(pContext.CurrentToken->Text,wKeyWords);

      if (!wApprox.isEmpty()) {
          ErrorLog.textLog("Do you mean <%s> ?", wApprox.toString() );
      }
      return pContext.Status=ZS_MISS_KEYWORD;
    }

    while (pContext.notEOF()) {

//      CurrentToken=pContext.CurrentToken;

      switch (pContext.CurrentToken->Type) {


      case ZSRCH_DECLARE:
      {
          pContext.Status = _parseDeclare(pContext);
          if ((pContext.Status==ZS_SUCCESS) && !pContext.isEOF()&& !pContext.testSemiColon())
              return pContext.Status=ZS_MISS_PUNCTSIGN;
          return pContext.Status;
      }// ZSRCH_DECLARE

      case ZSRCH_SAVE:
      {
          pContext.Status = _parseSave(pContext);
          if ((pContext.Status==ZS_SUCCESS) && !pContext.isEOF()&& !pContext.testSemiColon())
              return pContext.Status=ZS_MISS_PUNCTSIGN;
          return pContext.Status;
      }// ZSRCH_DECLARE

      case ZSRCH_SET:
        {
          pContext.InstructionType = ZSITP_Set ;
          pContext.Store=ZSearchHistory::DataExecute;

          if (!pContext.advanceIndex())
              return ZS_SYNTAX_ERROR;

//          CurrentToken=pContext.CurrentToken;

           /* set History xxxx */

          if (pContext.CurrentToken->Type ==ZSRCH_HISTORY) {
              if (!pContext.advanceIndex())
                  return pContext.Status=ZS_SYNTAX_ERROR;

              if (pContext.CurrentToken->Type != ZSRCH_MAXIMUM) {
                  ErrorLog.errorLog("Missing required word <MAXIMUM> at line %d column %d . Found <%s>.",
                           pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
                  pContext.LastErroredToken = pContext.CurrentToken;
                  return pContext.Status=ZS_MISS_KEYWORD;
              }
              if (!pContext.advanceIndex())
                  return pContext.Status=ZS_SYNTAX_ERROR;

              if (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL) {
                  ErrorLog.errorLog("Missing numeric literal at line %d column %d . Found <%s>.",
                           pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
                  pContext.LastErroredToken = pContext.CurrentToken;
                  return pContext.Status=ZS_MISS_LITERAL;
              }

              int wNewMax = pContext.CurrentToken->Text.toInt();

              setHistoryMaximum(wNewMax);

              if (!pContext.isEOF()&& !pContext.testSemiColon())
                  return pContext.Status=ZS_MISS_PUNCTSIGN;

              return pContext.Status=ZS_SUCCESS;

          } //   ZSRCH_HISTORY


          if (pContext.CurrentToken->Type !=ZSRCH_FILE) {
              ErrorLog.errorLog("Missing required word <FILE> at line %d column %d . Found <%s>.",
                       pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
              pContext.LastErroredToken = pContext.CurrentToken;
              return pContext.Status=ZS_MISS_KEYWORD;
          }

          if (!pContext.advanceIndex())
              return pContext.Status=ZS_SYNTAX_ERROR;

          pContext.Status=_parseDeclareFile(pContext);
          if ((pContext.Status==ZS_SUCCESS) && !pContext.isEOF()&& !pContext.testSemiColon())
              return pContext.Status=ZS_MISS_PUNCTSIGN;
          if (pContext.Status!=ZS_SUCCESS)
              return pContext.Status;
          pContext.HasInstruction=true;
          return pContext.Status;
        }// ZSRCH_SET

      case ZSRCH_SHOW:
      {
        pContext.InstructionType = ZSITP_Show;
        pContext.Store=ZSearchHistory::Other;
        if (!pContext.advanceIndex())
          return ZS_SYNTAX_ERROR;
        pContext.Status = _parseShow(pContext);

        /* end instruction line management :
         * in case of error : return status immediately
         * in case of success :
         * if end of tokens -> instruction is finished
         * if not end of tokens :
         *   if semi-colon : another instruction line may follow or semi-colon is the last token
         */
        if ((pContext.Status==ZS_SUCCESS) && !pContext.isEOF()&& !pContext.testSemiColon())
            return pContext.Status=ZS_MISS_PUNCTSIGN;
        if (!pContext.isEOF())
            pContext.advanceIndex();

        if (pContext.Status!=ZS_SUCCESS)
          return pContext.Status;
        pContext.HasInstruction=true;
        return pContext.Status;
      }
/*

  clear history ;       -> removes ALL instruction lines in the history. History is then empty.

  clear history first 5; -> removes the 5 FIRST instruction lines in the history or removes all the first lines until 3.

  clear history last 3;  -> removes the 3 LAST instruction lines in the history or removes all the last lines until 3.

  clear history at 3 ; -> removes the third instruction line in the history. If rank 3 does not exist in history, nothing is done.

*/
      case ZSRCH_CLEAR:
      {
        pContext.InstructionType = ZSITP_Clear;
        pContext.Store=false;
        if (!pContext.advanceIndex())
          return pContext.Status=ZS_SYNTAX_ERROR;
        if (pContext.CurrentToken->Type != ZSRCH_HISTORY) {
          ErrorLog.errorLog("Missing required word <HISTORY> at line %d column %d . Found <%s>.",
              pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
          return pContext.Status=ZS_MISS_KEYWORD;
        }
        if (!pContext.advanceIndex())
          return pContext.Status=ZS_SYNTAX_ERROR;

        if (pContext.CurrentToken->Type == ZSRCH_SEMICOLON) {
          if (!pContext.advanceIndex())
            return pContext.Status=ZS_SYNTAX_ERROR;
          clearHistory();
          break;
        }

        if (pContext.CurrentToken->Type == ZSRCH_FIRST) {
          if (!pContext.advanceIndex())
            return ZS_SYNTAX_ERROR;
          if (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL) {
            ErrorLog.errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_LITERAL;
          }
          int wNb = pContext.CurrentToken->Text.toInt();
          if (!pContext.advanceIndex())
            return pContext.Status=ZS_SYNTAX_ERROR;

          if (!pContext.testSemiColon()) {
            return pContext.Status=ZS_MISS_PUNCTSIGN;
          }
          clearHistoryFirst(wNb);
          break;
        } // ZSRCH_FIRST

        if (pContext.CurrentToken->Type == ZSRCH_LAST) {
          if (!pContext.advanceIndex())
            return pContext.Status=ZS_SYNTAX_ERROR;
          if (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL) {
            ErrorLog.errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
            return pContext.Status=ZS_MISS_LITERAL;
          }
          int wNb = pContext.CurrentToken->Text.toInt();
          if (!pContext.advanceIndex())
            return pContext.Status=ZS_SYNTAX_ERROR;

          if (!pContext.testSemiColon()) {
            return pContext.Status=ZS_MISS_PUNCTSIGN;
          }
          clearHistoryLast(wNb);
          break;
        } // ZSRCH_LAST

        if (pContext.CurrentToken->Type == ZSRCH_AT) {
          if (!pContext.advanceIndex())
            return pContext.Status=ZS_SYNTAX_ERROR;
          if (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL) {
            ErrorLog.errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_LITERAL;
          }
          int wNb = pContext.CurrentToken->Text.toInt();
          if (!pContext.advanceIndex())
            return pContext.Status=ZS_SYNTAX_ERROR;

          if (!pContext.testSemiColon()) {
            return pContext.Status=ZS_MISS_PUNCTSIGN;
          }
          clearHistoryRank(wNb);
          break;
        } // ZSRCH_AT

          ErrorLog.errorLog("Missing required keyword one of {FIRST,LAST,AT} at line %d column %d . Found <%s>.",
                            pContext.CurrentToken->TokenLine,
                            pContext.CurrentToken->TokenColumn,
                            pContext.CurrentToken->Text.toString() );
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_MISS_KEYWORD;

      } // ZSRCH_CLEAR


      case ZSRCH_FIND:
      {
        pContext.InstructionType = ZSITP_Find;
        pContext.Store = true;
        pContext.Status=_parseFind(pContext);
        if (pContext.Status!=ZS_SUCCESS)
          return pContext.Status;

//        EntityList.push(CurEntities[0]);  // done within _parseFind()
/*
        ErrorLog.infoLog("Collection report \n %s\n",pContext.Entity->_report().toString());

        wSt= _executeFind(wCollection->_CollectionEntity);
        if (wSt!=ZS_SUCCESS) {
          return wSt;
        }
*/
        pContext.HasInstruction=true;
        break;
      } // ZSRCH_FIND
      case ZSRCH_FOR:
        pContext.InstructionType = ZSITP_For ;
        pContext.Store=true;
        if (!pContext.advanceIndex())
          return pContext.Status=ZS_SYNTAX_ERROR;
        pContext.Status=_parseFor(pContext);
        if (pContext.Status!=ZS_SUCCESS)
          return pContext.Status;
        pContext.HasInstruction=true;
        break;

      case ZSRCH_FINISH:
        if (!pContext.testSemiColon())
          return pContext.Status=ZS_MISS_PUNCTSIGN;
        pContext.InstructionType = ZSITP_Finish ;
        pContext.HasInstruction=true;
        finish();
        break;
      default:
          ErrorLog.errorLog("Keyword <%s> has not been recognized as a valid first instruction at line %d column %d .",
                            pContext.CurrentToken->Text.toString(),
                            pContext.CurrentToken->TokenLine,
                            pContext.CurrentToken->TokenColumn );
          pContext.LastErroredToken = pContext.CurrentToken;
          return ZS_SYNTAX_ERROR;
/*        else {
        ErrorLog.errorLog("Expected semicolon sign at the end of instruction at line %d column %d . Found <%s>.",
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_SYNTAX_ERROR;
        }
*/
      }// switch
    }//while

    if (pContext.Index == ZSearchTokenizer::count()) {
      if (ZSearchTokenizer::_progressCallBack!=nullptr) {
        ZSearchTokenizer::_progressCallBack(ZSearchTokenizer::count());
      }
      return pContext.Status=ZS_SUCCESS;
    }

    if (ZSearchTokenizer::_progressCallBack!=nullptr) {
      ZSearchTokenizer::_progressCallBack(int(pContext.Index));
    }

  }// main while

  if (!pContext.HasInstruction) {
    ErrorLog.warningLog("Sentence has no valid instruction." );
  }

  pContext.Phrase = pContext.MainPhrase->subString(pContext.TokenStart->TokenOffset,
                                                   pContext.CurrentToken->TokenOffset + 1 - pContext.TokenStart->TokenOffset);

  return pContext.Status=ZS_SUCCESS;
} // ZSearchParser::_parse



ZStatus
ZSearchParser::_parseDeclare(ZSearchContext& pContext)
{
    if (pContext.CurrentToken->Type!=ZSRCH_DECLARE)
        return ZS_INVTYPE;


    pContext.InstructionType = ZSITP_Declare;
    pContext.Store=ZSearchHistory::DataExecute;

    if (!pContext.advanceIndex())
        return ZS_SYNTAX_ERROR;

//    CurrentToken=pContext.CurrentToken;

    if (pContext.CurrentToken->Type ==ZSRCH_FILE) {

        if (!pContext.advanceIndex())
            return pContext.Status=ZS_SYNTAX_ERROR;

        return _parseDeclareFile(pContext);
    }

    /* if identifier then a symbol name is expected  :
         *   declare <symbol name> = <literal>  ;
         *
         *   declare gabu = ZDATE(30/01/24) ; -> creates symbol gabu with literal date value set to 30/01/24
         *   declare wResource = ZResource(ZDoc_Physical,1002) ;
        */

    if (pContext.CurrentToken->Type ==ZSRCH_IDENTIFIER) {
        return _parseDeclareSymbol(pContext);
    }
    ErrorLog.errorLog("Missing required word <FILE> or valid symbol name at line %d column %d . Found <%s>.",
             pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
    pContext.LastErroredToken = pContext.CurrentToken;
    utf8VaryingString wApprox = searchSymbolWeighted(pContext.CurrentToken->Text);
    if (!wApprox.isEmpty()) {
        ErrorLog.errorLog("Do you mean <%s> ?", wApprox.toString() );
    }
    return pContext.Status=ZS_MISS_KEYWORD;

} //ZSearchParser::_parseDeclare


/* if identifier then a symbol name is expected  :
         *   declare <symbol name> = <literal>  ;
         *
         *   declare gabu = ZDATE(30/01/24) ; -> creates symbol gabu with literal date value set to 30/01/24
         *   declare wResource = ZResource(ZDoc_Physical,1002) ;
        */
ZStatus
ZSearchParser::_parseDeclareSymbol(ZSearchContext& pContext)
{
    if (pContext.CurrentToken->Type !=ZSRCH_IDENTIFIER) {
        return pContext.Status = ZS_INVTYPE ;
    }

    /* avoid symbol name duplicates */

    if (SymbolList.exists(pContext.CurrentToken->Text)) {
        ErrorLog.errorLog("Symbol %s has already been defined.Cannot superseed at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_INVNAME ;
    }

    ZSearchSymbol wSymbol;
    wSymbol.FullFieldName = pContext.CurrentToken->Text;

    if (!pContext.advanceIndex() || (pContext.CurrentToken->Type == ZSRCH_SEMICOLON) ) {
        SymbolList.push(wSymbol);
        ErrorLog.textLog("Symbol %s has been defined at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
        return pContext.Status = ZS_SUCCESS ;
    }

    if (pContext.CurrentToken->Type !=ZSRCH_OPERATOR_MOVE) {
        ErrorLog.errorLog("Expected sign '=' while found %s at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_MISS_OPERATOR ;
    }// ZSRCH_OPERATOR_MOVE

    if (!pContext.advanceIndex())
        return pContext.Status=ZS_SYNTAX_ERROR;

    ZSearchOperandType_type wZSTO=ZSTO_Nothing;
    ZSearchLiteral *wLit=nullptr;
    pContext.Status=_parseLiteral(wLit,pContext,wZSTO);
    if (pContext.Status!=ZS_SUCCESS)
        return pContext.Status;
    wSymbol.setLiteral(wLit);
    wSymbol.ZSTO = wZSTO | ZSTO_Symbol;
    SymbolList.push(wSymbol);
    ErrorLog.textLog("Symbol %s has been defined and its value has been set at line %d column %d.",
            wSymbol.FullFieldName.toString(),
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
    pContext.Status = ZS_SUCCESS ;

    if (!pContext.advanceIndex())
        return pContext.Status=ZS_SYNTAX_ERROR;
}// ZSearchParser::_parseDeclareSymbol

#endif // __DEPRECATED__


/*
 *  set display colmax <value>
 *              colmin <value>
 *              format { + , - }    '+' adds the option, '-' removes the option
 *
 *                      current,    existing cell format value
 *                      numhexa ,   show numeric fields in hexa  (default is standard numeric representation )*
 *                      { --exclusive options --
 *                      mdy         month/day/year only
 *                      dmy         day/month/year only
 *                      dmyhms      day/month/year-hh:mm:ss
 *                      mdyhms      month/day/year-hh:mm:ss
 *                      dlocale     locale date format
 *                      dutc        UTC date format
 *                      } ,
 *                      ressymbol , Resources : displays symbol name in place of numeric value for ZEntities
 *                      resstd  ,   Resources : numeric values for Resource::id  are expressed in standard numeric representation (default is hexa)
 *
 *                      prefztype , displayed values are prefixed with explicit ZType name
 *
 *                      dumpblob    switch display to <blob content> : a byte dump to max 64 bytes
 *
 *
 *
 */


ZStatus ZSearchParser::_parseContext(ZSearchContext& pContext)
{
//  pContext.clear();
  pContext.Status=ZS_SUCCESS;
  pContext.beginContext();

  pContext.HasInstruction=false;

  if ((pContext.CurrentToken->Type & ZSRCH_INSTRUCTION_MASK)!=ZSRCH_INSTRUCTION_MASK) {

      ErrorLog.errorLog("Missing instruction at line %d column %d one of { DECLARE,SET,FIND,FOR,FETCH,SAVE,SHOW }. Found <%s>.",
                        pContext.CurrentToken->TokenLine,
                        pContext.CurrentToken->TokenColumn,
                        pContext.CurrentToken->Text.toString() );

      ZArray<utf8VaryingString> wKeyWords = { "DECLARE","SET","FIND","FOR","FETCH","SAVE", "SHOW"  };
      pContext.LastErroredToken = pContext.CurrentToken;
      utf8VaryingString wApprox = searchAdHocWeighted(pContext.CurrentToken->Text,wKeyWords);

      if (!wApprox.isEmpty()) {
          ErrorLog.textLog("Do you mean <%s> ?", wApprox.toString() );
      }
      return pContext.Status=ZS_MISS_KEYWORD;
    }

    while (pContext.notEOF() && (pContext.Status == ZS_SUCCESS) ) {

      switch (pContext.CurrentToken->Type) {

      case ZSRCH_DECLARE:
      {
          pContext.InstructionType = ZSITP_Declare;
          pContext.Status = _parseContextDeclare(pContext);
          break;
//          return pContext.Status;
      }// ZSRCH_DECLARE

      case ZSRCH_SAVE:
      {
          pContext.InstructionType = ZSITP_Save;
          pContext.Status = _parseContextSave(pContext);
          break;
//          return pContext.Status;
      }// ZSRCH_DECLARE

      case ZSRCH_SET:
        {
          pContext.InstructionType = ZSITP_Set ;
          pContext.Store=ZSearchHistory::DataExecute;

          if (!pContext.advanceIndex())
              return ZS_SYNTAX_ERROR;

           /* set History xxxx */

          if (pContext.CurrentToken->Type ==ZSRCH_HISTORY) {
              if (!pContext.advanceIndex())
                  return pContext.Status=ZS_SYNTAX_ERROR;

              if (pContext.CurrentToken->Type != ZSRCH_MAXIMUM) {
                  ErrorLog.errorLog("Missing required word <MAXIMUM> at line %d column %d . Found <%s>.",
                                    pContext.CurrentToken->TokenLine,
                                    pContext.CurrentToken->TokenColumn,
                                    pContext.CurrentToken->Text.toString() );
                  pContext.LastErroredToken = pContext.CurrentToken;
                  return pContext.Status=ZS_MISS_KEYWORD;
              }
              pContext.advanceIndex(false);

              if (pContext.isEOF() ||(pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL)) {
                  ErrorLog.errorLog("Missing numeric literal at line %d column %d . Found <%s>.",
                                    pContext.CurrentToken->TokenLine,
                                    pContext.CurrentToken->TokenColumn,
                                    pContext.CurrentToken->Text.toString() );
                  pContext.LastErroredToken = pContext.CurrentToken;
                  return pContext.Status=ZS_MISS_LITERAL;
              }

              int wNewMax = pContext.CurrentToken->Text.toInt();

              setHistoryMaximum(wNewMax);
              pContext.advanceIndex(false);

              return pContext.Status=ZS_SUCCESS;
          } //   ZSRCH_HISTORY

          if (pContext.CurrentToken->Type == ZSRCH_DISPLAY) {
              return pContext.Status=_parseSetDisplay(pContext);
          } //   ZSRCH_DISPLAY

          if (pContext.CurrentToken->Type !=ZSRCH_FILE) {
              ErrorLog.errorLog("Missing required word one of { <HISTORY> , <FILE> , <DISPLAY> } at line %d column %d. Found <%s>.",
                                pContext.CurrentToken->TokenLine,
                                pContext.CurrentToken->TokenColumn,
                                pContext.CurrentToken->Text.toString() );
              pContext.LastErroredToken = pContext.CurrentToken;
              return pContext.Status=ZS_MISS_KEYWORD;
          }

          if (!pContext.advanceIndex())
              return pContext.Status=ZS_SYNTAX_ERROR;

          pContext.Status=_parseContextDeclareFile(pContext);
          if (pContext.Status!=ZS_SUCCESS)
              return pContext.Status;
          pContext.HasInstruction=true;
          return pContext.Status;
        }// ZSRCH_SET

      case ZSRCH_SHOW:
      {
        pContext.InstructionType = ZSITP_Show;
        pContext.Store=ZSearchHistory::Other;

        pContext.Status = _parseContextShow(pContext);

        if (pContext.Status!=ZS_SUCCESS)
          return pContext.Status;
        pContext.HasInstruction=true;
        break;
      }
/*

  clear history ;       -> removes ALL instruction lines in the history. History is then empty.

  clear history first 5; -> removes the 5 FIRST instruction lines in the history or removes all the first lines until 3.

  clear history last 3;  -> removes the 3 LAST instruction lines in the history or removes all the last lines until 3.

  clear history at 3 ; -> removes the third instruction line in the history. If rank 3 does not exist in history, nothing is done.

*/
      case ZSRCH_CLEAR:
      {
        return _parseContextClear(pContext);
      } // ZSRCH_CLEAR


      case ZSRCH_FIND:
      {
        pContext.InstructionType = ZSITP_Find;
        pContext.Store = true;
        pContext.Status=_parseContextFind(pContext);
        if (pContext.Status!=ZS_SUCCESS)
          return pContext.Status;

        pContext.HasInstruction=true;
        break;
      } // ZSRCH_FIND

      case ZSRCH_FETCH:
      {
        pContext.InstructionType = ZSITP_Fetch;
        pContext.Store = true;
        pContext.Status=_parseFetch(pContext);
        if (pContext.Status!=ZS_SUCCESS)
          return pContext.Status;
        pContext.HasInstruction=true;
        break;
      } // ZSRCH_FETCH
      case ZSRCH_DISPLAY:
      {
        pContext.InstructionType = ZSITP_Display;
        pContext.Store = true;
        pContext.Status=_parseDisplay(pContext);
        if (pContext.Status!=ZS_SUCCESS)
          return pContext.Status;
        pContext.HasInstruction=true;
        break;
      } // ZSRCH_DISPLAY

      case ZSRCH_FOR:
        pContext.InstructionType = ZSITP_For ;
        pContext.Store=true;
        if (!pContext.advanceIndex())
          return pContext.Status=ZS_SYNTAX_ERROR;
        pContext.Status=_parseContextFor(pContext);
        if (pContext.Status!=ZS_SUCCESS)
          return pContext.Status;
        pContext.HasInstruction=true;
        break;

      case ZSRCH_FINISH:
        pContext.InstructionType = ZSITP_Finish ;
        pContext.HasInstruction=true;
        finish();
        break;

      default:
          ErrorLog.errorLog("Keyword <%s> has not been recognized as a valid first instruction at line %d column %d .",
                            pContext.CurrentToken->Text.toString(),
                            pContext.CurrentToken->TokenLine,
                            pContext.CurrentToken->TokenColumn );
          pContext.LastErroredToken = pContext.CurrentToken;
          return ZS_SYNTAX_ERROR;
      }// switch
    }//while

    if (pContext.isEOF()) {
      if (ZSearchTokenizer::_progressCallBack!=nullptr) {
        ZSearchTokenizer::_progressCallBack(pContext.CurrentTokenList.count());
      }
      return pContext.Status=ZS_SUCCESS;
    }

    if (ZSearchTokenizer::_progressCallBack!=nullptr) {
      ZSearchTokenizer::_progressCallBack(int(pContext.Index));
    }

 // }// main while

  if (!pContext.HasInstruction) {
    ErrorLog.warningLog("Sentence has no valid instruction." );
  }
  pContext.Phrase = pContext.MainPhrase->subString(pContext.TokenStart->TokenOffset,
                                                   pContext.CurrentToken->TokenOffset + 1 - pContext.TokenStart->TokenOffset);

  return pContext.Status ;
} // ZSearchParser::_parseContext

ZStatus
ZSearchParser::_parseSetDisplay(ZSearchContext &pContext)
{
    if (!pContext.advanceIndex())
        return pContext.Status=ZS_SYNTAX_ERROR;


    if (pContext.CurrentToken->Type==ZSRCH_PATH) {
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_OPERATOR_MOVE)) {
            ErrorLog.errorLog("Missing sign equal '=' at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_OPERATOR;
        }
        pContext.advanceIndex(false);
        if (pContext.isEOF() ||
                ((pContext.CurrentToken->Type != ZSRCH_STRING_LITERAL) &&
                (pContext.CurrentToken->Type != ZSRCH_URISTRING))) {
            ErrorLog.errorLog("Missing string literal or uriString description at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_LITERAL;
        }

        if (pContext.CurrentToken->Type == ZSRCH_STRING_LITERAL) {
                URIDisplay = pContext.CurrentToken->Text;
                if (URIDisplay.getDirectoryPath().isEmpty()) {
                    URIDisplay = GeneralParameters.getWorkDirectory().addConditionalDirectoryDelimiter() + URIDisplay;
                }

                ZStatus wSt=URIDisplay.check();
                if (wSt!=ZS_SUCCESS && wSt!=ZS_FILENOTEXIST && wSt!=ZS_FILENOTOPEN) {
                    ErrorLog.errorLog("<%s> is an invalid path at line %d column %d.",
                                      pContext.CurrentToken->Text.toString(),
                                      pContext.CurrentToken->TokenLine,
                                      pContext.CurrentToken->TokenColumn);
                    return ZS_INV_LITERAL;
                }
                ErrorLog.infoLog("Entity report display is redirected to file <%s>.",URIDisplay.toString());
                pContext.advanceIndex(false);
                return pContext.Status=ZS_SUCCESS;
        }//ZSRCH_STRING_LITERAL

        if (pContext.CurrentToken->Type == ZSRCH_URISTRING) {
            pContext.advanceIndex(false);
            if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_OPENPARENTHESIS)) {
                ErrorLog.errorLog("Missing open parenthesis at line %d column %d . Found <%s>.",
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn,
                                  pContext.CurrentToken->Text.toString() );
                pContext.LastErroredToken = pContext.CurrentToken;
                return pContext.Status=ZS_MISS_PUNCTSIGN;
            }
            pContext.advanceIndex(true);
            URIDisplay = pContext.CurrentToken->Text;
            if (URIDisplay.getDirectoryPath().isEmpty()) {
                URIDisplay = GeneralParameters.getWorkDirectory().addConditionalDirectoryDelimiter() + URIDisplay;
            }
            ZStatus wSt=URIDisplay.check();
            if (wSt!=ZS_SUCCESS) {
                ErrorLog.errorLog("<%s> is an invalid path at line %d column %d.",
                                  pContext.CurrentToken->Text.toString(),
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn);
                return ZS_INV_LITERAL;
            }
            pContext.advanceIndex(false);
            if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_CLOSEPARENTHESIS)) {
                ErrorLog.errorLog("Missing close parenthesis at line %d column %d . Found <%s>.",
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn,
                                  pContext.CurrentToken->Text.toString() );
                pContext.LastErroredToken = pContext.CurrentToken;
                return pContext.Status=ZS_MISS_PUNCTSIGN;
            }

            ErrorLog.infoLog("Entity content display is set to file <%s>.",URIDisplay.toString());
            pContext.advanceIndex(false);
            return pContext.Status=ZS_SUCCESS;
        }//ZSRCH_URISTRING

        if (pContext.CurrentToken->Type == ZSRCH_DEFAULT) {
            pContext.advanceIndex(false);
            ErrorLog.infoLog("Entity content display is redirected DEFAULT.");
            DisplayCurrent = false;
            URIDisplay.clear();

            pContext.advanceIndex(false);
            return pContext.Status=ZS_SUCCESS;
        }//ZSRCH_URISTRING
        ErrorLog.errorLog("Missing one of { <file path as literal> , uristring(<file uri>) } at line %d column %d. Found <%s>.",
                          pContext.CurrentToken->TokenLine,
                          pContext.CurrentToken->TokenColumn,
                          pContext.CurrentToken->Text.toString() );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_SYNTAX_ERROR;
    } // ZSRCH_PATH

    if (pContext.CurrentToken->Type==ZSRCH_DEFAULT) {
        pContext.advanceIndex(false);
        ErrorLog.infoLog("setDisplay-I_DEFAULT Entity display output is set to <DEFAULT>.\n");
        DisplayCurrent = false;
        return pContext.Status=ZS_SUCCESS;
    } // ZSRCH_DEFAULT

    if (pContext.CurrentToken->Type==ZSRCH_CURRENT) {
        pContext.advanceIndex(false);
        if (DisplayEntity==nullptr) {
            ErrorLog.errorLog("setDisplay-E-NOCURRENT Current entity display has not been yet defined.");
            ErrorLog.errorLog("                       Entity display is set to <DEFAULT> (Error log).");
        DisplayCurrent = false;
        return pContext.Status=ZS_NULLPTR;
        }
        ErrorLog.infoLog("setDisplay-I_CURRENT Entity display output is set to <CURRENT>.\n");
        DisplayCurrent = true;
        return pContext.Status=ZS_SUCCESS;
    } // ZSRCH_CURRENT

    if (pContext.CurrentToken->Text.compareCase("LIMIT")==0) {
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_OPERATOR_MOVE)) {
            ErrorLog.errorLog("Missing sign equal '=' at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_OPERATOR;
        }
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL)) {
            ErrorLog.errorLog("Missing numeric literal at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_LITERAL;
        }
        int wNewMax = pContext.CurrentToken->Text.toInt();
        setDisplayLimit(wNewMax);
        pContext.advanceIndex(false);
        return pContext.Status=ZS_SUCCESS;
    } // LIMIT

    if (pContext.CurrentToken->Text.compareCase("COLMAX")==0) {
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_OPERATOR_MOVE)) {
            ErrorLog.errorLog("Missing sign equal '=' at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_OPERATOR;
        }
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL)) {
            ErrorLog.errorLog("Missing numeric literal at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_LITERAL;
        }
        int wNewMax = pContext.CurrentToken->Text.toInt();
        setDisplayColMax(wNewMax);
        pContext.advanceIndex(false);
        return pContext.Status=ZS_SUCCESS;
    } // COLMAX
    if (pContext.CurrentToken->Text.compareCase("COLMIN")==0) {
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_OPERATOR_MOVE)) {
            ErrorLog.errorLog("Missing sign equal '=' at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_OPERATOR;
        }
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL)) {
            ErrorLog.errorLog("Missing numeric literal at line %d column %d . Found <%s>.",
                              pContext.CurrentToken->TokenLine,
                              pContext.CurrentToken->TokenColumn,
                              pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_MISS_LITERAL;
        }
        int wNewMax = pContext.CurrentToken->Text.toInt();
        setDisplayColMin(wNewMax);
        pContext.advanceIndex(false);
        return pContext.Status=ZS_SUCCESS;
    } // COLMIN

    if (pContext.CurrentToken->Text.compareCase("FORMAT")==0) {
        pContext.advanceIndex(false);

        ZCFMT_Type wZCellFormat = DisplayCellFormat;
        bool wEnd=false;
        bool wPlus=false;
        while (pContext.notEOF() && ! wEnd) {
            if (pContext.isEOF() ||
                    ((pContext.CurrentToken->Type != ZSRCH_OPERATOR_PLUS) &&
                     (pContext.CurrentToken->Type != ZSRCH_OPERATOR_MINUS))) {
                ErrorLog.errorLog("Missing operator : one of { '+' , '-' }  at line %d column %d . Found <%s>.",
                                  pContext.CurrentToken->TokenLine,
                                  pContext.CurrentToken->TokenColumn,
                                  pContext.CurrentToken->Text.toString() );
                pContext.LastErroredToken = pContext.CurrentToken;
                return pContext.Status=ZS_MISS_OPERATOR;
            }
            if (pContext.CurrentToken->Type == ZSRCH_OPERATOR_PLUS)
                wPlus=true;
            /* remark : if not plus, then minus has been mentionned */
            pContext.advanceIndex(false);
          while (true) {

          if (pContext.CurrentToken->Text.compareCase("NUMHEXA")==0) {
              wZCellFormat &= ~ ZCFMT_NumMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_NumHexa;
              else
                  wZCellFormat &= ~ZCFMT_NumHexa;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("MDY")==0) {
              wZCellFormat &= ~ ZCFMT_DateMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_MDY;
              else
                  wZCellFormat &= ~ZCFMT_MDY;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("DMY")==0) {
              wZCellFormat &= ~ ZCFMT_DateMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_DMY;
              else
                  wZCellFormat &= ~ZCFMT_DMY;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("MDYHMS")==0) {
              wZCellFormat &= ~ ZCFMT_DateMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_MDYHMS;
              else
                  wZCellFormat &= ~ZCFMT_MDYHMS;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("DMYHMS")==0) {
              wZCellFormat &= ~ ZCFMT_DateMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_DMYHMS;
              else
                  wZCellFormat &= ~ZCFMT_DMYHMS;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("DLOCALE")==0) {
              wZCellFormat &= ~ ZCFMT_DateMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_DLocale;
              else
                  wZCellFormat &= ~ZCFMT_DLocale;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("DUTC")==0) {
              wZCellFormat &= ~ ZCFMT_DateMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_DUTC;
              else
                  wZCellFormat &= ~ZCFMT_DUTC;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("RESSYMBOL")==0) {
              wZCellFormat &= ~ ZCFMT_ResMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_ResSymb;
              else
                  wZCellFormat &= ~ZCFMT_ResSymb;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("RESSTD")==0) {
              wZCellFormat &= ~ ZCFMT_ResMask ;
              if (wPlus)
                  wZCellFormat |= ZCFMT_ResStd;
              else
                  wZCellFormat &= ~ZCFMT_ResStd;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("PREFZTYPE")==0) {
              if (wPlus)
                  wZCellFormat |= ZCFMT_PrefZType;
              else
                  wZCellFormat &= ~ZCFMT_PrefZType;
              break;
          }
          if (pContext.CurrentToken->Text.compareCase("DUMPBLOB")==0) {
              if (wPlus)
                  wZCellFormat |= ZCFMT_DumpBlob;
              else
                  wZCellFormat &= ~ZCFMT_DumpBlob;
              break;
          }
          wEnd = true;
          break;
          }// while true
          if (!wEnd) {
              pContext.advanceIndex(false);
          }
        }// while not eof and not wEnd

        if (wZCellFormat == DisplayCellFormat) {
            ErrorLog.warningLog("Nothing changed for Cell format");
            return pContext.Status=ZS_SUCCESS;
        }
        DisplayCellFormat = wZCellFormat;
        ErrorLog.infoLog("Current display format has been changed to <%s>",decode_ZCellFormat(DisplayCellFormat).toString());

        return pContext.Status=ZS_SUCCESS;
    } // FORMAT

    ErrorLog.errorLog("Missing required word one of { <DEFAULT>,<CURRENT>,<PATH>,<LIMIT>,<COLMAX>,<COLMIN>,<FORMAT> } at line %d column %d . Found <%s>.",
                      pContext.CurrentToken->TokenLine,
                      pContext.CurrentToken->TokenColumn,
                      pContext.CurrentToken->Text.toString() );
    pContext.LastErroredToken = pContext.CurrentToken;
    return pContext.Status=ZS_MISS_KEYWORD;
} //  ZSearchParser::_parseSetDisplay


ZStatus
ZSearchParser::_parseContextClear(ZSearchContext & pContext)
{
    pContext.InstructionType = ZSITP_Clear;
    pContext.Store=false;
    if (!pContext.advanceIndex())
      return pContext.Status=ZS_SYNTAX_ERROR;

    if (pContext.CurrentToken->Type != ZSRCH_HISTORY) {
      ErrorLog.errorLog("Missing required word <HISTORY> at line %d column %d . Found <%s>.",
                        pContext.CurrentToken->TokenLine,
                        pContext.CurrentToken->TokenColumn,
                        pContext.CurrentToken->Text.toString() );
      pContext.LastErroredToken = pContext.CurrentToken;
      return pContext.Status=ZS_MISS_KEYWORD;
    }

    if (pContext.isEOF()) {
      clearHistory();
      return pContext.Status= ZS_SUCCESS ;
    }

    if (!pContext.advanceIndex())
      return pContext.Status=ZS_SYNTAX_ERROR;

    if (pContext.CurrentToken->Type == ZSRCH_FIRST) {
        if (pContext.isEOF()) {
          ErrorLog.errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                            pContext.CurrentToken->TokenLine,
                            pContext.CurrentToken->TokenColumn,
                            pContext.CurrentToken->Text.toString() );
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_MISS_LITERAL;
        }
      if (!pContext.advanceIndex(true))
        return ZS_SYNTAX_ERROR;
      if (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL) {
        ErrorLog.errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                          pContext.CurrentToken->TokenLine,
                          pContext.CurrentToken->TokenColumn,
                          pContext.CurrentToken->Text.toString() );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_MISS_LITERAL;
      }
      int wNb = pContext.CurrentToken->Text.toInt();

      pContext.Status = clearHistoryFirst(wNb);
      pContext.advanceIndex(false);
      if (pContext.Status==ZS_EOF)
          pContext.Status=ZS_SUCCESS;
      return pContext.Status ;
    } // ZSRCH_FIRST

    if (pContext.CurrentToken->Type == ZSRCH_LAST) {
      pContext.advanceIndex(false);
      if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL)) {
        ErrorLog.errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                          pContext.CurrentToken->TokenLine,
                          pContext.CurrentToken->TokenColumn,
                          pContext.CurrentToken->Text.toString() );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_MISS_LITERAL;
      }
      int wNb = pContext.CurrentToken->Text.toInt();
      pContext.Status=clearHistoryLast(wNb);
      pContext.advanceIndex(false);
      if (pContext.Status==ZS_EOF)
          pContext.Status=ZS_SUCCESS;
      return pContext.Status ;
    } // ZSRCH_LAST

    if (pContext.CurrentToken->Type == ZSRCH_AT) {
      if (!pContext.advanceIndex())
        return pContext.Status=ZS_SYNTAX_ERROR;
      if (pContext.CurrentToken->Type != ZSRCH_NUMERIC_LITERAL) {
        ErrorLog.errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                          pContext.CurrentToken->TokenLine,
                          pContext.CurrentToken->TokenColumn,
                          pContext.CurrentToken->Text.toString() );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_MISS_LITERAL;
      }
      int wNb = pContext.CurrentToken->Text.toInt();
      pContext.Status=clearHistoryRank(wNb);
      pContext.advanceIndex(false);
      if (pContext.Status==ZS_EOF)
          pContext.Status=ZS_SUCCESS;
      return pContext.Status ;
    } // ZSRCH_AT

  ErrorLog.errorLog("Missing required keyword one of {FIRST,LAST,AT} at line %d column %d . Found <%s>.",
                    pContext.CurrentToken->TokenLine,
                    pContext.CurrentToken->TokenColumn,
                    pContext.CurrentToken->Text.toString() );
  pContext.LastErroredToken = pContext.CurrentToken;
  return pContext.Status=ZS_MISS_KEYWORD;

} // _parseContextClear

/* collection is instantiated as soon as first entity name is known and validated to be a valid, stored entity */

ZStatus
ZSearchParser::_parseContextFind(ZSearchContext & pContext)
{
  ZStatus wSt=ZS_SUCCESS;
  pContext.InstructionType = ZSITP_Find;

  ZSearchLogicalTerm*   wLogicalTerm=nullptr;

  pContext.clearEntities(); /* reset current entities being used (defined within phrase) */

  if (!pContext.advanceIndex())
    return ZS_SYNTAX_ERROR;

  switch (pContext.CurrentToken->Type) {
    case ZSRCH_ALL:
      if (!pContext.advanceIndex())
        return ZS_SYNTAX_ERROR;
      break;
    case ZSRCH_FIRST:
      pContext.InstructionType |= ZSITP_First;  /* Nb ; Equivalence for ZSITP_All is 0x0 as defaulted value */
      if (!pContext.advanceIndex())
        return ZS_SYNTAX_ERROR;

      if (pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
          ErrorLog.errorLog("Missing numeric literal after clause FIRST. set to default value : 1 - at line %d column %d. Found <%s>.",
                     pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
          pContext.LastErroredToken = pContext.CurrentToken;
          return ZS_MISS_LITERAL;
      }

      pContext.Number = pContext.CurrentToken->Text.toInt(10);
      if (!pContext.advanceIndex())
          return ZS_SYNTAX_ERROR;
      break;
    default:
      ErrorLog.warningLog("Missing keyword one of [<ALL>,<FIRST>] set to default : ALL - at line %d column %d. Found <%s>.",
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
      pContext.InstructionType |= ZSITP_All;
      break;
    }// switch

    /* search for entity(ies) after keyword IN */

    int wFieldListBookMark = pContext.Index;  /* keep memory of fields selection clause beginning */

    while ((pContext.CurrentTokenList.count() > pContext.Index ) &&(pContext.CurrentToken->Type != ZSRCH_IN ))  /* search for beginning of entities */
    {
        if (!pContext.advanceIndex()) {
            ErrorLog.errorLog("Missing keyword <IN> in instruction.");
            return ZS_SYNTAX_ERROR;
        }
    }
    if (pContext.isEOF()) {
        ErrorLog.errorLog("Missing keyword <IN> in instruction.");
        return ZS_SYNTAX_ERROR;
    }
    /* here we are pointing on <IN> */
    if (!pContext.advanceIndex())
        return ZS_SYNTAX_ERROR;

    if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
        ErrorLog.errorLog("Expected active entity. Found <%s> line <%d> column <%d>",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken= pContext.CurrentToken;
        return ZS_INV_ENTITY;
    }

    while ((pContext.CurrentToken->Type != ZSRCH_WITH) && (pContext.CurrentToken->Type!=ZSRCH_AS)) {

        if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
            ErrorLog.errorLog("Expected entity identifier. Found <%s> at line %d column %d.",
                       pContext.CurrentToken->Text.toString(),
                       pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
            pContext.LastErroredToken=pContext.CurrentToken;
            return ZS_INV_ENTITY;
        }
        /* search for a valid open entity */
        std::shared_ptr<ZSearchEntity>          wEntity=nullptr;
        wEntity = EntityList.getEntityByName(pContext.CurrentToken->Text);
        if (wEntity==nullptr) {
            ErrorLog.errorLog("Entity identifier not found in declared entities list. Found <%s> at line %d column %d.",
              pContext.CurrentToken->Text.toString(),
              pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
            pContext.LastErroredToken=pContext.CurrentToken;
            return ZS_INV_ENTITY;
        }

        /* here active entity is set to pContext.CurrentToken->Text */

/* collection is instantiated as soon as first entity name is known and validated to be a valid, stored entity */
//        pContext.CurEntity=ZSearchEntity::constructWithZSearchEntity(wEntity,pContext.CurrentToken);

        pContext.SourceEntity = wEntity ;
        pContext.TargetEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(wEntity,pContext.CurrentToken));

        /* entity name will be allocated later after having parsed AS clause */

        if (!pContext.advanceIndex())
            return ZS_SYNTAX_ERROR;

        if (pContext.CurrentToken->Type == ZSRCH_COMMA) {
            if (!pContext.advanceIndex())
                return ZS_SYNTAX_ERROR;
            continue;
        }
    } //  while != ZSRCH_WITH && !=ZSRCH_AS

    if (pContext.SourceEntity==nullptr) {
        ErrorLog.errorLog("No valid entity has been mentionned. Found <%s> at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
        pContext.LastErroredToken=pContext.CurrentToken;
        return ZS_SYNTAX_ERROR;
    }



 //   int wWithBookMark = pContext.Index; // store pContext.Index pointing to first token after WITH Clause

    /* get back to parse selection */
//    setpContext.Index(wWithBookMark);  /* restore pContext.Index to first token after with clause */

    if (pContext.CurrentToken->Type == ZSRCH_WITH) {
        if (!pContext.advanceIndex(true))
          return ZS_SYNTAX_ERROR;

        wSt=_parseLogicalTerm(wLogicalTerm,pContext,0);
        if (wSt!=ZS_SUCCESS)
          return wSt;
    } // ZSRCH_WITH
    else {
        ErrorLog.warningLog("Missing keyword <WITH> : all ranks of entity will be selected." );
    }
    /* here token must be 'AS' */

    if (pContext.CurrentToken->Type!=ZSRCH_AS) {
      ErrorLog.errorLog("Syntax error : Missing clause <AS> while found <%s> at line %d column %d.",
          pContext.CurrentToken->Text.toString(),
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
        pContext.LastErroredToken = pContext.CurrentToken;
      if (wLogicalTerm!=nullptr)
        delete wLogicalTerm;
      return ZS_MISS_KEYWORD;
    }

    if (!pContext.advanceIndex()) {
      if (wLogicalTerm!=nullptr)
        delete wLogicalTerm;
      return ZS_SYNTAX_ERROR;
    }

    /* here entity name as collection name */

 //   ZSearchToken* wEntityNameToken= pContext.CurrentToken;

    if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER ) {
        ErrorLog.errorLog("Syntax error : Missing collection identifier while found <%s> at line %d column %d.",
                  pContext.CurrentToken->Text.toString(),
                  pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
        pContext.LastErroredToken = pContext.CurrentToken;
        if (wLogicalTerm!=nullptr)
            delete wLogicalTerm;
        return ZS_MISS_FIELD;
    }
/* at this stage, collection is already built */

    pContext.TargetEntity->setNameByToken(pContext.CurrentToken);


    pContext.TargetEntity->setLogicalTerm(wLogicalTerm);

    /* in the end get back to parse selected fields */

//    int wEndIndex = pContext.Index;
    pContext.setIndex(wFieldListBookMark); /* get back to field list clause : NB: we have entity(ies) field dictionary at disposal */

    wSt=_parseSelClause(pContext);
    if (wSt!=ZS_SUCCESS)
        return wSt;

//    pContext.setIndex(wEndIndex);
    pContext.popIndex();

    EntityList.push(pContext.TargetEntity);

    ErrorLog.infoLog("Collection <%s> has been created and registered as a new search entity.",
            pContext.TargetEntity->EntityFullName.toString()  );

    pContext.advanceIndex();
  return ZS_SUCCESS;
}// ZSearchParser::_parseFind


ZStatus
ZSearchParser::_parseContextDeclare(ZSearchContext& pContext)
{
    if (pContext.CurrentToken->Type!=ZSRCH_DECLARE)
        return pContext.Status=ZS_INVTYPE;

    pContext.InstructionType = ZSITP_Declare;
    pContext.Store=ZSearchHistory::DataExecute;

    if (!pContext.advanceIndex(true))
        return pContext.Status=ZS_SYNTAX_ERROR;

    if (pContext.CurrentToken->Type != ZSRCH_IDENTIFIER) {
        ErrorLog.errorLog("Expecting an identifier (entity or symbol name) while found <%s> at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_SYNTAX_ERROR;
    }

    pContext.TokenIdentifier = pContext.CurrentToken ; /* store new entity name or new symbol name as token identifier */

    if (!pContext.advanceIndex(true))
        return pContext.Status=ZS_SYNTAX_ERROR;

    /* if identifier then a symbol name is expected  :
         *   declare <symbol name> = <literal>  ;
         *
         *   declare gabu = ZDATE(30/01/24) ; -> creates symbol gabu with literal date value set to 30/01/24
         *   declare wResource = ZResource(ZDoc_Physical,1002) ;
        */
    if (pContext.CurrentToken->Type == ZSRCH_OPERATOR_MOVE) { /* then it is a symbol and its literal value follows */
        return _parseContextDeclareSymbol(pContext);
    }

    if (pContext.CurrentToken->Type ==ZSRCH_AS) {
        pContext.advanceIndex(false);

        switch (pContext.CurrentToken->Type)
        {
        case ZSRCH_FILE:
            pContext.advanceIndex(false);
            return _parseContextDeclareFile(pContext);

        case ZSRCH_IDENTIFIER:
        case ZSRCH_WILDCARD:
            return _parseContextDeclareEntity(pContext);
/*
        case ZSRCH_JOIN:
            return _parseContextDeclareJoin(pContext);
*/
        default:
        {
            ErrorLog.errorLog("Expecting one of { <FILE> or  <valid entity name> } while found <%s> at line %d column %d.",
                     pContext.CurrentToken->Text.toString(),pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
            pContext.LastErroredToken = pContext.CurrentToken;

            /* suggest keywords */
            ZArray<utf8VaryingString> wSugArray ={"FILE","JOIN" };
            /* add entities names to suggest */
            for (int wi=0;wi < EntityList.count();wi++)
                wSugArray.push(EntityList[wi]->EntityName);

            /* try to suggest */
            utf8VaryingString wSuggest=searchAdHocWeighted (pContext.CurrentToken->Text,wSugArray);
            if (!wSuggest.isEmpty())
                ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );

            return pContext.Status=ZS_SYNTAX_ERROR;
        } // default
        }// switch
    } // ZSRCH_AS

    ErrorLog.errorLog("Missing required word <FILE> , <new symbol name> or <new entity name> at line %d column %d . Found <%s>.",
             pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
    pContext.LastErroredToken = pContext.CurrentToken;
    utf8VaryingString wApprox = searchSymbolWeighted(pContext.CurrentToken->Text);
    if (!wApprox.isEmpty()) {
        ErrorLog.errorLog("Do you mean <%s> ?", wApprox.toString() );
    }
    return pContext.Status=ZS_MISS_KEYWORD;

} //ZSearchParser::_parseContextDeclare

/* declare <entity name> as file <path> [mode readonly,modify]  ;  // by default mode is read only */

ZStatus ZSearchParser::_parseContextDeclareFile(ZSearchContext &pContext)
{
//  pContext.clear();

  ZStatus wSt=ZS_SUCCESS;
  uriString wPath;
  pContext.InstructionType = ZSITP_Declare;

  while (pContext.notEOF()) {

    while (true) {
    if(pContext.CurrentToken->Type==ZSRCH_STRING_LITERAL)
      {
        wPath=pContext.CurrentToken->Text;
//        wPath.eliminateChar('"');
        wSt=wPath.check();
        if (wSt!=ZS_SUCCESS) {
          ErrorLog.logZStatus(ZAIES_Error,wSt,"Path <%s> does not point to a valid, existing file.");
          ErrorLog.errorLog("Invalid file path <%s> at line %d column %d.",
                            pContext.CurrentToken->TokenLine,
                            pContext.CurrentToken->TokenColumn,
                            pContext.CurrentToken->Text.toString() );
          pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status = ZS_FILEERROR ;
        }
        ErrorLog.infoLog("Path <%s> is valid at line %d column %d.",
                         pContext.CurrentToken->Text.toString(),
                         pContext.CurrentToken->TokenLine,
                         pContext.CurrentToken->TokenColumn );
        break;
      } // ZSRCH_STRING_LITERAL

      if(pContext.CurrentToken->Type==ZSRCH_IDENTIFIER)  /* pre-defined path  <symbol> <path> */
      {
          ZSearchSymbol wSymbol=SymbolList.getSymbol(pContext.CurrentToken->Text);
          if (wSymbol.isNull()) {
              ErrorLog.errorLog("Expected symbol : invalid symbol name <%s>  at line %d column %d.",
                                pContext.CurrentToken->Text.toString(),
                                pContext.CurrentToken->TokenLine,
                                pContext.CurrentToken->TokenColumn );
              pContext.LastErroredToken = pContext.CurrentToken;
              utf8VaryingString wApprox = searchSymbolWeighted(pContext.CurrentToken->Text);
              if (!wApprox.isEmpty()) {
                  ErrorLog.textLog("Do you mean <%s> ?", wApprox.toString() );
              }
              return pContext.Status = ZS_INVNAME ;
          }

          wPath = wSymbol.getURI();
          if (wPath.isEmpty()) {
              ErrorLog.errorLog("Symbol name <%s> is not a valid URI symbol.  at line %d column %d.",
                                pContext.CurrentToken->Text.toString(),
                                pContext.CurrentToken->TokenLine,
                                pContext.CurrentToken->TokenColumn );
              pContext.LastErroredToken = pContext.CurrentToken;
              return pContext.Status = ZS_INVTYPE ;
          }
          if (wPath.check()!=ZS_SUCCESS) {
//          if (wSt!=ZS_SUCCESS) {
              ErrorLog.errorLog("Invalid file path <%s> deduced from symbol <%s> at line %d column %d.",
                                wPath.toString(),
                                pContext.CurrentToken->Text.toString(),
                                pContext.CurrentToken->TokenLine,
                                pContext.CurrentToken->TokenColumn );
              pContext.LastErroredToken = pContext.CurrentToken;
              return pContext.Status = wSt ;
          }
      break;
      }// ZSRCH_IDENTIFIER

      ErrorLog.errorLog("Syntax error: Expected either path or symbol identifier. Found <%s> at line %d column %d.",
                        pContext.CurrentToken->Text.toString(),
                        pContext.CurrentToken->TokenLine,
                        pContext.CurrentToken->TokenColumn );
      pContext.LastErroredToken = pContext.CurrentToken;
      return ZS_SYNTAX_ERROR ;
      }// while true

    pContext.advanceIndex(false);
    if (pContext.isEOF() || (pContext.CurrentToken->Type!= ZSRCH_MODE)) {
        ErrorLog.warningLog("Expecting MODE keyword at line %d column %d. Found <%s>. Mode is set to READONLY.",
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
          pContext.InstructionType |= ZSITP_ReadOnly;
        break;
    }
    pContext.advanceIndex(false);
    if (pContext.isEOF() || ((pContext.CurrentToken->Type!= ZSRCH_READONLY)&&pContext.CurrentToken->Type!= ZSRCH_MODIFY)) {
        ErrorLog.errorLog("Expecting mode as one of [READONLY,MODIFY] at line %d column %d. Found <%s>.",
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status =ZS_INVPARAMS ;
    }

    if (pContext.CurrentToken->Type==ZSRCH_READONLY) {
            pContext.InstructionType |= ZSITP_ReadOnly;
    } else if (pContext.CurrentToken->Type==ZSRCH_MODIFY) {
            pContext.InstructionType |= ZSITP_Modify;
    }

    pContext.advanceIndex(false);
    break;
  }//wwhile (pContext.notEOF())



  pContext.Store = ZSearchHistory::DataExecute;

  std::shared_ptr <ZMasterFile> wMF=nullptr;
  ZSearchMasterFile* wZSRCHMF=nullptr;
  int wi=0;
  for (;wi < MasterFileList.count();wi++){
    if (wPath==MasterFileList[wi]->getURIContent()) {
      wMF = MasterFileList[wi] ;
        if (!wMF->isOpen()) {
            ErrorLog.errorLog("File <%s> is not open.", wPath.toString());
            return pContext.Status =ZS_FILENOTOPEN;
        }
      if (pContext.InstructionType & ZSITP_Modify) {
            if (wMF->getOpenMode()>=ZRF_Modify) {
            ErrorLog.errorLog("File <%s> is already is use with access mode <%s> and cannot use mode <ZRF_Modify>.",
                wPath.toString(), decode_ZRFMode(wMF->getOpenMode()));
            return pContext.Status =ZS_MODEINVALID;
            }
            break;
      }// ZSPA_Modify
      if (wMF->getOpenMode()>ZRF_Read_Only) {
        ErrorLog.errorLog("File <%s> is already is use with access mode <%s> cannot use mode <ZRF_Read_Only>.",
            wPath.toString(), decode_ZRFMode(wMF->getOpenMode()));

        return pContext.Status =ZS_MODEINVALID;
      }
      break;
    } // if (wPath==MasterFileList[wi]->MasterFile.getURIContent())
  } // for

  if (wi == MasterFileList.count())  {  /* not found in master file list : create one in list */
    wMF=std::shared_ptr<ZMasterFile>(new ZMasterFile);
    wZSRCHMF = new ZSearchMasterFile(wMF,pContext.TokenIdentifier->Text);
    if (pContext.InstructionType & ZSITP_Modify) {
        wSt=wZSRCHMF->openModify(wPath);
    }
    else {
        wSt=wZSRCHMF->openReadOnly(wPath);
    }
    if (wSt!=ZS_SUCCESS) {
        ErrorLog.logZExceptionLast("ZSearchParser::_parse");
        ErrorLog.logZStatus(ZAIES_Error,wSt,"ZSearchParser::_parse-E-ERROPEN Cannot access file <%s> mode <%s>.",
                   wPath.toString(),
                   pContext.InstructionType & ZSITP_Modify?"Modify":"Readonly");
      return pContext.Status =wSt;
    }
    ZMasterFileItem wZMFI(pContext.TokenIdentifier->Text,wMF);
    MasterFileList.push(wZMFI);
  }// not found


  /* search if an entity with same name already exists */

  if (EntityList.getEntityByName(pContext.TokenIdentifier->Text)!=nullptr) {
    ErrorLog.errorLog("An entity with name <%s> has already been registrated previously.",
        pContext.TokenIdentifier->Text.toString());
    return pContext.Status =ZS_INVNAME;
  }

  pContext.TargetEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(wZSRCHMF,pContext.TokenIdentifier->Text));

  pContext.TargetEntity->BuildDic.addMetaDic(wZSRCHMF->getDictionary());
  for (int wi=0; wi < wZSRCHMF->getDictionary()->count(); wi++) {
      pContext.TargetEntity->BuildDic.push(ZSearchField(pContext.TargetEntity,wZSRCHMF->getDictionary(),wi));
  }


  EntityList.push(pContext.TargetEntity);

  ErrorLog.textLog("File <%s> has been opened with mode <%s> as <%s> ",
      wPath.toString(),pContext.InstructionType & ZSITP_Modify?"Modify":"Readonly",pContext.TargetEntity->getEntityName().toString());


//  pContext.setMessage("Entity <%s> has been created and registered.", pContext.TargetEntity->getEntityName().toString());
  ErrorLog.textLog("Entity <%s> has been created and registered.", pContext.TargetEntity->getEntityName().toString());

  pContext.HasInstruction=true;

  return pContext.Status =ZS_SUCCESS;
} //_parseContextDeclareFile

ZStatus
ZSearchParser::_parseContextDeclareEntity(ZSearchContext & pContext)
{
  ZStatus wSt=ZS_SUCCESS;

  _parseDeclareEntity(pContext);
  if (pContext.Status!=ZS_SUCCESS)
      return pContext.Status;

//  pContext.advanceIndex(false);

  if (pContext.isEOF()||(pContext.CurrentToken->Text.compareCase( "JOIN") != 0)) {
      /* assign entity name as single entity name from stored token text */
      pContext.TargetEntity->setNameByToken(pContext.TokenIdentifier);
      pContext.InstructionType = ZSITP_SingleEntity;

      EntityList.push(pContext.TargetEntity);  /* register created single entity */

      ErrorLog.infoLog("Single entity <%s> <%s> has been created and registered as a new search entity.",
                       pContext.TargetEntity->EntityName.toString(),
                       pContext.TargetEntity->EntityFullName.toString()  );

      return pContext.Status;
  }

  /* here join keyword has been found :
   *    create join entity and feed with first single entity
   *    parse second entity
   *    store parsed entity
   *    parse using clause
   *    store all entities
   */
  if (!pContext.advanceIndex(true))
    return ZS_SYNTAX_ERROR;

  /* create join entity with name given by pContext.TokenIdentifier->Text */
  std::shared_ptr<ZSearchEntity> wJoinEntity = std::make_shared<ZSearchEntity>();

  std::shared_ptr<ZSearchEntity> wEntity = pContext.TargetEntity; /* store parsed first entity */
  /* Now we may assign name to join entity */
  wJoinEntity->setNameByToken(pContext.TokenIdentifier);

  /* assign name for first entity */
  utf8VaryingString wEName = pContext.TokenIdentifier->Text;
  wEName += "$";
  wEName += wEntity->_BaseEntity->getEntityName();
  wEntity->setName(wEName);

  //wJoinEntity->_JoinList.push(wEntity);

  /* parse second entity */

  wSt=_parseDeclareEntity(pContext);
  if (pContext.Status!=ZS_SUCCESS) {
      return pContext.Status;
  }

  /* check we have <using> clause */

  if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_USING))  {
      ErrorLog.errorLog("Missing keyword <USING> in instruction line. Found <%s> line <%d> column <%d>",
                        pContext.CurrentToken->Text.toString(),
                        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken= pContext.CurrentToken;
      return pContext.Status = ZS_MISS_KEYWORD;
  }

  wEName = pContext.TokenIdentifier->Text;
  wEName += "$";
  wEName += pContext.TargetEntity->_BaseEntity->getEntityName();
  pContext.TargetEntity->setName(wEName);

  wJoinEntity->_JoinList.push(wEntity);                  /*  first entity for join entity (join master) */
  wJoinEntity->_JoinList.push(pContext.TargetEntity);    /*  second entity for join entity (join slave) */

  pContext.TargetEntity = wJoinEntity; /* set join entity as new target */

  /* parse USING clause */

  pContext.Status = _parseUsingClause(pContext);
  if (pContext.Status!= ZS_SUCCESS) {
      return pContext.Status;
  }

  for (int wi=0; wi < pContext.TargetEntity->_JoinList[0]->BuildDic.count();wi++)
    pContext.TargetEntity->BuildDic.push(pContext.TargetEntity->_JoinList[0]->BuildDic[wi]);
  for (int wi=0; wi < pContext.TargetEntity->_JoinList[1]->BuildDic.count();wi++)
    pContext.TargetEntity->BuildDic.push(pContext.TargetEntity->_JoinList[1]->BuildDic[wi]);

  for (int wi=0; wi < pContext.TargetEntity->_JoinList[0]->LocalMetaDic.count();wi++)
    pContext.TargetEntity->LocalMetaDic.push(pContext.TargetEntity->_JoinList[0]->LocalMetaDic[wi]);
  for (int wi=0; wi < pContext.TargetEntity->_JoinList[1]->LocalMetaDic.count();wi++)
    pContext.TargetEntity->LocalMetaDic.push(pContext.TargetEntity->_JoinList[1]->LocalMetaDic[wi]);


  EntityList.push(pContext.TargetEntity->_JoinList[0]);
  ErrorLog.infoLog("Single entity <%s> has been created and registered as a new search entity.",
                   pContext.TargetEntity->_JoinList[0]->EntityFullName.toString()  );

  EntityList.push(pContext.TargetEntity->_JoinList[1]);
  ErrorLog.infoLog("Single entity <%s> has been created and registered as a new search entity.",
                   pContext.TargetEntity->_JoinList[1]->EntityFullName.toString()  );

  EntityList.push(pContext.TargetEntity);  /* register join entity */
  ErrorLog.infoLog("Join master entity <%s> has been created and registered as a new search entity.",
                   pContext.TargetEntity->EntityFullName.toString()  );

  pContext.advanceIndex(false);
  return pContext.Status;
}// ZSearchParser::_parseContextDeclareEntity

/* one entity definition is parsed :
 * identifies and creates ZSearchContext::SourceEntity
 * instantiates ZSearchContext::TargetEntity
 */

ZStatus
ZSearchParser::_parseDeclareEntity(ZSearchContext & pContext)
{
    ZStatus wSt=ZS_SUCCESS;
    pContext.InstructionType = ZSITP_Declare | ZSITP_Find;

    ZSearchLogicalTerm*   wLogicalTerm=nullptr;

    pContext.clearEntities(); /* reset source and target entities being used  */

    int wFieldListBookMark = pContext.Index;

      /* go to the end of entity definition to search for source entity name following keyword IN */

      while (pContext.notEOF()  && (pContext.CurrentToken->Type != ZSRCH_IN ))  /* search for beginning of entities list */
      {
          pContext.advanceIndex(false) ;
      }
      if (pContext.isEOF() ) {
          ErrorLog.errorLog("Missing keyword <IN> in instruction.");
          pContext.LastErroredToken = pContext.lastToken();
          return pContext.Status=ZS_SYNTAX_ERROR;
      }

      /* here we are pointing on <IN> */
      pContext.advanceIndex(false) ;

      if (pContext.isEOF() ||(pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER)) {
          ErrorLog.errorLog("Expected active entity identifier. Found <%s> line <%d> column <%d>",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
          pContext.LastErroredToken= pContext.CurrentToken;
          return pContext.Status=ZS_INV_ENTITY;
      }

      /* here we are pointing on source entity identifier */
      /* search for a valid open entity as source for current entity */

      pContext.SourceEntity = EntityList.getEntityByName(pContext.CurrentToken->Text);
      if (pContext.SourceEntity==nullptr) {
          ErrorLog.errorLog("Entity identifier <%s> has not been found in active entities list. at line %d column %d.",
            pContext.CurrentToken->Text.toString(),
            pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
          pContext.LastErroredToken=pContext.CurrentToken;
          return pContext.Status=ZS_INV_ENTITY;
      }

/* target entity is instantiated as soon as source entity name is known and validated to be a valid, registered entity */

/*     if (pContext.SourceEntity->isFile()) {
          ZSearchMasterFile* wMF=new ZSearchMasterFile(pContext.SourceEntity->_FileEntity->getMasterFile(),
                                                       pContext.SourceEntity->EntityName);
          pContext.TargetEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(pContext.SourceEntity->_FileEntity,
                                                                                   pContext.TokenIdentifier))
      }
      else
        pContext.TargetEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(pContext.SourceEntity,
                                                                                 pContext.TokenIdentifier));
*/
      pContext.TargetEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(pContext.SourceEntity,
                                                                               pContext.TokenIdentifier)) ;

      /* target entity name will be allocated later according the following rule :
       *  either it is a single entity -> pContext.TokenIdentifier->text
       *  or it is a member of a join clause :  pContext.TokenIdentifier->text'$'<SourceEntity name>
       */

      pContext.advanceIndex(false) ;

      if (pContext.notEOF() &&
              (pContext.CurrentToken->Type != ZSRCH_WITH) &&
              (pContext.CurrentToken->Type != ZSRCH_JOIN) &&
              (pContext.CurrentToken->Type != ZSRCH_USING)) {

          ErrorLog.errorLog("Expected one of { <WITH> , <USING> , <JOIN>. Found <%s> at line %d column %d.",
                   pContext.CurrentToken->Text.toString(),
                   pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
          pContext.LastErroredToken=pContext.CurrentToken;
          pContext.TargetEntity.reset();
          return pContext.Status=ZS_SYNTAX_ERROR;
      }

      /* get back to parse source entity ranks selection */

      if (pContext.CurrentToken->Type == ZSRCH_WITH) {
          if (!pContext.advanceIndex(true))
            return ZS_SYNTAX_ERROR;

          wSt=_parseLogicalTerm(wLogicalTerm,pContext,0);
          if (wSt!=ZS_SUCCESS)
            return wSt;

          pContext.TargetEntity->setLogicalTerm(wLogicalTerm);
      } // ZSRCH_WITH
      else {
          ErrorLog.warningLog("Missing keyword <WITH> : all ranks of entity will be selected." );
      }

      int wEndEntityIndex = pContext.Index; /* keep book mark of end of entity declaration */

      /* in the end get back to parse selected fields (must be from source entity) */

      pContext.setIndex(wFieldListBookMark); /* get back to field list clause : NB: we have entity(ies) field dictionary at disposal */

      pContext.Status =_parseSelClause(pContext);

      pContext.setIndex(wEndEntityIndex); /* get to end of entity declaration */

      if (pContext.Status!=ZS_SUCCESS) {
          pContext.TargetEntity.reset();
      }
    return pContext.Status;
}// ZSearchParser::_parseEntity

ZStatus
ZSearchParser::_parseUsingClause(ZSearchContext & pContext)
{
//  ZStatus wSt=ZS_SUCCESS;


  if ( pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_USING ) )  {
      ErrorLog.errorLog("Keyword <USING> was expected here. Found <%s> at line %d column %d.",
               pContext.CurrentToken->Text.toString(),
               pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
      pContext.LastErroredToken=pContext.CurrentToken;
      return pContext.Status=ZS_MISS_KEYWORD ;
  }

  if (!pContext.TargetEntity->isJoin()) {
      ErrorLog.errorLog("<USING> clause is reserved to joined entity. Entity <%s> is not a joined entity. at line %d column %d.",
                        pContext.TargetEntity->getEntityName().toString(),
                        pContext.CurrentToken->Text.toString(),
                        pContext.CurrentToken->TokenLine,
                        pContext.CurrentToken->TokenColumn  );
      pContext.LastErroredToken=pContext.CurrentToken;
      return pContext.Status = ZS_INV_OPERATOR ;
  }

  if (!pContext.advanceIndex(true))
    return pContext.Status=ZS_SYNTAX_ERROR;

  pContext.Status =_parseLogicalTerm(pContext.TargetEntity->_Using,pContext,0);

  return pContext.Status;
}// ZSearchParser::_parseUsingClause

/* if identifier then a symbol name is expected  :
         *   declare <symbol name> = <literal>  ;
         *
         *   declare gabu = ZDATE(30/01/24) ; -> creates symbol gabu with literal date value set to 30/01/24
         *   declare wResource = ZResource(ZDoc_Physical,1002) ;
        */
ZStatus
ZSearchParser::_parseContextDeclareSymbol(ZSearchContext& pContext)
{
    pContext.InstructionType = ZSITP_Declare | ZSITP_Symbol ;

    if (pContext.CurrentToken->Type !=ZSRCH_IDENTIFIER) {
        return pContext.Status = ZS_INVTYPE ;
    }
    /* avoid symbol name duplicates */

    if (SymbolList.exists(pContext.CurrentToken->Text)) {
        ErrorLog.errorLog("Symbol %s has already been defined.Cannot superseed at line %d column %d.",
                          pContext.CurrentToken->Text.toString(),
                          pContext.CurrentToken->TokenLine,
                          pContext.CurrentToken->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_INVNAME ;
    }

    ZSearchSymbol wSymbol;
    wSymbol.FullFieldName = pContext.CurrentToken->Text;

    pContext.advanceIndex(false);

    if (pContext.isEOF())  {
        SymbolList.push(wSymbol);
        ErrorLog.textLog("Symbol %s has been defined at line %d column %d.",
                         pContext.CurrentToken->Text.toString(),
                         pContext.CurrentToken->TokenLine,
                         pContext.CurrentToken->TokenColumn );
        return pContext.Status = ZS_SUCCESS ;
    }

    if (pContext.CurrentToken->Type !=ZSRCH_OPERATOR_MOVE) {
        ErrorLog.errorLog("Expected sign '=' while found token <%s> at line %d column %d.",
                          pContext.CurrentToken->Text.toString(),
                          pContext.CurrentToken->TokenLine,
                          pContext.CurrentToken->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;
        return pContext.Status=ZS_MISS_OPERATOR ;
    }// ZSRCH_OPERATOR_MOVE

    if (!pContext.advanceIndex(false))
        return pContext.Status=ZS_SYNTAX_ERROR;

    ZSearchOperandType_type wZSTO=ZSTO_Nothing;
    ZSearchLiteral *wLit=nullptr;
    pContext.Status=_parseLiteral(wLit,pContext,wZSTO);
    if (pContext.Status!=ZS_SUCCESS)
        return pContext.Status;
    wSymbol.setLiteral(wLit);
    wSymbol.ZSTO = wZSTO | ZSTO_Symbol;
    SymbolList.push(wSymbol);
    ErrorLog.textLog("Symbol %s has been defined and its value has been set at line %d column %d.",
                     wSymbol.FullFieldName.toString(),
                     pContext.CurrentToken->TokenLine,
                     pContext.CurrentToken->TokenColumn );

    return pContext.Status = ZS_SUCCESS ;

}// ZSearchParser::_parseContextDeclareSymbol

/*
 * fetch <entity name> {    [first [<number of ranks to display>] ],
 *                          [next [<number of ranks to display>] ] ,
 *                          [all] } [;]
 *
*/
ZStatus
ZSearchParser::_parseFetch(ZSearchContext & pContext)
{
  ZStatus wSt=ZS_SUCCESS;
  pContext.InstructionType = ZSITP_Fetch;

//  ZSearchLogicalTerm*   wLogicalTerm=nullptr;

  pContext.clearEntities(); /* reset current entities being used (defined within phrase) */

  if (!pContext.advanceIndex())
    return ZS_SYNTAX_ERROR;

  if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
      ErrorLog.errorLog("Expected entity identifier. Found <%s> at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),
                 pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
      pContext.LastErroredToken=pContext.CurrentToken;
      return ZS_INV_ENTITY;
  }
  /* search for a valid open entity */
  std::shared_ptr<ZSearchEntity>          wEntity=nullptr;
  wEntity = EntityList.getEntityByName(pContext.CurrentToken->Text);
  if (wEntity==nullptr) {
      ErrorLog.errorLog("Entity identifier not found in declared entities list. Found <%s> at line %d column %d.",
        pContext.CurrentToken->Text.toString(),
        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
      pContext.LastErroredToken=pContext.CurrentToken;
      return ZS_INV_ENTITY;
  }

  /* here active entity is set to pContext.CurrentToken->Text */

/* collection is instantiated as soon as first entity name is known and validated to be a valid, stored entity */
//        pContext.CurEntity=ZSearchEntity::constructWithZSearchEntity(wEntity,pContext.CurrentToken);

  pContext.SourceEntity = wEntity ;
//  pContext.TargetEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(wEntity,pContext.CurrentToken));

  if (!pContext.advanceIndex())
    return ZS_SYNTAX_ERROR;

  switch (pContext.CurrentToken->Type) {
    case ZSRCH_ALL:
      pContext.InstructionType |= ZSITP_All;
      /* execute fetch all */
      break;
    case ZSRCH_FIRST:
    {
      pContext.Number=1;
      pContext.InstructionType |= ZSITP_First;  /* Nb ; Equivalence for ZSITP_All is 0x0 as defaulted value */
      pContext.advanceIndex();
      if (pContext.notEOF()) {
      if (pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
          ErrorLog.errorLog("Expecting numeric literal after clause FIRST. at line %d column %d. Found <%s>.",
                     pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
          pContext.LastErroredToken = pContext.CurrentToken;
          return ZS_MISS_LITERAL;
      }
      pContext.Number = pContext.CurrentToken->Text.toInt(10);
      }// not EOF

      /* execute fetch first xx */

      break;
    } // ZSRCH_FIRST

  case ZSRCH_NEXT:
  {
    pContext.Number=1;
    pContext.InstructionType |= ZSITP_Next;  /* Nb ; Equivalence for ZSITP_All is 0x0 as defaulted value */
    pContext.advanceIndex();
    if (!pContext.isEOF()) {
        if (pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
            ErrorLog.errorLog("Expecting numeric literal after clause NEXT at line %d column %d. Found <%s>.",
                       pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
            pContext.LastErroredToken = pContext.CurrentToken;
            return ZS_MISS_LITERAL;
        }
        pContext.Number = pContext.CurrentToken->Text.toInt(10);
    }
    } //ZSRCH_NEXT

    /* execute fetch next pContext.NumberToFind */
    break;
    default:
      ErrorLog.warningLog("Missing keyword one of { ALL , FIRST , NEXT }  at line %d column %d. Found <%s>.",
          pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
      pContext.InstructionType |= ZSITP_All;
      break;
    }// switch
    pContext.advanceIndex();
  return executeFetch(pContext);
}// ZSearchParser::_parseFetch

ZStatus
ZSearchParser::execute(ZSearchContext & pContext)
{
    if (!pContext.InstructionType & ZSITP_ToBeExecuted)   /* display parameters, parameters change etc. */
        return ZS_SUCCESS;

    /* search for entity context, and create one if not found */
    pContext.SEC = nullptr;
    for (int wi=0;wi < SECList.count(); wi++) {
        if (SECList[wi]->Entity == pContext.SourceEntity) {
            pContext.SEC = SECList[wi];
            break;
        }
    }
    if (pContext.SEC==nullptr) {
        pContext.SEC = ZSearchEntityContext::newEntityContext (pContext.SourceEntity);
        SECList.push(pContext.SEC);
    }


    if (pContext.InstructionType & ZSITP_Fetch)
        return executeFetch(pContext);
    if (pContext.InstructionType & ZSITP_Display)
        return executeDisplay(pContext);
    return ZS_SUCCESS;
}

ZStatus
ZSearchParser::executeFetch(ZSearchContext & pContext)
{
    ZStatus wSt=ZS_SUCCESS;

    if (pContext.InstructionType & ZSITP_Next) {
        return pContext.SourceEntity->populateNext(*pContext.SEC,pContext.Number);
    }
    if (pContext.InstructionType & ZSITP_First) {
        return pContext.SourceEntity->populateFirst(*pContext.SEC,pContext.Number);
    }
    if (pContext.InstructionType & ZSITP_All) {
        return pContext.SourceEntity->populateAll(*pContext.SEC);
    }
    return wSt;
}
/*
 * display <entity name> {  [first [<number of ranks to display>] ],
 *                          [next [<number of ranks to display>] ] ,
 *                          [at <rank>] ,
 *                          [all] } [;]
 *
*/
ZStatus
ZSearchParser::_parseDisplay(ZSearchContext & pContext)
{
  ZStatus wSt=ZS_SUCCESS;
  pContext.InstructionType = ZSITP_Display;

  pContext.clearEntities(); /* reset current entities being used (defined within phrase) */

  if (!pContext.advanceIndex())
    return pContext.Status=ZS_SYNTAX_ERROR;

  if (pContext.CurrentToken->Type!=ZSRCH_IDENTIFIER) {
      ErrorLog.errorLog("Expected active entity. Found <%s> line <%d> column <%d>",
               pContext.CurrentToken->Text.toString(),
               pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
      pContext.LastErroredToken= pContext.CurrentToken;
      return pContext.Status=ZS_INV_ENTITY;
  }
  /* search for a valid open entity */
  std::shared_ptr<ZSearchEntity>          wEntity=nullptr;
  wEntity = EntityList.getEntityByName(pContext.CurrentToken->Text);
  if (wEntity==nullptr) {
      ErrorLog.errorLog("Entity identifier not found in declared entities list. Found <%s> at line %d column %d.",
        pContext.CurrentToken->Text.toString(),
        pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn  );
      pContext.LastErroredToken=pContext.CurrentToken;
      return pContext.Status=ZS_INV_ENTITY;
  }
  pContext.SourceEntity = wEntity;

  pContext.advanceIndex(false);

  if (pContext.isEOF()) {
      pContext.InstructionType |= ZSITP_All;
  }
  else {
      switch (pContext.CurrentToken->Type) {
        case ZSRCH_ALL:
          pContext.InstructionType |= ZSITP_All;
          pContext.advanceIndex(false);
          break;
        case ZSRCH_FIRST:
          pContext.InstructionType |= ZSITP_First;
          pContext.Number = 1;
          pContext.advanceIndex();
          if (pContext.notEOF()) {
              if (pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
                  ErrorLog.errorLog("Missing numeric literal after clause FIRST at line %d column %d. Found <%s>.",
                             pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
                  pContext.LastErroredToken = pContext.CurrentToken;
                  return pContext.Status=ZS_MISS_LITERAL;
              }
              pContext.Number = pContext.CurrentToken->Text.toInt(10);
              pContext.advanceIndex(false);
          }
          break;
      case ZSRCH_NEXT:
        pContext.InstructionType |= ZSITP_Next;
        pContext.Number = 1;
        pContext.advanceIndex(false);
        if (pContext.notEOF()) {
            if (pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
                ErrorLog.errorLog("Missing numeric literal after clause NEXT at line %d column %d. Found <%s>.",
                           pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
                pContext.LastErroredToken = pContext.CurrentToken;
                return pContext.Status=ZS_MISS_LITERAL;
            }
            pContext.Number = pContext.CurrentToken->Text.toInt(10);
            pContext.advanceIndex(false);
        }
        break;
      case ZSRCH_AT:
        pContext.InstructionType |= ZSITP_At;
        pContext.Number = 1;
        pContext.advanceIndex(false);
        if (pContext.notEOF()) {
            if (pContext.CurrentToken->Type!=ZSRCH_NUMERIC_LITERAL) {
                ErrorLog.errorLog("Missing numeric literal after clause AT at line %d column %d. Found <%s>.",
                           pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
                pContext.LastErroredToken = pContext.CurrentToken;
                return pContext.Status=ZS_MISS_LITERAL;
            }
            pContext.Number = pContext.CurrentToken->Text.toInt(10);
            pContext.advanceIndex(false);
        }
        break;
        default:
          ErrorLog.warningLog("Missing keyword one of [<ALL>,<FIRST>]  at line %d column %d. Found <%s>.",
              pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString() );
          return pContext.Status=ZS_MISS_KEYWORD ;
          break;
        }// switch
    }


  return pContext.Status=ZS_SUCCESS;
}// ZSearchParser::_parseDisplay


ZStatus
ZSearchParser::executeDisplay(ZSearchContext& pContext)
{
    ZStatus wSt = ZS_SUCCESS;

//    return DisplayEntity(pContext.SourceEntity,pContext.InstructionType,pContext.Number);
    if (DisplayCurrent && (DisplayEntity != nullptr))
        return DisplayEntity(pContext);
    return DisplayEntityDefault(pContext);
/*    if (pContext.InstructionType & ZSITP_Next) {
        return pContext.SourceEntity->populateNext(pContext.Number);
    }
    if (pContext.InstructionType & ZSITP_First) {
        return pContext.SourceEntity->populateFirst(pContext.Number);
    }
    if (pContext.InstructionType & ZSITP_All) {
        wSt = pContext.SourceEntity->populateAll();
        if (wSt!=ZS_SUCCESS)
            return wSt;
        return DisplayEntity(pContext.SourceEntity);
    }
    */
}

ZStatus ZSearchParser::DisplayEntityDefault (ZSearchContext &pContext)
{
    if (pContext.SourceEntity==nullptr) {
        ErrorLog.errorLog("DisplayEntityDefault-E-NULLPTR No entity defined ");
        return ZS_NULLPTR ;
    }
    if (pContext.SourceEntity->isJoin())
        return _DisplayEntityJoinDefault(pContext);
    return _DisplayEntitySingleDefault (pContext);
}


void ZSearchParser::_DESD_Init ()
{
    if (URIDisplay.isEmpty())
        return;

    ZStatus wSt = URIDisplay.createFile();
    if (wSt!=ZS_SUCCESS) {
        ErrorLog.errorLog("ZSearchParser::_DESD_Init-E-CREERR Cannot create / replace display output file %s",
                          URIDisplay.toString() );
        ErrorLog.errorLog("                 Status is %s",
                          decode_ZStatus(wSt));
        ErrorLog.errorLog("                 Entity display output is redicted to DEFAULT.");
        URIDisplay.clear();
        if (DisplayCurrent)
            DisplayCurrent = false;
    }
} // ZSearchParser::_DESD_Init

void ZSearchParser::_DESD_Display (const utf8VaryingString& pFormat,...)
{
    utf8VaryingString wStr;
    va_list arglist;
    va_start (arglist, pFormat);
        wStr.vsnprintf(cst_messagelen,pFormat.toCChar(), arglist);
//        wStr.addUtfUnit( '\n' );
    va_end(arglist);

    if (!URIDisplay.isEmpty()) {
        ZStatus wSt = URIDisplay << wStr;
        if (wSt!=ZS_SUCCESS) {
            ErrorLog.errorLog("ZSearchParser::_DESD_Display-E-WRITERR Cannot write to display output file %s",
                              URIDisplay.toString() );
            ErrorLog.errorLog("                 Status is %s",
                              decode_ZStatus(wSt));
            ErrorLog.errorLog("                 Entity display output is redicted to DEFAULT.");
            URIDisplay.clear();
            if (DisplayCurrent)
                DisplayCurrent = false;

            ErrorLog.textLog(wStr.toCChar());
        }
        return ;
    }
    ErrorLog.textLog(wStr.toCChar());
} // ZSearchParser::_DESD_Display

ZStatus ZSearchParser::_DisplayEntitySingleDefault (ZSearchContext &pContext)
{
    utf8VaryingString wStr;

    ZStatus wSt=ZS_SUCCESS;

    ZArray<utf8VaryingString>            wHeader;
    ZArray<utf8VaryingString>*           wRow=nullptr;
    ZArray<ZArray<utf8VaryingString>*>   wRowList;

//    URFParser _URFParser;

    if (pContext.SourceEntity==nullptr) {
        ErrorLog.errorLog("_DisplayEntitySingleDefault-E-NULLPTR No entity defined ");
        return ZS_NULLPTR ;
    }
    if (pContext.SourceEntity->isJoin()) {
        ErrorLog.errorLog("_DisplayEntitySingleDefault-E-INVTYPE Entity is a join and is not allowed here.\n");
        return ZS_INVTYPE;
    }


    zaddress_type wAddress;

    pContext.SEC->CaptureTime=true;
    pContext.SEC->ProcessTi.init();


    if ( (pContext.InstructionType & ZSITP_First) || (pContext.InstructionType & ZSITP_All)) {
            wSt=pContext.SourceEntity->getFirst(*pContext.SEC,wAddress);
    }
    else if (pContext.InstructionType & ZSITP_Next) {
            wSt=pContext.SourceEntity->getNext(*pContext.SEC,wAddress);
    }
    else if (pContext.InstructionType & ZSITP_At) {
        zaddress_type wAddress;
        wSt=pContext.SourceEntity->getByRank(*pContext.SEC,pContext.Number,wAddress);
        pContext.Number=1;
       /* to be done */
    }
        else {
            ErrorLog.errorLog("Invalid instruction type given. Expected one of {ZSITP_First,ZSITP_All,ZSITP_Next,ZSITP_At}");
            return ZS_INVOP;
        }

    _DESD_Init();

    int wCount = 0;

    while ((wSt == ZS_SUCCESS) && ((pContext.InstructionType & ZSITP_All) || (wCount < pContext.Number) )) {

        if ((DisplayLimit>0)&&(wCount > DisplayLimit)) {
            if (!URIDisplay.isEmpty())
                _DESD_Display("ZSearchParser::_DisplayEntitySingleDefault-W-LIMIT Maximum number of ranks allowed reached. Limit %d",
                              DisplayLimit);
             ErrorLog.warningLog("ZSearchParser::_DisplayEntitySingleDefault-W-LIMIT Maximum number of ranks allowed reached. Limit %d",
                                 DisplayLimit);
             break ;
        }
        wRow = new ZArray<utf8VaryingString>;
        for (int wi=0; wi < pContext.SEC->_URFParser.URFFieldList.count();wi++) {
//            wRow->push(_URFParser.URFFieldList[wi].displayFmt(DisplayCellFormat));
            wRow->push(pContext.SEC->_URFParser.URFFieldList[wi].displayFmt(DisplayCellFormat));
        } // for
        wRowList.push(wRow);
        wCount++;
        if ((pContext.InstructionType & ZSITP_All) || (wCount < pContext.Number) ) {
                wSt=pContext.SourceEntity->getNext(*pContext.SEC,wAddress);
        }
    } // while

    if (wSt==ZS_OUTBOUNDHIGH)
        wSt=ZS_SUCCESS;

    if (pContext.SEC->CaptureTime) {
        ErrorLog.infoLog("Performance report : Entity ranks obtained %d processing time %s",
                         wCount,pContext.SEC->ProcessTi.reportDeltaTime().toString());
    }


    const char* wAction="Unknown action";
    if (pContext.InstructionType & ZSITP_First)
        wAction = "Display first";
    else if (pContext.InstructionType & ZSITP_Next)
        wAction = "Display next";
    else if (pContext.InstructionType & ZSITP_All)
        wAction = "Display all";
    else if (pContext.InstructionType & ZSITP_At)
        wAction = "Display at";

    _DESD_Display("\n Entity %s %s  %s %d ",
                     pContext.SourceEntity->getEntityName().toString(),
                     entityType(pContext.SourceEntity), wAction , pContext.Number);



    if (wRowList.count()==0) {
        _DESD_Display("No entity rank are available for display,");
//        return;
    }

    /* compute columns size for detail rows */

    ZArray<int> wColSizeList;

    for (int wi = 0 ; wi < pContext.SourceEntity->LocalMetaDic.count() ; wi++)
        wColSizeList.push(0);


    for (int wi=0; wi < wRowList.count(); wi++) {
        for (int wj=0; wj < wRowList[wi]->count(); wj++) {
            int wLen = wRowList[wi]->Tab(wj).strlen();

            if (wLen > DisplayColMax) {
                wRowList[wi]->Tab(wj).truncate(ssize_t(DisplayColMax));
                wRowList[wi]->Tab(wj) += cst_OverflowChar8 ;
                wLen = DisplayColMax;
            }// if

            if ( wLen > wColSizeList[wj] )
                wColSizeList[wj] = wLen;

            if (wColSizeList[wj] < DisplayColMin)
                wColSizeList[wj] = DisplayColMin ;
        } // for
//        wColSizeList.push(wColSize);
    }// for


    /* populate header from build dictionary */

    for (int wi=0; wi < pContext.SourceEntity->LocalMetaDic.count(); wi++)
        wHeader.push(pContext.SourceEntity->LocalMetaDic[wi].getName());

    /* adjust header column size to field sizes */

//    int wTLen = 1;

    for (int wi = 0; wi  < wHeader.count(); wi++ ) {
        size_t wL1,wL ;
        wL=wL1=wHeader[wi].strlen();
        if (wL1 > wColSizeList[wi]) {
            if (wL1 > DisplayColMax) {
                wL1=DisplayColMax;
            }
            wColSizeList[wi]=wL1;
            if (wL > wColSizeList[wi]) {
                wHeader[wi].truncate(ssize_t(wColSizeList[wi]));
                wHeader[wi] += cst_OverflowChar8 ;
            }
        }
//        wTLen += wColSizeList[wi];
//        wTLen ++;
    } // for

    /* display header */

    utf8VaryingString wBar = "+";
//    wBar.setChar('_',0,ssize_t(wTLen));

    utf8VaryingString wHContent = "|";
    for (int wi = 0; wi  < wHeader.count(); wi++ ) {
        /* pad with leading space whenever necessary */
        ssize_t wPad = ssize_t(wColSizeList[wi] - wHeader[wi].strlen());
        if (wPad > 0)
            wHContent.addChar(' ',wPad);
        wHContent.addsprintf("%s|",wHeader[wi].toString());
        wBar.addChar('_',wColSizeList[wi]);
        wBar.addUtfUnit('+');
    } // for

    _DESD_Display(wBar.toCChar());
    _DESD_Display(wHContent.toCChar());
    _DESD_Display(wBar.toCChar());

    /* display detail rows */

    for (int wi = 0; wi  < wRowList.count(); wi++ ) {
        wHContent = "|" ;
        for (int wj = 0; wj  < wRowList[wi]->count(); wj++ ) {
            wHContent.addsprintf("%s|",wRowList[wi]->Tab(wj).leftPad(wColSizeList[wj],' ').toString());
        } // for
        _DESD_Display(wHContent.toCChar());
    }// for

    _DESD_Display(wBar.toCChar());

    for (long wi=0 ; wi < wRowList.count() ; wi++) {
        delete wRowList[wi];
    }

    return wSt;
} // ZSearchParser::DisplayEntityDefault

ZStatus ZSearchParser::_DisplayEntityJoinDefault (ZSearchContext &pContext)
{
    if (pContext.SourceEntity==nullptr) {
        ErrorLog.errorLog("_DisplayEntityJoinDefault-E-NULLPTR No entity defined ");
        return ZS_NULLPTR ;
    }

    if (!pContext.SourceEntity->isJoin()) {
        _DBGPRINT("_DisplayEntityJoinDefault-E-INVTYPE Entity <%s> is not a join.\n")
        return ZS_INVTYPE;
    }

    ZStatus wSt=ZS_SUCCESS;

    ZArray<utf8VaryingString>            wHeader;
    ZArray<utf8VaryingString>*           wRow=nullptr;
    ZArray<ZArray<utf8VaryingString>*>   wRowList;

//    URFParser _URFParser;

    ZSearchJoinAddress wJAddress;

    pContext.SEC->CaptureTime=true;
    pContext.SEC->ProcessTi.init();


    if ( (pContext.InstructionType & ZSITP_First) || (pContext.InstructionType & ZSITP_All)) {
            wSt=pContext.SourceEntity->getFirstJoin(*pContext.SEC,wJAddress);
    }
    else if (pContext.InstructionType & ZSITP_Next) {
            wSt=pContext.SourceEntity->getNextJoin(*pContext.SEC,wJAddress);
    }
    else if (pContext.InstructionType & ZSITP_At) {
            wSt=pContext.SourceEntity->_getByRankJoin(*pContext.SEC,pContext.Number,wJAddress);
       pContext.Number=1;
       /* to be done */
    }
        else {
            ErrorLog.errorLog("_DisplayEntityJoinDefault-E-INVINSTR Invalid instruction type given. Expected one of {ZSITP_First,ZSITP_All,ZSITP_Next,ZSITP_At}");
            return ZS_INVOP;
        }
    int wCount = 0;

    while ((wSt == ZS_SUCCESS) && ((pContext.InstructionType & ZSITP_All) || (wCount < pContext.Number) )) {
        wRow = new ZArray<utf8VaryingString>;
        for (int wi=0; wi < pContext.SEC->_URFParser.URFFieldList.count();wi++) {
            wRow->push(pContext.SEC->_URFParser.URFFieldList[wi].displayFmt(DisplayCellFormat));
        } // for
        wRowList.push(wRow);
        wCount++;
        if ((pContext.InstructionType & ZSITP_All) || (wCount < pContext.Number) ) {
                wSt=pContext.SourceEntity->getNextJoin(*pContext.SEC,wJAddress);
        }
    } // while

    if (wSt==ZS_OUTBOUNDHIGH)
        wSt=ZS_SUCCESS;

    if (pContext.SEC->CaptureTime) {
        ErrorLog.infoLog("Performance report : Entity ranks obtained %d processing time %s",
                         wCount,pContext.SEC->ProcessTi.reportDeltaTime().toString());
    }


    _DESD_Init();

    const char* wAction="Unknown action";
    if (pContext.InstructionType & ZSITP_First)
        wAction = "Display first";
    else if (pContext.InstructionType & ZSITP_Next)
        wAction = "Display next";
    else if (pContext.InstructionType & ZSITP_All)
        wAction = "Display all";
    else if (pContext.InstructionType & ZSITP_At)
        wAction = "Display at";

    _DESD_Display("\n Entity %s %s  %s %d ",
                     pContext.SourceEntity->getEntityName().toString(),
                     entityType(pContext.SourceEntity), wAction , pContext.Number);

    if (wRowList.count()==0) {
        _DESD_Display("No entity rank have been selected for display,");
//        return;
    }

    /* compute columns size for detail rows */

    ZArray<int> wColSizeList;

    for (int wi = 0 ; wi < pContext.SourceEntity->LocalMetaDic.count() ; wi++)
        wColSizeList.push(0);


    for (int wi=0; wi < wRowList.count(); wi++) {
        for (int wj=0; wj < wRowList[wi]->count(); wj++) {
            int wLen = wRowList[wi]->Tab(wj).strlen();

            if (wLen > DisplayColMax) {
                wRowList[wi]->Tab(wj).truncate(ssize_t(DisplayColMax));
                wRowList[wi]->Tab(wj) += cst_OverflowChar8 ;
                wLen = DisplayColMax;
            }// if

            if ( wLen > wColSizeList[wj] )
                wColSizeList[wj] = wLen;

            if (wColSizeList[wj] < DisplayColMin)
                wColSizeList[wj] = DisplayColMin ;
        } // for
//        wColSizeList.push(wColSize);
    }// for


    /* populate header from build dictionary */

    for (int wi=0; wi < pContext.SourceEntity->LocalMetaDic.count(); wi++)
        wHeader.push(pContext.SourceEntity->LocalMetaDic[wi].getName());

    /* adjust header column size to field sizes */

//    int wTLen = 1;

    for (int wi = 0; wi  < wHeader.count(); wi++ ) {
        size_t wL1,wL ;
        wL=wL1=wHeader[wi].strlen();
        if (wL1 > wColSizeList[wi]) {
            if (wL1 > DisplayColMax) {
                wL1=DisplayColMax;
            }
            wColSizeList[wi]=wL1;
            if (wL > wColSizeList[wi]) {
                wHeader[wi].truncate(ssize_t(wColSizeList[wi]));
                wHeader[wi] += cst_OverflowChar8 ;
            }
        }
//        wTLen += wColSizeList[wi];
//        wTLen ++;
    } // for

    /* display header */

    utf8VaryingString wBar = "+";
//    wBar.setChar('_',0,ssize_t(wTLen));

    utf8VaryingString wHContent = "|";
    for (int wi = 0; wi  < wHeader.count(); wi++ ) {
        /* pad with leading space whenever necessary */
        ssize_t wPad = ssize_t(wColSizeList[wi] - wHeader[wi].strlen());
        if (wPad > 0)
            wHContent.addChar(' ',wPad);
        wHContent.addsprintf("%s|",wHeader[wi].toString());
        wBar.addChar('_',wColSizeList[wi]);
        wBar.addUtfUnit('+');
    } // for

    _DESD_Display(wBar.toCChar());
    _DESD_Display(wHContent.toCChar());
    _DESD_Display(wBar.toCChar());

    /* display detail rows */

    for (int wi = 0; wi  < wRowList.count(); wi++ ) {
        wHContent = "|" ;
        for (int wj = 0; wj  < wRowList[wi]->count(); wj++ ) {
//            wHContent.addsprintf("%s|",leftPad(wRowList[wi]->Tab(wj),wColSizeList[wj],' ').toString());
            wHContent.addsprintf("%s|",wRowList[wi]->Tab(wj).leftPad(wColSizeList[wj],' ').toString());
        } // for
        _DESD_Display(wHContent.toCChar());
    }// for

    _DESD_Display(wBar.toCChar());

    for (long wi=0 ; wi < wRowList.count() ; wi++) {
        delete wRowList[wi];
    }

    return wSt;
} // ZSearchParser::_DisplayEntityJoinDefault
utf8VaryingString
leftPad(const utf8VaryingString& pString,int pSize,utf8_t pPadChar)
{
    utf8VaryingString wReturn;
    ssize_t wPad = ssize_t(pSize - pString.strlen());
    if (wPad > 0)
        wReturn.setChar(pPadChar,0,wPad);
    wReturn += pString;
    return wReturn;
}

utf8VaryingString
rightPad(const utf8VaryingString& pString,int pSize,utf8_t pPadChar)
{
    utf8VaryingString wReturn = pString ;
    ssize_t wPad = ssize_t(pSize - pString.strlen());
    if (wPad > 0)
        wReturn.addChar(pPadChar,wPad);
    return wReturn;
}

#ifdef __COMMENT__
QList<QStandardItem *> ZSearchParser::DisplayOneLine( ZDataBuffer &pRecord)
{
  utf8VaryingString wStr;
  utf8VaryingString wRow;
  zaddress_type wOffset=0;
  const unsigned char* wPtr = pRecord.Data;
  const unsigned char* wPtrEnd = pRecord.Data + pRecord.Size;
  int wFieldRank = 0;
  ZTypeBase wZType;
  ZBitset wPresence;

  _importAtomic<ZTypeBase>(wZType,wPtr);
  while (true) {
    if ((wZType != ZType_bitset) && (wZType != ZType_bitsetFull)) {
      utf8VaryingString wStr;
      wStr.sprintf("Invalid format. While expecting <ZType_bitset>, found <%6X> <%s>.",wZType,decode_ZType(wZType));
      statusbar->showMessage(wStr.toCChar());
      displayMWn->appendTextColor(Qt::red,wStr.toCChar());
      break;
    }
    if (wZType==ZType_bitset) {
      wPtr -= sizeof(ZTypeBase);
      ssize_t wSize=wPresence._importURF(wPtr);
      wOffset += wSize;
    } // if (wZType==ZType_bitset)

    else if (wZType==ZType_bitsetFull) {
      wPresence.setFullBitset();
      wOffset += sizeof(ZTypeBase);
    } // if (wZType==ZType_bitsetFull)
     break;
  }// while true
  ZSearchDictionary& wDic = CurrentEntity->BuildDic;
  ZCFMT_Type wCellFormat ;

  while (wPtr < wPtrEnd) {
    if (!wPresence.isFullBitset() && (wFieldRank >= int(wPresence.count())))
      break;
    if (wPresence[wFieldRank]) {
      wCellFormat = wDic[wFieldRank].getCellFormat() |  wDic.getGlobalCellFormat();
      wRow += DisplayOneURFField(wPtr,wPtrEnd,wCellFormat);
    }
    else
      wRow += "<missing>";
    wFieldRank++;
  } // while

  return wRow;
} //DisplayOneLine


utf8VaryingString
ZSearchParser::DisplayOneURFField(const unsigned char *&pPtr,const unsigned char *wPtrEnd, ZCFMT_Type pCellFormat)
{
  ZTypeBase wZType;
  size_t    wOffset=0;


  ZStatus wSt=ZS_SUCCESS;

  utf8VaryingString wStr;

  _importAtomic<ZTypeBase>(wZType,pPtr);

  /* for atomic URF data, value is just following ZType. For other types, use _importURF function that implies ZType */
  if (!(wZType & ZType_Atomic))
    pPtr -= sizeof(ZTypeBase);
  else  {
    wZType &= ~ZType_Atomic;
    wOffset += sizeof(ZTypeBase);
  }
  switch (wZType) {
  case ZType_UChar:
  case ZType_U8: {
    uint8_t wValue;

    wValue=convertAtomicBack<uint8_t> (ZType_U8,pPtr);

    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%u",wValue);
    return  wStr;
  }
  case ZType_Char:
  case ZType_S8: {
    int8_t wValue;
    wValue=convertAtomicBack<int8_t> (ZType_S8,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%d",wValue);
    return wStr;
  }
  case ZType_U16:{
    uint16_t wValue;
    wValue=convertAtomicBack<uint16_t> (ZType_U16,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
        wStr.sprintf("%u",wValue);

    return wStr;
  }
  case ZType_S16: {
    int16_t wValue;
    wValue=convertAtomicBack<int16_t> (ZType_S16,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%d",wValue);
    return wStr;
  }

  case ZType_U32:{
    uint32_t wValue;
    wValue=convertAtomicBack<uint32_t> (ZType_U32,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%u",wValue);
    return wStr;
    break;
  }
  case ZType_S32: {
    int32_t wValue;
    wValue=convertAtomicBack<int32_t> (ZType_S32,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%d",wValue);
    return wStr;
  }
  case ZType_U64: {
    uint64_t wValue;
    wValue=convertAtomicBack<uint64_t> (ZType_U64,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%llu",wValue);
   return wStr;
  }
  case ZType_S64: {
    int64_t wValue;
    wValue=convertAtomicBack<int64_t> (ZType_S64,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%lld",wValue);

    return wStr;
  }
  case ZType_Float: {
    float wValue;
    wValue=convertAtomicBack<float> (ZType_Float,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%a",wValue);
    else
      wStr.sprintf("%g",wValue);
    return wStr ;
  }

  case ZType_Double: {
    double wValue;
    wValue=convertAtomicBack<double> (ZType_Double,pPtr);

    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%a",wValue);
    else
      wStr.sprintf("%g",wValue);
    return wStr ;
  }

  case ZType_LDouble: {
    long double wValue;
    wValue=convertAtomicBack<long double> (ZType_LDouble,pPtr);

    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%a",wValue);
    else
      wStr.sprintf("%g",wValue);
    return wStr ;

  }

  case ZType_ZDateFull: {
    ssize_t wSize;
    ZDateFull wZDateFull;
    utf8VaryingString wStr;

    if ((wSize = wZDateFull._importURF(pPtr)) < 0) {
      return "**Invalid date value**";
    }
    int wFmt = pCellFormat & ZCFMT_DateMask;
    switch (wFmt)
    {

    case ZCFMT_DMY:
      wStr=wZDateFull.toDMY();
      break;
    case ZCFMT_MDY:
      wStr=wZDateFull.toMDY();
      break;
    case ZCFMT_MDYHMS:
      wStr=wZDateFull.toMDYhms();
      break;
    case ZCFMT_DMYHMS:
      wStr=wZDateFull.toDMYhms();
      break;
    case ZCFMT_DLocale:
      wStr=wZDateFull.toLocale();
      break;
    case ZCFMT_DUTC:
      wStr=wZDateFull.toUTCGMT();
      break;
    case ZCFMT_Nothing:
    default:
      wStr=wZDateFull.toLocale();
      break;
    }
    return wStr;
  } //ZType_ZDateFull

  case ZType_URIString:{
    uriString wString;
    ssize_t wSize = wString._importURF(pPtr);

    if (wString.strlen() > StringDiplayMax){
      utf8VaryingString w1 = cst_OverflowChar8 ;
      w1 += wString.Right(StringDiplayMax).toString();

      wString = w1;
    }

    if (wString.isEmpty()) {
      return "<empty string>";
    }

    return wString ;
  }
  case ZType_Utf8VaryingString: {
    utf8VaryingString wString;
    ssize_t wSize = wString._importURF(pPtr);

    if (wString.strlen() > StringDiplayMax){
      utf8VaryingString w1 = cst_OverflowChar8 ;
      w1 += wString.Right(StringDiplayMax).toString();

      wString = w1;
    }

    if (wString.isEmpty()) {
      return "<empty string>";
    }

    return wString ;
  }


  case ZType_Utf16VaryingString:{
    utf16VaryingString wString;

    ssize_t wSize = wString._importURF(pPtr);
    if (wString.strlen() > StringDiplayMax){
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar16);
    }
    if (wString.isEmpty()) {
      return  new QStandardItem("<empty string>");
    }
    return new QStandardItem(QString((const QChar *)wString.toString()));
  }
  case ZType_Utf32VaryingString:{
    utf32VaryingString wString;
    utf16VaryingString wAdd;

    ssize_t wSize = wString._importURF(pPtr);
    if (wString.strlen() > StringDiplayMax){
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar32);
      wString += wAdd ;
    }
    if (wString.isEmpty()) {
      return new QStandardItem("<empty string>");
    }

    return wString.toUtf8();
  }

  case ZType_Utf8FixedString:{
    utf8VaryingString wString;

    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wStringByteSize;

    pPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    wStringByteSize = size_t (wUnitsCount) * sizeof(utf8_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf8_t* wPtrOut = (utf8_t*)wString.Data;
    utf8_t* wPtrIn = (utf8_t*)pPtr;
    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    pPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString += cst_OverflowChar8 ;
    }
    wOffset += sizeof(ZTypeBase)+sizeof(URF_Capacity_type) +sizeof(URF_UnitCount_type)+ wStringByteSize;


    if (wUnitsCount == 0)
      return  "<empty string>";
    else
      return wString;
  } // ZType_Utf8FixedString

    /* for fixed string URF header is different */

  case ZType_Utf16FixedString:{
    utf16VaryingString wString;

    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;


    pPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf16_t);

    /* the whole string must be imported, then possibly truncated afterwards to maximum displayable */

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf16_t* wPtrOut = (utf16_t*)wString.Data;
    utf16_t* wPtrIn = (utf16_t*)pPtr;
    while ( wI-- && *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    pPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit( cst_OverflowChar16);
    }
    wOffset += sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + wStringByteSize ;


    if (wUnitsCount == 0)
      return "<empty string>";
    else
      return wString.toUtf8() ;
    break;
  }

  case ZType_Utf32FixedString:{
    utf32VaryingString wString;
    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;

    pPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf32_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf32_t* wPtrOut = (utf32_t*)wString.Data;
    utf32_t* wPtrIn = (utf32_t*)pPtr;

    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    pPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar32);
    }
    wOffset += sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + wStringByteSize ;

    if (wUnitsCount == 0)
      return "<empty string>";
    else
     return wString.toUtf8();
  }

  case ZType_CheckSum: {
    checkSum wCheckSum;

    wCheckSum._importURF(pPtr);
    wOffset += wCheckSum.getURFSize();

    return createItem(wCheckSum);
  }

  case ZType_MD5: {
    md5 wCheckSum;

    wCheckSum._importURF(pPtr);
    wOffset += wCheckSum.getURFSize();

    return createItem(wCheckSum);
    break;
  }

  case ZType_Blob: {
    uint64_t wDataSize;
    pPtr += sizeof(ZTypeBase);
    _importAtomic(wDataSize,pPtr);

    pPtr += size_t(wDataSize);

    return  new QStandardItem("<Blob content cannot be displayed>");
    break;
  }

  case ZType_bitset: {
    ZBitset wBitset;

    ssize_t wSize=wBitset._importURF(pPtr);
    utf8VaryingString wBS = "<";
    wBS += wBitset.toString();
    wBS += ">";

    wOffset += wBitset.getURFSize();
    return new QStandardItem(wBS.toCChar());

    break;
  }

  case ZType_bitsetFull: {
    pPtr += sizeof(ZTypeBase);
    return new QStandardItem("All bits are set");
  }

  case ZType_Resource: {
    ZResource wValue;
    ssize_t wSize=wValue._importURF(pPtr);

    if (!wValue.isValid()) {
      return new QStandardItem("<Invalid resource>");
    }
    else {

      int wFmt = pCellFormat & ZCFMT_ResMask;
      if (wFmt & ZCFMT_ResSymb) {
        int wi=0;
        for (; wi < ZEntitySymbolList.count();wi++)
          if (ZEntitySymbolList[wi].Value == wValue.Entity)
            break;
        utf8VaryingString wZEntitySymbol = "Unknown entity";
        if (wi < ZEntitySymbolList.count())
          wZEntitySymbol = ZEntitySymbolList[wi].Symbol;
        if (wFmt & ZCFMT_ResStd) {
          wStr.sprintf("ZResource[%s,%ld]",wZEntitySymbol.toCChar(),wValue.id);
        }
        else {
          wStr.sprintf("ZResource[%s,0x%X]",wZEntitySymbol.toCChar(),wValue.id);
        }
      } // ZCFMT_ResSymb
      else {
        if (wFmt & ZCFMT_ResStd) {
          wStr.sprintf("ZResource[%d,%ld]",wValue.Entity,wValue.id);
        }
        else {
          wStr.sprintf("ZResource[0x%X,0x%X]",wValue.Entity,wValue.id);
        }
      }// else

    } // else
    return new  QStandardItem(wStr.toCChar());
  }// ZType_Resource

  default: {
    pPtr += sizeof(ZTypeBase);
    return new QStandardItem("--Unknown data type--");
  }

  }// switch


  return new QStandardItem();
} // DisplayOneURFField
#endif // __COMMENT__


ZStatus
ZSearchParser::_parseContextFor(ZSearchContext& pContext)
{

}


/*
    save symbol ;
    save symbol to <xml file path> ;

    save instruction ;
*/

ZStatus
ZSearchParser::_parseContextSave(ZSearchContext& pContext)
{
    ZStatus wSt=ZS_SUCCESS;

    if (!pContext.advanceIndex()) /* skip save key word */
        return ZS_SYNTAX_ERROR;

    switch (pContext.CurrentToken->Type)
    {
    case ZSRCH_SYMBOL: {

        if (pContext.isEOF()) {
            uriString wXmlPath = __SEARCHPARSER_SYMBOL_FILE__;
            bool wExists=wXmlPath.exists();
            wSt = SymbolList.XmlSave(wXmlPath, &ErrorLog);
            if (wSt==ZS_SUCCESS) {
                ErrorLog.textLog("Symbol file %s successfully %s",wXmlPath.toString(),wExists?"replaced":"saved");
                return wSt;
            }
            ErrorLog.errorLog("Cannot write symbol file %s status %s",wXmlPath.toString(),decode_ZStatus(wSt));
            return wSt;
        }
        pContext.advanceIndex();

        if (pContext.CurrentToken->Type!=ZSRCH_TO)  {
            ErrorLog.errorLog("Expected keyword TO. Found %s instead line %d column %d",
                     pContext.CurrentToken->Text.toString(), pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
            pContext.LastErroredToken=pContext.CurrentToken;
            return ZS_MISS_KEYWORD;
        }

        pContext.advanceIndex();

        if (pContext.CurrentToken->Type!=ZSRCH_STRING_LITERAL) {
            ErrorLog.errorLog("Expected xml file path. Found %s instead line %d column %d",
                     pContext.CurrentToken->Text.toString(), pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
            pContext.LastErroredToken=pContext.CurrentToken;
            return ZS_MISS_KEYWORD;
        }
        uriString wXmlPath = pContext.CurrentToken->Text;
        bool wExists=wXmlPath.exists();
        wSt = SymbolList.XmlSave(wXmlPath, &ErrorLog);
        if (wSt==ZS_SUCCESS) {
            ErrorLog.textLog("Symbol file %s successfully %s",wXmlPath.toString(),wExists?"replaced":"saved");
            return wSt;
        }
        ErrorLog.errorLog("Cannot write symbol file %s status %s",wXmlPath.toString(),decode_ZStatus(wSt));
        return wSt;
    }//ZSRCH_SYMBOL
    default:
        ErrorLog.errorLog("Expected one of {symbol,... }. Found %s instead line %d column %d",
                 pContext.CurrentToken->Text.toString(), pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken=pContext.CurrentToken;
        return ZS_MISS_KEYWORD;
    }

    /* here parsing of save whole context */
    /*
    while (pContext.notEOF()) {
//        CurrentToken=pContext.CurrentToken;
        while (true) {
            if(pContext.CurrentToken->Type==ZSRCH_STRING_LITERAL)
                break;
        }
    }
    */
    return ZS_SUCCESS;
} //_parseContextSave


ZStatus
ZSearchParser::_parseContextShow(ZSearchContext& pContext)
{
  ZStatus wSt=ZS_SUCCESS;

    if (!pContext.advanceIndex(true))
        return pContext.Status;

    switch(pContext.CurrentToken->Type)
    {
    case ZSRCH_HISTORY:
    {
        pContext.advanceIndex(false);

        if (pContext.isEOF() ) {
          showHistory();
          return pContext.Status=ZS_SUCCESS ;
        }

        if (pContext.CurrentToken->Type!=ZSRCH_MAXIMUM) {
            ErrorLog.errorLog("Expected either keyword <MAXIMUM> or end of instruction line. Found <%s> line %d column %d.",
                     pContext.CurrentToken->Text.toString(),
                     pContext.CurrentToken->TokenLine,
                     pContext.CurrentToken->TokenColumn);
            pContext.LastErroredToken = pContext.CurrentToken;
          return pContext.Status=ZS_SYNTAX_ERROR;
        }
        showHistoryMaximum();
        pContext.advanceIndex(false);
        return pContext.Status=ZS_SUCCESS;
    }// ZSRCH_HISTORY
    case ZSRCH_ENTITY:
    {
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_IDENTIFIER) ) {
            showEntityList();
            return pContext.Status=ZS_SUCCESS;
        }
        pContext.TokenIdentifier = pContext.CurrentToken;

        pContext.advanceIndex(false);
        if (pContext.isEOF()) {
        return showEntity (pContext,pContext.TokenIdentifier,0xFF);
        }

        int wShowType=SHENT_Nothing;
        bool wLoop=true;
        while (wLoop) {
            switch (pContext.CurrentToken->Type)
            {
            case ZSRCH_WITH:
                 wShowType |= SHENT_With ;
                 pContext.advanceIndex(false);
            case ZSRCH_USING:
                wShowType |= SHENT_Using ;
                pContext.advanceIndex(false);
                break;
            case ZSRCH_ALL:
                wShowType |= SHENT_All ;
                pContext.advanceIndex(false);
                break;
            case ZSRCH_IDENTIFIER:
                if (!pContext.CurrentToken->Text.compareCase("BRIEF")) {
                    wShowType |= SHENT_Brief;
                    pContext.advanceIndex(false);
                    break;
                }
                if (!pContext.CurrentToken->Text.compareCase("DICTIONARIES")) {
                    wShowType |= SHENT_Dictionaries ;
                    pContext.advanceIndex(false);
                    break;
                }
                wLoop=false;
                break;
            case ZSRCH_SEMICOLON:
            default:
                wLoop=false;
                break;
            } // switch

        } // while

        return showEntity (pContext,pContext.TokenIdentifier,wShowType);

     }//ZSRCH_ENTITY

    case ZSRCH_SYMBOL:
    {
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_IDENTIFIER) ) {
            showSymbols();
            return pContext.Status=ZS_SUCCESS;
        }

        /* here we have an identifier following SHOW SYMBOL : specific symbol name to show */
        showSymbol (pContext,pContext.CurrentToken);
        pContext.advanceIndex(false);
        return pContext.Status ;
    }//ZSRCH_SYMBOL
    case ZSRCH_MASTERFILES:
    {
        showMasterFiles();
//        pContext.advanceIndex(false);
        return pContext.Status=ZS_SUCCESS;
    }// ZSRCH_MASTERFILES

    case ZSRCH_DISPLAY :
    {
        showDisplay();
        pContext.advanceIndex(false);
        return  pContext.Status=ZS_SUCCESS;
    }


    case ZSRCH_IDENTIFIER: /* if identifier : must be an entity name */
    {
        pContext.TokenIdentifier = pContext.CurrentToken;

        pContext.advanceIndex(false);
        if (pContext.isEOF()) {
        return showEntity (pContext,pContext.TokenIdentifier,0xFF);
        }

        int wShowType=SHENT_Nothing;
        bool wLoop=true;
        while (wLoop) {

            switch (pContext.CurrentToken->Type)
            {
            case ZSRCH_WITH:
                 wShowType |= SHENT_With ;
                 pContext.advanceIndex(false);
            case ZSRCH_USING:
                wShowType |= SHENT_Using ;
                pContext.advanceIndex(false);
                break;
            case ZSRCH_ALL:
                wShowType |= SHENT_All ;
                pContext.advanceIndex(false);
                break;
            case ZSRCH_IDENTIFIER:
                if (!pContext.CurrentToken->Text.compareCase("BRIEF")) {
                    wShowType |= SHENT_Brief;
                    pContext.advanceIndex(false);
                    break;
                }
                if (!pContext.CurrentToken->Text.compareCase("DICTIONARIES")) {
                    wShowType |= SHENT_Dictionaries ;
                    pContext.advanceIndex(false);
                    break;
                }
                wLoop=false;
                break;
            case ZSRCH_SEMICOLON:
            default:
                wLoop=false;
                break;
            } // switch
            if (pContext.isEOF())
                break;
        } // while

        return showEntity (pContext,pContext.TokenIdentifier,wShowType);


    }//ZSRCH_IDENTIFIER
    default:
    {
        /* if not one of above, then syntax error */
        pContext.LastErroredToken = pContext.CurrentToken;
        ErrorLog.errorLog("Syntax error: Found <%s> while expecting one of {ENTITY,MASTERFILE,SYMBOL,ZENTITY} at line %d column %d.",
            pContext.CurrentToken->Text.toString(),pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );

        /* suggest keywords */
        ZArray<utf8VaryingString> wSugArray ={"ENTITY","MASTERFILE","SYMBOL","ZENTITY" };
        /* add entities names to suggest */
        for (int wi=0;wi < EntityList.count();wi++)
            wSugArray.push(EntityList[wi]->EntityName);

        /* try to suggest */
        utf8VaryingString wSuggest=searchAdHocWeighted (pContext.CurrentToken->Text,wSugArray);
        if (!wSuggest.isEmpty())
            ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );

        return pContext.Status=ZS_SYNTAX_ERROR;
    }  // default
    }// switch


} // ZSearchParser::_parseContextShow


ZStatus
ZSearchParser::_parseContextShowEntity(ZSearchContext& pContext)
{

    int wEntityIndex=0;
    for (; wEntityIndex < EntityList.count(); wEntityIndex++) {
      if (EntityList[wEntityIndex]->getEntityName() == pContext.CurrentToken->Text) {
          break;
      } // if (EntityList[wEnti
    }// for


    if (wEntityIndex >= EntityList.count()) {
      ErrorLog.errorLog("Syntax error: Unrecognized keyword <%s> while expecting one of {ENTITY,MASTERFILE,SYMBOL,ZENTITY,<valid entity name>} at line %d column %d.",
          pContext.CurrentToken->Text.toString() ,pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
        pContext.LastErroredToken = pContext.CurrentToken;

        /* suggest keywords */
        ZArray<utf8VaryingString> wSugArray ={"ENTITY","MASTERFILE","SYMBOL","ZENTITY" };
        /* add entities names to suggest */
        for (int wi=0;wi < EntityList.count();wi++)
            wSugArray.push(EntityList[wi]->EntityName);

        /* try to suggest */
        utf8VaryingString wSuggest=searchAdHocWeighted (pContext.CurrentToken->Text,wSugArray);
        if (!wSuggest.isEmpty())
            ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );

      return pContext.Status=ZS_SYNTAX_ERROR;
    }

    pContext.SourceEntity = EntityList[wEntityIndex] ;
    pContext.TokenIdentifier = pContext.CurrentToken;

    pContext.advanceIndex(false);

    int wF=3;
    if (pContext.isEOF()) {
        wF=3;
        showEntityDetails(EntityList[wEntityIndex],wF);
        return pContext.Status= ZS_SUCCESS;
    }

    switch (pContext.CurrentToken->Type)
    {
//        case ZSRCH_SEMICOLON:

    case ZSRCH_STRING_LITERAL:
    {
        pContext.Status = showEntityField(pContext,EntityList[wEntityIndex],pContext.CurrentToken->Text);
        if (pContext.Status!=ZS_SUCCESS)
            return pContext.Status ;
        pContext.advanceIndex(false);
        return pContext.Status ;
     }

    case ZSRCH_IDENTIFIER:  /* request to show an entity field */
    {
        utf8VaryingString wSearchField = pContext.CurrentToken->Text;
        pContext.advanceIndex(false);
        if (pContext.isEOF() || (pContext.CurrentToken->Type != ZSRCH_OPERATOR_MULTIPLY) ) {
            for (int wi=0; wi < pContext.SourceEntity->LocalMetaDic.count(); wi++) {
                if (pContext.SourceEntity->LocalMetaDic[wi].getName()==pContext.CurrentToken->Text) {
                    _displayOneFieldDetail(pContext.SourceEntity->LocalMetaDic[wi]);
                    return pContext.Status=ZS_SUCCESS;
                }
            }// for

            /* field name not found
                NB : if eof then current token points to the last valid token before eof
            */
            ErrorLog.errorLog("Entity <%s> has no field named <%s> at line %d column %d.",
                pContext.CurrentToken->Text.toString() ,pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
              pContext.LastErroredToken = pContext.CurrentToken;

            ZArray<utf8VaryingString> wSugArray ;
            /* add entities names to suggest */
            for (int wi=0;wi < EntityList.count();wi++)
                wSugArray.push(EntityList[wi]->EntityName);

            /* try to suggest */
            utf8VaryingString wSuggest=searchAdHocWeighted (pContext.CurrentToken->Text,wSugArray);
            if (!wSuggest.isEmpty())
                ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );

            return pContext.Status=ZS_INVNAME;
        }// isEOF
/*
        if (pContext.CurrentToken->Type != ZSRCH_OPERATOR_MULTIPLY) {
            ErrorLog.errorLog("Invalid token <%s> at line %d column %d.",
                pContext.CurrentToken->Text.toString() ,pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
              pContext.LastErroredToken = pContext.CurrentToken;
              return pContext.Status=ZS_SYNTAX_ERROR;
        }
*/
        /* here current token is wild card for end of field name */
        bool wFound=false;
        for (int wi=0; wi < pContext.SourceEntity->LocalMetaDic.count(); wi++) {
            if (pContext.SourceEntity->LocalMetaDic[wi].getName().startsWithT(wSearchField)) {
                _displayOneFieldDetail(pContext.SourceEntity->LocalMetaDic[wi]);
                wFound=true;
            }
        }// for

        if (!wFound) {
            ErrorLog.errorLog("Entity <%s> has no field with name starting with <%s> at line %d column %d.",
                wSearchField.toString() ,pContext.previousToken()->TokenLine,pContext.previousToken()->TokenColumn);
              pContext.LastErroredToken = pContext.previousToken();

            ZArray<utf8VaryingString> wSugArray ;
            /* add entities names to suggest */
            for (int wi=0;wi < EntityList.count();wi++)
                wSugArray.push(EntityList[wi]->EntityName);

            /* try to suggest */
            utf8VaryingString wSuggest=searchAdHocWeighted (pContext.CurrentToken->Text,wSugArray);
            if (!wSuggest.isEmpty())
                ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );
            return pContext.Status=ZS_INVNAME;
        }
        pContext.advanceIndex(false);
        return pContext.Status=ZS_SUCCESS;

    } // ZSRCH_IDENTIFIER

    case ZSRCH_OPERATOR_MULTIPLY: /* wild card for beginning of name (ends with) or alone (all) or wild card-string-wild card (contains) */
    {
        bool wFound=false;
        pContext.advanceIndex(false);
        if (pContext.isEOF()) {  /* wild card alone : all fields requested to be displayed */
            for (int wi=0; wi < pContext.SourceEntity->LocalMetaDic.count(); wi++) {
                 _displayOneFieldDetail(pContext.SourceEntity->LocalMetaDic[wi]);
                 wFound=true;
            }// for

            return pContext.Status=ZS_SUCCESS;
        }
        /*
        if (pContext.CurrentToken->Type != ZSRCH_IDENTIFIER) {
            ErrorLog.errorLog("Expected identifier at line %d column %d. Found <%s>",
                pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn,pContext.CurrentToken->Text.toString());
              pContext.LastErroredToken = pContext.CurrentToken;
            return pContext.Status=ZS_SYNTAX_ERROR ;
        }
        */

        utf8VaryingString wSearchField = pContext.CurrentToken->Text;
        pContext.advanceIndex(false);
        if (pContext.isEOF()||(pContext.CurrentToken->Type != ZSRCH_OPERATOR_MULTIPLY)) {
            for (int wi=0; wi < pContext.SourceEntity->LocalMetaDic.count(); wi++) {
                if (pContext.SourceEntity->LocalMetaDic[wi].getName().endsWithT(wSearchField)) {
                    _displayOneFieldDetail(pContext.SourceEntity->LocalMetaDic[wi]);
                    wFound=true;
                }
            }// for
            if (!wFound) {
                ErrorLog.errorLog("Entity <%s> has no field with name ending with <%s> at line %d column %d.",
                                  pContext.SourceEntity->getEntityName().toString(),
                                  wSearchField.toString() ,pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
                  pContext.LastErroredToken = pContext.CurrentToken;
            }
            return pContext.Status=ZS_SUCCESS;
        }// isEOF

/*        if (pContext.CurrentToken->Type != ZSRCH_OPERATOR_MULTIPLY) {
            ErrorLog.errorLog("Invalid token <%s> at line %d column %d.",
                pContext.CurrentToken->Text.toString() ,pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn);
              pContext.LastErroredToken = pContext.CurrentToken;
              return pContext.Status=ZS_SYNTAX_ERROR;
        }
*/
        /* here search string enclosed in wild cards : contains */
        wFound=false;
        for (int wi=0; wi < pContext.SourceEntity->LocalMetaDic.count(); wi++) {
            if (pContext.SourceEntity->LocalMetaDic[wi].getName().containsT(wSearchField)) {
                _displayOneFieldDetail(pContext.SourceEntity->LocalMetaDic[wi]);
                wFound=true;
            }
        }// for

        if (!wFound) {
            ErrorLog.errorLog("Entity <%s> has no field with name containing <%s> at line %d column %d.",
                wSearchField.toString() ,pContext.previousToken()->TokenLine,pContext.previousToken()->TokenColumn);
              pContext.LastErroredToken = pContext.previousToken();

            ZArray<utf8VaryingString> wSugArray ;
            /* add entities names to suggest */
            for (int wi=0;wi < EntityList.count();wi++)
                wSugArray.push(EntityList[wi]->EntityName);

            /* try to suggest */
            utf8VaryingString wSuggest=searchAdHocWeighted (pContext.CurrentToken->Text,wSugArray);
            if (!wSuggest.isEmpty())
                ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );
            return pContext.Status=ZS_INVNAME;
        }
        pContext.advanceIndex(false);
        return pContext.Status=ZS_SUCCESS;

    } // ZSRCH_OPERATOR_MULTIPLY

    case ZSRCH_ALL:
        wF=3;
        break;
    case ZSRCH_FIELDS:
        wF=1;
        break;
    case ZSRCH_FORMULA:
        wF=2;
        break;
    default:
        pContext.LastErroredToken = pContext.CurrentToken;
        ErrorLog.errorLog("Syntax error: Found <%s> while expecting one of {ALL,FORMULA,FIELDS} at line %d column %d.",
                 pContext.CurrentToken->Text.toString(),pContext.CurrentToken->TokenLine,pContext.CurrentToken->TokenColumn );
        pContext.LastErroredToken = pContext.CurrentToken;

        ZArray<utf8VaryingString> wSugArray ={"ALL","FORMULA","FIELDS" };
        utf8VaryingString wSuggest=searchAdHocWeighted (pContext.CurrentToken->Text,wSugArray);
        if (!wSuggest.isEmpty())
            ErrorLog.textLog("Do you mean <%s> ?", wSuggest.toString() );

        return ZS_SYNTAX_ERROR;
    }

    showEntityDetails(EntityList[wEntityIndex],wF);

    return pContext.Status=ZS_SUCCESS;

} // ZSearchParser::_parseContextShowEntity

void
ZSearchParser::_displayOneFieldDetail(const ZFieldDescription& pField)
{
    ErrorLog.textLog("%25s %25s ",
            pField.getName().toString(),
            decode_ZType(pField.ZType));
}
#ifdef __COMMENT__
ZStatus
ZSearchParser::_executeFind(std::shared_ptr <ZSearchEntity>  pCollection)
{
    if(pCollection==nullptr) {
        ErrorLog.errorLog("ZSearchParser::_executeFind-E-NULLPTR Requested collection to execute is null.");
        return ZS_NULLPTR;
    }
  pCollection->AddressList.clear();
  ZDataBuffer wRecord;
  if (ProgressSetupCallBack!=nullptr)
    ProgressSetupCallBack(int(pCollection->_rootGetCount()),"Finding collection");
  if (ProgressCallBack!=nullptr)
    ProgressCallBack(0,"Preparing");
  long wRank=0;
  zaddress_type wAddress=0;

  ZTimer wTi;
  wTi.start();

  pCollection->AddressList.clear();

  ZStatus wSt=pCollection->_baseGet(wRecord,wRank,wAddress);
  int wSelected=0;
  int wUpdate=0;
  bool wResult;


  while (wSt==ZS_SUCCESS) {
    _DBGPRINT("ZSearchParser::_executeFind processing record rank %ld\n",wRank)
//    wSt=pCollection->evaluateRecord(wResult,wRecord);
    wSt=pCollection->evaluateRecord(wResult);
    _DBGPRINT("ZSearchParser::_executeFind evaluate status <%s> result is <%s>\n",
              decode_ZStatus(wSt),
              wResult?"true":"false")
    if(wResult) {
      pCollection->AddressList.push(wAddress);
      wSelected++;
    }

    if (ProgressCallBack!=nullptr) {
      wUpdate++;
      if (wUpdate > UpdateFrequence) {
        wUpdate=0;
        ProgressCallBack(wRank,utf8VaryingString());
      }
    }
    wRank++;
    wSt=pCollection->_baseGet(wRecord,wRank,wAddress);
  } // while ZS_SUCCESS
/*
  if (wSt==ZS_OUTBOUNDHIGH) {
    wSt=ZS_SUCCESS;
  }
*/
  ProgressCallBack(wRank,utf8VaryingString());

  wTi.end();
  ErrorLog.infoLog("_executeFind report\n"
          " Number of record processed  %d\n"
          " Matches (address count)     %d\n"
          " End status                  %s\n"
          " Elapsed                     %s",
      wRank,pCollection->AddressList.count(),decode_ZStatus(wSt),wTi.reportDeltaTime().toString());

  return wSt;
}
#endif // __COMMENT__

void ZSearchParser::displayTokenList(ZArray<ZSearchToken*> &Whole)
{
  ZSearchTokenizer::_print("    Token list  <%ld>\n",Whole.count());
  for (long wi=0;wi<Whole.count();wi++)
    fprintf(stdout,"     %3ld [line %d col %d]  %15s-<%15s> \n",
        wi,Whole[wi]->TokenLine,Whole[wi]->TokenColumn,
        decode_SearchTokenType( Whole[wi]->Type).toCChar(),Whole[wi]->Text.toCChar());
}

void ZSearchParser::displayTokenList()
{
  ZSearchTokenizer::_print("    Token list  <%ld>\n",count());
  for (long wi=0;wi < count();wi++)
    fprintf(stdout,"     %3ld [line %d col %d]  %15s-<%15s> \n",
        wi,Tab(wi)->TokenLine,Tab(wi)->TokenColumn,
        decode_SearchTokenType( Tab(wi)->Type).toCChar(),Tab(wi)->Text.toCChar());
}

/*
<?xml version='1.0' encoding='UTF-8'?>
 <zsearchparserzentity version = "'0.30-0'">
    <!--  ZEntity table : defines application dependant ZEntity values. Values are expressed in hexadecimal format -->
    <zentitytable>
        <zentityitem>
            <zentity>ZEntity_DocFile</zentity>
            <value>0x1234</value>
        </zentityitem>
    </zentitytable>

</zsearchparserzentity>
*/
ZStatus
ZSearchParser::loadXmlSearchParserZEntity(const uriString& pXmlFile)
{
  utf8VaryingString wXmlString;
  ZStatus wSt;

  ErrorLog.setAutoPrintOn(ZAIES_Text);

  if (!pXmlFile.exists())  {
    ErrorLog.errorLog("ZSearchParser::loadXmlSearchParserZEntity-E-FILNFND Parameter file <%s> has not been found.",pXmlFile.toCChar());
    return ZS_FILENOTEXIST;
  }

  URIZEntity=pXmlFile;

  if ((wSt=URIZEntity.loadUtf8(wXmlString))!=ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    return wSt;
  }

  zxmlDoc     *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;
  zxmlElement *wParamRootNode=nullptr;
  zxmlElement *wSymbolListNode=nullptr;

  zxmlElement *wSymbolKeyword=nullptr;
  zxmlElement *wSwapNode=nullptr;

  utf8VaryingString wKeyword;
  long              wValue;

  utf8VaryingString     wSkipIdentifier;
  utf8VaryingString     wSkipEndToken;

  wDoc = new zxmlDoc;
  wSt = wDoc->ParseXMLDocFromMemory(wXmlString.toCChar(), wXmlString.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    ErrorLog.errorLog(
        "ZSearchParser::loadXmlSearchParserZEntity-E-PARSERR Xml parsing error for string <%s> ",
        wXmlString.subString(0, 25).toUtf());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    return wSt;
  }
  if (!(wRoot->getName() == "zsearchparserzentity")) {
    ErrorLog.errorLog(
        "ZSearchParser::loadXmlSearchParserZEntity-E-INVROOT Invalid root node name <%s> expected <zsearchparserzentity>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  /*------------------ ZEntity table -----------------------*/

  while (true) {
    wSt=wRoot->getChildByName((zxmlNode*&)wSymbolListNode,"zentitytable");
    if (wSt!=ZS_SUCCESS) {
      ErrorLog.logZStatus(
          ZAIES_Error,
          wSt,
          "ZSearchParser::loadXmlSearchParserZEntity-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
          "zentitytable",
          decode_ZStatus(wSt));
      break;
    }


    wSt=wSymbolListNode->getFirstChild((zxmlNode*&)wSymbolKeyword);

    ZEntitySymbol wZEntityElt;
    while (wSt==ZS_SUCCESS) {
      if (wSymbolKeyword->getName()=="zentityitem") {
        wSt=XMLgetChildText( wSymbolKeyword,"zentity",wKeyword,&ErrorLog);
        if (wSt >= 1)
           wZEntityElt.Symbol = wKeyword ;

        wSt=XMLgetChildLongHexa( wSymbolKeyword,"value",wValue,&ErrorLog);
        wZEntityElt.Value = wValue;

        ZEntitySymbolList.push(wZEntityElt);
      } // typeconversion
      wSt=wSymbolKeyword->getNextNode((zxmlNode*&)wSwapNode);
      XMLderegister(wSymbolKeyword);
      wSymbolKeyword=wSwapNode;
    }// while (wSt==ZS_SUCCESS)

    ErrorLog.textLog("_________________Search Parser ZEntity table load ____________________"
                              " %ld ZEntity values loaded.", ZEntitySymbolList.count());
    ErrorLog.textLog("%3s- %25s %8s\n","rnk","ZEntity","Value");
    for (long wi=0;wi<ZEntitySymbolList.count();wi++) {
      ErrorLog.textLog("%3ld- %25s 0x%08lX",wi+1,
          ZEntitySymbolList[wi].Symbol.toString(),
          ZEntitySymbolList[wi].Value
          );
    }
    ErrorLog.textLog("________________________________________________________________\n");
    XMLderegister(wSymbolListNode);
    break;
  } // while (true)


  XMLderegister((zxmlNode *&) wParamRootNode);
  XMLderegister((zxmlNode *&) wRoot);

  if (wSt==ZS_EOF)
    return ZS_SUCCESS;
  return wSt;
}

/*
<?xml version='1.0' encoding='UTF-8'?>
 <zsearchparsercontext version = "'0.30-0'">
    <!--  context gathers all session definitions : entities, etc. -->
  <entities>
    <files>
        <fileitem>
            <path>/home/mydir/filename.ext</path>
            <symbol>mysymbol</symbol>
        </fileitem>
    </files>
    <collections>
      <collectionitem>
          <selectionphrase> </selectionphrase>
          <entityref> </entityref>
          <selectionclause>
            <logicalterm>
            </logicalterm>
          </selectionclause>

      </collectionitem>
    </collections>
  </entities>
</zsearchparsercontext>
*/
ZStatus
ZSearchParser::saveContext(uriString& pXmlFile)
{
  return ZS_SUCCESS;
}

/*
<?xml version='1.0' encoding='UTF-8'?>
 <zsearchparsersymbol version = "'0.30-0'">
    <!--  Symbols table : defines symbols that points to a valid full file path -->
    <symboltable>
        <symbolitem>
            <path>/home/mydir/filename.ext</path>
            <symbol>mysymbol</symbol>
        </symbolitem>
    </symboltable>

</zsearchparsersymbol>
*/
ZStatus
ZSearchParser::loadXmlSearchParserSymbols(const uriString& pXmlFile)
{
  utf8VaryingString wXmlString;
  ZStatus wSt;

  ErrorLog.setAutoPrintOn(ZAIES_Text);

  ErrorLog.textLog(" Loading symbol table <%s>",pXmlFile.toString());

  if (!pXmlFile.exists())  {
    ErrorLog.errorLog("ZSearchParser::loadXmlSearchParserSymbols-E-FILNFND Parameter file <%s> has not been found.",pXmlFile.toCChar());
    return ZS_FILENOTEXIST;
  }

  URIZEntity=pXmlFile;

  if ((wSt=URIZEntity.loadUtf8(wXmlString))!=ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    return wSt;
  }

  zxmlDoc     *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;
  zxmlElement *wParamRootNode=nullptr;
  zxmlElement *wSymbolListNode=nullptr;

  zxmlElement *wSymbolKeyword=nullptr;
  zxmlElement *wSwapNode=nullptr;

  utf8VaryingString wKeyword;

  utf8VaryingString     wSkipIdentifier;
  utf8VaryingString     wSkipEndToken;



  wDoc = new zxmlDoc;
  wSt = wDoc->ParseXMLDocFromMemory(wXmlString.toCChar(), wXmlString.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    ErrorLog.errorLog(
        "ZSearchParser::loadXmlSearchParserSymbols-E-PARSERR Xml parsing error for string <%s> ",
        wXmlString.subString(0, 25).toUtf());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    return wSt;
  }
  if (!(wRoot->getName() == "zsearchparsersymbol")) {
    ErrorLog.errorLog(
        "ZSearchParser::loadXmlSearchParserSymbols-E-INVROOT Invalid root node name <%s> expected <zsearchparsersymbol>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  /*------------------ Symbol table -----------------------*/

  while (true) {
    wSt=wRoot->getChildByName((zxmlNode*&)wSymbolListNode,"symboltable");
    if (wSt!=ZS_SUCCESS) {
      ErrorLog.logZStatus(
          ZAIES_Error,
          wSt,
          "ZSearchParser::loadXmlSearchParserSymbols-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
          "symboltable",
          decode_ZStatus(wSt));
      break;
    }


    wSt=wSymbolListNode->getFirstChild((zxmlNode*&)wSymbolKeyword);

    ZSearchSymbol wSymbolElt;

#ifdef __DEPRECATED__
    while (wSt==ZS_SUCCESS) {
      if (wSymbolKeyword->getName()=="symbolitem") {
        wSt=XMLgetChildText( wSymbolKeyword,"symbol",wKeyword,this);
        if (wSt >= 1)
          wSymbolElt.Symbol = wKeyword ;

        wSt=XMLgetChildText( wSymbolKeyword,"path",wKeyword,this);
        wSymbolElt.Path = wKeyword;

        SymbolList.push(wSymbolElt);
      } // typeconversion
      wSt=wSymbolKeyword->getNextNode((zxmlNode*&)wSwapNode);
      XMLderegister(wSymbolKeyword);
      wSymbolKeyword=wSwapNode;
    }// while (wSt==ZS_SUCCESS)
#endif // __DEPRECATED__


    while (wSt==ZS_SUCCESS) {
        wSt=wSymbolElt.fromXml(wSymbolKeyword);
        SymbolList.push(wSymbolElt);
        wSt=wSymbolKeyword->getNextNode((zxmlNode*&)wSwapNode);
        XMLderegister(wSymbolKeyword);
        wSymbolKeyword=wSwapNode;
    }// while (wSt==ZS_SUCCESS)

    ErrorLog.textLog("_________________Search Parser symbols table load ____________________\n"
                              " %ld symbols loaded.\n", SymbolList.count());
    ErrorLog.textLog("%3s- %27s %s\n","rnk","Symbol","Content");
    for (long wi=0;wi<SymbolList.count();wi++) {
      ErrorLog.textLog("%3ld- <%25s> <%s>",wi+1,
          SymbolList[wi].FullFieldName.toString(),
          SymbolList[wi].display().toString()
          );
    }
    ErrorLog.textLog("________________________________________________________________\n");
    XMLderegister(wSymbolListNode);
    break;
  } // while (true)


  XMLderegister((zxmlNode *&) wParamRootNode);
  XMLderegister((zxmlNode *&) wRoot);

  if (wSt==ZS_EOF)
    return ZS_SUCCESS;
  return wSt;
}

/*
<?xml version='1.0' encoding='UTF-8'?>
 <zsearchparserparameters version = "'0.30-0'">
    <!--  ZSearchParser parameters -->
    <history>
      <maximum>40</maximum>
    </history>
    <options>
      <!-- Default options are ZSRCHO_Report | ZSRCHO_SkipNL -->
      <include>false</include> <!-- Allows  include files -->
      <verbose>false</verbose> <!-- messages starting at criticity level ZAIES_Warning are displayed -->
      <fullverbose>false</fullverbose> <!-- messages starting at criticity level ZAIES_Text are displayed -->
      <skipnl>true</skipnl> <!-- Tells tokenizer not to count newline in TokenOffset -->
      <report>false</report> <!-- generates a report after parsing process -->
    </options>
    <symboltable>
        <symbolitem>
            <path>/home/mydir/filename.ext</path>
            <symbol>mysymbol</symbol>
        </symbolitem>
    </symboltable>

</zsearchparserparameters>
*/
ZStatus
ZSearchParser::loadXmlSearchParserParams(const uriString& pXmlFile)
{
  utf8VaryingString wXmlString;
  ZStatus wSt;

  ErrorLog.setAutoPrintOn(ZAIES_Text);

  ErrorLog.textLog(" Loading symbol table <%s>",pXmlFile.toString());

  if (!pXmlFile.exists())  {
    ErrorLog.errorLog("ZSearchParser::loadXmlSearchParserParams-E-FILNFND Parameter file <%s> has not been found.",pXmlFile.toCChar());
    return ZS_FILENOTEXIST;
  }

  URIParams=pXmlFile;

  if ((wSt=URIParams.loadUtf8(wXmlString))!=ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    return wSt;
  }

  zxmlDoc     *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;
  zxmlElement *wParamRootNode=nullptr;
  zxmlElement *wParamsNode=nullptr;

  zxmlElement *wParamKeyword=nullptr;
  zxmlElement *wSwapNode=nullptr;

  utf8VaryingString wKeyword;

  utf8VaryingString     wSkipIdentifier;
  utf8VaryingString     wSkipEndToken;



  wDoc = new zxmlDoc;
  wSt = wDoc->ParseXMLDocFromMemory(wXmlString.toCChar(), wXmlString.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    ErrorLog.errorLog(
        "ZSearchParser::loadXmlSearchParserSymbols-E-PARSERR Xml parsing error for string <%s> ",
        wXmlString.subString(0, 25).toUtf());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    ErrorLog.logZExceptionLast();
    return wSt;
  }
  if (!(wRoot->getName() == "zsearchparsersymbol")) {
    ErrorLog.errorLog(
        "ZSearchParser::loadXmlSearchParserSymbols-E-INVROOT Invalid root node name <%s> expected <zsearchparsersymbol>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  /*------------------ Symbol table -----------------------*/

  while (true) {
    wSt=wRoot->getChildByName((zxmlNode*&)wParamsNode,"symboltable");
    if (wSt!=ZS_SUCCESS) {
      ErrorLog.logZStatus(
          ZAIES_Error,
          wSt,
          "ZSearchParser::loadXmlSearchParserSymbols-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
          "symboltable",
          decode_ZStatus(wSt));
      break;
    }


    wSt=wParamsNode->getFirstChild((zxmlNode*&)wParamKeyword);

    ZSearchSymbol wSymbolElt;
    while (wSt==ZS_SUCCESS) {
      wSt =  wSymbolElt.fromXml(wParamKeyword,"symbolitem",&ErrorLog);
/*      if (wParamKeyword->getName()=="symbolitem") {
        wSt=XMLgetChildText( wParamKeyword,"symbol",wKeyword,this);
        if (wSt >= 1)
          wSymbolElt.Symbol = wKeyword ;

        wSt=XMLgetChildText( wParamKeyword,"path",wKeyword,this);
        wSymbolElt.Path = wKeyword;

        SymbolList.push(wSymbolElt);
      } // typeconversion
*/
      SymbolList.push(wSymbolElt);
      wSt=wParamKeyword->getNextNode((zxmlNode*&)wSwapNode);
      XMLderegister(wParamKeyword);
      wParamKeyword=wSwapNode;
    }// while (wSt==ZS_SUCCESS)

    XMLderegister(wParamsNode);
    break;
  } // while (true)

  XMLderegister((zxmlNode *&) wParamRootNode);
  XMLderegister((zxmlNode *&) wRoot);

  ErrorLog.textLog("_________________Search Parser symbols table load ____________________\n"
          " %ld symbols loaded.\n", SymbolList.count());
  ErrorLog.textLog("%3s- %27s %s\n","rnk","Symbol","Path");
  for (long wi=0;wi<SymbolList.count();wi++) {
      ErrorLog.textLog("%3ld- <%25s> <%s>\n",wi+1,
              SymbolList[wi].FullFieldName.toString(),
              SymbolList[wi].display().toString()
              );
  }
  ErrorLog.textLog("________________________________________________________________\n");

  if (wSt==ZS_EOF)
    return ZS_SUCCESS;
  return wSt;
}



ZTypeBase
ZSearchParser::convertToZType(ZSearchTokentype_type pType) {

  ZTypeBase wZTp=0;
  /*  if ((pType & ZSRCH_LONG)== ZSRCH_LONG) {
    if (sizeof(long)==4)
      wZTp = ZType_AtomicU32;
    else
      wZTp = ZType_AtomicU64;
  }
  else {
*/
  unsigned int wTp = pType & ~ZSRCH_MODIFIER_MASK;

  if (pType & ZSRCH_ARRAY) {
    wZTp |= ZType_Array;
    wTp &= ~ ZSRCH_ARRAY;
  }
  if (pType & ZSRCH_ARRAY) {
    wZTp |= ZType_Array;
    wTp &= ~ ZSRCH_ARRAY;
  }


  switch (wTp) {
  case ZSRCH_INT8:
    wZTp |= ZType_AtomicU8;
    break;
  case ZSRCH_INT16:
    wZTp |= ZType_AtomicU16;
    break;
  case ZSRCH_INT32:
    wZTp |= ZType_AtomicU32;
    break;
  case ZSRCH_INT64:
    wZTp |= ZType_AtomicU64;
    break;
  case ZSRCH_CHAR:
    wZTp |= ZType_Char;
    break;

  case ZSRCH_LONG:
    if (sizeof(long)==4)
      wZTp |= ZType_AtomicU32;
    else
      wZTp |= ZType_AtomicU64;
    break;
  case ZSRCH_LONG_LONG:
    wZTp |= ZType_U64;
    break;
  case ZSRCH_FLOAT:
    wZTp |= ZType_Float;
    break;
  case ZSRCH_DOUBLE:
    if (pType & ZSRCH_LONG)
      wZTp |= ZType_LDouble;
    else
      wZTp |= ZType_Double;
    break;
  case ZSRCH_LONG_DOUBLE:
    wZTp |= ZType_LDouble;
    break;

  case ZSRCH_BOOL:
    wZTp |= ZType_Bool;
    break;

  case ZSRCH_UTF8VARYINGSTRING:
    wZTp |= ZType_Utf8VaryingString;
    break;
  case ZSRCH_UTF16VARYINGSTRING:
    wZTp |= ZType_Utf16VaryingString;
    break;

  case ZSRCH_UTF32VARYINGSTRING:
    wZTp |= ZType_Utf32VaryingString;
    break;

  case ZSRCH_URISTRING:
    wZTp = ZType_URIString;
    break;

  case ZSRCH_UTF8FIXEDSTRING:
    wZTp |= ZType_Utf8FixedString;
    break;

  case ZSRCH_UTF16FIXEDSTRING:
    wZTp |= ZType_Utf16FixedString;
    break;

  case ZSRCH_UTF32FIXEDSTRING:
    wZTp |= ZType_Utf32FixedString;
    break;


  case ZSRCH_ZDATEFULL:
    wZTp |= ZType_ZDateFull;
    break;
    /*    case ZSRCH_ZDATE:
      wZTp |= ZType_ZDate;
      break;
*/
  case ZSRCH_ZBITSET:
    wZTp |= ZType_bitset;
    break;

  case ZSRCH_CHECKSUM:
    wZTp |= ZType_CheckSum;
    break;

  case ZSRCH_MD5:
    wZTp |= ZType_MD5;
    break;

  case ZSRCH_STDSTRING:
    wZTp |= ZType_StdString;
    break;

  case ZSRCH_ZRESOURCE:
  case ZSRCH_ZRESOURCECODE:
    wZTp |= ZType_Resource;
    break;

  default:
    if ((wZTp==0)&&(pType&ZSRCH_LONG)) {
      if (sizeof(long)==4)
        wZTp |= ZType_AtomicU32;
      else
        wZTp |= ZType_AtomicU64;
      break;
    }
    wZTp |= ZType_Unknown;
    break;
  }// switch
  //  } // else

  if (wTp==ZSRCH_CHAR) {
    if (pType & ZSRCH_UNSIGNED)
      wZTp |= ZType_AtomicUChar;
    else
      wZTp |= ZType_AtomicChar  ;
  }

  /* const modifier is diguarded */

  /* only if atomic : take care of sign flag */

  if (wZTp & ZType_Atomic) {

    /* sign applies for Atomic data but not for floating types kind (ZType_Float and higher) */
    if ((wZTp&~(ZType_Atomic)) < ZType_Float) {
      if (!(pType & ZSRCH_UNSIGNED))
        wZTp |= ZType_Signed;
    }
  }
  /* pointer modifier concerns any data type */

  if (pType & ZSRCH_POINTER)
    wZTp |= ZType_Pointer;

  if (pType & ZSRCH_AMPERSAND)   // reference is not taken into account in ZType
    ErrorLog.infoLog("ZSearchParser::convertToZType-I-REFSKIP Found reference (Ampersand). Skipped.");
  if (pType & ZSRCH_LONG) {
    if ((pType & ZSRCH_DOUBLE)==ZSRCH_DOUBLE)
      wZTp = ZType_LDouble ;
  } // ZSRCH_LONG

  return wZTp;
} // ZCppType::convertToZType



ZStatus
extractURIfromLiteral(const utf8VaryingString& pPathLiteral,uriString pURIOut)
{
  pURIOut = pPathLiteral;
  pURIOut.eliminateChar('"');
  return pURIOut.check();
}




/*
void ZParserError::ErrorLog.errorLog(const char* pFormat,...)
{
  pContext.LastErroredToken = Father->CurrentToken;
  va_list args;
  va_start (args, pFormat);
  _log(ZAIES_Error,pFormat,args);

  va_end(args);
}
void ZParserError::textLog(const char* pFormat,...)
{
  pContext.LastErroredToken= Father->CurrentToken;
  va_list args;
  va_start (args, pFormat);
  _log(ZAIES_Text,pFormat,args);
  va_end(args);
}
void ZParserError::ErrorLog.infoLog(const char* pFormat,...)
{
  pContext.LastErroredToken= Father->CurrentToken;
  va_list args;
  va_start (args, pFormat);
   _log(ZAIES_Info,pFormat,args);
  va_end(args);
}
void ZParserError::warningLog(const char* pFormat,...)
{
  pContext.LastErroredToken= Father->CurrentToken;
  va_list args;
  va_start (args, pFormat);
  _log(ZAIES_Warning,pFormat,args);
  va_end(args);
}

*/







class CWeightList  WeightList ;



bool
matchWeight(CMatchResult& pResult,const utf8VaryingString& pIn,const utf8VaryingString& pToMatch,int pMatchMin,int pRadixMin)
{
    pResult.clear();
    int wRadCur=0;
    size_t wInLen=0 , wKwLen=0 ;
    utf8_t* wPtrIn=pIn.Data ;
    utf8_t* wPtrKw=pToMatch.Data ;

    /* get size of word to match */
    while (*wPtrKw) {
        wPtrKw++;
        wKwLen++;
    }

    /* first get match count */
    while (*wPtrIn) {
        wPtrKw=pToMatch.Data ;

        while (*wPtrIn && *wPtrKw) {
            if ( utfUpper(*wPtrIn) == utfUpper(*wPtrKw)) {
                pResult.Match++;
                break;
            }
            wPtrKw++;
        }// while
        wInLen++;
        wPtrIn++;
    }// while


    /* get consecutive count : radix
     * search consecutive sequences of segments of keyword in input string
     *
     *  for each character of input string
     *   search for possible substrings matches in keyword string
     *
    */
    utf8_t* wPtrIn1=wPtrIn=pIn.Data ;

    while (*wPtrIn) {
        wPtrKw=pToMatch.Data ;
        wPtrIn1 = wPtrIn;
        while (*wPtrIn1 && *wPtrKw) {
            if ( utfUpper(*wPtrIn1) == utfUpper(*wPtrKw)) {
                wRadCur++;
                if ( wRadCur > pResult.Radix )
                    pResult.Radix = wRadCur;
                wPtrKw++;
                wPtrIn1++;
                continue ;
            }
            wRadCur=0;
            wPtrIn1 = wPtrIn;
            wPtrKw++;
        }// while

        wInLen++;
        wPtrIn++;
    }// while
/*
    if ( wRadCur > pResult.Radix )
        pResult.Radix = wRadCur;
*/
    if (pMatchMin < 1) {
        if ( wInLen < 4 )
            pMatchMin = (wInLen - 1)>2?(wInLen - 1):wInLen;
        else
            pMatchMin = std::min(wInLen,wKwLen) * 40 / 100 ;
    }

    if ((pResult.Match < pMatchMin) || (pResult.Radix < pRadixMin))
        return false;

    return true ;
} // matchWeight

utf8VaryingString
searchAdHocWeighted_old(const utf8VaryingString& pIn, ZArray<utf8VaryingString>& pKeyArray)
{
    CWeightList WeightList ;

    if (pKeyArray.count()==0)
        return utf8VaryingString();

    utf8_t* wPtrIn=pIn.Data ;
    size_t wInLen=pIn.strlen();
    int wMin = wInLen - 3 ;
    if ( wInLen < 4 )
        wMin = (wInLen - 1)>2?(wInLen - 1):wInLen;
    int wMatch=0 ;
    int wi=0 ;
    for (; wi < pKeyArray.count() ; wi++) {
        wMatch=0;
        CWeight wWeight;
        wPtrIn=pIn.Data ;
        utf8_t* wPtrKw=pKeyArray[wi].Data ;
        while (*wPtrIn && *wPtrKw ) {
            if ( utfUpper(*wPtrIn) == utfUpper(*wPtrKw))
                wMatch++;
            wPtrIn++;
            wPtrKw++;
        }// while
        if (wMatch>0) {
            wWeight.Match = wMatch ;
            wWeight.Index = wi ;
            WeightList.add(wWeight);
        }
    }// for

    /* return the highest pounded if match is more than minimum */

    if (WeightList.count()>0)
        if ( WeightList.Tab(0).Match >= wMin )
            return pKeyArray[WeightList.Tab(0).Index] ;

    return utf8VaryingString();
}//searchAdHocWeighted_old

utf8VaryingString
searchAdHocWeighted(const utf8VaryingString& pIn, ZArray<utf8VaryingString>& pKeyArray)
{
    CWeightList WeightList ;

    if (pKeyArray.count()==0)
        return utf8VaryingString();

    utf8_t* wPtrIn=pIn.Data ;
    size_t wInLen=pIn.strlen();
    int wMinimum = wInLen - (wInLen * 30 / 100) ;
    if ( wInLen < 4 )
        wMinimum = (wInLen - 1)>2?(wInLen - 1):wInLen;



    int wMatch=0 ;
    int wi=0 ;
    for (; wi < pKeyArray.count() ; wi++) {
        CWeight wWeight;
        CMatchResult wMR;
        if(matchWeight(wMR,pIn,pKeyArray[wi],wMinimum)) {
            wWeight.Index = wi ;
            wWeight.Match = wMR.Match;
            wWeight.Radix = wMR.Radix;
            WeightList.add(wWeight);
        }
    }// for

    /* return the highest pounded if match is more than minimum */

    if (WeightList.count()>0)
            return pKeyArray[WeightList.Tab(0).Index] ;

    return utf8VaryingString();
}//searchAdHocWeighted



utf8VaryingString
searchSymbolWeighted(const utf8VaryingString& pIn)
{
    CWeightList WeightList ;

    utf8_t* wPtrIn=pIn.Data ;
    size_t wInLen=pIn.strlen();
    int wMin = wInLen - 3 ;
    if ( wInLen < 4 )
        wMin = (wInLen - 1)>2?(wInLen - 1):wInLen;
    int wMatch=0 ;
    int wi=0 ;
    for (; wi < GParser->SymbolList.count() ; wi++) {
        wMatch=0;
        CWeight wWeight;
        wPtrIn=pIn.Data ;
        utf8_t* wPtrKw=GParser->SymbolList[wi].FullFieldName.Data ;
        while (*wPtrIn && *wPtrKw ) {
            if ( utfUpper(*wPtrIn) == utfUpper(*wPtrKw))
                wMatch++;
            wPtrIn++;
            wPtrKw++;
        }// while
        if (wMatch>0) {
            wWeight.Match = wMatch ;
            wWeight.Index = wi ;
            WeightList.add(wWeight);
        }
    }// for

    /* return the highest pounded if match is more than minimum */

    if (WeightList.count()>0)
        if ( WeightList.Tab(0).Match >= wMin )
            return GParser->SymbolList[WeightList.Tab(0).Index].FullFieldName ;

    return utf8VaryingString();
}//searchSymbolWeighted


utf8VaryingString
searchKeywordWeighted(const utf8VaryingString& pIn)
{
    CWeightList WeightList ;
    ZArray<ZSearchKeyWord>* wKeyWordList=&KeywordList;// for debug
    WeightList.clear();

    utf8_t* wPtrIn=pIn.Data ;
    size_t wInLen=pIn.strlen();
    int wMin = wInLen - 3 ;
    if ( wInLen < 4 )
        wMin = (wInLen - 1)>2?(wInLen - 1):wInLen;
    int wMatch=0 ;
    int wi=0 ;
    for (; wi < KeywordList.count() ; wi++) {
        wMatch=0;
        CWeight wWeight;
        wPtrIn=pIn.Data ;
        utf8_t* wPtrKw=KeywordList[wi].Text.Data ;
        while (*wPtrIn && *wPtrKw ) {
            if ( utfUpper(*wPtrIn) == utfUpper(*wPtrKw))
                wMatch++;
            wPtrIn++;
            wPtrKw++;
        }// while
        if (wMatch>0) {
            wWeight.Match = wMatch ;
            wWeight.Index = wi ;
            WeightList.add(wWeight);
        }
    }// for

    /* return the highest pounded if match is more than minimum */

    if (WeightList.count()>0)
        if ( WeightList.Tab(0).Match >= wMin )
            return KeywordList[WeightList.Tab(0).Index].Text ;

    return utf8VaryingString();
}//searchKeywordWeighted

/** @brief setDisplayColMax sets maximum column width for default entity display routine */
void ZSearchParser::setDisplayLimit(int pLimit)
{
    if (pLimit < 0) {
        ErrorLog.infoLog("setDisplayLimit-I-NOLIMIT No limit set to default entity display routine.\n");
        DisplayLimit=pLimit;
        return;
    }
    DisplayLimit=pLimit;
    ErrorLog.infoLog("setDisplayLimit A limit of maximum %d rank is defined for default entity display.", DisplayLimit);
    return ;
}

/** @brief setDisplayColMax sets maximum column width for default entity display routine */
void ZSearchParser::setDisplayColMax(int pColMax)
{
    if (pColMax < DisplayColMin) {
        ErrorLog.errorLog("setDisplayColMax-E-INVVALUE Display column maximum value %d cannot be less than minimum %d.\n"
                          "                            Maximum stays at its former value %d.",
                          pColMax,DisplayColMin,DisplayColMax);
        return;
    }
    DisplayColMax=pColMax;
    ErrorLog.infoLog("setDisplayColMax Display column maximum value has been set successfully to %d.", DisplayColMax);
    return ;
}
/** @brief setDisplayColMin sets minimum column width for default entity display routine */
void ZSearchParser::setDisplayColMin(int pColMin)
{
    if (pColMin > DisplayColMax) {
        ErrorLog.errorLog("setDisplayColMin-E-INVVALUE Display column minimum value %d cannot be greater than maximum %d.\n"
                          "                            Minimum stays at its former value %d.",
                          pColMin,DisplayColMax,DisplayColMin);
        return;
    }
    if (pColMin < 3) {
        ErrorLog.errorLog("setDisplayColMin-E-INVVALUE Display column minimum value %d cannot be less than 3.\n"
                          "                            Maximum stays at its former value %d.",
                          pColMin,DisplayColMin);
        return;
    }
    DisplayColMin=pColMin;
    ErrorLog.infoLog("setDisplayColMax Display column minimum value has been set successfully to %d.", DisplayColMin);
    return ;
}


}//namespace zbs
