#include <zindexedfile/zmfdictionary.h>
#include <zxml/zxmlprimitives.h>


using namespace zbs;


ZMFDictionary::ZMFDictionary()
{
}
ZMFDictionary::ZMFDictionary(const ZMFDictionary &pIn)
{
  _copyFrom(pIn);
}
ZMFDictionary::ZMFDictionary(const ZMFDictionary&& pIn)
{
  _copyFrom(pIn);
}

ZMFDictionary::ZMFDictionary(const ZMetaDic& pIn)
{
  ZMetaDic::_copyFrom(pIn);
}

ZMFDictionary&
ZMFDictionary::_copyFrom(const ZMFDictionary& pIn)
{
  Active = pIn.Active;

  ZMetaDic::_copyFrom(pIn);
  KeyDic.clear();
  for (long wi = 0; wi < pIn.KeyDic.count(); wi ++)
  {
    KeyDic.push(new ZKeyDictionary(pIn.KeyDic[wi]));
    KeyDic.last()->Dictionary=this;
  }
  return *this;
}



ZStatus
ZMFDictionary::addKey(ZKeyDictionary*pIn, long &pOutKeyRank)
{
  pOutKeyRank=-1;
  for (long wi=0;wi < KeyDic.count();wi++)
    if (pIn->hasSameContentAs(KeyDic[wi]))
      {
      ZException.setMessage("ZMFDictionary::addKey",
          ZS_DUPLICATEKEY,
          Severity_Error,
          "While trying to add Key named <%s> to Master dictionary, a key with same content name <%s> rank <%ld> has been found.",
          pIn->DicKeyName.toString(),
          KeyDic[wi]->DicKeyName.toString(),
          wi);
      return  ZS_DUPLICATEKEY;
      }

  long wDI=KeyDic.push(new ZKeyDictionary(pIn->DicKeyName,this));
  for (long wi=0; wi < pIn->size();wi++)
    {
    ZIndexField wF=pIn->Tab[wi];  /* debug */
    KeyDic[wDI]->push(pIn->Tab[wi]);
    }
//  KeyDic[wDI]->setName (pIn->DicKeyName);
  fprintf (stdout,"Key <%s> added within master dictionary at rank <%ld>.\n",KeyDic[wDI]->DicKeyName.toCChar(),wDI);
  pOutKeyRank=wDI;
  return ZS_SUCCESS;
}//addKey

ZStatus
ZMFDictionary::addKey(ZKeyDictionary*pIn,const utf8String& pKeyName, long &pOutKeyRank)
{
  pOutKeyRank=-1;
  for (long wi=0;wi < KeyDic.count();wi++)
    {
    if (pIn->hasSameContentAs(KeyDic[wi]))
      {
        ZException.setMessage("ZMFDictionary::addKey",
            ZS_DUPLICATEKEY,
            Severity_Error,
            "While trying to add Key named <%s> to Master dictionary, a key with same content name <%s> rank <%ld> has been found.",
            pKeyName.toString(),
            KeyDic[wi]->DicKeyName.toString(),
            wi);
        return  ZS_DUPLICATEKEY;
      }
    if (pKeyName==KeyDic[wi]->DicKeyName.toCChar())
      {
        ZException.setMessage("ZMFDictionary::addKey",
            ZS_INVNAME,
            Severity_Error,
            "While trying to add Key named <%s> to Master dictionary, a key with same content name <%s> rank <%ld> has been found.",
            pKeyName.toString(),
            KeyDic[wi]->DicKeyName.toString(),
            wi);
        return  ZS_INVNAME;
      }
    }
  long wDI=KeyDic.push(new ZKeyDictionary(this));
  for (long wi=0; wi < pIn->size();wi++)
  {
    ZIndexField wF=pIn->Tab[wi];  /* debug */
    KeyDic[wDI]->push(pIn->Tab[wi]);
  }
  KeyDic[wDI]->setName(pKeyName);
  pOutKeyRank=wDI;
  return ZS_SUCCESS;
}//addKey


ZDataBuffer& ZMFDictionary::_exportAppend(ZDataBuffer& pZDB)
{
  ZMFDicExportHeader wHead;
  wHead.set(this);
  wHead._exportAppend(pZDB);

//  ZMetaDic::_exportAppend(pZDB);
  ZMetaDic::_exportAppendMetaDicFlat(pZDB);

  /* KeyDic is a ZArray of ZArrays */

  for (long wi=0;wi<KeyDic.count();wi++)
    KeyDic[wi]->_exportAppendFlat(pZDB);

  pZDB.append_T<uint32_t>(cst_ZBLOCKEND);
  return pZDB;
}

