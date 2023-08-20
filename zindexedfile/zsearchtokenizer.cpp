#include "zsearchtokenizer.h"

using namespace zbs;

#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zdatefull.h>

#include <zcontentcommon/zresource.h>

/*-------------Application dependant------------*/
#include <zcontent/zcontentutils/zentity.h>
/*----------------------------------------------*/

#include <ztoolset/ztime.h>
#include <iostream>
#include <zcontent/zindexedfile/zdictionaryfile.h>


#define __NAME__ "ZSearchTokenizer::parse"

namespace zbs {

using namespace std;
using namespace zbs;

int Line=1,Column=1,Offset=0;

/*

  # declaration

  set file /home/gerard/Development/gabu.zmf as GABU

  show dictionary

  # query

  find [all,first] GABU.<field name> = "string" and [not] GABU.<field name> = double.value or [not] GABU.<date>.month = 1 and GABU.<resource>.entity = ZEntity_Document and GABU.<resource>.id = 2

  find GABU.<resource field> = ZResource(ZEntity_Document,1)

  find  GABU.<date> = "12/12/1959"

  find <numeric field> = 12 + 1
  find <numeric field> = <numeric field> + 2.2

  find int(<double field>) = 10000

  find all <string field> starts with "gabu"

  find all <string field> contains "gabu"

  find all right(<string field>,4) = "gabu"

  find all left(<string field>,4) = "gabu"

  find all substring(<string field>,10,4) = "gabu"

  find all <uristring>.extension = "h"

  find all <uristring>.basename contains "gabu"

  find index="index primary key" (<resource field> = ZResource(ZEntity_Document,1))
  find index=0 (<resource field> = ZResource(ZEntity_Document,1))

  # update

  for GABU.index=0 GABU.<resource field> = ZResource(ZEntity_Document,1) set GABU.<string field>="gabu"

  for all GABU.index=0 (GABU.<resource field> = ZResource(ZEntity_Document,1)) and GABU.<date field>.month = 10 set GABU.<numeric field=1000



*/


bool
compareKeywordCase(utf8_t* pPtr,utf8_t* pKeyword,size_t& pLen) {
  pLen=0;
  while (*pKeyword && *pPtr) {
    int wC1 = utfUpper(*pPtr);
    int wC2 = utfUpper(*pKeyword);
    if (wC1 != wC2)
      return false;
    pLen++;
    pKeyword++;
    pPtr++;
  }
  if (*pKeyword!=0)
    return false;
  if (*pPtr!=0)
    if (std::isalnum(*pPtr))
      return false;
  return true;
}

bool
compareKeyword(utf8_t* pPtr,utf8_t* pKeyword,size_t& pLen) {
  pLen=0;
  while (*pKeyword && *pPtr) {
    if (*pPtr != *pKeyword)
      return false;
    pLen++;
    pKeyword++;
    pPtr++;
  }
  if (*pKeyword!=0)
    return false;
  if (*pPtr!=0)
    if (std::isalnum(*pPtr))
      return false;
  return true;
}



void
ZSearchTokenizer::parse(const utf8VaryingString &pIn)
{
  ZSearchToken* wCurrentToken=nullptr;
  int* wTLine=&Line;   // debug
  int* wTCol=&Column;  // dito
  int* wTOffset=&Offset; // dito

  Line = 1;
  Column = 1;
  Offset=0;

  size_t wLen=0;

  wCurrentToken = new ZSearchToken(this);

  for (long wi=0; wi < pIn.UnitCount ; wi++,Column++,Offset++) {
    char wCurrCh = pIn[wi];
/*
    if (wCurrentToken->Type == ZSRCH_SPACE) {
      if (searchForKeyword(wCurrentToken,pIn,wi)) {
        endToken(wCurrentToken);
        continue;
      }
    }
*/
    switch (wCurrCh) {
    case 0:
      continue;

    case '0':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        endToken(wCurrentToken);

        if ((pIn[wi+1]=='x')||(pIn[wi+1]=='X')){
          wCurrentToken->Type = ZSRCH_HEXA_LITERAL;
          wCurrentToken->Text.addUtfUnit(wCurrCh);
          wCurrentToken->Text.addUtfUnit(pIn[wi+1]);
          Column++;
          Offset++;
          wi++;
          continue;
        }
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type = ZSRCH_NUMERIC_LITERAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        Column++;
        Offset++;
        wi++;
        continue;
      } // ZSRCH_SPACE
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL){ /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        endToken(wCurrentToken);

        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type = ZSRCH_NUMERIC_LITERAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case '.':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        endToken(wCurrentToken);

        wCurrentToken->Type = ZSRCH_MAYBE_DOUBLE_LITERAL; /* double as .125 for instance */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_NUMERIC_LITERAL) {
        wCurrentToken->Type = ZSRCH_DOUBLE_LITERAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
/*
      if (wCurrentToken->Type == ZSRCH_MAYBE_INCLUDE_FILE) {
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
*/
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_DOT;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;


    case 't':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type==ZSRCH_SPACE) {
        endToken(wCurrentToken);

        if (utfStrncasecmp(pIn.Data+wi,(utf8_t*)"true ",5)==0) {
          wCurrentToken->setCoords(Line,Column,Offset);
          wCurrentToken->Type = ZSRCH_BOOL_LITERAL_TRUE;
          wCurrentToken->Text="true";
          endToken(wCurrentToken);
          Column+=4;
          Offset+=4;
          wi+=4;
          continue;
        }
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;
    case 'T':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type==ZSRCH_SPACE) {
        endToken(wCurrentToken);

        if (utfStrncasecmp(pIn.Data+wi,(utf8_t*)"TRUE ",5)==0) {
          wCurrentToken->setCoords(Line,Column,Offset);
          wCurrentToken->Type = ZSRCH_BOOL_LITERAL_TRUE;
          wCurrentToken->Text="TRUE";
          endToken(wCurrentToken);
          Column+=4;
          Offset+=4;
          wi+=4;
          continue;
        }
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;


    case '{':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPENBRACE;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '}':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_CLOSEBRACE;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case ';':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_SEMICOLON;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case ':':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL){ /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }

      if (pIn[wi+1]==':') {
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type = ZSRCH_DOUBLECOLON;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        wi++;
        Column++;
        Offset++;
        wCurrentToken->Text.addUtfUnit(pIn[wi]);
        endToken(wCurrentToken);
        continue;
      }

      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_COLON;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '(':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
/*
      if (wCurrentToken->Type != ZSRCH_SPACE) {
        wCurrentToken->Type=ZSRCH_METHOD;
        endToken(wCurrentToken);
      }
      else {
        if (last()->Type == ZSRCH_IDENTIFIER) {
          last()->Type=ZSRCH_METHOD;
        }
      }// else
*/
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPENPARENTHESIS;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);

