#ifndef TEXTEDITMWN_H
#define TEXTEDITMWN_H
#include <iostream>
#include <QMainWindow>
#include <ztoolset/zstatus.h>
#include <functional>

#include <QTextCursor>


extern const int cst_MessageDuration ;

#define __CLOSE_CALLBACK__(__NAME__)  std::function<void (QEvent*)> __NAME__
#define __MORE_CALLBACK__(__NAME__)  std::function<void ()> __NAME__
#define __FILTRATE_CALLBACK__(__NAME__)  std::function<bool (const utf8VaryingString&)> __NAME__


class uriString;
class utf8VaryingString;
class ZQPlainTextEdit;
class QTextCursor;
class QTextCharFormat;

namespace Ui {
class textEditMWn;
}

enum ZTextEditOption : uint32_t {
  TEOP_Nothing        = 0,
  TEOP_CloseBtnHide   = 0x01,     /* if set Close button hides dialog, while if not set close button closes */
  TEOP_ShowLineNumbers= 0x04
};

class ZContentVisuMain;
class ZLineNumberArea;
class QTextDocument;

class textEditMWn : public QMainWindow
{
  Q_OBJECT

  void _init(uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack) );
public:
//  explicit textEditMWn(ZContentVisuMain*parent,__CLOSE_CALLBACK__(pCloseCallBack) );
  explicit textEditMWn(QWidget*parent ,uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack));
  explicit textEditMWn(QWidget*parent, uint32_t pOptions );
  explicit textEditMWn(QWidget*parent );
  ~textEditMWn();

  ZStatus setTextFromFile(const uriString& pTextFile);
  void setText(const utf8VaryingString& pText, const utf8VaryingString &pTitle);
  void appendText(const utf8VaryingString& pText);

  void setPositionOrigin ();

  void filtrate() ;

  void setFileClosed(bool pYesNo);

  void useLineNumbers(bool pOnOff) ;

  void highlightWordAtOffset(int pOffset);
  void highlightLine(int pLine);
  void resetLine(int pLine);
  void resetWordAtOffset(int pOffset);

  void clear() ;

  void registerCloseCallback(__CLOSE_CALLBACK__(pCloseCallBack)) {CloseCallBack=pCloseCallBack;}
  void registerMoreCallback(__MORE_CALLBACK__(pCloseCallBack)) {MoreCallBack=pCloseCallBack;}
  void registerFiltrateCallback(__FILTRATE_CALLBACK__(pFiltrateCallBack)) {FiltrateCallBack=pFiltrateCallBack;}

  void setFiltrateActive(bool pOnOff) ;


  void setCloseButtonRole () ;

  void resizeEvent(QResizeEvent*) override;
  void closeEvent(QCloseEvent *event) override;
  void keyPressEvent(QKeyEvent *pEvent) override;

private Q_SLOTS :
  void morePressed();
  void closePressed();
  void wrapPressed();
  void filterPressed();
  void lineNumbersOnOff();
  void searchMainPressed();
  void searchPressed();
  void searchReturnPressed();

private:
  void search(const utf8VaryingString &pSearchString);
  void searchFirst(const utf8VaryingString &pSearchString);
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
  ZQPlainTextEdit*  Text=nullptr;
  bool              FResizeInitial=true;
  Ui::textEditMWn   *ui;
};

#endif // TEXTEDITMWN_H
