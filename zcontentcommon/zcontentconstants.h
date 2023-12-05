#ifndef ZCONTENTCONSTANTS_H
#define ZCONTENTCONSTANTS_H
#include <stdint.h>
#include <ztoolset/utfvaryingstring.h>
const int cst_filename_max = 255;

extern long cst_StringDisplayMax ;
void setStringMaxDislay (long pMax);


namespace zbs {

//#ifndef __CELL_FORMAT__
//#define __CELL_FORMAT__

/* Cell format code : applies to selected table column */
typedef int ZCFMT_Type;
enum ZCellFormat : ZCFMT_Type {
    ZCFMT_Nothing =         0,  /* default */

    ZCFMT_NumMask =     0x000F,
    ZCFMT_NumHexa =     0x0001, /* show numeric fields in hexa  (default is standard numeric representation )*/

    ZCFMT_DateMask=     0x0F00,

    ZCFMT_DMY     =     0x0100,   /* day/month/year only */
    ZCFMT_MDY     =     0x0200,   /* month/day/year only */
    ZCFMT_DMYHMS  =     0x0300,   /* day/month/year-hh:mm:ss */
    ZCFMT_MDYHMS  =     0x0400,   /* month/day/year-hh:mm:ss */

    ZCFMT_DLocale =     0x0500,   /* locale date format */
    ZCFMT_DUTC    =     0x0600,   /* UTC format */

    ZCFMT_ResMask =   0x0F0000,

    ZCFMT_ResSymb =   0x010000, /* Show symbol name in place of numeric value for ZEntities */
    ZCFMT_ResStd  =   0x020000, /* Resource numeric values are expressed in standard numeric representation (default is hexa)*/


    ZCFMT_PrefZType=  0x200000, /* prefix displayed formatted content with explicit ZType name */

    ZCFMT_DumpBlob= 0x01000000, /* switch display to <blob content> to a byte dump up to max 64 bytes */

    ZCFMT_ApplyAll=   0x100000  /* apply to all : only used by cell format dialog */

};

utf8VaryingString decode_ZCellFormat(ZCFMT_Type pZCFMT);

//#endif // __CELL_FORMAT__
} // namespace zbs

/**
 * @brief The ZSort_Type enum Gives the type of access to index : with duplicate key values or without duplicates
 */
enum ZSort_Type: uint8_t
{
    ZST_Nothing      = 0,
    ZST_NoDuplicates = 1,  //!< No duplicate allowed
    ZST_Duplicates   = 2   //!< Duplicates are allowed
};

const char* decode_ZST(ZSort_Type pZST);
ZSort_Type  encode_ZST(const char* pZST);

#endif // ZCONTENTCONSTANTS_H
