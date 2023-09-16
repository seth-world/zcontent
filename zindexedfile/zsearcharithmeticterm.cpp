#include "zsearcharithmeticterm.h"

using namespace zbs;

ZSearchOperandType ZSTO_Def = ZSTO_Nothing;

/*
ZOperandContent
ZSearchArithmeticTerm::compute(URFParser& pURFParser)
{

}

ZOperandContent
ZSearchArithmeticTerm::computeLiteral()
{

}
*/

void
ZSearchArithmeticTerm::copyOperand(void*& pOperand,const void* pOpIn)
{
  //  if (pOperand!=nullptr)
  //    clearOperand(pOperand);
  _copyOperand(pOperand,pOpIn);
}



ZSearchArithmeticTerm&
ZSearchArithmeticTerm::_copyFrom(const ZSearchArithmeticTerm& pIn)
{
  ZSearchOperandBase::_copyFrom((ZSearchOperandBase)pIn);
  ParenthesisLevel = pIn.ParenthesisLevel;
  copyOperand(Operand,pIn.Operand);
  copyOperand(OperandNext,pIn.OperandNext);
  Operator._copyFrom(pIn.Operator);
  return *this;
}

ZSearchArithmeticTerm&
ZSearchArithmeticTerm::_copyFrom(const ZSearchArithmeticTerm* pIn)
{
  ZSearchOperandBase::_copyFrom((ZSearchOperandBase)*pIn);
  ParenthesisLevel = pIn->ParenthesisLevel;
  copyOperand(Operand,pIn->Operand);
  copyOperand(OperandNext,pIn->OperandNext);
  Operator._copyFrom(pIn->Operator);
  return *this;
}

ZOperandContent
computeArithmeticLiteral(void*& pOperand, ZSearchOperandType& pMainType)
{
  ZOperandContent wResult , wResultNext;

  ZSearchArithmeticTerm* wTerm = static_cast<ZSearchArithmeticTerm*>(pOperand);

  ZSearchOperandBase* wOB= static_cast<ZSearchOperandBase*> (wTerm->Operand);

  if (wOB->Type & ZSTO_Arithmetic) {
    wResult = computeArithmeticLiteral(pOperand,pMainType);
    if (!wResult.isValid())
      return wResult;
  }
  else {
    wResult = getLiteralOperandContent( wTerm->Operand);

  }
  if (pMainType == ZSTO_Nothing) {
    pMainType = ZSearchOperandType(wResult.Type & ZSTO_BaseMask);
  }
  else
      if (pMainType!=(wResult.Type & ZSTO_BaseMask)) {

    _DBGPRINT("computeArithmeticLiteral-E-INVTYP Expected literal operand type <%s> while got <%s>\n",
        decode_OperandType(pMainType),decode_OperandType(wResult.Type & ZSTO_BaseMask))
    return ZOperandContent(false);  /* return invalid operand content */
  }

  if (wTerm->OperandNext==nullptr)
    return wResult;


  wResultNext = computeArithmeticLiteral(wTerm->OperandNext,pMainType);
  if (!wResultNext.isValid())
    return wResultNext;

  /* now do effective computation according main type */

  switch (pMainType)
  {
  case ZSTO_Integer:
  {
    switch (wTerm->Operator.Type)
    {
    case   ZSOPV_PLUS :
      wResult.Integer = wResult.Integer + wResultNext.Integer;
      return wResult;
    case   ZSOPV_MINUS :
      wResult.Integer = wResult.Integer - wResultNext.Integer;
      return wResult;
    case   ZSOPV_DIVIDE :
      wResult.Integer = wResult.Integer / wResultNext.Integer;
      return wResult;
    case   ZSOPV_MULTIPLY :
      wResult.Integer = wResult.Integer * wResultNext.Integer;
      return wResult;
    case   ZSOPV_POWER :
      wResult.Integer = wResult.Integer ^ wResultNext.Integer;
      return wResult;
    case   ZSOPV_MODULO :
      wResult.Integer = wResult.Integer % wResultNext.Integer;
      return wResult;
    default:
      _DBGPRINT("computeArithmeticLiteral-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
          decode_ZSOPV(wTerm->Operator.Type),decode_OperandType(pMainType))
      return ZOperandContent(false);
    }

  case ZSTO_Float:
    switch (wTerm->Operator.Type)
    {
    case   ZSOPV_PLUS :
      wResult.Float = wResult.Float + wResultNext.Float;
      return wResult;
    case   ZSOPV_MINUS :
      wResult.Float = wResult.Float - wResultNext.Float;
      return wResult;
    case   ZSOPV_DIVIDE :
      wResult.Float = wResult.Float / wResultNext.Float;
      return wResult;
    case   ZSOPV_MULTIPLY :
      wResult.Float = wResult.Float * wResultNext.Float;
      return wResult;
    case   ZSOPV_POWER :
      wResult.Float = pow (wResult.Float , wResultNext.Float);
      return wResult;
    case   ZSOPV_MODULO :
      wResult.Float = fmod (wResult.Float , wResultNext.Float);
      return wResult;
    default:
      _DBGPRINT("computeArithmeticLiteral-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
          decode_ZSOPV(wTerm->Operator.Type),decode_OperandType(pMainType))
      return ZOperandContent(false);
    }
  }
  case ZSTO_UriString:
  case ZSTO_String:
    switch (wTerm->Operator.Type)
    {
    case   ZSOPV_PLUS :
      wResult.String.add(wResultNext.String);
      break;

    case   ZSOPV_MINUS :
    {
      utf8_t* wS = utfExpurgeString<utf8_t>(wResult.String.Data,wResultNext.String.Data);
      wResult.String = wS;
      free(wS);
      break;
    }
    default:
      _DBGPRINT("computeArithmeticLiteral-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
          decode_ZSOPV(wTerm->Operator.Type),decode_OperandType(pMainType))
      return ZOperandContent(false);
    }
    break; /* not necessary, just to avoid compiler warnings */

  default:
    _DBGPRINT("computeArithmeticLiteral-E-INVTYP Found invalid operand type <%s> for an arithmetic expression\n",
        decode_OperandType(pMainType))
    return ZOperandContent(false);

  }// switch (pMainType)

  return wResult;
} //computeArithmeticLiteral

