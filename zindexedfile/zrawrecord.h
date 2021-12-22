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


  /** @brief prepareForWrite  prepares Record raw content from record data pContent for being written on file */
  ZDataBuffer prepareForWrite(ZDataBuffer &pContent);

  /**
 * @brief ZRawRecord::getContentFromRaw process raw data pRaw (read from file) and extracts record content into pContent.
 * Raw data :
 * if first uint_32_t is ZType_bitsetFull, then no bitset, all fields are reputed to be present,
 * no dictionary is available (pure ZRawMasterRecord)
 * else
 * a bitset is present and must be read.
 *
 *
                      Raw Record bulk structure on file :

  RawMasterFile                   Master File
      (no dictionary no presence)       with master dictionary


      uint32_t        ZType_bitsetFull                  ZType_bitset
                              \            [...]    Zbitset content
                               \                   /
                                \                 /
      uint64_t                  record content size

      ...                       RECORD EFFECTIVE CONTENT


      uint32_t                    Number of key contents

      uint32_t                      Key 0 size
      ...                           KEY 0 CONTENT

      uint32_t                      Key 1 size
      ...                           KEY 1 CONTENT

      ....

      uint32_t                      Key n size
      ...                           KEY n CONTENT


      uint32_t                      cst_ZEND  : end of record marker


                              */

  ZStatus getContentFromRaw(ZDataBuffer& pContent, ZDataBuffer &pRaw);

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

  ZBitset                   FieldPresence;
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
