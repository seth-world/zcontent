#include "zquerywizardmwd.h"
//#include "ui_ZQueryWizardMWd.h"
#include <functional>

#include <zcontent/zcontentcommon/zgeneralparameters.h>
#include <zcontent/zcontentcommon/zcontentconstants.h>
#include <zcontent/zcontentcommon/zdomainpath.h>
#include <zcontent/zcontentcommon/zcontentobjectbroker.h>
#include "zdomainobjectfinder.h"

#include <QWidget>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QFrame>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QGroupBox>

#include <QStatusBar>
#include <QLabel>
#include <QLineEdit>

#include <QTableView>
#include <QCheckBox>

#include <zqt/zqtwidget/zqlabel.h>


#include <QProgressBar>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QHeaderView>


#include <zqt/zqtwidget/zqplaintextedit.h>

#include <zqt/zqtwidget/zqtableview.h>

#include "texteditmwn.h"

#include "zhelp.h"

#include <zcontent/zindexedfile/zsearchparser.h>

#include <zcontent/zindexedfile/zsearchparsertype.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zcontentcommon/zresource.h>

#include "zcellformatdlg.h"
#include "zcollectionlist.h"
#include "zmfprogressmwn.h"


#include "zhelp.h"
#include "zsearchquerymwd.h"

#include <zcontent/zindexedfile/zsearchentitycontext.h>

#include <zqt/zqtwidget/zcombodelegate.h>

#include <zqt/zqtwidget/zqstatusbar.h>

using namespace zbs;
/*
ZQueryWizardMWd::ZQueryWizardMWd(ZSearchQueryMWd *parent) : QMainWindow((QWidget*)parent)
{
    QueryMain = parent;
    ErrorLog = QueryMain->ErrorLog;

    ErrorLog->setAutoPrintAll();

    initLayout();
}
*/
ZQueryWizardMWd::ZQueryWizardMWd(ZaiErrors* pErrorLog, QWidget *parent) : QMainWindow((QWidget*)parent)
{
//    QueryMain = parent;
    ErrorLog = pErrorLog;

    ErrorLog->setAutoPrintAll();

    initLayout();
}


ZQueryWizardMWd::~ZQueryWizardMWd()
{

}

void
ZQueryWizardMWd::setQueryMain(ZSearchQueryMWd* pQueryMain)
{
    QueryMain=pQueryMain;
    QObject::connect(QueryMain,&ZSearchQueryMWd::instructionHasBeenExecuted,this,&ZQueryWizardMWd::QueryHasBeenExecuted);
}

void
ZQueryWizardMWd::QueryHasBeenExecuted(ZSearchContext* pContext)
{
//    statusBar->showMessage("Request executed with returned status <%s>",decode_ZStatus(pContext->Status));
    if (LabelGoodPalette == QPalette()) {
        LabelGoodPalette=LabelBadPalette=LabelDefaultPalette = LastStatusLBl->palette();
        LabelGoodPalette.setColor(QPalette::Window, Qt::white);
        LabelGoodPalette.setColor(QPalette::WindowText, Qt::darkGreen);
        LabelBadPalette.setColor(QPalette::Window, Qt::white);
        LabelBadPalette.setColor(QPalette::WindowText, Qt::red);
    }
    if ((pContext->Status==ZS_SUCCESS)||(pContext->Status==ZS_OUTBOUNDHIGH))
        LastStatusLBl->setPalette(LabelGoodPalette);
    else
        LastStatusLBl->setPalette(LabelBadPalette);
    LastStatusLBl->setText(decode_ZStatus(pContext->Status));
    if (pContext->Status==ZS_SUCCESS) {
        QueryQTe->clear();
    }
    setupEntitiesFromQuery();
}

