#include "texteditmwn.h"
#include "ui_texteditmwn.h"

#include <ztoolset/uristring.h>
#include <ztoolset/utfvaryingstring.h>
#include <QtWidgets/QPlainTextEdit>

#include <zcontent/zcontentutils/zexceptiondlg.h>

#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory()

#include <zcontentcommon/zgeneralparameters.h>

#include <QTextCursor>
#include <QTextBlock>
#include <QWidget>

#include <QAction>
#include <QMenu>
#include <QActionGroup>

#include <QFileDialog>

#include <zcontentcommon/zgeneralparameters.h>
#include <ztoolset/zaierrors.h>

void
//textEditMWn::_init(uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack)) {
textEditMWn::_init(uint32_t pOptions) {

    if (objectName().isEmpty())
        setObjectName("textEditMWn");
    resize(813, 623);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setSizeIncrement(QSize(3, 3));
    centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralwidget");
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
    centralwidget->setSizePolicy(sizePolicy1);
    verticalLayoutWidget = new QWidget(centralwidget);
    verticalLayoutWidget->setObjectName("verticalLayoutWidget");
    verticalLayoutWidget->setGeometry(QRect(0, 10, 811, 571));
    verticalLayout = new QVBoxLayout(verticalLayoutWidget);
    verticalLayout->setObjectName("verticalLayout");
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName("horizontalLayout");


    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName("horizontalLayout_2");
    searchBTn = new QPushButton(verticalLayoutWidget);
    searchBTn->setObjectName("searchBTn");

    uriString wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "magnifyingglass.png";

    QIcon icon;
    icon.addFile(QString::fromUtf8(wIcon.toCChar()), QSize(), QIcon::Normal, QIcon::Off);
    searchBTn->setIcon(icon);

    horizontalLayout_2->addWidget(searchBTn);

    searchLEd = new QLineEdit(verticalLayoutWidget);
    searchLEd->setObjectName("searchLEd");

    horizontalLayout_2->addWidget(searchLEd);


    horizontalLayout->addLayout(horizontalLayout_2);

    searchMainBTn = new QPushButton(verticalLayoutWidget);
    searchMainBTn->setObjectName("searchMainBTn");
#if QT_CONFIG(tooltip)
    searchMainBTn->setToolTip(QString::fromUtf8("set search on/off"));
#endif // QT_CONFIG(tooltip)

    wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "strigi.png";

    QIcon icon1;
    icon1.addFile(QString::fromUtf8(wIcon.toCChar()), QSize(), QIcon::Normal, QIcon::Off);
    searchMainBTn->setIcon(icon1);

    horizontalLayout->addWidget(searchMainBTn);

    filterBTn = new QPushButton(verticalLayoutWidget);
    filterBTn->setObjectName("filterBTn");

    wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "funel.png";

    QIcon icon2;
    icon2.addFile(QString::fromUtf8(wIcon.toCChar()), QSize(), QIcon::Normal, QIcon::Off);
    filterBTn->setIcon(icon2);

    horizontalLayout->addWidget(filterBTn);

    lineNumbersBTn = new QPushButton(verticalLayoutWidget);
    lineNumbersBTn->setObjectName("lineNumbersBTn");

    horizontalLayout->addWidget(lineNumbersBTn);

    wrapBTn = new QPushButton(verticalLayoutWidget);
    wrapBTn->setObjectName("wrapBTn");

    horizontalLayout->addWidget(wrapBTn);

    closeBTn = new QPushButton(verticalLayoutWidget);
    closeBTn->setObjectName("closeBTn");

    horizontalLayout->addWidget(closeBTn);


    verticalLayout->addLayout(horizontalLayout);

    TextPTe = new ZQPlainTextEdit(verticalLayoutWidget);
    TextPTe->setObjectName("textPTe");
    sizePolicy1.setHeightForWidth(TextPTe->sizePolicy().hasHeightForWidth());
    TextPTe->setSizePolicy(sizePolicy1);
    TextPTe->setSizeIncrement(QSize(0, 0));
    QFont font1;
    font1.setFamilies({QString::fromUtf8("Courier")});
    font1.setPointSize(10);
    font1.setBold(true);
    TextPTe->setFont(font1);
    TextPTe->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    TextPTe->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    verticalLayout->addWidget(TextPTe);

    this->setCentralWidget(centralwidget);
    statusbar = new QStatusBar(this);
    statusbar->setObjectName("statusbar");
    this->setStatusBar(statusbar);
    menubar = new QMenuBar(this);
    menubar->setObjectName("menubar");
    menubar->setGeometry(QRect(0, 0, 813, 20));
    this->setMenuBar(menubar);

    this->setWindowTitle(QCoreApplication::translate("textEditMWn", "MainWindow", nullptr));
    searchBTn->setText(QString());
    searchMainBTn->setText(QString());
    filterBTn->setText(QString());
    lineNumbersBTn->setText(QCoreApplication::translate("textEditMWn", "Line numbers On", nullptr));
    wrapBTn->setText(QCoreApplication::translate("textEditMWn", "No wrap", nullptr));
    closeBTn->setText(QCoreApplication::translate("textEditMWn", "Close", nullptr));
#if QT_CONFIG(tooltip)
    TextPTe->setToolTip(QCoreApplication::translate("textEditMWn", "Filter content", nullptr));
#endif // QT_CONFIG(tooltip)



  Options = pOptions;

  genMEn = new QMenu("General",this);

  menubar->addMenu(genMEn);

  menuActionQAg = new QActionGroup(this);
  menuActionQAg->setExclusive(false);

  lineNbQAc = new QAction("line numbers",genMEn);
  lineNbQAc->setCheckable(true);
  lineNbQAc->setChecked(false);

  wrapQAc = new QAction("word wrap",genMEn);
  wrapQAc->setCheckable(true);
  wrapQAc->setChecked(false);

  writeQAc = new QAction("write to text file",genMEn);

  clearQAc = new QAction("clear",genMEn);

  genMEn->addAction(lineNbQAc);
  genMEn->addAction(wrapQAc);

  genMEn->addSeparator();

  genMEn->addAction(writeQAc);
  genMEn->addAction(clearQAc);

  genMEn->addSeparator();

  menuActionQAg->addAction(lineNbQAc);
  menuActionQAg->addAction(wrapQAc);
  menuActionQAg->addAction(writeQAc);
  menuActionQAg->addAction(clearQAc);

  genMEn->addSeparator();

  zoomInQAc = new QAction("Zoom in",genMEn);
  zoomOutQAc = new QAction("Zoom out",genMEn);

  genMEn->addAction(zoomInQAc);
  genMEn->addAction(zoomOutQAc);

  menuActionQAg->addAction(zoomInQAc);
  menuActionQAg->addAction(zoomOutQAc);

  TextPTe->setUseLineNumbers(Options & TEOP_ShowLineNumbers);
  if (TextPTe->getUseLineNumbers())
    lineNumbersBTn->setText(tr("Hide line numbers"));
  else
    lineNumbersBTn->setText(tr("Show line numbers"));



  setAttribute(Qt::WA_DeleteOnClose , true);

  TextPTe->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  TextPTe->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  TextPTe->setCenterOnScroll(true);

  TextPTe->setWordWrapMode(QTextOption::NoWrap);

  filterBTn->setVisible(false);

  searchMainBTn->setVisible(true);
  searchBTn->setVisible(false);
  searchLEd->setVisible(false);

  wrapBTn->setText(QObject::tr("Wrap","textEditMWn"));

  QMainWindow::setWindowTitle("Text");


  QObject::connect(lineNumbersBTn, SIGNAL(pressed()), this, SLOT(lineNumbersBTnClicked()));
  QObject::connect(closeBTn, SIGNAL(pressed()), this, SLOT(closePressed()));
  QObject::connect(wrapBTn, SIGNAL(pressed()), this, SLOT(wrapPressed()));
  QObject::connect(filterBTn, SIGNAL(pressed()), this, SLOT(filterPressed()));

  QObject::connect(searchMainBTn, SIGNAL(pressed()), this, SLOT(searchMainPressed()));
  QObject::connect(searchBTn, SIGNAL(pressed()), this, SLOT(searchPressed()));

  QObject::connect(searchLEd, SIGNAL(returnPressed()), this, SLOT(searchReturnPressed()));

  QObject::connect(menuActionQAg, &QActionGroup::triggered, this, &textEditMWn::MenuAction);

  if (pOptions & TEOP_NoCloseBtn) {
    setWindowFlag(Qt::WindowCloseButtonHint,false ); // no close button
    closeBTn->setVisible(false);
  }

  //  CloseCallBack = pCloseCallBack;

  setCloseButtonRole();
}

