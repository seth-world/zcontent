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

void
textEditMWn::_init(uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack)) {

  ui->setupUi(this);

  Options = pOptions;

  genMEn = new QMenu("General",this);

  ui->menubar->addMenu(genMEn);

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

  Text= ui->textPTe;
  Text->setUseLineNumbers(Options & TEOP_ShowLineNumbers);
  if (Text->getUseLineNumbers())
    ui->lineNumbersBTn->setText(tr("Hide line numbers"));
  else
    ui->lineNumbersBTn->setText(tr("Show line numbers"));

  CloseCallBack = pCloseCallBack;

  setAttribute(Qt::WA_DeleteOnClose , true);

  Text->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  Text->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  Text->setCenterOnScroll(true);

  Text->setWordWrapMode(QTextOption::NoWrap);
/*
  Cursor = new QTextCursor(Text->textCursor());
  FmtDefault = new QTextCharFormat(Cursor->charFormat());
  delete Cursor;
  Cursor=nullptr;
*/
  ui->filterBTn->setVisible(false);

  ui->searchMainBTn->setVisible(true);
  ui->searchBTn->setVisible(false);
  ui->searchLEd->setVisible(false);

  ui->wrapBTn->setText(QObject::tr("Wrap","textEditMWn"));

  QMainWindow::setWindowTitle("Text");


  //  QObject::connect(this, SIGNAL(resizeEvent(QResizeEvent*)), this, SLOT(resizeWindow(QResizeEvent*)));
  QObject::connect(ui->lineNumbersBTn, SIGNAL(pressed()), this, SLOT(lineNumbersBTnClicked()));
  QObject::connect(ui->closeBTn, SIGNAL(pressed()), this, SLOT(closePressed()));
  QObject::connect(ui->wrapBTn, SIGNAL(pressed()), this, SLOT(wrapPressed()));
  QObject::connect(ui->filterBTn, SIGNAL(pressed()), this, SLOT(filterPressed()));

  QObject::connect(ui->searchMainBTn, SIGNAL(pressed()), this, SLOT(searchMainPressed()));
  QObject::connect(ui->searchBTn, SIGNAL(pressed()), this, SLOT(searchPressed()));

  QObject::connect(ui->searchLEd, SIGNAL(returnPressed()), this, SLOT(searchReturnPressed()));

  QObject::connect(menuActionQAg, &QActionGroup::triggered, this, &textEditMWn::MenuAction);

  if (pOptions & TEOP_NoFileLab)
    ui->ClosedLBl->setVisible(false);

  if (pOptions & TEOP_NoCloseBtn) {
    setWindowFlag(Qt::WindowCloseButtonHint,false ); // no close button
    ui->closeBTn->setVisible(false);
  }

  setCloseButtonRole();
}

textEditMWn::textEditMWn(QWidget *parent) :QMainWindow(parent),ui(new Ui::textEditMWn)
{
  _init(TEOP_Nothing,nullptr);
}


textEditMWn::textEditMWn(QWidget *parent,uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack)) :QMainWindow(parent),ui(new Ui::textEditMWn)
{
  _init(pOptions,pCloseCallBack);
}
textEditMWn::~textEditMWn()
{
  delete ui;
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
      if (Text->wordWrapMode()==QTextOption::WordWrap)
        return;
      wordWrap();
      return;
    }
    if (Text->wordWrapMode()==QTextOption::NoWrap)
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
    QString wC=Text->toPlainText();
    utf8VaryingString wTS=wC.toUtf8().data();
    ZStatus wSt=wFN.writeContent(wTS);
    if (wSt!=ZS_SUCCESS) {
      ZExceptionDLg::displayLast("Saving text file");
      return;
    }

  }//writeQAc

  if (pAction==clearQAc) {
    Text->clear();
  } //clearQAc


  if (pAction==zoomInQAc) {
    QFont wF=ui->textPTe->font();
    qreal wFwght = wF.pointSizeF();
    wF.setPointSizeF(++wFwght);
    ui->textPTe->setFont(wF);
    return;
  }//zoomInQAc

  if (pAction==zoomOutQAc) {
    QFont wF=ui->textPTe->font();
    qreal wFwght = wF.pointSizeF();
    wF.setPointSizeF(--wFwght);
    ui->textPTe->setFont(wF);
    return;
  }//zoomInQAc

} //MenuAction



