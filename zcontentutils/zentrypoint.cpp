#include "zentrypoint.h"


#include <zio/zdir.h>
#include <stdio.h>
#include <fcntl.h>

#include <zcontentcommon/zgeneralparameters.h>

#include <zio/zioutils.h>

#include <QStandardItemModel>
#include <qaction.h>
#include <qactiongroup.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <QVBoxLayout>

#include <QLabel>
#include <QAction>
#include <QActionGroup>
#include <QPushButton>
#include <QMenu>
#include <QLineEdit>
#include <QComboBox>
#include <QStatusBar>
#include <QMenuBar>
#include <QFrame>
#include <QCoreApplication>
#include <QProgressBar>




#include <zcontent/zrandomfile/zrandomfile.h>
#include <zcontent/zindexedfile/zrawmasterfile.h>

#include "displaymain.h"
#include "zscan.h"


#include <zexceptiondlg.h>

#include <zcontent/zindexedfile/zmfdictionary.h>

//#include "zcontentvisumain.h"
#include "zrawvisumain.h"

#include <dicedit.h>
#include <qfiledialog.h>

#include <texteditmwn.h>
#include <zqt/zqtwidget/zqtableview.h>

#include <functional> // for std::function and placeholders

#include <QGroupBox>

#include "zrawmasterfilevisu.h"

#include <zcontent/zrandomfile/zrandomfile.h>
#include <zcontent/zrandomfile/zrfutilities.h>
#include <zcontent/zindexedfile/zrawmasterfileutils.h>

#include "visulinecol.h"

#include "poolvisu.h"
#include "filegeneratedlg.h"

#include "zrawkeylistdlg.h"

#include "visuraw.h"

#include "zbackupdlg.h"
#include "zrestoredlg.h"

#include "zsearchquerymwd.h"
#include "zmfprogressmwn.h"
#include "zcopydlg.h"

#include <zcontentcommon/zgeneralparameters.h>
#include "zgeneralparamsdlg.h"

#include <ztoolset/zexceptionmin.h>

#include <zclearfiledlg.h>

#include <QApplication>

#include <ztoolset/zaierrors.h>

//const int cst_maxraisonablevalue = 100000;

using namespace std;
using namespace zbs;





//ZEntryPoint::ZEntryPoint(QWidget *parent) :QMainWindow(parent),
//                                                     ui(new Ui::ZEntryPoint)
ZEntryPoint::ZEntryPoint(QWidget *parent) :QMainWindow(parent)
{
//  setAttribute(Qt::WA_DeleteOnClose , true); // first main window cannot be delete on close otherwise QApplication crashes at exit

  mainQAg = new QActionGroup(this);
  initLayout();

  QObject::connect(mainQAg, &QActionGroup::triggered, this, &ZEntryPoint::actionMenuEvent);

}
ZEntryPoint::~ZEntryPoint()
{
/*
    if (VizuRaw!=nullptr)
        delete VizuRaw;
*/
    if (entityWnd)
        delete entityWnd;
/*
    if (RandomFile)
        if (RandomFile->isOpen())
        {
            RandomFile->zclose();
            delete RandomFile;
        }
*/
    if (RawMasterFile)
    {
        if (RawMasterFile->isOpen())
            RawMasterFile->zclose();
        delete RawMasterFile;
    }
/*
    if (Fd>=0)
        ::close(Fd);
*/
//    delete ui;
}

void
ZEntryPoint::initLayout()
{
    resize(868, 50);
    setWindowTitle("Content file utilities");


    /*=======================Menu main level==============================*/

    menubar = new QMenuBar(this);
    menubar->setGeometry(QRect(0, 0, 868, 20));

    setMenuBar(menubar);
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);


    /*----------Leading quit icon--------------*/

    uriString wQuitImg;
    wQuitImg = GeneralParameters.getIconDirectory();
    wQuitImg.addConditionalDirectoryDelimiter();
    wQuitImg += "system-shutdown.png";
    QIcon wQuitICn(wQuitImg.toCChar());

    QuitIconQAc = new QAction("",menubar);
    QuitIconQAc->setIcon(wQuitICn);

    menubar->addAction(QuitIconQAc);
    mainQAg->addAction(QuitIconQAc);



    /*======================== General =============================*/
    generalMEn = new QMenu("General",menubar);
    menubar->addMenu(generalMEn);

    ParametersQMe=new QMenu(QObject::tr("Parameters"),this);
    generalMEn->addMenu(ParametersQMe);

    ParamLoadQAc=new QAction(QObject::tr("Load xml parameter file"),this);
    ParametersQMe->addAction(ParamLoadQAc);
    mainQAg->addAction(ParamLoadQAc);

    ParamChangeQAc=new QAction(QObject::tr("Change current parameters"),this);
    ParametersQMe->addAction(ParamChangeQAc);
    mainQAg->addAction(ParamChangeQAc);

    QuitQAc = new QAction("Quit",this);
    generalMEn->addAction(QuitQAc);
    mainQAg->addAction(QuitQAc);

    /*==================== Raw file ============================*/

    rawMEn = new QMenu("Raw file", menubar);
    menubar->addMenu(rawMEn);

    rawDumpQAc = new QAction("Raw dump",this);
    rawMEn->addAction(rawDumpQAc);
    mainQAg->addAction(rawDumpQAc);

    surfaceScanRawQAc = new QAction("Surface dump (ZRF)",this);
    rawMEn->addAction(surfaceScanRawQAc);
    mainQAg->addAction(surfaceScanRawQAc);

    /*====================== Header file ========================*/
    headerMEn = new QMenu("Header file",menubar);
    menubar->addMenu(headerMEn);


    openZRHQAc = new QAction("Open header file",this);
    headerMEn->addAction(openZRHQAc);
    mainQAg->addAction(openZRHQAc);/* open new header file */

    displayFCBQAc = new QAction("File Control Block",this);
    headerMEn->addAction(displayFCBQAc);
    mainQAg->addAction(displayFCBQAc);

    displayHCBQAc = new QAction("Header Control Block",this);
    headerMEn->addAction(displayHCBQAc);
    mainQAg->addAction(displayHCBQAc);

    displayMCBQAc = new QAction("Master Control Block",this);
    headerMEn->addAction(displayMCBQAc);
    mainQAg->addAction(displayMCBQAc);

    displayICBQAc = new QAction(QObject::tr("Index control blocks","ZEntryPoint"),this);
    headerMEn->addAction(displayICBQAc);
    mainQAg->addAction(displayICBQAc);

    viewpoolMEn = new QMenu("View pool",headerMEn);
    headerMEn->addMenu(viewpoolMEn);

    displayZBATQAc = new QAction("Block Access Table",this);
    viewpoolMEn->addAction(displayZBATQAc);
    mainQAg->addAction(displayZBATQAc);
/*
    displayZDBTQAc = new QAction("Deleted Blocks Table",this);
    viewpoolMEn->addAction(displayZDBTQAc();
    mainQAg->addAction(displayZDBTQAc);
*/
    displayZFBTQAc = new QAction("Free Blocks Table",this);
    viewpoolMEn->addAction(displayZFBTQAc);
    mainQAg->addAction(displayZFBTQAc);

//  mainQAg->addAction(displayZDBTQAc);


  displayHCBQAc->setEnabled(false);
  displayMCBQAc->setEnabled(false);
  displayICBQAc->setEnabled(false);
  displayFCBQAc->setEnabled(false);
  viewpoolMEn->setEnabled(false);

  headerMEn->setEnabled(true);

