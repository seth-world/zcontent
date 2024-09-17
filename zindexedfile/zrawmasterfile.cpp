#ifndef ZRAWMASTERFILE_CPP
#define ZRAWMASTERFILE_CPP

#include "zrawmasterfile.h"

#include <stdio.h>
#include <unistd.h>

#include <zio/zioutils.h>

//#include <zindexedfile/zkey.h>
#include <zindexedfile/zindexitem.h>

#include <zrandomfile/zrfcollection.h>

#include <zindexedfile/zjournal.h>
#include <zindexedfile/zsjournalcontrolblock.h>


#include <zindexedfile/zmasterfile_utilities.h>

#include <zxml/zxmlprimitives.h>

#include <zindexedfile/zrawrecord.h>

#include <zxml/zxmlprimitives.h>

#include <zindexedfile/zmfdictionary.h>  // for addDictionary()

#include <zindexedfile/zrawmasterfileutils.h>

#include <zindexedfile/bckelement.h>

#include <zcontentcommon/urfparser.h>
#include <zxml/zxmlerror.h>

#include <zcontent/zrandomfile/zrfutilities.h>

using namespace  zbs;

ZRawMasterFile::ZRawMasterFile(ZFile_type pType) : ZRandomFile(pType),ZMasterControlBlock(this)
{
//  RawRecord=new ZRawRecord(this);
  ZJCB=nullptr;
  // MetaDic = nullptr;
  Dictionary = nullptr;

  setFileType(pType);
  return ;
}

ZRawMasterFile::ZRawMasterFile() : ZRandomFile(ZFT_ZRawMasterFile),ZMasterControlBlock(this)
{
//  RawRecord=new ZRawRecord(this);
  ZJCB=nullptr;
  Dictionary = nullptr;
  return ;
}

/*
ZRawMasterFile::ZRawMasterFile(uriString pURI) : ZRandomFile(pURI)
{
    ZStatus wSt=setPath(pURI);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
    ZJCB=nullptr;
    Dictionary = nullptr;
    setFileType(ZFT_ZRawMasterFile);
    return;
}

*/
ZRawMasterFile:: ~ZRawMasterFile(void)
{
    if (_isOpen)
                        zclose();
     while (IndexTable.size() >0)
             {
             if (IndexTable.last()->_isOpen)
                                            IndexTable.last()->closeIndexFile();
             IndexTable.pop(); // deletes the contained object
             }


      setJournalingOff();

/*
  if (RawRecord!=nullptr)
    delete RawRecord;
*/
}// DTOR

//------------Journaling--------------------------------

ZStatus
ZRawMasterFile::initJournaling (uriString &pJournalPath)
{
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
// this does not touch integrity of files

    if (!_isOpen)
            {

                wasOpen=false;
                wSt = zopen(ZRF_Exclusive|ZRF_Write);
                if (wSt!=ZS_SUCCESS)
                            {return  wSt;}// Beware return  is multiple instructions in debug mode
            }
        else
        {
        if (Mode!=(ZRF_Exclusive|ZRF_Write))
            {
            wFormerMode = Mode;
            zclose();
            zopen(ZRF_Exclusive|ZRF_Write);
            }

        }// else


    if (BaseParameters->VerboseFileEngine())
            fprintf (stderr,
                     "setJournalingOn-I Starting journaling for file %s\n",
                     getURIContent().toString());
    if (ZJCB==nullptr)
            {
            ZJCB = new ZJournalControlBlock;
            ZJCB->Journal = new ZSJournal(this);
            }
        else
    {
    if (ZJCB!=nullptr)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot start journaling for file %s : already started.\n",
                     getURIContent().toString());
            if (ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZJCB->Journal->getURIContent().toString());
                        return  ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Journaling for file %s is set to started while journal file is marked as not open.",
                                    getURIContent().toString());
            ZException.exit_abort();
//            return  ZS_FILENOTOPEN;
            }
        } // else
    wSt=ZJCB->Journal->init();
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While initializing journaling");
        return  wSt;
        }

//    JournalingOn=true;
    ZJCB->JournalLocalDirectoryPath = pJournalPath;
    wSt=writeControlBlocks();

    if (wSt!=ZS_SUCCESS)
                {
                    return  wSt; // Beware return  is multiple instructions in debug mode
                }
    if (BaseParameters->VerboseFileEngine())
            _DBGPRINT("setJournalingOn-I-JCB Journaling set on within JournalingControlBlock\n")

    if (!wasOpen)  // if was not open at routine entrance, then it has been openned locally and must be closed
                {return  zclose();}

    if (wFormerMode!=ZRF_Nothing) // if was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open with former mode
            }
    if (BaseParameters->VerboseFileEngine())
            _DBGPRINT("setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZJCB->Journal->getURIContent().toString())
    wSt=ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (BaseParameters->VerboseFileEngine())
            _DBGPRINT(
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZJCB->Journal->getURIContent().toString())
    }
    else
        {
        ZException.addToLast("While starting journaling thread for journaling file %s",
                             ZJCB->Journal->getURIContent().toString());
        ZException.printUserMessage(stderr);

        }// else
    return  wSt;
} // initJournaling

ZStatus
ZRawMasterFile::setJournalingOn (void)
{
ZStatus wSt;

if (!_isOpen)
        {
        fprintf (stderr,
                 "setJournalingOn-E File <%s> must be open to start/restart journaling.\n",
                 getURIContent().toString());
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                "File <%s> must be open to start/restart journaling.",
                                getURIContent().toString());
        return  ZS_INVOP;
        }

    if (BaseParameters->VerboseFileEngine())
            _DBGPRINT(
                     "setJournalingOn-I Starting/restarting journaling on for file %s\n",
                     getURIContent().toString())
    if (ZJCB->Journal==nullptr) {
      if (BaseParameters->VerboseFileEngine())
                _DBGPRINT(
                         "setJournalingOn-E Journaling has not be defined for file %s. Cannot start/restart journaling.\n",
                         getURIContent().toString())

      ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Error,
                              "Journaling has not be defined for file %s. Cannot start/restart journaling.",
                              getURIContent().toString());
      return  ZS_INVOP;
    }

    if (ZJCB->Journal->JThread.getState() > ZTHS_Nothing)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot start journaling for file %s : journaling already started. Thread id is %ld\n",
                     getURIContent().toString(),
                     ZJCB->Journal->JThread.getId());
            if (ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZJCB->Journal->getURIContent().toString());
                        return  ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Setting journaling on : Journaling for file %s is started while journal file is marked as not open.",
                                    getURIContent().toString());
            return  ZS_FILENOTOPEN;
            }

    wSt=ZJCB->Journal->init(true); // Journal file must exist
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While initializing journaling");
        return  wSt;
        }


    if (BaseParameters->VerboseFileEngine())
            _DBGPRINT(
                    "setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZJCB->Journal->getURIContent().toString())
    wSt=ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (BaseParameters->VerboseFileEngine())
            _DBGPRINT(
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZJCB->Journal->getURIContent().toString())
    }
    else
    {
        if (BaseParameters->VerboseFileEngine()) {
                    _DBGPRINT(
                            "setJournalingOn-E-Failure Journaling on file %s has not started.See ZException stack dump (following) to get information\n",
                            ZJCB->Journal->getURIContent().toString())
                    ZException.printUserMessage();
        }
    }// else

  return  wSt;
} // setJournalingOn

#ifdef __COMMENT__
/**
 * @brief ZRawMasterFile::setJournalingOn starts Journaling process for the given file.
 * If journaling is already started, nothing happens.
 * ZRawMasterFile must be open to start journaling, otherwise a ZS_INVOP status is returned.
 *
 * @return a ZStatus set to ZS_SUCCESS if everything went well
 */
ZStatus
ZRawMasterFile::setJournalingLOn()
{


ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
// this does not touch integrity of files

if (!_isOpen)
            {  return  (ZS_INVOP);}

    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOn-I Setting journaling on for file %s\n",
                     getURIContent().toString());
    if (ZJCB->Journal==nullptr)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                " Journaling parameters have not be defined for file <%s>. Cannot start/restart journaling.",
                                getURIContent().toString());
    }

        else

//    if (JournalingOn)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot set journaling on for file %s : journaling already started.\n",
                     getURIContent().toString());
            if (ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZJCB->Journal->getURIContent().toString());
                        return  ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Setting journaling on : Journaling for file %s is started while journal file is marked as not open.",
                                    getURIContent().toString());
            return  ZS_FILENOTOPEN;
            }
    wSt=ZJCB->Journal->init(true); // journaling file must have been created elsewhere and must exist
    if (wSt!=ZS_SUCCESS)
        {

        ZException.addToLast("While initializing journaling");
        return  wSt;
        }

/*
 //   JournalingOn=true;
 //   ZJCB->JournalLocalDirectoryPath = pJournalPath;
    wSt=writeControlBlocks();

    if (wSt!=ZS_SUCCESS)
                 {   return  wSt;}
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-MCB Journaling set on within MCB header \n");

    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZJCB->Journal->getURIContent().toString());
    wSt=ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZJCB->Journal->getURIContent().toString());
    }
    else
    {
        if (ZVerbose)
            {
                fprintf(stdout,
                        "setJournalingOn-E-Failure Journaling on file %s has not started.See ZException stack dump to get information\n",
                        ZJCB->Journal->getURIContent().toString());
                ZException.printUserMessage();
            }
    }// else

    if (!wasOpen)  // if ZRawMasterFile was not open close it
            {   return  zclose();}

    if (wFormerMode!=ZRF_Nothing) // if ZRawMasterFile was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open ZRawMasterFile with former mode
            }
    return  wSt;*/
} // setJournalingOn
#endif // __COMMENT__

ZStatus
ZRawMasterFile::setJournalingOff (void)
{


//    if (!JournalingOn)
//                  {  return  ZS_SUCCESS;}
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
    // this does not touch integrity of files

    if (!_isOpen)
            {

                wasOpen=false;
                wSt = zopen(ZRF_Exclusive|ZRF_Write);
                if (wSt!=ZS_SUCCESS)
                            {return  wSt; } // Beware return  is multiple instructions in debug mode
            }
        else
        {
        if (Mode!=(ZRF_Exclusive|ZRF_Write))
            {
            wFormerMode = Mode;
            zclose();
            zopen(ZRF_Exclusive|ZRF_Write);
            }

        }// else


    if (BaseParameters->VerboseFileEngine())
            _DBGPRINT("setJournalingOff-I Stopping journaling for file %s\n",
                     getURIContent().toString())
    if (ZJCB==nullptr)
            { return  ZS_SUCCESS;}  // Beware return  is multiple instructions in debug mode
    if (ZJCB->Journal==nullptr)
            { return  ZS_SUCCESS;}  // Beware return  is multiple instructions in debug mode

/*    if (!JournalingOn)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Journaling for file %s already stopped\n",
                     getURIContent().toString());
            if (ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning Journaling stopped but journal file %s is open. closing file\n",
                                 Journal->getURIContent().toString());
                        return  Journal->zclose();
                        }
            delete Journal;
            return  ZS_SUCCESS;
            }*/

    delete ZJCB;  // deletes Journaling Control Block  AND ZJournal instance : send a ZJOP_Close to journal thread



//    JournalingOn=false;
    wSt=writeControlBlocks();
    if (wSt!=ZS_SUCCESS)
                { return  wSt;} // Beware return  is multiple instructions in debug mode
    if (BaseParameters->VerboseFileEngine())
            _DBGPRINT("setJournalingOn-I-MCB Journaling for file %s has been set off within MCB header \n",
                    getURIContent().toCChar())


    if (!wasOpen)  // if was not open close it
            { return  zclose();} // Beware return  is multiple instructions in debug mode

    if (wFormerMode!=ZRF_Nothing) // if was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open with former mode
            }
    return  wSt;
} // setJournalingOff
/*
ZRawRecord*
ZRawMasterFile::generateRawRecord()
{
  return new ZRawRecord (this);
}
*/
//----------- ZRawMasterFile Index methods----------------
//


/**
 * @brief ZRawMasterFile::setIndexFilesDirectoryPath  sets a fixed Directory path into which index files will be stored for this master file
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
ZRawMasterFile::setIndexFilesDirectoryPath (uriString &pPath)
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

    _Base::ZReserved.clear();
    ZMasterControlBlock::_exportAppend(_Base::ZReserved);
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
ZRawMasterFile::setJournalLocalDirectoryPath (uriString &pPath)
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

    _Base::ZReserved.clear();
    ZMasterControlBlock::_exportAppend(_Base::ZReserved);
//    _Base::setReservedContent(_exportAppend(wReserved));
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


//------------------File Header : updates (read - write) ZMasterControlBlock AND ZSJournalControlBlock if exists

ZStatus
ZRawMasterFile::readJCBfromHeader(void)
{
ZStatus wSt;
ZDataBuffer wRawMCB;
    wSt=_Base::getReservedBlock(wRawMCB,true);
    if (wSt!=ZS_SUCCESS)
            {
            return  wSt;
            }
    return  _getJCBfromReserved();
}

ZStatus
ZRawMasterFile::_getJCBfromReserved(void)
{
    ZMCB_Export* wMCBe = (ZMCB_Export*)ZReserved.Data;

    wMCBe->deserialize();

    if (wMCBe->JCBOffset==0)  // no journaling
                {return  ZS_SUCCESS;} // Beware return  is multiple instructions in debug mode
    if (ZJCB==nullptr)
                {
                ZJCB=new ZJournalControlBlock;
                }

    const unsigned char* wPtrIn=ZReserved.Data+wMCBe->JCBOffset;

    return  ZJCB->_import(wPtrIn);
}



/**
 * @brief ZRawMasterFile::writeMasterControlBlock updates ZMasterControlBlock AND ZSJournalControlBlock (if exists)
 * for current ZRawMasterFile to ZReserved Header within header file.
 *
 * see @ref ZMasterControlBlock::_exportMCB()
 * see @ref ZSJournalControlBlock::_exportJCB()
 *
 * @return
 */
ZStatus
ZRawMasterFile::writeControlBlocks(void)
{
ZStatus wSt;
ZDataBuffer wReserved;

  _Base::ZReserved.clear();
  ZMasterControlBlock::_exportAppend(_Base::ZReserved);
//    ZMasterControlBlock::_exportAppend(wReserved);
//    _Base::setReservedContent(wReserved);
//    wSt=_Base::_writeAllHeaders(true);
    wSt=_Base::_writeAllFileHeader();
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot write Reserved Header (ZMasterControlBlock+ZSJournalControlBlock) to file header. Content file is <%s>",
                                getURIContent().toString());
        }
    return  wSt;
}//writeControlBlocks

/**
 * @brief ZRawMasterFile::readControlBlocks reads ZMasterControlBlock AND ZSJournalControlBlock is exists
 * see @ref ZMasterControlBlock::_import()
 * see @ref ZSJournalControlBlock::_import()
 * @return
 */
ZStatus
ZRawMasterFile::readControlBlocks(void)
{
ZStatus wSt;
ZDataBuffer wReserved;

const unsigned char*  wPtrIn=nullptr;
ZArray<ZPRES>    wIndexPresence;

    wSt=_Base::getReservedBlock(ZReserved,true);
    if (wSt!=ZS_SUCCESS)
            {
            return  wSt;
            }

//    wSt=_importMCB(wReserved);

    wPtrIn=ZReserved.Data;

//    wSt=_import(this,wPtrIn,wIndexPresence);
    wSt=_import(wPtrIn);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While importing ZMCB");
            return  wSt;
            }
  /*  if (hasJournal())
            {
            if (ZJCB!=nullptr)
                    delete ZJCB;
             return  wSt;
            }
*/
/*
    wPtrIn=ZReserved.Data+JCBOffset;
    ZJCB->_import(wPtrIn);
*/
    return wSt;
}// readControlBlocks


ZStatus
ZRawMasterFile::zcreateRawIndex ( ZRawIndexFile *&pIndexObjectOut,
                                  const utf8VaryingString &pIndexName,
                                  uint32_t pkeyguessedsize,
                                  ZSort_Type pDuplicates,
                                  long & pOutIndexRank,
                                  bool pBackup)
{
  ZStatus wSt;
//  bool wasOpen=false;
  long wi;
  zsize_type wIndexAllocatedSize=0;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
//  ZRawIndexFile *wIndexObject=nullptr;
  long w1=0,w2=0;


  pIndexObjectOut=nullptr;
  pOutIndexRank=-1;
  if  (getOpenMode()!=(ZRF_Exclusive | ZRF_All)) {
      ZException.setMessage("ZRawMasterFile::zcreateRawIndex",
          ZS_MODEINVALID,
          Severity_Error,
          " Trying to create index <%s> while file <%s> has invalid open mode  <%s> while required <ZRF_Exclusive|ZRF_All>.\n",
          pIndexName.toCChar(),
          URIContent.toCChar(),
          decode_ZRFMode(getMode()));
      ErrorLog.logZExceptionLast("ZRawMasterFile::zcreateRawIndex");
      return ZS_MODEINVALID;
      }

  // check index name ambiguity

  if ((wi=IndexTable.searchCaseIndexByName(pIndexName.toCChar()))>-1)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_INVNAME,
                          Severity_Error,
                          " Ambiguous index name <%s>. Index name already exist at Index rank <%ld>. Please use zremoveIndex first.\n",
                          pIndexName.toCChar(),
                          wi);
    wSt= ZS_INVNAME;
    goto zcreateRawIndexEnd ;
    }


  // instantiation of the ZIndexFile new structure :
  // it gives a pointer to the actual ICB stored in Index vector
  //
  pIndexObjectOut = new ZRawIndexFile(this,pkeyguessedsize,pIndexName,pDuplicates);

  pOutIndexRank=IndexTable.push(pIndexObjectOut); // since here any creation error will induce a desctruction of IndexTableObjects.lastIdx()

  // ---------compute index file name-------------------

  // Define IndexFileDirectoryPath
  //     if mentionned then take it
  //     if not then take the directory from Master File URI

  if (IndexFilePath.isEmpty())
  {
    //        utfdescString wDInfo;
    wIndexFileDirectoryPath=getURIContent().getDirectoryPath();
  }
  else
    {
    wIndexFileDirectoryPath=IndexFilePath;
    }

  wSt=generateIndexURI( wIndexURI,
                        getURIContent(),
                        wIndexFileDirectoryPath,
 //                       IndexTable.lastIdx(),
                        pIndexName);
  if (wSt!=ZS_SUCCESS)
  {return  wSt;} // Beware return  is multiple instructions in debug mode

  // compute the allocated size
  w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
  w2 = IndexTable[pOutIndexRank]->IndexRecordSize();

  if (_Base::getBlockTargetSize()>0)
    if (_Base::getAllocatedBlocks()>0)
      wIndexAllocatedSize =  _Base::getAllocatedBlocks() * IndexTable[pOutIndexRank]->IndexRecordSize();
  //
  // Nota Bene : there is no History and Journaling processing for Index Files
  //
  wSt =  pIndexObjectOut->zcreateIndexFile(*IndexTable.last(),      // pointer to index control block because ZIndexFile stores pointer to Father's ICB
                                            wIndexURI,
                                            _Base::getAllocatedBlocks(),
                                            _Base::getBlockExtentQuota(),
                                            wIndexAllocatedSize,
                                            _Base::getHighwaterMarking(),
                                            //                                      _Base::getGrabFreeSpace(),
                                            false,        // grabfreespace is set to false
                                            pBackup,
                                            true          // leave it open
                                            );
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;

  //    IndexTable.last()->generateCheckSum();

  // update MCB to Reserved block in RandomFile header (_Base)
  // then write updated Master Control Block to Master Header
  //
/*  wSt= pIndexObjectOut->openIndexFile(wIndexURI,wIndexRank, (ZRF_Exclusive | ZRF_All));
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;
*/

/*  @TODO : address rebuild index while creating a new index */
/*
  wSt=pIndexObjectOut->zrebuildRawIndex(ZMFStatistics ,stderr);
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;
*/

zcreateRawIndexEnd:
//  IndexCount = (uint32_t)IndexTable.count();
/*  zclose();  // close everything and therefore update MCB in file
  if (wasOpen)
    {
    return  zopen(wMode);
    }
*/
  return  wSt;
zcreateRawIndexError:
  if (pIndexObjectOut)
    {
    pIndexObjectOut->zclose();
    IndexTable.pop() ; // destroy the ZIndexFile object
    popIndex(); // destroy created ICB
    }
  pIndexObjectOut=nullptr;
  ZException.addToLast(" While creating new raw index <%s> for raw master file <%s>",
      pIndexName.toCChar(),
      URIContent.toCChar());
  goto zcreateRawIndexEnd;

}//zcreateRawIndex


