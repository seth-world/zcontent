#ifndef ZSEARCHOPERAND_H
#define ZSEARCHOPERAND_H

#include <zcontentcommon/zresource.h>
#include <ztoolset/zdatefull.h>

#include "zsearchparsertype.h"
#include "zfielddescription.h"

#include <zcontent/zcontentcommon/urfparser.h>

namespace zbs {

/* a Collection has its own name (identifier) and is a set of record addresses,
 * either extracted with a selection clause or manually made,
 * that refers to a ZSearchEntity, either file entity or collection entity (restricted view on entity) */

class ZOperandContent
{
public:
  ZOperandContent() ;
  ZOperandContent(bool pValid) {Valid=pValid;}
  ZOperandContent(const ZOperandContent& pIn) {_copyFrom(pIn);}

  ZOperandContent& _copyFrom(const ZOperandContent& pIn);
  ZOperandContent& operator = (const ZOperandContent& pIn) {return _copyFrom(pIn);}

  utf8VaryingString display();

  void setInvalid() {Valid=false;}

  bool isValid() {return Valid;}

  bool              Valid=true;
  ZSearchOperandType Type=ZSTO_Nothing;
  long              Integer=0;
  utf8VaryingString String;
  uriString         URI;
  double            Float=0.0;
  bool              Bool=false;
  ZDateFull         Date;
  ZResource         Resource;
  checkSum          CheckSum;
};




class ZSearchToken;
class ZMetaDic;



class ZSearchOperator
{
public:
  ZSearchOperator()=default;
  ZSearchOperator(ZSearchOperator_type pType) {Type = pType;}
  ZSearchOperator(const ZSearchOperator& pIn) { _copyFrom(pIn);  }
  ~ZSearchOperator();
  ZSearchOperator& _copyFrom (const ZSearchOperator& pIn) ;
  ZSearchOperator& operator = (const ZSearchOperator& pIn) {return _copyFrom(pIn);}

  void clear() {
/*    while (TokenList.count())
      TokenList.pop();
*/
    Type=ZSOPV_Nothing;
  }

  void set(ZSearchToken* pTokenOperator) ;
  void add(ZSearchToken* pTokenOperator) ;

  static bool isValid(ZSearchToken* pTokenOperator) ;

  static bool isComparator(ZSearchToken* pTokenOperator) ;
  static bool isLogical(ZSearchToken* pTokenOperator);
  static bool isStringComparator(ZSearchToken* pTokenOperator);
  static bool isArithmeric(ZSearchToken* pTokenOperator) ;

  static bool isAndOr(ZSearchToken* pTokenOperator) ;

  bool _isValid() ;

  bool _isComparator() ;
  bool _isLogical();
  bool _isStringComparator();
  bool _isArithmeric() ;
  bool _isAndOr();
  bool _isNothing();

  bool _isNot() {return Type == ZSOPV_NOT ;}

  bool _isAnd() {return Type = ZSOPV_AND; }
  bool _isOr() {return Type = ZSOPV_OR; }

  ZSearchOperator& operator = (ZSearchOperandType pType) {Type = pType; return *this;}

  ZSearchOperator_type get() {return Type;}

  utf8VaryingString     _report(int pLevel);
  utf8VaryingString     _reportFormula(bool pDetailed=true);

  ZSearchOperator_type  Type=ZSOPV_Nothing;
//  ZArray<ZSearchToken*> TokenList;
} ;


class ZSearchOperandBase
{
public:
  ZSearchOperandBase() {}
  ZSearchOperandBase(ZSearchOperandType pType) {Type=pType;}

  ZSearchOperandBase(const ZSearchOperandBase& pIn) {_copyFrom(pIn);}
  ~ZSearchOperandBase() {

  }

  ZSearchOperandBase& _copyFrom(const ZSearchOperandBase& pIn)
  {
    Type = pIn.Type;
    ModifierType = pIn.ModifierType;
    ModVal1 = pIn.ModVal1;
    ModVal2 = pIn.ModVal2;
    Comment = pIn.Comment;
    return *this;
  }

  ZSearchOperandType_type getType() {return Type;}

  bool hasModifier() {return ModifierType!=ZSRCH_NOTHING;}

  ZSearchOperandBase& operator = (const ZSearchOperandBase& pIn) {return _copyFrom(pIn); }

  ZSearchOperandType    Type=ZSTO_Nothing;
  ZSearchTokentype_type ModifierType=ZSRCH_NOTHING;
  utf8VaryingString     Comment;
  long ModVal1=0;
  long ModVal2=0;
//  ZArray<ZSearchToken*> TokenList;
};


class ZSearchFieldOperandOwnData : public ZSearchOperandBase
{
public:
  ZSearchFieldOperandOwnData()=default;
  ZSearchFieldOperandOwnData(const ZSearchFieldOperandOwnData& pIn):ZSearchOperandBase(pIn)  {_copyFrom(pIn);}
  ZSearchFieldOperandOwnData& _copyFrom(const ZSearchFieldOperandOwnData& pIn)
  {
    ZSearchOperandBase::_copyFrom(pIn);
    MDicRank=pIn.MDicRank;
    MDic=pIn.MDic;
    IndexRank=pIn.IndexRank;  /* index rank to search in if not equal to -1 */
    FullFieldName=pIn.FullFieldName;
    FieldDescription._copyFrom(pIn.FieldDescription);
    return *this;
  }
  ZSearchFieldOperandOwnData& operator=(const ZSearchFieldOperandOwnData& pIn) {return _copyFrom(pIn);}

