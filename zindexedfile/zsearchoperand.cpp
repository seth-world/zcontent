#include "zsearchoperand.h"
#include "zsearchtoken.h"

#include <zcontent/zindexedfile/zmetadic.h>

#include "zsearchparser.h"


ZOperandContent::ZOperandContent()
{

}

ZOperandContent&
ZOperandContent::_copyFrom(const ZOperandContent& pIn) {
  Type = pIn.Type;
  String = pIn.String;
  URI = pIn.URI;
  Integer = pIn.Integer;
  Float = pIn.Float;
  CheckSum = pIn.CheckSum;
  Bool = pIn.Bool;
  Date = pIn.Date;
  Resource = pIn.Resource;
  return *this;
}



using namespace  zbs;
ZSearchOperator::~ZSearchOperator()
{
  while (TokenList.count())
    TokenList.pop();
}

ZSearchOperator&
ZSearchOperator::_copyFrom (const ZSearchOperator& pIn) {
  Type=pIn.Type;
  for (long wi=0; wi < pIn.TokenList.count();wi++)
    TokenList.push(pIn.TokenList[wi]);
  return *this;
}


void
ZSearchOperator::add(ZSearchToken* pTokenOperator) {
  TokenList.push(pTokenOperator);
  Type |= convert_ZSRCH_ZSOPV( pTokenOperator->Type );
}
void
ZSearchOperator::set(ZSearchToken* pTokenOperator) {
  TokenList.push(pTokenOperator);
  Type = convert_ZSRCH_ZSOPV( pTokenOperator->Type );
}
bool ZSearchOperator::isValid(ZSearchToken* pTokenOperator) {
  return (convert_ZSRCH_ZSOPV( pTokenOperator->Type) != ZSOPV_INVALID);
}
bool ZSearchOperator::isLogical(ZSearchToken *pTokenOperator) {
  return ((pTokenOperator->Type & ZSRCH_OPERATOR_LOGICAL)== ZSRCH_OPERATOR_LOGICAL );
}
bool ZSearchOperator::isStringComparator(ZSearchToken* pTokenOperator) {
  return ((pTokenOperator->Type & ZSRCH_OPERATOR_COMPARESTRING)== ZSRCH_OPERATOR_COMPARESTRING );
}

bool ZSearchOperator::isComparator(ZSearchToken* pTokenOperator) {
  return isLogical(pTokenOperator) || isStringComparator(pTokenOperator);
}
bool ZSearchOperator::isAndOr(ZSearchToken* pTokenOperator) {
  return (pTokenOperator->Type == ZSRCH_OPERATOR_AND ) || (pTokenOperator->Type == ZSRCH_OPERATOR_OR );
}
bool ZSearchOperator::isArithmeric(ZSearchToken* pTokenOperator) {
  return (pTokenOperator->Type & ZSRCH_OPERATOR_ARITHMETIC)== ZSRCH_OPERATOR_ARITHMETIC ;
}
bool ZSearchOperator::_isValid() {
  return Type != ZSOPV_INVALID;
}
bool ZSearchOperator::_isLogical() {
  return (Type & ZSOPV_LOGICAL_MASK)||(Type==ZSOPV_GREATEREQUAL);
}
bool ZSearchOperator::_isStringComparator() {
  return Type & ZSOPV_STRING_MASK;
}

bool ZSearchOperator::_isComparator() {
  switch(Type)
  {
  case ZSOPV_EQUAL:
  case ZSOPV_NOTEQUAL:
  case ZSOPV_LESS:
  case ZSOPV_GREATER:
  case ZSOPV_LESSEQUAL:
  case ZSOPV_GREATEREQUAL:
    return true;
  default:
    return false;
  }
}

bool ZSearchOperator::_isArithmeric() {
  //  return (Type & ~ZSOPV_GREATEREQUAL) & ZSOPV_ARITHMETIC_MASK;

  switch(Type)
  {
  case ZSOPV_MINUS:
  case ZSOPV_DIVIDE:
  case ZSOPV_MULTIPLY:
  case ZSOPV_POWER:
  case ZSOPV_MODULO:
  case ZSOPV_PLUS:
    return true;
  default:
    return false;
  }
}
bool ZSearchOperator::_isAndOr() {

  if ((Type == ZSOPV_AND) || (Type == ZSOPV_OR))
    return true;
  return false;
}
bool ZSearchOperator::_isNothing() {

  return  (Type == ZSOPV_Nothing) ;
}

utf8VaryingString
ZSearchOperator::_reportFormula()
{
  utf8VaryingString wReturn;


  switch (Type)
  {
  case ZSOPV_AND:
    wReturn="AND";
    break;
  case ZSOPV_OR:
    wReturn="OR";
    break;
  case ZSOPV_NOT:
    wReturn="NOT";
    break;

  case ZSOPV_EQUAL:
    wReturn="==";
    break;
  case ZSOPV_NOTEQUAL:
    wReturn="!=";
    break;

  case ZSOPV_LESS:
    wReturn="<";
    break;
  case ZSOPV_GREATER:
    wReturn=">";
    break;
  case ZSOPV_LESSEQUAL:
    wReturn="<=";
    break;
  case ZSOPV_GREATEREQUAL:
    wReturn=">=";
    break;
  case ZSOPV_CONTAINS:
    wReturn="CONTAINS";
    break;
  case ZSOPV_STARTS_WITH:
    wReturn="STARTS_WITH";
    break;
  case ZSOPV_ENDS_WITH:
    wReturn="ENDS_WITH";
    break;

  case ZSOPV_PLUS:
    wReturn="+";
    break;
  case ZSOPV_MINUS:
    wReturn="-";
    break;
  case ZSOPV_DIVIDE:
    wReturn="/";
    break;
  case ZSOPV_MULTIPLY:
    wReturn="*";
    break;
  case ZSOPV_POWER:
    wReturn="^";
    break;
  case ZSOPV_MODULO:
    wReturn="%";
    break;

  case ZSOPV_Nothing:
    break;

  default:
    wReturn = "unknown operator";
    break;
  }// switch

  return wReturn;
} //ZSearchOperator::_reportOp


