#include "zkeydlg.h"
#include "ui_zkeydlg.h"

#include <QStatusBar>

ZKeyDLg::ZKeyDLg( QWidget *parent) :  QDialog(parent),
                                      ui(new Ui::ZKeyDLg)
{
  ui->setupUi(this);


//  MasterDic=pMasterDic;
  QObject::connect (ui->OKBTn,SIGNAL(clicked()),this,SLOT(OKBTnClicked()));
  QObject::connect (ui->DisguardBTn,SIGNAL(clicked()),this,SLOT(CancelBTnClicked()));
}

ZKeyDLg::~ZKeyDLg()
{
  delete ui;
}

void ZKeyDLg::OKBTnClicked()
{
  refresh();
  accept();
}

void ZKeyDLg::refresh() {

  KHR.DicKeyName =(const utf8_t*)ui->KeyNameLEd->text().toUtf8().data();
  KHR.Duplicates = (uint8_t)ui->DuplicatesCHk->isChecked();
//  KHR.KeyUniversalSize  remains unchanged
  KHR.ToolTip = ui->TooltipLEd->text().toUtf8().data();
}

void ZKeyDLg::CancelBTnClicked()
{
  reject();
}



/*
void ZKeyDLg::set(KeyDic_Pack* pKeyDic)
{
  KeyDic=pKeyDic->to;

  ui->KeyNameLEd->setText(KeyDic->DicKeyName.toCChar());
  ui->DuplicatesCHk->setChecked(KeyDic->Duplicates);
}
*/
void ZKeyDLg::set(ZKeyHeaderRow* pKeyHeaderRow)
{
/*  if (KeyDic)
    delete KeyDic;
//  KeyDic=new ZKeyDictionary(pKeyDic->get(MasterDic));
  KeyDic=new ZKeyDictionary(pKeyDic->get());

  ui->KeyNameLEd->setText(KeyDic->DicKeyName.toCChar());
  ui->DuplicatesCHk->setChecked(KeyDic->Duplicates);
  ui->TooltipLEd->setText(KeyDic->ToolTip.toCChar());
*/
  ui->KeyNameLEd->setText(pKeyHeaderRow->DicKeyName.toCChar());
  ui->DuplicatesCHk->setChecked(pKeyHeaderRow->Duplicates);
  ui->TooltipLEd->setText(pKeyHeaderRow->ToolTip.toCChar());

  KHR._copyFrom(*pKeyHeaderRow);
}
void ZKeyDLg::setCreate()
{
//  if (KeyDic)
//    delete KeyDic;
//  KeyDic=new ZKeyDictionary(pKeyDic->get(MasterDic));
//  KeyDic=new ZKeyDictionary(MasterDic);



  KHR.DicKeyName=__NEW_KEYNAME__;
  ui->DuplicatesCHk->setChecked(false);
  ui->TooltipLEd->clear();
}
