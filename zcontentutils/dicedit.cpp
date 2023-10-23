#include "dicedit.h"
#include "ui_dicedit.h"

#include <zcontentcommon/zgeneralparameters.h>

#include <QStandardItemModel>

#include <unistd.h>

#include <zrandomfile/zheadercontrolblock.h>
#include <zrandomfile/zfilecontrolblock.h>
#include <zindexedfile/zmastercontrolblock.h>

#include <zindexedfile/zmfdictionary.h>

#include <zexceptiondlg.h>

#include <zqt/zqtwidget/zpinboard.h>

#include <zqt/zqtwidget/zqtreeview.h>
#include <zqt/zqtwidget/zqtableview.h>

#include <zqt/zqtwidget/zqstandarditem.h>

#include <zqt/zqtwidget/zdatareference.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <zentity.h>

#include <zfielddlg.h>
#include <zkeydlg.h>

#include <zcontent/zindexedfile/zfielddescription.h>

#include <zcontent/zindexedfile/zdictionaryfile.h>

#include <qclipboard.h>
#include <qmimedata.h>
#include <qfiledialog.h>

//#include <qtreeview.h>

#include <qaction.h>
#include <qactiongroup.h>
#include <zindexedfile/zfielddescription.h>
#include <zindexedfile/zkeydictionary.h>
#include <zindexedfile/zindexfield.h>

#include <zindexedfile/zdataconversion.h>// for _getURFHeaderSize

#include <texteditmwn.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>


#include <zcppparser/zcppparserutils/rawfields.h>

#include <zcppgenerate.h>


#include <zindexedfile/zdictionaryfile.h>

#include <zstdlistdlg.h>
#include <zcontent/zcontentutils/zdisplayedfield.h>

//#include <zcontent/zcontentutils/zdicdlg.h>  //renamed to be saved to zdicdlg_zrf.h (and .cpp)

#include <filegeneratedlg.h>

#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory
#include <QVariant>
#include <QMetaType>

#ifdef __USE_WINDOWS__
#include <io.h>
#else
#include <fcntl.h>  // for open()
#endif


#include <zxml/zxmlprimitives.h>

#include <texteditmwn.h>

#include <zcontent/zindexedfile/zmasterfile.h>


using namespace zbs;

const int cst_MessageDuration = 20000;  /* milliseconds */
int cst_KeyNameColumn = 0 ;
int cst_KeyOffsetColumn = 2 ;
int cst_KeyUSizeColumn = 3 ;
int cst_DuplicateColumn = 4 ;
int cst_TooltipColumn = 5 ;

FileGenerateMWn*  FileGenerate=nullptr;
ZSearchQueryMWd*  QueryMWd=nullptr;


extern ZPinboard   Pinboard;
extern bool        UseMimeData;
extern bool        DragEngaged;





DicEditMWn::~DicEditMWn()
{
  if (FieldDLg) {
    delete FieldDLg;
    FieldDLg=nullptr;
  }
  if (GenerateEngine)
    delete GenerateEngine;

  clearAllRows();

  GenerateEngine=nullptr;
//  if (MasterDic)
//    delete MasterDic;

  if (DictionaryFile) {
/*    if (DictionaryFile->isOpen())
      DictionaryFile->zclose();
*/
    delete DictionaryFile;
  }

  delete ui;
}
/*
void
DicEditMWn::displayErrorCallBack(const utf8VaryingString& pMessage) {
    statusBar()->showMessage(QObject::tr(pMessage.toCChar()),cst_MessageDuration);
}
*/
DicEditMWn::DicEditMWn(QWidget *parent) : QMainWindow(parent),ui(new Ui::DicEdit) , Parent(parent)
{
  init();
}

DicEditMWn::DicEditMWn(std::function<void()> pQuitCallback,QWidget *parent ) : QMainWindow(parent),ui(new Ui::DicEdit) , Parent(parent) {
  QuitDicEditCallback = pQuitCallback;
  init();
}
void
DicEditMWn::init() {

  ui->setupUi(this);

  setWindowTitle("Master Dictionary");

  ui->SourceLBl->setText("No input");
  ui->SourceURILBl->setText("");

  ErrorLog.setAutoPrintOn(ZAIES_Text);
//  ErrorLog.setDisplayCallback(std::bind(&DicEditMWn::displayErrorCallBack, this,_1));
  ErrorLog.setDisplayColorCB(std::bind(&DicEditMWn::displayErrorColorCB, this,placeholders::_1,placeholders::_2));

  ui->ActiveCBx->addItem("Not active");
  ui->ActiveCBx->addItem("Active");
  ui->ActiveCBx->setCurrentIndex(1);

  ui->DicNameLBl->setText ("No dictionary name");
  ui->VersionLBl->setText ("0.0-0");
  ui->KeysNbLBl ->setText ("0");
  ui->FieldsNbLBl->setText("0");

  /*  ui->displayTBv->setStyleSheet(QString::fromUtf8("QTableView::item{border-left : 1px solid black;\n"
                                                "border-right  : 1px solid black;\n"
                                                "font: 75 12pt \"Courier\";\n"
                                                " }"));
*/
  fieldTBv = setupFieldTBv( ui->displayDicTBv,true,10);/* 10 columns */

  FieldSM=fieldTBv->selectionModel();

  int wCol=0;
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Name")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType code")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType symbols")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Capacity")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("HeaderSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("UniversalSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("NaturalSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("KeyEligible")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Tooltip")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Hash code")));


  fieldTBv->setShowGrid(true);

  fieldTBv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
  fieldTBv->setWordWrap(false);

  fieldTBv->setSupportedDropActions(Qt::CopyAction);

  fieldTBv->addFilterMask(ZEF_DoubleClick);
  fieldTBv->setAlternatingRowColors(true);

  fieldTBv->show();

  /* for key fields */
  keyTRv = setupKeyTRv(ui->displayKeyTRv,true,5); /* 5 columns */
  KeySM=keyTRv->selectionModel();

  wCol=0;
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Name")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType code")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("KeyOffset")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("UniversalSize")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Hash code")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Tooltip")));

  //  ui->displayKeyTRv->setShowGrid(true);

  keyTRv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
  keyTRv->setWordWrap(false);

  keyTRv->setSupportedDropActions(Qt::CopyAction);

  keyTRv->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  keyTRv->setAlternatingRowColors(true);

  keyTRv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

  keyTRv->show();

  /* menus and options */

  setupReadWriteMenu();

  generalMEn=new QMenu(QObject::tr("general","DicEdit"),ui->menubar);
  ui->menubar->addMenu(generalMEn);

  generalGroup = new QActionGroup(generalMEn);
  generalGroup->setExclusive(false);

  parserQAc= new QAction(QObject::tr("parse a C++ header","DicEdit"),generalMEn);
  parserQAc->setObjectName("parserQAc");
  generalGroup->addAction(parserQAc);
  generalMEn->addAction(parserQAc);

  generalMEn->addMenu(FloadMEn);

  generateMainMEn=new QMenu(QObject::tr("code generation","DicEdit"),this);

  generalMEn->addMenu(generateMainMEn);

  generateQAc= new QAction(QObject::tr("generate","DicEdit"),generateMainMEn);
  generateQAc->setObjectName("generateQAc");
  generalGroup->addAction(generateQAc);
  generateMainMEn->addAction(generateQAc);

  genShowHideMEn = new QMenu(QObject::tr("Show / hide","DicEdit"),generateMainMEn);

  genFileMEn=new QMenu(QObject::tr("file generation","DicEdit"),this);
  generalMEn->addMenu(genFileMEn);

  fileXmlGenQAc=new QAction(QObject::tr("xml file definition","DicEdit"),genFileMEn);
  genFileMEn->addAction(fileXmlGenQAc);
  generalGroup->addAction(fileXmlGenQAc);

  fileCreateQAc=new QAction(QObject::tr("master file from dictionary","DicEdit"),genFileMEn);
  genFileMEn->addAction(fileCreateQAc);
  generalGroup->addAction(fileCreateQAc);


  genShowLogQAc= new QAction(QObject::tr("show generation log","DicEdit"),genShowHideMEn);
  genShowLogQAc->setObjectName("genShowLogQAc");
  genShowLogQAc->setCheckable(true);
  genShowLogQAc->setChecked(false);
  generalGroup->addAction(genShowLogQAc);
//  genShowHideMEn->addAction(genShowLogQAc);
//  generateMainMEn->addAction(genShowLogQAc);

  genShowCppQAc= new QAction(QObject::tr("show cpp code","DicEdit"),genShowHideMEn);
  genShowCppQAc->setObjectName("genShowCppQAc");
  genShowCppQAc->setCheckable(true);
  genShowCppQAc->setChecked(false);
  generalGroup->addAction(genShowCppQAc);
//  genShowHideMEn->addAction(genShowCppQAc);
  generateMainMEn->addAction(genShowCppQAc);

  genShowHeaderQAc= new QAction(QObject::tr("show header code","DicEdit"),genShowHideMEn);
  genShowHeaderQAc->setObjectName("genShowHeaderQAc");
  genShowHeaderQAc->setCheckable(true);
  genShowHeaderQAc->setChecked(false);
  generalGroup->addAction(genShowHeaderQAc);
//  genShowHideMEn->addAction(genShowHeaderQAc);
  generateMainMEn->addAction(genShowHeaderQAc);

  generalMEn->addAction(SaveQAc);
  generalMEn->addAction(UpdateEmbeddedQAc);

  generalGroup->addAction(SaveQAc);
  generalGroup->addAction(UpdateEmbeddedQAc);

  generalGroup->addAction(FwritetoDicQAc);
  generalGroup->addAction(FwritetoclipQAc);
  generalGroup->addAction(FwriteXmltofileQAc);
  generalGroup->addAction(FviewXmlQAc);

  generalGroup->addAction(FloadfromDicQAc);
  generalGroup->addAction(FloadfromclipQAc);
  generalGroup->addAction(FloadfromXmlFileQAc);

  generalMEn->addMenu(FwritetoMEn);

  quitQAc= new QAction(QObject::tr("quit","DicEdit"),generalMEn);
  quitQAc->setObjectName("quitQAc");
  generalGroup->addAction(quitQAc);
  generalMEn->addAction(quitQAc);

  QObject::connect(generalGroup, SIGNAL(triggered(QAction*)), this, SLOT(generalActionEvent(QAction*)));

  /* here after ZQLabel fields with doubleClicked signal */
  QObject::connect(ui->DicNameLBl, SIGNAL(doubleClicked()), this, SLOT(dicDescriptionClicked()));
  QObject::connect(ui->VersionLBl, SIGNAL(doubleClicked()), this, SLOT(dicDescriptionClicked()));

 // QObject::connect(keyTRv->ItemModel,&QStandardItemModel::itemChanged,this,&DicEditMWn::KeyItemChanged);

  DicLog=new textEditMWn((QWidget*)this,TEOP_CloseBtnHide | TEOP_NoFileLab,nullptr);
  DicLog->registerCloseCallback(std::bind(&DicEditMWn::closeDiclogCB, this,std::placeholders::_1));
  DicLog->setWindowTitle("Communication log");
  DicLog->show();

  ErrorLog.setDisplayCallback(std::bind(&DicEditMWn::displayErrorCallBack, this,_1));

  DictionaryFile = new ZDictionaryFile;
} // DicEditMWn::init()

void
DicEditMWn::closeDiclogCB(const QEvent* pEvent)
{
    DicLog=nullptr;
}
void
DicEditMWn::displayErrorCallBack(const utf8VaryingString& pMessage) {
    if (DicLog!=nullptr)
        DicLog->appendText(pMessage);
    else
        fprintf(stderr,pMessage.toCChar());

    statusBar()->showMessage(QObject::tr(pMessage.toCChar()),cst_MessageDuration);
}
void
DicEditMWn::displayErrorColorCB(uint8_t pSeverity, const utf8VaryingString& pMessage) {
    if (DicLog==nullptr) {
        fprintf(stderr,pMessage.toCChar());
        return;
    }
    switch (pSeverity) {
    case ZAIES_Text:
        DicLog->appendText(pMessage);
        return;
    case ZAIES_Info:
        DicLog->appendTextColor(Qt::blue, pMessage);
        return;
    case ZAIES_Warning:
        DicLog->appendTextColor(Qt::darkGreen, pMessage);
        return;
    case ZAIES_Error:
    case ZAIES_Fatal:
        DicLog->appendTextColor(Qt::red, pMessage);
        return;
    default:
        DicLog->appendTextColor(Qt::yellow, pMessage);
        return;
    }
    statusBar()->showMessage(QObject::tr(pMessage.toCChar()),cst_MessageDuration);
}
#ifdef __COMMENT__
void
DicEditMWn::KeyItemChanged(QStandardItem *pItem)
{
  QStandardItem *wMasterItem=nullptr;
  QVariant wV;
  ZDataReference wDRef;
  ZKeyHeaderRow* wKHR=nullptr;
  if (pItem==nullptr)
    return;
  int wColumn=pItem->column();

  /* get ZDataReference object */
  if (wColumn==0)
    wMasterItem = pItem;
  else
    wMasterItem = keyTRv->ItemModel->item(pItem->row(),0);
  wV = wMasterItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  wKHR = wDRef.getPtr<ZKeyHeaderRow*>();
  while (true)
  {
    if (wColumn==cst_KeyNameColumn) {
        wKHR->DicKeyName = pItem->text().toUtf8().data();
        wKHR->ChangeStatus |= KHCHG_KeyName;
        break;
    }
    if (wColumn==cst_DuplicateColumn) {
        break;
/*        ZDataReference wDRef1;
        QVariant wV1;
        wV1 = pItem->data(ZQtDataReference);
        wDRef1 = wV1.value<ZDataReference>();
        wKHR->Duplicates = (ZSort_Type)wDRef1.DataRank;
        wKHR->ChangeStatus |= KHCHG_Duplicates;
        break;
*/
    }
    if (wColumn==cst_KeyUSizeColumn) {
        wKHR->KeyGuessedSize = (uint32_t)pItem->text().toInt();
        wKHR->ChangeStatus |= KHCHG_GuessedSize;
        break;
    }
    if (wColumn==cst_TooltipColumn) {
        wKHR->ToolTip = pItem->text().toUtf8().data();
        wKHR->ChangeStatus |= KHCHG_Tooltype;
        break;
    }
    break;
  }// while true
  return;
}//KeyItemChanged
#endif // #ifdef __COMMENT__
void
DicEditMWn::dicDescriptionClicked() {
  utf8VaryingString wDicName = ui->DicNameLBl->text().toUtf8().data();
  unsigned long wVersion = getVersionNum(ui->VersionLBl->text().toUtf8().data());
  bool wActive = ui->ActiveCBx->currentIndex()==1 ;
  if (!getDicName(wVersion,wActive,wDicName))
    return ;

  ui->DicNameLBl->setText(wDicName.toCChar());
  ui->VersionLBl->setText(getVersionStr(wVersion).toCChar());
  ui->ActiveCBx->setCurrentIndex(wActive?1:0);
  return;
}

void
DicEditMWn::generalActionEvent(QAction* pAction) {

  if (pAction == parserQAc) {

    if (rawFields==nullptr) {
//      rawFields = new RawFields( "/home/gerard/Development/zbasetools/zcontent/ztest_zindexedfile/testdata/zcppparserparameters.xml", this);
      rawFields = new RawFields( this);
    }
  rawFields->showAll();
  rawFields->setFocus();
  } // parserQAc

  if (pAction == quitQAc) {
    Quit();
  } // parserQAc

  if (pAction == generateQAc) {

    if (fieldTBv->ItemModel->rowCount()==0) {
      ZExceptionDLg::adhocMessage("Generate",Severity_Error,&ErrorLog, nullptr,"Nothing to generate");
      return;
    }

    ZMFDictionary* wMasterDic = screenToDic(nullptr);  /* create Master dictionary content from views */
    DictionaryFile->setDictionary(*wMasterDic);
    if (GenerateEngine==nullptr)
      GenerateEngine=new ZCppGenerate(DictionaryFile);

    ErrorLog.setAutoPrintOn(ZAIES_Text);
    ErrorLog.setDisplayCallback(nullptr);
    ZStatus wSt=GenerateEngine->loadGenerateParameters(uriString(),&ErrorLog);
    if (wSt!=ZS_SUCCESS) {
      ZExceptionDLg::displayLast("Generation parameters");
    }
    utf8VaryingString wGenPath = GenerateEngine->getGenPath();

    utf8VaryingString wOutFileBaseName, wClass , wBrief = DictionaryFile->DicName.toString();
    if (!getGenerationNames(wOutFileBaseName,wClass,wBrief,wGenPath))
        return;
    /*
    if (DictionaryFile->getURIContent().isEmpty()) {
      GenerateEngine->setXmlDictionaryFile( XmlDictionaryFile);
    }
    */
    if (DictionaryFile==nullptr) {
      GenerateEngine->setXmlDictionaryFile( XmlDictionaryFile);
    }
    else
        if (DictionaryFile->URIDictionary.isEmpty()) {
      GenerateEngine->setXmlDictionaryFile( XmlDictionaryFile);
    }

    wSt=GenerateEngine->generateInterface(wOutFileBaseName,wClass,wBrief,wGenPath);

    if (GenCppFileWin==nullptr) {
      GenCppFileWin = new textEditMWn(this,
          TEOP_ShowLineNumbers | TEOP_CloseBtnHide , /* show line numbers and close button hides dialog */
          std::bind(&DicEditMWn::closeGenShowCppCB, this,std::placeholders::_1));
      if (genShowCppQAc->isChecked())
        GenCppFileWin->show();
    }
    if (GenHeaderFileWin==nullptr) {
      GenHeaderFileWin = new textEditMWn(this,
          TEOP_ShowLineNumbers | TEOP_CloseBtnHide , /* show line numbers and close button hides dialog */
          std::bind(&DicEditMWn::closeGenShowHeaderCB, this,std::placeholders::_1));
      if (genShowHeaderQAc->isChecked())
        GenHeaderFileWin->show();
    }
    if (GenerateEngine && GenerateEngine->getGenCppFile().exists())
      GenCppFileWin->setTextFromFile(GenerateEngine->getGenCppFile());
    if (GenerateEngine && GenerateEngine->getGenCppFile().exists())
      GenHeaderFileWin->setTextFromFile(GenerateEngine->getGenHeaderFile());
//    delete wGen;
    return;
  } // generateQAc

  if (pAction == genShowCppQAc) {
    if (GenCppFileWin==nullptr) {
      GenCppFileWin = new textEditMWn(this,
          TEOP_ShowLineNumbers | TEOP_CloseBtnHide , /* show line numbers and close button hides dialog */
          std::bind(&DicEditMWn::closeGenShowCppCB, this,std::placeholders::_1));

      if (GenerateEngine && GenerateEngine->getGenCppFile().exists())
        GenCppFileWin->setTextFromFile(GenerateEngine->getGenCppFile());
    }
    if (genShowCppQAc->isChecked())
      GenCppFileWin->show();
    else
      GenCppFileWin->hide();
    return;
  } // genShowCppQAc

  if (pAction == genShowHeaderQAc) {
    if (GenHeaderFileWin==nullptr) {
      GenHeaderFileWin = new textEditMWn(this,
          TEOP_ShowLineNumbers | TEOP_CloseBtnHide , /* show line numbers and close button hides dialog */
          std::bind(&DicEditMWn::closeGenShowHeaderCB, this,std::placeholders::_1));

      if (GenerateEngine && GenerateEngine->getGenHeaderFile().exists())
        GenHeaderFileWin->setTextFromFile(GenerateEngine->getGenHeaderFile());
    }
    if (genShowHeaderQAc->isChecked())
      GenHeaderFileWin->show();
    else
      GenHeaderFileWin->hide();
    return;
  } // genShowCppQAc

  if (pAction == fileCreateQAc) {

    if (fieldTBv->ItemModel->rowCount()==0) {
      ZExceptionDLg::adhocMessage("Generate",Severity_Error,&ErrorLog,nullptr,"Nothing to generate");
      return;
    }
    FileGenerate=new FileGenerateMWn(DictionaryFile, this);
    FileGenerate->show();
    return;
  }
  if (pAction == fileXmlGenQAc) {

    if (fieldTBv->ItemModel->rowCount()==0) {
      ZExceptionDLg::adhocMessage("Generate",Severity_Error,&ErrorLog,nullptr,"Nothing to generate");
      return;
    }
  }
  if (pAction == quitQAc) {
    Quit();
  } // parserQAc

  readWriteActionEvent(pAction);
}

void DicEditMWn::closeGenShowCppCB(QEvent *pEvent)
{
  if (pEvent->type()==QEvent::Destroy) {
    genShowCppQAc->setEnabled(false);
    genShowCppQAc->setChecked(false);
    GenCppFileWin->QWidget::close();
    GenCppFileWin->deleteLater();
    GenCppFileWin=nullptr;
    return;
  }
  if (pEvent->type()==QEvent::Hide) {
    genShowCppQAc->setChecked(false);
    GenCppFileWin->QWidget::hide();
    return;
  }
  if (pEvent->type()==QEvent::Close) {
    genShowCppQAc->setEnabled(false);
    genShowCppQAc->setChecked(false);
    GenCppFileWin->QWidget::close();
    GenCppFileWin->deleteLater();
    GenCppFileWin=nullptr;
    return;
  }
}//DicEdit::closeGenShowCppCB

void DicEditMWn::closeGenShowHeaderCB(QEvent *pEvent)
{
  if (pEvent->type()==QEvent::Destroy) {
    genShowHeaderQAc->setEnabled(false);
    genShowHeaderQAc->setChecked(false);
    GenHeaderFileWin->QWidget::close();
    GenHeaderFileWin->deleteLater();
    GenHeaderFileWin=nullptr;
    return;
  }
  if (pEvent->type()==QEvent::Hide) {
    genShowHeaderQAc->setChecked(false);
    GenHeaderFileWin->QWidget::hide();
    return;
  }
  if (pEvent->type()==QEvent::Close) {
    genShowHeaderQAc->setEnabled(false);
    genShowHeaderQAc->setChecked(false);
    GenHeaderFileWin->QWidget::close();
    GenHeaderFileWin->deleteLater();
    GenHeaderFileWin=nullptr;
    return;
  }
}//DicEdit::closeGenShowCppCB

void
DicEditMWn::Quit() {

  if (DictionaryChanged) {
    int wRet=ZExceptionDLg::adhocMessage2B("",Severity_Warning,"Continue","Quit",nullptr, nullptr,
          "Dictionary has been changed.\n"
          "Are you sure you want to quit and loose changes ?");
    if (wRet==QDialog::Rejected)
      return;
  }
  if ( !Parent )
    QApplication::quit(); /* if launched in standalone : quit application */

  if (QuitDicEditCallback!=nullptr)
    QuitDicEditCallback();

  this->deleteLater();
} // Quit

