#ifndef ZRAWRECORD_CPP
#define ZRAWRECORD_CPP

#include "zrawrecord.h"

#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zsmastercontrolblock.h>
#include <ztoolset/zbitset.h>
#include <zindexedfile/zrawmasterfile.h>

using namespace zbs;

ZRawRecord::ZRawRecord(ZRawMasterFile *pFather)
{
  RawMasterFile=pFather;
  Rank=0;

/*  if (pMCB->MetaDic==nullptr)
      FieldPresence=nullptr;
    else
        FieldPresence=new ZBitset(pMCB->MetaDic->size());
*/
  for (long wi=0;wi< RawMasterFile->ZMCB.IndexCount;wi++)
    KeyValue.push(new ZSIndexItem());
}

ZRawRecord::~ZRawRecord()
{
  while (KeyValue.count())
    delete KeyValue.popR();
}



ZStatus
ZRawRecord::setRawKeyContent(long pIdx,const ZDataBuffer& pKeyContent)
{
  if (pIdx > KeyValue.count())
    return ZS_OUTBOUND;
  KeyValue[pIdx]->KeyContent = pKeyContent;
  return ZS_SUCCESS;
}


/**
 * @brief ZRawRecord::prepareForWrite  setup raw record content for being written to ZSMasterFile
 *
 *  Content : must hold the genuine record content
 *
 *  KeyValue[0..n]->KeyContent  Array that must hold the genuine key value (0 for primary key up to n )
 *
 *
 * FieldPresence bitset is set by ZSMasterFile.
 * Raw master file does not use FieldPresence :
 *  all fields are managed by application and therefore :
 *  - fieldPresence is set to nullptr
 *  - all fields are reputated to be present within record.
 *
 *
 * This setup is done using :
 *  ZRawRecord::Content : this ZDataBuffer contains record data in universal format
 *
 *  ZRawRecord::KeyValue [0..n] -> KeyContent : for each defined key, contains Key content as it will be written to index file.
 *
 * This routine returns a ZDataBuffer, ready to be written to main file, with the following structure :
 *  -------------
 *  FieldPresence (ZType_bitset)  (bitset in URF format)
 * or
 *  ZType_bitsetFull (uint32_t)
 *  ----------------
 *  Record content size     (uint64_t)
 *  -------------------
 *  Record raw content (universal values (not URF))
 *  ------------------
 *  Key 0 content
 *    key canonical size  (uint32_t)
 *    key content (pure universal values (not URF) : values we can sort on )
 *  -------------
 *  Key 1 content
 *  -------------
 *  ...
 *  Key n content
 *  -------------
 * @return
 */

ZDataBuffer
ZRawRecord::prepareForWrite()
{
  /* FieldPresence bitset is set by ZSMasterFile. Raw master file does not use FieldPresence :
   * all fields are managed by application and therefore :
   * FieldPresence is set to nullptr
   * All fields are reputated to be present within record.
   */
  if (FieldPresence!=nullptr)
    RawContent.setData(FieldPresence->_exportURF(Content));  // export bitset as first record element
  else
    RawContent.setInt(ZType_bitsetFull);  /* if bitset FieldPresence is omitted : all fields are reputated present */

  uint64_t wRecordSize = reverseByteOrder_Conditional<uint64_t> ((uint64_t)Content.Size);
  RawContent.appendData(&wRecordSize,sizeof(uint64_t));
  RawContent.appendData(Content);
  uint32_t wKeySize;
  /* number of keys to store */
  wKeySize= reverseByteOrder_Conditional<uint32_t> ((uint32_t)KeyValue.count());
  RawContent.appendData(&wKeySize,sizeof(uint32_t));
  for (long wi=0;wi < KeyValue.count();wi++)
  {
    /* size of next key content */
    wKeySize= reverseByteOrder_Conditional<uint32_t> ((uint32_t)KeyValue[wi]->KeyContent.Size);
    RawContent.appendData(&wKeySize,sizeof(uint32_t));
    /* key content */
    RawContent.appendData(KeyValue[wi]->KeyContent);
  }

  RawContent.appendData(&cst_ZEND,sizeof(cst_ZEND)); /* no need to indian convert */

  return RawContent;
}
/**
 * @brief ZRawRecord::getContentFromRaw process raw data (read from file) and extracts record content.
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
  uint64_t                      record content size

  ...                           RECORD EFFECTIVE CONTENT


  uint32_t                      Number of key contents

  uint32_t                      Key 0 size
  ...                           KEY 0 CONTENT

  uint32_t                      Key 1 size
    ...                         KEY 1 CONTENT

                                    ....

  uint32_t                      Key n size
    ...                         KEY n CONTENT


  uint32_t                      cst_ZEND  : end of record marker


 */

