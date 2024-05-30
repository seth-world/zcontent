#ifndef ZSEARCHPARSER_H
#define ZSEARCHPARSER_H

#define __DEBUG__

#define __SEARCHPARSER_VERSION__  "0.30-0"

/* in case of prefix with entity name */

#define __SEARCH_ENTITY_SEPARATOR__ ZSRCH_DOT
#define __SEARCH_ENTITY_SEPARATOR_ASC__ "."
#define __MODIFIER_SEPARATOR__ ZSRCH_RIGHTARROW
#define __MODIFIER_SEPARATOR_ASC__ "->"

#include "zsearchtokenizer.h"
//#include "type.h"
//#include "FunctionDefinition.hpp"
//#include "Statement.hpp"
#include <optional>
#include <memory>

//#include <string>
//#include <map>
#include <ztoolset/zarray.h>
#include <ztoolset/uristring.h>

#include <ztoolset/zaierrors.h>

#include "zmetadic.h"
//#include "zmasterfile.h"

#include "zsearchentity.h"
#include "zsearchhistory.h"
#include "zselclause.h"

#include <zcontent/zrandomfile/zrandomfile.h>

#include "zsearchcontext.h"

#include "zsearchsymbol.h"

namespace zbs {


extern class ZSearchParser* GParser;

extern  ZArray<ZEntitySymbol>  ZEntitySymbolList;

//class ZSearchFormula;
class ZSearchCollection;

using namespace std;
/*
  enum ZSearchParseAction : uint32_t {
    ZSPA_Nothing          =   0,

    ZSPA_DeclareFile          =   0x10,
    ZSPA_ReadOnly         =   0x12,
    ZSPA_Modify           =   0x14,

    ZSPA_Find             =   0x1000,
    ZSPA_All              =   0x2000,
    ZSPA_First            =   0x4000,

    ZSPA_MultipleEntities =   0x8000
    };
*/



enum LastProcessed: uint16_t {
  LPE_Nothing =   0,
  LPE_Class   =   1,
  LPE_Enum    =   2,
  LPE_Field   =   4,
  LPE_Method  =   8,
  LPE_Typedef =0x10
};


class ZConversionElement {
public:
  ZConversionElement() = default;
  ZConversionElement(const ZConversionElement& pIn) {_copyFrom(pIn);}
  ZConversionElement& _copyFrom(const ZConversionElement& pIn) {
    Syntax=pIn.Syntax;
    ZType = pIn.ZType;
    Capacity = pIn.Capacity;
    HeaderSize = pIn.HeaderSize;
    NaturalSize = pIn.NaturalSize;
    UniversalSize = pIn.UniversalSize;
    return *this;
  }
  ZConversionElement& operator =(const ZConversionElement& pIn) {return _copyFrom(pIn);}

  //  ZTokentype_type     TokenType;  // Deprecated
  utf8VaryingString   Syntax;
  ZTypeBase           ZType;
  int                 Capacity=1;
  size_t              HeaderSize=0;
  size_t              NaturalSize=0;
  size_t              UniversalSize=0;
};

class ConvertTuple {
public:
  ConvertTuple() {setInvalid();}
  ConvertTuple(ZTypeBase pZType,int pCapacity,size_t pHeaderSize=0,size_t pUniversalSize=0,size_t pNaturalSize=0) {
    ZType=pZType;
    Capacity=pCapacity;
    HeaderSize=pHeaderSize;
    UniversalSize=pUniversalSize;
    NaturalSize=pNaturalSize;
  }
  ConvertTuple(const ConvertTuple& pIn) {
    ZType=pIn.ZType;
    Capacity=pIn.Capacity;
    HeaderSize=pIn.HeaderSize;
    UniversalSize=pIn.UniversalSize;
    NaturalSize=pIn.NaturalSize;
  }
  ConvertTuple& operator=(const ConvertTuple& pIn) {
    ZType=pIn.ZType;
    Capacity=pIn.Capacity;
    HeaderSize=pIn.HeaderSize;
    UniversalSize=pIn.UniversalSize;
    NaturalSize=pIn.NaturalSize;
    return *this;
  }
  void setInvalid() {ZType=0; Capacity=0; UniversalSize=0; NaturalSize=0;}
  bool isInvalid() {
    if (!ZType && !Capacity && !UniversalSize && !NaturalSize)
      return true;
    return false;
  }

