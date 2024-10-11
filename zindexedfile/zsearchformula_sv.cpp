#include "zsearchformula.h"

void
ZSearchFormula::clear()
{
  if (NextFormula!=nullptr)
    delete NextFormula;
  NextFormula=nullptr;

  if (FormulaType==ZFORT_Term) {
    delete static_cast<ZSearchLogicalOperand*>(TermOrFormula);
  }
  else if (FormulaType==ZFORT_Formula) {
    delete static_cast<ZSearchFormula*>(TermOrFormula);
  }
  FormulaType = ZFORT_Nothing;
  return;
}

ZSearchFormula&
ZSearchFormula::_copyFrom(const ZSearchFormula& pIn) {
  LeadingOperator = pIn.LeadingOperator;

  if (pIn.FormulaType==ZFORT_Term) {
    TermOrFormula = new ZSearchLogicalOperand(*static_cast<ZSearchLogicalOperand*>(pIn.TermOrFormula));
  }
  else if (pIn.FormulaType==ZFORT_Formula) {
    TermOrFormula = new ZSearchFormula(*static_cast<ZSearchFormula*>(pIn.TermOrFormula));
  }

  ParenthesisLevel = pIn.ParenthesisLevel;
  FormulaType = pIn.FormulaType;
  if (pIn.NextFormula!=nullptr)
    NextFormula = new ZSearchFormula(*pIn.NextFormula);
  else
    NextFormula=nullptr;

  return *this;
}




utf8VaryingString
ZSearchFormula::_report(int pLevel)
{
  utf8VaryingString wReturn;
  wReturn.sprintf( "%*cFormula________________\n"
                   "%*cLevel %d \n"
                   "%*cType %s\n",
                   pLevel,' ',
                   pLevel,' ',ParenthesisLevel,
                   pLevel,' ',decode_ZFormulaType(FormulaType));
  while (true) {
    if (TermOrFormula==nullptr) {
      wReturn.addsprintf( "%*c--- Term is nullptr ----\n",pLevel,' ');
      break;
    }
    if (FormulaType==ZFORT_Nothing) {
      wReturn.addsprintf( "%*c--- Nothing ----\n",pLevel,' ');
      break;
    }
    if (FormulaType==ZFORT_Formula) {
      pLevel++;

      wReturn += static_cast<ZSearchFormula*>(TermOrFormula)->_report(pLevel);
      break;
    }
    if (FormulaType==ZFORT_Term) {
      wReturn += static_cast<ZSearchLogicalOperand*>(TermOrFormula)->_report(pLevel);
      break;
    }
  }// while true

  wReturn += LeadingOperator._report(pLevel);

  return wReturn;
}



namespace zbs {

const char* decode_ZFormulaType(ZFormulaType pType)
{
  switch (pType)
  {
  case ZFORT_Term:
    return "ZFORT_Term";
  case ZFORT_Formula:
    return "ZFORT_Formula";
  case ZFORT_Arithmetic:
    return "ZFORT_Arithmetic";
  case ZFORT_Nothing:
    return "ZFORT_Nothing";
  default:
    return "unknown ZFormulaType";
  }
}

}// namespace zbs
