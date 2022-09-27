#ifndef ZMASTERFILE_CPP
#define ZMASTERFILE_CPP

#include <zindexedfile/zmasterfile.h>
#include <zindexedfile/zskey.h>

#include <zrandomfile/zrfcollection.h>
#include <zindexedfile/zsjournal.h>
//#include <QUrl>
#include <zindexedfile/zmasterfile_utilities.h>
#include <zindexedfile/zmasterfiletype.h>

#include <zxml/zxmlprimitives.h>

#include <zindexedfile/zsjournalcontrolblock.h>

#include <zindexedfile/zmfdictionary.h>
#include <zindexedfile/zrecord.h>


bool ZMFStatistics = false;

/** @addtogroup ZBSOptions Setting Options for runtime
 * We can set on or off options in order to change runtime behavior of product
@{
*/

/**
 * @brief setStatistics Set the option ZMFStatistics on (true) or off (false)
 * If ZMFStatistics is set then statistics are made and could be reported
 */
void setZMFStatistics (bool pStatistics) {ZMFStatistics=pStatistics;}
/** @} */

using namespace zbs ;

//namespace zbs {

/**
  @addtogroup ZSMasterFileGroup
  @{
  */


//------------------------------------ZSMasterFile-------------------
//
ZMasterFile::ZMasterFile(void) : ZRawMasterFile()
{
  setFileType(ZFT_ZMasterFile);
  ZJCB=nullptr;
return ;
}
/*
ZSMasterFile::ZSMasterFile(uriString pURI) : ZRawMasterFile(pURI)
{
  setFileType(ZFT_ZSMasterFile);
  return;
}
*/

ZMasterFile:: ~ZMasterFile(void)
{
  if (_isOpen)
                        zclose();
     while (IndexTable.size() >0)
             {
             if (IndexTable.last()->_isOpen)
                                            IndexTable.last()->closeIndexFile();
             IndexTable.pop(); // deletes the contained object
             }

      while (IndexTable.size() >0)
             {
             IndexTable.pop();
             }

      setJournalingOff();
}// DTOR

//------------Journaling (see raw master file )--------------------------------


//----------- ZSMasterFile Index methods----------------
//


/**
 * @brief ZSMasterFile::setIndexFilesDirectoryPath  sets a fixed Directory path into which index files will be stored for this master file
 *
 *  General case is that index files are stored in the same given path that the Master File.
 *
 *  But it may be interesting to store index files on a different device / directory from the one containing Master File for performance / security purpose.
 *
 *  Error cases
 *
 *  - File must not be open to set IndexFilesDirectoryPath.
 *  - Mentionned path must be a valid, existing directory.
 *
 * @note operation is not usual as index files could be moved while file is already indexed.
 *
 * @param[in] pPath an uriString containing the path where ZIndexFiles will be located
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::setIndexFilesDirectoryPath (uriString &pPath)
{


ZStatus wSt;
ZDataBuffer wReserved;
    if (isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Error,
                                    " Cannot modify IndexFilesDirectoryPath while file is already open for file  <%s>",
                                    URIContent.toString());
            return  ZS_INVOP;
            }
    if (!pPath.isDirectory())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_NOTDIRECTORY,
                                    Severity_Error,
                                    " Cannot set IndexFilesDirectoryPath : path is not a valid directory. Content file is <%s>",
                                    URIContent.toString());
            return  ZS_NOTDIRECTORY;
            }
    wSt=_Base::_ZRFopen(ZRF_Exclusive|ZRF_All,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While openning for setting IndexFileDirectoryPath. File <%s>",
                               URIContent.toString());
        return  wSt;
        }
    IndexFilePath = pPath;

    _Base::setReservedContent(_exportMCBAppend(wReserved));
//    wSt=_Base::_writeAllHeaders(true);
    wSt=_Base::_writeAllFileHeader();
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot set IndexFilesDirectoryPath / cannot write Reserved header. Content file is <%s>",
                                URIContent.toString());
        }
    _Base::zclose();
    return  wSt;
}//setIndexFilesDirectoryPath

ZStatus
ZMasterFile::setJournalLocalDirectoryPath (uriString &pPath)
{


ZStatus wSt;
ZDataBuffer wReserved;
    if (isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Severe,
                                    " Cannot modify JournalingLocalDirectoryPath while Journaling is started for file  <%s>",
                                    getURIContent().toString());
            return  ZS_INVOP;
            }
    if (!pPath.isDirectory())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_NOTDIRECTORY,
                                    Severity_Error,
                                    " Cannot set JournalingLocalDirectoryPath : path is not a valid directory. Content file is <%s>",
                                    URIContent.toString());
            return  ZS_NOTDIRECTORY;
            }
    wSt=_Base::_ZRFopen(ZRF_Exclusive|ZRF_All,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While openning for setting JournalingLocalDirectoryPath. File <%s>",
                               getURIContent().toString());
        return  wSt;
        }
    ZJCB->JournalLocalDirectoryPath = pPath;

    _Base::setReservedContent(_exportMCBAppend(wReserved));
//    wSt=_Base::_writeAllHeaders(true);
    wSt=_Base::_writeAllFileHeader();
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot set IndexFilesDirectoryPath / cannot write Reserved header. Content file is <%s>",
                                getURIContent().toString());
        }
    _Base::zclose();
    return  wSt;
}//setJournalingLocalDirectoryPath




#ifdef __COMMENT__

//  To add a new index :
//
//       _addIndexField:
//         create all fields definition in key sequence order
//
//       _addIndexKey
//         Create the index block (ZICB)
//
//       _ZIndexFileCreate
//

void
ZSMasterFile::_addIndexField (ZArray<ZSIndexField_struct>& pZIFField,utfdescString& pName, size_t pOffset, size_t pLength)
{
    ZSIndexField_struct wField;
    wField.Name = pName;
    wField.RecordOffset = pOffset;
    wField.NaturalSize = pLength;
    pZIFField.push(wField);
}

/**
 * @brief ZSMasterFile::_addIndexKeyDefinition creates a new ZICB from pZIFFields field list definition and push it to pZICB
 *
 *
 * @param[out] pZICB     ZIndexControlBlock to populate with index definition (Key dictionary)
 * @param[in] pZIFField     Key fields dictionary to add
 * @param[in] pIndexName    User name of the index key as a utfdescString (for description only)
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
 */

void
ZSMasterFile::_addIndexKeyDefinition (ZIndexControlBlock* pZICB,
                                      ZSKeyDictionary& pZKDic,
                                      utf8String& pIndexName,
                                      uint32_t pKeyUniversalSize,
                                      ZSort_Type pDuplicates)
{
    pZICB->clear();
    pZICB->IndexName = pIndexName;
 //   pZICB.AutoRebuild = pAutoRebuild ;
    //pZICB.KeyType = pKeyType;
    pZICB->Duplicates = pDuplicates ;

    pZICB->KeyUniversalSize = pKeyUniversalSize;

    for (long wi = 0; wi<pZKDic.size();wi++)
        {
        pZICB->KeyDic->push(pZKDic[wi]);
        }
        pZICB->KeyDic->_reComputeKeySize();

        return ;
}
#endif // __COMMENT__

ZStatus
ZMasterFile::addKeyToDic(ZKeyDictionary* pKeyDic,long &pOutKeyRank)
{
  pOutKeyRank=-1;
  if (pKeyDic==nullptr)
    return ZS_NULLPTR;
  if ( Dictionary->searchKey(pKeyDic->DicKeyName)!=nullptr)
    return ZS_DUPLICATEKEY;
 return Dictionary->addKey(pKeyDic,pOutKeyRank);
}

/**
 * @brief ZSMasterFile::zcreateIndexWithDefinition Generates a new index from a key definition (meaning a new ZRandomFile data + header).
 *
 * This routine will create a new index with the files structures necessary to hold and manage it : a ZIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 *
 *@note
 * 1. ZIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZIndexFile file pathname is not stored but is a computed data from actual ZSMasterFile file pathname.
 *
 * @param[in] pZIFField     Key fields dictionary to create the index with
 * @param[in] pIndexName    User name of the index key as a utf8String. This name replaces ZSKeyDictionary::DicKeyName.
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
 * @param[in] pBackup   If set to true, then a backup copy of possible existing index files is made
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zcreateIndexWithDefinition (ZKeyDictionary *pKeyDic,  // contains the description of index key to add (multi fields)
                            ZSort_Type pDuplicates,
                            long & pOutKeyRank)
{

ZStatus wSt;
uriString wIndexURI;
uriString wIndexFileDirectoryPath;

  pOutKeyRank=-1;
  if (pKeyDic==nullptr)
    {
      ZException.setMessage (_GET_FUNCTION_NAME_,
          ZS_NULLPTR,
          Severity_Severe,
          " ZSMasterFile <%s>  Key dictionary to add is nullptr.",
          getURIContent().toString());
      return  ZS_NULLPTR;
    }
    if (Dictionary==nullptr)
      {
      ZException.setMessage (_GET_FUNCTION_NAME_,
          ZS_NULLPTR,
          Severity_Severe,
          " ZSMasterFile <%s> - Main dictionary is nullptr  (no main dictionary defined while trying to create index <%s>.)",
          getURIContent().toString(), pKeyDic->DicKeyName.toCChar());
      return  ZS_NULLPTR;
      }

  if (!ZRandomFile::isOpen())
        {
        ZException.setMessage (_GET_FUNCTION_NAME_,
            ZS_ERROPEN,
            Severity_Severe,
            " ZSMasterFile <%s>  must be open (in privileged write mode) to create an index.",
            getURIContent().toString());
        return  ZS_ERROPEN;
        }
  if (!(getMode() & (ZRF_Exclusive | ZRF_All)))
        {
          ZException.setMessage (_GET_FUNCTION_NAME_,
              ZS_ERROPEN,
              Severity_Severe,
              " ZSMasterFile <%s>  must be open in exclusive write mode to create an index.\n"
              " current mode is <%s>",
              getURIContent().toString(),
              decode_ZRFMode(getMode()));
          return  ZS_ERROPEN;
        }

    wSt=Dictionary->addKey(pKeyDic,pOutKeyRank);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    uint32_t wKeyUniversalSize = pKeyDic->computeKeyUniversalSize();
    ZIndexFile *wIndexObject=nullptr;
    wSt=ZMasterFile::zcreateRawIndex(wIndexObject,pKeyDic->DicKeyName,wKeyUniversalSize,pDuplicates,pOutKeyRank,false);
    if (wSt==ZS_SUCCESS)
      {

//      wIndexObject->newKeyDic(pKeyDic,Dictionary);  // no key dictionary within ZIndexControlBlock
      wSt=wIndexObject->writeIndexControlBlock();
      }

    wSt=writeControlBlocks();
    return  wSt;
}//zcreateIndexWithDefinition

ZStatus
ZMasterFile::zcreateIndexFromDictionary (const utf8String &pDicKeyName,
                                          ZSort_Type pDuplicates,
                                          long &pOutKeyRank)
{
  ZStatus wSt;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
  pOutKeyRank=-1;
  if (Dictionary==nullptr)
    {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_NULLPTR,
        Severity_Severe,
        " ZSMasterFile <%s> - Main dictionary is nullptr  (no main dictionary defined while trying to create index <%s>.)",
        getURIContent().toString(), pDicKeyName.toCChar());
    return  ZS_NULLPTR;
    }

  if (!ZRandomFile::isOpen())
  {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        " ZSMasterFile <%s>  must be open (in privileged write mode) to create an index.",
        getURIContent().toString());
    return  ZS_ERROPEN;
  }
  if (!(getMode() & (ZRF_Exclusive | ZRF_All)))
  {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        " ZSMasterFile <%s>  must be open in exclusive write mode to create an index.\n"
        " current mode is <%s>",
        getURIContent().toString(),
        decode_ZRFMode(getMode()));
    return  ZS_ERROPEN;
  }

  /* key name must exist within master dictionary */
  ZKeyDictionary* wKeyDic= Dictionary->searchKeyCase(pDicKeyName);

  if (wKeyDic==nullptr)
    {
    ZException.setMessage ("ZSMasterFile::zcreateIndexFromDic",
                            ZS_NOTFOUND,
                            Severity_Error,
                            " Key <%s>  has not been found in master dictionary.",
                            pDicKeyName.toString());
    return ZS_NOTFOUND;
    }


  /* key name must not have been already created */

  if (IndexTable.searchCaseIndexByName(pDicKeyName)>=0)
    {
    ZException.setMessage ("ZSMasterFile::zcreateIndexFromDic",
        ZS_DUPLICATEKEY,
        Severity_Error,
        " Key <%s>  already exists for this master file.",
        pDicKeyName.toString());
    return ZS_DUPLICATEKEY;
    }



  uint32_t wKeyUniversalSize = wKeyDic->computeKeyUniversalSize();
  ZIndexFile *wIndexObject=nullptr;
  return zcreateRawIndex(wIndexObject,wKeyDic->DicKeyName,wKeyUniversalSize,pDuplicates,pOutKeyRank,false);
}//ZSMasterFile::zcreateIndexFromDictionary

#ifdef __COMMENT__
/**
 * @brief ZSMasterFile::zcreateIndex overload calling zcreateIndexFile primary method
 *
 * This routine will create a new index with the files structures necessary to hold and manage it : a ZIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 * - rebuilds the index from current ZSMasterFile's content
 *
 *@note
 * 1. ZIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZIndexFile file pathname is not stored but is a computed data from actual ZSMasterFile file pathname.
 *
 * @param[in] pZIFField     Key fields dictionary to create the index with
 * @param[in] pIndexName    User name of the index key as a const char *
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zcreateIndex (ZSKeyDictionary& pZKDic,
                           const utf8_t* pIndexName,
                           ZSort_Type pDuplicates,
                           bool pBackup)
{


    utf8String wIndexName;
    wIndexName = pIndexName;
    return   zcreateIndex (pZKDic,
                          wIndexName,
                          pDuplicates,
                          pBackup);
} // zcreateIndex
#endif // __COMMENT__


#include <stdio.h>
/**
 * @brief ZSMasterFile::zremoveIndex Destroys an Index definition and its files' content on storage
 *
 *  Removes an index.
 *
 *      renames all index files of rank greater than the current on to make them comply with ZIndexFile naming rules.
 *
 *
 * @param pIndexRank Index rank to remove from ZSMasterFile
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zremoveIndex (const long pIndexRank)
{
//  utf8String wIndexName=IndexTable[pIndexRank]->IndexName;
  ZStatus wSt=  ZRawMasterFile::zremoveIndex(pIndexRank);
  if (wSt==ZS_SUCCESS)
    {
    delete Dictionary->KeyDic[pIndexRank];
    Dictionary->KeyDic.erase(pIndexRank);
    }
  return wSt;
}//zremoveIndex

//---------------------------------Utilities-----------------------------------------------------

/**  * @addtogroup ZMFUtilities
 * @{ */

/**
 * @brief ZSMasterFile::zclearMCB Sets the current ZSMasterFile's ZMCB to its minimum value. Destroys all existing indexes, and their related files.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.upgradelog
 * @return
 */
