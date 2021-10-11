#ifndef ZRAWMASTERFILE_CPP
#define ZRAWMASTERFILE_CPP

#include "zrawmasterfile.h"

#include <zindexedfile/zskey.h>

#include <zrandomfile/zrfcollection.h>

#include <zindexedfile/zsjournal.h>

#include <zindexedfile/zmasterfile_utilities.h>

#include <zxml/zxmlprimitives.h>

#include <zindexedfile/zsjournalcontrolblock.h>
#include <zindexedfile/zrawrecord.h>

#include <zxml/zxmlprimitives.h>

using namespace  zbs;

ZRawMasterFile::ZRawMasterFile(ZFile_type pType) : ZRandomFile(pType)
{
  RawRecord=new ZRawRecord(this);
  ZMCB.ZJCB=nullptr;
  // ZMCB.MetaDic = nullptr;
  ZMCB.MasterDic = nullptr;

  setFileType(pType);
  return ;
}

ZRawMasterFile::ZRawMasterFile() : ZRandomFile(ZFT_ZRawMasterFile)
{
  RawRecord=new ZRawRecord(this);
  ZMCB.ZJCB=nullptr;
// ZMCB.MetaDic = nullptr;
  ZMCB.MasterDic = nullptr;
  return ;
}

/*
ZRawMasterFile::ZRawMasterFile(uriString pURI) : ZRandomFile(pURI)
{
    ZStatus wSt=setPath(pURI);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
    ZMCB.ZJCB=nullptr;
    ZMCB.MasterDic = nullptr;
    setFileType(ZFT_ZRawMasterFile);
    return;
}

*/
ZRawMasterFile:: ~ZRawMasterFile(void)
{
    if (_isOpen)
                        zclose();
     while (ZMCB.IndexTable.size() >0)
             {
             if (ZMCB.IndexTable.last()->_isOpen)
                                            ZMCB.IndexTable.last()->closeIndexFile();
             ZMCB.IndexTable.pop(); // deletes the contained object
             }


      setJournalingOff();


  if (RawRecord!=nullptr)
    delete RawRecord;

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


    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOn-I Starting journaling for file %s\n",
                     getURIContent().toString());
    if (ZMCB.ZJCB==nullptr)
            {
            ZMCB.ZJCB = new ZSJournalControlBlock;
            ZMCB.ZJCB->Journal = new ZSJournal(this);
            }
        else
    {
    if (ZMCB.ZJCB!=nullptr)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot start journaling for file %s : already started.\n",
                     getURIContent().toString());
            if (ZMCB.ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZMCB.ZJCB->Journal->getURIContent().toString());
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
    wSt=ZMCB.ZJCB->Journal->init();
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While initializing journaling");
        return  wSt;
        }

//    ZMCB.JournalingOn=true;
    ZMCB.ZJCB->JournalLocalDirectoryPath = pJournalPath;
    wSt=writeControlBlocks();

    if (wSt!=ZS_SUCCESS)
                {
                    return  wSt; // Beware return  is multiple instructions in debug mode
                }
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-JCB Journaling set on within JournalingControlBlock\n");

    if (!wasOpen)  // if was not open at routine entrance, then it has been openned locally and must be closed
                {return  zclose();}

    if (wFormerMode!=ZRF_Nothing) // if was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open with former mode
            }
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    wSt=ZMCB.ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    }
    else
        {
        ZException.addToLast("While starting journaling thread for journaling file %s",
                             ZMCB.ZJCB->Journal->getURIContent().toString());
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

    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOn-I Starting/restarting journaling on for file %s\n",
                     getURIContent().toString());
    if (ZMCB.ZJCB->Journal==nullptr)
                {
                fprintf (stderr,
                         "setJournalingOn-E Journaling has not be defined for file %s. Cannot start/restart journaling.\n",
                         getURIContent().toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVOP,
                                        Severity_Error,
                                        "Journaling has not be defined for file %s. Cannot start/restart journaling.",
                                        getURIContent().toString());
                return  ZS_INVOP;
                }

    if (ZMCB.ZJCB->Journal->JThread.getState() > ZTHS_Nothing)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot start journaling for file %s : journaling already started. Thread id is %ld\n",
                     getURIContent().toString(),
                     ZMCB.ZJCB->Journal->JThread.getId());
            if (ZMCB.ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZMCB.ZJCB->Journal->getURIContent().toString());
                        return  ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Setting journaling on : Journaling for file %s is started while journal file is marked as not open.",
                                    getURIContent().toString());
            return  ZS_FILENOTOPEN;
            }

    wSt=ZMCB.ZJCB->Journal->init(true); // Journal file must exist
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While initializing journaling");
        return  wSt;
        }


    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    wSt=ZMCB.ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    }
    else
    {
        if (ZVerbose)
            {
                fprintf(stdout,
                        "setJournalingOn-E-Failure Journaling on file %s has not started.See ZException stack dump (following) to get information\n",
                        ZMCB.ZJCB->Journal->getURIContent().toString());
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
    if (ZMCB.ZJCB->Journal==nullptr)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                " Journaling parameters have not be defined for file <%s>. Cannot start/restart journaling.",
                                getURIContent().toString());
    }

        else

//    if (ZMCB.JournalingOn)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Cannot set journaling on for file %s : journaling already started.\n",
                     getURIContent().toString());
            if (ZMCB.ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning journal file %s is already open.\n",
                                 ZMCB.ZJCB->Journal->getURIContent().toString());
                        return  ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Setting journaling on : Journaling for file %s is started while journal file is marked as not open.",
                                    getURIContent().toString());
            return  ZS_FILENOTOPEN;
            }
    wSt=ZMCB.ZJCB->Journal->init(true); // journaling file must have been created elsewhere and must exist
    if (wSt!=ZS_SUCCESS)
        {

        ZException.addToLast("While initializing journaling");
        return  wSt;
        }

/*
 //   ZMCB.JournalingOn=true;
 //   ZMCB.ZJCB->JournalLocalDirectoryPath = pJournalPath;
    wSt=writeControlBlocks();

    if (wSt!=ZS_SUCCESS)
                 {   return  wSt;}
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-MCB Journaling set on within MCB header \n");

    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- starting journaling.  journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    wSt=ZMCB.ZJCB->Journal->start();
    if (wSt==ZS_SUCCESS)
    {
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I- Journaling active with journaling file %s \n",
                    ZMCB.ZJCB->Journal->getURIContent().toString());
    }
    else
    {
        if (ZVerbose)
            {
                fprintf(stdout,
                        "setJournalingOn-E-Failure Journaling on file %s has not started.See ZException stack dump to get information\n",
                        ZMCB.ZJCB->Journal->getURIContent().toString());
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


//    if (!ZMCB.JournalingOn)
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


    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOff-I Stopping journaling for file %s\n",
                     getURIContent().toString());
    if (ZMCB.ZJCB==nullptr)
            { return  ZS_SUCCESS;}  // Beware return  is multiple instructions in debug mode
    if (ZMCB.ZJCB->Journal==nullptr)
            { return  ZS_SUCCESS;}  // Beware return  is multiple instructions in debug mode

/*    if (!ZMCB.JournalingOn)
            {
            fprintf (stderr,
                     "setJournalingOn-W Warning Journaling for file %s already stopped\n",
                     getURIContent().toString());
            if (ZMCB.ZJCB->Journal->isOpen())
                        {
                        fprintf (stderr,
                                 "setJournalingOn-W Warning Journaling stopped but journal file %s is open. closing file\n",
                                 Journal->getURIContent().toString());
                        return  Journal->zclose();
                        }
            delete Journal;
            return  ZS_SUCCESS;
            }*/

    delete ZMCB.ZJCB;  // deletes Journaling Control Block  AND ZJournal instance : send a ZJOP_Close to journal thread



//    ZMCB.JournalingOn=false;
    wSt=writeControlBlocks();
    if (wSt!=ZS_SUCCESS)
                { return  wSt;} // Beware return  is multiple instructions in debug mode
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-MCB Journaling for file %s has been set off within MCB header \n",
                    getURIContent().toCChar());


    if (!wasOpen)  // if was not open close it
            { return  zclose();} // Beware return  is multiple instructions in debug mode

    if (wFormerMode!=ZRF_Nothing) // if was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open with former mode
            }
    return  wSt;
} // setJournalingOff

ZRawRecord*
ZRawMasterFile::generateRawRecord()
{
  return new ZRawRecord (this);
}

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
 * @param[in] pPath an uriString containing the path where ZSIndexFiles will be located
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
    wSt=_Base::_open(ZRF_Exclusive|ZRF_All,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While openning for setting IndexFileDirectoryPath. File <%s>",
                               URIContent.toString());
        return  wSt;
        }
    ZMCB.IndexFilePath = pPath;

    _Base::setReservedContent(ZMCB._exportMCBAppend(wReserved));
    wSt=_Base::_writeReservedHeader(true);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot set IndexFilesDirectoryPath / cannot write Reserved header. Content file is <%s>",
                                URIContent.toString());
        }
    _Base::_close();
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
    wSt=_Base::_open(ZRF_Exclusive|ZRF_All,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While openning for setting JournalingLocalDirectoryPath. File <%s>",
                               getURIContent().toString());
        return  wSt;
        }
    ZMCB.ZJCB->JournalLocalDirectoryPath = pPath;

    _Base::setReservedContent(ZMCB._exportMCBAppend(wReserved));
    wSt=_Base::_writeReservedHeader(true);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot set IndexFilesDirectoryPath / cannot write Reserved header. Content file is <%s>",
                                getURIContent().toString());
        }
    _Base::_close();
    return  wSt;
}//setJournalingLocalDirectoryPath


