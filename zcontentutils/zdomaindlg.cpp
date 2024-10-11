#include "zdomaindlg.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QFrame>
#include <QResizeEvent>
#include <QGroupBox>
#include <QRadioButton>

/* for text search within html document facilities */
#include <QTextDocument>
#include <QTextCursor>
#include <QStandardItem>


#include <zexceptiondlg.h>


#include <zqt/zqtwidget/zqtableview.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zqt/zqtwidget/zqtwidgettypes.h>

//#include <zcontentcommon/zgeneralparameters.h>
#include <zcontent/zcontentcommon/zcontentobjectbroker.h>


namespace zbs {

ZDomainDLg::ZDomainDLg(QWidget *pParent) : QDialog(pParent)
{
    ErrorLog.setAutoPrintAll();
    initLayout();
}

ZDomainDLg::~ZDomainDLg() {

}

void
ZDomainDLg::initLayout()
{
  resize(900,361);

    if (DomainPathRef!=nullptr) {
        delete DomainPathRef ;
        DomainPathRef = nullptr ;
    }

  NameHasChanged=false;

  AcceptBTn = new QPushButton(this);
  AcceptBTn->setText("OK");
  AcceptBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );

  RejectBTn = new QPushButton(this);
  RejectBTn->setText("Forget");
  RejectBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );

  InferiorsTBv=new ZQTableView(this);
  InferiorsTBv->newModel(3); /* 3 columns */
  InferiorsTBv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected


  QVBoxLayout*  VBL=new QVBoxLayout(this);
  setLayout(VBL);

  FatherNameLBl = new QLabel(this);
  FatherNameLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  FatherHierarchyLBl = new QLabel(this);
  FatherHierarchyLBl->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  QHBoxLayout* wFNHBL=new QHBoxLayout;
  VBL->addLayout(wFNHBL);
  QLabel* wFDN=new QLabel("Father node name");
  wFDN->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  wFNHBL->addWidget(wFDN);
  wFNHBL->addSpacing(2);
  wFNHBL->addWidget(FatherNameLBl);
  wFNHBL->addSpacing(2);
  QLabel* wFDH=new QLabel("Hierarchy");
  wFDN->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  wFNHBL->addWidget(wFDH);
  wFNHBL->addSpacing(2);
  wFNHBL->addWidget(FatherHierarchyLBl);

  QFormLayout* QForm=new QFormLayout(this);
  VBL->addItem(QForm);

  NameLEd = new QLineEdit(this);
  QForm->addRow(new QLabel("Domain node name",this),NameLEd);

  ToolTipLEd = new QLineEdit(this);
  QForm->addRow(new QLabel("ToolTip",this),ToolTipLEd);
  ContentLEd = new QLineEdit(this);
  QForm->addRow(new QLabel("Content",this),ContentLEd);

  TypeWDg = new QWidget(this);
  QVBoxLayout* TWQVB= new QVBoxLayout;
  TypeWDg->setLayout(TWQVB);
/*
  PathCHk = new QCheckBox("Is a path",this);
  TWQVB->addWidget(PathCHk);
*/
  ObjectTypeCBx = new QComboBox(this);

  for (int wi=0; TypeDecodeTable[wi].Type != ZDOM_Nothing ; wi ++ )
      ObjectTypeCBx->addItem(TypeDecodeTable[wi].Display.toCChar());
  /*
  ObjectTypeCBx->addItem("Path");
  ObjectTypeCBx->addItem("Icon");
  ObjectTypeCBx->addItem("Image");
  ObjectTypeCBx->addItem("Text file");
  ObjectTypeCBx->addItem("Pdf file");
  ObjectTypeCBx->addItem("Master file");
  ObjectTypeCBx->addItem("Header file");
  ObjectTypeCBx->addItem("Executable");
  ObjectTypeCBx->addItem("Other file");
  */

  TWQVB->addWidget( ObjectTypeCBx ) ;

  QWidget *RelativeBtnGrp = new QWidget(this);
  TWQVB->addWidget(RelativeBtnGrp);
  QVBoxLayout* VBtnGrp1 = new QVBoxLayout;
  RelativeBtnGrp->setLayout(VBtnGrp1);

  RelativeRBn = new QRadioButton("Relative path",this);
  VBtnGrp1->addWidget(RelativeRBn);
  AbsoluteRBn = new QRadioButton("Absolute path",this);
  VBtnGrp1->addWidget(AbsoluteRBn);
