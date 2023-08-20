#include "zsearch.h"

#include <zindexedfile/zmasterfile.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zdatefull.h>

#include <zcontentcommon/urfparser.h>

//#include "zsearchformula.h"

using namespace zbs;

ZStatus
ZSearch::search()
{
  zaddress_type   wAddress=0;
  ZStatus wSt=ZS_SUCCESS;
  ZDataBuffer wRecord;
  long wRank=0;

  //wSt=MasterFile.zgetWAddress(wRecord,wRank,wAddress);



  return wSt;

}// ZSearch::search()

/*

ZStatus
ZSearch::_getLitteralOperand(ZSearchLitteral* pOperand, ZOperandContent& pOutContent)
{
  ZStatus wSt=ZS_SUCCESS;
  uint16_t wType = pOperand->Type & ZSRCT_BaseType ;
  switch (wType) {
  case ZSRCT_String:
    pOutContent.Str = pOperand->toString();
    break;
  case ZSRCT_Integer:
    pOutContent.Integer = static_cast<int>(*pOperand->Data);
    break;
  case ZSRCT_Float:
    pOutContent.Double = static_cast<double>(*pOperand->Data);
    break;
  case ZSRCT_Date:
    memmove(&pOutContent.Date,pOperand->Data,sizeof(ZDateFull));
    break;
  default:
    return ZS_INVTYPE;
  } // switch
  return ZS_SUCCESS;
}

ZStatus
ZSearch::_getStringModifier(ZSearchFieldOperand* pOperand, ZOperandContent &pContent)
{
  if(!pOperand->HasModifier)
    return ZS_INVOP;
  if (pOperand->ModifierName.compareCase("LEFT")) {
    pContent.Str=pContent.Str.Left(pOperand->ModVal1);
    return ZS_SUCCESS;
  }
  if (pOperand->ModifierName.compareCase("RIGHT")) {
    pContent.Str=pContent.Str.Right(pOperand->ModVal1);
    return ZS_SUCCESS;
  }
  if (pOperand->ModifierName.compareCase("SUBSTRING")) {
    pContent.Str=pContent.Str.subString(pOperand->ModVal1,pOperand->ModVal2);
    return ZS_SUCCESS;
  }

  fprintf (stderr,"ZSearch::_getStringModifier-E-INVPARAMS Invalid modifier code for utf string <%s>\n",pOperand->ModifierName.toCChar());
  return ZS_INVPARAMS;
}

ZStatus
ZSearch::_getDateModifier(ZSearchFieldOperand* pOperand, ZOperandContent &pContent)
{
  if (pOperand->ModifierName.compareCase("YEAR")){
    pContent.Integer = long(pContent.Date.year());
    pContent.Type = ZSRCT_Integer;
    return ZS_SUCCESS;
  }

  if (pOperand->ModifierName.compareCase("MONTH")){
    pContent.Integer = long(pContent.Date.month());
    pContent.Type = ZSRCT_Integer;
    return ZS_SUCCESS;
  }
  if (pOperand->ModifierName.compareCase("DAY")){
    pContent.Integer = long(pContent.Date.day());
    pContent.Type = ZSRCT_Integer;
    return ZS_SUCCESS;
  }
  if (pOperand->ModifierName.compareCase("HOUR")){
    pContent.Integer = long(pContent.Date.hour());
    pContent.Type = ZSRCT_Integer;
    return ZS_SUCCESS;
  }
  if (pOperand->ModifierName.compareCase("MIN")){
    pContent.Integer = long(pContent.Date.min());
    pContent.Type = ZSRCT_Integer;
    return ZS_SUCCESS;
  }
  if (pOperand->ModifierName.compareCase("SEC")){
    pContent.Integer = long(pContent.Date.sec());
    pContent.Type = ZSRCT_Integer;
    return ZS_SUCCESS;
  }
  fprintf (stderr,"ZSearch::_getDateModifier-E-INVPARAMS Invalid modifier code for ZDateFull <%s>\n",pOperand->ModifierName.toCChar());
  return ZS_INVPARAMS;
}

ZStatus
ZSearch::_getResourceModifier(ZSearchFieldOperand* pOperand, ZOperandContent &pContent)
{
  if (pOperand->ModifierName.compareCase("ENTITY")){
    pContent.Integer = long(pContent.Date.year());
    pContent.Type = ZSRCT_Integer;
    return ZS_SUCCESS;
  }
  if (pOperand->ModifierName.compareCase("ID")){
    pContent.Integer = long(pContent.Date.year());
    pContent.Type = ZSRCT_Integer;
    return ZS_SUCCESS;
  }

  fprintf (stderr,"ZSearch::_getResourceModifier-E-INVPARAMS Invalid modifier code for ZResource <%s>\n",pOperand->ModifierName.toCChar());
  return ZS_INVPARAMS;
}

ZStatus
ZSearch::_getFieldOperand(const ZDataBuffer& pRecord,ZSearchFieldOperand* pOperand, ZOperandContent &pContent)
{
  ZStatus wSt=ZS_SUCCESS;
  URFField wF ;
  ssize_t wRet=0;
  const unsigned char* wPtr=nullptr;
  ZSearchFieldOperand* wF1 = static_cast<ZSearchFieldOperand*>(pOperand);

  wSt=MasterFile.getURFFieldByRank(pRecord,wF,pOperand->MDicRank);
  if ((!wF.Present) ||(wSt!=ZS_SUCCESS)) {
    pContent.Bool=false;
    return ZS_FIELDMISSING;
  }

  wPtr=wF.Ptr;
  switch(wF.ZType)
  {
  case ZType_Utf8VaryingString:
    wRet=pContent.Str._importURF(wF.Ptr);
    pContent.Type=ZSRCT_String;
    if (pOperand->HasModifier)
      return _getStringModifier(pOperand,pContent);
    return wRet>0?ZS_SUCCESS:ZS_CONVERR;
  case ZType_Utf16VaryingString:
  {
    utf16VaryingString wInStr;
    wRet=wInStr._importURF(wF.Ptr);
    pContent.Str.fromUtf16(wInStr);
    pContent.Type=ZSRCT_String;
    if (pOperand->HasModifier)
      return _getStringModifier(pOperand,pContent);
    return wRet>0?ZS_SUCCESS:ZS_CONVERR;
  }
  case ZType_Utf32VaryingString:
  {
    utf32VaryingString wInStr;
    wRet=wInStr._importURF(wF.Ptr);
    pContent.Str.fromUtf32(wInStr);
    pContent.Type=ZSRCT_String;
    if (pOperand->HasModifier)
      return _getStringModifier(pOperand,pContent);
    return wRet>0?ZS_SUCCESS:ZS_CONVERR;
  }
  case ZType_ZDateFull:
  {
    pContent.Date._importURF(wF.Ptr);
    pContent.Type=ZSRCT_Date;
    if (pOperand->HasModifier)
      return _getDateModifier(pOperand,pContent);
    return ZS_SUCCESS;
  }
  case ZType_UChar:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    unsigned char wC;
    _importAtomic<unsigned char>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_U8:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    uint8_t wC;
    _importAtomic<uint8_t>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_S8:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    int8_t wC;
    _importAtomic<int8_t>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_U16:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    uint16_t wC;
    _importAtomic<uint16_t>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_S16:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    int16_t wC;
    _importAtomic<int16_t>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_U32:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    uint32_t wC;
    _importAtomic<uint32_t>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_S32:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    int32_t wC;
    _importAtomic<int32_t>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_U64:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    uint64_t wC;
    _importAtomic<uint64_t>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_S64:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    int64_t wC;
    _importAtomic<int64_t>(wC,wPtr);
    pContent.Integer = long (wC);
    pContent.Type=ZSRCT_Integer;
    break;
  }
  case ZType_Float:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    float wC;
    _importAtomic<float>(wC,wPtr);
    pContent.Double = double (wC);
    pContent.Type=ZSRCT_Float;
    break;
  }
  case ZType_Double:
  {
    wPtr=wF.Ptr;
    wPtr+= sizeof(ZTypeBase);
    float wC;
    _importAtomic<double>(pContent.Double,wPtr);
    pContent.Type=ZSRCT_Float;
    break;
  }
  }// switch

  return ZS_SUCCESS;
}


bool
ZSearch::_processFormula(ZOperandContent& wOp1,ZOperandContent& wOp2,ZSearchOperator_type pOperator)
{
  bool wResult=false;
  if (wOp1.Type != wOp2.Type) {

  }

  if (wOp1.Type==ZSRCT_String) {
    switch(pOperator){
    case ZSOP_Equal:
      return (wOp1.Str.compare(wOp2.Str)==0);
    case ZSOP_Greater:
      return (wOp1.Str.compare(wOp2.Str)>0);
    case ZSOP_Less:
      return (wOp1.Str.compare(wOp2.Str)<0);
    case ZSOP_GreaterEqual:
      return (wOp1.Str.compare(wOp2.Str)>=0);
    case ZSOP_LessEqual:
      return (wOp1.Str.compare(wOp2.Str)<=0);
    case ZSOP_StartsWith:
      return (wOp1.Str.startsWith(wOp2.Str.toString())<=0);
    case ZSOP_EndsWith:
      return (wOp1.Str.endsWith(wOp2.Str.toString())<=0);

    case ZSOP_Contains:
      return (wOp1.Str.strstr(wOp2.Str.toString())!=nullptr);
    default:
      fprintf(stderr,"Invalid string operation\n");
    }
  }

  return wResult;
}

*/

