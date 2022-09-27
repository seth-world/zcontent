#include "zdicdlg.h"
#include "zfielddlg.h"

#include <qevent.h>
#include <zqt/zqtwidget/zqstandarditemmodel.h>
#include <zqt/zqtwidget/zqstandarditem.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <zcontent/zcontentutils/ztypelistbutton.h>
#include <QPushButton>
#include <zexceptiondlg.h>

#include <QLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>

const int cst_ActiveFlag = 1;
const int cst_CreationDate = 4;
const int cst_ModificationDate = 5;

ZDicDLg::ZDicDLg(QWidget *pParent) : QDialog(pParent)//: ZQTreeView(pDialog)
{
  init();
}

ZDicDLg::ZDicDLg(const uriString& pDicFile, QWidget *pParent) : QDialog(pParent)//: ZQTreeView(pDialog)
{
  init();
  setDicFile(pDicFile);
  dataSetup();
}
void
ZDicDLg::init()
{
  resize(900,361);

  AcceptBTn = new QPushButton(this);
  AcceptBTn->setObjectName("AcceptBTn");
  AcceptBTn->setText(QObject::tr("Accept","ZDicDLg"));
  AcceptBTn->setGeometry(QRect(410,5,100,20));

  RejectBTn = new QPushButton(this);
  RejectBTn->setObjectName("RejectBTn");
  RejectBTn->setText(QObject::tr("Discard","ZDicDLg"));
  RejectBTn->setGeometry(QRect(520,5,100,20));

  QRect wRDlg=geometry();
/*  QRect wR=ArrayCHk->geometry();

  TreeHeightResizeMargin = wR.height()+1;
*/
  QRect wR=AcceptBTn->geometry();
  TreeHeightResizeMargin = wR.height() + 10 ;
  TableView = new ZQTableView(this);

  int wXpos=0;
  int wYpos= TreeHeightResizeMargin;
  int wWidth=wRDlg.width()-1;
  int wHeight=wRDlg.height()-TreeHeightResizeMargin;
  TableView->setGeometry(QRect(wXpos, wYpos,
      wWidth,
      wHeight));

  TableView->setModel(new QStandardItemModel(0,6,TableView)); /* 6 columns */

  TableView->ItemModel->setSortRole(ZSortRole);     // Key to have a model sorted on numeric data and not editable data
  TableView->_registerCallBacks(std::bind(&ZDicDLg::KeyFiltered, this,_1,_2),
      std::bind(&ZDicDLg::MouseFiltered, this,_1,_2),
      nullptr,      /* no contextual menu */
      nullptr);     /* no drop event callback */

  TableView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  TableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  TableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  TableView->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  TableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  TableView->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  TableView->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  TableView->setModel(new QStandardItemModel(0,5,TableView));

  TableView->setZLayout(ZLayout_FieldTBv);
  TableView->setFilterMask(ZEF_Escape | ZEF_CommonControlKeys | ZEF_DoubleClick);
  TableView->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );

  int wCol=0;
  TableView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Name","ZDicDLg").toUtf8().data()));
  TableView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Status","ZDicDLg").toUtf8().data()));
  ZQStandardItem::setItemAlignment(Qt::AlignLeft);
  TableView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Version","ZDicDLg").toUtf8().data()));
  TableView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Keys","ZDicDLg").toUtf8().data()));
  TableView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Created","ZDicDLg").toUtf8().data()));
  TableView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Released","ZDicDLg").toUtf8().data()));

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  TableView->setShowGrid(true);
  TableView->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
  TableView->setWordWrap(false);


  QObject::connect(AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));
  QObject::connect(RejectBTn,SIGNAL(clicked()),this,SLOT(RejectClicked()));

  return ;
} // init()


ZDicDLg::~ZDicDLg() {
  if (DicFile)
    if (DicFile->isOpen()) {
      DicFile->zclose();
      delete DicFile;
    }
  TableView->deleteLater();
  AcceptBTn->deleteLater();
}

