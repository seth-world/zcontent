#include "zgeneralparamsdlg.h"

#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QGridLayout>
#include <QGroupBox>

#include <QStatusBar>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QCheckBox>
#include <QFrame>

#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>

#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>

#include <QPointer>
#include <QVariant>

#include <QModelIndex>
#include <QTextEdit>

#include <QSizePolicy>

#include <ztoolset/uristring.h>

#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory()

#include <zcontent/zcontentutils/zexceptiondlg.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <zqt/zqtwidget/zqtreeview.h>

#include <QTableView>
#include <QItemSelection>
#include <QItemSelectionRange>

#include <QProcess>

#include "texteditmwn.h"

/* for xml exchanges */
#include <zxml/zxmlprimitives.h>

#include <zcontent/zcontentcommon/zcontentobjectbroker.h>
#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zqt/zqtwidget/zfakebtn.h>

#include <QIcon>
#include <QFileDialog>
#include <QFontDialog>
#include <QWindow>

#include "zdomaindlg.h"
#include <zqt/zqtwidget/zdatareference.h>

#include <ztoolset/zfunctions.h>


#define __SEARCH_ICON__ "question.png"

ZGeneralParamsWNd::ZGeneralParamsWNd(ZaiErrors* pErrorLog, QWidget* pParent) : WorkParams(&WorkBaseParams) , QMainWindow(pParent)
{
    ErrorLog=pErrorLog;
    initLayout();
}

ZGeneralParamsWNd::~ZGeneralParamsWNd()
{
//    if (ComLog != nullptr)
//        ComLog->deleteLater();
    if (FakeZFb != nullptr)
        delete FakeZFb;
    if (ViewZFb != nullptr)
        delete ViewZFb;
    if (ToolTipWDg != nullptr)
        ToolTipWDg->deleteLater();

}

void
ZGeneralParamsWNd::initLayout() {


    StatusBar = new QStatusBar(this);
    setStatusBar(StatusBar);

    setWindowTitle("General parameters");
    resize(1050,550);
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QIcon wQuitICn = ContentObjectBroker.iconFactory("general.iconfactory/system-shutdown.png",ErrorLog);
    QuitQAc = new QAction(wQuitICn,"",menuBar);
    menuBar->addAction(QuitQAc);
    ActionGroupQAg = new QActionGroup(menuBar);
    ActionGroupQAg->addAction(QuitQAc);

    QObject::connect(ActionGroupQAg,&QActionGroup::triggered, this,&ZGeneralParamsWNd::MenuTriggered);

    QVBoxLayout* MainQVL=new QVBoxLayout;
    setCentralWidget( new QWidget(this));
    centralWidget()->setLayout(MainQVL);

    QHBoxLayout* QHL=new QHBoxLayout;
    MainQVL->insertLayout(0,QHL);

    QLabel* wLblBcksetLBl = new QLabel("Parameter file");
    QHL->addWidget(wLblBcksetLBl);
    wLblBcksetLBl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QHL->addWidget(wLblBcksetLBl);
    QHL->addSpacing(2);


    XmlParamsLBl=new QLabel();
    XmlParamsLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    QHL->addWidget(XmlParamsLBl);

    SearchXmlFileBTn = new QPushButton("Search");
    SearchXmlFileBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );
    QHL->addWidget(SearchXmlFileBTn);
    QObject::connect(SearchXmlFileBTn, &QPushButton::clicked, this, &ZGeneralParamsWNd::searchXmlFileClicked);


    /* table view */

    QHBoxLayout* QHLTBv=new QHBoxLayout;
    MainQVL->insertLayout(-1,QHLTBv);

    ParamsTBv = new ZQTableView(this);
    QHLTBv->addWidget(ParamsTBv);
    QSizePolicy wSP = ParamsTBv->sizePolicy();
    wSP.setVerticalPolicy(QSizePolicy::Fixed);
    ParamsTBv->setSizePolicy(wSP);

    QHBoxLayout* QHBLViewSwitch = new QHBoxLayout;
    SwitchViewBTn = new QPushButton("Show Verbose",this);
 //   SwitchViewBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QHBLViewSwitch->addWidget(SwitchViewBTn);
    MainQVL->insertLayout(-1,QHBLViewSwitch,Qt::AlignCenter);

    QObject::connect(SwitchViewBTn, &QPushButton::clicked, this, &ZGeneralParamsWNd::SwitchView);

    DomainsWDg = new QWidget(this);
    QVBoxLayout* QVLTRv=new QVBoxLayout;
    DomainTRv = new ZQTreeView(this);
    DomainTRv->addFilterMask(ZEF_MouseFullTrap | ZEF_KeybdFullTrap);
    QVLTRv->addWidget((QWidget*)DomainTRv);
    DomainsWDg->setLayout(QVLTRv);
    MainQVL->addWidget(DomainsWDg);

    VerboseFRm=new QFrame(this);
    VerboseFRm->setFrameShape(QFrame::StyledPanel);
    VerboseFRm->setFrameShadow(QFrame::Raised);
    QVBoxLayout* wVBL1 = new QVBoxLayout;
    VerboseFRm->setLayout(wVBL1);


    MainQVL->addWidget(VerboseFRm);
    VerboseFRm->setVisible(false);

    ZVB_BasicCBx=new QCheckBox("Basic: Messages with no particular domain",this);
    wVBL1->addWidget(ZVB_BasicCBx);
    ZVB_MutexCBx=new QCheckBox("Mutex: Mutexes management",this);
    wVBL1->addWidget(ZVB_MutexCBx);
    ZVB_ThreadCBx=new QCheckBox("Thread: Threads management",this);
    wVBL1->addWidget(ZVB_ThreadCBx);
    ZVB_StatsCBx=new QCheckBox("Stats: Collects stats and performance data",this);
    wVBL1->addWidget(ZVB_StatsCBx);
    ZVB_NetCBx=new QCheckBox("Net: Sockets servers SSL & protocol",this);
    wVBL1->addWidget(ZVB_NetCBx);
    ZVB_NetStatsCBx=new QCheckBox("NetStats: Collects stats and performance for net operations",this);
    wVBL1->addWidget(ZVB_NetStatsCBx);
    ZVB_XmlCBx=new QCheckBox("Xml: Xml engine verbose",this);
    wVBL1->addWidget(ZVB_XmlCBx);

    ZVB_ZRFCBx=new QCheckBox("ZRF: ZRandomFile general messages",this);
    wVBL1->addWidget(ZVB_ZRFCBx);
    ZVB_MemEngineCBx=new QCheckBox("MemEngine: file memory space allocation and management operations",this);
    wVBL1->addWidget(ZVB_MemEngineCBx);
    ZVB_FileEngineCBx=new QCheckBox("FileEngine: file access operations from file engine",this);
    wVBL1->addWidget(ZVB_FileEngineCBx);
    ZVB_SearchEngineCBx=new QCheckBox("SearchEngine: all search operations from all search engines",this);
    wVBL1->addWidget(ZVB_SearchEngineCBx);

    ZVB_ZMFCBx=new QCheckBox("ZMF: ZMasterFile general messages",this);
    wVBL1->addWidget(ZVB_ZMFCBx);
    ZVB_ZIFCBx=new QCheckBox("ZIF: ZIndexFile general messages",this);
    wVBL1->addWidget(ZVB_ZIFCBx);


    /* buttons */

    QHBoxLayout* wButtonBoxQHb=new QHBoxLayout;
    wButtonBoxQHb->setAlignment(Qt::AlignRight);
    MainQVL->insertLayout(-1,wButtonBoxQHb);

    CancelBTn = new QPushButton("Quit");
    wButtonBoxQHb->addWidget(CancelBTn);
    OkBTn = new QPushButton("Update");
    wButtonBoxQHb->addWidget(OkBTn);

    QObject::connect(OkBTn, &QPushButton::clicked, this, &ZGeneralParamsWNd::update);
    QObject::connect(CancelBTn, &QPushButton::clicked, this, &ZGeneralParamsWNd::Quit);

    /* table view setup */

    ParamsTBv->newModel(3);

    ParamsTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("Parameter name"));
    ParamsTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem("Value"));
    ParamsTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem(""));

    ParamsTBv->verticalHeader()->hide();

    QObject::connect(ParamsTBv->ItemModel, &QStandardItemModel::itemChanged, this, &ZGeneralParamsWNd::ParamsTBvChanged);
    QObject::connect(ParamsTBv, &QTableView::clicked, this, &ZGeneralParamsWNd::ParamsTBvClicked);
    QObject::connect(DomainTRv,&ZQTreeView::SelectionModeChanged,this,&ZGeneralParamsWNd::DomainTRvSelModeChanged);

    FakeZFb = new ZFakeBTn ("general.iconfactory.iconlight","Tooltip",ErrorLog, ZFakeBTn::HasBackground ) ;

    ViewZFb = new ZFakeBTn ("general.iconfactory.iconview",ErrorLog) ;

    HierarchyZFb = new ZFakeBTn("general.iconfactory.iconhierarchy",ErrorLog) ;


    DomainTRv->newModel(7);
    DomainTRv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("Domain"));
