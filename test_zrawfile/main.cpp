
#include <zcontent/zindexedfile/zrawmasterfile.h>
#include <zcontent/zindexedfile/zrawmasterfileutils.h>
#include <zcontent/zindexedfile/zrawindexfile.h>
#include "zdocphysical.h"

#include "zdocphysical_nokey.h"
#include "zdocphysical_1key.h"
#include "zdocphysical_2keys.h"

#include <QApplication>

#include <zcontent/zrandomfile/zrandomfile.h>

#include <zentity.h>

#include <QFile>

#include <zcontent/zcontentcommon/urffield.h>
#include <zcontent/zcontentcommon/urfparser.h>

#include <zcontent/zindexedfile/zmasterfile.h>

#include <zcontent/zcontentcommon/zresource.h>

#include <ztoolset/zaierrors.h>


#define __PARSER_WORK_DIRECTORY__  "zparserworkdir"

#define __NOKEY__

/*
file:///home/gerard/Development/zmftest/testpicture/emailconnection.jpg
file:///home/gerard/Development/zmftest/testpicture/graverip.jpg
file:///home/gerard/Development/zmftest/testpicture/Iceberg.jpeg
file:///home/gerard/Development/zmftest/testpicture/key.png
file:///home/gerard/Development/zmftest/testpicture/skullhead.png
file:///home/gerard/Development/zmftest/testpicture/smashedpaper.jpg
file:///home/gerard/Development/zmftest/testpicture/stylo.jpeg
file:///home/gerard/Development/zmftest/testpicture/tissuebrownbure.jpeg
file:///home/gerard/Development/zmftest/testpicture/trash-closed.jpg
file:///home/gerard/Development/zmftest/testpicture/safe.jpeg


file:///home/gerard/Development/zmftest/testpicture/Book-Active.png
file:///home/gerard/Development/zmftest/testpicture/Book-Inactive.png
file:///home/gerard/Development/zmftest/testpicture/braindump.png
file:///home/gerard/Development/zmftest/testpicture/brandnew.png
file:///home/gerard/Development/zmftest/testpicture/bubblegreen.gif
file:///home/gerard/Development/zmftest/testpicture/bubblegrey.gif
file:///home/gerard/Development/zmftest/testpicture/bubblered.gif
file:///home/gerard/Development/zmftest/testpicture/calligraauthor.png
file:///home/gerard/Development/zmftest/testpicture/calligrasheets.png
file:///home/gerard/Development/zmftest/testpicture/calligrastage.png
file:///home/gerard/Development/zmftest/testpicture/calligrawords.png
file:///home/gerard/Development/zmftest/testpicture/Charm.png
file:///home/gerard/Development/zmftest/testpicture/chineseman.png
file:///home/gerard/Development/zmftest/testpicture/clock.png
file:///home/gerard/Development/zmftest/testpicture/configure.png
file:///home/gerard/Development/zmftest/testpicture/crossblue.gif
file:///home/gerard/Development/zmftest/testpicture/curvedarrowdownleft.gif
file:///home/gerard/Development/zmftest/testpicture/curvedarrowL2R1blue.gif
file:///home/gerard/Development/zmftest/testpicture/curvedarrowL2R1grey.gif
file:///home/gerard/Development/zmftest/testpicture/curvedarrowL2R2blue.gif
file:///home/gerard/Development/zmftest/testpicture/curvedarrowL2R2grey.gif


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

utf8String displayResource(ZResource& pRes) ;
ZStatus displayAll(ZRandomFile& pZRF);
void displayKeys (ZRawMasterFile& pMasterFile);
ZStatus displayAllUtf8(ZRandomFile& pZRF);
ZStatus displayRank(ZRandomFile& pZRF,long pRank);


ZStatus populate(const uriString& pZMF);
ZStatus testSequentialKey(const uriString& pZMF);
ZStatus testRandomSearchUnique(const uriString& pZMF);

ZStatus testRandomSearchAllExact(const uriString& pZMF);

ZStatus testRandomSearchAllPartial(const uriString& pZMF);

#include <zcontentcommon/zgeneralparameters.h>


ZaiErrors ErrorLog;

int main(int argc, char *argv[])
{

  addVerbose(ZVB_MemEngine);
  ZDataBuffer wS1,wKey;
  ZDataBuffer wRecord;
  ZResource wResource;
  ZIndexItem wII;
  utf8VaryingString wStr;

  zaddress_type wAddress=0;

  ZStatus wSt;
  //  ZVerbose |= ZVB_FileEngine | ZVB_SearchEngine | ZVB_MemEngine ;
 // ZVerbose |= ZVB_FileEngine | ZVB_SearchEngine ;


  //ZVerbose |= ZVB_Mutex ;

  QApplication a(argc, argv);

  wSt=GeneralParameters.setFromArg(argc,argv);
/*
  const char* wWD=getenv(__PARSER_WORK_DIRECTORY__);
  if (!wWD)
    wWD="";

uriString wDocFile = wWD ;
*/
    uriString wDocFile = GeneralParameters.getWorkDirectory() ;

    wDocFile.addConditionalDirectoryDelimiter();

