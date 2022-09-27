#include "zcontentvisumain.h"
#include "ui_zcontentvisumain.h"

#include <QStandardItemModel>
#include <qaction.h>
#include <qactiongroup.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <zio/zdir.h>
#include <stdio.h>
#include <fcntl.h>

#include <zcontent/zrandomfile/zrandomfile.h>
#include <zcontent/zindexedfile/zmasterfile.h>

#include "displaymain.h"
#include "zscan.h"


#include <zexceptiondlg.h>

#include <zcontent/zindexedfile/zmfdictionary.h>

#include <dicedit.h>
#include <qfiledialog.h>

#include <texteditmwn.h>
#include <zqt/zqtwidget/zqtableview.h>


#define __FIXED_FONT__ "courrier"

using namespace std;
using namespace zbs;

ZContentVisuMain::ZContentVisuMain(QWidget *parent) :QMainWindow(parent),
                                                     ui(new Ui::ZContentVisuMain)
{
  ui->setupUi(this);

  GetRawQAc = new QAction(QCoreApplication::translate("ZContentVisuMain", "Open raw file", nullptr),this);

  ui->fileMEn->insertAction(ui->openZMFQAc ,GetRawQAc);

  ui->CurAddressLBl->setText("0");

  setWindowTitle("File dump and explore");

  ui->ZRFVersionLBl->setText(getVersionStr(__ZRF_VERSION__).toCChar());
  ui->ZMFVersionLBl->setText(getVersionStr(__ZMF_VERSION__).toCChar());

  /* columns : group of (4 byte hexa plus 1) x 4 - 1 large for ascii  = 20 + 1 */

  /* per line : 4 x 4 bytes
   *  addressing a byte column  : row = integer (address / 4x 4 bytes)
   *  column : remainder = address - integer (address / 4x 4 bytes)
   *  if remainder < 4
   *    column : remainder / 4
   *  if remainder < 8
   *    column : (remainder / 8) + 1
   *  if remainder < 16
   *    column : (remainder / 16) + 2
   *  address - integer (address / 4x 4 bytes) / (4 + 1)
*/
  VisuTBv = new ZQTableView(this);

  VisuTBv->newModel(21);
/*
  VisuTBv->setModel(new QStandardItemModel(this));
  VisuTBv->model()->setColumnCount(21);
  VisuTBv->setShowGrid(true);
  VisuTBv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  VisuTBv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

//  VisuTBv->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Hexadecimal")));

//  VisuTBv->setSpan();
*/
  ui->verticalLayout->addWidget(VisuTBv);

//  QHeaderView* wVisuHead=new QHeaderView(Qt::Horizontal,VisuTBv);


  for (int wi=0;wi < 20;wi++)
    VisuTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(tr(" ")));
  VisuTBv->ItemModel->setHorizontalHeaderItem(20,new QStandardItem(tr("Ascii")));


  DictionaryQAc=new QAction("Dictionaries",this);
  ui->menubar->addAction(DictionaryQAc);

  mainQAg = new QActionGroup(this);

  mainQAg->addAction(DictionaryQAc);
  mainQAg->addAction(ui->setfileQAc); /* set the current file to be open */

  mainQAg->addAction(ui->openByTypeQAc);  /* open according file extension */

  mainQAg->addAction(ui->openRawQAc);/* force open as raw file */
  mainQAg->addAction(ui->openZRFQAc);/* force open as ZRF file */
  mainQAg->addAction(ui->openZMFQAc);/* force open as ZMF file */
  mainQAg->addAction(ui->openZRHQAc);/* force open as header file */

  mainQAg->addAction(GetRawQAc);     /* Shortcut to open file as raw */

  mainQAg->addAction(ui->surfaceScanZRFQAc); /* surface scan ZRF file */
   mainQAg->addAction(ui->surfaceScanRawQAc); /* surface scan raw file */

  mainQAg->addAction(ui->closeQAc);

  mainQAg->addAction(ui->QuitQAc);

  mainQAg->addAction(ui->displayHCBQAc);
  mainQAg->addAction(ui->displayFCBQAc);
  mainQAg->addAction(ui->displayMCBQAc);

  /* dictionary sub-choice */
  mainQAg->addAction(ui->DictionaryQAc);

  /* dictionary menu */
  mainQAg->addAction(ui->dictionaryQAc);
//  actionGroup->addAction(ui->dicLoadXmlQAc);

  /* display pool choices */

  mainQAg->addAction(ui->displayZBATQAc);
  mainQAg->addAction(ui->displayZDBTQAc);
  mainQAg->addAction(ui->displayZFBTQAc);

  mainQAg->addAction(ui->HeaderRawUnlockQAc);

  mainQAg->addAction(ui->unlockZRFQAc);

  mainQAg->addAction(ui->clearQAc);
  mainQAg->addAction(ui->cloneQAc);
  mainQAg->addAction(ui->extendQAc);
  mainQAg->addAction(ui->truncateQAc);
  mainQAg->addAction(ui->reorganizeQAc);
  mainQAg->addAction(ui->upgradeZRFtoZMFQAc);
  mainQAg->addAction(ui->rebuildHeaderQAc);

/* ZMFMEn */
  mainQAg->addAction(ui->runRepairQAc); /* repair all indexes */
  mainQAg->addAction(ui->removeIndexQAc);
  mainQAg->addAction(ui->addIndexQAc);      /* adds an index from an xml definition */
  mainQAg->addAction(ui->downgradeZMFtoZRFQAc);
  mainQAg->addAction(ui->rebuildIndexQAc);      /* rebuild single index */
  mainQAg->addAction(ui->extractIndexQAc);      /* extract xml index definition */
  mainQAg->addAction(ui->extractAllIndexesQAc); /* extract all xml index definitions */
  mainQAg->addAction(ui->reorganizeZMFQAc);
  mainQAg->addAction(ui->MCBReportQAc);         /* list Master Control Block */

  ui->openByTypeQAc->setVisible(true);  /* open file by type is always visible */

  ui->openRawQAc->setVisible(false);
  ui->openZRFQAc->setVisible(false);
  ui->openZMFQAc->setVisible(false);
  ui->openZRHQAc->setVisible(false);
  ui->unlockZRFQAc->setVisible(false);
  ui->closeQAc->setVisible(false);

  ui->rawMEn->setEnabled(false);
  ui->ZRFMEn->setEnabled(false);
  ui->headerMEn->setEnabled(false);
  ui->ZMFMEn->setEnabled(false);

  ui->RecordFRm->setVisible(false);
  ui->SequentialFRm->setVisible(false);

  ui->displayHCBQAc->setEnabled(false);
  ui->displayMCBQAc->setEnabled(false);
  ui->displayFCBQAc->setEnabled(false);


  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(false);
  ui->LoadAllBTn->setVisible(false);

  ui->ProgressPGb->setVisible(false);

  ui->OpenModeLbl->setVisible(false);
  ui->ClosedLBl->setVisible(true);


  ui->searchTypeCBx->addItem("Ascii case sensitive");       /* 0 */
  ui->searchTypeCBx->addItem("Ascii no case");              /* 1 */
  ui->searchTypeCBx->addItem("Hexa free sequence");         /* 2 */
  ui->searchTypeCBx->addItem("File block start F9F9F9F9");  /* 3 */
  ui->searchTypeCBx->addItem("Data block start F5F5F5F5");  /* 4 */
  ui->searchTypeCBx->addItem("Field start      F4F4F4F4");  /* 5 */
  ui->searchTypeCBx->addItem("Data block end   FCFCFCFC");  /* 6 */


  QObject::connect(ui->BackwardBTn, SIGNAL(pressed()), this, SLOT(backward()));
  QObject::connect(ui->ForwardBTn, SIGNAL(pressed()), this, SLOT(forward()));
  QObject::connect(ui->LoadAllBTn, SIGNAL(pressed()), this, SLOT(loadAll()));

  QObject::connect(mainQAg, SIGNAL(triggered(QAction*)), this, SLOT(actionMenuEvent(QAction*)));
  QObject::connect(ui->searchFwdBTn, SIGNAL(pressed()), this, SLOT(searchFwd()));
  QObject::connect(ui->searchBckBTn, SIGNAL(pressed()), this, SLOT(searchBck()));
  QObject::connect(ui->searchTypeCBx, SIGNAL(currentIndexChanged(int)), this, SLOT(searchCBxChanged(int)));
  QObject::connect(VisuTBv, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(VisuDoubleClicked(QModelIndex)));

}

ZContentVisuMain::~ZContentVisuMain()
{
  if (entityWnd)
    delete entityWnd;

  if (RandomFile)
    if (RandomFile->isOpen())
      {
        RandomFile->zclose();
        delete RandomFile;
      }
  if (MasterFile)
    {
      if (MasterFile->isOpen())
        MasterFile->zclose();
      delete MasterFile;
    }
  if (Fd>=0)
    ::close(Fd);

  delete ui;
}


ssize_t computeOffsetFromCoord(int pRow, int pCol) {
  int wD=0;
  ssize_t wOffset =0;
  for (int wi=0;wi < pCol ; wi++) {

    if (wD==4) {
      wD=0;
      continue;
    }
    else
      wD++;
    wOffset ++ ;
  }
  for (int wi=0;wi < pRow ; wi++)
    wOffset += 16 ;
  return wOffset;
}

void ZContentVisuMain::VisuDoubleClicked(QModelIndex pIdx) {
  setSearchOffset(computeOffsetFromCoord(pIdx.row(),pIdx.column()));
}

void ZContentVisuMain::setSearchOffset(ssize_t pOffset) {
  utf8VaryingString wStr;
  SearchOffset=pOffset;
  wStr.sprintf("%ld",SearchOffset);
  ui->CurAddressLBl->setText(wStr.toCChar());
}

void ZContentVisuMain::searchCBxChanged(int pIndex){
  switch(pIndex) {
  case 0:
  case 1:
  case 2:
    SearchOffset=-1;
    return ;
  case 3:
    ui->searchLEd->clear();
    ui->searchLEd->setText("F9F9F9F9");
    SearchOffset=-1;
    return;
  case 4:
    ui->searchLEd->clear();
    ui->searchLEd->setText("F5F5F5F5");
    SearchOffset=-1;
    return;
  case 5:
    ui->searchLEd->clear();
    ui->searchLEd->setText("F4F4F4F4");
    SearchOffset=-1;
    return;
  case 6:
    ui->searchLEd->clear();
    ui->searchLEd->setText("FCFCFCFC");
    SearchOffset=-1;
    return;
  }
  return;
}

void ZContentVisuMain::searchFwd() {
  utf8VaryingString wStr;

  if (SearchOffset<0)
    SearchOffset=0;

  SelectedBackGround = QVariant(QBrush(Qt::cyan));
  /* analyzis of string to search */

  switch (ui->searchTypeCBx->currentIndex()) {
  case 0:
    if (searchAscii(false,false)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      ui->CurAddressLBl->setText(wStr.toCChar());
    }
    else
      ui->CurAddressLBl->setText("--");
    return;
  case 1:
    if (searchAscii(true,false)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      ui->CurAddressLBl->setText(wStr.toCChar());
    }
    else
      ui->CurAddressLBl->setText("--");
    return;
  default:
    if (searchHexa(false)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      ui->CurAddressLBl->setText(wStr.toCChar());
    }
    else
      ui->CurAddressLBl->setText("--");
    return;
  }
}// searchFwd

