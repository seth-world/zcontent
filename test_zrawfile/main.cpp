
#include <zcontent/zindexedfile/zrawmasterfile.h>
#include <zcontent/zindexedfile/zrawmasterfileutils.h>
#include <zcontent/zindexedfile/zrawindexfile.h>
#include "zdocphysical.h"
#include <QApplication>

#include <zcontent/zrandomfile/zrandomfile.h>

#include <zentity.h>

#include <QFile>

#define __PARSER_WORK_DIRECTORY__  "zparserworkdir"
/*
file:///home/gerard/Development/zmftest/testpicture/emailconnection.jpg
file:///home/gerard/Development/zmftest/testpicture/graverip.jpg
file:///home/gerard/Development/zmftest/testpicture/Iceberg.jpeg
file:///home/gerard/Development/zmftest/testpicture/key.png
file:///home/gerard/Development/zmftest/testpicture/safe.jpeg
file:///home/gerard/Development/zmftest/testpicture/skullhead.png
file:///home/gerard/Development/zmftest/testpicture/smashedpaper.jpg
file:///home/gerard/Development/zmftest/testpicture/stylo.jpeg
file:///home/gerard/Development/zmftest/testpicture/tissuebrownbure.jpeg
file:///home/gerard/Development/zmftest/testpicture/trash-closed.jpg



*/
/* extracted from /zflow/common/zapplicationtypes.h */

typedef uint64_t DocSize_t;
typedef uint32_t ZAccessRight_t;

typedef uint32_t DocumentMetaType_t;
enum DocumentMetaType : DocumentMetaType_t
{
  ZDMT_nothing  = 0,
  ZDMT_mail     = 1,
  ZDMT_text     = 2,
  ZDMT_memo     = 6,
  ZDMT_picture  = 7,
  ZDMT_other    = 8
};



enum ZRights_type: uint32_t
{
  ZRights_OwnerRead     = QFile::ReadUser ,
  ZRights_OwnerWrite    = QFile::WriteUser ,
  ZRights_OwnerExecute  = QFile::ExeUser ,
  ZRights_GroupRead     = QFile::ReadGroup ,
  ZRights_GroupWrite    = QFile::WriteGroup ,
  ZRights_GroupExecute  = QFile::ExeGroup,
  ZRights_AllRead       = QFile::ReadOther ,
  ZRights_AllWrite      = QFile::WriteOther ,
  ZRights_AllExecute    = QFile::ExeOther

};
//
// \brief The Storage_type enum
//
//     First byte is Document Origin
//
//
//     Second byte storage conditions :
//         External to Vault- then local (if not set : internal by default)
//         Crypted content (by default : uncrypted but should be crypted)
//
typedef uint32_t  Storage_t;
enum Storage_type:Storage_t
{
  Storage_Nothing     = 0,
  Storage_Origin_URI          = 0x000001, // document file that is acquired via Pinboard
  Storage_Origin_Content      = 0x000002, // document content acquired from memory paste


  Storage_Crypted             = 0x001000, // crypted content

  Storage_InCache             = 0x010000, // Document has been loaded in Cache
  Storage_InVault             = 0x020000,  // Document has been stored in Server's Vault : NB a document could be in vault and in cache at the same time
  Storage_External            = 0x040000   // externally stored (not in vault file system)
};

/* end applicationtype.h */


utf8String displayResource(ZResource& pRes) {
  utf8String wReturn;
  wReturn.sprintf("%s-%ld",
      decode_ZEntity(pRes.Entity).toChar(),pRes.id);
  return wReturn;
}

ZStatus displayAll(ZRandomFile& pZRF) {

  ZDataBuffer wRecord;
  ZIndexItem wItem;
  ZResource wRes;

  _DBGPRINT(" List all\n")

  int wi=0;
  ZStatus wSt=pZRF.zgetFirst(wRecord);
  while (wSt==ZS_SUCCESS) {
    wItem.fromFileKey(wRecord);
    const unsigned char* wPtr = wItem.Data;
    wRes._importURF(wPtr);
    _DBGPRINT(" rank %d  Address %ld resource %s\n",wi,wItem.ZMFAddress,displayResource(wRes).toString())
    wSt=pZRF.zgetNext(wRecord);
    wi++;
  }
  return wSt;
}