      //                 getMethodArguments(wi,pIn);
      continue;


    case ')':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL){ /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_CLOSEPARENTHESIS;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '=':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);

      if (pIn[wi+1]=='='){
        wCurrentToken->Type = ZSRCH_OPERATOR_EQUAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        wCurrentToken->Text.addUtfUnit(pIn[wi+1]);
        Column++;
        Offset++;
        wi++;
        endToken(wCurrentToken);
        continue;
      }

      wCurrentToken->Type = ZSRCH_OPERATOR_EQUAL;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

/* Arithmetic operators */

    case '+':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPERATOR_PLUS;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '-':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPERATOR_MINUS;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '^':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPERATOR_POWER;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '*':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL){ /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }

      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);

      if (pIn[wi+1]=='/') //  */
      {
        wi++;
        Column++;
        Offset++;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        wCurrentToken->Text.addUtfUnit(pIn[wi]);
        wCurrentToken->Type = ZSRCH_COMMENT_ENDSLASHSTAR ;
        endToken(wCurrentToken);
        continue;
      }

      wCurrentToken->Type = ZSRCH_OPERATOR_MULTIPLY ;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '/':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_NUMERIC_LITERAL) {
        wCurrentToken->Type = ZSRCH_MAYBE_DATE_LITERAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_MAYBE_IDENTIFIER) {
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type = ZSRCH_OPERATOR_DIVIDEORSLASH;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        endToken(wCurrentToken);
        continue;
      }

      if (wCurrentToken->Type == ZSRCH_SPACE) {
        switch (pIn[wi+1]) {
        case '/':  /* double slash comment : till end of line */
          wCurrentToken->setCoords(Line,Column-1,Offset-1);

          wCurrentToken->Type = ZSRCH_COMMENT_DOUBLESLASH;
          /* get all comment string till end of line */
          /* skip leading slash */
          wi++;
          Column++;
          Offset++;
          wi++;
          Column++;
          Offset++;

          if (pIn[wi]=='/') { /* triple slash comment ? */
            /* skip third slash */
            wi++;
            Column++;
            Offset++;
          }
          parseComment(pIn,wCurrentToken,wi);
          endToken(wCurrentToken);
          continue;

        case '*': /* slash star comment : potential multi line comment */
          wCurrentToken->Type = ZSRCH_COMMENT_SLASHSTAR;
          wCurrentToken->setCoords(Line,Column-1,Offset-1);
          /* get all comment - potentially multi-lines */
          /* skip star */
          wi++;
          Column++;
          Offset++;
          wi++;
          Column++;
          Offset++;

          if (pIn[wi]=='*') { /* slash double star comment ? */
            /* skip second star */
            wi++;
            Column++;
            Offset++;
          }
          parseComment(pIn,wCurrentToken,wi);
          endToken(wCurrentToken);
          continue;

        default :
          wCurrentToken->setCoords(Line,Column,Offset);
          wCurrentToken->Type = ZSRCH_OPERATOR_DIVIDEORSLASH;
          wCurrentToken->Text.addUtfUnit(wCurrCh);
          endToken(wCurrentToken);
          continue;
        }//switch (pIn[wi+1])
      }//ZSRCH_SPACE
      endToken(wCurrentToken);
      continue;

    case '%':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) {
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPERATOR_MODULO;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

/* logical operators */

    case '<':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) {
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      /* true < logical operator */
      if (pIn[wi+1]=='=') {
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type = ZSRCH_OPERATOR_LESSOREQUAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        wi++;
        Column++;
        Offset++;
        wCurrentToken->Text.addUtfUnit(pIn[wi]);
        endToken(wCurrentToken);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPERATOR_LESS;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '>':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }


      /*  >= operator */
      if (pIn[wi+1]=='=') {
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type = ZSRCH_OPERATOR_GREATEROREQUAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        wi++;
        Column++;
        Offset++;
        wCurrentToken->Text.addUtfUnit(pIn[wi]);
        endToken(wCurrentToken);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPERATOR_LOGICAL;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '&':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPERATOR_AND  ;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '|':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_OPERATOR_OR  ;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;


    case 'a':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit('A');
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case 'A':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {

        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;
    case 'b':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL){ /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit('B');
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        if (compareKeyword(&pIn.Data[wi],(utf8_t*)"bool",wLen)) {
          wCurrentToken->setCoords(Line,Column,Offset);
          wCurrentToken->Type = ZSRCH_BOOL;
          wCurrentToken->Text = "bool";
          wi += wLen;
          Column+=wLen;
          Offset+=wLen;
          //                     endToken(wCurrentToken);
          continue;
        }
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;
    case 'B':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        if (compareKeyword(&pIn.Data[wi],(utf8_t*)"BOOL",wLen)) {
          wCurrentToken->setCoords(Line,Column,Offset);
          wCurrentToken->Type = ZSRCH_BOOL;
          wCurrentToken->Text = "BOOL";
          wi += wLen;
          Column+=wLen;
          Offset+=wLen;
          //                     endToken(wCurrentToken);
          continue;
        }
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case 'c':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit('C');
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }//ZSRCH_SPACE

      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case 'C':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }//ZSRCH_SPACE

      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;


    case 'd':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit('D');
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }//ZSRCH_SPACE

      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case 'D':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit('D');
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }//ZSRCH_SPACE

      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case 'e':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL){ /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit('E');
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }//ZSRCH_SPACE

      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case 'E':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL){ /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit('E');
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }//ZSRCH_SPACE

      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case 'F':
    case 'f':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL){ /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_HEXA_LITERAL) {
        wCurrentToken->Text.addUtfUnit('F');
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_DOUBLE_LITERAL) {
        wCurrentToken->Type = ZSRCH_FLOAT_LITERAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        endToken(wCurrentToken);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }//ZSRCH_SPACE

      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case ',':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_COMMA;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case '#':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_MAYBE_PREPROC;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;

    case ' ':
    case '\t':

      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      continue;

    case '\r':
    case '\n':
      if (wCurrentToken->Type == ZSRCH_COMMENT_SLASHSTAR) { /* multi line comment : store and continue */
        wCurrentToken->newLine();
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->newLine();
      continue;

    case '"':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* end of string litteral */
        endToken(wCurrentToken);
        continue;
      }

      if (last()->Type==ZSRCH_INCLUDE) { /*  include file spec */
        wCurrentToken->Type = ZSRCH_INCLUDE_FILE;
        wCurrentToken->setCoords(Line,Column,Offset);
        /* get include file definition */
        wCurrentToken->setCoords(Line,Column,Offset);
        wi++; // skip '"' sign
        while ((wi < pIn.getUnitCount())&&(pIn[wi] != '"')) {
          wCurrCh = pIn[wi];
          if (wCurrCh=='\n') {
            ErrorLog.infoLog("%s-E-INVINC Line %ld column %ld Invalid #include clause %s\n",__NAME__,wCurrentToken->Text.toCChar());
            wCurrentToken->newLine();
            break;
          }//if (wCurrCh=='\n')
          wCurrentToken->Text.addUtfUnit(pIn[wi]);
          wi++;
          Column++;
          Offset++;
        }//while
        endToken(wCurrentToken);
        continue;
      }//if (_Base::last()->Type==ZSRCH_INCLUDE)

      /* if not yet string literal, then start one */
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);
      wCurrentToken->Type = ZSRCH_STRING_LITERAL;
      /* String literal is starting : get all string content till <"> and continue */
      wi++; /* skip <*> */
      Column++;
      Offset++;
      wCurrCh = pIn[wi];
      while ((pIn[wi]!='"')&&(wi<pIn.getUnitCount())) {
        if (wCurrCh=='\n')
          wCurrentToken->newLine();
        wCurrentToken->Text.addUtfUnit(pIn[wi]);
        wi++;
        Column++;
        Offset++;
      }//while
      wCurrCh = pIn[wi];
      endToken(wCurrentToken);
      continue;


    case '\\':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) {
        //                        wCurrentToken->Type = ZSRCH_STRING_ESCAPE_SEQUENCE;
        wCurrentToken->setCoords(Line,Column,Offset);
        continue;
      }