void ZContentVisuMain::searchBck() {
  utf8VaryingString wStr;

  if (SearchOffset<0)
    SearchOffset=0;

  SelectedBackGround = QVariant(QBrush(Qt::cyan));
  /* analyzis of string to search */

  switch (ui->searchTypeCBx->currentIndex()) {
  case 0:
    if (searchAscii(false,true)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      ui->CurAddressLBl->setText(wStr.toCChar());
    }
    else
      ui->CurAddressLBl->setText("--");
    return;
  case 1:
    if (searchAscii(true,true)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      ui->CurAddressLBl->setText(wStr.toCChar());
    }
    else
      ui->CurAddressLBl->setText("--");
    return;
  default:
    if (searchHexa(true)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      ui->CurAddressLBl->setText(wStr.toCChar());
    }
    else
      ui->CurAddressLBl->setText("--");
    return;
  }
}


uint8_t getHexaFromChar(uint8_t pChar) {
  if ((pChar >= '0') && (pChar <= '9')) {
    return uint8_t(pChar - '0');
  }
  if ((pChar >= 'a') && (pChar <= 'f')) {
    return uint8_t(pChar - 'a' + 10 );
  }
  if ((pChar >= 'A') && (pChar <= 'F')) {
    return uint8_t(pChar - 'A' + 10) ;
  }
  return -1;
}// getHexaFromChar

int getHexaDigits(uint8_t* &pChar,uint8_t & pHexaValue) {
  uint8_t Value ;
  Value =   getHexaFromChar(*pChar);
  if (Value < 0)
    return 1;
  pHexaValue = Value * 16 ;
  pChar++;
  Value =   getHexaFromChar(*pChar);
  if (Value < 0)
    return 1;
  pHexaValue += Value ;
  pChar++;
  return 0;
}// getHexaDigits

class VisuLineCol {
public:
  int line=0;
  int col=0;
  VisuLineCol& compute(ssize_t wOffset) {
    /* highlight searched result in table view
   * start block :
     offset / 16 -> gives the line

    offset - (offset / 16) gives the column
                     */

    double wOf = wOffset;
    line = int(wOf / 16.0);

    col = int(wOf - (double (line)* 16.0));

    col = col + (col / 4);

    return *this;
  }

};

/* highlight searched result in table view
   * start block :
     offset / 16 -> gives the line

offset - (offset / 16) gives the column


    double wOf = SearchOffset;
int wStartLine = int(wOf / 16.0);

int wStartColumn = int(wOf - (double (wLine)* 16.0));

wStartColumn = wStartColumn + (wStartColumn / 4);
*/
void
ZContentVisuMain::setSelectionBackGround(QVariant& pBackground,ssize_t pOffset,size_t pSize,bool pScrollTo) {
  VisuLineCol wVLC;
  wVLC.compute(pOffset);
  int wStartLine = wVLC.line;
  int wStartColumn = wVLC.col;
/*
  wVLC.compute(pOffset+pSize);
  int wEndLine = wVLC.line;
  int wEndColumn = wVLC.col;
*/
  int wCurLine = wStartLine;
  int wCurCol = wStartColumn;
  int wCount=0;
  int wColLimit = Width + 3 ;

  /* highlight process */
  QStandardItem* wFirstItem=VisuTBv->ItemModel->item(wCurLine,wCurCol);
  if (wFirstItem==nullptr) {
    wCurCol++;
    if (wCurCol >= wColLimit ) {
      wCurCol = 0;
      wCurLine ++;
    }
    wFirstItem = VisuTBv->ItemModel->item(wCurLine,wCurCol);
  }
  wFirstItem->setData(pBackground, Qt::BackgroundRole);
  while (wCount < pSize) {

    QStandardItem* wItem=VisuTBv->ItemModel->item(wCurLine,wCurCol);
    if (wItem!=nullptr) {
      wItem->setData(pBackground, Qt::BackgroundRole);
      wCount++;
    }
    wCurCol++;
    if (wCurCol >= wColLimit ) {
      wCurCol = 0;
      wCurLine ++;
    }
  }// while true

  if (pScrollTo) {
    QModelIndex wIdx=VisuTBv->ItemModel->indexFromItem(wFirstItem);
    VisuTBv->scrollTo(wIdx);
  }

} // ZContentVisuMain::setSelectionBackGround


bool ZContentVisuMain::searchHexa(bool pReverse) {

  utf8VaryingString wSearchStr=ui->searchLEd->text().toUtf8().data();

  /* must be hexa digit*/
  SearchContent.allocateBZero(wSearchStr.strlen() / 2);

  utf8_t* wChar = wSearchStr.Data;
  uint8_t wHexaValue=0;
  long wOffset=0;
  while (*wChar) {
    if (getHexaDigits(wChar,wHexaValue)) {
      ZExceptionDLg::adhocMessage("Invalid hexadecimal string",Severity_Error,&wSearchStr,"Invalid hexadecimal string. Cannot convert.");
      return false;
    }
    SearchContent[wOffset++]=wHexaValue;
  }// while

  if (pReverse) {
    if (SearchOffset==0) {
      ZExceptionDLg::adhocMessage("Search",Severity_Error,&wSearchStr,"Already at beginning.");
      return false;
    }
    SearchOffset -=   SearchContent.Size ;
  }

  if ((FormerSearchOffset >= 0 )&&(FormerSearchSize > 0))
    setSelectionBackGround(DefaultBackGround,FormerSearchOffset,FormerSearchSize,false);
  if (SearchOffset < 0)
    SearchOffset=0;

  if (pReverse)
    SearchOffset=RawData.breverseSearch(SearchContent,SearchOffset);
    else
    SearchOffset=RawData.bsearch(SearchContent,SearchOffset);

  FormerSearchOffset=SearchOffset;
  FormerSearchSize=SearchContent.Size;

  if (SearchOffset < 0) {
    ZExceptionDLg::adhocMessage("Search",Severity_Error,&wSearchStr,"Value not found.");
    return false;
  }

  setSelectionBackGround(SelectedBackGround,SearchOffset,SearchContent.Size,true);

  SearchOffset += SearchContent.Size;

  return true;
//  model.setData(model.index(0, 0), QVariant(QBrush(Qt::red)), Qt::BackgroundRole);
}//ZContentVisuMain::searchHexa

bool ZContentVisuMain::searchAscii(bool pCaseRegardless, bool pReverse) {
  utf8VaryingString wSearchStr=ui->searchLEd->text().toUtf8().data();
  if (pReverse) {
    if (SearchOffset==0) {
    ZExceptionDLg::adhocMessage("Search",Severity_Error,&wSearchStr,"Already at beginning.");
    return false;
    }
    SearchOffset -=   wSearchStr.strlen();
  }

  /* must be hexa digit*/
  size_t wLength=wSearchStr.strlen();
  SearchContent.allocateBZero( wLength+ 1);

  if ((FormerSearchOffset >= 0 )&&(FormerSearchSize > 0))
    setSelectionBackGround(DefaultBackGround,FormerSearchOffset,FormerSearchSize);
  if (SearchOffset < 0)
    SearchOffset=0;

  if (pCaseRegardless) {
    if (pReverse)
      SearchOffset=RawData.breverseSearchCaseRegardless(wSearchStr.Data,wLength,SearchOffset);
    else
      SearchOffset=RawData.bsearchCaseRegardless(wSearchStr.Data,wLength,SearchOffset);
  }
  else {
    if (pReverse)
      SearchOffset=RawData.breverseSearch(wSearchStr.Data,wLength,SearchOffset);
    else
      SearchOffset=RawData.bsearch(wSearchStr.Data,wLength,SearchOffset);
  }

  FormerSearchOffset=SearchOffset;
  FormerSearchSize=SearchContent.Size;

  if (SearchOffset < 0) {
    ZExceptionDLg::adhocMessage("Search",Severity_Error,&wSearchStr,"Value not found.");
    return false;
  }

  setSelectionBackGround(SelectedBackGround,SearchOffset,SearchContent.Size);

  SearchOffset += SearchContent.Size;

  return true;
}//ZContentVisuMain::searchAscii


void ZContentVisuMain::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (FResizeInitial)
  {
    FResizeInitial=false;
    return;
  }
  QRect wR1 = ui->verticalLayoutWidget->geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  ui->verticalLayoutWidget->resize(wVW,wVH);  /* expands in width and height */

}//ZContentVisuMain::resizeEvent

void
ZContentVisuMain::actionMenuEvent(QAction* pAction)
{
  utf8VaryingString wStr;
  ZStatus wSt=ZS_SUCCESS;



  if (pAction==ui->openByTypeQAc)
  {
    actionOpenFileByType();

    return;
  }
  if (pAction==ui->openRawQAc)
  {
    openRaw();
    ui->OpenModeLbl->setText("Raw file");
    return;
  }
  if (pAction==GetRawQAc)
  {
    getRaw();
    ui->OpenModeLbl->setText("Raw file");
    return;
  }
  if (pAction==ui->openZRFQAc)
  {
    wSt=openZRF(URICurrent.toCChar());
    if ((wSt==ZS_SUCCESS)||(wSt==ZS_FILETYPEWARN))
    {

      removeAllRows();
//      displayListZRFFirstRecord(Width);
      return;
    }
    ui->OpenModeLbl->setText("Random file");
    return;
  }
  if (pAction==ui->openZMFQAc)
  {
    OpenMode = VMD_Master;
    removeAllRows();
    wSt=openZMF(URICurrent.toCChar());
    ui->OpenModeLbl->setText("Master file");
    return;
  }
  if (pAction==ui->openZRHQAc)
  {
    OpenMode = VMD_Header;
    removeAllRows();
    wSt=openZRH(URICurrent.toCChar());
    ui->OpenModeLbl->setText("Header file");
    return;
  }
  if (pAction==ui->unlockZRFQAc)
  {
    unlockZRFZMF(URICurrent.toCChar());
    return;
  }

  if (pAction==ui->setfileQAc)
  {
    chooseFile(true);
    return;
  }

  if (pAction==ui->ZRFRecordsQac)
  {
    ui->OpenModeLbl->setText("By record");
    ui->BackwardBTn->setVisible(true);
    ui->ForwardBTn->setVisible(true);
    return;
  }
  if (pAction==ui->unlockZRFQAc)
  {
    ZRFUnlock();
    return;
  }
  if (pAction==ui->HeaderRawUnlockQAc)
  {
    ZHeaderRawUnlock();
    return;
  }
  if (pAction==ui->closeQAc)
  {
    OpenMode = VMD_Nothing;
    ui->BackwardBTn->setVisible(false);
    ui->ForwardBTn->setVisible(false);
    actionClose();
    return;
  }
  if (pAction==ui->displayHCBQAc)
  {
    displayHCB();
    return;
  }
  if (pAction==ui->displayFCBQAc)
  {
    displayFCB();
    return;
  }
  if (pAction==ui->displayMCBQAc)
  {
    displayMCB();
    return;
  }
  if (pAction==ui->displayZBATQAc)
  {
    displayZBAT();
    return;
  }
  if (pAction==ui->displayZDBTQAc)
  {
    displayZDBT();
    return;
  }
  if (pAction==ui->displayZFBTQAc)
  {
    displayZFBT();
    return;
  }

  if (pAction==DictionaryQAc)
  {
    if (!dictionaryWnd)
      dictionaryWnd=new DicEdit(std::bind(&ZContentVisuMain::DicEditQuitCallback, this),this);
    dictionaryWnd->clear();

//    dictionaryWnd->setNewDictionary();

    dictionaryWnd->show();
    return;
  }
/*
  if (pAction==ui->dicLoadXmlQAc)
  {
    QFileDialog wFd;
    wFd.setWindowTitle(QObject::tr("Xml file","DicEdit"));
    wFd.setLabelText(QFileDialog::Accept,  QObject::tr("Select","ZContentVisuMain"));
    wFd.setLabelText(QFileDialog::Reject ,  QObject::tr("Cancel","ZContentVisuMain"));
    wFd.setFileMode(QFileDialog::ExistingFile);
    while (true)
      {
      int wRet=wFd.exec();
      if (wRet==QDialog::Rejected)
        return;

      if (wFd.selectedFiles().isEmpty())
        ZExceptionDLg::message("dicLoadXmlQAc",ZS_EMPTY,Severity_Error,QObject::tr("No file selected.Please select a valid file.","ZContentVisuMain").toUtf8().data());
      else
        break;
      }//while (true)

    uriString wXmlFile= wFd.selectedFiles()[0].toUtf8().data();

    if (!dictionaryWnd)
      dictionaryWnd=new DicEdit(this);

    wSt=dictionaryWnd->loadXmlDictionary(wXmlFile);
    if (wSt==ZS_SUCCESS)
      {
      wStr.sprintf("Xml file %s has been successfully loaded.",wXmlFile.toCChar());
      statusBar()->showMessage(QObject::tr(wStr.toCChar(),"DicEdit"),cst_MessageDuration);
      }
    dictionaryWnd->show();
    return;
  }
*/
  if (pAction==ui->surfaceScanZRFQAc) {
    surfaceScanZRF(URICurrent);
    return;
  }
  if (pAction==ui->surfaceScanRawQAc)
  {
    displayRawSurfaceScan(URICurrent);
    return;
  }

  if (pAction==ui->QuitQAc)
  {
    actionClose();
    //    this->deleteLater();
    QApplication::quit();
    return;
  }


  /* ZMFMEn */

  if (pAction==ui->runRepairQAc)
  {
    repairIndexes(ui->testRunQAc->isChecked(), ui->rebuilAllQAc->isChecked());
    return;
  }
  if (pAction==ui->MCBReportQAc)
  {
    reportMCB();
    return;
  }

  return;

}//actionMenuEvent


