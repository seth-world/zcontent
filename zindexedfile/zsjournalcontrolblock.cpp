#include "zsjournalcontrolblock.h"

#include <zindexedfile/zsjournal.h>
#include <zxml/zxmlprimitives.h>


using namespace zbs;

ZSJournalControlBlock& ZSJournalControlBlock::_copyFrom(const ZSJournalControlBlock& pIn)
{
  Journal=pIn.Journal;
  Remote=pIn.Remote;
  Keep=pIn.Keep;
  Depth=pIn.Depth;
  JournalingOn=pIn.JournalingOn;
  JournalLocalDirectoryPath=pIn.JournalLocalDirectoryPath;
  return *this;
}

bool ZSJournalControlBlock::_isSameAs(const ZSJournalControlBlock* pJCB)
{
  if (pJCB==nullptr)
    return false;
  if (Journal!=nullptr)
    {
/* first compare journal */
      if (pJCB->Journal==nullptr)
        return false;
      if (Journal->getURIContent()!=pJCB->Journal->getURIContent())
        return false;
      if (Journal->getURIHeader()!=pJCB->Journal->getURIHeader())
        return false;
    }
  while (true)
    {
    if (Remote==nullptr)
      {
      if (pJCB->Remote!=nullptr)
          return false;
      /* here nothing more to compare : both are nullptr */
      break;
      }
    /* not nullptr */
    if (pJCB->Remote==nullptr)
        return false;
    if (Remote->Host!=pJCB->Remote->Host)
        return false;

    if (Remote->Port!=pJCB->Remote->Port)
      return false;
    if (Remote->SSLKey!=pJCB->Remote->SSLKey)
      return false;
    if (Remote->Protocol!=pJCB->Remote->Protocol)
      return false;
    if (Remote->SSLVector!=pJCB->Remote->SSLVector)
      return false;
    if (Remote->AddressType!=pJCB->Remote->AddressType)
      return false;
    if (Remote->Authenticate!=pJCB->Remote->Authenticate)
      return false;
    break;
    }// while true Remote

    if (Keep!=pJCB->Keep)
      return false;
    if (Depth!=pJCB->Depth)
      return false;
    if (JournalingOn!=pJCB->JournalingOn)
      return false;
    if (JournalLocalDirectoryPath!=pJCB->JournalLocalDirectoryPath)
      return false;
    if (Keep!=pJCB->Keep)
      return false;

  return true;
}

ZSJournalControlBlock::ZSJournalControlBlock()  {clear();}

//-----------------ZSJournalControlBlock-----------------------------

ZSJournalControlBlock::~ZSJournalControlBlock(void)
{
  if (Journal!=nullptr)
    delete Journal;
  return ;
}


void ZSJournalControlBlock::clear(void)
{
  memset(this,0,sizeof(ZSJournalControlBlock));
  JournalLocalDirectoryPath.clear();
  return ;
}



ZSJCBOwnData_Export& ZSJCBOwnData_Export::_copyFrom(const ZSJCBOwnData_Export& pIn)
{
  StartSign=pIn.StartSign;
  BlockID=pIn.BlockID;
  ZMFVersion=pIn.ZMFVersion;
  JCBSize=pIn.JCBSize;
  JournalingOn=pIn.JournalingOn;
  Keep=pIn.Keep;
  Depth=pIn.Depth;
  return *this;
}

ZSJCBOwnData_Export& ZSJCBOwnData_Export::_copyFrom(const ZSJCBOwnData& pIn)
{

  StartSign=cst_ZSTART;
  BlockID=ZBID_MCB;
  ZMFVersion=__ZMF_VERSION__;

  JournalingOn=pIn.JournalingOn;
  Keep=pIn.Keep;
  return *this;
}

ZSJCBOwnData& ZSJCBOwnData_Export::_toJCBOwnData(ZSJCBOwnData& pOut)
{
  pOut.Keep=Keep;
  pOut.Depth=Depth;
  pOut.JournalingOn=JournalingOn;

  return pOut;
}