  ZSearchOperandBase* getOperandBase() {return (ZSearchOperandBase*)this;}

  long              MDicRank=0;
  const ZMetaDic*   MDic=nullptr;
  long              IndexRank=-1;  /* index rank to search in if not equal to -1 */
  ZFieldDescription FieldDescription;
  utf8VaryingString FullFieldName;
};


/** operand type is located within ZSearchOperandBase */
template <class _Tp>
class ZSearchFieldOperand : public ZSearchFieldOperandOwnData {
public:
  ZSearchFieldOperand()=default;
  ZSearchFieldOperand(const ZSearchFieldOperand& pIn) {_copyFrom(pIn);}
  ~ZSearchFieldOperand() { }

  void setOwnData(const ZSearchFieldOperandOwnData& pIn) {ZSearchFieldOperandOwnData::_copyFrom(pIn);}
  void setFieldDescription(const ZFieldDescription& pFD) {FieldDescription._copyFrom(pFD);}

  ZSearchFieldOperand<_Tp>& _copyFrom(const ZSearchFieldOperand<_Tp>& pIn)
  {
    //    ZSearchOperandBase::_copyFrom((ZSearchOperandBase)pIn);
    ZSearchFieldOperandOwnData::_copyFrom((ZSearchFieldOperandOwnData)pIn);
    Content = pIn.Content;
    return *this;
  }


  ZSearchFieldOperand& operator = (const ZSearchFieldOperand& pIn) {return _copyFrom(pIn); }
  _Tp               Content;
};

/*
 *
Valid ZEntity literals

  ZEntity(ZEntity_DocPhysical,0xAB);

Valid checSum literals

  checkSum(0x22890F5D10ADBOF001288890AFB22C3);
             <-------32 hexa digits-------->

Valid date literals

  ZDate(10/12/2023)
  "10/12/2023"

*/

/** operand type is located within ZSearchOperandBase */
template <class _Tp>
class ZSearchLiteral : public ZSearchOperandBase {
public:
  ZSearchLiteral()=default;
  ZSearchLiteral(ZSearchOperandBase& pOB) {ZSearchOperandBase::_copyFrom(pOB);}
  ZSearchLiteral(const ZSearchLiteral& pIn) {_copyFrom(pIn);}
  ~ZSearchLiteral() { }

  void setOperandBase(ZSearchOperandBase& pOB) {
    ZSearchOperandBase::_copyFrom(pOB);
  }

  ZSearchLiteral<_Tp>& _copyFrom(const ZSearchLiteral<_Tp>& pIn)
  {
    ZSearchOperandBase::_copyFrom(pIn);
    Content=pIn.Content;
    return *this;
  }

  ZSearchOperandBase* getOperandBase() {return (ZSearchOperandBase*)this;}

  _Tp Content;
};



/** Former ZSearchTerm
 *
 * a term is a combination of 2 elements fields or literals
 *  and a logical operator that when evaluated, gives a boolean result true or false */

class ZSearchLogicalOperand : public ZSearchOperandBase
{
public:

  ZSearchLogicalOperand() : ZSearchOperandBase(ZSTO_Logical) {}
  ZSearchLogicalOperand(const ZSearchLogicalOperand& pIn) : ZSearchOperandBase(pIn) {_copyFrom(pIn);}
  ~ZSearchLogicalOperand();
  ZSearchLogicalOperand& _copyFrom(const ZSearchLogicalOperand& pIn) ;

  ZSearchLogicalOperand& operator = (const ZSearchLogicalOperand& pIn) {return _copyFrom(pIn);}

  void copyOperand(void *&pOperand, const void *pOpIn) ;

  void clear();


  utf8VaryingString             _reportDetailed(int pLevel);
  utf8VaryingString             _reportFormula();

  static utf8VaryingString      _evaluateOpLiteral(void *pOp);

  /* URFParser is set at the entity level, while accessing records */

  bool            evaluate(URFParser &pURFParser);
  ZOperandContent getContent(URFParser& pURFParser);

  bool            isNull() const {return Operand==nullptr;}

  void*           Operand=nullptr;

};



} // namespace zbs


ZOperandContent getFieldOperandContent (URFField& wField);
ZOperandContent getLiteralOperandContent (void* pLiteral);

ZStatus evaluateTerm(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator);

/** extracts and returns one operands content :
 *  either gets the literal
 *  or extracts the corresponding fields from record using URFParser
 *  applies modifier if any
 */

ZOperandContent gettermOperandContent (void* pOperand, URFParser &pURFParser);
ZOperandContent getOperandContent(void* pOp,URFParser& pURFParser);

utf8VaryingString OperandReportFormula(void * pOperand, bool pDetailed=true);
utf8VaryingString OperandReportDetailed(void *pOperand, int pLevel);
utf8VaryingString OperandReportLiteral(void *pOp);

void clearOperand(void *&pOp);  /* here because it is common to any derived class */
void _copyOperand(void *&pOperand, const void *pOpIn) ;

#endif // ZSEARCHOPERAND_H