void ZContentVisuMain::reportMCB()
{
  FILE* wReportLog = fopen("reportMCB.log","w");
  MasterFile->report(wReportLog);
  fflush(wReportLog);
  fclose(wReportLog);

  if (MCBWin==nullptr)
    MCBWin=new textEditMWn(this,false,nullptr);
//    MCBWin=new textEditMWn(this,nullptr);
  else
    MCBWin->clear();

  MCBWin->registerCloseCallback(std::bind(&ZContentVisuMain::closeMCBCB, this,std::placeholders::_1));
  MCBWin->registerMoreCallback(std::bind(&ZContentVisuMain::textEditMorePressed, this));

  MCBWin->setTextFromFile("reportMCB.log");
  MCBWin->show();
}


void ZContentVisuMain::repairIndexes(bool pTestRun,bool pRebuildAll)
{
  ZStatus wSt=ZS_SUCCESS;

  FILE* wRepairLog = fopen("repairindex.log","w");
  wSt=MasterFile->zrepairIndexes( URICurrent.toCChar(),
                                  pTestRun,
                                  pRebuildAll,
                                  wRepairLog);
  fflush(wRepairLog);
  fclose(wRepairLog);

  openGenLogWin();
  GenlogWin->setTextFromFile("repairindex.log");
  GenlogWin->show();

  if (wSt==ZS_SUCCESS)
    return;

  ZExceptionDLg::displayLast();
  return;
}

void ZContentVisuMain::removeIndex()
{

}

void ZContentVisuMain::addIndex()
{

}

void ZContentVisuMain::rebuildIndex()
{

}

void
ZContentVisuMain::extractIndex()
{

}

void
ZContentVisuMain::dowdgrade()
{

}
void
ZContentVisuMain::reorganizeZMF()
{

}

void
ZContentVisuMain::ZRFUnlock()
{
  if (OpenMode != VMD_Random)
  {
    QMessageBox::critical(this,tr("Operation error"),"Cannot unlock : file must be open as random file.");
    return;
  }
  ZStatus wSt=ZRandomFile::zutilityUnlockZRF(URICurrent);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast(false);
    ZException.pop();
  }
}
void
ZContentVisuMain::ZHeaderRawUnlock()
{
  if (OpenMode != VMD_RawSequential)
  {
    QMessageBox::critical(this,tr("Operation error"),"Cannot unlock : header file must be open as raw file.");
    return;
  }
  ZStatus wSt=ZRandomFile::zutilityUnlockHeaderFile(URICurrent);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast(false);
    ZException.pop();
  }
}


void
ZContentVisuMain::actionOpenFileByType(bool pChecked)
{
  ZStatus wSt;

  if (!chooseFile(false))
    return;

  if (URICurrent.getFileExtension()=="zrf")
    {
    if (ZS_SUCCESS==openZRF(URICurrent.toCChar()))
      {
        removeAllRows();
        //          displayListZRFFirstRecord(Width);
        return;
      }
    }

  if (URICurrent.getFileExtension()=="zmf")
      {
        ui->FileTypeLBl->setText("Master file");
        wSt=openZMF(URICurrent.toCChar());
        if ((wSt==ZS_SUCCESS)||(wSt==ZS_FILETYPEWARN))
        {
          removeAllRows();
          //          displayListZRFFirstRecord(Width);
          return;
        }
      }
  if (URICurrent.getFileExtension()=="zrh")
      {
        removeAllRows();
        ui->FileTypeLBl->setText("Header file");
        openZRH();
        loadAll();
        return;
      }

  if (ZS_SUCCESS==openOther(URICurrent.toCChar()))
    {
      removeAllRows();
      ui->LoadAllBTn->setVisible(true);
      return;
    }

    ZExceptionDLg::displayLast(false);
    ZException.pop();

  return;
} // actionOpenFileByType

#ifdef __COMMENT__
void
ZContentVisuMain::actionOpenFileByType(bool pChecked)
{
  ZStatus wSt;
  QList<QString> wNameFilters;
  wNameFilters << "*.zrf";
  wNameFilters << "*.zmf";


  QFileDialog *FileSelectionDLg = new QFileDialog(this);

  FileSelectionDLg->setNameFilters(wNameFilters);
  FileSelectionDLg->setFileMode(QFileDialog::ExistingFile);
  FileSelectionDLg->setAcceptMode(QFileDialog::AcceptOpen);

  FileSelectionDLg->setDirectory("/home/gerard/Development/zmftest");


  FileSelectionDLg->setLabelText(QFileDialog::Accept,  "Select");
  FileSelectionDLg->setLabelText(QFileDialog::Reject ,  "Cancel");



  while (true)
  {
    if (FileSelectionDLg->exec()==QDialog::Rejected)
    {
      FileSelectionDLg->deleteLater();
      return;
    }
    if (FileSelectionDLg->selectedFiles().isEmpty())
    {
      QMessageBox::critical(this,tr("No file selected"),"Please select a valid file");
    }
    else
    {

      URICurrent = FileSelectionDLg->selectedFiles()[0].toUtf8().data();
      if (URICurrent.getFileExtension()=="zrf")
      {
        if (ZS_SUCCESS==openZRF(URICurrent.toCChar()))
        {
          removeAllRows();
//          displayListZRFFirstRecord(Width);
          break;
        }

      }
      else
          if (URICurrent.getFileExtension()=="zmf")
      {
        ui->FileTypeLBl->setText("Master file");
        wSt=openZMF(URICurrent.toCChar());
        if ((wSt==ZS_SUCCESS)||(wSt==ZS_FILETYPEWARN))
        {
          removeAllRows();
//          displayListZRFFirstRecord(Width);
          break;
        }

      }
      else
      {
        if (ZS_SUCCESS==openOther(URICurrent.toCChar()))
        {
          removeAllRows();
          ui->LoadAllBTn->setVisible(true);
//          displayFdNextRawBlock(BlockSize,Width);
          break;
        }
//        QMessageBox::critical(this,tr("Random File open error"),ZException.formatFullUserMessage().toCChar());
        ZExceptionDLg::display(*ZException.popR());
      }
    }//else
    }//while (true)

  setFileType (FileSelectionDLg->selectedFiles()[0].toUtf8().data());
  ui->FullPathLbl->setText(FileSelectionDLg->selectedFiles()[0]);

  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(true);

  ui->displayHCBQAc->setEnabled(false);


  return;
} // actionOpenFileByType
#endif //  __COMMENT__

bool
ZContentVisuMain::chooseFile(bool pChecked)
{
  QList<QString> wNameFilters;
  wNameFilters << "*.*";

  QFileDialog *wFileSelectionDLg = new QFileDialog(this);

  wFileSelectionDLg->setNameFilters(wNameFilters);
  wFileSelectionDLg->setFileMode(QFileDialog::ExistingFile);
  wFileSelectionDLg->setAcceptMode(QFileDialog::AcceptOpen);

  wFileSelectionDLg->setDirectory("/home/gerard/Development/zmftest");

  wFileSelectionDLg->setLabelText(QFileDialog::Accept,  "Select");
  wFileSelectionDLg->setLabelText(QFileDialog::Reject ,  "Cancel");

  while (true)
  {
    if (wFileSelectionDLg->exec()==QDialog::Rejected)
    {
      wFileSelectionDLg->deleteLater();
      return false;
    }
    if (wFileSelectionDLg->selectedFiles().isEmpty())
    {
      QMessageBox::critical(this,tr("No file selected"),"Please select a valid file");
    }
    else
      break;
  }//while (true)

  URICurrent = wFileSelectionDLg->selectedFiles()[0].toUtf8().data();
  ui->FullPathLbl->setText(wFileSelectionDLg->selectedFiles()[0]);

  setFileType (wFileSelectionDLg->selectedFiles()[0].toUtf8().data());

  wFileSelectionDLg->deleteLater();

  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(true);

  ui->RecordFRm->setVisible(false);
  ui->SequentialFRm->setVisible(true);

  return true;
}//chooseFile

void
ZContentVisuMain::openZRH()
{

  if (openZRH(URICurrent.toCChar())!= ZS_SUCCESS)
    {
    QMessageBox::critical(this,tr("header file open error"),ZException.formatFullUserMessage().toCChar());
    return;
    }

  removeAllRows();
//  displayFdNextRawBlock(BlockSize,16);

  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(true);

  ui->RecordFRm->setVisible(false);
  ui->SequentialFRm->setVisible(true);


  ui->rawMEn->setEnabled(true);

  ui->openByTypeQAc->setVisible(false);
  ui->openRawQAc->setVisible(false);
  ui->openZRFQAc->setVisible(false);
  ui->openZMFQAc->setVisible(false);
  ui->unlockZRFQAc->setVisible(false);

  return;
}//actionOpenFileAsZRH

void
ZContentVisuMain::openRaw()
{

  if (openOther(URICurrent.toCChar())!= ZS_SUCCESS)
  {
    QMessageBox::critical(this,tr("Random File open error"),ZException.formatFullUserMessage().toCChar());
    return;
  }

  removeAllRows();
  //  displayFdNextRawBlock(BlockSize,16);

  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(true);

  ui->RecordFRm->setVisible(false);
  ui->SequentialFRm->setVisible(true);

  ui->closeQAc->setVisible(true);

  ui->rawMEn->setEnabled(true);

  ui->openByTypeQAc->setVisible(false);
  ui->openRawQAc->setVisible(false);
  ui->openZRFQAc->setVisible(false);
  ui->openZMFQAc->setVisible(false);
  ui->unlockZRFQAc->setVisible(false);

  return;
}//actionOpenFileAsRaw

