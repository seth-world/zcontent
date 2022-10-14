#include "zexceptiondlg.h"

#include "ui_zexceptiondlg.h"
#include <ztoolset/zexceptionmin.h>

#include <qevent.h>


#define __ICON_PATH__ "/home/gerard/Development/zbasetools/zqt/icons/"
#define __INFO_ICON__ "info.png"
#define __WARNING_ICON__ "warning.png"
#define __ERROR_ICON__ "error.png"
#define __SEVERE_ICON__ "errorcross.png"
#define __FATAL_ICON__ "skullhead.png"
#define __HIGHEST_ICON__ "graverip.png"

ZStatus DontShowStatus=ZS_NOTHING;

/** adhoc message */
ZExceptionDLg::ZExceptionDLg(const utf8VaryingString& pTitle,Severity_type pSeverity,
                            const utf8VaryingString& pMessage,
                            const utf8VaryingString* pComplement,
                            bool pDontShow) : QDialog(nullptr), ui(new Ui::ZExceptionDLg)
{
  DoNotShow=false;
  ui->setupUi(this);

  setWindowTitle(pTitle.toCChar());

  if (pDontShow)
  {
    ui->dontShowCKb->setVisible(true);
    //    QObject::connect (ui->dontShowCKb,&QCheckBox::clicked,this,[this]{ DontShowClicked(pException.Status); });
    QObject::connect (ui->dontShowCKb,&QCheckBox::clicked,this,&ZExceptionDLg::DontShowClicked);

  }
  else
    ui->dontShowCKb->setVisible(false);

  ui->ExceptionFRm->setVisible(false);

  applySeverity(pSeverity);

  ui->MessageTEd->setText(pMessage.toCChar());

  ui->CancelBTn->setText("Cancel");
  ui->OKBTn->setText("OK");
  ui->OtherBTn->setText("");

  if (pComplement && !pComplement->isEmpty())
    setAdditionalInfo(*pComplement);
  else
    ui->MoreBTn->setVisible(false);

  QObject::connect (ui->CancelBTn,&QAbstractButton::pressed,this,[this]{ reject(); });
  QObject::connect (ui->OKBTn,&QAbstractButton::pressed,this,[this]{ accept(); });
  QObject::connect (ui->OtherBTn,&QAbstractButton::pressed,this,[this]{ done(ZEDLG_Third); });
  QObject::connect (ui->MoreBTn,&QAbstractButton::pressed,this,[this]{ MoreClicked(); });
}

/** ZException message */

ZExceptionDLg::ZExceptionDLg(const utf8VaryingString& pTitle,const ZExceptionBase& pException, bool pDontShow) :
                                                QDialog(nullptr),
                                                ui(new Ui::ZExceptionDLg) {
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

  ui->CancelBTn->setText("Cancel");
  ui->OKBTn->setText("OK");
  ui->OtherBTn->setText("");

  if (!pException.Complement.isEmpty())
    setAdditionalInfo(pException.Complement);
  else
    ui->MoreBTn->setVisible(false);



  QObject::connect (ui->CancelBTn,&QAbstractButton::pressed,this,[this]{ reject(); });
  QObject::connect (ui->OKBTn,&QAbstractButton::pressed,this,[this]{ accept(); });
  QObject::connect (ui->OtherBTn,&QAbstractButton::pressed,this,[this]{ done(ZEDLG_Third); });
  QObject::connect (ui->MoreBTn,&QAbstractButton::pressed,this,[this]{ MoreClicked(); });

//  QObject::connect(this, SIGNAL(QDialog::resizeEvent(QResizeEvent*)), this, SLOT(resizeWindow(QResizeEvent*)));

}

