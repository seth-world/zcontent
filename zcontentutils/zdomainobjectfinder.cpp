#include "zdomainobjectfinder.h"
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QActionGroup>

#include <QStatusBar>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QPushButton>
#include <QLabel>
#include <QComboBox>

#include <QTextEdit>
#include <QLineEdit>


#include <zcontent/zcontentcommon/zdomainbroker.h>
#include <zcontent/zcontentcommon/zcontentobjectbroker.h>

#include "zobjectfileselector.h"

#include <zqt/zqtwidget/zqtreeview.h>

#include <zqt/zqtwidget/zqtableview.h>

#include <zqt/zqtwidget/zfakebtn.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zqt/zqtwidget/zqstatusbar.h>

#include <zio/zdir.h>

#include <zcontent/zcontentcommon/zviewers.h>

namespace zbs {


ZDomainObjectFinder::ZDomainObjectFinder(ZaiErrors* pErrorLog, QWidget *parent)
    : QMainWindow{parent}
{
    ErrorLog = pErrorLog;
    initLayout();
}

void
ZDomainObjectFinder::initLayout() {

    StatusBar = new ZQStatusBar(this);
    setStatusBar(StatusBar);

    setWindowTitle(tr("Domain object finder"));
    resize(1050,550);
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QIcon wQuitICn = ContentObjectBroker.iconFactory("general.iconfactory/system-shutdown.png",ErrorLog);
    QuitQAc = new QAction(wQuitICn,"",menuBar);
    menuBar->addAction(QuitQAc);
    ActionGroupQAg = new QActionGroup(menuBar);
    ActionGroupQAg->addAction(QuitQAc);

    QObject::connect(ActionGroupQAg,&QActionGroup::triggered, this,&ZDomainObjectFinder::MenuTriggered);

    QVBoxLayout* MainQVL=new QVBoxLayout;
    setCentralWidget( new QWidget(this));
    centralWidget()->setLayout(MainQVL);

    QHBoxLayout* QHL=new QHBoxLayout;
    MainQVL->insertLayout(0,QHL);

    QPushButton* BackwardBTn = new QPushButton(this);
    BackwardBTn->setIcon(ContentObjectBroker.iconFactory("general.iconfactory.iconbackward",ErrorLog));
    BackwardBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    BackwardBTn->setToolTip("Level up");
    QHL->addWidget(BackwardBTn);
    QObject::connect(BackwardBTn, &QPushButton::clicked, this, &ZDomainObjectFinder::Backward);

    QLabel* wLB1 = new QLabel(tr("Object type to search"),this);
    QHL->addWidget(wLB1);
    wLB1->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QHL->addWidget(wLB1);
    QHL->addSpacing(2);

    ObjectTypeCBx = new QComboBox(this);

    ObjectTypeCBx->addItem(tr("Any"));

    for (int wi=0; TypeDecodeTable[wi].Type != ZDOM_End ;wi++){
        ObjectTypeCBx->addItem(TypeDecodeTable[wi].DisplayName.toCChar());
    }
    QHL->addWidget(ObjectTypeCBx);
    QObject::connect(ObjectTypeCBx, &QComboBox::currentIndexChanged, this, &ZDomainObjectFinder::ObjectTypeFilterChanged);

    QSizePolicy wSP = ObjectTypeCBx->sizePolicy();
    wSP.setHorizontalPolicy(QSizePolicy::Fixed);
    ObjectTypeCBx->setSizePolicy(wSP);

    CurrentPathLEd = new QLineEdit(this);
    MainQVL->addSpacing(2);
    MainQVL->addWidget(CurrentPathLEd);


    /* table view */

    QHBoxLayout* QHLTBv=new QHBoxLayout;
    MainQVL->insertLayout(-1,QHLTBv);

    ObjectTRv = new ZQTreeView(this);

    MainQVL->addWidget(ObjectTRv);

    MainQVL->addSpacing(2);

    QHBoxLayout* wHBSel = new QHBoxLayout;
    QLabel* wSelLBl = new QLabel("Selected",this);
    wSelLBl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    wHBSel->addWidget(wSelLBl);
    wHBSel->addSpacing(2);
    SelectedLEd = new QLineEdit(this);
    wHBSel->addWidget(SelectedLEd);
    MainQVL->addLayout(wHBSel);


    /* buttons */

    QHBoxLayout* wButtonBoxQHb=new QHBoxLayout;
    wButtonBoxQHb->setAlignment(Qt::AlignRight);
    MainQVL->insertLayout(-1,wButtonBoxQHb);

    QPushButton * CancelBTn = nullptr;
    QPushButton * OkBTn = nullptr;
    CancelBTn = new QPushButton("Quit");
    wButtonBoxQHb->addWidget(CancelBTn);
    OkBTn = new QPushButton("Select");
    wButtonBoxQHb->addWidget(OkBTn);

    QObject::connect(OkBTn, &QPushButton::clicked, this, &ZDomainObjectFinder::Validation);
    QObject::connect(CancelBTn, &QPushButton::clicked, this, &ZDomainObjectFinder::Quit);

    ObjectTRv->addFilterMask(ZEF_MouseFullTrap | ZEF_KeybdFullTrap);

    ObjectTRv->newModel(8);

    ObjectTRv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("Domain"));

