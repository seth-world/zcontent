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
  ZEDLG_Rejected = 0,
  ZEDLG_Accepted = 1,
  ZEDLG_Third    = 2
};

#include <ztoolset/utfvaryingstring.h>
typedef utf8VaryingString utf8String;
typedef utf16VaryingString utf16String;
typedef utf32VaryingString utf32String;
#include <ztoolset/uristring.h>

class ZExceptionBase;

class ZExceptionDLg : public QDialog
{
  Q_OBJECT

public:
  explicit ZExceptionDLg(const ZExceptionBase &pException, bool pDontShow=false);
  ~ZExceptionDLg();

  void setInfo();
  void setWarning();
  void setError();
  void setSevere();
  void setFatal();
  void setHighest();

  void setButtonText(int pOrder,const utf8String& pButtonText);
  void setThirdButton(const utf8String& pButtonText);

  static int message(const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...);
  static int createErrno(const int pErrno,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...);
  static int createFileError(FILE *pf,const char *pModule, ZStatus pStatus, Severity_type pSeverity,const char *pFormat,...);
  static int createAddrinfo(int pError, const char *pModule, ZStatus pStatus, Severity_type pSeverity, const char *pFormat,...);


  static int displayLast( bool pDontShow = false);

  static int display(const ZExceptionBase pException, bool pDontShow = false);


  static int info(ZExceptionBase& pException, QWidget *parent = nullptr);
  static int warning(ZExceptionBase& pException, QWidget *parent = nullptr);
  static int error(ZExceptionBase& pException, QWidget *parent = nullptr);
  static int severe(ZExceptionBase& pException, QWidget *parent = nullptr);
  static int fatal(ZExceptionBase& pException, QWidget *parent = nullptr);

  static int info3B(ZExceptionBase& pException,const utf8String& pButtonText, QWidget *parent = nullptr);
  static int warning3B(ZExceptionBase& pException,const utf8String& pButtonText, QWidget *parent = nullptr);
  static int error3B(ZExceptionBase& pException,const utf8String& pButtonText, QWidget *parent = nullptr);
  static int severe3B(ZExceptionBase& pException,const utf8String& pButtonText, QWidget *parent = nullptr);
  static int fatal3B(ZExceptionBase& pException,const utf8String& pButtonText, QWidget *parent = nullptr);


  static void resetDontShow() ;

public Q_SLOTS:

  void buttonPressed(int pValue);

  void DontShowClicked();

private:
  ZStatus currentStatus=ZS_NOTHING;

  bool DoNotShow=false;

  Ui::ZExceptionDLg *ui;
};




#endif // ZEXCEPTIONDLG_H
