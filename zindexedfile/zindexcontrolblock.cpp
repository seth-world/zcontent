#include "zindexcontrolblock.h"
#include <zindexedfile/zmfdictionary.h>
#include <zxml/zxmlprimitives.h>

ZSICBOwnData& ZSICBOwnData::_copyFrom(const ZSICBOwnData &pIn)
{
//  ICBTotalSize=pIn.ICBTotalSize;
//  ZKDicOffset=pIn.ZKDicOffset;
 // KeyType=pIn.KeyType;
 // AutoRebuild=pIn.AutoRebuild;
  Duplicates=pIn.Duplicates;
  KeyUniversalSize=pIn.KeyUniversalSize;
  IndexName=pIn.IndexName;
  return *this;
}

void
ZSICB_Export::clear(void)
{
  BlockId=ZBID_ICB;
  ZMFVersion =  __ZMF_VERSION__ ;
  //    Name.clear();   // index name is not defined in ICB export structure : got from ICB memory structure and exported as Universal string
  StartSign=cst_ZBLOCKSTART;
  Duplicates = ZST_NODUPLICATES;
//  AutoRebuild = false;
  ICBTotalSize =0;
  KeyUniversalSize=0;
  ZKDicOffset=0;
  //    Name=nullptr;
  return;
}//clear



ZSICB_Export& ZSICB_Export::_copyFrom(const ZSICB_Export &pIn)
{
  StartSign=pIn.StartSign;
  BlockId=pIn.BlockId;
  ZMFVersion=pIn.ZMFVersion;
  ICBTotalSize=pIn.ICBTotalSize;
  ZKDicOffset=pIn.ZKDicOffset;
  KeyUniversalSize = pIn.KeyUniversalSize;
//  AutoRebuild=pIn.AutoRebuild;  /* uint8_t */
  Duplicates=pIn.Duplicates;    /* uint8_t */

  return *this;
}

ZSICB_Export& ZSICB_Export::set(const ZSICBOwnData* pIn)
{
  StartSign=cst_ZBLOCKSTART;
  BlockId=ZBID_ICB;
  ZMFVersion=__ZMF_VERSION__;
  /*                      ICB data size               Index name UVF size            cst_ZBLOCKEND size */
  ICBTotalSize = uint32_t(sizeof(ZSICB_Export) + pIn->IndexName._getexportUVFSize()+sizeof(uint32_t));

  ZKDicOffset=(int32_t)ICBTotalSize;
//  HasKeyDictionary=pIn->HasKeyDictionary; /* uint8_t from bool */
  KeyUniversalSize = pIn->KeyUniversalSize;
//  AutoRebuild=pIn->AutoRebuild;  /* uint8_t */
  Duplicates=pIn->Duplicates;      /* uint8_t */
  return *this;
}



void ZSICB_Export::_convert()
{
  if (!is_little_endian())
    return ;
  EndianCheck=reverseByteOrder_Conditional<uint16_t>(EndianCheck);

//  StartSign=reverseByteOrder_Conditional<uint32_t>(StartSign);
  ZMFVersion=reverseByteOrder_Conditional<uint32_t>(ZMFVersion);
  ICBTotalSize=reverseByteOrder_Conditional<uint32_t>(ICBTotalSize);
  ZKDicOffset=reverseByteOrder_Conditional<int32_t>(ZKDicOffset);
  KeyUniversalSize=reverseByteOrder_Conditional<uint32_t>(KeyUniversalSize);
/* other fields are uint8_t */

  return ;
}


void
ZSICB_Export::serialize()
{
  if (!is_little_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZSICB_Export::serialize-W-ALRDY File Control Block already serialized.\n");
    return;
  }
  _convert();
}

void
ZSICB_Export::deserialize()
{
  if (!is_little_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZSICB_Export::deserialize-W-ALRDY File Control Block already deserialized. \n");
    return;
  }
  _convert();
}



