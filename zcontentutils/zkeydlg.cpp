#include "zkeydlg.h"
#include "ui_zkeydlg.h"

ZKeyDLg::ZKeyDLg(QWidget *parent) :
                                    QDialog(parent),
                                    ui(new Ui::ZKeyDLg)
{
  ui->setupUi(this);

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

  accept();
}
void ZKeyDLg::CancelBTnClicked()
{
  reject();
}


void ZKeyDLg::set(KeyDic_Pack* pKeyDic)
{
  KeyDic=pKeyDic;

  ui->KeyNameLEd->setText(KeyDic->getName().toCChar());
  ui->DuplicatesCHk->setChecked(KeyDic->Duplicates);
}
