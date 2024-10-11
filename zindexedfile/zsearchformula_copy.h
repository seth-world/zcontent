#ifndef ZSEARCHFORMULA_H
#define ZSEARCHFORMULA_H

#include <ztoolset/zarray.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zdatefull.h>
#include <zcontentcommon/zresource.h>

#include "zsearchparsertype.h"

#include "zfielddescription.h"

namespace zbs {

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

  bool _isValid() ;

  bool _isComparator() ;
  bool _isLogical();
  bool _isStringComparator();
  bool _isArithmeric() ;


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


/*
   <field> + ( <field> + <literal> / (<literal> + <field> )  ) - <literal>

  ZSearchArith
  -----------------
(op <field>)  (oper +)   (next ZSearchArith)

                          (op ZSearchArith)    (oper -)   (next <literal>)

            (op <literal>)  (oper +)  (next ZSearchArith)

                                        (op <literal> (oper /) (next ZSearchArith)

                                                                  (op <literal>)  (oper +) (next <literal>)



*/

class ZSearchFormula;
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

  /* compute() evaluate arithmetic expression and all related descendants and returns a qualified literal operand as void* */
  void* compute();

  int             ParenthesisLevel=0;
  int             Collateral=0;
  void*           Operand=nullptr;        /* may be either field, literal or ZSearchArithmeticOperand* */
  ZSearchOperator Operator;               /* arithmetic operator */
  void*           OperandNext=nullptr;     /* may be either field, literal or ZSearchArithmeticOperand* */
};




/*
  <field> == <literal>
  <literal> > <field>

*/

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
    ParenthesisLevel=0;
    Collateral=0;
    LeadingOperator.clear();
    Operator.clear();
    clearOperand(Operand);
    clearOperand(OperandNext);
  }


  utf8VaryingString             _report(int pLevel);
  static utf8VaryingString      _evaluateOpLiteral(void *pOp);

  bool            evaluate();

  int             ParenthesisLevel=0;
  int             Collateral=0;
  ZSearchOperator LeadingOperator=ZSTO_Nothing;
  void*           Operand=nullptr;
  ZSearchOperator Operator;
  void*           OperandNext=nullptr;

};


enum ZFormulaType : int {
  ZFORT_Nothing     = 0,
  ZFORT_Term        = 1,    /* points to logical term class ZSeach*/
  ZFORT_Formula     = 2,    /* points to another formula */
  ZFORT_Arithmetic  = 3     /* points to a ZSearchArithmeticOperand */
};


/*
                           +-->trailing link operator
                           |                      ^       +-> open parenthesis = new embedded formula    +--> end embedded formula
                           |                      |       |                                              |
     <field> == <literal> AND <field> < <literal> OR [NOT]( <field> == <literal> AND <field> > <literal> ) AS
                                                            <----term 31------->     <------term 32---->
     <-----term 1------->     <-------term 2---->           <-----Formula 21----> <-----Formula 22---->
                                                               ^              |    |
                                                               |              +----+
                                                             embedded(term is formula)
     <---formula 0-----------> <-------formula 1------> <-------formula 2---->
                            |    |                  |    |
                            +----+                  +----+
                           chained
*/
/* a formula is a term and a leading link logical operator
 * a formula has a result of true or false and is combined with a logical operator {AND-OR} to global logical result
 *        formula result may be inversed by leading NOT operator.
*/
class ZSearchFormula
{
public:
  ZSearchFormula()=default;
  ZSearchFormula(ZFormulaType pType,int pParenthesisLevel) : FormulaType(pType),ParenthesisLevel(pParenthesisLevel) {}
  ZSearchFormula(const ZSearchFormula& pIn) {_copyFrom(pIn);}
  ~ZSearchFormula() {
    clear();
    return;
  }

  void clear();


  ZSearchFormula& _copyFrom(const ZSearchFormula& pIn) ;

  ZSearchFormula& operator = (const ZSearchFormula& pIn) { return _copyFrom(pIn);}

  ZSearchFormula* getNext()
  {
    return NextFormula;
  }

  void setType(ZFormulaType pType) {FormulaType=pType;}

//  bool _evaluate(const ZDataBuffer& pRecord);

  utf8VaryingString _report(int pLevel=0);


  ZSearchOperator       LeadingOperator;    /* Leading link operator Allowed operators : <nothing> AND OR (NOT combined with previous) -
                                             link with following formula after evaluation */
  ZFormulaType          FormulaType=ZFORT_Term ;    /* 1 : ZSearchTerm (logical op) - 2 : ZSearchFormula -3 : ZFORT_Arithmetic (arithmetic op) */
  void*                 TermOrFormula=nullptr;
  ZSearchFormula*       NextFormula=nullptr;
  int                   ParenthesisLevel=0;
};


class ZSearchFormulaList : public ZArray <ZSearchFormula>
{
public:
};


const char* decode_ZFormulaType(ZFormulaType pType);

} // namespace zbs
#endif // ZSEARCHFORMULA_H
