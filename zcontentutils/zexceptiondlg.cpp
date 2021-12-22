#include "zexceptiondlg.h"

#include "ui_zexceptiondlg.h"
#include <ztoolset/zexceptionmin.h>




#define __ICON_PATH__ "/home/gerard/Development/zbasetools/zqt/icons/"
#define __INFO_ICON__ "info.png"
#define __WARNING_ICON__ "warning.png"
#define __ERROR_ICON__ "error.png"
#define __SEVERE_ICON__ "errorcross.png"
#define __FATAL_ICON__ "skullhead.png"
#define __HIGHEST_ICON__ "graverip.png"

ZStatus DontShowStatus=ZS_NOTHING;



ZExceptionDLg::ZExceptionDLg(const ZExceptionBase& pException, bool pDontShow) :
                                                QDialog(nullptr),
                                                ui(new Ui::ZExceptionDLg)
{
  DoNotShow=false;
  ui->setupUi(this);
  currentStatus=pException.Status;
  if (pException.Status==DontShowStatus)
    {
    DoNotShow=true;
    return;
    }

  if (pDontShow)
    {
    ui->dontShowCKb->setVisible(true);
//    QObject::connect (ui->dontShowCKb,&QCheckBox::clicked,this,[this]{ DontShowClicked(pException.Status); });
    QObject::connect (ui->dontShowCKb,&QCheckBox::clicked,this,&ZExceptionDLg::DontShowClicked);

    }
  else
    ui->dontShowCKb->setVisible(false);


  ui->ModuleLBl->setText(pException.Module.toCChar());
  ui->ZStatusLBl->setText(decode_ZStatus(pException.Status));
  ui->SeverityLBl->setText(decode_Severity(pException.Severity));

  ui->MessageTEd->setText(pException.Message.toCChar());

  if (pException.Complement.isEmpty())
    ui->ComplementTEd->setText("no complementary information");
  else
    ui->ComplementTEd->setText(pException.Complement.toCChar());

  ui->CancelBTn->setText("Cancel");
  ui->OKBTn->setText("OK");
  ui->OtherBTn->setText("");

  ui->OtherBTn->setVisible(false);

  QObject::connect (ui->CancelBTn,&QAbstractButton::pressed,this,[this]{ done(ZEDLG_Rejected); });
  QObject::connect (ui->OKBTn,&QAbstractButton::pressed,this,[this]{ done(ZEDLG_Accepted); });
  QObject::connect (ui->OtherBTn,&QAbstractButton::pressed,this,[this]{ done(ZEDLG_Third); });

}


void ZExceptionDLg::buttonPressed(int pValue)
{
  done(pValue);
}

void ZExceptionDLg::resetDontShow()
{
  DontShowStatus = ZS_NOTHING;
}

void ZExceptionDLg::DontShowClicked()
{
  if (ui->dontShowCKb->isChecked())
      {
      DontShowStatus=currentStatus;
      DoNotShow=true;
      }
}


ZExceptionDLg::~ZExceptionDLg()
{
  delete ui;
}

void ZExceptionDLg::setInfo()
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __INFO_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  ui->LogoLBl->setScaledContents(true);
  ui->LogoLBl->setPixmap(wPxMp);

  setWindowTitle("Information");
}
void ZExceptionDLg::setWarning()
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __WARNING_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  ui->LogoLBl->setScaledContents(true);
  ui->LogoLBl->setPixmap(wPxMp);

  setWindowTitle("Warning");
}
void ZExceptionDLg::setError()
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __ERROR_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  ui->LogoLBl->setScaledContents(true);
  ui->LogoLBl->setPixmap(wPxMp);

  setWindowTitle("Error");

}
void ZExceptionDLg::setSevere()
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __SEVERE_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  ui->LogoLBl->setScaledContents(true);
  ui->LogoLBl->setPixmap(wPxMp);
}
void ZExceptionDLg::setFatal()
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __FATAL_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  ui->LogoLBl->setScaledContents(true);
  ui->LogoLBl->setPixmap(wPxMp);

  setWindowTitle("Fatal error");

}
void ZExceptionDLg::setHighest()
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __HIGHEST_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  ui->LogoLBl->setScaledContents(true);
  ui->LogoLBl->setPixmap(wPxMp);

  setWindowTitle("Highest Error");

}

void
ZExceptionDLg::setButtonText(int pButtonOrder,const utf8String& pButtonText)
{
  switch (pButtonOrder)
  {
  case 0:
    ui->CancelBTn->setText(pButtonText.toCChar());
    return;
  case 1:
    ui->OKBTn->setText(pButtonText.toCChar());
    return;
  case 2:
    ui->OtherBTn->setText(pButtonText.toCChar());
    return;
  default:
    fprintf(stderr,"ZExceptionDLg::setButtonText-E-IVBTN error invalid button order <%d>\n",pButtonOrder);
  }
}
void
ZExceptionDLg::setThirdButton(const utf8String& pButtonText)
{
  ui->OtherBTn->setText(pButtonText.toCChar());
  ui->OtherBTn->setVisible(true);
}



int
ZExceptionDLg::info(ZExceptionBase& pException, QWidget *parent)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg(pException,parent);
  wDlg->ui->CancelBTn->setVisible(false);
  wDlg->setInfo();

  int wRet= wDlg->exec();
  wDlg->deleteLater();
  return wRet;
}
int
ZExceptionDLg::warning(ZExceptionBase& pException, QWidget *parent)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg(pException,parent);
  wDlg->ui->CancelBTn->setVisible(false);
  wDlg->setWarning();

  int wRet= wDlg->exec();
  wDlg->deleteLater();
  return wRet;
}

int
ZExceptionDLg::message(const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet._setMessage(pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display(wRet);
}

int
ZExceptionDLg::createErrno(const int pErrno,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet.getErrno(pErrno,pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display(wRet);
}
int
ZExceptionDLg::createFileError(FILE *pf,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet.getFileError(pf,pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display(wRet);
}
int
ZExceptionDLg::createAddrinfo(int pError,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet.getAddrinfo(pError,pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display(wRet);
}

int
ZExceptionDLg::displayLast(bool pDontShow)
{
  return display(ZException.last(),pDontShow);
}

int
ZExceptionDLg::display(const ZExceptionBase pException, bool pDontShow)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg(pException,pDontShow);
  if (wDlg->DoNotShow)
    {
    wDlg->deleteLater();
    return 0;
    }

  wDlg->ui->CancelBTn->setVisible(false);
  wDlg->ui->OtherBTn->setVisible(false);

  switch(pException.Severity)
  {
  case Severity_Information:
    wDlg->setInfo();
    break;
  case Severity_Warning:
    wDlg->setWarning();
    break;
  case Severity_Error:
    wDlg->setError();
    break;
  case Severity_Severe:
    wDlg->setSevere();
    break;
  case Severity_Fatal:
    wDlg->setFatal();
    break;
  case Severity_Highest:
    wDlg->setHighest();
    break;
  default:
    wDlg->setError();
    break;
  }

 int wRet= wDlg->exec();
  wDlg->deleteLater();
  return wRet;
}

