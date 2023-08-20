#ifndef ZSEARCHQUERYMWD_H
#define ZSEARCHQUERYMWD_H

#include <QMainWindow>

#include <ztoolset/utfvaryingstring.h>
/*
namespace Ui {
class ZSearchQueryMWd;
}
*/
class QWidget;
class QTextEdit;

class ZQPlainTextEdit;
class ZQTableView;
class QHBoxLayout;
class QSpacerItem;
class QPushButton;
class QWidget;
class QACtion;

class QProgressBar;


class textEditMWn;
namespace zbs {
  class ZSearchParser;
}




class ZSearchQueryMWd : public QMainWindow
{
  Q_OBJECT

public:
  explicit ZSearchQueryMWd(QWidget *parent = nullptr);
  ~ZSearchQueryMWd();

  void setup();

  void insertHighlightedText(const utf8VaryingString& pText);
  void recallForward() ;
  void recallBackward() ;

  void normalKeyEnter();

  void resizeEvent(QResizeEvent* pEvent) override;
  bool _FResizeInitial=true;

  zbs::ZSearchParser* Parser=nullptr;

  void displayErrorCallBack(const utf8VaryingString& pMessage) ;

  bool KeyFiltered(int pKey, QKeyEvent *pEvent);
  void KeyUnFiltered( QKeyEvent *pEvent);

  void SaveInstructions();


  void setProgressBarMax(int pMax);
  void setProgressBarValue(int pValue);

private slots:
  void QuitBTnClicked(bool pChecked);
  void ExecuteBTnClicked(bool pChecked);
  void ClearBTnClicked(bool pChecked);

  void MenuTriggered(QAction* pAction);
private:
  QWidget     *centralwidget=nullptr;
  ZQPlainTextEdit   *QueryQTe=nullptr;
  ZQTableView *ResultTBv=nullptr;
  QWidget     *horizontalLayoutWidget=nullptr;
  QWidget*    ExpWidget1=nullptr;
  QWidget*    ExpWidget2=nullptr;


  QPushButton *ExecuteBTn;
  QPushButton *QuitBTn;
  QPushButton *ClearBTn;

  QMenuBar    *menubar;
  QStatusBar  *statusbar;

  QAction* ExecQAc=nullptr;
  QAction* QuitQAc=nullptr;

  QAction* SaveInstructionsQAc=nullptr;

  QAction*  OptReportQAc=nullptr;
  QAction*  OptNoVerboseQAc=nullptr;
  QAction*  OptVerboseQAc=nullptr;
  QAction*  OptFullVerboseQAc=nullptr;


  QProgressBar* ProgressBar=nullptr;

  textEditMWn* displayMWn=nullptr;
  int IndexRecall             = -1;
  int LastHighlightPosition   = -1;
  int FirstInsertedPosition    = -1;
  int LastInsertedPosition    = -1;
};

#endif // ZSEARCHQUERYMWD_H
