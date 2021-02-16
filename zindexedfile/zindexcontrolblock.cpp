#include "zindexcontrolblock.h"


ZSICBOwnData& ZSICBOwnData::_copyFrom(const ZSICBOwnData &pIn)
{
  ICBTotalSize=pIn.ICBTotalSize;
  ZKDicOffset=pIn.ZKDicOffset;
  KeyType=pIn.KeyType;
  AutoRebuild=pIn.AutoRebuild;
  Duplicates=pIn.Duplicates;
  Name=pIn.Name;
  return *this;
}

void
ZSICBOwnData_Export::clear(void)
{
  BlockID=ZBID_ICB;
  ZMFVersion =  __ZMF_VERSION__ ;
  //    Name.clear();   // index name is not defined in ICB export structure : got from ICB memory structure and exported as Universal string
  BlockID = ZBID_ICB;
  StartSign=cst_ZSTART;
  Duplicates = ZST_NODUPLICATES;
  AutoRebuild = false;
  ICBTotalSize =0;
  ZKDicOffset=0;
  //    Name=nullptr;
  return;
}//clear
ZDataBuffer&
ZSICBOwnData::_exportICBOwn(ZDataBuffer& pZDBExport)
{
  ZDataBuffer wIndexName;
  ZSICBOwnData_Export wICB;

  memset(&wICB,0,sizeof(ZSICBOwnData_Export));

  wIndexName=Name._exportUVF();
  ICBTotalSize = wIndexName.Size+sizeof(ZSICBOwnData_Export);
  ZKDicOffset = ICBTotalSize;
  wICB.clear();

  wICB.BlockID=ZBID_ICB ;   // uint8_t
  wICB.ZMFVersion=reverseByteOrder_Conditional<unsigned long>(__ZMF_VERSION__);
  wICB.ICBTotalSize=reverseByteOrder_Conditional<size_t>(ICBTotalSize);
  wICB.ZKDicOffset=reverseByteOrder_Conditional<size_t>(ZKDicOffset);

  wICB.AutoRebuild=AutoRebuild; // uint8_t
  wICB.Duplicates=Duplicates; // uint8_t
  wICB.KeyType=KeyType; // uint8_t
  //    wICB.EndSign=EndSign;
  pZDBExport.setData(&wICB,sizeof(wICB));

  pZDBExport.appendData(wIndexName);
  return pZDBExport;
}// ZSICBOwnData::_exportICBOwn
ZStatus
ZSICBOwnData::_importICBOwn(unsigned char* pZDBImport_Ptr)
{


  ZSICBOwnData_Export* wICBOwn_Import=(ZSICBOwnData_Export*)( pZDBImport_Ptr);

  clear();
  if (wICBOwn_Import->BlockID != ZBID_ICB) // has not to be reversed
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADICB,
        Severity_Severe,
        "Error Index Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
        wICBOwn_Import->BlockID);
    return (ZS_BADICB);
  }
  if (wICBOwn_Import->StartSign != cst_ZSTART)    // has not to be reversed
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Severe,
        "Error Index Control Block  : Index header appears to be corrupted - invalid ICB StartBlock");
    return (ZS_BADFILEHEADER);
  }

  if (reverseByteOrder_Conditional<unsigned long>(wICBOwn_Import->ZMFVersion) != __ZMF_VERSION__)// HAS to be reversed
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEVERSION,
        Severity_Severe,
        "Error Index Control Block   : Found version <%ld> while current ZMF version is <%ld>",
        wICBOwn_Import->ZMFVersion,
        __ZMF_VERSION__);
    return (ZS_BADFILEVERSION);
  }



  ICBTotalSize=reverseByteOrder_Conditional<size_t>(wICBOwn_Import->ICBTotalSize);
  ZKDicOffset=reverseByteOrder_Conditional<size_t>(wICBOwn_Import->ZKDicOffset);

  AutoRebuild=wICBOwn_Import->AutoRebuild;  // uint8_t
  Duplicates=wICBOwn_Import->Duplicates; // uint8_t
  KeyType=wICBOwn_Import->KeyType; // uint8_t

  unsigned char* wPtr=pZDBImport_Ptr+sizeof(ZSICBOwnData_Export); // index Name is stored just after ZSICBOwnData_Export structure
      // format is Universal format with leading size (uint16_t)

  size_t wSize=Name._importUVF(wPtr);  // Name is stored after ZSICBOwnData_Export as a varying number of byte (uint16_t is leading string size)

  //    size_t wZKDicOffset = wSize+sizeof(ZSICBOwnData_Export); // ZKDic starts after varying string Index Name
  //    EndSign=wICB->EndSign;

  return  ZS_SUCCESS;
}// ZSICBOwnData::_importICBOwn




utf8String ZSIndexControlBlock::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("indexcontrolblock",pLevel);
  wLevel++;
  wReturn += ZSICBOwnData::toXml(wLevel);

  wReturn += ZKDic->toXml(wLevel);

  wReturn += fmtXMLendnode("indexcontrolblock",pLevel);
  return wReturn;
} // toXml

