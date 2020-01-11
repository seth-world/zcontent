#ifndef ZSMASTERFILE_CPP
#define ZSMASTERFILE_CPP

#include <zindexedfile/zsmasterfile.h>
#include <zindexedfile/zskey.h>

#include <zrandomfile/zrfcollection.h>
#include <zindexedfile/zsjournal.h>
#include <QUrl>
#include <zindexedfile/zmasterfile_utilities.h>

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



//------------ZIndexObjectTable-------------------

long ZSIndexObjectTable::pop (void)
{
_MODULEINIT_
    if (size()<0)
            {
            _RETURN_ -1;// Beware _RETURN_ is multiple instructions in debug mode
            }
    last()->~ZSIndexFile();
    if (last()!=nullptr)
            {
            delete last();
            last()=nullptr;
            }
    _RETURN_ _Base::pop();
} // pop

long ZSIndexObjectTable::erase (long pRank)
{
_MODULEINIT_

    if (pRank>lastIdx())
                {
                _RETURN_ -1;
                }
    Tab[pRank]->~ZSIndexFile();

    if (Tab[pRank]!=nullptr)
            delete Tab[pRank];   // appearingly it puts the mess
    Tab[pRank]=nullptr; // just in case for avoiding double free
    _RETURN_ _Base::erase(pRank);
} // erase

void ZSIndexObjectTable::clear(void)
{
    while (size()>0)
                    pop();
    _Base::clear();
}// clear

//------------------End ZIndexObjectTable ---------------------

//----------------ZIndexControlTable--------------------------

long ZSIndexControlTable::erase(const long pIdx)
{
_MODULEINIT_
    if (Tab[pIdx]->ZKDic!=nullptr)
                        delete Tab[pIdx]->ZKDic;
    Tab[pIdx]->ZKDic=nullptr;
    _RETURN_ _Base::erase(pIdx);
}
long ZSIndexControlTable::push (ZSIndexControlBlock *pICB)
{
_MODULEINIT_
/*    newBlankElement();
    last().clear(pMetaDic);  // set up ZICB and its Key Dictionnary

    memmove (&last(),pICB,sizeof(ZSICBOwnData));
    long wj = 0 ;
    while (wj<pICB.ZKDic->size())
          {
          last().ZKDic->push(pICB.ZKDic->Tab[wj]);
          wj++;
          }*/
    _Base::push(pICB);
    _RETURN_ lastIdx();
}//push

long ZSIndexControlTable::pop (void)
{
    if (size()<1)
            {
            _RETURN_ -1;
            }
    if (last()->ZKDic!=nullptr)
            {
            delete last()->ZKDic ;
            last()->ZKDic=nullptr;
            }
    delete last();
    return _Base::pop();
}
void ZSIndexControlTable::clear (void)
{
    while (size()>0)
                    ZSIndexControlTable::pop();
    return;
}

long ZSIndexControlTable::zsearchIndexByName (const char* pName)
{
    for (long wi =0;wi<size();wi++)
            {
            if (Tab[wi]->Name==pName)
                        {
                        _RETURN_ wi;
                        }
            }

    return -1;
}//zsearchIndexByName


//-----------End ZIndexControlTable --------------------

// ----------ZMasterControlBlock ----------
//

ZSMasterControlBlock::~ZSMasterControlBlock(void)
{
/*    if (MDicCheckSum!=nullptr)   // No MDicCheckSum object is deleted while deleting MetaDic object
                delete MDicCheckSum;*/
    if (ZJCB!=nullptr)
                delete ZJCB;
    while (Index.size()>0)
            {
            IndexObjects.pop();
            Index.pop();
            }
    return;
}


long ZSMasterControlBlock::popICB(void)
    {
    if (Index.pop()<0)
                {
                return -1;
                }
    IndexCount=Index.size();
    return Index.size();
    }


void ZSMasterControlBlock::clear(void)
{
    IndexCount=0;
    Index.clear();
//    HistoryOn=false;
//    JournalingOn = false;
    MCBSize = 0;
//    BlockID=ZBID_MCB;                 // only used within _Export structures
//    ZMFVersion= __ZMF_VERSION__;
//    StartSign=cst_ZSTART;
    while (Index.size()>0)
                Index.pop();
    IndexObjects.clear();
    IndexFilePath.clear();
    return;
}

/**
 * @brief ZMasterControlBlock::pushICBtoIndex creates a new rank in ZMCB.Index rank and stores ZIndexControlblock in it
 * @param pICB
 */
void
ZSMasterControlBlock::pushICBtoIndex(ZSIndexControlBlock* pICB)
{
//    ZIndexField_struct wField;

/*    Index.newBlankElement();
    Index.last()->clear(pMetaDic);  // create ZICB instance -  Key Dictionnary

    IndexCount= Index.size();
    long wi = Index.lastIdx();

    memmove (&Index[wi],pICB,sizeof(ZSICBOwnData));
    long wj = 0 ;
    while (wj<pICB.ZKDic->size())
        {
        Index[wi]->ZKDic->push(pICB.ZKDic->Tab[wj]);
        wj++;
        }*/
    Index.push(pICB);
    return;
}

void
ZSMasterControlBlock::removeICB(const long pRank)
{
 //   delete Index[pRank].ZKDic; - done in erase overloaded routine
    Index.erase(pRank);
    IndexObjects.erase(pRank);
    IndexCount= Index.size();
    return ;
}
/**
 * @brief ZSMCBOwnData::_export
 *      Exports Master Control Block own data
 *      IndexFilePath, a fixed length uriString, is stored using its varying Universal Format (VUniversal)
 *      -> uint16_t size of string content WITHOUT ending '\0' char
 *      -> string content
 *      IndexFilePath is then added under this format at the end of MCB own data export format
 *
 *      Size of exported MCB is then sizeof(ZZSMCBOwnData_Export)+size of VUFormat of string
 *      This size is stored in MCBSize.
 *      Nota Bene : MDicOffset is equal to MCBSize.
 *
 * @param pZDBExport
 * @return
 */

ZDataBuffer&
ZSMCBOwnData::_export(ZDataBuffer& pZDBExport)
{
ZDataBuffer wIndexPath;
    ZSMCBOwnData_Export wMCB;

    wMCB.StartSign=cst_ZSTART ;
    wMCB.BlockID=ZBID_MCB;
    wMCB.ZMFVersion=reverseByteOrder_Conditional<unsigned long>(__ZMF_VERSION__);
    IndexFilePath._exportUVF(&wIndexPath);

    MDicOffset=MCBSize=  sizeof(ZSMCBOwnData_Export) + wIndexPath.Size ;

    wMCB.MCBSize=reverseByteOrder_Conditional<size_t>(MCBSize);
    wMCB.MDicOffset=reverseByteOrder_Conditional<size_t>(MDicOffset);
    wMCB.MDicSize=reverseByteOrder_Conditional<size_t>(MDicSize);
    wMCB.ICBOffset=reverseByteOrder_Conditional<size_t>(ICBOffset);
    wMCB.ICBSize=reverseByteOrder_Conditional<size_t>(ICBSize);
    wMCB.JCBOffset=reverseByteOrder_Conditional<ssize_t>(JCBOffset);
    wMCB.JCBSize=reverseByteOrder_Conditional<ssize_t>(JCBSize);
    wMCB.IndexCount=reverseByteOrder_Conditional<size_t>(IndexCount);

    wMCB.HistoryOn=HistoryOn;  // uint8_t used as boolean

    pZDBExport.setData(&wMCB,sizeof(ZSMCBOwnData_Export));  // move all up until IndexFilePath
                    // append IndexFilePath without '\0' end character : NO END SIGN MARKER
    pZDBExport.appendData(wIndexPath);

    return pZDBExport;
}//ZSMCBOwnData::_export

ZStatus
ZSMCBOwnData::_import(unsigned char* pZDBImport_Ptr)
{
_MODULEINIT_
ZStatus wSt;
    ZSMCBOwnData_Export* wMCB=(ZSMCBOwnData_Export*) pZDBImport_Ptr;
    printf ("%s>\n",_GET_FUNCTION_NAME_);

    if (wMCB->StartSign != cst_ZSTART)
            {
           ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_BADFILEHEADER,
                                  Severity_Error,
                                  "Error Master Control Block  : file header appears to be corrupted - invalid MCB StartBlock");
           _RETURN_(ZS_BADFILEHEADER);
            }
    if (wMCB->BlockID != ZBID_MCB)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                   ZS_BADICB,
                                   Severity_Error,
                                   "Error Master Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
                                   wMCB->BlockID);
            _RETURN_ (ZS_BADICB);
            }
    if (reverseByteOrder_Conditional<unsigned long>(wMCB->ZMFVersion)!= __ZMF_VERSION__)
            {
           ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_BADFILEVERSION,
                                  Severity_Error,
                                  "Error Master Control Block   : Found version <%ld> while current ZMF version is <%ld>",
                                  reverseByteOrder_Conditional<unsigned long>(wMCB->ZMFVersion),
                                  __ZMF_VERSION__);
           _RETURN_(ZS_BADFILEVERSION);
            }


    MCBSize=reverseByteOrder_Conditional<size_t>(wMCB->MCBSize);
    MDicOffset=reverseByteOrder_Conditional<size_t>(wMCB->MDicOffset);
    MDicSize=reverseByteOrder_Conditional<size_t>(wMCB->MDicSize);
    ICBOffset=reverseByteOrder_Conditional<size_t>(wMCB->ICBOffset);
    ICBSize=reverseByteOrder_Conditional<size_t>(wMCB->ICBSize);
    JCBOffset=reverseByteOrder_Conditional<ssize_t>(wMCB->JCBOffset);
    JCBSize=reverseByteOrder_Conditional<ssize_t>(wMCB->JCBSize);
    IndexCount=reverseByteOrder_Conditional<size_t>(wMCB->IndexCount);

    HistoryOn=wMCB->HistoryOn;  // uint8_t used as boolean
    IndexFilePath._importUVF(pZDBImport_Ptr+sizeof(ZSMCBOwnData_Export));

/*    wMCB->IndexFilePathSize=_reverseByteOrder_T<uint16_t>(wMCB->IndexFilePathSize);
    wMCB->IndexFilePath=(char*) (wMCB + offsetof(ZSMCBOwnData_Export,IndexFilePath));
//size_t wSize = MCBSize - offsetof(ZSMCBOwnData_Export,IndexFilePath)-sizeof (uint32_t);
    IndexFilePath.clear();
    IndexFilePath.fromStringCount(wMCB->IndexFilePath,wMCB->IndexFilePathSize);*/

    _RETURN_ ZS_SUCCESS;
}//ZSMCBOwnData::_import

/**
 * @brief ZSMasterFileControlBlock::_exportMCB   exports ZSMasterFileControlBlock content to a flat ZDataBuffer.
 * Updates values (offset and size) for ZJournalingControlBlock if ever journaling exists.
 * Both values are set to -1 if journaling does not exist.
 * @return a ZDataBuffer containing the flat raw data exported from ZMCB
 */
ZDataBuffer& ZSMasterControlBlock::_exportMCB(ZDataBuffer &pMCBContent)
{
_MODULEINIT_
ZDataBuffer wExportContent;
// set / reset IndexCount
size_t wMetaDicOffset=0,wICBOffset =0;
    printf ("%s>\n",_GET_FUNCTION_NAME_);
    IndexCount = Index.size(); // by security

//    pMCBContent.setData(this,sizeof(ZSMCBOwnData)); // set Own MCB Data to ZDataBuffer
    ZSMCBOwnData::_export(pMCBContent); // export ZSMCBOwnData in Universal Format

    ZSMCBOwnData_Export* wMCBOwn = (ZSMCBOwnData_Export*)pMCBContent.Data;
    wMetaDicOffset=pMCBContent.Size;
    wMCBOwn->MDicOffset = reverseByteOrder_Conditional<size_t>(wMetaDicOffset); // update MetaDic offset within ZDataBuffer

    MetaDic._export(wExportContent);        // export MetaDic to temporary
    pMCBContent.appendData(wExportContent);

    wICBOffset=wMetaDicOffset+wExportContent.Size;

    wMCBOwn = (ZSMCBOwnData_Export*)pMCBContent.Data;  // warning after appendData ZDB memory location changes
    wMCBOwn->MDicSize = reverseByteOrder_Conditional<size_t>(wExportContent.Size); // update MetaDic export Size within ZDataBuffer

    wMCBOwn->ICBOffset = reverseByteOrder_Conditional<size_t>(wICBOffset); // update ICB offset within ZDataBuffer

    size_t wICBTotalSize=0;
    for (long wi = 0;wi < Index.size(); wi++)
        {
        Index[wi]->_exportICB(wExportContent);
        pMCBContent.appendData(wExportContent);  // export only index definition not object pointer
        wICBTotalSize+= wExportContent.Size;
        }
    wMCBOwn = (ZSMCBOwnData_Export*)pMCBContent.Data;  // warning after appendData ZDB memory location changes
//    wMCBOwn->MCBSize = _reverseByteOrder_T<size_t>(pMCBContent.Size); // update total size of exported MCB in exported buffer
    wMCBOwn->ICBSize = reverseByteOrder_Conditional<size_t>(wICBTotalSize); // update size of exported ICB in exported buffer
    //---------ZJournalingControlBlock data-----------------------

    if (ZJCB==nullptr)
            {
            wMCBOwn->JCBOffset = -1 ;
            wMCBOwn->JCBSize = -1 ;
            }
        else        // if journaling defined : export ZSJournalControlBlock
            {
            wMCBOwn->JCBOffset = wMCBOwn->MCBSize;// already reversed if necessary

            ZDataBuffer wJCB;
            if (ZJCB!=nullptr)
                    pMCBContent.appendData(ZJCB->_exportJCB(wJCB)); //
            wMCBOwn = (ZSMCBOwnData_Export*)pMCBContent.Data;  // warning after appendData ZDB memory location changes
            wMCBOwn->JCBSize = reverseByteOrder_Conditional<size_t>(wJCB.Size); // update size of exported JCB in exported buffer
            }

    _RETURN_ pMCBContent;
}// _exportMCB

/**
 * @brief ZSMasterFileControlBlock::_importMCB imports (rebuild) a ZSMasterFileControlBlock from a ZDataBuffer containing flat raw data to import
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterControlBlock::_importMCB(ZDataBuffer& pBuffer)
{
_MODULEINIT_

ZStatus wSt;
//long wIndexCountsv ;

//ZSMCBOwnData* wMCB =(ZSMCBOwnData*) pBuffer.Data;
if (ZVerbose)
        fprintf (stdout,
                 "%s>> Importing MCB Own data\n",_GET_FUNCTION_NAME_);
    ZSMCBOwnData wMCBOwn;
    wSt=wMCBOwn._import(pBuffer.Data);  // needed to taken into account conversion to natural format
    if (wSt!=ZS_SUCCESS)
                {
                _RETURN_ wSt;
                }
    clear();
    memmove(this,&wMCBOwn,sizeof(ZSMCBOwnData));

//-----------Import defined IndexControlBlocks for the file

//    wIndexCountsv = IndexCount;
    ZSIndexControlBlock* wICB=nullptr;
    if (ZVerbose)
            fprintf (stdout,
                     "%s-I- Importing meta dictionary\n",_GET_FUNCTION_NAME_);
    unsigned char* wPtr;
    size_t wInSize;
    wPtr = pBuffer.Data + wMCBOwn.MDicOffset ;  // or MDicOffset

    wInSize = MetaDic._import(wPtr);  // importing Meta dictionary
//    MDicCheckSum=MetaDic.getCheckSum();

    wPtr += wInSize;
    if (ZVerbose)
            fprintf (stdout,
                     "%s-I- Importing Index Control Blocks\n",_GET_FUNCTION_NAME_);
    for (size_t wi = 0;wi < IndexCount; wi++)  // IndexCount has been updated from memmove
    {
        wICB= new ZSIndexControlBlock(&MetaDic);

        wSt=wICB->_importICB(&MetaDic,wPtr,wMCBOwn.ICBSize);
        Index.push(wICB);
        if (wSt!=ZS_SUCCESS)
                    {  _RETURN_ wSt; }
        wPtr = wPtr + wInSize ;    // adjust wPtr to next ZIndexControlBlock
//        if (wPtr >= (pBuffer.Data+pBuffer.Size))
//                        break;
        if (wPtr >= (pBuffer.Data + JCBOffset))
                                            break;
    }// for

//------------Import Journaling Control block if defined must be done else where--------

    if (ZVerbose)
            fprintf (stdout,
                     "%s-I- End import Master Control Block\n",_GET_FUNCTION_NAME_);

    _RETURN_ ZS_SUCCESS;
}//_importMCB

/**
 * @brief ZMasterControlBlock::print Reports the whole content of ZMCB : indexes definitions and dictionaries
 */
void
ZSMasterControlBlock::report(FILE*pOutput)
{
    fprintf (pOutput,
             "________________ZMasterControlBlock Content________________________________\n");
    fprintf(pOutput,
            "Meta dictionary\n"
            "---------------\n");
    for (long wi=0;wi < Index.size();wi++)
        {
         fprintf (pOutput,
                  "<%2ld> <%5ld> <%20s> <%s> \n",
                 wi,
                  MetaDic[wi].Capacity,
                 MetaDic[wi].Name.toString(),
                 decode_ZType(MetaDic[wi].ZType));
        }

     fprintf(pOutput,
                 "Index Control Blocks\n"
                 "--------------------\n");
    for (long wi=0;wi < Index.size();wi++)
        {
         fprintf (pOutput,
                  "Index Rank <%2ld> <%20s>\n"
                 "          <%15s> <%15s> Index Record size <%ld>\n",
                 wi,
                 Index[wi]->Name.toString(),
                 Index[wi]->Duplicates==ZST_DUPLICATES?"Duplicates":"No Duplicates",
                 Index[wi]->AutoRebuild?"Autorebuild":"NoAutorebuild",
                 Index[wi]->IndexRecordSize());
         fprintf (pOutput,
                  "   Fields %17s %5s %5s %12s %s\n",
                 "Field Name",
                 "Natural",
                 "Internal",
                 "Offset",
                 "ZType");
        for (long wj=0; wj < Index[wi]->ZKDic->size();wj++)
        {
         fprintf (pOutput,
                 "    <%ld> <%15s>  %5ld %5ld %s\n",
                 wj,
                 MetaDic.Tab[Index[wi]->ZKDic->Tab[wj].MDicRank].Name.toString(),
                 Index[wi]->ZKDic->Tab[wj].NaturalSize,
                 Index[wi]->ZKDic->Tab[wj].UniversalSize,
//                 Index[wi].ZKDic->Tab[wj].RecordOffset,
                 decode_ZType(MetaDic.Tab[Index[wi]->ZKDic->Tab[wj].MDicRank].ZType));
        }
        }
    fprintf (pOutput,
             "___________________________________________________________________________\n");
    return ;
} // print

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
    BlockID=ZBID_MCB;
    ZMFVersion= __ZMF_VERSION__;
    StartSign=cst_ZSTART;
    JournalLocalDirectoryPath.clear();
    return ;
}

ZDataBuffer&
ZSJCBOwnData::_export(ZDataBuffer& pZDBExport)
{
    ZSJCBOwnData wJCB;
    wJCB.StartSign=reverseByteOrder_Conditional<uint32_t>(StartSign);
    wJCB.BlockID=BlockID;

    wJCB.ZMFVersion=reverseByteOrder_Conditional<unsigned long>(ZMFVersion);
    wJCB.JCBSize=reverseByteOrder_Conditional<size_t>(JCBSize);
    wJCB.JournalLocalDirectoryPath=JournalLocalDirectoryPath;
    wJCB.JournalingOn=JournalingOn; // uint8_t
    wJCB.Keep=Keep; // uint8_t
    pZDBExport.setData(&wJCB,sizeof(wJCB));
    return  pZDBExport;
}// ZSJCBOwnData::_export
ZSJCBOwnData&
ZSJCBOwnData::_import(unsigned char* pZDBImport_Ptr)
{
    ZSJCBOwnData* wJCB=(ZSJCBOwnData*) pZDBImport_Ptr;

    StartSign=reverseByteOrder_Conditional<uint32_t>(wJCB->StartSign);
    BlockID=wJCB->BlockID;

    ZMFVersion=reverseByteOrder_Conditional<unsigned long>(wJCB->ZMFVersion);
    JCBSize=reverseByteOrder_Conditional<size_t>(wJCB->JCBSize);

    JournalLocalDirectoryPath=wJCB->JournalLocalDirectoryPath;
    JournalingOn=wJCB->JournalingOn;
    Keep=wJCB->Keep;

   return *this;
}//ZSJCBOwnData::_import

/**
 * @brief ZSJournalControlBlock::_exportJCB   exports ZSJournalControlBlock content to a flat ZDataBuffer.
 * @return a ZDataBuffer containing the flat raw data exported from ZJCB
 */
ZDataBuffer& ZSJournalControlBlock::_exportJCB(ZDataBuffer &pJCBContent)
{
ZDataBuffer wJCBContent;


//    pJCBContent.setData(this,sizeof(ZSJCBOwnData)); // set Own JCB Data to ZDataBuffer

    ZSJCBOwnData::_export(pJCBContent);
    ZSJCBOwnData* wJCB = (ZSJCBOwnData*) pJCBContent.Data ;
    wJCB->JCBSize = pJCBContent.Size;          // update size of exported JCB in exported buffer

    return pJCBContent;
}// _exportJCB

size_t
ZSJournalControlBlock::_getExportSize()
{
    return (sizeof(ZSJCBOwnData));
}

/**
 * @brief ZSJournalControlBlock::_importJCB imports (rebuild) a ZSJournalControlBlock from a ZDataBuffer containing flat raw data to import
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSJournalControlBlock::_importJCB(ZDataBuffer& pJCBContent)
{
_MODULEINIT_
//ZStatus wSt;
//long wIndexCountsv ;

ZSJCBOwnData* wJCB =(ZSJCBOwnData*) pJCBContent.Data;

    if (wJCB->BlockID != ZBID_JCB)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_BADICB,
                               Severity_Error,
                               "Error Journal Control Block identification is bad. Value <%ld>  : File header appears to be corrupted - invalid BlockID",
                               wJCB->BlockID);
        _RETURN_(ZS_BADICB);
        }
    if (wJCB->StartSign != cst_ZSTART)
        {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADFILEHEADER,
                              Severity_Error,
                              "Error Journal Control Block  : Index header appears to be corrupted - invalid JCB StartBlock mark");
       _RETURN_(ZS_BADFILEHEADER);
        }
    if (wJCB->ZMFVersion!= __ZMF_VERSION__)
        {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADFILEVERSION,
                              Severity_Error,
                              "Error Journal Control Block   : Found version <%ld> while current ZMF version is <%ld>",
                              wJCB->ZMFVersion,
                              __ZMF_VERSION__);
       _RETURN_(ZS_BADFILEVERSION);
        }

    clear();
    memmove(this,pJCBContent.Data,sizeof(ZSJCBOwnData));

    _RETURN_ ZS_SUCCESS;
}//_importJCB

ZStatus
ZSJournalControlBlock::purge(const zrank_type pKeepRanks)
{

    if (Journal==nullptr)
                {
                return ZS_INVOP; // Beware _RETURN_ is multiple instructions in debug mode
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
                                          char* pPort,
                                          char* pAuthenticate,
                                          char* pSSLKey,
                                          char* pSSLVector)
{
    if (Remote!=nullptr)
                delete Remote;
    Remote = new ZRemoteMirroring;
    Remote->Protocol = pProtocol;
    strncpy(Remote->Host,pHost,sizeof(Remote->Host));
    strncpy(Remote->Port,pPort,sizeof(Remote->Port));
    strncpy(Remote->Authenticate,pAuthenticate,sizeof(Remote->Authenticate));
    strncpy(Remote->SSLKey,pSSLKey,sizeof(Remote->SSLKey));
    strncpy(Remote->SSLVector,pSSLVector,sizeof(Remote->SSLVector));

}


//------------------------------------ZSMasterFile-------------------
//
ZSMasterFile::ZSMasterFile(void) //: ZRandomFile(_cst_default_allocation,_cst_realloc_quota)
{
// ZMCB.JournalingOn=pJournaling;
 ZMCB.ZJCB=nullptr;
// ZMCB.JournalingOn = false;
return ;
}

ZSMasterFile::ZSMasterFile(uriString pURI) : ZRandomFile(pURI)
{
    ZStatus wSt=setPath(pURI);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
//    ZMCB.JournalingOn=false;
    ZMCB.ZJCB=nullptr;
//    Journal->init(this);
    return;
}


ZSMasterFile:: ~ZSMasterFile(void)
{
    if (ZDescriptor._isOpen)
                        zclose();
     while (ZMCB.IndexObjects.size() >0)
             {
             if (ZMCB.IndexObjects.last()->ZDescriptor._isOpen)
                                            ZMCB.IndexObjects.last()->closeIndexFile();
             ZMCB.IndexObjects.pop(); // deletes the contained object
             }

      while (ZMCB.Index.size() >0)
             {
             ZMCB.Index.pop();
             }

      setJournalingOff();
}// DTOR

//------------Journaling--------------------------------

ZStatus
ZSMasterFile::initJournaling (uriString &pJournalPath)
{
_MODULEINIT_

ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
// this does not touch integrity of files

    if (!ZDescriptor._isOpen)
            {

                wasOpen=false;
                wSt = zopen(ZRF_Exclusive|ZRF_Write);
                if (wSt!=ZS_SUCCESS)
                            {_RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode
            }
        else
        {
        if (ZDescriptor.Mode!=(ZRF_Exclusive|ZRF_Write))
            {
            wFormerMode = ZDescriptor.Mode;
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
                        _RETURN_ ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Journaling for file %s is set to started while journal file is marked as not open.",
                                    getURIContent().toString());
            ZException.exit_abort();
//            _RETURN_ ZS_FILENOTOPEN;
            }
        } // else
    wSt=ZMCB.ZJCB->Journal->init();
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While initializing journaling");
        _RETURN_ wSt;
        }

//    ZMCB.JournalingOn=true;
    ZMCB.ZJCB->JournalLocalDirectoryPath = pJournalPath;
    wSt=writeControlBlocks();

    if (wSt!=ZS_SUCCESS)
                {
                    _RETURN_ wSt; // Beware _RETURN_ is multiple instructions in debug mode
                }
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-JCB Journaling set on within JournalingControlBlock\n");

    if (!wasOpen)  // if was not open close it
                {_RETURN_ zclose();}

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
    _RETURN_ wSt;
} // initJournaling

ZStatus
ZSMasterFile::setJournalingOn (void)
{
ZStatus wSt;
//bool wasOpen=true;
//zmode_type wFormerMode=ZRF_Nothing;
// this does not touch integrity of files

if (!ZDescriptor._isOpen)
        {
        fprintf (stderr,
                 "setJournalingOn-E File <%s> must be open to start/restart journaling.\n",
                 getURIContent().toString());
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                "File <%s> must be open to start/restart journaling.",
                                getURIContent().toString());
        _RETURN_ ZS_INVOP;
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
                _RETURN_ ZS_INVOP;
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
                        _RETURN_ ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Setting journaling on : Journaling for file %s is started while journal file is marked as not open.",
                                    getURIContent().toString());
            _RETURN_ ZS_FILENOTOPEN;
            }

    wSt=ZMCB.ZJCB->Journal->init(true); // Journal file must exist
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While initializing journaling");
        _RETURN_ wSt;
        }


 //   ZMCB.JournalingOn=true;
 //   ZMCB.ZJCB->JournalLocalDirectoryPath = pJournalPath;
 //   wSt=writeControlBlocks();

//    if (wSt!=ZS_SUCCESS)
//                  {  _RETURN_ wSt;}

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

/*    if (!wasOpen)  // if ZSMasterFile was not open close it
           { _RETURN_ zclose();}

    if (wFormerMode!=ZRF_Nothing) // if ZSMasterFile was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open ZSMasterFile with former mode
            }*/
    _RETURN_ wSt;
} // setJournalingOn

