#ifndef ZQUERYWIZARDMWD_H
#define ZQUERYWIZARDMWD_H


/*
namespace zbs {
    typedef int ZCFMT_Type;
}
*/

#include <QPalette>

#include <memory>
#include <QMainWindow>

#include <ztoolset/utfvaryingstring.h>

#include <zcontent/zindexedfile/zsearchentity.h>
#include <zcontent/zindexedfile/zsearchcontext.h>
#include <QPalette>


#include <ztoolset/uristring.h>

namespace zbs {

class ZDomainPath;
}// namespace zbs

class ZQStatusBar;

using namespace zbs;

class ZSearchQueryMWd;


class QWidget;
class QTextEdit;

class ZQPlainTextEdit;
class ZQTableView;
class QHBoxLayout;
class QSpacerItem;
class QPushButton;
class QWidget;
class QAction;
class QLabel;
class ZQLabel;
class QStandardItem;
class QCheckBox;
class QMenuBar;
class QMenu;
class QStatusBar;
class QGroupBox;
class QMenu;

class QProgressBar;

class ZSearchQueryMWd;

class textEditMWn;


class ZComboDelegate;

namespace zbs {
  class ZSearchParser;
  class ZHelp;
}


class ZQueryWizardMWd : public QMainWindow
{
  Q_OBJECT

public:
//  explicit ZQueryWizardMWd(ZSearchQueryMWd *parent = nullptr);
  explicit ZQueryWizardMWd(ZaiErrors* pErrorLog, QWidget *parent);
  ~ZQueryWizardMWd();

  void setQueryMain(ZSearchQueryMWd* pQueryMain) ;

  void initLayout();

  void setupEntitiesFromQuery();

  void insertHighlightedText(const utf8VaryingString& pText);
/*  void recallForward() ;
  void recallBackward() ;

  void normalKeyEnter();
*/
  void resizeEvent(QResizeEvent* pEvent) override;
  bool _FResizeInitial=true;

//  zbs::ZSearchParser* Parser=nullptr;

//  bool KeyFiltered(int pKey, QKeyEvent *pEvent);
//  void KeyUnFiltered( QKeyEvent *pEvent);

  void SaveInstructions();

  void setProgressBarMax(int pMax, const utf8VaryingString &pStep);
  void setProgressBarValue(int pValue, const utf8VaryingString &pStep);


  void CreateFileEntityClicked();

  void statusBarMessage(const utf8VaryingString& pMsg);

private slots:

  void MenuTriggered(QAction* pAction);

  void ExecuteClicked();
  void QueryHasBeenExecuted(ZSearchContext* pContext);

  void QuitClicked();
  void SaveClicked();

  void CreateCollectionClicked();

/* return from ZDomainObjectFinder signal */
  void CreateFileEntity(ZDomainPath* pDP);


 // void EntityListLBlClicked();



private:

  bool hasBeenSaved = true;

  ZaiErrors*    ErrorLog=nullptr;
  zbs::ZHelp*   HelpMWn=nullptr;
  QWidget*      CentralWidget=nullptr;
  QWidget*     QueryWDg = nullptr;
  QWidget*     ExpWidget2 = nullptr;
  QWidget*     ForBackWDg = nullptr;

  ZQPlainTextEdit*  QueryQTe=nullptr;
//  ZQTableView*      ResultTBv=nullptr;


  ZSearchQueryMWd*  QueryMain=nullptr;

  QLabel*     EntityNameLBl=nullptr;
  QLabel*     EntityTypeLBl=nullptr;
  QLabel*     RecordCountLBl=nullptr;
  QLabel*     LastStatusLBl=nullptr;

  ZQLabel*    ForwardLBl=nullptr;
  ZQLabel*    BackwardLBl=nullptr;
  ZQLabel*    ExecuteLBl=nullptr;
  ZQLabel*    EraseLBl=nullptr;

  ZQLabel*    EntityListLBl=nullptr;
  ZQLabel*    TextClearLBl=nullptr;
  ZQLabel*    GlobalCellFmtLBl=nullptr;

  int LBlSize = 25;
  int HeightMargin = 2;
  int WidthMargin = 6 ;

  QMenuBar    *menuBar=nullptr;
public:  ZQStatusBar *statusBar=nullptr;
private:
  QAction* CreateSymbolQAc=nullptr;
  QAction* CreateFileEntityQAc=nullptr;
  QAction* CreateCollectionEntityQAc=nullptr;
  QMenu*   ContentMEn=nullptr;
  QAction* RestrictQAc=nullptr;

  QAction* FieldSelectionQAc=nullptr;

  QAction* JoinQAc=nullptr;
  QAction* UsingClauseQAc=nullptr;

  QAction* ArithmeticQAc=nullptr;

  QAction* SubmitQAc=nullptr;

  QAction* QuitQAc=nullptr;
  QAction* QuitIconQAc=nullptr;

  QAction* SaveQAc=nullptr;


  QAction*  OptReportQAc=nullptr;
  QAction*  OptNoVerboseQAc=nullptr;
  QAction*  OptVerboseQAc=nullptr;
  QAction*  OptFullVerboseQAc=nullptr;

  QLabel*   CurrentEntityLBl=nullptr;


  QGroupBox*   EntityListGPb=nullptr;
  ZQTableView* EntityListTBv=nullptr;

  QGroupBox*   RestrictGPb=nullptr;
  QCheckBox*   AllRanksCHk=nullptr;
  ZQTableView* RestrictTBv=nullptr;

  QGroupBox*   FieldsSelectionGPb=nullptr;
  QCheckBox*   AllFieldsCHk=nullptr;
  ZQTableView* FieldsSelectionTBv=nullptr;


  QGroupBox*   JoinGPb=nullptr;
  QCheckBox*   CartesianProductCHk=nullptr;
  ZQTableView* JoinTBv=nullptr;

  QWidget*     ArithmeticWDg=nullptr;
  ZQTableView* ArithmeticTBv=nullptr;

  //QPushButton* OKBTn=nullptr;
  //QPushButton* QuitBTn=nullptr;

  std::shared_ptr<ZSearchEntity> CurrentEntity=nullptr;

  QPalette LabelDefaultPalette;
  QPalette LabelBadPalette;
  QPalette LabelGoodPalette;

};



#endif // ZQUERYWIZARDMWD_H
