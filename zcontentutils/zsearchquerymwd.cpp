#include "zsearchquerymwd.h"
//#include "ui_zsearchquerymwd.h"
#include <functional>

#include <zcontent/zcontentcommon/zgeneralparameters.h>
#include <zcontent/zcontentcommon/zcontentconstants.h>

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

#include <QTableView>

#include <zqt/zqtwidget/zqlabel.h>


#include <QProgressBar>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QHeaderView>


#include <zqt/zqtwidget/zqplaintextedit.h>

#include <zqt/zqtwidget/zqtableview.h>

#include "texteditmwn.h"

#include "zhelp.h"

#include <zcontent/zindexedfile/zsearchparser.h>

#include <zcontent/zindexedfile/zsearchparsertype.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zcontentcommon/zresource.h>

#include "zcellformatdlg.h"
#include "zcollectionlist.h"
#include "zmfprogressmwn.h"

#include "zhelp.h"

#include <zcontent/zindexedfile/zsearchentitycontext.h>

namespace zbs {
ZSearchParser* GParser=nullptr;

void
setGParser(ZSearchParser* pParser) {
    GParser=pParser;
}

} // namespace zbs

const long StringDiplayMax = 64;
using namespace zbs;

ZSearchQueryMWd::ZSearchQueryMWd(ZaiErrors *pErrorLog, QWidget *parent) : QMainWindow(parent)
{
    ErrorLog = pErrorLog;
    initLayout();
}

ZSearchQueryMWd::~ZSearchQueryMWd()
{
  if (displayMWn!=nullptr)
    delete displayMWn;
  if (Parser!=nullptr)
    delete Parser;
//  delete ui;
}

ZQLabel* ZSearchQueryMWd::createBiStateZButton(const utf8VaryingString& pIconEnabled,
                                               const utf8VaryingString& pIconDisabled,
                                               const utf8VaryingString& pToolTip,
                                               int wSize,
                                               QWidget* pParent)
{
    uriString  wURIEnabledImg,wURIDisabledImg;
    wURIEnabledImg = GeneralParameters.getIconDirectory();
    wURIEnabledImg.addConditionalDirectoryDelimiter();
    wURIEnabledImg += pIconEnabled;

    wURIDisabledImg = GeneralParameters.getIconDirectory();
    wURIDisabledImg.addConditionalDirectoryDelimiter();
    wURIDisabledImg += pIconDisabled;


    ZQLabel* wButtonLBl=new ZQLabel(pParent);
    wButtonLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    wButtonLBl->setScaledContents( true );
    wButtonLBl->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    wButtonLBl->setMaximumSize(wSize,wSize);
    wButtonLBl->setEnabledPixMap(QPixmap(wURIEnabledImg.toCChar()));
    wButtonLBl->setDisabledPixMap(QPixmap(wURIDisabledImg.toCChar()));
//   wButtonLBl->setPixmap(QPixmap(wURIGenericImg.toCChar()));
    wButtonLBl->setToolTip(pToolTip.toCChar());
    wButtonLBl->enable();
    return wButtonLBl;
}

ZQLabel* ZSearchQueryMWd::createZButton(const utf8VaryingString& pIconName,
                                        const utf8VaryingString& pToolTip,
                                        int wSize,
                                        QWidget* pParent)
{
    uriString  wURIGenericImg;
    wURIGenericImg = GeneralParameters.getIconDirectory();
    wURIGenericImg.addConditionalDirectoryDelimiter();
    wURIGenericImg += pIconName;

    ZQLabel* wButtonLBl=new ZQLabel(pParent);
    wButtonLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    wButtonLBl->setScaledContents( true );
    wButtonLBl->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    wButtonLBl->setMaximumSize(wSize,wSize);
    wButtonLBl->setPixmap(QPixmap(wURIGenericImg.toCChar()));
    wButtonLBl->setToolTip(pToolTip.toCChar());
    return wButtonLBl;
}

