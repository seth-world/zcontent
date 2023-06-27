#ifndef ZSTDLISTDLG_H
#define ZSTDLISTDLG_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <qcheckbox.h>
#include <zqt/zqtwidget/zqtableview.h>
#include <zqt/zqtwidget/zqtwidgettools.h>
#include <QStandardItemModel>

class QMouseEvent;
class ZFieldDLg;
struct ZTypeEditStruct;

class QActionGroup;
class QAction;
class QMenu;


class ZStdListDLgLine : public ZArray<const utf8VaryingString*> {
public:
  ZStdListDLgLine() = default;
  ~ZStdListDLgLine() {
    while (count())
      delete popR();
  }

  ZStdListDLgLine(const ZStdListDLgLine& pIn) {
    _copyFrom(pIn) ;
  }
  ZStdListDLgLine& _copyFrom(const ZStdListDLgLine& pIn) {
    while (count())
      delete popR();
    for (long wi=0;wi < pIn.count();wi++)
      add(new utf8VaryingString(*pIn[wi]));
    return *this;
  }

  ZStdListDLgLine& operator = (const ZStdListDLgLine& pIn) {
    return _copyFrom(pIn);
  }
  void add (const utf8VaryingString& pAdd) {
    push(new utf8VaryingString(pAdd));
  }
  void add (const utf8VaryingString* pAdd) {
    push(new utf8VaryingString(*pAdd));
  }
};

class ZRowCol {
public:
  ZRowCol()=default;
  ZRowCol(const ZRowCol& pIn) {_copyFrom(pIn);}
  ZRowCol(const QModelIndex& pMI) {if (!set(pMI)) abort();}


  ZRowCol& _copyFrom(const ZRowCol& pIn) {Row=pIn.Row; Col=pIn.Col; return *this;}

  ZRowCol& operator = (const ZRowCol& pIn) {return _copyFrom(pIn);}

  bool set(const QModelIndex& pMI) {
    if (pMI.isValid()) {
      Row=pMI.row();
      Col=pMI.column();
      return true;
      }
      return false;
  }// set

  int row() {return Row;}
  int col() {return Col;}

  int Row=0;
  int Col=0;
};

class ZStdListDLgContent : public ZArray<ZStdListDLgLine> {
public:
  ZStdListDLgContent() = default;
  ~ZStdListDLgContent() {}
};

typedef ZStdListDLgLine ZStdListHeaders;

class ZStdListDLg : public QDialog//: public ZQTreeView
{
  Q_OBJECT
public:

  explicit ZStdListDLg(QWidget *pParent=nullptr) ;


  void tableViewSetup( bool pColumnAutoAdjust, int pColumns);
  void dataSetup(const ZStdListDLgLine& pTitle,const ZStdListDLgContent& pContent);

  void KeyFiltered(int pKey,QKeyEvent* pEvent);
  void MouseFiltered(int pMouseFlag,QMouseEvent * event);

  void insertRow(const ZStdListDLgLine &pContent, int pRow);
  void appendRow(const ZStdListDLgLine& pContent);
  void deleteRow(int pRow);

  void set(int pRow,int pCol=0) {
    QModelIndex wI = TableView->ItemModel->index(pRow,pCol);
    TableView->setCurrentIndex(wI);
    return;
  } // set

  void set(const utf8VaryingString& pCell, int pCol) {
    for (long wi=0;wi < Content->count();wi++) {
      if (*Content->TabConst(wi).TabConst(pCol)== pCell.toCChar() ) {
        QModelIndex wI = TableView->ItemModel->index(wi,pCol);
        TableView->setCurrentIndex(wI);
        return;
      }
    }// for
  } //set

  ZRowCol get() ;

  QModelIndex searchZType(ZTypeBase pZType);
  QModelIndex _searchZType(QStandardItem* pItem, ZTypeBase pZType);


  virtual void AcceptCurrent(int pRow) ;

  virtual void ChangeCurrent(int pRow) ;

  virtual void InsertCurrent(int pRow) ;

  virtual void DeleteCurrent(int pRow) ;

  virtual void AppendCurrent() ;

  /** @brief setupContextMenu  This virtual method may be overriden to set up different menu item text */

  virtual void setupContextMenu();

  void AppendLine(const ZStdListDLgLine& pLine) ;

  void InsertLine(int pRow,const ZStdListDLgLine &pLine ) ;

  void DeleteLine(int pRow ) ;

  void ReplaceLine(int pRow , const ZStdListDLgLine &pLine) ;

  void ContextMenu(QContextMenuEvent *event);

  QMenu* popupMEn=nullptr;
  QActionGroup* popupGRp=nullptr;
  QAction *insertQAc=nullptr;
  QAction *deleteQAc=nullptr;
  QAction *appendQAc=nullptr;
  QAction *changeQAc=nullptr;

protected:
  void closeEvent(QCloseEvent *event) override ;
  void resizeEvent(QResizeEvent *) override ;

private slots:

  void AcceptClicked();
  void RejectClicked();
  void generalActionEvent(QAction* pAction);
private:
  ZTypeBase  ZType=ZType_Unknown;

  const ZStdListDLgLine *Title=nullptr;
  const ZStdListDLgContent *Content=nullptr;

//  QCheckBox*    ArrayCHk=nullptr;
  QPushButton*  AcceptBTn=nullptr;
  QPushButton*  RejectBTn=nullptr;
  ZQTableView*  TableView=nullptr;
  bool        _FResizeInitial=true;
  int         TreeHeightResizeMargin=0;
};


class ZTypeListButton : public  QPushButton
{
public:
  ZTypeListButton(QWidget* pParent):QPushButton(pParent) {}

  ZTypeBase ZType=ZType_Unknown;
};

QList<QStandardItem*> createItemRow (ZTypeEditStruct &pTypeStr);

#endif // ZSTDLISTDLG_H
