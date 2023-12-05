#include "zrawvisumain.h"

#include <zio/zdir.h>
#include <stdio.h>
#include <fcntl.h>

#include <zcontentcommon/zgeneralparameters.h>

#include <zio/zioutils.h>
#include <ztoolset/utfutils.h>  /* for formatSize  */

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

#include <QCheckBox>


#include <zcontent/zrandomfile/zrandomfile.h>
#include <zcontent/zindexedfile/zrawmasterfile.h>

#include "displaymain.h"
#include "zscan.h"


#include <zexceptiondlg.h>

#include <zcontent/zindexedfile/zmfdictionary.h>

#include <dicedit.h>
#include <qfiledialog.h>

#include <texteditmwn.h>
#include <zqt/zqtwidget/zqtableview.h>

#include <functional> // for std::function and placeholders

#include <QGroupBox>

#include "zrawmasterfilevisu.h"

#include <zcontent/zrandomfile/zrandomfile.h>

#include "visulinecol.h"

#include "poolvisu.h"
#include "filegeneratedlg.h"

#include "zrawkeylistdlg.h"

#include "visuraw.h"

#include "zbackupdlg.h"
#include "zrestoredlg.h"

#include "zsearchquerymwd.h"
#include "zmfprogressmwn.h"

#include <zcontentcommon/zgeneralparameters.h>
#include "zgeneralparamsdlg.h"

#include <ztoolset/zexceptionmin.h>

#include <zclearfiledlg.h>

#include <QApplication>

#include <zio/zioutils.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zcontent/zrandomfile/zrfutilities.h>

const int cst_maxraisonablevalue = 100000;

using namespace std;
using namespace zbs;


DicEditMWn* DicEdit=nullptr;


//ZRawVisuMain::ZRawVisuMain(QWidget *parent) :QMainWindow(parent),
//                                                     ui(new Ui::ZRawVisuMain)
ZRawVisuMain::ZRawVisuMain(QWidget *parent) :QMainWindow(parent)
{
 // setupUi(this);

  initLayout();

  setAttribute(Qt::WA_DeleteOnClose , true);

  GetRawQAc = new QAction(QCoreApplication::translate("ZRawVisuMain", "Open raw file", nullptr),this);

  fileMEn->insertAction(openZMFQAc ,GetRawQAc);

  CurAddressLBl->setText("0");

  setWindowTitle("File dump and explore");

  ZRFVersionLBl->setText(getVersionStr(__ZRF_VERSION__).toCChar());
  ZMFVersionLBl->setText(getVersionStr(__ZMF_VERSION__).toCChar());

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

  VizuRaw = new VisuRaw(VisuTBv,&RawData);

  VisuTBv->newModel(21);

  VisuTBv->setContextMenuCallback(std::bind(&VisuRaw::VisuBvFlexMenuCallback, VizuRaw,placeholders::_1));

  QFont wVisuFont (GeneralParameters.getFixedFont().toCChar());

  VisuTBv->setFont(wVisuFont);

  verticalLayout->addWidget(VisuTBv);

  utf8VaryingString wStr;
  int wj=0;
  int wk=0;
  for (int wi=0;wi < 20;wi++) {
    if (wk==4) {
      wk=0;
      VisuTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(" "));
      continue;
    }
    wStr.sprintf("%d",wj);
    VisuTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(wStr.toCChar()));
    wj++;
    wk++;
  }
//  for (int wi=0;wi < 20;wi++)
//    VisuTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(tr(" ")));
  VisuTBv->ItemModel->setHorizontalHeaderItem(20,new QStandardItem(tr("Ascii")));

  openZRFQAc = new QAction("Random file",this);
  menubar->addAction(openZRFQAc);

  RandomFileMEn = new QMenu("Random file",this);
  menubar->addMenu(RandomFileMEn);

  ZRFCloneQAc = new QAction("Clone",this);
  RandomFileMEn->addAction(ZRFCloneQAc);
  ZRFClearQAc = new QAction("Clear",this);
  RandomFileMEn->addAction(ZRFClearQAc);
  ZRFReorgQAc = new QAction("Reorganize",this);
  RandomFileMEn->addAction(ZRFReorgQAc);
  ZRFRebuildHeaderQAc = new QAction("Rebuild header",this);
  RandomFileMEn->addAction(ZRFRebuildHeaderQAc);
  ZRFSurfaceScanQAc = new QAction("Surface scan",this);
  RandomFileMEn->addAction(ZRFSurfaceScanQAc);

  MasterFileMEn = new QMenu("Master file",this);
  menubar->addMenu(MasterFileMEn);

  DictionaryQAc=new QAction("Dictionaries management",this);
  MasterFileMEn->addAction(DictionaryQAc);

  ZMFQueryQAc = new QAction("Query",this);
  MasterFileMEn->addAction(ZMFQueryQAc);

  ZmfDefQAc = new QAction("Master file definition",this);
  MasterFileMEn->addAction(ZmfDefQAc);

  IndexRebuildQAc = new QAction("Rebuild index",this);
  MasterFileMEn->addAction(IndexRebuildQAc);

  ZMFExportQAc = new QAction("Export data",this);
  MasterFileMEn->addAction(ZMFExportQAc);

  ZMFImportQAc = new QAction("Import data",this);
  MasterFileMEn->addAction(ZMFImportQAc);

  ZMFClearQAc = new QAction("Clear all file data",this);
  MasterFileMEn->addAction(ZMFClearQAc);

  ZMFBackupQAc = new QAction("Backup file and all indexes",this);
  MasterFileMEn->addAction(ZMFBackupQAc);

  ZMFRestoreQAc = new QAction("Restore file and all indexes",this);
  MasterFileMEn->addAction(ZMFRestoreQAc);

  mainQAg = new QActionGroup(this);

  mainQAg->addAction(DictionaryQAc);
  mainQAg->addAction(openZRFQAc);

  mainQAg->addAction(ZMFQueryQAc);
  mainQAg->addAction(ZMFExportQAc);
  mainQAg->addAction(ZMFImportQAc);

  mainQAg->addAction(setfileQAc); /* set the current file to be open */

  mainQAg->addAction(openByTypeQAc);  /* open according file extension */

  mainQAg->addAction(openRawQAc);/* force open as raw file */
  mainQAg->addAction(openZRFQAc);/* force open as ZRF file */
  mainQAg->addAction(openZMFQAc);/* force open as ZMF file */
  mainQAg->addAction(openZRHQAc);/* force open as header file */

  mainQAg->addAction(GetRawQAc);     /* Shortcut to open file as raw */

  mainQAg->addAction(surfaceScanZRFQAc); /* surface scan ZRF file */
  mainQAg->addAction(surfaceScanRawQAc); /* surface scan raw file */

  mainQAg->addAction(closeQAc);

  mainQAg->addAction(QuitQAc);

  mainQAg->addAction(displayHCBQAc);
  mainQAg->addAction(displayFCBQAc);
  mainQAg->addAction(displayMCBQAc);

  mainQAg->addAction(rawDumpQAc);

  displayICBQAc = new QAction(QObject::tr("Index control blocks","ZRawVisuMain"),this);
  mainQAg->addAction(displayICBQAc);

  headerMEn->insertAction(displayMCBQAc,displayICBQAc);

  mainQAg->addAction(displayICBQAc);

  mainQAg->addAction(dictionaryQAc);
//  actionGroup->addAction(dicLoadXmlQAc);

  ParametersQMe=new QMenu(QObject::tr("Parameters"),this);
  fileMEn->insertMenu(setfileQAc ,ParametersQMe);

  ParamLoadQAc=new QAction(QObject::tr("Load xml parameter file"),this);
  ParamChangeQAc=new QAction(QObject::tr("Change current parameters"),this);
  ParametersQMe->addAction(ParamLoadQAc);
  ParametersQMe->addAction(ParamChangeQAc);

  mainQAg->addAction(ParamLoadQAc);
  mainQAg->addAction(ParamChangeQAc);

  /* Master file menus */

  mainQAg->addAction(ZmfDefQAc);
  mainQAg->addAction(IndexRebuildQAc);
  mainQAg->addAction(ZMFBackupQAc);
  mainQAg->addAction(ZMFRestoreQAc);
  mainQAg->addAction(ZMFClearQAc);

  /* display pool choices */

  mainQAg->addAction(displayZBATQAc);
  mainQAg->addAction(displayZDBTQAc);
  mainQAg->addAction(displayZFBTQAc);

//  mainQAg->addAction(HeaderRawUnlockQAc);

  mainQAg->addAction(unlockZRFQAc);

  mainQAg->addAction(clearQAc);
  mainQAg->addAction(cloneQAc);
  mainQAg->addAction(extendQAc);
  mainQAg->addAction(truncateQAc);
  mainQAg->addAction(reorganizeQAc);
  mainQAg->addAction(upgradeZRFtoZMFQAc);
  mainQAg->addAction(rebuildHeaderQAc);

/* ZMFMEn */
  mainQAg->addAction(runRepairQAc); /* repair all indexes */
  mainQAg->addAction(removeIndexQAc);
  mainQAg->addAction(addIndexQAc);      /* adds an index from an xml definition */
  mainQAg->addAction(downgradeZMFtoZRFQAc);
  mainQAg->addAction(rebuildIndexQAc);      /* rebuild single index */
  mainQAg->addAction(extractIndexQAc);      /* extract xml index definition */
  mainQAg->addAction(extractAllIndexesQAc); /* extract all xml index definitions */
  mainQAg->addAction(reorganizeZMFQAc);
  mainQAg->addAction(MCBReportQAc);         /* list Master Control Block */


  mainQAg->addAction(ZRFCloneQAc);
  mainQAg->addAction(ZRFClearQAc);
  mainQAg->addAction(ZRFReorgQAc);
  mainQAg->addAction(ZRFRebuildHeaderQAc);
  mainQAg->addAction(ZRFSurfaceScanQAc);

  openByTypeQAc->setVisible(true);  /* open file by type is always visible */

  openRawQAc->setVisible(false);
  openZRFQAc->setVisible(false);
  openZMFQAc->setVisible(false);
  openZRHQAc->setVisible(false);
  unlockZRFQAc->setVisible(false);
  closeQAc->setVisible(false);

  rawMEn->setEnabled(false);
  ZRFMEn->setEnabled(false);
  headerMEn->setEnabled(false);
  ZMFMEn->setEnabled(false);

  RecordFRm->setVisible(false);
  SequentialFRm->setVisible(false);

  displayHCBQAc->setEnabled(false);
  displayMCBQAc->setEnabled(false);
  displayICBQAc->setEnabled(false);
  displayFCBQAc->setEnabled(false);


  BackwardBTn->setVisible(false);
  ForwardBTn->setVisible(false);
  LoadAllBTn->setVisible(false);

  LoadAllBTn->setText(QObject::tr("All","ZRawVisuMain"));

  ProgressPGb->setVisible(false);

  OpenModeLbl->setVisible(false);
