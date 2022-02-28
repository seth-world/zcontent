#ifndef ZSINDEXITEM_H
#define ZSINDEXITEM_H

#include <ztoolset/zdatabuffer.h>
#include <zindexedfile/zsindextype.h>
#include <zcontentcommon/zoperation.h>


namespace zbs {

/**
 @brief The ZIndexItem class  Main Input/Ouput for indexes. It relates to ZRecord.
The key effective content as it will be stored using ZIndexFile object (and not its Fields structure definition).

*/
class ZSIndexItem : public ZDataBuffer
{
public:
  ZSIndexItem();
  ~ZSIndexItem() {}
  ZSIndexItem(const ZSIndexItem& pIn) {_copyFrom(pIn);}
  ZSIndexItem& _copyFrom(const ZSIndexItem& pIn)
  {
    ZMFaddress=pIn.ZMFaddress;
    Operation=pIn.Operation;
    KeyContent=pIn.KeyContent;
    this->ZDataBuffer::_copyFrom(pIn);
    return *this;
  }
  ZSIndexItem& operator = (const ZSIndexItem& pIn) { return _copyFrom(pIn); }

  zaddress_type ZMFaddress;    //!< Master file block record address to link index key with
  ZOp           Operation;     //!< this is NOT stored on index file (see toFileKey() method) but only for history & journaling purpose
  ZDataBuffer   KeyContent;    //!< extracted key content from user record according key extraction rules. Size of content is fixed and value is ZIndexControlBlock::KeySize.


  void clear (void) {KeyContent.clearData(); ZMFaddress=0L; Operation = ZO_Nothing;
    //State = ZAMNothing;
    return;}
  ZDataBuffer& toFileKey(void);
  ZSIndexItem&  fromFileKey (ZDataBuffer &pFileKey);

};




class ZSIndexItemList : public ZArray<ZSIndexItem*>
{
  typedef ZArray<ZSIndexItem*> _Base;
public:
  ZSIndexItemList() {}
  ~ZSIndexItemList(void)
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