ZDataBuffer&
ZSJCBOwnData::_exportAppend(ZDataBuffer& pZDBExport)
{
  ZSJCBOwnData_Export wJCBE;
  ZSJCBOwnData_Export* wJCBEPtr=nullptr ;

  wJCBE._copyFrom(*this);
  wJCBE.reverseConditional();
  pZDBExport.appendData(&wJCBE,sizeof(ZSJCBOwnData_Export));
  JournalLocalDirectoryPath._exportAppendUVF(pZDBExport);
/* set size */
  wJCBEPtr= (ZSJCBOwnData_Export*) pZDBExport.Data ;
  wJCBEPtr->JCBSize = uint32_t(pZDBExport.Size);
  wJCBEPtr->reverseConditional();
  return  pZDBExport;
}// ZSJCBOwnData::_exportAppend

ZDataBuffer
ZSJCBOwnData::_export()
{
  ZDataBuffer wZDB;
  return  _exportAppend(wZDB);
}// ZSJCBOwnData::_export

ZSJCBOwnData&
ZSJCBOwnData::_import(unsigned char* &pPtrIn)
{
  ZSJCBOwnData_Export* wJCB=(ZSJCBOwnData_Export*) pPtrIn;

  wJCB->reverseConditional();

  JournalingOn=wJCB->JournalingOn;
  Keep=wJCB->Keep;
  Depth=wJCB->Depth;
  pPtrIn+=sizeof(ZSJCBOwnData_Export);

  JournalLocalDirectoryPath._importUVF(pPtrIn);

  return *this;
}//ZSJCBOwnData::_import

/**
 * @brief ZSJournalControlBlock::_exportJCB   exports ZSJournalControlBlock content to a flat ZDataBuffer.
 * @return a ZDataBuffer containing the flat raw data exported from ZJCB
 */
ZDataBuffer& ZSJournalControlBlock::_exportJCB(ZDataBuffer &pJCBContent)
{

  return ZSJCBOwnData::_exportAppend(pJCBContent);
}// _exportJCB

size_t
ZSJournalControlBlock::_getExportSize()
{
  return (sizeof(ZSJCBOwnData_Export)+JournalLocalDirectoryPath._getexportUVFSize());
}

/**
 * @brief ZSJournalControlBlock::_importJCB imports (rebuild) a ZSJournalControlBlock from a ZDataBuffer containing flat raw data to import
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSJournalControlBlock::_importJCB(ZDataBuffer& pJCBContent)
{

  //ZStatus wSt;
  //long wIndexCountsv ;

  ZSJCBOwnData_Export* wJCB =(ZSJCBOwnData_Export*) pJCBContent.Data;

  if (wJCB->BlockID != ZBID_JCB)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADICB,
        Severity_Error,
        "Error Journal Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
        wJCB->BlockID);
    return (ZS_BADICB);
  }
  if (wJCB->StartSign != cst_ZSTART)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Error,
        "Error Journal Control Block  : Index header appears to be corrupted - invalid JCB StartBlock mark");
    return (ZS_BADFILEHEADER);
  }
  if (wJCB->ZMFVersion!= __ZMF_VERSION__)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEVERSION,
        Severity_Error,
        "Error Journal Control Block   : Found version <%ld> while current ZMF version is <%ld>",
        wJCB->ZMFVersion,
        __ZMF_VERSION__);
    return (ZS_BADFILEVERSION);
  }

  clear();
  memmove(this,pJCBContent.Data,sizeof(ZSJCBOwnData));

  return  ZS_SUCCESS;
}//_importJCB
ZStatus
ZSJournalControlBlock::_importJCB(unsigned char* & pPtrIn)
{

  //ZStatus wSt;
  //long wIndexCountsv ;

  ZSJCBOwnData_Export* wJCB =(ZSJCBOwnData_Export*) pPtrIn;

  wJCB->reverseConditional();

  if (wJCB->BlockID != ZBID_JCB)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADICB,
        Severity_Error,
        "Error Journal Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
        wJCB->BlockID);
    return (ZS_BADICB);
  }
  if (wJCB->StartSign != cst_ZSTART)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEHEADER,
        Severity_Error,
        "Error Journal Control Block  : Index header appears to be corrupted - invalid JCB StartBlock mark");
    return (ZS_BADFILEHEADER);
  }
  if (wJCB->ZMFVersion!= __ZMF_VERSION__)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_BADFILEVERSION,
        Severity_Error,
        "Error Journal Control Block   : Found version <%ld> while current ZMF version is <%ld>",
        wJCB->ZMFVersion,
        __ZMF_VERSION__);
    return (ZS_BADFILEVERSION);
  }

  clear();


  wJCB->_toJCBOwnData(*this);


  return  ZS_SUCCESS;
}//_importJCB

ZStatus
ZSJournalControlBlock::purge(const zrank_type pKeepRanks)
{

  if (Journal==nullptr)
  {
    return ZS_INVOP; // Beware return  is multiple instructions in debug mode
  }
  return Journal->purge(pKeepRanks);
}
/**
 * @brief ZSJournalControlBlock::setParameters sets the local journaling parameters (actually only JournalDirectoryPath)
 * @param pJournalPath
 */
