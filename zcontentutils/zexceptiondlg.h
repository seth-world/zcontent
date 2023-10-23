#ifndef ZEXCEPTIONDLG_H
#define ZEXCEPTIONDLG_H

#include <QDialog>

#include <ztoolset/utfvaryingstring.h>
typedef utf8VaryingString utf8String;
typedef utf16VaryingString utf16String;
typedef utf32VaryingString utf32String;
#include <ztoolset/uristring.h>


class QCheckBox;
class QFrame;

enum ZExceptionDlg_type : uint16_t
{
  ZEXDLG_Nothing = 0,
  ZEXDLG_YesNo = 1,
  ZEXDLG_OkCancel = 2,
  ZEXDLG_Other = 4,

  ZEXDLG_ButtonMask=0x00FF,


  ZEXDLG_Info = 0x0100,

  ZEXDLG_Warning = 0x0200,
  ZEXDLG_Error = 0x0300,
  ZEXDLG_Severe = 0x0400,
  ZEXDLG_Fatal = 0x0500,
  ZEXDLG_Highest =0x0600,

  ZEXDLG_SeverityMask=0xFF00
};
/*
namespace Ui {
class ZExceptionDLg;
}
*/

class ZExceptionDLgOptions {
public:
  ZExceptionDLgOptions() {}

  utf8VaryingString* OKBtnText=nullptr;
  utf8VaryingString* CancelBtnText=nullptr;
  utf8VaryingString* ThirdBtnText=nullptr;

  utf8VaryingString* OKLabel=nullptr;
  utf8VaryingString* CancelLabel=nullptr;
  utf8VaryingString* ThirdLabel=nullptr;
};

enum ZEDLGReturn : int{
  ZEDLG_Rejected = QDialog::Rejected,
  ZEDLG_Accepted = QDialog::Accepted,
  ZEDLG_Third    = 0xFF
};



class ZExceptionBase;
class QWidget;
class QVBoxLayout;
class QLabel;
class QTextEdit;

class ZExceptionDLg : public QDialog
{
  Q_OBJECT

public:
  explicit ZExceptionDLg(const utf8VaryingString& pTitle,const ZExceptionBase &pException, bool pDontShow=false);
  /* CTOR For adhoc message */
  explicit ZExceptionDLg( const utf8VaryingString& pTitle, Severity_type pSeverity,
                          const utf8VaryingString& pMessage,
                          const utf8VaryingString *pComplement=nullptr,
                          bool pHtml=false,
                          bool pDontShow=false);

  ~ZExceptionDLg();

  void setErrorLogBTn(ZaiErrors* pErrorLog) ;


  static void setFixedFont(bool pOnOff=true);

  void applyFixedFont();

  void applySeverity(Severity_type pSeverity, const utf8VaryingString &pTitle);

  void setInfo(const utf8VaryingString &pTitle);
  void setQuestion(const utf8VaryingString &pTitle);
  void setWarning(const utf8VaryingString &pTitle);
  void setError(const utf8VaryingString &pTitle);
  void setSevere(const utf8VaryingString &pTitle);
  void setFatal(const utf8VaryingString &pTitle);
  void setHighest(const utf8VaryingString &pTitle);

  void setButtonText(int pOrder,const utf8String& pButtonText);
  void setThirdButton(const utf8String& pButtonText);

  static int adhocMessage(const utf8VaryingString &pTitle, Severity_type pSeverity, const char *pFormat,...);
  /**
   * @brief adhocMessage  displays an adhoc message(not dependant from ZException) with one button <close>
   * @param pTitle        displayed dialog title
   * @param pSeverity     induces the severity logo (one of info,warning, error, fatal) in the top right corner of dialog
   * @param pErrorlog     when exists (not nullptr) must point to a valid Error log object. Then an <ErrorLog> button becomes visible.
   * Error log full content is displayed in a separate window when clicked.
   * @param pComplement   a string with potential complement to be displayed.
   * If exists (not nullptr) then <More> button becomes visible
   * and complement is displayed in a separate text widget when more is clicked.
   * @param pFormat       the main message with variadic content
   * @return  a return type corresponding to the button pressed
   */
  static int adhocMessage(const utf8VaryingString &pTitle, Severity_type pSeverity,
                          ZaiErrors *pErrorlog, const utf8VaryingString *pComplement,
                          const char *pFormat,...);
  static int adhocMessageHtml(const utf8VaryingString &pTitle, Severity_type pSeverity,
                              ZaiErrors *pErrorlog, const utf8VaryingString *pComplement,
                              const char *pFormat,...);
  static int _adhocMessage(const utf8VaryingString &pTitle, Severity_type pSeverity,
                            ZaiErrors *pErrorlog, const utf8VaryingString *pComplement, bool pHtml,
                            const utf8VaryingString &pMessage);
  /**
   * @brief adhocMessage2B displays an adhoc message(not dependant from ZException) with two buttons, cancel and ok button.
   * @param pTitle        displayed dialog title
   * @param pSeverity     induces the severity logo (one of severity levels info,warning, error, fatal,highest) in the top right corner of dialog
   * @param pCancelText   a string with cancel button displayed text
   * @param pOkText       a string with OK button displayed text
   * @param pComplement   a string with potential complement to be displayed
   * @param pFormat       the main message with variadic content
   * @return  a return type corresponding to the button pressed (Dialog::Rejected or Dialog::Accepted)
   */
  static int adhocMessage2B(const utf8String&pTitle, Severity_type pSeverity,
      const utf8String& pCancelText, const utf8String& pOkText,
      const char *pFormat,...);