//    DomainTRv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem("Full hierarchy"));
    DomainTRv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem(""));
    DomainTRv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem("Type"));
    DomainTRv->ItemModel->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);

    DomainTRv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem("Content/value"));
    DomainTRv->ItemModel->setHorizontalHeaderItem(4,new QStandardItem("ToolTip"));
    DomainTRv->ItemModel->setHorizontalHeaderItem(5,new QStandardItem("View"));
    DomainTRv->ItemModel->setHorizontalHeaderItem(6,new QStandardItem(""));

    DomainTRv->setSelectionMode(QAbstractItemView::ExtendedSelection);
    DomainTRv->setSelectionBehavior(QAbstractItemView::SelectRows);

    DomainTRv->_register(std::bind(&ZGeneralParamsWNd::DomainTRvKeyFiltered, this,_1,_2),
                         std::bind(&ZGeneralParamsWNd::DomainTRvMouseFiltered, this,_1,_2),
                         std::bind(&ZGeneralParamsWNd::DomainTRvFlexMenu, this,_1),
                         nullptr);

    QObject::connect(DomainTRv, &QTreeView::clicked, this, &ZGeneralParamsWNd::DomainTRvClicked);
    QObject::connect(DomainTRv,&QTreeView::expanded,this,&ZGeneralParamsWNd::DomainTRvExpanded);

    ZFakeBTnDelegate*wID = new ZFakeBTnDelegate (DomainTRv->ItemModel,this);
    DomainTRv->setItemDelegateForColumn(4,wID);
} //initLayout

void
ZGeneralParamsWNd::DomainTRvExpanded()
{
    for (int wi=0; wi < DomainTRv->ItemModel->columnCount();wi++)
        DomainTRv->resizeColumnToContents(wi);
    return ;
}
void
ZGeneralParamsWNd::DomainTRvSelModeChanged(QAbstractItemView::SelectionMode pSelMode)
{
    if (pSelMode==QAbstractItemView::SingleSelection) {
        StatusBar->showMessage("Row selection set to single selection");
    }
    else
        StatusBar->showMessage("Row selection set to multiple selection");

}
bool
ZGeneralParamsWNd::DomainTRvKeyFiltered(int pKey,QKeyEvent* pEvent)
{
    if(pKey == Qt::Key_Escape) {
        Quit();
        return true ;
    }

    int wKey = pEvent->key();
    if (pEvent->modifiers() &  Qt::ControlModifier) {
        if (wKey == Qt::Key_V) {
            return domainPasteCurrent();
        }
        if ( pEvent->modifiers() &  Qt::ShiftModifier ) {
            if (wKey == Qt::Key_V) {
                return domainPasteUpper();
            }
        }// ctr-shift ...
        if (wKey== Qt::Key_Up) {
            return domainMoveUp();
        }
        if (wKey == Qt::Key_Down) {
            return domainMoveDown();
        }
        if (wKey == Qt::Key_C) {
            return domainCopy();
        }
        if (wKey == Qt::Key_V) {
            return domainPasteCurrent();
        }
        if (wKey == Qt::Key_X) {
           return domainCut();
        }
        if (wKey == Qt::Key_S) {
            update();
            return true;
        }
    } // modifier == ControlModifier

    if(wKey == Qt::Key_Insert)
    {
        return domainCreate();
    }
    if(wKey == Qt::Key_Delete)
    {
        return domainRemove();
    }//Qt::Key_Delete


//    DomainTRv->baseKeyPressEvent(pEvent);
    return true;
}//ZGeneralParamsWNd::KeyTReKeyFiltered

bool
ZGeneralParamsWNd::DomainTRvMouseFiltered(int pKey,QMouseEvent* pEvent)
{
    GlobalMousePosition = pEvent->globalPos();

    if (pKey==ZEF_DoubleClickLeft) {
        return domainChange();
    }
    return false;
}

bool    ZGeneralParamsWNd::domainRemove()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return false;

    QVariant wV = wIdx.data(ZQtValueRole);
    ZDomainPath* wDP=wV.value<ZDomainPath*>();
//    ZDomainPath* wFather = WorkDomainBroker.findFatherDomain(wDP);
/*    if ((wDP->Father == WorkDomainBroker.DomainRoot ) || (wDP->Father==nullptr)) {
        for (int wi=0; wi < WorkDomainBroker.DomainRoot->Inferiors.count(); wi++) {
            if (wDP==WorkDomainBroker.DomainRoot->Inferiors[wi]) {
                WorkDomainBroker.DomainRoot->Inferiors.erase(wi);
                break;
            }
        }
        DomainTRv->ItemModel->removeRow(wIdx.row(),wIdx.parent());
        return;
    }
*/
 //   if (wDP->Father != nullptr) {
        for (int wj=0 ; wj < wDP->Father->Inferiors.count(); wj++) {
            if (wDP==wDP->Father->Inferiors[wj]) {
                wDP->Father->Inferiors.erase(wj);
                break;
            }
        }
//    }

    DomainTRv->ItemModel->removeRow(wIdx.row(),wIdx.parent());
    return true;
} //ZGeneralParamsWNd::domainRemove

/* create a new domain at the same level of current in place (row) of current */
bool    ZGeneralParamsWNd::domainCreate()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return false;

    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    ZDomainPath* wDPFather=nullptr;
    QStandardItem* wParentItem = DomainTRv->ItemModel->itemFromIndex( wRIdx.parent());
    ZDomainDLg* wDomDLg = new ZDomainDLg(this);
    if ((wParentItem==nullptr) || (wParentItem==DomainTRv->ItemModel->invisibleRootItem())) {
        wDPFather = WorkDomainBroker.getRoot();
    }
    else {
        QVariant wV = wParentItem->data(ZQtValueRole);
        if (wV.isNull()) {
            ErrorLog->errorLog("ZGeneralParamsWNd::domainCreate-E-NULL Cannot get item data from item<%s>",wParentItem->text().toUtf8().data());
            return false;
        }
        wDPFather=wV.value<ZDomainPath*>();
    }
    wDomDLg->setupCreate(wDPFather,WorkDomainBroker.getRoot());
    int wRet = wDomDLg->exec();
    if (wRet==QDialog::Rejected)
        return true;

    ZDomainPath* wDP=wDomDLg->getDomainPath();

    HasChanged=true;

    QList<QStandardItem*> wRow ;

    wRow << createItemAligned(wDP->Name,Qt::AlignLeft);
    if (wDP->Father==WorkDomainBroker.getRoot())
        wRow[0]->setIcon(MainDomainICn);
    else
        wRow[0]->setIcon(DomainICn);

    QVariant wV;
    wV.setValue(wDP);
    wRow[0]->setData(wV,ZQtValueRole);

    wRow.last()->setFlags(Qt::ItemIsEnabled );

//    utf8VaryingString wHierarchy=wDP->getHierarchy() ;
//    wRow << createItemAligned(wHierarchy.toCChar(),Qt::AlignLeft);
    wRow << HierarchyZFb->create() ;

    wRow << createItem(decode_ZDomainTypeForDisplay(wDP->Type));
    wRow << createItem(wDP->Content);
    wRow.last()->setTextAlignment(Qt::AlignLeft);

    if (!wDP->ToolTip.isEmpty())
        wRow << FakeZFb->create() ;
    else
        wRow << new QStandardItem("");

//    if (wDP->isViewable()) {
    if (wDP->hasViewer()) {
        wRow << ViewZFb->create() ;
    }
    else
        wRow << new QStandardItem("");

    for (int wi=0; wi < wDP->Inferiors.count() ; wi++) {
        appendDomainRow(wDP->Inferiors[wi],wRow[0] );
    }
    if (wParentItem==nullptr || (wParentItem==DomainTRv->ItemModel->invisibleRootItem()) ) {
        if (DomainTRv->ItemModel->rowCount()==0) {
            DomainTRv->ItemModel->appendRow(wRow);
            wDP->Father->Inferiors.push(wDP);
        }
        else {
            DomainTRv->ItemModel->insertRow(wIdx.row(),wRow);
            wDP->Father->Inferiors.insert(wDP,wIdx.row());
        }
    }
    else {
        wParentItem->appendRow(wRow);
        wDP->Father->Inferiors.push(wDP);
    }
    return true;
} // ZGeneralParamsWNd::domainCreate

bool ZGeneralParamsWNd::domainInsert()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return false;


    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    ZDomainPath* wDPFather=nullptr;

    QStandardItem* wParentItem = DomainTRv->ItemModel->itemFromIndex( wRIdx);
    ZDomainDLg* wDomDLg = new ZDomainDLg(this);
    if ((wParentItem==nullptr) || (wParentItem==DomainTRv->ItemModel->invisibleRootItem())) {
        wDPFather = WorkDomainBroker.getRoot();
    }
    else {
        QVariant wV = wParentItem->data(ZQtValueRole);
        wDPFather=wV.value<ZDomainPath*>();
    }
    wDomDLg->setupCreate(wDPFather,WorkDomainBroker.getRoot());

    int wRet = wDomDLg->exec();
    if (wRet==QDialog::Rejected)
        return true;

    HasChanged=true;

    ZDomainPath* wDP = wDomDLg->getDomainPath();
    wDP->Father = wDPFather;

    wDP->Father->Inferiors.push(wDP);

    QList<QStandardItem*> wRow ;

    wRow << createItemAligned(wDP->Name,Qt::AlignLeft);
    if (wDP->Father==WorkDomainBroker.getRoot())
        wRow[0]->setIcon(MainDomainICn);
    else
        wRow[0]->setIcon(DomainICn);

    QVariant wV;
    wV.setValue(wDP);
    wRow[0]->setData(wV,ZQtValueRole);

    wRow << HierarchyZFb->create();
//    wRow << createItemAligned(wDP->getHierarchy().toCChar(),Qt::AlignLeft);

    wRow << createItem(decode_ZDomainTypeForDisplay(wDP->Type));

    wRow << createItemAligned(wDP->Content,Qt::AlignLeft);

    if (!wDP->ToolTip.isEmpty())
        wRow << FakeZFb->create() ;
    else
        wRow << new QStandardItem("");

