#ifndef ZSEARCHQUERYMWD_H
#define ZSEARCHQUERYMWD_H


/*
namespace zbs {
    typedef int ZCFMT_Type;
}
*/

#include <memory>
#include <QMainWindow>

#include <ztoolset/utfvaryingstring.h>

#include <zcontent/zindexedfile/zsearchentity.h>
#include <QPalette>

//using namespace zbs;

class QWidget;
class QTextEdit;

class ZQPlainTextEdit;
class ZQTableView;
class QHBoxLayout;
class QSpacerItem;
class QPushButton;
class QWidget;
class QACtion;
class QLabel;
class ZQLabel;
class QStandardItem;

class QProgressBar;


class textEditMWn;
namespace zbs {
  class ZSearchParser;
  class ZHelp;
}


class ZSearchQueryMWd : public QMainWindow
{
  Q_OBJECT

public:
  explicit ZSearchQueryMWd(QWidget *parent = nullptr);
  ~ZSearchQueryMWd();

  ZQLabel *createZButton(const utf8VaryingString &pIconName,
                         const utf8VaryingString &pToolTip,
                         int wSize,
                         QWidget *pParent);

  ZQLabel* createBiStateZButton(const utf8VaryingString& pIconEnabled,
                                const utf8VaryingString& pIconDisabled,
                                const utf8VaryingString& pToolTip,
                                int wSize,
                                QWidget* pParent);

  void initLayout();


  void help();
  void helpClose(QEvent *);

  void insertHighlightedText(const utf8VaryingString& pText);
  void recallForward() ;
  void recallBackward() ;

  void normalKeyEnter();

  void resizeEvent(QResizeEvent* pEvent) override;
  bool _FResizeInitial=true;

  zbs::ZSearchParser* Parser=nullptr;

  void displayErrorCallBack(const utf8VaryingString& pMessage) ;
  void displayErrorColorCB(uint8_t pSeverity, const utf8VaryingString& pMessage);

  bool KeyFiltered(int pKey, QKeyEvent *pEvent);
  void KeyUnFiltered( QKeyEvent *pEvent);

  void SaveInstructions();

  void setProgressBarMax(int pMax, const utf8VaryingString &pStep);
  void setProgressBarValue(int pValue, const utf8VaryingString &pStep);

  ZStatus DisplayEntityRaw(std::shared_ptr<ZSearchEntity> &pEntity);
  ZStatus DisplayEntity(std::shared_ptr<ZSearchEntity> pEntity, int pInstructionType, int pNumber);

  void DisplayCurrentEntity ();


  QList<QStandardItem *> DisplayOneRawLine(ZDataBuffer &pRecord);

  QStandardItem *DisplayOneURFFieldFromSurface(const unsigned char *&pPtr,
                                               const unsigned char *wPtrEnd,
                                               zbs::ZCFMT_Type pCellFormat);
  QList<QStandardItem *> DisplayOneLine(ZArray<URFField> &pFields);

  //  QStandardItem * DisplayOneURFField(URFField& pURFField, int pCellFormat);

  void ClearLog();

  private slots:

//  void QuitBTnClicked(bool pChecked);
//  void ClearBTnClicked(bool pChecked);
//  void ListBTnClicked(bool pChecked);
  void ParseAndStoreQuery();

  void MenuTriggered(QAction* pAction);

  void TableHeaderClicked(int pLogicalIndex);

  void TableHeaderCornerClicked();

  void BackwardClicked();
  void ForwardClicked();
  void ExecuteClicked();
  void QuitLBlClicked();
  void TextClearLBlClicked();
  void EntityListLBlClicked();



private:
  zbs::ZHelp*  HelpMWn=nullptr;
  QWidget     *CentralWidget=nullptr;
  QWidget*     QueryWDg = nullptr;
  QWidget*     ExpWidget2 = nullptr;
  QWidget*     ForBackWDg = nullptr;

  ZQPlainTextEdit   *QueryQTe=nullptr;
  ZQTableView *ResultTBv=nullptr;

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

  QMenuBar    *menubar;
  QStatusBar  *statusbar;

  QAction* ExecQAc=nullptr;
  QAction* QuitQAc=nullptr;
  QAction* QuitIconQAc=nullptr;

  QAction* SaveInstructionsQAc=nullptr;

  QAction* ClearLogQAc=nullptr;

  QAction*  OptReportQAc=nullptr;
  QAction*  OptNoVerboseQAc=nullptr;
  QAction*  OptVerboseQAc=nullptr;
  QAction*  OptFullVerboseQAc=nullptr;

  textEditMWn* displayMWn=nullptr;

  std::shared_ptr<ZSearchEntity> CurrentEntity=nullptr;

  QPalette LabelDefaultPalette;
  QPalette LabelBadPalette;
  QPalette LabelGoodPalette;

  int IndexRecall             = -1;
  int LastHighlightPosition   = -1;
  int FirstInsertedPosition    = -1;
  int LastInsertedPosition    = -1;
};

#endif // ZSEARCHQUERYMWD_H