  ZTypeBase ZType;
  int       Capacity=1;
  size_t    HeaderSize=0;
  size_t    UniversalSize=0;
  size_t    NaturalSize=0;
};

class ZConversionRules : public zbs::ZArray<ZConversionElement>  {
public:
  ZConversionRules() {}
  ~ZConversionRules() {}

  long searchForSyntax(const utf8VaryingString& pSyntax) {
    long wi=0;
    for (;wi<count();wi++) {
      if (Tab(wi).Syntax==pSyntax) {
        return wi;
      }
    }
    return -1;
  }
};


class ZSearchLogicalOperand;
class ZMasterFile;


/* this class is used ONLY for storing valued shared pointer to unique masterfile object instance once it has been created */

class ZMasterFileItem : public std::shared_ptr<ZMasterFile>
{
public:
    ZMasterFileItem() : std::shared_ptr<ZMasterFile>(nullptr) {}
    ZMasterFileItem(const utf8VaryingString& pName,std::shared_ptr<ZMasterFile> pZMF)
    {
        this->std::shared_ptr<ZMasterFile>::operator =(pZMF);
//        ZMFPtr = pZMF;
        Name=pName;
    }

    ZMasterFileItem& _copyFrom(const ZMasterFileItem& pIn)
    {
        this->std::shared_ptr<ZMasterFile>::operator =(pIn);
        Name = pIn.Name;
        return *this;
    }

    ZMasterFileItem& operator= (std::shared_ptr<ZMasterFile> pZMF)
    {
        this->std::shared_ptr<ZMasterFile>::operator =(pZMF);
        return *this;
    }

    std::shared_ptr<ZMasterFile> getPtr() {return std::shared_ptr<ZMasterFile>(*this);}

    utf8VaryingString Name;
};

class ZSearchEntityList : public ZArray<std::shared_ptr<ZSearchEntity>>
{
public:
  ZSearchEntityList()=default;
  ~ZSearchEntityList()
  {
    while (count())
      pop();
  }
  std::shared_ptr<ZSearchEntity>  getEntityByName(const utf8VaryingString& pEntityName) {
    for (int wi=0; wi < count();wi++) {
      if (Tab(wi)->getEntityName() == pEntityName) {
        return Tab(wi);
      }
    }
    return nullptr;
  }
};


class ZSearchParserState;
class ZParserError : public ZaiErrors {
public:
  ZParserError(ZSearchParserState*     pFather) {Father=pFather;}
  /*
  void errorLog(const char *pFormat,...)
  {
      va_list args;
      va_start (args, pFormat);
      ZaiError* wErrorToPush = new ZaiError(ZAIES_Error,pFormat,pArgs);
      utf8VaryingString wMessage;
      wMessage.sprintf("<%s> %s ",decode_ZAIES(wErrorToPush->Severity),wErrorToPush->Message());
      pFather->statusBar
      ZaiError::_log(ZAIES_Error,pFormat,args);
      va_end(args);
  }
  */
//  ZSearchToken* LastErroredToken=nullptr;
  ZSearchParserState*     Father=nullptr;
};

#ifdef __DEPRECATED__
class ZSearchParserState
{
public:
 // ZSearchParserState(): ZParserError(this) {}
  ZSearchParserState() {}
  ZSearchParserState(ZSearchParserState&)=delete;

