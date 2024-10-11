#ifndef ZGENERALPARAMSDLG_H
#define ZGENERALPARAMSDLG_H

#include <QMainWindow>
#include <QPoint>

#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <zcontentcommon/zgeneralparameters.h>


class QFrame;
class QLabel;
class QLineEdit;
class ZQTableView;
class ZQTreeView;
class textEditMWn;
class QStandardItem;
class QCheckBox;
class ZFakeBTn;
class QMenuBar;
class QAction;
class QActionGroup;
class QPushButton;
class QTextEdit;

class ZGeneralParamsWNd : public QMainWindow
{
public:
    explicit ZGeneralParamsWNd(ZaiErrors *pErrorLog, QWidget *pParent);
    ~ZGeneralParamsWNd();

    void initLayout();

    ZStatus setup(ZGeneralParameters& pGeneralParameters, ZDomainBroker& pDomainBroker);
    QStandardItem* fakeBTnsetup();
    QStandardItem* inactiveSetup();

    int getDir(uriString &pDir);
    int getFont(utf8VaryingString &pFont);

    ZVerbose_Base fromScreenVerbose();
    void displayVerbose(ZVerbose_Base pVerbose);

    void displayDomains();
    void displayDomainRow(ZDomainPath* pDomain, QStandardItem* pItem, int pLevel);

    void    DomainTRvKeyFiltered(int pKey,QKeyEvent* pEvent);
    void    DomainTRvMouseFiltered(int pKey,QMouseEvent* pEvent);
    void    DomainTRvFlexMenu(QContextMenuEvent *event);
    QMenu*  setupDomainFlexMenu();

    void    domainRemove(); /* remove current domain and all its inferiors */
    void    domainCreate(); /* create a new domain at the same level of current in place of current */
    void    domainChange(); /* change current selected model content - no change for inferiors */
    void    domainInsert(); /* create a new sub-domain as first inferior to current */
    void    domainPasteUpper(); /* create a new sub-domain as first inferior to current from clipboard */
    void    domainPasteCurrent(); /* create a new sub-domain as first inferior to current from clipboard */
    void    domainCopy();   /* copy current domain to clipboard with all its inferiors without removing it */
    void    domainCut();   /* copy current domain to clipboard  with all its inferiors and remove it from view and from DomainBroker */
    void    domainMoveDown(); /* move current domain up within current father domain */
    void    domainMoveUp();  /* move current domain down within current father domain */

    QList<QStandardItem*> _copyDomainRow(QStandardItem* pFirstItem,ZDomainPath* pNewDP);
    void hierarchyCascadeUpdate(QModelIndex pIdx);
private slots:
    void searchXmlFileClicked ();
    void update();
    void Quit();
    void ParamsTBvChanged (QStandardItem *pItem);
    void ParamsTBvClicked (const QModelIndex &pIndex);

    void DomainTRvExpanded();

    void SwitchView();
    void MenuTriggered(QAction* pAction);
    void DomainActionEvent(QAction* pAction);
    void DomainTRvClicked (const QModelIndex &pIndex);


private:

    QList<QStandardItem*> QSIClipboard;
    ZDomainPath*   DPClipboard=nullptr;

    bool HasChanged=false;

    QIcon MainDomainICn;
    QIcon DomainICn;

    ZFakeBTn*           FakeZFb=nullptr;
    ZFakeBTn*           ViewZFb=nullptr;
    ZFakeBTn*           HierarchyZFb=nullptr;

    QMenuBar*           menuBar=nullptr;
    QMenu*              DomainFlexMEn=nullptr;
    QActionGroup*       DomainActionGroupQAg=nullptr;

    QActionGroup*       ActionGroupQAg=nullptr;
    QAction*            QuitQAc=nullptr;

    QAction*            CreateDomainQAc=nullptr;
    QAction*            InsertDomainQAc=nullptr;
    QAction*            RemoveDomainQAc=nullptr;
    QAction*            CopyDomainQAc=nullptr;
    QAction*            CutDomainQAc=nullptr;
    QAction*            PasteDomainQAc=nullptr;
    QAction*            MoveupQAc=nullptr;
    QAction*            MovedownQAc=nullptr;

    ZBaseParameters     WorkBaseParams;
    ZGeneralParameters  WorkParams;
    ZDomainBroker       WorkDomainBroker;

    uriString           XmlParamFile;

    QFrame*       VerboseFRm=nullptr;
    QWidget*      DomainsWDg=nullptr;

    QLabel*       XmlParamsLBl=nullptr;
    QLabel*       TargetRestoreLBl=nullptr;
    QLabel*       BcksetLBl=nullptr;
    QLabel*       BckDateLBl=nullptr;
    QLabel*       TargetZMFLBl=nullptr;
    QLabel*       CommentLBl=nullptr;
    ZQTableView*  ParamsTBv=nullptr;

    QPushButton*  SwitchViewBTn=nullptr;

    ZQTreeView*   DomainTRv=nullptr;

    QCheckBox*    ZVB_BasicCBx=nullptr;
    QCheckBox*    ZVB_MutexCBx=nullptr;
    QCheckBox*    ZVB_ThreadCBx=nullptr;
    QCheckBox*    ZVB_StatsCBx=nullptr;
    QCheckBox*    ZVB_NetCBx=nullptr;
    QCheckBox*    ZVB_NetStatsCBx=nullptr;
    QCheckBox*    ZVB_XmlCBx=nullptr;
    QCheckBox*    ZVB_ZRFCBx=nullptr;
    QCheckBox*    ZVB_ZMFCBx=nullptr;
    QCheckBox*    ZVB_ZIFCBx=nullptr;
    QCheckBox*    ZVB_MemEngineCBx=nullptr;
    QCheckBox*    ZVB_FileEngineCBx=nullptr;
    QCheckBox*    ZVB_SearchEngineCBx=nullptr;

    QPushButton* SearchXmlFileBTn=nullptr;
    QPushButton* OkBTn=nullptr;
    QPushButton* CancelBTn=nullptr;

    const int ColumnMame=0;
    const int ColumnHierarchy=1;
    const int ColumnType=2;
    const int ColumnContent=3;
    const int ColumnToolTip=4;
    const int ColumnView=5;

    QPoint      GlobalMousePosition;
    QWidget*    ToolTipWDg=nullptr;
    QTextEdit*  ToolTipTEd=nullptr;

    QWidget*    ViewerWNd=nullptr;

    ZaiErrors*   ErrorLog=nullptr;
//    textEditMWn* ComLog=nullptr;
};

#endif // ZGENERALPARAMSDLG_H