    ObjectTRv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem(""));/* Hierarchy */
    ObjectTRv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem("Type"));
    ObjectTRv->ItemModel->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);

    ObjectTRv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem("Content/value"));
    ObjectTRv->ItemModel->setHorizontalHeaderItem(4,new QStandardItem(""));
    ObjectTRv->ItemModel->setHorizontalHeaderItem(5,new QStandardItem("View"));
    ObjectTRv->ItemModel->setHorizontalHeaderItem(6,new QStandardItem("Register\nNew"));
    ObjectTRv->ItemModel->setHorizontalHeaderItem(7,new QStandardItem(""));

    ObjectTRv->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ObjectTRv->setSelectionBehavior(QAbstractItemView::SelectRows);

    ObjectTRv->_register(std::bind(&ZDomainObjectFinder::ObjectTRvKeyFiltered, this,_1,_2),
                         std::bind(&ZDomainObjectFinder::ObjectTRvMouseFiltered, this,_1,_2),
                         std::bind(&ZDomainObjectFinder::ObjectTRvFlexMenu, this,_1),
                         nullptr);

    QObject::connect(ObjectTRv, &QTreeView::doubleClicked, this, &ZDomainObjectFinder::ObjectTRvDoubleClicked);
    QObject::connect(ObjectTRv, &QTreeView::clicked, this, &ZDomainObjectFinder::ObjectTRvClicked);
    QObject::connect(ObjectTRv,&QTreeView::expanded,this,&ZDomainObjectFinder::ObjectTRvExpanded);

    ToolTipZFb.setup ("general.iconfactory.iconlight","Tooltip",ErrorLog) ;
    HierarchyZFb.setup ("general.iconfactory.iconhierarchy",ErrorLog) ;
    ViewZFb.setup  ("general.iconfactory.iconview",ErrorLog) ;
    NewZFb.setup  ("general.iconfactory.iconnew",ErrorLog) ; /* applies only for path to create a child domain either object or sub path */

    ZFakeBTnDelegate*wID = new ZFakeBTnDelegate (ObjectTRv->ItemModel,this);
    ObjectTRv->setItemDelegateForColumn(ColumnHierarchy,wID);
    ObjectTRv->setItemDelegateForColumn(ColumnToolTip,wID);
    ObjectTRv->setItemDelegateForColumn(ColumnView,wID);
    ObjectTRv->setItemDelegateForColumn(ColumnEdit,wID);
} //initLayout

void
ZDomainObjectFinder::setup(ZDomainPath* pDP)
{
    setCurrent(pDP);
    displayDomains(pDP);
    return ;
}

void
ZDomainObjectFinder::Backward()
{
    if (CurrentDP==nullptr)
        return;
    if (!CurrentDP->hasFather())
        return;
    setup(CurrentDP->father());
}

void
ZDomainObjectFinder::MenuTriggered(QAction* pAction)
{
    if ((pAction==QuitQAc))
    {
        Quit();
        return;
    }
}

void ZDomainObjectFinder::ObjectTRvFlexMenu(QContextMenuEvent *event)
{
    if (DomainFlexMEn==nullptr)
        setupDomainFlexMenu();
    DomainFlexMEn->exec(event->globalPos());
}

