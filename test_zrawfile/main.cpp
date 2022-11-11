
#include <zcontent/zindexedfile/zrawmasterfile.h>
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

const char* wPictureDir = "/home/gerard/Development/zmftest/testpicture/";

int main(int argc, char *argv[])
{
  ZStatus wSt;
  ZVerbose |= ZVB_ZRF;
  QApplication a(argc, argv);


  const char* wWD=getenv(__PARSER_WORK_DIRECTORY__);
  if (!wWD)
    wWD="";

  ZResource wUserId=ZResource::getNew(ZEntity_User);

  uriString wDocFile = wWD ;
  wDocFile.addConditionalDirectoryDelimiter();
  wDocFile += "zdocphysical_nokey.zmf";

// uriString wPictureDir;

  ZRawMasterFile wMasterFile;

  wSt=wMasterFile.zopen(wDocFile,ZRF_All);
  ZDocPhysical wDocPhy;

  ZArray<ZDataBuffer> wKeys;

  wDocPhy.Documentid  = ZResource::getNew(ZEntity_DocPhysical);

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_mail ;
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
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
    ZException.exit_abort();
  }

  /* second record */

  wDocPhy.Documentid  = ZResource::getNew(ZEntity_DocPhysical);

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_text ;
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
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
    ZException.exit_abort();
  }

  /* record #3 */
  wDocPhy.Documentid  = ZResource::getNew(ZEntity_DocPhysical);

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_other ;
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
    ZException.exit_abort();
  }

  wMasterFile.zclose();
}