void
DicEditMWn::clear()
{
//  fieldTBv->ItemModel->clear();
//  keyTRv->I
  clearAllRows();

  ui->VersionLBl->clear();
  ui->FieldsNbLBl->clear();
  ui->KeysNbLBl->clear();
  ui->DicNameLBl->clear();
}

void DicEditMWn::KeyTRvFlexMenu(QContextMenuEvent *event) {
  if (keyFlexMEn==nullptr)
            setupKeyFlexMenu();
  keyFlexMEn->exec(event->globalPos());
//  menuFlex->deleteLater();
}

void DicEditMWn::FieldTBvFlexMenu(QContextMenuEvent *event)
{
  if (fieldFlexMEn==nullptr)
    setupFieldFlexMenu();
  fieldFlexMEn->exec(event->globalPos());
/*  QMenu* menuFlex=setupFieldFlexMenu();
  menuFlex->exec(event->globalPos());
  menuFlex->deleteLater();
*/
}


void
DicEditMWn::setupReadWriteMenu()
{
  SaveQAc = new QAction(QObject::tr("Save","DicEdit"),this);
  FwritetoMEn=new QMenu(QObject::tr("Save as","DicEdit"),this);
  UpdateEmbeddedQAc = new QAction(QObject::tr("Update embedded","DicEdit"),this);

  FwritetoDicQAc = new QAction(QObject::tr("dictionary file","DicEdit"),this);
  FwritetoDicQAc->setObjectName("FwritetoDicQAc");
  FwritetoMEn->addAction(FwritetoDicQAc);

  FwritetoclipQAc = new QAction(QObject::tr("clipboard as xml","DicEdit"),this);

  FwritetoMEn->addAction(FwritetoclipQAc);

  FwriteXmltofileQAc = new QAction(QObject::tr("xml file","DicEdit"),this);

  FwritetoMEn->addAction(FwriteXmltofileQAc);

  FviewXmlQAc = new QAction(QObject::tr("view xml","DicEdit"),this);
  FviewXmlQAc->setObjectName("FviewXmlQAc");
  FwritetoMEn->addAction(FviewXmlQAc);

  FloadMEn=new QMenu(QObject::tr("load from","DicEdit"),this);

  FloadfromDicQAc = new QAction(QObject::tr("dictionary file","DicEdit"),this);
  FloadfromDicQAc->setObjectName("FloadfromDicQAc");
  FloadMEn->addAction(FloadfromDicQAc);

  FloadfromXmlFileQAc = new QAction(QObject::tr("xml file","DicEdit"),this);
  FloadfromXmlFileQAc->setObjectName("FloadfromXmlFileQAc");
  FloadMEn->addAction(FloadfromXmlFileQAc);

  FloadfromclipQAc = new QAction(QObject::tr("clipboard","DicEdit"),this);
  FloadfromclipQAc->setObjectName("FloadfromclipQAc");
  FloadMEn->addAction(FloadfromclipQAc);

}//setupReadWriteMenu


QMenu *
DicEditMWn::setupFieldFlexMenu()
{
  fieldFlexMEn=new QMenu(keyTRv);

  fieldFlexMEn->setTitle(QObject::tr("Fields","DicEdit"));

  fieldActionGroup=new QActionGroup(fieldFlexMEn) ;
  QObject::connect(fieldActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(fieldActionEvent(QAction*)));

  FInsertQAc= new QAction(fieldFlexMEn);

  FInsertQAc->setText( QObject::tr("Insert new field","DicEdit"));
  FInsertQAc->setObjectName("FInsertQAc");
  fieldFlexMEn->addAction(FInsertQAc);
  fieldActionGroup->addAction(FInsertQAc);


  FAppendQAc= new QAction(fieldFlexMEn);

  FAppendQAc->setText(QObject::tr("Append new field","DicEdit"));
  FAppendQAc->setObjectName("FAppendQAc");
  fieldFlexMEn->addAction(FAppendQAc);
  fieldActionGroup->addAction(FAppendQAc);

  FChangeQAc= new QAction(fieldFlexMEn);
  FChangeQAc->setText(QCoreApplication::translate("DicEdit", "Change", nullptr));
  FChangeQAc->setObjectName("FChangeQAc");
  fieldFlexMEn->addAction(FChangeQAc);
  fieldActionGroup->addAction(FChangeQAc);

  FDeleteQAc= new QAction(fieldFlexMEn);
  FDeleteQAc->setText(QObject::tr("Delete","DicEdit"));
  FDeleteQAc->setObjectName("FDeleteQAc");
  fieldFlexMEn->addAction(FDeleteQAc);
  fieldActionGroup->addAction(FDeleteQAc);

  fieldFlexMEn->addSeparator();

  FCutQAc= new QAction(fieldFlexMEn);
  FCutQAc->setText(QObject::tr("Cut","DicEdit"));
  FCutQAc->setObjectName("FCutQAc");
  fieldFlexMEn->addAction(FCutQAc);
  fieldActionGroup->addAction(FCutQAc);

  FcopyQAc= new QAction(fieldFlexMEn);
  FcopyQAc->setText(QObject::tr("Copy","DicEdit"));
  FcopyQAc->setObjectName("FcopyQAc");
  fieldFlexMEn->addAction(FcopyQAc);
  fieldActionGroup->addAction(FcopyQAc);

  FpasteQAc= new QAction(fieldFlexMEn);
  FpasteQAc->setText(QObject::tr("Paster field here","DicEdit"));
  FpasteQAc->setObjectName("FpasteQAc");
  fieldFlexMEn->addAction(FpasteQAc);
  fieldActionGroup->addAction(FpasteQAc);

  FmoveupQAc= new QAction(fieldFlexMEn);
  FmoveupQAc->setText(QObject::tr("Move up","DicEdit"));
  fieldFlexMEn->addAction(FmoveupQAc);
  fieldActionGroup->addAction(FmoveupQAc);

  FmovedownQAc= new QAction(fieldFlexMEn);
  FmovedownQAc->setText(QObject::tr("Move down","DicEdit"));
  fieldFlexMEn->addAction(FmovedownQAc);
  fieldActionGroup->addAction(FmovedownQAc);

  if (FwritetoMEn==nullptr)
    setupReadWriteMenu();

  fieldFlexMEn->addSeparator();
  fieldFlexMEn->addMenu(FwritetoMEn);
  fieldFlexMEn->addMenu(FloadMEn);

  fieldActionGroup->addAction(FwritetoclipQAc);
  fieldActionGroup->addAction(FviewXmlQAc);
  fieldActionGroup->addAction(FwriteXmltofileQAc);
  fieldActionGroup->addAction(FwritetoDicQAc);

  fieldActionGroup->addAction(FloadfromclipQAc);
  fieldActionGroup->addAction(FloadfromXmlFileQAc);
  fieldActionGroup->addAction(FloadfromDicQAc);

  return fieldFlexMEn;
}//setupFieldFlexMenu



void DicEditMWn::fieldActionEvent(QAction* pAction)
{
  utf8String wMsg;
  if (pAction==FInsertQAc) {
    QModelIndex wIdx=fieldTBv->currentIndex();
    if (wIdx.isValid())
      fieldInsertNewBefore(wIdx);
    else
      fieldAppend();
    return;
  }

  if (pAction==FAppendQAc)
  {
    if (fieldAppend()) {
      ui->statusBar->showMessage(QObject::tr("1 field created and appended","DicEdit"),cst_MessageDuration);
    }
    return;
  }

  if (pAction==FChangeQAc) {
    fieldChange(fieldTBv->currentIndex());
    return;
  }// FChangeQAc

  if (pAction==FDeleteQAc)
  {
    if (fieldDelete()) {
      ui->statusBar->showMessage(QObject::tr("1 field deleted","DicEdit"),cst_MessageDuration);
    }
    return;
  }
/* copy dictionary field to pinboard : only QStandardItem* (col 0) is copied : if field is later on pasted,
 * then field row has to be rebuilt from infra data (ZFieldDescription)
 * However, field name must be unique.
*/
  if (pAction==FcopyQAc) {

    if(!fieldTBv->currentIndex().isValid()) {
      statusBarMessage(QObject::tr("Nothing selected.","DicEdit").toUtf8().data());
      return;
    }
    QModelIndex wIdx=fieldTBv->currentIndex();
    if (wIdx.column()!=0) {
      wIdx=wIdx.siblingAtColumn(0);
    }

    QStandardItem* wFieldItem=fieldTBv->ItemModel->itemFromIndex(wIdx);
    if (wFieldItem==nullptr) {
      statusBarMessage("cannot get field item.");
      return;
    }

    ZDataReference wDRef (ZLayout_FieldTBv,getNewResource(ZEntity_DicFieldItem));
    wDRef.setPtr<QStandardItem>(wFieldItem);
    ZPinboardElement wPBElt;
    wPBElt.setDataReference(wDRef);
    Pinboard.push(wPBElt);

    ui->statusBar->showMessage(QObject::tr("1 field copied to pinboard","DicEdit"),cst_MessageDuration);
    return;
  } // FcopyQAc

  /* cut dictionary field to pinboard : the whole dictionary field row is cut (using takerow) ,
   * The result is a QList<QStandardItem*> that is passed as a pointer to pinboard qualified as ZEntity_DicFieldRow
   * then field row may be directly inserted as is.
   * field name is then reputated to be unique.
   */
  if (pAction==FCutQAc) {
    QModelIndex wIdx=fieldTBv->currentIndex();
    if (!wIdx.isValid()) {
      ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
      return ;
    }
    QList<QStandardItem*>* wRow = new QList<QStandardItem*>(fieldTBv->ItemModel->takeRow(wIdx.row()));

    ZDataReference wDRef (ZLayout_FieldTBv,getNewResource(ZEntity_DicFieldRow));
    wDRef.setPtr<QList<QStandardItem*>>(wRow);
    ZPinboardElement wPBElt;
    wPBElt.setDataReference(wDRef);
    Pinboard.push(wPBElt);
    ui->statusBar->showMessage(QObject::tr("1 field cut to pinboard","DicEdit"),cst_MessageDuration);
    return;
  } // FCutQAc

  /* paste dictionary field to field table view :
   *  from copy field -> duplicate field : (source is  ZEntity_DicFieldItem)
   *        create QStandardItems from ZFieldDescription with ZFieldDescription as infradata
   *        check name is unique, if not, ask for new name
   *        insert <before> created item row at appropriate row
   *
   *  from cut -> simply insert <before> QList at appropriate row (source must be  ZEntity_DicFieldRow)
   */
  if (pAction==FpasteQAc) {

    if (Pinboard.isEmpty()) {
      statusBarMessage(QObject::tr("Nothing in pinboard.","DicEdit").toUtf8());
      return ;
    }
    if (Pinboard.getLast()->DRef.getZEntity()!=ZEntity_DicFieldRow ) {
      statusBarMessage("Cannot paste this to field table view : Invalid entity from pinboard <%s> while expecting ZEntity_DicFieldRow.",
          decode_ZEntity (Pinboard.getLast()->DRef.getZEntity()).toCChar());
      return;
    }

    QList<QStandardItem*>* wRow= Pinboard.getLast()->DRef.getPtr<QList<QStandardItem*>>();
    ZDataReference wDRef;
    QVariant wV=(*wRow)[0]->data(ZQtDataReference);
    wDRef=wV.value<ZDataReference>();
    utf8VaryingString wFieldName = wDRef.getPtr<ZFieldDescription>()->getName();

    if (fieldTBv->currentIndex().isValid()) {
      QModelIndex wIdx=fieldTBv->currentIndex();
      int wRowNb= wIdx.row();
      fieldTBv->ItemModel->insertRow(wRowNb,*wRow);
      statusBarMessage("field <%s> inserted at row %d",wFieldName.toCChar(),wRowNb);
    }
    else {
      fieldTBv->ItemModel->appendRow(*wRow);
      statusBarMessage("field <%s> appended.",wFieldName.toCChar());
    }
    DictionaryChanged=true;
    Pinboard.pop();
    delete wRow;
  }

  if (pAction==FmoveupQAc)
  {
    fieldMoveup();
    return;
  }
  if (pAction==FmovedownQAc)
  {
    fieldMovedown();
    return;
  }


  readWriteActionEvent(pAction);  /* call the common menu actions */

  return;
}//fieldActionEvent


QMenu *
DicEditMWn::setupKeyFlexMenu()
{
  keyFlexMEn=new QMenu(keyTRv);
  keyFlexMEn->setTitle(QCoreApplication::translate("DicEdit", "Keys", nullptr));

  keyActionGroup=new QActionGroup(keyFlexMEn) ;
  QObject::connect(keyActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(keyActionEvent(QAction*)));

  KInsertKeyQAc= new QAction(keyFlexMEn);
  KInsertKeyQAc->setText(QCoreApplication::translate("DicEdit", "Insert new key", nullptr));
  KInsertKeyQAc->setObjectName("KInsertKeyQAc");

  KAppendKeyQAc= new QAction(keyFlexMEn);
  KAppendKeyQAc->setText(QCoreApplication::translate("DicEdit", "Add new key", nullptr));
  KAppendKeyQAc->setObjectName("KAppendKeyQAc");

  KChangeQAc= new QAction(keyFlexMEn);  /* only keys may be changed */
  KChangeQAc->setText(QCoreApplication::translate("DicEdit", "Change key", nullptr));
//  KChangeQAc->setObjectName("KChangeKeyQAc");

//  KDeleteKeyQAc= new QAction(keyFlexMEn);
//  KDeleteKeyQAc->setText(QCoreApplication::translate("DicEdit", "Delete key", nullptr));
//  KDeleteKeyQAc->setObjectName("KDeleteKeyQAc");

  KDeleteQAc= new QAction(keyFlexMEn);
  KDeleteQAc->setText(QCoreApplication::translate("DicEdit", "Delete", nullptr));
  KDeleteQAc->setObjectName("KDeleteQAc");

  KcutQAc= new QAction(keyFlexMEn);
  KcutQAc->setText(QCoreApplication::translate("DicEdit", "Cut", nullptr));
  KcutQAc->setObjectName("KcutQAc");

  KpasteQAc= new QAction(keyFlexMEn);
  KpasteQAc->setText(QCoreApplication::translate("DicEdit", "Paste", nullptr));
  KpasteQAc->setObjectName("KpasteQAc");

  KappendQAc= new QAction(keyFlexMEn);
  KappendQAc->setText(QCoreApplication::translate("DicEdit", "Append", nullptr));
  KappendQAc->setObjectName("KappendQAc");

  KMoveupQAc= new QAction(keyFlexMEn);
  KMoveupQAc->setText(QCoreApplication::translate("DicEdit", "Move up", nullptr));
  KMoveupQAc->setObjectName("KMoveupQAc");

  KMovedownQAc= new QAction(keyFlexMEn);
  KMovedownQAc->setText(QCoreApplication::translate("DicEdit", "Move down", nullptr));
  KMovedownQAc->setObjectName("KKeyMovedownQAc");

  keyFlexMEn->addAction(KDeleteQAc);
  keyActionGroup->addAction(KDeleteQAc);

  keyFlexMEn->addSeparator();
  keyFlexMEn->addAction(KMoveupQAc);
  keyActionGroup->addAction(KMoveupQAc);
  keyFlexMEn->addAction(KMovedownQAc);
  keyActionGroup->addAction(KMovedownQAc);
//  keyFlexMEn->addAction(KKeyfieldMoveupQAc);
//  keyActionGroup->addAction(KKeyfieldMoveupQAc);
//  keyFlexMEn->addAction(KKeyfieldMovedownQAc);
//  keyActionGroup->addAction(KKeyfieldMovedownQAc);

  keyFlexMEn->addSeparator();
  keyFlexMEn->addAction(KInsertKeyQAc);
  keyActionGroup->addAction(KInsertKeyQAc);
  keyFlexMEn->addAction(KAppendKeyQAc);
  keyActionGroup->addAction(KAppendKeyQAc);
  keyFlexMEn->addAction(KChangeQAc);
  keyActionGroup->addAction(KChangeQAc);
//  keyFlexMEn->addAction(KDeleteKeyQAc);
//  keyActionGroup->addAction(KDeleteKeyQAc);

  keyFlexMEn->addSeparator();
  keyFlexMEn->addAction(KcutQAc);
  keyActionGroup->addAction(KcutQAc);
  keyFlexMEn->addAction(KpasteQAc);
  keyActionGroup->addAction(KpasteQAc);
  keyFlexMEn->addAction(KappendQAc);
  keyActionGroup->addAction(KappendQAc);

  if (FwritetoMEn==nullptr)
    setupReadWriteMenu();

  keyFlexMEn->addSeparator();
  keyFlexMEn->addMenu(FwritetoMEn);
  keyFlexMEn->addMenu(FloadMEn);

  keyActionGroup->addAction(FwritetoclipQAc);
  keyActionGroup->addAction(FviewXmlQAc);
  keyActionGroup->addAction(FwriteXmltofileQAc);
  keyActionGroup->addAction(FwritetoDicQAc);

  keyActionGroup->addAction(FloadfromclipQAc);
  keyActionGroup->addAction(FloadfromXmlFileQAc);
  keyActionGroup->addAction(FloadfromDicQAc);

  return keyFlexMEn;
} // setupKeyFlexMenu

void DicEditMWn::keyActionEvent(QAction* pAction)
{
  utf8String wMsg;
  if (pAction==KInsertKeyQAc) {
    insertNewKey();
    return;
  }
  if (pAction==KMoveupQAc) {
    QModelIndex wIdx= keyTRv->currentIndex();
    if(!wIdx.isValid())
      return;

    if (wIdx.column()!=0)           /* because infra data is stored at column 0 */
      wIdx=wIdx.siblingAtColumn(0);

    /* extract data from item */
    QVariant wV=wIdx.data(ZQtDataReference);
    ZDataReference wDRef=wV.value<ZDataReference>();

    if (wDRef.getZEntity()==ZEntity_KeyDic) {
      keyMoveup();
      return;
    }
    if (wDRef.getZEntity()!=ZEntity_KeyField) {
      statusBarMessage("KDeleteQAc-E-INVTYP Invalid infradata type <%s> for row.",wDRef.getZEntity());
    }

    keyfieldMoveup();
    return;
  }
  if (pAction==KMovedownQAc) {
    QModelIndex wIdx= keyTRv->currentIndex();
    if(!wIdx.isValid())
      return;

    if (wIdx.column()!=0)           /* because infra data is stored at column 0 */
      wIdx=wIdx.siblingAtColumn(0);

    /* extract data from item */
    QVariant wV=wIdx.data(ZQtDataReference);
    ZDataReference wDRef=wV.value<ZDataReference>();

    if (wDRef.getZEntity()==ZEntity_KeyDic) {
      keyMovedown();
      return;
    }
    if (wDRef.getZEntity()!=ZEntity_KeyField) {
      statusBarMessage("KDeleteQAc-E-INVTYP Invalid infradata type <%s> for row.",wDRef.getZEntity());
    }

    keyfieldMovedown();
    return;
  }


  if (pAction==KAppendKeyQAc) {
    appendNewKey();
    return;
  }
  if (pAction==KChangeQAc) {
    QModelIndex wIdx=keyTRv->currentIndex();
    if (!wIdx.isValid()) {
      return ;
    }
    keyChange(wIdx);
    return;
  }

  if (pAction==KDeleteQAc) {
    QModelIndex wIdx= keyTRv->currentIndex();

    if(!wIdx.isValid())
      return;

    if (wIdx.column()!=0)           /* because infra data is stored at column 0 */
      wIdx=wIdx.siblingAtColumn(0);

    /* must delete infra data object */

    /* extract data from item */
    QVariant wV=wIdx.data(ZQtDataReference);
    ZDataReference wDRef=wV.value<ZDataReference>();

    if (wDRef.getZEntity()==ZEntity_KeyDic) {
      _keyDelete(wIdx);
      return;
    }
    if (wDRef.getZEntity()!=ZEntity_KeyField) {
      statusBarMessage("KDeleteQAc-E-INVTYP Invalid infradata type <%s> for row.",wDRef.getZEntity());
    }

    _keyfieldDelete(wIdx);
    return;
  }// KDeleteQAc

  if (pAction==KcutQAc)
    {
    QModelIndex wIdx= keyTRv->currentIndex();
    if(!wIdx.isValid()){
        wMsg.sprintf("no row selected");
        ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
        return;
    }
    if (wIdx.column()!=0)
      wIdx=wIdx.siblingAtColumn(0);

    ZPinboardElement wElt;
    QStandardItem* wCurItem= keyTRv->ItemModel->itemFromIndex(wIdx);

    /* extract data from item */
    QVariant wV=wIdx.data(ZQtDataReference);
    ZDataReference wDRef=wV.value<ZDataReference>();

    if (wDRef.getZEntity()==ZEntity_KeyDic) {
      QList<QStandardItem*>* wKeyRow= new QList<QStandardItem*>(keyTRv->ItemModel->takeRow(wIdx.row()));
      wElt.DRef.setZLayout(ZLayout_KeyTRv);
      wElt.DRef.setResource(getNewResource(ZEntity_KeyDicRow));
      wElt.DRef.setDataRank(0);
      wElt.DRef.setPtr<QList<QStandardItem*>>(wKeyRow);
      return;
      }
    if (wDRef.getZEntity()==ZEntity_KeyField) {
      QStandardItem* wKeyParent= wCurItem->parent();
      QList<QStandardItem*>* wKeyFieldRow= new QList<QStandardItem*>(wKeyParent->takeRow(wIdx.row()));
      wElt.DRef.setZLayout(ZLayout_KeyTRv);
      wElt.DRef.setResource(getNewResource(ZEntity_KeyDicRow));
      wElt.DRef.setDataRank(0);
      wElt.DRef.setPtr(wKeyFieldRow);
      return;
    }
    return;
    }// KcutQAc

/* paste from pinboard :
 *  a previously cut key row            ZEntity_KeyDicRow
 *  a previously cut key field row      ZEntity_KeyFieldRow
 *  a copied field item from fieldTBv   ZEntity_DicFieldItem
 */
  if (pAction==KpasteQAc) {
    keyTRvInsertFromPinboard (keyTRv->currentIndex());
    return;
    }// KappendQAc

    readWriteActionEvent(pAction);  /* call the common menu actions */

  return;
}//keyActionEvent