//    const char* wWD=getenv("zmasterfile");

#ifdef __NOKEY__
    const char* wWD=getenv("zmasterfilenokey");
    if (!wWD) {
        ZException.setMessage("main",ZS_MISS_FIELD,Severity_Fatal, "missing file name as environment variable <zmasterfilenokey>");
        ZException.exit_abort();
    }
#endif
#ifdef __ONEKEY__
   const char* wWD=getenv("zmasterfileonekey");
    if (!wWD) {
        ZException.setMessage("main",ZS_MISS_FIELD,Severity_Fatal, "missing file name as environment variable <zmasterfileonekey>");
        ZException.exit_abort();
    }

#endif
#ifdef __TWOKEY__
    const char* wWD=getenv("zmasterfiletwokeys");
    if (!wWD) {
        ZException.setMessage("main",ZS_MISS_FIELD,Severity_Fatal, "missing file name as environment variable <zmasterfiletwokeys>");
        ZException.exit_abort();
    }

#endif


    wDocFile += wWD;
// uriString wPictureDir;
/*
uriString wIncludeFile = "/home/gerard/Development/zbasetools/zcontent/test_zrawfile/zdocphysical.cpp";
uriString wOutFile="/home/gerard/Development/zbasetools/zcontent/test_zrawfile/zdocphysical1.cpp";
  wSt=wIncludeFile.loadUtf8(wStr);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();
  wSt=wOutFile.writeContent(wStr);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  wSt=wOutFile.appendContent(wStr);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();
*/
  wSt=populate(wDocFile);

 // wSt=testSequentialKey(wDocFile);

 // wSt=testRandomSearchUnique(wDocFile);

/*
   wSt=testRandomSearchAll(wDocFile);

*/
return 0;
}//main



