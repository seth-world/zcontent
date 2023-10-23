#include "zcontentconstants.h"
#include <string.h>
#include <stdio.h>

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
