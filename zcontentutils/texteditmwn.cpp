#include "texteditmwn.h"
#include "ui_texteditmwn.h"

#include <ztoolset/uristring.h>
#include <QtWidgets/QPlainTextEdit>

textEditMWn::textEditMWn(QWidget *parent) :
                                            QMainWindow(parent),
                                            ui(new Ui::textEditMWn)
{
  ui->setupUi(this);
  ui->textPTe->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->textPTe->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  ui->textPTe->setWordWrapMode(QTextOption::NoWrap);


  setWindowTitle("Text");

//  QObject::connect(this, SIGNAL(resizeEvent(QResizeEvent*)), this, SLOT(resizeWindow(QResizeEvent*)));

}

textEditMWn::~textEditMWn()
{
  delete ui;
}

void textEditMWn::resizeWindow(QResizeEvent* pEvent)
{
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  QSize wDelta=  pEvent->size() - pEvent->oldSize();

  double wDeltaWidth = double(wDelta.width());
  double wDeltaHeight = double(wDelta.height());

  double wWindowWidth = double(width());
  double wWindowHeight = double(height());

  double wWidgetWidth = double(ui->textPTe->width());
  double wWidgetHeight = double(ui->textPTe->height());

  wWidgetWidth = wWidgetWidth + ((wDeltaWidth * wWidgetWidth) / wWindowWidth);

  wWidgetHeight = wWidgetHeight + ((wDeltaHeight * wWidgetHeight) / wWindowHeight);

  ui->textPTe->resize(int(wWidgetWidth),int(wWidgetHeight));
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
