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

#include <QProgressBar>


#include <zqt/zqtwidget/zqplaintextedit.h>

#include <zqt/zqtwidget/zqtableview.h>

#include "texteditmwn.h"

#include <zcontent/zindexedfile/zsearchparser.h>

#include <zcontent/zindexedfile/zsearchparsertype.h>

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
  QHBoxLayout* hl2 = new QHBoxLayout;
  ExpWidget2->setLayout(vl1);

  vl1->addLayout(hl2);

  ResultTBv = new ZQTableView(ExpWidget2);
  ResultTBv->setObjectName(QString::fromUtf8("ResultTBv"));

  hl2->addWidget(ResultTBv);


  ButtonBox = new QHBoxLayout;
  ButtonBox->setObjectName(QString::fromUtf8("ButtonBox"));
  ButtonBox->setContentsMargins(0, 0, 0, 0);
  horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

  ButtonBox->addItem(horizontalSpacer);
  vl1->addLayout(ButtonBox);


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

//  ProgressBar->setVisible(false);

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

  QObject::connect(QuitBTn,&QPushButton::clicked,this,&ZSearchQueryMWd::QuitBTnClicked);
  QObject::connect(ExecuteBTn,&QPushButton::clicked,this,&ZSearchQueryMWd::ExecuteBTnClicked);
  QObject::connect(ClearBTn,&QPushButton::clicked,this,&ZSearchQueryMWd::ClearBTnClicked);

  QObject::connect(wMainAGp,&QActionGroup::triggered,this,&ZSearchQueryMWd::MenuTriggered);


  /* set call back for function keys */
  QueryQTe->TrappedKeyPressCallBack =  std::bind(&ZSearchQueryMWd::KeyFiltered, this,placeholders::_1,placeholders::_2);
  QueryQTe->UntrappedKeyPressCallBack =  std::bind(&ZSearchQueryMWd::KeyUnFiltered, this,placeholders::_1);

  displayMWn = new textEditMWn(this,TEOP_NoFileLab|TEOP_CloseBtnHide);
  displayMWn->setWindowTitle("Search parser output");

  Parser = new ZSearchParser;

  Parser->setDisplayCallback(std::bind(&ZSearchQueryMWd::displayErrorCallBack, this,placeholders::_1));

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

  insertHighlightedText(wInst);
}

void
ZSearchQueryMWd::recallBackward()
{
  utf8VaryingString wInst;
  ZStatus wSt=Parser->History.getPrevious(wInst);
  if (wSt!=ZS_SUCCESS)
    return;

  insertHighlightedText(wInst);
}

/*
void
ZSearchQueryMWd::recallForwardOld()
{
  if (Parser->InstructionLog.count()==0)
    return;
  if (IndexRecall < 0)
    IndexRecall = Parser->InstructionLog.lastIdx();
  IndexRecall++;
  if (IndexRecall > Parser->InstructionLog.lastIdx()) {
    IndexRecall = Parser->InstructionLog.lastIdx();
    return;
  }

  insertHighlightedText(Parser->InstructionLog[IndexRecall].Phrase);
}

void
ZSearchQueryMWd::recallBackward()
{
  if (Parser->InstructionLog.count()==0)
    return;
  if (IndexRecall < 0)
    IndexRecall = Parser->InstructionLog.count();
  if (IndexRecall>0)
    IndexRecall--;


  insertHighlightedText(Parser->InstructionLog[IndexRecall].Phrase);
}

*/

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
ZSearchQueryMWd::ExecuteBTnClicked (bool pChecked)
{
  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wQuery = QueryQTe->getText();

  statusbar->clearMessage();

  displayMWn->show();
//  displayMWn->clear();

  wSt=Parser->parse(wQuery);
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


void ZSearchQueryMWd::setProgressBarMax(int pMax) {
  ProgressBar->setMaximum(pMax);
}

void ZSearchQueryMWd::setProgressBarValue(int pValue) {
  ProgressBar->setValue(pValue);
}