void
ZQueryWizardMWd::setupEntitiesFromQuery()
{
    EntityListTBv->removeAll();


    for (int wi = 0 ; wi < SECList.count() ; wi++ ) {
        QList<QStandardItem*> wRow;
        wRow << new QStandardItem(QueryMain->Parser->EntityList[wi]->getEntityName().toCChar());
        if (QueryMain->Parser->EntityList[wi]->_BaseEntity!=nullptr)
            wRow << new QStandardItem("Collection");
        else
            if (QueryMain->Parser->EntityList[wi]->_FileEntity!=nullptr)
                wRow << new QStandardItem("File");
            else
                wRow << new QStandardItem("Unknown");
        EntityListTBv->ItemModel->appendRow(wRow);
    }
    EntityListTBv->resizeToContent();
}

void ZQueryWizardMWd::initLayout()
{
    menuBar = new QMenuBar;
    setMenuBar(menuBar);

    setWindowTitle("Search Query Wizard");

    statusBar = new ZQStatusBar(this);
    setStatusBar(statusBar);
    statusBar->setMultiLine();

    QVBoxLayout* MainVBox=new QVBoxLayout(this);
    CentralWidget = new QWidget(this);
    setCentralWidget(CentralWidget);
    CentralWidget->setLayout(MainVBox);

    resize(815, 640);

    QHBoxLayout* QHB0 = new QHBoxLayout;
    CurrentEntityLBl = new QLabel("<no current entity>",this);
    QHB0->addWidget(new QLabel("Current entity",this));
    QHB0->addWidget(CurrentEntityLBl);

    MainVBox->addLayout(QHB0);

    QueryQTe = new ZQPlainTextEdit(this);
    MainVBox->addWidget(QueryQTe);

    QHBoxLayout* wHBLQry = new QHBoxLayout;
    MainVBox->addLayout(wHBLQry);
    wHBLQry->setAlignment(Qt::AlignRight);

    QLabel* QL13 = new QLabel("Last status",ExpWidget2);
    LastStatusLBl = new QLabel(ExpWidget2);
    LastStatusLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    LastStatusLBl->setLineWidth(2);
    LastStatusLBl->setAutoFillBackground(true);
    wHBLQry->addWidget(QL13);
    wHBLQry->addWidget(LastStatusLBl);

    QPushButton* wExecuteBTn= new QPushButton("Execute",this);
    wExecuteBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QObject::connect(wExecuteBTn,&QPushButton::clicked,this,&ZQueryWizardMWd::ExecuteClicked);
    wHBLQry->addWidget(wExecuteBTn);

    QSize wLS =   wExecuteBTn->size();
    wLS.setHeight(LastStatusLBl->sizeHint().height());
    LastStatusLBl->resize(wLS);
    LastStatusLBl->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    EntityListGPb = new QGroupBox("Entities",this);
    QVBoxLayout* QVBL0 = new QVBoxLayout;
    EntityListGPb->setLayout(QVBL0);

    EntityListTBv = new ZQTableView(this);
    EntityListTBv->newModel(3);
    QVBL0->addWidget(EntityListTBv);

    MainVBox->addWidget(EntityListGPb);

    FieldsSelectionGPb = new QGroupBox("Fields selection",this);
    QVBoxLayout* QVBL1 = new QVBoxLayout;
    MainVBox->addWidget(FieldsSelectionGPb);

    FieldsSelectionGPb->setLayout(QVBL1);
    AllFieldsCHk = new QCheckBox("All fields",this);
    QVBL1->addWidget(AllFieldsCHk);
    FieldsSelectionTBv = new ZQTableView(this);
    QVBL1->addWidget(FieldsSelectionTBv);
    MainVBox->addWidget(FieldsSelectionGPb);


    RestrictGPb = new QGroupBox("Rank selection",this);
    QVBoxLayout* QVBL2 = new QVBoxLayout;
    RestrictGPb->setLayout(QVBL2);
    AllRanksCHk = new QCheckBox("All ranks",this);
    QVBL2->addWidget(AllRanksCHk);
    RestrictTBv = new ZQTableView(this);
    QVBL2->addWidget(RestrictTBv);
    MainVBox->addWidget(RestrictGPb);

 /* menu bar  menus &  actions*/

 /*   menuBar=new QMenuBar(this);
    setMenuBar(menuBar);
*/
    QActionGroup* wMainAGp=new QActionGroup(this);
    QObject::connect(wMainAGp,&QActionGroup::triggered,this,&ZQueryWizardMWd::MenuTriggered);

    /*
    uriString wQuitImg;
    wQuitImg = GeneralParameters.getIconDirectory();
    wQuitImg.addConditionalDirectoryDelimiter();
    wQuitImg += "system-shutdown.png";
    QIcon wQuitICn(wQuitImg.toCChar());
*/
    QIcon wQuitICn;
    ZStatus wSt=ZS_SUCCESS;

    ErrorLog->infoLog("ZQueryWizardMWd::initLayout ");

//    wSt= zbs::ContentObjectBroker.iconFactory("general.iconfactory/system-shutdown.png",wQuitICn,ErrorLog);

    wQuitICn = zbs::ContentObjectBroker.iconFactory("general.iconfactory/system-shutdown.png",ErrorLog);

    QuitIconQAc = new QAction(wQuitICn,"",this);
    menuBar->addAction(QuitIconQAc);
    wMainAGp->addAction(QuitIconQAc);

    QMenu* GeneralMEn = new QMenu("General",this);
    menuBar->addMenu(GeneralMEn);

    QMenu* ActionsMEn = new QMenu("Actions",this);
    menuBar->addMenu(ActionsMEn);


    SaveQAc = new QAction("Save generated requests",this);
    wMainAGp->addAction(SaveQAc);
    GeneralMEn->addAction(SaveQAc);

    QuitQAc = new QAction("Quit",this);
    wMainAGp->addAction(QuitQAc);
    GeneralMEn->addAction(QuitQAc);


    CreateFileEntityQAc = new QAction("Create file entity");
    wMainAGp->addAction(CreateFileEntityQAc);
    ActionsMEn->addAction(CreateFileEntityQAc);

    CreateCollectionEntityQAc = new QAction("Create collection");
    wMainAGp->addAction(CreateCollectionEntityQAc);
    ActionsMEn->addAction(CreateCollectionEntityQAc);

    CreateSymbolQAc = new QAction("Create symbol",this);
    wMainAGp->addAction(CreateSymbolQAc);
    ActionsMEn->addAction(CreateSymbolQAc);

    /* not in action group */

    QMenu*    OptionsQMe=new QMenu("Options",this);
    GeneralMEn->addMenu(OptionsQMe);

    QAction*  OptReportQAc=new QAction("Parsing report");
    OptReportQAc->setCheckable(true);


    QMenu*    VerboseQMe=new QMenu("Verbose level");
    QAction*  OptNoVerboseQAc=new QAction("Mute");
    OptNoVerboseQAc->setCheckable(true);
    QAction*  OptVerboseQAc=new QAction("Verbose");
    OptNoVerboseQAc->setCheckable(true);
    QAction*  OptFullVerboseQAc=new QAction("Full verbose");
    OptNoVerboseQAc->setCheckable(true);

    VerboseQMe->addAction(OptFullVerboseQAc);
    VerboseQMe->addAction(OptVerboseQAc);
    VerboseQMe->addAction(OptNoVerboseQAc);


} // initLayout