ZStatus
ZContentVisuMain::openZRF(const char*pFilePath)
{
utf8String wStr;
ZStatus wSt=ZS_SUCCESS;

  if (RandomFile)
  {
    if (RandomFile->isOpen())
      RandomFile->zclose();
  }
  else
    RandomFile=new ZRandomFile;

  wSt = RandomFile->zopen(pFilePath,ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS)
    {
    if (wSt==ZS_FILETYPEWARN)
      ZExceptionDLg::displayLast(true);
    else
      {
      delete RandomFile;
      RandomFile=nullptr;
      ZExceptionDLg::displayLast(false);
      return wSt;
      }
    }

  RawData.clear();
  getFileSize(pFilePath);

  if (Fd>=0)
  {
    ::close(Fd);
    Fd=-1;
  }

  if (MasterFile)
  {
    if (MasterFile->isOpen())
      MasterFile->zclose();
    delete MasterFile;
    MasterFile=nullptr;
  }
  FileOffset=0;
  RecordNumber=0;

  ui->SequentialFRm->setVisible(false);
  ui->RecordFRm->setVisible(true);

  wStr.sprintf("%ld",RandomFile->getFileDescriptor().ZBAT.count());
  ui->RecordTotalLBl->setText(wStr.toCChar());

  ui->LoadAllBTn->setVisible(false);
  ui->BackwardBTn->setVisible(false);
  if (RandomFile->getFileDescriptor().ZBAT.count() > 1)
    ui->BackwardBTn->setVisible(true);
  else
    ui->BackwardBTn->setVisible(false);

  ui->OpenModeLbl->setText("Random file mode");
  OpenMode = VMD_Random;

  ui->openByTypeQAc->setVisible(false);
  ui->openRawQAc->setVisible(false);
  ui->openZRFQAc->setVisible(false);
  ui->openZMFQAc->setVisible(false);

   ui->FileTypeLBl->setText("Random file");

   ui->rawMEn->setEnabled(false);
   ui->ZRFMEn->setEnabled(true);
   ui->ZMFMEn->setEnabled(false);

  return ZS_SUCCESS;
//  return  displayListZRFFirstRecord(Width);
}//openZRF

ZStatus
ZContentVisuMain::openZMF(const char* pFilePath)
{
utf8String wStr;
ZStatus wSt=ZS_SUCCESS;

  if (MasterFile)
    {
    if (MasterFile->isOpen())
      MasterFile->zclose();
    }
  else
    MasterFile=new ZMasterFile;

  wSt= MasterFile->zopen(pFilePath,ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS)
  {
    if (wSt==ZS_FILETYPEWARN)
      ZExceptionDLg::displayLast(true);
    else
    {
      delete MasterFile;
      MasterFile=nullptr;
      ZExceptionDLg::displayLast(false);
      return wSt;
    }
  }

  RawData.clear();
  getFileSize(pFilePath);

  if (Fd>=0)
    {
    ::close(Fd);
    Fd=-1;
    }

  if (RandomFile)
  {
    if (RandomFile->isOpen())
      RandomFile->zclose();
    delete RandomFile;
    RandomFile=nullptr;
  }
  FileOffset=0;
  RecordNumber=0;

  wStr.sprintf("%ld",MasterFile->getFileDescriptor().ZBAT.count());
  ui->RecordTotalLBl->setText(wStr.toCChar());

  ui->LoadAllBTn->setVisible(false);
  ui->BackwardBTn->setVisible(false);
  if (MasterFile->getFileDescriptor().ZBAT.count() > 1)
    ui->BackwardBTn->setVisible(true);
  else
    ui->BackwardBTn->setVisible(false);

   ui->OpenModeLbl->setText("Master file mode");
   OpenMode = VMD_Master;

   ui->openByTypeQAc->setVisible(false);
   ui->openRawQAc->setVisible(false);
   ui->openZRFQAc->setVisible(false);
   ui->openZMFQAc->setVisible(false);

   ui->rawMEn->setEnabled(false);
   ui->ZRFMEn->setEnabled(false);
   ui->ZMFMEn->setEnabled(true);

  return ZS_SUCCESS;
} // openZMF


void
ZContentVisuMain::getRaw()
{
  if (!chooseFile(true))
    return ;
  if (openOther(URICurrent.toCChar())!= ZS_SUCCESS) {
    utf8VaryingString wAdd = ZException.last().formatFullUserMessage().toString();
    ZExceptionDLg::adhocMessage(tr("Random File open error").toUtf8().data(),ZException.last().Severity,&wAdd,
        QCoreApplication::translate("ZContentVisuMain","Cannot load file",nullptr).toUtf8().data());
//    QMessageBox::critical(this,tr("Random File open error"),ZException.formatFullUserMessage().toCChar());
    return;
  }
  openRaw();

  return;
}//getRaw


ZStatus
ZContentVisuMain::unlockZRFZMF(const char* pFilePath)
{

  ZStatus wSt=  ZRandomFile::zutilityUnlockZRF(pFilePath);

  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast(false);
    ZException.pop();
  }
  else
    ZExceptionDLg::message("ZContentVisuMain::unlockZRFZMF",ZS_SUCCESS,Severity_Information,"File <%s> has been successfully unlocked.",pFilePath);
  return wSt;
}//unlockZRFZMF




utf8String
formatSize(long long wSize)
{
  utf8String wSizeStr;
  double wSizeD = (double)wSize;

  const char* wUnit= "B";
  if (wSizeD > 1024.0)
  {
    wSizeD= wSizeD / 1024.0;
    if (wSizeD > 1024.0)
    {
      wSizeD= wSizeD / 1024.0;
      if (wSizeD > 1024.0)
        {
        wSizeD= wSizeD / 1024.0;
        wUnit="gB";
        }
      else
        wUnit="mB";
    }
    else
      wUnit="kB";
  }
  wSizeStr.sprintf("%g %s",wSizeD,wUnit);
  return wSizeStr;
}

void
ZContentVisuMain::getFileSize(const char* pFilePath)
{
  uriString wFile = pFilePath;  
  ui->FileSizeLBl->setText(formatSize(wFile.getFileSize()).toCChar());
  ui->FullSizeLBl->setText(formatSize(wFile.getFileSize()).toCChar());
}


ZStatus
ZContentVisuMain::openOther(const char* pFileName)
{

  ZStatus wSt=ZS_SUCCESS;
  uriString wFilePath = pFileName;

  Fd = open(wFilePath.toCChar(),O_RDONLY);       // open content file for read only

  if (Fd < 0)
    {
    ZException.getErrno(errno,
              _GET_FUNCTION_NAME_,
              ZS_ERROPEN,
              Severity_Severe,
              " Error opening file %s ",
              wFilePath.toCChar());
    return  ZS_ERROPEN;
    }

  RawData.clear();
  ui->LoadAllBTn->setVisible(true);
  getFileSize(pFileName);

  if (MasterFile)
    {
    if (MasterFile->isOpen())
      MasterFile->zclose();
    delete MasterFile;
    MasterFile=nullptr;
    }

  if (RandomFile)
    {
      if (RandomFile->isOpen())
        RandomFile->zclose();
      delete RandomFile;
      RandomFile=nullptr;
    }

  AllFileLoaded=false;
  FileOffset=0;
  RecordNumber=0;

  ui->SequentialFRm->setVisible(true);
  ui->RecordFRm->setVisible(false);

  ui->LoadAllBTn->setVisible(true);

  ui->displayHCBQAc->setEnabled(true);
  ui->displayMCBQAc->setEnabled(true);
  ui->displayFCBQAc->setEnabled(true);

  ui->headerMEn->setEnabled(true);

  ui->OpenModeLbl->setText("Raw mode");
  ui->OpenModeLbl->setVisible(true);
  ui->ClosedLBl->setVisible(false);

  OpenMode = VMD_RawSequential;

  ui->rawMEn->setEnabled(true);
  ui->ZRFMEn->setEnabled(false);
  ui->ZMFMEn->setEnabled(false);

  return ZS_SUCCESS;
}//openOther

ZStatus
ZContentVisuMain::openZRH(const char* pFileName)
{
  ZStatus wSt=ZS_SUCCESS;
  uriString wFilePath = pFileName;

  Fd = open(wFilePath.toCChar(),O_RDONLY);       // open content file for read only

  if (Fd < 0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        " Error opening file %s ",
        wFilePath.toCChar());
    return  ZS_ERROPEN;
  }

  RawData.clear();
  ui->LoadAllBTn->setVisible(true);
  getFileSize(pFileName);

  if (MasterFile)
  {
    if (MasterFile->isOpen())
      MasterFile->zclose();
    delete MasterFile;
    MasterFile=nullptr;
  }

  if (RandomFile)
  {
    if (RandomFile->isOpen())
      RandomFile->zclose();
    delete RandomFile;
    RandomFile=nullptr;
  }

  loadAll();

  FileOffset=0;
  RecordNumber=0;

  ui->SequentialFRm->setVisible(true);
  ui->RecordFRm->setVisible(false);

  ui->LoadAllBTn->setVisible(true);

  ui->displayHCBQAc->setEnabled(true);
  ui->displayMCBQAc->setEnabled(true);
  ui->displayFCBQAc->setEnabled(true);

  ui->headerMEn->setEnabled(true);

  ui->OpenModeLbl->setText("Header");
  ui->OpenModeLbl->setVisible(true);
  ui->ClosedLBl->setVisible(false);
  ui->closeQAc->setVisible(true);

  OpenMode = VMD_Header;

  ui->rawMEn->setEnabled(true);
  ui->ZRFMEn->setEnabled(false);
  ui->ZMFMEn->setEnabled(false);

  return ZS_SUCCESS;
}//openZRH