ZStatus
ZMasterFile::zclearMCB (FILE* pOutput)
{
ZStatus wSt;
ZDataBuffer wMCBContent;

FILE* wOutput=nullptr;
bool FOutput=false;
utfdescString wBase;


   wOutput=pOutput;
   if (pOutput==nullptr)
       {
//       utfdescString wDInfo;
       wBase=URIContent.getBasename().toCChar();
       wBase+=".downgradelog";
       wOutput=fopen(wBase.toCString_Strait(),"w");
       if (wOutput==nullptr)
                   {
           wOutput=stdout;
           fprintf(wOutput,
                   "%s>>  cannot open file <%s> redirected to stdout\n",
                   _GET_FUNCTION_NAME_,
                   wBase.toString());
                   }
       else
           {
           FOutput=true;
           }
       } // if nullptr

    if ((getOpenMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All))
            {
            fprintf(wOutput,
                    "%s>>  ZSMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                     _GET_FUNCTION_NAME_,
                    getURIContent().toString());

            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_MODEINVALID,
                                     Severity_Error,
                                     " ZSMasterFile <%s> is open in bad mode. Must be (ZRF_Exclusive|ZRF_All) or closed",
                                     URIContent.toString());
            return  ZS_MODEINVALID;
            }


    if (ZVerbose)
        {
        fprintf (wOutput,
                 "%s>>      Clearing ZMasterControlBlock of file <%s>\n"
                 "              Actual content\n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());
        report(pOutput);
        }

    while (IndexTable.size()>0)
            {
            wSt=zremoveIndex(IndexTable.lastIdx());
            if (wSt!=ZS_SUCCESS)
                {
                if (ZVerbose)
                    {
                    fprintf (wOutput,
                             "%s>> ****Error: removing index rank <%ld> status <%s> clearing ZMasterControlBlock of file <%s>\n"
                             "              Actual content\n",
                             _GET_FUNCTION_NAME_,
                             IndexTable.lastIdx(),
                             decode_ZStatus(wSt),
                             URIContent.toString());
                    }
                ZException.addToLast(" Index rank <%ld>. Clearing ZMasterControlBlock of file <%s>.",
                                       IndexTable.lastIdx(),
                                       URIContent.toString());
                return  wSt;
                }// not ZS_SUCCESS
            if (ZVerbose)
                {
                fprintf (pOutput,
                         "%s>>      Index successfully removed\n",
                         _GET_FUNCTION_NAME_);
                report(wOutput);
                }
            }//while

    if (FOutput)
            fclose(wOutput);
    ZMasterControlBlock::clear();
    return   _Base::updateReservedBlock(_exportMCBAppend(wMCBContent),true);
}//zclearMCB

/**
 * @brief ZSMasterFile::zdowngradeZMFtoZRF downgrades a ZSMasterFile structure to a ZRandomFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * Former dependant index files are left to themselves and not destroyed by this method.
 *
 * @param[in] pZMFPath      file path of the ZSMasterFile to convert
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.downgradelog
 */
void
ZMasterFile::zdowngradeZMFtoZRF (const char* pZMFPath,FILE* pOutput)
{


ZStatus wSt;
ZMasterFile wMasterFile;
ZRandomFile wMasterZRF;

uriString   wURIContent;

ZDataBuffer wReservedBlock;

FILE* wOutput=nullptr;
bool FOutput=false;
utfdescString wBase;

   wURIContent = (const utf8_t*)pZMFPath;

   wOutput=pOutput;
   if (pOutput==nullptr)
       {
//       utfdescString wDInfo;
       wBase=wURIContent.getBasename().toCChar();
       wBase+=(const utf8_t*)".downgradelog";
       wOutput=fopen(wBase.toCString_Strait(),"w");
       if (wOutput==nullptr)
                   {
           wOutput=stdout;
           fprintf(wOutput,
                   "%s>>  cannot open file <%s> redirected to stdout\n",
                   _GET_FUNCTION_NAME_,
                   wBase.toString());
                   }
       else
           {
           FOutput=true;
           }
       } // if nullptr

   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   fprintf (wOutput,"%s>> starting downgrading ZSMasterFile to ZRandomFile file path <%s>  \n",
            _GET_FUNCTION_NAME_,
            pZMFPath);


    wSt=wMasterFile.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));

    fprintf (wOutput,"%s>> getting ZReservedBlock content and load ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_);


   fprintf (wOutput,
            "%s>> clearing ZMCB\n"
            "            <%ld>  defined index(es) in ZMasterControlBlock. Destroying all index files & definitions from ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_,
            wMasterFile.IndexTable.size());

    wSt=wMasterFile.zclearMCB(wOutput);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }
    fprintf (wOutput,"%s>>  ZMCB cleared successfully\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();

    fprintf (wOutput,"%s>>  Converting to ZRandomFile\n",
            _GET_FUNCTION_NAME_);
    wSt=wMasterZRF.setPath(wURIContent);
    if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }

   wSt=wMasterZRF._ZRFopen (ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile);  // open ZMF using ZRandomFile routines
   if (wSt!=ZS_SUCCESS)
          {
          ZException.exit_abort();
          }

    wMasterZRF.ZHeader.FileType = ZFT_ZRandomFile;
    wReservedBlock.clear();
    wMasterZRF.setReservedContent(wReservedBlock);

    fprintf (wOutput,"%s>>  Writing new header to file\n",
            _GET_FUNCTION_NAME_);
    wSt=wMasterZRF._writeFullFileHeader(true);
    if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }

   wMasterZRF.zclose();
   fprintf (wOutput,"%s>>  File <%s> has been successfully converted from ZSMasterFile to ZRandomFile\n",
           _GET_FUNCTION_NAME_,
            wURIContent.toString());
   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   ZException.printUserMessage(wOutput);
   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
//   if (wSt==ZS_SUCCESS)
           ZException.clearStack();
   if (FOutput)
          fclose(wOutput);
   return ;
}//zdowngradeZMFtoZRF

/**
 * @brief ZSMasterFile::zupgradeZRFtoZMF upgrades a ZRandomFile structure to an empty ZSMasterFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * @param pZRFPath file path of the ZRandomFile to upgrade
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.upgradelog
 */
void
ZMasterFile::zupgradeZRFtoZMF (const char* pZRFPath,FILE* pOutput)
{


ZStatus wSt;
ZMasterFile wMasterFile;
ZRandomFile wMasterZRF;

uriString   wURIContent;

ZDataBuffer wReservedBlock;

FILE* wOutput=nullptr;
bool FOutput=false;
utfdescString wBase;

   wURIContent = (const utf8_t*)pZRFPath;

   wOutput=pOutput;
   if (pOutput==nullptr)
       {
//       utfdescString wDInfo;
       wBase=wURIContent.getBasename().toCChar();
       wBase+=".upgradelog";
       wOutput=fopen(wBase.toCString_Strait(),"w");
       if (wOutput==nullptr)
                   {
           wOutput=stdout;
           fprintf(wOutput,
                   "%s>>  cannot open file <%s> redirected to stdout\n",
                   _GET_FUNCTION_NAME_,
                   wBase.toString());
                   }
       else
           {
           FOutput=true;
           }
       } // if nullptr

   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   fprintf (wOutput,"%s>> starting upgrading ZRandomFile to ZSMasterFile file path <%s>  \n",
            _GET_FUNCTION_NAME_,
            pZRFPath);


    wSt=wMasterZRF.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }
    fprintf (wOutput,"%s>> creating ZReservedBlock content and write ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_);

    wSt=wMasterFile.setPath(wURIContent);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }
    wMasterFile._exportMCBAppend(wReservedBlock);

    wMasterZRF.setReservedContent(wReservedBlock);
    wMasterZRF.ZHeader.FileType = ZFT_ZMasterFile;
    wSt=wMasterZRF._writeFullFileHeader(true);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }

   wMasterZRF.zclose();
   fprintf (wOutput,"%s>>  File <%s> has been successfully converted from ZRandomFile to ZSMasterFile.\n",
           _GET_FUNCTION_NAME_,
            wURIContent.toString());
   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   ZException.printUserMessage(wOutput);
   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
 //  if (wSt==ZS_SUCCESS)
           ZException.clearStack();
   if (FOutput)
           fclose(wOutput);
   return ;
}//zupgradeZRFtoZMF


/**
 *
 * @brief ZSMasterFile::zrepairIndexes Scans and Repairs indexes of a ZSMasterFile
 *
 *  zrepairIndexes unlock the file in case it has been left open and locked.
 *
 *  For any defined index of the given ZSMasterFile, it tests index file presence and controls ZIndexControlBlock integrity.
 *
 *  It repairs damaged indexes if pRepair option is set to true (and rebuilds them).
 *
 *  It rebuilds all indexes if pRebuildAll is set to true
 *
 *  This routine may be used to test wether a file structure is still correct.
 *
 *  In addition, it may be used to regularly rebuild and reorder indexes if necessary.
 *
 * @warning Be sure to use zrepairIndexes in a standalone context, as it opens the file regardless the possible locks.
 *
 * @param[in] pZMFPath      a const char* with the ZSMasterFile main content file path.
 * @param[in] pRepair       if set this option will try to repair/ rebuilt damaged or missing indexes. If not a report is emitted.
 * @param[in] pRebuildAll   Option if set (true) then all indexes are rebuilt, even if they are healthy indexes.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.repairlog
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zrepairIndexes (const char *pZMFPath,
                             bool pRepair,
                             bool pRebuildAll,
                             FILE* pOutput)
{

ZStatus wSt;
ZMasterFile wMasterFile;
ZRandomFile wMasterZRF;

ZIndexControlBlock* wZICB=nullptr;
ZIndexFile wIndexFile(&wMasterFile);
ZRandomFile wIndexZRF;
uriString   wURIContent;
uriString   wIndexUri;
ZDataBuffer wReservedBlock;
size_t      wImportSize;
zsize_type  wIndexAllocatedSize;
ZDataBuffer wICBContent;

long wi=0, IndexRank=0;

FILE* wOutput=nullptr;
bool FOutput=false;
utfdescString wBase;

ZArray<char> IndexPresence ; // 0 : Index to be deleted     1 : Index present but not to be rebuilt    2 : Index to be built or rebuilt

long wIndexProcessed = 0,wMissIndexFile = 0, wCorruptZICB = 0, wCreatedIndex = 0, wRebuiltIndex = 0 ;

ZSMCBOwnData wMCBOwn;
const unsigned char* wPtrIn=nullptr;


   wURIContent = (const utf8_t*)pZMFPath;

   wOutput=pOutput;
   if (pOutput==nullptr)
       {
//       utfdescString wDInfo;
       wBase=wURIContent.getBasename().toCChar();
       wBase+=".repairlog";
       wOutput=fopen(wBase.toCChar(),"w");
       if (wOutput==nullptr)
                   {
           wOutput=stdout;
           fprintf(wOutput,
                   "%s>>  cannot open file <%s> redirected to stdout\n",
                   _GET_FUNCTION_NAME_,
                   wBase.toString());
                   }
       else
           {
           FOutput=true;
           }
       } // if nullptr

   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");
   fprintf (wOutput,"%s>> starting repairing indexes for ZSMasterFile <%s>  \n"
                    ,
            _GET_FUNCTION_NAME_,
            pZMFPath);


   wSt=wMasterZRF.setPath(wURIContent);
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }
   wSt=wMasterZRF._ZRFopen (ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile,true);  // open ZMF using ZRandomFile routines
   if (wSt!=ZS_SUCCESS)
            goto ErrorRepairIndexes;
   //-------------Must be a ZFT_ZSMasterFile----------------

   if (wMasterZRF.ZHeader.FileType!=ZFT_ZMasterFile)
           {

           fprintf (wOutput,"%s>> **** Fatal error : file is not of mandatory type ZFT_ZSMasterFile but is <%s> ******\n",
                    _GET_FUNCTION_NAME_,
                    decode_ZFile_type( wMasterZRF.ZHeader.FileType));
           goto ErrorRepairIndexes;
           }

   //wMasterFile.ZMFURI = wMasterZRF.getURIContent();     // align uris: ZMFURI is getting redundant. Only using ZRandomFile URIContent

   fprintf (wOutput,"%s>> getting ZReservedBlock content and load ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_);

   wSt=wMasterZRF.getReservedBlock(wMasterZRF.ZReserved,true);     // get reserved block content
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }
  wPtrIn= wMasterZRF.ZReserved.Data;
  wSt=wMasterFile._import(&wMasterFile,wPtrIn);     // load ZMCB from reserved block content
  if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }

   fprintf (wOutput,
            "%s>> existing ZSMasterFile index(es)\n"
            "            <%ld>  defined index(es) in ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_,
            wMasterFile.IndexTable.size());

   wMasterFile.ZMCBreport();

/*
 *  for each index
*/
   IndexPresence.allocateCurrentElements(wMasterFile.IndexTable.size());
   IndexPresence.bzero();
   IndexRank=0;
   for (IndexRank=0;IndexRank<wMasterFile.IndexTable.size();IndexRank++)
   {
       wSt=generateIndexURI(wMasterFile.getURIContent(),
                            wMasterFile.IndexFilePath,
                            wIndexUri,
                            IndexRank,
                            wMasterFile.IndexTable[IndexRank]->IndexName);
       if (wSt!=ZS_SUCCESS)
               {
               return  wSt;// Beware return  is multiple instructions in debug mode
               }
       fprintf (wOutput,
                "%s>>   .....rank <%ld> processing index file <%s> \n",
                _GET_FUNCTION_NAME_,
                IndexRank,
                wIndexUri.toString());
        if (wIndexUri.exists())
                fprintf(wOutput,

                        "%s>>       Index file has been found\n",
                        _GET_FUNCTION_NAME_);
            else
            {
            fprintf(wOutput,
                    "\n%s>>  ****Error Index file <%s> is missing ****\n"
                    "                        Index will be created then rebuilt\n\n",
                    _GET_FUNCTION_NAME_,
                    wIndexUri.toString());

            wMissIndexFile++;

            IndexPresence[IndexRank]= 1;
            if (!pRepair)
                        continue;
            //----------------Create a new ZIndexFile-------------------------


            fprintf (wOutput,
                     "%s>> creating index file\n",
                     _GET_FUNCTION_NAME_);

            wIndexAllocatedSize=0;
            if (wMasterZRF.getBlockTargetSize()>0)
                        if (wMasterZRF.getAllocatedBlocks()>0)
                            wIndexAllocatedSize =  wMasterZRF.getAllocatedBlocks() * wMasterFile.IndexTable[wi]->IndexRecordSize();

            wSt =  wIndexFile.zcreateIndex((ZIndexControlBlock&)*wMasterFile.IndexTable[IndexRank],  // pointer to index control block because ZIndexFile stores pointer to Father's ICB
                                              wIndexUri,
                                              wMasterZRF.getAllocatedBlocks(),
                                              wMasterZRF.getBlockExtentQuota(),
                                              wIndexAllocatedSize,
                                              wMasterZRF.getHighwaterMarking(),
                                              false,        // grabfreespace is set to false : not necessary for an index
                                              true,         // replace existing file
                                              false         // do not leave it open
                                              );
            if (wSt!=ZS_SUCCESS)
                {
                goto ErrorRepairIndexes;
                }
            fprintf (wOutput,
                     "%s>> index file has been created\n",
                     _GET_FUNCTION_NAME_);
            wCreatedIndex++;

            fprintf (wOutput,
                     "%s>> ......rebuilding created index file\n",
                     _GET_FUNCTION_NAME_);

            wSt = wIndexFile.openIndexFile(wIndexUri,IndexRank,ZRF_Exclusive| ZRF_All);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>  ****Error: Unexpected Fatal Error while opening ZIndexFile index rank <%ld> path <%s> ****\n",
                         _GET_FUNCTION_NAME_,
                         IndexRank,
                         wIndexUri.toString());
                 wIndexFile.zclose();
                 goto ErrorRepairIndexes;
                }
            wSt = wIndexFile.zrebuildIndex(ZMFStatistics,wOutput);
            if (wSt!=ZS_SUCCESS)
                    {
                    fprintf (wOutput,
                             "%s>>  ****Unexpected Fatal Error while rebuilding ZIndexFile index rank <%ld> path <%s> *******\n",
                             _GET_FUNCTION_NAME_,
                             IndexRank,
                             wIndexUri.toString());
                    wIndexFile.zclose();
                    goto ErrorRepairIndexes;
                    } // ! ZS_SUCCESS

            wIndexFile.zclose();
            fprintf (wOutput,
                     "%s>>  Index file has been rebuilt successfully\n",
                     _GET_FUNCTION_NAME_);

            IndexPresence[IndexRank] = 1; // Index file is now present and does not need to be rebuilt
            wRebuiltIndex ++;

            //----------------End Create a new ZIndexFile-------------------------
            continue;
            }  // IndexUri does not exist

