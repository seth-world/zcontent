#ifndef ZMASTERFILE_UTILITIES_CPP
#define ZMASTERFILE_UTILITIES_CPP

#include <zindexedfile/zmasterfile_utilities.h>

#include <zindexedfile/zindexcollection.h>
#include <zindexedfile/zrawmasterfile.h>
#include <zindexedfile/zsmasterfile.h>
#include <zxml/zxml.h>
#include <zxml/zxmlprimitives.h>
#include <ztoolset/zaierrors.h>
#include <zindexedfile/zmfdictionary.h>
#include <zindexedfile/zkeydictionary.h>
#include <zindexedfile/zsjournalcontrolblock.h>

using namespace zbs;

/*


<indexcontrolblock> <!-- no dictionary in index control block -->
    <indexname> </indexname>
    <keyuniversalsize> </keyuniversalsize>
    <duplicates> </duplicates>
</indexcontrolblock>

*/



/*

<!-- zrandomfile descriptor -->

    <zfiledescriptor>
    <uricontent> </uricontent>
    <uriheader> </uriheader>
    <uridirectorypath> </uridirectorypath>

    <zfdowndata> <!--see if it is required in xml -->
    <physicalposition> </physicalposition>
    <logicalposition> </logicalposition>
    <currentrank> </currentrank>
    </zfdowndata> <!-- en see if it is required in xml -->

    <zheadercontrolblock>
    <filetype> </filetype>
    <offsetfcb> </offsetfcb>
    <offsetreserved> </offsetreserved>
    <sizereserved> </sizereserved>
    </zheadercontrolblock>

    <zfilecontrolblock>
    <startofdata> </startofdata>
    <allocatedblocks> </allocatedblocks>
    <blockextentquota> </blockextentquota>
    <zbatdataoffset> </zbatdataoffset>
    <zbatexportsize> </zbatexportsize>
    <zfbtdataoffset> </zfbtdataoffset>
    <zfbtexportsize> </zfbtexportsize>
    <zdbtdataoffset> </zdbtdataoffset>
    <zdbtexportsize> </zdbtexportsize>
    <zreserveddataoffset> </zreserveddataoffset>
    <zreservedexportsize> </zreservedexportsize>
    <initialsize> </initialsize>
    <allocatedsize> </allocatedsize>
    <usedsize> </usedsize>
    <maxsize> </maxsize>
    <blocktargetsize> </blocktargetsize>
    <highwatermarking> </highwatermarking>
    <grabfreespace> </grabfreespace>

    </zfilecontrolblock>

    </zfiledescriptor>


*/


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
utf8String generateIndexRootName(const utf8String &pMasterRootName,
                                 const long pRank,
                                 const utf8String &pIndexName)
{
  utf8String wIndexRootName;
    wIndexRootName = pMasterRootName;
//    wIndexRootName += (utf8_t)'-';
    wIndexRootName.addUtfUnit('-');
    if (pIndexName.isEmpty())
                {
                wIndexRootName+="index";
                }
            else
                {
                wIndexRootName += pIndexName.toCChar();
                }
//    wIndexRootName += (utf8_t)'-';
    wIndexRootName.addUtfUnit('-');
    wIndexRootName.addsprintf("%02ld",pRank);
//    sprintf(&DSRootName.content[DSRootName.size()],"%02ld",pRank);

//    wIndexRootName.eliminateChar(' ');
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
generateIndexURI(uriString &pZSIndexFileUri,
                 const uriString pMasterFileUri,
                 const uriString &pDirectory,
                 const long pRank,
                 const utf8String& pIndexName)
{
uriString  wPath_Uri;
utf8String wMasterRoot;
utf8String wMasterExt;


    if (pDirectory.isEmpty())
      wPath_Uri = pMasterFileUri.getDirectoryPath();
        else
            wPath_Uri = pDirectory;

    QUrl wUrl(wPath_Uri.toCChar());
    pZSIndexFileUri.fromQString(wUrl.toString(QUrl::PreferLocalFile));
    pZSIndexFileUri.addConditionalDirectoryDelimiter() ;

    wMasterRoot = pMasterFileUri.getRootname();
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
    wM=generateIndexRootName(wMasterRoot,pRank,pIndexName);
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
<file>
  <filetype>ZFT_ZRawMasterFile</filetype>
  <zfiledescriptor>
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
   <zfilecontrolblock>
      <AllocatedBlocks>15</AllocatedBlocks> <!-- not modifiable -->
      <BlockExtentQuota>5</BlockExtentQuota>
      <InitialSize>100000</InitialSize>
      <BlockTargetSize>1</BlockTargetSize>
      <HighwaterMarking>false</HighwaterMarking>
      <GrabFreeSpace>true</GrabFreeSpace>
   </zfilecontrolblock>
  </zfiledescriptor>
  <zmastercontrolblock>
     <historyon>false</historyon>
     <JournalingOn>true</JournalingOn>
     <IndexFileDirectoryPath></IndexFileDirectoryPath>
      <zmasterdictionary>
      <metadic>
        <dicfields>
          <field>
            <name>%s</name>
    <!-- if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if atomic -->
            <capacity>%d</capacity>
            <headersize>%ld</headersize>
            <naturalsize>%ld</naturalsize>
            <universalsize>%ld</universalsize>
            <ztype>%s</ztype>   <!-- see ZType_type definition : converted to its value number -->
            <hash>%s</hash>
          </field>
        </dicfields>
      </metadic>

      </zmasterdictionary>



      <indextable>
        <index>
          <indexcontrolblock> <!-- no dictionary in index control block -->
            <indexname> </indexname>
            <keyuniversalsize> </keyuniversalsize>
            <duplicates> </duplicates>
          </indexcontrolblock>
          <file> <!-- from ZIndexTable >
            <zfiledescriptor>
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
              <zfilecontrolblock>
                <AllocatedBlocks>15</AllocatedBlocks> <!-- not modifiable -->
                <BlockExtentQuota>5</BlockExtentQuota>ZSKeyDictionary
                <InitialSize>100000</InitialSize>
                <BlockTargetSize>1</BlockTargetSize>
                <HighwaterMarking>false</HighwaterMarking>
                <GrabFreeSpace>true</GrabFreeSpace>
              </zfilecontrolblock>
            </zfiledescriptor>
          </file>
        </index>
      </indextable>


         <!-- Changing IndexRank position in a file description will imply huge modifications -->

         <indextable>
            <indexcontrolblock>
              <indexname> </indexname>
              <keyuniversalsize> </keyuniversalsize>
              <duplicates>false</duplicates>
            </indexcontrolblock>


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
         </indextable>
  </zmastercontrolblock>
</file>
</zicm>

*/

//} // namespace zbs
/** @cond Development */

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
    fprintf (pOutput,"%s>> processing <zfilecontrolblock>\n",_GET_FUNCTION_NAME_);

    wSt=pFileDescNode->getChildByName(wFCBNode,"zfilecontrolblock");
//       wNodeWork = wFileDescNode.firstChildElement("ZFileControlBlock");
    if (wSt!=ZS_SUCCESS)
        {
        pErrorLog->errorLog("xml document may be corrupted or incomplete.Expected <zfilecontrolblock>.");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                   wSt,
                                   Severity_Error,
                                   "xml document may be corrupted or incomplete.Expected <zfilecontrolblock> child to <File> tag : no child found.");
        return wSt; // we must explore all document before returning
        }

    if (XMLgetChildULong((zxmlElement*)wFCBNode,"allocatedblocks",pFCB->AllocatedBlocks,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "allocatedblocks");
             pMissingTags ++;
           }
    if (XMLgetChildULong((zxmlElement*)wFCBNode,"blockextentquota",pFCB->BlockExtentQuota,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "blockextentquota");
             pMissingTags ++;
           }
    unsigned long wL;
    if (XMLgetChildULong((zxmlElement*)wFCBNode,"initialsize",wL,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "initialsize");
             pMissingTags ++;
           }
           else
             pFCB->InitialSize= wL;

    if (XMLgetChildULong((zxmlElement*)wFCBNode,"blocktargetsize",pFCB->BlockTargetSize,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "blocktargetsize");
             pMissingTags ++;
           }
    bool wBool;
    if (XMLgetChildBool((zxmlElement*)wFCBNode,"highwatermarking",wBool,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "highwatermarking");
             pMissingTags ++;
           }
           else
             pFCB->HighwaterMarking=wBool;

    if (XMLgetChildBool((zxmlElement*)wFCBNode,"grabfreespace",wBool,pErrorLog)<0)
           {
             fprintf(pOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                 _GET_FUNCTION_NAME_,
                 "grabfreespace");
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
                                const char *pFilePath,
                                const char *pZFile_type,
                                uriString &pURIContent,
                                FILE* pOutput,
                                ZaiErrors* pErrorLog)
{
ZStatus wSt;

//QString wContent;
utf8String wContent;
zxmlNode * wFileDescNode=nullptr;
zxmlNode * wHCBNode=nullptr;

ZFileDescriptor wFileDescriptor;

pErrorLog->setAutoPrintOn(true);
pErrorLog->setErrorLogContext("_testXMLZFileDescriptor");

    if (pFileNode->getName()!="file")
      {
      pErrorLog->errorLog("_testXMLZFileDescriptor-E-XMLINVROOTNAME Invalid root node <%s> while expected <file>.",pFileNode->getName().toCChar());
      return ZS_XMLINVROOTNAME;
      }

    fprintf (pOutput,"%s>> processing node <zfiledescriptor>  \n",
             _GET_FUNCTION_NAME_);

    wSt=pFileNode->getChildByName(wFileDescNode,"zfiledescriptor");
    if (wSt!=ZS_SUCCESS)
    {
      pErrorLog->errorLog("xml document may be corrupted or incomplete.Expected <zfiledescriptor>.");
      ZException.setMessage(_GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          "xml document may be corrupted or incomplete.Expected <zfiledescriptor>.");
      return wSt;
    }


    wSt=wFileDescNode->getChildByName(wHCBNode,"zheadercontrolblock");
    if (wSt!=ZS_SUCCESS)
    {
      pErrorLog->errorLog("   ****Error*** Missing <zheadercontrolblock> ******");
      ZException.setMessage(_GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          "xml document may be corrupted or incomplete.Expected <ZHeaderControlBlock> child to <ZFileDescriptor> tag : no child found.");

      return wSt;
    }

    if (XMLgetChildText((zxmlElement *)wHCBNode,"filetype",wContent,pErrorLog))
      {
        pErrorLog->errorLog("   ****Error*** Missing <filetype> ******");
        ZException.setMessage(_GET_FUNCTION_NAME_,
            ZS_INVTYPE,
            Severity_Error,
            "xml document may be corrupted or incomplete.Expected <filetype> child to <zheadercontrolblock> tag : no child found.");
        return(ZS_INVTYPE);
      }// !ZS_SUCCESS

    if (wContent!=pZFile_type)
        {
        pErrorLog->errorLog("   ****Error*** Invalid <filetype> ******");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Error,
                                "Invalid <filetype> must be <%s> found <%s>.",
                                pZFile_type,
                                wContent.toCChar());
        return(ZS_INVTYPE);
        }


    if (pFilePath==nullptr)
            {
            if (XMLgetChildText((zxmlElement *)wHCBNode,"uricontent",wContent,pErrorLog))
            {
              pErrorLog->errorLog("No file path has been specified and xml file does not contain a valid <uricontent> field.");
              ZException.setMessage(_GET_FUNCTION_NAME_,
                  ZS_NOTFOUND,
                  Severity_Error,
                  "No file path has been specified and xml file does not contain a valid <uricontent> field.");
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

/*
<?xml version='1.0' encoding='UTF-8'?>
<zicm version="2.00">
<file>
  <filetype> </filetype> <-- File type is ZFT_ZMasterFile i. e. ZRawMasterFile or ZSMasterFile -->
  <zfiledescriptor>
      <uricontent>/home/gerard/testindexedpictures.zmf</uricontent>
      <uriheader>/home/gerard/testindexedpictures.zrh</uriheader> <!-- FYI : not modifiable generated by ZRandomFile from uricontent -->
      <uridirectorypath>/home/gerard/</uridirectorypath> <!-- FYI : not modifiable generated by ZRandomFile-->
      <zheadercontrolblock> <!-- FYI : all children fields of zheadercontrolblock are not modifiable -->
          <filetype>ZFT_ZSMasterFile</filetype>
          <zrfversion>2000</zrfversion>
          <offsetfcb>4698</offsetfcb>
          <offsetreserved>64</offsetreserved>
          <sizereserved>4634</sizereserved>
      </zheadercontrolblock>
   <zfilecontrolblock>
      <startofdata>nnnn</startofdata>  <!-- FYI : offset where Data storage starts -->
      <allocatedblocks>15</allocatedblocks> <!-- FYI: for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT -->
      <blockextentquota>5</blockextentquota> <!--for ZBAT & ZFBT : initial extension quota -->
      <initialsize>100000</initialsize> <-- Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk -->
      <allocatedsize>100000</allocatedsize> <--  FYI: Total current allocated size in bytes for file -->
      <usedsize>100000</usedsize>  <!-- FYI : Total of currently used size within file in bytes -->
      <minsize>1</minsize> <!-- FYI - statistical value : minimum length of block record in file -->
      <maxsize>1</maxsize> <!-- FYI - statistical value : maximum length of block record in file -->
      <blocktargetsize>1</blocktargetsize> <!-- (user defined value) Foreseen medium size of blocks in a varying block context. -->
      <highwatermarking>false</highwatermarking> <!-- if set, marks to zero the whole deleted block content when removed. This option adds disk access  -->
      <grabfreespace>true</grabfreespace> <!-- if set, attempts to grab free space and holes at each block free operation -->
   </zfilecontrolblock>
  </zfiledescriptor>

  <zmastercontrolblock>
    <indexfilepath></indexfilepath> <!-- Directory path for index files. If empty, then directory is the extracted from master file content URI -->
    <rank>n</rank>
    <historyon>false</historyon> <!-- historyon is not currently used -->
    <indexcount>n</indexcount>
    <indextable>
      <index>
        <!-- from ZSIndexControlTable >
        <indexcontrolblock> <!-- no dictionary in index control block -->
          <indexname> </indexname>
          <keyuniversalsize> </keyuniversalsize>
          <duplicates> </duplicates>
        </indexcontrolblock>
        <file> <!-- from ZIndexTable >
           <zfiledescriptor>
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
           <zfilecontrolblock>
              <AllocatedBlocks>15</AllocatedBlocks> <!-- not modifiable -->
              <BlockExtentQuota>5</BlockExtentQuota>ZSKeyDictionary
              <InitialSize>100000</InitialSize>
              <BlockTargetSize>1</BlockTargetSize>
              <HighwaterMarking>false</HighwaterMarking>
              <GrabFreeSpace>true</GrabFreeSpace>
           </zfilecontrolblock>
          </zfiledescriptor>
        </file>
      </index>

    </indextable>

    <!-- optional zjournalcontrolblock : if exists then journaling has been defined for this file : if not no journaling-->
    <zjournalcontrolblock>
      <journalingon>true</journalingon>
      <keep>true</keep>
      <journallocaldirectorypath></journallocaldirectorypath> <!-- if omitted : same directory as ZRawMasterFile's path  -->
      <depth>-1</depth> <!-- max number of journaling records to keep. -1 : infinite. in this case need to use purgeJournaling(keepranks) regularly -->
      <!-- optional zremotemirroring : if exists then remote mirroring will be done -->
      <zremotemirroring>
        <protocol>ZJP_RPC|ZJP_SSLV5</protocol>
        <addresstype>ZHAT_IPV4</addresstype> <!-- values ZHAT_IPV4 ZHAT_IPV6-->
        <port> 1156 </port>
        <host>JournalServer</host>

        <authenticate> <authenticate>
        <!-- optional SSL encryption : if exists then SSL Key and Vector will be generated accordingly-->
        <sslkey>  </sslkey>
        <sslvector> </sslvector>
        </zremotemirroring>
      </zjournalcontrolblock>

      <zmasterdictionary>
        <keycount>n</keycount>
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
                  <mdicrank>nn</mdicrank>
                  <hash>hhhhhhhhhh</hash> <--! hexa value -->
                  <keyoffset>nnnn</keyoffset>

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


        <!-- Changing IndexRank position in a file description will imply huge modifications -->

      <indextable>
        <indexcontrolblock>
          <indexname> </indexname>
          <keyuniversalsize> </keyuniversalsize>
          <duplicates>false</duplicates>
        </indexcontrolblock>
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
        </indextable>
      </zmastercontrolblock>
    </file>
  </zicm>
*/




/** Xml get field helpers */

ZStatus
zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,bool& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity)
{
  bool wInitial=pValue;
  if (XMLgetChildBool(pRootNode,pName,pValue,pErrorlog)<0)
    {
    if (!pMandatory)
    {
      pErrorlog->log(pSeverity,"failed to get optional parameter <%s>. Default value <%s> will be used.\n",
          pName,
          pValue?"true":"false");
      return ZS_XMLWARNING;
    }
    pErrorlog->log(pSeverity,"failed to get mandatory parameter <%s>.\n", pName);
    return ZS_XMLMISSREQ;
    }

  if (wInitial==pValue)
    pErrorlog->textLog(" got parameter <%s> value <%s> ===unchanged===.\n",pName,pValue?"true":"false");
  else
    pErrorlog->textLog(" got parameter <%s> value <%s> former value <%s>.\n",pName,pValue?"true":"false",wInitial?"true":"false");
  return ZS_SUCCESS;
}


ZStatus
zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,utf8String& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity)
{
  utf8String wInitial=pValue;
  if (XMLgetChildText(pRootNode,pName,pValue,pErrorlog)<0)
    {
    if (!pMandatory)
    {
      pErrorlog->log(pSeverity,"failed to get optional parameter <%s>. Default value <%s> will be used.\n",pName,pValue.toCChar());
      return ZS_XMLWARNING;
    }
    pErrorlog->log(pSeverity,"failed to get mandatory parameter <%s>.\n", pName);
    return ZS_XMLMISSREQ;
    }

  if (wInitial==pValue)
    pErrorlog->textLog(" got parameter <%s> value <%s> ===unchanged===.\n",pName,pValue.toCChar());
  else
    pErrorlog->textLog(" got parameter <%s> value <%s> former value <%s>.\n",pName,pValue.toCChar(),wInitial.toCChar());
  return ZS_SUCCESS;
}

ZStatus
zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,int& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity)
{
  int wInitial=pValue;
  if (XMLgetChildInt(pRootNode,pName,pValue,pErrorlog)<0)
    {
    if (!pMandatory)
    {
      pErrorlog->log(pSeverity,"failed to get optional parameter <%s>. Default value <%d> will be used.\n",pName,pValue);
      return ZS_XMLWARNING;
    }
    pErrorlog->log(pSeverity,"failed to get mandatory parameter <%s>.\n", pName);
    return ZS_XMLMISSREQ;
    }

  if (wInitial==pValue)
    pErrorlog->textLog(" got parameter <%s> value <%d> ===unchanged===.\n",pName,pValue);
  else
    pErrorlog->textLog(" got parameter <%s> value <%d> former value <%d>.\n",pName,pValue,wInitial);
  return ZS_SUCCESS;
}

