#ifndef ZOPERATION_H
#define ZOPERATION_H

#include <stdint.h> // for uint32_t

enum ZOp:  uint32_t
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
    ZO_Reset                =0xFF,


    //!----------ZRF add-ons------------------------
    //!
    ZO_Free                 =0x004100,
    ZO_Writeblock           =0x004200,
    ZO_Remove               =0x004400,

    //!---------End ZRF add-ons----------------------

    ZO_RollBack             =0x00001000,
    ZO_RollBack_Insert      =ZO_RollBack|ZO_Insert,
    ZO_RollBack_Push        =ZO_RollBack|ZO_Push,
    ZO_RollBack_Replace     =ZO_RollBack|ZO_Replace,
    ZO_RollBack_Pop         =ZO_RollBack|ZO_Pop,
    ZO_RollBack_Erase       =ZO_RollBack|ZO_Erase,

    ZO_Master               =0x00001000,
    ZO_Slave                =0x00002000,

    ZO_join                 =0x00010000,
    ZO_joinWithDefault      =0x00020000,
    ZO_joinNotFound         =0x00040000,
    ZO_Unique               =0x01000000,
    ZO_joinUnique           =0x01010000,
    ZO_joinUniqueWithDefault=0x01020000,
    ZO_joinUniqueNotFound   =0x01040000,


    ZO_Historized           =0x10000000
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


/* code located within zam.h */
const char *decode_ZOperation(ZOp &pOp);

#endif // ZOPERATION_H