//---------- wIndexUri exists-----------------
        fprintf (wOutput,"%s>>  Opening ZIndexFile\n",
                 _GET_FUNCTION_NAME_);


        wSt=wIndexZRF.setPath(wIndexUri);
        if (wSt!=ZS_SUCCESS)
                {
                goto ErrorRepairIndexes;
                }
        wSt=wIndexZRF._ZRFopen(ZRF_Exclusive | ZRF_All,ZFT_ZIndexFile,true);
        if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                     "%s>>   ******Error: rank <%ld> cannot open index file <%s>.\n"
                     "                  Status is <%s> Check <IndexFileDirectoryPath> parameter\n"
                      "       ...continuing...\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     decode_ZStatus(wSt),
                     wIndexUri.toString());
                if (pRepair)
                        {
                        fprintf (wOutput,
                        "                  Repare option has been chosen "
                        "                  Trying to delete file and reprocess it as missing file.\n");

                        wIndexZRF._removeFile(); // may be not necessary : to be checked

                        IndexRank--;
                        continue;
                        }
                }

        wReservedBlock.clear();

        fprintf (wOutput,"%s>>  getting ZReservedBlock content and load ZICB\n",
                 _GET_FUNCTION_NAME_);

        wSt=wIndexZRF.getReservedBlock(wReservedBlock,true);     // get reserved block content
        if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>   ******Error: Index file rank <%ld> file  <%s>.\n"
                         "                  Status is <%s> Cannot get ZReservedBlock from header file. \n"
                         "       ...continuing...\n",
                         _GET_FUNCTION_NAME_,

                         IndexRank,
                         wIndexUri.toString(),
                         decode_ZStatus(wSt));

                if (pRepair)
                        {
                        fprintf (wOutput,
                        "                  Repare option has been chosen \n"
                        "                  Trying to delete file and reprocess it as missing file.\n");

                        wIndexZRF._removeFile(); // may be not necessary : to be checked

                        IndexRank--;
                        continue;
                        }
                }

        wPtrIn=wReservedBlock.Data;
        wMCBOwn._import(wPtrIn);

//        wSt=wZICB->_importICB(&wMasterFile.MetaDic,wReservedBlock,wMCBOwn.ICBSize,wMCBOwn.ICBOffset);  // load ZICB from reserved block content

        wZICB=new ZIndexControlBlock;
        wSt=wZICB->_import(wPtrIn);  // load ZICB from reserved block content

        if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>   ******Error: Index file rank <%ld> file  <%s>.\n"
                         "                  Status is <%s> Cannot import ZIndexControlBlock \n"
                         "       ...continuing...\n",
                         _GET_FUNCTION_NAME_,
                         IndexRank,
                         wIndexUri.toString(),
                         decode_ZStatus(wSt));

                if (pRepair)
                        {
                        fprintf (wOutput,
                        "                  Repare option has been chosen \n"
                        "                  Trying to delete file and reprocess it as missing file.\n");

                        wIndexZRF._removeFile(); // may be not necessary : to be checked

                        IndexRank--;
                        continue;
                        }
                }
        fprintf (wOutput,"%s>>  checking ZICB content alignment with ZSMasterFile\n",
                 _GET_FUNCTION_NAME_);

        if (memcmp(wReservedBlock.Data,wMasterFile.IndexTable[IndexRank]->_exportAppend(wICBContent).Data,wReservedBlock.Size)==0)
            {
            fprintf (wOutput,
                     "%s>>  ZICB content is aligned with its ZSMasterFile for index rank <%ld> path <%s>\n"
                     "             To rebuild ZIndexFile content (zrebuid) use Option rebuildAll\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     wIndexUri.toString());

            if (pRebuildAll)
                {
                fprintf (wOutput,
                         "%s>>  Option <RebuildAll> : healthy index file is marked for rebuilt\n",
                         _GET_FUNCTION_NAME_);

                IndexPresence[IndexRank]=2;
                }

            }
            else
            {
            fprintf (wOutput,
                     "%s>>  ****Error: ZICB content is NOT aligned with its ZSMasterFile index rank <%ld> path <%s>\n"
                     "             Need to be realigned and rebuilt rebuildAll\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     wIndexUri.toString());

            wCorruptZICB ++;

            IndexPresence[IndexRank]= 2;  // need to be either realigned (ZICB) and rebuilt OR destroyed . recreated and rebuilt.

            //--------------------------------------------------------------------------

            if(pRepair)
            {
                fprintf (wOutput,
                         "%s>> removing corrupted index file\n",
                         _GET_FUNCTION_NAME_);

                 wIndexZRF._removeFile();
                 fprintf (wOutput,
                          "%s>> reprocessing index as missing index file\n",
                          _GET_FUNCTION_NAME_);
                 IndexRank --;
                 continue;

            }// if pRepare

            //----------------------------------------------------------------------


            }// else


            wIndexZRF.zclose();
   }// main for loop

   wIndexProcessed= IndexRank;
//----------------Post processing check index files to be rebuilt-------------------------

   wMasterZRF.zclose();

   wMasterFile.zopen(ZRF_Exclusive | ZRF_All);

    fprintf (wOutput,"%s>>   Rebuilding indexes to be rebuilt \n",
             _GET_FUNCTION_NAME_);

    for (wi=0;wi<IndexPresence.size();wi ++)
        {
        if (IndexPresence[wi]==2)
                {
                fprintf (wOutput,"%s>>   Rebuilding index rank <%ld> <%s> \n",
                         _GET_FUNCTION_NAME_,
                         wi,
                         wMasterFile.IndexTable[wi]->IndexName.toCChar());

                wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput);
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf (wOutput,
                             "%s>>   ****Error while rebuilding index rank <%ld> <%s> \n"
                             "          Status is <%s>\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             wMasterFile.IndexTable[wi]->IndexName.toCChar(),
                             decode_ZStatus(wSt));
                    goto ErrorRepairIndexes;
                    }
                fprintf (wOutput,"      rebuilt done\n");
                wRebuiltIndex ++;
                }// if IndexPresence == 2
        }// for

EndRepairIndexes:

   fprintf (wOutput,"%s>>  Closing ZSMasterFile\n",
            _GET_FUNCTION_NAME_);
   wMasterZRF.zclose();

   fprintf (wOutput,
            "_____________________________________________________________________________________________\n");

    fprintf (wOutput,
             "%s>>  Report\n"
             "          Index(es) processed         %ld\n"
             "          Index file(s) missing       %ld\n"
             "          Index ZICB corrupted        %ld\n\n"
             "          Index(es) rebuilt           %ld\n",
             _GET_FUNCTION_NAME_,
             wIndexProcessed,
             wMissIndexFile,
             wCorruptZICB,
             wRebuiltIndex);
    fprintf (wOutput,
             "_____________________________________________________________________________________________\n");
    ZException.printUserMessage(wOutput);
    fprintf (wOutput,
             "_____________________________________________________________________________________________\n");
    if (wSt==ZS_SUCCESS)
            ZException.clearStack();
    if (FOutput)
            fclose(wOutput);
    return  wSt;

ErrorRepairIndexes:
    fprintf (wOutput,"%s>>  **** Index repair ended with error ***\n",
             _GET_FUNCTION_NAME_);
    goto EndRepairIndexes;
}//zrepairIndexes

/** @ */ // ZMFUtilities


/**
 * @brief ZSMasterFile::zcreate  ZSMasterFile creation with a full definition with a file path that will name main content file.
 * Other file names will be deduced from this name.
 * @note At this stage, no indexes are created for ZSMasterFile.
 *
 * Main file content and file header are created with appropriate parameters as given in parameters.
 * ZSMasterFile infradata structure is created within header file.
 *
 * @param[in] pURI  uriString containing the path of the future ZSMasterFile main content file.
 *          Other file names will be deduced from this main name.
 * @param[in] pAllocatedBlocks  number of initial elements in ZBAT pool and other pools(pInitialAlloc) see: @ref ZArrayParameters
 * @param[in] pBlockExtentQuota extension quota for pools (pReallocQuota) see: @ref ZArrayParameters
 * @param[in] pBlockTargetSize  approximation of best record size. see: @ref ZRFBlockTargetSize
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pHistory          RFFU History option true : on ; false : off
 * @param[in] pAutocommit       RFFU Autocommit option true : on ; false : off
 * @param[in] pJournaling       RFFU Journaling option true : on ; false : off
 * @param[in] pHighwaterMarking HighWaterMarking option true : on ; false : off see: @ref ZRFHighWaterMarking
 * @param[in] pGrabFreeSpace    GrabFreespace option true : on ; false : off see: @ref ZRFGrabFreeSpace
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZMasterFile::zcreateMasterFile(uriString *pDictionary,
                                const uriString pURI,
                                long pAllocatedBlocks,
                                long pBlockExtentQuota,
                                long pBlockTargetSize,
                                const zsize_type pInitialSize,
                                bool pHighwaterMarking,
                                bool pGrabFreeSpace,
                                bool pJournaling,
                                bool pBackup,
                                bool pLeaveOpen)
{
  ZStatus wSt=ZRawMasterFile::zcreateRawMasterFile( pURI,
                                                    pAllocatedBlocks,
                                                    pBlockExtentQuota,
                                                    pBlockTargetSize,
                                                    pInitialSize,
                                                    pHighwaterMarking,
                                                    pGrabFreeSpace,
                                                    pJournaling,
                                                    pBackup,
                                                    pLeaveOpen);


  ZHeader.FileType = ZFT_ZMasterFile;
  return wSt;
}
/*
void ZMasterFile::setDictionary (const ZMFDictionary& pDictionary)
{
  Dictionary->setDictionary(pDictionary);
  ZHeader.FileType = ZFile_type (ZHeader.FileType | ZFT_ZDicMasterFile) ;
}
*/
ZStatus
ZMasterFile::zcreateMasterFile(uriString *pDictionary, const uriString pURI, const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{
  ZStatus wSt;
  printf ("ZSMasterFile::zcreate \n");
  if (pDictionary==nullptr)
    {
    ZException.setMessage("ZSMasterFile::zcreate",ZS_NULLPTR,Severity_Severe,"Meta dictionary is nullptr while trying to create Master File <%s>.",URIContent.toCChar());
    return (ZS_NULLPTR);
    }
  if (pDictionary->isEmpty())
    {
      ZException.setMessage("ZSMasterFile::zcreate",ZS_EMPTY,Severity_Error,"Meta dictionary is empty while trying to create Master File <%s>.",URIContent.toCChar());
      return (ZS_NULLPTR);
    }


  wSt=ZRandomFile::setPath (pURI);
  if (wSt!=ZS_SUCCESS)
  {return (wSt);}
  ZRandomFile::setCreateMinimum(pInitialSize);
  wSt=ZRandomFile::_create(pInitialSize,ZFT_ZMasterFile,pBackup,true); // calling ZRF creation routine and it leave open
  if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast(" While creating Master file <%s>",
          getURIContent().toString());
      return (wSt);
    }
  ZMasterFile::setDictionary(*pDictionary);
//  Dictionary->generateCheckSum();


  ZHeader.FileType = ZFT_ZMasterFile;     // setting ZFile_type (Already done in _create routine but as ZRandomFiles)
  ZDataBuffer wMCBContent;
  if (pLeaveOpen)
  {
    _exportMCBAppend(wMCBContent);
    ZRandomFile::setReservedContent(wMCBContent);
    //    wSt=_Base::updateReservedBlock(_exportMCB());
    wSt=ZRandomFile::_writeFullFileHeader(true);
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("\nWhile creating Raw Master file %s",
          getURIContent().toString());
      return  wSt;
    }

    return  wSt;
  }
  printf ("ZRawMasterFile::zclose \n");
  zclose(); // updates headers including reserved block
  return  wSt;

}

#ifdef __OLDVERSION__
ZStatus
ZSMasterFile::zcreate(ZMetaDic* pMetaDic,
                    const uriString pURI,
                     long pAllocatedBlocks,
                     long pBlockExtentQuota,
                     long pBlockTargetSize,
                     const zsize_type pInitialSize,
                     bool pHighwaterMarking,
                     bool pGrabFreeSpace,
                     bool pJournaling,
                     bool pBackup,
                     bool pLeaveOpen)
{


ZStatus wSt;
ZDataBuffer wMCBContent;
//    ZMFURI=pURI;
    wSt=_Base::setPath(pURI);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return  wSt;
            }

    _Base::setCreateMaximum (pAllocatedBlocks,
                             pBlockExtentQuota,
                             pBlockTargetSize,
                             pInitialSize,
                             pHighwaterMarking,
                             pGrabFreeSpace);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return  wSt;
            }
    wSt=_Base::_create(pInitialSize,ZFT_ZSMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return  wSt;
            }

    if (Dictionary)
      Dictionary->clear();
    else
      Dictionary=new ZMFDictionary;
    for (long wi=0;wi<pMetaDic->size();wi++)
                Dictionary->push(pMetaDic->Tab[wi]);
    Dictionary->generateCheckSum();

    ZHeader.FileType = ZFT_ZSMasterFile;     // setting ZFile_type

    ZRandomFile::setReservedContent(_exportMCBAppend(wMCBContent));
//    wSt=_Base::updateReservedBlock(_exportMCB());
    wSt=_Base::_writeFullFileHeader(true);
    if (wSt!=ZS_SUCCESS)
      {
      _Base::zclose();
      ZException.addToLast(" While creating Master file %s",
                               getURIContent().toString());
      return  wSt;
      }

// Manage journaling for the being created file
// - set option to MCB
// - if journaling enabled : create journaling file

//    JournalingOn = pJournaling; // update journaling MCB option for the file
    if (pJournaling)
        {
        ZJCB->Journal=new ZSJournal(this);
        wSt=ZJCB->Journal->createFile();
        if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast(" while creating ZSMasterFile %s",
                                       getURIContent().toString());
                return  wSt;
                }
        }

    if (pLeaveOpen)
            { return  wSt;}


    return   zclose();
}// zcreate