/*

ZDataBuffer&
ZSICBOwnData::_exportAppend(ZDataBuffer& pICBE)
{
  ZSICB_Export wICB;
  wICB.set(this);

 // wICB.ZKDicOffset = wICB.ICBTotalSize = sizeof(ZSICBOwnData_Export) +IndexName._getexportUVFSize();

  wICB._reverseConditional();
  pICBE.setData(&wICB,sizeof(ZSICB_Export));

  IndexName._exportAppendUVF(pICBE);

  _exportAtomic<uint32_t>(cst_ZBLOCKEND,pICBE);

  return pICBE;
}// ZSICBOwnData::_exportAppend
*/
ZStatus
ZSICBOwnData::_import(const unsigned char* &pPtrIn)
{
  ZSICB_Export wICBe;
  wICBe.setFromPtr(pPtrIn);

  clear();


  if ((wICBe.BlockId!=ZBID_ICB)||(wICBe.StartSign!=cst_ZBLOCKSTART))
    {
    ZException.setMessage("ZSICBOwnData::_import",
        ZS_BADICB,
        Severity_Severe,
        "Invalid Index Control Block : found Start marker <%X> ZBlockID <%X>. One of these is invalid (or both are).",
        wICBe.StartSign,
        wICBe.BlockId);
    return  ZS_BADICB;
    }
  wICBe.deserialize();
  if (wICBe.ZMFVersion != __ZMF_VERSION__)// HAS to be reversed
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEVERSION,
        Severity_Severe,
        "Error Index Control Block   : Found version <%ld> while current ZMF version is <%ld>",
        wICBe.ZMFVersion,
        __ZMF_VERSION__);
    return (ZS_BADFILEVERSION);
    }

  uint32_t wICBTotalSize=wICBe.ICBTotalSize;
  uint32_t wZKDicOffset=wICBe.ZKDicOffset;

  KeyUniversalSize=wICBe.KeyUniversalSize;

//  AutoRebuild=wICBOwn_Import->AutoRebuild;  // uint8_t
  Duplicates=wICBe.Duplicates; // uint8_t
//  KeyType=wICBOwn_Import->KeyType; // uint8_t

  pPtrIn+=sizeof(ZSICB_Export); // index Name is stored just after ZSICBOwnData_Export structure
      // format is Universal format with leading unit size and text size (uint16_t)

  IndexName._importUVF(pPtrIn);  // Name is stored after ZSICBOwnData_Export as a varying number of byte (uint16_t is leading string size)
  URIIndex._importUVF(pPtrIn);  // index file name is stored after indexname
  return  ZS_SUCCESS;
}// ZSICBOwnData::_importICBOwn

ZStatus
ZIndexControlBlock::_import(const unsigned char* &pPtrIn)
{
  ZSICB_Export wICBe;
  wICBe.setFromPtr( pPtrIn); /* pPtrIn is updated */

  if (wICBe.StartSign != cst_ZBLOCKSTART)    // not to be reversed : palyndroma
  {
    ZException.setMessage("ZSIndexControlBlock::_import",
        ZS_BADFILEHEADER,
        Severity_Severe,
        "Invalid Index Control Block header : found Start marker <%X>.",
        wICBe.StartSign);
    return  ZS_BADICB;
  }
  if (wICBe.BlockId != ZBID_ICB) // uint8_t :not to be reversed
  {
    ZException.setMessage("ZSIndexControlBlock::_import",
        ZS_BADICB,
        Severity_Severe,
        "Invalid Index Control Block header : found  ZBlockID <%X>.",
        wICBe.BlockId);
    return (ZS_BADICB);
  }

  wICBe.deserialize();
  if (wICBe.ZMFVersion != __ZMF_VERSION__)// HAS to be reversed
  {
    ZException.setMessage("ZSIndexControlBlock::_import",
        ZS_BADFILEVERSION,
        Severity_Error,
        "Error Index Control Block   : Found version <%ld> while current ZMF version is <%ld>",
        wICBe.ZMFVersion,
        __ZMF_VERSION__);
    return (ZS_BADFILEVERSION);
  }

  KeyUniversalSize=wICBe.KeyUniversalSize;

  //  AutoRebuild=wICBOwn_Import->AutoRebuild;  // uint8_t
  Duplicates=wICBe.Duplicates; // uint8_t
  //  KeyType=wICBOwn_Import->KeyType; // uint8_t

  pPtrIn+=sizeof(ZSICB_Export); // index Name is stored just after ZSICBOwnData_Export structure
      // format is Universal format with leading unit size and text size (uint16_t)

  IndexName._importUVF(pPtrIn);  // Name is stored after ZSICBOwnData_Export as a varying number of byte (uint16_t is leading string size)
                                  // NB: pPtrIn is updated
  URIIndex._importUVF(pPtrIn);  // index file name is stored after indexname


   /* ZIndexControlBlock has no key dictionary definition : key dictionary is stored with MetaDictionary */

  /* take care of key dictionary */

//  if (wICBe.ZKDicOffset == 0)
//  {
    /* see the case of a ZKDic is existing while it is declared by import as non existing */
//    return  ZS_SUCCESS;
//  }

  /* there is a key dictionary */
/*  unsigned char* wPtrIn= (unsigned char*)wICBE + wICBe.ZKDicOffset ;
  if (KeyDic)
    delete KeyDic;
  KeyDic=new ZSKeyDictionary(pMetaDic);
  KeyDic->_import(wPtrIn);
*/
/*
  if (ZKDic != nullptr)
    delete ZKDic;

  ZKDic=new ZSKeyDictionary(pMetaDic);

  ZKDic->_import(pPtrIn);

*/
  return  ZS_SUCCESS;
}// ZSIndexControlBlock::_importICBOwn



