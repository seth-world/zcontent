#include "zexceptiondlg.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

#include <QRect>

#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QCoreApplication>
#include <QGuiApplication>

#include "ui_zexceptiondlg.h"
#include <ztoolset/zexceptionmin.h>

#include <texteditmwn.h>
#include <ztoolset/zaierrors.h>

#include <qevent.h>

bool FixedFont = false;
#define __ICON_PATH__ "/home/gerard/Development/zbasetools/zqt/icons/"
#define __INFO_ICON__ "info.png"
#define __WARNING_ICON__ "warning.png"
#define __ERROR_ICON__ "error.png"
#define __SEVERE_ICON__ "errorcross.png"
#define __FATAL_ICON__ "skullhead.png"
#define __HIGHEST_ICON__ "graverip.png"

ZStatus DontShowStatus=ZS_NOTHING;

/** adhoc message */
ZExceptionDLg::ZExceptionDLg(const utf8VaryingString& pTitle, Severity_type pSeverity,
                            const utf8VaryingString& pMessage,
                            const utf8VaryingString* pComplement, bool pHtml,
                            bool pDontShow) : QDialog(nullptr)
{
  layoutSetup( pTitle,pDontShow);
  applySeverity(pSeverity,pTitle);

  ExceptionFRm->setVisible(false);

  if (pHtml)
    MessageTEd->setHtml(pMessage.toCChar());
  else
    MessageTEd->setText(pMessage.toCChar());
  if (pComplement && !pComplement->isEmpty())
    setAdditionalInfo(*pComplement,pHtml);
  else
    MoreBTn->setVisible(false);

  AdditionalTEd->setVisible(false);
  FMore=false;

  CancelBTn->setVisible(false);
  OKBTn->setText("Close");

  OtherBTn->setVisible(false);
}


/* ZException message */

ZExceptionDLg::ZExceptionDLg(const utf8VaryingString& pTitle,const ZExceptionBase& pException, bool pDontShow) : QDialog(nullptr) {

  layoutSetup( pTitle,pDontShow);
  DoNotShow=false;
  ExceptionFRm->setVisible(true);

  currentStatus=pException.Status;
  if (pException.Status==DontShowStatus)
  {
    DoNotShow=true;
    return;
  }

  if (pDontShow)
  {
    dontShowCKb->setVisible(true);
    //    QObject::connect (dontShowCKb,&QCheckBox::clicked,this,[this]{ DontShowClicked(pException.Status); });
    QObject::connect (dontShowCKb,&QCheckBox::clicked,this,&ZExceptionDLg::DontShowClicked);

  }
  else
    dontShowCKb->setVisible(false);


  ModuleLBl->setText(pException.Module.toCChar());
  ZStatusLBl->setText(decode_ZStatus(pException.Status));
  SeverityLBl->setText(decode_Severity(pException.Severity));

  MessageTEd->setText(pException.Message.toCChar());

  CancelBTn->setText("Close");
  OKBTn->setText("OK");
  OtherBTn->setText("");
  OtherBTn->setVisible(false);


  if (!pException.Complement.isEmpty())
    setAdditionalInfo(pException.Complement);
  else
    MoreBTn->setVisible(false);

  AdditionalTEd->setVisible(false);
  FMore=false;

  ErrlogBTn->setVisible(true);

  QObject::connect (CancelBTn,&QAbstractButton::pressed,this,[this]{ reject(); });
  QObject::connect (OKBTn,&QAbstractButton::pressed,this,[this]{ accept(); });
  QObject::connect (OtherBTn,&QAbstractButton::pressed,this,[this]{ done(ZEDLG_Third); });
  QObject::connect (MoreBTn,&QAbstractButton::pressed,this,[this]{ MoreClicked(); });
  QObject::connect (ErrlogBTn,&QAbstractButton::pressed,this,[this]{ ErrlogClicked(); });

}