void ZSearchQueryMWd::initLayout()
{
    menubar = new QMenuBar(this);
    menubar->setGeometry(QRect(0, 0, 814, 20));

    QHBoxLayout *ButtonBox=nullptr;

    resize(815, 640);
    CentralWidget = new QWidget(this);
    setCentralWidget(CentralWidget);

    ForBackWDg=new QWidget(centralWidget());
    ForBackWDg->setGeometry(QRect(0 , 0 , 801, 50));
    ForBackWDg->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QHBoxLayout* hl10 = new QHBoxLayout;
    ForBackWDg->setLayout(hl10);

    int wWidgetSize=LBlSize;
    int wWidth = ForBackWDg->geometry().width() - WidthMargin ;
    int wCount = wWidth / (wWidgetSize + 1);
    wWidth = wWidth - (wWidgetSize*5);
    int wHeigth= ForBackWDg->geometry().height()-2;
/*
    GlobalCellFmtLBl = createBiStateZButton("pen-simple.gif","pen-simple-grey.gif","Column global format",wWidgetSize,ForBackWDg);
    GlobalCellFmtLBl->disable();
*/
    GlobalCellFmtLBl = createZButton("pen-simple.gif","Column global format",wWidgetSize,ForBackWDg);
    GlobalCellFmtLBl->setVisible(false);

    hl10->addWidget(GlobalCellFmtLBl);

    QObject::connect(GlobalCellFmtLBl,&ZQLabel::clicked, this, &ZSearchQueryMWd::TableHeaderCornerClicked);

    TextClearLBl = createZButton("brandnew.gif","Clear text log window",wWidgetSize,ForBackWDg);
    hl10->addWidget(TextClearLBl);
//    TextClearLBl->setToolTip("Clear text log window");
    QObject::connect(TextClearLBl,&ZQLabel::clicked, this, &ZSearchQueryMWd::TextClearLBlClicked);

    EntityListLBl = createZButton("magnifyingglass.png","List all active entities",wWidgetSize,ForBackWDg);
    hl10->addWidget(EntityListLBl);
//    EntityListLBl->setToolTip("List all active entities");
    QObject::connect(EntityListLBl,&ZQLabel::clicked, this, &ZSearchQueryMWd::EntityListLBlClicked);

 //   hl10->addItem(wSpacer0);
    QSpacerItem* wSpacer0= new QSpacerItem(wWidth, wHeigth, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hl10->addItem(wSpacer0);

    EraseLBl = createZButton("errorcross.png","Clear instruction text window",wWidgetSize,ForBackWDg);
    hl10->addWidget(EraseLBl);


    BackwardLBl = createZButton("backward.gif","Previous instruction",wWidgetSize,ForBackWDg);
    hl10->addWidget(BackwardLBl);
//    BackwardLBl->setToolTip("Previous instruction");
    QObject::connect(BackwardLBl,&ZQLabel::clicked, this, &ZSearchQueryMWd::BackwardClicked);

    ExecuteLBl = createZButton("check.gif","Execute current instruction",wWidgetSize,ForBackWDg);
    hl10->addWidget(ExecuteLBl);
//    ExecuteLBl->setToolTip("Execute current instruction");
    QObject::connect(ExecuteLBl,&ZQLabel::clicked, this, &ZSearchQueryMWd::ExecuteClicked);

    ForwardLBl = createZButton("forward.gif","Next instruction",wWidgetSize,ForBackWDg);
    hl10->addWidget(ForwardLBl,0,Qt::AlignRight);
//    ForwardLBl->setToolTip("Next instruction");
    QObject::connect(ForwardLBl,&ZQLabel::clicked, this, &ZSearchQueryMWd::ForwardClicked);



    int wY= ForBackWDg->size().height()+HeightMargin;
    QueryQTe = new ZQPlainTextEdit(centralWidget());
    QueryQTe->setGeometry(QRect(WidthMargin, wY, 801-WidthMargin, 91));

    QObject::connect(EraseLBl,&ZQLabel::clicked, QueryQTe, &ZQPlainTextEdit::clear);

    wY += QueryQTe->size().height() + HeightMargin;

    ExpWidget2= new QWidget(centralWidget());
    ExpWidget2->setGeometry(0,wY,800,470);

    QGridLayout* hl20 = new QGridLayout;
    QHBoxLayout* hl21 = new QHBoxLayout;

    QVBoxLayout* vl1 = new QVBoxLayout;
    ExpWidget2->setLayout(vl1);

    vl1->addLayout(hl20);


    QLabel* QL10 = new QLabel("Entity",ExpWidget2);
    EntityNameLBl = new QLabel(ExpWidget2);
    EntityNameLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    EntityNameLBl->setLineWidth(2);

    QLabel* QL11 = new QLabel("Type",ExpWidget2);
    EntityTypeLBl = new QLabel(ExpWidget2);
    EntityTypeLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    EntityTypeLBl->setLineWidth(2);

    QLabel* QL12 = new QLabel("Count",ExpWidget2);
    RecordCountLBl = new QLabel(ExpWidget2);
    RecordCountLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    RecordCountLBl->setLineWidth(2);
    QLabel* QL13 = new QLabel("Last status",ExpWidget2);
    LastStatusLBl = new QLabel(ExpWidget2);
    LastStatusLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    LastStatusLBl->setLineWidth(2);
    LastStatusLBl->setAutoFillBackground(true);

//    QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hl20->addWidget(QL10,0,0,Qt::AlignRight);
    hl20->addWidget(EntityNameLBl,0,1);
    hl20->addWidget(QL11,0,2,Qt::AlignRight);
    hl20->addWidget(EntityTypeLBl,0,3);
    hl20->addWidget(QL12,0,4,Qt::AlignRight);
    hl20->addWidget(RecordCountLBl,0,5);
    hl20->addWidget(QL13,0,6,Qt::AlignRight);
    hl20->addWidget(LastStatusLBl,0,7);

    vl1->addLayout(hl21);

    ResultTBv = new ZQTableView(ExpWidget2);
    hl21->addWidget(ResultTBv);

    setMenuBar(menubar);
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    QActionGroup* wMainAGp=new QActionGroup(menubar);

    /*----------Leading quit icon--------------*/

    uriString wQuitImg;
    wQuitImg = GeneralParameters.getIconDirectory();
    wQuitImg.addConditionalDirectoryDelimiter();
    wQuitImg += "system-shutdown.png";
    QIcon wQuitICn(wQuitImg.toCChar());

    QuitIconQAc = new QAction("",menubar);
    QuitIconQAc->setIcon(wQuitICn);

    menubar->addAction(QuitIconQAc);
    wMainAGp->addAction(QuitIconQAc);


    /* other menu bar  menus */

    QMenu* MainMenuMEn = new QMenu("General",this);
    menubar->addMenu(MainMenuMEn);

    WizardQAc = new QAction("Query wizard" );
    MainMenuMEn->addAction(WizardQAc);
    wMainAGp->addAction(WizardQAc);

    MainMenuMEn->addSeparator();

    ExecQAc=new QAction("Execute (F9)");
    ClearLogQAc= new QAction("Clear log");
    QuitQAc=new QAction("Quit");
    SaveInstructionsQAc=new QAction("Save instuctions");


    MainMenuMEn->addAction(ExecQAc);
    MainMenuMEn->addAction(SaveInstructionsQAc);
    MainMenuMEn->addAction(ClearLogQAc);
    MainMenuMEn->addAction(QuitQAc);

    wMainAGp->addAction(QuitQAc);
    wMainAGp->addAction(ExecQAc);
    wMainAGp->addAction(ClearLogQAc);
    wMainAGp->addAction(SaveInstructionsQAc);

    /* not in action group */

    QMenu*    OptionsQMe=new QMenu("Options",this);
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
    QObject::connect(wMainAGp,&QActionGroup::triggered,this,&ZSearchQueryMWd::MenuTriggered);

    /* table corner */
    ResultTBv->SelectAllCallBack = std::bind(&ZSearchQueryMWd::TableHeaderCornerClicked, this);

    /* set call back for function keys */
    QueryQTe->TrappedKeyPressCallBack =  std::bind(&ZSearchQueryMWd::KeyFiltered, this,placeholders::_1,placeholders::_2);
    QueryQTe->UntrappedKeyPressCallBack =  std::bind(&ZSearchQueryMWd::KeyUnFiltered, this,placeholders::_1);

//    displayMWn = new textEditMWn(this,TEOP_NoFileLab|TEOP_CloseBtnHide);
    displayMWn = new textEditMWn(this,TEOP_NoFileLab,&displayMWn);
    displayMWn->setWindowTitle("Search parser output");

    Parser = new ZSearchParser;
    setGParser(Parser);

    Parser->ErrorLog.setStoreMinSeverity(ZAIES_Warning);
    Parser->ErrorLog.setAutoPrintOn(ZAIES_Text);

    Parser->setEntityDisplayCallBack(std::bind(&ZSearchQueryMWd::DisplayEntity, this,placeholders::_1));

    displayMWn->registerDisplayColorCallBack(&Parser->ErrorLog);
    displayMWn->show();

    uriString wXMLSymbol = GeneralParameters.getParamDirectory();
    wXMLSymbol.addConditionalDirectoryDelimiter();
    wXMLSymbol += __SEARCHPARSER_SYMBOL_FILE__ ;

    uriString wXMLZEntity = GeneralParameters.getParamDirectory();
    wXMLZEntity.addConditionalDirectoryDelimiter();
    wXMLZEntity += __SEARCHPARSER_ZENTITY_FILE__ ;

    uriString wHistory = GeneralParameters.getParamDirectory();
    wHistory.addConditionalDirectoryDelimiter();
    wHistory += __QUERY_HISTORY_FILE__ ;

    uriString wParams = GeneralParameters.getParamDirectory();;
    wParams.addConditionalDirectoryDelimiter();
    wParams += __SEARCHPARSER_PARAMS_FILE__ ;

    Parser->setup(wXMLSymbol,wXMLZEntity,wParams,wHistory);

} // initLayout


void
ZSearchQueryMWd::help()
{
    if (HelpMWn==nullptr) {
        HelpMWn = new zbs::ZHelp("Query help",this);
        HelpMWn->setCloseCallBack(std::bind(&ZSearchQueryMWd::helpClose,this,std::placeholders::_1));
        HelpMWn->setup("query",ErrorLog);
    }
//    HelpMWn->setHtmlSource("file://home/gerard/Development/zbasetools/zqt/help/generalindex.html");

    HelpMWn->show();

    return;
}
void
ZSearchQueryMWd::helpClose(QEvent*)
{
    if (HelpMWn==nullptr) {
        return;
    }
    HelpMWn=nullptr;
    return;
}


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
        ExecuteClicked();
        return true;
    }
    if(pKey == Qt::Key_F10) {
        Parser->showEntityList();
        return true;
    }
    if(pKey == Qt::Key_F11) {
        Parser->showZEntities();
        return true;
    }
    if(pKey == Qt::Key_F1) {
        help();
        return true;
    }
    return false;
}//KeyFiltered