utf8VaryingString
ZSearchOperator::_report(int pLevel)
{
  utf8VaryingString wReturn, wRet;
  if (Type & ZSOPV_AND) {
    wReturn="ZSOPV_AND";
  }
  if (Type & ZSOPV_OR) {
    wReturn.addConditionalOR("ZSOPV_OR");
  }
  if (Type & ZSOPV_NOT) {
    wReturn.addConditionalOR("ZSOPV_NOT");
  }


  if (Type & ZSOPV_EQUAL) {
    wReturn.addConditionalOR("ZSOPV_EQUAL");
  }
  if (Type & ZSOPV_NOTEQUAL) {
    wReturn.addConditionalOR("ZSOPV_NOTEQUAL");
  }
  if (Type & ZSOPV_LESS) {
    wReturn.addConditionalOR("ZSOPV_LESS");
  }
  if (Type & ZSOPV_GREATER) {
    wReturn.addConditionalOR("ZSOPV_GREATER");
  }
  if (Type & ZSOPV_LESSEQUAL) {
    wReturn.addConditionalOR("ZSOPV_LESSEQUAL");
  }
  if (Type & ZSOPV_GREATEREQUAL) {
    wReturn.addConditionalOR("ZSOPV_GREATEREQUAL");
  }

  if (Type & ZSOPV_CONTAINS) {
    wReturn.addConditionalOR("ZSOPV_CONTAINS");
  }
  if (Type & ZSOPV_STARTS_WITH) {
    wReturn.addConditionalOR("ZSOPV_STARTS_WITH");
  }
  if (Type & ZSOPV_ENDS_WITH) {
    wReturn.addConditionalOR("ZSOPV_ENDS_WITH");
  }



  if (Type & ZSOPV_PLUS) {
    wReturn.addConditionalOR("ZSOPV_PLUS");
  }
  if (Type & ZSOPV_MINUS) {
    wReturn.addConditionalOR("ZSOPV_MINUS");
  }
  if (Type & ZSOPV_DIVIDE) {
    wReturn.addConditionalOR("ZSOPV_DIVIDE");
  }
  if (Type & ZSOPV_MULTIPLY) {
    wReturn.addConditionalOR("ZSOPV_MULTIPLY");
  }
  if (Type & ZSOPV_POWER) {
    wReturn.addConditionalOR("ZSOPV_POWER");
  }
  if (Type & ZSOPV_MODULO) {
    wReturn.addConditionalOR("ZSOPV_MODULO");
  }

  if (wReturn.isEmpty()) {
    wReturn = "unknown operator";
  }

  wRet.sprintf ("%*c%s\n",pLevel,' ', wReturn.toString());
  return wRet;
} //ZSearchOperator::_report




ZSearchLogicalOperand::~ZSearchLogicalOperand()
{
  clearOperand(Operand);
}



void
ZSearchLogicalOperand::copyOperand(void*& pOperand,const void* pOpIn)
{
  if (pOperand!=nullptr)
    clearOperand(pOperand);
  copyOperand(pOperand,pOpIn);
}

ZSearchLogicalOperand&
ZSearchLogicalOperand::_copyFrom(const ZSearchLogicalOperand& pIn)
{
/*  ParenthesisLevel = pIn.ParenthesisLevel;
  Collateral = pIn.Collateral;

  Operator = pIn.Operator;
  Operator = pIn.Operator;
*/
  copyOperand(Operand,pIn.Operand);
//  copyOperand(OperandNext,pIn.OperandNext);

  return *this;
} // _copyFrom

utf8VaryingString
ZSearchLogicalOperand::_evaluateOpLiteral(void *pOp)
{
  utf8VaryingString wReturn;

  if (!(static_cast<ZSearchOperandBase*>(pOp)->Type & ZSTO_Literal)) {
    wReturn.sprintf("_evaluateOpLiteral-E-INVTYP Invalid type to evaluate <%s>",
        decode_OperandType(static_cast<ZSearchOperandBase*>(pOp)->Type));
    return wReturn;
  }
  switch (static_cast<const ZSearchOperandBase*>(pOp)->Type )
  {
  case ZSTO_LiteralString:
  {
    ZSearchLiteral<utf8VaryingString>* wOp=static_cast<ZSearchLiteral<utf8VaryingString>*>(pOp);
    wReturn.sprintf("\"%s\"",wOp->Content.toString());
//    return wOp->Content;
    return wReturn;
  }
  case ZSTO_LiteralUriString:
  {
    ZSearchLiteral<uriString>* wOp=static_cast<ZSearchLiteral<uriString>*>(pOp);
    return wOp->Content;
  }
  case ZSTO_LiteralInteger:
  {
    ZSearchLiteral<long>* wOp=static_cast<ZSearchLiteral<long>*>(pOp);
    wReturn.sprintf("%ld",wOp->Content);
    return wReturn;
  }
  case ZSTO_LiteralFloat:
  {
    ZSearchLiteral<double>* wOp=static_cast<ZSearchLiteral<double>*>(pOp);
    wReturn.sprintf("%g",wOp->Content);
    return wReturn;
  }
  case ZSTO_LiteralDate:
  {
    ZSearchLiteral<ZDateFull>* wOp=static_cast<ZSearchLiteral<ZDateFull>*>(pOp);
    return wOp->Content.toLocale();
  }
  case ZSTO_LiteralResource:
  {
    ZSearchLiteral<ZResource>* wOp=static_cast<ZSearchLiteral<ZResource>*>(pOp);
//    return wOp->Content.toHexa();
    long wi=0;
    for (;wi < GParser->ZEntityList.count();wi++) {
      if (GParser->ZEntityList[wi].Value==wOp->Content.Entity) {
        break;
      }
    }// for
    if (wi==GParser->ZEntityList.count())
      wReturn.sprintf("ZResource(%6X,%6X)",wOp->Content.Entity,wOp->Content.id);
    else
      wReturn.sprintf("ZResource(%s,%6X)",GParser->ZEntityList[wi].Symbol.toString(),wOp->Content.id);
    return wReturn;
  }
  case ZSTO_LiteralBool:
  {
    ZSearchLiteral<bool>* wOp=static_cast<ZSearchLiteral<bool>*>(pOp);
    return wOp->Content?"TRUE":"FALSE";
  }
  case ZSTO_LiteralChecksum:
  {
    ZSearchLiteral<checkSum>* wOp=static_cast<ZSearchLiteral<checkSum>*>(pOp);
//    return wOp->Content.toHexa();
    wReturn.sprintf("checkSum(%s)",wOp->Content.toHexa().toString());
    return wReturn;
  }
  default:
  {
    wReturn.sprintf("_evaluateOpLiteral-E-INVTYP Invalid type to evaluate <%s>",
        decode_OperandType(static_cast<ZSearchOperandBase*>(pOp)->Type));
    return wReturn;
  }

  }// switch
}