QMenu *
ZDomainObjectFinder::setupDomainFlexMenu()
{
    DomainFlexMEn=new QMenu(ObjectTRv);

    DomainFlexMEn->setTitle("Domains");

    DomainActionGroupQAg=new QActionGroup(DomainFlexMEn) ;
    QObject::connect(DomainActionGroupQAg, &QActionGroup::triggered, this, &ZDomainObjectFinder::DomainActionEvent);
    /*
    CreateDomainQAc= new QAction("Add domain at same level<Insert>",DomainFlexMEn);
    DomainFlexMEn->addAction(CreateDomainQAc);
    DomainActionGroupQAg->addAction(CreateDomainQAc);
    */

    return DomainFlexMEn;
}//setupDomainFlexMenu

void
ZDomainObjectFinder::ObjectTypeFilterChanged (int pIndex)
{
    if (pIndex==0) {
        ObjectFilter = ZDOM_ObjectMask;
    }
    else {
        ObjectFilter = TypeDecodeTable[pIndex-1].Type ;
    }
    setup(CurrentDP);
}

void
ZDomainObjectFinder::Validation()
{
    emit (domainObjectSelected(SelectedDP));
    this->deleteLater();
    return;
}

void
ZDomainObjectFinder::Quit()
{
/*
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
*/
    this->deleteLater();
    return;
}


bool
ZDomainObjectFinder::ObjectTRvKeyFiltered(int pKey,QKeyEvent* pEvent)
{
    if(pKey == Qt::Key_Escape) {
        Quit();
        return true ;
    }

    int wKey = pEvent->key();
    if (pEvent->modifiers() &  Qt::ControlModifier) {
        if (wKey == Qt::Key_V) {
//            return domainPasteCurrent();
        }
        if ( pEvent->modifiers() &  Qt::ShiftModifier ) {
            if (wKey == Qt::Key_V) {
//                return domainPasteUpper();
            }
        }// ctr-shift ...
        if (wKey== Qt::Key_Up) {
//            return domainMoveUp();
        }
        if (wKey == Qt::Key_Down) {
//            return domainMoveDown();
        }
        if (wKey == Qt::Key_C) {
//            return domainCopy();
        }
        if (wKey == Qt::Key_V) {
//            return domainPasteCurrent();
        }
        if (wKey == Qt::Key_X) {
//            return domainCut();
        }
        if (wKey == Qt::Key_S) {
//            update();
            return true;
        }
    } // modifier == ControlModifier

    if(wKey == Qt::Key_Insert)
    {
//        return domainCreate();
    }
    if(wKey == Qt::Key_Delete)
    {
//        return domainRemove();
    }//Qt::Key_Delete


    //    DomainTRv->baseKeyPressEvent(pEvent);
    return true;
}//ZGeneralParamsWNd::KeyTReKeyFiltered

bool
ZDomainObjectFinder::ObjectTRvMouseFiltered(int pKey,QMouseEvent* pEvent)
{
    GlobalMousePosition = pEvent->globalPos();

    if (pKey==ZEF_DoubleClickLeft) {
//        return domainChange();
    }
    return false;
}

void
ZDomainObjectFinder::ObjectTRvExpanded()
{
    for (int wi=0; wi < ObjectTRv->ItemModel->columnCount();wi++)
        ObjectTRv->resizeColumnToContents(wi);
    return ;
}

void
ZDomainObjectFinder::setCurrent(ZDomainPath* pDP)
{
    if (pDP->getHierarchy().isEmpty()){
        CurrentPathLEd->setText("<root>");
    }
    else
        CurrentPathLEd->setText(pDP->getHierarchy().toCChar());
    CurrentDP = pDP;
}