void
ZSearchQueryMWd::KeyUnFiltered( QKeyEvent *pEvent)
{

    if( pEvent->key() == Qt::Key_Up) {
        if (QueryQTe->textCursor().atStart()) {
            recallBackward();
            QTextCursor wTC=QueryQTe->textCursor();
            wTC.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
//            wTC.setPosition(0,QTextCursor::MoveAnchor);
            QueryQTe->setTextCursor(wTC);
        }
    }
    if( pEvent->key() == Qt::Key_Down) {
        QTextCursor wTC=QueryQTe->textCursor();
        if (wTC.atEnd())
            recallForward();
        return ;
    }
    if( pEvent->key() == Qt::Key_Enter) {
        ExecuteClicked();
        return ;
    }
    if( pEvent->key() == Qt::Key_F9) {
        ExecuteClicked();
        return ;
    }
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
ZSearchQueryMWd::ForwardClicked()
{
    recallForward();
}
void
ZSearchQueryMWd::BackwardClicked()
{
    recallBackward();
}

void
ZSearchQueryMWd::TextClearLBlClicked()
{

    ClearLog();
    if (ResultTBv==nullptr)
        return;
    if (ResultTBv->ItemModel==nullptr)
        return;
    ResultTBv->ItemModel->removeColumns(0,ResultTBv->ItemModel->columnCount());
    GlobalCellFmtLBl->setVisible(false);

    EntityTypeLBl->setText("");
    EntityNameLBl->setText("");
    RecordCountLBl->setText("");
}

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
  if ((pAction==QuitQAc)||(pAction==QuitIconQAc))
    {
    QuitLBlClicked();
    return;
  }
  if (pAction == WizardQAc){
      ExecuteClicked();
      return;
  }

  if (pAction == ExecQAc){
    ExecuteClicked();
    return;
  }
  if (pAction == SaveInstructionsQAc){
    SaveInstructions();
    return;
  }
}
/*
void
ZSearchQueryMWd::QuitBTnClicked (bool pChecked)
{
  hide();
  this->deleteLater();
}
*/