  static int adhocMessage2B(const utf8String&pTitle, Severity_type pSeverity,
      const utf8String& pCancelText, const utf8String& pOkText,
      ZaiErrors *pErrorlog, const utf8VaryingString *pComplement,
      const char *pFormat,...);

  static int adhocMessage2BHtml(const utf8String&pTitle, Severity_type pSeverity,
      const utf8String& pCancelText, const utf8String& pOkText,
      ZaiErrors *pErrorlog, const utf8VaryingString *pComplement,
      const char *pFormat,...);

  static int _adhocMessage2B(const utf8VaryingString &pTitle, Severity_type pSeverity,
      const utf8String& pCancelText, const utf8String& pOkText,
      ZaiErrors *pErrorlog, const utf8VaryingString *pComplement, bool pHtml,
      const utf8VaryingString &pMessage);

  static int adhocMessage3B(const utf8String&pTitle, Severity_type pSeverity,
      const utf8String& pOtherText, const utf8String& pCancelText, const utf8String& pOkText,
      const char *pFormat,...);
  static int adhocMessage3B(const utf8String&pTitle, Severity_type pSeverity,
      const utf8String& pOtherText, const utf8String& pCancelText, const utf8String& pOkText,
      ZaiErrors *pErrorlog, const utf8VaryingString *pComplement,
      const char *pFormat,...);

  static int adhocMessage3BHtml(const utf8String&pTitle, Severity_type pSeverity,
      const utf8String& pOtherText, const utf8String& pCancelText, const utf8String& pOkText,
      ZaiErrors *pErrorlog, const utf8VaryingString *pComplement,
      const char *pFormat,...);

  static int _adhocMessage3B(const utf8VaryingString &pTitle, Severity_type pSeverity,
      const utf8String& pOtherText,const utf8String& pCancelText, const utf8String& pOkText,
      ZaiErrors *pErrorlog, const utf8VaryingString *pComplement, bool pHtml,
      const utf8VaryingString &pMessage);

  static int message(const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...);
  static int message2B(const char *pModule, ZStatus pStatus, Severity_type pSeverity, const utf8String& pCancelText, const utf8String& pOkText, const char *pFormat,...);
  static int message3B(const char *pModule, ZStatus pStatus, Severity_type pSeverity,const utf8String& pButtonText, const char *pFormat,...);


  /** @brief messageWAdd one button message with additional information */
  static int messageWAdd(const char *pModule,
                        ZStatus pStatus,
                        Severity_type pSeverity,
                        const utf8VaryingString& pAdd,
                        const char *pFormat,...);
  /** @brief messageWAdd 2 buttons message with additional information */
  static int message2BWAdd( const char *pModule,
                            ZStatus pStatus,
                            Severity_type pSeverity,
                            const utf8VaryingString& pAdd,
                            const utf8String& pCancelText,
                            const utf8String& pOkText,
                            const char *pFormat,...);
  static int createErrno(const int pErrno,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...);
  static int createFileError(FILE *pf,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...);
  static int createAddrinfo(int pError, const char *pModule, ZStatus pStatus, Severity_type pSeverity, const char *pFormat,...);


  static int displayLast(const utf8VaryingString &pTitle=utf8VaryingString());

  static int display(const utf8VaryingString& pTitle, const ZExceptionBase pException);
  static int display2B(const utf8VaryingString& pTitle,const ZExceptionBase pException, const char *pCancelText=nullptr, const char *pOKText=nullptr);
  static int display3B(const utf8VaryingString &pTitle, const ZExceptionBase pException, const utf8String& pButtonText, const char *pCancelText=nullptr, const char *pOKText=nullptr);


  static void resetDontShow() ;

  void setAdditionalInfo(const utf8VaryingString& pComp, bool pHtml=false);
  void removeAdditionalInfo ();

public Q_SLOTS:

  void buttonPressed(int pValue);
  void DontShowClicked();
  void MoreClicked();
  void ErrlogClicked();
  void ZExceptionClicked();
  void ZExceptionPurgeClicked();

  void layoutSetup(const utf8VaryingString &pTitle, bool pDontShow);

private Q_SLOTS :
  void resizeEvent(QResizeEvent* pEvent);
public:
  ZaiErrors* ErrorLog=nullptr;


private:
  ZStatus currentStatus=ZS_INVALIDSTATUS;

  bool FResizeInitial=true;
  bool DoNotShow=false;
  bool FMore=false;
//  Ui::ZExceptionDLg *ui;

private:
  QWidget *verticalLayoutWidget=nullptr;

  QCheckBox *dontShowCKb=nullptr;

  QPushButton *ErrlogBTn=nullptr;
  QPushButton *ZExceptionBTn=nullptr;
  QPushButton *ZExceptionPurgeBTn=nullptr;
  QPushButton *MoreBTn=nullptr;
  QPushButton *OtherBTn=nullptr;
  QPushButton *OKBTn=nullptr;
  QPushButton *CancelBTn=nullptr;
  QLabel *LogoLBl=nullptr;
  QFrame *ExceptionFRm=nullptr;
  QLabel *ZStatusLBl=nullptr;

  QLabel *ModuleLBl=nullptr;
  QLabel *SeverityLBl=nullptr;

//  QLabel *TitleLBl=nullptr;
  QTextEdit *MessageTEd=nullptr;
  QWidget *VVLWDg=nullptr;
  QVBoxLayout *VLayout=nullptr;
  QLabel*   LabLBl=nullptr;
  QTextEdit * AdditionalTEd=nullptr;

  const int cst_Width = 590;
  const int cst_Height = 350;
  const int cst_HighHeight = 450;
};

utf8VaryingString escapeHtmlSeq(const char* pString);
utf8VaryingString escapeHtmlSeq(const utf8VaryingString& pString);

#endif // ZEXCEPTIONDLG_H
