#ifndef ZRANDOMFILETYPES_H
#define ZRANDOMFILETYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ztoolset/ztypetype.h>
#include <ztoolset/zexceptionmin.h>
#include <zcontentcommon/zlock.h>

//#include <ztoolset/zlockmanager.h>


#define __ZRF_VERSION__  2000UL
#define __ZRF_XMLVERSION__  "\"2.00\""
//#define __ZRF_XMLVERSION_CONTROL__  (const utf8_t*)"2.00"

#define __ZRF_XMLVERSION_CONTROL__  "2.00"


/** @namespace zbs */

/**
 * @addtogroup ZRandomFileGroup
 * @{
 */

//! @macro  __HEADER_FILEEXTENSION__ preprocessor parameter for ZRF header file extension
#define __HEADER_FILEEXTENSION__ ".zrh"
//! @macro __ZINDEX_FILEEXTENSION__  preprocessor parameter for ZIF file extension
#define __ZINDEX_FILEEXTENSION__ ".zix"

typedef long            zrank_type;     //!< Block record rank data type NB: could be negative if unknown
typedef long long       zaddress_type;  //!< ZRandomFile address data type : nb : could be negative if unknown
typedef long long       zsize_type;     //!< ZRandomFile size data type

typedef uint8_t         zlockmask_type ;    //!< refers to ZLockMask_type
typedef uint32_t        zmode_type ;    //!< refers to ZRFMode_type

const uint32_t     cst_ZSTART = 0xF5F5F5F5;  //!< Begin marker of a data structure on file
const uint32_t     cst_ZEND   = 0xF0F0F0F0;  //!< End marker of a data structure on file

const uint8_t      cst_ZSTART_BYTE = 0xF5;


/** @cond Development
 * @brief The ZPMSCounter_type enum
 */
enum ZPMSCounter_type {
    ZPMS_Nothing =          0,
    ZPMS_BlockHeader =      1,
    ZPMS_User =             2,
    ZPMS_Field =            4,
    ZPMS_FileHeader =       0x10,
    ZPMS_FCB        =       0x11,
    ZPMS_Reserved   =       0x12,
    ZPMS_Other =            0xFF
};

enum ZBlockID : uint8_t
{
    ZBID_Nothing        = 0,
    ZBID_FileHeader     = 0x0F,
    ZBID_FCB            = 0x1F, //!< File Control Block
    ZBID_MCB            = 0x2F, //!< Master Control Block
    ZBID_ICB            = 0x3F, //!< Index Control Block
    ZBID_JCB            = 0x4F, //!< Journal Control Block
    ZBID_Data           = 0x10,
    ZBID_Index          = 0x20,
    ZBID_Master         = 0x30,
    ZBID_MDIC           = 0x80  //!< MetaDictionary

};

/** @endcond */ // Development
/**
 * @brief The ZRFMode_type enum  open mode for ZRandomFile
 */
enum ZRFMode_type : zmode_type
{
    ZRF_Nothing          =   0,  //!< NOP
    ZRF_Read_Only        =   1,  //!< file is open for read only
    ZRF_Write_Only       =   2,  //!< file is open  write only ( This value is not operational )
    ZRF_Delete_Only      =   4,  //!< file is open for read and delete mode

    ZRF_Exclusive        =   0x10,  /**< File is opened in exclusive mode (only valid for zopen instruction)

                                     mask 0x10 : when set, then any element of file header is not written to file nor read from file
                                     excepted at open time (forced read) or close (forced write)

                                     example 0x11 -> exclusive read only  */

    ZRF_ManualLock      =   0x20,   /**< by default, Lock management mode is Automatic unless ZRF_ManualLock is set to open Mode */

    ZRF_Modify      = ZRF_Read_Only | ZRF_Write_Only | ZRF_Delete_Only, //!< file is open for any operation

    ZRF_Write       = ZRF_Read_Only | ZRF_Write_Only , //!< file is open for read & write (either modify or add )

    ZRF_Delete      = ZRF_Read_Only | ZRF_Delete_Only, //!< file is open for read & delete