/**
 * @brief ZSMasterFile::zcreate Creates the raw content file and its header as a ZRandomFile with a structure capable of creating indexes.
 * @param[in] pURI  uriString containing the path of the future ZSMasterFile main content file. Other file names will be deduced from this main name.
 * @param[in] pInitialSize Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pBackup    If set to true file will be replaced if it already exists. If false (default value), existing file will be renamed according renaming rules.
 * @param[in] pLeaveOpen   If set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zcreate (ZMetaDic* pMetaDic,const uriString pURI, const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{


ZStatus wSt;
ZDataBuffer wMCBContent;
//    ZMFURI=pURI;


    wSt=_Base::setPath (pURI);
    if (wSt!=ZS_SUCCESS)
                {return (wSt);}
    _Base::setCreateMinimum(pInitialSize);
    wSt=_Base::_create(pInitialSize,ZFT_ZSMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return (wSt);
            }
    /* ----------File is left open : so no necessity to open again
    wSt=_Base::_open(ZRF_Exclusive | ZRF_Write,ZFT_ZSMasterFile);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     ZMFURI.toString());
            return  wSt;
            }
*/

    if (Dictionary)
      delete Dictionary;
    Dictionary=new ZMFDictionary;
    for (long wi=0;wi<pMetaDic->size();wi++)
                Dictionary->push(pMetaDic->Tab[wi]);

    Dictionary->generateCheckSum();

    ZHeader.FileType = ZFT_ZSMasterFile;     // setting ZFile_type (Already done in _create routine)
    if (pLeaveOpen)
            {
            _exportMCBAppend(wMCBContent);
            _Base::setReservedContent(wMCBContent);
        //    wSt=_Base::updateReservedBlock(_exportMCB());
            wSt=_Base::_writeFullFileHeader(true);
            if (wSt!=ZS_SUCCESS)
                    {
                    ZException.addToLast(" While creating Master file %s",
                                             getURIContent().toString());
                    return  wSt;
                    }

             return  wSt;
            }

/* only close header & content files : header has been updated */


    return  zclose(); // updates headers including reserved block
}//zcreate

/**
 * @brief ZSMasterFile::zcreate Creates the raw content file and its header as a ZRandomFile with a structure capable of creating indexes.
 *  @note if a file of the same name already exists (either content file or header file)
 *        then content and header file will be renamed to  <base file name>.<extension>_bck<nn>
 *        where <nn> is a version number
 *
 * @param[in] pPathHame  a C string (const char*) containing the path of the future ZSMasterFile main content file. Other file names will be deduced from this main name.
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zcreate (ZMetaDic *pMetaDic,const char* pPathName, const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{
uriString wURI(pPathName);
//    ZMFURI=pPathName;
    return (zcreate(pMetaDic,wURI,pInitialSize,pBackup,pLeaveOpen));
}//zcreate

#endif // __OLDVERSION__
//----------------End zcreate--------------------


/**
 * @brief ZSMasterFile::createZKeyByName Creates a ZKey objects for the index corresponding to given user index key name.
 *
 * @note If index name is not recognized within ZSMasterFile's ZIndexControlBlock, routine aborts.
 *
 * @param[in] pKeyName User given name of the index to create the ZKey for
 * @return a pointer to a ZKey structure. ZKey object is instantiated by 'new' instruction and must be deleted by calling procedure.
 *
 */
ZSKey*
ZMasterFile::createZKeyByName (const char* pKeyName)
{


    long wi=0;
    for (;wi<IndexTable.size();wi++)
                    if (IndexTable[wi]->IndexName == pKeyName)
                                    break;

    if (wi==this->IndexTable.size())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVOP,
                                        Severity_Fatal,
                                        " Invalid key name while creating Key structure. Given name is %s", pKeyName);
                ZException.exit_abort();
                }
    return  (createZKey(wi));
}
/**
 * @brief ZSMasterFile::createZKey Creates a ZKey objects for the index corresponding to given index key rank.
 *
 * @note If index rank is out of ZSMasterFile's ZIndexControlBlock boundaries, routine aborts.
 *
 * @param[in] pKeyNumber index key ZICB rank to create the ZKey for
 * @return a pointer to a ZKey structure. ZKey object is instantiated by 'new' instruction and must be deleted by calling procedure.

 */
ZSKey*
ZMasterFile::createZKey (const long pKeyNumber)
{

    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " File must be open to created a ZKey object from file <%s>",
                                URIContent.isEmpty()?"Unknown":URIContent.toCString_Strait());
        return  nullptr;
        }
    if (pKeyNumber> this->IndexTable.size())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Invalid key number while creating Key structure. given value is %ld", pKeyNumber);
        return  nullptr;
        }
        ZSKey* wKey = new ZSKey(this,pKeyNumber);
    return  (wKey);
}


/**
 * @brief ZSMasterFile::zprintIndexFieldValues prints the whole key description and content for ZIndex pIndex and for its rank pIdxRank
 *
 *  pIdxRank is then the key value relative position within the index (as a ZRandomFile)
 *  pIndex is the index number within ZMCB
 *
 * @see ZIndexFile::zprintKeyFieldsValues()
 *
 * @param[in] pIndex    Number of the index for the ZSMasterFile
 * @param[in] pIdxRank  Logical rank of key record within ZIndexFile
  * @param[in] pHeader  if set to true then key fields description is printed. False means only values are printed.
  * @param[in] pKeyDump if set to true then index key record content is dumped after the list of its fields values. False means only values are printed.
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zprintIndexFieldValues (const zrank_type pIndex,const zrank_type pIdxRank,bool pHeader,bool pKeyDump,FILE *pOutput)
{


    if ((pIndex<0)||(pIndex>IndexTable.lastIdx()))
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_INVOP,
                                            Severity_Error,
                                            " invalid index number %ld. Number of ZIndexes is %ld",
                                            IndexTable.lastIdx());
                    return  ZS_INVOP;
                    }
    return  (static_cast<ZIndexFile*> (IndexTable[pIndex])->zprintKeyFieldsValues(pIdxRank,pHeader,pKeyDump,pOutput));
} //zprintIndexFieldValues


/**
 * @brief ZSMasterFile::zopen opens the ZSMasterFile and all its associated ZIndexFiles with a mode set to defined mask pMode
 * @param pURI  MasterFile file path to open
 * @param pMode Open mode (mask)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */

ZStatus
ZMasterFile::zopen  (const uriString pURI, const int pMode)
{

  if (pURI.isEmpty())
  {
    ZException.setMessage("ZSMasterFile::zopen ",
        ZS_INVNAME,
        Severity_Error,
        "Invalid file name <empty>.");
    return ZS_INVNAME;
  }

  ZStatus wSt;
  ZDataBuffer wRawMCB;
  ZArray<ZPRES> wIndexPresence;

  wSt=ZRandomFile::setPath(pURI);
  wSt=ZRandomFile::_ZRFopen (pMode,ZFT_ZMasterFile); /* opens and get all headers including Reserved header : ZReserved */
  if (wSt!=ZS_SUCCESS)
    return  wSt;

  //    ZMFURI = pURI;

  IndexTable.clear();

  const unsigned char*wPtrIn=ZReserved.Data;
  wSt=ZMasterControlBlock::_import(this,wPtrIn, wIndexPresence);
  if (wSt!=ZS_SUCCESS)
      {return  wSt;}

/* get dictionary */

  if (Dictionary)
    delete Dictionary ;

  Dictionary = new ZDictionaryFile;

  wSt = Dictionary->generateAndSetFileName(URIContent); /* compute dictionary file name from master file content file name */
  if (wSt==ZS_FILENOTEXIST) {
    ZException.setMessage("ZMasterFile::zopen",wSt,Severity_Error,"Dictionary file <%s> has not been found.",
        Dictionary->getURIContent().toCChar());
    return ZS_FILENOTEXIST;
  }
  wSt=Dictionary->zopen(ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("ZMasterFile::zopen",wSt,Severity_Error,"Dictionary file <%s> cannot be openned.",
        Dictionary->getURIContent().toCChar());
    return ZS_FILENOTEXIST;
  }
  wSt=Dictionary->loadActiveDictionary();
  if (wSt!=ZS_SUCCESS) {
    ZException.addToLast(" while openning file <%s>",URIContent.toCChar());
    return wSt;
  }
  // MCB is loaded
  //----------Journaling----------------------

  //    if (JournalingOn)
  if (JCBSize>0)     // if journaling requested
  {
    if (ZJCB->Journal==nullptr) // if no journaling : create one
    {
      ZJCB->Journal=new ZSJournal(this);
      wSt=ZJCB->Journal->init();
      if (wSt!=ZS_SUCCESS)
      {return  wSt;}// Beware return  is multiple instructions in debug mode
      ZJCB->Journal->start();
    }
    else
    {
      if (!ZJCB->Journal->isOpen()) // if journal file is closed then need to re-open and restart
      {
        if (ZJCB->Journal->JThread.isCreated())  // journal file is not open but journaling thread is active
        {
          ZJCB->Journal->JThread.kill();
        }
      }
    }

    const unsigned char* wPtrIn=ZReserved.Data+JCBOffset;

    ZJCB->_import(wPtrIn);
    ZJCB->Journal->init();
    ZJCB->Journal->start();
  } // JournalingOn
  else // no journaling requested
  {
    if (ZJCB!=nullptr)
    {
      if (ZJCB->Journal!=nullptr)
        delete ZJCB->Journal;
      delete ZJCB;
    }
  }// else
  //--------------End journaling----------------------------

  //     Need to create ZIndexFile object instances and open corresponding ZIndexFiles for each IndexTable list rank
  //

  uriString wIndexUri;
  long wi;

  for (wi=0;wi < IndexTable.size();wi++)
  {
    //            ZIndexFile* wIndex = new ZIndexFile (this,(ZSIndexControlBlock&)*IndexTable[wi]);
    //            IndexTable.push(wIndex);
    ZIndexFile* wIndex = IndexTable[wi];
    wIndexUri.clear();

    wSt=generateIndexURI(wIndexUri,getURIContent(),IndexFilePath,wi,IndexTable[wi]->IndexName);
    if (wSt!=ZS_SUCCESS)
    {
      return  wSt;// Beware return  is multiple instructions in debug mode
    }
    if (ZVerbose)
      fprintf(stdout,"Opening Index file <%s>\n",(const char*)wIndexUri.toString());
    wSt=wIndex->openIndexFile(wIndexUri,wi,pMode);
    if (wSt!=ZS_SUCCESS)
      {
        ZException.addToLast("\n while opening index rank <%ld>", wi);
        // In case of open index error : close any already opened index file
        //              Then close master content file before return ing
        ZStatus wSvSt = wSt;
        long wj;
        for (wj = 0;wj < wi;wj++)
          IndexTable[wj]->zclose();  // use the base ZRandomFile zclose routine
        _Base::zclose();

        return  wSt;
      }
  }// for


  return  ZS_SUCCESS;
}// zopen


/**
 * @brief ZSMasterFile::zget gets a record from Master File
 *
 *      Add operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
 *      If an error occur, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
 *      If all indexes are successfully updated, then update on Master File is committed .
 *
 *      if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically removed from index file.
 *
 * @param[in] pRecord   user record content to add in a ZDataBuffer.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zget(ZRecord *pRecord, const zrank_type pZMFRank)
{


ZStatus wSt;
//    wSt=_Base::zaddWithAddress (pRecord,wAddress);      // record must stay locked until successfull commit for indexes

zrank_type      wZMFIdxCommit;
zaddress_type   wZMFAddress;

ZArray <zrank_type>    IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZSIndexItemList        IndexItemList;      // stores keys description per index processed

ZSIndexItem             *wIndexItem;
zrank_type              wIndex_Rank;
long wi = 0;

    if (!pRecord->testCheckSum())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADCHECKSUM,
                              Severity_Severe,
                              "Record dictionary is not in line with ZSMasterFile dictionary : bad checksum");
        return  ZS_BADCHECKSUM;
        }
    wSt=_Base::zget(pRecord->Content,pZMFRank);
    if (wSt==ZS_SUCCESS)
            wSt=pRecord->_split(pRecord->Content);
    return  wSt;
}// ZSMasterFile::zget
/**
 * @brief ZSMasterFile::zgetRaw gets a raw record pRecord at relative file position pZMFRank.
 * If operation is successfull (ZS_SUCCESS), then pRawRecord has been set with following data :
 *  - full raw record content : ZRawRecord::RawContent
 *  - record content : ZRawRecord::Content
 *  - keys raw content : ZRawRecord::KeyValue
 * @param pRecord
 * @param pZMFRank
 * @return
 */
ZStatus
ZMasterFile::_getRaw(ZRawRecord &pRecord, const zrank_type pZMFRank)
{
  ZStatus wSt;
  //    wSt=_Base::zaddWithAddress (pRecord,wAddress);      // record must stay locked until successfull commit for indexes
  wSt=_Base::zget(pRecord.RawContent,pZMFRank);
  if (wSt==ZS_SUCCESS)
    pRecord.getContentFromRaw(pRecord.Content,pRecord.RawContent);

  return  wSt;
}// ZSMasterFile::zgetRaw

//----------------Insert sequence------------------------------------------------------

/**
 * @brief ZSMasterFile::zinsert Inserts a new record given by pRecord at forced position pZMFRank within Master File and updates all indexes

Add operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
If an error occur, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
If all indexes are successfully updated, then update on Master File is committed .

 if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically removed from index file.

* @param[in] pRecord    user record content to insert in a ZDataBuffer
* @param[in] pZMFRank   relative position within the ZSMasterFile to insert the record (@see ZRandomFile
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError


 */
ZStatus
ZMasterFile::zinsert       (ZRecord* pRecord, const zrank_type pZMFRank)
{
ZStatus wSt;

zrank_type      wZMFIdxCommit;
zaddress_type   wZMFAddress;

ZArray <zrank_type>       IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZSIndexItemList      IndexItemList;          // stores keys description per index processed

ZSIndexItem*    wIndexItem;
zrank_type      wIndex_Rank;
long wi = 0;

//    ZExceptionStack.clear();
//    ZException.getLastStatus() = ZS_SUCCESS;
    wZMFIdxCommit=pZMFRank;
    wSt=_Base::_insert2Phases_Prepare(pRecord->Content,wZMFIdxCommit,wZMFAddress);  // prepare the add on Master File, reserve appropriate space, get entry in pool
    if (wSt!=ZS_SUCCESS)
            {
            goto zinsert_error;
            }
//
// update all Indexes
//

    for (wi=0;wi< IndexTable.size();wi++)
    {
        if (ZVerbose)
        {
            fprintf (stdout,"Index number <%ld>\n",wi);
        }
        wIndexItem = new ZSIndexItem;
        wIndexItem->Operation=ZO_Insert;
        wSt=static_cast<ZIndexFile*>(IndexTable[wi])->_keyExtraction(pRecord,wIndexItem->KeyContent);
        if (wSt!=ZS_SUCCESS)
        {
          ZException.addToLast("During zinsert operation on index number <%ld>",wi);
          delete wIndexItem;
          // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
          _add_RollbackIndexes ( IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
          // on error reset ZMF in its original state
          _Base::_add2Phases_Rollback(wZMFIdxCommit); // do not accept update on Master File and free resources
          goto zinsert_error;
        }
        wSt=IndexTable[wi]->_addRawKeyValue_Prepare(wIndexItem,wIndex_Rank, wZMFAddress);// for indexes don't care about insert, this is an add key value
        if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast("During zinsert operation on index number <%ld>",wi);
                delete wIndexItem;
// on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
                _add_RollbackIndexes ( IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
// on error reset ZMF in its original state
                _Base::_add2Phases_Rollback(wZMFIdxCommit); // do not accept update on Master File and free resources
                goto zinsert_error;
                }

     IndexRankProcessed.push(wIndex_Rank) ;     // this index has been added to this rank
     IndexItemList.push(wIndexItem);          // with that key content

//        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);
    }// main for

// so far everything when well
//     commit Indexes changes
//      if an error occurs during index commit :
//          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
// Nb: Exception(s) is(are) pushed on stack
    wSt=_add_CommitIndexes (IndexItemList,IndexRankProcessed) ;
    if (wSt!=ZS_SUCCESS)
        {
        // Soft rollback master update regardless returned ZStatus
        // Nb: Exception is pushed on stack. ZException keeps the last status.
        _Base::_add2Phases_Rollback(wZMFIdxCommit);

        goto zinsert_error;
        }

// at this stage all indexes have been committed
//         commit for Master file data must be done now
//
    wSt = _Base::_insert2Phases_Commit(pRecord->Content,wZMFIdxCommit, wZMFAddress);//! accept insert update on Master File
    if (wSt!=ZS_SUCCESS)    //! and if then an error occur : hard rollback all indexes and signal exception
            {
            _add_HardRollbackIndexes ( IndexRankProcessed); // indexes are already committed so use hardRollback to counter pass
            }                                                    // don't care about the status . In case of error exception stack will trace it
zinsert_return :
    if (getJournalingStatus())
        {
        ZJCB->Journal->enqueue(ZJOP_Insert,pRecord->Content);
        }

zinsert_error:
//    _Base::_unlockFile () ; // set Master file unlocked
    IndexItemList.clear();
    return  wSt;
}// zinsert

//------------End insert sequence----------------------------------------------------

//----------------Add sequence------------------------------------------------------

/**
 * @brief ZSMasterFile::zadd Adds a new record within Master File and updates all indexes
 *
 *      Add operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
 *      If an error occur, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
 *      If all indexes are successfully updated, then update on Master File is committed .
 *
 *      if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically removed from index file.
 *
 * @param[in] pRecord   user record content to add in a ZDataBuffer.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zadd       (ZRecord* pRecord)
{
  ZStatus wSt=ZS_SUCCESS;

    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_FILENOTOPEN,
                              Severity_Severe,
                              " File <%s> is not open while trying to access it",
                              getURIContent().toString());
        return  ZS_FILENOTOPEN;
        }

    if (!pRecord->testCheckSum())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADCHECKSUM,
                              Severity_Severe,
                              "Record dictionary is not in line with ZSMasterFile dictionary : bad checksum");
        return  ZS_BADCHECKSUM;
        }

    wSt=pRecord->_aggregate();  /* serialize the fields from ZRecord memory storage to a ZDataBuffer record */
    if (wSt!=ZS_SUCCESS)
      return wSt;

    pRecord->_extractAllKeys();  /* extract all keys from record content according dictionary */

    return _add(pRecord);


}// zadd


