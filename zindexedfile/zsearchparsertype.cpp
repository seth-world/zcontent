#include "zsearchparsertype.h"

#include "zsearchtoken.h"

namespace zbs {

using namespace std;
using namespace zbs;


ZArray<ZSearchKeyWord> KeywordList = {

    {"SET",ZSRCH_SET},
    {"DECLARE",ZSRCH_DECLARE},
    {"SAVE",ZSRCH_SAVE},
    {"LOAD",ZSRCH_LOAD},
    {"SYMBOL",ZSRCH_SYMBOL},
    {"FILE",ZSRCH_FILE},

    {"MODE",ZSRCH_MODE},
    {"READONLY",ZSRCH_READONLY},
    {"MODIFY",ZSRCH_MODIFY},

    {"FINISH",ZSRCH_FINISH},

    {"FIND",ZSRCH_FIND},
    {"FETCH",ZSRCH_FETCH},
    {"IN",ZSRCH_IN},
    {"ALL",ZSRCH_ALL},
    {"FIRST",ZSRCH_FIRST},
    {"NEXT",ZSRCH_NEXT},
    {"LAST",ZSRCH_LAST},
    {"AT",ZSRCH_AT},

    {"WITH",ZSRCH_WITH},
    {"AS",ZSRCH_AS},
    {"JOIN",ZSRCH_JOIN},
    {"USING",ZSRCH_USING},

    {"TO",ZSRCH_TO},
    {"FROM",ZSRCH_FROM},

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

    {"FORMULA",ZSRCH_FORMULA},
    {"FIELDS",ZSRCH_FIELDS},

    {"MASTERFILES",ZSRCH_MASTERFILES},
    {"FILES",ZSRCH_MASTERFILES},

    {"DISPLAY",ZSRCH_DISPLAY},


    {"AND",ZSRCH_OPERATOR_AND},
    {"OR",ZSRCH_OPERATOR_OR},


    {"CONTAINS",ZSRCH_CONTAINS},
    {"STARTS",ZSRCH_MAYBE_STARTS_WITH},
    {"ENDS",ZSRCH_MAYBE_ENDS_WITH},
    {"SUBSTRING",ZSRCH_SUBSTRING},
    {"RIGHT",ZSRCH_SUBSTRINGRIGHT},
    {"LEFT",ZSRCH_SUBSTRINGLEFT},


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
    {"INTEGER",ZSRCH_INTEGER} ,

    {"TRANSLATE",ZSRCH_TRANSLATE},
    {"DECODETABLE",ZSRCH_DECODETABLE},

    {"HELP",ZSRCH_HELP},

    {"DEFAULT",ZSRCH_DEFAULT},
    {"CURRENT",ZSRCH_CURRENT}

};




//char wSBuffer[150];


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

/*
  for (int wi=0; wi < KeywordList.count();wi++) {
    if (pType==KeywordList[wi].Type) {
      wReturn.sprintf("%s",KeywordList[wi].Text.toCChar());
      return wReturn;
    }
  }
*/


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
  case ZSRCH_RIGHTARROW:
      return "ZSRCH_RIGHTARROW";
  case ZSRCH_LEFTARROW:
      return "ZSRCH_LEFTARROW";

  case ZSRCH_EOL:
    return "ZSRCH_EOL";
  case ZSRCH_BACKSLASH:
    return "ZSRCH_BACKSLASH";

  /* search engine instructions */

  case ZSRCH_SET:
    return "ZSRCH_SET";

  case ZSRCH_HELP:
  return "ZSRCH_HELP";
  case ZSRCH_DECLARE:
      return "ZSRCH_DECLARE";
  case ZSRCH_SAVE:
      return "ZSRCH_SAVE";
  case ZSRCH_LOAD:
      return "ZSRCH_LOAD";

  case ZSRCH_FILE:
    return "ZSRCH_FILE";