ZStatus
ZRawMasterFile::zinsertRawIndex ( long pIndexRank,
                                  ZRawIndexFile *&pIndexObjectOut,
                                  const utf8VaryingString &pIndexName,
                                  uint32_t pkeyguessedsize,
                                  ZSort_Type pDuplicates,
                                  long & pOutIndexRank,
                                  bool pBackup)
{
  ZStatus wSt;
  //  bool wasOpen=false;
  long wi;
  zsize_type wIndexAllocatedSize=0;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
  //  ZRawIndexFile *wIndexObject=nullptr;
  long w1=0,w2=0;



  pIndexObjectOut=nullptr;
  pOutIndexRank=-1;
  if  (getOpenMode()!=(ZRF_Exclusive | ZRF_All)) {
    ZException.setMessage("ZRawMasterFile::zcreateRawIndex",
        ZS_MODEINVALID,
        Severity_Error,
        " Trying to create index <%s> while file <%s> has invalid open mode  <%s> while required <ZRF_Exclusive|ZRF_All>.\n",
        pIndexName.toCChar(),
        URIContent.toCChar(),
        decode_ZRFMode(getMode()));
    ErrorLog.logZExceptionLast();
    return ZS_MODEINVALID;
  }

  // check index name ambiguity

  if ((wi=IndexTable.searchCaseIndexByName(pIndexName.toCChar()))>-1)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVNAME,
        Severity_Error,
        " Ambiguous index name <%s>. Index name already exist at Index rank <%ld>. Please use zremoveIndex first.\n",
        pIndexName.toCChar(),
        wi);
    wSt= ZS_INVNAME;
    goto zcreateRawIndexEnd ;
  }


  // instantiation of the ZIndexFile new structure :
  // it gives a pointer to the actual ICB stored in Index vector
  //
  pIndexObjectOut = new ZRawIndexFile(this,pkeyguessedsize,pIndexName,pDuplicates);

  pOutIndexRank=IndexTable.push(pIndexObjectOut); // since here any creation error will induce a desctruction of IndexTableObjects.lastIdx()

  // ---------compute index file name-------------------

  // Define IndexFileDirectoryPath
  //     if mentionned then take it
  //     if not then take the directory from Master File URI

  if (IndexFilePath.isEmpty())
  {
    //        utfdescString wDInfo;
    wIndexFileDirectoryPath=getURIContent().getDirectoryPath();
  }
  else
  {
    wIndexFileDirectoryPath=IndexFilePath;
  }

  wSt=generateIndexURI( wIndexURI,
                        getURIContent(),
                        wIndexFileDirectoryPath,
//                        IndexTable.lastIdx(),
                        pIndexName);
  if (wSt!=ZS_SUCCESS)
  {return  wSt;} // Beware return  is multiple instructions in debug mode

  // compute the allocated size
  w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
  w2 = IndexTable[pOutIndexRank]->IndexRecordSize();

  if (_Base::getBlockTargetSize()>0)
    if (_Base::getAllocatedBlocks()>0)
      wIndexAllocatedSize =  _Base::getAllocatedBlocks() * IndexTable[pOutIndexRank]->IndexRecordSize();
  //
  // Nota Bene : there is no History and Journaling processing for Index Files
  //
  wSt =  pIndexObjectOut->zcreateIndexFile(*IndexTable.last(),      // pointer to index control block because ZIndexFile stores pointer to Father's ICB
      wIndexURI,
      _Base::getAllocatedBlocks(),
      _Base::getBlockExtentQuota(),
      wIndexAllocatedSize,
      _Base::getHighwaterMarking(),
      //                                      _Base::getGrabFreeSpace(),
      false,        // grabfreespace is set to false
      pBackup,
      true          // leave it open
      );
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;

  //    IndexTable.last()->generateCheckSum();

  // update MCB to Reserved block in RandomFile header (_Base)
  // then write updated Master Control Block to Master Header
  //
  /*  wSt= pIndexObjectOut->openIndexFile(wIndexURI,wIndexRank, (ZRF_Exclusive | ZRF_All));
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;
*/

/*
  wSt=pIndexObjectOut->zrebuildRawIndex(ZMFStatistics ,stderr);
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;

*/
zcreateRawIndexEnd:
 // IndexCount = (uint32_t)IndexTable.count();
  /*  zclose();  // close everything and therefore update MCB in file
  if (wasOpen)
    {
    return  zopen(wMode);
    }
*/
  return  wSt;
zcreateRawIndexError:


  if (pIndexObjectOut)
  {
    pIndexObjectOut->zclose();
    IndexTable.pop() ; // destroy the ZIndexFile object
    popIndex(); // destroy created ICB
  }
  pIndexObjectOut=nullptr;
  ZException.addToLast(" While creating new raw index <%s> for raw master file <%s>",
      pIndexName.toCChar(),
      URIContent.toCChar());
  goto zcreateRawIndexEnd;

}//zcreateRawIndex
#ifdef __COMMENT__
ZStatus
ZRawMasterFile::zcreateRawIndexDetailed (const utf8VaryingString &pIndexName, /*-----ICB------*/
                                          uint32_t pkeyguessedsize,
                                          ZSort_Type pDuplicates,
                                          long pAllocatedBlocks,      /* ---FCB (for index ZRandomFile)---- */
                                          long pBlockExtentQuota,
                                          zsize_type pInitialSize,
                                          bool pHighwaterMarking,
                                          bool pGrabFreeSpace,
                                          bool pReplace)
{
  ZStatus wSt;
//  bool wIsOpen = isOpen();
  long wi;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
  long w1=0,w2=0;
  long wIndexRank;
/*
  if (isOpen())
    {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Severe,
        " ZRawMasterFile <%s> is already open : must be closed before calling zcreateIndex",
        getURIContent().toCChar());
    return  ZS_MODEINVALID;
    }
  wSt=zopen(ZRF_Exclusive|ZRF_All);
  if (wSt!=ZS_SUCCESS)
    {
    ZException.addToLast(" While creating new Index <%s> for MasterFile <%s>",
        pIndexName.toCChar(),
        URIContent.toCChar());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
    }
*/

  if  (getMode()!=(ZRF_Exclusive|ZRF_All))
    {
      ZException.setMessage("ZRawMasterFile::zcreateRawIndexDetailed",
          ZS_MODEINVALID,
          Severity_Error,
          " Trying to create index <%s> while file <%s> has invalid open mode  <%s> while required <ZRF_Exclusive|ZRF_All>.\n",
          pIndexName.toCChar(),
          URIContent.toCChar(),
          decode_ZRFMode(getMode()));
      return ZS_MODEINVALID;
    }

  // check index name ambiguity


  wSt =_createRawIndexDet ( pIndexName,         /*-----ICB------*/
                            pkeyguessedsize,
                            pDuplicates,
                            pAllocatedBlocks,   /* ---FCB (for index ZRandomFile)---- */
                            pBlockExtentQuota,
                            pInitialSize,
                            pHighwaterMarking,
                            pGrabFreeSpace,
                            pReplace);
/*
  if (!wIsOpen)
    zclose();   // close everything and therefore update MCB in file
        // else All dependent files are open with mode (ZRF_Exclusive | ZRF_All) when return ing.
*/
  return  wSt;

} //zcreateRawIndexDetailed
#endif // __COMMENT__

#ifdef __COMMENT__
ZStatus
ZRawMasterFile::zcreateRawIndexDetailed (const utf8VaryingString &pIndexName, /*-----ICB------*/
                                          uint32_t pkeyguessedsize,
                                          ZSort_Type pDuplicates,
                                          long pAllocatedBlocks,      /* ---FCB (for index ZRandomFile)---- */
                                          long pBlockExtentQuota,
                                          zsize_type pInitialSize,
                                          bool pHighwaterMarking,
                                          bool pGrabFreeSpace,
                                          bool pReplace)
{
  ZStatus wSt;
  bool wIsOpen = isOpen();
  long wi;
  zsize_type wIndexAllocatedSize=0;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
  ZRawIndexFile *wIndexObject=nullptr;
  long w1=0,w2=0;
  long wIndexRank;

  if (isOpen())
  {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Severe,
        " ZRawMasterFile <%s> is already open : must be closed before calling zcreateIndex",
        getURIContent().toCChar());
    return  ZS_MODEINVALID;
  }
  wSt=zopen(ZRF_Exclusive|ZRF_All);
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" While creating new Index <%s> for MasterFile <%s>",
        pIndexName.toCChar(),
        URIContent.toCChar());
    ZException.setLastSeverity(Severity_Severe);
    return  wSt;
  }

  // check index name ambiguity

  if ((wi=IndexTable.searchCaseIndexByName(pIndexName.toCChar()))>-1)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVNAME,
        Severity_Error,
        " Ambiguous index name <%s>. Index name already exist at Index rank <%ld>. Please use zremoveIndex first.\n",
        pIndexName.toCChar(),
        wi);
    wSt= ZS_INVNAME;
    goto createRawIndexDetEnd ;
  }


  // instantiation of the ZIndexFile new structure :
  // it gives a pointer to the actual ICB stored in Index vector
  //
  wIndexObject = new ZRawIndexFile(this,pkeyguessedsize,pIndexName,pDuplicates);

  wIndexRank=IndexTable.push(wIndexObject); // since here any creation error will induce a desctruction of IndexTableObjects.lastIdx()

  if (wIndexRank < 0)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_ERROR,
        Severity_Error,
        "Index name <%s> : Cannot push index object to index table.\n",
        pIndexName.toCChar());
    delete wIndexObject;
    return ZS_ERROR;
    }

  // ---------compute index file name-------------------

  // Define IndexFileDirectoryPath
  //     if mentionned then take it
  //     if not then take the directory from Master File URI

  if (IndexFilePath.isEmpty())
  {
    //        utfdescString wDInfo;
    wIndexFileDirectoryPath=getURIContent().getDirectoryPath().toCChar();
  }
  else
  {
    wIndexFileDirectoryPath=IndexFilePath;
  }

  wSt=generateIndexURI( getURIContent(),
                        wIndexFileDirectoryPath,
                        wIndexURI,
                        IndexTable.lastIdx(),
                        pIndexName);
  if (wSt!=ZS_SUCCESS)
  {goto createRawIndexDetEnd;}

  // compute the allocated size
  w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
  w2 = IndexTable.last()->IndexRecordSize();

  if (_Base::getBlockTargetSize()>0)
    if (_Base::getAllocatedBlocks()>0)
      wIndexAllocatedSize =  _Base::getAllocatedBlocks() * IndexTable.last()->IndexRecordSize();
  //
  // Nota Bene : there is no History and Journaling processing for Index Files
  //
  wSt =  wIndexObject->zcreateIndex(*IndexTable[wIndexRank],      // pointer to index control block because ZIndexFile stores pointer to Father's ICB
                                    wIndexURI,
                                    pAllocatedBlocks,
                                    pBlockExtentQuota,
                                    pInitialSize,
                                    pHighwaterMarking,
                                    pGrabFreeSpace,        // grabfreespace is set to false
                                    pReplace,
                                    false          // do not leave it open
                                    );
  if (wSt!=ZS_SUCCESS)
    goto createRawIndexDetError;

  //    IndexTable.last()->generateCheckSum();

  // update MCB to Reserved block in RandomFile header (_Base)
  // then write updated Master Control Block to Master Header
  //
  wSt= wIndexObject->openIndexFile(wIndexURI,(ZRF_Exclusive | ZRF_All));
  if (wSt!=ZS_SUCCESS)
    goto createRawIndexDetError;

  wSt=wIndexObject->zrebuildIndex(ZMFStatistics ,stderr);
  if (wSt!=ZS_SUCCESS)
    goto createRawIndexDetError;


createRawIndexDetEnd:
  if (!wIsOpen)
    zclose();   // close everything and therefore update MCB in file
                // else All dependent files are open with mode (ZRF_Exclusive | ZRF_All) when return ing.
  return  wSt;
createRawIndexDetError:

  IndexTable[wIndexRank]->zclose();
  IndexTable.erase(wIndexRank) ; // destroy the ZIndexFile object
  ZException.addToLast(" While creating new raw index <%s> for raw master file <%s>",
      pIndexName.toCChar(),
      URIContent.toCChar());
  goto createRawIndexDetEnd;
}//_createRawIndexDetailed
#endif // __COMMENT__


void ZRawMasterFile::_testZReserved()
{
  if (ZReserved.Size == 0)
  {
    fprintf (stdout,"ZRawMasterFile::_testZReserved-I-EMPTY ZReserved is empty.\n");
    return ;
  }
  if (ZReserved.Size < sizeof(ZMCB_Export))
  {
    fprintf (stdout,"ZRawMasterFile::_testZReserved-I-CORRUPTED ZReserved has data but size is lower than minimum requested.\n");
    return ;
  }
  ZMCB_Export* wZMCBe=(ZMCB_Export*)ZReserved.Data;

  fprintf (stdout,"ZRawMasterFile::_testZReserved-I-REV ZReserved has data and is <%s>.\n",wZMCBe->isReversed()?"Serialized":"NOT Serialized");
  return ;
}

ZStatus
ZRawMasterFile::_createRawIndexDet (long &pOutRank,
                                    const utf8VaryingString &pIndexName,
                                    uint32_t pkeyguessedsize,
                                    ZSort_Type pDuplicates,
                                    long pAllocatedBlocks,      /* ---FCB parameters (for index ZRandomFile)---- */
                                    long pBlockExtentQuota,
                                    zsize_type pInitialSize,
                                    bool pHighwaterMarking,
                                    bool pGrabFreeSpace,
                                    bool pReplace,
                                    ZaiErrors* pErrorLog)
{
  ZStatus wSt;
//  bool wIsOpen = isOpen();
  long wi;
  zsize_type wIndexAllocatedSize=0;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
  ZRawIndexFile *wIndexObject=nullptr;
  ssize_t wRet=0;
  long w1=0,w2=0;

  pErrorLog->setContext("_createRawIndexDet");
  if (!isOpen())
    {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Error,
        " ZRawMasterFile <%s> must be open.",
        getURIContent().toCChar());
    pErrorLog->logZExceptionLast();
    pErrorLog->popContext();
    return  ZS_MODEINVALID;
    }

  if (!(getOpenMode()&(ZRF_Exclusive|ZRF_All)))
      {
      ZException.setMessage (_GET_FUNCTION_NAME_,
          ZS_MODEINVALID,
          Severity_Error,
          " ZRawMasterFile <%s> must be open with mode <ZRF_Exclusive|ZRF_All>.",
          getURIContent().toCChar());
      pErrorLog->logZExceptionLast();
      pErrorLog->popContext();

      return  ZS_MODEINVALID;
      }

  // check index name ambiguity

  if ((wi=IndexTable.searchCaseIndexByName(pIndexName.toCChar()))>-1)
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_INVNAME,
          Severity_Error,
          " Ambiguous index name <%s>. Index name already exist at Index rank <%ld>. Please use zremoveIndex first.\n",
          pIndexName.toCChar(),
          wi);
      pErrorLog->logZExceptionLast();
      wSt= ZS_INVNAME;
      goto createRawIndexDet1End ;
    }

  // instantiation of the ZIndexFile new structure :
  // it gives a pointer to the actual ICB stored in Index vector
  //
  wIndexObject = new ZRawIndexFile(this,pkeyguessedsize,pIndexName,pDuplicates);

  pOutRank=IndexTable.push(wIndexObject); // since here any creation error will induce a desctruction of IndexTableObjects.lastIdx()

  if (pOutRank < 0)
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_ERROR,
          Severity_Error,
          "Index name <%s> : Cannot push index object to index table.\n",
          pIndexName.toCChar());
      pErrorLog->logZExceptionLast();
      delete wIndexObject;
      pErrorLog->popContext();
      return ZS_ERROR;
    }

  // ---------compute index file name-------------------

  // Define IndexFileDirectoryPath
  //     if mentionned then take it
  //     if not then take the directory from Master File URI

  if (IndexFilePath.isEmpty())
    {
      //        utfdescString wDInfo;
      wIndexFileDirectoryPath=getURIContent().getDirectoryPath();
    }
  else
    {
      wIndexFileDirectoryPath=IndexFilePath;
    }

  wSt=generateIndexURI( wIndexURI,
                        getURIContent(),
                        wIndexFileDirectoryPath,
//                        pOutRank,
                        pIndexName);
  if (wSt!=ZS_SUCCESS)
  {goto createRawIndexDet1End;}

  // compute the allocated size
  w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
  w2 = IndexTable.last()->IndexRecordSize();

  if (_Base::getBlockTargetSize()>0)
    if (_Base::getAllocatedBlocks()>0)
      wIndexAllocatedSize =  _Base::getAllocatedBlocks() * IndexTable[pOutRank]->IndexRecordSize();
  //
  // Nota Bene : there is no History and Journaling processing for Index Files
  //
  wSt =  wIndexObject->zcreateIndexFile(*IndexTable[pOutRank],      // pointer to index control block because ZIndexFile stores pointer to Father's ICB
                                    wIndexURI,
                                    pAllocatedBlocks,
                                    pBlockExtentQuota,
                                    pInitialSize,
                                    pHighwaterMarking,
                                    pGrabFreeSpace,        // grabfreespace is set to false
                                    pReplace,
                                    false          // do not leave it open
                                    );
  if (wSt!=ZS_SUCCESS)
    goto createRawIndexDet1Error;

  //    IndexTable.last()->generateCheckSum();

  // update MCB to Reserved block in RandomFile header (_Base)
  // then write updated Master Control Block to Master Header
  //
  wSt= wIndexObject->openIndexFile(wIndexURI,pOutRank,(ZRF_Exclusive | ZRF_All));
  if (wSt!=ZS_SUCCESS)
    goto createRawIndexDet1Error;
/*
  wSt=wIndexObject->zrebuildRawIndex(ZMFStatistics ,stderr);
  if (wSt!=ZS_SUCCESS)
    goto createRawIndexDet1Error;
*/
createRawIndexDet1End:
  /* ICB has changed : force save all control blocks */
  ZReserved.clear();
  wRet=ZMasterControlBlock::_exportAppend(ZReserved);

  wSt=_writeAllFileHeader();

  pErrorLog->popContext();
  return  wSt;

createRawIndexDet1Error:
  if (IndexTable[pOutRank]->getURIContent().exists()) {
    IndexTable[pOutRank]->zclose();
    IndexTable[pOutRank]->_removeFile(true,&ErrorLog);
  }
  IndexTable.erase(pOutRank) ; // destroy the ZIndexFile object
  ZException.addToLast(" While creating new raw index <%s> for raw master file <%s>. Index has not been created.",
                        pIndexName.toCChar(),
                        URIContent.toCChar());
  pErrorLog->logZExceptionLast();
  pOutRank=-1;
  goto createRawIndexDet1End;
}//_createRawIndexDetailed


ZStatus
ZRawMasterFile::_insertRawIndexDet (long pInputIndexRank,
                                    const utf8VaryingString &pIndexName,/*-----ICB------*/
                                    uint32_t pkeyguessedsize,
                                    ZSort_Type pDuplicates,
                                    long pAllocatedBlocks,      /* ---FCB (for index ZRandomFile)---- */
                                    long pBlockExtentQuota,
                                    zsize_type pInitialSize,
                                    bool pHighwaterMarking,
                                    bool pGrabFreeSpace,
                                    bool pReplace,
                                    ZaiErrors* pErrorLog)
{
  ZStatus wSt;
  //  bool wIsOpen = isOpen();
  long wi;
  zsize_type wIndexAllocatedSize=0;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
  ZRawIndexFile *wIndexObject=nullptr;
  long w1=0,w2=0;

  if (!isOpen())
  {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Error,
        " ZRawMasterFile <%s> must be open.",
        getURIContent().toCChar());
    pErrorLog->logZExceptionLast();
    return  ZS_MODEINVALID;
  }

  if (!(getOpenMode()&(ZRF_Exclusive|ZRF_All)))
  {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Error,
        " ZRawMasterFile <%s> must be open with mode <ZRF_Exclusive|ZRF_All>.",
        getURIContent().toCChar());

    pErrorLog->logZExceptionLast();
    return  ZS_MODEINVALID;
  }

  // check index name ambiguity

  if ((wi=IndexTable.searchCaseIndexByName(pIndexName.toCChar()))>-1)
  {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_INVNAME,
        Severity_Error,
        " Ambiguous index name <%s>. Index name already exist at Index rank <%ld>. Please use zremoveIndex first.\n",
        pIndexName.toCChar(),
        wi);
    pErrorLog->logZExceptionLast();
    wSt= ZS_INVNAME;
    goto insertRawIndexDet1Error ;
  }


  // instantiation of the ZIndexFile new structure :

  // it gives a pointer to the actual ICB stored in Index vector
  //
  wIndexObject = new ZRawIndexFile(this,pkeyguessedsize,pIndexName,pDuplicates);

  if(IndexTable.insert(wIndexObject,pInputIndexRank)<0) {// since here any creation error will induce a desctruction of IndexTableObjects.lastIdx()
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_ERROR,
        Severity_Error,
        "Index name <%s> : Cannot insert index object to index table at rank %ld.\n",
        pIndexName.toCChar(),
        pInputIndexRank);
    pErrorLog->logZExceptionLast();
    delete wIndexObject;
    return ZS_ERROR;
  }

  // ---------compute index file name-------------------

  // Define IndexFileDirectoryPath
  //     if mentionned then take it
  //     if not then take the directory from Master File URI

  if (IndexFilePath.isEmpty())
  {
    //        utfdescString wDInfo;
    wIndexFileDirectoryPath=getURIContent().getDirectoryPath().toCChar();
  }
  else
  {
    wIndexFileDirectoryPath=IndexFilePath;
  }

  wSt=generateIndexURI( getURIContent(),
                        wIndexFileDirectoryPath,
                        wIndexURI,
//                        pInputIndexRank,
                        pIndexName);
  if (wSt!=ZS_SUCCESS)
  {goto insertRawIndexDet1Error;}

  // compute the allocated size
  w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
  w2 = IndexTable.last()->IndexRecordSize();

  if (_Base::getBlockTargetSize()>0)
    if (_Base::getAllocatedBlocks()>0)
      wIndexAllocatedSize =  _Base::getAllocatedBlocks() * IndexTable[pInputIndexRank]->IndexRecordSize();
  //
  // Nota Bene : there is no History and Journaling processing for Index Files
  //

  /* effective creation of ZRandomFile containing the index- enrich wIndexObject with file's parameters */

  wSt =  wIndexObject->zcreateIndexFile(*IndexTable[pInputIndexRank], // ZIndexFile inherits from ZRawIndexFile then from ZIndexControlBlock )
                                      wIndexURI,
                                      pAllocatedBlocks,
                                      pBlockExtentQuota,
                                      pInitialSize,
                                      pHighwaterMarking,
                                      pGrabFreeSpace,        // grabfreespace is set to false
                                      pReplace,
                                      false          // do not leave it open
                                      );
  if (wSt!=ZS_SUCCESS)
    goto insertRawIndexDet1Error;

  //    IndexTable.last()->generateCheckSum();

  // update MCB to Reserved block in RandomFile header (_Base)
  // then write updated Master Control Block to Master Header
  //
  wSt= wIndexObject->openIndexFile(wIndexURI,pInputIndexRank,(ZRF_Exclusive | ZRF_All));
  if (wSt!=ZS_SUCCESS)
    goto insertRawIndexDet1End;
/*
  wSt=wIndexObject->zrebuildRawIndex(ZMFStatistics ,stderr);
  if (wSt!=ZS_SUCCESS)
    goto insertRawIndexDet1Error;
*/
insertRawIndexDet1End:
  return  wSt;