ZOperandContent getOperandContent(void* pOp,URFParser& pURFParser)
{
  ZOperandContent wOperandContent;
  ZSearchOperandType wType =  static_cast<ZSearchOperandBase*> (pOp)->Type;

  if (wType==ZSTO_Logical) {
    /* Operand is a logical expression.
       * It returns a boolean
       * Operator is a logical link {AND,OR} to OperandNext */
    wOperandContent.Type = ZSTO_Logical; /* This is a logical expression - type is bool */
    wOperandContent.Bool = static_cast<ZSearchLogicalOperand*> (pOp)->evaluate(pURFParser);
    return wOperandContent;
  }
  /* Operand is an arithmetic expression.
   * result of an arithmetic expression is a qualified literal and must be processed as a literal constant
   */
  if (wType & ZSTO_Arithmetic) {
    return  static_cast<ZSearchArithmeticOperand*> (pOp)->compute(pURFParser);
  }
  /* operand is not an expression
   * It is either a field operand or a literal operand
   *  get Next operand and evaluate expression
  */

  return gettermOperandContent(pOp,pURFParser);
}

ZOperandContent
ZSearchLogicalOperand::getContent(URFParser& pURFParser) {
  return getOperandContent(Operand,pURFParser);
}



ZSearchLogicalTerm&
ZSearchLogicalTerm::_copyFrom(const ZSearchLogicalTerm& pIn)
{
  NotOperator._copyFrom(pIn.NotOperator);
  CompareOperator._copyFrom(pIn.CompareOperator);
  AndOrOperator._copyFrom(pIn.AndOrOperator);

  Operand1.clear();
  if (!pIn.Operand1.isNull())
  {
    Operand1._copyFrom(pIn.Operand1);
  }

  Operand2.clear();
  if (!pIn.Operand2.isNull())
  {
    Operand2._copyFrom(pIn.Operand2);
  }

  if (NextTerm!=nullptr)
    NextTerm = new ZSearchLogicalTerm(*pIn.NextTerm);

  return *this;
}

bool
ZSearchLogicalTerm::evaluate(  URFParser& pURFParser)
{
  bool wResult=true;
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

    if (Operand2.isNull()) {
      return wResult;
    }
  }
  if (Operand2.isNull())
    return wResult;

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

  return wResult;
} // ZSearchLogicalTerm::evaluate


bool
ZSearchLogicalOperand::evaluate(URFParser& pURFParser)
{

  ZOperandContent wOperandContent = getOperandContent(Operand,pURFParser);




} //ZSearchLogicalOperand::evaluate


utf8VaryingString
ZSearchLogicalOperand::_reportDetailed(int pLevel)
{
  utf8VaryingString wReturn;

  wReturn.addsprintf("%*cOperand\n",pLevel,' ');
  while (true) {
    if (static_cast<ZSearchOperandBase*>(Operand)->Type == ZSTO_Arithmetic) {
      wReturn += static_cast<ZSearchArithmeticOperand*>(Operand)->_report(pLevel+1);
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->Type == ZSTO_Logical) {
      wReturn += static_cast<ZSearchLogicalTerm*>(Operand)->_report(pLevel+1);
      break;
    }

    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Field) {
      ZSearchFieldOperandOwnData* wFldOwnData = static_cast<ZSearchFieldOperandOwnData*>( Operand);
      wReturn.addsprintf("%*c type %s full field name <%s> dictionary type <%s>\n",
          pLevel,' ',
          decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type),
          wFldOwnData->FullFieldName.toString(),
          decode_ZType(wFldOwnData->FieldDescription.ZType)
          );
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Literal) {
      wReturn.addsprintf("%*c type %s value <%s>\n",
          pLevel,' ',
          decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type),
          _evaluateOpLiteral(Operand).toString()
          );
      break;
    }
    wReturn.addsprintf("%*c type %s <Invalid type>\n",
        pLevel,' ',
        decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type)
        );
    break;
  }// while true

  if (static_cast<ZSearchOperandBase*>(Operand)->TokenList.count()>0) {
    wReturn.addsprintf("%*c Tokens <",pLevel,' ');
    for (int wj=0; wj < static_cast<ZSearchOperandBase*>(Operand)->TokenList.count();wj++) {
      wReturn.addsprintf("%s",static_cast<ZSearchOperandBase*>(Operand)->TokenList[wj]->Text.toString());
    }
    wReturn += ">\n";
  } else {
    wReturn.addsprintf("%*c No Token for this operand\n",pLevel,' ');
  }

  //    setMaxSprintfBufferCount(4096);

  if (static_cast<ZSearchOperandBase*>(Operand)->ModifierType!=ZSRCH_NOTHING){
    wReturn.addsprintf("%*c modifier %s\n"
                       "%*c     value 1 %ld\n"
                       "%*c     value 2 %ld\n",
        pLevel,' ',decode_SearchTokenType(static_cast<ZSearchOperandBase*>(Operand)->ModifierType).toString(),
        pLevel,' ',static_cast<ZSearchOperandBase*>(Operand)->ModVal1,
        pLevel,' ',static_cast<ZSearchOperandBase*>(Operand)->ModVal2 );
  }

  return wReturn;
} // ZSearchLogicalOperand::_report