  case ZSRCH_MODE:
    return "ZSRCH_MODE";
  case ZSRCH_READONLY:
    return "ZSRCH_READONLY";
  case ZSRCH_MODIFY:
    return "ZSRCH_MODIFY";
  case ZSRCH_AS:
     return "ZSRCH_AS";
  case ZSRCH_TO:
      return "ZSRCH_TO";
  case ZSRCH_FROM:
      return "ZSRCH_FROM";

   case ZSRCH_FINISH:
    return "ZSRCH_FINISH";


  case ZSRCH_FIND:
    return "ZSRCH_FIND";
  case ZSRCH_FETCH:
      return "ZSRCH_FETCH";

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


  case ZSRCH_IN:
      return "ZSRCH_IN";
  case ZSRCH_AT:
      return "ZSRCH_AT";

  case ZSRCH_JOIN:
      return "ZSRCH_JOIN";

  case ZSRCH_USING:
      return "ZSRCH_USING";

  case ZSRCH_DEFAULT :
      return "ZSRCH_DEFAULT";
  case ZSRCH_CURRENT :
      return "ZSRCH_CURRENT";


  /* modifiers */
  case ZSRCH_TRANSLATE:
      return "ZSRCH_TRANSLATE";
  case ZSRCH_DECODETABLE:
      return "ZSRCH_DECODETABLE";


    /* string modifiers */

  case ZSRCH_SUBSTRING:
      return "ZSRCH_SUBSTRING";
  case ZSRCH_SUBSTRINGRIGHT:
      return "ZSRCH_SUBSTRINGRIGHT";
  case ZSRCH_SUBSTRINGLEFT:
      return "ZSRCH_SUBSTRINGLEFT";

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
  if (pIn.hasToken((const utf8_t*)"ZSRCH_CONST")) {
    wType |= ZSRCH_CONST;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_UNSIGNED")) {
    wType |= ZSRCH_UNSIGNED;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_AMPERSAND")) {
    wType |= ZSRCH_AMPERSAND;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_DOUBLE_AMPERSAND")) {
    wType |= ZSRCH_DOUBLE_AMPERSAND;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_POINTER")) {
    wType |= ZSRCH_POINTER;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_ARRAY")) {
    wType |= ZSRCH_ARRAY;
  }

  if (pIn.hasToken((const utf8_t*)"ZSRCH_LONG")) {
    wType |= ZSRCH_LONG;
  }

  /* return or data type */

  if (pIn.hasToken((const utf8_t*)"ZSRCH_VOID")) {
    wType |=  ZSRCH_VOID;
    return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_BOOL")) {
    wType |= ZSRCH_BOOL; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_CHAR")) {
    wType |= ZSRCH_CHAR; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_INTEGER")) {
    wType |= ZSRCH_INTEGER; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_LONG_LONG")) {
    wType |= ZSRCH_LONG_LONG; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_FLOAT")) {
    wType |= ZSRCH_FLOAT; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_DOUBLE")) {
    wType |= ZSRCH_DOUBLE; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_LONG_DOUBLE")) {
    wType |= ZSRCH_LONG_DOUBLE; return wType ;
  }

  if (pIn.hasToken((const utf8_t*)"ZSRCH_INT8")) {
    wType |= ZSRCH_INT8; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_INT16")) {
    wType |= ZSRCH_INT16; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_INT32")) {
    wType |= ZSRCH_INT32; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_INT64")) {
    wType |= ZSRCH_INT64; return wType ;
  }

