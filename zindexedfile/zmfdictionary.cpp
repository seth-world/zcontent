#include <zindexedfile/zmfdictionary.h>
#include <zxml/zxmlprimitives.h>


using namespace zbs;


ZMFDictionary::ZMFDictionary()
{
}
ZMFDictionary::ZMFDictionary(ZMFDictionary& pIn)
{
  _copyFrom(pIn);
}
ZMFDictionary::ZMFDictionary(ZMFDictionary&& pIn)
{
  _copyFrom(pIn);
}

ZMFDictionary&
ZMFDictionary::_copyFrom(ZMFDictionary& pIn)
{
  ZMetaDic::_copyFrom(pIn);
  KeyDic.clear();
  for (long wi = 0; wi < pIn.KeyDic.count(); wi ++)
  {
    KeyDic.push(new ZSKeyDictionary(pIn.KeyDic[wi]));
    KeyDic.last()->MetaDic=this;
  }
  return *this;
}


ZDataBuffer& ZMFDictionary::_exportAppend(ZDataBuffer& pZDB)
{
  ZMFDicExportHeader wHead;
  wHead.set(this);
  wHead._exportAppend(pZDB);

  ZMetaDic::_export(pZDB);
  for (long wi=0;wi<KeyDic.count();wi++)
    KeyDic[wi]->_exportAppend(pZDB);
  return pZDB;
}
size_t ZMFDictionary::_import(unsigned char* &pPtrIn)
{
  size_t wSizeImported=0;
  ZMFDicExportHeader wHead;
  wSizeImported += wHead._import(pPtrIn);

  wSizeImported += ZMetaDic::_import(pPtrIn);

  ZSKeyDictionary* wKDic=nullptr;
  uint32_t wi=0;
  while(wi++ < wHead.IndexCount)
    {
    wKDic=new ZSKeyDictionary(this);
    wSizeImported += wKDic->_import(pPtrIn);
    KeyDic.push(wKDic);
    }
  return wSizeImported;
}//ZMFDictionary::_import

/*
      <zmasterdictionary>
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

      </zmasterdictionary>
 */



utf8String ZMFDictionary::toXml(int pLevel)
{
  int wLevel=pLevel+1;
  utf8String wReturn;
  ZDataBuffer wB64;
  wReturn = fmtXMLnode("zmasterdictionary",pLevel);

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
  wReturn += ZMetaDic::toXml(wLevel);


  wReturn += fmtXMLnode("keydictionary",wLevel);
  for (long wi=0;wi < KeyDic.count();wi++)
  {
    wReturn += KeyDic[wi]->toXml(wLevel+1,wi);
  }
  wReturn += fmtXMLendnode("keydictionary",wLevel);
  wReturn += fmtXMLendnode("zmasterdictionary",pLevel);
  return wReturn;
} // ZMFDictionary::toXml

ZStatus
ZMFDictionary::fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog,ZaiE_Severity pSeverity)
{
  zxmlElement *wRootNode=nullptr;
  zxmlElement *wFieldsRootNode=nullptr;
  zxmlElement *wSingleFieldNode=nullptr;
  zxmlElement *wSwapNode=nullptr;
  utfcodeString wXmlHexaId;
  ZFieldDescription wFD;
  utf8String wValue;
  utfcodeString wCValue;
  bool wBool;
  unsigned int wInt;
  ZStatus wSt = pRootNode->getChildByName((zxmlNode *&) wRootNode, "zmfdictionary");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        pSeverity,
        wSt,
        "ZMFDictionary::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
        "zmfdictionary",
        decode_ZStatus(wSt));
    return wSt;
  }

  wSt=wRootNode->getChildByName((zxmlNode*&)wFieldsRootNode,"dicfields");
  if (wSt!=ZS_SUCCESS)
  {
    pErrorlog->logZStatus(
        pSeverity,
        wSt,
        "ZMetaDic::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "dicfields",
        decode_ZStatus(wSt));
    return wSt;
  }

  wSt=wFieldsRootNode->getFirstChild((zxmlNode*&)wSingleFieldNode);
  long wi=0;
  clear();  /* clear dictionary definitions */
  while (wSt==ZS_SUCCESS)
  {
    wFD.clear();
    if (wFD.fromXml(wSingleFieldNode,pErrorlog)==0)
      push(wFD);
    wSt=wSingleFieldNode->getNextNode((zxmlNode*&)wSwapNode);
    XMLderegister(wSingleFieldNode);
    wSingleFieldNode=wSwapNode;
  }
  XMLderegister(wRootNode);
  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//ZMFDictionary::fromXml






void ZMFDicExportHeader::set(const ZMFDictionary* pDic)
{
  IndexCount=uint32_t(pDic->KeyDic.count());
}

ZDataBuffer& ZMFDicExportHeader::_exportAppend(ZDataBuffer& pZDB)
{
  IndexCount = reverseByteOrder_Conditional<uint32_t>(IndexCount);
  pZDB.appendData(this,sizeof(ZMFDicExportHeader));
  return pZDB;
}
size_t ZMFDicExportHeader::_import(unsigned char* &pPtrIn)
{
  size_t wSizeImported=0;
  ZMFDicExportHeader* wHead=(ZMFDicExportHeader*)pPtrIn;
  if (wHead->BlockId!=ZBID_MDIC)
    {
    fprintf(stderr,"ZMFDicExportHeader::_import-W-CORRUPT Dictionary data to import appears to be corrupted.\n");
    abort();
    }
  IndexCount=reverseByteOrder_Conditional<uint8_t>(wHead->IndexCount);
  pPtrIn += sizeof(ZMFDicExportHeader);
  return sizeof(ZMFDicExportHeader);
}