ZStatus
zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,uint32_t& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity)
{
  uint32_t wInitial=pValue;
  if (XMLgetChildUInt(pRootNode,pName,pValue,pErrorlog)<0)
    {
      if (!pMandatory)
      {
        pErrorlog->log(pSeverity,"failed to get optional parameter <%s>. Default value <%d> will be used.\n",pName,pValue);
        return ZS_XMLWARNING;
      }
    pErrorlog->log(pSeverity,"failed to get mandatory parameter <%s>.\n", pName);
    return ZS_XMLMISSREQ;
    }

  if (wInitial==pValue)
    pErrorlog->textLog(" got parameter <%s> value <%d> ===unchanged===.\n",pName,pValue);
  else
    pErrorlog->textLog(" got parameter <%s> value <%d> former value <%d>.\n",pName,pValue,wInitial);
  return ZS_SUCCESS;
}

ZStatus
zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,md5& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity)
{
  md5 wInitial=pValue;
  if (XMLgetChildMd5(pRootNode,pName,pValue,pErrorlog)<0)
  {
    if (!pMandatory)
    {
      pErrorlog->log(pSeverity,"failed to get optional parameter <%s>. Default value <%s> will be used.\n",pName,pValue.StrHexa);
      return ZS_XMLWARNING;
    }
    pErrorlog->log(pSeverity,"failed to get mandatory parameter <%s>.\n", pName);
    return ZS_XMLMISSREQ;
  }

  if (wInitial==pValue)
    pErrorlog->textLog(" got parameter <%s> value <%s> ===unchanged===.\n",pName,pValue.StrHexa);
  else
    pErrorlog->textLog(" got parameter <%s> value <%s> former value <%s>.\n",pName,pValue.StrHexa,wInitial.StrHexa);
  return ZS_SUCCESS;
}


ZStatus
zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,unsigned long& pValue,ZaiErrors* pErrorlog,bool pMandatory,ZaiE_Severity pSeverity)
{
  unsigned long wInitial=pValue;
  if (XMLgetChildULong(pRootNode,pName,pValue,pErrorlog)<0)
  {
    if (!pMandatory)
    {
      pErrorlog->log(pSeverity,"failed to get optional parameter <%s>. Default value <%ld> will be used.\n",pName,pValue);
      return ZS_XMLWARNING;
    }
    pErrorlog->log(pSeverity,"failed to get mandatory parameter <%s>.\n", pName);
    return ZS_XMLMISSREQ;
  }

  if (wInitial==pValue)
    pErrorlog->textLog(" got parameter <%s> value <%d> ===unchanged===.\n",pName,pValue);
  else
    pErrorlog->textLog(" got parameter <%s> value <%d> former value <%d>.\n",pName,pValue,wInitial);
  return ZS_SUCCESS;
}
ZStatus
zmuXMLgetChild(zxmlElement* pRootNode,const char* pName,long& pValue,bool pMandatory,ZaiErrors* pErrorlog,ZaiE_Severity pSeverity)
{
  long wInitial=pValue;
  if (XMLgetChildLong(pRootNode,pName,pValue,pErrorlog)<0)
  {
    if (!pMandatory)
    {
      pErrorlog->log(pSeverity,"failed to get optional parameter <%s>. Default value <%ld> will be used.\n",pName,pValue);
      return ZS_XMLWARNING;
    }
    pErrorlog->log(pSeverity,"failed to get mandatory parameter <%s>.\n", pName);
    return ZS_XMLMISSREQ;
  }

  if (wInitial==pValue)
    pErrorlog->textLog(" got parameter <%s> value <%d> ===unchanged===.\n",pName,pValue);
  else
    pErrorlog->textLog(" got parameter <%s> value <%d> former value <%d>.\n",pName,pValue,wInitial);
  return ZS_SUCCESS;
}



/**
 * @brief validateXmlDicDefinition controls a full definition for a master dictionary and its associated keys.
 *  This must be done in case (and only in case) of master dictionary creation.
 * @param wMasterDic
 * @param wIndexData
 * @param pErrorLog
 * @return
 */

/*
    error cases
    -----------

1- error: index key has no correspondence with key dictionary definitions  (orphan)
                -> reject whole key dictionary (All key definitions)
                -> keep meta dic
            2- warning : dictionary key has no correspondence with index key
                  -> dictionary key must be suppressed
              3- error: index key and dictionary key has same name but key universal size differs.
                -> dictionary key must be suppressed. ZPRES_Suppress
            4- warning: multiple dictionary keys match one index key over its name (dictionary keys have same Index name)
                  -> surnumerous dictionary key must be suppressed,

    case 5 is already checked and managed when acquiring index control block.
        [5- error: multiple Index key have same IndexName and match one single dictionary key :
                   -> Major error : Index control block is corrupted and master file creation MUST BE rejected.]

        */

ZStatus
validateXmlDicDefinition(ZMFDictionary* pMasterDic,ZArray<IndexData_st>* pIndexData,ZaiErrors* pErrorLog)
{
  ZStatus wSt=ZS_SUCCESS;

  if (pMasterDic==nullptr)
    return ZS_NULLPTR;
  if (pIndexData==nullptr)
    return ZS_NULLPTR;

  pErrorLog->setErrorLogContext("validateXmlDicDefinition");
  pErrorLog->textLog("Check xml dictionary definition against xml index control block definitions.");

  if (pIndexData->count()!=pMasterDic->KeyDic.count())
    {
    wSt=ZS_BADDIC;
    pErrorLog->warningLog("W-BADDIC Indexes number <%ld> is not equal to dictionary key definitions <%ld>\n"
                          "Continuing parsing but dictionary is rejected.",pIndexData->count(),pMasterDic->KeyDic.count());
    }
    uint8_t wICBPres[pIndexData->count()];
    uint8_t wKDicPres[pMasterDic->KeyDic.count()];

    memset (wICBPres,0,pIndexData->count());
    memset (wKDicPres,0,pMasterDic->KeyDic.count());

    for (long wi = 0; wi < pIndexData->count();wi++)
    {
      long wj = 0;
      for (; wj < pMasterDic->KeyDic.count();wj++)
        if (pMasterDic->KeyDic[wj]->DicKeyName==pIndexData->Tab[wi].IndexName)
          break;

      if (wj < pMasterDic->KeyDic.count()) /* found */
      {
        if (wKDicPres[wi] > 0)
        {
          pErrorLog->errorLog( "E-DUPLICATEKEY Two dictionary keys match the same ICB index name <%s> (rank %ld). Duplicate from key dictionary.",
              pIndexData->Tab[wi].IndexName.toCChar(),wi);
          wSt=ZS_DUPLICATEKEY;
        }
        wICBPres[wi]=1;   /* may be referenced more than once (in this case this is an error to be detected */
        wKDicPres[wj]=1;  /* may be referenced more than once (in this case this is an error to be detected */

        /* check key universal size */
        /* compute universal key size from key dictionary */
        uint32_t wKeyUSize=pMasterDic->KeyDic[wj]->computeKeyUniversalSize();
        if (wKeyUSize != pIndexData->Tab[wi].KeyUniversalSize)
        {
          pErrorLog->errorLog( "E-INVSIZE For key <%s> (rank %ld) dictionary key universal size <%d> does not match index size <%d>",
              pIndexData->Tab[wi].IndexName.toCChar(),wi,wKeyUSize,pIndexData->Tab[wi].KeyUniversalSize);
          wICBPres[wi]=ZPRES_Errored;
          wKDicPres[wj]=ZPRES_Errored;
          wSt=ZS_INVSIZE;
        }
        else
          pErrorLog->infoLog( "Index <%s> (rank %ld) dictionary key universal size <%d> checked : OK.",
              pIndexData->Tab[wi].IndexName.toCChar(),wi,wKeyUSize);
      }
      else
      {
        pErrorLog->errorLog(   "E-NOTFOUND ICB Index <%s> (rank %ld) not found within key dictionary.\n",
                              pIndexData->Tab[wi].IndexName.toCChar(),wi);
        wSt=ZS_NOTFOUND;
        wICBPres[wi]=ZPRES_Errored;
/*        while (wMasterDic->KeyDic.count())
          delete wMasterDic->KeyDic.popR();
        return ZS_BADDIC;
*/
      }

    }// for wi

  pErrorLog->popContext();
  return wSt;
}//validateXmlDicDefinition



ZStatus
loadXMLDictionaryForCreate(zxmlElement* pRoot,ZMFDictionary*& pMasterDic,ZaiErrors* pErrorLog)
{
  ZStatus wSt=ZS_SUCCESS;
  zxmlElement* wMasterDicNode=nullptr;
  zxmlElement* wMetaDicNode=nullptr;
  zxmlElement* wKeyDicNode=nullptr;

  ZElementCollection  wIndexElts;
  int                 wMissingTags=0;
  int                 wErroredKeys=0;

  if (pMasterDic!=nullptr)
  {
    pErrorLog->logZStatus(ZAIES_Error,ZS_INVVALUE,"loadXMLDictionary: pMasterDic must be initialized to nullptr by callee.");
    return ZS_INVVALUE;
  }

  /* MCB components :

     *  - IndexTable : array of ZSIndexFile
     *      - ZIndexControlBlock : key size and parameters
     *      - ZRandomFile parameters : target block size is dependent from key size + sizeof address_type
     *
     *
     *  - Master dictionary
     *      - meta dictionary : array of field definitions S
     *      - keyDictionary array
     *            array of keyfields referring to meta dictionary field
     */
  wSt=pRoot->getChildByName((zxmlNode*&) wMasterDicNode,"zmasterdictionary");

  /* looking for <zmasterdictionary>  */

  if (wSt!=ZS_SUCCESS)
  {
    pErrorLog->textLog("loadXMLDictionary: Node <zmasterdictionary> has not been found.");
    //      wMayCreateMasterDic = false;
    return ZS_NOTFOUND;
  }


  pMasterDic=new ZMFDictionary;

  /* loading Master dictionary */
  pErrorLog->textLog("loadXMLDictionary: found node <zmasterdictionary>. Loading master dictionary from its Xml definition.");

  while (true)//=======while true===============
    {
      int wKeyCount=-1; /* default value set to <field omitted> */
      wSt=zmuXMLgetChild(wMasterDicNode,"keycount",wKeyCount,pErrorLog,false,ZAIES_Warning);

      wSt=wMasterDicNode->getChildByName((zxmlNode*&) wMetaDicNode,"metadic");
      if (wSt!=ZS_SUCCESS)
        {
        pErrorLog->logZStatus(ZAIES_Error,ZS_BADDIC,"loadXMLDictionary: Node <metadic> has not been found.\n"
                           "Master dictionary appears to be corrupted.\n "
                           "Master dictionary xml parsing stopped.\n");
        //        wMayCreateMasterDic = false;
        delete pMasterDic;
        pMasterDic=nullptr;
        XMLderegister(wMasterDicNode);
        wSt = ZS_BADDIC;
        goto EndloadXMLDictionary;
        }
      XMLderegister(wMetaDicNode);
      pErrorLog->textLog("loadXMLDictionary: found node <metadic>. Loading meta dictionary.");

      wSt=pMasterDic->ZMetaDic::fromXml(wMasterDicNode,pErrorLog,ZAIES_Warning);
      if (wSt!=ZS_SUCCESS)
        {
        pErrorLog->errorLog("loadXMLDictionary: Meta dictionary (field dictionary) cannot be loaded or appears to be corrupted.\n "
                           "Dictionary Xml parsing stopped.\n");
        delete pMasterDic;
        pMasterDic=nullptr;
        wSt = ZS_XMLERROR;
        goto EndloadXMLDictionary;
        }

      pErrorLog->textLog("loadXMLDictionary: Meta dictionary loaded. <%ld> field definitions loaded.",pMasterDic->count());


    /*-------------------------------
     *  loading key dictionary
     *-------------------------------*/

      wSt=wMasterDicNode->getChildByName((zxmlNode*&) wKeyDicNode,"keydictionary");
      if (wSt==ZS_NOTFOUND)
        {
        pErrorLog->logZStatus(ZAIES_Warning,wSt,  "loadXMLDictionary: Node <keydictionary> has not been found.\n"
                                                  "Master dictionary has no key definitions.\n"
                                                  "File's Key dictionary, if exists, will remain unchanged.");
        goto EndloadXMLDictionary;
        }

      pErrorLog->textLog("loadXMLDictionary: found node <keydictionary>. Loading key dictionary.");
      {// instruction block
        ZElementCollection wKeyNodes = wKeyDicNode->getAllChildren("key");

        if (wKeyNodes.count()==0)
          {
          pErrorLog->logZStatus(ZAIES_Warning,ZS_XMLEMPTY,"Key dictionary malformed : no key definitions found.\n");
          wSt = ZS_XMLEMPTY;
          goto EndloadXMLDictionary;
          }

        pErrorLog->textLog("found %ld key definitions.",wKeyNodes.count());

        for (long wi=0;wi < wKeyNodes.count() ; wi++)
        {
          ZSKeyDictionary* wKeyDictionary=new ZSKeyDictionary(pMasterDic);
          bool wFieldsOK=true;
          zxmlElement* wKeyFields=nullptr;

          if (XMLgetChildText(wKeyNodes[wi],"indexname",wKeyDictionary->DicKeyName,pErrorLog,ZAIES_Error))
          {
            pErrorLog->errorLog("loadXMLDictionary: Key rank <%ld> missing <indexname> mandatory parameter. Skipping key definition.",wi);
            delete wKeyDictionary;
            wKeyDictionary=nullptr;
            continue;
          }

          wSt=wKeyNodes[wi]->getChildByName((zxmlNode*&)wKeyFields,"keyfields");
          if (wSt!=ZS_SUCCESS)
            {
            pErrorLog->errorLog("loadXMLDictionary: Key rank <%ld> - name <%s> has empty key definition. No key field found. Skipping key definition.",wi,wKeyDictionary->DicKeyName.toCChar());
            delete wKeyDictionary;
            wKeyDictionary=nullptr;
            continue;
            }

          ZElementCollection wKeyFieldNodes =  wKeyFields->getAllChildren("field");
          XMLderegister(wKeyFields);

          pErrorLog->textLog("loadXMLDictionary: For key name <%s> found %ld field definitions.",wKeyDictionary->DicKeyName.toCChar(),wKeyFieldNodes.count());

          for (long wj=0;wj < wKeyFieldNodes.count() ; wj++)
          {
            /* if hash is not found (may be a new field introduced there) :
             * try to get field by name from meta dic
             */
            ZSIndexField wKeyField;
            utf8String wFieldName;
            wSt=zmuXMLgetChild(wKeyFieldNodes[wj],"mdicrank",wKeyField.MDicRank,pErrorLog,true,ZAIES_Error);
            wSt=zmuXMLgetChild(wKeyFieldNodes[wj],"hash",wKeyField.Hash,pErrorLog,true,ZAIES_Error);
            wSt=zmuXMLgetChild(wKeyFieldNodes[wj],"keyoffset",wKeyField.KeyOffset,pErrorLog,true,ZAIES_Error);

            if (pMasterDic->Tab[wKeyField.MDicRank].Hash==wKeyField.Hash)
              {
              wKeyDictionary->push(wKeyField); /* good, next */
              continue;
              }
            /* try to recover */
            long wMRank=pMasterDic->getFieldByHash(wKeyField.Hash);
            if (wMRank<0)
            {
              wSt=zmuXMLgetChild(wKeyFieldNodes[wj],"name",wFieldName,pErrorLog,true,ZAIES_Error);
              if (wSt!=ZS_SUCCESS)
              {
                pErrorLog->errorLog("loadXMLDictionary: From key rank <%ld> key Field rank <%ld> Hash is invalid and <name> is missing.Skipping errored field.",wi,wj);
                wFieldsOK=false;
                continue;
              }
              wMRank=pMasterDic->getFieldByName(wFieldName.toCChar());
              if (wMRank<0)
              {
                pErrorLog->errorLog("loadXMLDictionary: From key rank <%ld> key Field rank <%ld>\n"
                                   " Hash is invalid and field name <%s> is not found within meta dictionary.\n"
                                   "Skipping errored field.",wi,wj,wFieldName.toCChar());
                wFieldsOK=false;
                continue;
              }//if (wMRank<0)

              wKeyField.MDicRank=(uint32_t)wMRank;
              wKeyDictionary->push(wKeyField); /*  recovered  */
            }//if (wMRank<0)
            }//for

          if (!wFieldsOK)
          {
            pErrorLog->textLog ("loadXMLDictionary: Key rank <%ld> had irrecoverable errors and is not loaded within master dictionary.\n",wi);
            delete wKeyDictionary;
            wKeyDictionary=nullptr;
          }
          else
            pMasterDic->KeyDic.push(wKeyDictionary);

        }//for (long wi=0;wi < wKeyNodes.count() ; wi++)
      }// instruction block
    }//=======while true===============

EndloadXMLDictionary:

    if (pMasterDic==nullptr)
        pErrorLog->textLog(
            "________________%s Report_______________\n"
            "  No dictionary present or dictionary errored.\n"
            "______________________________________________________\n",
              _GET_FUNCTION_NAME_);

    else
      pErrorLog->textLog(
          "________________%s Report_______________\n"
          "  Missing tags                     %ld\n"
          "  Meta dictionary fields           %ld\n"
          "  Meta dictionary load had errors  %s\n"
          "  Key(s) definition processed      %ld\n"
          "  Errored Key(s)                   %ld\n"
          "______________________________________________________\n",
          _GET_FUNCTION_NAME_,
          wMissingTags,
          pMasterDic->count(),
          pMasterDic->KeyDic.count(),
          wErroredKeys);

    
    
    return wSt;
}//loadXMLDictionaryForCreate