  ~ZSearchParserState()
  {
  //  Entity.reset();
/*
    for (int wi=0;wi < CurEntities.count();wi++)
      CurEntities[wi].reset();
*/
  }


/*
  void setIndex(int pIndex,ZSearchContext& pContext) {
//    IndexLog.push(Index);
    pContext.Index=pIndex;
    pContext.CurrentToken = Tokenizer->Tab(pIndex);
  }
*/
//  std::shared_ptr<ZSearchEntity>          Entity=nullptr;

//  ZArray<std::shared_ptr<ZSearchEntity>>  CurEntities;        /* current used entities within phrase */

//  int                                     Index=0;
//  ZSearchToken*                           CurrentToken=nullptr;
//  ZSearchToken*                           FirstToken=nullptr;
//public:  ZSearchTokenizer*                Tokenizer=nullptr;

};
#endif // __DEPRECATED__



class ZSearchFieldOperandOwnData;
class ZSearchOperandBase;
class ZSearchArithmeticTerm;

//class ZSearchParser : public ZaiErrors, public ZSearchParserState
//class ZSearchParser : public ZaiErrors
//class ZSearchParser : public ZaiErrors, public ZSearchTokenizer
class ZSearchParser : public ZSearchTokenizer
{
public:
  friend class ZSearchLogicalOperand;
  friend class ZSearchParserField;
//  friend class ZCppClass;


  ZSearchParser() ;

  ~ZSearchParser();


  ZStatus setup(uriString& pXmlParserSymbol,
                uriString& pXmlParserZEntity,
                uriString& pXmlParserParams,
                const uriString& pHistory,
                uint32_t pOptions=ZSRCHO_Default);

  /* see enum zbs::Options */
  void setOptions(uint32_t pOptions) {
    Options=pOptions;
    if (pOptions & ZSRCHO_Verbose)
        ErrorLog.setAutoPrintOn(ZAIES_Warning);
    if (pOptions & ZSRCHO_FullVerbose)
      ErrorLog.setAutoPrintOn(ZAIES_Text);
  }
  void addOptions(uint32_t pOptions) {
    Options |= pOptions;
  }
  void removeOptions(uint32_t pOptions) {
    Options&=~pOptions;
    if (pOptions & ZSRCHO_FullVerbose)
      ErrorLog.setAutoPrintOn(ZAIES_None);
    if (pOptions & ZSRCHO_Verbose)
      ErrorLog.setAutoPrintOn(ZAIES_None);
  }
/*
  void clear(ZSearchContext& pContext) {
//    Tokenizer->clear();
    pContext.Tokenizer->clear();
    pContext.Index=0;
  }
*/
  void clearErrors() {
      ErrorLog.clear();
  }

  ZaiErrors* getErrorLog() {return (ZaiErrors*)this;}

  void setIncludePath(const utf8VaryingString& pIncludePath);

  void finish() {}

  /**
   * @brief buildContext Creates a mandatory context from current parser that has to be used later on for any call to parser.
   */
  ZSearchContext buildContext()
  {
    return ZSearchContext(this);
  }

//  ZStatus parse(const utf8VaryingString &pContent,std::shared_ptr<ZSearchEntity> &pCollection) ;

  /* parsing main loop : parse a single instruction or a set of instructions delimited with semi-colon sign */
  ZStatus parse(const utf8VaryingString &pContent, ZSearchContext& pSearchContext);


  bool searchKeyword(ZSearchToken* pToken) ;


//  ZStatus _parse(ZSearchContext& pContext);


  ZStatus parseAllocationFormula(const utf8VaryingString* pEntity,const utf8VaryingString& pSelectionPhrase);
  ZStatus _parsAllocationFormula(const utf8VaryingString* pEntity,ZSearchTokenizer* pTokenizer);
  bool    _parseComment (ZSearchToken* &pToken);

//  ZStatus _parseShow(ZSearchContext &pContext);


//  ZStatus _parseFind(std::shared_ptr<ZSearchEntity> &pCollection);
//  ZStatus _parseFind(ZSearchContext & pContext);

  ZStatus _parseFor(ZSearchContext &pContext);

  ZStatus _parseSave(ZSearchContext &pContext);


  ZStatus _parseContext(ZSearchContext& pContext);
  ZStatus _parseContextFind(ZSearchContext & pContext);
  ZStatus _parseContextFor(ZSearchContext & pContext);
  ZStatus _parseContextSave(ZSearchContext & pContext);