void
ZExceptionDLg::init(const utf8VaryingString& pTitle,Severity_type pSeverity,ZStatus pStatus,const utf8VaryingString& pMessage, const utf8VaryingString& pComplement,bool pDontShow)
{
  DoNotShow=false;
  ui->setupUi(this);

  setWindowTitle(pTitle.toCChar());

  currentStatus=pStatus;
  if (pStatus==DontShowStatus)
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

/*
  ui->ModuleLBl->setText(pException.Module.toCChar());
  ui->ZStatusLBl->setText(decode_ZStatus(pException.Status));
  ui->SeverityLBl->setText(decode_Severity(pException.Severity));
*/
  ui->MessageTEd->setText(pMessage.toCChar());

  ui->CancelBTn->setText("Cancel");
  ui->OKBTn->setText("OK");
  ui->OtherBTn->setText("");

  if (!pComplement.isEmpty())
    setAdditionalInfo(pComplement);
  else
    ui->MoreBTn->setVisible(false);



  QObject::connect (ui->CancelBTn,&QAbstractButton::pressed,this,[this]{ reject(); });
  QObject::connect (ui->OKBTn,&QAbstractButton::pressed,this,[this]{ accept(); });
  QObject::connect (ui->OtherBTn,&QAbstractButton::pressed,this,[this]{ done(ZEDLG_Third); });
  QObject::connect (ui->MoreBTn,&QAbstractButton::pressed,this,[this]{ MoreClicked(); });

}



/**
 * @brief ZExceptionDLg::resizeEvent resizes two layout widgets :
 *  - first : only width -> keep in width only same distance from edge of dialog
 *  - second : width and height -> keep in width and height the initial distance from edges of dialog
 *  Overrides base widget resizeEvent()
 */
void ZExceptionDLg::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (FResizeInitial)
  {
    FResizeInitial=false;
    return;
  }

  QRect wR1 = ui->verticalLayoutWidget->geometry();

  int wVH=wR1.height();
  int wWMargin = (wRDlg.width()-wR1.width());  /* keep the same width margin */
  int wVW=pEvent->size().width() - wWMargin;

  ui->verticalLayoutWidget->resize(wVW,wVH);  /* does not expand in height */

  /* VVLWDg expands in width and height */
  if (VVLWDg)
    {
    QRect wR2 = VVLWDg->geometry();

    int wWMargin = (wRDlg.width()-wR2.width());
    int wVW=pEvent->size().width() - wWMargin;
    int wHMargin = wRDlg.height() - wR2.height();
    int wVH=pEvent->size().height() - wHMargin ;
    VVLWDg->resize(wVW,wVH);
    }
  return;
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

