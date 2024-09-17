
#include "filegeneratedlg.h"
//#include "ui_filegeneratedlg.h"

#include <zcontent/zcontentcommon/zgeneralparameters.h>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>

#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QSpacerItem>
#include <QProgressBar>

#include <ztoolset/zlimit.h>

#include <zcontent/zindexedfile/zdictionaryfile.h>
#include <zcontent/zindexedfile/zmasterfile.h>

#include <zcontent/zindexedfile/zdataconversion.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <QStandardItemModel>
#include <QStandardItem>

#include <ztoolset/zarray.h>

#include <zqt/zqtwidget/zqtwidgettools.h>

#include <zqt/zqtwidget/zcombodelegate.h>

#include <QMenu>
#include <QMenuBar>

#include <QFileDialog>


#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory
#include <zcontent/zindexedfile/zrawmasterfileutils.h> // for generateIndexRootName
#include <zcontentcommon/zgeneralparameters.h>

#include <zqt/zqtwidget/zqtutils.h>

#include <zexceptiondlg.h>

#include <zxml/zxmlprimitives.h>

#include <zkeylistdlg.h>
#include <texteditmwn.h>

#include <zrandomfile/zrfutilities.h>  // for renameFile()

#include "dicedit.h"

#include "zkeydlg.h"

#include <ztoolset/ztime.h>

#include "zmfprogressmwn.h"

//#include "zcontentvisumain.h"
#include "zentrypoint.h"

#include <zcontent/zindexedfile/zmetadic.h>
#include <zcontent/zindexedfile/zkeydictionary.h>

const QColor ErroredQCl=Qt::red;
const QColor WarnedQCl=Qt::darkGreen;

const int cst_IndexNameCol =  0 ;
const int cst_IndexRootNameCol =  cst_IndexNameCol + 1 ;
const int cst_KeySizeCol =  cst_IndexRootNameCol + 1  ;
const int cst_KeyDupCol =  cst_KeySizeCol + 1  ;

const int cst_KeyGrabCol =  cst_KeyDupCol + 1  ;
const int cst_KeyHighCol =  cst_KeyGrabCol + 1  ;
const int cst_KeyAllocCol =  cst_KeyHighCol + 1  ;
const int cst_KeyAllocSizeCol =  cst_KeyAllocCol + 1  ;
const int cst_KeyExtentQuotaCol =  cst_KeyAllocSizeCol + 1  ;
const int cst_KeyExtentSizeCol =  cst_KeyExtentQuotaCol + 1  ;


//class textEditMWn;

using namespace zbs;