//    if (wDP->isViewable()) {
    if (wDP->hasViewer()) {
        wRow << ViewZFb->create() ;
    }
    else
        wRow << new QStandardItem("");


    for (int wi=0; wi < wDP->Inferiors.count() ; wi++) {
        utf8VaryingString wHierarchy = wDP->getHierarchy() ;
        appendDomainRow(wDP->Inferiors[wi], wRow[0] );
    }
    wParentItem->appendRow(wRow);
    return true;
} // ZGeneralParamsWNd::domainInsert

void
ZGeneralParamsWNd::hierarchyCascadeUpdate(QModelIndex pIdx)
{
    QStandardItem* wSI = DomainTRv->ItemModel->itemFromIndex( pIdx );
    for (int wi=0; wi < wSI->rowCount() ; wi++) {
        QStandardItem* wSIc = wSI->child(wi,0);
        QVariant wV = wSIc->data(ZQtValueRole);
        ZDomainPath* wChildDP=wV.value<ZDomainPath*>();
        QModelIndex wSIcIdx = wSIc->index();
        QModelIndex wSIhIdx = wSIcIdx.siblingAtColumn(1);
        QStandardItem* wSIh = DomainTRv->ItemModel->itemFromIndex( wSIhIdx );
        wSIh->setText(wChildDP->getHierarchy().toCChar());
        if (wSIc->rowCount()!=0)
            hierarchyCascadeUpdate(wSIcIdx);
    }
}

bool
ZGeneralParamsWNd::domainChange()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return false;

    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    ZDomainDLg* wDomDLg = new ZDomainDLg(this);

    QVariant wV = wRIdx.data(ZQtValueRole);
    if (wV.isNull()) {
        ErrorLog->errorLog("ZGeneralParamsWNd::domainChange-E-INFRADATA Wrong infradata (ZQtValueRole) for tree view.");
        return false;
    }
    ZDomainPath* wDP=wV.value<ZDomainPath*>();
    wDomDLg->setupChange(wDP,WorkDomainBroker.getRoot());


    int wRet = wDomDLg->exec();
    if (wRet==QDialog::Rejected)
        return false ;

    HasChanged=true;
    QStandardItem* wSI=DomainTRv->ItemModel->itemFromIndex( wRIdx );
    wSI->setText(wDP->Name.toCChar());

    wRIdx = wRIdx.siblingAtColumn(1);
    wSI=DomainTRv->ItemModel->itemFromIndex( wRIdx );
//    wSI->setText(wDP->getHierarchy().toCChar());

    wRIdx = wRIdx.siblingAtColumn(2);
    wSI=DomainTRv->ItemModel->itemFromIndex( wRIdx );
    wSI->setText(decode_ZDomainTypeForDisplay( wDP->Type).toCChar() );

    wRIdx = wRIdx.siblingAtColumn(3);
    wSI=DomainTRv->ItemModel->itemFromIndex( wRIdx );
    wSI->setText(wDP->Content.toCChar());
    wSI->setTextAlignment(Qt::AlignLeft);


    if (!wDP->ToolTip.isEmpty())
        wSI = FakeZFb->create() ;
    else
        wSI = new QStandardItem("");

    DomainTRv->ItemModel->setItem(wRIdx.row(),4,wSI);

//    if (wDP->isViewable())
    if (wDP->hasViewer())
        wSI = ViewZFb->create() ;
    else
        wSI = new QStandardItem("");

    DomainTRv->ItemModel->setItem(wRIdx.row(),5,wSI);

    if (wDomDLg->NameHasChanged) { /* cascade update inferiors' hierarchy */
        hierarchyCascadeUpdate(wRIdx.siblingAtColumn(0));
    }

    return true;

} // ZGeneralParamsWNd::domainChange

bool    ZGeneralParamsWNd::domainPasteUpper()
{
   if (DomainClipboard.isEmpty())
        return false;


    QModelIndex wIdx= DomainTRv->currentIndex().siblingAtColumn(0);
    if(!wIdx.isValid())
        return false;
    QStandardItem* wQSIFather = DomainTRv->ItemModel->itemFromIndex(wIdx)->parent() ;
    bool wIsFirstLevel=false;
    if ( wQSIFather == DomainTRv->ItemModel->invisibleRootItem() )
        wIsFirstLevel=true;


    QVariant wV = wQSIFather->data(ZQtValueRole);
    ZDomainPath* wDPFather= wV.value<ZDomainPath*>();

    int wi=0;
    for ( ; wi < DomainClipboard.count(); wi++) {
        DomainClipboard[wi]->useDomainPath()->Father = wDPFather;  /* use domain path and not get domain path */
        wDPFather->Inferiors.push(DomainClipboard[wi]->useDomainPath()); /* use domain path and not get domain path */
        wV.setValue(DomainClipboard[wi]->getDomainPath()); /* get domain path and destroy it from clipboard */
        if (wIsFirstLevel) {
            QIcon wIcn = ContentObjectBroker.iconFactory("general.iconfactory.rightarrow",ErrorLog);
            DomainClipboard[wi]->Row[0]->setIcon(wIcn);
        }
        else {
            QIcon wIcn = ContentObjectBroker.iconFactory("general.iconfactory.curvedarrowdownright",ErrorLog);
            DomainClipboard[wi]->Row[0]->setIcon(wIcn);
        }

        wQSIFather->appendRow(DomainClipboard[wi]->Row);
    }// for

    utf8VaryingString wMsg;
    wMsg.sprintf("%d rows appended to <%s>",wi,wQSIFather->text().toUtf8().data());

    StatusBar->showMessage(wMsg.toCChar());

    DomainClipboard.clear();
//    DPClipboard = nullptr;
    return true;
}// ZGeneralParamsWNd::domainPasteUpper

bool
ZGeneralParamsWNd::domainPasteCurrent() /* paste selection to current row as children */
{
    if ( DomainClipboard.isEmpty() )
        return false;

    QModelIndex wIdx= DomainTRv->currentIndex().siblingAtColumn(0);
    if(!wIdx.isValid())
        return false;
    QStandardItem* wQSIFather = DomainTRv->ItemModel->itemFromIndex(wIdx);

    bool wIsFirstLevel=false;
    if ( wQSIFather == DomainTRv->ItemModel->invisibleRootItem() )
        wIsFirstLevel=true;

    QVariant wV = wQSIFather->data(ZQtValueRole);
    ZDomainPath* wDPFather= wV.value<ZDomainPath*>();
    int wi=0;
    for ( ; wi < DomainClipboard.count(); wi++) {
        DomainClipboard[wi]->useDomainPath()->Father = wDPFather;  /* use domain path and not get domain path */
        wDPFather->Inferiors.push(DomainClipboard[wi]->useDomainPath()); /* use domain path and not get domain path */
        wV.setValue(DomainClipboard[wi]->getDomainPath()); /* get domain path and destroy it from clipboard */
        DomainClipboard[wi]->Row[0]->setData(wV,ZQtValueRole);
        if (wIsFirstLevel) {
            QIcon wIcn = ContentObjectBroker.iconFactory("general.iconfactory.rightarrow",ErrorLog);
            DomainClipboard[wi]->Row[0]->setIcon(wIcn);
        }
        else {
            QIcon wIcn = ContentObjectBroker.iconFactory("general.iconfactory.curvedarrowdownright",ErrorLog);
            DomainClipboard[wi]->Row[0]->setIcon(wIcn);
        }
        wQSIFather->appendRow(DomainClipboard[wi]->Row);
    }// for

    utf8VaryingString wMsg;
    wMsg.sprintf("%d rows appended to <%s>",wi,wQSIFather->text().toUtf8().data());

    StatusBar->showMessage(wMsg.toCChar());


    DomainClipboard.clear();
    return true;
}// ZGeneralParamsWNd::domainPaste

void ZGeneralParamsWNd::feedRowHierarchy(ZDomainPath*   pDP , QStandardItem* pRow)
{
    for (int wj=0; wj < pRow->rowCount() ; wj++) {
        QStandardItem* wSubRow = pRow->child(wj,0);
        QVariant wV ;
        wV.setValue( pDP->Inferiors[wj] );
        wSubRow->setData(wV,ZQtValueRole);
        if (wSubRow->hasChildren()) {
            feedRowHierarchy(pDP->Inferiors[wj],wSubRow) ;
        }
    } // for
} // ZGeneralParamsWNd::feedRowHierarchy

bool ZGeneralParamsWNd::domainCut()
{
    if (!DomainTRv->getSelectionModel()->hasSelection())
        return false;

    QModelIndexList wQMList = DomainTRv->getSelectionModel()->selectedRows();

    /* translate list of index to list of items */
    ZArray<QStandardItem*> wSIList;
    for (int wi=0; wi < wQMList.count() ; wi ++) {
        QModelIndex wIdx = wQMList[wi].siblingAtColumn(0);
        wSIList.push( DomainTRv->ItemModel->itemFromIndex(wIdx));
    }
    int wi = 0 ;
    for (  ; wi < wQMList.count() ; wi++ ) {
        ZDomainClipboardElement *wElt = new ZDomainClipboardElement ;
        QStandardItem* wQSI =wSIList[wi];

        QVariant wV = wQSI->data(ZQtValueRole);
        ZDomainPath* pNewDP = wV.value<ZDomainPath*>(); /* debug */
        if (pNewDP==nullptr)
            _DBGPRINT("ZGeneralParamsWNd::_cutDomainRow-E-NULL Invalid infra data for row <%d> text <%s>\n",wQSI->row(),wQSI->text().toUtf8().data())
        ZDomainPath* wDP = wV.value<ZDomainPath*>();
        bool wFound = false;
        for (int wj = 0 ; wj < wDP->Father->Inferiors.count() ; wj++ ) {
            if (wDP->Father->Inferiors[wj]==wDP) {
                wFound = true;
                wDP->Father->Inferiors.erase(wj);
                wDP->Father = nullptr;
                wElt->copyDomainPath(wDP); /* create a new instance of DomainPath object as copy */
                delete wDP;
                break;
            }
        }

        QStandardItem* wQSIParent = wQSI->parent();
        if ((wQSIParent==nullptr)||(wQSIParent==DomainTRv->ItemModel->invisibleRootItem())) {
            wElt->Row = DomainTRv->ItemModel->invisibleRootItem()->takeRow(wQSI->row());
        }
        else
            wElt->Row = wQSIParent->takeRow(wQSI->row());

        wV.setValue(wElt->useDomainPath()) ;
        wElt->Row[0]->setData(wV,ZQtValueRole);

        if (wElt->Row[0]->hasChildren()) {
            feedRowHierarchy(wElt->useDomainPath(), wElt->Row[0] );
        }
        DomainClipboard.push(wElt);
    } // for

    utf8VaryingString wMsg;
    wMsg.sprintf("%d rows cut",wi);

    StatusBar->showMessage(wMsg.toCChar());
    return true;
}//ZGeneralParamsWNd::domainCut