    ZRF_All         = ZRF_Modify
};
/**
 * @brief The ZFile_type enum stored in ZHeaderControlBlock and defines the file type ZRandomFile, ZMasterFile, etc...
 */
enum ZFile_type : uint8_t
{
    ZFT_Nothing     = 0,    //!< nop
    ZFT_ZRandomFile = 1,    //!< file is ZRandomFile
    ZFT_ZMasterFile = 2,    //!< file is ZMasterFile
    ZFT_ZIndexFile  = 4,    //!< file is ZIndexFile
    ZFT_ZSMasterFile = 8,   //!< file is Structured Master File
    ZFT_ZSIndexFile = 0x10  //!< file is Structured index File
};


/** @public */
/**
 * @brief The ZBlockState_type enum defines the state of the block in file
 */
enum ZBlockState_type : unsigned char
{
    ZBS_Nothing         = 0,    //!< NOP
    ZBS_Used            = 1,    //!< block contains a data block
    ZBS_Free            = 2,    //!< block is free
    ZBS_Deleted         = 0xFF, //!< block has been deleted
    ZBS_BeingDeleted    = 0x1F, //!< block has been marked for deletion but is still not deleted yet : it could be restored using a rollback operation
    ZBS_Control         = 4,    //!< control block reserved for future use
    ZBS_Allocated       = 8     //!< block in allocated from free blocks pool - an entry has been created in ZBAT . But still not allocated. It still could be freed if create operation is not committed
} ;



class ZBlock;
class ZBlockDescriptor;

#pragma pack(push)
#pragma pack(0)
class ZBlockHeader_Export
{
public:
    uint32_t            StartBlock=cst_ZSTART ;    // Start marker
    ZBlockID            BlockID=ZBID_Data;         // Block identification : always ZBID_Data here
    zsize_type          BlockSize;      // total size of the physical block, ZBH_Export size+user content size
    ZBlockState_type    State;          // state of the block see @ref ZBlockState_type
    zlockmask_type          Lock;           // relates to ZLockMask_type (zlockmanager.h)
    ZLock_Reason        LockReason;     // RFFU (zlockmanager.h)
    pid_t               Pid;            // process identification that locked the block
};
class ZBlockDescriptor_Export: public ZBlockHeader_Export
{
public:
    zaddress_type        Address;      // Offset from begining of file : Physical Address (starting 0)
};

#pragma pack(pop)

/**
 * @brief The ZBlockHeader class  : ZBlockHeader is heading any user data and stored in file before user record content as a header for each data block
 *  This header is effectively what is written on file before any user data record.
 *
 */
class ZBlockHeader
{
public:

//    uint32_t            StartBlock ;    //!< Start sign
//    ZBlockID            BlockID;        // Block identification
    zsize_type          BlockSize;      // total size of the physical block on file ,
    //                                       ZBlockHeader_Export plus user content size
    ZBlockState_type    State;          // state of the block see @ref ZBlockState_type
    zlockmask_type          Lock;           // relates to ZLockMask_type (zlockmanager.h)
    ZLock_Reason        LockReason;     // RFFU (zlockmanager.h)
    pid_t               Pid;            // process identification that locked the block


