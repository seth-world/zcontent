#ifndef ZTOKENTYPEDLG_H
#define ZTOKENTYPEDLG_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <qcheckbox.h>
#include <zqt/zqtwidget/zqtreeview.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

class QMouseEvent;
class ZFieldDLg;
struct ZTypeEditStruct;

class ZTokenTypeDLg : public QDialog//: public ZQTreeView
{
  Q_OBJECT

public:

  explicit ZTokenTypeDLg(ZFieldDLg *pParent=nullptr) ;

  void setupTableView( bool pColumnAutoAdjust, int pColumns);
  void dataSetup();

  void KeyFiltered(int pKey,QKeyEvent* pEvent);
  void MouseFiltered(int pMouseFlag,QMouseEvent * event);

  void set(ZTypeBase pZType) ;
  ZTypeBase get() ;

  QModelIndex searchZType(ZTypeBase pZType);
  QModelIndex _searchZType(QStandardItem* pItem, ZTypeBase pZType);


  void AcceptCurrent();

protected:
  void closeEvent(QCloseEvent *event) override ;
  void resizeEvent(QResizeEvent *) override ;

private slots:

  void AcceptClicked();
  void RejectClicked();

private:
  ZTypeBase  ZType=ZType_Unknown;

  QCheckBox*    ArrayCHk=nullptr;
  QPushButton*  AcceptBTn=nullptr;
  QPushButton*  RejectBTn=nullptr;
  ZQTreeView*   TreeView=nullptr;
  bool        _FResizeInitial=true;
  int         TreeHeightResizeMargin=0;
};


class ZTypeListButton : public  QPushButton
{
  Q_OBJECT
public:
  ZTypeListButton(QWidget* pParent):QPushButton(pParent) {}

  ZTypeBase ZType=ZType_Unknown;
};

/* allows to store a value within the button */
template <class _Tp>
class ZListButton : public  QPushButton
{
  Q_OBJECT
public:
  ZListButton(QWidget* pParent):QPushButton(pParent) {}

  _Tp ZType ;
};


QList<QStandardItem*> createItemRow (ZTypeEditStruct &pTypeStr);

#endif // ZTOKENTYPEDLG_H