  if (pIn.hasToken((const utf8_t*)"ZSRCH_UTF8VARYINGSTRING")) {
    wType |= ZSRCH_UTF8VARYINGSTRING; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_UTF16VARYINGSTRING")) {
    wType |= ZSRCH_UTF16VARYINGSTRING; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_UTF32VARYINGSTRING")) {
    wType |= ZSRCH_UTF32VARYINGSTRING; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_URISTRING")) {
    wType |= ZSRCH_URISTRING; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_UTF8FIXEDSTRING")) {
    wType |= ZSRCH_UTF8FIXEDSTRING; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_UTF16FIXEDSTRING")) {
    wType |= ZSRCH_UTF16FIXEDSTRING; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_UTF32FIXEDSTRING")) {
    wType |= ZSRCH_UTF32FIXEDSTRING; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_ZDATEFULL")) {
    wType |= ZSRCH_ZDATEFULL; return wType ;
  }
  /*  if (pIn.hasToken((const utf8_t*)"ZSRCH_ZDATE")) {
     wType |= ZSRCH_ZDATE; return wType ;
  }
*/
  if (pIn.hasToken((const utf8_t*)"ZSRCH_MD5")) {
    wType |= ZSRCH_MD5; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_CHECKSUM")) {
    wType |= ZSRCH_CHECKSUM; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_ZDATABUFFER")) {
    wType |= ZSRCH_ZDATABUFFER; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_ZRESOURCECODE")) {
    wType |= ZSRCH_ZRESOURCECODE; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_ZRESOURCE")) {
    wType |= ZSRCH_ZRESOURCE; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_ZBITSET")) {
    wType |= ZSRCH_ZBITSET; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_STDSTRING")) {
    wType |= ZSRCH_STDSTRING; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_UNKNOWN_TYPE")) {
    return ZSRCH_UNKNOWN_TYPE;
  }
  if (pIn.strstr((const utf8_t*)"ZSRCH_SUBSTITUED")) {
    return ZSRCH_SUBSTITUED;
  }
  /* not data type or return type */

  if (pIn.hasToken((const utf8_t*)"ZSRCH_METHOD_ARGUMENT")) {
    wType |= ZSRCH_METHOD_ARGUMENT; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_INCLUDE")) {
    wType |= ZSRCH_INCLUDE; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_PRAGMA")) {
    wType |= ZSRCH_PRAGMA; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_IFDEF")) {
    wType |= ZSRCH_IFDEF; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_DEFINE")) {
    wType |= ZSRCH_DEFINE; return wType ;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_INCLUDE_FILE")) {
    wType |= ZSRCH_INCLUDE_FILE; return wType ;
  }

  if (pIn.hasToken((const utf8_t*)"ZSRCH_OPENBRACE")) {
    wType |= ZSRCH_OPENBRACE;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_CLOSEBRACE")) {
    wType |= ZSRCH_CLOSEBRACE;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_OPENBRACKET")) {
    wType |= ZSRCH_OPENBRACKET;
  }
  if (pIn.hasToken((const utf8_t*)"ZSRCH_CLOSEBRACKET")) {
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

  case ZSRCH_OPERATOR_MOVE:
      return ZSOPV_MOVE;

  default:
    fprintf(stderr,"convert_ZSRCH_ZSOPV-E-INVOP Invalid operator %s 0x%X\n",decode_SearchTokenType(pType).toCChar(),int(pType));
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
  case ZType_AtomicUChar:
  case ZType_AtomicU8:
  case ZType_AtomicU16:
  case ZType_AtomicU32:
  case ZType_AtomicU64:
    return ZSTO_Integer;
  case ZType_Char:
  case ZType_S8:
  case ZType_S16:
  case ZType_S32:
  case ZType_S64:
  case ZType_AtomicChar:
  case ZType_AtomicS8:
  case ZType_AtomicS16:
  case ZType_AtomicS32:
  case ZType_AtomicS64:
    return ZSTO_Integer;

  case ZType_Float:
  case ZType_Double:
  case ZType_LDouble:
  case ZType_AtomicFloat:
  case ZType_AtomicDouble:
  case ZType_AtomicLDouble:
    return ZSTO_Float;

  case ZType_ZDateFull:
    return ZSTO_Date;
  case ZType_Resource:
    return ZSTO_Resource;
  case ZType_CheckSum:
    return ZSTO_Checksum;

  }
  return ZSTO_Nothing;
}

ZTypeBase
ZTypefromZSTO(ZSearchOperandType_type pZSTO) {

    switch (pZSTO) {
    case ZSTO_String:
        return ZType_Utf8VaryingString;
    case ZSTO_Integer:
        return ZType_S32;

    case ZSTO_Float:
        return ZType_Double;

    case ZSTO_Date:
        return ZType_ZDateFull;

    case ZSTO_Resource:
        return ZType_Resource;
    case ZSTO_Checksum:
        return ZType_CheckSum;

    }
    return ZType_Nothing;
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
  case ZSTO_Symbol:
      return "ZSTO_Symbol";

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

  case ZSOPV_MOVE:
      return "ZSOPV_MOVE";

  case ZSOPV_INVALID:
    return "ZSOPV_INVALID";

  default:
    return "invalid/unknown Operator";

  }// switch

}