void textEditMWn::useLineNumbers(bool pOnOff) {
  Text->setUseLineNumbers(pOnOff);
}


void textEditMWn::setCloseButtonRole () {

  if (Options & TEOP_CloseBtnHide) {
    ui->closeBTn->setText(QObject::tr("Hide","textEditMWn"));
    }
    else {
      ui->closeBTn->setText(QObject::tr("Close","textEditMWn"));
    }
}

void textEditMWn::closeEvent(QCloseEvent *event)
{
  if (CloseCallBack!=nullptr) {
    CloseCallBack(event);
  }
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
  QRect wR1 = ui->verticalLayoutWidget->geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  ui->verticalLayoutWidget->resize(wVW,wVH);  /* expands in width and height */

}//textEditMWn::resizeEvent

void textEditMWn::morePressed()
{
  if (MoreCallBack!=nullptr)
    MoreCallBack();
}
bool textEditMWn::hasLineNumbers() {
  return Text->getUseLineNumbers();
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
  Text->setUseLineNumbers(!Text->getUseLineNumbers());
  if (Text->getUseLineNumbers())  {
    ui->lineNumbersBTn->setText(tr("Hide line numbers"));
    Options &= ~ TEOP_ShowLineNumbers;
    return;
  }

  ui->lineNumbersBTn->setText(tr("Show line numbers"));
  Options |=  TEOP_ShowLineNumbers;

}

