#ifndef ZDOMAINOBJECTFINDER_H
#define ZDOMAINOBJECTFINDER_H

#include <QMainWindow>
#include <QAbstractItemView>

#include <zcontent/zcontentcommon/zdomaintype.h>
#include <zcontent/zcontentcommon/zdomainpath.h>
#include <zxml/zxmlprimitives.h>
#include <zcontent/zcontentcommon/zdomainbroker.h>


#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zaierrors.h>

#include <zqt/zqtwidget/zfakebtn.h>

class QComboBox;
class ZQTreeView;
class ZQStatusBar;

namespace zbs {

class ZDomainObjectFinder : public QMainWindow
{
    Q_OBJECT
public:
    explicit ZDomainObjectFinder(ZaiErrors *pErrorLog, QWidget *parent = nullptr);

    void initLayout() ;



    void displayDomains(ZDomainPath *pDomain);
    void appendDomainRow(ZDomainPath* pDomain, QStandardItem* pFirstItemOfRowToChange);


    ZQStatusBar* StatusBar=nullptr;
private:
//    ZQTreeView*   ObjectTRv=nullptr;
    ZQTreeView* ObjectTRv=nullptr;

    QMenuBar*           menuBar=nullptr;
    QAction*            QuitQAc=nullptr;
    QMenu*              DomainFlexMEn=nullptr;
    QActionGroup*       DomainActionGroupQAg=nullptr;

    QActionGroup*       ActionGroupQAg=nullptr;


    QComboBox*          ObjectTypeCBx=nullptr;

    QLineEdit*          CurrentPathLEd=nullptr;
    QLineEdit*          SelectedLEd=nullptr;
    ZDomainPath*        SelectedDP = nullptr;

    const int ColumnMame=0;
    const int ColumnHierarchy=1;
    const int ColumnType=2;
    const int ColumnContent=3;
    const int ColumnToolTip=4;
    const int ColumnView=5;
    const int ColumnEdit=6;

    QPoint      GlobalMousePosition;

    bool ObjectTRvKeyFiltered(int pKey,QKeyEvent* pEvent);
    bool ObjectTRvMouseFiltered(int pKey,QMouseEvent* pEvent);

    void ObjectTRvFlexMenu(QContextMenuEvent *event);

    void    setCurrent (ZDomainPath* pDP);
    QMenu*  setupDomainFlexMenu();

    bool objectSelector (ZDomainPath* wDP, QWidget* pFather, ZaiErrors* pErrorLog);

signals:
    void domainObjectSelected(ZDomainPath* pDP);

public slots:
    void setup(ZDomainPath* pDP);
private slots:

    void Quit();
    void Validation();

    void ObjectTRvExpanded();
    void ObjectTRvDoubleClicked(const QModelIndex &pIndex);
    void ObjectTRvClicked(const QModelIndex &pIndex);

    void ObjectTypeFilterChanged(int pIndex);

    void Backward();

    void MenuTriggered(QAction* pAction);
    void DomainActionEvent(QAction* pAction);

    void DomainTRwSelModeChanged(QAbstractItemView::SelectionMode pSelMode);

    QList<QStandardItem*> createDomainRow(ZDomainPath* pDomain, ZaiErrors *pErrorLog);

signals:

private:
    ZDomainPath*    CurrentDP=nullptr;
    ZDomain_type    ObjectFilter = ZDOM_ObjectMask ;

    QIcon           MainDomainICn;
    QIcon           DomainICn;

    ZFakeBTn        ToolTipZFb;
    ZFakeBTn        ViewZFb;
    ZFakeBTn        NewZFb;
    ZFakeBTn        HierarchyZFb;
public:
    ZaiErrors* ErrorLog=nullptr;
};


}// namespace zbs



#endif // ZDOMAINOBJECTFINDER_H