void
ZContentVisuMain::displayHCB()
{
  if (RawData.Size < sizeof(ZHeaderControlBlock_Export))
  {
    ZExceptionBase wE= ZExceptionBase::create("ZContentVisuMain::displayHCB",ZS_INVSIZE,Severity_Error,
        "Not enough data loaded. Requested minimum size is <%ld> bytes.\n"
        "Only <%ld> Bytes have been loaded.\n\n"
        "Load / reload file content <Reload>\n"
        "Quit <Quit>",sizeof(ZHeaderControlBlock_Export),RawData.Size);

    int wRet=ZExceptionDLg::display2B("HCB Exception",wE,"Quit","Reload");
    if (wRet==ZEDLG_Rejected)
      return;
    //      case ZEDLG_Accepted:
    if (URICurrent.getFileSize()>100000000)
    {
      ZExceptionDLg::message("ZContentVisuMain::displayHCB",ZS_INVSIZE,Severity_Error,
          "Cannot load content of file <%s> size <%lld> exceeds capacity",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    if (URICurrent.getFileSize()<(sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+1))
    {
      ZExceptionDLg::message("ZContentVisuMain::displayHCB",ZS_INVSIZE,Severity_Error,
          "file <%s> has size <%lld> that does not allow to store requested header data.",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    URICurrent.loadContent(RawData);
  }//if (RawData.Size < sizeof(ZHeaderControlBlock_Export))


  if (!entityWnd)
    entityWnd=new DisplayMain(this);
  entityWnd->displayHCB(RawData);
}

void
ZContentVisuMain::displayFCB()
{
  if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export))
  {
    ZExceptionBase wE= ZExceptionBase::create("ZContentVisuMain::displayFCB",ZS_INVSIZE,Severity_Error,
        "Not enough data loaded. Requested minimum size is <%ld> bytes.\n"
        "Only <%ld> Bytes have been loaded.\n\n"
        "Load / reload file content <Reload>\n"
        "Quit <Quit>",sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export),RawData.Size);

    int wRet=ZExceptionDLg::display2B("FCB Exception",wE,"Quit","Reload");
    if (wRet==ZEDLG_Rejected)
      return;
    //      case ZEDLG_Accepted:
    if (URICurrent.getFileSize()>100000000)
    {
      ZExceptionDLg::message("ZContentVisuMain::displayHCB",ZS_INVSIZE,Severity_Error,
          "Cannot load content of file <%s>. Size <%lld> exceeds memory capacity",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    if (URICurrent.getFileSize()<(sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+1))
    {
      ZExceptionDLg::message("ZContentVisuMain::displayHCB",ZS_INVSIZE,Severity_Error,
          "File <%s> has size <%lld> that does not allow to store requested header data.",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    URICurrent.loadContent(RawData);
  }//if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export))
  if (!entityWnd)
    entityWnd=new DisplayMain(this);
  entityWnd->displayFCB(RawData);
}
void
ZContentVisuMain::displayMCB()
{
  if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))
  {
    ZExceptionBase wE= ZExceptionBase::create("ZContentVisuMain::displayMCB",ZS_INVSIZE,Severity_Error,
        "Not enough data loaded. Requested minimum size is <%ld> bytes.\n"
        "Only <%ld> Bytes have been loaded.\n\n"
        "Load / reload file content <Reload>\n"
        "Quit <Quit>",sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export),RawData.Size);

    int wRet=ZExceptionDLg::display2B("MCB Exception",wE,"Quit","Reload");
    if (wRet==ZEDLG_Rejected)
      return;
    //      case ZEDLG_Accepted:
    if (URICurrent.getFileSize()>100000000)
    {
      ZExceptionDLg::message("ZContentVisuMain::displayMCB",ZS_INVSIZE,Severity_Error,
          "Cannot load content of file <%s> size <%lld> exceeds memory capacity",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    if (URICurrent.getFileSize()<(sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export)+1))
    {
      ZExceptionDLg::message("ZContentVisuMain::displayMCB",ZS_INVSIZE,Severity_Error,
          "file <%s> has size <%lld> that does not allow to store requested header data.",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    URICurrent.loadContent(RawData);
  }//if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))
  if (!entityWnd)
      entityWnd=new DisplayMain(this);
  entityWnd->displayMCB(RawData);
}

void
ZContentVisuMain::displayZBAT()
{
  if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))
  {
    ZExceptionBase wE= ZExceptionBase::create("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
        "Not enough data loaded. Requested minimum size is <%ld> bytes.\n"
        "Only <%ld> Bytes have been loaded.\n\n"
        "Load / reload file content <Reload>\n"
        "Quit <Quit>",sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export),RawData.Size);

    int wRet=ZExceptionDLg::display2B("ZBAT Exception",wE,"Quit","Reload");
    if (wRet==ZEDLG_Rejected)
      return;
    //      case ZEDLG_Accepted:
    if (URICurrent.getFileSize()>100000000)
    {
      ZExceptionDLg::message("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
          "Cannot load content of file <%s> size <%lld> exceeds memory capacity",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    if (URICurrent.getFileSize()<(sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export)+1))
    {
      ZExceptionDLg::message("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
          "file <%s> has size <%lld> that does not allow to store requested header data.",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    URICurrent.loadContent(RawData);
  }//if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))


  unsigned char* wPtr=RawData.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)RawData.Data;
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

  displayPool(wPtr,wOffset,"ZBAT Block Access Table");
  return;
}

void
ZContentVisuMain::displayPool(unsigned char* pPtr,zaddress_type pOffset,const char* pTitle)
{
  ZAExport* wZAEe=(ZAExport*) pPtr;
  utf8VaryingString wOut;
  utf8VaryingString wStr;
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
      size_t(pPtr-RawData.Data),size_t(pPtr-RawData.Data),pOffset,pOffset,
      wZAEe->StartSign,wZAEe->StartSign,wZAEe->StartSign==cst_ZMSTART?"Correct":"Wrong",
      wZAEe->EndianCheck,reverseByteOrder_Conditional<uint16_t>(wZAEe->EndianCheck),wZAEe->isReversed()?"Serialized":"NOT Serialized",
      wZAEe->AllocatedSize,reverseByteOrder_Conditional (wZAEe->AllocatedSize),
      wZAEe->CurrentSize,reverseByteOrder_Conditional (wZAEe->CurrentSize),
      wZAEe->DataSize,reverseByteOrder_Conditional (wZAEe->DataSize),
      wZAEe->FullSize,reverseByteOrder_Conditional (wZAEe->FullSize),
      wZAEe->AllocatedElements,reverseByteOrder_Conditional (wZAEe->AllocatedElements),
      wZAEe->ExtentQuota,reverseByteOrder_Conditional (wZAEe->ExtentQuota),
      wZAEe->InitialAllocation,reverseByteOrder_Conditional (wZAEe->InitialAllocation),
      wZAEe->NbElements,reverseByteOrder_Conditional (wZAEe->NbElements));


//  textEditMWn* wTEx=openGenLogWin();
  textEditMWn* wTEx=GenlogWin = new textEditMWn(this);
  wTEx->setText(wOut,pTitle);

  int wNbElt = int(reverseByteOrder_Conditional<ssize_t> (wZAEe->NbElements));
  if (wNbElt==0)
  {
    wOut.sprintf("\n\n           No element in Pool.\n");
    wTEx->appendText(wOut);
    wTEx->show();
    return;
  }

  unsigned char* wPtr = pPtr+sizeof(ZAExport);
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
        "%5ld |%08X %10s|%4x %10s|%2x %12s|%2x %12s|%20lld 0x%10X|%16X %20lld|",
        wi,
        wBDe.StartSign ,wBDe.StartSign==cst_ZBLOCKSTART?"Correct":"Invalid",
        wBDe.EndianCheck ,wBDe.isReversed()?"Serialized":"Not Serial",
        wBDe.BlockId,decode_ZBID(wBDe.BlockId),  // BlockId is uint8_t and do not need to be deserialized
        wBDe.State , decode_ZBS(wBDe.State),                      // State is uint8_t and do not need to be deserialized
//        wBDe.Address,reverseByteOrder_Conditional<zaddress_type>(wBDe.Address),reverseByteOrder_Conditional<zaddress_type>(wBDe.Address),
        reverseByteOrder_Conditional<zaddress_type>(wBDe.Address),reverseByteOrder_Conditional<zaddress_type>(wBDe.Address),
        wBDe.BlockSize,reverseByteOrder_Conditional<zaddress_type>(wBDe.BlockSize)
        );
    wTEx->appendText(wOut);
    wi++;
  }

  wTEx->appendText("------+-------------------+---------------+---------------+---------------+---------------------------------+-------------------------------------+\n");
  wTEx->show();

}// displayPool


void
ZContentVisuMain::displayZDBT()
{
  if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))
  {
    ZExceptionBase wE= ZExceptionBase::create("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
        "Not enough data loaded. Requested minimum size is <%ld> bytes.\n"
        "Only <%ld> Bytes have been loaded.\n\n"
        "Load / reload file content <Reload>\n"
        "Quit <Quit>",sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export),RawData.Size);

    int wRet=ZExceptionDLg::display2B("ZDBT Exception",wE,"Quit","Reload");
    if (wRet==ZEDLG_Rejected)
      return;
    //      case ZEDLG_Accepted:
    if (URICurrent.getFileSize()>100000000)
    {
      ZExceptionDLg::message("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
          "Cannot load content of file <%s> size <%lld> exceeds memory capacity",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    if (URICurrent.getFileSize()<(sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export)+1))
    {
      ZExceptionDLg::message("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
          "file <%s> has size <%lld> that does not allow to store requested header data.",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    URICurrent.loadContent(RawData);
  }//if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))


  unsigned char* wPtr=RawData.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)RawData.Data;
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
  wOffset= reverseByteOrder_Conditional<size_t>(wFCBe->ZDBT_DataOffset);
  wPtr += wOffset;
  displayPool(wPtr,wOffset,"DBT Deleted blocks table");

}
void
ZContentVisuMain::displayZFBT()
{
  if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))
  {
    ZExceptionBase wE= ZExceptionBase::create("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
        "Not enough data loaded. Requested minimum size is <%ld> bytes.\n"
        "Only <%ld> Bytes have been loaded.\n\n"
        "Load / reload file content <Reload>\n"
        "Quit <Quit>",sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export),RawData.Size);

    int wRet=ZExceptionDLg::display2B("HCB_Export Exception",wE,"Quit","Reload");
    if (wRet==ZEDLG_Rejected)
      return;
    //      case ZEDLG_Accepted:
    if (URICurrent.getFileSize()>100000000)
    {
      ZExceptionDLg::message("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
          "Cannot load content of file <%s> size <%lld> exceeds memory capacity",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    if (URICurrent.getFileSize()<(sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export)+1))
    {
      ZExceptionDLg::message("ZContentVisuMain::displayZBAT",ZS_INVSIZE,Severity_Error,
          "file <%s> has size <%lld> that does not allow to store requested header data.",
          URICurrent.toCChar(),
          URICurrent.getFileSize());
      return;
    }
    URICurrent.loadContent(RawData);
  }//if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))


  unsigned char* wPtr=RawData.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)RawData.Data;
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
  displayPool(wPtr,wOffset,"FBT Free blocks table");
}//displayZFBT
/*
void
ZContentVisuMain::Dictionary()
{
  ZStatus wSt=URICurrent.loadContent(RawData);

  if (wSt!=ZS_SUCCESS)
    {
    ZExceptionDLg::displayLast(false);
    return;
    }

  if (!dictionaryWnd)
    dictionaryWnd=new DicEdit(this);

  dictionaryWnd->importDicFromFullHeader(RawData);
}
*/
void
ZContentVisuMain::setFileType (const char* pFilePath)
{

  utf8String wExt=URICurrent.getFileExtension();

  while (true)
  {
  if (wExt=="zrf")
    {
    ui->FileTypeLBl->setText("Random file");
    break;
    }
    if (wExt=="zmf")
    {
      ui->FileTypeLBl->setText("Master file");
      break;
    }
    if (wExt=="zix")
    {
      ui->FileTypeLBl->setText("Index file");
      break;
    }
    if (wExt=="zrh")
    {
      ui->FileTypeLBl->setText("Header file");
      ui->unlockZRFQAc->setVisible(true);
      return ;
    }
    if (wExt=="dic")
    {
      ui->FileTypeLBl->setText("Dictionary file");
      ui->unlockZRFQAc->setVisible(true);
      return ;
    }
    ui->FileTypeLBl->setText("Unmanaged file type");
    break;
  }//while true


  ui->openByTypeQAc->setVisible(true);
  ui->openRawQAc->setVisible(true);
  ui->openZRFQAc->setVisible(true);
  ui->openZMFQAc->setVisible(true);
  ui->unlockZRFQAc->setVisible(true);
  ui->closeQAc->setVisible(true);
/*
  ui->OpenModeLbl->setVisible(true);
  ui->ClosedLBl->setVisible(false);
*/
//  ui->actionMEn->setEnabled(true);
}//setFileType

