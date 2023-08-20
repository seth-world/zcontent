#include "poolvisu.h"
#include "ui_poolvisu.h"

#include <fcntl.h>

using namespace std::placeholders ; // for std::bind
#include <functional>               // for std::function<>

#include <zqt/zqtwidget/zqtutils.h> // for setQLabelNum() template

#include <ztoolset/zexceptionmin.h>

#include <ztoolset/uristring.h>
#include <zcontent/zrandomfile/zheadercontrolblock.h>
#include <zcontent/zrandomfile/zfilecontrolblock.h>
#include <zcontent/zrandomfile/zblock.h>

#include <zcontent/zrandomfile/zrandomfile.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <zrawmasterfilevisu.h>

#include <texteditmwn.h>
#include <displaymain.h>
#include <zcontentvisumain.h>

#include <zcontent/zrandomfile/zheadercontrolblock.h>
#include <zcontent/zindexedfile/zmastercontrolblock.h>
#include <zcontent/zindexedfile/zindexcontrolblock.h>

#include <zexceptiondlg.h>
#include <zcontent/zrandomfile/zrfutilities.h>

#include <zentity.h>

#include <qevent.h>
#include <QStandardItem>
#include <QHeaderView>

#include <QList>
#include <QMenu>
#include <QAction>
#include <QActionGroup>

#include <QGraphicsView>
#include <zqt/zqtwidget/zqgraphicscene.h>
#include <QGraphicsItem>
#include <QGraphicsRectItem>

#include <QGraphicsSceneMouseEvent>

#include "editblockdlg.h"

extern const int cst_maxraisonablevalue;


const double cst_GBlockheight = 30.0 ;
const double cst_GAddrPosHeight = 10.0 ;
const double cst_GAddrPosXNudge = 0.5 ;
const double cst_GAddrPosYNudge = 0.5 ;

poolVisu::poolVisu(QWidget *parent) :
                                      QMainWindow(parent),
                                      ui(new Ui::poolVisu)
{
  ui->setupUi(this);

  setWindowTitle("Pool management");
  HeaderTBv = new ZQTableView(this);
  HeaderTBv->newModel(4);

  HeaderTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("Field"));
  HeaderTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem("Raw value"));
  HeaderTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem("Deserialized"));
  HeaderTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem("Comment"));


  HeaderTBv->verticalHeader()->hide();

  ContentTBv = new ZQTableView(this);
  ContentTBv->newModel(7);
  int wCol=0;
  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Start Sign"));   /* 0 */
  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Validity"));     /* 1 */
//  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Endian Check"));
  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("EC State"));     /* 2 */
//  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Block id raw"));
  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Block id"));     /* 3 */
//  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("State raw"));
  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("State"));        /* 4 */
//  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Address raw"));
  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Address"));      /* 5 */
//  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Block size raw"));
  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Block size"));   /* 6 */
  ContentTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Comment"));      /* 7 */

  ContentTBv->setContextMenuCallback(std::bind(&poolVisu::menuFlex, this,placeholders::_1));
  ContentTBv->setMouseClickCallback(std::bind(&poolVisu::poolMouseCallback, this,placeholders::_1,placeholders::_2)  );

  ui->HeaderVBl->addWidget(HeaderTBv);
  ui->ContentVBl->addWidget(ContentTBv);

  ui->PoolCBx->addItem("ZBAT");
  ui->PoolCBx->addItem("ZFBT");
  ui->PoolCBx->addItem("ZHOT");
//  ui->PoolCBx->addItem("ZDBT");  // Deprecated

  QMenu*  generalMEn=new QMenu("General",this);

  ui->menubar->addMenu(generalMEn);

  QActionGroup* actionAGp=new QActionGroup(this);
  actionAGp->setExclusive(false);

  quitQAc = new QAction("quit",this);
  generalMEn->addAction(quitQAc);
  actionAGp->addAction(quitQAc);

  QMenu*  repairMEn=new QMenu("Repair",this);

  ui->menubar->addMenu(repairMEn);

  selectAllQAc = new QAction("Select all",this);
  repairMEn->addAction(selectAllQAc);
  actionAGp->addAction(selectAllQAc);

  unSelectAllQAc = new QAction("Unselect all",this);
  repairMEn->addAction(unSelectAllQAc);
  actionAGp->addAction(unSelectAllQAc);

  repairMEn->addSeparator();

  fixStateQAc = new QAction("ZPOR_FixState",this);
  fixStateQAc->setCheckable(true);
  fixStateQAc->setChecked(false);
  repairMEn->addAction(fixStateQAc);
  actionAGp->addAction(fixStateQAc);

  fixSizeQAc = new QAction("ZPO_FixSize",this);
  fixSizeQAc->setCheckable(true);
  fixSizeQAc->setChecked(false);
  repairMEn->addAction(fixSizeQAc);
  actionAGp->addAction(fixSizeQAc);

  fixOrphanQAc = new QAction("ZPOR_FixOrphan",this);
  fixOrphanQAc->setCheckable(true);
  fixOrphanQAc->setChecked(false);
  repairMEn->addAction(fixOrphanQAc);
  actionAGp->addAction(fixOrphanQAc);

  repairQAc = new QAction("run",this);
  repairMEn->addAction(repairQAc);
  actionAGp->addAction(repairQAc);


  QMenu*  listMEn=new QMenu("List",this);
  ui->menubar->addMenu(listMEn);

  displayHCBQAc = new QAction("Header Control Block",this);
  listMEn->addAction(displayHCBQAc);
  actionAGp->addAction(displayHCBQAc);

  displayFCBQAc = new QAction("File Control Block",this);
  listMEn->addAction(displayFCBQAc);
  actionAGp->addAction(displayFCBQAc);

  displayMCBQAc = new QAction("Master Control Block",this);
  listMEn->addAction(displayMCBQAc);
  actionAGp->addAction(displayMCBQAc);

  displayICBQAc = new QAction("Index Control Block",this);
  listMEn->addAction(displayICBQAc);
  actionAGp->addAction(displayICBQAc);
  listMEn->addSeparator();

  displayZBATQAc = new QAction("ZBAT raw list",this);
  listMEn->addAction(displayZBATQAc);
  actionAGp->addAction(displayZBATQAc);

  displayZFBTQAc = new QAction("ZFBT raw list",this);
  listMEn->addAction(displayZFBTQAc);
  actionAGp->addAction(displayZFBTQAc);

  displayZHOTQAc = new QAction("ZHOT raw list",this);
  listMEn->addAction(displayZHOTQAc);
  actionAGp->addAction(displayZHOTQAc);

  surfaceScanQAc = new QAction("Content surface scan",this);
  listMEn->addAction(surfaceScanQAc);
  actionAGp->addAction(surfaceScanQAc);


  listMEn->addSeparator();


  unlockHeaderQAc = new QAction("Unlock header file",this);
  listMEn->addAction(unlockHeaderQAc);
  actionAGp->addAction(unlockHeaderQAc);

  QObject::connect(ui->PoolCBx, SIGNAL(currentIndexChanged(int)), this, SLOT(PoolChange(int)));
  QObject::connect(ContentTBv, SIGNAL(destroyed()), this, SLOT(udpdateHeaderWnd()));

  QObject::connect(ui->SaveBTn, SIGNAL(clicked()), this, SLOT(udpdateHeaderWnd()));
  QObject::connect(ui->RefreshBTn, SIGNAL(clicked()), this, SLOT(refresh()));


  QObject::connect(ui->ZoomInBTn, SIGNAL(clicked()), this, SLOT(zoomIn()));
  QObject::connect(ui->ZoomOutBTn, SIGNAL(clicked()), this, SLOT(zoomOut()));
  QObject::connect(ui->FitBTn, SIGNAL(clicked()), this, SLOT(zoomFit()));



  QObject::connect(actionAGp, SIGNAL(triggered(QAction*)), this, SLOT(generalActionEvent(QAction*)));

}


poolVisu::~poolVisu()
{
  updateHeader();

  if (FdContent>=0)
    ::close(FdContent);
  if (FdHeader>=0)
    ::close(FdHeader);

  if (ContentTBv)
    delete ContentTBv;
  if (HeaderTBv)
    delete HeaderTBv;

  delete ui;
}

void
poolVisu::repairDisplay(const utf8VaryingString& pOut){
  repairLog->appendText(pOut);
}

void
poolVisu::generalActionEvent(QAction* pAction) {

  if (pAction == quitQAc) {
    this->hide();
    this->deleteLater();
    return;
  }

  if (pAction == unSelectAllQAc) {
    fixStateQAc->setChecked(false);
    fixSizeQAc->setChecked(false);
    fixOrphanQAc->setChecked(false);
    return;
  }

  if (pAction == selectAllQAc) {
    fixStateQAc->setChecked(true);
    fixSizeQAc->setChecked(true);
    fixOrphanQAc->setChecked(true);
    return;
  }

  if (pAction == repairQAc) {
    uint8_t wFlag = 0;
    if (fixStateQAc->isChecked())
      wFlag |= ZPOR_FixState ;
    if (fixSizeQAc->isChecked())
      wFlag |= ZPOR_FixSize ;
    if (fixOrphanQAc->isChecked())
      wFlag |= ZPOR_FixOrphan ;

    repairLog=new textEditMWn(this);
    repairLog->setWindowTitle("repair log");
    repairLog->show();

    poolVisu::repair(URIContent,URIHeader,wFlag,std::bind(&poolVisu::repairDisplay,this,placeholders::_1));

    refresh();
  }
  if (pAction == displayHCBQAc) {
    ZDataBuffer wRawData;
    URIHeader.loadContent(wRawData);
    DisplayMain*  entityWnd=new DisplayMain(nullptr);
    entityWnd->displayHCB(wRawData);
    entityWnd->show();
    return;
  }
  if (pAction == displayFCBQAc) {
    ZDataBuffer wRawData;
    URIHeader.loadContent(wRawData);
    DisplayMain*  entityWnd=new DisplayMain(nullptr);
    entityWnd->displayFCB(wRawData);
    entityWnd->show();
    return;
  }
  if (pAction == displayMCBQAc) {
    ZDataBuffer wRawData;
    URIHeader.loadContent(wRawData);
    DisplayMain*  entityWnd=new DisplayMain(nullptr);
    entityWnd->displayMCB(wRawData);
    entityWnd->show();
    return;
  }
  if (pAction == displayICBQAc) {
    ZDataBuffer wRawData;
    URIHeader.loadContent(wRawData);
    DisplayMain*  entityWnd=new DisplayMain(nullptr);
    entityWnd->displayMCB(wRawData);
    entityWnd->show();

    const unsigned char* wPtr = wRawData.Data;
    ZHeaderControlBlock_Export wZHCBe;
    memmove(&wZHCBe,wRawData.Data,sizeof(ZHeaderControlBlock_Export));
    if (wZHCBe.StartSign!=cst_ZBLOCKSTART) {
      ZExceptionDLg::adhocMessage("Invalid Header",Severity_Error,nullptr,nullptr,"Header Control Block appears to be corrupted.");
      return;
    }

    wZHCBe.deserialize();

    /* Index file : ICB is the only content of reserved block */
    if (wZHCBe.FileType==ZFT_ZIndexFile) {
      if (!ZContentVisuMain::testRequestedSize(URIHeader,wRawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZICB_Export)))
        return;
      if (!entityWnd)
        entityWnd=new DisplayMain(nullptr);
      const unsigned char* wPtrIn=wRawData.Data + wZHCBe.OffsetReserved;
      size_t wDisplayOffset = sizeof(ZHeaderControlBlock_Export) + wZHCBe.OffsetReserved ;
      int wRow=0;

      entityWnd->setWindowTitle(QObject::tr("Index File ICB","DisplayMain"));
      entityWnd->displaySingleICBValues(wPtrIn,wDisplayOffset,wRow);

      entityWnd->show();
      return;
    }
    /* master file  : must get ZMasterControlBlock first to jump to ICBs list  */

    ZMCB_Export wZMCBe;
    memmove(&wZMCBe,wPtr+wZHCBe.OffsetReserved,sizeof(ZMCB_Export));

    wZMCBe.deserialize();

    if (!wZMCBe.isValid()) {
      ZExceptionDLg::adhocMessage("Invalid MCB",Severity_Error,nullptr,nullptr,"Master Control Block (Reserved space) appears to be corrupted.");
      return;
    }

    if (!ZContentVisuMain::testRequestedSize(URIHeader,wRawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)+wZMCBe.ICBOffset + wZMCBe.ICBSize))
      return;
    if (!entityWnd)
      entityWnd=new DisplayMain(nullptr);

    entityWnd->displayICBs(wRawData);

    return;
  }

  if (pAction == displayZBATQAc) {
    rawListZBAT();
    return;
  }
  if (pAction == displayZFBTQAc) {
    rawListZFBT();
    return;
  }
  if (pAction == displayZHOTQAc) {
    rawListZHOT();
    return;
  }
  if (pAction == surfaceScanQAc) {
    rawSurfaceScan();
    return;
  }
  return;
}