void
ZQueryWizardMWd::MenuTriggered(QAction* pAction)
{
  if ((pAction==QuitQAc)||(pAction==QuitIconQAc)) {
    QuitClicked();
    return;
  }
  if ((pAction==SaveQAc)) {
      SaveClicked();
      return;
  }
  if (pAction == CreateFileEntityQAc){
      CreateFileEntityClicked();
      return;
  }
  if (pAction == CreateCollectionEntityQAc){
      CreateCollectionClicked();
      return;
  }

  if (pAction == SubmitQAc){
    ExecuteClicked();
    return;
  }
  if (pAction == SaveQAc){
    SaveInstructions();
    return;
  }
} // MenuTriggered


void
ZQueryWizardMWd::QuitClicked ()
{
    /*  manage if requests has to be saved or not
    */
    hide();
    this->deleteLater();
}

/* save the query requests to file */
void
ZQueryWizardMWd::SaveClicked ()
{

    hasBeenSaved = true;
}

void
ZQueryWizardMWd::CreateFileEntityClicked ()
{
    ZDomainObjectFinder* wDOF = new ZDomainObjectFinder(ErrorLog,this);
    wDOF->setup(DomainBroker.getRoot());
//    wDOF-> setAttribute(Qt::WA_SetWindowModality , true);
    QObject::connect(wDOF , &ZDomainObjectFinder::domainObjectSelected,this, &ZQueryWizardMWd::CreateFileEntity ,Qt::SingleShotConnection);
    wDOF->show();
    hasBeenSaved = false;
}