ZStatus displayAllUtf8(ZRandomFile& pZRF) {

  ZDataBuffer wRecord;
  ZIndexItem wItem;
  utf8VaryingString wStr;

  _DBGPRINT(" List all utf8\n")

  int wi=0;
  ZStatus wSt=pZRF.zgetFirst(wRecord);
  while (wSt==ZS_SUCCESS) {
    wItem.fromFileKey(wRecord);
    const unsigned char* wPtr=wItem.Data;
    wStr._importURF( wPtr );
    _DBGPRINT(" rank %d  Address %ld content <%s>\n",wi,wItem.ZMFAddress,wStr.toString())
    wSt=pZRF.zgetNext(wRecord);
    wi++;
  }
  return wSt;
}

ZStatus displayRank(ZRandomFile& pZRF,long pRank){
  ZDataBuffer wRecord;
  ZIndexItem wItem;
  ZResource wRes;
  ZStatus wSt=pZRF.zget(wRecord,pRank);
  wItem.fromFileKey(wRecord);
  const unsigned char* wPtr = wItem.Data;
  wRes._importURF(wPtr);
  _DBGPRINT(" rank %ld  Address %ld resource %s\n",pRank,wItem.ZMFAddress,displayResource(wRes).toString())
  return wSt;
}


void displayKeys (ZRawMasterFile& pMasterFile);

const char* wPictureDir = "/home/gerard/Development/zmftest/testpicture/";


void addKeyValue (ZRawIndexFile& wZIX,ZIndexItem* wIndexItem,ZDataBuffer& pRecord,const zaddress_type pAddress) {
  ZStatus wSt=wZIX._addRawKeyValue_Prepare(wIndexItem,pRecord,pAddress);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }
  wSt=wZIX._rawKeyValue_Commit(wIndexItem);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }
}