/**
 * @brief applyXMLDictionaryChange
 *
 *
 *  <zmasterdictionary>                       may use <suppress> option (suppress master dictionary)
 * NB: suppress metadictionary not allowed : either the whole master dictionary or nothing
 *
 *          <zkeydictionary>                  may use <suppress> option (suppress all dictionary keys)
 *          <key>                             may use <suppress> option (suppress one dictionary key)
 *
 *
 * @param pMasterFile
 * @param pRoot
 * @param pErrorLog
 * @return  as ZStatus
 *  ZS_NOTFOUND <zmasterdictionary>  has not been found within Xml code
 *
 */
ZStatus
applyXMLDictionaryChange(ZRawMasterFile* pMasterFile, zxmlElement* pRoot, bool pRealRun, ZaiErrors* pMessageLog)
{
  ZStatus wSt=ZS_SUCCESS;
  zxmlElement* wMasterDicNode=nullptr;
  zxmlElement* wMetaDicNode=nullptr;
  zxmlElement* wKeyDicNode=nullptr;

  ZElementCollection  wIndexElts;
  int                 wMissingTags=0;
  int                 wErroredKeys=0;
  bool                wCannotProcess=false;

  ZMFDictionary* wMasterDic=nullptr;

  ZPRES wMetaDicStatus=ZPRES_Nothing;
  ZPRES wKeyDicStatus=ZPRES_Nothing;

  pMessageLog->setErrorLogContext(_GET_FUNCTION_NAME_);

  if (pMasterFile->getOpenMode()==ZRF_NotOpen)
  {
    pMessageLog->errorLog("Master file is not open.");
    pMessageLog->popContext();
    return ZS_FILENOTOPEN;
  }
  if (!(pMasterFile->getOpenMode()&ZRF_Exclusive))
    {
    pMessageLog->errorLog("Master file open mode is not exclusive.");
    pMessageLog->popContext();
    return ZS_MODEINVALID;
    }

   wSt=pRoot->getChildByName((zxmlNode*&) wMasterDicNode,"zmasterdictionary");

  /* looking for <zmasterdictionary>  */

  if (wSt!=ZS_SUCCESS)
      {
      pMessageLog->textLog("Node <zmasterdictionary> has not been found.");
      //      wMayCreateMasterDic = false;
      return ZS_NOTFOUND;
      }

  wMasterDic=new ZMFDictionary;

  /* loading Master dictionary */
  pMessageLog->textLog("found node <zmasterdictionary>. Loading master dictionary from its Xml definition.");

  while (true)//=======while true===============
  {
    /***************************
     *  find if <suppress> : master dictionary is explicitly requested to be suppressed
     ****************************/
    bool wSuppress=false;
    if (XMLgetChildBool(wMasterDicNode,"suppress",wSuppress,pMessageLog,ZAIES_Info)==0)
    {
      pMessageLog->infoLog("<suppress> keyword with value <%s> found for node <zmasterdictionary>",
          wSuppress?"true":"false");

      if (wSuppress)
      {
        if (wMasterDic!=nullptr)
        {
          delete wMasterDic;
          wMasterDic=nullptr;
        }
        pMessageLog->textLog("It is requested to suppress whole master dictionary (meta dic & key dictionaries). Stopping Xml parsing for master dictionary. ");
        wMetaDicStatus = ZPRES_ToBeDeleted;  /* request to delete the whole dictionary*/
        break;
      }//if (wSuppress)

      pMessageLog->infoLog("Found node <suppress> but set to value FALSE. Clause is invalidated.\n"
                           "Continuing parsing xml for modifications.");
      wMetaDicStatus = ZPRES_Unchanged;     /* do not touch */
    }// ----------if <suppress> found--------------

    wSt=wMasterDicNode->getChildByName((zxmlNode*&) wMetaDicNode,"metadic");
    if (wSt!=ZS_SUCCESS)
      {
      pMessageLog->logZStatus(ZAIES_Error,ZS_BADDIC,"loadXMLDictionary: mandatory node <metadic> has not been found.\n"
                                                    "Master dictionary appears to be corrupted.\n "
                                                    "Master dictionary xml parsing stopped.\n");
      XMLderegister(wMasterDicNode);
      wCannotProcess=true;
      break;
      }
    XMLderegister(wMetaDicNode);

    pMessageLog->textLog("found node <metadic>. Loading meta dictionary.");
    wSt=wMasterDic->ZMetaDic::fromXml(wMasterDicNode,pMessageLog,ZAIES_Error);
    if (wSt!=ZS_SUCCESS)
      {
      pMessageLog->errorLog("loadXMLDictionary: Meta dictionary (field dictionary) cannot be loaded or appears to be corrupted.\n "
                            "Dictionary Xml parsing stopped.\n");
      wCannotProcess=true;
      XMLderegister(wMasterDicNode);
      break;
      }

    pMessageLog->textLog("loadXMLDictionary: Meta dictionary loaded. <%ld> field definitions loaded.",wMasterDic->count());




    /*-------------------------------
     *  loading key dictionary
     *-------------------------------*/

    wSt=wMasterDicNode->getChildByName((zxmlNode*&) wKeyDicNode,"keydictionary");

    XMLderegister(wMasterDicNode);

    if (wSt==ZS_NOTFOUND)
      {
      pMessageLog->logZStatus(ZAIES_Warning,wSt,"Node <keydictionary> has not been found.\n"
                                                "Master dictionary has no key definitions.\n"
                                                "File's Key dictionary, if exists, will remain unchanged.");
      wKeyDicStatus = ZPRES_Unchanged;
      break;
      }

    pMessageLog->textLog("Found node <keydictionary>.");

    /***********************
     *  suppress
     ***********************/

    if (XMLgetChildBool(wKeyDicNode,"suppress",wSuppress,pMessageLog,ZAIES_Error)==0)
        {
        pMessageLog->infoLog("<suppress> keyword with value <%s> found for <keydictionary> node",
            wSuppress?"true":"false");
        if (wSuppress)
          {
            wKeyDicStatus = ZPRES_ToBeDeleted;
            pMessageLog->textLog("Suppressing key dictionaries (all keys definition). Stopping Xml parsing for master dictionary.");
            break;
          }//if (wSuppress)
        else
          {
            pMessageLog->infoLog("Node name <keydictionary> : <suppress> node has value FALSE. Normal key modification will apply. Continuing parsing Xml data.");
          }
        }//if (XMLgetChildBool... suppress


    pMessageLog->textLog("Loading key dictionary.");

    ZElementCollection wKeyNodes = wKeyDicNode->getAllChildren("key");
    XMLderegister(wKeyDicNode);

    if (wKeyNodes.count()==0)
        {
        pMessageLog->logZStatus(ZAIES_Error,ZS_XMLEMPTY,"Key dictionary malformed : no key definition found.");
        wCannotProcess=true;
        break;
        }

      pMessageLog->textLog("found %ld key definitions.",wKeyNodes.count());

      for (long wi=0;wi < wKeyNodes.count() ; wi++)
      {
        ZSKeyDictionary* wKeyDictionary=new ZSKeyDictionary(wMasterDic);
        bool wFieldsOK=true;
        zxmlElement* wKeyFields=nullptr;

        if (XMLgetChildText(wKeyNodes[wi],"indexname",wKeyDictionary->DicKeyName,pMessageLog,ZAIES_Error))
          {
          pMessageLog->errorLog("Xml key rank <%ld> missing <indexname> mandatory parameter. Xml key dictionary definition is corrupted.",wi);
          delete wKeyDictionary;
          wKeyDictionary=nullptr;
          wCannotProcess=true;
          continue;
          }

        long wh=wMasterDic->searchKeyRankCase(wKeyDictionary->DicKeyName) ;


  /*******************
   *       Suppress
   *******************/

        wSuppress=false;
        if (XMLgetChildBool(wKeyNodes[wi],"suppress",wSuppress,pMessageLog,ZAIES_Error)==0)
        {

          pMessageLog->infoLog("<suppress> keyword with value <%s> found for key <%s>",
              wSuppress?"true":"false",
              wKeyDictionary->DicKeyName.toCChar());

          if (wSuppress)
            {
            if (wh < 0)
              {
              pMessageLog->infoLog("Key Dictionary : Key with index name <%s> is not found within actual master dictionary and cannot be removed.\n"
                                  "Skipping this key",wKeyDictionary->DicKeyName.toCChar());
              delete wKeyDictionary;
              continue;
              }

            wKeyDictionary->Status=ZPRES_ToBeDeleted;
            wMasterDic->KeyDic.push(wKeyDictionary);
            continue;
          }//if (wSuppress)
          else
          {
            pMessageLog->infoLog("For key name <%s> <suppress> node has value FALSE. No removal done.Normal key modification will apply. Continuing parsing Xml data.",wKeyDictionary->DicKeyName.toCChar());
          }
        }//if (XMLgetChildBool... suppress


        if (wh<0)
          wKeyDictionary->Status = ZPRES_ToCreate;
        else
          wKeyDictionary->Status = ZPRES_ToChange;


        wSt=wKeyNodes[wi]->getChildByName((zxmlNode*&)wKeyFields,"keyfields");
        if (wSt!=ZS_SUCCESS)
        {
          pMessageLog->errorLog("Key rank <%ld> - name <%s> has empty key definition. No key field found. Skipping key definition.\n"
                                "No modification will be applied to file.",wi,wKeyDictionary->DicKeyName.toCChar());
          wCannotProcess = true;
          delete wKeyDictionary;
          wKeyDictionary=nullptr;
          continue;
        }

        ZElementCollection wKeyFieldNodes =  wKeyFields->getAllChildren("field");
        XMLderegister(wKeyFields);

        pMessageLog->textLog("Key named <%s> found %ld field definitions.",wKeyDictionary->DicKeyName.toCChar(),wKeyFieldNodes.count());


        for (long wj=0;wj < wKeyFieldNodes.count() ; wj++)
        {
          /* if hash is not found (may be a new field introduced there) :
             * try to get field by name from meta dic
             */
          ZSIndexField wKeyField;
          utf8String wFieldName;
          wSt=zmuXMLgetChild(wKeyFieldNodes[wj],"mdicrank",wKeyField.MDicRank,pMessageLog,true,ZAIES_Error);
          wSt=zmuXMLgetChild(wKeyFieldNodes[wj],"hash",wKeyField.Hash,pMessageLog,true,ZAIES_Error);
          wSt=zmuXMLgetChild(wKeyFieldNodes[wj],"keyoffset",wKeyField.KeyOffset,pMessageLog,true,ZAIES_Error);

          if (wMasterDic->Tab[wKeyField.MDicRank].Hash==wKeyField.Hash)
          {
            wKeyDictionary->push(wKeyField); /* good, next */
            continue;
          }
          /* try to recover */
          long wMRank=wMasterDic->getFieldByHash(wKeyField.Hash);
          if (wMRank<0)
          {
            wSt=zmuXMLgetChild(wKeyFieldNodes[wj],"name",wFieldName,pMessageLog,true,ZAIES_Error);
            if (wSt!=ZS_SUCCESS)
            {
              pMessageLog->errorLog("loadXMLDictionary: From key rank <%ld> key Field rank <%ld> Hash is invalid and <name> is missing.Skipping errored field.",wi,wj);
              wFieldsOK=false;
              continue;
            }
            wMRank=wMasterDic->getFieldByName(wFieldName.toCChar());
            if (wMRank<0)
            {
              pMessageLog->errorLog("loadXMLDictionary: From key rank <%ld> key Field rank <%ld>\n"
                                  " Hash is invalid and field name <%s> is not found within meta dictionary.\n"
                                  "Skipping errored field.",wi,wj,wFieldName.toCChar());

              wFieldsOK=false;
              continue;
            }//if (wMRank<0)

            wKeyField.MDicRank=(uint32_t)wMRank;
            wKeyDictionary->push(wKeyField); /*  recovered  */
          }//if (wMRank<0)
          }//for

        if (!wFieldsOK)
        {
          pMessageLog->textLog ("Key rank <%ld> had irrecoverable errors.\n"
                               "No modification will be applied to master dictionary.\n",wi);
          wErroredKeys ++;
          wCannotProcess = true;
          continue;
        }
/* here wKeyDictionary contains a valid key definition */


        /* define key status */

        if (pMasterFile->MasterDic==nullptr)
          wKeyDictionary->Status=ZPRES_ToCreate;
        else
            if (pMasterFile->MasterDic->KeyDic.count()==0)
                wKeyDictionary->Status=ZPRES_ToCreate;
            else
            {
            ZSKeyDictionary* wKD=pMasterFile->MasterDic->searchKeyCase(wKeyDictionary->DicKeyName);
            if (!wKD)
                 wKeyDictionary->Status=ZPRES_ToCreate;
            else
            {
            /* check if key is really different from what exist in file */
            if (wKD->hasSameContentAs(wKeyDictionary))
                wKeyDictionary->Status=ZPRES_Unchanged;
            else
                wKeyDictionary->Status=ZPRES_ToChange;
            }// else
            }// else
        wMasterDic->KeyDic.push(wKeyDictionary);
      }//for (long wi=0;wi < wKeyNodes.count() ; wi++)

  }//=======while true===============


  if (wMasterDic==nullptr)
    {
    pMessageLog->textLog(
        "________________%s Report_______________\n"
        "  No dictionary present whithin Xml definition or dictionary errored.\n"
        "_____________________________________________________________________\n",
        _GET_FUNCTION_NAME_);
    pMessageLog->popContext();
    return wSt;
    }


    pMessageLog->textLog(
        "________________%s Report_______________\n"
        "  Meta dictionary fields           %ld\n"
        "  Key(s) definition processed      %ld\n"
        "  Errored Key(s)                   %ld\n"
        "______________________________________________________\n",
        _GET_FUNCTION_NAME_,
        wMasterDic->count(),
        wMasterDic->KeyDic.count(),
        wErroredKeys);


    if (!pRealRun)
      return ZS_SUCCESS;

    if (wCannotProcess)
      {
        pMessageLog->warningLog("Fatal errors have been detected during Xml parsing : Modification cannot be applied.");
        pMessageLog->popContext();
        return ZS_ERROR;
      }


  if (wMetaDicStatus==ZPRES_ToBeDeleted)
    {
    pMessageLog->textLog("Removing Master dictionary from file.");

    delete pMasterFile->MasterDic;
    pMasterFile->MasterDic=nullptr;

    wSt= pMasterFile->writeControlBlocks();
    if (wSt!=ZS_SUCCESS)
      pMessageLog->textLog("done.");
    else
      pMessageLog->logZStatus(ZAIES_Error,wSt,"Cannot remove master dictionary on file.");
    pMessageLog->popContext();
    return wSt;
    }

  if (wMetaDicStatus==ZPRES_ToCreate)
    {
     ZArray <IndexData_st> wIndexData;
     IndexData_st wIData;
     for (long wi=0;wi < pMasterFile->IndexTable.count();wi++)
       {
       wIndexData.push(pMasterFile->IndexTable[wi]->getIndexData());
       }

     wSt=validateXmlDicDefinition(wMasterDic, &wIndexData, pMessageLog);
  if (wSt!=ZS_SUCCESS)
      {
       pMessageLog->infoLog("Cannot create master dictionary.");
       pMessageLog->popContext();
       return wSt;
      }



  pMessageLog->textLog("About to create Master dictionary ;\n"
                        "   ___Meta dictionary____\n"
                        "   Dictionary Name       <%s>\n"
                        "   # fields definitions  %d\n"
                        "   ______________________\n",
                        wMasterDic->DicName.toCChar(),
                        wMasterDic->count());

  if (wMasterDic->KeyDic.count()>0)
      {
        pMessageLog->textLog("About to create key dictionary ;\n"
                          "   ___________________Key dictionary_______________\n"
                          "   Defined keys                %ld\n"
                          "   _________________________________________________n",
                          wMasterDic->KeyDic.count());

        for (long wi=0;wi < wMasterDic->KeyDic.count() ; wi ++)
          pMessageLog->textLog("<%3ld>Key dictionary name   <%s>\n"
                            "     # fields definitions       %ld\n"
                            "     key universal size         %d\n"
                            "   _________________________________________________\n",
                            wi,wMasterDic->KeyDic[wi]->DicKeyName.toCChar(),
                            wMasterDic->KeyDic[wi]->count(),
                            wMasterDic->KeyDic[wi]->computeKeyUniversalSize());
      }
    if (!pRealRun)
        {
        pMessageLog->popContext();
        return ZS_SUCCESS;
        }

    if (wCannotProcess)
      {
        pMessageLog->warningLog("Fatal errors have been detected during Xml parsing : Modifications cannot be applied to file.");
        pMessageLog->popContext();
        return ZS_ERROR;
      }
    pMasterFile->MasterDic = new ZMFDictionary(*wMasterDic);

    wSt= pMasterFile->writeControlBlocks();
    if (wSt==ZS_SUCCESS)
        pMessageLog->textLog("done.");
      else
        pMessageLog->logZStatus(ZAIES_Error,wSt,"Cannot write master dictionary on file.");


    pMessageLog->popContext();
    return wSt;
    }//ZPRES_ToCreate

  if (wMetaDicStatus!=ZPRES_ToChange)
      {
      pMessageLog->errorLog("Invalid/unknown dictionary status found.");
      pMessageLog->popContext();
      return ZS_INVOP;
      }

  ZArray <IndexData_st> wIndexData;
  IndexData_st wIData;
  for (long wi=0;wi < pMasterFile->IndexTable.count();wi++)
      {
        wIndexData.push(pMasterFile->IndexTable[wi]->getIndexData());
      }

  wSt=validateXmlDicDefinition(wMasterDic, &wIndexData, pMessageLog);
  if (wSt!=ZS_SUCCESS)
      {
        pMessageLog->infoLog("Xml dictionary has been invalidated.");
        pMessageLog->popContext();
        return wSt;
      }


 /* here status is     ZPRES_ToChange */

    pMessageLog->textLog("About to modify Master dictionary\n"
                         "   ___________________Meta dictionary___________________________\n"
                         "   Dictionary former name  <%s>   new name <%s>\n"
                         "   # fields definitions former number %d  new number %d\n"
                         "   _____________________________________________________________\n",
                          pMasterFile->MasterDic->DicName.toCChar(),
                          wMasterDic->DicName.toCChar(),
                          pMasterFile->MasterDic->count(),
                          wMasterDic->count());

  if (wCannotProcess)
    {
    pMessageLog->logZStatus(ZAIES_Error,wSt,"Unrecoverable errors have been encountered while loading Xml data.");
    pMessageLog->popContext();
    return ZS_ERROR;
    }

  if (pRealRun)
    {
    pMasterFile->MasterDic->clear();
    pMasterFile->MasterDic->DicName=wMasterDic->DicName;
    for (long wi=0;wi < wMasterDic->count();wi++)
      pMasterFile->MasterDic->push(wMasterDic->Tab[wi]);
    }

    pMessageLog->textLog("About to modify Key dictionary ");

    for (long wi=0;wi < wMasterDic->KeyDic.count();wi++)
    {
      switch(wMasterDic->KeyDic[wi]->Status)
      {
      case ZPRES_ToCreate:
      {
        pMessageLog->textLog("About to create key");
        displayKeyDicElement(wMasterDic,wi,pMessageLog);
        if (pRealRun)
          {
          pMasterFile->MasterDic->addKey(wMasterDic->KeyDic[wi]);
          pMasterFile->MasterDic->KeyDic.push(new ZSKeyDictionary(wMasterDic->KeyDic[wi]));
          pMessageLog->textLog("done.");
          }
        break;

      }// ZPRES_ToCreate

      case ZPRES_ToBeDeleted:
      {
        pMessageLog->textLog("About to delete key\n");
        displayKeyDicElement(wMasterDic,wi,pMessageLog);
        ZSKeyDictionary* wDK=pMasterFile->MasterDic->searchKeyCase(wMasterDic->KeyDic[wi]->DicKeyName);
        if (!wDK)
          pMessageLog->errorLog("Strange... dictionary key <%s> not found in file while trying to delete it.",wMasterDic->KeyDic[wi]->DicKeyName.toCChar());
        else
          {
          if (pRealRun)
            {
            pMasterFile->MasterDic->removeKey(wDK);
            pMessageLog->textLog("done.");
            }
          }
        break;
      }
      case ZPRES_ToChange:
        {
        pMessageLog->textLog("About to change key\n");
        ZSKeyDictionary* wDK=pMasterFile->MasterDic->searchKeyCase(wMasterDic->KeyDic[wi]->DicKeyName);
        if (!wDK)
          {
          pMessageLog->errorLog("Strange... dictionary key <%s> not found in file while trying to change it.",wMasterDic->KeyDic[wi]->DicKeyName.toCChar());
          pMessageLog->textLog("Cannot display former key values. Key will be added to dictionary.");
          }
        else
          {
          pMessageLog->textLog("__________Former key values_________\n");
          displayKeyDicElement(wDK,pMessageLog);
          pMasterFile->MasterDic->removeKey(wDK);
          }

        pMessageLog->textLog("__________New key values_____________\n");
        displayKeyDicElement(wMasterDic,wi,pMessageLog);

        pMasterFile->MasterDic->addKey(wMasterDic->KeyDic[wi]);
        pMessageLog->textLog("done.");
        break;
        }
      }// switch

    }// for



  wSt= pMasterFile->writeControlBlocks();
  if (wSt==ZS_SUCCESS)
    pMessageLog->textLog("done.");
  else
    pMessageLog->logZStatus(ZAIES_Error,wSt,"Cannot create master dictionary on file.");


  pMessageLog->popContext();
  return wSt;

}//applyXMLDictionaryChange




