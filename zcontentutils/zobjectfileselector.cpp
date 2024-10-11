#include "zobjectfileselector.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QHeaderView>
#include <QRadioButton>
#include <QFrame>
#include <QCheckBox>


#include <qevent.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <zqt/zqtwidget/zqstatusbar.h>

#include <zcontent/zcontentcommon/zdomainpath.h>
#include <zcontent/zcontentcommon/zcontentobjectbroker.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <ztoolset/zaierrors.h>

#include "zdomaindlg.h"

#include <zcontent/zcontentcommon/zviewers.h>
#include "zexceptiondlg.h"

using namespace zbs;


ZObjectFileSelector::ZObjectFileSelector(ZDomain_type pObjectFilter, ZaiErrors *pErrorLog, QWidget *pFather)
    : QMainWindow{pFather}
{
    ErrorLog = pErrorLog;
    ObjectFilter=pObjectFilter;
    initLayout();
}

void
ZObjectFileSelector::initLayout()
{
    setWindowTitle("Object file selector");
    setWindowFlag(Qt::Tool);
    QWidget* wCentralWdg = new QWidget(this);
    setCentralWidget(wCentralWdg);
    resize(1000,500);

    StatusBar=new ZQStatusBar(this);
    setStatusBar(StatusBar);

    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    GeneralActionGroupQAg= new QActionGroup(menuBar);
    QuitQAc = new QAction(this);
    QuitQAc->setIcon(ContentObjectBroker.iconFactory("general.iconfactory.iconquit",ErrorLog));
    menuBar->addAction(QuitQAc);
    GeneralActionGroupQAg->addAction(QuitQAc);
    QObject::connect(GeneralActionGroupQAg, &QActionGroup::triggered, this, &ZObjectFileSelector::DomainActionEvent);

    QVBoxLayout* wVBLMain = new QVBoxLayout;
    wCentralWdg->setLayout(wVBLMain);


    QHBoxLayout* wHBL1 = new QHBoxLayout;
    wHBL1->addWidget(new QLabel("Domain chain"));
    wHBL1->addSpacing(2);

    DomainChainLBl = new QLabel(this);
    wHBL1->addWidget(DomainChainLBl);
    wHBL1->addSpacing(2);

    LocalRemoteLBl = new QLabel(this);
    wHBL1->addWidget(LocalRemoteLBl);
    wHBL1->addSpacing(2);

    CurrentPathLBl = new QLabel(this);
    CurrentPathLBl->setAlignment(Qt::AlignRight);
    wHBL1->addWidget(CurrentPathLBl);
    wVBLMain->addLayout(wHBL1);

    QHBoxLayout* wHBL10 = new QHBoxLayout;
    wHBL10->addWidget(new QLabel("Filter",this));
    wHBL10->addSpacing(2);

    ObjectFilterLBl = new QLabel(this);
    ObjectFilterLBl->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    ObjectFilterLBl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    wHBL10->addWidget(ObjectFilterLBl);


    wHBL10->addSpacing(2);
    wHBL10->addWidget(new QLabel("Selected file extensions",this));
    wHBL10->addSpacing(2);

    FileSelection = inducePossibleType(ObjectFilter);

    _DBGPRINT(FileSelection.dump().toCChar())

    FileSelectionLBl = new QLabel(FileSelection.display().toCChar());
    FileSelectionLBl->adjustSize();
    FileSelectionLBl->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    FileSelectionLBl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    wHBL10->addWidget(FileSelectionLBl);

    wHBL10->addSpacing(2);

    CustomSelectionCBx = new QCheckBox("Custom selection",this);
    wHBL10->addWidget(CustomSelectionCBx);
    QObject::connect(CustomSelectionCBx,&QCheckBox::checkStateChanged,this,&ZObjectFileSelector::CustomSelectionChanged);

    SelectionFRm = new QFrame(this);
    SelectionFRm->setVisible(false);
    SelectionFRm->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    wHBL10->addWidget(SelectionFRm);

    QHBoxLayout* wSelHBL = new QHBoxLayout;
    SelectionFRm->setLayout(wSelHBL);

    CustomSelLEd= new QLineEdit(this);
    wSelHBL->addWidget(CustomSelLEd);

    QPushButton* wSelBTn=new QPushButton("Apply selection",this);
    wSelHBL->addWidget(wSelBTn);
    QObject::connect(wSelBTn,&QPushButton::clicked,this,&ZObjectFileSelector::ApplyCustomSelection);


 //   wHBL10->addStretch();

    wVBLMain->addLayout(wHBL10);
    wVBLMain->addSpacing(2);

//    QHBoxLayout* wHBLTBL=new QHBoxLayout;
    ObjectListTBv = new ZQTableView(this);
//    wVBLMain->addLayout(wHBLTBL);
//    wHBLTBL->addWidget(ObjectListTBv);
    wVBLMain->addWidget(ObjectListTBv);

 //   ObjectListTBv->setMouseClickCallback(std::bind(&ZObjectFileSelector::ObjectTRvMouseFiltered, this,placeholders::_1,placeholders::_2));

    QHBoxLayout* wHBL2=new QHBoxLayout;
//    wHBL2->addStretch();


    SelectedLEd = new QLineEdit(this);
//    SelectedLEd->setAlignment(Qt::AlignRight);
    SelectedLEd->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    wHBL2->addWidget(SelectedLEd);
    wHBL2->addSpacing(2);

    QPushButton* wCancel=new QPushButton("Quit",this);
    wCancel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    wHBL2->addWidget(wCancel);

    QPushButton* wOk=new QPushButton("Select",this);
    wOk->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    wHBL2->addWidget(wOk);
    wVBLMain->addLayout(wHBL2);


    QObject::connect(wCancel,&QPushButton::clicked,this,&ZObjectFileSelector::Quit);
    QObject::connect(wOk,&QPushButton::clicked,this,&ZObjectFileSelector::Validation);


    ObjectListTBv->newModel(6);

//    ObjectListTBv->ItemModel->setSortRole(ZSortRole);


    ObjectListTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("Name"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem("Type"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem("Size"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem("Creation"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(4,new QStandardItem("Modification"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(5,new QStandardItem("View"));

    ObjectListTBv->verticalHeader()->setVisible(false);

    ObjectListTBv->setSelectionMode(QAbstractItemView::SingleSelection);
    ObjectListTBv->setSelectionBehavior(QAbstractItemView::SelectRows);
/*
    ObjectListTBv->_registerCallBacks(std::bind(&ZObjectFileSelector::ObjectTRvKeyFiltered, this,placeholders::_1,placeholders::_2),
                         std::bind(&ZObjectFileSelector::ObjectTRvMouseFiltered, this,placeholders::_1,placeholders::_2),
                         std::bind(&ZObjectFileSelector::ObjectTRvFlexMenu, this,placeholders::_1),
                         nullptr);
*/
    ObjectListTBv->_registerCallBacks(std::bind(&ZObjectFileSelector::ObjectTRvKeyFiltered, this,placeholders::_1,placeholders::_2),
                                      nullptr,
                                      std::bind(&ZObjectFileSelector::ObjectTRvFlexMenu, this,placeholders::_1),
                                      nullptr);

    QObject::connect(ObjectListTBv,&QTableView::clicked,this,&ZObjectFileSelector::ObjectTRvClicked);
    QObject::connect(ObjectListTBv,&QTableView::doubleClicked,this,&ZObjectFileSelector::ObjectTRvDoubleClicked);

    ObjectListHeader = ObjectListTBv->horizontalHeader();
    QObject::connect(ObjectListHeader,&QHeaderView::sectionClicked,this,&ZObjectFileSelector::HeaderClicked);

    ViewZFb.setup  ("general.iconfactory.iconview",ErrorLog) ;
    ZFakeBTnDelegate*wID = new ZFakeBTnDelegate (ObjectListTBv->ItemModel,this);
    ObjectListTBv->setItemDelegateForColumn(ColumnView,wID);


}// ZObjectFileSelector::initLayout


bool
ZObjectFileSelector::setup(ZDomainPath *pFather)
{
    removeAllDirEntryRow();
    FatherDP = pFather ;
    uriString wURIPath ;
    utf8VaryingString wObject ;

    DomainChainLBl->setText(pFather->getHierarchy().toCChar());

    pFather->constructPath(wURIPath,wObject);

    CurrentPathLBl->setText(wURIPath.toCChar());

    if (pFather->isRemote())
        LocalRemoteLBl->setText("Remote");
    else
        LocalRemoteLBl->setText("Local");


    if (ObjectFilter == ZDOM_ObjectMask)
        ObjectFilterLBl->setText("Any object");
    else {
        bool wFound=false;
        for (int wi = 0; wi < TypeDecodeTable[wi].Type ; wi++) {
            if ( TypeDecodeTable[wi].Type == ObjectFilter) {
                wFound=true;
                ObjectFilterLBl->setText(TypeDecodeTable[wi].DisplayName.toCChar());
                break;
            }
        }
        if (!wFound) {
            ObjectFilterLBl->setText("Unknown filter mask");
        }
    }

    ZDir wDir;
    ZStatus wSt=wDir.setPath(wURIPath);
    if (wSt!=ZS_SUCCESS) {
        ErrorLog->errorLog("ZObjectFileSelector::setup Cannot dir <%s> status <%s>",
                           wURIPath.toCChar(),decode_ZStatus(wSt));
        this->deleteLater();
        return false;
    }

    DirMap wDirEntry ;

    wSt=wDir.fullDirSel(wDirEntry, FileSelection);
    while (wSt==ZS_SUCCESS) {
        if (!(wDirEntry.Type & ZDFT_Hidden)) {
        QList<QStandardItem*> wRow = createDirEntryRow(wDirEntry);
        ObjectListTBv->ItemModel->appendRow(wRow);
        }
        wSt=wDir.fullDirSel(wDirEntry, FileSelection);
    }

    for ( int wi = 0 ; wi < ObjectListTBv->ItemModel->columnCount() ; wi++) {
        ObjectListTBv->resizeColumnToContents(wi);
    }
    return true;
} // ZObjectFileSelector::setup



void
ZObjectFileSelector::Quit()
{
    this->deleteLater();
    return;
}

bool
ZObjectFileSelector::ObjectTRvKeyFiltered(int pKey,QKeyEvent* pEvent)
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
}//ZObjectFileSelector::KeyTReKeyFiltered

bool
ZObjectFileSelector::ObjectTRvMouseFiltered(int pKey,QMouseEvent* pEvent)
{
    GlobalMousePosition = pEvent->globalPos();

    if (pKey==ZEF_DoubleClickLeft) {
        //        return domainChange();
    }
    return false;
}

void ZObjectFileSelector::ObjectTRvFlexMenu(QContextMenuEvent *event)
{
    if (DomainFlexMEn==nullptr)
        setupDomainFlexMenu();
    DomainFlexMEn->exec(event->globalPos());
}

QMenu *
ZObjectFileSelector::setupDomainFlexMenu()
{
    DomainFlexMEn=new QMenu(this);

    DomainFlexMEn->setTitle("Domains");

    ActionGroupQAg=new QActionGroup(DomainFlexMEn) ;
    QObject::connect(ActionGroupQAg, &QActionGroup::triggered, this, &ZObjectFileSelector::DomainActionEvent);
    /*
    CreateDomainQAc= new QAction("Add domain at same level<Insert>",DomainFlexMEn);
    DomainFlexMEn->addAction(CreateDomainQAc);
    DomainActionGroupQAg->addAction(CreateDomainQAc);

*/

    return DomainFlexMEn;
}//setupDomainFlexMenu

void
ZObjectFileSelector::DomainActionEvent(QAction* pAction)
{
    if ((pAction==QuitQAc)) {
        Quit();
        return;
    }

}//ZObjectFileSelector::DomainActionEvent

void
ZObjectFileSelector::setCurrent(DirMap* pDM)
{
    SelectedDirEntry = *pDM;
    SelectedLEd->setText(pDM->Name.getBasename().toCChar());
}


void
ZObjectFileSelector::ObjectTRvDoubleClicked (const QModelIndex &pIndex)
{
    ObjectTRvClicked(pIndex);
    Validation();
    return;
} // ZDomainObjectFinder::ObjectTRvDoubleClicked

void
ZObjectFileSelector::ObjectTRvClicked (const QModelIndex &pIndex)
{
    QModelIndex wIndex=pIndex.sibling(pIndex.row(),0);
    QVariant wV=wIndex.data(ZQtValueRole);
    DirMap* wDM = wV.value<DirMap*>();    

    SelectedDirEntry._copyFrom(*wDM);
    SelectedLEd->setText(wDM->Name.toCChar());
//    SelectedLEd->adjustSize();

    if (pIndex.column() != ColumnView)
        return;

    if ((wDM->Type != ZDFT_Directory) && (wDM->Type != ZDFT_RegularFile)) {
        erroredViewer(this);
        return;
    }

    ZDomain_type wType = deducePossibleType(wDM->Name);

    ZDomainPath wDP(wType,FatherDP);

    ZStatus wSt=wDP.setName(wDM->Name.getBasename());
    if (wSt!=ZS_SUCCESS) {
        int wI=0;
        utf8VaryingString wName ;
        while( wSt != ZS_SUCCESS) {
            wName.sprintf("%s_%02d",wDM->Name.getBasename().toString(),++wI);
            wSt=wDP.setName(wName);
        }
        StatusBar->showMessage("Warning:: Domain name has been set to %s ",wName.toCChar());
    }


    wDP.Content = wDM->Name ;


    if ( wDP.hasViewer()) {
            utf8VaryingString wS="Launching viewer for " + wDP.Content ;
            StatusBar->showMessage("Launching viewer for " + wDP.Content );
            if (genericViewer(&wDP,this,ErrorLog)){
                StatusBar->clearMessage();
                return;
            }
            StatusBar->showMessage(ErrorLog->getLastMessage().toCChar());
        }

    return;

} // ZObjectFileSelector::ObjectTRwClicked

void ZObjectFileSelector::HeaderClicked(int pColumn)
{

    switch (pColumn)
    {
    case 0:
    case 1:
        ObjectListTBv->ItemModel->setSortRole(Qt::DisplayRole);
        break;
    default:
        ObjectListTBv->ItemModel->setSortRole(ZSortRole);
    }

    if (ObjectListTBv->SortOrder==ZQTableView::ZSOR_Nothing)
        ObjectListTBv->SortOrder=ZQTableView::ZSOR_Descending;

    if (ObjectListHeader->sortIndicatorSection() == pColumn) {
        if (ObjectListTBv->SortOrder==ZQTableView::ZSOR_Descending) {

            ObjectListHeader->setSortIndicator(pColumn,Qt::AscendingOrder);
            ObjectListHeader->setSortIndicatorShown(true);
            ObjectListTBv->sortByColumn(pColumn,Qt::AscendingOrder);
            ObjectListTBv->SortOrder=ZQTableView::ZSOR_Ascending;
            return ;
        }
        ObjectListHeader->setSortIndicator(pColumn,Qt::DescendingOrder);
        ObjectListHeader->setSortIndicatorShown(true);
        ObjectListTBv->sortByColumn(pColumn,Qt::DescendingOrder);
        ObjectListTBv->SortOrder=ZQTableView::ZSOR_Descending;
        return;
    }

    if (ObjectListTBv->SortOrder==ZQTableView::ZSOR_Descending) {
            ObjectListHeader->setSortIndicator(pColumn,Qt::AscendingOrder);
            ObjectListHeader->setSortIndicatorShown(true);
            ObjectListTBv->sortByColumn(pColumn,Qt::AscendingOrder);
            ObjectListTBv->SortOrder=ZQTableView::ZSOR_Ascending;
            return;
    }
    ObjectListHeader->setSortIndicator(pColumn,Qt::DescendingOrder);
    ObjectListHeader->setSortIndicatorShown(true);
    ObjectListTBv->sortByColumn(pColumn,Qt::DescendingOrder);
    ObjectListTBv->SortOrder=ZQTableView::ZSOR_Descending;
    return;
}


QList<QStandardItem*>
ZObjectFileSelector::createDirEntryRow(DirMap& pDirEntry)
{
    QList<QStandardItem*> wRow;
    wRow << createItem(pDirEntry.Name.getBasename());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    utf8VaryingString wTypeStr;
    ZDirFileEn_type wType = pDirEntry.Type;
    if (pDirEntry.Type & ZDFT_Hidden) {
        wTypeStr = "Hidden";
    }
    switch (pDirEntry.Type)
    {
    case ZDFT_Nothing:
        wTypeStr.addConditionalOR("Nothing");
        break;
    case ZDFT_RegularFile:
        wTypeStr.addConditionalOR("File");
        break;
    case ZDFT_Directory:
        wTypeStr.addConditionalOR("Directory");
        break;
    case ZDFT_SymbolicLink:
        wTypeStr.addConditionalOR("Symb link");
        break;
    case ZDFT_Other:
        wTypeStr.addConditionalOR("Other");
        break;
    case ZDFT_Hidden:
        wTypeStr.addConditionalOR("Hidden");
        break;
    }
    QVariant wV;
    wRow << createItem(wTypeStr);
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    wRow << createItem(pDirEntry.Size);
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
//    wV.setValue<long long>(pDirEntry.Size);
    wRow.last()->setData(QVariant((long long)pDirEntry.Size),ZSortRole);
    wRow << createItem(pDirEntry.Created);
    wV.setValue(QVariant((long long)pDirEntry.Created.tv_sec));
    wRow.last()->setData(wV,ZSortRole);
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    wRow << createItem(pDirEntry.Modified);
    wV.setValue(QVariant((long long)pDirEntry.Created.tv_sec));
    wRow.last()->setData(wV,ZSortRole);
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    wRow << ViewZFb.create();
 //   wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

//    QVariant wV;
    wV.setValue<DirMap*> (new DirMap(pDirEntry));
    wRow[0]->setData(wV,ZQtValueRole);

    return wRow;
}

void
ZObjectFileSelector::removeDirEntryRow(int pRow)
{
    QStandardItem* wSI=ObjectListTBv->ItemModel->item(pRow,0);
    QVariant wV;
    wV = wSI->data(ZQtValueRole);
    DirMap* wDM= wV.value<DirMap*>();
    delete wDM;
    ObjectListTBv->ItemModel->removeRow(pRow);
}

void
ZObjectFileSelector::removeAllDirEntryRow()
{
    while (ObjectListTBv->ItemModel->rowCount()>0)
        removeDirEntryRow(0);
}

void
ZObjectFileSelector::Validation()
{
    if (SelectedDirEntry.Name.isEmpty()) {
        ZExceptionDLg::adhocMessage("Object selection",Severity_Error,nullptr,nullptr,
                                    "No object has been selected");
        return;
    }
    ZDomainDLg* wDDLg=new ZDomainDLg(this);
    wDDLg->setupCreate(FatherDP);
    wDDLg->setContent(SelectedDirEntry.Name.getBasename());

    uriString wURI = SelectedDirEntry.Name ;
    ZDomain_type wType = deducePossibleType(wURI);

    wDDLg->setObjectType(wType);
    wDDLg->setDomainName(SelectedDirEntry.Name.getRootname());
     int wRet = wDDLg->exec();
    if (wRet==QDialog::Accepted) {
        /*
        wDDLg->getDomainPathRef()->setFather(FatherDP);
        FatherDP->addChild(wDDLg->getDomainPathRef());  // already done within ZDomainDLg

        FatherDP->setHasChanged(true);
        */
         emit (DomainsHaveChanged(wDDLg->getDomainPathRef()->father()));

    }
    wDDLg->deleteLater();
    setup(FatherDP);
    return;
}

void
ZObjectFileSelector::CustomSelectionChanged(Qt::CheckState pState)
{
    if (pState==Qt::Checked) {
        if (CustomSelLEd->text().isEmpty()) {
            CustomSelLEd->setText(FileSelection.Phrase.toCChar());
            CustomSelLEd->adjustSize();
        }
        SelectionFRm->setVisible(true);
        return;
    }
    SelectionFRm->setVisible(false);
}


void
ZObjectFileSelector::ApplyCustomSelection()
{
    FileSelection.setSelPhrase( CustomSelLEd->text().toUtf8().data()) ;

    _DBGPRINT(FileSelection.dump().toCChar())

    setup(FatherDP);

    FileSelectionLBl->setText(CustomSelLEd->text());

    return;
}

bool
ZObjectFileSelector::ObjectListMouseCallBack(int pKey,QMouseEvent* pEvent)
{
    return false;
}