  ZStatus _parseContextShow(ZSearchContext & pContext);
  ZStatus _parseContextShowEntity(ZSearchContext & pContext);

  ZStatus _parseContextClear(ZSearchContext & pContext);

  /* entry point for declare keyword parsing */
  ZStatus _parseContextDeclare(ZSearchContext & pContext);
  /* parse file entity */
  ZStatus _parseContextDeclareFile(ZSearchContext & pContext);
  /* parse a new entity either collection or join */
  ZStatus _parseContextDeclareEntity(ZSearchContext &pContext);
  /* parse a single entity either collection-single entity or component of a join entity */
  ZStatus _parseDeclareEntity(ZSearchContext &pContext);

  ZStatus _parseContextDeclareSymbol(ZSearchContext & pContext);
  ZStatus _parseContextSet(ZSearchContext & pContext);
  ZStatus _parseContextSetHistoryMaximum(ZSearchContext & pContext);




  ZStatus _parseFetch(ZSearchContext & pContext);
  ZStatus _parseDisplay(ZSearchContext & pContext);

  ZStatus _parseSetDisplay(ZSearchContext & pContext);

  ZStatus executeDisplay(ZSearchContext& pContext);


  void _displayOneFieldDetail(const ZFieldDescription& pField);

  ZStatus _parseGet(ZSearchContext & pContext);

  ZStatus execute(ZSearchContext &pContext);

  ZStatus executeFetch(ZSearchContext &pContext);

//  ZStatus _executeFind(std::shared_ptr<ZSearchEntity> pCollection);

  /** Parses a field and all possible modifiers. Calls _parseFieldIdentifier() method.
   *
   * if pRequestedType is ZSTO_Nothing, then all types are allowed (first operand of an expression)
 */

  ZStatus _parseOperandField(void *&pTermOperand,
                             ZSearchContext &pContext,
                             ZSearchOperandType_type &pRequestedType);
  ZStatus _parseLiteral(ZSearchLiteral *&pOperand,
                        ZSearchContext &pContext,
                        ZSearchOperandType_type &pRequestedType);

  /* @brief _parseSelClause creates TargetEntity BuildDic (ZSearchDictionary) from selection clause and SourceEntity LocalDic (ZMetaDic)
   */
  ZStatus _parseSelClause(ZSearchContext &pContext);

  ZStatus _parseWithClause(ZSearchContext &pContext);

  ZStatus _parseUsingClause(ZSearchContext& pContext);
// Deprecated
//  ZStatus _parseSelClause_old(ZSearchContext& pContext);

  /** parse a field identifier and validate it according Entity array
   *  if more than one entity in CurEntities array, then field must be prefixed with its entity name
   *  if not, field may be prefixed or not by its entity name, it is reputated belonging to the unique entity
   *
   *  Field is validated against entity dictionary content.
   * extracts field identifier either under the form of
   <field>
   <field>.<modifier>

    <entity>.<field>
    <entity>.<field>.<modifier>
      <modifer> itself and its possible arguments will be extracted by callee routine (i. e. _parseOperandField())
   */
  ZStatus _parseFieldIdentifier(ZSearchContext &pContext, ZSearchFieldOperand *pFOD);
/*
  ZStatus _parseFieldIdentifier(ZSearchContext &pContext,
                                ZArray<std::shared_ptr<ZSearchEntity>> pAllowedEntities,
                                ZSearchField &pResultingField);
*/
  ZStatus _parseModifier(ZSearchOperandBase *pOB, ZSearchContext &pContext);

  // ZStatus _parseLogicalOperand(void *&pOperand, int pParenthesisLevel, int pCollateral, int pBookMark);

  bool _parseZEntity(void *&pOperand,
                     ZSearchContext &pContext,
                     ZSearchOperandType_type &pRequestedType);
  bool _parseEntity(void *&pOperand, ZSearchContext &pContext, ZSearchOperandType_type &pRequestedType);
  bool _getSymbol(void *&pOperand, ZSearchContext &pContext, ZSearchOperandType_type &pRequestedType);

