#ifndef ZMASTERFILE_UTILITIES_CPP
#define ZMASTERFILE_UTILITIES_CPP
#include <zindexedfile/zmasterfile_utilities.h>
#include <zindexedfile/zindexcollection.h>
#include <zindexedfile/zsmasterfile.h>
#include <zxml/zxml.h>
#include <zxml/zxmlprimitives.h>
#include <ztoolset/zaierrors.h>
/**
  @ingroup ZMFPhysical
  @{ */

/**
 * @brief generateIndexRootName generates a ZSIndexFile root name from its father ZSMasterFile's root name
 *
 * ZSIndexFile name generation rule
 *@verbatim
 *          <master root name>[<index name>]-<index rank>
 *@endverbatim
 *
 * @param[in] pMasterRootName   a descString containing the father ZSMasterFile's root name
 * @param[in] pRank             Index rank
 * @param[in] pIndexName        Index user name : could be empty
 * @return an utf8String with the appropriate ZSIndexFile root name
 */
utf8String generateIndexRootName(utf8String &pMasterRootName,
                                 const long pRank,
                                 utf8String &pIndexName)
{
  utf8String wIndexRootName;
    wIndexRootName = pMasterRootName;
    wIndexRootName += (utf8_t)'-';
    if (pIndexName.isEmpty())
                {
                wIndexRootName+="index";
                }
            else
                {
                wIndexRootName += pIndexName.toCChar();
                }
    wIndexRootName += (utf8_t)'-';
    wIndexRootName.addsprintf("%02ld",pRank);
//    sprintf(&DSRootName.content[DSRootName.size()],"%02ld",pRank);

    wIndexRootName.eliminateChar(' ');
    wIndexRootName += __ZINDEX_FILEEXTENSION__;
    return wIndexRootName;
} // generateIndexRootName
/**
 * @brief generateIndexURI
 *              generates the index uri full base name (including directory path) but without any extension (and without '.' char)
 *
 *  If no directory is mentionned in pMasterName path, then the current directory is taken.
 *  returns a ZS_INVNAME status if ZSMasterFile's extension is __ZINDEX_FILEEXTENSION__
 *  returns ZS_SUCCESS otherwise.
 *
 *  For base name generation @see generateIndexRootName
 *
 * @param[in]  pMasterFileUri   Base name for ZSMasterFile to create ZSIndexFile name for
 * @param[in]  pPathDir         Directory path to create the ZSIndexFile in
 * @param[out] pZSIndexFileUri   Resulting ZSIndexFile name
 * @param [in] pRank            Index rank for the ZSMasterFile
 * @param[in]  pIndexName       Index name (given by user) for the Index
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
generateIndexURI(uriString pMasterFileUri,
                 uriString &pDirectory,
                 uriString &pZSIndexFileUri,
                 const long pRank,
                 utf8String& pIndexName)
{
uriString  wPath_Uri;
utf8String wMasterName;
utf8String wMasterExt;


    if (pDirectory.isEmpty())
            wPath_Uri = pMasterFileUri.getDirectoryPath().toCChar();
        else
            wPath_Uri = pDirectory.toString();

    QUrl wUrl(wPath_Uri.toCString_Strait());
    pZSIndexFileUri.fromQString(wUrl.toString(QUrl::PreferLocalFile));
    pZSIndexFileUri.addConditionalDirectoryDelimiter() ;

    wMasterName = pMasterFileUri.getBasename().toCChar();
    wMasterExt=pMasterFileUri.getFileExtension().toCChar();
    const utf8_t* wExt=(const utf8_t*)__ZINDEX_FILEEXTENSION__;
    wExt++;                             // skip the '.' char
    if (wMasterExt==wExt)
                {
                ZException.setMessage (_GET_FUNCTION_NAME_,
                                         ZS_INVNAME,
                                         Severity_Error,
                                         " Invalid ZSMasterFile name extension %s for name %s",
                                         wMasterExt.toCChar(),
                                         pMasterFileUri.toCChar()
                                         );
                return ZS_INVNAME;
                }

    utf8String wM;
    wM=generateIndexRootName(wMasterName,pRank,pIndexName);
    pZSIndexFileUri += wM.toCChar();

    return(ZS_SUCCESS);
} //generateIndexURI

/** @} */ // ingroup ZMFPhysical


