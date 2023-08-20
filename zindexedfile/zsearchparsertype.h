#ifndef ZSEARCHPARSERTYPE_H
#define ZSEARCHPARSERTYPE_H

#include <ztoolset/utfvaryingstring.h>

#define __SEARCHPARSER_HISTORY_FILE__ "zsearchparserhistory.log"
#define __SEARCHPARSER_SYMBOL_FILE__ "zsearchparsersymbol.xml"
#define __SEARCHPARSER_PARAMS_FILE__ "zsearchparserparams.xml"
#define __SEARCHPARSER_ZENTITY_FILE__ "zsearchparserzentity.xml"
#define __SEARCHPARSER_WORK_DIRECTORY__  "zparserworkdir"
#define __SEARCHPARSER_PARAM_DIRECTORY__  "zparserparamdir"
#define __SEARCHPARSER_ICON_DIRECTORY__ "zparsericondir"

namespace zbs {

/* Parser and tokenizer options */
enum Options : uint32_t {
  ZSRCHO_Nothing      =      0,
  ZSRCHO_Include      =   0x01,       /* Allows  include files */

  //  ZCPO_Objects      =   0x02,       /* parse objects : structs and classes */
  //  ZCPO_Methods      =   0x04,       /* parse class and struct methods including contructors and destructors */
  //  ZCPO_Typedef      =   0x08,       /* resolve typedef translations */
  ZSRCHO_Verbose      = 0x0100, /* messages from ZAIES_Warning are displayed */
  ZSRCHO_FullVerbose  = 0x0300, /* all messages from ZAIES_Text are displayed */
//  ZSRCHO_Translate= 0x00010000, /* if set, translate data type  ENUM : give translated data type , typedef give target */
//  ZSRCHO_Recursive= 0x00030000, /* translate recursively : means sets also ZCPO_Translate */
  ZSRCHO_SkipNL   = 0x01000000, /* Tells tokenizer not to count newline in TokenOffset */

  ZSRCHO_Report   =     0x0400, /* generates a report after parsing process */

  ZSRCHO_Default = ZSRCHO_Report | ZSRCHO_SkipNL
};



typedef unsigned long ZSearchTokentype_type;
enum ZSearchTokentype : ZSearchTokentype_type {

  ZSRCH_NOTHING                =        0,

  ZSRCH_SPACE                  =        1,

  ZSRCH_LITERAL                  =   0x1000,
  ZSRCH_NUMERIC_LITERAL          =   0x1001,
  ZSRCH_HEXA_LITERAL             =   0x1002,
//  ZSRCH_OCTAL_LITERAL            =   0x1002,

  ZSRCH_INTEGER_LITERAL          =   0x1100,
  ZSRCH_INTEGER_LONG_LITERAL     =   0x1101,
  ZSRCH_INTEGER_ULONG_LITERAL    =   0x1103,


  ZSRCH_MAYBE_DOUBLE_LITERAL     =   0x1201,
  ZSRCH_DOUBLE_LITERAL           =   0x1200,
  ZSRCH_FLOAT_LITERAL            =   0x1400,
  ZSRCH_LONGDOUBLE_LITERAL       =   0x1800,

  ZSRCH_MAYBE_DATE_LITERAL       =   0x1900,
  ZSRCH_DATE_LITERAL             =   0X1901,

  ZSRCH_RESOURCE_LITERAL         =   0x1A00,

  ZSRCH_BOOL_LITERAL             =   0x1B00,

  ZSRCH_BOOL_LITERAL_TRUE        =   0x1B01,
  ZSRCH_BOOL_LITERAL_FALSE       =   0x1B02,

  ZSRCH_STRING_LITERAL           =   0x1D00,

  ZSRCH_MAYBE_PATH_LITERAL       =   0x1C00,
  ZSRCH_PATH_LITERAL             =   0x1C01,

  ZSRCH_CHECKSUM_LITERAL         =   0x1E00,
  ZSRCH_MD5_LITERAL              =   0x1E01,





  ZSRCH_FIELD_INITIALIZER        =   0x3000,

  ZSRCH_STRING_ESCAPE_SEQUENCE   =   0x2001,

  ZSRCH_MAYBE_COMMENT            =   0x2100,
  ZSRCH_MAYBE_ENDCOMMENT         =   0x2101,
  ZSRCH_COMMENT_DOUBLESLASH      =   0x2300,
  ZSRCH_COMMENT_SLASHSTAR        =   0x2500,
  ZSRCH_COMMENT_ENDSLASHSTAR     =   0x2900,