  ZStatus _getZDateOnePiece(int &pValue,
                            ZSearchContext &pContext,
                            int pMaxDigits,
                            ZSearchTokentype pEndSeparator);

  ZStatus _parseOneLogicalOperand(void *&pOperand,
                                  ZSearchContext &pContext,
                                  ZSearchOperandType_type &pMainType,
                                  int pParenthesisLevel);
/*
  ZStatus _parseLogicalTerm(ZSearchLogicalTerm *&pTerm,
                            ZSearchContext &pContext,
                            int pParenthesisLevel,
                            int pBookMark);
*/
  ZStatus _parseLogicalTerm(ZSearchLogicalTerm *&pTerm,
                            ZSearchContext &pContext,
                            int pParenthesisLevel);

  ZStatus _parseArithmeticTerm(ZSearchArithmeticTerm *&pArithTerm,
                               ZSearchContext &pContext,
                               int pParenthesisLevel,
//                               int pBookMark,
                               ZSearchOperandType_type &pRequestedType,
                               bool &pIsLiteral);

  bool _arithmeticTypeCheck(ZSearchOperandType_type &pMainType,
                            ZSearchContext &pContext,
                            ZSearchOperandBase *pOB,
                            int pIndex);
  bool OperandTypeCheck(ZSearchOperandType_type &pMainType,
                        ZSearchContext &pContext,
                        ZSearchOperandBase *pOB,
                        int pIndex);

  ZStatus _finish();

  void    showHistory();
  void    showHistoryMaximum();
  ZStatus setHistoryMaximum(int pMax);
  void    clearHistory();
  ZStatus clearHistoryFirst (int pLines);
  ZStatus clearHistoryLast (int pLines);
  ZStatus clearHistoryRank (int pRank);

  void    showSymbols();
  ZStatus showSymbol(ZSearchContext &pContext, ZSearchToken *pTokenName);
  void    showZEntities();

  void    showDisplay(); /* show display parameters */

  void    showMasterFiles();
  void    showEntityList();
  void    showEntityDetailsBrief (std::shared_ptr<ZSearchEntity> pEntity, int pShowType);

  ZStatus showEntity(ZSearchContext &pContext, ZSearchToken *pTokenName,int pShowType);

  enum ShowEntityType : int
  {
      SHENT_Nothing = 0,
      SHENT_With    = 2,
      SHENT_Dictionaries = 1,
      SHENT_Using   = 4,
      SHENT_All     = 0x0F,
      SHENT_Brief   = 0x10
  };

  /* pShowType values   2 : Formula  -   1 : Fields -    3 : all   - 4 : using - 8 brief */
  void    showEntityDetails(std::shared_ptr<ZSearchEntity> pEntity, int pShowType = 3);

  void showEntityHeader (std::shared_ptr<ZSearchEntity> pEntity);
  void showEntityTrailer (std::shared_ptr<ZSearchEntity> pEntity);
  void showEntityWithClause (std::shared_ptr<ZSearchEntity> pEntity,int pDetail);
  void showEntityUsingClause (std::shared_ptr<ZSearchEntity> pEntity,int pDetail);
  void showEntityBuildDic (std::shared_ptr<ZSearchEntity> pEntity,int pDetail);
  void showEntityLocalDic (std::shared_ptr<ZSearchEntity> pEntity,int pDetail);

  void showJoinDetails (std::shared_ptr<ZSearchEntity> pEntity,int pShowType);
  void showCollectionDetails (std::shared_ptr<ZSearchEntity> pEntity,int pShowType);

  ZStatus showEntityField(ZSearchContext &pContext,
                          std::shared_ptr<ZSearchEntity> pEntity,
                          const utf8VaryingString &pFieldName);

  /** @brief init  clean all data structures */
  void init() {
    ZSearchTokenizer::clear();
    SymbolList.clear();
    ZEntitySymbolList.clear();
    EntityList.clear();
  }

//  void report(ZArray<ZSearchParserField> &pObjectList) const;


//public: ZParserError Errorlog;

//public: ZSearchTokenizer*        Tokenizer=nullptr;