#ifdef __COMMENT__
/**
 * @brief ZSMasterFile::setJournalingOn starts Journaling process for the given file.
 * If journaling is already started, nothing happens.
 * ZSMasterFile must be open to start journaling, otherwise a ZS_INVOP status is returned.
 *
 * @return a ZStatus set to ZS_SUCCESS if everything went well
 */
ZStatus
ZSMasterFile::setJournalingLOn()
{
_MODULEINIT_

ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
// this does not touch integrity of files

if (!ZDescriptor._isOpen)
            {  _RETURN_ (ZS_INVOP);}

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
                        _RETURN_ ZS_SUCCESS;
                        }
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTOPEN,
                                    Severity_Severe,
                                    " Setting journaling on : Journaling for file %s is started while journal file is marked as not open.",
                                    getURIContent().toString());
            _RETURN_ ZS_FILENOTOPEN;
            }
    wSt=ZMCB.ZJCB->Journal->init(true); // journaling file must have been created elsewhere and must exist
    if (wSt!=ZS_SUCCESS)
        {

        ZException.addToLast("While initializing journaling");
        _RETURN_ wSt;
        }

/*
 //   ZMCB.JournalingOn=true;
 //   ZMCB.ZJCB->JournalLocalDirectoryPath = pJournalPath;
    wSt=writeControlBlocks();

    if (wSt!=ZS_SUCCESS)
                 {   _RETURN_ wSt;}
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

    if (!wasOpen)  // if ZSMasterFile was not open close it
            {   _RETURN_ zclose();}

    if (wFormerMode!=ZRF_Nothing) // if ZSMasterFile was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open ZSMasterFile with former mode
            }
    _RETURN_ wSt;*/
} // setJournalingOn
#endif // __COMMENT__

ZStatus
ZSMasterFile::setJournalingOff (void)
{
_MODULEINIT_

//    if (!ZMCB.JournalingOn)
//                  {  _RETURN_ ZS_SUCCESS;}
ZStatus wSt;
bool wasOpen=true;
zmode_type wFormerMode=ZRF_Nothing;
    // this does not touch integrity of files

    if (!ZDescriptor._isOpen)
            {

                wasOpen=false;
                wSt = zopen(ZRF_Exclusive|ZRF_Write);
                if (wSt!=ZS_SUCCESS)
                            {_RETURN_ wSt; } // Beware _RETURN_ is multiple instructions in debug mode
            }
        else
        {
        if (ZDescriptor.Mode!=(ZRF_Exclusive|ZRF_Write))
            {
            wFormerMode = ZDescriptor.Mode;
            zclose();
            zopen(ZRF_Exclusive|ZRF_Write);
            }

        }// else


    if (ZVerbose)
            fprintf (stderr,
                     "setJournalingOff-I Stopping journaling for file %s\n",
                     getURIContent().toString());
    if (ZMCB.ZJCB==nullptr)
            { _RETURN_ ZS_SUCCESS;}  // Beware _RETURN_ is multiple instructions in debug mode
    if (ZMCB.ZJCB->Journal==nullptr)
            { _RETURN_ ZS_SUCCESS;}  // Beware _RETURN_ is multiple instructions in debug mode

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
                        _RETURN_ Journal->zclose();
                        }
            delete Journal;
            _RETURN_ ZS_SUCCESS;
            }*/

    delete ZMCB.ZJCB;  // deletes Journaling Control Block  AND ZJournal instance : send a ZJOP_Close to journal thread



//    ZMCB.JournalingOn=false;
    wSt=writeControlBlocks();
    if (wSt!=ZS_SUCCESS)
                { _RETURN_ wSt;} // Beware _RETURN_ is multiple instructions in debug mode
    if (ZVerbose)
            fprintf(stdout,
                    "setJournalingOn-I-MCB Journaling for file %s has been set off within MCB header \n",
                    getURIContent().toCChar());


    if (!wasOpen)  // if was not open close it
            { _RETURN_ zclose();} // Beware _RETURN_ is multiple instructions in debug mode

    if (wFormerMode!=ZRF_Nothing) // if was open but not in appropriate mode
            {
            zclose();       // close it
            wSt=zopen(wFormerMode); // re-open with former mode
            }
    _RETURN_ wSt;
} // setJournalingOff

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
 * @param[in] pPath an uriString containing the path where ZSIndexFiles will be located
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::setIndexFilesDirectoryPath (uriString &pPath)
{
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wReserved;
    if (isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Error,
                                    " Cannot modify IndexFilesDirectoryPath while file is already open for file  <%s>",
                                    ZDescriptor.URIContent.toString());
            _RETURN_ ZS_INVOP;
            }
    if (!pPath.isDirectory())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_NOTDIRECTORY,
                                    Severity_Error,
                                    " Cannot set IndexFilesDirectoryPath : path is not a valid directory. Content file is <%s>",
                                    ZDescriptor.URIContent.toString());
            _RETURN_ ZS_NOTDIRECTORY;
            }
    wSt=_Base::_open(ZDescriptor,ZRF_Exclusive|ZRF_All,ZFT_ZSMasterFile);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While openning for setting IndexFileDirectoryPath. File <%s>",
                               ZDescriptor.URIContent.toString());
        _RETURN_ wSt;
        }
    ZMCB.IndexFilePath = pPath;

    _Base::setReservedContent(ZMCB._exportMCB(wReserved));
    wSt=_Base::_writeReservedHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot set IndexFilesDirectoryPath / cannot write Reserved header. Content file is <%s>",
                                ZDescriptor.URIContent.toString());
        }
    _Base::_close(ZDescriptor);
    _RETURN_ wSt;
}//setIndexFilesDirectoryPath

ZStatus
ZSMasterFile::setJournalLocalDirectoryPath (uriString &pPath)
{
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wReserved;
    if (isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Severe,
                                    " Cannot modify JournalingLocalDirectoryPath while Journaling is started for file  <%s>",
                                    getURIContent().toString());
            _RETURN_ ZS_INVOP;
            }
    if (!pPath.isDirectory())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_NOTDIRECTORY,
                                    Severity_Error,
                                    " Cannot set JournalingLocalDirectoryPath : path is not a valid directory. Content file is <%s>",
                                    ZDescriptor.URIContent.toString());
            _RETURN_ ZS_NOTDIRECTORY;
            }
    wSt=_Base::_open(ZDescriptor,ZRF_Exclusive|ZRF_All,ZFT_ZSMasterFile);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast("While openning for setting JournalingLocalDirectoryPath. File <%s>",
                               getURIContent().toString());
        _RETURN_ wSt;
        }
    ZMCB.ZJCB->JournalLocalDirectoryPath = pPath;

    _Base::setReservedContent(ZMCB._exportMCB(wReserved));
    wSt=_Base::_writeReservedHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot set IndexFilesDirectoryPath / cannot write Reserved header. Content file is <%s>",
                                getURIContent().toString());
        }
    _Base::_close(ZDescriptor);
    _RETURN_ wSt;
}//setJournalingLocalDirectoryPath


//------------------File Header : updates (read - write) ZMasterControlBlock AND ZSJournalControlBlock if exists
/**
 * @brief ZSMasterFile::zgetJCBfromHeader Accesses File header and updates JournalingControlBlock if any.
 * There is no reverse operation : writting JCB to Header is done using writing whole Control blocks to header file
 * using ZSMasterFile::writeControlBlocks()
 *
 * @return
 */
ZStatus
ZSMasterFile::readJCBfromHeader(void)
{
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wRawMCB;
    wSt=_Base::getReservedBlock(wRawMCB,true);
    if (wSt!=ZS_SUCCESS)
            {
            _RETURN_ wSt;
            }
    _RETURN_ _getJCBfromReserved();
}
/**
 * @brief ZSMasterFile::_getJCBfromReserved updates Journaling control block if any with its content from ZReserved from Filedescriptor.
 *  ZReserved have to be up to date with an already done getReservedBlock().
 * @return
 */
ZStatus
ZSMasterFile::_getJCBfromReserved(void)
{
_MODULEINIT_

    ZSMasterControlBlock* wMCB = (ZSMasterControlBlock*)ZDescriptor.ZReserved.Data;

    if (wMCB->JCBSize<1)  // no journaling
                {_RETURN_ ZS_SUCCESS;} // Beware _RETURN_ is multiple instructions in debug mode
    if (ZMCB.ZJCB==nullptr)
                {
                ZMCB.ZJCB=new ZSJournalControlBlock;
                }
    ZDataBuffer wJCBContent;
    wJCBContent.setData(ZDescriptor.ZReserved.Data+wMCB->JCBOffset,wMCB->JCBSize);
    _RETURN_ ZMCB.ZJCB->_importJCB(wJCBContent);
}



/**
 * @brief ZSMasterFile::writeMasterControlBlock updates ZMasterControlBlock AND ZSJournalControlBlock (if exists)
 * for current ZSMasterFile to ZReserved Header within header file.
 *
 * see @ref ZMasterControlBlock::_exportMCB()
 * see @ref ZSJournalControlBlock::_exportJCB()
 *
 * @return
 */
ZStatus
ZSMasterFile::writeControlBlocks(void)
{
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wReserved;

    ZMCB._exportMCB(wReserved);
    _Base::setReservedContent(wReserved);
    wSt=_Base::_writeReservedHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                wSt,
                                Severity_Error,
                                " Cannot write Reserved Header (ZMasterControlBlock+ZSJournalControlBlock) to file header. Content file is <%s>",
                                getURIContent().toString());
        }
    _RETURN_ wSt;
}//writeControlBlocks

/**
 * @brief ZSMasterFile::readControlBlocks reads ZMasterControlBlock AND ZSJournalControlBlock is exists
 * see @ref ZMasterControlBlock::_importMCB()
 * see @ref ZSJournalControlBlock::_importJCB()
 * @return
 */
ZStatus
ZSMasterFile::readControlBlocks(void)
{
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wReserved;
//ZDataBuffer wRawJCB;

    wSt=_Base::getReservedBlock(wReserved,true);
    if (wSt!=ZS_SUCCESS)
            {
            _RETURN_ wSt;
            }

    wSt=ZMCB._importMCB(wReserved);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While importing ZMCB");
            _RETURN_ wSt;
            }
    if (ZMCB.JCBOffset<1)
            {
            if (ZMCB.ZJCB!=nullptr)
                    delete ZMCB.ZJCB;
             _RETURN_ wSt;
            }
    ZDataBuffer wReservedJCB;
    wReservedJCB.setData(wReserved.Data+ZMCB.JCBOffset,ZMCB.JCBSize);
    _RETURN_ ZMCB.ZJCB->_importJCB(wReservedJCB);

}// readControlBlocks

#ifdef __COMMENT__

//  To add a new index :
//
//       _addIndexField:
//         create all fields definition in key sequence order
//
//       _addIndexKey
//         Create the index block (ZICB)
//
//       _ZSIndexFileCreate
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
#endif // __COMMENT__
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
ZSMasterFile::_addIndexKeyDefinition (ZSIndexControlBlock* pZICB,
                                      ZSKeyDictionary& pZKDic,
                                      utffieldNameString pIndexName,
                                      ZSort_Type pDuplicates)
{
_MODULEINIT_

    pZICB->clear(&ZMCB.MetaDic);
    pZICB->Name = pIndexName;
 //   pZICB.AutoRebuild = pAutoRebuild ;
    //pZICB.KeyType = pKeyType;
    pZICB->Duplicates = pDuplicates ;

    for (long wi = 0; wi<pZKDic.size();wi++)
        {
        pZICB->ZKDic->push(pZKDic[wi]);
        }
        pZICB->ZKDic->_reComputeSize();

        _RETURN_;
}


/**
 * @brief ZSMasterFile::zcreateIndexFile Generates a new index from a description (meaning a new ZRandomFile data + header).
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
 * 2. ZSIndexFile file pathname is not stored but is a computed data from actual ZSMasterFile file pathname.
 *
 * @param[in] pZIFField     Key fields dictionary to create the index with
 * @param[in] pIndexName    User name of the index key as a utfdescString
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zcreateIndex (ZSKeyDictionary &pZIFField,  // contains the description of index key to add (multi fields)
                           utffieldNameString &pIndexName,
//                          bool pAutorebuild,
                           ZSort_Type pDuplicates,
                           bool pBackup)
{
_MODULEINIT_
ZStatus wSt;
ZSIndexControlBlock* wZICB=new ZSIndexControlBlock(&ZMCB.MetaDic);
bool wIsOpen = isOpen();
long wi;
zsize_type wIndexAllocatedSize=0;
uriString wIndexURI;
uriString wIndexFileDirectoryPath;
ZSIndexFile *wIndexObject;
long w1=0,w2=0;

    if (isOpen())
        {
        ZException.setMessage (_GET_FUNCTION_NAME_,
                                 ZS_MODEINVALID,
                                 Severity_Severe,
                                 " ZSMasterFile <%s> is already open : must be closed before calling zcreateIndex",
                                 getURIContent().toString());
        _RETURN_ ZS_MODEINVALID;
        }
    wSt=zopen(ZRF_Exclusive|ZRF_All);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" While creating new Index <%s> for MasterFile <%s>",
                                 pIndexName.toString(),
                                 ZDescriptor.URIContent.toString());
        ZException.setLastSeverity(Severity_Severe);
        _RETURN_ wSt;
        }
 /*       if (!isOpen())
                {
                wSt=zopen(ZRF_Exclusive|ZRF_All);
                }
            else
        {
        if ((getMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All))
                {
                ZException.setMessage (_GET_FUNCTION_NAME_,
                                         ZS_MODEINVALID,
                                         Severity_Error,
                                         " ZSMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                                         getURIContent().toString());
                _RETURN_ ZS_MODEINVALID;
                }
        }//else
*/
// Control index name ambiguity

    if ((wi=ZMCB.Index.zsearchIndexByName(pIndexName.toCString_Strait()))>-1)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVNAME,
                                        Severity_Error,
                                        " Ambiguous index name <%s>. Index name already exist at Index rank <%ld>. Please use zremoveIndex first.\n",
                                        pIndexName.toString(),
                                        wi);
                wSt= ZS_INVNAME;
                goto zcreateIndexEnd ;
                }


    _addIndexKeyDefinition ( wZICB,
                             pZIFField,
                             pIndexName,
//                             pKeyType,
//                             pAutorebuild,
                             pDuplicates);



    ZMCB.pushICBtoIndex(wZICB);  // since here any creation error will induce a desctruction of ZMCB.Index.lastIdx()

// instantiation of the ZSIndexFile new structure :
// it gives a pointer to the actual ICB stored in Index vector
//
    wIndexObject = new ZSIndexFile(this,ZMCB.Index.last() );

    ZMCB.IndexObjects.push(wIndexObject); // since here any creation error will induce a desctruction of ZMCB.IndexObjects.lastIdx()

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
                         ZMCB.Index.lastIdx(),
                         pIndexName);
    if (wSt!=ZS_SUCCESS)
                {_RETURN_ wSt;} // Beware _RETURN_ is multiple instructions in debug mode

// compute the allocated size
    w1 = wIndexAllocatedSize =  _Base::getAllocatedBlocks();
    w2 = ZMCB.Index.last()->IndexRecordSize();

    if (_Base::getBlockTargetSize()>0)
                if (_Base::getAllocatedBlocks()>0)
                    wIndexAllocatedSize =  _Base::getAllocatedBlocks() * ZMCB.Index.last()->IndexRecordSize();
//
// Nota Bene : there is no History and Journaling processing for Index Files
//

    wSt =  wIndexObject->zcreateIndex(ZMCB.Index.last(),      // pointer to index control block because ZSIndexFile stores pointer to Father's ICB
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
                {
                goto zcreateIndexError;
                }
    ZMCB.Index.last()->generateCheckSum();
// update MCB to Reserved block in RandomFile header (_Base)
// then write updated Master Control Block to Master Header
//
    wSt= wIndexObject->openIndexFile(wIndexURI,(ZRF_Exclusive | ZRF_All));
    if (wSt!=ZS_SUCCESS)
            {
            goto zcreateIndexError;
            }
    wSt=wIndexObject->zrebuildIndex(ZMFStatistics ,stderr);
    if (wSt!=ZS_SUCCESS)
            {
            goto zcreateIndexError;
            }

zcreateIndexEnd:
    if (!wIsOpen)
            zclose();  // close everything and therefore update MCB in file

    // else All dependent files are open with mode (ZRF_Exclusive | ZRF_All) when _RETURN_ing.

    _RETURN_ wSt;
zcreateIndexError:

    ZMCB.IndexObjects.last()->zclose();
    ZMCB.IndexObjects.pop() ; // destroy the ZSIndexFile object
    ZMCB.popICB(); // destroy created ICB
    ZException.addToLast(" While creating new Index <%s> for MasterFile <%s>",
                             pIndexName.toString(),
                             ZDescriptor.URIContent.toString());
    _RETURN_ wSt;

}//zcreateIndex

/**
 * @brief ZSMasterFile::zcreateIndexFile overload calling zcreateIndexFile primary method
 *
 * This routine will create a new index with the files structures necessary to hold and manage it : a ZSIndexFile object will be instantiated.
 * Headerfile of this new ZRF will contain the Index Control Block (ZICB) describing the key.
 *
 * - generates a new ZSIndexFile object.
 * - gives to it the Key description ZICB and appropriate file parameter.
 * - then calls zcreateIndex of the created object that will
 *    + creates the appropriate file from a ZRandomFile
 *    + writes the ZICB into the header
 * - rebuilds the index from current ZSMasterFile's content
 *
 *@note
 * 1. ZSIndexFile never has journaling nor history function. Everything is managed from Master File.
 * 2. ZSIndexFile file pathname is not stored but is a computed data from actual ZSMasterFile file pathname.
 *
 * @param[in] pZIFField     Key fields dictionary to create the index with
 * @param[in] pIndexName    User name of the index key as a const char *
 * @param[in] pDuplicates   Type of key : Allowing duplicates (ZST_DUPLICATES) or not (ZST_NODUPLICATES)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zcreateIndex (ZSKeyDictionary& pZIFField,
                           const utf8_t* pIndexName,
                           ZSort_Type pDuplicates,
                           bool pBackup)
{
_MODULEINIT_

    utffieldNameString wIndexName;
    wIndexName = pIndexName;
    _RETURN_  zcreateIndex (pZIFField,
                          wIndexName,
                          pDuplicates,
                          pBackup);
} // zcreateIndex



#include <stdio.h>
/**
 * @brief ZSMasterFile::zremoveIndex Destroys an Index definition and its files' content on storage
 *
 *  Removes an index.
 *
 *      renames all index files of rank greater than the current on to make them comply with ZSIndexFile naming rules.
 *
 *
 * @param pIndexRank Index rank to remove from ZSMasterFile
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zremoveIndex (const long pIndexRank)
{
_MODULEINIT_

ZStatus wSt;
zmode_type wMode = ZDescriptor.Mode;
uriString FormerIndexContent;
uriString FormerIndexHeader;
uriString NewIndexContent;
uriString NewIndexHeader;

int wRet;
    if ((getMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All))
            {
            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_MODEINVALID,
                                     Severity_Error,
                                     " ZSMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                                     getURIContent().toString());
            _RETURN_ ZS_MODEINVALID;
            }
    if ((pIndexRank<0)||(pIndexRank>ZMCB.Index.size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Severe,
                                    " Out of indexes boundaries: rank <%ld>  boundaries [0,%ld] : File <%s>",
                                    pIndexRank,
                                    ZMCB.Index.lastIdx(),
                                    ZDescriptor.URIContent.toString());
            _RETURN_ ZS_OUTBOUND;
            }

    wSt= ZMCB.IndexObjects[pIndexRank]->zclose();  // close ZSIndexFile files to delete
    if (wSt!=ZS_SUCCESS)
                { _RETURN_ wSt;} // Beware _RETURN_ is multiple instructions in debug mode

    wSt=ZMCB.IndexObjects[pIndexRank]->zremoveFile();  // remove the files
    if (wSt!=ZS_SUCCESS)
                { _RETURN_ wSt;} // Beware _RETURN_ is multiple instructions in debug mode

 //   ZMCB.removeICB(pIndexRank); // removes Index stuff IndexObjects suff and deletes appropriately

    ZMCB.Index.erase(pIndexRank);
    ZMCB.IndexObjects.erase(pIndexRank);
    ZMCB.IndexCount= ZMCB.Index.size();

    for (long wi = pIndexRank;wi<ZMCB.Index.size();wi++)
        {

        FormerIndexContent = ZMCB.IndexObjects[wi]->ZDescriptor.URIContent;
        FormerIndexHeader = ZMCB.IndexObjects[wi]->ZDescriptor.URIHeader;

        ZMCB.IndexObjects[wi]->zclose();// close index files before renaming its files

        wSt=generateIndexURI(getURIContent(),ZMCB.IndexFilePath,NewIndexContent,wi,ZMCB.Index[wi]->Name);
        if (wSt!=ZS_SUCCESS)
                {
                _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
                }
        wSt=generateURIHeader(NewIndexContent,NewIndexHeader);
        if (wSt!=ZS_SUCCESS)
                {
                _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
                }
        ZMCB.IndexObjects[wi]->ZDescriptor.URIContent = NewIndexContent;
        ZMCB.IndexObjects[wi]->ZDescriptor.URIHeader = NewIndexHeader;

        wRet=rename(FormerIndexContent.toCString_Strait(),NewIndexContent.toCString_Strait());
        if (wRet)
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_FILEERROR,
                             Severity_Severe,
                             "Cannot rename index content file <%s> to <%s>",
                             FormerIndexContent.toString(),
                             NewIndexContent.toString());
        else
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_SUCCESS,
                                    Severity_Information,
                                    "Index content file <%s> has been renamed to <%s>",
                                    FormerIndexContent.toString(),
                                    NewIndexContent.toString());
            if (ZVerbose)
                    ZException.printLastUserMessage(stderr);
        }

        wRet=rename(FormerIndexHeader.toCString_Strait(),NewIndexHeader.toCString_Strait());
        if (wRet)
            ZException.getErrno(errno,
                             _GET_FUNCTION_NAME_,
                             ZS_FILEERROR,
                             Severity_Severe,
                             "Cannot rename index header file <%s> to <%s>",
                             FormerIndexHeader.toString(),
                             NewIndexHeader.toString());
        else
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_SUCCESS,
                                    Severity_Information,
                                    "Index content file <%s> has been renamed to <%s>",
                                    FormerIndexHeader.toString(),
                                    NewIndexHeader.toString());
            if (ZVerbose)
                    ZException.printLastUserMessage(stderr);
        }
        ZMCB.IndexObjects[wi]->openIndexFile( NewIndexContent,wMode);
        }// for

    ZDataBuffer wMCBContent;
    _RETURN_  _Base::updateReservedBlock(ZMCB._exportMCB(wMCBContent),true);
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
ZSMasterFile::zclearMCB (FILE* pOutput)
{
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wMCBContent;

FILE* wOutput=nullptr;
bool FOutput=false;
utfdescString wBase;


   wOutput=pOutput;
   if (pOutput==nullptr)
       {
//       utfdescString wDInfo;
       wBase=ZDescriptor.URIContent.getBasename().toCChar();
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

    if ((getMode()&(ZRF_Exclusive|ZRF_All))!=(ZRF_Exclusive|ZRF_All))
            {
            fprintf(wOutput,
                    "%s>>  ZSMasterFile <%s> is open in bad mode for zcreateIndex. Must be (ZRF_Exclusive|ZRF_All) or closed",
                     _GET_FUNCTION_NAME_,
                    getURIContent().toString());

            ZException.setMessage (_GET_FUNCTION_NAME_,
                                     ZS_MODEINVALID,
                                     Severity_Error,
                                     " ZSMasterFile <%s> is open in bad mode. Must be (ZRF_Exclusive|ZRF_All) or closed",
                                     ZDescriptor.URIContent.toString());
            _RETURN_ ZS_MODEINVALID;
            }


    if (ZVerbose)
        {
        fprintf (wOutput,
                 "%s>>      Clearing ZMasterControlBlock of file <%s>\n"
                 "              Actual content\n",
                 _GET_FUNCTION_NAME_,
                 ZDescriptor.URIContent.toString());
        ZMCB.report(pOutput);
        }

    while (ZMCB.Index.size()>0)
            {
            wSt=zremoveIndex(ZMCB.Index.lastIdx());
            if (wSt!=ZS_SUCCESS)
                {
                if (ZVerbose)
                    {
                    fprintf (wOutput,
                             "%s>> ****Error: removing index rank <%ld> status <%s> clearing ZMasterControlBlock of file <%s>\n"
                             "              Actual content\n",
                             _GET_FUNCTION_NAME_,
                             ZMCB.IndexObjects.lastIdx(),
                             decode_ZStatus(wSt),
                             ZDescriptor.URIContent.toString());
                    }
                ZException.addToLast(" Index rank <%ld>. Clearing ZMasterControlBlock of file <%s>.",
                                       ZMCB.IndexObjects.lastIdx(),
                                       ZDescriptor.URIContent.toString());
                _RETURN_ wSt;
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
    _RETURN_  _Base::updateReservedBlock(ZMCB._exportMCB(wMCBContent),true);
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
ZSMasterFile::zdowngradeZMFtoZRF (const char* pZMFPath,FILE* pOutput)
{
_MODULEINIT_

ZStatus wSt;
ZSMasterFile wMasterFile;
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
            wMasterFile.ZMCB.Index.size());

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

   wSt=wMasterZRF._open (wMasterZRF.ZDescriptor,ZRF_Exclusive | ZRF_All,ZFT_ZSMasterFile);  // open ZMF using ZRandomFile routines
   if (wSt!=ZS_SUCCESS)
          {
          ZException.exit_abort();
          }

    wMasterZRF.ZDescriptor.ZHeader.FileType = ZFT_ZRandomFile;
    wReservedBlock.clear();
    wMasterZRF.setReservedContent(wReservedBlock);

    fprintf (wOutput,"%s>>  Writing new header to file\n",
            _GET_FUNCTION_NAME_);
    wSt=wMasterZRF._writeFullFileHeader(wMasterZRF.ZDescriptor,true);
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
   _RETURN_;
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
ZSMasterFile::zupgradeZRFtoZMF (const char* pZRFPath,FILE* pOutput)
{
_MODULEINIT_

ZStatus wSt;
ZSMasterFile wMasterFile;
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
    wMasterFile.ZMCB._exportMCB(wReservedBlock);

    wMasterZRF.setReservedContent(wReservedBlock);
    wMasterZRF.ZDescriptor.ZHeader.FileType = ZFT_ZSMasterFile;
    wSt=wMasterZRF._writeFullFileHeader(wMasterZRF.ZDescriptor,true);
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
   _RETURN_;
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
ZSMasterFile::zrepairIndexes (const char *pZMFPath,
                             bool pRepair,
                             bool pRebuildAll,
                             FILE* pOutput)
{
_MODULEINIT_

ZStatus wSt;
ZSMasterFile wMasterFile;
ZRandomFile wMasterZRF;

ZSIndexControlBlock* wZICB;
ZSIndexFile  wIndexFile(&wMasterZRF);
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


   wURIContent = (const utf8_t*)pZMFPath;

   wOutput=pOutput;
   if (pOutput==nullptr)
       {
//       utfdescString wDInfo;
       wBase=wURIContent.getBasename().toCChar();
       wBase+=".repairlog";
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
   fprintf (wOutput,"%s>> starting repairing indexes for ZSMasterFile <%s>  \n"
                    ,
            _GET_FUNCTION_NAME_,
            pZMFPath);


   wSt=wMasterZRF.setPath(wURIContent);
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }
   wSt=wMasterZRF._open (wMasterZRF.ZDescriptor,ZRF_Exclusive | ZRF_All,ZFT_ZSMasterFile,true);  // open ZMF using ZRandomFile routines
   if (wSt!=ZS_SUCCESS)
           {
            goto ErrorRepairIndexes;
           }
   //-------------Must be a ZFT_ZSMasterFile----------------

   if (wMasterZRF.ZDescriptor.ZHeader.FileType!=ZFT_ZSMasterFile)
           {

           fprintf (wOutput,"%s>> **** Fatal error : file is not of mandatory type ZFT_ZSMasterFile but is <%s> ******\n",
                    _GET_FUNCTION_NAME_,
                    decode_ZFile_type( wMasterZRF.ZDescriptor.ZHeader.FileType));
           goto ErrorRepairIndexes;
           }

   //wMasterFile.ZMFURI = wMasterZRF.getURIContent();     // align uris: ZMFURI is getting redundant. Only using ZRandomFile URIContent

   fprintf (wOutput,"%s>> getting ZReservedBlock content and load ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_);

   wSt=wMasterZRF.getReservedBlock(wReservedBlock,true);     // get reserved block content
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }
   wSt=wMasterFile.ZMCB._importMCB(wReservedBlock);     // load ZMCB from reserved block content
   if (wSt!=ZS_SUCCESS)
           {
           ZException.exit_abort();
           }

   fprintf (wOutput,
            "%s>> existing ZSMasterFile index(es)\n"
            "            <%ld>  defined index(es) in ZMasterControlBlock\n",
            _GET_FUNCTION_NAME_,
            wMasterFile.ZMCB.Index.size());

   wMasterFile.ZMCBreport();

/*
 *  for each index
*/
   IndexPresence.allocateCurrentElements(wMasterFile.ZMCB.Index.size());
   IndexPresence.bzero();
   IndexRank=0;
   for (IndexRank=0;IndexRank<wMasterFile.ZMCB.Index.size();IndexRank++)
   {
       wSt=generateIndexURI(wMasterFile.getURIContent(),
                            wMasterFile.ZMCB.IndexFilePath,
                            wIndexUri,
                            IndexRank,
                            wMasterFile.ZMCB.Index[IndexRank]->Name);
       if (wSt!=ZS_SUCCESS)
               {
               _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
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
            //----------------Create a new ZSIndexFile-------------------------


            fprintf (wOutput,
                     "%s>> creating index file\n",
                     _GET_FUNCTION_NAME_);

            wIndexAllocatedSize=0;
            if (wMasterZRF.getBlockTargetSize()>0)
                        if (wMasterZRF.getAllocatedBlocks()>0)
                            wIndexAllocatedSize =  wMasterZRF.getAllocatedBlocks() * wMasterFile.ZMCB.Index[wi]->IndexRecordSize();

            wSt =  wIndexFile.zcreateIndex(wMasterFile.ZMCB.Index[wi],  // pointer to index control block because ZSIndexFile stores pointer to Father's ICB
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

            wSt = wIndexFile.openIndexFile(wIndexUri,ZRF_Exclusive| ZRF_All);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>  ****Error: Unexpected Fatal Error while opening ZSIndexFile index rank <%ld> path <%s> ****\n",
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
                             "%s>>  ****Unexpected Fatal Error while rebuilding ZSIndexFile index rank <%ld> path <%s> *******\n",
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

            //----------------End Create a new ZSIndexFile-------------------------
            continue;
            }  // IndexUri does not exist

//---------- wIndexUri exists-----------------
        fprintf (wOutput,"%s>>  Opening ZSIndexFile\n",
                 _GET_FUNCTION_NAME_);


        wSt=wIndexZRF.setPath(wIndexUri);
        if (wSt!=ZS_SUCCESS)
                {
                goto ErrorRepairIndexes;
                }
        wSt=wIndexZRF._open(wIndexZRF.ZDescriptor,ZRF_Exclusive | ZRF_All,ZFT_ZSIndexFile,true);
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

                        wIndexZRF.zremoveFile(); // may be not necessary : to be checked

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

                        wIndexZRF.zremoveFile(); // may be not necessary : to be checked

                        IndexRank--;
                        continue;
                        }
                }
        ZSMCBOwnData wMCBOwn;
        wMCBOwn._import(wReservedBlock.Data);

        wSt=wZICB->_importICB(&wMasterFile.ZMCB.MetaDic,wReservedBlock,wMCBOwn.ICBSize,wMCBOwn.ICBOffset);  // load ZICB from reserved block content
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

                        wIndexZRF.zremoveFile(); // may be not necessary : to be checked

                        IndexRank--;
                        continue;
                        }
                }
        fprintf (wOutput,"%s>>  checking ZICB content alignment with ZSMasterFile\n",
                 _GET_FUNCTION_NAME_);

        if (memcmp(wReservedBlock.Data,wMasterFile.ZMCB.Index[IndexRank]->_exportICB(wICBContent).Data,wReservedBlock.Size)==0)
            {
            fprintf (wOutput,
                     "%s>>  ZICB content is aligned with its ZSMasterFile for index rank <%ld> path <%s>\n"
                     "             To rebuild ZSIndexFile content (zrebuid) use Option rebuildAll\n",
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

                 wIndexZRF.zremoveFile();
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

   wMasterZRF._close(wMasterZRF.ZDescriptor);

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
                         wMasterFile.ZMCB.Index[wi]->Name.toString());

                wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput);
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf (wOutput,
                             "%s>>   ****Error while rebuilding index rank <%ld> <%s> \n"
                             "          Status is <%s>\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             wMasterFile.ZMCB.Index[wi]->Name.toString(),
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
    _RETURN_ wSt;

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
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wMCBContent;
//    ZMFURI=pURI;
    wSt=_Base::setPath(pURI);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            _RETURN_ wSt;
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
            _RETURN_ wSt;
            }
    wSt=_Base::_create(ZDescriptor,pInitialSize,ZFT_ZSMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            _RETURN_ wSt;
            }

    ZMCB.MetaDic.clear();
    for (long wi=0;wi<pMetaDic->size();wi++)
                ZMCB.MetaDic.push(pMetaDic->Tab[wi]);
    ZMCB.MetaDic.generateCheckSum();

    ZDescriptor.ZHeader.FileType = ZFT_ZSMasterFile;     // setting ZFile_type

    _Base::setReservedContent(ZMCB._exportMCB(wMCBContent));