/*==================== Random file =======================*/

  RandomFileMEn = new QMenu("Random file",this);
  menubar->addMenu(RandomFileMEn);

  ZRFPoolAccessQAc = new QAction("Access by pool",this);
  RandomFileMEn->addAction(ZRFPoolAccessQAc);
  mainQAg->addAction(ZRFPoolAccessQAc);

  ZRFSurfaceAccessQAc = new QAction("Access by surface scan",this);
  RandomFileMEn->addAction(ZRFSurfaceAccessQAc);
  mainQAg->addAction(ZRFSurfaceAccessQAc);


  ZRFCloneQAc = new QAction("Clone",this);
  RandomFileMEn->addAction(ZRFCloneQAc);
  mainQAg->addAction(ZRFCloneQAc);

  ZRFClearQAc = new QAction("Clear",this);
  RandomFileMEn->addAction(ZRFClearQAc);
  mainQAg->addAction(ZRFClearQAc);

  ZRFReorgQAc = new QAction("Reorganize",this);
  RandomFileMEn->addAction(ZRFReorgQAc);
  mainQAg->addAction(ZRFReorgQAc);

  ZRFRebuildHeaderQAc = new QAction("Rebuild header",this);
  RandomFileMEn->addAction(ZRFRebuildHeaderQAc);
  mainQAg->addAction(ZRFRebuildHeaderQAc);

  ZRFtruncateQAc = new QAction("Truncate file",this);
  RandomFileMEn->addAction(ZRFtruncateQAc);
  mainQAg->addAction(ZRFtruncateQAc);

  ZRFextendQAc = new QAction("Extend File",this);
  RandomFileMEn->addAction(ZRFextendQAc);
  mainQAg->addAction(ZRFextendQAc);

  ZRFunlockQAc = new QAction("Unlock file", this);
  RandomFileMEn->addAction(ZRFunlockQAc);
  mainQAg->addAction(ZRFunlockQAc);

  ZRFSurfaceScanQAc = new QAction("Surface scan",this);
  RandomFileMEn->addAction(ZRFSurfaceScanQAc);
  mainQAg->addAction(ZRFSurfaceScanQAc);

  ZRFupgradetoZMFQAc = new QAction("upgrade to MasterFile",this);
  RandomFileMEn->addAction(ZRFupgradetoZMFQAc);
  mainQAg->addAction(ZRFupgradetoZMFQAc);

  ZRFPutthemessQAc = new QAction("Put the mess in ZBAT",this);
  RandomFileMEn->addAction(ZRFPutthemessQAc);
  mainQAg->addAction(ZRFPutthemessQAc);


/*=================== Master file ========================*/
  MasterFileMEn = new QMenu("Master file",this);
  menubar->addMenu(MasterFileMEn);

  DictionaryQAc=new QAction("Dictionaries management",this);
  MasterFileMEn->addAction(DictionaryQAc);
  mainQAg->addAction(DictionaryQAc);

  //   ZMFQueryQAc = new QAction("Query",this);
  //   MasterFileMEn->addAction(ZMFQueryQAc);

  ZmfDefQAc = new QAction("Master file definition",this);
  MasterFileMEn->addAction(ZmfDefQAc);
  mainQAg->addAction(ZmfDefQAc);

  IndexRebuildQAc = new QAction("Rebuild index",this);
  MasterFileMEn->addAction(IndexRebuildQAc);
  mainQAg->addAction(IndexRebuildQAc);

  ZMFExportQAc = new QAction("Export data from standard access",this);
  MasterFileMEn->addAction(ZMFExportQAc);
  mainQAg->addAction(ZMFExportQAc);

  ZMFExportFromSurfaceQAc = new QAction("Export data from surface scan");
  MasterFileMEn->addAction(ZMFExportFromSurfaceQAc);
  mainQAg->addAction(ZMFExportFromSurfaceQAc);

  ZMFImportQAc = new QAction("Import data",this);
  MasterFileMEn->addAction(ZMFImportQAc);
  mainQAg->addAction(ZMFImportQAc);

  ZMFClearQAc = new QAction("Clear all file data",this);
  MasterFileMEn->addAction(ZMFClearQAc);
  mainQAg->addAction(ZMFClearQAc);

  ZMFBackupQAc = new QAction("Backup file and all indexes",this);
  MasterFileMEn->addAction(ZMFBackupQAc);
  mainQAg->addAction(ZMFBackupQAc);

  ZMFRestoreQAc = new QAction("Restore file and all indexes",this);
  MasterFileMEn->addAction(ZMFRestoreQAc);
  mainQAg->addAction(ZMFRestoreQAc);

  ZMFdowngradetoZRFQAc = new QAction("dowgrade to RandomFile",this);
  MasterFileMEn->addAction(ZMFdowngradetoZRFQAc);
  mainQAg->addAction(ZMFdowngradetoZRFQAc);

/*=================== Copy ========================*/
  ZCopyQAc = new QAction("Copy any file",this);
  menuBar()->addAction(ZCopyQAc);
  mainQAg->addAction(ZCopyQAc);

/*=================== Query ========================*/
  ZMFQueryQAc = new QAction("Query",this);
  menuBar()->addAction(ZMFQueryQAc);
  mainQAg->addAction(ZMFQueryQAc);

 /*

    generalMEn->addAction(openZRHQAc);
    generalMEn->addAction(unlockZRFQAc);
    generalMEn->addAction(closeQAc);
    generalMEn->addAction(QuitQAc);

    ZRFMEn->addAction(surfaceScanZRFQAc);
    ZRFMEn->addAction(RecoveryQAc);
    ZRFMEn->addAction(clearQAc);
    ZRFMEn->addAction(cloneQAc);
    ZRFMEn->addAction(truncateQAc);
    ZRFMEn->addAction(extendQAc);
    ZRFMEn->addAction(rebuildHeaderQAc);
    ZRFMEn->addAction(reorganizeQAc);
    ZRFMEn->addAction(upgradeZRFtoZMFQAc);
    ZMFMEn->addAction(listIndexesQAc);
    ZMFMEn->addAction(menurepair_all_indexes->menuAction());
    ZMFMEn->addAction(downgradeZMFtoZRFQAc);
    ZMFMEn->addAction(reorganizeZMFQAc);
    ZMFMEn->addAction(removeIndexQAc);
    ZMFMEn->addAction(addIndexQAc);
    ZMFMEn->addAction(rebuildIndexQAc);
    ZMFMEn->addAction(extractIndexQAc);
    ZMFMEn->addAction(extractAllIndexesQAc);
    ZMFMEn->addAction(MCBReportQAc);
*/








    /*
  ZRFVersionLBl->setText(getVersionStr(__ZRF_VERSION__).toCChar());
  ZMFVersionLBl->setText(getVersionStr(__ZMF_VERSION__).toCChar());
*/
/*
    openZRFQAc = new QAction("Random file",this);
    menubar->addAction(openZRFQAc);
*/




} // initSetup


