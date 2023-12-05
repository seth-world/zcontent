#include "zsearchfield.h"
#include "zdataconversion.h"

#include "zsearchentity.h"


ZSearchField::ZSearchField() {}

ZSearchField::ZSearchField(std::shared_ptr<ZSearchEntity> pSourceEntity,
                           const ZMetaDic *pMetaDic,
                           long pDicRank)
{
    Operand = new ZSearchFieldOperand(pSourceEntity, pMetaDic, pDicRank);
}

ZSearchField::~ZSearchField()
{
    clearOperand(Operand);
}

ZSearchOperandType_type
ZSearchField::getOperandZSTO()
{
    if (Operand==nullptr)
        return ZSTO_Nothing;

    return static_cast<ZSearchOperandBase*>(Operand)->OperandZSTO ;

}

ZStatus
ZSearchField::setField(std::shared_ptr<ZSearchEntity> pEntity,const ZMetaDic*   pMDic,long pMDicRank)
{
    ZSearchOperandType_type wZSTOTp=getZSTOFromZType(pMDic->TabConst(pMDicRank).ZType);

    Operand = new ZSearchFieldOperand(pEntity,pMDic,pMDicRank);

//    wOpFld->MDicRank = pMDicRank;
//    wOpFld->MDic = pMDic;
 //   wOpFld->FieldDescription = &pMDic->TabConst(pMDicRank);

//    wOpFld->ZSTO = ZSearchOperandType(wZSTOTp | ZSearchOperandType_type(ZSTO_Field));
    return ZS_SUCCESS;
}

ZSearchField&
ZSearchField::_copyFrom(const ZSearchField& pIn)
{
    _copyOperand(Operand,pIn.Operand);
    CellFormat = pIn.CellFormat;
    return *this;
}
void ZSearchField::clear()
{
    clearOperand(Operand);
    CellFormat=ZCFMT_Nothing;
//    MetaDicRank=-1;
//    MetaDicPtr=nullptr;
}

long ZSearchField::getMetaDicRank() const
{
    if (Operand==nullptr)
        return -1;
    if (!(static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Field))
        return -1;
    return static_cast<ZSearchFieldOperand*>(Operand)->MetaDicRank;
}
const ZMetaDic *ZSearchField::getMetaDicPtr() const
{
    if (Operand==nullptr)
        return nullptr;
    if (!(static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Field))
        return nullptr;
    return static_cast<ZSearchFieldOperand*>(Operand)->MetaDicPtr;
}
const utf8VaryingString
ZSearchField::getFieldName() const
{
    if (Operand==nullptr)
        return utf8VaryingString();

    ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(Operand);
    return wOpBase->FullFieldName ;
}

ZTypeBase
ZSearchField::getZType() const
{
    if (Operand==nullptr)
        return ZType_Nothing;
    if (!(static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Field))
        return ZType_Nothing;
    long wR=static_cast<ZSearchFieldOperand*>(Operand)->MetaDicRank;
    return static_cast<ZSearchFieldOperand*>(Operand)->MetaDicPtr->TabConst(wR).ZType;
}

const ZFieldDescription ZSearchField::getDicFullDescription() const
{
    if (Operand==nullptr)
        return ZFieldDescription();
    if (!(static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Field))
        return ZFieldDescription();
    long wR=static_cast<ZSearchFieldOperand*>(Operand)->MetaDicRank;
    return static_cast<ZSearchFieldOperand*>(Operand)->MetaDicPtr->TabConst(wR);
}

/* builds a ZFieldDescription from source data  */

ZFieldDescription ZSearchField::buildFieldDescription() const
{
    if (Operand==nullptr)
        return ZFieldDescription();

    ZFieldDescription wFieldDescription;
    ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(Operand);

    wFieldDescription.setFieldName( wOpBase->FullFieldName) ;

    if (isPureField()) {
        long wR=static_cast<ZSearchFieldOperand*>(Operand)->MetaDicRank;
        return static_cast<ZSearchFieldOperand*>(Operand)->MetaDicPtr->TabConst(wR);
    }

    ZSearchOperandType_type wZSTO = wOpBase->ZSTO & ZSTO_BaseMask ;
    wFieldDescription.ZType = ZTypefromZSTO(wZSTO);
    if (hasModifier()) {
        switch (wOpBase->ModifierType)
        {
        case ZSRCH_SUBSTRING:
        case ZSRCH_SUBSTRINGLEFT:
        case ZSRCH_SUBSTRINGRIGHT:
            wFieldDescription.ZType = ZType_Utf8VaryingString;
            break;
        case ZSRCH_YEAR:
        case ZSRCH_MONTH:
        case ZSRCH_DAY:
        case ZSRCH_HOUR:
        case ZSRCH_MIN:
        case ZSRCH_SEC:
            wFieldDescription.ZType = ZType_U32;
            break;
        case ZSRCH_ZENTITY:
            wFieldDescription.ZType = ZType_U32;
            break;
        case ZSRCH_ID:
            wFieldDescription.ZType = ZType_U32;
            break;
        case ZSRCH_PATH:
        case ZSRCH_EXTENSION:
        case ZSRCH_BASENAME:
        case ZSRCH_ROOTNAME:
            wFieldDescription.ZType = ZType_Utf8VaryingString;
            break;
        }
    } // hasModifier

    wFieldDescription.Capacity = 0 ;
    wFieldDescription.KeyEligible = false ;

    wFieldDescription.HeaderSize = getURFHeaderSize(wFieldDescription.ZType);
    wFieldDescription.UniversalSize = getAtomicUniversalSize(wFieldDescription.ZType);
    wFieldDescription.NaturalSize = getAtomicNaturalSize(wFieldDescription.ZType);

    return wFieldDescription;
} // ZSearchField::createFieldDescription

/*
URFParser*
ZSearchField::getURFParser() {
    if (!isField())
        return nullptr;
    return &(static_cast<ZSearchFieldOperand*>(Operand)->Entity->_URFParser);
}
*/

std::shared_ptr<ZSearchEntity>
ZSearchField::getEntity() {
    if (!isField())
        return nullptr;
    return static_cast<ZSearchFieldOperand*>(Operand)->Entity;
}
