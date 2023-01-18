#include "zindexitem.h"

using namespace zbs;

ZIndexItem::ZIndexItem()
{
  Operation = ZO_Nothing;
  ZMFAddress = -1;
  IndexRank = -1;
}
//---------ZIndexItem_struct routines-------------------------------------------------------
//
void
ZIndexItem::setBuffer(const ZDataBuffer& pKeyContent) {
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
  wAddress = reverseByteOrder_Conditional<zaddress_type>(ZMFAddress);
  wReturn.setData(&wAddress,sizeof(zaddress_type));
  wReturn.appendData(*this);
  return  wReturn;
}

/**
 * @brief ZIndexItem_struct::fromFileKey loads a ZIndexItem_struct from an Index file record contained in a ZDataBuffer structure (pFileKey)
 *
 *  ZMFAddress is feeded with index record content
 *  key content is feeded with index record content
 *
 * @param[in] pFileKey ZIndexFile record content to load into current ZIndexItem
 * @return a reference to current ZIndexItem being processed
 */
ZIndexItem&
ZIndexItem::fromFileKey (ZDataBuffer &pKeyFileRecord)
{
//  size_t wOffset =0;
  size_t wSize;

  clear();
  memmove (&ZMFAddress,pKeyFileRecord.Data,sizeof(ZMFAddress));

  ZMFAddress = reverseByteOrder_Conditional<zaddress_type>(ZMFAddress);
//  wOffset += sizeof(ZMFaddress);
  wSize = pKeyFileRecord.Size - sizeof(ZMFAddress) ;
  ZDataBuffer::setData(pKeyFileRecord.Data + sizeof(ZMFAddress),wSize);
  return *this;
}

utf8VaryingString
ZIndexItem::display() {
  utf8VaryingString wReturn ;

  wReturn.sprintf("ZMFaddress %lld\n"
                  "IndexAddress %lld\n"
                  "IndexRank %lld\n"
                  "Operation %lld\n"
                  "Buffer size %ld\n" ,
      ZMFAddress,IndexAddress,IndexRank,decode_ZOperation(Operation).toCChar(),ZDataBuffer::Size
      );
  return wReturn;
}

size_t ZIndexItemList::_exportAppend(ZDataBuffer& pOut) {
  size_t wSize = sizeof(uint64_t) ;

  for (long wi=0;wi < count();wi++) {
    wSize += sizeof(ZIIExport) +  Tab[wi]->getURFSize() ;
  }

  unsigned char* wPtr=pOut.extend(wSize);

  _exportAtomicPtr<uint64_t>(uint64_t(wSize),wPtr);

  for (long wi=0;wi < count();wi++) {
    ZIIExport* wZIIe =  (ZIIExport*) wPtr;
    wZIIe->set(*Tab[wi]);
    wZIIe->serialize();

    wPtr += sizeof(ZIIExport) ;
    Tab[wi]->_exportURF_Ptr(wPtr);
  }// for

  return wSize;
}

ZIIExport& ZIIExport::_copyFrom(const ZIIExport& pIn) {
  ZMFaddress = pIn.ZMFaddress;
  IndexAddress = pIn.IndexAddress;
  Operation = pIn.Operation;
  IndexRank = pIn.IndexRank;
  EndianCheck = pIn.EndianCheck;
  return *this;
}

void ZIIExport::reverse() {
  EndianCheck = reverseByteOrder_Conditional(EndianCheck);
  ZMFaddress = reverseByteOrder_Conditional(ZMFaddress);
  IndexAddress = reverseByteOrder_Conditional(IndexAddress);
  Operation = reverseByteOrder_Conditional(Operation);
  IndexRank = reverseByteOrder_Conditional(IndexRank);
  return ;
}

void ZIIExport::set(const ZIndexItem& pIn) {
  ZMFaddress = pIn.ZMFAddress;
  IndexAddress = pIn.IndexAddress;
  Operation = pIn.Operation;
  IndexRank = pIn.IndexRank;
  EndianCheck = cst_EndianCheck_Normal;
}