textEditMWn::textEditMWn(QWidget *parent) :QMainWindow(parent)//,ui(new Ui::textEditMWn)
{
//  _init(TEOP_Nothing,nullptr);
  _init(TEOP_Nothing);
}

/*
textEditMWn::textEditMWn(QWidget *parent,uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack)) :QMainWindow(parent)//,ui(new Ui::textEditMWn)
{
  _init(pOptions,pCloseCallBack);
}
*/
textEditMWn::textEditMWn(QWidget *parent,uint32_t pOptions,textEditMWn** pCalleePointer ) :QMainWindow(parent)//,ui(new Ui::textEditMWn)
{
//    _init(pOptions,nullptr);
    _init(pOptions);
    CalleePointer=pCalleePointer;
}
textEditMWn::~textEditMWn()
{
    if (hasErrorLog())
        ErrorLog->clearDisplayCallBacks();
    if (CalleePointer!=nullptr)
        *CalleePointer=nullptr;
 // delete ui;
}

void
textEditMWn::MenuAction(QAction* pAction) {
  if (pAction==quitQAc) {
    closePressed();
  }
  if (pAction==lineNbQAc) {
    if (lineNbQAc->isChecked()){
      if (hasLineNumbers())
        return;
      lineNumbersOnOff();
      return;
    }
    if (!hasLineNumbers())
      return;
    lineNumbersOnOff();
    return;
  }// lineNbQAc

  if (pAction==wrapQAc) {
    if (wrapQAc->isChecked()){
      if (TextPTe->wordWrapMode()==QTextOption::WordWrap)
        return;
      wordWrap();
      return;
    }
    if (TextPTe->wordWrapMode()==QTextOption::NoWrap)
      return;
    wordWrap();
    return;
  }// wrapQAc

  if (pAction==writeQAc) {
    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getSaveFileName(this, "text log file",
        wDir.toCChar(),
        "Text files (*.txt *.log);;All (*.*)");
    if (wFileName.isEmpty())
      return;
    uriString wFN=wFileName.toUtf8().data();
    QString wC=TextPTe->toPlainText();
    utf8VaryingString wTS=wC.toUtf8().data();
    ZStatus wSt=wFN.writeContent(wTS);
    if (wSt!=ZS_SUCCESS) {
      ZExceptionDLg::displayLast("Saving text file");
      return;
    }

  }//writeQAc

  if (pAction==clearQAc) {
    TextPTe->clear();
  } //clearQAc


  if (pAction==zoomInQAc) {
    QFont wF=TextPTe->font();
    qreal wFwght = wF.pointSizeF();
    wF.setPointSizeF(++wFwght);
    TextPTe->setFont(wF);
    return;
  }//zoomInQAc

  if (pAction==zoomOutQAc) {
    QFont wF=TextPTe->font();
    qreal wFwght = wF.pointSizeF();
    wF.setPointSizeF(--wFwght);
    TextPTe->setFont(wF);
    return;
  }//zoomInQAc

} //MenuAction



