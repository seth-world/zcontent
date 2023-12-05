#ifndef ZSEARCHOPERAND_H
#define ZSEARCHOPERAND_H

#include <zcontentcommon/zresource.h>
#include <ztoolset/zdatefull.h>

#include "zsearchparsertype.h"

#include "zmetadic.h"

#include <zcontent/zcontentcommon/urfparser.h>

#include "zsearchentitycontext.h"

ZSearchOperandType getZSTOFromZType(ZTypeBase pZType);

namespace zbs {


enum TermScanStatus {
    TSST_Success = 0,
    TSST_MissOperand = 1,
    TSST_MissOperator = 2,
    TSST_MissLogTerm = 4,
    TSST_MissArithTerm = 8,
    TSST_Other  = 0x10
};

class CountTerm
{
public:
    CountTerm()=default;
    CountTerm(const CountTerm& pIn) {_copyFrom(pIn);}

    CountTerm& _copyFrom(const CountTerm& pIn)
    {
        LogOperator=pIn.LogOperator;
        LogOperand=pIn.LogOperand;
        LogTerm=pIn.LogTerm;
        ArithOperator=pIn.ArithOperator;
        ArithOperand=pIn.ArithOperand;
        ArithTerm=pIn.ArithTerm;
        Status=pIn.Status;
        return *this;
    }
    void clear()
    {
        LogOperator=0;
        LogOperand=0;
        LogTerm=0;
        ArithOperator=0;
        ArithOperand=0;
        ArithTerm=0;
        Status = TSST_Success ;
    }

    int LogOperator=0;
    int LogOperand=0;
    int LogTerm=0;
    int ArithOperator=0;
    int ArithOperand=0;
    int ArithTerm=0;
    int Status = TSST_Success ;
};




/* a Collection has its own name (identifier) and is a set of record addresses,
 * either extracted with a selection clause or manually made,
 * that refers to a ZSearchEntity, either file entity or collection entity (restricted view on entity) */

class ZOperandContent
{
public:
  ZOperandContent() ;
  ZOperandContent(const ZOperandContent& pIn) {_copyFrom(pIn);}
  ~ZOperandContent() { clear(); }


  ZOperandContent& _copyFrom(const ZOperandContent& pIn);
  ZOperandContent& operator = (const ZOperandContent& pIn) {return _copyFrom(pIn);}

  utf8VaryingString display();

  ZDataBuffer       toURF();
  ZStatus           appendURF(ZDataBuffer& pURFBuffer);

  bool isNull() const {return Value==nullptr;}

  void clear();

  bool operator == (const ZOperandContent& pIn) const;
  bool isSame (const ZOperandContent* pIn);

  long getInteger();
  ZStatus setInteger(long pValue);
  void replaceWithInteger(long pValue);

  double getFloat();
  ZStatus setFloat(double pValue);
  void replaceWithFloat(double pValue);

  bool getBool();
  ZStatus setBool(bool pValue);
  void replaceWithBool(bool pValue);

  utf8VaryingString getString();

  ZStatus setString(utf8VaryingString pValue);
  ZStatus setString(utfVaryingString<utf8_t> pValue);
  ZStatus setString(utf8_t *pValue);
  void replaceWithString(utf8VaryingString pValue);


  uriString getURI();
//  void setURI(uriString& pValue);
  ZStatus setURI(uriString pValue);
  void replaceWithURI(uriString pValue);

  ZDateFull getDate();
  ZStatus setDate(ZDateFull pValue);
  void replaceWithDate(ZDateFull pValue);

  ZResource getResource();
  ZStatus setResource(ZResource pValue);
  void replaceWithResource(ZResource pValue);

  checkSum getChecksum();
  ZStatus setChecksum(checkSum pValue);
  void replaceWithChecksum(checkSum pValue);

  utf8VaryingString displayContent();

  ZSearchOperandType_type   OperandZSTO=ZSTO_Nothing;
  void*                     Value=nullptr;

};

class ZSearchToken;
class ZMetaDic;

class ZSearchOperator
{
public:
  ZSearchOperator()=default;
  ZSearchOperator(ZSearchOperator_type pType) { ZSOPV = pType; }
  ZSearchOperator(const ZSearchOperator& pIn) { _copyFrom(pIn);  }
  ~ZSearchOperator();
  ZSearchOperator& _copyFrom (const ZSearchOperator& pIn) ;
  ZSearchOperator& operator = (const ZSearchOperator& pIn) {return _copyFrom(pIn);}