bool
DicEditMWn::getDicName(unsigned long &pVersion,bool &pActive,utf8VaryingString& pDicName)
{
  utf8VaryingString wDicName;
  if (pDicName.isEmpty())
    wDicName="<no name>";
  else
    wDicName=pDicName;

  utf8VaryingString wVersion=getVersionStr(pVersion);

  QDialog wDicNameDLg;
  wDicNameDLg.setObjectName("wDicNameDLg");
  wDicNameDLg.setWindowTitle(QObject::tr("Dictionary name","DicEdit"));
  wDicNameDLg.resize(400,150);

  QVBoxLayout* QVL=new QVBoxLayout(&wDicNameDLg);
  wDicNameDLg.setLayout(QVL);

  QHBoxLayout* QHL=new QHBoxLayout;
  QVL->insertLayout(0,QHL);
  QLabel* wLb=new QLabel(QObject::tr("Dictionary name","DicEdit"),&wDicNameDLg);
  QHL->addWidget(wLb);
  QLineEdit* wDicNameLEd=new QLineEdit(wDicName.toCChar(),&wDicNameDLg);
  QHL->addWidget(wDicNameLEd);

  QHBoxLayout* QHL1=new QHBoxLayout;
  QVL->insertLayout(1,QHL1);
  QLabel* wLb1=new QLabel(QObject::tr("Version number","DicEdit"),&wDicNameDLg);
  QHL1->addWidget(wLb1);
  QLineEdit* wVersionLEd=new QLineEdit(wVersion.toCChar());
  wVersionLEd->setObjectName("wVersionLEd");
  QHL1->addWidget(wVersionLEd);
  QCheckBox* wActiveCHk=new QCheckBox("Active version",&wDicNameDLg);
  QHL1->addWidget(wActiveCHk);
  wActiveCHk->setChecked(pActive);

  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QHLBtn->setObjectName("QHLBtn");
  QVL->insertLayout(2,QHLBtn);

  QPushButton* wOk=new QPushButton(QObject::tr("Ok","DicEdit"),&wDicNameDLg);
  wOk->setObjectName("wOk");
  QPushButton* wCancel=new QPushButton(QObject::tr("Cancel","DicEdit"),&wDicNameDLg);
  wCancel->setObjectName("wCancel");
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHLBtn->addItem(wSpacer);

  QHLBtn->addWidget(wCancel);
  QHLBtn->addWidget(wOk);

  wDicNameLEd->setText(wDicName.toCChar());
  wDicNameLEd->setSelection(0,wDicName.UnitCount);

  wDicNameDLg.setWindowTitle(QObject::tr("Dictionary identification","DicEdit"));

  QObject::connect(wOk, &QPushButton::clicked, &wDicNameDLg, &QDialog::accept);
  QObject::connect(wCancel, &QPushButton::clicked, &wDicNameDLg, &QDialog::reject);

  int wRet=wDicNameDLg.exec();
  if (wRet==QDialog::Accepted)
  {
    pDicName=wDicNameLEd->text().toUtf8().data();
    pVersion = getVersionNum(wVersionLEd->text().toUtf8().data());
    pActive = wActiveCHk->isChecked();
    return true;
  }
  return false;

}//getDicName

bool
DicEditMWn::getGenerationNames(utf8VaryingString& pOutFileBaseName,utf8VaryingString& pClass,utf8VaryingString& pBrief,utf8VaryingString& pGenPath)
{

  utf8VaryingString wClass=pClass,wOutFileName;
  if (pOutFileBaseName.isEmpty())
    wOutFileName=wClass;
  else
    wOutFileName=pOutFileBaseName;

  SelectedDirectory=pGenPath;

  QDialog wGenNamesDLg;
  wGenNamesDLg.setObjectName("wGenNamesDLg");
  wGenNamesDLg.setWindowTitle(QObject::tr("Generation names","DicEdit"));
  wGenNamesDLg.resize(400,150);

  QVBoxLayout* QVL=new QVBoxLayout(&wGenNamesDLg);
  wGenNamesDLg.setLayout(QVL);

  QVL->setObjectName("QVL");
  QHBoxLayout* QHL=new QHBoxLayout;
  QHL->setObjectName("QHL");
  QVL->insertLayout(0,QHL);
  QLabel* wLb=new QLabel(QObject::tr("Class name","DicEdit"),&wGenNamesDLg);
  wLb->setObjectName("wLb");
  QHL->addWidget(wLb);
  QLineEdit* wClassLEd=new QLineEdit(pClass.toCChar(),&wGenNamesDLg);
  wClassLEd->setObjectName("wClassLEd");
  QHL->addWidget(wClassLEd);

  QHBoxLayout* QHL1=new QHBoxLayout;
  QHL1->setObjectName("QHL1");
  QVL->insertLayout(1,QHL1);

  QLabel* wLb1=new QLabel(QObject::tr("Output base name","DicEdit"),&wGenNamesDLg);
  wLb1->setObjectName("wLb1");
  QHL1->addWidget(wLb1);
  QLineEdit* wOutNameLEd=new QLineEdit(pOutFileBaseName.toCChar());
  wOutNameLEd->setObjectName("wVersionLEd");
  QHL1->addWidget(wOutNameLEd);

  QHBoxLayout* QHL2=new QHBoxLayout;
  QHL2->setObjectName("QHL2");
  QVL->insertLayout(2,QHL2);

  QLabel* wLb2=new QLabel(QObject::tr("Class description","DicEdit"),&wGenNamesDLg);
  wLb2->setObjectName("wLb2");
  QHL2->addWidget(wLb2);
  QLineEdit* wBriefLEd=new QLineEdit(pBrief.toCChar());
  wBriefLEd->setObjectName("wBriefLEd");
  QHL2->addWidget(wBriefLEd);

  QHBoxLayout* QHL3=new QHBoxLayout;
  QHL3->setObjectName("QHL3");
  QVL->insertLayout(3,QHL3);

  QLabel* wLb3=new QLabel(QObject::tr("Target directory","DicEdit"),&wGenNamesDLg);
  wLb3->setObjectName("wLb3");
  QHL3->addWidget(wLb3);
  GenPathLEd=new QLineEdit(pGenPath.toCChar());
//  wGenPathLEd->setObjectName("wGenPathLEd");
  QHL3->addWidget(GenPathLEd);
  QPushButton* wDirBTn=new QPushButton(QObject::tr("Search","DicEdit"),&wGenNamesDLg);
  QHL3->addWidget(wDirBTn);

  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QHLBtn->setObjectName("QHLBtn");
  QVL->insertLayout(4,QHLBtn);

  QPushButton* wOk=new QPushButton(QObject::tr("Ok","DicEdit"),&wGenNamesDLg);
  wOk->setObjectName("wOk");
  QPushButton* wCancel=new QPushButton(QObject::tr("Cancel","DicEdit"),&wGenNamesDLg);
  wCancel->setObjectName("wCancel");
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHLBtn->addItem(wSpacer);

  QHLBtn->addWidget(wCancel);
  QHLBtn->addWidget(wOk);

  wClassLEd->setText(wClass.toCChar());


  wBriefLEd->setText(pBrief.toCChar());

  wGenNamesDLg.setWindowTitle(QObject::tr("Dictionary identification","DicEdit"));

  QObject::connect(wOk, &QPushButton::clicked, &wGenNamesDLg, &QDialog::accept);
  QObject::connect(wCancel, &QPushButton::clicked, &wGenNamesDLg, &QDialog::reject);

  QObject::connect(wDirBTn, &QPushButton::clicked, this, &DicEditMWn::searchDirectory);


  while (true) {
    wClassLEd->setSelection(0,wClass.UnitCount);
    wClassLEd->setFocus();
    int wRet=wGenNamesDLg.exec();
    if (wRet==QDialog::Accepted) {
      wClass=wClassLEd->text().toUtf8().data();
      wOutFileName = wOutNameLEd->text().toUtf8().data();
      pBrief = wBriefLEd->text().toUtf8().data();
      pGenPath = GenPathLEd->text().toUtf8().data();

      if (wClass.contains((utf8_t*)" ")) {
        ZExceptionDLg::adhocMessage("Invalid name",Severity_Error,&ErrorLog,nullptr,"Class name <%s> must not contain space(s)",wClass.toString());
        continue;
      }
      uriString wGenPath=GenPathLEd->text().toUtf8().data();
      if (!wGenPath.exists()) {
        ZExceptionDLg::adhocMessage("Generation directory",Severity_Error,&ErrorLog,nullptr,"Generation directory <%s> does not exist.",wGenPath.toString());
        continue;
      }
      pClass = wClass;
      if (wOutFileName.isEmpty())
        wOutFileName.clear();
      pOutFileBaseName = wOutFileName;
      return true;
    } // if (wRet==QDialog::Accepted)
    return false;
  }// while true
}//getGenNames

void
DicEditMWn::searchDirectory() {


    QString wD=QFileDialog::getExistingDirectory(this,"Source directory",SelectedDirectory.toCChar(),QFileDialog::ShowDirsOnly);
    if (wD.isEmpty()) {
      return;
    }
    SelectedDirectory = wD.toUtf8().data();
    GenPathLEd->setText(wD);
    return;
}

void
DicEditMWn:: renewDicFile(ZMFDictionary& pDic){
  if (DictionaryFile==nullptr)
    DictionaryFile = new ZDictionaryFile;
  DictionaryFile->setDictionary(pDic);
}

void
DicEditMWn::readWriteActionEvent(QAction*pAction)
{
  utf8String wMsg;

  /* common menu actions key and field menu */


  if  (pAction==SaveQAc) /* save to dictionary file if exists  */
  {
    if (fieldTBv->ItemModel->rowCount()==0) {
      ZExceptionDLg::adhocMessage("Save",Severity_Error,&ErrorLog,nullptr,"Nothing to save");
      return;
    }
    while (true)
    {
    if (MasterFile!=nullptr) {
        if (MasterFile->Dictionary!=nullptr) {
            updateEmbedded();
            break;
        }
    }
    if (DictionaryFile==nullptr) {
        break;
    }
    if (DictionaryFile->URIDictionary.isEmpty()) {
        if (XmlDictionaryFile.isEmpty()) {
            saveOrCreateDictionaryFile();
            break;
        }
        DictionaryFile->URIDictionary = XmlDictionaryFile;
    }

    screenToDic(DictionaryFile);
    DictionaryFile->save_xml(true);
    utf8VaryingString wStr;
    wStr.sprintf("Dictionary file %s saved with %d fields and %d key(s)",
                     DictionaryFile->URIDictionary.toCChar(),DictionaryFile->count(),DictionaryFile->KeyDic.count());
    ui->statusBar->showMessage(wStr.toCChar(),cst_MessageDuration);
    break;
  /*
    if (!XmlDictionaryFile.isEmpty()) {
        saveToXmlFile(XmlDictionaryFile);
        break;
    }
    */
    }// while true
    return;
  }// SaveQAc

  if  (pAction==UpdateEmbeddedQAc) /* save to dictionary embedded in MasterFile if exists and if open */
  {
    if (fieldTBv->ItemModel->rowCount()==0) {
      ZExceptionDLg::adhocMessage("Save",Severity_Error,&ErrorLog,nullptr,"Nothing to save");
      return;
    }
    updateEmbedded();
    return;
  }// SaveEmbeddedQAc
  /* @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types */
  if  (pAction==FwritetoclipQAc) /* generate xml from current content and write xml to clipboard */
  {
    if (fieldTBv->ItemModel->rowCount()==0) {
      ZExceptionDLg::adhocMessage("Save",Severity_Error,&ErrorLog,nullptr,"Nothing to write");
      return;
    }
    /* generate xml from current views content */

    ZMFDictionary* wMasterDic = screenToDic(nullptr);  /* get Master dictionary content from views */
    if (wMasterDic==nullptr)
      return;
    getDicName(wMasterDic->Version,wMasterDic->Active, wMasterDic->DicName);         /* name meta dictionary */

    utf8String wXmlDic=wMasterDic->XmlSaveToString(true); /* generate full xml definition */

    /* effective write to clipboard */
    QByteArray wBA (wXmlDic.toCChar());
    QClipboard *wClipboard = QGuiApplication::clipboard();
    QMimeData* wMime=new QMimeData;
    /* mime type could be application/xml or (deprecated) text/xml */
    /* but clipboard is always text/plain */
    wMime->setData("text/plain",wBA);
    wClipboard->setMimeData(wMime,QClipboard::Clipboard);

    statusBar()->showMessage(QObject::tr("Dictionary content has been copied to clipboard.","DicEdit"),cst_MessageDuration);
    delete wMasterDic;

    return;
  }// FwritetoclipQAc

  if  (pAction==FloadfromclipQAc)
    {
    ZStatus wSt;
    ZMFDictionary wMasterDic;
    utf8String wXmlContent;
    QClipboard *wClipboard = QGuiApplication::clipboard();
    const QMimeData * wMData=wClipboard->mimeData(QClipboard::Clipboard);
    if (wMData==nullptr)
      {
      statusBar()->showMessage(QObject::tr("Clipboard is empty","DicEdit"),cst_MessageDuration);
      }
    if (!wMData->hasFormat("text/plain"))
      {
      statusBar()->showMessage(QObject::tr("Clipboard has no text data.","DicEdit"),cst_MessageDuration);
      }
    wXmlContent = wMData->data("text/plain").data();  /* load mime data as utf8 string */

    ErrorLog.setContext("FloadfromclipQAc");
    /* here load master dictionary */
    wSt =wMasterDic.XmlLoadFromString(wXmlContent,true,&ErrorLog);
    if (wSt!=ZS_SUCCESS)   /* XmlLoadFromString uses returns a ZStatus but uses ErrorLog to log messages */
    {
      ZExceptionDLg::messageWAdd("FloadfromclipQAc",
          wSt,
          ErrorLog.getSeverity(),
          ErrorLog.allLoggedToString(),
          "Status is %s : Cannot load xml string definition from clipboard.",decode_ZStatus(wSt));
      return;
    }
    /* here we may only have warnings, infos or text messages */
    if (ErrorLog.hasMessages())
    {
      Severity_type wS = ErrorLog.getSeverity();

      int wRet = ZExceptionDLg::message2BWAdd("FloadfromclipQAc",
          wS > Severity_Warning?ZS_ERROR:ZS_WARNING,
          wS,
          ErrorLog.allLoggedToString(),
          "Stop","Continue",
          "Some warning messages have been issued during load of xml definition from clipboard");
      if (wRet==QDialog::Rejected)
        return;
    }

//    delete MasterDic;
//    MasterDic = new ZMFDictionary(wMasterDic) ;

    renewDicFile(wMasterDic);
    XmlDictionaryFile ="clipboard.xml";
    wMsg.sprintf("Xml dictionary definition has been successfully loaded from clipboard.");
    statusBar()->showMessage(QObject::tr(wMsg.toCChar(),"DicEdit"),cst_MessageDuration);

    ui->SourceLBl->setText("Clipboard");
    ui->SourceURILBl->setText("");
    XmlDictionaryFile.clear();

    DictionaryChanged=false;
    } //  FloadfromclipQAc


  if  (pAction==FviewXmlQAc) /* generate xml from current content and view it */
  {
    /* generate xml from current views content */
    ZMFDictionary* wMasterDic = screenToDic(nullptr);
    getDicName(wMasterDic->Version,wMasterDic->Active,wMasterDic->DicName);

    utf8String wXmlDic=wMasterDic->XmlSaveToString(true);

    /* display xml content */
    textEditMWn* wTEx=new textEditMWn((QWidget*)this,
                                        TEOP_Nothing, /*no line numbers and close button is really closing and destroying dialog */
                                        nullptr);   /* no close callback */
    wTEx->setText(wXmlDic,wMasterDic->DicName);



    delete wMasterDic;
    wTEx->show();
    return;
  }//  FviewXmlQAc

  if  (pAction==FwriteXmltofileQAc)
  {
    uriString wDir = GeneralParameters.getWorkDirectory();
//    const char* wDir=getWorkDirectory();

    QString wFileName = QFileDialog::getSaveFileName(this, tr("Save as xml file"),
                                                      wDir.toCChar(),
                                                      "Xml files (*.xml);;All (*.*)");
    if (wFileName.isEmpty()) {
          return ;
    }

    uriString wOutFile= wFileName.toUtf8().data();
    if (wOutFile.getFileExtension().isEmpty()) {
        wOutFile.changeFileExtension(".xml");
    }
    bool Fexists=wOutFile.exists();

    /* generate xml from current views content */
    ZMFDictionary* wMasterDic = screenToDic(nullptr);
    wMasterDic->CreationDate = ZDateFull::currentDateTime();
    wMasterDic->ModificationDate = ZDateFull::currentDateTime();
    getDicName(wMasterDic->Version,wMasterDic->Active,wMasterDic->DicName);

    utf8String wXmlDic=wMasterDic->XmlSaveToString(true);

    wOutFile.writeContent(wXmlDic);

    delete wMasterDic;

    wMsg.sprintf("file %s has been %s",wOutFile.toCChar(),Fexists?"replaced":"created");
    statusBar()->showMessage(QObject::tr(wMsg.toCChar(),"DicEdit"),cst_MessageDuration);
    return;
  } // FwriteXmltofileQAc


  if  (pAction==FloadfromXmlFileQAc) {

    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, tr("Open xml file"),
                                                     wDir.toCChar(),
                                                     "Xml files (*.xml);;All (*.*)");
    if (wFileName.isEmpty())
      return;

    XmlDictionaryFile= wFileName.toUtf8().data();

    ZMFDictionary wMasterDic;
    utf8VaryingString wXmlContent;

    ZStatus wSt = XmlDictionaryFile.loadUtf8(wXmlContent);
    if (wSt!=ZS_SUCCESS)
      {
      ZExceptionDLg::displayLast();  /* loadUtf8 uses ZException */
      return;
      }
    /* here load master dictionary */
    wSt =wMasterDic.XmlLoadFromString(wXmlContent,true,&ErrorLog);
    if (wSt!=ZS_SUCCESS)  { /* XmlLoadFromString uses returns a ZStatus but uses ErrorLog to log messages */
      ZExceptionDLg::messageWAdd("FloadfromXmlFileQAc",
          wSt,
          ErrorLog.getSeverity(),
          ErrorLog.allLoggedToString(),
          "Status is %s : Cannot load xml string definition from file %s",decode_ZStatus(wSt), XmlDictionaryFile.toCChar());
      return;
      }
  /* here we may only have warnings, infos or text messages */
    if (ErrorLog.hasMessages()) {
      Severity_type wS = ErrorLog.getSeverity();

      int wRet = ZExceptionDLg::message2BWAdd("FloadfromXmlFileQAc",
              wS > Severity_Warning?ZS_ERROR:ZS_WARNING,
              wS,
              ErrorLog.allLoggedToString(),
              "Stop","Continue",
              "Some warning messages have been issued during load of xml definition from file %s",XmlDictionaryFile.toCChar());
      if (wRet==QDialog::Rejected)
              return;
      }

  renewDicFile(wMasterDic);

  wMsg.sprintf("Xml file %s has been successfully loaded.",XmlDictionaryFile.toCChar());
  statusBar()->showMessage(QObject::tr(wMsg.toCChar(),"DicEdit"),cst_MessageDuration);

  displayZMFDictionary(*DictionaryFile);
  DictionaryChanged=false;

  ui->SourceLBl->setText("Xml file");
  ui->SourceURILBl->setText(XmlDictionaryFile.toCChar());

  return;
  } // FloadfromXmlFileQAc


  if  (pAction==FwritetoDicQAc) {
    if (fieldTBv->ItemModel->rowCount()==0) {
      ZExceptionDLg::adhocMessage("Save",Severity_Error,&ErrorLog,nullptr,"Nothing to write");
      return;
    }
    if (saveOrCreateDictionaryFile()!=ZS_SUCCESS)
      ZExceptionDLg::displayLast();

    return;
    } // FwritetoDicQAc


  if  (pAction==FloadfromDicQAc)
    {
    manageDictionaryFiles();
    return;
    } // FloadfromDicQAc

  return;

}//readWriteActionEvent
void
DicEditMWn::statusBarMessage(const char* pFormat,...) {
  if (pFormat==nullptr)
    ui->statusBar->showMessage("<nullptr>");


  utf8VaryingString wStr,wFormat;

  wFormat=QObject::tr(pFormat).toUtf8().data();

  va_list ap;
  va_start(ap, pFormat);

  wStr.vsnprintf(250,wFormat.toCChar(),ap);
  va_end(ap);

  ui->statusBar->showMessage(wStr.toCChar(),cst_MessageDuration);
}

QStandardItem*
DicEditMWn::insertNewKey() {

  if (keyTRv->ItemModel->rowCount()==0)
    return appendNewKey();

  QVariant wV;
  ZDataReference wDRef;
  ZKeyHeaderRow wKHR;

  QModelIndex wIdx = keyTRv->currentIndex();
  if (!wIdx.isValid())
    return nullptr;
  if (wIdx.column()!=0)
    wIdx=wIdx.siblingAtColumn(0);



  if (!keyCreateDLg(wKHR))
    return nullptr;

  /* if current item is dictionary key : insert before current
   * if current item is key field      : get key parent and insert before key parent
   */

  QStandardItem* wCurrent=keyTRv->ItemModel->itemFromIndex(wIdx);
  wV=wCurrent->data(ZQtDataReference);
  if (wV.isNull())
    return nullptr;
  wDRef=wV.value<ZDataReference>();

  QList<QStandardItem*> wKeyRow = createKeyDicRow(wKHR);

  if (wDRef.getZEntity()==ZEntity_KeyDic) {
    keyTRv->ItemModel->insertRow(wCurrent->row());
    statusBarMessage("Inserted key <%s> at row %d",wKHR.DicKeyName.toCChar(),wCurrent->row());
    return wKeyRow[0];
  }
  /* it is a dictionary field : get parent key item and insert before its row */

  QStandardItem* wParentKey=wCurrent->parent();

  keyTRv->ItemModel->insertRow(wParentKey->row());
  statusBarMessage("Inserted key <%s> at row %d",wKHR.DicKeyName.toCChar(),wParentKey->row());
  return wKeyRow[0];
} // keyInsert
#ifdef __COMMENT__
bool
DicEdit::_keyInsert (const QModelIndex& pIdx,ZKeyHeaderRow*  wKHR) {
  QVariant wV;
  ZDataReference  wDRef;
//  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;
  /* define key index (row and array rank are same) to insert before */

  if ((keyTRv->ItemModel->rowCount()==0)||(!pIdx.isValid())) {
    return _keyAppend(wKHR);
  }

  /* find key item to insert before */
  QStandardItem* wKeyItem=keyTRv->ItemModel->item(pIdx.row(),0);
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.getZEntity()==ZEntity_KeyField) {
    wKeyItem=wKeyItem->parent();  /* jump to key item */
  } else if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    statusBarMessage("DicEdit::_keyInsert-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toCChar());
    return false;
  }
  /* create and insert key row */
  wKHR=new ZKeyHeaderRow;
  QList<QStandardItem*> wKeyRow = createKeyDicRow(wKHR);
  keyTRv->ItemModel->insertRow(wKeyItem->row(),wKeyRow);

  /* insert whole key data within dictionary */
 // long wKeyRank=DictionaryFile->KeyDic.insert(pKey,wKeyItem->row());