void
displayKeyDicElement(ZMFDictionary* pMasterDic,long pIdx,ZaiErrors* pMessageLog)
{
  displayKeyDicElement(pMasterDic->KeyDic[pIdx],pMessageLog);
}//displayKeyDicElement

void
displayKeyDicElement(ZSKeyDictionary* pKeyDic,ZaiErrors* pMessageLog)
{

  pMessageLog->textLog("Key <%s> fields count %ld  key universal size %ld\n"
                       "   ___________________key dictionary___________________________\n",
      pKeyDic->DicKeyName.toCChar(),
      pKeyDic->count(),
      pKeyDic->computeKeyUniversalSize());

  pMessageLog->textLog("-#- %15s %15s %6s %4s %18s\n","name","type","Usize","Koff","hash code");
  for (long wh=0;wh<pKeyDic->count();wh++)
  {
    long wMR=pKeyDic->Tab[wh].MDicRank;
    pMessageLog->textLog("%3ld %15s %15s %18s %6ld  %4ld\n",
        wh,
        pKeyDic->MetaDic->Tab[wMR].getName().toCChar(),
        decode_ZType( pKeyDic->MetaDic->Tab[wMR].ZType),
        pKeyDic->Tab[wh].Hash.toHexa().toChar(),
        pKeyDic->MetaDic->Tab[wMR].UniversalSize,
        pKeyDic->Tab[wh].KeyOffset);
  }
  pMessageLog->textLog("   _____________________________________________________________\n");
  return;
}//displayKeyDicElement

ZStatus
createDicFromXmlDefinition( ZRawMasterFile& pMasterFile,
                            ZMFDictionary* wMasterDic,
                            ZArray<IndexData_st>* wIndexData,
                            bool pRealRun,
                            ZaiErrors* pErrorLog)
{
  if (pMasterFile.hasDictionary())
    return ZS_INVOP;
  ZStatus wSt=validateXmlDicDefinition(wMasterDic,wIndexData,pErrorLog);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  pErrorLog->textLog("About to create Master dictionary ;\n"
                    "   ___Meta dictionary____\n"
                    "   Dictionary Name       <%s>\n"
                    "   # fields definitions  %d\n"
                    "   ______________________\n",
      wMasterDic->DicName.toCChar(),
      wMasterDic->count());

  if (wMasterDic->KeyDic.count()>0)
  {
    pErrorLog->textLog("About to create key dictionary ;\n"
                      "   ____Key dictionary____\n"
                      "   Defined keys          %ld\n"
                      "   ______________________\n",
        wMasterDic->KeyDic.count());

    for (long wi=0;wi < wMasterDic->KeyDic.count() ; wi ++)
      pErrorLog->textLog("<%3ld>Index name            %s\n"
                        "     # fields definitions  %ld\n"
                        "     key universal size    %d\n"
                        "   ________________________\n",
          wi,wMasterDic->KeyDic[wi]->DicKeyName.toCChar(),
          wMasterDic->KeyDic[wi]->count(),
          wMasterDic->KeyDic[wi]->computeKeyUniversalSize());

  }
  else
  {
    pErrorLog->textLog("No valid key dictionary has been found in xml definition. Key dictionary not created (part of master dictionary).");
  }

  if (pRealRun)
    {
    pMasterFile.MasterDic = new ZMFDictionary(*wMasterDic);
    pErrorLog->textLog("Master dictionary created and added to Master file.");
    }

  return ZS_SUCCESS;
}//createDicFromXmlDefinition


