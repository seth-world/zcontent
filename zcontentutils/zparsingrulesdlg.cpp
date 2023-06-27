#include "zparsingrulesdlg.h"
#include "ui_zparsingrulesdlg.h"
#include <zcontent/zcontentutils/ztypedlg.h>
#include <zexceptiondlg.h>
#include <zcontent/zindexedfile/zdatatype.h>

#include <zindexedfile/zdataconversion.h> // for _getURFHeaderSize

#include <qmenu.h>
#include <qaction.h>
#include <qactiongroup.h>
#include <zqt/zqtwidget/zqlineedit.h>

#include <qclipboard.h>
#include <qmimedatabase.h>
#include <qmimedata.h>

using namespace zbs;

ZParsingRulesDLg::ZParsingRulesDLg(ZCppParser* pParser,QWidget *parent) : QDialog(parent),
                                                                           ui(new Ui::ZParsingRulesDLg)
{
  ui->setupUi(this);
  Parser=pParser;
  setWindowTitle(QObject::tr("Parsing rule","ZParsingRulesDLg"));


  SyntaxLEd = new ZQLineEdit(ui->SyntaxWDg);
  SyntaxLEd->setGeometry(QRect(0, 1, 280, 25));
//  SyntaxLEd->setEnabled(true);
//  SyntaxLEd->setReadOnly(false);

  SyntaxLEd->keyPressCallback =std::bind(&ZParsingRulesDLg::keyPress, this,_1);
  SyntaxLEd->contextMenuCallback =std::bind(&ZParsingRulesDLg::contextMenuCallback, this,_1);

  /*
  TypeListBtn= new ZTypeListButton(ui->ZTypeListWDg);
  TypeListBtn->setObjectName(QString::fromUtf8("TypeListBtn"));
  TypeListBtn->setGeometry(QRect(10, 10, 51, 23));
  TypeListBtn->setText(QObject::tr("List","ZParsingRulesDLg"));

  TokenTypeListBtn= new ZListButton<ZTokentype_type>(ui->ZTypeListWDg);
  TokenTypeListBtn->setObjectName(QString::fromUtf8("TokenTypeListBtn"));
  TokenTypeListBtn->setGeometry(QRect(10, 10, 51, 23));
  TokenTypeListBtn->setText(QObject::tr("List","ZParsingRulesDLg"));
  */
  TypeListBtn= new QPushButton(ui->ZTypeListWDg);
  TypeListBtn->setObjectName(QString::fromUtf8("TypeListBtn"));
  TypeListBtn->setGeometry(QRect(10, 10, 51, 23));
  TypeListBtn->setText(QObject::tr("List","ZParsingRulesDLg"));

  QObject::connect(TypeListBtn,SIGNAL(clicked()),this,SLOT(ZTypeListClicked()));

  QObject::connect(ui->DiscardBTn,SIGNAL(clicked()),this,SLOT(DiscardClicked()));
  QObject::connect(ui->AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));

  ui->AcceptBTn->setDefault(true);
}

ZParsingRulesDLg::~ZParsingRulesDLg()
{
  delete SyntaxLEd;
  delete ui;
}
void
ZParsingRulesDLg::setCreateFromSyntax(const utf8VaryingString& pSyntax) {

  ConversionElt.Syntax=pSyntax;
  ConversionElt.ZType = ZType_Unknown;
  ConversionElt.HeaderSize = ConversionElt.NaturalSize=ConversionElt.UniversalSize=0;
  setup(ConversionElt);
}

void
ZParsingRulesDLg::setup(const ZConversionElement& pConvElt) {

  ConversionElt=pConvElt;

  utf8VaryingString wStr;
  // ui->TokenTypeLEd->setText( decode_TokenType (pConvElt.TokenType).toCChar());

  SyntaxLEd->setText(ConversionElt.Syntax.toCChar());
  SyntaxLEd->setReadOnly(false);

  wStr.sprintf("0x%08X",ConversionElt.ZType);
  ui->ZTypeLEd->setText(wStr.toCChar());
  ui->ZTypeLEd->setReadOnly(true);

  ui->ZTypeStrLEd->setText(decode_ZType(ConversionElt.ZType));
  ui->ZTypeStrLEd->setReadOnly(true);

  wStr.sprintf("%d",ConversionElt.Capacity);
  ui->CapacityLEd->setText(wStr.toCChar());

  wStr.sprintf("%ld",ConversionElt.UniversalSize);
  ui->UniversalLEd->setText(wStr.toCChar());
  //  ui->UniversalLEd->setEnabled(false);
  ui->UniversalLEd->setEnabled(true);

  wStr.sprintf("%ld",ConversionElt.NaturalSize);
  ui->NaturalLEd->setText(wStr.toCChar());
  ui->NaturalLEd->setReadOnly(true);

  wStr.sprintf("%ld",ConversionElt.HeaderSize);
  ui->HeaderSizeLEd->setText(wStr.toCChar());
  ui->HeaderSizeLEd->setReadOnly(true);
}

