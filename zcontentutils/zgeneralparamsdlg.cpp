#include "zgeneralparamsdlg.h"

#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QGridLayout>
#include <QGroupBox>

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

#include <ztoolset/uristring.h>

#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory()

#include <zcontent/zcontentutils/zexceptiondlg.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <zqt/zqtwidget/zqtreeview.h>
#include <QTableView>


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

    /* backup dialog setup */

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

    QHBoxLayout* QHBLViewSwitch = new QHBoxLayout;
    SwitchViewBTn = new QPushButton("Show Verbose",this);
 //   SwitchViewBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QHBLViewSwitch->addWidget(SwitchViewBTn);
    MainQVL->insertLayout(-1,QHBLViewSwitch,Qt::AlignCenter);

    QObject::connect(SwitchViewBTn, &QPushButton::clicked, this, &ZGeneralParamsWNd::SwitchView);

    DomainsWDg = new QWidget(this);
    QVBoxLayout* QVLTRv=new QVBoxLayout;
    DomainTRv = new ZQTreeView(this);
    DomainTRv->addFilterMask(ZEF_SingleClick | ZEF_KeybdFullTrap);
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

    QObject::connect(DomainTRv, &QTreeView::clicked, this, &ZGeneralParamsWNd::DomainTRvClicked);

    FakeZFb = new ZFakeBTn ("general.iconfactory.icons.iconlight","Tooltip",ErrorLog, ZFakeBTn::HasBackground ) ;

    ViewZFb = new ZFakeBTn ("general.iconfactory.icons.iconview",ErrorLog) ;

    HierarchyZFb = new ZFakeBTn("general.iconfactory.icons.iconhierarchy",ErrorLog) ;


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

    DomainTRv->setSelectionMode(QAbstractItemView::SingleSelection);

    DomainTRv->_register(std::bind(&ZGeneralParamsWNd::DomainTRvKeyFiltered, this,_1,_2),
                         std::bind(&ZGeneralParamsWNd::DomainTRvMouseFiltered, this,_1,_2),
                         std::bind(&ZGeneralParamsWNd::DomainTRvFlexMenu, this,_1),
                         nullptr);

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
ZGeneralParamsWNd::DomainTRvKeyFiltered(int pKey,QKeyEvent* pEvent)
{
    if(pKey == Qt::Key_Escape) {
        Quit();
        return;
    }
    int wKey = pEvent->key();
    if (pEvent->modifiers() &  Qt::ControlModifier) {
        if ( pEvent->modifiers() &  Qt::ShiftModifier ) {
            if (wKey == Qt::Key_V) {
                domainPasteUpper();
                return;
            }
        }// ctr-shift ...
        if (wKey== Qt::Key_Up) {
            domainMoveUp();
            return;
        }
        if (wKey == Qt::Key_Down) {
            domainMoveDown();
        }
        if (wKey == Qt::Key_C) {
            domainCopy();
            return;
        }
        if (wKey == Qt::Key_V) {
            domainPasteCurrent();
            return;
        }
        if (wKey == Qt::Key_X) {
           domainCut();
            return;
        }
        if (wKey == Qt::Key_S) {
            update();
            return;
        }
    } // modifier == ControlModifier

    if(wKey == Qt::Key_Insert)
    {
        domainCreate();
        return;
    }
    if(wKey == Qt::Key_Delete)
    {
        domainRemove();
        return;
    }//Qt::Key_Delete

    DomainTRv->baseKeyPressEvent(pEvent);
    return;
}//ZGeneralParamsWNd::KeyTReKeyFiltered

void
ZGeneralParamsWNd::DomainTRvMouseFiltered(int pKey,QMouseEvent* pEvent)
{
    GlobalMousePosition = pEvent->globalPos();

    if (pKey==ZEF_DoubleClickLeft) {
        domainChange();
    }
}

void    ZGeneralParamsWNd::domainRemove()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;

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
} //ZGeneralParamsWNd::domainRemove

