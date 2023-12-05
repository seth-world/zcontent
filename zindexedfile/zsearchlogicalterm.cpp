#include "zsearchlogicalterm.h"

#include "zsearcharithmeticterm.h"

using namespace zbs;

ZSearchLogicalTerm::ZSearchLogicalTerm()
{

}

ZSearchLogicalTerm::~ZSearchLogicalTerm()
    {
        clearOperand(Operand1);
        clearOperand(Operand2);
        if (NextTerm!=nullptr)
            delete NextTerm;
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



ZStatus ZSearchLogicalTerm::evaluate(ZSearchEntityContext &pSEC, bool &pResult)
{
  _DBGPRINT("ZSearchLogicalTerm::evaluate operand 1\n")
/*
  if (pSEC.URFPS < URFPS_TotallyParsed) {
      abort();
  }
*/
  pResult = true;

  ZOperandContent wC1;
//  ZStatus wSt  = extractOperandContent( Operand1,pURFParser,&wC1);
  ZStatus wSt = extractOperandContent(pSEC, Operand1, &wC1);
  if (wSt!=ZS_SUCCESS)
      abort();
  if ((wC1.OperandZSTO == ZSTO_Logical) || (wC1.OperandZSTO & ZSTO_Bool)) {

    pResult = wC1.getBool();

    if (NotOperator._isNot())
      pResult = !pResult;

    if (!AndOrOperator._isAndOr()) {
      _DBGPRINT("getOneTerm Expected operator {AND,OR} while getting <%s>\n",
          decode_ZSOPV(AndOrOperator.ZSOPV))
      abort();
    }
    else
      _DBGPRINT("ZSearchLogicalTerm::evaluate operator is %s\n", decode_ZSOPV(AndOrOperator.ZSOPV))

    if (Operand2==nullptr) {
      _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2 is NULL\n")
      return ZS_NULLPTR;
    }
  } // if ((wC1.Type == ZSTO_Logical) || (wC1.Type & ZSTO_Bool)

  if (Operand2==nullptr) {
    _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2 is NULL\n")
    return ZS_NULLPTR;
  }

  _DBGPRINT("ZSearchLogicalTerm::evaluate operand 2\n")

  ZOperandContent wC2 ;
//  wSt = extractOperandContent( Operand2,pURFParser,&wC2);
  wSt = extractOperandContent(pSEC, Operand2, &wC2);
  if (wSt!=ZS_SUCCESS) {
      pResult=false;
      return wSt;
  }

  wSt=evaluateTerm(pResult,wC1,wC2,CompareOperator);
  if (wSt!=ZS_SUCCESS) {
      pResult=false;
      return wSt;
  }

  if (NotOperator._isNot())
    pResult = !pResult;

  if (NextTerm!=nullptr) {
      bool wResult1 ;
//      wSt = NextTerm->evaluate(pURFParser,wResult1);
      wSt = NextTerm->evaluate(pSEC, wResult1);
      if (AndOrOperator._isAnd())
          pResult = pResult && wResult1;
      else if (AndOrOperator._isOr())
          pResult = pResult || wResult1;
      else {
          _DBGPRINT("Expecting link operator either AND or OR. Found %s\n",
                    decode_ZSOPV(AndOrOperator.ZSOPV))
          return ZS_INVOP;
    }
  }
  else
    _DBGPRINT("ZSearchLogicalTerm::evaluate next term is NULL\n")

  return ZS_SUCCESS ;
} // ZSearchLogicalTerm::evaluate

utf8VaryingString
ZSearchLogicalTerm::_reportFormula(bool pHasParenthesis)
{
  utf8VaryingString wReturn;
    if (this==nullptr)
      return "<nullptr>";

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

  wReturn.sprintf("%*c Logical term Parenthesis level %d\n",pLevel,' ',ParenthesisLevel);

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

utf8VaryingString
ZSearchLogicalTerm::_reportDetailed(int pLevel)
{
    utf8VaryingString wReturn;

    wReturn.addsprintf("%*cLeading Operator\n",pLevel,' ');
    wReturn += NotOperator._report(pLevel);

    wReturn.sprintf("%*c Logical term Parenthesis level %d\n",pLevel,' ',ParenthesisLevel);

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
        wReturn += NextTerm->_reportDetailed(pLevel);

    return wReturn;
} // ZSearchLogicalTerm::_reportDetailed

namespace zbs {

class ZSearchArithmeticTerm;

void _countLogicalTerm(ZSearchLogicalTerm* pLogicalTerm,CountTerm &wCT)
{
    if (pLogicalTerm==nullptr) {
        wCT.Status = TSST_MissLogTerm;
        return ;
    }
    wCT.LogTerm ++;

    while (true) {
        if (pLogicalTerm->Operand1==nullptr) {
            wCT.Status = TSST_MissOperand;
            return ;
        }
        ZSearchOperandBase* wOpB = static_cast<ZSearchOperandBase*>(pLogicalTerm->Operand1);
        if ( wOpB->ZSTO == ZSTO_Arithmetic ) {
            ZSearchArithmeticTerm* wOpAr = static_cast<ZSearchArithmeticTerm*>(pLogicalTerm->Operand1);
            _countArithmeticTerm(wOpAr,wCT);
            break;
        }
        if ( wOpB->ZSTO == ZSTO_Logical ) {
            ZSearchLogicalTerm* wOpLo = static_cast<ZSearchLogicalTerm*>(pLogicalTerm->Operand1);
            _countLogicalTerm(wOpLo,wCT);
            break;
        }
        wCT.LogOperand ++ ;
        break;
    }// while true
    if (pLogicalTerm->CompareOperator._isValid())
        wCT.LogOperator ++;
    else {
        wCT.Status = TSST_MissOperator;
        return;
    }

    while (true) {
        if (pLogicalTerm->Operand2==nullptr) {
            wCT.Status = TSST_MissOperand;
            return ;
        }
        ZSearchOperandBase* wOpB = static_cast<ZSearchOperandBase*>(pLogicalTerm->Operand2);
        if ( wOpB->ZSTO == ZSTO_Arithmetic ) {
            ZSearchArithmeticTerm* wOpAr = static_cast<ZSearchArithmeticTerm*>(pLogicalTerm->Operand2);
            _countArithmeticTerm(wOpAr,wCT);
            break;
        }
        if ( wOpB->ZSTO == ZSTO_Logical ) {
            ZSearchLogicalTerm* wOpLo = static_cast<ZSearchLogicalTerm*>(pLogicalTerm->Operand2);
            _countLogicalTerm(wOpLo,wCT);
            break;
        }
        wCT.LogOperand ++ ;
        break;
    }// while true

    if (pLogicalTerm->NextTerm!=nullptr)
        _countLogicalTerm(pLogicalTerm->NextTerm,wCT);

    return ;
} // _countLogicalTerm

}// namespace zbs