//  TWQVB->addWidget(RelativeCHk);

  QWidget *RemoteBtnGrp = new QWidget(this);
  TWQVB->addWidget(RemoteBtnGrp);
  QVBoxLayout* VBtnGrp2 = new QVBoxLayout;
  RemoteBtnGrp->setLayout(VBtnGrp2);

  RemoteRBn = new QRadioButton("Remote",this);
  VBtnGrp2->addWidget(RemoteRBn);
  LocalRBn = new QRadioButton("Local",this);
  VBtnGrp2->addWidget(LocalRBn);

  PathSearchBTn = new QPushButton("Search",this);
  PathSearchBTn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

  QForm->addRow(TypeWDg,PathSearchBTn);
  QForm->setAlignment(PathSearchBTn,Qt::AlignCenter);

  VBL->addWidget(InferiorsTBv);

  QHBoxLayout* HBL=new QHBoxLayout;
  HBL->setAlignment(Qt::AlignRight);
  HBL->addWidget(AcceptBTn);
  HBL->addWidget(RejectBTn);

  VBL->addLayout(HBL);


  int wCol=0;
  InferiorsTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Sub-domain"));
  InferiorsTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Type"));
  InferiorsTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Content"));


  QObject::connect(PathSearchBTn,&QPushButton::clicked,this,&ZDomainDLg::PathSearchClicked);
  QObject::connect(AcceptBTn,&QPushButton::clicked,this,&ZDomainDLg::AcceptClicked);
  QObject::connect(RejectBTn,&QPushButton::clicked,this,&ZDomainDLg::RejectClicked);

  return ;
} // init()

void
ZDomainDLg::DomainNameChanged(const QString & pNewName)
{
    NameHasChanged=true;
}

void
ZDomainDLg::PathSearchClicked() {


}

void
ZDomainDLg::setupCreate(ZDomainPath* pFatherDomainPath,ZDomainPath* pDomainRoot)
{
    /*
    if ((pFatherDomainPath==nullptr)||(pFatherDomainPath==pDomainRoot)) {
        FatherNameLBl->setText("<Root>");
    }
    else {
        FatherNameLBl->setText(pFatherDomainPath->Name.toCChar());
    }
    */
    FatherNameLBl->setText(pFatherDomainPath->getName().toCChar());
    FatherHierarchyLBl->setText(pFatherDomainPath->getHierarchy().toCChar());

    Mode = MT_Create;
    DomainPathRef = new ZDomainPath;
    DomainPathRef->Father = pFatherDomainPath;

    NameLEd->setText("");
    ContentLEd->setText("");
    ToolTipLEd->setText("");

 //   PathCHk->setChecked(false);

    RelativeRBn->setChecked(false);
    RemoteRBn->setChecked(false);

    InferiorsTBv->ItemModel->removeRows(0,InferiorsTBv->ItemModel->rowCount());

}// ZDomainDLg::setupCreate