#ifdef __COMMENT
      if (wCurrentToken->Type == ZSRCH_MAYBE_INCLUDE_FILE) {  /* this is relevant for windows  */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
#endif // __COMMENT__
      endToken(wCurrentToken);
      wCurrentToken->Type = ZSRCH_BACKSLASH;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;


    case '[':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_MAYBE_IDENTIFIER) {
        wCurrentToken->Type=ZSRCH_ARRAY_IDENTIFIER;
        endToken(wCurrentToken);
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);

      wCurrentToken->Type = ZSRCH_OPENBRACKET  ;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;

    case ']':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      endToken(wCurrentToken);
      wCurrentToken->setCoords(Line,Column,Offset);

      wCurrentToken->Type = ZSRCH_CLOSEBRACKET  ;
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      endToken(wCurrentToken);
      continue;


    case 'l':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }
      if (wCurrentToken->Type == ZSRCH_SPACE) {
        if (!memcmp(&pIn.Data[wi],"long ",5)) {
          wCurrentToken->setCoords(Line,Column,Offset);
          wCurrentToken->Type = ZSRCH_LONG;
          wCurrentToken->Text = "long";
          wi += 3;
          Column+=3;
          Offset+=3;
          //                     endToken(wCurrentToken);
          continue;
        }
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }//ZSRCH_SPACE
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;
    case 'L':
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }

      if (wCurrentToken->Type == ZSRCH_DOUBLE_LITERAL) {
        wCurrentToken->Type = ZSRCH_LONGDOUBLE_LITERAL;
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        endToken(wCurrentToken);
        continue;
      }
      if (wCurrentToken->Type==ZSRCH_SPACE) {
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }
      wCurrentToken->Text.addUtfUnit(wCurrCh);
      continue;



    default:
      if ((wCurrentToken->Type & ZSRCH_STRING_LITERAL)==ZSRCH_STRING_LITERAL) { /* type is string literal OR comment */
        wCurrentToken->Text.addUtfUnit(wCurrCh);
        continue;
      }

      if ((wCurrentToken->Type & ZSRCH_NUMERIC_LITERAL)==ZSRCH_NUMERIC_LITERAL) {
        endToken(wCurrentToken);
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_SPACE;
      }
      if (wCurrentToken->Type==ZSRCH_SPACE)
      {
        wCurrentToken->setCoords(Line,Column,Offset);
        wCurrentToken->Type=ZSRCH_MAYBE_IDENTIFIER;
      }