//    wSt=_Base::updateReservedBlock(ZMCB._exportMCB());
    wSt=_Base::_writeFullFileHeader(ZDescriptor,true);
    if (wSt!=ZS_SUCCESS)
            {
            _Base::_close(ZDescriptor);
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            _RETURN_ wSt;
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
                ZException.addToLast(" while creating ZSMasterFile %s",
                                       getURIContent().toString());
                _RETURN_ wSt;
                }
        }

    if (pLeaveOpen)
            { _RETURN_ wSt;}


    _RETURN_  zclose();
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
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wMCBContent;
//    ZMFURI=pURI;

    printf ("ZMasterFile::zcreate \n");

    wSt=_Base::setPath (pURI);
    if (wSt!=ZS_SUCCESS)
                {_RETURN_(wSt);}
    _Base::setCreateMinimum(pInitialSize);
    wSt=_Base::_create(ZDescriptor,pInitialSize,ZFT_ZSMasterFile,pBackup,true); // calling ZRF base creation routine and it leave open
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     getURIContent().toString());
            _RETURN_(wSt);
            }
    /* ----------File is left open : so no necessity to open again
    wSt=_Base::_open(ZDescriptor,ZRF_Exclusive | ZRF_Write,ZFT_ZSMasterFile);
    if (wSt!=ZS_SUCCESS)
            {
            ZException.addToLast(" While creating Master file %s",
                                     ZMFURI.toString());
            _RETURN_ wSt;
            }
            */

    ZMCB.MetaDic.clear();
    for (long wi=0;wi<pMetaDic->size();wi++)
                ZMCB.MetaDic.push(pMetaDic->Tab[wi]);

    ZMCB.MetaDic.generateCheckSum();

    ZDescriptor.ZHeader.FileType = ZFT_ZSMasterFile;     // setting ZFile_type (Already done in _create routine)
    if (pLeaveOpen)
            {
            ZMCB._exportMCB(wMCBContent);
            _Base::setReservedContent(wMCBContent);
        //    wSt=_Base::updateReservedBlock(ZMCB._exportMCB());
            wSt=_Base::_writeFullFileHeader(ZDescriptor,true);
            if (wSt!=ZS_SUCCESS)
                    {
                    ZException.addToLast(" While creating Master file %s",
                                             getURIContent().toString());
                    _RETURN_ wSt;
                    }

             _RETURN_ wSt;
            }
    printf ("ZMasterFile::zclose \n");
    zclose(); // updates headers including reserved block
    _RETURN_ wSt;
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
ZSMasterFile::createZKeyByName (const char* pKeyName)
{
_MODULEINIT_

    long wi=0;
    for (;wi<ZMCB.Index.size();wi++)
                    if (ZMCB.Index[wi]->Name == pKeyName)
                                    break;

    if (wi==this->ZMCB.Index.size())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_INVOP,
                                        Severity_Fatal,
                                        " Invalid key name while creating Key structure. Given name is %s", pKeyName);
                ZException.exit_abort();
                }
    _RETURN_ (createZKey(wi));
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
ZSMasterFile::createZKey (const long pKeyNumber)
{
_MODULEINIT_

    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " File must be open to created a ZKey object from file <%s>",
                                ZDescriptor.URIContent.isEmpty()?"Unknown":ZDescriptor.URIContent.toCString_Strait());
        _RETURN_ nullptr;
        }
    if (pKeyNumber> this->ZMCB.Index.size())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                " Invalid key number while creating Key structure. given value is %ld", pKeyNumber);
        _RETURN_ nullptr;
        }
        ZSKey* wKey = new ZSKey(this,pKeyNumber);
    _RETURN_ (wKey);
}


/**
 * @brief ZSMasterFile::zprintIndexFieldValues prints the whole key description and content for ZIndex pIndex and for its rank pIdxRank
 *
 *  pIdxRank is then the key value relative position within the index (as a ZRandomFile)
 *  pIndex is the index number within ZMCB
 *
 * @see ZSIndexFile::zprintKeyFieldsValues()
 *
 * @param[in] pIndex    Number of the index for the ZSMasterFile
 * @param[in] pIdxRank  Logical rank of key record within ZSIndexFile
  * @param[in] pHeader  if set to true then key fields description is printed. False means only values are printed.
  * @param[in] pKeyDump if set to true then index key record content is dumped after the list of its fields values. False means only values are printed.
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zprintIndexFieldValues (const zrank_type pIndex,const zrank_type pIdxRank,bool pHeader,bool pKeyDump,FILE *pOutput)
{
_MODULEINIT_

    if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.lastIdx()))
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_INVOP,
                                            Severity_Error,
                                            " invalid index number %ld. Number of ZIndexes is %ld",
                                            ZMCB.IndexObjects.lastIdx());
                    _RETURN_ ZS_INVOP;
                    }
    _RETURN_ (ZMCB.IndexObjects[pIndex]->zprintKeyFieldsValues(pIdxRank,pHeader,pKeyDump,pOutput));
} //zprintIndexFieldValues


/**
 * @brief ZSMasterFile::zopen opens the ZSMasterFile and all its associated ZSIndexFiles with a mode set to defined mask pMode
 * @param pURI  MasterFile file path to open
 * @param pMode Open mode (mask)
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError

 */
ZStatus
ZSMasterFile::zopen  (const uriString pURI, const int pMode)
{
_MODULEINIT_

ZStatus wSt;
ZDataBuffer wRawMCB;

    wSt=_Base::setPath(pURI);
    wSt=_Base::_open (ZDescriptor,pMode,ZFT_ZSMasterFile);
    if (wSt!=ZS_SUCCESS)
            {
            _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
            }

//    ZMFURI = pURI;

    wSt=_Base::getReservedBlock(wRawMCB,true);
    if (wSt!=ZS_SUCCESS)
            {
            _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
            }

    wSt=ZMCB._importMCB(wRawMCB);
    if (wSt!=ZS_SUCCESS)
                    {_RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode

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
                        {_RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode
                ZMCB.ZJCB->Journal->start();
                }
            else
                {
                if (!ZMCB.ZJCB->Journal->isOpen()) // if journal file is closed then need to re-open and restart
                        {
                        if (ZMCB.ZJCB->Journal->JThread.Created)  // journal file is not open but journaling thread is active
                                {
                                ZMCB.ZJCB->Journal->JThread.kill();
                                }
                        }
                }
        ZDataBuffer wJCBContent;

        wJCBContent.setData(ZDescriptor.ZReserved.Data+ZMCB.JCBOffset,ZMCB.JCBSize);
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

//     Need to create ZSIndexFile object instances and open corresponding ZSIndexFiles for each ZMCB.Index list rank
//
    ZMCB.IndexObjects.clear();
    uriString wIndexUri;
long wi;

    for (wi=0;wi < ZMCB.Index.size();wi++)
            {
            ZSIndexFile* wIndex = new ZSIndexFile (this,ZMCB.Index[wi]);
            ZMCB.IndexObjects.push(wIndex);
            wIndexUri.clear();

            wSt=generateIndexURI(getURIContent(),ZMCB.IndexFilePath,wIndexUri,wi,ZMCB.Index[wi]->Name);
            if (wSt!=ZS_SUCCESS)
                    {
                    _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
                    }
            if (ZVerbose)
                        fprintf(stdout,"Opening Index file <%s>\n",(const char*)wIndexUri.toString());
            wSt=wIndex->openIndexFile(wIndexUri,pMode);
            if (wSt!=ZS_SUCCESS)
                    {
                    ZException.addToLast(" while opening index rank <%ld>", wi);
// In case of open index error : close any already opened index file
//              Then close master content file before _RETURN_ing
                    ZStatus wSvSt = wSt;
                    long wj;
                    for (wj = 0;wj < wi;wj++)
                                    ZMCB.IndexObjects[wj]->zclose();  // use the base ZRandomFile zclose routine
                     _Base::_close(ZDescriptor);

                    _RETURN_ wSt;
                    }
            }// for


    _RETURN_ ZS_SUCCESS;
}// zopen

/**
 * @brief ZSMasterFile::zclose close the ZSMasterFile, and all dependent ZSIndexFiles
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zclose(void)
{
_MODULEINIT_

ZStatus wSt;
ZStatus SavedSt=ZS_SUCCESS;
ZDataBuffer wMCBContent;

// closing index files

    if (!ZDescriptor._isOpen)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Error,
                                " file is not open : cannot close it.");
        _RETURN_ ZS_INVOP;
    }
    for (long wi=0;wi < ZMCB.IndexObjects.size();wi++)
            {
            if ((wSt=ZMCB.IndexObjects[wi]->closeIndexFile())!=ZS_SUCCESS)
                {
                ZException.printUserMessage(stderr);  // error on close is not blocking because we need to close ALL files
                SavedSt = wSt;
                }
            }// for

// flush MCB to file

    wSt=_Base::updateReservedBlock(ZMCB._exportMCB(wMCBContent),true);// force to write

    if (wSt!=ZS_SUCCESS)
                {

                ZException.addToLast( " Writing Reserved header for ZSMasterFile %s",
                                        getURIContent().toString());
                _Base::zclose();
                _RETURN_ wSt;
                }

// releasing index resources

    while (ZMCB.IndexObjects.size()>0)
                            ZMCB.IndexObjects.pop();

    while (ZMCB.Index.size()>0)
                            ZMCB.Index.pop();

// ending journaling process

//    setJournalingOff();  // nope

// closing main ZMF content file

    wSt=_Base::zclose();
    if (wSt!=ZS_SUCCESS)
            {
            _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
            }
    if (SavedSt!=ZS_SUCCESS)
                { _RETURN_ SavedSt;}// Beware _RETURN_ is multiple instructions in debug mode
    _RETURN_ ZS_SUCCESS;
}// zclose



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
ZSMasterFile::zget(ZRecord *pRecord, const zrank_type pZMFRank)
{
_MODULEINIT_

ZStatus wSt;
//    wSt=_Base::zaddWithAddress (pRecord,wAddress);      // record must stay locked until successfull commit for indexes

zrank_type      wZMFIdxCommit;
zaddress_type   wZMFAddress;

ZArray <zrank_type>    IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZSIndexItemList        IndexItemList;      // stores keys description per index processed

ZSIndexItem             *wIndexItem;
zrank_type              wIndex_Rank;
long wi = 0;

    pRecord->init();

    if (!pRecord->testCheckSum())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADCHECKSUM,
                              Severity_Severe,
                              "Record dictionary is not in line with ZSMasterFile dictionary : bad checksum");
        _RETURN_ ZS_BADCHECKSUM;
        }
    wSt=_Base::zget(pRecord->getBaseContent(),pZMFRank);
    if (wSt==ZS_SUCCESS)
            wSt=pRecord->_split();
    _RETURN_ wSt;
}// ZSMasterFile::zget


//----------------Insert sequence------------------------------------------------------

/**
 * @brief ZSMasterFile::zinsert Inserts a new record given by pRecord at position pZMFRank within Master File and updates all indexes

Add operation is done in a two phase commit, so that Master file is only updated when all indexes are updated and ok.
If an error occur, Indexes are rolled back to previous state and Master file resources are freed (rolled back)
If all indexes are successfully updated, then update on Master File is committed .

 if then an error occurred during Master File commit, all indexes are Hard Rolledback, meaning they are physically removed from index file.

* @param[in] pRecord    user record content to insert in a ZDataBuffer
* @param[in] pZMFRank   relative position within the ZSMasterFile to insert the record (@see ZRandomFile
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError


 */
ZStatus
ZSMasterFile::zinsert       (ZRecord* pRecord, const zrank_type pZMFRank)
{
_MODULEINIT_

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

    wSt=_Base::_insert2PhasesCommit_Prepare(_Base::ZDescriptor,pRecord->getBaseContent(),pZMFRank,wZMFIdxCommit,wZMFAddress);  //! prepare the add on Master File, reserve appropriate space, get entry in pool
    if (wSt!=ZS_SUCCESS)
            {
            goto zinsert_error;
            }
//
// update all Indexes
//

    for (wi=0;wi< ZMCB.IndexObjects.size();wi++)
    {
        if (ZVerbose)
        {
            fprintf (stdout,"Index number <%ld>\n",wi);
        }
        wIndexItem = new ZSIndexItem;
        wSt=ZMCB.IndexObjects[wi]->_addKeyValue_Prepare(pRecord,wIndexItem,wIndex_Rank, wZMFAddress);// for indexes don't care about insert, this is an add key value
        if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast("During zinsert operation on index number <%ld>",wi);
                delete wIndexItem;
// on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
                _add_RollbackIndexes (ZMCB, IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
// on error reset ZMF in its original state
                _Base::_add2PhasesCommit_Rollback(_Base::ZDescriptor,wZMFIdxCommit); // do not accept update on Master File and free resources
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
    wSt=_add_CommitIndexes (ZMCB,IndexItemList,IndexRankProcessed) ;
    if (wSt!=ZS_SUCCESS)
        {
        // Soft rollback master update regardless returned ZStatus
        // Nb: Exception is pushed on stack. ZException keeps the last status.
        _Base::_add2PhasesCommit_Rollback(_Base::ZDescriptor,wZMFIdxCommit);

        goto zinsert_error;
        }

// at this stage all indexes have been committed
//         commit for Master file data must be done now
//
    wSt = _Base::_insert2PhasesCommit_Commit(_Base::ZDescriptor,pRecord->getBaseContent(),wZMFIdxCommit);//! accept insert update on Master File
    if (wSt!=ZS_SUCCESS)    //! and if then an error occur : hard rollback all indexes and signal exception
            {
            _add_HardRollbackIndexes (ZMCB, IndexRankProcessed); // indexes are already committed so use hardRollback to counter pass
            }                                                    // don't care about the status . In case of error exception stack will trace it
zinsert__RETURN_:
    if (getJournalingStatus())
        {
        ZMCB.ZJCB->Journal->enqueue(ZJOP_Insert,pRecord->getBaseContent());
        }

zinsert_error:
    _Base::_unlockFile (_Base::ZDescriptor) ; // set Master file unlocked
    IndexItemList.clear();
    _RETURN_ wSt;
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
ZSMasterFile::zadd       (ZRecord* pRecord)
{
_MODULEINIT_

ZStatus wSt;
//    wSt=_Base::zaddWithAddress (pRecord,wAddress);      // record must stay locked until successfull commit for indexes

zrank_type      wZMFIdxCommit;
zaddress_type   wZMFAddress;

ZArray <zrank_type>    IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZSIndexItemList        IndexItemList;      // stores keys description per index processed

ZSIndexItem     *wIndexItem;
zrank_type      wIndex_Rank;
long wi = 0;

    if (!isOpen())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_FILENOTOPEN,
                              Severity_Severe,
                              " File <%s> is not open while trying to access it",
                              getURIContent().toString());
        _RETURN_ ZS_FILENOTOPEN;
        }

    if (!pRecord->testCheckSum())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_BADCHECKSUM,
                              Severity_Severe,
                              "Record dictionary is not in line with ZSMasterFile dictionary : bad checksum");
        _RETURN_ ZS_BADCHECKSUM;
        }

    pRecord->_aggregate();  /* serialize the fields from ZRecord memory storage to a ZDataBuffer record */

//    ZExceptionStack.clear();
//    ZException.getLastStatus() = ZS_SUCCESS;

    wSt=_Base::_add2PhasesCommit_Prepare(_Base::ZDescriptor,
                                         (ZDataBuffer&)pRecord->getBaseContent(),
                                         wZMFIdxCommit,
                                         wZMFAddress);  // prepare the add on Master File, reserve appropriate space, get entry in pool, lock it
    if (wSt!=ZS_SUCCESS)
            {
            goto zadd_error;
            }

// =========== update all defined Indexes ======================

    IndexRankProcessed.clear();

    for (wi=0;wi< ZMCB.IndexObjects.size();wi++)
    {
        if (ZVerbose)
        {
            fprintf (stdout,"Index number <%ld>\n",wi);
        }
        wIndexItem=new ZSIndexItem;
        wSt=ZMCB.IndexObjects[wi]->_addKeyValue_Prepare(pRecord,wIndexItem,wIndex_Rank, wZMFAddress);
        if (wSt!=ZS_SUCCESS)
                {
                ZException.addToLast("During zadd operation on index number <%ld>",wi);
                delete wIndexItem;
// on error Soft rollback all already processed indexes in their original state (IndexRankProcessed heap contains the Index ranks added to Indexes that have been processed)
                _add_RollbackIndexes (ZMCB, IndexRankProcessed); // An additional error during index rollback will pushed on exception stack
// on error reset ZMF in its original state
                _Base::_add2PhasesCommit_Rollback(_Base::ZDescriptor,wZMFIdxCommit); // do not accept update on Master File and free resources
                goto zadd_error;
                }

     IndexRankProcessed.push(wIndex_Rank) ;     // this index has been added to this rank
     IndexItemList.push(wIndexItem);            // with that key content

//        ZMFJournaling.push (ZO_Add,wi,pRecord,wAddress);

    }// main for - ZMCB.IndexObjects.size()

// so far everything when well
//     commit Indexes changes
//      if an error occurs during index commit :
//          indexes are rolled back (soft or hard) appropriately within _add_CommitIndexes
// Nb: Exception(s) is(are) pushed on stack

    wSt=_add_CommitIndexes (ZMCB,IndexItemList,IndexRankProcessed) ;
    if (wSt!=ZS_SUCCESS)
        {
        // Soft rollback master update regardless returned ZStatus
        // Nb: Exception is pushed on stack. ZException keeps the last status.
        _Base::_add2PhasesCommit_Rollback(_Base::ZDescriptor,wZMFIdxCommit);

        goto zadd_error;
        }

// at this stage all indexes have been committed
//         commit for Master file data must be done now
//
    wSt = _Base::_add2PhasesCommit_Commit(_Base::ZDescriptor,pRecord->getBaseContent(),wZMFIdxCommit,wZMFAddress);// accept update on Master File
    if (wSt!=ZS_SUCCESS)    // and if then an error occur : hard rollback all indexes
            {
            _add_HardRollbackIndexes (ZMCB, IndexRankProcessed);// indexes are already committed so use hardRollback regardless returned ZStatus
            }                                                   // don't care about the status . In case of error exception stack will trace it
zadd__RETURN_:
    if (getJournalingStatus())
        {
        ZMCB.ZJCB->Journal->enqueue(ZJOP_Add,pRecord->getBaseContent());
        }
zadd_error:
    _Base::_unlockFile (_Base::ZDescriptor) ; // set Master file unlocked
    IndexItemList.clear();
//    while (IndexItemList.size()>0)
//                delete IndexItemList.popR();

    _RETURN_ wSt;
}// zadd

/** @cond Development */

ZStatus
ZSMasterFile::_add_CommitIndexes (ZSMasterControlBlock& pZMCB,ZArray <ZSIndexItem*>  &pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed)
{
_MODULEINIT_

ZStatus wSt;
long wj = 0;

 /*   if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_CORRUPTED,
                                Severity_Fatal,
                                " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZSIndexFile objects number <%s>",
                                pZMCB.Index.size(),
                                pZMCB.IndexObjects.size());
        ZException.exit_abort();
    }
*/
//    ZException.clear();
//    ZException.getLastStatus() = ZS_SUCCESS;
    wj=0;

    for (wj=0;wj<pIndexRankProcessed.size();wj++)
            {
//            wSt=pZMCB.IndexObjects[wj]->_addRollBackIndex(pIndexRankProcessed.popR()); //! rollback update on each index concerned

             wSt=pZMCB.IndexObjects[wj]->_addKeyValue_Commit(pIndexItemList[wj],pIndexRankProcessed[wj]);
             if (wSt!=ZS_SUCCESS)
                 {
                 ZException.addToLast("While committing add operation on index(es)");
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to pZMCB.IndexObjects.size() soft rollback

                    for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
                    {
                    pZMCB.IndexObjects[wR]->_addKeyValue_HardRollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                            // regardless ZStatus (exception is on stack)
                    } // for

                    for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
                    {
                    pZMCB.IndexObjects[wR]->_addKeyValue_Rollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                        // regardless ZStatus (exception is on stack)
                    } //for

                    _RETURN_ ZException.getLastStatus(); // _RETURN_ the very last status encountered
                }//wSt
            } // for
   _RETURN_ ZS_SUCCESS;
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
ZSMasterFile::_add_RollbackIndexes (ZSMasterControlBlock& pZMCB, ZArray<zrank_type> &pIndexRankProcessed)
{
_MODULEINIT_

ZStatus wSt;
long wi = 0;
/*    if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZSIndexFile objects number <%s>",
                                        pZMCB.Index.size(),
                                        pZMCB.IndexObjects.size());
                ZException.exit_abort();
            }*/

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_add_RollbackIndexes Soft Rollback of indexes on add \n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexObjects[wi]->_addKeyValue_Rollback(pIndexRankProcessed[wi]); // rollback add on each index concerned
            }//for                                                                  // don't care about ZStatus: exception stack will track

   _RETURN_ ZException.getLastStatus();
} // _add_RollbackIndexes