QList<QStandardItem*>
ZGeneralParamsWNd::_copyDomainRow(QStandardItem* pFirstItem,ZDomainPath* pNewDP)
{
    QList<QStandardItem*> wRow ;
    QModelIndex wIdx =  pFirstItem->index().siblingAtColumn(0) ;
    QStandardItem* wFirstItem = DomainTRv->ItemModel->itemFromIndex(wIdx);

    QStandardItem* wNewItem = wFirstItem->clone() ;
    QVariant wV ;
    wV.setValue(pNewDP) ;
    wNewItem->setData(wV,ZQtValueRole) ;

    wRow << wNewItem;

    int wi = 1 ;
    wIdx=wIdx.siblingAtColumn(wi++);
    while (wIdx.isValid()) {
        QStandardItem* wOldSI = DomainTRv->ItemModel->itemFromIndex(wIdx) ;
        QStandardItem* wSI = wOldSI->clone() ;

        wRow << wSI ;
        wIdx=wIdx.siblingAtColumn(wi++);
    }
    return wRow;
} //ZGeneralParamsWNd::_copyDomainRow

ZDomainClipboardElement*
ZGeneralParamsWNd::_cloneDomainRow(QModelIndex pItemIdx)
{
    QModelIndex wIdx =  pItemIdx.siblingAtColumn(0) ;
    /*
      get the father's QStandardItem from QModelIndex
      duplicate the whole Row items
    */

    ZDomainClipboardElement *wElt = new ZDomainClipboardElement ;

//    wElt->clear();
    QStandardItem* wQSIOld = DomainTRv->ItemModel->itemFromIndex(wIdx);
    QStandardItem* wNewItem = wQSIOld->clone();  /* Nb: Doc says "children items are not copied" - But infra data is copied */

    QVariant wV = wIdx.data(ZQtValueRole);
//    ZDomainPath* wNewDP = wV.value<ZDomainPath*>(); /* debug */
    wElt->copyDomainPath(wV.value<ZDomainPath*>()); /* create a new instance of DomainPath object as copy */
    wNewItem->setData(wV,ZQtValueRole);
    wElt->Row << wNewItem ;
    int wi = 1;
    while (wi < DomainTRv->ItemModel->columnCount()) {
        wIdx.siblingAtColumn(wi);
        wQSIOld = DomainTRv->ItemModel->itemFromIndex(wIdx);
        wNewItem = wQSIOld->clone();
        wElt->Row << wNewItem ;
        wi++ ;
    }

    return wElt;
} //ZGeneralParamsWNd::_cloneDomainRow

ZDomainClipboardElement*
ZGeneralParamsWNd::_cutDomainRow(QModelIndex pItemIdx)
{
    QModelIndex wIdx =  pItemIdx.siblingAtColumn(0) ;
    /*
      get the father's QStandardItem from QModelIndex
      duplicate the whole Row items
    */

    ZDomainClipboardElement *wElt = new ZDomainClipboardElement ;

    QStandardItem* wQSIOld = DomainTRv->ItemModel->itemFromIndex(wIdx);
    QStandardItem* wQSIParent = wQSIOld->parent();

    QVariant wV = wQSIOld->data(ZQtValueRole);
    ZDomainPath* pNewDP = wV.value<ZDomainPath*>(); /* debug */
    if (pNewDP==nullptr)
        _DBGPRINT("ZGeneralParamsWNd::_cutDomainRow-E-NULL Invalid infra data for row <%d> text <%s>\n",wQSIOld->row(),wQSIOld->text().toUtf8().data())
    wElt->copyDomainPath(wV.value<ZDomainPath*>()); /* create a new instance of DomainPath object as copy */
    wV.setValue(wElt->useDomainPath());

    if ((wQSIParent==nullptr)||(wQSIParent==DomainTRv->ItemModel->invisibleRootItem())) {
        wElt->Row = DomainTRv->ItemModel->invisibleRootItem()->takeRow(wQSIOld->row());
    }
    else
        wElt->Row = wQSIParent->takeRow(wQSIOld->row());
    /*
    wElt->useDomainPath()->Father->Inferiors[wi]

    wElt->useDomainPath()->Father = nullptr;
    */
    wElt->Row[0]->setData(wV,ZQtValueRole);

    return wElt;
} //ZGeneralParamsWNd::_cutDomainRow




bool
ZGeneralParamsWNd::domainCopy()
{
    if (!DomainTRv->getSelectionModel()->hasSelection())
        return false;

    QModelIndexList wQMList = DomainTRv->getSelectionModel()->selectedRows();

    for (int wi = 0 ; wi < wQMList.count() ; wi++ ) {
        ZDomainClipboardElement* wElt = _cloneDomainRow(wQMList[wi]);
        if (wElt!=nullptr)
            DomainClipboard.push(wElt);
    }
    utf8VaryingString wMsg;
    wMsg.sprintf("%d rows selected",wQMList.count());

    StatusBar->showMessage(wMsg.toCChar());

    return true;
}


bool    ZGeneralParamsWNd::domainMoveDown()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return false;
    int wRow = wIdx.row();


    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    QModelIndex wParentIdx = wRIdx.parent();
    //    if (wParentIdx.isValid())

    QStandardItem* wSIParent = DomainTRv->ItemModel->itemFromIndex(wParentIdx);
    if (wSIParent==nullptr)
        wSIParent=DomainTRv->ItemModel->invisibleRootItem();
    if (wRow==wSIParent->rowCount()-1)
        return false;

    QVariant wV = wRIdx.data(ZQtValueRole);
    if (wV.isNull()) {
        ErrorLog->errorLog("ZGeneralParamsWNd::domainMoveDown-E-IVINFRA Invalid infra data at row %d.",wRow);
        return false;
    }
    ZDomainPath* wDP = wV.value<ZDomainPath*>();

    wDP->Father->Inferiors.erase(wRow);
    QList<QStandardItem*> wRowSI = wSIParent->takeRow(wRow);
    if (wRow+1 == wDP->Father->Inferiors.count()) {
        wDP->Father->Inferiors.push(wDP);
        wSIParent->appendRow(wRowSI);
        return true;
    }
    wSIParent->insertRow(wRow+1,wRowSI);
    wDP->Father->Inferiors.insert(wDP,wRow+1);
    return true;
}// ZGeneralParamsWNd::domainMoveDown

bool
ZGeneralParamsWNd::domainMoveUp()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return false;
    int wRow = wIdx.row();
    if (wRow==0)
        return false;

    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    QModelIndex wParentIdx = wRIdx.parent();
//    if (wParentIdx.isValid())

    QStandardItem* wSIParent = DomainTRv->ItemModel->itemFromIndex(wParentIdx);
    if (wSIParent==nullptr)
        wSIParent=DomainTRv->ItemModel->invisibleRootItem();


    QVariant wV = wRIdx.data(ZQtValueRole);
    if (wV.isNull()) {
        ErrorLog->errorLog("ZGeneralParamsWNd::domainMoveUp-E-IVINFRA Invalid infra data at row %d.",wRow);
        return false;
    }
    ZDomainPath* wDP = wV.value<ZDomainPath*>();

    wDP->Father->Inferiors.erase(wRow);
    wDP->Father->Inferiors.insert(wDP,wRow-1);

    QList<QStandardItem*> wRowSI = wSIParent->takeRow(wRow);
//    DomainTRv->ItemModel->beginInsertRows(wSIParent,wRow-1,wRow);
    wSIParent->insertRow(wRow-1,wRowSI);
//    DomainTRv->ItemModel->endInsertRows();
    return true;
} // ZGeneralParamsWNd::domainMoveUp



void ZGeneralParamsWNd::DomainTRvFlexMenu(QContextMenuEvent *event)
{
    if (DomainFlexMEn==nullptr)
        setupDomainFlexMenu();
    DomainFlexMEn->exec(event->globalPos());
}

