#ifndef TEXTEDITMWN_H
#define TEXTEDITMWN_H

#include <QMainWindow>
#include <ztoolset/zstatus.h>

class uriString;

namespace Ui {
class textEditMWn;
}

class textEditMWn : public QMainWindow
{
  Q_OBJECT

public:
  explicit textEditMWn(QWidget *parent = nullptr);
  ~textEditMWn();

  ZStatus setTextFromFile(const uriString& pTextFile);

Q_SIGNALS :
      void resizeEvent(QResizeEvent*);

private Q_SLOTS :
  void resizeWindow(QResizeEvent* pEvent);

private:


  Ui::textEditMWn *ui;
};

#endif // TEXTEDITMWN_H