/**
 * @brief ZSMasterFile::_add_HardRollbackIndexes Hard rollback : counter-pass the operation done to restore indexes in their previous state
 * @param pZMCB
 * @param pIndexRankProcessed
 * @return
 */
ZStatus
ZSMasterFile::_add_HardRollbackIndexes (ZSMasterControlBlock& pZMCB, ZArray<zrank_type> &pIndexRankProcessed)
{
_MODULEINIT_

ZStatus wSt;
long wi = 0;
//    ZException.getLastStatus() = ZS_SUCCESS;
    if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZSIndexFile objects number <%s>",
                                        pZMCB.Index.size(),
                                        pZMCB.IndexObjects.size());
                ZException.exit_abort();
            }
    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on add operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexObjects[wi]->_addKeyValue_HardRollback(pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            }// for                                                                     // don't care about ZStatus: exception stack will track

   _RETURN_ ZException.getLastStatus();

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
 * - rebuild for all defined indexes for the ZSMasterFile @see ZSIndexFile::zrebuildIndex()
 *
 * @param[in] pDump this boolean sets (true) the option of having surfaceScan during the progression of reorganization. Omitted by default (false)
 * @param[in] pOutput a FILE* pointer where the reporting will be made. Defaulted to stdout.
  * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zreorgFile (bool pDump,FILE *pOutput)
{
_MODULEINIT_

ZStatus wSt;
long wi = 0;
//ZExceptionMin ZException_sv;
bool wasOpen=false;
bool wgrabFreeSpaceSet = false;

zmode_type wMode = ZRF_Nothing ;
    if (ZDescriptor._isOpen)
             {
             wMode=ZDescriptor.getMode();
             zclose();
             wasOpen=true;
             }
     if ((wSt=zopen(ZDescriptor.URIContent,ZRF_Exclusive|ZRF_Write))!=ZS_SUCCESS)
                                                         {  _RETURN_ wSt;}


     if (!ZDescriptor.ZFCB->GrabFreeSpace)        // activate grabFreeSpace if it has been set on
                 {
     ZDescriptor.ZFCB->GrabFreeSpace=true;
                 wgrabFreeSpaceSet = true;
                 }

    zstartPMSMonitoring();

    wSt = ZRandomFile::_reorgFileInternals(ZDescriptor,pDump,pOutput);

   while (wi < ZMCB.IndexObjects.size())
           {
//            wSt=pZMCB.IndexObjects[wj]->_addRollBackIndex(pIndexRankProcessed.popR()); // rollback update on each index concerned
       wSt=ZMCB.IndexObjects[wi]->zrebuildIndex(false,pOutput); // hard rollback update on each already committed index

       if (wSt!=ZS_SUCCESS)
               {
//               ZException_sv = ZException; // in case of error : store the exception but continue rolling back other indexes
               ZException.addToLast(" during Index rebuild on index <%s> number <%02ld> ",
                                           ZMCB.Index[wi]->Name.toString(),
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
                 ZDescriptor.ZFCB->GrabFreeSpace=false;
                 }
    zclose ();
    if (wasOpen)
            zopen(ZDescriptor.URIContent,wMode);

    _RETURN_ ZException.getLastStatus();

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
ZSMasterFile::zindexRebuild (const long pIndexRank,bool pStat, FILE *pOutput)
{
_MODULEINIT_

    if (!isOpen())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Severe,
                                    " File <%s> must be opened to use this function",
                                    ZDescriptor.URIContent.toString());
            _RETURN_ ZS_INVOP;
            }
    if ((pIndexRank<0)||(pIndexRank>ZMCB.Index.size()))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_OUTBOUND,
                                    Severity_Severe,
                                    " Out of indexes boundaries: rank <%ld>  boundaries [0,%ld] : File <%s>",
                                    pIndexRank,
                                    ZMCB.Index.lastIdx(),
                                    ZDescriptor.URIContent.toString());
            _RETURN_ ZS_OUTBOUND;
            }
 _RETURN_ ZMCB.IndexObjects[pIndexRank]->zrebuildIndex (pStat,pOutput);
}//zindexRebuild


//----------------Remove sequence------------------------------------------------------

ZStatus
ZSMasterFile::zremoveByRank    (const zrank_type pZMFRank)
{

ZRecord wZMFRecord(&ZMCB.MetaDic);

    return _removeByRank (&wZMFRecord,pZMFRank);
} // zremoveByRank

ZStatus
ZSMasterFile::zremoveByRankR     (ZRecord *pZMFRecord,const zrank_type pZMFRank)
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
ZSMasterFile::_removeByRank  (ZRecord *pZMFRecord,
                             const zrank_type pZMFRank)
{
_MODULEINIT_

ZStatus     wSt;

zrank_type              wZMFIdxCommit;
zaddress_type           wZMFAddress;
ZArray <zrank_type>     IndexRankProcessed; // stores ranks of already processed indexes (rollback purpose)
ZSIndexItemList         IndexItemList;      // stores keys description per index processed (rollback purpose). Must stay as pointer (double free)
ZSIndexItem             *wIndexItem;
ZDataBuffer             wKeyContent;

ZBlock          wBlock;

//---- get Master File header test for lock - if OK lock it

    wSt=_Base::_remove_Prepare(ZDescriptor,pZMFRank,wZMFAddress);  // prepare remove on Master File mark entry in pool as to be deleted
    if (wSt!=ZS_SUCCESS)
            {
            _Base::_remove_Rollback(ZDescriptor,pZMFRank);
            _RETURN_ wSt;
            }
    wZMFIdxCommit = pZMFRank;
    wSt=_Base::_readBlockAt(ZDescriptor,wBlock,wZMFAddress);  // we know that it is locked as ZBS_BeingDeleted
    if (wSt!=ZS_SUCCESS)
            {
            _Base::_remove_Rollback(ZDescriptor,pZMFRank);
            _RETURN_ wSt;
            }

    pZMFRecord->setContent(wBlock.Content);
//    pZMFRecord.setData(wBlock.Content.Data,wBlock.Content.Size);
    wBlock.Content.clear();
//
// update all Indexes
//
    IndexRankProcessed.clear();
    IndexItemList.clear();
    zrank_type wIndex_Rank;
    long wi;

    for (wi=0;wi< ZMCB.IndexObjects.size();wi++)
    {
        wSt = _keyValueExtraction(ZMCB.Index[wi]->ZKDic,pZMFRecord,wKeyContent);
        if (wSt!=ZS_SUCCESS)
                        goto _removeByRank__RETURN_;
            wIndexItem = new ZSIndexItem ;
            wSt=ZMCB.IndexObjects[wi]->_removeKeyValue_Prepare(wKeyContent,
                                                                wIndexItem,
                                                                wIndex_Rank,
                                                                wZMFAddress);
            if (wSt!=ZS_SUCCESS)
                {
                delete wIndexItem; // free memory for errored key value
                // on error reset all already processed indexes in their original state
                // (IndexRankProcessed heap contains the Index ranks added)
                // An additional error during index rollback will be put on exception stack

                _remove_RollbackIndexes (ZMCB, IndexRankProcessed); // do not care about ZStatus : exception will be on stack

                // on error reset ZMF in its original state
                _Base::_remove_Rollback(ZDescriptor,wZMFIdxCommit); // do not accept update on Master File and free resources

//                if (ZMCB.HistoryOn)
//                      ZMFHistory.push (ZO_Add,wi,wAddress,wSt); //! journalize Error on index if journaling is enabled

               // _Base::_unlockFile (ZDescriptor) ; // unlock done in remove rollback

                goto _removeByRank__RETURN_; ;   // and send appropriate status
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

                _Base::_remove_Rollback(ZDescriptor,wZMFIdxCommit); // an error occurred on indexes : rollback all MasterFile
               // _Base::_unlockFile (ZDescriptor) ; // unlock is done in rollback routine
                goto _removeByRank__RETURN_;// if an error occurs here : this is a severe error that only may be caused by an HW or IO subsystem failure.
                }

    wSt = _Base::_remove_Commit(ZDescriptor,wZMFIdxCommit);// accept update on Master File
    if (wSt!=ZS_SUCCESS)    // and if then an error occur : rollback all indexes and signal exception
            {
            _remove_HardRollbackIndexes (ZMCB, IndexItemList,IndexRankProcessed);    // indexes are already committed so use hardRollback to counter pass
            goto _removeByRank__RETURN_;
            }

    if (getJournalingStatus())
    {
        ZMCB.ZJCB->Journal->enqueue(ZJOP_RemoveByRank,pZMFRecord->getBaseContent(),pZMFRank,wZMFAddress);
    }

_removeByRank__RETURN_:
    _Base::_unlockFile (ZDescriptor) ;
/*
    while (IndexItemList.size()>0)
                  delete IndexItemList.popR();
*/
    IndexItemList.clear();
    _RETURN_ wSt;
}// _removeByRank



/**
 * @brief ZSMasterFile::_remove_CommitIndexes For All ZSIndexFile associated to current ZSMasterFile commits the remove operation
 *
 *
 * @param[in] pZMCB             ZMasterControlBlock owning indexes definitions
 * @param[in] pIndexItemList        List of pointers to ZIndexItem to be removed (Index Key contents)- Necessary in case of Hard Rollback.
 * @param[in] pIndexRankProcessed   List of ranks for ZSIndexFile to be removed
 * @return  a ZStatus value. ZException is set appropriately with error message content in case of error.
 */
ZStatus
ZSMasterFile::_remove_CommitIndexes (ZSMasterControlBlock& pZMCB, ZSIndexItemList & pIndexItemList, ZArray<zrank_type> &pIndexRankProcessed)
{
_MODULEINIT_

ZStatus wSt;
long wj = 0;

//    ZException.getLastStatus() =ZS_SUCCESS;
    for (wj=0;wj<pIndexRankProcessed.size();wj++)
            {
            wSt=pZMCB.IndexObjects[wj]->_removeKeyValue_Commit(pIndexRankProcessed[wj]);
            if (wSt!=ZS_SUCCESS)
                {
//  hard roll back already processed indexes
//  and soft rollback not yet processed indexes
//  wj = errored index rank : up to wj : hardRollback - from wj included to pZMCB.IndexObjects.size() soft rollback

                for (long wR=0;wR < wj;wR++) // Hard rollback for already committed indexes
                {
                pZMCB.IndexObjects[wR]->_removeKeyValue_HardRollback(pIndexItemList[wR] ,pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                        // regardless ZStatus (exception is on stack)
                } // for

                for (long wR = wj;wR<pIndexRankProcessed.size();wR++) // soft rollback
                {
                pZMCB.IndexObjects[wR]->_removeKeyValue_Rollback(pIndexRankProcessed[wR]); // rollback update on each index concerned
                                                                                    // regardless ZStatus (exception is on stack)
                }// for

            _RETURN_ ZException.getLastStatus(); // _RETURN_ the very last status encountered
            }//wSt
    } // Main for

   _RETURN_ ZException.getLastStatus();
} // _remove_CommitIndexes


ZStatus
ZSMasterFile::_remove_RollbackIndexes (ZSMasterControlBlock& pZMCB, ZArray<zrank_type> &pIndexRankProcessed)
{
_MODULEINIT_

ZStatus wSt;
long wi = 0;
        if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
                {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_CORRUPTED,
                                            Severity_Fatal,
                                            " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZSIndexFile objects number <%s>",
                                            pZMCB.Index.size(),
                                            pZMCB.IndexObjects.size());
                    ZException.exit_abort();
                }

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (ZVerbose)
            fprintf(stderr,"_remove_RollbackIndexes Soft Rollback of indexes on remove\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexObjects[wi]->_removeKeyValue_Rollback(pIndexRankProcessed[wi]);  // rollback remove on each index concerned
            }// for                                                                     // don't care about ZStatus: exception stack will track

   _RETURN_ ZException.getLastStatus();
} // _remove_RollbackIndexes


ZStatus
ZSMasterFile::_remove_HardRollbackIndexes (ZSMasterControlBlock& pZMCB,
                                           ZArray<ZSIndexItem*> &pIndexItemList,
                                           ZArray<zrank_type> &pIndexRankProcessed)
{
_MODULEINIT_

long wi = 0;

//    ZException.getLastStatus() = ZS_SUCCESS;
    if (pZMCB.IndexObjects.size()!=pZMCB.Index.size())
            {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_CORRUPTED,
                                        Severity_Fatal,
                                        " Fatal error (ZICB) Indexes definition number <%s> is not aligned with ZSIndexFile objects number <%s>",
                                        pZMCB.Index.size(),
                                        pZMCB.IndexObjects.size());
                ZException.exit_abort();
            }
    if (ZVerbose)
            fprintf(stderr,"Hard Rollback of indexes on remove operation\n");

    for (wi=0;wi < pIndexRankProcessed.size();wi++)
            {
            pZMCB.IndexObjects[wi]->_removeKeyValue_HardRollback(pIndexItemList[wi],pIndexRankProcessed[wi]); // hard rollback update on each already committed index
            } // for                                                    // don't care about ZStatus: exception stack will track

   _RETURN_ ZException.getLastStatus();
} // _remove_HardRollbackIndexes

//----------------End Remove sequence------------------------------------------------------

#ifdef __COMMENT__
//----------------Search sequence-----------------------------------------------------------
//

ZStatus
ZSMasterFile::zgenerateKeyValueList ( ZDataBuffer& pKey ,long pKeyNumber,int pCount,...)
{
_MODULEINIT_
va_list args;
va_start (args, pCount);
ZArray<void*> wKeyValues;

    for (int wi=0;wi<pCount;wi++)
    {
        wKeyValues.push(va_arg(args,void*));
    }
 va_end(args);
 _RETURN_ (zgenerateKeyValue(ZMCB.Index[pKeyNumber],wKeyValues,pKey));
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
_MODULEINIT_
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
                    _RETURN_ ZS_INVOP;
                   }

    pKey.clearData();
    pKey.allocate(pICB.IndexRecordSize()+1);
    memset (pKey.Data,0,pICB.IndexRecordSize());

    for (long wi=0;wi<pKeyValues.size();wi++)
        {
        memmove (pKey.Data+wKeyOffset,pKeyValues[wi],pICB.ZKDic->Tab[wi].NaturalSize);

        wKeyOffset += pICB.ZKDic->Tab[wi].NaturalSize;
        } // for

    _RETURN_  ZS_SUCCESS;
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
ZSMasterFile::zsearch(ZDataBuffer &pRecord,ZSKey *pKey)
{

    return (zsearch(pRecord,(ZDataBuffer &)*pKey,pKey->IndexNumber));
}
/**
 * @brief ZSMasterFile::zsearch searches for a single key value using a ZDataBuffer containing key value to search
 * Key to search for has to be previously formatted to index internal format using appropriate routines
 * @copydetail ZSIndexFile::_search()
 *
 * @param[out] pRecord
 * @param[in] pKeyValue
 * @param[in] pIndexNumber
 * @return
 */
ZStatus
ZSMasterFile::zsearch (ZDataBuffer &pRecord,ZDataBuffer &pKeyValue,const long pIndexNumber)
{
_MODULEINIT_

ZStatus wSt;
//zaddress_type wAddress;
//long wIndexRank;
ZSIndexResult wZIR;
    wSt = ZMCB.IndexObjects[pIndexNumber]->_search(pKeyValue,*ZMCB.IndexObjects[pIndexNumber],wZIR);
    if (wSt!=ZS_FOUND)
            { _RETURN_ wSt;}
    _RETURN_ zgetByAddress(pRecord,wZIR.ZMFAddress);
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
ZSMasterFile::zsearchAll (ZDataBuffer &pKeyValue,
                          const long pIndexNumber,
                          ZSIndexCollection& pIndexCollection,
                          const ZMatchSize_type pZMS)
{
_MODULEINIT_


    _RETURN_ ZMCB.IndexObjects[pIndexNumber]->_searchAll(pKeyValue,
                                                      *ZMCB.IndexObjects[pIndexNumber],
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
ZSMasterFile::zsearchAll (ZSKey &pZKey,ZSIndexCollection& pIndexCollection)
{
_MODULEINIT_

ZStatus wSt;

    pIndexCollection.reset();
    ZMatchSize_type wZSC = ZMS_MatchIndexSize;

    pIndexCollection.ZIFFile = ZMCB.IndexObjects[pZKey.IndexNumber];// assign ZSIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    if (pZKey.FPartialKey)
            wZSC=ZMS_MatchKeySize ;
    wSt = ZMCB.IndexObjects[pZKey.IndexNumber]->_searchAll(pZKey,*ZMCB.IndexObjects[pZKey.IndexNumber],pIndexCollection,wZSC);
    _RETURN_ wSt;
}//zsearchAll

/**
 * @brief ZSMasterFile::zsearchFirst searches for first key value (in index key order) of pKey (see @ref ZKey) and _RETURN_s its corresponding ZSMasterFile record content.
 *
 * a Collection and its context ( see @ref ZSIndexCollection and @ref ZSIndexCollectionContext )
 * is created by 'new' instruction and needs to be mentionned when further calls to ZSMasterFile::zsearchNext() are made.
 *
 * enriched during further zsearchNext accesses - and may be obtained to use Collection facitilies. see @ref Collection.
 *
 * Collection context is created and maintained within collection. This meta data describes the context of current search.
 *
 *  zsearchFirst uses ZSIndexFile::_searchFirst().
 *
 * @copydoc ZSIndexFile::_searchFirst()
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
ZSMasterFile::zsearchFirst (ZSKey &pZKey,
                           ZDataBuffer& pRecord,
                           ZSIndexCollection *pCollection)
{
_MODULEINIT_

ZStatus wSt;
ZSIndexResult wZIR;

    ZMatchSize_type wZMS = ZMS_MatchIndexSize;


    if (pZKey.FPartialKey)
            wZMS=ZMS_MatchKeySize ;
    wSt = ZMCB.IndexObjects[pZKey.IndexNumber]->_searchFirst(pZKey,
                                                             *ZMCB.IndexObjects[pZKey.IndexNumber],
                                                             pCollection,
                                                             wZIR,
                                                             wZMS);
    if (wSt!=ZS_FOUND)
               { _RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode

    wSt=zgetByAddress(pRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
                {_RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode
    _RETURN_ ZS_FOUND;
}// zsearchFirst
/**
 * @brief ZSMasterFile::zsearchNext
 * @copydoc ZSIndexFile::_searchNext()
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
ZSMasterFile::zsearchNext (ZSKey &pZKey,
                          ZDataBuffer & pRecord,
                          ZSIndexCollection *pCollection)
{
_MODULEINIT_

ZStatus wSt;
ZSIndexResult wZIR;

    if (pCollection==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                "Invalid collection/context : it must have been created using zsearchFirst");
        _RETURN_ ZS_INVOP;
        }

    //pIndexCollection.ZIFFile = ZMCB.IndexObjects[pIndexNumber];// assign ZSIndexFile object to Collection : NB Collection is NOT in charge of opening or closing files

    wSt = ZMCB.IndexObjects[pZKey.IndexNumber]->_searchNext(wZIR,pCollection);
    if (wSt!=ZS_FOUND)
                {_RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode

    wSt=zgetByAddress(pRecord,wZIR.ZMFAddress);
    if (wSt!=ZS_SUCCESS)
                {_RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode
    _RETURN_ ZS_FOUND;
}// zsearchFirst





//----------------End Search sequence----------------------------------------------------


/**
 * @brief ZSMasterFile::getKeyIndexFields _RETURN_s natural field values from a ZIndex key at rank pIndexRank for ZIndex pIndexNumber (key number)
 * @note given rank is relative position of record within ZSIndexFile file and NOT relative position of record within ZSMasterFile file
 * @param[out] pKeyFieldValues   _RETURN_s natural field values concatenated within a ZDataBuffer
 * @param[in] pIndexRank        Index key row
 * @param[in] pIndexNumber      ZIndex number (key number) - in definition order
 * @return a ZStatus
 */
ZStatus
ZSMasterFile::getKeyIndexFields(ZDataBuffer &pKeyFieldValues, const zrank_type pIndexRank, const zrank_type pIndexNumber)
{
_MODULEINIT_

 ZStatus wSt;
 ZDataBuffer wIndexRawContent;
 ZSIndexItem  wIndexContent;
    wSt=ZMCB.IndexObjects[pIndexNumber]->_Base::zget(wIndexRawContent,pIndexRank);
    if (wSt!=ZS_SUCCESS)
                {_RETURN_ wSt;}
    wIndexContent.fromFileKey(wIndexRawContent);
    _RETURN_ ZMCB.IndexObjects[pIndexNumber]->getKeyIndexFields(pKeyFieldValues,wIndexContent.KeyContent);
}

/**
 * @brief getRawIndex   Gets a raw index item (without Endian conversion) corresponding to is IndexNumber and its rank
 * @param pIndexItem    A ZIndexItem with returned index content
 * @param pIndexRank    Rank for index
 * @param pIndexNumber  Index number
 * @return
 */
ZStatus
ZSMasterFile::getRawIndex(ZSIndexItem &pIndexItem,const zrank_type pIndexRank,const zrank_type pIndexNumber)
{
_MODULEINIT_

    ZStatus wSt;
    ZDataBuffer wIndexContent;
    wSt=ZMCB.IndexObjects[pIndexNumber]->_Base::zget(wIndexContent,pIndexRank);
    if (wSt!=ZS_SUCCESS)
                   { _RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode
    pIndexItem.fromFileKey(wIndexContent);
    _RETURN_ wSt;
}

/** @} */ // ZSMasterFileGroup

//----------------------Reports---------------------------------------
/**
 * @brief ZSMasterFile::ZMCBreport Reports the whole content of ZMasterControlBlock : indexes definitions and dictionaries
 */
void ZSMasterFile::ZMCBreport(void)
{
    ZMCB.report();
    return;
}

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
_MODULEINIT_

   ZRandomFile::_writeXML_ZRandomFileHeader(pZMF.ZDescriptor,pOutput);

   fprintf (pOutput,

            "  <ZMasterControlBlock>\n"
            "         <HistoryOn>%s</HistoryOn>\n"
            "         <JournalingOn>%s</JournalingOn>\n"
            "         <IndexFileDirectoryPath>%s</IndexFileDirectoryPath>\n"
            ,

            pZMF.ZMCB.HistoryOn?"true":"false",
            pZMF.getJournalingStatus()?"true":"false",
            pZMF.ZMCB.IndexFilePath.toString()
            );

   fprintf (pOutput,
            "         <!-- Changing IndexRank position in a file description has no impact -->\n"
            "         <Index>\n");
   for (long wi=0;wi<pZMF.ZMCB.Index.size();wi++)
   {
   _writeXML_Index(pZMF,wi,pOutput);
   }//for wi
   fprintf (pOutput,
            "         </Index>\n");
   fprintf (pOutput,
            "  </ZMasterControlBlock>\n");
   _RETURN_;
}//_writeXML_FileHeader

void
ZSMasterFile::_writeXML_Index(ZSMasterFile& pZMF,const long pIndexRank,FILE* pOutput)
{
_MODULEINIT_
    pZMF.ZMCB.Index[pIndexRank]->ZKDic->_reComputeSize();
    fprintf (pOutput,
             "         <IndexRank>\n"
             "           <Rank>%ld</Rank> <!-- not significant -->\n"
             "           <Name>%s</Name> <!-- warning modification of this field implies index duplication see documentation -->\n"
             "           <Duplicates>%s</Duplicates> <!-- warning modification of this field must be cautiously done see documentation -->\n"
             "           <AutoRebuild>%s</AutoRebuild>\n",
             pIndexRank,
             pZMF.ZMCB.Index[pIndexRank]->Name.toString(),
             pZMF.ZMCB.Index[pIndexRank]->Duplicates==ZST_DUPLICATES?"ZST_DUPLICATES":"ZST_NODUPLICATES",
             pZMF.ZMCB.Index[pIndexRank]->AutoRebuild?"true":"false"
             );

 //$$$           _writeXML_KDic(pZMF.ZMCB.Index[pIndexRank].ZKDic,pOutput);

    fprintf (pOutput,
             "         </IndexRank>\n");

    _RETURN_ ;
}//_writeXML_Index

#ifdef __COMMENT__
void
ZSMasterFile::_writeXML_KDic(ZSKeyDictionary* ZKDic,FILE* pOutput)
{
_MODULEINIT_

    ZKDic->_reComputeSize();
    fprintf (pOutput,
             "           <ZKDic>\n"
             "               <NaturalSize>%ld</NaturalSize>   <!-- not modifiable : computed field -->\n"
             "               <UniversalSize>%ld</UniversalSize> <!-- not modifiable : computed field -->\n"
             ,
             ZKDic->NaturalSize,
             ZKDic->UniversalSize
             );
     for (long wd=0;wd<ZKDic->size();wd++) // dictionary detail
             {
    fprintf (pOutput,
             "              <KeyField>\n"
             "                <Rank>%ld</Rank>  <!-- not modifiable : only field position in ZKDic is taken -->\n"
             "                <Name>%s</Name>\n"
             "                <NaturalSize>%ld</NaturalSize>\n"
             "                <UniversalSize>%ld</UniversalSize>\n"
             "                <ArrayCount>%ld</ArrayCount>\n"
             "                <ZType>%s</ZType>   <!-- see ZType_type definition : beware the typos -->\n"
             "                <RecordOffset>%ld</RecordOffset>\n"
             "              </KeyField>\n"
             ,
             wd,
             ZKDic->Tab[wd].Name.toString() ,
             ZKDic->Tab[wd].NaturalSize,
             ZKDic->Tab[wd].UniversalSize,
             ZKDic->Tab[wd].ArrayCount,
             decode_ZType( ZKDic->Tab[wd].ZType),
             ZKDic->Tab[wd].RecordOffset
             );
             }// for
    fprintf (pOutput,
              "          </ZKDic>\n");
    _RETURN_;

}//_writeXML_KDic
void
ZSMasterFile::_writeXML_MetaDic(ZMetaDic* ZMDic,FILE* pOutput)
{
_MODULEINIT_

    fprintf (pOutput,
             "           <ZMetaDic>\n"
             );
     for (long wd=0;wd<ZMDic->size();wd++) // dictionary detail
             {
    fprintf (pOutput,
             "              <Field>\n"
             "                <Rank>%ld</Rank>  <!-- not modifiable : only field position in ZKDic is taken -->\n"
             "                <Name>%s</Name>\n"
             "                <ArrayCount>%d</ArrayCount>\n"
             "                <HeaderSize>%ld</HeaderSize>\n"
             "                <ZType>%s</ZType>   <!-- see ZType_type definition : beware the typos -->\n"
             "              </Field>\n"
             ,
             wd,
             ZMDic->Tab[wd].Name ,
             ZMDic->Tab[wd].ArrayCount,
             ZMDic->Tab[wd].HeaderSize,
             decode_ZType( ZMDic->Tab[wd].ZType)
             );
             }// for
    fprintf (pOutput,
              "          </ZMetaDic>\n");
    _RETURN_;

}//_writeXML_KDic
#endif // __COMMENT__

/** @endcond */ // Development

/**
 * @brief ZSMasterFile::zwriteXML_IndexDefinition Generates the xml definition for current opened ZSMasterFile's Index of rank pIndexRank
 * @param[in]   Index rank to generate the XML definition for
 * @param[out] pOutput defaulted to nullptr. Could be stdout or stderr .
 *             If nullptr, then an xml file is generated named <directory path><base name>-<indexname><pIndexRank>.xml
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zwriteXML_IndexDefinition(const long pIndexRank, FILE *pOutput)
{
_MODULEINIT_

FILE* wOutput=pOutput;
utfdescString OutPath;
    if ((pIndexRank<0)||(pIndexRank>ZMCB.IndexObjects.lastIdx()))
       {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_INVOP,
                               Severity_Severe,
                               "Invalid index number <%ld> requested while index range is [0,%ld]",
                               pIndexRank,
                               ZMCB.IndexObjects.lastIdx());
       _RETURN_ (ZS_INVOP);
       }
    if (pOutput==nullptr)
    {
        uriString uriOutput;
        utfdescString OutBase;
        uriOutput= ZDescriptor.URIContent.toString();
        OutBase = uriOutput.getBasename().toCChar();
        OutPath = uriOutput.getDirectoryPath().toCChar();
        OutPath += OutBase;
        OutPath += "-";
        OutPath += ZMCB.Index[pIndexRank]->Name.toString();
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
                                 OutBase.toString());
                ZException.exit_abort();
                }
    }
    fprintf (wOutput,
             "<?xml version='1.0' encoding='UTF-8'?>\n"
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");

    _writeXML_Index(*this,pIndexRank,wOutput);

    fprintf (wOutput,
             "</zicm>");
    if (pOutput==nullptr)
                fclose(wOutput);

    fprintf (stderr,"Xml index definition file <%s> has been created.\n",
                     OutPath.toString());
    _RETURN_ ZS_SUCCESS;
}//zwriteXML_IndexDefinition

/**
 * @brief ZSMasterFile::zwriteXML_IndexDictionary generates the xml definition of a Key dictionary for the mentionned index rank for current ZMF
 * @param[in]   Index rank to generate the XML definition for
 * @param[out] pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>-<indexname><pIndexRank>-kdic.xml
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zwriteXML_IndexDictionary(const long pIndexRank, FILE *pOutput)
{
_MODULEINIT_

FILE* wOutput=pOutput;
utfdescString OutPath;
    if ((pIndexRank<0)||(pIndexRank>ZMCB.IndexObjects.lastIdx()))
       {
       ZException.setMessage(_GET_FUNCTION_NAME_,
                               ZS_INVOP,
                               Severity_Severe,
                               "Invalid index number <%ld> requested while index range is [0,%ld]",
                               pIndexRank,
                               ZMCB.IndexObjects.lastIdx());
       _RETURN_ (ZS_INVOP);
       }
    if (pOutput==nullptr)
    {
        uriString uriOutput;
        utfdescString OutBase;
        uriOutput= ZDescriptor.URIContent.toString();
        OutBase = uriOutput.getBasename().toCChar();
        OutPath = uriOutput.getDirectoryPath().toCChar();
        OutPath += OutBase;
        OutPath += "-";
        OutPath += ZMCB.Index[pIndexRank]->Name.toString();
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
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");

//$$$ _writeXML_KDic(ZMCB.Index[pIndexRank].ZKDic,wOutput);

    fprintf (wOutput,
             "</zicm>");
    if (pOutput==nullptr)
                fclose(wOutput);

    fprintf (stderr,"Xml key dictionary definition file <%s> has been created.\n",
                     OutPath.toString());
    _RETURN_ ZS_SUCCESS;
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
ZSMasterFile::zwriteXML_FileHeader(FILE *pOutput)
{
_MODULEINIT_

FILE* wOutput=pOutput;
utfdescString OutPath;
    if (pOutput==nullptr)
    {
        uriString uriOutput;
        utfdescString OutBase;
        uriOutput= ZDescriptor.URIContent.toString();
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
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");
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
    _RETURN_;
}//zwriteXML_FileHeader
/**
 * @brief ZSMasterFile::zwriteXML_FileHeader Static function : Generates the xml definition for a ZSMasterFile's header given by it path name pFilePath
 * @note the ZSMasterFile is opened for read only ZRF_Read_Only then closed.
 *
 * @param[in] pFilePath points to a valid file to generate the definition from
 * @param[in] pOutput stdout or stderr . If nullptr, then an xml file is generated named <directory path><base name>.xml
 */
void
ZSMasterFile::zwriteXML_FileHeader(const char* pFilePath,FILE *pOutput)
{
_MODULEINIT_

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
             "<zicm version=" __ZRF_XMLVERSION__ ">\n");

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
    _RETURN_;
}  // static zwriteXML_FileHeader

#ifdef QT_CORE_LIB

/*----------------------------------------------------------------------------------------------
 *  Warning : never use QDomNode::cloneNode (true) . It generates SIGEV after combined 2 usages
 * ---------------------------------------------------------------------------------------------
*/
/** @cond Development */
/**
 * @brief ZSMasterFile::_XMLzicmControl Loads an XML document and Makes all XML controls to have an appropriate <zicm> document
 *
 *  Controls are made on
 *  - xml coherence
 *  - zicm version attribute
 *
 *  It delivers the FIRST child node ELEMENT AFTER root node element <zicm>, without controlling either tag name or content
 *
 * @param[in] pFilePath     a Cstring pointing to xml file to load and control
 * @param[out] XmlDoc       The XML document content loaded as a return
 * @param[out] pFirstNode   The First node to exploit as a return
 * @return
 */
ZStatus
ZSMasterFile::_XMLzicmControl(const utf8_t* pFilePath,QDomDocument &XmlDoc,QDomNode &pFirstNode)
{
_MODULEINIT_

uriString wUriPath;
ZDataBuffer wXMLcontent;
ZStatus wSt;
    wUriPath =(const utf8_t*) pFilePath;
//    wUriPath = "/home/gerard/uncryptedparams.xml";
    if (!wUriPath.exists())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTEXIST,
                                    Severity_Error,
                                    " File <%s> does not exist while trying to load it",
                                    pFilePath);
            _RETURN_ ZS_FILENOTEXIST;
            }
    wSt=wUriPath.loadContent(wXMLcontent);
    if (wSt!=ZS_SUCCESS)
                { _RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode
QDomNode wNode;
QString wN;
//QDomDocument XmlDoc;


QString ErrorMsg;
int ErrLine=0;
int ErrColumn=0;
bool Result = true;
bool NameSpaceProcessing = true; //option to be set at a higher level
    XmlDoc.clear();
    Result = XmlDoc.setContent(   wXMLcontent.toQByteArray(),
                                        NameSpaceProcessing,
                                        &ErrorMsg,
                                        &ErrLine,
                                        &ErrColumn) ;

    if (!Result)
                {
                ZException.setComplement(
                        "line %d Column %d Xml error >>",
                        ErrLine,
                        ErrColumn,
                        ErrorMsg.toUtf8().data());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Warning,
                                        " XML Error(s) have been reported : ");
                }


QDomNode wNodeRoot= XmlDoc.firstChildElement("zicm");

    if (wNodeRoot.isNull())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_EMPTYFILE,
                                        Severity_Error,
                                        "In file %s : xml document is empty or errored",
                                        pFilePath);
                _RETURN_ ZS_EMPTYFILE;
                }