  uint32_t getOption() {return Options;}
  bool isVerbose()  {return Options & ZSRCHO_Verbose;}
  bool isFullVerbose()  {return Options & ZSRCHO_FullVerbose;}
  bool hasOption(ZSRCHO pOpt) {return Options & pOpt;}

  ZStatus saveContext(uriString& pXmlFile);

  /** @brief loadXmlSearchParserSymbols loads ZSearchParser symbols : translation for uri files path
   */
  ZStatus loadXmlSearchParserSymbols(const uriString& pXmlFile);

  /** @brief loadXmlSearchParserZEntity loads ZSearchParser ZEntity values for application
   */
  ZStatus loadXmlSearchParserZEntity(const uriString& pXmlFile);

  ZStatus loadXmlSearchParserParams(const uriString& pXmlFile);

private:
//  void loadIncludeFiles(ZSearchTokenizer &pTokenizer);

//    bool advanceIndex(ZSearchContext &pContext);

    bool testSemiColon(ZSearchContext &pContext);

    ZTypeBase convertToZType(ZSearchTokentype_type pType);
    ConvertTuple TableConvert(ZSearchTokentype_type &pTokenType, const utf8VaryingString &pSyntax);

public:

  void setEntityDisplayCallBack(std::function<ZStatus (ZSearchContext&)>  pDisplay)
    {
      DisplayEntity = pDisplay;
    }

  void setDisplayCallback(__DISPLAYCALLBACK__(pdisplayCallback) ) {
    ZSearchTokenizer::setDisplayCallback(pdisplayCallback);
    setDisplayCallback(pdisplayCallback);
  }
  void setProgressCallback(__progressCallBack__(pdisplayCallback) ) {
    ProgressCallBack  = pdisplayCallback;
  }
  void setProgressSetupCallback(__progressCallBack__(pdisplayCallback) ) {
    ProgressSetupCallBack  = pdisplayCallback;
  }

  void setUpdateFrequence(int pFrequence) {UpdateFrequence = pFrequence;}

  void displayTokenList(ZArray<ZSearchToken*> &Whole);
  void displayTokenList();

  /** @brief DisplayEntityDefault default routine to output entity content to ErrorLog */
  ZStatus DisplayEntityDefault(ZSearchContext& pContext);
  ZStatus _DisplayEntitySingleDefault(ZSearchContext& pContext);
  ZStatus _DisplayEntityJoinDefault (ZSearchContext &pContext);

  /* base routine for output */
  void _DESD_Display (const utf8VaryingString& pFormat,...);
  void _DESD_Init();
  /* Deprecated
  ZStatus DisplayEntityDefault(std::shared_ptr<ZSearchEntity> pEntity,int pInstructionType,int pNumber);
  */
  /** @brief setDisplayLimit sets maximum number of entity ranks to be displayed.If pLimit is negative, then no limit is applied */
  void setDisplayLimit(int pLimit);
  /** @brief setDisplayColMax sets maximum column width for default entity display routine */
  void setDisplayColMax(int pColMax);
  /** @brief setDisplayColMin sets minimum column width for default entity display routine */
  void setDisplayColMin(int pColMin);
  /** @brief setDisplayCellFmt sets display cell format (see ZCellFormat) for default entity display routine */
  void setDisplayCellFmt(ZCFMT_Type pCellFmt) {DisplayCellFormat = pCellFmt;}
  /** @brief addDisplayCellFmt adds display cell format (see ZCellFormat) to current display cell format for default entity display routine */
  void addDisplayCellFmt(ZCFMT_Type pCellFmt) {DisplayCellFormat |= pCellFmt;}

  uriString             URIZEntity;
  uriString             URISymbol;
  uriString             URIParams;