int main(int argc, char *argv[])
{

  ZDataBuffer wS1,wKey;
  ZDataBuffer wRecord;
  ZResource wResource;
  ZIndexItem wII;
  utf8VaryingString wStr;

  zaddress_type wAddress=0;

  ZStatus wSt;
//  ZVerbose |= ZVB_FileEngine | ZVB_SearchEngine | ZVB_MemEngine ;
  ZVerbose |= ZVB_FileEngine | ZVB_SearchEngine | ZVB_MemEngine ;

  QApplication a(argc, argv);


  const char* wWD=getenv(__PARSER_WORK_DIRECTORY__);
  if (!wWD)
    wWD="";

  ZResource wUserId=ZResource::getNew(ZEntity_User);

  ZResource wR1 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR2 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR3 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR4 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR5 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR6 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR7 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR8 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR9 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR10 = ZResource::getNew(ZEntity_DocPhysical);

  ZResource wRes;

  ZIndexItem wItem;
#ifdef __TESTRUN_1__
  long wRank=0;
  uriString wUriZRF = wWD;

  wUriZRF.addConditionalDirectoryDelimiter();
  wUriZRF += "zrftest.zrf";

  ZRandomFile wZRF;

  wSt=wZRF.zcreate(wUriZRF,1000,10,5,49,false,true,false,false);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wSt=wZRF.zopen(wUriZRF,ZRF_All);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wZRF.setUpdateHeader(true);

  wRecord.clear();
  wR1._exportURF(wRecord);
  wRecord.setData(wRecord);

  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 1;
  wRecord=wItem.toFileKey();

  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR1).toString())
  wRank++;
  wSt=wZRF.zadd(wRecord);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wRecord.clear();
  wR2._exportURF(wRecord);
  wRecord.setData(wRecord);
  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 2;
  wRecord=wItem.toFileKey();

  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR2).toString())
  wRank++;
  wSt=wZRF.zadd(wRecord);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  displayAll(wZRF);

  wRecord.clear();
  wR3._exportURF(wRecord);
  wRecord.setData(wRecord);
  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 3;
  wRecord=wItem.toFileKey();

  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR3).toString())
  wRank++;

  wSt=wZRF.zadd(wRecord);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  displayAll(wZRF);

  wRank=2;
  _DBGPRINT("insert at %d resource %s\n",2,displayResource(wR4).toString())

  wRecord.clear();
  wR4._exportURF(wRecord);
  wRecord.setData(wRecord);
  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 4;
  wRecord=wItem.toFileKey();

  wSt=wZRF._insert2Phases_Prepare(wRecord,2,wAddress);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wSt=wZRF._insert2Phases_Commit(wRecord,2,wAddress);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  displayAll(wZRF);

  wRank = wZRF.getRecordCount();

  wRecord.clear();
  wR5._exportURF(wRecord);
  wRecord.setData(wRecord);
  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 5;
  wRecord=wItem.toFileKey();

  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR5).toString())
  wRank++;

  wSt=wZRF.zadd(wRecord);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  displayAll(wZRF);

  wRecord.clear();
  wR6._exportURF(wRecord);
  wRecord.setData(wRecord);
  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 6;
  wRecord=wItem.toFileKey();

  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR6).toString())
  wRank++;

  wSt=wZRF.zadd(wRecord);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  displayAll(wZRF);

  wRecord.clear();
  wR7._exportURF(wRecord);
  wRecord.setData(wRecord);
  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 7;
  wRecord=wItem.toFileKey();

  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR7).toString())
  wRank++;

  wSt=wZRF.zadd(wRecord);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  displayAll(wZRF);

  long wEraseRank = 2L;
  _DBGPRINT(" Erase %ld\n",wEraseRank)

  wSt=wZRF._remove_Prepare(wEraseRank,wAddress);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wSt=wZRF._remove_Commit(wEraseRank);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wEraseRank = 5L ;
  _DBGPRINT(" Erase %ld\n",wEraseRank)

  wSt=wZRF._remove_Prepare(wEraseRank,wAddress);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wSt=wZRF._remove_Commit(wEraseRank);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  displayAll(wZRF);

  wRank = 2L ;
  _DBGPRINT(" Replace rank %ld with R9 %s\nContent before replace is :\n",wRank, displayResource( wR9).toString() )
  displayRank(wZRF,wRank);

  wRecord.clear();
  wR9._exportURF(wRecord);
  wRecord.setData(wRecord);
  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 9;
  wRecord=wItem.toFileKey();

  wSt = wZRF._replace(wRecord,wRank,wAddress);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }
  _DBGPRINT("Content after replace is :\n")
  displayRank(wZRF,wRank);


  wRank = wZRF.getRecordCount();
  _DBGPRINT(" Add rank %ld with R10 %s\n",wRank, displayResource( wR10).toString() )

  wRecord.clear();
  wR10._exportURF(wRecord);
  wRecord.setData(wRecord);
  wItem.setBuffer(wRecord);
  wItem.ZMFAddress = 10;
  wRecord=wItem.toFileKey();

   wSt=wZRF.zadd(wRecord);

  _DBGPRINT("Content after add is :\n")
  displayRank(wZRF,wRank);
  wZRF.zclose();

  wSt=wZRF.zopen(ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }


  displayAll(wZRF);

  wZRF.zclose();
  return 0;
#endif // __TESTRUN_1__

  /* test run 2 */