void textEditMWn::useLineNumbers(bool pOnOff) {
  TextPTe->setUseLineNumbers(pOnOff);
}


void textEditMWn::setCloseButtonRole () {

  if (Options & TEOP_CloseBtnHide) {
    closeBTn->setText(QObject::tr("Hide","textEditMWn"));
    }
    else {
      closeBTn->setText(QObject::tr("Close","textEditMWn"));
    }
}

void textEditMWn::closeEvent(QCloseEvent *event)
{
/*
  if (CloseCallBack!=nullptr) {
        CloseCallBack(event);
  }
*/
  if (CalleePointer!=nullptr)
      *CalleePointer=nullptr;
  QMainWindow::closeEvent(event);
  return;
}


void textEditMWn::keyPressEvent(QKeyEvent *pEvent) {

  if (pEvent->key()==Qt::Key_F3) {
    searchPressed();
  }

  if (pEvent->modifiers() & Qt::ControlModifier) {
    if (pEvent->key()==Qt::Key_F) {
    searchMainPressed();
    }
  }

  QWidget::keyPressEvent(pEvent);
}


void textEditMWn::resizeEvent(QResizeEvent* pEvent)
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

}//textEditMWn::resizeEvent

void textEditMWn::morePressed()
{
  if (MoreCallBack!=nullptr)
    MoreCallBack();
}
bool textEditMWn::hasLineNumbers() {
  return TextPTe->getUseLineNumbers();
}

void textEditMWn::lineNumbersBTnClicked()
{
  lineNumbersOnOff();
  if (hasLineNumbers()){
    if (lineNbQAc->isChecked())
        return;
    lineNbQAc->setChecked(true);
    return;
  }
  if (!lineNbQAc->isChecked())
    return;
  lineNbQAc->setChecked(false);
}