    ZBlockHeader(void) {clear(); return;}
    void clear(void) {memset(this,0,sizeof(ZBlockHeader)); return;}
    ZBlockHeader& operator = (const ZBlockHeader &pBlock) {memmove(this,&pBlock,sizeof(ZBlockHeader));  return *this;}
    ZBlockHeader& operator = (const ZBlockDescriptor& pBlockDescriptor) {memmove(this,&pBlockDescriptor,sizeof(ZBlockHeader)); return *this;}
    ZBlockHeader& operator = (const ZBlock& pBlock) {memmove(this,&pBlock,sizeof(ZBlockHeader));  return *this;}
/*    ZDataBuffer& _export(ZDataBuffer& pZDBExport)
    {
        ZBlockHeader wExport;
        wExport.StartBlock = _reverseByteOrder_T<int32_t>(StartBlock);
        wExport.BlockID= BlockID;   // unsigned char
        wExport.BlockSize=_reverseByteOrder_T<zsize_type>(BlockSize);
        wExport.State= State;   // unsigned char
        wExport.Lock=_reverseByteOrder_T<zlock_type>(Lock);
        wExport.LockReason=LockReason; // unsigned char
        wExport.Pid=_reverseByteOrder_T<pid_t>(Pid);
        pZDBExport.setData(&wExport,sizeof(wExport));
        return pZDBExport;
    }//_export
    ZBlockHeader& _import(unsigned char* pZDBImport_Ptr)
    {
        ZBlockHeader* pImport=(ZBlockHeader*)(pZDBImport_Ptr);
        StartBlock = _reverseByteOrder_T<int32_t>(pImport->StartBlock);
        BlockID= pImport->BlockID;   // unsigned char
        BlockSize=_reverseByteOrder_T<zsize_type>(pImport->BlockSize);
        State= pImport->State;   // unsigned char
        Lock=_reverseByteOrder_T<zlock_type>(pImport->Lock);
        LockReason=pImport->LockReason; // unsigned char
        Pid=_reverseByteOrder_T<pid_t>(pImport->Pid);
        return *this;
    }//_import*/
    static ZBlockHeader_Export& _exportConvert(ZBlockHeader& pIn,ZBlockHeader_Export* pOut)
    {
        pOut->StartBlock = cst_ZSTART;
//        pOut->BlockID= pIn.BlockID;   // uint8_t
        pOut->BlockID= ZBID_Data;   // uint8_t
        pOut->BlockSize = pIn.BlockSize ;
        pOut->BlockSize=reverseByteOrder_Conditional<zsize_type>(pIn.BlockSize);
        pOut->State= pIn.State;   // unsigned char
        pOut->Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn.Lock);
        pOut->LockReason=pIn.LockReason; // unsigned char
        pOut->Pid=reverseByteOrder_Conditional<pid_t>(pIn.Pid);

        return *pOut;
    }//_exportConvert

    static ZStatus _importConvert(ZBlockHeader& pOut,ZBlockHeader_Export* pIn)
    {
        _MODULEINIT_
        memset(&pOut,0,sizeof(ZBlockHeader));
        if ((pIn->StartBlock!=cst_ZSTART)||(pIn->BlockID!=ZBID_Data))
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                          ZS_INVBLOCKADDR,
                                          Severity_Severe,
                                          "Invalid block format: invalid marker  Startblock<%X> or BlockId<%X>",
                                          pIn->StartBlock,
                                          pIn->BlockID);
                    _RETURN_ ZS_INVBLOCKADDR;
                    }
//        pOut.StartBlock = pIn->StartBlock;
//        pOut.BlockID= pIn->BlockID;   // unsigned char
        pOut.BlockSize = pIn->BlockSize ;
        pOut.BlockSize=reverseByteOrder_Conditional<zsize_type>(pOut.BlockSize);
        pOut.State= pIn->State;   // unsigned char
        pOut.Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn->Lock);
        pOut.LockReason=pIn->LockReason; // unsigned char
        pOut.Pid=reverseByteOrder_Conditional<pid_t>(pIn->Pid);

        _RETURN_ ZS_SUCCESS;
    }//_importConvert
};

/**
 * @brief The ZBlockDescriptor class This class enhance ZBlockHeader with other fields (Address).
 * This is the main entry for describing file blocks within pools (ZBlockAccessTable, ZFreeBlockPool,...).
 */
class ZBlockDescriptor : public ZBlockHeader
{
public:
    zaddress_type        Address;                                       //!< Offset from begining of file : Physical Address

    ZBlockDescriptor(void) {ZBlockHeader::clear(); Address=0; return;}

    void clear(void) {memset(this,0,sizeof(ZBlockDescriptor));return;}