void
FileGenerateMWn::initLayout() {

  setWindowTitle(QCoreApplication::translate("FileGenerateDLg", "Master file parameters", nullptr));

  resize(800, 700);

  verticalLayoutWidget = new QWidget((QWidget*)this);
  setCentralWidget(verticalLayoutWidget);
  verticalLayoutWidget->setGeometry(QRect(0, 0, 799, 690));
  //  verticalLayoutWidget = centralWidget();

  QVBoxLayout* VLYt = new QVBoxLayout(verticalLayoutWidget);
  VLYt->setContentsMargins(2, 5, 2, 2);
  verticalLayoutWidget->setLayout(VLYt);

  QMenu* GenMEn = new QMenu(QCoreApplication::translate("FileGenerateDLg", "General", nullptr),verticalLayoutWidget);
  QMenuBar* GenMenuBar = menuBar();
  GenMenuBar->addMenu(GenMEn);

  SetupFromZmfQAc=new QAction("Setup from existing master file",GenMEn);
  SetupFromXmlDicQAc=new QAction("Setup from xml dictionary",GenMEn);
  SetupFromDicFileQAc=new QAction("Setup from dictionary file",GenMEn);
  SetupFromXmlDefQAc=new QAction("Setup from xml definition file",GenMEn);

  QuitQAc=new QAction(QObject::tr( "Quit", "FileGenerateDLg") ,GenMEn);

  GenMEn->addAction(SetupFromZmfQAc);
  GenMEn->addAction(SetupFromXmlDicQAc);
  GenMEn->addAction(SetupFromDicFileQAc);
  GenMEn->addAction(SetupFromXmlDefQAc);

  GenMEn->addSeparator();
  GenMEn->addAction(QuitQAc);

  QMenu* DirectoriesMEn = new QMenu("Directories");
  GenMenuBar->addMenu(DirectoriesMEn);

  SearchDirQAc=new QAction("Change master directory" ,GenMEn);
  IndexSearchDirQAc=new QAction("Change indexes directory" ,GenMEn);
  SameAsMasterQAc = new QAction("Set indexes directory same as master" ,GenMEn);

  DirectoriesMEn->addAction(SearchDirQAc);
  DirectoriesMEn->addAction(IndexSearchDirQAc);
  DirectoriesMEn->addAction(SameAsMasterQAc);

  QMenu* KeyMEn = new QMenu("Index keys");
  GenMenuBar->addMenu(KeyMEn);

  KeyAppendRawQAc = new QAction("Add a raw key",KeyMEn);
  KeyAppendFromLoadedDicQAc = new QAction("Add from loaded dictionary",KeyMEn);
  KeyAppendFromEmbeddedDicQAc = new QAction("Add from embedded dictionary",KeyMEn);
  KeyDeleteQAc = new QAction("Delete current key",KeyMEn);

  indexRebuildQAc = new QAction("Rebuild index key",KeyMEn);

  KeyMEn->addAction(KeyAppendRawQAc);
  KeyMEn->addAction(KeyAppendFromLoadedDicQAc);
  KeyMEn->addAction(KeyAppendFromEmbeddedDicQAc);
  KeyMEn->addSeparator();
  KeyMEn->addAction(KeyDeleteQAc);
  KeyMEn->addSeparator();
  KeyMEn->addAction(indexRebuildQAc);

  QMenu* ShowHideMEn = new QMenu("Show-Hide");
  GenMenuBar->addMenu(ShowHideMEn);

  ShowGuessValQAc = new QAction("Show guessed values",GenMEn);
  HideGuessValQAc = new QAction("Hide guessed values",GenMEn);

  ShowLogQAc = new QAction("Show log",GenMEn);
  HideLogQAc = new QAction("Hide log",GenMEn);

  ShowGenLogQAc = new QAction("Show/refresh action list",GenMEn);
  HideGenLogQAc = new QAction("Hide actions",GenMEn);

  ShowHideMEn->addAction(ShowGuessValQAc);
  ShowHideMEn->addAction(HideGuessValQAc);
  ShowHideMEn->addAction(ShowLogQAc);
  ShowHideMEn->addAction(HideLogQAc);
  ShowHideMEn->addAction(ShowGenLogQAc);
  ShowHideMEn->addAction(HideGenLogQAc);

  QMenu* DictionariesMEn = new QMenu("Dictionaries");
  GenMenuBar->addMenu(DictionariesMEn);


  LoadXmlDicQAc=new QAction("Load external dictionary from xml dictionary file",GenMEn);
  LoadDicFileQAc=new QAction("Load external dictionary from dictionary file",GenMEn);

  EmbedDicQAc = new QAction("Embed external dictionary",GenMEn);
  EditLoadedDicQAc = new QAction("Edit external dictionary",GenMEn);
  EditEmbeddedDicQAc = new QAction("Edit embedded dictionary",GenMEn);

  DictionariesMEn->addAction(LoadXmlDicQAc);
  DictionariesMEn->addAction(LoadDicFileQAc);
  DictionariesMEn->addSeparator();
  DictionariesMEn->addAction(EmbedDicQAc);
  DictionariesMEn->addSeparator();
  DictionariesMEn->addAction(EditLoadedDicQAc);
  DictionariesMEn->addAction(EditEmbeddedDicQAc);


  QMenu* ApplySaveMEn = new QMenu("Apply-Save");
  GenMenuBar->addMenu(ApplySaveMEn);

  GenFileQAc=new QAction("Create master file" ,GenMEn);

  TestRunQAc=new QAction("Test run",GenMEn);
  TestRunQAc->setCheckable(true);
  TestRunQAc->setChecked(false);

  ApplyToCurrentQAc=new QAction("Apply to current master file" ,GenMEn);
//  ApplyToZmfQAc=new QAction("Apply to existing master file",GenMEn);
//  ApplyToLoadedQAc=new QAction("Choose master file to apply" ,GenMEn);

  SaveToXmlQAc=new QAction("Save definition to xml file",GenMEn);

  ApplySaveMEn->addAction(GenFileQAc);
  ApplySaveMEn->addAction(ApplyToCurrentQAc);
//  ApplySaveMEn->addAction(ApplyToZmfQAc);
//  ApplySaveMEn->addAction(ApplyToLoadedQAc);
  ApplySaveMEn->addAction(TestRunQAc);
  ApplySaveMEn->addAction(SaveToXmlQAc);


  QMenu* ExportMEn = new QMenu("Data export",this);

  DataExportQAc=new QAction ("Export content",ExportMEn);
  DataImportQAc=new QAction ("Import content",ExportMEn);

  GenMenuBar->addMenu(ExportMEn);
  ExportMEn->addAction(DataExportQAc);
  ExportMEn->addAction(DataImportQAc);

  GenActionGroup = new QActionGroup(this);

  GenActionGroup->addAction(DataExportQAc);
  GenActionGroup->addAction(DataImportQAc);

  GenActionGroup->addAction(SearchDirQAc);
  GenActionGroup->addAction(IndexSearchDirQAc);
  GenActionGroup->addAction(SameAsMasterQAc);

  GenActionGroup->addAction(GenFileQAc);
  GenActionGroup->addAction(ApplyToCurrentQAc);
//  GenActionGroup->addAction(ApplyToLoadedQAc);

  GenActionGroup->addAction(SetupFromZmfQAc);
  GenActionGroup->addAction(QuitQAc);

  GenActionGroup->addAction(SetupFromZmfQAc);
  GenActionGroup->addAction(SetupFromXmlDicQAc);
  GenActionGroup->addAction(SetupFromDicFileQAc);
  GenActionGroup->addAction(SetupFromXmlDefQAc);

//  GenActionGroup->addAction(ApplyToZmfQAc);
  GenActionGroup->addAction(SaveToXmlQAc);

  GenActionGroup->addAction(KeyAppendRawQAc);
  GenActionGroup->addAction(KeyAppendFromEmbeddedDicQAc);
  GenActionGroup->addAction(KeyDeleteQAc);
  GenActionGroup->addAction(indexRebuildQAc);


  GenActionGroup->addAction(ShowGuessValQAc);
  GenActionGroup->addAction(HideGuessValQAc);

  GenActionGroup->addAction(ShowLogQAc);
  GenActionGroup->addAction(HideLogQAc);

  GenActionGroup->addAction(ShowGenLogQAc);
  GenActionGroup->addAction(HideGenLogQAc);

  GenActionGroup->addAction(EmbedDicQAc);
  GenActionGroup->addAction(EditLoadedDicQAc);
  GenActionGroup->addAction(EditEmbeddedDicQAc);

  GenActionGroup->addAction(LoadXmlDicQAc);
  GenActionGroup->addAction(LoadDicFileQAc);


  /* Definition Source */

  QGroupBox* wGBS= new QGroupBox( "Definition source" ,(QWidget*)this);
  QVBoxLayout* wVBS=new QVBoxLayout(wGBS);
  wGBS->setLayout(wVBS);
  QHBoxLayout* wHBS1=new QHBoxLayout(wGBS);
  wVBS->addLayout(wHBS1);
  SourceLBl = new QLabel("No definition source", wGBS);
  wHBS1->addWidget( SourceLBl,0,Qt::AlignCenter);

  DicEmbedLBl = new QLabel("No embedded dictionary",wGBS);
  wHBS1->addWidget( DicEmbedLBl,0,Qt::AlignCenter);
  DicLoadLBl = new QLabel("No loaded dictionary",wGBS);
  wHBS1->addWidget( DicLoadLBl,0,Qt::AlignCenter);

  SourceContentLBl = new QLabel("No source content",wGBS);
  wVBS->addWidget( SourceContentLBl,0,Qt::AlignCenter);

  VLYt->addWidget(wGBS);

  /* directory and basename */

  QGroupBox* wGBB= new QGroupBox( "File name and location" ,(QWidget*)this);
  QVBoxLayout* wVBB=new QVBoxLayout(wGBB);
  wGBB->setLayout(wVBB);
  QHBoxLayout* wHBDir = new QHBoxLayout;
  wVBB->addLayout(wHBDir);
  DirectoryLBl = new QLabel(wGBB);
  DirectoryLBl->setText(GeneralParameters.getWorkDirectory().toCChar());
  TargetDirectory = GeneralParameters.getWorkDirectory().toCChar();
  wHBDir->addWidget(DirectoryLBl);

  SearchDirBTn=new QPushButton( "Search",wGBB);
  SearchDirBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  wHBDir->addWidget(SearchDirBTn);

  QHBoxLayout* wHBB = new QHBoxLayout;
  wVBB->addLayout(wHBB);

  QLabel* wLbRoot=new QLabel(QObject::tr("Root name","FileGenerateDLg"),wGBB);
  wHBB->addWidget(wLbRoot);

  RootNameLEd = new QLineEdit(wGBB);

  if (DictionaryFile==nullptr) {
    RootNameLEd->setText("<no name>");
  }
  else if (DictionaryFile->DicName.isEmpty()) {
    RootNameLEd->setText("<no name>");
  } else {
    RootName = DictionaryFile->DicName;
    RootName.eliminateChar(' ');
    RootName = RootName.toLower();
    RootNameLEd->setText(RootName.toCChar());
  }
  RootNameLEd->setToolTip(QObject::tr("Root name of generated files (content files, header files).","FileGenerateDLg"));

  wHBB->addWidget(RootNameLEd);
  VLYt->addWidget(wGBB);

  /* fields */

  QGroupBox* wGB0 = new QGroupBox(QObject::tr("Master file values","FileGenerateDLg"),verticalLayoutWidget);
  QGridLayout* wGLYt=new QGridLayout ;
  wGLYt->setAlignment(Qt::AlignRight);
  wGB0->setLayout(wGLYt);

  QLabel* label0 = new QLabel((QObject::tr("Sizes","FileGenerateDLg")),verticalLayoutWidget);
  wGLYt->addWidget(label0,0,2);

  QLabel* label1 = new QLabel((QObject::tr("Mean record size (Block target size)","FileGenerateDLg")),verticalLayoutWidget);
  wGLYt->addWidget(label1,1,0);


  MeanSizeLEd = new QLineEdit(verticalLayoutWidget);
  MeanSizeLEd->setAlignment(Qt::AlignRight);
  MeanSizeLEd->setToolTip(QObject::tr("Average size of user record size in byte. This value will be automatically updated during file's lifecycle by ZRandom file engine.","FileGenerateDLg"));

  wGLYt->addWidget(MeanSizeLEd,1,2);

  ComputeBTn = new QPushButton(verticalLayoutWidget);
  ComputeBTn->setText(QObject::tr("ReCompute","FileGenerateDLg"));
  ComputeBTn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  wGLYt->addWidget(ComputeBTn,2,2);

  QLabel* label2 = new QLabel(QObject::tr("Allocated blocks","FileGenerateDLg"),verticalLayoutWidget);
  //  FLYt->setWidget(2,QFormLayout::LabelRole,label1);
  wGLYt->addWidget(label2,3,0);

  AllocatedLEd = new QLineEdit(verticalLayoutWidget);
  AllocatedLEd->setAlignment(Qt::AlignRight);
  //  FLYt->setWidget(2, QFormLayout::FieldRole, AllocatedLEd);
  wGLYt->addWidget(AllocatedLEd,3,1);

  AllocatedSizeLEd = new QLineEdit(verticalLayoutWidget);
  AllocatedSizeLEd->setAlignment(Qt::AlignRight);
  AllocatedSizeLEd->setEnabled(false);

  wGLYt->addWidget(AllocatedSizeLEd,3,2);

  QLabel* label3 = new QLabel(verticalLayoutWidget);
  label3->setText(QObject::tr("Block extent quota","FileGenerateDLg"));
  wGLYt->addWidget(label3,4,0);


  ExtentQuotaLEd = new QLineEdit(verticalLayoutWidget);
  ExtentQuotaLEd->setAlignment(Qt::AlignRight);
  ExtentQuotaLEd->setToolTip(QObject::tr("When file requires more space, number of blocks for each extent operation.","FileGenerateDLg"));

  wGLYt->addWidget(ExtentQuotaLEd,4,1);

  ExtentQuotaSizeLEd = new QLineEdit(verticalLayoutWidget);
  ExtentQuotaSizeLEd->setAlignment(Qt::AlignRight);
  ExtentQuotaSizeLEd->setEnabled(false);

  wGLYt->addWidget(ExtentQuotaSizeLEd,4,2);

  QLabel* label4 = new QLabel(verticalLayoutWidget);
  label4->setText(QObject::tr("Initial allocation (bytes)","FileGenerateDLg"));
  wGLYt->addWidget(label4,5,0);

  InitialSizeLEd = new QLineEdit(verticalLayoutWidget);
  InitialSizeLEd->setAlignment(Qt::AlignRight);
  InitialSizeLEd->setToolTip(QObject::tr("File is created with this initial byte size then logically truncated to zero.","FileGenerateDLg"));
  wGLYt->addWidget(InitialSizeLEd,5,2);


  VLYt->addWidget(wGB0);
  /* check boxes */

  QGroupBox* wGB1 = new QGroupBox(QObject::tr("Behavior parameters","FileGenerateDLg"),verticalLayoutWidget);
  QHBoxLayout* HLYt1 = new QHBoxLayout;
  HighWaterMarkingCHk = new QCheckBox(verticalLayoutWidget);
  HighWaterMarkingCHk->setText(QObject::tr("High warter marking","FileGenerateDLg"));
  HLYt1->addWidget(HighWaterMarkingCHk);

  GrabFreeSpaceCHk = new QCheckBox(verticalLayoutWidget);
  GrabFreeSpaceCHk->setText(QObject::tr("Grab free space","FileGenerateDLg"));
  HLYt1->addWidget(GrabFreeSpaceCHk);

  JournalingCHk = new QCheckBox(verticalLayoutWidget);
  JournalingCHk->setText(QObject::tr( "Journaling enabled","FileGenerateDLg"));
  HLYt1->addWidget(JournalingCHk);

  wGB1->setLayout(HLYt1);

  //  VLYt->insertLayout(1,HLYt1);
  VLYt->addWidget(wGB1);

  /* key table view widget */
  QGroupBox* wGB2 = new QGroupBox(QObject::tr("Index files parameters","FileGenerateDLg"));
  QVBoxLayout* HLYt2 = new QVBoxLayout;
  wGB2->setLayout(HLYt2);
  QHBoxLayout* wHB2Dir=new QHBoxLayout;
  IndexDirectoryLBl = new QLabel(verticalLayoutWidget);
  IndexDirectoryLBl->setText(__SAME_AS_MASTER__);

  SameAsMasterBTn=new QPushButton( "SameAsMaster",wGBB);
  SameAsMasterBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  wHB2Dir->addWidget(SameAsMasterBTn);

  wHB2Dir->addWidget(IndexDirectoryLBl);
  SearchIndexDirBTn=new QPushButton( "Search",wGBB);
  SearchIndexDirBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  wHB2Dir->addWidget(SearchIndexDirBTn);
  HLYt2->addLayout(wHB2Dir);

  KeyTBv = new ZQTableView(verticalLayoutWidget);
  HLYt2->addWidget(KeyTBv);

  VLYt->addWidget(wGB2);
  KeyTBv->setGeometry(QRect(0, 0, 490, 100));

  GuessGBx = new QGroupBox(QObject::tr("Guessed values","FileGenerateDLg"),verticalLayoutWidget);
  QVBoxLayout* wGuessVBx=new QVBoxLayout;
  GuessGBx->setLayout(wGuessVBx);


  HideGuessBTn=new QPushButton(verticalLayoutWidget);
  uriString wURIIC1;

  wURIIC1 = GeneralParameters.getIconDirectory();
  wURIIC1.addConditionalDirectoryDelimiter();
  wURIIC1 += "errorcross.png";
  CrossRedPXm.load(wURIIC1.toCChar());
  wURIIC1 = GeneralParameters.getIconDirectory();
  wURIIC1.addConditionalDirectoryDelimiter();
  wURIIC1 += "crossblue.gif";
  CrossBluePXm.load(wURIIC1.toCChar());
  HideGuessBTn->setIcon(CrossRedPXm);
  HideGuessBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  wGuessVBx->addWidget(HideGuessBTn);

  GuessTBv = new ZQTableView(verticalLayoutWidget);
  wGuessVBx->addWidget(GuessTBv);

  GuessGBx->setVisible(false);

  VLYt->addWidget(GuessGBx);

  verticalLayoutWidget->setContentsMargins(2, 2, 2, 2);

  SameAsMasterBTn->setVisible(false);

  QObject::connect(ComputeBTn, &QPushButton::clicked, this, &FileGenerateMWn::Compute);
  QObject::connect(SearchDirBTn, &QPushButton::clicked, this, &FileGenerateMWn::SearchDir);
  QObject::connect(SearchIndexDirBTn, &QPushButton::clicked, this, &FileGenerateMWn::SearchIndexDir);
  QObject::connect(SameAsMasterBTn, &QPushButton::clicked, this, &FileGenerateMWn::SameAsMaster);

  QObject::connect(HideGuessBTn, &QPushButton::clicked, this, &FileGenerateMWn::HideGuess);

  QObject::connect(RootNameLEd, &QLineEdit::editingFinished, this, &FileGenerateMWn::BaseNameEdit);
  QObject::connect(AllocatedLEd, &QLineEdit::editingFinished, this, &FileGenerateMWn::AllocatedEdit);
  QObject::connect(ExtentQuotaLEd, &QLineEdit::editingFinished, this, &FileGenerateMWn::ExtentQuotaEdit);
  QObject::connect(MeanSizeLEd, &QLineEdit::editingFinished, this, &FileGenerateMWn::MeanSizeEdit);

  QObject::connect(GenActionGroup, &QActionGroup::triggered, this, &FileGenerateMWn::MenuAction);

  QStandardItemModel* wModel=new QStandardItemModel(KeyTBv);
  KeyTBv->setModel(wModel);

  KeyTBv->ItemModel->setColumnCount(8);

  KeyTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem(tr("Name")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem(tr("Index base name")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem(tr("Guessed key size")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem(tr("Duplicates")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(4,new QStandardItem(tr("GrabFreeSpace")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(5,new QStandardItem(tr("Highwater")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(6,new QStandardItem(tr("Allocated")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(7,new QStandardItem(tr("Allocated Size")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(8,new QStandardItem(tr("Extent quota")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(9,new QStandardItem(tr("Extent Size")));


  ComboDelegate=new ZComboDelegate(ZEntity_SortType,KeyTBv);
  ComboDelegate->addItem( "ZST_Nothing");
  ComboDelegate->addItem( "ZST_NoDuplicates");
  ComboDelegate->addItem( "ZST_Duplicates");

  KeyTBv->setItemDelegateForColumn(3,ComboDelegate);

  KeyTBv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  KeyTBv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  GuessTBv->setModel(new QStandardItemModel(GuessTBv));
  GuessTBv->ItemModel->setColumnCount(4);
  GuessTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem(tr("Name")));
  GuessTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem(tr("Type")));
  GuessTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem(tr("Header size")));
  GuessTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem(tr("Guessed size (including header)")));

  ComLog=new textEditMWn((QWidget*)this,TEOP_CloseBtnHide | TEOP_NoFileLab,&ComLog);

//  ComLog->registerCloseCallback(std::bind(&FileGenerateMWn::closeComlogCB, this,std::placeholders::_1));
  ComLog->setWindowTitle("Communication log");
  ComLog->show();

  ErrorLog.setDisplayCallback(std::bind(&FileGenerateMWn::displayErrorCallBack, this,_1));
}

void
FileGenerateMWn::displayErrorCallBack(const utf8VaryingString& pMessage) {
  if (ComLog!=nullptr)
    ComLog->appendText(pMessage);
  else
    fprintf(stderr,pMessage.toCChar());
}

FileGenerateMWn::FileGenerateMWn(ZDictionaryFile* pDictionary, QWidget *parent) : QMainWindow(parent)
{
  utf8VaryingString wStr;
  DictionaryFile=pDictionary;

  KeyValues = new ZArray<KeyData>;
  initLayout();

  SourceLBl->setText("External dictionary file");
  if (pDictionary->URIDictionary.isEmpty())
    SourceContentLBl->setText("no dictionary file path");
  else {
    SourceContentLBl->setText(pDictionary->URIDictionary.toCChar());
  }
  dataSetupFromDictionary();
}

FileGenerateMWn::FileGenerateMWn( QWidget *parent) : QMainWindow(parent)
{
  KeyValues = new ZArray<KeyData>;
  initLayout();
//  LoadFromFile();
}

FileGenerateMWn::~FileGenerateMWn()
{
  if (ComboDelegate)
    delete ComboDelegate;
//  delete ui;
}

void
FileGenerateMWn::dataSetupFromDictionary() {

  utf8VaryingString wStr;

  if (MasterFile!=nullptr) {
    if (MasterFile->Dictionary!=nullptr) {
      if (MasterFile->getRecordCount()>0) {
          ZExceptionDLg::adhocMessage("Load dictionary",Severity_Error,
             nullptr,nullptr,
              "A master file with %ld active records has been loaded %s,\n"
              "This file has its own dictionary <%s>.\n"
              "It is requested to change its embedded dictionary.\n"
              "This change is not allowed since it could corrupt its content.\n"
              "In place, create a new master file with this dictionary and transfer data to it.",
              MasterFile->getRecordCount(),
              MasterFile->getURIContent().toString(),
              MasterFile->Dictionary->DicName.toString());
          return;
      } //if (MasterFile->getRecordCount()>0)

    int wRet=ZExceptionDLg::adhocMessage2B("Load dictionary",Severity_Information,
        "Quit","Aggree",nullptr,nullptr,
          "An empty master file has been loaded %s,\n"
          "This file has its own dictionary <%s>.\n"
          "Some master parameters might be changed.\n"
          "%ld keys will be created. Some may be redundant.\n"
          "Loading a new dictionary will request to change current dictionary with the loaded one.\n",
          MasterFile->getURIContent().toString(),
          MasterFile->Dictionary->DicName.toString(),
          MasterFile->Dictionary->KeyDic.count());
    if (wRet==QDialog::Rejected)
      return;
    }// if (MasterFile->Dictionary!=nullptr)
  } // if (MasterFile!=nullptr)

  wStr.sprintf("Loaded dictionary <%s>",DictionaryFile->DicName.toString());
  DicLoadLBl->setText(wStr.toCChar());


  if (MasterFile==nullptr) {
    AllocatedBlocks = cst_ZRF_default_allocation;
    wStr.sprintf("%ld",AllocatedBlocks);
    AllocatedLEd->setText(wStr.toCChar());


    ExtentQuota = cst_ZRF_default_extentquota;
    wStr.sprintf("%ld",ExtentQuota);
    ExtentQuotaLEd->setText(wStr.toCChar());

    GrabFreeSpace = true;
    HighWaterMarking = false;
  }
  for (long wi=0; wi < DictionaryFile->KeyDic.count() ; wi++) {

    ZKeyDictionary* wKeyDic=DictionaryFile->KeyDic.Tab(wi);

    KeyData wKeyData;
    wKeyData.IndexName = wKeyDic->DicKeyName;
    wKeyData.KeySize = wKeyDic->KeyGuessedSize;
    wKeyData.Forced = wKeyDic->Forced;
 //   wKeyData.KeySize = AllocatedBlocks*wKeyDic->computeKeyUniversalSize();
    wKeyData.Allocated = AllocatedBlocks;
    wKeyData.AllocatedSize = wKeyData.KeySize * wKeyData.Allocated;
    wKeyData.ExtentQuota = ExtentQuota;
    wKeyData.ExtentSize = wKeyData.KeySize * wKeyData.ExtentQuota;
    wKeyData.IndexRootName = generateIndexRootName(RootName,wKeyDic->DicKeyName);
    wKeyData.Duplicates   = wKeyDic->Duplicates;

    wKeyData.GrabFreeSpace = GrabFreeSpace ;
    wKeyData.HighwaterMarking = false ;
    long wR = KeyValues->push(wKeyData);
    if (MasterFile!=nullptr) {
      ZChangeRecord wChgRec(ZFGC_KeyDicAppend);
      wChgRec.setPostKeyData(wKeyData);
      wChgRec.setIndexRank (wR);
      ChangeLog.push(wChgRec);
      ComLog->appendText("Created ChangeLog record : new key <%s> waiting for creation.",
          wKeyData.IndexName.toString());
    }
    else {
      ComLog->appendText("New key <%s> appended. No change record created.",
          wKeyData.IndexName.toString());
    }

  }// for

  Compute(); /* recompute first mean record size and then all sizes */

  _dataSetup();

  QVariant wV;

  for (long wi=0;wi < DictionaryFile->count();wi++) {
    if (DictionaryFile->Tab(wi).UniversalSize==0) {
      QList<QStandardItem*> wGuessRow;
      wGuessRow << createItem(DictionaryFile->Tab(wi).getName());
      wGuessRow[0]->setEditable(false);
      wGuessRow << createItem(decode_ZType( DictionaryFile->Tab(wi).ZType));
      wGuessRow.last()->setEditable(false);
      wGuessRow << createItem(DictionaryFile->Tab(wi).HeaderSize);
      wGuessRow.last()->setEditable(false);
      wGuessRow << createItem(DictionaryFile->Tab(wi).UniversalSize);
      wGuessRow.last()->setEditable(true);

      GuessTBv->ItemModel->appendRow(wGuessRow);
      if (!GuessGBx->isVisible())
        GuessGBx->setVisible(true);

      wStr.sprintf( "%s-W-INVSIZ Size value for field <%s> has not been guessed and remains invalid.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wGuessRow[0]->text().toUtf8().data());
      ComLog->appendTextColor(WarnedQCl, wStr);
    }
  }// for

  testGuessed();

  for (int wi=0;wi < GuessTBv->ItemModel->columnCount();wi++)
    GuessTBv->resizeColumnToContents(wi);
  for (int wi=0;wi < GuessTBv->ItemModel->rowCount();wi++)
    GuessTBv->resizeRowToContents(wi);

  QObject::connect(GuessTBv->ItemModel,&QStandardItemModel::itemChanged,this,&FileGenerateMWn::GuessItemChanged);

  State |= FGST_FDic;
} // dataSetupFromDictionary


bool
FileGenerateMWn::dataSetupFromMasterFile(const uriString& pURIMaster) {
  utf8VaryingString wStr;
  wStr.sprintf("Loading parameters from existing master file <%s>.",pURIMaster.toString());
  ComLog->appendText(wStr);

  if (ChangeLog.count()!=0) {
    ZExceptionDLg::adhocMessage("Load master file",Severity_Error,
        nullptr,nullptr,
        "There are pending changes (count %ld)"
        "concerning master file %s,\n"
        "Apply changes first then load another current file.\n",
        ChangeLog.count(),
        MasterFile->getURIContent().toString());
    return false;
  }
  if (MasterFile!=nullptr) {
      int wRet=ZExceptionDLg::adhocMessage2B("Load master file",Severity_Warning,
          "Do not load","Load anyway",nullptr,nullptr,
          "A master file has been loaded :%s,\n"
          "Loading a new file will disregard current changes for this file.\n",
          MasterFile->getURIContent().toString());
      if (wRet==QDialog::Rejected)
        return false;

    ComLog->appendTextColor(QColor(),WarnedQCl, "%s-W-ALRDYLD A file has already being loaded/created <%s>.",
        ZDateFull::currentDateTime().toFormatted().toString(),
        MasterFile->getURIContent().toString());

    if (MasterFile->isOpen()) {
      ComLog->appendText("Closing current file prior of loading new master file.");
      MasterFile->zclose();
    }
    delete MasterFile;
  }
  MasterFile = new ZMasterFile;
  ZStatus wSt=MasterFile->zopen(pURIMaster,ZRF_All | ZRF_TypeRegardless);
  if (wSt < 0) {
    ComLog->appendText("Cannot open requested file.");
    ZExceptionDLg::displayLast("Opening Master file");
    ComLog->appendTextColor(ErroredQCl, "%s-E-CANTLD Cannot load requested file.\n%s",
        ZDateFull::currentDateTime().toFormatted().toString(),ZException.last().formatFullUserMessage().toString());
    delete MasterFile;
    MasterFile = nullptr;
    return false;
  }

  if (MasterFile->hasDictionary()) {
    SourceLBl->setText("Master file");
    DicEmbedLBl->setText(MasterFile->Dictionary->DicName.toCChar());
    wStr.sprintf("Loaded file has an associated dictionary <%s>.",MasterFile->Dictionary->DicName.toString());
    ComLog->appendText(wStr);
    if (MasterFile->getFileType()!= ZFT_ZMasterFile ) {
      ComLog->appendTextColor(QColor(),WarnedQCl, "%s-W-BADTYP File has a dictionary, though its file type is %s",
          ZDateFull::currentDateTime().toFormatted().toString(),
          decode_ZFile_type(MasterFile->getFileType()));
    }
    if (DictionaryFile!=nullptr) {
      ComLog->appendText("Currently loaded external dictionary %s is made unavailable.",
          DictionaryFile->URIDictionary.toString());
      delete DictionaryFile;
      DicLoadLBl->setText("No loaded dictionary");
    }

    wStr.sprintf("Embedded Dictionary <%s>",MasterFile->Dictionary->DicName.toCChar());
    DicEmbedLBl->setText(wStr.toCChar());
  }
  else {
    SourceLBl->setText("Raw master file");
    ComLog->appendText("Loaded file has no dictionary and is a raw master file.");
  }

  SourceContentLBl->setText(pURIMaster.toCChar());

  TargetDirectory = MasterFile->getURIContent().getDirectoryPath();
  IndexDirectory = MasterFile->IndexFilePath;

  RootName = MasterFile->getURIContent().getRootname();
  MeanRecordSize = MasterFile->getFCB()->BlockTargetSize;
  AllocatedBlocks = MasterFile->getFCB()->AllocatedBlocks;
  AllocatedSize = MasterFile->getFileSize();
  ExtentQuota = MasterFile->getFCB()->BlockExtentQuota;
  ExtentQuotaSize = MasterFile->getFCB()->BlockExtentQuota*MasterFile->getFCB()->BlockTargetSize;

  InitialSize= MasterFile->getFCB()->InitialSize;

  GrabFreeSpace = MasterFile->getFCB()->GrabFreeSpace;
  HighWaterMarking = MasterFile->getFCB()->HighwaterMarking;
  Journaling = MasterFile->hasJournal();

  KeyTBv->ItemModel->removeRows(0,KeyTBv->ItemModel->rowCount());

  KeyData wKeyData;
  KeyValues->clear();

  for (long wi=0; wi < MasterFile->IndexTable.count(); wi++) {
    wKeyData.clear();
 //   uriString wIndexURI = MasterFile->getURIIndex(wi);
    wKeyData.IndexRootName = MasterFile->getURIIndex(wi).getRootname();
    wKeyData.IndexName = MasterFile->IndexTable[wi]->IndexName ;
    wKeyData.KeySize = MasterFile->IndexTable[wi]->KeyGuessedSize;
    wKeyData.Allocated = MasterFile->IndexTable[wi]->getFCB()->AllocatedBlocks;
    wKeyData.AllocatedSize = wKeyData.KeySize * wKeyData.Allocated;
    wKeyData.ExtentQuota = MasterFile->IndexTable[wi]->getFCB()->BlockExtentQuota;
    wKeyData.ExtentSize = wKeyData.ExtentQuota * wKeyData.KeySize;
    wKeyData.Duplicates   = MasterFile->IndexTable[wi]->Duplicates;
    wKeyData.GrabFreeSpace   = MasterFile->IndexTable[wi]->getFCB()->GrabFreeSpace;
    wKeyData.HighwaterMarking   = MasterFile->IndexTable[wi]->getFCB()->HighwaterMarking;

    KeyValues->push(wKeyData);
  }

  _dataSetup();

  return true;
} // dataSetupFromFile




ZStatus FileGenerateMWn::XmlDefinitionSave(uriString &pXmlFile, bool pComment) {
  utf8VaryingString wReturn = fmtXMLdeclaration();
  int wLevel=0;
  wReturn += fmtXMLnodeWithAttributes("zicm","version",__ZRF_XMLVERSION_CONTROL__,0);

  /*----------------------Content---------------------------------*/
  wReturn += fmtXMLnode("filedefinition",wLevel);
  wLevel++;

  if (TargetDirectory.isEmpty())
    wReturn+=fmtXMLchar("targetdirectory"," ",wLevel);
    else
    wReturn+=fmtXMLchar("targetdirectory",TargetDirectory.toCChar(),wLevel);

  if (IndexDirectory.isEmpty())
      wReturn+=fmtXMLchar("indexdirectory"," ",wLevel);
    else
      wReturn+=fmtXMLchar("indexdirectory",IndexDirectory.toCChar(),wLevel);

  if (RootName.isEmpty())
      wReturn+=fmtXMLchar("rootname"," ",wLevel);
    else
      wReturn+=fmtXMLchar("rootname",RootName.toCChar(),wLevel);


  wReturn+=fmtXMLlong("meanrecordsize",MeanRecordSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Average size of record (varying record size). Updated by file record engine.");

  wReturn+=fmtXMLlong("allocatedblocks",AllocatedBlocks,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Number of allocated blocks in block access table.");
  wReturn+=fmtXMLlong("allocatedsize",AllocatedSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Size in bytes allocated to these blocks.");

  wReturn+=fmtXMLlong("extentquota",ExtentQuota,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Number of blocks extended for each file extent operation.");

  wReturn+=fmtXMLlong("extentquotasize",ExtentQuotaSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Size in bytes extended for each file extent operation.");

  wReturn+=fmtXMLlong("initialblocks",InitialBlocks,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Number of blocks initially allocated to file during its creation.");

  wReturn+=fmtXMLlong("initialsize",InitialSize,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Size in bytes initially allocated to file during its creation.");

  wReturn+=fmtXMLbool("grabfreespace",GrabFreeSpace,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Defines wether engine should try to gather free space once deallocated.");

  wReturn+=fmtXMLbool("highwatermarking",HighWaterMarking,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Defines wether engine should binary zero any freed space.");

  wReturn+=fmtXMLbool("journaling",Journaling,wLevel);
  if (pComment)
    fmtXMLaddInlineComment(wReturn,"Defines if journaling is set or not (NB: indexes are never journalized).");


  /*     --keys--   */
  if (KeyValues!=nullptr) {
    wReturn += fmtXMLnode("keyvalues",wLevel);
    wLevel++;
    for (long wi=0; wi < KeyValues->count();wi++) {
      wReturn += fmtXMLnode("keyitem",wLevel);
      wLevel++;

      wReturn+=fmtXMLlong("keysize",KeyValues->Tab(wi).KeySize,wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"Total size in bytes of the key or guessed key size if key is composed of one or more varying fields.");
      wReturn+=fmtXMLlong("allocated",KeyValues->Tab(wi).Allocated,wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"Number of allocated block in index file.");
      wReturn+=fmtXMLlong("allocatedsize",KeyValues->Tab(wi).AllocatedSize,wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"Total size in bytes of the key or guessed key size if key is composed of one or more varying fields.");
      wReturn+=fmtXMLlong("extentquota",KeyValues->Tab(wi).ExtentQuota,wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"Total number of blocks index file will be extended at each extent operation.");
      wReturn+=fmtXMLlong("extentsize",KeyValues->Tab(wi).ExtentSize,wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"Total size in bytes index file will be extended at each extent operation.");
      wReturn+=fmtXMLchar("indexrootname",KeyValues->Tab(wi).IndexRootName.toString(),wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"Root name for building index file name.It is concatenated with embedded index directory if defined at master file level or with master file's default directory if none.");
      wReturn+=fmtXMLchar("indexname",KeyValues->Tab(wi).IndexName.toString(),wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"Name for the key - index.");
      wReturn+=fmtXMLchar("duplicate",decode_ZST(KeyValues->Tab(wi).Duplicates),wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"Defines wether key accepts or not duplicates.");
      wReturn+=fmtXMLbool("grabfreespace",bool(KeyValues->Tab(wi).GrabFreeSpace),wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"If set file engine will try to gather free blocks and holes together.Not usefull is key has a fixed size.");
      wReturn+=fmtXMLbool("highwatermarking",bool(KeyValues->Tab(wi).HighwaterMarking),wLevel);
      if (pComment)
        fmtXMLaddInlineComment(wReturn,"If set each deleted block from the key will be set to binary zero by file engine.This is not much relevant because of time consummed by operation vs confidentiality.");

      wLevel--;
      wReturn += fmtXMLendnode("keyitem",wLevel);
    } // for

    wLevel--;
    wReturn += fmtXMLendnode("keyvalues",wLevel);
  } //  if (KeyValues!=nullptr)
  /*     --end keys--   */

  wLevel--;
  wReturn += fmtXMLendnode("filedefinition",wLevel);
  wLevel--;
  /*----------------------End Content---------------------------------*/
  wReturn += fmtXMLendnode("zicm",0);

  return pXmlFile.writeContent(wReturn);
}// FileGenerateDLg::XmlSave


ZStatus
FileGenerateMWn::XmlDefinitionLoad(const utf8VaryingString& pXmlContent, ZaiErrors* pErrorLog) {

  MasterFileValues wZMFVal;

  ZStatus wSt;

  zxmlDoc *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;
  zxmlElement *wRootNode=nullptr;
  zxmlElement *wKeyRootNode=nullptr;
  zxmlElement *wSingleKeyNode=nullptr;
  zxmlElement *wSwapNode=nullptr;

  utf8VaryingString wValue;

  pErrorLog->setContext("FileGenerateDLg::XmlLoad");

  wDoc = new zxmlDoc;
  wSt=wDoc->XmlParseFromMemory(pXmlContent,pErrorLog);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    pErrorLog->logZExceptionLast("FileGenerateMWn::XmlDefinitionLoad");
    return wSt;
  }
  if (!(wRoot->getName() == "zicm")) {
    pErrorLog->errorLog(
        "FileGenerateDLg::XmlLoad-E-INVROOT Invalid root node name <%s> expected <zicm>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }


  /*----------------------Master File Values Content---------------------------------*/

  wSt=wRoot->getChildByName((zxmlNode*&)wRootNode,"filedefinition");
  if (wSt!=ZS_SUCCESS)
  {
    pErrorLog->logZStatus(
        ZAIES_Error,
        wSt,
        "FileGenerateDLg::XmlLoadE-CNTFINDND Error cannot find node element with name <%s> status <%s>",
        "filedefinition",
        decode_ZStatus(wSt));
    return wSt;
  }
  XMLgetChildText(wRootNode,"targetdirectory",wZMFVal.TargetDirectory,pErrorLog,ZAIES_Warning);
  XMLgetChildText(wRootNode,"indexdirectory",wZMFVal.IndexDirectory,pErrorLog,ZAIES_Warning);
  XMLgetChildText(wRootNode,"rootname",wZMFVal.RootName,pErrorLog,ZAIES_Warning);

  XMLgetChildLong(wRootNode,"meanrecordsize",(long&)wZMFVal.MeanRecordSize,pErrorLog,ZAIES_Warning);
  XMLgetChildLong(wRootNode,"allocatedblocks",(long&)wZMFVal.AllocatedBlocks,pErrorLog,ZAIES_Warning);
  XMLgetChildLong(wRootNode,"allocatedsize",(long&)wZMFVal.AllocatedSize,pErrorLog,ZAIES_Warning);
  XMLgetChildLong(wRootNode,"extentquota",(long&)wZMFVal.ExtentQuota,pErrorLog,ZAIES_Warning);
  XMLgetChildLong(wRootNode,"extentquotasize",(long&)wZMFVal.ExtentQuotaSize,pErrorLog,ZAIES_Warning);
  XMLgetChildLong(wRootNode,"initialblocks",(long&)wZMFVal.InitialBlocks,pErrorLog,ZAIES_Warning);
  XMLgetChildLong(wRootNode,"initialsize",(long&)wZMFVal.InitialSize,pErrorLog,ZAIES_Warning);

  XMLgetChildBool(wRootNode,"journaling",wZMFVal.Journaling,pErrorLog,ZAIES_Warning);
  XMLgetChildBool(wRootNode,"highwatermarking",wZMFVal.HighWaterMarking,pErrorLog,ZAIES_Warning);
  XMLgetChildBool(wRootNode,"grabfreespace",wZMFVal.GrabFreeSpace,pErrorLog,ZAIES_Warning);

  wSt=wRootNode->getChildByName((zxmlNode*&)wKeyRootNode,"keyvalues");
  if (wSt==ZS_SUCCESS)
    wSt=wKeyRootNode->getFirstChild((zxmlNode*&)wSingleKeyNode);
  while (wSt==ZS_SUCCESS)
  {
    KeyData wKeyD;
    XMLgetChildLong(wSingleKeyNode,"keysize",(long&)wKeyD.KeySize,pErrorLog,ZAIES_Warning);
    XMLgetChildLong(wSingleKeyNode,"allocated",(long&)wKeyD.Allocated,pErrorLog,ZAIES_Warning);
    XMLgetChildLong(wSingleKeyNode,"allocatedsize",(long&)wKeyD.AllocatedSize,pErrorLog,ZAIES_Warning);
    XMLgetChildLong(wSingleKeyNode,"extentquota",(long&)wKeyD.ExtentQuota,pErrorLog,ZAIES_Warning);
    XMLgetChildLong(wSingleKeyNode,"extentsize",(long&)wKeyD.ExtentSize,pErrorLog,ZAIES_Warning);

 //   XMLgetChildBool(wSingleKeyNode,"duplicate",wKeyD.Duplicates,pErrorLog,ZAIES_Warning);
    XMLgetChildText(wSingleKeyNode,"duplicate",wValue,pErrorLog,ZAIES_Warning);
    wKeyD.Duplicates = encode_ZST(wValue.toCChar());
    if (wKeyD.Duplicates==ZST_Nothing)
      wKeyD.Duplicates = ZST_NoDuplicates;
    XMLgetChildBool(wSingleKeyNode,"grabfreespace",wKeyD.GrabFreeSpace,pErrorLog,ZAIES_Warning);
    XMLgetChildBool(wSingleKeyNode,"highwatermarking",wKeyD.HighwaterMarking,pErrorLog,ZAIES_Warning);

    XMLgetChildText(wSingleKeyNode,"indexname",wKeyD.IndexName,pErrorLog,ZAIES_Warning);
    XMLgetChildText(wSingleKeyNode,"indexrootname",wKeyD.IndexRootName,pErrorLog,ZAIES_Warning);

    if (wZMFVal.KeyValues==nullptr) {
      wZMFVal.KeyValues=new zbs::ZArray<KeyData>;
    }
    wZMFVal.KeyValues->push(wKeyD);

    wSt=wSingleKeyNode->getNextNode((zxmlNode*&)wSwapNode);
    XMLderegister(wSingleKeyNode);
    wSingleKeyNode=wSwapNode;
  }// while

  //-------------------------------------------------------------

  XMLderegister(wKeyRootNode);
  XMLderegister((zxmlNode *&) wRootNode);
  XMLderegister((zxmlNode *&) wRoot);


  MasterFileValues::_copyFrom(wZMFVal);

  return ZS_SUCCESS;
} // FileGenerateDLg::XmlLoad

void
FileGenerateMWn::_refresh() {
  GrabFreeSpace = GrabFreeSpaceCHk->isChecked();
  HighWaterMarking = HighWaterMarkingCHk->isChecked();
  Journaling = JournalingCHk->isChecked();
}

void
FileGenerateMWn::_dataSetup() {
  utf8VaryingString wStr;

  DirectoryLBl->setText(TargetDirectory.toCChar());
  if (IndexDirectory.isEmpty()) {
    IndexDirectoryLBl->setText(__SAME_AS_MASTER__);
    SameAsMasterBTn->setVisible(false);
    SameAsMasterQAc->setEnabled(false);
  }
  else {
    IndexDirectoryLBl->setText(IndexDirectory.toCChar());
    SameAsMasterBTn->setVisible(true);
    SameAsMasterQAc->setEnabled(true);
  }
  RootNameLEd->setText(RootName.toCChar());
  wStr.sprintf("%ld",MeanRecordSize);
  MeanSizeLEd->setText(wStr.toCChar());
  wStr.sprintf("%ld",AllocatedBlocks);
  AllocatedLEd->setText(wStr.toCChar());
  wStr.sprintf("%ld",ExtentQuota);
  ExtentQuotaLEd->setText(wStr.toCChar());
  wStr.sprintf("%ld",ExtentQuotaSize);
  ExtentQuotaSizeLEd->setText(wStr.toCChar());
  wStr.sprintf("%ld",AllocatedSize);
  AllocatedSizeLEd->setText(wStr.toCChar());
  wStr.sprintf("%ld",InitialSize);
  InitialSizeLEd->setText(wStr.toCChar());

  HighWaterMarkingCHk->setCheckState(HighWaterMarking?Qt::Checked:Qt::Unchecked);
  GrabFreeSpaceCHk->setCheckState(GrabFreeSpace?Qt::Checked:Qt::Unchecked);
  JournalingCHk->setCheckState(Journaling?Qt::Checked:Qt::Unchecked);

  KeyTBv->ItemModel->removeRows(0,KeyTBv->ItemModel->rowCount());

  QList<QStandardItem*> wKeyRow;

  if (KeyValues!=nullptr) {
    for (long wi=0; wi < KeyValues->count(); wi++) {
      wKeyRow=formatKeyRow(KeyValues->Tab(wi));
      KeyTBv->ItemModel->appendRow(wKeyRow);
    }
    for (int wi=0; wi < KeyTBv->ItemModel->columnCount(); wi++)
      KeyTBv->resizeColumnToContents(wi);
    for (int wi=0; wi < KeyTBv->ItemModel->rowCount(); wi++)
      KeyTBv->resizeRowToContents(wi);
  }// KeyValues

  ComputeBTn->setVisible(false);

  QObject::connect(KeyTBv->ItemModel,&QStandardItemModel::itemChanged,this,&FileGenerateMWn::KeyItemChanged);
}

QList<QStandardItem*>
FileGenerateMWn::formatKeyRow(KeyData& pKey) {
  utf8VaryingString wStr;
  QList<QStandardItem*> wKeyRow;
  wKeyRow.clear();
  wKeyRow << new QStandardItem(pKey.IndexName.toCChar());
  wKeyRow << new QStandardItem(pKey.IndexRootName.toCChar());
  wStr.sprintf("%ld",pKey.KeySize);
  wKeyRow.push_back(new QStandardItem(wStr.toCChar()));
  wKeyRow.last()->setTextAlignment(Qt::AlignRight);
#ifdef __DEPRECATED__
  QStandardItem* wDup=new QStandardItem("Duplicates");
  wDup->setTextAlignment(Qt::AlignLeft);
  wDup->setCheckable(true);

  if (pKey.Duplicates) {
    wDup->setCheckState(Qt::Checked);
    wDup->setText("Duplicates");
  }
  else {
    wDup->setCheckState(Qt::Unchecked);
    wDup->setText("No duplicates");
  }
#endif

  QStandardItem* wDup = ComboDelegate->generateItemWithIndex(int(pKey.Duplicates));
  QStandardItem* wGrab=new QStandardItem("GrabFreeSpace");
  wGrab->setCheckable(true);
  if (pKey.GrabFreeSpace) {
    wGrab->setCheckState(Qt::Checked);
  }
  else {
    wGrab->setCheckState(Qt::Unchecked);
  }
  QStandardItem* wHigh=new QStandardItem("Highwater");
  wHigh->setCheckable(true);
  if (pKey.HighwaterMarking) {
    wHigh->setCheckState(Qt::Checked);
  }
  else {
    wHigh->setCheckState(Qt::Unchecked);
  }

  wDup->setEditable(true);
  wKeyRow.push_back(wDup);

  wGrab->setEditable(true);
  wKeyRow.push_back(wGrab);

  wHigh->setEditable(true);
  wKeyRow.push_back(wHigh);

  wKeyRow << createItemAligned(pKey.Allocated,Qt::AlignRight,"%ld");
  wKeyRow.last()->setEditable(true);

  wKeyRow << createItemAligned(pKey.AllocatedSize,Qt::AlignRight,"%ld");
  wKeyRow.last()->setEditable(false);

  wKeyRow << createItemAligned(pKey.ExtentQuota,Qt::AlignRight,"%ld");
  wKeyRow.last()->setEditable(true);

  wKeyRow << createItemAligned(pKey.ExtentSize,Qt::AlignRight,"%ld");
  wKeyRow.last()->setEditable(false);

  return wKeyRow;
}

bool
FileGenerateMWn::dataSetupFromXmlDefinition(const uriString& pXmlFile) {
  utf8VaryingString  wXmlContent;

  ZStatus wSt=pXmlFile.loadUtf8(wXmlContent);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Loading Xml definition");
    return false;
  }

  SourceLBl->setText("Xml file definition");
  SourceContentLBl->setText(pXmlFile.toCChar());

  ErrorLog.setAutoPrintOn(ZAIES_Info);

  wSt=XmlDefinitionLoad(wXmlContent,&ErrorLog);
  if (wSt!=ZS_SUCCESS)
    return false;

  _dataSetup();
  return true;
} // dataSetupFromXmlDefinition

void
FileGenerateMWn::SearchDir(){

  QString wDir;
  if (IndexDirectory.isEmpty())
    wDir = QFileDialog::getExistingDirectory(this,"Target master directory",GeneralParameters.getWorkDirectory().toCChar());
  else
    wDir = QFileDialog::getExistingDirectory(this,"Target master directory",TargetDirectory.toCChar());

  if (wDir.isEmpty())
    return;

  TargetDirectory = wDir.toUtf8().data();
  DirectoryLBl->setText(TargetDirectory.toCChar());
  return;
}//FileGenerateDLg::SearchDir

void
FileGenerateMWn::SearchIndexDir(){
/*
  if (IndexDirectory.isEmpty())
    IndexDirectory = TargetDirectory;
*/

  QString wDir;
  if (IndexDirectory.isEmpty())
    wDir = QFileDialog::getExistingDirectory(this,"Target Indexes directory",TargetDirectory.toCChar());
  else
    wDir = QFileDialog::getExistingDirectory(this,"Target Indexes directory",IndexDirectory.toCChar());

  if (wDir.isEmpty())
    return;


  SameAsMasterBTn->setVisible(true);
  SameAsMasterQAc->setEnabled(true);

  IndexDirectory = wDir.toUtf8().data();
  IndexDirectoryLBl->setText(IndexDirectory.toCChar());
  return;
}//FileGenerateDLg::SearchDir



void
FileGenerateMWn::SameAsMaster(){

  IndexDirectory.clear();
  IndexDirectoryLBl->setText(__SAME_AS_MASTER__);

  SameAsMasterBTn->setVisible(false);
  SameAsMasterQAc->setEnabled(false);

  return;
}//FileGenerateDLg::SameAsMaster



void
FileGenerateMWn::DataImport(){
  ZStatus wSt=ZS_SUCCESS;
  if (MasterFile==nullptr) {
    ZExceptionDLg::adhocMessage("Data import",Severity_Error,
                                "Please select an active, valid master file before feeding its data.");
    return;
  }
  if (MasterFile->getRecordCount() > 0) {
    int wRet=ZExceptionDLg::adhocMessage2B("Data import",Severity_Error, "Cancel","Confirm",&ErrorLog,nullptr,
                                "There are existing records within selected master file.\n"
                                "This may cause further malfunctions.\n"
                                "Please confirm import <Confirm> or cancel operation <Cancel>");
    if (wRet==QDialog::Rejected)
      return;
  }

  QString wDir;
  wDir = QFileDialog::getOpenFileName(this,"Import content",GeneralParameters.getWorkDirectory().toCChar(),"Xml files (*.xml);;All (*.*)");
  if (wDir.isEmpty())
    return;
  uriString wURIImport=wDir.toUtf8().data();

  /* export progress window setup */

  ProgressMWn=new ZMFProgressMWn("Data import",this,false);

  ProgressMWn->setDescBudy("File");
  ProgressMWn->setDescText(wURIImport);

  ProgressMWn->advanceSetupCallBack(MasterFile->getRecordCount(),"Data import");

  MasterFile->registerProgressSetupCallBack(std::bind(&ZMFProgressMWn::advanceSetupCallBack, ProgressMWn,_1,_2));
  MasterFile->registerProgressCallBack(std::bind(&ZMFProgressMWn::advanceCallBack, ProgressMWn,_1,_2));

  ProgressMWn->show();

  if (!MasterFile->isOpen())
    wSt=MasterFile->zopen(ZRF_All);

  wSt=MasterFile->XmlImportContentByChunk( wURIImport,ZEXOP_CheckName | ZEXOP_CheckZType, &ErrorLog);

  ProgressMWn->setDone(wSt!=ZS_SUCCESS);

  if (wSt!=ZS_SUCCESS) {
    ErrorLog.errorLog("Data import: Something went wrong while importing data from <%s>",MasterFile->getURIContent().getBasename().toCChar());
    return;
  }

  return;
} // DataImport

void
FileGenerateMWn::DataExport(){
  ZStatus wSt=ZEntryPoint::exportZMF(this,MasterFile);
  return;
}//FileGenerateDLg::DataExport



void
FileGenerateMWn::HideGuess() {
  GuessGBx->setVisible(false);
  return;
}//FileGenerateDLg::HideGuess

void
FileGenerateMWn::Quit(){

  if (ChangeLog.count() > 0) {
    int wRet=ZExceptionDLg::adhocMessage2B("Quit requested",Severity_Information,
        "Stay","Quit anyway",nullptr,nullptr,
        "There are %ld changes that haven't been applied.\n"
        "If you quit now changes will be lost.",ChangeLog.count());
    if (wRet==QDialog::Rejected)
      return;
  }
  this->hide();
  this->deleteLater();
  return;
}//FileGenerateDLg::Discard

void
FileGenerateMWn::BaseNameEdit(){

  RootName = RootNameLEd->text().toUtf8().data() ;

  RootName.eliminateChar(' ');
  RootNameLEd->setText(RootName.toCChar());

    return;
}//FileGenerateDLg::BaseNameEdit

void
FileGenerateMWn::AllocatedEdit(){
  utf8VaryingString wStr;
  wStr = AllocatedLEd->text().toUtf8().data();
  AllocatedBlocks = getValueFromString<size_t>(wStr) ;

  AllocatedSize = AllocatedBlocks * MeanRecordSize;
  wStr.sprintf("%ld",AllocatedSize);
  AllocatedSizeLEd->setText(wStr.toCChar());

  return;
}//FileGenerateDLg::AllocatedEdit


void
FileGenerateMWn::MeanSizeEdit(){
  utf8VaryingString wStr;
  wStr = MeanSizeLEd->text().toUtf8().data();
  MeanRecordSize = getValueFromString<size_t>(wStr) ;

  AllocatedSize = AllocatedBlocks * MeanRecordSize;
  wStr.sprintf("%ld",AllocatedSize);
  AllocatedSizeLEd->setText(wStr.toCChar());

  ExtentQuotaSize = ExtentQuota * MeanRecordSize;
  wStr.sprintf("%ld",ExtentQuotaSize);
  ExtentQuotaSizeLEd->setText(wStr.toCChar());

  return;
}//FileGenerateDLg::MeanSizeEdit

void
FileGenerateMWn::ExtentQuotaEdit(){
  utf8VaryingString wStr;
  wStr = ExtentQuotaLEd->text().toUtf8().data();
  ExtentQuota = getValueFromString<size_t>(wStr) ;

  ExtentQuotaSize = ExtentQuota * MeanRecordSize;
  wStr.sprintf("%ld",ExtentQuotaSize);
  ExtentQuotaSizeLEd->setText(wStr.toCChar());

  return;
}//FileGenerateDLg::MeanSizeEdit


void
FileGenerateMWn::GuessItemChanged(QStandardItem* pItem){
  utf8VaryingString wStr;
  static bool wFTwice=false;
  if (wFTwice==true) {
    wFTwice=false;
    return;
  }
  wFTwice=true;
  QVariant wV;
  wStr = pItem->text().toUtf8().data();
  wV.setValue<size_t>(getValueFromString<size_t>(wStr));
  pItem->setData(wV,Qt::UserRole);

  QStandardItem* wItem = GuessTBv->ItemModel->item(pItem->row(),0);

  wStr.sprintf( "%s Value for field <%s> has been guessed to <%ld>.",
      ZDateFull::currentDateTime().toFormatted().toString(),
      wItem->text().toUtf8().data(),
      getValueFromString<size_t>(wStr) );
  ComLog->appendText(wStr);

  Compute();

  testGuessed();

  return;
}//FileGenerateDLg::GuessItemChanged


void
FileGenerateMWn::KeyItemChanged(QStandardItem* pItem){

  if (DoNotChangeKeyValues) {
    DoNotChangeKeyValues=false;
    return;
  }

  utf8VaryingString wStr;

  QVariant wV;
  wStr = pItem->text().toUtf8().data();
  size_t wValue=getValueFromString<size_t>(wStr);

  QStandardItem* wMainKeyItem = KeyTBv->ItemModel->item(pItem->row(),0);

  const char* wName=nullptr;
  int wCol = pItem->column();

  switch (wCol) {

  case cst_IndexNameCol: {
    utf8VaryingString wName = pItem->text().toUtf8().data() ;

    for (long wi=0; wi < KeyValues->count(); wi++) {
      if (wName == KeyValues->Tab(wi).IndexName) {
        ZExceptionDLg::adhocMessage("Change index name",Severity_Error,nullptr,nullptr,
            "An index with name <%s> already exists in index table.\n"
            "Cannot add this index to index table.",wName.toString());
        DoNotChangeKeyValues = true;
        pItem->setText(KeyValues->Tab(pItem->row()).IndexName.toCChar());
        return;
      }
    }

    /* first setup change log with values ex-ante and ex-post */
    ZChangeRecord wChgRec(ZFGC_INameChange);
    wChgRec.setChangeKey(wName);
    wChgRec.setAnteString(KeyValues->Tab(pItem->row()).IndexName);
    wChgRec.setPostString(wName);
    wChgRec.setIndexRank (pItem->row());
    ChangeLog.push(wChgRec);

    KeyValues->Tab(pItem->row()).IndexName = wName ;

    wStr.sprintf( "%s Key rank %d index name has been changed to <%s>. col %d",
          ZDateFull::currentDateTime().toFormatted().toString(),
          pItem->row(),
          wMainKeyItem->text().toUtf8().data() ,
        wCol);

    ComLog->appendText(wStr);
    DoNotChangeKeyValues = false;
    return ;
  } // cst_IndexNameCol

  case cst_KeyDupCol: {

    /* first setup change log with values ex-ante and ex-post */
    ZChangeRecord wChgRec(ZFGC_ChgDuplicate);
    wChgRec.setChangeKey(KeyValues->Tab(pItem->row()).IndexName);
    wChgRec.setAnteZSortType(KeyValues->Tab(pItem->row()).Duplicates);
    QVariant wV = pItem->data(ZQtDataReference);
    ZDataReference wDRef=wV.value<ZDataReference>();

    KeyValues->Tab(pItem->row()).Duplicates= (ZSort_Type)wDRef.DataRank;
    wChgRec.setPostZSortType(KeyValues->Tab(pItem->row()).Duplicates);
    wChgRec.setIndexRank (pItem->row());
    ChangeLog.push(wChgRec);

//    KeyValues->Tab(pItem->row()).Duplicates = (pItem->checkState()==Qt::Checked)?ZST_NoDuplicates:ZST_Duplicates  ;
    wV=pItem->data(ZQtDataReference);
    KeyValues->Tab(pItem->row()).Duplicates = (ZSort_Type) wV.value<ZDataReference>().DataRank  ;
    wStr.sprintf( "%s Key <%s>  : Duplicates  has been set to <%s>. col %d",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wMainKeyItem->text().toUtf8().data(),
                 decode_ZST(KeyValues->Tab(pItem->row()).Duplicates) ,
          wCol );
    ComLog->appendText(wStr);
/*    if (KeyValues->Tab(pItem->row()).Duplicates==ZST_Duplicates) {
        DoNotChangeKeyValues = true;
        pItem->setText("Duplicates");
      }
      else {
        DoNotChangeKeyValues = true;
        pItem->setText("No duplicates");
    }
*/
    DoNotChangeKeyValues = false;

    return ;
  }//cst_KeyDupCol

  case cst_KeyGrabCol: {
    DoNotChangeKeyValues = false;
    /* first setup change log with values ex-ante and ex-post */
    ZChangeRecord wChgRec(ZFGC_ChgGrab);
    wChgRec.setChangeKey(KeyValues->Tab(pItem->row()).IndexName);
    wChgRec.setAnteBool(KeyValues->Tab(pItem->row()).GrabFreeSpace);
    wChgRec.setPostBool(pItem->checkState()==Qt::Checked );
    wChgRec.setIndexRank (pItem->row());
    ChangeLog.push(wChgRec);

    KeyValues->Tab(pItem->row()).GrabFreeSpace = pItem->checkState()==Qt::Checked  ;
    wStr.sprintf( "%s Key <%s>  : GrabFreeSpace has been set to <%s>. col %d",
        ZDateFull::currentDateTime().toFormatted().toString(),
        wMainKeyItem->text().toUtf8().data(),
        KeyValues->Tab(pItem->row()).GrabFreeSpace?"true":"false" ,
        wCol );
    ComLog->appendText(wStr);

    return ;
  }//cst_KeyGrabCol

  case cst_KeyHighCol: {
    DoNotChangeKeyValues = false;
    /* first setup change log with values ex-ante and ex-post */
    ZChangeRecord wChgRec(ZFGC_ChgHigh);
    wChgRec.setChangeKey(KeyValues->Tab(pItem->row()).IndexName);
    wChgRec.setAnteBool(KeyValues->Tab(pItem->row()).HighwaterMarking);
    wChgRec.setPostBool(pItem->checkState()==Qt::Checked );
    wChgRec.setIndexRank (pItem->row());
    ChangeLog.push(wChgRec);

    KeyValues->Tab(pItem->row()).HighwaterMarking = pItem->checkState()==Qt::Checked  ;

    wStr.sprintf( "%s Key <%s>  : HighwaterMarking has been set to <%s>. col %d",
        ZDateFull::currentDateTime().toFormatted().toString(),
        wMainKeyItem->text().toUtf8().data(),
        KeyValues->Tab(pItem->row()).HighwaterMarking?"true":"false",
        wCol);
    ComLog->appendText(wStr);


    return ;
  }//cst_KeyHighCol

    /* Changing key size induces or may induce a change on
     *  KeySize
     *  Allocation      (if allocation is zero then set to default allocation)
     *  AllocationSize
     */
  case cst_KeySizeCol: {
    DoNotChangeKeyValues = false;
      wStr = pItem->text().toUtf8().data();
      wValue=getValueFromString<size_t>(wStr);

      /* first setup change log with values ex-ante and ex-post */
      ZChangeRecord wChgRec(ZFGC_ChgKeySize);
      wChgRec.setChangeKey(KeyValues->Tab(pItem->row()).IndexName);
      wChgRec.setAnteU64(KeyValues->Tab(pItem->row()).KeySize);
      wChgRec.setPostU64(wValue);
      wChgRec.setIndexRank (pItem->row());
      ChangeLog.push(wChgRec);

      KeyValues->Tab(pItem->row()).KeySize = wValue;
      wStr.sprintf( "%s Value <Guessed key size> for key <%s> has been guessed to <%ld>.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wMainKeyItem->text().toUtf8().data(),
          KeyValues->Tab(pItem->row()).KeySize  );
      ComLog->appendText(wStr);

      DoNotChangeKeyValues = true;

      if (KeyValues->Tab(pItem->row()).Allocated==0){
        KeyValues->Tab(pItem->row()).Allocated = cst_ZRF_default_allocation;

        ZChangeRecord wChgRecAll(ZFGC_ChgAlloc);
        wChgRecAll.setChangeKey(KeyValues->Tab(pItem->row()).IndexName);
        wChgRecAll.setAnteU64(size_t(0L));
        wChgRecAll.setPostU64(KeyValues->Tab(pItem->row()).Allocated );
        wChgRecAll.setIndexRank (pItem->row());
        ChangeLog.push(wChgRecAll);
        wStr.sprintf( "%s Value <Key Allocated blocks> for key <%s> has been defaulted to <%ld>.Logging change for this value.",
                      ZDateFull::currentDateTime().toFormatted().toString(),
                      wMainKeyItem->text().toUtf8().data(),
                      cst_ZRF_default_allocation);
        ComLog->appendText(wStr);

      }

      ZChangeRecord wChgRecAllSiz(ZFGC_ChgAlloc);
      wChgRecAllSiz.setChangeKey(KeyValues->Tab(pItem->row()).IndexName);
      wChgRecAllSiz.setAnteU64(KeyValues->Tab(pItem->row()).AllocatedSize);
      wChgRecAllSiz.setPostU64(KeyValues->Tab(pItem->row()).Allocated * wValue );
      wChgRecAllSiz.setIndexRank (pItem->row());
      ChangeLog.push(wChgRecAllSiz);

      KeyValues->Tab(pItem->row()).AllocatedSize =  KeyValues->Tab(pItem->row()).Allocated * wValue;
      wStr.sprintf("%ld",KeyValues->Tab(pItem->row()).AllocatedSize);
      KeyTBv->ItemModel->item(pItem->row(),cst_KeyAllocSizeCol)->setText(wStr.toCChar());
      wStr.sprintf( "%s Value <Key Allocation Size> for key <%s> has been computed to <%ld>.Logging change for this value.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wMainKeyItem->text().toUtf8().data(),
          KeyValues->Tab(pItem->row()).AllocatedSize );
//      plainTextEdit->appendPlainText(wStr.toCChar());
      ComLog->appendText(wStr);

      KeyValues->Tab(pItem->row()).ExtentSize = KeyValues->Tab(pItem->row()).ExtentQuota * wValue;
      wStr.sprintf("%ld",KeyValues->Tab(pItem->row()).ExtentSize);
      KeyTBv->ItemModel->item(pItem->row(),cst_KeyExtentSizeCol)->setText(wStr.toCChar());

      wStr.sprintf( "%s Value <Key Extent Size> for key <%s> has been computed to <%ld>. Logging change for this value.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wMainKeyItem->text().toUtf8().data(),
          KeyValues->Tab(pItem->row()).ExtentSize );
      ComLog->appendText(wStr);

      DoNotChangeKeyValues = false;
      return ;
  }// cst_KeySizeCol

  case cst_KeyAllocCol: {

      wStr = pItem->text().toUtf8().data();
      wValue=getValueFromString<size_t>(wStr);

      /* first setup change log with values ex-ante and ex-post */
      ZChangeRecord wChgRec(ZFGC_ChgAlloc);
      wChgRec.setChangeKey(KeyValues->Tab(pItem->row()).IndexName);
      wChgRec.setAnteU64(KeyValues->Tab(pItem->row()).Allocated);
      wChgRec.setPostU64(wValue);
      wChgRec.setIndexRank (pItem->row());
      ChangeLog.push(wChgRec);

      KeyValues->Tab(pItem->row()).Allocated = wValue;

      wStr.sprintf( "%s Value <Allocation> for key <%s> has been changed to <%ld>.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wMainKeyItem->text().toUtf8().data(),
          KeyValues->Tab(pItem->row()).Allocated );

      DoNotChangeKeyValues = true;

      KeyValues->Tab(pItem->row()).AllocatedSize = KeyValues->Tab(pItem->row()).KeySize * wValue;
      wStr.sprintf("%ld",KeyValues->Tab(pItem->row()).AllocatedSize);
      KeyTBv->ItemModel->item(pItem->row(),cst_KeyAllocSizeCol)->setText(wStr.toCChar());

      wStr.sprintf( "%s Value <Key Allocation Size> for key <%s> has been computed to <%ld>.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wMainKeyItem->text().toUtf8().data(),
          KeyValues->Tab(pItem->row()).AllocatedSize);
      ComLog->appendText(wStr);

      DoNotChangeKeyValues = false;

      return ;
  } //cst_KeyAllocCol
  case cst_KeyExtentQuotaCol: {

      wStr = pItem->text().toUtf8().data();
      wValue=getValueFromString<size_t>(wStr);

      /* first setup change log with values ex-ante and ex-post */
      ZChangeRecord wChgRec(ZFGC_ChgExtent);
      wChgRec.setChangeKey(KeyValues->Tab(pItem->row()).IndexName);
      wChgRec.setAnteU64(KeyValues->Tab(pItem->row()).ExtentQuota);
      wChgRec.setPostU64(wValue);
      wChgRec.setIndexRank (pItem->row());
      ChangeLog.push(wChgRec);

      KeyValues->Tab(pItem->row()).ExtentQuota = wValue;
      wName="Extent quota";
      wStr.sprintf( "%s Value %s for key <%s> has been changed to <%ld>.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wName,
          wMainKeyItem->text().toUtf8().data(),
          KeyValues->Tab(pItem->row()).ExtentQuota );
      ComLog->appendText(wStr);

      DoNotChangeKeyValues = true;

      KeyValues->Tab(pItem->row()).ExtentSize = KeyValues->Tab(pItem->row()).KeySize * wValue;
      wStr.sprintf("%ld",KeyValues->Tab(pItem->row()).ExtentSize);
      KeyTBv->ItemModel->item(pItem->row(),cst_KeyExtentSizeCol)->setText(wStr.toCChar());

      wName="Extent quota size";
      wStr.sprintf( "%s Value %s for key <%s> has been computed to <%ld>.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wName,
          wMainKeyItem->text().toUtf8().data(),
          KeyValues->Tab(pItem->row()).ExtentSize);
//      plainTextEdit->appendPlainText(wStr.toCChar());
      ComLog->appendText(wStr);

      DoNotChangeKeyValues = false;

      return ;
  } // cst_KeyExtentQuotaCol

    default:
//      plainTextEdit->appendPlainText("Modifying this column is not authorized.");
      ComLog->appendText("Modifying this column is not authorized.");
      return;
    }// switch

  return;
}//FileGenerateDLg::GuessItemChanged

void
FileGenerateMWn::Compute() {
  ZArray<utf8VaryingString> wWarnedFields;
  MeanRecordSize=0;
  utf8VaryingString wStr;

//  MeanRecordSize += sizeof(uint64_t);  /* user record size */  // no --deprecated--

  /* size of key section : address array */

  MeanRecordSize += sizeof(uint32_t);  /* number of key addresses */
  MeanRecordSize += sizeof(zaddress_type) * DictionaryFile->KeyDic.count() ;

  /* bitset size */
  ZBitset wBS;
  wBS._allocate(DictionaryFile->count() );
  MeanRecordSize += wBS.getURFSize();

  MeanRecordSize += sizeof(uint64_t);  /* URF Data size */

  for (long wi=0 ; wi < DictionaryFile->count() ; wi++) {
    MeanRecordSize += DictionaryFile->Tab(wi).HeaderSize;
    MeanRecordSize += DictionaryFile->Tab(wi).UniversalSize;
    if ((DictionaryFile->Tab(wi).ZType & ZType_VaryingMask) || (DictionaryFile->Tab(wi).UniversalSize==0)) {
    }
  } // for

  bool wNotGuessed = false;
  for (int wi=0;wi < GuessTBv->ItemModel->rowCount();wi++) {
    QVariant wV = GuessTBv->ItemModel->item(wi,3)->data(Qt::UserRole);
    if (wV.isValid()) {
      size_t wValue = wV.value<size_t>();
      MeanRecordSize += wValue;
    }
    else {
      QStandardItem* wItem = GuessTBv->ItemModel->item(wi,0);

      ComLog->appendTextColor(QColor(),WarnedQCl, "%s-W-INVSIZ Size value for field <%s> has not been guessed and remains invalid.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wItem->text().toUtf8().data());
      wNotGuessed=true;
    }
  }// for

  if (wNotGuessed)
    HideGuessBTn->setIcon(CrossRedPXm);
  else
    HideGuessBTn->setIcon(CrossBluePXm);

  MeanRecordSize += sizeof(uint32_t);

  wStr.sprintf("%ld",MeanRecordSize);
  MeanSizeLEd->setText(wStr.toCChar());

  AllocatedSize = MeanRecordSize*AllocatedBlocks;
  wStr.sprintf("%ld",MeanRecordSize*AllocatedBlocks);
  AllocatedSizeLEd->setText(wStr.toCChar());

  wStr.sprintf("%ld",(MeanRecordSize*AllocatedBlocks)+1024);
  InitialSizeLEd->setText(wStr.toCChar());

  ExtentQuotaSize = MeanRecordSize*ExtentQuota;
  wStr.sprintf("%ld",ExtentQuotaSize);
  ExtentQuotaSizeLEd->setText(wStr.toCChar());
}

bool FileGenerateMWn::testGuessed() {
  bool wNotGuessed = false;
  for (int wi=0;wi < GuessTBv->ItemModel->rowCount();wi++) {
    QVariant wV = GuessTBv->ItemModel->item(wi,3)->data(Qt::UserRole);
    if (wV.isValid()) {
      size_t wValue = wV.value<size_t>();
      MeanRecordSize += wValue;
    }
    else {
      QStandardItem* wItem = GuessTBv->ItemModel->item(wi,0);

      ComLog->appendTextColor(QColor(),WarnedQCl, "%s-W-INVSIZ Size value for field <%s> has not been guessed and remains invalid.",
                              ZDateFull::currentDateTime().toFormatted().toString(),
                              wItem->text().toUtf8().data());
      wNotGuessed=true;
    }
  }// for

  if (wNotGuessed)
    HideGuessBTn->setIcon(CrossRedPXm);
  else
    HideGuessBTn->setIcon(CrossBluePXm);

  return wNotGuessed;
}


void FileGenerateMWn::DicEditQuitCallback(){
  DicEdit = nullptr;
}

void FileGenerateMWn::closeComlogCB(const QEvent* pEvent)
{
  ComLog=nullptr;
}

void
FileGenerateMWn::MenuAction(QAction* pAction){

  if (pAction==GenFileQAc){
    GenFile();
    return;
  }
  if (pAction==ApplyToCurrentQAc){
    applyToCurrentZmf();
    return;
  }
/*  if (pAction==ApplyToLoadedQAc){
    changeChosenZmf();
    return;
  }
*/
  if (pAction==LoadXmlDicQAc){

    if (DicEdit==nullptr) {
      DicEdit = new DicEditMWn(std::bind(&FileGenerateMWn::DicEditQuitCallback, this),this);
    }

    if (DictionaryFile==nullptr) {
      ZExceptionDLg::adhocMessage("External dictionary",Severity_Error,nullptr,nullptr,
          "No external dictionary has been loaded yet.\n"
          "Please load one before trying to edit it.");
      return;
    }
    DicEdit->displayZMFDictionary(DictionaryFile->getDictionary());
    DicEdit->show();
    return;
  }//if (pAction==LoadXmlDicQAc)

  if (pAction==LoadDicFileQAc){

    if (DicEdit==nullptr) {
      DicEdit = new DicEditMWn(std::bind(&FileGenerateMWn::DicEditQuitCallback, this),this);
    }

    if (DictionaryFile==nullptr) {
      ZExceptionDLg::adhocMessage("External dictionary",Severity_Error,nullptr,nullptr,
          "No external dictionary has been loaded yet.\n"
          "Please load one before trying to edit it.");
      return;
    }
    DicEdit->displayZMFDictionary(DictionaryFile->getDictionary());
    DicEdit->show();
    return;
  }//if (pAction==LoadDicFileQAc)


  if (pAction==EditLoadedDicQAc){

    if (DicEdit==nullptr) {
      DicEdit = new DicEditMWn(std::bind(&FileGenerateMWn::DicEditQuitCallback, this),this);
    }

    if (DictionaryFile==nullptr) {
      ZExceptionDLg::adhocMessage("External dictionary",Severity_Error,nullptr,nullptr,
          "No external dictionary has been loaded yet.\n"
          "Please load one before trying to edit it.");
      return;
    }
    DicEdit->displayZMFDictionary(DictionaryFile->getDictionary());
    DicEdit->show();
    return;
  }//if (pAction==EditLoadedDicQAc)

  if (pAction==EmbedDicQAc){

    if (MasterFile==nullptr) {
      ZExceptionDLg::adhocMessage("Embedded dictionary",Severity_Error,nullptr,nullptr,
          "No current master file has been set.\n"
          "Either generate one or load an existing one.\n"
          "You may afterwards embed a dictionary.");
      return;
    }

    if (DictionaryFile==nullptr) {
      ZExceptionDLg::adhocMessage("Embed dictionary",Severity_Error,
          "No External dictionary to embed.\n"
          "Please load an external dictionary prior embedding.");
      return;
    }
    if (MasterFile->Dictionary!=nullptr) {
      int wRet = ZExceptionDLg::adhocMessage2B("Embed dictionary",Severity_Error,
          "Give up","Confirm",
          nullptr,nullptr,
          "Current file <%s> \n"
          "has already an embedded dictionary <%s> version <%s> created <%s>.\n"
          "Embedding external dictionary <%s> will overwrite former one.\n"
          "External dictionary has name <%s> version <%s> created <%s>.\n\n"
          "It is requested that you confirm your choice."
          ,
          MasterFile->getURIContent().toString(),
          MasterFile->Dictionary->DicName.toString(),
          getVersionStr( MasterFile->Dictionary->Version).toString(),
          MasterFile->Dictionary->CreationDate.toDMY().toString(),
          DictionaryFile->DicName.toString(),
          getVersionStr( DictionaryFile->Version).toString(),
          DictionaryFile->CreationDate.toDMY().toString()
          );
      if (wRet==QDialog::Rejected)
        return;
      ComLog->appendText("Confirmed request to replace embedded dictionary <%s> version file <%s>",
          MasterFile->Dictionary->DicName.toString(),
          getVersionStr( MasterFile->Dictionary->Version).toString(),
          MasterFile->getURIDictionary().toString());
      ComLog->appendText("with external dictionary <%s> version from file <%s>",
          DictionaryFile->DicName.toString(),
          getVersionStr( DictionaryFile->Version).toString(),
          DictionaryFile->URIDictionary.toString());

      } // if (MasterFile->Dictionary!=nullptr)

    ZStatus wSt = MasterFile->setDictionary(*DictionaryFile);
    if (wSt!=ZS_SUCCESS) {
      ZExceptionDLg::adhocMessage("Embedded dictionary",Severity_Error,
          "Cannot embed dictionary %s file %s.\n"
          "See ZException stack for more information.",
          DictionaryFile->DicName.toString(),
          DictionaryFile->URIDictionary.toString());
      ComLog->appendTextColor(ErroredQCl, "%s-E-CREDIC Cannot create dictionary to embed.\n"
                         "Exception follows :\n"
                         "%s",
          ZDateFull::currentDateTime().toFormatted().toString(),ZException.last().formatFullUserMessage().toString());
      return;
    }

    ComLog->appendText("Dictionary embedded into master file with file name <%s>.",MasterFile->getURIDictionary().toString());
    return;
    }//if (pAction==EmbedDicQAc)


  if (pAction==EditEmbeddedDicQAc){

    if (MasterFile==nullptr) {
      ZExceptionDLg::adhocMessage("Embedded dictionary",Severity_Error,nullptr,nullptr,
          "No current master file has been set.\n"
          "Either generate one or load an existing one.\n"
          "You may afterwards edit its embedded dictionary.");
      return;
    }
    if (MasterFile->Dictionary==nullptr) {
      if (DictionaryFile == nullptr) {
      ZExceptionDLg::adhocMessage("Embedded dictionary",Severity_Error,nullptr,nullptr,
            "Current file <%s> \n"
            "has no embedded dictionary (Raw master file).\n"
            "No external dictionary has been loaded.\n"
            "Either you may define one new dictionary to embed\n"
            "Or load an existing external one and embed it.",MasterFile->getURIContent().toString());
      return;
      }
      /* here there is an external dictionary loaded */
      int wRet = ZExceptionDLg::adhocMessage2B("Embedded dictionary",Severity_Error,
          "Give up","Embed",
          nullptr,nullptr,
          "Current file <%s> \n"
          "has no embedded dictionary (Raw master file).\n"
          "But an external dictionary <%s> has been loaded.\n"
          "Either you may define one new dictionary to embed\n"
          "Or load an existing external one and embed it.",
          MasterFile->getURIContent().toString(),
          DictionaryFile->DicName.toString());
      if (wRet==QDialog::Rejected)
        return;
      ComLog->appendText("Request to embed dictionary %s to raw master file <%s>",
                DictionaryFile->DicName.toString(),
                MasterFile->getURIContent().toString());
      ZStatus wSt = MasterFile->setDictionary(*DictionaryFile);
      if (wSt!=ZS_SUCCESS) {
        ZExceptionDLg::adhocMessage("Embedded dictionary",Severity_Error,
            "Cannot create dictionary to embed.\n"
            "See ZException stack for more information.");
        ComLog->appendTextColor(ErroredQCl, "%s-E-CREDIC Cannot create dictionary to embed.\n"
                                            "Exception follows :\n"
                                            "%s",
            ZDateFull::currentDateTime().toFormatted().toString(),ZException.last().formatFullUserMessage().toString());
        return;
      }
      ComLog->appendText("Dictionary embedded into master file with file name <%s>.",MasterFile->getURIDictionary().toString());
//      return; // let normal embedded dic edit from newly embedded dictionary
    }
    if (DicEdit==nullptr) {
      DicEdit = new DicEditMWn(std::bind(&FileGenerateMWn::DicEditQuitCallback, this),this);
    }
    DicEdit->setMasterFile(MasterFile);
    DicEdit->displayZMFDictionary(MasterFile->Dictionary->getDictionary());
    DicEdit->show();
    return;
  }

  if (pAction==SetupFromZmfQAc){
    setupFromZmfFile();
    return;
  }
  if (pAction==SetupFromXmlDicQAc){
    setupFromXmlDic();
    return;
  }
  if (pAction==SetupFromDicFileQAc){
    setupFromDicFile();
    return;
  }
  if (pAction==SetupFromXmlDefQAc){

    setupFromXmlDef();
    return;
  }


  if (pAction==SaveToXmlQAc){
    if (MasterFile==nullptr) {
      ZExceptionDLg::adhocMessage("Xml definition file",Severity_Error,
                                  nullptr,nullptr,
                                  "No master file has been loaded or defined yet.");

      return;
    }
    uriString wXmlFile = GeneralParameters.getWorkDirectory();
    wXmlFile.addConditionalDirectoryDelimiter();
    wXmlFile += MasterFile->getURIContent().getRootname().toCChar() ;
    wXmlFile += "-def.xml" ;



    QString wFileName = QFileDialog::getSaveFileName(this, tr("xml definition file"),
                                                     wXmlFile.toCChar(),
                                                     "Xml files (*.xml);;All (*.*)");
    if (wFileName.isEmpty()) {
      return;
    }
    wXmlFile = wFileName.toUtf8().data();
    _refresh(); /* update booleans like GrabFreeSpace */

    if (XmlDefinitionSave(wXmlFile)==ZS_SUCCESS) {
      ZExceptionDLg::adhocMessage("Xml definition file",Severity_Information,nullptr,nullptr,"Definition file <%s> \n" "has been successfully written.",wXmlFile.toString());
      return;
    }

    ZExceptionDLg::displayLast("Xml definition file");
    return;
  }
/*
  if (pAction==ApplyToZmfQAc){
    changeChosenZmf();
    return;
  }
*/
  if (pAction==SearchDirQAc){
    SearchDir();
    return;
  }
  if (pAction==IndexSearchDirQAc){
    SearchIndexDir();
    return;
  }

  if (pAction==DataExportQAc){
    DataExport();
    return;
  }

  if (pAction==DataImportQAc){
    DataImport();
    return;
  }

  if (pAction==SameAsMasterQAc){
    SameAsMaster();
    return;
  }

  if (pAction==KeyDeleteQAc){
    KeyDelete();
    return;
  }

  if (pAction==KeyAppendRawQAc){
    KeyAppendRaw();
    return;
  }

  if (pAction==KeyAppendFromEmbeddedDicQAc){
    KeyAppendFromEmbeddedDic();
    return;
  }
  if (pAction==KeyAppendFromLoadedDicQAc){
    KeyAppendFromLoadedDic();
    return;
  }

  if (pAction==indexRebuildQAc){
    indexRebuildFromMenu();
    return;
  }

  if (pAction==ShowGuessValQAc){
    GuessGBx->setVisible(true);
    return;
  }
  if (pAction==HideGuessValQAc){
    HideGuess();
    return;
  }
  if (pAction==ShowLogQAc){
    ComLog->show();
    ComLog->setFocus();
    return;
  }
  if (pAction==HideLogQAc){
    ComLog->hide();
    return;
  }

  if (pAction==ShowGenLogQAc){
    displayChangeLog();
    return;
  }
  if (pAction==HideGenLogQAc){
    ChangeLogMWn->hide();
    return;
  }

  if (pAction==QuitQAc){
    Quit();
    return;
  }
  return;
}//FileGenerateDLg::MenuAction

void
FileGenerateMWn::KeyDelete() {
  QModelIndex wIdx= KeyTBv->currentIndex();
  if (!wIdx.isValid())
    return;
  long wI= long(wIdx.row());

  /* first setup change log with values ex-ante and ex-post */
  ZChangeRecord wChgRec(ZFGC_KeyDelete);
  wChgRec.setChangeKey(KeyValues->Tab(wI).IndexName);

  wChgRec.setIndexRank (wIdx.row());
  wChgRec.setAnteKeyData(KeyValues->Tab(wI));  /* store deleted key data (post remains nullptr) */

  ChangeLog.push(wChgRec);

  if (DeletedKeyValues==nullptr)
    DeletedKeyValues = new ZArray<KeyData> ;

  DeletedKeyValues->push(KeyValues->Tab(wI));


  KeyTBv->ItemModel->removeRow(int(wI));

  if (DictionaryFile!=nullptr) {
      if (DictionaryFile->KeyDic.count()>wI) {
          ComLog->appendText("Corresponding key named <%s> is been removed from dictionary.",
                             DictionaryFile->KeyDic[wI]->DicKeyName.toString());
          DictionaryFile->KeyDic.erase(wI);
      }
      else
          ComLog->appendText("Cannot remove from dictionary key named <%s>.",
                             KeyValues->Tab(wI).IndexName.toString());

  }

  KeyValues->erase(wI);
  return;
}

void
FileGenerateMWn::KeyAppendRaw() {
  if (KeyValues==nullptr)
    KeyValues = new ZArray<KeyData> ;


  KeyData wKD;
  wKD.IndexName = "New key";
  ZKeyHeaderRow wKHR;

  ZKeyDLg* wKeyDLg = new ZKeyDLg(this);
  while (true) {
    int wRet=wKeyDLg->exec();
    if (wRet==QDialog::Rejected)
      return;
    wKHR = wKeyDLg->get();
    if (wKHR.DicKeyName.isEmpty()){
      ZExceptionDLg::adhocMessage("New raw key",Severity_Error,nullptr,nullptr,
          "Key name cannot be null");
      continue;
    }
    long wKI=-1;
    for (long wi=0;wi < KeyValues->count();wi++) {
      if (KeyValues->Tab(wi).IndexName==wKHR.DicKeyName) {
        wKI=wi;
        break;
      }
    }
    if (wKI<0){
      ZExceptionDLg::adhocMessage("New raw key",Severity_Error,nullptr,nullptr,
          "Key name <%s> already exists."
          "A key/index name must be unique.",wKHR.DicKeyName.toString());
      continue;
    }
    break;
  }//while true

  wKD.IndexName = wKHR.DicKeyName ;
  wKD.Duplicates = wKHR.Duplicates ;
  wKD.KeySize = size_t(wKHR.KeyGuessedSize);

  long wR=KeyValues->push(wKD);
  /* first setup change log with values ex-ante and ex-post */
  ZChangeRecord wChgRec(ZFGC_KeyRawAppend);
  wChgRec.setChangeKey(wKD.IndexName);
  wChgRec.setPostKeyData(wKD);
  wChgRec.setIndexRank (wR);
  ChangeLog.push(wChgRec);

  QList<QStandardItem*> wKeyRow = formatKeyRow(wKD);
  KeyTBv->ItemModel->appendRow(wKeyRow);
}

ZStatus
FileGenerateMWn::KeyAppendFromEmbeddedDic() {
  if (MasterFile==nullptr) {

    ZExceptionDLg::adhocMessage("Append key",Severity_Error,nullptr,nullptr,"No Master file has currently being loaded.");
    ComLog->appendTextColor(QColor(),ErroredQCl,"%s-E-NOZMF No Master file has currently being loaded.",
        ZDateFull::currentDateTime().toFormatted().toString());
    return ZS_NULLPTR;
  }
  if (!MasterFile->isOpen()) {
    ZExceptionDLg::adhocMessage("Append key",Severity_Error,nullptr,nullptr,"Master file <%s> containing dictionary is not open.",
        MasterFile->getURIContent().toString());
    ComLog->appendTextColor(QColor(),ErroredQCl,"%s-E-NOTOPEN Master file <%s> containing dictionary is not open.",
        ZDateFull::currentDateTime().toFormatted().toString(),
        MasterFile->getURIContent().toString());
    return ZS_FILENOTOPEN ;
  }
  if (MasterFile->Dictionary==nullptr) {
    ZExceptionDLg::adhocMessage("Append key",Severity_Error,nullptr,nullptr,"Master file <%s> has no valid dictionary.",
        MasterFile->getURIContent().toString());
    ComLog->appendTextColor(QColor(),ErroredQCl,"%s-E-NODIC Master file <%s> has no valid dictionary.",
        ZDateFull::currentDateTime().toFormatted().toString(),
        MasterFile->getURIContent().toString());
    return ZS_NULLPTR;
  }

  return KeyAppendFromZMFDic (MasterFile->Dictionary);
}

ZStatus
FileGenerateMWn::KeyAppendFromLoadedDic() {

  if (DictionaryFile==nullptr) {
      ZExceptionDLg::adhocMessage("Append key",Severity_Error,nullptr,nullptr,"Neither master file nor dictionary have been loaded.");
      ComLog->appendTextColor(QColor(),ErroredQCl,"%s-E-NOLDED Neither master file nor dictionary have been loaded.",ZDateFull::currentDateTime().toFormatted().toString());

      return ZS_NULLPTR;
  }

  return KeyAppendFromZMFDic (DictionaryFile);
}

ZStatus
FileGenerateMWn::KeyAppendFromZMFDic(const ZMFDictionary* pDic) {
  ZKeyListDLg* wKLDLg=new ZKeyListDLg(this);
  wKLDLg->initLayout();
  ZStatus wSt=wKLDLg->displayKeyDictionaries(pDic);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wKLDLg->show();
  int wRet=wKLDLg->exec();
  if (wRet==QDialog::Accepted) {
    KeyData wKD;
    wKD = wKLDLg->getKeyData();
    if (KeyValues==nullptr) {
      KeyValues=new zbs::ZArray<KeyData>;
    }
    /*  generate index file root name */
    wKD.IndexRootName = generateIndexRootName(RootName,wKD.IndexName);

    if (wKD.KeySize==0) {
      ComLog->appendTextColor(QColor(),WarnedQCl, "%s-W-KEYZERSZ Appended Key <%s> has KeySize equal to Zero and must be guessed.\n"
          "All size values are computed from this single value and will be set to zero.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wKD.IndexName.toString());
    }

    /* compute size values */
    wKD.Allocated = cst_ZRF_default_allocation;
    wKD.ExtentQuota = cst_ZRF_default_extentquota;
    wKD.AllocatedSize = wKD.Allocated * wKD.KeySize;
    if (wKD.AllocatedSize==0) {
      wKD.AllocatedSize = 150; /* a default value as another */
      ComLog->appendText("Key creation: Defaulting AllocatedSize to 150 ");
    }
    wKD.ExtentSize = wKD.ExtentQuota * wKD.KeySize;
    if (wKD.ExtentSize==0) {
      wKD.ExtentSize = 150; /* a default value as another */
      ComLog->appendText("Key creation: Defaulting ExtentQuota to 150 ");
    }
    /* then store new key from dic */
    long wR=KeyValues->push(wKD);
    /* first setup change log with values ex-ante and ex-post */
    ZChangeRecord wChgRec(ZFGC_KeyDicAppend);
    wChgRec.setChangeKey(wKD.IndexName);
    wChgRec.setPostKeyData(wKD);
    wChgRec.setIndexRank (wR);
    ChangeLog.push(wChgRec);

    ComLog->appendText("Created ChangeLog record : new key <%s> waiting for creation.",
        wKD.IndexName.toString());

    /* display key in key list */
    QList<QStandardItem*> wKeyRow = formatKeyRow(wKD);
    KeyTBv->ItemModel->appendRow(wKeyRow);

    for (int wi=0; wi < KeyTBv->ItemModel->columnCount(); wi++)
      KeyTBv->resizeColumnToContents(wi);
    for (int wi=0; wi < KeyTBv->ItemModel->rowCount(); wi++)
      KeyTBv->resizeRowToContents(wi);

  }
  delete wKLDLg;
  return ZS_SUCCESS;
}

bool FileGenerateMWn::setupFromZmfFile() {
  utf8VaryingString wStr;

  QFileDialog wFDLg((QWidget*)this,QObject::tr("Search master file","FileGenerateDLg"),
                    GeneralParameters.getWorkDirectory().toCChar(),
                    "ZMF files (*.zmf);;All files (*.*)");

  wFDLg.setLabelText(QFileDialog::Accept,  QObject::tr("Select","FileGenerateDLg"));
  wFDLg.setLabelText(QFileDialog::Reject,  QObject::tr("Cancel","FileGenerateDLg"));

  wFDLg.setOptions(QFileDialog::ReadOnly);
  wFDLg.setFileMode(QFileDialog::ExistingFiles);
  wFDLg.setDirectory(GeneralParameters.getWorkDirectory().toCChar());
  int wRet=wFDLg.exec();

  if ((wRet==QDialog::Rejected) ||(wFDLg.selectedFiles().isEmpty()))
    return false;

  uriString wURIMaster = wFDLg.selectedFiles()[0].toUtf8().data();

  ComLog->appendText("Loading parameters from existing file <%s>.",
      wURIMaster.toString());

  return dataSetupFromMasterFile(wURIMaster);
}

bool FileGenerateMWn::setupFromXmlDic() {
  ZStatus wSt=loadExternalXmlDic();
  if (wSt!=ZS_SUCCESS)
    return false;

  dataSetupFromDictionary();
  return true;
} //setupFromXmlDic

bool FileGenerateMWn::setupFromDicFile() {
  ZStatus wSt=loadExternalDicFile();
  if (wSt!=ZS_SUCCESS)
    return false;

  dataSetupFromDictionary();
  return true;
} //setupFromXmlDic


bool FileGenerateMWn::setupFromXmlDef() {
  utf8VaryingString wStr;
  QString wFileName = QFileDialog::getOpenFileName((QWidget*)this, tr("Xml definition file"),
                                                   GeneralParameters.getWorkDirectory().toCChar(),
                                                   "Xml files (*.xml);;All (*.*)");
  if (wFileName.isEmpty()) {
    ZExceptionDLg::adhocMessage("Xml file",Severity_Information,&ErrorLog,nullptr,"No file selected. Please select a valid file");
    return false;
  }
  uriString wXmlFile = wFileName.toUtf8().data();

  wStr.sprintf("Loading file definition from <%s>.",wXmlFile.toString());
  ComLog->appendText(wStr);

  utf8VaryingString wXmlContent;
  ZStatus wSt=wXmlFile.loadUtf8(wXmlContent);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Xml definition");
    ComLog->appendText(ZException.last().formatFullUserMessage());
    return false;
  }

 ErrorLog.setAutoPrintOn(ZAIES_Info);
 wSt=XmlDefinitionLoad(wXmlContent, &ErrorLog);
 if (wSt!=ZS_SUCCESS) {
   ZExceptionDLg::adhocMessage("Xml definition",Severity_Error,&ErrorLog,nullptr,"Error while parsing Xml string");
   return false;
 }
  _dataSetup();
  return true;
}



ZStatus FileGenerateMWn::loadExternalXmlDic() {
  utf8VaryingString wStr;

  QString wFileName = QFileDialog::getOpenFileName((QWidget*)this, tr("Xml Dictionary"),
                                                   GeneralParameters.getWorkDirectory().toCChar(),
                                                   "Xml dictionaries (*.dic);;Xml files (*.xml);;All (*.*)");
  if (wFileName.isEmpty()) {
     return ZS_EMPTY ;
  }
  uriString wXmlURI = wFileName.toUtf8().data();

  wStr.sprintf("loading xml dictionary file <%s>",wXmlURI.toString());
  ComLog->appendText(wStr);

  utf8VaryingString wXmlContent;
  ZStatus wSt=wXmlURI.loadUtf8(wXmlContent);
  if (wSt!=ZS_SUCCESS) {

    ComLog->appendText(ZException.last().formatFullUserMessage());

    ZExceptionDLg::displayLast("Xml Dictionary");
    return wSt;
  }

  SourceLBl->setText("Loaded Xml dictionary file");
  SourceContentLBl->setText(wXmlURI.toCChar());


  DictionaryFile = new ZDictionaryFile;
  DictionaryFile->setDicFilename(wXmlURI);

  ErrorLog.setAutoPrintOn(ZAIES_Info);
//  wSt=DictionaryFile->ZMFDictionary::XmlLoadFromString(wXmlContent,false,&ErrorLog);
  wSt=DictionaryFile->loadDictionary(wXmlURI,&ErrorLog);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Xml Dictionary",Severity_Error,&ErrorLog,nullptr,
        "Cannot load xml dictionary from file <%s>\n"
        "Status is <%s>",wXmlURI.toString(),decode_ZStatus(wSt));

    wStr.sprintf("Cannot load xml dictionary from file <%s>\n"
                 "Status is <%s>",wXmlURI.toString(),decode_ZStatus(wSt));
    ComLog->appendText(wStr);

    return wSt;
  }

  SourceLBl->setText("Xml  Dictionary File");
  SourceContentLBl->setText(wXmlURI.toCChar());
  wStr.sprintf("Loaded <%s>",DictionaryFile->DicName.toString());
  DicLoadLBl->setText(wStr.toCChar());

  return ZS_SUCCESS ;
} //loadExternalXmlDic

ZStatus FileGenerateMWn::loadExternalDicFile() {
  utf8VaryingString wStr;

  QString wFileName = QFileDialog::getOpenFileName((QWidget*)this, tr("Dictionary file"),
                                                   GeneralParameters.getWorkDirectory().toCChar(),
                                                   "Dic files (*.dic);;All (*.*)");
  if (wFileName.isEmpty()) {
    ZExceptionDLg::adhocMessage("Xml file",Severity_Information,nullptr,nullptr,"No file selected. Please select a valid file");
    return ZS_EMPTY ;
  }
  uriString wURIDic = wFileName.toUtf8().data();

  ComLog->appendText("loading dictionary file <%s>",wURIDic.toString());

  DictionaryFile = new ZDictionaryFile;
  ZStatus wSt = DictionaryFile->loadDictionary(wURIDic,&ErrorLog);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Dictionary load",Severity_Error,"Error while loading dictionary file <%s>.",wURIDic.toString());
    return wSt;
  }
  SourceLBl->setText("Dictionary File");
  SourceContentLBl->setText(wURIDic.toCChar());
  wStr.sprintf("Loaded <%s>",DictionaryFile->DicName.toString());
  DicLoadLBl->setText(wStr.toCChar());
  return ZS_SUCCESS ;
} //loadExternalDicFile


bool FileGenerateMWn::applyToCurrentZmf() {

  if (ValuesControl()) {
        return false;
      }    
  if (MasterFile==nullptr) {
    ZExceptionDLg::adhocMessage("FileGenerateDLg::applyToZmf",Severity_Error,nullptr,nullptr,
        "No master file loaded. Please load one.");
    return false;
  }
/*
  zmode_type wMode=ZRF_NotOpen;
  if (MasterFile->isOpen()){
    wMode = MasterFile->getMode();
    MasterFile->zclose();
  }
*/
  int wRet=ZExceptionDLg::adhocMessage2B("Backup files before",Severity_Question,"Do not backup","Backup",
      "Do you want to backup files before change ?");

  if (wRet==QDialog::Accepted)
    return applyChangesZmf(true);

  return applyChangesZmf(false);
}


ZStatus
FileGenerateMWn::applyChangesZmf(bool pBackup) {
  ZStatus wSt;
  zbs::ZArray<utf8VaryingString> wIndexToRebuild;

  if ((MasterFile==nullptr) || (!MasterFile->isOpen())) {
    ZExceptionDLg::adhocMessage("Apply changes",Severity_Error,&ErrorLog,nullptr,"Master file must be open to be changed");
    return ZS_FILENOTOPEN;
  }

  _refresh(); /* update some booleans like GrabFreeSpace */

  if (ValuesControl()) {
    return ZS_INVPARAMS;
  }
  wSt=MasterFile->Dictionary->save_xml(true);
//  DictionaryFile->save_xml(true);
  wSt=MasterFile->writeControlBlocks();

  int wModif=0,wKeyModif=0,wKeyCreated=0,wKeyDeleted=0,wKeyErrored=0,wRebuildErrored=0;

  utf8VaryingString wStr;

  ComLog->appendText("Applying changes to file <%s>.", MasterFile->getURIContent().toString());

  if (MasterFile->getOpenMode()!=ZRF_All)  {
//    if (MasterFile->getOpenMode()!=ZRF_NotOpen) {
      ComLog->appendText(" File is open in mode <%s>. Closing file then reopenning it.\n",
          decode_ZRFMode(MasterFile->getOpenMode()));
      MasterFile->zclose();
//    }

    ComLog->appendText("re-openning file <%s> with mode ZRF_All | ZRF_TypeRegardless.", MasterFile->getURIContent().toString());

    wSt=MasterFile->zopen(ZRF_All| ZRF_TypeRegardless);
    if (wSt!=ZS_SUCCESS) {
      ZExceptionDLg::adhocMessage("Master file change",Severity_Error,nullptr,nullptr,
          " Error while opening master file <%s> \n"
          " status <%s>",
          MasterFile->getURIContent().toString(),
          decode_ZStatus(wSt));

      ComLog->appendTextColor( ErroredQCl," Error while opening master file <%s> \nException content follows:\n%s",
          MasterFile->getURIContent().toString(),
          ZException.last().formatFullUserMessage().toString());
      return wSt;
    }
  } //if (MasterFile->getOpenMode()!=ZRF_All)


  /* backup of files */
  if (pBackup) {
    ComLog->appendText("Backing up of components for file %s",MasterFile->getURIContent().toString());

    utf8VaryingString wBckset = MasterFile->getURIContent().getRootname() + utf8VaryingString("_bck");
    uriString wBckDir = MasterFile->getURIContent().getDirectoryPath();
    wBckDir.addConditionalDirectoryDelimiter();
    wSt=MasterFile->backupAll(wBckDir,wBckset);
    if (wSt!=ZS_SUCCESS) {
      ComLog->appendText("Cannot backup files ");
      ComLog->appendText(ZException.last().formatFullUserMessage());
      ZExceptionDLg::adhocMessage("Backup files",Severity_Error,nullptr,nullptr,
          ZException.last().formatFullUserMessage().toCChar());
      ComLog->appendTextColor(QColor(), ErroredQCl,"Cannot backup files.\nException content follows:\n%s",
          ZException.last().formatFullUserMessage().toString());
      return wSt;
    }

    ComLog->appendText("Backed up all files of <%s> ",MasterFile->getURIContent().toString());
  }

  /* effective apply changes */

  ComLog->appendText("Applying changes to file <%s>",MasterFile->getURIContent().toString());

  if (MasterFile->getFCB()->BlockTargetSize != MeanRecordSize) {
    wStr.sprintf("Changing BlockTargetSize from %ld to %ld.",
        MasterFile->getFCB()->BlockTargetSize,MeanRecordSize );
    ComLog->appendText(wStr);

    MasterFile->getFCB()->BlockTargetSize = MeanRecordSize ;
    wModif ++;
  }

  if (MasterFile->getFCB()->BlockExtentQuota != ExtentQuota) {
    wStr.sprintf("Changing BlockExtentQuota from %ld to %ld.",
        MasterFile->getFCB()->BlockExtentQuota,ExtentQuota );
    ComLog->appendText(wStr);

    MasterFile->getFCB()->BlockExtentQuota = ExtentQuota ;
    wModif ++;
  }

  if (MasterFile->getFCB()->GrabFreeSpace != GrabFreeSpace) {
    wStr.sprintf("Changing GrabFreeSpace from %s to %s.",
        MasterFile->getFCB()->GrabFreeSpace?"true":"false",GrabFreeSpace?"true":"false" );
    ComLog->appendText(wStr);
    if (!pBackup)
      MasterFile->getFCB()->GrabFreeSpace = GrabFreeSpace ;
  }

  if (MasterFile->getFCB()->HighwaterMarking != HighWaterMarking) {
    wStr.sprintf("Changing HighwaterMarking from %s to %s.",
        MasterFile->getFCB()->HighwaterMarking?"true":"false",HighWaterMarking?"true":"false" );
    ComLog->appendText(wStr);

    MasterFile->getFCB()->HighwaterMarking = HighWaterMarking ;
    wModif ++;
  }

  if (MasterFile->hasJournal() != Journaling) {
    wStr.sprintf("Changing Journaling from %s to %s.",
        MasterFile->hasJournal()?"true":"false",Journaling?"true":"false" );
    ComLog->appendText(wStr);
    /* put journaling on or off according Journaling */
    if (Journaling) {
      MasterFile->setJournalingOn();
      ComLog->appendText("Journaling started.");
    }
    else {
      MasterFile->setJournalingOff();
      ComLog->appendText("Journaling stopped.");
    }

    wModif ++;
  } // Journaling


  ComLog->appendText("Processing key modifications.");

  for (long wChgIdx=0; wChgIdx < ChangeLog.count();wChgIdx++) {
    ZChangeRecord wChgRec = ChangeLog[wChgIdx];
    const char* wName="<no field>";
    long wKeyRow = ChangeLog[wChgIdx].getIndexRank();
    ComLog->appendText("Found change log code <%s> index key rank <%ld>",
        decode_ZFGC(ChangeLog[wChgIdx].getChangeCode()),
        ChangeLog[wChgIdx].getIndexRank() );
    switch(ChangeLog[wChgIdx].getChangeCode()) {

    case ZFGC_ChgKeySize : {
      size_t wNewKeySize = ChangeLog[wChgIdx].getPostU64();
      wName="KeyGuessedSize";
      ComLog->appendText("%s changed %s field from <%ld> to <%ld>.",
          wName,
          MasterFile->IndexTable[wKeyRow]->getURIContent().toString(),
          MasterFile->IndexTable[wKeyRow]->KeyGuessedSize,
          wNewKeySize);
      MasterFile->IndexTable[wKeyRow]->KeyGuessedSize = wNewKeySize ;

      wKeyModif++;
      break;
    }
    case ZFGC_ChgExtent : {
      size_t wNewExtent = ChangeLog[wChgIdx].getPostU64();
      wName="Extent quota";
      ComLog->appendText("%s changed %s field from <%ld> to <%ld>.",
          wName,
          MasterFile->IndexTable[wKeyRow]->getURIContent().toString(),
          MasterFile->IndexTable[wKeyRow]->getFCB()->BlockExtentQuota,
          wNewExtent);
      MasterFile->IndexTable[wKeyRow]->getFCB()->BlockExtentQuota = wNewExtent ;
      wKeyModif++;
      break;
    }
    case ZFGC_ChgAlloc : {
      size_t wNewAlloc = ChangeLog[wChgIdx].getPostU64();
      wName="Allocated blocks";
      wStr.sprintf("%s changed %s field from <%ld> to <%ld>.",
          wName,
          MasterFile->IndexTable[wKeyRow]->getURIContent().toString(),
          MasterFile->IndexTable[wKeyRow]->getFCB()->AllocatedBlocks,
          wNewAlloc);
      ComLog->appendText(wStr);
      MasterFile->IndexTable[wKeyRow]->getFCB()->AllocatedBlocks = wNewAlloc ;
      wKeyModif++;
      break;
    }

    case ZFGC_ChgDuplicate : {
      ZSort_Type wNewDup = ChangeLog[wChgIdx].getPostZSortType();
      wName="Duplicates";
      wStr.sprintf("%s changed %s field from <%ld> to <%ld>.",
          wName,
          MasterFile->IndexTable[wKeyRow]->getURIContent().toString(),
          decode_ZST( MasterFile->IndexTable[wKeyRow]->Duplicates),
          wNewDup?"true":"false");
      ComLog->appendText(wStr);
      MasterFile->IndexTable[wKeyRow]->Duplicates = wNewDup ;
      wKeyModif++;
      break;
    }
    case ZFGC_ChgGrab: {
      bool wNewGrab = ChangeLog[wChgIdx].getPostBool();
      wName="GrabFreeSpace";
      wStr.sprintf("%s changed %s field from <%ld> to <%ld>.",
          wName,
          MasterFile->IndexTable[wKeyRow]->getURIContent().toString(),
          MasterFile->IndexTable[wKeyRow]->getFCB()->GrabFreeSpace?"true":"false",
          wNewGrab?"true":"false");
      ComLog->appendText(wStr);
      MasterFile->IndexTable[wKeyRow]->getFCB()->GrabFreeSpace = wNewGrab;
      wKeyModif++;
      break;
    }
    case ZFGC_ChgHigh: {
      bool wNewHigh = ChangeLog[wChgIdx].getPostBool();
      wName="Highwater marking";
      wStr.sprintf("%s changed %s field from <%ld> to <%ld>.",
          wName,
          MasterFile->IndexTable[wKeyRow]->getURIContent().toString(),
          MasterFile->IndexTable[wKeyRow]->getFCB()->HighwaterMarking?"true":"false",
          wNewHigh?"true":"false");
      ComLog->appendText(wStr);
      MasterFile->IndexTable[wKeyRow]->getFCB()->HighwaterMarking = wNewHigh;
      wKeyModif++;
      break;
    }

    case ZFGC_INameChange: {

      long wIndexRank=ChangeLog[wChgIdx].getIndexRank();

      utf8VaryingString wZMFRootName,wIndexRootName,wComplement,wOldKeyName,wNewKeyName;
      uriString wNewIndexURI;
      wOldKeyName = ChangeLog[wChgIdx].getAnteString();
      wNewKeyName = ChangeLog[wChgIdx].getPostString();

      wComplement = "Changing index name induces a change in related index file name accordingly.";

      int wRet=ZExceptionDLg::adhocMessage2BHtml("Change index name",Severity_Question,
          "Do not change","Change",
          nullptr,&wComplement,
          "Index file <%s>\n"
          "About to change index file name \n"
          "from <%s> to <%s>.\n\n"
          "<table>"
          "<tr><td> Abandon operation and keep index name</td>      <td><b>Do not Change</b></td> </tr>"
          "<tr><td> Change index name and rename index file</td>    <td><b>Change</b></td> </tr>"
          "</table>",
          MasterFile->IndexTable[wIndexRank]->getURIContent().toString(),
          wOldKeyName.toString(),
          wNewKeyName.toString());
      if (wRet==QDialog::Rejected) {
        ComLog->appendText("%s index name change from <%s> to <%s> has been rejected.",
            MasterFile->IndexTable[wIndexRank]->getURIContent().toString(),
            wOldKeyName.toString(),
            wNewKeyName.toString());
        continue;
      }
      uriString wFormerIndexURI,wNewIndex;
      zmode_type wMode=ZRF_NotOpen;
      if (MasterFile->IndexTable[wIndexRank]->isOpen()) {
        wMode = MasterFile->IndexTable[wIndexRank]->getMode();
      }
      else {
        wSt=MasterFile->zopenIndexFile(wIndexRank,ZRF_All);
      }

      wFormerIndexURI = MasterFile->IndexTable[wIndexRank]->getURIContent();

      wZMFRootName=MasterFile->getURIContent().getRootname();

      wSt=generateIndexURI(wNewIndexURI,MasterFile->getURIContent(),MasterFile->IndexFilePath,wNewIndex);

      wSt=wFormerIndexURI.rename(wNewIndexURI);
//      wSt=renameFile(wFormerIndexURI,wNewIndexURI);
      if (wSt!=ZS_SUCCESS) {

        ComLog->appendText(ZException.last().formatFullUserMessage());
        return ZException.last().Status;
      }

      ComLog->appendText("%s renamed index file from <%ld> to <%ld>.",
          MasterFile->getURIContent().toString(),
          wFormerIndexURI.toString(),
          wNewIndexURI.toString());

      ComLog->appendText("%s changed index name from <%ld> to <%ld>.",
          wName,
          MasterFile->IndexTable[wIndexRank]->getURIContent().toString(),
          wOldKeyName.toString(),
          wNewKeyName.toString());
      MasterFile->IndexTable[wIndexRank]->IndexName = wNewKeyName;

      if (wMode!=ZRF_NotOpen) {
        MasterFile->zopenIndexFile(wIndexRank,wMode);
      }
      wKeyModif++;
      break;
    } //ZFGC_INameChange

    case ZFGC_KeyDelete: {
      utf8VaryingString wKeyName = ChangeLog[wChgIdx].getChangeKey();
      long wIndexRank=ChangeLog[wChgIdx].getIndexRank();

      uriString wURIIndex = MasterFile->IndexTable[wIndexRank]->getURIContent();

      ComLog->appendText("Removing index file %s.", wURIIndex.toString() );
      wKeyModif++;

      wKeyDeleted ++;
      wSt=wURIIndex.remove();
//      wSt =removeFile(wURIIndex.toCChar());
      if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wComplement = ZException.last().formatFullUserMessage();
        int wRet = ZExceptionDLg::adhocMessage2BHtml("Index file deletion",Severity_Error,"Abort","Continue",
            &ErrorLog,&wComplement,
            "Cannot remove index file <%s>\n\n"
            "<table>"
            "<tr><td> terminate processing of changes</td>      <td><b>Abort</b></td></tr>"
            "<tr><td> continue anyway</td>    <td><b>Continue</b></td></tr>"
            "</table>"
            ,wURIIndex.toString());
        if (wRet==QDialog::Rejected)
          return wSt;
      }
      break;
    } // ZFGC_KeyDelete

    case ZFGC_KeyDicAppend:
    case ZFGC_KeyRawAppend: {

      /* A created key has mandatorily a name (unique name) whatever its data has been set to  */

      KeyData wKD = ChangeLog[wChgIdx].getPostKeyData();
      bool wSkip=false;
      for (long wj=0; wj < MasterFile->IndexTable.count(); wj++) {
        if (wKD.IndexName == MasterFile->IndexTable[wj]->IndexName) {
          ComLog->appendText("Index key <%s> already exists in file <%s>.Skipping index key creation.",
              wKD.IndexName.toString(),
              MasterFile->getURIContent().toString());
          wSkip=true;
          wKeyErrored ++;
        }
      }// for
      if (wSkip)
        continue;

      //     KeyValues->push(wKD);

      long wOutRank;
      wSt = MasterFile->_createRawIndexDet(wOutRank,
                                           wKD.IndexName,
                                           uint32_t(wKD.KeySize),
                                           wKD.Duplicates,
                                           long(wKD.Allocated),
                                           long(wKD.ExtentQuota),
                                           zsize_type(wKD.AllocatedSize),
                                           wKD.HighwaterMarking,
                                           wKD.GrabFreeSpace,
                                           true,&ErrorLog);

      if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wComplement = ZException.last().formatFullUserMessage();
        int wRet = ZExceptionDLg::adhocMessage2BHtml("Index file creation",Severity_Error,"Abort","Continue",
            &ErrorLog,&wComplement,
            "Cannot create index file for index key <%s>\n\n"
            "<table>"
            "<tr><td> terminate processing of changes</td>      <td><b>Abort</b></td></tr>"
            "<tr><td> continue anyway</td>    <td><b>Continue</b></td></tr>"
            "</table>"
            ,wKD.IndexName.toString());
        if (wRet==QDialog::Rejected)
          return wSt;
      }
      else
        wIndexToRebuild.push(wKD.IndexName);
      wKeyModif++;
      break;
    }
    default:
      break;
    }// switch
  } // for

  ComLog->appendText("%s Writting changes to files.",ZDateFull::currentDateTime().toFormatted().toString());
  wSt=MasterFile->_writeAllHeaders();
  ComLog->appendText("%s Done.",ZDateFull::currentDateTime().toFormatted().toString());

  if (wIndexToRebuild.count()!=0){
    if (MasterFile->Dictionary==nullptr) {
      utf8VaryingString wStr;
      for (long wi=0 ; wi < wIndexToRebuild.count() ; wi++) {
        wStr += wIndexToRebuild[wi].toString() ;
        wStr += "\n";
      }
      ZExceptionDLg::adhocMessage("Index rebuild",Severity_Information,nullptr,nullptr,
          "Some index keys require to be rebuilt.\n"
          "Following index keys have to be rebuilt.\n"
          "%s",wStr.toString());
      ComLog->appendText("Some index keys require to be rebuilt.\n"
                         "Following index keys have to be rebuilt.\n"
                         "%s",wStr.toString());

      return MasterFile->_writeAllHeaders();
    }// if (MasterFile->Dictionary==nullptr)

    utf8VaryingString wStr;
    for (long wi=0 ; wi < wIndexToRebuild.count() ; wi++) {
      int wRet= ZExceptionDLg::adhocMessage2B(  "Index rebuild",Severity_Question,
          "Skip","Rebuild",
          nullptr,nullptr,
          "Index key <%s> has to be rebuilt.\n",wIndexToRebuild[wi].toString());
      if (wRet==QDialog::Rejected) {
        ComLog->appendText("Index key <%s> needs to be rebuilt.\n",wIndexToRebuild[wi].toString());
        continue;
      }
      long wIRank =-1;
      for (long wj=0 ; wj < MasterFile->IndexTable.count();wj++ )
        if (MasterFile->IndexTable[wj]->IndexName==wIndexToRebuild[wi]){
          wIRank=wj;
          break;
        }
      if (wIRank<0) {
        ZExceptionDLg::adhocMessage("Index key rebuild",Severity_Error,nullptr,nullptr,
            "Index key to be rebuilt <%s> cannot be found.\n"
            "Rebuild action is skipped.",MasterFile->IndexTable[wi]->IndexName.toString());
        wRebuildErrored ++;
        continue;
      }
      ComLog->appendText("Rebuilding index key <%s> using embedded dictionary",MasterFile->IndexTable[wIRank]->IndexName.toString());
      wSt=rebuildIndex(wIRank);
      if (wSt!=ZS_SUCCESS){
        if (wSt==ZS_EMPTY) {
          ComLog->appendTextColor(ErroredQCl,"%s-W-EMPTY Index key name <%s> cannot be rebuilt because file is empty.\n",
                                  ZDateFull::currentDateTime().toFormatted().toString(),
                                  MasterFile->IndexTable[wIRank]->IndexName.toString());
        }
        else {
        wStr=ZException.last().formatFullUserMessage();
        ZExceptionDLg::adhocMessage("Index key rebuild",Severity_Error,nullptr,&wStr,
            "Index key rebuilt process has been errored.\n"
            "Index key name <%s>",MasterFile->IndexTable[wIRank]->IndexName.toString());

        ComLog->appendTextColor(ErroredQCl,"%s-E-ERRBLD Index key name <%s> rebuilt process has been errored.\n"
                                            "Exception follows:\n%s",
            ZDateFull::currentDateTime().toFormatted().toString(),
            MasterFile->IndexTable[wIRank]->IndexName.toString(),
            wStr.toString());
        wRebuildErrored++;
        }
        continue;
      }
      ComLog->appendText("Index key rebuilt process has successfully completed.\n"
                         "Index key name <%s>",MasterFile->IndexTable[wIRank]->IndexName.toString());
    }//for (long wi=0 ; wi < wIndexToRebuild.count() ; wi++)
  } //if (wIndexToRebuild.count()!=0)
  else
    ComLog->appendText("No index to rebuild.");

  ComLog->appendText("Deleting change log content");
  int wi=0;
  while (ChangeLog.count() > 0) {
    ChangeLog.pop();
    wi++;
  }


  ComLog->appendHtml("<br>"
                      "Changed file <%s><br>"
                      "<table border=\"1\""
                      "<tr><td>Master file parameter changes</td>     <td>%d</td> </tr>"
                      "<tr><td>Index key changes</td>                 <td>%d</td> </tr>"
                      "<tr><td>Created index keys</td>                <td>%d</td> </tr>"
                      "<tr><td>Suppressed index keys</td>             <td>%d</td> </tr>"
                      "<tr><td>Errors on key change </td>             <td>%d</td> </tr>"
                      "<tr><td>Errors on key rebuild process</td>     <td>%d</td> </tr>"
                      "</table>",
                      wModif,wKeyModif,
                      wKeyCreated, wKeyDeleted,
                      wKeyErrored, wRebuildErrored);


  ComLog->appendText("\nEnd of change log processing.");
  return wSt;
} // applyChangesZmf

ZStatus
FileGenerateMWn::indexRebuildFromMenu(){
  ZKeyListDLg* wKLDLg=new ZKeyListDLg(this);
  wKLDLg->initLayout();
  ZStatus wSt=wKLDLg->displayKeyDictionaries(MasterFile->Dictionary);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wKLDLg->show();
  int wRet=wKLDLg->exec();
  if (wRet==QDialog::Rejected)
    return ZS_REJECTED;

  KeyData wKD;
  wKD = wKLDLg->getKeyData();
  long wIdx=-1;
  for (long wi=0;wi < MasterFile->Dictionary->KeyDic.count();wi++) {
    if (MasterFile->Dictionary->KeyDic[wi]->DicKeyName==wKD.IndexName){
      wIdx=wi;
      break;
    }
  }
  if (wIdx<0) {
    ZExceptionDLg::adhocMessage("Rebuild index key",Severity_Error,"Selected index key named <%s> cannot be found.\n"
        "It must have been deleted meanwhile by another thread.",wKD.IndexName.toString());
    return ZS_NOTFOUND;
  }

  wSt=rebuildIndex(wIdx);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Rebuild index key",Severity_Error,"Errors were encountered while rebuilding index key <%s>.\n",
        wKD.IndexName.toString());
    return wSt;
  }

  return ZS_SUCCESS;
}


ZStatus
FileGenerateMWn::rebuildIndex(long pIndexRankToRebuild) {

/* index rebuild progress window setup */

  ProgressMWn=new ZMFProgressMWn("Index key rebuild",this,false);

  ProgressMWn->advanceSetupCallBack(MasterFile->getRecordCount(),"Index key rebuild");
  ProgressMWn->setDescBudy("Index key name");
  ProgressMWn->setDescText(MasterFile->IndexTable[pIndexRankToRebuild]->IndexName);

  ProgressMWn->show();

  /* rebuild process*/

  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wStr;
  ZDataBuffer wRecord, wKeyRecord;
  zaddress_type wZMFAddress;
  ZIndexItem* wIndexItem=nullptr;
  long wCurrentRank=0L,wRecordCount=0L;
  zmode_type wMasterOpenMode, wIndexOpenMode;

  wMasterOpenMode = MasterFile->getMode();
  if ((MasterFile->getMode()&ZRF_All)!=ZRF_All) {
    ZException.setMessage("ZMasterFile::rebuildIndex",ZS_FILENOTOPEN,Severity_Error,
        "File is open in mode %s. It must be open with access mode ZRF_All.",decode_ZRFMode(MasterFile->getMode()));
    ComLog->appendText("ZMasterFile::rebuildIndex-E-INVMOD File is open in mode %s. It must be open with access mode ZRF_All.",decode_ZRFMode(MasterFile->getMode()));
    ProgressMWn->close(); /* is deleteonclose */
    return ZS_MODEINVALID;
  }
  wRecordCount = MasterFile->getRecordCount();
  if (MasterFile->getRecordCount()==0) {
    ZException.setMessage("ZMasterFile::rebuildIndex",ZS_FILENOTOPEN,Severity_Error,
        "rebuilding index key <%s> - Master file is empty.\n",
        ZDateFull::currentDateTime().toFormatted().toString(),
        MasterFile->IndexTable[pIndexRankToRebuild]->IndexName.toString());

    ComLog->appendTextColor( WarnedQCl, "%s-W rebuilding index key <%s> - Master file is empty.\n",
        ZDateFull::currentDateTime().toFormatted().toString(),
        MasterFile->IndexTable[pIndexRankToRebuild]->IndexName.toString());
    ProgressMWn->close(); /* is deleteonclose */
    return ZS_EMPTY;

  }

  if (pIndexRankToRebuild >= MasterFile->IndexTable.count()) {
    ZException.setMessage("ZMasterFile::rebuildIndex",ZS_OUTBOUND,Severity_Error,
        "Invalid index rank <%ld> while expecting one of [0,%ld[.",pIndexRankToRebuild,MasterFile->IndexTable.count());
    ComLog->appendText("ZMasterFile::rebuildIndex-E-INVRANK Invalid index rank <%ld> while expecting one of [0,%ld[.",pIndexRankToRebuild,MasterFile->IndexTable.count());

    ProgressMWn->close(); /* is deleteonclose */
    return ZS_INVTYPE;
  }


  ComLog->appendText("%s- rebuilding index key <%s> - clearing index file.",
      ZDateFull::currentDateTime().toFormatted().toString(),
      MasterFile->IndexTable[pIndexRankToRebuild]->IndexName.toString());


  ComLog->appendText("%s- rebuilding index key <%s> - clearing index file.",
      ZDateFull::currentDateTime().toFormatted().toString(),
      MasterFile->IndexTable[pIndexRankToRebuild]->IndexName.toString());

  wSt=MasterFile->IndexTable[pIndexRankToRebuild]->zclearFile(-1,MasterFile->IndexTable[pIndexRankToRebuild]->getFCB()->HighwaterMarking,&ErrorLog);
  if (wSt!=ZS_SUCCESS){
    ProgressMWn->close();  /* is deleteonclose */
    return wSt;
  }

  ComLog->appendText("%s- rebuilding index key <%s> - populating index file.",
      ZDateFull::currentDateTime().toFormatted().toString(),
      MasterFile->IndexTable[pIndexRankToRebuild]->IndexName.toString());


  /* browse all master file records, extract key and feed index file */
  wSt=MasterFile->zgetWAddress(wRecord,0L,wZMFAddress);
  if (wSt!=ZS_SUCCESS){
    goto rebuildIndexError;
  }
  while (wSt==ZS_SUCCESS) {
    wSt=MasterFile->extractKeyValues(wRecord,wKeyRecord,pIndexRankToRebuild, &ErrorLog);
    if (wSt!=ZS_SUCCESS)
      goto rebuildIndexError;
    wSt=MasterFile->IndexTable[pIndexRankToRebuild]->_addRawKeyValue_Prepare(wIndexItem,wKeyRecord,wZMFAddress);
    if (wSt!=ZS_SUCCESS)
      goto rebuildIndexError;
    wSt=MasterFile->IndexTable[pIndexRankToRebuild]->_rawKeyValue_Commit(wIndexItem);
    if (wSt!=ZS_SUCCESS)
      goto rebuildIndexError;

    ProgressMWn->AdvanceDisplay(wCurrentRank);

    wSt=MasterFile->zgetNextWAddress(wRecord,wCurrentRank,wZMFAddress);
  } // while (wSt==ZS_SUCCESS)

  if (wSt==ZS_EOF)
    wSt=ZS_SUCCESS;



  if (wSt==ZS_SUCCESS) {  
    ComLog->appendText("%s- rebuilding index key <%s> - rebuild process done successfully.\n"
                       "                writting all headers to files.",
        ZDateFull::currentDateTime().toFormatted().toString(),
        MasterFile->IndexTable[pIndexRankToRebuild]->IndexName.toString());

    QApplication::processEvents();

    wSt = MasterFile->_writeAllHeaders();
  }
  if (wSt==ZS_SUCCESS) {
    return wSt;
  }

//  IndexRebuildMWn->deleteLater();

rebuildIndexError:
    ProgressMWn->setDone(wSt!=ZS_SUCCESS);

  if (ZException.count()==0){
    ZException.setMessage("ZMasterFile::rebuildIndex",wSt,Severity_Error,"Error while rebuilding index");
  }
  else {
    ZException.addToLast(" calling module <ZMasterFile::rebuildIndex>");
  }
  ComLog->appendTextColor( ErroredQCl, "%s-E rebuilding index key <%s> - rebuild process ended with error <%s>.\n"
                                      "Exception follows:\n%s",
      ZDateFull::currentDateTime().toFormatted().toString(),
      MasterFile->IndexTable[pIndexRankToRebuild]->IndexName.toString(),
      decode_ZStatus(wSt),
      ZException.last().formatFullUserMessage().toString());

  QApplication::processEvents();

  return wSt;
} //rebuildIndex

void
FileGenerateMWn::sleepWithLoop(int pTimes) {
  while (pTimes--) {
  sleep(1);
  QApplication::processEvents();
  }
  QApplication::processEvents();
}


void
FileGenerateMWn::changeKeySize(KeyData& pKeyData,size_t pNewKeySize) {
  pKeyData.KeySize = pNewKeySize;
  if (pKeyData.Allocated==0){
    pKeyData.Allocated = cst_ZRF_default_allocation;
  }
  pKeyData.AllocatedSize =  pKeyData.Allocated * pNewKeySize;
}



bool
FileGenerateMWn::ValuesControl() {
  utf8VaryingString wStr, wErrComp;

  wStr.sprintf("Controlling values for (raw) master file creation or change ");
  ComLog->appendText(wStr);
  int wErrored=0;

  if (AllocatedBlocks==0) {
    wErrComp += "Raw master file : Allocated blocks cannot be zero.\n";
    wErrored++;
  }
  if (ExtentQuota==0) {
    wErrComp += "Raw master file : ExtentQuota cannot be zero.\n";
    wErrored++;
  }
  if (MeanRecordSize==0) {
    wErrComp += "Raw master file : MeanRecordSize cannot be zero.\n";
    wErrored++;
  }
  if (AllocatedSize==0) {
    wErrComp += "Raw master file : AllocatedSize cannot be zero.\n";
    wErrored++;
  }
  if (MeanRecordSize > AllocatedSize) {
    wErrComp += "Raw master file : MeanRecordSize cannot be greater than AllocatedSize.\n";
    wErrored++;
  }

  if (KeyValues->count() > 0) {
    for (long wi=0;wi < KeyValues->count(); wi ++) {
      if (KeyValues->Tab(wi).IndexName.isEmpty()) {
        wErrComp.addsprintf("Index <%ld> : IndexName cannot be empty.\n",wi);
        wErrored++;
      }
      if (KeyValues->Tab(wi).KeySize==0) {
        wErrComp.addsprintf("Index <%ld><%s> : KeySize cannot be zero.\n",wi,KeyValues->Tab(wi).IndexName.toString());
        wErrored++;
      }
      if (KeyValues->Tab(wi).Allocated==0) {
        wErrComp.addsprintf("Index <%ld><%s> : Allocated blocks cannot be zero.\n",wi,KeyValues->Tab(wi).IndexName.toString());
        wErrored++;
      }
      if (KeyValues->Tab(wi).ExtentQuota==0) {
        wErrComp.addsprintf("Index <%ld> : ExtentQuota cannot be zero.\n",wi,KeyValues->Tab(wi).IndexName.toString());
        wErrored++;
      }
      if (KeyValues->Tab(wi).AllocatedSize==0) {
        wErrComp.addsprintf("Index <%ld><%s> : AllocatedSize cannot be zero.\n",wi,KeyValues->Tab(wi).IndexName.toString());
        wErrored++;
      }
      if (KeyValues->Tab(wi).KeySize > KeyValues->Tab(wi).AllocatedSize) {
        wErrComp.addsprintf("Index <%ld><%s> : KeySize cannot be greater than AllocatedSize.\n",wi,KeyValues->Tab(wi).IndexName.toString());
        wErrored++;
      }
    }// for
  }// if (KeyValues->count() > 0)

  if (wErrored > 0) {
    ComLog->appendText(wErrComp);
    ZExceptionDLg::adhocMessage("Master file creation",Severity_Error,nullptr,&wErrComp,
                                "%d errors have been detected.\n"
                                "click <More> to have a detailed report",wErrored);

    return true;
  }
  ComLog->appendText("Controls passed successfully.");
  return false;
}


void FileGenerateMWn::GenFile() {

  if (RootName.isEmpty()){
    ZExceptionDLg::adhocMessage("Master file creation",Severity_Error,nullptr,nullptr,
        "File root name is missing.\n"
        "Cannot create file without root name.");
    return;
  }

  _refresh(); /* update some booleans like GrabFreeSpace */

  utf8VaryingString wStr;
  utf8VaryingString wErrComp;
  ZaiErrors wErrorLog;
  ZRawMasterFile* wMasterFile=new ZRawMasterFile;
  uriString wURIFile = TargetDirectory ;
  wURIFile.addConditionalDirectoryDelimiter();
  wURIFile += RootName;
  wURIFile += __MASTER_FILEEXTENSION__;

  wStr.sprintf("Creating (raw) master file <%s>",wURIFile.toString());
  ComLog->appendText(wStr);

  bool wBackup=false;
  const char* wF=nullptr;
  long wBadRank=0;

  utf8VaryingString wMissfields;
  for (long wi=0;wi < KeyValues->count(); wi ++) {
    if (KeyValues->Tab(wi).KeySize==0){
      wF="Guessed key size";
      wBadRank=wi;
      wMissfields.addsprintf(" Key <%s> field <%s> value 0 is invalid and must be guessed.\n",
          DictionaryFile->KeyDic[wBadRank]->DicKeyName.toString(),
          wF);
    }
    if (KeyValues->Tab(wi).Allocated==0){
      wF="Allocated blocks";
      wBadRank=wi;
      wMissfields.addsprintf(" Key <%s> field <%s> value 0 is invalid and must be guessed.\n",
          DictionaryFile->KeyDic[wBadRank]->DicKeyName.toString(),
          wF);
    }
    if (KeyValues->Tab(wi).ExtentQuota==0){
      wF="Extent quota";
      wBadRank=wi;
      wMissfields.addsprintf(" Key <%s> field <%s> value 0 is invalid and must be guessed.\n",
          DictionaryFile->KeyDic[wBadRank]->DicKeyName.toString(),
          wF);
    }
    if (KeyValues->Tab(wi).AllocatedSize==0){
      wF="Allocated size";
      wBadRank=wi;
      wMissfields.addsprintf(" Key <%s> field <%s> value 0 is invalid and must be guessed.\n",
          DictionaryFile->KeyDic[wBadRank]->DicKeyName.toString(),
          wF);
    }
  }

  if (wF!=nullptr) {
    ZExceptionDLg::adhocMessage("File generation",Severity_Error,nullptr,&wMissfields,
        "Cannot create file. Some invalid values have been detected.\n"
        "click <More> to have details.");
    return;
  }

  if (ValuesControl()) {
    return;
  }

  if (wURIFile.exists()) {
    uriStat wStat;
    wURIFile.getStatR(wStat);

    int wRet = ZExceptionDLg::adhocMessage3BHtml("Master file creation",Severity_Warning,"Backup","Cancel","Replace",
        nullptr,nullptr,
        " Found existing file <%s> \n"
        " It has been created <%s> and last modified <%s>\n"
        "<table>"
        "<tr><td>creates the file but backs up existing file</td>      <td><b>Backup</b></td> </tr>"
        "<tr><td>replaces existing file without backup</td>    <td><b>Replace</b></td> </tr>"
        "<tr><td>give up action</td>    <td><b>Cancel</b></td> </tr>"
        "</table>",
        wURIFile.toString(),
        wStat.Created.toFormatted().toString(),
        wStat.LastModified.toFormatted().toString());
    if (wRet==QDialog::Rejected)
      return;

    if (wRet==ZEDLG_Third) {
      wBackup=true;
    }

  }// if (wURIFile.exists())


  ZStatus wSt = wMasterFile->zcreateRawMasterFile( wURIFile,
                                                AllocatedBlocks,
                                                ExtentQuota,
                                                MeanRecordSize,
                                                AllocatedSize,
                                                HighWaterMarking,
                                                GrabFreeSpace,
                                                Journaling,
                                                wBackup,
                                                true);

  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Master file creation",Severity_Error,nullptr,nullptr,
        " Error while creating raw master file <%s> \n"
        " status <%s>",
        wURIFile.toString(),
        decode_ZStatus(wSt));
    return;
  }
  wStr.sprintf("%s - created Master file <%s>.",
      ZDateFull::currentDateTime().toFormatted().toString(),
          wURIFile.toString());
  ComLog->appendText(wStr);

  /* create all indexes */
  uriString wIndexUri;
  long wIndexRank;


  if (IndexDirectory.isEmpty()){
    ComLog->appendText("%s - Index directory is omitted.",
        ZDateFull::currentDateTime().toFormatted().toString());
  }
  else {
    wSt=wMasterFile->setIndexFilesDirectoryPath(IndexDirectory);
    if (wSt!=ZS_SUCCESS) {
      utf8VaryingString wErrContent = ZException.last().formatFullUserMessage();
      ZExceptionDLg::adhocMessage("Set index path",Severity_Severe,nullptr,&wErrContent,
          "Error while setting index directory <%ld> ",
          IndexDirectory.toString());
      ComLog->appendText("Error while setting index directory <%ld> ",
          IndexDirectory.toString());
      ComLog->appendText(wErrContent);

      wMasterFile->zclose();
      ComLog->appendText("Master file closed without index.");
      return;
    }

    ComLog->appendText("%s - set index directory to <%s>.",
                        ZDateFull::currentDateTime().toFormatted().toString(),
                        IndexDirectory.toString());
  }


  if (KeyValues->count() == 0) {
    ComLog->appendText("%s - No index key has been defined.",
        ZDateFull::currentDateTime().toFormatted().toString());
  }
  else {
    for (long wi=0;wi < KeyValues->count(); wi ++) {
      wIndexUri = IndexDirectory ;
      wIndexUri.addConditionalDirectoryDelimiter();
      wIndexUri += KeyValues->Tab(wi).IndexRootName;

       wSt = wMasterFile->_createRawIndexDet (wIndexRank,                               /* returned key rank */
                                              KeyValues->Tab(wi).IndexName,   /* Index name */
                                              KeyValues->Tab(wi).KeySize, /* Key universal total size */
                                              KeyValues->Tab(wi).Duplicates,
                                              KeyValues->Tab(wi).Allocated,      /* ---FCB parameters (for index ZRandomFile)---- */
                                              KeyValues->Tab(wi).ExtentQuota,
                                              KeyValues->Tab(wi).AllocatedSize,
                                              HighWaterMarking,
                                              GrabFreeSpace,
                                              wBackup,
                                              &wErrorLog);
       if (wSt!=ZS_SUCCESS) {
         utf8VaryingString wErrContent = ZException.last().formatFullUserMessage();
         ZExceptionDLg::adhocMessage("Raw index creation",Severity_Severe,nullptr,&wErrContent,"Error while creating index rank <%ld> name <%s> file <%s>",
             wi,DictionaryFile->KeyDic[wi]->DicKeyName.toString(),wMasterFile->getURIContent().toString());

         ComLog->appendTextColor(ErroredQCl,"%s-E-CREIDX Error while creating index rank <%ld> name <%s> for file <%s>\n"
             "Exception follows.\n%s",
             ZDateFull::currentDateTime().toFormatted().toString(),
             wi,DictionaryFile->KeyDic[wi]->DicKeyName.toString(),
             wMasterFile->getURIContent().toString(),
             wErrContent.toString());


         wMasterFile->zclose();
         ComLog->appendText("Master file closed.");
         return;
       }
       wStr.sprintf("%s - created index file <%s>.",
           ZDateFull::currentDateTime().toFormatted().toString(),
           wMasterFile->getURIIndex(wIndexRank).toString());
       ComLog->appendText(wStr);
    } // for
  }// else

  wStr.sprintf("%s - end creation.",
      ZDateFull::currentDateTime().toFormatted().toString());
//  plainTextEdit->appendPlainText(wStr.toCChar());
  ComLog->appendText(wStr);


  /* File generation choices

  Dictionary file exists and Master file does not exist
          -> may create a ZMasterFile with embedded dictionary
          -> a raw master file with no embedded dictionary

              MasterFile exists (has been loaded) and Dictionary is null :
          -> MasterFile has no dictionary -> create a raw master file
          -> MasterFile has a dictionary -> may create a ZMasterFile with this embedded dictionary


      if both exist :
          -> create raw master file (no dictionary)
          -> create ZMF and update dictionary with given dictionary

          if none exist :
              -> create a raw master file with given parameters with no embedded dictionary.

          */

  if (DictionaryFile==nullptr) {
    if (MasterFile==nullptr) {
      ComLog->appendText("No dictionary is available either embedded or loaded.");
      goto GenFileNoDic;
    }
    if (MasterFile->Dictionary==nullptr) {
      ComLog->appendText("Loaded master file has no dictionary : no dictionary has been embedded into created file.");
      goto GenFileNoDic;
    }
    /* here loaded master file has a dictionary */
    ZExceptionDLg::setFixedFont(true);
    int wRet=ZExceptionDLg::adhocMessage2BHtml("Generate file",Severity_Information,
        "Raw","Embedded",
        nullptr,nullptr,
        "A master file is loaded <%s>.\n"
        "Current Master file has its own embedded dictionary <%s>.\n"
        "<table>"
        "<th><tr>Create file </tr></th>"
        "<tr><td> as raw master file without dictionary</td>      <td><b>Raw</b></td> </tr>"
        "<tr><td> as a master file using embedded dictionary</td>    <td><b>Embedded</b></td> </tr>"
        "</table>",
        MasterFile->getURIContent().toString(),
        MasterFile->Dictionary->DicName.toString());
    if (wRet==ZEDLG_Rejected){
      goto GenFileNoDic;
    }
    wSt=wMasterFile->createDictionary(*MasterFile->Dictionary);
    if (wSt!=ZS_SUCCESS){
      wErrComp = ZException.last().formatFullUserMessage();
      ZExceptionDLg::adhocMessage("Embedding dictionary",Severity_Error,&ErrorLog,&wErrComp,
          "Cannot embed dictionary in master file.\n"
          "A raw master file with no dictionary is created and available.\n");
      goto GenFileNoDic;
    }
    goto GenFileEnd;
  } // if (DictionaryFile==nullptr)

  if (DictionaryFile != nullptr) {
    if (MasterFile!=nullptr) {

      if (MasterFile->Dictionary!=nullptr) {
        ZExceptionDLg::setFixedFont(true);
        int wRet=ZExceptionDLg::adhocMessage3BHtml("Generate file",Severity_Information,
            "Raw","Loaded","Embedded",
            nullptr,nullptr,
            "A dictionary is loaded <%s>.\n"
            "A master file is loaded <%s>.\n"
            "Master file has its own dictionary <%s>.\n"
            " \n"
            "<table>"
            "<th><td>Create file</td></th>"
            "<tr><td>as raw master file without dictionary</td>             <td><b>Raw</b></td></tr>"
            "<tr><td>as master file using loaded file dictionary </td>      <td><b>Loaded</b></td></tr>"
            "<tr><td>as master file  using embedded file dictionary  </td>  <td><b>Embedded</b></td></tr>"
            "</table>",
            DictionaryFile->DicName.toString(),
            MasterFile->getURIContent().toString(),
            MasterFile->Dictionary->DicName.toString());
        switch (wRet) {
        case ZEDLG_Rejected:  /* include loaded dictionary */
          wSt=wMasterFile->createDictionary(DictionaryFile->getDictionary());
          if (wSt!=ZS_SUCCESS){
            wErrComp = ZException.last().formatFullUserMessage();
            ZExceptionDLg::adhocMessage("Embedding dictionary",Severity_Error,&ErrorLog,&wErrComp,
                "Cannot embed dictionary in master file.\n"
                "A raw master file with no dictionary is created and available.\n");
            goto GenFileNoDic;
          }
          goto GenFileEnd;
        case ZEDLG_Accepted: /* Embedded dictionary in master file */
          wSt=wMasterFile->createDictionary(*MasterFile->Dictionary);
          if (wSt!=ZS_SUCCESS){
            wErrComp = ZException.last().formatFullUserMessage();
            ZExceptionDLg::adhocMessage("Embedding dictionary",Severity_Error,&ErrorLog,&wErrComp,
                "Cannot embed dictionary in master file.\n"
                "A raw master file with no dictionary is created and available.\n");
            goto GenFileNoDic;
          }
          goto GenFileEnd;
        case ZEDLG_Third: /* Raw requested */
          goto GenFileNoDic;
        }// switch
        ComLog->appendText("No dictionary has been embedded.");
        goto GenFileEnd;
      } //if (MasterFile->Dictionary!=nullptr)
      ZExceptionDLg::setFixedFont(true);
      int wRet=ZExceptionDLg::adhocMessage2B("Generate file",Severity_Information,
          "Raw","Loaded",
          nullptr,nullptr,
          "A dictionary has been loaded <%s>.\n"
          "A raw master file has been loaded <%s>.\n"
          "Raw master file has no dictionary.\n"
          "Create file \n"
          "<table>"
          "<tr><td>as raw master file without dictionary</td>      <td><b>Raw</b></td></tr>"
          "<tr><td>as a master file using loaded file dictionary</td>    <td><b>Loaded</b></td></tr>"
          "</table>",
          DictionaryFile->DicName.toString(),
          MasterFile->getURIContent().toString());
      switch (wRet) {
      case ZEDLG_Accepted:  /* include loaded dictionary */
        wSt=createDic(DictionaryFile->getDictionary(),wMasterFile->getURIContent());
        if (wSt!=ZS_SUCCESS){
          utf8VaryingString wErrComp = ZException.last().formatFullUserMessage();
          ZExceptionDLg::adhocMessage("Embedding dictionary",Severity_Error,&ErrorLog,&wErrComp,
              "Cannot embed dictionary in master file.\n"
              "A raw master file with no dictionary is created and available.\n");
          goto GenFileNoDic;
        }
        ComLog->appendText("Dictionary %s has been embedded (from loaded) into created file.",
            DictionaryFile->getDictionary().DicName.toString());
        goto GenFileEnd;
      case ZEDLG_Rejected: /* Raw requested */
        goto GenFileNoDic;
      }// switch
      goto GenFileNoDic;
    }//if (MasterFile!=nullptr)
  } // if (DictionaryFile != nullptr)


  if (DictionaryFile!=nullptr) {
    ZExceptionDLg::setFixedFont();
      int wRet=ZExceptionDLg::adhocMessage2BHtml("Generate file",Severity_Information,
          "Raw","Loaded",
          nullptr,nullptr,
          "A dictionary is loaded <%s>.\n\n"
          "No master file has been loaded.\n"
          "<table>"
          "<th><td>Create file</td></th>"
          "<tr><td>as raw master file without dictionary</td>      <td><b>Raw</b></td></tr>"
          "<tr><td>as master file using loaded file dictionary </td>    <td><b>Loaded</b></td></tr>"
          "</table>",
          DictionaryFile->DicName.toString());
      switch (wRet) {
      case ZEDLG_Accepted:  { /* include loaded dictionary */
        wSt=wMasterFile->createDictionary(DictionaryFile->getDictionary());
        if (wSt==ZS_SUCCESS) {
          ComLog->appendText("Embedded dictionary file <%s> has been loaded.",wMasterFile->Dictionary->URIDictionary.toString());
          ComLog->appendText("Dictionary has been embedded as active dictionary for master file %s",wMasterFile->getURIContent().toString());
          goto GenFileEnd;
        }
        utf8VaryingString wErrComp = ZException.last().formatFullUserMessage();
        ZExceptionDLg::adhocMessage("Embedding dictionary",Severity_Error,&ErrorLog,&wErrComp,
              "Cannot embed dictionary in master file.\n"
              "However a raw master file with no dictionary is created and remains available.");

        ComLog->appendTextColor(ErroredQCl,"%s-E-EMBEDDIC  Cannot embed dictionary in master file.\n"
                                            "However a raw master file with no dictionary is created and available.\n"
                                            "Exception follows:\n"
                                            "%s",
            ZDateFull::currentDateTime().toFormatted().toString(),wErrComp.toString());

        goto GenFileNoDic;
      } // ZEDLG_Accepted
      case ZEDLG_Rejected: /* Raw requested */
        goto GenFileNoDic;
      }// switch
      ComLog->appendText("No dictionary has been embedded.");
      goto GenFileEnd;
  } //if (MasterFile->Dictionary!=nullptr)



GenFileNoDic:
  ComLog->appendText("No dictionary has been embedded.");

GenFileEnd:

  uriString wURIMaster = wMasterFile->getURIContent().toString();
  wMasterFile->zclose();
  ComLog->appendText("Master file closed.");

  ComLog->appendText("Deleting change log content");
  int wi=0;
  while (ChangeLog.count() > 0) {
    ChangeLog.pop();
    wi++;
  }

  ComLog->appendText("Change log content deleted\n"
                     "%d elements deleted.",wi);



  if (MasterFile!=nullptr){
    if (MasterFile->isOpen()){
      ComLog->appendText("Current master file %s is to be closed.",MasterFile->getURIContent().toString());
      MasterFile->zclose();
      ComLog->appendText("Closed.");
    }
    delete MasterFile;
    MasterFile = nullptr;
  }// if (MasterFile!=nullptr)

  MasterFile=new ZMasterFile;
  wSt=MasterFile->zopen(wURIMaster,ZRF_All | ZRF_TypeRegardless);
  if (wSt!=ZS_SUCCESS) {
    wErrComp = ZException.last().formatFullUserMessage();
    ZExceptionDLg::adhocMessage("Open master file",Severity_Error,&ErrorLog,&wErrComp,
        "Cannot set master file <%s> as current.\n",wURIMaster.toString());
    delete MasterFile;
    MasterFile=nullptr;
    return;
  }



  SourceContentLBl->setText(MasterFile->getURIContent().toCChar());

  if (MasterFile->hasDictionary()) {
    SourceLBl->setText("Master file");

    wStr.sprintf("Embedded dictionary <%s>",MasterFile->Dictionary->DicName.toCChar());
    DicEmbedLBl->setText(wStr.toCChar());
    ComLog->appendText("Current master file is set to %s.",MasterFile->getURIContent().toString());
  }
  else {
    SourceLBl->setText("Raw master file");
    if (DictionaryFile==nullptr) {
      DicEmbedLBl->setText("No embedded dictionary");
      ComLog->appendText("Current raw master file is set to %s.",MasterFile->getURIContent().toString());
    }
    else {
      wStr.sprintf("Embedded dictionary <%s>",DictionaryFile->DicName.toCChar());
      DicEmbedLBl->setText(wStr.toCChar());
    }
  }

  ErrorLog.textLog("\n Created file <%s>\n",MasterFile->getURIContent().toString());

  MasterFile->FCBReport(&ErrorLog);
  MasterFile->PoolReport(&ErrorLog);
  MasterFile->MCBreport(&ErrorLog);


  return;
} //GenFile

ZStatus FileGenerateMWn::createDic(ZMFDictionary& pDic,const uriString& pURIMaster) {
  DictionaryFile = new ZDictionaryFile();

  return DictionaryFile->saveAsEmbedded(pURIMaster);
}
ZStatus
FileGenerateMWn::checkDic()
{
    return ZS_SUCCESS;
}
void FileGenerateMWn::displayChangeLog(){
  if (ChangeLogMWn==nullptr) {
    ChangeLogMWn = new textEditMWn(this,TEOP_NoFileLab | TEOP_CloseBtnHide,nullptr);
    ChangeLogMWn->setWindowTitle("Change log");
  }
  ChangeLogMWn->clear();
  if (ChangeLog.count()==0) {
    ChangeLogMWn->appendText("No change to display");
    return;
  }
  for (long wi=0;wi < ChangeLog.count();wi++) {
    displayChangeLine(ChangeLog[wi]);
  }

  ChangeLogMWn->show();
  ChangeLogMWn->setFocus();
} //displayChangeLog

void FileGenerateMWn::displayChangeLine(const ZChangeRecord& pChgRec) {
  ChangeLogMWn->appendText("<%25s> <%s> <%s> <%ld>",
                            pChgRec.getChangeKey().toString(),
                            pChgRec.getChangeCode_Str().toString(),
                            pChgRec.getZType_Str().toString(),
                            pChgRec.getIndexRank());
  ChangeLogMWn->appendText("                     ante: %s", pChgRec.getAnte_Str().toString());
  ChangeLogMWn->appendText("                     post: %s", pChgRec.getPost_Str().toString());
}


void FileGenerateMWn::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (FResizeInitial)
  {
    FResizeInitial=false;
    return;
  }
  QRect wR1 = verticalLayoutWidget->geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  verticalLayoutWidget->resize(wVW,wVH);  /* expands in width and height */

}//ZContentVisuMain::resizeEvent