  ZSRCH_COMMENT_BRIEF            =   0x2110,
  ZSRCH_COMMENT_BRIEF_IDENTIFIER =   0x2130,
  ZSRCH_COMMENT_PREVIOUS         =   0x2140,
  ZSRCH_COMMENT_AFTER            =   0x2180,
  ZSRCH_COMMENT_TEXT             =    0x2102,


  ZSRCH_OPERATOR                 =    0x4000,

  ZSRCH_OPERATOR_LOGICAL         =    0x4100, // mask for logical operators

  ZSRCH_OPERATOR_AND             =    0x4102, // logical and '&' or AND
  ZSRCH_OPERATOR_OR              =    0x4103, // logical or '|' or OR
  ZSRCH_OPERATOR_NOT             =    0x4104, // logical not '!' or NOT

  ZSRCH_OPERATOR_EQUAL           =    0x4105, // logical equal '=' or EQUAL
  ZSRCH_OPERATOR_NOTEQUAL        =    0x4106, // logical different '!=' or NOT EQUAL

  ZSRCH_OPERATOR_LESS            =    0x4107, // '<' or LESS
  ZSRCH_OPERATOR_GREATER         =    0x4108, // '>' or GREATER

  ZSRCH_OPERATOR_LESSOREQUAL     =    0x4109, // '<=' or LESS OR EQUAL
  ZSRCH_OPERATOR_GREATEROREQUAL  =    0x410A, // '>=' or GREATER OR EQUAL


  ZSRCH_OPERATOR_ARITHMETIC      =    0x4200, // mask for arithmetic operators

  ZSRCH_OPERATOR_PLUS            =    0x4201,
  ZSRCH_OPERATOR_MINUS           =    0x4202,
  ZSRCH_OPERATOR_DIVIDEORSLASH   =    0x4203,
  ZSRCH_OPERATOR_MULTIPLY        =    0x4204,
  ZSRCH_OPERATOR_POWER           =    0x4205,
  ZSRCH_OPERATOR_MODULO          =    0x4206,

  /* string operators */

  ZSRCH_OPERATOR_COMPARESTRING   =    0x4300,

  ZSRCH_CONTAINS                 =    0x4310,
  ZSRCH_MAYBE_STARTS_WITH        =    0x4320,
  ZSRCH_STARTS_WITH              =    0x4321,

  ZSRCH_MAYBE_ENDS_WITH          =    0x4330,
  ZSRCH_ENDS_WITH                =    0x4331,

  ZSRCH_OPERATOR_MASK            =    0x4FF0,

  /* punctuation tokens */
  ZSRCH_OPENBRACE                =     0x8100,     /* Braces are curly  {} */
  ZSRCH_CLOSEBRACE               =     0x8101,
  ZSRCH_OPENBRACKET              =     0x8102,     /* Brackets are square  [] */
  ZSRCH_CLOSEBRACKET             =     0x8103,
  ZSRCH_OPENPARENTHESIS          =     0x8104,
  ZSRCH_CLOSEPARENTHESIS         =     0x8105,
  ZSRCH_SEMICOLON                =     0x8106,
  ZSRCH_COLON                    =     0x8107,
  ZSRCH_DOUBLECOLON              =     0x8108,
  ZSRCH_COMMA                    =     0x8109,

  ZSRCH_DOT                      =     0x810A,

  ZSRCH_EOL                      =     0x8110,

  ZSRCH_BACKSLASH                =     0x8111,


  ZSRCH_SIMPLE_TOKEN             =   0x010000,

  ZSRCH_MAYBE_IDENTIFIER         =  0x00010001,
  ZSRCH_IDENTIFIER               =  0x00010020,

  ZSRCH_ARRAY_IDENTIFIER         =  0x40010020,
  ZSRCH_ARRAY                    =  0x40000000,

  ZSRCH_IF                       =  0x00010030,

//  ZSRCH_FIELD_IDENTIFIER         =  0x00010021,


  ZSRCH_INSTRUCTION_MASK         = 0x00010300,
  ZSRCH_SET                      = 0x00010310,
  ZSRCH_FILE                     = 0x00010311,

  ZSRCH_MODE                     = 0x00010312,
  ZSRCH_READONLY                 = 0x00010313,
  ZSRCH_MODIFY                   = 0x00010314,

  ZSRCH_AS                       = 0x00010315,


  ZSRCH_FINISH                   = 0x0001031F,

  ZSRCH_FIND                     = 0x00010320,
  ZSRCH_WITH                     = 0x00010321,


  ZSRCH_INDEX                    = 0x00010331,
  ZSRCH_INDEXNUM                 = 0x00010332,
  ZSRCH_INDEXNAME                = 0x00010333,