//  ClosedLBl->setVisible(true);

  searchTypeCBx->addItem("Ascii case sensitive");       /* 0 */
  searchTypeCBx->addItem("Ascii no case");              /* 1 */
  searchTypeCBx->addItem("Hexa free sequence");         /* 2 */
  searchTypeCBx->addItem("F9F9F9F9 File block start");  /* 3 */
  searchTypeCBx->addItem("F5F5F5F5 Data block start");  /* 4 */
  searchTypeCBx->addItem("F4F4F4F4 Dictionary field start (file)");  /* 5 */
  searchTypeCBx->addItem("FCFCFCFC Data block end");  /* 6 */
  searchTypeCBx->addItem("F6F6F6F6 Dictionary structure begin(file)");  /* 7 */
  searchTypeCBx->addItem("FAFAFAFA String data begin (memory)");  /* 8 */
  searchTypeCBx->addItem("FBFBFBFB Buffer end");  /* 9 */

  QObject::connect(BackwardBTn, &QPushButton::clicked, this, &ZRawVisuMain::backward);
  QObject::connect(ForwardBTn, &QPushButton::clicked, this, &ZRawVisuMain::forward);
  QObject::connect(LoadAllBTn, &QPushButton::clicked, this, &ZRawVisuMain::loadAll);

 // QObject::connect(mainQAg, SIGNAL(triggered(QAction*)), this, SLOT(actionMenuEvent(QAction*)));
  QObject::connect(mainQAg, &QActionGroup::triggered, this, &ZRawVisuMain::actionMenuEvent);
  QObject::connect(searchFwdBTn, &QPushButton::clicked, this, &ZRawVisuMain::searchFwd);
  QObject::connect(searchBckBTn, &QPushButton::clicked, this, &ZRawVisuMain::searchBck);
  QObject::connect(searchTypeCBx, &QComboBox::currentIndexChanged, this, &ZRawVisuMain::searchCBxChanged);
  QObject::connect(VisuTBv, &ZQTableView::doubleClicked, this, &ZRawVisuMain::VisuClicked);
  QObject::connect(VisuTBv, &ZQTableView::clicked, this, &ZRawVisuMain::VisuClicked);

  QObject::connect(ReadSizeLBl, &QLineEdit::textChanged, this, &ZRawVisuMain::UserOffsetChanged);
  QObject::connect(ViewHexaCHk, &QCheckBox::toggled, this, &ZRawVisuMain::ViewHexaChanged);


 // ErrorLog.setDisplayCallback(std::bind(&ZRawVisuMain::GenlogWindisplayErrorCallBack, this,std::placeholders::_1));
  ErrorLog.setDisplayColorCallBack(std::bind(&ZRawVisuMain::GenlogWindisplayErrorColorCB, this,placeholders::_1,placeholders::_2));

}
ZRawVisuMain::~ZRawVisuMain()
{
    if (VizuRaw!=nullptr)
        delete VizuRaw;

    if (entityWnd)
        delete entityWnd;

    if (RandomFile)
        if (RandomFile->isOpen())
        {
            RandomFile->zclose();
            delete RandomFile;
        }
    if (RawMasterFile)
    {
        if (RawMasterFile->isOpen())
            RawMasterFile->zclose();
        delete RawMasterFile;
    }
    if (Fd>=0)
        ::close(Fd);

//    delete ui;
}
void
ZRawVisuMain::initLayout()
{
    resize(868, 676);
    openByTypeQAc = new QAction("Open file according type",this);
    rawQAc = new QAction("Raw",this);
    ZRFRecordsQac = new QAction("ZRF Records",this);
    actionSuface = new QAction("Suface",this);
    surfaceScanRawQAc = new QAction("Surface scan",this);
    RecoveryQAc = new QAction("Record recovery",this);
    closeQAc = new QAction("Close current", this);
    rawDumpQAc = new QAction("Raw dump",this);
    surfaceScanZRFQAc = new QAction("Surface dump (ZRF)",this);
    displayHCBQAc = new QAction("Header Control Block", this);
    QuitQAc = new QAction("Quit",this);

    displayFCBQAc = new QAction("File Control Block",this);
    displayMCBQAc = new QAction("Master Control Block",this);
    setfileQAc = new QAction("Set file",this);
    openRawQAc = new QAction("Open file as raw file",this);
    openZRFQAc = new QAction("Open file as Random File",this);
    openZMFQAc = new QAction("Open file as Master File",this);
    actionHeader_file = new QAction("Header file", this);
    unlockZRFQAc = new QAction("Unlock random file",this);
    headerFileUnlockQAc = new QAction("Header file unlock",this);

    listIndexesQAc = new QAction("List indexes",this);
    clearQAc = new QAction("Clear file",this);
    cloneQAc = new QAction( "Clone file",this);
    truncateQAc = new QAction("Truncate file",this);
    extendQAc = new QAction("Extend File",this);
    rebuildHeaderQAc = new QAction("Rebuild header file",this);
    reorganizeQAc = new QAction("Reorganize file",this);
    downgradeZMFtoZRFQAc = new QAction("dowgrade to ZRF",this);
    upgradeZRFtoZMFQAc = new QAction("upgrade to MasterFile",this);
    reorganizeZMFQAc = new QAction("reorganize",this);
    removeIndexQAc = new QAction("remove Index",this);
    addIndexQAc = new QAction("add Index",this);
    rebuildIndexQAc = new QAction("rebuild single Index",this);
    extractIndexQAc = new QAction("extract Index definition",this);
    extractAllIndexesQAc = new QAction("extract all Indexes definitions", this);
    testRunQAc = new QAction("test run",this);
    testRunQAc->setCheckable(true);
    rebuilAllQAc = new QAction("rebuild all",this);
    rebuilAllQAc->setCheckable(true);
    runRepairQAc = new QAction("run repair", this);
    MCBReportQAc = new QAction("Master Control Block report",this);
    DictionaryQAc = new QAction( "Dictionary",this);
    displayZBATQAc = new QAction("Block Access Table",this);
    displayZDBTQAc = new QAction("Deleted Blocks Table",this);
    displayZFBTQAc = new QAction("Free Blocks Table",this);
    dictionaryQAc = new QAction("Dictionary facilities", this);
    dicLoadXmlQAc = new QAction( "Load from Xml file",this);
    cppparserQAc = new QAction( "c++ parser",this);
    openZRHQAc = new QAction("Open file as header file",this);
    centralwidget = new QWidget(this);
    MainFRm = new QFrame(centralwidget);
    MainFRm->setGeometry(QRect(0, 0, 861, 211));
    MainFRm->setFrameShape(QFrame::StyledPanel);
    MainFRm->setFrameShadow(QFrame::Raised);
    OpenModeLbl = new QLabel("not open", MainFRm);
    OpenModeLbl->setGeometry(QRect(140, 40, 131, 16));
    FileTypeLBl = new QLabel("Unknown type",MainFRm);
    FileTypeLBl->setGeometry(QRect(140, 80, 131, 16));
    FullPathLbl = new QLabel("no file selected",MainFRm);
    FullPathLbl->setGeometry(QRect(20, 10, 831, 16));
    FullPathLbl->setFrameShape(QFrame::NoFrame);
    FullPathLbl->setFrameShadow(QFrame::Plain);
    BackwardBTn = new QPushButton("<",MainFRm);
    BackwardBTn->setGeometry(QRect(320, 47, 41, 41));
    ForwardBTn = new QPushButton(">",MainFRm);
    ForwardBTn->setGeometry(QRect(810, 47, 41, 41));
    RecordFRm = new QFrame(MainFRm);
    RecordFRm->setGeometry(QRect(370, 37, 421, 71));
    RecordFRm->setFrameShape(QFrame::StyledPanel);
    RecordFRm->setFrameShadow(QFrame::Raised);
    RecordNumberLBl = new QLabel("Record",RecordFRm);
    RecordNumberLBl->setGeometry(QRect(10, 10, 61, 20));
    QLabel * label_2 = new QLabel("Address",RecordFRm);
    label_2->setGeometry(QRect(220, 10, 61, 16));
    AddressLBl = new QLabel("0",RecordFRm);
    AddressLBl->setGeometry(QRect(310, 10, 101, 20));
    QLabel * label_10 = new QLabel("Block Size", RecordFRm);
    label_10->setGeometry(QRect(10, 30, 71, 16));
    BlockSizeLBl = new QLabel("Block Size",RecordFRm);
    BlockSizeLBl->setGeometry(QRect(80, 30, 81, 16));
    QLabel * label_11 = new QLabel("User Size",RecordFRm);
    label_11->setGeometry(QRect(220, 30, 54, 15));
    UserSizeLBl = new QLabel("User Size",RecordFRm);
    UserSizeLBl->setGeometry(QRect(310, 30, 101, 20));
    QLabel * label_12 = new QLabel("State",RecordFRm);
    label_12->setGeometry(QRect(10, 50, 54, 15));
    StateLBl = new QLabel("State",RecordFRm);
    StateLBl->setGeometry(QRect(80, 50, 81, 16));
    QLabel *label_13 = new QLabel("Lock mask",RecordFRm);
    label_13->setGeometry(QRect(220, 50, 61, 16));
    LockMaskLBl = new QLabel("lock",RecordFRm);
    LockMaskLBl->setGeometry(QRect(300, 50, 121, 16));
    RecordTotalLBl = new QLabel("Record total",RecordFRm);
    RecordTotalLBl->setGeometry(QRect(140, 10, 61, 20));
    QLabel *label = new QLabel("of",RecordFRm);
    label->setGeometry(QRect(100, 10, 21, 16));
    FileSizeLBl = new QLabel("File Size",MainFRm);
    FileSizeLBl->setGeometry(QRect(80, 60, 131, 16));
    QLabel *label_3 = new QLabel("File size", MainFRm);
    label_3->setGeometry(QRect(20, 60, 91, 16));
    LoadAllBTn = new QPushButton("All",MainFRm);
    LoadAllBTn->setGeometry(QRect(800, 97, 51, 41));
    ProgressPGb = new QProgressBar(MainFRm);
    ProgressPGb->setGeometry(QRect(20, 100, 191, 20));
    QFont font;
    font.setPointSize(6);
    font.setItalic(true);
    ProgressPGb->setFont(font);
    ProgressPGb->setValue(24);
    SequentialFRm = new QFrame(MainFRm);
    SequentialFRm->setGeometry(QRect(370, 170, 421, 31));
    SequentialFRm->setFrameShape(QFrame::StyledPanel);
    SequentialFRm->setFrameShadow(QFrame::Raised);
    QLabel *label_6 = new QLabel("Offset", SequentialFRm);
    label_6->setGeometry(QRect(10, 10, 41, 16));

    ReadSizeLBl = new QLineEdit(SequentialFRm);
    ReadSizeLBl->setGeometry(QRect(80, 10, 81, 16));


    InterPunctLBl = new QLabel("of",SequentialFRm);
    InterPunctLBl->setGeometry(QRect(170, 10, 21, 16));
    FullSizeLBl = new QLabel("Full Size",SequentialFRm);
    FullSizeLBl->setGeometry(QRect(210, 10, 81, 16));
    //QLabel * label_00 = new QLabel("File is open as",MainFRm);
    //label_00->setGeometry(QRect(20, 40, 91, 16));
    QLabel * label_1 = new QLabel( "File has type",MainFRm);
    label_1->setGeometry(QRect(20, 80, 91, 16));
    ZRFVersionLBl = new QLabel("ZRF Version",MainFRm);
    ZRFVersionLBl->setGeometry(QRect(240, 170, 101, 16));
    ZMFVersionLBl = new QLabel("ZMF Version",MainFRm);
    ZMFVersionLBl->setGeometry(QRect(240, 190, 101, 16));
    QLabel * label_4 = new QLabel("Random File Software version",MainFRm);
    label_4->setGeometry(QRect(20, 170, 211, 16));
    QLabel * label_5 = new QLabel("Master File Software version",MainFRm);
    label_5->setGeometry(QRect(20, 190, 211, 16));
//    ClosedLBl = new QLabel("No Open File", MainFRm);
//    ClosedLBl->setGeometry(QRect(130, 30, 151, 31));
//    QFont font1;
//    font1.setPointSize(12);
//    font1.setBold(true);
//    ClosedLBl->setFont(font1);
    searchLEd = new QLineEdit(MainFRm);
    searchLEd->setGeometry(QRect(530, 110, 261, 23));
    searchTypeCBx = new QComboBox(MainFRm);
    searchTypeCBx->setGeometry(QRect(380, 110, 141, 23));
    searchTypeCBx->setMinimumContentsLength(0);
    searchFwdBTn = new QPushButton("search forward",MainFRm);
    searchFwdBTn->setGeometry(QRect(240, 110, 121, 23));
    QLabel *label_8 = new QLabel("Address", MainFRm);
    label_8->setGeometry(QRect(390, 140, 61, 16));
    CurAddressLBl = new QLabel("Cur Address",MainFRm);
    CurAddressLBl->setGeometry(QRect(480, 140, 101, 20));


    ViewHexaCHk=new QCheckBox("Address hexa",MainFRm);
    ViewHexaCHk->setGeometry(QRect(590, 140, 101, 20));

    searchBckBTn = new QPushButton("search backward",MainFRm);
    searchBckBTn->setGeometry(QRect(240, 140, 121, 23));
    verticalLayoutWidget = new QWidget(centralwidget);
    verticalLayoutWidget->setGeometry(QRect(0, 220, 861, 381));
    verticalLayout = new QVBoxLayout(verticalLayoutWidget);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(centralwidget);
    menubar = new QMenuBar(this);
    menubar->setGeometry(QRect(0, 0, 868, 20));
    fileMEn = new QMenu("File", menubar);
    ZRFMEn = new QMenu("ZRF",menubar);
    ZMFMEn = new QMenu("ZMF",menubar);
    menurepair_all_indexes = new QMenu("repair all indexes",ZMFMEn);
    rawMEn = new QMenu("Raw file",menubar);
    headerMEn = new QMenu("Header file",menubar);
    menuView_pool = new QMenu("View pool", headerMEn);
    setMenuBar(menubar);
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);
    /*
    toolBar = new QToolBar(this);
    toolBar->setObjectName("toolBar");
    addToolBar(Qt::TopToolBarArea, toolBar);
    */
    menubar->addAction(fileMEn->menuAction());
    menubar->addAction(ZRFMEn->menuAction());
    menubar->addAction(ZMFMEn->menuAction());
    menubar->addAction(rawMEn->menuAction());
    menubar->addAction(headerMEn->menuAction());
    fileMEn->addAction(setfileQAc);
    fileMEn->addAction(openByTypeQAc);
    fileMEn->addAction(openRawQAc);
    fileMEn->addAction(openZRFQAc);
    fileMEn->addAction(openZMFQAc);
    fileMEn->addAction(openZRHQAc);
    fileMEn->addAction(unlockZRFQAc);
    fileMEn->addAction(closeQAc);
    fileMEn->addAction(QuitQAc);
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
    menurepair_all_indexes->addAction(testRunQAc);
    menurepair_all_indexes->addAction(rebuilAllQAc);
    menurepair_all_indexes->addAction(runRepairQAc);
    rawMEn->addAction(rawDumpQAc);
    rawMEn->addAction(surfaceScanRawQAc);
    headerMEn->addAction(headerFileUnlockQAc);
    headerMEn->addAction(displayFCBQAc);
    headerMEn->addAction(displayHCBQAc);
    headerMEn->addAction(displayMCBQAc);
    headerMEn->addAction(menuView_pool->menuAction());
    menuView_pool->addAction(displayZBATQAc);
    menuView_pool->addAction(displayZFBTQAc);
