#include "zsearchquerymwd.h"
//#include "ui_zsearchquerymwd.h"

#include <functional>

#include <QWidget>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QFrame>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>

#include <QStatusBar>
#include <QLabel>

#include <QProgressBar>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QHeaderView>

#include <zqt/zqtwidget/zqplaintextedit.h>

#include <zqt/zqtwidget/zqtableview.h>

#include "texteditmwn.h"

#include <zcontent/zindexedfile/zsearchparser.h>

#include <zcontent/zindexedfile/zsearchparsertype.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zcontentcommon/zresource.h>

#include "zcellformatdlg.h"
#include "zcollectionlist.h"

const long StringDiplayMax = 64;

ZSearchQueryMWd::ZSearchQueryMWd(QWidget *parent) : QMainWindow(parent)
{
  setup();
}

ZSearchQueryMWd::~ZSearchQueryMWd()
{
  if (displayMWn!=nullptr)
    delete displayMWn;
  if (Parser!=nullptr)
    delete Parser;
//  delete ui;
}



void ZSearchQueryMWd::setup()
{

  QHBoxLayout *ButtonBox=nullptr;
  QSpacerItem *horizontalSpacer=nullptr;
  setObjectName(QString::fromUtf8("ZSearchQueryMWd"));

  resize(814, 641);
  centralwidget = new QWidget(this);
  setCentralWidget(centralwidget);

  ExpWidget1= new QWidget(centralwidget);
  ExpWidget1->setGeometry(QRect(0, 10, 801, 91));
  QHBoxLayout* hl1 = new QHBoxLayout;
  ExpWidget1->setLayout(hl1);

  QueryQTe = new ZQPlainTextEdit(ExpWidget1);
  QueryQTe->setGeometry(QRect(0, 10, 801, 91));
  QueryQTe->setObjectName(QString::fromUtf8("QueryQTe"));

  hl1->addWidget(QueryQTe);

  ExpWidget2= new QWidget(centralwidget);
  ExpWidget2->setGeometry(0,130,801,411);

  QVBoxLayout* vl1 = new QVBoxLayout;
  QHBoxLayout* hl20 = new QHBoxLayout;
  QHBoxLayout* hl21 = new QHBoxLayout;

  ExpWidget2->setLayout(vl1);

  vl1->setAlignment(Qt::AlignLeft);

  vl1->addLayout(hl20);
  QLabel* QL10 = new QLabel("Entity",ExpWidget2);
  EntityNameLBl = new QLabel(ExpWidget2);
  EntityNameLBl->setFrameStyle(QFrame::Panel | QFrame::Raised);
  EntityNameLBl->setLineWidth(2);

  QLabel* QL11 = new QLabel("Type",ExpWidget2);
  EntityTypeLBl = new QLabel(ExpWidget2);
  EntityTypeLBl->setFrameStyle(QFrame::Panel | QFrame::Raised);
  EntityTypeLBl->setLineWidth(2);

  QLabel* QL12 = new QLabel("Count",ExpWidget2);
  RecordCountLBl = new QLabel(ExpWidget2);
  RecordCountLBl->setFrameStyle(QFrame::Panel | QFrame::Raised);
  RecordCountLBl->setLineWidth(2);
   QLabel* QL13 = new QLabel("Last status",ExpWidget2);
  LastStatusLBl = new QLabel(ExpWidget2);
  LastStatusLBl->setFrameStyle(QFrame::Panel | QFrame::Raised);
  LastStatusLBl->setLineWidth(2);

  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);


  hl20->addWidget(QL10);
  hl20->addWidget(EntityNameLBl);
  hl20->addWidget(QL11);
  hl20->addWidget(EntityTypeLBl);
  hl20->addWidget(QL12);
  hl20->addWidget(RecordCountLBl);
  hl20->addWidget(QL13);
  hl20->addWidget(LastStatusLBl);
  hl20->addItem(wSpacer);

  vl1->addLayout(hl21);

  ResultTBv = new ZQTableView(ExpWidget2);
  ResultTBv->setObjectName(QString::fromUtf8("ResultTBv"));

  hl21->addWidget(ResultTBv);

  ButtonBox = new QHBoxLayout;
  ButtonBox->setObjectName(QString::fromUtf8("ButtonBox"));
  ButtonBox->setContentsMargins(0, 0, 0, 0);
  horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

  ButtonBox->addItem(horizontalSpacer);
  vl1->addLayout(ButtonBox);


  ListBTn = new QPushButton("List",ExpWidget2);
  ButtonBox->addWidget(ListBTn);

  ClearBTn = new QPushButton("Clear log",ExpWidget2);
  ButtonBox->addWidget(ClearBTn);

  QuitBTn = new QPushButton("Quit",ExpWidget2);
  ButtonBox->addWidget(QuitBTn);
  ExecuteBTn = new QPushButton("Execute",ExpWidget2);
  ButtonBox->addWidget(ExecuteBTn);




  ProgressBar = new QProgressBar(this);
  QHBoxLayout* hl3 = new QHBoxLayout;

  vl1->addLayout(hl3);
  hl3->addWidget(ProgressBar);

  menubar = new QMenuBar(this);
  menubar->setGeometry(QRect(0, 0, 814, 20));
  setMenuBar(menubar);
  statusbar = new QStatusBar(this);
  setStatusBar(statusbar);

  QMenu* MainMenuMEn = new QMenu("General");
  menubar->addMenu(MainMenuMEn);

  QActionGroup* wMainAGp=new QActionGroup(MainMenuMEn);

  ExecQAc=new QAction("Execute (F9)");
  QuitQAc=new QAction("Quit");
  SaveInstructionsQAc=new QAction("Save instuctions");


  MainMenuMEn->addAction(QuitQAc);
  MainMenuMEn->addAction(ExecQAc);
  MainMenuMEn->addAction(SaveInstructionsQAc);

  wMainAGp->addAction(QuitQAc);
  wMainAGp->addAction(ExecQAc);
  wMainAGp->addAction(SaveInstructionsQAc);

  /* not in action group */

  QMenu*    OptionsQMe=new QMenu("Options");
  MainMenuMEn->addMenu(OptionsQMe);

  QAction*  OptReportQAc=new QAction("Parsing report");
  OptReportQAc->setCheckable(true);


  QMenu*    VerboseQMe=new QMenu("Verbose level");
  QAction*  OptNoVerboseQAc=new QAction("Mute");
  OptNoVerboseQAc->setCheckable(true);
  QAction*  OptVerboseQAc=new QAction("Verbose");
  OptNoVerboseQAc->setCheckable(true);
  QAction*  OptFullVerboseQAc=new QAction("Full verbose");
  OptNoVerboseQAc->setCheckable(true);

  VerboseQMe->addAction(OptFullVerboseQAc);
  VerboseQMe->addAction(OptVerboseQAc);
  VerboseQMe->addAction(OptNoVerboseQAc);

  QHeaderView * wHV=ResultTBv->horizontalHeader();
  wHV->setSectionsClickable(true);
  QObject::connect(wHV,&QHeaderView::sectionDoubleClicked,this,&ZSearchQueryMWd::TableHeaderClicked);

  QObject::connect(QuitBTn,&QPushButton::clicked,this,&ZSearchQueryMWd::QuitBTnClicked);
  QObject::connect(ExecuteBTn,&QPushButton::clicked,this,&ZSearchQueryMWd::ExecuteBTnClicked);
  QObject::connect(ClearBTn,&QPushButton::clicked,this,&ZSearchQueryMWd::ClearBTnClicked);

  QObject::connect(ListBTn,&QPushButton::clicked,this,&ZSearchQueryMWd::ListBTnClicked);

  QObject::connect(wMainAGp,&QActionGroup::triggered,this,&ZSearchQueryMWd::MenuTriggered);

  ResultTBv->SelectAllCallBack = std::bind(&ZSearchQueryMWd::TableHeaderCornerClicked, this);

  /* set call back for function keys */
  QueryQTe->TrappedKeyPressCallBack =  std::bind(&ZSearchQueryMWd::KeyFiltered, this,placeholders::_1,placeholders::_2);
  QueryQTe->UntrappedKeyPressCallBack =  std::bind(&ZSearchQueryMWd::KeyUnFiltered, this,placeholders::_1);

  displayMWn = new textEditMWn(this,TEOP_NoFileLab|TEOP_CloseBtnHide);
  displayMWn->setWindowTitle("Search parser output");

  Parser = new ZSearchParser;