void
poolVisu::rawListZBAT()
{
  ZDataBuffer wRawData;
  URIHeader.loadContent(wRawData);
  if (!ZContentVisuMain::testRequestedSize(URIHeader,wRawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return ;

  unsigned char* wPtr=wRawData.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)wRawData.Data;
  if (wHe->isNotReversed())
  {
    _ABORT_
  }
  zaddress_type wOffset= reverseByteOrder_Conditional<zaddress_type>(wHe->OffsetFCB);
  wPtr += wOffset;
  ZFCB_Export* wFCBe = (ZFCB_Export*)(wPtr);
  if (wFCBe->isNotReversed())
  {
    _ABORT_
  }
  wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZBAT_DataOffset);
  wPtr += wOffset;

  rawListPool(wRawData,wPtr,wOffset,"ZBAT Block Access Table");
  return;
}

void
poolVisu::rawListZFBT()
{
  ZDataBuffer wRawData;
  URIHeader.loadContent(wRawData);
  if (!ZContentVisuMain::testRequestedSize(URIHeader,wRawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return ;

  unsigned char* wPtr=wRawData.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)wRawData.Data;
  if (wHe->isNotReversed())
  {
    _ABORT_
  }
  zaddress_type wOffset= reverseByteOrder_Conditional<zaddress_type>(wHe->OffsetFCB);
  wPtr += wOffset;
  ZFCB_Export* wFCBe = (ZFCB_Export*)(wPtr);
  if (wFCBe->isNotReversed())
  {
    _ABORT_
  }
  wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZFBT_DataOffset);
  wPtr += wOffset;

  rawListPool(wRawData,wPtr,wOffset,"FBT Free blocks table");
  return;
}
void
poolVisu::rawListZHOT()
{
  ZDataBuffer wRawData;
  URIHeader.loadContent(wRawData);
  if (!ZContentVisuMain::testRequestedSize(URIHeader,wRawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return ;

  unsigned char* wPtr=wRawData.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)wRawData.Data;
  if (wHe->isNotReversed())
  {
    _ABORT_
  }
  zaddress_type wOffset= reverseByteOrder_Conditional<zaddress_type>(wHe->OffsetFCB);
  wPtr += wOffset;
  ZFCB_Export* wFCBe = (ZFCB_Export*)(wPtr);
  if (wFCBe->isNotReversed())
  {
    _ABORT_
  }
  wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZHOT_DataOffset);
  wPtr += wOffset;

  rawListPool(wRawData,wPtr,wOffset,"Holes table");
  return;
}
void
poolVisu::rawListPool(ZDataBuffer& pRawData,const unsigned char* pPtr,zaddress_type pOffset,const char* pTitle)
{
  ZAExport* wZAEe=(ZAExport*) pPtr;
  utf8VaryingString wOut;
  utf8VaryingString wStr;
  utf8VaryingString wEndianStatus;
  wStr.sprintf("%08X",wZAEe->StartSign);
  wOut.sprintf (
      "offset  absolute %ld 0x%X relative to FCB %ld 0x%X \n"
      "Field name                          Raw value              Deserialized Complement/Comment\n"
      "StartSign                            %08X                  %08X %s\n"
      "EndianCheck         %25d %25d %s\n"
      "AllocatedSize       %25ld %25ld Size in bytes of allocated space\n"
      "CurrentSize         %25ld %25ld Current data size in bytes (data not exported)\n"
      "DataSize            %25ld %25ld ZArray content export size\n"
      "FullSize            %25ld %25ld Total exported size in bytes including this header\n"
      "AllocatedElements   %25ld %25ld Number of allocated elements\n"
      "ExtentQuota         %25ld %25ld Extension quota : number of elements ZArray will be increased each time\n"
      "InitialAllocation   %25ld %25ld Number of elements intially allocated during creation\n"
      "NbElements          %25ld %25ld Number of current elements\n",
      size_t(pPtr-pRawData.Data),size_t(pPtr-pRawData.Data),pOffset,pOffset,
      wZAEe->StartSign,wZAEe->StartSign,wZAEe->StartSign==cst_ZMSTART?"cst_ZMSTART":"Wrong",
      wZAEe->EndianCheck,reverseByteOrder_Conditional<uint16_t>(wZAEe->EndianCheck),wZAEe->isReversed()?"Serialized":"NOT Serialized",
      wZAEe->AllocatedSize,reverseByteOrder_Conditional (wZAEe->AllocatedSize),
      wZAEe->CurrentSize,reverseByteOrder_Conditional (wZAEe->CurrentSize),
      wZAEe->DataSize,reverseByteOrder_Conditional (wZAEe->DataSize),
      wZAEe->FullSize,reverseByteOrder_Conditional (wZAEe->FullSize),
      wZAEe->AllocatedElements,reverseByteOrder_Conditional (wZAEe->AllocatedElements),
      wZAEe->ExtentQuota,reverseByteOrder_Conditional (wZAEe->ExtentQuota),
      wZAEe->InitialAllocation,reverseByteOrder_Conditional (wZAEe->InitialAllocation),
      wZAEe->NbElements,reverseByteOrder_Conditional (wZAEe->NbElements));



  textEditMWn* wTEx= new textEditMWn(this);
  wTEx->setText(wOut,pTitle);

  int wNbElt = int(reverseByteOrder_Conditional<ssize_t> (wZAEe->NbElements));
  if (wNbElt==0)
  {
    wOut.sprintf("\n\n           No element in Pool.\n");
    wTEx->appendText(wOut);
    wTEx->show();
    return;
  }
  if (wNbElt > cst_maxraisonablevalue ) {
    wOut.sprintf("\n\n           value <%d> is not a reasonable value.\n"
                 "               Pool might be corrupted.\n"
                 "               Limiting number elements to 10.\n",wNbElt);
    wNbElt = 10;
  }

  const unsigned char* wPtr = pPtr+sizeof(ZAExport);
  ZBlockDescriptor_Export wBDe;
  wOut=
      " \n\n"
      "------+-------------------+---------------+---------------+---------------+---------------------------------+-------------------------------------+\n"
      " Rank |     StartSign     |   EndianCheck |   BlockId     |   State       |      Address (deserialized)     |          Block size                 |\n"
      "      | hexa       state  |Value  Meaning |Val    Meaning |Val    Meaning |           Decimal        Hexa   |      Raw (Hexa)         deserialized|\n"
      "------+-------------------+---------------+---------------+---------------+---------------------------------+-------------------------------------+";
  wTEx->appendText(wOut);
  int wi=0;
  while (wi < wNbElt)
  {
    wBDe.setFromPtr(wPtr);

    wOut.sprintf(
        "%5ld |%08X %10s|%4X %10s|%2X %12s|%2X %12s|%20lld 0x%10llX|%16llX %20lld|",
        wi,
        wBDe.StartSign ,wBDe.StartSign==cst_ZFILEBLOCKSTART?"Valid":"Invalid",
        wBDe.EndianCheck ,wBDe.isReversed()?"Reversed":"Not Rever.",
        wBDe.BlockId,decode_ZBID(wBDe.BlockId),  // BlockId is uint8_t and do not need to be deserialized
        wBDe.State , decode_ZBS(wBDe.State),                      // State is uint8_t and does not need to be deserialized
        reverseByteOrder_Conditional<zaddress_type>(wBDe.Address),reverseByteOrder_Conditional<zaddress_type>(wBDe.Address),
        wBDe.BlockSize,reverseByteOrder_Conditional<zsize_type>(wBDe.BlockSize)
        );
    wTEx->appendText(wOut);
    wi++;
  }

  wTEx->appendText("------+-------------------+---------------+---------------+---------------+---------------------------------+-------------------------------------+\n");
  wTEx->show();

}// displayPool

void
poolVisu::rawSurfaceScan()
{
  ZDataBuffer wRawData;
  URIHeader.loadContent(wRawData);
  if (!ZContentVisuMain::testRequestedSize(URIHeader,wRawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return ;
  utf8VaryingString wOut;
  utf8VaryingString wStr;

  wOut.sprintf("Content file <%s>",URIContent.toString());

  textEditMWn* wTEx= new textEditMWn(this);
  wTEx->setText(wOut,"Raw file surface scan");



  ZStatus wSt;

  wSt=ZS_SUCCESS;

  ZBlockHeader wBlockHeader;

  ZDataBuffer wZDB;

  zaddress_type wOldAddress = 0;
  zsize_type    wOldSize = 0;
  zaddress_type wAddressNext = 0;
  zaddress_type wTheoricAddress = 0;

  wTEx->appendText("\n________________________________________Content Data blocks in physical order__________________________________________________");

  wTEx->appendText(
      "Physical address    | Block size         | User content size  |  Theorical next    |   State       |    Lock       |  Pid     |\n"
      "____________________|____________________|____________________|____________________|_______________|_______________|__________|"
      );

  size_t wSize;
  wSt=rawSeekEnd(FdContent,wSize);

  wSt=rawSeekBegin(FdContent);
  if (wSt!=ZS_SUCCESS)
    return ;

  wSt=rawSearchNextStartSign(FdContent,wSize,2000L,zaddress_type(0L),wAddressNext);

  bool wNotFirst=false;

  while (wSt==ZS_SUCCESS) {
    wTheoricAddress = wOldAddress+wOldSize;

    if ( wNotFirst &&(wAddressNext != wTheoricAddress)) {
      wTEx->appendTextColor(Qt::magenta, "%20ld|%20ld|%20s|%20s|%15s|%15s|%10s|",
          wTheoricAddress,
          wAddressNext-wTheoricAddress,
          " "," ",
          "--- <Hole> ---",
          " ", " "
          );
    }

    wSt = rawReadAt(FdContent,wZDB,sizeof(ZBlockHeader_Export),wAddressNext);
    if (wSt!=ZS_SUCCESS)
      break;
    ZBlockHeader_Export* wZDBE=(ZBlockHeader_Export*)wZDB.Data;
    utf8VaryingString wLock = " ";
    if (wZDBE->Lock != ZLock_Nolock) {
      wLock = decode_ZLockMask(wZDBE->Lock);
    }
    wZDBE->deserialize();
    switch (wZDBE->State) {
    case ZBS_Deleted:
      wTEx->appendTextColor(Qt::red, "%20ld|%20ld|%20ld|%20ld|%15s|%15s|%10ld|",
          wAddressNext,
          wZDBE->BlockSize,
          (wZDBE->BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
          wAddressNext+wZDBE->BlockSize,
          decode_ZBS( wZDBE->State),wLock.toString(),wZDBE->Pid);
      break;
    case ZBS_Free:
      wTEx->appendTextColor(Qt::blue, "%20ld|%20ld|%20ld|%20ld|%15s|%15s|%10ld|",
          wAddressNext,
          wZDBE->BlockSize,
          (wZDBE->BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
          wAddressNext+wZDBE->BlockSize,
          decode_ZBS( wZDBE->State),wLock.toString(),wZDBE->Pid);
      break;
    case ZBS_Used:
      wTEx->appendText( "%20ld|%20ld|%20ld|%20ld|%15s|%15s|%10ld|",
          wAddressNext,
          wZDBE->BlockSize,
          (wZDBE->BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
          wAddressNext+wZDBE->BlockSize,
          decode_ZBS( wZDBE->State),wLock.toString(),wZDBE->Pid);
      break;
      default:
        wTEx->appendTextColor(Qt::magenta, "%20ld|%20ld|%20ld|%20ld|%15s|%15s|%10ld|",
            wAddressNext,
            wZDBE->BlockSize,
            (wZDBE->BlockSize-(zsize_type)sizeof(ZBlockHeader_Export)),
            wAddressNext+wZDBE->BlockSize,
            decode_ZBS( wZDBE->State),wLock.toString(),wZDBE->Pid);
        break;

    }

    wOldAddress=wAddressNext;
    wOldSize = wZDBE->BlockSize ;
/*    if (wZDBE->State == ZBS_Used)
      wAddressNext += wZDBE->BlockSize;
    else
*/
    wAddressNext += sizeof(ZBlockHeader_Export);   // just to grab deleted Blocks

    wNotFirst=true;
    wSt=rawSearchNextStartSign(FdContent,wSize,2000L,wAddressNext,wAddressNext);
  }// while ZS_SUCCESS

  wTEx->appendText("____________________|____________________|____________________|____________________|_______________|_______________|__________|\n");

  if ((wSt==ZS_READPARTIAL)||(wSt==ZS_EOF)||(wAddressNext >= wSize)) {
    wTEx->appendText("\n       **** End of report at address <%ld> *****\n"
                       "            End of report status is <%s>",
                      wAddressNext,decode_ZStatus(wSt));
  }

  if (ZException.count() > 0)
    wTEx->appendText(ZException.last().formatFullUserMessage(false));

  wTEx->show();
  return ;
}

ZStatus
poolVisu::set(const uriString& pContentUri,const uriString& pHeaderUri)
{
  if (!pHeaderUri.exists())
    return ZS_FILENOTEXIST;
  if (!pContentUri.exists())
    return ZS_FILENOTEXIST;

  URIContent = pContentUri;
  URIHeader = pHeaderUri;

  FdContent = ::open(URIContent.toCChar(),O_RDWR);
  if (FdContent<0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        "Error opening file <%s> ",
        URIContent.toCChar());
    return  (ZS_ERROPEN);
  }
  FdHeader = ::open(URIHeader.toCChar(),O_RDWR);
  if (FdHeader<0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        "Error opening file <%s> ",
        URIHeader.toCChar());
    return  (ZS_ERROPEN);
  }

  URIHeader.loadContent(HeaderContent);

  ui->ContentLBl->setText(URIContent.toCChar());
  ui->HeaderLBl->setText(URIHeader.toCChar());

  return ZS_SUCCESS;
} // poolVisu::set


void poolVisu::refresh() {

  rawClose(FdHeader);
  rawClose(FdContent);
  ZStatus wSt=rawOpen(FdHeader,URIHeader,O_RDWR);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Open header",Severity_Error,"Error opening file <%s> ",
        URIHeader.toCChar());
    return;
  }
  wSt=rawOpen(FdContent,URIContent,O_RDWR);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Open content",Severity_Error,"Error opening file <%s> ",
        URIContent.toCChar());
    return;
  }
  size_t wS=0;
  wSt=rawSeekEnd(FdContent,wS);