int ZSIndexControlBlock::fromXml(zxmlNode* pIndexRankNode, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode;
  utfcodeString wXmlHexaId;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pIndexRankNode->getChildByName((zxmlNode *&) wRootNode, "fielddescription");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "FieldDescription::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "fielddescription",
        decode_ZStatus(wSt));
    return -1;
  }
  if (ZSICBOwnData::fromXml(wRootNode, pErrorlog) != 0)
    return -1;
  if (ZKDic->fromXml(wRootNode, pErrorlog)!= 0)
    return -1;
  return (int)pErrorlog->hasError();
}//fromXml

ZSIndexControlBlock&
ZSIndexControlBlock::_copyFrom( const ZSIndexControlBlock& pIn)
{
  ZSICBOwnData::_copyFrom(pIn);
  if (ZKDic!=nullptr)
    delete ZKDic;
  if (pIn.ZKDic!=nullptr)
  {
    ZKDic->_copyFrom(*pIn.ZKDic);
  }
  if (CheckSum!=nullptr)
  {
    delete CheckSum;
    CheckSum=nullptr;
  }
  if (pIn.CheckSum)
    CheckSum = new checkSum(*pIn.CheckSum);
  if (MetaDic!=nullptr)
  {
    delete MetaDic;
    MetaDic=nullptr;
  }
  if (pIn.MetaDic!=nullptr)
    MetaDic=new ZMetaDic(*pIn.MetaDic);

  return *this;
}//_copyFrom



ZSIndexControlBlock::ZSIndexControlBlock (ZMetaDic *pMetaDic)
{
  clear(pMetaDic);
  return;
}
ZSIndexControlBlock::~ZSIndexControlBlock (void)
{
  /*    if (ZKDic!=nullptr)
                {
                delete ZKDic ;
                ZKDic=nullptr;
                }*/
  if (CheckSum!=nullptr)
    delete CheckSum;
  MetaDic=nullptr; //MetaDic refers to ZSMasterFile::MetaDic
  return;
}
/**
 * @brief ZIndexControlBlock::clear Resets ZIndexControlBlock to initial data - reset Key Dictionnary
 */
void
ZSIndexControlBlock::clear(ZMetaDic*pMetaDic)
{

  ZSICBOwnData::clear();
  MetaDic = pMetaDic;
  if (ZKDic!=nullptr)
  {
    delete ZKDic ;
    ZKDic = nullptr;
  }
  ZKDic = new ZSKeyDictionary(pMetaDic) ;
  return;
}//clear

// computes and return the effective size of a ZIndex key record
// variable fields length impose to compute record size at record level
ssize_t ZSIndexControlBlock::IndexRecordSize (void)

{
  if (ZKDic==nullptr)
    return 0;
  ZKDic->_reComputeSize();
  return (ZKDic->KeyUniversalSize + sizeof(zaddress_type));
}//IndexRecordSize


utf8String ZSICBOwnData::toXml(int pLevel)
{
  int wLevel=pLevel;
  utf8String wReturn;
  wReturn = fmtXMLnode("icbowndata",pLevel);
  wLevel++;
  wReturn+=fmtXMLchar("name",Name.toCChar(),wLevel);

  wReturn+=fmtXMLuint64("icbtotalsize",ICBTotalSize,wLevel);
  wReturn+=fmtXMLuint64("zkdicoffset",ZKDicOffset,wLevel);

  wReturn+=fmtXMLuint("keytype",  KeyType,wLevel);  /* uint8_t */
  wReturn+=fmtXMLuint("autorebuild",AutoRebuild,wLevel);  /* uint16 must be casted */
  wReturn+=fmtXMLuint("duplicates",Duplicates,wLevel);

  wReturn += fmtXMLendnode("icbowndata",pLevel);
  return wReturn;
} // ZSICBOwnData::toXml

int ZSICBOwnData::fromXml(zxmlNode* pIndexRankNode, ZaiErrors* pErrorlog)
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
        "FieldDescription::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "icbowndata",
        decode_ZStatus(wSt));
    return -1;
  }
  if (XMLgetChildText(wRootNode, "name", Name, pErrorlog) < 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "name");
  }



  if (XMLgetChildULong(wRootNode, "icbtotalsize", ICBTotalSize, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "icbtotalsize");
  }
  if (XMLgetChildULong(wRootNode, "zkdicoffset", ZKDicOffset, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "zkdicoffset");
  }


  if (XMLgetChildUInt(wRootNode, "keytype", wInt, pErrorlog)< 0) {
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

  if (XMLgetChildUInt(wRootNode, "duplicates", wInt, pErrorlog)< 0) {
    fprintf(stderr,
        "FieldDescription::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its "
        "default.",
        "duplicates");
  }
  else
    Duplicates = (ZSort_Type)wInt;

  return (int)pErrorlog->hasError();
}//ZSICBOwnData::fromXml