/*
void ZEntryPoint::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (FResizeInitial) {
    FResizeInitial=false;
    return;
  }
  QRect wR1 = verticalLayoutWidget->geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  verticalLayoutWidget->resize(wVW,wVH);

}//ZEntryPoint::resizeEvent
*/
void
ZEntryPoint::actionMenuEvent(QAction* pAction)
{
  utf8VaryingString wStr;
  ZStatus wSt=ZS_SUCCESS;

  /*===============================*/
  /* general parameters */

  if (pAction==ParamLoadQAc) {
      uriString wDir = GeneralParameters.getParamDirectory();
      QString wFileName = QFileDialog::getOpenFileName(this, tr("Xml parameter file"),
                                                       wDir.toCChar(),
                                                       "xml files (*.xml);;All (*.*)");
      if (wFileName.isEmpty())
          return;
      uriString wXmlParams= wFileName.toUtf8().data();
      if (!wXmlParams.exists())
          return;
      wSt=GeneralParameters.XmlLoad(wXmlParams,nullptr);
      if (wSt!=ZS_SUCCESS) {
          utf8VaryingString wExcp = ZException.last().formatFullUserMessage().toString();
          ZExceptionDLg::adhocMessage("Load parameters",Severity_Error,
                                      nullptr,&wExcp,"Error while loading xml parameter file %s",wXmlParams.toString());
          return;
      }
      return;
  } //ParamLoadQAc

  if (pAction==ParamChangeQAc) {
      ZGeneralParamsDLg* wParamDLg = new ZGeneralParamsDLg(this);
      wParamDLg->setup(GeneralParameters);
      wParamDLg->show();
      int wRet=wParamDLg->exec();
      if (wRet==QDialog::Rejected)
          return;
      return;
  }
  /* end general parameters */

  if ((pAction==QuitQAc)||(pAction==QuitIconQAc))
  {
      actionClose();
      //    this->deleteLater();
      QApplication::quit();
      return;
  }
   /*===============================*/

  if (pAction==rawDumpQAc) {
      uriString wDir = GeneralParameters.getWorkDirectory();
      QString wFileName = QFileDialog::getOpenFileName(this, tr("Open raw file"),
                                                       wDir.toCChar(),
                                                       "ZContent files (*.zrf *.zmf *.zix *.zrh);;All (*.*)");
      if (wFileName.isEmpty()) {
          return;
      }

      ZRawVisuMain* wVisu = new ZRawVisuMain(this);
      URICurrent = wFileName.toUtf8().data();
      wVisu->show();

      wVisu->setUpRawFile(URICurrent);
      //    ZRawMasterFileVisu* wVisu= new ZRawMasterFileVisu(this);
      //    wVisu->setup(URICurrent, Fd);
      //    wVisu->setup(URICurrent);
      //    wVisu->setModal(false);
      //    wVisu->firstIterate();

      return;
  } // rawDumpQAc

  if (pAction==surfaceScanRawQAc)
  {
      ZRFSurfaceScan();
      return;
  }



  /*===============================*/

  if (pAction==ZRFunlockQAc)
  {
      ZRFUnlock();
      return;
  }

  if (pAction==ZRFPoolAccessQAc)
  {
      ZRFAccessByPool();
      return;
  }
  if (pAction==ZRFSurfaceAccessQAc)
  {
      ZRFSurfaceAccess();
      return;
  }


  if (pAction==openZRHQAc)
  {
    wSt=openZRH();
    return;
  }

  if (pAction==displayHCBQAc)
  {
      displayHCB();
    return;
  }
  if (pAction==displayFCBQAc)
  {
    displayFCB();
    return;
  }
  if (pAction==displayMCBQAc)
  {
      displayMCB();
    return;
  }
  if (pAction==displayICBQAc)
  {
    displayICBs();
    return;
  }

  if (pAction==displayZBATQAc)
  {
    displayZBAT();
    return;
  }

  if (pAction==displayZFBTQAc)
  {
    displayZFBT();
    return;
  }
  if (pAction==ZCopyQAc)
  {
      ZRFCopy();
      return;
  }
  if (pAction==ZRFCloneQAc)
  {
      cloneZRF();
      return;
  }

  if (pAction==ZRFClearQAc)
  {
      clearZRF();
      return;
  }
  if (pAction==ZRFReorgQAc)
  {
      ZRFReorganize();
      return;
  }
  if (pAction==ZRFRebuildHeaderQAc)
  {
      ZRFrebuildHeader();
      return;
  }
  if (pAction==ZRFSurfaceScanQAc)
  {
      ZRFSurfaceScan();
      return;
  }
  if (pAction==ZRFupgradetoZMFQAc)
  {
      ZRFupgradetoZMF();
      return;
  }
  if (pAction==ZMFdowngradetoZRFQAc)
  {
      ZMFdowngradetoZRF();
      return;
  }
  if (pAction==ZRFPutthemessQAc)
  {
      ZRFPutthemessinZBAT();
      return;
  }


  if (pAction==DictionaryQAc)
  {
    if (DicEdit==nullptr)
      DicEdit=new DicEditMWn(std::bind(&ZEntryPoint::DicEditQuitCallback, this),this);

    DicEdit->show();
    return;
  }

  if (pAction==ZMFQueryQAc) {
    QueryMWd= new ZSearchQueryMWd(this);
    QueryMWd->show();
    return;
  }

  if (pAction==ZmfDefQAc) {
    FileGenerate= new FileGenerateMWn(this);
    FileGenerate->show();
    return;
  }

  if (pAction==IndexRebuildQAc) {
    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, tr("Master file"),
        wDir.toCChar(),
        "master files (*.zmf);;All (*.*)");
    if (wFileName.isEmpty())
      return;
    uriString wZMFURI= wFileName.toUtf8().data();
    if (!wZMFURI.exists())
      return;
    ZMasterFile wZMF;
    ZStatus wSt=wZMF.zopen(wZMFURI,ZRF_All);
    if (wSt!=ZS_SUCCESS) {
      utf8VaryingString wExcp = ZException.last().formatFullUserMessage().toString();
      ZExceptionDLg::adhocMessage("Rebuild index",Severity_Error,
          nullptr,&wExcp,"Error while opening master file %s",wZMFURI.toString());
      return;
    }
    if (wZMF.IndexTable.count()==0) {
      ZExceptionDLg::adhocMessage("Rebuild index",Severity_Error,
          "Master file %s\nhas no index key.",wZMFURI.toString());
       wZMF.zclose();
      return;
    }

    ZRawKeyListDLg* KeyListDLg=new ZRawKeyListDLg(this);
    KeyListDLg->set(&wZMF);

    int wRet = KeyListDLg->exec();
    if (wRet==QDialog::Rejected) {
      wZMF.zclose();
      delete KeyListDLg;
      return;
    }
    if (wRet!=QDialog::Accepted) {
        wZMF.zclose();
        delete KeyListDLg;
        return;
    }

      wSt=wZMF.rebuildIndex(KeyListDLg->getCurrentIndexRank(),nullptr,&ErrorLog);

      if (wSt!=ZS_SUCCESS) {
        ZExceptionDLg::displayLast("Index rebuild");
      }

      wZMF.zclose();
      delete KeyListDLg;
      return;

  }//IndexRebuildQAc

  if (pAction==ZMFExportQAc) {
    uriString wDir = GeneralParameters.getWorkDirectory();
    ZRawMasterFile *wZMF=nullptr;
    ZStatus wSt=ZS_SUCCESS;
    bool wHasBeenOpened=false;
    bool wHasBeenCreated=false;
    while (true) {
        if (RawMasterFile!=nullptr) {
          int wRet=ZExceptionDLg::adhocMessage3B("Export raw master file",Severity_Question,"Other","Quit","Use",nullptr,nullptr,
                                        "There is a current raw masterfile in use :\n"
                                        "%s\n"
                                        "Do you want to export this raw masterfile data ?\n"
                                        "Use it                      <Use>\n"
                                        "Choose another one          <Other>\n"
                                        "Quit without doing anything <Quit>\n",
                                        RawMasterFile->getURIContent().toString());
          if (wRet==QDialog::Rejected)
            return;
          if (wRet==QDialog::Accepted) {
            wZMF = RawMasterFile;
            if (!wZMF->isOpen()) {
                wSt=wZMF->zopen(ZRF_Read_Only);
                if (wSt!=ZS_SUCCESS) {
                    utf8VaryingString wExcp = ZException.last().formatFullUserMessage().toString();
                    ZExceptionDLg::adhocMessage("ZMF data export",Severity_Error,
                                                nullptr,&wExcp,"Error while opening master file %s",wZMF->getURIContent().toString());
                    return;
                }
                wHasBeenOpened=true;
            }
            break;
          }// QDialog::Accepted)
          /* ZEDLG_Third : let's go to file selection */
          //if (wRet==QDialog::ZEDLG_Third) {
          //} // ZEDLG_Third
        } //if (RawMasterFile!=nullptr)

        QString wFileName = QFileDialog::getOpenFileName(this, tr("Raw master file"),
                                                         wDir.toCChar(),
                                                         "master files (*.zmf);;All (*.*)");
        if (wFileName.isEmpty())
          return;
        uriString wZMFURI= wFileName.toUtf8().data();
        if (!wZMFURI.exists())
          return;
        wZMF = new ZRawMasterFile;
        wHasBeenCreated=true;
        wSt=wZMF->zopen(wZMFURI,ZRF_Read_Only);
        if (wSt!=ZS_SUCCESS) {
          utf8VaryingString wExcp = ZException.last().formatFullUserMessage().toString();
          ZExceptionDLg::adhocMessage("ZMF data export",Severity_Error,
                                      nullptr,&wExcp,"Error while opening master file %s",wZMFURI.toString());
          return;
        }
        wHasBeenOpened=true;
        break;
    } // while true

    wSt=exportZMF(this,wZMF);
    if (wHasBeenOpened)
        wZMF->zclose();
    if (wHasBeenCreated)
        delete wZMF;
    return;
  }//ZMFExportQAc

  if (pAction==ZMFExportFromSurfaceQAc) {
      exportZMFFromSurface();
      return;
  }//ZMFExportQAc


  if (pAction==ZMFImportQAc) {
    initLogWindow();
    wSt=importZMF(this, &ErrorLog);
    return;
  }//ZMFImportQAc

  if (pAction==ZMFBackupQAc) {
    ZBackupDLg wBckDLg(this);

    int wRet=wBckDLg.exec();

  } //ZMFBackupQAc

  if (pAction==ZMFRestoreQAc) {
    ZRestoreDLg RestoreDLg(this);
    int wRet=RestoreDLg.exec();

  } //ZMFRestoreQAc

  if (pAction==ZMFClearQAc) {
    clearZMF(this);
  } //ZMFClearQAc


  if (pAction==ZRFSurfaceScanQAc) {
//    surfaceScanZRF();
    return;
  }


  /* ZMFMEn */

  if (pAction==runRepairQAc) {
    repairIndexes(testRunQAc->isChecked(), rebuilAllQAc->isChecked());
    return;
  }


  return;

}//actionMenuEvent