void
ZExceptionDLg::layoutSetup( const utf8VaryingString& pTitle,
                            bool pDontShow)
{
  DoNotShow=false;

  QHBoxLayout *wButtonBoxHLy;
  QLabel *label_6;
  QSpacerItem *whorizontalSpacer;
  QLabel *label_7;
  QLabel *label_8;

  this->resize(587, 349);

  ExceptionFRm = new QFrame(this);
  ExceptionFRm->setGeometry(QRect(10, 10, 391, 81));
  ExceptionFRm->setFrameShape(QFrame::StyledPanel);
  ExceptionFRm->setFrameShadow(QFrame::Raised);
  ZStatusLBl = new QLabel(ExceptionFRm);
  ZStatusLBl->setGeometry(QRect(80, 30, 151, 16));
  label_6 = new QLabel(ExceptionFRm);
  label_6->setGeometry(QRect(10, 10, 54, 15));
  ModuleLBl = new QLabel(ExceptionFRm);
  ModuleLBl->setGeometry(QRect(80, 10, 301, 16));
  SeverityLBl = new QLabel(ExceptionFRm);
  SeverityLBl->setGeometry(QRect(80, 50, 141, 16));
  label_7 = new QLabel(ExceptionFRm);
  label_7->setGeometry(QRect(10, 50, 54, 15));
  label_8 = new QLabel(ExceptionFRm);
  label_8->setGeometry(QRect(10, 30, 54, 15));


  verticalLayoutWidget = new QWidget(this);

  QRect wR = ExceptionFRm->geometry();
//  layoutWidget->setGeometry(QRect(10, 300, 557, 25));
  wR.setY( wR.y() + wR.height() + 2 );
  wR.setWidth(560);
  wR.setHeight( 250 );

  verticalLayoutWidget->setGeometry(wR);

  QVBoxLayout *wMainVLayoutVLy=new QVBoxLayout(verticalLayoutWidget);
  verticalLayoutWidget->setLayout(wMainVLayoutVLy);

  QVBoxLayout *wVLayoutMsgVLy=new QVBoxLayout(verticalLayoutWidget);

  wMainVLayoutVLy->addLayout(wVLayoutMsgVLy);
  MessageTEd = new QTextEdit(this);
  wVLayoutMsgVLy->addWidget(MessageTEd);

  wButtonBoxHLy = new QHBoxLayout(verticalLayoutWidget);
  wButtonBoxHLy->setContentsMargins(0, 0, 0, 0);
  dontShowCKb = new QCheckBox(verticalLayoutWidget);
  wButtonBoxHLy->addWidget(dontShowCKb);

  whorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

  wButtonBoxHLy->addItem(whorizontalSpacer);
  ErrlogBTn = new QPushButton(verticalLayoutWidget);
  ErrlogBTn->setText("Error log");

  wButtonBoxHLy->addWidget(ErrlogBTn);

  MoreBTn = new QPushButton(verticalLayoutWidget);
  MoreBTn->setText("More");

  wButtonBoxHLy->addWidget(MoreBTn);

  OtherBTn = new QPushButton(verticalLayoutWidget);
  OtherBTn->setText("Other");

  wButtonBoxHLy->addWidget(OtherBTn);

  OKBTn = new QPushButton(verticalLayoutWidget);
  OKBTn->setObjectName(QString::fromUtf8("OKBTn"));

  wButtonBoxHLy->addWidget(OKBTn);

  CancelBTn = new QPushButton(verticalLayoutWidget);
  CancelBTn->setText("Close");

  wButtonBoxHLy->addWidget(CancelBTn);
  wMainVLayoutVLy->addLayout(wButtonBoxHLy);

  LogoLBl = new QLabel(this);
  LogoLBl->setGeometry(QRect(410, 10, 61, 61));
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(LogoLBl->sizePolicy().hasHeightForWidth());
  LogoLBl->setSizePolicy(sizePolicy);
  LogoLBl->setAutoFillBackground(false);
  LogoLBl->setStyleSheet(QString::fromUtf8("background:transparent"));
  LogoLBl->setFrameShape(QFrame::StyledPanel);
  LogoLBl->setScaledContents(true);


  dontShowCKb->setText(QCoreApplication::translate("ZExceptionDLg", "Don't show again", nullptr));
  ErrlogBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Error log", nullptr));
  MoreBTn->setText(QCoreApplication::translate("ZExceptionDLg", "More", nullptr));
  OtherBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Other", nullptr));
  OKBTn->setText(QCoreApplication::translate("ZExceptionDLg", "OK", nullptr));
  CancelBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Cancel", nullptr));
  LogoLBl->setText(QCoreApplication::translate("ZExceptionDLg", "logo", nullptr));
  ZStatusLBl->setText(QCoreApplication::translate("ZExceptionDLg", "ZStatus", nullptr));
  label_6->setText(QCoreApplication::translate("ZExceptionDLg", "Module", nullptr));
  ModuleLBl->setText(QCoreApplication::translate("ZExceptionDLg", "module", nullptr));
  SeverityLBl->setText(QCoreApplication::translate("ZExceptionDLg", "Severity", nullptr));
  label_7->setText(QCoreApplication::translate("ZExceptionDLg", "Severity", nullptr));
  label_8->setText(QCoreApplication::translate("ZExceptionDLg", "Status", nullptr));

  QVBoxLayout* wAddHLy=new QVBoxLayout(verticalLayoutWidget);
  AdditionalTEd = new QTextEdit(verticalLayoutWidget);
  wAddHLy->addWidget(AdditionalTEd);

  wMainVLayoutVLy->addLayout(wAddHLy);

  if (pDontShow)
  {
    dontShowCKb->setVisible(true);
    QObject::connect (dontShowCKb,&QCheckBox::clicked,this,&ZExceptionDLg::DontShowClicked);
  }
  else
    dontShowCKb->setVisible(false);

  if (FixedFont) {
    MessageTEd->setFontFamily(QString::fromUtf8("Courier"));
  }

  ExceptionFRm->setVisible(false);
  ErrlogBTn->setVisible(true);

  CancelBTn->setText("Cancel");
  OKBTn->setText("OK");
  OtherBTn->setText("");

  setWindowTitle(pTitle.toCChar());


  QObject::connect (CancelBTn,&QAbstractButton::pressed,this,[this]{ reject(); });
  QObject::connect (OKBTn,&QAbstractButton::pressed,this,[this]{ accept(); });
  QObject::connect (OtherBTn,&QAbstractButton::pressed,this,[this]{ done(ZEDLG_Third); });
  QObject::connect (MoreBTn,&QAbstractButton::pressed,this,[this]{ MoreClicked(); });
  QObject::connect (ErrlogBTn,&QAbstractButton::pressed,this,[this]{ ErrlogClicked(); });
}