/*  off_t wS=lseek(FdContent,0L,SEEK_END);
 */
  setQLabelNum<off_t>(ui->ContentSizeLBl, wS);

  URIHeader.loadContent(HeaderContent);
  dataSetup(ui->PoolCBx->currentIndex());
}

void poolVisu::dataSetup(int pPoolid) {

  ZBAT.clear();
  ZFBT.clear();
//  ZDBT.clear(); // Deprecated

  PoolId = pPoolid;

  utf8VaryingString wStr;

  ContentFileSize = size_t(URIContent.getFileSize());

  setQLabelNum<size_t>(ui->ContentSizeLBl,ContentFileSize);

  const unsigned char* wPtr=HeaderContent.Data;

  const ZHeaderControlBlock_Export* wHe = (const ZHeaderControlBlock_Export*)wPtr;
  if (wHe->isNotReversed()) {
    ZException.setMessage("poolVisu::dataSetup",ZS_CORRUPTED,Severity_Severe,"Header is corrupted. File<%s>",URIHeader.toString());
    return ;
  }

  if (wHe->FileType == ZFT_ZIndexFile)  /* FileType is uint8_t */
    isIndexFile = true;
  else
    isIndexFile = false;

  size_t wOffset= size_t(reverseByteOrder_Conditional<zaddress_type>(wHe->OffsetFCB));
  wPtr += wOffset;
  const ZFCB_Export* wFCBe = (const ZFCB_Export*)(wPtr);
  if (wFCBe->isNotReversed()) {
    PoolNotLoaded=true;
    ZException.setMessage("poolVisu::dataSetup",ZS_CORRUPTED,Severity_Severe,"File control block is corrupted. File<%s>",URIHeader.toString());
    return ;
  }

  if (reverseByteOrder_Conditional<size_t>(wFCBe->ZBAT_DataOffset) > (sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export))) {
    ZException.setMessage("poolVisu::dataSetup",ZS_CORRUPTED,Severity_Severe,"File control block is corrupted. File<%s>",URIHeader.toString());
    return ;
  }

  StartOfData = size_t(reverseByteOrder_Conditional<zaddress_type>(wFCBe->StartOfData));

  BlockTargetSize=long(reverseByteOrder_Conditional<size_t>(wFCBe->BlockTargetSize));

  if (BlockTargetSize < 10) {
    BlockTargetSize = cst_FileNudge;
  }

  const unsigned char* wPtrZBAT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZBAT_DataOffset);
  const unsigned char* wPtrZFBT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZFBT_DataOffset);
//  const unsigned char* wPtrZDBT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZDBT_DataOffset); // Deprecated

  PoolNotLoaded = false;
  while (true) {
    ZAExport wZAEMe;

    /* check pool import structure validity for ZBAT */
    memmove(&wZAEMe,wPtrZBAT,sizeof(ZAExport));
    wZAEMe.deserialize();

    if ((wZAEMe.StartSign!=cst_ZMSTART) || (wZAEMe.NbElements > cst_maxraisonablevalue)||(wZAEMe.EndianCheck!=cst_EndianCheck_Normal)) {
      PoolNotLoaded = true;
      break;
    }
    /* effective ZBAT pool import */
    ZBAT._importPool(wPtrZBAT);

    /* check pool import structure validity for ZFBT */
    memmove(&wZAEMe,wPtrZFBT,sizeof(ZAExport));
    wZAEMe.deserialize();

    if ((wZAEMe.StartSign!=cst_ZMSTART) || (wZAEMe.NbElements > cst_maxraisonablevalue)||(wZAEMe.EndianCheck!=cst_EndianCheck_Normal)) {
      PoolNotLoaded = true;
      break;
    }
    /* effective pool import */
    ZFBT._importPool(wPtrZFBT);

    ZHOT._importPool(wPtrZFBT);
#ifdef __DEPRECATED__
    /* check pool import structure validity for ZDBT */
    memmove(&wZAEMe,wPtrZDBT,sizeof(ZAExport));
    wZAEMe.deserialize();

    if ((wZAEMe.StartSign!=cst_ZMSTART) || (wZAEMe.NbElements > cst_maxraisonablevalue)||(wZAEMe.EndianCheck!=cst_EndianCheck_Normal)) {
      PoolNotLoaded = true;
      break;
    }
    /* effective pool import */
    ZDBT._importPool(wPtrZDBT);
#endif // __DEPRECATED__
    break;
  }// while true

  PoolChanged=false;

  switch (pPoolid) {
  case 0:
    wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZBAT_DataOffset);
    ui->PoolNameLBl->setText("Blocks access table");
    break;
  case 1:
    wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZFBT_DataOffset);
    ui->PoolNameLBl->setText("Free blocks table");
    break;
  case 2:
    wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZHOT_DataOffset);
    ui->PoolNameLBl->setText("Holes table");
    break;
/*
  case 2:
    wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZDBT_DataOffset);
    ui->PoolNameLBl->setText("Deleted blocks table");
    break;
*/
  }

  wPtr += wOffset;
  size_t wFromBeg=wPtr-HeaderContent.Data;

  wStr.sprintf("%ld",wFromBeg);
  ui->OffsetAbsLBl->setText(wStr.toCChar());

  wStr.sprintf("%ld",wOffset);
  ui->OffsetRelLBl->setText(wStr.toCChar());

  HeaderTBv->ItemModel->removeRows(0,HeaderTBv->ItemModel->rowCount());
  ContentTBv->ItemModel->removeRows(0,ContentTBv->ItemModel->rowCount());

  const ZAExport* wZAEe=(const ZAExport*) wPtr;
  utf8VaryingString wOut;

  QList<QStandardItem*> wRow;

  wRow.clear();
  wRow << createItem("Start sign");
  wRow << createItem(wZAEe->StartSign,"%X");
  wRow << createItem(wZAEe->StartSign,"%X");
  wRow << createItem(wZAEe->StartSign==cst_ZMSTART?"cst_ZMSTART":"Wrong");

  HeaderTBv->ItemModel->appendRow(wRow);

  wRow.clear();
  wRow << createItem("Endian check");
  wRow << createItem(wZAEe->EndianCheck,"%X");
  wRow << createItem(reverseByteOrder_Conditional<uint16_t>(wZAEe->EndianCheck),"%X");
  wRow << createItem(wZAEe->EndianCheck==cst_EndianCheck_Reversed?"Reversed":"Wrong");

  HeaderTBv->ItemModel->appendRow(wRow);

  wRow.clear();
  wRow << createItem("AllocatedSize");
  wRow << createItem(wZAEe->AllocatedSize,"%ld");
  wRow << createItem(reverseByteOrder_Conditional (wZAEe->AllocatedSize),"%ld");
  wRow << createItem("Size in bytes of allocated space");

  HeaderTBv->ItemModel->appendRow(wRow);

  wRow.clear();
  wRow << createItem("CurrentSize");
  wRow << createItem(wZAEe->CurrentSize,"%ld");
  wRow << createItem(reverseByteOrder_Conditional (wZAEe->CurrentSize),"%ld");
  wRow << createItem("Current data size in bytes (data not exported)");

  HeaderTBv->ItemModel->appendRow(wRow);

  wRow.clear();
  wRow << createItem("DataSize");
  wRow << createItem(wZAEe->DataSize,"%ld");
  wRow << createItem(reverseByteOrder_Conditional (wZAEe->DataSize),"%ld");
  wRow << createItem("ZArray content export size");

  HeaderTBv->ItemModel->appendRow(wRow);

  wRow.clear();
  wRow << createItem("FullSize");
  wRow << createItem(wZAEe->FullSize,"%ld");
  wRow << createItem(reverseByteOrder_Conditional (wZAEe->FullSize),"%ld");
  wRow << createItem("Total exported size in bytes including this header");

  HeaderTBv->ItemModel->appendRow(wRow);

  wRow.clear();
  wRow << createItem("AllocatedElements");
  wRow << createItem(wZAEe->AllocatedElements,"%ld");
  wRow << createItem(reverseByteOrder_Conditional (wZAEe->AllocatedElements),"%ld");
  wRow << createItem("Number of allocated elements");

  HeaderTBv->ItemModel->appendRow(wRow);

  wRow.clear();
  wRow << createItem("ExtentQuota");
  wRow << createItem(wZAEe->ExtentQuota,"%ld");
  wRow << createItem(reverseByteOrder_Conditional (wZAEe->ExtentQuota),"%ld");
  wRow << createItem("Extension quota : number of elements ZArray will be increased each time");

  HeaderTBv->ItemModel->appendRow(wRow);

  wRow.clear();
  wRow << createItem("InitialAllocation");
  wRow << createItem(wZAEe->InitialAllocation,"%ld");
  wRow << createItem(reverseByteOrder_Conditional (wZAEe->InitialAllocation),"%ld");
  wRow << createItem("Number of elements intially allocated during creation");

  HeaderTBv->ItemModel->appendRow(wRow);

  int wNbElt = int(reverseByteOrder_Conditional<ssize_t> (wZAEe->NbElements));

  wRow.clear();
  wRow << createItem("NbElements");
  wRow << createItem(wZAEe->NbElements,"%ld");
  wRow << createItem(reverseByteOrder_Conditional (wZAEe->NbElements),"%ld");

  if (wNbElt > cst_maxraisonablevalue ) {
    wRow << createItem("is not a reasonable value.Limiting number elements to 10.");
    wNbElt=10;
  }
  else
    wRow << createItem("Current number of elements");

  HeaderTBv->ItemModel->appendRow(wRow);
  if ( BackGroundOnce ) {
    BackGroundOnce=false;
    wRow[0]->setData(DefaultBackGround, Qt::BackgroundRole);
  }
  DefaultBackGround = HeaderTBv->ItemModel->item(0,0)->data(Qt::BackgroundRole);

  for (long wi=0; wi < HeaderTBv->ItemModel->columnCount();wi++)
    HeaderTBv->resizeColumnToContents(wi);
  for (long wi=0; wi < HeaderTBv->ItemModel->rowCount();wi++)
    HeaderTBv->resizeRowToContents(wi);

  if (wNbElt==0) {
    return;
  }

  wPtr = wPtr+sizeof(ZAExport);
  BDe=(const ZBlockDescriptor_Export*)wPtr;

  bool FErrored=false;
  int wi=0;
  while (wi < wNbElt)
  {
    //    wBDe[wi].setFromPtr(wPtr);
    ZBlockDescriptor wBD;

    FErrored=false;
    wRow.clear();

    wRow << createItem(BDe[wi].StartSign,"%X");
    wRow << createItem(BDe[wi].StartSign==cst_ZFILEBLOCKSTART?"Valid":"Invalid");

    //    wRow << createItem(wBDe[wi].EndianCheck,"%X");
    wRow << createItem(BDe[wi].isReversed()?"Reversed":"Not Rever.");

    //    wRow << createItem(uint8_t(wBDe[wi].BlockId),"%2X");
    wRow << createItem(decode_ZBID(BDe[wi].BlockId));

    //    wRow << createItem(uint8_t(wBDe[wi].State),"%2X");
    wRow << createItem(decode_ZBS(BDe[wi].State));

    //    wRow << createItem(wBDe[wi].Address,"%llX");
    zaddress_type wAddress = reverseByteOrder_Conditional<zaddress_type>(BDe[wi].Address);
    wRow << createItem(wAddress,"%lld");
    QVariant wV;
    wV.setValue<zaddress_type>(wAddress);
    wRow[0]->setData(wV,Qt::UserRole);

    //    wRow << createItem(wBDe[wi].BlockSize,"%llX");
    zsize_type wBlockSize=reverseByteOrder_Conditional<zsize_type>(BDe[wi].BlockSize);
    wRow << createItem(wBlockSize,"%lld");

    /* check block existence in main file */

    wBD.Address= wAddress;
    wBD.State=    BDe[wi].State;
    wBD.BlockSize = wBlockSize;

    uint16_t wBE=rawCheckContentBlock(PoolId,FdContent, wBD);
    if (wBE==ZBEX_Correct) {
      wRow << createItem(" ");
    }
    else {
      wRow << createItem(decode_ZBEx(wBE));
      FErrored = true;
    }
  ContentTBv->ItemModel->appendRow(wRow);
  ContentTBv->ItemModel->setVerticalHeaderItem(int(wi),createItem(wi));

  if (FErrored) {
    for (int wi=0;wi < wRow.count();wi++)
      wRow[wi]->setData(WrongBackGround, Qt::BackgroundRole);
  }

  wi++;
}

