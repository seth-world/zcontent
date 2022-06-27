#ifndef ZEXCEPTIONDLG_H
#define ZEXCEPTIONDLG_H

#include <QDialog>

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

namespace Ui {
class ZExceptionDLg;
}

enum ZEDLGReturn : int{
  ZEDLG_Rejected = QDialog::Rejected,
  ZEDLG_Accepted = QDialog::Accepted,
  ZEDLG_Third    = 0xFF
};

#include <ztoolset/utfvaryingstring.h>
typedef utf8VaryingString utf8String;
typedef utf16VaryingString utf16String;
typedef utf32VaryingString utf32String;
#include <ztoolset/uristring.h>

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
  explicit ZExceptionDLg(const utf8VaryingString& pTitle, Severity_type pSeverity,
      const utf8VaryingString& pMessage,
      const utf8VaryingString *pComplement=nullptr,
      bool pDontShow=false);
  ~ZExceptionDLg();

  void init(const utf8VaryingString& pTitle,Severity_type pSeverity,ZStatus pStatus,const utf8VaryingString& pMessage, const utf8VaryingString& pComplement,bool pDontShow)  ;

  void applySeverity(Severity_type pSeverity);

  void setInfo();
  void setWarning();
  void setError();
  void setSevere();
  void setFatal();
  void setHighest();

  void setButtonText(int pOrder,const utf8String& pButtonText);
  void setThirdButton(const utf8String& pButtonText);
  /**
   * @brief adhocMessage  displays an adhoc message(not dependant from ZException) with one button <close>
   * @param pTitle        displayed dialog title
   * @param pSeverity     induces the severity logo (one of info,warning, error, fatal) in the top right corner of dialog
   * @param pComplement   a string with potential complement to be displayed
   * @param pFormat       the main message with variadic content
   * @return  a return type corresponding to the button pressed
   */
  static int adhocMessage(const utf8VaryingString &pTitle, Severity_type pSeverity, const utf8VaryingString *pComplement, const char *pFormat,...);
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
  static int adhocMessage2B(const utf8String&pTitle, Severity_type pSeverity,const utf8String& pCancelText, const utf8String& pOkText,const utf8VaryingString *pComplement, const char *pFormat,...);
  static int adhocMessage3B(const utf8String&pTitle, Severity_type pSeverity,const utf8String& pOtherText,const utf8String& pCancelText, const utf8String& pOkText,const utf8VaryingString *pComplement, const char *pFormat,...);

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


  static int displayLast( bool pDontShow = false);

  static int display(const utf8VaryingString& pTitle,const ZExceptionBase pException, bool pDontShow = false);
  static int display2B(const utf8VaryingString& pTitle,const ZExceptionBase pException, const char *pCancelText=nullptr, const char *pOKText=nullptr);
  static int display3B(const utf8VaryingString &pTitle, const ZExceptionBase pException, const utf8String& pButtonText, const char *pCancelText=nullptr, const char *pOKText=nullptr);


  static int info(ZExceptionBase& pException, QWidget *parent = nullptr);
  static int warning(ZExceptionBase& pException, QWidget *parent = nullptr);
  static int error(ZExceptionBase& pException, QWidget *parent = nullptr);
  static int severe(ZExceptionBase& pException, QWidget *parent = nullptr);
  static int fatal(ZExceptionBase& pException, QWidget *parent = nullptr);



  static void resetDontShow() ;

  void setAdditionalInfo(const utf8VaryingString& pComp);
  void removeAdditionalInfo ();

public Q_SLOTS:

  void buttonPressed(int pValue);
  void DontShowClicked();
  void MoreClicked();

private Q_SLOTS :
  void resizeEvent(QResizeEvent* pEvent);
public:
  QWidget *VVLWDg=nullptr;
  QVBoxLayout *VLayout=nullptr;
  QLabel*   LabLBl=nullptr;
  QTextEdit * AdditionalTEd=nullptr;
private:
  ZStatus currentStatus=ZS_INVALIDSTATUS;

  utf8VaryingString Additional;

  bool FResizeInitial=true;
  bool DoNotShow=false;
  bool FMore=false;
  Ui::ZExceptionDLg *ui;
};




#endif // ZEXCEPTIONDLG_H
