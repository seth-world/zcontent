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

using namespace zbs;

const char* ZSearchQueryHelp =
" help </b>"
" set file <path> [mode readonly,modify] as <entity name> ;  // by default mode is read only<br>"
    "                      <----------------selection phrase---------------> <br>";
/*
  help <br><br>
  set file <path> [mode readonly,modify] as <entity name> ;  // by default mode is read only <br>"
<br>
                                          <-----selection phrase---------><---alias------------>\n";
  find  [all,first]  <entity name> with   <entity name>.<field name> .... [as <collection name>] ;  NB: by default, find first is executed \n
\n
                      <--------------- assignment phrase ------------>
for <entity> with [all,first] <selection phrase > set [<entity name>.]<field name> = <value> , [<entity name>.]<field name> = <value> ... ;

for <collection name> set <fieldname>=<value> , <fieldname>=<value> ... ;

display {<collection name>.<field>, <collection name>.<field>,... | <collection name>.all | <collection name>.* } ;


-------- Dictionary display -----------------------------------------

  show entities;                                  -> list all entities and collections available
  show ZEntity;                                   -> list all ZEntity symbols (used for ZResource)
  show symbols ;                                  -> list all symbols available : a symbol is a shortcut to a filename
  show {<entity> | <collection name>} fields ;
  show {<entity> | <collection name>} <fieldname> ; // exact field name
  show {<entity> | <collection name>} sss* ; // field name starting with 'sss'
  show {<entity> | <collection name>} *sss ; // field name ending with 'sss'
  show {<entity> | <collection name>} *sss* ; // field name containing 'sss'



Instruction history

  set history maximum nn ;  -> set the maximum instruction lines to keep in history log. Default is 40

  show history ;  -> display all instruction lines in history log


  clear history ;       -> removes ALL instruction lines from the history. History is then empty.

  clear history first 5; -> removes the 5 FIRST instruction lines in the history or removes all the first lines until 3.

  clear history last 3;  -> removes the 3 LAST instruction lines in the history or removes all the last lines until 3.

  clear history at 3 ; -> removes the third instruction line in the history. If rank 3 does not exist in history, nothing is done.
                            NB: rank is counted from 1 (and not from 0)



  finish ;  // release resources flush and close open files

*/