  void clear() {
    ZSOPV=ZSOPV_Nothing;
  }

  void set(ZSearchToken* pTokenOperator) ;
  void add(ZSearchToken* pTokenOperator) ;

  static bool isValid(ZSearchToken* pTokenOperator) ;

  static bool isComparator(ZSearchToken* pTokenOperator) ;
  static bool isLogical(ZSearchToken* pTokenOperator);
  static bool isStringComparator(ZSearchToken* pTokenOperator);
  static bool isArithmeric(ZSearchToken* pTokenOperator) ;

  static bool isAndOr(ZSearchToken* pTokenOperator) ;

  bool _isValid() ;

  bool _isComparator() ;
  bool _isLogical();
  bool _isStringComparator();
  bool _isArithmeric() ;
  bool _isAndOr();
  bool _isNothing();

  bool _isNot() { return ZSOPV == ZSOPV_NOT ;}

  bool _isAnd() { return ZSOPV = ZSOPV_AND; }
  bool _isOr() { return ZSOPV = ZSOPV_OR; }

  ZSearchOperator& operator = ( ZSearchOperandType pType ) { ZSOPV = pType; return *this; }

  ZSearchOperator_type get() { return ZSOPV; }

  utf8VaryingString     _report(int pLevel);
  utf8VaryingString     _reportFormula(bool pDetailed=true);

  ZSearchOperator_type  ZSOPV=ZSOPV_Nothing;
} ;

class ZSearchEntity;
class Mod_Translate {
public:
    Mod_Translate() = default;
    Mod_Translate(Mod_Translate& pIn) {_copyFrom(pIn);}
    Mod_Translate& _copyFrom(Mod_Translate& pIn)
    {
        Entity=pIn.Entity;
        KeyDicRank= pIn.KeyDicRank;
        ResultDicRank= pIn.ResultDicRank;
        Operator= pIn.Operator;
        return *this;
    }

    std::shared_ptr<ZSearchEntity> Entity=nullptr;
    int                             KeyDicRank=0;
    int                             ResultDicRank=0;
    ZSearchOperator_type            Operator= ZSOPV_EQUAL ;

};
/*
struct Mod_Date {
    int Content;
};
*/
class Mod_SubString {
public:
    Mod_SubString() = default;
    Mod_SubString(Mod_SubString& pIn) {_copyFrom(pIn);}
    Mod_SubString& _copyFrom(Mod_SubString& pIn)
    {
        Offset=pIn.Offset;
        Length= pIn.Length;
        return *this;
    }
    int Offset=0;
    int Length=0;
};

class Mod_LeftRight {
public:
    Mod_LeftRight() = default;
    Mod_LeftRight(Mod_LeftRight& pIn) {_copyFrom(pIn);}
    Mod_LeftRight& _copyFrom(Mod_LeftRight& pIn)
    {
        Length= pIn.Length;
        return *this;
    }
    int Length=0;
};

/*
struct Mod_Entity {
    int Content=0;
};

struct Mod_Uri {
    utf8VaryingString Content;
};
*/
class ZSearchOperandBase : public ZOperandContent
{
public:
  ZSearchOperandBase() {}
  ZSearchOperandBase(ZSearchOperandType_type pType) {ZSTO=pType;}

  ZSearchOperandBase(const ZSearchOperandBase& pIn) : ZOperandContent(pIn) {_copyFrom(pIn);}
  ~ZSearchOperandBase();


  ZSearchOperandBase& _copyFrom(const ZSearchOperandBase& pIn);

  ZSearchOperandBase& _fullCopyFrom(const ZSearchOperandBase& pIn)
  {
      ZOperandContent::_copyFrom(pIn);
      return _copyFrom(pIn);
  }


  ZSearchOperandType_type getType() {return ZSTO;}

  bool hasModifier() {return ModifierType!=ZSRCH_NOTHING;}

  ZSearchOperandBase& operator = (const ZSearchOperandBase& pIn) {return _fullCopyFrom(pIn); }

