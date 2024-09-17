#ifndef ZHELP_H
#define ZHELP_H
#include <iostream>
#include <QMainWindow>
#include <ztoolset/zstatus.h>
#include <functional>

#include <QTextCursor>
#include <stdint.h>
#include <ztoolset/zaierrors.h>


#include <zqt/zqtwidget/ztextformattuple.h>



extern const int cst_MessageDuration ;

#define __CLOSE_CALLBACK__(__NAME__)  std::function<void (QEvent*)> __NAME__
#define __MORE_CALLBACK__(__NAME__)  std::function<void ()> __NAME__
#define __FILTRATE_CALLBACK__(__NAME__)  std::function<bool (const utf8VaryingString&)> __NAME__

class QVBoxLayout;
class QHBoxLayout;

class QSpacerItem;
class QLineEdit;
class QLabel;
class QPushButton;

class QStatusBar;
class QMenuBar;

class uriString;
class utf8VaryingString;
class ZQTextBrowser;
class QTextCursor;
class QTextCharFormat;

class QActionGroup;
class QAction;
class QMenu;
#ifndef __ZTEXTEDITOPTION__
#define __ZTEXTEDITOPTION__
enum ZTextEditOption : uint32_t {
  TEOP_Nothing        = 0,
  TEOP_CloseBtnHide   = 0x01,     /* if set Close button hides dialog, while if not set close button closes */
  TEOP_ShowLineNumbers= 0x04,
  TEOP_NoCloseBtn     = 0x08,      /* no close button */
  TEOP_NoFileLab      = 0x10      /* hide label closed / open file */
};
#endif // __ZTEXTEDITOPTION__
//class ZContentVisuMain;
//class ZLineNumberArea;
class QTextDocument;
class ZQTableView;

namespace zbs {


class ZHelpKeyword {
public:
    ZHelpKeyword() {}
    ZHelpKeyword(const ZHelpKeyword& pIn) { _copyFrom(pIn);}
    ~ZHelpKeyword() {
    }

    void resetToNull()
    {
        Html.clear();
        Keyword.clear();
        Brief.clear();
    }


    ZHelpKeyword& _copyFrom(const ZHelpKeyword& pIn) {
        Html=pIn.Html;
        Keyword = pIn.Keyword;
        Brief = pIn.Brief;
        return *this;
    }

    utf8VaryingString getKeyword()
    {
        return Keyword;
    }
    uriString getHtml()
    {
        return Html;
    }
    utf8VaryingString getBrief()
    {
        return Brief;
    }
    utf8VaryingString  Keyword;
    utf8VaryingString  Brief;
    uriString          Html;
};


class ZHelp : public QMainWindow
{
  Q_OBJECT

  friend class ZHelpFreetextSearchDLg ;

  void _init(uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack) );
public:
  explicit ZHelp(QWidget*parent ,uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack)=nullptr);
  explicit ZHelp(QWidget*parent );
  explicit ZHelp(const utf8VaryingString& pTitle , QWidget*parent);
  ~ZHelp();

  ZStatus setup(const utf8VaryingString &pDomain, ZaiErrors* pErrorLog);
  ZStatus XmlLoad(uriString& pXmlFile, const utf8VaryingString &pDomain, ZaiErrors* pErrorLog);

  void displayNoHelp();

  /* source file must explicitely be html */
  void setHtmlSource(const uriString& pSource) ;
  void setHtmlSourceWithHighlight(const uriString& pSource,const utf8VaryingString& pSearch);

  void setSource(const uriString& pTextFile);

  ZStatus setTextFromFile(const uriString& pTextFile);
  void setText(const utf8VaryingString& pText, const utf8VaryingString &pTitle);
  void appendText(const utf8VaryingString& pText);
  void appendText(const char *pText,...);

  void appendTextColor(QColor pBkgndColor,QColor pTextColor,const utf8VaryingString& pText);
  void appendTextColor(QColor pBkgndColor,QColor pTextColor,const char *pText,...);
  void appendTextColor(QColor pTextColor,const utf8VaryingString& pText) {
    appendTextColor(QColor(),pTextColor,pText);
  }
  void appendTextColor(QColor pTextColor,const char *pText,...);

  void appendHtml(const char *pText,...);

  void setPositionOrigin ();

  void filtrate() ;

  void highlightWordAtOffset(int pOffset);
  void highlightCharactersAtOffset(int pOffset,int pLength);
  void highlightLine(int pLine);
  void resetLine(int pLine);
  void resetWordAtOffset(int pOffset);

  void highlightAll(const utf8VaryingString &pSearchString,QColor pBackgnd=Qt::yellow,QColor pForegnd=Qt::black);
  void resetHighLightAll();

  void clear() ;

  void registerCloseCallback(__CLOSE_CALLBACK__(pCloseCallBack)) {CloseCallBack=pCloseCallBack;}
  void registerMoreCallback(__MORE_CALLBACK__(pCloseCallBack)) {MoreCallBack=pCloseCallBack;}
  void registerFiltrateCallback(__FILTRATE_CALLBACK__(pFiltrateCallBack)) {FiltrateCallBack=pFiltrateCallBack;}

  void setFiltrateActive(bool pOnOff) ;

  void setCloseButtonRole () ;

  void resizeEvent(QResizeEvent*) override;
  void closeEvent(QCloseEvent *event) override;
  void keyPressEvent(QKeyEvent *pEvent) override;

