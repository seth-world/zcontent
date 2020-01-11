#include <zcontentcommon/zresult.h>

const char * decode_ZSA_Action (ZSA_Action &ZS)
{
    switch (ZS)
    {
    case ZSA_NoAction :
    {
        return ("ZSA_NoAction");
    }
    case ZSA_Error :
    {
        return ("ZSA_Push");
    }
    case  ZSA_Push_Front  :
    {
        return ("ZSA_Push_Front");
    }
    case  ZSA_Push  :
    {
        return ("ZSA_Push");
    }
    case ZSA_Insert :
    {
        return ("ZSA_Insert");
    }
    case  ZSA_Remove :
    {
        return ("ZSA_Remove");
    }
    default :
    {
        return ("ZSA_UNKNOWN");
    }

    }// switch
}