//    toolBar->addAction(HeaderRawUnlockQAc);



    searchTypeCBx->setCurrentText(QString());

} // initLayout

ZStatus ZRawVisuMain::setUpRawFile(const uriString& pFile)
{
    URICurrent = pFile;
    setFileDescription (URICurrent);

    VisuType = ZCVM_Raw;

    return displayRawFromOffset(PayLoad);

} // setup


void ZRawVisuMain::VisuMouseCallback(int pZEF, QMouseEvent *pEvent)
{
  /* update offset */
  QModelIndex wIdx;
  if (!wIdx.isValid())
    return;
  ssize_t wOffset=computeOffsetFromCoord(wIdx.row(),wIdx.column());
  SearchOffset=wOffset;
  utf8VaryingString wStr;
  wStr.sprintf("%ld",wOffset);
  CurAddressLBl->setText(wStr.toCChar());
}//VisuMouseCallback



void ZRawVisuMain::VisuClicked(QModelIndex pIdx) {
  setSearchOffset(computeOffsetFromCoord(pIdx.row(),pIdx.column()));

}

void ZRawVisuMain::setSearchOffset(ssize_t pOffset) {
  utf8VaryingString wStr;
  SearchOffset=pOffset;
  wStr.sprintf("%ld",SearchOffset);
  CurAddressLBl->setText(wStr.toCChar());
}

void ZRawVisuMain::UserOffsetChanged(const QString & pText)
{
    if (UserOffsetDoNotChange) {
        UserOffsetDoNotChange=false;
        return;
    }
    utf8VaryingString wValueStr = pText.toUtf8().data();

    size_t wValue = 0 ;
    if (UseHexa)
        wValue = wValueStr.toLong(16);
    else
        wValue = wValueStr.toLong(10);

    if (wValue > FileSize)
        wValue = FileSize - PayLoad ;
    if (wValue < 0)  {
        wValue=0;
    }
    FileOffset = wValue;
    displayRawFromOffset(PayLoad);
}//UserOffsetChanged


void ZRawVisuMain::ViewHexaChanged(bool pToggled)
{
    utf8VaryingString wStr;
    UseHexa = pToggled;
    FileOffset -= PayLoad;
    if (pToggled)
        wStr.sprintf("0x%X",FileOffset);
    else
        wStr.sprintf("%ld",FileOffset);

    UserOffsetDoNotChange = true;
    ReadSizeLBl->setText(wStr.toCChar());
    switch (VisuType)
    {
    case ZCVM_Raw:
        displayRawFromOffset(PayLoad);
        break;
    case ZCVM_RandomFile:
        displayListZRFNextRecord();
        break;
    default:
        break;
    }
}
void ZRawVisuMain::searchCBxChanged(int pIndex){
  switch(pIndex) {
  case 0:
  case 1:
  case 2:
    SearchOffset=-1;
    return ;
  case 3:
    searchLEd->clear();
    searchLEd->setText("F9F9F9F9");
    SearchOffset=-1;
    return;
  case 4:
    searchLEd->clear();
    searchLEd->setText("F5F5F5F5");
    SearchOffset=-1;
    return;
  case 5:
    searchLEd->clear();
    searchLEd->setText("F4F4F4F4");
    SearchOffset=-1;
    return;
  case 6:
    searchLEd->clear();
    searchLEd->setText("FCFCFCFC");
    SearchOffset=-1;
    return;
  case 7:
    searchLEd->clear();
    searchLEd->setText("F6F6F6F6");
    SearchOffset=-1;
    return;
  case 8:
    searchLEd->clear();
    searchLEd->setText("FAFAFAFA");
    SearchOffset=-1;
    return;

  case 9:
    searchLEd->clear();
    searchLEd->setText("FBFBFBFB");
    SearchOffset=-1;
    return;
  }
  return;
}

void ZRawVisuMain::searchFwd() {
  utf8VaryingString wStr;

  if (SearchOffset<0)
    SearchOffset=0;

  SelectedBackGround = QVariant(QBrush(Qt::cyan));
  /* analyzis of string to search */

  switch (searchTypeCBx->currentIndex()) {
  case 0:
    if (searchAscii(false,false)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      CurAddressLBl->setText(wStr.toCChar());
    }
    else
      CurAddressLBl->setText("--");
    return;
  case 1:
    if (searchAscii(true,false)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      CurAddressLBl->setText(wStr.toCChar());
    }
    else
      CurAddressLBl->setText("--");
    return;
  default:
    if (searchHexa(false)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      CurAddressLBl->setText(wStr.toCChar());
    }
    else
      CurAddressLBl->setText("--");
    return;
  }
}// searchFwd

void ZRawVisuMain::searchBck() {
  utf8VaryingString wStr;

  if (SearchOffset<0)
    SearchOffset=0;

  SelectedBackGround = QVariant(QBrush(Qt::cyan));
  /* analyzis of string to search */

  switch (searchTypeCBx->currentIndex()) {
  case 0:
    if (searchAscii(false,true)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      CurAddressLBl->setText(wStr.toCChar());
    }
    else
      CurAddressLBl->setText("--");
    return;
  case 1:
    if (searchAscii(true,true)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      CurAddressLBl->setText(wStr.toCChar());
    }
    else
      CurAddressLBl->setText("--");
    return;
  default:
    if (searchHexa(true)) {
      wStr.sprintf("%ld",SearchOffset-SearchContent.Size);
      CurAddressLBl->setText(wStr.toCChar());
    }
    else
      CurAddressLBl->setText("--");
    return;
  }
}



