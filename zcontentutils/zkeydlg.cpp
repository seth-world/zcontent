#include "zkeydlg.h"
#include "ui_zkeydlg.h"

#include <QStatusBar>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QComboBox>

//#include <QWidget>

#include <qevent.h>

#include <zqt/zqtwidget/zqtutils.h>
/*
ZKeyDLg::ZKeyDLg( QWidget *parent) :  QDialog(parent),
                                      ui(new Ui::ZKeyDLg)
*/
ZKeyDLg::ZKeyDLg( QWidget *parent) :  QDialog(parent)
{
//  setupUi(this);
    initLayout();

//  MasterDic=pMasterDic;
  QObject::connect (OKBTn,SIGNAL(clicked()),this,SLOT(OKBTnClicked()));
  QObject::connect (DisguardBTn,SIGNAL(clicked()),this,SLOT(CancelBTnClicked()));
  QObject::connect (ForcedCHk,SIGNAL(checkStateChanged(Qt::CheckState)),this,SLOT(ForcedCHkStateChanged(Qt::CheckState)));
}

ZKeyDLg::~ZKeyDLg()
{
 // delete ui;
}

void
ZKeyDLg::initLayout()
{
  int wWidth=510, wHeight=140;
  ZKeyDLg::resize(wWidth, wHeight);

  setWindowTitle("Index key");

  MainWidget = new QWidget(this);
  MainWidget->setGeometry(QRect(1, 1, geometry().width() -2, geometry().height() -2));
  QVBoxLayout* wVL=new QVBoxLayout(MainWidget);

  QHBoxLayout* wHBL1 = new QHBoxLayout;
  wVL->addLayout(wHBL1);

  if (objectName().isEmpty())
      setObjectName("ZKeyDLg");
 // ZKeyDLg->resize(510, 139);

  KeyNameLEd = new QLineEdit(MainWidget);
  KeyNameLEd->setObjectName("KeyNameLEd");
 // KeyNameLEd->setGeometry(QRect(120, 10, 211, 23));
  QLabel* lbl01 = new QLabel("Index key Name",MainWidget);
//  lbl01->setGeometry(QRect(20, 20, 91, 16));
  QSpacerItem* wSpc01= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

  wHBL1->addWidget(lbl01);
  wHBL1->addWidget(KeyNameLEd);
  wHBL1->addItem(wSpc01);

  DuplicatesCBx = new QComboBox(MainWidget);

  DuplicatesCBx->addItem(decode_ZST(ZST_Nothing));
  DuplicatesCBx->addItem(decode_ZST(ZST_NoDuplicates));
  DuplicatesCBx->addItem(decode_ZST(ZST_Duplicates));

  QLabel* lbl020 = new QLabel("Duplicates",MainWidget);

  DuplicatesCBx->setObjectName("DuplicatesCBx");

  QLabel* lbl02 = new QLabel("Guessed size",MainWidget);

  KeySizeLEd = new QLineEdit(MainWidget);
  KeySizeLEd->setObjectName("KeySizeLEd");
 // KeySizeLEd->setGeometry(QRect(350, 40, 151, 23));

  ForcedCHk = new QCheckBox("Forced",MainWidget);
  ForcedCHk->setObjectName("ForcedCHk");

  QHBoxLayout* wHBL2 = new QHBoxLayout;
  wVL->addLayout(wHBL2);

  wHBL2->addWidget(lbl020);
  wHBL2->addWidget(DuplicatesCBx);
  wHBL2->addWidget(lbl02);
  wHBL2->addWidget(KeySizeLEd);
  wHBL2->addWidget(ForcedCHk);

  KeySizeLEd->setEnabled(false);
  ForcedCHk->setCheckState(Qt::Unchecked);

  TooltipLEd = new QLineEdit(MainWidget);
  TooltipLEd->setObjectName("TooltipLEd");
//  TooltipLEd->setGeometry(QRect(120, 80, 381, 23));

  QLabel* lbl03 = new QLabel("Tooltip",MainWidget);

  QHBoxLayout* wHBL3 = new QHBoxLayout;
  wVL->addLayout(wHBL3);

  wHBL3->addWidget(lbl03);
  wHBL3->addWidget(TooltipLEd);

  OKBTn = new QPushButton("OK",MainWidget);
  OKBTn->setObjectName("OKBTn");
  DisguardBTn = new QPushButton("Discard",MainWidget);
  DisguardBTn->setObjectName("DisguardBTn");

  QSpacerItem* wSpc03= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

  QHBoxLayout* wHBL4 = new QHBoxLayout;
  wVL->addLayout(wHBL4);

  wHBL4->addItem(wSpc03);
  wHBL4->addWidget(DisguardBTn);
  wHBL4->addWidget(OKBTn);


}


void ZKeyDLg::ForcedCHkStateChanged(Qt::CheckState pState)
{
  if (pState==Qt::Checked) {
      KeySizeLEd->setEnabled(true);
      return;
  }

  KeySizeLEd->setEnabled(false);
  return;
}

void ZKeyDLg::OKBTnClicked()
{
  refresh();
  accept();
}

void ZKeyDLg::refresh() {
  utf8VaryingString wStr;
  KHR.DicKeyName =(const utf8_t*)KeyNameLEd->text().toUtf8().data();
  KHR.Duplicates = (ZSort_Type)(DuplicatesCBx->currentIndex());
//  KHR.KeyGuessedSize  remains unchanged
  KHR.ToolTip = TooltipLEd->text().toUtf8().data();
  wStr = KeySizeLEd->text().toUtf8().data();
  if (wStr.isEmpty())   {
    KHR.KeyGuessedSize =0;
    KHR.Forced=false;
  }
  else {
    KHR.KeyGuessedSize = getValueFromString<uint32_t>(wStr);
    KHR.Forced=true;
  }
  KHR.ToolTip = TooltipLEd->text().toUtf8().data();
} // refresh

void ZKeyDLg::CancelBTnClicked()
{
  reject();
}


void ZKeyDLg::set(ZKeyHeaderRow* pKeyHeaderRow)
{
  utf8VaryingString wStr;

  KHR._copyFrom(*pKeyHeaderRow);

  KeyNameLEd->setText(pKeyHeaderRow->DicKeyName.toCChar());
  DuplicatesCBx->setCurrentIndex( int(pKeyHeaderRow->Duplicates));
//  DuplicatesCBx->setChecked(bool(pKeyHeaderRow->Duplicates));
  ForcedCHk->setChecked(bool(pKeyHeaderRow->Forced));
  TooltipLEd->setText(pKeyHeaderRow->ToolTip.toCChar());

  wStr.sprintf("%ld",pKeyHeaderRow->KeyGuessedSize);
  KeySizeLEd->setText(wStr.toCChar());

}
void ZKeyDLg::setCreate()
{
//  if (KeyDic)
//    delete KeyDic;
//  KeyDic=new ZKeyDictionary(pKeyDic->get(MasterDic));
//  KeyDic=new ZKeyDictionary(MasterDic);
  KHR.DicKeyName=__NEW_KEYNAME__;
  DuplicatesCBx->setCurrentIndex(0);
  ForcedCHk->setChecked(false);
  KeySizeLEd->setText("0");
  KeySizeLEd->setEnabled(false);
  TooltipLEd->clear();
}
void
ZKeyDLg::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (_FResizeInitial) {
    _FResizeInitial=false;
    return;
  }
  QRect wR1 = MainWidget->geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  MainWidget->resize(wVW,wVH);  /* expands in width and height */

}//ZKeyDLgZKeyDLg::resizeEvent

