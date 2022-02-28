#ifndef TEXTEDITMWN_H
#define TEXTEDITMWN_H

#include <QMainWindow>
#include <ztoolset/zstatus.h>

class uriString;
class utf8VaryingString;

namespace Ui {
class textEditMWn;
}

class ZContentVisuMain;

class textEditMWn : public QMainWindow
{
  Q_OBJECT

public:
  explicit textEditMWn(ZContentVisuMain*parent );
  explicit textEditMWn(QWidget*parent = nullptr);
  ~textEditMWn();

  ZStatus setTextFromFile(const uriString& pTextFile);
  void setText(const utf8VaryingString& pText, const utf8VaryingString &pTitle);
  void appendText(const utf8VaryingString& pText);

  ZContentVisuMain*     VisuMain=nullptr;

  void setFileClosed(bool pYesNo);

  void resizeEvent(QResizeEvent*);

private Q_SLOTS :
  void morePressed();
  void closePressed();
  void wrapPressed();

private:
  bool FWrap=false;
  bool FResizeInitial=true;
  Ui::textEditMWn *ui;
};

#endif // TEXTEDITMWN_H