ZDataBuffer
ZIndexControlBlock::_export()
{
  ZDataBuffer     wZDB;
  return _exportAppend(wZDB);
}

/**
 * @brief ZIndexControlBlock::_exportICB exports ZIndexControlBlock content to a flat ZDataBuffer.
 * @return a ZDataBuffer containing the flat content of ZIndexControlBlock
 */
ZDataBuffer &
ZIndexControlBlock::_exportAppend(ZDataBuffer &pICBContent)
{
//  ZSICB_Export* wICBE;
  ZSICB_Export wICBe;

  wICBe.set(this);

  wICBe.serialize();
  pICBContent.appendData(&wICBe,sizeof(ZSICB_Export));

  IndexName._exportAppendUVF(pICBContent);  /* export index name */
  URIIndex._exportAppendUVF(pICBContent);   /* then index file name */

  /* no key dictionary in ZIndexControlBlock */

  return  pICBContent ;
}// _exportAppend



/*
 <indexcontrolblock> <!-- no dictionary in index control block -->
    <indexname> </indexname>
    <keyuniversalsize> </keyuniversalsize>
    <duplicates> </duplicates>
  </indexcontrolblock>
*/
utf8String ZIndexControlBlock::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("indexcontrolblock",pLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," no dictionary in index control block");
  wLevel++;
//  wReturn += ZSICBOwnData::toXml(wLevel); /* see if it makes sense to keep zsicbowndata as a specific section */
  wReturn+=fmtXMLchar("indexname",IndexName.toCChar(),wLevel);

  //wReturn+=fmtXMLuint32("icbtotalsize",ICBTotalSize,wLevel);
  //wReturn+=fmtXMLint32("zkdicoffset",ZKDicOffset,wLevel);
  wReturn+=fmtXMLuint32("keyuniversalsize",KeyUniversalSize,wLevel);

  //  wReturn+=fmtXMLuint("keytype",  KeyType,wLevel);  /* uint8_t */
  //  wReturn+=fmtXMLuint("autorebuild",AutoRebuild,wLevel);  /* uint16 must be casted */
  wReturn+=fmtXMLuint("duplicates",Duplicates,wLevel);

  wReturn += fmtXMLendnode("indexcontrolblock",pLevel);
  return wReturn;
} // toXml

ZStatus ZIndexControlBlock::fromXml(zxmlNode* pIndexNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode=nullptr;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  uint32_t wInt=0;

  ZStatus wSt = pIndexNode->getName()=="indexcontrolblock"?ZS_SUCCESS:ZS_INVNAME;
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZSIndexControlBlock::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "indexcontrolblock",
        decode_ZStatus(wSt));
    return ZS_XMLERROR;
  }