for (long wi=0; wi < ContentTBv->ItemModel->columnCount();wi++)
  ContentTBv->resizeColumnToContents(wi);
for (long wi=0; wi < ContentTBv->ItemModel->rowCount();wi++)
  ContentTBv->resizeRowToContents(wi);

/* ----------------------
 * graphical view set-up
 * ----------------------
 */


  /* create a scene with
   *
   *   0-->File content size
   *   |
   *
   *   20.0
   *   |
   *   v
   *
   */
  /* see https://htmlcolorcodes.com/color-picker/ */
  QColor lightBlue(51, 190, 255,255);
  QColor darkBlue(38, 77, 115,255);
  QColor erroredBlue(51, 118, 255,255);
  QColor holeColor(102, 102, 153,255);
  QBrush validZBATBrush(lightBlue);
  QBrush erroredZBATBrush(erroredBlue);
  QBrush unmanagedHoleBrush(darkBlue);
  QBrush greenBrush(Qt::green);
  QBrush yellowBrush(Qt::yellow);
  QBrush darkYellowBrush(Qt::darkYellow);
  QBrush blueBrush(Qt::blue);
  QBrush redBrush(Qt::red);
  QBrush cyanBrush(Qt::cyan);
  QBrush magentaBrush(Qt::magenta);
  QBrush grayBrush(Qt::lightGray);
  QBrush darkGrayBrush(Qt::gray);

  QBrush holeBrush(holeColor);

  QPen outlinePen(Qt::yellow);
  outlinePen.setWidth(1);

  QFont wFont;
  wFont.setPointSize(6);
//  wFont.setBold(true);
//  wFont.setWeight(75);

  GScene=new ZQGraphicScene(ui->FileGraphicGVw);

  GScene->setSceneRect(0.0,0.0,double(ContentFileSize+4),30.0);

  GScene->doubleClickCallBack = std::bind(&poolVisu::GSceneDoubleClick, this,_1) ;

  QGraphicsRectItem* wB1=nullptr;
  QGraphicsTextItem* wAdd=nullptr;
  QGraphicsTextItem* wSize=nullptr;

  QBrush wblueBrush(Qt::blue);

  zaddress_type wAddressMax = 0L ;

  zaddress_type wAddressMin = 0L ;


  wB1=GScene->addRect(0.0,1.0,qreal(ContentFileSize),cst_GBlockheight,outlinePen,unmanagedHoleBrush);



  if (ZBAT.count())
    wAddressMin = ZBAT[0].Address;

  for (long wi=0;wi < ZBAT.count();wi++)  {
    uint16_t wBE=rawCheckContentBlock(ZPTP_ZBAT,FdContent, ZBAT[wi]);
    if (wBE!=ZBEX_Correct){
      wB1=GScene->addRect(qreal(ZBAT[wi].Address),1.0,qreal(ZBAT[wi].BlockSize),cst_GBlockheight,outlinePen,redBrush);
    }
    else {
      wB1=GScene->addRect(qreal(ZBAT[wi].Address),1.0,qreal(ZBAT[wi].BlockSize),cst_GBlockheight,outlinePen,validZBATBrush);
    }

    positionValue (GScene,wB1,ZBAT[wi].Address,wFont,&wblueBrush);

    QVariant wV;
    ZResource wRes ;
    wRes = ZResource::getNew(ZEntity_ZBAT);
    ZDataReference wDRef (ZLayout_FileBlock,wRes,-1);
    wDRef.setPtr(&ZBAT[wi]);
    wDRef.DataRank = wi;

//    wDRef.ResourceReference.Entity=ZEntity_ZBAT;
    wV.setValue<ZDataReference>(wDRef);
    wB1->setData(ZQtDataReference,wV);

    if ((ZBAT[wi].Address+ZBAT[wi].BlockSize) > wAddressMax)
       wAddressMax = ZBAT[wi].Address +ZBAT[wi].BlockSize;

    if (ZBAT[wi].Address < wAddressMin)
      wAddressMin = ZBAT[wi].Address ;
  } // for ZBAT

  for (long wi=0;wi < ZFBT.count();wi++)  {

    uint16_t wBE=rawCheckContentBlock(ZPTP_ZFBT,FdContent, ZFBT[wi]);
    if (wBE!=ZBEX_Correct){
      wB1=GScene->addRect(qreal(ZFBT[wi].Address),1.0,qreal(ZFBT[wi].BlockSize),cst_GBlockheight,outlinePen,magentaBrush);
    }
    else {
    if (ZFBT[wi].State==ZBS_Free)
      wB1=GScene->addRect(qreal(ZFBT[wi].Address),1.0,qreal(ZFBT[wi].BlockSize),cst_GBlockheight,outlinePen,grayBrush);
    else if (ZFBT[wi].State==ZBS_Deleted)
      wB1=GScene->addRect(qreal(ZFBT[wi].Address),1.0,qreal(ZFBT[wi].BlockSize),cst_GBlockheight,outlinePen,greenBrush);
    else
      wB1=GScene->addRect(qreal(ZFBT[wi].Address),1.0,qreal(ZFBT[wi].BlockSize),cst_GBlockheight,outlinePen,magentaBrush);
    }

    positionValue (GScene,wB1,ZFBT[wi].Address,wFont);

    QVariant wV;
    ZResource wRes ;
    wRes = ZResource::getNew(ZEntity_ZFBT);
    ZDataReference wDRef (ZLayout_FileBlock,wRes,-1);
    wDRef.setPtr(&ZFBT[wi]);
    wDRef.DataRank = wi;

    wV.setValue<ZDataReference>(wDRef);
    wB1->setData(ZQtDataReference,wV);

    if ((ZFBT[wi].Address+ZFBT[wi].BlockSize) > wAddressMax)
      wAddressMax = ZFBT[wi].Address +ZFBT[wi].BlockSize;

    if (ZFBT[wi].Address < wAddressMin)
      wAddressMin = ZFBT[wi].Address ;

  }// for ZFBT


  for (long wi=0;wi < ZHOT.count();wi++)  {
    wB1=GScene->addRect(qreal(ZHOT[wi].Address),1.0,qreal(ZHOT[wi].BlockSize),cst_GBlockheight,outlinePen,holeBrush);
    QVariant wV;
    ZResource wRes ;
    wRes = ZResource::getNew(ZEntity_ZHOT);
    ZDataReference wDRef (ZLayout_FileBlock,wRes,-1);
    //    wDRef.ResourceReference.Entity=ZEntity_ZFBT;
    wDRef.setPtr(&ZHOT[wi]);
    wDRef.DataRank = wi;

    wV.setValue<ZDataReference>(wDRef);
    wB1->setData(ZQtDataReference,wV);

    if ((ZHOT[wi].Address+ZHOT[wi].BlockSize) > wAddressMax)
      wAddressMax = ZHOT[wi].Address +ZHOT[wi].BlockSize;

    if (ZHOT[wi].Address < wAddressMin)
      wAddressMin = ZHOT[wi].Address ;

  }// for ZHOT

  ui->FileGraphicGVw->setScene(GScene);
  ui->FileGraphicGVw->show();

} // poolVisu::dataSetup



void positionText(ZQGraphicScene* pScene,QGraphicsRectItem* pItem,const utf8VaryingString& pText,QFont& pFont, QBrush* pBrush) {
  QRectF wRB1 = pItem->rect();
  QGraphicsSimpleTextItem* wT1 =  pScene->addSimpleText(pText.toCChar(),pFont);
  if (pBrush)
    wT1->setBrush(*pBrush);
  QRectF wRT1 = wT1->boundingRect();
  if (wRT1.width() > wRB1.width()) {
    size_t wL=pText.strlen();
    utf8VaryingString wStr1;
    while ((wRT1.width() > wRB1.width())&&(--wL>0)) {
      wStr1 = pText.Left(wL);
      wT1->setText(wStr1.toCChar());
    }
  }
  wT1->setPos(wRB1.x()+cst_GAddrPosXNudge,wRB1.y()+cst_GAddrPosYNudge);
}//positionText

void positionValue(ZQGraphicScene* pScene, QGraphicsRectItem* pItem, zaddress_type pValue, QFont& pFont, QBrush *pBrush) {
  utf8VaryingString wStr;
  wStr.sprintf("@%lld",pValue);
  positionText(pScene,pItem,wStr,pFont,pBrush);
}

void poolVisu::zoomIn() {
  ui->FileGraphicGVw->scale(1.1,1.1);
}
void poolVisu::zoomOut() {
  ui->FileGraphicGVw->scale(0.9,0.9);
}
void poolVisu::zoomFit() {
  QRectF wR = GScene->sceneRect();
  ui->FileGraphicGVw->fitInView(wR,Qt::KeepAspectRatio);
}



void poolVisu::GSceneDoubleClick(QGraphicsSceneMouseEvent *pEvent) {
  utf8VaryingString wStr;
  int   wPoolId = 0;
  QGraphicsItem *wItem = GScene->itemAt(pEvent->scenePos(), QTransform());// it is your clicked item, you can do everything what you want. for example send it somewhere
  if (wItem==nullptr) /* if no graphic widget, then outside any stuff */
    return;
  ZDataReference wDRef;
  QVariant wV;

  wV = wItem->data(ZQtDataReference);
  if (!wV.isValid()) {
    /* this is a hole : there is a graphic widget but with no data */
    QPointF wPoint = pEvent->scenePos();
    int wOffset = int(wPoint.x());
    ZExceptionDLg::adhocMessage("Hole",Severity_Information,nullptr,nullptr,"Offset %d is within a hole",wOffset);
    return;
  }
  wDRef = wV.value<ZDataReference>();

  /* get block descriptor from pool stored as item data */
  ZBlockDescriptor* wBD=(ZBlockDescriptor*)wDRef.getPtr<ZBlockDescriptor*>();
  if (wDRef.ResourceReference.Entity == ZEntity_ZBAT) {
    wPoolId = 0;
  }
  else if (wDRef.ResourceReference.Entity == ZEntity_ZFBT) {
    wPoolId = 1;
  }
  else if (wDRef.ResourceReference.Entity == ZEntity_ZHOT) {
    wPoolId = 2;
  } else {
    wPoolId = -1;
  }


  showBlockDetail(wPoolId,wDRef.DataRank, wBD);
}

