#include "zobjectselector.h"
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


#include <qevent.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <zqt/zqtwidget/zqstatusbar.h>

#include <zcontent/zcontentcommon/zdomainpath.h>
#include <zcontent/zcontentcommon/zcontentobjectbroker.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <ztoolset/zaierrors.h>

#include "zdomaindlg.h"

#include <zcontent/zcontentcommon/zviewers.h>

using namespace zbs;


ZObjectSelector::ZObjectSelector(ZDomain_type pObjectFilter, ZaiErrors *pErrorLog, QWidget *pFather)
    : QMainWindow{pFather}
{
    ErrorLog = pErrorLog;
    ObjectFilter=pObjectFilter;
    initLayout();
}

void
ZObjectSelector::initLayout()
{
    setWindowTitle("Object selector");
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
    QObject::connect(GeneralActionGroupQAg, &QActionGroup::triggered, this, &ZObjectSelector::DomainActionEvent);

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
    ObjectFilterLBl->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    ObjectFilterLBl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    wHBL10->addWidget(ObjectFilterLBl);


    wHBL10->addSpacing(2);
    wHBL10->addWidget(new QLabel("Selected file extensions",this));
    wHBL10->addSpacing(2);

    FileSelection = inducePossibleType(ObjectFilter);

    QLabel* wExtLBl = new QLabel(FileSelection.display().toCChar());
    wExtLBl->adjustSize();
    wExtLBl->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    wExtLBl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    wHBL10->addWidget(wExtLBl);
    wHBL10->addStretch();

    wVBLMain->addLayout(wHBL10);
    wVBLMain->addSpacing(2);

    ObjectListTBv = new ZQTableView(this);
    wVBLMain->addWidget(ObjectListTBv);
    ObjectListTBv->setFilterMask(ZEF_DoubleClickLeft);


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



    QObject::connect(wCancel,&QPushButton::clicked,this,&ZObjectSelector::Quit);
    QObject::connect(wOk,&QPushButton::clicked,this,&ZObjectSelector::Validation);


    ObjectListTBv->newModel(6);

    ObjectListTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("Name"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem("Type"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem("Size"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem("Creation"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(4,new QStandardItem("Modification"));
    ObjectListTBv->ItemModel->setHorizontalHeaderItem(5,new QStandardItem("View"));

    ObjectListTBv->verticalHeader()->setVisible(false);

    QHeaderView * wHHead = ObjectListTBv->horizontalHeader();
    wHHead->setSectionsClickable(true);
    QObject::connect(wHHead,&QHeaderView::sectionDoubleClicked,this,&ZObjectSelector::HeaderClicked);


    ObjectListTBv->setSelectionMode(QAbstractItemView::SingleSelection);
    ObjectListTBv->setSelectionBehavior(QAbstractItemView::SelectRows);

    ObjectListTBv->setSortingEnabled(true);

/*
    ObjectListTBv->_registerCallBacks(std::bind(&ZObjectSelector::ObjectTRvKeyFiltered, this,placeholders::_1,placeholders::_2),
                         std::bind(&ZObjectSelector::ObjectTRvMouseFiltered, this,placeholders::_1,placeholders::_2),
                         std::bind(&ZObjectSelector::ObjectTRvFlexMenu, this,placeholders::_1),
                         nullptr);
*/
    ObjectListTBv->_registerCallBacks(std::bind(&ZObjectSelector::ObjectTRvKeyFiltered, this,placeholders::_1,placeholders::_2),
                                      nullptr,
                                      std::bind(&ZObjectSelector::ObjectTRvFlexMenu, this,placeholders::_1),
                                      nullptr);

    QObject::connect(ObjectListTBv,&QTableView::clicked,this,&ZObjectSelector::ObjectTRvClicked);
    QObject::connect(ObjectListTBv,&QTableView::doubleClicked,this,&ZObjectSelector::ObjectTRvDoubleClicked);

    ViewZFb.setup  ("general.iconfactory.iconview",ErrorLog) ;
    ZFakeBTnDelegate*wID = new ZFakeBTnDelegate (ObjectListTBv->ItemModel,this);
    ObjectListTBv->setItemDelegateForColumn(ColumnView,wID);


}// ZObjectSelector::initLayout


bool
ZObjectSelector::setup(ZDomainPath *pDP)
{
    removeAllDirEntryRow();
    CurrentDP = pDP ;
    uriString wURIPath ;
    utf8VaryingString wObject ;

    DomainChainLBl->setText(pDP->getHierarchy().toCChar());

    pDP->constructPath(wURIPath,wObject);

    CurrentPathLBl->setText(wURIPath.toCChar());

    if (pDP->isRemote())
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
        ErrorLog->errorLog("ZObjectSelector::setup Cannot dir <%s> status <%s>",
                           wURIPath.toCChar(),decode_ZStatus(wSt));
        this->deleteLater();
        return false;
    }

    DirMap wDirEntry ;

//    wSt=wDir.fullDir(wDirEntry);
    wSt=wDir.fullDirSel(wDirEntry, FileSelection);
    while (wSt==ZS_SUCCESS) {
        if (!(wDirEntry.Type & ZDFT_Hidden)) {
        QList<QStandardItem*> wRow = createDirEntryRow(wDirEntry);
        ObjectListTBv->ItemModel->appendRow(wRow);
        }
        wSt=wDir.fullDirSel(wDirEntry,FileSelection);
    }

    for ( int wi = 0 ; wi < ObjectListTBv->ItemModel->columnCount() ; wi++) {
        ObjectListTBv->resizeColumnToContents(wi);
    }
    return true;
} // ZObjectSelector::setup



void
ZObjectSelector::Quit()
{
    this->deleteLater();
    return;
}

bool
ZObjectSelector::ObjectTRvKeyFiltered(int pKey,QKeyEvent* pEvent)
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
}//ZObjectSelector::KeyTReKeyFiltered

bool
ZObjectSelector::ObjectTRvMouseFiltered(int pKey,QMouseEvent* pEvent)
{
    GlobalMousePosition = pEvent->globalPos();

    if (pKey==ZEF_DoubleClickLeft) {
        //        return domainChange();
    }
    return false;
}

void ZObjectSelector::ObjectTRvFlexMenu(QContextMenuEvent *event)
{
    if (DomainFlexMEn==nullptr)
        setupDomainFlexMenu();
    DomainFlexMEn->exec(event->globalPos());
}

QMenu *
ZObjectSelector::setupDomainFlexMenu()
{
    DomainFlexMEn=new QMenu(this);

    DomainFlexMEn->setTitle("Domains");

    ActionGroupQAg=new QActionGroup(DomainFlexMEn) ;
    QObject::connect(ActionGroupQAg, &QActionGroup::triggered, this, &ZObjectSelector::DomainActionEvent);
    /*
    CreateDomainQAc= new QAction("Add domain at same level<Insert>",DomainFlexMEn);
    DomainFlexMEn->addAction(CreateDomainQAc);
    DomainActionGroupQAg->addAction(CreateDomainQAc);

*/

    return DomainFlexMEn;
}//setupDomainFlexMenu

void
ZObjectSelector::DomainActionEvent(QAction* pAction)
{
    if ((pAction==QuitQAc)) {
        Quit();
        return;
    }

}//ZObjectSelector::DomainActionEvent

void
ZObjectSelector::setCurrent(DirMap* pDM)
{
    SelectedDirEntry = *pDM;
    SelectedLEd->setText(pDM->Name.getBasename().toCChar());
}


void
ZObjectSelector::ObjectTRvDoubleClicked (const QModelIndex &pIndex)
{
    ObjectTRvClicked(pIndex);
    Validation();
    return;
} // ZDomainObjectFinder::ObjectTRvDoubleClicked

void
ZObjectSelector::ObjectTRvClicked (const QModelIndex &pIndex)
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

    ZDomainPath wDP(wType);
    wDP.forceSetName(wDM->Name.getBasename());
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

} // ZObjectSelector::ObjectTRwClicked

 void
ZObjectSelector::HeaderClicked(int pColumn)
{
    ObjectListTBv->horizontalHeader()->setSortIndicator(pColumn, Qt::AscendingOrder);
    ObjectListTBv->sortByColumn(pColumn, Qt::AscendingOrder);
}


QList<QStandardItem*>
ZObjectSelector::createDirEntryRow(DirMap& pDirEntry)
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
    wRow << createItem(wTypeStr);
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    wRow << createItem(pDirEntry.Size);
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    wRow << createItem(pDirEntry.Created);
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    wRow << createItem(pDirEntry.Modified);
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    wRow << ViewZFb.create();
 //   wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QVariant wV;
    wV.setValue<DirMap*> (new DirMap(pDirEntry));
    wRow[0]->setData(wV,ZQtValueRole);

    return wRow;
}