utfdescString ZStringBuffer2;
const char *decode_ZCOP (uint16_t pZCOP)
{
    if (pZCOP==ZCOP_Nothing)
                return "ZCOP_Nothing" ;

    ZStringBuffer2.clear();

    if (pZCOP & ZCOP_Interval)
            {
            ZStringBuffer2=(const utf8_t*)"ZCOP_Interval |" ;

            }
    if (pZCOP & ZCOP_Exclude)
            {
            ZStringBuffer2 += (const utf8_t*)" ZCOP_Exclude |" ;
            }

    if (pZCOP & ZCOP_GetFirst)
            ZStringBuffer2 += (const utf8_t*)" ZCOP_GetFirst";
    if (pZCOP & ZCOP_GetNext)
            ZStringBuffer2 += (const utf8_t*)" ZCOP_GetNext";
    if (pZCOP & ZCOP_GetAll)
            ZStringBuffer2 += (const utf8_t*)" ZCOP_GetAll";
    if (pZCOP & ZCOP_RemoveAll)
            ZStringBuffer2 += (const utf8_t*)" ZCOP_RemoveAll";
    if (pZCOP & ZCOP_LockAll)
            ZStringBuffer2 += (const utf8_t*)" ZCOP_LockAll";


    return ZStringBuffer2.toCString_Strait() ;
} // decode_ZCOP

//=======================XML==================================================

/*
<?xml version='1.0' encoding='UTF-8'?>
<zicm version="2.00">
<File>
  <ZFileDescriptor>
      <URIContent>/home/gerard/testindexedpictures.zmf</URIContent>
      <URIHeader>/home/gerard/testindexedpictures.zrh</URIHeader> <!-- not modifiable generated by ZRandomFile-->
      <URIDirectoryPath>/home/gerard/</URIDirectoryPath> <!-- not modifiable generated by ZRandomFile-->
      <ZHeaderControlBlock> <!-- not modifiable -->
          <FileType>ZFT_ZSMasterFile</FileType>  <!-- do not modify : could cause data loss  see documentation-->
          <ZRFVersion>2000</ZRFVersion> <!-- not modifiable -->
          <OffsetFCB>4698</OffsetFCB> <!-- not modifiable -->
          <OffsetReserved>64</OffsetReserved> <!-- not modifiable -->
          <SizeReserved>4634</SizeReserved> <!-- not modifiable -->
      </ZHeaderControlBlock>
   <ZFileControlBlock>
      <AllocatedBlocks>15</AllocatedBlocks> <!-- not modifiable -->
      <BlockExtentQuota>5</BlockExtentQuota>
      <InitialSize>100000</InitialSize>
      <BlockTargetSize>1</BlockTargetSize>
      <HighwaterMarking>false</HighwaterMarking>
      <GrabFreeSpace>true</GrabFreeSpace>
   </ZFileControlBlock>
  </ZFileDescriptor>
  <ZMasterControlBlock>
         <HistoryOn>false</HistoryOn>
         <JournalingOn>true</JournalingOn>
         <IndexFileDirectoryPath></IndexFileDirectoryPath>
         <!-- Changing IndexRank position in a file description will imply huge modifications -->
         <Index>
         <IndexRank>
           <Rank>0</Rank> <!-- not significant -->
           <Name>First Index</Name> <!-- warning modification of this field is dangerous see documentation -->
           <Duplicates>ZST_NODUPLICATES</Duplicates> <!-- warning modification of this field is dangerous see documentation -->
           <AutoRebuild>false</AutoRebuild>
           <ZKDic>
               <NaturalSize>62</NaturalSize>   <!-- not modifiable -->
               <UniversalSize>65</UniversalSize> <!-- not modifiable -->
              <KeyField>
                <Rank>0</Rank>  <!-- not significant -->
                <Name>String1</Name>
                <NaturalSize>50</NaturalSize>
                <UniversalSize>50</UniversalSize>
                <ArrayCount>50</ArrayCount>
                <ZType>ZType_Array | ZType_Char</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>0</RecordOffset>
              </KeyField>
              <KeyField>
                <Rank>1</Rank>  <!-- not significant -->
                <Name>NumericField</Name>
                <NaturalSize>12</NaturalSize>
                <UniversalSize>15</UniversalSize>
                <ArrayCount>3</ArrayCount>
                <ZType>ZType_Array | ZType_Signed | ZType_Endian | ZType_Float</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>52</RecordOffset>
              </KeyField>
          </ZKDic>
         </IndexRank>
         <IndexRank>
           <Rank>1</Rank> <!-- not significant -->
           <Name>Second Index</Name> <!-- warning modification of this field is dangerous see documentation -->
           <Duplicates>ZST_DUPLICATES</Duplicates> <!-- warning modification of this field is dangerous see documentation -->
           <AutoRebuild>false</AutoRebuild>
           <ZKDic>
               <NaturalSize>4</NaturalSize>   <!-- not modifiable -->
               <UniversalSize>5</UniversalSize> <!-- not modifiable -->
              <KeyField>
                <Rank>0</Rank>  <!-- not significant -->
                <Name>Type</Name>
                <NaturalSize>4</NaturalSize>
                <UniversalSize>5</UniversalSize>
                <ArrayCount>1</ArrayCount>
                <ZType>ZType_Enum| ZType_Signed | ZType_Endian | ZType_S32</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>64</RecordOffset>
              </KeyField>
          </ZKDic>
         </IndexRank>
         </Index>
  </ZMasterControlBlock>
</File>
</zicm>
  */