ZStatus
ZMasterFile::_add(ZRecord* pRecord)
{
  ZStatus wSt=ZS_SUCCESS;
  //    wSt=_Base::zaddWithAddress (pRecord,wAddress);      // record must stay locked until successfull commit for indexes

  zrank_type      wZMFZBATIndex, wIdxZBATIndex;
  zaddress_type   wZMFAddress;

  ZArray <zrank_type>    IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
  ZSIndexItemList        IndexItemList;      // stores keys description per index processed

  ZSIndexItem     *wIndexItem;
  zrank_type      wIndex_Rank;
  long wi = 0;

  if (!isOpen())
  {
    ZException.setMessage("ZRawMasterFile::zaddRaw",
        ZS_FILENOTOPEN,
        Severity_Severe,
        " File <%s> is not open while trying to access it",
        getURIContent().toString());
    return  ZS_FILENOTOPEN;
  }

  // prepare the add on Master File, reserve appropriate space, get entry in pool, lock it

  wSt=_Base::_add2Phases_Prepare( pRecord->RawContent,
      wZMFZBATIndex,     // get internal ZBAT pool allocated index
      wZMFAddress);
  if (wSt!=ZS_SUCCESS)
  {
    goto zaddRaw_error;
  }

  // =========== update all defined Indexes ======================

  IndexRankProcessed.clear();

  for (wIndex_Rank=0;wIndex_Rank< IndexTable.size();wIndex_Rank++)
  {
    if (ZVerbose)
    {
      fprintf (stdout,"Index number <%ld>\n",wIndex_Rank);
    }
    wIndexItem=new ZSIndexItem;
    wIndexItem->Operation=ZO_Push;
    wSt=IndexTable[wIndex_Rank]->_keyExtraction(pRecord,wIndexItem->KeyContent);
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During zadd operation on index number <%ld>",wi);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _add_RollbackIndexes (IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2Phases_Rollback(wZMFZBATIndex); // do not accept update on Master File and free resources
      goto zaddRaw_error;
    }

    wSt=IndexTable[wIndex_Rank]->_addKeyValue_Prepare(wIndexItem,wIdxZBATIndex, wZMFAddress);
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During zadd operation on index number <%ld>",wi);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _add_RollbackIndexes ( IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2Phases_Rollback(wZMFZBATIndex); // do not accept update on Master File and free resources
      goto zaddRaw_error;
    }

    IndexRankProcessed.push(wIdxZBATIndex) ;   // this index has been added to this rank
    IndexItemList.push(wIndexItem);            // with that key content

    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);

  }// main for - IndexTableObjects.size()

  // so far everything when well
  //     commit Indexes changes
  //      if an error occurs during index commit :
  //          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
  // Nb: Exception(s) is(are) pushed on stack

  wSt=_add_CommitIndexes (IndexItemList,IndexRankProcessed) ;
  if (wSt!=ZS_SUCCESS)
  {
    // Soft rollback master update regardless returned ZStatus
    // Nb: Exception is pushed on stack. ZException keeps the last status.
    _Base::_add2Phases_Rollback(wZMFZBATIndex);

    goto zaddRaw_error;
  }

  // at this stage all indexes have been committed
  //         commit for Master file data must be done now
  //
  //  wSt = _Base::_add2PhasesCommit_Commit(_Base::pRecord->RawContent,wZMFZBATIndex,wZMFAddress);// accept update on Master File
  wSt = _Base::_add2Phases_Commit(pRecord->RawContent,pRecord->Rank,pRecord->Address);// accept update on Master File

  if (wSt!=ZS_SUCCESS)    // and if then an error occur : hard rollback all indexes
  {
    _add_HardRollbackIndexes ( IndexRankProcessed);// indexes are already committed so use hardRollback regardless returned ZStatus
  }                                                   // don't care about the status . In case of error exception stack will trace it


  if ((wSt==ZS_SUCCESS) && getJournalingStatus())
  {
    ZJCB->Journal->enqueue(ZJOP_Add,pRecord->RawContent);
  }
zaddRaw_error:
  //  _Base::_unlockFile () ; // set Master file unlocked
  IndexItemList.clear();
  //    while (IndexItemList.size()>0)
  //                delete IndexItemList.popR();

  return  wSt;
}// _addRaw




/** @cond Development */

ZStatus
ZMasterFile::_add_CommitIndexes (ZArray <ZSIndexItem*>  &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed)
{
ZStatus wSt;
long wj = 0;
    wj=0;

    for (wj=0;wj<pIndexRankProcessed.size();wj++)
      {
       wSt=IndexTable[wj]->_addRawKeyValue_Commit(pIndexItemList[wj],pIndexRankProcessed[wj]);
       if (wSt!=ZS_SUCCESS)
           {
           ZException.addToLast("While committing add operation on index(es)");
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to IndexTableObjects.size() soft rollback

            for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
              {
              IndexTable[wR]->_addKeyValue_HardRollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                      // regardless ZStatus (exception is on stack)
              } // for

              for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
              {
              IndexTable[wR]->_addRawKeyValue_Rollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                  // regardless ZStatus (exception is on stack)
              } //for

              return  ZException.getLastStatus(); // return  the very last status encountered
          }//wSt
      } // for
   return  ZS_SUCCESS;
} // _add_CommitIndexes

/**
 * @brief ZSMasterFile::_add_RollbackIndexes During an add operation, soft rollback indexes
 *      using a ZArray containing index rank per index processed
 *      Soft rollback means : free memory resources in Pools that have been allocated (ZBAT) and locked with status ZBS_Allocated
 *      Opposed to hard rollback : in this case, operation must be done onto the file itself.
 *
 * @param[in] pZMCB ZMasterControlBlock containing indexes definitions
 * @param[in] pIndexRankProcessed rank of this array gives the Index rank - content gives the Index Rank processed within the Index
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::_add_RollbackIndexes (ZArray<zrank_type> &pIndexRankProcessed)
{
ZStatus wSt;
long wi = 0;
/*    if (IndexTableObjects.size()!=IndexTable.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                        IndexTable.size(),
                                        IndexTableObjects.size());
                ZException.exit_abort();
            }*/

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_add_RollbackIndexes Soft Rollback of indexes on add \n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            IndexTable[wi]->_addRawKeyValue_Rollback(pIndexRankProcessed[wi]); // rollback add on each index concerned
            }//for                                                                  // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();
} // _add_RollbackIndexes

/**
 * @brief ZSMasterFile::_add_HardRollbackIndexes Hard rollback : counter-pass the operation done to restore indexes in their previous state
 * @param pZMCB
 * @param pIndexRankProcessed
 * @return
 */
ZStatus
ZMasterFile::_add_HardRollbackIndexes (ZArray<zrank_type> &pIndexRankProcessed)
{


ZStatus wSt;
long wi = 0;
//    ZException.getLastStatus() = ZS_SUCCESS;
    if (IndexTable.size()!=IndexTable.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                        IndexTable.size(),
                                        IndexTable.size());
                ZException.exit_abort();
            }
    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on add operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            IndexTable[wi]->_addKeyValue_HardRollback(pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            }// for                                                                     // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();

} // _add_HardRollbackIndexes

/** @endcond */
//----------------End Add sequence------------------------------------------------------
/**
 * @brief ZSMasterFile::zreorgFile reorder the base file for ZSMasterFile and rebuild all indexes
 *
 * superseeds ZRandomFile::zreorgFile as an method overload.
 *
 * As ZRandomFile::zreorgFile() changes physical block addresses in reorganizing file structure,
 * it is necessary to rebuild any defined index after having done a zreorgFile process.
 *
 * ZSMasterFile::zreorgFile() does the whole stuff :
 * - base file reorganization
 * - rebuild for all defined indexes for the ZSMasterFile @see ZIndexFile::zrebuildIndex()
 *
 * @param[in] pDump this boolean sets (true) the option of having surfaceScan during the progression of reorganization. Omitted by default (false)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zreorgFile (bool pDump,FILE *pOutput)
{


ZStatus wSt;
long wi = 0;
//ZExceptionMin ZException_sv;
bool wasOpen=false;
bool wgrabFreeSpaceSet = false;

zmode_type wMode = ZRF_Nothing ;
  if (_isOpen)
           {
           wMode=getMode();
           zclose();
           wasOpen=true;
           }



  if ((wSt=zopen(URIContent,ZRF_Exclusive|ZRF_Write))!=ZS_SUCCESS)
           {  return  wSt;}


  if (!ZFCB.GrabFreeSpace)    {    // activate grabFreeSpace if it has been set on
    ZFCB.GrabFreeSpace=true;
    wgrabFreeSpaceSet = true;
  }

  zstartPMSMonitoring();

  wSt = ZRandomFile::_reorgFileInternals(pDump,pOutput);

  while (wi < IndexTable.size()) {
//            wSt=IndexTableObjects[wj]->_addRollBackIndex(pIndexRankProcessed.popR()); // rollback update on each index concerned
//       wSt=IndexTable[wi]->zrebuildRawIndex(false,pOutput); // hard rollback update on each already committed index

    wSt=IndexTable[wi]->zrebuildIndex(false,pOutput); // hard rollback update on each already committed index

    if (wSt!=ZS_SUCCESS)
               {
//               ZException_sv = ZException; // in case of error : store the exception but continue rolling back other indexes
               ZException.addToLast(" during Index rebuild on index <%s> number <%02ld> ",
                                           IndexTable[wi]->IndexName.toCChar(),
                                           wi);
               }

           wi++;
           }

    zendPMSMonitoring ();
    fprintf (pOutput,
             " ----------End of ZSMasterFile reorganization process-------------\n");

    zreportPMSMonitoring(pOutput);

    if (ZException.getLastStatus()!=ZS_SUCCESS)
             {
//             ZException=ZException_sv;
             goto error_zreorgZMFFile;
             }

//    ZException.getLastStatus() = ZS_SUCCESS;

end_zreorgZMFFile:

    if (wgrabFreeSpaceSet)        // restore grabFreeSpace if it was off and has been set on
                 {
                 ZFCB.GrabFreeSpace=false;
                 }
    zclose ();
    if (wasOpen)
            zopen(URIContent,wMode);

    return  ZException.getLastStatus();

error_zreorgZMFFile:
    ZException.printUserMessage(pOutput);
    goto end_zreorgZMFFile;

}// ZSMasterFile::zreorgFile

/**
 * @brief ZSMasterFile::zindexRebuild Rebuilds an index from scratch for a ZSMasterFile
 * @warning ZSMasterFile must be opened in mode ZRF_Exclusive | ZRF_All before calling this method. If not, the operation will be rejected with an error status of ZS_INVOP.
 *          ZException will be setup accordingly.
 *
 * @param pIndexRank    rank of the index for the ZSMasterFile (ZMasterControlBlock)
 * @param pStat         option to request for statistical report (true: yes , false:no)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::zindexRebuild (const long pIndexRank,bool pStat, FILE *pOutput)
{


    if (!isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Severe,
                                    " File <%s> must be opened to use this function",
                                    URIContent.toString());
            return  ZS_INVOP;
            }
    if ((pIndexRank<0)||(pIndexRank>IndexTable.size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Severe,
                                    " Out of indexes boundaries: rank <%ld>  boundaries [0,%ld] : File <%s>",
                                    pIndexRank,
                                    IndexTable.lastIdx(),
                                    URIContent.toString());
            return  ZS_OUTBOUND;
            }
 return  IndexTable[pIndexRank]->zrebuildRawIndex (pStat,pOutput);
}//zindexRebuild


//----------------Remove sequence------------------------------------------------------

ZStatus
ZMasterFile::zremoveByRank    (const zrank_type pZMFRank)
{

ZRecord wZMFRecord(this);

    return _removeByRank (&wZMFRecord,pZMFRank);
} // zremoveByRank

ZStatus
ZMasterFile::zremoveByRankR     (ZRecord *pZMFRecord,const zrank_type pZMFRank)
{
    return _removeByRank (pZMFRecord,pZMFRank);
} // zremoveByRankR

/**
 * @brief ZSMasterFile::_removeByRank Removes a record corresponding to logical position pZMFRank within Master File and updates all indexes
 *
 * Remove operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
 *
 * @note  If an error occurs during processing, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
 * If all indexes are successfully updated, then update on Master File is committed .
 * if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically recreated within index file.
 *
 * @param[in,out] pDescriptor ZSMasterFile descriptor. It is updated during remove operation
 * @param[in] pZMCB ZMasterControlBlock of the file
 * @param[out] pZMFRecord the Record that is removed
 * @param[in] pZMFRank the logical position (rank) of the record to remove
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::_removeByRank  (ZRecord *pZMFRecord,
                             const zrank_type pZMFRank)
{
ZStatus     wSt;

zrank_type              wZMFIdxCommit;
zaddress_type           wZMFAddress;
ZArray <zrank_type>     IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZSIndexItemList         IndexItemList;      // stores keys description per index processed (rollback purpose). Must stay as pointer (double free)
ZSIndexItem             *wIndexItem;
ZDataBuffer             wKeyContent;

ZBlock          wBlock;

//---- get Master File header test for lock - if OK lock it

    wSt=_Base::_remove_Prepare(pZMFRank,wZMFAddress);  // prepare remove on Master File mark entry in pool as to be deleted
    if (wSt!=ZS_SUCCESS)
            {
            _Base::_remove_Rollback(pZMFRank);
            return  wSt;
            }
    wZMFIdxCommit = pZMFRank;
    wSt=_Base::_readBlockAt(wBlock,wZMFAddress);  // we know that it is locked as ZBS_BeingDeleted
    if (wSt!=ZS_SUCCESS)
            {
            _Base::_remove_Rollback(pZMFRank);
            return  wSt;
            }

    pZMFRecord->Content=wBlock.Content;
//    pZMFRecord.setData(wBlock.Content.Data,wBlock.Content.Size);
    wBlock.Content.clear();
//
// update all Indexes
//
    IndexRankProcessed.clear();
    IndexItemList.clear();
    zrank_type wIndex_Rank;
    long wi;

    // must extract keys value per key to remove each index key
    pZMFRecord->_extractAllKeys();

    for (wi=0;wi< pZMFRecord->KeyValue.count();wi++)
      {
        wSt=IndexTable[wi]->_removeIndexItem_Prepare(*pZMFRecord->KeyValue[wi],
                                                            wIndex_Rank);
        if (wSt!=ZS_SUCCESS)
            {
//                delete wIndexItem; // free memory for errored key value
            // on error reset all already processed indexes in their original state
            // (IndexRankProcessed heap contains the Index ranks added)
            // An additional error during index rollback will be put on exception stack

            _remove_RollbackIndexes (IndexRankProcessed); // do not care about ZStatus : exception will be on stack

            // on error reset ZMF in its original state
            _Base::_remove_Rollback(wZMFIdxCommit); // do not accept update on Master File and free resources

//                if (HistoryOn)
//                      ZMFHistory.push (ZO_Add,wi,wAddress,wSt); //! journalize Error on index if journaling is enabled

           // _Base::_unlockFile (ZDescriptor) ; // unlock done in remove rollback

            goto _removeByRank_return ; ;   // and send appropriate status
            }
         IndexRankProcessed.push(wIndex_Rank) ; // this index has been added to this rank
         IndexItemList.push(wIndexItem);        // with that key content
    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);
        }// main for

// so far everything when well
//     commit changes

    wSt=_remove_CommitIndexes (IndexItemList,IndexRankProcessed) ;
    if (wSt!=ZS_SUCCESS)
                {
                // in case of error : appropriate indexes soft or hard rollback is made in Commit Indexes routine

                _Base::_remove_Rollback(wZMFIdxCommit); // an error occurred on indexes : rollback all MasterFile
               // _Base::_unlockFile (ZDescriptor) ; // unlock is done in rollback routine
                goto _removeByRank_return ;// if an error occurs here : this is a severe error that only may be caused by an HW or IO subsystem failure.
                }

    wSt = _Base::_remove_Commit(wZMFIdxCommit);// accept update on Master File
    if (wSt!=ZS_SUCCESS)    // and if then an error occur : rollback all indexes and signal exception
            {
            _remove_HardRollbackIndexes ( IndexItemList,IndexRankProcessed);    // indexes are already committed so use hardRollback to counter pass
            goto _removeByRank_return ;
            }

    if (getJournalingStatus())
      {
      ZJCB->Journal->enqueue(ZJOP_RemoveByRank,pZMFRecord->Content,pZMFRank,wZMFAddress);
      }

_removeByRank_return :
//    _Base::_unlockFile () ;
/*
    while (IndexItemList.size()>0)
                  delete IndexItemList.popR();
*/
    IndexItemList.clear();
    return  wSt;
}// _removeByRank

