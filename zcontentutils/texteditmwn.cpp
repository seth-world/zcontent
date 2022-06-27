#include "texteditmwn.h"
#include "ui_texteditmwn.h"

#include <ztoolset/uristring.h>
#include <ztoolset/utfvaryingstring.h>
#include <QtWidgets/QPlainTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QWidget>



void
textEditMWn::_init(uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack)) {

  ui->setupUi(this);

  Options = pOptions;

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
  QObject::connect(ui->lineNumbersBTn, SIGNAL(pressed()), this, SLOT(lineNumbersOnOff()));
  QObject::connect(ui->closeBTn, SIGNAL(pressed()), this, SLOT(closePressed()));
  QObject::connect(ui->wrapBTn, SIGNAL(pressed()), this, SLOT(wrapPressed()));
  QObject::connect(ui->filterBTn, SIGNAL(pressed()), this, SLOT(filterPressed()));

  QObject::connect(ui->searchMainBTn, SIGNAL(pressed()), this, SLOT(searchMainPressed()));
  QObject::connect(ui->searchBTn, SIGNAL(pressed()), this, SLOT(searchPressed()));

  QObject::connect(ui->searchLEd, SIGNAL(returnPressed()), this, SLOT(searchReturnPressed()));


  setCloseButtonRole();
}

textEditMWn::textEditMWn(QWidget *parent) :QMainWindow(parent),ui(new Ui::textEditMWn)
{
  _init(TEOP_Nothing,nullptr);
}

textEditMWn::textEditMWn(QWidget *parent,uint32_t pOptions) :QMainWindow(parent),ui(new Ui::textEditMWn)
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
  QWidget::closeEvent(event);
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
void textEditMWn::lineNumbersOnOff()
{
  Text->setUseLineNumbers(!Text->getUseLineNumbers());
  if (Text->getUseLineNumbers())  {
    ui->lineNumbersBTn->setText(tr("Hide line numbers"));
    Options &= ~ TEOP_ShowLineNumbers;
  }
  else {
    ui->lineNumbersBTn->setText(tr("Show line numbers"));
    Options |=  TEOP_ShowLineNumbers;
  }
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
void textEditMWn::wrapPressed()
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
textEditMWn::appendText(const utf8VaryingString& pText)
{
  Text->appendPlainText(pText.toCChar());
}

void
textEditMWn::clear()
{
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
  if (FiltrateCallBack(wBC))
    wTBloc.setVisible(true);
  else
    wTBloc.setVisible(false);

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