void
ZContentVisuMain::actionClose(bool pChecked)
{
  displayWidgetBlockOnce=false;
  if (RandomFile)
    {
    RandomFile->zclose();
    delete RandomFile;
    RandomFile=nullptr;
    }

  if (MasterFile)
    {
      MasterFile->zclose();
      delete MasterFile;
      MasterFile=nullptr;
    }
  if (Fd >= 0)
      {
      ::close(Fd);
      Fd=-1;
      }
    ui->BackwardBTn->setVisible(false);
    ui->ForwardBTn->setVisible(false);

    ui->rawMEn->setEnabled(false);
    ui->ZRFMEn->setEnabled(false);
    ui->ZMFMEn->setEnabled(false);

    ui->openByTypeQAc->setVisible(true);
    ui->openRawQAc->setVisible(true);
    ui->openZRFQAc->setVisible(true);
    ui->openZMFQAc->setVisible(true);

    ui->OpenModeLbl->setText("not open");
    ui->OpenModeLbl->setVisible(false);
    ui->ClosedLBl->setVisible(true);

    OpenMode=VMD_Nothing;

  return;
}//actionClose



ZStatus
ZContentVisuMain::displayFdNextRawBlock(ssize_t pBlockSize,size_t pWidth)
{
  ZDataBuffer wRecord;
  wRecord.allocate(pBlockSize);
  int wS=read(Fd,wRecord.Data,pBlockSize);
  if (wS < 0)
    {
    ZException.getErrno(errno,"displayFdOneRawBlock",ZS_FILEERROR,Severity_Error,
        "Error reading file <%s>",URICurrent.toCChar());
    return ZS_FILEERROR;
    }
  if (wS==0)
    {
      return ZS_EOF;
      AllFileLoaded=true;
      ui->ForwardBTn->setVisible(false);
    }
  if (wS < pBlockSize)
    wRecord.truncate(wS);

  RawData.appendData(wRecord);

  ui->ReadSizeLBl->setText(formatSize(RawData.Size).toCChar());

  return displayWidgetBlock(wRecord);
}//displayFdNextRawBlock

ZStatus
ZContentVisuMain::displayWidgetBlock_old(ZDataBuffer& pData,int pWidth)
{
  ui->ProgressPGb->setRange(0,pData.Size);
  ui->ProgressPGb->setValue(0);
  ui->ProgressPGb->setVisible(true);

  /* how many lines within pData block */

  int wLines=pData.Size / pWidth;

  int wRem=pData.Size - ( wLines * pWidth);

  if (wRem > 0)
    wLines++;

  int wBaseLine=VisuTBv->ItemModel->rowCount();

  VisuTBv->ItemModel->setRowCount(VisuTBv->ItemModel->rowCount()+wLines);

  utf8String  wLineHexa,wLineAscii,wlineOffset;

  /* cut by chunk of pWidth bytes */
  long  wRemain=pData.Size;
  unsigned char* wPtr = pData.Data;
  unsigned char* wPtrEnd = pData.Data+pData.Size;

  int wTick = 0;
  long wProgress=0;
  int wCurLine=wBaseLine;
  while ((wPtr < wPtrEnd)&&(wRemain >= pWidth))
  {
//    wlineOffset.sprintf("%6X - %6ld",FileOffset,FileOffset);
    wlineOffset.sprintf("%6d-%6X",FileOffset,FileOffset);
    zmemDumpHexa(wPtr,pWidth,wLineHexa,wLineAscii);
    VisuTBv->ItemModel->setVerticalHeaderItem (wCurLine,new QStandardItem(wlineOffset.toCChar()));

    VisuTBv->ItemModel->setItem(wCurLine,0,new QStandardItem(wLineHexa.toCChar()));
    VisuTBv->ItemModel->setItem(wCurLine,1,new QStandardItem(wLineAscii.toCChar()));



    wPtr+= pWidth;
    FileOffset += pWidth;
    wRemain -= pWidth;
    wProgress += pWidth;
    wCurLine++;
    if (wTick++ > 10)
      {
      wTick=0;
      ui->ProgressPGb->setValue(wProgress);
      }
  }
  if (wRemain > 0)
    {
    zmemDumpHexa(wPtr,wRemain,wLineHexa,wLineAscii);
//    wlineOffset.sprintf("%6X - %6ld",FileOffset,FileOffset);
    wlineOffset.sprintf("%6d-%6X",FileOffset,FileOffset);
    VisuTBv->ItemModel->setVerticalHeaderItem(wCurLine,new QStandardItem(wlineOffset.toCChar()));
    VisuTBv->ItemModel->setItem(wCurLine,0,new QStandardItem(wLineHexa.toCChar()));
    VisuTBv->ItemModel->setItem(wCurLine,1,new QStandardItem(wLineAscii.toCChar()));
    }

//  VisuTBv->resizeRowsToContents();
//  VisuTBv->resizeColumnsToContents();

    for (int wi=0;wi < VisuTBv->ItemModel->rowCount();wi++)
      VisuTBv->resizeRowToContents(wi);
    for (int wi=0;wi < VisuTBv->ItemModel->columnCount();wi++)
      VisuTBv->resizeColumnToContents(wi);

  if (!displayWidgetBlockOnce)
    {
//    VisuTBv->resizeRowsToContents();
//    VisuTBv->resizeColumnsToContents();
    displayWidgetBlockOnce=true;
    }
  ui->ProgressPGb->setValue(pData.Size);
return ZS_SUCCESS;
}//displayBlock


void
ZContentVisuMain::displayOneLine(int pRow,unsigned char* &wPtr,unsigned char* wPtrEnd) {

  utf8String  wLineHexa,wLineAscii,wlineOffset;

  wlineOffset.sprintf("%6d-%6X",FileOffset,FileOffset);

  wLineAscii.allocateUnitsBZero(Width+1);

  int wCol=0;
  int wD=0;
  for (int wi=0;(wi < Width) && (wPtr < wPtrEnd);wi++) {
    wLineHexa.sprintf("%02X",*wPtr);
    VisuTBv->ItemModel->setItem(pRow,wCol,new QStandardItem(wLineHexa.toCChar()));
    if ((*wPtr>31)&&(*wPtr<127))
      wLineAscii.Data[wi]=*wPtr;
    else
      wLineAscii.Data[wi] = '.' ;

    wCol++;
    wD++;
    if (wD==4) {
      wD=0;
      wCol++;
    }
    wPtr++;
  }// for
  VisuTBv->ItemModel->setVerticalHeaderItem (pRow,new QStandardItem(wlineOffset.toCChar()));

  //    VisuTBv->setItem(wCurLine,0,new QTableWidgetItem(wLineHexa.toCChar()));
  VisuTBv->ItemModel->setItem(pRow,20,new QStandardItem(wLineAscii.toCChar()));

  if (DefaultBackGround.isValid())
    return;
  DefaultBackGround = VisuTBv->ItemModel->item(0,0)->data(Qt::BackgroundRole);

} // displayOneLine


ZStatus
ZContentVisuMain::displayWidgetBlock(ZDataBuffer& pData) {
//  VisuTBv->horizontalHeader()->hide();

  ui->ProgressPGb->setRange(0,pData.Size);
  ui->ProgressPGb->setValue(0);
  ui->ProgressPGb->setVisible(true);

  /* how many lines within pData block */

  int wLines=pData.Size / Width;

  int wRem=pData.Size - ( wLines * Width);

  if (wRem > 0)
    wLines++;

  int wBaseLine=VisuTBv->ItemModel->rowCount();

  VisuTBv->ItemModel->setRowCount(VisuTBv->ItemModel->rowCount()+wLines);

  utf8String  wLineHexa,wLineAscii,wlineOffset;

  /* cut by chunk of Width bytes */
  long  wRemain=pData.Size;
  unsigned char* wPtr = pData.Data;
  unsigned char* wPtrEnd = pData.Data+pData.Size;

  int wTick = 0;
  long wProgress=0;
  int wCurLine=wBaseLine;
  while ((wPtr < wPtrEnd)&&(wRemain >= Width)) {

    displayOneLine(wCurLine,wPtr,wPtrEnd);

    FileOffset += Width;
    wRemain -= Width;
    wProgress += Width;
    wCurLine++;
    if (wTick++ > 10) {
      wTick=0;
      ui->ProgressPGb->setValue(wProgress);
    }
  }//while ((wPtr < wPtrEnd)&&(wRemain >= Width))

  if (wRemain > 0) {
    displayOneLine(wCurLine,wPtr,wPtrEnd);
  }

  for (int wi=0;wi < VisuTBv->ItemModel->rowCount();wi++)
    VisuTBv->resizeRowToContents(wi);
  for (int wi=0;wi < VisuTBv->ItemModel->columnCount();wi++)
    VisuTBv->resizeColumnToContents(wi);

  VisuTBv->setColumnWidth(4,4);
  VisuTBv->setColumnWidth(9,4);
  VisuTBv->setColumnWidth(16,4);

  ui->ProgressPGb->setValue(pData.Size);
  return ZS_SUCCESS;
}//displayBlock

void
ZContentVisuMain::displayBlockData()
{
  utf8String wStr;
  wStr.sprintf("%6ld",RecordNumber+1L) ;
  ui->RecordNumberLBl->setText(wStr.toCChar());
  wStr.sprintf("0x%08X",Address) ;
  ui->AddressLBl->setText(wStr.toCChar());

  wStr.sprintf("%8ld",Block.BlockSize) ;
  ui->BlockSizeLBl->setText(wStr.toCChar());

  wStr.sprintf("%8ld",Block.DataSize()) ;
  ui->UserSizeLBl->setText(wStr.toCChar());

  ui->StateLBl->setText(decode_ZBS(Block.State));

  wStr.sprintf("%02X %s",Block.Lock,decode_ZLockMask(Block.Lock).toChar());
  ui->LockMaskLBl->setText(wStr.toCChar());

}

ZStatus
ZContentVisuMain::displayListZRFNextRecord(size_t pWidth)
{
  ZStatus wSt;
  if (RecordNumber==0)
    ui->BackwardBTn->setVisible(false);
  else
    ui->BackwardBTn->setVisible(true);

  // get first block of the file
  wSt=RandomFile->_getNext(Block,RecordNumber,Address);     // next logical block
  if (wSt==ZS_EOF)
  {
    ui->ForwardBTn->setVisible(false);
    return ZS_EOF;
  }
  ui->ForwardBTn->setVisible(true);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast(false);
//    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }


  displayBlockData();

  RawData=Block.Content;

  removeAllRows();

  displayWidgetBlock(RawData);

  ui->ForwardBTn->setVisible(true);
  return ZS_SUCCESS;
}//displayListZRFNextRecord

ZStatus
ZContentVisuMain::displayListZRFPreviousRecord(size_t pWidth)
{
  ZStatus wSt;

  RecordNumber--;
  if (RecordNumber==0)
    ui->BackwardBTn->setVisible(false);

  // get first block of the file
  wSt=RandomFile->_getByRank(Block,RecordNumber,Address);     // get first block of the file
  if (wSt==ZS_EOF)
  {
    ui->ForwardBTn->setVisible(false);
    return ZS_EOF;
  }

  if (wSt!=ZS_SUCCESS)
  {
    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }

  displayBlockData();

  if (RandomFile->getFileDescriptor().ZBAT.count() > 1)
    ui->ForwardBTn->setVisible(true);
  RawData=Block.Content;

  displayWidgetBlock(RawData);


  return ZS_SUCCESS;
}//displayListZRFPreviousRecord

