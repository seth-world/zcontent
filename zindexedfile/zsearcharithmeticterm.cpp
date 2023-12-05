#include "zsearcharithmeticterm.h"
#include "zsearchlogicalterm.h"

using namespace zbs;

namespace zbs {


ZSearchOperandType ZSTO_Def = ZSTO_Nothing;


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



ZStatus
computeArithmeticOperation(ZOperandContent & pOperand1,
                           ZOperandContent & pOperand2,
                           ZSearchOperator& pOperator,
                           ZSearchOperandType_type pRequestedType,
                           ZOperandContent &pResult)
{
//    pResult.clear();

    ZStatus wSt=ZS_SUCCESS;

    if (pRequestedType!=(pOperand1.OperandZSTO & ZSTO_BaseMask)) {

        _DBGPRINT("computeArithmeticOperation-E-INVTYP Operand 1 - Expected operand type <%s> while got <%s>\n",
                  decode_OperandType(pRequestedType),decode_OperandType(pOperand1.OperandZSTO & ZSTO_BaseMask))
        return ZS_INVTYPE;  /* return invalid operand content */
    }
    if (pRequestedType!=(pOperand2.OperandZSTO & ZSTO_BaseMask)) {

        _DBGPRINT("computeArithmeticOperation-E-INVTYPE Operand 2 - Expected operand type <%s> while got <%s>\n",
                  decode_OperandType(pRequestedType),decode_OperandType(pOperand2.OperandZSTO & ZSTO_BaseMask))
        return ZS_INVTYPE;  /* return invalid operand content */
    }

    switch (pRequestedType)
    {
    case ZSTO_Integer:
    {
        switch (pOperator.ZSOPV)
        {
        case   ZSOPV_PLUS :
            pResult.setInteger ( pOperand1.getInteger() + pOperand2.getInteger());
            return ZS_SUCCESS;
        case   ZSOPV_MINUS :
            pResult.setInteger ( pOperand1.getInteger() - pOperand2.getInteger());
            return ZS_SUCCESS;
        case   ZSOPV_DIVIDE :
            pResult.setInteger ( pOperand1.getInteger() / pOperand2.getInteger());
            return ZS_SUCCESS;
        case   ZSOPV_MULTIPLY :
            pResult.setInteger ( pOperand1.getInteger() * pOperand2.getInteger());
            return ZS_SUCCESS;
        case   ZSOPV_POWER :
            pResult.setInteger ( pOperand1.getInteger() ^ pOperand2.getInteger());
            return ZS_SUCCESS;
        case   ZSOPV_MODULO :
            pResult.setInteger ( pOperand1.getInteger()  % pOperand2.getInteger());
            return ZS_SUCCESS;
        default:
            _DBGPRINT("computeArithmeticOperation-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
                      decode_ZSOPV(pOperator.ZSOPV),decode_OperandType(pRequestedType))
            return ZS_INVOP;
        }
    } //ZSTO_Integer

    case ZSTO_Float:
        switch (pOperator.ZSOPV)
        {
        case   ZSOPV_PLUS :
            pResult.setFloat ( pOperand1.getFloat() + pOperand2.getFloat());
            return ZS_SUCCESS;
        case   ZSOPV_MINUS :
            pResult.setFloat ( pOperand1.getFloat() - pOperand2.getFloat());
            return ZS_SUCCESS;
        case   ZSOPV_DIVIDE :
            pResult.setFloat ( pOperand1.getFloat() / pOperand2.getFloat());
            return ZS_SUCCESS;
        case   ZSOPV_MULTIPLY :
            pResult.setFloat ( pOperand1.getFloat() * pOperand2.getFloat());
            return ZS_SUCCESS;
        case   ZSOPV_POWER :
            pResult.setFloat ( pow (pOperand1.getFloat()  , pOperand2.getFloat()));
            return ZS_SUCCESS;
        case   ZSOPV_MODULO :
            pResult.setFloat ( fmod (pOperand1.getFloat()  , pOperand2.getFloat()));
            return ZS_SUCCESS;
        default:
            _DBGPRINT("computeArithmeticOperation-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
                      decode_ZSOPV(pOperator.ZSOPV),decode_OperandType(pRequestedType))
            return ZS_INVOP;
        } //ZSTO_Float
    case ZSTO_UriString:
    case ZSTO_String:
        switch (pOperator.ZSOPV)
        {
        case   ZSOPV_PLUS :
        {
            utf8VaryingString* wString = static_cast<utf8VaryingString*> (pResult.Value);
            utf8VaryingString* wString1 = static_cast<utf8VaryingString*> (pOperand1.Value);
            utf8VaryingString* wString2 = static_cast<utf8VaryingString*> (pOperand2.Value);
            wString1->add(*wString2);
            wString->strset(wString1->Data);
            return ZS_SUCCESS;
        }
        case   ZSOPV_MINUS :
        {
            utf8VaryingString* wString = static_cast<utf8VaryingString*> (pResult.Value);
            utf8VaryingString* wString1 = static_cast<utf8VaryingString*> (pOperand1.Value);
            utf8VaryingString* wString2 = static_cast<utf8VaryingString*> (pOperand2.Value);
            wString1->add(*wString2);
            utf8_t* wS = utfExpurgeString<utf8_t>(wString1->Data,wString2->Data);
            wString->strset(wS);
            free(wS);
            return ZS_SUCCESS;

            //      utf8_t* wS = utfExpurgeString<utf8_t>(wResult.String.Data,wResultNext.String.Data);
            //      pOperandContent.String = wS;
            //      free(wS);
            //      break;
        }
        default:
            _DBGPRINT("computeArithmeticOperation-E-INVOP Found invalid operator <%s> for operand type <%s>\n",
                      decode_ZSOPV(pOperator.ZSOPV),decode_OperandType(pRequestedType))
            return ZS_INVOP;
        }
        break; /* not necessary, just to avoid compiler warnings */

    default:
        _DBGPRINT("computeArithmeticOperation-E-INVTYP Found invalid operand type <%s> for an arithmetic expression\n",
                  decode_OperandType(pRequestedType))
        return ZS_INVTYPE;

    }// switch (pMainType)

    return ZS_SUCCESS;
} //computeArithmeticOperation

ZStatus computeArithmeticLiteral(ZSearchArithmeticTerm *&pOperand,
                                 ZSearchOperandType_type &pMainType,
                                 ZOperandContent &pOperandContent)
{
  pOperandContent.clear();

  ZOperandContent wResult , wResultNext;
  ZStatus wSt=ZS_SUCCESS;

  ZSearchArithmeticTerm* wTerm = static_cast<ZSearchArithmeticTerm*>(pOperand);

  ZSearchOperandBase* wOB= static_cast<ZSearchOperandBase*> (wTerm->Operand);

  if (!(wOB->ZSTO & ZSTO_Literal) && !(wOB->ZSTO & ZSTO_Symbol)) {
      _DBGPRINT("computeArithmeticLiteral-E-INVTYPE Expecting Literal or Symbol for operand <%s> while got %X-%s",
                wOB->FullFieldName.toString(),wOB->ZSTO,decode_ZSTO(wOB->ZSTO));
      return ZS_INVTYPE;
  }

  if (wOB->ZSTO & ZSTO_Arithmetic) {
     wSt = computeArithmeticLiteral(pOperand,pMainType,wResult);
    if (wSt !=ZS_SUCCESS)
      return wSt;
  }
  else {
    wSt = getLiteralOperandContent( wTerm->Operand,&wResult);
    if (wSt !=ZS_SUCCESS)
        return wSt;
  }
  if (pMainType == ZSTO_Nothing) {
    pMainType = ZSearchOperandType(wResult.OperandZSTO & ZSTO_BaseMask);
  }
  else
      if (pMainType!=(wResult.OperandZSTO & ZSTO_BaseMask)) {

    _DBGPRINT("computeArithmeticLiteral-E-INVTYP Expected literal operand type <%s> while got <%s>\n",
        decode_OperandType(pMainType),decode_OperandType(wResult.OperandZSTO & ZSTO_BaseMask))
    return ZS_INVTYPE;  /* return invalid operand content */
  }

  if (wTerm->OperandNext==nullptr) {
      pOperandContent=wResult;
      return ZS_EOF;
  }

 ZSearchArithmeticTerm* wTermNext = static_cast<ZSearchArithmeticTerm*>(wTerm->OperandNext);

  wSt = computeArithmeticLiteral(wTermNext,pMainType,wResultNext);
  if (wSt!=ZS_SUCCESS) {
      pOperandContent=wResult;
      return wSt ;
  }

  /* now do effective computation according main type */
  return computeArithmeticOperation(wResult,wResultNext,wTerm->Operator,pMainType & ZSTO_BaseMask ,pOperandContent);

} //computeArithmeticLiteral

ZStatus
computeArithmetic(ZSearchEntityContext& pSEC,ZSearchArithmeticTerm*& wTerm,  ZSearchOperandType_type& pMainType,ZOperandContent& pOperandContent)
{
  ZOperandContent wResult , wResultNext;
  ZStatus wSt=ZS_SUCCESS;

//  ZSearchArithmeticTerm* wTerm = static_cast<ZSearchArithmeticTerm*>(pOperand);

  if (wTerm->ZSTO & ZSTO_Arithmetic) {
    wSt = computeArithmetic(pSEC,wTerm,pMainType,wResult);
    if (wSt!=ZS_SUCCESS)
      return wSt;
  }
  else {
      if (wTerm->ZSTO & ZSTO_Literal)
        wSt=computeArithmeticLiteral(wTerm,pMainType,pOperandContent);
      else
        wSt = buildOperandContent( pSEC,wTerm,&wResult);
  }
  if (pMainType == ZSTO_Nothing) {
    pMainType = ZSearchOperandType(wResult.OperandZSTO & ZSTO_BaseMask);
  }
  else
      if (pMainType!=(wResult.OperandZSTO & ZSTO_BaseMask)) {

    _DBGPRINT("computeArithmetic-E-INVTYP Expected literal operand type <%s> while got <%s>\n",
        decode_OperandType(pMainType),decode_OperandType(wResult.OperandZSTO & ZSTO_BaseMask))
    return ZS_INVTYPE;  /* return invalid operand content */
  }

  if (wTerm->OperandNext==nullptr) {
      pOperandContent = wResult;
      return ZS_EOF;
  }

  ZSearchArithmeticTerm* wTermNext = static_cast<ZSearchArithmeticTerm*>(wTerm->OperandNext);
  wSt = computeArithmetic(pSEC,wTermNext,pMainType,wResultNext);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  /* now do effective computation according main type */
  return computeArithmeticOperation(wResult,wResultNext,wTerm->Operator,pMainType & ZSTO_BaseMask ,pOperandContent);

} //computeArithmetic



void _countArithmeticTerm(ZSearchArithmeticTerm* pArithTerm,CountTerm &wCT)
{

    if (pArithTerm==nullptr)
        return ;
    wCT.ArithTerm ++;

    while (true) {
        if (pArithTerm->Operand == nullptr)
            return ;
        ZSearchOperandBase* wOpB = static_cast<ZSearchOperandBase*>(pArithTerm->Operand);
        if ( wOpB->ZSTO == ZSTO_Arithmetic ) {
            _countArithmeticTerm(static_cast<ZSearchArithmeticTerm*>(pArithTerm->Operand),wCT);
            break;
        }
        if ( wOpB->ZSTO == ZSTO_Logical ) {
            _countLogicalTerm(static_cast<ZSearchLogicalTerm*>(pArithTerm->Operand),wCT);
            break;
        }
        wCT.ArithOperand ++;
        break;
    }
    if (pArithTerm->Operator._isNothing())
        return;
    wCT.ArithOperator ++;

    while (true) {
        if (pArithTerm->OperandNext == nullptr)
            return ;
        ZSearchOperandBase* wOpB = static_cast<ZSearchOperandBase*>(pArithTerm->OperandNext);
        if ( wOpB->ZSTO == ZSTO_Arithmetic ) {
            ZSearchArithmeticTerm* wOpAr = static_cast<ZSearchArithmeticTerm*>(pArithTerm->OperandNext);
            _countArithmeticTerm(wOpAr,wCT);
            break;
        }
        if ( wOpB->ZSTO == ZSTO_Logical ) {
            ZSearchLogicalTerm* wOpLo = static_cast<ZSearchLogicalTerm*>(pArithTerm->OperandNext);
            _countLogicalTerm(wOpLo,wCT);
            break;
        }
        wCT.ArithOperand ++;
        break;
    }
    return ;
}


}// namespace zbs