QDomElement wElement;
//    wNodeRoot=wNodeRoot.nextSiblingElement(); // first node is never an element
    wElement=wNodeRoot.toElement();
    wN= wElement.tagName();
    if (wN!="zicm")
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "Bad xml document <%s>.Expected first element <zicm> not found. Found <%s>",
                                        pFilePath,
                                        wN.toStdString().c_str());
                _RETURN_ ZS_XMLERROR;
                }
QString wAttribute;
    wAttribute = wElement.attribute("version");
    if (wAttribute!=__ZRF_XMLVERSION_CONTROL__)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "Bad xml document <%s>.Bad zicm version.Expected <2.00>  Found <%s>",
                                pFilePath,
                                wAttribute.toStdString().c_str());
        _RETURN_ ZS_XMLERROR;
        }
    wNode = wNodeRoot.firstChildElement();
    wN= wNode.toElement().tagName(); // debug control of tag name

    if (wNode.isNull())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "Bad xml document <%s>.Empty document no child to <zicm> tag found ",
                                pFilePath);
        _RETURN_ ZS_XMLERROR;
        }


    pFirstNode = wNode;
    _RETURN_ ZS_SUCCESS;
} //_XMLzicmControl

/**
 * @brief ZSMasterFile::_XMLLoadAndControl Loads an XML document and Makes all XML controls to have an appropriate <zicm> document
 *
 *  Controls are made on
 *  - xml coherence
 *  - Root node existence
 *  - Root node tag name validity using pRootName
 *  - Root node Element attribute. Optional control : if pRootAttrName is omitted (nullptr), no control is done.
 *
 *  It delivers the ROOT node ELEMENT after having done the controls
 *
 * @param[in] pFilePath     a Cstring pointing to xml file to load and control
 * @param[out] XmlDoc       The XML document content loaded as a _RETURN_
 * @param[out] pNodeRoot   The First node under the root name to exploit as a _RETURN_
 * @param[in] pRootName      Root node tag name we are searching for
 * @param[in] pRootAttrName Root node Element attribute name. If nullptr then no control is made.
 * @param[in] pRootAttrValue Root node Element attribute expected value
 * @return
 */
ZStatus
ZSMasterFile::_XMLLoadAndControl(const utf8_t *pFilePath,
                   QDomDocument &XmlDoc,
                   QDomNode & wRootNode,
                   const utf8_t *pRootName,
                   const utf8_t *pRootAttrName,
                   const utf8_t *pRootAttrValue,
                   FILE* pOutput)
{
_MODULEINIT_

uriString wUriPath;
ZDataBuffer wXMLcontent;
ZStatus wSt;

QDomElement wElement;

QString wN;
QString wAttribute;

    wUriPath = (const utf8_t*)pFilePath;

    if (!wUriPath.exists())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_FILENOTEXIST,
                                    Severity_Error,
                                    " File <%s> does not exist while trying to load it",
                                    pFilePath);
            fprintf(pOutput,
                    "%s>> File <%s> does not exist while trying to load it\n",
                    _GET_FUNCTION_NAME_,
                    pFilePath);
            _RETURN_ ZS_FILENOTEXIST;
            }
    if (pOutput)
            fprintf(pOutput,"%s>> loading xml file %s\n",
                    _GET_FUNCTION_NAME_,
                    pFilePath);

    wSt=wUriPath.loadContent(wXMLcontent);
    if (wSt!=ZS_SUCCESS)
            {_RETURN_ wSt;} // Beware _RETURN_ is multiple instructions in debug mode

//QDomDocument XmlDoc;


QString ErrorMsg;
int ErrLine=0;
int ErrColumn=0;
bool Result = true;
bool NameSpaceProcessing = true; //option to be set at a higher level

    XmlDoc.clear();
    Result = XmlDoc.setContent(   wXMLcontent.toQByteArray(),
                                        NameSpaceProcessing,
                                        &ErrorMsg,
                                        &ErrLine,
                                        &ErrColumn) ;

    if (!Result)
                {
                ZException.setComplement(
                        "line %d Column %d Xml error >> %s",
                        ErrLine,
                        ErrColumn,
                        ErrorMsg.toStdString().c_str());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Warning,
                                        " XML Error(s) have been reported : ");
                if (pOutput)
                        ZException.printLastUserMessage(pOutput);
                }// !Result


    wRootNode= XmlDoc.firstChildElement((const char*)pRootName);

    if (wRootNode.isNull())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_EMPTYFILE,
                                        Severity_Error,
                                        "In file %s : xml document is empty or errored",
                                        pFilePath);
                _RETURN_ ZS_EMPTYFILE;
                }

    wElement=wRootNode.toElement();
    wN= wElement.tagName();
    if (wN!=(const char*)pRootName)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "Bad xml document <%s>.Expected first element <%s> not found. Found <%s>",
                                        pFilePath,
                                        pRootName,
                                        wN.toStdString().c_str());
                _RETURN_ ZS_XMLERROR;
                }

    if (pRootAttrName==nullptr)         // if Optional attribute is omitted then no more control
                    { _RETURN_ ZS_SUCCESS;}

    wAttribute = wElement.attribute((const char*)pRootAttrName);
    if (wAttribute!=(const char*)pRootAttrValue)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "Bad xml document <%s>.Bad root node attribute <%s>.Expected <%s>  Found <%s>",
                                pFilePath,
                                pRootAttrName,
                                pRootAttrValue,
                                wAttribute.toStdString().c_str());
        _RETURN_ ZS_XMLERROR;
        }
    if (pOutput)
            fprintf(pOutput,"%s>> load and control on xml file %s has been done successfully\n",
                    _GET_FUNCTION_NAME_,
                    pFilePath);
    _RETURN_ ZS_SUCCESS;
} //_XMLControl


ZStatus
ZSMasterFile::_loadXML_Index(QDomNode &pIndexNode, ZSIndexControlBlock* pZICB,ZMetaDic* pMetaDic)
{
_MODULEINIT_

ZStatus wSt;
QDomNode wNode;
QString wN;
    wNode = pIndexNode.firstChildElement();
    if (wNode.isNull())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document is corrupted.Expected children to <IndexRank> tag : no child found.");
        _RETURN_ ZS_XMLERROR;
        }
    while (!wNode.isNull())
    {
        while (true)
        {
            wN=wNode.toElement().tagName();
            if (wN=="Rank")
            {
            break; // of no use
            }
            if (wN=="Name")
            {
              pZICB->Name=wNode.toElement().text();
              break;
            }
            if (wN=="Duplicates")
            {
              if (wNode.toElement().text()=="ZST_NODUPLICATES")
                        pZICB->Duplicates = ZST_NODUPLICATES ;
                    else
                        pZICB->Duplicates = ZST_DUPLICATES ;
              break;
            }
            if (wN=="AutoRebuild")
            {
            pZICB->AutoRebuild = (wNode.toElement().text()=="true");
            break;
            }
            if (wN=="ZKDic")
            {
            QDomNode wNodeField = wNode;
            pZICB->ZKDic=new ZSKeyDictionary(pMetaDic);
//$$$            wSt=_loadXMLDictionary(wNodeField,pZICB->ZKDic);
            if (wSt!=ZS_SUCCESS)
                        {
                        _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
                        }
            break;
            }

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "XML error unknown tag name found %s  at line %d ",
                                    wN.toStdString().c_str(),
                                    wNode.lineNumber());
            _RETURN_ ZS_XMLERROR;
        }//while true
    wNode=wNode.nextSiblingElement();
    }// while wNode is not null
   _RETURN_ ZS_SUCCESS;
}// _loadXML_Index

/** @endcond */ // Development

/**
 * @brief ZSMasterFile::zloadXML_Index creates a ZIndexControlBlock instance and loads it data from the xml file pointed by pFilePath
 *
 * Created ZIndex
 *
 * @param[in] pFilePath xml file to load
 * @param[out] pZICB the ZIndexControlBlock populated with data contained in xml file
 * @return
 */
ZStatus
ZSMasterFile::zloadXML_Index(const utf8_t *pFilePath, ZSIndexControlBlock &pZICB, ZMetaDic* pMetaDic)
{
_MODULEINIT_

ZStatus wSt;
QString wN;
QDomDocument XmlDoc;
QDomNode wNode;
QDomNode wIndexNode;


    wSt=_XMLzicmControl(pFilePath,XmlDoc,wIndexNode);
    if (wSt!=ZS_SUCCESS)
                { _RETURN_ wSt;}

    if (wIndexNode.toElement().tagName()!="IndexRank")
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_XMLERROR,
                            Severity_Error,
                            "XML error : unknown tag name found %s  at line %d. Tag <IndexRank> was expected",
                            wN.toStdString().c_str(),
                            wNode.lineNumber());
        _RETURN_ ZS_XMLERROR;
        } //  not IndexRank


//    pZICB= new ZIndexControlBlock;

//    while (!wIndexNode.isNull())
//    {
    wNode = wIndexNode;
    wSt=_loadXML_Index(wNode,&pZICB,pMetaDic);
    if (wSt!=ZS_SUCCESS)
                {
//                delete pZICB;
                _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
                }
//    wIndexNode=wIndexNode.nextSibling();
//    }// while wNode is not null


    _RETURN_ ZS_SUCCESS;

}//zloadXML_Index

//----------------------Dictionary and fields load---------------------------

/** @cond Development */
#ifdef __COMMENT__
ZStatus
ZSMasterFile::_loadXMLKeyField(QDomNode& wNode, ZSKeyDictionary *&pZKDic)
{
_MODULEINIT_

ZStatus wSt;
QString wN;
QDomElement wElement;
    wNode = wNode.firstChildElement();

    if (wNode.isNull())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document is corrupted.Expected children to <KeyField> tag : no child found.");
        _RETURN_ ZS_XMLERROR;
        }


    while (!wNode.isNull())
    {
    while (true)
    {
        wElement=wNode.toElement();

        wN=wNode.toElement().tagName();
        if (wN=="Rank")
        {
        break; // of no use
        }
        if (wN=="Name")
        {
        pZKDic->last().Name = wElement.text();
          break;
        }
        if (wN=="NaturalSize")
        {
          getElementUInt64_t (pZKDic->last().NaturalSize ,wElement);
          break;
        }
        if (wN=="UniversalSize")
        {
          getElementUInt64_t (pZKDic->last().UniversalSize ,wElement);
          break;
        }
        if (wN=="ArrayCount")
        {
          getElementUInt32_t (pZKDic->last().ArrayCount ,wElement);
          break;
        }
        if (wN=="RecordOffset")
        {
          getElementUInt64_t (pZKDic->last().RecordOffset ,wElement);
          break;
        }
        if (wN=="ZType")
        {
            ZDataBuffer wS;
            wS=wElement.text();
          wSt=encodeZTypeFromString (pZKDic->last().ZType ,wS);
          if (wSt!=ZS_SUCCESS)
                  {  _RETURN_ wSt;}

          break;
        }
        if (wN=="KeyField")
        {
        QDomNode wFieldNode;
        wFieldNode = wNode; // to keep sibling potential
        pZKDic->newBlankElement();
        wSt=_loadXMLKeyField(wFieldNode,pZKDic);
        if (wSt!=ZS_SUCCESS)
                {
                pZKDic->pop();
                _RETURN_ wSt;
                }
        break;
        }

        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error unknown tag name found %s  at line %d ",
                                wN.toStdString().c_str(),
                                wNode.lineNumber());
        break;
        }//while true

    wNode=wNode.nextSiblingElement();

    }// while wNode is not null

    _RETURN_ ZS_SUCCESS;
}

ZStatus
ZSMasterFile::_loadXMLDictionary(QDomNode& wNode, ZSKeyDictionary *pZKDic)
{
_MODULEINIT_

ZStatus wSt;
QString wN;
QDomElement wElement;
QDomNode wNodeField;
    wNode = wNode.firstChildElement();

    if (wNode.isNull())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document is corrupted.Expected children to <ZKDic> tag : no child found.");
        _RETURN_ ZS_XMLERROR;
        }


    while (!wNode.isNull())
    {
    while (true)
    {
        wElement=wNode.toElement();

        wN=wNode.toElement().tagName();
        if (wN=="NaturalSize")
        {
          getElementSize_t (pZKDic->NaturalSize ,wElement);
          break;
        }
        if (wN=="UniversalSize")
        {
          getElementSize_t (pZKDic->UniversalSize ,wElement);
          break;
        }
        if (wN=="KeyField")
        {
        wNodeField = wNode;
        pZKDic->newBlankElement();
        wSt=_loadXMLKeyField(wNodeField,pZKDic);
        if (wSt!=ZS_SUCCESS)
                {
                pZKDic->pop();
                _RETURN_ wSt;
                }
        break;
        }

        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error unknown tag name found %s  at line %d ",
                                wN.toStdString().c_str(),
                                wNode.lineNumber());
        break;
    }//while true

    wNode=wNode.nextSiblingElement();

    }// while wNode is not null


    _RETURN_ ZS_SUCCESS;
}// loadXMLDictionary

/** @endcond */ // Development

/**
 * @brief ZSMasterFile::zloadXML_Dictionary creates a CZKeyDictionary instance and loads it data from the xml file pointed by pFilePath
 * @param[in] pFilePath xml file to load
 * @param[out] pZKDIC the CZKeyDictionary populated with data contained in xml file
 * @return
 */
ZStatus
ZSMasterFile::zloadXML_Dictionary(const char* pFilePath, ZSKeyDictionary &pZKDIC)
{
_MODULEINIT_

ZStatus wSt;
QString wN;
QDomDocument XmlDoc;
QDomNode wNode;
QDomNode wIndexNode;


    wSt=_XMLzicmControl(pFilePath,XmlDoc,wIndexNode);
    if (wSt!=ZS_SUCCESS)
                {_RETURN_ wSt;}// Beware _RETURN_ is multiple instructions in debug mode

    wN = wIndexNode.toElement().tagName();
    if (wIndexNode.toElement().tagName()!="ZKDic")
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_XMLERROR,
                            Severity_Error,
                            "XML error : unknown tag name found %s  at line %d. Tag <ZKDic> was expected",
                            wN.toStdString().c_str(),
                            wNode.lineNumber());
        _RETURN_ ZS_XMLERROR;
        } //  not IndexRank


//    pZKDIC= new CZKeyDictionary;

//    wIndexNode = wIndexNode.firstChildElement();       // one single index is loaded
//    while (!wIndexNode.isNull())
//    {
    wNode = wIndexNode; // to preserve wIndexNode context for potential next sibling
    wSt=_loadXMLDictionary(wNode,&pZKDIC);
    if (wSt!=ZS_SUCCESS)
                {
//                delete pZKDIC;
                _RETURN_ wSt;
                }
//    wIndexNode=wIndexNode.nextSibling();
//    }// while wNode is not null


    _RETURN_ ZS_SUCCESS;

}//zloadXML_Dictionary




/**
 * @brief ZSMasterFile::zapplyXMLIndexRankDefinition gets an index rank (one key) definition from an xml file given by pXMLPath
 *                      and apply it to the ZSMasterFile given by pContentFilePath.
 * This is a static method.
 *
 *  The index definition is parsed and validated.
 *
 *  The named index may be suppressed if the tag <Remove> following index name is set to value 'true'.
 *  If no index with the given <Name> tag does not exist within ZSMasterFile ZMCB, then it will be created as an index and
 *
 * @param pXMLPath          Path of the xml file containing the index definition.
 * @param pContentFilePath  Path for main content file for the ZSMasterFile to apply the modifications to.
 * @param pRealRun          If set to true, then modifications will be applied. If set to false, this is a test run with no modification.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.\n
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.xmllog
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
 */