/* dictionary is not updated only screen displayed info is used */
  long wKeyRank = wKeyItem->row();

  /* create and link infradata to key item */
  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyDic),wKeyRank);
  wV.setValue(wDRef);
  wKeyRow[0]->setData(wV,ZQtDataReference);


  /* create and insert key field rows individually */
  QList<QStandardItem*> wKeyFieldRow;
  for (long wi=0; wi < wKHR->count();wi++) {
    wKeyFieldRow.clear();
    /* create data to stick to field row */
    wKFR = new ZKeyFieldRow(pKey->Tab(wi));
//    wKFR->set(pKey->Tab(wi));
    /* create row from this data */
    wKeyFieldRow=createKeyFieldRow(*wKFR);
    /* stick this data as infradata to field row */
    wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyField),wi);
    wDRef.setPtr(wKFR );
    wV.setValue(wDRef);
    wKeyFieldRow[0]->setData(wV,ZQtDataReference);
    /* append row to mother key row */
    wKeyRow[0]->appendRow(wKeyFieldRow);
  }// for


  return true; /* done */

} // _keyInsert

#endif

QStandardItem*
DicEditMWn::appendNewKey () {
  ZKeyHeaderRow wNewKey;
  if (!keyCreateDLg(wNewKey))
    return nullptr;

  QList<QStandardItem *> wKeyRow = createKeyDicRow(wNewKey);
  keyTRv->ItemModel->appendRow(wKeyRow);
  DictionaryChanged=true;
  return wKeyRow[0];

}//appendNewKey


void
DicEditMWn::keyChange(QModelIndex &pIdx) {
  QVariant wV;
  ZDataReference wDRef;
  ZKeyHeaderRow*  wKHR=nullptr;
  QModelIndex wIdx;
  if (pIdx.column()!=0) {
    wIdx=pIdx.siblingAtColumn(0);
  }
  else
    wIdx=pIdx;

//  QStandardItem* wKeyItem=keyTRv->ItemModel->itemFromIndex(pIdx);
  wV=wIdx.data(ZQtDataReference);
  if (wV.isNull()) {
    statusBarMessage("DicEditMWn::keyChange-E-INVDATA No infradata found for key at row <%d> ",pIdx.row());
    return;
  }
  wDRef = wV.value<ZDataReference>();

  if (wDRef.getZEntity()==ZEntity_KeyField) {
    wIdx=wIdx.parent();  /* if field, jump to key item */
  } else if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    statusBarMessage("DicEdit::_keyCopy-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toCChar());
    return ;
  } /*else {
    wIdx = pIdx; // index points to a key item (at column 0)
  }*/
  /* extract infradata again - now it has been validated keyItem points to the correct item */
  wV=wIdx.data(ZQtDataReference);
  if (wV.isNull()) {
    fprintf(stderr,"DicEditMWn::keyChange-E-INVDATA Invalid infra data for key at row %d (QVariant)\n",wIdx.row());
    return;
  }
  wDRef = wV.value<ZDataReference>();
  if (wDRef.isInvalid()) {
    fprintf(stderr,"DicEditMWn::keyChange-E-INVDATA Invalid infra data for key at row %d (ZDataReference)\n",wIdx.row());
    return;
  }
  wKHR=wDRef.getPtr<ZKeyHeaderRow>();
//  ZKeyHeaderRow wKeyHeaderRow(wKHR);

  ZKeyDLg* wKeyDLg=new ZKeyDLg(this);
  wKeyDLg->setWindowTitle("Key");

  wKeyDLg->set(wKHR);

  ZKeyHeaderRow wKeyHeaderRow;
  while (true) {
    int wRet=wKeyDLg->exec();
    if (wRet==QDialog::Rejected) {
      wKeyDLg->deleteLater();
      return ;
    }

    wKeyHeaderRow._copyFrom(wKeyDLg->get());
    /* key name control only if key name has changed */
    if (wKeyHeaderRow.DicKeyName==wKHR->DicKeyName)
      break;
    int wRow=0;
    if ((wRow=searchForKeyName(wKeyHeaderRow.DicKeyName))<0){
      /* second control case regardless */
      if ((wRow=searchForKeyNameCase(wKeyHeaderRow.DicKeyName)) < 0)
              break;
      wRet=ZExceptionDLg::adhocMessage2B("New key",Severity_Error,"Force","Change",
                                           nullptr,nullptr,
                                           "A key with different name but with only uppercase/lowercase difference(s) exists at row %d.\n"
                                           "It is highly recommended that key names differs drastically.\n"
                                           "Keep anyway this name (Force) or change its spelling (Change).");
      if (wRet==QDialog::Rejected)
              break;
      continue;
    }

    wRet=ZExceptionDLg::adhocMessage2B("New key",Severity_Error,"Give up","Change",
                                         nullptr,nullptr,
                                         "A key with same name exists at row %d.\n"
                                         "This is forbidden and may induce malfunctions.\n"
                                         "Please change." , wRow);
    if (wRet==QDialog::Rejected) {
      wKeyDLg->deleteLater();
      return ;
    }
  }// while true

  wKeyDLg->deleteLater();

  /* replace key infra data */
  wKHR->_copyFrom(wKeyHeaderRow); /* get back modified data and by the way update infradata */

  /* change rows content */

  utf8VaryingString wStr;
  int wRow=wIdx.row();
  keyTRv->ItemModel->item(wRow,cst_KeyNameColumn)->setText(wKHR->DicKeyName.toCChar());

  wStr.sprintf("%ld",wKHR->KeyGuessedSize);
  keyTRv->ItemModel->item(wRow,cst_KeyUSizeColumn)->setText(wStr.toCChar());

  QVariant wV1 = keyTRv->ItemModel->item(wRow,0)->data(ZQtDataReference);
  ZDataReference wDRef1 = wV1.value<ZDataReference>();

  ZKeyHeaderRow*  wKHR1 = wDRef1.getPtr<ZKeyHeaderRow>();

  keyTRv->ItemModel->item(wRow,cst_DuplicateColumn)->setText(decode_ZST(wKHR->Duplicates));

  if (wKHR->ToolTip.isEmpty())
    keyTRv->ItemModel->item(wRow,cst_TooltipColumn)->setText(" ");
  else {
    keyTRv->ItemModel->item(wRow,cst_TooltipColumn)->setText(wKHR->ToolTip.toCChar());
    keyTRv->ItemModel->item(wRow,cst_TooltipColumn)->setTextAlignment(Qt::AlignLeft);
  }
  return ;
}//keyChange


/**
 * @brief DicEdit::_keyDelete deletes key pointed by pIdx :
 *  - delete key item row and deletes infradata associated to it
 *  - deletes all children field rows and infradata associated with QStandardItem at column 0 for children fields
 *  - release associated resources
 */
bool
DicEditMWn::_keyDelete (const QModelIndex& pIdx) {
  QVariant wV;
  ZDataReference  wDRef , wFDRef;
  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;

  /* define key index (row and array rank are same) to insert before */

  if (!pIdx.isValid()) {
    statusBarMessage("DicEdit::_keyDelete-E-INVIDX Invalid row index.");
    return false;
  }

  QStandardItem* wKeyItem=keyTRv->ItemModel->item(pIdx.row(),0);
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_KeyDic){
    statusBarMessage("DicEdit::_keyDelete-E-INVENT Invalid entity<%s> while trying to delete a key.",decode_ZEntity(wDRef.getZEntity()).toCChar());
    return  false;
  }

  wKHR=wDRef.getPtr<ZKeyHeaderRow>();

  /* delete infradata for each field */
  QStandardItem* wFieldItem=nullptr;
  for (int wi=0;wi < wKeyItem->rowCount();wi++)  {
    wFieldItem=wKeyItem->child(wi,0);
    wV=wFieldItem->data(ZQtDataReference);
    if (!wV.isValid())
      abort();
    wFDRef = wV.value<ZDataReference>();
    if (wFDRef.getZEntity()==ZEntity_KeyField) {
      wKFR=wFDRef.getPtr<ZKeyFieldRow>();
      releaseResource(wFDRef.ResourceReference);
    }
    else
      abort();
    delete wKFR;
  }// for

  wKeyItem->removeRows(0,wKeyItem->rowCount());
  releaseResource(wDRef.ResourceReference);
  delete wKHR; /* delete infradata for key row */

  /* dictionary is not updated only screen displayed info is used */

  /* delete dictionary data */
//  DictionaryFile->KeyDic.erase(wKeyItem->row());

  /* remove key row (key is the first child of root element) */

  keyTRv->ItemModel->removeRow(pIdx.row());
  DictionaryChanged=true;
  return true;
} // _keyDelete

/** @brief DicEdit::_keyfieldDelete deletes key field pointed by pIdx :
 *  - delete key field row and deletes infradata associated to it
 *  - recomputes and displays key universal size value and each key field offset within the mother key.
 */
bool
DicEditMWn::_keyfieldDelete (QModelIndex pIdx) {
  QVariant wV;
  ZDataReference  wDRef , wFDRef;
  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;

  /* define key index (row and array rank are same) to insert before */

  if (!pIdx.isValid()) {
    statusBarMessage("DicEdit::_keyDelete-E-INVIDX Invalid row index.");
    return false;
  }
  if (pIdx.column()!=0) {
    pIdx = pIdx.siblingAtColumn(0);
  }

  wV=pIdx.data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  wKFR=wDRef.getPtr<ZKeyFieldRow>();

  if (wDRef.getZEntity()!=ZEntity_KeyField) {
    statusBarMessage("DicEdit::_keyDelete-E-INVTYPE Invalid row type. Expected <ZEntity_KeyField> while got <%s>",wDRef.getZEntity());
    return false;
  }
  /* get to item at column 0 (contains infradata) */
  QStandardItem* wKeyFieldItem=keyTRv->ItemModel->itemFromIndex(pIdx);

  /* delete infradata */
  delete wKFR;

  /* get mother key item infradata */
  QStandardItem* wKeyItem=wKeyFieldItem->parent();

  /* deprecated */
  /* delete key dictionary key field element */
//  DictionaryFile->KeyDic[wKeyItem->row()]->erase(wKeyFieldItem->row());

  /* delete key field row */
  wKeyItem->removeRow(wKeyItem->row());

  /* recompute values for key Universal Size and key field offsets */
  _recomputeKeyValues(wKeyItem);

  return true;  /* done */
} // _keyDelete


/** @brief DicEdit::_recomputeKeyValues  recomputes and displays for each field key offset and key Universal Size for the key
 *            computation is made from infra data objects.
*/
void DicEditMWn::_recomputeKeyValues(QStandardItem* wKeyItem) {

  QVariant wV;
  ZDataReference  wDRef , wFDRef;
  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;

  int wUniversalSize=0, wOffset=0 ;
  QStandardItem* wKField=nullptr;
  QStandardItem* wKFOffset=nullptr;
  QStandardItem* wKUSize=nullptr;
  utf8VaryingString wStr;

  /* get mother key item infradata */
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  wKHR=wDRef.getPtr<ZKeyHeaderRow>();

  if (wKHR->ChangeStatus & KHCHG_GuessedSize) { /* if key guessed size value has been forced */
    return ; /* do not recompute */
  }

  for (int wi = 0 ; wi < wKeyItem->rowCount() ; wi ++ ) {
    wKField=wKeyItem->child(wi,0);
    wV=wKField->data(ZQtDataReference);
    wDRef = wV.value<ZDataReference>();
    wKFR=wDRef.getPtr<ZKeyFieldRow>();
    /* compute values and change infradata offset value */
    wKFR->KeyOffset = wOffset ;
    wOffset += wKFR->UniversalSize;
    wUniversalSize += wKFR->UniversalSize;
    /* display field offset new value */
    wKFOffset=wKeyItem->child(wi,cst_KeyOffsetColumn);  /* get offset item */
    wStr.sprintf("%ld", wKFR->KeyOffset);
    wKFOffset->setText(wStr.toCChar());   /* update displayed value */
  } // for

  /* update infradata */
  wKHR->KeyGuessedSize = wUniversalSize;

  /* update key displayed value */
  /* 'keyTRv->ItemModel->' because key is the uppermost element */
  int wRNb=wKeyItem->row();
  wKUSize = keyTRv->ItemModel->item(wKeyItem->row(),cst_KeyUSizeColumn);
  if (wKUSize==nullptr) {
    abort();
  }

  wStr.sprintf("total size: %d", wUniversalSize);
  wKUSize->setText(wStr.toCChar());   /* update displayed value */

  /* recompute key dictionary values */

/*  DEPRECATED : Only screen values are used. Dictionary is further updated with screentodic()
 *
  DictionaryFile->KeyDic[wKeyItem->row()]->computeKeyOffsets();
  DictionaryFile->KeyDic[wKeyItem->row()]->computeKeyUniversalSize();
*/
  DictionaryChanged=true;
  return ;  /* done */
}// _recomputeKeyValues


bool
DicEditMWn::fieldMoveup() {

  QModelIndex  wIdx= fieldTBv->currentIndex();
  if (!wIdx.isValid()) {
    return false;
  }
  if (wIdx.row()==0) {
    statusBarMessage("cannot move up. Aready at top of table view.");
    return false;
  }
  int wRow=wIdx.row();
  QList<QStandardItem*> wFieldRow=fieldTBv->ItemModel->takeRow(wIdx.row());
  wRow--;
  fieldTBv->ItemModel->insertRow(wRow);
  DictionaryChanged=true;
  return true;
 }//fieldMoveup
 bool
 DicEditMWn::fieldMovedown() {

   QModelIndex  wIdx= fieldTBv->currentIndex();
   if (!wIdx.isValid()) {
     return false;
   }
   if (wIdx.row()==fieldTBv->ItemModel->rowCount()-1) {
     statusBarMessage("cannot move down. Aready at end of table view.");
     return false;
   }
   int wRow=wIdx.row();
   QList<QStandardItem*> wFieldRow=fieldTBv->ItemModel->takeRow(wIdx.row());
   wRow++;
   fieldTBv->ItemModel->insertRow(wRow);
   DictionaryChanged=true;
   return true;
 }//fieldMovedown
bool
DicEditMWn::keyMoveup() {

  QModelIndex  wIdx= keyTRv->currentIndex();
  if (!wIdx.isValid()) {
    return false;
  }
  QVariant wV;
  ZDataReference wDRef;
  ZKeyHeaderRow*  wKHR=nullptr;

  if (wIdx.column()!=0) {
    wIdx=wIdx.siblingAtColumn(0);
  }

  QStandardItem* wKeyItem=keyTRv->ItemModel->itemFromIndex(wIdx);
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.getZEntity()==ZEntity_KeyField) {
    wKeyItem=wKeyItem->parent();  /* if field, jump to key item */
  } else if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    statusBarMessage("DicEdit::_keyCopy-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toCChar());
    return false;
  }

  int wRow = wKeyItem->row();
  if (wRow==0) {
    statusBarMessage("Already at first position ");
    return false;
  }

  QList<QStandardItem*> wKeyRow = keyTRv->ItemModel->takeRow(wRow);
  if (wKeyRow.count()==0)
    return false;
  wRow--;
  keyTRv->ItemModel->insertRow(wRow,wKeyRow);
  DictionaryChanged=true;
  return true;
}//keyMoveup

bool
DicEditMWn::keyMovedown() {

  QModelIndex  wIdx= keyTRv->currentIndex();
  if (!wIdx.isValid()) {
    return false;
  }
  QVariant wV;
  ZDataReference wDRef;
  ZKeyHeaderRow*  wKHR=nullptr;

  if (wIdx.column()!=0) {
    wIdx=wIdx.siblingAtColumn(0);
  }

  QStandardItem* wKeyItem=keyTRv->ItemModel->itemFromIndex(wIdx);
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.getZEntity()==ZEntity_KeyField) {
    wKeyItem=wKeyItem->parent();  /* if field, jump to key item */
  } else if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    statusBarMessage("DicEdit::_keyCopy-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toCChar());
    return false;
  }

  int wRow = wKeyItem->row();
  if (wRow==keyTRv->ItemModel->rowCount()-1) {
    statusBarMessage("Already at last position ");
    return false;
  }

  QList<QStandardItem*> wKeyRow = keyTRv->ItemModel->takeRow(wRow);
  if (wKeyRow.count()==0)
    return false;
  wRow++;
  keyTRv->ItemModel->insertRow(wRow,wKeyRow);
  DictionaryChanged=true;
  return true;
}//keyMovedown
bool
DicEditMWn::keyfieldMoveup() {

  QModelIndex  wIdx= keyTRv->currentIndex();
  if (!wIdx.isValid()) {
    return false;
  }
  QVariant wV;
  ZDataReference wDRef;

  if (wIdx.column()!=0) {
    wIdx=wIdx.siblingAtColumn(0);
  }

  QStandardItem* wKeyfieldItem=keyTRv->ItemModel->itemFromIndex(wIdx);
  wV=wKeyfieldItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.getZEntity()!=ZEntity_KeyField) {
    statusBarMessage("DicEdit::_keyCopy-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toCChar());
    return false;
  }

  QStandardItem* wKeyItem=wKeyfieldItem->parent();

  int wRow = wKeyfieldItem->row();
  if (wRow==0) {
    statusBarMessage("Already at first key field position ");
    return false;
  }

  QList<QStandardItem*> wKeyfieldRow = wKeyItem->takeRow(wRow);
  if (wKeyfieldRow.count()==0)
    return false;
  wRow--;
  wKeyItem->insertRow(wRow,wKeyfieldRow);
  DictionaryChanged=true;
  return true;
}//keyfieldMoveup

bool
DicEditMWn::keyfieldMovedown() {

  QModelIndex  wIdx= keyTRv->currentIndex();
  if (!wIdx.isValid()) {
    return false;
  }
  QVariant wV;
  ZDataReference wDRef;

  if (wIdx.column()!=0) {
    wIdx=wIdx.siblingAtColumn(0);
  }

  QStandardItem* wKeyfieldItem=keyTRv->ItemModel->itemFromIndex(wIdx);
  wV=wKeyfieldItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.getZEntity()!=ZEntity_KeyField) {
    statusBarMessage("DicEdit::_keyCopy-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toCChar());
    return false;
  }

  QStandardItem* wKeyItem=wKeyfieldItem->parent();

  int wRow = wKeyfieldItem->row();
  if (wRow==wKeyItem->rowCount()-1) {
    statusBarMessage("Already at last key field position ");
    return false;
  }

  QList<QStandardItem*> wKeyfieldRow = wKeyItem->takeRow(wRow);
  if (wKeyfieldRow.count()==0)
    return false;
  wRow++;
  wKeyItem->insertRow(wRow,wKeyfieldRow);
  DictionaryChanged=true;
  return true;
}//keyfieldMovedown



void
DicEditMWn::KeyTRvKeyFiltered(int pKey,QKeyEvent* pEvent)
{

  if(pKey == Qt::Key_Escape) {
    Quit();
    return;
  }
  if(pKey == Qt::Key_Insert)
  {
    insertNewKey();
    return;
  }
  if(pKey == Qt::Key_Delete)
  {
    QModelIndex wIdx=keyTRv->currentIndex();
    if (!wIdx.isValid())
      return;
    if (wIdx.column()!=0)
      wIdx=wIdx.siblingAtColumn(0);
    QVariant wV=wIdx.data(ZQtDataReference);
    if (wV.isNull())
      return;
    ZDataReference wDRef = wV.value<ZDataReference>();
    if (wDRef.isInvalid())
      return;

    if (wDRef.getZEntity()==ZEntity_KeyDic) {
      _keyDelete(wIdx);
      return;
    }
    if (wDRef.getZEntity()!=ZEntity_KeyField) {
      return;
    }
    _keyfieldDelete(wIdx);
    return;
  }//Qt::Key_Delete

  return;
}//KeyTReKeyFiltered

void
DicEditMWn::KeyTRvMouseFiltered(int pKey,QMouseEvent* pEvent)
{
  if (pKey==ZEF_DoubleClickLeft) {
    QModelIndex wIdx= keyTRv->currentIndex();

    if(!wIdx.isValid())
      return;

    keyChange(wIdx);
  }

}
/* deleting a field : must care about field used within key
 *  search for all key field item linked with current field item
 */

bool DicEditMWn::fieldDelete()
{
  utf8VaryingString wStr;
  QModelIndex pIdx=fieldTBv->currentIndex();
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }
  if (pIdx.column()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }
  QStandardItem* wFieldItem = fieldTBv->ItemModel->itemFromIndex(pIdx);
//  QStandardItem* wFather = wItem->parent();  /* for a table view this is not really necessary */

  ZFieldDescription* wFD= getItemData<ZFieldDescription>(wFieldItem);

  zbs::ZArray<QStandardItem*> wKeyFieldItemList;
  zbs::ZArray<KeyFieldRes>    wKeyFieldRes;
  KeyFieldRes wKFRes;
  for (long wKeyRow=0 ; wKeyRow < keyTRv->ItemModel->rowCount();wKeyRow++) {

    QStandardItem* wKeyItem = keyTRv->ItemModel->item(wKeyRow,0);
    wKFRes.KeyName=wKeyItem->text().toUtf8().data();

    for (long wKFRow=0;wKFRow < wKeyItem->rowCount();wKFRow++) {
      QStandardItem* wKeyFieldItem=wKeyItem->child(wKFRow,0);
      ZKeyFieldRow* wKFR= getItemData<ZKeyFieldRow>(wKeyFieldItem);
      if (wKFR==nullptr)
        continue;
      if (wKFR->FieldItem==wFieldItem) {
        wKeyFieldItemList.push(wKeyFieldItem);
        wKFRes.KeyFieldRow=wFieldItem;
        wKeyFieldRes.push(wKFRes);
      }
    } // for wKFRow
  }// for wKeyRow

  if (wKeyFieldItemList.count()>0) {
    utf8VaryingString wComplement;

    for (long wi=0 ; wi < wKeyFieldRes.count(); wi++) {
      wComplement.addsprintf("key %s row %ld\n",wKeyFieldRes[wi].KeyName.toCChar(),wKeyFieldRes[wi].KeyFieldRow->row());
    }


    int wRet=ZExceptionDLg::adhocMessage2B("Delete field",Severity_Information,"Do not delete","Delete all",
        nullptr,&wComplement,
        "Field to delete <%s> is used in %ld key(s).\n"
        "    click <More> for details.\n"
        "Removing this field will modify these keys.\n"
        "Proceed anyway <Delete all> or give up <Do not delete>.",
        wFD->getName().toCChar(),wKeyFieldRes.count());
    if (wRet==QDialog::Rejected)
      return false;
  }// if wKeyFieldItemList.count()>0


  /* delete all related key field rows */
  utf8VaryingString wAdd ;
  for (long wi=0;wi < wKeyFieldItemList.count(); wi ++) {
    QStandardItem* wKeyItem = wKeyFieldItemList[wi]->parent();
    ZKeyFieldRow* wKFR= getItemData<ZKeyFieldRow>(wKeyFieldItemList[wi]);
    delete wKFR;
    wKeyItem->removeRow(wKeyFieldItemList[wi]->row());

    /* if key has no more key field row : ask wether to delete key or not */
    if (wKeyItem->rowCount()==0){
      int wRet=ZExceptionDLg::adhocMessage2B("Empty key",Severity_Information,"Do not remove","Remove",
          nullptr,nullptr,
          "Key <%s> has no more fields.\n"
          "Do you wish to remove this key?\n",
          wKeyItem->text().toUtf8().data());
      if (wRet==QDialog::Accepted) {
        ZKeyHeaderRow* wKHR=getItemData<ZKeyHeaderRow>(wKeyItem);
        delete wKHR;
        keyTRv->ItemModel->removeRow(wKeyItem->row());
        continue;
      }
    }// if (wKeyItem->rowCount()==0)

    _recomputeKeyValues(wKeyItem);
  }

  wAdd.sprintf( " %d key(s) has(have) been modified.",wKeyFieldItemList.count());


  QVariant wV;
  wV = wFieldItem->data(ZQtDataReference);
  if (!wV.isValid()){
    statusBarMessage("DicEdit::_fieldDelete-E-INVENT Cannot get infra data <ZQtDataReference> row #%d.\n",pIdx.row());
    return false;
  }
  ZDataReference wDRef=wV.value<ZDataReference>();
  ZFieldDescription* wF=wDRef.getPtr<ZFieldDescription>();

  utf8VaryingString wMessage ;
  statusBarMessage("Field %d row %d has been suppressed %s",wF->getName().toCChar(),wFieldItem->row(),wAdd.toCChar());

  /* now delete field row */
  fieldTBv->ItemModel->removeRow( wFieldItem->row());
  DictionaryChanged=true;
  return true;
}//_fieldDelete