insertRawIndexDet1Error:
  IndexTable[pInputIndexRank]->zclose();
  IndexTable[pInputIndexRank]->_removeFile(true,&ErrorLog);
  IndexTable.erase(pInputIndexRank) ; // destroy the ZIndexFile object
  ZException.addToLast(" While creating new raw index <%s> for raw master file <%s>. Index has not been created.",
      pIndexName.toCChar(),
      URIContent.toCChar());
  goto insertRawIndexDet1End;
}//_insertRawIndexDetailed



/**
 * @brief ZRawMasterFile::zremoveIndex Destroys an Index definition and its files' content on storage
 *
 *  Removes an index.
 *
 *      renames all index files of rank greater than the current on to make them comply with ZIndexFile naming rules.
 *
 *
 * @param pIndexRank Index rank to remove from ZRawMasterFile
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zremoveIndex (const long pIndexRank,bool pBackup,ZaiErrors* pErrorLog)
{

ZStatus wSt;
zmode_type wMode = Mode;
uriString FormerIndexContent;
uriString FormerIndexHeader;
uriString NewIndexContent;
uriString NewIndexHeader;

int wRet;
    if ((getOpenMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All))
            {
            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_MODEINVALID,
                                     Severity_Error,
                                     " ZRawMasterFile <%s> is open in bad mode for zremoveIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                                     getURIContent().toString());
            if (pErrorLog)
              pErrorLog->logZExceptionLast();
            return  ZS_MODEINVALID;
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
            if (pErrorLog)
              pErrorLog->logZExceptionLast();
            return  ZS_OUTBOUND;
            }

    wSt= IndexTable[pIndexRank]->zclose();  // close ZIndexFile files to delete
    if (wSt!=ZS_SUCCESS)
                { return  wSt;} // Beware return  is multiple instructions in debug mode



    wSt=IndexTable[pIndexRank]->_removeFile(&ErrorLog);  // remove the files
    if (wSt!=ZS_SUCCESS)
                { return  wSt;} // Beware return  is multiple instructions in debug mode

 //   removeICB(pIndexRank); // removes Index stuff IndexObjects suff and deletes appropriately

    IndexTable.erase(pIndexRank);
 //   IndexCount= IndexTable.size();

    if ((wSt=shiftIndexNameDown(pIndexRank,pErrorLog))!=ZS_SUCCESS) {
      return wSt;
    }
/*
    for (long wi = pIndexRank;wi<IndexTable.size();wi++)
        {
        FormerIndexContent = IndexTable[wi]->URIContent;
        FormerIndexHeader = IndexTable[wi]->URIHeader;

        IndexTable[wi]->zclose();// close index files before renaming its files

        wSt=generateIndexURI(getURIContent(),IndexFilePath,NewIndexContent,wi,IndexTable[wi]->IndexName);
        if (wSt!=ZS_SUCCESS)
                {
                return  wSt;// Beware return  is multiple instructions in debug mode
                }
        wSt=generateURIHeader(NewIndexContent,NewIndexHeader);
        if (wSt!=ZS_SUCCESS)
                {
                return  wSt;// Beware return  is multiple instructions in debug mode
                }
        IndexTable[wi]->URIContent = NewIndexContent;
        IndexTable[wi]->URIHeader = NewIndexHeader;

        wRet=rename(FormerIndexContent.toCChar(),NewIndexContent.toCChar());
        if (wRet)
        {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_FILEERROR,
                             Severity_Severe,
                             "Cannot rename index content file <%s> to <%s>",
                             FormerIndexContent.toString(),
                             NewIndexContent.toString());
            if (pErrorLog)
                pErrorLog->logZExceptionLast();
        }
        else
        {
            if (pErrorLog) {
              pErrorLog->infoLog("Index content file <%s> has been renamed to <%s>",
                  FormerIndexContent.toString(),
                  NewIndexContent.toString());
            }
        }

        wRet=rename(FormerIndexHeader.toCChar(),NewIndexHeader.toCChar());
        if (wRet)
          {
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_FILEERROR,
                             Severity_Severe,
                             "Cannot rename index header file <%s> to <%s>",
                             FormerIndexHeader.toString(),
                             NewIndexHeader.toString());
          if (pErrorLog)
            pErrorLog->logZExceptionLast();
          }
        else
        {
            if (pErrorLog) {
              pErrorLog->infoLog("Index header file <%s> has been renamed to <%s>",
                  FormerIndexHeader.toString(),
                  NewIndexHeader.toString());
            }
        }
        IndexTable[wi]->openIndexFile( NewIndexContent,wi,wMode);
        }// for
*/
    ZDataBuffer wMCBContent;
    return   _Base::updateReservedBlock(_exportAppend(wMCBContent),true);
}//zremoveIndex

ZStatus
ZRawMasterFile::shiftIndexNameDown(long pStartRank,ZaiErrors* pErrorLog) {
  ZStatus wSt=ZS_SUCCESS;
  uriString FormerIndexContent;
  uriString FormerIndexHeader;
  uriString NewIndexContent;
  uriString NewIndexHeader;
  zmode_type wMode = getOpenMode();


  for (long wi = pStartRank;wi<IndexTable.size();wi++)
  {
    FormerIndexContent = IndexTable[wi]->URIContent;
    FormerIndexHeader = IndexTable[wi]->URIHeader;

    IndexTable[wi]->zclose();// close index files before renaming its files

//    wSt=generateIndexURI(NewIndexContent,getURIContent(),IndexFilePath,wi,IndexTable[wi]->IndexName);
    wSt=generateIndexURI(NewIndexContent,getURIContent(),IndexFilePath,IndexTable[wi]->IndexName);
    if (wSt!=ZS_SUCCESS)
    {
      return  wSt;// Beware return  is multiple instructions in debug mode
    }
    wSt=generateURIHeader(NewIndexContent,NewIndexHeader);
    if (wSt!=ZS_SUCCESS)
    {
      return  wSt;// Beware return  is multiple instructions in debug mode
    }
    IndexTable[wi]->URIContent = NewIndexContent;
    IndexTable[wi]->URIHeader = NewIndexHeader;

    int wRet=rename(FormerIndexContent.toCChar(),NewIndexContent.toCChar());
    if (wRet)
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_FILEERROR,
          Severity_Severe,
          "Cannot rename index content file <%s> to <%s>",
          FormerIndexContent.toString(),
          NewIndexContent.toString());
      if (pErrorLog)
        pErrorLog->logZExceptionLast();
    }
    else
    {
      if (pErrorLog) {
        pErrorLog->infoLog("Index content file <%s> has been renamed to <%s>",
            FormerIndexContent.toString(),
            NewIndexContent.toString());
      }
    }

    wRet=rename(FormerIndexHeader.toCChar(),NewIndexHeader.toCChar());
    if (wRet)
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_FILEERROR,
          Severity_Severe,
          "Cannot rename index header file <%s> to <%s>",
          FormerIndexHeader.toString(),
          NewIndexHeader.toString());
      if (pErrorLog)
        pErrorLog->logZExceptionLast();
    }
    else
    {
      if (pErrorLog) {
        pErrorLog->infoLog("Index header file <%s> has been renamed to <%s>",
            FormerIndexHeader.toString(),
            NewIndexHeader.toString());
      }
    }
    IndexTable[wi]->openIndexFile( NewIndexContent,wi,wMode);
  }// for
  return ZS_SUCCESS;
}

/* must be launched before new index insertion */
ZStatus
ZRawMasterFile::shiftIndexNameUp(long pStartRank,ZaiErrors* pErrorLog) {
  ZStatus wSt=ZS_SUCCESS;
  uriString FormerIndexContent;
  uriString FormerIndexHeader;
  uriString NewIndexContent;
  uriString NewIndexHeader;
  zmode_type wMode = getOpenMode();


  for (long wi = pStartRank; wi<IndexTable.size() ;wi++)
  {
    FormerIndexContent = IndexTable[wi]->URIContent;
    FormerIndexHeader = IndexTable[wi]->URIHeader;

    IndexTable[wi]->zclose();// close index files before renaming its files

//    wSt=generateIndexURI(NewIndexContent,getURIContent(),IndexFilePath,wi+1,IndexTable[wi]->IndexName);
    wSt=generateIndexURI(NewIndexContent,getURIContent(),IndexFilePath,IndexTable[wi]->IndexName);
    if (wSt!=ZS_SUCCESS)
    {
      return  wSt;// Beware return  is multiple instructions in debug mode
    }
    wSt=generateURIHeader(NewIndexContent,NewIndexHeader);
    if (wSt!=ZS_SUCCESS)
    {
      return  wSt;// Beware return  is multiple instructions in debug mode
    }
    IndexTable[wi]->URIContent = NewIndexContent;
    IndexTable[wi]->URIHeader = NewIndexHeader;

    int wRet=rename(FormerIndexContent.toCChar(),NewIndexContent.toCChar());
    if (wRet)
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_FILEERROR,
          Severity_Severe,
          "Cannot rename index content file <%s> to <%s>",
          FormerIndexContent.toString(),
          NewIndexContent.toString());
      if (pErrorLog)
        pErrorLog->logZExceptionLast();
    }
    else
    {
      if (pErrorLog) {
        pErrorLog->infoLog("Index content file <%s> has been renamed to <%s>",
            FormerIndexContent.toString(),
            NewIndexContent.toString());
      }
    }

    wRet=rename(FormerIndexHeader.toCChar(),NewIndexHeader.toCChar());
    if (wRet)
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_FILEERROR,
          Severity_Severe,
          "Cannot rename index header file <%s> to <%s>",
          FormerIndexHeader.toString(),
          NewIndexHeader.toString());
      if (pErrorLog)
        pErrorLog->logZExceptionLast();
    }
    else
    {
      if (pErrorLog) {
        pErrorLog->infoLog("Index header file <%s> has been renamed to <%s>",
            FormerIndexHeader.toString(),
            NewIndexHeader.toString());
      }
    }
    IndexTable[wi]->openIndexFile( NewIndexContent,wi,wMode);
  }// for
  return ZS_SUCCESS;
}

ZStatus
ZRawMasterFile::zremoveAll() {
  ZStatus wSt=ZS_SUCCESS;

  if (getOpenMode()!=ZRF_All) {
    ZException.setMessage("ZRawMasterFile::zremoveAll",ZS_INVOPENMODE,Severity_Error,
        "Master file %s must be open in ZRF_All mode while mode is %s",
        getURIContent().toString(),
        decode_ZRFMode(getOpenMode()));
    return ZS_INVOPENMODE;
  }

  /* first delete all index key files records */

  if (BaseParameters->VerboseFileEngine()) {
    _DBGPRINT("ZRawMasterFile::zremoveAll-I- Removing all content of file <%s>.\n",getURIContent().toString())
    _DBGPRINT("ZRawMasterFile::zremoveAll-I- Removing content for indexes.\n")
  }


  for (long wi=0; wi < IndexTable.count(); wi++) {
    if (BaseParameters->VerboseFileEngine()) {
    _DBGPRINT("ZRawMasterFile::zremoveAll-I- Removing all content for index file <%s>\n",IndexTable[wi]->getURIContent().toString())
    }
    wSt=IndexTable[wi]->zremoveAll();
    if (wSt!=ZS_SUCCESS) {
      ZException.addToLast(" While deleting index key");
      if (BaseParameters->VerboseFileEngine()) {
        _DBGPRINT("ZRawMasterFile::zremoveAll-E Error while removing index file's content\n%s\n",ZException.last().formatFullUserMessage().toString())
      }
      return wSt;
    }
  }// for
  if (BaseParameters->VerboseFileEngine()) {
    _DBGPRINT("ZRawMasterFile::zremoveAll-I- Removing all content for master file itself.\n")
  }
  return ZRandomFile::zremoveAll();
}// zremoveAll


int
ZRawMasterFile::_getAvailableBckNum(const uriString& pBackupPath,const utf8VaryingString &pBckExt){
  uriString wBackupPath=pBackupPath;
  if (pBackupPath.isEmpty())
    wBackupPath = getURIContent().getDirectoryPath();
  wBackupPath.addConditionalDirectoryDelimiter();

  utf8VaryingString wMasterRootName = getURIContent().getRootname();
  utf8VaryingString wMasterBaseName = getURIContent().getBasename();

  utf8VaryingString wDicBaseName = getURIDictionary().getBasename();
  utf8VaryingString wDicPath = getURIDictionary().getDirectoryPath();
  uriString wURI ;
  int wBckNumber=1;
  if (Dictionary!=nullptr){
    wURI.sprintf("%s%s_%s%d", wBackupPath.toString(),wDicBaseName.toString(),pBckExt.toString(),wBckNumber);

    while (wURI.exists()) {
      wBckNumber++;
      wURI.sprintf("%s%s_%s%d", wBackupPath.toString(),wDicBaseName.toString(), pBckExt.toString(),wBckNumber);
    } // while
  }// if (Dictionary!=nullptr)

  while (true) {
    while (true) {
      wURI.sprintf("%s%s_%s%d",wBackupPath.toString(),wMasterBaseName.toString(),pBckExt.toString(),wBckNumber);
      if (wURI.exists())
        break;
      wURI.sprintf("%s%s_%s%d",wBackupPath.toString(),getURIHeader().getBasename().toString(),pBckExt.toString(),wBckNumber);
      if (wURI.exists())
        break;
      for (long wi=0; wi < IndexTable.count() ; wi++ ) {
        utf8VaryingString wIR = generateIndexRootName(wMasterRootName,IndexTable[wi]->IndexName);

        wURI.sprintf("%s%s.%s_%s%d",wBackupPath.toString(),wIR.toString(),__ZINDEX_FILEEXTENSION__,pBckExt.toString(),wBckNumber);
        if (wURI.exists())
          break;
        wURI.sprintf("%s%s.%s_%s%d",wBackupPath.toString(),wIR.toString(),__HEADER_FILEEXTENSION__,pBckExt.toString(),wBckNumber);
        if (wURI.exists())
          break;
      }
      /* no file with that wFormerNumber already exist */
      return wBckNumber;
    } // while true
    wBckNumber++;
  }// while true
} //_getAvailableBckNum


ZStatus
ZRawMasterFile::extractBasenameBack(const uriString& pBckFile,utf8VaryingString& pBasename,utf8VaryingString& pBckExt,int &pNum){

  utf8VaryingString wBasename=pBckFile.getBasename();

  utf8_t* wPtr=wBasename.Data;

  /* get till the end of full name string */
  while (*wPtr){
    wPtr++;
  }

  utf8_t* wPtrEnd = wPtr;

  /* capture initial base name  (root name + extension) */

  while (wPtr > wBasename.Data) {
    if (*wPtr == '_') {
      break;
    }
    if (*wPtr == '.')
      break;
    wPtr--;
  }
  if (wPtr==wBasename.Data)
    return ZS_INVNAME;
  if (*wPtr == '.') {  /* not a bck extension */
    ZException.setMessage("ZRawMasterFile::extractBasenameBack",ZS_INVTYPE,Severity_Error,"%s is not a backup file. Cannot find a valid bck extension (missing \'_\' character).",pBckFile.toString());
    return ZS_INVTYPE;
  }

  *wPtr=0;
  pBasename = wBasename;
  /* capture 2 last digits */

  pNum=0;
  int wPow = 1 ;
  bool wHasDigit=false;
  while (wPtrEnd > wPtr) {
    if ((*wPtrEnd < '0') || (*wPtrEnd > '9'))
      break;
    pNum += (int(*wPtrEnd)-int('0')) * wPow ;
    wPow *= 10 ;
    wHasDigit = true;
    wPtrEnd--;
  } // while

  if (!wHasDigit) {  /* not a bck extension */
    ZException.setMessage("ZRawMasterFile::extractBasenameBack",ZS_INVTYPE,Severity_Error,"%s is not a backup file. Cannot find a valid bck extension (missing trailing 2 digits).",pBckFile.toString());
    return ZS_INVTYPE;
  }

  /* capture bck extension : characters between wPtr and wPtrEnd */

  wPtrEnd++;
  *wPtrEnd = 0;

  wPtr++; /* skip 0 mark */

  pBckExt = wPtr;
  return ZS_SUCCESS ;
}// extractRootBack



ZStatus
ZRawMasterFile::backupAll_old(const uriString& pBackupPath,const utf8VaryingString &pBckExt) {
  ZStatus wSt=ZS_SUCCESS;

  uriString wBackupPath=pBackupPath;
  if (pBackupPath.isEmpty())
    wBackupPath = getURIContent().getDirectoryPath();
  wBackupPath.addConditionalDirectoryDelimiter();


  utf8VaryingString wMasterRootName = getURIContent().getRootname();
  utf8VaryingString wMasterBaseName = getURIContent().getBasename();

  utf8VaryingString wDicBaseName = getURIDictionary().getBasename();
  utf8VaryingString wDicPath = getURIDictionary().getDirectoryPath();


  uriString wURI ;

  int wBckNumber=_getAvailableBckNum(pBackupPath,pBckExt);

  if (Dictionary!=nullptr){
    wURI.sprintf("%s%s_%s%d", wBackupPath.toString(),wDicBaseName.toString(),pBckExt.toString(),wBckNumber);
    wSt=uriString::copyFile(wURI,Dictionary->URIDictionary);
    if (wSt!=ZS_SUCCESS) {
      ZException.addToLast(" from ZRawMasterFile::backupAll()");
      return wSt;
    }
  }// if (Dictionary!=nullptr)


  wURI.sprintf("%s%s%s_%s%d",wBackupPath.toString(),wMasterBaseName.toString(),pBckExt.toString(),wBckNumber);

  wSt=uriString::copyFile(wURI,getURIContent());
  if (wSt!=ZS_SUCCESS) {
    ZException.addToLast(" from ZRawMasterFile::backupAll()");
    return wSt;
  }
  wURI.sprintf("%s%s_%s%d",wBackupPath.toString(),getURIHeader().getBasename().toString(),pBckExt.toString(),wBckNumber);
  wSt=uriString::copyFile(wURI,getURIHeader());
  if (wSt!=ZS_SUCCESS) {
    ZException.addToLast(" from ZRawMasterFile::backupAll()");
    return wSt;
  }
  for (long wi=0; wi < IndexTable.count() ; wi++ ) {
    uriString wIdxSourcePath,wIdxSource;
    utf8VaryingString wIRoot = generateIndexRootName(wMasterRootName,IndexTable[wi]->IndexName);
    if(!IndexFilePath.isEmpty())
      wIdxSourcePath = IndexFilePath;
    else
      wIdxSourcePath = getURIContent().getDirectoryPath();

    wIdxSourcePath.addConditionalDirectoryDelimiter();

    wIdxSource = wIdxSourcePath;
    wIdxSource += wIRoot;
    wIdxSource += __ZINDEX_FILEEXTENSION__;

    wURI.sprintf("%s%s%s_%s%d.%s",wBackupPath.toString(),wIRoot.toString(),pBckExt.toString(),wBckNumber,__ZINDEX_FILEEXTENSION__);
    wSt=uriString::copyFile(wURI,wIdxSource);
    if (wSt!=ZS_SUCCESS) {
      ZException.addToLast(" from ZRawMasterFile::backupAll()");
      return wSt;
    }

    wIdxSource = wIdxSourcePath;
    wIdxSource += wIRoot;
    wIdxSource += __HEADER_FILEEXTENSION__;
    wURI.sprintf("%s%s%s_%s%d.%s",wBackupPath.toString(),wIRoot.toString(),pBckExt.toString(),wBckNumber,__HEADER_FILEEXTENSION__);

    wSt=uriString::copyFile(wURI,wIdxSource);
    if (wSt!=ZS_SUCCESS) {
      ZException.addToLast(" from ZRawMasterFile::backupAll()");
      return wSt;
    }
  } //for (long wi=0; wi < IndexTable.count() ; wi++ )
  return ZS_SUCCESS;
} // backupAll

ZStatus
ZRawMasterFile::backupAll(const uriString& pBackupPath,const utf8VaryingString& pBackupSetName) {
  ZStatus wSt=ZS_SUCCESS;
  /* --- New ---*/


  ZArray<BckElement> Files;
  if (pBackupSetName.isEmpty()){
    ZException.setMessage("ZRawMasterFile::backupAll",ZS_EMPTY,Severity_Error,
        "Backup set name is empty");
    return ZS_EMPTY;
  }
  if (pBackupPath.isEmpty()){
    ZException.setMessage("ZRawMasterFile::backupAll",ZS_EMPTY,Severity_Error,
        "Backup path is empty");
    return ZS_EMPTY;
  }
/*
  utf8VaryingString wBackupSetName = BckName;
  wBackupSetName += ZDateFull::currentDateTime().toUTCGMT() ;
*/
  uriString wTargetDir = pBackupPath;
  wTargetDir.addConditionalDirectoryDelimiter();

  if (!wTargetDir.exists()) {
    wSt = uriString::createDirectory(wTargetDir);
    if (wSt!=ZS_SUCCESS) {
      ZException.setMessage("ZRawMasterFile::backupAll",ZS_EMPTY,Severity_Error,
          "Cannot create directory <%s>",wTargetDir.toString());
      wTargetDir.addDirectoryDelimiter();
      return wSt;
    }
  }

  getBackupSet(Files);

  uriString wTargetFile ;

  int wSizeCur=0;
  for (long wi=0; wi < Files.count(); wi++) {

    wTargetFile = wTargetDir;
    wTargetFile += Files[wi].Target;

    Files[wi].Status=uriString::copyFile(wTargetFile,Files[wi].Source);

    wSizeCur += int(Files[wi].Size);
  }// for

  /* writing backup set xml file */

  uriString wURIBckset = utf8VaryingString(wTargetDir) + pBackupSetName;

  wSt = XmlSaveBackupset(wURIBckset,Files,true);
  if (wSt!=ZS_SUCCESS) {
    ZException.prependToLast("Problem while writing backup set xml description file <%s> \n",wURIBckset.toString());
  }

  return wSt;

} // backupAll


