#ifndef ZSEARCHLOGICALTERM_H
#define ZSEARCHLOGICALTERM_H


/*
 *  a logical term consist in
 *
 * - a couple of logical operand to be compared
 * - an optional leading NOT operator
 * - a mandatory comparizon operand
 *
 * - a pointer to next logical term to be evaluated or nullptr if there is no following term
 *
 *
 *
 * a logical operand may consist in
 *
 *  - a literal
 *  - a field with or without field modifiers
 *  - a logical term : in case of parenthesis, then an operand is created as a new logical term
 *
 *  - an arithmetic formula : in this case the result is
 *    - a pure literal if all members of the arithmetic formula are literal.
 *      In this case, formula is evaluated once and result is stored as a literal.
 *
 *    - a formula that involves fields contents, including using field modifiers.
 *      In this case, arithmetic formula is evaluated for each record
 *
 *
 * logical term evaluation :
 *
 *
 */


#include "zsearchoperand.h"
namespace zbs {
class ZSearchLogicalTerm
{
public:
  ZSearchLogicalTerm() ;
  ZSearchLogicalTerm(const ZSearchLogicalTerm& pIn) {_copyFrom(pIn);}

  ZSearchLogicalTerm& _copyFrom(const ZSearchLogicalTerm& pIn);

  void setOperand1(const ZSearchLogicalOperand& pOperand1);
  void setOperand2(const ZSearchLogicalOperand& pOperand2);

  utf8VaryingString             _report(int pLevel);
  utf8VaryingString             _reportFormula(bool pHasParenthesis=false);

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
} //namespace zbs
#endif // ZSEARCHLOGICALTERM_H