void
ZDomainObjectFinder::ObjectTRvDoubleClicked (const QModelIndex &pIndex)
{
    QModelIndex wIndex=pIndex.sibling(pIndex.row(),0);
    QVariant wV=wIndex.data(ZQtValueRole);
    ZDomainPath* wDP = wV.value<ZDomainPath*>();



    setCurrent(wDP) ;
    displayDomains(wDP);
    return;

} // ZDomainObjectFinder::ObjectTRwClicked
void
ZDomainObjectFinder::ObjectTRvClicked (const QModelIndex &pIndex)
{
    QModelIndex wIndex=pIndex.sibling(pIndex.row(),0);
    QVariant wV=wIndex.data(ZQtValueRole);
    ZDomainPath* wDP = wV.value<ZDomainPath*>();

    if (wDP->isSelected(ObjectFilter)) {
        SelectedDP=wDP;
        SelectedLEd->setText(wDP->getHierarchy().toCChar());
    }


    if (pIndex.column() == ColumnToolTip) {
        if (wDP->ToolTip.isEmpty())
            return;

        QWidget* ToolTipWDg = new QWidget(nullptr,Qt::ToolTip);
        QVBoxLayout* QVB=new QVBoxLayout;
        ToolTipWDg->setLayout(QVB);
        QTextEdit* ToolTipTEd= new QTextEdit(ToolTipWDg);
        QVB->addWidget(ToolTipTEd);
        ToolTipTEd->setEnabled(false);

        QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory/check50.png",ErrorLog);
        QPushButton* PBClose=new QPushButton(wCheck,"",ToolTipWDg);
        PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

        QVB->addWidget(PBClose,Qt::AlignRight);
        QObject::connect(PBClose,&QPushButton::clicked,ToolTipWDg,&QWidget::hide);

        ToolTipWDg->move(GlobalMousePosition);
        ToolTipTEd->clear();
        ToolTipTEd->setText(wDP->ToolTip.toCChar());
        ToolTipWDg->show();
        return ;
    } // ColumnToolTip

    if (pIndex.column() == ColumnView) {
        if ( wDP->hasViewer()) {
 //           utf8VaryingString wS="Launching viewer for " + wDP->getHierarchy();
            StatusBar->showMessage("Launching viewer for " + wDP->getHierarchy());
            if (genericViewer(wDP,(QWidget*)this,ErrorLog)){
                StatusBar->clearMessage();
                return;
            }
            StatusBar->showMessage(ErrorLog->getLastMessage().toCChar());
        }

        return;
    } // ColumnView

    if (pIndex.column() == ColumnEdit) {
        QModelIndex wIndex=pIndex.sibling(pIndex.row(),0);
        QVariant wV=wIndex.data(ZQtValueRole);
        ZDomainPath* wDP = wV.value<ZDomainPath*>();
        utf8VaryingString wS="Launching editor for " + wDP->getHierarchy();
        StatusBar->showMessage(wS.toCChar());
        if (objectSelector(wDP,this,ErrorLog)){
            StatusBar->clearMessage();
            return;
        }
        StatusBar->showMessage(ErrorLog->getLastMessage().toCChar());
        return ;
    } // ColumnEdit

    if (pIndex.column() == ColumnHierarchy) {
        QWidget* wHierarchyWDg = new QWidget(this,Qt::ToolTip);

        QVBoxLayout* QVB=new QVBoxLayout;
        wHierarchyWDg->setLayout(QVB);

        QTextEdit* wHierTEd= new QTextEdit(wDP->getHierarchy().toCChar(),wHierarchyWDg);
        wHierTEd->setEnabled(false);
        QVB->addWidget(wHierTEd);

        QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory/check50.png",ErrorLog);
        QPushButton* PBClose=new QPushButton(wCheck,"",wHierarchyWDg);
        PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        QVB->addWidget(PBClose,Qt::AlignRight);
        QObject::connect(PBClose,&QPushButton::clicked,wHierarchyWDg,&QWidget::close);
        wHierarchyWDg->move(GlobalMousePosition);
        wHierarchyWDg->show();

        return ;
    } // ColumnHierarchy

    return;

} // ZDomainObjectFinder::ObjectTRvClicked


void
ZDomainObjectFinder::DomainActionEvent(QAction* pAction)
{
    /*
    if ((pAction==CreateDomainQAc)) {
        domainCreate();
        return;
    }
    */
}//ZDomainObjectFinder::DomainActionEvent


void
ZDomainObjectFinder::DomainTRwSelModeChanged(QAbstractItemView::SelectionMode pSelMode)
{
    if (pSelMode==QAbstractItemView::SingleSelection) {
        StatusBar->showMessage("Row selection set to single selection");
    }
    else
        StatusBar->showMessage("Row selection set to multiple selection");

} // ZDomainObjectFinder::DomainTRwSelModeChanged