void ZSJournalControlBlock::setParameters (uriString &pJournalPath)
{
  JournalLocalDirectoryPath=pJournalPath;
  return;
}
void
ZSJournalControlBlock::setRemoteMirroring (int8_t pProtocol,
    char* pHost,
    int   pPort,
    char* pAuthenticate,
    ZCryptKeyAES256* pSSLKey,
    ZCryptVectorAES256* pSSLVector)
{
  if (Remote!=nullptr)
    delete Remote;
  Remote = new ZRemoteMirroring;
  Remote->Protocol = pProtocol;
  Remote->Host=pHost;
  Remote->Port=pPort;
  Remote->Authenticate=pAuthenticate;

  if (pHost==nullptr)

  Remote->SSLKey=pSSLKey;
  Remote->SSLVector=pSSLVector;

//  strncpy(Remote->Host,pHost,sizeof(Remote->Host));
//  strncpy(Remote->Port,pPort,sizeof(Remote->Port));
//  strncpy(Remote->Authenticate,pAuthenticate,sizeof(Remote->Authenticate));
//  strncpy(Remote->SSLKey,pSSLKey,sizeof(Remote->SSLKey));
//  strncpy(Remote->SSLVector,pSSLVector,sizeof(Remote->SSLVector));

}

/*
    <zjournalcontrolblock>
      <journalingon>true</journalingon>
      <keep>true</keep>
      <journallocaldirectorypath></journallocaldirectorypath> <!-- omitted : same directory as ZRawMasterFile's path  -->
      <depth>-1</depth> <!-- max number of journaling records to keep. -1 : infinite. in this case need to use purgeJournaling(keepranks) regularly -->
      <zremotemirroring>
        <protocol>ZJP_RPC|ZJP_SSLV5</protocol> <!--   -->
        <addresstype>ZHAT_IPV4</addresstype> <!-- values ZHAT_IPV4 ZHAT_IPV6-->
        <port> 1156 </port>
        <host>JournalServer</host>

        <authenticate> <authenticate>

        <!-- optional SSL encryption : if exists then SSL Key and Vector will be generated accordingly-->
        <sslkey>  </sslkey>
        <sslvector> </sslvector>
      </zremotemirroring>
    </zjournalcontrolblock>
*/

utf8String
ZSJournalControlBlock::toXml(int pLevel,bool pComment)
{
  int wLevel=pLevel+1;
  utf8String wReturn;
  ZDataBuffer wB64;
  wReturn = fmtXMLnode("zjournalcontrolblock",pLevel);


  wReturn += fmtXMLbool("journalingon",bool(JournalingOn),wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," switches on/off journaling.");

  wReturn += fmtXMLbool("keep",bool(Keep),wLevel);

  wReturn += fmtXMLchar("journallocaldirectorypath",JournalLocalDirectoryPath.toCChar(),wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"omitted : same directory as ZRawMasterFile's path ");

  wReturn += fmtXMLint("depth",Depth,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Max number of journaling records to keep. -1 : infinite. in this case need to use purgeJournaling(keepranks) regularly ");
  if (pComment)
    fmtXMLcomment("optional zremotemirroring : if exists then remote mirroring will be done",wLevel);
  if (Remote!=nullptr)
    wReturn += Remote->toXml(wLevel);

  wReturn += fmtXMLendnode("zjournalcontrolblock",pLevel);
  return wReturn;
} // ZMFDictionary::toXml

ZStatus ZSJournalControlBlock::fromXml(zxmlNode* pIndexRankNode, ZaiErrors* pErrorlog,ZaiE_Severity pSeverity)
{
  zxmlElement *wRootNode=nullptr;
  zxmlElement *wRootNode_1=nullptr;

  utf8String wValue;
  bool wBool;
  ZStatus wSt = pIndexRankNode->getChildByName((zxmlNode *&) wRootNode, "zjournalcontrolblock");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZSJournalControlBlock::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "zjournalcontrolblock",
        decode_ZStatus(wSt));
    return wSt;
  }

  if (XMLgetChildBool(wRootNode, "journalingon", wBool, pErrorlog,pSeverity)< 0) {
    pErrorlog->errorLog(
        "ZSJournalControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s.It will stay to <false>.\n",
        "journalingon");
  }
  else
    JournalingOn=wBool;

  if (XMLgetChildBool(wRootNode, "keep", wBool, pErrorlog,pSeverity)< 0) {
    pErrorlog->errorLog(
        "ZSJournalControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s.It will stay to <false>.\n",
        "keep");
  }
  else
    Keep=wBool;

  if (XMLgetChildInt(wRootNode, "depth", Depth, pErrorlog,pSeverity)< 0) {
    pErrorlog->errorLog(
        "ZSJournalControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s.It will stay to its current value <%d>.\n",
        "depth",Depth);
  }


  if (XMLgetChildText(wRootNode, "journallocaldirectorypath", wValue, pErrorlog,pSeverity)< 0) {
    pErrorlog->errorLog(
        "ZSJournalControlBlock::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
        "journallocaldirectorypath");
  }
  else
    JournalLocalDirectoryPath=wValue.toCChar();

  wSt = wRootNode->getChildByName((zxmlNode *&) wRootNode_1, "zremotemirroring");
  if (wSt == ZS_SUCCESS)
    {
    if (Remote==nullptr)
      {
      Remote=new ZRemoteMirroring;
      }
    Remote->fromXml(wRootNode_1,pErrorlog,pSeverity);
    XMLderegister(wRootNode_1);
    }

  XMLderegister(wRootNode);
  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//ZSJournalControlBlock::fromXml



