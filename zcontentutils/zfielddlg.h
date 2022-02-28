#ifndef ZFIELDDLG_H
#define ZFIELDDLG_H

#include <QDialog>

#include <qwindow.h>
#include <zqt/zqtwidget/zqtreeview.h>

#include <zindexedfile/zfielddescription.h>

namespace Ui {
class ZFieldDLg;
}

class ZFieldDLg;

class ZTypeClass : public ZQTreeView
{
Q_OBJECT
public:

  ZTypeClass(ZFieldDLg *pParent, QDialog *pDialog) ;

  void setupTableView( bool pColumnAutoAdjust, int pColumns);
  void dataSetup();

  void KeyFiltered(int pKey,int pMouseFlag);

  void set(ZTypeBase pZType) ;
  ZTypeBase get() {return ZType;}

protected:
  void closeEvent(QCloseEvent *event) ;


private:
  ZTypeBase  ZType=ZType_Unknown;

  ZFieldDLg* Parent=nullptr;
  QDialog* FatherDLg=nullptr;
};

class ZFieldDLg : public QDialog
{
  Q_OBJECT

public:
  explicit ZFieldDLg(QWidget *parent = nullptr);
  ~ZFieldDLg();

  /* updates dialog content with pField */
  void setup(ZFieldDescription *pField);
  /* updates Field from dialog content */
  void refresh();



  ZFieldDescription *Field=nullptr;
  QDialog* ZTypeDLg=nullptr;
  ZTypeClass* ZTypeTRv=nullptr;

public slots:
//  void ZTypeFocusIn(QFocusEvent* pEvent);
  void ListClicked();
  void AcceptClicked();
  void DisguardClicked();

  int exec() override;


private:

  Ui::ZFieldDLg *ui;
};

template <class _Tp>
bool getValueFromItem(QModelIndex& pIdx,_Tp &pRet)
{
  if (!pIdx.isValid())
    return false;
  QVariant wV=pIdx.data(ZQtValueRole);
  if (!wV.isValid())
    return false;
  pRet= wV.value<_Tp>();
  return true;
}

#endif // ZFIELDDLG_H