void
ZDomainObjectFinder::appendDomainRow(ZDomainPath* pDomain,QStandardItem* pItem)
{
    //    QList<QStandardItem*> wRow = createDomainRow(pDomain);
    if (!pDomain->isRoot() && !(pDomain->isSelectedForDisplay(ObjectFilter)))
        return;
    QList<QStandardItem*> wRow = createDomainRow(pDomain, ErrorLog);

    for (int wi=0; wi < pDomain->childrenCount() ; wi++) {
            appendDomainRow(pDomain->child(wi),wRow[0]);
    }
    pItem->appendRow(wRow);
} // ZDomainObjectFinder::appendDomainRow



void
ZDomainObjectFinder::displayDomains(ZDomainPath* pDomain)
{

    ObjectTRv->model()->removeRows(0,ObjectTRv->model()->rowCount());

    QList<QStandardItem*> wRow;

    if (MainDomainICn.isNull()) {
        //        MainDomainICn=ContentObjectBroker.iconFactory("general.iconfactory/rightArrow.png",ErrorLog);
        //        DomainICn=ContentObjectBroker.iconFactory("general.iconfactory/curvedarrowdownright.gif",ErrorLog);
        MainDomainICn=ContentObjectBroker.iconFactory("general.iconfactory.iconrightarrow",ErrorLog);
        DomainICn=ContentObjectBroker.iconFactory("general.iconfactory.iconcurvedarrowdownright",ErrorLog);
    }


    for (int wi=0; wi < pDomain->childrenCount() ; wi++) {
        if (pDomain->isRoot() || pDomain->isSelectedForDisplay(ObjectFilter))
            appendDomainRow(pDomain->child(wi),ObjectTRv->ItemModel->invisibleRootItem());
    }


    for (int wi=0; wi < ObjectTRv->ItemModel->columnCount();wi++)
        ObjectTRv->resizeColumnToContents(wi);
    return ;
} // ZDomainObjectFinder::displayDomains


QList<QStandardItem*>
ZDomainObjectFinder::createDomainRow(ZDomainPath* pDomain,ZaiErrors* pErrorLog)
{
    QList<QStandardItem*> wRow;

    if (MainDomainICn.isNull()) {
        MainDomainICn=ContentObjectBroker.iconFactory("general.iconfactory.iconrightarrow",pErrorLog);
        DomainICn=ContentObjectBroker.iconFactory("general.iconfactory.iconcurvedarrowdownright",pErrorLog);
    }
    if (HierarchyZFb.isNull()) {
        HierarchyZFb.setup("general.iconfactory.iconhierarchy",pErrorLog );
    }
    if (ToolTipZFb.isNull()) {
        ToolTipZFb.setup("general.iconfactory.iconlight","Tooltip",pErrorLog);
    }
    if (ViewZFb.isNull()) {
        ViewZFb.setup("general.iconfactory.iconview",pErrorLog );
    }
    if (NewZFb.isNull()) {
        NewZFb.setup("general.iconfactory.iconnew",pErrorLog );
    }


    wRow << createItemAligned(pDomain->getName(),Qt::AlignLeft);
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

    if (!pDomain->ToolTip.isEmpty())
        wRow << ToolTipZFb.create() ;
    else
        wRow << new QStandardItem("");

    if (pDomain->isViewable() && pDomain->hasViewer()) {
        wRow << ViewZFb.create() ;
    }
    else
        wRow << new QStandardItem("");

//    if (pDomain->isTruePath() ) {
    if (pDomain->isPath() ) {
        wRow << NewZFb.create() ;
    }
    else
        wRow << new QStandardItem("");

    return wRow ;
} //createZDomainSingleRow


bool
ZDomainObjectFinder::objectSelector (ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    if (!wDP->isPath())
        return false;
/*
    switch (wDP->getObjectType())
    {
    case ZDOM_HtmlFile:

    }
*/
    ZObjectFileSelector* wOS = new ZObjectFileSelector(ObjectFilter, ErrorLog, this);

    QObject::connect(wOS,&ZObjectFileSelector::DomainsHaveChanged,this,&ZDomainObjectFinder::setup);

    if (!wOS->setup(wDP)) {
        StatusBar->showMessage("Cannot list directory components of path <%s>",wDP->constructFullPhysicalPath().toCChar());
        return false;
    }

    wOS->show();
    return true;
}



}// namespace zbs