void textEditMWn::lineNumbersOnOff()
{
  TextPTe->setUseLineNumbers(!TextPTe->getUseLineNumbers());
  if (TextPTe->getUseLineNumbers())  {
    lineNumbersBTn->setText(tr("Hide line numbers"));
    Options &= ~ TEOP_ShowLineNumbers;
    return;
  }

  lineNumbersBTn->setText(tr("Show line numbers"));
  Options |=  TEOP_ShowLineNumbers;

}

void textEditMWn::wrapPressed()
{
  wordWrap();
  if (TextPTe->wordWrapMode()==QTextOption::WordWrap){
    if (wrapQAc->isChecked())
      return;
    wrapQAc->setChecked(true);
    return;
  }
  if (!wrapQAc->isChecked())
    return;
  wrapQAc->setChecked(false);
}

void textEditMWn::wordWrap()
{
  if (TextPTe->wordWrapMode()==QTextOption::NoWrap) {
    TextPTe->setWordWrapMode(QTextOption::WordWrap);
    wrapBTn->setText(tr("No wrap"));
  }
  else {
    TextPTe->setWordWrapMode(QTextOption::NoWrap);
    wrapBTn->setText(tr("Word wrap"));
  }
}

void textEditMWn::searchMainPressed()
{
  if (FSearch) {
    FSearch=false;
    searchBTn->setVisible(false);
    searchLEd->setVisible(false);
    searchLEd->clear();
    TextPTe->resetSelection();
    return;
  }
  FSearch=true;
  searchBTn->setVisible(true);
  searchLEd->setVisible(true);
  return;
}
void textEditMWn::searchPressed()
{
  search(searchLEd->text().toUtf8().data());
}

void textEditMWn::searchReturnPressed()
{
  searchFirst(searchLEd->text().toUtf8().data());
}

void textEditMWn::closePressed()
{
  if (Options & TEOP_CloseBtnHide) {
    this->hide();
      /*
    if (CloseCallBack) {
      QEvent wEv(QEvent::Hide);

      CloseCallBack(&wEv);
    }
*/
    return;
  }
  this->close();
  this->deleteLater();
  return;
}


ZStatus
textEditMWn::setTextFromFile(const uriString& pTextFile)
{
  utf8VaryingString wText;
  ZStatus wSt=pTextFile.loadUtf8(wText);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  TextPTe->setPlainText(wText.toQString());


  setWindowTitle(pTextFile.getBasename().toCChar());

  searchMainBTn->setVisible(true);

  return ZS_SUCCESS;
}

void
textEditMWn::setText(const utf8VaryingString& pText,const utf8VaryingString& pTitle)
{
  TextPTe->setPlainText(pText.toCChar());
  QMainWindow::setWindowTitle(pTitle.toCChar());
  searchMainBTn->setVisible(true);
}

void
textEditMWn::appendText(const utf8VaryingString& pText) {
  TextPTe->appendPlainText(pText.toCChar());
}
void
textEditMWn::appendTextColor(QColor pBkgndColor,QColor pTextColor,const utf8VaryingString& pText) {
  TextPTe->appendTextColor(pBkgndColor,pTextColor,pText.toCChar());
}
void
textEditMWn::appendText(const char* pText,...) {
  utf8VaryingString wT;
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

  TextPTe->appendPlainText(wT.toCChar());
}

void
textEditMWn::appendHtml(const char* pText,...) {
  utf8VaryingString wT = utf8VaryingString::escapeHtmlSeq(pText);
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);


  TextPTe->appendHtml(wT.toCChar());
}

void
textEditMWn::appendTextColor(QColor pBkgndColor,QColor pTextColor,const char* pText,...) {
  utf8VaryingString wT;
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

  appendTextColor(pBkgndColor,pTextColor,wT);
}

void
textEditMWn::appendTextColor(QColor pTextColor,const char* pText,...) {
  utf8VaryingString wT;
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

   TextPTe->appendTextColor(QColor(),pTextColor,wT.toCChar());
}
void
textEditMWn::clear() {
  TextPTe->clear();
}

void
textEditMWn::highlightWordAtOffset(int pOffset)
{
  TextPTe->highlightWordAtOffset(pOffset);
}
void
textEditMWn::highlightLine(int pLine)
{
  TextPTe->highlightLine(pLine);
}
void
textEditMWn::resetLine(int pLine) {
  TextPTe->resetLine(pLine);
}
void
textEditMWn::resetWordAtOffset(int pOffset) {
  TextPTe->resetWordAtOffset(pOffset);
}