//  if (ZSICBOwnData::fromXml(wRootNode, pErrorlog) != 0)
//    return ZS_XMLERROR;

  if (XMLgetChildText(wRootNode, "indexname", IndexName, pErrorlog) < 0)
    {
      pErrorlog->errorLog("ZSIndexControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter <%s>.\n","indexname");
    }


  if (XMLgetChildUInt(wRootNode, "keyuniversalsize", KeyUniversalSize, pErrorlog)< 0)
    {
      pErrorlog->errorLog("ZSIndexControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter <%s>.\n","keyuniversalsize");
    }
  if (XMLgetChildUInt(wRootNode, "duplicates", wInt, pErrorlog)< 0)
    {
      pErrorlog->warningLog("ZSIndexControlBlock::fromXml-W-CNTFINDPAR Cannot find parameter <%s>. Will stay to its default.\n","duplicates");
//      Duplicates = ZST_NODUPLICATES;
    }
    else
      Duplicates = (ZSort_Type)wInt;

    return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//fromXml

ZIndexControlBlock&
ZIndexControlBlock::_copyFrom( const ZIndexControlBlock& pIn)
{
  ZSICBOwnData::_copyFrom(pIn);
  return *this;
}//_copyFrom


ZIndexControlBlock::~ZIndexControlBlock (void)
{

  if (CheckSum!=nullptr)
    delete CheckSum;

  return;
}
/*
void ZIndexControlBlock::newKeyDic(ZSKeyDictionary *pZKDic,ZMetaDic* pMetaDic)
{
  if (KeyDic)
    delete KeyDic;
  KeyDic=new ZSKeyDictionary(pMetaDic);
  KeyDic->_copyFrom(*pZKDic);
  KeyDic->MetaDic=pMetaDic;
  return;
}
*/
// computes and return the effective size of a ZIndex key record
// variable fields length impose to compute record size at record level
ssize_t ZIndexControlBlock::IndexRecordSize (void)
{
  return (KeyUniversalSize + sizeof(zaddress_type));
}//IndexRecordSize
/*
<icbowndata>
  <indexname> </indexname>
  <keyuniversalsize> </keyuniversalsize>
  <duplicates> </duplicates>
</icbowndata>
*/

utf8String ZSICBOwnData::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("icbowndata",pLevel);
  wLevel++;
  wReturn+=fmtXMLchar("indexname",IndexName.toCChar(),wLevel);

  //wReturn+=fmtXMLuint32("icbtotalsize",ICBTotalSize,wLevel);
  //wReturn+=fmtXMLint32("zkdicoffset",ZKDicOffset,wLevel);
  wReturn+=fmtXMLuint32("keyuniversalsize",KeyUniversalSize,wLevel);

//  wReturn+=fmtXMLuint("keytype",  KeyType,wLevel);  /* uint8_t */
//  wReturn+=fmtXMLuint("autorebuild",AutoRebuild,wLevel);  /* uint16 must be casted */
  wReturn+=fmtXMLuint("duplicates",Duplicates,wLevel);

  wReturn += fmtXMLendnode("icbowndata",pLevel);
  return wReturn;
} // ZSICBOwnData::toXml

ZStatus ZSICBOwnData::fromXml(zxmlNode* pIndexRankNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pIndexRankNode->getChildByName((zxmlNode *&) wRootNode, "icbowndata");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZSICBOwnData::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "icbowndata",
        decode_ZStatus(wSt));
    return wSt;
  }
  if (XMLgetChildText(wRootNode, "indexname", IndexName, pErrorlog) < 0)
    {
    pErrorlog->errorLog("ZSICBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter <%s>.\n","indexname");
    }


  if (XMLgetChildUInt(wRootNode, "keyuniversalsize", KeyUniversalSize, pErrorlog)< 0)
    {
    pErrorlog->errorLog("ZSICBOwnData::fromXml-E-CNTFINDPAR Cannot find parameter <%s>.\n","keyuniversalsize");
    }
  if (XMLgetChildUInt(wRootNode, "duplicates", wInt, pErrorlog)< 0)
    {
    pErrorlog->warningLog("ZSICBOwnData::fromXml-W-CNTFINDPAR Cannot find parameter <%s>. Will stay to its default.\n","duplicates");
    Duplicates = ZST_NODUPLICATES;
    }
  else
    Duplicates = (ZSort_Type)wInt;