utf8VaryingString
ZSearchLogicalOperand::_reportFormula()
{
  utf8VaryingString wReturn;

  while (true) {
    if (static_cast<ZSearchOperandBase*>(Operand)->Type == ZSTO_Arithmetic) {
      wReturn += static_cast<ZSearchArithmeticOperand*>(Operand)->_reportFormula();
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->Type == ZSTO_Logical) {
      wReturn += static_cast<ZSearchLogicalTerm*>(Operand)->_reportFormula(true);
      break;
    }

    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Field) {
      ZSearchFieldOperandOwnData* wFldOwnData = static_cast<ZSearchFieldOperandOwnData*>( Operand);

      wReturn.addsprintf(" %s ", wFldOwnData->FullFieldName.toString() );
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Literal) {
      wReturn.addsprintf(" %s",_evaluateOpLiteral(Operand).toString() );
      break;
    }
    wReturn.addsprintf("type %s <Invalid type>\n",
        decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type)
        );
    break;
  }// while true

  if (static_cast<ZSearchOperandBase*>(Operand)->ModifierType!=ZSRCH_NOTHING){
    wReturn.addsprintf(".%s(%ld,%ld)",decode_SearchTokenType(static_cast<ZSearchOperandBase*>(Operand)->ModifierType).toString(),
        static_cast<ZSearchOperandBase*>(Operand)->ModVal1,
        static_cast<ZSearchOperandBase*>(Operand)->ModVal2 );
  }

  wReturn += " ";

  return wReturn;
} // ZSearchLogicalOperand::_report


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
utf8VaryingString
ZSearchArithmeticOperand::_report(int pLevel)
{
  utf8VaryingString wReturn;

  wReturn.sprintf("%*cArithmetic operand Parenthesis level %d collateral %d\n",pLevel,' ',ParenthesisLevel,Collateral);

  wReturn.addsprintf("%*cOperand\n",pLevel,' ');
  while (true) {
    if (static_cast<ZSearchOperandBase*>(Operand)->Type == ZSTO_Arithmetic) {
      wReturn += static_cast<ZSearchArithmeticOperand*>(Operand)->_report(pLevel+1);
      break;
    }

    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Field) {
      ZSearchFieldOperandOwnData* wFldOwnData = static_cast<ZSearchFieldOperandOwnData*>( Operand);
      wReturn.addsprintf("%*c type %s full field name <%s> dictionary type <%s>\n",
          pLevel,' ',
          decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type),
          wFldOwnData->FullFieldName.toString(),
          decode_ZType(wFldOwnData->FieldDescription.ZType)
          );
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Literal) {
      wReturn.addsprintf("%*c type %s value <%s>\n",
          pLevel,' ',
          decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type),
          ZSearchLogicalOperand::_evaluateOpLiteral(Operand).toString()
          );
      break;
    }
    wReturn.addsprintf("%*c type %s <Invalid type>\n",
        pLevel,' ',
        decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type)
        );
    break;

    if (static_cast<ZSearchOperandBase*>(Operand)->TokenList.count()>0) {
      wReturn.addsprintf("%*c Tokens <",pLevel,' ');
      for (int wj=0; wj < static_cast<ZSearchOperandBase*>(Operand)->TokenList.count();wj++) {
        wReturn.addsprintf("%s",static_cast<ZSearchOperandBase*>(Operand)->TokenList[wj]->Text.toString());
      }
      wReturn += ">\n";
    } else {
      wReturn.addsprintf("%*c No Token for this operand\n",pLevel,' ');
    }

    //    setMaxSprintfBufferCount(4096);

    if (static_cast<ZSearchOperandBase*>(Operand)->ModifierType!=ZSRCH_NOTHING){
      wReturn.addsprintf("%*c modifier %s\n"
                         "%*c     value 1 %ld\n"
                         "%*c     value 2 %ld\n",
          pLevel,' ',decode_SearchTokenType(static_cast<ZSearchOperandBase*>(Operand)->ModifierType).toString(),
          pLevel,' ',static_cast<ZSearchOperandBase*>(Operand)->ModVal1,
          pLevel,' ',static_cast<ZSearchOperandBase*>(Operand)->ModVal2 );
    }
  }// while true

  wReturn.addsprintf("%*cOperator\n",pLevel,' ');
  wReturn += Operator._report(pLevel+1);

  if (OperandNext==nullptr) {
    wReturn.addsprintf("%*cNext arithmetic operand is <nullptr>\n",pLevel,' ');
    return wReturn;
  }

  wReturn += static_cast<ZSearchArithmeticOperand*>(OperandNext)->_report(pLevel);
  return wReturn;
} // ZSearchArithmeticOperand::_report

utf8VaryingString
ZSearchArithmeticOperand::_reportFormula()
{
  utf8VaryingString wReturn;

  while (true) {
    if (static_cast<ZSearchOperandBase*>(Operand)->Type == ZSTO_Arithmetic) {
      wReturn += " ( ";
      wReturn = static_cast<ZSearchArithmeticOperand*>(Operand)->_reportFormula();
      wReturn += " ) ";
      break;
    }

    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Field) {
      ZSearchFieldOperandOwnData* wFldOwnData = static_cast<ZSearchFieldOperandOwnData*>( Operand);
      wReturn.addsprintf(" %s", wFldOwnData->FullFieldName.toString() );
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Literal) {
      wReturn.addsprintf(" %s", ZSearchLogicalOperand::_evaluateOpLiteral(Operand).toString() );
      break;
    }

    wReturn.addsprintf("%s <Invalid type> ", decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type) );
    break;
  }// while true

  if (static_cast<ZSearchOperandBase*>(Operand)->ModifierType!=ZSRCH_NOTHING){
    wReturn.addsprintf(".%s(%ld,%ld) ",
        decode_SearchTokenType(static_cast<ZSearchOperandBase*>(Operand)->ModifierType).toString(),
        static_cast<ZSearchOperandBase*>(Operand)->ModVal1,
        static_cast<ZSearchOperandBase*>(Operand)->ModVal2 );
  }
  else
    wReturn += " ";

  wReturn += Operator._reportFormula();



  if (OperandNext==nullptr) {
    wReturn += "<nullptr>";
    return wReturn;
  }

  wReturn += static_cast<ZSearchArithmeticOperand*>(OperandNext)->_reportFormula();
  return wReturn;
} // ZSearchArithmeticOperand::_report



ZSearchArithmeticOperand::~ZSearchArithmeticOperand()
{

  clearOperand(OperandNext);
  clearOperand(Operand);
}