QMenu *
ZGeneralParamsWNd::setupDomainFlexMenu()
{
    DomainFlexMEn=new QMenu(DomainTRv);

    DomainFlexMEn->setTitle("Domains");

    DomainActionGroupQAg=new QActionGroup(DomainFlexMEn) ;
    QObject::connect(DomainActionGroupQAg, &QActionGroup::triggered, this, &ZGeneralParamsWNd::DomainActionEvent);

    CreateDomainQAc= new QAction("Add domain at same level<Insert>",DomainFlexMEn);
    DomainFlexMEn->addAction(CreateDomainQAc);
    DomainActionGroupQAg->addAction(CreateDomainQAc);


    InsertDomainQAc= new QAction("Add to current as inferior<ctrl-Insert>",DomainFlexMEn);
    DomainFlexMEn->addAction(InsertDomainQAc);
    DomainActionGroupQAg->addAction(InsertDomainQAc);

    CopyDomainQAc= new QAction("Copy selection<Ctrl-C>",DomainFlexMEn);
    DomainFlexMEn->addAction(CopyDomainQAc);
    DomainActionGroupQAg->addAction(CopyDomainQAc);

    CutDomainQAc= new QAction("Cut selection<Ctrl-X>",DomainFlexMEn);
    DomainFlexMEn->addAction(CutDomainQAc);
    DomainActionGroupQAg->addAction(CutDomainQAc);

    DomainFlexMEn->addSeparator();

    PasteDomainUpperQAc= new QAction("Paste to father<Ctrl-Shift-V>",DomainFlexMEn);
    DomainFlexMEn->addAction(PasteDomainUpperQAc);
    DomainActionGroupQAg->addAction(PasteDomainUpperQAc);

    PasteDomainCurrentQAc= new QAction("Paste to current<Ctrl-V>",DomainFlexMEn);
    DomainFlexMEn->addAction(PasteDomainCurrentQAc);
    DomainActionGroupQAg->addAction(PasteDomainCurrentQAc);

    DomainFlexMEn->addSeparator();

    RemoveDomainQAc= new QAction("Remove selection<Delete>",DomainFlexMEn);
    DomainFlexMEn->addAction(RemoveDomainQAc);
    DomainActionGroupQAg->addAction(RemoveDomainQAc);

    DomainFlexMEn->addSeparator();

    MoveupQAc= new QAction(DomainFlexMEn);
    MoveupQAc->setText("Move up<Ctrl-up arrow>");
    DomainFlexMEn->addAction(MoveupQAc);
    DomainActionGroupQAg->addAction(MoveupQAc);

    MovedownQAc= new QAction(DomainFlexMEn);
    MovedownQAc->setText("Move down <Ctrl-down arrow>");
    DomainFlexMEn->addAction(MovedownQAc);
    DomainActionGroupQAg->addAction(MovedownQAc);

    return DomainFlexMEn;
}//setupDomainFlexMenu

void
ZGeneralParamsWNd::MenuTriggered(QAction* pAction)
{
    if ((pAction==QuitQAc))
    {
        Quit();
        return;
    }
}


void
ZGeneralParamsWNd::DomainActionEvent(QAction* pAction)
{
    if ((pAction==CreateDomainQAc)) {
        domainCreate();
        return;
    }
    if ((pAction==InsertDomainQAc)) {
        domainInsert();
        return;
    }
    if ((pAction==PasteDomainUpperQAc)) {
        domainPasteUpper();
        return;
    }
    if ((pAction==PasteDomainCurrentQAc)) {
        domainPasteCurrent();
        return;
    }
    if ((pAction==CopyDomainQAc)) {
        domainCopy();
        return;
    }
    if ((pAction==MoveupQAc)) {
        domainMoveUp();
        return;
    }
    if ((pAction==MovedownQAc)) {
        domainMoveDown();
        return;
    }
    if ((pAction==CutDomainQAc)) {
        domainCut();
        return;
    }
    if ((pAction==RemoveDomainQAc)) {
        domainRemove();
        return;
    }
}


void
ZGeneralParamsWNd::SwitchView()
{
    if(VerboseFRm->isVisible()) {
        VerboseFRm->setVisible(false);
        DomainsWDg->setVisible(true);
        SwitchViewBTn->setText("Show Verbose");
        return;
    }
    VerboseFRm->setVisible(true);
    DomainsWDg->setVisible(false);
    SwitchViewBTn->setText("Show Domains");
}





void
ZGeneralParamsWNd::ParamsTBvChanged (QStandardItem *pItem)
{
    int wRet=0;
    if (pItem->column() != 1)
        return;
    if (pItem->row() < 3) {
        uriString wDir=ParamsTBv->ItemModel->item(pItem->row(),1)->text().toUtf8().data();
        if (!wDir.exists()) {
            wRet=ZExceptionDLg::adhocMessage3B("Directory",Severity_Warning,"Find another","Give up","Force",
                                                     "Directory does not exist.\nFind another one, force anyway give up ?");
            if (wRet==ZEDLG_Third) { /* if third : search for a new directory value */
                wRet =getDir(wDir);
            }
            if (wRet==QDialog::Rejected) {
                switch (pItem->row())
                {
                case 0:
                    pItem->setText(WorkParams.WorkDirectory.toCChar());
                    return;
                case 1:
                    pItem->setText(WorkParams.ParamDirectory.toCChar());
                    return;
                case 2:
                    pItem->setText(WorkParams.IconDirectory.toCChar());
                    return;
                }// switch
                return;
            }
            if (wRet==QDialog::Accepted) { /* if ok : force the directory value to a non existing one */
                switch (pItem->row()) {
                case 0:
                    WorkParams.WorkDirectory = wDir;
                    pItem->setText(WorkParams.WorkDirectory.toCChar());
                    return;
                case 1:
                    WorkParams.ParamDirectory = wDir;
                    pItem->setText(WorkParams.ParamDirectory.toCChar());
                    return;
                case 2:
                    WorkParams.IconDirectory = wDir;
                    pItem->setText(WorkParams.IconDirectory.toCChar());
                    return;
                } // switch
            } // Accepted
        }
        switch (pItem->row())
        {
        case 0:
            WorkParams.WorkDirectory = wDir;
            return;
        case 1:
            WorkParams.ParamDirectory = wDir;
            return;
        case 2:
            WorkParams.IconDirectory = wDir;
            return;
        }// switch
    } // if (pIndex.row()<3)

    if (pItem->row() == 3) {
        if (QFont(pItem->text()).exactMatch())
            WorkParams.FixedFont = pItem->text().toUtf8().data();
        else {
            utf8VaryingString wFont = pItem->text().toUtf8().data();
            int wRet=getFont(wFont);
            if (wRet==QDialog::Rejected) {
                pItem->setText("");
                return;
            }
            WorkParams.FixedFont = wFont;
            pItem->setText(WorkParams.FixedFont.toCChar());
        }
        return;
    } // if (pItem->row() == 3)

}
int
ZGeneralParamsWNd::getFont(utf8VaryingString& pFont)
{
    QFont wFont;
    QFontDialog* wFontDLg=nullptr;
    if (!pFont.isEmpty()) {
        wFont.setFamily(pFont.toCChar());
        wFontDLg=new QFontDialog(wFont,this);
    }
    else
        wFontDLg=new QFontDialog(this);

    int wRet=wFontDLg->exec();
    if (wRet==QDialog::Rejected)
        return wRet;
    wFont = wFontDLg->selectedFont();
    pFont = wFont.family().toUtf8().data();
    return QDialog::Accepted;
}
int
ZGeneralParamsWNd::getDir(uriString &pDir)
{
    uriString wDir=pDir;
    QString wFileName = QFileDialog::getExistingDirectory(this, tr("Directory"),
                                                     wDir.toCChar());
    if (wFileName.isEmpty())
        return QDialog::Rejected;
    pDir=wFileName.toUtf8().data();
    return QDialog::Accepted;
}

void
ZGeneralParamsWNd::ParamsTBvClicked (const QModelIndex &pIndex)
{
    if (pIndex.column() != 2)
        return;

    if (pIndex.row()<3) {
        uriString wDir=ParamsTBv->ItemModel->item(pIndex.row(),1)->text().toUtf8().data();
        int wRet=getDir(wDir);
        if (wRet==QDialog::Rejected)
            return;
        /* a directory has been selected */

        switch (pIndex.row())
        {
        case 0:
            WorkParams.WorkDirectory = wDir;
            ParamsTBv->ItemModel->item(pIndex.row(),1)->setText(WorkParams.WorkDirectory.toCChar());
            return;
        case 1:
            WorkParams.ParamDirectory = wDir;
            ParamsTBv->ItemModel->item(pIndex.row(),1)->setText(WorkParams.ParamDirectory.toCChar());
            return;
        case 2:
            WorkParams.IconDirectory = wDir;
            ParamsTBv->ItemModel->item(pIndex.row(),1)->setText(WorkParams.IconDirectory.toCChar());
            return;
        }// switch
    } // if (pIndex.row()<3)

    if (pIndex.row()==3) {
        int wRet=getFont(WorkParams.FixedFont);
        if (wRet==QDialog::Rejected)
            return;
        ParamsTBv->ItemModel->item(pIndex.row(),1)->setText(WorkParams.FixedFont.toCChar());
        return;
    } //if (pIndex.row()==3)

}