void
ZExceptionDLg::setFixedFont(bool pOnOff) {
  FixedFont = pOnOff;
}
void
ZExceptionDLg::applyFixedFont() {
  MessageTEd->setFontFamily(QString::fromUtf8("Courier"));
  MessageTEd->setFontWeight( QFont::Bold);
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

  QRect wR1 = verticalLayoutWidget->geometry();


  int wHMargin = (wRDlg.height()-wR1.height());  /* keep the same height margin */
  int wVH=pEvent->size().height() - wHMargin;
//  int wVH=wR1.height();
  int wWMargin = (wRDlg.width()-wR1.width());  /* keep the same width margin */
  int wVW=pEvent->size().width() - wWMargin;

  verticalLayoutWidget->resize(wVW,wVH);  /* does not expand in height */

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
  if (dontShowCKb->isChecked())
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
    AdditionalTEd->setVisible(false);
    this->resize(560, 270);
    MoreBTn->setText(QObject::tr("More","ZExceptionDLg"));
    MoreBTn->setVisible(true);
    return;
  }

  FMore=true;
  this->resize(560, 450);
  AdditionalTEd->setVisible(true);
  MoreBTn->setText(QObject::tr("Less","ZExceptionDLg"));
  return;
}

void ZExceptionDLg::setErrorLog(ZaiErrors* pErrorLog) {
  ErrorLog=pErrorLog;
  ErrlogBTn->setVisible(true);
}