//------------------File Header : updates (read - write) ZMasterControlBlock AND ZSJournalControlBlock if exists
/**
 * @brief ZRawMasterFile::zgetJCBfromHeader Accesses File header and updates JournalingControlBlock if any.
 * There is no reverse operation : writting JCB to Header is done using writing whole Control blocks to header file
 * using ZRawMasterFile::writeControlBlocks()
 *
 * @return
 */
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
/**
 * @brief ZRawMasterFile::_getJCBfromReserved updates Journaling control block if any with its content from ZReserved from Filedescriptor.
 *  ZReserved have to be up to date with an already done getReservedBlock().
 * @return
 */
ZStatus
ZRawMasterFile::_getJCBfromReserved(void)
{
    ZSMCBOwnData_Export* wMCB = (ZSMCBOwnData_Export*)ZReserved.Data;

    wMCB->reverseConditional();

    if (wMCB->JCBOffset==0)  // no journaling
                {return  ZS_SUCCESS;} // Beware return  is multiple instructions in debug mode
    if (ZMCB.ZJCB==nullptr)
                {
                ZMCB.ZJCB=new ZSJournalControlBlock;
                }
    ZDataBuffer wJCBContent;
    wJCBContent.setData(ZReserved.Data+wMCB->JCBOffset,wMCB->JCBSize);
    return  ZMCB.ZJCB->_importJCB(wJCBContent);
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

    ZMCB._exportMCBAppend(wReserved);
    _Base::setReservedContent(wReserved);
    wSt=_Base::_writeReservedHeader(true);
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
//ZDataBuffer wRawJCB;

unsigned char*  wPtrIn=nullptr;
ZArray<ZPRES>    wIndexPresence;

    wSt=_Base::getReservedBlock(wReserved,true);
    if (wSt!=ZS_SUCCESS)
            {
            return  wSt;
            }

//    wSt=ZMCB._importMCB(wReserved);

    wPtrIn=wReserved.Data;

    wSt=ZMCB._import(wPtrIn,wIndexPresence);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While importing ZMCB");
            return  wSt;
            }
    if (ZMCB.JCBOffset<1)
            {
            if (ZMCB.ZJCB!=nullptr)
                    delete ZMCB.ZJCB;
             return  wSt;
            }
    ZDataBuffer wReservedJCB;
    wReservedJCB.setData(wReserved.Data+ZMCB.JCBOffset,ZMCB.JCBSize);
    return  ZMCB.ZJCB->_importJCB(wReservedJCB);

}// readControlBlocks





/**
 * @brief ZRawMasterFile::zcreateRawIndex Generates a new index from a description (meaning a new ZRandomFile data + header).
 *
 * This routine will create a new index with the files structures necessary to hold and manage it : a ZSIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZSIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 *
 *@note
 * 1. ZSIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZSIndexFile file pathname is not stored but is a computed data from actual ZRawMasterFile file pathname.
 *
 * @param[in] pIndexName        User name of the index key as a utfdescString
 * @param[in] pKeyUniversalSize Fixed length index key size
 * @param[in] pDuplicates       Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 *  ZS_MODEINVALID ZRawMasterFile must be closed when calling zcreateRawIndex. If not, this status is returned.
 *
 */
ZStatus
ZRawMasterFile::zcreateRawIndex (const utf8String &pIndexName,
                                  uint32_t pKeyUniversalSize,
                                  ZSort_Type pDuplicates,
                                  bool pDoNotClose,
                                  bool pBackup)
{
  ZStatus wSt;
  bool wIsOpen = isOpen();
  long wi;
  zsize_type wIndexAllocatedSize=0;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
  ZRawIndexFile *wIndexObject=nullptr;
  long w1=0,w2=0,wIndexRank=0;

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

  if ((wi=ZMCB.IndexTable.searchCaseIndexByName(pIndexName.toCChar()))>-1)
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


  // instantiation of the ZSIndexFile new structure :
  // it gives a pointer to the actual ICB stored in Index vector
  //
  wIndexObject = new ZRawIndexFile(this,pKeyUniversalSize,pIndexName,pDuplicates);

  wIndexRank=ZMCB.IndexTable.push(wIndexObject); // since here any creation error will induce a desctruction of ZMCB.IndexTableObjects.lastIdx()

  // ---------compute index file name-------------------

  // Define IndexFileDirectoryPath
  //     if mentionned then take it
  //     if not then take the directory from Master File URI

  if (ZMCB.IndexFilePath.isEmpty())
  {
    //        utfdescString wDInfo;
    wIndexFileDirectoryPath=getURIContent().getDirectoryPath().toCChar();
  }
  else
  {
    wIndexFileDirectoryPath=ZMCB.IndexFilePath;
  }

  wSt=generateIndexURI(getURIContent(),
      wIndexFileDirectoryPath,
      wIndexURI,
      ZMCB.IndexTable.lastIdx(),
      pIndexName);
  if (wSt!=ZS_SUCCESS)
  {return  wSt;} // Beware return  is multiple instructions in debug mode

  // compute the allocated size
  w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
  w2 = ZMCB.IndexTable[wIndexRank]->IndexRecordSize();

  if (_Base::getBlockTargetSize()>0)
    if (_Base::getAllocatedBlocks()>0)
      wIndexAllocatedSize =  _Base::getAllocatedBlocks() * ZMCB.IndexTable[wIndexRank]->IndexRecordSize();
  //
  // Nota Bene : there is no History and Journaling processing for Index Files
  //
  wSt =  wIndexObject->zcreateIndex(*ZMCB.IndexTable.last(),      // pointer to index control block because ZSIndexFile stores pointer to Father's ICB
      wIndexURI,
      _Base::getAllocatedBlocks(),
      _Base::getBlockExtentQuota(),
      wIndexAllocatedSize,
      _Base::getHighwaterMarking(),
      //                                      _Base::getGrabFreeSpace(),
      false,        // grabfreespace is set to false
      pBackup,
      false          // do not leave it open
      );
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;

  //    ZMCB.IndexTable.last()->generateCheckSum();

  // update MCB to Reserved block in RandomFile header (_Base)
  // then write updated Master Control Block to Master Header
  //
  wSt= wIndexObject->openIndexFile(wIndexURI,wIndexRank, (ZRF_Exclusive | ZRF_All));
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;

  wSt=wIndexObject->zrebuildIndex(ZMFStatistics ,stderr);
  if (wSt!=ZS_SUCCESS)
    goto zcreateRawIndexError;


zcreateRawIndexEnd:
  if (!wIsOpen)
    zclose();  // close everything and therefore update MCB in file

  // else All dependent files are open with mode (ZRF_Exclusive | ZRF_All) when return ing.

  return  wSt;
zcreateRawIndexError:

  ZMCB.IndexTable.last()->zclose();
  ZMCB.IndexTable.pop() ; // destroy the ZSIndexFile object
  ZMCB.popIndex(); // destroy created ICB
  ZException.addToLast(" While creating new raw index <%s> for raw master file <%s>",
      pIndexName.toCChar(),
      URIContent.toCChar());
  return  wSt;

}//zcreateRawIndex

ZStatus
ZRawMasterFile::zcreateRawIndexDetailed (const utf8String &pIndexName, /*-----ICB------*/
                                          uint32_t pKeyUniversalSize,
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
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
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


  wSt =_createRawIndexDet ( pIndexName,         /*-----ICB------*/
                            pKeyUniversalSize,
                            pDuplicates,
                            pAllocatedBlocks,   /* ---FCB (for index ZRandomFile)---- */
                            pBlockExtentQuota,
                            pInitialSize,
                            pHighwaterMarking,
                            pGrabFreeSpace,
                            pReplace);

  if (!wIsOpen)
    zclose();   // close everything and therefore update MCB in file
        // else All dependent files are open with mode (ZRF_Exclusive | ZRF_All) when return ing.
  return  wSt;

} //zcreateRawIndexDetailed