ZStatus
ZDicDLg::setDicFile(const uriString& pDicFile) {
//  ZDicHeaderList wDicHeaderList;
  DicFile=new ZDictionaryFile();
  ZStatus wSt=DicFile->zopen(pDicFile,ZRF_Modify);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wSt= DicFile->getAllDictionaries(DicList);
  if (wSt!=ZS_EOF) {
    if (wSt==ZS_EMPTY){
      ZExceptionDLg::adhocMessage("Dictionaries load",Severity_Warning,nullptr,
          "Dictionary file <%s> is empty.",pDicFile.getBasename().toCChar());
    }
    else{
    ZExceptionDLg::messageWAdd("Dictionaries load",ZException.last().Status,ZException.last().Severity,
        ZException.last().formatUtf8(),"Cannot load dictionaries from file <%s>.",pDicFile.getBasename().toCChar());
    }
  }
  DataChanged=false;
  return wSt;
}// setDicFile


void
ZDicDLg::resizeEvent(QResizeEvent* pEvent)
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

  TableView->resize(wVW,wVH);  /* expands in width and height */

}//ZDicDLg::resizeEvent

void
ZDicDLg::AcceptClicked() {
  if (DataChanged){
  if (update() != ZS_SUCCESS)
    reject();
  }
  if (!TableView->currentIndex().isValid()) {
    ZExceptionDLg::adhocMessage(QCoreApplication::translate("ZDicDLg","Dictionary").toUtf8().data(),Severity_Error,nullptr,
        QCoreApplication::translate("ZDicDLg","Please select a dictionary").toUtf8().data());
    return;
  }
  accept();
}

void
ZDicDLg::RejectClicked() {
  reject();
}