ZStatus
ZContentVisuMain::displayListZRFFirstRecord(size_t pWidth)
{
  ZStatus wSt;

  ui->BackwardBTn->setVisible(false);

  FileOffset=0;
  RecordNumber = 0L;
  wSt=RandomFile->_getByRank(Block,RecordNumber,Address);     // get first block of the file
  if (wSt==ZS_EOF)
  {
    ui->ForwardBTn->setVisible(false);
    return ZS_EOF;
  }

  if (wSt!=ZS_SUCCESS) {
    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }

  displayBlockData();

  RawData=Block.Content;

  displayWidgetBlock(RawData);

  ui->ForwardBTn->setVisible(true);
  return ZS_SUCCESS;
}//displayListZRFFirstRecord

void
ZContentVisuMain::removeAllRows()
{
/*  if (TBlItemModel)
    if (TBlItemModel->rowCount()>0)
      TBlItemModel->removeRows(0,TBlItemModel->rowCount());

  if (ui->ListOffsetWDg->count() > 0)
    ui->ListOffsetWDg->clear();
  if (ui->ListHexaWDg->count() > 0)
    ui->ListHexaWDg->clear();
  if (ui->ListAsciiWDg->count() > 0)
    ui->ListAsciiWDg->clear();
*/
  if (VisuTBv->ItemModel->rowCount() > 0)
  {
    VisuTBv->ItemModel->removeRows(0,VisuTBv->ItemModel->rowCount());
  }
}

void
ZContentVisuMain::backward()
{
  ui->ForwardBTn->setEnabled(true);
  ui->ForwardBTn->setVisible(true);

  if (RandomFile)
  {
    displayListZRFPreviousRecord(Width);
    return;
  }

  FileOffset=lseek(Fd,(__off_t)-BlockSize,SEEK_CUR);
  if (FileOffset <= 0)
    {
    ui->BackwardBTn->setVisible(false);
    }
  displayFdNextRawBlock(BlockSize,Width);
}
void
ZContentVisuMain::forward()
{
  if (RandomFile)
  {
    displayListZRFNextRecord(Width);
    return;
  }

  ui->AddressLBl->setText("0");
  ui->RecordNumberLBl->setText("0");

//  ui->BackwardBTn->setVisible(true);
  ZStatus wSt=displayFdNextRawBlock(BlockSize,Width);
  if (wSt==ZS_EOF)
    ui->ForwardBTn->setVisible(false);
}

void
ZContentVisuMain::loadAll()
{
  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(false);
  ui->LoadAllBTn->setVisible(false);

  ui->SequentialFRm->setVisible(true);

  FileOffset=0;
  URICurrent.loadContent(RawData);
  removeAllRows();
  ZStatus wSt=displayWidgetBlock(RawData);

  ui->ReadSizeLBl->setText((formatSize(RawData.Size)).toCChar());
  AllFileLoaded=true;
  return;
}

void
ZContentVisuMain::surfaceScanZRF(const uriString& pFileToScan)
{
  FILE* wScan=fopen ("surfacescanzrf.txt","w");
  ZStatus wSt=RandomFile->zsurfaceScan(pFileToScan,wScan);
  fflush(wScan);
  fclose(wScan);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::display("Surface Scan",ZException.last(),false);
//    QMessageBox::critical(this,tr("Random file error"),ZException.formatFullUserMessage().toCChar());
    return;
  }

  openGenLogWin();

  GenlogWin->setTextFromFile("surfacescanzrf.txt");
  GenlogWin->show();

  return;
}



zbs::ZArray<ScanBlock> BlockList;

struct ScanBlockCount {
  int TotalBlocks   = 0;
  int UsedBlocks    = 0;
  int FreeBlocks    = 0;
  int DeletedBlocks = 0;
  int ErroredBlocks = 0;
  int Holes         = 0;
  int HoleVolume    = 0;
  int UsedVolume    = 0;
  int FreeVolume    = 0;
  int DeletedVolume = 0;
  int ErroredVolume = 0;
};


utf8String
displaytScanBlock(ScanBlock& pSB,ScanBlockCount& pSBCount) {
  int wH=0;
  utf8String wOut,wBlockSize,wHoleSize,wComment;
  pSBCount.TotalBlocks ++;
  wHoleSize=" ";

  switch (pSB.BlockHeader.State) {
  case ZBS_Used:
    pSBCount.UsedBlocks ++;
    pSBCount.UsedVolume += pSB.BlockHeader.BlockSize;
    wBlockSize.sprintf("%15ld",pSB.BlockHeader.BlockSize);
    wH = int (pSB.End - pSB.Begin - pSB.BlockHeader.BlockSize);
    if (wH > 0) {
      wHoleSize.sprintf("%ld",wH);
      pSBCount.Holes ++;
      pSBCount.HoleVolume += wH;
    }
    else if (wH < 0){
      wComment.sprintf("discrepancy physical block size %ld header block size %ld.",pSB.End - pSB.Begin,pSB.BlockHeader.BlockSize);
    }
    break;
  case ZBS_Free:
    pSBCount.FreeBlocks ++;
    pSBCount.FreeVolume += pSB.BlockHeader.BlockSize;
    wBlockSize.sprintf("%15ld",pSB.BlockHeader.BlockSize);
    wH = int (pSB.End - pSB.Begin - pSB.BlockHeader.BlockSize);
    if (wH > 0) {
      wHoleSize.sprintf("%ld",wH);
      pSBCount.Holes ++;
      pSBCount.HoleVolume += wH;
    }
    else if (wH < 0){
      wComment.sprintf("discrepancy physical block size %ld header block size %ld.",pSB.End - pSB.Begin,pSB.BlockHeader.BlockSize);
    }
    break;
  case ZBS_Deleted:
    pSBCount.DeletedBlocks ++;
    pSBCount.DeletedVolume += pSB.BlockHeader.BlockSize;
    wBlockSize.sprintf("%15ld",pSB.BlockHeader.BlockSize);
    wH = int (pSB.End - pSB.Begin - pSB.BlockHeader.BlockSize);
    if (wH > 0) {
      wHoleSize.sprintf("%ld",wH);
      pSBCount.Holes ++;
      pSBCount.HoleVolume += wH;
    }
    else if (wH < 0){
      wComment.sprintf("discrepancy physical block size %ld header block size %ld.",pSB.End - pSB.Begin,pSB.BlockHeader.BlockSize);
    }
    break;
  case ZBS_Invalid:
    wBlockSize = "missing header" ;
    wHoleSize.sprintf("%ld",pSB.End-pSB.Begin);
    pSBCount.ErroredBlocks ++;
    pSBCount.ErroredVolume += pSB.BlockHeader.BlockSize;
    pSBCount.HoleVolume += pSB.BlockHeader.BlockSize;
    pSBCount.Holes ++;

    wBlockSize.sprintf("%15ld",pSB.BlockHeader.BlockSize);
    wH = int (pSB.End - pSB.Begin - pSB.BlockHeader.BlockSize);
    if (wH > 0) {
      wHoleSize.sprintf("%ld",wH);
      pSBCount.Holes ++;
      pSBCount.HoleVolume += wH;
    }
    else if (wH < 0){
      wComment.sprintf("discrepancy physical block size %ld header block size %ld.",pSB.End - pSB.Begin,pSB.BlockHeader.BlockSize);
    }
    break;
  }// switch


  if (pSB.BlockHeader.State== ZBS_Invalid){
    wBlockSize = "missing header" ;
    wHoleSize.sprintf("%ld",pSB.End-pSB.Begin);
    pSBCount.Holes ++;
    pSBCount.HoleVolume += pSB.End-pSB.Begin;
    pSBCount.ErroredBlocks ++;
  }
  else {
    wBlockSize.sprintf("%15ld",pSB.BlockHeader.BlockSize);
    int wH = int (pSB.End - pSB.Begin - pSB.BlockHeader.BlockSize);
    if (wH > 0) {
      wHoleSize.sprintf("%ld",wH);
      pSBCount.Holes ++;
      pSBCount.HoleVolume += wH;
    }
    else if (wH < 0){
      wComment.sprintf("discrepancy physical block size %ld header block size %ld.",pSB.End - pSB.Begin,pSB.BlockHeader.BlockSize);
    }
//    pSBCount.ErroredBlocks ++;
  }
  wComment = " ";
  if (pSB.BlockHeader.State == ZBS_Invalid) {
    wComment = "Errored block";
  }

  wOut.sprintf("%15ld|%15ld|%15s|%15s|%15s|%s\n",
      pSB.Begin,
      pSB.End,
      decode_ZBS(pSB.BlockHeader.State),
      wBlockSize.toCChar(),
      wHoleSize.toCChar(),
      wComment.toCChar());
  return wOut;
}

/*
 *  <------------ _searchBlockStart--------------------->
 *
 *  <-----------_searchBlockEnd---------------->
 *
 *  block start                         block end        block start
 *  F9F9F9F9 ...........................FCFCFCFC.........F5F5F5F5,,,,,,>
 *  +--------------------+
 *   Block header export
 *
 *      Well formed block content                  HOLE
 *
 */
