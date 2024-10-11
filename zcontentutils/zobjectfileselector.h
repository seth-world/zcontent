#ifndef ZOBJECTFILESELECTOR_H
#define ZOBJECTFILESELECTOR_H

#include <QMainWindow>
#include <ztoolset/utfvaryingstring.h>

#include <zcontent/zcontentcommon/zdomainobject.h>

#include <zio/zdir.h>
#include <zqt/zqtwidget/zfakebtn.h>


using namespace zbs;

class ZQTableView;
class QHeaderView;

class ZQStatusBar;
namespace zbs {
class ZDomainPath;
}
class QLineEdit;
class QStandardItem;
class QLabel;
class QFrame;
class QCheckBox;

class ZaiErrors;

class ZObjectFileSelector : public QMainWindow
{
    Q_OBJECT
public:
    explicit ZObjectFileSelector(ZDomain_type pObjectFilter,ZaiErrors* pErrorLog , QWidget *pFather );

    ~ZObjectFileSelector()
    {
        removeAllDirEntryRow();
    }

    void initLayout();

    bool ObjectTRvKeyFiltered(int pKey,QKeyEvent* pEvent);
    bool ObjectTRvMouseFiltered(int pKey,QMouseEvent* pEvent);

    void ObjectTRvFlexMenu(QContextMenuEvent *event);


    QMenu*  setupDomainFlexMenu();

    bool setup(ZDomainPath* pFather);
    void setCurrent(DirMap *pDM);

    void removeDirEntryRow(int pRow);
    void removeAllDirEntryRow();

    QList<QStandardItem*> createDirEntryRow(DirMap& pDirEntry);

    bool ObjectListMouseCallBack(int pKey,QMouseEvent* pEvent);

    ZFileSelection  FileSelection;

    ZFakeBTn        ViewZFb;

    ZDomainPath*    FatherDP=nullptr;

    DirMap          SelectedDirEntry;
    ZDirFileEn_type DirEntrySelection = ZDFT_All;

    ZDomain_type    ObjectFilter;

//    ZFileSelection  ObjectSelection;
/*
    utf8VaryingString ObjectRootSelection;
    ZDir::DENM_type   RootSelFlag;
    utf8VaryingString ObjectExtSelection;
    ZDir::DENM_type   ExtSelFlag;
*/
    QLabel* CurrentPathLBl=nullptr;
    QLineEdit* SelectedLEd=nullptr;
    QLabel* DomainChainLBl=nullptr;
    QLabel* LocalRemoteLBl=nullptr;
    QLabel* ObjectFilterLBl=nullptr;
    QLabel* FileSelectionLBl=nullptr;

    ZQTableView*    ObjectListTBv=nullptr;
    QHeaderView*    ObjectListHeader=nullptr;


    QPoint              GlobalMousePosition;

    QMenuBar*           menuBar=nullptr;
    QAction*            QuitQAc=nullptr;
    QMenu*              DomainFlexMEn=nullptr;
    QActionGroup*       GeneralActionGroupQAg=nullptr;

    QActionGroup*       ActionGroupQAg=nullptr;

    ZQStatusBar*        StatusBar=nullptr;

    QCheckBox*          CustomSelectionCBx=nullptr;
    QFrame*             SelectionFRm=nullptr;

    QLineEdit*          CustomSelLEd=nullptr;


    const int ColumnView=5;

signals:
    void DomainsHaveChanged(ZDomainPath* pDP);

private slots:
    void DomainActionEvent(QAction* pAction);

    void Quit();
    void Validation();

    void ObjectTRvDoubleClicked(const QModelIndex &pIndex);
    void ObjectTRvClicked(const QModelIndex &pIndex);

    void HeaderClicked(int pColumn);

    void CustomSelectionChanged(Qt::CheckState pState);
    void ApplyCustomSelection();
private:
    ZaiErrors* ErrorLog=nullptr;
};
/*
namespace zbs {
ZDomain_type deducePossibleType(const uriString& pFileName);
}
*/
#endif // ZOBJECTFILESELECTOR_H
