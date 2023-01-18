#ifndef ZSINDEXITEM_H
#define ZSINDEXITEM_H

#include <ztoolset/zdatabuffer.h>
#include <zindexedfile/zindextype.h>
#include <zcontentcommon/zoperation.h>


namespace zbs {

/**
 @brief The ZIndexItem class  Main Input/Ouput for indexes. It relates to ZRecord.
The key effective content as it will be stored using ZIndexFile object (and not its Fields structure definition).

*/
class ZIndexItem : public ZDataBuffer
{
public:
  ZIndexItem();
  ~ZIndexItem() {}

  ZIndexItem(const ZIndexItem& pIn): ZDataBuffer(pIn) {_copyFrom(pIn);}
  ZIndexItem(const ZDataBuffer& pKeyValue):ZDataBuffer(pKeyValue) {}
  ZIndexItem& _copyFrom(const ZIndexItem& pIn)
  {
    ZMFAddress=pIn.ZMFAddress;
    Operation=pIn.Operation;
    IndexRank = pIn.IndexRank;
    IndexAddress = pIn.IndexAddress ;
//    KeyContent=pIn.KeyContent;
    this->ZDataBuffer::_copyFrom(pIn);
    return *this;
  }
  ZIndexItem& operator = (const ZIndexItem& pIn) { return _copyFrom(pIn); }

  zaddress_type ZMFAddress;     //!< Master file block record address to link index key with
  zaddress_type IndexAddress;   //!< Index record address : to be stored in ZMF record
  ZOp_type      Operation;      //!< this is NOT stored on index file (see toFileKey() method) but only for history & journaling purpose
  zrank_type    IndexRank;      //!< index rank for the on going operation : set by prepare operation and used in commit rollback and hardrollback operations

  void setBuffer(const ZDataBuffer& pKeyContent) ;
  void clear (void) {
    ZDataBuffer::clearData();
    ZMFAddress=-1L;
    Operation = ZO_Nothing;
    IndexAddress=-1L;
    IndexRank=0L;
    //State = ZAMNothing;
    return;
  }

  utf8VaryingString display();

  ZDataBuffer   toFileKey(void);
  ZIndexItem&   fromFileKey (ZDataBuffer &pKeyFileRecord);

};



/* NB: Index item list must be exported to be saved in journalling base */
class ZIndexItemList : public ZArray<ZIndexItem*>
{
  typedef ZArray<ZIndexItem*> _Base;
public:
  ZIndexItemList() {}
  ~ZIndexItemList(void)
  {
    clear();
  }

  size_t _exportAppend(ZDataBuffer& pOut);

  void clear(void)
  {
    while (size()>0)
      delete popR();

    _Base::reset();
  }
};

#pragma pack(push)
#pragma pack(1)         // memory alignment on byte
class ZIIExport {
public:
  ZIIExport()=default;
  ~ZIIExport() {}
  ZIIExport(const ZIIExport& pIn) {_copyFrom(pIn);}

  uint32_t      EndianCheck = cst_EndianCheck_Normal;
  zaddress_type ZMFaddress;     //!< Master file block record address to link index key with
  zaddress_type IndexAddress;   //!< Index record address : to be stored in ZMF record
  ZOp_type      Operation;      //!< this is NOT stored on index file (see toFileKey() method) but only for history & journaling purpose
  zrank_type    IndexRank;      //!< index rank for the on going operation : set by prepare operation and used in commit rollback and hardrollback operations

  ZIIExport& _copyFrom(const ZIIExport& pIn);

  ZIIExport& operator= (const ZIIExport& pIn) {return _copyFrom(pIn);}

  bool isReversed () {return EndianCheck==cst_EndianCheck_Reversed;}
  void set(const ZIndexItem& pIn);
  void reverse();
  void serialize() {
    if (isReversed())
      return;
    reverse();
  }
  void deserialize() {
    if (!isReversed())
      return;
    reverse();
  }
};
#pragma pack(pop) // end no memory alignment

} // namespace zbs

#endif // ZSINDEXITEM_H