void
ZSearchOperandBase::clearOperand(void *&pOp)
{
  if (pOp==nullptr)
    return;
  if (static_cast<ZSearchOperandBase*>(pOp)->Type & ZSTO_Arithmetic)
  {
    delete static_cast<ZSearchArithmeticOperand*>(pOp);
    pOp=nullptr;
    return;
  }

  switch (static_cast<ZSearchOperandBase*>(pOp)->Type)
  {
  case ZSTO_Nothing:
    break;
  case ZSTO_FieldString:
    delete static_cast<ZSearchFieldOperand<utf8VaryingString>*>(pOp);
    break;
  case ZSTO_FieldInteger:
    delete static_cast<ZSearchFieldOperand<long>*>(pOp);
    break;
  case ZSTO_FieldFloat:
    delete static_cast<ZSearchFieldOperand<double>*>(pOp);
    break;
  case ZSTO_FieldDate:
    delete static_cast<ZSearchFieldOperand<ZDateFull>*>(pOp);
    break;
  case ZSTO_FieldChecksum:
    delete static_cast<ZSearchFieldOperand<checkSum>*>(pOp);
    break;
  case ZSTO_FieldResource:
    delete static_cast<ZSearchFieldOperand<ZResource>*>(pOp);
    break;
  case ZSTO_FieldBool:
    delete static_cast<ZSearchFieldOperand<bool>*>(pOp);
    break;

  case ZSTO_LiteralString:
    delete static_cast<ZSearchLiteral<utf8VaryingString>*>(pOp);
    break;
  case ZSTO_LiteralDate:
    delete static_cast<ZSearchLiteral<ZDateFull>*>(pOp);
    break;
  case ZSTO_LiteralChecksum:
    delete static_cast<ZSearchLiteral<checkSum>*>(pOp);
    break;
  case ZSTO_LiteralInteger:
    delete static_cast<ZSearchLiteral<long>*>(pOp);
    break;
  case ZSTO_LiteralFloat:
    delete static_cast<ZSearchLiteral<double>*>(pOp);
    break;
  case ZSTO_LiteralResource:
    delete static_cast<ZSearchLiteral<ZResource>*>(pOp);
    break;
  case ZSTO_LiteralBool:
    delete static_cast<ZSearchLiteral<bool>*>(pOp);
    break;
  }//switch
  pOp=nullptr;
  return;
}

ZOperandContent
ZSearchArithmeticOperand::compute(URFParser& pURFParser)
{

}


void
ZSearchArithmeticOperand::copyOperand(void*& pOperand,const void* pOpIn)
{
  if (pOperand!=nullptr)
    clearOperand(pOperand);
  _copyOperand(pOperand,pOpIn);
}

void
ZSearchArithmeticOperand::_copyOperand(void*& pOperand,const void* pOpIn)
{
  if (static_cast<const ZSearchOperandBase*>(pOpIn)->Type & ZSTO_Arithmetic)
  {
    pOperand = new ZSearchArithmeticOperand(*static_cast<const ZSearchArithmeticOperand*>(pOpIn));
    return;
  }
  switch (static_cast<const ZSearchOperandBase*>(pOpIn)->Type)
  {
  case ZSTO_Nothing:
    break;
  case ZSTO_FieldString:
    pOperand = new ZSearchFieldOperand<utf8VaryingString>(*static_cast<const ZSearchFieldOperand<utf8VaryingString>*>(pOpIn));
    break;
  case ZSTO_FieldInteger:
    pOperand = new ZSearchFieldOperand<long>(*static_cast<const ZSearchFieldOperand<long>*>(pOpIn));
    break;
  case ZSTO_FieldFloat:
    pOperand = new ZSearchFieldOperand<double>(*static_cast<const ZSearchFieldOperand<double>*>(pOpIn));
    break;
  case ZSTO_FieldDate:
    pOperand = new ZSearchFieldOperand<ZDateFull>(*static_cast<const ZSearchFieldOperand<ZDateFull>*>(pOpIn));
    break;
  case ZSTO_FieldChecksum:
    pOperand = new ZSearchFieldOperand<checkSum>(*static_cast<const ZSearchFieldOperand<checkSum>*>(pOpIn));
    break;
  case ZSTO_FieldResource:
    pOperand = new ZSearchFieldOperand<ZResource>(*static_cast<const ZSearchFieldOperand<ZResource>*>(pOpIn));
    break;
  case ZSTO_FieldBool:
    pOperand = new ZSearchFieldOperand<bool>(*static_cast<const ZSearchFieldOperand<bool>*>(pOpIn));
    break;

  case ZSTO_LiteralString:
    pOperand = new ZSearchLiteral<utf8VaryingString>(*static_cast<const ZSearchLiteral<utf8VaryingString>*>(pOpIn));
    break;
  case ZSTO_LiteralDate:
    pOperand = new ZSearchLiteral<ZDateFull>(*static_cast<const ZSearchLiteral<ZDateFull>*>(pOpIn));
    break;
  case ZSTO_LiteralChecksum:
    pOperand = new ZSearchLiteral<checkSum>(*static_cast<const ZSearchLiteral<checkSum>*>(pOpIn));
    break;
  case ZSTO_LiteralInteger:
    pOperand = new ZSearchLiteral<long>(*static_cast<const ZSearchLiteral<long>*>(pOpIn));
    break;
  case ZSTO_LiteralFloat:
    pOperand = new ZSearchLiteral<double>(*static_cast<const ZSearchLiteral<double>*>(pOpIn));
    break;
  case ZSTO_LiteralResource:
    pOperand = new ZSearchLiteral<ZResource>(*static_cast<const ZSearchLiteral<ZResource>*>(pOpIn));
    break;
  case ZSTO_LiteralBool:
    pOperand = new ZSearchLiteral<bool>(*static_cast<const ZSearchLiteral<bool>*>(pOpIn));
    break;
  }

  return;
}// ZSearchArithmeticOperand::_copyOperand


ZSearchArithmeticOperand&
ZSearchArithmeticOperand::_copyFrom(const ZSearchArithmeticOperand& pIn)
{
  ZSearchOperandBase::_copyFrom((ZSearchOperandBase)pIn);
  ParenthesisLevel = pIn.ParenthesisLevel;
  copyOperand(Operand,pIn.Operand);
  copyOperand(OperandNext,pIn.OperandNext);
  Operator._copyFrom(pIn.Operator);
  return *this;
}


/*----------------------Evaluate operands' content routines -------------------*/


utf8VaryingString
displayOperandContent(ZOperandContent& pContent) {
  utf8VaryingString wReturn;
  switch (pContent.Type)
  {
  case ZSTO_Integer:
    wReturn.sprintf("%ld",pContent.Integer);
    break;
  case ZSTO_Float:
    wReturn.sprintf("%g",pContent.Float);
    break;
  case ZSTO_String:
  case ZSTO_UriString:
    wReturn=pContent.String;
    break;
  case ZSTO_FieldBool:
    wReturn = pContent.Bool?"TRUE":"FALSE";
    break;
  case ZSTO_Date:
    wReturn=pContent.Date.toFormatted();
    break;
  case ZSTO_Resource:
    wReturn=pContent.Resource.toHexa();
    break;
  case ZSTO_Checksum:
    wReturn=pContent.CheckSum.toHexa();
    break;
  }
  return wReturn;
}