ZOperandContent
computeArithmetic(void*& pOperand, URFParser &pURFParser, ZSearchOperandType& pMainType)
{
  ZOperandContent wResult , wResultNext;

  ZSearchArithmeticTerm* wTerm = static_cast<ZSearchArithmeticTerm*>(pOperand);

  if (wTerm->Type & ZSTO_Arithmetic) {
    wResult = computeArithmeticLiteral(pOperand,pMainType);
    if (!wResult.isValid())
      return wResult;
  }
  else {
    wResult = getOperandContent( pOperand,pURFParser);

  }
  if (pMainType == ZSTO_Nothing) {
    pMainType = ZSearchOperandType(wResult.Type & ZSTO_BaseMask);
  }
  else
      if (pMainType!=(wResult.Type & ZSTO_BaseMask)) {

    _DBGPRINT("computeArithmeticLiteral-E-INVTYP Expected literal operand type <%s> while got <%s>\n",
        decode_OperandType(pMainType),decode_OperandType(wResult.Type & ZSTO_BaseMask))
    return ZOperandContent(false);  /* return invalid operand content */
  }

  if (wTerm->OperandNext==nullptr)
    return wResult;


  wResultNext = computeArithmeticLiteral(wTerm->OperandNext,pMainType);
  if (!wResultNext.isValid())
    return wResultNext;

  /* now do effective computation according main type */

  switch (pMainType)
  {
  case ZSTO_Integer:
  {
    switch (wTerm->Operator.Type)
    {
    case   ZSOPV_PLUS :
      wResult.Integer = wResult.Integer + wResultNext.Integer;
      return wResult;
    case   ZSOPV_MINUS :
      wResult.Integer = wResult.Integer - wResultNext.Integer;
      return wResult;
    case   ZSOPV_DIVIDE :
      wResult.Integer = wResult.Integer / wResultNext.Integer;
      return wResult;
    case   ZSOPV_MULTIPLY :
      wResult.Integer = wResult.Integer * wResultNext.Integer;
      return wResult;
    case   ZSOPV_POWER :
      wResult.Integer = wResult.Integer ^ wResultNext.Integer;
      return wResult;
    case   ZSOPV_MODULO :
      wResult.Integer = wResult.Integer % wResultNext.Integer;
      return wResult;
    default:
      _DBGPRINT("computeArithmeticLiteral-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
          decode_ZSOPV(wTerm->Operator.Type),decode_OperandType(pMainType))
      return ZOperandContent(false);
    }

  case ZSTO_Float:
    switch (wTerm->Operator.Type)
    {
    case   ZSOPV_PLUS :
      wResult.Float = wResult.Float + wResultNext.Float;
      return wResult;
    case   ZSOPV_MINUS :
      wResult.Float = wResult.Float - wResultNext.Float;
      return wResult;
    case   ZSOPV_DIVIDE :
      wResult.Float = wResult.Float / wResultNext.Float;
      return wResult;
    case   ZSOPV_MULTIPLY :
      wResult.Float = wResult.Float * wResultNext.Float;
      return wResult;
    case   ZSOPV_POWER :
      wResult.Float = pow (wResult.Float , wResultNext.Float);
      return wResult;
    case   ZSOPV_MODULO :
      wResult.Float = fmod (wResult.Float , wResultNext.Float);
      return wResult;
    default:
      _DBGPRINT("computeArithmeticLiteral-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
          decode_ZSOPV(wTerm->Operator.Type),decode_OperandType(pMainType))
      return ZOperandContent(false);
    }
  }
  case ZSTO_UriString:
  case ZSTO_String:
    switch (wTerm->Operator.Type)
    {
    case   ZSOPV_PLUS :
      wResult.String.add(wResultNext.String);
      break;
    case   ZSOPV_MINUS :
    {
      utf8_t* wS = utfExpurgeString<utf8_t>(wResult.String.Data,wResultNext.String.Data);
      wResult.String = wS;
      free(wS);
      break;
    }
    default:
      _DBGPRINT("computeArithmeticLiteral-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
          decode_ZSOPV(wTerm->Operator.Type),decode_OperandType(pMainType))
      return ZOperandContent(false);
    }
    break; /* not necessary, just to avoid compiler warnings */

  default:
    _DBGPRINT("computeArithmetic-E-INVTYP Found invalid operand type <%s> for an arithmetic expression\n",
        decode_OperandType(pMainType))
    return ZOperandContent(false);
  }// switch (pMainType)

  return wResult;
} //computeArithmetic
