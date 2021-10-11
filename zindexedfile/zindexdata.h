#ifndef ZINDEXDATA_H
#define ZINDEXDATA_H

#include <stdint.h>
#include <ztoolset/zutfstrings.h>
#include <zrandomfile/zfilecontrolblock.h>

/* IndexPresence values meaning
   * 0 : Index to be deleted
   * 1 : Index present but not to be rebuilt
   * 2 : Index to be built or rebuilt -
   * 3 : Index created (therefore rebuilt)
   * 4 : Index errored
   * 5 : missing and not created
   */
enum ZPRES : uint8_t
{
  ZPRES_Nothing       = 0,
  ZPRES_ToBeDeleted   = 1,  /**<  to be deleted : 1 */
  ZPRES_Unchanged     = 2,  /**<  present but not to be rebuilt - or not to be modified if not index : 2 */
  ZPRES_ToChange      = 0x13,  /**<  present to be built or rebuilt - or to be modified if not index : 3 */
  ZPRES_ToCreate      = 0x14,  /**<  created (therefore to be built ) : 4 */
  ZPRES_Errored       = 5,  /**<  errored  : 5 */
  ZPRES_MissNotCre    = 6,   /**<  missing but not to be created  : 6 */
  ZPRES_ToRebuild     =0x10
};


struct IndexData_st
{
  IndexData_st()=default;
  IndexData_st(const IndexData_st& pIn) {_copyFrom(pIn);}

  IndexData_st& _copyFrom(const IndexData_st& pIn)
  {
    Status = pIn.Status;
    IndexName = pIn.IndexName;
    Duplicates = pIn.Duplicates;
    KeyUniversalSize = pIn.KeyUniversalSize;
    FCB=pIn.FCB;
    return *this;
  }

  ZPRES       Status=ZPRES_Nothing;
  utf8String  IndexName;
  ZSort_Type  Duplicates=ZST_NODUPLICATES;
  uint32_t    KeyUniversalSize=0;
  FCBParams   FCB;
};

#endif // ZINDEXDATA_H