void ZEntryPoint::DicEditQuitCallback(){
    //  delete dictionaryWnd;
    DicEdit = nullptr;
}






void ZEntryPoint::repairIndexes(bool pTestRun,bool pRebuildAll)
{
    initLogWindow();

    uriString wDir = GeneralParameters.getWorkDirectory();
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Master file"),
                                                     wDir.toCChar(),
                                                     "master files (*.zmf);;All (*.*)");
    if (wFileName.isEmpty())
        return;
    uriString wZMFURI= wFileName.toUtf8().data();
    if (!wZMFURI.exists())
        return;

/*
  FILE* wRepairLog = fopen("repairindex.log","w");
*/
  wSt=zrepairIndexes( URICurrent.toCChar(),
                       pTestRun,
                       pRebuildAll,
                       &ErrorLog);
/*
  fflush(wRepairLog);
  fclose(wRepairLog);

  openGenLogWin();
  GenlogWin->setTextFromFile("repairindex.log");
  GenlogWin->show();

  if (wSt==ZS_SUCCESS)
    return;

  ZExceptionDLg::displayLast();
*/
  return;
}

void ZEntryPoint::removeIndex()
{

}

void ZEntryPoint::addIndex()
{

}

void ZEntryPoint::rebuildIndex()
{

}

void
ZEntryPoint::extractIndex()
{

}

void
ZEntryPoint::dowdgrade()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, "Master file to downgrade",
                                                     wDir.toCChar(),
                                                     "Master files (*.zmf);;All (*.*)");
    if (wFileName.isEmpty()) {
        return;
    }
    uriString wURIMaster = wFileName.toUtf8().data();

    initLogWindow();

    zdowngradeZMFtoZRF(wURIMaster,&ErrorLog);
}
void
ZEntryPoint::reorganizeZMF()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, "Master file to reorg",
                                                     wDir.toCChar(),
                                                     "Master files (*.zmf);;All (*.*)");
    if (wFileName.isEmpty()) {
        return;
    }
    uriString wURIMaster = wFileName.toUtf8().data();

    initLogWindow();

    zreorgMasterFile(wURIMaster,-1,&ErrorLog);
}

void
ZEntryPoint::ZRFUnlock()
{
    ZStatus wSt=ZS_SUCCESS;
    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, "Content file Unlock",
                                                     wDir.toCChar(),
                                                     "Random files (*.zmf *.zrf *.zrh);;All (*.*)");
    if (wFileName.isEmpty()) {
        return;
    }
    uriString wURIMaster = wFileName.toUtf8().data();
    if (wURIMaster.getFileExtension()==__HEADEREXTENSIONSTRING__) {
         wSt=ZRandomFile::zutilityUnlockHeaderFile(wURIMaster);
    }
    else {
         wSt=ZRandomFile::zutilityUnlockZRF(wURIMaster);
    }
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Unlock file");
    ZException.pop();
  }
}


ZStatus
ZEntryPoint::ZRFAccessByPool()
{
    ZStatus wSt = ZS_SUCCESS;
    QString wFileName = QFileDialog::getOpenFileName(this, "Open as random file",
                                                        GeneralParameters.getWorkDirectory().toCChar(),
                                                        "Random files(*.zmf *.zrf);;All(*.*)");

    if (wFileName.isEmpty())
      return ZS_CANCEL;

    URICurrent = wFileName.toUtf8().data();

    uriString wURIHeader;
    generateURIHeader(URICurrent,wURIHeader);

    while (!wURIHeader.exists()) {
      int wRet = ZExceptionDLg::adhocMessage2B("Header file",Severity_Error,"Give up","Search",
          nullptr,nullptr,
          "Header file <%s> does not exist.\n"
          "Give up operation or search a new one.",wURIHeader.toCChar());
      if (wRet==QDialog::Rejected)
        return ZS_CANCEL;
      wFileName = QFileDialog::getOpenFileName(this, tr("Open File"),
          GeneralParameters.getWorkDirectory().toCChar(),
          "Header (*.zrh);;All (*.*)");
      if (wFileName.isEmpty())
        return ZS_CANCEL;

      wURIHeader = wFileName.toUtf8().data();
    }

    poolVisu* wPoolVisu = new poolVisu(this);

    wSt=wPoolVisu->set(URICurrent,wURIHeader);

    wPoolVisu->dataSetup(0);

    wPoolVisu->show();

    return wSt;
}//ZRFAccessByPool

ZStatus
ZEntryPoint::ZRFSurfaceAccess()
{
    ZStatus wSt = ZS_SUCCESS;
    QString wFileName = QFileDialog::getOpenFileName(this, "Open as random file",
                                                     GeneralParameters.getWorkDirectory().toCChar(),
                                                     "Random files(*.zmf *.zrf *.zix);;All(*.*)");

    if (wFileName.isEmpty())
        return ZS_CANCEL;

    URICurrent = wFileName.toUtf8().data();

    ZRawMasterFileVisu* wZMFVisu = new ZRawMasterFileVisu(this);
    wZMFVisu->setup(URICurrent);
    wZMFVisu->firstIterate();
    wZMFVisu->show();
    /*
    zaddress_type wAddress=0;
    ZDataBuffer wRecord;
    ZStatus wSt=wZMFVisu->searchNextBlock(wRecord)
*/
    return ZS_SUCCESS;
}//ZRFSurfaceAccess