void
ZSearchQueryMWd::QuitLBlClicked ()
{
    hide();
    this->deleteLater();
}


void
ZSearchQueryMWd::ClearLog ()
{
    if (Parser!=nullptr)
        Parser->clearErrors();
    if (displayMWn!=nullptr)
        displayMWn->clear();
}

void
ZSearchQueryMWd::EntityListLBlClicked ()
{
  ZCollectionList* wColList=new ZCollectionList(Parser,this);
  int wRet=wColList->exec();

  if (wRet==QDialog::Rejected)
    return;

  CurrentEntity = wColList->get();
  wColList->deleteLater();

  DisplayCurrentEntity();
} // EntityListLBlClicked

void
ZSearchQueryMWd::ExecuteClicked ()
{
  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wQuery = QueryQTe->getText();
  statusbar->clearMessage();

  displayMWn->show();

  ZSearchContext wSearchContext = Parser->buildContext();
  LastStatusLBl->setText("");

  wSt=Parser->parse(wQuery,wSearchContext);

  if (wSt==ZS_HELP)  {
      help();
      return ;
  }

  if (LabelGoodPalette == QPalette()) {
      LabelGoodPalette=LabelBadPalette=LabelDefaultPalette = LastStatusLBl->palette();
      LabelGoodPalette.setColor(QPalette::Window, Qt::white);
      LabelGoodPalette.setColor(QPalette::WindowText, Qt::darkGreen);
      LabelBadPalette.setColor(QPalette::Window, Qt::white);
      LabelBadPalette.setColor(QPalette::WindowText, Qt::red);
  }

  if ((wSearchContext.Status==ZS_SUCCESS)&&(wSearchContext.InstructionType & ZSITP_ToBeExecuted)) {

      ZMFProgressMWn* wProgress = new ZMFProgressMWn("Query progress", this,ZPTP_TimedClose | ZPTP_HasCloseBtn);
      Parser->setProgressSetupCallback(std::bind(&ZProgressBase::advanceSetupCallBack,wProgress,std::placeholders::_1,std::placeholders::_2));
      Parser->setProgressCallback(std::bind(&ZProgressBase::advanceCallBack,wProgress,std::placeholders::_1,std::placeholders::_2));
      wProgress->show();

      wSt=Parser->execute(wSearchContext);
/*
       if (wSearchContext.TargetEntity != nullptr) {
        wSt=DisplayEntity(wSearchContext.TargetEntity);
           if (wSt==ZS_SUCCESS) {
               GlobalCellFmtLBl->setVisible(true);
           }
      }
*/
      wProgress->setDone((wSt!=ZS_SUCCESS)&&(wSt!=ZS_OUTBOUNDHIGH));
  }


  if ((wSt==ZS_SUCCESS)||(wSt==ZS_OUTBOUNDHIGH))
      LastStatusLBl->setPalette(LabelGoodPalette);
  else
      LastStatusLBl->setPalette(LabelBadPalette);

  LastStatusLBl->setText(decode_ZStatus(wSt));

  if (wSt!=ZS_SUCCESS) {
    if (wSearchContext.LastErroredToken!=nullptr) {
      int wLen = wSearchContext.LastErroredToken->Text.strlen();
      if (wSearchContext.LastErroredToken->Type==ZSRCH_STRING_LITERAL)
        wLen += 2 ; // the quotes are elided within token content but are present in the text
      QueryQTe->highlightLengthAtOffset( wSearchContext.LastErroredToken->TokenOffset,wLen);
    }
    if (Parser->ErrorLog.hasSomething())
        statusbar->showMessage(Parser->ErrorLog.last()->Message(),30000); /* timout is expressed in milliseconds */
    return ;
  } // if (wSt!=ZS_SUCCESS)

//  if (wSearchContext.hasMessage()) {
//      statusbar->showMessage(wSearchContext.Message.toCChar(),30000); /* timout is expressed in milliseconds */
//  }

  /*
  if (CurrentEntity!=nullptr)
    DisplayCurrentEntity();
  */
  QueryQTe->clear();

} // ZSearchQueryMWd::ExecuteBTnClicked