void ZExceptionDLg::MoreClicked()
{

  if (FMore)
  {
    FMore=false;

    if (VVLWDg!=nullptr)  /* deleting the father is enough for deleting the whole */
    {
      delete AdditionalTEd;
      AdditionalTEd=nullptr;
      delete LabLBl;
      LabLBl=nullptr;
      delete VLayout;
      VLayout=nullptr;
      delete VVLWDg;
      VVLWDg = nullptr;
    }

    this->resize(560, 270);
    ui->MoreBTn->setText(QObject::tr("More","ZExceptionDLg"));
    ui->MoreBTn->setVisible(true);

    QGuiApplication::processEvents();
    return;
  }

  FMore=true;

  this->resize(560, 450);

  if (VVLWDg==nullptr)
    {
    VVLWDg = new QWidget(this);
    VVLWDg->setObjectName(QString::fromUtf8("VVLWDg"));
    }
  VVLWDg->setGeometry(QRect(10, 260, 550, 170));
  if (VLayout==nullptr)
    {
    VLayout = new QVBoxLayout(VVLWDg);
    VLayout->setObjectName(QString::fromUtf8("VLayout"));
    }
  VLayout->setContentsMargins(0, 0, 0, 0);
  if (LabLBl==nullptr)
    {
    LabLBl = new QLabel(QObject::tr("Additional information","ZExceptionDLg"),VVLWDg);
    LabLBl->setObjectName(QString::fromUtf8("LabLBl"));
    LabLBl->setAlignment(Qt::AlignCenter);
    }
  VLayout->addWidget(LabLBl);
  if (AdditionalTEd==nullptr)
    {
    AdditionalTEd = new QTextEdit(VVLWDg);
    AdditionalTEd->setObjectName(QString::fromUtf8("ComplementTEd"));
    AdditionalTEd->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    AdditionalTEd->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
  VLayout->addWidget(AdditionalTEd);
  AdditionalTEd->setText(Additional.toCChar());

  VVLWDg->show();

  QGuiApplication::processEvents();
  ui->MoreBTn->setText(QObject::tr("Less","ZExceptionDLg"));
  return;
}

void ZExceptionDLg::setAdditionalInfo(const utf8VaryingString& pComp)
{
  Additional = pComp;
  ui->MoreBTn->setVisible(true);
}

void ZExceptionDLg::removeAdditionalInfo()
{
  Additional.clear();
  ui->MoreBTn->setVisible(false);
}

ZExceptionDLg::~ZExceptionDLg()
{
  delete ui;
}
void ZExceptionDLg::applySeverity(Severity_type pSeverity) {
  switch(pSeverity)
  {
  case Severity_Information:
    setInfo();
    break;
  case Severity_Warning:
    setWarning();
    break;
  case Severity_Error:
    setError();
    break;
  case Severity_Severe:
    setSevere();
    break;
  case Severity_Fatal:
    setFatal();
    break;
  case Severity_Highest:
    setHighest();
    break;
  default:
    setError();
    break;
  }
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
  ZExceptionDLg* wDlg=new ZExceptionDLg("Information",pException,false);
  wDlg->ui->CancelBTn->setVisible(false);
  wDlg->setInfo();

  int wRet= wDlg->exec();
  wDlg->deleteLater();
  return wRet;
}
int
ZExceptionDLg::warning(ZExceptionBase& pException, QWidget *parent)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg("Warning",pException,false);
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
  return display("Exception message",wRet);
}
int
ZExceptionDLg::messageWAdd(const char *pModule, ZStatus pStatus, Severity_type pSeverity,const utf8VaryingString& pAdd, const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet._setMessage(pModule,pStatus,pSeverity,pFormat,arglist);
  wRet.Complement = pAdd;
  va_end(arglist);
  return display("Exception message",wRet);
}
int
ZExceptionDLg::message2B(const char *pModule,
                          ZStatus pStatus,
                          Severity_type pSeverity,
                          const utf8String& pCancelText,
                          const utf8String& pOkText,
                          const char *pFormat,...)
{

  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet._setMessage(pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display2B("Exception message",wRet,pCancelText.toCChar(),pOkText.toCChar());
}
int
ZExceptionDLg::message2BWAdd(const char *pModule,
    ZStatus pStatus,
    Severity_type pSeverity,
    const utf8VaryingString& pAdd,
    const utf8String& pCancelText,
    const utf8String& pOkText,
    const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet._setMessage(pModule,pStatus,pSeverity,pFormat,arglist);
  wRet.Complement = pAdd;
  va_end(arglist);
  return display2B("Exception message",wRet,pCancelText.toCChar(),pOkText.toCChar());
}
int
ZExceptionDLg::message3B(const char *pModule,
                        ZStatus pStatus,
                        Severity_type pSeverity,
                        const utf8String& pButtonText,
                        const char *pFormat,...)
{

  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet._setMessage(pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display3B("Exception message",wRet,pButtonText);
}

int
ZExceptionDLg::createErrno(const int pErrno,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet.getErrno(pErrno,pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display("System exception",wRet);
}
int
ZExceptionDLg::createFileError(FILE *pf,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet.getFileError(pf,pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display("File exception",wRet);
}
int
ZExceptionDLg::createAddrinfo(int pError,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...)
{
  ZExceptionBase wRet;
  va_list arglist;
  va_start (arglist, pFormat);
  wRet.getAddrinfo(pError,pModule,pStatus,pSeverity,pFormat,arglist);
  va_end(arglist);
  return display("Network exception",wRet);
}

int
ZExceptionDLg::displayLast(const utf8VaryingString& pTitle,bool pDontShow)
{
  if (pTitle.isEmpty())
    return display("Last exception",ZException.last(),pDontShow);
  return display(pTitle,ZException.last(),pDontShow);
}

int
ZExceptionDLg::adhocMessage(const utf8VaryingString& pTitle, Severity_type pSeverity,const utf8VaryingString* pComplement,const char *pFormat,...) {
  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);

  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pSeverity,wMessage,pComplement,false);

  wDlg->ui->OKBTn->setVisible(true);
  wDlg->ui->OKBTn->setText("Close");
  wDlg->ui->CancelBTn->setVisible(false);
  wDlg->ui->OtherBTn->setVisible(false);

  wDlg->ui->TitleLBl->setText(pTitle.toCChar());

  int wRet= wDlg->exec();
  wDlg->deleteLater();
  return wRet;
}

int
ZExceptionDLg::adhocMessage2B(const utf8String&pTitle, Severity_type pSeverity,
    const utf8String& pCancelText,
    const utf8String& pOkText,
    const utf8VaryingString *pComplement,
    const char *pFormat,...) {

  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);

  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pSeverity,wMessage,pComplement);

  wDlg->ui->OKBTn->setVisible(true);
  if (pOkText.isEmpty())
    wDlg->ui->OKBTn->setText("Ok");
  else
    wDlg->ui->OKBTn->setText(pOkText.toCChar());
  wDlg->ui->CancelBTn->setVisible(true);
  if (pCancelText.isEmpty())
    wDlg->ui->CancelBTn->setText("Cancel");
  else
    wDlg->ui->CancelBTn->setText(pCancelText.toCChar());

  wDlg->ui->OtherBTn->setVisible(false);

  wDlg->ui->TitleLBl->setText(pTitle.toCChar());

  int wRet= wDlg->exec();
  wDlg->deleteLater();
  return wRet;
}

