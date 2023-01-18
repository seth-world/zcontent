#include "poolvisu.h"
#include "ui_poolvisu.h"

#include <fcntl.h>

using namespace std::placeholders ; // for std::bind
#include <functional>               // for std::function<>


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

#include <qevent.h>
#include <QStandardItem>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QActionGroup>

extern const int cst_maxraisonablevalue;

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
  ui->PoolCBx->addItem("ZDBT");

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

  unlockHeaderQAc = new QAction("Unlock header file",this);
  listMEn->addAction(unlockHeaderQAc);
  actionAGp->addAction(unlockHeaderQAc);

  QObject::connect(ui->PoolCBx, SIGNAL(currentIndexChanged(int)), this, SLOT(PoolChange(int)));
  QObject::connect(ContentTBv, SIGNAL(destroyed()), this, SLOT(udpdateHeaderWnd()));

  QObject::connect(ui->SaveBTn, SIGNAL(clicked()), this, SLOT(udpdateHeaderWnd()));
  QObject::connect(ui->RefreshBTn, SIGNAL(clicked()), this, SLOT(refresh()));

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


    return;
  }
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
  URIHeader.loadContent(HeaderContent);
  dataSetup(ui->PoolCBx->currentIndex());
}

void poolVisu::dataSetup(int pPoolid) {

  ZBAT.clear();
  ZFBT.clear();
  ZDBT.clear();

  PoolId = pPoolid;

  utf8VaryingString wStr;

  ContentSize = size_t(URIContent.getFileSize());

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

  const unsigned char* wPtrZBAT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZBAT_DataOffset);
  const unsigned char* wPtrZFBT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZFBT_DataOffset);
  const unsigned char* wPtrZDBT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZDBT_DataOffset);

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

    /* check pool import structure validity for ZDBT */
    memmove(&wZAEMe,wPtrZDBT,sizeof(ZAExport));
    wZAEMe.deserialize();

    if ((wZAEMe.StartSign!=cst_ZMSTART) || (wZAEMe.NbElements > cst_maxraisonablevalue)||(wZAEMe.EndianCheck!=cst_EndianCheck_Normal)) {
      PoolNotLoaded = true;
      break;
    }
    /* effective pool import */
    ZDBT._importPool(wPtrZDBT);
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
    wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZDBT_DataOffset);
    ui->PoolNameLBl->setText("Deleted blocks table");
    break;
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

    uint16_t wBE=checkContentBlock(PoolId,FdContent, wBD);
    if (wBE==ZBEX_Correct) {
      wRow << createItem(" ");
    }
    else {
      wRow << createItem(decode_ZBEx(wBE));
      FErrored = true;
    }
  ContentTBv->ItemModel->appendRow(wRow);

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


} // poolVisu::dataSetup