void
ZRawVisuMain::setSelectionBackGround(QVariant& pBackground,ssize_t pOffset,size_t pSize,bool pScrollTo) {
  VisuLineCol wVLC;
  wVLC.compute(pOffset);
  int wStartLine = wVLC.line;
  int wStartColumn = wVLC.col;

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

} // ZRawVisuMain::setSelectionBackGround


bool ZRawVisuMain::searchHexa(bool pReverse) {

  utf8VaryingString wSearchStr=searchLEd->text().toUtf8().data();

  /* must be hexa digit*/
  SearchContent.allocateBZero(wSearchStr.strlen() / 2);

  utf8_t* wChar = wSearchStr.Data;
  uint8_t wHexaValue=0;
  long wOffset=0;
  while (*wChar) {
    if (getHexaDigits(wChar,wHexaValue)) {
      ZExceptionDLg::adhocMessage("Invalid hexadecimal string",Severity_Error,nullptr,&wSearchStr,"Invalid hexadecimal string. Cannot convert.");
      return false;
    }
    SearchContent[wOffset++]=wHexaValue;
  }// while

  if (pReverse) {
    if (SearchOffset==0) {
      ZExceptionDLg::adhocMessage("Search",Severity_Error,nullptr,&wSearchStr,"Already at beginning.");
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
    ZExceptionDLg::adhocMessage("Search",Severity_Error,nullptr,&wSearchStr,"Value not found.");
    return false;
  }

  setSelectionBackGround(SelectedBackGround,SearchOffset,SearchContent.Size,true);

  SearchOffset += SearchContent.Size;

  return true;
//  model.setData(model.index(0, 0), QVariant(QBrush(Qt::red)), Qt::BackgroundRole);
}//ZRawVisuMain::searchHexa

bool ZRawVisuMain::searchAscii(bool pCaseRegardless, bool pReverse) {
  utf8VaryingString wSearchStr=searchLEd->text().toUtf8().data();
  if (pReverse) {
    if (SearchOffset==0) {
    ZExceptionDLg::adhocMessage("Search",Severity_Error,nullptr,&wSearchStr,"Already at beginning.");
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
    ZExceptionDLg::adhocMessage("Search",Severity_Error,nullptr,&wSearchStr,"Value not found.");
    return false;
  }

  setSelectionBackGround(SelectedBackGround,SearchOffset,SearchContent.Size);

  SearchOffset += SearchContent.Size;

  return true;
}//ZRawVisuMain::searchAscii


void ZRawVisuMain::resizeEvent(QResizeEvent* pEvent)
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

  verticalLayoutWidget->resize(wVW,wVH);  /* expands in width and height */

}//ZRawVisuMain::resizeEvent

void
ZRawVisuMain::actionMenuEvent(QAction* pAction)
{
  utf8VaryingString wStr;
  ZStatus wSt=ZS_SUCCESS;


  if (pAction==openByTypeQAc)
  {
    actionOpenFileByType();

    return;
  }
  if (pAction==openRawQAc) {
    openRaw();
    OpenModeLbl->setText("Raw file");
    return;
  }
  if (pAction==openZRFQAc) {
    openZRF();
    OpenModeLbl->setText("Random file");
    return;
  }
  if (pAction==rawDumpQAc) {
    ZRawMasterFileVisu* wVisu= new ZRawMasterFileVisu(this);
//    wVisu->setup(URICurrent, Fd);
    wVisu->setup(URICurrent);
    wVisu->setModal(false);
    wVisu->firstIterate();
    wVisu->show();
    return;
  }
  if (pAction==GetRawQAc)
  {
    getRaw();
    OpenModeLbl->setText("Raw file");
    return;
  }
  if (pAction==openZRFQAc) {
    wSt=openZRF();
    if ((wSt==ZS_SUCCESS)||(wSt==ZS_FILETYPEWARN))
    {

      removeAllRows();
//      displayListZRFFirstRecord(Width);
      return;
    }
    OpenModeLbl->setText("Random file");
    return;
  }
  if (pAction==openZMFQAc)
  {
    OpenMode = VMD_Master;
    removeAllRows();
    wSt=openZMF(URICurrent.toCChar());
    OpenModeLbl->setText("Master file");
    return;
  }
  if (pAction==openZRHQAc)
  {
    OpenMode = VMD_Header;
    removeAllRows();
    wSt=openZRH(URICurrent.toCChar());
    OpenModeLbl->setText("Header file");
    return;
  }
  if (pAction==unlockZRFQAc)
  {
    unlockZRFZMF(URICurrent.toCChar());
    return;
  }

  if (pAction==setfileQAc)
  {

    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, tr("Open File"),
        wDir.toCChar(),
        "ZContent files (*.zrf *.zmf *.zix *.zrh);;All (*.*)");
    if (wFileName.isEmpty()) {
      QMessageBox::critical(this,tr("No file selected"),"Please select a valid file");
      return;
    }
    URICurrent = wFileName.toUtf8().data();
    FullPathLbl->setText(wFileName);

    setFileType (wFileName.toUtf8().data());
//    chooseFile(true);
    return;
  }

  if (pAction==ZRFRecordsQac)
  {
    OpenModeLbl->setText("By record");
    BackwardBTn->setVisible(true);
    ForwardBTn->setVisible(true);
    return;
  }
  if (pAction==unlockZRFQAc)
  {
    ZRFUnlock();
    return;
  }
/*
  if (pAction==HeaderRawUnlockQAc)
  {
    ZHeaderRawUnlock();
    return;
  }
*/
  if (pAction==closeQAc)
  {
    OpenMode = VMD_Nothing;
    BackwardBTn->setVisible(false);
    ForwardBTn->setVisible(false);
    actionClose();
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

  /* general parameters */

  if (pAction==ParamLoadQAc) {
    uriString wDir = GeneralParameters.getWorkDirectory();
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


  if (pAction==displayZFBTQAc)
  {
    displayZFBT();
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
//      reorganizeZRF();
      return;
  }
  if (pAction==ZRFRebuildHeaderQAc)
  {
//      rebuildHeaderZRF();
      return;
  }
  if (pAction==ZRFSurfaceScanQAc)
  {
 //     rebuildHeaderZRF();
      return;
  }

  if (pAction==DictionaryQAc)
  {
    if (DicEdit==nullptr)
      DicEdit=new DicEditMWn(std::bind(&ZRawVisuMain::DicEditQuitCallback, this),this);

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

    wSt=exportZMF(this,wZMF, nullptr);
    if (wHasBeenOpened)
        wZMF->zclose();
    if (wHasBeenCreated)
        delete wZMF;
    return;
  }//ZMFExportQAc

  if (pAction==ZMFImportQAc) {
    openGenLogWin();
    GenlogWin->setWindowTitle("Import process log");
    GenlogWin->show();

    wSt=importZMF(this, &ErrorLog);
/*
    if (wHasBeenOpened)
        wZMF->zclose();
    if (wHasBeenCreated)
        delete wZMF;
*/
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
    clearZMF(this,&ErrorLog);
  } //ZMFClearQAc


  if (pAction==ZRFSurfaceScanQAc) {
    surfaceScanZRF();
    return;
  }
  if (pAction==surfaceScanRawQAc)
  {
    displayRawSurfaceScan(URICurrent);
    return;
  }

  if (pAction==QuitQAc)
  {
 //   actionClose();
    close();
//    this->deleteLater();
//    QApplication::quit();
    return;
  }


  /* ZMFMEn */

  if (pAction==runRepairQAc) {
    repairIndexes(testRunQAc->isChecked(), rebuilAllQAc->isChecked());
    return;
  }

  if (pAction==MCBReportQAc) {
    reportMCB();
    return;
  }

  return;

}//actionMenuEvent

void ZRawVisuMain::reportMCB()
{
    if (MCBWin==nullptr) {
        MCBWin= new textEditMWn(this,TEOP_CloseBtnHide | TEOP_NoFileLab,nullptr);
        MCBWin->setWindowTitle("Master Control Block");
        ErrorLog.setAutoPrintOn(ZAIES_Text);
        ErrorLog.setStoreMinSeverity(ZAIES_Warning);
        MCBWin->registerDisplayColorCallBack(&ErrorLog);
    }
    MCBWin->show();
//  FILE* wReportLog = fopen("reportMCB.log","w");
  RawMasterFile->report(&ErrorLog);
/*  fflush(wReportLog);
  fclose(wReportLog);

  if (MCBWin==nullptr)
    MCBWin=new textEditMWn(this,false,nullptr);
//    MCBWin=new textEditMWn(this,nullptr);
  else
    MCBWin->clear();

  MCBWin->registerCloseCallback(std::bind(&ZRawVisuMain::closeMCBCB, this,std::placeholders::_1));
  MCBWin->registerMoreCallback(std::bind(&ZRawVisuMain::textEditMorePressed, this));

  MCBWin->setTextFromFile("reportMCB.log");
  MCBWin->show();
*/
}
void
ZRawVisuMain::initLogWindow()
{
    if (GenlogWin==nullptr) {
        GenlogWin= new textEditMWn(this,TEOP_CloseBtnHide | TEOP_NoFileLab,nullptr);
        GenlogWin->setWindowTitle("Master file visu");
        ErrorLog.setAutoPrintOn(ZAIES_Text);
        ErrorLog.setStoreMinSeverity(ZAIES_Warning);
        GenlogWin->registerDisplayColorCallBack(&ErrorLog);
    }
    GenlogWin->show();
}

void ZRawVisuMain::repairIndexes(bool pTestRun,bool pRebuildAll)
{
    initLogWindow();
  ZStatus wSt=ZS_SUCCESS;
/*
  FILE* wRepairLog = fopen("repairindex.log","w");
*/
  wSt=zrepairIndexes( URICurrent.toCChar(),
                                  pTestRun,
                                  pRebuildAll,
                                  &ErrorLog);
/*  fflush(wRepairLog);
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

void ZRawVisuMain::removeIndex()
{

}

void ZRawVisuMain::addIndex()
{

}

void ZRawVisuMain::rebuildIndex()
{

}

void
ZRawVisuMain::extractIndex()
{

}

void
ZRawVisuMain::dowdgrade()
{

}
void
ZRawVisuMain::reorganizeZMF()
{

}

void
ZRawVisuMain::ZRFUnlock()
{
  if (OpenMode != VMD_Random)
  {
    QMessageBox::critical(this,tr("Operation error"),"Cannot unlock : file must be open as random file.");
    return;
  }
  ZStatus wSt=ZRandomFile::zutilityUnlockZRF(URICurrent);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast("Unlock file");
    ZException.pop();
  }
}
void
ZRawVisuMain::ZHeaderRawUnlock()
{
  if (OpenMode != VMD_RawSequential)
  {
    QMessageBox::critical(this,tr("Operation error"),"Cannot unlock : header file must be open as raw file.");
    return;
  }
  ZStatus wSt=ZRandomFile::zutilityUnlockHeaderFile(URICurrent);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast("Unlock header file");
    ZException.pop();
  }
}


void
ZRawVisuMain::actionOpenFileByType(bool pChecked)
{
  ZStatus wSt;
  uriString wDir = GeneralParameters.getWorkDirectory();
  QString wFileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  wDir.toCChar(),
                                                  "ZContent files (*.zrf *.zmf *.zix *.zrh);;All (*.*)");
  if (wFileName.isEmpty()) {
    QMessageBox::critical(this,tr("No file selected"),"Please select a valid file");
    return;
  }
  URICurrent = wFileName.toUtf8().data();
  FullPathLbl->setText(wFileName);

  setFileType (wFileName.toUtf8().data());

  if (URICurrent.getFileExtension()=="zrf")
    {
    if (ZS_SUCCESS==openZRF())
      {
        removeAllRows();
        //          displayListZRFFirstRecord(Width);
        return;
      }
    }

  if (URICurrent.getFileExtension()=="zmf")
      {
        FileTypeLBl->setText("Master file");
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
        FileTypeLBl->setText("Header file");
        openZRH();
        loadAll();
        return;
      }

  if (ZS_SUCCESS==openOther(URICurrent.toCChar()))
    {
      removeAllRows();
      LoadAllBTn->setVisible(true);
      LoadAllBTn->setText(QObject::tr("All","ZRawVisuMain"));
      return;
    }

    ZExceptionDLg::displayLast("Open file");
    ZException.pop();

  return;
} // actionOpenFileByType




ZStatus
ZRawVisuMain::openZRH()
{
    ZStatus wSt= openZRH(URICurrent.toCChar());
  if (wSt != ZS_SUCCESS)
    {
    QMessageBox::critical(this,tr("header file open error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
    }

  removeAllRows();
//  displayFdNextRawBlock(BlockSize,16);

  BackwardBTn->setVisible(false);
  ForwardBTn->setVisible(true);

  RecordFRm->setVisible(false);
  SequentialFRm->setVisible(true);


  rawMEn->setEnabled(true);

  openByTypeQAc->setVisible(false);
  openRawQAc->setVisible(false);
  openZRFQAc->setVisible(false);
  openZMFQAc->setVisible(false);
  unlockZRFQAc->setVisible(false);

  return wSt;
}//actionOpenFileAsZRH

void
ZRawVisuMain::openRaw()
{
  ZStatus wSt;
    if ((wSt=openOther(URICurrent))!= ZS_SUCCESS) {
    utf8VaryingString wStr=ZException.formatFullUserMessage();
    ZExceptionDLg::messageWAdd("openRaw",wSt, Severity_Error,wStr,"File open error. File %s",URICurrent.toCChar());
    return;
  }

  removeAllRows();

  BackwardBTn->setVisible(false);
  ForwardBTn->setVisible(true);

  RecordFRm->setVisible(false);
  SequentialFRm->setVisible(true);

  closeQAc->setVisible(true);

  rawMEn->setEnabled(true);

  openByTypeQAc->setVisible(false);
  openRawQAc->setVisible(false);
  openZRFQAc->setVisible(false);
  openZMFQAc->setVisible(false);
  unlockZRFQAc->setVisible(false);

  return;
}//actionOpenFileAsRaw





ZStatus
ZRawVisuMain::openZRF()
{

/*
  const char* wWDParam=getenv(__PARSER_PARAM_DIRECTORY__);
  if (!wWDParam)
    wWDParam="";
  const char* wWDWork=getenv(__PARSER_WORK_DIRECTORY__);
  if (!wWDWork)
    wWDWork="";
*/
  uriString wWDWork = GeneralParameters.getWorkDirectory();
//  uriString wWDParam = GeneralParameters.getParamDirectory();

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Open File"),
        wWDWork.toCChar(),
        "Random files(*.zmf *.zrf *.zix);;All(*.*)");

    if (wFileName.isEmpty())
      return ZS_CANCEL;

    URICurrent = wFileName.toUtf8().data();

    setFileDescription(URICurrent);

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
          wWDWork.toCChar(),
          "Header (*.zrh);;All (*.*)");
      if (wFileName.isEmpty())
        return ZS_CANCEL;

      wURIHeader = wFileName.toUtf8().data();
    }

    poolVisu* wPoolVisu = new poolVisu(this);

    ZStatus wSt1=wPoolVisu->set(URICurrent,wURIHeader);

    wPoolVisu->dataSetup(0);

    wPoolVisu->show();

    return wSt1;