  ZSRCH_FOR                      = 0x00010340,
  ZSRCH_SHOW                     = 0x00010350,

  ZSRCH_CLEAR                    = 0x00010351,
  ZSRCH_HISTORY                  = 0x00010352,

  ZSRCH_FIELDS                   = 0x00010353,  /* show instruction */
  ZSRCH_ENTITY                   = 0x00010354,  /* show instruction */
  ZSRCH_SYMBOL                   = 0x00010355,  /* show instruction */
  ZSRCH_MASTERFILES              = 0x00010356,  /* show instruction */

  ZSRCH_MAXIMUM                  = 0x00010356,  /* set history maximum instruction */

  ZSRCH_DISPLAY                  = 0x00010357,  /* diplay instruction */

  ZSRCH_ALL                      = 0x00010360,
  ZSRCH_FIRST                    = 0x00010361,
  ZSRCH_LAST                     = 0x00010362,
  ZSRCH_AT                       = 0x00010363,


  ZSRCH_MODIFIER                 = 0x00020300,

  /* string modifier */

  ZSRCH_SUBSTRING                 = 0x00020366,

  /* Date modifier */

  ZSRCH_YEAR                      = 0x00020370,
  ZSRCH_MONTH                     = 0x00020371,
  ZSRCH_DAY                       = 0x00020372,
  ZSRCH_HOUR                      = 0x00020373,
  ZSRCH_MIN                       = 0x00020374,
  ZSRCH_SEC                       = 0x00020375,

  /* Resource  */

 // ZSRCH_LITERAL_RESOURCE          = 0x00010380,  /* litteral declaration : ZSRCH_RESOURCE_LITERAL */

  /* Resource modifier */
  ZSRCH_ZENTITY                   = 0x00020381,  /* used for resource */
  ZSRCH_ID                        = 0x00020382,




  /* uristring modifiers */

  ZSRCH_PATH                      = 0x00020390,
  ZSRCH_EXTENSION                 = 0x00020391,
  ZSRCH_BASENAME                  = 0x00020392,
  ZSRCH_ROOTNAME                  = 0x00020393,

  /* checksum */

  /* md5 */

  ZSRCH_FIELD                    = 0x00011000,
  ZSRCH_ARGUMENT                 = 0x00011001,


  /* data type modifiers */

  ZSRCH_CONST            = 0x00100000,
  ZSRCH_UNSIGNED         = 0x00200000,
  ZSRCH_AMPERSAND        = 0x00400000,
  ZSRCH_DOUBLE_AMPERSAND = 0x00800000,
  ZSRCH_POINTER          = 0x01000000,
  ZSRCH_LONG             = 0x02000000,  /* long is a modifier and a data type */

  ZSRCH_MODIFIER_MASK    = 0x0FF00000 , /* to extract reference or pointer or long from Token Type */

  ZSRCH_MAYBE_DATA_TYPE  = 0x00012000,
  ZSRCH_DATA_TYPE        = 0x00012001,

  ZSRCH_UNKNOWN_TYPE     = 0xF000012000,
  ZSRCH_UNKN_NESTED_TYPE = 0xF100012000,
  ZSRCH_SUBSTITUED       = 0xF200012000,
  //                        0x00800000,
  /* atomic data types */

  ZSRCH_VOID             = 0x00011002,
  ZSRCH_CHAR             = 0x00011003,
  ZSRCH_INTEGER          = ZSRCH_CHAR + 1,
  ZSRCH_LONG_LONG        = ZSRCH_INTEGER + 1,
  ZSRCH_FLOAT            = ZSRCH_LONG_LONG + 1,
  ZSRCH_DOUBLE           = ZSRCH_FLOAT + 1,
  ZSRCH_LONG_DOUBLE      = ZSRCH_DOUBLE | ZSRCH_LONG,
  ZSRCH_BOOL             = ZSRCH_DOUBLE + 1,

  ZSRCH_INT8             = 0x00011101,
  ZSRCH_INT16            = ZSRCH_INT8+1,    /* 0x00011102 */
  ZSRCH_INT32            = ZSRCH_INT16+1,   /* 0x00011103 */
  ZSRCH_INT64            = ZSRCH_INT32+1,   /* 0x00011104 */

  /* methods and functions */

  ZSRCH_MAYBE_METHOD     = 0x00014000,
  ZSRCH_METHOD           = 0x00014001,
  ZSRCH_METHOD_CTOR      = 0x00014003,
  ZSRCH_METHOD_DTOR      = 0x00014005,