//} // namespace zbs
/** @cond Development */
#ifdef __COMMENT__
/**
 * @brief getChildElementValue gets from Node pNodeWork the content into pContent of a child node Element with name pTagName
 * @param wNodeWork father node to get the child element from
 * @param wTagName tag name of the child node to get the content from
 * @param wContent QString to receive the content of the element
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
getChildElementValue(zxmlNode& pNodeWork,const char*pTagName,QString &pContent, bool pMandatory)
{
  zxmlNode* wNodeDetail;
  zxmlElement* wElement;
  QString wName;

  ZStatus wSt = pNodeWork.getChildByName(wNodeDetail,pTagName);
  if (wSt!=ZS_SUCCESS)
  {
    if (pMandatory)
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_XMLERROR,
          Severity_Error,
          "xml document may be corrupted or incomplete.Expected <%s> child to <%s> tag : child not found.",
          pTagName,
          pNodeWork.getName().toCChar());
      return ZS_XMLERROR;
    }
    else
      return ZS_NOTFOUND;
  }
  wElement=(zxmlElement*)wNodeDetail;
  wName=wNodeDetail->getName().toCChar();
  pContent=wElement->getFirstText();
  if (pContent.isEmpty())
  {
    if (pMandatory)
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_XMLERROR,
          Severity_Error,
          "Warning: xml document may be incomplete.Expected <%s> child to <%s> tag : child content is empty.",
          pTagName,
          pNodeWork.toElement().tagName().toStdString().c_str());
      return ZS_XMLERROR;
    }
    else
      return ZS_NOTFOUND;
  }
  return ZS_SUCCESS;
} // getChildElementValue
/**
 * @brief getChildElementValue gets from Node pNodeWork the content into pContent of a child node Element with name pTagName
 * @param wNodeWork father node to get the child element from
 * @param wTagName tag name of the child node to get the content from
 * @param wContent QString to receive the content of the element
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
getChildElementValue(QDomNode pNodeWork,const char*pTagName,QString &pContent, bool pMandatory)
{ 
QDomNode wNodeDetail;
QDomElement wElement;
QString wName;

    wNodeDetail = pNodeWork.firstChildElement(pTagName);
    if (wNodeDetail.isNull())
        {
        if (pMandatory)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "xml document may be corrupted or incomplete.Expected <%s> child to <%s> tag : child not found.",
                                    pTagName,
                                    pNodeWork.toElement().tagName().toStdString().c_str());
            return ZS_XMLERROR;
            }
        else
            return ZS_NOTFOUND;
        }
    wElement=wNodeDetail.toElement();
    wName=wNodeDetail.toElement().tagName();
    pContent=wElement.text();
    if (pContent.isEmpty())
        {
        if (pMandatory)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "Warning: xml document may be incomplete.Expected <%s> child to <%s> tag : child content is empty.",
                                    pTagName,
                                    pNodeWork.toElement().tagName().toStdString().c_str());
            return ZS_XMLERROR;
            }
        else
            return ZS_NOTFOUND;
        }
   return ZS_SUCCESS;
} // getChildElementValue
ZStatus
getChildElementValue(QDomNode pNodeWork,const utf8_t*pTagName,QString &pContent, bool pMandatory)
{
    return getChildElementValue(pNodeWork,(const char*)pTagName,pContent, pMandatory);
}
#endif // __COMMENT__
/**
 * @brief _testXMLZFileControlBlock
 * @param pFCB
 * @param pFirstNode
 * @param pMissingTags
 * @param pRealRun
 * @param pOutput
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
_testXMLZFileControlBlock(ZFileControlBlock *pFCB,
                          zxmlNode *pFileDescNode,
                          long &pMissingTags,
                          bool pRealRun,
                          FILE* pOutput,
                          ZaiErrors* pErrorLog)
{
ZStatus wSt;
QString wContent;

zxmlNode* wFCBNode=nullptr;
//zxmlNode* wFileDescNode=nullptr;
utf8String wMessage;
utf8String wTagName;
//--------------ZFileControlBlock modifiable fields----------------

    pErrorLog->setErrorLogContext("_testXMLZFileControlBlock");
    pErrorLog->setAutoPrintOn(true);
/*
    wSt=pFileNode->getChildByName(wFileDescNode,"ZFileDescriptor");
    if (wSt!=ZS_SUCCESS)
        {
          pErrorLog->errorLog("xml document may be corrupted or incomplete.Expected <ZFileDescriptor>.");
          ZException.setMessage(_GET_FUNCTION_NAME_,
              wSt,
              Severity_Error,
              "xml document may be corrupted or incomplete.Expected <ZFileDescriptor>.");
          return wSt;
          }
*/
    fprintf (pOutput,"%s>> processing <ZFileControlBlock>\n",_GET_FUNCTION_NAME_);

    wSt=pFileDescNode->getChildByName(wFCBNode,"ZFileControlBlock");