namespace zbs {


class ZSearchParser* GParser=nullptr;

using namespace std;
ZSearchParser::ZSearchParser()
{
Tokenizer = new ZSearchTokenizer;
}
ZSearchParser::~ZSearchParser() {

  if (Tokenizer)
    delete Tokenizer;

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

  setAutoPrintOn(ZAIES_Text);

  ZStatus wSt=loadXmlSearchParserZEntity(pXmlParserZEntity);
  if ((wSt!=ZS_SUCCESS)&&(wSt!=ZS_EOF)) {
    warningLog("Cannot load parser ZEntity file <%s>, status is <%s>.No application entity will be defined.",
        pXmlParserZEntity.toCChar(),decode_ZStatus(wSt));
  }
  wSt=loadXmlSearchParserSymbols(pXmlParserSymbol);
  if ((wSt!=ZS_SUCCESS)&&(wSt!=ZS_EOF))  {
    warningLog("Cannot load parser symbol file <%s>, status is <%s>.No symbol files will be defined.",
        pXmlParserSymbol.toCChar(),decode_ZStatus(wSt));
  }

  wSt=History.setup(pHistory);
  if (wSt!=ZS_SUCCESS) {
    logZException();
    logZStatus(ZAIES_Error,wSt,"Cannot open history file <%s>.\n History will be disabled.",pHistory.toString());
  }

  displayAllLogged(stdout);
  clear();

  GParser = this;

  return ZS_SUCCESS;
}

ZStatus ZSearchParser::parse(const utf8VaryingString& pContent, std::shared_ptr<ZSearchEntity> &pCollection) {

  clear();

  if (pContent.isEmpty())
    return ZS_EMPTY;

  Tokenizer->parse(pContent);

  /* search for keywords */

  for (int wi=0; wi < Tokenizer->count();wi++) {
    if (Tokenizer->Tab(wi)->Type==ZSRCH_IDENTIFIER) {
      if (searchKeyword(Tokenizer->Tab(wi)))
        continue;
    }
  }// for

  if (Tokenizer->Options & ZSRCHO_Report)
    Tokenizer->report();
  bool wStoreInstruction=true;

  int wInstructionType = ZSITP_Nothing;

  Phrase.clear();

  ZStatus wSt = _parse(pCollection,wStoreInstruction,wInstructionType);
  if (wStoreInstruction)
    History.add(pContent);

  Phrase = pContent.subString(FirstToken->TokenOffset,CurrentToken->TokenOffset + 1 - FirstToken->TokenOffset);

  if ((wSt==ZS_SUCCESS)&&(wInstructionType==ZSITP_Find)) {
    textLog("Collection report \n %s\n",EntityList.last()->_report().toString());

   wSt= _executeFind(EntityList.last()->_CollectionEntity);
   if (wSt==ZS_OUTBOUNDHIGH)
     wSt=ZS_SUCCESS;
  }

//  InstructionLog.push(ZInstructionItem(pContent,wSt));
  return wSt;
}//ZSearchParser::parse



bool  ZSearchParser::_parseComment (ZSearchToken* &pToken) {
  switch (pToken->Type) {

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
}


bool ZSearchParser::testSemiColon()
{
  if (Index >= Tokenizer->count()) {
    if (Tokenizer->count()==0)
      errorLog("No valid token parsed.");
    else
      errorLog("Expected semi colon at the end of instruction line. Last token <%s> line %d column %d.",
          Tokenizer->last()->Text.toString(),
          Tokenizer->last()->TokenLine,Tokenizer->last()->TokenColumn
          );
    return false;
  }

  if (Tokenizer->Tab(Index)->Type != ZSRCH_SEMICOLON) {
    errorLog("Expected semi colon at the end of instruction line. Last token <%s> line %d column %d.",
        Tokenizer->Tab(Index)->Text.toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
    return false;
  }

  return true;
}


bool ZSearchParser::advanceIndex() {

  if (Tokenizer->count()==0) {
    errorLog("No instruction.");
    return false;
  }

  if (Index >= Tokenizer->count()) {
    errorLog("Syntax error : End of token while expecting more. Last token <%s> line %d column %d.",
        Tokenizer->last()->Text.toString(),
        Tokenizer->last()->TokenLine,Tokenizer->last()->TokenColumn
        );
    return false;
  }

  Index++;
  while (Index < Tokenizer->count()) {

    CurrentToken=Tokenizer->Tab(Index);

    if (Tokenizer->Tab(Index)->Type == ZSRCH_SPACE) {
      Index++;
      continue;
    }
    if (_parseComment(Tokenizer->Tab(Index))) {
      Index++;
      continue;
    }
    /*    if (!wRe)
      break;
    wRe=false;
*/
    break;
  }// while (pIdx < Tokenizer->count())
  return true;
} // ZSearchParser::advanceIndex

ZStatus
ZSearchParser::_parseShow()
{
  int wEntityIndex=0;
  enum ShowEnum {
    SHENUM_NOTHING = 0,
    SHENUM_ENTITY = 1,
    SHENUM_ENTITYFIELDS = 2,
    SHENUM_MASTER = 3,
    SHENUM_ZENTITY = 4,
    SHENUM_SYMBOL = 5,
    SHENUM_HISTORY = 6,
  } wShowEnum=SHENUM_NOTHING;

  ZStatus wSt=ZS_SUCCESS;
  while ((Index < Tokenizer->count())&&(Tokenizer->Tab(Index)->Type!=ZSRCH_SEMICOLON)) {
    CurrentToken=Tokenizer->Tab(Index);

    if (Tokenizer->Tab(Index)->Type==ZSRCH_HISTORY) {
      if (!advanceIndex()) {
        errorLog("Expected semi colon at the end of instruction line. Last token <%s> line %d column %d.",
            Tokenizer->last()->Text.toString(),
            Tokenizer->last()->TokenLine,Tokenizer->last()->TokenColumn
            );
      }
      if (Tokenizer->Tab(Index)->Type==ZSRCH_SEMICOLON) {
        showHistory();
        break;
      }
      if (Tokenizer->Tab(Index)->Type!=ZSRCH_MAXIMUM) {
        errorLog("Expected either semi colon or keyword <MAXIMUM>. Last token <%s> line %d column %d.",
            Tokenizer->last()->Text.toString(),
            Tokenizer->last()->TokenLine,Tokenizer->last()->TokenColumn
            );
        return ZS_SYNTAX_ERROR;
      }
      if (!testSemiColon())
        return ZS_MISS_PUNCTSIGN;

        showHistoryMaximum();
        return ZS_SUCCESS;
      }// ZSRCH_HISTORY

    if (Tokenizer->Tab(Index)->Type==ZSRCH_ENTITY) {
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;
      if (!testSemiColon())
        return ZS_MISS_PUNCTSIGN;
      showEntities();
      break;
    }
    if (Tokenizer->Tab(Index)->Type==ZSRCH_ZENTITY) {
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;
      if (!testSemiColon())
        return ZS_MISS_PUNCTSIGN;
      showZEntities();
      return ZS_SUCCESS;
    }

    if (Tokenizer->Tab(Index)->Type==ZSRCH_SYMBOL) {
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;
      if (!testSemiColon())
        return ZS_MISS_PUNCTSIGN;

      showSymbols();
      return ZS_SUCCESS;
    }

    if (Tokenizer->Tab(Index)->Type==ZSRCH_MASTERFILES) {
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;
      if (!testSemiColon())
        return ZS_MISS_PUNCTSIGN;

      showMasterFiles();
      return ZS_SUCCESS;
    }

    if (Tokenizer->Tab(Index)->Type!=ZSRCH_IDENTIFIER) {
      errorLog("Syntax error: Found <%s> while expecting one of {ENTITY,MASTERFILE,SYMBOL,ZENTITY} at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
      return ZS_SYNTAX_ERROR;
    }
    wEntityIndex=0;
    for (; wEntityIndex < EntityList.count(); wEntityIndex++) {
      if (EntityList[wEntityIndex]->getName() == Tokenizer->Tab(Index)->Text) {

        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;
        if (!testSemiColon()) {
          return ZS_MISS_PUNCTSIGN ;
        }

        showEntityFields(wEntityIndex);
        return ZS_SUCCESS;
      }
    }// for

    if (wEntityIndex == EntityList.count()) {
      errorLog("Syntax error: Unrecognized keyword <%s> while expecting one of {ENTITY,MASTERFILE,SYMBOL,ZENTITY,<valid entity name>} at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString() ,Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      return ZS_SYNTAX_ERROR;
    }

    break;

  } // main while
/*
  switch(wShowEnum)
  {
  case SHENUM_ENTITY:
    showEntities();
    break;
  case SHENUM_ENTITYFIELDS:
    showEntityFields(wEntityIndex);
    break;
  case SHENUM_MASTER:
    showMasterFiles();
    break;
  case SHENUM_ZENTITY:
    showZEntities();
    break;
  case SHENUM_SYMBOL:
    showSymbols();
    break;
  case SHENUM_HISTORY:
    showHistory();
    break;
  }
*/
  return ZS_SUCCESS;
} // ZSearchParser::_parseShow

ZStatus
ZSearchParser::_parseSetHistoryMaximum()
{

}



/* set file <path> [mode readonly,modify] as <entity name> ;  // by default mode is read only */

ZStatus
ZSearchParser::_parseSetFile(std::shared_ptr<ZSearchEntity> &pCollection)
{
  ZStatus wSt=ZS_SUCCESS;
  uriString wPath;
  Action = ZSPA_SetFile;
  ZSearchToken* wEntityToken=nullptr;

  while ((Index < Tokenizer->count())&&(Tokenizer->Tab(Index)->Type!=ZSRCH_SEMICOLON)) {
    CurrentToken=Tokenizer->Tab(Index);
    while (true) {
    if(Tokenizer->Tab(Index)->Type==ZSRCH_STRING_LITERAL)
      {
        wPath=Tokenizer->Tab(Index)->Text;
//        wPath.eliminateChar('"');
        wSt=wPath.check();
        if (wSt!=ZS_SUCCESS) {
          logZStatus(ZAIES_Error,wSt,"Path <%s> does not point to a valid, existing file.");
          errorLog("Invalid file path <%s> at line %d column %d.",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
           return ZS_FILEERROR ;
        }
        infoLog("Path <%s> is valid at line %d column %d.",
            Tokenizer->Tab(Index)->Text.toString(),
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;
        break;
      } // ZSRCH_STRING_LITERAL

      if(Tokenizer->Tab(Index)->Type==ZSRCH_IDENTIFIER)  /* pre-defined path  <symbol> <path> */
      {
        int wi=0;
        for (;wi < SymbolList.count();wi++) {
          if (SymbolList[wi].Symbol == Tokenizer->Tab(Index)->Text) {
            wSt=SymbolList[wi].Path.check();
            if (wSt!=ZS_SUCCESS) {
              errorLog("Invalid file path <%s> deduced from symbol <%s> at line %d column %d.",
                  SymbolList[wi].Path.toString(),Tokenizer->Tab(Index)->Text.toString(),Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
              return wSt ;
            }
            break;
          }
        }
        if (wi== SymbolList.count()) {
          errorLog("Invalid symbol identifier <%s> cannot find corresponding symbol at line %d column %d.",
              Tokenizer->Tab(Index)->Text.toString(),Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
          return ZS_NOTFOUND ;
        }
      wPath = SymbolList[wi].Path;
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;
      break;
      }// ZSRCH_IDENTIFIER

      errorLog("Syntax error: Expected either path or symbol identifier. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
      return ZS_SYNTAX_ERROR ;
      }// while true

      CurrentToken=Tokenizer->Tab(Index);

      if(Tokenizer->Tab(Index)->Type== ZSRCH_MODE) {
        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;
        if (Tokenizer->Tab(Index)->Type==ZSRCH_READONLY) {
          Action |= ZSPA_ReadOnly ;
        } else if (Tokenizer->Tab(Index)->Type==ZSRCH_MODIFY) {
          Action |= ZSPA_Modify ;
        } else {
          errorLog("Expecting mode as one of [READONLY,MODIFY] at line %d column %d. Found <%s>.",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
          return ZS_INVPARAMS ;
        }
        advanceIndex();
      } else {
        warningLog("Expecting MODE keyword at line %d column %d. Found <%s>. Mode is set to READONLY.",
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
          Action = ZSPA_ReadOnly ;
        }

      if (Tokenizer->Tab(Index)->Type != ZSRCH_AS) {
          errorLog("Expecting AS keyword at line %d column %d. Found <%s>. ",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
          return ZS_MISS_KEYWORD;
      }
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;

      if (Tokenizer->Tab(Index)->Type != ZSRCH_IDENTIFIER) {
          errorLog("Expecting entity identifier at line %d column %d. Found <%s>. ",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
          return ZS_MISS_KEYWORD;
        }
        wEntityToken=Tokenizer->Tab(Index) ;

        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;

        if (!testSemiColon()) {
          return ZS_MISS_PUNCTSIGN;
        }

  }//while not semi colon


  std::shared_ptr <ZSearchMasterFile> wMF=nullptr;
  int wi=0;
  for (;wi < MasterFileList.count();wi++){
    if (wPath==MasterFileList[wi]->_MasterFile.getURIContent()) {
      wMF=MasterFileList[wi]->getSharedPtr();
      if (Action & ZSPA_Modify) {
        if (!wMF->isOpenModify()) {
          warningLog("File <%s> is already is use with access mode <%s> cannot use mode <ZRF_Modify>.",
              wPath.toString(), decode_ZRFMode(wMF->getOpenMode()));
          return ZS_MODEINVALID;
        }
        break;
      }// ZSPA_Modify
      if (!wMF->isOpenAtLeastReadOnly()) {
        warningLog("File <%s> is already is use with access mode <%s> cannot use mode <ZRF_Modify>.",
            wPath.toString(), decode_ZRFMode(wMF->getOpenMode()));
        return ZS_MODEINVALID;
      }
      break;
    } // if (wPath==MasterFileList[wi]->MasterFile.getURIContent())
  } // for

  if (wi == MasterFileList.count())  {  /* not found in master file list : create one in list */
    wMF=std::make_shared <ZSearchMasterFile>();
    if (Action & ZSPA_Modify) {
      wSt=wMF->openModify(wPath);
    }
    else {
      wSt=wMF->openReadOnly(wPath);
    }
    if (wSt!=ZS_SUCCESS) {
      logZStatus(ZAIES_Error,wSt,"ZSearchParser::_parse-E-ERROPEN Cannot access file <%s> mode <%s>.",
          wPath.toString(),Action & ZSPA_Modify?"Modify":"Readonly");
      return wSt;
    }
    MasterFileList.push(wMF);
  }// not found


  /* search if an entity with same name already exists */

  if (EntityList.getEntityByName(wEntityToken->Text)!=nullptr) {
    textLog("An entity with name <%s> has already been registrated previously.",
        wEntityToken->Text.toString());
    return ZS_INVNAME;
  }

  Entity = ZSearchEntity::constructWithMaster(wMF,wEntityToken);

//  CurEntities.push(Entity);

  EntityList.push(Entity);

  textLog("File <%s> has been opened with mode <%s> as <%s> ",
      wPath.toString(),Action & ZSPA_Modify?"Modify":"Readonly",Entity->getName().toString());
  textLog("Dictionary name <%s> %ld fields %ld index(es)",
      Entity->getFieldDictionary().DicName.toString(),
      Entity->getFieldDictionary().count(),
      Entity->getKeyDictionary().count());
  for (long wi=0; wi < Entity->getKeyDictionary().count();wi++) {
    textLog("    %ld - index name <%s> ", wi, Entity->getKeyDictionary()[int(wi)]->DicKeyName.toString());
  }

  textLog("Entity <%s> has been created and registered.",
     Entity->getName().toString());

  return ZS_SUCCESS;
} //_parseSetFilePhrase

void
ZSearchParser::showHistory () {
  History.show(this);
}
void
ZSearchParser::showHistoryMaximum () {
  textLog(" History maximum is set to %d ",History.Maximum);
}

void
ZSearchParser::setHistoryMaximum (int pMax) {
  ZStatus wSt=History.setMaximum(pMax,this);
}
void
ZSearchParser::clearHistory () {
  History.List.clear();
  textLog("            ---instruction log is cleared--.");
}
void
ZSearchParser::clearHistoryFirst (int pLines) {

  History.removeFirst(pLines,this);
}
void
ZSearchParser::clearHistoryLast (int pLines) {
  History.removeLast(pLines,this);
/*
  if (History.List.count()<=pLines){
    History.List.clear();
    textLog("            ---instruction log is cleared---");
    return;
  }
  for (int wi=0;wi < pLines;wi++)
    History.List.pop();

  textLog("            --- removed %d lines from then end of instruction log ---",pLines,pLines);
*/
}
void
ZSearchParser::clearHistoryRank (int pRank) {
  History.remove(pRank,this);
}


void
ZSearchParser::showSymbols () {
  textLog("__________________Symbol list___________________");
  if (SymbolList.count()==0)
    textLog("            ---no symbol to show--.");
  else
    for (int wi=0;wi < SymbolList.count();wi++) {
      textLog("%25s %s",SymbolList[wi].Symbol.toString(),SymbolList[wi].Path.toString());
  }
  textLog("________________________________________________");
}
void
ZSearchParser::showZEntities () {
  textLog("__________________ZEntity list___________________");
  if (ZEntityList.count()==0)
    textLog("            ---no ZEntity to show--.");
  else
    for (int wi=0;wi < ZEntityList.count();wi++) {
      textLog("%25s 0x%10lX",ZEntityList[wi].Symbol.toString(),ZEntityList[wi].Value);
    }
  textLog("________________________________________________");
}

void
ZSearchParser::showMasterFiles () {
  textLog("__________________Master files___________________");
  if (MasterFileList.count()==0)
    textLog("           ---no Master file to show--.");
  else
    for (int wi=0;wi < MasterFileList.count();wi++) {
      textLog("%30s %s",
          decode_ZRFMode( MasterFileList[wi]->getOpenMode()), MasterFileList[wi]->getPath().toString());
    }
  textLog("________________________________________________");
}

void
ZSearchParser::showEntities () {
  textLog("__________________Entities___________________");
  if (EntityList.count()==0)
    textLog("           ---no Entity to show--.");
  else
    for (int wi=0;wi < EntityList.count();wi++) {
      if (!EntityList[wi]->isValid()) {
        textLog("<Invalid entity>");
        continue;
      }

      textLog("<%s>",
          EntityList[wi]->getName().toString());
      if (EntityList[wi]->isFile()) {
      textLog("       has associated file : open mode <%30s> %s",
            decode_ZRFMode( EntityList[wi]->_FileEntity->_MasterFile->getOpenMode()), EntityList[wi]->_FileEntity->_MasterFile->getPath().toString());
      continue;
      }
      if (!EntityList[wi]->isCollection()) {
        errorLog("       error : entity is neither a file entity nor a collection entity.",
            decode_ZRFMode( EntityList[wi]->_FileEntity->_MasterFile->getOpenMode()), EntityList[wi]->_FileEntity->_MasterFile->getPath().toString());
        continue;
      }
      const char* wBaseType = nullptr;
      if (EntityList[wi]->_CollectionEntity->_BaseEntity->isFile())
        wBaseType="File entity";
      else if (EntityList[wi]->_CollectionEntity->_BaseEntity->isCollection())
        wBaseType="Collection entity";
      else
        wBaseType="Undefined invalid entity";
      textLog("       is a collection entity  : base entity is <%30s> of type <%s>.",
          EntityList[wi]->_CollectionEntity->_BaseEntity->getName().toString(),wBaseType);
      if (EntityList[wi]->_CollectionEntity->isValid()) {
        textLog("___________________Logical term_______________________");
        if (EntityList[wi]->_CollectionEntity->LogicalTerm!=nullptr) {
          textLog("___________________Logical expression_______________________");
          textLog(EntityList[wi]->_CollectionEntity->LogicalTerm->_report(0).toCChar());
        }
        else
          textLog("***No logical expression***");

       textLog("___________________ Selected fields _______________________");

       for (long wj=0;wj < EntityList[wi]->_CollectionEntity->getFieldDictionary().count();wj++ ) {
         textLog(EntityList[wi]->_CollectionEntity->getFieldDictionary().TabConst(wj).getName().toCChar());
       }

      } // is valid
      else
        textLog("***Base entity is NULL***");
    }
  textLog("__________________________________________________________________");
}

void
ZSearchParser::showEntityFields (int pEntityIndex) {
  textLog("__________________Entity %s___________________",EntityList[pEntityIndex]->getName().toString());
  for (int wi=0; wi < EntityList[pEntityIndex]->getFieldDictionary().count();wi++) {
    textLog("%30s %s",
        EntityList[pEntityIndex]->getFieldDictionary()[wi].getName().toString(),
        decode_ZType(EntityList[pEntityIndex]->getFieldDictionary()[wi].ZType));
  }
  textLog("________________end entity %s_________________",EntityList[pEntityIndex]->getName().toString());
}

ZStatus
ZSearchParser::_finish()
{

/* release all created collections */
  return ZS_SUCCESS;
}

/* for <entity name> with < selection clause > set  <entity name>.<field name> = <value> , <entity name>.<field name> = <value> ; */

ZStatus
ZSearchParser::_parseFor(std::shared_ptr<ZSearchEntity> &pCollection)
{
  return ZS_SUCCESS;
}

#ifdef __COMMENT__

ZStatus
ZSearchParser::_parseLogicalOperand(void* & pOperand,int pParenthesisLevel, int pCollateral,int pBookMark)
{
  setIndex(pBookMark);
  ZStatus wSt=ZS_SUCCESS;
  ZSearchOperandType wOpType=ZSTO_Nothing;

  while (true) {
    if (Tokenizer->Tab(Index)->Type==ZSRCH_IDENTIFIER) {

      /* check if symbol or ZEntity */

      if (_parseZEntity(pOperand) ) {
        break;
      }
      if (_parseSymbol(pOperand) ) {
        break;
      }

      wSt=_parseOperandField(pOperand);
      if (wSt!=ZS_SUCCESS) {
        return wSt;
      }
      wOpType = ZSTO_Field;
      break;
    } // ZSRCH_IDENTIFIER

    if ((Tokenizer->Tab(Index)->Type & ZSRCH_LITERAL)==ZSRCH_LITERAL) {
      wSt=_parseLiteral(pOperand);
      if (wSt!=ZS_SUCCESS) {
        return wSt;
      }
      wOpType = ZSTO_Literal;
      break;
    } // ZSRCH_LITERAL
  } // while true


  /*
    if operator following operand is arithmetic, then the whole operand becomes an arithmetic formula that gives a literal result.
    - Nature of the operand changes : it becomes ZSearchArithmeticTerm in place of ZSearchFieldOperand or ZSearchLiteralOperand
    - operand parsed so far becomes the first operand of the arithmetic expression
    - Expression type is initialized with first operand type :
        Expression type has two states : literal or field.
        Purpose :
          in an arithmetic expression,
          if all operands are literal, then it is evaluated once, then expression result is used as literal
          if there are fields involved in there, it is evaluated at each record

  */

  if (ZSearchOperator::isArithmeric(Tokenizer->Tab(Index))) {
    return _parseArithmetic(pOperand,0,0,pBookMark);
  }
  return ZS_SUCCESS;
}
#endif // __COMMENT__


/* extracts field identifier either under the form of
 *
   <field>
   <field>.<modifier>

  <entity>.<field>

  <entity>.<field>.<modifier>


  <modifer> itself and its possible arguments will be extracted by callee routine (i. e. _parseOperandField())


*/

ZStatus
ZSearchParser::_parseFieldIdentifier (int & pEntityListIndex, ZSearchFieldOperandOwnData *pFOD)
{
 ZStatus wSt=ZS_SUCCESS;
 utf8VaryingString wEntityName;

 int wCurEntityIndex=-1;

 if (Tokenizer->Tab(Index)->Type != ZSRCH_IDENTIFIER) {
   errorLog("Missing field or entity identifier. Found <%s> at line %d column %d.",
       Tokenizer->Tab(Index)->Text.toString(),
       Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );

   return ZS_MISS_FIELD;
 }

 /* if dot, then previous is ZEntity name as literal  or Field and current is modifier */
 if (Tokenizer->Tab(Index+1)->Type == ZSRCH_DOT) {
   if ((Tokenizer->Tab(Index+2)->Type & ZSRCH_MODIFIER)==ZSRCH_MODIFIER) {
     /* there no entity : only one current entity must be selected */
     if (CurEntities.count()>1) {
       errorLog("Missing entity identifier : when multiple entities are selected then fields must be prefixed by entity name. Found <%s> at line %d column %d.",
           Tokenizer->Tab(Index+2)->Text.toString(),
           Tokenizer->Tab(Index+2)->TokenLine,Tokenizer->Tab(Index+2)->TokenColumn );
       return ZS_MISS_FIELD;
     }
     /* here this is a valid field description : search it in current entity */
     pFOD->MDicRank=0;
     for (;pFOD->MDicRank < CurEntities[0]->getFieldDictionary().count();pFOD->MDicRank++) {
       if (CurEntities[0]->getFieldDictionary().TabConst(pFOD->MDicRank).getName()==Tokenizer->Tab(Index)->Text)
         break;
     }
     if (pFOD->MDicRank==CurEntities[0]->getFieldDictionary().count()) {
       errorLog("Wrong field name <%s> for single entity <%s> (field not found in dictionary) at line %d column %d.",
           Tokenizer->Tab(Index+2)->Text.toString(),
           CurEntities[0]->getName().toString(),
           Tokenizer->Tab(Index+2)->TokenLine,Tokenizer->Tab(Index+2)->TokenColumn );
       pFOD->MDicRank=-1;
       return ZS_MISS_FIELD;
     }

     pFOD->MDic = CurEntities[0]->getFieldDictionaryPtr();

     pFOD->FieldDescription = CurEntities[0]->getFieldDictionary()[pFOD->MDicRank];
     pFOD->FullFieldName = CurEntities[0]->getName();
     pFOD->FullFieldName.addUtfUnit('.');
     pFOD->FullFieldName += Tokenizer->Tab(Index)->Text;
//     pFOD->FullFieldName.addUtfUnit('.');
//     pFOD->TokenList.push(Tokenizer->Tab(Index+2)); /* modifier name */


//     pFOD->TokenList.push(Tokenizer->Tab(Index)); /* store field identifier token */
     if (!advanceIndex())/* skip field identifier */
       return ZS_SYNTAX_ERROR;
//     pFOD->TokenList.push(Tokenizer->Tab(Index)); /* dot token */
//     if (!advanceIndex())/* skip dot */
//       return ZS_SYNTAX_ERROR;

//     pFOD->TokenList.push(Tokenizer->Tab(Index)); /* modifier token */
//     if (!advanceIndex())/* skip modifier */
//       return ZS_SYNTAX_ERROR;

     return ZS_SUCCESS;
   } // ZSRCH_MODIFIER

   if (Tokenizer->Tab(Index+2)->Type != ZSRCH_IDENTIFIER) {
     errorLog("Missing field identifier. Found <%s> at line %d column %d.",
         Tokenizer->Tab(Index+2)->Text.toString(),
         Tokenizer->Tab(Index+2)->TokenLine,Tokenizer->Tab(Index+2)->TokenColumn );

     return ZS_MISS_FIELD;
   }
   /* check entity identifier is a valid current entity name */
   wCurEntityIndex=0;
   for (; wCurEntityIndex < CurEntities.count();wCurEntityIndex++) {
     if (CurEntities[wCurEntityIndex]->getName() == Tokenizer->Tab(Index)->Text)
       break;
   }
   if (wCurEntityIndex==CurEntities.count()) {
     errorLog("Identifier<%s> is not a current entity. at line %d column %d.",
         Tokenizer->Tab(Index)->Text.toString(),
         Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index+2)->TokenColumn );

     return ZS_INV_ENTITY;
   }


//   pFOD->TokenList.push(Tokenizer->Tab(Index));
   wEntityName = Tokenizer->Tab(Index)->Text;

   if (!advanceIndex())/* entity identifier */
     return ZS_SYNTAX_ERROR;

//   pFOD->TokenList.push(Tokenizer->Tab(Index));
   if (!advanceIndex())/* dot sign */
     return ZS_SYNTAX_ERROR;
//   pFOD->TokenList.push(Tokenizer->Tab(Index)); /* store field name token */

/*  must be positionned to field name token
 *   if (!advanceIndex())
     return ZS_SYNTAX_ERROR;
*/
 } // dot found ZSRCH_DOT
 else {
 /* if not dot, then CurEntities must have only one entity */
 if (CurEntities.count() > 1) {
     errorLog("Field identifier <%s> must be prefixed with its entity name (multiple entities selected) at line %d column %d.",
         Tokenizer->Tab(Index)->Text.toString(),
         Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
     return ZS_INV_ENTITY;
   }
   wCurEntityIndex=0; /* get first and only current entity */

//   pFOD->TokenList.push(Tokenizer->Tab(Index)); /* store field name token */
   /*  must be positionned to field name token
 *   if (!advanceIndex())
     return ZS_SYNTAX_ERROR;
*/

  } // dot not found

//_parseFieldIdentifier_GetField:

  pFOD->MDicRank=0;
  for (; pFOD->MDicRank < CurEntities[wCurEntityIndex]->getFieldDictionary().count() ; pFOD->MDicRank++)
    if (CurEntities[wCurEntityIndex]->getFieldDictionary()[pFOD->MDicRank].getName()==Tokenizer->Tab(Index)->Text)
      break;

  if (pFOD->MDicRank==CurEntities[wCurEntityIndex]->getFieldDictionary().count()) {
    errorLog("Field identifier <%s> is not found (invalid) for entity <%s> at line %d column %d.",
        Tokenizer->Tab(Index)->Text.toString(),
        CurEntities[wCurEntityIndex]->getName().toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
    return ZS_INV_FIELD;
  }

  pFOD->MDic = CurEntities[wCurEntityIndex]->getFieldDictionaryPtr();

  pFOD->FieldDescription = CurEntities[wCurEntityIndex]->getFieldDictionary()[pFOD->MDicRank];
  pFOD->FullFieldName = CurEntities[wCurEntityIndex]->getName();
  pFOD->FullFieldName.addUtfUnit('.');
  pFOD->FullFieldName += Tokenizer->Tab(Index)->Text;


  return ZS_SUCCESS;
} //_parseFieldIdentifier


/**
 * @brief ZSearchParser::_parseModifier   index must point after ZSRC_DOT token directly to Modifier token
 */
ZStatus
ZSearchParser::_parseModifier(ZSearchOperandBase*   pOB)
{
  if ((Tokenizer->Tab(Index)->Type & ZSRCH_MODIFIER)!=ZSRCH_MODIFIER) {
      errorLog("Expecting modifier. Possibly there is a mismatch with entity,field name,modifier. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
      return ZS_SYNTAX_ERROR;
  }

  ZSearchOperandType_type wType = pOB->Type & ZSTO_BaseMask;

  switch (Tokenizer->Tab(Index)->Type ) {
  case ZSRCH_SUBSTRING : {
    if ((wType != ZSTO_String)&&(wType != ZSTO_UriString)) {
      errorLog("Modifier SUBSTRING is only possible with a string or an URI string. Possibly there is a mismatch with entity.field name.modifier. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
      return ZS_INV_MODIFIER;
    }
    pOB->ModifierType = Tokenizer->Tab(Index)->Type;
    if (!advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
    if(Tokenizer->Tab(Index)->Type!=ZSRCH_OPENPARENTHESIS) {
      errorLog("Wrong modifier syntax. Expecting open parenthesis found <%s> at line %d column %d ",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      return ZS_MISS_PUNCTSIGN ;
    }
    if (!advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
    if(Tokenizer->Tab(Index)->Type!=ZSRCH_NUMERIC_LITERAL) {
      errorLog("Missing numeric literal. Found <%s> at line %d column %d",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      return ZS_MISS_LITERAL ;
    }
    pOB->ModVal1 = Tokenizer->Tab(Index)->Text.toLong();
    if (!advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
    if(Tokenizer->Tab(Index)->Type!=ZSRCH_COMMA) {
      errorLog("Wrong modifier syntax. Expecting comma, found <%s> at line %d column %d",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      return ZS_MISS_PUNCTSIGN ;
    }
    if (!advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
    if(Tokenizer->Tab(Index)->Type!=ZSRCH_NUMERIC_LITERAL) {
      errorLog("Missing numeric literal. Found <%s> at line %d column %d ",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      return ZS_MISS_LITERAL ;
    }
    pOB->ModVal2 = Tokenizer->Tab(Index)->Text.toLong();
    if(Tokenizer->Tab(Index)->Type!=ZSRCH_CLOSEPARENTHESIS) {
      errorLog("Wrong modifier syntax. Expecting close parenthesis found <%s> at line %d column %d ",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      return ZS_MISS_PUNCTSIGN ;
    }
    /*
    if (!advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
*/
    return ZS_SUCCESS;
  }
  case ZSRCH_PATH:
  case ZSRCH_EXTENSION:
  case ZSRCH_BASENAME:
  case ZSRCH_ROOTNAME: {
    if (wType != ZSTO_UriString) {
      errorLog("Modifier <%s> is only possible with an URI string.at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
      return ZS_INV_MODIFIER;
    }

    pOB->ModifierType = Tokenizer->Tab(Index)->Type;
    /*
    if (!advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
*/
    return ZS_SUCCESS;
  }
  case ZSRCH_YEAR:
  case ZSRCH_MONTH:
  case ZSRCH_DAY:
  case ZSRCH_HOUR:
  case ZSRCH_MIN:
  case ZSRCH_SEC: {
    if (wType != ZSTO_Date) {
      errorLog("Modifier <%s> is only possible with an Date.at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
      return ZS_INV_MODIFIER;
    }

    pOB->ModifierType = Tokenizer->Tab(Index)->Type;
/*
    if (!advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
*/
    return ZS_SUCCESS;
  }
  case ZSRCH_ZENTITY:
  case ZSRCH_ID:  {
      if (wType != ZSTO_Resource) {
        errorLog("Modifier <%s> is only possible with an Resource.at line %d column %d.",
            Tokenizer->Tab(Index)->Text.toString(),
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
        return ZS_INV_MODIFIER;
      }
      pOB->ModifierType = Tokenizer->Tab(Index)->Type;
      /*
      if (!advanceIndex()) {
        return ZS_SYNTAX_ERROR;
      }
*/
      return ZS_SUCCESS;
  }
  default:
      errorLog("Invalid modifier <%s> for field at line %d column %d ",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      return ZS_INV_MODIFIER ;
  }// switch Type
}// _parseModifier

ZStatus
ZSearchParser::_parseOperandField(void* &    pTermOperand,ZSearchOperandType& pRequestedType)
{
  ZStatus wSt=ZS_SUCCESS;
  int wSvIndex=Index;
  ZSearchFieldOperandOwnData  wFOD;

  int wEntityListIndex=0;

  pTermOperand=nullptr;

  wSt=_parseFieldIdentifier(wEntityListIndex,&wFOD);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  /* set up term */
  switch(wFOD.FieldDescription.ZType) {
  case ZType_S8:
  case ZType_U8:
  case ZType_Char:
  case ZType_UChar:
  case ZType_S16:
  case ZType_U16:
  case ZType_S32:
  case ZType_U32:
  case ZType_S64:
  case ZType_U64: {
    wFOD.Type = ZSTO_FieldInteger;
    ZSearchFieldOperand<long>* wF1=new ZSearchFieldOperand<long>();

    wF1->setOwnData(wFOD);

    /* no modifiers for numeric field */

    if (Tokenizer->Tab(Index)->Type == ZSRCH_DOT) {
      errorLog("No modifier allowed to integer data. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index+2)->TokenColumn );
      return ZS_SYNTAX_ERROR;
    }


    if (!OperandTypeCheck(pRequestedType,wF1->getOperandBase(), wSvIndex)) {
      delete wF1;
      return ZS_INVTYPE ;
    }
    if (!advanceIndex()) {
      delete wF1;
      return ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return ZS_SUCCESS;

  }// ZSTO_FieldInteger

  case ZType_Float:
  case ZType_Double:
  case ZType_LDouble: {
    wFOD.Type = ZSTO_FieldFloat;
    ZSearchFieldOperand<double>* wF1=new ZSearchFieldOperand<double>();
    wF1->setOwnData(wFOD);

    /* no modifiers for numeric field */
    if (Tokenizer->Tab(Index)->Type == ZSRCH_DOT) {
      errorLog("No modifier allowed to floating point data. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wF1->getOperandBase(), wSvIndex)) {
      delete wF1;
      return ZS_INVTYPE ;
    }

    if (!advanceIndex()) {
      delete wF1;
      return ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return ZS_SUCCESS;
  } //  ZSTO_FieldInteger


  case ZType_Utf8VaryingString:
  case ZType_Utf16VaryingString:
  case ZType_Utf32VaryingString:
  case ZType_Utf8FixedString:
  case ZType_Utf16FixedString:
  case ZType_Utf32FixedString: {
    wFOD.Type = ZSTO_FieldString;
    ZSearchFieldOperand<utf8VaryingString>* wF1=new ZSearchFieldOperand<utf8VaryingString>();
    wF1->setOwnData(wFOD);

    if (Tokenizer->Tab(Index)->Type == ZSRCH_DOT) {
      if (!advanceIndex()) {  /* position to modifier token */
        delete wF1;
        return ZS_SYNTAX_ERROR;
      }

      if ((wSt=_parseModifier(wF1->getOperandBase()))!=ZS_SUCCESS) {
        delete wF1;
        return wSt;
      }

    } // ZSRCH_DOT

    if (!OperandTypeCheck(pRequestedType,wF1->getOperandBase(), wSvIndex)) {
      delete wF1;
      return ZS_INVTYPE ;
    }
    if (!advanceIndex()) {
      delete wF1;
      return ZS_SYNTAX_ERROR;
    }
    pTermOperand = wF1;
    return ZS_SUCCESS;
  } // case strings

  case ZType_URIString: {
    wFOD.Type = ZSTO_FieldUriString;
    ZSearchFieldOperand<utf8VaryingString>* wF1=new ZSearchFieldOperand<utf8VaryingString>();
    wF1->setOwnData(wFOD);

    if (Tokenizer->Tab(Index)->Type == ZSRCH_DOT) {
      if (!advanceIndex()) {  /* position to modifier token */
        delete wF1;
        return ZS_SYNTAX_ERROR;
      }

      if ((wSt=_parseModifier(wF1->getOperandBase()))!=ZS_SUCCESS) {
        delete wF1;
        return wSt;
      }

    } // ZSRCH_DOT


    if (!OperandTypeCheck(pRequestedType,wF1->getOperandBase(), wSvIndex)) {
      delete wF1;
      pTermOperand = nullptr;
      return ZS_INVTYPE ;
    }
    if (!advanceIndex()) {
      delete wF1;
      return ZS_SYNTAX_ERROR;
    }
    pTermOperand = wF1;
    return ZS_SUCCESS;
  } // ZType_URIString

  case ZType_ZDateFull: {
    wFOD.Type = ZSTO_FieldDate;
    ZSearchFieldOperand<ZDateFull>* wF1=new ZSearchFieldOperand<ZDateFull>();
    wF1->setOwnData(wFOD);

    if (Tokenizer->Tab(Index)->Type == ZSRCH_DOT) {
      if (!advanceIndex()) {  /* position to modifier token */
        delete wF1;
        return ZS_SYNTAX_ERROR;
      }

      if ((wSt=_parseModifier(wF1->getOperandBase()))!=ZS_SUCCESS) {
        delete wF1;
        return wSt;
      }

    } // ZSRCH_DOT

    if (!OperandTypeCheck(pRequestedType,wF1->getOperandBase(), wSvIndex)) {
      delete wF1;
      pTermOperand = nullptr;
      return ZS_INVTYPE ;
    }

    if (!advanceIndex()) {
      delete wF1;
      return ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return ZS_SUCCESS;
  } // ZType_ZDateFull

  case ZType_CheckSum: {
    ZSearchFieldOperand<checkSum>* wF1=new ZSearchFieldOperand<checkSum>();
    wF1->setOwnData(wFOD);


    if (!OperandTypeCheck(pRequestedType,wF1->getOperandBase(), wSvIndex)) {
      delete wF1;
      pTermOperand = nullptr;
      return ZS_INVTYPE ;
    }

    if (!advanceIndex()) {
      delete wF1;
      return ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return ZS_SUCCESS;
  }
  case ZType_Resource: {
    wFOD.Type = ZSTO_FieldResource;
    ZSearchFieldOperand<ZResource>* wF1=new ZSearchFieldOperand<ZResource>();
    wF1->setOwnData(wFOD);

    if (Tokenizer->Tab(Index)->Type == ZSRCH_DOT) {
      if (!advanceIndex()) {  /* position to modifier token */
        delete wF1;
        return ZS_SYNTAX_ERROR;
      }

      if ((wSt=_parseModifier(wF1->getOperandBase()))!=ZS_SUCCESS) {
        delete wF1;
        return wSt;
      }

    } // ZSRCH_DOT

    if (!OperandTypeCheck(pRequestedType,wF1->getOperandBase(), wSvIndex)) {
      delete wF1;
      return ZS_INVTYPE ;
    }

    if (!advanceIndex()) {
      delete wF1;
      return ZS_SYNTAX_ERROR;
    }

    pTermOperand = wF1;
    return ZS_SUCCESS;
  } // ZType_Resource

  default:
    errorLog("Dictionary field type <%s> for field <%s>.<%s> is not supported at line %d column %d in entity dictionary <%s>.",
        decode_ZType(wFOD.FieldDescription.ZType),
        Tokenizer->Tab(Index-2)->Text.toString(),Tokenizer->Tab(Index)->Text.toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn, Entity->getFieldDictionary().DicName.toString());
    return ZS_INVTYPE ;
  }// switch

  return ZS_SUCCESS;
} //_parseOperandField


bool
ZSearchParser::_parseEntity (void* &    pOperand, ZSearchOperandType &pRequestedType)
{
  if (Tokenizer->Tab(Index)->Type!=ZSRCH_IDENTIFIER)
    return false;

  int wSvIndex=Index;
  pOperand = nullptr;

  /* check if ZEntity */

  for (int wi=0 ; wi < CurEntities.count();wi++) {
    if (Tokenizer->Tab(Index)->Text.compare(CurEntities[wi]->getName())==0) {
      ZSearchOperandBase            wOB;
      wOB.Type = ZSTO_LiteralInteger;
      ZSearchLiteral<long> *wLit = new ZSearchLiteral<long>(wOB) ;
      wLit->Content = ZEntityList[wi].Value;
      wLit->Comment = ZEntityList[wi].Symbol;

      if (!advanceIndex()) {
        delete wLit;
        return false;
      }

      if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
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
ZSearchParser::_parseZEntity (void* &    pOperand, ZSearchOperandType &pRequestedType)
{
  if (Tokenizer->Tab(Index)->Type!=ZSRCH_IDENTIFIER)
    return false;

  pOperand = nullptr;
  /* check if ZEntity */

  for (int wi=0 ; wi < ZEntityList.count();wi++) {
    if (Tokenizer->Tab(Index)->Text.compareCase(ZEntityList[wi].Symbol)==0) {
      ZSearchOperandBase            wOB;
//      wOB.TokenList.push(Tokenizer->Tab(Index));
      wOB.Type = ZSTO_LiteralInteger;
      ZSearchLiteral<long> *wLit = new ZSearchLiteral<long> (wOB);
      wLit->Content = ZEntityList[wi].Value;
      wLit->Comment = ZEntityList[wi].Symbol;

      if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), Index)) {
        delete wLit;
        return false ;
      }

      if (!advanceIndex()) {
        delete wLit;
        return false;
      }
       pOperand = wLit;
      return true;
    }
  }
  return false;
} //_parseZEntity

bool
ZSearchParser::_parseSymbol (void* &    pOperand, ZSearchOperandType &pRequestedType)
{
  if (Tokenizer->Tab(Index)->Type!=ZSRCH_IDENTIFIER)
    return false;

  /* check if symbol */

  for (int wi=0 ; wi < SymbolList.count();wi++) {
    if (Tokenizer->Tab(Index)->Text.compareCase(SymbolList[wi].Symbol)==0) {
      ZSearchOperandBase            wOB;
 //     wOB.TokenList.push(Tokenizer->Tab(Index));
      wOB.Type = ZSTO_LiteralUriString;
      ZSearchLiteral<uriString> *wLit = new ZSearchLiteral<uriString> (wOB);
      wLit->Content = SymbolList[wi].Path;
      wLit->Comment = SymbolList[wi].Symbol;

      if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), Index)) {
        delete wLit;
        return false ;
      }

      if (!advanceIndex()) {
        delete wLit;
        pOperand = nullptr;
        return false;
      }

      pOperand = wLit;

      return true;
    }
  }
  return false;
} //_parseSymbol


ZStatus
ZSearchParser::_parseLiteral(void* &    pOperand, ZSearchOperandType &pRequestedType)
{
  ZStatus wSt;

  pOperand=nullptr;
  int wSvIndex=Index;

  switch (Tokenizer->Tab(Index)->Type ) {

  case ZSRCH_STRING_LITERAL: {
    ZSearchLiteral<utf8VaryingString> *wLit=new ZSearchLiteral<utf8VaryingString>;

    wLit->Type = ZSTO_LiteralString;
//    wLit->TokenList.push(Tokenizer->Tab(Index));

    wLit->Content=Tokenizer->Tab(Index)->Text;

/* Eliminate leading and trailing quotes if any */

    utf8_t * wContent = Tokenizer->Tab(Index)->Text.duplicate();
    utf8_t * wPtr = wContent;
    utf8_t * wHardEnd = wContent+Tokenizer->Tab(Index)->Text.UnitCount;

    if (wPtr[0]=='"')
      wPtr++;

    utf8_t * wEnd= wContent;
    while ((*wEnd != 0) && (wEnd < wHardEnd))
      wEnd++;

    wEnd--;
    if (wEnd[0]=='"')
      wEnd--;

    *wEnd = 0;

    wLit->Content=wPtr;

    zfree(wContent);

    /* done */

    if (!advanceIndex()) {
      pOperand = nullptr;
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
      delete wLit;
      return ZS_INVTYPE ;
    }

    pOperand = wLit;

    return ZS_SUCCESS;
  } //ZSRCH_STRING_LITERAL


  case ZSRCH_HEXA_LITERAL: {
/*
    if (Tokenizer->Tab(Index)->Text.UnitCount >= cst_checksumHexa) { // possible checksum litteral

    }
    else
        if (Tokenizer->Tab(Index)->Text.UnitCount >= cst_md5Hexa) { // possible md5 litteral

    }
*/
    ZSearchLiteral<long> *wLit=new ZSearchLiteral<long>;
    wLit->Type = ZSTO_LiteralInteger;
//    wLit->TokenList.push(Tokenizer->Tab(Index));

    /* getting hexa integer */

    wLit->Content =  0;
    long wSign = 1;
    long w16 = 0;

    utf8_t* wPtr=Tokenizer->Tab(Index)->Text.Data;

    if (*wPtr=='-') /* leading sign */
      wSign=-1;

    if ((wPtr[0]=='0')&&((wPtr[1]=='x')||(wPtr[1]=='X')))
        wPtr += 2;


    while (*wPtr != 0) {
      if ((*wPtr>='0')&&((*wPtr<='9')))
        wLit->Content += long(*wPtr - '0') * (w16==0?1:w16) ;

      else {
          if ((*wPtr>='A')&&((*wPtr<='F')))
            wLit->Content += long(*wPtr - 'A' + 10)* (w16==0?1:w16);
        else
         if ((*wPtr>='a')&&((*wPtr<='f')))
          wLit->Content += long(*wPtr - 'a' + 10)* (w16==0?1:w16);
      }
      if (*wPtr=='-') { /* trailing sign */
          wSign=-1;
          break;
      }
      wPtr++;
      w16++;
    }
  /* got it */

    wLit->Content *= wSign ;


    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
      delete wLit;
      return ZS_INVTYPE ;
    }

    pOperand = wLit ;

    return ZS_SUCCESS;
  } //ZSRCH_HEXA_LITERAL

  case ZSRCH_NUMERIC_LITERAL:
  case ZSRCH_INTEGER_LITERAL:
  case ZSRCH_INTEGER_LONG_LITERAL:
  case ZSRCH_INTEGER_ULONG_LITERAL: {

    ZSearchLiteral<long> *wLit=new ZSearchLiteral<long>;

    wLit->Type = ZSTO_LiteralInteger;
//    wLit->TokenList.push(Tokenizer->Tab(Index));

    wLit->Content =  0;
    long wSign = 1;
    long w10 = 0;

    utf8_t* wPtr=Tokenizer->Tab(Index)->Text.Data;
    if (wPtr[0]=='-') { /* leading sign */
      wSign = -1;
      wPtr ++;
    }


    while (*wPtr != 0) {
      if ((*wPtr>='0')&&((*wPtr<='9'))) {
        wLit->Content += long(*wPtr - '0') * (w10==0?1:w10) ;
        w10 += 10;
      }
      if (*wPtr=='-') { /* trailing sign */
        wSign = -1;
        break;
      }
      wPtr++;
    }

    wLit->Content *= wSign ;


    if (!advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
      delete wLit;
      return ZS_INVTYPE ;
    }

    pOperand = wLit ;
    return ZS_SUCCESS;
  } // long integer unsigned long;

  case ZSRCH_FLOAT_LITERAL:
  case ZSRCH_DOUBLE_LITERAL: {

    ZSearchLiteral<double> *wLit=new ZSearchLiteral<double>;
    wLit->Type = ZSTO_LiteralFloat;
//    wLit->TokenList.push(Tokenizer->Tab(Index));

    wLit->Content = Tokenizer->Tab(Index)->Text.toDouble();

    if (!advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
      delete wLit;
      return ZS_INVTYPE ;
    }

    pOperand = wLit;

    return ZS_SUCCESS;
  } // long integer unsigned long;

  case ZSRCH_DATE_LITERAL: {

    ZSearchLiteral<ZDateFull> *wLit=new ZSearchLiteral<ZDateFull>;
    wLit->Type = ZSTO_LiteralDate;
//    wLit->TokenList.push(Tokenizer->Tab(Index));

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
    wSt=ZDateFull::checkDMY(Tokenizer->Tab(Index)->Text,wTm,pFed);
    if (wSt!=ZS_SUCCESS) {
      wSt=ZDateFull::checkMDY(Tokenizer->Tab(Index)->Text,wTm,pFed);
      if (wSt!=ZS_SUCCESS) {
        errorLog("Invalid literal date <%s> at line %d column %d.",
            Tokenizer->Tab(Index)->Text.toString(),
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
        delete wLit;
        pOperand = nullptr;
        return ZS_INV_LITERAL;
      }
    }
    ZDateFull wD;
    wD._toInternal(wTm);
    wLit->Content = wD;


    if (!advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
      delete wLit;
      return ZS_INVTYPE ;
    }

    pOperand = wLit;
    return ZS_SUCCESS;
  } // ZSRCH_DATE_LITERAL

  case ZSRCH_ZDATE_LITERAL: {
    ZSearchLiteral<ZDateFull> *wLit=new ZSearchLiteral<ZDateFull>;
    wLit->Type = ZSTO_LiteralDate;
//    wLit->TokenList.push(Tokenizer->Tab(Index));
    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }
    if(Tokenizer->Tab(Index)->Type!=ZSRCH_OPENPARENTHESIS) {
      errorLog("ZDate literal : wrong syntax. Expecting open parenthesis. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      return ZS_MISS_PUNCTSIGN ;
    }
    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }
    int SvIndex=Index;
    utf8VaryingString wDLContent;

/*    [d]d{/|-}[m]m{/|-}[yy]yy[-hh:mm:ss] */

    while (Tokenizer->Tab(Index)->Type!=ZSRCH_CLOSEPARENTHESIS) {
      if ((Tokenizer->Tab(Index)->Type!=ZSRCH_NUMERIC_LITERAL)
          && (Tokenizer->Tab(Index)->Type!=ZSRCH_OPERATOR_DIVIDEORSLASH)
          && (Tokenizer->Tab(Index)->Type!=ZSRCH_OPERATOR_MINUS)
          && (Tokenizer->Tab(Index)->Type!=ZSRCH_COLON))
      {
        errorLog("ZDate literal : invalid date literal value. Expecting date literal with format [d]d{/|-}[m]m{/|-}[yy]yy[-hh[:mm:[ss]]]. Found <%s> at line %d column %d.",
            Tokenizer->Tab(Index)->Text.toString(),
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
        delete wLit;
        pOperand=nullptr;
        return ZS_SYNTAX_ERROR;
      }

      wDLContent += Tokenizer->Tab(Index)->Text;
      if (!advanceIndex()) {
        delete wLit;
        pOperand=nullptr;
        return ZS_SYNTAX_ERROR;
      }
    }// not ZSRCH_CLOSEPARENTHESIS

    uint8_t pFed=0;
    struct tm wTm;
    wSt=ZDateFull::checkDMY(wDLContent,wTm,pFed);
    if (wSt!=ZS_SUCCESS) {
      wSt=ZDateFull::checkMDY(wDLContent,wTm,pFed);
      if (wSt!=ZS_SUCCESS) {
        errorLog("Invalid literal date <%s> at line %d column %d.",
            wDLContent.toString(),
            Tokenizer->Tab(SvIndex)->TokenLine,Tokenizer->Tab(SvIndex)->TokenColumn);
        return ZS_INV_LITERAL;
      }
    }
    ZDateFull wD;
    wD._toInternal(wTm);
    wLit->Content = wD;

    if (!advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
      delete wLit;
      return ZS_INVTYPE ;
    }

    pOperand = wLit ;
    return ZS_SUCCESS;
  } //ZSRCH_ZDATE_LITERAL

  case ZSRCH_RESOURCE_LITERAL: {

    ZSearchLiteral<ZResource> *wLit=new ZSearchLiteral<ZResource>;

    wLit->Type = ZSTO_LiteralResource;
//    wLit->TokenList.push(Tokenizer->Tab(Index));

    if (!advanceIndex()) {
      delete wLit;
      pOperand=nullptr;
      return ZS_SYNTAX_ERROR;
    }

    if(Tokenizer->Tab(Index)->Type!=ZSRCH_OPENPARENTHESIS) {
      errorLog("ZResource literal : wrong syntax. Expecting open parenthesis. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      return ZS_MISS_PUNCTSIGN ;
    }
//    wLit->TokenList.push(Tokenizer->Tab(Index));
    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }
    if(Tokenizer->Tab(Index)->Type != ZSRCH_IDENTIFIER ) {
      errorLog("Missing ZEntity identifier. Found <%s> at line %d column %d",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      pOperand=nullptr;
      return ZS_MISS_LITERAL ;
    }
    int wi=0;
    for (;wi < ZEntityList.count();wi++) {
      if (Tokenizer->Tab(Index)->Text==ZEntityList[wi].Symbol) {
        wLit->ModVal1 = ZEntityList[wi].Value ;
        break;
      }
    }
    if (wi==ZEntityList.count()) {
      errorLog("Invalid ZEntity identifier (not found in ZEntityList). Token <%s> at line %d column %d",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      pOperand=nullptr;
      return ZS_MISS_LITERAL ;
    }
    ZEntity_type    wEntity = ZEntityList.Tab(wi).Value;

//    wLit->TokenList.push(Tokenizer->Tab(Index));
    if (!advanceIndex())
      return ZS_SYNTAX_ERROR;
    if(Tokenizer->Tab(Index)->Type!=ZSRCH_COMMA) {
      errorLog("Wrong ZResource literal syntax. Expecting comma, found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      pOperand=nullptr;
      return ZS_MISS_PUNCTSIGN ;
    }
//    wLit->TokenList.push(Tokenizer->Tab(Index));

    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }

    if(Tokenizer->Tab(Index)->Type!=ZSRCH_NUMERIC_LITERAL) {
      errorLog("Wrong ZResource literal syntax. Missing numeric literal. Found <%s> at line %d column %d",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      return ZS_MISS_LITERAL ;
    }

    Resourceid_type wId = Tokenizer->Tab(Index)->Text.toLong() ;
    wLit->Content = ZResource(wId,wEntity);

//    wLit->TokenList.push(Tokenizer->Tab(Index));

    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }

    if(Tokenizer->Tab(Index)->Type!=ZSRCH_CLOSEPARENTHESIS) {
      errorLog("ZResource literal : wrong syntax. Expecting close parenthesis. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      return ZS_MISS_PUNCTSIGN ;
    }

//    wLit->TokenList.push(Tokenizer->Tab(Index));

    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
      delete wLit;
      return ZS_INVTYPE ;
    }

    pOperand = wLit;

    return ZS_SUCCESS;
  } // ZSRCH_RESOURCE_LITERAL

  case ZSRCH_CHECKSUM_LITERAL: {

    ZSearchLiteral<checkSum> *wLit=new ZSearchLiteral<checkSum>;

    wLit->Type = ZSTO_LiteralChecksum;
//    wLit->TokenList.push(Tokenizer->Tab(Index));

    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }
    if(Tokenizer->Tab(Index)->Type!=ZSRCH_OPENPARENTHESIS) {
      errorLog("checkSum literal : wrong syntax. Expecting open parenthesis. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      return ZS_MISS_PUNCTSIGN ;
    }
//    wLit->TokenList.push(Tokenizer->Tab(Index));
    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }

    if(Tokenizer->Tab(Index)->Type != ZSRCH_HEXA_LITERAL ) {
      errorLog("Missing Hexadecimal literal (64 hexa characters). Found <%s> at line %d column %d",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      return ZS_MISS_LITERAL ;
    }

    wSt=wLit->Content.fromHexa(Tokenizer->Tab(Index)->Text);
    if (wSt!=ZS_SUCCESS) {
      errorLog("Invalid checksum hexadecimal litteral : possibly invalid size of hash code. Token <%s> at line %d column %d",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      return ZS_MISS_LITERAL ;
    }
//    wLit->TokenList.push(Tokenizer->Tab(Index));

    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }

    if(Tokenizer->Tab(Index)->Type!=ZSRCH_CLOSEPARENTHESIS) {
      errorLog("ZResource literal : wrong syntax. Expecting close parenthesis. Found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
      delete wLit;
      return ZS_MISS_PUNCTSIGN ;
    }
//    wLit->TokenList.push(Tokenizer->Tab(Index));


    if (!advanceIndex()) {
      delete wLit;
      return ZS_SYNTAX_ERROR;
    }

    if (!OperandTypeCheck(pRequestedType,wLit->getOperandBase(), wSvIndex)) {
      delete wLit;
      return ZS_INVTYPE ;
    }

    pOperand = wLit;
    return ZS_SUCCESS;
  } // ZSRCH_RESOURCE_LITERAL

  default:
    errorLog("Wrong literal syntax. Expecting literal, found <%s> at line %d column %d.",
        Tokenizer->Tab(Index)->Text.toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
    return ZS_MISS_LITERAL ;
  }//switch

  return ZS_SUCCESS;
} //_parseLiteral
/** gets one element of a date either day, month year, hour minute seconds whenever mentionned (will define date precision).
 * Element must be immediately followed by a valid separator pEndSeparator or not if equals SRCH_Nothing.
 * Returs ZS_SUCCESS if OK, ZS_EOF if Closing parenthesis has been encountered AFTER the date element or ZS_SYNTAX_ERROR in other cases
 *
*/
ZStatus
ZSearchParser::_getZDateOnePiece(int &pValue,int pMaxDigits,ZSearchTokentype pEndSeparator)
{
  if (Tokenizer->Tab(Index)->Type!=ZSRCH_NUMERIC_LITERAL) {
    errorLog("Date literal : invalid date literal value. Expecting date literal with format [d]d{/|-}[m]m{/|-}[yy]yy[-hh[:mm:[ss]]]. Found <%s> at line %d column %d.",
        Tokenizer->Tab(Index)->Text.toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);
    return ZS_SYNTAX_ERROR;
  }
  pValue=Tokenizer->Tab(Index)->Text.toInt();
  if (!advanceIndex()) {
    return ZS_SYNTAX_ERROR;
  }
  if (Tokenizer->Tab(Index)->Type==ZSRCH_CLOSEPARENTHESIS)
    return ZS_EOF;

  if (Tokenizer->Tab(Index)->Type==pEndSeparator) {
    if (!advanceIndex()) {
      return ZS_SYNTAX_ERROR;
    }
  }
  return ZS_SUCCESS;
}


/*                                 <-------selection clause-------------->
  find  [all,first]  <entity name> with   <entity name>.<field name> .... [as <collection name>] ;  NB: by default, find first is executed \n
 */
ZStatus
ZSearchParser::_parseFind(std::shared_ptr<ZSearchEntity> &pCollection)
{
  ZStatus wSt=ZS_SUCCESS;
  Action = ZSPA_Find;
  ZSearchLogicalTerm* wLogicalTerm=nullptr;

  CurEntities.clear(); /* reset current entities being used (defined within phrase) */

  if (!advanceIndex())
    return ZS_SYNTAX_ERROR;

  switch (Tokenizer->Tab(Index)->Type) {
    case ZSRCH_ALL:
      Action |= ZSPA_All;
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;
      break;
    case ZSRCH_FIRST:
      Action |= ZSPA_First;
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;
      break;
    default:
      warningLog("Missing keyword one of [<ALL>,<FIRST>] set to default : FIRST - at line %d column %d. Found <%s>.",
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
      Action |= ZSPA_First;
      break;
    }

    if (Tokenizer->Tab(Index)->Type!=ZSRCH_IDENTIFIER) {
      warningLog("No active entity has been defined." );
      return ZS_INV_ENTITY;
    }

    /* search for a valid open entity */
    Entity = EntityList.getEntityByName(Tokenizer->Tab(Index)->Text);
    if (Entity==nullptr) {
        warningLog("Invalid entity identifier found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn  );
        return ZS_INV_ENTITY;
      }
      /* here active entity is set to Tokenizer->Tab(Index)->Text */

      CurEntities.push(Entity); /* set found entity as current entity */

      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;
/*
    if (Tokenizer->Tab(Index)->Type!=ZSRCH_WITH) {
      warningLog("Syntax error : Expected clause <WITH> while found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn  );
      return ZS_MISS_KEYWORD;
    }
*/
  if (Tokenizer->Tab(Index)->Type == ZSRCH_WITH) {

    if (!advanceIndex())
      return ZS_SYNTAX_ERROR;

    wSt=_parseLogicalTerm(wLogicalTerm,0,Index);
    if (wSt!=ZS_SUCCESS)
      return wSt;

  } // there is a WITH clause
  else {
    warningLog("No WITH clause has been found : all elements from entity <%s> will be selected .",
        Entity->getName().toString()  );
  }

    /* here token must be 'AS' */

    if (Tokenizer->Tab(Index)->Type!=ZSRCH_AS) {
      errorLog("Syntax error : Missing clause <AS> while found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn  );
      if (wLogicalTerm!=nullptr)
        delete wLogicalTerm;
      return ZS_MISS_KEYWORD;
    }

    if (!advanceIndex()) {
      if (wLogicalTerm!=nullptr)
        delete wLogicalTerm;
      return ZS_SYNTAX_ERROR;
    }

    /* here entity name as collection name */

    if (Tokenizer->Tab(Index)->Type!=ZSRCH_IDENTIFIER ) {
      warningLog("Syntax error : Missing collection identifier while found <%s> at line %d column %d.",
          Tokenizer->Tab(Index)->Text.toString(),
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn  );
      if (wLogicalTerm!=nullptr)
        delete wLogicalTerm;
      return ZS_MISS_FIELD;
    }

    /* create collection */

    pCollection = ZSearchEntity::constructWithCollectionEntity(Entity,Tokenizer->Tab(Index));

    if (!advanceIndex()) {
      pCollection.reset();
      delete wLogicalTerm;
      return ZS_SYNTAX_ERROR;
    }
    if (!testSemiColon()){
      pCollection.reset();
      delete wLogicalTerm;
      return ZS_MISS_PUNCTSIGN ;
    }

    pCollection->setLogicalTerm(wLogicalTerm);

    EntityList.push(pCollection);
    infoLog("Collection <%s> has been created and registered as a new search entity.",
        pCollection->getName().toString()  );


  return ZS_SUCCESS;
}// ZSearchParser::_parseFind

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


ZSearchOperandType getMainOperandType(ZSearchLogicalOperand* pOperand)
{
  if (pOperand->Type==ZSTO_Logical)
    return ZSTO_Bool;             /* if operand is logical then result has type boolean */
  if (pOperand->Type==ZSTO_Arithmetic)
    return pOperand->Type;        /* if operand is arithmetic then result has literal type defined at operand level */


  /* in all other cases, main type is deduced from detailed operand type */

  ZSearchOperandType wType = ZSearchOperandType(pOperand->Type & ZSTO_BaseMask);
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
ZStatus
ZSearchParser::_parseOneLogicalOperand(void* & pOperand, ZSearchOperandType  &pMainType,int pParenthesisLevel)
{
  int wSvIndex=Index;
  pOperand=nullptr;
  ZStatus wSt=ZS_SUCCESS;

  while (true) {

    if (Tokenizer->Tab(Index)->Type == ZSRCH_OPENPARENTHESIS) {
      if (!advanceIndex()) {
        return ZS_SYNTAX_ERROR;
      }
      pParenthesisLevel++;
      wSt=_parseLogicalTerm((ZSearchLogicalTerm* &)pOperand,pParenthesisLevel+1,Index);
      if (wSt!=ZS_SUCCESS) {
        return wSt;
      }
      pMainType = ZSTO_Bool;/* result of a logical term operand is a bool */

      if (Tokenizer->Tab(Index)->Type!=ZSRCH_CLOSEPARENTHESIS) {
        errorLog("Syntax error : Closed parenthesis expected. found <%s> - at line %d column %d",
            Tokenizer->Tab(Index)->Text.toString(),
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);

        return ZS_SYNTAX_ERROR;
      }
      if (pParenthesisLevel<0) {
        errorLog("Syntax error : Closed parenthesis without corresponding open parenthesis - at line %d column %d",
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);

        return ZS_SYNTAX_ERROR;
      }

      if (!advanceIndex()) {
        return ZS_SYNTAX_ERROR;
      }

      break;  /* Operand is set as a new logical formula */
    }// ZSRCH_OPENPARENTHESIS

    if (Tokenizer->Tab(Index)->Type==ZSRCH_IDENTIFIER) {

      /* check if symbol or ZEntity */

      if (_parseZEntity(pOperand,pMainType) ) {
        break;
      }
      if (_parseSymbol(pOperand,pMainType) ) {
        /* Capture base type of operand */
        break;
      }

      wSt=_parseOperandField(pOperand,pMainType);
      if (wSt!=ZS_SUCCESS) {
        return wSt;
      }

      break;
    } // ZSRCH_IDENTIFIER

    if ((Tokenizer->Tab(Index)->Type & ZSRCH_LITERAL)==ZSRCH_LITERAL) {
      wSt=_parseLiteral(pOperand,pMainType);
      if (wSt!=ZS_SUCCESS) {
        return wSt;
      }
      //        wTerm->Operand1.Type = ZSTO_Literal;
      break;
    } // ZSRCH_LITERAL
    break;
  } // while true

  if (ZSearchOperator::isArithmeric(Tokenizer->Tab(Index))) {
    bool wIsLiteral=true;
    clearOperand(pOperand);

    wSt=_parseArithmeticTerm((ZSearchArithmeticTerm* &)pOperand,pParenthesisLevel,wSvIndex, pMainType,wIsLiteral);
    if (wSt!=ZS_SUCCESS) {
      return wSt;
    }
    /* if all components of arithmetic operation are literal, then arithmetic term is once evaluated and its value is stored and never recomputed */

    if (wIsLiteral) {

      utf8VaryingString wFormula ;

      for (int wi=wSvIndex; wi < Index; wi++) {
        wFormula += Tokenizer->Tab(wi)->Text;
        wFormula += " ";
      }

    ZSearchOperandType wType = ZSearchOperandType(static_cast<ZSearchArithmeticTerm*>(pOperand)->Type & ZSTO_BaseMask);
    infoLog("Arithmetic expression [%s] is a strict literal of type <%s> ", wFormula.toString(),decode_OperandType(wType));

    ZOperandContent wResult=computeArithmeticLiteral(pOperand);
    if (!wResult.isValid()) {
        errorLog("Arithmetic expression [%s] is an invalid literal expression ", wFormula.toString());
        clearOperand(pOperand);
        return ZS_INVOP;
    }

    switch (wResult.Type)
    {
      case ZSTO_Integer:
      {
        ZSearchLiteral<long>* wLit = new ZSearchLiteral<long>;
        clearOperand(pOperand);
        wLit->Type = ZSTO_LiteralInteger;
        wLit->Content = wResult.Integer;
        wLit->Comment = wFormula;
        if (!OperandTypeCheck(pMainType,wLit->getOperandBase(),wSvIndex)) {
          delete wLit;
          return ZS_INVTYPE;
        }
        pOperand = wLit;
        return ZS_SUCCESS;
      }
      case ZSTO_Float:
      {
        ZSearchLiteral<double>* wLit = new ZSearchLiteral<double>;
        wLit->Type = ZSTO_LiteralFloat;
        wLit->Content = wResult.Float;
        wLit->Comment = wFormula;
        if (!OperandTypeCheck(pMainType,wLit->getOperandBase(),wSvIndex)) {
          delete wLit;
          return ZS_INVTYPE;
        }
        pOperand = wLit;
        return ZS_SUCCESS;
      }
      case ZSTO_UriString:
      case ZSTO_String:
      {
        ZSearchLiteral<utf8VaryingString>* wLit = new ZSearchLiteral<utf8VaryingString>;
        wLit->Type = ZSTO_LiteralString;
        wLit->Content = wResult.String;
        wLit->Comment = wFormula;
        if (!OperandTypeCheck(pMainType,wLit->getOperandBase(),wSvIndex)) {
          delete wLit;
          return ZS_INVTYPE;
        }
        pOperand = wLit;
        return ZS_SUCCESS;
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
        return ZS_INVOP;
      }// switch

   } // is a full literal

    /* arithmetic expression involving at least one field :
     * pOperand is already loaded with arithmetic terms chain */

    return ZS_SUCCESS;
  } // operator is arithmetic


  return ZS_SUCCESS;
} // _parseOneLogicalOperand


ZStatus
ZSearchParser::_parseLogicalTerm(ZSearchLogicalTerm* & pTerm,
    int pParenthesisLevel, int pBookMark)
{

  setIndex(pBookMark);
  int wSvIndex=Index;
  pTerm=nullptr;

  ZSearchOperandType wOpType = ZSTO_Nothing;
  ZStatus wSt=ZS_SUCCESS;

  ZSearchLogicalTerm* wTerm = new ZSearchLogicalTerm;
  wTerm->ParenthesisLevel = pParenthesisLevel;
  //  wTerm->Collateral = pCollateral;

  setIndex(pBookMark);


  if (Tokenizer->Tab(Index)->Type == ZSRCH_OPERATOR_NOT)  {
    wTerm->NotOperator.set(Tokenizer->Tab(Index));
    if (!advanceIndex()) {
      delete wTerm;
      return ZS_SYNTAX_ERROR;
    }
  }

  wTerm->MainType = ZSTO_Nothing;

  /* first operand acquisition */

  wSt = _parseOneLogicalOperand(wTerm->Operand1,wTerm->MainType,pParenthesisLevel);
  if (wSt!=ZS_SUCCESS) {
    delete wTerm;
    return wSt;
  }

  /* compare operator */

  else if (!ZSearchOperator::isComparator(Tokenizer->Tab(Index))) {
    errorLog("Invalid logical operator <%s> at line %d column %d.",
        Tokenizer->Tab(Index)->Text.toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index+2)->TokenColumn );
    delete wTerm;
    return ZS_INV_OPERATOR;
  }// not comparator


  wTerm->CompareOperator.set(Tokenizer->Tab(Index));

  if (!advanceIndex()) {
    delete wTerm;
    return ZS_SYNTAX_ERROR;
  }

  /*----------Operand 2 ----------------*/
  wSvIndex = Index;

  wSt = _parseOneLogicalOperand(wTerm->Operand2,wTerm->MainType,pParenthesisLevel);
  if (wSt!=ZS_SUCCESS) {
    delete wTerm;
    return wSt;
  }


  if (ZSearchOperator::isAndOr(Tokenizer->Tab(Index))) {
    wTerm->AndOrOperator.set(Tokenizer->Tab(Index));
    if (!advanceIndex()) {
      delete wTerm;
      return ZS_SYNTAX_ERROR;
    }
    wSt=_parseLogicalTerm(wTerm->NextTerm,pParenthesisLevel,Index);
    if (wSt!=ZS_SUCCESS) {
      delete wTerm;
      return wSt;
    }

  }

  pTerm=wTerm;
  return ZS_SUCCESS;

}//ZSearchParser::_parseLogicalTerm



bool ZSearchParser::_arithmeticTypeCheck(ZSearchOperandType& pMainType,ZSearchOperandBase* pOB, int pIndex)
{
  ZSearchOperandType wOpBaseType = ZSearchOperandType(pOB->Type & ZSTO_BaseMask);
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
      errorLog("Type mismatch: invalid operand type Bool in arithmetic expression at line %d column %d.",
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }// switch

    /* ???? */
    pMainType = wOpBaseType ;
    return true;

  case ZSTO_UriString:
  case ZSTO_String:
    if (wOpBaseType!=ZSTO_String) {
      errorLog("Type mismatch: expected operand with type string while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
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
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    if (wOpBaseType == ZSTO_Date) {
      if (pOB->hasModifier())
        return true;
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    if (wOpBaseType == ZSTO_Resource) {
      if (pOB->hasModifier())
        return true;
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
        decode_OperandType(pMainType),
        decode_OperandType(wOpBaseType),
        Tokenizer->Tab(pIndex)->Text.toString(),
        Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
    return false;

  case ZSTO_Date:
    /* here must be strictly date */
    if (wOpBaseType!=ZSTO_Date) {
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    return true;

  case ZSTO_Resource:
    /* here must be strictly resource */
    if (wOpBaseType!=ZSTO_Resource) {
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    return true;
  case ZSTO_Bool:
    errorLog("Arithmetic operand type mismatch: invalid operand with type Bool in an arithmetic expression. Token  <%s> at line %d column %d.",
        Tokenizer->Tab(Index)->Text.toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
    return false;
  case ZSTO_Checksum:
    if (wOpBaseType!=ZSTO_Checksum) {
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    return true;
  default:
    errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
        decode_OperandType(pMainType),
        decode_OperandType(wOpBaseType),
        Tokenizer->Tab(pIndex)->Text.toString(),
        Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
    return false;
  }//swich

} // ZSearchParser::_arithmeticTypeCheck


bool ZSearchParser::OperandTypeCheck(ZSearchOperandType& pMainType,ZSearchOperandBase* pOB, int pIndex)
{
  ZSearchOperandType wOpBaseType = ZSearchOperandType(pOB->Type & ZSTO_BaseMask);
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
      errorLog("Type mismatch: expected operand with type string while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
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
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    if (wOpBaseType == ZSTO_Date) {
      if (pOB->hasModifier())
        return true;
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    if (wOpBaseType == ZSTO_Resource) {
      if (pOB->hasModifier())
        return true;
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
        decode_OperandType(pMainType),
        decode_OperandType(wOpBaseType),
        Tokenizer->Tab(pIndex)->Text.toString(),
        Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
    return false;

  case ZSTO_Date:
    /* here must be strictly date */
    if (wOpBaseType!=ZSTO_Date) {
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    return true;

  case ZSTO_Resource:
    /* here must be strictly resource */
    if (wOpBaseType!=ZSTO_Resource) {
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    return true;
  case ZSTO_Bool:
    errorLog("Arithmetic operand type mismatch: invalid operand with type Bool in an arithmetic expression. Token  <%s> at line %d column %d.",
        Tokenizer->Tab(Index)->Text.toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
    return false;
  case ZSTO_Checksum:
    if (wOpBaseType!=ZSTO_Checksum) {
      errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
          decode_OperandType(pMainType),
          decode_OperandType(wOpBaseType),
          Tokenizer->Tab(pIndex)->Text.toString(),
          Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
      return false;
    }
    return true;
  default:
    errorLog("Type mismatch: expected operand with type <%s> while found base type <%s> within <%s> at line %d column %d.",
        decode_OperandType(pMainType),
        decode_OperandType(wOpBaseType),
        Tokenizer->Tab(pIndex)->Text.toString(),
        Tokenizer->Tab(pIndex)->TokenLine,Tokenizer->Tab(pIndex)->TokenColumn );
    return false;
  }//swich

} // ZSearchParser::_logicalTypeCheck


ZStatus
ZSearchParser::_parseArithmeticTerm(ZSearchArithmeticTerm*& pArithTerm,
                                int pParenthesisLevel,  int pBookMark, ZSearchOperandType& pRequestedType, bool &pIsLiteral)
{
  ZStatus wSt=ZS_SUCCESS;

  pArithTerm=nullptr;

  setIndex(pBookMark);

  int wSvIndex=Index;

  ZSearchArithmeticTerm* wArithTerm = new ZSearchArithmeticTerm;
  wArithTerm->ParenthesisLevel = pParenthesisLevel;

  while (true) {

    if (Tokenizer->Tab(Index)->Type==ZSRCH_OPENPARENTHESIS) {
      if (!advanceIndex()) {
        delete wArithTerm;
        return ZS_SYNTAX_ERROR;
      }
      wSt=_parseArithmeticTerm((ZSearchArithmeticTerm*&)wArithTerm->Operand,pParenthesisLevel+1,Index,pRequestedType,pIsLiteral);
      if (wSt!=ZS_SUCCESS) {
        delete wArithTerm;
        return wSt;
      }
      if (Tokenizer->Tab(Index)->Type!=ZSRCH_CLOSEPARENTHESIS) {
        errorLog("Syntax error: expected closing parenthesis while found <%s> at line %d column %d.",
            Tokenizer->Tab(Index)->Text.toString(),
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
        delete wArithTerm;
        return ZS_MISS_PUNCTSIGN;
      }
      break; /* first operand is stored as an arithmetic formula */
    } //ZSRCH_OPENPARENTHESIS


    if (Tokenizer->Tab(Index)->Type==ZSRCH_IDENTIFIER) {

      if (_parseZEntity(wArithTerm->Operand,pRequestedType) ) {
        break;
      }
      if (_parseSymbol(wArithTerm->Operand,pRequestedType) ) {
        break;
      }

      wSt=_parseOperandField(wArithTerm->Operand,pRequestedType);
      if (wSt!=ZS_SUCCESS) {
        delete wArithTerm;
        return wSt;
      }
      pIsLiteral = false;
      break;
    } // ZSRCH_IDENTIFIER

    if ((Tokenizer->Tab(Index)->Type & ZSRCH_LITERAL)==ZSRCH_LITERAL) {
      wSt=_parseLiteral(wArithTerm->Operand,pRequestedType);
      if (wSt!=ZS_SUCCESS) {
        delete wArithTerm;
        return wSt;
      }
      break;
    } // ZSRCH_LITERAL

    errorLog("Syntax error: expected either an identifier or a litteral while found <%s> at line %d column %d.",
        Tokenizer->Tab(Index)->Text.toString(),
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn );
    return ZS_INVVALUE;
  }// while true

  ZSearchOperandBase* wOB=static_cast<ZSearchOperandBase*>(wArithTerm->Operand);
  if (!_arithmeticTypeCheck(pRequestedType,wOB,wSvIndex)) {
    delete wArithTerm;
    return ZS_INVTYPE;
  }

  wArithTerm->Type = ZSearchOperandType( ZSTO_Arithmetic | (wOB->Type & ZSTO_BaseMask));  /* extract the base type (string, integer,etc.) from operand */

  /* if not an arithmetic operator, then arithmetic expression is ended */
  if (!ZSearchOperator::isArithmeric(Tokenizer->Tab(Index))) {
    wArithTerm->Operator.Type=ZSTO_Nothing;
    wArithTerm->OperandNext = nullptr;
    pArithTerm = wArithTerm;
    return ZS_SUCCESS ;
  }

  wArithTerm->Operator.set(Tokenizer->Tab(Index));

  if (!advanceIndex()) {
    delete wArithTerm;
    return ZS_SYNTAX_ERROR;
  }
  /*  parse next operand */

  wSt=_parseArithmeticTerm((ZSearchArithmeticTerm*&)wArithTerm->OperandNext,pParenthesisLevel,Index,pRequestedType,pIsLiteral);
  if (wSt!=ZS_SUCCESS) {
    delete wArithTerm;
    return wSt;
  }

  pArithTerm = wArithTerm;
  return wSt;
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
  int wBookMark = Index;

  pFormula = new ZSearchFormula(ZFORT_Term,pParenthesisLevel);

  if (Tokenizer->Tab(Index)->Type == ZSRCH_OPERATOR_NOT)  {
    pFormula->LeadingOperator.add(Tokenizer->Tab(Index));
    pFormula->LeadingOperator.Type |= ZSOPV_NOT ;
    if (!advanceIndex())
      return ZS_SYNTAX_ERROR;
  }

  if (Tokenizer->Tab(Index)->Type == ZSRCH_OPENPARENTHESIS)
    {
    if (!advanceIndex())
      return ZS_SYNTAX_ERROR;
      pParenthesisLevel++;
      pFormula->setType(ZFORT_Formula);
      ZSearchFormula* wFormulaPtr=nullptr;
      wSt=_parseFormula(wFormulaPtr,pParenthesisLevel);
      if (wSt!=ZS_SUCCESS)
        goto _parseFormulaError;

      pFormula->TermOrFormula = wFormulaPtr;

      if (Tokenizer->Tab(Index)->Type!=ZSRCH_CLOSEPARENTHESIS) {
        errorLog("Syntax error : Closed parenthesis expected. found <%s> - at line %d column %d",
            Tokenizer->Tab(Index)->Text.toString(),
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);

        wSt = ZS_SYNTAX_ERROR ;
        goto _parseFormulaError;
      }
      if (pParenthesisLevel<0) {
          errorLog("Syntax error : Closed parenthesis without corresponding open parenthesis - at line %d column %d",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn);

          wSt = ZS_SYNTAX_ERROR ;
          goto _parseFormulaError;
      }

      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;

    }// ZSRCH_OPENPARENTHESIS

    if ((Tokenizer->Tab(Index)->Type == ZSRCH_IDENTIFIER) ||
        ((Tokenizer->Tab(Index)->Type & ZSRCH_LITERAL)==ZSRCH_LITERAL)) {

      ZSearchLogicalOperand* wTerm=nullptr;
      wSt=_parseLogical((void*)wTerm,0,0,Index);
      if (wSt!=ZS_SUCCESS)
        goto _parseFormulaError;

      pFormula->setType(ZFORT_Term);
      pFormula->TermOrFormula=wTerm;

//      advanceIndex();

    } else  {
      errorLog("Syntax error : Expected either field identifier or literal at line %d column %d. Found <%s>.",
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,
          Tokenizer->Tab(Index)->Text.toString());

      wSt = ZS_SYNTAX_ERROR ;
      goto _parseFormulaError;
    }

  /* getting link logical operator that will link formula */



  if ((Tokenizer->Tab(Index)->Type == ZSRCH_OPERATOR_AND) ||
        (Tokenizer->Tab(Index)->Type == ZSRCH_OPERATOR_OR)) {
    pFormula->LeadingOperator.add(Tokenizer->Tab(Index));
    if (!advanceIndex())
      return ZS_SYNTAX_ERROR;
  }
  else if (Tokenizer->Tab(Index)->Type != ZSRCH_AS) {
    errorLog("Syntax error : Expected logical operator one of {AND,OR} at line %d column %d. Found <%s>.",
        Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,
        Tokenizer->Tab(Index)->Text.toString());

    wSt = ZS_SYNTAX_ERROR ;
    goto _parseFormulaError;
  } // not = 'AS'

  if (Tokenizer->Tab(Index)->Type == ZSRCH_OPERATOR_NOT)  {

    pFormula->LeadingOperator.add(Tokenizer->Tab(Index));
    if (!advanceIndex())
      return ZS_SYNTAX_ERROR;

    /* NB: NOT operand must be followed by expression within parenthesis */
    if (Tokenizer->Tab(Index)->Type != ZSRCH_OPENPARENTHESIS)
    {
      errorLog("Syntax error : Logical operator NOT must be followed by parenthesis at line %d column %d. Found <%s>.",
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,
          Tokenizer->Tab(Index)->Text.toString());

      wSt = ZS_SYNTAX_ERROR ;
      goto _parseFormulaError;
    }

  } // ZSRCH_OPERATOR_NOT


  /* getting next formula */

  if ((Tokenizer->count() < Index) &&
      (Tokenizer->Tab(Index)->Type!= ZSRCH_AS) &&
      (Tokenizer->Tab(Index)->Type!= ZSRCH_SEMICOLON)) {

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

ZStatus ZSearchParser::_parse(std::shared_ptr<ZSearchEntity> &pCollection,bool &pStoreInstruction,int &pInstructionType) {
  ZStatus wSt=ZS_SUCCESS;

  FirstToken = Tokenizer->Tab(Index);

  CurrentToken=nullptr;
  bool wHasInstruction = false;

  while ((wSt!=ZS_EOF)&&(Index < Tokenizer->count())) {
    bool wExit=false;

    while ((Index < Tokenizer->count())&&(Tokenizer->Tab(Index)->Type==ZSRCH_SPACE))
      if (!advanceIndex())
        return ZS_SYNTAX_ERROR;

    CurrentToken=Tokenizer->Tab(Index);

    if ((Tokenizer->Tab(Index)->Type & ZSRCH_INSTRUCTION_MASK)!=ZSRCH_INSTRUCTION_MASK) {
      errorLog("Missing instruction at line %d column %d (one of SET...,FIND---,FOR---). Found <%s>.",
          Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );

      return ZS_MISS_KEYWORD;
    }

    while ((Index < Tokenizer->count())&&(!wExit)) {
      if (_parseComment (Tokenizer->Tab(Index))) {
        if (!advanceIndex())
          return ZS_EOF;
        continue;
      }
      CurrentToken=Tokenizer->Tab(Index);

      switch (Tokenizer->Tab(Index)->Type) {
      case ZSRCH_SET:
      {
        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;

        CurrentToken=Tokenizer->Tab(Index);

        if (Tokenizer->Tab(Index)->Type ==ZSRCH_HISTORY) {
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;

          if (Tokenizer->Tab(Index)->Type != ZSRCH_MAXIMUM) {
          errorLog("Missing required word <MAXIMUM> at line %d column %d . Found <%s>.",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
          return ZS_MISS_KEYWORD;
          }
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;

          if (Tokenizer->Tab(Index)->Type != ZSRCH_NUMERIC_LITERAL) {
            errorLog("Missing numeric literal at line %d column %d . Found <%s>.",
                Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
            return ZS_MISS_LITERAL;
          }

          int wNewMax = Tokenizer->Tab(Index)->Text.toInt();

          if (!testSemiColon())
            return ZS_MISS_PUNCTSIGN;

          setHistoryMaximum(wNewMax);
          break;

        } //   ZSRCH_HISTORY


        if (Tokenizer->Tab(Index)->Type !=ZSRCH_FILE) {
          errorLog("Missing required word <FILE> at line %d column %d . Found <%s>.",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
          return ZS_MISS_KEYWORD;
        }

        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;

        wSt=_parseSetFile(pCollection);
        if (wSt!=ZS_SUCCESS)
            return wSt;
        wHasInstruction=true;
        break;
      }// ZSRCH_SET

      case ZSRCH_SHOW:
      {
        pStoreInstruction=false;
        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;
        wSt = _parseShow();
        if (wSt!=ZS_SUCCESS)
          return wSt;
        wHasInstruction=true;
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
        pStoreInstruction=false;
        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;
        if (Tokenizer->Tab(Index)->Type != ZSRCH_HISTORY) {
          errorLog("Missing required word <HISTORY> at line %d column %d . Found <%s>.",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
          return ZS_MISS_KEYWORD;
        }
        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;

        if (Tokenizer->Tab(Index)->Type == ZSRCH_SEMICOLON) {
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;
          clearHistory();
          break;
        }

        if (Tokenizer->Tab(Index)->Type == ZSRCH_FIRST) {
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;
          if (Tokenizer->Tab(Index)->Type != ZSRCH_NUMERIC_LITERAL) {
            errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
            return ZS_MISS_LITERAL;
          }
          int wNb = Tokenizer->Tab(Index)->Text.toInt();
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;

          if (!testSemiColon()) {
            return ZS_MISS_PUNCTSIGN;
          }
          clearHistoryFirst(wNb);
          break;
        } // ZSRCH_FIRST

        if (Tokenizer->Tab(Index)->Type == ZSRCH_LAST) {
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;
          if (Tokenizer->Tab(Index)->Type != ZSRCH_NUMERIC_LITERAL) {
            errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
            return ZS_MISS_LITERAL;
          }
          int wNb = Tokenizer->Tab(Index)->Text.toInt();
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;

          if (!testSemiColon()) {
            return ZS_MISS_PUNCTSIGN;
          }
          clearHistoryLast(wNb);
          break;
        } // ZSRCH_LAST

        if (Tokenizer->Tab(Index)->Type == ZSRCH_AT) {
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;
          if (Tokenizer->Tab(Index)->Type != ZSRCH_NUMERIC_LITERAL) {
            errorLog("Missing required numeric literal as number of history lines to remove at line %d column %d . Found <%s>.",
                Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
            return ZS_MISS_LITERAL;
          }
          int wNb = Tokenizer->Tab(Index)->Text.toInt();
          if (!advanceIndex())
            return ZS_SYNTAX_ERROR;

          if (!testSemiColon()) {
            return ZS_MISS_PUNCTSIGN;
          }
          clearHistoryRank(wNb);
          break;
        } // ZSRCH_AT

          errorLog("Missing required keyword one of {FIRST,LAST,AT} at line %d column %d . Found <%s>.",
              Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
          return ZS_MISS_KEYWORD;

      } // ZSRCH_CLEAR


      case ZSRCH_FIND:
      {
 //       std::shared_ptr<ZSearchEntity>  wCollection=nullptr;
        wSt=_parseFind(pCollection);
        if (wSt!=ZS_SUCCESS)
          return wSt;

        EntityList.push(pCollection);

        pInstructionType = ZSITP_Find;

        infoLog("Collection report \n %s\n",pCollection->_report().toString());
/*
        wSt= _executeFind(wCollection->_CollectionEntity);
        if (wSt!=ZS_SUCCESS) {
          return wSt;
        }
*/
        wHasInstruction=true;
        break;
      } // ZSRCH_FIND
      case ZSRCH_FOR:
        if (!advanceIndex())
          return ZS_SYNTAX_ERROR;
        wSt=_parseFor(pCollection);
        if (wSt!=ZS_SUCCESS)
          return wSt;
        wHasInstruction=true;
        break;

      case ZSRCH_FINISH:
        if (!testSemiColon())
          return ZS_MISS_PUNCTSIGN;
        finish();
        break;

      default:
        if (Tokenizer->Tab(Index)->Type==ZSRCH_SEMICOLON)
          Index++;
        else {
        errorLog("Unrecognized instruction at line %d column %d . Found <%s>.",
            Tokenizer->Tab(Index)->TokenLine,Tokenizer->Tab(Index)->TokenColumn,Tokenizer->Tab(Index)->Text.toString() );
        return ZS_SYNTAX_ERROR;
        }
      }
    }//while ((Index < Tokenizer->count())&&(!wExit))

    if (Index == Tokenizer->count()) {
      if (Tokenizer->_progressCallback!=nullptr) {
        Tokenizer->_progressCallback(Tokenizer->count());
      }
      return ZS_SUCCESS;
    }

    if (Tokenizer->_progressCallback!=nullptr) {
      Tokenizer->_progressCallback(int(Index));
    }

  }// main while

  if (!wHasInstruction) {
    infoLog("Sentence has no valid instruction." );
  }

  return ZS_SUCCESS;
} // ZSearchParser::_parse


ZStatus
ZSearchParser::_executeFind(std::shared_ptr<_BaseCollectionEntity> pCollection)
{
  pCollection->AddressList.clear();
  ZDataBuffer wRecord;
  if (ProgressSetupCallBack!=nullptr)
    ProgressSetupCallBack(int(pCollection->getMaxRecords()));
  if (ProgressCallBack!=nullptr)
    ProgressCallBack(0);
  long wRank=0;
  zaddress_type wAddress=0;

  ZTimer wTi;
  wTi.start();

  ZStatus wSt=pCollection->get(wRecord,wRank,wAddress);
  int wSelected=0;
  int wUpdate=0;
  bool wResult;


  while (wSt==ZS_SUCCESS) {
    _DBGPRINT("ZSearchParser::_executeFind processing record rank %ld\n",wRank)
    wSt=pCollection->evaluate(wResult,wRecord);
    if(wResult) {
      pCollection->AddressList.push(wAddress);
      wSelected++;
    }

    if (ProgressCallBack!=nullptr) {
      wUpdate++;
      if (wUpdate > UpdateFrequence) {
        wUpdate=0;
        ProgressCallBack(wRank);
      }
    }
    wRank++;
    wSt=pCollection->get(wRecord,wRank,wAddress);
  } // while ZS_SUCCESS
/*
  if (wSt==ZS_OUTBOUNDHIGH) {
    wSt=ZS_SUCCESS;
  }
*/
  ProgressCallBack(wRank);

  wTi.end();
  infoLog("_executeFind report\n"
          " Number of record processed  %d\n"
          " Matches (address count)     %d\n"
          " End status                  %s\n"
          " Elapsed                     %s",
      wRank,pCollection->AddressList.count(),decode_ZStatus(wSt),wTi.reportElapsed().toString());

  return wSt;
}


void ZSearchParser::displayTokenList(ZArray<ZSearchToken*> &Whole)
{
  Tokenizer->_print("    Token list  <%ld>\n",Whole.count());
  for (long wi=0;wi<Whole.count();wi++)
    fprintf(stdout,"     %3ld [line %d col %d]  %15s-<%15s> \n",
        wi,Whole[wi]->TokenLine,Whole[wi]->TokenColumn,
        decode_SearchTokenType( Whole[wi]->Type).toCChar(),Whole[wi]->Text.toCChar());
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

  setAutoPrintOn(ZAIES_Text);

  if (!pXmlFile.exists())  {
    errorLog("ZSearchParser::loadXmlSearchParserZEntity-E-FILNFND Parameter file <%s> has not been found.",pXmlFile.toCChar());
    return ZS_FILENOTEXIST;
  }

  URIZEntity=pXmlFile;

  if ((wSt=URIZEntity.loadUtf8(wXmlString))!=ZS_SUCCESS) {
    logZException();
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
    logZException();
    errorLog(
        "ZSearchParser::loadXmlSearchParserZEntity-E-PARSERR Xml parsing error for string <%s> ",
        wXmlString.subString(0, 25).toUtf());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    logZException();
    return wSt;
  }
  if (!(wRoot->getName() == "zsearchparserzentity")) {
    errorLog(
        "ZSearchParser::loadXmlSearchParserZEntity-E-INVROOT Invalid root node name <%s> expected <zsearchparserzentity>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  /*------------------ ZEntity table -----------------------*/

  while (true) {
    wSt=wRoot->getChildByName((zxmlNode*&)wSymbolListNode,"zentitytable");
    if (wSt!=ZS_SUCCESS) {
      logZStatus(
          ZAIES_Error,
          wSt,
          "ZSearchParser::loadXmlSearchParserZEntity-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
          "zentitytable",
          decode_ZStatus(wSt));
      break;
    }


    wSt=wSymbolListNode->getFirstChild((zxmlNode*&)wSymbolKeyword);

    ZSearchFileZEntity wZEntityElt;
    while (wSt==ZS_SUCCESS) {
      if (wSymbolKeyword->getName()=="zentityitem") {
        wSt=XMLgetChildText( wSymbolKeyword,"zentity",wKeyword,this);
        if (wSt >= 1)
           wZEntityElt.Symbol = wKeyword ;

        wSt=XMLgetChildLongHexa( wSymbolKeyword,"value",wValue,this);
        wZEntityElt.Value = wValue;

        ZEntityList.push(wZEntityElt);
      } // typeconversion
      wSt=wSymbolKeyword->getNextNode((zxmlNode*&)wSwapNode);
      XMLderegister(wSymbolKeyword);
      wSymbolKeyword=wSwapNode;
    }// while (wSt==ZS_SUCCESS)

    textLog("_________________Search Parser ZEntity table load ____________________\n"
                              " %ld ZEntity values loaded.\n", ZEntityList.count());
    textLog("%3s- %27s %10s\n","rnk","ZEntity","Value");
    for (long wi=0;wi<ZEntityList.count();wi++) {
      textLog("%3ld- <%25s> <0x%08lX>\n",wi+1,
          ZEntityList[wi].Symbol.toString(),
          ZEntityList[wi].Value
          );
    }
    textLog("________________________________________________________________\n");
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

  setAutoPrintOn(ZAIES_Text);

   textLog(" Loading symbol table <%s>",pXmlFile.toString());

  if (!pXmlFile.exists())  {
    errorLog("ZSearchParser::loadXmlSearchParserSymbols-E-FILNFND Parameter file <%s> has not been found.",pXmlFile.toCChar());
    return ZS_FILENOTEXIST;
  }

  URIZEntity=pXmlFile;

  if ((wSt=URIZEntity.loadUtf8(wXmlString))!=ZS_SUCCESS) {
    logZException();
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
    logZException();
    errorLog(
        "ZSearchParser::loadXmlSearchParserSymbols-E-PARSERR Xml parsing error for string <%s> ",
        wXmlString.subString(0, 25).toUtf());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    logZException();
    return wSt;
  }
  if (!(wRoot->getName() == "zsearchparsersymbol")) {
    errorLog(
        "ZSearchParser::loadXmlSearchParserSymbols-E-INVROOT Invalid root node name <%s> expected <zsearchparsersymbol>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  /*------------------ Symbol table -----------------------*/

  while (true) {
    wSt=wRoot->getChildByName((zxmlNode*&)wSymbolListNode,"symboltable");
    if (wSt!=ZS_SUCCESS) {
      logZStatus(
          ZAIES_Error,
          wSt,
          "ZSearchParser::loadXmlSearchParserSymbols-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
          "symboltable",
          decode_ZStatus(wSt));
      break;
    }


    wSt=wSymbolListNode->getFirstChild((zxmlNode*&)wSymbolKeyword);

    ZSearchFileSymbol wSymbolElt;
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

    textLog("_________________Search Parser symbols table load ____________________\n"
                              " %ld symbols loaded.\n", SymbolList.count());
    textLog("%3s- %27s %s\n","rnk","Symbol","Path");
    for (long wi=0;wi<SymbolList.count();wi++) {
      textLog("%3ld- <%25s> <%s>\n",wi+1,
          SymbolList[wi].Symbol.toString(),
          SymbolList[wi].Path.toString()
          );
    }
    textLog("________________________________________________________________\n");
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

  setAutoPrintOn(ZAIES_Text);

  textLog(" Loading symbol table <%s>",pXmlFile.toString());

  if (!pXmlFile.exists())  {
    errorLog("ZSearchParser::loadXmlSearchParserParams-E-FILNFND Parameter file <%s> has not been found.",pXmlFile.toCChar());
    return ZS_FILENOTEXIST;
  }

  URIParams=pXmlFile;

  if ((wSt=URIParams.loadUtf8(wXmlString))!=ZS_SUCCESS) {
    logZException();
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
    logZException();
    errorLog(
        "ZSearchParser::loadXmlSearchParserSymbols-E-PARSERR Xml parsing error for string <%s> ",
        wXmlString.subString(0, 25).toUtf());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    logZException();
    return wSt;
  }
  if (!(wRoot->getName() == "zsearchparsersymbol")) {
    errorLog(
        "ZSearchParser::loadXmlSearchParserSymbols-E-INVROOT Invalid root node name <%s> expected <zsearchparsersymbol>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  /*------------------ Symbol table -----------------------*/

  while (true) {
    wSt=wRoot->getChildByName((zxmlNode*&)wParamsNode,"symboltable");
    if (wSt!=ZS_SUCCESS) {
      logZStatus(
          ZAIES_Error,
          wSt,
          "ZSearchParser::loadXmlSearchParserSymbols-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
          "symboltable",
          decode_ZStatus(wSt));
      break;
    }


    wSt=wParamsNode->getFirstChild((zxmlNode*&)wParamKeyword);

    ZSearchFileSymbol wSymbolElt;
    while (wSt==ZS_SUCCESS) {
      if (wParamKeyword->getName()=="symbolitem") {
        wSt=XMLgetChildText( wParamKeyword,"symbol",wKeyword,this);
        if (wSt >= 1)
          wSymbolElt.Symbol = wKeyword ;

        wSt=XMLgetChildText( wParamKeyword,"path",wKeyword,this);
        wSymbolElt.Path = wKeyword;

        SymbolList.push(wSymbolElt);
      } // typeconversion
      wSt=wParamKeyword->getNextNode((zxmlNode*&)wSwapNode);
      XMLderegister(wParamKeyword);
      wParamKeyword=wSwapNode;
    }// while (wSt==ZS_SUCCESS)

    textLog("_________________Search Parser symbols table load ____________________\n"
            " %ld symbols loaded.\n", SymbolList.count());
    textLog("%3s- %27s %s\n","rnk","Symbol","Path");
    for (long wi=0;wi<SymbolList.count();wi++) {
      textLog("%3ld- <%25s> <%s>\n",wi+1,
          SymbolList[wi].Symbol.toString(),
          SymbolList[wi].Path.toString()
          );
    }
    textLog("________________________________________________________________\n");
    XMLderegister(wParamsNode);
    break;
  } // while (true)


  XMLderegister((zxmlNode *&) wParamRootNode);
  XMLderegister((zxmlNode *&) wRoot);

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
    infoLog("ZSearchParser::convertToZType-I-REFSKIP Found reference (Ampersand). Skipped.");
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





void ZParserError::errorLog(const char* pFormat,...)
{
  LastErroredToken = Father->CurrentToken;
  va_list args;
  va_start (args, pFormat);
  _errorLog(pFormat,args);

  va_end(args);
}
void ZParserError::textLog(const char* pFormat,...)
{
  LastErroredToken= Father->CurrentToken;
  va_list args;
  va_start (args, pFormat);
  _textLog(pFormat,args);
  va_end(args);
}
void ZParserError::infoLog(const char* pFormat,...)
{
  LastErroredToken= Father->CurrentToken;
  va_list args;
  va_start (args, pFormat);
  _infoLog(pFormat,args);
  va_end(args);
}
void ZParserError::warningLog(const char* pFormat,...)
{
  LastErroredToken= Father->CurrentToken;
  va_list args;
  va_start (args, pFormat);
  _warningLog(pFormat,args);
  va_end(args);
}



}//namespace zbs