/*
  void useLineNumbers(bool pOnOff) ;

  bool hasLineNumbers();
  void lineNumbersOnOff();
*/
  void wordWrap();

  void displayColorCallBack(uint8_t pSeverity, const utf8VaryingString &pMessage);
  void displayCallBack(const utf8VaryingString &pMessage);

  void registerDisplayColorCallBack(ZaiErrors* pErrorLog)
  {
      ErrorLog=pErrorLog;
      pErrorLog->setDisplayColorCallBack(std::bind(&ZHelp::displayColorCallBack,this,std::placeholders::_1,std::placeholders::_2));
  }

  private Q_SLOTS:
  void morePressed();
  void closePressed();
  void wrapPressed();
  void filterPressed();
//  void lineNumbersBTnClicked();
  void searchMainPressed();
  void searchPressed();
  void searchReturnPressed();

  void MenuAction(QAction* pAction);

public:

  bool hasErrorLog() {return ErrorLog!=nullptr;}

  void setCloseCallBack(__CLOSE_CALLBACK__(pCloseCallBack))
  {
      CloseCallBack = pCloseCallBack;
  }

  void displayKeyword(const utf8VaryingString& pToSearch);
//  void freeTextSearch(const utf8VaryingString& pToSearch);

private:
  void search(const utf8VaryingString &pSearchString);
  void searchFirst(const utf8VaryingString &pSearchString);


  ZArray<ZTextFormatTuple> TextFormatList;

  /* ZHelp internal data */
  utf8VaryingString HelpDomain;
  uriString RootDirectory;
  uriString HelpMainHtml;
  ZArray<ZHelpKeyword> KeywordList;
  ZArray<utf8VaryingString> DomainList;

  ZaiErrors* ErrorLog=nullptr;
private:
  QWidget *centralwidget=nullptr;
  QWidget *verticalLayoutWidget=nullptr;
  QVBoxLayout *verticalLayout=nullptr;
  QHBoxLayout *horizontalLayout=nullptr;
  QVBoxLayout *KeywordLayout=nullptr;
//  QLabel *ClosedLBl=nullptr;
  QSpacerItem *horizontalSpacer=nullptr;
  QHBoxLayout *horizontalLayout_2=nullptr;
  QPushButton *searchBTn=nullptr;
  QLineEdit *searchLEd=nullptr;
  QPushButton *searchMainBTn=nullptr;
  QPushButton *filterBTn=nullptr;
//  QPushButton *lineNumbersBTn=nullptr;
  QPushButton *homeBTn=nullptr;
  QPushButton *backwardBTn=nullptr;
  QPushButton *forwardBTn=nullptr;
  QPushButton *wrapBTn=nullptr;
  QPushButton *closeBTn=nullptr;
 // ZQPlainTextEdit *textPTe=nullptr;
  QStatusBar *statusbar=nullptr;
  QMenuBar *menubar=nullptr;

//  ZQTableView* KeywordTBv=nullptr;

  QMenu*        genMEn=nullptr;
  QAction*      writeQAc=nullptr;
  QAction*      clearQAc=nullptr;
//  QAction*      lineNbQAc=nullptr;
  QAction*      wrapQAc=nullptr;
  QAction*      quitQAc=nullptr;

  QAction*      zoomInQAc=nullptr;
  QAction*      zoomOutQAc=nullptr;

  QActionGroup* menuActionQAg=nullptr;

  QMenu*        HistoryMEn=nullptr;
  QAction*      HomeQAc=nullptr;
  QAction*      BackwardQAc=nullptr;
  QAction*      ForwardQAc=nullptr;

  QAction*      ShowKeywordsQAc=nullptr;
  QAction*      SearchKeywordsQAc=nullptr;
 // QAction*      HideKeywordsQAc=nullptr;

private:
  QTextDocument*  searchDoc=nullptr;
  QTextCursor     searchCursor;
  QTextCursor     oldSearchCursor;

  int             searchOffset=0;

  __CLOSE_CALLBACK__(CloseCallBack) =nullptr;
  __MORE_CALLBACK__(MoreCallBack) =nullptr;
  __FILTRATE_CALLBACK__(FiltrateCallBack)=nullptr;

  bool FWrap=false;
  bool FSearch=false;

  bool              FiltrateActive=false;
  uint32_t          Options=TEOP_Nothing;
  ZQTextBrowser*    TextTBr=nullptr;
  bool              FResizeInitial=true;
 // Ui::ZHelp   *ui;
};

} // namespace zbs

#endif // ZHelp_H