void
ZObjectSelector::removeDirEntryRow(int pRow)
{
    QStandardItem* wSI=ObjectListTBv->ItemModel->item(pRow,0);
    QVariant wV;
    wV = wSI->data(ZQtValueRole);
    DirMap* wDM= wV.value<DirMap*>();
    delete wDM;
    ObjectListTBv->ItemModel->removeRow(pRow);
}

void
ZObjectSelector::removeAllDirEntryRow()
{
    while (ObjectListTBv->ItemModel->rowCount()>0)
        removeDirEntryRow(0);
}

void
ZObjectSelector::Validation()
{
    ZDomainDLg* wDDLg=new ZDomainDLg(this);
    wDDLg->setupCreate(CurrentDP);
    wDDLg->setContent(SelectedDirEntry.Name.getBasename());

    uriString wURI = SelectedDirEntry.Name ;
    ZDomain_type wType = deducePossibleType(wURI);

    wDDLg->setObjectType(wType);
    wDDLg->setDomainName(SelectedDirEntry.Name.getRootname());
     int wRet = wDDLg->exec();
    if (wRet==QDialog::Accepted) {
         wDDLg->getDomainPathRef()->setFather( CurrentDP );
//         CurrentDP->addChild(wDDLg->getDomainPathRef());  // already done within ZDomainDLg
         CurrentDP->setHasChanged(true);
         emit (DomainsHaveChanged(wDDLg->getDomainPathRef()->father()));

    }
    wDDLg->deleteLater();
    setup(CurrentDP);
    return;
}