utf8String
ZRemoteMirroring::toXml(int pLevel)
{
  int wLevel=pLevel+1;
  utf8String wReturn;
  ZDataBuffer wB64;
  wReturn = fmtXMLnode("zremotemirroring",pLevel);

  wReturn += fmtXMLint("protocol",int(Protocol),wLevel);
  wReturn += fmtXMLint("addresstype",int(AddressType),wLevel);
  fmtXMLaddInlineComment(wReturn,"values ZHAT_IPV4 ZHAT_IPV6. Defaulted to ZHAT_IPV4.");

  wReturn += fmtXMLchar("host",Host.toCChar(),wLevel);
  wReturn += fmtXMLint("port",Port,wLevel);

  wReturn += fmtXMLchar("authenticate",Authenticate.toCChar(),wLevel);

  if (SSLKey!=nullptr)
  {
    wReturn += fmtXMLcomment(" optional SSL encryption : SSL Key and Vector will be generated accordingly",wLevel);
    wReturn += fmtXMLSSLKeyB64("sslkey",*SSLKey,wLevel);  /* if SSLKey is nullptr then SSLVector MUST BE nullptr */
    wReturn += fmtXMLSSLVectorB64("sslvector",*SSLVector,wLevel);
  }
/*  if (SSLVector!=nullptr)
  {
    wReturn += fmtXMLSSLVectorB64("sslvector",*SSLVector,wLevel);
  }
*/
  wReturn += fmtXMLendnode("zremotemirroring",pLevel);
  return wReturn;
} // ZRemoteMirroring::toXml


