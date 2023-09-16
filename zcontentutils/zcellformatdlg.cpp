#include "zcellformatdlg.h"
#include "ui_zcellformatdlg.h"

#include <zcontent/zindexedfile/zsearchparser.h>

ZCellFormatDLg::ZCellFormatDLg(QWidget *parent) :
                                                QDialog(parent),
                                                ui(new Ui::ZCellFormatDLg)
{
  ui->setupUi(this);

  QObject::connect(ui->OKBTn,&QPushButton::clicked,this,&QDialog::accept);
}

ZCellFormatDLg::~ZCellFormatDLg()
{
  delete ui;
}

void
ZCellFormatDLg::setup(int pCellFormat)
{
  int wBase = pCellFormat & ZCFMT_NumMask;
  if (wBase & ZCFMT_NumHexa) {
    ui->NumHexaRBn->setChecked(true);
  }
  else
    ui->NumStdRBn->setChecked(true);

  wBase = pCellFormat & ZCFMT_DateMask;

  switch (wBase)
  {
  case ZCFMT_DMY:
    ui->DMYRBn->setChecked(true);
    break;
  case ZCFMT_MDY:
    ui->MDYRBn->setChecked(true);
    break;
  case ZCFMT_DMYHMS:
    ui->DMYHMSRBn->setChecked(true);
    break;
  case ZCFMT_MDYHMS:
    ui->MDYHMSRBn->setChecked(true);
    break;
  case ZCFMT_DLocale:
    ui->LocaleRBn->setChecked(true);
    break;
  case ZCFMT_DUTC:
    ui->UTCRBn->setChecked(true);
    break;
  }

  wBase = pCellFormat & ZCFMT_ResMask;

  if (wBase & ZCFMT_ResSymb) {
    ui->ResSymbolCKb->setChecked(true);
  }
  else
    ui->ResSymbolCKb->setChecked(false);
  if (wBase & ZCFMT_ResStd) {
    ui->ResStdNumCKb->setChecked(true);
  }
  else
    ui->ResStdNumCKb->setChecked(false);
}
void
ZCellFormatDLg::setAppliance(const char* pAppliance)
{
  ui->ApplianceLBl->setText(pAppliance);
}

int
ZCellFormatDLg::get()
{
  int wCellFormat=ZCFMT_Nothing;
/*
  if (ui->ApplyAllRBn->isChecked())
    wCellFormat |= ZCFMT_ApplyAll;
*/
  /* general numeric */
  if (ui->NumHexaRBn->isChecked())
    wCellFormat |= ZCFMT_NumHexa;

  /* resources */
  if (ui->ResSymbolCKb->isChecked())
    wCellFormat |= ZCFMT_ResSymb;
  if (ui->ResStdNumCKb->isChecked())
    wCellFormat |= ZCFMT_ResStd;
  /* dates */
  while (true) {
    if (ui->DMYRBn->isChecked()) {
      wCellFormat |= ZCFMT_DMY;
      break;
    }
    if (ui->MDYRBn->isChecked()) {
      wCellFormat |= ZCFMT_MDY;
      break;
    }
    if (ui->DMYHMSRBn->isChecked()) {
      wCellFormat |= ZCFMT_DMYHMS;
      break;
    }
    if (ui->MDYHMSRBn->isChecked()) {
      wCellFormat |= ZCFMT_MDYHMS;
      break;
    }
    if (ui->LocaleRBn->isChecked()) {
      wCellFormat |= ZCFMT_DLocale;
      break;
    }
    if (ui->UTCRBn->isChecked()) {
      wCellFormat |= ZCFMT_DUTC;
      break;
    }
    /* strange but no check */
    break;
  }//while
  return wCellFormat;
}