void
ZQueryWizardMWd::CreateFileEntity(ZDomainPath* pDP)
{
    QDialog* wEntityDLg = new QDialog(this) ;
    QVBoxLayout* wVBL=new QVBoxLayout;
    wEntityDLg->setLayout(wVBL);
    QHBoxLayout* wHBL= new QHBoxLayout;
    wVBL->addLayout(wHBL);
    wHBL->addWidget(new QLabel("Entity name",this));
    QLineEdit* wEntityNameLEd=new QLineEdit(pDP->getName().toCChar(),this);
    wHBL->addWidget(wEntityNameLEd);
    QHBoxLayout* wHBL1= new QHBoxLayout;
    wVBL->addLayout(wHBL1);
    wHBL1->setAlignment(Qt::AlignRight);

    QPushButton* wOk=new QPushButton("Ok",this);
    wOk->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    QPushButton* wCancel=new QPushButton("Cancel",this);
    wCancel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    wHBL1->addWidget(wOk);
    wHBL1->addWidget(wCancel);
    QObject::connect(wOk,&QPushButton::clicked,wEntityDLg,&QDialog::accept,Qt::SingleShotConnection );
    QObject::connect(wCancel,&QPushButton::clicked,wEntityDLg,&QDialog::reject,Qt::SingleShotConnection );

    int wRet=wEntityDLg->exec();
    if (wRet==QDialog::Rejected)
        return;
    utf8VaryingString wEntityName = wEntityNameLEd->text().toUtf8().data();
    utf8VaryingString wQry;
    wQry.sprintf("declare %s as file \"%s\" ;",wEntityNameLEd->text().toUtf8().data(),pDP->constructFullPhysicalPath().toString() );
    QueryQTe->setPlainText(wQry.toCChar());
    return;
}


void
ZQueryWizardMWd::CreateCollectionClicked ()
{

    hasBeenSaved = false;
}


