#ifndef ZSEARCHPARSER_H
#define ZSEARCHPARSER_H

#define __DEBUG__

#define __SEARCHPARSER_VERSION__  "0.30-0"

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

#include <zcontent/zrandomfile/zrandomfile.h>

namespace zbs {

extern class ZSearchParser* GParser;

//class ZSearchFormula;
class ZSearchCollection;

using namespace std;

  enum ZSearchParseAction : uint32_t {
    ZSPA_Nothing          =   0,

    ZSPA_SetFile          =   0x10,
    ZSPA_ReadOnly         =   0x12,
    ZSPA_Modify           =   0x14,

    ZSPA_Find             =   0x1000,
    ZSPA_All              =   0x2000,
    ZSPA_First            =   0x4000,

    ZSPA_MultipleEntities =   0x8000
    };

  class  ZSearchFileSymbol
  {
  public:
    ZSearchFileSymbol()=default;

    ZSearchFileSymbol(const ZSearchFileSymbol& pIn) {_copyFrom(pIn);}

    ZSearchFileSymbol& _copyFrom(const ZSearchFileSymbol& pIn) {
      Symbol=pIn.Symbol;
      Path=pIn.Path;
      return *this;
    }

    ZSearchFileSymbol& operator=(const ZSearchFileSymbol& pIn) {return _copyFrom(pIn);}

    utf8VaryingString Symbol;
    uriString         Path;
  };


class ZSearchFileSymbolList : public ZArray<ZSearchFileSymbol>
{
public:
  ZStatus getSymbolPath(const utf8VaryingString& pSymbol,uriString & pPath) {
    for (int wi=0;wi < count();wi++)
      if (pSymbol==Tab(wi).Symbol) {
        pPath=Tab(wi).Path;
        return ZS_SUCCESS;
      }
     return ZS_NOTFOUND;
  }

};



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



class ZMasterList : public ZArray<ZSearchMasterFile>
{
public:

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
      if (Tab(wi)->getName() == pEntityName) {
        return Tab(wi);
      }
    }
    return nullptr;
  }



};


class ZInstructionItem
{
public:
  ZInstructionItem()=default;
  ZInstructionItem (const utf8VaryingString& pPhrase,ZStatus pStatus) {
    Phrase = pPhrase;
    Status = pStatus;
  }

  ZInstructionItem(const ZInstructionItem& pIn) {_copyFrom(pIn);}

  ZInstructionItem& _copyFrom (const ZInstructionItem& pIn)
  {
    Status = pIn.Status;
    Phrase = pIn.Phrase;
    return *this;
  }

  utf8VaryingString Phrase;
  ZStatus           Status;
};

class ZSearchParserState;
class ZParserError : public ZaiErrors {
public:
  ZParserError(ZSearchParserState*     pFather) {Father=pFather;}
  void errorLog(const char* pFormat,...) ;
  void infoLog(const char* pFormat,...);
  void textLog(const char* pFormat,...);
  void warningLog(const char* pFormat,...);
  ZSearchToken* LastErroredToken=nullptr;
  ZSearchParserState*     Father=nullptr;
};


class ZSearchParserState : public ZParserError
{
public:
  ZSearchParserState(): ZParserError(this) {}
  ZSearchParserState(ZSearchParserState&)=delete;

  ~ZSearchParserState()
  {
    Entity.reset();
    for (int wi=0;wi < CurEntities.count();wi++)
      CurEntities[wi].reset();

  }
  void clearErrors() {
    ZaiErrors::clear();
  }

  void setIndex(int pIndex) {
//    IndexLog.push(Index);
    Index=pIndex;
  }
/*  void popIndex() {
    Index = IndexLog.popR();
  }
*/
  uriString                               URIMaster;
  std::shared_ptr<ZSearchEntity>          Entity=nullptr;

  ZArray<std::shared_ptr<ZSearchEntity>>  CurEntities;        /* current used entities within phrase */
  uint32_t                                Action=ZSPA_Nothing;
  int                                     Index=0;
  ZSearchToken*                           CurrentToken=nullptr;
//  ZArray<int>                             IndexLog;
//  ZaiErrors                               ErrorLog;
};




class ZSearchFieldOperandOwnData;
class ZSearchOperandBase;
class ZSearchArithmeticOperand;