void
ZDomainDLg::setupChange(ZDomainPath* pDomainPath, ZDomainPath *pDomainRoot)
{
    if ((pDomainPath==nullptr)||(pDomainPath==pDomainRoot)) {
        return;
    }

    DomainPathRef = pDomainPath ;
    Mode = MT_Change;

    FatherNameLBl->setText(DomainPathRef->Father->getName().toCChar());
    FatherHierarchyLBl->setText(DomainPathRef->getHierarchy().toCChar());


    NameLEd->setText(DomainPathRef->Name.toCChar());

    QObject::connect(NameLEd,&QLineEdit::textChanged,this,&ZDomainDLg::DomainNameChanged);

    ContentLEd->setText(DomainPathRef->Content.toCChar());
    ToolTipLEd->setText(DomainPathRef->ToolTip.toCChar());

    uint32_t wType = DomainPathRef->Type & ZDOM_ObjectMask ;
    ObjectTypeCBx->setCurrentIndex(-1);
    for (int wi=0; TypeDecodeTable[wi].Type != ZDOM_End ; wi ++ )
        if (wType == TypeDecodeTable[wi].Type)
            ObjectTypeCBx->setCurrentIndex(wi);


    if ((DomainPathRef->Type & ZDOM_Path)==ZDOM_Path)
        ObjectTypeCBx->setCurrentIndex(0);
    else if ((DomainPathRef->Type & ZDOM_Icon)==ZDOM_Icon)
        ObjectTypeCBx->setCurrentIndex(1);

    RelativeRBn->setChecked(!(DomainPathRef->Type & ZDOM_Absolute));
    AbsoluteRBn->setChecked(DomainPathRef->Type & ZDOM_Absolute);
    RemoteRBn->setChecked(DomainPathRef->Type & ZDOM_Remote);
    LocalRBn->setChecked(!(DomainPathRef->Type & ZDOM_Remote));

    InferiorsTBv->ItemModel->removeRows(0,InferiorsTBv->ItemModel->rowCount());

    QList<QStandardItem*> wRow;

    for (int wi = 0 ; wi < DomainPathRef->Inferiors.count() ; wi++) {

        wRow = createZDomainSingleRow(DomainPathRef->Inferiors[wi],&ErrorLog);
    } // for

}// ZDomainDLg::setupCreate



void
ZDomainDLg::AcceptClicked()
{
/*    if ( Mode == MT_Create) {
        DomainPathRef->Father->Inferiors.push(DomainPathRef);
    }
*/
    utf8VaryingString wName = NameLEd->text().toUtf8().data();
/*    if (DomainPathRef->Name != wName)
            NameHasChanged = true;
*/
    DomainPathRef->Name = wName;
    if (DomainPathRef->Name==DomainPathRef->Father->Name) {
        ZExceptionDLg::adhocMessage("Domain node name",Severity_Error,
                                    "Domain node name <%s> cannot be the same of its superior.",DomainPathRef->Name.toString());
        NameLEd->setFocus();
        return;
    }
//    DomainPathRef->Father->Inferiors.push(DomainPathRef);  // this is made by callee
    DomainPathRef->ToolTip = ToolTipLEd->text().toUtf8().data();
    DomainPathRef->Content = ContentLEd->text().toUtf8().data();

    DomainPathRef->Type = ZDOM_Nothing;
    if (ObjectTypeCBx->currentIndex()>=0)
        DomainPathRef->Type = TypeDecodeTable[ObjectTypeCBx->currentIndex()].Type;
/*
    switch (ObjectTypeCBx->currentIndex())
    {
    case 0:
        DomainPathRef->Type = ZDOM_Path;
        break;
    case 1:
        DomainPathRef->Type = ZDOM_Icon;
        break;
    case 2:
        DomainPathRef->Type = ZDOM_Image;
        break;

    case 3:
        DomainPathRef->Type = ZDOM_TextFile;
        break;
    case 4:
        DomainPathRef->Type = ZDOM_PdfFile;
        break;
    case 5:
        DomainPathRef->Type = ZDOM_MasterFile;
        break;
    case 6:
        DomainPathRef->Type = ZDOM_HeaderFile;
        break;
    case 7:
        DomainPathRef->Type = ZDOM_Executable;
        break;
    case 8:
        DomainPathRef->Type = ZDOM_File;
        break;
    }// switch
*/
    if (AbsoluteRBn->isChecked())
        DomainPathRef->Type |= ZDOM_Absolute;
//    else
//        DomainPathRef->Type |= ZDOM_Relative;

    if (RemoteRBn->isChecked())
        DomainPathRef->Type |= ZDOM_Remote;

    accept();
}

void
ZDomainDLg::RejectClicked() {
    if ( Mode == MT_Create) {
        if (DomainPathRef!=nullptr)
            delete DomainPathRef;
    }
  reject();
}


void
ZDomainDLg::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QDialog::resize(pEvent->size().width(),pEvent->size().height());

  if (_FResizeInitial) {
    _FResizeInitial=false;
    return;
  }
  QRect wR1 = geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  InferiorsTBv->resize(wVW,wVH);  /* expands in width and height */

}//ZDomainDLg::resizeEvent

} // namespace zbs
