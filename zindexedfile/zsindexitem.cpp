#include "zsindexitem.h"

using namespace zbs;

ZSIndexItem::ZSIndexItem()
{

}
//---------ZIndexItem_struct routines-------------------------------------------------------
//
/**
 * @brief ZIndexItem_struct::toFileKey converts the memory content of the ZIndexItem Key to a flat stream of bytes to write on Index File
 * @note ZOp Operation field is not stored on Index File
 *
 * @return a reference to a ZDataBuffer containing the flat content of ZIndexItem being processed
 */
ZDataBuffer&
ZSIndexItem::toFileKey(void)
{
  zaddress_type wAddress;
  wAddress = reverseByteOrder_Conditional<zaddress_type>(ZMFaddress);
  setData(&wAddress,sizeof(zaddress_type));
  appendData(KeyContent);
  return  (ZDataBuffer&)*this;
}

/**
 * @brief ZIndexItem_struct::fromFileKey loads a ZIndexItem_struct from an Index file record contained in a ZDataBuffer structure (pFileKey)
 * @note ZOp Operation field is not stored on Index File, and therefore is not subject to be loaded
 *
 * @param[in] pFileKey ZIndexFile record content to load into current ZIndexItem
 * @return a reference to current ZIndexItem being processed
 */
ZSIndexItem&
ZSIndexItem::fromFileKey (ZDataBuffer &pFileKey)
{
  size_t wOffset =0;
  size_t wSize;

  clear();
  memmove (&ZMFaddress,pFileKey.Data,sizeof(ZMFaddress));

  ZMFaddress = reverseByteOrder_Conditional<zaddress_type>(ZMFaddress);
  wOffset += sizeof(ZMFaddress);
  wSize = pFileKey.Size - sizeof(ZMFaddress) ;
  KeyContent.setData(pFileKey.Data+wOffset,wSize);
  return *this;
}