ZStatus ZMFDictionary::_import(const unsigned char* &pPtrIn)
{
  ZStatus wSt=ZS_SUCCESS;
  ZMFDicExportHeader wHead;
  wSt=wHead._import(pPtrIn);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wHead.get(*this);

//  wSt=ZMetaDic::_import(pPtrIn,wSize);
  wSt=ZMetaDic::_importMetaDicFlat(pPtrIn);

  if (wSt!=ZS_SUCCESS)
    return wSt;

  ZKeyDictionary* wKDic=nullptr;


  uint32_t wi=0;
  KeyDic.clear();
  while(wi++ < wHead.DicKeyCount)
    {
    wKDic=new ZKeyDictionary(this);
    wSt= wKDic->_importFlat(pPtrIn);
//    wSt= wKDic->_importRaw(pPtrIn,wSize);
    if (wSt!=ZS_SUCCESS)
      {
      ZException.addToLast("While importing dictionary key rank <%ld>.",KeyDic.lastIdx()+1);
          return wSt;
      }

    KeyDic.push(wKDic);
    }
  uint32_t wEndSign;
  _importAtomic<uint32_t>(wEndSign,pPtrIn);
  return wSt;
}//ZMFDictionary::_import

/*
      <dictionary>
        <active>true</active>
        <keycount>1</keycount>
        <metadic>
          <dicfields>
            <field>
              <name>%s</name>
              <capacity>%d</capacity><!-- if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if atomic -->
              <headersize>%ld</headersize>
              <naturalsize>%ld</naturalsize>
              <universalsize>%ld</universalsize>
              <ztype>%s</ztype>   <!-- see ZType_type definition : converted to its value number -->
              <hash>%s</hash>
            </field>
          </dicfields>
        </metadic>

        <keydictionary>
          <key>
          <rank>n</rank>
          <keyfields>
            <field>
                <mdicrank> </mdicrank>
                <hash>hhhhhhhhhh</hash> <--! hexa value -->
                <keyoffset> </keyoffset>

                <name>name of the field from meta dic</name> <--! from metadic -->
                <ztype> </ztype> <--! from meta dic not used -->
                <universalsize> </universalsize>  <--! from meta dic not used -->
            </field>
            <field>
              ...
            </field>
          </keyfields>
        </key>
        <key>
            <rank>n</rank>
            ....
          </key>
        </keydictionary>

      </zDictionarytionary>
 */

utf8VaryingString ZMFDictionary::XmlSaveToString(bool pComment)
{
  utf8String wReturn = fmtXMLdeclaration();
  wReturn += fmtXMLmainVersion("zmfdictionary",__ZDIC_VERSION__,0);
  wReturn += toXml(1,pComment);
  wReturn += fmtXMLendnode("zmfdictionary",0);
  return wReturn;
}

utf8VaryingString ZMFDictionary::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel+1;
  utf8String wReturn;
  ZDataBuffer wB64;
  wReturn = fmtXMLnode("dictionary",pLevel);


  wReturn += fmtXMLbool("active",Active,wLevel);
  wReturn += fmtXMLuint32("keycount",KeyDic.count(),wLevel);
  /* if (CheckSum!=nullptr)
      {
      wB64.setCheckSum(*CheckSum);
      wB64.encryptB64();
      wReturn+=fmtXMLchar("checksum",wB64.DataChar,wLevel);
      }
      else
      wReturn+=fmtXMLchar("checksum","none",wLevel);
*/
  wReturn += ZMetaDic::toXml(wLevel,pComment);


  wReturn += fmtXMLnode("keydictionary",wLevel);
  for (long wi=0;wi < KeyDic.count();wi++)
  {
    wReturn += KeyDic[wi]->toXml(wLevel+1,wi,pComment);
  }
  wReturn += fmtXMLendnode("keydictionary",wLevel);
  wReturn += fmtXMLendnode("dictionary",pLevel);
  return wReturn;
} // ZMFDictionary::toXml