/**
 * @brief ZSMasterFile::_removeByRank Removes a record corresponding to logical position pZMFRank within Master File and updates all indexes
 *
 * Remove operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
 *
 * @note  If an error occurs during processing, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
 * If all indexes are successfully updated, then update on Master File is committed .
 * if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically recreated within index file.
 * ZSMasterFile internal data structures (descriptor, MCB, ICBs) are updated during remove operation.
 *
 * @param[out] pZMFRecord the Raw Record that is being removed
 * @param[in] pZMFRank the logical position (rank) of the record to remove
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZMasterFile::_removeByRankRaw  (ZRawRecord *pZMFRecord,
                                 const zrank_type pZMFRank)
{
  ZStatus     wSt;

  zrank_type              wZMFZBATIndex;
  zaddress_type           wZMFAddress;
  ZArray <zrank_type>     IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
  ZSIndexItemList         IndexItemList;      // stores keys description per index processed (rollback purpose). Must stay as pointer (double free)
  ZSIndexItem             *wIndexItem;
  ZDataBuffer             wKeyContent;

  ZBlock          wBlock;

  //---- get Master File header test for lock - if OK lock it

  wSt=_Base::_remove_Prepare(pZMFRank,wZMFAddress);  // prepare remove on Master File mark entry in pool as to be deleted
  if (wSt!=ZS_SUCCESS)
    {
    _Base::_remove_Rollback(pZMFRank);
    return  wSt;
    }

  wSt=_Base::_readBlockAt(wBlock,wZMFAddress);  // we know that it is locked as ZBS_BeingDeleted
  if (wSt!=ZS_SUCCESS)
  {
    _Base::_remove_Rollback(pZMFRank);
    return  wSt;
  }

  pZMFRecord->RawContent=wBlock.Content;
  //    pZMFRecord.setData(wBlock.Content.Data,wBlock.Content.Size);
  wBlock.Content.clear();
  //
  // update all Indexes
  //
  IndexRankProcessed.clear();
  IndexItemList.clear();
  zrank_type wIndex_Rank;
  long wi;

  // must extract keys value per key to remove each index key one by one
  pZMFRecord->getContentFromRaw(pZMFRecord->Content,pZMFRecord->RawContent);

  for (wi=0;wi< pZMFRecord->KeyValue.count();wi++)
  {
    wSt=IndexTable[wi]->_removeIndexItem_Prepare(*pZMFRecord->KeyValue[wi],
        wIndex_Rank);
    if (wSt!=ZS_SUCCESS)
    {
      //                delete wIndexItem; // free memory for errored key value
      // on error reset all already processed indexes in their original state
      // (IndexRankProcessed heap contains the Index ranks added)
      // An additional error during index rollback will be put on exception stack

      _remove_RollbackIndexes (IndexRankProcessed); // do not care about ZStatus : exception will be on stack

      // on error reset ZMF in its original state
      _Base::_remove_Rollback(pZMFRank); // do not accept update on Master File and free resources

      //                if (HistoryOn)
      //                      ZMFHistory.push (ZO_Add,wi,wAddress,wSt); //! journalize Error on index if journaling is enabled

      // _Base::_unlockFile (ZDescriptor) ; // unlock done in remove rollback

      goto _removeByRank_return ; ;   // and send appropriate status
    }
    IndexRankProcessed.push(wIndex_Rank) ; // this index has been added to this rank
    IndexItemList.push(wIndexItem);        // with that key content
    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);
  }// main for

  // so far everything when well
  //     commit changes

  wSt=_remove_CommitIndexes (IndexItemList,IndexRankProcessed) ;
  if (wSt!=ZS_SUCCESS)
  {
    // in case of error : appropriate indexes soft or hard rollback is made in Commit Indexes routine

    _Base::_remove_Rollback(pZMFRank); // an error occurred on indexes : rollback all MasterFile
    // _Base::_unlockFile (ZDescriptor) ; // unlock is done in rollback routine
    goto _removeByRank_return ;// if an error occurs here : this is a severe error that only may be caused by an HW or IO subsystem failure.
  }

  wSt = _Base::_remove_Commit(pZMFRank);// accept update on Master File
  if (wSt!=ZS_SUCCESS)    // and if then an error occur : rollback all indexes and signal exception
  {
    _remove_HardRollbackIndexes (IndexItemList,IndexRankProcessed);    // indexes are already committed so use hardRollback to counter pass
    goto _removeByRank_return ;
  }

  if ((wSt==ZS_SUCCESS)&&getJournalingStatus())
  {
    ZJCB->Journal->enqueue(ZJOP_RemoveByRank,pZMFRecord->RawContent,pZMFRank,wZMFAddress);
  }

_removeByRank_return :
//  _Base::_unlockFile () ;
  /*
    while (IndexItemList.size()>0)
                  delete IndexItemList.popR();
*/
  IndexItemList.clear();
  return  wSt;
}// _removeByRankRaw



/**
 * @brief ZSMasterFile::_remove_CommitIndexes For All ZIndexFile associated to current ZSMasterFile commits the remove operation
 *
 *
 * @param[in] pZMCB             ZMasterControlBlock owning indexes definitions
 * @param[in] pIndexItemList        List of pointers to ZIndexItem to be removed (Index Key contents)- Necessary in case of Hard Rollback.
 * @param[in] pIndexRankProcessed   List of ranks for ZIndexFile to be removed
 * @return  a ZStatus value. ZException is set appropriately with error message content in case of error.
 */
ZStatus
ZMasterFile::_remove_CommitIndexes (ZSIndexItemList & pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed)
{


ZStatus wSt;
long wj = 0;

//    ZException.getLastStatus() =ZS_SUCCESS;
    for (wj=0;wj<pIndexRankProcessed.size();wj++)
            {
            wSt=IndexTable[wj]->_removeKeyValue_Commit(pIndexRankProcessed[wj]);
            if (wSt!=ZS_SUCCESS)
                {
//  hard roll back already processed indexes
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to IndexTableObjects.size() soft rollback

                for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
                {
                IndexTable[wR]->_removeKeyValue_HardRollback(pIndexItemList[wR] ,pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                        // regardless ZStatus (exception is on stack)
                } // for

                for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
                {
                IndexTable[wR]->_removeKeyValue_Rollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                    // regardless ZStatus (exception is on stack)
                }// for

            return  ZException.getLastStatus(); // return  the very last status encountered
            }//wSt
    } // Main for

   return  ZException.getLastStatus();
} // _remove_CommitIndexes


ZStatus
ZMasterFile::_remove_RollbackIndexes (ZArray<zrank_type> &pIndexRankProcessed)
{


ZStatus wSt;
long wi = 0;
        if (IndexTable.size()!=IndexTable.size())
                {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_CORRUPTED,
                                            Severity_Fatal,
                                            " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                            IndexTable.size(),
                                            IndexTable.size());
                    ZException.exit_abort();
                }

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_remove_RollbackIndexes Soft Rollback of indexes on remove\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            IndexTable[wi]->_removeKeyValue_Rollback(pIndexRankProcessed[wi]);  // rollback remove on each index concerned
            }// for                                                                     // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();
} // _remove_RollbackIndexes


ZStatus
ZMasterFile::_remove_HardRollbackIndexes (ZArray<ZSIndexItem*> &pIndexItemList,
                                           ZArray<zrank_type> &pIndexRankProcessed)
{


long wi = 0;

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (IndexTable.size()!=IndexTable.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZIndexFile objects number <%s>",
                                        IndexTable.size(),
                                        IndexTable.size());
                ZException.exit_abort();
            }
    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on remove operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            IndexTable[wi]->_removeKeyValue_HardRollback(pIndexItemList[wi],pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            } // for                                                    // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();
} // _remove_HardRollbackIndexes

//----------------End Remove sequence------------------------------------------------------

#ifdef __COMMENT__
//----------------Search sequence-----------------------------------------------------------
//

ZStatus
ZSMasterFile::zgenerateKeyValueList ( ZDataBuffer& pKey ,long pKeyNumber,int pCount,...)
{

va_list args;
va_start (args, pCount);
ZArray<void*> wKeyValues;

    for (int wi=0;wi<pCount;wi++)
    {
        wKeyValues.push(va_arg(args,void*));
    }
 va_end(args);
 return  (zgenerateKeyValue(IndexTable[pKeyNumber],wKeyValues,pKey));
} // zgenerateKeyValue


/**
 * @brief ZSMasterFile::_generateKeyValue creates a ZDataBuffer with key values from a ZArray containing natural individual values
 * @param[in] pICB
 * @param[in] pKeyValues a ZArray with each element represents a key field value in key dictionary rank order
 * @param[out] pKey a ZDataBuffer with the key content in natural format
 * @return
 */
ZStatus
ZSMasterFile::zgenerateKeyValue ( ZSIndexControlBlock& pICB,ZArray<void*> &pKeyValues,ZDataBuffer& pKey)
{

ssize_t wKeyOffset = 0;

// As there might be partial key search, we do not test wether all key fields are processed but only on maximum number of requested fields
//
    if (pKeyValues.size()>pICB.ZKDic->size())
                  {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_INVOP,
                                            Severity_Fatal,
                                            " Number of requested fields for key is %ld while number of key fields is %ld",
                                            pKeyValues.size(),
                                            pICB.ZKDic->size());
                    return  ZS_INVOP;
                   }

    pKey.clearData();
    pKey.allocate(pICB.IndexRecordSize()+1);
    memset (pKey.Data,0,pICB.IndexRecordSize());

    for (long wi=0;wi<pKeyValues.size();wi++)
        {
        memmove (pKey.Data+wKeyOffset,pKeyValues[wi],pICB.ZKDic->Tab[wi].NaturalSize);

        wKeyOffset += pICB.ZKDic->Tab[wi].NaturalSize;
        } // for

    return   ZS_SUCCESS;
}//_generateKeyValue
#endif // __COMMENT__
/**
 * @brief ZSMasterFile::zsearch searches for a single key value using a ZKey Object
 *
 * if key has duplicates, the first found value according search algorithm is given
 *
 * @param pRecord
 * @param pKey
 * @return
 */
ZStatus
ZMasterFile::zsearch(ZDataBuffer &pRecord,ZSKey *pKey)
{

    return (zsearch(pRecord,(ZDataBuffer &)*pKey,pKey->IndexNumber));
}
/**
 * @brief ZSMasterFile::zsearch searches for a single key value using a ZDataBuffer containing key value to search
 * Key to search for has to be previously formatted to index internal format using appropriate routines
 * @copydetail ZIndexFile::_search()
 *
 * @param[out] pRecord
 * @param[in] pKeyValue
 * @param[in] pIndexNumber
 * @return
 */
ZStatus
ZMasterFile::zsearch (ZDataBuffer &pRecord,ZDataBuffer &pKeyValue,const long pIndexNumber)
{
ZStatus wSt;
//zaddress_type wAddress;
//long wIndexRank;
ZSIndexResult wZIR;
    wSt = IndexTable[pIndexNumber]->_Rawsearch(pKeyValue,*IndexTable[pIndexNumber],wZIR);
    if (wSt!=ZS_FOUND)
            { return  wSt;}
    return  zgetByAddress(pRecord,wZIR.ZMFAddress);
}

class ZSIndexCollection;

/**
 * @brief ZSMasterFile::zsearchAll search for all index rank using a ZDataBuffer containing key value to search.
 * Exact or Partial key search is defined by a ZMatchSize_type (pZMS) that defines wether search is exact or partial.
 *
 * Exact key value : comparizon is done on key content and key length. Both must be equal.
 * Partial key value : given key must have a length less or equal to key length
 *
 * @param[in] pKeyValue a ZDataBuffer containing the key content to search for. Key content must have been previously formatted to index internal format.
 * @param[in] pIndexNumber ZSMasterFile index number to search
 * @param[out] pIndexCollection returned collection of index references
 * @return
 */