//  Parser->setDisplayCallback(std::bind(&ZSearchQueryMWd::displayErrorCallBack, this,placeholders::_1));
  Parser->setDisplayColorCB(std::bind(&ZSearchQueryMWd::displayErrorColorCB, this,placeholders::_1,placeholders::_2));

  Parser->setProgressSetupCallback(std::bind(&ZSearchQueryMWd::setProgressBarMax, this,placeholders::_1));
  Parser->setProgressCallback(std::bind(&ZSearchQueryMWd::setProgressBarValue, this,placeholders::_1));

  displayMWn->show();

  const char* wWD=getenv(__SEARCHPARSER_PARAM_DIRECTORY__);
  if (!wWD)
    wWD="";

  uriString wXMLSymbol = wWD;
  wXMLSymbol.addConditionalDirectoryDelimiter();
  wXMLSymbol += __SEARCHPARSER_SYMBOL_FILE__ ;

  uriString wXMLZEntity = wWD;
  wXMLZEntity.addConditionalDirectoryDelimiter();
  wXMLZEntity += __SEARCHPARSER_ZENTITY_FILE__ ;

  uriString wHistory = wWD;
  wHistory.addConditionalDirectoryDelimiter();
  wHistory += __SEARCHPARSER_HISTORY_FILE__ ;

  uriString wParams = wWD;
  wParams.addConditionalDirectoryDelimiter();
  wParams += __SEARCHPARSER_PARAMS_FILE__ ;

  Parser->setup(wXMLSymbol,wXMLZEntity,wParams,wHistory);




} // setup



