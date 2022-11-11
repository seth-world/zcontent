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
    ZMFaddress=pIn.ZMFaddress;
    Operation=pIn.Operation;
    KeyContent=pIn.KeyContent;
    this->ZDataBuffer::_copyFrom(pIn);
    return *this;
  }
  ZIndexItem& operator = (const ZIndexItem& pIn) { return _copyFrom(pIn); }

  zaddress_type ZMFaddress;    //!< Master file block record address to link index key with
  ZOp           Operation;     //!< this is NOT stored on index file (see toFileKey() method) but only for history & journaling purpose
  ZDataBuffer   KeyContent;    //!< extracted key content from user record according key extraction rules. Size of content is fixed and value is ZIndexControlBlock::KeySize.

  void set(const ZDataBuffer& pKeyContent) ;
  void clear (void) {KeyContent.clearData(); ZMFaddress=0L; Operation = ZO_Nothing;
    //State = ZAMNothing;
    return;}
  ZDataBuffer& toFileKey(void);
  ZIndexItem&  fromFileKey (ZDataBuffer &pFileKey);

};




class ZIndexItemList : public ZArray<ZIndexItem*>
{
  typedef ZArray<ZIndexItem*> _Base;
public:
  ZIndexItemList() {}
  ~ZIndexItemList(void)
  {
    clear();
  }

  void clear(void)
  {
    while (size()>0)
      delete popR();

    _Base::reset();
  }
};

} // namespace zbs

#endif // ZSINDEXITEM_H