void
ZRawMasterFile::getBackupSet(ZArray<BckElement>& pFileList) {
  uriStat wStat;
  BckElement wElt;
  /* prepare files to copy and compute all file sizes */
  size_t wSize=0;
  /* master content file */
  wElt.Source = getURIContent();

  wStat=wElt.Source.getStatR();
  wElt.Created = wStat.Created;
  wElt.LastModified = wStat.LastModified;

  wSize += wElt.Size = wStat.Size ;

  wElt.Target = getURIContent().getBasename();
  pFileList.push(wElt) ;

  /* master header file */
  wElt.Source = getURIHeader();

  wStat=wElt.Source.getStatR();
  wElt.Created = wStat.Created;
  wElt.LastModified = wStat.LastModified;

  wSize += wElt.Size = wStat.Size ;

  wElt.Target = getURIHeader().getBasename();

  //  wElt.Target = wTargetDir;
  //  wElt.Target += MasterFile.getURIHeader().getBasename();
  pFileList.push(wElt) ;

  /* dictionary file if any */
  if (Dictionary!=nullptr) {
    wElt.Source = Dictionary->URIDictionary ;

    wStat=wElt.Source.getStatR();
    wElt.Created = wStat.Created;
    wElt.LastModified = wStat.LastModified;

    wSize += wElt.Size = wStat.Size ;

    wElt.Target = Dictionary->URIDictionary.getBasename();
    pFileList.push(wElt);
  }

  for (long wi=0; wi < IndexTable.count(); wi++) {

    wElt.Source = IndexTable[wi]->getURIContent() ;
    wElt.Size = wElt.Source.getFileSize();

    wStat=wElt.Source.getStatR();
    wElt.Created = wStat.Created;
    wElt.LastModified = wStat.LastModified;

    wSize += wElt.Size = wStat.Size ;

    wElt.Target = IndexTable[wi]->getURIContent().getBasename();
    pFileList.push(wElt);

    wElt.Source = IndexTable[wi]->getURIHeader() ;
    wElt.Size = wElt.Source.getFileSize();

    wStat=wElt.Source.getStatR();

    wElt.Created = wStat.Created;
    wElt.LastModified = wStat.LastModified;
    wSize += wElt.Size = wStat.Size ;

    wElt.Target = IndexTable[wi]->getURIHeader().getBasename();
    pFileList.push(wElt);
  } // for
}


ZStatus
ZRawMasterFile::XmlSaveBackupset(uriString& pXmlFile,ZArray<BckElement>& pFileList, bool pComment) {
  utf8VaryingString wReturn = fmtXMLdeclaration();
  int wLevel=0;
  wReturn += fmtXMLnodeWithAttributes("zbackupset","version",__ZRF_XMLVERSION_CONTROL__,0);

  wReturn+=fmtXMLdatefull("date",ZDateFull::currentDateTime(),wLevel);

  wReturn += fmtXMLnode("filelist",wLevel);
  wLevel++;

  for (long wi=0; wi < pFileList.count();wi++) {
    wReturn += fmtXMLnode("file",wLevel);
    wLevel++;
    wReturn+=fmtXMLchar("source",pFileList[wi].Source.toString(),wLevel);
    wReturn+=fmtXMLchar("target",pFileList[wi].Target.toString(),wLevel);
    wReturn+=fmtXMLulong("size",pFileList[wi].Size,wLevel);
    wReturn+=fmtXMLZStatus("status",pFileList[wi].Status,wLevel);
    wReturn+=fmtXMLdatefull("creationdate",pFileList[wi].Created,wLevel);
    wReturn+=fmtXMLdatefull("modificationdate",pFileList[wi].LastModified,wLevel);
    wLevel--;
    wReturn += fmtXMLendnode("file",wLevel);
  }

  wLevel--;
  wReturn += fmtXMLendnode("filelist",wLevel);
  wLevel--;

  wReturn += fmtXMLendnode("zbackupset",0);

  return pXmlFile.writeContent(wReturn);
}

ZStatus
ZRawMasterFile::XmlLoadBackupset(const uriString& pXmlFile,ZArray<BckElement>& pFileList,ZDateFull& pBackupDate)
{
  ZStatus wSt;

  utf8VaryingString wXmlContent;

  zxmlDoc *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;
  zxmlElement *wFileRootNode=nullptr;
  zxmlElement *wSingleFileNode=nullptr;
  zxmlElement *wSwapNode=nullptr;

  wSt = pXmlFile.loadUtf8(wXmlContent);
  if (wSt!=ZS_SUCCESS) {
    ZException.prependToLast("Problem while loading backup set xml description file <%s> \n",pXmlFile.toString());
    return wSt;
  }

  wDoc = new zxmlDoc;
  wSt = wDoc->ParseXMLDocFromMemory(wXmlContent.toCChar(), wXmlContent.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    ZException.setMessage("ZRawMasterFile::XmlLoadBackupset",wSt,Severity_Error,"Xml parsing error for xml document <%s>",
        wXmlContent.subString(0, 25).toUtf());
    return wSt;
  }
  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    return wSt;
  }
  if (!(wRoot->getName() == "zbackupset")) {
    ZException.setMessage("ZRawMasterFile::XmlLoadBackupset",ZS_XMLINVROOTNAME,Severity_Error,
        "Invalid root node name <%s> expected <zbackupset>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  wSt=XMLgetChildZDateFull(wSingleFileNode,"date",pBackupDate);

  wSt=wRoot->getChildByName((zxmlNode*&)wFileRootNode,"filelist");
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("ZRawMasterFile::XmlLoadBackupset",ZS_XMLMISSREQ,Severity_Error,
        "Cannot find node name <filelist>");
    return ZS_XMLMISSREQ;
  }

  wSt=wFileRootNode->getFirstChild((zxmlNode*&)wSingleFileNode);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("ZRawMasterFile::XmlLoadBackupset",ZS_XMLCORRUPTED,Severity_Error,
        "Cannot find child node name under node <filelist>. Xml document is incomplete or corrupted.");
    return ZS_XMLCORRUPTED;
  }
  if (wSingleFileNode->getName()!="file") {
    ZException.setMessage("ZRawMasterFile::XmlLoadBackupset",ZS_XMLINVNODENAME,Severity_Error,
        "Got node name <%s> while expecting node <file>",wSingleFileNode->getName().toString());
    return ZS_XMLINVNODENAME;
  }

  pFileList.clear();  /* clear files definitions */
  while (wSt==ZS_SUCCESS)
  {
    BckElement wElt;
    wSt=XMLgetChildText(wSingleFileNode,"source",wElt.Source);
    wSt=XMLgetChildText(wSingleFileNode,"target",wElt.Target);
    wSt=XMLgetChildULong(wSingleFileNode,"size",wElt.Size);
    wSt=XMLgetChildZStatus(wSingleFileNode,"status",(int64_t&)wElt.Status);
    wSt=XMLgetChildZDateFull(wSingleFileNode,"creationdate",wElt.Created);
    wSt=XMLgetChildZDateFull(wSingleFileNode,"modificationdate",wElt.LastModified);

    wSt=wSingleFileNode->getNextNode((zxmlNode*&)wSwapNode);
    XMLderegister(wSingleFileNode);
    wSingleFileNode=wSwapNode;
  } // while

  XMLderegister(wSingleFileNode);
  XMLderegister(wFileRootNode);
  XMLderegister(wRoot);

  return ZS_SUCCESS;
} // ZMFBckDLg::XmlLoadBackupset



//---------------------------------Utilities-----------------------------------------------------

/**  * @addtogroup ZMFUtilities
 * @{ */

/**
 * @brief ZRawMasterFile::zclearMCB Sets the current ZRawMasterFile's ZMCB to its minimum value. Destroys all existing indexes, and their related files.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.upgradelog
 * @return
 */
ZStatus
ZRawMasterFile::zclearMCB (ZaiErrors* pErrorLog)
{
ZStatus wSt;
ZDataBuffer wMCBContent;
/*
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
*/

    if ((getOpenMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All)) {
        ZException.setMessage ("ZRawMasterFile::zclearMCB",
                              ZS_MODEINVALID,
                              Severity_Error,
                              " ZRawMasterFile <%s> is open in bad mode. Must be (ZRF_Exclusive|ZRF_All) or closed",
                              URIContent.toString());
        pErrorLog->logZExceptionLast("ZRawMasterFile::zclearMCB");
        return  ZS_MODEINVALID;
    }



    pErrorLog->textLog("Clearing ZMasterControlBlock of file <%s>", URIContent.toString());
    ZMasterControlBlock::report(pErrorLog);

    while (IndexTable.size()>0) {
            wSt=zremoveIndex(IndexTable.lastIdx());
            if (wSt!=ZS_SUCCESS) {
                pErrorLog->logZExceptionLast("ZRawMasterFile::zclearMCB");
                pErrorLog->textLog(
                    " ****Error: removing index rank <%ld> status <%s> clearing ZMasterControlBlock of file <%s>",
                    IndexTable.lastIdx(),
                    decode_ZStatus(wSt),
                    URIContent.toString());

                return  wSt;
        }// not ZS_SUCCESS
        pErrorLog->textLog("      Index successfully removed");
        ZMasterControlBlock::report(pErrorLog);
    }//while
/*
    if (FOutput)
            fclose(wOutput);
*/
    ZMasterControlBlock::clear();
    return   _Base::updateReservedBlock(_exportAppend(wMCBContent),true);
}//zclearMCB

/** @ */ // ZMFUtilities


/**
 * @brief ZRawMasterFile::zcreate  ZRawMasterFile creation with a full definition with a file path that will name main content file.
 * Other file names will be deduced from this name.
 * @note At this stage, no indexes are created for ZRawMasterFile.
 *
 * Main file content and file header are created with appropriate parameters as given in parameters.
 * ZRawMasterFile infradata structure is created within header file.
 *
 * @param[in] pURI  uriString containing the path of the future ZRawMasterFile main content file.
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
ZRawMasterFile::zcreateRawMasterFile(const uriString pURI,
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
    wSt=ZRandomFile::setPath(pURI);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" While creating Master file %s",
                                 getURIContent().toString());
        return  wSt;
        }

    ZRandomFile::setCreateMaximum (pInitialSize,
                                      pAllocatedBlocks,
                                      pBlockExtentQuota,
                                      pBlockTargetSize,
                                      pHighwaterMarking,
                                      pGrabFreeSpace);

    wSt=ZRandomFile::_create(pInitialSize,ZFT_ZRawMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" While creating Master file %s",
                                 getURIContent().toString());
        return  wSt;
        }


//  ZHeader.FileType = ZFT_ZRawMasterFile;     // setting ZFile_type
  _Base::ZReserved.clear();
  ZMasterControlBlock::_exportAppend(_Base::ZReserved);

//  ZMasterControlBlock::_exportAppend(wMCBContent);
//  ZRandomFile::setReservedContent(wMCBContent);

//  wSt=ZRandomFile::_writeFullFileHeader(true);
  wSt=ZRandomFile::_writeAllFileHeader();
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
                ZException.addToLast(" while creating ZRawMasterFile %s",
                                       getURIContent().toString());
                return  wSt;
                }
        }

    if (pLeaveOpen)
            { return  wSt;}


    return   zclose();
}// zcreate

ZStatus ZRawMasterFile::_writeAllHeaders() {
  _Base::ZReserved.clear();
  ZMasterControlBlock::_exportAppend(_Base::ZReserved);

  ZStatus wSt=ZRandomFile::_writeAllFileHeader();
  if (wSt!=ZS_SUCCESS)
  {
    ZException.addToLast(" writing headers for Master file %s",
        getURIContent().toString());
    return  wSt;
  }


  for (long wi=0;wi < IndexTable.count() ; wi++) {
    wSt=IndexTable[wi]->_writeAllFileHeader();
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast(" writing headers for Master file %s",
          getURIContent().toString());
      return  wSt;
    }
  }
  return ZS_SUCCESS;
}


/**
 * @brief ZRawMasterFile::zcreate Creates the raw content file and its header as a ZRandomFile with a structure capable of creating indexes.
 * @param[in] pURI  uriString containing the path of the future ZRawMasterFile main content file. Other file names will be deduced from this main name.
 * @param[in] pInitialSize Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pBackup    If set to true file will be replaced if it already exists. If false (default value), existing file will be renamed according renaming rules.
 * @param[in] pLeaveOpen   If set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zcreateRawMasterFile (const uriString pURI, const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{

ZStatus wSt;
ZDataBuffer wMCBContent;
//    ZMFURI=pURI;

    printf ("ZRawMasterFile::zcreate \n");

    wSt=ZRandomFile::setPath (pURI);
    if (wSt!=ZS_SUCCESS)
                {return (wSt);}
    ZRandomFile::setCreateMinimum(pInitialSize);
    wSt=ZRandomFile::_create(pInitialSize,ZFT_ZRawMasterFile,pBackup,true); // calling ZRF creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return (wSt);
            }

    ZHeader.FileType = ZFT_ZRawMasterFile;     // setting ZFile_type (Already done in _create routine but as ZRandomFiles)
    if (pLeaveOpen) {
      _Base::ZReserved.clear();
      ZMasterControlBlock::_exportAppend(_Base::ZReserved);
      wSt=ZRandomFile::_writeAllFileHeader();
      if (wSt!=ZS_SUCCESS) {
        ZException.addToLast("\nWhile creating Raw Master file %s",
                                             getURIContent().toString());
        return  wSt;
      }

      return  wSt;
    }// if (pLeaveOpen)

    zclose(); // updates headers including reserved block (MCB)
    return  wSt;
}//zcreate





//----------------End zcreate--------------------



/**
 * @brief ZRawMasterFile::zopen opens the ZRawMasterFile and all its associated ZIndexFiles with a mode set to defined mask pMode
 * @param pURI  MasterFile file path to open
 * @param pMode Open mode (mask)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
ZRawMasterFile::zopen  (const uriString &pURI, const int pMode)
{
ZStatus wSt;

const unsigned char*wPtrIn = nullptr;

  if (pURI.isEmpty())
    {
    ZException.setMessage("ZRawMasterFile::zopen ",
                          ZS_INVNAME,
                          Severity_Error,
                          "Invalid file name <empty>.");
    return ZS_INVNAME;
    }
    wSt=ZRandomFile::setPath(pURI);
    wSt=ZRandomFile::_ZRFopen (pMode,ZHeader.FileType);/* opens and get all headers including Reserved header : ZReserved */
    if (wSt!=ZS_SUCCESS)
            return  wSt;

    wPtrIn=ZReserved.Data;

    wSt=ZMasterControlBlock::_import(wPtrIn); /* beware wPtrIn is modified by _import */

    if (wSt!=ZS_SUCCESS)
                    {return  wSt;}// Beware return  is multiple instructions in debug mode

// MCB is loaded
//----------Journaling----------------------

//    if (JournalingOn)
    if (hasJournal())  {   // if journaling requested
        if (ZJCB->Journal==nullptr) // if no journaling : create one
                {
                ZJCB->Journal=new ZSJournal(this);
                wSt=ZJCB->Journal->init();
                if (wSt!=ZS_SUCCESS)
                        {return  wSt;}// Beware return  is multiple instructions in debug mode
//                ZJCB->Journal->start();
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
 /*       const unsigned char* wPtrIn=ZReserved.Data+JCBOffset;

        ZJCB->_import(wPtrIn);
*/
        ZJCB->Journal->init();
        ZJCB->Journal->start();
        } // JournalingOn


//--------------End journaling----------------------------

//    Index table has been created during import operation.
//    Each index object must be opened has an index file
//    IndexTable.clear();
    uriString wIndexUri;
long wi;

    for (wi=0;wi < IndexTable.size();wi++)
            {
//           ZIndexFile* wIndex = new ZIndexFile (this,(ZSIndexControlBlock&) *IndexTable[wi]);
//            IndexTable.push(wIndex);
            wIndexUri.clear();

            wSt=generateIndexURI(wIndexUri,pURI,IndexFilePath,IndexTable[wi]->IndexName);
            if (wSt!=ZS_SUCCESS)
                    {
                    return  wSt;// Beware return  is multiple instructions in debug mode
                    }
            if (BaseParameters->VerboseFileEngine())
              _DBGPRINT("Opening Index file <%s>\n",(const char*)wIndexUri.toString())

            wSt=IndexTable[wi]->openIndexFile(wIndexUri,wi,pMode);
            if (wSt!=ZS_SUCCESS)
                    {
                    ZException.addToLast("\n while opening index rank <%ld>", wi);
// In case of open index error : close any already opened index file
//              Then close master content file before return ing
                    ZStatus wSvSt = wSt;
                    long wj;
                    for (wj = 0;wj < wi;wj++) {
                      IndexTable[wj]->zclose();  // use the base ZRandomFile zclose routine
                    } // for
                     _Base::zclose();

                    return  wSt;
                    }
            }// for


    return  ZS_SUCCESS;
}// zopen


ZStatus
ZRawMasterFile::zopenIndexFile(long pRank,const int pMode) {
  uriString wIndexUri;

//  ZStatus wSt=generateIndexURI(wIndexUri,getURIContent(),IndexFilePath,pRank,IndexTable[pRank]->IndexName);
  ZStatus wSt=generateIndexURI(wIndexUri,getURIContent(),IndexFilePath,IndexTable[pRank]->IndexName);
  if (wSt!=ZS_SUCCESS)
  {
    return  wSt;// Beware return  is multiple instructions in debug mode
  }
  if (BaseParameters->VerboseFileEngine())
    _DBGPRINT("Opening Index file <%s>\n",(const char*)wIndexUri.toString())

  return IndexTable[pRank]->openIndexFile(wIndexUri,pRank,pMode);
} // zopenIndexFile

/**
 * @brief ZRawMasterFile::zclose close the ZRawMasterFile, and all dependent ZIndexFiles
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zclose(void)
{
ZStatus wSt;
ZStatus SavedSt=ZS_SUCCESS;
ZDataBuffer wMCBContent;

// closing index files

  if (!_isOpen) {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                " file is not open : cannot close it.");
        return  ZS_INVOP;
    }
  for (long wi=0;wi < IndexTable.size();wi++)
    {
    if ((wSt=IndexTable[wi]->closeIndexFile())!=ZS_SUCCESS)
        {
        ZException.printUserMessage(stderr);  // error on close is not blocking because we need to close ALL files
        SavedSt = wSt;
        }
    }// for

// flush MCB to file
    size_t wRet=_exportAppend(wMCBContent);
    setReservedContent(wMCBContent);

/* Following is not necessary : zclose does the job
 *
    wSt=ZRandomFile::updateReservedBlock(wMCBContent,true);// force to write

    if (wSt!=ZS_SUCCESS) {
      ZException.addToLast( "\nWriting Reserved header for ZRawMasterFile %s",
                                        getURIContent().toString());
      ZRandomFile::_forceClose();
      return  wSt;
    }
*/
// releasing index resources

    while (IndexTable.size()>0) {
      IndexTable.pop();
    }
// ending journaling process

//    setJournalingOff();  // nope

// closing main ZMF content file

    wSt=ZRandomFile::zclose();  /*  zclose writes file header blocks (HCB, MCB FCB) */
    if (wSt!=ZS_SUCCESS)
            {
            return  wSt;// Beware return  is multiple instructions in debug mode
            }
/*    if (SavedSt!=ZS_SUCCESS)
                { return  SavedSt;}// Beware return  is multiple instructions in debug mode
*/
    ZReserved.clear();
    return  ZS_SUCCESS;
}// zclose

ZStatus
ZRawMasterFile::_removeFile(const uriString& pContentPath, ZaiErrors *pErrorLog)
{
  ZStatus wRetSt=ZS_SUCCESS;
  if (isOpen())
    zclose();
  ZStatus wSt=zopen(pContentPath,ZRF_Exclusive);
  if (wSt!=ZS_SUCCESS)
    {
    if (pErrorLog!=nullptr)
        pErrorLog->logZExceptionLast();
    zclose();
    return wSt;
    }
  for (long wi=0;wi < IndexTable.size();wi++)
  {
    if ((wSt=IndexTable[wi]->_removeFile(pErrorLog))!=ZS_SUCCESS)
      wRetSt= wSt;

  }// for
  wSt= ZRandomFile::_removeFile(pErrorLog);
  if (wSt!=ZS_SUCCESS)
    {
    zclose();
    return wSt;
    }
  return wRetSt;
}//_removeFile


ZStatus
ZRawMasterFile::removeMasterFile (const uriString& pContentPath, ZaiErrors *pErrorLog)
{
  ZRawMasterFile wZRMF;
  return wZRMF._removeFile(pContentPath,pErrorLog);
}

ZStatus ZRawMasterFile::_renameBck(const uriString &pContentPath,
                                   ZaiErrors *pErrorLog,
                                   bool pNoExcept,
                                   const char *pBckExt)
{
  ZStatus wRetSt=ZS_SUCCESS;
  uriString wNewURI;
  if (isOpen())
    zclose();
  ZStatus wSt=zopen(pContentPath,ZRF_Exclusive);
  if (wSt!=ZS_SUCCESS)
    {
    if (pErrorLog!=nullptr)
      pErrorLog->logZExceptionLast();
    zclose();
    return wSt;
    }

  int wBckNumber = _testBckNumber(1,pBckExt);
  for (long wi=0;wi < IndexTable.size();wi++)
  {
      wBckNumber=IndexTable[wi]->_testBckNumber(wBckNumber,pBckExt);
  }// for
  if (hasDictionary()) {
      uriString wDic = getURIDictionary();
      do {
          wDic = getURIDictionary();
          wDic.addsprintf("_%s%02d",pBckExt,wBckNumber);
          wBckNumber ++;
      } while ((wDic.exists()));
  }

  wNewURI=URIContent;
  wNewURI.addsprintf("_%s%02d",pBckExt,wBckNumber);
  wSt=rawRename(URIContent,wNewURI,pNoExcept,pErrorLog);

  wNewURI=URIHeader;
  wNewURI.addsprintf("_%s%02d",pBckExt,wBckNumber);
  wSt=rawRename(URIHeader,wNewURI,pNoExcept,pErrorLog);



  for (long wi=0;wi < IndexTable.size();wi++) {
      wNewURI=IndexTable[wi]->URIContent;
      wNewURI.addsprintf("_%s%02d",pBckExt,wBckNumber);
      wSt=rawRename(URIContent,wNewURI,pNoExcept,pErrorLog);

      wNewURI=IndexTable[wi]->URIHeader;
      wNewURI.addsprintf("_%s%02d",pBckExt,wBckNumber);
      wSt=rawRename(URIHeader,wNewURI,pNoExcept,pErrorLog);
  }// for

  if (hasDictionary()) {
      wNewURI=getURIDictionary();
      wNewURI.addsprintf("_%s%02d",pBckExt,wBckNumber);
      wSt=rawRename(getURIDictionary(),wNewURI,pNoExcept,pErrorLog);
  }

  return wSt;
}//_renameBck