bool
ZSearchQueryMWd::KeyFiltered(int pKey, QKeyEvent *pEvent)
{

  if(pKey == Qt::Key_F3) {
    recallBackward();
    return true;
  }
  if(pKey == Qt::Key_F4) {
    recallForward();
    return true;
  }

  if(pKey == Qt::Key_F9) {
    ExecuteBTnClicked(true);
    return true;
  }
  if(pKey == Qt::Key_F10) {
    Parser->showEntities();
    return true;
  }
  if(pKey == Qt::Key_F11) {
    Parser->showEntities();
    return true;
  }

  return false;
}//KeyFiltered

void
ZSearchQueryMWd::KeyUnFiltered( QKeyEvent *pEvent)
{
  normalKeyEnter();
}//KeyFiltered

void
ZSearchQueryMWd::normalKeyEnter()
{
  IndexRecall=-1;

  if (LastInsertedPosition < 0)
    return;

  QTextCursor wTC=QueryQTe->textCursor();

//  _DBGPRINT("Changing inserted text attributes\n");
//  _DBGPRINT("FirstInsertedPosition %d\n",FirstInsertedPosition);
  wTC.setPosition(FirstInsertedPosition,QTextCursor::MoveAnchor);
  /* If mode is QTextCursor::KeepAnchor, the cursor selects the text it moves over.
   * This is the same effect that the user achieves when they hold down the Shift key
   * and move the cursor with the cursor keys.*/
//  _DBGPRINT("LastInsertedPosition %d\n",LastInsertedPosition);
  wTC.setPosition(LastInsertedPosition,QTextCursor::KeepAnchor);
  // If any, this should be block selection
  //    wTC.select(QTextCursor::BlockUnderCursor);
  wTC.setCharFormat(*QueryQTe->FmtDefault);
  QueryQTe->setTextCursor(wTC);
//  _DBGPRINT("Changed\n");

  FirstInsertedPosition = LastInsertedPosition = -1;
}


void
ZSearchQueryMWd::insertHighlightedText(const utf8VaryingString& pText)
{
  QTextCursor wTC=QueryQTe->textCursor();
//  int wCurPos = wTC.position();
//  _DBGPRINT("cur pos %d\n",wCurPos)
  if (LastInsertedPosition >= 0) {
//    _DBGPRINT("Removing\n");
//    _DBGPRINT("FirstInsertedPosition %d\n",FirstInsertedPosition);
    wTC.setPosition(FirstInsertedPosition,QTextCursor::MoveAnchor);
    /* If mode is QTextCursor::KeepAnchor, the cursor selects the text it moves over.
     * This is the same effect that the user achieves when they hold down the Shift key
     * and move the cursor with the cursor keys.*/
//    _DBGPRINT("LastInsertedPosition %d\n",LastInsertedPosition);
    wTC.setPosition(LastInsertedPosition,QTextCursor::KeepAnchor);
    wTC.removeSelectedText();
    QueryQTe->setTextCursor(wTC);
//    _DBGPRINT("Removed\n")
  }
//  _DBGPRINT("Inserting\n");
  FirstInsertedPosition=wTC.position();
//  _DBGPRINT("FirstInsertedPosition %d\n",FirstInsertedPosition);
  wTC.setPosition(FirstInsertedPosition,QTextCursor::MoveAnchor);
  QTextCharFormat wDefaultFormat , wFmt;
  wDefaultFormat = wFmt = wTC.charFormat();
  wFmt.setBackground(Qt::lightGray);
  wFmt.setForeground(Qt::blue);
  wTC.setCharFormat(wFmt);

  wTC.insertText(pText.toCChar());
  wTC.setCharFormat(wDefaultFormat);
//  _DBGPRINT("Inserted\n");
  LastInsertedPosition = wTC.position();
//  _DBGPRINT("LastInsertedPosition %d\n",LastInsertedPosition);
  QueryQTe->setTextCursor(wTC);
} //insertHighlightedText


