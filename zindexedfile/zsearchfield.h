#ifndef ZSEARCHFIELD_H
#define ZSEARCHFIELD_H


#include <ztoolset/zstatus.h>
#include <zcontentcommon/zcontentconstants.h>  // for ZCellFormat

#include "zfielddescription.h"
#include "zmetadic.h"
#include "zsearcharithmeticterm.h"

#include "zsearchoperand.h"


class ZSearchField //: public ZFieldDescription
{
public:
    ZSearchField() ;
    ZSearchField(std::shared_ptr<ZSearchEntity> pSourceEntity,const ZMetaDic* pMetaDic,long pDicRank);
    ZSearchField(const ZSearchField& pIn) {_copyFrom(pIn);}

    ~ZSearchField();


    void clear();

    ZStatus setField(std::shared_ptr<ZSearchEntity> pEntity, const ZMetaDic *pMDic, long pMDicRank);

    ZSearchField& _copyFrom(const ZSearchField& pIn);

    ZSearchField& operator = (const ZSearchField& pIn) {return _copyFrom(pIn);}

    ZCFMT_Type getCellFormat() {return CellFormat;}
    void setCellFormat(ZCFMT_Type pCellFormat) {CellFormat=pCellFormat;}

    bool isNull() { return Operand==nullptr ; }

    bool isField()
    {
        if ( Operand==nullptr )
            return false;

        return (static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Field);

    }
    bool isPureField() const
    {
        if (Operand==nullptr)
            return false;

        return ((static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Field)&&
                (!static_cast<ZSearchOperandBase*>(Operand)->hasModifier()));

    }
    bool isField() const
    {
        if (Operand==nullptr)
            return false;

        return (static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Field);

    }
    bool isLiteral() const
    {
        if (Operand==nullptr)
            return false;

        return (static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Literal);

    }
    std::shared_ptr<ZSearchEntity> getEntity() ;

/* Parser is no more located at entity level but within entity context
 *
    URFParser* getURFParser() ;
 */


    bool isExpression() const
    {
        if (Operand==nullptr)
            return false;

        return (static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Arithmetic)
               || (static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Logical);

    }

    bool hasModifier() const
    {
        if (Operand==nullptr)
            return false;
        return (static_cast<ZSearchOperandBase*>(Operand)->hasModifier());

    }

    utf8VaryingString _reportBrief(int pLevel)
    {
        return OperandReportFormula(Operand,false);
    }
    utf8VaryingString _reportFormula(int pLevel)
    {
        return OperandReportFormula(Operand,true);
    }

    ZSearchOperandType_type getOperandZSTO() ;

    long                    getMetaDicRank() const;
    const ZMetaDic*         getMetaDicPtr() const ;
    const utf8VaryingString getFieldName() const ;
    ZTypeBase getZType() const;
    const ZFieldDescription getDicFullDescription() const ;

    ZFieldDescription buildFieldDescription() const;

    const utf8VaryingString getOperandName() const
    {
        if (Operand==nullptr)
            return utf8VaryingString();
        ZSearchOperandBase* wOpBase =static_cast<ZSearchOperandBase*> (Operand);

        return wOpBase->FullFieldName;
    }

    void setOperand(void* pArithmeticTerm)
    {
        Operand = pArithmeticTerm ;
    }

    ZCFMT_Type                  CellFormat=ZCFMT_Nothing;
    void*                       Operand=nullptr ;
};

#endif // ZSEARCHFIELD_H
