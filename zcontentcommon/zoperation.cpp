#include <zcontentcommon/zoperation.h>
#include <ztoolset/utfvaryingstring.h>

/**
 * @brief DecodeZOperation   gives a const char string of max 25 chars mentioning the ZOperation given in parameter
 * @param pOp
 * @return              a constant string that names the given code
 */
utf8VaryingString decode_ZOperation(ZOp_type &pOp)
{
  utf8VaryingString wReturn;

  uint32_t wOp=pOp;

  if (wOp & ZO_Processed)
    wReturn = "ZO_Processed | " ;

  wOp &= ZO_OpMask;

  switch (wOp) {
    case    ZO_Nothing :
        wReturn += "ZO_Nothing" ;
        return wReturn;
    case     ZO_Add  :
      wReturn += "ZO_Add" ;
      return wReturn;
    case    ZO_Erase :
      wReturn += "ZO_Erase" ;
      return wReturn;
    case    ZO_Replace :
      wReturn += "ZO_Replace" ;
      return wReturn;
    case    ZO_Insert  :
      wReturn += "ZO_Insert" ;
      return wReturn;
    case    ZO_Swap :
      wReturn += "ZO_Swap" ;
      return wReturn;
      wReturn += "ZO_Push" ;
      return wReturn;
    case    ZO_Push_front :
      wReturn += "ZO_Push_front" ;
      return wReturn;
    case    ZO_Pop :
      wReturn += "ZO_Pop" ;
      return wReturn;
    case    ZO_Pop_front :
      wReturn += "ZO_Pop_front" ;
      return wReturn;

/*
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
*/
    default:
      wReturn += "Unknown operation" ;
      return wReturn;

    } // switch
}//decode_ZOperation