ZStatus ZRemoteMirroring::fromXml(zxmlNode* pIndexRankNode, ZaiErrors* pErrorlog,ZaiE_Severity pSeverity)
{
  zxmlElement *wRootNode=nullptr;
  utf8String wValue;
  int wInt;
  ZStatus wSt = pIndexRankNode->getChildByName((zxmlNode *&) wRootNode, "zremotemirroring");
  if (wSt != ZS_SUCCESS) {
    pErrorlog->logZStatus(
        ZAIES_Error,
        wSt,
        "ZRemoteMirroring::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status "
        "<%s>",
        "zremotemirroring",
        decode_ZStatus(wSt));
    return wSt;
  }

  if (XMLgetChildInt(wRootNode,"protocol",wInt,pErrorlog,pSeverity)<0)
    {
    pErrorlog->errorLog(
        "ZRemoteMirroring::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its default value.\n",
        "protocol");
    }
    else
      Protocol=uint8_t(wInt);

    if (XMLgetChildInt(wRootNode,"addresstype",wInt,pErrorlog,pSeverity)<0)
    {
      pErrorlog->errorLog(
          "ZRemoteMirroring::fromXml-E-CNTFINDPAR Cannot find parameter %s. It will stay to its default value.\n",
          "addresstype");
    }
    else
      AddressType=ZHostAddressType(wInt);

    if (XMLgetChildText(wRootNode,"host",Host,pErrorlog,pSeverity)<0)
    {
      pErrorlog->errorLog(
          "ZRemoteMirroring::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
          "host");
    }
    if (XMLgetChildInt(wRootNode,"port",Port,pErrorlog,pSeverity)<0)
    {
      pErrorlog->errorLog(
          "ZRemoteMirroring::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
          "port");
    }
    if (XMLgetChildText(wRootNode,"authenticate",Authenticate,pErrorlog,pSeverity)<0)
    {
      pErrorlog->errorLog(
          "ZRemoteMirroring::fromXml-E-CNTFINDPAR Cannot find parameter %s.\n",
          "authenticate");
    }

    /* SSL Encryption rules :
     * ----------------------
     * XML file : there is an SSL key, there must be an SSL vector  -> xml encryption defined
     *    (if one of these exists while the other miss : xml corrupted)
     *  XML file : there is neither SSL key nor SSL vector -> xml encryption not defined
     * if xml has encryption defined :
     *    -> SSL key and vector do not exist : create them and load with xml key and vector -> encryption is enabled. a warning is issued
     *    -> SSL key and vector exist : replace them with xml key and vector -> encryption stays enabled
     *
     * if xml encryption not defined :
     *    -> SSL key and vector do not exist : leave as it is -> encryption stays disabled
     *    -> SSL key and vector exist : DESTROY THEM -> encryption is disabled . a warning is issued
     */

    if (XMLhasNamedChild(wRootNode,"sslkey"))
      {
        if (!XMLhasNamedChild(wRootNode,"sslvector"))
        {
          pErrorlog->errorLog(
              "ZRemoteMirroring::fromXml-E-CRYPTCORRUPT SSL key exists while vector is not found : xml file corrupted.\n");
          return ZS_XMLERROR;
        }
      }
      else  /* sslkey is not found */
      if (XMLhasNamedChild(wRootNode,"sslkey"))
        {
        pErrorlog->errorLog(
            "ZRemoteMirroring::fromXml-E-CRYPTCORRUPT SSL vector exists while key is not found : xml file corrupted.\n");
        return ZS_XMLERROR;
        }



    if (XMLhasNamedChild(wRootNode,"sslkey"))
    {
      if (SSLKey==nullptr)
        {
        SSLKey=new ZCryptKeyAES256;
        pErrorlog->warningLog(
            "ZRemoteMirroring::fromXml-W-KEYCRE creation of encryption key. Encryption is set to enabled.\n");
        }
      if (XMLgetChildSSLKeyB64(wRootNode,"sslkey",*SSLKey,pErrorlog,pSeverity)<0)
      {
        pErrorlog->errorLog(
            "ZRemoteMirroring::fromXml-E-CNTFINDPAR Cannot load parameter %s.\n",
            "sslkey");
        return ZS_XMLERROR;
      }
      if (SSLVector==nullptr)
      {
         SSLVector=new ZCryptVectorAES256;
        pErrorlog->warningLog(
            "ZRemoteMirroring::fromXml-W-VECTCRE creation of encryption vector. Encryption is set to enabled.\n");
      }

      if (XMLgetChildSSLVectorB64(wRootNode,"sslvector",*SSLVector,pErrorlog,pSeverity)<0)
      {
        pErrorlog->errorLog(
            "ZRemoteMirroring::fromXml-E-CNTFINDPAR Cannot load parameter %s.\n",
            "sslvector");
        return ZS_XMLERROR;
      }
    }//if (XMLhasNamedChild(wRootNode,"sslkey"))
    else
    {
    if (SSLKey!=nullptr)
      {
        delete SSLKey;
        pErrorlog->warningLog(
            "ZRemoteMirroring::fromXml-W-KEYDEL deletion of encryption key. Encryption has been disabled.\n");
      }
    if (SSLVector!=nullptr)
      {
      delete SSLVector;
      pErrorlog->warningLog(
          "ZRemoteMirroring::fromXml-W-VECTDEL deletion of encryption vector. Encryption has been disabled.\n");
      }
    }

  XMLderegister(wRootNode);
  return pErrorlog->hasError()?ZS_XMLERROR:ZS_SUCCESS;
}//ZRemoteMirroring::fromXml