void
ZQueryWizardMWd::ExecuteClicked ()
{
  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wQuery = QueryQTe->getText();
  statusBar->clearMessage();

  QueryMain->QueryQTe->setPlainText(wQuery.toCChar());
  QueryMain->ExecuteClicked();

#ifdef __COMMENT__

  LastStatusLBl->setText("");



  if (LabelGoodPalette == QPalette()) {
      LabelGoodPalette=LabelBadPalette=LabelDefaultPalette = LastStatusLBl->palette();
      LabelGoodPalette.setColor(QPalette::Window, Qt::white);
      LabelGoodPalette.setColor(QPalette::WindowText, Qt::darkGreen);
      LabelBadPalette.setColor(QPalette::Window, Qt::white);
      LabelBadPalette.setColor(QPalette::WindowText, Qt::red);
  }
/*
  if ((wSearchContext.Status==ZS_SUCCESS)&&(wSearchContext.InstructionType & ZSITP_ToBeExecuted)) {

      ZMFProgressMWn* wProgress = new ZMFProgressMWn("Query progress", this,ZPTP_TimedClose | ZPTP_HasCloseBtn);
      Parser->setProgressSetupCallback(std::bind(&ZProgressBase::advanceSetupCallBack,wProgress,std::placeholders::_1,std::placeholders::_2));
      Parser->setProgressCallback(std::bind(&ZProgressBase::advanceCallBack,wProgress,std::placeholders::_1,std::placeholders::_2));
      wProgress->show();

      wSt=Parser->execute(wSearchContext);
*/
/*
       if (wSearchContext.TargetEntity != nullptr) {
        wSt=DisplayEntity(wSearchContext.TargetEntity);
           if (wSt==ZS_SUCCESS) {
               GlobalCellFmtLBl->setVisible(true);
           }
      }

      wProgress->setDone((wSt!=ZS_SUCCESS)&&(wSt!=ZS_OUTBOUNDHIGH));
  }
*/


  if ((wSt==ZS_SUCCESS)||(wSt==ZS_OUTBOUNDHIGH))
      LastStatusLBl->setPalette(LabelGoodPalette);
  else
      LastStatusLBl->setPalette(LabelBadPalette);
#endif // __COMMENT__
//  QueryQTe->clear();

} // ZQueryWizardMWd::ExecuteBTnClicked

void
ZQueryWizardMWd::statusBarMessage(const utf8VaryingString& pMsg)
{
    statusBar->showMessage(pMsg);
}


void
ZQueryWizardMWd::resizeEvent(QResizeEvent* pEvent)
{
  QSize wROld = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (_FResizeInitial) {
    _FResizeInitial=false;
    return;
  }
  QRect wRNew = geometry();

  int wWMargin = (pEvent->size().width() - pEvent->oldSize().width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin =  pEvent->size().height() - pEvent->oldSize().height();
  int wVH=pEvent->size().height() - wHMargin ;
  QRect wR2 = geometry();
  int wWdgW= wR2.width()+wWMargin;
  int wWdgH= wR2.height()+wHMargin;

#ifdef __COMMENT__
  QRect wR1 = Centralwidget->geometry();
  int wWd1gW= wR1.width()+wWMargin;     /* resize only in width */
  Centralwidget->resize(wWd1gW,wR1.height());  /* expands in width only */
#endif // __COMMENT__
/*

  CentralWidget->resize(wWdgW,wWdgH);  // expands in width and height
*/
  QRect wR3 = ForBackWDg->geometry();
  wWdgW= wR3.width()+wWMargin;
  ForBackWDg->setGeometry(QRect(0, 0, wWdgW, wR3.height())); /* expands in width only */
/*
  int wX = ForBackWDg->size().width() - LBlSize -LBlSize;
  ForwardLBl->setGeometry(QRect(wX,HeightMargin,LBlSize,LBlSize));
  wX -= (ForwardLBl->size().width()*2);
  ExecuteLBl->setGeometry(QRect(wX,HeightMargin,LBlSize,LBlSize));
  wX -= (ExecuteLBl->size().width()*2);
  BackwardLBl->setGeometry(QRect(wX,HeightMargin,LBlSize,LBlSize));
*/
  int wY= ForBackWDg->size().height()+HeightMargin;
  QRect wR4 = QueryQTe->geometry();
  wWdgW= wR4.width()+wWMargin;
  QueryQTe->setGeometry(QRect(5, wY, wWdgW, 91)); /* expands in width only */

  wY += QueryQTe->size().height() + HeightMargin;

  QRect wR5 = ExpWidget2->geometry();
  wWdgW= wR5.width()+wWMargin;
  wWdgH= wR5.height()+wHMargin;
  ExpWidget2->resize(wWdgW,wWdgH);  /* expands in width and height */

}//DicEdit::resizeEvent
/*
void
ZQueryWizardMWd::ExecuteRequest()
{

}
*/

void ZQueryWizardMWd::SaveInstructions() {

}