ZStatus ZRawMasterFile::renameBck(const uriString &pContentPath,
                                  ZaiErrors *pErrorLog,
                                  bool pNoExcept,
                                  const char *pBckExt)
{
  ZRawMasterFile wZRMF;
  return wZRMF._renameBck(pContentPath,pErrorLog,pBckExt);
}


/**
 * @brief ZRawMasterFile::zget gets a record from Raw Master File at logical position pZMFRank
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
ZStatus ZRawMasterFile::zget(ZDataBuffer &pRecordContent, const zrank_type pZMFRank)
{
  return _getRaw(pRecordContent, pZMFRank);
}// ZRawMasterFile::zget
/**
 * @brief ZRawMasterFile::_getRaw gets a raw record pRecord at relative file position pZMFRank.
 * If operation is successfull (ZS_SUCCESS), then pRawRecord has been set with following data :
 *  - full raw record content : ZRawRecord::RawContent
 *  - record content : ZRawRecord::Content
 *  - keys raw content : ZRawRecord::KeyValue
 * @param pRecord resulting full record content
 * @param pZMFRank relative position for record to get
 * @return a ZStatus
 */
ZStatus
ZRawMasterFile::_getRaw(ZDataBuffer& pRecord, const zrank_type pRank)
{
  zaddress_type wAddress;
  /*
  ZDataBuffer   wRawRecord;
  ZStatus wSt= ZRandomFile::zgetWAddress(wRawRecord,pRank,wAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  uint64_t wRecSize;
  const unsigned char* wPtr=wRawRecord.Data;
  _importAtomic<uint64_t>(wRecSize,wPtr);
  pRecord.setData(wPtr,size_t(wRecSize));
  return wSt;
*/
  return ZRandomFile::zgetWAddress(pRecord,pRank,wAddress);
}// ZRawMasterFile::_getRaw


ZStatus ZRawMasterFile::_getFirst (ZDataBuffer &pRecord) {
  return _getRaw(pRecord,0L);
}
ZStatus ZRawMasterFile::_getNext (ZDataBuffer &pRecord) {
  ZStatus wSt=ZS_SUCCESS;
  wSt=testRank(CurrentRank+1,"ZRawMasterFile::_getNext");
  if (wSt!=ZS_SUCCESS)
    return wSt;
  CurrentRank++;
  return _getRaw(pRecord,CurrentRank);
}

//----------------Insert sequence------------------------------------------------------

/**
 * @brief ZRawMasterFile::zinsert Inserts a new record given by pRecord at forced position pZMFRank within Master File and updates all indexes

Add operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
If an error occur, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
If all indexes are successfully updated, then update on Master File is committed .

 if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically removed from index file.

* @param[in] pRecord    user record content to insert in a ZDataBuffer
* @param[in] pZMFRank   relative position within the ZRawMasterFile to insert the record (@see ZRandomFile
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError


 */
ZStatus
ZRawMasterFile::zinsert (ZDataBuffer& pRecordContent, ZArray<ZDataBuffer> &pKeys, const zrank_type pZMFRank)
{

  if (pKeys.count() != IndexTable.count())
    {
    ZException.setMessage("ZRawMasterFile::zinsert", ZS_INVSIZE, Severity_Error,
        "Invalid key count. Provided <%ld> keys while Index number is <%ld>.",
        pKeys.count(), IndexTable.count());
    return ZS_INVSIZE;
    }
    return _insertRaw(pRecordContent,pKeys,pZMFRank);
}// zinsert


#ifdef __OLD_VERSION__

ZStatus
ZRawMasterFile::_insertRaw       (ZRawRecord *pRecord, const zrank_type pZMFRank)
{
  ZStatus wSt;

  zrank_type      wZMFIdxCommit;
  zaddress_type   wZMFAddress=0;

  ZArray <zrank_type>       IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
  ZIndexItemList      IndexItemList;          // stores keys description per index processed

  ZIndexItem*    wIndexItem;
  zrank_type      wIndex_Rank;

  /* prepare the add on Master File, reserve appropriate space, get entry in pool.
   * NB: wZMFIdxCommit must be equal to requested rank (pRecord->Rank) in case of success or -1 if fails.
   */

  pRecord->Rank = pZMFRank;
  wZMFIdxCommit=pRecord->Rank ;
  wSt=_Base::_insert2Phases_Prepare(pRecord->RawContent,wZMFIdxCommit,pRecord->Address);
  if (wSt!=ZS_SUCCESS)
    goto _insertRaw_error;

  //
  // update all Indexes
  //

  for (wIndex_Rank=0;wIndex_Rank< IndexTable.size();wIndex_Rank++)
  {
    wIndexItem = new ZIndexItem;
    wIndexItem->Operation=ZO_Insert;

    wSt=IndexTable[wIndex_Rank]->_extractRawKey(pRecord,wIndexItem->KeyContent);
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During _extractRawKey operation on index number <%ld>",wIndex_Rank);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _add_RollbackIndexes (IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2Phases_Rollback(wZMFIdxCommit); // do not accept update on Master File and free resources
      goto _insertRaw_error;
    }

    wSt=IndexTable[wIndex_Rank]->_addRawKeyValue_Prepare(wIndexItem,wZMFIdxCommit, wZMFAddress);// for indexes don't care about insert, this is an add key value
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During zinsert operation on index number <%ld>",wIndex_Rank);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _add_RollbackIndexes ( IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2Phases_Rollback(wZMFIdxCommit); // do not accept update on Master File and free resources
      goto _insertRaw_error;
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

    goto _insertRaw_error;
  }

  // at this stage all indexes have been committed
  //         commit for Master file data must be done now
  //
  wSt = _Base::_insert2Phases_Commit(pRecord->RawContent,wZMFIdxCommit,wZMFAddress);// accept insert update on Master File
  if (wSt!=ZS_SUCCESS)    //! and if then an error occur : hard rollback all indexes and signal exception
  {
    _add_HardRollbackIndexes (IndexRankProcessed); // indexes are already committed so use hardRollback to counter pass
  }                                                    // don't care about the status . In case of error exception stack will trace it


  if (getJournalingStatus())
  {
    ZJCB->Journal->enqueue(ZJOP_Insert,pRecord->RawContent);
  }

_insertRaw_error:
  //  _Base::_unlockFile () ; // set Master file unlocked
  IndexItemList.clear();
  return  wSt;
}// _insertRaw
#endif // __OLD_VERSION__

//------------End insert sequence----------------------------------------------------

//----------------Add sequence------------------------------------------------------




/**
 * @brief ZRawMasterFile::zadd Adds a new record within a Raw Master File and updates all indexes
 *
 *      Add operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
 *      If an error occur, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
 *      If all indexes are successfully updated, then update on Master File is committed .
 *
 *      if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically removed from index file.
 *
 * @param[in] pRecordContent  user record content to add in a ZDataBuffer.
 * @param[in] pKeys           an array of corresponding keys content related to record content.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus ZRawMasterFile::zadd (ZDataBuffer& pRecordContent, ZArray<ZDataBuffer> &pKeys )
{
  if (pKeys.count() != IndexTable.count())
    {
    ZException.setMessage("ZRawMasterFile::zadd",
        ZS_INVSIZE,
        Severity_Error,
        "Invalid key count. Provided <%ld> keys while Index number is <%ld>.", pKeys.count(),IndexTable.count());
    return ZS_INVSIZE;
    }
/*
  RawRecord->Content = pRecordContent;
  for (long wi=0;wi < pKeys.count();wi++)
    RawRecord->setRawKeyContent(wi,pKeys[wi]);
//  RawRecord->prepareForWrite(pRecordContent);
*/
  /* to be changed */
  return _addRaw(pRecordContent,pKeys);
}

ZStatus
ZRawMasterFile::_addRaw(ZDataBuffer& pRecord, ZArray<ZDataBuffer> &pKeysContent)
{
  ZStatus wSt=ZS_SUCCESS;

  zrank_type      wZMFZBATRank ;
  zaddress_type   wZMFAddress;

  ZIndexItemList        IndexItemList;      // stores keys description per index processed

  ZIndexItem     *wIndexItem=nullptr;
  zrank_type      wIndex_Rank;

  if (pKeysContent.size() != IndexTable.count()) {
    ZException.setMessage("ZRawMasterFile::zaddRaw",
        ZS_CORRUPTED,
        Severity_Severe,
        "Master file number of indexes <%ld> does not correspond to given keys <%ld>. File <%s> cannot be accessed.\n"
                          "Record source definition must be outdated or invalid.\n"
                          "Align <.h> header file with file structure.",
        IndexTable.count(),pKeysContent.size() ,
        getURIContent().toString());
    return  ZS_CORRUPTED;
  }

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


  wSt=_Base::_add2Phases_Prepare( pRecord,  wZMFZBATRank,   // get internal ZBAT pool allocated rank
                                            wZMFAddress);       // get also zmf record address
  if (wSt!=ZS_SUCCESS)
    {
    _Base::_add2Phases_Rollback(wZMFZBATRank);
    goto zaddRaw_end;
    }

  // =========== update all defined Indexes ======================


  for (wIndex_Rank=0;wIndex_Rank< IndexTable.size();wIndex_Rank++)
  {
    if (BaseParameters->VerboseFileEngine())
      _DBGPRINT( "ZRawMasterFile::_addRaw  processing key rank <%ld>\n",wIndex_Rank)


    wSt=IndexTable[wIndex_Rank]->_addRawKeyValue_Prepare(wIndexItem,pKeysContent[wIndex_Rank], wZMFAddress);
    if (wSt!=ZS_SUCCESS)
      {
      ZException.addToLast(" During _addRawKeyValue_Prepare operation on key rank <%ld>",wIndex_Rank);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2Phases_Rollback(wZMFZBATRank); // do not accept update on Master File and free resources
      goto zaddRaw_end;
      }

    IndexItemList.push(wIndexItem);            // with that key content

    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);

  }// main for - IndexTableObjects.size()

  // so far everything when well
  //     commit Indexes changes
  //      if an error occurs during index commit :
  //          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
  // Nb: Exception(s) is(are) pushed on stack

  wSt=_commitIndexes (IndexItemList) ;
  if (wSt!=ZS_SUCCESS)
  {
    _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack

    // Soft rollback master update regardless returned ZStatus
    // Nb: Exception is pushed on stack. ZException keeps the last status.
    _Base::_add2Phases_Rollback(wZMFZBATRank);

    goto zaddRaw_end;
  }
  // at this stage all indexes have been committed
  //         commit for Master file data must be done now
  //
  wSt = _Base::_add2Phases_Commit(pRecord,wZMFZBATRank,wZMFAddress);// accept update on Master File

// Then: if then an error occurs at this stage : hard rollback all indexes since begin till processed index
  if (wSt!=ZS_SUCCESS) {
    _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack
  }                                              // we don't care about the status . In case of error exception stack will trace it

  if ((wSt==ZS_SUCCESS) && getJournalingStatus())
    {
//    IndexItemList._exportAppend(wEffectiveRecord);
    ZJCB->Journal->enqueue(ZJOP_Add,pRecord);
    }
zaddRaw_end:
//  _Base::_unlockFile () ; // set Master file unlocked
  return  wSt;

}// _addRaw


ZStatus
ZRawMasterFile::_addRawDisregardKeys(ZDataBuffer& pRecord)
{
  ZStatus wSt=ZS_SUCCESS;

  zrank_type      wZMFZBATRank ;
  zaddress_type   wZMFAddress;

  ZIndexItemList        IndexItemList;      // stores keys description per index processed

  ZIndexItem     *wIndexItem=nullptr;
  zrank_type      wIndex_Rank;


  if (!isOpen())
  {
    ZException.setMessage("ZRawMasterFile::_addRawDisregardKeys",
                          ZS_FILENOTOPEN,
                          Severity_Severe,
                          " File <%s> is not open while trying to access it",
                          getURIContent().toString());
    return  ZS_FILENOTOPEN;
  }

  // prepare the add on Master File, reserve appropriate space, get entry in pool, lock it


  wSt=_Base::_add2Phases_Prepare( pRecord,  wZMFZBATRank,   // get internal ZBAT pool allocated rank
                                   wZMFAddress);       // get also zmf record address
  if (wSt!=ZS_SUCCESS)
  {
    _Base::_add2Phases_Rollback(wZMFZBATRank);
    goto zaddRawDisregard_end;
  }
    // at this stage all indexes have been committed
  //         commit for Master file data must be done now
  //
  wSt = _Base::_add2Phases_Commit(pRecord,wZMFZBATRank,wZMFAddress);// accept update on Master File

  // Then: if then an error occurs at this stage : hard rollback all indexes since begin till processed index
  if (wSt!=ZS_SUCCESS) {
    _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack
  }                                              // we don't care about the status . In case of error exception stack will trace it

  if ((wSt==ZS_SUCCESS) && getJournalingStatus())
  {
    ZJCB->Journal->enqueue(ZJOP_Add,pRecord);
  }
zaddRawDisregard_end:
  HealthStatus |= HSTP_IndexMustRebuild ;
  //  _Base::_unlockFile () ; // set Master file unlocked
  return  wSt;

}// _addRawDisregardKeys

#ifdef __DEPRECATED__
ZStatus
ZRawMasterFile::_addRawOld(ZDataBuffer& pRecord, ZArray<ZDataBuffer> &pKeysContent)
{
  ZDataBuffer wEffectiveRecord;
  ZStatus wSt=ZS_SUCCESS;

  zrank_type      wZMFZBATIndex ;
  zaddress_type   wZMFAddress;

  ZIndexItemList        IndexItemList;      // stores keys description per index processed

  ZIndexItem     *wIndexItem=nullptr;
  zrank_type      wIndex_Rank;

  if (pKeysContent.count() != IndexTable.count()) {
    ZException.setMessage("ZRawMasterFile::zaddRaw",
        ZS_CORRUPTED,
        Severity_Severe,
        "Master file number of indexes <%ld> does not correspond to given keys <%ld>. File <%s> cannot be accessed.",
        IndexTable.count(),pKeysContent.count() ,
        getURIContent().toString());
    return  ZS_CORRUPTED;
  }

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

/* Appropriate space ;
    uint64_t  size of user record

    ....  record content (pRecord) ....

    user record buffer space given by pRecord.Size
    uint32_t  number of keys
    zaddress_type for key 1
    zaddress_type for key 2
      ...
    zaddress_type for key n


IMPORTANT : wEffectiveRecord must not be modified until its final setup

*/
  wEffectiveRecord.setData_T<uint64_t>(uint64_t(pRecord.Size));
  wEffectiveRecord.appendData( pRecord);
  unsigned char* wPtr=wEffectiveRecord.extend(sizeof(uint32_t)+ (pKeysContent.count() * sizeof(zaddress_type)));

  wSt=_Base::_add2Phases_Prepare( wEffectiveRecord,  wZMFZBATIndex,   // get internal ZBAT pool allocated index
                                                     wZMFAddress);       // get also zmf record address
  if (wSt!=ZS_SUCCESS)
    {
    _Base::_add2Phases_Rollback(wZMFZBATIndex);
    goto zaddRaw_end;
    }

  // =========== update all defined Indexes ======================


  for (wIndex_Rank=0;wIndex_Rank< IndexTable.size();wIndex_Rank++)
  {
    if (BaseParameters->VerboseFileEngine())
      _DBGPRINT( "ZRawMasterFile::_addRaw  processing key rank <%ld>\n",wIndex_Rank)


    wSt=IndexTable[wIndex_Rank]->_addRawKeyValue_Prepare(wIndexItem,pKeysContent[wIndex_Rank], wZMFAddress);
    if (wSt!=ZS_SUCCESS)
      {
      ZException.addToLast(" During _addRawKeyValue_Prepare operation on key rank <%ld>",wIndex_Rank);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2Phases_Rollback(wZMFZBATIndex); // do not accept update on Master File and free resources
      goto zaddRaw_end;
      }

    IndexItemList.push(wIndexItem);            // with that key content

    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);

  }// main for - IndexTableObjects.size()

  // so far everything when well
  //     commit Indexes changes
  //      if an error occurs during index commit :
  //          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
  // Nb: Exception(s) is(are) pushed on stack

  wSt=_commitIndexes (IndexItemList) ;
  if (wSt!=ZS_SUCCESS)
  {
    _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack

    // Soft rollback master update regardless returned ZStatus
    // Nb: Exception is pushed on stack. ZException keeps the last status.
    _Base::_add2Phases_Rollback(wZMFZBATIndex);

    goto zaddRaw_end;
  }

  /* Before committing Raw Master File (writting record)
          set all index record addresses to ZMF record
   */
  _exportAtomicPtr<uint32_t>(pKeysContent.count() , wPtr);

  for (long wi = 0; wi < pKeysContent.count() ; wi++)
    _exportAtomicPtr<zaddress_type>(IndexItemList[wi]->IndexAddress , wPtr);

  // at this stage all indexes have been committed
  //         commit for Master file data must be done now
  //
//  wSt = _Base::_add2PhasesCommit_Commit(_Base::pRecord->RawContent,wZMFZBATIndex,wZMFAddress);// accept update on Master File
  wSt = _Base::_add2Phases_Commit(pRecord,wZMFZBATIndex,wZMFAddress);// accept update on Master File

// Then: if then an error occurs at this stage : hard rollback all indexes since begin till processed index
  if (wSt!=ZS_SUCCESS) {
    _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack
  }                                              // we don't care about the status . In case of error exception stack will trace it

  if ((wSt==ZS_SUCCESS) && getJournalingStatus())
    {
    IndexItemList._exportAppend(wEffectiveRecord);
    ZJCB->Journal->enqueue(ZJOP_Add,wEffectiveRecord);
    }
zaddRaw_end:
//  _Base::_unlockFile () ; // set Master file unlocked
  return  wSt;
}// _addRawOld
#endif // __DEPRECATED__


ZStatus
ZRawMasterFile::_insertRaw (const ZDataBuffer& pRecord, ZArray<ZDataBuffer> &pKeys, const zrank_type pZMFRank)
{
  ZStatus wSt;

  //  zrank_type      wZMFIdxCommit;
  zaddress_type   wZMFAddress=0;

  //  ZArray <zrank_type> IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
  ZIndexItemList      IndexItemList;          // stores keys description per index processed

  ZIndexItem*     wIndexItem=nullptr;
  zrank_type      wIndex_Rank;


  wSt=_Base::_insert2Phases_Prepare(pRecord,pZMFRank,wZMFAddress);
  if (wSt!=ZS_SUCCESS)
    goto _insertRaw_error;
  //
  // update all Indexes
  //

  for (wIndex_Rank=0;wIndex_Rank< IndexTable.size();wIndex_Rank++)
  {
    wSt=IndexTable[wIndex_Rank]->_addRawKeyValue_Prepare(wIndexItem,pKeys[wIndex_Rank], wZMFAddress);// for indexes don't care about insert, this is an add key value

    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During zinsert operation on index number <%ld>",wIndex_Rank);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2Phases_Rollback(pZMFRank); // do not accept update on Master File and free resources
      goto _insertRaw_error;
    }

    //    IndexRankProcessed.push(wIndex_Rank) ;     // this index has been added to this rank
    IndexItemList.push(wIndexItem);          // with that key content

    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);
  }// main for

  // so far everything when well
  //     commit Indexes changes
  //      if an error occurs during index commit :
  //          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
  // Nb: Exception(s) is(are) pushed on stack
  wSt=_commitIndexes (IndexItemList) ;
  if (wSt!=ZS_SUCCESS)
  {
    // Soft rollback master update regardless returned ZStatus
    // Nb: Exception is pushed on stack. ZException keeps the last status.
    _Base::_add2Phases_Rollback(pZMFRank);
    _rollbackIndexes ( IndexItemList);
    goto _insertRaw_error;
  }

  // at this stage all indexes have been committed
  //
  /* Before committing Raw Master File (writting record)
          set all index record addresses to ZMF record
   */

  wSt = _Base::_insert2Phases_Commit(pRecord,pZMFRank,wZMFAddress);// accept insert update on Master File
  if (wSt!=ZS_SUCCESS)    //! and if then an error occur : hard rollback all indexes and signal exception
  {
    _rollbackIndexes (IndexItemList); // indexes are already committed so use hardRollback to counter pass
        // nb: roll back routine manages soft and hard rollback
  }                                 // don't care about the status . In case of error exception stack will trace it


  if (getJournalingStatus())
  {
//    IndexItemList._exportAppend(wEffectiveRecord);
    ZJCB->Journal->enqueue(ZJOP_Insert,pRecord);
  }

_insertRaw_error:
  //  _Base::_unlockFile () ; // set Master file unlocked
  IndexItemList.clear();
  return  wSt;
}// _insertRaw

#ifdef __DEPRECATED__