ZStatus
ZSMasterFile::zapplyXMLIndexRankDefinition(const char* pXMLPath,
                                          const char *pContentFilePath,
                                          bool pRealRun,
                                          FILE*pOutput)
{
_MODULEINIT_

ZStatus wSt;
uriString URIContent;
uriString wURIOutput;
utfdescString wBase;
FILE* wOutput;
bool FOutput=false;
char    IndexPresence=0;
long    IndexRank=0,
        wAddedIndexes=0 ,
        wDestroyedIndexes = 0,
        wRebuiltIndexes = 0,
        wMissingTags=0 ,
        wMissMandatoryTags=0,
        wIndexProcessed=0;

QDomDocument XmlDoc;
QDomNode wRootNode;
QDomNode wFirstlevelNode;


// 0 : Index to be deleted     1 : Index present    2 : Index added  3 : Index created   4 : Index errored

ZSIndexControlBlock  wZICB;
ZSMasterFile         wMasterFile;


    wOutput=pOutput;
    if (pOutput==nullptr)
        {
        wURIOutput=pXMLPath;
        wBase=wURIOutput.getBasename();
        wBase+=".xmllog";
        wOutput=fopen(wBase.toString(),"w");
        if (wOutput==nullptr)
                    {
            wOutput=stdout;
            fprintf(wOutput," cannot open file <%s> redirected to stdout\n",wBase.toString());
                    }
        else
            {
             FOutput=true;
            }
        }

    fprintf (wOutput,
             "__________________________________________________________\n");
    fprintf (wOutput,
             "%s>> starting processing xml file <%s>  \n"
             "            Run options are \n"
             "                 Real run or test run <%s>\n"
             "                 File to apply        <%s>\n"
                     ,
             _GET_FUNCTION_NAME_,
             pXMLPath,
             pRealRun?"Real Run":"Test Run",
             pContentFilePath);

    if (pRealRun)
            fprintf(wOutput," Real Run : file structure is to be modified  \n");
        else
            fprintf(wOutput,"***test*** no update is done  \n");

    wSt=_XMLLoadAndControl(pXMLPath,XmlDoc,wRootNode,"zicm","version",__ZRF_XMLVERSION_CONTROL__,wOutput);
    if (wSt!=ZS_SUCCESS)
                {
                _RETURN_ wSt;// Beware _RETURN_ is multiple instructions in debug mode
                }

    wFirstlevelNode=wRootNode.firstChildElement("IndexRank");

    if (wFirstlevelNode.isNull())
        {
        fprintf (wOutput,
                 "%s>> **** Error : XML error : corrupted or empty file <%s>. Mandatory tag <File> was expected and not found.****",
                 _GET_FUNCTION_NAME_,
                 pXMLPath);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error : corrupted or empty file <%s>. Mandatory tag <File> was expected and not found.",
                                pXMLPath);
            goto ErrorzapplyXMLIndex;
        }
    URIContent = pContentFilePath;

    if (!pRealRun)
        {
        fprintf (wOutput,"%s>> opening ZSMasterFile in mode ZRF_Read_Only  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Read_Only);  // for test read is enough
        }
    else
        {
        fprintf (wOutput,"%s>> opening ZSMasterFile in mode ZRF_Exclusive | ZRF_Write  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Exclusive | ZRF_All); // must be this way
        }
    if (wSt!=ZS_SUCCESS)
            goto ErrorzapplyXMLIndex;
//-------------------Indexes-----------------------------------------


    while (!wFirstlevelNode.isNull())
        {
//-------------here get index rank and dictionary details and test ------------------------
        fprintf(wOutput,
                "%s>>   getting Index definition\n",
                _GET_FUNCTION_NAME_);

        wIndexProcessed ++;
        wSt= zgetXMLIndexRank(wMasterFile,
                              wZICB,
                              wFirstlevelNode,
                              wMissingTags,
                              wMissMandatoryTags,
                              IndexPresence,
                              IndexRank,
                              wOutput);
        break;

        wFirstlevelNode=wFirstlevelNode.nextSiblingElement("IndexRank");
        IndexRank ++;
        }
BackProcess_zapplyXMLIndex:

    if (wSt!=ZS_SUCCESS)
            goto ErrorzapplyXMLIndex;
    fprintf(wOutput,
            "%s>>     -----------Checking for index to build/rebuild/suppressed-----------------------------\n",
            _GET_FUNCTION_NAME_);

    switch(IndexPresence)
    {
    case 0 :
    {
        fprintf (wOutput,
                 "%s>>          Suppressing Index rank <%ld> <%s> ...",
                 _GET_FUNCTION_NAME_,
                 IndexRank,
                 wZICB.Name.toString());
        if (pRealRun)
            {
             wSt=   wMasterFile.zremoveIndex(IndexRank);
             if (wSt!=ZS_SUCCESS)
                 {
                 fprintf (wOutput,
                          "%s>>  **** Error : an error occurred during index remove. Status is %s. please see error stack dump.",
                          _GET_FUNCTION_NAME_,
                          decode_ZStatus(wSt));
                 goto ErrorzapplyXMLIndex;
                 }
             fprintf (wOutput,
                      " Done\n"
                      "%s>>   Index rank <%ld> <%s> successfully removed.",
                      _GET_FUNCTION_NAME_,
                      IndexRank,
                      wZICB.Name.toString());
            }
            else
            fprintf(wOutput,"\n");
        break;
    }// 0
    case 1 :
    {
        fprintf (wOutput,
                 "%s>>          Nothing to do for Index rank <%ld> <%s>.\n",
                 _GET_FUNCTION_NAME_,
                 IndexRank,
                 wZICB.Name.toString());
        break;
    }
    case 2:
    {
    fprintf(wOutput,
            "%s>>           Index rank <%ld> Name <%s> is to be modified into Master file structure ...",
            _GET_FUNCTION_NAME_,
            IndexRank,
            wMasterFile.ZMCB.Index[IndexRank].Name.toString());

    if (pRealRun)
        {
        wMasterFile.ZMCB.Index[IndexRank].ZKDic->clear();
        wMasterFile.ZMCB.Index[IndexRank].ZKDic->setAllocation(wZICB.ZKDic->size());

        for (long wi = 0;wi< wZICB.ZKDic->size();wi++)
            {
            wMasterFile.ZMCB.Index[IndexRank].ZKDic->push(wZICB.ZKDic->Tab[wi]);
            }//for
        wMasterFile.ZMCB.Index[IndexRank].ZKDic->_reComputeSize();
        fprintf(wOutput, " Done.\n");
        }
        else
            fprintf(wOutput,"\n");

        fprintf(wOutput,
                "%s>>           Index rank <%ld> <%s> : Index control block is to be modified into index file structure ...",
                _GET_FUNCTION_NAME_,
                IndexRank,
                wMasterFile.ZMCB.Index[IndexRank].Name.toString());
        if (pRealRun)
            {
            wMasterFile.ZMCB.IndexObjects[IndexRank]->setICB(&wMasterFile.ZMCB.Index[IndexRank]);
            wSt=wMasterFile.ZMCB.IndexObjects[IndexRank]->writeIndexControlBlock();
            if (wSt!=ZS_SUCCESS)
                {
                fprintf (wOutput,
                         "%s>>  **** Error : an error occurred during index control block update. Status is %s. please see error stack dump.",
                         _GET_FUNCTION_NAME_,
                         decode_ZStatus(wSt));
                goto ErrorzapplyXMLIndex;
                }
            fprintf (wOutput," Done.\n");

            }
            else
            fprintf (wOutput,"\n");

    fprintf(wOutput,
            "%s>>           Index rank <%ld> Name <%s> is to be rebuilt\n",
            _GET_FUNCTION_NAME_,
            IndexRank,
            wMasterFile.ZMCB.Index[IndexRank].Name.toString());
    if (pRealRun)
        {
         wSt=wMasterFile.zindexRebuild(IndexRank,ZMFStatistics,wOutput);
         if (wSt!=ZS_SUCCESS)
             {
             fprintf(wOutput,
                     "%s>>     ***** Error occurred while rebuilding Index rank <%ld> status is <%s> Aborting\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     decode_ZStatus(wSt));
             goto ErrorzapplyXMLIndex;
             }// ! ZS_SUCCESS
        } // pRealRun
    wRebuiltIndexes ++;
    } // case 2

    case 3:
    {
    fprintf (wOutput,
             "%s>>          Index <%s> is to be created and rebuilt.\n",
             _GET_FUNCTION_NAME_,
             wZICB.Name.toString());
    if (pRealRun)
    {
    wZICB.ZKDic->_reComputeSize();
    wSt=wMasterFile.zcreateIndex(*wZICB.ZKDic,
                                 wZICB.Name.toString(),
                                 wZICB.Duplicates,
                                 false);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,
                "%s>> **** Index creation ended with error. Status is <%s>. Index has been removed from file. ****\n",
                 _GET_FUNCTION_NAME_,
                decode_ZStatus(wSt));
        break;
        }
    fprintf(wOutput,
            "%s>>           Index has been created and rebuilt sucessfully.\n",
             _GET_FUNCTION_NAME_);
    }
    wRebuiltIndexes ++;
    break;
    }// case 3

    case 4: // errored
    {
        break;
    }
    default :
        break;

    }// switch
    wMasterFile.zclose();

EndzapplyXMLIndex:

    //-----------------------Final report------------------------------

        fprintf(wOutput,
                "________________%s Report_______________\n"
                "  Missing tags                     %ld\n"
                "  Mandatory tags missing           %ld\n"
                "  Index(es) definition processed   %ld\n"
                "  Added Index(es)                  %ld\n"
                "  Suppressed Index(es)             %ld\n"
                "  Index(es) rebuilt                %ld\n"
                "______________________________________________________\n",
                _GET_FUNCTION_NAME_,
                wMissingTags,
                wMissMandatoryTags,
                wIndexProcessed,
                wAddedIndexes,
                wDestroyedIndexes,
                wRebuiltIndexes);

        ZException.printUserMessage(wOutput);
        ZException.clearStack();

        if (FOutput)
                fclose(wOutput);

    _RETURN_ wSt;

ErrorzapplyXMLIndex:
    fprintf(wOutput,
            "%s>>  *****process ended with errors*****************\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();
    goto EndzapplyXMLIndex;

}//zapplyXMLIndexRankDefinition

/** @cond Development */
ZStatus
ZSMasterFile::zgetXMLIndexRank(ZSMasterFile &wMasterFile,
                              ZSIndexControlBlock &wZICB,
                              QDomNode pNode,
                              long &wMissingTags,
                              long &wMissMandatoryTags,
                              char &IndexPresence,  // 0 : Index to be deleted     1 : Index present but not changed and not to be rebuilt    2 : Index is changed and to be built or rebuilt   3 : Index is to be created  4 : Index is errored
                              long &IndexRank,
                              FILE *wOutput)
{
_MODULEINIT_

ZStatus wSt;
long    wKeyRank = 0;

QString wContent;
QDomNode wFifthlevelNode;
QDomNode wSixthlevelNode;
QDomNode wNodeWork;

messageString wMessage;
utfdescString wTagName;

uint32_t    wZType;
ZDataBuffer wZTypeString;
ZSIndexField_struct  wField;
bool FRemove=false;
utfdescString wTagValue;


    wZICB.clear();

    wNodeWork = pNode;

    wTagName = "Name";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s> - stopping process\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Fatal,
                                "xml document may be corrupted or incomplete.Expected mandatory field <Name> child to <IndexRank> tag : no child found.");
        wMissingTags++;
        IndexPresence = 4;
        _RETURN_ ZS_XMLERROR;
        }

//--------Search for index rank by its name

    IndexRank =  wMasterFile.ZMCB.Index.zsearchIndexByName(wContent.toStdString().c_str());

    wMessage.clear();
    wMessage.add("       parameter <%s>  ",
                 wTagName.toString());
    wMessage.add(" value <%s>\n",
                 wContent.toStdString().c_str());
    fprintf(wOutput,wMessage.toString());
    wZICB.Name = wContent;

    if (IndexRank < 0) // not found
            {
            fprintf(wOutput,
                    "%s>>           Index name <%s> is not found and will be added to ZSMasterFile index at rank <%ld>\n",
                    _GET_FUNCTION_NAME_,
                    wContent.toStdString().c_str(),
                    wMasterFile.ZMCB.Index.size());

            IndexPresence = 3;      // Index will be created(and rebuilt) and does not have to to be rebuilt - pushed on index presence list
            IndexRank=wMasterFile.ZMCB.Index.size();

            }
            else
            {
            IndexPresence = 1; // so far : Index to be kept as is (see later): need to check fields to suppress however
            wZICB.Name = wMasterFile.ZMCB.Index[IndexRank].Name;
            fprintf(wOutput,
                    "%s>>       Found index name <%s> to be modified at Index rank <%ld> \n",
                    _GET_FUNCTION_NAME_,
                    wZICB.Name.toString(),
                    IndexRank);
            }// else

    fprintf(wOutput,wMessage.toString());
//---------Search for tag Remove --------------------------------------
    wTagName = "Remove";
    wSt=getChildElementValue(pNode,wTagName.toString(),wContent,false); // not mandatory
    if (wSt==ZS_SUCCESS)
        {
        wTagValue = wContent;
        wMessage.clear();
        wMessage.add("%s>>        Found tag <%s> value <%s> \n",
                     _GET_FUNCTION_NAME_,
                     wTagName.toString(),
                     wTagValue.toString());

        FRemove = (wTagValue=="true");
        if (FRemove)
            {
            if (IndexPresence==1)
                {
                wMessage.add("%s>>        It is requested to remove index rank <%ld> <%s> \n"
                             "                  Index definition is no more parsed\n",
                             _GET_FUNCTION_NAME_,
                             IndexRank,
                             wZICB.Name.toString());
                IndexPresence = 0;
                fprintf (wOutput,wMessage.toString());
                _RETURN_ ZS_SUCCESS;
                }
            wMessage.add("%s>>   ****Error : It is requested to remove index rank <%ld> <%s> while this index does not exist in ZSMasterFile ****\n"
                         "                  Index definition is no more parsed\n",
                         _GET_FUNCTION_NAME_,
                         IndexRank,
                         wZICB.Name.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_INVOP,
                                    Severity_Error,
                                    "It is requested to remove index rank <%ld> <%s> while this index does not exist in ZSMasterFile\n",
                                    IndexRank,
                                    wZICB.Name.toString());
            fprintf (wOutput,wMessage.toString());
            IndexPresence = 4;
            _RETURN_ ZS_INVOP;
            }
        } // ZS_SUCCESS
//------------ End Tag Remove---------------------------------



//------------------End Index Name------------------------------------------

//--------------------Adding Index---------------------------------
        wTagName = "Duplicates";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> default value <%s> applied\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString(),
                    "ZST_DUPLICATES");
            wMissingTags++;
            }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());

            wZICB.Duplicates = encode_ZST(wContent.toStdString().c_str());

            wMessage.add(" value <%s>\n",
                         decode_ZST(wZICB.Duplicates));// Just to cross-check

            fprintf(wOutput,wMessage.toString());
            }// else---------------------------------------------
        wTagName = "AutoRebuild";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> default value <%s> applied \n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString(),
                    "false");
             wMissingTags++;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",  // field does not exist in file
                         wTagName.toString());
            wMessage.add(" value <%s>\n",
                         wContent.toStdString().c_str());
            fprintf(wOutput,wMessage.toString());
            }// else---------------------------------------------

//----------------ZKDic------------------------------

        wFifthlevelNode = pNode.firstChildElement("ZKDic");

        if (wFifthlevelNode.isNull())
            {
            fprintf(wOutput,
                    "%s>>   ****Error No Dictionary found in Index definition. Rank <%ld> name <%s>. Index definition is corrupted******\n",
                    _GET_FUNCTION_NAME_,
                    IndexRank,
                    wZICB.Name.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** No Dictionary found in Index definition. Rank <%ld> <%s>  **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    IndexRank,
                                    wZICB.Name.toString(),
                                    wTagName.toString()
                                    );
            wMissingTags++;
            wMissMandatoryTags ++;

            IndexPresence = 4;
            _RETURN_ ZS_XMLERROR;
            }// isNull

        wSixthlevelNode = wFifthlevelNode.firstChildElement("KeyField");  // an index must at least have one declared field

        if (wFifthlevelNode.isNull())
            {
            fprintf(wOutput,
                    "%s>>   ****Error No Key field found in Dictionary of Index number <%ld> definition . Index definition is corrupted******\n",
                    _GET_FUNCTION_NAME_,
                    IndexRank);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags ++;

            IndexPresence = 4;
            _RETURN_ ZS_XMLERROR;
            }// isNull

        while (!wSixthlevelNode.isNull())
        {
        memset(&wField,0,sizeof(wField));

        wNodeWork= wSixthlevelNode;

        wTagName = "Name";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>****Warning*** Failed to get parameter <%s>. Field rank <%ld> will miss its name.\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString(),
                    wKeyRank);
             wMissingTags++;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%s>\n",
                         wContent.toStdString().c_str());
            fprintf(wOutput,wMessage.toString());

            wField.Name = wContent;

            }// else-----------------------------------------------
        wTagName = "NaturalSize";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld>\n",
                         wContent.toLong());
            fprintf(wOutput,wMessage.toString());

            wField.NaturalSize = wContent.toLong();
            }// else-----------------------------------------------
        wTagName = "UniversalSize";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;

            IndexPresence = 4;
            _RETURN_ ZS_XMLERROR;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld>\n",
                         wContent.toLong());
            fprintf(wOutput,wMessage.toString());

            wField.UniversalSize = wContent.toLong();
            }// else-----------------------------------------------
        wTagName = "ArrayCount";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;

            IndexPresence = 4;
            _RETURN_ ZS_XMLERROR;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld>\n",
                         wContent.toLong());
            fprintf(wOutput,wMessage.toString());

            wField.ArrayCount = wContent.toLong();
            }// else-----------------------------------------------

        wTagName = "RecordOffset";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;

            IndexPresence = 4;
            _RETURN_ ZS_XMLERROR;
             }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld>\n",
                         wContent.toLong());
            fprintf(wOutput,wMessage.toString());

            wField.RecordOffset = wContent.toLong();
            }// else-----------------------------------------------

        wTagName = "ZType";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Error,
                                    "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                    wTagName.toString(),
                                    IndexRank
                                    );
            wMissingTags++;
            wMissMandatoryTags++;

            IndexPresence = 4;
            _RETURN_ ZS_XMLERROR;
             }
            else
            {
           //---------encode ZType to test it----------------
            wZTypeString= wContent;
            wSt = encodeZTypeFromString(wZType,wZTypeString);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** parameter <%s> is errored with status <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString(),
                        decode_ZStatus(wSt));
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        wSt,
                                        Severity_Error,
                                        "**** **** parameter <%s> is errored with status <%s> Index will not be built for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        decode_ZStatus(wSt),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;

                IndexPresence = 4;
                _RETURN_ ZS_XMLERROR;
                }
                else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            wMessage.add(" value <%ld> <%s>\n",
                         wZType,
                         decode_ZType(wZType));
            fprintf(wOutput,wMessage.toString());

            wField.ZType = wZType;

            }// else
            }// else-----------------------------------------------

        wZICB.ZKDic->push(wField);

        wSixthlevelNode=wSixthlevelNode.nextSiblingElement("KeyField");
        }// ! wSixthlevelNode.isNull

//--------------- End ZKDic--------------------------------------------------------
//--------------------Index Add------------------------------------------------------

    if (IndexPresence == 3)     // Creation then _RETURN_
                    {_RETURN_ wSt; }// Beware _RETURN_ is multiple instructions in debug mode
 //--------Search for first difference in index

    if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->size()!=wZICB.ZKDic->size())
        {
        fprintf (wOutput,
                 "%s>>          Index has been modified and must be rebuilt\n",
                 _GET_FUNCTION_NAME_);
        IndexPresence = 2;
        _RETURN_ wSt;
        }
    for (long wi=0; wi < wZICB.ZKDic->size();wi++)
    {
        if (memcmp(&wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wi],
                   &wZICB.ZKDic->Tab[wi],
                   sizeof(ZSIndexField_struct))!=0)
                    {
                    fprintf (wOutput,
                             "%s>>          Index has been modified and must be rebuilt\n",
                             _GET_FUNCTION_NAME_);
                    IndexPresence =2;
                    _RETURN_ wSt;
                    }
    }//for
    fprintf (wOutput,
             "%s>>          Index has not been modified and do not need to be rebuilt\n",
             _GET_FUNCTION_NAME_);
    _RETURN_ wSt;
}//zgetXMLIndexRank
#endif // #endif // __COMMENT__

/** @endcond */ // Development

/**
 * @brief ZSMasterFile::zapplyXMLFileDefinition Static method that tests or applies an xml file give by pFilePath to change parameters of an existing file
 * @param[in] pXMLPath         a valid file path pointing to an xml file full definition
 * @param[in] pContentFilePath the ZSMasterFile file path to generate the definition for. if omitted (nullptr) then the field <URIContent> from XML definition file will be taken as file path.
 * @param[in] pRealRun         if set to true, then modifications from xml file will be applied to the file.\n
 *                             if set to false, then only a test run with report will be done to evaluate the possible changes.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.\n
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.xmllog
 * @return
 */
ZStatus
ZSMasterFile::zapplyXMLFileDefinition(const utf8_t* pXMLPath,
                                    const utf8_t *pContentFilePath,
                                    bool pRealRun,
                                    FILE*pOutput)
{
_MODULEINIT_

ZStatus wSt;
uriString URIContent;
uriString wURIOutput;
utfdescString wBase;
FILE* wOutput;
bool FOutput=false;

long    IndexRank=0,
        wAddedIndexes=0 ,
        wDestroyedIndexes = 0,
        wRebuiltIndexes = 0,
        wMissingTags=0 ,
        wMissMandatoryTags=0,
        wIndexProcessed=0;

QString wContent;
QDomDocument XmlDoc;
QDomNode wRootNode;
QDomNode wFirstlevelNode;
QDomNode wSecondlevelNode;
QDomNode wThirdlevelNode;
QDomNode wFourthlevelNode;
QDomNode wNodeWork;

utfmessageString wMessage;
utfdescString wTagName;

// 0 : Index to be deleted     1 : Index present but not to be rebuilt    2 : Index to be built or rebuilt -  3 : Index created (therefore rebuilt)
ZArray<char> IndexPresence ;
char wIndexPresence;



ZSMasterFile         wMasterFile;
ZSIndexControlBlock*  wZICB=new ZSIndexControlBlock(&wMasterFile.ZMCB.MetaDic);
ZSKeyDictionary     wZKDic(&wMasterFile.ZMCB.MetaDic);

    wOutput=pOutput;
    if (pOutput==nullptr)
        {
        wURIOutput=(const utf8_t*)pXMLPath;
        wBase=wURIOutput.getBasename().toCChar();
        wBase+=(const utf8_t*)".xmllog";
        wOutput=fopen(wBase.toCChar(),"w");
        if (wOutput==nullptr)
                    {
            wOutput=stdout;
            fprintf(wOutput," cannot open file <%s> redirected to stdout\n",wBase.toString());
                    }
        else
        {
            FOutput=true;
         }
        }

    fprintf (wOutput,
             "__________________________________________________________\n");
    fprintf (wOutput,
             "%s>> starting processing xml file <%s>  \n"
             "            Run options are \n"
             "                 Real run or test run <%s>\n"
             "                 File to apply        <%s>\n"
                     ,
             _GET_FUNCTION_NAME_,
             pXMLPath,
             pRealRun?"Real Run":"Test Run",
             pContentFilePath==nullptr?"to be found in xml file":(const char*)pContentFilePath);

    if (pRealRun)
            fprintf(wOutput," Real Run : file structure is to be modified  \n");
        else
            fprintf(wOutput,"***test*** no update is done  \n");

    wSt=_XMLLoadAndControl(pXMLPath,XmlDoc,wRootNode,(const utf8_t*)"zicm",(const utf8_t*)"version",(const utf8_t*)__ZRF_XMLVERSION_CONTROL__,wOutput);
    if (wSt!=ZS_SUCCESS)
                {
                goto ErrorzapplyXMLFile;
                }

    wFirstlevelNode=wRootNode.firstChildElement("File");

    if (wFirstlevelNode.isNull())
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error : corrupted or empty file <%s>. Mandatory tag <File> was expected and not found.",
                                pXMLPath);
            goto ErrorzapplyXMLFile;
        }

//---------ZRandomFile Content (ZFileDescriptor)------------------------

    wSt=_testXMLZFileDescriptor(wFirstlevelNode,pContentFilePath,(const utf8_t*)"ZFT_ZSMasterFile",URIContent,wOutput);

    fprintf (wOutput,"%s>> starting processing ZSMasterFile <%s>   \n",
             _GET_FUNCTION_NAME_,
             URIContent.toString());


    if (!pRealRun)
        {
        fprintf (wOutput,"%s>> opening ZSMasterFile in mode ZRF_Read_Only  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Read_Only);  // for test read is enough
        }
    else
        {
        fprintf (wOutput,"%s>> opening ZSMasterFile in mode ZRF_Exclusive | ZRF_Write  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Exclusive | ZRF_All); // must be this way
        }

     if (wSt!=ZS_SUCCESS)
             {
             ZException.addToLast(" While applying xml modifications.");
             goto EndzapplyXMLFile;
             }
     fprintf (wOutput,"%s>> file <%s> successfully opened in ZRF_Read_only mode\n",
              _GET_FUNCTION_NAME_,
              URIContent.toString());

//-------------Control FileType------------------------------------

    if (wMasterFile.ZDescriptor.ZHeader.FileType!=ZFT_ZSMasterFile)
            {

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "File type is <%s> while expected <ZFT_ZSMasterFile>.",
                                    decode_ZFile_type(wMasterFile.ZDescriptor.ZHeader.FileType));

            goto ErrorzapplyXMLFile;
            }

//--------------ZFileControlBlock modifiable fields----------------

    wSt=_testXMLZFileControlBlock(wMasterFile.ZDescriptor.ZFCB,wFirstlevelNode,wMissingTags,pRealRun, wOutput);
    if (wSt!=ZS_SUCCESS)
            goto ErrorzapplyXMLFile;

 //----------------ZMasterControlBlock modifiable fields----------------
    fprintf (wOutput,"%s>> processing node <ZMasterControlBlock>  \n",
             _GET_FUNCTION_NAME_);

    wSecondlevelNode = wFirstlevelNode.firstChildElement("ZMasterControlBlock");
    if (wSecondlevelNode.isNull())
        {
        wMasterFile.zclose();
        if (FOutput)
                fclose(wOutput);
        fprintf(wOutput,"   ****Error*** Missing <ZMasterControlBlock> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <ZMasterControlBlock> child to <File> tag : no child found.");
        goto ErrorzapplyXMLFile;
        }// isNull
    wNodeWork = wSecondlevelNode;

    wTagName = (const utf8_t*)"HistoryOn";
    wSt=getChildElementValue(wNodeWork,wTagName.toCString_Strait(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. It will remain unchanged\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.addsprintf("       parameter <%s>  ",
                 wTagName.toCString_Strait());
    if (wMasterFile.ZMCB.HistoryOn==(wContent=="true"))
                wMessage.addsprintf( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.HistoryOn?"true":"false");
            else
                {
                wMessage.addsprintf(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.HistoryOn?"true":"false",
                             wContent=="true"?"true":"false");
                if (pRealRun)
                     {
                     wMasterFile.ZMCB.HistoryOn = (wContent=="true");
                     }
                }
    fprintf(wOutput,wMessage.toCString_Strait());
    }// else field----------------------------------------------------

    wTagName = (const utf8_t*)"IndexFileDirectoryPath";
    wSt=getChildElementValue(wNodeWork,wTagName.toCString_Strait(),wContent,false); // not mandatory tag
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>Warning: Failed to get optional parameter <%s>. It will remain unchanged.\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.addsprintf("       parameter <%s>  ",
                 wTagName.toCString_Strait());
    if (wMasterFile.ZMCB.IndexFilePath.toQString()==wContent)
                wMessage.addsprintf( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.IndexFilePath.toCString_Strait());
            else
                {
                wMessage.addsprintf(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.IndexFilePath.toCString_Strait(),
                             wContent.toStdString().c_str());
                if (pRealRun)
                     {
                     wMasterFile.ZMCB.IndexFilePath.fromQString( wContent);
                     }
                }
    fprintf(wOutput,wMessage.toCString_Strait());
    }// else field----------------------------------------------------

//---------------------------Journaling---------------------------------
/*
    <Journaling>
      <!--	   <JournalingOn>true</JournalingOn>  -->
      <JournalLocalDirectoryPath></JournalLocalDirectoryPath> <!--null: same directory as ZSMasterFile's path  -->
      <JournalingMaxDepth>-1</JournalingMaxDepth> <!-- max number of journaling records to keep. -1 : infinite. in this case need to use purgeJournaling(keepranks) regularly -->
      <JournalRemoteMirroring>
        <JournalHostAddressType>ZHAT_IPV4</JournalHostAddressType> <!-- values ZHAT_IPV4 ZHAT_IPV6-->
        <JournalHostPort> 1156 </JournalHostPort>
        <JournalHostName>JournalServer</JournalHostName>
        <JournalProtocol>ZJP_RPC|ZJP_SSLV5</JournalProtocol> <!--   -->
        <JournalAuthenticate> <JournalAuthenticate>
        <JournalSSLKey>  </JournalSSLKey>
        <JournalSSLVector> </JournalSSLVector>
      </JournalRemoteMirroring>
    </Journaling>
*/
    fprintf (wOutput,"%s>> processing node <Journaling>  \n",
             _GET_FUNCTION_NAME_);


    if (wMasterFile.ZMCB.ZJCB!=nullptr)
            {
        fprintf (wOutput,"%s>> Journaling will be stopped for this file  \n",
                 _GET_FUNCTION_NAME_);
            }
        else
            {
            fprintf (wOutput,"%s>> Journaling data structures will be created for this file  \n",
                     _GET_FUNCTION_NAME_);
            }
    while (true)
    {
    wThirdlevelNode = wSecondlevelNode.firstChildElement("Journaling");
    if (wThirdlevelNode.isNull())
        {
        wMasterFile.zclose();
        if (FOutput)
                fclose(wOutput);
        fprintf(wOutput,"   ****Warning*** No <Journaling> definition for this file******\n Journaling is disabled.\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Warning,
                                "Expected <Journaling> child to <File> tag : no child found. Journaling will not be enabled -or disabled- for this file.");
        break; // out of loop while true
        }// isNull

    ZSJCBOwnData wJCB;
    wNodeWork = wThirdlevelNode;

    wTagName = (const utf8_t*)"JournalLocalDirectoryPath";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s>.JournalLocalDirectoryPath will be set to ZSMasterFile's default directory path\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.addsprintf("       parameter <%s>  ",
                 wTagName.toCString_Strait());
    wJCB.JournalLocalDirectoryPath.fromQString(wContent);
    if (wMasterFile.ZMCB.ZJCB!=nullptr)
                {
        wMessage.addsprintf(" to be created with value <%s>\n",
                     wJCB.JournalLocalDirectoryPath.toCString_Strait());
                }
        else
                {
    if (wMasterFile.ZMCB.ZJCB->JournalLocalDirectoryPath== wJCB.JournalLocalDirectoryPath)
                wMessage.sprintf( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.ZJCB->JournalLocalDirectoryPath.toCString_Strait());
            else
                {
                wMessage.sprintf( " current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.ZJCB->JournalLocalDirectoryPath.toCString_Strait(),
                             wJCB.JournalLocalDirectoryPath.toCString_Strait());

                } // else unchanged
                } // else created
    fprintf(wOutput,wMessage.toCString_Strait());
    }// else field----------------------------------------------------
    wTagName = (const utf8_t*)"JournalKeep";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>Warning: Failed to get optional parameter <%s>. It will remain unchanged or will be set to its default value (-1).\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wJCB.Keep=wContent.toLong();
    wMessage.clear();
    wMessage.addsprintf("       parameter <%s>  ",
                 wTagName.toCString_Strait());
    if (wMasterFile.ZMCB.ZJCB!=nullptr)
                {
        wMessage.addsprintf(" to be created with value <%ld>\n",
                     wJCB.Keep);
                }
        else
        {
    if (wMasterFile.ZMCB.ZJCB->Keep==wJCB.Keep)
                wMessage.addsprintf( " <%ld> ==unchanged== \n",
                              wMasterFile.ZMCB.ZJCB->Keep);
            else
                {
                wMessage.addsprintf(" current <%ld> modified to <%ld>\n",
                             wMasterFile.ZMCB.ZJCB->Keep,
                             wJCB.Keep);
                } // else unchanged
                } // else created
    fprintf(wOutput,wMessage.toCString_Strait());
    }// else field----------------------------------------------------