utf8String displayResource(ZResource& pRes) {
  utf8String wReturn;
  wReturn.sprintf("%s-%ld",
      decode_ZEntity(pRes.Entity).toCChar(),pRes.id);
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


ZStatus populate(const uriString& pZMF) {
  ZStatus wSt=ZS_SUCCESS;

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
  ZResource wR11 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR12 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR13 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR14 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR15 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR16 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR17 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR18 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR19 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR20 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR21 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR22 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR23 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR24 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR25 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR26 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR27 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR28 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR29 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR30 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR31 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR32 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR33 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR34 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR35 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR36 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR37 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR38 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR39 = ZResource::getNew(ZEntity_DocPhysical);
  ZResource wR40 = ZResource::getNew(ZEntity_DocPhysical);

  _DBGPRINT("________________________\n"
            "  Populate test begin\n"
            "________________________\n")

  ZMasterFile wMasterFile;

  wSt=wMasterFile.zopen(pZMF,ZRF_All);
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }


  _DBGPRINT("____________________________________________________________________\n"
            " Clearing file %s\n"
            "____________________________________________________________________\n",pZMF.toString() )


  wSt=wMasterFile.zclearAll(-1,false,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  wMasterFile.setEngineMode(ZIXM_SearchDycho);

#ifdef __NOKEY__
  ZDocPhysical_nokey wDocPhy; /* no key */
#endif
#ifdef __ONEKEY__
  ZDocPhysical_1key wDocPhy; /* one key */
#endif
#ifdef __TWOKEY__
  ZDocPhysical_2keys wDocPhy; /* one key */
#endif


  ZArray<ZDataBuffer> wKeys;
//  QList<ZDataBuffer> wKeys;

  wDocPhy.Documentid  = wR1;

  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_mail ;
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.Short = "Short description for email connection";
  wDocPhy.Desc = "Picture used for figuring an email connection.record";
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
    ZException.exit_abort();
  }

  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toCChar(),wDocPhy.Documentid.id)
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
  wDocPhy.Desc="a simple grave for resting in peace zmf record";
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


  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toCChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #3\n")
  /* record #3 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR3;

  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_other ;
  wDocPhy.Short = "iceberg";
  wDocPhy.Desc="This is iceberg zmf record";
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


  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toCChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #4\n")

  /* record #4 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR4;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_other ;
  wDocPhy.Short = "skull head";
  wDocPhy.Desc="No more than a skull head record";
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
    ZException.exit_abort();
  }

  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toCChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #5\n")
  /* record #5 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR5;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "smashed paper";
  wDocPhy.Desc="A smashed paper bowl record";
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
    ZException.exit_abort();
  }

  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toCChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("                   Record #6\n")
  /* record #6 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR6;
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
    ZException.exit_abort();
  }
  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toCChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);

  _DBGPRINT("\n                   Record #7\n")
  /* record #7 */
  wDocPhy.clear();

  wDocPhy.Documentid  = wR7;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "trash bin";
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
    ZException.exit_abort();
  }

  _DBGPRINT("            %s-<%ld>\n",decode_ZEntity( wDocPhy.Documentid.Entity).toCChar(),wDocPhy.Documentid.id)
  displayKeys(wMasterFile);


  _DBGPRINT("                   Record #8\n")

  wDocPhy.Documentid  = wR8;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "key";
  wDocPhy.Desc="image of key";
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
    ZException.exit_abort();
  }



  _DBGPRINT("                   Record #9\n")

  wDocPhy.Documentid  = wR9;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Tissue";
  wDocPhy.Desc="Tissue brown bure";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "tissuebrownbure.jpeg";
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
    ZException.exit_abort();
  }

 _DBGPRINT("                   Record #10\n")

  wDocPhy.Documentid  = wR10;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Book active";
  wDocPhy.Desc="This is an active book";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "Book-Active.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

 _DBGPRINT("                   Record #11\n")

  wDocPhy.Documentid  = wR11;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Book inactive";
  wDocPhy.Desc="This is an inactive book";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "Book-Inactive.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

 _DBGPRINT("                   Record #12\n")
  wDocPhy.Documentid  = wR12;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Brain dump";
  wDocPhy.Desc="Brain dump icon";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "braindump.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }



 _DBGPRINT("                   Record #13\n")
  wDocPhy.Documentid  = wR13;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Brand new icon";
  wDocPhy.Desc="Brand new icon";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "brandnew.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

 _DBGPRINT("                   Record #14\n")
  wDocPhy.Documentid  = wR14;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Green bubble";
  wDocPhy.Desc="Green bubble";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "bubblegreen.gif";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

 _DBGPRINT("                   Record #15\n")
  wDocPhy.Documentid  = wR15;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Grey bubble";
  wDocPhy.Desc="Grey bubble";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "bubblegrey.gif";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

 _DBGPRINT("                   Record #16\n")
  wDocPhy.Documentid  = wR30;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Red bubble";
  wDocPhy.Desc="Red bubble";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "bubblered.gif";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }


_DBGPRINT("                   Record #17\n")
  wDocPhy.Documentid  = wR16;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Author";
  wDocPhy.Desc="Author of writting";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "calligraauthor.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }


_DBGPRINT("                   Record #18\n")
  wDocPhy.Documentid  = wR18;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Compound";
  wDocPhy.Desc="Compound document";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "calligrastage.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

_DBGPRINT("                   Record #19\n")
  wDocPhy.Documentid  = wR19;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "World utility";
  wDocPhy.Desc="World utility";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "calligrawords.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

_DBGPRINT("                   Record #20\n")
  wDocPhy.Documentid  = wR20;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Kind of fax";
  wDocPhy.Desc="Kind of fax";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "Charm.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