void
ZEntryPoint::showStatusBar(const char* pFormat,...)
{
    utf8VaryingString wString;
    va_list arglist;
    va_start (arglist, pFormat);
    wString.vsnprintf(cst_MaxSprintfBufferCount,pFormat,arglist);
    va_end(arglist);
    showStatusBar(wString);
}

void
ZEntryPoint::showStatusBar(const utf8VaryingString& pString)
{
    statusBar()->showMessage(pString.toCChar());
}





ZStatus
ZEntryPoint::openZRH()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, tr("Open raw file"),
                                                     wDir.toCChar(),
                                                     "Header files (*.zrh);;All (*.*)");
    if (wFileName.isEmpty()) {
        return ZS_CANCEL;
    }
  ZStatus wSt=ZS_SUCCESS;
  URIHeader = wFileName.toUtf8().data();

  wSt=URIHeader.loadContent(HeaderData);

  if (wSt!=ZS_SUCCESS)
      return wSt;

  showStatusBar("Loaded header <%s>",URIHeader.getBasename().toString());

  displayHCBQAc->setEnabled(true);
  displayICBQAc->setEnabled(true);
  displayMCBQAc->setEnabled(true);
  displayFCBQAc->setEnabled(true);

  viewpoolMEn->setEnabled(true);

  return ZS_SUCCESS;
}//openZRH

void ZEntryPoint::displayHCB()
{
    initLogWindow();

    if (URIHeader.isEmpty()) {
        ZExceptionDLg::adhocMessage("Display header file",Severity_Error,nullptr,nullptr,
                                    "No header file has been loaded yet.\n"
                                    "Please load one. "
                                    );
        return;
    }

  if (!entityWnd)
    entityWnd=new DisplayMain(&URIHeader,this);

  entityWnd->displayHCB(HeaderData);
}

void
ZEntryPoint::displayFCB()
{
    initLogWindow();

    if (URIHeader.isEmpty()) {
        ZExceptionDLg::adhocMessage("Display header file",Severity_Error,nullptr,nullptr,
                                    "No header file has been loaded yet.\n"
                                    "Please load one. "
                                    );
        return;
    }

    if (!entityWnd)
        entityWnd=new DisplayMain(&URIHeader,this);
  entityWnd->displayFCB(HeaderData);
}

void
ZEntryPoint::displayMCB()
{
    initLogWindow();

    if (URIHeader.isEmpty()) {
        ZExceptionDLg::adhocMessage("Display header file",Severity_Error,nullptr,nullptr,
                                    "No header file has been loaded yet.\n"
                                    "Please load one. "
                                    );
        return;
    }

    if (!entityWnd)
        entityWnd=new DisplayMain(&URIHeader,this);

  entityWnd->displayMCB(HeaderData);
}


void
ZEntryPoint::displayICBs()
{
  /* HCB and MCB are deserialized here just to test if there is enough space.
   * these values are computed again within DisplayMain::--- to access real values
   */
    if (URIHeader.isEmpty()) {
        ZExceptionDLg::adhocMessage("Display header file",Severity_Error,nullptr,nullptr,
                                    "No header file has been loaded yet.\n"
                                    "Please load one. "
                                    );
        return;
    }
  const unsigned char* wPtr = HeaderData.Data;
  ZHeaderControlBlock_Export wZHCBe;
  memmove(&wZHCBe,HeaderData.Data,sizeof(ZHeaderControlBlock_Export));
  if (wZHCBe.StartSign!=cst_ZBLOCKSTART) {
    ZExceptionDLg::adhocMessage("Invalid Header",Severity_Error,nullptr,nullptr,"Header Control Block appears to be corrupted.");
    return;
  }

  wZHCBe.deserialize();

  /* Index file : ICB is the only content of reserved block */
  if (wZHCBe.FileType==ZFT_ZIndexFile) {
    if (!testRequestedSize(URIHeader,HeaderData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZICB_Export)))
      return;
    if (!entityWnd)
      entityWnd=new DisplayMain(&URIHeader,this);
    const unsigned char* wPtrIn=HeaderData.Data + wZHCBe.OffsetReserved;
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

  if (!testRequestedSize(URIHeader,HeaderData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)+wZMCBe.ICBOffset + wZMCBe.ICBSize))
    return;
  if (!entityWnd)
    entityWnd=new DisplayMain(&URIHeader,this);

  entityWnd->displayICBs(HeaderData);

  return;
}

void
ZEntryPoint::displayZBAT()
{
    if (URIHeader.isEmpty()) {
        ZExceptionDLg::adhocMessage("Display header file",Severity_Error,nullptr,nullptr,
                                    "No header file has been loaded yet.\n"
                                    "Please load one. "
                                    );
        return;
    }
  unsigned char* wPtr=HeaderData.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)HeaderData.Data;
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

  utf8VaryingString wStr = URIHeader.getBasename() ;
  wStr += "::ZBAT Block Access Table";

  displayPool(wPtr,wOffset,wStr);
  return;
}

void
ZEntryPoint::displayPool(const unsigned char* pPtr,zaddress_type pOffset,const utf8VaryingString& pTitle)
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
      size_t(pPtr-RawData.Data),size_t(pPtr-RawData.Data),pOffset,pOffset,
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

//    initLogWindow();
//  textEditMWn* wTEx=openGenLogWin();
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

#ifdef __DEPRECATED__
void
ZEntryPoint::displayZDBT()
{
  if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return;

  const unsigned char* wPtr=RawData.Data;
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
#endif // __DEPRECATED__
void
ZEntryPoint::displayZFBT()
{
    if (URIHeader.isEmpty()) {
        ZExceptionDLg::adhocMessage("Display header file",Severity_Error,nullptr,nullptr,
                                    "No header file has been loaded yet.\n"
                                    "Please load one. "
                                    );
        return;
    }

  unsigned char* wPtr=HeaderData.Data;
  ZHeaderControlBlock_Export* wHe = (ZHeaderControlBlock_Export*)HeaderData.Data;
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
  utf8VaryingString wStr = URIHeader.getBasename() ;
  wStr += "::FBT Free blocks table";
  displayPool(wPtr,wOffset,wStr);
}//displayZFBT



bool
ZEntryPoint::testRequestedSize(const uriString& pURI, ZDataBuffer& pRawData,size_t pRequestedSize){
  if (pRawData.Size < pRequestedSize)
  {
    int wRet=ZExceptionDLg::adhocMessage2B("displayICBs",Severity_Warning,"Quit","Load",
        nullptr,nullptr,
        "Not enough data loaded. Requested minimum size is <%ld> bytes.\n"
        "Only <%ld> Bytes have been loaded.\n\n"
        "Load / reload file content <Reload>\n"
        "Quit <Quit>",pRequestedSize);

    if (wRet==ZEDLG_Rejected)
      return false;
    //      case ZEDLG_Accepted:
    if (pURI.getFileSize()>100000000)
    {
      ZExceptionDLg::message("ZEntryPoint::displayMCB",ZS_INVSIZE,Severity_Error,
          "Cannot load content of file <%s> size <%lld> exceeds memory capacity",
          pURI.toString(),
          pURI.getFileSize());
      return false;
    }
    if (pURI.getFileSize()<(pRequestedSize+1))
    {
      ZExceptionDLg::message("ZEntryPoint::displayMCB",ZS_INVSIZE,Severity_Error,
          "file <%s> has size <%lld> that does not allow to store requested header data.",
          pURI.toString(),
          pURI.getFileSize());
      return false;
    }
    if (pURI.loadContent(pRawData)!=ZS_SUCCESS)
      return false;
  }//if (RawData.Size < sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZSMCBOwnData_Export))
  return true;
}


/*
void
ZEntryPoint::Dictionary()
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
ZEntryPoint::actionClose(bool pChecked)
{
    /*
  displayWidgetBlockOnce=false;
  if (RandomFile)
    {
    RandomFile->zclose();
    delete RandomFile;
    RandomFile=nullptr;
    }

  if (RawMasterFile)
    {
      RawMasterFile->zclose();
      delete RawMasterFile;
      RawMasterFile=nullptr;
    }
  if (Fd >= 0)
      {
      ::close(Fd);
      Fd=-1;
      }

    rawMEn->setEnabled(false);
    ZRFMEn->setEnabled(false);
    ZMFMEn->setEnabled(false);


    OpenMode=VMD_Nothing;
*/
  return;
}//actionClose




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






