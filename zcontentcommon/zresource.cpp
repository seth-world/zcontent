#ifndef ZRESOURCE_CPP
#define ZRESOURCE_CPP

#include "zresource.h"
#include <zxml/zxmlprimitives.h>
#include <zindexedfile/zdatatype.h>
#include <zindexedfile/zdataconversion.h>
#include <zxml/zxml.h>
#include <stdint.h>


Resourceid_type ResourceId = 0;

Resourceid_type getUniqueResourceId() {return ResourceId++;}


utf8VaryingString ZResource::toXml( int pLevel,const utf8VaryingString &pName)
{
    utf8VaryingString wContent = fmtXMLnode(pName, pLevel);

    wContent += fmtXMLint64("id", id, pLevel+1);
    wContent += fmtXMLint64("entity", Entity, pLevel+1);
    //    wContent += fmtXMLint64("datarank", DataRank, pLevel);
    wContent += fmtXMLendnode(pName, pLevel);
    return wContent;
}
/*
int ZResource::fromXml(zxmlElement *pRootNode, const char *pChildName, ZaiErrors *pErrorLog)
{
    zxmlElement*wChild=nullptr;
    ZStatus wSt=pRootNode->getChildByName((zxmlNode*&)wChild,pChildName);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorLog->logZStatus(ZAIES_Error,wSt,"ZResource::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pChildName,
            decode_ZStatus(wSt));
        return -1;
    }
    wSt=wChild->getChildByName((zxmlNode*&)wChild,"zresource");
    if (wSt!=ZS_SUCCESS)
    {
        pErrorLog->logZStatus(ZAIES_Error,wSt,"ZResource::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pChildName,
            decode_ZStatus(wSt));
        return -1;
    }
    if (XMLgetChildUInt64Hexa(pRootNode, "entity", Entity, pErrorLog) < 0) {
        pErrorLog->errorLog("ZResource::fromXML-E-CANTGETTXT Cannot get child <entity> as "
                            "hexadecimal text for node <%s>",
            pRootNode->getName().toCChar(),
            "zresource");
        return -1;
    }
    if (XMLgetChildInt64Hexa(pRootNode, "entity", id, pErrorLog) < 0) {
        pErrorLog->errorLog("ZResource::fromXML-E-CANTGETTXT Cannot get child <id> as "
                            "hexadecimal text for node <%s>",
            pRootNode->getName().toCChar(),
            "zresource");
        return -1;
    }
    return 0;
}
*/

ZStatus
ZResource::fromXml (zxmlElement*pElement,const utf8VaryingString &pChildName,ZaiErrors* pErrorLog, ZaiE_Severity pSeverity)
{
    uint64_t wValue;

     zxmlElement* wChild=nullptr;
    zxmlElement* wChildRes=nullptr;
 //   zxmlElement*wChildEntity=nullptr;
 //   zxmlElement*wChildId=nullptr;
    ZStatus wSt=pElement->getChildByName((zxmlNode*&)wChild,pChildName);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorLog->logZStatus(pSeverity,wSt,"ZResource::fromXml-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                              pChildName.toCChar(),
                              decode_ZStatus(wSt));
        return ZS_NOTFOUND;
    }

    wSt=wChild->getChildByName((zxmlNode*&)wChildRes,"zresource");
    XMLderegister(wChild);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorLog->logZStatus(pSeverity,wSt,"ZResource::fromXml-E-CNTFINDND Error cannot find node element with name <zresource> status <%s>",
                              decode_ZStatus(wSt));
        return ZS_INVNAME;
    }
    int wRet=XMLgetChildUInt64(wChildRes,"entity",wValue,pErrorLog,pSeverity);
    if (wRet < 0) {
        XMLderegister(wChildRes);
        return ZS_INVNAME;
    }
    Entity=(ZEntity_type)wValue;

    wRet=XMLgetChildUInt64(wChildRes,"id",wValue,pErrorLog,pSeverity);
    XMLderegister(wChildRes);
    if (wRet < 0) {
        return ZS_INVNAME;
    }
    id = wValue;
    return ZS_SUCCESS;
} //fromXml

int ZResource::fromXmltoHexa(zxmlElement *pRootNode,
                             const char *pChildName,
                             utf8VaryingString &wCode,
                             ZaiErrors *pErrorLog)
{
    utf8VaryingString wValue;
//    utf8VaryingString wCode;

    zxmlElement*wChild=nullptr;
    ZStatus wSt=pRootNode->getChildByName((zxmlNode*&)wChild,pChildName);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorLog->logZStatus(ZAIES_Error,wSt,"ZResource::fromXmltoHexa-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pChildName,
            decode_ZStatus(wSt));
        return -1;
    }
    wSt=wChild->getChildByName((zxmlNode*&)wChild,"zresource");
    if (wSt!=ZS_SUCCESS)
    {
        pErrorLog->logZStatus(ZAIES_Error,wSt,"ZResource::fromXmltoHexa-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pChildName,
            decode_ZStatus(wSt));
        return -1;
    }

    if (XMLgetChildText(wChild, "entity", wValue, pErrorLog) < 0) {
        pErrorLog->errorLog("ZResource::fromXML-E-CANTGETTXT Cannot get child <entity> as "
                            "hexadecimal text for node <%s>",
            pRootNode->getName().toCChar(),
            "zresource");
        return -1;
    }
    wCode = wValue.toCChar();
    if (XMLgetChildText(pRootNode, "id", wValue, pErrorLog) < 0) {
        pErrorLog->errorLog("ZResource::fromXML-E-CANTGETTXT Cannot get child <id> as hexadecimal "
                            "text for node <%s>",
            pRootNode->getName().toCChar(),
            "zresource");
        return -1;
    }

    wCode += "#";
    wCode += wValue.toCChar();

    return 0;
}//fromXmltoHexa