void
ZSearchQueryMWd::recallForward()
{
  utf8VaryingString wInst;
  ZStatus wSt=Parser->History.getNext(wInst);
  if (wSt!=ZS_SUCCESS)
    return;
  QueryQTe->clear();
  QueryQTe->insertText(wInst);
//  insertHighlightedText(wInst);
}

void
ZSearchQueryMWd::recallBackward()
{
  utf8VaryingString wInst;
  ZStatus wSt=Parser->History.getPrevious(wInst);
  if (wSt!=ZS_SUCCESS)
    return;

  QueryQTe->clear();
  QueryQTe->insertText(wInst);
//  insertHighlightedText(wInst);
}


void
ZSearchQueryMWd::MenuTriggered(QAction* pAction)
{
  if (pAction == QuitQAc){
    QuitBTnClicked(true);
    return;
  }
  if (pAction == ExecQAc){
    ExecuteBTnClicked(true);
    return;
  }
  if (pAction == SaveInstructionsQAc){
    SaveInstructions();
    return;
  }
}

void
ZSearchQueryMWd::QuitBTnClicked (bool pChecked)
{
  hide();
  this->deleteLater();
}

void
ZSearchQueryMWd::ClearBTnClicked (bool pChecked)
{
  Parser->clearErrors();
  displayMWn->clear();
}

void
ZSearchQueryMWd::ListBTnClicked (bool pChecked)
{
  ZCollectionList* wColList=new ZCollectionList(Parser,this);
  int wRet=wColList->exec();

  if (wRet==QDialog::Rejected)
    return;

  CurrentEntity = wColList->get();
  wColList->deleteLater();

  DisplayCurrentEntity();
}

void
ZSearchQueryMWd::ExecuteBTnClicked (bool pChecked)
{
  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wQuery = QueryQTe->getText();
  statusbar->clearMessage();

  displayMWn->show();

  wSt=Parser->parse(wQuery,CurrentEntity);
  if (wSt!=ZS_SUCCESS) {

    LastStatusLBl->setText(decode_ZStatus(wSt));

    if (Parser->LastErroredToken!=nullptr) {
      int wLen = Parser->LastErroredToken->Text.strlen();
      if (Parser->LastErroredToken->Type==ZSRCH_STRING_LITERAL)
        wLen += 2 ; // the quotes are elided within token content but are present in the text
      QueryQTe->highlightLengthAtOffset( Parser->LastErroredToken->TokenOffset,wLen);
    }
    statusbar->showMessage(Parser->ZaiErrors::last()->Message());
    return;
  }
  LastStatusLBl->setText(decode_ZStatus(wSt));
  if (CurrentEntity!=nullptr)
    DisplayCurrentEntity();
  QueryQTe->clear();


} // ZSearchQueryMWd::ExecuteBTnClicked


void ZSearchQueryMWd::DisplayCurrentEntity ()
{
  utf8VaryingString wStr;
  ZDataBuffer wRecord;
  ZStatus wSt=ZS_SUCCESS;



  QList<QStandardItem*> wRow;

  if (CurrentEntity==nullptr)
    return;

  LastStatusLBl->setText("");

  const ZMetaDic* wMDic = CurrentEntity->getFieldDictionaryPtr();

  if (CurrentEntity->CellFormat==nullptr) {
    CurrentEntity->reallocateCellFormat();
  }


  if (ResultTBv==nullptr) {
    ResultTBv = new ZQTableView(this);
  }

  if (!ResultTBv->hasModel())
    ResultTBv->newModel(wMDic->count());
  else {
    ResultTBv->ItemModel->clear();
    ResultTBv->setColumnCount(wMDic->count());
    ResultTBv->ItemModel->setColumnCount(wMDic->count());
  }



  ResultTBv->verticalHeader()->setVisible(true);

  for (int wi=0;wi < wMDic->count();wi++) {
    ResultTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(wMDic->TabConst(wi).getName().toCChar()));
  }


  EntityNameLBl->setText(CurrentEntity->getName().toCChar());

  if (CurrentEntity->isCollection()) {
    EntityTypeLBl->setText("Collection");
    wStr.sprintf("%ld",CurrentEntity->_CollectionEntity->getMaxRecords());
    RecordCountLBl->setText(wStr.toCChar());
  }
  else if (CurrentEntity->isFile()) {
    EntityTypeLBl->setText("File");
    wStr.sprintf("%ld",CurrentEntity->_FileEntity->getRecordCount());
    RecordCountLBl->setText(wStr.toCChar());
  } else {
    EntityTypeLBl->setText("Unknown type");
    RecordCountLBl->setText("---");
  }


  long wRank=0;
  zaddress_type wAddress;
  const unsigned char* wRecordPtr=nullptr;
  const unsigned char* wRecordPtrEnd=nullptr;



  wSt=CurrentEntity->get(wRecord,wRank,wAddress);

  while (wSt == ZS_SUCCESS) {
    wRecordPtr = wRecord.Data;
    wRecordPtrEnd = wRecord.Data + wRecord.Size;
    wRow=DisplayOneLine(int(wRank),wRecord,CurrentEntity->CellFormat);
    ResultTBv->ItemModel->appendRow(wRow);
    wRank++;
    wSt=CurrentEntity->get(wRecord,wRank,wAddress);
  }
  LastStatusLBl->setText(decode_ZStatus(wSt));


  int wR=ResultTBv->ItemModel->rowCount();
  ResultTBv->setVisible(true);
  ResultTBv->repaint();
  ResultTBv->show();
  return;
}