ZStatus
ZRawMasterFile::_insertRawOld (const ZDataBuffer& pRecord, ZArray<ZDataBuffer>& pKeys, const zrank_type pZMFRank)
{
  ZDataBuffer wEffectiveRecord;
  ZStatus wSt;

  //  zrank_type      wZMFIdxCommit;
  zaddress_type   wZMFAddress=0;

  //  ZArray <zrank_type> IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
  ZIndexItemList      IndexItemList;          // stores keys description per index processed

  ZIndexItem*     wIndexItem=nullptr;
  zrank_type      wIndex_Rank;

/* Appropriate space ;
    uint64_t  size of user record

    ....  record content (pRecord) ....

    user record buffer space given by pRecord.Size
    uint32_t  number of keys
    zaddress_type for key 1
    zaddress_type for key 2
      ...
    zaddress_type for key n


IMPORTANT : wEffectiveRecord must not be modified until its final setup

*/
  wEffectiveRecord.setData_T<uint64_t>(uint64_t(pRecord.Size));
  wEffectiveRecord.appendData( pRecord);
  unsigned char* wPtr=wEffectiveRecord.extend(sizeof(uint32_t)+ (pKeys.count() * sizeof(zaddress_type)));

  wSt=_Base::_insert2Phases_Prepare(wEffectiveRecord,pZMFRank,wZMFAddress);
  if (wSt!=ZS_SUCCESS)
    goto _insertRaw_error;
  //
  // update all Indexes
  //

  for (wIndex_Rank=0;wIndex_Rank< IndexTable.size();wIndex_Rank++)
  {
    wSt=IndexTable[wIndex_Rank]->_addRawKeyValue_Prepare(wIndexItem,pKeys[wIndex_Rank], wZMFAddress);// for indexes don't care about insert, this is an add key value

    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During zinsert operation on index number <%ld>",wIndex_Rank);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _rollbackIndexes ( IndexItemList); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2Phases_Rollback(pZMFRank); // do not accept update on Master File and free resources
      goto _insertRaw_error;
    }

    //    IndexRankProcessed.push(wIndex_Rank) ;     // this index has been added to this rank
    IndexItemList.push(wIndexItem);          // with that key content

    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);
  }// main for

  // so far everything when well
  //     commit Indexes changes
  //      if an error occurs during index commit :
  //          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
  // Nb: Exception(s) is(are) pushed on stack
  wSt=_commitIndexes (IndexItemList) ;
  if (wSt!=ZS_SUCCESS)
  {
    // Soft rollback master update regardless returned ZStatus
    // Nb: Exception is pushed on stack. ZException keeps the last status.
    _Base::_add2Phases_Rollback(pZMFRank);
    _rollbackIndexes ( IndexItemList);
    goto _insertRaw_error;
  }

  // at this stage all indexes have been committed
  //
  /* Before committing Raw Master File (writting record)
          set all index record addresses to ZMF record
   */
  _exportAtomicPtr<uint32_t>(pKeys.count() , wPtr);

  for (long wi = 0; wi < pKeys.count() ; wi++)
    _exportAtomicPtr<zaddress_type>(IndexItemList[wi]->IndexAddress , wPtr);


  wSt = _Base::_insert2Phases_Commit(pRecord,pZMFRank,wZMFAddress);// accept insert update on Master File
  if (wSt!=ZS_SUCCESS)    //! and if then an error occur : hard rollback all indexes and signal exception
  {
    _rollbackIndexes (IndexItemList); // indexes are already committed so use hardRollback to counter pass
        // nb: roll back routine manages soft and hard rollback
  }                                 // don't care about the status . In case of error exception stack will trace it


  if (getJournalingStatus())
  {
    IndexItemList._exportAppend(wEffectiveRecord);
    ZJCB->Journal->enqueue(ZJOP_Insert,wEffectiveRecord);
  }

_insertRaw_error:
  //  _Base::_unlockFile () ; // set Master file unlocked
  IndexItemList.clear();
  return  wSt;
}// _insertRawOld
#endif // __DEPRECATED__


/**
 * @brief ZRawMasterFile::_removeByRankRaw Removes a record corresponding to logical position pZMFRank within Master File and updates all indexes
 *
 * Remove operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
 *
 * @note  If an error occurs during processing, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
 * If all indexes are successfully updated, then update on Master File is committed .
 * if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically recreated within index file.
 * ZRawMasterFile internal data structures (descriptor, MCB, ICBs) are updated during remove operation.
 *
 * @param[out] pZMFRecord the Raw Record updated with record content that is being removed
 * @param[in] pZMFRank the logical position (rank) of the record to remove
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */

ZStatus
ZRawMasterFile::_removeByRankR  (ZDataBuffer &pRecord, const zrank_type pZMFRank)
{
  ZStatus     wSt;
  ZArray<zaddress_type>   wIndexAddresses;
  ZIndexItemList          IndexItemList;      // stores keys description per index processed (rollback purpose). Must stay as pointer (double free)
  ZIndexItem              *wIndexItem;
  ZDataBuffer             wKeyContent;
  zaddress_type           wZMFAddress;

  ZDataBuffer         wEffectiveRecord;
  ZArray<ZDataBuffer> wKeys;

  //---- get Master File header test for lock - if OK lock it

  wSt=_Base::_removeR_Prepare(wEffectiveRecord,pZMFRank,wZMFAddress);  // prepare remove on Master File mark entry in pool as to be deleted
  if (wSt!=ZS_SUCCESS)
  {
    _Base::_remove_Rollback(pZMFRank);
    return  wSt;
  }

  /* extract record data : user record content - list of corresponding index record addresses*/
  uint64_t wUserContentSize;

  const unsigned char* wPtr = wEffectiveRecord.Data;

  _importAtomic<uint64_t>(wUserContentSize,wPtr);

  /* point to index addresses block */
  wPtr += size_t(wUserContentSize);
  uint32_t wIndexCount;

  _importAtomic<uint32_t>(wIndexCount,wPtr);

  /* import all adresses */
  zaddress_type wIdxAd;
  for (uint32_t wi=0 ; wi < wIndexCount ; wi++) {
    _importAtomic<zaddress_type>(wIdxAd,wPtr);
    wIndexAddresses.push(wIdxAd);
  }
  //
  // update all Indexes
  //

  IndexItemList.clear();
  long wi;
  long wIdxRank;
  // must extract keys value per key to remove each index key one by one
  //  pRawRecord->getContentFromRaw(pRawRecord->Content,pRawRecord->RawContent);

  for (wi=0;wi< wKeys.count();wi++) {

    wIndexItem = new ZIndexItem;
    wIndexItem->setBuffer(wKeys[wi]);
    wIndexItem->ZMFAddress = wZMFAddress;
    wIndexItem->IndexAddress = wIndexAddresses[wi];

    wSt=IndexTable[wi]->_removeRByAddress_Prepare(wKeyContent,wIdxRank , wIndexAddresses[wi]);

//    wSt=IndexTable[wi]->_removeIndexItem_Prepare(wIndexItem , wi);
    if (wSt!=ZS_SUCCESS)
    {
      //                delete wIndexItem; // free memory for errored key value
      // on error reset all already processed indexes in their original state
      // (IndexRankProcessed heap contains the Index ranks added)
      // An additional error during index rollback will be put on exception stack

      _rollbackIndexes ( IndexItemList); // do not care about ZStatus : exception will be on stack

      // on error reset ZMF in its original state
      _Base::_remove_Rollback(pZMFRank); // do not accept update on Master File and free resources

      //                if (HistoryOn)
      //                      ZMFHistory.push (ZO_Add,wi,wAddress,wSt); //! journalize Error on index if journaling is enabled

      // _Base::_unlockFile (ZDescriptor) ; // unlock done in remove rollback

      goto _removeByRank_return ;    // and send appropriate status
    }

    wIndexItem->fromFileKey(wKeyContent);
    wIndexItem->IndexRank = wIdxRank;
    IndexItemList.push(wIndexItem);

    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);
  }// main for

  // so far everything when well
  //     commit changes

  wSt=_commitIndexes (IndexItemList) ;
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
    _rollbackIndexes (IndexItemList);    // indexes are already committed so use hardRollback to counter pass
    goto _removeByRank_return ;
  }

  if ((wSt==ZS_SUCCESS) && getJournalingStatus()) {
    IndexItemList._exportAppend(wEffectiveRecord);
    ZJCB->Journal->enqueue(ZJOP_RemoveByRank,wEffectiveRecord);
  }

_removeByRank_return :
  //  _Base::_unlockFile () ;

  IndexItemList.clear();
  return  wSt;
}// _removeByRankRaw


/** @cond Development */

ZStatus
ZRawMasterFile::_commitIndexes (ZArray <ZIndexItem*>  &pIndexItemList)
{
  ZStatus wSt;
  long wj = 0;
  wj=0;

  for (wj=0;wj<pIndexItemList.size();wj++)
  {
    wSt=IndexTable[wj]->_rawKeyValue_Commit(pIndexItemList[wj]);
    if (wSt!=ZS_SUCCESS){
      _rollbackIndexes(pIndexItemList);
      return wSt;
    }//wSt
  } // for
  return  ZS_SUCCESS;
} // _commitIndexes

/**
 * @brief ZRawMasterFile::_add_RollbackIndexes During an add operation, soft rollback indexes
 *      using a ZArray containing index rank per index processed
 *      Soft rollback means : free memory resources in Pools that have been allocated (ZBAT) and locked with status ZBS_Allocated
 *      Opposed to hard rollback : in this case, operation must be done onto the file itself.
 *
 * @param[in] pZMCB ZMasterControlBlock containing indexes definitions
 * @param[in] pIndexRankProcessed rank of this array gives the Index rank - content gives the Index Rank processed within the Index
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::_rollbackIndexes ( ZArray <ZIndexItem*>  &pIndexItemList)
{
  ZStatus wSt;
  long wi = 0;
  ZOp_type wOp;


  for (wi=0;wi < pIndexItemList.size();wi++)
  {
    if (pIndexItemList[wi]->Operation & ZO_Processed)
      IndexTable[wi]->_rawKeyValue_HardRollback(pIndexItemList[wi]);
    else
      IndexTable[wi]->_rawKeyValue_Rollback(pIndexItemList[wi]); // rollback add on each index concerned
  }//for                                                                  // don't care about ZStatus: exception stack will track

  return  ZS_SUCCESS;
} // _rollbackIndexes




/** @endcond */
//----------------End Add sequence------------------------------------------------------
#ifdef __DEPRECATED__
/**
 * @brief ZRawMasterFile::zindexRebuild Rebuilds an index from scratch for a ZRawMasterFile
 * @warning ZRawMasterFile must be opened in mode ZRF_Exclusive | ZRF_All before calling this method. If not, the operation will be rejected with an error status of ZS_INVOP.
 *          ZException will be setup accordingly.
 *
 * @param pIndexRank    rank of the index for the ZRawMasterFile (ZMasterControlBlock)
 * @param pStat         option to request for statistical report (true: yes , false:no)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zindexRebuild (const long pIndexRank,bool pStat, FILE *pOutput)
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
template <class _Tp>
ZStatus
ZRawMasterFile::zremoveByRank    (_Tp& pContent, const zrank_type pZMFRank)
{
    return _removeByRankRaw (RawRecord,pZMFRank);
} // zremoveByRank
template <class _Tp>
ZStatus
ZRawMasterFile::zremoveByRankR     (_Tp& pContent,const zrank_type pZMFRank)
{
    ZStatus wSt= _removeByRankRaw (RawRecord,pZMFRank);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    pRecordContent=RawRecord->Content;
    return wSt;
} // zremoveByRankR





/**
 * @brief ZRawMasterFile::_remove_CommitIndexes For All ZIndexFile associated to current ZRawMasterFile commits the remove operation
 *
 *
 * @param[in] pZMCB             ZMasterControlBlock owning indexes definitions
 * @param[in] pIndexItemList        List of pointers to ZIndexItem to be removed (Index Key contents)- Necessary in case of Hard Rollback.
 * @param[in] pIndexRankProcessed   List of ranks for ZIndexFile to be removed
 * @return  a ZStatus value. ZException is set appropriately with error message content in case of error.
 */
ZStatus
ZRawMasterFile::_remove_CommitIndexes (ZIndexItemList & pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed)
{
ZStatus wSt;
long wj = 0;

//    ZException.getLastStatus() =ZS_SUCCESS;
    for (wj=0;wj<pIndexRankProcessed.size();wj++)
            {
            wSt=IndexTable[wj]->_rawKeyValue_Commit(pIndexItemList[wj]);
            if (wSt!=ZS_SUCCESS)
                {
//  hard roll back already processed indexes
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to pIndexTableObjects.size() soft rollback

                for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
                {
                IndexTable[wR]->_rawKeyValue_HardRollback(pIndexItemList[wR] ,pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                        // regardless ZStatus (exception is on stack)
                } // for

                for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
                {
                IndexTable[wR]->_rawKeyValue_Rollback(pIndexItemList[wR] ,pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                    // regardless ZStatus (exception is on stack)
                }// for

            return  ZException.getLastStatus(); // return  the very last status encountered
            }//wSt
    } // Main for

   return  ZException.getLastStatus();
} // _remove_CommitIndexes


ZStatus
ZRawMasterFile::_remove_RollbackIndexes (ZArray<ZIndexItem*> &pIndexItemList,ZArray<zrank_type> &pIndexRankProcessed)
{


ZStatus wSt;
long wi = 0;


//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_remove_RollbackIndexes Soft Rollback of indexes on remove\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            IndexTable[wi]->_rawKeyValue_Rollback(pIndexItemList[wi],pIndexRankProcessed[wi]);  // rollback remove on each index concerned
            }// for                                                                     // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();
} // _remove_RollbackIndexes


ZStatus
ZRawMasterFile::_remove_HardRollbackIndexes (ZArray<ZIndexItem*> &pIndexItemList,
                                           ZArray<zrank_type> &pIndexRankProcessed)
{
long wi = 0;

    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on remove operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            IndexTable[wi]->_rawKeyValue_HardRollback(pIndexItemList[wi],pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            } // for                                                    // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();
} // _remove_HardRollbackIndexes

//----------------End Remove sequence------------------------------------------------------
#endif// __DEPRECATED__
#ifdef __DEPRECATED__
/**
 * @brief ZRawMasterFile::zsearch searches for a single key value using a ZKey Object
 *
 * if key has duplicates, the first found value according search algorithm is given
 *
 * @param pRecord
 * @param pKey
 * @return
 */
ZStatus
ZRawMasterFile::zsearch(ZDataBuffer &pRecord,ZKey *pKey)
{

    return (zsearch(pRecord,(ZDataBuffer &)*pKey,pKey->IndexNumber));
}
#endif
/**
 * @brief ZRawMasterFile::zsearch searches for a single key value using a ZDataBuffer containing key value to search
 * Key to search for has to be previously formatted to index internal format using appropriate routines
 * @copydetail ZIndexFile::_search()
 *
 * @param[out] pRecord
 * @param[in] pKeyValue
 * @param[in] pIndexNumber
 * @return
 */
ZStatus
ZRawMasterFile::zsearch (ZDataBuffer &pRecord,ZDataBuffer &pKeyValue,const long pIndexNumber)
{
ZStatus wSt;
//zaddress_type wAddress;
//long wIndexRank;
ZIndexItemList  IndexItemList;      // stores keys description per index processed
ZIndexItem      wIndexItem;

  if (EngineMode==SENG_Dycho)
    wSt = IndexTable[pIndexNumber]->_URFsearchDychoUnique(pKeyValue,wIndexItem,ZLock_Nolock);
  else
    wSt = IndexTable[pIndexNumber]->_URFsearchUnique(pKeyValue,wIndexItem,ZLock_Nolock);
  if (wSt!=ZS_FOUND) { return  wSt;}

  wSt =   zgetByAddress(pRecord,wIndexItem.ZMFAddress);
  if (wSt==ZS_SUCCESS)
      return ZS_FOUND;
  return wSt;
}

ZStatus
ZRawMasterFile::_getByKey (ZDataBuffer &pRecord, ZDataBuffer &pKeyValue, const zrank_type pKeyNumber,ZIndexItem* pOutIndexItem) {
  pOutIndexItem=new ZIndexItem;
//  return IndexTable[pKeyNumber]->_URFsearchDychoUnique(pKeyValue,pOutIndexItem->IndexRank,pOutIndexItem->IndexAddress,pOutIndexItem->ZMFaddress,ZLock_Nolock);
  return IndexTable[pKeyNumber]->_URFsearchUnique(pKeyValue,*pOutIndexItem,ZLock_Nolock);
}



class ZIndexCollection;

/**
 * @brief ZRawMasterFile::zsearchAll search for all index rank using a ZDataBuffer containing key value to search.
 * Exact or Partial key search is defined by a ZMatchSize_type (pZMS) that defines wether search is exact or partial.
 *
 * Exact key value : comparizon is done on key content and key length. Both must be equal.
 * Partial key value : given key must have a length less or equal to key length
 *
 * @param[in] pKeyValue a ZDataBuffer containing the key content to search for. Key content must have been previously formatted to index internal format.
 * @param[in] pIndexNumber ZRawMasterFile index number to search
 * @param[out] pIndexCollection returned collection of index references
 * @return
 */
ZStatus
ZRawMasterFile::zsearchAll (ZDataBuffer &pKeyValue,
                          const long pIndexNumber,
                          ZIndexCollection& pIndexCollection,
                          const ZMatchSize_type pZMS)
{



    return  IndexTable[pIndexNumber]->_URFsearchAll(pKeyValue,
                                                      pIndexCollection,
                                                      pZMS);

}
#ifdef __DEPRECATED__
/**
 * @brief ZRawMasterFile::zsearchAll search for all index rank using a ZKey object. Exact or Partial key search is set by ZKey
 *
 * Exact key value : comparizon is done on key content and key length. Both must be equal.
 * Partial key value : given key must have a length less or equal to key length
 *
 * @param[in] pZKey a ZKey containing the key content to search with all necessary index references
 * @param[out] pIndexCollection returned collection of index references
 * @return
 */
ZStatus
ZRawMasterFile::zsearchAll (ZKey &pZKey,ZIndexCollection& pIndexCollection)
{


ZStatus wSt;

    pIndexCollection.reset();
    ZMatchSize_type wZSC = ZMS_MatchIndexSize;

    pIndexCollection.ZIFFile = IndexTable[pZKey.IndexNumber];// assign ZIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    if (pZKey.FPartialKey)
            wZSC=ZMS_MatchKeySize ;
    wSt = IndexTable[pZKey.IndexNumber]->_URFsearchAll(pZKey,pIndexCollection,wZSC);
    return  wSt;
}//zsearchAll

/**
 * @brief ZRawMasterFile::zsearchFirst searches for first key value (in index key order) of pKey (see @ref ZKey) and return s its corresponding ZRawMasterFile record content.
 *
 * a Collection and its context ( see @ref ZSIndexCollection and @ref ZSIndexCollectionContext )
 * is created by 'new' instruction and needs to be mentionned when further calls to ZRawMasterFile::zsearchNext() are made.
 *
 * enriched during further zsearchNext accesses - and may be obtained to use Collection facitilies. see @ref Collection.
 *
 * Collection context is created and maintained within collection. This meta data describes the context of current search.
 *
 *  zsearchFirst uses ZIndexFile::_searchFirst().
 *
 * @copydoc ZIndexFile::_searchFirst()
 *
 * @param[in] pZKey     Key object. contains key content value to find and ZRawMasterFile index number.
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
ZRawMasterFile::zsearchFirst (ZKey &pZKey,
                           ZDataBuffer& pOutRecord,
                           ZIndexCollection *pCollection)
{


ZStatus wSt;
ZIndexResult wZIR;

    ZMatchSize_type wZMS = ZMS_MatchIndexSize;


    if (pZKey.FPartialKey)
            wZMS=ZMS_MatchKeySize ;
    wSt = IndexTable[pZKey.IndexNumber]->_URFsearchFirst(pZKey,pCollection,wZIR,wZMS);
    if (wSt!=ZS_FOUND)
               { return  wSt;}// Beware return  is multiple instructions in debug mode

    wSt=zgetByAddress(pOutRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}// Beware return  is multiple instructions in debug mode
    return  ZS_FOUND;
}// zsearchFirst


/**
 * @brief ZRawMasterFile::zsearchNext
 * @copydoc ZIndexFile::_searchNext()
 *
 * @param [in] pZKey  Key object. Contains key content value to find and ZRawMasterFile index number.
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
ZRawMasterFile::zsearchNext (ZKey &pZKey,
                          ZDataBuffer & pRecord,
                          ZIndexCollection *pCollection)
{


ZStatus wSt;
ZIndexResult wZIR;

    if (pCollection==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                "Invalid collection/context : it must have been created using zsearchFirst");
        return  ZS_INVOP;
        }

    wSt = IndexTable[pZKey.IndexNumber]->_URFsearchNext(wZIR,pCollection);
    if (wSt!=ZS_FOUND)
                {return  wSt;}// Beware return  is multiple instructions in debug mode

    wSt=zgetByAddress(pRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}// Beware return  is multiple instructions in debug mode
    return  ZS_FOUND;
}// zsearchFirst

#endif // __DEPRECATED__

//----------------End Search sequence----------------------------------------------------


/**
 * @brief getRawIndex   Gets a raw index item (without Endian conversion) corresponding to is IndexNumber and its rank
 * @param pIndexItem    A ZIndexItem with returned index content
 * @param pIndexRank    Rank for index
 * @param pIndexNumber  Index number
 * @return
 */
ZStatus
ZRawMasterFile::getRawIndex(ZIndexItem &pIndexItem,const zrank_type pIndexRank,const zrank_type pKeyNumber)
{
    ZStatus wSt;
    ZDataBuffer wIndexContent;
    wSt=IndexTable[pKeyNumber]->_Base::zget(wIndexContent,pIndexRank);
    if (wSt!=ZS_SUCCESS)
                   { return  wSt;}// Beware return  is multiple instructions in debug mode
    pIndexItem.fromFileKey(wIndexContent);
    return  wSt;
}

/** @} */ // ZRawMasterFileGroup

//----------------------Reports---------------------------------------
/**
 * @brief ZRawMasterFile::ZMCBreport Reports the whole content of ZMasterControlBlock : indexes definitions and dictionaries
 */
void ZRawMasterFile::MCBreport(ZaiErrors* pErrorLog)
{
    ZMasterControlBlock::report(pErrorLog);
    return;
}

//----------------------XML Reports and stuff-----------------------------------
/** addtogroup XMLGroup
 * @{ */



utf8VaryingString ZRawMasterFile::toXml(int pLevel,bool pComment)
{
  utf8VaryingString wReturn = fmtXMLnode("file",pLevel);

  wReturn += fmtXMLint("filetype",int(ZFT_ZMasterFile),pLevel+1);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," File type is ZFT_ZMasterFile i. e. ZRawMasterFile or ZSMasterFile");
  /* first file descriptor */
  wReturn += toXml(pLevel+1,pComment);
  /* second master control block */
  wReturn += toXml(pLevel+1,pComment);


  wReturn += fmtXMLendnode("file", pLevel);
  return wReturn;
} // ZRawMasterFile:toXml


