#include "texteditmwn.h"
#include "ui_texteditmwn.h"

#include <ztoolset/uristring.h>
#include <ztoolset/utfvaryingstring.h>
#include <QtWidgets/QPlainTextEdit>

#include <zcontentvisumain.h>

textEditMWn::textEditMWn(ZContentVisuMain *parent) :QMainWindow(parent),ui(new Ui::textEditMWn)
{
  ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose , true);

  VisuMain=parent;

  ui->textPTe->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->textPTe->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  ui->textPTe->setWordWrapMode(QTextOption::NoWrap);
  ui->wrapBTn->setText(QObject::tr("Wrap","textEditMWn"));

  QMainWindow::setWindowTitle("Text");

//  QObject::connect(this, SIGNAL(resizeEvent(QResizeEvent*)), this, SLOT(resizeWindow(QResizeEvent*)));
  QObject::connect(ui->moreBTn, SIGNAL(pressed()), this, SLOT(morePressed()));
  QObject::connect(ui->closeBTn, SIGNAL(pressed()), this, SLOT(closePressed()));
  QObject::connect(ui->wrapBTn, SIGNAL(pressed()), this, SLOT(wrapPressed()));
}
textEditMWn::textEditMWn(QWidget *parent) :QMainWindow(parent),ui(new Ui::textEditMWn)
{
  ui->setupUi(this);

  QMainWindow::setWindowTitle("Text");

  setAttribute(Qt::WA_DeleteOnClose , true);

  VisuMain=nullptr;

  ui->textPTe->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  ui->textPTe->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  ui->textPTe->setWordWrapMode(QTextOption::NoWrap);

  ui->moreBTn->setVisible(false);

  setWindowTitle("Text");

//  QObject::connect(this, SIGNAL(resizeEvent(QResizeEvent*)), this, SLOT(resizeWindow(QResizeEvent*)));
  QObject::connect(ui->moreBTn, SIGNAL(pressed()), this, SLOT(morePressed()));
  QObject::connect(ui->closeBTn, SIGNAL(pressed()), this, SLOT(closePressed()));
  QObject::connect(ui->wrapBTn, SIGNAL(pressed()), this, SLOT(wrapPressed()));
}
textEditMWn::~textEditMWn()
{
  delete ui;
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
}

void textEditMWn::morePressed()
{
  if (VisuMain!=nullptr)
    VisuMain->textEditMorePressed();

}
void textEditMWn::closePressed()
{
  this->close();
  this->deleteLater();
}
void textEditMWn::wrapPressed()
{
  if (FWrap)
    {
    ui->textPTe->setWordWrapMode(QTextOption::NoWrap);
    ui->wrapBTn->setText(QObject::tr("Wrap","textEditMWn"));
    FWrap = false;
    }
  else
    {
    ui->textPTe->setWordWrapMode(QTextOption::WordWrap);
    ui->wrapBTn->setText(QObject::tr("No Wrap","textEditMWn"));
    FWrap = true;
    }
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

  return ZS_SUCCESS;
}

void
textEditMWn::setText(const utf8VaryingString& pText,const utf8VaryingString& pTitle)
{
  ui->textPTe->setPlainText(pText.toCChar());
  QMainWindow::setWindowTitle(pTitle.toCChar());
}

void
textEditMWn::appendText(const utf8VaryingString& pText)
{
  ui->textPTe->appendPlainText(pText.toCChar());
}