utf8VaryingString wStr;
ZStatus wSt=ZS_SUCCESS;

  if (RandomFile)
  {
    if (RandomFile->isOpen())
      RandomFile->zclose();
  }
  else
    RandomFile=new ZRandomFile;

  wSt = RandomFile->zopen(URICurrent.toCChar(),ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS)
    {
    if (wSt==ZS_FILETYPEWARN)
      ZExceptionDLg::displayLast("Open file");
    else
      {
      delete RandomFile;
      RandomFile=nullptr;
      ZExceptionDLg::displayLast("Open file");
      return wSt;
      }
    }

  RawData.clear();

  if (Fd>=0)
  {
    ::close(Fd);
    Fd=-1;
  }

  if (RawMasterFile)
  {
    if (RawMasterFile->isOpen())
      RawMasterFile->zclose();
    delete RawMasterFile;
    RawMasterFile=nullptr;
  }
  FileOffset=0;
  RecordNumber=0;

  SequentialFRm->setVisible(false);
  RecordFRm->setVisible(true);

  wStr.sprintf("%ld",RandomFile->getFileDescriptor().ZBAT.count());
  RecordTotalLBl->setText(wStr.toCChar());

  LoadAllBTn->setVisible(false);

  BackwardBTn->setVisible(false);
  if (RandomFile->getFileDescriptor().ZBAT.count() > 1)
    BackwardBTn->setVisible(true);
  else
    BackwardBTn->setVisible(false);

  OpenModeLbl->setText("Random file mode");
  OpenMode = VMD_Random;

  openByTypeQAc->setVisible(false);
  openRawQAc->setVisible(false);
  openZRFQAc->setVisible(false);
  openZMFQAc->setVisible(false);

   FileTypeLBl->setText("Random file");

   rawMEn->setEnabled(false);
   ZRFMEn->setEnabled(true);
   ZMFMEn->setEnabled(false);

  return ZS_SUCCESS;
//  return  displayListZRFFirstRecord(Width);
}//openZRF

ZStatus
ZRawVisuMain::openZMF(const uriString& pFilePath)
{
utf8VaryingString wStr;
ZStatus wSt=ZS_SUCCESS;

  if (RawMasterFile)
    {
    if (RawMasterFile->isOpen())
      RawMasterFile->zclose();
    }
  else
    RawMasterFile=new ZRawMasterFile;

  wSt= RawMasterFile->zopen(pFilePath,ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS)
  {
    if (wSt==ZS_FILETYPEWARN)
      ZExceptionDLg::displayLast("Open file");
    else
    {
      delete RawMasterFile;
      RawMasterFile=nullptr;
      ZExceptionDLg::displayLast("Open file");
      return wSt;
    }
  }

  RawData.clear();
  setFileDescription(pFilePath);


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

  wStr.sprintf("%ld",RawMasterFile->getFileDescriptor().ZBAT.count());
  RecordTotalLBl->setText(wStr.toCChar());

  LoadAllBTn->setVisible(false);
  BackwardBTn->setVisible(false);
  if (RawMasterFile->getFileDescriptor().ZBAT.count() > 1)
    BackwardBTn->setVisible(true);
  else
    BackwardBTn->setVisible(false);

   OpenModeLbl->setText("Master file mode");
   OpenMode = VMD_Master;

   openByTypeQAc->setVisible(false);
   openRawQAc->setVisible(false);
   openZRFQAc->setVisible(false);
   openZMFQAc->setVisible(false);

   rawMEn->setEnabled(false);
   ZRFMEn->setEnabled(false);
   ZMFMEn->setEnabled(true);

  return ZS_SUCCESS;
} // openZMF


void
ZRawVisuMain::getRaw()
{

  uriString wDir = GeneralParameters.getWorkDirectory();
  QString wFileName = QFileDialog::getOpenFileName(this, "Dictionary file",
      wDir.toCChar(),"ZContent files (*.zrf *.zmf *.zix *.zrh);;All (*.*)");
  if (wFileName.isEmpty()) {
    return ;
  }
  uriString wSelected = wFileName.toUtf8().data();

  if (!wSelected.exists()){
    ZExceptionDLg::adhocMessage("File",Severity_Error,nullptr,nullptr,"File %s does not exist ",wSelected.toCChar());
    return ;
  }
  URICurrent = wSelected;
//  if (!chooseFile(true))
//    return ;
  if (openOther(URICurrent.toCChar())!= ZS_SUCCESS) {
    utf8VaryingString wAdd = ZException.last().formatFullUserMessage().toString();
    ZExceptionDLg::adhocMessage(tr("Random File open error").toUtf8().data(),ZException.last().Severity,nullptr,&wAdd,
        QCoreApplication::translate("ZRawVisuMain","Cannot load file",nullptr).toUtf8().data());
//    QMessageBox::critical(this,tr("Random File open error"),ZException.formatFullUserMessage().toCChar());
    return;
  }
  openRaw();

  return;
}//getRaw


ZStatus
ZRawVisuMain::unlockZRFZMF(const char* pFilePath)
{

  ZStatus wSt=  ZRandomFile::zutilityUnlockZRF(pFilePath);

  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Unlock file");
    ZException.pop();
  }
  else
    ZExceptionDLg::message("ZRawVisuMain::unlockZRFZMF",ZS_SUCCESS,Severity_Information,"File <%s> has been successfully unlocked.",pFilePath);
  return wSt;
}//unlockZRFZMF




/*
void
ZRawVisuMain::getFileSize(const uriString& pFile)
{
  FileSizeLBl->setText(formatSize(pFile.getFileSize()).toCChar());
}
*/