/*  if (XMLgetChildUInt(wRootNode, "keytype", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "keytype");
  }
  else
    KeyType = (ZIFKeyType_type)wInt;

  if (XMLgetChildUInt(wRootNode, "autorebuild", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "autorebuild");
  }
  else
    AutoRebuild = (uint8_t)wInt;
*/


  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//ZSICBOwnData::fromXml
#ifdef __COMMENT__
//----------------ZIndexControlTable--------------------------

long ZSIndexControlTable::erase(const long pIdx)
{

/*  if (Tab[pIdx]->ZKDic!=nullptr)
    delete Tab[pIdx]->ZKDic;
  Tab[pIdx]->ZKDic=nullptr; */
  return  _Base::erase(pIdx);
}
long ZSIndexControlTable::push (ZSIndexControlBlock *pICB)
{

  /*    newBlankElement();
    last().clear(pMetaDic);  // set up ZICB and its Key Dictionnary

  memmove (&last(),pICB,sizeof(ZSICBOwnData));
  long wj = 0 ;
  while (wj<pICB.ZKDic->size())
  {
    last().ZKDic->push(pICB.ZKDic->Tab[wj]);
    wj++;
  }*/
      _Base::push(pICB);
  return  lastIdx();
}//push

long ZSIndexControlTable::pop (void)
{
  if (size()<1)
    return  -1;
/*                              done within ZSIndexControlBlock DTOR
  if (last()->ZKDic!=nullptr)
    {
    delete last()->ZKDic ;
    last()->ZKDic=nullptr;
    }*/
  return _Base::pop();
}
void ZSIndexControlTable::clear (void)
{
  while (size()>0)
    ZSIndexControlTable::pop();
  return;
}

long ZSIndexControlTable::zsearchIndexByName (const char* pName)
{
  for (long wi =0;wi<size();wi++)
  {
    if (Tab[wi]->IndexName==pName)
      return  wi;

  }

  return -1;
}//zsearchIndexByName
long ZSIndexControlTable::zsearchCaseIndexByName (const char* pName)
{
  for (long wi =0;wi<size();wi++)
  {
    if (Tab[wi]->IndexName.isEqualCase((const utf8_t*)pName))
    {
      return  wi;
    }
  }

  return -1;
}//zsearchIndexByName

#endif
/*
<indexcontroltable>
  <indexcontrolblock> <!-- no dictionary in index control block -->
    <indexname> </indexname>
    <keyuniversalsize> </keyuniversalsize>
    <duplicates> </duplicates>
  </indexcontrolblock>
...
  <indexcontrolblock> <!-- no dictionary in index control block -->
    <indexname> </indexname>
    <keyuniversalsize> </keyuniversalsize>
    <duplicates> </duplicates>
  </indexcontrolblock>
</indexcontroltable>
*/
#ifdef __COMMENT__
utf8String ZSIndexControlTable::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("indexcontroltable",pLevel);
  wLevel++;

  for (long wi=0;wi<count();wi++)
    wReturn+=Tab[wi]->toXml(wLevel);

  wReturn += fmtXMLendnode("indexcontroltable",pLevel);
  return wReturn;
} // toXml

ZStatus ZSIndexControlTable::fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  zxmlNode* wNode;
  ZSIndexControlBlock wICB;
  long wIndexRank=0;

  ZStatus wSt = pRootNode->getChildByName((zxmlNode *&) wRootNode, "indexcontroltable");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZSIndexControlTable::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "indexcontroltable",
        decode_ZStatus(wSt));
    return ZS_XMLERROR;
  }
  ZNodeCollection wNC =  wRootNode->getAllChildren("indexcontrolblock");

  wSt=ZS_SUCCESS;
  while ((wNC.count() > 0)&&(wSt==ZS_SUCCESS))
  {
    wNode=wNC.popR();

    wSt=wICB.fromXml(wNode,pErrorlog);
    if (wSt==ZS_SUCCESS)
    {
      if ((wIndexRank=zsearchCaseIndexByName(wICB.IndexName.toCChar()))>=0)
        Tab[wIndexRank]->_copyFrom(wICB);
      else
        push(new ZSIndexControlBlock(wICB));
    }
    XMLderegister(wNode);
  }// while

  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//fromXml
#endif // __COMMENT__

//-----------End ZIndexControlTable --------------------