ZVerbose_Base
ZGeneralParamsWNd::fromScreenVerbose()
{
    ZVerbose_Base wVerbose=ZVB_NoVerbose;

    if (ZVB_BasicCBx->isChecked())
        wVerbose |= ZVB_Basic ;
    if (ZVB_MutexCBx->isChecked())
        wVerbose |= ZVB_Mutex ;
    if (ZVB_ThreadCBx->isChecked())
        wVerbose |= ZVB_Thread ;
    if (ZVB_StatsCBx->isChecked())
        wVerbose |= ZVB_Stats ;
    if (ZVB_NetCBx->isChecked())
        wVerbose |= ZVB_Net ;
    if (ZVB_NetStatsCBx->isChecked())
        wVerbose |= ZVB_NetStats ;
    if (ZVB_XmlCBx->isChecked())
        wVerbose |= ZVB_Xml ;
    if (ZVB_ZRFCBx->isChecked())
        wVerbose |= ZVB_ZRF ;
    if (ZVB_ZMFCBx->isChecked())
        wVerbose |= ZVB_ZMF ;
    if (ZVB_ZIFCBx->isChecked())
        wVerbose |= ZVB_ZIF ;
    if (ZVB_MemEngineCBx->isChecked())
        wVerbose |= ZVB_MemEngine ;
    if (ZVB_FileEngineCBx->isChecked())
        wVerbose |= ZVB_FileEngine ;
    if (ZVB_SearchEngineCBx->isChecked())
        wVerbose |= ZVB_SearchEngine ;

    return wVerbose;
}
void
ZGeneralParamsWNd::displayVerbose(ZVerbose_Base pVerbose)
{
    ZVB_BasicCBx->setChecked(pVerbose&ZVB_Basic);
    ZVB_MutexCBx->setChecked(pVerbose&ZVB_Mutex);
    ZVB_ThreadCBx->setChecked(pVerbose&ZVB_Thread);
    ZVB_StatsCBx->setChecked(pVerbose&ZVB_Stats);
    ZVB_NetCBx->setChecked(pVerbose&ZVB_Net);
    ZVB_NetStatsCBx->setChecked(pVerbose&ZVB_NetStats);
    ZVB_XmlCBx->setChecked(pVerbose&ZVB_Xml);
    ZVB_ZRFCBx->setChecked(pVerbose&ZVB_ZRF);
    ZVB_ZMFCBx->setChecked(pVerbose&ZVB_ZMF);
    ZVB_ZIFCBx->setChecked(pVerbose&ZVB_ZIF);
    ZVB_MemEngineCBx->setChecked(pVerbose&ZVB_MemEngine);
    ZVB_FileEngineCBx->setChecked(pVerbose&ZVB_FileEngine);
    ZVB_SearchEngineCBx->setChecked(pVerbose&ZVB_SearchEngine);

    return ;
}

QList<QStandardItem*>
ZGeneralParamsWNd::createDomainRow(ZDomainPath* pDomain)
{
    QList<QStandardItem*> wRow;

    wRow << createItemAligned(pDomain->Name,Qt::AlignLeft);
    if (pDomain->isFirstLevel())
        wRow[0]->setIcon(MainDomainICn);
    else
        wRow[0]->setIcon(DomainICn);

    QVariant wV;
    wV.setValue(pDomain);
    wRow[0]->setData(wV,ZQtValueRole);

//    wRow.last()->setFlags(Qt::ItemIsEnabled );

//    wRow << createItemAligned(pDomain->getHierarchy(),Qt::AlignLeft);
    wRow << HierarchyZFb->create();

//    wRow << createItem(decode_ZDomain_type(pDomain->Type));
    wRow << createItem(decode_ZDomainTypeForDisplay(pDomain->Type));

//    wRow.last()->setFlags(Qt::ItemIsEnabled);
    wRow << createItemAligned(pDomain->Content,Qt::AlignLeft);
 //   wRow.last()->setFlags( Qt::ItemIsEnabled);

    if (!pDomain->ToolTip.isEmpty())
        wRow << FakeZFb->create() ;
    else
        wRow << new QStandardItem("");

    if (pDomain->isViewable()) {
        wRow << ViewZFb->create() ;
    }
    else
        wRow << new QStandardItem("");

    return wRow ;
} // ZGeneralParamsWNd::createDomainRow

void
ZGeneralParamsWNd::appendDomainRow(ZDomainPath* pDomain,QStandardItem* pItem)
{
    QList<QStandardItem*> wRow = createDomainRow(pDomain);

    for (int wi=0; wi < pDomain->Inferiors.count() ; wi++) {
        appendDomainRow(pDomain->Inferiors[wi],wRow[0]);
    }
    pItem->appendRow(wRow);
} // ZGeneralParamsWNd::appendDomainRow

void
ZGeneralParamsWNd::changeDomainRow(ZDomainPath* pDomain,QStandardItem* pFirstItemOfRowToChange)
{

}

void
ZGeneralParamsWNd::displayDomains()
{

    DomainTRv->model()->removeRows(0,DomainTRv->model()->rowCount());

    QList<QStandardItem*> wRow;

    if (MainDomainICn.isNull()) {
//        MainDomainICn=ContentObjectBroker.iconFactory("general.iconfactory/rightArrow.png",ErrorLog);
//        DomainICn=ContentObjectBroker.iconFactory("general.iconfactory/curvedarrowdownright.gif",ErrorLog);
        MainDomainICn=ContentObjectBroker.iconFactory("general.iconfactory.rightarrow",ErrorLog);
        DomainICn=ContentObjectBroker.iconFactory("general.iconfactory.curvedarrowdownright",ErrorLog);
    }


    for (int wi=0; wi < WorkDomainBroker.getRoot()->Inferiors.count() ; wi++) {
        appendDomainRow(WorkDomainBroker.getRoot()->Inferiors[wi],DomainTRv->ItemModel->invisibleRootItem());
    }


    for (int wi=0; wi < DomainTRv->ItemModel->columnCount();wi++)
        DomainTRv->resizeColumnToContents(wi);
    return ;
}

void
ZGeneralParamsWNd::Quit()
{
    if (HasChanged) {
        utf8VaryingString wSaveMsg;
        wSaveMsg.addsprintf("\n\nParameters have changed.\n"
                            "Do you really want to ignore changes made <Ignore>?");
        int wRet=ZExceptionDLg::adhocMessage2B("Save parameters",Severity_Question, "OOOps","Ignore",
                                                 nullptr,nullptr,wSaveMsg.toCChar());
        if (wRet==QDialog::Rejected) {
            return ;
        }
    }
    this->deleteLater();
    return;
}
void
ZGeneralParamsWNd::update()
{
    WorkBaseParams.setVerbose(fromScreenVerbose());

    ZStatus wSt;

    QString wParamsBase = __GENERAL_PARAMETERS_FILE__;
    uriString wDir = WorkParams.getParamDirectory();

    if (!WorkParams.currentXml.isEmpty()) {
        wDir = WorkParams.currentXml;
        wParamsBase = WorkParams.currentXml.getBasename().toCChar();
    }
    QString wFileName = QFileDialog::getSaveFileName(this, tr("Xml parameter file"),
                                                     wDir.toCChar(),
                                                     "xml files (*.xml);;All (*.*)",
                                                     &wParamsBase);
    if (wFileName.isEmpty())
        return;
    uriString wXmlParams= wFileName.toUtf8().data();
    uriString wBck;
    if (wXmlParams.exists()) {
        wBck=wXmlParams.getBckName(wXmlParams,"bck");
        wSt=wXmlParams.rename(wBck);
    }
    ErrorLog->setAutoPrintAll();
    wSt=XmlSaveAllParameters(wXmlParams,WorkParams,WorkDomainBroker,ErrorLog);

    if (wSt!=ZS_SUCCESS) {
        ZExceptionDLg::adhocMessageErrorLog("Save parameters",Severity_Error,ErrorLog,"Error saving xml parameter file %s status <%s>",
                                            wXmlParams.toString(),decode_ZStatus(wSt));
        return;
    }

    utf8VaryingString wSaveMsg ;
    wSaveMsg.sprintf("Saved parameters as %s\n",wXmlParams.toString());
    if (!wBck.isEmpty()) {
        wSaveMsg.addsprintf("\nPrevious parameters version saved as %s",wBck.toString());
    }
    wSaveMsg.addsprintf("\n\nMake these parameters current <Make current>",wBck.toString());
    int wRet=ZExceptionDLg::adhocMessage2B("Save parameters",Severity_Question, "No","Make current",
                                nullptr,nullptr,wSaveMsg.toCChar());

    if (wRet==QDialog::Rejected) {
        return;
    }

    HasChanged = false;

    GeneralParameters._copyFrom(WorkParams);
    DomainBroker._copyFrom(WorkDomainBroker);


    return;
} // update


void
ZGeneralParamsWNd::searchXmlFileClicked ()
{
    ZGeneralParameters wGP;
    ZDomainBroker wDB;
    uriString wDir = GeneralParameters.getParamDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, tr("Xml parameters file"),
                                                     wDir.toCChar(),
                                                     "xml files (*.xml);;All (*.*)");
    if (wFileName.isEmpty())
        return;
    uriString wXmlParams= wFileName.toUtf8().data();
    if (!wXmlParams.exists())  {
        ZException.setMessage("ZGeneralParamsDLg::searchXmlFileClicked",ZS_FILENOTEXIST,Severity_Error,"Parameter file <%s> has not been found.",wXmlParams.toCChar());
        if (ErrorLog!=nullptr) {
            ErrorLog->errorLog("ZGeneralParamsDLg::searchXmlFileClicked-E-FILNFND Parameter file <%s> has not been found.",wXmlParams.toCChar());
        }
        return ;
    }

    utf8VaryingString wXmlString;

    ZStatus wSt;
    if (ErrorLog!=nullptr)
        ErrorLog->setAutoPrintOn(ZAIES_Text);

    if ((wSt=wXmlParams.loadUtf8(wXmlString))!=ZS_SUCCESS) {
        if (ErrorLog!=nullptr) {
            ErrorLog->logZExceptionLast();
        }
        return ;
    }

     wSt=wGP.XmlLoadString(wXmlString,ErrorLog);
    if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp = ZException.last().formatFullUserMessage().toString();
        ZExceptionDLg::adhocMessage("Load parameters",Severity_Error,
                                    nullptr,&wExcp,"Error while parsing xml parameter (general parameters) file %s",wXmlParams.toString());
        return;
    }
    wSt=wDB.XmlLoadString(wXmlString,ErrorLog);
    if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp = ZException.last().formatFullUserMessage().toString();
        ZExceptionDLg::adhocMessage("Load parameters",Severity_Error,
                                    nullptr,&wExcp,"Error while parsing xml parameter (domains) file %s",wXmlParams.toString());
        return;
    }
    setup(wGP,wDB);
    return;
} // searchXmlFileClicked