void ZExceptionDLg::ErrlogClicked()
{
  utf8VaryingString wStr;
  textEditMWn* wTE=new textEditMWn(this,TEOP_ShowLineNumbers | TEOP_NoFileLab);

  if (ErrorLog==nullptr) {
    for (long wi = ZException.count()-1;wi >= 0; wi--) {
      wTE->appendText(ZException.Tab[wi]->formatFullUserMessage(false));
    }

    wTE->show();
    return;
  }

  for (long wi = 0; wi < ErrorLog->count(); wi++){
    wStr.sprintf( "<%s> %s",decode_ZAIES(ErrorLog->Tab[wi]->Severity),ErrorLog->Tab[wi]->Message());
    wTE->appendText(wStr);
  }

  wTE->show();

  QGuiApplication::processEvents();
  return;
}

void ZExceptionDLg::setAdditionalInfo(const utf8VaryingString& pComp,bool pHtml)
{
  if (pHtml)
    AdditionalTEd->setHtml(pComp.toCChar());
  else
    AdditionalTEd->setText(pComp.toCChar());
  MoreBTn->setVisible(true);
}


ZExceptionDLg::~ZExceptionDLg()
{
  //delete ui;
}
void ZExceptionDLg::applySeverity(Severity_type pSeverity,const utf8VaryingString& pTitle) {
  switch(pSeverity)
  {
  case Severity_Information:
    setInfo(pTitle);
    break;
  case Severity_Warning:
    setWarning(pTitle);
    break;
  case Severity_Error:
    setError(pTitle);
    break;
  case Severity_Severe:
    setSevere(pTitle);
    break;
  case Severity_Fatal:
    setFatal(pTitle);
    break;
  case Severity_Highest:
    setHighest(pTitle);
    break;
  default:
    setError(pTitle);
    break;
  }
}

void ZExceptionDLg::setInfo(const utf8VaryingString& pTitle)
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __INFO_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  LogoLBl->setScaledContents(true);
  LogoLBl->setPixmap(wPxMp);
  if (pTitle.isEmpty())
    setWindowTitle("Information");
  else
    setWindowTitle(pTitle.toCChar());
}
void ZExceptionDLg::setWarning(const utf8VaryingString &pTitle)
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __WARNING_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  LogoLBl->setScaledContents(true);
  LogoLBl->setPixmap(wPxMp);
  if (pTitle.isEmpty())
    setWindowTitle("Warning");
  else
    setWindowTitle(pTitle.toCChar());
}
void ZExceptionDLg::setError(const utf8VaryingString &pTitle)
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __ERROR_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  LogoLBl->setScaledContents(true);
  LogoLBl->setPixmap(wPxMp);
  if (pTitle.isEmpty())
    setWindowTitle("Error");
  else
    setWindowTitle(pTitle.toCChar());
}
void ZExceptionDLg::setSevere(const utf8VaryingString &pTitle)
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __SEVERE_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  LogoLBl->setScaledContents(true);
  LogoLBl->setPixmap(wPxMp);
  if (pTitle.isEmpty())
    setWindowTitle("Severe error");
  else
    setWindowTitle(pTitle.toCChar());

}
void ZExceptionDLg::setFatal(const utf8VaryingString &pTitle)
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __FATAL_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  LogoLBl->setScaledContents(true);
  LogoLBl->setPixmap(wPxMp);
  if (pTitle.isEmpty())
    setWindowTitle("Fatal error");
  else
    setWindowTitle(pTitle.toCChar());

}
void ZExceptionDLg::setHighest(const utf8VaryingString &pTitle)
{
  uriString wImgFile = __ICON_PATH__;
  wImgFile += __HIGHEST_ICON__;

  QPixmap wPxMp;
  wPxMp.load(wImgFile.toCChar());
  LogoLBl->setScaledContents(true);
  LogoLBl->setPixmap(wPxMp);
  if (pTitle.isEmpty())
    setWindowTitle("Highest Error");
  else
    setWindowTitle(pTitle.toCChar());

}