class ZSearchParser : public ZSearchParserState
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
    Tokenizer->Options=pOptions;
    if (pOptions & ZSRCHO_Verbose)
      setAutoPrintOn(ZAIES_Warning);
    if (pOptions & ZSRCHO_FullVerbose)
      setAutoPrintOn(ZAIES_Text);
  }
  void addOptions(uint32_t pOptions) {
    Tokenizer->Options|=pOptions;
  }
  void removeOptions(uint32_t pOptions) {
    Tokenizer->Options&=~pOptions;
    if (pOptions & ZSRCHO_FullVerbose)
      setAutoPrintOn(ZAIES_None);
    if (pOptions & ZSRCHO_Verbose)
      setAutoPrintOn(ZAIES_None);
  }

  void clear() {
    Tokenizer->clear();
    Index=0;
  }

  void setIncludePath(const utf8VaryingString& pIncludePath);

  void finish() {}

  ZStatus parse(const utf8VaryingString &pContent) ;


  bool searchKeyword(ZSearchToken* pToken) ;

  ZStatus _parse(bool &pStoreInstruction);

  ZStatus parseAllocationFormula(const utf8VaryingString* pEntity,const utf8VaryingString& pSelectionPhrase);
  ZStatus _parsAllocationFormula(const utf8VaryingString* pEntity,ZSearchTokenizer* pTokenizer);
  bool    _parseComment (ZSearchToken* &pToken);

  ZStatus _parseShow();

  ZStatus _parseSetFile();
  ZStatus _parseSetHistoryMaximum();

  ZStatus _parseFind(std::shared_ptr<ZSearchEntity> &pCollection);
  ZStatus _parseFor();

  ZStatus _executeFind(std::shared_ptr<_BaseCollectionEntity> pCollection);



  /** Parses a field and all possible modifiers. Calls _parseFieldIdentifier() method */

  ZStatus _parseOperandField(void *&pTermOperand);
  ZStatus _parseLiteral(void *&pOperand);

  /** parse a field identifier and validate it according CurEntities array
   *  if more than one entity in CurEntities array, then field must be prefixed with its entity name
   *  if not, field may be prefixed or not by its entity name, it is reputated belonging to the unique entity
   *
   *  Field is validated against entity dictionary content.
   */
  ZStatus _parseFieldIdentifier (int & pEntityListIndex, ZSearchFieldOperandOwnData *pFOD);


 // ZStatus _parseLogicalOperand(void *&pOperand, int pParenthesisLevel, int pCollateral, int pBookMark);

  bool _parseZEntity(void *&pOperand);
  bool _parseEntity(void *&pOperand);
  bool _parseSymbol(void *&pOperand);

  ZStatus _getZDateOnePiece(int &pValue,int pMaxDigits,ZSearchTokentype pEndSeparator);

  ZStatus _parseLogicalTerm(ZSearchLogicalTerm *&pTerm,
                            int pParenthesisLevel, int pCollateral, int pBookMark, ZSearchOperandType &pMainType);

  ZStatus _parseArithmetic(ZSearchArithmeticOperand *&pArithOperand,  int pParenthesisLevel, int pCollateral, int pBookMark,ZSearchOperandType &pMainType);
//  ZStatus _parseFormula(ZSearchFormula *&pFormula, int pParenthesisLevel);

  bool _arithmeticTypeCheck(ZSearchOperandType& pMainType,ZSearchOperandBase* pOB);
  bool _logicalTypeCheck(ZSearchOperandType& pMainType,ZSearchOperandBase* pOB);
/*
  ZStatus _parseSelectionClause(ZSearchFormula *&pOutFormula, const utf8VaryingString &pEntityName, const ZMetaDic *pDictionary=nullptr );
*/
  ZStatus _finish();

  void    showHistory();
  void    showHistoryMaximum();
  void    setHistoryMaximum(int pMax);
  void    clearHistory();
  void    clearHistoryFirst (int pLines);
  void    clearHistoryLast (int pLines);
  void    clearHistoryRank (int pRank);

  void    showSymbols();
  void    showZEntities();
  void    showEntityFields (int pEntityIndex);

  void    showMasterFiles();
  void    showEntities();


  /** @brief init  clean all data structures */
  void init() {
    Tokenizer->clear();
    SymbolList.clear();
    ZEntityList.clear();
    EntityList.clear();
  }

//  void report(ZArray<ZSearchParserField> &pObjectList) const;


//public: ZParserError Errorlog;

public: ZSearchTokenizer*        Tokenizer=nullptr;

  uint32_t getOption() {return Tokenizer->Options;}
  bool isVerbose()  {return Tokenizer->Options & ZSRCHO_Verbose;}
  bool isFullVerbose()  {return Tokenizer->Options & ZSRCHO_FullVerbose;}
  bool hasOption(Options pOpt) {return Tokenizer->Options & pOpt;}

  /** @brief loadXmlSearchParserSymbols loads ZSearchParser symbols : translation for uri files path
   */
  ZStatus loadXmlSearchParserSymbols(const uriString& pXmlFile);

  /** @brief loadXmlSearchParserZEntity loads ZSearchParser ZEntity values for application
   */
  ZStatus loadXmlSearchParserZEntity(const uriString& pXmlFile);

  ZStatus loadXmlSearchParserParams(const uriString& pXmlFile);

private:
//  void loadIncludeFiles(ZSearchTokenizer &pTokenizer);

  bool advanceIndex();

  bool testSemiColon();

  ZTypeBase convertToZType(ZSearchTokentype_type pType);
  ConvertTuple TableConvert(ZSearchTokentype_type &pTokenType, const utf8VaryingString& pSyntax);

public:

  void setDisplayCallback(__DISPLAYCALLBACK__(pdisplayCallback) ) {
    Tokenizer->setDisplayCallback(pdisplayCallback);
    ZSearchParserState::setDisplayCallback(pdisplayCallback);
  }
  void setProgressCallback(__PROGRESSCALLBACK__(pdisplayCallback) ) {
    ProgressCallBack  = pdisplayCallback;
  }
  void setProgressSetupCallback(__PROGRESSCALLBACK__(pdisplayCallback) ) {
    ProgressCallBack  = pdisplayCallback;
  }
  void displayTokenList(ZArray<ZSearchToken*> &Whole);

  //  uint32_t              Options=ZCPO_Recursive;
  uriString             URIZEntity;
  uriString             URISymbol;
  uriString             URIParams;

  ZArray<ZSearchFileSymbol>                       SymbolList;
  ZArray<ZSearchFileZEntity>                      ZEntityList;
  ZSearchEntityList                               EntityList;
  ZArray<std::shared_ptr<ZSearchMasterFile>>      MasterFileList;


  /* parser clauses and rules */
//  class ZConversionRules      ConversionRules;

  /* parser internal data structures */

//  ZArray<ZInstructionItem>        InstructionLog;


//  ZArray<std::shared_ptr<ZSearchEntity>>          CollectionList;


  ZSearchHistory                  History;
  __PROGRESSCALLBACK__(ProgressCallBack)=nullptr;
  __PROGRESSCALLBACK__(ProgressSetupCallBack)=nullptr;

};


}//namespace zbs

#endif // ZSEARCHPARSER_H