//      wCurrentToken->Text.addUtfUnit(wCurrCh);
//      continue;
      break;
    }//switch (currCh)

    wCurrentToken->Text.addUtfUnit(wCurrCh);
  }//for (char currCh : inProgram)

  if (wCurrentToken!=nullptr)
    endToken(wCurrentToken);

  if (wCurrentToken!=nullptr)
    delete wCurrentToken;
  wCurrentToken = nullptr;

  _print( "______________________________________________________\n"
         " Tokenizer token parsing report - ended at %s\n"
         " parsed text lines %d\n"
         " token found       %d\n\n"
         " warnings          %d\n"
         " errors            %d\n"
         "_______________________________________________________",
      ZTime::currentTime().toString().toCChar(),
      Line-1,count(),ErrorLog.countWarnings(),ErrorLog.countErrors());


//  if (Options & ZSRCHO_Report)
//    report();
  return ;
}//Tokenizer::parse


void ZSearchTokenizer::endCommentToken(ZSearchToken* &pCommentToken) {

  if (pCommentToken==nullptr)
    return;
  switch (pCommentToken->Type) {
  case ZSRCH_COMMENT_BRIEF:
    push(pCommentToken);
    if (Options & ZSRCHO_Verbose )
      pCommentToken->display(0);

    pCommentToken = new ZSearchToken(this);
    pCommentToken->Type = ZSRCH_COMMENT_BRIEF_IDENTIFIER;
    pCommentToken->setCoords(Line,Column,Offset);
    return;
  case ZSRCH_COMMENT_BRIEF_IDENTIFIER:
    push(pCommentToken);
    if (Options & ZSRCHO_Verbose )
      pCommentToken->display(0);

    pCommentToken = new ZSearchToken(this);
    pCommentToken->Type = ZSRCH_COMMENT_TEXT;
    pCommentToken->setCoords(Line,Column,Offset);
    return;
  case ZSRCH_COMMENT_PREVIOUS:
    push(pCommentToken);
    if (Options & ZSRCHO_Verbose )
      pCommentToken->display(0);
    pCommentToken = new ZSearchToken(this);
    pCommentToken->Type = ZSRCH_COMMENT_TEXT;
    pCommentToken->setCoords(Line,Column,Offset);
    return;
  default:
    if (Options & ZSRCHO_Verbose )
      pCommentToken->display(0);
    push(pCommentToken);
    pCommentToken = nullptr;
    return;
  }

  if (Options & ZSRCHO_Verbose )
    pCommentToken->display(0);
  push(pCommentToken);
  pCommentToken = nullptr;
  return;
} // endCommentToken