//-----------------remote mirroring parameters-------------------------
    ZRemoteMirroring* wJRM=nullptr;
    fprintf (wOutput,"%s>> getting remote journaling parameters (if any)\n",
             _GET_FUNCTION_NAME_);

    while (true)
    {
    wFourthlevelNode = wThirdlevelNode.firstChildElement("JournalRemoteMirroring");
    if (wFourthlevelNode.isNull())
        {
        wMasterFile.zclose();
        if (FOutput)
                fclose(wOutput);
        fprintf(wOutput,"   ****Warning*** No <JournalRemoteMirroring> definition for this file******\n Remote Journaling is disabled.\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Warning,
                                "Expected <JournalRemoteMirroring> child to <File> tag : no child found. Remote Journaling will not be enabled -or disabled- for this file.");
        break; // out of loop while true
        }// isNull

    wJRM=new ZRemoteMirroring;

    wTagName = (const utf8_t*)"JournalHostAddressType";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>Warning: Failed to get optional parameter <%s>. It will remain unchanged or will be set to its default value (-1).\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wJRM->AddressType.encode(wContent);
    wMessage.clear();
    wMessage.addsprintf("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.ZJCB->Remote==nullptr)
                {
        wMessage.addsprintf(" to be created with value <%s>\n",
                     wJRM->AddressType.decode());
                }
        else
        {
    if (wMasterFile.ZMCB.ZJCB->Remote->AddressType==wJRM->AddressType)
                wMessage.addsprintf( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.ZJCB->Remote->AddressType.decode());
            else
                {
                wMessage.addsprintf(" current <%ld> modified to <%s>\n",
                             wMasterFile.ZMCB.ZJCB->Remote->AddressType.decode(),
                             wJRM->AddressType.decode());
                } // else unchanged
                } // else created
    fprintf(wOutput,wMessage.toCString_Strait());
    }// else field----------------------------------------------------



    }// while true remote
//----------------end remote mirroring---------------------------------



//---------------Setting Local Journaling Values-----------------------

     if (pRealRun)
     {
     if (wMasterFile.ZMCB.ZJCB!=nullptr)
             {
         fprintf (wOutput,"%s>> stopping journaling for this file  \n",
                  _GET_FUNCTION_NAME_);
             wMasterFile.setJournalingOff();
 //            delete wMasterFile.ZMCB.ZJCB ;
             }
         else
             {
             fprintf (wOutput,"%s>> creating journaling data structures for this file  \n",
                      _GET_FUNCTION_NAME_);
             wMasterFile.ZMCB.ZJCB = new ZSJournalControlBlock;
             wMasterFile.ZMCB.ZJCB->Journal = new ZSJournal(&wMasterFile);
             }
     fprintf (wOutput,"%s>> setting journaling values  \n",
              _GET_FUNCTION_NAME_);

     memmove(wMasterFile.ZMCB.ZJCB,&wJCB,sizeof(wJCB));
     wSt=wMasterFile.writeControlBlocks();
     if (wSt!=ZS_SUCCESS)
             {
             fprintf (wOutput,"    ***** Error setting journaling values **** \n");
             ZException.addToLast("while writting journaling data in file header");
             goto ErrorzapplyXMLFile;
             }
     fprintf (wOutput,"%s>> starting local journaling \n",
              _GET_FUNCTION_NAME_);
     wSt= wMasterFile.setJournalingOn();
     if (wSt!=ZS_SUCCESS)
             {
             fprintf (wOutput,"    ***** Error starting journaling **** \n");
             ZException.addToLast("while starting journaling for file");
             goto ErrorzapplyXMLFile;
             }

     }// pRealRun

    break;
    }// while true journaling
//-------------------End Journaling------------------------------------



//-------------------Indexes-----------------------------------------

    fprintf (wOutput,"%s>> processing node <%s>  \n",
             _GET_FUNCTION_NAME_,
             "Index");

    wZICB->clear(&wMasterFile.ZMCB.MetaDic);
    wZKDic.clear();

    IndexPresence.allocateCurrentElements(wMasterFile.ZMCB.Index.size());
    IndexPresence.bzero(); // equivalent to clearData

    wThirdlevelNode = wSecondlevelNode.firstChildElement("Index");

    if (wThirdlevelNode.isNull())  // this could be the case with 'Super ZRandomFile' (ZRandomFile + Journaling options)
        {
        fprintf(wOutput,"%s>>   ****Warning No Index found in definition ******\n",
                _GET_FUNCTION_NAME_);
        if (wMasterFile.ZMCB.Index.size()>0)
                {
         fprintf(wOutput,"%s>>    ****Found <%ld> existing indexes in file header : they will ALL be deleted  ******\n",
                 _GET_FUNCTION_NAME_,
                 wMasterFile.ZMCB.Index.size());
         wMissingTags++;
                }
        goto BackProcess_zapplyXMLFile;
        }// isNull

    IndexRank = 0; // xml index rank

    wFourthlevelNode = wThirdlevelNode.firstChildElement("IndexRank");

    if (wFourthlevelNode.isNull())
        {
        fprintf(wOutput,"%s>>   ****Error No Index rank found in Index number <%ld> definition . Index definition is corrupted******\n",
                _GET_FUNCTION_NAME_, IndexRank);
        wMissingTags++;
        wMissMandatoryTags ++;
        }// isNull


    while (!wFourthlevelNode.isNull())
        {
//-------------here get index rank and dictionary details and test ------------------------
        fprintf(wOutput,
                "%s>>   getting Index definition \n",
                _GET_FUNCTION_NAME_);

        wIndexProcessed++;

 /*       wSt= zgetXMLIndexRank(wMasterFile,
                              wZICB,
                              wFourthlevelNode,
                              wMissingTags,
                              wMissMandatoryTags,
                              wIndexPresence,
                              IndexRank,
                              wOutput);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get index definition  - stopping process\n",
                    _GET_FUNCTION_NAME_);

            goto ErrorzapplyXMLFile;
            }

        if (wSt!=ZS_SUCCESS)
                goto ErrorzapplyXMLFile;*/
        fprintf(wOutput,
                "%s>>     -----------Checking for index to build/rebuild/suppressed-----------------------------\n",
                _GET_FUNCTION_NAME_);

        switch(wIndexPresence)
        {
        case 0 :
        {
            fprintf (wOutput,
                     "%s>>          Suppressing Index rank <%ld> <%s> ...",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     wZICB->Name.toString());
            if (pRealRun)
                {
                 wSt=   wMasterFile.zremoveIndex(IndexRank);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf (wOutput,
                              "%s>>  **** Error : an error occurred during index remove. Status is %s. please see error stack dump.",
                              _GET_FUNCTION_NAME_,
                              decode_ZStatus(wSt));
                     goto ErrorzapplyXMLFile;
                     }
                 fprintf (wOutput,
                          " Done\n"
                          "%s>>   Index rank <%ld> <%s> successfully removed.",
                          _GET_FUNCTION_NAME_,
                          IndexRank,
                          wZICB->Name.toString());

                 IndexPresence.erase(IndexRank);
                }
                else
                fprintf(wOutput,"\n");
            break;
        }// 0
        case 1 :
        {
            fprintf (wOutput,
                     "%s>>          Nothing to do for Index rank <%ld> <%s>.\n",
                     _GET_FUNCTION_NAME_,
                     IndexRank,
                     wZICB->Name.toString());
            IndexPresence[IndexRank] = 1 ;
            break;
        }
        case 2:
        {
        fprintf(wOutput,
                "%s>>           Index rank <%ld> Name <%s> is to be modified into Master file structure ...",
                _GET_FUNCTION_NAME_,
                IndexRank,
                wMasterFile.ZMCB.Index[IndexRank]->Name.toString());

        IndexPresence[IndexRank] = 2 ; // index modified to be rebuilt

        if (pRealRun)
            {
            wMasterFile.ZMCB.Index[IndexRank]->ZKDic->clear();
            wMasterFile.ZMCB.Index[IndexRank]->ZKDic->setAllocation(wZICB->ZKDic->size());

            for (long wi = 0;wi< wZICB->ZKDic->size();wi++)
                {
                wMasterFile.ZMCB.Index[IndexRank]->ZKDic->push(wZICB->ZKDic->Tab[wi]);
                }//for
            wMasterFile.ZMCB.Index[IndexRank]->ZKDic->_reComputeSize();
            fprintf(wOutput, " Done.\n");
            }
            else
                fprintf(wOutput,"\n");

            fprintf(wOutput,
                    "%s>>           Index rank <%ld> <%s> : Index control block is to be modified into index file structure ...",
                    _GET_FUNCTION_NAME_,
                    IndexRank,
                    wMasterFile.ZMCB.Index[IndexRank]->Name.toString());
            if (pRealRun)
                {
                wMasterFile.ZMCB.IndexObjects[IndexRank]->setICB(wMasterFile.ZMCB.Index[IndexRank]);
                wSt=wMasterFile.ZMCB.IndexObjects[IndexRank]->writeIndexControlBlock();
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf (wOutput,
                             "%s>>  **** Error : an error occurred during index control block update. Status is %s. please see error stack dump.",
                             _GET_FUNCTION_NAME_,
                             decode_ZStatus(wSt));
                    goto ErrorzapplyXMLFile;
                    }
                fprintf (wOutput," Done.\n");

                }
                else
                fprintf (wOutput,"\n");

        fprintf(wOutput,
                "%s>>           Index rank <%ld> Name <%s> is to be rebuilt\n",
                _GET_FUNCTION_NAME_,
                IndexRank,
                wMasterFile.ZMCB.Index[IndexRank]->Name.toString());
        if (pRealRun)
            {
             wSt=wMasterFile.zindexRebuild(IndexRank,ZMFStatistics,wOutput);
             if (wSt!=ZS_SUCCESS)
                 {
                 fprintf(wOutput,
                         "%s>>     ***** Error occurred while rebuilding Index rank <%ld> status is <%s> Aborting\n",
                         _GET_FUNCTION_NAME_,
                         IndexRank,
                         decode_ZStatus(wSt));
                 goto ErrorzapplyXMLFile;
                 }// ! ZS_SUCCESS
            } // pRealRun
        wRebuiltIndexes ++;
        } // case 2

        case 3: // Index rank creation
        {
        fprintf (wOutput,
                 "%s>>          Index <%s> is to be created and rebuilt.\n",
                 _GET_FUNCTION_NAME_,
                 wZICB->Name.toString());


        IndexPresence.push(3);
        if (pRealRun)
        {
        wZICB->ZKDic->_reComputeSize();
        wSt=wMasterFile.zcreateIndex(*wZICB->ZKDic,
                                     wZICB->Name.toString(),
                                     wZICB->Duplicates,
                                     false);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,
                    "%s>> **** Index creation ended with error. Status is <%s>. Index has been removed from file. ****\n",
                     _GET_FUNCTION_NAME_,
                    decode_ZStatus(wSt));
            break;
            }
        fprintf(wOutput,
                "%s>>           Index has been created and rebuilt sucessfully.\n",
                 _GET_FUNCTION_NAME_);

        IndexPresence.last() = 1 ; // rebuilt is done nothing more to do there
        }// pRealRun

        wRebuiltIndexes ++;
        break;
        }// case 3

        case 4: // errored
        {
            fprintf(wOutput,
                    "%s>> **** Index definition for rank <%ld> is errored. Status is <%s>. Index definition is ignored. ****\n",
                     _GET_FUNCTION_NAME_,
                    IndexRank,
                    decode_ZStatus(wSt));
            break;
        }
        default :
            break;

        }// switch


        wFourthlevelNode=wFourthlevelNode.nextSiblingElement("IndexRank");
        }// while !isNull()
//----------------End ZMasterControlBlock modifiable fields----------------

BackProcess_zapplyXMLFile:
//-------------Checking for index to suppress----------------------
    fprintf(wOutput,
            "%s>>     -----------Checking for index to suppress-----------------------------\n",
            _GET_FUNCTION_NAME_);

    for (long wi = 0;wi<IndexPresence.size();wi++)
    {
        if (!IndexPresence[wi])
        {
            fprintf(wOutput,
                    "%s>>     Index rank <%ld> Name <%s> is to be deleted\n",
                    _GET_FUNCTION_NAME_,
                    wi,
                    wMasterFile.ZMCB.Index[wi]->Name.toString());
            wDestroyedIndexes ++;

            if (pRealRun)
                {
                 wSt=wMasterFile.zremoveIndex(wi);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf(wOutput,
                             "%s>>     ***** Error occurred while removing Index rank <%ld> status is <%s>\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             decode_ZStatus(wSt));
                     }// ! ZS_SUCCESS
                 fprintf(wOutput,
                         "%s>>     Index rank <%ld> has been deleted\n",
                         _GET_FUNCTION_NAME_,
                         wi);
                 IndexPresence.erase(wi); // remove the index flag from table
                 wi--;
                } // pRealRun
        }// IndexPresence
    }// for


    fprintf(wOutput,
            "%s>>     -----------End processing index(es) to suppress------------------------\n",
            _GET_FUNCTION_NAME_);

    fprintf(wOutput,
            "%s>>     -----------Checking for index to build/rebuild-----------------------------\n",
            _GET_FUNCTION_NAME_);

    for (long wi = 0;wi<IndexPresence.size();wi++)
    {
        if (IndexPresence[wi] == 2)
        {
            fprintf(wOutput,
                    "%s>>     Index rank <%ld> Name <%s> is to be rebuilt\n",
                    _GET_FUNCTION_NAME_,
                    wi,
                    wMasterFile.ZMCB.Index[wi]->Name.toString());
            if (pRealRun)
                {
                 wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf(wOutput,
                             "%s>>     ***** Error occurred while rebuilding Index rank <%ld> status is <%s> Aborting\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             decode_ZStatus(wSt));
                     goto ErrorzapplyXMLFile;
                     }// ! ZS_SUCCESS
                } // pRealRun
            wRebuiltIndexes ++;
        }
    }

    fprintf(wOutput,
            "%s>>     -----------End processing index(es) to rebuild------------------------\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();

EndzapplyXMLFile:

    //-----------------------Final report------------------------------

    fprintf(wOutput,
            "________________%s Report_______________\n"
            "  Missing tags                     %ld\n"
            "  Mandatory tags missing           %ld\n"
            "  Index(es) definition processed   %ld\n"
            "  Added Index(es)                  %ld\n"
            "  Suppressed Index(es)             %ld\n"
            "  Index(es) rebuilt                %ld\n"
            "______________________________________________________\n",
            _GET_FUNCTION_NAME_,
            wMissingTags,
            wMissMandatoryTags,
            wIndexProcessed,
            wAddedIndexes,
            wDestroyedIndexes,
            wRebuiltIndexes);

    ZException.printUserMessage(wOutput);
    ZException.clearStack();

    if (FOutput)
            fclose(wOutput);

    _RETURN_ wSt;

ErrorzapplyXMLFile:
    fprintf(wOutput,
            "%s>>  *****process ended with errors*****************\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();
    goto EndzapplyXMLFile;

}// zapplyXMLFileDefinition


/**
 * @brief ZSMasterFile::zapplyXMLFileDefinition Static method that tests or applies an xml file give by pFilePath to change parameters of an existing file
 * @param[in] pXMLPath         a valid file path pointing to an xml file full definition
 * @param[in] pContentFilePath the ZSMasterFile file path to generate the definition for. if omitted (nullptr) then the field <URIContent> from XML definition file will be taken as file path.
 * @param[in] pRealRun         if set to true, then modifications from xml file will be applied to the file.\n
 *                             if set to false, then only a test run with report will be done to evaluate the possible changes.
 * @param[in] pOutput defaulted to nullptr. Could be stdout or stderr.\n
 *                    If set to its default value (nullptr), then an xml file is generated with name <directory path><base name>.xmllog
 * @return
 */
ZStatus
ZSMasterFile::zapplyXMLFileDefinition_old(const utf8_t* pXMLPath,
                                    const utf8_t *pContentFilePath,
                                    bool pRealRun,
                                    FILE*pOutput)
{
_MODULEINIT_

ZStatus wSt;
uriString URIContent;
uriString wURIOutput;
utfdescString wBase;
FILE* wOutput;
bool FOutput=false;

long    IndexRank=0,
        wKeyRank=0,
        wAddedIndexes=0 ,
        wDestroyedIndexes = 0,
        wRebuiltIndexes = 0,
        wMissingTags=0 ,
        wMissMandatoryTags=0;

QString wContent;
QDomDocument XmlDoc;
QDomNode wRootNode;
QDomNode wFirstlevelNode;
QDomNode wSecondlevelNode;
QDomNode wThirdlevelNode;
QDomNode wFourthlevelNode;
QDomNode wFifthlevelNode;
QDomNode wSixthlevelNode;
QDomNode wNodeWork;

utfmessageString wMessage;
utfdescString wTagName;

// 0 : Index to be deleted     1 : Index present but not to be rebuilt    2 : Index to be built or rebuilt -  3 : Index created (therefore rebuilt)
ZArray<char> IndexPresence ;

bool        FAddIndex = false;
bool        FAddField = false;
bool        FIndexRebuild = false;
uint32_t    wZType;
ZDataBuffer wZTypeString;


ZSIndexField_struct  wField;
ZSMasterFile         wMasterFile;
ZSKeyDictionary     wZKDic(&wMasterFile.ZMCB.MetaDic);
ZSIndexControlBlock  wZICB(&wMasterFile.ZMCB.MetaDic);
    wOutput=pOutput;
    if (pOutput==nullptr)
        {
        wURIOutput=(const utf8_t*)pXMLPath;
        wBase=wURIOutput.getBasename().toCChar();
        wBase+=(const utf8_t*)".xmllog";
        wOutput=fopen(wBase.toCChar(),"w");
        if (wOutput==nullptr)
                    {
            wOutput=stdout;
            fprintf(wOutput," cannot open file <%s> redirected to stdout\n",wBase.toString());
                    }
        else
        {
            FOutput=true;
         }
        }

    fprintf (wOutput,
             "__________________________________________________________\n");
    fprintf (wOutput,
             "%s>> starting processing xml file <%s>  \n"
             "            Run options are \n"
             "                 Real run or test run <%s>\n"
             "                 File to apply        <%s>\n"
                     ,
             _GET_FUNCTION_NAME_,
             pXMLPath,
             pRealRun?"Real Run":"Test Run",
             pContentFilePath==nullptr?"to be found in xml file":(const char*)pContentFilePath);

    if (pRealRun)
            fprintf(wOutput," Real Run : file structure is to be modified  \n");
        else
            fprintf(wOutput,"***test*** no update is done  \n");

    wSt=_XMLLoadAndControl(pXMLPath,XmlDoc,wRootNode,(const utf8_t*)"zicm",(const utf8_t*)"version",(const utf8_t*)__ZRF_XMLVERSION_CONTROL__,wOutput);
    if (wSt!=ZS_SUCCESS)
                {
                goto ErrorzapplyXMLFile;
                }

    wFirstlevelNode=wRootNode.firstChildElement("File");

    if (wFirstlevelNode.isNull())
        {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "XML error : corrupted or empty file <%s>. Mandatory tag <File> was expected and not found.",
                                pXMLPath);
            goto ErrorzapplyXMLFile;
        }

//---------ZRandomFile Content (ZFileDescriptor)------------------------

    wSt=_testXMLZFileDescriptor(wFirstlevelNode,pContentFilePath,(const utf8_t*)"ZFT_ZSMasterFile",URIContent,wOutput);

    fprintf (wOutput,"%s>> starting processing ZSMasterFile <%s>   \n",
             _GET_FUNCTION_NAME_,
             URIContent.toString());


    if (!pRealRun)
        {
        fprintf (wOutput,"%s>> opening ZSMasterFile in mode ZRF_Read_Only  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toString());

         wSt=wMasterFile.zopen(URIContent,ZRF_Read_Only);  // for test read is enough
        }
    else
        {
        fprintf (wOutput,"%s>> opening ZSMasterFile in mode ZRF_Exclusive | ZRF_Write  <%s> \n",
                 _GET_FUNCTION_NAME_,
                 URIContent.toCString_Strait());

         wSt=wMasterFile.zopen(URIContent,ZRF_Exclusive | ZRF_All); // must be this way
        }

     if (wSt!=ZS_SUCCESS)
             {
             ZException.addToLast(" While applying xml modifications.");
             goto EndzapplyXMLFile;
             }
     fprintf (wOutput,"%s>> file <%s> successfully opened in ZRF_Read_only mode\n",
              _GET_FUNCTION_NAME_,
              URIContent.toCString_Strait());

//-------------Control FileType------------------------------------

    if (wMasterFile.ZDescriptor.ZHeader.FileType!=ZFT_ZSMasterFile)
            {

            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_BADFILETYPE,
                                    Severity_Error,
                                    "File type is <%s> while expected <ZFT_ZSMasterFile>.",
                                    decode_ZFile_type(wMasterFile.ZDescriptor.ZHeader.FileType));

            goto ErrorzapplyXMLFile;
            }

//--------------ZFileControlBlock modifiable fields----------------

    wSt=_testXMLZFileControlBlock(wMasterFile.ZDescriptor.ZFCB,wFirstlevelNode,wMissingTags,pRealRun, wOutput);
    if (wSt!=ZS_SUCCESS)
            goto ErrorzapplyXMLFile;

 //----------------ZMasterControlBlock modifiable fields----------------
    fprintf (wOutput,"%s>> processing node <ZMasterControlBlock>  \n",
             _GET_FUNCTION_NAME_);

    wSecondlevelNode = wFirstlevelNode.firstChildElement("ZMasterControlBlock");
    if (wSecondlevelNode.isNull())
        {
        wMasterFile.zclose();
        if (FOutput)
                fclose(wOutput);
        fprintf(wOutput,"   ****Error*** Missing <ZMasterControlBlock> ******\n");
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_XMLERROR,
                                Severity_Error,
                                "xml document may be corrupted or incomplete.Expected <ZMasterControlBlock> child to <File> tag : no child found.");
        goto ErrorzapplyXMLFile;
        }// isNull
    wNodeWork = wSecondlevelNode;

    wTagName = (const utf8_t*)"HistoryOn";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. It will remain unchanged\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.addsprintf("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.HistoryOn==(wContent=="true"))
                wMessage.addsprintf( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.HistoryOn?"true":"false");
            else
                {
                wMessage.addsprintf(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.HistoryOn?"true":"false",
                             wContent=="true"?"true":"false");
                if (pRealRun)
                     {
                     wMasterFile.ZMCB.HistoryOn = (wContent=="true");
                     }
                }
    fprintf(wOutput,wMessage.toCString_Strait());
    }// else field----------------------------------------------------

    wTagName = (const utf8_t*)"JournalingOn";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>**** Failed to get parameter <%s>.It will remain unchanged\n",
                _GET_FUNCTION_NAME_,
                wTagName.toString());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.addsprintf("       parameter <%s>  ",
                 wTagName.toCString_Strait());
    if (wMasterFile.getJournalingStatus()==(wContent=="true"))
                wMessage.addsprintf( " <%s> ==unchanged== \n",
                              wMasterFile.getJournalingStatus()?"true":"false");
            else
                {
                wMessage.addsprintf(" current <%s> modified to <%s>\n",
                             wMasterFile.getJournalingStatus()?"true":"false",
                             wContent=="true"?"true":"false");
                if (pRealRun)
                     {
//                     wMasterFile.ZMCB.JournalingOn = (wContent=="true");
                     }
                }
    fprintf(wOutput,wMessage.toCString_Strait());
    }// else field----------------------------------------------------

    wTagName = (const utf8_t*)"IndexFileDirectoryPath";
    wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(wOutput,"%s>>Warning: Failed to get optional parameter <%s>. It will remain unchanged.\n",
                _GET_FUNCTION_NAME_,
                wTagName.toCString_Strait());
        wMissingTags++;
        }
    else
    {
    wMessage.clear();
    wMessage.addsprintf("       parameter <%s>  ",
                 wTagName.toString());
    if (wMasterFile.ZMCB.IndexFilePath.toQString()==wContent)
                wMessage.addsprintf( " <%s> ==unchanged== \n",
                              wMasterFile.ZMCB.IndexFilePath.toString());
            else
                {
                wMessage.addsprintf(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.IndexFilePath.toString(),
                             wContent.toStdString().c_str());
                if (pRealRun)
                     {
                     wMasterFile.ZMCB.IndexFilePath.fromQString( wContent);
                     }
                }
    fprintf(wOutput,wMessage.toCString_Strait());
    }// else field----------------------------------------------------

//-------------------Indexes-----------------------------------------

    fprintf (wOutput,"%s>> processing node <%s>  \n",
             _GET_FUNCTION_NAME_,
             "Index");

    wZICB.clear(&wMasterFile.ZMCB.MetaDic);
    wZKDic.clear();

    IndexPresence.allocateCurrentElements(wMasterFile.ZMCB.Index.size());
    IndexPresence.bzero(); // equivalent to clearData

    wThirdlevelNode = wSecondlevelNode.firstChildElement("Index");

    if (wThirdlevelNode.isNull())  // this could be the case with 'Super ZRandomFile' (ZRandomFile + Journaling options)
        {
        fprintf(wOutput,"%s>>   ****Warning No Index found in definition ******\n",
                _GET_FUNCTION_NAME_);
        if (wMasterFile.ZMCB.Index.size()>0)
                {
         fprintf(wOutput,"%s>>    ****Found <%ld> existing indexes in file header : they will ALL be deleted  ******\n",
                 _GET_FUNCTION_NAME_,
                 wMasterFile.ZMCB.Index.size());
         wMissingTags++;
                }
        goto BackProcess_zapplyXMLFile;
        }// isNull

    IndexRank = 0; // xml index rank

    wFourthlevelNode = wThirdlevelNode.firstChildElement("IndexRank");

    if (wFourthlevelNode.isNull())
        {
        fprintf(wOutput,"%s>>   ****Error No Index rank found in Index number <%ld> definition . Index definition is corrupted******\n",
                _GET_FUNCTION_NAME_, IndexRank);
        wMissingTags++;
        wMissMandatoryTags ++;
        }// isNull


    while (!wFourthlevelNode.isNull())
        {
//-------------here get index rank and dictionary details and test ------------------------
        fprintf(wOutput,
                "%s>>   getting Index rank <%ld>-------\n",
                _GET_FUNCTION_NAME_,
                IndexRank);

        wNodeWork = wFourthlevelNode;

        wTagName = (const utf8_t*)"Name";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> - stopping process\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                    ZS_XMLERROR,
                                    Severity_Fatal,
                                    "xml document may be corrupted or incomplete.Expected mandatory field <Name> child to <IndexRank> tag : no child found.");
            wMissingTags++;
            goto ErrorzapplyXMLFile;
            }

//--------Search for index rank by its name

        IndexRank =  wMasterFile.ZMCB.Index.zsearchIndexByName(wContent.toStdString().c_str());

        if (IndexRank < 0) // not found
                {
                fprintf(wOutput,
                        "%s>> Index name <%s> will be added to Index at rank <%ld> \n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString(),
                        wMasterFile.ZMCB.Index.lastIdx());

                IndexPresence.push(1);      // Index will be created(and rebuilt) and does not have to to be rebuilt - pushed on index presence list
                IndexRank=IndexPresence.lastIdx();
                FAddIndex = true;
                wAddedIndexes ++;

                wMessage.clear();
                wMessage.addsprintf("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.addsprintf(" adding value <%s>\n",
                             wContent.toUtf8().data());
                wZICB.Name.fromQString(wContent);
                }
                else
                {
                IndexPresence[IndexRank]=1; // so far : Index to be kept as is (see later)
                FAddIndex = false;

                wMessage.clear();
                wMessage.addsprintf("       parameter <%s>  ",
                             wTagName.toString());
                 if (wMasterFile.ZMCB.Index[IndexRank]->Name.toQString()==wContent)
                             wMessage.addsprintf( " <%s> ==unchanged== \n",
                                           wMasterFile.ZMCB.Index[IndexRank]->Name.toString());
                        else
                            wMessage.addsprintf(" current <%s> modified to <%s>\n",
                                         wMasterFile.ZMCB.Index[IndexRank]->Name.toString(),
                                         wContent.toStdString().c_str());

                }

        fprintf(wOutput,wMessage.toCString_Strait());