QList<QStandardItem*>
ZSearchQueryMWd::DisplayOneLine(int pRow,ZDataBuffer& pRecord,int* pCellFormat)
{
  utf8VaryingString wStr;
  QList<QStandardItem*> wRow;
  zaddress_type wOffset=0;
  const unsigned char* wPtr = pRecord.Data;
  const unsigned char* wPtrEnd = pRecord.Data + pRecord.Size;
  int wFieldRank = 0;
  ZTypeBase wZType;
  ZBitset wPresence;

  _importAtomic<ZTypeBase>(wZType,wPtr);
  while (true) {
    if ((wZType != ZType_bitset) && (wZType != ZType_bitsetFull)) {
      utf8VaryingString wStr;
      wStr.sprintf("Invalid format. While expecting <ZType_bitset>, found <%6X> <%s>.",wZType,decode_ZType(wZType));
      statusbar->showMessage(wStr.toCChar());
      displayMWn->appendTextColor(Qt::red,wStr.toCChar());
      break;
    }
    if (wZType==ZType_bitset) {
      wPtr -= sizeof(ZTypeBase);
      ssize_t wSize=wPresence._importURF(wPtr);
      wOffset += wSize;
    } // if (wZType==ZType_bitset)

    else if (wZType==ZType_bitsetFull) {
      wPresence.setFullBitset();
      wOffset += sizeof(ZTypeBase);
    } // if (wZType==ZType_bitsetFull)
     break;
  }// while true

  while (wPtr < wPtrEnd) {
    if (!wPresence.isFullBitset() && (wFieldRank >= int(wPresence.count())))
      break;
    if (wPresence[wFieldRank]) {
      int wCellFormat = ZCFMT_Nothing;
      if (pCellFormat!=nullptr) {
        if (pCellFormat[wFieldRank+1]==ZCFMT_Nothing)
          wCellFormat = pCellFormat[0];
        else
          wCellFormat = pCellFormat[wFieldRank+1];
      }

      wRow << DisplayOneURFField(wPtr,wPtrEnd,wCellFormat);
    }
    else
      wRow << new QStandardItem("missing");
    wFieldRank++;
  } // while

  return wRow;
} //DisplayOneLine

void
ZSearchQueryMWd::TableHeaderCornerClicked()
{
  int wCellFormat=CurrentEntity->CellFormat[0];
  ZCellFormatDLg* ZCellFormat= new ZCellFormatDLg(this);
  ZCellFormat->setup(wCellFormat);
  ZCellFormat->setAppliance("All columns");
  ZCellFormat->exec();

  int wNewCellFormat=ZCellFormat->get();
  if (wNewCellFormat!=wCellFormat) {
    CurrentEntity->CellFormat[0]=wNewCellFormat;
    DisplayCurrentEntity();
  }
}

void
ZSearchQueryMWd::TableHeaderClicked(int pLogicalIndex)
{
  int wCellFormat=CurrentEntity->CellFormat[pLogicalIndex+1];
  ZCellFormatDLg* ZCellFormat= new ZCellFormatDLg(this);
  ZCellFormat->setup(wCellFormat);
  utf8VaryingString wStr;
  wStr.sprintf("Column %d",pLogicalIndex+1);
  ZCellFormat->setAppliance(wStr.toCChar());

  ZCellFormat->exec();

  int wNewCellFormat=ZCellFormat->get();
  if (wNewCellFormat!=wCellFormat) {
    CurrentEntity->CellFormat[pLogicalIndex+1]=wNewCellFormat;
    DisplayCurrentEntity();
  }
}