ZStatus ZMFDictionary::XmlLoadFromString(const utf8String &pXmlString,bool pCheckHash,ZaiErrors* pErrorlog)
{
  ZStatus wSt;

  zxmlDoc *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;
  zxmlElement *wMetaRootNode=nullptr;

  pErrorlog->setContext("ZMFDictionary::XmlLoadFromString");

  wDoc = new zxmlDoc;
  wSt = wDoc->ParseXMLDocFromMemory(pXmlString.toCChar(), pXmlString.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZException();
    pErrorlog->errorLog(
        "ZMFDictionary::XmlloadFromString-E-PARSERR Xml parsing error for string <%s>",
        pXmlString.subString(0, 25).toUtf());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZException();
    return wSt;
  }
  if (!(wRoot->getName() == "zmfdictionary")) {
    pErrorlog->errorLog(
        "ZMFDictionary::XmlLoadFromString-E-INVROOT Invalid root node name <%s> expected <zmfdictionary>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }


  wSt=wRoot->getChildByName((zxmlNode*&)wMetaRootNode,"dictionary");
  if (wSt!=ZS_SUCCESS)
    {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZMFDictionary::XmlLoadFromString-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
        "dictionary",
        decode_ZStatus(wSt));
    return wSt;
    }

  wSt = fromXml(wMetaRootNode, pCheckHash,pErrorlog);

  XMLderegister((zxmlNode *&) wMetaRootNode);
  XMLderegister((zxmlNode *&) wRoot);

  return wSt;
}//ZMFDictionary::XmlLoadFromString

/*
<?xml version='1.0' encoding='UTF-8'?>
 <zmfdictionary version = "'0.30-0'">
   <Dictionary>
      <active>true</active>
      <keycount>1</keycount>
      <metadictionary>
         <version>'1.0-0'</version><!--  <version> field is the local version to the dictionary data (not to be confused with software version). -->
         <dicname>zdoc physical</dicname>
*/


ZStatus
ZMFDictionary::fromXml(zxmlNode* pDicNode, bool pCheckHash, ZaiErrors* pErrorlog)
{
  zxmlElement *wRootNode=nullptr;
  zxmlElement *wMDicRootNode=nullptr;
  zxmlElement *wKeyRootNode=nullptr;
//  zxmlElement *wKeyDicNode=nullptr;
  zxmlElement *wSwapNode=nullptr;

  pErrorlog->setContext("ZMFDictionary::fromXml");

  if (pDicNode->getName()!="dictionary")
    {
    pErrorlog->logZStatus(
        ZAIES_Error,
        ZS_INVNAME,
        "ZMFDictionary::fromXml-E-CNTFINDND Error cannot find master dictionary root node element with name <%s> status <%s>",
        "Dictionary",
        decode_ZStatus(ZS_XMLINVROOTNAME));
    return ZS_XMLINVROOTNAME;
    }

  ZStatus wSt;
  uint32_t wKeyCount;
  Active=false;
  wRootNode=(zxmlElement *)pDicNode;

  if (XMLgetChildBool(wRootNode,"active",Active,pErrorlog,ZAIES_Error)<0) {
    pErrorlog->logZStatus(ZAIES_Warning, ZS_XMLWARNING,"ZMFDictionary::fromXml-W-MISSFLD Missing field <active>");
  }

  if (XMLgetChildUInt32(wRootNode,"keycount",wKeyCount,pErrorlog,ZAIES_Error)<0) {
      pErrorlog->logZStatus(ZAIES_Warning, ZS_XMLWARNING,"ZMFDictionary::fromXml-W-MISSFLD Missing field <keycount>");
    }

  wSt=wRootNode->getChildByName((zxmlNode*&)wMDicRootNode,"metadictionary");
  if (wSt!=ZS_SUCCESS)
  {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZMFDictionary::fromXml-E-CNTFINDND Error cannot find meta dictionary root node element with name <%s> status <%s>",
        "metadictionary",
        decode_ZStatus(wSt));
    return wSt;
  }

  wSt=ZMetaDic::fromXml(wMDicRootNode,pCheckHash,pErrorlog);


/*
<keydictionary>
         <key>
            <keyname>Primary key</keyname><!--  linked to index control block indexname field  -->
            <duplicates>false</duplicates><!--  If set, key allows duplicates. if not - key must be unique  -->
            <keyfields>
               <field>
                  <mdicrank>0</mdicrank>
                  <hash>
                     <md5>412D1A55DB1580BF7B79177786B50693</md5>
                  </hash>
                  <keyoffset>0</keyoffset>
                  <!--  hereafter optional fields from MetaDic describing key field  -->
                  <name>Documentid</name>
                  <ztype>536870913</ztype><!--  ZType_type <ZType_Resource > converted to its value number -->
                  <universalsize>0</universalsize>
               </field>
            </keyfields>
         </key>
      </keydictionary>
*/


  wSt=wRootNode->getChildByName((zxmlNode*&)wKeyRootNode,"keydictionary");
  if (wSt!=ZS_SUCCESS)
  {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZMFDictionary::fromXml-W-CNTFINDND Warning. cannot find key dictionary root node element with name <%s> status <%s>.\n"
        "                                       No key will be defined for dictionary <%s>.",
        "keydictionary",
        decode_ZStatus(wSt),
        DicName.toString()
        );
    return wSt;
  }
  ZNodeCollection wNodes = wKeyRootNode->getAllChildren("key");