bool
ZDicDLg::getDicActive(long pRank) {

  utf8VaryingString wDicName ;
  if (DicList[pRank]->DicName.isEmpty())
    wDicName="<no name>";
  else
    wDicName=DicList[pRank]->DicName;
  utf8VaryingString wVersion=getVersionStr(DicList[pRank]->Version);

  QDialog wDicNameDLg;
  wDicNameDLg.setObjectName("wDicActiveDLg");
  wDicNameDLg.setWindowTitle(QObject::tr("Dictionary status","DicEdit"));
  wDicNameDLg.resize(400,150);

  QVBoxLayout* QVL=new QVBoxLayout(&wDicNameDLg);
  wDicNameDLg.setLayout(QVL);

  QVL->setObjectName("QVL");
  QHBoxLayout* QHL=new QHBoxLayout;
  QHL->setObjectName("QHL");
  QVL->insertLayout(0,QHL);
  QLabel* wLb=new QLabel(QObject::tr("Dictionary name","DicEdit"),&wDicNameDLg);
  wLb->setObjectName("wLb");
  QHL->addWidget(wLb);
  QLineEdit* wDicNameLEd=new QLineEdit(wDicName.toCChar(),&wDicNameDLg);
  wDicNameLEd->setObjectName("wDicNameLEd");
  wDicNameLEd->setReadOnly(true);
  QHL->addWidget(wDicNameLEd);

  QHBoxLayout* QHL1=new QHBoxLayout;
  QHL1->setObjectName("QHL1");
  QVL->insertLayout(1,QHL1);
  QLabel* wLb1=new QLabel(QObject::tr("Version number","DicEdit"),&wDicNameDLg);
  wLb->setObjectName("wLb1");
  QHL1->addWidget(wLb1);
  QLineEdit* wVersionLEd=new QLineEdit(wVersion.toCChar());
  wVersionLEd->setObjectName("wVersionLEd");
  wVersionLEd->setReadOnly(true);
  QHL1->addWidget(wVersionLEd);
  QCheckBox* wActiveCHk=new QCheckBox("Active version",&wDicNameDLg);
  QHL1->addWidget(wActiveCHk);
  wActiveCHk->setChecked(DicList[pRank]->Active);

  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QHLBtn->setObjectName("QHLBtn");
  QVL->insertLayout(2,QHLBtn);

  QPushButton* wOk=new QPushButton(QObject::tr("Ok","DicEdit"),&wDicNameDLg);
  wOk->setObjectName("wOk");
  QPushButton* wCancel=new QPushButton(QObject::tr("Cancel","DicEdit"),&wDicNameDLg);
  wCancel->setObjectName("wCancel");
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHLBtn->addItem(wSpacer);

  QHLBtn->addWidget(wCancel);
  QHLBtn->addWidget(wOk);

  wDicNameLEd->setText(wDicName.toCChar());
//  wDicNameLEd->setSelection(0,wDicName.UnitCount);

  wDicNameDLg.setWindowTitle(QObject::tr("Dictionary identification","DicEdit"));

  QObject::connect(wOk, &QPushButton::clicked, &wDicNameDLg, &QDialog::accept);
  QObject::connect(wCancel, &QPushButton::clicked, &wDicNameDLg, &QDialog::reject);

  int wRet=wDicNameDLg.exec();
  if (wRet==QDialog::Rejected)
    return false;

//  if (wActiveCHk->isChecked()== DicList[pRank]->Active)
//    return false;  /* no change */

  if (wActiveCHk->isChecked()) {
    DataChanged=true;
    /* reset all active versions to inactive (theorically only one version is Active) */
    for (long wi=0; wi < DicList.count() ; wi++) {
      if (wi==pRank)
        continue;
      if (DicList[wi]->Active) {
        ZDicEvent wEvent (DicList[wi],nullptr,wi,ZDET_Change);
        DicList[wi]->Active = false;
        DicList[pRank]->ModificationDate = ZDateFull::currentDateTime();
        wEvent.setAfter(DicList[wi]);
        DicEvents.push(wEvent);
        TableView->ItemModel->item(int(wi),cst_ActiveFlag)->setText("");
        TableView->ItemModel->item(int(wi),cst_ModificationDate)->setText(DicList[wi]->ModificationDate.toFormatted().toCChar());
      }
    }// for

    /* set current version as active */
    ZDicEvent wEvent (DicList[pRank],nullptr,pRank,ZDET_Change);
    DicList[pRank]->Active = true;
    DicList[pRank]->ModificationDate = ZDateFull::currentDateTime();

    wEvent.setAfter(DicList[pRank]);
    DicEvents.push(wEvent);

    TableView->ItemModel->item(int(pRank),cst_ActiveFlag)->setText("Active");
    TableView->ItemModel->item(int(pRank),cst_ModificationDate)->setText(DicList[pRank]->ModificationDate.toFormatted().toCChar());
    DataChanged=true;
    return true;
  } // if (wActiveCHk->isChecked())

  /* here active becomes inactive : no search for active, simply set current to inactive */
  ZDicEvent wEvent (DicList[pRank],nullptr,pRank,ZDET_Change);
  DicList[pRank]->Active = false;
  DicList[pRank]->ModificationDate = ZDateFull::currentDateTime();
  wEvent.setAfter(DicList[pRank]);
  DicEvents.push(wEvent);
  TableView->ItemModel->item(int(pRank),cst_ActiveFlag)->setText("");
  DataChanged=true;
  return true;
} // getDicActive


void
ZDicDLg::KeyFiltered(int pKey, QKeyEvent *pEvent) {
  if(pKey == Qt::Key_Escape) {
    reject();
    return;
  }

  if(pKey == Qt::Key_Insert) {
    return;
  }

  if(pKey == Qt::Key_Delete) {
    if (!TableView->currentIndex().isValid())
      return;

    deleteDic(TableView->currentIndex().row());
    //    deleteGeneric();
    return;
  }
  return;
}// KeyFiltered

void
ZDicDLg::MouseFiltered(int pKey,QMouseEvent* pEvent) {
  switch (pKey) {
  case ZEF_DoubleClickLeft:
  {
    if (!TableView->currentIndex().isValid())
      return;
    getDicActive(TableView->currentIndex().row());
    return;
  }
    //  case ZEF_SingleClickLeft:
  }// switch

  return;
} // MouseFiltered

