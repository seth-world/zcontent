#include "zsearchlogicalterm.h"

using namespace zbs;

ZSearchLogicalTerm::ZSearchLogicalTerm()
{

}
ZSearchLogicalTerm&
ZSearchLogicalTerm::_copyFrom(const ZSearchLogicalTerm& pIn)
{
  _DBGPRINT("ZSearchLogicalTerm::_copyFrom\n")
  NotOperator._copyFrom(pIn.NotOperator);
  CompareOperator._copyFrom(pIn.CompareOperator);
  AndOrOperator._copyFrom(pIn.AndOrOperator);

  Operand1.clear();
  if (!pIn.Operand1.isNull())
  {
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom Operand1\n")
    Operand1._copyFrom(pIn.Operand1);
  }

  Operand2.clear();
  if (!pIn.Operand2.isNull())
  {
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom Operand2\n")
    Operand2._copyFrom(pIn.Operand2);
  }
  else
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom Operand2 is NULL\n")

  if (NextTerm!=nullptr) {
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom copy NextTerm \n")
    NextTerm = new ZSearchLogicalTerm(*pIn.NextTerm);
  }
  else
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom NextTerm is NULL \n")
  return *this;
}



void
ZSearchLogicalTerm::setOperand1(const ZSearchLogicalOperand& pOperand)
{
  Operand1.clear();
  if (!pOperand.isNull())
  {
    _DBGPRINT("ZSearchLogicalTerm::setOperand1\n")
    Operand1._copyFrom(pOperand);
  }
}
void
ZSearchLogicalTerm::setOperand2(const ZSearchLogicalOperand& pOperand)
{
  Operand1.clear();
  if (!pOperand.isNull())
  {
    _DBGPRINT("ZSearchLogicalTerm::setOperand2\n")
    Operand2._copyFrom(pOperand);
  }
}

bool
ZSearchLogicalTerm::evaluate(  URFParser& pURFParser)
{
  bool wResult=true;
  _DBGPRINT("ZSearchLogicalTerm::evaluate operand 1\n")

  ZOperandContent wC1 = Operand1.getContent(pURFParser);
  if ((wC1.Type == ZSTO_Logical) || (wC1.Type & ZSTO_Bool)) {

    wResult = wC1.Bool;

    if (NotOperator._isNot())
      wResult = !wResult;

    if (!AndOrOperator._isAndOr()) {
      _DBGPRINT("getOneTerm Expected operator {AND,OR} while getting <%s>\n",
          decode_ZSOPV(AndOrOperator.Type))
      abort();
    }
    else
      _DBGPRINT("ZSearchLogicalTerm::evaluate operator is %s\n", decode_ZSOPV(AndOrOperator.Type))

    if (Operand2.isNull()) {
      _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2 is NULL\n")
      return wResult;
    }
  } // if ((wC1.Type == ZSTO_Logical) || (wC1.Type & ZSTO_Bool)

  if (Operand2.isNull()) {
    _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2 is NULL\n")
    return wResult;
  }

  _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2\n")

  ZOperandContent wC2 = Operand2.getContent(pURFParser);

  ZStatus wSt=evaluateTerm(wResult,wC1,wC2,CompareOperator);
  if (wSt!=ZS_SUCCESS)
    abort();

  if (NotOperator._isNot())
    wResult = !wResult;

  if (NextTerm!=nullptr) {
    bool wResult1 = NextTerm->evaluate(pURFParser);
    if (AndOrOperator._isAnd())
      wResult = wResult && wResult1;
    else if (AndOrOperator._isOr())
      wResult = wResult || wResult1;
    else
      _DBGPRINT("Expecting link operator either AND or OR. Found %s\n",decode_ZSOPV(AndOrOperator.Type))
  }
  else
    _DBGPRINT("ZSearchLogicalTerm::evaluate next term is NULL\n")

  return wResult;
} // ZSearchLogicalTerm::evaluate

utf8VaryingString
ZSearchLogicalTerm::_reportFormula(bool pHasParenthesis)
{
  utf8VaryingString wReturn;

  if (!NotOperator._isNothing()) {
    wReturn = NotOperator._reportFormula();
  }


  if (pHasParenthesis)
    wReturn += " ( ";
  else
    wReturn += " ";

  wReturn += Operand1._reportFormula();

  wReturn += " ";

  wReturn += CompareOperator._reportFormula();

  wReturn += " ";

  if (Operand2.isNull()) {
    wReturn.addsprintf("<NULL>");
    if (pHasParenthesis)
      wReturn += " ) ";
    else
      wReturn += " ";
    return wReturn;
  }

  wReturn += Operand2._reportFormula();

  wReturn += AndOrOperator._reportFormula();

  if (NextTerm) {
    if (ParenthesisLevel != NextTerm->ParenthesisLevel) {
      wReturn += NextTerm->_reportFormula(true);
    }
    else
      wReturn += NextTerm->_reportFormula();
  }
  if (pHasParenthesis)
    wReturn += " ) ";
  else
    wReturn += " ";
  return wReturn;
} // ZSearchLogicalTerm::_reportFormula

utf8VaryingString
ZSearchLogicalTerm::_report(int pLevel)
{
  utf8VaryingString wReturn;

  wReturn.addsprintf("%*cLeading Operator\n",pLevel,' ');
  wReturn += NotOperator._report(pLevel);

  wReturn.sprintf("%*c Logical term Parenthesis level %d collateral %d\n",pLevel,' ',ParenthesisLevel,Collateral);

  wReturn += Operand1._reportDetailed(pLevel);

  wReturn.addsprintf("%*cCompare operator :  ",pLevel,' ');
  wReturn += CompareOperator._report(pLevel);

  if (Operand2.isNull()) {
    wReturn.addsprintf("%*cNext logical operand is NULL\n",pLevel,' ');
    return wReturn;
  }

  wReturn += Operand2._reportDetailed(pLevel);

  wReturn.addsprintf("%*cCompare operator :  ",pLevel,' ');
  wReturn += AndOrOperator._report(pLevel);

  if (NextTerm)
    wReturn += NextTerm->_report(pLevel);

  return wReturn;
} // ZSearchLogicalTerm::_report