void ZEntryPoint::closeMCBCB(const QEvent *pEvent)
{
  MCBWin=nullptr;
}
void
ZEntryPoint::textEditMorePressed()
{

}
ZStatus
ZEntryPoint::exportZMFFromSurface()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Raw master file"),
                                                     wDir.toCChar(),
                                                     "master files (*.zmf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_CANCEL;
    uriString wZMFURI= wFileName.toUtf8().data();
    if (!wZMFURI.exists())
        return ZS_FILENOTEXIST;

    wFileName = QFileDialog::getSaveFileName(this, tr("Target Xml file"),
                                             wDir.toCChar(),
                                             "Xml files (*.xml);;All (*.*)");

    if (wFileName.isEmpty())
        return ZS_CANCEL;
    uriString wXmlURI= wFileName.toUtf8().data();

    if (wXmlURI.getFileExtension().isEmpty())
        wXmlURI += ".xml";

    ZaiErrors* wErrorLog = new ZaiErrors;
    wErrorLog->setAutoPrintOn(ZAIES_Text);

    utf8VaryingString wStr = wZMFURI.getBasename();
    wStr += "::Export content";
    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle(wStr.toCChar());
    wTEMWn->registerDisplayColorCallBack(wErrorLog);
    wTEMWn->show();

    ZMFProgressMWn* wProgress = new ZMFProgressMWn("Data export by surface scan",this,ZPTP_TimedClose);
    wProgress->show();

    wSt=ZRawMasterFile::XmlExportContentFromSurfaceScan(wZMFURI,wXmlURI,
                                                          std::bind(&ZMFProgressMWn::advanceCallBack,wProgress,std::placeholders::_1,std::placeholders::_2),
                                                          std::bind(&ZMFProgressMWn::advanceSetupCallBack,wProgress,std::placeholders::_1,std::placeholders::_2),
                                                          wErrorLog );

    wProgress->setDone(wSt);

    delete wErrorLog;

    return wSt;
}

ZStatus
ZEntryPoint::exportZMF(QWidget* pParent,zbs::ZRawMasterFile* pRawMasterFile)
{
  if (pRawMasterFile==nullptr) {
    ZExceptionDLg::adhocMessage("Data export",Severity_Error,
                                "Please select a (raw) master file before exporting its data.");
    return ZS_FILENOTEXIST;
  }

  if (pRawMasterFile->getRecordCount()==0) {
    ZExceptionDLg::adhocMessage("Data export",Severity_Error,
                                "No record in file. Nothing to export.");
    return ZS_EMPTY;
  }


  QString wDir;
  wDir = QFileDialog::getSaveFileName(pParent,"Export content",GeneralParameters.getWorkDirectory().toCChar(),"Xml files (*.xml);;All (*.*)");
  if (wDir.isEmpty())
    return ZS_EMPTY ;
  uriString wURIExport=wDir.toUtf8().data();

  /* export progress window setup */

  ZMFProgressMWn* ProgressMWn = new ZMFProgressMWn("Data export",pParent,ZPTP_TimedClose);


  ProgressMWn->labelsSetup("Export to file",wURIExport,"Records processed");

  ProgressMWn->advanceSetupCallBack(pRawMasterFile->getRecordCount(),"Export to file");

  ProgressMWn->show();

  pRawMasterFile->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack, ProgressMWn,std::placeholders::_1,std::placeholders::_2));
  pRawMasterFile->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack, ProgressMWn,std::placeholders::_1,std::placeholders::_2));

  ZaiErrors* wErrorLog = new ZaiErrors;
  wErrorLog->setAutoPrintOn(ZAIES_Text);

  utf8VaryingString wStr = pRawMasterFile->getURIContent().getBasename();
  wStr += "::Export content";
  textEditMWn* wTEMWn = new textEditMWn(pParent);
  wTEMWn->setWindowTitle(wStr.toCChar());
  wTEMWn->registerDisplayColorCallBack(wErrorLog);
  wTEMWn->show();

  ZStatus wSt=pRawMasterFile->XmlExportContent(wURIExport,wErrorLog);

  ProgressMWn->setDone(wSt);

  if (wSt!=ZS_SUCCESS) {
        wErrorLog->errorLog("Data export: Something went wrong status <%s> while exporting data from <%s>",
                            decode_ZStatus(wSt),
                            pRawMasterFile->getURIContent().getBasename().toCChar());
  }

  delete wErrorLog;
  return wSt;
} //ZEntryPoint::exportZMF

ZStatus
ZEntryPoint::clearZMF(QWidget* pParent)
{
  uriString wDir = GeneralParameters.getWorkDirectory();
  uriString wZMFURI;
  ZRawMasterFile *wZMF=nullptr;
  ZStatus wSt=ZS_SUCCESS;

  QString wFileName = QFileDialog::getOpenFileName(pParent, tr("Raw master file to be cleared"),
                                                   wDir.toCChar(),
                                                   "master files (*.zmf);;All (*.*)");
  if (wFileName.isEmpty())
    return ZS_EMPTY ;
  wZMFURI= wFileName.toUtf8().data();
  if (!wZMFURI.exists()) {
    return ZS_FILENOTEXIST;
  }


  long wSize = wZMFURI.getFileSize();
  wZMF = new ZMasterFile;
  wSt=wZMF->zopen(wZMFURI,ZRF_All);
  if (wSt!=ZS_SUCCESS) {
    delete wZMF;
    return wSt;
  }


  ZClearFileDLg* wClearFileDLg = new ZClearFileDLg(pParent);
  wClearFileDLg->setup(wZMF);

  int wRet=wClearFileDLg->exec();
  if (wRet==QDialog::Rejected)
      return ZS_SUCCESS ;


  ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Clearing master file",pParent,ZPTP_TimedClose);

  wZMF->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));
  wZMF->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));

  ProgressMWn->setDescBudy("File");
  ProgressMWn->setDescText(wZMFURI.toCChar());

  ProgressMWn->show();

  bool wHighwater = wClearFileDLg->getHighwater();
  size_t wSizeToKeep = wClearFileDLg->getSizeToKeep();
  
  ZaiErrors* wErrorLog=new ZaiErrors;
  wErrorLog->setAutoPrintOn(ZAIES_Text);

  wSt = wZMF->zclearAll(wSizeToKeep,wHighwater,wErrorLog);

  ProgressMWn->setDone(wSt);

  wZMF->zclose();
  delete wZMF;
  delete wErrorLog;
  return wSt;
}