void ZParsingRulesDLg::refresh()
{
  utf8String wStr;

  ConversionElt.Syntax=SyntaxLEd->text().toUtf8().data();

  if (ZTypeTRv)
    ConversionElt.ZType = ZTypeTRv->get();

  wStr=ui->HeaderSizeLEd->text().toUtf8().data();
  ConversionElt.HeaderSize = (uint64_t)wStr.toULong();

  wStr=ui->CapacityLEd->text().toUtf8().data();
  ConversionElt.Capacity = (URF_Capacity_type)wStr.toInt();

  wStr=ui->NaturalLEd->text().toUtf8().data();
  ConversionElt.NaturalSize = (uint64_t)wStr.toULong();

  wStr=ui->UniversalLEd->text().toUtf8().data();
  ConversionElt.UniversalSize = (uint64_t)wStr.toULong();

  wStr=ui->HeaderSizeLEd->text().toUtf8().data();
  ConversionElt.HeaderSize = (uint64_t)wStr.toULong();
}

bool
ZParsingRulesDLg::getSyntaxFromClipboard() {
  utf8String wClipContent;
  ZaiErrors Errorlog;
  QByteArray wBA ;
  QClipboard *wClipboard = QGuiApplication::clipboard();
  const QMimeData * wMData=wClipboard->mimeData(QClipboard::Clipboard);
  if (wMData==nullptr)
  {
    ZExceptionDLg::adhocMessage("Clipboard",Severity_Error,&Errorlog,nullptr,QObject::tr("Clipboard is empty.","RawFields").toUtf8().data());
    return false;
  }
  if (!wMData->hasFormat("text/plain"))
  {
    ZExceptionDLg::adhocMessage("Clipboard",Severity_Error,&Errorlog,nullptr,QObject::tr("Clipboard has no text data.","RawFields").toUtf8().data());
    return false;
  }
  wClipContent = wMData->data("text/plain").data();  /* load mime data as utf8 string */
  SyntaxLEd->setText(wClipContent.toCChar());
  return true;
}

bool
ZParsingRulesDLg::keyPress(QKeyEvent*pEvent) {
  if (pEvent->modifiers()== Qt::ControlModifier) {
    if (pEvent->key()==Qt::Key_V) {
      getSyntaxFromClipboard();
      return true;
    }
  }
  return false;
}
bool
ZParsingRulesDLg::contextMenuCallback(QContextMenuEvent*pEvent) {

  return false;
}

void
ZParsingRulesDLg::ZTypeListClicked() {
  utf8String wStr;
  if (ZTypeTRv==nullptr)
  {
    ZTypeTRv=new ZTypeDLg(this);
    ZTypeTRv->setWindowTitle(QObject::tr("Data types","ZFieldDLg"));
    ZTypeTRv->setModal(true);
  }

  ZTypeTRv->set(ConversionElt.ZType);
  while (true) {
    int wRet=ZTypeTRv->exec();
    if (wRet==QDialog::Rejected) {
      ZTypeTRv->deleteLater();
      ZTypeTRv=nullptr;
      return;
    }

    ZTypeBase wType = ZTypeTRv->get();
    ZTypeBase wType2 = wType & ~ZType_Array ;
    if ((wType & ZType_Array) && !isAtomic(wType2)) {
      wRet=ZExceptionDLg::message2B("ZFieldDLg",ZS_INVTYPE,Severity_Error,
          "Quit","Retry",
          "An array must concern uniquely atomic data types.\n0X%X-<%s> is an invalid data type.",
          wType,decode_ZType(wType));
      if (wRet==QDialog::Rejected) {
        ZTypeTRv->deleteLater();
        ZTypeTRv=nullptr;
        return;
      }
      continue;
    }
    break;
  }//while true

  //  TypeListBtn->ZType = ConversionElt.ZType = ZTypeTRv->get();
  ConversionElt.ZType = ZTypeTRv->get();

  wStr.sprintf("0x%08X",ConversionElt.ZType);
  ui->ZTypeLEd->setText(wStr.toCChar());

  ui->ZTypeStrLEd->setText(decode_ZType(ConversionElt.ZType));
  ui->ZTypeStrLEd->setReadOnly(true);

  ui->KeyEligibleCHk->setChecked(!(ConversionElt.ZType & ZType_VaryingLength));

  ConversionElt.HeaderSize=getURFHeaderSize(ConversionElt.ZType);
  wStr.sprintf("%ld",ConversionElt.HeaderSize);

  ui->HeaderSizeLEd->setText(wStr.toCChar());

  if (ConversionElt.ZType & ZType_Atomic) {
    ui->CreateArrayBTn->setVisible(true);
    getAtomicZType_Sizes(ConversionElt.ZType,ConversionElt.NaturalSize,ConversionElt.UniversalSize);

    wStr.sprintf("%ld",ConversionElt.NaturalSize);
    ui->NaturalLEd->setText(wStr.toCChar());

    wStr.sprintf("%ld",ConversionElt.UniversalSize);
    ui->UniversalLEd->setText(wStr.toCChar());

    ConversionElt.Capacity=1;
    wStr.sprintf("%ld",ConversionElt.Capacity);
    ui->CapacityLEd->setText(wStr.toCChar());
  }
  else {
    ui->CreateArrayBTn->setVisible(false);
  }
}//ListClicked




void
ZParsingRulesDLg::AcceptClicked()
{
    refresh();   
    accept();
}

void
ZParsingRulesDLg::DiscardClicked()
{
  reject();
}


void
ZParsingRulesDLg::contextMenuEvent(QContextMenuEvent *pEvent) {
/*  QMenu* wFlexMenu=new QMenu("Edit choices",this);

  QAction* ParseSyntax=wFlexMenu->addAction("Parse syntax");

  wFlexMenu->exec(pEvent->pos());
*/
  QDialog::contextMenuEvent(pEvent);
}

ZConversionElement& ZParsingRulesDLg::getConversionElt() {
  refresh();
  return ConversionElt;
}