ZStatus
createMasterFileFromXml(const char* pXMLPath,
                        const char *pContentFilePath,
                        bool pRealRun,
                        bool pReplace,
                        const char* pLogfile)
{
  ZStatus     wSt;
  uriString   URIContent=pContentFilePath;
  uriString   wURIOutput;
  utf8String  wBase;
  FILE* wOutput=nullptr;
  bool FOutput=false;

  ZaiErrors wErrorLog;

  int  wAddedIndexes=0 ,
       wErroredIndexes = 0,
       wRebuiltIndexes = 0,
       wMissingTags=0 ,
       wMissMandatoryTags=0,
       wIndexProcessed=0;

  zxmlDoc* wXmlDoc=nullptr;
  zxmlElement* wRootElement=nullptr;
  zxmlElement* wFileNode=nullptr;
  zxmlElement* wFileNode_2=nullptr;
  zxmlElement* wFDNode=nullptr;
  zxmlElement* wFDNode_2=nullptr;
  zxmlElement* wFCBNode=nullptr;
  zxmlElement* wFCBNode_2=nullptr;
  zxmlElement* wMCBNode=nullptr;
  zxmlElement* wMasterDicNode=nullptr;
//  zxmlElement* wMetaDicNode=nullptr;
//  zxmlElement* wKeyDicNode=nullptr;
  zxmlElement* wIndexTableNode=nullptr;
  zxmlElement* wICBNode=nullptr;
  zxmlElement* wJCBNode=nullptr;

  ZElementCollection wIndexElts;


  /* IndexPresence values meaning
   * 0 : Index to be deleted
   * 1 : Index present but not to be rebuilt
   * 2 : Index to be built or rebuilt -
   * 3 : Index created (therefore rebuilt)
   * 4 : Index errored
   */


  ZFile_type wFileType=ZFT_ZRawMasterFile;
  int wFileTypeI=0;

  ZRawMasterFile          wMasterFile;




/* ZRandomFile FCB data */

  struct MCB_st
  {
    utf8String IndexFilePath;
  } ;



  FCBParams* wMasterFCB=nullptr;
  ZSJournalControlBlock* wJCB=nullptr;
  ZMFDictionary*  wMasterDic=nullptr;

  MCB_st        wMCB;

  bool      wMayCreateMasterFile=true;
  bool      hasMasterDic=false;
  bool      wMayCreateKeyDictionary=true;

  ZArray<IndexData_st>* wIndexData=nullptr;
  bool      wMayCreateIndex=true;


/* meta dictionary */
  int                 wKeyCount=-1; /* default value <field omitted> */
  ZElementCollection  wFieldsElts;
  ZElementCollection  wKeyDicElts;


  if (pLogfile==nullptr)
    {
      wURIOutput=pXMLPath;
      wBase=wURIOutput.getBasename().toCChar();
      wBase+=".xmllog";
    }//if (pLogfile==nullptr)
    else
      wBase=pLogfile;

  wErrorLog.setErrorLogContext("createFromXml");
  wErrorLog.setAutoPrintOn(true);
  if (!wBase.isEmpty())
    wErrorLog.setOutput(wBase.toCChar());
  else
    wErrorLog.setOutput("createFromXml.log");

  utf8String wT;
  wT.sprintf(" Creating file <%s> from xml definition file <%s> ", pContentFilePath,pXMLPath);
  int wS=(int)wT.getUnitCount();


  wErrorLog.textLog(
      "%*c\n"
      " %s\n"
      "%*c\n"
      "            Run options are \n"
      "                 - %s\n"
      "                 - %s\n"
      ,
      wS,'_',
      wT.toCChar(),
      wS,'_',
      pRealRun?"Real run (file will be created)":"Test run (no created file)",
      pReplace?"Replace if exists":"Abort if exists");

    wErrorLog.textLog(" Loading xml data...\n");

  wSt=_XMLLoadAndControl(pXMLPath,
                        wXmlDoc,
                        wRootElement,
                        "zicm",
                        __ZRF_XMLVERSION_CONTROL__,
                        &wErrorLog,
                        wOutput);
  if (wSt!=ZS_SUCCESS)
    goto ErrorcreateXMLFile;

  wSt=wRootElement->getChildByName((zxmlNode* &)wFileNode,"zfile");
  XMLderegister(wRootElement);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        wSt,
        Severity_Error,
        "XML error : corrupted or empty file <%s>. Mandatory tag <file> was expected and not found.",
        pXMLPath);

    wErrorLog.logZStatus(ZAIES_Error, wSt,"XML error : corrupted or empty file <%s>. Mandatory tag <file> was expected and not found.",
        pXMLPath);
    goto ErrorcreateXMLFile;
  }

  while (true)
  {

    //-------------FileType Control ------------------------------------

  if (zmuXMLgetChild(wFileNode,"filetype",wFileTypeI,&wErrorLog,true,ZAIES_Error)!=ZS_SUCCESS)
    {
    wErrorLog.textLog("Assuming ZFT_ZRawMasterFile (value %d).",(int)wFileType);
    }
    else
    {
      wFileType =(ZFile_type)wFileTypeI;
      wErrorLog.textLog("Found file type to create <%s>(value %d).",decode_ZFile_type(wFileType), wFileTypeI);
    }


  wSt=wFileNode->getChildByName((zxmlNode*&) wFDNode,"zfiledescriptor");
  if (wSt!=ZS_SUCCESS)
    {
    wErrorLog.errorLog("Cannot find required node <zfiledescriptor>. Stopping xml parsing.");
    goto ErrorcreateXMLFile;
    }

  /* looking for <zfilecontrolblock> where usable/operational data for ZRandomFile is located */

  wSt=wFDNode->getChildByName((zxmlNode*&) wFCBNode,"zfilecontrolblock");
  if (wSt!=ZS_SUCCESS)
    {
      wErrorLog.errorLog("Cannot find required node <zfilecontrolblock>. Stopping xml parsing.");
      goto ErrorcreateXMLFile;
    }


  wMasterFCB = new FCBParams;
  wSt=zmuXMLgetChild(wFCBNode,"allocatedblocks",wMasterFCB->AllocatedBlocks,&wErrorLog,true,ZAIES_Error);
  wSt=zmuXMLgetChild(wFCBNode,"blockextentquota",wMasterFCB->BlockExtentQuota,&wErrorLog,true,ZAIES_Error);
  wSt=zmuXMLgetChild(wFCBNode,"initialsize",wMasterFCB->InitialSize,&wErrorLog,true,ZAIES_Error);
  wSt=zmuXMLgetChild(wFCBNode,"blocktargetsize",wMasterFCB->BlockTargetSize,&wErrorLog,true,ZAIES_Error);
  wMasterFCB->HighwaterMarking=false; /* default value for highwater marking is false. Nb : not a mandatory field */
  wSt=zmuXMLgetChild(wFCBNode,"highwatermarking",wMasterFCB->HighwaterMarking,&wErrorLog,false,ZAIES_Warning);
  wMasterFCB->GrabFreeSpace=false; /* default value for grabfreespace is false. Nb : not a mandatory field  */
  wSt=zmuXMLgetChild(wFCBNode,"grabfreespace",wMasterFCB->GrabFreeSpace,&wErrorLog,false,ZAIES_Warning);

  break;
  }// while true




   /*---------------------------------------------------------*/


  /* looking for <zmastercontrolblock> where usable/operational data for ZRawMasterFile is located */

  wErrorLog.textLog(" Processing master control block.\n");


  wSt=wFileNode->getChildByName((zxmlNode*&) wMCBNode,"zmastercontrolblock");
  if (wSt!=ZS_SUCCESS)
      {
      wErrorLog.errorLog("Cannot find mandatory node <zmastercontrolblock>. Stopping xml parsing.");
      goto ErrorcreateXMLFile;
      }

    /* MCB components :
     *  - Master dictionary
     *      - meta dictionary : array of field definitions S
     *      - keyDictionary array
     *            array of keyfields referring to meta dictionary field
     *  - IndexTable : array of ZSIndexFile
     *      - ZIndexControlBlock : key size and parameters
     *      - ZRandomFile parameters : target block size is dependent from key size + sizeof address_type
     */

    /* getting Master dictionary : for master dictionary rootnode is mcb node */

    wSt = loadXMLDictionaryForCreate(wMCBNode, wMasterDic, &wErrorLog);


    if (XMLgetChildText(wMCBNode,"indexfilepath",wMCB.IndexFilePath,&wErrorLog,ZAIES_Warning)<0)
      {
      wErrorLog.infoLog("<indexfilepath> not found : index files will be located in same directory than master content.");
      wMCB.IndexFilePath.clear();
      }
    else
      wErrorLog.infoLog("Got <indexfilepath> value <%s>.",wMCB.IndexFilePath.toCChar());




  /* processing index control block array (raw index definition) */


  wSt=wMCBNode->getChildByName((zxmlNode*&) wIndexTableNode,"indextable");
  if (wSt!=ZS_SUCCESS)
      {
        wErrorLog.log(ZAIES_Fatal, "Cannot find node <indextable>.\n"
                                   "Master file definition must have an indextable (even empty).\n"
                                   "Stopping xml parsing.");
        wSt = ZS_XMLMISSREQ;
        wMissMandatoryTags++;
        XMLderegister(wFileNode);
        XMLderegister(wMCBNode);
        goto ErrorcreateXMLFile;
      }


  wIndexElts = wIndexTableNode->getAllChildren("index");
  if (wIndexElts.count()==0)
      {
      wErrorLog.warningLog("Cannot find any index within Master Control Block <indextable> definition.");
      }
      else
        wIndexData=new ZArray<IndexData_st>;

  for (long wi=0;wi < wIndexElts.count();wi++)
    {
    IndexData_st wIData;
    wMayCreateIndex=true;
    while (true) // ================while true ======================
      {

      /* first get ZIndexControlBlock operational data */

      wSt=wIndexElts[wi]->getChildByName((zxmlNode*&) wICBNode,"indexcontrolblock");
      if (wSt!=ZS_SUCCESS)
        {
        wErrorLog.errorLog("Cannot find required node <indexcontrolblock> for index rank <%ld>.\n"
                           "Malformed xml definition. Stopping xml parsing for indexes.\n"
                           "No index will be created.",wi);
        XMLderegister(wFileNode);
        XMLderegister(wMCBNode);
        XMLderegister(wIndexTableNode);
        wMayCreateIndex=false;
        break;
        }

        wSt=zmuXMLgetChild(wICBNode,"indexname",wIData.IndexName,&wErrorLog,true,ZAIES_Error);
        if (wSt!=ZS_SUCCESS)
          wMayCreateIndex = false;
        wSt=zmuXMLgetChild(wICBNode,"keyuniversalsize",wIData.KeyUniversalSize,&wErrorLog,true,ZAIES_Error);
        if (wSt!=ZS_SUCCESS)
          wMayCreateIndex = false;
        wIData.Duplicates=ZST_NODUPLICATES;
        wSt=zmuXMLgetChild(wICBNode,"keyuniversalsize",(int&)wIData.Duplicates,&wErrorLog,false,ZAIES_Warning);

    /* check if there is already an ICB with an index name equal to that one : errored index control block */

      for (long wh=0;wh<wIndexData->count();wh++)
        if (wIndexData->Tab[wh].IndexName==wIData.IndexName)
          {
          wErrorLog.errorLog("Two index control blocks has same index name <%s> -> index rank <%ld> and <%ld>.\n"
                             "Malformed xml definition. Stopping xml parsing for indexes.\n"
                             "No index will be created.",wi,wh);
          XMLderegister(wFileNode);
          XMLderegister(wMCBNode);
          XMLderegister(wIndexTableNode);
          wMayCreateIndex=false;
          break;
          }


    /* then get Index file FCB operational data */

    /* compute default/recommended parameter values for index file */

      wIData.FCB.AllocatedBlocks = wMasterFCB->AllocatedBlocks ; /* Index has same allocated blocks as master */
      wIData.FCB.BlockExtentQuota = wMasterFCB->BlockExtentQuota ; /* Index has same extent quota as master */
      /* key size dependent values */
      wIData.FCB.InitialSize = wIData.FCB.AllocatedBlocks  * ( wIData.KeyUniversalSize + sizeof(zaddress_type)) ;
      wIData.FCB.BlockTargetSize = wIData.KeyUniversalSize + sizeof(zaddress_type) ;
      /* other options */
      wIData.FCB.HighwaterMarking = false ;
      wIData.FCB.GrabFreeSpace = false;


    wSt=wIndexElts[wi]->getChildByName((zxmlNode*&) wFileNode_2,"file");
    if (wSt!=ZS_SUCCESS)
      {
      wErrorLog.warningLog("Cannot find node <file>.\n"
                           " Using computed File Control Block values for Index named <%s>.",
                            wIData.IndexName.toCChar());
      break ;
      }
    wSt=wFileNode_2->getChildByName((zxmlNode*&) wFDNode_2,"zfiledescriptor");
    if (wSt!=ZS_SUCCESS)
      {
      wErrorLog.warningLog("Cannot find node <zfiledescriptor>.\n"
                           " Using computed File Control Block values for Index named <%s>.",
                            wIData.IndexName.toCChar());
      break;
      }
    wSt=wFDNode_2->getChildByName((zxmlNode*&) wFCBNode_2,"zfilecontrolblock");
    if (wSt!=ZS_SUCCESS)
      {
      wErrorLog.warningLog("Cannot find node <zfilecontrolblock>.\n"
                           " Using computed File Control Block values for Index named <%s>.",
                            wIData.IndexName.toCChar());

      break;
      }

    /* acquiring ZRandomFile parameters for index file creation */

    wSt=zmuXMLgetChild(wFCBNode_2,"allocatedblocks",wIData.FCB.AllocatedBlocks,&wErrorLog,false,ZAIES_Warning);
    wSt=zmuXMLgetChild(wFCBNode_2,"blockextentquota",wIData.FCB.BlockExtentQuota,&wErrorLog,false,ZAIES_Warning);
    wSt=zmuXMLgetChild(wFCBNode_2,"initialsize",wIData.FCB.InitialSize,&wErrorLog,false,ZAIES_Warning);
    wSt=zmuXMLgetChild(wFCBNode_2,"blocktargetsize",wIData.FCB.BlockTargetSize,&wErrorLog,false,ZAIES_Warning);
    wSt=zmuXMLgetChild(wFCBNode_2,"highwatermarking",wIData.FCB.HighwaterMarking,&wErrorLog,false,ZAIES_Warning);
    wSt=zmuXMLgetChild(wFCBNode_2,"grabfreespace",wIData.FCB.GrabFreeSpace,&wErrorLog,false,ZAIES_Warning);

    break;
    }// ================while true ======================

    if (!wMayCreateIndex)
      wErrorLog.textLog("***** Xml Index definition has non recoverable errors.Index rank %ld will not be created.******",wi);
    else
      wIndexData->push(wIData);

  } // for (long wi=0;wi < wIndexElts.count();wi++)

  //---------------------------Journaling---------------------------------
  /*
    <zjournalcontrolblock>
      <journalingon>true</journalingon>
      <keep>true</keep>
      <journallocaldirectorypath></journallocaldirectorypath> <!-- omitted : same directory as ZRawMasterFile's path  -->
      <JournalingMaxDepth>-1</JournalingMaxDepth> <!-- max number of journaling records to keep. -1 : infinite. in this case need to use purgeJournaling(keepranks) regularly -->
      <zremotemirroring>
        <protocol>ZJP_RPC|ZJP_SSLV5</protocol> <!--   -->
        <addresstype>ZHAT_IPV4</addresstype> <!-- values ZHAT_IPV4:AF_INET ZHAT_IPV6:AF_INET6 - default:ZHAT_IPV4 -->
        <port> 1156 </port>
        <host>JournalServer</host>
        <authenticate> <authenticate>
        <sslkey>  </sslkey>
        <sslvector> </sslvector>
      </zremotemirroring>
    </zjournalcontrolblock>
*/


  while (true)
    {
    if (wMCBNode->getChildByName((zxmlNode*&)wJCBNode,"zjournalcontrolblock")!=ZS_SUCCESS)
      {
       wErrorLog.textLog(" No journaling data within Xml definition.\n");
//       wMayCreateJCB=false;
      break;
      }
    wJCB=new ZSJournalControlBlock;
    wSt=wJCB->fromXml(wMCBNode,&wErrorLog,ZAIES_Warning);
    if (wSt!=ZS_SUCCESS)
      {
      wErrorLog.errorLog(" Cannot load journaling data.\n");
      delete wJCB;
      wJCB=nullptr;
//      wMayCreateJCB=false;
      }
//    wMayCreateJCB=true;
    break;
    }//while (true)


   /* check dictionary-key definition vs index control block */
   /*
    error cases
    -----------

    1- error: index key has no correspondence with key dictionary definitions  (orphan)
        -> reject whole key dictionary (All key definitions)
        -> keep meta dic
    2- warning : dictionary key has no correspondence with index key
        -> dictionary key must be suppressed
    3- error: index key and dictionary key has same name but key universal size differs.
        -> dictionary key must be suppressed. ZPRES_Suppress
    4- warning: multiple dictionary keys match one index key over its name (dictionary keys have same Index name)
        -> surnumerous dictionary key must be suppressed,

    case 5 is already checked and managed when acquiring index control block.
    [5- error: multiple Index key have same IndexName and match one single dictionary key :
        -> Major error : Index control block is corrupted and master file creation MUST BE rejected.]

    */

  wSt=validateXmlDicDefinition(wMasterDic,wIndexData,&wErrorLog);



  if (wMasterDic!=nullptr)
    {
    wErrorLog.infoLog("Check dictionary against index control block definitions.");

    if (wIndexData->count()!=wMasterDic->KeyDic.count())
      wErrorLog.warningLog("W-BADCOUNT Indexes number <%ld> is not equal to dictionary key definitions <%ld>",
          wIndexData->count(),wMasterDic->KeyDic.count());

    uint8_t wICBPres[wIndexData->count()];
    uint8_t wKDicPres[wMasterDic->KeyDic.count()];

    memset (wICBPres,0,wIndexData->count());
    memset (wKDicPres,0,wMasterDic->KeyDic.count());

    for (long wi = 0; wi < wIndexData->count();wi++)
    {
      long wj = 0;
      for (; wj < wMasterDic->KeyDic.count();wj++)
        if (wMasterDic->KeyDic[wj]->DicKeyName==wIndexData->Tab[wi].IndexName)
          break;

      if (wj < wMasterDic->KeyDic.count()) /* found */
        {
        if (wICBPres[wi] > 0)
          {
          wErrorLog.errorLog( "E-DUPDICMATCH Two dictionary keys match the same ICB index name <%s> (rank %ld). Duplicate from key dictionary.",
              wIndexData->Tab[wi].IndexName.toCChar(),wi);
          }
        if (wKDicPres[wi] > 0)
          {
          wErrorLog.errorLog( "E-DUPICBMATCH Two dictionary keys match the same ICB index name <%s> (rank %ld). Duplicate from key dictionary.",
              wIndexData->Tab[wi].IndexName.toCChar(),wi);
          }
        wICBPres[wi]=1;   /* may be referenced more than once (in this case this is an error to be detected */
        wKDicPres[wj]=1;  /* may be referenced more than once (in this case this is an error to be detected */

        /* check key universal size */
        /* compute universal key size from key dictionary */
        uint32_t wKeyUSize=wMasterDic->KeyDic[wj]->computeKeyUniversalSize();
        if (wKeyUSize != wIndexData->Tab[wi].KeyUniversalSize)
          {
          wErrorLog.errorLog( "E-BADUSIZE For index <%s> (rank %ld) dictionary key universal size <%d> does not match index size <%d>",
                              wIndexData->Tab[wi].IndexName.toCChar(),wi,wKeyUSize,wIndexData->Tab[wi].KeyUniversalSize);
          wICBPres[wi]=4;
          wKDicPres[wj]=4;
          }
          else
            wErrorLog.infoLog( "Index <%s> (rank %ld) dictionary key universal size <%d> checked : OK.",
                wIndexData->Tab[wi].IndexName.toCChar(),wi,wKeyUSize);
        }
      else
        {
          wErrorLog.infoLog( "E-ICBORPHAN Index <%s> (rank %ld) not found within key dictionary.\n"
                            "Dictionary key definitions rejected (all).",
              wIndexData->Tab[wi].IndexName.toCChar(),wi);
          while (wMasterDic->KeyDic.count())
            delete wMasterDic->KeyDic.popR();
          wMayCreateKeyDictionary=false;
        }

      }// for wi
    }//if (wMasterDic!=nullptr)



/* back process */

  if (!wMayCreateMasterFile)
    {
    wErrorLog.infoLog(" Severe errors were encountered that prevent creating a master file from xml definition.");
    goto ErrorcreateXMLFile;
    }    

  wErrorLog.textLog("_____________Effective files creation from collected parameters._____________________");


  wErrorLog.textLog("About to create MasterFile with collected and/or defaulted data ;\n"
                    "   Allocated blocks      %ld\n"
                    "   Block extent quota    %ld\n"
                    "   Block target size     %ld\n"
                    "   Initial size          %ld\n"
                    "   Highwater marking     %s\n"
                    "   Grab free space       %s\n",
                    wMasterFCB->AllocatedBlocks,
                    wMasterFCB->BlockExtentQuota,
                    wMasterFCB->BlockTargetSize,
                    wMasterFCB->InitialSize,
                    wMasterFCB->HighwaterMarking?"yes":"no",
                    wMasterFCB->GrabFreeSpace?"yes":"no"
                    );
  if (pRealRun)
    {
    if (URIContent.exists())
      {
      if (!pReplace)
        {
        wErrorLog.logZStatus(ZAIES_Error,ZS_FILEEXIST," Master file <%s> already exists. It has been requested not to replace it.",URIContent.toCChar());
        wSt=ZS_FILEEXIST;
        goto ErrorcreateXMLFile;
        }
      wSt=ZRawMasterFile::renameBck(URIContent.toCChar(),&wErrorLog,"bck");
      if (wSt!=ZS_SUCCESS)
        goto ErrorcreateXMLFile;
      }//if (URIContent.exists())

    wSt=wMasterFile.zcreate(  pContentFilePath,
                              wMasterFCB->AllocatedBlocks,
                              wMasterFCB->BlockExtentQuota,
                              wMasterFCB->BlockTargetSize,
                              wMasterFCB->InitialSize,
                              wMasterFCB->HighwaterMarking,
                              wMasterFCB->GrabFreeSpace,
                              false,            /* no journaling at this stage */
                              false);           /* do not leave open */
    if (wSt!=ZS_SUCCESS)
        {
        wErrorLog.errorLog(" Master file <%s> has not been created, Aborting creation process.",pContentFilePath);
        wErrorLog.logZException();
        goto ErrorcreateXMLFile;
        }

    wErrorLog.textLog(" Master file <%s> has been created.\n",pContentFilePath);

    /* check file type */
    if (wMasterDic)
      {
      if (wFileType!=ZFT_ZMasterFile)
        {
        wErrorLog.warningLog("Requested file type is <%s>. It will be forced to <%s> (because of presence of master dictionary).\n",
            decode_ZFile_type(wFileType),decode_ZFile_type(ZFT_ZMasterFile));
        wFileType = ZFT_ZMasterFile;
        }
      }
      else  /* no master dic */
      {
        if (wFileType!=ZFT_ZRawMasterFile)
        {
          wErrorLog.warningLog("Requested file type is <%s>. It will be forced to <%s> (because of presence of master dictionary).\n",
              decode_ZFile_type(wFileType),decode_ZFile_type(ZFT_ZRawMasterFile));
          wFileType = ZFT_ZRawMasterFile;
        }
      }
    wMasterFile.setFileType(wFileType);
    }// if pRealRun


  if (wJCB!=nullptr)
    {
    wErrorLog.textLog("About to create and enable journaling for created file.\n");
    wMasterFile.ZJCB = new ZSJournalControlBlock(*wJCB);
    wMasterFile.setJournalingOn();
    wErrorLog.textLog(" Journaling has been defined and enabled.\n");
    }


  if (wIndexData != nullptr)
    for (long wi=0; wi < wIndexData->count() ; wi++)
    {
    wErrorLog.textLog("About to create Index with collected and/or defaulted data ;\n"
                      "   _______ICB____________\n"
                      "   Index name            <%s>\n"
                      "   Key universal size    %d\n"
                      "   Duplicates            %s\n"
                      "   _______FCB____________\n"
                      "   Allocated blocks      %ld\n"
                      "   Block extent quota    %ld\n"
                      "   Block target size     %ld\n"
                      "   Initial size          %ld\n"
                      "   Highwater marking     %s\n"
                      "   Grab free space       %s\n"
                      "   ______________________\n",
                      wIndexData->Tab[wi].IndexName.toCChar(),
                      wIndexData->Tab[wi].KeyUniversalSize,
                      wIndexData->Tab[wi].Duplicates== ZST_NODUPLICATES?"no duplicate":"allows duplicates",
                      wIndexData->Tab[wi].FCB.AllocatedBlocks,
                      wIndexData->Tab[wi].FCB.BlockExtentQuota,
                      wIndexData->Tab[wi].FCB.BlockTargetSize,
                      wIndexData->Tab[wi].FCB.InitialSize,
                      wIndexData->Tab[wi].FCB.HighwaterMarking?"yes":"no",
                      wIndexData->Tab[wi].FCB.GrabFreeSpace?"yes":"no"
                      );

    if (pRealRun)
      {
      wSt=wMasterFile.zcreateRawIndexDetailed(  wIndexData->Tab[wi].IndexName,
                                                wIndexData->Tab[wi].KeyUniversalSize,
                                                wIndexData->Tab[wi].Duplicates,
                                                wIndexData->Tab[wi].FCB.AllocatedBlocks,
                                                wIndexData->Tab[wi].FCB.BlockExtentQuota,
                                                wIndexData->Tab[wi].FCB.InitialSize,
                                                wIndexData->Tab[wi].FCB.HighwaterMarking,
                                                wIndexData->Tab[wi].FCB.GrabFreeSpace,
                                                false);
      if (wSt==ZS_SUCCESS)
        wErrorLog.infoLog(" Index structure and files created.");
      else
        {
        wErrorLog.errorLog("********* Cannot create index. Exception follows ***********");
        wErrorLog.logZException();
        }
      }//if (pRealRun)
  }//for (long wi=0; wi < wIndexData.count() ; wi++)

  if (wMasterDic != nullptr)
  {
  wErrorLog.textLog("About to create Master dictionary ;\n"
                    "   ___Meta dictionary____\n"
                    "   Dictionary Name       <%s>\n"
                    "   # fields definitions  %d\n"
                    "   ______________________\n",
                    wMasterDic->DicName.toCChar(),
                    wMasterDic->count());

  if (wMasterDic->KeyDic.count()>0)
    {
      wErrorLog.textLog("About to create key dictionary ;\n"
                        "   ____Key dictionary____\n"
                        "   Defined keys          %ld\n"
                        "   ______________________\n",
                        wMasterDic->KeyDic.count());

      for (long wi=0;wi < wMasterDic->KeyDic.count() ; wi ++)
            wErrorLog.textLog("<%3ld>Index name            %s\n"
                              "     # fields definitions  %ld\n"
                              "     key universal size    %d\n"
                              "   ________________________\n",
                              wi,wMasterDic->KeyDic[wi]->DicKeyName.toCChar(),
                              wMasterDic->KeyDic[wi]->count(),
                              wMasterDic->KeyDic[wi]->computeKeyUniversalSize());

    }
    else
      {
      wErrorLog.textLog("No valid key dictionary has been found in xml definition. Key dictionary not created.");
      }

    if (pRealRun && (wMasterDic != nullptr))
      {
      wMasterFile.MasterDic = new ZMFDictionary(*wMasterDic);
      wErrorLog.textLog("Master dictionary created.");
      }

  }//if (wMayCreateMasterDic)
  else
    wErrorLog.textLog("No valid master dictionary has been found in xml definition. Master dictionary not created.");


  wMasterFile.zclose();