  ZSRCH_CTOR_DEFAULT     = 0x00014006,
  ZSRCH_CTOR_DELETED     = 0x00014007,

  ZSRCH_METHOD_ARGUMENT  = 0x00014010,
  ZSRCH_OPERATOR_OVERLD  = 0x00014021,

  /* preprocessor clauses */

  ZSRCH_MAYBE_PREPROC    = 0x00020000,
  ZSRCH_INCLUDE          = 0x00020001,
  ZSRCH_PRAGMA           = 0x00020002,
  ZSRCH_IFDEF            = 0x00020004,
  ZSRCH_IFNDEF           = 0x00020014,
  ZSRCH_DEFINE           = 0x00020008,
  ZSRCH_ENDIF            = 0x00020020,
  ZSRCH_MAYBE_INCLUDE_FILE= 0x00020010,
  ZSRCH_INCLUDE_FILE     = 0x00020011,

  ZSRCH_UTF8VARYINGSTRING = 0x10000011,
  ZSRCH_UTF16VARYINGSTRING= 0x10000012,
  ZSRCH_UTF32VARYINGSTRING= 0x10000013,

  ZSRCH_URISTRING         = 0x10000031,

  ZSRCH_UTF8FIXEDSTRING   = 0x10000001,
  ZSRCH_UTF16FIXEDSTRING  = 0x10000002,
  ZSRCH_UTF32FIXEDSTRING  = 0x10000003,

  ZSRCH_ZDATEFULL         = 0x10000100,
  //  ZSRCH_ZDATE             = 0x10000200,

  ZSRCH_MD5                = 0x10001000,
  ZSRCH_CHECKSUM           = 0x10002000,

  ZSRCH_ZDATABUFFER        = 0x10004000,

  ZSRCH_ZBITSET            = 0x10007000,

  ZSRCH_ZRESOURCE          = 0x10005000,
  ZSRCH_ZRESOURCECODE      = 0x10006000,
  ZSRCH_STDSTRING          = 0x20000000

};


typedef  uint16_t ZSearchOperandType_type;
enum ZSearchOperandType : ZSearchOperandType_type
{
  ZSTO_Nothing          = 0,
  ZSTO_String           = 0x0001,
  ZSTO_Integer          = 0x0002,
  ZSTO_Float            = 0x0004,
  ZSTO_Date             = 0x0008,
  ZSTO_Checksum         = 0x0010,
  ZSTO_Resource         = 0x0020,
  ZSTO_UriString        = 0x0040,
  ZSTO_Bool             = 0x0080,

  ZSTO_BaseMask         = 0x00FF,

  ZSTO_Field            = 0x0100,

  ZSTO_FieldString      = ZSTO_Field|ZSTO_String,
  ZSTO_FieldInteger     = ZSTO_Field|ZSTO_Integer,
  ZSTO_FieldFloat       = ZSTO_Field|ZSTO_Float,
  ZSTO_FieldDate        = ZSTO_Field|ZSTO_Date,
  ZSTO_FieldChecksum    = ZSTO_Field|ZSTO_Checksum,
  ZSTO_FieldResource    = ZSTO_Field|ZSTO_Resource,
  ZSTO_FieldUriString   = ZSTO_Field|ZSTO_UriString,
  ZSTO_FieldBool        = ZSTO_Field|ZSTO_Bool,

  ZSTO_Literal          = 0x0200,
  ZSTO_LiteralString    = ZSTO_Literal|ZSTO_String,
  ZSTO_LiteralInteger   = ZSTO_Literal|ZSTO_Integer,
  ZSTO_LiteralFloat     = ZSTO_Literal|ZSTO_Float,
  ZSTO_LiteralDate      = ZSTO_Literal|ZSTO_Date,
  ZSTO_LiteralChecksum  = ZSTO_Literal|ZSTO_Checksum,
  ZSTO_LiteralResource  = ZSTO_Literal|ZSTO_Resource,
  ZSTO_LiteralUriString = ZSTO_Literal|ZSTO_UriString,
  ZSTO_LiteralBool      = ZSTO_Literal|ZSTO_Bool,

  ZSTO_Logical          = 0x0400,

  ZSTO_Arithmetic       = 0x0800,

