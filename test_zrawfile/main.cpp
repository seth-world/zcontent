
#include <zcontent/zindexedfile/zrawmasterfile.h>
#include <zcontent/zindexedfile/zrawmasterfileutils.h>
#include "zdocphysical.h"
#include <QApplication>

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


void displayKeys (ZRawMasterFile& pMasterFile);

const char* wPictureDir = "/home/gerard/Development/zmftest/testpicture/";

int main(int argc, char *argv[])
{

  ZDataBuffer wS1,wKey;
  ZDataBuffer wRecord;
  ZResource wResource;
  ZIndexItem wII;
  utf8VaryingString wStr;

  ZStatus wSt;
  ZVerbose |= ZVB_ZRF;
  QApplication a(argc, argv);


  const char* wWD=getenv(__PARSER_WORK_DIRECTORY__);
  if (!wWD)
    wWD="";

  ZResource wUserId=ZResource::getNew(ZEntity_User);

  uriString wDocFile = wWD ;
  wDocFile.addConditionalDirectoryDelimiter();
  wDocFile += "physicaldocumentonekey.zmf";

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


  ZArray<ZDataBuffer> wKeys;

  wDocPhy.Documentid  = wR1;

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


  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_SUCCESS) {
    goto endofmain;
  }

  /* second record */
  wDocPhy.clear();
  _DBGPRINT("                   Record #2\n")

  wDocPhy.Documentid  = wR2;

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

  wStr.sprintf("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id);
  _DBGPRINT(wStr.toCChar())
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #3\n")
  /* record #3 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR10;

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

  wStr.sprintf("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id);
  _DBGPRINT(wStr.toCChar())
  displayKeys(wMasterFile);
  _DBGPRINT("                   Record #4\n")
  /* record #4 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR9;

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

  wStr.sprintf("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id);
  _DBGPRINT(wStr.toCChar())
  _DBGPRINT("                   Record #5\n")
  /* record #5 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR8;

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

  wStr.sprintf("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id);
  _DBGPRINT(wStr.toCChar())
  _DBGPRINT("                   Record #6\n")
  /* record #6 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR5;

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

  wStr.sprintf("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id);
  _DBGPRINT(wStr.toCChar())
  _DBGPRINT("                   Record #7\n")
  /* record #7 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR6;

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

  wStr.sprintf("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toChar(),wDocPhy.Documentid.id);
  _DBGPRINT(wStr.toCChar())
  _DBGPRINT("                   Record #8\n")

  wDocPhy.Documentid  = wR7;

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


  _DBGPRINT("                  End populate\n")
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
  long wIdx=0;

  _DBGPRINT("           Keys\n")

  while (true) {
    long wi=0;
    _DBGPRINT("Index %ld - %s\n",wIdx,pMasterFile.IndexTable[wIdx]->IndexName.toCChar())
    wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi++);
    while (wSt==ZS_SUCCESS) {
      wII.fromFileKey(wKey);
      const unsigned char* wPtr=wII.Data;
      wResource._importURF(wPtr);
      _DBGPRINT("%6ld %6ld %s %ld\n",
          wi,
          wII.ZMFaddress,
          decode_ZEntity( wResource.Entity).toChar(),
          wResource.id)
      wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi++);
    }
    wIdx++;
    if (wIdx >= pMasterFile.IndexTable.count())
      break;
    wi=0;
    _DBGPRINT("Index %ld - %s\n",wIdx,pMasterFile.IndexTable[wIdx]->IndexName.toCChar())
    utf8VaryingString wStr;
    wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi++);
    while (wSt==ZS_SUCCESS) {
      wII.fromFileKey(wKey);
      const unsigned char* wPtr=wII.Data;
      wStr._importURF(wPtr);
      _DBGPRINT("%6ld %6ld <%s>\n",
          wi,
          wII.ZMFaddress,
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
    while (wSt==ZS_SUCCESS) {
      wII.fromFileKey(wKey);

      const unsigned char* wPtr=wII.Data;
      wD1._importURF(wPtr);
      wD2._importURF(wPtr);
      _DBGPRINT("%6ld %6ld <%s><%s>\n",
          wi,
          wII.ZMFaddress,
          wD1.toLocale().toCChar(),
          wD2.toLocale().toCChar())
      wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi++);
    }
  }// while true

}