utf8VaryingString ZRawMasterFile::XmlSaveToString(bool pComment)
{
  utf8VaryingString wReturn = fmtXMLdeclaration();
  wReturn += fmtXMLnodeWithAttributes("zicm","version",__ZRF_VERSION_CHAR__,0);
  wReturn += fmtXMLnode("file",0);

  wReturn += toXml(0,pComment);

  wReturn += fmtXMLendnode("file",0);
  wReturn += fmtXMLendnode("zicm",0);
  return wReturn;
}


ZStatus ZRawMasterFile::XmlSaveToFile(uriString &pXmlFile,bool pComment)
{
  ZDataBuffer wOutContent;
  utf8VaryingString wContent = XmlSaveToString(pComment);

  wOutContent.setData(wContent.toCChar(), wContent.ByteSize);

  return pXmlFile.writeContent(wOutContent);
}// ZRawMasterFile::XmlSaveToFile


/** @} */ // group XMLGroup

uriString ZRawMasterFile::getURIIndex(long pIndexRank) {
  if (!isOpen())
    return utf8VaryingString();
  if ((pIndexRank < 0) || (pIndexRank > IndexTable.count())) {
    fprintf(stderr,"ZRawMasterFile::getURIIndex Error index count <%ld> requested index rank <%ld>.",
        IndexTable.count(),pIndexRank);
    abort();
  }
  return IndexTable[pIndexRank]->getURIIndex();
}

// ---------------------import / export function -----------------------------

/*
    <zmasterfilecontent>
        <identification>
            <creationdate> </creationdate>
            <modificationdate> </modificationdate>
        </identification>
    <record>
        <field>
            <!-- if field is present -->
            <ztype> </ztype>
            <content>
            </content>
            <!-- if field is omitted : no ztype , no content -->
        </field>
    </record>
    <record>
        ...
    </record>
    </zmasterfilecontent>
*/
ZStatus ZRawMasterFile::XmlExportOneRecord(__FILEHANDLE__ pFd,
                                           const ZDataBuffer &pRecord,
                                           int pLevel,
                                           ZBlockHeader *pBlockHeader,
                                           ZMetaDic *pMetaDic,
                                           ZaiErrors *pErrorLog)
{
    ZArray<URFField> wFieldList;
    utf8VaryingString wStr;
    size_t wSizeWritten;
    size_t wCurrentOffset=0;
    size_t wHoleValue=0;
    ZStatus wSt=rawWriteText(pFd,fmtXMLnode("record",pLevel),wSizeWritten);
    if (wSt!=ZS_SUCCESS)
        return wSt;

    if (pBlockHeader!=nullptr) {
        wStr.sprintf("Block info : state %s size %ld lock %s pid %d",
                     decode_ZBS(pBlockHeader->State),
                     pBlockHeader->BlockSize,
                     decode_ZLockMask(pBlockHeader->Lock),
                     pBlockHeader->Pid);
        wSt=rawWriteText(pFd,fmtXMLcomment(wStr,pLevel),wSizeWritten);
    }

    wSt=URFParser::rawParse(pRecord,wFieldList,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return wSt;

    for (int wi=0; wi < wFieldList.count();wi++) {
        if (pBlockHeader!=nullptr) {
            const char* wHoleNature=nullptr;
            wCurrentOffset = wFieldList[wi].FieldPtr - pRecord.Data;
            wStr.sprintf("rank %d record offset %ld size %ld",wi,wCurrentOffset,wFieldList[wi].Size);
            wSt=rawWriteText(pFd,fmtXMLcomment(wStr,pLevel+1),wSizeWritten);
            if (wi == 0) {
                if (wCurrentOffset!=0) {
                    if (wCurrentOffset < 0) {
                        wStr.sprintf ("MAJOR ERROR : Field offset is %ld bytes before record boundary.",
                                     -wCurrentOffset);
                    }
                    else {
                        wStr.sprintf ("Hole found in beginning of file with %ld bytes : reserved to Presence bit set.",
                                          wCurrentOffset);
                    }
                    wSt=rawWriteText(pFd,fmtXMLcomment(wStr,pLevel+1),wSizeWritten);
                }
            }
            else {
                wHoleValue = wFieldList[wi].FieldPtr - (wFieldList[wi-1].FieldPtr + wFieldList[wi-1].Size) ;
                if (wHoleValue!=0) {
                    wStr.sprintf ("Hole found with %ld bytes",
                                 wHoleValue);
                    wSt=rawWriteText(pFd,fmtXMLcomment(wStr,pLevel+1),wSizeWritten);
                }
            }//else
        } //  block header exists

        wSt=rawWriteText(pFd,fmtXMLnode("field",pLevel+1),wSizeWritten);
        if (wSt!=ZS_SUCCESS)
            return wSt;

        if (pMetaDic!=nullptr) {
            if (wi < pMetaDic->count()) {
                wSt=rawWriteText(pFd, fmtXMLchar("name",pMetaDic->TabConst(wi).getName(),pLevel+2),wSizeWritten);

                if (wFieldList[wi].ZType != pMetaDic->Tab(wi).ZType) {
                    wStr.sprintf(" ***Warning*** dictionary field ZType <%s> does not match with file ZType <%s>",
                                 decode_ZType(pMetaDic->TabConst(wi).ZType),
                                 decode_ZType(wFieldList[wi].ZType ));
                    wSt=rawWriteText(pFd, fmtXMLcomment(wStr,pLevel+2),wSizeWritten);
                }
            }
            else {
                wStr.sprintf("Field of rank %d is not in meta dictionary",wi);
                wSt=rawWriteText(pFd, fmtXMLcomment(wStr,pLevel+2),wSizeWritten);
            }
        }



        wSt=rawWriteText(pFd,wFieldList[wi].toXml(pLevel+2),wSizeWritten);
        if (wSt!=ZS_SUCCESS)
            return wSt;

        wSt=rawWriteText(pFd,fmtXMLendnode("field",pLevel+1),wSizeWritten);
        if (wSt!=ZS_SUCCESS)
            return wSt;

    }// for
    return rawWriteText(pFd,fmtXMLendnode("record",pLevel),wSizeWritten);
} // XmlExportOneRecord
ZStatus ZRawMasterFile::XmlExportContent(const uriString &pXmlContentFile,ZaiErrors *pErrorLog)
{
    ZArray<URFField> wFieldList;
    bool wHasBeenOpened=false;
    long wRank=0;
    __FILEHANDLE__ wFd=__FILEHANDLEINVALID__;
    utf8VaryingString wXmlString ;
    ZStatus wSt=ZS_SUCCESS;
    ZDataBuffer wZDB;

    if (!isOpen()) {
        if (pErrorLog!=nullptr)
            pErrorLog->errorLog("Master file <%s> must be open with at least mode ZRB_ALL for importing data with XmlExportContent.",getURIContent().toString());
        return ZS_FILENOTOPEN;
    }// isOpen

    uriStat wStats;
    wSt=URIContent.getStatR(wStats); /* get dates for identification section */
    if (wSt!=ZS_SUCCESS) {
        if (pErrorLog!=nullptr)
            pErrorLog->errorLog("Cannot stat master file <%s>.",getURIContent().toString());
        return wSt ;
    }
    if (_progressSetupCallBack!=nullptr)
        _progressSetupCallBack(int(getRecordCount()),"Exporting master file content");


    if (_progressCallBack!=nullptr)
        _progressCallBack(wRank,"Exporting identification");

    wXmlString=fmtXMLdeclaration();
    utf8VaryingString wCom;
    wCom.sprintf("This file has been processed by XmlExportContent on %s",ZDateFull::currentDateTime().toDMYhms().toString());
    wXmlString += fmtXMLcomment(wCom,0);

    wXmlString += fmtXMLnode("zmasterfilecontent",0);
    wXmlString += fmtXMLcomment(" identification section is there for audit purposes ",1);
    wXmlString += fmtXMLnode("identification",1);
    wXmlString += fmtXMLchar("source",getURIContent(),2);
    wXmlString += fmtXMLdatefull("creationdate",wStats.Created,2);
    fmtXMLaddInlineComment(wXmlString," effective data creation date ");
    wXmlString += fmtXMLdatefull("modificationdate",wStats.LastModified,2);
    fmtXMLaddInlineComment(wXmlString," effective data last modification date ");
    wXmlString += fmtXMLendnode("identification",1);

    wSt=rawOpenCreate(  wFd,pXmlContentFile.toCChar());
    if (wSt!=ZS_SUCCESS) {
        if (wHasBeenOpened)
            zclose();
        return wSt;
    }
    size_t wSizeWritten=0;
    wSt=rawWriteText(wFd,wXmlString,wSizeWritten);
    if (wSt!=ZS_SUCCESS) {
        goto ErrorExportContent;
    }

    if (_progressCallBack!=nullptr)
        _progressCallBack(wRank,"Exporting file records");

    if (Dictionary!=nullptr) {
        utf8VaryingString wStr;
        wStr.sprintf(" A Dictionary named <%s> has been defined for this file.\n",Dictionary->DicName.toString());
        wSt=rawWriteText(wFd,fmtXMLcomment(wStr.toCChar(),1),wSizeWritten);
    }
    else
         wSt=rawWriteText(wFd,fmtXMLcomment(" No Dictionary has been defined for this file ",1),wSizeWritten);

    wSt=zget(wZDB,wRank);
    while (wSt==ZS_SUCCESS) {

        if (_progressCallBack!=nullptr)
            _progressCallBack(wRank,utf8VaryingString());  /* with null string */

//        sleepTimes(1);
        wSt=XmlExportOneRecord(wFd,wZDB,1,nullptr,Dictionary,pErrorLog);


        if (wSt!=ZS_SUCCESS)
            return wSt;
        wSt=zget(wZDB,++wRank);
    }// while ZS_SUCCESS

    if (wSt!=ZS_SUCCESS) {
        if (wSt==ZS_OUTBOUNDHIGH)
            wSt=ZS_EOF;
        else
            goto ErrorExportContent;
    }
    wSt=rawWriteText(wFd,fmtXMLendnode("zmasterfilecontent",0),wSizeWritten);



    if (_progressCallBack!=nullptr)
        _progressCallBack(wRank,"End process");
EndExportContent :
    rawClose(wFd);
    if (wHasBeenOpened)
        zclose();
    return wSt;
ErrorExportContent :
    if (wSt==ZS_EOF) {
        wSt=ZS_SUCCESS;
        if (_progressCallBack!=nullptr)
            _progressCallBack(wRank,"End process");
        goto EndExportContent;
    }
    if (_progressCallBack!=nullptr)
        _progressCallBack(wRank,"Errored process");
    goto EndExportContent;

} //exportContent

ZStatus
ZRawMasterFile::XmlExportContentFromSurfaceScan(const uriString& pURIContentFile,
                                                const uriString& pXmlContentFile,
                                                __progressCallBack__(_progressCallBack),
                                                __progressSetupCallBack__(_progressSetupCallBack),
                                                ZaiErrors* pErrorLog)
{
    ZArray<URFField> wFieldList;
    ZBlockDescriptor wBlockDescriptor;
    ZDataBuffer wRecord;
    size_t wProgress=0;
    size_t wSizeWritten=0;
    int     wBlockCount=0;
    size_t  wTotalBlockSize=0, wTotalUserSize=0,wUserSize , wTotalFreeSize=0 , wTotalDeletedSize=0 ;
    zaddress_type wCurrentAddress=0, wNextAddress=0,wPhysicalSize=0;

    __FILEHANDLE__ wFdXml=__FILEHANDLEINVALID__,wFdContent=__FILEHANDLEINVALID__;

    utf8VaryingString wXmlString ;
    ZStatus wSt=ZS_SUCCESS;

    uriStat wStats;
    wSt=pURIContentFile.getStatR(wStats); /* get dates for identification section */
    if (wSt!=ZS_SUCCESS) {
        if (pErrorLog!=nullptr)
            pErrorLog->errorLog("Cannot stat master file <%s>.",pURIContentFile.toString());
        return wSt ;
    }

    wSt = rawOpenRead(wFdContent,pURIContentFile);
    if (wSt!=ZS_SUCCESS) {
        return wSt ;
    }

    wSt = rawOpenCreate(wFdXml,pXmlContentFile);
    if (wSt!=ZS_SUCCESS) {

        return wSt ;
    }

    if (_progressSetupCallBack!=nullptr)
        _progressSetupCallBack(int(wStats.Size),"Read byte size");

    if (_progressCallBack!=nullptr)
        _progressCallBack(0,"Exporting identification");

    if (pErrorLog!=nullptr)
        pErrorLog->textLog("Exporting identification");


    wXmlString=fmtXMLdeclaration();

    utf8VaryingString wCom;
    wCom.sprintf("This file has been processed by XmlExportContentFromSurfaceScan on %s",ZDateFull::currentDateTime().toDMYhms().toString());
    wXmlString += fmtXMLcomment(wCom,0);


    wXmlString += fmtXMLnode("zmasterfilecontent",0);
    wXmlString += fmtXMLcomment(" identification section is there for audit purposes ",1);
    wXmlString += fmtXMLnode("identification",1);
    wXmlString += fmtXMLchar("source",pURIContentFile,2);
    wXmlString += fmtXMLdatefull("creationdate",wStats.Created,2);
    fmtXMLaddInlineComment(wXmlString," effective data creation date ");
    wXmlString += fmtXMLdatefull("modificationdate",wStats.LastModified,2);
    fmtXMLaddInlineComment(wXmlString," effective data last modification date ");
    wXmlString += fmtXMLendnode("identification",1);

    wSt=rawWriteText(wFdXml,wXmlString,wSizeWritten);
    if (wSt!=ZS_SUCCESS) {
        goto ErrorExportContentFSS;
    }

    if (pErrorLog!=nullptr)
        pErrorLog->textLog("Exporting identification done");

    wProgress=100;
    if (_progressCallBack!=nullptr)
        _progressCallBack(wProgress,utf8VaryingString());


    if (_progressCallBack!=nullptr)
        _progressCallBack(wProgress,"Exporting file records");

    if (pErrorLog!=nullptr)
        pErrorLog->textLog("Exporting file records");

    wSt=rawSearchNextStartSign(wFdContent,wStats.Size,-1,wCurrentAddress,wCurrentAddress);
    if (wSt!=ZS_SUCCESS)
        goto ErrorExportContentFSS;

    /* search next block to test block size validity */
    wSt=rawSearchNextStartSign(wFdContent,wStats.Size,-1,wCurrentAddress+sizeof(cst_ZFILEBLOCKSTART),wNextAddress);
    if (wSt!=ZS_SUCCESS) { /* ZS_READPARTIAL is not returned : considered as valid access if startsign is found */
        if (wSt==ZS_EOF)
            wNextAddress = wStats.Size;
        else
            goto ErrorExportContentFSS;
    }

    while (wSt==ZS_SUCCESS) {
        wProgress = wCurrentAddress ;
        if (_progressCallBack!=nullptr)
            _progressCallBack(wProgress,utf8VaryingString());  /* with null string */

        wSt=rawGetBlockDescriptor(wFdContent,wBlockDescriptor,wCurrentAddress);
        if (wSt!=ZS_SUCCESS) {
            if (pErrorLog!=nullptr)
                pErrorLog->logZExceptionLast("ZRawMasterFile::XmlExportContentFromSurfaceScan");
            goto ErrorExportContentFSS;
        }
        wPhysicalSize = wNextAddress-wCurrentAddress;
        if (pErrorLog!=nullptr) {
            pErrorLog->textLog("Getting #%d size %ld state %s lock %s pid %d address %ld next %ld physical size %ld %s",
                               ++wBlockCount,wBlockDescriptor.BlockSize,decode_ZBS(wBlockDescriptor.State),
                               decode_ZLockMask(wBlockDescriptor.Lock).toString(),wBlockDescriptor.Pid,
                               wCurrentAddress,wNextAddress,wPhysicalSize, wBlockDescriptor.BlockSize <= wPhysicalSize?"Size is OK":"Invalid block size");
        }

        wTotalBlockSize += wBlockDescriptor.BlockSize;
        if (wBlockDescriptor.State==ZBS_Free)
            wTotalFreeSize += wBlockDescriptor.BlockSize ;
        else if (wBlockDescriptor.State==ZBS_Deleted)
                wTotalDeletedSize += wBlockDescriptor.BlockSize ;
        else
            wTotalUserSize += wUserSize = wBlockDescriptor.BlockSize - sizeof(ZBlockDescriptor_Export);
         wBlockCount++;

        wSt=rawRead(wFdContent,wRecord,wUserSize);
        if (wSt!=ZS_SUCCESS) {
            if (pErrorLog!=nullptr)
                pErrorLog->logZExceptionLast("ZRawMasterFile::XmlExportContentFromSurfaceScan");
            goto ErrorExportContentFSS;
        }
//        sleepTimes(1);
        wSt=XmlExportOneRecord (wFdXml,wRecord,1,&wBlockDescriptor,nullptr,pErrorLog);
        if (wSt!=ZS_SUCCESS) {
            goto ErrorExportContentFSS;
        }
        wCurrentAddress = wNextAddress;
        wSt=rawSearchNextStartSign(wFdContent,wStats.Size,-1,wCurrentAddress + sizeof(cst_ZFILEBLOCKSTART) ,wNextAddress);
    } // while

    wSt=rawWriteText(wFdXml,fmtXMLendnode("zmasterfilecontent",0),wSizeWritten);

    wProgress = wCurrentAddress ;

    if (_progressCallBack!=nullptr)
        _progressCallBack(wProgress,"End process");

EndExportContentFSS:

    pErrorLog->textLog("___________Export from surface scan Report___________\n"
                       "Overall file size                   %ld\n"
                       "Total block volume read             %ld\n"
                       "User volume                         %ld\n"
                       "Free volume                         %ld\n"
                       "Deleted volume                      %ld\n"
                       "Number of blocks found              %ld\n"
                       "Mean used block size                %ld\n"
                       "Last address                        %ld\n"
                       "Overall content space usage (1)     %g %\n"
                       "Used content space usage (2)        %g %\n"
                       "Free space vs used size (3)         %g %\n"
 //                      "Available space vs used size (4)    %g %\n"
                       "(1) Percentage of user used space vs total file size.\n"
                       "(2) Percentage of user used space vs total of used blocks size.\n"
                       "(3) Percentage of free space vs total of used blocks size.\n"
                       "(4) Percentage of free and deleted space vs total of used blocks size.\n",
                       wStats.Size,
                       wTotalBlockSize,
                       wTotalUserSize,
                       wTotalFreeSize,
                       wTotalDeletedSize,
                       wBlockCount,
                       wTotalUserSize / wBlockCount,
                       wCurrentAddress,
                       double(wTotalUserSize) * 100.0 / double(wStats.Size),
                       double(wTotalUserSize) * 100.0 / double(wTotalBlockSize),
                       double(wTotalFreeSize+wTotalDeletedSize) * 100.0 / double(wTotalUserSize)
                       );


    rawClose(wFdContent);
    rawClose(wFdXml);
    return wSt;
ErrorExportContentFSS:
    if (wSt==ZS_EOF) {
        if (_progressCallBack!=nullptr)
            _progressCallBack(wProgress,"End process");
        goto EndExportContentFSS;
    }
    pErrorLog->logZExceptionLast("XmlExportContentFromSurfaceScan");
    pErrorLog->errorLog("Export from surface scan stopped at address %ld",wCurrentAddress);
    goto EndExportContentFSS;
} //XmlExportContentFromSurfaceScan


/*  File must exist and be opened in mode ZRF_All. File is left open when returning.
 *  _progressSetupCallBack  and _progressCallBack must be set to appropriate routine
*/

ZStatus
ZRawMasterFile::XmlImportContentByChunk( const uriString& pXmlContentFile,ZEXOP_Type pOption,ZaiErrors* pErrorLog)
{
  if (!pXmlContentFile.exists()) {
    pErrorLog->errorLog("Xml file <%s> does not exist",pXmlContentFile.toString());
    return ZS_FILENOTEXIST;
  }
  int wRecordsRead=0,wRecordsWritten=0;
  utf8VaryingString wXmlRecordContent;
 // utf8VaryingString wXmlFullRecordContent;
  zmode_type wOpenMode=0;

  uriStat wStats;
  pXmlContentFile.getStatR(wStats);

  ZMFIdentification wIdentification;
  uriString wXmlRecord = "xmlrecordcontent.xml";

  size_t wOffset=0;
 // zxmlDoc     *wDoc = nullptr;
 // zxmlElement *wRoot = nullptr;

  ZDataBuffer wRecordToWrite;
//  bool wHasBeenOpened=false;
  long wRank=0;

  __FILEHANDLE__ wFd=__FILEHANDLEINVALID__;
  utf8VaryingString wXmlString , wXmlComment;
  ZStatus wSt=ZS_SUCCESS;
  bool wEof=false;
  ZDataBuffer wZDB;
  if (!isOpen()) {
    if (pErrorLog!=nullptr)
        pErrorLog->errorLog("Master file <%s> must be open for importing datausing XmlImportContentByChunk.",getURIContent().toString());
    else
        fprintf(stderr,"Master file <%s> must be open for importing data using XmlImportContentByChunk.\n",getURIContent().toCChar());

    return ZS_FILENOTOPEN;
  }// isOpen
  wOpenMode = getOpenMode();
  if (wOpenMode!=ZRF_All) {
    if (pErrorLog!=nullptr)
        pErrorLog->errorLog("Master file <%s> must be open with mode ZRB_ALL for importing data using XmlImportContentByChunk.",getURIContent().toString());
    else
        fprintf(stderr,"Master file <%s> must be open with mode ZRB_ALL for importing data using XmlImportContentByChunk.\n",getURIContent().toCChar());
  }


  wSt = rawOpen(wFd,pXmlContentFile,O_RDONLY);
  if (wSt!=ZS_SUCCESS) {
    if (pErrorLog!=nullptr)
         pErrorLog->errorLog("Cannot open xml output file <%s> mode ZRF_All status is <%s>",
                             pXmlContentFile.toString(),
                             decode_ZStatus(wSt));
    else
        fprintf(stderr,"Master file <%s> must be open with mode ZRB_ALL for importing data using XmlImportContentByChunk.\n",
                 "Cannot open xml output file <%s> mode ZRF_All status is <%s>\n",
                 pXmlContentFile.toCChar(),
                 decode_ZStatus(wSt));

 //   zclose();
    return  wSt;
  }
  { // specific code segment to delete local variables later on
      utf8VaryingString wMsg;
      while (true) {
      if (pOption==ZEXOP_Nothing) {
          wMsg="No option";
          break;
      }
      if (pOption & ZEXOP_CheckName) {
          wMsg="Check Names";
      }
      if (pOption & ZEXOP_CheckZType) {
          wMsg.addConditionalOR("Check ZType");
          break;
      }
      break;
      } // while true
      if (pErrorLog!=nullptr)
          pErrorLog->infoLog("Importing options  <%s>",
                              wMsg.toString());
      else
          fprintf(stdout,"Importing options  <%s>\n",
                  wMsg.toCChar());

      if (Dictionary==nullptr) {
          if (pErrorLog!=nullptr)
              pErrorLog->errorLog(" File dictionary does not exist. This will prevent to use mentionned options.");
          else
              fprintf(stderr," File dictionary does not exist. This will prevent to use mentionned options.\n");
      }
  } // end code segment

  if (_progressSetupCallBack!=nullptr)
    _progressSetupCallBack(int(wStats.Size),"Importing master file content");

  if (_progressCallBack!=nullptr)
    _progressCallBack(0,"Importing identification");

  /* identification part */

  wSt=XMLLoadEntity(wFd,"identification",wXmlRecordContent,wOffset);
  if (wSt!=ZS_SUCCESS)
    goto XmlImportContentByChunkErrored ;

  wSt = XmlImportIdentification(wXmlRecordContent,wIdentification,pErrorLog);
  if (wSt!=ZS_SUCCESS)
    goto XmlImportContentByChunkErrored ;
  if (pErrorLog!=nullptr)
      pErrorLog->infoLog("Data identification section : creation date %s modification date %s",
                         wIdentification.CreationDate.toDMYhms().toString(),
                         wIdentification.ModificationDate.toDMYhms().toString());

  _progressCallBack(wOffset,"identification imported");
//  sleepTimes(1);

  wSt=XMLLoadEntity(wFd,"record",wXmlRecordContent,wOffset);
  if ((wSt!=ZS_SUCCESS)&&(wSt!=ZS_EOF))
    goto XmlImportContentByChunkErrored ;


  if (_progressCallBack!=nullptr)
    _progressCallBack(wOffset,"Importing file records");


  while ((wSt==ZS_SUCCESS) || (wSt==ZS_EOF))
  {
    wRecordsRead++;
//    sleepTimes(1);
    if (_progressCallBack!=nullptr)
        _progressCallBack(wOffset,utf8VaryingString());

  // below is already done by XMLLoadEntity
//    wXmlFullRecordContent = fmtXMLdeclaration(); /* add xml declaration to make a valid xml document from the fragment */
//    wXmlFullRecordContent += wXmlRecordContent ;
    wSt=wXmlRecord.writeContent(wXmlRecordContent);  /* just to debug-view content */
    wRecordsWritten++;
    wSt = XmlImportRecordContent(wXmlRecordContent,wRecordToWrite,pOption,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        goto XmlImportContentByChunkErrored ;

    wSt = _addRawDisregardKeys(wRecordToWrite);
    if (wSt!=ZS_SUCCESS)
        goto XmlImportContentByChunkErrored ;
    if (wEof)
        break;
    wSt=XMLLoadEntity(wFd,"record",wXmlRecordContent,wOffset);
    if (wSt==ZS_EOF)
        wEof=true;
    }// while
  if (wSt!=ZS_EOF)
    goto XmlImportContentByChunkErrored ;

XmlImportContentByChunkEnd:
  if (_progressCallBack!=nullptr)
    _progressCallBack(wStats.Size,utf8VaryingString());
 /* if (wOpenMode==0)
    zclose();
*/
XmlImportContentByChunkEnd_1:
  rawClose(wFd);

  pErrorLog->textLog("Record import report\n"
                     "Imported data creation date    %s\n"
                     "              Last modified    %s\n"
                     "records from Xml          %d\n"
                     "records written to file   %d\n",
                     wIdentification.CreationDate.toDMYhms().toString(),
                     wIdentification.ModificationDate.toDMYhms().toString(),
                     wRecordsRead,wRecordsWritten);

  if (wSt==ZS_EOF)
      wSt=ZS_SUCCESS;
  return wSt;
XmlImportContentByChunkErrored:
  if (wSt==ZS_EOF) {
        if (_progressCallBack!=nullptr)
            _progressCallBack(wRank,"End process");
        goto XmlImportContentByChunkEnd_1;
  }
  if (_progressCallBack!=nullptr)
    _progressCallBack(wRank,"Processing errored");
  pErrorLog->errorLog("Importing record process is errored and interrupted.");
  goto XmlImportContentByChunkEnd_1;
} // XmlImportContentByChunk

ZStatus
ZRawMasterFile::XmlImportIdentification(const utf8VaryingString& pXmlRecordContent, ZMFIdentification& pIdentification, ZaiErrors *pErrorLog)
{
  zxmlDoc     *wDoc = nullptr;
  zxmlElement *wRecordRoot = nullptr;
  zxmlElement *wFieldElt=nullptr;

  wDoc = new zxmlDoc;
  ZStatus wSt=wDoc->XmlParseFromMemory(pXmlRecordContent,pErrorLog);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  /*
  ZStatus wSt = wDoc->ParseXMLDocFromMemory(pXmlRecordContent.toCChar(), pXmlRecordContent.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    if (pErrorLog!=nullptr) {
        pErrorLog->logZExceptionLast();
        pErrorLog->errorLog(
            "XmlImportIdentification-E-PARSERR Xml parsing error for string <%s> ",
            pXmlRecordContent.subString(0, 25).toString());
    }
    else
        fprintf(stderr,"XmlImportIdentification-E-PARSERR Xml parsing error for string <%s>",
                pXmlRecordContent.subString(0, 25).toString());
    return wSt;
  }
*/
  wSt=wDoc->getRootElement(wRecordRoot);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  if (wRecordRoot->getName()!="identification") {
    if (pErrorLog!=nullptr) {
        pErrorLog->errorLog(
            "XmlImportIdentification-E-INVROOT Invalid Xml root node <%s> while expecting <identification>" , wRecordRoot->getName());
    }
    else
        fprintf(stderr,"XmlImportIdentification-E-INVROOT Invalid Xml root node <%s> while expecting <identification>",wRecordRoot->getName());

    return ZS_XMLINVNODENAME;
  } // == zrecord

  wSt=XMLgetChildZDateFull(wRecordRoot,"creationdate",pIdentification.CreationDate,pErrorLog,ZAIES_Warning);

  wSt=XMLgetChildZDateFull(wRecordRoot,"modificationdate",pIdentification.ModificationDate,pErrorLog,ZAIES_Warning);

  XMLderegister(wRecordRoot);
  XMLderegister(wDoc);
  return wSt;
}



ZStatus
ZRawMasterFile::XmlImportRecordContent(const utf8VaryingString& pXmlRecordContent, ZDataBuffer& pRecord, ZEXOP_Type pOption,ZaiErrors *pErrorLog)
{
  zxmlDoc     *wDoc = nullptr;
  zxmlElement *wRecordRoot = nullptr;
  zxmlElement *wFieldElt=nullptr;

  pRecord.clear();

  wDoc = new zxmlDoc;

  ZStatus wSt=wDoc->XmlParseFromMemory(pXmlRecordContent,pErrorLog);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wSt=wDoc->getRootElement(wRecordRoot);

  if (wRecordRoot->getName()!="record") {
    if (pErrorLog!=nullptr) {
        pErrorLog->errorLog(
            "XmlImportRecordContent-E-INVROOT Invalid Xml root node <%s> while expecting <record>" , wRecordRoot->getName());
    }
    else
        fprintf(stderr,"XmlImportRecordContent-E-INVROOT Invalid Xml root node <%s> while expecting <record>",wRecordRoot->getName());

    return ZS_XMLINVNODENAME;
  } // == zrecord


  ZArray<URFField> wFieldList;
  URFField wField;

  zxmlNode* wFieldNode=nullptr;
  ZDataBuffer wRecordZDB;
  ZDataBuffer wFieldZDB;

  int wCount=0;
  wSt=wRecordRoot->getFirstChild(wFieldNode);

  if (pErrorLog==nullptr)
      pErrorLog = &ErrorLog;

  while (wSt==ZS_SUCCESS) {
    /* get one field */
    wSt = wField.fromXml((zxmlElement*)wFieldNode, wFieldZDB,pErrorLog);
    if (wSt!=ZS_SUCCESS)
        return wSt;

    /* dictionary integrity check */

    if  (Dictionary != nullptr) {
        if (pOption & ZEXOP_CheckName) {
            if (!wField.Name.isEmpty()) {
                if (wField.Name != Dictionary->Tab(wCount).getName()) {
                    pErrorLog->errorLog("ZRawMasterFile::XmlImportRecordContent-E-INVNAM Read field name <%s> does not match dictionary name <%s>",
                                       wField.Name.toString(),
                                       Dictionary->Tab(wCount).getName().toString());
                }
            }
            else
                pErrorLog->warningLog("ZRawMasterFile::XmlImportRecordContent-E-EMPTYNAM Read file name is empty and cannot be compared to dictionary name <%s>",
                                    Dictionary->Tab(wCount).getName().toString());
        } //ZEXOP_CheckName

        if (pOption & ZEXOP_CheckZType) {
            if (Dictionary->Tab(wCount).ZType != wField.ZType) {
                pErrorLog->errorLog("ZRawMasterFile::XmlImportRecordContent-E-INVTYP Read field ZType <%X %s> does not match dictionary ZType <%X %s>",
                                   wField.ZType, decode_ZType(wField.ZType),
                                   Dictionary->Tab(wCount).ZType,decode_ZType(Dictionary->Tab(wCount).ZType) );
            }
        }// ZEXOP_CheckZType
    } // Dictionary != nullptr)

    wCount++ ;
    wRecordZDB.appendData(wFieldZDB);
    wFieldList.push(wField);

    XMLderegister(wFieldNode);
    wSt=wRecordRoot->getNextNode(wFieldNode);
  } // while
  /* Define field presence : a field is present if at least ZType node has been defined for this field */

  ZBitset wPresence(uint16_t(wFieldList.count()));
  wPresence.clear();
  for (int wi=0; wi < wFieldList.count();wi++) {
    if (wFieldList[wi].Present)
        wPresence.set(wi);
  }

  /* final formatting of URF record with leading presence bitset */
  wPresence._exportURF(pRecord);
  pRecord.appendData(wRecordZDB);

  XMLderegister(wRecordRoot);
  XMLderegister(wDoc);
  return ZS_SUCCESS;
} // XmlImportRecordContent

/**
@addtogroup ZMFSTATS ZRawMasterFile and ZIndexFile storage statistics and PMS session monitoring
//-------------------Statistical functions-----------------------------------------
@{
*/

/**
 * @brief ZRawMasterFile::ZRFPMSIndexStats gives storage statistical data for an Index given by its rank
 *
 *
 * @param pIndex    Index rank
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZRawMasterFile::ZRFPMSIndexStats(const long pIndex,FILE* pOutput)
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
* @brief ZRawMasterFile::zstartPMSMonitoring Starts PMS monitoring session for current ZRawMasterFile
* @note collected data concerns only ZRawMasterFile and NOT indexes
*   @ref  ZRawMasterFile::zstartIndexPMSMonitoring
*   @ref  ZRawMasterFile::zendIndexPMSMonitoring
*   @ref  ZRawMasterFile::zreportIndexPMSMonitoring
*/
 void
 ZRawMasterFile::zstartPMSMonitoring (void)
 {
     ZPMSStats.init();
 }
/**
* @brief ZRawMasterFile::zendIndexPMSMonitoring Ends current PMS monitoring session on Index given by its rank (pIndex)
* @note collected data concerns only ZRawMasterFile and NOT indexes
*   @ref  ZRawMasterFile::zstartIndexPMSMonitoring
*   @ref  ZRawMasterFile::zendIndexPMSMonitoring
*   @ref  ZRawMasterFile::zreportIndexPMSMonitoring
*/
  void
  ZRawMasterFile::zendPMSMonitoring (void)
  {
      ZPMSStats.end();
  }
/**
* @brief ZRawMasterFile::zreportPMSMonitoring Fully reports current PMS monitoring session on current ZRawMasterFile
* This report lists collected data :
*   - timer data
*   - ZRandomFile PMS data
* @note collected data concerns only ZRawMasterFile and NOT indexes
*   @ref  ZRawMasterFile::zstartIndexPMSMonitoring
*   @ref  ZRawMasterFile::zendIndexPMSMonitoring
*   @ref  ZRawMasterFile::zreportIndexPMSMonitoring
*
* @param pIndex rank of the index to monitor
* @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
*/
   void
   ZRawMasterFile::zreportPMSMonitoring (ZaiErrors* pErrorLog)
   {
       pErrorLog->infoLog(
               "________________________________________________\n"
               "   ZRawMasterFile <%s>\n"
               "   File open mode         %s\n"
               "________________________________________________\n",
               URIContent.toString(),
               decode_ZRFMode( Mode)
               );
       ZPMSStats.reportFull(pErrorLog);
       return ;
   }



/**
  * @brief ZRawMasterFile::zstartIndexPMSMonitoring Starts PMS monitoring session on the specific index given by its rank (pIndex)
  * @param pIndex rank of the index to monitor
  */
 ZStatus
 ZRawMasterFile::zstartIndexPMSMonitoring (const long pIndex)
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
   * @brief ZRawMasterFile::zendIndexPMSMonitoring Ends current PMS monitoring session on the specific index given by its rank (pIndex)
   * @param pIndex rank of the index to monitor
   */
ZStatus
ZRawMasterFile::zstopIndexPMSMonitoring(const long pIndex)
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
* @brief ZRawMasterFile::zreportIndexPMSMonitoring Fully reports current PMS monitoring session on the specific index given by its rank (pIndex)
* @param pIndex rank of the index to monitor
* @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
*/
ZStatus
ZRawMasterFile::zreportIndexPMSMonitoring (const long pIndex, FILE* pOutput)
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
           "   ZRawMasterFile <%s>\n"
           "   Index rank <%ld> <%s>\n"
           "   File open mode    %s\n",
           URIContent.toString(),
           pIndex,
           IndexTable[pIndex]->IndexName.toCChar(),
           decode_ZRFMode( Mode));
   IndexTable[pIndex]->ZPMSStats.reportFull(pOutput);
   return  ZS_SUCCESS;
}// zreportIndexPMSMonitoring