QStandardItem*
DicEditMWn::fieldChange(QModelIndex pIdx)
{
  QModelIndex wIdx=fieldTBv->currentIndex();
  if (!wIdx.isValid()) {
    statusBarMessage(QObject::tr("No field row selected","DicEdit").toUtf8());
    return nullptr;
  }
  if (wIdx.column()!=0)
    wIdx=wIdx.siblingAtColumn(0);

  QStandardItem* wItem=fieldTBv->ItemModel->itemFromIndex(wIdx);

  ZFieldDescription* wFD=getItemData<ZFieldDescription>(wItem);
  if (wFD==nullptr) {
    ZExceptionDLg::adhocMessage("field change",Severity_Error,&ErrorLog,nullptr,"Invalid infra data for row %d.",wIdx.row());
    return nullptr;
  }

  return fieldChangeDLg(wItem);

}//DicEdit::fieldChange



bool controlField(ZFieldDescription& pField)
{
  if (pField.getName()==__NEW_FIELDNAME__)
    {
    ZExceptionDLg::message("DicEdit::controlField",ZS_INVNAME,Severity_Error,
        "Field name <%s> is invalid. Please enter a valid field name.",pField.getName().toCChar());
    return false;
    }
  if (pField.ZType==ZType_Unknown)
    {
    ZExceptionDLg::message("DicEdit::controlField",ZS_INVTYPE,Severity_Error,
        "Field type 0x%08X <%s> is invalid. Please enter a valid field type.",pField.ZType,decode_ZType(pField.ZType));
    return false;
    }
  if (pField.HeaderSize!=getURFHeaderSize(pField.ZType))
    {
      int wRet=ZExceptionDLg::message2B("DicEdit::controlField",ZS_INVVALUE,Severity_Warning, "Force","Modify",
        "Field type 0x%08X <%s> has a specified header size of <%ld> while you entered <%ld>.\n"
        " Please confirm <Force> or set to standard value <Modify>.",
        pField.ZType,decode_ZType(pField.ZType),
        getURFHeaderSize(pField.ZType),pField.HeaderSize);
      if (wRet==QDialog::Rejected) {
        pField.HeaderSize = getURFHeaderSize(pField.ZType);
        utf8VaryingString wStr;
//        wStr.sprintf("%ld",Field.HeaderSize);
//        ui->HeaderSizeLEd->setText(wStr.toCChar());
        return false;
      }
    return true;
    }
  return true;
}

void processItem(QStandardItem* pItem, long pDataRank)
{
  ZDataReference wDRef;
  QVariant wV;
  if (pItem==nullptr)
    return ;

  wV=pItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.DataRank >= pDataRank)
  {
    wDRef.DataRank++;
    wV.setValue(wDRef);
    pItem->setData(wV,ZQtDataReference);
  }
}

bool DicEditMWn::fieldAppend() {

  ZFieldDescription wFDesc;
  if (!fieldCreateDLg(wFDesc))
    return false;
  return _fieldAppend(wFDesc);

}//fieldAppend

bool DicEditMWn::fieldCreateDLg(ZFieldDescription &pNewField) {
  ZFieldDLg* wFieldDLg= new ZFieldDLg(this);
//  if (FieldDLg==nullptr)
//    FieldDLg=new ZFieldDLg(this);

  wFieldDLg->setCreate();
  while (true) {
    int wRet=wFieldDLg->exec();
    if (wRet==QDialog::Rejected) {
      wFieldDLg->deleteLater();
      return false;
    }
    pNewField=wFieldDLg->getFieldDescription();
    int wRow=0;
    if ((wRow=searchForFieldName(pNewField.getName())) < 0){
      /* second control case regardless */
      if ((wRow=searchForFieldNameCase(pNewField.getName())) < 0)
        break;
      wRet=ZExceptionDLg::adhocMessage2B("Field creation",Severity_Error,"Force","Change",
          nullptr,nullptr,
          "A field with different name but with only uppercase/lowercase difference(s) exists at row %d.\n"
          "It is highly recommended that field names differs drastically.\n"
          "Keep anyway this name (Force) or change its spelling (Change).");
      if (wRet==QDialog::Rejected)
        break;
      continue;
    }
    wRet=ZExceptionDLg::adhocMessage2B("Field creation",Severity_Error,"Give up","Change",
        nullptr,nullptr,
        "A field with same name exists at row %d.\n"
        "This is forbidden and may induce malfunctions.\n"
        "Please change.",wRow);
    if (wRet==QDialog::Rejected){
       wFieldDLg->deleteLater();
      return false;
    }
  }// while true
  wFieldDLg->deleteLater();
  return true;
}//fieldCreateDLg

QStandardItem* DicEditMWn::fieldChangeDLg( QStandardItem* pFieldItem) {

  ZFieldDLg* wFieldDLg= new ZFieldDLg(this);
//  if (FieldDLg==nullptr)
//    FieldDLg=new ZFieldDLg(this);
  ZFieldDescription* wFieldIn=getItemData<ZFieldDescription>(pFieldItem);
  if (wFieldIn==nullptr) {
    ZExceptionDLg::adhocMessage("field change",Severity_Error,&ErrorLog,nullptr,"Invalid infra data for row %d.",pFieldItem->row());
    return nullptr;
  }
  ZFieldDescription wField;
  wFieldDLg->setup(*wFieldIn);
  while (true) {
    int wRet=wFieldDLg->exec();
    if (wRet==QDialog::Rejected) {
      wFieldDLg->deleteLater();
      return nullptr;
    }
    wField=wFieldDLg->getFieldDescription();
    if (wField.getName() == wFieldIn->getName())  /* if field name stays the same, no control */
      break;
    int wRow=0;
    if ((wRow=searchForFieldName(wField.getName())) < 0) {
      /* second control case regardless */
      if ((wRow=searchForFieldNameCase(wField.getName())) < 0)
        break;
      wRet=ZExceptionDLg::adhocMessage2B("Field creation",Severity_Error,"Force","Change",
          nullptr,nullptr,
          "A field with different name but with only uppercase/lowercase difference(s) exists at row %d.\n"
          "It is highly recommended that field names differs drastically.\n"
          "Keep anyway this name (Force) or change its spelling (Change).");
      if (wRet==QDialog::Rejected)
        break;
      continue;
    }
    wRet=ZExceptionDLg::adhocMessage2B("Field creation",Severity_Error,"Give up","Change",
        nullptr,nullptr,
        "A field with same name exists at row %d.\n"
        "This is forbidden and may induce malfunctions.\n"
        "Please change.");
    if (wRet==QDialog::Rejected) {
      wFieldDLg->deleteLater();
      return nullptr;
    }
  }// while true

  zbs::ZArray<QStandardItem*> wKeyFieldItemList;
  zbs::ZArray<KeyFieldRes>    wKeyFieldRes;

  if ( wField.Hash != wFieldIn->Hash) {
    KeyFieldRes wKFRes;
    for (long wKeyRow=0 ; wKeyRow < keyTRv->ItemModel->rowCount();wKeyRow++) {

      QStandardItem* wKeyItem = keyTRv->ItemModel->item(wKeyRow,0);
      wKFRes.KeyName=wKeyItem->text().toUtf8().data();

      for (long wKFRow=0;wKFRow < wKeyItem->rowCount();wKFRow++) {
        QStandardItem* wKeyFieldItem=wKeyItem->child(wKFRow,0);
        ZKeyFieldRow* wKFR= getItemData<ZKeyFieldRow>(wKeyFieldItem);
        if (wKFR==nullptr)
          continue;
        if (wKFR->FieldItem==pFieldItem) {
          wKeyFieldItemList.push(wKeyFieldItem);
          wKFRes.KeyFieldRow=pFieldItem;
          wKeyFieldRes.push(wKFRes);
        }
      } // for wKFRow
    }// for wKeyRow
  }
    if (wKeyFieldItemList.count()>0) {
      utf8VaryingString wComplement;

      for (long wi=0 ; wi < wKeyFieldRes.count(); wi++) {
        wComplement.addsprintf("key %s row %ld\n",wKeyFieldRes[wi].KeyName.toCChar(),wKeyFieldRes[wi].KeyFieldRow->row());
      }

      int wRet=ZExceptionDLg::adhocMessage2B("Change field",Severity_Information,"Do not change","Change all",
          nullptr,&wComplement,
          "Changed field <%s> is used in %ld key(s).\n"
          "    see complement for details.\n"
          "Changing this field will modify these keys.\n"
          "Proceed  <Change all> or give up <Do not change>.",
          wField.getName().toCChar(),wKeyFieldItemList.count());
      if (wRet==QDialog::Rejected)
        return nullptr;
  } // if (wKeyFieldItemList.count()>0)

  /* first : change field */

  int wRow=pFieldItem->row();

//  QList<QStandardItem*> wItemRow = createFieldRowFromField(&pField);
  utf8String wStr;
  QList<QStandardItem*> wFieldRow;
  QVariant wV;
  ZDataReference wDRef(ZLayout_FieldTBv,getNewResource(ZEntity_DicField),0);

  wDRef.setPtr<ZFieldDescription>(new ZFieldDescription(wField));

  wV.setValue(wDRef);

  wFieldRow << new QStandardItem(wField.getName().toCChar());
  wFieldRow[0]->setData(wV,ZQtDataReference);

  wStr.sprintf("0x%08X",wField.ZType);
  wFieldRow << new QStandardItem( wStr.toCChar());

  wFieldRow << new QStandardItem( decode_ZType(wField.ZType));

  wStr.sprintf("%d",wField.Capacity);
  wFieldRow << new QStandardItem( wStr.toCChar());

  wStr.sprintf("%ld",wField.HeaderSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());

  wStr.sprintf("%ld",wField.UniversalSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());

  wStr.sprintf("%ld",wField.NaturalSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());

  wFieldRow <<  new QStandardItem( wField.KeyEligible?"Yes":"No");

  if (wField.ToolTip.isEmpty())
    wFieldRow <<  new QStandardItem( "" );
  else
    wFieldRow <<  new QStandardItem( wField.ToolTip.toCChar());

    wFieldRow <<  new QStandardItem( wField.Hash.toHexa().toCChar());


  QStandardItem* wReplace=fieldTBv->ItemModel->item(wRow,0);

//  QVariant wV = wReplace->data(ZQtDataReference);
//  ZDataReference wDRef = wV.value<ZDataReference>();

  wV = wReplace->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  ZFieldDescription* wFD=wDRef.getPtr<ZFieldDescription>();
  delete wFD;

  fieldTBv->ItemModel->removeRow(wRow);

//  wRootItem->insertRow(wRow,wItemRow);
  fieldTBv->ItemModel->insertRow(wRow,wFieldRow);

  for (int wi=0;wi < fieldTBv->ItemModel->columnCount();wi++)
    fieldTBv->resizeColumnToContents(wi);
  for (int wi=0;wi < fieldTBv->ItemModel->rowCount();wi++)
    fieldTBv->resizeRowToContents(wi);

//  QStandardItem* wNewFieldItem=setFieldRowFromField(fieldTBv->ItemModel,pFieldItem->row(),wField);


  /* second :modify all keys impacted by field change */

  utf8VaryingString wAdd ;
  for (long wi=0;wi < wKeyFieldItemList.count(); wi ++) {
    utf8VaryingString wStr;
    QStandardItem* wKeyItem = wKeyFieldItemList[wi]->parent();
    int wKeyFieldRow=wKeyFieldItemList[wi]->row();

    QVariant wV=wKeyFieldItemList[wi]->data(ZQtDataReference);
    ZDataReference wDRef=wV.value<ZDataReference>();
    ZKeyFieldRow wKFRNew(pFieldItem);


    ZKeyFieldRow* wKFR= wDRef.getPtr<ZKeyFieldRow>();
    wKFR->_copyFrom(wKFRNew);

    wKeyItem->child(wKeyFieldRow,0)->setText(wKFRNew.Name.toCChar());

    wKeyItem->child(wKeyFieldRow,1)->setText(decode_ZType (wKFRNew.ZType));

    wStr.sprintf("%d",wKFRNew.KeyOffset);
    wKeyItem->child(wKeyFieldRow,2)->setText(wStr.toCChar());

    wStr.sprintf("%ld",wKFRNew.UniversalSize);
    wKeyItem->child(wKeyFieldRow,3)->setText(wStr.toCChar());

    wKeyItem->child(wKeyFieldRow,4)->setText(wKFRNew.Hash.toHexa().toCChar());
    _recomputeKeyValues(wKeyItem);
  }// for

  if (wKeyFieldItemList.count())
    statusBarMessage( "field %s changed. %d key(s) has(have) been modified.",wField.getName().toCChar(),wKeyFieldItemList.count());
  else
    statusBarMessage( "field %s changed. No key modified.",wField.getName().toCChar());

  wFieldDLg->deleteLater();
/*
  fprintf (stdout,"Item text <");
  for (int wi=0;wi<wFieldRow.count();wi++) {
    fprintf (stdout," %s ",wFieldRow[wi]->text().toUtf8().data());
  }
  fprintf (stdout,">\n");
  std::cout.flush();

//  QVariant wV;
  fprintf (stdout,"Item display role <");
  for (int wi=0;wi<wFieldRow.count();wi++) {
    wV=wFieldRow[wi]->data(Qt::DisplayRole);
    fprintf (stdout," %s ",wV.value<QString>().toUtf8().data());
  }
  fprintf (stdout,"\n");
  std::cout.flush();
*/
  DictionaryChanged = true;
  return wFieldRow[0];
}//fieldChangeDLg

bool
DicEditMWn::keyCreateDLg(ZKeyHeaderRow &pNewKey) {

  ZKeyDLg* wKeyDLg=new ZKeyDLg(this);
  wKeyDLg->setWindowTitle("Key");

  wKeyDLg->setCreate();

  while (true) {
    int wRet=wKeyDLg->exec();
    if (wRet==QDialog::Rejected) {
      wKeyDLg->deleteLater();
      return false;
    }

    pNewKey = wKeyDLg->get();

    /* check key name */
    int wRow=0;
    if ((wRow=searchForKeyName(pNewKey.DicKeyName))<0){
      /* second control case regardless */
      if ((wRow=searchForKeyNameCase(pNewKey.DicKeyName)) < 0)
        break;
      wRet=ZExceptionDLg::adhocMessage2B("New key",Severity_Error,"Force","Change",
          nullptr,nullptr,
          "A key with different name but with only uppercase/lowercase difference(s) exists at row %d.\n"
          "It is highly recommended that key names differs drastically.\n"
          "Keep anyway this name (Force) or change its spelling (Change).");
      if (wRet==QDialog::Rejected)
        break;
      continue;
    }

    wRet=ZExceptionDLg::adhocMessage2B("New key",Severity_Error,"Give up","Change",
        nullptr,nullptr,
        "A key with same name exists at row %d.\n"
        "This is forbidden and may induce malfunctions.\n"
        "Please change." , wRow);
    if (wRet==QDialog::Rejected)
      return false;
  }// while true

  wKeyDLg->deleteLater();
  return true;
}//keyCreateDLg
#ifdef __COMMENT__
bool
DicEditMWn::keyChangeDLg(ZKeyHeaderRow &pKey) {

  ZKeyHeaderRow wKey;
  ZKeyDLg* wKeyDLg=new ZKeyDLg(this);
  wKeyDLg->setWindowTitle("Key");

  wKeyDLg->set(&pKey);

  while (true) {
    int wRet=wKeyDLg->exec();
    if (wRet==QDialog::Rejected) {
      wKeyDLg->deleteLater();
      return false;
    }

    wKey = wKeyDLg->get();

    /* key name control only if key name has changed */
    if (wKey.DicKeyName==pKey.DicKeyName)
      break;
    int wRow=0;
    if ((wRow=searchForKeyName(wKey.DicKeyName))<0){
      /* second control case regardless */
      if ((wRow=searchForKeyNameCase(wKey.DicKeyName)) < 0)
        break;
      wRet=ZExceptionDLg::adhocMessage2B("New key",Severity_Error,"Force","Change",
          nullptr,nullptr,
          "A key with different name but with only uppercase/lowercase difference(s) exists at row %d.\n"
          "It is highly recommended that key names differs drastically.\n"
          "Keep anyway this name (Force) or change its spelling (Change).");
      if (wRet==QDialog::Rejected)
        break;
      continue;
    }

    wRet=ZExceptionDLg::adhocMessage2B("New key",Severity_Error,"Give up","Change",
        nullptr,nullptr,
        "A key with same name exists at row %d.\n"
        "This is forbidden and may induce malfunctions.\n"
        "Please change." , wRow);
    if (wRet==QDialog::Rejected)
      return false;
  }// while true

  pKey=wKey;
  wKeyDLg->deleteLater();
  return true;
}//keyChangeDLg
#endif // __COMMENT__


bool DicEditMWn::fieldInsertNewBefore(QModelIndex pIdx) {
  ZFieldDescription wFDesc;

  if (!fieldCreateDLg(wFDesc))
    return false;

  return _fieldInsertNewBefore(pIdx,wFDesc);

}//fieldInsertNewBefore

bool DicEditMWn::_fieldAppend(ZFieldDescription &wFDesc) {
  QVariant wV;
  ZDataReference wDRef;
  QList<QStandardItem*> wRow = createFieldRowFromField(&wFDesc);

//  wRIdx=DictionaryFile->push(wFDesc);

  fieldTBv->ItemModel->appendRow(wRow);

  long wRIdx=fieldTBv->ItemModel->rowCount()-1;

  wDRef.set(ZLayout_FieldTBv,getNewResource(ZEntity_DicField),wRIdx);
  wDRef.setPtr<ZFieldDescription>(new ZFieldDescription(wFDesc));
  wDRef.setDataRank(wRIdx);
  wV.setValue(wDRef);

  wRow[0]->setData(wV,ZQtDataReference);

  fieldTBv->resizeRowsToContents();
  fieldTBv->resizeColumnsToContents();
  DictionaryChanged=true;
  return true;
}//_fieldAppend


bool DicEditMWn::fieldInsertAfterFromPinboard(QModelIndex pIdx)
{
  if (Pinboard.isEmpty()) {
    ui->statusBar->showMessage(QObject::tr("Nothing in pinboard.","DicEdit"),cst_MessageDuration);
    return false;
  }
  if (!pIdx.isValid())
  {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }

  if (Pinboard.getLast()->DRef.getZEntity()!=ZEntity_DicField ) {
    statusBarMessage("DicEdit::fieldInsertFromPinboard-E-INVENT Invalid entity from pinboard <%s> while expecting ZEntity_DicField.",
        decode_ZEntity (Pinboard.getLast()->DRef.getZEntity()).toCChar());
    return false;
  }
  ZFieldDescription* wFD= Pinboard.getLast()->DRef.getPtr<ZFieldDescription>(); /* will be linked to created field row*/

  bool wRet=_fieldInsertAfter(pIdx, *wFD);

  delete wFD;
  Pinboard.pop();

  fieldTBv->resizeColumnsToContents();
  fieldTBv->resizeRowsToContents();

  return wRet;
}//fieldInsertAfterFromPinboard

bool DicEditMWn::_fieldInsertAfter(QModelIndex pIdx, ZFieldDescription &pFDesc) {
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }

  QVariant wV;
  ZDataReference wDRef (ZLayout_FieldTBv,getNewResource(ZEntity_DicField));
  QList<QStandardItem*> wRow ;

  if (pIdx.column()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }

  int wRank= pIdx.row() + 1;

  long wFieldRank;

  wRow = createFieldRowFromField(&pFDesc);

  if (fieldTBv->ItemModel->rowCount() < wRank) {   /* if last row : append */
    fieldTBv->ItemModel->appendRow(wRow);
    /* usage of masterdic is deprecated when working with screen */
//    wFieldRank=DictionaryFile->push(pFDesc);
  }
  else {

    fieldTBv->ItemModel->insertRow(wRank,wRow);
     /* usage of masterdic is deprecated when working with screen */
//    wFieldRank=DictionaryFile->insert(pFDesc,wRank);
  }
  wDRef.setDataRank(wFieldRank);
  wDRef.setPtr<ZFieldDescription>(new ZFieldDescription(pFDesc));
  wV.setValue(wDRef);
  wRow[0]->setData(wV,ZQtDataReference);
  DictionaryChanged=true;

  return true;
}//_fieldInsertAfter

bool DicEditMWn::_fieldInsertNewBefore(QModelIndex pIdx, ZFieldDescription &pFDesc) {
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }
  QVariant wV;
  ZDataReference wDRef (ZLayout_FieldTBv,getNewResource(ZEntity_DicField));
  QList<QStandardItem*> wRow ;

  if (pIdx.column()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }

  int wRank= pIdx.row();

   /* usage of masterdic is deprecated when working with screen */
//  long wFieldRank = DictionaryFile->insert(pFDesc,wRank);

  wRow = createFieldRowFromField(&pFDesc);
  wDRef.setDataRank(0);
  wDRef.setPtr<ZFieldDescription>(new ZFieldDescription(pFDesc));
  wV.setValue(wDRef);
  wRow[0]->setData(wV,ZQtDataReference);

  fieldTBv->ItemModel->insertRow(wRank,wRow);


  fieldTBv->resizeRowsToContents();
  fieldTBv->resizeColumnsToContents();

  return true;
}//_fieldInsertAfter