ZStatus ZSearchQueryMWd::DisplayEntity (ZSearchContext& pContext)
{
  utf8VaryingString wStr;
  ZStatus wSt=ZS_SUCCESS;

  std::shared_ptr <ZSearchEntityContext> wSEC=SECList.getSEC(pContext.SourceEntity);
  if (wSEC==nullptr) {
      wSEC = ZSearchEntityContext::newEntityContext(pContext.SourceEntity);
  }

  QList<QStandardItem*> wRow;

  if (pContext.SourceEntity==nullptr) {
      statusBar()->showMessage("No entity defined ",30);
      return ZS_NULLPTR ;
  }

  CurrentEntity = pContext.SourceEntity;

  ZSearchDictionary* wDic = pContext.SourceEntity->getDic();

/*
  if (ResultTBv==nullptr) {
    ResultTBv = new ZQTableView(this);
  }
*/
  if (!ResultTBv->hasModel())
    ResultTBv->newModel(wDic->count());
  else {
    ResultTBv->ItemModel->clear();
    ResultTBv->setColumnCount(wDic->count());
    ResultTBv->ItemModel->setColumnCount(wDic->count());
  }

  ResultTBv->verticalHeader()->setVisible(true);

  bool wHasCorner = ResultTBv->isCornerButtonEnabled();


  for (int wi=0;wi < wDic->count();wi++) {
    ResultTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(wDic->TabConst(wi).getFieldName().toCChar()));
  }

  EntityNameLBl->setText(pContext.SourceEntity->getEntityName().toCChar());

  if (pContext.SourceEntity->isCollection()) {
    EntityTypeLBl->setText("Collection");
  }
  else if (pContext.SourceEntity->isFile()) {
    EntityTypeLBl->setText("File");
  } else {
    EntityTypeLBl->setText("Unknown(invalid) type");
    RecordCountLBl->setText("---");
  }
  wStr.sprintf("%ld",pContext.SourceEntity->getCount());
  RecordCountLBl->setText(wStr.toCChar());

  long wRank=0;
  zaddress_type wAddress;

  ZArray<URFField> wFieldList;

  wSt=pContext.SourceEntity->getFirst(*wSEC,wAddress);


  while (wSt == ZS_SUCCESS) {
      wRow=DisplayOneLine(wSEC->_URFParser.URFFieldList);
    ResultTBv->ItemModel->appendRow(wRow);
    wRank++;
    wSt=pContext.SourceEntity->getNext(*wSEC,wAddress);
  }
  if (wSt==ZS_OUTBOUNDHIGH)
      wSt=ZS_SUCCESS;

  LastStatusLBl->setText(decode_ZStatus(wSt));


  ResultTBv->resizeColumnsToContents();
  ResultTBv->resizeRowsToContents();

  wHasCorner = ResultTBv->isCornerButtonEnabled();

  _DBGPRINT("Table view <%s> corner button enabled\n",wHasCorner?"has":"does not have")
 // ResultTBv->setVisible(true);
