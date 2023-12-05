#ifndef ZRAWRECORD_CPP
#define ZRAWRECORD_CPP

#include "zrawrecord.h"

#include <zindexedfile/zmetadic.h>
#include <zindexedfile/zmastercontrolblock.h>
#include <ztoolset/zbitset.h>
#include <zindexedfile/zrawmasterfile.h>

#include <zindexedfile/zindexitem.h>

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
  for (long wi=0;wi< RawMasterFile->IndexTable.count();wi++)
    KeyValue.push(new ZIndexItem());
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
  KeyValue[pIdx]->setBuffer( pKeyContent );
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
 *
 *  cst_ZBLOCKSTART  (uint32_t) | Added by ZRandom Block routine :
 *  ZBID_Data         0x10      |
 *  ----------------
 *  |
 *  |  ------------------------
 *  |    FieldPresence (ZType_bitset)  (bitset in URF format)
 *  |  or
 *  |    ZType_bitsetFull (uint32_t)
 *  |  ------------------------
 *  |  Record raw content size     (uint64_t)
 *  |  ------------------------
 *  |  Record raw content (universal values (not URF))
 *  |  -------------------------
 *  --------------
 *  uint32_t number of keys
 *  --------------
 *  Key 0 content
 *    key canonical size  (uint32_t)
 *    key content (pure universal values (not URF) : values we can sort on )
 *  -------------
 *  Key 1 content
 *  -------------
 *  ...
 *  Key n content
 *  -------------
 *
 * cst_ZBLOCKEND  (uint32_t)
 *
 *
 * @return
 */

ZDataBuffer
ZRawRecord::prepareForWrite(ZDataBuffer& pContent)
{
  /* FieldPresence bitset is set by ZSMasterFile.
   * Raw master file does not use FieldPresence :
   *    all fields and FieldPresence have to be managed by application.
   */
//  RawContent.clear();

  RawContent.setData(pContent); // put user record data ( FieldPresence bitset + size uint64_t + record content )

  printf ("ZRawRecord::prepareForWrite put number of keys <%ld> at offset <%ld>\n",KeyValue.count(),RawContent.getByteSize());

  _exportAtomic<uint32_t>((uint32_t)KeyValue.count(),RawContent); /* number of keys to store */

  for (long wi=0;wi < KeyValue.count();wi++)
  {
    /* size of next key content */
    _exportAtomic<uint32_t>((uint32_t)KeyValue[wi]->Size,RawContent);

    /* key content */
    RawContent.appendData(*KeyValue[wi]);
  }

//  RawContent.appendData(&cst_ZBLOCKEND,sizeof(cst_ZBLOCKEND)); /* no need to indian convert */

  if (BaseParameters->VerboseFileEngine())
    {
    fprintf (stdout,"ZRawRecord::prepareForWrite-I  raw record prepared presence bitset <%s> # keys <%ld> - user record content <%ld> total raw size <%ld>\n",
        FieldPresence.isNull()?"null":"mentionned",
        KeyValue.count(),
        pContent.Size,
        RawContent.Size);
    }

  return RawContent;
}//prepareForWrite

ZStatus
ZRawRecord::getContentFromRaw(ZDataBuffer& pContent,ZDataBuffer& pRaw )
{
  /* FieldPresence bitset is set by ZSMasterFile.
   * Raw master file does not use FieldPresence :
   *    all fields and FieldPresence have to be managed by application.
   */
  ZStatus wSt=ZS_SUCCESS;
  ZTypeBase wType=0;
  const unsigned char* wPtrIn=pRaw.Data ;

/*  if (FieldPresence==nullptr)
    FieldPresence=new ZBitset;*/
  size_t wS=FieldPresence._importURF(wPtrIn);

  if (wS==0)
    {
    ZException.addToLast(" from ZRawRecord::getContentFromRaw");
    return wSt;
    }
    /* getting effective record content */

  uint64_t wRecordContentSize;

  _importAtomic<uint64_t>(wRecordContentSize,wPtrIn);

  if ((wRecordContentSize > __INVALID_SIZE__) ||(ssize_t(wRecordContentSize) < 0))
      {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_INVSIZE,
          Severity_Severe,
          "Raw record has an invalid record content size unsigned <%lld> signed <%lld> hexa <0x%X>.",wRecordContentSize,(ssize_t)wRecordContentSize,wRecordContentSize);
      return ZS_INVSIZE;
      }
  pContent.setData(wPtrIn,wRecordContentSize);

    /* getting all keys universal content */

  wPtrIn += wRecordContentSize;


  RawContent.appendData(Content);

  uint32_t wKeysCount=0,wKeySize=0;

  _importAtomic<uint32_t>(wKeysCount,wPtrIn);
  long wi=0;

  KeyValue.clear();

  while (wi < wKeysCount)
    {
    _importAtomic<uint32_t>(wKeySize,wPtrIn);
    if (wKeySize==cst_ZBLOCKEND)
      break;
    KeyValue.push(new ZIndexItem);
    KeyValue.last()->Operation = ZO_Nothing ;
    KeyValue.last()->ZMFAddress = 0L;
    KeyValue.last()->setData(wPtrIn,wKeySize);
    wPtrIn += wKeySize;
    wi++;
    }
  return ZS_SUCCESS;
}//getContentFromRaw



void
ZRawRecord::resetAll()
{
//  FieldPresence.clear();
  RawContent.reset();
  Content.reset();
  for (long wi=0;wi<KeyValue.count();wi++ )
    KeyValue[wi]->reset();
}//resetAll


void
ZRawRecord::setup()
{
  RawContent.reset();
  Content.reset();
  while (KeyValue.count())
    delete KeyValue.popR();
  for (long wi=0;wi< RawMasterFile->IndexTable.count();wi++ )
    KeyValue.push(new ZIndexItem);

 }//setup

/**
 * @brief ZRawRecord::getRawKeyContent gets from master file raw record being read from file (pRawRecordContent),
 *  the raw content of key number pKeyIdx. This content may be used to directly access corresponding index file.
 * @param pKeyIdx
 * @param pKeyContent
 * @param pRawRecordContent raw record buffer read from ZSMasterFile
 * @return a ZStatus
 * ZS_INVTYPE if bitset has not been loaded.
 */

ZStatus
ZRawRecord::getRawKeyContent(unsigned int pKeyIdx,ZDataBuffer& pKeyContent)
{
  const unsigned char* wPtrIn=RawContent.Data;
  const unsigned char* wPtrEnd = wPtrIn + RawContent.Size;
  ZStatus wSt;
  size_t wS= FieldPresence._importURF(wPtrIn);
  if(wS==0)
    {
    ZException.setMessage("ZRawRecord::getRawKeyContent",
        ZS_INVVALUE,
        Severity_Severe,
        " Cannot load Field presence bitset ");
    return ZS_INVTYPE;
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