/* create a new domain at the same level of current in place (row) of current */
void    ZGeneralParamsWNd::domainCreate()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;

    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    ZDomainPath* wDPFather=nullptr;
    QStandardItem* wParentItem = DomainTRv->ItemModel->itemFromIndex( wRIdx.parent());
    ZDomainDLg* wDomDLg = new ZDomainDLg(this);
    if ((wParentItem==nullptr) || (wParentItem==DomainTRv->ItemModel->invisibleRootItem())) {
        wDPFather = WorkDomainBroker.DomainRoot;
    }
    else {
        QVariant wV = wParentItem->data(ZQtValueRole);
        if (wV.isNull()) {
            ErrorLog->errorLog("ZGeneralParamsWNd::domainCreate-E-NULL Cannot get item data from item<%s>",wParentItem->text().toUtf8().data());
            return;
        }
        wDPFather=wV.value<ZDomainPath*>();
    }
    wDomDLg->setupCreate(wDPFather,WorkDomainBroker.DomainRoot);
    int wRet = wDomDLg->exec();
    if (wRet==QDialog::Rejected)
        return;

    ZDomainPath* wDP=wDomDLg->getDomainPath();

    HasChanged=true;

    QList<QStandardItem*> wRow ;

    wRow << createItemAligned(wDP->Name,Qt::AlignLeft);
    if (wDP->Father==WorkDomainBroker.DomainRoot)
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

    if (wDP->Type & ZDOM_Viewable) {
        wRow << ViewZFb->create() ;
    }
    else
        wRow << new QStandardItem("");

    for (int wi=0; wi < wDP->Inferiors.count() ; wi++) {
        displayDomainRow(wDP->Inferiors[wi],wRow[0] , 1 );
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

} // ZGeneralParamsWNd::domainCreate

void    ZGeneralParamsWNd::domainInsert()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;


    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    ZDomainPath* wDPFather=nullptr;

    QStandardItem* wParentItem = DomainTRv->ItemModel->itemFromIndex( wRIdx);
    ZDomainDLg* wDomDLg = new ZDomainDLg(this);
    if ((wParentItem==nullptr) || (wParentItem==DomainTRv->ItemModel->invisibleRootItem())) {
        wDPFather = WorkDomainBroker.DomainRoot;
    }
    else {
        QVariant wV = wParentItem->data(ZQtValueRole);
        wDPFather=wV.value<ZDomainPath*>();
    }
    wDomDLg->setupCreate(wDPFather,WorkDomainBroker.DomainRoot);

    int wRet = wDomDLg->exec();
    if (wRet==QDialog::Rejected)
        return;

    HasChanged=true;

    ZDomainPath* wDP = wDomDLg->getDomainPath();
    wDP->Father = wDPFather;

    wDP->Father->Inferiors.push(wDP);

    QList<QStandardItem*> wRow ;

    wRow << createItemAligned(wDP->Name,Qt::AlignLeft);
    if (wDP->Father==WorkDomainBroker.DomainRoot)
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

    if (wDP->Type & ZDOM_Viewable) {
        wRow << ViewZFb->create() ;
    }
    else
        wRow << new QStandardItem("");


    for (int wi=0; wi < wDP->Inferiors.count() ; wi++) {
        utf8VaryingString wHierarchy = wDP->getHierarchy() ;
        displayDomainRow(wDP->Inferiors[wi], wRow[0] , 1 );
    }
    wParentItem->appendRow(wRow);

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

void
ZGeneralParamsWNd::domainChange()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;

    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    ZDomainDLg* wDomDLg = new ZDomainDLg(this);

    QVariant wV = wRIdx.data(ZQtValueRole);
    if (wV.isNull()) {
        ErrorLog->errorLog("ZGeneralParamsWNd::domainChange-E-INFRADATA Wrong infradata (ZQtValueRole) for tree view.");
        return;
    }
    ZDomainPath* wDP=wV.value<ZDomainPath*>();
    wDomDLg->setupChange(wDP,WorkDomainBroker.DomainRoot);


    int wRet = wDomDLg->exec();
    if (wRet==QDialog::Rejected)
        return;

    HasChanged=true;
    QStandardItem* wSI=DomainTRv->ItemModel->itemFromIndex( wRIdx );
    wSI->setText(wDP->Name.toCChar());

    wRIdx = wRIdx.siblingAtColumn(1);
    wSI=DomainTRv->ItemModel->itemFromIndex( wRIdx );
    wSI->setText(wDP->getHierarchy().toCChar());

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

    if (!wDP->Type & ZDOM_Viewable)
        wSI = ViewZFb->create() ;
    else
        wSI = new QStandardItem("");

    DomainTRv->ItemModel->setItem(wRIdx.row(),5,wSI);

    if (wDomDLg->NameHasChanged) { /* cascade update inferiors' hierarchy */
        hierarchyCascadeUpdate(wRIdx.siblingAtColumn(0));
    }



} // ZGeneralParamsWNd::domainChange

void    ZGeneralParamsWNd::domainPasteUpper()
{
    if ((QSIClipboard.isEmpty()) || (DPClipboard==nullptr))
        return;
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;
    QStandardItem* wFather=nullptr;

    QModelIndex wRIdx=wIdx.siblingAtColumn(0);
    wRIdx = wRIdx.parent();
    if (!wRIdx.isValid()) {
        wFather = DomainTRv->ItemModel->invisibleRootItem();
    }
    else {
        wFather = DomainTRv->ItemModel->itemFromIndex(wRIdx);
    }

    QVariant wV = wFather->data(ZQtValueRole);
    ZDomainPath* wDPFather= wV.value<ZDomainPath*>();

    DPClipboard->Father = wDPFather;

    wDPFather->Inferiors.insert(DPClipboard,wIdx.row());
    wFather->insertRow(wIdx.row(),QSIClipboard);

    QSIClipboard.clear();
    DPClipboard = nullptr;
}// ZGeneralParamsWNd::domainPasteUpper

