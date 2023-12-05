#ifndef ZSEARCHARITHMETICTERM_H
#define ZSEARCHARITHMETICTERM_H

#include "zsearchoperand.h"
namespace zbs {
class ZSearchArithmeticTerm;

extern ZSearchOperandType ZSTO_Def;
ZStatus computeArithmeticLiteral(ZSearchArithmeticTerm *&pOperand,
                                 ZSearchOperandType_type &pMainType,
                                 ZOperandContent &pOperandContent);
ZStatus computeArithmetic(ZSearchEntityContext &pSEC,
                          ZSearchArithmeticTerm *&pOperand,
                          ZSearchOperandType_type &pMainType,
                          ZOperandContent &pOperandContent);

ZStatus computeArithmeticOperation(ZOperandContent &pOperand1,
                                   ZOperandContent &pOperand2,
                                   ZSearchOperator &pOperator,
                                   ZSearchOperandType_type pRequestedType,
                                   ZOperandContent &pResult);




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
/*
  ZStatus compute(URFParser &pURFParser, ZSearchOperandType_type& pMainType);
  ZStatus computeLiteral(ZSearchOperandType_type& pMainType)
  {
      return computeArithmeticLiteral(this,pMainType,this);
  }
*/
  utf8VaryingString _report(int pLevel);
  utf8VaryingString _reportFormula(bool pDetailed=true);
  /* initializes and copies to current operand pOperand (either Operand or OperandNext) corresponding data from pOpIn */
  void copyOperand(void *&pOperand, const void *pOpIn) ;

  void setName (const utf8VaryingString& pName)
  {
      FullFieldName = pName;
  }

  /* compute() evaluate arithmetic expression and all related descendants and returns a qualified literal operand as void*
   *
   * URFParser is set at the entity level, while accessing records
    */
//  ZOperandContent compute(URFParser &pURFParser);

  /* compute arithmetic expression containing all literal operands */
//  ZOperandContent computeLiteral();

  ZSearchOperandType_type getOperandType() {
    if (Operand==nullptr)
      return ZSTO_Nothing;
    return static_cast<ZSearchOperandBase*>(Operand)->ZSTO;
  }

  int             ParenthesisLevel=0;
  //  int             Collateral=0;
  void*           Operand=nullptr;        /* may be either field, literal, Symbol or ZSearchArithmeticTerm  */
  ZSearchOperator Operator;               /* arithmetic operator */
  void*           OperandNext=nullptr;    /* may be either field, literal, Symbol or ZSearchArithmeticTerm  */
};


void _countArithmeticTerm(ZSearchArithmeticTerm* pArithTerm,CountTerm &wCT);

} // namespace zbs


#endif // ZSEARCHARITHMETICTERM_H