QStandardItem*
ZSearchQueryMWd::DisplayOneURFField(const unsigned char* &pPtr,const unsigned char* wPtrEnd,int pCellFormat)
{
  ZTypeBase wZType;
  size_t    wOffset=0;
  //  uint64_t  wURFDataSize = 0;

  ZStatus wSt=ZS_SUCCESS;

  utf8VaryingString wStr;

  _importAtomic<ZTypeBase>(wZType,pPtr);

  /* for atomic URF data, value is just following ZType. For other types, use _importURF function that implies ZType */
  if (!(wZType & ZType_Atomic))
    pPtr -= sizeof(ZTypeBase);
  else  {
    wZType &= ~ZType_Atomic;
    wOffset += sizeof(ZTypeBase);
  }
  switch (wZType) {
  case ZType_UChar:
  case ZType_U8: {
    uint8_t wValue;

    wValue=convertAtomicBack<uint8_t> (ZType_U8,pPtr);

    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%u",wValue);
    return  createItem(wStr);
  }
  case ZType_Char:
  case ZType_S8: {
    int8_t wValue;
    wValue=convertAtomicBack<int8_t> (ZType_S8,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%d",wValue);
    return createItem(wStr);
  }
  case ZType_U16:{
    uint16_t wValue;
    wValue=convertAtomicBack<uint16_t> (ZType_U16,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
        wStr.sprintf("%u",wValue);

    return createItem(wStr);
  }
  case ZType_S16: {
    int16_t wValue;
    wValue=convertAtomicBack<int16_t> (ZType_S16,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%d",wValue);
    return createItem(wStr);
  }

  case ZType_U32:{
    uint32_t wValue;
    wValue=convertAtomicBack<uint32_t> (ZType_U32,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%u",wValue);
    return createItem(wStr);
    break;
  }
  case ZType_S32: {
    int32_t wValue;
    wValue=convertAtomicBack<int32_t> (ZType_S32,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%d",wValue);
    return createItem(wStr);
  }
  case ZType_U64: {
    uint64_t wValue;
    wValue=convertAtomicBack<uint64_t> (ZType_U64,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%llu",wValue);
   return createItem(wStr);
  }
  case ZType_S64: {
    int64_t wValue;
    wValue=convertAtomicBack<int64_t> (ZType_S64,pPtr);
    if ((pCellFormat & ZCFMT_NumMask)==ZCFMT_NumHexa)
      wStr.sprintf("0x%X",wValue);
    else
      wStr.sprintf("%lld",wValue);

    return createItem(wStr);
  }
  case ZType_Float: {
    float wValue;
    wValue=convertAtomicBack<float> (ZType_Float,pPtr);

    return createItem(wValue,"%g");
  }

  case ZType_Double: {
    double wValue;
    wValue=convertAtomicBack<double> (ZType_Double,pPtr);

    return createItem(wValue,"%g");
  }

  case ZType_LDouble: {
    long double wValue;
    wValue=convertAtomicBack<long double> (ZType_LDouble,pPtr);

    return createItem(wValue,"%g");

  }



  case ZType_ZDateFull: {
    ssize_t wSize;
    ZDateFull wZDateFull;
    utf8VaryingString wStr;

    if ((wSize = wZDateFull._importURF(pPtr)) < 0) {
      return new QStandardItem("**Invalid date value**");
    }
    int wFmt = pCellFormat & ZCFMT_DateMask;
    switch (wFmt)
    {

    case ZCFMT_DMY:
      wStr=wZDateFull.toDMY();
      break;
    case ZCFMT_MDY:
      wStr=wZDateFull.toMDY();
      break;
    case ZCFMT_MDYHMS:
      wStr=wZDateFull.toMDYhms();
      break;
    case ZCFMT_DMYHMS:
      wStr=wZDateFull.toDMYhms();
      break;
    case ZCFMT_DLocale:
      wStr=wZDateFull.toLocale();
      break;
    case ZCFMT_DUTC:
      wStr=wZDateFull.toUTCGMT();
      break;
    case ZCFMT_Nothing:
    default:
      wStr=wZDateFull.toLocale();
      break;
    }
    return new  QStandardItem(wStr.toCChar());
  } //ZType_ZDateFull

  case ZType_URIString:{
    uriString wString;
    ssize_t wSize = wString._importURF(pPtr);

    if (wString.strlen() > StringDiplayMax){
      utf8VaryingString w1 = cst_OverflowChar8 ;
      w1 += wString.Right(StringDiplayMax).toString();

      wString = w1;
    }

    if (wString.isEmpty()) {
      return new QStandardItem("<empty string>");
    }

    return  new QStandardItem(wString.toCChar());
  }
  case ZType_Utf8VaryingString: {
    utf8VaryingString wString;
    ssize_t wSize = wString._importURF(pPtr);

    if (wString.strlen() > StringDiplayMax){
      utf8VaryingString w1 = cst_OverflowChar8 ;
      w1 += wString.Right(StringDiplayMax).toString();

      wString = w1;
    }

    if (wString.isEmpty()) {
      return new QStandardItem("<empty string>");
    }

    return new QStandardItem(wString.toCChar());
  }


  case ZType_Utf16VaryingString:{
    utf16VaryingString wString;

    ssize_t wSize = wString._importURF(pPtr);
    if (wString.strlen() > StringDiplayMax){
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar16);
    }
    if (wString.isEmpty()) {
      return  new QStandardItem("<empty string>");
    }
    return new QStandardItem(QString((const QChar *)wString.toString()));
  }
  case ZType_Utf32VaryingString:{
    utf32VaryingString wString;
    utf16VaryingString wAdd;

    ssize_t wSize = wString._importURF(pPtr);
    if (wString.strlen() > StringDiplayMax){
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar32);
      wString += wAdd ;
    }
    if (wString.isEmpty()) {
      return new QStandardItem("<empty string>");
    }

    return new QStandardItem(QString((const QChar *)wString.toUtf16().toString()));
  }

  case ZType_Utf8FixedString:{
    utf8VaryingString wString;

    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wStringByteSize;

    pPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    wStringByteSize = size_t (wUnitsCount) * sizeof(utf8_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf8_t* wPtrOut = (utf8_t*)wString.Data;
    utf8_t* wPtrIn = (utf8_t*)pPtr;
    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    pPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString += cst_OverflowChar8 ;
    }
    wOffset += sizeof(ZTypeBase)+sizeof(URF_Capacity_type) +sizeof(URF_UnitCount_type)+ wStringByteSize;


    if (wUnitsCount == 0)
      return  new QStandardItem("<empty string>");
    else
      return new QStandardItem(wString.toCChar());
  } // ZType_Utf8FixedString

    /* for fixed string URF header is different */

  case ZType_Utf16FixedString:{
    utf16VaryingString wString;

    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;


    pPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf16_t);

    /* the whole string must be imported, then possibly truncated afterwards to maximum displayable */

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf16_t* wPtrOut = (utf16_t*)wString.Data;
    utf16_t* wPtrIn = (utf16_t*)pPtr;
    while ( wI-- && *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    pPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit( cst_OverflowChar16);
    }
    wOffset += sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + wStringByteSize ;


    if (wUnitsCount == 0)
      return new QStandardItem("<empty string>");
    else
      return new QStandardItem(QString((const QChar *)wString.toString()));
    break;
  }

  case ZType_Utf32FixedString:{
    utf32VaryingString wString;
    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;

    pPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf32_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf32_t* wPtrOut = (utf32_t*)wString.Data;
    utf32_t* wPtrIn = (utf32_t*)pPtr;

    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    pPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar32);
    }
    wOffset += sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + wStringByteSize ;

    if (wUnitsCount == 0)
      return new QStandardItem("<empty string>");
    else
     return new QStandardItem(QString((const QChar *)wString.toUtf16().toString()));
  }

  case ZType_CheckSum: {
    checkSum wCheckSum;

    wCheckSum._importURF(pPtr);
    wOffset += wCheckSum.getURFSize();

    return createItem(wCheckSum);
  }

  case ZType_MD5: {
    md5 wCheckSum;

    wCheckSum._importURF(pPtr);
    wOffset += wCheckSum.getURFSize();

    return createItem(wCheckSum);
    break;
  }

  case ZType_Blob: {
    uint64_t wDataSize;
    pPtr += sizeof(ZTypeBase);
    _importAtomic(wDataSize,pPtr);

    pPtr += size_t(wDataSize);

    return  new QStandardItem("<Blob content cannot be displayed>");
    break;
  }

  case ZType_bitset: {
    ZBitset wBitset;

    ssize_t wSize=wBitset._importURF(pPtr);
    utf8VaryingString wBS = "<";
    wBS += wBitset.toString();
    wBS += ">";

    wOffset += wBitset.getURFSize();
    return new QStandardItem(wBS.toCChar());

    break;
  }

  case ZType_bitsetFull: {
    pPtr += sizeof(ZTypeBase);
    return new QStandardItem("All bits are set");
  }

  case ZType_Resource: {
    ZResource wValue;
    ssize_t wSize=wValue._importURF(pPtr);

    if (!wValue.isValid()) {
      return new QStandardItem("<Invalid resource>");
    }
    else {

      int wFmt = pCellFormat & ZCFMT_ResMask;
      if (wFmt & ZCFMT_ResSymb) {
        int wi=0;
        for (; wi < Parser->ZEntityList.count();wi++)
          if (Parser->ZEntityList[wi].Value == wValue.Entity)
            break;
        utf8VaryingString wZEntitySymbol = "Unknown entity";
        if (wi < Parser->ZEntityList.count())
          wZEntitySymbol = Parser->ZEntityList[wi].Symbol;
        if (wFmt & ZCFMT_ResStd) {
          wStr.sprintf("ZResource[%s,%ld]",wZEntitySymbol.toCChar(),wValue.id);
        }
        else {
          wStr.sprintf("ZResource[%s,0x%X]",wZEntitySymbol.toCChar(),wValue.id);
        }
      } // ZCFMT_ResSymb
      else {
        if (wFmt & ZCFMT_ResStd) {
          wStr.sprintf("ZResource[%d,%ld]",wValue.Entity,wValue.id);
        }
        else {
          wStr.sprintf("ZResource[0x%X,0x%X]",wValue.Entity,wValue.id);
        }
      }// else

    } // else
    return new  QStandardItem(wStr.toCChar());
  }// ZType_Resource

  default: {
    pPtr += sizeof(ZTypeBase);
    return new QStandardItem("--Unknown data type--");
  }

  }// switch


  return new QStandardItem();
}


void
ZSearchQueryMWd::ParseAndStoreQuery ()
{
  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wQuery = QueryQTe->getText();

  statusbar->clearMessage();

  displayMWn->show();

  wSt=Parser->parse(wQuery,CurrentEntity);
  if (wSt!=ZS_SUCCESS) {
    if (Parser->LastErroredToken!=nullptr) {
      int wLen = Parser->LastErroredToken->Text.strlen();
      if (Parser->LastErroredToken->Type==ZSRCH_STRING_LITERAL)
        wLen += 2 ; // the quotes are elided within token content but are present in the text
      QueryQTe->highlightLengthAtOffset( Parser->LastErroredToken->TokenOffset,wLen);
    }
    statusbar->showMessage(Parser->ZaiErrors::last()->Message());
    return;
  }
  QueryQTe->clear();
} // ZSearchQueryMWd::ExecuteBTnClicked


void
ZSearchQueryMWd::resizeEvent(QResizeEvent* pEvent)
{
  QSize wROld = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (_FResizeInitial) {
    _FResizeInitial=false;
    return;
  }
  QRect wRNew = geometry();

  int wWMargin = (wRNew.width() - wROld.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin =  wRNew.height() - wROld.height();
  int wVH=pEvent->size().height() - wHMargin ;

  QRect wR1 = ExpWidget1->geometry();
  int wWd1gW= wR1.width()+wWMargin;     /* resize only in width */
  ExpWidget1->resize(wWd1gW,wR1.height());  /* expands in width only */

  QRect wR2 = ExpWidget2->geometry();

  int wWdgW= wR2.width()+wWMargin;
  int wWdgH= wR2.height()+wHMargin;

  ExpWidget2->resize(wWdgW,wWdgH);  /* expands in width and height */

}//DicEdit::resizeEvent



void ZSearchQueryMWd::SaveInstructions() {

}


void ZSearchQueryMWd::displayErrorCallBack(const utf8VaryingString& pMessage) {
  if (displayMWn!=nullptr)
    displayMWn->appendText(pMessage);
  else
    fprintf(stderr,pMessage.toCChar());
}

void ZSearchQueryMWd::displayErrorColorCB(uint8_t pSeverity, const utf8VaryingString& pMessage) {
  if (displayMWn==nullptr) {
     fprintf(stderr,pMessage.toCChar());
    return;
  }
  switch (pSeverity) {
  case ZAIES_Text:
    displayMWn->appendText(pMessage);
    return;
  case ZAIES_Info:
    displayMWn->appendTextColor(Qt::blue, pMessage);
    return;
  case ZAIES_Warning:
    displayMWn->appendTextColor(Qt::magenta, pMessage);
    return;
  case ZAIES_Error:
  case ZAIES_Fatal:
    displayMWn->appendTextColor(Qt::red, pMessage);
    return;
  default:
    displayMWn->appendTextColor(Qt::yellow, pMessage);
    return;
  }

}

void ZSearchQueryMWd::setProgressBarMax(int pMax) {
  ProgressBar->setMaximum(pMax);
}

void ZSearchQueryMWd::setProgressBarValue(int pValue) {
  ProgressBar->setValue(pValue);
}