//  ResultTBv->repaint();
  ResultTBv->show();
  return wSt;
} // ZSearchQueryMWd::DisplayEntity

ZStatus ZSearchQueryMWd::DisplayEntityRaw (std::shared_ptr<ZSearchEntity> &pEntity)
{
    utf8VaryingString wStr;
    ZStatus wSt=ZS_SUCCESS;

    QList<QStandardItem*> wRow;

    if (pEntity==nullptr) {
        statusBar()->showMessage("No entity defined ",30);
        return ZS_NULLPTR ;
    }


    std::shared_ptr <ZSearchEntityContext> wSEC=SECList.getSEC(pEntity);
    if (wSEC==nullptr) {
        wSEC = ZSearchEntityContext::newEntityContext(pEntity);
    }


    ZSearchDictionary* wDic = pEntity->getDic();

    if (!ResultTBv->hasModel())
        ResultTBv->newModel(wDic->count());
    else {
//        ResultTBv->ItemModel->clear();
        ResultTBv->setColumnCount(wDic->count());
        ResultTBv->ItemModel->setColumnCount(wDic->count());
    }

    ResultTBv->setCornerButtonEnabled(true);

    ResultTBv->verticalHeader()->setVisible(true);

    for (int wi=0;wi < wDic->count();wi++) {
        ResultTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(wDic->TabConst(wi).getFieldName().toCChar()));
    }

    EntityNameLBl->setText(pEntity->getEntityName().toCChar());

    if (pEntity->isCollection()) {
        EntityTypeLBl->setText("Collection");
    }
    else if (pEntity->isFile()) {
        EntityTypeLBl->setText("File");
    } else {
        EntityTypeLBl->setText("Unknown(invalid) type");
        RecordCountLBl->setText("---");
    }
    wStr.sprintf("%ld",pEntity->_BaseEntity->getCount());
    RecordCountLBl->setText(wStr.toCChar());

    long wRank=0;
    zaddress_type wAddress;
    const unsigned char* wRecordPtr=nullptr;
    const unsigned char* wRecordPtrEnd=nullptr;

    ResultTBv->setCornerButtonEnabled(true);
    wSt=pEntity->getFirst(*wSEC,wAddress);

    while (wSt == ZS_SUCCESS) {

        wRecordPtr = wSEC->_URFParser.Record.Data;
        wRecordPtrEnd = wSEC->_URFParser.Record.Data + wSEC->_URFParser.Record.Size;
        wRow=DisplayOneRawLine(wSEC->_URFParser.Record);
        ResultTBv->ItemModel->appendRow(wRow);
        wRank++;
        wSt=pEntity->getNext(*wSEC,wAddress);
    }
    if (wSt==ZS_OUTBOUNDHIGH)
        wSt=ZS_SUCCESS;

    ResultTBv->setCornerButtonEnabled(true);