void    ZGeneralParamsWNd::domainPasteCurrent()
{
    if ((QSIClipboard.isEmpty()) || (DPClipboard==nullptr))
        return;
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;
    QStandardItem* wFather=nullptr;

    QModelIndex wRIdx=wIdx.siblingAtColumn(0);
    if (!wRIdx.isValid()) {
        wFather = DomainTRv->ItemModel->invisibleRootItem();
    }
    else {
        wFather = DomainTRv->ItemModel->itemFromIndex(wRIdx);
    }

    QVariant wV = wFather->data(ZQtValueRole);
    ZDomainPath* wDPFather= wV.value<ZDomainPath*>();

    DPClipboard->Father = wDPFather;

    wDPFather->Inferiors.insert(DPClipboard,wIdx.row());
    wFather->insertRow(wIdx.row(),QSIClipboard);

    QSIClipboard.clear();
    DPClipboard = nullptr;
}// ZGeneralParamsWNd::domainPaste


void    ZGeneralParamsWNd::domainCut()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;
    QStandardItem* wFather=nullptr;

    QModelIndex wRIdx=wIdx.siblingAtColumn(0);
    wRIdx = wRIdx.parent();
    if (!wRIdx.isValid()) {
        wFather = DomainTRv->ItemModel->invisibleRootItem();
    }
    else {
        wFather = DomainTRv->ItemModel->itemFromIndex(wRIdx);
    }

    QSIClipboard = wFather->takeRow(wIdx.row());
    QVariant wV = QSIClipboard[0]->data(ZQtValueRole);

    DPClipboard =  wV.value<ZDomainPath*>();

    DPClipboard->Father->removeInferior(DPClipboard);
    DPClipboard->Father = nullptr;
    return ;
}//ZGeneralParamsWNd::domainCut


QList<QStandardItem*>
ZGeneralParamsWNd::_copyDomainRow(QStandardItem* pFirstItem,ZDomainPath* pNewDP)
{
    QList<QStandardItem*> wRow;
    QStandardItem* wNewItem = new QStandardItem(pFirstItem->text());
    wNewItem->setTextAlignment(pFirstItem->textAlignment());
    wRow << wNewItem;
    QVariant wV;
    wV.setValue(pNewDP);
    wRow[0]->setData(wV,ZQtValueRole);
    QModelIndex wIdx =  pFirstItem->index();
    int wi = 1 ;
    while (wIdx.isValid()) {
        QStandardItem* wOldSI = DomainTRv->ItemModel->itemFromIndex(wIdx);
//        QStandardItem* wSI = new QStandardItem(wOldSI->text());
//        wSI->setTextAlignment(wOldSI->textAlignment());
        QStandardItem* wSI = wOldSI->clone();

        wRow << wSI ;
        wIdx=wIdx.siblingAtColumn(wi++);
    }
    return wRow;
} //ZGeneralParamsWNd::_copyDomainRow

void
ZGeneralParamsWNd::domainCopy()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;

    QModelIndex wRIdx=wIdx.siblingAtColumn(0);
    QStandardItem* wSI=DomainTRv->ItemModel->itemFromIndex(wRIdx);
    QVariant wV = wSI->data(ZQtValueRole);
    ZDomainPath* wDP = wV.value<ZDomainPath*>();
    DPClipboard = new ZDomainPath(*wDP);

    QSIClipboard = _copyDomainRow(wSI,DPClipboard);
    return ;
}


void    ZGeneralParamsWNd::domainMoveDown()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;
    int wRow = wIdx.row();


    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    QModelIndex wParentIdx = wRIdx.parent();
    //    if (wParentIdx.isValid())

    QStandardItem* wSIParent = DomainTRv->ItemModel->itemFromIndex(wParentIdx);
    if (wSIParent==nullptr)
        wSIParent=DomainTRv->ItemModel->invisibleRootItem();
    if (wRow==wSIParent->rowCount()-1)
        return;

    QVariant wV = wRIdx.data(ZQtValueRole);
    if (wV.isNull()) {
        ErrorLog->errorLog("ZGeneralParamsWNd::domainMoveDown-E-IVINFRA Invalid infra data at row %d.",wRow);
        return;
    }
    ZDomainPath* wDP = wV.value<ZDomainPath*>();

    wDP->Father->Inferiors.erase(wRow);
    QList<QStandardItem*> wRowSI = wSIParent->takeRow(wRow);
    if (wRow+1 == wDP->Father->Inferiors.count()) {
        wDP->Father->Inferiors.push(wDP);
        wSIParent->appendRow(wRowSI);
        return;
    }
    wSIParent->insertRow(wRow+1,wRowSI);
    wDP->Father->Inferiors.insert(wDP,wRow+1);
}// ZGeneralParamsWNd::domainMoveDown