int DicEditMWn::searchForFieldName (const utf8VaryingString& pName) {
  ZFieldDescription* wFD=nullptr;
  ZDataReference wDRef;
  QVariant wV;
  int wCount= fieldTBv->ItemModel->rowCount();
  int wRow=0;
  for (; wRow < wCount ; wRow++ ) {
    QStandardItem* wItem=fieldTBv->ItemModel->item(wRow,0);
    if (wItem==nullptr)
      continue;
    wV=wItem->data(ZQtDataReference);
    if (wV.isNull())
      continue;
    wDRef=wV.value<ZDataReference>();
    if (wDRef.isInvalid())
      continue;
    wFD=wDRef.getPtr<ZFieldDescription>();
//    if (wFD==nullptr)   // must abort if null
//      continue;
    if (wFD->getName()==pName)
      return wRow;
  }// for
  return -1;
}//searchForFieldName

int DicEditMWn::searchForFieldNameCase (const utf8VaryingString& pName) {
  ZFieldDescription* wFD=nullptr;
  ZDataReference wDRef;
  QVariant wV;
  int wCount= fieldTBv->ItemModel->rowCount();
  int wRow=0;
  for (; wRow < wCount ; wRow++ ) {
    QStandardItem* wItem=fieldTBv->ItemModel->item(wRow,0);
    if (wItem==nullptr)
      continue;
    wV=wItem->data(ZQtDataReference);
    if (wV.isNull())
      continue;
    wDRef=wV.value<ZDataReference>();
    if (wDRef.isInvalid())
      continue;
    wFD=wDRef.getPtr<ZFieldDescription>();
    //    if (wFD==nullptr)   // must abort if null
    //      continue;
    if (wFD->getName().isEqualCase( pName))
      return wRow;
  }// for
  return -1;
}//searchForFieldNameCase


int DicEditMWn::searchForKeyName (const utf8VaryingString& pName) {
  ZKeyHeaderRow* wKHR=nullptr;
  ZDataReference wDRef;
  QVariant wV;
  int wCount= keyTRv->ItemModel->rowCount();
  int wRow=0;
  for (; wRow < wCount ; wRow++ ) {
    QStandardItem* wItem=keyTRv->ItemModel->item(wRow,0);
    if (wItem==nullptr)
      continue;
    wV=wItem->data(ZQtDataReference);
    if (wV.isNull())
      continue;
    wDRef=wV.value<ZDataReference>();
    if (wDRef.isInvalid())
      continue;
    wKHR=wDRef.getPtr<ZKeyHeaderRow>();
    //    if (wFD==nullptr)   // must abort if null
    //      continue;
    if (wKHR->DicKeyName.isEqualCase( pName))
      return wRow;
  }// for
  return -1;
}//searchForKeydName


int
DicEditMWn::searchForKeyNameCase (const utf8VaryingString& pName) {
  ZKeyHeaderRow* wKHR=nullptr;
  ZDataReference wDRef;
  QVariant wV;
  int wCount= keyTRv->ItemModel->rowCount();
  int wRow=0;
  for (; wRow < wCount ; wRow++ ) {
    QStandardItem* wItem=keyTRv->ItemModel->item(wRow,0);
    if (wItem==nullptr)
      continue;
    wV=wItem->data(ZQtDataReference);
    if (wV.isNull())
      continue;
    wDRef=wV.value<ZDataReference>();
    if (wDRef.isInvalid())
      continue;
    wKHR=wDRef.getPtr<ZKeyHeaderRow>();
    //    if (wFD==nullptr)   // must abort if null
    //      continue;
    if (wKHR->DicKeyName==pName)
      return wRow;
  }// for
  return -1;
}//searchForKeydName

bool DicEditMWn::fieldAppendFromPinboard()
{
  if (Pinboard.isEmpty())
    return false;

  if (Pinboard.getLast()->DRef.getZEntity()!=ZEntity_DicField ) {
    statusBarMessage("DicEdit::fieldInsertFromPinboard-E-INVENT Invalid entity from pinboard <%s> while expecting ZEntity_DicField.",
        decode_ZEntity (Pinboard.getLast()->DRef.getZEntity()).toCChar());
    return false;
  }
  QVariant wV;
  ZDataReference wDRef= Pinboard.getLast()->DRef;
  QList<QStandardItem*> wRow ;

  ZFieldDescription* wFD= Pinboard.getLast()->DRef.getPtr<ZFieldDescription>();

  bool wRet = _fieldAppend(*wFD);

  Pinboard.getLast()->DRef.releasePtr<ZFieldDescription>();

 // delete wFD;
  Pinboard.pop() ;

  fieldTBv->resizeColumnsToContents();
  fieldTBv->resizeRowsToContents();

  return wRet;
}//_fieldAppendFromPinboard

/* replaces the values (displayed value and dictionary value) of a field at model index wIdx */
void DicEditMWn::acceptFieldChange(ZFieldDescription& pField,QModelIndex &wIdx)
{
  utf8String wStr;
//  QModelIndex wIdx=fieldTBv->currentIndex();
  if (!wIdx.isValid()) {
    ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
    return;
  }

  QStandardItem* wItem=fieldTBv->ItemModel->item(wIdx.row(),0);
  QVariant wV=wItem->data(ZQtDataReference);
  if (!wV.isValid())
    return;
  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_DicField)
  {
    fprintf(stderr,"DicEdit::acceptFieldChange-E-INVENT target item has invalid entity : expected ZEntity_DicField.");
    return;
  }

  ZFieldDescription* wField= wDRef.getPtr<ZFieldDescription>();
  wField->_copyFrom(pField);

/* setFieldRowFromField  changes field in table view QStandardItems and NOT in dictionary */

  setFieldRowFromField(fieldTBv->ItemModel,wIdx.row(),pField);

} //acceptFieldChange


void
DicEditMWn::FieldTBvKeyFiltered(int pKey, QKeyEvent *pEvent)
{

  if(pKey == Qt::Key_Escape) {
    Quit();
    return;
  }

  if(pKey == Qt::Key_Insert)
  {
    ZDataReference WR;
    WR.ZLayout = ZLayout_FieldTBv;
    WR.ResourceReference = getNewResource(ZEntity_DicField);
    WR.DataRank=-1;
    QModelIndex wIdx = fieldTBv->currentIndex();
    if (wIdx.isValid())
      fieldInsertNewBefore(wIdx);
//    createEvent(WR);
    return;
  }
  if(pKey == Qt::Key_Delete)
  {
    fieldDelete();
    return;
  }

  return;

}//KeyTReKeyFiltered

void
DicEditMWn::FieldTBvMouseFiltered(int pKey, QMouseEvent *pEvent)
{
  switch (pKey)
  {
  case ZEF_DoubleClickLeft:
  {
    QModelIndex wIdx= fieldTBv->currentIndex();
    if(!wIdx.isValid())
    {
      statusBar()->showMessage(QObject::tr("No row element selected","DicEdit"),cst_MessageDuration);
      return;
    }
    fieldChange(wIdx);
    break;
  }
    //  case ZEF_SingleClickLeft:
  }

  return;
}



ZQTableView* DicEditMWn::setupFieldTBv(ZQTableView* pFieldTBv, bool pColumnAutoAdjust, int pColumns)
{
  int wFilter =  ZEF_Escape | ZEF_CommonControlKeys | ZEF_DragAndDrop ;

  if (pColumnAutoAdjust)
    wFilter |= ZEF_AutoAdjustColumn;
//  GenericTRe = new ZQTreeView((QWidget *)this,ZEF_CommonControlKeys | ZEF_AllowDataEnter );
//  keyTRv = new ZQTreeView(this,ZLayout_Generic, wFilter ); // no data enter

  pFieldTBv->setZLayout(ZLayout_FieldTBv);
  pFieldTBv->setFilterMask(wFilter);

  //  ui->centralwidget->addWidget(GenericTRv);

  pFieldTBv->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );

  pFieldTBv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  pFieldTBv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  pFieldTBv->setDragEnabled(true);
  pFieldTBv->setDropIndicatorShown(true);
  pFieldTBv->setDataEnter(false);
  pFieldTBv->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  //  GenericTRv->ItemModel=new QStandardItemModel(0,GenericModelColumn,this) ;
  pFieldTBv->setModel(new QStandardItemModel(0,pColumns,pFieldTBv));
  pFieldTBv->ItemModel->setSortRole(ZSortRole);     // Key to have a model sorted on numeric data and not editable data


  // registering callback function : this notation allows to be readable
  //
  std::function<void(int,QKeyEvent *)>                        wKeyFilterCBF;
  std::function<void(QContextMenuEvent *)>                    wContextMenuCBF;
  std::function<bool (int,ZDataReference *,ZDataReference *)> wDropEventCBF=nullptr;
  std::function<void ()>                                      wRawStartDrag;
  std::function<bool (QDropEvent *)>                          wRawDropEventCBF;
/*
  wKeyFilterCBF     = std::bind(&DicEdit::FieldTBvKeyFiltered, this,_1,_2);
  wContextMenuCBF   = std::bind(&DicEdit::FieldTBvFlexMenu, this,_1);     // for context menu callback for ZQTreeView
//  wDropEventCBF     = std::bind(&DicEdit::FieldTBvDropEvent, this,_1,_2,_3);     // for drag and drop
  wRawStartDrag     = std::bind(&DicEdit::FieldTBvRawStartDrag, this);
  wRawDropEventCBF  = std::bind(&DicEdit::FieldTBvRawDropEvent, this,_1);
*/
  pFieldTBv->_registerCallBacks(std::bind(&DicEditMWn::FieldTBvKeyFiltered, this,_1,_2),
                      std::bind(&DicEditMWn::FieldTBvMouseFiltered, this,_1,_2),
                      std::bind(&DicEditMWn::FieldTBvFlexMenu, this,_1),
                      nullptr);

  pFieldTBv->setDestructorCallback(std::bind(&DicEditMWn::DestructorCallback, this));

  pFieldTBv->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  pFieldTBv->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  pFieldTBv->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  pFieldTBv->setSupportedDropActions(Qt::CopyAction|Qt::MoveAction);

  pFieldTBv->setRawStartDragCallback(std::bind(&DicEditMWn::FieldTBvRawStartDrag, this));
  pFieldTBv->setRawDropEventCallback(std::bind(&DicEditMWn::FieldTBvRawDropEvent, this,_1));

  return pFieldTBv;
}//setupFieldTBv

void DicEditMWn::DestructorCallback() {
  int wFieldRows=fieldTBv->ItemModel->rowCount();
  QStandardItem* wItem;
  QVariant wV;
  for (int wi=0;wi< wFieldRows;wi++) {
    wItem=fieldTBv->ItemModel->item(wi,0);
    if (wItem==nullptr)
      continue;
    wV = wItem->data(ZQtDataReference);
    if (!wV.isValid())
      continue;
    ZFieldDescription* wFD =(ZFieldDescription*) wV.value<void*>();
    delete wFD;
  }
} // DestructorCallback

ZQTreeView *DicEditMWn::setupKeyTRv(ZQTreeView* pKeyTRv, bool pColumnAutoAdjust, int pColumns)
{
//  FGeneric=true;
  int wFilter =  ZEF_Escape | ZEF_CommonControlKeys | ZEF_DragAndDrop | ZEF_DoubleClick | ZEF_AllowDataEnter;


  if (pColumnAutoAdjust)
    wFilter |= ZEF_AutoAdjustColumn;
//        GenericTRe = new ZQTreeView((QWidget *)this,ZEF_CommonControlKeys | ZEF_AllowDataEnter );
//  pKeyTRv = new ZQTreeView(this,ZLayout_Generic, wFilter ); // no data enter

  pKeyTRv->setFilterMask(wFilter);

  pKeyTRv->setZLayout(ZLayout_KeyTRv);


//  ui->centralwidget->addWidget(GenericTRv);

  pKeyTRv->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );

  pKeyTRv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  pKeyTRv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  pKeyTRv->setDragEnabled(true);
  pKeyTRv->setDropIndicatorShown(true);
  pKeyTRv->setDataEnter(true);
  pKeyTRv->setAcceptDrops(true);


//  GenericTRv->ItemModel=new QStandardItemModel(0,GenericModelColumn,this) ;
  pKeyTRv->setModel(new QStandardItemModel(0,pColumns,pKeyTRv));
  pKeyTRv->ItemModel->setSortRole(ZSortRole);     // Key to have a model sorted on numeric data and not editable data

  // registering callback function : this notation allows to be readable
  //

  pKeyTRv->_register( std::bind(&DicEditMWn::KeyTRvKeyFiltered, this,_1,_2),
                      std::bind(&DicEditMWn::KeyTRvMouseFiltered, this,_1,_2),
                      std::bind(&DicEditMWn::KeyTRvFlexMenu, this,_1),
                      nullptr);

  pKeyTRv->setSortingEnabled(false);  // will be set to true in the end of setup_effective
/* it is EDITABLE "/
//  pKeyTRv->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
//  pKeyTRv->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  pKeyTRv->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  pKeyTRv->setRawStartDragCallback(std::bind(&DicEditMWn::KeyTRvRawStartDrag, this));
  pKeyTRv->setRawDropEventCallback(std::bind(&DicEditMWn::KeyTRvRawDropEvent,this,_1));

  pKeyTRv->setSupportedDropActions(Qt::CopyAction|Qt::MoveAction);



  return pKeyTRv;
  //    PerDomainTRe->expandAll();
}//setupKeyTRv


/* during start drag, only source item reference is copied to pinboard : it is up to drop operation to select the row */
void DicEditMWn::KeyTRvRawStartDrag() {
  QModelIndex wIndex=keyTRv->currentIndex();
  if (!wIndex.isValid())
    return;
  if (wIndex.column()!=0)
    wIndex=wIndex.siblingAtColumn(0);
  QVariant wV;
  ZDataReference wDRef;
  wV=wIndex.data(ZQtDataReference);
  wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()==ZEntity_KeyDic)  {
    ZDataReference wDRefCopy(ZLayout_KeyTRv,getNewResource(ZEntity_KeyDicItem));
    wDRef.setPtr<QStandardItem>(keyTRv->ItemModel->itemFromIndex( wIndex));
  }
  else {
    ZDataReference wDRefCopy(ZLayout_KeyTRv,getNewResource(ZEntity_KeyFieldItem));
    wDRef.setPtr<QStandardItem>(keyTRv->ItemModel->itemFromIndex( wIndex));
  }
  ZPinboardElement wElt;
  wElt.setDataReference(wDRef);
  Pinboard.push(wElt);
  return;
}//KeyTRvRawStartDrag

/* drop event for key tree view : all drop operations are driven thru pinboard stack */

/* paste from pinboard on key tree view
 * and drop on key tree view
 *                             source objects allowed (Pinboard content) :
 *
 *      Object              ZEntity_type          ZDataReference pointer
 *
 * From start drag operation or from copy (menu choice)
 * ----------------------------------------------------
 *
 *  - dictionary field      ZEntity_DicFieldItem  QStandardItem* pointing to fieldTBv item
 *      (start drag from fieldTBv or copy -menu choice from fieldTBv )
 *
 *  - key dictionary item   ZEntity_KeyDicItem    QStandardItem* pointing to key dictionary item (start drag from keyTRv)
 *
 *  - key field item        ZEntity_KeyFieldItem  QStandardItem* pointing to key field item (start drag from keyTRv)
 *
 * From cut operation (menu choice for keyTRv)
 *-------------------
 *  - key dictionary row    ZEntity_KeyDicRow     QList<QStandardItem*>* pointing to key dictionary row
 *
 *  - key field row  ZEntity_KeyFieldRow   QList<QStandardItem*>* pointing to key field row
 *
 *
 */

bool DicEditMWn::KeyTRvRawDropEvent(QDropEvent *pEvent) {
   QModelIndex wIdx = keyTRv->indexAt (pEvent->pos());
   return keyTRvInsertFromPinboard(wIdx);
}// KeyTRvRawDropEvent


bool DicEditMWn::keyTRvInsertFromPinboard (QModelIndex pIdx) {
  ZPinboard* wPinboard=&Pinboard; /* debug */

  if (Pinboard.isEmpty()) {
    statusBar()->showMessage(QObject::tr("No source element.","DicEdit"),cst_MessageDuration);
    return false;
  }

  QModelIndex wIdx;
  if (keyTRv->ItemModel->rowCount()==0) {
    /* no key yet defined :
     * if pinboard content is a field define a new key, then append field
     * if not : reject (only fields are allowed to be appended if no row in keyTRv)
     */
    if (Pinboard.last().getZEntity()!=ZEntity_DicFieldItem) {
      statusBarMessage("Found invalid infra data <%s> ",decode_ZEntity(Pinboard.last().getZEntity()).toCChar());
      return false;
    }
    QStandardItem* wNewKeyItem=nullptr;
    if (!(wNewKeyItem=appendNewKey()))
      return false; /* if not defined or error : return */

    wIdx = keyTRv->ItemModel->index(0,0); /* set index to newly created key */

    QStandardItem* wFieldItem = Pinboard.last().getDRefPtr<QStandardItem>();

    ZKeyFieldRow wKFR;
    wKFR.setFromQItem(wFieldItem);
    QList<QStandardItem*> wKeyFieldItemRow=createKeyFieldRow(wKFR);
    wNewKeyItem->appendRow(wKeyFieldItemRow);

    _recomputeKeyValues(wNewKeyItem);

    keyTRv->expandItem(wNewKeyItem->index());

    for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
      keyTRv->resizeColumnToContents(wi);
    return true;
  }

  wIdx = pIdx;
  if (!wIdx.isValid()) {
    if (Pinboard.last().getZEntity()==ZEntity_DicFieldItem) {
      QStandardItem* wNewKeyItem=nullptr;
      if (!(wNewKeyItem=appendNewKey()))
        return false; /* if not defined or error : return */

      wIdx = keyTRv->ItemModel->index(0,0); /* set index to newly created key */

      QStandardItem* wFieldItem = Pinboard.last().getDRefPtr<QStandardItem>();

      ZKeyFieldRow wKFR;
      wKFR.setFromQItem(wFieldItem);
      QList<QStandardItem*> wKeyFieldItemRow=createKeyFieldRow(wKFR);
      wNewKeyItem->appendRow(wKeyFieldItemRow);
      _recomputeKeyValues(wNewKeyItem);

      keyTRv->expandItem(wNewKeyItem->index());
      for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
        keyTRv->resizeColumnToContents(wi);
      return true;
    }//if (Pinboard.last().getZEntity()==ZEntity_DicFieldItem)

    statusBar()->showMessage(QObject::tr("No target element selected","DicEdit"),cst_MessageDuration);
    return false;
  }

  /* get the drop target entity type : key or key field */
  QStandardItem* wDropTarget = keyTRv->ItemModel->itemFromIndex(wIdx);
  QVariant wV= wDropTarget->data(ZQtDataReference);
  ZDataReference wTargetDRef= wV.value<ZDataReference>();

  /* get the origin of the dragndrop operation */
  switch (Pinboard.getLast()->DRef.getZEntity()) {

    /*--------------- start drag source (either from fieldTBv or keyTRv) -------------------*/

  case ZEntity_KeyFieldItem: {
    /* Comes from a start drag from a field row in fieldTBv
     *  create key field row from key definition
     *  drop target is dictionary key row : append key field row to key row
     *  drop target is dictionary key field : find parent key row, insert insert key field row before current key row
     */

    QStandardItem* wKeyFieldItem=Pinboard.getLast()->getDRefPtr<QStandardItem>();

    QList<QStandardItem*> wKeyFieldRow=  keyTRv->ItemModel->takeRow(wKeyFieldItem->row());

    switch (wTargetDRef.getZEntity()) {
    case ZEntity_KeyDic:{
      wDropTarget->appendRow(wKeyFieldRow);
      _recomputeKeyValues(wDropTarget);
      break;
    }
    case ZEntity_KeyField: {
      QStandardItem* wKeyParent= wDropTarget->parent();
      wKeyParent->insertRow(wDropTarget->row(),wKeyFieldRow);
      _recomputeKeyValues(wKeyParent);
      break;
    }
    }//switch

    keyTRv->expandItem(wDropTarget->index());
    for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
      keyTRv->resizeColumnToContents(wi);

    releaseResource( Pinboard.last().DRef.ResourceReference);
    Pinboard.pop();
    return true;
  }//ZEntity_DicFieldItem

  case ZEntity_KeyDicItem: {
    /* Comes from a start drag from a key dictionary row
     *  cut the key dictionary row using takeRow() [this cuts also all children key field rows]
     *  if drop target is dictionary key row : insert key row before
     *  if drop target is dictionary key field : find parent key row, insert insert key row before
     */

    QStandardItem* wKeyItem=Pinboard.getLast()->getDRefPtr<QStandardItem>();

    QList<QStandardItem*> wKeyRow=  keyTRv->ItemModel->takeRow(wKeyItem->row());

    /* insert row according target type */

    switch (wTargetDRef.getZEntity()) {
    case ZEntity_KeyDic: {
      keyTRv->ItemModel->insertRow(wDropTarget->row(),wKeyRow);
      _recomputeKeyValues(wDropTarget);
      keyTRv->expandItem(wDropTarget->index());
      break;
    }
    case ZEntity_KeyField: {
      QStandardItem* wKeyParent = wDropTarget->parent();
      keyTRv->ItemModel->insertRow(wKeyParent->row(),wKeyRow);
      _recomputeKeyValues(wKeyParent);
      keyTRv->expandItem(wKeyParent->index());
      break;
    }
    }// switch
    releaseResource( Pinboard.last().DRef.ResourceReference);
    Pinboard.pop();

    for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
      keyTRv->resizeColumnToContents(wi);

    return true;
  }//ZEntity_KeyDicItem

  case ZEntity_DicFieldItem: {
    /* Comes from a start drag from a key field row
     *  cut the key field row using takeRow()
     *  drop target is dictionary key row : append key field row to key row
     *  drop target is dictionary key field : find parent key row, insert insert key field row before current key row
     */

    ZKeyFieldRow wKFR;
    QStandardItem* wFieldItem=Pinboard.getLast()->getDRefPtr<QStandardItem>();
    wKFR.setFromQItem(wFieldItem);

    /* what kind is drop target */
    if (wTargetDRef.getZEntity()==ZEntity_KeyDic) {
      wDropTarget->appendRow(createKeyFieldRow(wKFR));/* if drop target is dictionary key row : append to it*/

      _recomputeKeyValues(wDropTarget);

      keyTRv->expandItem(wDropTarget->index());

      for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
        keyTRv->resizeColumnToContents(wi);

      Pinboard.pop();  /* pop() releases ZResource */
      return true;
    }
    if (wTargetDRef.getZEntity()!=ZEntity_KeyField) {
      statusBarMessage("Found invalid infra data <%s> within key tree view item <%s> row %d ",
          decode_ZEntity(wTargetDRef.getZEntity()).toCChar(), wDropTarget->text().toUtf8().data(), wIdx.row());
      return false;
    }
    /* find drop target parent key row */
    QStandardItem* wParentKeyItem=wDropTarget->parent();
    /* create key field row and insert it at drop target row */
    wParentKeyItem->insertRow(wDropTarget->row(),createKeyFieldRow(wKFR));

    _recomputeKeyValues(wParentKeyItem);

    keyTRv->expandItem(wParentKeyItem->index());
    for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
      keyTRv->resizeColumnToContents(wi);

    Pinboard.pop();  /* pop() releases ZResource */
    return true;
    return true;
  }//ZEntity_KeyFieldItem

    /*--------------- cut to pinboard source (menu choice)  : entire rows -------------------*/

  case ZEntity_KeyDicRow: {
    /* Inserting a whole key row :
     *    if drop target is another key row : insert row at target row
     *    if target is key field : find key parent - create key field and insert it at parent row()
     */
    QList<QStandardItem*>* wKeyDicRow=Pinboard.getLast()->DRef.getPtr<QList<QStandardItem*>>();
    if (wTargetDRef.getZEntity()==ZEntity_KeyDic) {
      keyTRv->ItemModel->insertRow(wIdx.row(),*wKeyDicRow);

      _recomputeKeyValues((*wKeyDicRow)[0]);

      for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
        keyTRv->resizeColumnToContents(wi);

      Pinboard.pop();
      return true;
    }
    if (wTargetDRef.getZEntity()!=ZEntity_KeyField) {
      statusBarMessage("Found invalid infra data within key tree view item <%s> row %d ",wDropTarget->text().toUtf8().data(), wIdx.row());
      return false;
    }
    /* here drop target is a key field : find key row father and insert key row before target key */
    QStandardItem* wKeyFather=   wDropTarget->parent();
    keyTRv->ItemModel->insertRow(wKeyFather->row(),*wKeyDicRow);

    _recomputeKeyValues(wKeyFather);

    keyTRv->expandItem(wDropTarget->index());
    for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
      keyTRv->resizeColumnToContents(wi);

    return true;
  }// ZEntity_KeyDicRow

  case ZEntity_KeyFieldRow: {
    /* Inserting a key field row (cut or copied to pinboard) :
     *    if drop target is a key row : append key field row to key item
     *    if drop target is key field : find key parent - insert key field row within parent at target row()
     */
    QList<QStandardItem*>* wKeyFieldRow=Pinboard.getLast()->DRef.getPtr<QList<QStandardItem*>>();
    if (wTargetDRef.getZEntity()==ZEntity_KeyDic) {
      wDropTarget->appendRow(*wKeyFieldRow);

      _recomputeKeyValues(wDropTarget);

      keyTRv->expandItem(wDropTarget->index());
      for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
        keyTRv->resizeColumnToContents(wi);

      Pinboard.pop();
      return true;
    }
    if (wTargetDRef.getZEntity()==ZEntity_KeyField) {
      QStandardItem* wTargetKey= wDropTarget->parent();
      wTargetKey->insertRow(wDropTarget->row(),*wKeyFieldRow);

      _recomputeKeyValues(wTargetKey);

      keyTRv->expandItem(wDropTarget->index());
      for (long wi = 0 ; wi < keyTRv->ItemModel->columnCount() ;wi++)
        keyTRv->resizeColumnToContents(wi);

      Pinboard.pop();
      return true;
    }
    return false;
  } //
  default:
    statusBarMessage("DicEdit::_keyCopy-E-INVTYP Invalid infradata entity type <%s> ",
        decode_ZEntity(Pinboard.getLast()->DRef.getZEntity()).toCChar());
    return false;
  }

  return false;
}

