#ifndef ZSEARCHOPERAND_H
#define ZSEARCHOPERAND_H

#include <zcontentcommon/zresource.h>
#include <ztoolset/zdatefull.h>

#include "zsearchparsertype.h"
#include "zfielddescription.h"

#include <zcontent/zcontentcommon/urfparser.h>

namespace zbs {

/* a Collection has its own name (identifier) and is a set of record addresses,
 * either extracted with a selection clause or manually made,
 * that refers to a ZSearchEntity, either file entity or collection entity (restricted view on entity) */

class ZOperandContent
{
public:
  ZOperandContent() ;
  ZOperandContent(const ZOperandContent& pIn) {_copyFrom(pIn);}

  ZOperandContent& _copyFrom(const ZOperandContent& pIn);
  ZOperandContent& operator = (const ZOperandContent& pIn) {return _copyFrom(pIn);}

  ZSearchOperandType Type=ZSTO_Nothing;
  long              Integer;
  utf8VaryingString String;
  uriString         URI;
  double            Float;
  bool              Bool;
  ZDateFull         Date;
  ZResource         Resource;
  checkSum          CheckSum;
};




class ZSearchToken;
class ZMetaDic;



class ZSearchOperator
{
public:
  ZSearchOperator()=default;
  ZSearchOperator(ZSearchOperator_type pType) {Type = pType;}
  ZSearchOperator(const ZSearchOperator& pIn) { _copyFrom(pIn);  }
  ~ZSearchOperator();
  ZSearchOperator& _copyFrom (const ZSearchOperator& pIn) ;
  ZSearchOperator& operator = (const ZSearchOperator& pIn) {return _copyFrom(pIn);}

  void clear() {
    while (TokenList.count())
      TokenList.pop();
    Type=ZSOPV_Nothing;
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

  bool _isNot() {return Type == ZSOPV_NOT ;}

  bool _isAnd() {return Type = ZSOPV_AND; }
  bool _isOr() {return Type = ZSOPV_OR; }

  ZSearchOperator& operator = (ZSearchOperandType pType) {Type = pType; return *this;}

  ZSearchOperator_type get() {return Type;}

  utf8VaryingString     _report(int pLevel);

  ZSearchOperator_type  Type=ZSOPV_Nothing;
  ZArray<ZSearchToken*> TokenList;
} ;


class ZSearchOperandBase
{
public:
  ZSearchOperandBase() {}
  ZSearchOperandBase(ZSearchOperandType pType) {Type=pType;}

  ZSearchOperandBase(const ZSearchOperandBase& pIn) {_copyFrom(pIn);}
  ~ZSearchOperandBase() {
    while (TokenList.count()>0) {
      TokenList.pop();
    }
  }

  ZSearchOperandBase& _copyFrom(const ZSearchOperandBase& pIn)
  {
    Type = pIn.Type;
    ModifierType = pIn.ModifierType;
    ModVal1 = pIn.ModVal1;
    ModVal2 = pIn.ModVal2;
    for (long wi=0; wi < pIn.TokenList.count();wi++)
      TokenList.push(pIn.TokenList[wi]);

    return *this;
  }

  static void clearOperand(void *&pOp);  /* here because it is common to any derived class */

  ZSearchOperandBase& operator = (const ZSearchOperandBase& pIn) {return _copyFrom(pIn); }

  ZSearchOperandType  Type=ZSTO_Nothing;
  ZSearchTokentype_type ModifierType=ZSRCH_NOTHING;
  long ModVal1=0;
  long ModVal2=0;
  ZArray<ZSearchToken*> TokenList;
};


class ZSearchFieldOperandOwnData : public ZSearchOperandBase
{
public:
  ZSearchFieldOperandOwnData()=default;
  ZSearchFieldOperandOwnData(const ZSearchFieldOperandOwnData& pIn) {_copyFrom(pIn);}
  ZSearchFieldOperandOwnData& _copyFrom(const ZSearchFieldOperandOwnData& pIn)
  {
    ZSearchOperandBase::_copyFrom(pIn);
    MDicRank=pIn.MDicRank;
    MDic=pIn.MDic;
    IndexRank=pIn.IndexRank;  /* index rank to search in if not equal to -1 */
    FullFieldName=pIn.FullFieldName;
    FieldDescription._copyFrom(pIn.FieldDescription);
    return *this;
  }
  ZSearchFieldOperandOwnData& operator=(const ZSearchFieldOperandOwnData& pIn) {return _copyFrom(pIn);}
  long              MDicRank=0;
  const ZMetaDic*   MDic=nullptr;
  long              IndexRank=-1;  /* index rank to search in if not equal to -1 */
  ZFieldDescription FieldDescription;
  utf8VaryingString FullFieldName;
};


/** operand type is located within ZSearchOperandBase */
template <class _Tp>
class ZSearchFieldOperand : public ZSearchFieldOperandOwnData {
public:
  ZSearchFieldOperand()=default;
  ZSearchFieldOperand(const ZSearchFieldOperand& pIn) {_copyFrom(pIn);}
  ~ZSearchFieldOperand() { }

