#ifndef ZSEARCHFORMULA_H
#define ZSEARCHFORMULA_H

#include <ztoolset/zarray.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zdatefull.h>
#include <zcontentcommon/zresource.h>

#include "zsearchparsertype.h"

#include "zfielddescription.h"



/*
   <field> + ( <field> + <literal> / (<literal> + <field> )  ) - <literal>

  ZSearchArith
  -----------------
(op <field>)  (oper +)   (next ZSearchArith)

                          (op ZSearchArith)    (oper -)   (next <literal>)

            (op <literal>)  (oper +)  (next ZSearchArith)

                                        (op <literal> (oper /) (next ZSearchArith)

                                                                  (op <literal>)  (oper +) (next <literal>)



*/

class ZSearchFormula;



/*
  <field> == <literal>
  <literal> > <field>

*/

enum ZFormulaType : int {
  ZFORT_Nothing     = 0,
  ZFORT_Term        = 1,    /* points to logical term class ZSeach*/
  ZFORT_Formula     = 2,    /* points to another formula */
  ZFORT_Arithmetic  = 3     /* points to a ZSearchArithmeticOperand */
};


/*
                           +-->trailing link operator
                           |                      ^       +-> open parenthesis = new embedded formula    +--> end embedded formula
                           |                      |       |                                              |
     <field> == <literal> AND <field> < <literal> OR [NOT]( <field> == <literal> AND <field> > <literal> ) AS
                                                            <----term 31------->     <------term 32---->
     <-----term 1------->     <-------term 2---->           <-----Formula 21----> <-----Formula 22---->
                                                               ^              |    |
                                                               |              +----+
                                                             embedded(term is formula)
     <---formula 0-----------> <-------formula 1------> <-------formula 2---->
                            |    |                  |    |
                            +----+                  +----+
                           chained
*/
/* a formula is a term and a leading link logical operator
 * a formula has a result of true or false and is combined with a logical operator {AND-OR} to global logical result
 *        formula result may be inversed by leading NOT operator.
*/
class ZSearchFormula
{
public:
  ZSearchFormula()=default;
  ZSearchFormula(ZFormulaType pType,int pParenthesisLevel) : FormulaType(pType),ParenthesisLevel(pParenthesisLevel) {}
  ZSearchFormula(const ZSearchFormula& pIn) {_copyFrom(pIn);}
  ~ZSearchFormula() {
    clear();
    return;
  }

  void clear();


  ZSearchFormula& _copyFrom(const ZSearchFormula& pIn) ;

  ZSearchFormula& operator = (const ZSearchFormula& pIn) { return _copyFrom(pIn);}

  ZSearchFormula* getNext()
  {
    return NextFormula;
  }

  void setType(ZFormulaType pType) {FormulaType=pType;}

//  bool _evaluate(const ZDataBuffer& pRecord);

  utf8VaryingString _report(int pLevel=0);


  ZSearchOperator       LeadingOperator;    /* Leading link operator Allowed operators : <nothing> AND OR (NOT combined with previous) -
                                             link with following formula after evaluation */
  ZFormulaType          FormulaType=ZFORT_Term ;    /* 1 : ZSearchTerm (logical op) - 2 : ZSearchFormula -3 : ZFORT_Arithmetic (arithmetic op) */
  void*                 TermOrFormula=nullptr;
  ZSearchFormula*       NextFormula=nullptr;
  int                   ParenthesisLevel=0;
};


class ZSearchFormulaList : public ZArray <ZSearchFormula>
{
public:
};


const char* decode_ZFormulaType(ZFormulaType pType);

} // namespace zbs
#endif // ZSEARCHFORMULA_H