bool
ZSearch::_evaluateFormulaTerm(const ZDataBuffer& pRecord,const ZSearchLogicalOperand& pElt) {

  ZStatus wSt=ZS_SUCCESS;
  const unsigned char* wPtr=nullptr;

  bool wMainReturn=false;
  /*
  if ((pElt.getOperandBase(0)->Type & ZSTO_Field)==ZSTO_Field) {

  }


  while (true) {

    if (pElt.getOperandBase()->Type & ZSTO_FieldString) {
      ZSearchTerm* wF1 = static_cast<ZSearchTerm*>(pElt.Operand);
      wReturn = _evaluateFormulaTerm(pRecord,*wF1);
      break;
    }

    if (pElt.Type & ZSRCT_Litteral) {
      wSt=_getLitteralOperand(static_cast<ZSearchLitteral*> (pElt.Operand),wOp2);
      break;
    }// if (pElt.LType & ZSRCR_Litteral)

    if (pElt.Type & ZSRCT_Field) {
      ZSearchFieldOperand* wF1 = static_cast<ZSearchFieldOperand*>(pElt.Operand);
      wSt =_getFieldOperand (pRecord,wF1,wOp1);
      if (wSt!=ZS_SUCCESS)
        return false;
      break;
    }
    fprintf(stderr,"Invalid operand type found for left operand\n");
    return false;
  }// while true


  while (true) {

    if (pElt.Type & ZSRCT_Formula) {
      ZSearchTerm* wF1 = static_cast<ZSearchTerm*>(pElt.ROperand);
      wReturn = _evaluateFormulaTerm(pRecord,*wF1);
      break;
    }

    if (pElt.Type & ZSRCT_Litteral) {
      wSt=_getLitteralOperand(static_cast<ZSearchLitteral*> (pElt.ROperand),wOp2);
      break;
    }// if (pElt.LType & ZSRCR_Litteral)

    if (pElt.Type & ZSRCT_Field) {
      ZSearchFieldOperand* wF1 = static_cast<ZSearchFieldOperand*>(pElt.ROperand);
      wSt =_getFieldOperand (pRecord,wF1,wOp1);
      if (wSt!=ZS_SUCCESS)
        return false;
      break;
    }
    fprintf(stderr,"Invalid operand type found for right operand\n");
    return false;
  }// while true

  return _processFormula(wOp1,wOp2,pElt.MiddleOperator);

*/
  return wMainReturn;
} // ZSearch::_evaluateFormula

bool
ZSearch::_evaluateRecord(const ZDataBuffer& pRecord)
{
  bool wResult = false;
/*  for (long wi=0; wi < Phrase.count(); wi++) {

  }
*/
  return wResult;
} // _evaluateRecord