  uriString             URIDisplay;
  bool                  DisplayCurrent=false;
  int                   DisplayLimit=-1;

//  ZArray<ZSearchFileSymbol>                         SymbolList;
  ZSearchSymbolList                                 SymbolList;
//  ZArray<ZEntitySymbol>                           ZEntitySymbolList;
  ZSearchEntityList                                 EntityList;
//  ZArray<std::shared_ptr<ZSearchMasterFile>>      MasterFileList;
  ZArray<ZMasterFileItem>                           MasterFileList;


  ZSearchHistory                  History;

//  uint32_t                         Action=ZSPA_All;

  int UpdateFrequence = 5;
  __progressCallBack__(ProgressCallBack)=nullptr;
  __progressSetupCallBack__(ProgressSetupCallBack)=nullptr;
/*
  std::function<ZStatus (ZSearchContext&)>  DisplayEntity =
      std::bind(&ZSearchParser::DisplayEntityDefault, this,placeholders::_1);
*/
  std::function<ZStatus (ZSearchContext&)>  DisplayEntity = nullptr ;
  int DisplayColMax = 35;
  int DisplayColMin = 5;
  ZCFMT_Type DisplayCellFormat = ZCFMT_ResSymb | ZCFMT_DMYHMS ;

};




utf8VaryingString
searchKeywordWeighted(const utf8VaryingString& pIn);
utf8VaryingString
searchSymbolWeighted(const utf8VaryingString& pIn);

utf8VaryingString
searchSymbolApprox(const utf8VaryingString& pIn);
utf8VaryingString
searchKeywordApprox(const utf8VaryingString& pIn);

class CMatchResult
{
public:
    CMatchResult()=default;
    CMatchResult(const CMatchResult& pIn) {_copyFrom(pIn); }
    CMatchResult(int pMatch,int pRadix) : Match(pMatch),Radix(pRadix) {}
    CMatchResult& _copyFrom(const CMatchResult& pIn)
    {
        Match=pIn.Match;
        Radix=pIn.Radix;
        return *this;
    }
    CMatchResult& operator = (const CMatchResult& pIn) {return _copyFrom(pIn);}

    void clear()
    {
        Match=0;
        Radix=0;
    }

    int Match=0;
    int Radix=0;
};


class CWeight : public CMatchResult
{
public:
    CWeight()=default;
    CWeight(int pMatch,int pRadix,int pIndex) : CMatchResult(pMatch,pRadix),Index(pIndex) {}
    CWeight(CMatchResult pMR,int pIndex) : CMatchResult(pMR),Index(pIndex) {}
/*
    int Match=0;
    int Radix=0;
*/
    int Index=0;
};

class CWeightList : public ZArray<CWeight>
{
public:
    CWeightList()=default;
    CWeightList(CWeightList&)=delete;
    /* add in decreasing order */
    void add(CWeight& pIn)
    {
        int wi=0;
        while (( wi < count() ) && ( pIn.Match < Tab(wi).Match) )
            wi++;
        if ((wi < count())&&(pIn.Match == Tab(wi).Match)) {
            while (( wi < count() ) && ( pIn.Match == Tab(wi).Match) && ( pIn.Radix < Tab(wi).Radix) )
                wi++;
        }

        insert(pIn,wi);
    }

    void add(int pMatch,int pRadix,int pIndex)
    {
        CWeight wW(pMatch,pIndex,pRadix);
        add(wW);
    }
    void add(CMatchResult pMR,int pIndex)
    {
        CWeight wW(pMR,pIndex);
        add(wW);
    }
} ;

utf8VaryingString searchAdHocWeighted(const utf8VaryingString &pIn,
                                      ZArray<utf8VaryingString> &pKeyArray);

bool matchWeight(CMatchResult &pResult,
                 const utf8VaryingString &pIn,
                 const utf8VaryingString &pToMatch,
                 int pMatchMin = -1,        /* minimum number of characters to match */
                 int pRadixMin = 2);        /* minimum size of radix found */

utf8VaryingString
leftPad(const utf8VaryingString& pString,int pSize,utf8_t pPadChar=' ');

}//namespace zbs

#endif // ZSEARCHPARSER_H
