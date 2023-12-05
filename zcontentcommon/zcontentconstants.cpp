#include "zcontentconstants.h"
#include <string.h>
#include <stdio.h>

namespace zbs {

utf8VaryingString decode_ZCellFormat(ZCFMT_Type pZCFMT)
{

    if (pZCFMT==ZCFMT_Nothing)
        return "ZCFMT_Nothing";
    utf8VaryingString wReturn;

    ZCFMT_Type wZCFMT = pZCFMT & ZCFMT_NumMask;
    if (wZCFMT == ZCFMT_NumHexa)
        wReturn.addConditionalOR("ZCFMT_NumHexa");

    wZCFMT = pZCFMT & ZCFMT_DateMask;
    if (wZCFMT == ZCFMT_DMY)
        wReturn.addConditionalOR("ZCFMT_DMY");
    if (wZCFMT == ZCFMT_MDY)
        wReturn.addConditionalOR("ZCFMT_MDY");
    if (wZCFMT == ZCFMT_DMYHMS)
        wReturn.addConditionalOR("ZCFMT_DMYHMS");
    if (wZCFMT == ZCFMT_MDYHMS)
        wReturn.addConditionalOR("ZCFMT_MDYHMS");
    if (wZCFMT == ZCFMT_DLocale)
        wReturn.addConditionalOR("ZCFMT_DLocale");
    if (wZCFMT == ZCFMT_DUTC)
        wReturn.addConditionalOR("ZCFMT_DUTC");

    wZCFMT = pZCFMT & ZCFMT_ResMask;
    if (wZCFMT == ZCFMT_ResSymb)
        wReturn.addConditionalOR("ZCFMT_ResSymb");
    if (wZCFMT == ZCFMT_ResStd)
        wReturn.addConditionalOR("ZCFMT_ResStd");


    if (pZCFMT & ZCFMT_PrefZType)
        wReturn.addConditionalOR("ZCFMT_PrefZType");

    if (pZCFMT & ZCFMT_DumpBlob)
        wReturn.addConditionalOR("ZCFMT_DumpBlob");
    if (pZCFMT & ZCFMT_ApplyAll)
        wReturn.addConditionalOR("ZCFMT_ApplyAll");
    return wReturn;
}
} // namespace zbs


long cst_StringDisplayMax = 64;
void setStringMaxDislay (long pMax)
{
    cst_StringDisplayMax=pMax;
}


const char*
decode_ZST(ZSort_Type pZST)
{
    switch (pZST)
    {
    case ZST_Duplicates :
    {
        return "ZST_Duplicates";
    }
    case ZST_NoDuplicates :
    {
        return "ZST_NoDuplicates";
    }
    case ZST_Nothing :
    {
        return "ZST_Nothing";
    }
    default:
    {
        return "Unknown ZSort_Type";
    }
    }//switch
}

ZSort_Type
encode_ZST(const char* pZST)
{
    if (pZST==nullptr) {
        fprintf(stderr,"encode_ZST-E-NULLPTR pZST is null\n");
        return ZST_Nothing;
    }
    if (strcasecmp(pZST,"ZST_DUPLICATES")==0)
        return ZST_Duplicates;
    if (strcasecmp(pZST,"ZST_NODUPLICATES")==0)
        return ZST_NoDuplicates;

    return ZST_Nothing;
}
