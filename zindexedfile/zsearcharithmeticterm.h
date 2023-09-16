#ifndef ZSEARCHARITHMETICTERM_H
#define ZSEARCHARITHMETICTERM_H

#include "zsearchoperand.h"



namespace zbs {


/* Arithmetic expression term */
class ZSearchArithmeticTerm : public ZSearchOperandBase {
public:
  ZSearchArithmeticTerm(): ZSearchOperandBase(ZSTO_Arithmetic)  {}
  ZSearchArithmeticTerm(const ZSearchArithmeticTerm& pIn) : ZSearchOperandBase(ZSTO_Arithmetic) {_copyFrom(pIn);}
  ZSearchArithmeticTerm(const ZSearchArithmeticTerm* pIn) : ZSearchOperandBase(ZSTO_Arithmetic) {_copyFrom(pIn);}
  //  ZSearchExpressionOperand(const ZSearchExpressionOperand& pIn)=delete;
  ~ZSearchArithmeticTerm() ;

  void setOperandBase(const ZSearchOperandBase& pOB) {ZSearchOperandBase::_copyFrom(pOB);}

  ZSearchArithmeticTerm& _copyFrom(const ZSearchArithmeticTerm& pIn);
  ZSearchArithmeticTerm& _copyFrom(const ZSearchArithmeticTerm* pIn);

  utf8VaryingString _report(int pLevel);
  utf8VaryingString _reportFormula(bool pDetailed=true);
  /* initializes and copies to current operand pOperand (either Operand or OperandNext) corresponding data from pOpIn */
  void copyOperand(void *&pOperand, const void *pOpIn) ;


  /* compute() evaluate arithmetic expression and all related descendants and returns a qualified literal operand as void*
   *
   * URFParser is set at the entity level, while accessing records
    */
//  ZOperandContent compute(URFParser &pURFParser);

  /* compute arithmetic expression containing all literal operands */
//  ZOperandContent computeLiteral();

  ZSearchOperandType getOperandType() {
    if (Operand==nullptr)
      return ZSTO_Nothing;
    return static_cast<ZSearchOperandBase*>(Operand)->Type;
  }

  int             ParenthesisLevel=0;
  //  int             Collateral=0;
  void*           Operand=nullptr;        /* may be either field, literal or ZSearchArithmeticTerm* */
  ZSearchOperator Operator;               /* arithmetic operator */
  void*           OperandNext=nullptr;    /* may be either field, literal or ZSearchArithmeticTerm* */
};


} // namespace zbs

extern ZSearchOperandType ZSTO_Def;

ZOperandContent computeArithmeticLiteral(void*& pOperand, ZSearchOperandType & pMainType=ZSTO_Def);
ZOperandContent computeArithmetic(void*& pOperand, URFParser &pURFParser, ZSearchOperandType & pMainType=ZSTO_Def);
#endif // ZSEARCHARITHMETICTERM_H
