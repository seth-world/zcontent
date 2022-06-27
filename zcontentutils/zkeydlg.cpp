#include "zkeydlg.h"
#include "ui_zkeydlg.h"


ZKeyDLg::ZKeyDLg(ZMFDictionary* pMasterDic, QWidget *parent) :
                                    QDialog(parent),
                                    ui(new Ui::ZKeyDLg)
{
  ui->setupUi(this);

  MasterDic=pMasterDic;
  QObject::connect (ui->OKBTn,SIGNAL(clicked()),this,SLOT(OKBTnClicked()));
  QObject::connect (ui->DisguardBTn,SIGNAL(clicked()),this,SLOT(CancelBTnClicked()));
}

ZKeyDLg::~ZKeyDLg()
{
  delete ui;
}

void ZKeyDLg::OKBTnClicked()
{

  KeyDic->setName((const utf8_t*)ui->KeyNameLEd->text().toUtf8().data());
  KeyDic->Duplicates = (uint8_t)ui->DuplicatesCHk->isChecked();
  KeyDic->ToolTip = ui->TooltipLEd->text().toUtf8().data();

  accept();
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
void ZKeyDLg::set(ZKeyHeaderRow* pKeyDic)
{
  if (KeyDic)
    delete KeyDic;
//  KeyDic=new ZKeyDictionary(pKeyDic->get(MasterDic));
  KeyDic=new ZKeyDictionary(pKeyDic->get());

  ui->KeyNameLEd->setText(KeyDic->DicKeyName.toCChar());
  ui->DuplicatesCHk->setChecked(KeyDic->Duplicates);
  ui->TooltipLEd->setText(KeyDic->ToolTip.toCChar());
}
void ZKeyDLg::setCreate()
{
  if (KeyDic)
    delete KeyDic;
  //  KeyDic=new ZKeyDictionary(pKeyDic->get(MasterDic));
  KeyDic=new ZKeyDictionary(MasterDic);

  KeyDic->DicKeyName=__NEW_KEYNAME__;
  ui->DuplicatesCHk->setChecked(false);
  ui->TooltipLEd->clear();

}