bool InitFBS=true;
QStandardItem*
ZGeneralParamsWNd::fakeBTnsetup()
{
    QStandardItem* wButton=nullptr;
    static QPixmap wSearchPXm;
    static QIcon wSearchICn;
 //   QBrush wBrush0(Qt::blue);
//    QColor wLightBlue(30,136,229);

    QPalette wPalette=OkBTn->palette();
    QBrush wBrush0(wPalette.window());
    QBrush wBrush1(wPalette.windowText());
    /* /home/gerard/Development/zbasetools/zqt/icons/question.png */
    if (InitFBS) {
        InitFBS=false;
        if (!WorkParams.IconDirectory.isEmpty()) {
            uriString wPixURI="/home/gerard/Development/zbasetools/zqt/icons/";
 //           uriString wPixURI=WorkParams.IconDirectory;
            wPixURI.addConditionalDirectoryDelimiter() ;
            wPixURI += __SEARCH_ICON__;
            wSearchPXm.load(wPixURI.toCChar());
            wSearchICn.addPixmap(wSearchPXm);
            wButton = new QStandardItem(wSearchICn,"Search");
 //           wButton = new QStandardItem();
 //           QBrush wBrushIcn(wSearchPXm);
 //           wButton->setForeground(wBrushIcn);
        }
        else {
            wButton = new QStandardItem("Search");
        }
        wButton->setBackground(wBrush0);
        wButton->setForeground(wBrush1);
        wButton->setFlags( Qt::ItemIsEnabled);
        return wButton;
    }

    if (!WorkParams.IconDirectory.isEmpty()) {
        wButton = new QStandardItem(wSearchICn,"Search");
    }
    else {
        wButton = new QStandardItem("Search");
    }
    wButton->setBackground(wBrush0);
    wButton->setForeground(wBrush1);
    wButton->setFlags(Qt::ItemIsEnabled);
    return wButton;
} //fakeBTnsetup


QStandardItem*
ZGeneralParamsWNd::inactiveSetup()
{
    QStandardItem* wButton=nullptr;
    static QPixmap wSearchPXm;
    static QIcon wSearchICn;
    QBrush wBrush0(Qt::blue);
    QBrush wBrush1(Qt::white);

    /* /home/gerard/Development/zbasetools/zqt/icons/question.png */
    if (InitFBS) {
        InitFBS=false;
 /*       if (!WorkParams.IconDirectory.isEmpty()) {
            uriString wPixURI=WorkParams.IconDirectory;
            wPixURI.addConditionalDirectoryDelimiter() ;
            wPixURI += __SEARCH_ICON__;
            wSearchPXm.load(wPixURI.toCChar());
            wSearchICn.addPixmap(wSearchPXm);
//            wButton = new QStandardItem(wSearchICn,"");
            wButton = new QStandardItem();
            wButton->setData(QVariant(wSearchPXm), Qt::DecorationRole);
        }
        else { */
            wButton = new QStandardItem("Search");
            wButton->setBackground(wBrush0);
            wButton->setForeground(wBrush1);
 //       }
        wButton->setFlags( Qt::ItemIsEnabled);
        return wButton;
    }

    if (!WorkParams.IconDirectory.isEmpty()) {
//        wButton = new QStandardItem(wSearchICn,"");
        wButton = new QStandardItem();
        wButton->setData(QVariant(wSearchPXm), Qt::DecorationRole);
    }
    else {
        wButton = new QStandardItem("Search");
        wButton->setBackground(wBrush0);
        wButton->setForeground(wBrush1);
    }
    wButton->setFlags(Qt::NoItemFlags);
    return wButton;
} //fakeBTnsetup
ZStatus
ZGeneralParamsWNd::setup(ZGeneralParameters &pGeneralParameters, ZDomainBroker &pDomainBroker) {

    WorkParams = pGeneralParameters;

    WorkDomainBroker = pDomainBroker;

    XmlParamsLBl->setText(WorkParams.currentXml.toCChar());

    QList<QStandardItem*> wRow;

    setQtItemAlignment( Qt::AlignRight);

    wRow.clear();
    wRow << createItem("Working directory");
    wRow.last()->setFlags(Qt::ItemIsEnabled );
    wRow << createItem(WorkParams.getWorkDirectory());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    wRow << fakeBTnsetup() ;
    ParamsTBv->ItemModel->appendRow(wRow);

    wRow.clear();
    wRow << createItem("Parameters directory");
    wRow.last()->setFlags(Qt::ItemIsEnabled );
    wRow << createItem(WorkParams.getParamDirectory());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    wRow << fakeBTnsetup() ;
    ParamsTBv->ItemModel->appendRow(wRow);

    wRow.clear();
    wRow << createItem("Icon directory");
    wRow.last()->setFlags(Qt::ItemIsEnabled );
    wRow << createItem(WorkParams.getIconDirectory());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    wRow << fakeBTnsetup() ;
    ParamsTBv->ItemModel->appendRow(wRow);

    wRow.clear();
    wRow << createItem("Fixed size font");
    wRow.last()->setFlags(Qt::ItemIsEnabled );
    wRow << createItem(WorkParams.getFixedFont());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    wRow << fakeBTnsetup() ;
    ParamsTBv->ItemModel->appendRow(wRow);

    ParamsTBv->resizeColumnsToContents();
    ParamsTBv->resizeRowsToContents();

    displayVerbose(WorkParams.getVerbose());

    displayDomains();
    return ZS_SUCCESS;
} // dataSetup



void imageViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    QWidget* ViewerWNd = new QWidget(pFather,Qt::ToolTip);

    QVBoxLayout* QVB=new QVBoxLayout;
    ViewerWNd->setLayout(QVB);
    ZDomainObject wObject ;
    wObject.constructFromDP(wDP,pErrorLog);

    QLabel* wPixImageLBl = new QLabel(ViewerWNd);
    QVB->addWidget(wPixImageLBl);

    //            ZQLabel* wZQL=ContentObjectBroker.labelFactory("general.iconfactory/check.gif",ErrorLog);

    QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory.iconcheck",pErrorLog);
    QPushButton* PBClose=new QPushButton(wCheck,"",ViewerWNd);
    PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QVB->addWidget(PBClose,Qt::AlignRight);
    QObject::connect(PBClose,&QPushButton::clicked,ViewerWNd,&QWidget::close);
    //            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);
    ViewerWNd->move(QCursor::pos());
    QImage wImg (wObject.getFullPath(pErrorLog).toCChar());
    wPixImageLBl->setPixmap(QPixmap::fromImage(wImg));
    ViewerWNd->show();
}
void textViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    QWidget* ViewerWNd = new QWidget(pFather,Qt::ToolTip);

    QVBoxLayout* QVB=new QVBoxLayout;
    ViewerWNd->setLayout(QVB);
    ZDomainObject wObject ;
    wObject.constructFromDP(wDP,pErrorLog);

    QTextEdit* wTextTEd = new QTextEdit(ViewerWNd);
    QVB->addWidget(wTextTEd);


    QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory.iconcheck",pErrorLog);
    QPushButton* PBClose=new QPushButton(wCheck,"",ViewerWNd);
    PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QVB->addWidget(PBClose,Qt::AlignRight);
    QObject::connect(PBClose,&QPushButton::clicked,ViewerWNd,&QWidget::close);
    //            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);
    ViewerWNd->move(QCursor::pos());
    utf8VaryingString wTextContent;
    wObject.loadUtf8(wTextContent,pErrorLog);

    if (wTextContent.isEmpty()) {
        QTextCursor wCursor = wTextTEd->textCursor();
        QTextCharFormat wFmt = wCursor.charFormat();
        wFmt.setFontPointSize(24.0);
        wCursor.setCharFormat(wFmt);
        wTextTEd->setTextCursor(wCursor);
        wTextTEd->setText("\n\nNo text");
    }
    else
        wTextTEd->setText(wTextContent.toCChar());
    ViewerWNd->show();
}
void htmlViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    QWidget* ViewerWNd = new QWidget(pFather,Qt::ToolTip);

    QVBoxLayout* QVB=new QVBoxLayout;
    ViewerWNd->setLayout(QVB);
    ZDomainObject wObject ;
    wObject.constructFromDP(wDP,pErrorLog);

    QTextEdit* wTextTEd = new QTextEdit(ViewerWNd);
    QVB->addWidget(wTextTEd);


    QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory.iconcheck",pErrorLog);
    QPushButton* PBClose=new QPushButton(wCheck,"",ViewerWNd);
    PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QVB->addWidget(PBClose,Qt::AlignRight);
    QObject::connect(PBClose,&QPushButton::clicked,ViewerWNd,&QWidget::close);
    //            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);
    ViewerWNd->move(QCursor::pos());
    utf8VaryingString wTextContent;
    wObject.loadUtf8(wTextContent,pErrorLog);
    if (wTextContent.isEmpty()) {
        QTextCursor wCursor = wTextTEd->textCursor();
        QTextCharFormat wFmt = wCursor.charFormat();
        wFmt.setFontPointSize(24.0);
        wCursor.setCharFormat(wFmt);
        wTextTEd->setTextCursor(wCursor);
        wTextTEd->setText("\n\nNo text");
    }
    else
        wTextTEd->setHtml(wTextContent.toCChar());
    ViewerWNd->show();
}
bool builtinViewer (ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    ZDomain_type wType = wDP->Type & ZDOM_ObjectMask;
    switch (wType)
    {
    case ZDOM_Icon:
    case ZDOM_Image:
        imageViewer(wDP,pFather,pErrorLog);
        return true;
    case ZDOM_HtmlFile:
        textViewer(wDP,pFather,pErrorLog);
        return true;
    case ZDOM_TextFile:
        textViewer(wDP,pFather,pErrorLog);
        return true;
    case ZDOM_PdfFile:
        textViewer(wDP,pFather,pErrorLog);
        return true;
    default:
    {
        ZStatus wSt = externalViewer(wDP,pFather,pErrorLog);
        return wSt==ZS_SUCCESS;
    }
    }// switch
}
/* launch appropriate viewer for domain object pointed by path wDP
 * viewer is selected according wDP Type
 */