  utf8VaryingString     _reportModifier(int pLevel);
  utf8VaryingString     _reportModifierBrief();

  void clear();

  void setZSTO(ZSearchOperandType_type pZSTO)
  {
      ZSTO=pZSTO;
      OperandZSTO=pZSTO;
  }

  ZSearchOperandType_type   ZSTO=ZSTO_Nothing;
  ZSearchTokentype_type     ModifierType=ZSRCH_NOTHING;
  utf8VaryingString         Comment;
  utf8VaryingString         FullFieldName;
  void *                    ModParams=nullptr;
  URFParser*                _URFParser=nullptr;
}; // ZSearchOperandBase



/** operand type is located within ZSearchOperandBase */

//class ZSearchFieldOperand :  public ZSearchFieldOperandOwnData , public ZOperandContent
class ZSearchFieldOperand :  public ZSearchOperandBase
{
public:
  ZSearchFieldOperand()=default;
    ZSearchFieldOperand(std::shared_ptr<ZSearchEntity> pEntity, const ZMetaDic* pMDic,long pMDicRank)
  {
      ZSearchOperandType_type wZSTO=getZSTOFromZType(pMDic->TabConst(pMDicRank).ZType);
      Entity = pEntity;
      MetaDicPtr = pMDic;
      MetaDicRank = pMDicRank;
      ZSTO = wZSTO | ZSTO_Field;
      OperandZSTO=ZSTO;
      FullFieldName = pMDic->TabConst(pMDicRank).getName();
  }
    ZSearchFieldOperand(const ZSearchFieldOperand& pIn) : ZSearchOperandBase(pIn) {_copyFrom(pIn);}
  ~ZSearchFieldOperand() { }

  ZSearchFieldOperand& _copyFrom(const ZSearchFieldOperand& pIn)
  {
      MetaDicRank=pIn.MetaDicRank;
      MetaDicPtr=pIn.MetaDicPtr;
      Entity = pIn.Entity;
      IndexRank=pIn.IndexRank;  /* index rank to search in if not equal to -1 */
      return *this;
  }

  ZSearchFieldOperand& _fullCopyFrom(const ZSearchFieldOperand& pIn)
  {
      ZSearchOperandBase::_copyFrom(pIn);
      return _copyFrom(pIn);
  }

  bool isValid() { return ZSTO != ZSTO_Nothing; }
  bool isNull() { return Value == nullptr; }

  ZTypeBase getZType()
  {
      return MetaDicPtr->TabConst(MetaDicRank).ZType;
  }

  ZSearchFieldOperand& operator = (const ZSearchFieldOperand& pIn) {return _fullCopyFrom(pIn); }

  ZStatus getURFContent(ZSearchEntityContext &pSEC, URFField &pURFField);

  std::shared_ptr<ZSearchEntity>        Entity=nullptr;
  long              MetaDicRank=0;     /* input record field rank */
  long              ZSEDicRank=0;   /* ZSearch Entity dictionary rank */
  const ZMetaDic*   MetaDicPtr=nullptr;
  long              IndexRank=-1;  /* index rank to search in if not equal to -1 */
};

/*
 *
Valid ZEntity literals

  ZEntity(ZEntity_DocPhysical,0xAB);

Valid checSum literals

  checkSum(0x22890F5D10ADBOF001288890AFB22C3);
             <-------32 hexa digits-------->

Valid date literals

  ZDate(10/12/2023)
  "10/12/2023"

*/

/** operand type is located within ZSearchOperandBase */

/* literal is a content */
class ZSearchLiteral :  public ZSearchOperandBase  {
public:
    ZSearchLiteral(): ZSearchOperandBase(ZSTO_Literal) {}
    ZSearchLiteral(ZSearchOperandType_type pZSTO): ZSearchOperandBase(pZSTO) {}
    ZSearchLiteral(ZSearchOperandBase& pOB)
    {
        ZSearchOperandBase::_copyFrom(pOB);
    }
    ZSearchLiteral(const ZSearchLiteral& pIn) : ZSearchOperandBase(pIn) {}
    ~ZSearchLiteral() { }

    void setOperandBase(ZSearchOperandBase& pOB) {
        ZSearchOperandBase::_copyFrom(pOB);
        ZOperandContent::OperandZSTO = pOB.ZSTO;
    }

