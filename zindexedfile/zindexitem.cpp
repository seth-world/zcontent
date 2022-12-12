#include "zindexitem.h"

using namespace zbs;

ZIndexItem::ZIndexItem()
{

}
//---------ZIndexItem_struct routines-------------------------------------------------------
//
void
ZIndexItem::set(const ZDataBuffer& pKeyContent) {
  ZDataBuffer::setData(pKeyContent);
}

/**
 * @brief ZIndexItem_struct::toFileKey converts the memory content of the ZIndexItem Key to a flat stream of bytes to write on Index File
 * @note ZOp Operation field is not stored on Index File
 *
 * @return a reference to a ZDataBuffer containing the flat content of ZIndexItem being processed
 */
ZDataBuffer
ZIndexItem::toFileKey(void)
{
  ZDataBuffer wReturn;
  zaddress_type wAddress;
  wAddress = reverseByteOrder_Conditional<zaddress_type>(ZMFaddress);
  wReturn.setData(&wAddress,sizeof(zaddress_type));
  wReturn.appendData(*this);
  return  wReturn;
}

/**
 * @brief ZIndexItem_struct::fromFileKey loads a ZIndexItem_struct from an Index file record contained in a ZDataBuffer structure (pFileKey)
 * @note ZOp Operation field is not stored on Index File, and therefore is not subject to be loaded
 *
 * @param[in] pFileKey ZIndexFile record content to load into current ZIndexItem
 * @return a reference to current ZIndexItem being processed
 */
ZIndexItem&
ZIndexItem::fromFileKey (ZDataBuffer &pKeyFileRecord)
{
  size_t wOffset =0;
  size_t wSize;

  clear();
  memmove (&ZMFaddress,pKeyFileRecord.Data,sizeof(ZMFaddress));

  ZMFaddress = reverseByteOrder_Conditional<zaddress_type>(ZMFaddress);
  wOffset += sizeof(ZMFaddress);
  wSize = pKeyFileRecord.Size - sizeof(ZMFaddress) ;
  ZDataBuffer::setData(pKeyFileRecord.Data+wOffset,wSize);
  return *this;
}

