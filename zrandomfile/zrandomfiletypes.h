#ifndef ZRANDOMFILETYPES_H
#define ZRANDOMFILETYPES_H

#include <zindexedfile/zmf_limits.h>  // for __ZRF_VERSION__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <ztoolset/ztypetype.h>
#include <ztoolset/zexceptionmin.h>
#include <zcontentcommon/zlock.h>
#include <ztoolset/zmem.h>


/** @namespace zbs */

/**
 * @addtogroup ZRandomFileGroup
 * @{
 */

//! @macro  __HEADER_FILEEXTENSION__ preprocessor parameter for standard header file extension
#define __HEADER_FILEEXTENSION__ ".zrh"
//! @macro __ZINDEX_FILEEXTENSION__  preprocessor parameter for index content file extension
#define __ZINDEX_FILEEXTENSION__ ".zix"
//! @macro __ZINDEX_FILEEXTENSION__  preprocessor parameter for master dictionary content file extension
#define __ZDICTIONARY_FILEEXTENSION__ ".dic"
//! @macro  __MASTER_FILEEXTENSION__ preprocessor parameter for master file content file extension
#define __MASTER_FILEEXTENSION__ ".zmf"

typedef long      zrank_type;     //!< Block record rank data type NB: could be negative if unknown
typedef int64_t   zaddress_type;  //!< ZRandomFile address data type : nb : could be negative if unknown
typedef uint64_t  zsize_type;     //!< ZRandomFile size data type

typedef uint8_t   zlockmask_type ;    //!< refers to ZLockMask_type
typedef uint32_t  zmode_type ;        //!< refers to ZRFMode_type


#ifndef __ZOPENZRFPOOL__
#define __ZOPENZRFPOOL__
namespace zbs {
class ZRandomFile; }
class ZOpenZRFPool: public zbs::ZArray <zbs::ZRandomFile*>
{
public:
  void addOpenFile(zbs::ZRandomFile* pFileData)
  { push(pFileData);}
  ZStatus removeFileByObject(zbs::ZRandomFile*pZRF);
  ZStatus removeFileByFd(int pFd);
  void closeAll();

}; //ZOpenZRFPool

#ifndef ZRANDOMFILE_CPP
extern ZOpenZRFPool* ZRFPool;
#endif //ZRANDOMFILE_CPP*/
#endif  // __ZOPENZRFPOOL__

/** @cond Development
 * @brief The ZPMSCounterextern ZOpenZRFPool* ZRFPool;_type enum
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

const char* decode_BlockId(ZBlockID pBID);  /* see zblock.cpp for function instantiation */

/** @endcond */ // Development
/**
 * @brief The ZRFMode_type enum  open mode for ZRandomFile
 */
enum ZRFMode_type : zmode_type
{
    ZRF_Nothing          =   0,  //!< NOP
    ZRF_NotOpen          =0xFF00,//!< file not open
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
//    ZFT_ZMasterFile = 2,    //!< file is ZMasterFile
    ZFT_ZIndexFile  = 4,    //!< file is ZIndexFile
    ZFT_ZRawMasterFile=2,   //!< file is raw master file (keys are only defined by their universal size)
    ZFT_ZDicMasterFile=8,   //!< this file uses a dictionary
    ZFT_ZMasterFile = ZFT_ZDicMasterFile|ZFT_ZRawMasterFile,   //!< file is Structured Master File using a dictionary

    ZFT_DictionaryFile = 0x10,
    ZFT_Any         = 0xFF  //!< all file types allowed
};


/** @public */
/**
 * @brief The ZBlockState_type enum defines the state of the block in file
 */
enum ZBlockState_type : uint8_t
{
    ZBS_Nothing         = 0,    // NOP
    ZBS_Used            = 0x01, // block contains a data block
    ZBS_Free            = 0x02, // block is free
    ZBS_Deleted         = 0x10, // block has been deleted
    ZBS_BeingDeleted    = 0x12, // block has been marked for deletion but is still not deleted yet : it could be restored using a rollback operation
    ZBS_Allocated       = 0x04, // block in allocated from free blocks pool - an entry has been created in ZBAT . But still not allocated. It still could be freed if create operation is not committed
    ZBS_AllocFromDelete = 0x14, // block is allocated from a deleted block in free block pool

    ZBS_Control         = 0x20,  //  used for file reorganization

    ZBS_Split           = 0x40, //  checksplit has splitted this block with previous or next block

    ZBS_Invalid         = 0x80  // block has been declared invalid by ZRandomFile scanning function
} ;


ZStatus generateURIHeader(uriString pURIPath, uriString &pURIHeader);


//-----------Functions-----------------------
//
//const char * decode_ZStatus (ZStatus ZS);
const char * decode_ZBS (uint8_t pZBS);

const char * decode_ZRFMode (zmode_type pZRF);
zmode_type encode_ZRFMode (char* pZRF);


const char * decode_ZFile_type (uint8_t pType);
ZFile_type encode_ZFile_type (char * pType);

utf8VaryingString getVersionStr (unsigned long pVersion); // defined in zrandomfile.cpp
unsigned long getVersionNum (const utf8VaryingString& pVersion);// defined in zrandomfile.cpp


/** @}  */ //  ZRandomFileGroup

#endif // ZRANDOMFILETYPES_H