_DBGPRINT("                   Record #21\n")
  wDocPhy.Documentid  = wR21;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Chinese";
  wDocPhy.Desc="Stylized chinese man";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "chineseman.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

  wSt = wMasterFile._writeAllHeaders();
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }


  _DBGPRINT("_______________________________________\n"
            "   Duplicate key on secondary key test \n"
            "_______________________________________\n")

  _DBGPRINT("                   Record #22\n")

  wDocPhy.Documentid  = wR17;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Author";
  wDocPhy.Desc="Author of spread sheet";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "calligrasheets.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }
 _DBGPRINT("                   Record #23\n")
  wDocPhy.Documentid  = wR22;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Author duplicate";
  wDocPhy.Desc="Author of spread sheet";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "calligrasheets.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }
 _DBGPRINT("                   Record #24\n")
  wDocPhy.Documentid  = wR24;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Author duplicate #2";
  wDocPhy.Desc="Author of spread sheet";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "calligrasheets.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }

 _DBGPRINT("                   Record #25\n")
  wDocPhy.Documentid  = wR23;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Author quasi duplicate";
  wDocPhy.Desc="Author of spread sheet quasi duplicate";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "calligrasheets.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
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
    ZException.exit_abort();
  }


  _DBGPRINT("________________________________\n"
            "   Duplicate key on unique test \n"
            "________________________________\n")

  addVerbose(ZVB_SearchEngine);
 _DBGPRINT("                   Record #26\n")
  wDocPhy.Documentid  = wR1;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Ntbs";
  wDocPhy.Desc="Not to be stored";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "chineseman.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
  wDocPhy.Created = ZDateFull::fromDMY("06/09");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_DUPLICATEKEY) {
    ZException.exit_abort();
  }

 _DBGPRINT("                   Record #27\n")
  wDocPhy.Documentid  = wR3;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Ntbs";
  wDocPhy.Desc="Not to be stored";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "chineseman.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
  wDocPhy.Created = ZDateFull::fromDMY("06/09");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_DUPLICATEKEY) {
    ZException.exit_abort();
  }

  _DBGPRINT("                   Record #28\n")
  wDocPhy.Documentid  = wR15;
  _DBGPRINT("            %s\n",displayResource( wDocPhy.Documentid ).toString() )

  wDocPhy.DataRank = 0;
  wDocPhy.DocMetaType = ZDMT_picture ;
  wDocPhy.Short = "Ntbs";
  wDocPhy.Desc="Not to be stored";
  wDocPhy.Storage = Storage_External ;  // not in vault
  wDocPhy.URI = wPictureDir ;      // not in vault
  wDocPhy.URI.addConditionalDirectoryDelimiter();
  wDocPhy.URI += "chineseman.png";
  wDocPhy.DocSize= wDocPhy.URI.getFileSize();


  wDocPhy.Registrated = ZDateFull::fromDMY("15/04");
  wDocPhy.Created = ZDateFull::fromDMY("06/09");
  wDocPhy.LastModified = ZDateFull::currentDateTime();

  wDocPhy.OwnerOrigin = wUserId;

  wDocPhy.AccessRights = 00001;

  wDocPhy.CheckSum  = wDocPhy.URI.getChecksum();

  wDocPhy.Temporary=0;
  wDocPhy.Ownerid=wUserId;
  wDocPhy.Vaultid=ZResource();

  wSt=wMasterFile.zadd_T(wDocPhy);
  if (wSt!=ZS_DUPLICATEKEY) {
    ZException.exit_abort();
  }

  _DBGPRINT("_______________________________________\n"
            "   Duplicate key on unique test passed \n"
            "_______________________________________\n")



  displayKeys(wMasterFile);

  wSt = wMasterFile.zclose();
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }


  _DBGPRINT("___________________________\n"
            "   populate test passed\n"
            "___________________________\n")

  ZException.printUserMessage();

  return wSt;
}//populateOneKey