  ZSTO_ArithFieldString     = ZSTO_Arithmetic|ZSTO_FieldString,
  ZSTO_ArithFieldInteger    = ZSTO_Arithmetic|ZSTO_FieldInteger,
  ZSTO_ArithFieldFloat      = ZSTO_Arithmetic|ZSTO_FieldFloat,
  ZSTO_ArithFieldDate       = ZSTO_Arithmetic|ZSTO_FieldDate,
  ZSTO_ArithFieldChecksum   = ZSTO_Arithmetic|ZSTO_FieldChecksum,
  ZSTO_ArithFieldResource   = ZSTO_Arithmetic|ZSTO_FieldResource,
  ZSTO_ArithFieldUriString  = ZSTO_Arithmetic|ZSTO_FieldUriString,
  ZSTO_ArithFieldBool       = ZSTO_Arithmetic|ZSTO_FieldBool,

  ZSTO_ArithLiteralString     = ZSTO_Arithmetic|ZSTO_LiteralString,
  ZSTO_ArithLiteralInteger    = ZSTO_Arithmetic|ZSTO_LiteralInteger,
  ZSTO_ArithLiteralFloat      = ZSTO_Arithmetic|ZSTO_LiteralFloat,
  ZSTO_ArithLiteralDate       = ZSTO_Arithmetic|ZSTO_LiteralDate,
  ZSTO_ArithLiteralChecksum   = ZSTO_Arithmetic|ZSTO_LiteralChecksum,
  ZSTO_ArithLiteralResource   = ZSTO_Arithmetic|ZSTO_LiteralResource,
  ZSTO_ArithLiteralUriString  = ZSTO_Arithmetic|ZSTO_LiteralUriString,
  ZSTO_ArithLiteralBool       = ZSTO_Arithmetic|ZSTO_LiteralBool,

};

const char* decode_OperandType(ZSearchOperandType_type pType);

utf8VaryingString decode_SearchTokenType(ZSearchTokentype_type pType);
ZSearchTokentype_type encode_ZTokenType(const utf8VaryingString& pIn);

ZSearchOperandType ZSTOfromZType(ZTypeBase pZType);



typedef uint32_t ZSearchOperator_type;

enum ZSearchOperatorValue : ZSearchOperator_type {
  ZSOPV_Nothing       =   0,
  /* logical operators */

  ZSOPV_LOGICAL_MASK  =   0xFF,

  /* link operators */
  ZSOPV_NOT           =   0x01,
  ZSOPV_AND           =   0x02,
  ZSOPV_OR            =   0x04,

  /* compare operators */
  ZSOPV_EQUAL         =   0x08,
  ZSOPV_NOTEQUAL      =   0x10,
  ZSOPV_LESS          =   0x20,
  ZSOPV_GREATER       =   0x40,

  ZSOPV_LESSEQUAL     =   0x80,
  ZSOPV_GREATEREQUAL  =   0x0100,

  /* arithmetic operators */

  ZSOPV_ARITHMETIC_MASK=    0xFF00,

  ZSOPV_MINUS         =     0x0200,
  ZSOPV_DIVIDE        =     0x0400,
  ZSOPV_MULTIPLY      =     0x0800,
  ZSOPV_POWER         =     0x1000,
  ZSOPV_MODULO        =     0x2000,
  ZSOPV_PLUS          =     0x4000,

  /* string operators */

  ZSOPV_STRING_MASK   =     0x0F0000,

  ZSOPV_CONTAINS      =     0x010000,
  ZSOPV_STARTS_WITH   =     0x020000,
  ZSOPV_ENDS_WITH     =     0x040000,

  ZSOPV_INVALID       =   0xFFF00000
};

const char* decode_ZSOPV(ZSearchOperator_type pOp);
ZSearchOperatorValue convert_ZSRCH_ZSOPV(ZSearchTokentype_type pType);



class ZSearchKeyWord {
public:
  ZSearchKeyWord()=default;
  ZSearchKeyWord(const ZSearchKeyWord& pIn) {_copyFrom(pIn);}
  ZSearchKeyWord(const utf8VaryingString& pText,ZSearchTokentype pType)
  {
    Text=pText;
    Type=pType;
  }
  ZSearchKeyWord& _copyFrom(const ZSearchKeyWord& pIn)
  {
    Text=pIn.Text;
    Type=pIn.Type;
    return *this;
  }

  ZSearchKeyWord& operator = (const ZSearchKeyWord& pIn) {return _copyFrom(pIn);}

  utf8VaryingString Text;
  ZSearchTokentype  Type;
};

extern ZArray<ZSearchKeyWord> KeywordList;





class ZSearchToken;

bool
searchForKeyword(ZSearchToken* &pCurrentToken,const utf8VaryingString &pIn,long &pCurrentIndex) ;


}//namespace zbs


const char* getSearchParserWorkDirectory();
const char* getSearchParserParamDirectory();

#endif // ZSEARCHPARSERTYPE_H