/* during start drag, only source item reference is copied to pinboard : it is up to drop operation to select the row */
void DicEditMWn::FieldTBvRawStartDrag()
{
  QModelIndex wIndex=fieldTBv->currentIndex();
  if (!wIndex.isValid())
    return;
  if (wIndex.column()!=0)
    wIndex=wIndex.siblingAtColumn(0);

  ZDataReference wDRef(ZLayout_FieldTBv,getNewResource(ZEntity_DicFieldItem));
  wDRef.setPtr<QStandardItem>(fieldTBv->ItemModel->itemFromIndex( wIndex));

  ZPinboardElement wElt;
  wElt.setDataReference(wDRef);
  Pinboard.push(wElt);
  return;
}//FieldTBvRawStartDrag

bool DicEditMWn::FieldTBvRawDropEvent(QDropEvent *pEvent)
{
  const QMimeData *wMimeData = pEvent->mimeData();

  /* test case where a file url is dropped onto field table view */

  if (wMimeData->hasUrls()) {
    if (wMimeData->urls().isEmpty()){
      pEvent->ignore(); // error no url while one was expected : meaning external to Qt application
      return false;
    }
    uriString wURI ;
    wURI = wMimeData->urls()[0].toString();
    wURI = wURI.getLocal();
    /* eliminate <file://>   */

    if (rawFields==nullptr) {
      rawFields = new RawFields(this); }
    rawFields->setFile(wURI) ;
    rawFields->showAll();
    rawFields->parse(false);
    return true;
  }//if (wMimeData->hasUrls())

//  QList<QString>FormatList = wMimeData->formats();


  QVariant wV;
  ZDataReference wDRef;

  QModelIndex wIdx = fieldTBv->indexAt (pEvent->pos());

  if (!wIdx.isValid())
    return false;

  if (Pinboard.isEmpty()) {
      ui->statusBar->showMessage(QObject::tr("Nothing in pinboard.","DicEdit"),cst_MessageDuration);
      return false;
  }

  wDRef=Pinboard.last().DRef;

  if (wDRef.getZEntity()!=ZEntity_DicFieldItem) {
    statusBarMessage("Invalid drop source. Expected a field or an include file");
    return false;
  }
  QStandardItem* wFieldItem=Pinboard.last().DRef.getPtr<QStandardItem>();

  if (wIdx.row()==wFieldItem->row()) {
    statusBarMessage("Cannot drop a field onto itself.");
    return false;
  }

  QList<QStandardItem*> wFieldRow= fieldTBv->ItemModel->takeRow(wFieldItem->row());

  fieldTBv->ItemModel->insertRow(wIdx.row(),wFieldRow);


  for (long wi = 0 ; wi < fieldTBv->ItemModel->columnCount() ;wi++)
    fieldTBv->resizeColumnToContents(wi);

  releaseResource(Pinboard.last().DRef.ResourceReference);
  Pinboard.pop();

  return true;
}//FieldTBvRawDropEvent

ZMFDictionary*
DicEditMWn::screenToDic(ZMFDictionary* pZMFDic)
{
  bool wErrored=false;
  ZMFDictionary* wZMFDic = nullptr;
  if (pZMFDic==nullptr)
    wZMFDic=new ZMFDictionary;
  else {
    wZMFDic= new ZMFDictionary (*pZMFDic);
    wZMFDic->clearData();
  }

  ZKeyDictionary* wKeyDic=nullptr;

  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;

  ZDataReference wDRef, wDRefKh, wDRefKf;

  int wFieldRows=fieldTBv->ItemModel->rowCount();
  QStandardItem* wItem;
  QVariant wV;
  for (int wi=0;wi< wFieldRows;wi++) {
    wItem=fieldTBv->ItemModel->item(wi,0);
    wV = wItem->data(ZQtDataReference);
    wDRef=wV.value<ZDataReference>();

    ZFieldDescription* wFD =wDRef.getPtr<ZFieldDescription>();
    wZMFDic->push(*wFD);
    } // for

  int wKeyRows=keyTRv->ItemModel->rowCount();
  QStandardItem* wKeyItem=nullptr;
//  QStandardItem* wDuplicateItem=nullptr;
  for (int wi=0;wi < wKeyRows;wi++) {
    wKeyItem=keyTRv->ItemModel->item(wi,0);
    wV = wKeyItem->data(ZQtDataReference);
    if (!wV.isValid()) {
      abort();
    }
    wDRefKh=wV.value<ZDataReference>();
    if (wDRefKh.isInvalid()) {
      abort();
    }
    wKHR = wDRefKh.getPtr<ZKeyHeaderRow>();

    wKeyDic  = new ZKeyDictionary(wKHR->get(wZMFDic));

    /* key fields */
    for (int wj=0;wj < wKeyItem->rowCount();wj++)
      {
      wItem=wKeyItem->child(wj,0);
      wV = wItem->data(ZQtDataReference);
      wDRefKf=wV.value<ZDataReference>();
      wKFR = wDRefKf.getPtr<ZKeyFieldRow>();

      ZIndexField wIxFld (wKeyDic);
      wIxFld.KeyOffset   = wKFR->KeyOffset;
      wIxFld.Hash   = wKFR->Hash;
      wIxFld.MDicRank=-1;
      long wF=0;
      for (;wF< wZMFDic->count();wF++)
        if (wKFR->Name == wZMFDic->Tab(wF).getName()){
            wIxFld.MDicRank=wF;
            break;
        }
      if (wIxFld.MDicRank<0)
        {
        ErrorLog.errorLog("DicEdit::screenToDic-E-KFNTFND Key field <%s> not found in master dictionary (field name not found). Key field discarded.\n"
                           "                               Dictionary will not be saved.",
                           wKFR->Name.toCChar());
        wErrored=true;
        continue;
        }

        wIxFld.KeyOffset = wKFR->KeyOffset;
      for (int wh=0;wh<cst_md5;wh++)
        wIxFld.Hash.content[wh] = wKFR->Hash.content[wh];

      wKeyDic->push(wIxFld);
      }//for (int wj=0;wj < wKeyItem->rowCount();wj++)

    wZMFDic->KeyDic.push(wKeyDic);  /* then push the whole Key dictionary */
    }//for (int wi=0;wi< wKeyRows;wi++)

  if (wErrored) {
    delete wZMFDic;
    return nullptr;
  }
  wZMFDic->DicName = ui->DicNameLBl->text().toUtf8().data();
  wZMFDic->Version = getVersionNum(ui->VersionLBl->text().toUtf8().data());
  wZMFDic->Active = ui->ActiveCBx->currentIndex()==1 ;


  if (pZMFDic!=nullptr) {
    pZMFDic->_copyFrom(*wZMFDic);
    pZMFDic->setModified();
    delete wZMFDic;
    return pZMFDic;
  }
  wZMFDic->setModified();
  return wZMFDic;
}//screenToDic

void
DicEditMWn::manageDictionaryFiles(){
  utf8VaryingString wStr;
  ZStatus wSt;
  uriString wEnvDir;
  ZDicHeaderList wDicHList;

  if ((DictionaryFile!=nullptr) && (!DictionaryFile->isEmpty())) {
    wStr = DictionaryFile->URIDictionary.getDirectoryPath();
  }
  else
    wStr=GeneralParameters.getWorkDirectory();

  QString wQDicFile=QFileDialog::getOpenFileName(this,"Dictionary Files",wStr.toCChar(),
        "Dictionaries (*.dic);;All (*.*)");
  if (wQDicFile.isEmpty())
      return;

  uriString wSelected = wQDicFile.toUtf8().data();

  if (DictionaryFile!=nullptr) {
    if (DictionaryFile->count()!=0) {
      int wRet= ZExceptionDLg::adhocMessage2B("Dictionary file",Severity_Information,
          "No","Replace",nullptr,nullptr,
          "Replace current dictionary <%s> with loaded dictionary ?",DictionaryFile->DicName.toString());
      if (wRet==QDialog::Rejected)
        return;
    }
  }
  else
      DictionaryFile = new ZDictionaryFile;

  wSt = DictionaryFile->loadDictionary(wSelected,&ErrorLog);
  if (wSt!=ZS_SUCCESS) {
      wStr = ZException.last().formatFullUserMessage();
      ZExceptionDLg::adhocMessage("Dictionary file",Severity_Error,&ErrorLog,
          &wStr,"File %s has been errored while trying to load dictionary.",
          wSelected.toString());
      return ;
  }

  wStr.sprintf("Dictionary %s version %s status %s has been loaded",DictionaryFile->DicName.toCChar(),getVersionStr(DictionaryFile->Version).toCChar(),
          DictionaryFile->Active?"Active":"Not active");
  ui->statusBar->showMessage(wStr.toCChar(),cst_MessageDuration);

  displayZMFDictionary(*DictionaryFile);
  DictionaryChanged=false;

  ui->SourceLBl->setText("Dictionary file");
  ui->SourceURILBl->setText(DictionaryFile->URIDictionary.toCChar());
  XmlDictionaryFile.clear();

  return ;
}


bool
DicEditMWn::saveOrCreateDictionaryFile() {

  ZStatus wSt;
  uriString wXmlDictionary ;

  if (DictionaryFile!=nullptr) {
      wXmlDictionary = DictionaryFile->URIDictionary;
  }
  else {
      wXmlDictionary=GeneralParameters.getWorkDirectory();
      wXmlDictionary.addConditionalDirectoryDelimiter();
  }
  QString wFileName = QFileDialog::getSaveFileName(this, "Dictionary file",
                                                  wXmlDictionary.toCChar(),
                                                  "Dictionary files (*.dic);;All (*.*)");
  if (wFileName.isEmpty()) {
    return false;
  }

  wXmlDictionary = wFileName.toUtf8().data();

  if (wXmlDictionary.getFileExtension().isEmpty()) {
    wXmlDictionary += ".";
    wXmlDictionary += __DICTIONARY_EXTENSION__;
  }

  utf8VaryingString wDicName = ui->DicNameLBl->text().toUtf8().data();
  unsigned long     wVersion = getVersionNum(ui->VersionLBl->text().toUtf8().data());
  bool              wActive=ui->ActiveCBx->currentIndex()==1;

  if (!getDicName(wVersion,wActive,wDicName))/* name meta dictionary and version */
    return false;

  /* update appropriately name and version */

  ui->DicNameLBl->setText(wDicName.toCChar());
  ui->VersionLBl->setText( getVersionStr( wVersion).toCChar());
  ui->ActiveCBx->setCurrentIndex(wActive?1:0);
  /*
  ---------------------------------------------------------------------------------
  dictionary file does not exist         create and don't care active (zinitialize)
  ---------------------------------------------------------------------------------

  dictionary file exits

  open it

  ------saving rules-----------

  Dic content to write is not active :
      [searchAndWrite] search for (DicName , Version) in file :
        not exists -> append
        exists -> replace

  Dic content to write is active :
      search for active in file :
      active not exists :
        [searchAndWrite] search for (DicName , Version) in file :
          not exists -> append
          exists -> replace
      active exists :
        (DicName , Version) are the same : replace.
        (DicName , Version) are not the same :
          found in Dictionary set to not Active, and replace
          [searchAndWrite] search for (DicName , Version) in file :
            not exists -> append
            exists -> replace
*/

  if (DictionaryFile==nullptr)
      DictionaryFile = new ZDictionaryFile;
    else
      DictionaryFile->clear();

  DictionaryFile->URIDictionary = wXmlDictionary;


    if (screenToDic(DictionaryFile)==nullptr) {
      ZExceptionDLg::adhocMessage("Dictionary",Severity_Error,&ErrorLog,nullptr,
                                  "Cannot create dictionary file %s.\n"
                                  "Name <%s> version <%s> status <%s>",
                                  wXmlDictionary.getBasename().toCChar(),
                                  DictionaryFile->DicName.toCChar(),
                                  getVersionStr( DictionaryFile->Version).toCChar(),
                                  DictionaryFile->Active?"Active":"Not active");
      return false;
    }

  DictionaryChanged=false;


  bool wCreatedDic = wXmlDictionary.exists();

  wSt= DictionaryFile->create(wXmlDictionary,true);
  if (wSt != ZS_SUCCESS) {
    ZExceptionDLg::display("Dictionary save",ZException.last());
    return false;
  }
  if (wCreatedDic) {
  ZExceptionDLg::adhocMessage("Dictionary created",Severity_Information,&ErrorLog,nullptr,
      "New dictionary file %s.\n"
      "Created dic <%s> version <%s> status <%s>",
      wXmlDictionary.getBasename().toCChar(),
      DictionaryFile->DicName.toCChar(),
      getVersionStr( DictionaryFile->Version).toCChar(),
      DictionaryFile->Active?"Active":"Not active");
  return true;
  }

  ZExceptionDLg::adhocMessage("Dictionary replaced",Severity_Information,&ErrorLog,nullptr,
      "Saved dictionary file %s.\n"
      "Name <%s> version <%s> status <%s>",
      wXmlDictionary.getBasename().toCChar(),
      DictionaryFile->DicName.toCChar(),
      getVersionStr( DictionaryFile->Version).toCChar(),
      DictionaryFile->Active?"Active":"Not active");
  return true;

}//saveOrCreateDictionaryFile


bool
DicEditMWn::saveToDictionaryFile(uriString& pXmlDictionary) {
  ZStatus wSt;
  if (DictionaryFile==nullptr)
  DictionaryFile = new ZDictionaryFile;
  else
  DictionaryFile->clear();



  if ( screenToDic(DictionaryFile)==nullptr) {
    ZExceptionDLg::adhocMessage("Xml dictionary file",Severity_Error,&ErrorLog,nullptr,
                                  "Cannot create dictionary file %s.\n"
                                  "Created dic <%s> version <%s> status <%s>",
                                  pXmlDictionary.getBasename().toCChar(),
                                  DictionaryFile->DicName.toCChar(),
                                  getVersionStr( DictionaryFile->Version).toCChar(),
                                  DictionaryFile->Active?"Active":"Not active");
    return false;
  }
  DictionaryChanged=false;


  bool wCreatedDic = !pXmlDictionary.exists();
  if (wCreatedDic) {
    wSt= DictionaryFile->create(pXmlDictionary,true);
    if (wSt != ZS_SUCCESS) {
        ZExceptionDLg::display("Create dictionary",ZException.last());
        return false;
    }
    ZExceptionDLg::adhocMessage("Dictionary created",Severity_Information,&ErrorLog,nullptr,
                                  "New dictionary file %s.\n"
                                  "Created dic <%s> version <%s> status <%s>",
                                  pXmlDictionary.getBasename().toCChar(),
                                  DictionaryFile->DicName.toCChar(),
                                  getVersionStr( DictionaryFile->Version).toCChar(),
                                  DictionaryFile->Active?"Active":"Not active");
    return true;
  }
  wSt= DictionaryFile->saveToDicFile(pXmlDictionary,true);
  if (wSt != ZS_SUCCESS) {
    ZExceptionDLg::display("Save dictionary",ZException.last());
    return false;
  }
  ZExceptionDLg::adhocMessage("Dictionary replaced",Severity_Information,&ErrorLog,nullptr,
                              "Saved dictionary file %s.\n"
                              "Name <%s> version <%s> status <%s>",
                              pXmlDictionary.getBasename().toCChar(),
                              DictionaryFile->DicName.toCChar(),
                              getVersionStr( DictionaryFile->Version).toCChar(),
                              DictionaryFile->Active?"Active":"Not active");
  return true;
} //saveToDictionaryFile

bool
DicEditMWn::saveToXmlFile(uriString& pXmlDictionary) {
  ZStatus wSt;
  utf8VaryingString wXmlContent;
  ZMFDictionary* wZMFDic = screenToDic(nullptr);
  if (wZMFDic==nullptr)
    return false ;
  wZMFDic->setModified();  /* adjust dates (creation, modification)  */

  wXmlContent=wZMFDic->XmlSaveToString(true);

  delete wZMFDic;

  bool wExist = pXmlDictionary.exists();
  if (wExist) {
    uriString wBck= pXmlDictionary.getBckName("bck");
    pXmlDictionary.rename(wBck);
    wSt=pXmlDictionary.writeContent(wXmlContent);
    if (wSt != ZS_SUCCESS) {
        ZExceptionDLg::display("Replacing Xml File",ZException.last());
        return false;
    }
    ZExceptionDLg::adhocMessage("Xml dictionary replaced",Severity_Information,&ErrorLog,nullptr,
                                "Saved Xml dictionary file %s.\n"
                                "Existing file backuped to %s\n"
                                "Dictionary Name <%s> version <%s> status <%s>",
                                pXmlDictionary.getBasename().toCChar(),
                                wBck.getBasename().toCChar(),
                                DictionaryFile->DicName.toCChar(),
                                getVersionStr( DictionaryFile->Version).toCChar(),
                                DictionaryFile->Active?"Active":"Not active");
    return true;
  }
// DictionaryChanged=false;  /* dictionary remains in change mode */
    wSt=pXmlDictionary.writeContent(wXmlContent);
    if (wSt != ZS_SUCCESS) {
        ZExceptionDLg::display("Create Xml File",ZException.last());
        return false;
    }
    ZExceptionDLg::adhocMessage("Xml dictionary created",Severity_Information,&ErrorLog,nullptr,
                                "New dictionary file %s.\n"
                                "Created dic <%s> version <%s> status <%s>",
                                pXmlDictionary.getBasename().toCChar(),
                                DictionaryFile->DicName.toCChar(),
                                getVersionStr( DictionaryFile->Version).toCChar(),
                                DictionaryFile->Active?"Active":"Not active");
    return true;
} //saveToDictionaryFile

bool
DicEditMWn::updateEmbedded() {

  ZStatus wSt;

  if (MasterFile==nullptr) {
    ZExceptionDLg::adhocMessage("Embedded dictionary",Severity_Error,nullptr,nullptr,
        "No target master file has been defined.\n"
        "Go to file generation dialog and open a valid master file.");
    return false;
  }
  if (MasterFile->Dictionary==nullptr) {
    ZExceptionDLg::adhocMessage("Embedded dictionary",Severity_Error,nullptr,nullptr,
        "Target master file has no dictionary defined.\n"
        "Go to file generation dialog and open a valid master file with a dictionary.");
    return false;
  }


  utf8VaryingString wDicName = ui->DicNameLBl->text().toUtf8().data();
  unsigned long     wVersion = getVersionNum(ui->VersionLBl->text().toUtf8().data());
  bool              wActive=ui->ActiveCBx->currentIndex()==1;

  if (!getDicName(wVersion,wActive,wDicName))/* name meta dictionary and version */
    return false;

  /* update appropriately name and version */

  ui->DicNameLBl->setText(wDicName.toCChar());
  ui->VersionLBl->setText( getVersionStr( wVersion).toCChar());
  ui->ActiveCBx->setCurrentIndex(wActive?1:0);



  ZMFDictionary* wZMFDic = screenToDic(nullptr);

  wZMFDic->setModified();
/*
  ZExceptionDLg::setFixedFont();
  utf8VaryingString wComplement;
  wComplement.sprintf("Dictionary name <%s> version <%s> status <%s>\n"
                        " created <%s>\n"
                        " last modified <%s>",
                        MasterFile->Dictionary->DicName.toString(),
                        getVersionStr( MasterFile->Dictionary->Version).toString(),
                        MasterFile->Dictionary->Active?"Active":"Not active",
                        MasterFile->Dictionary->CreationDate.toMDY().toString(),
                        MasterFile->Dictionary->ModificationDate.toMDY().toString());
    int wRet=ZExceptionDLg::adhocMessage2B("Replace dictionary",Severity_Information,"Quit","Replace",
        nullptr,&wComplement,
        "MasterFile <%s>\n"
        "has already a dictionary named <%s> embedded to it."
        "    click <More> for details.\n"
        "Proceed anyway and replace <Replace>\n"
        "Give up and do not replace <Quit>.",
        MasterFile->getURIContent().toString(),
        MasterFile->Dictionary->DicName.toString());
    if (wRet==QDialog::Rejected)
      return false;
*/
  wSt=MasterFile->setDictionary(*wZMFDic);
  if (wSt!=ZS_SUCCESS) {
    utf8VaryingString wComplement;
    wComplement=ZException.last().formatFullUserMessage();
    int wRet=ZExceptionDLg::adhocMessage("Replace dictionary",Severity_Error,
        nullptr,&wComplement,
        "Cannot updated embedded dictionary for MasterFile <%s>",
        MasterFile->getURIContent().toString());
    return false;
  }

  utf8VaryingString wMsg;
  wMsg.sprintf("Dictionary <%s> has been successfully updated.",MasterFile->Dictionary->DicName.toString());
  statusBar()->showMessage(QObject::tr(wMsg.toCChar(),"DicEdit"),cst_MessageDuration);

  DictionaryChanged=false;
  delete wZMFDic;

  return true;
}//updateEmbedded