//       wNodeWork = wFileDescNode.firstChildElement("ZFileControlBlock");
    if (wSt!=ZS_SUCCESS)
        {
        pErrorLog->errorLog("xml document may be corrupted or incomplete.Expected <ZFileControlBlock>.");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                   wSt,
                                   Severity_Error,
                                   "xml document may be corrupted or incomplete.Expected <ZFileControlBlock> child to <File> tag : no child found.");
        return wSt; // we must explore all document before returning
        }

    if (XMLgetChildULong((zxmlElement*)wFCBNode,"AllocatedBlocks",pFCB->AllocatedBlocks,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "AllocatedBlocks");
             pMissingTags ++;
           }
    if (XMLgetChildULong((zxmlElement*)wFCBNode,"BlockExtentQuota",pFCB->BlockExtentQuota,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "BlockExtendQuota");
             pMissingTags ++;
           }
    if (XMLgetChildULong((zxmlElement*)wFCBNode,"BlockExtentQuota",pFCB->BlockExtentQuota,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "BlockExtendQuota");
             pMissingTags ++;
           }
    unsigned long wL;
    if (XMLgetChildULong((zxmlElement*)wFCBNode,"InitialSize",wL,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "InitialSize");
             pMissingTags ++;
           }
           else
             pFCB->InitialSize= wL;

    if (XMLgetChildULong((zxmlElement*)wFCBNode,"BlockTargetSize",pFCB->BlockTargetSize,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "BlockTargetSize");
             pMissingTags ++;
           }
    bool wBool;
    if (XMLgetChildBool((zxmlElement*)wFCBNode,"HighwaterMarking",wBool,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "HighwaterMarking");
             pMissingTags ++;
           }
           else
             pFCB->HighwaterMarking=wBool;

    if (XMLgetChildBool((zxmlElement*)wFCBNode,"GrabFreeSpace",wBool,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "GrabFreeSpace");
             pMissingTags ++;
           }
           else
             pFCB->GrabFreeSpace=wBool;

    XMLderegister(wFCBNode);
    //--------------End ZFileControlBlock modifiable fields----------------
   return ZS_SUCCESS;
}//_testXMLZFileControlBlock