#ifdef __COMMENT__
ZStatus
ZRawMasterFile::zcreateRawIndexDetailed (const utf8String &pIndexName, /*-----ICB------*/
                                          uint32_t pKeyUniversalSize,
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

  if ((wi=ZMCB.IndexTable.searchCaseIndexByName(pIndexName.toCChar()))>-1)
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


  // instantiation of the ZSIndexFile new structure :
  // it gives a pointer to the actual ICB stored in Index vector
  //
  wIndexObject = new ZRawIndexFile(this,pKeyUniversalSize,pIndexName,pDuplicates);

  wIndexRank=ZMCB.IndexTable.push(wIndexObject); // since here any creation error will induce a desctruction of ZMCB.IndexTableObjects.lastIdx()

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

  if (ZMCB.IndexFilePath.isEmpty())
  {
    //        utfdescString wDInfo;
    wIndexFileDirectoryPath=getURIContent().getDirectoryPath().toCChar();
  }
  else
  {
    wIndexFileDirectoryPath=ZMCB.IndexFilePath;
  }

  wSt=generateIndexURI( getURIContent(),
                        wIndexFileDirectoryPath,
                        wIndexURI,
                        ZMCB.IndexTable.lastIdx(),
                        pIndexName);
  if (wSt!=ZS_SUCCESS)
  {goto createRawIndexDetEnd;}

  // compute the allocated size
  w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
  w2 = ZMCB.IndexTable.last()->IndexRecordSize();

  if (_Base::getBlockTargetSize()>0)
    if (_Base::getAllocatedBlocks()>0)
      wIndexAllocatedSize =  _Base::getAllocatedBlocks() * ZMCB.IndexTable.last()->IndexRecordSize();
  //
  // Nota Bene : there is no History and Journaling processing for Index Files
  //
  wSt =  wIndexObject->zcreateIndex(*ZMCB.IndexTable[wIndexRank],      // pointer to index control block because ZSIndexFile stores pointer to Father's ICB
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

  //    ZMCB.IndexTable.last()->generateCheckSum();

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

  ZMCB.IndexTable[wIndexRank]->zclose();
  ZMCB.IndexTable.erase(wIndexRank) ; // destroy the ZSIndexFile object
  ZException.addToLast(" While creating new raw index <%s> for raw master file <%s>",
      pIndexName.toCChar(),
      URIContent.toCChar());
  goto createRawIndexDetEnd;
}//_createRawIndexDetailed
#endif // __COMMENT__

ZStatus
ZRawMasterFile::_createRawIndexDet (const utf8String &pIndexName,/*-----ICB------*/
                                    uint32_t pKeyUniversalSize,
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
  zsize_type wIndexAllocatedSize=0;
  uriString wIndexURI;
  uriString wIndexFileDirectoryPath;
  ZRawIndexFile *wIndexObject=nullptr;
  long w1=0,w2=0;
  long wIndexRank;

  if (!isOpen())
    {
    ZException.setMessage (_GET_FUNCTION_NAME_,
        ZS_MODEINVALID,
        Severity_Error,
        " ZRawMasterFile <%s> must be open.",
        getURIContent().toCChar());
    return  ZS_MODEINVALID;
    }

  if (!(getOpenMode()&(ZRF_Exclusive|ZRF_All)))
      {
      ZException.setMessage (_GET_FUNCTION_NAME_,
          ZS_MODEINVALID,
          Severity_Error,
          " ZRawMasterFile <%s> must be open with mode ZRF_Exclusive|ZRF_All.",
          getURIContent().toCChar());
      return  ZS_MODEINVALID;
      }

  // check index name ambiguity

  if ((wi=ZMCB.IndexTable.searchCaseIndexByName(pIndexName.toCChar()))>-1)
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_INVNAME,
          Severity_Error,
          " Ambiguous index name <%s>. Index name already exist at Index rank <%ld>. Please use zremoveIndex first.\n",
          pIndexName.toCChar(),
          wi);
      wSt= ZS_INVNAME;
      goto createRawIndexDet1End ;
    }


  // instantiation of the ZSIndexFile new structure :
  // it gives a pointer to the actual ICB stored in Index vector
  //
  wIndexObject = new ZRawIndexFile(this,pKeyUniversalSize,pIndexName,pDuplicates);

  wIndexRank=ZMCB.IndexTable.push(wIndexObject); // since here any creation error will induce a desctruction of ZMCB.IndexTableObjects.lastIdx()

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

  if (ZMCB.IndexFilePath.isEmpty())
    {
      //        utfdescString wDInfo;
      wIndexFileDirectoryPath=getURIContent().getDirectoryPath().toCChar();
    }
  else
    {
      wIndexFileDirectoryPath=ZMCB.IndexFilePath;
    }

  wSt=generateIndexURI( getURIContent(),
                        wIndexFileDirectoryPath,
                        wIndexURI,
                        wIndexRank,
                        pIndexName);
  if (wSt!=ZS_SUCCESS)
  {goto createRawIndexDet1End;}

  // compute the allocated size
  w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
  w2 = ZMCB.IndexTable.last()->IndexRecordSize();

  if (_Base::getBlockTargetSize()>0)
    if (_Base::getAllocatedBlocks()>0)
      wIndexAllocatedSize =  _Base::getAllocatedBlocks() * ZMCB.IndexTable[wIndexRank]->IndexRecordSize();
  //
  // Nota Bene : there is no History and Journaling processing for Index Files
  //
  wSt =  wIndexObject->zcreateIndex(*ZMCB.IndexTable[wIndexRank],      // pointer to index control block because ZSIndexFile stores pointer to Father's ICB
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

  //    ZMCB.IndexTable.last()->generateCheckSum();

  // update MCB to Reserved block in RandomFile header (_Base)
  // then write updated Master Control Block to Master Header
  //
  wSt= wIndexObject->openIndexFile(wIndexURI,wIndexRank,(ZRF_Exclusive | ZRF_All));
  if (wSt!=ZS_SUCCESS)
    goto createRawIndexDet1Error;

  wSt=wIndexObject->zrebuildIndex(ZMFStatistics ,stderr);
  if (wSt!=ZS_SUCCESS)
    goto createRawIndexDet1Error;

createRawIndexDet1End:
  return  wSt;

createRawIndexDet1Error:
  ZMCB.IndexTable[wIndexRank]->zclose();
  ZMCB.IndexTable[wIndexRank]->_removeFile();
  ZMCB.IndexTable.erase(wIndexRank) ; // destroy the ZSIndexFile object
  ZException.addToLast(" While creating new raw index <%s> for raw master file <%s>. Index has not been created.",
                        pIndexName.toCChar(),
                        URIContent.toCChar());
  goto createRawIndexDet1End;
}//_createRawIndexDetailed

#include <stdio.h>
/**
 * @brief ZRawMasterFile::zremoveIndex Destroys an Index definition and its files' content on storage
 *
 *  Removes an index.
 *
 *      renames all index files of rank greater than the current on to make them comply with ZSIndexFile naming rules.
 *
 *
 * @param pIndexRank Index rank to remove from ZRawMasterFile
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zremoveIndex (const long pIndexRank,ZaiErrors* pErrorLog)
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
              pErrorLog->logZException();
            return  ZS_MODEINVALID;
            }
    if ((pIndexRank<0)||(pIndexRank>ZMCB.IndexTable.size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Severe,
                                    " Out of indexes boundaries: rank <%ld>  boundaries [0,%ld] : File <%s>",
                                    pIndexRank,
                                    ZMCB.IndexTable.lastIdx(),
                                    URIContent.toString());
            if (pErrorLog)
              pErrorLog->logZException();
            return  ZS_OUTBOUND;
            }

    wSt= ZMCB.IndexTable[pIndexRank]->zclose();  // close ZSIndexFile files to delete
    if (wSt!=ZS_SUCCESS)
                { return  wSt;} // Beware return  is multiple instructions in debug mode

    wSt=ZMCB.IndexTable[pIndexRank]->_removeFile();  // remove the files
    if (wSt!=ZS_SUCCESS)
                { return  wSt;} // Beware return  is multiple instructions in debug mode

 //   ZMCB.removeICB(pIndexRank); // removes Index stuff IndexObjects suff and deletes appropriately

    ZMCB.IndexTable.erase(pIndexRank);
    ZMCB.IndexCount= ZMCB.IndexTable.size();

    for (long wi = pIndexRank;wi<ZMCB.IndexTable.size();wi++)
        {

        FormerIndexContent = ZMCB.IndexTable[wi]->URIContent;
        FormerIndexHeader = ZMCB.IndexTable[wi]->URIHeader;

        ZMCB.IndexTable[wi]->zclose();// close index files before renaming its files

        wSt=generateIndexURI(getURIContent(),ZMCB.IndexFilePath,NewIndexContent,wi,ZMCB.IndexTable[wi]->IndexName);
        if (wSt!=ZS_SUCCESS)
                {
                return  wSt;// Beware return  is multiple instructions in debug mode
                }
        wSt=generateURIHeader(NewIndexContent,NewIndexHeader);
        if (wSt!=ZS_SUCCESS)
                {
                return  wSt;// Beware return  is multiple instructions in debug mode
                }
        ZMCB.IndexTable[wi]->URIContent = NewIndexContent;
        ZMCB.IndexTable[wi]->URIHeader = NewIndexHeader;

        wRet=rename(FormerIndexContent.toCString_Strait(),NewIndexContent.toCString_Strait());
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
                pErrorLog->logZException();
        }
        else
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_SUCCESS,
                                    Severity_Information,
                                    "Index content file <%s> has been renamed to <%s>",
                                    FormerIndexContent.toString(),
                                    NewIndexContent.toString());
            if (pErrorLog)
              pErrorLog->logZException();
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
            pErrorLog->logZException();
          }
        else
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_SUCCESS,
                                    Severity_Information,
                                    "Index content file <%s> has been renamed to <%s>",
                                    FormerIndexHeader.toString(),
                                    NewIndexHeader.toString());
            if (pErrorLog)
              pErrorLog->logZException();
        }
        ZMCB.IndexTable[wi]->openIndexFile( NewIndexContent,wi,wMode);
        }// for

    ZDataBuffer wMCBContent;
    return   _Base::updateReservedBlock(ZMCB._exportMCBAppend(wMCBContent),true);
}//zremoveIndex

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
ZRawMasterFile::zclearMCB (FILE* pOutput)
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
                    "%s>>  ZRawMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                     _GET_FUNCTION_NAME_,
                    getURIContent().toString());

            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_MODEINVALID,
                                     Severity_Error,
                                     " ZRawMasterFile <%s> is open in bad mode. Must be (ZRF_Exclusive|ZRF_All) or closed",
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
        ZMCB.report(pOutput);
        }

    while (ZMCB.IndexTable.size()>0)
            {
            wSt=zremoveIndex(ZMCB.IndexTable.lastIdx());
            if (wSt!=ZS_SUCCESS)
                {
                if (ZVerbose)
                    {
                    fprintf (wOutput,
                             "%s>> ****Error: removing index rank <%ld> status <%s> clearing ZMasterControlBlock of file <%s>\n"
                             "              Actual content\n",
                             _GET_FUNCTION_NAME_,
                             ZMCB.IndexTable.lastIdx(),
                             decode_ZStatus(wSt),
                             URIContent.toString());
                    }
                ZException.addToLast(" Index rank <%ld>. Clearing ZMasterControlBlock of file <%s>.",
                                       ZMCB.IndexTable.lastIdx(),
                                       URIContent.toString());
                return  wSt;
                }// not ZS_SUCCESS
            if (ZVerbose)
                {
                fprintf (pOutput,
                         "%s>>      Index successfully removed\n",
                         _GET_FUNCTION_NAME_);
                ZMCB.report(wOutput);
                }
            }//while

    if (FOutput)
            fclose(wOutput);
    ZMCB.clear();
    return   _Base::updateReservedBlock(ZMCB._exportMCBAppend(wMCBContent),true);
}//zclearMCB

/**
 * @brief ZRawMasterFile::zdowngradeZMFtoZRF downgrades a ZRawMasterFile structure to a ZRandomFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * Former dependant index files are left to themselves and not destroyed by this method.
 *
 * @param[in] pZMFPath      file path of the ZRawMasterFile to convert
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.downgradelog
 */
