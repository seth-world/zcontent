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

  _copyOperand(Operand1,pIn.Operand1);
  _copyOperand(Operand2,pIn.Operand2);

  if (pIn.NextTerm!=nullptr) {
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom copy NextTerm \n")
    NextTerm = new ZSearchLogicalTerm(*pIn.NextTerm);
  }
  else
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom NextTerm is NULL \n")
  return *this;
}

ZSearchLogicalTerm&
ZSearchLogicalTerm::_copyFrom(const ZSearchLogicalTerm* pIn)
{
  _DBGPRINT("ZSearchLogicalTerm::_copyFrom\n")
  NotOperator._copyFrom(pIn->NotOperator);
  CompareOperator._copyFrom(pIn->CompareOperator);
  AndOrOperator._copyFrom(pIn->AndOrOperator);

  _copyOperand(Operand1,pIn->Operand1);
  _copyOperand(Operand2,pIn->Operand2);

  if (pIn->NextTerm!=nullptr) {
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom copy NextTerm \n")
    NextTerm = new ZSearchLogicalTerm(pIn->NextTerm);
  }
  else
    _DBGPRINT("ZSearchLogicalTerm::_copyFrom NextTerm is NULL \n")
  return *this;
}


void
ZSearchLogicalTerm::setOperand1(const ZSearchLogicalOperand& pOperand)
{
  _copyOperand(Operand1,&pOperand);
}
void
ZSearchLogicalTerm::setOperand2(const ZSearchLogicalOperand& pOperand)
{
  _copyOperand(Operand2,&pOperand);
}

bool
ZSearchLogicalTerm::evaluate(  URFParser& pURFParser)
{
  bool wResult=true;
  _DBGPRINT("ZSearchLogicalTerm::evaluate operand 1\n")

  ZOperandContent wC1 = gettermOperandContent( Operand1,pURFParser);
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

    if (Operand2==nullptr) {
      _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2 is NULL\n")
      return wResult;
    }
  } // if ((wC1.Type == ZSTO_Logical) || (wC1.Type & ZSTO_Bool)

  if (Operand2==nullptr) {
    _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2 is NULL\n")
    return wResult;
  }

  _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2\n")

  ZOperandContent wC2 = gettermOperandContent( Operand2,pURFParser);

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

  wReturn += OperandReportFormula(Operand1);

  wReturn += " ";

  wReturn += CompareOperator._reportFormula();

  wReturn += " ";
/*
  if (Operand2==nullptr) {
    wReturn.addsprintf("<NULL>");
    if (pHasParenthesis)
      wReturn += " ) ";
    else
      wReturn += " ";
//    return wReturn;
  }
*/
  wReturn += OperandReportFormula(Operand2);
  if (pHasParenthesis)
    wReturn += " ) ";
  else
    wReturn += " ";

  wReturn += AndOrOperator._reportFormula();

  if (NextTerm!=nullptr) {
    if (ParenthesisLevel != NextTerm->ParenthesisLevel) {
      wReturn += "( ";
      wReturn += NextTerm->_reportFormula(true);
      wReturn += ") ";
    }
    else
      wReturn += NextTerm->_reportFormula();
  }
  else
    wReturn += " <no next term>";
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

  wReturn.sprintf("%*c Logical term Parenthesis level %d collateral %d\n",pLevel,' ',ParenthesisLevel);

  wReturn += OperandReportDetailed(Operand1,pLevel);

  wReturn.addsprintf("%*cCompare operator :  ",pLevel,' ');
  wReturn += CompareOperator._report(pLevel);

  if (Operand2==nullptr) {
    wReturn.addsprintf("%*cNext logical operand is NULL\n",pLevel,' ');
    return wReturn;
  }

  wReturn += OperandReportDetailed(Operand2,pLevel);

  wReturn.addsprintf("%*cCompare operator :  ",pLevel,' ');
  wReturn += AndOrOperator._report(pLevel);

  if (NextTerm)
    wReturn += NextTerm->_report(pLevel);

  return wReturn;
} // ZSearchLogicalTerm::_report