ZStatus
ZMasterFile::zsearchAll (ZDataBuffer &pKeyValue,
                          const long pIndexNumber,
                          ZSIndexCollection& pIndexCollection,
                          const ZMatchSize_type pZMS)
{



    return  IndexTable[pIndexNumber]->_searchAll(pKeyValue,
                                                      *IndexTable[pIndexNumber],
                                                      pIndexCollection,
                                                      pZMS);

}
/**
 * @brief ZSMasterFile::zsearchAll search for all index rank using a ZKey object. Exact or Partial key search is set by ZKey
 *
 * Exact key value : comparizon is done on key content and key length. Both must be equal.
 * Partial key value : given key must have a length less or equal to key length
 *
 * @param[in] pZKey a ZKey containing the key content to search with all necessary index references
 * @param[out] pIndexCollection returned collection of index references
 * @return
 */
ZStatus
ZMasterFile::zsearchAll (ZSKey &pZKey,ZSIndexCollection& pIndexCollection)
{


ZStatus wSt;

    pIndexCollection.reset();
    ZMatchSize_type wZSC = ZMS_MatchIndexSize;

    pIndexCollection.ZIFFile = IndexTable[pZKey.IndexNumber];// assign ZIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    if (pZKey.FPartialKey)
            wZSC=ZMS_MatchKeySize ;
    wSt = IndexTable[pZKey.IndexNumber]->_searchAll(pZKey,*IndexTable[pZKey.IndexNumber],pIndexCollection,wZSC);
    return  wSt;
}//zsearchAll

/**
 * @brief ZSMasterFile::zsearchFirst searches for first key value (in index key order) of pKey (see @ref ZKey) and return s its corresponding ZSMasterFile record content.
 *
 * a Collection and its context ( see @ref ZSIndexCollection and @ref ZSIndexCollectionContext )
 * is created by 'new' instruction and needs to be mentionned when further calls to ZSMasterFile::zsearchNext() are made.
 *
 * enriched during further zsearchNext accesses - and may be obtained to use Collection facitilies. see @ref Collection.
 *
 * Collection context is created and maintained within collection. This meta data describes the context of current search.
 *
 *  zsearchFirst uses ZIndexFile::_searchFirst().
 *
 * @copydoc ZIndexFile::_searchFirst()
 *
 * @param[in] pZKey     Key object. contains key content value to find and ZSMasterFile index number.
 * @param[out] pRecord  Found record content if any match found.
 * @param[out] pCollection A pointer to the contextual meta-data from the search created by _searchFirst() routine.
 *
 * It contains
 * - a ZArray of ZSIndexResult objects, enriched with successive _searchNext() calls
 * - Contextual meta-data for the search (an ZSIndexCollectionContext embeded object holds this data)
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on
 *
 * @param[in] pLock  lock mask to apply to found record.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 */
ZStatus
ZMasterFile::zsearchFirst (ZSKey &pZKey,
                           ZDataBuffer& pRecord,
                           ZSIndexCollection *pCollection)
{


ZStatus wSt;
ZSIndexResult wZIR;

    ZMatchSize_type wZMS = ZMS_MatchIndexSize;


    if (pZKey.FPartialKey)
            wZMS=ZMS_MatchKeySize ;
    wSt = IndexTable[pZKey.IndexNumber]->_searchFirst(pZKey,
                                                             *IndexTable[pZKey.IndexNumber],
                                                             pCollection,
                                                             wZIR,
                                                             wZMS);
    if (wSt!=ZS_FOUND)
               { return  wSt;}// Beware return  is multiple instructions in debug mode

    wSt=zgetByAddress(pRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}// Beware return  is multiple instructions in debug mode
    return  ZS_FOUND;
}// zsearchFirst
/**
 * @brief ZSMasterFile::zsearchNext
 * @copydoc ZIndexFile::_searchNext()
 *
 * @param [in] pZKey  Key object. Contains key content value to find and ZSMasterFile index number.
 * @param [in-out] pCollection A pointer to the contextual meta-data from the search created by _searchFirst() routine.
 *
 * It contains
 * - a ZArray of ZSIndexResult objects, enriched with successive _searchNext calls
 * - Contextual meta-data for the search.
 *
 * As it is created by 'new' instruction, it has to be deleted by caller later on.
 * @param [out] pRecord Record content as a result of the _searchNext operation, if any.
 *
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 * - ZS_FOUND in case of success (And NOT ZS_SUCCESS)
 * - ZS_OUTBOUNDLOW if key value in lower than the lowest (first) index value
 * - ZS_OUTBOUNDHIGH if key value is higher than the highest (last) index value
 * - ZS_NOTFOUND if key value is not found in the middle of index values set
 */
ZStatus
ZMasterFile::zsearchNext (ZSKey &pZKey,
                          ZDataBuffer & pRecord,
                          ZSIndexCollection *pCollection)
{


ZStatus wSt;
ZSIndexResult wZIR;

    if (pCollection==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                "Invalid collection/context : it must have been created using zsearchFirst");
        return  ZS_INVOP;
        }

    //pIndexCollection.ZIFFile = IndexTableObjects[pIndexNumber];// assign ZIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    wSt = IndexTable[pZKey.IndexNumber]->_searchNext(wZIR,pCollection);
    if (wSt!=ZS_FOUND)
                {return  wSt;}// Beware return  is multiple instructions in debug mode

    wSt=zgetByAddress(pRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}// Beware return  is multiple instructions in debug mode
    return  ZS_FOUND;
}// zsearchFirst





//----------------End Search sequence----------------------------------------------------


/**
 * @brief ZSMasterFile::getKeyIndexFields return s natural field values from a ZIndex key at rank pIndexRank for ZIndex pIndexNumber (key number)
 * @note given rank is relative position of record within ZIndexFile file and NOT relative position of record within ZSMasterFile file
 * @param[out] pKeyFieldValues   return s natural field values concatenated within a ZDataBuffer
 * @param[in] pIndexRank        Index key row
 * @param[in] pIndexNumber      ZIndex number (key number) - in definition order
 * @return a ZStatus
 */
ZStatus
ZMasterFile::getKeyIndexFields(ZDataBuffer &pKeyFieldValues, const zrank_type pIndexRank, const zrank_type pIndexNumber)
{


 ZStatus wSt;
 ZDataBuffer wIndexRawContent;
 ZSIndexItem  wIndexContent;
    wSt=IndexTable[pIndexNumber]->_Base::zget(wIndexRawContent,pIndexRank);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}
    wIndexContent.fromFileKey(wIndexRawContent);
    return  static_cast<ZIndexFile*> (IndexTable[pIndexNumber])->getKeyIndexFields(pKeyFieldValues,wIndexContent.KeyContent);
}

/**
 * @brief getRawIndex   Gets a raw index item (without Endian conversion) corresponding to is IndexNumber and its rank
 * @param pIndexItem    A ZIndexItem with returned index content
 * @param pIndexRank    Rank for index
 * @param pIndexNumber  Index number
 * @return
 */
ZStatus
ZMasterFile::getRawIndex(ZSIndexItem &pIndexItem,const zrank_type pIndexRank,const zrank_type pIndexNumber)
{


    ZStatus wSt;
    ZDataBuffer wIndexContent;
    wSt=IndexTable[pIndexNumber]->_Base::zget(wIndexContent,pIndexRank);
    if (wSt!=ZS_SUCCESS)
                   { return  wSt;}// Beware return  is multiple instructions in debug mode
    pIndexItem.fromFileKey(wIndexContent);
    return  wSt;
}

/** @} */ // ZSMasterFileGroup

//----------------------Reports---------------------------------------
/**
 * @brief ZSMasterFile::ZMCBreport Reports the whole content of ZMasterControlBlock : indexes definitions and dictionaries
 */
void ZMasterFile::ZMCBreport(void)
{
    report();
    return;
}
#ifdef __COMMENT__
//----------------------XML Reports and stuff-----------------------------------
/** addtogroup XMLGroup
 * @{ */

/** @cond Development
 *
 * @brief ZSMasterFile::_writeXML_FileHeader Generates the xml definition for current opened ZSMasterFile's ZFileDescriptor (header) plus ZReserved : ZMasterControlBlock
 * @param pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZSMasterFile::_writeXML_ZRandomFileHeader(ZSMasterFile& pZMF,FILE *pOutput)
{

   ZRandomFile::_writeXML_ZRandomFileHeader(pZMF.pOutput);

   fprintf (pOutput,

            "  <zmastercontrolblock>\n"
            "         <historyon>%s</historyon>\n"
            "         <journalingon>%s</journalingon>\n"
            "         <indexfiledirectorypath>%s</indexfiledirectorypath>\n"
            ,

            pZMF.HistoryOn?"true":"false",
            pZMF.getJournalingStatus()?"true":"false",
            pZMF.IndexFilePath.toString()
            );

   fprintf (pOutput,
            "         <!-- Changing IndexRank position in a file description has no impact -->\n"
            "         <index>\n");
   for (long wi=0;wi<pZMF.IndexTable.size();wi++)
   {
   _writeXML_Index(pZMF,wi,pOutput);
   }//for wi
   fprintf (pOutput,
            "         </index>\n");
   fprintf (pOutput,
            "  </zmastercontrolblock>\n");
   return ;
}//_writeXML_FileHeader
void
ZSMasterFile::_writeXML_Index(ZSMasterFile& pZMF,const long pIndexRank,FILE* pOutput)
{

  fprintf (pOutput,
      "         <indexcontent>\n"
      "           <rank>%ld</rank> <!-- not significant -->\n"
      "           <indexname>%s</indexname> <!-- warning modification of this field implies index duplication see documentation -->\n"
      "           <duplicates>%s</duplicates> <!-- warning modification of this field must be cautiously done see documentation -->\n"
      "           <keyuniversalsize>%d</keyuniversalsize> <!-- warning this field cannot be modified without a major impact on index -->\n"
      //            "           <autorebuild>%s</autorebuild>\n"
      ,
      pIndexRank,
      pZMF.IndexTable[pIndexRank]->IndexName.toCChar(),
      pZMF.IndexTable[pIndexRank]->Duplicates==ZST_DUPLICATES?"duplicates":"unique",
      pZMF.IndexTable[pIndexRank]->KeyUniversalSize
      //             pZMF.IndexTable[pIndexRank]->AutoRebuild?"true":"false"
      );

  _writeXML_KDic(pZMF.IndexTable[pIndexRank]->ZKDic,pOutput);

  fprintf (pOutput,
      "         </indexcontent>\n");

  return  ;
}//_writeXML_Index




void
ZSMasterFile::_writeXML_KDic( ZSKeyDictionary* ZKDic, FILE* pOutput)
{

  fprintf (pOutput,
      "           <keydictionary>\n"
        );

  if (ZKDic!=nullptr) // if key dictionary exists
  {
    for (long wd=0;wd<ZKDic->size();wd++) // dictionary detail
    {
      fprintf (pOutput,
          "              <keyfield>\n"
          "                <rank>%ld</rank>  <!-- not modifiable : only field position in ZKDic is taken -->\n"
          "                <fieldname>%s</fieldname>\n"
          "                <naturalsize>%ld</naturalsize>\n"
          "                <universalsize>%ld</universalsize>\n"
          "                <capacity>%u</capacity>\n"
          "                <ztype>Field%s</ztype>   <!-- see ZType_type definition : beware the typos -->\n"
          "                <keyoffset>%d</keyoffset>\n"
          "                <hash>%s</hash>\n"
          "              </keyfield>\n"
          ,
          wd,
          ZKDic->MetaDic->Tab[ZKDic->Tab[wd].MDicRank].getName().toCChar(),
          ZKDic->MetaDic->Tab[ZKDic->Tab[wd].MDicRank].NaturalSize,
          ZKDic->MetaDic->Tab[ZKDic->Tab[wd].MDicRank].UniversalSize,
          ZKDic->MetaDic->Tab[ZKDic->Tab[wd].MDicRank].Capacity,
          decode_ZType(ZKDic->MetaDic->Tab[ZKDic->Tab[wd].MDicRank].ZType),
          ZKDic->Tab[wd].KeyOffset,
          ZKDic->Tab[wd].Hash.toHexa()
          );
    }// for
  }//if (ZKDic!=nullptr)
  fprintf (pOutput,
      "          </keydictionary>\n");
  return ;

}//_writeXML_KDic




/** @endcond */ // Development

