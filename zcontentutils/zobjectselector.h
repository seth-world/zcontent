#ifndef ZOBJECTSELECTOR_H
#define ZOBJECTSELECTOR_H

#include <QMainWindow>
#include <ztoolset/utfvaryingstring.h>

#include <zcontent/zcontentcommon/zdomainobject.h>

#include <zio/zdir.h>
#include <zqt/zqtwidget/zfakebtn.h>

using namespace zbs;

class ZQTableView;
class ZQStatusBar;
namespace zbs {
class ZDomainPath;
}
class QLineEdit;
class QStandardItem;
class QLabel;

class ZaiErrors;

class ZObjectSelector : public QMainWindow
{
    Q_OBJECT
public:
    explicit ZObjectSelector(ZDomain_type pObjectFilter,ZaiErrors* pErrorLog , QWidget *pFather );

    ~ZObjectSelector()
    {
        removeAllDirEntryRow();
    }

    void initLayout();

    bool ObjectTRvKeyFiltered(int pKey,QKeyEvent* pEvent);
    bool ObjectTRvMouseFiltered(int pKey,QMouseEvent* pEvent);

    void ObjectTRvFlexMenu(QContextMenuEvent *event);


    QMenu*  setupDomainFlexMenu();

    bool setup(ZDomainPath* pDP);
    void setCurrent(DirMap *pDM);

    void removeDirEntryRow(int pRow);
    void removeAllDirEntryRow();

    QList<QStandardItem*> createDirEntryRow(DirMap& pDirEntry);


    ZFileSelection  FileSelection;

    ZFakeBTn        ViewZFb;
    ZDomainPath*    CurrentDP=nullptr;
    DirMap          SelectedDirEntry;
    ZDirFileEn_type DirEntrySelection = ZDFT_All;

    ZDomain_type    ObjectFilter;

    utf8VaryingString ObjectRootSelection;
    ZDir::DENM_type   RootSelFlag;
    utf8VaryingString ObjectExtSelection;
    ZDir::DENM_type   ExtSelFlag;

    QLabel* CurrentPathLBl=nullptr;
    QLineEdit* SelectedLEd=nullptr;
    QLabel* DomainChainLBl=nullptr;
    QLabel* LocalRemoteLBl=nullptr;
    QLabel* ObjectFilterLBl=nullptr;

    ZQTableView* ObjectListTBv=nullptr;


    QPoint              GlobalMousePosition;

    QMenuBar*           menuBar=nullptr;
    QAction*            QuitQAc=nullptr;
    QMenu*              DomainFlexMEn=nullptr;
    QActionGroup*       GeneralActionGroupQAg=nullptr;

    QActionGroup*       ActionGroupQAg=nullptr;

    ZQStatusBar*        StatusBar=nullptr;

    const int           ColumnView=5;

signals:
    void DomainsHaveChanged(ZDomainPath* pDP);

private slots:
    void DomainActionEvent(QAction* pAction);

    void Quit();
    void Validation();

    void ObjectTRvDoubleClicked(const QModelIndex &pIndex);
    void ObjectTRvClicked(const QModelIndex &pIndex);

    void HeaderClicked(int pColumn);
private:
    ZaiErrors* ErrorLog=nullptr;
};
/*
namespace zbs {
ZDomain_type deducePossibleType(const uriString& pFileName);
}
*/
#endif // ZOBJECTSELECTOR_H