#ifdef __TESTRUN_2__
  long wRank=0;
  uriString wUriZix = wWD;

  wUriZix.addConditionalDirectoryDelimiter();
  wUriZix += "zixtest.zix";


  ZRawIndexFile wZRF(nullptr,ZIXM_Debug | ZIXM_Dycho | ZIXM_UpdateHeader);
  ZIndexControlBlock wICB;

  wICB.IndexName= "Test index";
  wICB.URIIndex = wUriZix;
  wICB.KeyUniversalSize = 49 ;
  wICB.Duplicates=ZST_NODUPLICATES;

  wSt=wZRF.zcreateIndexFile(wICB,wUriZix,10,5,49,false,true,false,false,
      ZIXM_Debug | ZIXM_Dycho | ZIXM_UpdateHeader); /*run mode*/
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wZRF.setRunMode(ZIXM_Debug | ZIXM_Dycho | ZIXM_UpdateHeader);

  wSt=wZRF.openIndexFile(wUriZix,0,ZRF_All);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  //  wZRF.setUpdateHeader(true);


  wZRF.showRunMode();

  ZIndexItem* wIndexItem=nullptr;

  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR1).toString())
  wRecord.clear();
  wR1._exportURF(wRecord);
  wRecord.setData(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,1L);
  displayAll((ZRandomFile&)wZRF);

  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR2).toString())
  wRecord.clear();
  wR2._exportURF(wRecord);
  wRecord.setData(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,2L);
  displayAll((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #3\n")
  /* record #3 */
  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR10).toString())
  wRecord.clear();
  wR10._exportURF(wRecord);
  wRecord.setData(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,10L);
  displayAll((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #4\n")
  /* record #4 */
  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR9).toString())
  wRecord.clear();
  wR9._exportURF(wRecord);
  wRecord.setData(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,9L);
  displayAll((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #5\n")
  /* record #5 */
  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR8).toString())
  wRecord.clear();
  wR8._exportURF(wRecord);
  wRecord.setData(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,8L);
  displayAll((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #6\n")
  /* record #6 */
  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR5).toString())
  wRecord.clear();
  wR5._exportURF(wRecord);
  wRecord.setData(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,5L);
  displayAll((ZRandomFile&)wZRF);

  _DBGPRINT("\n                   Record #7\n")
  /* record #7 */
  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR6).toString())
  wRecord.clear();
  wR6._exportURF(wRecord);
  wRecord.setData(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,6L);
  displayAll((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #8\n")
  /* record #8 */
  _DBGPRINT("add %ld resource %s\n",wRank,displayResource(wR7).toString())
  wRecord.clear();
  wR7._exportURF(wRecord);
  wRecord.setData(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,7L);
  displayAll((ZRandomFile&)wZRF);

  wZRF.zclose();
  return 0;

  /* End test run 2 */

#endif // __TESTRUN_2__
#ifdef __TESTRUN_3__
  long wRank=0;
  uriString wUriZix = wWD;

  wUriZix.addConditionalDirectoryDelimiter();
  wUriZix += "zixtestAlpha.zix";


  ZRawIndexFile wZRF(nullptr,ZIXM_Debug | ZIXM_Dycho | ZIXM_UpdateHeader);
  ZIndexControlBlock wICB;

  wICB.IndexName= "Test index";
  wICB.URIIndex = wUriZix;
  wICB.KeyUniversalSize = 150 ;
  wICB.Duplicates=ZST_NODUPLICATES;

  wSt=wZRF.zcreateIndexFile(wICB,wUriZix,10,5,150,false,true,false,false,
                            ZIXM_Debug | ZIXM_Dycho | ZIXM_UpdateHeader); /*run mode*/
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

  wZRF.setRunMode(ZIXM_Dycho | ZIXM_Debug | ZIXM_UpdateHeader);

  wSt=wZRF.openIndexFile(wUriZix,0,ZRF_All);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }

//  wZRF.setUpdateHeader(true);


  wZRF.showRunMode();

  ZIndexItem* wIndexItem=nullptr;

  utf8VaryingString  wDesc;
  wDesc="Picture used for figuring an email connection.record #1";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,1L);
  displayAllUtf8((ZRandomFile&)wZRF);

  wDesc="a simple grave for resting in peace zmf record #2";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,2L);
  displayAllUtf8((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #3\n")
  /* record #3 */
  wDesc="This is iceberg zmf record #3";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,3L);
  displayAllUtf8((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #4\n")
  /* record #4 */
  wDesc="No more than a skull head record #4";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,4L);
  displayAllUtf8((ZRandomFile&)wZRF);


  /* hole test */

  wDesc="a simple grave for resting in peace zmf record #2";
  _DBGPRINT("**** remove desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);

  wSt=wZRF._removeRawKeyValue_Prepare(wIndexItem,wRank,wRecord,wAddress);

  wSt=wZRF._rawKeyValue_Commit(wIndexItem);

  displayAllUtf8((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #5\n")
  /* record #5 */
  wDesc="A smashed paper bowl record #5";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,5L);
  displayAllUtf8((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #6\n")
  /* record #6 */
  wDesc="Simple ink pen record #6";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,6L);
  displayAllUtf8((ZRandomFile&)wZRF);


  // hole grabbing test

  wDesc="This is iceberg zmf record #3";
  _DBGPRINT("**** remove desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);

  wSt=wZRF._removeRawKeyValue_Prepare(wIndexItem,wRank,wRecord,wAddress);

  wSt=wZRF._rawKeyValue_Commit(wIndexItem);

  displayAllUtf8((ZRandomFile&)wZRF);


  _DBGPRINT("\n                   Record #7\n")
  /* record #7 */
  wDesc="closed trash bin #7";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,7L);
  displayAllUtf8((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #8\n")
  /* record #8 */
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wDesc="image of key #8";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,8L);
  displayAllUtf8((ZRandomFile&)wZRF);

  _DBGPRINT("                   Record #9\n")
  /* record #9 */
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wDesc="Nothing more than 9 #9";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,9L);
  displayAllUtf8((ZRandomFile&)wZRF);



  _DBGPRINT("                   Record #10\n")
  /* record #10 */
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wDesc="THIS is number 10 #10";
  _DBGPRINT("add desc <%s>\n",wDesc.toString())
  wRecord.clear();
  wDesc._exportURF(wRecord);
  addKeyValue(wZRF,wIndexItem,wRecord,10L);
  displayAllUtf8((ZRandomFile&)wZRF);




  wZRF.zclose();
  return 0;

  /* End test run 3 */
#endif //__TESTRUN_3__


  uriString wDocFile = wWD ;
  wDocFile.addConditionalDirectoryDelimiter();
  wDocFile += "physicaldocument.zmf";

// uriString wPictureDir;

  ZRawMasterFile wMasterFile;

  wSt=wMasterFile.zopen(wDocFile,ZRF_All);
/*
  wSt=zrebuildRawIndex<ZDocPhysical>(*wMasterFile.IndexTable[0],true);
  if (wSt!=ZS_SUCCESS){
    ZException.exit_abort();
  }
*/

  ZDocPhysical wDocPhy;

//#ifdef __POPULATE__


  ZArray<ZDataBuffer> wKeys;

  wDocPhy.Documentid  = wR1;

  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_mail ;
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.Short = "Short description for picure used figuring an email connection";
  wDocPhy.Desc="Picture used for figuring an email connection.record #0";
  wDocPhy.URI += "emailconnection.jpg";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::currentDateTime();
  wDocPhy.Created = ZDateFull::currentDateTime();
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();
  wDocPhy.Temporary=0;
  wDocPhy.Ownerid = ZResource::getNew(ZEntity_User);
  wDocPhy.Vaultid = ZResource::getNew(ZEntity_Vault);

  wKeys = wDocPhy.getAllKeys();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }

  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  /* second record */
  wDocPhy.clear();
  _DBGPRINT("                   Record #2\n")

  wDocPhy.Documentid  = wR2;

  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_text ;
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.Desc="a simple grave for resting in peace zmf record #1";
  wDocPhy.URI += "graverip.jpg";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();

  wDocPhy.Registrated = ZDateFull::fromDMY("01/09");
  wDocPhy.Created = ZDateFull::fromDMY("01/09");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=ZResource::getNew(ZEntity_User);
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }


  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #3\n")
  /* record #3 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR10;

  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_other ;
  wDocPhy.Desc="This is iceberg zmf record #2";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "Iceberg.jpeg";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("01/10");
  wDocPhy.Created = ZDateFull::fromDMY("01/10");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }


  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #4\n")

  /* record #4 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR9;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_other ;
  wDocPhy.Short = "skull head";
  wDocPhy.Desc="No more than a skull head record #3";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "skullhead.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("02/10");
  wDocPhy.Created = ZDateFull::fromDMY("02/10");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }

  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #5\n")
  /* record #5 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR8;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "smashed paper";
  wDocPhy.Desc="A smashed paper bowl record #4";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "smashedpaper.jpg";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("03/10");
  wDocPhy.Created = ZDateFull::fromDMY("03/10");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }

  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #6\n")
  /* record #6 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR5;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "stylo";
  wDocPhy.Desc="Simple ink pen record #5";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "stylo.jpeg";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("04/10");
  wDocPhy.Created = ZDateFull::fromDMY("04/10");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }
  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("\n                   Record #7\n")
  /* record #7 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR6;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "stylo";
  wDocPhy.Desc="closed trash bin #6";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "trash-closed.jpg";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("04/09");
  wDocPhy.Created = ZDateFull::fromDMY("04/09");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }

  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #8\n")

  wDocPhy.Documentid  = wR7;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "key";
  wDocPhy.Desc="image of key #7";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "key.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("04/09");
  wDocPhy.Created = ZDateFull::fromDMY("06/09");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }
//#endif  // __POPULATE__


  _DBGPRINT("\n                  End populate\n")
  displayKeys(wMasterFile);


 endofmain:
   wMasterFile.zclose();
   if (wSt!=ZS_SUCCESS) {
     ZException.exit_abort();
   }


}//main




void
displayKeys (ZRawMasterFile& pMasterFile) {
  ZStatus wSt;
  ZDataBuffer wKey;
  ZIndexItem wII;
  ZResource wResource;
  zaddress_type wIndexAddress;
  long wIdx=0;

  ZRawIndexFile* wIFile=nullptr;

  _DBGPRINT("\n\n           List of all keys\n")

  while (wIdx < pMasterFile.IndexTable.count()) {
    long wi=0;
    _DBGPRINT("Index key rank <%ld> - <%s>\n"
              "rank  index address  zmf address       Resource\n",wIdx,pMasterFile.IndexTable[wIdx]->IndexName.toCChar())
    wSt=pMasterFile.IndexTable[wIdx]->zgetWAddress(wKey,wi,wIndexAddress);
    while ((wi < pMasterFile.IndexTable[wIdx]->getRecordCount())&&(wSt==ZS_SUCCESS)) {
      wII.fromFileKey(wKey);
      const unsigned char* wPtr=wII.Data;
      wResource._importURF(wPtr);
      _DBGPRINT("%6ld %12ld %12ld %s %ld\n",
          wi,
          wIndexAddress,
          wII.ZMFAddress,
          decode_ZEntity( wResource.Entity).toChar(),
          wResource.id)
      wi++;
      wSt=pMasterFile.IndexTable[wIdx]->zgetWAddress(wKey,wi,wIndexAddress);
    }
    wIdx++;
    if (wIdx >= pMasterFile.IndexTable.count())
      break;
    wi=0;
    _DBGPRINT("Index key rank <%ld> - <%s>\n",wIdx,pMasterFile.IndexTable[wIdx]->IndexName.toCChar())
    utf8VaryingString wStr;
    wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi++);
    while  ((wi < pMasterFile.IndexTable[wIdx]->getRecordCount())&&(wSt==ZS_SUCCESS)) {
      wII.fromFileKey(wKey);
      const unsigned char* wPtr=wII.Data;
      wStr._importURF(wPtr);
      _DBGPRINT("%6ld %6ld <%s>\n",
          wi,
          wII.ZMFAddress,
          wStr.toCChar())
      wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi++);
    }

    wIdx++;
    wi=0;
    if (wIdx >= pMasterFile.IndexTable.count())
      break;
    _DBGPRINT("Index %ld - %s\n",wIdx,pMasterFile.IndexTable[wIdx]->IndexName.toCChar())
    ZDateFull wD1,wD2;
    wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi++);
    while  ((wi < pMasterFile.IndexTable[wIdx]->getRecordCount())&&(wSt==ZS_SUCCESS)) {
      wII.fromFileKey(wKey);

      const unsigned char* wPtr=wII.Data;
      wD1._importURF(wPtr);
      wD2._importURF(wPtr);
      _DBGPRINT("%6ld %6ld <%s><%s>\n",
          wi,
          wII.ZMFAddress,
          wD1.toLocale().toCChar(),
          wD2.toLocale().toCChar())
      wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi++);
    }
    break;
  }// while true

  _DBGPRINT("_____________________________________________\n\n")
}