void ZSearchTokenizer::endToken(ZSearchToken *&pToken) {
  if ((pToken->Type & ZSRCH_MAYBE_COMMENT)==ZSRCH_MAYBE_COMMENT) {
    ErrorLog.infoLog("ZSearchTokenizer::endToken line %4d col.%3d Ignoring comment <%s>\n",
        pToken->TokenLine,
        pToken->TokenColumn,
        pToken->Text.toCChar());
    pToken->reset();
    return;
  }
  if (pToken->Type ==ZSRCH_SPACE) {
    ErrorLog.infoLog("ZSearchTokenizer::endToken line %4d col.%3d Ignoring space token\n",
        pToken->TokenLine,
        pToken->TokenColumn,
        pToken->Text.toCChar());
    pToken->reset();
    return;
  }

  while (true) {
    if (pToken->Type == ZSRCH_MAYBE_DOUBLE_LITERAL) {
      if (pToken->Text == ".")
        pToken->Type = ZSRCH_DOUBLE_LITERAL;
      else
        pToken->Type = ZSRCH_OPERATOR;
      break;
    }
    if (pToken->Type == ZSRCH_MAYBE_IDENTIFIER) {
      pToken->Type = ZSRCH_IDENTIFIER;
      break;
    }//ZSRCH_MAYBE_IDENTIFIER
/*
    if (pToken->Type == ZSRCH_MAYBE_STRUCT) {
      if (pToken->Text == "struct") {
        pToken->Type = ZSRCH_STRUCT;
        break;
      }
      pToken->Type = ZSRCH_IDENTIFIER ;
      break;
    }//ZSRCH_MAYBE_STRUCT
*/
    if (pToken->Type==ZSRCH_DOUBLE    ) {
      if (pToken->Text!="double")
        pToken->Type=ZSRCH_IDENTIFIER;
      break;
    }

    if (pToken->Type==ZSRCH_MAYBE_PREPROC) {
      if (pToken->Text == "#include"){
        pToken->Type = ZSRCH_INCLUDE;
        break;
      }
      pToken->Type = ZSRCH_SIMPLE_TOKEN;
      break;
    }//ZSRCH_MAYBE_PREPROC


    if (pToken->Type==ZSRCH_MAYBE_INCLUDE_FILE) {
      pToken->Type=ZSRCH_INCLUDE_FILE;
      break;
    }
    if (pToken->Type==ZSRCH_MAYBE_IDENTIFIER) {
      pToken->Type=ZSRCH_IDENTIFIER;
      break;
    }
    break;
  } // while true;

  if (pToken->Type != ZSRCH_SPACE) {
    _Base::push(pToken);

    if (Options & ZSRCHO_Verbose )
      pToken->displayNoindex();

    pToken = new ZSearchToken(this);
    pToken->Type = ZSRCH_SPACE;
    pToken->Text.clear();
  }

}// CppTokenizer::endToken