ZStatus
DicEditMWn::loadXmlDictionary (const uriString& pXmlDic)
{
  ZMFDictionary wMasterDic;
  utf8VaryingString wXmlContent,wStr;


  clear();

  ZStatus wSt = pXmlDic.loadUtf8(wXmlContent);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast();  /* loadUtf8 uses ZException */
    return wSt;
  }
  /* here load master dictionary */
  wSt =wMasterDic.XmlLoadFromString(wXmlContent,true,&ErrorLog);
  if (wSt!=ZS_SUCCESS)   /* XmlLoadFromString uses returns a ZStatus but uses ErrorLog to log messages */
  {
    ZExceptionDLg::messageWAdd("FloadfromXmlFileQAc",
        wSt,
        ErrorLog.getSeverity(),
        ErrorLog.allLoggedToString(),
        "Status is %s : Cannot load xml definition from file %s",decode_ZStatus(wSt), pXmlDic.toCChar());
    return wSt;
  }
  /* here we may only have warnings, infos or text messages */
  if (ErrorLog.hasMessages()) {
    Severity_type wS = ErrorLog.getSeverity();

    int wRet = ZExceptionDLg::message2BWAdd("FloadfromXmlFileQAc",
        wS > Severity_Warning?ZS_ERROR:ZS_WARNING,
        wS,
        ErrorLog.allLoggedToString(),
        "Stop","Continue",
        "Some warning messages have been issued during load of xml definition from file %s",pXmlDic.toCChar());
    if (wRet==QDialog::Rejected)
      return ZS_CANCEL;
  }

  wStr.sprintf("Xml dictionary file %s has been successfully loaded.",pXmlDic.toCChar());
  statusBar()->showMessage(QObject::tr(wStr.toCChar(),"DicEdit"),cst_MessageDuration);

  DictionaryFile->setDictionary(wMasterDic);
  DictionaryFile->URIDictionary = pXmlDic;
  displayZMFDictionary(wMasterDic);
  DictionaryChanged=false;
  return ZS_SUCCESS;
}// loadXmlDictionary

void
DicEditMWn::displayZMFDictionary(ZMFDictionary &pDic)
{
  utf8String wStr;
  QVariant   wV;
  ZDataReference wDRef;

  if (pDic.DicName.isEmpty())
    ui->DicNameLBl->setText("<no name>");
  else
    ui->DicNameLBl->setText(pDic.DicName.toCChar());

  ui->VersionLBl->setText(getVersionStr(pDic.Version).toCChar());

  ui->ActiveCBx->setCurrentIndex(pDic.Active?1:0);

  wStr.sprintf("%ld",pDic.count());
  ui->FieldsNbLBl->setText(wStr.toCChar());

  wStr.sprintf("%ld",pDic.KeyDic.count());
  ui->KeysNbLBl->setText(wStr.toCChar());

  clearAllRows();

  int wRow=0;
  QList<QStandardItem *> wFieldRow ;

  ZFieldDescription* wFD=nullptr;

  for (long wi=0;wi < pDic.count();wi++)
    {
    wFieldRow = createFieldRowFromField(&pDic[wi]);  /* create item list from field description */
#ifdef __COMMENT__ // already done by createFieldRowFromField
    /* set up data to item 0 */
    wFD=new ZFieldDescription(pDic[wi]);
    wDRef.setZLayout(ZLayout_FieldTBv);
    wDRef.setResource(getNewResource(ZEntity_DicField));
    wDRef.setDataRank((long)wi);    /* store rank within meta dictionary (for information) */
    wDRef.setPtr(wFD);              /* store pointer to effective field data */
    wV.setValue(wDRef);
    wFieldRow[0]->setData(wV,ZQtDataReference);
#endif // __COMMENT__
    fieldTBv->ItemModel->appendRow(wFieldRow);

    }// for

  fieldTBv->resizeRowsToContents();
  fieldTBv->resizeColumnsToContents();

  displayKeyDictionaries(pDic);

  generateQAc->setEnabled(true);
  DictionaryChanged=false;

}//DicEdit::displayZMFDictionary

void
DicEditMWn::displayKeyDictionaries(ZMFDictionary &pDic)
{
//  ZKeyHeaderRow* wKHR=nullptr;
//  ZKeyFieldRow* wKFR=nullptr;

  utf8String wStr;
  QVariant   wV;
  ZDataReference wDRefKey , wDRefKeyField;
  QList<QStandardItem *> wKeyRow, wKeyFieldRow ;

  clearKeyRows();

  for (long wi=0;wi < pDic.KeyDic.count();wi++) {
    ZKeyHeaderRow wKHR ;
    wKHR.set(pDic.KeyDic[wi]);

    wKeyRow=createKeyDicRow(wKHR); /* create item row with appropriate infra data linked to it */

    for (long wj=0;wj < pDic.KeyDic[wi]->count(); wj++) {
      ZKeyFieldRow wKFR ;
      wKFR.Hash = pDic.KeyDic[wi]->Tab(wj).Hash;
      wKFR.KeyOffset = pDic.KeyDic[wi]->Tab(wj).KeyOffset;
      wKFR.Name = pDic[pDic.KeyDic[wi]->Tab(wj).MDicRank].getName();
      wKFR.ZType  = pDic[pDic.KeyDic[wi]->Tab(wj).MDicRank].ZType;
      wKFR.UniversalSize  = pDic[pDic.KeyDic[wi]->Tab(wj).MDicRank].UniversalSize;
      int wFieldRow=searchForFieldName(wKFR.Name);
      if (wFieldRow < 0){
        abort();
      }

      wKeyFieldRow = createKeyFieldRow(wKFR);
      wKFR.FieldItem = fieldTBv->ItemModel->item(wFieldRow,0);

      wKeyRow[0]->appendRow(wKeyFieldRow);

      /* update index after child row has been created */
      wDRefKeyField.set(ZLayout_KeyTRv, getNewResource(ZEntity_KeyField),0);
      wDRefKeyField.setIndex(wKeyRow[0]->index());

    }// for wj

    keyTRv->ItemModel->appendRow(wKeyRow);
  }// for wi

//  ui->displayKeyTRv->resizeRowsToContents();
  keyTRv->expandAll();
  for (int wi=0;wi < keyTRv->ItemModel->columnCount();wi++ ){
    keyTRv->resizeColumnToContents(wi);
  }
  DictionaryChanged=true;
} //displayKeyDictionaries

#ifdef __COMMENT__
bool
DicEditMWn::updateKeyValues(const QModelIndex& pKeyIdx)
{
  utf8String wStr;
  if (!pKeyIdx.isValid())
  {
    fprintf(stderr,"DicEdit::updateKeyValues-E-INVIDX Index is not valid.\n");
    return false;
  }

  QModelIndex wKeyIdx=pKeyIdx;
  if (pKeyIdx.column()!=0)
    wKeyIdx=pKeyIdx.siblingAtColumn(0);

  QVariant wV = wKeyIdx.data(ZQtDataReference);
  if (!wV.isValid())
  {
    fprintf(stderr,"DicEdit::updateKeyValues-E-INVDATA Index data is not valid.\n");
    return false;
  }

  ZDataReference wDRef=wV.value<ZDataReference>();
  ZDataReference wDRefField;
  if (wDRef.isInvalid())
  {
    fprintf(stderr,"DicEdit::updateKeyValues-E-INVDREF Invalid Data reference\n");
    return false;
  }

  if (wDRef.getZEntity()!=ZEntity_KeyDic)
  {
    fprintf(stderr,"DicEdit::updateKeyValues-E-INVROW Index is not a Key Dictionary.\n");
    return false;
  }

  int wRNb=keyTRv->ItemModel->rowCount(wKeyIdx);

  ZKeyFieldRow*  wKFR=nullptr;
  ZKeyHeaderRow* wKHR=wDRef.getPtr<ZKeyHeaderRow*>();
  if (!wKHR->Forced)
        wKHR->KeyGuessedSize=0;
  long wKeyOffset=0;
  QStandardItem* wItem=nullptr;
  QModelIndex wItemIdx;

  int wRow=0;
  QModelIndex wIdx=keyTRv->ItemModel->index(wRow,0,wKeyIdx);
  while (wRow < wRNb)
  {
    wIdx=keyTRv->ItemModel->index(wRow,0,wKeyIdx);
    wV=wIdx.data(ZQtValueRole);
    wDRefField=wV.value<ZDataReference>();
/*    wKFPack = wV.value<KeyField_Pack>();
    wKFPack.KeyOffset = wKeyOffset;
    wKDPack->KeyGuessedSize += wKFPack.UniversalSize;
*/
    wKFR=wDRefField.getPtr<ZKeyFieldRow*>();
    wKFR->KeyOffset = wKeyOffset;
    if (!wKHR->Forced)
        wKHR->KeyGuessedSize += wKFR->UniversalSize;
    wKeyOffset+=wKFR->UniversalSize;

 //   wV.setValue(wKFPack); /* update back item data */
 //   wItem= keyTRv->ItemModel->itemFromIndex(wIdx);
 //   wItem->setData(wV,ZQtValueRole);

    /* update displayed key offset value */

    wItemIdx = keyTRv->ItemModel->index(wRow,2,wKeyIdx);
    wItem= keyTRv->ItemModel->itemFromIndex(wItemIdx);
    wStr.sprintf("%d",wKFR->KeyOffset);
    wItem->setText(wStr.toCChar());
    /* done */

    wRow++;
  }//while (wRow < wRNb)

  /* update back key dic header data */

 /* wItem= keyTRv->ItemModel->itemFromIndex(wKeyIdx);
  wV.setValue(wKDPack);
  wItem->setData(wV,ZQtValueRole);
*/

  /* update key size */

  wItemIdx = wKeyIdx.siblingAtColumn(2);
  wItem= keyTRv->ItemModel->itemFromIndex(wItemIdx);
  wStr.sprintf("%ld",wKHR->KeyGuessedSize);
  wItem->setText(wStr.toCChar());

  return true;
}//updateKeyValues
#endif // __COMMENT__

QList<QStandardItem *>
createKeyDicRow(const ZKeyHeaderRow& pKHR) {
  QVariant wV;
  ZDataReference wDRef;
  QList<QStandardItem *>  wKeyRow;

  wKeyRow << createItem( pKHR.DicKeyName.toCChar());  /* column 0 */
  wKeyRow[0]->setEditable(true);
  cst_KeyNameColumn = 0;

  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyDic),0);
  wDRef.setPtr<ZKeyHeaderRow>(new ZKeyHeaderRow(pKHR));
  wV= QVariant::fromValue<ZDataReference>(wDRef);
  wKeyRow[0]->setData(wV,ZQtDataReference);

  wKeyRow << createItem( "Dictionary key");  /* column 1 */

  wKeyRow << createItem( " total size");  /* column 2 */

  /* other characteristics of the key */
  wKeyRow << createItem( pKHR.KeyGuessedSize,"%ld");
  cst_KeyUSizeColumn = 3 ;
  wKeyRow.last()->setEditable(true);

  QStandardItem* wDup = createItemAligned( decode_ZST(pKHR.Duplicates),Qt::AlignLeft);
  wDup->setEditable(false);

  wKeyRow << wDup;
  cst_DuplicateColumn = 4 ;

  if (pKHR.ToolTip.isEmpty())
    wKeyRow << createItem( " ");
  else
    wKeyRow << createItemAligned( pKHR.ToolTip.toCChar(),Qt::AlignLeft);
  wKeyRow.last()->setEditable(true);
  cst_TooltipColumn = 5 ;
  return wKeyRow;
}//createKeyDicRow



void
DicEditMWn::clearAllRows()
{
  clearFieldRows();
  clearKeyRows();
} // clearAllRows

void
DicEditMWn::clearFieldRows() {
  ZDataReference wDRef;
  QVariant wV;

  if (fieldTBv) {
    if (fieldTBv->ItemModel) {
      if (fieldTBv->ItemModel->rowCount()>0) {
        for (int wi=0;wi < fieldTBv->ItemModel->rowCount();wi++) {
          wV = fieldTBv->ItemModel->item(wi,0)->data(ZQtDataReference);
          wDRef = wV.value<ZDataReference>();
          wDRef.releasePtr<ZFieldDescription>();
//          ZFieldDescription* wFDesc = wDRef.getPtr<ZFieldDescription*>();
//          delete wFDesc;
        } // for
      } // rowCount()
      fieldTBv->ItemModel->removeRows(0,fieldTBv->ItemModel->rowCount());
    } // fieldTBv->ItemModel
  }// fieldTBv
}

void
DicEditMWn::clearKeyRows() {
  ZDataReference wDRef;
  QVariant wV;
  if (keyTRv) {
    if (keyTRv->ItemModel) {
      if (keyTRv->ItemModel->rowCount()>0) {

        for (int wi=0;wi < keyTRv->ItemModel->rowCount();wi++) {
          /* delete all data for key fields in the key */
          QStandardItem* wKeyItem = keyTRv->ItemModel->item(wi,0);
          for (int wj=0;wj < wKeyItem->rowCount();wj++) {
            wV = wKeyItem->child(wj,0)->data(ZQtDataReference);
            wDRef = wV.value<ZDataReference>();
            ZKeyFieldRow* wKFR = wDRef.getPtr<ZKeyFieldRow>();
            delete wKFR;
          }
          /* delete data for the key itself */
          wV = keyTRv->ItemModel->item(wi,0)->data(ZQtDataReference);
          wDRef = wV.value<ZDataReference>();
          ZKeyHeaderRow* wKHR = wDRef.getPtr<ZKeyHeaderRow>();
          delete wKHR;
        } // for

        /* clean display */
        keyTRv->ItemModel->removeRows(0,keyTRv->ItemModel->rowCount());
      }// rowCount
    } // ItemModel
  } // keyTRv
}

void
DicEditMWn::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (_FResizeInitial) {
    _FResizeInitial=false;
    return;
  }
  QRect wR1 = ui->layoutWidget->geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  ui->layoutWidget->resize(wVW,wVH);  /* expands in width and height */

}//DicEdit::resizeEvent


QList<QStandardItem *>
createKeyFieldRow(const ZKeyFieldRow& wKFR) {

  QList<QStandardItem *>  wKeyFieldRow;
  ZDataReference          wDRef(ZLayout_KeyTRv,getNewResource(ZEntity_KeyField));
  QVariant                wV;

  wKeyFieldRow << createItem(wKFR.Name.toCChar());
  /* assign data reference and ZKeyFieldRow data to item 0 */
  wDRef.setPtr<ZKeyFieldRow>(new ZKeyFieldRow(wKFR));
  wV.setValue(wDRef);
  wKeyFieldRow[0]->setData(wV,ZQtDataReference);

  wKeyFieldRow << createItem( decode_ZType (wKFR.ZType));
  wKeyFieldRow << createItem( wKFR.KeyOffset,"%d");
  cst_KeyOffsetColumn = 2;
  wKeyFieldRow << createItem( wKFR.UniversalSize,"%ld");
  wKeyFieldRow << createItem( wKFR.Hash);

  return wKeyFieldRow;
}

QList<QStandardItem *>
createKeyFieldRowFromFieldItem(QStandardItem* pFieldItem) {

  ZKeyFieldRow wKFR;
  wKFR.setFromQItem(pFieldItem);
  return createKeyFieldRow(wKFR);
}


/**
 * @brief createRowFromFieldDescription creates an item list describing field desciption.
 *                                      infra data (ZFieldDescription) is created and set by this routine.
 */

QList<QStandardItem*> createFieldRowFromField(ZFieldDescription* pField) {
  utf8String wStr;
  QList<QStandardItem*> wFieldRow;
  QVariant wV;
  ZDataReference wDRef(ZLayout_FieldTBv,getNewResource(ZEntity_DicField),0);

  wDRef.setPtr<ZFieldDescription>(new ZFieldDescription(*pField));

  wV.setValue(wDRef);

  wFieldRow << new QStandardItem(pField->getName().toCChar());
  wFieldRow[0]->setData(wV,ZQtDataReference);

  wStr.sprintf("0x%08X",pField->ZType);
  wFieldRow << new QStandardItem( wStr.toCChar());

  wFieldRow << new QStandardItem( decode_ZType(pField->ZType));

  wStr.sprintf("%d",pField->Capacity);
  wFieldRow << new QStandardItem( wStr.toCChar());

  wStr.sprintf("%ld",pField->HeaderSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());

  wStr.sprintf("%ld",pField->UniversalSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());

  wStr.sprintf("%ld",pField->NaturalSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());

  wFieldRow <<  new QStandardItem( pField->KeyEligible?"Yes":"No");

  if (pField->ToolTip.isEmpty())
    wFieldRow <<  new QStandardItem( "" );
  else
    wFieldRow <<  new QStandardItem( pField->ToolTip.toCChar());

  wFieldRow <<  new QStandardItem( pField->Hash.toHexa().toCChar());

/*
  for (int wi=0;wi<wFieldRow.count();wi++) {
    fprintf (stdout," %s ",wFieldRow[wi]->text().toUtf8().data());
  }
  fprintf (stdout,"\n");
  std::cout.flush();
*/
  return wFieldRow;
}

QStandardItem* setFieldRowFromField(QStandardItemModel *pModel, int pRow, ZFieldDescription& pField) {
  utf8String wStr;
  if (pModel->columnCount() < 10) {
    fprintf(stderr,"setRowFromField-E-IVCNT  Invalid number of QStandardItem within field row to update.\n");
    return nullptr;
  }
  int wCol=0;
  pModel->item(pRow,wCol++)->setText(pField.getName().toCChar());
  /* change infra data accordingly */
  QVariant wV;
  ZDataReference wDRef;
  wV=pModel->item(pRow,0)->data(ZQtDataReference);
  wDRef=wV.value<ZDataReference>();
  ZFieldDescription* wFD=wDRef.getPtr<ZFieldDescription>();
  wFD->_copyFrom(pField);

  wStr.sprintf("0x%08X",pField.ZType);
  pModel->item(pRow,wCol++)->setText(wStr.toCChar());

  pModel->item(pRow,wCol++)->setText(decode_ZType(pField.ZType));

  wStr.sprintf("%d",pField.Capacity);
  pModel->item(pRow,wCol++)->setText(wStr.toCChar());

  wStr.sprintf("%ld",pField.HeaderSize);
  pModel->item(pRow,wCol++)->setText(wStr.toCChar());


  wStr.sprintf("%ld",pField.UniversalSize);
  pModel->item(pRow,wCol++)->setText(wStr.toCChar());

  wStr.sprintf("%ld",pField.NaturalSize);
  pModel->item(pRow,wCol++)->setText(wStr.toCChar());

  pModel->item(pRow,wCol++)->setText(pField.KeyEligible ? "Yes":"No");

  if (pField.ToolTip.isEmpty())
    pModel->item(pRow,wCol++)->setText("");
  else
    pModel->item(pRow,wCol++)->setText(pField.ToolTip.toCChar());

    pModel->item(pRow,wCol++)->setText(pField.Hash.toHexa().toCChar());

  return pModel->item(pRow,0);
}//setFieldRowFromField

QStandardItem* DicEditMWn::changeFieldRowFromField(int pRow,ZFieldDescription& pField) {
  utf8String wStr;
  /* first delete infradata for item (pRow,0) : release resource and delete ZFieldDescription */
  QVariant wV=fieldTBv->ItemModel->item(pRow,0)->data(ZQtDataReference);
  ZDataReference wDRef = wV.value<ZDataReference>();
  releaseResource(wDRef.ResourceReference);
  ZFieldDescription* wToBeDeleted=wDRef.getPtr<ZFieldDescription>();
  delete (wToBeDeleted);

  /* recreate item 0 with appropriate infradata */
  QStandardItem* wItem=createItem(pField.getName());
  wDRef.set(ZLayout_FieldTBv,getNewResource(ZEntity_DicField),0); /* request a new resource */
  wDRef.setPtr<ZFieldDescription>(new ZFieldDescription(pField));                    /* associate a new instance of ZFieldDescription */
  wV.setValue(wDRef);
  wItem->setData(wV,ZQtDataReference);                           /* link infra data to item */

//  wItem->setText(pField.getName().toCChar());
  int wCol=0;
  fieldTBv->ItemModel->setItem(pRow,wCol,wItem);

  wStr.sprintf("0x%08X",pField.ZType);
  fieldTBv->ItemModel->item(pRow,wCol++)->setText(wStr.toCChar());

  fieldTBv->ItemModel->item(pRow,wCol++)->setText(decode_ZType(pField.ZType));

  wStr.sprintf("%d",pField.Capacity);
  fieldTBv->ItemModel->item(pRow,wCol++)->setText(wStr.toCChar());

  wStr.sprintf("%ld",pField.HeaderSize);
  fieldTBv->ItemModel->item(pRow,wCol++)->setText(wStr.toCChar());

  wStr.sprintf("%ld",pField.UniversalSize);
  fieldTBv->ItemModel->item(pRow,wCol++)->setText(wStr.toCChar());

  wStr.sprintf("%ld",pField.NaturalSize);
  fieldTBv->ItemModel->item(pRow,wCol++)->setText(wStr.toCChar());

  fieldTBv->ItemModel->item(pRow,wCol++)->setText(pField.KeyEligible?"Yes":"No");

  if (pField.ToolTip.isEmpty())
    fieldTBv->ItemModel->item(pRow,wCol++)->setText("");
  else
    fieldTBv->ItemModel->item(pRow,wCol++)->setText(pField.ToolTip.toCChar());

    fieldTBv->ItemModel->item(pRow,wCol++)->setText(pField.Hash.toHexa().toCChar());

  return wItem;
} // changeFieldRowFromField