EndcreateXMLFile:

  XMLderegister(wFileNode);
  XMLderegister(wMCBNode);
  XMLderegister(wIndexTableNode);
  XMLderegister(wFileNode_2);
  XMLderegister(wFDNode_2);
  XMLderegister(wFDNode);
  XMLderegister(wFCBNode);
  XMLderegister(wMasterDicNode);
  XMLderegister(wJCBNode);



  if (wMasterFCB)
    {
    delete wMasterFCB;
    wMasterFCB=nullptr;
    }

  if (wMasterDic)
    {
    delete wMasterDic;
    wMasterDic=nullptr;
    }

  if (wJCB)
    {
    delete wJCB;
    wJCB=nullptr;
    }

    if (wIndexData)
      delete wIndexData;

  //-----------------------Final report------------------------------

  wErrorLog.infoLog(
      "________________%s Report_______________\n"
      "  Missing tags                     %ld\n"
      "  Mandatory tags missing           %ld\n"
      "  Index(es) definition processed   %ld\n"
      "  Added Index(es)                  %ld\n"
      "  Errored Index(es)                %ld\n"
      "  Index(es) rebuilt                %ld\n"
      "______________________________________________________\n",
      _GET_FUNCTION_NAME_,
      wMissingTags,
      wMissMandatoryTags,
      wIndexProcessed,
      wAddedIndexes,
      wErroredIndexes,
      wRebuiltIndexes);

  ZException.printUserMessage(wOutput);
  ZException.clearStack();

  if (FOutput)
    fclose(wOutput);

  return  wSt;

ErrorcreateXMLFile:
  fprintf(stderr,
      "%s>>  **********************process ended with errors*****************\n",
      _GET_FUNCTION_NAME_);

//  wMasterFile.zclose();
  goto EndcreateXMLFile;
  //#endif // __TEMPORARY_COMMENT__
}// createFromXml






/**
 * @brief applyXmltoFile Controls and applies (if requested) an xml definition to modify parameters of an existing master file (raw or master file with dictionary)
 *
 * Xml Content :
 *
 *
 *
 * Element Missing in Xml definition file :
 *
 * If one element is missing, no modification is made on hierarcal dependent elements.<br>
 * Examples : if <zfiledescriptor> and/or <zfilecontrolblock> is/are missing all ZRandomFile parameters will no be changed.
 *
 *
 * Parameters that may miss or that could be suppressed using <suppress> option :
 *
 *  <zfiledescriptor> <zfilecontrolblock>         can not use <suppress> option [no effect if used]
 *
 *  <zmasterdictionary>                       may use <suppress> option (suppress master dictionary) NB: metadictionary not allowed
 *          <zkeydictionary>                  may use <suppress> option (suppress all dictionary keys)
 *          <key>                             may use <suppress> option (suppress one dictionary key)
 *
 *  <mastercontrolblock>                          can not use <suppress> option [no effect if used]
 *    <indexcontrolblock>                         can not use <suppress> option [no effect if used]
 *        <indextable>                            may use <suppress> option (suppress all indexes)
 *           <index>                              may use <suppress> option  (suppress individual index)
 *
 *
 *
 * Element Errored in Xml definition file:
 * if one element exists but is errored, either Xml error or check error,
 *  then ALL MODIFICATION to file will be rejected.
 *
 *
 *  update process ICB Index vs Key Dictionary
 *
 *
 *  Rule :
 *  When a key dictionary is defined ICB index must exist either created from xml definition or existing ICB index
 *
 *    ICB Index
 * Exists   has Xml     Action
 * in File
 *
 *  no        no        Nothing (must even not be tested)
 *
 *  no        yes       -> ICB Index is tagged ToBeCreated+Rebuild
 *
 *  yes       no        -> ICB index tagged ToSuppress from file (no rebuild)
 *
 *  yes       yes       -> compare indexes :  if changes -> tag Present+Rebuild -
 *                                                              if no change -> tag Present+NoChange
 *
 *      FOR A GIVEN KEY-INDEX NAME
 *
 *
 * Operation before : if master dictionary exists in file but not in Xml : tag masterdic to be deleted in file
 *                    master dictionary exists in file and in Xml : for each key : tag
 *
 *            Master Dic
 *  exist in file       exist in Xml
 *
 *    yes                   yes         for each key : check if changes tag according below rule
 *
 *    yes                   no          tag Master dic to be suppressed
 *
 *    no                    yes         tag Master dic to be created : each key is tagged ToBeCreated
 *
 *    no                    no          nothing
 *
 *
 *  Master dic exists in file and in Xml
 *
 * For each Key
 *     Exist in
 * File         Xml
 *
 *  yes         yes       check if equal : Yes tag Present+NoChange No : Present+Modified
 *
 *  yes         no        tag key ToSuppress
 *
 *  no          yes       tag key ToBeCreated
 *
 *  no          no        no action
 *
 *
 *
 *    ICB Index         Key (from KeyDic) Actions
 * Exists   has Xml     Exists  has Xml
 * in File              in File
 *
 *  no        no         no masterdic     Nothing (must even not be tested)
 *
 *  no        yes        no masterdic     -> ICB Index is tagged ToBeCreated+Rebuild
 *
 *  yes       no         no masterdic     -> ICB index tagged ToSuppress from file (no rebuild)
 *
 *  yes       yes        no masterdic     -> compare indexes :  if changes -> tag Present+Rebuild -
 *                                                              if no change -> tag Present+NoChange
 *
 *
 *  no        no        yes      no      -> Error in file : KeyDic key tag ToSuppress
 *
 *  no        no        yes     yes       -> Error in file and in Xml : KeyDic key tag ToSuppress
 *
 *  no        no        no      yes       -> Error in Xml : KeyDic key tag NoChange (KeyDic file is not modified)
 *
 *  no        yes
 *                                           no   -> no change - no check
 *
 *      yes                   yes            yes  -> check:  Xml Key Dictionary is controlled vs Xml loaded ICB Indexes
 *                                                           ICB Index xml and
 *      no                    yes
 *
 *      yes                   no
 *
 * Dictionary and index changes sequence :
 *    -load of ICB indexes
 *    -load of Master Dictionary and KeyDic if any
 *
 *    - KeyDic exists in Xml - ICB Indexes exists in Xml -> control Xml KeyDic vs Xml ICB Indexes
 *
 *                           - ICB Indexes does not exist in Xml -> ERROR
 *
 *    - Master Dictionary does not exist in Xml : no control at all  -> if exists in file Master Dic will be deleted
 *
 *
 * @param[in] pXMLPath         a valid file path pointing to an xml file full definition
 * @param[in] pContentFilePath master file file path to apply the definition to.
 * @param[in] pRealRun         if set to true, then modifications from xml file will be applied to the file.\n
 *                             if set to false, then only a test run with report will be done to evaluate the possible changes.
 * @param[in] pOutput defaulted to nullptr.
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.xmllog
 * @return
 */
ZStatus
applyXmltoFile(const char* pXMLPath,
              const char *pContentFilePath,
              bool pRealRun,
              const char* pLogfile)
{
  ZStatus     wSt;
  uriString   URIContent = pContentFilePath;
  uriString   wURIOutput;
  utf8String  wBase;
  FILE* wOutput=nullptr;
  bool FOutput=false;

  ZaiErrors wMessageLog;

  int  wAddedIndexes=0 ,
      wErroredIndexes = 0,
      wRebuiltIndexes = 0,
      wMissingTags=0 ,
      wMissMandatoryTags=0,
      wIndexProcessed=0;

  zxmlDoc* wXmlDoc=nullptr;
  zxmlElement* wRootElement=nullptr;
  zxmlElement* wFileNode=nullptr;
  zxmlElement* wFileNode_2=nullptr;
  zxmlElement* wFDNode=nullptr;
  zxmlElement* wFDNode_2=nullptr;
  zxmlElement* wFCBNode=nullptr;
  zxmlElement* wFCBNode_2=nullptr;
  zxmlElement* wMCBNode=nullptr;
  zxmlElement* wMasterDicNode=nullptr;
  zxmlElement* wMetaDicNode=nullptr;
  zxmlElement* wKeyDicNode=nullptr;
  zxmlElement* wIndexTableNode=nullptr;
  zxmlElement* wICBNode=nullptr;
  zxmlElement* wJCBNode=nullptr;

  ZElementCollection wIndexElts;


  ZFile_type wFileType=ZFT_ZRawMasterFile;
  int wFileTypeI=0;

  ZRawMasterFile          wMasterFile;


  /* ZRandomFile FCB data */

  struct MCB_st
  {
    utf8String IndexFilePath;
  } ;


  FCBParams*            wMasterFCB=nullptr;
  ZSJournalControlBlock*wJCB=nullptr;

  ZMFDictionary*        wMasterDic=nullptr;
//  ZArray<ZPRES>*        wKeyStatus=nullptr;

  MCB_st                wMCB;
  ZArray<IndexData_st>* wIndexData=nullptr;

  ZPRES                 wMetaDicStatus=ZPRES_Unchanged;
  ZPRES                 wKeyDicStatus=ZPRES_Unchanged;
  ZPRES                 wIndexTableStatus=ZPRES_Unchanged;
  ZPRES                 wJCBStatus=ZPRES_Unchanged;

  bool                  wMasterDicSuppress=false;
  bool                  wKeyDicSuppress=false;
  bool                  wKeySuppress=false;

  bool                  wCannotProcess=false; /* if set to true : cannot apply any modification */

  /* meta dictionary */
  int                 wKeyCount=-1; /* default value <field omitted> */
  ZElementCollection  wFieldsElts;
  ZElementCollection  wKeyDicElts;


  if (pLogfile==nullptr)
  {
    wURIOutput=pXMLPath;
    wBase=wURIOutput.getBasename().toCChar();
    wBase+=".xmllog";
  }//if (pLogfile==nullptr)
  else
    wBase=pLogfile;

  wMessageLog.setErrorLogContext("applyXmltoFile");
  wMessageLog.setAutoPrintOn(true);
  if (!wBase.isEmpty())
    wMessageLog.setOutput(wBase.toCChar());
  else
    wMessageLog.setOutput("applyXmltoFile.log");

  utf8String wT;
  wT.sprintf("Applying xml definition file <%s> to master file <%s> ",
              pXMLPath,
              pContentFilePath);
  int wS=(int)wT.getUnitCount();


  wMessageLog.textLog(
      "%*c\n"
      " %s\n"
      "%*c\n"
      "            Run options are \n"
      "                 - %s\n"
      "                 - %s\n"
      ,
      wS,'_',
      wT.toCChar(),
      wS,'_',
      pRealRun?"Real run (valid changes are made)":"Test run (no change made)");

  wSt=wMasterFile.zopen(URIContent,ZRF_Exclusive);
  if (wSt!=ZS_SUCCESS)
    {
    wMessageLog.logZStatus(ZAIES_Error,wSt,"Cannot open file to process <%s>.",URIContent.toCChar());
    wMessageLog.logZException();
    goto ErrorapplyXMLtoFile;
    }

  wFileTypeI=wFileType= (ZFile_type)wMasterFile.getFileType();
  wMCB.IndexFilePath = wMasterFile.IndexFilePath;

/*
  if (wMasterFile.hasDictionary())
    {
    wMasterDic=new ZMFDictionary(*wMasterFile.MasterDic);
    wIndexStatus.allocateCurrentElements(wMasterFile.MasterDic->KeyDic.count());
    for (long wi=0;wi < wIndexStatus.count();wi++)
      wIndexStatus[wi]=ZPRES_Nothing;
    }
  else
    {
    wMasterDic=nullptr;
    wIndexStatus.clear();
    }
*/

  wMessageLog.textLog("Loading xml data...\n");

  wSt=_XMLLoadAndControl(pXMLPath,
                        wXmlDoc,
                        wRootElement,
                        "zicm",
                        __ZRF_XMLVERSION_CONTROL__,
                        &wMessageLog,
                        wOutput);
  if (wSt!=ZS_SUCCESS)
    {
    goto ErrorapplyXMLtoFile;
    }

  wSt=wRootElement->getChildByName((zxmlNode* &)wFileNode,"zfile");
  XMLderegister(wRootElement);
  if (wSt!=ZS_SUCCESS)
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          "XML error : corrupted or empty file <%s>. Mandatory tag <file> was expected and not found.",
          pXMLPath);

      wMessageLog.logZStatus(ZAIES_Error, wSt,"XML error : corrupted or empty file <%s>. Mandatory tag <file> was expected and not found.",
          pXMLPath);
      goto ErrorapplyXMLtoFile;
    }

  while (true) // zfiledescriptor
    {

    if (zmuXMLgetChild(wFileNode,"filetype",wFileTypeI,&wMessageLog,true,ZAIES_Error)!=ZS_SUCCESS)
      {
        wMessageLog.textLog("Assuming ZFT_ZRawMasterFile (value %d).",(int)wFileType);
      }
      else
      {
        wFileType =(ZFile_type)wFileTypeI;
        wMessageLog.textLog("Found file type <%s>(value %d).",decode_ZFile_type(wFileType), wFileTypeI);
      }

    wSt=wFileNode->getChildByName((zxmlNode*&) wFDNode,"zfiledescriptor");
    if (wSt!=ZS_SUCCESS)
    {
      wMessageLog.errorLog("Cannot find node <zfiledescriptor>. No File control block data parameter change.");
      break;
    }

    /* looking for <zfilecontrolblock> where usable/operational data for ZRandomFile is located */

    wSt=wFDNode->getChildByName((zxmlNode*&) wFCBNode,"zfilecontrolblock");
    if (wSt!=ZS_SUCCESS)
    {
      wMessageLog.infoLog("Node <zfilecontrolblock> is missing. No File control block data parameter change.");
      break;
    }

    /* FCB will be changed : setup temporary FCB with current file values and acquire available parameters */

    wMasterFCB = new FCBParams;
    wMasterFCB->AllocatedBlocks = wMasterFile.getFCB()->AllocatedBlocks;
    wMasterFCB->InitialSize = wMasterFile.getFCB()->InitialSize;

    wMasterFCB->BlockExtentQuota = wMasterFile.getFCB()->BlockExtentQuota;
    wMasterFCB->BlockTargetSize = wMasterFile.getFCB()->BlockTargetSize;
    wMasterFCB->HighwaterMarking = wMasterFile.getFCB()->HighwaterMarking;
    wMasterFCB->GrabFreeSpace = wMasterFile.getFCB()->GrabFreeSpace;

    /*
     * AllocatedBlocks and InitialSize are only used when creating the file
     *
     * When modifying Master File, those 2 parameters are used to create index files,
     * but they are taken from effective, current Master values already stored in wMasterFCB.
     *
    wMasterFCB->AllocatedBlocks=wMasterFile.getFCB()->AllocatedBlocks;
    wSt=zmuXMLgetChild(wFCBNode,"allocatedblocks",wMasterFCB->AllocatedBlocks,wMissMandatoryTags,&wErrorLog,ZAIES_Error);
    wMasterFCB->InitialSize=wMasterFile.getFCB()->InitialSize;
    wSt=zmuXMLgetChild(wFCBNode,"initialsize",wMasterFCB->InitialSize,wMissMandatoryTags,&wErrorLog,ZAIES_Error);
    */
    wMasterFCB->BlockExtentQuota=wMasterFile.getFCB()->BlockExtentQuota;
    wSt=zmuXMLgetChild(wFCBNode,"blockextentquota",wMasterFCB->BlockExtentQuota,&wMessageLog,true,ZAIES_Error);

    wMasterFCB->BlockTargetSize=wMasterFile.getFCB()->BlockTargetSize;
    wSt=zmuXMLgetChild(wFCBNode,"blocktargetsize",wMasterFCB->BlockTargetSize,&wMessageLog,true,ZAIES_Error);
    wMasterFCB->HighwaterMarking=wMasterFile.getFCB()->HighwaterMarking;
    wSt=zmuXMLgetChild(wFCBNode,"highwatermarking",wMasterFCB->HighwaterMarking,&wMessageLog,false,ZAIES_Warning);
    wMasterFCB->GrabFreeSpace=wMasterFile.getFCB()->GrabFreeSpace;
    wSt=zmuXMLgetChild(wFCBNode,"grabfreespace",wMasterFCB->GrabFreeSpace,&wMessageLog,false,ZAIES_Warning);

    break;
  }// while true zfiledescriptor


  /*---------------------------------------------------------*/


  /* looking for <zmastercontrolblock> where usable/operational data for ZRawMasterFile is located */

  wMessageLog.textLog("Processing master control block.\n");


  wSt=wFileNode->getChildByName((zxmlNode*&) wMCBNode,"zmastercontrolblock");
  if (wSt!=ZS_SUCCESS)
    {
    wMessageLog.errorLog( "Cannot find node <zmastercontrolblock>. No raw index and/or dictionary modifications.\n");
    goto PostMCBapplyXMLtoFile;
    }

  /* MCB components :

     *  - IndexTable : array of ZSIndexFile
     *      - ZIndexControlBlock : key size and parameters
     *      - ZRandomFile parameters : target block size is dependent from key size + sizeof address_type
     *
     *
     *  - Master dictionary
     *      - meta dictionary : array of field definitions S
     *      - keyDictionary array
     *            array of keyfields referring to meta dictionary field
     */



  if (XMLgetChildText(wMCBNode,"indexfilepath",wMCB.IndexFilePath,&wMessageLog,ZAIES_Warning)<0)
    {
    wMessageLog.infoLog("Optional <indexfilepath> not found : index files is be located in same directory than master content.");
    wMCB.IndexFilePath.clear();
    }
  else
    wMessageLog.textLog("Got <indexfilepath> value <%s>.",wMCB.IndexFilePath.toCChar());


  /****************************************************************
   *  processing index table array (raw index definition)
   ****************************************************************/

  while (true)
    {
    wSt=wMCBNode->getChildByName((zxmlNode*&) wIndexTableNode,"indextable");
    if (wSt!=ZS_SUCCESS)
      {
      wMessageLog.warningLog("Cannot find node <indextable>. Xml definition has no raw index defined. (Not an error)\n");
      wSt = ZS_XMLWARNING;
      wIndexTableStatus=ZPRES_Unchanged;
      break ;
      }


    /* ******************
     * find if <suppress> : index table is explicitly requested to be suppressed
       *******************/
    bool wIndexTableSuppress=false;
    if (XMLgetChildBool(wIndexTableNode,"suppress",wIndexTableSuppress,&wMessageLog,ZAIES_Info)==0)
    {
      wMessageLog.infoLog("<suppress> keyword with value <%s> found for node <indextable>",
          wIndexTableSuppress?"true":"false");

      if (wIndexTableSuppress)
        {
        wMessageLog.textLog("It is requested to suppress whole index table. Stopping Xml parsing for indexes. ");
        if (wMasterFile.hasDictionary() && (wMasterFile.IndexTable.count()>0))
          wIndexTableStatus = ZPRES_ToBeDeleted;  /* request to delete the whole index table*/
        else
          {
          wMessageLog.infoLog("It is requested to suppress index table and index table is empty in file.");
          wIndexTableStatus = ZPRES_Unchanged;  /* do not change */
          }
        break;
        }//if (wMasterDicSuppress)

      wMessageLog.infoLog("Found node <suppress> but set to value false. No suppression will be done. Continuing parsing xml for modifications.");
      wIndexTableStatus = ZPRES_Unchanged;     /* do not touch */
    }// suppress index table





    wIndexElts = wIndexTableNode->getAllChildren("index");
    if (wIndexElts.count()==0)
      {
      wMessageLog.errorLog("Cannot find any index within Master Control Block <indextable> definition.\n"
                           "Indexes will not be changed.");
      break;
      }

    XMLderegister(wIndexTableNode);

    for (long wi=0;wi < wIndexElts.count();wi++)
    {
      bool wMayCreateIndex=true;
      IndexData_st wIData;
      while (true) // ================while true   wIndexElts[]  ======================
        {
          wMessageLog.infoLog("Found node <suppress> but set to value false. No suppression will be done. Continuing parsing xml for modifications.");
          wIndexTableStatus = ZPRES_Unchanged;     /* do not touch */

        /* first get ZIndexControlBlock operational data */

        wSt=wIndexElts[wi]->getChildByName((zxmlNode*&) wICBNode,"indexcontrolblock");
        if (wSt!=ZS_SUCCESS)
          {
          wMessageLog.errorLog("Cannot find required node <indexcontrolblock> for index rank <%ld>.\n"
                             "Malformed xml definition. Stopping xml parsing for indexes.\n"
                             "No index will be created.",wi);
          wMayCreateIndex=false;
          break;
          }

  /* get index data : indexname is index descriptor */

        wSt=zmuXMLgetChild(wICBNode,"indexname",wIData.IndexName,&wMessageLog,true,ZAIES_Error);
        if (wSt!=ZS_SUCCESS)
          {
          wMayCreateIndex = false;
          break;
          }
        /* check existence within master file */

        long wh=wMasterFile.IndexTable.searchCaseIndexByName(wIData.IndexName.toCChar());
        if (wh < 0)
          wIData.Status = ZPRES_ToCreate;
        else
          wIData.Status = ZPRES_ToChange;


      /*******************
       * find if <suppress> : individual index  is explicitly requested to be suppressed (search by index name)
       *******************/
          bool wIndexSuppress=false;
          if (XMLgetChildBool(wIndexTableNode,"suppress",wIndexSuppress,&wMessageLog,ZAIES_Info)==0)
          {
            wMessageLog.infoLog("<suppress> keyword with value <%s> found for node <index>",
                wIndexSuppress?"true":"false");
            if (wIData.IndexName.isEmpty())
            {
              wMessageLog.errorLog("Using <suppress> clause while index name is missing : Xml definition is corrupted.\n"
                                   "No modification will be made to file.");
              wCannotProcess=true;
              break;
            }
          if (wh<0)
              {
              wMessageLog.warningLog("Using <suppress> clause while index name <%s> does not exist in file.\n Skipping that Xml index definition.");
              continue;
              }
            if (wIndexSuppress)
            {
              wIData.Status = ZPRES_ToBeDeleted;
              wIndexData->push(wIData);   /* need only name and status set to tobedeleted */
              continue;
            }
          }//find if <suppress>


        wSt=zmuXMLgetChild(wICBNode,"keyuniversalsize",wIData.KeyUniversalSize,&wMessageLog,true,ZAIES_Error);
        if (wSt!=ZS_SUCCESS)
          wMayCreateIndex = false;
        wIData.Duplicates=ZST_NODUPLICATES;
        wSt=zmuXMLgetChild(wICBNode,"duplicates",(int&)wIData.Duplicates,&wMessageLog,true,ZAIES_Warning);

        /* check if there is already an ICB with an index name equal to that one : errored index control block */

        for (long wj=0;wj < wIndexData->count() ; wj++)
          {
          if (wIndexData->Tab[wj].IndexName==wIData.IndexName)
            {
            wMessageLog.errorLog("Within Xml : two indexes has same index name <%s> -> index rank <%ld> and <%ld>.\n"
                                 "Malformed xml definition. Stopping xml parsing for indexes.\n",wi,wj);
            wMayCreateIndex=false;
            break;
            }
          }//for (long wh




        /* then get Index file FCB operational data */

        /* compute default/recommended parameter values for index file */

        if (wMasterFCB==nullptr)
          {
          wIData.FCB.AllocatedBlocks =cst_ZRF_default_allocation;
          wIData.FCB.BlockExtentQuota = cst_ZRF_default_extentquota ;
          }
        else
          {
          wIData.FCB.AllocatedBlocks = wMasterFCB->AllocatedBlocks ; /* Index has same allocated blocks as master */
          wIData.FCB.BlockExtentQuota = wMasterFCB->BlockExtentQuota ; /* Index has same extent quota as master */
          }
        /* key size dependent values */
        wIData.FCB.InitialSize =  wIData.FCB.AllocatedBlocks * ( wIData.KeyUniversalSize + sizeof(zaddress_type)) ;
        wIData.FCB.BlockTargetSize = wIData.KeyUniversalSize + sizeof(zaddress_type) ;
        /* other options */
        wIData.FCB.HighwaterMarking = false ;
        wIData.FCB.GrabFreeSpace = false;


        wSt=wIndexElts[wi]->getChildByName((zxmlNode*&) wFileNode_2,"file");
        if (wSt!=ZS_SUCCESS)
          {
          wMessageLog.warningLog("For index named <%s> cannot find node <file>.\n"
                                 "Using computed File Control Block values %s",
                                  wIData.IndexName.toCChar(),
                                  wMasterFCB==nullptr?"computed from default values":"computed from master file FCB values");

          break ;
          }
        wSt=wFileNode_2->getChildByName((zxmlNode*&) wFDNode_2,"zfiledescriptor");
        if (wSt!=ZS_SUCCESS)
          {
          wMessageLog.warningLog("Cannot find node <zfiledescriptor>.\n"
                                 " Using computed File Control Block values for Index named <%s>.",
                                  wIData.IndexName.toCChar());

          XMLderegister(wFileNode_2);
          break;
          }
        wSt=wFDNode_2->getChildByName((zxmlNode*&) wFCBNode_2,"zfilecontrolblock");
        if (wSt!=ZS_SUCCESS)
          {
          wMessageLog.warningLog("Cannot find node <zfilecontrolblock>.\n"
                               "Using computed File Control Block values for Index named <%s>.",
                                wIData.IndexName.toCChar());

          XMLderegister(wFileNode_2);
          XMLderegister(wFDNode_2);
          break;
          }

        /* acquiring ZRandomFile parameters for index file creation */

        wSt=zmuXMLgetChild(wFCBNode_2,"allocatedblocks",wIData.FCB.AllocatedBlocks,&wMessageLog,false,ZAIES_Warning);
        wSt=zmuXMLgetChild(wFCBNode_2,"blockextentquota",wIData.FCB.BlockExtentQuota,&wMessageLog,false,ZAIES_Warning);
        wSt=zmuXMLgetChild(wFCBNode_2,"initialsize",wIData.FCB.InitialSize,&wMessageLog,false,ZAIES_Warning);
        wSt=zmuXMLgetChild(wFCBNode_2,"blocktargetsize",wIData.FCB.BlockTargetSize,&wMessageLog,false,ZAIES_Warning);
        wSt=zmuXMLgetChild(wFCBNode_2,"highwatermarking",wIData.FCB.HighwaterMarking,&wMessageLog,false,ZAIES_Warning);
        wSt=zmuXMLgetChild(wFCBNode_2,"grabfreespace",wIData.FCB.GrabFreeSpace,&wMessageLog,false,ZAIES_Warning);


        XMLderegister(wFileNode_2);
        XMLderegister(wFDNode_2);
        XMLderegister(wFCBNode_2);

        break;
      }// ================while true wIndexElts[] ======================

      if (!wMayCreateIndex)
        {
        wMessageLog.infoLog("***** Xml Index definition has non recoverable errors.Index rank %ld will not be created.******\n"
                            "All modifications to file will be rejected.",wi);
        wCannotProcess=true;
        }
      else
        wIndexData->push(wIData);

    } // for (long wi=0;wi < wIndexElts.count();wi++)

    XMLderegister(wIndexTableNode);

    break;
    } // while (true) <indextable>