void
ZGeneralParamsWNd::domainMoveUp()
{
    QModelIndex wIdx= DomainTRv->currentIndex();
    if(!wIdx.isValid())
        return;
    int wRow = wIdx.row();
    if (wRow==0)
        return;

    QModelIndex wRIdx = wIdx.siblingAtColumn(0);
    QModelIndex wParentIdx = wRIdx.parent();
//    if (wParentIdx.isValid())

    QStandardItem* wSIParent = DomainTRv->ItemModel->itemFromIndex(wParentIdx);
    if (wSIParent==nullptr)
        wSIParent=DomainTRv->ItemModel->invisibleRootItem();


    QVariant wV = wRIdx.data(ZQtValueRole);
    if (wV.isNull()) {
        ErrorLog->errorLog("ZGeneralParamsWNd::domainMoveUp-E-IVINFRA Invalid infra data at row %d.",wRow);
        return;
    }
    ZDomainPath* wDP = wV.value<ZDomainPath*>();

    wDP->Father->Inferiors.erase(wRow);
    wDP->Father->Inferiors.insert(wDP,wRow-1);

    QList<QStandardItem*> wRowSI = wSIParent->takeRow(wRow);
    wSIParent->insertRow(wRow-1,wRowSI);

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

    CopyDomainQAc= new QAction("Copy current <Ctrl-C>",DomainFlexMEn);
    DomainFlexMEn->addAction(CopyDomainQAc);
    DomainActionGroupQAg->addAction(CopyDomainQAc);

    CutDomainQAc= new QAction("Cut current<Ctrl-X>",DomainFlexMEn);
    DomainFlexMEn->addAction(CutDomainQAc);
    DomainActionGroupQAg->addAction(CutDomainQAc);

    PasteDomainQAc= new QAction("Paste to father<Ctrl-V>",DomainFlexMEn);
    DomainFlexMEn->addAction(PasteDomainQAc);
    DomainActionGroupQAg->addAction(PasteDomainQAc);

    DomainFlexMEn->addSeparator();

    RemoveDomainQAc= new QAction("Remove current<Delete>",DomainFlexMEn);
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
    if ((pAction==PasteDomainQAc)) {
        domainPasteUpper();
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

void
ZGeneralParamsWNd::displayDomainRow(ZDomainPath* pDomain,QStandardItem* pItem,int pLevel)
{
    QList<QStandardItem*> wRow;

    wRow << createItemAligned(pDomain->Name,Qt::AlignLeft);
    if (pLevel==0)
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

    if (pDomain->Type & ZDOM_Viewable) {
        wRow << ViewZFb->create() ;
    }
    else
        wRow << new QStandardItem("");

    for (int wi=0; wi < pDomain->Inferiors.count() ; wi++) {
        displayDomainRow(pDomain->Inferiors[wi],wRow[0],pLevel+1);
    }
    pItem->appendRow(wRow);
}

void
ZGeneralParamsWNd::displayDomains()
{

    DomainTRv->model()->removeRows(0,DomainTRv->model()->rowCount());

    QList<QStandardItem*> wRow;

    if (MainDomainICn.isNull()) {
//        MainDomainICn=ContentObjectBroker.iconFactory("general.iconfactory/rightArrow.png",ErrorLog);
//        DomainICn=ContentObjectBroker.iconFactory("general.iconfactory/curvedarrowdownright.gif",ErrorLog);
        MainDomainICn=ContentObjectBroker.iconFactory("general.iconfactory.icons.rightarrow",ErrorLog);
        DomainICn=ContentObjectBroker.iconFactory("general.iconfactory.icons.curvedarrowdownright",ErrorLog);
    }


    for (int wi=0; wi < WorkDomainBroker.DomainRoot->Inferiors.count() ; wi++) {
        displayDomainRow(WorkDomainBroker.DomainRoot->Inferiors[wi],DomainTRv->ItemModel->invisibleRootItem(),0);
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
        int wRet=ZExceptionDLg::adhocMessage2B("Save parameters",Severity_Question, "OOOps","Do ignore",
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
//            QHBoxLayout* QHB = new QHBoxLayout;
//            QHB->addWidget(PBClose,Qt::AlignRight);
//            QVB->addLayout(QHB);
            QVB->addWidget(PBClose,Qt::AlignRight);
            QObject::connect(PBClose,&QPushButton::clicked,ToolTipWDg,&QWidget::hide);
//            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);

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
        if (!(wDP->Type & ZDOM_Viewable))
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
            ViewerWNd->move(GlobalMousePosition);
            QImage wImg (wObject.getPath(ErrorLog).toCChar());
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