ZStatus
ZRawVisuMain::openOther(const uriString& pFile)
{

  ZStatus wSt=ZS_SUCCESS;

  URICurrent = pFile ;
  wSt=rawOpen(Fd,URICurrent,O_RDONLY);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage(
              _GET_FUNCTION_NAME_,
              ZS_ERROPEN,
              Severity_Severe,
              " Error opening file %s ",
              URICurrent.toCChar());
    return  ZS_ERROPEN;
    }

  RawData.clear();
  LoadAllBTn->setVisible(true);
  LoadAllBTn->setText(QObject::tr("All","ZRawVisuMain"));

  FileSizeLBl->setText(formatSize(URICurrent.getFileSize()).toCChar());

  if (RawMasterFile)
    {
    if (RawMasterFile->isOpen())
      RawMasterFile->zclose();
    delete RawMasterFile;
    RawMasterFile=nullptr;
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

  SequentialFRm->setVisible(true);
  RecordFRm->setVisible(false);

  displayHCBQAc->setEnabled(true);
  displayMCBQAc->setEnabled(true);
  displayICBQAc->setEnabled(true);
  displayFCBQAc->setEnabled(true);

  headerMEn->setEnabled(true);

  OpenModeLbl->setText("Raw mode");
  OpenModeLbl->setVisible(true);
//  ClosedLBl->setVisible(false);

  OpenMode = VMD_RawSequential;

  rawMEn->setEnabled(true);
  ZRFMEn->setEnabled(false);
  ZMFMEn->setEnabled(false);

  return ZS_SUCCESS;
}//openOther

ZStatus
ZRawVisuMain::openZRH(const uriString& pFileName)
{
  ZStatus wSt = rawOpen(Fd,pFileName,O_RDONLY);       // open content file for read only
  if (wSt != ZS_SUCCESS) {
      ZException.setMessage(
                          _GET_FUNCTION_NAME_,
                          ZS_ERROPEN,
                          Severity_Severe,
                          " Error opening file %s ",
                          pFileName.toCChar());
  }


  RawData.clear();
  LoadAllBTn->setVisible(true);
  LoadAllBTn->setText(QObject::tr("All","ZRawVisuMain"));
  setFileDescription(pFileName);

  if (RawMasterFile)
  {
    if (RawMasterFile->isOpen())
      RawMasterFile->zclose();
    delete RawMasterFile;
    RawMasterFile=nullptr;
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

  SequentialFRm->setVisible(true);
  RecordFRm->setVisible(false);

  LoadAllBTn->setVisible(true);

  displayHCBQAc->setEnabled(true);
  displayICBQAc->setEnabled(true);
  displayMCBQAc->setEnabled(true);
  displayFCBQAc->setEnabled(true);

  headerMEn->setEnabled(true);

  OpenModeLbl->setText("Header");
  OpenModeLbl->setVisible(true);
//  ClosedLBl->setVisible(false);
  closeQAc->setVisible(true);

  OpenMode = VMD_Header;

  rawMEn->setEnabled(true);
  ZRFMEn->setEnabled(false);
  ZMFMEn->setEnabled(false);

  return ZS_SUCCESS;
}//openZRH

void
ZRawVisuMain::displayHCB()
{
  if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)))
    return;

  if (!entityWnd)
    entityWnd=new DisplayMain(&URICurrent,this);
  entityWnd->displayHCB(RawData);
}

void
ZRawVisuMain::displayFCB()
{
  if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)))
    return;

  if (!entityWnd)
    entityWnd=new DisplayMain(&URICurrent,this);
  entityWnd->displayFCB(RawData);
}

void
ZRawVisuMain::displayMCB()
{
  if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return ;

  if (!entityWnd)
      entityWnd=new DisplayMain(&URICurrent,this);
  entityWnd->displayMCB(RawData);
}


void
ZRawVisuMain::displayICBs()
{
  /* HCB and MCB are deserialized here just to test if there is enough space.
   * these values are computed again within DisplayMain::--- to access real values
   */
  if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return ;

  const unsigned char* wPtr = RawData.Data;
  ZHeaderControlBlock_Export wZHCBe;
  memmove(&wZHCBe,RawData.Data,sizeof(ZHeaderControlBlock_Export));
  if (wZHCBe.StartSign!=cst_ZBLOCKSTART) {
    ZExceptionDLg::adhocMessage("Invalid Header",Severity_Error,nullptr,nullptr,"Header Control Block appears to be corrupted.");
    return;
  }

  wZHCBe.deserialize();

  /* Index file : ICB is the only content of reserved block */
  if (wZHCBe.FileType==ZFT_ZIndexFile) {
    if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZICB_Export)))
      return;
    if (!entityWnd)
      entityWnd=new DisplayMain(&URICurrent,this);
    const unsigned char* wPtrIn=RawData.Data + wZHCBe.OffsetReserved;
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

  if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)+wZMCBe.ICBOffset + wZMCBe.ICBSize))
    return;
  if (!entityWnd)
    entityWnd=new DisplayMain(&URICurrent,this);

  entityWnd->displayICBs(RawData);

  return;
}