/*
    uriString wURIGenericImg = GeneralParameters.getIconDirectory();
    wURIGenericImg.addConditionalDirectoryDelimiter();
    wURIGenericImg += "crossblue.gif";
    ZQLabel* wResultCornerLBl=new ZQLabel(ForBackWDg);
    wResultCornerLBl->setPixmap(QPixmap(wURIGenericImg.toCChar()));
    ResultTBv->setCornerWidget(wResultCornerLBl);


    QObject::connect(wResultCornerLBl,&ZQLabel::clicked, this, &ZSearchQueryMWd::TableHeaderCornerClicked);
*/

    LastStatusLBl->setText(decode_ZStatus(wSt));


    ResultTBv->resizeColumnsToContents();
    ResultTBv->resizeRowsToContents();


    // ResultTBv->setVisible(true);
    //  ResultTBv->repaint();
    ResultTBv->show();
    return wSt;
} // ZSearchQueryMWd::DisplayEntityRaw

void ZSearchQueryMWd::DisplayCurrentEntity ()
{
    ZSearchContext wContext = Parser->buildContext();
    wContext.SourceEntity = CurrentEntity;
    wContext.InstructionType = ZSITP_Display|ZSITP_All;
    wContext.Number = -1 ;
    DisplayEntity(wContext);
//    DisplayEntity(CurrentEntity,ZSITP_Display|ZSITP_All,1);
    return;
} // ZSearchQueryMWd::DisplayCurrentEntity

QList<QStandardItem *> ZSearchQueryMWd::DisplayOneRawLine( ZDataBuffer &pRecord)
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
  ZSearchDictionary& wDic = CurrentEntity->BuildDic;
  ZCFMT_Type wCellFormat ;

  while (wPtr < wPtrEnd) {
    if (!wPresence.isFullBitset() && (wFieldRank >= int(wPresence.count())))
      break;
    if (wPresence[wFieldRank]) {
      wCellFormat = wDic[wFieldRank].getCellFormat() |  wDic.getGlobalCellFormat();
      wRow << DisplayOneURFFieldFromSurface(wPtr,wPtrEnd,wCellFormat);
    }
    else
      wRow << new QStandardItem("missing");
    wFieldRank++;
  } // while

  return wRow;
} //DisplayOneLine

QList<QStandardItem *> ZSearchQueryMWd::DisplayOneLine( ZArray<URFField> &pFields)
{
    utf8VaryingString wStr;
    QList<QStandardItem*> wRow;
    ZSearchDictionary& wDic = CurrentEntity->BuildDic;
    ZCFMT_Type wCellFormat ;
    for (int wi=0; wi < pFields.count(); wi++) {
        wCellFormat = wDic[wi].getCellFormat() |  wDic.getGlobalCellFormat();
        wRow << createItem(pFields[wi].displayFmt(wCellFormat));
    }

    return wRow;
} //DisplayOneLine



void
ZSearchQueryMWd::TableHeaderCornerClicked()
{
  if (CurrentEntity==nullptr)
    return;
  if (!GlobalCellFmtLBl->isVisible())
      return;
  ZCFMT_Type wCellFormat=CurrentEntity->BuildDic.getGlobalCellFormat();

  ZCellFormatDLg* ZCellFormat= new ZCellFormatDLg(this);
  ZCellFormat->setup(wCellFormat);
  ZCellFormat->setAppliance("All columns");
  ZCellFormat->exec();

  ZCFMT_Type wNewCellFormat=ZCellFormat->get();
  if (wNewCellFormat!=wCellFormat) {
    CurrentEntity->BuildDic.setGlobalCellFormat(wNewCellFormat);
    DisplayCurrentEntity();
  }
}