ZStatus testRandomSearchUnique(const uriString& pZMF) {
  ZStatus wSt=ZS_SUCCESS;
  ZDataBuffer wKeyRecord;
  ZDataBuffer wMasterRecord;
  ZIndexItem wIdxItem;

  ZMasterFile wMasterFile;

  wMasterFile.setEngineMode(ZIXM_SearchDycho);

  wSt=wMasterFile.zopen(pZMF,ZRF_All);
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }


  fprintf (stdout,
      "\n______________________________________________________________________________________\n"
      "testRandomSearchUnique  Key random search unique value of a key - index key <%s> <%s>\n"
      "______________________________________________________________________________________\n"
      ,wMasterFile.IndexTable[1]->IndexName.toString()
          ,wMasterFile.IndexTable[1]->Duplicates?"Allow Duplicates":"No duplicates");
  ZDataBuffer wKeyContent;
  utf8VaryingString wKeyStr ;
  ZArray<URFField> wFieldList;



  wKeyStr = "No more than a skull head record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }



  wKeyContent.clear();
  wKeyStr = "image of key";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("Searching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }


  wKeyContent.clear();
  wKeyStr = "Picture used for figuring an email connection.record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }


  wKeyContent.clear();
  wKeyStr = "a simple grave for resting in peace zmf record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }

  wKeyContent.clear();
  wKeyStr = "gabu not to be found";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_NOTFOUND)
    ZException.exit_abort();

  _DBGPRINT("\n__________________________________________________\n" \
            "testRandomSearchUnique Test passed. Key index name <%s> <%s>\n"\
            "___________________________________________________\n" \
            ,wMasterFile.IndexTable[1]->IndexName.toString() \
            ,wMasterFile.IndexTable[1]->Duplicates?"Allow duplicates":"No duplicates" )

  wSt = wMasterFile.zclose();
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }

  return wSt;
} // testRandomSearchUnique

ZStatus testRandomSearchAllExact(const uriString& pZMF) {
  ZStatus wSt=ZS_SUCCESS;
  ZDataBuffer wKeyRecord;
  ZDataBuffer wMasterRecord;
  ZIndexItem wIdxItem;

  ZMasterFile wMasterFile;

  wMasterFile.setEngineMode(ZIXM_SearchDycho);

  wSt=wMasterFile.zopen(pZMF,ZRF_All);
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }



  fprintf (stdout,
      "\n______________________________________________________________________________________\n"
      "testRandomSearchAllExact  Key random search for all exact values of a key - index key <%s> <%s>\n"
      "_______________________________________________________________________________________\n"
      ,wMasterFile.IndexTable[1]->IndexName.toString()
      ,wMasterFile.IndexTable[1]->Duplicates?"Allow Duplicates":"No duplicates");
  ZDataBuffer wKeyContent;
  utf8VaryingString wKeyStr ;
  ZArray<URFField> wFieldList;



  wKeyStr = "No more than a skull head record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }



  wKeyContent.clear();
  wKeyStr = "image of key";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("Searching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }


  wKeyContent.clear();
  wKeyStr = "Picture used for figuring an email connection.record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }


  wKeyContent.clear();
  wKeyStr = "a simple grave for resting in peace zmf record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }

  wKeyContent.clear();
  wKeyStr = "gabu not to be found";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_NOTFOUND)
    ZException.exit_abort();

  _DBGPRINT("\n__________________________________________________\n" \
            "testRandomSearchAllExact Test passed. Key index name <%s> <%s>\n"\
            "___________________________________________________\n" \
            ,wMasterFile.IndexTable[1]->IndexName.toString() \
            ,wMasterFile.IndexTable[1]->Duplicates?"Allow duplicates":"No duplicates" )


  wSt = wMasterFile.zclose();
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }

  return wSt;
} // testRandomSearchAllExact



