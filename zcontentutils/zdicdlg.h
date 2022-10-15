#ifndef ZDICDLG_H
#define ZDICDLG_H

#include <QObject>
#include <QDialog>
#include <qcheckbox.h>
#include <zqt/zqtwidget/zqtableview.h>
#include <zqt/zqtwidget/zqtwidgettools.h>
#include <ztoolset/ztypetype.h>
#include <zindexedfile/zdictionaryfile.h>

//class QMouseEvent;
class ZFieldDLg;
struct ZTypeEditStruct;
class ZTypeListButton;

class QPushButton;

enum ZDicEvent_type : uint8_t {
  ZDET_Nothing  = 0,
  ZDET_Deleted  = 1,
  ZDET_Change   = 2 ,
  ZDET_Created  = 4
};
class ZDicEvent {
public:
  ZDicEvent ()=default;
  ZDicEvent (const ZMFDictionary*  pDicBefore,const ZMFDictionary*  pDicAfter,long pRank,ZDicEvent_type pType) {
    Rank=pRank;
    Type=pType;
    if (pDicBefore)
      DataBefore = new ZMFDictionary(*pDicBefore);
    if (pDicAfter)
      DataAfter = new ZMFDictionary(*pDicAfter);
  }
  ZDicEvent (const ZDicEvent & pIn){_copyFrom(pIn);}

  ZDicEvent & _copyFrom(const ZDicEvent & pIn) {
    Type=pIn.Type;
    Rank=pIn.Rank;
    if (pIn.DataBefore)
      DataBefore = new ZMFDictionary(*pIn.DataBefore);
    else
      DataBefore=nullptr;
    if (pIn.DataAfter)
      DataAfter = new ZMFDictionary(*pIn.DataAfter);
    else
      DataAfter=nullptr;
    return *this;
  }
  ~ZDicEvent() {
    if (DataBefore)
      delete DataBefore;
    if (DataAfter)
      delete DataAfter;
  }


  ZDicEvent & operator = (const ZDicEvent & pIn) {return _copyFrom(pIn);}

  void setBefore(const ZMFDictionary*  pBefore) {DataBefore = new ZMFDictionary(*pBefore);}
  void setAfter(const ZMFDictionary*  pAfter) {DataAfter = new ZMFDictionary(*pAfter);}

  ZDicEvent_type  Type=ZDET_Nothing;
  long            Rank=0;
  ZMFDictionary*  DataBefore=nullptr;
  ZMFDictionary*  DataAfter=nullptr;
};

/**
 * @brief The ZDicDLg class manage list of dictionaries contained in a selected dictionary file
 */

class ZDicDLg : public QDialog//: public ZQTreeView
{
  Q_OBJECT

public:

  explicit ZDicDLg(QWidget *pParent=nullptr) ;
  explicit ZDicDLg(const uriString& pDicFile, QWidget *pParent=nullptr) ;

  ~ZDicDLg() override ;
  /** loads dictionaries from pDicFile. Return ZS_SUCCESS when done */
  ZStatus setDicFile(const uriString& pDicFile);

  void init();

  void dataSetup();

  void KeyFiltered(int pKey,QKeyEvent* pEvent);
  void MouseFiltered(int pMouseFlag,QMouseEvent * event);

  void deleteDic(int pRow);

  void set(const utf8VaryingString pDicName, unsigned long pVersion) ;
  int getRank() ;
  uriString getDicFileURI() ;

  /**  @brief getDicActive create and exec a QDialog to manage a simple dictionary status (active or not) at pRank.
   *                       DicList is modified accordingly as well as TableView screen display
  */
  bool getDicActive(long pRank);
  
  ZStatus update();
  void replace();

protected:

  void closeEvent(QCloseEvent *event) override ;
  void resizeEvent(QResizeEvent *) override ;

private slots:
  void AcceptClicked();
  void RejectClicked();

private:
  ZArray<ZDicEvent> DicEvents;
  bool              DataChanged=false;
  ZDictionaryFile*  DicFile=nullptr;
  ZDicList          DicList;
  ZTypeBase  ZType=ZType_Unknown;

  QPushButton*  AcceptBTn=nullptr;
  QPushButton*  RejectBTn=nullptr;
  ZQTableView*   TableView=nullptr;
  bool        _FResizeInitial=true;
  int         TreeHeightResizeMargin=0;
};

#endif // ZDICDLG_H