utf8VaryingString ZResource::toStr() const
{
    utf8VaryingString wContent;
    if (Entity==cst_EntityInvalid)
      wContent = "InvalidEntity#";
    else
      wContent.sprintf("%ld#", Entity);
    if (id==cst_ResourceInvalid)
      wContent += "InvalidId";
    else
      wContent.addsprintf("%ld", id);
    return wContent;
}
utf8VaryingString ZResource::toHexa() const
{
    utf8VaryingString wContent;
    if (Entity==cst_EntityInvalid)
      wContent = "InvalidEntity#";
    else
      wContent.sprintf("%08X#", Entity);
    if (id==cst_ResourceInvalid)
      wContent += "InvalidId";
    else
      wContent.addsprintf("%08X", id);
    return wContent;
}


ZDataBuffer ZResource::_export() const
{
  ZDataBuffer wReturn;
  ZEntity_type wEntity=reverseByteOrder_Conditional<ZEntity_type>(Entity);
  wReturn.appendData(&wEntity,sizeof (ZEntity_type));
  Resourceid_type wId=reverseByteOrder_Conditional<Resourceid_type>(id);
  wReturn.setData(&wId,sizeof(Resourceid_type));
  return wReturn;
}

unsigned char* ZResource::_export(unsigned char*& pBuffer,size_t& pSize) const
{
  errno=0;
  pSize = sizeof(ZEntity_type)+sizeof(Resourceid_type);
  pBuffer =(unsigned char*) malloc(pSize);
  if (pBuffer==nullptr)
    {
    pSize=0;
    errno=ENOMEM;
    return nullptr;
    }
  ZEntity_type wEntity=reverseByteOrder_Conditional<ZEntity_type>(Entity);
  Resourceid_type wId=reverseByteOrder_Conditional<Resourceid_type>(id);
  memmove(pBuffer,&wEntity,sizeof(wEntity));
  pBuffer += sizeof(wEntity);
  memmove(pBuffer,&wId,sizeof(Resourceid_type));
  pBuffer += sizeof(Resourceid_type);
  return pBuffer;
}

size_t ZResource::_import(const unsigned char *&pUniversalPtr)
{
  const unsigned char* wPtrIn=pUniversalPtr;
  ZEntity_type wEntity;
  Resourceid_type wId;
  memmove(&wEntity,wPtrIn,sizeof(ZEntity_type));
  Entity=reverseByteOrder_Conditional<ZEntity_type>(wEntity);
  wPtrIn += sizeof(ZEntity_type);
  memmove(&wId,wPtrIn,sizeof(Resourceid_type));
  id = reverseByteOrder_Conditional<Resourceid_type>(wId);
  pUniversalPtr += sizeof(ZEntity_type)+sizeof(Resourceid_type);
  return sizeof(ZEntity_type)+sizeof(Resourceid_type);
}


size_t ZResource::_exportURF(ZDataBuffer& pReturn) const
{
  unsigned char* wPtrOut=pReturn.extend(sizeof(ZTypeBase)+sizeof(ZEntity_type)+sizeof(Resourceid_type));
  return _exportURF_Ptr(wPtrOut);
}

size_t ZResource::_exportURF_Ptr(unsigned char* &pURF) const
{
  _exportAtomicPtr<ZTypeBase>(ZType_Resource, pURF);
  _exportAtomicPtr<ZEntity_type>(Entity, pURF);
  _exportAtomicPtr<Resourceid_type>(id, pURF);
  return sizeof(ZTypeBase)+sizeof(ZEntity_type)+sizeof(Resourceid_type);
}
size_t ZResource::getURFSize() const
{
  return sizeof(ZTypeBase)+sizeof(ZEntity_type)+sizeof(Resourceid_type);
}

size_t ZResource::getURFHeaderSize()
{
  return sizeof(ZTypeBase) ;
}

size_t ZResource::getUniversalSize() const
{
  return sizeof(ZEntity_type)+sizeof(Resourceid_type);
}

ssize_t ZResource::_importURF(const unsigned char *&pUniversalPtr)
{
  ZTypeBase wType;
  _importAtomic<ZTypeBase>(wType, pUniversalPtr);
  if (wType!=ZType_Resource)
    {
    fprintf(stderr,"ZResource::_importURF-E-INVTYP Invalid data type found <%X> <%s>while expecting <ZType_Resource>.\n",
        wType,decode_ZType(wType));
    return -1;
    }

  _importAtomic<ZEntity_type>(Entity, pUniversalPtr);
  _importAtomic<Resourceid_type>(id, pUniversalPtr);
  return sizeof(ZTypeBase)+sizeof(ZEntity_type)+sizeof(Resourceid_type);
}


size_t ZResource::getUniversalSize() {
  return sizeof(ZEntity_type)+sizeof(Resourceid_type)+1; /* NB: Resourceid_type is signed */
}
size_t ZResource::getUniversal_Ptr(unsigned char*& pPtr) {
  _exportAtomicPtr<ZEntity_type>(Entity,pPtr);
  _getAtomicUfN_Ptr<Resourceid_type>(id,pPtr);
  return sizeof(ZEntity_type)+sizeof(Resourceid_type)+1;
}

#endif // ZRESOURCE_CPP