void
ZRawVisuMain::displayZBAT()
{
  if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return ;

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
ZRawVisuMain::displayPool(const unsigned char* pPtr,zaddress_type pOffset,const char* pTitle)
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
ZRawVisuMain::displayZDBT()
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
ZRawVisuMain::displayZFBT()
{
  if (!testRequestedSize(URICurrent,RawData,sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+sizeof(ZMCB_Export)))
    return;

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



bool
ZRawVisuMain::testRequestedSize(const uriString& pURI, ZDataBuffer& pRawData,size_t pRequestedSize){
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
      ZExceptionDLg::message("ZRawVisuMain::displayMCB",ZS_INVSIZE,Severity_Error,
          "Cannot load content of file <%s> size <%lld> exceeds memory capacity",
          pURI.toString(),
          pURI.getFileSize());
      return false;
    }
    if (pURI.getFileSize()<(pRequestedSize+1))
    {
      ZExceptionDLg::message("ZRawVisuMain::displayMCB",ZS_INVSIZE,Severity_Error,
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


void
ZRawVisuMain::setFileDescription (const uriString &pFilePath)
{
    FullPathLbl->setText(pFilePath.toCChar());
    FileSize = pFilePath.getFileSize();
    FileSizeLBl->setText(formatSize(FileSize).toCChar());
    FullSizeLBl->setText((formatSize(FileSize)).toCChar());
    URICurrent = pFilePath;
    setFileType(pFilePath);
}
void
ZRawVisuMain::setFileType (const uriString &pFilePath)
{

  utf8VaryingString wExt=pFilePath.getFileExtension();

  while (true)
  {
  if (wExt=="zrf")
    {
    FileTypeLBl->setText("Random file");
    break;
    }
    if (wExt=="zmf")
    {
      FileTypeLBl->setText("Master file");
      break;
    }
    if (wExt=="zix")
    {
      FileTypeLBl->setText("Index file");
      break;
    }
    if (wExt=="zrh")
    {
      FileTypeLBl->setText("Header file");
      unlockZRFQAc->setVisible(true);
      return ;
    }
    if (wExt=="dic")
    {
      FileTypeLBl->setText("Dictionary file");
      unlockZRFQAc->setVisible(true);
      return ;
    }
    FileTypeLBl->setText("Unmanaged file type");
    break;
  }//while true

/*
  openByTypeQAc->setVisible(true);
  openRawQAc->setVisible(true);
  openZRFQAc->setVisible(true);
  openZMFQAc->setVisible(true);
  unlockZRFQAc->setVisible(true);
  closeQAc->setVisible(true);
*/
/*
  OpenModeLbl->setVisible(true);
  ClosedLBl->setVisible(false);
*/
//  actionMEn->setEnabled(true);
}//setFileType

void
ZRawVisuMain::actionClose(bool pChecked)
{
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
    BackwardBTn->setVisible(false);
    ForwardBTn->setVisible(false);

    rawMEn->setEnabled(false);
    ZRFMEn->setEnabled(false);
    ZMFMEn->setEnabled(false);

    openByTypeQAc->setVisible(true);
    openRawQAc->setVisible(true);
    openZRFQAc->setVisible(true);
    openZMFQAc->setVisible(true);

    OpenModeLbl->setText("not open");
    OpenModeLbl->setVisible(false);
//    ClosedLBl->setVisible(true);

    OpenMode=VMD_Nothing;

  return;
}//actionClose



ZStatus
ZRawVisuMain::displayFdNextRawBlock(ssize_t pBlockSize,size_t pWidth)
{
  ZDataBuffer wRecord;
  wRecord.allocate(pBlockSize);
  ZStatus wSt=rawRead(Fd,wRecord,pBlockSize);
  if (wSt==ZS_EOF) {
    AllFileLoaded=true;
    ForwardBTn->setVisible(false);
    return wSt;
  }
  if (wSt!=ZS_SUCCESS)
  return wSt;

  RawData.appendData(wRecord);

  UserOffsetDoNotChange = true;
  ReadSizeLBl->setText(formatSize(RawData.Size).toCChar());

  return displayWidgetBlock(wRecord,(size_t&)FileOffset);
}//displayFdNextRawBlock


void
ZRawVisuMain::displayOneLine(int pRow,unsigned char* &pPtr,unsigned char* pPtrEnd, size_t pFileOffset, bool pUseHexa) {

  utf8VaryingString  wLineHexa,wLineAscii,wlineOffset;

//  wlineOffset.sprintf("%6d-0x%6X",FileOffset,FileOffset);

  if (pUseHexa)
        wlineOffset.sprintf("0x%6X",pFileOffset);
    else
        wlineOffset.sprintf("%6ld",pFileOffset);

  wLineAscii.allocateUnitsBZero(Width+1);

  int wCol=0;
  int wD=0;
  for (int wi=0;(wi < Width) && (pPtr < pPtrEnd);wi++) {
    wLineHexa.sprintf("%02X",*pPtr);
    VisuTBv->ItemModel->setItem(pRow,wCol,new QStandardItem(wLineHexa.toCChar()));
    if ((*pPtr>31)&&(*pPtr<127))
      wLineAscii.Data[wi]=*pPtr;
    else
      wLineAscii.Data[wi] = '.' ;

    wCol++;
    wD++;
    if (wD==4) {
      wD=0;
      wCol++;
    }
    pPtr++;
  }// for
  VisuTBv->ItemModel->setVerticalHeaderItem (pRow,new QStandardItem(wlineOffset.toCChar()));

  //    VisuTBv->setItem(wCurLine,0,new QTableWidgetItem(wLineHexa.toCChar()));
  VisuTBv->ItemModel->setItem(pRow,20,new QStandardItem(wLineAscii.toCChar()));

  if (DefaultBackGround.isValid())
    return;
  DefaultBackGround = VisuTBv->ItemModel->item(0,0)->data(Qt::BackgroundRole);

} // displayOneLine


ZStatus
ZRawVisuMain::displayWidgetBlock(ZDataBuffer& pData, size_t& pFileOffset) {

  ProgressPGb->setRange(0,pData.Size);
  ProgressPGb->setValue(0);
  ProgressPGb->setVisible(true);

  /* how many lines within pData block */

  int wLines=pData.Size / Width;

  int wRem=pData.Size - ( wLines * Width);

  if (wRem > 0)
    wLines++;

  int wBaseLine=VisuTBv->ItemModel->rowCount();

  VisuTBv->ItemModel->setRowCount(VisuTBv->ItemModel->rowCount()+wLines);

 // utf8VaryingString  wLineHexa,wLineAscii,wlineOffset;

  /* cut by chunk of Width bytes */
  long  wRemain=pData.Size;
  unsigned char* wPtr = pData.Data;
  unsigned char* wPtrEnd = pData.Data+pData.Size;

  int wTick = 0;
  long wProgress=0;
  int wCurLine=wBaseLine;
  while ((wPtr < wPtrEnd)&&(wRemain >= Width)) {

    displayOneLine(wCurLine,wPtr,wPtrEnd,pFileOffset,UseHexa);

    pFileOffset += Width;
    wRemain -= Width;
    wProgress += Width;
    wCurLine++;
    if (wTick++ > 10) {
      wTick=0;
      ProgressPGb->setValue(wProgress);
    }
  }//while ((wPtr < wPtrEnd)&&(wRemain >= Width))

  if (wRemain > 0) {
    displayOneLine(wCurLine,wPtr,wPtrEnd,pFileOffset,UseHexa);
    pFileOffset += wRemain;
  }


  for (int wi=0;wi < VisuTBv->ItemModel->rowCount();wi++)
    VisuTBv->resizeRowToContents(wi);
  for (int wi=0;wi < VisuTBv->ItemModel->columnCount();wi++)
      VisuTBv->resizeColumnToContents(wi);

  VisuTBv->setColumnWidth(4,4);
  VisuTBv->setColumnWidth(9,4);
  VisuTBv->setColumnWidth(16,4);

  ProgressPGb->setValue(pData.Size);
  return ZS_SUCCESS;
}//displayBlock

void
ZRawVisuMain::displayBlockData()
{
  utf8VaryingString wStr;
  wStr.sprintf("%6ld",RecordNumber+1L) ;
  RecordNumberLBl->setText(wStr.toCChar());
  wStr.sprintf("0x%08X",Address) ;
  AddressLBl->setText(wStr.toCChar());

  wStr.sprintf("%8ld",Block.BlockSize) ;
  BlockSizeLBl->setText(wStr.toCChar());

  wStr.sprintf("%8ld",Block.DataSize()) ;
  UserSizeLBl->setText(wStr.toCChar());

  StateLBl->setText(decode_ZBS(Block.State));

  wStr.sprintf("%02X %s",Block.Lock,decode_ZLockMask(Block.Lock).toCChar());
  LockMaskLBl->setText(wStr.toCChar());

}

ZStatus
ZRawVisuMain::displayListZRFNextRecord(size_t pWidth)
{
  ZStatus wSt;
  if (RecordNumber==0)
    BackwardBTn->setVisible(false);
  else
    BackwardBTn->setVisible(true);

  // get first block of the file
  wSt=RandomFile->_getNext(Block,RecordNumber,Address);     // next logical block
  if (wSt==ZS_EOF)
  {
    ForwardBTn->setVisible(false);
    return ZS_EOF;
  }
  ForwardBTn->setVisible(true);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast("Read file");
//    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }


  displayBlockData();

  RawData=Block.Content;

  removeAllRows();

  displayWidgetBlock(RawData,(size_t&)FileOffset);

  ForwardBTn->setVisible(true);
  return ZS_SUCCESS;
}//displayListZRFNextRecord

ZStatus
ZRawVisuMain::displayListZRFPreviousRecord(size_t pWidth)
{
  ZStatus wSt;

  RecordNumber--;
  if (RecordNumber==0)
    BackwardBTn->setVisible(false);

  // get first block of the file
  wSt=RandomFile->_getByRank(Block,RecordNumber,Address);     // get first block of the file
  if (wSt==ZS_EOF)
  {
    ForwardBTn->setVisible(false);
    return ZS_EOF;
  }

  if (wSt!=ZS_SUCCESS)
  {
    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }

  displayBlockData();

  if (RandomFile->getFileDescriptor().ZBAT.count() > 1)
    ForwardBTn->setVisible(true);
  RawData=Block.Content;

  displayWidgetBlock(RawData,(size_t&)FileOffset);


  return ZS_SUCCESS;
}//displayListZRFPreviousRecord

ZStatus
ZRawVisuMain::displayListZRFFirstRecord(size_t pWidth)
{
  ZStatus wSt;

  BackwardBTn->setVisible(false);

  FileOffset=0;
  RecordNumber = 0L;
  wSt=RandomFile->_getByRank(Block,RecordNumber,Address);     // get first block of the file
  if (wSt==ZS_EOF)
  {
    ForwardBTn->setVisible(false);
    return ZS_EOF;
  }

  if (wSt!=ZS_SUCCESS) {
    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }

  displayBlockData();

  RawData=Block.Content;

  displayWidgetBlock(RawData,(size_t&)FileOffset);

  ForwardBTn->setVisible(true);
  return ZS_SUCCESS;
}//displayListZRFFirstRecord

void
ZRawVisuMain::removeAllRows()
{
  if (VisuTBv->ItemModel->rowCount() > 0)
  {
    VisuTBv->ItemModel->removeRows(0,VisuTBv->ItemModel->rowCount());
  }
}

void
ZRawVisuMain::backward()
{
  ZStatus wSt=ZS_SUCCESS;

  switch (VisuType)
  {
  case ZCVM_RandomFile:
      wSt =  displayListZRFNextRecord(Width);
      return;
  case ZCVM_Raw:
      if (FileOffset == 0 ) {
          BackwardBTn->setVisible(false);
          return ;
      }
      FileOffset -= PayLoad;
      FileOffset -= PayLoad;
      if (FileOffset < 0 )
          FileOffset = 0;

      if ((FileOffset+PayLoad) > FileSize)
          ForwardBTn->setVisible(false);
      else
          ForwardBTn->setVisible(true);

      wSt=displayRawFromOffset(PayLoad);
      return ;
  case ZCVM_MasterFile:
      wSt =  displayListZRFNextRecord(Width);
      return;
  }


  if (RandomFile)
  {
    displayListZRFPreviousRecord(Width);
    return;
  }

  FileOffset=lseek(Fd,(__off_t)-BlockSize,SEEK_CUR);
  if (FileOffset <= 0)
    {
    BackwardBTn->setVisible(false);
    }
  displayFdNextRawBlock(BlockSize,Width);
}
void
ZRawVisuMain::forward()
{
ZStatus wSt=ZS_SUCCESS;

  switch (VisuType)
    {
    case ZCVM_RandomFile:
      wSt =  displayListZRFNextRecord(Width);
      break;
    case ZCVM_Raw:
        if ( FileOffset >= FileSize  ) {
            ForwardBTn->setVisible(false);
            return;
        }

        wSt=displayRawFromOffset(PayLoad);
        break;
    case ZCVM_MasterFile:
        wSt =  displayListZRFNextRecord(Width);
        break;
    }
/*
  if (RandomFile)
  {
    displayListZRFNextRecord(Width);
    return;
  }

  AddressLBl->setText("0");
  RecordNumberLBl->setText("0");

  if (FromOffset) {
      wSt=displayRawFromOffset();
  } else {
  wSt=displayFdNextRawBlock(BlockSize,Width);
  }
*/
  if ((wSt==ZS_EOF) || (wSt==ZS_READPARTIAL))
    ForwardBTn->setVisible(false);
}

ZStatus
ZRawVisuMain::displayRawFromOffset(size_t pPayLoad)
{
//    ZDataBuffer wPayLoadContent;

    ForwardBTn->setVisible(true);

    LoadAllBTn->setVisible(false);
//    LoadAllBTn->setText(QObject::tr("Reload","ZRawVisuMain"));

    SequentialFRm->setVisible(true);

    if (FileOffset==0) {
        BackwardBTn->setVisible(false);
    }
    else
        BackwardBTn->setVisible(true);

 //   ZStatus wSt = URICurrent.loadContentAt(wPayLoadContent,FileOffset,pPayLoad);
    ZStatus wSt = URICurrent.loadContentAt(RawData,off_t(FileOffset),pPayLoad);

    if (wSt!=ZS_SUCCESS){
        if (wSt==ZS_READPARTIAL) {
            pPayLoad = RawData.Size;

            ForwardBTn->setVisible(false);
        }
        else if (wSt==ZS_EOF) {
            return wSt;
        } else {
            utf8VaryingString wAdd = ZException.last().formatFullUserMessage().toString();
            ZExceptionDLg::adhocMessage("Raw File load error",ZException.last().Severity,nullptr,&wAdd,
                                        "Cannot load file");
            return wSt;
        }
    }

    utf8VaryingString wStr;
    if (UseHexa)
        wStr.sprintf("0x%X",FileOffset);
    else
        wStr.sprintf("%ld",FileOffset);
    UserOffsetDoNotChange = true;
    ReadSizeLBl->setText(wStr.toCChar());

    InterPunctLBl->setText("to");

    if (UseHexa)
        wStr.sprintf("0x%X",FileOffset+pPayLoad);
    else
        wStr.sprintf("%ld",FileOffset+pPayLoad);
    FullSizeLBl->setText(wStr.toCChar());



    removeAllRows();
//    wSt=displayWidgetBlock(wPayLoadContent,(size_t&)FileOffset);
    wSt=displayWidgetBlock(RawData,(size_t&)FileOffset);

 //   AllFileLoaded=true;
    return wSt;
} // ZRawVisuMain::loadAll

ZStatus
ZRawVisuMain::loadAll()
{
  BackwardBTn->setVisible(false);
  ForwardBTn->setVisible(false);

  LoadAllBTn->setVisible(true);
  LoadAllBTn->setText(QObject::tr("Reload","ZRawVisuMain"));

  SequentialFRm->setVisible(true);

  FileOffset=0;

  RawData.clear();
  ZStatus wSt = URICurrent.loadContent(RawData);
  if (wSt!=ZS_SUCCESS) {
      utf8VaryingString wAdd = ZException.last().formatFullUserMessage().toString();
      ZExceptionDLg::adhocMessage("Raw File load error",ZException.last().Severity,nullptr,&wAdd,
                                  "Cannot load file");
      return wSt;
  }
  UserOffsetDoNotChange = true;
  ReadSizeLBl->setText((formatSize(RawData.Size)).toCChar());

  removeAllRows();
  wSt=displayWidgetBlock(RawData,(size_t&)FileOffset);


  AllFileLoaded=true;
  return wSt;
} // ZRawVisuMain::loadAll

ZStatus
ZRawVisuMain::loadRawPayLoad()
{
    VisuType = ZCVM_Raw;
    ZDataBuffer wReadPayLoad;
 //   BackwardBTn->setVisible(false);
 //   ForwardBTn->setVisible(false);

    LoadAllBTn->setVisible(true);
 //   LoadAllBTn->setText(QObject::tr("Reload","ZRawVisuMain"));

    SequentialFRm->setVisible(true);

    FileOffset=0;

//    RawData.clear();

    ZStatus wSt=rawRead(Fd,wReadPayLoad,PayLoad);
    if ((wSt!=ZS_SUCCESS)&&(wSt != ZS_READPARTIAL)) {

        return wSt;
    }

    if (wSt==ZS_READPARTIAL)
        AllFileLoaded=true;

    RawData.appendData(wReadPayLoad);
 //   removeAllRows();
    wSt=displayWidgetBlock(wReadPayLoad,(size_t&)FileOffset);

    UserOffsetDoNotChange = true;
    ReadSizeLBl->setText((formatSize(RawData.Size)).toCChar());

    return wSt;
} //loadRawPayLoad


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


utf8VaryingString
displaytScanBlock(ScanBlock& pSB,ScanBlockCount& pSBCount) {
  int wH=0;
  utf8VaryingString wOut,wBlockSize,wHoleSize,wComment;
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
ZRawVisuMain::surfaceScanRaw(const uriString & pURIContent,FILE*pOutput) {
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

  wSt=wSBlock.BlockHeader._importConvert(wSBlock.BlockHeader,(ZBlockHeader_Export*)wBlockBuffer.Data);
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
    wSt=wSBlock.BlockHeader._importConvert(wSBlock.BlockHeader,(ZBlockHeader_Export*)wBlockBuffer.Data);
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
    wSt=wSBlock.BlockHeader._importConvert(wSBlock.BlockHeader,(ZBlockHeader_Export*)wBlockBuffer.Data);
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

void ZRawVisuMain::displayRawSurfaceScan(const uriString& pFileToScan)
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

void ZRawVisuMain::closeMCBCB(const QEvent *pEvent)
{
  MCBWin=nullptr;
}
void
ZRawVisuMain::textEditMorePressed()
{

}
void ZRawVisuMain::closeGenlogCB(const QEvent* pEvent)
{
  GenlogWin=nullptr;
}
textEditMWn*  ZRawVisuMain::openGenLogWin()
{
  if (GenlogWin==nullptr) {
      GenlogWin = new textEditMWn(this);
      GenlogWin->registerCloseCallback(std::bind(&ZRawVisuMain::closeGenlogCB, this,std::placeholders::_1));
  }
  else
    GenlogWin->clear();
  return GenlogWin;
}
void
ZRawVisuMain::GenlogWindisplayErrorCallBack(const utf8VaryingString& pMessage) {
  if (GenlogWin!=nullptr)
    GenlogWin->appendText(pMessage);
  else
    fprintf(stderr,pMessage.toCChar());
}
void
ZRawVisuMain::GenlogWindisplayErrorColorCB(uint8_t pSeverity,const utf8VaryingString& pMessage) {
  if (GenlogWin==nullptr) {
    fprintf(stderr,pMessage.toCChar());
    return;
  }
  switch (pSeverity) {
  case ZAIES_Text:
    GenlogWin->appendText(pMessage);
    return;
  case ZAIES_Info:
    GenlogWin->appendTextColor(Qt::blue, pMessage);
    return;
  case ZAIES_Warning:
    GenlogWin->appendTextColor(Qt::darkGreen, pMessage);
    return;
  case ZAIES_Error:
  case ZAIES_Fatal:
    GenlogWin->appendTextColor(Qt::red, pMessage);
    return;
  default:
    GenlogWin->appendTextColor(Qt::yellow, pMessage);
    return;
  }
}
void ZRawVisuMain::DicEditQuitCallback(){
//  delete dictionaryWnd;
  DicEdit = nullptr;
}

ZStatus
ZRawVisuMain::exportZMF(QWidget* pParent,zbs::ZRawMasterFile* pRawMasterFile,ZaiErrors* pErrorLog)
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

  ZMFProgressMWn* ProgressMWn = new ZMFProgressMWn("Data export",pParent,false);

  /*
  ProgressMWn->setDescBudy("Export to file");
  ProgressMWn->setDescText(wURIExport);
  ProgressMWn->setProcessDescription("Records processed");
  */
  ProgressMWn->labelsSetup("Export to file",wURIExport,"Records processed");

  ProgressMWn->advanceSetupCallBack(pRawMasterFile->getRecordCount(),"Export to file");

  ProgressMWn->show();

  pRawMasterFile->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack, ProgressMWn,std::placeholders::_1,std::placeholders::_2));
  pRawMasterFile->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack, ProgressMWn,std::placeholders::_1,std::placeholders::_2));

  ZStatus wSt=pRawMasterFile->XmlExportContent(wURIExport,nullptr);

  ProgressMWn->setDone(wSt!=ZS_SUCCESS);

  if (wSt!=ZS_SUCCESS) {
    if (pErrorLog==nullptr)
        fprintf(stderr,"Data export: Something went wrong status <%s> while exporting data from <%s>",
                decode_ZStatus(wSt),
                pRawMasterFile->getURIContent().getBasename().toCChar());
    else
        pErrorLog->errorLog("Data export: Something went wrong status <%s> while exporting data from <%s>",
                            decode_ZStatus(wSt),
                            pRawMasterFile->getURIContent().getBasename().toCChar());
  }

  return wSt;
} //ZRawVisuMain::exportZMF

ZStatus
ZRawVisuMain::clearZMF(QWidget* pParent, ZaiErrors *pErrorLog)
{
  uriString wDir = GeneralParameters.getWorkDirectory();
  uriString wZMFURI;
  ZRawMasterFile *wZMF=nullptr;
  zmode_type wOpenMode=0;
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


  ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Clearing master file",pParent,false);

  wZMF->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));
  wZMF->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));

  ProgressMWn->setDescBudy("File");
  ProgressMWn->setDescText(wZMFURI.toCChar());

  ProgressMWn->show();

  bool wHighwater = wClearFileDLg->getHighwater();
  size_t wSizeToKeep = wClearFileDLg->getSizeToKeep();
  
  wSt = wZMF->zclearAll(wSizeToKeep,wHighwater,pErrorLog);

  ProgressMWn->setDone(wSt!=ZS_SUCCESS);

  wZMF->zclose();
  delete wZMF;
  return wSt;
}
ZStatus ZRawVisuMain::clearZRF()
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


    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Clearing random file",this,false);

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
ZRawVisuMain::cloneZRF( )
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
/*
ZStatus
ZRawVisuMain::reorganizeZRF( )
{
    textEditMWn* wTEMWn = new textEditMWn(this);
    wTEMWn->setWindowTitle("Clone random file log");
    wTEMWn->registerDisplayColorCallBack(&ErrorLog);

    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;
    uriString wCloneURI;
    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Random file to reorganize"),
                                                     wDir.toCChar(),
                                                     "random files (*.zrf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }

    wZRF = new ZRandomFile;

    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Reorganize file",this);

    wZRF->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));
    wZRF->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));

    ProgressMWn->setDescBudy("File");
    ProgressMWn->setDescText(wZRFURI.toCChar());

    ProgressMWn->show();

    wSt=wZRF->zreorgFile(true,&ErrorLog);

    ProgressMWn->setDone(wSt!=ZS_SUCCESS);

    delete wZRF;
    return wSt;
}
*/
/*
ZStatus
ZRawVisuMain::rebuildHeaderZRF()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;
    uriString wCloneURI;
    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Random content file to rebuild"),
                                                     wDir.toCChar(),
                                                     "random content files (*.zrf);;All (*.*)");
    if (wFileName.isEmpty())
        return ZS_EMPTY ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ZS_FILENOTEXIST;
    }

    wZRF = new ZRandomFile;

    ZMFProgressMWn* ProgressMWn=new ZMFProgressMWn("Reorganize file",this);

    wZRF->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));
    wZRF->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));

    ProgressMWn->setDescBudy("File");
    ProgressMWn->setDescText(wZRFURI.toCChar());

    ProgressMWn->show();

    wSt=ZRandomFile::zheaderRebuild(wZRFURI,true,&ErrorLog,
                                      std::bind(&ZMFProgressMWn::advanceCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2),
                                      std::bind(&ZMFProgressMWn::advanceSetupCallBack,ProgressMWn,std::placeholders::_1,std::placeholders::_2));

    ProgressMWn->setDone(wSt!=ZS_SUCCESS);

    delete wZRF;
    return wSt;
}
*/