PostMCBapplyXMLtoFile:

  //---------------------------Journaling---------------------------------
  /*
    <zjournalcontrolblock>
      <journalingon>true</journalingon>
      <keep>true</keep>
      <journallocaldirectorypath></journallocaldirectorypath> <!-- omitted : same directory as ZRawMasterFile's path  -->
      <JournalingMaxDepth>-1</JournalingMaxDepth> <!-- max number of journaling records to keep. -1 : infinite. in this case need to use purgeJournaling(keepranks) regularly -->
      <zremotemirroring>
        <protocol>ZJP_RPC|ZJP_SSLV5</protocol> <!--   -->
        <addresstype>ZHAT_IPV4</addresstype> <!-- values ZHAT_IPV4:AF_INET ZHAT_IPV6:AF_INET6 - default:ZHAT_IPV4 -->
        <port> 1156 </port>
        <host>JournalServer</host>
        <authenticate> <authenticate>
        <sslkey>  </sslkey>
        <sslvector> </sslvector>
      </zremotemirroring>
    </zjournalcontrolblock>
*/

  /* load journal control block data */
  while (true)
  {
    if (wMCBNode->getChildByName((zxmlNode*&)wJCBNode,"zjournalcontrolblock")!=ZS_SUCCESS)
    {
      wMessageLog.infoLog("No journal control block definition in Xml model loaded.\n"
                          "Journal control block if exists will remain unchanged.");
      break;
    }
    wJCB= new ZSJournalControlBlock;
    wSt=wJCB->fromXml(wMCBNode,&wMessageLog,ZAIES_Warning);
    if (wSt!=ZS_SUCCESS)
    {
      wMessageLog.errorLog(" Cannot load journaling data. Malformed Xml definition. Modifications\n");

      delete wJCB;
      wJCB=nullptr;
      break;
    }
    if (wMasterFile.ZJCB!=nullptr)
    {
      if (wMasterFile.ZJCB->_isSameAs(wJCB))
        break;
      delete wMasterFile.ZJCB;
    }
    wMasterFile.ZJCB = new ZSJournalControlBlock(*wJCB);
    wMessageLog.textLog("Replaced ZSJournalControlBlock.");
    delete wJCB;
    break;
  }//while (true)


  /* back process */

  if (wCannotProcess)
  {
    wMessageLog.errorLog("Errors have been detected that prevent modifying  MasterFile file control block parameters");
  }


  if (wMasterFCB!=nullptr)
  {
  wMessageLog.textLog("_____________File change from collected parameters._____________________");

  wMessageLog.textLog("About to change MasterFile file control block parameters with collected and/or defaulted data ;\n"
                    "   File type             %s\n"
                    "   Allocated blocks      %ld <not updated>\n"
                    "   Block extent quota    %ld\n"
                    "   Block target size     %ld\n"
                    "   Initial size          %ld <not updated>\n"
                    "   Highwater marking     %s\n"
                    "   Grab free space       %s\n",
                    decode_ZFile_type( wFileType),
                    wMasterFCB->AllocatedBlocks,
                    wMasterFCB->BlockExtentQuota,
                    wMasterFCB->BlockTargetSize,
                    wMasterFCB->InitialSize,
                    wMasterFCB->HighwaterMarking?"yes":"no",
                    wMasterFCB->GrabFreeSpace?"yes":"no"
                    );

  if (pRealRun)
    {
    while (true)
      {
      if (wCannotProcess)
        {
          break;
        }

      wSt=wMasterFile.setFCBParameters(wFileType,
          wMasterFCB->GrabFreeSpace,wMasterFCB->HighwaterMarking,wMasterFCB->BlockTargetSize,wMasterFCB->BlockExtentQuota);
      if (wSt!=ZS_SUCCESS)
          {
          wMessageLog.errorLog(" Master file <%s> has not been modified. Aborting modification process.",pContentFilePath);
          wMessageLog.logZException();
          wCannotProcess=true;
          break;
          }
      wMessageLog.textLog("Master file <%s> has been modified.\n",pContentFilePath);
      }//while (true)
    }// if pRealRun

  }//if (wMasterFCB!=nullptr)


  if ((wJCB!=nullptr)&&!wCannotProcess)
    {
    wMasterFile.ZJCB = new ZSJournalControlBlock(*wJCB);
    wMasterFile.setJournalingOn();
    wMessageLog.textLog(" Journaling has been defined and enabled.\n");
    }


/*================Index table=============================*/

  if (wIndexData!=nullptr)
  {
    for (long wi=0; wi < wIndexData->count() ; wi++)
    {
      switch(wIndexData->Tab[wi].Status)
      {

      case ZPRES_ToBeDeleted:
        {
        long wh=wMasterFile.IndexTable.searchCaseIndexByName(wIndexData->Tab[wi].IndexName);
        if (wh < 0)
          continue;

        wMessageLog.textLog("About to remove index <%s> from Index table.\n",wIndexData->Tab[wi].IndexName.toCChar());

        wMessageLog.textLog("About to remove Index\n"
                            "   _______ICB____________\n"
                            "   Index name            <%s>\n"
                            "   Key universal size    %d\n"
                            "   Duplicates            %s\n"
                            "   ______________________\n",
            wIndexData->Tab[wi].IndexName.toCChar(),
            wIndexData->Tab[wi].KeyUniversalSize,
            wIndexData->Tab[wi].Duplicates== ZST_NODUPLICATES?"no duplicate":"allows duplicates"
            );


        if ((pRealRun)&&!wCannotProcess)
          {

          wSt=wMasterFile.zremoveIndex(wh,&wMessageLog);
          if (wSt==ZS_SUCCESS)
              wMessageLog.textLog("done.\n");
          else
            wMessageLog.textLog("Couldn't remove index <%s>.\n",wIndexData->Tab[wi].IndexName.toCChar());
          }
        break;
        }//ZPRES_ToBeDeleted

      case ZPRES_ToCreate:
        {
          long wh=wMasterFile.IndexTable.searchCaseIndexByName(wIndexData->Tab[wi].IndexName);
          if (wh >= 0)
            {
            wMessageLog.errorLog("Index <%s> was supposed to be created while it exists within file. Creation rejected. Try to change.",wIndexData->Tab[wi].IndexName.toCChar());
            wIndexData->Tab[wi].Status=ZPRES_ToChange;  /* set status to appropriate */
            wi--;                                       /* reprocess same rank */
            continue;
            }
          wMessageLog.textLog("About to create Index with collected and/or defaulted data \n"
                              "   _______ICB____________\n"
                              "   Index name            <%s>\n"
                              "   Key universal size    %d\n"
                              "   Duplicates            %s\n"
                              "   _______FCB____________\n"
                              "   Allocated blocks      %ld\n"
                              "   Block extent quota    %ld\n"
                              "   Block target size     %ld\n"
                              "   Initial size          %ld\n"
                              "   Highwater marking     %s\n"
                              "   Grab free space       %s\n"
                              "   ______________________\n",
              wIndexData->Tab[wi].IndexName.toCChar(),
              wIndexData->Tab[wi].KeyUniversalSize,
              wIndexData->Tab[wi].Duplicates== ZST_NODUPLICATES?"no duplicate":"allows duplicates",
              wIndexData->Tab[wi].FCB.AllocatedBlocks,
              wIndexData->Tab[wi].FCB.BlockExtentQuota,
              wIndexData->Tab[wi].FCB.BlockTargetSize,
              wIndexData->Tab[wi].FCB.InitialSize,
              wIndexData->Tab[wi].FCB.HighwaterMarking?"yes":"no",
              wIndexData->Tab[wi].FCB.GrabFreeSpace?"yes":"no"
              );

        if ((pRealRun)&&!wCannotProcess)
          {
            wSt=wMasterFile.zcreateRawIndexDetailed(  wIndexData->Tab[wi].IndexName,
                wIndexData->Tab[wi].KeyUniversalSize,
                wIndexData->Tab[wi].Duplicates,
                wIndexData->Tab[wi].FCB.AllocatedBlocks,
                wIndexData->Tab[wi].FCB.BlockExtentQuota,
                wIndexData->Tab[wi].FCB.InitialSize,
                wIndexData->Tab[wi].FCB.HighwaterMarking,
                wIndexData->Tab[wi].FCB.GrabFreeSpace,
                false);
            if (wSt==ZS_SUCCESS)
              wMessageLog.infoLog(" Index structure and files created. Index built.");
            else
            {
              wMessageLog.errorLog("********* Cannot create and/or build index. Exception follows ***********");
              wMessageLog.logZException();
            }

          break;
        } //if ((pRealRun)&&!wCannotProcess)
      }//ZPRES_ToCreate

      case ZPRES_ToChange:
      case ZPRES_Nothing:
      {
        long wh=wMasterFile.IndexTable.searchCaseIndexByName(wIndexData->Tab[wi].IndexName);
        if (wh<0) /* That cannot be */
        {
          break;
        }
        wMessageLog.textLog("About to modify Index with collected and/or defaulted data \n"
                            "Former data\n"
                            "   _______ICB____________\n"
                            "   Index name            <%s>\n"
                            "   Key universal size    %d\n"
                            "   Duplicates            %s\n"
                            "   _______FCB____________\n"
                            "   Block extent quota    %ld\n"
                            "   Block target size     %ld\n"
                            "   Highwater marking     %s\n"
                            "   Grab free space       %s\n"
                            "   ______________________\n",
            wMasterFile.IndexTable[wh]->IndexName.toCChar(),
            wMasterFile.IndexTable[wh]->KeyUniversalSize,
            wMasterFile.IndexTable[wh]->Duplicates== ZST_NODUPLICATES?"no duplicate":"allows duplicates",
            wMasterFile.IndexTable[wh]->getFCB()->BlockExtentQuota,
            wMasterFile.IndexTable[wh]->getFCB()->BlockTargetSize,
            wMasterFile.IndexTable[wh]->getFCB()->HighwaterMarking?"yes":"no",
            wMasterFile.IndexTable[wh]->getFCB()->GrabFreeSpace?"yes":"no"
            );

        wMessageLog.textLog("Xml and/or computed data\n"
                            "   _______ICB____________\n"
                            "   Index name            <%s>\n"
                            "   Key universal size    %d\n"
                            "   Duplicates            %s\n"
                            "   Block extent quota    %ld\n"
                            "   Block target size     %ld\n"
                            "   Highwater marking     %s\n"
                            "   Grab free space       %s\n"
                            "   ______________________\n",
            wIndexData->Tab[wi].IndexName.toCChar(),
            wIndexData->Tab[wi].KeyUniversalSize,
            wIndexData->Tab[wi].Duplicates== ZST_NODUPLICATES?"no duplicate":"allows duplicates",
            wIndexData->Tab[wi].FCB.BlockExtentQuota,
            wIndexData->Tab[wi].FCB.BlockTargetSize,
            wIndexData->Tab[wi].FCB.HighwaterMarking?"yes":"no",
            wIndexData->Tab[wi].FCB.GrabFreeSpace?"yes":"no"
            );

      if ((pRealRun)&&!wCannotProcess)
        {
        wMasterFile.IndexTable[wh]->KeyUniversalSize=wIndexData->Tab[wi].KeyUniversalSize;
        wMasterFile.IndexTable[wh]->Duplicates=wIndexData->Tab[wi].Duplicates;
        wMasterFile.IndexTable[wh]->setParameters(wIndexData->Tab[wi].FCB.GrabFreeSpace,
                                                        wIndexData->Tab[wi].FCB.HighwaterMarking,
                                                        wIndexData->Tab[wi].FCB.BlockTargetSize,
                                                        wIndexData->Tab[wi].FCB.BlockExtentQuota);
        wMasterFile.writeControlBlocks();
        wSt=wMasterFile.zindexRebuild(wh,true);
        if (wSt==ZS_SUCCESS)
          wMessageLog.infoLog(" Index structure and files modified. Index rebuilt.");
        else
        {
          wMessageLog.errorLog("********* Cannot modify and/or rebuild index. Exception follows ***********");
          wMessageLog.logZException();
        }
        }

      }//ZPRES_ToChange
      }//switch

    }//for (long wi=0; wi < wIndexData.count() ; wi++)    
  }//if (wIndexData!=nullptr





  wMasterFile.zclose();