void
ZExceptionDLg::setButtonText(int pButtonOrder,const utf8String& pButtonText)
{
  switch (pButtonOrder)
  {
  case 0:
    CancelBTn->setText(pButtonText.toCChar());
    return;
  case 1:
    OKBTn->setText(pButtonText.toCChar());
    return;
  case 2:
    OtherBTn->setText(pButtonText.toCChar());
    return;
  default:
    fprintf(stderr,"ZExceptionDLg::setButtonText-E-IVBTN error invalid button order <%d>\n",pButtonOrder);
  }
}
void
ZExceptionDLg::setThirdButton(const utf8String& pButtonText)
{
  OtherBTn->setText(pButtonText.toCChar());
  OtherBTn->setVisible(true);
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
ZExceptionDLg::adhocMessage(const utf8VaryingString& pTitle,
                            Severity_type pSeverity,
                            ZaiErrors* pErrorlog,
                            const utf8VaryingString* pComplement,
                            const char *pFormat,...) {
  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);

  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);

  return _adhocMessage(pTitle,pSeverity,pErrorlog,pComplement,false,wMessage);
}
int
ZExceptionDLg::adhocMessage(const utf8VaryingString& pTitle,
    Severity_type pSeverity,
    const char *pFormat,...) {
  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);

  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);

  return _adhocMessage(pTitle,pSeverity,nullptr,nullptr,false,wMessage);
}

int
ZExceptionDLg::adhocMessageHtml(const utf8VaryingString& pTitle,
                                Severity_type pSeverity,
                                ZaiErrors* pErrorlog,
                                const utf8VaryingString* pComplement,
                                const char *pFormat,...) {
  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);

  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);

  return _adhocMessage(pTitle,pSeverity,pErrorlog,pComplement,true,wMessage);
}
int
ZExceptionDLg::_adhocMessage( const utf8VaryingString& pTitle,
                              Severity_type pSeverity,
                              ZaiErrors* pErrorlog,
                              const utf8VaryingString* pComplement,
                              bool pHtml,
                              const utf8VaryingString& pMessage) {

  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pSeverity,pMessage,pComplement,pHtml,false);

  if (pErrorlog)
    wDlg->setErrorLog(pErrorlog);

  wDlg->OKBTn->setVisible(true);
  wDlg->OKBTn->setText("Close");
  wDlg->CancelBTn->setVisible(false);
  wDlg->OtherBTn->setVisible(false);

  if (FixedFont) {
    wDlg->applyFixedFont();
  }

  wDlg->setWindowTitle(pTitle.toCChar());
  //  wDlg->TitleLBl->setText(pTitle.toCChar());

  int wRet= wDlg->exec();

  setFixedFont(false);

  wDlg->deleteLater();
  return wRet;
}

int
ZExceptionDLg::adhocMessage2B(const utf8String&pTitle, Severity_type pSeverity,
    const utf8String& pCancelText, const utf8String& pOkText,
    const char *pFormat,...) {

  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);
  return _adhocMessage2B(pTitle,pSeverity,pCancelText,pOkText,nullptr,nullptr,false,wMessage);
}

int
ZExceptionDLg::adhocMessage2B(const utf8String&pTitle, Severity_type pSeverity,
    const utf8String& pCancelText, const utf8String& pOkText,
    ZaiErrors* pErrorlog, const utf8VaryingString *pComplement,
    const char *pFormat,...) {

  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);
  return _adhocMessage2B(pTitle,pSeverity,pCancelText,pOkText,pErrorlog,pComplement,false,wMessage);
}