ZStatus ZEntryPoint::clearZRF()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;
    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Random file to be cleared"),
                                                     wDir.toCChar(),
                                                     "random files (*.zrf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }


    long wSize = wZRFURI.getFileSize();
    wZRF = new ZRandomFile;
    wSt=wZRF->zopen(wZRFURI,ZRF_All);
    if (wSt!=ZS_SUCCESS) {
        delete wZRF;
        return wSt;
    }

    ZClearFileDLg* wClearFileDLg = new ZClearFileDLg(this);
    wClearFileDLg->setup(wZRF);

    int wRet=wClearFileDLg->exec();
    if (wRet==QDialog::Rejected)
        return ZS_SUCCESS ;


    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Clearing random file",this,ZPTP_TimedClose);

    wZRF->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));
    wZRF->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));

    ProgressMWn->setDescBudy("File");
    ProgressMWn->setDescText(wZRFURI.toCChar());

    ProgressMWn->show();

    bool wHighwater = wClearFileDLg->getHighwater();
    size_t wSizeToKeep = wClearFileDLg->getSizeToKeep();

    wSt = wZRF->zclearFile(wSizeToKeep,wHighwater,&ErrorLog);

    ProgressMWn->setDone(wSt!=ZS_SUCCESS);

    wZRF->zclose();
    delete wZRF;
    return wSt;
}

ZStatus
ZEntryPoint::cloneZRF( )
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;
    uriString wCloneURI;
    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Source Random file to be cloned"),
                                                     wDir.toCChar(),
                                                     "random files (*.zrf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }

    wFileName = QFileDialog::getOpenFileName(this, tr("Target cloned file name"),
                                                     wDir.toCChar(),
                                                     "random files (*.zrf);;All (*.*)");

    if (!wFileName.isEmpty())
        wCloneURI=wFileName.toUtf8().data();

    wZRF = new ZRandomFile;

    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle("Clone random file log");
    wTEMWn->registerDisplayColorCallBack(&ErrorLog);

    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Cloning file",this,false);

    wZRF->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));
    wZRF->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));

    ProgressMWn->setDescBudy("File");
    ProgressMWn->setDescText(wZRFURI.toCChar());

    ProgressMWn->show();

    wSt=wZRF->_cloneFile(-1,wCloneURI,&ErrorLog);

    ProgressMWn->setDone(wSt!=ZS_SUCCESS);

    delete wZRF;
    return wSt;
}
ZStatus
ZEntryPoint::ZRFReorganize( )
{

    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle("Reorganize file");
    wTEMWn->registerDisplayColorCallBack(&ErrorLog);
    wTEMWn->show();

    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;
    uriString wCloneURI;
    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Random file to reorganize"),
                                                     wDir.toCChar(),
                                                     "random files (*.zrf *.zmf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }

    wZRF = new ZRandomFile;

    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Reorganize file",this,false);

    ProgressMWn->registerZRFCallBacks(wZRF);

    ProgressMWn->setDescBudy("File");
    ProgressMWn->setDescText(wZRFURI.toCChar());

    ProgressMWn->show();

    wSt = wZRF->zopen(wZRFURI,ZRF_Read_Only);
    wZRF->zclose();
    if (wZRF->isMasterFile()) {
        int wRet=ZExceptionDLg::adhocMessage2B("Reorganize master file",Severity_Warning,"Give up","Do it", nullptr ,nullptr,
                                                "File %s belongs to master file family\n"
                                                "Reorganizing it will induce loss of index(es) integrity.\n"
                                                "You ought to rebuild all indexes after this operation.\n",
                                                wZRFURI.toString());
        if (wRet==QDialog::Rejected)
            return ZS_CANCEL ;
    }


    wSt=wZRF->zreorgFile(-1,true,&ErrorLog);

    ProgressMWn->setDone(wSt!=ZS_SUCCESS);

    delete wZRF;
    return wSt;
}

ZStatus
ZEntryPoint::ZRFPutthemessinZBAT( )
{

    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;
    uriString wCloneURI;
    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Random file to change"),
                                                     wDir.toCChar(),
                                                     "random files (*.zrf *.zmf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }

    ZaiErrors* wErrorLog=new ZaiErrors;
    wErrorLog->setAutoPrintOn(ZAIES_Text);

    utf8VaryingString wStr = wZRFURI.getBasename();
    wStr += "::Put the mess";
    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle(wStr.toCChar());
    wTEMWn->registerDisplayColorCallBack(wErrorLog);
    wTEMWn->show();

    wZRF = new ZRandomFile;

    wSt = wZRF->zopen(wZRFURI,ZRF_All);

    wZRF->putTheMess(wErrorLog);

    wZRF->zclose();

    delete wZRF;
    delete wErrorLog;
    return wSt;
}

ZStatus
ZEntryPoint::ZRFCopy( )
{
    ZaiErrors* wErrorLog=new ZaiErrors;
    wErrorLog->setAutoPrintOn(ZAIES_Text);
    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle("Copy file");
    wTEMWn->registerDisplayColorCallBack(wErrorLog);
    wTEMWn->show();

    ZCopyDLg* wCpDLg = new ZCopyDLg (&ErrorLog,this);

    wCpDLg->show();

    delete wErrorLog;
    return ZS_SUCCESS;
} // ZEntryPoint::ZRFCopy

ZStatus
ZEntryPoint::ZRFrebuildHeader()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;
    uriString wCloneURI;
    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Random content file to rebuild"),
                                                     wDir.toCChar(),
                                                     "content files (*.zrf *.zmf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }

    ZaiErrors* wErrorLog=new ZaiErrors;
    wErrorLog->setAutoPrintOn(ZAIES_Text);

    utf8VaryingString wStr = wZRFURI.getBasename();
    wStr += "::Header rebuild";
    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle(wStr.toCChar());
    wTEMWn->registerDisplayColorCallBack(wErrorLog);
    wTEMWn->show();

    wZRF = new ZRandomFile;

    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Reorganize file",this,false);

    ProgressMWn->registerZRFCallBacks(wZRF);

    ProgressMWn->setDescBudy("File");
    ProgressMWn->setDescText(wZRFURI.toCChar());

    ProgressMWn->show();

    wSt=wZRF->zheaderRebuild(wZRFURI,true,wErrorLog);

    ProgressMWn->setDone(wSt);
    wZRF->clearProgressCallBacks();

    delete wZRF;
    delete wErrorLog;
    return wSt;
}

ZStatus
ZEntryPoint::ZRFSurfaceScan()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;

    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Random content file to scan"),
                                                     wDir.toCChar(),
                                                     "content files (*.zrf *.zmf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }

    ZaiErrors* wErrorLog=new ZaiErrors;
    wErrorLog->setAutoPrintOn(ZAIES_Text);

    utf8VaryingString wStr = wZRFURI.getBasename();
    wStr += "::Surface scan";
    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle(wStr.toCChar());
    wTEMWn->registerDisplayColorCallBack(wErrorLog);
    wTEMWn->show();

    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Surface scan",this,false);

    ProgressMWn->setDescBudy("File");
    ProgressMWn->setDescText(wZRFURI.toCChar());

    ProgressMWn->show();

    wSt=ZRandomFile::RawSurfaceScan(wZRFURI,
                                   std::bind(&ZMFProgressMWn::advanceCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2),
                                   std::bind(&ZMFProgressMWn::advanceSetupCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2),
                                   wErrorLog);

    ProgressMWn->setDone((wSt!=ZS_SUCCESS)&&(wSt!=ZS_EOF));

    delete wErrorLog;
    return wSt;
}
void
ZEntryPoint::initLogWindow()
{
    if (TEMWn==nullptr) {
        TEMWn= new textEditMWn(this,TEOP_CloseBtnHide | TEOP_NoFileLab,nullptr);
        TEMWn->setWindowTitle("Content file utilities");
        ErrorLog.setAutoPrintOn(ZAIES_Text);
        ErrorLog.setStoreMinSeverity(ZAIES_Warning);
        TEMWn->registerDisplayColorCallBack(&ErrorLog);
    }
    TEMWn->show();
}