void
ZRawMasterFile::zdowngradeZMFtoZRF (const char* pZMFPath,FILE* pOutput)
{


ZStatus wSt;
ZRawMasterFile wMasterFile;
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
   fprintf (wOutput,"%s>> starting downgrading ZRawMasterFile to ZRandomFile file path <%s>  \n",
            _GET_FUNCTION_NAME_,
            pZMFPath);


    wSt=wMasterFile.zopen(wURIContent,(ZRF_Exclusive|ZRF_All));

    fprintf (wOutput,"%s>> getting ZReservedBlock content and load ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_);


   fprintf (wOutput,
            "%s>> clearing ZMCB\n"
            "            <%ld>  defined index(es) in ZMasterControlBlock. Destroying all index files & definitions from ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_,
            wMasterFile.ZMCB.IndexTable.size());

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

   wSt=wMasterZRF._open (ZRF_Exclusive | ZRF_All,ZFT_ZMasterFile);  // open ZMF using ZRandomFile routines
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
   fprintf (wOutput,"%s>>  File <%s> has been successfully converted from ZRawMasterFile to ZRandomFile\n",
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
 * @brief ZRawMasterFile::zupgradeZRFtoZMF upgrades a ZRandomFile structure to an empty ZRawMasterFile structure. Content data is not impacted.
 *  This is a static method.
 *
 * @param pZRFPath file path of the ZRandomFile to upgrade
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.
 *                    If set to its default value (nullptr), then an log file is generated with name <directory path><base name>.upgradelog
 */
void
ZRawMasterFile::zupgradeZRFtoZMF (const char* pZRFPath,FILE* pOutput)
{


ZStatus wSt;
ZRawMasterFile wMasterFile;
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
   fprintf (wOutput,"%s>> starting upgrading ZRandomFile to ZRawMasterFile file path <%s>  \n",
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
    wMasterFile.ZMCB._exportMCBAppend(wReservedBlock);

    wMasterZRF.setReservedContent(wReservedBlock);
    wMasterZRF.ZHeader.FileType = ZFT_ZMasterFile;
    wSt=wMasterZRF._writeFullFileHeader(true);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }

   wMasterZRF.zclose();
   fprintf (wOutput,"%s>>  File <%s> has been successfully converted from ZRandomFile to ZRawMasterFile.\n",
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
ZRawMasterFile::zcreate(const uriString pURI,
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

    wSt=_Base::_create(pInitialSize,ZFT_ZMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" While creating Master file %s",
                                 getURIContent().toString());
        return  wSt;
        }


  ZHeader.FileType = ZFT_ZMasterFile;     // setting ZFile_type

  _Base::setReservedContent(ZMCB._exportMCBAppend(wMCBContent));
//    wSt=_Base::updateReservedBlock(ZMCB._exportMCB());
  wSt=_Base::_writeFullFileHeader(true);
  if (wSt!=ZS_SUCCESS)
      {
      _Base::_close();
      ZException.addToLast(" While creating Master file %s",
                               getURIContent().toString());
      return  wSt;
      }

// Manage journaling for the being created file
// - set option to MCB
// - if journaling enabled : create journaling file

//    ZMCB.JournalingOn = pJournaling; // update journaling MCB option for the file
    if (pJournaling)
        {
        ZMCB.ZJCB->Journal=new ZSJournal(this);
        wSt=ZMCB.ZJCB->Journal->createFile();
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

/**
 * @brief ZRawMasterFile::zcreate Creates the raw content file and its header as a ZRandomFile with a structure capable of creating indexes.
 * @param[in] pURI  uriString containing the path of the future ZRawMasterFile main content file. Other file names will be deduced from this main name.
 * @param[in] pInitialSize Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pBackup    If set to true file will be replaced if it already exists. If false (default value), existing file will be renamed according renaming rules.
 * @param[in] pLeaveOpen   If set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zcreate (const uriString pURI, const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{

ZStatus wSt;
ZDataBuffer wMCBContent;
//    ZMFURI=pURI;

    printf ("ZMasterFile::zcreate \n");

    wSt=_Base::setPath (pURI);
    if (wSt!=ZS_SUCCESS)
                {return (wSt);}
    _Base::setCreateMinimum(pInitialSize);
    wSt=_Base::_create(pInitialSize,ZFT_ZMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            return (wSt);
            }
    /* ----------File is left open : so no necessity to open again
    wSt=_Base::_open(ZRF_Exclusive | ZRF_Write,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     ZMFURI.toString());
            return  wSt;
            }
            */

/*    ZMCB.MetaDic->clear();
    for (long wi=0;wi<pMetaDic->size();wi++)
                ZMCB.MetaDic->push(pMetaDic->Tab[wi]);

    ZMCB.MetaDic->generateCheckSum();
*/
    ZHeader.FileType = ZFT_ZMasterFile;     // setting ZFile_type (Already done in _create routine)
    if (pLeaveOpen)
            {
            ZMCB._exportMCBAppend(wMCBContent);
            _Base::setReservedContent(wMCBContent);
        //    wSt=_Base::updateReservedBlock(ZMCB._exportMCB());
            wSt=_Base::_writeFullFileHeader(true);
            if (wSt!=ZS_SUCCESS)
                    {
                    ZException.addToLast(" While creating Master file %s",
                                             getURIContent().toString());
                    return  wSt;
                    }

             return  wSt;
            }
    printf ("ZMasterFile::zclose \n");
    zclose(); // updates headers including reserved block
    return  wSt;
}//zcreate

/**
 * @brief ZRawMasterFile::zcreate Creates the raw content file and its header as a ZRandomFile with a structure capable of creating indexes.
 *  @note if a file of the same name already exists (either content file or header file)
 *        then content and header file will be renamed to  <base file name>.<extension>_bck<nn>
 *        where <nn> is a version number
 *
 * @param[in] pPathHame  a C string (const char*) containing the path of the future ZRawMasterFile main content file. Other file names will be deduced from this main name.
 * @param[in] pInitialSize      Initial file space in byte that is allocated at creation time. This space is placed in Free block pool as one block.
 * @param[in] pLeaveOpen   if set to true file is left open after its creation with open mode mask (ZRF_Exclusive | ZRF_All ). If false, file is closed.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zcreate (const char* pPathName, const zsize_type pInitialSize, bool pBackup, bool pLeaveOpen)
{
uriString wURI(pPathName);
//    ZMFURI=pPathName;
    return (zcreate(wURI,pInitialSize,pBackup,pLeaveOpen));
}//zcreate


//----------------End zcreate--------------------



/**
 * @brief ZRawMasterFile::zopen opens the ZRawMasterFile and all its associated ZSIndexFiles with a mode set to defined mask pMode
 * @param pURI  MasterFile file path to open
 * @param pMode Open mode (mask)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
ZRawMasterFile::zopen  (const uriString pURI, const int pMode)
{


ZStatus wSt;
ZDataBuffer wRawMCB;
ZArray<ZPRES> wIndexPresence;

    wSt=_Base::setPath(pURI);
    wSt=_Base::_open (pMode,ZFT_ZMasterFile);
    if (wSt!=ZS_SUCCESS)
            return  wSt;// Beware return  is multiple instructions in debug mode


//    ZMFURI = pURI;

    wSt=_Base::getReservedBlock(wRawMCB,true);
    if (wSt!=ZS_SUCCESS)
            {
            return  wSt;// Beware return  is multiple instructions in debug mode
            }

    wSt=ZMCB._import(wRawMCB.Data,wIndexPresence);
    if (wSt!=ZS_SUCCESS)
                    {return  wSt;}// Beware return  is multiple instructions in debug mode

// MCB is loaded
//----------Journaling----------------------

//    if (ZMCB.JournalingOn)
    if (ZMCB.JCBSize>0)     // if journaling requested
        {
        if (ZMCB.ZJCB->Journal==nullptr) // if no journaling : create one
                {
                ZMCB.ZJCB->Journal=new ZSJournal(this);
                wSt=ZMCB.ZJCB->Journal->init();
                if (wSt!=ZS_SUCCESS)
                        {return  wSt;}// Beware return  is multiple instructions in debug mode
                ZMCB.ZJCB->Journal->start();
                }
            else
                {
                if (!ZMCB.ZJCB->Journal->isOpen()) // if journal file is closed then need to re-open and restart
                        {
                  if (ZMCB.ZJCB->Journal->JThread.isCreated())  // journal file is not open but journaling thread is active
                                {
                                ZMCB.ZJCB->Journal->JThread.kill();
                                }
                        }
                }
        ZDataBuffer wJCBContent;

        wJCBContent.setData(ZReserved.Data+ZMCB.JCBOffset,ZMCB.JCBSize);
        ZMCB.ZJCB->_importJCB(wJCBContent);
        ZMCB.ZJCB->Journal->init();
        ZMCB.ZJCB->Journal->start();
        } // ZMCB.JournalingOn
        else // no journaling requested
        {
            if (ZMCB.ZJCB!=nullptr)
                {
                   if (ZMCB.ZJCB->Journal!=nullptr)
                                    delete ZMCB.ZJCB->Journal;
                   delete ZMCB.ZJCB;
                }
        }// else
//--------------End journaling----------------------------

//     Need to create ZSIndexFile object instances and open corresponding ZSIndexFiles for each ZMCB.IndexTable list rank
//
    ZMCB.IndexTable.clear();
    uriString wIndexUri;
long wi;

    for (wi=0;wi < ZMCB.IndexTable.size();wi++)
            {
//           ZSIndexFile* wIndex = new ZSIndexFile (this,(ZSIndexControlBlock&) *ZMCB.IndexTable[wi]);
//            ZMCB.IndexTable.push(wIndex);
            wIndexUri.clear();

            wSt=generateIndexURI(getURIContent(),ZMCB.IndexFilePath,wIndexUri,wi,ZMCB.IndexTable[wi]->IndexName);
            if (wSt!=ZS_SUCCESS)
                    {
                    return  wSt;// Beware return  is multiple instructions in debug mode
                    }
            if (ZVerbose)
                        fprintf(stdout,"Opening Index file <%s>\n",(const char*)wIndexUri.toString());
            wSt=ZMCB.IndexTable[wi]->openIndexFile(wIndexUri,wi,pMode);
            if (wSt!=ZS_SUCCESS)
                    {
                    ZException.addToLast(" while opening index rank <%ld>", wi);
// In case of open index error : close any already opened index file
//              Then close master content file before return ing
                    ZStatus wSvSt = wSt;
                    long wj;
                    for (wj = 0;wj < wi;wj++)
                                    ZMCB.IndexTable[wj]->zclose();  // use the base ZRandomFile zclose routine
                     _Base::_close();

                    return  wSt;
                    }
            }// for


    return  ZS_SUCCESS;
}// zopen

/**
 * @brief ZRawMasterFile::zclose close the ZRawMasterFile, and all dependent ZSIndexFiles
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zclose(void)
{


ZStatus wSt;
ZStatus SavedSt=ZS_SUCCESS;
ZDataBuffer wMCBContent;

// closing index files

  if (!_isOpen)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                " file is not open : cannot close it.");
        return  ZS_INVOP;
    }
  for (long wi=0;wi < ZMCB.IndexTable.size();wi++)
    {
    if ((wSt=ZMCB.IndexTable[wi]->closeIndexFile())!=ZS_SUCCESS)
        {
        ZException.printUserMessage(stderr);  // error on close is not blocking because we need to close ALL files
        SavedSt = wSt;
        }
    }// for

// flush MCB to file

    wSt=_Base::updateReservedBlock(ZMCB._exportMCBAppend(wMCBContent),true);// force to write

    if (wSt!=ZS_SUCCESS)
                {

                ZException.addToLast( " Writing Reserved header for ZRawMasterFile %s",
                                        getURIContent().toString());
                _Base::zclose();
                return  wSt;
                }

// releasing index resources

    while (ZMCB.IndexTable.size()>0)
                            ZMCB.IndexTable.pop();

    while (ZMCB.IndexTable.size()>0)
                            ZMCB.IndexTable.pop();

// ending journaling process

//    setJournalingOff();  // nope

// closing main ZMF content file

    wSt=_Base::zclose();
    if (wSt!=ZS_SUCCESS)
            {
            return  wSt;// Beware return  is multiple instructions in debug mode
            }
    if (SavedSt!=ZS_SUCCESS)
                { return  SavedSt;}// Beware return  is multiple instructions in debug mode
    return  ZS_SUCCESS;
}// zclose

ZStatus
ZRawMasterFile::_removeFile(const char* pContentPath, ZaiErrors *pErrorLog)
{
  ZStatus wRetSt=ZS_SUCCESS;
  if (isOpen())
    zclose();
  ZStatus wSt=zopen(pContentPath,ZRF_Exclusive);
  if (wSt!=ZS_SUCCESS)
    {
    if (pErrorLog!=nullptr)
        pErrorLog->logZException();
    zclose();
    return wSt;
    }
  for (long wi=0;wi < ZMCB.IndexTable.size();wi++)
  {
    if ((wSt=ZMCB.IndexTable[wi]->_removeFile(pErrorLog))!=ZS_SUCCESS)
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
ZRawMasterFile::removeFile (const char* pContentPath, ZaiErrors *pErrorLog)
{
  ZRawMasterFile wZRMF;
  return wZRMF._removeFile(pContentPath,pErrorLog);
}

ZStatus
ZRawMasterFile::_renameBck(const char* pContentPath, ZaiErrors *pErrorLog, const char* pBckExt)
{
  ZStatus wRetSt=ZS_SUCCESS;
  if (isOpen())
    zclose();
  ZStatus wSt=zopen(pContentPath,ZRF_Exclusive);
  if (wSt!=ZS_SUCCESS)
    {
    if (pErrorLog!=nullptr)
      pErrorLog->logZException();
    zclose();
    return wSt;
    }
  for (long wi=0;wi < ZMCB.IndexTable.size();wi++)
    {
    if ((wSt=ZMCB.IndexTable[wi]->_renameBck(pErrorLog,pBckExt))!=ZS_SUCCESS)
      wRetSt= wSt;
    }// for
  wSt= ZRandomFile::_renameBck(pErrorLog,pBckExt);
  if (wSt!=ZS_SUCCESS)
    {
    zclose();
    return wSt;
    }
  return wRetSt;
}//_renameBck


ZStatus
ZRawMasterFile::renameBck (const char* pContentPath, ZaiErrors* pErrorLog, const char* pBckExt)
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
  ZStatus wSt = _getRaw(RawRecord, pZMFRank);
  if (wSt == ZS_SUCCESS)
    pRecordContent = RawRecord->Content;
  return wSt;
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
ZRawMasterFile::_getRaw(ZRawRecord* pRecord, const zrank_type pZMFRank)
{
  pRecord->Rank=pZMFRank;
  ZStatus wSt= ZRandomFile::zgetWAddress(pRecord->RawContent,pRecord->Rank,pRecord->Address);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  pRecord->getContentFromRaw();
  return  wSt;
}// ZRawMasterFile::_getRaw

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

  if (pKeys.count() != ZMCB.IndexTable.count())
    {
    ZException.setMessage("ZRawMasterFile::zinsert", ZS_INVSIZE, Severity_Error,
        "Invalid key count. Provided <%ld> keys while Index number is <%ld>.",
        pKeys.count(), ZMCB.IndexTable.count());
    return ZS_INVSIZE;
    }
  RawRecord->Content = pRecordContent;
  for (long wi = 0; wi < pKeys.count(); wi++)
    RawRecord->setRawKeyContent(wi, pKeys[wi]);
  RawRecord->prepareForWrite();

  return _insertRaw(RawRecord, pZMFRank);
}// zinsert


ZStatus
ZRawMasterFile::_insertRaw       (ZRawRecord *pRecord, const zrank_type pZMFRank)
{
  ZStatus wSt;

  zrank_type      wZMFIdxCommit;
  zaddress_type   wZMFAddress=0;

  ZArray <zrank_type>       IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
  ZSIndexItemList      IndexItemList;          // stores keys description per index processed

  ZSIndexItem*    wIndexItem;
  zrank_type      wIndex_Rank;

  /* prepare the add on Master File, reserve appropriate space, get entry in pool.
   * NB: wZMFIdxCommit must be equal to requested rank (pRecord->Rank) in case of success or -1 if fails.
   */

  pRecord->Rank = pZMFRank;

  wSt=_Base::_insert2PhasesCommit_Prepare(pRecord->RawContent,pRecord->Rank,wZMFIdxCommit,pRecord->Address);
  if (wSt!=ZS_SUCCESS)
    goto _insertRaw_error;

  //
  // update all Indexes
  //

  for (wIndex_Rank=0;wIndex_Rank< ZMCB.IndexTable.size();wIndex_Rank++)
  {
    wIndexItem = new ZSIndexItem;
    wIndexItem->Operation=ZO_Insert;

    wSt=ZMCB.IndexTable[wIndex_Rank]->_extractRawKey(pRecord,wIndexItem->KeyContent);
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During _extractRawKey operation on index number <%ld>",wIndex_Rank);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _add_RollbackIndexes (IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2PhasesCommit_Rollback(wZMFIdxCommit); // do not accept update on Master File and free resources
      goto _insertRaw_error;
    }

    wSt=ZMCB.IndexTable[wIndex_Rank]->_addRawKeyValue_Prepare(wIndexItem,wZMFIdxCommit, wZMFAddress);// for indexes don't care about insert, this is an add key value
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During zinsert operation on index number <%ld>",wIndex_Rank);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _add_RollbackIndexes ( IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2PhasesCommit_Rollback(wZMFIdxCommit); // do not accept update on Master File and free resources
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
    _Base::_add2PhasesCommit_Rollback(wZMFIdxCommit);

    goto _insertRaw_error;
    }

  // at this stage all indexes have been committed
  //         commit for Master file data must be done now
  //
  wSt = _Base::_insert2PhasesCommit_Commit(pRecord->RawContent,wZMFIdxCommit);// accept insert update on Master File
  if (wSt!=ZS_SUCCESS)    //! and if then an error occur : hard rollback all indexes and signal exception
    {
    _add_HardRollbackIndexes (IndexRankProcessed); // indexes are already committed so use hardRollback to counter pass
    }                                                    // don't care about the status . In case of error exception stack will trace it


  if (getJournalingStatus())
    {
    ZMCB.ZJCB->Journal->enqueue(ZJOP_Insert,pRecord->RawContent);
    }

_insertRaw_error:
  _Base::_unlockFile () ; // set Master file unlocked
  IndexItemList.clear();
  return  wSt;
}// _insertRaw

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
ZStatus ZRawMasterFile::zadd (ZDataBuffer& pRecordContent,ZArray<ZDataBuffer>& pKeys )
{
  if (pKeys.count() != ZMCB.IndexTable.count())
    {
    ZException.setMessage("ZRawMasterFile::zadd",
        ZS_INVSIZE,
        Severity_Error,
        "Invalid key count. Provided <%ld> keys while Index number is <%ld>.", pKeys.count(),ZMCB.IndexTable.count());
    return ZS_INVSIZE;
    }
  RawRecord->Content = pRecordContent;
  for (long wi=0;wi < pKeys.count();wi++)
    RawRecord->setRawKeyContent(wi,pKeys[wi]);
  RawRecord->prepareForWrite();

  return _addRaw(RawRecord);
}

ZStatus
ZRawMasterFile::_addRaw(ZRawRecord* pRecord)
{
  ZStatus wSt;
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

  wSt=_Base::_add2PhasesCommit_Prepare( pRecord->RawContent,
                                        wZMFZBATIndex,     // get internal ZBAT pool allocated index
                                        wZMFAddress);
  if (wSt!=ZS_SUCCESS)
    {
    goto zaddRaw_error;
    }

  // =========== update all defined Indexes ======================

  IndexRankProcessed.clear();

  for (wIndex_Rank=0;wIndex_Rank< ZMCB.IndexTable.size();wIndex_Rank++)
  {
    if (ZVerbose)
    {
      fprintf (stdout,"Index number <%ld>\n",wIndex_Rank);
    }
    wIndexItem=new ZSIndexItem;
    wIndexItem->Operation=ZO_Push;
    wSt=ZMCB.IndexTable[wIndex_Rank]->_extractRawKey(pRecord,wIndexItem->KeyContent);
    if (wSt!=ZS_SUCCESS)
    {
      ZException.addToLast("During zadd operation on index number <%ld>",wi);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _add_RollbackIndexes (IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2PhasesCommit_Rollback(wZMFZBATIndex); // do not accept update on Master File and free resources
      goto zaddRaw_error;
    }

    wSt=ZMCB.IndexTable[wIndex_Rank]->_addRawKeyValue_Prepare(wIndexItem,wIdxZBATIndex, wZMFAddress);
    if (wSt!=ZS_SUCCESS)
      {
      ZException.addToLast("During zadd operation on index number <%ld>",wi);
      delete wIndexItem;
      // on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
      _add_RollbackIndexes ( IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
      // on error reset ZMF in its original state
      _Base::_add2PhasesCommit_Rollback(wZMFZBATIndex); // do not accept update on Master File and free resources
      goto zaddRaw_error;
      }

    IndexRankProcessed.push(wIdxZBATIndex) ;   // this index has been added to this rank
    IndexItemList.push(wIndexItem);            // with that key content

    //        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);

  }// main for - ZMCB.IndexTableObjects.size()

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
    _Base::_add2PhasesCommit_Rollback(wZMFZBATIndex);

    goto zaddRaw_error;
  }

  // at this stage all indexes have been committed
  //         commit for Master file data must be done now
  //
//  wSt = _Base::_add2PhasesCommit_Commit(_Base::pRecord->RawContent,wZMFZBATIndex,wZMFAddress);// accept update on Master File
  wSt = _Base::_add2PhasesCommit_Commit(pRecord->RawContent,pRecord->Rank,pRecord->Address);// accept update on Master File

  if (wSt!=ZS_SUCCESS)    // and if then an error occur : hard rollback all indexes
    {
    _add_HardRollbackIndexes ( IndexRankProcessed);// indexes are already committed so use hardRollback regardless returned ZStatus
    }                                                   // don't care about the status . In case of error exception stack will trace it


  if ((wSt==ZS_SUCCESS) && getJournalingStatus())
    {
    ZMCB.ZJCB->Journal->enqueue(ZJOP_Add,pRecord->RawContent);
    }
zaddRaw_error:
  _Base::_unlockFile () ; // set Master file unlocked
  IndexItemList.clear();
  //    while (IndexItemList.size()>0)
  //                delete IndexItemList.popR();

  return  wSt;
}// _addRaw


/** @cond Development */

ZStatus
ZRawMasterFile::_add_CommitIndexes (ZArray <ZSIndexItem*>  &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed)
{

ZStatus wSt;
long wj = 0;
    wj=0;

    for (wj=0;wj<pIndexRankProcessed.size();wj++)
      {
       wSt=ZMCB.IndexTable[wj]->_addRawKeyValue_Commit(pIndexItemList[wj],pIndexRankProcessed[wj]);
       if (wSt!=ZS_SUCCESS)
           {
           ZException.addToLast("While committing add operation on index(es)");
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to pZMCB.IndexTableObjects.size() soft rollback

            for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
              {
              ZMCB.IndexTable[wR]->_addKeyValue_HardRollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                      // regardless ZStatus (exception is on stack)
              } // for

              for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
              {
              ZMCB.IndexTable[wR]->_addRawKeyValue_Rollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                  // regardless ZStatus (exception is on stack)
              } //for

              return  ZException.getLastStatus(); // return  the very last status encountered
          }//wSt
      } // for
   return  ZS_SUCCESS;
} // _add_CommitIndexes

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
ZRawMasterFile::_add_RollbackIndexes ( ZArray<zrank_type> &pIndexRankProcessed)
{
ZStatus wSt;
long wi = 0;
/*    if (pZMCB.IndexTableObjects.size()!=pZMCB.IndexTable.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZSIndexFile objects number <%s>",
                                        pZMCB.IndexTable.size(),
                                        pZMCB.IndexTableObjects.size());
                ZException.exit_abort();
            }*/

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_add_RollbackIndexes Soft Rollback of indexes on add \n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            ZMCB.IndexTable[wi]->_addRawKeyValue_Rollback(pIndexRankProcessed[wi]); // rollback add on each index concerned
            }//for                                                                  // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();
} // _add_RollbackIndexes

/**
 * @brief ZRawMasterFile::_add_HardRollbackIndexes Hard rollback : counter-pass the operation done to restore indexes in their previous state
 * @param pZMCB
 * @param pIndexRankProcessed
 * @return
 */
ZStatus
ZRawMasterFile::_add_HardRollbackIndexes ( ZArray<zrank_type> &pIndexRankProcessed)
{


ZStatus wSt;
long wi = 0;
//    ZException.getLastStatus() = ZS_SUCCESS;

    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on add operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            ZMCB.IndexTable[wi]->_addKeyValue_HardRollback(pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            }// for                                                                     // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();

} // _add_HardRollbackIndexes

/** @endcond */
//----------------End Add sequence------------------------------------------------------
/**
 * @brief ZRawMasterFile::zreorgFile reorder the base file for ZRawMasterFile and rebuild all indexes
 *
 * superseeds ZRandomFile::zreorgFile as an method overload.
 *
 * As ZRandomFile::zreorgFile() changes physical block addresses in reorganizing file structure,
 * it is necessary to rebuild any defined index after having done a zreorgFile process.
 *
 * ZRawMasterFile::zreorgFile() does the whole stuff :
 * - base file reorganization
 * - rebuild for all defined indexes for the ZRawMasterFile @see ZSIndexFile::zrebuildIndex()
 *
 * @param[in] pDump this boolean sets (true) the option of having surfaceScan during the progression of reorganization. Omitted by default (false)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZRawMasterFile::zreorgFile (bool pDump,FILE *pOutput)
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


     if (!ZFCB->GrabFreeSpace)        // activate grabFreeSpace if it has been set on
                 {
     ZFCB->GrabFreeSpace=true;
                 wgrabFreeSpaceSet = true;
                 }

    zstartPMSMonitoring();

    wSt = ZRandomFile::_reorgFileInternals(pDump,pOutput);

   while (wi < ZMCB.IndexTable.size())
           {
//            wSt=pZMCB.IndexTableObjects[wj]->_addRollBackIndex(pIndexRankProcessed.popR()); // rollback update on each index concerned
       wSt=ZMCB.IndexTable[wi]->zrebuildIndex(false,pOutput); // hard rollback update on each already committed index

       if (wSt!=ZS_SUCCESS)
               {
//               ZException_sv = ZException; // in case of error : store the exception but continue rolling back other indexes
               ZException.addToLast(" during Index rebuild on index <%s> number <%02ld> ",
                                           ZMCB.IndexTable[wi]->IndexName.toCChar(),
                                           wi);
               }

           wi++;
           }

    zendPMSMonitoring ();
    fprintf (pOutput,
             " ----------End of ZRawMasterFile reorganization process-------------\n");

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
                 ZFCB->GrabFreeSpace=false;
                 }
    zclose ();
    if (wasOpen)
            zopen(URIContent,wMode);

    return  ZException.getLastStatus();

error_zreorgZMFFile:
    ZException.printUserMessage(pOutput);
    goto end_zreorgZMFFile;

}// ZRawMasterFile::zreorgFile

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
    if ((pIndexRank<0)||(pIndexRank>ZMCB.IndexTable.size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Severe,
                                    " Out of indexes boundaries: rank <%ld>  boundaries [0,%ld] : File <%s>",
                                    pIndexRank,
                                    ZMCB.IndexTable.lastIdx(),
                                    URIContent.toString());
            return  ZS_OUTBOUND;
            }
 return  ZMCB.IndexTable[pIndexRank]->zrebuildIndex (pStat,pOutput);
}//zindexRebuild


//----------------Remove sequence------------------------------------------------------

ZStatus
ZRawMasterFile::zremoveByRank    (const zrank_type pZMFRank)
{
    return _removeByRankRaw (RawRecord,pZMFRank);
} // zremoveByRank

ZStatus
ZRawMasterFile::zremoveByRankR     (ZDataBuffer& pRecordContent,const zrank_type pZMFRank)
{
    ZStatus wSt= _removeByRankRaw (RawRecord,pZMFRank);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    pRecordContent=RawRecord->Content;
} // zremoveByRankR


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
ZRawMasterFile::_removeByRankRaw  (ZRawRecord *pRawRecord,
                                    const zrank_type pZMFRank)
{
  ZStatus     wSt;

  ZArray <zrank_type>     IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
  ZSIndexItemList         IndexItemList;      // stores keys description per index processed (rollback purpose). Must stay as pointer (double free)
  ZSIndexItem             *wIndexItem;
  ZDataBuffer             wKeyContent;

  ZBlock          wBlock;

  //---- get Master File header test for lock - if OK lock it
  pRawRecord->Rank=pZMFRank;
  wSt=_Base::_remove_Prepare(pRawRecord->Rank,pRawRecord->Address);  // prepare remove on Master File mark entry in pool as to be deleted
  if (wSt!=ZS_SUCCESS)
    {
    _Base::_remove_Rollback(pRawRecord->Rank);
    return  wSt;
    }

  wSt=_Base::_readBlockAt(wBlock,pRawRecord->Address);  // we know that it is locked as ZBS_BeingDeleted
  if (wSt!=ZS_SUCCESS)
    {
    _Base::_remove_Rollback(pRawRecord->Rank);
    return  wSt;
    }

  pRawRecord->RawContent=wBlock.Content;  /* save record content */
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
  pRawRecord->getContentFromRaw();

  for (wi=0;wi< pRawRecord->KeyValue.count();wi++)
  {
    wSt=ZMCB.IndexTable[wi]->_removeIndexItem_Prepare(*pRawRecord->KeyValue[wi],
        wIndex_Rank);
    if (wSt!=ZS_SUCCESS)
    {
      //                delete wIndexItem; // free memory for errored key value
      // on error reset all already processed indexes in their original state
      // (IndexRankProcessed heap contains the Index ranks added)
      // An additional error during index rollback will be put on exception stack

      _remove_RollbackIndexes (ZMCB, IndexRankProcessed); // do not care about ZStatus : exception will be on stack

      // on error reset ZMF in its original state
      _Base::_remove_Rollback(pRawRecord->Rank); // do not accept update on Master File and free resources

      //                if (ZMCB.HistoryOn)
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

  wSt=_remove_CommitIndexes (ZMCB,IndexItemList,IndexRankProcessed) ;
  if (wSt!=ZS_SUCCESS)
  {
    // in case of error : appropriate indexes soft or hard rollback is made in Commit Indexes routine

    _Base::_remove_Rollback(pRawRecord->Rank); // an error occurred on indexes : rollback all MasterFile
    // _Base::_unlockFile (ZDescriptor) ; // unlock is done in rollback routine
    goto _removeByRank_return ;// if an error occurs here : this is a severe error that only may be caused by an HW or IO subsystem failure.
  }

  wSt = _Base::_remove_Commit(pRawRecord->Rank);// accept update on Master File
  if (wSt!=ZS_SUCCESS)    // and if then an error occur : rollback all indexes and signal exception
  {
    _remove_HardRollbackIndexes (ZMCB, IndexItemList,IndexRankProcessed);    // indexes are already committed so use hardRollback to counter pass
    goto _removeByRank_return ;
  }

  if ((wSt==ZS_SUCCESS)&&getJournalingStatus())
  {
    ZMCB.ZJCB->Journal->enqueue(ZJOP_RemoveByRank,pRawRecord->RawContent,pRawRecord->Rank,pRawRecord->Address);
  }

_removeByRank_return :
  _Base::_unlockFile () ;
  pRawRecord->Address = -1;/* if successfully removed, then address becomes invalid */
  pRawRecord->Rank = -1;/* if successfully removed, then rank becomes invalid */
  /*
    while (IndexItemList.size()>0)
                  delete IndexItemList.popR();
*/
  IndexItemList.clear();
  return  wSt;
}// _removeByRankRaw



/**
 * @brief ZRawMasterFile::_remove_CommitIndexes For All ZSIndexFile associated to current ZRawMasterFile commits the remove operation
 *
 *
 * @param[in] pZMCB             ZMasterControlBlock owning indexes definitions
 * @param[in] pIndexItemList        List of pointers to ZIndexItem to be removed (Index Key contents)- Necessary in case of Hard Rollback.
 * @param[in] pIndexRankProcessed   List of ranks for ZSIndexFile to be removed
 * @return  a ZStatus value. ZException is set appropriately with error message content in case of error.
 */
ZStatus
ZRawMasterFile::_remove_CommitIndexes (ZSMasterControlBlock& pZMCB, ZSIndexItemList & pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed)
{
ZStatus wSt;
long wj = 0;

//    ZException.getLastStatus() =ZS_SUCCESS;
    for (wj=0;wj<pIndexRankProcessed.size();wj++)
            {
            wSt=pZMCB.IndexTable[wj]->_removeKeyValue_Commit(pIndexRankProcessed[wj]);
            if (wSt!=ZS_SUCCESS)
                {
//  hard roll back already processed indexes
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to pZMCB.IndexTableObjects.size() soft rollback

                for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
                {
                pZMCB.IndexTable[wR]->_removeKeyValue_HardRollback(pIndexItemList[wR] ,pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                        // regardless ZStatus (exception is on stack)
                } // for

                for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
                {
                pZMCB.IndexTable[wR]->_removeKeyValue_Rollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                    // regardless ZStatus (exception is on stack)
                }// for

            return  ZException.getLastStatus(); // return  the very last status encountered
            }//wSt
    } // Main for

   return  ZException.getLastStatus();
} // _remove_CommitIndexes


ZStatus
ZRawMasterFile::_remove_RollbackIndexes (ZSMasterControlBlock& pZMCB, ZArray<zrank_type> &pIndexRankProcessed)
{


ZStatus wSt;
long wi = 0;
        if (pZMCB.IndexTable.size()!=pZMCB.IndexTable.size())
                {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_CORRUPTED,
                                            Severity_Fatal,
                                            " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZSIndexFile objects number <%s>",
                                            pZMCB.IndexTable.size(),
                                            pZMCB.IndexTable.size());
                    ZException.exit_abort();
                }

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_remove_RollbackIndexes Soft Rollback of indexes on remove\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexTable[wi]->_removeKeyValue_Rollback(pIndexRankProcessed[wi]);  // rollback remove on each index concerned
            }// for                                                                     // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();
} // _remove_RollbackIndexes


ZStatus
ZRawMasterFile::_remove_HardRollbackIndexes (ZSMasterControlBlock& pZMCB,
                                           ZArray<ZSIndexItem*> &pIndexItemList,
                                           ZArray<zrank_type> &pIndexRankProcessed)
{


long wi = 0;

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (pZMCB.IndexTable.size()!=pZMCB.IndexTable.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZSIndexFile objects number <%s>",
                                        pZMCB.IndexTable.size(),
                                        pZMCB.IndexTable.size());
                ZException.exit_abort();
            }
    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on remove operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexTable[wi]->_removeKeyValue_HardRollback(pIndexItemList[wi],pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            } // for                                                    // don't care about ZStatus: exception stack will track

   return  ZException.getLastStatus();
} // _remove_HardRollbackIndexes

//----------------End Remove sequence------------------------------------------------------


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
ZRawMasterFile::zsearch(ZDataBuffer &pRecord,ZSKey *pKey)
{

    return (zsearch(pRecord,(ZDataBuffer &)*pKey,pKey->IndexNumber));
}
/**
 * @brief ZRawMasterFile::zsearch searches for a single key value using a ZDataBuffer containing key value to search
 * Key to search for has to be previously formatted to index internal format using appropriate routines
 * @copydetail ZSIndexFile::_search()
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
ZSIndexResult wZIR;
    wSt = ZMCB.IndexTable[pIndexNumber]->_search(pKeyValue,*ZMCB.IndexTable[pIndexNumber],wZIR);
    if (wSt!=ZS_FOUND)
            { return  wSt;}

    RawRecord->Address=wZIR.ZMFAddress;

    return  zgetByAddress(RawRecord->RawContent,wZIR.ZMFAddress);
}

class ZSIndexCollection;

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
                          ZSIndexCollection& pIndexCollection,
                          const ZMatchSize_type pZMS)
{



    return  ZMCB.IndexTable[pIndexNumber]->_searchAll(pKeyValue,
                                                      *ZMCB.IndexTable[pIndexNumber],
                                                      pIndexCollection,
                                                      pZMS);

}
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
ZRawMasterFile::zsearchAll (ZSKey &pZKey,ZSIndexCollection& pIndexCollection)
{


ZStatus wSt;

    pIndexCollection.reset();
    ZMatchSize_type wZSC = ZMS_MatchIndexSize;

    pIndexCollection.ZIFFile = ZMCB.IndexTable[pZKey.IndexNumber];// assign ZSIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    if (pZKey.FPartialKey)
            wZSC=ZMS_MatchKeySize ;
    wSt = ZMCB.IndexTable[pZKey.IndexNumber]->_searchAll(pZKey,*ZMCB.IndexTable[pZKey.IndexNumber],pIndexCollection,wZSC);
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
 *  zsearchFirst uses ZSIndexFile::_searchFirst().
 *
 * @copydoc ZSIndexFile::_searchFirst()
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
ZRawMasterFile::zsearchFirst (ZSKey &pZKey,
                           ZDataBuffer& pRecord,
                           ZSIndexCollection *pCollection)
{


ZStatus wSt;
ZSIndexResult wZIR;

    ZMatchSize_type wZMS = ZMS_MatchIndexSize;


    if (pZKey.FPartialKey)
            wZMS=ZMS_MatchKeySize ;
    wSt = ZMCB.IndexTable[pZKey.IndexNumber]->_searchFirst(pZKey,
                                                             *ZMCB.IndexTable[pZKey.IndexNumber],
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
 * @brief ZRawMasterFile::zsearchNext
 * @copydoc ZSIndexFile::_searchNext()
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
ZRawMasterFile::zsearchNext (ZSKey &pZKey,
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

    //pIndexCollection.ZIFFile = ZMCB.IndexTableObjects[pIndexNumber];// assign ZSIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    wSt = ZMCB.IndexTable[pZKey.IndexNumber]->_searchNext(wZIR,pCollection);
    if (wSt!=ZS_FOUND)
                {return  wSt;}// Beware return  is multiple instructions in debug mode

    wSt=zgetByAddress(pRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
                {return  wSt;}// Beware return  is multiple instructions in debug mode
    return  ZS_FOUND;
}// zsearchFirst





//----------------End Search sequence----------------------------------------------------


/**
 * @brief getRawIndex   Gets a raw index item (without Endian conversion) corresponding to is IndexNumber and its rank
 * @param pIndexItem    A ZIndexItem with returned index content
 * @param pIndexRank    Rank for index
 * @param pIndexNumber  Index number
 * @return
 */
ZStatus
ZRawMasterFile::getRawIndex(ZSIndexItem &pIndexItem,const zrank_type pIndexRank,const zrank_type pKeyNumber)
{
    ZStatus wSt;
    ZDataBuffer wIndexContent;
    wSt=ZMCB.IndexTable[pKeyNumber]->_Base::zget(wIndexContent,pIndexRank);
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
void ZRawMasterFile::ZMCBreport(void)
{
    ZMCB.report();
    return;
}

//----------------------XML Reports and stuff-----------------------------------
/** addtogroup XMLGroup
 * @{ */



utf8String ZRawMasterFile::toXml(int pLevel,bool pComment)
{
  utf8String wReturn = fmtXMLnode("file",pLevel);

  wReturn += fmtXMLint("filetype",int(ZFT_ZMasterFile),pLevel+1);
  if (pComment)
    fmtXMLaddInlineComment(wReturn," File type is ZFT_ZMasterFile i. e. ZRawMasterFile or ZSMasterFile");
  /* first file descriptor */
  wReturn += toXml(pLevel+1,pComment);
  /* second master control block */
  wReturn += ZMCB.toXml(pLevel+1,pComment);


  wReturn += fmtXMLendnode("file", pLevel);
  return wReturn;
} // ZRawMasterFile:toXml


utf8String ZRawMasterFile::XmlSaveToString(bool pComment)
{
  utf8String wReturn = fmtXMLdeclaration();
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
  utf8String wContent = XmlSaveToString(pComment);

  wOutContent.setData(wContent.toCChar(), wContent.ByteSize);

  return pXmlFile.writeContent(wOutContent);
}// ZRawMasterFile::XmlSaveToFile


/** @} */ // group XMLGroup




//-------------------Statistical functions-----------------------------------------

/**
@addtogroup ZMFSTATS ZRawMasterFile and ZSIndexFile storage statistics and PMS session monitoring

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


    if ((pIndex<0)||(pIndex>ZMCB.IndexTable.size()))
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

             ZMCB.IndexTable[pIndex]->IndexName.toCChar(),
             URIHeader.toString(),
             ZMCB.IndexTable[pIndex]->getURIContent().toString(),
             ZMCB.IndexTable[pIndex]->getURIHeader().toString()
             );


    ZMCB.IndexTable[pIndex]->ZPMS.reportDetails(pOutput);
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
   ZRawMasterFile::zreportPMSMonitoring (FILE* pOutput)
   {
       fprintf(pOutput,
               "________________________________________________\n"
               "   ZRawMasterFile <%s>\n"
               "   File open mode         %s\n"
               "________________________________________________\n",
               URIContent.toString(),
               decode_ZRFMode( Mode)
               );
       ZPMSStats.reportFull(pOutput);
       return;
   }



/**
  * @brief ZRawMasterFile::zstartIndexPMSMonitoring Starts PMS monitoring session on the specific index given by its rank (pIndex)
  * @param pIndex rank of the index to monitor
  */
 ZStatus
 ZRawMasterFile::zstartIndexPMSMonitoring (const long pIndex)
 {

     if ((pIndex<0)||(pIndex>ZMCB.IndexTable.lastIdx()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                "Invalid index number <%ld> requested while index range is [0,%ld]",
                                pIndex,
                                ZMCB.IndexTable.lastIdx());
        return  (ZS_INVOP);
        }
     ZMCB.IndexTable[pIndex]->ZPMSStats.init();
     return  ZS_SUCCESS;
 }//zstartIndexPMSMonitoring
 /**
   * @brief ZRawMasterFile::zendIndexPMSMonitoring Ends current PMS monitoring session on the specific index given by its rank (pIndex)
   * @param pIndex rank of the index to monitor
   */
ZStatus
ZRawMasterFile::zstopIndexPMSMonitoring(const long pIndex)
{

  if ((pIndex<0)||(pIndex>ZMCB.IndexTable.lastIdx()))
     {
     ZException.setMessage(_GET_FUNCTION_NAME_,
                             ZS_INVOP,
                             Severity_Severe,
                             "Invalid index number <%ld> requested while index range is [0,%ld]",
                             pIndex,
                             ZMCB.IndexTable.lastIdx());
     return  (ZS_INVOP);
     }
  ZMCB.IndexTable[pIndex]->ZPMSStats.end();
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

   if ((pIndex<0)||(pIndex>ZMCB.IndexTable.lastIdx()))
      {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Invalid index number <%ld> requested while index range is [0,%ld]",
                              pIndex,
                              ZMCB.IndexTable.lastIdx());
      return  (ZS_INVOP);
      }
   fprintf(pOutput,
           "________________________________________________\n"
           "   ZRawMasterFile <%s>\n"
           "   Index rank <%ld> <%s>\n"
           "   File open mode    %s\n",
           URIContent.toString(),
           pIndex,
           ZMCB.IndexTable[pIndex]->IndexName.toCChar(),
           decode_ZRFMode( Mode));
   ZMCB.IndexTable[pIndex]->ZPMSStats.reportFull(pOutput);
   return  ZS_SUCCESS;
}// zreportIndexPMSMonitoring


#endif // ZRAWMASTERFILE_CPP