    ZBlockDescriptor & operator = (const ZBlockDescriptor &pBlock) {memmove(this,&pBlock,sizeof(ZBlockDescriptor));

                                                                    return *this;}
    ZBlockDescriptor& operator = (const ZBlockHeader& pBlockHeader) {memmove(this,&pBlockHeader,sizeof(ZBlockHeader));return *this;}
    ZBlockDescriptor& operator = (const ZBlock &pBlock) {memmove(this,&pBlock,sizeof(ZBlockHeader)); return *this;}
/*    ZDataBuffer& _export(ZDataBuffer& pZDBExport)
    {
        ZBlockHeader::_export(pZDBExport);
        size_t wAddress=_reverseByteOrder_T<zaddress_type>(Address);
        pZDBExport.appendData(&wAddress,sizeof(wAddress));
        return pZDBExport;
    }//_export
    ZBlockDescriptor& _import(unsigned char* pZDBImport_Ptr)
    {
        ZBlockDescriptor* pImport=(ZBlockDescriptor*)(pZDBImport_Ptr);
        ZBlockHeader::_import(pZDBImport_Ptr);
        Address=_reverseByteOrder_T<zaddress_type>(pImport->Address);;
        return *this;
    }//_import*/

    static ZBlockDescriptor_Export _exportConvert(ZBlockDescriptor& pIn,ZBlockDescriptor_Export* pOut)
    {
        memset(pOut,0,sizeof(ZBlockDescriptor_Export));
        pOut->StartBlock = cst_ZSTART;
        pOut->BlockID= ZBID_Data ;   // uint8_t
        pOut->BlockSize=reverseByteOrder_Conditional<zsize_type>(pIn.BlockSize);
        pOut->State= pIn.State;   // unsigned char
        pOut->Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn.Lock);
        pOut->LockReason=pIn.LockReason; // unsigned char
        pOut->Pid=reverseByteOrder_Conditional<pid_t>(pIn.Pid);
        // specific to ZBlockDescriptor
        pOut->Address=reverseByteOrder_Conditional<zaddress_type>(pIn.Address);

        return *pOut;
    }//_exportConvert

    static ZStatus _importConvert(ZBlockDescriptor& pOut,ZBlockDescriptor_Export* pIn)
    {
        _MODULEINIT_
        if ((pIn->StartBlock!=cst_ZSTART)||(pIn->BlockID!=ZBID_Data))
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                          ZS_INVBLOCKADDR,
                                          Severity_Severe,
                                          "Invalid block format: invalid marker  Startblock<%X> or BlockId<%X>",
                                          pIn->StartBlock,
                                          pIn->BlockID);
                    _RETURN_ ZS_INVBLOCKADDR;
                    }
        memset(&pOut,0,sizeof(ZBlockDescriptor));
//        pOut.StartBlock = _reverseByteOrder_T<int32_t>(pIn->StartBlock);
//        pOut.BlockID= pIn->BlockID;   // unsigned char
        pOut.BlockSize=reverseByteOrder_Conditional<zsize_type>(pIn->BlockSize);
        pOut.State= pIn->State;   // unsigned char
        pOut.Lock=reverseByteOrder_Conditional<zlockmask_type>(pIn->Lock);
        pOut.LockReason=pIn->LockReason; // unsigned char
        pOut.Pid=reverseByteOrder_Conditional<pid_t>(pIn->Pid);
        // specific to ZBlockDescriptor
        pOut.Address=reverseByteOrder_Conditional<zaddress_type>(pIn->Address);
        _RETURN_ ZS_SUCCESS;
    }//_importConvert


};

ZStatus generateURIHeader(uriString pURIPath, uriString &pURIHeader);

/**
 * @brief The ZBlockMin_struct struct Intermediate simplified structure to manage addresses and content size.
 */
struct ZBlockMin_struct
{
    zaddress_type Address;
    zsize_type   BlockSize;

    ZBlockMin_struct& operator = (ZBlockDescriptor &pBlock)
                {Address=pBlock.Address; BlockSize=pBlock.BlockSize; return *this;}
};



//-----------Functions-----------------------
//
//const char * decode_ZStatus (ZStatus ZS);
const char * decode_ZBS (ZBlockState_type pZBS);

const char * decode_ZRFMode (zmode_type pZRF);
zmode_type encode_ZRFMode (char* pZRF);


const char * decode_ZFile_type (ZFile_type pType);
ZFile_type encode_ZFile_type (char * pType);

/** @}  */ //  ZRandomFileGroup

#endif // ZRANDOMFILETYPES_H