/**
 * @brief ZSMasterFile::zwriteXML_IndexDefinition Generates the xml definition for current opened ZSMasterFile's Index of rank pIndexRank
 * @param[in]   Index rank to generate the XML definition for
 * @param[out] pOutput defaulted to nullptr. Could be stdout or stderr .
 *             If nullptr, then an xml file is generated named <directory path><base name>-<indexname><pIndexRank>.xml
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::generateXML_IndexDefinition(const long pIndexRank, const char* pFilename )
{
  utf8String wReturn;
  wReturn.clearData();
//FILE* wOutput=pOutput;
utf8String OutPath;
    if ((pIndexRank<0)||(pIndexRank>IndexTable.lastIdx()))
       {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_INVOP,
                               Severity_Severe,
                               "Invalid index number <%ld> requested while index range is [0,%ld]",
                               pIndexRank,
                               IndexTable.lastIdx());
       return  (ZS_INVOP);
       }
    if (pFilename==nullptr)
    {
        uriString   uriOutput;
        utf8String  OutBase;
        uriOutput = URIContent.toString();
        OutBase = uriOutput.getBasename().toCChar();
        OutPath = uriOutput.getDirectoryPath().toCChar();
        OutPath += OutBase;
        OutPath += "-";
        OutPath += IndexTable[pIndexRank]->IndexName.toCChar();
        OutPath.addsprintf("%02ld",pIndexRank);
        OutPath += ".xml";
        wOutput = fopen(OutPath.toCChar(),"w");
        if (wOutput==nullptr)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_ERROPEN,
                                 Severity_Severe,
                                 " cannot open file %s for output",
                                 OutBase.toCChar());
                ZException.exit_abort();
                }
    }


    fprintf (wzextractXML_AllIndexesOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");

    _writeXML_Index(*this,pIndexRank,wOutput);

    fprintf (wOutput,
             "</zicm>");
    if (pOutput==nullptr)
                fclose(wOutput);

    fprintf (stderr,"Xml index definition file <%s> has been created.\n",
                     OutPath.toCChar());
    return  ZS_SUCCESS;
}//zwriteXML_IndexDefinition

/**
 * @brief ZSMasterFile::zwriteXML_IndexDictionary generates the xml definition of a Key dictionary for the mentionned index rank for current ZMF
 * @param[in]   Index rank to generate the XML definition for
 * @param[out] pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>-<indexname><pIndexRank>-kdic.xml
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::generateXML_IndexDefinition(const long pIndexRank, FILE *pOutput)
{
FILE* wOutput=pOutput;
utfdescString OutPath;
    if ((pIndexRank<0)||(pIndexRank>IndexTable.lastIdx()))
       {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_INVOP,
                               Severity_Severe,
                               "Invalid index number <%ld> requested while index range is [0,%ld]",
                               pIndexRank,
                               IndexTable.lastIdx());
       return  (ZS_INVOP);
       }
    if (pOutput==nullptr)
    {
        uriString uriOutput;
        utfdescString OutBase;
        uriOutput= URIContent.toString();
        OutBase = uriOutput.getBasename().toCChar();
        OutPath = uriOutput.getDirectoryPath().toCChar();
        OutPath += OutBase;
        OutPath += "-";
        OutPath += IndexTable[pIndexRank]->IndexName.toCChar();
        OutPath.addsprintf("%02ld",pIndexRank);
        OutPath.add((const utf8_t*)"-kdic");
        OutPath += (const utf8_t*)".xml";
        wOutput = fopen(OutPath.toCString_Strait(),"w");
        if (wOutput==nullptr)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_ERROPEN,
                                 Severity_Severe,
                                 " cannot open file %s for output",
                                 OutBase.toString());
                ZException.exit_abort();
                }
    }
    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZMF_XMLVERSION__ ">\n");

//$$$ _writeXML_KDic(IndexTable[pIndexRank].ZKDic,wOutput);

    fprintf (wOutput,
             "</zicm>");
    if (pOutput==nullptr)
                fclose(wOutput);

    fprintf (stderr,"Xml key dictionary definition file <%s> has been created.\n",
                     OutPath.toString());
    return  ZS_SUCCESS;
}//zwriteXML_IndexDictionary


/**
 * @brief ZSMasterFile::zwriteXML_FileHeader Generates the xml definition for the whole current opened ZSMasterFile's header
 *  This is a non static routine
 *
 *  This routine generates an xml file containing :
 *      - the
 *
 * @param pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZSMasterFile::XmlWriteFileDefinition(FILE *pOutput)
{


FILE* wOutput=pOutput;
utfdescString OutPath;
    if (pOutput==nullptr)
    {
        uriString uriOutput;
        utfdescString OutBase;
        uriOutput= URIContent.toString();
        OutBase = uriOutput.getBasename().toCChar();
        OutPath = uriOutput.getDirectoryPath().toCChar();
        OutPath += OutBase;
        OutPath += ".xml";
        wOutput = fopen(OutPath.toCString_Strait(),"w");
        if (wOutput==nullptr)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_ERROPEN,
                                 Severity_Severe,
                                 " cannot open file %s for output",
                                 OutBase.toString());
                ZException.exit_abort();
                }
    }
    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZMF_XMLVERSION__ ">\n");
    fprintf (wOutput,
             "<File>\n");
    _writeXML_ZRandomFileHeader(*this,wOutput);
    fprintf (wOutput,
             "</File>\n"
             "</zicm>");

    if (pOutput==nullptr)
                fclose(wOutput);


    fprintf (stderr,"Xml header definition file <%s> has been created.\n",
                     OutPath.toString());
    return ;
}//zwriteXML_FileHeader
/**
 * @brief ZSMasterFile::zwriteXML_FileHeader Static function : Generates the xml definition for a ZSMasterFile's header given by it path name pFilePath
 * @note the ZSMasterFile is opened for read only ZRF_Read_Only then closed.
 *
 * @param[in] pFilePath points to a valid file to generate the definition from
 * @param[in] pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZSMasterFile::XmlWriteFileDefinition(const char* pFilePath,FILE *pOutput)
{


ZSMasterFile wZMF;
ZStatus wSt;
FILE* wOutput=pOutput;
utfdescString OutPath;
    wSt = wZMF.zopen(pFilePath,ZRF_Read_Only);
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();

    if (pOutput==nullptr)
    {
        uriString uriOutput;
        utfdescString OutBase;
        uriOutput= (const utf8_t*)pFilePath;
        OutBase = uriOutput.getBasename().toCChar();
        OutPath = uriOutput.getDirectoryPath().toCChar();
        OutPath += OutBase;
        OutPath += (const utf8_t*)".xml";
        wOutput = fopen(OutPath.toCString_Strait(),"w");
        if (wOutput==nullptr)
                {
                ZException.getErrno(errno,
                                 _GET_FUNCTION_NAME_,
                                 ZS_ERROPEN,
                                 Severity_Severe,
                                 " cannot open file %s for output",
                                 OutBase.toString());
                ZException.exit_abort();
                }
    }

    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZMF_XMLVERSION__ ">\n");

    fprintf (wOutput,
             "<File>\n");
    wZMF._writeXML_ZRandomFileHeader(wZMF,wOutput);
    fprintf (wOutput,
             "</File>\n");
    fprintf (wOutput,
             "</zicm>");
    wZMF.zclose();
    if (pOutput==nullptr)
                fclose(wOutput);
    fprintf (stderr,"Xml definition file <%s> has been created.\n",
                     OutPath.toString());
    return ;
}  // static zwriteXML_FileHeader


/*----------------------------------------------------------------------------------------------
 *  Warning : never use QDomNode::cloneNode (true) . It generates SIGEV after combined 2 usages
 * ---------------------------------------------------------------------------------------------
*/
/** @cond Development */


/** @endcond */ // Development


/**
 * @brief ZSMasterFile::zextractXML_MetaDic parses and explore a masterfile xml definition pFilePath
 *   and extracts master file record dictionary definition (ZMetaDic) regardless where it is located
 *   returns a pointer to ZMetaDic with loaded data. This created object needs to be later on deleted.
 *
 * @param[in] pFilePath xml file to load
 * @param[out] pMetaDic the ZMetaDic loaded with record dictionary data contained in xml file
 * @return
 */
int
ZSMasterFile::zextractXML_MetaDic(const char *pFilePath,
                                  zbs::ZMetaDic* &pMetaDic,
                                  ZaiErrors* pErrorlog)
{

  ZaiErrors ErrorLog;
  ZStatus wSt;
  //QString wN;
  zxmlDoc* wXmlDoc=nullptr;
  zxmlNode* wTopIndexNode=nullptr;
  zxmlNode* wIndexRank=nullptr;
  zxmlNode* wIndexRank1=nullptr;
  zxmlElement* wIndexFirstElement=nullptr;


  ErrorLog.setErrorLogContext("zextractXML_MetaDic");
  ErrorLog.setAutoPrintOn(true);

  wSt=_XMLzicmControl(pFilePath,wXmlDoc,wIndexFirstElement,pErrorlog);
  if (wSt!=ZS_SUCCESS)
    {
    return  wSt;
    }

  /* find xml subset with <index> tag */
  wTopIndexNode = XMLsearchForChildTag((zxmlNode*)wIndexFirstElement,"metadic");
  if (wTopIndexNode==nullptr)
  {
    ErrorLog.errorLog("zloadXML_AllIndexes-E-NOTFND <metadic> tag has not be found within xml file <%s>.",pFilePath);
    XMLderegister(wIndexFirstElement);
    return ZS_NOTFOUND;
  }

  int wRet=0;

  if (pMetaDic==nullptr)
    pMetaDic = new ZMetaDic;

  pMetaDic->clear();

  wRet=pMetaDic->fromXml(wTopIndexNode,pErrorlog);

  XMLderegister(wIndexFirstElement);
  XMLderegister(wTopIndexNode);

  return  wRet;
}//zextractXML_MetaDic

/**
 * @brief ZSMasterFile::zloadXML_Index parses and explore a masterfile xml definition pFilePath
 *   and extracts all indexes definitions regardless where they are located
 *   returns an array of pointers to ZSIndexControlBlock with loaded data.
 *   These pointers need to be deleted later on.
 *
 * @param[in] pFilePath xml file to load
 * @param[out] pZICB the ZIndexControlBlock populated with data contained in xml file
 * @return
 */
int
ZSMasterFile::zextractXML_AllIndexes(const char *pFilePath,
                                  zbs::ZArray<ZSIndexControlBlock *> &pZICBList,
                                  ZMetaDic* pMetaDic,
                                  ZaiErrors* pErrorlog)
{

ZaiErrors ErrorLog;
ZStatus wSt;
//QString wN;
zxmlDoc* wXmlDoc=nullptr;
zxmlNode* wTopIndexNode=nullptr;
zxmlNode* wIndexRank=nullptr;
zxmlNode* wIndexRank1=nullptr;
zxmlElement* wIndexFirstElement=nullptr;
ZSIndexControlBlock* wZICB=nullptr;

  pZICBList.clear();

  ErrorLog.setErrorLogContext("zloadXML_Index");
  ErrorLog.setAutoPrintOn(true);

    wSt=_XMLzicmControl(pFilePath,wXmlDoc,wIndexFirstElement,pErrorlog);
    if (wSt!=ZS_SUCCESS)
                { return  wSt;}

    /* find xml subset with <index> tag */
    wTopIndexNode = XMLsearchForChildTag((zxmlNode*)wIndexFirstElement,"index");
    if (wTopIndexNode==nullptr)
    {
      ErrorLog.errorLog("zloadXML_AllIndexes-E-NOTFND <index> tag has not be found within xml file <%s>.",pFilePath);
      return ZS_NOTFOUND;
    }

    /* scan all indexes <indexrank> tag within xml subset */
    wSt=wTopIndexNode->getFirstChild(wIndexRank);
    if (wSt!=ZS_SUCCESS)
    {
      ErrorLog.errorLog("zloadXML_AllIndexes-E-CORRUPT  Cannot find <indexrank> tag xml file <%s>.",pFilePath);
      return ZS_CORRUPTED;
    }
    int wRet=0;
    while (wSt==ZS_SUCCESS)
    {
      if (wIndexRank->getName()=="indexrank")
      {
        wRet=_loadXML_OneIndex(wTopIndexNode,wZICB,pMetaDic,pErrorlog);
        if (wRet==0)
          pZICBList.push(wZICB);
      }
      wSt=wIndexRank1->getNextNode(wIndexRank);
      XMLderegister(wIndexRank);
      wIndexRank=wIndexRank1;
    }
    return  ZS_SUCCESS;

}//zloadXML_Index

//----------------------Dictionary and fields load---------------------------

/** @endcond */ // Development

/** @} */ // group XMLGroup

#endif // __COMMENT__




ZRecord*
ZMasterFile::generateRecord()
{
  return new ZRecord (this);
}



//-------------------Statistical functions-----------------------------------------

/**
@addtogroup ZMFSTATS ZSMasterFile and ZIndexFile storage statistics and PMS session monitoring

@{
*/

/**
 * @brief ZSMasterFile::ZRFPMSIndexStats gives storage statistical data for an Index given by its rank
 *
 *
 * @param pIndex    Index rank
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZMasterFile::ZRFPMSIndexStats(const long pIndex,FILE* pOutput)
{


    if ((pIndex<0)||(pIndex>IndexTable.size()))
    {
             fprintf(pOutput, " Performance Data report error for file <%s> : invalid index number <%ld>\n",
                     URIContent.toString(),
                     pIndex);
             return ;
    }
    fprintf (pOutput,
             "\nPerformance report on index number <%ld> name <%s> for file <%s>\n"
             "         Index Content File <%s>\n"
             "                 session summary\n"
             "________________________________________________\n"
             "   Index Header file <%s>\n"
             "________________________________________________\n",
             pIndex,

             IndexTable[pIndex]->IndexName.toCChar(),
             URIHeader.toString(),
             IndexTable[pIndex]->getURIContent().toString(),
             IndexTable[pIndex]->getURIHeader().toString()
             );


    IndexTable[pIndex]->ZPMS.reportDetails(pOutput);
    return ;

}// ZRFPMSIndexStats

/**
* @brief ZSMasterFile::zstartPMSMonitoring Starts PMS monitoring session for current ZSMasterFile
* @note collected data concerns only ZSMasterFile and NOT indexes
*   @ref  ZSMasterFile::zstartIndexPMSMonitoring
*   @ref  ZSMasterFile::zendIndexPMSMonitoring
*   @ref  ZSMasterFile::zreportIndexPMSMonitoring
*/
 void
 ZMasterFile::zstartPMSMonitoring (void)
 {
     ZPMSStats.init();
 }
/**
* @brief ZSMasterFile::zendIndexPMSMonitoring Ends current PMS monitoring session on Index given by its rank (pIndex)
* @note collected data concerns only ZSMasterFile and NOT indexes
*   @ref  ZSMasterFile::zstartIndexPMSMonitoring
*   @ref  ZSMasterFile::zendIndexPMSMonitoring
*   @ref  ZSMasterFile::zreportIndexPMSMonitoring
*/
  void
  ZMasterFile::zendPMSMonitoring (void)
  {
      ZPMSStats.end();
  }
/**
* @brief ZSMasterFile::zreportPMSMonitoring Fully reports current PMS monitoring session on current ZSMasterFile
* This report lists collected data :
*   - timer data
*   - ZRandomFile PMS data
* @note collected data concerns only ZSMasterFile and NOT indexes
*   @ref  ZSMasterFile::zstartIndexPMSMonitoring
*   @ref  ZSMasterFile::zendIndexPMSMonitoring
*   @ref  ZSMasterFile::zreportIndexPMSMonitoring
*
* @param pIndex rank of the index to monitor
* @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
*/
   void
   ZMasterFile::zreportPMSMonitoring (FILE* pOutput)
   {
       fprintf(pOutput,
               "________________________________________________\n"
               "   ZSMasterFile <%s>\n"
               "   File open mode         %s\n"
               "________________________________________________\n",
               URIContent.toString(),
               decode_ZRFMode( Mode)
               );
       ZPMSStats.reportFull(pOutput);
       return;
   }



/**
  * @brief ZSMasterFile::zstartIndexPMSMonitoring Starts PMS monitoring session on the specific index given by its rank (pIndex)
  * @param pIndex rank of the index to monitor
  */
 ZStatus
 ZMasterFile::zstartIndexPMSMonitoring (const long pIndex)
 {

     if ((pIndex<0)||(pIndex>IndexTable.lastIdx()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                "Invalid index number <%ld> requested while index range is [0,%ld]",
                                pIndex,
                                IndexTable.lastIdx());
        return  (ZS_INVOP);
        }
     IndexTable[pIndex]->ZPMSStats.init();
     return  ZS_SUCCESS;
 }//zstartIndexPMSMonitoring
 /**
   * @brief ZSMasterFile::zendIndexPMSMonitoring Ends current PMS monitoring session on the specific index given by its rank (pIndex)
   * @param pIndex rank of the index to monitor
   */
ZStatus
ZMasterFile::zstopIndexPMSMonitoring(const long pIndex)
{

  if ((pIndex<0)||(pIndex>IndexTable.lastIdx()))
     {
     ZException.setMessage(_GET_FUNCTION_NAME_,
                             ZS_INVOP,
                             Severity_Severe,
                             "Invalid index number <%ld> requested while index range is [0,%ld]",
                             pIndex,
                             IndexTable.lastIdx());
     return  (ZS_INVOP);
     }
  IndexTable[pIndex]->ZPMSStats.end();
  return  ZS_SUCCESS;
}//zstopIndexPMSMonitoring

/**
* @brief ZSMasterFile::zreportIndexPMSMonitoring Fully reports current PMS monitoring session on the specific index given by its rank (pIndex)
* @param pIndex rank of the index to monitor
* @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
*/
ZStatus
ZMasterFile::zreportIndexPMSMonitoring (const long pIndex, FILE* pOutput)
{

   if ((pIndex<0)||(pIndex>IndexTable.lastIdx()))
      {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Invalid index number <%ld> requested while index range is [0,%ld]",
                              pIndex,
                              IndexTable.lastIdx());
      return  (ZS_INVOP);
      }
   fprintf(pOutput,
           "________________________________________________\n"
           "   ZSMasterFile <%s>\n"
           "   Index rank <%ld> <%s>\n"
           "   File open mode    %s\n",
           URIContent.toString(),
           pIndex,
           IndexTable[pIndex]->IndexName.toCChar(),
           decode_ZRFMode( Mode));
   IndexTable[pIndex]->ZPMSStats.reportFull(pOutput);
   return  ZS_SUCCESS;
}// zreportIndexPMSMonitoring

/** @}*/ // ZMFSTATS
//----------------End Stats---------------------------------------------------



//------------Generic Functions-----------------------------------------------------------
//








#endif // ZSMASTERFILE_CPP