//  wSt=wKeyRootNode->getFirstChild((zxmlNode*&)wKeyDicNode);
  ZKeyDictionary* wKeyDic=nullptr;
  for (long wi=0; wi < wNodes.count(); wi++ ) {
    ZKeyDictionary* wKeyDic = new ZKeyDictionary(this);
    wSt=wKeyDic->fromXml(wNodes[wi],pErrorlog);
    if (wSt==ZS_SUCCESS) {
      KeyDic.push(wKeyDic);
    }
    else {
      delete wKeyDic;
      wKeyDic=nullptr;
    }
  }// for
  pErrorlog->textLog(" %ld keys loaded.\n"
                     "___________________________________________________\n", KeyDic.count());

//  XMLderegister(wKeyDicNode);
  return ZS_SUCCESS;
}//ZMFDictionary::fromXml





void ZMFDicExportHeader::set(const ZMFDictionary* pDic)
{
  Active = pDic->Active ;
  Version = pDic->Version ;
  CreationDate = pDic->CreationDate;
  ModificationDate = pDic->ModificationDate;
  DicKeyCount=uint32_t(pDic->KeyDic.count());
  DicName = pDic->DicName;
}

void ZMFDicExportHeader::get(ZMFDictionary& pDic)
{
  pDic.CreationDate = CreationDate;
  pDic.ModificationDate = ModificationDate;
  pDic.DicName = DicName;
  pDic.Active = Active;
  pDic.Version = Version;

  /* key count is calculated from ZKeyDictionary array */

  return;
}


ZDataBuffer& ZMFDicExportHeader::_exportAppend(ZDataBuffer& pZDB)
{
  pZDB.allocateBZero(sizeof(uint32_t)+2+sizeof(uint32_t)+sizeof(unsigned long)+sizeof(uint64_t)+sizeof(uint64_t));

  unsigned char* wPtr= pZDB.Data;
  _exportAtomicPtr(cst_ZBLOCKSTART,wPtr);   /* uint32_t */
  *wPtr++=ZBID_MDIC;                        /* uint8_t */
  *wPtr++= Active;                          /* uint8_t */
  _exportAtomicPtr(DicKeyCount,wPtr);       /* uint32_t */
  _exportAtomicPtr(Version,wPtr);           /* unsigned long */

  CreationDate._exportPtr(wPtr);            /* ZDateFull exported to uint64_t */
  ModificationDate._exportPtr(wPtr);        /* ZDateFull exported to uint64_t */

  DicName._exportAppendUVF(pZDB);           /* utf8VaryingString exported with UVF format */

  return pZDB;
}

/**
 * @brief ZMFDicExportHeader::_import  this routine does not change source data
 * @param pPtrIn
 * @param pImportedSize
 * @return
 */
ZStatus ZMFDicExportHeader::_import(const unsigned char* &pPtrIn)
{
  uint32_t wStartSign ;
  _importAtomic(wStartSign,pPtrIn);
  uint8_t wBlockId = *pPtrIn++;

  if ((wBlockId!=ZBID_MDIC)||(wStartSign!=cst_ZBLOCKSTART))
  {
    ZException.setMessage("ZMFDicExportHeader::_import",
        ZS_BADDIC,
        Severity_Severe,
        "Invalid Dictionary Header : found Start marker <%X> ZBlockId <%X>. One of these is invalid (or both are).",
        wStartSign,
        wBlockId);
    return  ZS_BADDIC;
  }
  Active = *pPtrIn++;
  _importAtomic(DicKeyCount,pPtrIn);
  _importAtomic(Version,pPtrIn);

  CreationDate._import(pPtrIn);
  ModificationDate._import(pPtrIn);

  DicName._importUVF(pPtrIn);

  return ZS_SUCCESS;
}//_import



