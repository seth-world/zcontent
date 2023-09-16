#include "zsearchparsertype.h"

#include "zsearchtoken.h"

namespace zbs {

using namespace std;
using namespace zbs;


ZArray<ZSearchKeyWord> KeywordList = {
    {"SET",ZSRCH_SET},
    {"FILE",ZSRCH_FILE},

    {"MODE",ZSRCH_MODE},
    {"READONLY",ZSRCH_READONLY},
    {"MODIFY",ZSRCH_MODIFY},

    {"FINISH",ZSRCH_FINISH},

    {"FIND",ZSRCH_FIND},
    {"ALL",ZSRCH_ALL},
    {"FIRST",ZSRCH_FIRST},
    {"LAST",ZSRCH_LAST},
    {"AT",ZSRCH_AT},

    {"WITH",ZSRCH_WITH},
    {"AS",ZSRCH_AS},

    {"INDEX",ZSRCH_INDEX},
    {"FOR",ZSRCH_FOR},

    {"SHOW",ZSRCH_SHOW},
    {"CLEAR",ZSRCH_CLEAR},
    {"HISTORY",ZSRCH_HISTORY},

    {"ENTITY",ZSRCH_ENTITY},
    {"ENTITIES",ZSRCH_ENTITY},

    {"SYMBOL",ZSRCH_SYMBOL},
    {"SYMBOLS",ZSRCH_SYMBOL},

    {"MAXIMUM",ZSRCH_MAXIMUM},

    {"DISPLAY",ZSRCH_DISPLAY},


    {"AND",ZSRCH_OPERATOR_AND},
    {"OR",ZSRCH_OPERATOR_OR},


    {"CONTAINS",ZSRCH_CONTAINS},
    {"STARTS",ZSRCH_MAYBE_STARTS_WITH},
    {"ENDS",ZSRCH_MAYBE_ENDS_WITH},
    {"SUBSTRING",ZSRCH_SUBSTRING},


    {"YEAR",ZSRCH_YEAR},
    {"MONTH",ZSRCH_MONTH},
    {"DAY",ZSRCH_DAY},
    {"HOUR",ZSRCH_HOUR},
    {"MIN",ZSRCH_MIN},
    {"SEC",ZSRCH_SEC},

    {"ZRESOURCE",ZSRCH_RESOURCE_LITERAL},

    {"ZENTITY",ZSRCH_ZENTITY},
    {"ID",ZSRCH_ID},

    {"ZDATE",ZSRCH_ZDATE_LITERAL},

    {"CHECKSUM",ZSRCH_CHECKSUM_LITERAL},
    {"MD5",ZSRCH_MD5_LITERAL},

    /* uriString modifiers */

    {"PATH",ZSRCH_PATH},
    {"EXTENSION",ZSRCH_EXTENSION},
    {"BASENAME",ZSRCH_BASENAME},
    {"ROOTNAME",ZSRCH_ROOTNAME},

    {"INT",ZSRCH_INTEGER},
    {"INTEGER",ZSRCH_INTEGER}

};




char wSBuffer[150];
#ifdef __COMMENT__
const char*
_decode_TokenType(ZSearchTokentype_type pType)
{


  switch (pType)
  {
  case ZSRCH_SPACE:
    return "ZSRCH_SPACE";
  case ZSRCH_MAYBE_IDENTIFIER:
    return "ZSRCH_MAYBE_IDENTIFIER";
  case ZSRCH_IDENTIFIER:
    return "ZSRCH_IDENTIFIER";

  case ZSRCH_ARRAY_IDENTIFIER:
    return "ZSRCH_ARRAY_IDENTIFIER";


  case ZSRCH_IF:
    return "ZSRCH_IF";

  case ZSRCH_FIELD_IDENTIFIER:
    return "ZSRCH_FIELD_IDENTIFIER";
  case ZSRCH_FIELD_INITIALIZER:
    return "ZSRCH_FIELD_INITIALIZER";

  case ZSRCH_NUMERIC_LITERAL:
    return "ZSRCH_NUMERIC_LITERAL";
  case ZSRCH_INTEGER_LITERAL:
    return "ZSRCH_INTEGER_LITERAL";
  case ZSRCH_MAYBE_DOUBLE_LITERAL:
    return "ZSRCH_MAYBE_DOUBLE_LITERAL";
  case ZSRCH_DOUBLE_LITERAL:
    return "ZSRCH_DOUBLE_LITERAL";
  case ZSRCH_FLOAT_LITERAL:
    return "ZSRCH_FLOAT_LITERAL";
  case ZSRCH_LONGDOUBLE_LITERAL:
    return "ZSRCH_LONGDOUBLE_LITERAL";
  case ZSRCH_STRING_LITERAL:
    return "ZSRCH_STRING_LITERAL";
  case ZSRCH_STRING_ESCAPE_SEQUENCE:
    return "ZSRCH_STRING_ESCAPE_SEQUENCE";
  case ZSRCH_MAYBE_COMMENT:
    return "ZSRCH_MAYBE_COMMENT";
  case ZSRCH_MAYBE_ENDCOMMENT:
    return "ZSRCH_MAYBE_ENDCOMMENT";
  case ZSRCH_COMMENT_DOUBLESLASH:
    return "ZSRCH_COMMENT_DOUBLESLASH";
  case ZSRCH_COMMENT_SLASHSTAR:
    return "ZSRCH_COMMENT_SLASHSTAR";
  case ZSRCH_COMMENT_ENDSLASHSTAR:
    return "ZSRCH_COMMENT_ENDSLASHSTAR";

  case ZSRCH_COMMENT_BRIEF:
    return "ZSRCH_COMMENT_BRIEF";
  case ZSRCH_COMMENT_BRIEF_IDENTIFIER:
    return "ZSRCH_COMMENT_BRIEF_IDENTIFIER";
  case ZSRCH_COMMENT_PREVIOUS:
    return "ZSRCH_COMMENT_PREVIOUS";
  case ZSRCH_COMMENT_AFTER:
    return "ZSRCH_COMMENT_AFTER"  ;

  case ZSRCH_COMMENT_TEXT:
    return "ZSRCH_COMMENT_TEXT";

  case ZSRCH_OPERATOR:
    return "ZSRCH_OPERATOR";

  case ZSRCH_OPERATOR_ARITHMETIC:
    return "ZSRCH_OPERATOR_ARITHMETIC";



  case ZSRCH_OPERATOR_LOGICAL:


  case ZSRCH_OPENBRACE:
    return "ZSRCH_OPENBRACE";
  case ZSRCH_CLOSEBRACE:
    return "ZSRCH_CLOSEBRACE";
  case ZSRCH_OPENBRACKET:
    return "ZSRCH_OPENBRACKET";
  case ZSRCH_CLOSEBRACKET:
    return "ZSRCH_CLOSEBRACKET";
  case ZSRCH_OPENPARENTHESIS:
    return "ZSRCH_OPERATOR_OPENPARENTHESIS";
  case ZSRCH_CLOSEPARENTHESIS:
    return "ZSRCH_OPERATOR_CLOSEPARENTHESIS";
  case ZSRCH_SEMICOLON:
    return "ZSRCH_OPERATOR_SEMICOLON";
  case ZSRCH_COLON:
    return "ZSRCH_OPERATOR_COLON";
  case ZSRCH_COMMA:
    return "ZSRCH_OPERATOR_COMMA";

  case ZSRCH_EOL:
    return "ZSRCH_EOL";





  case ZSRCH_SIMPLE_TOKEN:
    return "ZSRCH_SIMPLE_TOKEN";
    /*
  case ZSRCH_EXPOSURE_TOKEN:
    return "ZSRCH_EXPOSURE_TOKEN";
  case ZSRCH_FRIEND_TOKEN:
    return "ZSRCH_FRIEND_TOKEN";

  case ZSRCH_DELETED_TOKEN:
    return "ZSRCH_DELETED_TOKEN";
  case ZSRCH_DEFAULT_TOKEN:
    return "ZSRCH_DEFAULT_TOKEN";

  case ZSRCH_TEMPLATE_TOKEN:
    return "ZSRCH_TEMPLATE_TOKEN";
*/


  case ZSRCH_FIELD:
    return "ZSRCH_FIELD";
  case ZSRCH_ARGUMENT:
    return "ZSRCH_ARGUMENT";

  case ZSRCH_MAYBE_DATA_TYPE:
    return "ZSRCH_MAYBE_DATA_TYPE";

  case ZSRCH_DATA_TYPE:
    return "ZSRCH_DATA_TYPE";

  case ZSRCH_VOID:
    return "ZSRCH_VOID";

  case ZSRCH_BOOL:
    return "ZSRCH_BOOL";

  case ZSRCH_CHAR:
    return "ZSRCH_CHAR";
  case ZSRCH_INTEGER:
    return "ZSRCH_INTEGER";
    /*  case ZSRCH_LONG:
        return "ZSRCH_LONG";
*/
  case ZSRCH_LONG_LONG:
    return "ZSRCH_LONG_LONG";
  case ZSRCH_FLOAT:
    return "ZSRCH_FLOAT";
  case ZSRCH_DOUBLE:
    return "ZSRCH_DOUBLE";
  case ZSRCH_LONG_DOUBLE:
    return "ZSRCH_LONG_DOUBLE";

    /*
  case ZSRCH_CONST:
    return "ZSRCH_CONST";
  case ZSRCH_UNSIGNED:
    return "ZSRCH_UNSIGNED";
  case ZSRCH_REFERENCE:
    return "ZSRCH_REFERENCE";
  case ZSRCH_POINTER:
    return "ZSRCH_POINTER";
 */
  case ZSRCH_INT8:
    return "ZSRCH_INT8";
  case ZSRCH_INT16:
    return "ZSRCH_INT16";
  case ZSRCH_INT32:
    return "ZSRCH_INT32";
  case ZSRCH_INT64:
    return "ZSRCH_INT64";

  case ZSRCH_MAYBE_METHOD:
    return "ZSRCH_MAYBE_METHOD";
  case ZSRCH_METHOD:
    return "ZSRCH_METHOD";
  case ZSRCH_METHOD_CTOR:
    return "ZSRCH_METHOD_CTOR";
  case ZSRCH_METHOD_DTOR:
    return "ZSRCH_METHOD_DTOR";
  case ZSRCH_OPERATOR_OVERLD:
    return "ZSRCH_OPERATOR_OVERLD";


  case ZSRCH_METHOD_ARGUMENT:
    return "ZSRCH_METHOD_ARGUMENT";


  case ZSRCH_MAYBE_PREPROC:
    return "ZSRCH_MAYBE_PREPROFULLC";
  case ZSRCH_INCLUDE:
    return "ZSRCH_INCLUDE";
  case ZSRCH_PRAGMA:
    return "ZSRCH_PRAGMA";
  case ZSRCH_IFDEF:
    return "ZSRCH_IFDEF";
  case ZSRCH_IFNDEF:
    return "ZSRCH_IFDEF";

  case ZSRCH_DEFINE:
    return "ZSRCH_DEFINE";
  case ZSRCH_ENDIF:
    return "ZSRCH_ENDIF";

  case ZSRCH_MAYBE_INCLUDE_FILE:
    return "ZSRCH_MAYBE_INCLUDE_FILE";
  case ZSRCH_INCLUDE_FILE:
    return "ZSRCH_INCLUDE_FILE";

  case ZSRCH_UTF8VARYINGSTRING:
    return "ZSRCH_UTF8VARYINGSTRING";
  case ZSRCH_UTF16VARYINGSTRING:
    return "ZSRCH_UTF16VARYINGSTRING";
  case ZSRCH_UTF32VARYINGSTRING:
    return "ZSRCH_UTF32VARYINGSTRING";
  case ZSRCH_URISTRING:
    return "ZSRCH_URISTRING";

  case ZSRCH_UTF8FIXEDSTRING:
    return "ZSRCH_UTF8FIXEDSTRING";
  case ZSRCH_UTF16FIXEDSTRING:
    return "ZSRCH_UTF16FIXEDSTRING";
  case ZSRCH_UTF32FIXEDSTRING:
    return "ZSRCH_UTF32FIXEDSTRING";

  case ZSRCH_ZDATEFULL:
    return "ZSRCH_ZDATEFULL";
    /*  case ZSRCH_ZDATE:
    return "ZSRCH_ZDATE";
*/
  case ZSRCH_MD5:
    return "ZSRCH_MD5";
  case ZSRCH_CHECKSUM:
    return "ZSRCH_CHECKSUM";

  case ZSRCH_ZDATABUFFER:
    return "ZSRCH_ZDATABUFFER";
  case ZSRCH_ZRESOURCE:
    return "ZSRCH_ZRESOURCE";
  case ZSRCH_ZRESOURCECODE:
    return "ZSRCH_ZRESOURCECODE";

  case ZSRCH_ZBITSET:
    return "ZSRCH_BITSET";

  case ZSRCH_STDSTRING:
    return "ZSRCH_STDSTRING";

  case ZSRCH_UNKNOWN_TYPE:
    return "ZSRCH_UNKNOWN_TYPE";
  case ZSRCH_SUBSTITUED:
    return "ZSRCH_SUBSTITUED";
/*
  default:
    memset(wSBuffer,0,sizeof(wSBuffer));

    sprintf(wSBuffer,"%s 0x%08X","unKnown Type",pType);
    return wSBuffer;
*/
  }
  memset(wSBuffer,0,sizeof(wSBuffer));
  sprintf(wSBuffer,"%s 0x%08lX","unKnown Type",pType);
  return wSBuffer;
}//_decode_TokenType

#endif // __COMMENT__

bool
searchForKeyword(ZSearchToken* &pCurrentToken,const utf8VaryingString &pIn,long &pCurrentIndex) {
  for (long wi=0; wi < KeywordList.count();wi++) {
    long wL= KeywordList[wi].Text.strlen();
    if (!KeywordList[wi].Text.ncompareCase(&pIn[pCurrentIndex],wL)) {
      if (utfIsAsciiChar(pIn[pCurrentIndex+wL]))
        continue;
      pCurrentToken->Text=KeywordList[wi].Text;
      pCurrentToken->Type = KeywordList[wi].Type;
      pCurrentIndex += wL;
      Column += wL;
      Offset += wL;

      return true;
    }
  }// for
  return false;
}// searchForKeyword



utf8VaryingString
decode_SearchTokenType(ZSearchTokentype_type pType)
{
  utf8VaryingString wReturn;


  for (int wi=0; wi < KeywordList.count();wi++) {
    if (pType==KeywordList[wi].Type) {
      ::sprintf(wSBuffer,"%s",KeywordList[wi].Text.toCChar());
      return wSBuffer;
    }
  }



  switch (pType)
  {
  case ZSRCH_OPERATOR_LOGICAL:
    return "ZSRCH_OPERATOR_LOGICAL";
  case ZSRCH_OPERATOR_AND:
    return "ZSRCH_OPERATOR_AND";
  case ZSRCH_OPERATOR_OR:
    return "ZSRCH_OPERATOR_OR";
  case ZSRCH_OPERATOR_NOT:
    return "ZSRCH_OPERATOR_NOT";

  case ZSRCH_OPERATOR_EQUAL:
    return "ZSRCH_OPERATOR_EQUAL";
  case ZSRCH_OPERATOR_NOTEQUAL:
    return "ZSRCH_OPERATOR_NOTEQUAL";
  case ZSRCH_OPERATOR_LESS:
    return "ZSRCH_OPERATOR_LESS";
  case ZSRCH_OPERATOR_GREATER:
    return "ZSRCH_OPERATOR_GREATER";
  case ZSRCH_OPERATOR_LESSOREQUAL:
    return "ZSRCH_OPERATOR_LESSOREQUAL";
  case ZSRCH_OPERATOR_GREATEROREQUAL:
    return "ZSRCH_OPERATOR_GREATEROREQUAL";

  case ZSRCH_OPERATOR_ARITHMETIC:
    return "ZSRCH_OPERATOR_ARITHMETIC";
  case ZSRCH_OPERATOR_PLUS:
    return "ZSRCH_OPERATOR_PLUS";
  case ZSRCH_OPERATOR_MINUS:
    return "ZSRCH_OPERATOR_MINUS";
  case ZSRCH_OPERATOR_DIVIDEORSLASH:
    return "ZSRCH_OPERATOR_DIVIDEORSLASH";
  case ZSRCH_OPERATOR_MULTIPLY:
    return "ZSRCH_OPERATOR_MULTIPLY";
  case ZSRCH_OPERATOR_POWER:
    return "ZSRCH_OPERATOR_POWER";
  case ZSRCH_OPERATOR_MODULO:
    return "ZSRCH_OPERATOR_MODULO";

  case ZSRCH_OPERATOR_MOVE:
    return "ZSRCH_OPERATOR_MOVE";

  case ZSRCH_LITERAL:
    return "ZSRCH_LITERAL";
  case ZSRCH_NUMERIC_LITERAL:
    return "ZSRCH_NUMERIC_LITERAL";
  case ZSRCH_HEXA_LITERAL:
    return "ZSRCH_HEXA_LITERAL";
  case ZSRCH_INTEGER_LITERAL:
    return "ZSRCH_INTEGER_LITERAL";
  case ZSRCH_INTEGER_LONG_LITERAL:
    return "ZSRCH_INTEGER_LONG_LITERAL";
  case ZSRCH_INTEGER_ULONG_LITERAL:
    return "ZSRCH_INTEGER_ULONG_LITERAL";
  case ZSRCH_DOUBLE_LITERAL:
    return "ZSRCH_DOUBLE_LITERAL";
  case ZSRCH_FLOAT_LITERAL:
    return "ZSRCH_FLOAT_LITERAL";
  case ZSRCH_LONGDOUBLE_LITERAL:
    return "ZSRCH_LONGDOUBLE_LITERAL";

  case ZSRCH_MAYBE_DATE_LITERAL:
    return "ZSRCH_MAYBE_DATE_LITERAL";

  case ZSRCH_DATE_LITERAL:
    return "ZSRCH_DATE_LITERAL";

  case ZSRCH_RESOURCE_LITERAL:
    return "ZSRCH_RESOURCE_LITERAL";

  case ZSRCH_CHECKSUM_LITERAL:
    return "ZSRCH_CHECKSUM_LITERAL";
  case ZSRCH_MD5_LITERAL:
    return "ZSRCH_MD5_LITERAL";

  case ZSRCH_BOOL_LITERAL:
    return "ZSRCH_BOOL_LITERAL";

  case ZSRCH_BOOL_LITERAL_TRUE:
    return "ZSRCH_BOOL_LITERAL_TRUE";
  case ZSRCH_BOOL_LITERAL_FALSE:
    return "ZSRCH_BOOL_LITERAL_FALSE";

  case ZSRCH_PATH_LITERAL:
    return "ZSRCH_PATH_LITERAL";

  case ZSRCH_STRING_LITERAL:
    return "ZSRCH_STRING_LITERAL";


  case ZSRCH_IDENTIFIER:
    return "ZSRCH_IDENTIFIER";
  case ZSRCH_MAYBE_IDENTIFIER:
    return "ZSRCH_MAYBE_IDENTIFIER";

     /* punctuation tokens */
  case ZSRCH_OPENBRACE:
    return "ZSRCH_OPENBRACE";
  case ZSRCH_CLOSEBRACE:
    return "ZSRCH_CLOSEBRACE";
  case ZSRCH_OPENBRACKET:
    return "ZSRCH_OPENBRACKET";
  case ZSRCH_CLOSEBRACKET:
    return "ZSRCH_CLOSEBRACKET";
  case ZSRCH_OPENPARENTHESIS:
    return "ZSRCH_OPENPARENTHESIS";
  case ZSRCH_CLOSEPARENTHESIS:
    return "ZSRCH_CLOSEPARENTHESIS";
  case ZSRCH_SEMICOLON:
    return "ZSRCH_SEMICOLON";
  case ZSRCH_COLON:
    return "ZSRCH_COLON";
  case ZSRCH_DOUBLECOLON:
    return "ZSRCH_DOUBLECOLON";
  case ZSRCH_COMMA:
    return "ZSRCH_COMMA";
  case ZSRCH_DOT:
    return "ZSRCH_DOT";

  case ZSRCH_EOL:
    return "ZSRCH_EOL";
  case ZSRCH_BACKSLASH:
    return "ZSRCH_BACKSLASH";

  /* search engine instructions */

  case ZSRCH_SET:
    return "ZSRCH_SET";
  case ZSRCH_FILE:
    return "ZSRCH_FILE";

  case ZSRCH_MODE:
    return "ZSRCH_MODE";
  case ZSRCH_READONLY:
    return "ZSRCH_READONLY";
  case ZSRCH_MODIFY:
    return "ZSRCH_AS";
  case ZSRCH_AS:

    return "ZSRCH_FINISH";
  case ZSRCH_FINISH:
    return "ZSRCH_OPENBRACKET";

  case ZSRCH_FIND:
    return "ZSRCH_FIND";
  case ZSRCH_WITH:
    return "ZSRCH_WITH";

  case ZSRCH_INDEX:
    return "ZSRCH_INDEX";
  case ZSRCH_INDEXNUM:
    return "ZSRCH_INDEXNUM";
  case ZSRCH_INDEXNAME:
    return "ZSRCH_INDEXNAME";


  case ZSRCH_FOR:
    return "ZSRCH_FOR";
  case ZSRCH_SHOW:
    return "ZSRCH_SHOW";
  case ZSRCH_ALL:
    return "ZSRCH_ALL";
  case ZSRCH_FIRST:
    return "ZSRCH_FIRST";

    /* string operators */

  case ZSRCH_CONTAINS:
    return "ZSRCH_CONTAINS";
  case ZSRCH_STARTS_WITH:
    return "ZSRCH_STARTS_WITH";
  case ZSRCH_ENDS_WITH:
    return "ZSRCH_ENDS_WITH";

    /* string modifiers */

  case ZSRCH_SUBSTRING:
    return "ZSRCH_SUBSTRING";

    /* Date modifiers */

  case ZSRCH_YEAR:
    return "ZSRCH_YEAR";
  case ZSRCH_MONTH:
    return "ZSRCH_MONTH";
  case ZSRCH_DAY:
    return "ZSRCH_DAY";
  case ZSRCH_HOUR:
    return "ZSRCH_HOUR";
  case ZSRCH_MIN:
    return "ZSRCH_MIN";
  case ZSRCH_SEC:
    return "ZSRCH_SEC";

    /* Resource modifiers */

  case ZSRCH_ZENTITY:
    return "ZSRCH_ZENTITY";
  case ZSRCH_ID:
    return "ZSRCH_ID";

    /* uristring modifiers */

  case ZSRCH_PATH:
    return "ZSRCH_PATH";
  case ZSRCH_EXTENSION:
    return "ZSRCH_EXTENSION";
  case ZSRCH_BASENAME:
    return "ZSRCH_BASENAME";
  case ZSRCH_ROOTNAME:
    return "ZSRCH_ROOTNAME";

  case ZSRCH_BOOL:
    return "ZSRCH_BOOL";

  case ZSRCH_INCLUDE:
    return "ZSRCH_INCLUDE";

  case ZSRCH_ZRESOURCE:
    return "ZSRCH_ZRESOURCE";

  default:
    return "unknown ZSRCH code";
  }

}

ZSearchTokentype_type encode_ZTokenType(const utf8VaryingString& pIn)
{
  ZSearchTokentype_type wType=0;
  /* modifiers */
  if (pIn.strstr((const utf8_t*)"ZSRCH_CONST")) {
    wType |= ZSRCH_CONST;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_UNSIGNED")) {
    wType |= ZSRCH_UNSIGNED;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_AMPERSAND")) {
    wType |= ZSRCH_AMPERSAND;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_DOUBLE_AMPERSAND")) {
    wType |= ZSRCH_DOUBLE_AMPERSAND;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_POINTER")) {
    wType |= ZSRCH_POINTER;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_ARRAY")) {
    wType |= ZSRCH_ARRAY;
  }

  if (pIn.strstr((const utf8_t*)"ZSRCH_LONG")) {
    wType |= ZSRCH_LONG;
  }

  /* return or data type */

  if (pIn.strstr((const utf8_t*)"ZSRCH_VOID")) {
    wType |=  ZSRCH_VOID;
    return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_BOOL")) {
    wType |= ZSRCH_BOOL; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_CHAR")) {
    wType |= ZSRCH_CHAR; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_INTEGER")) {
    wType |= ZSRCH_INTEGER; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_LONG_LONG")) {
    wType |= ZSRCH_LONG_LONG; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_FLOAT")) {
    wType |= ZSRCH_FLOAT; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_DOUBLE")) {
    wType |= ZSRCH_DOUBLE; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_LONG_DOUBLE")) {
    wType |= ZSRCH_LONG_DOUBLE; return wType ;
  }

  if (pIn.strstr((const utf8_t*)"ZSRCH_INT8")) {
    wType |= ZSRCH_INT8; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_INT16")) {
    wType |= ZSRCH_INT16; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_INT32")) {
    wType |= ZSRCH_INT32; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_INT64")) {
    wType |= ZSRCH_INT64; return wType ;
  }

  if (pIn.strstr((const utf8_t*)"ZSRCH_UTF8VARYINGSTRING")) {
    wType |= ZSRCH_UTF8VARYINGSTRING; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_UTF16VARYINGSTRING")) {
    wType |= ZSRCH_UTF16VARYINGSTRING; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_UTF32VARYINGSTRING")) {
    wType |= ZSRCH_UTF32VARYINGSTRING; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_URISTRING")) {
    wType |= ZSRCH_URISTRING; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_UTF8FIXEDSTRING")) {
    wType |= ZSRCH_UTF8FIXEDSTRING; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_UTF16FIXEDSTRING")) {
    wType |= ZSRCH_UTF16FIXEDSTRING; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_UTF32FIXEDSTRING")) {
    wType |= ZSRCH_UTF32FIXEDSTRING; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_ZDATEFULL")) {
    wType |= ZSRCH_ZDATEFULL; return wType ;
  }
  /*  if (pIn.strstr((const utf8_t*)"ZSRCH_ZDATE")) {
     wType |= ZSRCH_ZDATE; return wType ;
  }
*/
  if (pIn.strstr((const utf8_t*)"ZSRCH_MD5")) {
    wType |= ZSRCH_MD5; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_CHECKSUM")) {
    wType |= ZSRCH_CHECKSUM; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_ZDATABUFFER")) {
    wType |= ZSRCH_ZDATABUFFER; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_ZRESOURCECODE")) {
    wType |= ZSRCH_ZRESOURCECODE; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_ZRESOURCE")) {
    wType |= ZSRCH_ZRESOURCE; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_ZBITSET")) {
    wType |= ZSRCH_ZBITSET; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_STDSTRING")) {
    wType |= ZSRCH_STDSTRING; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_UNKNOWN_TYPE")) {
    return ZSRCH_UNKNOWN_TYPE;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_SUBSTITUED")) {
    return ZSRCH_SUBSTITUED;
  }
  /* not data type or return type */

  if (pIn.strstr((const utf8_t*)"ZSRCH_METHOD_ARGUMENT")) {
    wType |= ZSRCH_METHOD_ARGUMENT; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_INCLUDE")) {
    wType |= ZSRCH_INCLUDE; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_PRAGMA")) {
    wType |= ZSRCH_PRAGMA; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_IFDEF")) {
    wType |= ZSRCH_IFDEF; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_DEFINE")) {
    wType |= ZSRCH_DEFINE; return wType ;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_INCLUDE_FILE")) {
    wType |= ZSRCH_INCLUDE_FILE; return wType ;
  }

  if (pIn.strstr((const utf8_t*)"ZSRCH_OPENBRACE")) {
    wType |= ZSRCH_OPENBRACE;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_CLOSEBRACE")) {
    wType |= ZSRCH_CLOSEBRACE;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_OPENBRACKET")) {
    wType |= ZSRCH_OPENBRACKET;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_CLOSEBRACKET")) {
    wType |= ZSRCH_CLOSEBRACKET;
  }

  return ZSRCH_UNKNOWN_TYPE;
}

ZSearchOperatorValue
convert_ZSRCH_ZSOPV(ZSearchTokentype_type pType)
{
  switch (pType)
  {
  case ZSRCH_OPERATOR_NOT:
    return ZSOPV_NOT;
  case ZSRCH_OPERATOR_AND:
    return ZSOPV_AND;
  case ZSRCH_OPERATOR_OR:
    return ZSOPV_OR;

  case ZSRCH_OPERATOR_EQUAL:
    return ZSOPV_EQUAL;
  case ZSRCH_OPERATOR_NOTEQUAL:
    return ZSOPV_NOTEQUAL;
  case ZSRCH_OPERATOR_LESS:
    return ZSOPV_LESS;
  case ZSRCH_OPERATOR_GREATER:
    return ZSOPV_GREATER;
  case ZSRCH_OPERATOR_LESSOREQUAL:
    return ZSOPV_LESSEQUAL;
  case ZSRCH_OPERATOR_GREATEROREQUAL:
    return ZSOPV_GREATEREQUAL;

  case ZSRCH_OPERATOR_PLUS:
    return ZSOPV_PLUS;
  case ZSRCH_OPERATOR_MINUS:
    return ZSOPV_MINUS;
  case ZSRCH_OPERATOR_DIVIDEORSLASH:
    return ZSOPV_DIVIDE;
  case ZSRCH_OPERATOR_MULTIPLY:
    return ZSOPV_MULTIPLY;
  case ZSRCH_OPERATOR_POWER:
    return ZSOPV_POWER;
  case ZSRCH_OPERATOR_MODULO:
    return ZSOPV_MODULO;

  case ZSRCH_CONTAINS:
    return ZSOPV_CONTAINS;
  case ZSRCH_STARTS_WITH:
    return ZSOPV_STARTS_WITH;
  case ZSRCH_ENDS_WITH:
    return ZSOPV_ENDS_WITH;

  default:
    fprintf(stderr,"Invalid operator %s 0x%X",decode_SearchTokenType(pType).toCChar(),int(pType));
    return ZSOPV_INVALID;

  }// switch

}


ZSearchOperandType
ZSTOfromZType(ZTypeBase pZType) {
  ZSearchOperandType_type wZSTO=ZSTO_Nothing;

  switch (pZType) {
  case ZType_Utf8FixedString:
  case ZType_Utf16FixedString:
  case ZType_Utf32FixedString:
    return ZSTO_String;
  case ZType_Utf8VaryingString:
  case ZType_Utf16VaryingString:
  case ZType_Utf32VaryingString:
    return ZSTO_String;
  case ZType_UChar:
  case ZType_U8:
  case ZType_U16:
  case ZType_U32:
  case ZType_U64:
    return ZSTO_Integer;
  case ZType_Char:
  case ZType_S8:
  case ZType_S16:
  case ZType_S32:
  case ZType_S64:
    return ZSTO_Integer;

  case ZType_Float:
  case ZType_Double:
  case ZType_LDouble:
    return ZSTO_Float;

  case ZType_ZDateFull:
    return ZSTO_Date;
  case ZType_Resource:
    return ZSTO_Resource;
  case ZType_CheckSum:
    return ZSTO_Checksum;

  }

}

const char* decode_OperandType(ZSearchOperandType_type pType)
{
  switch (pType)
  {
  case ZSTO_FieldString:
    return "ZSTO_FieldString";
  case ZSTO_FieldInteger:
    return "ZSTO_FieldInteger";
  case ZSTO_FieldFloat:
    return "ZSTO_FieldFloat";
  case ZSTO_FieldDate:
    return "ZSTO_FieldDate";
  case ZSTO_FieldChecksum:
    return "ZSTO_FieldChecksum";
  case ZSTO_FieldResource:
    return "ZSTO_FieldResource";
  case ZSTO_FieldUriString:
    return "ZSTO_FieldUriString";
  case ZSTO_FieldBool:
    return "ZSTO_FieldBool";


  case ZSTO_LiteralString:
    return "ZSTO_LiteralString";
  case ZSTO_LiteralInteger:
    return "ZSTO_LiteralInteger";
  case ZSTO_LiteralFloat:
    return "ZSTO_LiteralFloat";
  case ZSTO_LiteralDate:
    return "ZSTO_LiteralDate";
  case ZSTO_LiteralChecksum:
    return "ZSTO_LiteralChecksum";
  case ZSTO_LiteralResource:
    return "ZSTO_LiteralResource";
  case ZSTO_LiteralUriString:
    return "ZSTO_LiteralUriString";
  case ZSTO_LiteralBool:
    return "ZSTO_LiteralBool";

  case ZSTO_ArithLiteralString:
    return "ZSTO_ArithLiteralString";
  case ZSTO_ArithLiteralInteger:
    return "ZSTO_ArithLiteralInteger";
  case ZSTO_ArithLiteralFloat:
    return "ZSTO_ArithLiteralFloat";
  case ZSTO_ArithLiteralDate:
    return "ZSTO_ExprLiteralDate";
  case ZSTO_ArithLiteralChecksum:
    return "ZSTO_ArithLiteralChecksum";
  case ZSTO_ArithLiteralResource:
    return "ZSTO_ArithLiteralResource";
  case ZSTO_ArithLiteralUriString:
    return "ZSTO_ArithLiteralUriString";
  case ZSTO_ArithLiteralBool:
    return "ZSTO_ArithLiteralBool";


  case ZSTO_ArithFieldString:
    return "ZSTO_ArithFieldString";
  case ZSTO_ArithFieldInteger:
    return "ZSTO_ArithFieldInteger";
  case ZSTO_ArithFieldFloat:
    return "ZSTO_ArithFieldFloat";
  case ZSTO_ArithFieldDate:
    return "ZSTO_ArithFieldDate";
  case ZSTO_ArithFieldChecksum:
    return "ZSTO_ArithFieldChecksum";
  case ZSTO_ArithFieldResource:
    return "ZSTO_ArithFieldResource";
  case ZSTO_ArithFieldUriString:
    return "ZSTO_ArithFieldUriString";
  case ZSTO_ArithFieldBool:
    return "ZSTO_ArithFieldBool";


  case ZSTO_Arithmetic:
    return "ZSTO_Expression";
  case ZSTO_Literal:
    return "ZSTO_Literal";
  case ZSTO_Field:
    return "ZSTO_Field";


  case ZSTO_String:
    return "ZSTO_String";
  case ZSTO_Integer:
    return "ZSTO_Integer";
  case ZSTO_Float:
    return "ZSTO_Float";
  case ZSTO_Date:
    return "ZSTO_Date";
  case ZSTO_Checksum:
    return "ZSTO_Checksum";
  case ZSTO_Resource:
    return "ZSTO_Resource";
  case ZSTO_UriString:
    return "ZSTO_UriString";
  case ZSTO_Bool:
    return "ZSTO_Bool";

  case ZSTO_Nothing:
    return "ZSTO_Nothing";

  default:
    return "invalid/unknown ZSTO";

  }// switch

} // decode_OperandType


const char* decode_ZSOPV(ZSearchOperator_type pOp)
{
  switch (pOp)
  {
  case ZSOPV_Nothing:
    return "ZSOPV_Nothing";
  case ZSOPV_NOT:
    return "ZSOPV_NOT";
  case ZSOPV_AND:
    return "ZSOPV_AND";
  case ZSOPV_OR:
    return "ZSOPV_OR";
  case ZSOPV_EQUAL:
    return "ZSOPV_EQUAL";
  case ZSOPV_NOTEQUAL:
    return "ZSOPV_NOTEQUAL";
  case ZSOPV_LESS:
    return "ZSOPV_LESS";
  case ZSOPV_GREATER:
    return "ZSOPV_GREATER";


  case ZSOPV_LESSEQUAL:
    return "ZSOPV_LESSEQUAL";
  case ZSOPV_GREATEREQUAL:
    return "ZSOPV_GREATEREQUAL";

  case ZSOPV_PLUS:
    return "ZSOPV_PLUS";
  case ZSOPV_MINUS:
    return "ZSOPV_MINUS";
  case ZSOPV_DIVIDE:
    return "ZSOPV_DIVIDE";
  case ZSOPV_MULTIPLY:
    return "ZSOPV_MULTIPLY";
  case ZSOPV_POWER:
    return "ZSOPV_POWER";
  case ZSOPV_MODULO:
    return "ZSOPV_MODULO";


  case ZSOPV_CONTAINS:
    return "ZSOPV_CONTAINS";
  case ZSOPV_STARTS_WITH:
    return "ZSOPV_STARTS_WITH";
  case ZSOPV_ENDS_WITH:
    return "ZSOPV_ENDS_WITH";


  case ZSOPV_INVALID:
    return "ZSOPV_INVALID";

  default:
    return "invalid/unknown Operator";

  }// switch

}


}//zbs

const char*
getParserWorkDirectory() {
  const char* wWD=getenv(__SEARCHPARSER_WORK_DIRECTORY__);
  if (!wWD)
    wWD="";
  return wWD;
}
const char*
getParserParamDirectory() {
  const char* wWD=getenv(__SEARCHPARSER_PARAM_DIRECTORY__);
  if (!wWD)
    wWD="";
  return wWD;
}