ZDataBuffer
ZRawRecord::getContentFromRaw()
{
  /* FieldPresence bitset is set by ZSMasterFile.
   * Raw master file does not use FieldPresence :
   * all fields are managed by application and therefore :
   * FieldPresence is set to nullptr
   * All fields are reputated to be present within record.
   * This first uint32_t is set to ZType_bitsetFull
   */

  uint32_t wType = *(uint32_t*)RawContent.Data;

  unsigned char* wPtrIn=RawContent.Data;

  if (wType == ZType_bitsetFull)
  {
    if (FieldPresence!=nullptr)
      delete FieldPresence;
    FieldPresence=nullptr;
    wPtrIn += sizeof(uint32_t);
  }
    else
    {
    if (FieldPresence==nullptr)
      FieldPresence=new ZBitset;
    FieldPresence->_importURF(wPtrIn);
    }

    /* getting effective record content */

    uint64_t wRecordContentSize;

    _importAtomic<uint64_t>(wRecordContentSize,wPtrIn);

    Content.setData(wPtrIn,wRecordContentSize);

    /* getting all keys universal content */

  RawContent.appendData(Content);

  uint32_t wKeysCount=0,wKeySize=0;

  _importAtomic<uint32_t>(wKeysCount,wPtrIn);
  long wi=0;

  KeyValue.clear();

  while (wi < wKeysCount)
    {
    _importAtomic<uint32_t>(wKeySize,wPtrIn);
    if (wKeySize==cst_ZEND)
      break;
    KeyValue.push(new ZSIndexItem);
    KeyValue.last()->Operation = ZO_Nothing ;
    KeyValue.last()->ZMFaddress = 0L;
    KeyValue.last()->KeyContent.setData(wPtrIn,wKeySize);
    wPtrIn += wKeySize;
    wi++;
    }
  return RawContent;
}//getContentFromRaw



void
ZRawRecord::resetAll()
{
  if (FieldPresence!=nullptr)
    FieldPresence->clear();
  RawContent.reset();
  Content.reset();
  for (long wi=0;wi<KeyValue.count();wi++ )
    KeyValue[wi]->KeyContent.reset();
}//resetAll


void
ZRawRecord::setup()
{
  RawContent.reset();
  Content.reset();
  while (KeyValue.count())
    delete KeyValue.popR();
  for (long wi=0;wi< RawMasterFile->ZMCB.IndexCount;wi++ )
    KeyValue.push(new ZSIndexItem);

 }//setup

/**
 * @brief ZRawRecord::getRawKeyContent gets from master file raw record being read from file (pRawRecordContent),
 *  the raw content of key number pKeyIdx. This content may be used to directly access corresponding index file.
 * @param pKeyIdx
 * @param pKeyContent
 * @param pRawRecordContent raw record buffer read from ZSMasterFile
 * @return a ZStatus
 */

ZStatus
ZRawRecord::getRawKeyContent(unsigned int pKeyIdx,ZDataBuffer& pKeyContent)
{
  unsigned char* wPtrIn=RawContent.Data;
  unsigned char* wPtrEnd = wPtrIn + RawContent.Size;

  if(FieldPresence->_importURF(wPtrIn)==nullptr)
  {
    ZException.setMessage("ZRawRecord::getRawKeyContent",
        ZS_INVVALUE,
        Severity_Severe,
        " Cannot load Field presence bitset ");
    return ZS_INVVALUE;
  }
  uint64_t wRecordSize;
  _importAtomic<uint64_t>(wRecordSize,wPtrIn);

  wPtrIn += wRecordSize;  /* skip record content to point to Key # 0 */
  int wi=0;
  uint32_t wKeySize;
  _importAtomic<uint32_t>(wKeySize,wPtrIn);
  while ((wi < pKeyIdx) && (wPtrIn < wPtrEnd))  /* search for key # pKeyIdx */
  {
    wPtrIn += wKeySize;
    _importAtomic<uint32_t>(wKeySize,wPtrIn);  /* nb: _importAtomic updates wPtrIn accordingly */
    wi++;
  }

  if (!(wPtrIn<wPtrEnd))
  {
    ZException.setMessage("ZRawRecord::getRawKeyContent",
        ZS_OUTBOUNDHIGH,
        Severity_Severe,
        " Out of boundaries for raw key value");
    return ZS_OUTBOUNDHIGH;
  }

  pKeyContent.reset();
  pKeyContent.setData(wPtrIn,(size_t)wKeySize);

  return ZS_SUCCESS;
}

ZRawRecord* generateRawRecord(ZRawMasterFile* pMasterFile)
{
  return new ZRawRecord (pMasterFile);
}



#endif // ZRAWRECORD_CPP