void textEditMWn::wrapPressed()
{
  wordWrap();
  if (Text->wordWrapMode()==QTextOption::WordWrap){
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
  if (Text->wordWrapMode()==QTextOption::NoWrap) {
    Text->setWordWrapMode(QTextOption::WordWrap);
    ui->wrapBTn->setText(tr("No wrap"));
  }
  else {
    Text->setWordWrapMode(QTextOption::NoWrap);
    ui->wrapBTn->setText(tr("Word wrap"));
  }
}

void textEditMWn::searchMainPressed()
{
  if (FSearch) {
    FSearch=false;
    ui->searchBTn->setVisible(false);
    ui->searchLEd->setVisible(false);
    ui->searchLEd->clear();
    Text->resetSelection();
    return;
  }
  FSearch=true;
  ui->searchBTn->setVisible(true);
  ui->searchLEd->setVisible(true);
  return;
}
void textEditMWn::searchPressed()
{
  search(ui->searchLEd->text().toUtf8().data());
}

void textEditMWn::searchReturnPressed()
{
  searchFirst(ui->searchLEd->text().toUtf8().data());
}

void textEditMWn::closePressed()
{
  if (Options & TEOP_CloseBtnHide) {
    this->hide();
    if (CloseCallBack) {
      QEvent wEv(QEvent::Hide);

      CloseCallBack(&wEv);
    }
    return;
  }
  this->close();
  this->deleteLater();
  return;
}

void textEditMWn::setFileClosed(bool pYesNo)
{
  ui->ClosedLBl->setVisible(pYesNo);
}

ZStatus
textEditMWn::setTextFromFile(const uriString& pTextFile)
{
  utf8String wText;
  ZStatus wSt=pTextFile.loadUtf8(wText);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  ui->textPTe->setPlainText(wText.toQString());


  setWindowTitle(pTextFile.getBasename().toCChar());

  ui->searchMainBTn->setVisible(true);

  return ZS_SUCCESS;
}

void
textEditMWn::setText(const utf8VaryingString& pText,const utf8VaryingString& pTitle)
{
  Text->setPlainText(pText.toCChar());
  QMainWindow::setWindowTitle(pTitle.toCChar());
  ui->searchMainBTn->setVisible(true);
}

void
textEditMWn::appendText(const utf8VaryingString& pText) {
  Text->appendPlainText(pText.toCChar());
}
void
textEditMWn::appendTextColor(QColor pBkgndColor,QColor pTextColor,const utf8VaryingString& pText) {
  Text->appendTextColor(pBkgndColor,pTextColor,pText.toCChar());
}
void
textEditMWn::appendText(const char* pText,...) {
  utf8VaryingString wT;
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

  Text->appendPlainText(wT.toCChar());
}

void
textEditMWn::appendHtml(const char* pText,...) {
  utf8VaryingString wT = escapeHtmlSeq(pText);
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

  Text->appendHtml(wT.toCChar());
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

  appendTextColor(QColor(),pTextColor,wT);
}
void
textEditMWn::clear() {
  Text->clear();
}

void
textEditMWn::highlightWordAtOffset(int pOffset)
{
  Text->highlightWordAtOffset(pOffset);
}
void
textEditMWn::highlightLine(int pLine)
{
  Text->highlightLine(pLine);
}
void
textEditMWn::resetLine(int pLine) {
  Text->resetLine(pLine);
}
void
textEditMWn::resetWordAtOffset(int pOffset) {
  Text->resetWordAtOffset(pOffset);
}

void
textEditMWn::setPositionOrigin (){
  Text->setPositionOrigin ();
}

void textEditMWn::filterPressed() {
  filtrate();
  Text->refresh();
}

void textEditMWn::setFiltrateActive(bool pOnOff) {
  if (pOnOff && (FiltrateCallBack==nullptr)) {
    fprintf(stderr,"textEditMWn::setFiltrateActive-F-NOCALLBCK FiltrateCallBack has not been registrated while trying to set filtrate on.\n");
    std::cout.flush();
    abort();
  }
  FiltrateActive=pOnOff;
  if (FiltrateActive) {
    ui->filterBTn->setEnabled(true);
    ui->filterBTn->setVisible(true);
  }
  else {
    ui->filterBTn->setEnabled(false);
    ui->filterBTn->setVisible(false);
  }
}

void
textEditMWn::filtrate() {
  bool wEndText=false;
  if (FiltrateCallBack==nullptr)
    return;
  QTextDocument* wTDoc=Text->document();
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

  Text->update();
} // textEditMWn::filtrate

void
textEditMWn::searchFirst(const utf8VaryingString& pSearchString) {

  if (pSearchString.isEmpty()) {
    ui->statusbar->showMessage(QObject::tr("search string is empty."),cst_MessageDuration);
    return;
  }
  if (searchDoc==nullptr) {
    searchDoc = Text->document();
  }

  searchCursor= searchDoc->find (pSearchString.toCChar(),searchOffset);
  if (searchCursor.isNull()) {
      ui->statusbar->showMessage(QObject::tr("search string not found."),cst_MessageDuration);
      return;
  }

  /* here string has been found and is pointed by wSearchCursor */
  Text->highlightSelection(searchCursor);

} // textEditMWn::filtrate
void
textEditMWn::search(const utf8VaryingString& pSearchString) {

  if (pSearchString.isEmpty()) {
    ui->statusbar->showMessage(QObject::tr("search string is empty."),cst_MessageDuration);
    return;
  }
  if (searchDoc==nullptr) {
    searchDoc = Text->document();
  }
  if (searchCursor.isNull()) {
    searchCursor= searchDoc->find (pSearchString.toCChar(),searchOffset);
    if (searchCursor.isNull()) {
      ui->statusbar->showMessage(QObject::tr("search string not found."),cst_MessageDuration);
      return;
    }
  }
  else {
    searchCursor= searchDoc->find (pSearchString.toCChar(),searchCursor);
    if (searchCursor.isNull()) {
      ui->statusbar->showMessage(QObject::tr("search string no more found."),cst_MessageDuration);
      return;
    }
  }

  /* here string has been found and is pointed by wSearchCursor */
  Text->highlightSelection(searchCursor);

} // textEditMWn::filtrate
