#ifndef ZMASTERFILE_UTILITIES_CPP
#define ZMASTERFILE_UTILITIES_CPP
#include <zindexedfile/zmasterfile_utilities.h>
#include <zindexedfile/zindexcollection.h>
#include <zindexedfile/zsmasterfile.h>
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
 * @return a descString with the appropriate ZSIndexFile root name
 */
utfdescString& generateIndexRootName(utfdescString& wIndexRootName,
                                    utfdescString &pMasterRootName,
                                     const long pRank,
                                     utffieldNameString &pIndexName)
{
    wIndexRootName = pMasterRootName;
    wIndexRootName += (utf8_t)'-';
    if (pIndexName.isEmpty())
                {
                wIndexRootName+="index";
                }
            else
                {
                wIndexRootName += pIndexName.toString();
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
                 utffieldNameString& pIndexName)
{
uriString  wPath_Uri;
utfdescString wMasterName;
utfdescString wMasterExt;


    if (pDirectory.isEmpty())
            wPath_Uri = pMasterFileUri.getDirectoryPath().toCChar();
        else
            wPath_Uri = pDirectory.toString();

    QUrl wUrl(wPath_Uri.toCString_Strait());
    pZSIndexFileUri.fromQString(wUrl.toString(QUrl::PreferLocalFile));
    pZSIndexFileUri.addConditionalDirectoryDelimiter() ;

    wMasterName = pMasterFileUri.getBasename();
    wMasterExt=pMasterFileUri.getFileExtension();
    const utf8_t* wExt=(const utf8_t*)__ZINDEX_FILEEXTENSION__;
    wExt++;                             // skip the '.' char
    if (wMasterExt==wExt)
                {
                ZException.setMessage (_GET_FUNCTION_NAME_,
                                         ZS_INVNAME,
                                         Severity_Error,
                                         " Invalid ZSMasterFile name extension %s for name %s",
                                         wMasterExt.toString(),
                                         pMasterFileUri.toString()
                                         );
                return ZS_INVNAME;
                }

    utfdescString wM;
    wM=generateIndexRootName(wM,wMasterName,pRank,pIndexName);
    pZSIndexFileUri += wM;

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
                          QDomNode pFirstNode,
                          long &pMissingTags,
                          bool pRealRun,
                          FILE*pOutput)
{
ZStatus wSt;
QString wContent;

QDomNode wNodeWork;
QDomNode wSecondlevelNode;
utfmessageString wMessage;
utfdescString wTagName;
//--------------ZFileControlBlock modifiable fields----------------

        wSecondlevelNode = pFirstNode.firstChildElement("ZFileDescriptor");

        fprintf (pOutput,"%s>> processing <ZFileControlBlock>\n",
                 _GET_FUNCTION_NAME_);

       wNodeWork = wSecondlevelNode.firstChildElement("ZFileControlBlock");
       if (wNodeWork.isNull())
           {
           ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_XMLERROR,
                                   Severity_Error,
                                   "xml document may be corrupted or incomplete.Expected <ZFileControlBlock> child to <File> tag : no child found.");
           return ZS_XMLERROR; // we must explore all document before returning
           }


       wTagName = (const utf8_t*)"BlockExtentQuota";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {

       wMessage.sprintf("       <%s>  ",
                    wTagName.toString());
       if (pFCB->BlockExtentQuota==wContent.toLong())
                   wMessage.addsprintf( " <%ld> ==unchanged== \n",
                                 pFCB->BlockExtentQuota);
               else
               {
               wMessage.addsprintf(" current <%ld> modified to <%ld>\n",
                            pFCB->BlockExtentQuota,
                            wContent.toLong());
               if (pRealRun)
                    {
                    pFCB->BlockExtentQuota = wContent.toLong();
                    }
               }
       fprintf(pOutput,wMessage.toCString_Strait());
       }// else field----------------------------------------------------

       wTagName = (const utf8_t*)"InitialSize";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.sprintf("       <%s>  ",
                    wTagName.toString());
       if (pFCB->InitialSize==wContent.toLong())
                   wMessage.addsprintf( " <%ld> ==unchanged== \n",
                                 pFCB->InitialSize);
               else
                {
                   wMessage.addsprintf(" current <%ld> modified to <%ld>\n",
                                pFCB->InitialSize,
                                wContent.toLong());
               if (pRealRun)
                    {
                    pFCB->InitialSize = wContent.toLong();
                    }
                }
       fprintf(pOutput,wMessage.toCString_Strait());
       }// else field----------------------------------------------------

       wTagName = (const utf8_t*)"BlockTargetSize";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.sprintf("       <%s>  ",
                    wTagName.toString());
       if (pFCB->BlockTargetSize==wContent.toLong())
                   wMessage.addsprintf( " <%ld> ==unchanged== \n",
                                 pFCB->BlockTargetSize);
               else
                {
                   wMessage.addsprintf(" current <%ld> modified to <%ld>\n",
                                pFCB->BlockTargetSize,
                                wContent.toLong());
               if (pRealRun)
                    {
                    pFCB->BlockTargetSize = wContent.toLong();
                    }
                }
       fprintf(pOutput,wMessage.toCString_Strait());
       }// else field----------------------------------------------------

       wTagName = (const utf8_t*)"HighwaterMarking";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.sprintf("       parameter <%s>  ",
                    wTagName.toString());
       if (pFCB->HighwaterMarking==(wContent=="true"))
                   wMessage.addsprintf( " <%s> ==unchanged== \n",
                                 pFCB->HighwaterMarking?"true":"false");
               else
                    {
                   wMessage.addsprintf(" current <%s> modified to <%s>\n",
                                pFCB->HighwaterMarking?"true":"false",
                                wContent=="true"?"true":"false");
                   if (pRealRun)
                        {
                        pFCB->HighwaterMarking = (wContent=="true");
                        }
                    }
       fprintf(pOutput,wMessage.toCString_Strait());
       }// else field----------------------------------------------------

       wTagName = (const utf8_t*)"GrabFreeSpace";
       wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
       if (wSt!=ZS_SUCCESS)
           {
           fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                   _GET_FUNCTION_NAME_,
                   wTagName.toString());
           pMissingTags ++;
           }
       else
       {
       wMessage.clear();
       wMessage.sprintf("       parameter <%s>  ",
                    wTagName.toString());
       if (pFCB->GrabFreeSpace==(wContent=="true"))
                   wMessage.addsprintf( " <%s> ==unchanged== \n",
                                 pFCB->GrabFreeSpace?"true":"false");
               else
                    {
                   wMessage.addsprintf(" current <%s> modified to <%s>\n",
                                pFCB->GrabFreeSpace?"true":"false",
                                wContent=="true"?"true":"false");
                   if (pRealRun)
                        {
                        pFCB->GrabFreeSpace = (wContent=="true");
                        }
                    }
       fprintf(pOutput,wMessage.toCString_Strait());
       }// else field----------------------------------------------------------

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
ZStatus _testXMLZFileDescriptor (QDomNode pFistNode, const utf8_t *pFilePath, const utf8_t *pZFile_type, uriString &pURIContent, FILE* pOutput)
{
ZStatus wSt;

QString wContent;

QDomNode wSecondlevelNode;
QDomNode wThirdlevelNode;

    fprintf (pOutput,"%s>> processing node <ZFileDescriptor>  \n",
             _GET_FUNCTION_NAME_);

    wSecondlevelNode = pFistNode.firstChildElement("ZFileDescriptor");
    if (wSecondlevelNode.isNull())
        {
        fprintf(pOutput,"   ****Error*** Missing <ZFileDescriptor> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <ZFileControlBlock> child to <File> tag : no child found.");
        return(ZS_XMLERROR);
        }// isNull

    wThirdlevelNode = wSecondlevelNode.firstChildElement("ZHeaderControlBlock");
    if (wThirdlevelNode.isNull())
        {
        fprintf(pOutput,"   ****Error*** Missing <ZHeaderControlBlock> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <ZHeaderControlBlock> child to <ZFileDescriptor> tag : no child found.");
        return(ZS_XMLERROR);
        }// isNull

    wSt=getChildElementValue(wThirdlevelNode,"FileType",wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(pOutput,"   ****Error*** Missing <FileType> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <FileType> child to <ZHeaderControlBlock> tag : no child found.");
        return(ZS_XMLERROR);
        }// !ZS_SUCCESS
    if (wContent!=(const char*)pZFile_type)
        {
        fprintf(pOutput,"   ****Error*** Invalid <FileType> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "Invalid <FileType> must be <%s> found <%s>.",
                                pZFile_type,
                                wContent.toStdString().c_str());
        return(ZS_XMLERROR);
        }


    if (pFilePath==nullptr)
            {
            wSt=getChildElementValue(wSecondlevelNode,"URIContent",wContent);
            if (wSt!=ZS_SUCCESS)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "No file path has been specified and xml file does not contain a valid <URIContent> field.",
                                        wContent.toStdString().c_str());
                                        ZException.exit_abort();
                }
            pURIContent.fromQString( wContent) ;
            }
        else
            {
            pURIContent = pFilePath ;
            }
    return wSt;
}// _testXMLZFileDescriptor
/** @endcond */ // Development

#endif // ZMASTERFILE_UTILITIES_CPP