void poolVisu::poolMouseCallback(int pZEF, QMouseEvent *pEvent)
{

  QModelIndex wIdx;
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
  ContentVisu->goToAddress(wAddress);
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
  if (blockVisuQAc==nullptr) {
    blockVisuQAc=new QAction("view content");
    flexAGp->addAction(blockVisuQAc);
  }

  flexMEn->addAction(blockVisuQAc);

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
      blockVisuQAc->setEnabled(false);
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
      blockVisuQAc->setEnabled(false);
    }

    break;

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

  if (pAction==blockVisuQAc) {
    QVariant wV;
    wV=ContentTBv->ItemModel->item(wIdx.row(),0)->data(Qt::UserRole);
    zaddress_type wAddress = wV.value<zaddress_type>();
    if (ContentVisu==nullptr)
      ContentVisu= new ZRawMasterFileVisu(this);
    ContentVisu->setup(URIContent, FdContent);
    ContentVisu->setModal(false);
    ContentVisu->goToAddress(wAddress);
    ContentVisu->show();

    if (isIndexFile) {
      ContentVisu->setVisuIndexFile();
    }
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

      wSt = grabFreeForward(URIContent,FdContent,ContentSize , StartOfData,ZFBT,widx,nullptr);

      ContentTBv->ItemModel->removeRow(int(wi));
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
      uint16_t wZBEx=checkContentBlock(PoolId,FdContent,ZBAT[wi]);
      if (wZBEx == ZBEX_Correct) {
        ZExceptionDLg::adhocMessage("Removing block",Severity_Error,nullptr,
            nullptr,"Block rank %ld is correct and cannot be removed as a null sized and/or orphan block",wi);
        return;
      }

      ZBAT.erase(wi);
      ContentTBv->ItemModel->removeRow(int(wi));

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
      uint16_t wZBEx=checkContentBlock(PoolId,FdContent,ZFBT[wi]);
      if (wZBEx == ZBEX_Correct) {
        ZExceptionDLg::adhocMessage("Removing block",Severity_Error,nullptr,
            nullptr,"Block rank %ld is correct and cannot be removed as a null sized and/or orphan block",wi);
        return;
      }

      ZFBT.erase(wi);
      ContentTBv->ItemModel->removeRow(int(wi));

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
    break;


  case ZPTP_ZDBT:
    if (pAction==removeNullQAc) {
      /* removing a block from pool (any pool) is possible if
       *  - block has a size of 0
       *  - address does NOT point to a valid block on content file  (orphan pool block)
       */
      long wi = long(wIdx.row());
      uint16_t wZBEx=checkContentBlock(PoolId,FdContent,ZDBT[wi]);
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
      uint16_t wZBEx=checkContentBlock(PoolId,FdContent,ZDBT[wi]);
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
    }

    break;
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

  ZStatus wSt= updateHeaderFromPool(URIHeader,&ZBAT,&ZFBT,&ZDBT);

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
  ssize_t wSize = ::read(pFdContent,wRec.DataChar,sizeof(ZBlockHeader_Export));
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

  wSize = ::write(pFdContent,wRec.DataChar,sizeof(ZBlockHeader_Export));
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

uint16_t
poolVisu::checkContentBlock(int pPoolId,int pFdContent,ZBlockDescriptor& pBlockDesc) {
  ZDataBuffer wBlock;
  uint16_t  wRet = ZBEX_Correct;
  off_t wFileOffset = lseek(pFdContent,off_t(pBlockDesc.Address),SEEK_SET);
  if (wFileOffset < 0){
    wRet |= ZBEX_SysBadAddress;
    return wRet;
  }
  wBlock.allocate(sizeof(ZBlockHeader_Export));
  ssize_t wSize=::read(pFdContent,wBlock.DataChar,sizeof(ZBlockHeader_Export));
  if (wSize < 0) {
    wRet |= ZBEX_SysBadAddress;
    return wRet;
  }
  if (pBlockDesc.BlockSize==0) {
    wRet |= ZBEX_PoolZeroSize;
  }
  ZBlockHeader_Export* wBlockE=(ZBlockHeader_Export* )wBlock.Data;
  if (wBlockE->StartSign != cst_ZFILEBLOCKSTART) {
    wRet |= ZBEX_Orphan;
  }
  zsize_type wSize1 = reverseByteOrder_Conditional<zsize_type>(wBlockE->BlockSize);
  if (wSize1==0) {
    wRet |= ZBEX_ContentZeroSize;
  }
  if (wSize1!=pBlockDesc.BlockSize) {
    wRet |= ZBEX_Size;
  }

  if ((pPoolId==ZPTP_ZBAT)&&(pBlockDesc.State!=ZBS_Used)) {
    wRet |= ZBEX_MustBeUsed ;
  }
  if ((pPoolId==ZPTP_ZFBT)&&(pBlockDesc.State!=ZBS_Free)) {
    wRet |= ZBEX_MustBeFree;
  }
  if ((pPoolId==ZPTP_ZDBT)&&(pBlockDesc.State!=ZBS_Deleted)) {
    wRet |= ZBEX_MustBeDeleted;
  }

  return wRet;
} // checkContentBlock


utf8VaryingString
decode_ZBEx(uint16_t pBEx) {

  /* first preemptive statuses : one of these two cannot be combined with others */
  if (pBEx==ZBEX_Correct)
    return "<ZBEX_Correct> Pool block is correct";

  if (pBEx==ZBEX_SysBadAddress)
    return "<ZBEX_SysBadAddress> Cannot seed/read at given address on content file";

  /* then cumulative statuses : check following priority */

  if (pBEx & ZBEX_Orphan)
    return "<ZBEX_Orphan> Orphan block:<cst_ZFILEBLOCKSTART> missing";

  if (pBEx & ZBEX_PoolZeroSize)
    return "<ZBEX_PoolZeroSize> Block size in pool has zero value";

  if (pBEx & ZBEX_ContentZeroSize)
    return "<ZBEX_ContentZeroSize> Block size on content file has zero value ";

  /* these following are mutually exclusive */
  if (pBEx & ZBEX_MustBeUsed)
    return "<ZBEX_MustBeUsed> Block state must be ZBS_Used";
  if (pBEx & ZBEX_MustBeFree)
    return "<ZBEX_MustBeFree> Block state must be ZBS_Free";
  if (pBEx & ZBEX_MustBeDeleted)
    return "<ZBEX_MustBeDeleted> Block state must be ZBS_Deleted";

  if (pBEx & ZBEX_Size)
    return "<ZBEX_Size> Block size in pool differs with block size on content file.";

  return "<?????> Unknown block check error ";

}


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
  case ZPTP_ZDBT:
    return "ZDBT";
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
    wBE=poolVisu::checkContentBlock(pPoolType,pFdContent, pPool->Tab[wi]);
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
          getPoolName(pPoolType),wi,pPool->Tab[wi].Address);
      pPool->erase(wi);
      wi--;
      continue;
    }
    if (pFlag & ZPOR_FixSize) {
      if (wBE & ZBEX_PoolZeroSize) {
        wStr.sprintf("<ZBEX_PoolZeroSize> Removing zero sized block from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab[wi].Address);
        pPool->erase(wi);
        wi--;
        continue;
      }
      if (wBE & ZBEX_Size) {
        wStr.sprintf("<ZBEX_PoolZeroSize> Adapting pool block size to content block size from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab[wi].Address);
        ZDataBuffer wBlock;
        off_t wFileOffset = lseek(pFdContent,off_t(pPool->Tab[wi].Address),SEEK_SET);
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
        ssize_t wSize=::read(pFdContent,wBlock.DataChar,sizeof(ZBlockHeader_Export));
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
        pPool->Tab[wi].BlockSize = reverseByteOrder_Conditional<zsize_type>(wBlockExp->BlockSize) ;
      } // if (wBE & ZBEX_Size)
    } // if (wBE & ZPOR_FixSize)
    if (pFlag & ZPOR_FixState) {
      if (wBE & ZBEX_MustBeUsed) {
        wStr.sprintf("<ZPOR_FixState> Changing pool block state to ZBS_Used from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab[wi].Address);

        if (pPool->Tab[wi].State==ZBS_Allocated) {
          wStr.sprintf("<ZPOR_FixState> Found pool block state <ZBS_Allocated> from %s pool rank %ld address %lld. Marking block <ZBS_BeingDeleted>.",
              getPoolName(pPoolType),wi,pPool->Tab[wi].Address);
          pDisplay(wStr);

          pPool->Tab[wi].State = ZBS_BeingDeleted;
          continue;
        }

        pPool->Tab[wi].State = ZBS_Used;
        ZStatus wSt=poolVisu::updateBlockHeaderState(pURIContent,pFdContent,pPool->Tab[wi].Address,ZBS_Used);
        if (wSt!=ZS_SUCCESS) {
          wStr = ZException.last().formatFullUserMessage();
          pDisplay(wStr);
          return wSt;
        }
      } // if (wBE & ZBEX_MustBeUsed)
      if (wBE & ZBEX_MustBeFree) {
        wStr.sprintf("<ZPOR_FixState> Changing pool block state to ZBS_Free from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab[wi].Address);

        pPool->Tab[wi].State = ZBS_Free;
        ZStatus wSt=poolVisu::updateBlockHeaderState(pURIContent,pFdContent,pPool->Tab[wi].Address,ZBS_Free);
        if (wSt!=ZS_SUCCESS) {
          wStr = ZException.last().formatFullUserMessage();
          pDisplay(wStr);
          return wSt;
        }
      } // if (wBE & ZBEX_MustBeFree)
      if (wBE & ZBEX_MustBeDeleted) {
        wStr.sprintf("<ZPOR_FixState> Changing pool block state to ZBS_Deleted from %s pool rank %ld address %lld",
            getPoolName(pPoolType),wi,pPool->Tab[wi].Address);

        pPool->Tab[wi].State = ZBS_Deleted;
        ZStatus wSt=poolVisu::updateBlockHeaderState(pURIContent,pFdContent,pPool->Tab[wi].Address,ZBS_Deleted);
        if (wSt!=ZS_SUCCESS) {
          wStr = ZException.last().formatFullUserMessage();
          pDisplay(wStr);
          return wSt;
        }
      } // if (wBE & ZBEX_MustBeDeleted)

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
  ssize_t wS = ::read(pFdContent,wRecord.DataChar,wPayload);
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
      wS = ::read(pFdContent,wRecord.DataChar,wPayload);
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
  ZBlockPool ZDBT;


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
  const unsigned char* wPtrZDBT= wPtr + reverseByteOrder_Conditional<size_t>(wFCBe->ZDBT_DataOffset);

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

  /* check pool import structure validity for ZDBT */
  memmove(&wZAEMe,wPtrZDBT,sizeof(ZAExport));
  wZAEMe.deserialize();
  if ((wZAEMe.StartSign!=cst_ZMSTART) || (wZAEMe.NbElements > cst_maxraisonablevalue)||(wZAEMe.EndianCheck!=cst_EndianCheck_Normal)) {
    ZException.setMessage("poolVisu::repair",ZS_INVBLOCK,Severity_Severe,"ZDBT (deleted blocks) pool header is corrupted. File<%s>",pURIHeader.toString());
    return ZExceptionDisplayAll(pDisplay);
  }

  /* effective ZDBT pool import */

  ZDBT._importPool(wPtrZDBT);

  wStr.sprintf("Deleted blocks table imported. %ld blocks defined.",ZDBT.count());
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

  for (long wi=0;wi < ZDBT.count(); wi++) {
    if (ZDBT[wi].BlockSize==0) {
      wStr.sprintf("<ZeroSizedBlock> Found pool block with size equal to 0 from ZDBT pool rank %ld address %lld.",
          wi,ZDBT[wi].Address);
      pDisplay(wStr);
      if (pFlag & ZPOR_FixSize) {
        wStr.sprintf("            <ZPOR_FixSize> suppressing zero sized block from ZDBT pool at rank %ld.",
            wi);
        pDisplay(wStr);
        ZDBT.erase(wi);
        wi--;
        continue;
      }
    }
  }// for

  /*
  wSt = fixPool(pURIContent, FdContent,ZPTP_ZBAT,&ZBAT,pFlag,pDisplay);
  if (wSt!=ZS_SUCCESS)
    return ZExceptionDisplayAll(pDisplay);


  wSt = fixPool(pURIContent, FdContent,ZPTP_ZFBT,&ZFBT,pFlag,pDisplay);
  if (wSt!=ZS_SUCCESS)
    return ZExceptionDisplayAll(pDisplay);
  wSt = fixPool(pURIContent, FdContent,ZPTP_ZDBT,&ZDBT,pFlag,pDisplay);
  if (wSt!=ZS_SUCCESS)
    return ZExceptionDisplayAll(pDisplay);
*/
  wStr = "Updating header file.";
  pDisplay(wStr);

  wSt= updateHeaderFromPool(pURIHeader,&ZBAT,&ZFBT,&ZDBT);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("poolVisu::repair",wSt,Severity_Severe,"Writing header file. File<%s>",pURIHeader.toString());
   return ZExceptionDisplayAll(pDisplay);
  }

  wStr.sprintf("Header file <%s> updated on disk.",pURIHeader.toString());
  pDisplay(wStr);

  return wSt;
} //poolVisu::repair

ZStatus
poolVisu::updateHeaderFromPool(const uriString& pURIHeader,ZBlockPool* pZBAT,ZBlockPool* pZFBT,ZBlockPool* pZDBT) {
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
  wFCBe=(ZFCB_Export*)(wNewHeaderContent.Data + size_t(reverseByteOrder_Conditional<zaddress_type>(wHCBe->OffsetFCB)));

  ZDataBuffer wZBATb;
  ZDataBuffer wZFBTb;
  ZDataBuffer wZDBTb;

  pZBAT->_exportAppendPool(wZBATb);
  pZFBT->_exportAppendPool(wZFBTb);
  pZDBT->_exportAppendPool(wZDBTb);

  /* update offsets and sizes within FCB */

  wOffset = reverseByteOrder_Conditional<size_t>(wFCBe->ZBAT_DataOffset); /* NB: ZBAT offset does not change */
  wFCBe->ZBAT_ExportSize = reverseByteOrder_Conditional<size_t>(wZBATb.Size); /* but size may change */

  wOffset += wZBATb.Size ;

  wFCBe->ZFBT_DataOffset = reverseByteOrder_Conditional<size_t>(wOffset);
  wFCBe->ZFBT_ExportSize = reverseByteOrder_Conditional<size_t>(wZFBTb.Size);

  wOffset += wZFBTb.Size ;

  wFCBe->ZDBT_DataOffset = reverseByteOrder_Conditional<size_t>(wOffset);
  wFCBe->ZDBT_ExportSize = reverseByteOrder_Conditional<size_t>(wZDBTb.Size);

  wNewHeaderContent.appendData(wZBATb);
  wNewHeaderContent.appendData(wZFBTb);
  wNewHeaderContent.appendData(wZDBTb);
  return pURIHeader.writeContent(wNewHeaderContent);
}


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