void
ZSearchQueryMWd::TableHeaderClicked(int pLogicalIndex)
{
    if (pLogicalIndex<0)
        return;
  ZCFMT_Type wCellFormat=CurrentEntity->BuildDic[pLogicalIndex].getCellFormat();
  ZCellFormatDLg* ZCellFormat= new ZCellFormatDLg(this);
  ZCellFormat->setup(wCellFormat);
  utf8VaryingString wStr;
  wStr.sprintf("Column %d <%s>",pLogicalIndex+1,CurrentEntity->BuildDic[pLogicalIndex].getFieldName().toString());
  ZCellFormat->setAppliance(wStr.toCChar());

  ZCellFormat->exec();

  ZCFMT_Type wNewCellFormat=ZCellFormat->get();
  if (wNewCellFormat!=wCellFormat) {
    CurrentEntity->BuildDic[pLogicalIndex].setCellFormat(wNewCellFormat);
    DisplayCurrentEntity();
  }
}


QStandardItem *ZSearchQueryMWd::DisplayOneURFFieldFromSurface(const unsigned char *&pPtr,
                                                              const unsigned char *wPtrEnd,
                                                              ZCFMT_Type pCellFormat)
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
        for (; wi < ZEntitySymbolList.count();wi++)
          if (ZEntitySymbolList[wi].Value == wValue.Entity)
            break;
        utf8VaryingString wZEntitySymbol = "Unknown entity";
        if (wi < ZEntitySymbolList.count())
          wZEntitySymbol = ZEntitySymbolList[wi].Symbol;
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
} // DisplayOneURFFieldFromSurface


void
ZSearchQueryMWd::ParseAndStoreQuery ()
{
  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wQuery = QueryQTe->getText();

  statusbar->clearMessage();

  displayMWn->show();

  ZSearchContext wSearchContext = Parser->buildContext();
  wSt=Parser->parse(wQuery,wSearchContext);

  if (wSt!=ZS_SUCCESS) {
    if (wSearchContext.LastErroredToken!=nullptr) {
      int wLen = wSearchContext.LastErroredToken->Text.strlen();
      if (wSearchContext.LastErroredToken->Type==ZSRCH_STRING_LITERAL)
        wLen += 2 ; // the quotes are elided within token content but are present in the text
      QueryQTe->highlightLengthAtOffset( wSearchContext.LastErroredToken->TokenOffset,wLen);
    }
    statusbar->showMessage(Parser->ErrorLog.last()->Message());
    return;
  }
//  if (wSearchContext.hasMessage())
//      statusbar->showMessage(wSearchContext.Message.toCChar());
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

  int wWMargin = (pEvent->size().width() - pEvent->oldSize().width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin =  pEvent->size().height() - pEvent->oldSize().height();
  int wVH=pEvent->size().height() - wHMargin ;
  QRect wR2 = geometry();
  int wWdgW= wR2.width()+wWMargin;
  int wWdgH= wR2.height()+wHMargin;

#ifdef __COMMENT__
  QRect wR1 = Centralwidget->geometry();
  int wWd1gW= wR1.width()+wWMargin;     /* resize only in width */
  Centralwidget->resize(wWd1gW,wR1.height());  /* expands in width only */
#endif // __COMMENT__
/*

  CentralWidget->resize(wWdgW,wWdgH);  // expands in width and height
*/
  QRect wR3 = ForBackWDg->geometry();
  wWdgW= wR3.width()+wWMargin;
  ForBackWDg->setGeometry(QRect(0, 0, wWdgW, wR3.height())); /* expands in width only */
/*
  int wX = ForBackWDg->size().width() - LBlSize -LBlSize;
  ForwardLBl->setGeometry(QRect(wX,HeightMargin,LBlSize,LBlSize));
  wX -= (ForwardLBl->size().width()*2);
  ExecuteLBl->setGeometry(QRect(wX,HeightMargin,LBlSize,LBlSize));
  wX -= (ExecuteLBl->size().width()*2);
  BackwardLBl->setGeometry(QRect(wX,HeightMargin,LBlSize,LBlSize));
*/
  int wY= ForBackWDg->size().height()+HeightMargin;
  QRect wR4 = QueryQTe->geometry();
  wWdgW= wR4.width()+wWMargin;
  QueryQTe->setGeometry(QRect(5, wY, wWdgW, 91)); /* expands in width only */

  wY += QueryQTe->size().height() + HeightMargin;

  QRect wR5 = ExpWidget2->geometry();
  wWdgW= wR5.width()+wWMargin;
  wWdgH= wR5.height()+wHMargin;
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

