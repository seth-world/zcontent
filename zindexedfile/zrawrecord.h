#ifndef ZRAWRECORD_H
#define ZRAWRECORD_H

#include <ztoolset/zstatus.h>
#include <ztoolset/zarray.h>
#include <ztoolset/zbitset.h>
#include <zindexedfile/zsindexitem.h>




namespace zbs {

class ZRawMasterFile;

class ZRawRecord
{
public:
  ZRawRecord(ZRawMasterFile *pFather);

  ~ZRawRecord();

  void setup();

  /** assigns a raw key value to index key rank pIdx. Raw key value must be prepared to be used : Universal values.*/
  ZStatus setRawKeyContent(long pIdx,const ZDataBuffer& pKeyContent);
  ZStatus getRawKeyContent(unsigned int pKeyIdx, ZDataBuffer& pKeyContent);


  /** @brief prepareForWrite  prepares Record raw content from record data (record content, keys and field presence) for being written on file */
  ZDataBuffer prepareForWrite();

  /** @brief getContentFromRaw after a raw read, updates record content, keys  and field presence (if necessary) from raw */
  ZDataBuffer getContentFromRaw();

  template<class _Entity>
  _Entity getEntityFromRaw()
  {
    return _Entity::getEntityFromRaw(Content);
  }

  template<class _Entity>
  ZStatus setContent(_Entity& pEntity)
  {
    return pEntity.setContent(RawContent,KeyValue);
  }


  /* reset all containers : raw record content, record content, keys content */
  void        resetAll();

  ZBitset*                  FieldPresence=nullptr;
  ZDataBuffer               RawContent;  /* what is written on the file : bitset, content, keys */
  ZDataBuffer               Content;     /* the record content */
  ZSIndexItemList           KeyValue;   /* key values to be written in RawContent and in index files */

  long                    Rank=0L;      /* ZBAT rank */
  zaddress_type           Address=0L;   /* logical address (used with ZRandomFile::zget) */

  ZRawMasterFile*         RawMasterFile=nullptr;
};

} // namespace zbs

ZRawRecord* generateRawRecord(ZRawMasterFile* pMasterFile);




#endif // ZRAWRECORD_H