//------------------End Index Name------------------------------------------


        if (FAddIndex)
        {
//--------------------Adding Index---------------------------------
            FIndexRebuild = true;

            wTagName = (const utf8_t*)"Duplicates";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s> default value <%s> applied\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toCString_Strait(),
                        "ZST_DUPLICATES");
                wMissingTags++;
                }
                else
                {
                wMessage.clear();
                wMessage.addsprintf("       parameter <%s>  ",
                             wTagName.toString());

                wZICB.Duplicates = encode_ZST(wContent.toStdString().c_str());

                wMessage.addsprintf(" adding value <%s>\n",
                             decode_ZST(wZICB.Duplicates));// Just to cross-check

                fprintf(wOutput,wMessage.toCString_Strait());
                }// else---------------------------------------------
            wTagName = (const utf8_t*)"AutoRebuild";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s> default value <%s> applied \n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toCString_Strait(),
                        "false");
                 wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.addsprintf("       parameter <%s>  ",  // field does not exist in file
                             wTagName.toString());
                wMessage.addsprintf(" adding value <%s>\n",
                             wContent.toStdString().c_str());
                fprintf(wOutput,wMessage.toCString_Strait());
                }// else---------------------------------------------

//----------------ZKDic------------------------------

            wFifthlevelNode = wFourthlevelNode.firstChildElement("ZKDic");

            if (wFifthlevelNode.isNull())
                {
                fprintf(wOutput,
                        "%s>>   ****Error No Dictionary found in Index number <%ld> definition . Index definition is corrupted******\n",
                        _GET_FUNCTION_NAME_,
                        IndexRank);
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** No Dictionary found in Index number <%ld> definition **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags ++;
                FIndexRebuild =false;
                goto ErrorzapplyXMLFile;
                }// isNull

            wSixthlevelNode = wFifthlevelNode.firstChildElement("KeyField");  // an index must at least have one declared field

            if (wFifthlevelNode.isNull())
                {
                fprintf(wOutput,
                        "%s>>   ****Error No Key field found in Dictionary of Index number <%ld> definition . Index definition is corrupted******\n",
                        _GET_FUNCTION_NAME_,
                        IndexRank);
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags ++;
                FIndexRebuild =false;
                goto ErrorzapplyXMLFile;
                }// isNull

            while (!wSixthlevelNode.isNull())
            {
            memset(&wField,0,sizeof(wField));

            wNodeWork= wSixthlevelNode;

            wTagName = (const utf8_t*)"Name";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. It will remain unchanged.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                 wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.addsprintf("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.addsprintf(" adding value <%s>\n",
                             wContent.toStdString().c_str());
                fprintf(wOutput,wMessage.toCString_Strait());

//$$$                wField.Name = wContent;

                }// else-----------------------------------------------
            wTagName = (const utf8_t*)"NaturalSize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                 }
                else
                {
                wMessage.clear();
                wMessage.addsprintf("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.addsprintf(" adding value <%ld>\n",
                             wContent.toLong());
                fprintf(wOutput,wMessage.toCString_Strait());

                wField.NaturalSize = wContent.toLong();
                }// else-----------------------------------------------
            wTagName = (const utf8_t*)"UniversalSize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                 }
                else
                {
                wMessage.sprintf("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.addsprintf(" adding value <%ld>\n",
                             wContent.toLong());
                fprintf(wOutput,wMessage.toCString_Strait());

                wField.UniversalSize = wContent.toLong();
                }// else-----------------------------------------------
            wTagName = (const utf8_t*)"ArrayCount";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                 }
                else
                {
                wMessage.sprintf("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.addsprintf(" adding value <%ld>\n",
                             wContent.toLong());
                fprintf(wOutput,wMessage.toCString_Strait());

                wField.ArrayCount = wContent.toLong();
                }// else-----------------------------------------------
            wTagName = (const utf8_t*)"RecordOffset";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                 }
                else
                {
                wMessage.sprintf("       parameter <%s>  ",
                             wTagName.toString());
                wMessage.addsprintf(" adding value <%ld>\n",
                             wContent.toLong());
                fprintf(wOutput,wMessage.toCString_Strait());

//$$$                wField.RecordOffset = wContent.toLong();
                }// else-----------------------------------------------

            wTagName = (const utf8_t*)"ZType";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get Mandatory parameter <%s> Index will not be built\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                FIndexRebuild =false;
                goto ErrorzapplyXMLFile;
                 }
                else
                {
               //---------encode ZType to test it----------------
                wZTypeString= wContent;
                wSt = encodeZTypeFromString(wZType,wZTypeString);
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf(wOutput,"%s>>**** parameter <%s> is errored with status <%s> Index will not be built\n",
                            _GET_FUNCTION_NAME_,
                            wTagName.toString(),
                            decode_ZStatus(wSt));
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            wSt,
                                            Severity_Error,
                                            "**** **** parameter <%s> is errored with status <%s> Index will not be built for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                            wTagName.toString(),
                                            decode_ZStatus(wSt),
                                            IndexRank
                                            );
                    wMissingTags++;
                    wMissMandatoryTags++;
                    FIndexRebuild =false;
                    }
                    else
                    {
                    wMessage.sprintf("       parameter <%s>  ",
                                 wTagName.toString());
                    wMessage.addsprintf(" adding value <%ld> <%s>\n",
                                 wZType,
                                 decode_ZType(wZType));
                    fprintf(wOutput,wMessage.toCString_Strait());

     //$$$               wField.ZType = wZType;

                    }// else
                }// else-----------------------------------------------

//$$$            wZKDic.push(wField);

            wSixthlevelNode=wSixthlevelNode.nextSiblingElement("KeyField");
            }// ! wSixthlevelNode.isNull

//--------------- End ZKDic--------------------------------------------------------
//--------------------Index Add------------------------------------------------------
            if (!FIndexRebuild)
            {
                fprintf(wOutput,"%s>>**** Index is errored and will not be added\n",
                        _GET_FUNCTION_NAME_);
                wAddedIndexes --;
                IndexPresence.pop();
            }
            else
            {
                fprintf(wOutput,"%s>>    Index <%s> rank <%ld> is to be created\n",
                        _GET_FUNCTION_NAME_,
                        wZICB.Name.toString(),
                        IndexRank
                        );

                if (pRealRun)
                        {
                fprintf(wOutput,"%s>>    Adding Index <%s>\n",
                        _GET_FUNCTION_NAME_,
                        wZICB.Name.toString());

                wSt =wMasterFile.zcreateIndex (wZKDic,  // contains the description of index key to add (multi fields)
                                               wZICB.Name,
                            //                 bool pAutorebuild,
                                               wZICB.Duplicates,
                                               false) ;
                if (wSt!=ZS_SUCCESS)
                            {
                    fprintf(wOutput,"%s>>    *********Fatal error creation of Index <%s> rank <%ld> ended with error\n",
                            _GET_FUNCTION_NAME_,
                            wZICB.Name.toString(),
                            IndexRank


                            );
                    IndexPresence[IndexRank]=1; // index exits now and does not have to be rebuilt
                    goto ErrorzapplyXMLFile;
                            }
                        }// pRealRun
            }// else FIndexRebuild
        } // FAddIndex

        else
//------------------Modifying existing index------------------------------
        {
        IndexPresence[IndexRank]=1 ;// not modified yet so not to be rebuilt

// Rule to change duplicate field : ZST_NODUPLICATES  -> ZST_DUPLICATES     : OK
//                                  ZST_DUPLICATES    -> ZST_NODUPLICATES   : Not allowed. delete index and recreate one
        wTagName = (const utf8_t*)"Duplicates";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> parameter will remain unchanged\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            wMissingTags++;
            }
        else
        {
        wMessage.sprintf("       parameter <%s>  ",
                     wTagName.toString());
        if (wMasterFile.ZMCB.Index[IndexRank]->Duplicates==encode_ZST( wContent.toStdString().c_str()))
                                wMessage.addsprintf( " <%s> ==unchanged== \n",
                                              decode_ZST(wMasterFile.ZMCB.Index[IndexRank]->Duplicates));
            else
                {
                wMessage.addsprintf(" current <%s> modified to <%s>\n",
                             decode_ZST(wMasterFile.ZMCB.Index[IndexRank]->Duplicates),
                             wContent.toStdString().c_str());
                if (wMasterFile.ZMCB.Index[IndexRank]->Duplicates==ZST_DUPLICATES) // if ZST_DUPLICATES must stay to this value
                    {
                    fprintf(wOutput,"%s>>**** Changing index <Duplicates> parameter to <%s> parameter is not allowed. It will remain unchanged\n",
                            _GET_FUNCTION_NAME_,
                            wContent.toStdString().c_str());
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            wSt,
                                            Severity_Error,
                                            "Changing index <%s> parameter to <%s> parameter is not allowed for index rank <%ld>. It will remain unchanged",
                                            wTagName.toString(),
                                            wContent.toStdString().c_str(),
                                            IndexRank);
                    }
                    else
                    {
                    if (pRealRun)
                        {
                        wMasterFile.ZMCB.Index[IndexRank]->Duplicates = encode_ZST( wContent.toStdString().c_str());
                        }
                   // IndexPresence[IndexRank] = 2; // rebuild
                    }
                }// else
        fprintf(wOutput,wMessage.toCString_Strait());
        }// else field----------------------------------------------------------

        wTagName = (const utf8_t*)"AutoRebuild";
        wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
        if (wSt!=ZS_SUCCESS)
            {
            fprintf(wOutput,"%s>>**** Failed to get parameter <%s> parameter will remain unchanged\n",
                    _GET_FUNCTION_NAME_,
                    wTagName.toString());
            wMissingTags++;
            }
        else
        {
        wMessage.sprintf("       parameter <%s>  ",
                     wTagName.toString());
        if (wMasterFile.ZMCB.Index[IndexRank]->AutoRebuild==(wContent=="true"))
                    wMessage.addsprintf( " <%s> ==unchanged== \n",
                                  wMasterFile.ZMCB.Index[IndexRank]->AutoRebuild?"true":"false");
                else
                {
                wMessage.addsprintf(" current <%s> modified to <%s>\n",
                             wMasterFile.ZMCB.Index[IndexRank]->AutoRebuild?"true":"false",
                             wContent=="true"?"true":"false");
                if (pRealRun)
                    {
                    wMasterFile.ZMCB.Index[IndexRank]->AutoRebuild = (wContent=="true");
                    }
                //IndexPresence[IndexRank] = 2; // rebuild
                }
        fprintf(wOutput,wMessage.toCString_Strait());
        }// else field----------------------------------------------------------

//--------------Processing dictionary------------------------------------------
//----------------ZKDic------------------------------
#ifdef __COMMENT__
            wFifthlevelNode = wFourthlevelNode.firstChildElement("ZKDic");

            if (wFifthlevelNode.isNull())
                {
                fprintf(wOutput,
                        "%s>>   ****Error No Dictionary found in Index number <%ld> definition . Index definition is corrupted******\n",
                        _GET_FUNCTION_NAME_,
                        IndexRank);
                wMissingTags++;
                wMissMandatoryTags++;

                goto ErrorzapplyXMLFile;
                }// isNull

            wSixthlevelNode = wFifthlevelNode.firstChildElement("KeyField");

            if (wFifthlevelNode.isNull())
                {
                fprintf(wOutput,
                        "%s>>   ****Error No Key field found in Dictionary of Index number <%ld> definition . Index definition is corrupted******\n",
                        _GET_FUNCTION_NAME_,
                        IndexRank);
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                        ZS_XMLERROR,
                                        Severity_Error,
                                        "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                        wTagName.toString(),
                                        IndexRank
                                        );
                wMissingTags++;
                wMissMandatoryTags++;
                goto ErrorzapplyXMLFile;
                }// isNull

            wKeyRank = 0;
            FAddField = false ;

            while (!wSixthlevelNode.isNull())
            {
            wNodeWork= wSixthlevelNode;

            if (wKeyRank > wMasterFile.ZMCB.Index[IndexRank].ZKDic->lastIdx())
                {
                fprintf(wOutput,"%s>>       Adding Field to index key dictionary for Index name <%s>.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                if (pRealRun)
                        {
                        wMasterFile.ZMCB.Index[IndexRank].ZKDic->newBlankElement();
                        IndexPresence[IndexRank] = 2;
                        }
                FAddField = true;
                } // wKeyRank > ZKDic size

            wTagName = "Name";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged Or omitted.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                }
            else
            {
            wMessage.clear();
            wMessage.add("       parameter <%s>  ",
                         wTagName.toString());
            if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].Name== wContent.toStdString().c_str())
                        wMessage.add( " <%s> ==unchanged== \n",
                                      wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].Name.toString());
                    else
                    {
                    wMessage.add(" current <%s> modified to <%s>\n",
                                 wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].Name.toString(),
                                 wContent.toStdString().c_str());
                    if (pRealRun)
                        {
                        wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].Name = wContent ;
                        }

                    //IndexPresence[IndexRank] = 2 ; // rebuild Changing name of a field is neutral
                    }
                // changing field name will not induce an Index rebuilt. In case of Adding a field it even could be omitted
            fprintf(wOutput,wMessage.toString());
            }//else----------------------------------------------------------------------------

            wTagName = "NaturalSize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }

                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].NaturalSize== wContent.toLong())
                        wMessage.add( " <%ld> ==unchanged== \n",
                                      wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].NaturalSize);
                        else
                        {
                        wMessage.add(" current <%ld> modified to <%ld>\n",
                                     wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].NaturalSize,
                                     wContent.toLong());
                        if (pRealRun)
                            {
                            wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].NaturalSize = wContent.toLong() ;
                            }
                        IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                        }
                fprintf(wOutput,wMessage.toString());
                }// else-----------------------------------------------


            wTagName = "UniversalSize";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].UniversalSize== wContent.toLong())
                            wMessage.add( " <%ld> ==unchanged== \n",
                                          wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].UniversalSize);
                        else
                            {
                            wMessage.add(" current <%ld> modified to <%ld>\n",
                                         wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].UniversalSize,
                                         wContent.toLong());
                            if (pRealRun)
                                {
                                wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].UniversalSize = wContent.toLong() ;
                                }
                            IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                            }
                fprintf(wOutput,wMessage.toString());
                }// else-----------------------------------------------

            wTagName = "ArrayCount";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ArrayCount== wContent.toLong())
                            wMessage.add( " <%ld> ==unchanged== \n",
                                          wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ArrayCount);
                        else
                            {
                            wMessage.add(" current <%ld> modified to <%ld>\n",
                                         wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ArrayCount,
                                         wContent.toLong());
                            if (pRealRun)
                                {
                                wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ArrayCount = wContent.toLong() ;
                                }
                            IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                            }
                fprintf(wOutput,wMessage.toString());
                }// else-----------------------------------------------

            wTagName = "RecordOffset";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
                wMessage.clear();
                wMessage.add("       parameter <%s>  ",
                             wTagName.toString());
                if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].RecordOffset== wContent.toLong())
                            wMessage.add( " <%ld> ==unchanged== \n",
                                          wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].RecordOffset);
                        else
                            {
                            wMessage.add(" current <%ld> modified to <%ld>\n",
                                         wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].RecordOffset,
                                         wContent.toLong());
                            if (pRealRun)
                                {
                                wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].RecordOffset = wContent.toLong() ;
                                }
                            IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                            }
                fprintf(wOutput,wMessage.toString());

                }// else-----------------------------------------------

            wTagName = "ZType";
            wSt=getChildElementValue(wNodeWork,wTagName.toString(),wContent);
            if (wSt!=ZS_SUCCESS)
                {
                if (FAddField)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            ZS_XMLERROR,
                                            Severity_Fatal,
                                            "**** Failed to get Mandatory parameter <%s> for index rank <%ld> **** Aborting process without saving file header.",
                                            wTagName.toString(),
                                            IndexRank
                                            );
                    ZException.exit_abort();
                    }
                fprintf(wOutput,"%s>>**** Failed to get parameter <%s>. Parameter will remain unchanged.\n",
                        _GET_FUNCTION_NAME_,
                        wTagName.toString());
                wMissingTags++;
                 }
                else
                {
               //---------encode ZType to test it----------------
                wZTypeString= wContent;
                wSt = encodeZTypeFromString(wZType,wZTypeString);
                if (wSt!=ZS_SUCCESS)
                    {
                    fprintf(wOutput,"%s>>**** parameter <%s> is errored with status <%s> Field will not be changed\n",
                            _GET_FUNCTION_NAME_,
                            wTagName.toString(),
                            decode_ZStatus(wSt));
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                            wSt,
                                            Severity_Error,
                                            "**** **** parameter <%s> is errored with status <%s> Field will not be changed for index rank <%ld> **** Failed to get Mandatory parameter <%s> Index will not be built",
                                            wTagName.toString(),
                                            decode_ZStatus(wSt),
                                            IndexRank
                                            );
                    wMissingTags++;
                    }
                    else
                    {
                    wMessage.clear();
                    wMessage.add("       parameter <%s>  ",
                                 wTagName.toString());

                    if (wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType== wZType)
                                wMessage.add( " <%s> ==unchanged== \n",
                                              decode_ZType(wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType));
                            else
                                {
                                wMessage.add(" current <%ld> <%s> ",
                                             wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType,
                                             decode_ZType(wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType));
                                wMessage.add("modified to <%ld> <%s>\n",
                                             wZType,
                                             decode_ZType(wZType));
                                if (pRealRun)
                                    {
                                    wMasterFile.ZMCB.Index[IndexRank].ZKDic->Tab[wKeyRank].ZType = wZType ;
                                    }
                                IndexPresence[IndexRank] = 2; // candidate to be rebuilt
                                }
                    fprintf(wOutput,wMessage.toString());
                    }// else
                }// else-----------------------------------------------

            wSixthlevelNode=wSixthlevelNode.nextSiblingElement("KeyField");
            wKeyRank++;
            } // while !wSixthlevelNode.isNull
#endif // __COMMENT__
        }// else FAddIndex



        wFourthlevelNode=wFourthlevelNode.nextSiblingElement("IndexRank");
        IndexRank ++;
        }// while !isNull()
//----------------End ZMasterControlBlock modifiable fields----------------

BackProcess_zapplyXMLFile:
//-------------Checking for index to suppress----------------------
    fprintf(wOutput,
            "%s>>     -----------Checking for index to suppress-----------------------------\n",
            _GET_FUNCTION_NAME_);

    for (long wi = 0;wi<IndexPresence.size();wi++)
    {
        if (!IndexPresence[wi])
        {
            fprintf(wOutput,
                    "%s>>     Index rank <%ld> Name <%s> is to be deleted\n",
                    _GET_FUNCTION_NAME_,
                    wi,
                    wMasterFile.ZMCB.Index[wi]->Name.toString());
            wDestroyedIndexes ++;

            if (pRealRun)
                {
                 wSt=wMasterFile.zremoveIndex(wi);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf(wOutput,
                             "%s>>     ***** Error occurred while removing Index rank <%ld> status is <%s>\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             decode_ZStatus(wSt));
                     }// ! ZS_SUCCESS
                 fprintf(wOutput,
                         "%s>>     Index rank <%ld> has been deleted\n",
                         _GET_FUNCTION_NAME_,
                         wi);
                 IndexPresence.erase(wi); // remove the index flag from table
                 wi--;
                } // pRealRun
        }// IndexPresence
    }// for


    fprintf(wOutput,
            "%s>>     -----------End processing index(es) to suppress------------------------\n",
            _GET_FUNCTION_NAME_);

    fprintf(wOutput,
            "%s>>     -----------Checking for index to build/rebuild-----------------------------\n",
            _GET_FUNCTION_NAME_);

    for (long wi = 0;wi<IndexPresence.size();wi++)
    {
        if (IndexPresence[wi] == 2)
        {
            fprintf(wOutput,
                    "%s>>     Index rank <%ld> Name <%s> is to be rebuilt\n",
                    _GET_FUNCTION_NAME_,
                    wi,
                    wMasterFile.ZMCB.Index[wi]->Name.toString());
            if (pRealRun)
                {
                 wSt=wMasterFile.zindexRebuild(wi,ZMFStatistics,wOutput);
                 if (wSt!=ZS_SUCCESS)
                     {
                     fprintf(wOutput,
                             "%s>>     ***** Error occurred while rebuilding Index rank <%ld> status is <%s> Aborting\n",
                             _GET_FUNCTION_NAME_,
                             wi,
                             decode_ZStatus(wSt));
                     goto ErrorzapplyXMLFile;
                     }// ! ZS_SUCCESS
                } // pRealRun
            wRebuiltIndexes ++;
        }
    }

    fprintf(wOutput,
            "%s>>     -----------End processing index(es) to suppress------------------------\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();

EndzapplyXMLFile:

    //-----------------------Final report------------------------------

    fprintf(wOutput,
            "________________%s Report_______________\n"
            "  Missing tags                     %ld\n"
            "  Mandatory tags missing           %ld\n"
            "  Index(es) definition processed   %ld\n"
            "  Added Index(es)                  %ld\n"
            "  Suppressed Index(es)             %ld\n"
            "  Index(es) rebuilt                %ld\n"
            "______________________________________________________\n",
            _GET_FUNCTION_NAME_,
            wMissingTags,
            wMissMandatoryTags,
            IndexRank,
            wAddedIndexes,
            wDestroyedIndexes,
            wRebuiltIndexes);

    ZException.printUserMessage(wOutput);
    ZException.clearStack();

    if (FOutput)
            fclose(wOutput);

    _RETURN_ wSt;

ErrorzapplyXMLFile:
    fprintf(wOutput,
            "%s>>  *****process ended with errors*****************\n",
            _GET_FUNCTION_NAME_);

    wMasterFile.zclose();
    goto EndzapplyXMLFile;

}// zapplyXMLFileDefinition


/** @} */ // group XMLGroup

#endif // QT_CORE_LIB



//-------------------Statistical functions-----------------------------------------

/**
@addtogroup ZMFSTATS ZSMasterFile and ZSIndexFile storage statistics and PMS session monitoring

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
ZSMasterFile::ZRFPMSIndexStats(const long pIndex,FILE* pOutput)
{
_MODULEINIT_

    if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.size()))
    {
             fprintf(pOutput, " Performance Data report error for file <%s> : invalid index number <%ld>\n",
                     ZDescriptor.URIContent.toString(),
                     pIndex);
             _RETURN_;
    }
    fprintf (pOutput,
             "\nPerformance report on index number <%ld> name <%s> for file <%s>\n"
             "         Index Content File <%s>\n"
             "                 session summary\n"
             "________________________________________________\n"
             "   Index Header file <%s>\n"
             "________________________________________________\n",
             pIndex,

             ZMCB.Index[pIndex]->Name.toString(),
             ZDescriptor.URIHeader.toString(),
             ZMCB.IndexObjects[pIndex]->getURIContent().toString(),
             ZMCB.IndexObjects[pIndex]->getURIHeader().toString()
             );


    ZMCB.IndexObjects[pIndex]->ZDescriptor.ZPMS.reportDetails(pOutput);
    _RETURN_;

}// ZRFPMSIndexStats

/**
* @brief ZSMasterFile::zstartPMSMonitoring Starts PMS monitoring session for current ZSMasterFile
* @note collected data concerns only ZSMasterFile and NOT indexes
*   @ref  ZSMasterFile::zstartIndexPMSMonitoring
*   @ref  ZSMasterFile::zendIndexPMSMonitoring
*   @ref  ZSMasterFile::zreportIndexPMSMonitoring
*/
 void
 ZSMasterFile::zstartPMSMonitoring (void)
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
  ZSMasterFile::zendPMSMonitoring (void)
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
   ZSMasterFile::zreportPMSMonitoring (FILE* pOutput)
   {
       fprintf(pOutput,
               "________________________________________________\n"
               "   ZSMasterFile <%s>\n"
               "   File open mode         %s\n"
               "________________________________________________\n",
               ZDescriptor.URIContent.toString(),
               decode_ZRFMode( ZDescriptor.Mode)
               );
       ZPMSStats.reportFull(pOutput);
       return;
   }



/**
  * @brief ZSMasterFile::zstartIndexPMSMonitoring Starts PMS monitoring session on the specific index given by its rank (pIndex)
  * @param pIndex rank of the index to monitor
  */
 ZStatus
 ZSMasterFile::zstartIndexPMSMonitoring (const long pIndex)
 {
_MODULEINIT_
     if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.lastIdx()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVOP,
                                Severity_Severe,
                                "Invalid index number <%ld> requested while index range is [0,%ld]",
                                pIndex,
                                ZMCB.IndexObjects.lastIdx());
        _RETURN_ (ZS_INVOP);
        }
     ZMCB.IndexObjects[pIndex]->ZPMSStats.init();
     _RETURN_ ZS_SUCCESS;
 }//zstartIndexPMSMonitoring
 /**
   * @brief ZSMasterFile::zendIndexPMSMonitoring Ends current PMS monitoring session on the specific index given by its rank (pIndex)
   * @param pIndex rank of the index to monitor
   */
ZStatus
ZSMasterFile::zstopIndexPMSMonitoring(const long pIndex)
{
_MODULEINIT_
  if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.lastIdx()))
     {
     ZException.setMessage(_GET_FUNCTION_NAME_,
                             ZS_INVOP,
                             Severity_Severe,
                             "Invalid index number <%ld> requested while index range is [0,%ld]",
                             pIndex,
                             ZMCB.IndexObjects.lastIdx());
     _RETURN_ (ZS_INVOP);
     }
  ZMCB.IndexObjects[pIndex]->ZPMSStats.end();
  _RETURN_ ZS_SUCCESS;
}//zstopIndexPMSMonitoring

/**
* @brief ZSMasterFile::zreportIndexPMSMonitoring Fully reports current PMS monitoring session on the specific index given by its rank (pIndex)
* @param pIndex rank of the index to monitor
* @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
*/
ZStatus
ZSMasterFile::zreportIndexPMSMonitoring (const long pIndex, FILE* pOutput)
{
_MODULEINIT_
   if ((pIndex<0)||(pIndex>ZMCB.IndexObjects.lastIdx()))
      {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Invalid index number <%ld> requested while index range is [0,%ld]",
                              pIndex,
                              ZMCB.IndexObjects.lastIdx());
      _RETURN_ (ZS_INVOP);
      }
   fprintf(pOutput,
           "________________________________________________\n"
           "   ZSMasterFile <%s>\n"
           "   Index rank <%ld> <%s>\n"
           "   File open mode    %s\n",
           ZDescriptor.URIContent.toString(),
           pIndex,
           ZMCB.Index[pIndex]->Name.toString(),
           decode_ZRFMode( ZDescriptor.Mode));
   ZMCB.IndexObjects[pIndex]->ZPMSStats.reportFull(pOutput);
   _RETURN_ ZS_SUCCESS;
}// zreportIndexPMSMonitoring

/** @}*/ // ZMFSTATS
//----------------End Stats---------------------------------------------------



//------------Generic Functions-----------------------------------------------------------
//








#endif // ZSMASTERFILE_CPP