ZOperandContent getFieldOperandContent (URFField& wField)
{
  ZOperandContent pOpContent;

  switch (wField.ZType) {
  case ZType_Char: {
    char wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<char>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_UChar: {
    unsigned char wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<unsigned char>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_S8: {
    int8_t wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<int8_t>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_U8: {
    uint8_t wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<uint8_t>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_S16: {
    int16_t wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<int16_t>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_U16: {
    uint16_t wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<uint16_t>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_S32: {
    int32_t wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<int32_t>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_U32: {
    uint32_t wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<uint32_t>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_S64: {
    int64_t wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<int64_t>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_U64: {
    uint64_t wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<uint64_t>(wVal,wField.Ptr);
    pOpContent.Integer = long(wVal);
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZType_Float: {
    float wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<float>(wVal,wField.Ptr);
    pOpContent.Float = double(wVal);
    pOpContent.Type = ZSTO_Float;
    break;
  }
  case ZType_Double: {
    double wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<double>(wVal,wField.Ptr);
    pOpContent.Float = double(wVal);
    pOpContent.Type = ZSTO_Float;
    break;
  }
  case ZType_LDouble: {
    long double wVal;
    wField.Ptr += sizeof(ZTypeBase);
    _importAtomic<long double>(wVal,wField.Ptr);
    pOpContent.Float = double(wVal);
    pOpContent.Type = ZSTO_Float;
    break;
  }
  case ZType_Bool: {
    bool wVal;
    _importAtomic<bool>(wVal,wField.Ptr);
    pOpContent.Bool = bool(wVal);
    pOpContent.Type = ZSTO_Bool;
    break;
  }


  case ZType_Utf8VaryingString: {
    utf8VaryingString wVal;
    wVal._importURF(wField.Ptr);
    pOpContent.String = wVal;
    pOpContent.Type = ZSTO_String;
    break;
  }
  case ZType_Utf16VaryingString: {
    utf16VaryingString wVal;
    wVal._importURF(wField.Ptr);
    pOpContent.String.fromUtf16( wVal);
    pOpContent.Type = ZSTO_String;
    break;
  }
  case ZType_Utf32VaryingString: {
    utf32VaryingString wVal;
    wVal._importURF(wField.Ptr);
    pOpContent.String.fromUtf32( wVal);
    pOpContent.Type = ZSTO_String;
    break;
  }

  case ZType_ZDateFull: {
    pOpContent.Date._importURF(wField.Ptr);
    pOpContent.Type = ZSTO_Date;
    break;
  }

  case ZType_Resource: {
    pOpContent.Resource._importURF(wField.Ptr);
    pOpContent.Type = ZSTO_Resource;

    break;
  }
  case ZType_CheckSum: {
    pOpContent.CheckSum._importURF(wField.Ptr);
    pOpContent.Type = ZSTO_Checksum;
    break;
  }
  case ZType_URIString: {
    pOpContent.URI._importURF(wField.Ptr);
    pOpContent.Type = ZSTO_UriString;
    break;
  }
  default:
    _DBGPRINT("Cannot convert data type %s - data type not supported.",decode_ZType(wField.ZType))
    abort();
  }//switch


  _DBGPRINT("getFieldOperandContent field type <%s> operand type<%s> value <%s>\n",
      decode_ZType(wField.ZType),decode_OperandType(pOpContent.Type),displayOperandContent(pOpContent).toString())

  return pOpContent;
} // getFieldOperandContent





ZOperandContent getLiteralOperandContent (void* pLiteral)
{
  ZOperandContent pOpContent;

  ZSearchOperandBase* wOpBase  = static_cast<ZSearchOperandBase*>(pLiteral);

  if (!(wOpBase->Type & ZSTO_Literal) )
    return ZOperandContent();

  int wOpType = wOpBase->Type & ~ZSTO_Literal;
  pOpContent.Type = ZSearchOperandType(wOpType);
  switch (pOpContent.Type) {
  case ZSTO_String: {
    pOpContent.String = static_cast<ZSearchLiteral<utf8VaryingString>*>(pLiteral)->Content;
    pOpContent.Type = ZSTO_String;
    break;
  }
  case ZSTO_UriString: {
    pOpContent.URI = static_cast<ZSearchLiteral<uriString>*>(pLiteral)->Content;
    pOpContent.Type = ZSTO_UriString;
    break;
  }
  case ZSTO_Integer: {
    pOpContent.Integer = static_cast<ZSearchLiteral<long>*>(pLiteral)->Content;
    pOpContent.Type = ZSTO_Integer;
    break;
  }
  case ZSTO_Float: {
    pOpContent.Float = static_cast<ZSearchLiteral<double>*>(pLiteral)->Content;
    pOpContent.Type = ZSTO_Float;
    break;
  }
  case ZSTO_Date: {
    pOpContent.Date = static_cast<ZSearchLiteral<ZDateFull>*>(pLiteral)->Content;
    pOpContent.Type = ZSTO_Date;
    break;
  }
  case ZSTO_Resource: {
    pOpContent.Resource = static_cast<ZSearchLiteral<ZResource>*>(pLiteral)->Content;
    pOpContent.Type = ZSTO_Resource;
    break;
  }
  case ZSTO_Checksum: {
    pOpContent.CheckSum = static_cast<ZSearchLiteral<checkSum>*>(pLiteral)->Content;
    pOpContent.Type = ZSTO_Checksum;
    break;
  }
  case ZSTO_Bool: {
    pOpContent.Bool = static_cast<ZSearchLiteral<bool>*>(pLiteral)->Content;
    pOpContent.Type = ZSTO_Bool;
    break;
  }

  }//switch

  _DBGPRINT("getLiteralOperandContent literal operand type<%s> value <%s>\n",
      decode_OperandType(pOpContent.Type),displayOperandContent(pOpContent).toString())

  return pOpContent;
} // getLiteralOperandContent

ZOperandContent gettermOperandContent (void* pOperand,URFParser& pURFParser)
{
//  URFParser wURFParser(pRecord);
  URFField wField;
  ZOperandContent wContent ;
  ZSearchOperandBase* wOpBase=static_cast<ZSearchOperandBase*>(pOperand);
  if (wOpBase->Type & ZSTO_Field) {
    ZSearchFieldOperandOwnData* wOpFieldOwnData=static_cast<ZSearchFieldOperandOwnData*>(pOperand);

    wField  = pURFParser.getURFFieldByRankIncremental(wOpFieldOwnData->MDicRank);
    if (!wField.Present)
      return ZOperandContent();

    wContent = getFieldOperandContent (wField);
  } // ZSTO_Field
  else {
    wContent = getLiteralOperandContent( pOperand);
  } // ZSTO_Literal

  if (wOpBase->ModifierType==ZSRCH_NOTHING)
    return wContent;


  switch(wOpBase->ModifierType) {
    /* uristring modifiers */
  case ZSRCH_PATH:
    wContent.String = wContent.URI.getDirectoryPath();
    wContent.Type = ZSTO_String;
    break;
  case ZSRCH_EXTENSION:
    wContent.String = wContent.URI.getFileExtension();
    wContent.Type = ZSTO_String;
    break;
  case ZSRCH_BASENAME:
    wContent.String = wContent.URI.getBasename();
    wContent.Type = ZSTO_String;
    break;
  case ZSRCH_ROOTNAME:
    wContent.String = wContent.URI.getRootname();
    wContent.Type = ZSTO_String;
    break;

    /* Date modifier */
  case ZSRCH_YEAR:
    wContent.Integer = wContent.Date.year();
    wContent.Type = ZSTO_Integer;
    break;
  case ZSRCH_MONTH:
    wContent.Integer = wContent.Date.month();
    wContent.Type = ZSTO_Integer;
    break;
  case ZSRCH_DAY:
    wContent.Integer = wContent.Date.day();
    wContent.Type = ZSTO_Integer;
    break;
  case ZSRCH_HOUR:
    wContent.Integer = wContent.Date.hour();
    wContent.Type = ZSTO_Integer;
    break;
  case ZSRCH_MIN:
    wContent.Integer = wContent.Date.min();
    wContent.Type = ZSTO_Integer;
    break;
  case ZSRCH_SEC:
    wContent.Integer = wContent.Date.sec();
    wContent.Type = ZSTO_Integer;
    break;

    /* resource modifier */
  case ZSRCH_ZENTITY:
    wContent.Integer = wContent.Resource.Entity;
    wContent.Type = ZSTO_Integer;
    break;
  case ZSRCH_ID:
    wContent.Integer = wContent.Resource.id;
    wContent.Type = ZSTO_Integer;
    break;
  }

  return wContent;
}

ZStatus
evaluateTermString(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZSearchOperandType_type wType = pOp2.Type & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
    break;
  case ZSTO_UriString:
    pOp2.String = pOp2.URI;
    break;
  case ZSTO_Bool:
    pOp2.String = pOp2.Bool?"TRUE":"FALSE";
    break;
  case ZSTO_Integer:
    pOp2.String.sprintf("%ld",pOp2.Integer);
    break;
  case ZSTO_Float:
    pOp2.String.sprintf("%g",pOp2.Float);
    break;
  case ZSTO_Date:
    pOp2.String = pOp2.Date.toUTCGMT();
    break;
  case ZSTO_Checksum:
    pOp2.String = pOp2.CheckSum.toHexa();
    break;
  case ZSTO_Resource:
    pOp2.String = pOp2.Resource.toHexa();
    break;
  } // switch

  switch (pOperator.Type)
  {
  case ZSOPV_EQUAL:
    pOutResult = pOp1.String.compare(pOp2.String)==0;
    break;
  case ZSOPV_NOTEQUAL:
    pOutResult = pOp1.String.compare(pOp2.String)!=0;
    break;
  case ZSOPV_LESS:
    pOutResult = pOp1.String < pOp2.String;
    break;
  case ZSOPV_GREATER:
    pOutResult = pOp1.String > pOp2.String;
    break;
  case ZSOPV_LESSEQUAL:
    pOutResult = pOp1.String.compare(pOp2.String)<=0;
    break;
  case ZSOPV_GREATEREQUAL:
    pOutResult = pOp1.String.compare(pOp2.String)>=0;
    break;

  case ZSOPV_CONTAINS:
    pOutResult = pOp1.String.contains(pOp2.String.toString());
    break;
  case ZSOPV_STARTS_WITH:
    pOutResult = pOp1.String.startsWith(pOp2.String.toString());
    break;
  case ZSOPV_ENDS_WITH:
    pOutResult = pOp1.String.endsWith(pOp2.String.toString());
    break;
  default:
    pOutResult = false;
    _DBGPRINT("evaluateTermString Invalid operator for type string logical compare operator %s\n",decode_ZSOPV(pOperator.Type))
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

ZStatus
evaluateTermInteger(bool & pOutReturn,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZSearchOperandType_type wType = pOp2.Type & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
    pOp2.Integer = pOp2.String.toLong();
    pOp2.Type = ZSTO_Integer;
    break;
  case ZSTO_Integer:
    break;
  case ZSTO_Float:
    pOp2.Integer = long(pOp2.Float);
    pOp2.Type = ZSTO_Integer;
    break;

  case ZSTO_Bool:
  case ZSTO_UriString:
  case ZSTO_Date:
  case ZSTO_Checksum:
    _DBGPRINT("evaluateTermInteger Cannot compare integer (operand 1) with %s (operand 2) invalid result",
        decode_OperandType(wType))
    return ZS_INVOP;
  } // switch

  switch (pOperator.Type)
  {
  case ZSOPV_EQUAL:
    pOutReturn = (pOp1.Integer == pOp2.Integer) ;
    break;
  case ZSOPV_NOTEQUAL:
    pOutReturn = ( pOp1.Integer != pOp2.Integer) ;
    break;
  case ZSOPV_LESS:
    pOutReturn = ( pOp1.Integer < pOp2.Integer);
    break;
  case ZSOPV_GREATER:
    pOutReturn = ( pOp1.Integer > pOp2.Integer);
    break;
  case ZSOPV_LESSEQUAL:
    pOutReturn = ( pOp1.Integer <= pOp2.Integer);
    break;
  case ZSOPV_GREATEREQUAL:
    pOutReturn = ( pOp1.Integer >= pOp2.Integer);
    break;
  default:
    _DBGPRINT("evaluateTermInteger Invalid operator for integer logical compare operation %s\n",decode_ZSOPV(pOperator.Type))
    pOutReturn = false;
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}


bool approximatelyEqual(double a, double b)
{
  return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * std::numeric_limits<double>::epsilon());
}

ZStatus
evaluateTermFloat(bool & pOutReturn,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZSearchOperandType_type wType = pOp2.Type & ZSTO_BaseMask;

  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
    pOp2.Float = pOp2.String.toDouble();
    pOp2.Type = ZSTO_Float;
    break;
  case ZSTO_Float:
    break;
  case ZSTO_Integer:
    pOp2.Float = double(pOp2.Integer);
    pOp2.Type = ZSTO_Float;
    break;

  case ZSTO_Bool:
  case ZSTO_UriString:
  case ZSTO_Date:
  case ZSTO_Checksum:
    _DBGPRINT("evaluateTermFloat Cannot compare double (operand 1) with %s (operand 2) invalid result",
        decode_OperandType(wType))
    return ZS_INVOP;
  } // switch

  switch (pOperator.Type)
  {
  case ZSOPV_EQUAL:
    pOutReturn = approximatelyEqual(pOp1.Float, pOp2.Float) ;
    break;
  case ZSOPV_NOTEQUAL:
    pOutReturn = ! approximatelyEqual(pOp1.Float, pOp2.Float);
    break;
  case ZSOPV_LESS:
    pOutReturn = ( pOp1.Float < pOp2.Float);
    break;
  case ZSOPV_GREATER:
    pOutReturn = ( pOp1.Float > pOp2.Float);
    break;
  case ZSOPV_LESSEQUAL:
    pOutReturn = ( pOp1.Float < pOp2.Float) || approximatelyEqual(pOp1.Float, pOp2.Float);
    break;
  case ZSOPV_GREATEREQUAL:
    pOutReturn = ( pOp1.Float > pOp2.Float) || approximatelyEqual(pOp1.Float, pOp2.Float);
    break;
  default:
    _DBGPRINT("evaluateTermFloat Invalid operator for type double logical compare %s\n",decode_ZSOPV(pOperator.Type))
    pOutReturn = false;
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

ZStatus
evaluateTermDate(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZDatePrecision wDP=ZDTPR_nano;

  ZSearchOperandType_type wType = pOp2.Type & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
    pOp2.Date = ZDateFull::fromString(pOp2.String);
    pOp2.Type = ZSTO_Date;
    break;
  case ZSTO_UriString:
  case ZSTO_Bool:
  case ZSTO_Integer:
  case ZSTO_Float:
  case ZSTO_Checksum:
    _DBGPRINT("evaluateTermDate Cannot compare tyoe ZDate (operand 1) with %s (operand 2) result set to invalid",
        decode_OperandType(pOp2.Type))
    return ZS_INVOP;

  case ZSTO_Date:
    break;
  } // switch

  switch (pOperator.Type)
  {
  case ZSOPV_EQUAL:

    wDP = std::min(pOp1.Date.Precision,pOp2.Date.Precision);

    pOutResult = pOp1.Date._Compare(pOp2.Date,wDP)==0;
    break;
  case ZSOPV_NOTEQUAL:
    pOutResult = pOp1.Date._Compare(pOp2.Date,wDP)!=0;
    break;
  case ZSOPV_LESS:
    pOutResult = pOp1.Date._Compare(pOp2.Date,wDP) < 0;
    break;
  case ZSOPV_GREATER:
    pOutResult = pOp1.Date._Compare(pOp2.Date,wDP) > 0;
    break;
  case ZSOPV_LESSEQUAL:
    pOutResult = pOp1.Date._Compare(pOp2.Date,wDP)<=0;
    break;
  case ZSOPV_GREATEREQUAL:
    pOutResult = pOp1.Date._Compare(pOp2.Date,wDP)>=0;
    break;

  default:
    _DBGPRINT("Invalid operator for type ZDateFull logical compare operator %s\n",decode_ZSOPV(pOperator.Type))
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

ZStatus
evaluateTermResource(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZSearchOperandType_type wType = pOp2.Type & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_Resource:
    break;
  default:
    _DBGPRINT("evaluateTermResource Cannot compare type ZResource (operand 1) with %s (operand 2) result set to invalid",
        decode_OperandType(pOp2.Type))
    return ZS_INVOP;
  } // switch

  switch (pOperator.Type)
  {
  case ZSOPV_EQUAL:
    pOutResult = pOp1.Resource == pOp2.Resource;
    fprintf(stdout,"evaluateTerm- Op1 <%s> == Op2 <%s> Result %s\n",
        pOp1.Resource.toHexa().toString(),
        pOp1.Resource.toHexa().toString(), pOutResult?"true":"false");
    break;
  case ZSOPV_NOTEQUAL:
    pOutResult = !(pOp1.Resource == pOp2.Resource);
    fprintf(stdout,"evaluateTerm- Op1 <%s> != Op2 <%s> Result %s\n",
        pOp1.Resource.toHexa().toString(),
        pOp1.Resource.toHexa().toString(), pOutResult?"true":"false");
    break;

  default:
    _DBGPRINT("Invalid operator for ZResource logical compare operation %s\n",decode_ZSOPV(pOperator.Type))
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

ZStatus
evaluateTermChecksum(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZDatePrecision wDP=ZDTPR_nano;

  ZSearchOperandType_type wType = pOp2.Type & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
    pOp2.CheckSum.fromHexa(pOp2.String);
    pOp2.Type = ZSTO_Checksum;
    break;
  case ZSTO_Checksum:
    break;
  default:
    _DBGPRINT("evaluateTermChecksum Cannot compare tyoe Checksum (operand 1) with %s (operand 2) result set to invalid",
        decode_OperandType(pOp2.Type))
    return ZS_INVOP;
  } // switch

  switch (pOperator.Type)
  {
  case ZSOPV_EQUAL:
    pOutResult = pOp1.CheckSum == pOp2.CheckSum ;
    break;
  case ZSOPV_NOTEQUAL:
    pOutResult = pOp1.CheckSum != pOp2.CheckSum ;
    break;


  default:
    _DBGPRINT("Invalid operator for type Checksum logical compare operator %s\n",decode_ZSOPV(pOperator.Type))
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

/* Manage possible conversion then logical operation between two operands.
 * the first operand defines the conversion if necessary
 */

ZStatus
evaluateTerm(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{

  ZSearchOperandType_type wType = pOp1.Type & ZSTO_BaseMask;
  switch (wType)
  {
  case ZSTO_String:
    return evaluateTermString(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Integer:
    return evaluateTermInteger(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Float:
    return evaluateTermFloat(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Date:
    return evaluateTermDate(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Resource:
    return evaluateTermResource(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Checksum:
    return evaluateTermChecksum(pOutResult,pOp1,pOp2,pOperator);
  }

  return ZS_INVOP;
}