ZSearchOperandType_type
encode_ZSTO(const utf8VaryingString& pZSTOString)
{
    ZSearchOperandType_type wZSTO=ZSTO_Nothing;
    if (pZSTOString.contains("ZSTO_Field")) {
        wZSTO |=  ZSTO_Field ;
    }
    if (pZSTOString.contains("ZSTO_Literal")) {
        wZSTO |=  ZSTO_Literal ;
    }
    if (pZSTOString.contains("ZSTO_Logical")) {
        wZSTO |=  ZSTO_Logical ;
    }
    if (pZSTOString.contains("ZSTO_Arithmetic")) {
        wZSTO |=  ZSTO_Arithmetic ;
    }

    if (pZSTOString.contains("ZSTO_String")) {
        wZSTO |=  ZSTO_String ;
    }
    if (pZSTOString.contains("ZSTO_UriString")) {
        wZSTO |=  ZSTO_UriString ;
    }
    if (pZSTOString.contains("ZSTO_Integer")) {
        wZSTO |=  ZSTO_Integer ;
    }
    if (pZSTOString.contains("ZSTO_Float")) {
        wZSTO |=  ZSTO_Float ;
    }
    if (pZSTOString.contains("ZSTO_Bool")) {
        wZSTO |=  ZSTO_Bool ;
    }
    if (pZSTOString.contains("ZSTO_Date")) {
        wZSTO |=  ZSTO_Date ;
    }
    if (pZSTOString.contains("ZSTO_Resource")) {
        wZSTO |=  ZSTO_Resource ;
    }
    if (pZSTOString.contains("ZSTO_Checksum")) {
        wZSTO |=  ZSTO_Checksum ;
    }
    return wZSTO;
}

utf8VaryingString
decode_ZSTO(ZSearchOperandType_type pZSTO)
{
    ZSearchOperandType_type wZSTO=ZSTO_Nothing;
    utf8VaryingString wReturn;
    if (pZSTO & ZSTO_Field) {
        wReturn.addConditionalOR("ZSTO_Field");
    }
    if (pZSTO & ZSTO_Literal) {
        wReturn.addConditionalOR("ZSTO_Literal");
    }
    if (pZSTO & ZSTO_Logical) {
        wReturn.addConditionalOR("ZSTO_Logical");
    }
    if (pZSTO & ZSTO_Arithmetic) {
        wReturn.addConditionalOR("ZSTO_Arithmetic");
    }
    if (pZSTO & ZSTO_Symbol) {
        wReturn.addConditionalOR("ZSTO_Symbol");
    }


    if (pZSTO & ZSTO_String) {
        wReturn.addConditionalOR("ZSTO_String");
    }
    if (pZSTO & ZSTO_UriString) {
        wReturn.addConditionalOR("ZSTO_UriString");
    }
    if (pZSTO & ZSTO_Integer) {
        wReturn.addConditionalOR("ZSTO_Integer");
    }
    if (pZSTO & ZSTO_Float) {
        wReturn.addConditionalOR("ZSTO_Float");
    }
    if (pZSTO & ZSTO_Bool) {
        wReturn.addConditionalOR("ZSTO_Bool");
    }
    if (pZSTO & ZSTO_Date) {
        wReturn.addConditionalOR("ZSTO_Date");
    }
    if (pZSTO & ZSTO_Resource) {
        wReturn.addConditionalOR("ZSTO_Resource");
    }
    if (pZSTO & ZSTO_Checksum) {
        wReturn.addConditionalOR("ZSTO_Checksum");
    }
    if (wReturn.isEmpty())
        wReturn = "ZSTO_Nothing";

    return wReturn;
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