void poolVisu::showBlockDetail(int pPoolId,long pDataRank,ZBlockDescriptor* pBD)
{

  editBlockDLg* wEdBlk=new editBlockDLg(this);
  wEdBlk->setup(FdContent,FdHeader,pPoolId,pDataRank,pBD);

  int wRet=wEdBlk->exec();

  wEdBlk->deleteLater();

  if (wRet==QDialog::Accepted) {
    viewBlock(pBD->Address, pDataRank);
    return;
  }
  if (wRet==3) { /* there was a modification in pool or in file and errors should have been cleared */
    refresh();
    return;
  }
}


void poolVisu::viewBlock(zaddress_type pAddress, zrank_type pRank) {
  if (ContentVisu==nullptr)
    ContentVisu= new ZRawMasterFileVisu(this);
  ContentVisu->setup(URIContent, FdContent);
  ContentVisu->setModal(false);
  ContentVisu->goToAddress(pAddress,pRank);
  ContentVisu->setViewModeRaw();

  ContentVisu->show();
}


void poolVisu::poolMouseCallback(int pZEF, QMouseEvent *pEvent)
{

  QModelIndex wIdx=ContentTBv->currentIndex();
  if (!wIdx.isValid())
    return;

  if (pZEF!= ZEF_DoubleClickLeft){
    return;
  }

  QVariant wV;
  wV=ContentTBv->ItemModel->item(wIdx.row(),0)->data(Qt::UserRole);
  zaddress_type wAddress = wV.value<zaddress_type>();
  if (ContentVisu==nullptr)
    ContentVisu= new ZRawMasterFileVisu(this);
  ContentVisu->setup(URIContent, FdContent);

  ContentVisu->setModal(false);
  ContentVisu->_goToAddress(wAddress,zrank_type(wIdx.row()));
  ContentVisu->setViewModeRaw();

  ContentVisu->show();

}//VisuMouseCallback


void
poolVisu::menuFlex(QContextMenuEvent *event) {
  QMenu* flexMEn=new QMenu;
  flexMEn->setTitle(QObject::tr("Evaluate","ZContentVisuMain"));
  if (flexAGp==nullptr) {
    flexAGp=new QActionGroup(this) ;
  //  QObject::connect(visuActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(visuActionEvent(QAction*)));
    QObject::connect(flexAGp, &QActionGroup::triggered,this,  &poolVisu::flexMenActionEvent);
  }
  if (blockVisuRawQAc==nullptr) {
    blockVisuRawQAc=new QAction("view raw content");
    flexAGp->addAction(blockVisuRawQAc);
  }
  flexMEn->addAction(blockVisuRawQAc);

  if (blockVisuURFQAc==nullptr) {
    blockVisuURFQAc=new QAction("view URF content");
    flexAGp->addAction(blockVisuURFQAc);
  }
  flexMEn->addAction(blockVisuURFQAc);

  if (blockEditQAc==nullptr) {
    blockEditQAc=new QAction("Edit block");
    flexAGp->addAction(blockEditQAc);
  }
  flexMEn->addAction(blockEditQAc);


  switch (uint8_t(PoolId)) {
  case ZPTP_ZBAT:

    if (moveFreeQAc==nullptr) {
      moveFreeQAc = new QAction("move to free pool");
      flexAGp->addAction(moveFreeQAc);
    }
    flexMEn->addAction(moveFreeQAc);

    if (moveDeleteQAc==nullptr) {
      moveDeleteQAc = new QAction("move to deleted pool");
      flexAGp->addAction(moveDeleteQAc);
    }
    flexMEn->addAction(moveDeleteQAc);


    if (changeToUsedQAc==nullptr) {
      changeToUsedQAc =  new QAction("force state to Used");
      flexAGp->addAction(changeToUsedQAc);
    }
    flexMEn->addAction(changeToUsedQAc);

    if (removeNullQAc==nullptr) {
      removeNullQAc =  new QAction("remove null block from pool");
      flexAGp->addAction(removeNullQAc);
    }
    flexMEn->addAction(removeNullQAc);

    if ((PoolNotLoaded) ||(ZBAT.count()==0)){
      moveFreeQAc->setEnabled(false);
      changeToUsedQAc->setEnabled((false));
      changeToUsedQAc->setEnabled((false));
      removeNullQAc->setEnabled((false));
      blockVisuRawQAc->setEnabled(false);
      blockVisuURFQAc->setEnabled(false);
      blockEditQAc->setEnabled(false);
      moveDeleteQAc->setEnabled(false);
    }

    break;

  case ZPTP_ZFBT:
    if (removeNullQAc==nullptr) {
      removeNullQAc =  new QAction("remove null block from pool");
      flexAGp->addAction(removeNullQAc);
    }
    flexMEn->addAction(removeNullQAc);

    if (changeToFreeQAc==nullptr) {
      changeToFreeQAc =  new QAction("force state to Free");
      flexAGp->addAction(changeToFreeQAc);
    }
    flexMEn->addAction(changeToFreeQAc);

    if ((PoolNotLoaded) ||(ZFBT.count()==0)){
      changeToFreeQAc->setEnabled(false);
      removeNullQAc->setEnabled((false));
      blockVisuRawQAc->setEnabled(false);
      blockVisuURFQAc->setEnabled(false);
      blockEditQAc->setEnabled(false);
    }

    break;
/*
  case ZPTP_ZDBT:
    undeleteQAc =  new QAction("undelete block",flexMEn);
    flexAGp->addAction(undeleteQAc);
    removeNullQAc =  new QAction("remove null block from pool",flexMEn);
    flexAGp->addAction(removeNullQAc);


    if ((PoolNotLoaded) ||(ZDBT.count()==0)){
      undeleteQAc->setEnabled(false);
      removeNullQAc->setEnabled((false));
      blockVisuQAc->setEnabled(false);
    }

    break;
*/
  }// switch

  flexMEn->exec(event->globalPos());
  flexMEn->deleteLater();
} // poolVisu::menuFlex



void
poolVisu::flexMenActionEvent(QAction* pAction) {
  ZStatus wSt;
  QModelIndex wIdx = ContentTBv->currentIndex();
  if (!wIdx.isValid())
    return;

  if (pAction==blockVisuRawQAc) {
    QVariant wV;
    wV=ContentTBv->ItemModel->item(wIdx.row(),0)->data(Qt::UserRole);
    zaddress_type wAddress = wV.value<zaddress_type>();
    if (ContentVisu==nullptr)
      ContentVisu= new ZRawMasterFileVisu(this);
    ContentVisu->setup(URIContent, FdContent);
    ContentVisu->setModal(false);

    ContentVisu->_goToAddress(wAddress,zrank_type(wIdx.row()));
    ContentVisu->setViewModeRaw();
    ContentVisu->show();

    if (isIndexFile) {
      ContentVisu->setVisuIndexFile();
    }
    return;
  }
  if (pAction==blockVisuURFQAc) {
    QVariant wV;
    wV=ContentTBv->ItemModel->item(wIdx.row(),0)->data(Qt::UserRole);
    zaddress_type wAddress = wV.value<zaddress_type>();
    if (ContentVisu==nullptr)
      ContentVisu= new ZRawMasterFileVisu(this);
    ContentVisu->setup(URIContent, FdContent);
    ContentVisu->setModal(false);

    ContentVisu->_goToAddress(wAddress,zrank_type(wIdx.row()));
    ContentVisu->setViewModeURF();
    ContentVisu->show();

    if (isIndexFile) {
      ContentVisu->setVisuIndexFile();
    }
    return;
  }
  if (pAction==blockEditQAc) {
    QVariant wV;
    wV=ContentTBv->ItemModel->item(wIdx.row(),0)->data(Qt::UserRole);
    zaddress_type wAddress = wV.value<zaddress_type>();


    //void poolVisu::showBlockDetail(int pPoolId,long pDataRank,ZBlockDescriptor* pBD)
    ZBlockDescriptor* wBD=nullptr;
    int wDataRank=wIdx.row();
    switch (PoolId) {
    case ZPTP_ZBAT :
      wBD=&ZBAT[wDataRank];
      break;
    case ZPTP_ZFBT :
      wBD=&ZFBT[wDataRank];
      break;
    case ZPTP_ZHOT :
      wBD=&ZHOT[wDataRank];
      break;

    }
    showBlockDetail(PoolId,wDataRank,wBD);
    return;
  }

  switch (PoolId) {
  case ZPTP_ZBAT :
    if (pAction==moveFreeQAc) {
      long wi = long(wIdx.row());
      wSt=updateBlockHeaderState(URIContent,FdContent, ZBAT[wi].Address,ZBS_Free);
      if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp;
        wExcp = ZException.last().formatFullUserMessage();
        ZExceptionDLg::adhocMessage("Updating state",Severity_Severe,nullptr,
            &wExcp,"Error while updating block state on content file <%s>",URIContent.toString());
        return;
      }
      ZBAT[wi].State = ZBS_Free;
      long widx=ZFBT.push(ZBAT[wi]);
      ZBAT.erase(wi);

      wSt = grabFreeForward(URIContent,FdContent,ContentFileSize , StartOfData,ZFBT,widx,nullptr);

      ContentTBv->ItemModel->removeRow(wIdx.row());
      PoolChanged=true;
      break;
    } // moveFreeQAc
    if (pAction==changeToUsedQAc) {
      long wi = long(wIdx.row());
      ZBAT[wi].State = ZBS_Used;

      /* update content file */
      wSt=updateBlockHeaderState(URIContent,FdContent,ZBAT[wi].Address,ZBS_Used);
      if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp;
        wExcp = ZException.last().formatFullUserMessage();
        ZExceptionDLg::adhocMessage("Updating state",Severity_Severe,nullptr,
            &wExcp,"Error while updating block state on content file <%s>",URIContent.toString());
        return;
      }

      /* then table view item */
      QStandardItem* wItem = ContentTBv->ItemModel->item(wi,4); /*State is column 4 */
      wItem->setText(decode_ZBS(ZBS_Used));
      PoolChanged=true;
      break;
    }
    if (pAction==removeNullQAc) {
      /* removing a block from pool (any pool) is possible if
       *  - block has a size of 0
       *  - address does NOT point to a valid block on content file  (orphan pool block)
       */
      long wi = long(wIdx.row());
      uint16_t wZBEx=rawCheckContentBlock(PoolId,FdContent,ZBAT[wi]);
      if (wZBEx == ZBEX_Correct) {
        ZExceptionDLg::adhocMessage("Removing block",Severity_Error,nullptr,
            nullptr,"Block rank %ld is correct and cannot be removed as a null sized and/or orphan block",wi);
        return;
      }

      ZBAT.erase(wi);
      ContentTBv->ItemModel->removeRow(wIdx.row());

      PoolChanged=true;
      break;
    }
    break;

  case ZPTP_ZFBT:
    if (pAction==removeNullQAc) {
      /* removing a block from pool (any pool) is possible if
       *  - block has a size of 0
       *  - address does NOT point to a valid block on content file  (orphan pool block)
       */
      long wi = long(wIdx.row());
      uint16_t wZBEx=rawCheckContentBlock(PoolId,FdContent,ZFBT[wi]);
      if (wZBEx == ZBEX_Correct) {
        ZExceptionDLg::adhocMessage("Removing block",Severity_Error,nullptr,
            nullptr,"Block rank %ld is correct and cannot be removed as a null sized and/or orphan block",wi);
        return;
      }

      ZFBT.erase(wi);
      ContentTBv->ItemModel->removeRow(wIdx.row());

      PoolChanged=true;
      break;
    }
    if (pAction==changeToFreeQAc) {
      long wi = long(wIdx.row());


      /* update content file */
      wSt=updateBlockHeaderState(URIContent,FdContent,ZFBT[wi].Address,ZBS_Free);
      if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp;
        wExcp = ZException.last().formatFullUserMessage();
        ZExceptionDLg::adhocMessage("Updating state",Severity_Severe,nullptr,
            &wExcp,"Error while updating block state on content file <%s>",URIContent.toString());
        return;
      }
      ZFBT[wi].State = ZBS_Free;

      /* then table view item */
      QStandardItem* wItem = ContentTBv->ItemModel->item(wi,4); /*State is column 4 */
      wItem->setText(decode_ZBS(ZBS_Free));
      PoolChanged=true;
      break;
    }
    if (pAction==undeleteQAc) {
      /*  - check block is valid
       *  - move block from ZDBT to ZBAT
       *  - change block state from ZBS_Deleted to ZBS_Used
       */
      long wi = long(wIdx.row());
      uint16_t wZBEx=rawCheckContentBlock(PoolId,FdContent,ZFBT[wi]);
      if (wZBEx != ZBEX_Correct) {
        ZExceptionDLg::adhocMessage("Undelete block",Severity_Error,nullptr,
            nullptr,"Block rank %ld is not correct and cannot be modified",wi);
        return;
      }

      wSt=updateBlockHeaderState(URIContent,FdContent,ZFBT[wi].Address,ZBS_Used);
      if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp;
        wExcp = ZException.last().formatFullUserMessage();
        ZExceptionDLg::adhocMessage("Undelete block",Severity_Severe,nullptr,
            &wExcp,"Error while updating block state on content file <%s>",URIContent.toString());
        return;
      }

      ZFBT[wi].State = ZBS_Used;
      ZBAT.push(ZFBT[wi]);
      ZFBT.erase(wi);
      ContentTBv->ItemModel->removeRow(wIdx.row());

      PoolChanged=true;
      break;
    } // if (pAction==undeleteQAc)
    break;