int
ZExceptionDLg::adhocMessage3B(const utf8String&pTitle, Severity_type pSeverity,const utf8String& pOtherText,const utf8String& pCancelText, const utf8String& pOkText,const utf8VaryingString *pComplement, const char *pFormat,...) {
  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);

  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pSeverity,wMessage,pComplement);

  wDlg->ui->OKBTn->setVisible(true);
  if (pOkText.isEmpty())
    wDlg->ui->OKBTn->setText("Ok");
  else
    wDlg->ui->OKBTn->setText(pOkText.toCChar());

  wDlg->ui->CancelBTn->setVisible(true);
  if (pCancelText.isEmpty())
    wDlg->ui->CancelBTn->setText("Cancel");
  else
    wDlg->ui->CancelBTn->setText(pCancelText.toCChar());

  if (pOtherText.isEmpty())
    wDlg->ui->OtherBTn->setText("Other");
  else
    wDlg->ui->OtherBTn->setText(pOtherText.toCChar());

  wDlg->ui->OtherBTn->setVisible(true);

  wDlg->ui->TitleLBl->setText(pTitle.toCChar());

  int wRet= wDlg->exec();
  wDlg->deleteLater();
  return wRet;
}

int
ZExceptionDLg::display(const utf8VaryingString &pTitle, const ZExceptionBase pException, bool pDontShow)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pException,pDontShow);
  if (wDlg->DoNotShow)
    {
    wDlg->deleteLater();
    return 0;
    }

  wDlg->ui->OKBTn->setVisible(true);
  wDlg->ui->OKBTn->setText("Close");
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

int
ZExceptionDLg::display2B(const utf8VaryingString &pTitle,const ZExceptionBase pException,
                        const char* pCancelText,
                        const char* pOKText)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pException,false);


  wDlg->ui->CancelBTn->setVisible(true);
  if (pCancelText)
    wDlg->ui->CancelBTn->setText(pCancelText);
  else
    wDlg->ui->CancelBTn->setText("Cancel");
  wDlg->ui->OKBTn->setVisible(true);
  if (pOKText)
    wDlg->ui->OKBTn->setText(pOKText);
  else
    wDlg->ui->OKBTn->setText("OK");

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
}//display2B



int
ZExceptionDLg::display3B(const utf8VaryingString& pTitle,const ZExceptionBase pException, const utf8String& pButtonText,
                        const char *pCancelText, const char *pOKText)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pException,false);

  wDlg->setThirdButton(pButtonText);

  wDlg->ui->CancelBTn->setVisible(true);
  if (pCancelText)
    wDlg->ui->CancelBTn->setText(pCancelText);
  else
    wDlg->ui->CancelBTn->setText("Cancel");
  wDlg->ui->OKBTn->setVisible(true);
  if (pCancelText)
    wDlg->ui->OKBTn->setText(pOKText);
  else
    wDlg->ui->OKBTn->setText("OK");


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