ZStatus testRandomSearchAllPartial(const uriString& pZMF) {
  ZStatus wSt=ZS_SUCCESS;
  ZDataBuffer wKeyRecord;
  ZDataBuffer wMasterRecord;
  ZIndexItem wIdxItem;

  ZMasterFile wMasterFile;

  wMasterFile.setEngineMode(ZIXM_SearchDycho);

  wSt=wMasterFile.zopen(pZMF,ZRF_All);
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }

  fprintf (stdout,
      "\n______________________________________________________________________________________\n"
      "testRandomSearchAllPartial  Key random search for all partial values of a key - index key <%s> <%s>\n"
      "______________________________________________________________________________________\n"
      ,wMasterFile.IndexTable[1]->IndexName.toString()
      ,wMasterFile.IndexTable[1]->Duplicates?"Allow Duplicates":"No duplicates");

  ZDataBuffer wKeyContent;
  utf8VaryingString wKeyStr ;
  ZArray<URFField> wFieldList;



  wKeyStr = "No more than a skull head record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }



  wKeyContent.clear();
  wKeyStr = "image of key";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("Searching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }


  wKeyContent.clear();
  wKeyStr = "Picture used for figuring an email connection.record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }


  wKeyContent.clear();
  wKeyStr = "a simple grave for resting in peace zmf record";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_FOUND)
    ZException.exit_abort();


  wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    ZException.exit_abort();

  for (long wi=0; wi < wFieldList.count();wi++){
    fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
        wFieldList[wi].display().toString());
  }

  wKeyContent.clear();
  wKeyStr = "gabu not to be found";
  wKeyStr._exportURF(wKeyContent);

  _DBGPRINT("\n\nSearching key <%s>\n",wKeyStr.toString())

  wSt=wMasterFile.zsearch(wMasterRecord,wKeyContent,1L);
  if (wSt!=ZS_NOTFOUND)
    ZException.exit_abort();

  _DBGPRINT("\n______________________________________________________\n" \
            "testRandomSearchAllPartial Test passed. Key index name <%s> <%s>\n"\
            "_______________________________________________________\n" \
            ,wMasterFile.IndexTable[1]->IndexName.toString() \
            ,wMasterFile.IndexTable[1]->Duplicates?"Allow duplicates":"No duplicates" )



  wSt = wMasterFile.zclose();
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }


  return wSt;
} // testRandomSearchAllPartial


ZStatus testSequentialKey(const uriString& pZMF) {
  ZStatus wSt=ZS_SUCCESS;
  ZDataBuffer wKeyRecord;
  ZDataBuffer wMasterRecord;
  ZIndexItem wIdxItem;

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



  ZMasterFile wMasterFile;

  wSt=wMasterFile.zopen(pZMF,ZRF_All);
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }

  long wIndexRank = 0L;

  fprintf (stdout,"testSearch Key order sequential read index key <%s>\n",wMasterFile.IndexTable[wIndexRank]->IndexName.toString());

  wSt=wMasterFile.zgetFirstPerIndex(wMasterRecord,wIndexRank);
  int wCurrentRec=0;
  while (wSt==ZS_SUCCESS) {
    fprintf(stdout,"Record #%d\n",wCurrentRec++);
    ZArray<URFField> wFieldList;
    wSt=URFParser::parse(wMasterRecord,wFieldList,&ErrorLog);
    if (wSt!=ZS_SUCCESS)
      ZException.exit_abort();


    for (long wi=0; wi < wFieldList.count();wi++){
      //      utf8VaryingString wName = wMasterFile.Dictionary->Tab(wi).getName();
      //      utf8VaryingString wValue = wFieldList[wi].display().toString();
      fprintf (stdout,"   %s %s\n",wMasterFile.Dictionary->Tab(wi).getName().toString(),
          wFieldList[wi].display().toString());
      //      fprintf (stdout,"   %s\n",wFieldList[wi].display().toString());
    }
    wSt=wMasterFile.zgetNextPerIndex(wMasterRecord,1L);
  }

  if (wSt!=ZS_EOF)
    ZException.exit_abort();

  _DBGPRINT("\n_____________________________________________\n" \
            "testSequentialKey Test passed. Key index name <%s> <%s>\n"\
            "_______________________________________________\n" \
            ,wMasterFile.IndexTable[1]->IndexName.toString() \
            ,wMasterFile.IndexTable[1]->Duplicates?"Allow duplicates":"No duplicates" )

  wSt = wMasterFile.zclose();
  if (wSt!=ZS_SUCCESS) {
    ZException.exit_abort();
  }




  return wSt;
} // testSequentialKey

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
          decode_ZEntity( wResource.Entity).toCChar(),
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
    wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi);
    while  ((wi < pMasterFile.IndexTable[wIdx]->getRecordCount())&&(wSt==ZS_SUCCESS)) {
      wII.fromFileKey(wKey);
      const unsigned char* wPtr=wII.Data;
      wStr._importURF(wPtr);
      _DBGPRINT("%6ld %6ld <%s>\n",
          wi,
          wII.ZMFAddress,
          wStr.toCChar())
      wi++;
      wSt=pMasterFile.IndexTable[wIdx]->zget(wKey,wi);
      std::cout.flush();
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
      std::cout.flush();
    }
    break;
  }// while true

  _DBGPRINT("_____________________________________________\n\n")
}