ZStatus ZRawMasterFile::createDictionary(const ZMFDictionary& pDic) {
  setTypeMasterFile();
  uriString wURIdic = ZDictionaryFile::generateDicFileName(getURIContent());
  Dictionary = new ZDictionaryFile;
  Dictionary->setDictionary(pDic);
  return Dictionary->saveToDicFile(wURIdic);
}


ZStatus ZRawMasterFile::createExternalDictionary(const uriString& pDicPath)
{
  if (!pDicPath.exists()) {
    ZException.setMessage("ZRawMasterFile::createExternalDictionary",ZS_FILENOTEXIST,Severity_Error,"Dictionary file %s does not exist.",pDicPath.toString());
    return ZS_FILENOTEXIST;
  }
  setTypeMasterFile();
  Dictionary = new ZDictionaryFile;
  DictionaryPath=pDicPath;
  Dictionary->URIDictionary = pDicPath;
  ZStatus wSt=Dictionary->load_xml(&ErrorLog);

  DictionaryPath = pDicPath;
  return wSt;
}

ZStatus ZRawMasterFile::zclearAll(ssize_t pSizeToKeep,bool pHighwater,ZaiErrors *pErrorLog) {

  ZStatus wSt;
  double wNbElt = double(ZBAT.count());
   ssize_t wIdxSizeToKeep=-1;

  for (long wi=0; wi < IndexTable.count();wi++) {
      if (pSizeToKeep >= 0) {
          wIdxSizeToKeep = ssize_t((wNbElt * double(IndexTable[wi]->getBlockTargetSize()))+0.5);
      }
      wSt=IndexTable[wi]->zclearFile(wIdxSizeToKeep,pHighwater,pErrorLog);
  }
  wSt=zclearFile(pSizeToKeep,getFCB()->HighwaterMarking|pHighwater,pErrorLog);
  return wSt;
}


ZStatus ZRawMasterFile::zgetPerIndex (ZDataBuffer &pRecordContent, const long pIndexRank,long pKeyRank) {
  ZIndexItem wItem;
  ZDataBuffer wKeyRecord;
  if ((pIndexRank < 0) || (pIndexRank > IndexTable.count()))
    return ZS_OUTBOUND;
  ZStatus wSt= IndexTable[pIndexRank]->zget(wKeyRecord,pKeyRank);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wItem.fromFileKey(wKeyRecord);
  return zgetByAddress(pRecordContent,wItem.ZMFAddress);
}

ZStatus ZRawMasterFile::zgetFirstPerIndex (ZDataBuffer &pRecordContent, const long pIndexRank){
  ZIndexItem wItem;
  ZDataBuffer wKeyRecord;
  if ((pIndexRank < 0) || (pIndexRank > IndexTable.count()))
    return ZS_OUTBOUND;
  ZStatus wSt= IndexTable[pIndexRank]->zgetFirst(wKeyRecord);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wItem.fromFileKey(wKeyRecord);
  return zgetByAddress(pRecordContent,wItem.ZMFAddress);
}

ZStatus ZRawMasterFile::zgetNextPerIndex (ZDataBuffer &pRecordContent, const long pIndexRank){
  ZIndexItem wItem;
  ZDataBuffer wKeyRecord;
  if ((pIndexRank < 0) || (pIndexRank > IndexTable.count()))
    return ZS_OUTBOUND;
  ZStatus wSt= IndexTable[pIndexRank]->zgetNext(wKeyRecord);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wItem.fromFileKey(wKeyRecord);
  return zgetByAddress(pRecordContent,wItem.ZMFAddress);
}



ZStatus ZRawMasterFile::zcopyTo(const uriString& pOldContentName,
                                const uriString& pNewContentName,
                                uint8_t pFlag,     // see ZCopyManip_enum (zioutils.h)
                                int pPayLoad,
                                ZaiErrors* pErrorLog)
{
    ZRawMasterFile wZMF;
    ZStatus wSt=wZMF.setPath(pOldContentName);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->logZExceptionLast("ZRawMasterFile::zcopyTo");
        return wSt;
    }
    return wZMF._copyTo(pNewContentName,pFlag,pPayLoad, pErrorLog);
}


ZStatus ZRawMasterFile::_copyTo(const uriString& pNewContentURI,
                                uint8_t pFlag,
                                int pPayLoad,  /* if -1 then defaulted to rawCopyPayLoad see setRawCopyPayLoad() (zioutils.h)*/
                                ZaiErrors* pErrorLog)
{
    ZStatus wSt = ZS_SUCCESS;
    if (!(getOpenMode()& ZRF_Read_Only)) {
        if (isOpen())
            zclose();
        wSt=zopen(ZRF_Read_Only);
        if (wSt!=ZS_SUCCESS) {
            pErrorLog->logZExceptionLast("ZRawMasterFile::getCopySize");
            return wSt;
        }
    } // ZRF_Read_Only

    if (pFlag & ZMNP_IncludeAll) {
        for (int wi=0; (wi < IndexTable.count()) && (wSt==ZS_SUCCESS);wi++) {
            uriString wIndexURI;
            __progressCallBack__(wPCB) = IndexTable[wi]->_progressCallBack;
            __progressSetupCallBack__(wPSUCB) = IndexTable[wi]->_progressSetupCallBack;
            IndexTable[wi]->registerProgressCallBack(_progressCallBack);
            IndexTable[wi]->registerProgressSetupCallBack(_progressSetupCallBack);
            wSt=generateIndexURI(wIndexURI,pNewContentURI,uriString(),IndexTable[wi]->IndexName);
            if (wSt==ZS_SUCCESS)
                wSt=IndexTable[wi]->_copyTo(wIndexURI,pFlag,pPayLoad,pErrorLog);
            IndexTable[wi]->_progressCallBack=wPCB;
            IndexTable[wi]->_progressSetupCallBack=wPSUCB;
        }// for
    } // ZMNP_IncludeAll
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->logZExceptionLast("ZRawMasterFile::_copyTo");
        return wSt;
    }

    wSt=ZRandomFile::_copyTo(pNewContentURI,pFlag,pPayLoad,pErrorLog);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->logZExceptionLast("ZRawMasterFile::_copyTo");
        return wSt;
    }
    zclose();
    return wSt;
} //  ZRawMasterFile::_copyTo

ZStatus ZRawMasterFile::getCopySize( size_t &pSize,uint8_t pFlag, ZaiErrors *pErrorLog)
{
    pSize=0;

    ZStatus wSt=ZS_SUCCESS;
    if (!(getOpenMode()& ZRF_Read_Only)) {
        if (isOpen())
            zclose();
        wSt=zopen(ZRF_Read_Only);
        if (wSt!=ZS_SUCCESS) {
            pErrorLog->logZExceptionLast("ZRawMasterFile::getCopySize");
            return wSt;
        }
    } // ZRF_Read_Only

    if (pFlag & ZMNP_IncludeAll) {
        for (int wi=0; (wi < IndexTable.count()) && (wSt==ZS_SUCCESS);wi++) {
            pSize += IndexTable[wi]->URIContent.getFileSize();
            pSize += IndexTable[wi]->URIHeader.getFileSize();
        }// for
    } // ZMNP_IncludeAll
    pSize += URIContent.getFileSize();
    pSize += URIHeader.getFileSize();

    return ZS_SUCCESS;
} //  ZRawMasterFile::getCopySize

void sleepTimes (int pTimes)
{
  while (pTimes-- > 0) {
    sleep(1);
  }
}

#endif // ZRAWMASTERFILE_CPP