void
textEditMWn::setPositionOrigin (){
  TextPTe->setPositionOrigin ();
}

void textEditMWn::filterPressed() {
  filtrate();
  TextPTe->refresh();
}

void textEditMWn::setFiltrateActive(bool pOnOff) {
  if (pOnOff && (FiltrateCallBack==nullptr)) {
    fprintf(stderr,"textEditMWn::setFiltrateActive-F-NOCALLBCK FiltrateCallBack has not been registrated while trying to set filtrate on.\n");
    std::cout.flush();
    abort();
  }
  FiltrateActive=pOnOff;
  if (FiltrateActive) {
    filterBTn->setEnabled(true);
    filterBTn->setVisible(true);
  }
  else {
    filterBTn->setEnabled(false);
    filterBTn->setVisible(false);
  }
}

void
textEditMWn::filtrate() {
  bool wEndText=false;
  if (FiltrateCallBack==nullptr)
    return;
  QTextDocument* wTDoc=TextPTe->document();
  QTextBlock wTBloc= wTDoc->firstBlock();
  while (true) {
    utf8VaryingString wBC=wTBloc.text().toUtf8().data();
    if (!wBC.isEmpty()) {
      if ( FiltrateCallBack(wBC))
        wTBloc.setVisible(true);
      else
        wTBloc.setVisible(false);
    }
    if (wEndText)
      break;
    wTBloc=wTBloc.next();
    wEndText=(wTBloc == wTDoc->lastBlock());
  }// while true

  TextPTe->update();
} // textEditMWn::filtrate

void
textEditMWn::searchFirst(const utf8VaryingString& pSearchString) {

  if (pSearchString.isEmpty()) {
    statusbar->showMessage(QObject::tr("search string is empty."),cst_MessageDuration);
    return;
  }
  if (searchDoc==nullptr) {
    searchDoc = TextPTe->document();
  }

  searchCursor= searchDoc->find (pSearchString.toCChar(),searchOffset);
  if (searchCursor.isNull()) {
      statusbar->showMessage(QObject::tr("search string not found."),cst_MessageDuration);
      return;
  }

  /* here string has been found and is pointed by wSearchCursor */
  TextPTe->highlightSelection(searchCursor);

} // textEditMWn::filtrate
void
textEditMWn::search(const utf8VaryingString& pSearchString) {

  if (pSearchString.isEmpty()) {
    statusbar->showMessage(QObject::tr("search string is empty."),cst_MessageDuration);
    return;
  }
  if (searchDoc==nullptr) {
    searchDoc = TextPTe->document();
  }
  if (searchCursor.isNull()) {
    searchCursor= searchDoc->find (pSearchString.toCChar(),searchOffset);
    if (searchCursor.isNull()) {
      statusbar->showMessage(QObject::tr("search string not found."),cst_MessageDuration);
      return;
    }
  }
  else {
    searchCursor= searchDoc->find (pSearchString.toCChar(),searchCursor);
    if (searchCursor.isNull()) {
      statusbar->showMessage(QObject::tr("search string no more found."),cst_MessageDuration);
      return;
    }
  }

  /* here string has been found and is pointed by wSearchCursor */
  TextPTe->highlightSelection(searchCursor);

} // textEditMWn::filtrate


void textEditMWn::displayColorCallBack (uint8_t pSeverity,const utf8VaryingString& pMessage)
{
    switch (pSeverity) {
    case ZAIES_Text:
    {
        QPalette wP = palette();
        QColor wC=wP.color(QPalette::Text);
        appendTextColor(wC, pMessage);
 //       appendTextColor(Qt::black, pMessage);
//        appendText(pMessage);
        return;
    }
    case ZAIES_Info:
        appendTextColor(Qt::blue, pMessage);
        return;
    case ZAIES_Warning:
        appendTextColor(Qt::darkGreen, pMessage);
        return;
    case ZAIES_Error:
        appendTextColor(Qt::red, pMessage);
        return;
    case ZAIES_Fatal:
        appendTextColor(Qt::magenta, pMessage);
        return;
    default:
        appendTextColor(Qt::yellow, pMessage);
        return;
    }
}//textEditMWn::displayColorCallBack
void textEditMWn::displayCallBack (const utf8VaryingString& pMessage)
{
        appendText(pMessage);
}//textEditMWn::displayCallBack