int
ZExceptionDLg::adhocMessage2BHtml(const utf8String&pTitle, Severity_type pSeverity,
    const utf8String& pCancelText, const utf8String& pOkText,
    ZaiErrors* pErrorlog, const utf8VaryingString *pComplement,
    const char *pFormat,...) {

  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);
  return _adhocMessage2B(pTitle,pSeverity,pCancelText,pOkText,pErrorlog,pComplement,true,wMessage);
}
int
ZExceptionDLg::_adhocMessage2B(const utf8String&pTitle, Severity_type pSeverity,
    const utf8String& pCancelText, const utf8String& pOkText,
    ZaiErrors* pErrorlog, const utf8VaryingString *pComplement, bool pHtml,
    const utf8String& pMessage) {


  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pSeverity,pMessage,pComplement,pHtml);

  if (pErrorlog)
    wDlg->setErrorLog(pErrorlog);

  wDlg->OKBTn->setVisible(true);
  if (pOkText.isEmpty())
    wDlg->OKBTn->setText("Ok");
  else
    wDlg->OKBTn->setText(pOkText.toCChar());
  wDlg->CancelBTn->setVisible(true);
  if (pCancelText.isEmpty())
    wDlg->CancelBTn->setText("Cancel");
  else
    wDlg->CancelBTn->setText(pCancelText.toCChar());

  wDlg->OtherBTn->setVisible(false);

  //  wDlg->TitleLBl->setText(pTitle.toCChar());

  if (FixedFont) {
    wDlg->applyFixedFont();
  }
  int wRet= wDlg->exec();
  setFixedFont(false);
  wDlg->deleteLater();
  return wRet;
}

int
ZExceptionDLg::adhocMessage3B(const utf8String&pTitle, Severity_type pSeverity,
    const utf8String& pOtherText,const utf8String& pCancelText, const utf8String& pOkText,
    ZaiErrors* pErrorlog,const utf8VaryingString *pComplement, const char *pFormat,...) {
  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);


  return _adhocMessage3B(pTitle,pSeverity,pOtherText,pCancelText,pOkText,pErrorlog,pComplement,false,wMessage);
}

int
ZExceptionDLg::adhocMessage3B(const utf8String&pTitle, Severity_type pSeverity,
    const utf8String& pOtherText,const utf8String& pCancelText, const utf8String& pOkText,
     const char *pFormat,...) {
  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);


  return _adhocMessage3B(pTitle,pSeverity,pOtherText,pCancelText,pOkText,nullptr,nullptr,false,wMessage);
}
int
ZExceptionDLg::adhocMessage3BHtml(const utf8String&pTitle, Severity_type pSeverity,
    const utf8String& pOtherText,const utf8String& pCancelText, const utf8String& pOkText,
    ZaiErrors* pErrorlog,const utf8VaryingString *pComplement, const char *pFormat,...) {
  utf8VaryingString wMessage;
  va_list arglist;
  va_start (arglist, pFormat);
  wMessage.vsnprintf(2000,pFormat,arglist);
  va_end(arglist);


  return _adhocMessage3B(pTitle,pSeverity,pOtherText,pCancelText,pOkText,pErrorlog,pComplement,true,wMessage);
}

int
ZExceptionDLg::_adhocMessage3B(const utf8String&pTitle, Severity_type pSeverity,
                                const utf8String& pOtherText,const utf8String& pCancelText, const utf8String& pOkText,
                                ZaiErrors* pErrorlog,const utf8VaryingString *pComplement,bool pHtml,
                                const utf8String& pMessage) {


  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pSeverity,pMessage,pComplement,pHtml);

  if (pErrorlog)
    wDlg->setErrorLog(pErrorlog);

  wDlg->OKBTn->setVisible(true);
  if (pOkText.isEmpty())
    wDlg->OKBTn->setText("Ok");
  else
    wDlg->OKBTn->setText(pOkText.toCChar());

  wDlg->CancelBTn->setVisible(true);
  if (pCancelText.isEmpty())
    wDlg->CancelBTn->setText("Cancel");
  else
    wDlg->CancelBTn->setText(pCancelText.toCChar());

  if (pOtherText.isEmpty())
    wDlg->OtherBTn->setText("Other");
  else
    wDlg->OtherBTn->setText(pOtherText.toCChar());

  wDlg->OtherBTn->setVisible(true);

  //  wDlg->TitleLBl->setText(pTitle.toCChar());

  if (FixedFont) {
    wDlg->applyFixedFont();
  }

  int wRet= wDlg->exec();
  setFixedFont(false);
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

  if (!pException.Complement.isEmpty())
    wDlg->setAdditionalInfo(pException.Complement);

  wDlg->OKBTn->setVisible(true);
  wDlg->OKBTn->setText("Close");
  wDlg->CancelBTn->setVisible(false);
  wDlg->OtherBTn->setVisible(false);

  switch(pException.Severity)
  {
  case Severity_Information:
    wDlg->setInfo(pTitle);
    break;
  case Severity_Warning:
    wDlg->setWarning(pTitle);
    break;
  case Severity_Error:
    wDlg->setError(pTitle);
    break;
  case Severity_Severe:
    wDlg->setSevere(pTitle);
    break;
  case Severity_Fatal:
    wDlg->setFatal(pTitle);
    break;
  case Severity_Highest:
    wDlg->setHighest(pTitle);
    break;
  default:
    wDlg->setError(pTitle);
    break;
  }

  if (FixedFont)
    wDlg->MessageTEd->setFontFamily(QString::fromUtf8("Courier"));

 int wRet= wDlg->exec();
 setFixedFont(false);
 wDlg->deleteLater();
 return wRet;
}