    ZSearchLiteral& _copyFrom(const ZSearchLiteral& pIn)
    {
        ZSearchOperandBase::_copyFrom(pIn);
        return *this;
    }

    ZSearchLiteral& operator=(const ZSearchLiteral& pIn) {return _copyFrom(pIn);}

    ZSearchOperandBase* getOperandBase() {return (ZSearchOperandBase*)this;}

};


/** Former ZSearchTerm
 *
 * a term is a combination of 2 elements fields or literals
 *  and a logical operator that when evaluated, gives a boolean result true or false */

class ZSearchLogicalOperand : public ZSearchOperandBase
{
public:

  ZSearchLogicalOperand() : ZSearchOperandBase(ZSTO_Logical) {}
  ZSearchLogicalOperand(const ZSearchLogicalOperand& pIn) : ZSearchOperandBase(pIn) {_copyFrom(pIn);}
  ~ZSearchLogicalOperand();
  ZSearchLogicalOperand& _copyFrom(const ZSearchLogicalOperand& pIn) ;

  ZSearchLogicalOperand& operator = (const ZSearchLogicalOperand& pIn) {return _copyFrom(pIn);}

  void copyOperand(void *&pOperand, const void *pOpIn) ;

  void clear();

  utf8VaryingString             _report(int pLevel);

  utf8VaryingString             _reportBrief(int pLevel);

  utf8VaryingString             _reportDetailed(int pLevel);

  utf8VaryingString             _reportFormula();

  static utf8VaryingString      _evaluateOpLiteral(void *pOp);

  /* URFParser is set at the entity level, while accessing records */

  bool evaluate(ZSearchEntityContext &pSEC);
  //  ZOperandContent getContent(URFParser& pURFParser);

  bool            isNull() const {return Operand==nullptr;}

  void*           Operand=nullptr;

};

const char *decode_TSST(int pSt);

} // namespace zbs

ZStatus getURFOperandContent(URFField &wField, ZOperandContent* pOperandContent);
ZStatus getLiteralOperandContent(void *pLiteral, ZOperandContent* pOperandContent);

ZStatus getURFOperandContentWModifier(ZSearchEntityContext &pSEC,
                                      URFField &wField,
                                      ZSearchTokentype_type pModifier,
                                      void *pModParams,
                                      ZOperandContent *pOperandContent);

ZStatus setStringOperandModifierValue(utf8VaryingString &pRawValue,
                                      ZSearchTokentype_type pModifier,
                                      void *pModParams,
                                      ZOperandContent *pOperandContent);

ZStatus evaluateTerm(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator);

/** extracts and returns one operands value :
 *  either gets the literal
 *  or extracts the corresponding fields from record using URFParser
 *  applies modifier if any declared
 */

ZStatus extractOperandContent(ZSearchEntityContext &pSEC,
                              void *pOperand,
                              ZOperandContent *pOperandContent);
/*
ZStatus extractOperandContent(void *pOperand,
                              URFParser &pURFParser,
                              ZOperandContent* pOperandContent);
*/
/**
 * @brief buildOperandContent  builds a content for pOperandContent using an input operand definition pOp :
 *  Extracts data fields from entities using URFParser that has to be loaded appropriately
 *  Makes possible conversions
 *  Computes logical or arithmetic expression,
 *  Applies possible modifiers
 * @return a ZStatus
 */
ZStatus buildOperandContent( ZSearchEntityContext &pSEC,void *pOp, ZOperandContent *pOperandContent);

utf8VaryingString OperandReportFormula(void * pOperand, bool pDetailed=true);
utf8VaryingString OperandReportDetailed(void *pOperand, int pLevel);
utf8VaryingString OperandReportLiteral(void *pOp);

void clearOperand(void *&pOp);  /* here because it is common to any derived operand class */
void _copyOperand(void *&pOperand, const void *pOpIn) ;

//ZSearchOperandType getZSTOFromZType(ZTypeBase pZType);

//ZStatus constructEmptyOperand(void*& pOperand,ZSearchOperandType_type pZSTOType);
//ZStatus constructOperandFromField(void*& pOperand,const ZFieldDescription* pFd);

//utf8VaryingString displayOperandContent(ZOperandContent* pContent);

#endif // ZSEARCHOPERAND_H