#ifdef __DEPRECATED__
  case ZPTP_ZDBT:
    if (pAction==removeNullQAc) {
      /* removing a block from pool (any pool) is possible if
       *  - block has a size of 0
       *  - address does NOT point to a valid block on content file  (orphan pool block)
       */
      long wi = long(wIdx.row());
      uint16_t wZBEx=rawCheckContentBlock(PoolId,FdContent,ZDBT[wi]);
      if (wZBEx == ZBEX_Correct) {
        ZExceptionDLg::adhocMessage("Removing null block",Severity_Error,nullptr,
            nullptr,"Block rank %ld is correct and cannot be removed as a null sized and/or orphan block",wi);
        return;
      }

      ZDBT.erase(wi);
      ContentTBv->ItemModel->removeRow(int(wi));

      PoolChanged=true;
      break;
    }
    if (pAction==undeleteQAc) {
      /*  - check block is valid
       *  - move block from ZDBT to ZBAT
       *  - change block state from ZBS_Deleted to ZBS_Used
       */
      long wi = long(wIdx.row());
      uint16_t wZBEx=rawCheckContentBlock(PoolId,FdContent,ZDBT[wi]);
      if (wZBEx != ZBEX_Correct) {
        ZExceptionDLg::adhocMessage("Undelete block",Severity_Error,nullptr,
            nullptr,"Block rank %ld is not correct and cannot be modified",wi);
        return;
      }

      wSt=updateBlockHeaderState(URIContent,FdContent,ZDBT[wi].Address,ZBS_Used);
      if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp;
        wExcp = ZException.last().formatFullUserMessage();
        ZExceptionDLg::adhocMessage("Undelete block",Severity_Severe,nullptr,
            &wExcp,"Error while updating block state on content file <%s>",URIContent.toString());
        return;
      }

      ZDBT[wi].State = ZBS_Used;
      ZBAT.push(ZDBT[wi]);
      ZDBT.erase(wi);
      ContentTBv->ItemModel->removeRow(int(wi));

      PoolChanged=true;
      break;
    } // if (pAction==undeleteQAc)

    break;
#endif // __DEPRECATED__
  }
  return;
}//flexMenActionEvent


void poolVisu::udpdateHeaderWnd() {
  ZStatus wSt=updateHeader();
  switch (wSt) {
  case ZS_NOTHING:
    statusBarMessage("Nothing to update.");
    return;
  case ZS_OMITTED:
    statusBarMessage("Pool is errored.Nothing to update.");
    return;
  case ZS_SUCCESS:
    statusBarMessage("Pool has been successfully updated.");
    return;
  default: {
    utf8VaryingString wAdd = ZException.last().formatFullUserMessage();

      ZExceptionDLg::adhocMessage("Pool update",ZException.last().Severity,nullptr,&wAdd,
        "Error while updating header file <%s>",URIHeader.toString());
      return;
  }
  }// switch
}//udpdateHeaderWnd

ZStatus
poolVisu::updateHeader() {

  if (!PoolChanged)
    return ZS_NOTHING;
  if (PoolNotLoaded)
    return ZS_OMITTED;

//  ZStatus wSt= updateHeaderFromPool(URIHeader,&ZBAT,&ZFBT,&ZDBT);
  ZStatus wSt= updateHeaderFromPool(URIHeader,&ZBAT,&ZFBT,&ZHOT);

  if (wSt==ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Update header",Severity_Information,nullptr,nullptr,
        "Header file <%s> has been updated.",URIHeader.toString());
    PoolChanged=false;
  }
  return wSt;
} // updateHeader()

ZStatus
poolVisu::updateBlockHeaderState(const utf8VaryingString& pURIContent,int pFdContent,zaddress_type &pAddress, ZBlockState_type pState) {
  errno = 0;
  ZDataBuffer wRec;
  off_t wOff = lseek(pFdContent,pAddress,SEEK_SET);
  if (wOff < 0) {
    ZException.getErrno(errno,
        "poolVisu::updateBlockHeaderState",
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error while positionning file <%s> at address <%lld>",
        pURIContent.toString(),pAddress);
    return  (ZS_FILEPOSERR);
  }
  wRec.allocate(sizeof(ZBlockHeader_Export));
  ssize_t wSize = ::read(pFdContent,wRec.Data,sizeof(ZBlockHeader_Export));
  if (wSize<0)
  {
    ZException.getErrno(errno,
        "poolVisu::updateBlockHeaderState",
        ZS_READERROR,
        Severity_Severe,
        "Error while reading file <%s> at address <%lld>",
        pURIContent.toString(),pAddress);
    return  (ZS_READERROR);
  }
  ZBlockHeader_Export* wBHe = (ZBlockHeader_Export*)wRec.Data;
  wBHe->State = pState;  /* block header state does not need to be reversed (uint8_t) */
  wOff = lseek(pFdContent,pAddress,SEEK_SET);
  if (wOff < 0) {
    ZException.getErrno(errno,
        "updateBlockHeaderState",
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error while positionning  at address <%lld>",
        pURIContent.toString(),pAddress);
    return  (ZS_FILEPOSERR);
  }

  wSize = ::write(pFdContent,wRec.Data,sizeof(ZBlockHeader_Export));
  if (wSize <0 ) {
    ZException.getErrno (errno,             // NB: write function sets errno in case of error
        "updateBlockHeaderState",
        ZS_WRITEERROR,
        Severity_Severe,
        "Error while writing block user content (Data). Address %lld size %ld",
        pAddress,
        sizeof(ZBlockHeader_Export));

    return  ZS_WRITEERROR;
  }
  return ZS_SUCCESS;
} // updateBlockHeaderState

ZStatus
poolVisu::getFileBlockDescriptor(__FILEHANDLE__ pFdContent,ZBlockDescriptor& pBDOut,zaddress_type pAddress) {
  ZDataBuffer wBlock;

  ZStatus wSt=rawReadAt(pFdContent,wBlock,sizeof(ZBlockHeader_Export),pAddress);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  ZBlockHeader_Export* wBlockE=(ZBlockHeader_Export* )wBlock.Data;
  if (wBlockE->StartSign != cst_ZFILEBLOCKSTART) {
    return ZS_INVBLOCKADDR;
  }

  wBlockE->deserialize();

  pBDOut.Address = pAddress;
  pBDOut.BlockSize = wBlockE->BlockSize ;
  pBDOut.State = wBlockE->State  ;
  pBDOut.Lock = wBlockE->Lock  ;
  pBDOut.Pid = wBlockE->Pid   ;

  return ZS_SUCCESS;
} // getFileBlockDescriptor





void displayOut(utf8VaryingString& pOut) {
  pOut += "\n";
  fprintf(stdout,pOut.toCChar());
}

const char*
getPoolName(uint8_t pPoolType) {
  switch (pPoolType) {
  case ZPTP_ZBAT:
    return "ZBAT";
  case ZPTP_ZFBT:
    return "ZFBT";
  case ZPTP_ZHOT:
    return "ZHOT";
/*  case ZPTP_ZDBT:
    return "ZDBT";
*/
  default:
    return "Unknown Pool type";
  }
}