int
ZExceptionDLg::display2B(const utf8VaryingString &pTitle,const ZExceptionBase pException,
                        const char* pCancelText,
                        const char* pOKText)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pException,false);


  wDlg->CancelBTn->setVisible(true);
  if (pCancelText)
    wDlg->CancelBTn->setText(pCancelText);
  else
    wDlg->CancelBTn->setText("Cancel");
  wDlg->OKBTn->setVisible(true);
  if (pOKText)
    wDlg->OKBTn->setText(pOKText);
  else
    wDlg->OKBTn->setText("OK");

  wDlg->OtherBTn->setVisible(false);

  switch(pException.Severity)
  {
  case Severity_Information:
    wDlg->setInfo(pTitle);
    break;
  case Severity_Warning:
    wDlg->setWarning(pTitle);
    break;
  case Severity_Error:
    wDlg->setError(pTitle);
    break;
  case Severity_Severe:
    wDlg->setSevere(pTitle);
    break;
  case Severity_Fatal:
    wDlg->setFatal(pTitle);
    break;
  case Severity_Highest:
    wDlg->setHighest(pTitle);
    break;
  default:
    wDlg->setError(pTitle);
    break;
  }

  if (FixedFont)
    wDlg->MessageTEd->setFontFamily(QString::fromUtf8("Courier"));

  int wRet= wDlg->exec();
  setFixedFont(false);
  wDlg->deleteLater();
  return wRet;
}//display2B



int
ZExceptionDLg::display3B(const utf8VaryingString& pTitle,const ZExceptionBase pException, const utf8String& pButtonText,
                        const char *pCancelText, const char *pOKText)
{
  ZExceptionDLg* wDlg=new ZExceptionDLg(pTitle,pException,false);

  wDlg->setThirdButton(pButtonText);

  wDlg->CancelBTn->setVisible(true);
  if (pCancelText)
    wDlg->CancelBTn->setText(pCancelText);
  else
    wDlg->CancelBTn->setText("Cancel");
  wDlg->OKBTn->setVisible(true);
  if (pCancelText)
    wDlg->OKBTn->setText(pOKText);
  else
    wDlg->OKBTn->setText("OK");


  switch(pException.Severity)
  {
  case Severity_Information:
    wDlg->setInfo(pTitle);
    break;
  case Severity_Warning:
    wDlg->setWarning(pTitle);
    break;
  case Severity_Error:
    wDlg->setError(pTitle);
    break;
  case Severity_Severe:
    wDlg->setSevere(pTitle);
    break;
  case Severity_Fatal:
    wDlg->setFatal(pTitle);
    break;
  case Severity_Highest:
    wDlg->setHighest(pTitle);
    break;
  default:
    wDlg->setError(pTitle);
    break;
  }

  if (FixedFont)
    wDlg->MessageTEd->setFontFamily(QString::fromUtf8("Courier"));

  int wRet= wDlg->exec();
  setFixedFont(false);
  wDlg->deleteLater();
  return wRet;
}