  //  void setOperandBase(const ZSearchOperandBase& pOB) {ZSearchOperandBase::_copyFrom(pOB);}
  void setOwnData(const ZSearchFieldOperandOwnData& pIn) {ZSearchFieldOperandOwnData::_copyFrom(pIn);}
  void setFieldDescription(const ZFieldDescription& pFD) {FieldDescription._copyFrom(pFD);}

  ZSearchFieldOperand<_Tp>& _copyFrom(const ZSearchFieldOperand<_Tp>& pIn)
  {
    //    ZSearchOperandBase::_copyFrom((ZSearchOperandBase)pIn);
    ZSearchFieldOperandOwnData::_copyFrom((ZSearchFieldOperandOwnData)pIn);
    Content = pIn.Content;
    return *this;
  }


  ZSearchFieldOperand& operator = (const ZSearchFieldOperand& pIn) {return _copyFrom(pIn); }
  _Tp               Content;
};

/** operand type is located within ZSearchOperandBase */
template <class _Tp>
class ZSearchLiteral : public ZSearchOperandBase {
public:
  ZSearchLiteral()=default;
  ZSearchLiteral(const ZSearchLiteral& pIn) {_copyFrom(pIn);}
  ~ZSearchLiteral() { }

  ZSearchLiteral<_Tp>& _copyFrom(const ZSearchLiteral<_Tp>& pIn)
  {
    ZSearchOperandBase::_copyFrom(pIn);
    Content=pIn.Content;
    return *this;
  }
  _Tp Content;
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

  void clear()
  {
/*    ParenthesisLevel=0;
    Collateral=0;
    LeadingOperator.clear();
    Operator.clear();
*/
    clearOperand(Operand);
//    clearOperand(OperandNext);
  }


  utf8VaryingString             _report(int pLevel);

  static utf8VaryingString      _evaluateOpLiteral(void *pOp);

  /* URFParser is set at the entity level, while accessing records */

  bool            evaluate(URFParser &pURFParser);
  ZOperandContent getContent(URFParser& pURFParser);

  bool            isNull() const {return Operand==nullptr;}

  /*
  int             ParenthesisLevel=0;
  int             Collateral=0;
  ZSearchOperator LeadingOperator=ZSTO_Nothing;
*/
  void*           Operand=nullptr;
/*
  ZSearchOperator Operator;
  void*           OperandNext=nullptr;
*/
};


class ZSearchLogicalTerm
{
public:
  ZSearchLogicalTerm() {}
  ZSearchLogicalTerm(const ZSearchLogicalTerm& pIn) {_copyFrom(pIn);}

  ZSearchLogicalTerm& _copyFrom(const ZSearchLogicalTerm& pIn);

  utf8VaryingString             _report(int pLevel);

  bool            evaluate(URFParser &pURFParser);

  int             ParenthesisLevel=0;
  int             Collateral=0;
  ZSearchOperator NotOperator=ZSOPV_Nothing;
  ZSearchLogicalOperand Operand1;
  ZSearchOperator CompareOperator;
  ZSearchLogicalOperand Operand2;
  ZSearchOperator AndOrOperator=ZSOPV_Nothing;
  ZSearchLogicalTerm* NextTerm=nullptr;
};

/* Arithmetic expression as operand */
class ZSearchArithmeticOperand : public ZSearchOperandBase {
public:
  ZSearchArithmeticOperand(): ZSearchOperandBase(ZSTO_Arithmetic)  {}
  ZSearchArithmeticOperand(const ZSearchArithmeticOperand& pIn) : ZSearchOperandBase(ZSTO_Arithmetic) {_copyFrom(pIn);}
  //  ZSearchExpressionOperand(const ZSearchExpressionOperand& pIn)=delete;
  ~ZSearchArithmeticOperand() ;

  void setOperandBase(const ZSearchOperandBase& pOB) {ZSearchOperandBase::_copyFrom(pOB);}

  ZSearchArithmeticOperand& _copyFrom(const ZSearchArithmeticOperand& pIn);

  utf8VaryingString _report(int pLevel);

  /* initializes and copies to current operand pOperand (either Operand or OperandNext) corresponding data from pOpIn */
  void copyOperand(void *&pOperand, const void *pOpIn) ;
  static void _copyOperand(void *&pOperand, const void *pOpIn) ;

  /* compute() evaluate arithmetic expression and all related descendants and returns a qualified literal operand as void*
   *
   * URFParser is set at the entity level, while accessing records
    */
  ZOperandContent compute(URFParser &pURFParser);

  int             ParenthesisLevel=0;
  int             Collateral=0;
  void*           Operand=nullptr;        /* may be either field, literal or ZSearchArithmeticOperand* */
  ZSearchOperator Operator;               /* arithmetic operator */
  void*           OperandNext=nullptr;     /* may be either field, literal or ZSearchArithmeticOperand* */
};




} // namespace zbs


ZOperandContent getFieldOperandContent (URFField& wField);
ZOperandContent getLiteralOperandContent (void* pLiteral);

ZStatus evaluateTerm(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator);

ZOperandContent gettermOperandContent (void* pOperand, URFParser &pURFParser);

#endif // ZSEARCHOPERAND_H