ZStatus
ZEntryPoint::ZMFdowngradetoZRF()
{

    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;

    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Content file to downgrade"),
                                                     wDir.toCChar(),
                                                     "content files (*.zmf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }

    ZaiErrors* wErrorLog=new ZaiErrors;
    wErrorLog->setAutoPrintOn(ZAIES_Text);

    utf8VaryingString wStr = wZRFURI.getBasename();
    wStr += "::Downgrade";
    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle(wStr.toCChar());
    wTEMWn->registerDisplayColorCallBack(wErrorLog);
    wTEMWn->show();

    wTEMWn->appendTextColor(Qt::blue,"Downgrade Master file <%s> to randow file.",
                           wZRFURI.toString());

    wSt= zdowngradeZMFtoZRF(wZRFURI,wErrorLog);

    delete wErrorLog;
    return wSt;
}

ZStatus
ZEntryPoint::ZRFupgradetoZMF()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;

    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Content file to upgrade"),
                                                     wDir.toCChar(),
                                                     "content files (*.zrf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }
    ZaiErrors* wErrorLog=new ZaiErrors;
    wErrorLog->setAutoPrintOn(ZAIES_Text);

    utf8VaryingString wStr = wZRFURI.getBasename();
    wStr += "::Downgrade";
    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle(wStr.toCChar());
    wTEMWn->registerDisplayColorCallBack(wErrorLog);
    wTEMWn->show();

    wTEMWn->appendTextColor(Qt::blue,"Upgrade Random file <%s> to Master file with or without dictionary",
                           wZRFURI.toString());


    uriString wURIdic = ZDictionaryFile::generateDicFileName(wZRFURI);
    int wRet=0;
    if (wURIdic.exists()) {
        ErrorLog.infoLog(" A file with well formed dictionary name exists <%s>.",wURIdic.toString());
        ErrorLog.textLog(" File may potentially be upgraded to master file rather than RAW master file.");

        wRet=ZExceptionDLg::adhocMessage2B("Upgrade to Master file",
                                      Severity_Question,"Raw","Embed",wErrorLog,nullptr,
                                      "A dictionary file <%s> exists.\n"
                                      "Do you intend to set it as embeded dictionary <Embed>\n"
                                      "Or leave master file as Raw master file <Raw>",
                                             wURIdic.toString());
        if (wRet==QDialog::Rejected)
            wURIdic.clear();
        wErrorLog->infoLog(" Dictionary file will be embedded to master file.");
    } // exists
    else {
        wRet=ZExceptionDLg::adhocMessage2B("Upgrade to Master file",
                                             Severity_Question,"Raw","Embed",wErrorLog,nullptr,
                                             "Do you want to search a dictionary file  to embed <Embed>\n"
                                             "Or leave master file as Raw master file <Raw>");
        if (wRet==QDialog::Accepted) {
            QString wFileName = QFileDialog::getOpenFileName(this, tr("Dictionary file"),
                                                             wDir.toCChar(),
                                                             "dictionaries (*.dic *.zmf);;All (*.*)");
            if (wFileName.isEmpty()) {
                wURIdic.clear();
            }
            wURIdic = wFileName.toUtf8().data();
        }
        else {
            wURIdic.clear();
        }

    } // else


/*

    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Upgrade to ZMF",this,ZPTP_TimedClose);
    ProgressMWn->registerZRFCallBacks(wZRF);
    ProgressMWn->setTimeToClose(10000);

    ProgressMWn->setDescBudy("File");
    ProgressMWn->setDescText(wZRFURI.toCChar());

    ProgressMWn->show();
*/

    wSt= zupgradeZRFtoZMF(wZRFURI,wURIdic,wErrorLog);

    delete wErrorLog;
    return wSt;
}

ZStatus
ZEntryPoint::importZMF(QWidget* pParent, ZaiErrors *pErrorLog)
{
  uriString wDir = GeneralParameters.getWorkDirectory();
  uriString wZMFURI;
  ZRawMasterFile *wZMF=nullptr;
  zmode_type wOpenMode=0;
  ZStatus wSt=ZS_SUCCESS;

  QString wFileName = QFileDialog::getOpenFileName(pParent, tr("Target import raw master file"),
                                                     wDir.toCChar(),
                                                     "master files (*.zmf);;All (*.*)");
  if (wFileName.isEmpty())
        return ZS_EMPTY ;
  wZMFURI= wFileName.toUtf8().data();
  if (!wZMFURI.exists()) {

        return ZS_FILENOTEXIST;
  }
  wZMF = new ZRawMasterFile;

  wSt=wZMF->zopen(wZMFURI,ZRF_All);

  if (wZMF->getRecordCount() > 0) {
  int wRet=ZExceptionDLg::adhocMessage2B("Import raw master file",Severity_Question,"Quit","Do it",nullptr,nullptr,
                                            "Processing %s\n"
                                           "Master file has already %ld record(s).\n"
                                           "Populating this file with imported data may cause key collisions and/or incompatibility.\n"
                                           "Do you want to import data ?\n"
                                           "Import anyway               <Do it>\n"
                                           "Quit without doing anything <Quit>\n",
                                           wZMFURI.toString(),
                                           wZMF->getRecordCount());
  if (wRet==QDialog::Rejected)
    return ZS_FILEERROR ;
  }
 // wZMF->zclose();


  wFileName = QFileDialog::getOpenFileName(pParent,"Source import xml content",wDir.toCChar(),"Xml files (*.xml);;All (*.*)");
  if (wFileName.isEmpty())
    return ZS_EMPTY ;
  uriString wURIContentImport=wFileName.toUtf8().data();

  /* export progress window setup */

  ZMFProgressMWn* ProgressMWn = new ZMFProgressMWn("Data import",pParent,ZPTP_HasCloseBtn);

  ProgressMWn->labelsSetup("Export to file",wURIContentImport,"Bytes of text processed");

  ProgressMWn->advanceSetupCallBack(wZMF->getRecordCount(),"Import to file");

  ProgressMWn->show();

  wZMF->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack, ProgressMWn,std::placeholders::_1,std::placeholders::_2));
  wZMF->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack, ProgressMWn,std::placeholders::_1,std::placeholders::_2));

  wSt=wZMF->XmlImportContentByChunk(wURIContentImport,pErrorLog);

  ProgressMWn->setDone(wSt);

  if (wSt!=ZS_SUCCESS) {
    if (pErrorLog==nullptr)
        fprintf(stderr,"Data import: Something went wrong status <%s> while importing data from <%s>",
                decode_ZStatus(wSt),
                wZMF->getURIContent().getBasename().toCChar());
    else
        pErrorLog->errorLog("Data import: Something went wrong status <%s> while importing data from <%s>",
                            decode_ZStatus(wSt),
                            wZMF->getURIContent().getBasename().toCChar());
    utf8VaryingString wMsg = ZException.formatFullUserMessage();
    ZExceptionDLg::adhocMessage("Data import",Severity_Error,pErrorLog,&wMsg,"Data export: Something went wrong status <%s> while importing data from <%s>",
                                decode_ZStatus(wSt),
                                wZMF->getURIContent().getBasename().toCChar());
  }

  wZMF->zclose();
  delete wZMF;
  return wSt;
} // ZEntryPoint::importZMF