ZStatus
fixPool(const utf8VaryingString& pURIContent, int pFdContent,uint8_t pPoolType,ZBlockPool* pPool,
    uint8_t pFlag,std::function<void (utf8VaryingString&)> pDisplay) {
  utf8VaryingString wStr;
  uint16_t wBE;
  wStr.sprintf("    Processing %s.",getPoolName(pPoolType));
  pDisplay(wStr);

  for (long wi=0; wi < pPool->count(); wi++) {
    wBE=rawCheckContentBlock(pPoolType,pFdContent, pPool->Tab(wi));
    if (wBE == ZBEX_Correct)
      continue;
    wStr.sprintf("%ld > %s",wi,decode_ZBEx(wBE).toString());
    pDisplay(wStr);
    if (pFlag==ZPOR_TestRun)
      continue;

    if (wBE & ZBEX_SysBadAddress) {
      wStr = "fixPool-F-SYSERR Severe system error : cannot continue.";
      pDisplay(wStr);
      return ZS_FILEERROR;
    }

    if ((wBE & ZBEX_Orphan)&&(pFlag & ZPOR_FixOrphan)) {
      wStr.sprintf("Removing Orphan block in %s pool rank %ld address %lld",
          getPoolName(pPoolType),wi,pPool->Tab(wi).Address);
      pPool->erase(wi);
      wi--;
      continue;
    }
    if (pFlag & ZPOR_FixSize) {
      if (wBE & ZBEX_PoolZeroSize) {
        wStr.sprintf("<ZBEX_PoolZeroSize> Removing zero sized block from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab(wi).Address);
        pPool->erase(wi);
        wi--;
        continue;
      }
      if (wBE & ZBEX_SizeNotSame) {
        wStr.sprintf("<ZBEX_PoolZeroSize> Adapting pool block size to content block size from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab(wi).Address);
        ZDataBuffer wBlock;
        off_t wFileOffset = lseek(pFdContent,off_t(pPool->Tab(wi).Address),SEEK_SET);
        if (wFileOffset < 0){
          ZException.getErrno(errno,
              _GET_FUNCTION_NAME_,
              ZS_FILEPOSERR,
              Severity_Severe,
              "Error positionning content file.");
          wStr = ZException.last().formatFullUserMessage();
          pDisplay(wStr);
          return ZS_FILEPOSERR;
        }
        wBlock.allocate(sizeof(ZBlockHeader_Export));
        ssize_t wSize=::read(pFdContent,wBlock.Data,sizeof(ZBlockHeader_Export));
        if (wSize < 0) {
          ZException.getErrno(errno,
              _GET_FUNCTION_NAME_,
              ZS_READERROR,
              Severity_Severe,
              "Error reading content file.");
          wStr = ZException.last().formatFullUserMessage();
          pDisplay(wStr);
          return ZS_READERROR;
        }
        ZBlockHeader_Export* wBlockExp=(ZBlockHeader_Export*)wBlock.Data;
        pPool->Tab(wi).BlockSize = reverseByteOrder_Conditional<zsize_type>(wBlockExp->BlockSize) ;
      } // if (wBE & ZBEX_SizeNotSame)
    } // if (wBE & ZPOR_FixSize)
    if (pFlag & ZPOR_FixState) {
      if (wBE & ZBEX_MustBeUsed) {
        wStr.sprintf("<ZPOR_FixState> Changing pool block state to ZBS_Used from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab(wi).Address);

        if (pPool->Tab(wi).State==ZBS_Allocated) {
          wStr.sprintf("<ZPOR_FixState> Found pool block state <ZBS_Allocated> from %s pool rank %ld address %lld. Marking block <ZBS_BeingDeleted>.",
              getPoolName(pPoolType),wi,pPool->Tab(wi).Address);
          pDisplay(wStr);

          pPool->Tab(wi).State = ZBS_BeingDeleted;
          continue;
        }

        pPool->Tab(wi).State = ZBS_Used;
        ZStatus wSt=poolVisu::updateBlockHeaderState(pURIContent,pFdContent,pPool->Tab(wi).Address,ZBS_Used);
        if (wSt!=ZS_SUCCESS) {
          wStr = ZException.last().formatFullUserMessage();
          pDisplay(wStr);
          return wSt;
        }
      } // if (wBE & ZBEX_MustBeUsed)
      if (wBE & ZBEX_MustBeFreeOrDeleted) {
        wStr.sprintf("<ZPOR_FixState> Changing pool block state to ZBS_Free from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab(wi).Address);

        pPool->Tab(wi).State = ZBS_Free;
        ZStatus wSt=poolVisu::updateBlockHeaderState(pURIContent,pFdContent,pPool->Tab(wi).Address,ZBS_Free);
        if (wSt!=ZS_SUCCESS) {
          wStr = ZException.last().formatFullUserMessage();
          pDisplay(wStr);
          return wSt;
        }
      } // if (wBE & ZBEX_MustBeFree)
/*      if (wBE & ZBEX_MustBeDeleted) {
        wStr.sprintf("<ZPOR_FixState> Changing pool block state to ZBS_Deleted from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab(wi).Address);

        pPool->Tab(wi).State = ZBS_Deleted;
        ZStatus wSt=poolVisu::updateBlockHeaderState(pURIContent,pFdContent,pPool->Tab(wi).Address,ZBS_Deleted);
        if (wSt!=ZS_SUCCESS) {
          wStr = ZException.last().formatFullUserMessage();
          pDisplay(wStr);
          return wSt;
        }
      } // if (wBE & ZBEX_MustBeDeleted)
*/

    } //if (pFlag & ZPOR_FixState)
  } // for
  return ZS_SUCCESS;
}


ZStatus
grabFreeForward(const utf8VaryingString& pURIContent,
                int pFdContent,
                size_t pFileSize,
                size_t pStartData,
                ZBlockPool& pZFBT,
                long        &pFreeRank,
                std::function<void (utf8VaryingString&)> pDisplay )
{
  utf8VaryingString wStr;

  ZDataBuffer wRecord;
  size_t wPayload = 10000;

  size_t wCurAddress = pZFBT[pFreeRank].Address+pZFBT[pFreeRank].BlockSize;

  if (wCurAddress >= pFileSize)
    return ZS_SUCCESS;

  if ((wCurAddress + wPayload ) > pFileSize) {
    wPayload = pFileSize - wCurAddress;
    if (wPayload <= sizeof(cst_ZFILEBLOCKSTART))
      return ZS_SUCCESS;
  }

  wRecord.allocate(wPayload);
  unsigned char* wPtr = wRecord.Data ;
  unsigned char* wEnd = wRecord.Data + wPayload ;

  /* get to next adjacent block */
  off_t wOff=lseek(pFdContent,off_t(wCurAddress),SEEK_SET);
  if (wOff < 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning at address <%ld> for file <%s> ",
        wCurAddress,
        pURIContent.toString());
    return  ZS_FILEPOSERR ;
  }
  ssize_t wS = ::read(pFdContent,wRecord.Data,wPayload);
  if (wS==0)
    return ZS_SUCCESS;
  if (wS < 0){
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_READERROR,
        Severity_Severe,
        "Reading size <%ld> from file <%s>.",wPayload,pURIContent.toString());
    return ZS_READERROR;
  }
  ZStatus wSt=ZS_SUCCESS;
  uint32_t* wStartSign = (uint32_t*)wPtr;
  while ((wCurAddress < pFileSize ) && (wPtr < wEnd) && (wSt == ZS_SUCCESS)) {
    wStartSign = (uint32_t*)wPtr;
    while ((*wStartSign!=cst_ZFILEBLOCKSTART) && (wPtr < wEnd) ) {
      wPtr++;
      wCurAddress++;
      wStartSign = (uint32_t*)wPtr;
    }
    if (wPtr==wEnd) {
      if (wCurAddress >= pFileSize)
        return ZS_SUCCESS;

      if ((wCurAddress + wPayload ) > pFileSize) {
        wPayload = pFileSize - wCurAddress;
        if (wPayload <= sizeof(cst_ZFILEBLOCKSTART))
          return ZS_SUCCESS;
      }
      wRecord.allocate(wPayload);
      wPtr = wRecord.Data ;
      wEnd = wRecord.Data + wPayload ;
      wS = ::read(pFdContent,wRecord.Data,wPayload);
      if (wS==0)
        return ZS_SUCCESS;
      if (wS < 0){
        ZException.getErrno(errno,
            _GET_FUNCTION_NAME_,
            ZS_READERROR,
            Severity_Severe,
            "Reading size <%ld> from file <%s>.",wPayload,pURIContent.toString());
        return ZS_READERROR;
      }
      continue ;
    }// if (wPtr==wEnd)
    if (*wStartSign==cst_ZFILEBLOCKSTART) {
      /* got a block header */
      ZBlockHeader_Export* wBHe=(ZBlockHeader_Export*)wStartSign;

      if (wBHe->State != ZBS_Free) {
        wStr.sprintf("            grabFreeForward Cannot grab free block. Following block state is %s.",decode_ZBS(wBHe->State));
        if (pDisplay!=nullptr) {
          pDisplay(wStr);
        }
        else
          fprintf(stdout,wStr.toCChar());
        return ZS_SUCCESS;
      }
      /* aggregate :
       *  initial free block  -> initial block address
       *  + hole if any
       *  found free block (supposed to have been divided)
       */
      zaddress_type wFoundAddress = (zaddress_type)wCurAddress ;
      size_t wFoundBlockSize =  size_t(reverseByteOrder_Conditional<zsize_type>(wBHe->BlockSize));
      size_t wNewSize = wCurAddress - pZFBT[pFreeRank].Address ;
      wNewSize += wFoundBlockSize;
      wStr.sprintf("            grabFreeForward Found following free block at address %lld.",wFoundAddress);
      if (pDisplay!=nullptr) {
        pDisplay(wStr);
      }
      else
        fprintf(stdout,wStr.toCChar());

      /* search for block in free block pool */
      long wi=0;
      for (; wi < pZFBT.count() ; wi ++) {
        if (pZFBT[wi].Address==wFoundAddress) {
          wStr.sprintf("            grabFreeForward address %lld corresponds to pool rank %ld\n"
                       "                            Removing block from free block pool.",wFoundAddress,wi);
          if (pDisplay!=nullptr) {
            pDisplay(wStr);
          }
          else
            fprintf(stdout,wStr.toCChar());

          /* update input rank whenever required */
          if (wi < pFreeRank)
            pFreeRank--;

          pZFBT.erase(wi);
          break;
        }
      }// for
      if (wi==pZFBT.count()) {
        wStr.sprintf( "            grabFreeForward Address %lld does not correspond to any address in pool.",wFoundAddress);
        if (pDisplay!=nullptr) {
          pDisplay(wStr);
        }
        else
          fprintf(stdout,wStr.toCChar());

        return ZS_SUCCESS;
      }

      /* extend block with new size */

      pZFBT[pFreeRank].BlockSize = wNewSize ;

      /* update content file with new block data */
      wOff=lseek(pFdContent,off_t(pZFBT[pFreeRank].Address),SEEK_SET);
      if (wOff < 0) {
        ZException.getErrno(errno,
            _GET_FUNCTION_NAME_,
            ZS_FILEPOSERR,
            Severity_Severe,
            "Error positionning at address <%ld> for file <%s> ",
            pZFBT[pFreeRank].Address,
            pURIContent.toString());
        return  ZS_FILEPOSERR ;
      }

      pZFBT[pFreeRank].State = ZBS_Free;
      ZBlockHeader_Export wBH;
      wBH.set(pZFBT[pFreeRank]);
      wBH.serialize();
      wS=::write(pFdContent,&wBH,sizeof(ZBlockHeader));
      if (wS < 0){
        ZException.getErrno(errno,
            _GET_FUNCTION_NAME_,
            ZS_WRITEERROR,
            Severity_Severe,
            "Writting size <%ld> from file <%s>.",sizeof(ZBlockHeader),pURIContent.toString());
        return ZS_WRITEERROR;
      }
      wStr.sprintf("            grabFreeForward Extended free block rank %ld address %lld to size %ld.",pFreeRank,pZFBT[pFreeRank].Address,pZFBT[pFreeRank].BlockSize);
      if (pDisplay!=nullptr) {
        pDisplay(wStr);
      }
      else
        fprintf(stdout,wStr.toCChar());

      return ZS_SUCCESS; /* successfully done */

    } // if (*wStartSign==cst_ZFILEBLOCKSTART)
  } // while ((wCurAddress ...

  return ZS_SUCCESS;
} //grabFreeForward

ZStatus ZExceptionDisplayAll(std::function<void (utf8VaryingString&)> pDisplay) {
  utf8VaryingString wStr;
  ZStatus wSt= ZS_SUCCESS;
  if (ZException.count()) {
    wStr = "Exception list from most recent to oldest";
    wSt= ZException.last().Status;
  }
  else{
    wStr = "No exception logged.";
  }
  pDisplay(wStr);

  while (ZException.count()) {
    wStr = ZException.popR()->formatFullUserMessage();
    pDisplay(wStr);
  }
  return wSt;
} // ZExceptionDisplayAll


ZStatus
poolVisu::repair(const uriString& pURIContent,const uriString& pURIHeader,uint8_t pFlag, std::function<void (utf8VaryingString&)> pDisplay ) {
  if (!pURIHeader.exists())
    return ZS_FILENOTEXIST;
  if (!pURIContent.exists())
    return ZS_FILENOTEXIST;

  if (pDisplay==nullptr) {
    pDisplay=std::bind(displayOut,std::placeholders::_1);
  }

  utf8VaryingString wStr;
  ZDataBuffer wHeaderContent;
  ZBlockPool ZBAT;
  ZBlockPool ZFBT;
  ZBlockPool ZHOT;
//  ZBlockPool ZDBT;  // Deprecated


  size_t wFileSize = size_t(pURIContent.getFileSize());

  zaddress_type wStartOfData;
  wStr = "Selected run options";
  pDisplay(wStr);

  if (pFlag==ZPOR_TestRun) {
    wStr = "Test run requested (nothing will be done)";
    pDisplay(wStr);
  }
  else {
    if (pFlag & ZPOR_FixState) {
      wStr = "ZPOR_FixState : fix invalid block state in pool or on content file\n"
             "                ZBS_Allocated will be moved to free block pool.\n"
             "                Other states will be set to be compliant with pool :\n"
             "                ZSB_Used for ZBAT , ZBS_Free for ZFBT and ZBS_Deleted for ZDBT.";
    }
    if (pFlag & ZPOR_FixOrphan) {
      if (!wStr.isEmpty())
        wStr +=  "\n" ;

      wStr +=  "ZPOR_FixOrphan : suppress orphan blocks\n"
               "                 Orphan block is a block in pool that does not point to a valid block on content file.";
    }
    if (pFlag & ZPOR_FixSize) {
      if (!wStr.isEmpty())
        wStr +=  "\n" ;

      wStr += "ZPOR_FixNull : suppress null sized blocks in pool and/or in content file";
    }
  }
  wStr += "\n____________________________________________________________________________";
  pDisplay(wStr);


  wStr.sprintf("Opening content file <%s>",pURIContent.toCChar());
  pDisplay(wStr);

  int FdContent = ::open(pURIContent.toCChar(),O_RDWR);
  if (FdContent<0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        "Error opening file <%s> ",
        pURIContent.toString());
    return ZExceptionDisplayAll(pDisplay);
  }
  wStr.sprintf("Opening header file <%s>",pURIHeader.toCChar());
  pDisplay(wStr);

  int FdHeader = ::open(pURIHeader.toCChar(),O_RDWR);
  if (FdHeader<0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        "Error opening file <%s> ",
        pURIHeader.toString());
    return ZExceptionDisplayAll(pDisplay);
  }

  ZStatus wSt=pURIHeader.loadContent(wHeaderContent);

  if (wSt!=ZS_SUCCESS) {
    return ZExceptionDisplayAll(pDisplay);
  }

  wStr.sprintf("Header content loaded : %lu bytes.",wHeaderContent.Size);
  pDisplay(wStr);

  unsigned char* wPtr=wHeaderContent.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)wPtr;
  if (wHe->isNotReversed()) {
    ZException.setMessage("poolVisu::repair",ZS_CORRUPTED,Severity_Severe,"Header is corrupted. File <%s>",pURIHeader.toString());
    return ZExceptionDisplayAll(pDisplay);
  }

  size_t wOffset= size_t(reverseByteOrder_Conditional<zaddress_type>(wHe->OffsetFCB));
  wPtr += wOffset;
  ZFCB_Export* wFCBe = (ZFCB_Export*)(wPtr);
  if (wFCBe->isNotReversed()) {
    ZException.setMessage("poolVisu::repair",ZS_CORRUPTED,Severity_Severe,"File control block is corrupted. File<%s>",pURIHeader.toString());
    return ZExceptionDisplayAll(pDisplay);
  }

  wStartOfData = reverseByteOrder_Conditional<zaddress_type>(wFCBe->StartOfData);

  const unsigned char* wPtrZBAT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZBAT_DataOffset);
  const unsigned char* wPtrZFBT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZFBT_DataOffset);
