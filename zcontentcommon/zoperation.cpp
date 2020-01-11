#include <zcontentcommon/zoperation.h>


/**
 * @brief DecodeZOperation   gives a const char string of max 25 chars mentioning the ZOperation given in parameter
 * @param pOp
 * @return              a constant string that names the given code
 */
const char* decode_ZOperation(ZOp &pOp)
{
    switch (pOp)
    {
    case    ZO_Nothing :
    {
        return ("ZO_Nothing");
    }
    case     ZO_Add  :
    {
        return ("ZO_Add");
    }
    case    ZO_Erase :
    {
        return ("ZO_Erase");
    }
    case    ZO_Replace :
    {
        return ("ZO_Replace");
    }
    case    ZO_Insert  :
    {
        return ("ZO_Insert");
    }
    case    ZO_Swap :
    {
        return ("ZO_Swap");
    }
    case    ZO_Push :
    {
        return ("ZO_Push");
    }
    case    ZO_Push_front :
    {
        return ("ZO_Push_front");
    }
    case    ZO_Pop :
    {
        return ("ZO_Pop");
    }
    case    ZO_Pop_front :
    {
        return ("ZO_Pop_front");
    }
    case    ZO_Reset :
    {
        return ("ZO_Reset");
    }
    case    ZO_Master :
    {
        return ("ZO_Master");
    }
    case    ZO_Slave :
    {
        return ("ZO_Slave");
    }
    case    ZO_join :
    {
        return ("ZO_join");
    }
    case    ZO_joinWithDefault :
    {
        return ("ZO_joinWithDefault");
    }
    case    ZO_joinNotFound:
    {
        return ("ZO_joinNotFound");
    }
    case    ZO_Unique :
    {
        return ("ZO_Unique");
    }
    case    ZO_joinUnique :
    {
        return ("ZO_joinUnique");
    }
    case    ZO_joinUniqueWithDefault :
    {
        return ("ZO_joinUniqueWithDefault");
    }
    case    ZO_joinUniqueNotFound :
    {
        return ("ZO_joinUniqueNotFound");
    }

        //!----------------ZRF Add-ons----------------
        //!
        //!
    case    ZO_Free :
    {
        return ("ZO_Free");
    }
    case    ZO_Writeblock :
    {
        return ("ZO_Writeblock");
    }
    case    ZO_Remove :
    {
        return ("ZO_Remove");
    }

        //!----------------End ZRF Add-ons----------------

    default:
    {
        return ("Unknown Zoperation");
    }
    } // switch
}//decode_ZOperation

