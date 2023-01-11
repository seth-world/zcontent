#ifndef ZOPERATION_H
#define ZOPERATION_H

#include <stdint.h> // for uint32_t
typedef  uint32_t ZOp_type;
enum ZOp:  ZOp_type
{
    ZO_Nothing              =0,
    ZO_Add                  =0x01,
    ZO_Erase                =0x02,
    ZO_Replace              =0x04,
    ZO_Insert               =0x08,
    ZO_Swap                 =0x10,
    ZO_Push                 =0x11,
    ZO_Push_front           =0x15,
    ZO_Pop                  =0x22,
    ZO_Pop_front            =0x26,
//    ZO_Reset                =0xFF,

    ZO_Processed            =0x0100,
    ZO_RolledBack           =0x0200,
    ZO_HardRolledBack       =0x0600,
    ZO_OpMask               =0x00FF
    //----------ZRF add-ons------------------------
    //
 /*   ZO_Free                 =0x004200,
    ZO_Writeblock           =0x004400,
    ZO_Remove               =0x004800,

    //---------End ZRF add-ons----------------------

    ZO_RollBack             =0x00001000,
    ZO_RollBack_Insert      =ZO_RollBack|ZO_Insert,
    ZO_RollBack_Push        =ZO_RollBack|ZO_Push,
    ZO_RollBack_Replace     =ZO_RollBack|ZO_Replace,
    ZO_RollBack_Pop         =ZO_RollBack|ZO_Pop,
    ZO_RollBack_Erase       =ZO_RollBack|ZO_Erase,

    ZO_join                 =0x00010000,
    ZO_joinWithDefault      =0x00020000,
    ZO_joinNotFound         =0x00040000,
    ZO_Unique               =0x01000000,
    ZO_joinUnique           =0x01010000,
    ZO_joinUniqueWithDefault=0x01020000,
    ZO_joinUniqueNotFound   =0x01040000,


    ZO_Historized           =0x10000000
*/
};

//! @brief ZCommitStatus is returned by user's function if any defined (not nullptr). It indicates the behavior to be taken just after the call.
enum ZCommitStatus {
    ZCS_Nothing,
    ZCS_Success,          //! commit is OK
    ZSC_Skip,             //! journal element must be skipped without interrupting the commit process, next journal element will be processed
    ZCS_Error,            //! commit is not to be continued but control is given back to caller with error.  ZS_USERERROR status is then returned.
    ZCS_Fatal             //! commit is errored. Processing is to be interrupted immediately with signal (abort())
};



template <class _Type>
struct CUF_struct
{
    typedef  ZCommitStatus (*CUF) (const _Type&,ZOp,void *) ;  //! commit user function
};

class utf8VaryingString;

/* code located within zam.h */
utf8VaryingString decode_ZOperation(ZOp_type &pOp);

#endif // ZOPERATION_H