void
ZDicDLg::deleteDic(int pRow) {
  TableView->ItemModel->removeRow(pRow);
  DicEvents.push(ZDicEvent(DicList[pRow],nullptr,pRow,ZDET_Deleted));
  DicList.erase(long(pRow));
  DataChanged=true;

  return;
}

void ZDicDLg::set(const utf8VaryingString pDicName,unsigned long Version) {
  for (long wi=0; wi < DicList.count() ; wi ++) {
    if ((DicList[wi]->DicName==pDicName)&&(DicList[wi]->Version==Version)){
      TableView->setCurrentIndex(TableView->ItemModel->index(wi,0));
      return;
    }
  }
  return;
}


int ZDicDLg::getRank() {
  ZStatus wSt;
  if (DataChanged){
    wSt=update();
  }
  return TableView->currentIndex().row();
}

uriString ZDicDLg::getDicFileURI() {
  return DicFile->getURIContent();
}

void ZDicDLg::dataSetup()
{
  QList<QStandardItem*> wTypeRow;
  QVariant wV;
  QStandardItem* wItem=nullptr;

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  TableView->ItemModel->removeRows(0,TableView->ItemModel->rowCount());
  for (long wi=0;wi < DicList.count() ; wi++){
    QList<QStandardItem*> wRow ;
    wRow << createItem(DicList[wi]->DicName);
    wRow << createItem(DicList[wi]->Active?"Active":"");
    wRow << createItem(getVersionStr( DicList[wi]->Version ));
    wRow << createItem(DicList[wi]->KeyDic.count());
    wRow << createItem(DicList[wi]->CreationDate.toFormatted().toCChar());
    wRow << createItem(DicList[wi]->ModificationDate.toFormatted().toCChar());
    TableView->ItemModel->appendRow(wRow);
  }
  for (int wi=0;wi < TableView->ItemModel->columnCount();wi++) {
    TableView->resizeColumnToContents(wi);
  }
  for (int wi=0;wi < TableView->ItemModel->rowCount();wi++) {
    TableView->resizeRowToContents(wi);
  }
}//dataSetup

void ZDicDLg::closeEvent(QCloseEvent *event)
{
  if (DataChanged) {
    int wRet= ZExceptionDLg::adhocMessage2B("Data has changed",Severity_Warning,"Do not save","Save",nullptr,
        "One or more dictionary data has changed.\n"
        "Do you want to save <Save> or loose changes <Do not save> ?");
    if (wRet==QDialog::Rejected)
      return;
    update();
  }
}

void
ZDicDLg::replace() {
  /* first delete all dictionaries from file */

  ZDataBuffer wB;
  ZStatus wSt ;

  while (wSt==ZS_SUCCESS){
    wSt=DicFile->zremove(0L);
  }
  /* create all dictionaries into file */
  for (long wi = 0; wi < DicList.count() ; wi ++) {
    ZDataBuffer wDicBuf;
    DicList[wi]->_exportAppend(wDicBuf);
    wSt=DicFile->zadd(wDicBuf);
  }
  DataChanged=false;
}//save

ZStatus
ZDicDLg::update() {

  ZDataBuffer wB;
  ZStatus wSt = ZS_SUCCESS ;

  for (long wi = 0; (wi < DicEvents.count()) && (wSt==ZS_SUCCESS); wi ++) {
    switch (DicEvents[wi].Type){
    case ZDET_Deleted:
      wSt=DicFile->zremove(DicEvents[wi].Rank);
      break;
    case ZDET_Change:
      wB.clear();
      DicEvents[wi].DataAfter->_exportAppend(wB);
      wSt=DicFile->zreplace(wB, DicEvents[wi].Rank) ;
      break;
    case ZDET_Created:
      wB.clear();
      DicEvents[wi].DataAfter->_exportAppend(wB);
      wSt=DicFile->zadd(wB) ;
      break;
    }// switch
  }// for
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::messageWAdd("Dictionary update",wSt,ZException.last().Severity,
        ZException.last().formatUtf8(),
        "Error while updating Dictionary File <%s> ");
    return wSt;
  }
  DataChanged=false;
  return wSt;
}//save
