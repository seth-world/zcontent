#ifndef ZCONTENTCONSTANTS_H
#define ZCONTENTCONSTANTS_H
#include <stdint.h>

const int cst_filename_max = 255;


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