/**
 * @brief _getXMLZFileDescriptor analyze ZFilecontrolBlock from xml definition, controls it, and return a pURIContent
 * @note an error here must end the processing
 *
 *
 * @param[in] pFistNode
 * @param[in] pFilePath     content file path. if omitted, the URIContent field from xml definition will be taken
 * @param[in] pZFile_type   type of file for validating xml definition
 * @param[out] pURIContent  selected content file path
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus _testXMLZFileDescriptor (zxmlNode *pFileNode,
                                const utf8_t *pFilePath,
                                const utf8_t *pZFile_type,
                                uriString &pURIContent,
                                FILE* pOutput,
                                ZaiErrors* pErrorLog)
{
ZStatus wSt;

//QString wContent;
utf8String wContent;
zxmlNode * wFileDescNode=nullptr;
zxmlNode * wHCBNode=nullptr;

pErrorLog->setAutoPrintOn(true);
pErrorLog->setErrorLogContext("_testXMLZFileDescriptor");

    fprintf (pOutput,"%s>> processing node <ZFileDescriptor>  \n",
             _GET_FUNCTION_NAME_);

    wSt=pFileNode->getChildByName(wFileDescNode,"ZFileDescriptor");
    if (wSt!=ZS_SUCCESS)
    {
      pErrorLog->errorLog("xml document may be corrupted or incomplete.Expected <ZFileDescriptor>.");
      ZException.setMessage(_GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          "xml document may be corrupted or incomplete.Expected <ZFileDescriptor>.");
      return wSt;
    }

    wSt=wFileDescNode->getChildByName(wHCBNode,"ZHeaderControlBlock");
    if (wSt!=ZS_SUCCESS)
    {
      pErrorLog->errorLog("   ****Error*** Missing <ZHeaderControlBlock> ******");
      ZException.setMessage(_GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          "xml document may be corrupted or incomplete.Expected <ZHeaderControlBlock> child to <ZFileDescriptor> tag : no child found.");

      return wSt;
    }

    if (XMLgetChildText((zxmlElement *)wHCBNode,"FileType",wContent,pErrorLog))
      {
        pErrorLog->errorLog("   ****Error*** Missing <FileType> ******");
        ZException.setMessage(_GET_FUNCTION_NAME_,
            ZS_INVTYPE,
            Severity_Error,
            "xml document may be corrupted or incomplete.Expected <FileType> child to <ZHeaderControlBlock> tag : no child found.");
        return(ZS_INVTYPE);
      }// !ZS_SUCCESS

    if (wContent!=pZFile_type)
        {
        pErrorLog->errorLog("   ****Error*** Invalid <FileType> ******");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Error,
                                "Invalid <FileType> must be <%s> found <%s>.",
                                pZFile_type,
                                wContent.toCChar());
        return(ZS_INVTYPE);
        }


    if (pFilePath==nullptr)
            {
            if (XMLgetChildText((zxmlElement *)wHCBNode,"URIContent",wContent,pErrorLog))
            {
              pErrorLog->errorLog("No file path has been specified and xml file does not contain a valid <URIContent> field.");
              ZException.setMessage(_GET_FUNCTION_NAME_,
                  ZS_NOTFOUND,
                  Severity_Error,
                  "No file path has been specified and xml file does not contain a valid <URIContent> field.");
              return(ZS_NOTFOUND);
            }// !ZS_SUCCESS
            pURIContent=wContent.toCChar() ;
            }
        else
            {
            pURIContent = pFilePath ;
            }
    return wSt;
}// _testXMLZFileDescriptor
/** @endcond */ // Development

#endif // ZMASTERFILE_UTILITIES_CPP