ZStatus
ZContentVisuMain::surfaceScanRaw(const uriString & pURIContent,FILE*pOutput) {
  ZStatus         wSt;
  ssize_t         wPayload=100000;
  int             wCount=0;

  ScanBlockCount  wSBCount;
  ZDataBuffer     wBlockBuffer;
//  const unsigned char wStartSign [5]={cst_ZFILESTART_BYTE,cst_ZFILESTART_BYTE,cst_ZFILESTART_BYTE,cst_ZFILESTART_BYTE , 0 };

//  ZBlockHeader_Export wBlockHeaderExp;
//  ZBlockHeader wBlockHeader;

//  unsigned char wEndSign [5]={0xFC,0xFC,0xFC,0xFC , 0 };

  int wContentFd = open(pURIContent.toCChar(),O_RDONLY);
  if (wContentFd < 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        "Error opening file <%s> ",
        pURIContent.toCChar());
    return  (ZS_ERROPEN);
  }

  off_t wFSoff = lseek(wContentFd,0,SEEK_END);  /* get file total size */
  if (wFSoff < 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEERROR,
        Severity_Severe,
        "Error lseek finding end of file <%s>.",
        pURIContent.toCChar());
    ::close(wContentFd);
    return ZS_FILEERROR;
  }

  fprintf(pOutput,
      "\n__________________________Content Data blocks in physical order______________________________________\n");

  size_t wFileSize = wFSoff;

  if (wFileSize < wPayload) {
    wPayload=wFileSize;
  }

  fprintf(pOutput,
      "Begin address  |End address    |    State      | Block size    |     Hole      | Comment\n"
      "_______________|_______________|_______________|_______________|_______________|____________________\n"
      //      "%15ld|%15ld|%15ld|%15s|%15ld|%15ld|%s"
      );

  off_t wOf = lseek(wContentFd,0,SEEK_SET);  /* set to beginning of file */
  if (wOf< 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEERROR,
        Severity_Severe,
        "Error lseek finding beginning of file <%s>.",
        pURIContent.toCChar());
    fprintf(pOutput,ZException.last().formatUtf8().toCChar());
    return ZS_FILEERROR;
  }
  uint32_t wStartMark=0;

  ScanBlock wSBlock;

  wSBlock.Begin=0;

  wSt=_searchBlockStart(wContentFd,wSBlock.Begin,wSBlock.End,wBlockBuffer,wPayload,wCount,wFileSize,&wStartMark);
  if (wSt!=ZS_FOUND) {
    if (wSt==ZS_EOF) {
      ZException.setMessage(_GET_FUNCTION_NAME_,
          ZS_INVTYPE,
          Severity_Error,
          "File <%s> has no start block. File appears to be invalid.",
          pURIContent.toString());
      fprintf(pOutput,ZException.last().formatUtf8().toCChar());
      return ZS_INVTYPE;
    }
    ZException.addToLast(" file <%s>",pURIContent.toString());
    fprintf(pOutput,ZException.last().formatUtf8().toCChar());
    return wSt;
  } // if (wSt!=ZS_FOUND)

  wSt=wSBlock.BlockHeader._importConvert(wSBlock.BlockHeader,(ZBlockHeader_Export*)wBlockBuffer.DataChar);
  if (wSt!=ZS_SUCCESS) {
    wSBlock.BlockHeader.clear();
    wSBlock.BlockHeader.State = ZBS_Invalid;
  }
  /* search end mark */

 // wSBlock.EndBlock=wBlockBuffer.bsearch(&wEndSign,sizeof(cst_ZBLOCKEND),sizeof(cst_ZBLOCKSTART));

  fprintf(pOutput,displaytScanBlock(wSBlock,wSBCount).toCChar());


  /* up to here, we have got the first start mark for first record of random file */
  wSBlock.Begin = wSBlock.End;
  wSt=_searchBlockStart(wContentFd,wSBlock.Begin,wSBlock.End,wBlockBuffer,wPayload,wCount,wFileSize,nullptr);

  while (wSt==ZS_FOUND) {
    wSt=wSBlock.BlockHeader._importConvert(wSBlock.BlockHeader,(ZBlockHeader_Export*)wBlockBuffer.DataChar);
    if (wSt!=ZS_SUCCESS) {
      wSBlock.BlockHeader.clear();
      wSBlock.BlockHeader.State = ZBS_Invalid;
    }
    /* search end mark */

//    wSBlock.EndBlock=wBlockBuffer.bsearch(&wEndSign,sizeof(cst_ZBLOCKEND),sizeof(cst_ZBLOCKSTART));

    fprintf(pOutput,displaytScanBlock(wSBlock,wSBCount).toCChar());

    wSBlock.Begin = wSBlock.End;
    wSt=_searchBlockStart(wContentFd,wSBlock.Begin,wSBlock.End,wBlockBuffer,wPayload,wCount,wFileSize,nullptr);
  }// while (wSt==ZS_FOUND)


  if (wSt==ZS_EOF){
    wSt=wSBlock.BlockHeader._importConvert(wSBlock.BlockHeader,(ZBlockHeader_Export*)wBlockBuffer.DataChar);
    if (wSt!=ZS_SUCCESS) {
      wSBlock.BlockHeader.clear();
      wSBlock.BlockHeader.State = ZBS_Invalid;
    }
    /* search end mark */

//    wSBlock.EndBlock=wBlockBuffer.bsearch(&wEndSign,sizeof(cst_ZBLOCKEND),sizeof(cst_ZBLOCKSTART));

    fprintf(pOutput,displaytScanBlock(wSBlock,wSBCount).toCChar());

  } // if (wSt==ZS_EOF)
  fprintf(pOutput,
      "\n____________________________________________________________________________________________________________________\n");

  fprintf(pOutput,"\nEnd of scan address %ld\n",wSBlock.End);
  fprintf(pOutput,
      "Total blocks.........%d\n"
      "Used  blocks.........%d\n"
      "Free  blocks.........%d\n"
      "Deleted blocks.......%d\n"
      "Errored blocks.......%d\n\n"
      "Used blocks size.....%d\n"
      "Free blocks size.....%d\n"
      "Deleted blocks size..%d\n"
      "Errored blocks size..%d\n\n"
      "Holes ...............%d\n"
      "Cumulated hole size..%d\n",

      wSBCount.TotalBlocks,
      wSBCount.UsedBlocks,
      wSBCount.FreeBlocks,
      wSBCount.DeletedBlocks,
      wSBCount.ErroredBlocks,

      wSBCount.UsedVolume,
      wSBCount.FreeVolume,
      wSBCount.DeletedVolume,
      wSBCount.ErroredVolume,

      wSBCount.Holes,
      wSBCount.HoleVolume);
  fprintf(pOutput,
      "____________________________________________________________________________________________________________________\n");

  ::close(wContentFd);
}


ssize_t LoadMax=100000;
void setLoadMax (ssize_t pLoadMax) {LoadMax=pLoadMax;}

ZStatus
_searchBlockStart  (int pContentFd,
                    zaddress_type pBeginAddress,      // Address to start searching for for next block
                    zaddress_type &pNextAddress,
                    ZDataBuffer &pBlockContent,
                    ssize_t &pPayload,
                    int     &pCount,
                    size_t &pFileSize,
                    uint32_t* pBeginContent) {

  ZDataBuffer     wBuffer;
  pBlockContent.clear();

  pNextAddress = pBeginAddress ;

  /* find StartSign */

  if (off_t (pBeginAddress + pPayload) > pFileSize ) {
    pPayload = pFileSize - pBeginAddress  ;
  }
 // wBuffer.allocate(pPayload);




  unsigned char wStartSign [5]={cst_ZFILESTART_BYTE,cst_ZFILESTART_BYTE,cst_ZFILESTART_BYTE,cst_ZFILESTART_BYTE , 0 };

  ssize_t wOffset= -1;

  wBuffer.allocate(pPayload);

  off_t wOf=lseek(pContentFd,pBeginAddress,SEEK_SET);
  if ( wOf< 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEERROR,
        Severity_Severe,
        "Error lseek finding beginning of file.");
    return ZS_FILEERROR;
  }

  ssize_t wSRead= read(pContentFd,wBuffer.Data,pPayload);
  if (wSRead < 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_READERROR,
        Severity_Severe,
        "Error while reading file.");
    return(ZS_READERROR);
  }

  /*if cst_ZBLOCKSTART if at first position, skip it */
  if (pPayload > sizeof(uint32_t)){
    uint32_t wStartMark=*((uint32_t*)wBuffer.Data);
    if (pBeginContent)
      *pBeginContent = wStartMark;
    if (wStartMark==cst_ZFILEBLOCKSTART)
      wOffset = sizeof(uint32_t) ;
  }

  /* search for cst_ZBLOCKSTART. If cst_ZBLOCKSTART if at first position, skip it */
    wOffset=wBuffer.bsearch(wStartSign,sizeof(cst_ZFILEBLOCKSTART),wOffset);

/* if not found in first payload, try one or more times until found or EOF */
  size_t wOff;
  while (wOffset < 0) {
    wOff = wBuffer.Size - sizeof(cst_ZFILEBLOCKSTART);
    pBlockContent.appendData(wBuffer.Data,wBuffer.Size - sizeof(cst_ZFILEBLOCKSTART));

    pNextAddress += wSRead - sizeof(cst_ZFILEBLOCKSTART) ;

    if (pBeginAddress >= pFileSize) {
      pNextAddress = pFileSize;
      return ZS_EOF;
    }
    wBuffer.allocate(pPayload);
    off_t wOf=lseek(pContentFd,pNextAddress,SEEK_SET);
    if ( wOf< 0) {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_FILEERROR,
          Severity_Severe,
          "Error lseek finding address %ld.",pNextAddress);
      return ZS_FILEERROR;
    }

    wSRead= read(pContentFd,wBuffer.Data,pPayload);
    if (wSRead<0) {
      ZException.getErrno(errno,
                          _GET_FUNCTION_NAME_,
                          ZS_READERROR,
                          Severity_Severe,
                          "Error while reading file.");
      return(ZS_READERROR);
    }
    if (wSRead==0) {  /* EOF encountered */
      pNextAddress = pFileSize;
      return(ZS_EOF);
    }
    if (wSRead < pPayload) { /* we have a partial read, so EOF has been encountered (hopefully-to be verified)*/
      wBuffer.truncate(wSRead);
      wOffset=wBuffer.bsearch(wStartSign,sizeof(cst_ZFILEBLOCKSTART),0L);
      if (wOffset < 0) {
          /* assume that wStartSign is truncated at the end of wBuffer (max will be 3 bytes) */
          pBlockContent.appendData(wBuffer.Data,wSRead);
          pNextAddress += wSRead ;
          return  (ZS_EOF);
      }
      pBlockContent.appendData(wBuffer.Data,wOffset);
      pNextAddress += wOffset;

      /* for stats purpose */
      double wT = double(pPayload * pCount);
      wT += double(pBlockContent.Size);
      pCount++;
      wT = wT / double(pCount);
      pPayload = ssize_t(wT);

/*        wSt=pBlockHeader._importConvert(pBlockHeader,(ZBlockHeader_Export*)&wBuffer.DataChar[wOffset]);
        if (wSt!=ZS_SUCCESS)
          { return (wSt); }
*/
      return  (ZS_FOUND) ;
    }// if (wSRead < pPayload)  partial read

    wOffset=wBuffer.bsearch(wStartSign,sizeof(cst_ZFILEBLOCKSTART),0L);
    /* if not found in current payload read another payload*/
#ifdef __COMMENT__
    if (wOffset < 0) {
      /* assume that wStartSign is truncated at the end of wBuffer (max will be 3 bytes) */
      pBeginAddress = pBeginAddress + pPayload - 3 ;
      pBlockContent.appendData(wBuffer.Data,pPayload-3);
      off_t wFSoff = lseek(pContentFd,-3,SEEK_CUR);  /* position 3 bytes before  */
      if (wFSoff < 0) {
        ZException.getErrno(errno,
            _GET_FUNCTION_NAME_,
            ZS_FILEERROR,
            Severity_Severe,
            "Error lseek finding end of file.");
//        pNextAddress=-1;
        return ZS_FILEERROR;
      }// if (wFSoff < 0)
    }// if (wOffset<0)
#endif // __COMMENT__

  }// while (wOffset<0)

  pNextAddress += wOffset;
  pBlockContent.appendData(wBuffer.Data,wOffset);

  double wT = double(pPayload * pCount);
  wT += double(pBlockContent.Size);
  pCount++;
  wT = wT / double(pCount);
  pPayload = ssize_t(wT);
 /*
  wSt=pBlockHeader._importConvert(pBlockHeader,(ZBlockHeader_Export*)&wBuffer.DataChar[wOffset]);
  if (wSt!=ZS_SUCCESS)
      { return (wSt); }
*/
  return  (ZS_FOUND) ;
}//_searchNextPhysicalBlock

void ZContentVisuMain::displayRawSurfaceScan(const uriString& pFileToScan)
{
  FILE* wScan=fopen ("surfacescanraw.txt","w");
  ZStatus wSt=surfaceScanRaw(pFileToScan,wScan);
  fflush(wScan);
  fclose(wScan);
  textEditMWn* wDisp=new textEditMWn(this,TEOP_ShowLineNumbers);

  wDisp->setTextFromFile("surfacescanraw.txt");
  wDisp->setWindowTitle(pFileToScan.getBasename().toCChar());
  wDisp->show();
  return ;
}

void ZContentVisuMain::closeMCBCB(const QEvent *pEvent)
{
  MCBWin=nullptr;
}
void
ZContentVisuMain::textEditMorePressed()
{

}
void ZContentVisuMain::closeGenlogCB(const QEvent* pEvent)
{
  GenlogWin=nullptr;
}
textEditMWn*  ZContentVisuMain::openGenLogWin()
{
  if (GenlogWin==nullptr) {
      GenlogWin = new textEditMWn(this);
      GenlogWin->registerCloseCallback(std::bind(&ZContentVisuMain::closeGenlogCB, this,std::placeholders::_1));
  }
  else
    GenlogWin->clear();
  return GenlogWin;
}

void ZContentVisuMain::DicEditQuitCallback(){
//  delete dictionaryWnd;
  dictionaryWnd = nullptr;
}