EndapplyXMLtoFile:

  XMLderegister(wFileNode);
  XMLderegister(wMCBNode);
  XMLderegister(wIndexTableNode);
  XMLderegister(wFileNode_2);
  XMLderegister(wFDNode_2);
  XMLderegister(wFDNode);
  XMLderegister(wFCBNode);
  XMLderegister(wMasterDicNode);
  XMLderegister(wJCBNode);

  if (wMasterFCB)
    {
    delete wMasterFCB;
    wMasterFCB=nullptr;
    }

  if (wMasterDic)
    {
    delete wMasterDic;
    wMasterDic=nullptr;
    }

  if (wJCB)
    {
    delete wJCB;
    wJCB=nullptr;
    }

  //-----------------------Final report------------------------------

  wMessageLog.textLog(
      "________________%s Report_______________\n"
      "  Missing tags                     %ld\n"
      "  Mandatory tags missing           %ld\n"
      "  Index(es) definition processed   %ld\n"
      "  Added Index(es)                  %ld\n"
      "  Errored Index(es)                %ld\n"
      "  Index(es) rebuilt                %ld\n"
      "______________________________________________________\n",
      _GET_FUNCTION_NAME_,
      wMissingTags,
      wMissMandatoryTags,
      wIndexProcessed,
      wAddedIndexes,
      wErroredIndexes,
      wRebuiltIndexes);

  if (wCannotProcess)
    wMessageLog.textLog("Errors prevented file modification.\n");

  ZException.printUserMessage(wOutput);
  ZException.clearStack();

  if (FOutput)
    fclose(wOutput);

  return  wSt;

ErrorapplyXMLtoFile:
  fprintf(stderr,
      "%s>>  **********************process ended with errors*****************\n",
      _GET_FUNCTION_NAME_);

  wMasterFile.zclose();
  goto EndapplyXMLtoFile;
}// applyXmltoFile



/**
 * @brief ZRawMasterFile::_XMLLoadAndControl Loads an XML document and Makes all XML controls to have an appropriate <zicm> document
 *
 *  Controls are made on
 *  - xml coherence
 *  - Root node existence
 *  - Root node tag name validity using pRootName
 *  - Root node Element attribute. Optional control : if pRootAttrName is omitted (nullptr), no control is done.
 *
 *  It delivers the ROOT node ELEMENT after having done the controls against
 *    - root element name
 *    - root element attribute name and value
 *
 * @param[in] pFilePath     a Cstring pointing to xml file to load and control
 * @param[out]pXmlDoc       The XML document content loaded as a return
 * @param[out]pRootElement  The First node under the root name to exploit as a return
 * @param[in] pRootName     Root node tag name we are searching for
 * @param[in] pRootAttrName Root node Element attribute name. If nullptr then no control is made.
 * @param[in] pRootAttrValue Root node Element attribute expected value
 * @return
 */
ZStatus
_XMLLoadAndControl(const char *pFilePath,
                    zxmlDoc* &    pXmlDoc,
                    zxmlElement* &pRootElement,
                    const char *pRootName,
                    const char *pVersion,
                    ZaiErrors*   pErrorLog,
                    FILE*        pOutput)
{
  uriString wUriPath;
  ZDataBuffer wXMLcontent;
  ZStatus wSt;

  utf8String wN;
  utf8String wAttribute;

  wUriPath = (const utf8_t*)pFilePath;

  if (!wUriPath.exists())
  {
    pErrorLog->errorLog("_XMLLoadAndControl-E-FILNTFND File <%s> does not exist while trying to load it.",pFilePath);
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_FILENOTEXIST,
        Severity_Error,
        " File <%s> does not exist while trying to load it",
        pFilePath);
    return  ZS_FILENOTEXIST;
  }
  if (pOutput)
    pErrorLog->textLog("_XMLLoadAndControl-I-LDFIL loading xml file %s",
        pFilePath);

  wSt=wUriPath.loadContent(wXMLcontent);
  if (wSt!=ZS_SUCCESS)
  {
    pErrorLog->textLog("_XMLLoadAndControl-E-LDFIL Error loading file %s",
        pFilePath);
    return  wSt;
  }
  pXmlDoc=new zxmlDoc;
  wSt=pXmlDoc->ParseXMLDocFromMemory(wXMLcontent.DataChar,wXMLcontent.Size,"utf8",0);
  if (wSt!=ZS_SUCCESS)
  {
    pErrorLog->textLog("_XMLLoadAndControl-E-PARSDOC Error parsing xml document",
        pFilePath);
    ZException.setMessage(_GET_FUNCTION_NAME_,
        wSt,
        Severity_Error,
        " XML Error(s) have been reported while parsing document");
    return  wSt;
  }

  wSt=pXmlDoc->getRootElement(pRootElement);
  if (wSt!=ZS_SUCCESS)
  {
    pErrorLog->infoLog("_XMLLoadAndControl-E-ROOTELT Root element not found in xml document <%s>",
        pFilePath);
    delete pXmlDoc;
    return wSt;
  }
  if (pRootName!=nullptr)
  {
    if (pRootElement->getName()!=pRootName)
    {
      pErrorLog->errorLog("_XMLLoadAndControl-E-ROOTELT Root element <%s> not found in xml document <%s>",
          pRootName,
          pFilePath);
      XMLderegister(pRootElement);
      delete pXmlDoc;
      return wSt;
    }

    wSt=pRootElement->getAttributeValue((const utf8_t*)"version",wAttribute);
    if (wSt!=ZS_SUCCESS)
    {
      pErrorLog->logZStatus(ZAIES_Error,wSt,"_XMLLoadAndControl-E-ATTRMIS Root element attribute <%s> not found in xml document <%s>",
          "version",
          pFilePath);

      XMLderegister(pRootElement);
      delete pXmlDoc;
      return wSt;

      if (wAttribute!=pVersion)
      {
        pErrorLog->errorLog("_XMLLoadAndControl-E-ATTRVAL Root element attribute <%s> has wrong value <%s> while <%s> was expected in xml document <%s>",
            "version",
            wAttribute.toCChar(),
            pVersion,
            pFilePath);
/*        ZException.setMessage(_GET_FUNCTION_NAME_,
            wSt,
            Severity_Error,
            "Root element attribute <%s> has wrong value <%s> while <%s> was expected in xml document <%s>",
            "version",
            wAttribute.toCChar(),
            pVersion,
            pFilePath);*/
        XMLderegister(pRootElement);
        delete pXmlDoc;
        return ZS_INVVALUE;
      }
    }//if (pRootAttrName!=nullptr)
    }//if (pRootName!=nullptr


  if (pOutput)
    fprintf(pOutput,"%s>> load and control on xml file %s has been done successfully\n",
        _GET_FUNCTION_NAME_,
        pFilePath);
  return  ZS_SUCCESS;
} //_XMLLoadAndControl





ZStatus
createZRandomFileFromXml(const char* pXMLPath,
                          const char *pContentFilePath,
                          bool pRealRun,
                          const char *pLogfile)
{
  ZStatus       wSt;
  zxmlDoc*      wXmlDoc=nullptr;
  zxmlElement*  wRootElement=nullptr,*wFileElement=nullptr,*wFCBElement=nullptr;
  zxmlNode *wFDNode=nullptr,*wFCBNode=nullptr;

  utf8String    wValue;
  int           wIntValue=0;
  bool          wBoolValue=false;

  ZaiErrors     wErrorLog;

  utf8String  wBase;
  uriString   URIContent;
  uriString   wURIOutput;

  FILE* wOutput=nullptr;
  bool FOutput=false;


  zbs::ZRandomFile       wZRF;
  ZFileControlBlock wZFCB;


  if (pLogfile==nullptr)
  {
    wURIOutput=pXMLPath;
    wBase=wURIOutput.getBasename().toCChar();
    wBase+=".xmllog";
  }//if (pLogfile==nullptr)
  else
    wBase=pLogfile;

  wErrorLog.setErrorLogContext("createZRandomFileFromXml");
  wErrorLog.setAutoPrintOn(true);
  if (!wBase.isEmpty())
    wErrorLog.setOutput(wBase.toCChar());
  else
    wErrorLog.setOutput("createZRandomFileFromXml.log");

  wErrorLog.textLog(
      "__________________________________________________________\n"
      "%s>> starting processing xml file <%s>  \n"
      "__________________________________________________________\n"
      "            Run options are \n"
      "                 Real run or test run <%s>\n"
      "                 File to apply        <%s>\n"
      ,
      _GET_FUNCTION_NAME_,
      pXMLPath,
      pRealRun?"Real Run":"Test Run",
      pContentFilePath==nullptr?"to be found within xml file":(const char*)pContentFilePath);

  if (pRealRun)
    wErrorLog.textLog(" Real Run : file structure is to be modified  \n");
  else
    wErrorLog.textLog("***test*** no update is done  \n");

  wSt=_XMLLoadAndControl(pXMLPath,
                        wXmlDoc,
                        wRootElement,
                        "zicm",
                        __ZRF_XMLVERSION_CONTROL__,
                        &wErrorLog,
                        wOutput);
  if (wSt!=ZS_SUCCESS)
    goto errorcreateZRFFromXml;


  wSt=wRootElement->getChildByName((zxmlNode*&)wFileElement, "file");
  XMLderegister(wRootElement);
  if (wSt!=ZS_SUCCESS)
    {
    wErrorLog.logZStatus(ZAIES_Error,wSt,"Cannot find node <file>. Xml file appears to be malformed.");
    goto errorcreateZRFFromXml;
    }
  if (XMLgetChildInt(wFileElement,"filetype",wIntValue,&wErrorLog,ZAIES_Warning)<0)
  {
    wErrorLog.warningLog("Cannot find <filetype>. Continuing processing ...");
  }
  else
  {
    wErrorLog.textLog("Found file type <%s>",decode_ZFile_type((uint8_t)wIntValue));
  }

  wFCBNode=XMLsearchForNamedChild((zxmlNode*)wFileElement,"zfilecontrolblock");
  XMLderegister(wFileElement);
  if (wFCBNode==nullptr)
  {
    wErrorLog.logZStatus(ZAIES_Error,ZS_XMLERROR, "Cannot find <zfilecontrolblock> within xml file definition. Xml declaration cannot be used.");
    wSt=ZS_XMLERROR;
    goto errorcreateZRFFromXml;
  }
  wFCBElement = (zxmlElement*)wFCBNode;
  wZFCB.clear();

  if (XMLgetChildULong(wFCBElement, "initialsize", wZFCB.InitialSize, &wErrorLog)< 0)
    {
    wErrorLog.errorLog(
        "-E-CNTFINDPAR Cannot find parameter %s.",
        "initialsize");
    }
    else
      wErrorLog.textLog(
          "-Found parameter <%s> value <%ld>.",
          "initialsize",wZFCB.InitialSize);

  if (XMLgetChildULong(wFCBElement, "allocatedblocks", wZFCB.AllocatedBlocks, &wErrorLog)< 0)
    {
      wErrorLog.errorLog(
          "-E-CNTFINDPAR Cannot find parameter %s.",
          "allocatedblocks");
    }
    else
    wErrorLog.textLog(
        "-Found parameter <%s> value <%ld>.",
        "allocatedblocks",wZFCB.AllocatedBlocks);

    if (XMLgetChildULong(wFCBElement, "blockextentquota", wZFCB.BlockExtentQuota, &wErrorLog)< 0)
    {
      wErrorLog.errorLog(
          "-E-CNTFINDPAR Cannot find parameter %s.",
          "blockextentquota");
    }
    else
      wErrorLog.textLog(
          "-Found parameter <%s> value <%ld>.",
          "blockextentquota",wZFCB.BlockExtentQuota);


    if (XMLgetChildULong(wFCBElement, "blocktargetsize", wZFCB.BlockTargetSize, &wErrorLog)< 0)
    {
      wErrorLog.errorLog(
          "-E-CNTFINDPAR Cannot find parameter %s.",
          "blocktargetsize");
    }
    else
      wErrorLog.textLog(
          "-Found parameter <%s> value <%ld>.",
          "blocktargetsize",wZFCB.BlockTargetSize);


    if (XMLgetChildULong(wFCBElement, "blocktargetsize", wZFCB.BlockTargetSize, &wErrorLog)< 0)
    {
      wErrorLog.errorLog(
          "-E-CNTFINDPAR Cannot find parameter %s.",
          "blocktargetsize");
    }
    else
      wErrorLog.textLog(
          "-Found parameter <%s> value <%ld>.",
          "blocktargetsize",wZFCB.BlockTargetSize);

    if (XMLgetChildBool(wFCBElement, "highwatermarking", wBoolValue, &wErrorLog)< 0)
    {
      wErrorLog.errorLog(
          "-E-CNTFINDPAR Cannot find parameter %s.",
          "highwatermarking");
    }
    else
    {
      wZFCB.HighwaterMarking=wBoolValue;
      wErrorLog.textLog(
          "-Found parameter <%s> value <%ld>.",
          "highwatermarking",wZFCB.HighwaterMarking?true:false);
    }
    if (XMLgetChildBool(wFCBElement, "grabfreespace", wBoolValue, &wErrorLog)< 0)
    {
      wErrorLog.errorLog(
          "-E-CNTFINDPAR Cannot find parameter %s.",
          "grabfreespace");
    }
    else
    {
      wZFCB.GrabFreeSpace=wBoolValue;
      wErrorLog.textLog(
          "-Found parameter <%s> value <%ld>.",
          "grabfreespace",wZFCB.GrabFreeSpace?true:false);
    }

  if (wSt==ZS_SUCCESS)
    {
    if (pRealRun)
    {
    wSt=wZRF.zcreate(pContentFilePath,
                      wZFCB.InitialSize,
                      wZFCB.AllocatedBlocks,
                      wZFCB.BlockExtentQuota,
                      wZFCB.BlockTargetSize,
                      wZFCB.HighwaterMarking,
                      wZFCB.GrabFreeSpace,
                      true,
                      false);
    if (wSt!=ZS_SUCCESS)
      {
      wErrorLog.logZStatus(ZAIES_Error,wSt,"Cannot create ZRandomFile <%s>.",pContentFilePath);
      wErrorLog.logZException();
      }
    }//if (pRealRun)
    }//if (wSt==ZS_SUCCESS)

errorcreateZRFFromXml:
  wErrorLog.infoLog("Process ended %s",wErrorLog.hasError()?"with errors":"without errors");


  return wSt;
}

/** @endcond */ // Development



#endif // ZMASTERFILE_UTILITIES_CPP