void ZSearchTokenizer::advance(long &pTokenIndex,int pInc)
{
  if (pInc != 0) {
    pTokenIndex += pInc;
    Column+= pInc;
    Offset+= pInc;
    return;
  }
  pTokenIndex++;
  Column++;
  Offset++;
}// ZSearchTokenizer::advance

void ZSearchTokenizer::parseComment(const utf8VaryingString &pIn,
    ZSearchToken* pCurrentToken,
    long &pTokenIndex)
{
  bool wTheClassOrEnum=false;
  ZSearchToken* wCommentToken=nullptr;
  utf8_t *wPtr = pIn.Data+pTokenIndex;

  if (pIn[pTokenIndex]=='<') {
    pCurrentToken->Text.addUtfUnit(pIn[pTokenIndex]);
    wCommentToken=new ZSearchToken(this);
    wCommentToken->Type = ZSRCH_COMMENT_PREVIOUS;
    wCommentToken->setCoords(Line,Column,Offset);
    wCommentToken->Text.addUtfUnit(pIn[pTokenIndex]);
    endCommentToken(wCommentToken);
    advance(pTokenIndex);
  }

  for (; pTokenIndex < pIn.UnitCount ; advance(pTokenIndex)) {
    wPtr = pIn.Data+pTokenIndex;
    char wCurrCh = pIn[pTokenIndex];
    switch (wCurrCh) {
    case 0:
      continue;
    case ' ':
      if (wCommentToken!=nullptr) {
        if (wCommentToken->Type==ZSRCH_COMMENT_BRIEF_IDENTIFIER) {
          endCommentToken(wCommentToken);
          break;
        }
      }
      break;

    case '*':
      if ((pCurrentToken->Type == ZSRCH_COMMENT_SLASHSTAR)&&(pIn[pTokenIndex+1]=='/')) {
        endCommentToken(wCommentToken);
        advance(pTokenIndex,2);
        return;
      }
      break;
    case '\n':
      endCommentToken(wCommentToken);
      pCurrentToken->newLine();
      if (pCurrentToken->Type==ZSRCH_COMMENT_DOUBLESLASH) {
        pCurrentToken->Text.addUtfUnit(wCurrCh);
        if (wCommentToken!=nullptr)  {
          endCommentToken(wCommentToken);
        }
        return;
      }//ZSRCH_COMMENT_DOUBLESLASH
      break;
    case '\\':
    case '@':
      pCurrentToken->Text.addUtfUnit(wCurrCh);
      advance(pTokenIndex);
      if (!memcmp(pIn.Data+pTokenIndex,"brief ",6) || !memcmp(pIn.Data+pTokenIndex,"short ",6)) {
        wTheClassOrEnum=false;
        pCurrentToken->Text.nadd(pIn.Data+pTokenIndex,6); /* pCurrentToken->Text contains the whole comment text */
        endCommentToken(wCommentToken); /* close if necessary (in case of : must be nullptr )  */

        pCurrentToken->Text.addUtfUnit(0);

        wCommentToken=new ZSearchToken(this);
        wCommentToken->Type=ZSRCH_COMMENT_BRIEF;
        wCommentToken->Text.strnset(pIn.Data+pTokenIndex,5);
        wCommentToken->setCoords(Line,Column,Offset);

        endCommentToken(wCommentToken);
        advance(pTokenIndex,5);
        continue;
      }
      break;
    case 't':
    case 'T': {      
      if (wCommentToken!=nullptr)  {
        if (wCommentToken->Type==ZSRCH_COMMENT_BRIEF_IDENTIFIER) {
          utf8_t* wPtr = pIn.Data+pTokenIndex;
          if (utfStrncasecmp(pIn.Data+pTokenIndex,(utf8_t*)"the ",4)==0) {
            wTheClassOrEnum=true;
            pCurrentToken->Text.strnset(pIn.Data+pTokenIndex,4);
            advance(pTokenIndex,3);
            continue;
          }
        }
      }
      break;
    } // case 'T':
    case 'c':
    case 'C':
      if (wCommentToken!=nullptr)  {
        if (wCommentToken->Type==ZSRCH_COMMENT_TEXT) {
          if (wTheClassOrEnum) {
            if (utfStrncasecmp(pIn.Data+pTokenIndex,(utf8_t*)"class ",6)==0) {
              wTheClassOrEnum=false;
              pCurrentToken->Text.strnset(pIn.Data+pTokenIndex,6);
              advance(pTokenIndex,5);
              continue;
            }
          }
        }//ZSRCH_COMMENT_TEXT
      }
      break;
    case 'e':
    case 'E':
      if (wCommentToken!=nullptr)  {
        if (wCommentToken->Type==ZSRCH_COMMENT_TEXT) {
          if (wTheClassOrEnum) {
            if (utfStrncasecmp(pIn.Data+pTokenIndex,(utf8_t*)"enum ",5)==0) {
              wTheClassOrEnum=false;
              pCurrentToken->Text.strnset(pIn.Data+pTokenIndex,5);
              advance(pTokenIndex,4);
              continue;
            }
          }
        }//ZSRCH_COMMENT_TEXT
      }
      break;
    case 's':
    case 'S':
      if (wCommentToken!=nullptr)  {
        if (wCommentToken->Type==ZSRCH_COMMENT_TEXT) {
          if (utfStrncasecmp(pIn.Data+pTokenIndex,(utf8_t*)"struct ",6)==0) {
            if (wTheClassOrEnum) {
              wTheClassOrEnum=false;
              pCurrentToken->Text.strnset(pIn.Data+pTokenIndex,6);
              advance(pTokenIndex,5);
              continue;
            }
          }
        }//ZSRCH_COMMENT_TEXT
      }
      break;
    default:
      if (wCommentToken!=nullptr)  {
        if (wCommentToken->Type==ZSRCH_COMMENT_TEXT) {
          wTheClassOrEnum = false;
        }
      }
      break;
    }//switch (wCurrCh)

    if (wCommentToken) {
      wCommentToken->Text.addUtfUnit( wCurrCh);
    }
    pCurrentToken->Text.addUtfUnit( wCurrCh);
  }//for
}//ZSearchTokenizer::parseComment


void ZSearchTokenizer::_print(const char* pFormat,...) const
{
  utf8VaryingString wOut;
  va_list ap;
  va_start(ap, pFormat);
  wOut.vsnprintf(500,pFormat,ap);
  va_end(ap);
  if (_displayCallback==nullptr) {
    fprintf(Output,wOut.toCChar());
    fprintf(Output,"\n");
    std::cout.flush();
  }
  else
    _displayCallback(wOut);
}

void ZSearchTokenizer::_print(const utf8VaryingString& pOut) const
{
  if (_displayCallback==nullptr) {
    fprintf(Output,pOut.toCChar());
    fprintf(Output,"\n");
    std::cout.flush();
  }
  else
    _displayCallback(pOut);
}

}//zbs