//  const unsigned char* wPtrZDBT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZDBT_DataOffset);

  wStr="Importing Block access table";
  pDisplay(wStr);

  /* check pool import structure validity for ZBAT */
  ZAExport wZAEMe;
  memmove(&wZAEMe,wPtrZBAT,sizeof(ZAExport));
  wZAEMe.deserialize();
  if ((wZAEMe.StartSign!=cst_ZMSTART) || (wZAEMe.NbElements > cst_maxraisonablevalue)||(wZAEMe.EndianCheck!=cst_EndianCheck_Normal)) {
    ZException.setMessage("poolVisu::repair",ZS_INVBLOCK,Severity_Severe,"ZBAT (block access table) pool header is corrupted. File<%s>",pURIHeader.toString());
    return ZExceptionDisplayAll(pDisplay);
  }
  /* effective ZBAT pool import */

  ZBAT._importPool(wPtrZBAT);

  wStr.sprintf("Block access table imported. %ld blocks defined.",ZBAT.count());
  pDisplay(wStr);

  wStr="Importing Free blocks table.";
  pDisplay(wStr);

  /* check pool import structure validity for ZFBT */
  memmove(&wZAEMe,wPtrZFBT,sizeof(ZAExport));
  wZAEMe.deserialize();
  if ((wZAEMe.StartSign!=cst_ZMSTART) || (wZAEMe.NbElements > cst_maxraisonablevalue)||(wZAEMe.EndianCheck!=cst_EndianCheck_Normal)) {
    ZException.setMessage("poolVisu::repair",ZS_INVBLOCK,Severity_Severe,"ZFBT (free blocks) pool header is corrupted. File<%s>",pURIHeader.toString());
    return ZExceptionDisplayAll(pDisplay);
  }

  /* effective ZFBT pool import */

  ZFBT._importPool(wPtrZFBT);

  wStr.sprintf("Free blocks table imported. %ld blocks defined.",ZFBT.count());
  pDisplay(wStr);

  wStr="Importing Deleted blocks table";
  pDisplay(wStr);

  wStr = "Scroll / fix pools.";
  pDisplay(wStr);

  /* checking ZBAT blocks to free */
  for (long wi=0;wi < ZBAT.count(); wi++) {
    if (ZBAT[wi].BlockSize==0) {
      wStr.sprintf("<ZeroSizedBlock> Found pool block with size equal to 0 from ZBAT pool rank %ld address %lld.",
          wi,ZBAT[wi].Address);
      pDisplay(wStr);
      if (pFlag & ZPOR_FixSize) {
        wStr.sprintf("            <ZPOR_FixSize> suppressing zero sized block from ZBAT pool at rank %ld.",
            wi);
        pDisplay(wStr);
        ZBAT.erase(wi);
        wi--;
        continue;
      }
    }
    if (ZBAT[wi].State==ZBS_Allocated) {
      wStr.sprintf("<ZBS_Allocated> Found pool block state <ZBS_Allocated> from ZBAT pool rank %ld address %lld.",
          wi,ZBAT[wi].Address);
      pDisplay(wStr);
      if (pFlag & ZPOR_FixState) {

        ZBAT[wi].State=ZBS_Free;
        long wj=ZFBT.push(ZBAT[wi]);
        ZBAT.erase(wi);
        wStr.sprintf("            <ZPOR_FixState> Moved block rank %ld from ZBAT pool to free block pool rank %ld.",
            wi,wj);
        pDisplay(wStr);
        wStr =       "            <ZPOR_FixState> Grabbing free block.";
        pDisplay(wStr);
        wSt=grabFreeForward(pURIContent,FdContent ,wFileSize,wStartOfData,ZFBT,wj,pDisplay);
        if (wSt!= ZS_SUCCESS)
          return ZExceptionDisplayAll(pDisplay);
      }
    }
  }

  for (long wi=0;wi < ZFBT.count(); wi++) {
    if (ZFBT[wi].BlockSize==0) {
      wStr.sprintf("<ZeroSizedBlock> Found pool block with size equal to 0 from ZFBT pool rank %ld address %lld.",
          wi,ZFBT[wi].Address);
      pDisplay(wStr);
      if (pFlag & ZPOR_FixSize) {
        wStr.sprintf("            <ZPOR_FixSize> suppressing zero sized block from ZFBT pool at rank %ld.",
            wi);
        pDisplay(wStr);
        ZFBT.erase(wi);
        wi--;
        continue;
      }
    }
  }// for

  wStr = "Updating header file.";
  pDisplay(wStr);

//  wSt= updateHeaderFromPool(pURIHeader,&ZBAT,&ZFBT,&ZDBT);
  wSt= updateHeaderFromPool(pURIHeader,&ZBAT,&ZFBT,&ZHOT);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("poolVisu::repair",wSt,Severity_Severe,"Writing header file. File<%s>",pURIHeader.toString());
   return ZExceptionDisplayAll(pDisplay);
  }

  wStr.sprintf("Header file <%s> updated on disk.",pURIHeader.toString());
  pDisplay(wStr);

  return wSt;
} //poolVisu::repair

ZStatus
//poolVisu::updateHeaderFromPool(const uriString& pURIHeader,ZBlockPool* pZBAT,ZBlockPool* pZFBT,ZBlockPool* pZDBT) {
poolVisu::updateHeaderFromPool(const uriString& pURIHeader,ZBlockPool* pZBAT,ZBlockPool* pZFBT,ZBlockPool* pZHOT) {
  ZDataBuffer wHeaderContent,wNewHeaderContent;

  //  ZDataBuffer WReserved;

  ZStatus wSt=pURIHeader.loadContent(wHeaderContent);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  /*
  ZHeader.OffsetReserved = sizeof(ZHeaderControlBlock_Export) ;
  ZHeader.SizeReserved = ZReserved.Size ;
  ZHeader.OffsetFCB =    ZHeader.OffsetReserved + ZReserved.Size ;
   *
   *  blocks positionning
   *
   * Header
   * Reserved
   * Fcb
   * ZBAT
   * ZFBT
   * ZDBT
   */

  ZHeaderControlBlock_Export* wHCBe= (ZHeaderControlBlock_Export*)wHeaderContent.Data;
//  unsigned char* wPtrReserved=wHeaderContent.Data + size_t(reverseByteOrder_Conditional<zaddress_type>(wHCBe->OffsetReserved));
  size_t wOffsetReserved = size_t(reverseByteOrder_Conditional<zaddress_type>(wHCBe->OffsetReserved));
  size_t wSizeReserved =size_t(reverseByteOrder_Conditional<zsize_type>(wHCBe->SizeReserved));
  //  WReserved.setData(wPtrReserved,wSizeReserved);

  /* first copy all data for header, Reserved , FCB  to new header content */

  wNewHeaderContent.setData(wHeaderContent.Data, sizeof(ZHeaderControlBlock_Export) + wSizeReserved + sizeof( ZFCB_Export ));

  /* point to copied data */
  unsigned char* wPtr = wNewHeaderContent.Data ;
  wHCBe = (ZHeaderControlBlock_Export*)wNewHeaderContent.Data;
  size_t wOffset= size_t(reverseByteOrder_Conditional<zaddress_type>(wHCBe->OffsetFCB));
  wPtr += wOffset;
  ZFCB_Export* wFCBe = (ZFCB_Export*)(wPtr);
  wFCBe=(ZFCB_Export*)(wNewHeaderContent.Data + reverseByteOrder_Conditional<zaddress_type>(wHCBe->OffsetFCB));

  ZDataBuffer wZBATb;
  ZDataBuffer wZFBTb;
  ZDataBuffer wZHOTb;
//  ZDataBuffer wZDBTb;  // Deprecated

  pZBAT->_exportAppendPool(wZBATb);
  pZFBT->_exportAppendPool(wZFBTb);
  pZHOT->_exportAppendPool(wZHOTb);
//  pZDBT->_exportAppendPool(wZDBTb);  // Deprecated

  /* update offsets and sizes within FCB */

  wOffset = reverseByteOrder_Conditional<size_t>(wFCBe->ZBAT_DataOffset); /* NB: ZBAT offset does not change */
  wFCBe->ZBAT_ExportSize = reverseByteOrder_Conditional<size_t>(wZBATb.Size); /* but size may change */

  wOffset += wZBATb.Size ;

  wFCBe->ZFBT_DataOffset = reverseByteOrder_Conditional<size_t>(wOffset);
  wFCBe->ZFBT_ExportSize = reverseByteOrder_Conditional<size_t>(wZFBTb.Size);

  wOffset += wZFBTb.Size ;

  wFCBe->ZHOT_DataOffset = reverseByteOrder_Conditional<size_t>(wOffset);
  wFCBe->ZHOT_ExportSize = reverseByteOrder_Conditional<size_t>(wZHOTb.Size);

  wOffset += wZHOTb.Size ;
/*
  wFCBe->ZDBT_DataOffset = reverseByteOrder_Conditional<size_t>(wOffset);
  wFCBe->ZDBT_ExportSize = reverseByteOrder_Conditional<size_t>(wZDBTb.Size);
*/
  wNewHeaderContent.appendData(wZBATb);
  wNewHeaderContent.appendData(wZFBTb);
  wNewHeaderContent.appendData(wZHOTb);
//  wNewHeaderContent.appendData(wZDBTb); // Deprecated

  return pURIHeader.writeContent(wNewHeaderContent);
} //updateHeaderFromPool


void
poolVisu::PoolChange(int pIdx) {

  udpdateHeaderWnd();

  PoolId=pIdx;
  dataSetup(pIdx);
}


void poolVisu::resizeEvent(QResizeEvent* pEvent) {
  QSize wDlgOld = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (FResizeInitial) {
    FResizeInitial=false;
    return;
  }
  /* message zone is only resized in width */
  QRect wHeaderQR = ui->verticalLayoutWidget->geometry();
  int wWMargHeader = (wDlgOld.width()-wHeaderQR.width());
  int wHeaderWidth=pEvent->size().width() - wWMargHeader;
  ui->verticalLayoutWidget->resize(wHeaderWidth,wHeaderQR.height());  /* resize only in width */

  /* graphic zone is only resized in width */
  QRect wGraphicQR = ui->FileGraphicGVw->geometry();
  int wWMargGraphic = (wDlgOld.width()-wGraphicQR.width());
  int wGraphicWidth=pEvent->size().width() - wWMargGraphic;
  ui->FileGraphicGVw->resize(wGraphicWidth,wGraphicQR.height());  /* resize only in width */


  /* Table view zone is resized both in width and height */
  QRect wBTBv = ui->verticalLayoutWidget_2->geometry();
  //  QRect wBTBv = ui->centerWidgetWDg->geometry();
  int wWMargin = (wDlgOld.width()-wBTBv.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wDlgOld.height() - wBTBv.height();
  int wVH=pEvent->size().height() - wHMargin ;

  ui->verticalLayoutWidget_2->resize(wVW,wVH);

}//poolVisu::resizeEvent


utf8VaryingString
getContentFullBaseNameFromHeader(const uriString& pHeaderUri) {
  utf8VaryingString wContent;

  const utf8_t* wPtr=pHeaderUri.Data;

  size_t wSize=0;
  while (*wPtr++!=0)
    wSize++; /* search string size */

  /* duplicate string */
  utf8_t* wDuplicate = (utf8_t*)malloc(wSize+1);
  if (wDuplicate==nullptr) {
    errno = ENOMEM;
    return utf8VaryingString();
  }
  utf8_t* wPtrOut=wDuplicate;
  wPtr=pHeaderUri.Data;
  while (*wPtr!=0) {
    *wPtrOut++ = *wPtr++;
  }
  *wPtrOut=0;

  while (*wPtrOut--!='.');/* search file extension delimiter */

  wPtrOut--; /* skip dot */
  while (*wPtrOut--!='-');/* search file base name header name extension delimiter */

  *wPtrOut=0; /* set to end of string */

  wContent = wPtrOut;

  zfree(wDuplicate);
  return wContent;
}
void
poolVisu::statusBarMessage(const char* pFormat,...) {
  if (pFormat==nullptr)
    ui->statusbar->showMessage("<nullptr>");


  utf8VaryingString wStr,wFormat;

  wFormat=QObject::tr(pFormat).toUtf8().data();

  va_list ap;
  va_start(ap, pFormat);

  wStr.vsnprintf(250,wFormat.toCChar(),ap);
  va_end(ap);

  ui->statusbar->showMessage(wStr.toCChar(),cst_MessageDuration);
}