void ZRawVisuMain::surfaceScanZRF()
{
    uriString wDir = GeneralParameters.getWorkDirectory();
    uriString wZRFURI;
    ZRandomFile *wZRF=nullptr;
    zmode_type wOpenMode=0;
    ZStatus wSt=ZS_SUCCESS;

    QString wFileName = QFileDialog::getOpenFileName(this, tr("Random file to scan"),
                                                     wDir.toCChar(),
                                                     "random content files (*.zrf);;All (*.*)");
    if (wFileName.isEmpty())
        return  ;
    wZRFURI= wFileName.toUtf8().data();
    if (!wZRFURI.exists()) {
        return ;
    }
    wZRF = new ZRandomFile;
    //  FILE* wScan=fopen ("surfacescanzrf.txt","w");
    textEditMWn* wTEMWn = new textEditMWn(this);
    ErrorLog.setDisplayColorCallBack(std::bind(&textEditMWn::displayColorCallBack,wTEMWn,std::placeholders::_1,std::placeholders::_2));
    ErrorLog.setStoreSeverityAtLeast(ZAIES_Info);

    wSt=wZRF->zsurfaceScan(wZRFURI,&ErrorLog);

    wZRF->zclose();
    delete wZRF;
    return;
} //surfaceScanZRF

ZStatus
ZRawVisuMain::importZMF(QWidget* pParent, ZaiErrors *pErrorLog)
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

  ZMFProgressMWn* ProgressMWn = new ZMFProgressMWn("Data import",pParent,false);

  /*
  ProgressMWn->setDescBudy("Export to file");
  ProgressMWn->setDescText(wURIExport);
  ProgressMWn->setProcessDescription("Records processed");
  */
  ProgressMWn->labelsSetup("Export to file",wURIContentImport,"Bytes of text processed");

  ProgressMWn->advanceSetupCallBack(wZMF->getRecordCount(),"Import to file");

  ProgressMWn->show();

  wZMF->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack, ProgressMWn,std::placeholders::_1,std::placeholders::_2));
  wZMF->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack, ProgressMWn,std::placeholders::_1,std::placeholders::_2));

  wSt=wZMF->XmlImportContentByChunk(wURIContentImport,pErrorLog);

  ProgressMWn->setDone(wSt!=ZS_SUCCESS);

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
} // ZRawVisuMain::importZMF


