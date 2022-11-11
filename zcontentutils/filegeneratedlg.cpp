#include "filegeneratedlg.h"
//#include "ui_filegeneratedlg.h"

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

#include <ztoolset/zlimit.h>

#include <zcontent/zindexedfile/zdictionaryfile.h>

#include <zcontent/zindexedfile/zdataconversion.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <QStandardItemModel>
#include <QStandardItem>

#include <ztoolset/zarray.h>

#include <zqt/zqtwidget/zqtwidgettools.h>

#include <QMenu>
#include <QMenuBar>

#include <QFileDialog>
#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory
#include <zcontent/zindexedfile/zmasterfile_utilities.h> // for generateIndexRootName

#include <zcontent/zindexedfile/zmasterfile.h>

#include <zexceptiondlg.h>

#include <zxml/zxmlprimitives.h>

using namespace zbs;
void
FileGenerateDLg::initLayout() {

  setWindowTitle(QCoreApplication::translate("FileGenerateDLg", "Generate master file", nullptr));

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

  SearchDirQAc=new QAction(QObject::tr( "Search master directory", "FileGenerateDLg") ,GenMEn);
  IndexSearchDirQAc=new QAction(QObject::tr( "Search indexes directory", "FileGenerateDLg") ,GenMEn);
  //  GenXmlQAc=new QAction(QObject::tr( "Generate xml description", "FileGenerateDLg") ,GenMEn);
  GenFileQAc=new QAction(QObject::tr( "Generate master file", "FileGenerateDLg") ,GenMEn);

  LoadFromFileQAc=new QAction(QObject::tr( "Load definition from existing file", "FileGenerateDLg") ,GenMEn);

  QuitQAc=new QAction(QObject::tr( "Quit", "FileGenerateDLg") ,GenMEn);

  GenActionGroup = new QActionGroup(this);
  GenActionGroup->addAction(SearchDirQAc);
  GenActionGroup->addAction(IndexSearchDirQAc);
  //  GenActionGroup->addAction(GenXmlQAc);
  GenActionGroup->addAction(GenFileQAc);
  GenActionGroup->addAction(LoadFromFileQAc);
  GenActionGroup->addAction(QuitQAc);

  GenMEn->addAction(SearchDirQAc);
  GenMEn->addAction(IndexSearchDirQAc);
  GenMEn->addAction(GenFileQAc);
  GenMEn->addAction(LoadFromFileQAc);
  // GenMEn->addAction(GenXmlQAc);
  GenMEn->addAction(QuitQAc);

  /* directory and basename */

  QGroupBox* wGBB= new QGroupBox(QObject::tr( "File name and location", "FileGenerateDLg") ,(QWidget*)this);
  QVBoxLayout* wVBB=new QVBoxLayout(wGBB);
  wGBB->setLayout(wVBB);
  QHBoxLayout* wHBDir = new QHBoxLayout;
  wVBB->addLayout(wHBDir);
  DirectoryLBl = new QLabel(wGBB);
  DirectoryLBl->setText(getParserWorkDirectory());
  TargetDirectory = getParserWorkDirectory();
  wHBDir->addWidget(DirectoryLBl);

  SearchDirBTn=new QPushButton(QObject::tr( "Search", "FileGenerateDLg"),wGBB);
  SearchDirBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  wHBDir->addWidget(SearchDirBTn);

  QHBoxLayout* wHBB = new QHBoxLayout;
  wVBB->addLayout(wHBB);

  QLabel* wLbRoot=new QLabel(QObject::tr("Root name","FileGenerateDLg"),wGBB);
  wHBB->addWidget(wLbRoot);

  RootNameLEd = new QLineEdit(wGBB);

  if (DictionaryFile->DicName.isEmpty()) {
    RootNameLEd->setText("<no name>");
  } else {
    RootName = DictionaryFile->DicName;
    RootName.eliminateChar(' ');
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


  //  VLYt->insertWidget(1,wGB0);
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
  IndexDirectoryLBl = new QLabel(verticalLayoutWidget);
  IndexDirectoryLBl->setText(DirectoryLBl->text());

  HLYt2->addWidget(IndexDirectoryLBl);

  KeyTBv = new ZQTableView(verticalLayoutWidget);
  HLYt2->addWidget(KeyTBv);

  VLYt->addWidget(wGB2);
  KeyTBv->setGeometry(QRect(0, 0, 490, 100));

  QGroupBox* wGB4 = new QGroupBox(QObject::tr("Guessed values","FileGenerateDLg"),verticalLayoutWidget);
  QHBoxLayout* HLYt4 = new QHBoxLayout;
  wGB4->setLayout(HLYt4);

  GuessTBv = new ZQTableView(verticalLayoutWidget);
  HLYt4->addWidget(GuessTBv);

  VLYt->addWidget(wGB4);
  /* plain text  */
  QGroupBox* wGB3 = new QGroupBox(QObject::tr("Communication log","FileGenerateDLg"),verticalLayoutWidget);
  QHBoxLayout* HLYt3 = new QHBoxLayout;
  wGB3->setLayout(HLYt3);

  plainTextEdit = new QPlainTextEdit(verticalLayoutWidget);
  HLYt3->addWidget(plainTextEdit);
  //  VLYt->insertLayout(4,HLYt3);
  VLYt->addWidget(wGB3);
  plainTextEdit->setGeometry(QRect(0, 0, 490, 20));


  /* button box */
  QHBoxLayout* buttonBoxLayout= new QHBoxLayout;

  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  buttonBoxLayout->addItem(wSpacer);

  GenerateBTn=new QPushButton(verticalLayoutWidget);
  GenerateBTn->setText(QObject::tr("Generate","FileGenerateDLg"));
  DiscardBTn=new QPushButton(verticalLayoutWidget);
  DiscardBTn->setText(QObject::tr("Discard","FileGenerateDLg"));

  buttonBoxLayout->addWidget(DiscardBTn);

  buttonBoxLayout->addWidget(GenerateBTn);
  verticalLayoutWidget->setContentsMargins(2, 2, 2, 2);

  VLYt->insertLayout(5,buttonBoxLayout);

  QObject::connect(ComputeBTn, &QPushButton::clicked, this, &FileGenerateDLg::Compute);

  QObject::connect(GenerateBTn, &QPushButton::clicked, this, &FileGenerateDLg::GenFile);
  QObject::connect(DiscardBTn, &QPushButton::clicked, this, &FileGenerateDLg::Quit);
  QObject::connect(SearchDirBTn, &QPushButton::clicked, this, &FileGenerateDLg::SearchDir);

  QObject::connect(RootNameLEd, &QLineEdit::editingFinished, this, &FileGenerateDLg::BaseNameEdit);
  QObject::connect(AllocatedLEd, &QLineEdit::editingFinished, this, &FileGenerateDLg::AllocatedEdit);
  QObject::connect(ExtentQuotaLEd, &QLineEdit::editingFinished, this, &FileGenerateDLg::ExtentQuotaEdit);
  QObject::connect(MeanSizeLEd, &QLineEdit::editingFinished, this, &FileGenerateDLg::MeanSizeEdit);

  QObject::connect(GenActionGroup, &QActionGroup::triggered, this, &FileGenerateDLg::MenuAction);

  QStandardItemModel* wModel=new QStandardItemModel(KeyTBv);
  KeyTBv->setModel(wModel);

  KeyTBv->ItemModel->setColumnCount(8);

  KeyTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem(tr("Name")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem(tr("Index base name")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem(tr("Key size")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem(tr("Duplicates")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(4,new QStandardItem(tr("Allocated")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(5,new QStandardItem(tr("Allocated Size")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(6,new QStandardItem(tr("Extent quota")));
  KeyTBv->ItemModel->setHorizontalHeaderItem(7,new QStandardItem(tr("Extent Size")));


  KeyTBv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  KeyTBv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  GuessTBv->setModel(new QStandardItemModel(GuessTBv));
  GuessTBv->ItemModel->setColumnCount(4);
  GuessTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem(tr("Name")));
  GuessTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem(tr("Type")));
  GuessTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem(tr("Header size")));
  GuessTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem(tr("Guessed size (including header)")));

}

FileGenerateDLg::FileGenerateDLg(const ZDictionaryFile* pDictionary, QWidget *parent) : QMainWindow(parent)
{
  DictionaryFile=pDictionary;

  KeyValues = new ZArray<KeyData>;
  initLayout();
  dataSetupFromDictionary();
}

FileGenerateDLg::FileGenerateDLg( QWidget *parent) : QMainWindow(parent)
{
  KeyValues = new ZArray<KeyData>;
  initLayout();
  LoadFromFile();

//  dataSetupFromDictionary();
}

FileGenerateDLg::~FileGenerateDLg()
{
//  delete ui;
}

void
FileGenerateDLg::dataSetupFromDictionary() {
  utf8VaryingString wStr;

  AllocatedBlocks = cst_ZRF_default_allocation;
  wStr.sprintf("%ld",AllocatedBlocks);
  AllocatedLEd->setText(wStr.toCChar());


  ExtentQuota = cst_ZRF_default_extentquota;
  wStr.sprintf("%ld",ExtentQuota);
  ExtentQuotaLEd->setText(wStr.toCChar());

  Compute();

  QList<QStandardItem*> wKeyRow;
  for (long wi=0; wi < DictionaryFile->KeyDic.count() ; wi++) {

    wKeyRow.clear();
    ZKeyDictionary* wKeyDic=DictionaryFile->KeyDic.Tab[wi];

    KeyData wKeyData;
    wKeyData.KeySize = AllocatedBlocks*wKeyDic->computeKeyUniversalSize();
    wKeyData.Allocated = AllocatedBlocks;
    wKeyData.AllocatedSize = wKeyData.KeySize * wKeyData.Allocated;
    wKeyData.ExtentQuota = ExtentQuota;
    wKeyData.ExtentSize = wKeyData.KeySize * wKeyData.ExtentQuota;
    wKeyData.IndexRootName = generateIndexRootName(RootName,wi,wKeyDic->DicKeyName);
    wKeyData.Duplicates   = DictionaryFile->KeyDic.Tab[wi]->Duplicates;
    KeyValues->push(wKeyData);

    wKeyRow.push_back(new QStandardItem(wKeyDic->DicKeyName.toCChar()));

    wKeyRow << new QStandardItem(wKeyData.IndexRootName.toCChar());

    wStr.sprintf("%ld",wKeyData.KeySize);
    wKeyRow.push_back(new QStandardItem(wStr.toCChar()));
    wKeyRow.last()->setTextAlignment(Qt::AlignRight);

    QStandardItem* wDup=new QStandardItem("Duplicates");
    wDup->setTextAlignment(Qt::AlignLeft);

    if (wKeyDic->Duplicates)
      wDup->setCheckState(Qt::Checked);
      else
      wDup->setCheckState(Qt::Unchecked);

    wDup->setEditable(false);
    wKeyRow.push_back(wDup);

    wKeyRow << createItemAligned(wKeyData.Allocated,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.AllocatedSize,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.ExtentQuota,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.ExtentSize,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    KeyTBv->ItemModel->appendRow(wKeyRow);

  }// for

  for (int wi=0; wi < KeyTBv->ItemModel->columnCount(); wi++)
    KeyTBv->resizeColumnToContents(wi);
  for (int wi=0; wi < KeyTBv->ItemModel->rowCount(); wi++)
    KeyTBv->resizeRowToContents(wi);

  QVariant wV;

  for (long wi=0;wi < DictionaryFile->count();wi++) {
    if (DictionaryFile->Tab[wi].UniversalSize==0) {
      QList<QStandardItem*> wGuessRow;
      wGuessRow << createItem(DictionaryFile->Tab[wi].getName());
      wGuessRow[0]->setEditable(false);
      wGuessRow << createItem(decode_ZType( DictionaryFile->Tab[wi].ZType));
      wGuessRow.last()->setEditable(false);
      wGuessRow << createItem(DictionaryFile->Tab[wi].HeaderSize);
      wGuessRow.last()->setEditable(false);
      wGuessRow << createItem(DictionaryFile->Tab[wi].UniversalSize);
      wGuessRow.last()->setEditable(true);

      GuessTBv->ItemModel->appendRow(wGuessRow);

      wStr.sprintf( "%s-Value for field <%s> has not been guessed and remains invalid.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wGuessRow[0]->text().toUtf8().data());
      plainTextEdit->appendPlainText(wStr.toCChar());
    }
  }// for


  for (int wi=0;wi < GuessTBv->ItemModel->columnCount();wi++)
    GuessTBv->resizeColumnToContents(wi);
  for (int wi=0;wi < GuessTBv->ItemModel->rowCount();wi++)
    GuessTBv->resizeRowToContents(wi);


  QObject::connect(GuessTBv->ItemModel,&QStandardItemModel::itemChanged,this,&FileGenerateDLg::GuessItemChanged);

} // dataSetupFromDictionary


void
FileGenerateDLg::dataSetupFromMasterFile(const uriString& pURIMaster) {
  utf8VaryingString wStr;


  ZRawMasterFile wMasterFile;
  ZStatus wSt=wMasterFile.zopen(pURIMaster,ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Opening Master file");
    return;
  }

  TargetDirectory = wMasterFile.getURIContent().getDirectoryPath();
  DirectoryLBl->setText(TargetDirectory.toCChar());

  RootName = wMasterFile.getURIContent().getRootname();
  RootNameLEd->setText(RootName.toCChar());

  MeanRecordSize = wMasterFile.getFCB()->BlockTargetSize;
  wStr.sprintf("%ld",MeanRecordSize);
  MeanSizeLEd->setText(wStr.toCChar());

  AllocatedBlocks = wMasterFile.getFCB()->AllocatedBlocks;
  wStr.sprintf("%ld",AllocatedBlocks);
  AllocatedLEd->setText(wStr.toCChar());

  ExtentQuota = wMasterFile.getFCB()->BlockExtentQuota;
  wStr.sprintf("%ld",ExtentQuota);
  ExtentQuotaLEd->setText(wStr.toCChar());

  InitialSize= wMasterFile.getFCB()->InitialSize;
  wStr.sprintf("%ld",InitialSize);
  InitialSizeLEd->setText(wStr.toCChar());

  HighWaterMarkingCHk->setCheckState(wMasterFile.getFCB()->HighwaterMarking?Qt::Checked:Qt::Unchecked);
  GrabFreeSpaceCHk->setCheckState(wMasterFile.getFCB()->GrabFreeSpace?Qt::Checked:Qt::Unchecked);
  JournalingCHk->setCheckState(wMasterFile.getJournalingStatus()?Qt::Checked:Qt::Unchecked);


  KeyTBv->ItemModel->removeRows(0,KeyTBv->ItemModel->rowCount());

  KeyData wKeyData;
  KeyValues->clear();
  QList<QStandardItem*> wKeyRow;

  for (long wi=0; wi < wMasterFile.IndexTable.count(); wi++) {
    uriString wIndexName = wMasterFile.getURIIndex(wi);
    wKeyData.IndexRootName = wIndexName.getRootname();

    wKeyData.KeySize = wMasterFile.IndexTable[wi]->KeyUniversalSize;
    wKeyData.Allocated = wMasterFile.IndexTable[wi]->getFCB()->AllocatedBlocks;
    wKeyData.AllocatedSize = wKeyData.KeySize * wKeyData.Allocated;
    wKeyData.ExtentQuota = wMasterFile.IndexTable[wi]->getFCB()->BlockExtentQuota;
    wKeyData.ExtentSize = wKeyData.ExtentQuota * wKeyData.KeySize;
    wKeyData.Duplicates   = wMasterFile.IndexTable[wi]->Duplicates;

    KeyValues->push(wKeyData);

    wKeyRow << new QStandardItem(wMasterFile.IndexTable[wi]->IndexName.toCChar());
    wKeyRow << new QStandardItem(wKeyData.IndexRootName.toCChar());
    wStr.sprintf("%ld",wKeyData.KeySize);
    wKeyRow.push_back(new QStandardItem(wStr.toCChar()));
    wKeyRow.last()->setTextAlignment(Qt::AlignRight);

    QStandardItem* wDup=new QStandardItem("Duplicates");
    wDup->setTextAlignment(Qt::AlignLeft);

    if (wKeyData.Duplicates)
      wDup->setCheckState(Qt::Checked);
    else
      wDup->setCheckState(Qt::Unchecked);

    wDup->setEditable(false);
    wKeyRow.push_back(wDup);

    wKeyRow << createItemAligned(wKeyData.Allocated,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.AllocatedSize,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.ExtentQuota,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.ExtentSize,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    KeyTBv->ItemModel->appendRow(wKeyRow);
  }


  wMasterFile.zclose();

  return;

  AllocatedBlocks = cst_ZRF_default_allocation;
  wStr.sprintf("%ld",AllocatedBlocks);
  AllocatedLEd->setText(wStr.toCChar());
  InitialSizeLEd->setText(wStr.toCChar());

  ExtentQuota = cst_ZRF_default_extentquota;
  wStr.sprintf("%ld",ExtentQuota);
  ExtentQuotaLEd->setText(wStr.toCChar());

  Compute();

//  QList<QStandardItem*> wKeyRow;
  for (long wi=0; wi < DictionaryFile->KeyDic.count() ; wi++) {

    wKeyRow.clear();
    ZKeyDictionary* wKeyDic=DictionaryFile->KeyDic.Tab[wi];

    KeyData wKeyData;
    wKeyData.KeySize = AllocatedBlocks*wKeyDic->computeKeyUniversalSize();
    wKeyData.Allocated = AllocatedBlocks;
    wKeyData.AllocatedSize = wKeyData.KeySize * wKeyData.Allocated;
    wKeyData.ExtentQuota = ExtentQuota;
    wKeyData.ExtentSize = wKeyData.KeySize * wKeyData.ExtentQuota;
    wKeyData.IndexRootName = generateIndexRootName(RootName,wi,wKeyDic->DicKeyName);
    wKeyData.Duplicates   = DictionaryFile->KeyDic.Tab[wi]->Duplicates;
    KeyValues->push(wKeyData);

    wKeyRow.push_back(new QStandardItem(wKeyDic->DicKeyName.toCChar()));

    wKeyRow << new QStandardItem(wKeyData.IndexRootName.toCChar());

    wStr.sprintf("%ld",wKeyData.KeySize);
    wKeyRow.push_back(new QStandardItem(wStr.toCChar()));
    wKeyRow.last()->setTextAlignment(Qt::AlignRight);

    QStandardItem* wDup=new QStandardItem("Duplicates");
    wDup->setTextAlignment(Qt::AlignLeft);

    if (wKeyDic->Duplicates)
      wDup->setCheckState(Qt::Checked);
    else
      wDup->setCheckState(Qt::Unchecked);

    wDup->setEditable(false);
    wKeyRow.push_back(wDup);

    wKeyRow << createItemAligned(wKeyData.Allocated,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.AllocatedSize,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.ExtentQuota,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    wKeyRow << createItemAligned(wKeyData.ExtentSize,Qt::AlignRight,"%ld");
    wKeyRow.last()->setEditable(false);

    KeyTBv->ItemModel->appendRow(wKeyRow);

  }// for

  for (int wi=0; wi < KeyTBv->ItemModel->columnCount(); wi++)
    KeyTBv->resizeColumnToContents(wi);
  for (int wi=0; wi < KeyTBv->ItemModel->rowCount(); wi++)
    KeyTBv->resizeRowToContents(wi);

  QVariant wV;

  for (long wi=0;wi < DictionaryFile->count();wi++) {
    if (DictionaryFile->Tab[wi].UniversalSize==0) {
      QList<QStandardItem*> wGuessRow;
      wGuessRow << createItem(DictionaryFile->Tab[wi].getName());
      wGuessRow[0]->setEditable(false);
      wGuessRow << createItem(decode_ZType( DictionaryFile->Tab[wi].ZType));
      wGuessRow.last()->setEditable(false);
      wGuessRow << createItem(DictionaryFile->Tab[wi].HeaderSize);
      wGuessRow.last()->setEditable(false);
      wGuessRow << createItem(DictionaryFile->Tab[wi].UniversalSize);
      wGuessRow.last()->setEditable(true);

      GuessTBv->ItemModel->appendRow(wGuessRow);

      wStr.sprintf( "%s-Value for field <%s> has not been guessed and remains invalid.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wGuessRow[0]->text().toUtf8().data());
      plainTextEdit->appendPlainText(wStr.toCChar());
    }
  }// for


  for (int wi=0;wi < GuessTBv->ItemModel->columnCount();wi++)
    GuessTBv->resizeColumnToContents(wi);
  for (int wi=0;wi < GuessTBv->ItemModel->rowCount();wi++)
    GuessTBv->resizeRowToContents(wi);


  QObject::connect(GuessTBv->ItemModel,&QStandardItemModel::itemChanged,this,&FileGenerateDLg::GuessItemChanged);

} // dataSetupFromDictionary

void
FileGenerateDLg::SearchDir(){

  QFileDialog wFDLg((QWidget*)this,QObject::tr("Target directory","FileGenerateDLg"),getParserWorkDirectory());

  wFDLg.setLabelText(QFileDialog::Accept,  QObject::tr("Select","FileGenerateDLg"));
  wFDLg.setLabelText(QFileDialog::Reject,  QObject::tr("Cancel","FileGenerateDLg"));

  wFDLg.setOptions(QFileDialog::ShowDirsOnly);
  wFDLg.setDirectory(TargetDirectory.toCChar());
  int wRet=wFDLg.exec();

  if ((wRet==QDialog::Rejected) ||(wFDLg.selectedFiles().isEmpty()))
    return;

  TargetDirectory = wFDLg.selectedFiles()[0].toUtf8().data();
  DirectoryLBl->setText(TargetDirectory.toCChar());
  return;
}//FileGenerateDLg::SearchDir

void
FileGenerateDLg::IndexSearchDir(){

  if (IndexDirectory.isEmpty())
    IndexDirectory = TargetDirectory;

  QFileDialog wFDLg((QWidget*)this,QObject::tr("Target Indexes directory","FileGenerateDLg"),getParserWorkDirectory());

  wFDLg.setLabelText(QFileDialog::Accept,  QObject::tr("Select","FileGenerateDLg"));
  wFDLg.setLabelText(QFileDialog::Reject,  QObject::tr("Cancel","FileGenerateDLg"));

  wFDLg.setOptions(QFileDialog::ShowDirsOnly);
  wFDLg.setDirectory(IndexDirectory.toCChar());
  int wRet=wFDLg.exec();

  if ((wRet==QDialog::Rejected) ||(wFDLg.selectedFiles().isEmpty()))
    return;

  IndexDirectory = wFDLg.selectedFiles()[0].toUtf8().data();
  IndexDirectoryLBl->setText(IndexDirectory.toCChar());
  return;
}//FileGenerateDLg::SearchDir
void
FileGenerateDLg::Quit(){
  this->hide();
  this->deleteLater();
  return;
}//FileGenerateDLg::Discard

void
FileGenerateDLg::BaseNameEdit(){

  RootName = RootNameLEd->text().toUtf8().data() ;

  RootName.eliminateChar(' ');
  RootNameLEd->setText(RootName.toCChar());

    return;
}//FileGenerateDLg::BaseNameEdit

void
FileGenerateDLg::AllocatedEdit(){
  utf8VaryingString wStr;
  wStr = AllocatedLEd->text().toUtf8().data();
  AllocatedBlocks = getValueFromString<size_t>(wStr) ;

  AllocatedSize = AllocatedBlocks * MeanRecordSize;
  wStr.sprintf("%ld",AllocatedSize);
  AllocatedSizeLEd->setText(wStr.toCChar());

  return;
}//FileGenerateDLg::AllocatedEdit


void
FileGenerateDLg::MeanSizeEdit(){
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
FileGenerateDLg::ExtentQuotaEdit(){
  utf8VaryingString wStr;
  wStr = ExtentQuotaLEd->text().toUtf8().data();
  ExtentQuota = getValueFromString<size_t>(wStr) ;

  ExtentQuotaSize = ExtentQuota * MeanRecordSize;
  wStr.sprintf("%ld",ExtentQuotaSize);
  ExtentQuotaSizeLEd->setText(wStr.toCChar());

  return;
}//FileGenerateDLg::MeanSizeEdit


void
FileGenerateDLg::GuessItemChanged(QStandardItem* pItem){
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
  plainTextEdit->appendPlainText(wStr.toCChar());

  Compute();
  return;
}//FileGenerateDLg::GuessItemChanged

void
FileGenerateDLg::Compute() {
  ZArray<utf8VaryingString> wWarnedFields;
  size_t  MeanRecordSize=0;
  utf8VaryingString wStr;
  for (long wi=0 ; wi < DictionaryFile->count() ; wi++) {
    MeanRecordSize += DictionaryFile->Tab[wi].HeaderSize;
    MeanRecordSize += DictionaryFile->Tab[wi].UniversalSize;
    if ((DictionaryFile->Tab[wi].ZType & ZType_VaryingMask) || (DictionaryFile->Tab[wi].UniversalSize==0)) {
    }
  } // for

  for (int wi=0;wi < GuessTBv->ItemModel->rowCount();wi++) {
    QVariant wV = GuessTBv->ItemModel->item(wi,3)->data(Qt::UserRole);
    if (wV.isValid()) {
      size_t wValue = wV.value<size_t>();
      MeanRecordSize += wValue;
    }
    else {
      QStandardItem* wItem = GuessTBv->ItemModel->item(wi,0);
      wStr.sprintf( "%s-Value for field <%s> has not been guessed and remains invalid.",
          ZDateFull::currentDateTime().toFormatted().toString(),
          wItem->text().toUtf8().data());
      plainTextEdit->appendPlainText(wStr.toCChar());
    }

  }// for


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

void
FileGenerateDLg::Guess() {
  //  size_t  MeanRecordSize=0;

}


void
FileGenerateDLg::MenuAction(QAction* pAction){

  if (pAction==GenFileQAc){
    GenFile();
    return;
  }
  if (pAction==GenXmlQAc){
    GenXml();
    return;
  }
  if (pAction==SearchDirQAc){
    SearchDir();
    return;
  }
  if (pAction==IndexSearchDirQAc){
    IndexSearchDir();
    return;
  }
  if (pAction==QuitQAc){
    Quit();
    return;
  }
  if (pAction==LoadFromFileQAc){
    LoadFromFile();
    return;
  }
  return;
}//FileGenerateDLg::MenuAction

void FileGenerateDLg::LoadFromFile() {
  utf8VaryingString wStr;

  QFileDialog wFDLg((QWidget*)this,QObject::tr("Search master file","FileGenerateDLg"),getParserWorkDirectory());

  wFDLg.setLabelText(QFileDialog::Accept,  QObject::tr("Select","FileGenerateDLg"));
  wFDLg.setLabelText(QFileDialog::Reject,  QObject::tr("Cancel","FileGenerateDLg"));

  wFDLg.setOptions(QFileDialog::ReadOnly);
  wFDLg.setFileMode(QFileDialog::ExistingFiles);
  wFDLg.setDirectory(getParserWorkDirectory());
  int wRet=wFDLg.exec();

  if ((wRet==QDialog::Rejected) ||(wFDLg.selectedFiles().isEmpty()))
    return;

  uriString wURIMaster = wFDLg.selectedFiles()[0].toUtf8().data();

  dataSetupFromMasterFile(wURIMaster);
  return;
}
void FileGenerateDLg::GenXml() {


}
void FileGenerateDLg::GenFile() {
  utf8VaryingString wStr;
  ZaiErrors wErrorLog;
  ZMasterFile* wMasterFile=new ZMasterFile;
  uriString wURIFile = TargetDirectory ;
  wURIFile.addConditionalDirectoryDelimiter();
  wURIFile += RootName;
  wURIFile += __MASTER_FILEEXTENSION__;

  bool wBackup=false;

  if (wURIFile.exists()) {
    uriStat wStat;
    wURIFile.getStatR(wStat);

    int wRet = ZExceptionDLg::adhocMessage3B("Master file creation",Severity_Warning,"Backup","Cancel","Replace",nullptr,
        " Found existing file <%s> \n"
        " It has been created <%s> and last modified <%s>\n"
        " <Backup> creates the file but backs up existing.\n"
        " <Replace> replaces file without backup.\n"
        " <Cancel> give up action.",
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
                                                HighWaterMarkingCHk->checkState(),
                                                GrabFreeSpaceCHk->checkState(),
                                                JournalingCHk->checkState(),
                                                wBackup,
                                                true);

  if (wSt!=ZS_SUCCESS) {

    return;
  }
  wStr.sprintf("%s - created Master file <%s>.",
      ZDateFull::currentDateTime().toFormatted().toString(),
          wURIFile.toString());
  plainTextEdit->appendPlainText(wStr.toCChar());

  /* create all indexes */
  uriString wIndexUri;
  long wIndexRank;

  wMasterFile->setIndexFilesDirectoryPath(IndexDirectory);

  for (long wi=0;wi < KeyValues->count(); wi ++) {
//    generateIndexURI(wIndexUri,wURIFile,TargetDirectory,wi,KeyValues[wi].)
    wIndexUri = IndexDirectory ;
    wIndexUri.addConditionalDirectoryDelimiter();
    wIndexUri += KeyValues->Tab[wi].IndexRootName;

   wSt = wMasterFile->_createRawIndexDet (wIndexRank,                               /* returned key rank */
                                          DictionaryFile->KeyDic[wi]->DicKeyName,   /* Index name */
                                          KeyValues->Tab[wi].KeySize, /* Key universal total size */
                                          KeyValues->Tab[wi].Duplicates?ZST_DUPLICATES:ZST_NODUPLICATES,
                                          KeyValues->Tab[wi].Allocated,      /* ---FCB parameters (for index ZRandomFile)---- */
                                          KeyValues->Tab[wi].ExtentQuota,
                                          KeyValues->Tab[wi].AllocatedSize,
                                          HighWaterMarkingCHk->checkState(),
                                          GrabFreeSpaceCHk->checkState(),
                                          wBackup,
                                          &wErrorLog);
   wStr.sprintf("%s - created index file <%s>.",
       ZDateFull::currentDateTime().toFormatted().toString(),
       wMasterFile->getURIIndex(wIndexRank).toString());
   plainTextEdit->appendPlainText(wStr.toCChar());
  } // for

  wStr.sprintf("%s - end creation.",
      ZDateFull::currentDateTime().toFormatted().toString());
  plainTextEdit->appendPlainText(wStr.toCChar());
  wMasterFile->zclose();
} // FileGenerateDLg::GenFile

void FileGenerateDLg::resizeEvent(QResizeEvent* pEvent)
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