ZStatus
externalViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    ZDomain_type wObjTyp=wDP->Type & ZDOM_ObjectMask;

    switch (wObjTyp)
    {
    case ZDOM_Icon:
    case ZDOM_Image:
    case ZDOM_TextFile:
    case ZDOM_HtmlFile:
        pErrorLog->errorLog("externalViewer-E-INVTYP Invalid object type <%s>",decode_ZDomain_type(wObjTyp).toCChar());
        return ZS_INVTYPE;
    default:
        break;
    }// switch


    ZDomainObject wViewer=ContentObjectBroker.getViewerFromType(wDP->Type,pErrorLog);
    if (wViewer.isNull())
        return ZS_NOTFOUND;

    ZDomainObject wToDisplay;
    wToDisplay.constructFromDP(wDP,pErrorLog);

    uriString wUToDisplay = wToDisplay.getFullPath(pErrorLog);

/*
    QProcess P(&w);
    QString programPath;
    programPath=
        "/home/erfan/Documents/Qt/test1-build-desktop- Qt_4_8_1_in_PATH__System__Release/test1";
    P.start(programPath);
  */
    QProcess *wProcess = new QProcess(pFather);
//    QString program = "dolphin";
    uriString wURIViewer = wViewer.getFullPath(pErrorLog);
//    wProcess->start(wViewer.getFullPath(pErrorLog).toCChar(),
    wProcess->start(wURIViewer.toCChar(),
                    QStringList() << wUToDisplay.toCChar());
    return ZS_SUCCESS;
}



void
ZGeneralParamsWNd::DomainTRvClicked (const QModelIndex &pIndex)
{
//    if (pIndex.column() != 2)
//        return;

    if (pIndex.column() == ColumnToolTip) {
        QModelIndex wIndex=pIndex.sibling(pIndex.row(),0);
        QVariant wV=wIndex.data(ZQtValueRole);
        ZDomainPath* wDP = wV.value<ZDomainPath*>();
//        if (wDP==nullptr)
//            return;
        if (wDP->ToolTip.isEmpty())
            return;

        if (ToolTipWDg==nullptr) {
            ToolTipWDg = new QWidget(nullptr,Qt::ToolTip);
            QVBoxLayout* QVB=new QVBoxLayout;
            ToolTipWDg->setLayout(QVB);
            ToolTipTEd= new QTextEdit(ToolTipWDg);
            QVB->addWidget(ToolTipTEd);
            ToolTipTEd->setEnabled(false);

//            ZQLabel* wZQL=ContentObjectBroker.labelFactory("general.iconfactory/check.gif",ErrorLog);

            QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory/check50.png",ErrorLog);
            QPushButton* PBClose=new QPushButton(wCheck,"",ToolTipWDg);
            PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

            QVB->addWidget(PBClose,Qt::AlignRight);
            QObject::connect(PBClose,&QPushButton::clicked,ToolTipWDg,&QWidget::hide);

        }
        ToolTipWDg->move(GlobalMousePosition);
        ToolTipTEd->clear();
        ToolTipTEd->setText(wDP->ToolTip.toCChar());
        ToolTipWDg->show();
        return ;
    } // ColumnToolTip

    if (pIndex.column() == ColumnView) {
        QModelIndex wIndex=pIndex.sibling(pIndex.row(),0);
        QVariant wV=wIndex.data(ZQtValueRole);
        ZDomainPath* wDP = wV.value<ZDomainPath*>();
        //        if (wDP==nullptr)
        //            return;
//        if (!wDP->isViewable())
//            return;

        if (wDP->hasViewer()) {
//        if (wDP->Type & ZDOM_HasViewer) {
            externalViewer(wDP,this,ErrorLog);
            return;
        }
        imageViewer(wDP,this,ErrorLog);
        return;
            ViewerWNd = new QWidget(this,Qt::ToolTip);

            QVBoxLayout* QVB=new QVBoxLayout;
            ViewerWNd->setLayout(QVB);
            ZDomainObject wObject ;
            wObject.constructFromDP(wDP,ErrorLog);

            QLabel* wPixImageLBl = new QLabel(ViewerWNd);
            QVB->addWidget(wPixImageLBl);

            //            ZQLabel* wZQL=ContentObjectBroker.labelFactory("general.iconfactory/check.gif",ErrorLog);

            QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory/check50.png",ErrorLog);
            QPushButton* PBClose=new QPushButton(wCheck,"",ToolTipWDg);
            PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            //            QHBoxLayout* QHB = new QHBoxLayout;
            //            QHB->addWidget(PBClose,Qt::AlignRight);
            //            QVB->addLayout(QHB);
            QVB->addWidget(PBClose,Qt::AlignRight);
            QObject::connect(PBClose,&QPushButton::clicked,ViewerWNd,&QWidget::close);
            //            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);
 //           QPoint wGlobalPosition = QCursor::pos();
            ViewerWNd->move(QCursor::pos());
            QImage wImg (wObject.getFullPath(ErrorLog).toCChar());
            wPixImageLBl->setPixmap(QPixmap::fromImage(wImg));
            ViewerWNd->show();

        return ;
    } // ColumnView
    if (pIndex.column() == ColumnHierarchy) {
        QModelIndex wIndex=pIndex.sibling(pIndex.row(),0);
        QVariant wV=wIndex.data(ZQtValueRole);
        ZDomainPath* wDP = wV.value<ZDomainPath*>();
        //        if (wDP==nullptr)
        //            return;

        QWidget* wHierarchyWDg = new QWidget(this,Qt::ToolTip);

        QVBoxLayout* QVB=new QVBoxLayout;
        wHierarchyWDg->setLayout(QVB);

        QTextEdit* wHierTEd= new QTextEdit(wDP->getHierarchy().toCChar(),wHierarchyWDg);
        wHierTEd->setEnabled(false);
        QVB->addWidget(wHierTEd);

        QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory/check50.png",ErrorLog);
        QPushButton* PBClose=new QPushButton(wCheck,"",wHierarchyWDg);
        PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        //            QHBoxLayout* QHB = new QHBoxLayout;
        //            QHB->addWidget(PBClose,Qt::AlignRight);
        //            QVB->addLayout(QHB);
        QVB->addWidget(PBClose,Qt::AlignRight);
        QObject::connect(PBClose,&QPushButton::clicked,wHierarchyWDg,&QWidget::close);
        wHierarchyWDg->move(GlobalMousePosition);
        wHierarchyWDg->show();

        return ;
    } // ColumnHierarchy
    return;

} // ZGeneralParamsWNd::DomainTRvClicked


QIcon MainDomainICn;
QIcon DomainICn;

ZFakeBTn           ToolTipZFb;
ZFakeBTn           ViewZFb;
ZFakeBTn           HierarchyZFb;

QList<QStandardItem*>
createZDomainSingleRow(ZDomainPath* pDomain,ZaiErrors* pErrorLog)
{
    QList<QStandardItem*> wRow;

    if (MainDomainICn.isNull()) {
        MainDomainICn=ContentObjectBroker.iconFactory("general.iconfactory.rightarrow",pErrorLog);
        DomainICn=ContentObjectBroker.iconFactory("general.iconfactory.curvedarrowdownright",pErrorLog);
    }
    if (ToolTipZFb.isNull()) {
        ToolTipZFb.setup("general.iconfactory.iconlight","Tooltip",pErrorLog, ZFakeBTn::HasBackground );
    }
    if (ViewZFb.isNull()) {
        ViewZFb.setup("general.iconfactory.iconview","",pErrorLog, ZFakeBTn::HasBackground|ZFakeBTn::NoText );
    }
    if (HierarchyZFb.isNull()) {
        HierarchyZFb.setup("general.iconfactory.iconhierarchy","",pErrorLog, ZFakeBTn::HasBackground|ZFakeBTn::NoText  );
    }

    wRow << createItemAligned(pDomain->Name,Qt::AlignLeft);
    if (pDomain->isFirstLevel())        /* root or direct root's child */
        wRow[0]->setIcon(MainDomainICn);
    else
        wRow[0]->setIcon(DomainICn);

    QVariant wV;
    wV.setValue(pDomain);
    wRow[0]->setData(wV,ZQtValueRole);

    wRow << HierarchyZFb.create();
    wRow << createItem(decode_ZDomainTypeForDisplay(pDomain->Type));
    wRow << createItemAligned(pDomain->Content,Qt::AlignLeft);
    //   wRow.last()->setFlags( Qt::ItemIsEnabled);

    if (!pDomain->ToolTip.isEmpty())
        wRow << ToolTipZFb.create() ;
    else
        wRow << new QStandardItem("");

    if (pDomain->isViewable()) {
        wRow << ViewZFb.create() ;
    }
    else
        wRow << new QStandardItem("");

    return wRow ;
} //createZDomainSingleRow


