#include "zstdlistdlg.h"

#include <qevent.h>
#include <zqt/zqtwidget/zqstandarditemmodel.h>
#include <zqt/zqtwidget/zqstandarditem.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <QPersistentModelIndex>

#include <functional>

#include <QMenu>
#include <QAction>
#include <QActionGroup>


ZStdListDLg::ZStdListDLg(QWidget *pParent) : QDialog(pParent)//: ZQTreeView(pDialog)
{
  resize(900,360);
/*
  ArrayCHk = new QCheckBox(this);
  ArrayCHk->setObjectName("ArrayCHk");
  ArrayCHk->setText(QObject::tr("Check to create an array of atomic data type","ZTypeDLg"));
  ArrayCHk->setChecked(false);
  ArrayCHk->setGeometry(QRect(100,0,400,30));
*/
  QRect wRDlg=geometry();

  int wBtnY=wRDlg.y() - 22;

  AcceptBTn = new QPushButton(this);
  AcceptBTn->setObjectName("AcceptBTn");
  AcceptBTn->setText(QObject::tr("Accept","ZStdListDLg"));
  AcceptBTn->setGeometry(QRect(410,5,100,20));

  RejectBTn = new QPushButton(this);
  RejectBTn->setObjectName("RejectBTn");
  RejectBTn->setText(QObject::tr("Discard","ZStdListDLg"));
  RejectBTn->setGeometry(QRect(520,5,100,20));


//  QRect wR=ArrayCHk->geometry();

//  TreeHeightResizeMargin = wR.height()+1;
  TreeHeightResizeMargin = RejectBTn->height()+RejectBTn->y()+2;

  TableView = new ZQTableView(this);

  int wXpos=0;
  int wYpos= TreeHeightResizeMargin;
  int wWidth=wRDlg.width()-1;
  int wHeight=wRDlg.height()-TreeHeightResizeMargin;
  TableView->setGeometry(QRect(wXpos, wYpos,
                        wWidth,
                        wHeight));

  TableView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  tableViewSetup(true,2); /* 2 columns */

  QObject::connect(AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));
  QObject::connect(RejectBTn,SIGNAL(clicked()),this,SLOT(RejectClicked()));
}//ZStdListDLg::ZStdListDLg


void
ZStdListDLg::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

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

}//ZTypeDLg::resizeEvent

void
ZStdListDLg::AcceptClicked() {
  accept();
}
void
ZStdListDLg::RejectClicked() {
  reject();
}

void ZStdListDLg::AcceptCurrent(int pRow) {
  done(pRow);
  return;
}

void ZStdListDLg::ChangeCurrent(int pRow) {
  return;
}
void ZStdListDLg::InsertCurrent(int pRow) {

  return;
}

void ZStdListDLg::AppendCurrent() {

  return;
}
void ZStdListDLg::DeleteCurrent(int pRow) {
  TableView->ItemModel->removeRow(pRow);
  return;
}

void ZStdListDLg::AppendLine(const ZStdListDLgLine &pLine) {
  QList<QStandardItem*> wRow;
  int wCol=0;
  wRow.clear();
  while (wCol < pLine.count()) {
      wRow.push_back(createItem(pLine[wCol]->toCChar()));
      wCol++;
  }
  TableView->ItemModel->appendRow(wRow);
  return;
}

void ZStdListDLg::InsertLine( int pRow,const ZStdListDLgLine& pLine ) {
  QList<QStandardItem*> wRow;
  int wCol=0;
  wRow.clear();
  while (wCol < pLine.count()) {
    wRow.push_back(createItem(pLine[wCol]->toCChar()));
    wCol++;
  }
  TableView->ItemModel->insertRow(pRow,wRow);
  return;
}
void ZStdListDLg::ReplaceLine( int pRow,const ZStdListDLgLine& pLine ) {
  QList<QStandardItem*> wRow;
  int wCol=0;
  wRow.clear();
  while (wCol < pLine.count()) {
    /* Sets the item for the given row and column to item. The model takes ownership of the item.
     * If necessary, the row count and column count are increased to fit the item.
     * The previous item at the given location (if there was one) is deleted.
     */
    TableView->ItemModel->setItem(pRow,wCol,createItem(pLine[wCol]->toCChar()));
    wCol++;
  }
  return;
}
void ZStdListDLg::DeleteLine( int pRow ){
  TableView->ItemModel->removeRow(pRow);
}


void
ZStdListDLg::KeyFiltered(int pKey, QKeyEvent *pEvent)
{
  if(pKey == Qt::Key_Escape) {
    reject();
    return;
  }

  if(pKey == Qt::Key_Insert) {
    if (!TableView->currentIndex().isValid())
      return;
    InsertCurrent(TableView->currentIndex().row());
    return;
  }
  if(pKey == Qt::Key_Delete) {
    if (!TableView->currentIndex().isValid())
      return;
    DeleteCurrent(TableView->currentIndex().row());
    return;
  }
  return;
}
void
ZStdListDLg::MouseFiltered(int pKey,QMouseEvent* pEvent)
{
  switch (pKey) {
    case ZEF_DoubleClickLeft:
    {
      if (TableView->currentIndex().isValid())
        AcceptCurrent(TableView->currentIndex().row());
      return;
    }
  }// switch

  return;
}

void
ZStdListDLg::setupContextMenu() {
  if (popupMEn==nullptr) {
    popupMEn=new QMenu(this);

    popupGRp=new QActionGroup(popupMEn);

    insertQAc=popupGRp->addAction(popupMEn->addAction("Insert new"));
    appendQAc=popupGRp->addAction(popupMEn->addAction("Append new"));
    changeQAc=popupGRp->addAction(popupMEn->addAction("Change current"));
    deleteQAc=popupGRp->addAction(popupMEn->addAction("Delete current"));

    QObject::connect(popupGRp,SIGNAL(triggered(QAction*)), this, SLOT(generalActionEvent(QAction*)));
  }

}

void ZStdListDLg::tableViewSetup(bool pColumnAutoAdjust, int pColumns)
{
  //  FGeneric=true;
  int wFilter =  ZEF_Escape | ZEF_DoubleClickLeft ;

  if (pColumnAutoAdjust)
    wFilter |= ZEF_AutoAdjustColumn;
  //  GenericTRe = new ZQTreeView((QWidget *)this,ZEF_CommonControlKeys | ZEF_AllowDataEnter );
  //  keyTRv = new ZQTreeView(this,ZLayout_Generic, wFilter ); // no data enter

  TableView->setZLayout(ZLayout_TypeDLg);
  TableView->setFilterMask(wFilter);

  //  ui->centralwidget->addWidget(GenericTRv);

  TableView->setSizePolicy(QSizePolicy ::Preferred , QSizePolicy ::Preferred );

  TableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  TableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  TableView->setDragEnabled(true);
  TableView->setDropIndicatorShown(true);
  TableView->setDataEnter(true);

  //  GenericTRv->ItemModel=new QStandardItemModel(0,GenericModelColumn,this) ;
  TableView->setModel(new QStandardItemModel(0,pColumns,TableView));
  TableView->ItemModel->setSortRole(ZSortRole);     // Key to have a model sorted on numeric data and not editable data

  // registering callback function : this notation allows to be readable
  //
  std::function<void(int,int)>                                wKeyFilterCBF=nullptr;
  std::function<void(QContextMenuEvent *)>                    wContextMenuCBF=nullptr;
  std::function<bool (int,ZDataReference *,ZDataReference *)> wDropEventCBF=nullptr;  // no Drag and Drop

  /*
   wContextMenuCBF   = std::bind(&ZTypeClass::FlexMenu, this,_1);     // for context menu callback for ZQTreeView
  wDropEventCBF     = std::bind(&ZTypeClass::DropEvent, this,_1,_2);     // for drag and drop
*/
  TableView->_registerCallBacks(std::bind(&ZStdListDLg::KeyFiltered, this,std::placeholders::_1,std::placeholders::_2),
                                std::bind(&ZStdListDLg::MouseFiltered, this,std::placeholders::_1,std::placeholders::_2),
                                std::bind(&ZStdListDLg::ContextMenu, this,std::placeholders::_1),
                                nullptr);     /* no drop event callback */


  TableView->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  TableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  TableView->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  TableView->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  return ;
  //    PerDomainTRe->expandAll();
}//setupTableView

void ZStdListDLg::ContextMenu(QContextMenuEvent *event)
{
  if (popupMEn==nullptr)
    setupContextMenu();
  popupMEn->exec(event->globalPos());
}




ZRowCol ZStdListDLg::get() {
  return ZRowCol (TableView->currentIndex());
}


void ZStdListDLg::dataSetup(const ZStdListDLgLine &pTitle,
                            const ZStdListDLgContent &pContent)
{
//  QList<QStandardItem*> wRow;
  QVariant wV;

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  int wCol=0;
  while (wCol < pTitle.count()) {
    TableView->ItemModel->setHorizontalHeaderItem(wCol,createItem(QObject::tr(pTitle[wCol]->toCChar(),"ZStdListDLg").toUtf8().data()));
    wCol++;
  }

  for (long wi=0;wi < pContent.count();wi++) {
    appendRow(pContent[wi]);
  }

  TableView->resizeColumnsToContents();
  TableView->resizeRowsToContents();
}
void
ZStdListDLg::insertRow(const ZStdListDLgLine& pContent,int pRow) {
  QList<QStandardItem*> wRow;
  int wCol=0;
  while (wCol < pContent.count()) {
    wRow.push_back(createItem(pContent.TabConst(wCol)->toCChar()));
    wCol++;
  }
  TableView->ItemModel->insertRow(pRow,wRow);
  TableView->resizeColumnsToContents();
}

void
ZStdListDLg::appendRow(const ZStdListDLgLine& pContent) {
  QList<QStandardItem*> wRow;
  int wCol=0;
  while (wCol < pContent.count()) {
    wRow.push_back(createItem(pContent.TabConst(wCol)->toCChar()));
    wCol++;
  }
  TableView->ItemModel->appendRow(wRow);
//  TableView->resizeColumnsToContents();
}

void
ZStdListDLg::deleteRow(int pRow) {
  TableView->ItemModel->removeRow(pRow);
}

void ZStdListDLg::closeEvent(QCloseEvent *event)
{
//  TableView->deleteLater();
}



/* test current item and if not OK get children recursively */
QModelIndex
ZStdListDLg::_searchZType(QStandardItem* pItem,ZTypeBase pZType)
{
  QVariant wV;
  ZTypeBase wZType;
  QStandardItem* wItem1=nullptr;
  QModelIndex wIdx;

  if (pItem==nullptr)
    return QModelIndex();

  wV=pItem->data(ZQtValueRole);
  wZType = wV.value<ZTypeBase>();
  if (wZType==pZType)
    return pItem->index();

  int wi = 0;
  wItem1 = pItem->child(wi);
  if (wItem1 ==nullptr)
    return QModelIndex();

  while (wItem1 && (wi < pItem->rowCount()))
  {
    wIdx = _searchZType(wItem1,pZType);
    if (wIdx.isValid())
      return wIdx;

    wV=wItem1->data(ZQtValueRole);
    wZType = wV.value<ZTypeBase>();
    if (wZType==pZType)
      return wItem1->index();
    wi++;
    wItem1 = pItem->child(wi);
  }
  return QModelIndex();
}// _searchZType


QModelIndex
ZStdListDLg::searchZType(ZTypeBase pZType)
{
  QModelIndex wIdx ;

  QStandardItem* wItem= TableView->ItemModel->item(0,0) ;

  int wi = 0;
  while (wItem && (wi < wItem->rowCount()))
  {
    wItem= TableView->ItemModel->item(wi,0) ;
    wIdx=_searchZType(wItem,pZType);
    if (wIdx.isValid())
      return wIdx;
    wi++;
  }
  /* not found : search ZType_Unknown */

  while (wItem && (wi < wItem->rowCount()))
  {
    wItem= TableView->ItemModel->item(wi,0) ;
    wIdx=_searchZType(wItem,ZType_Unknown);
    if (wIdx.isValid())
      return wIdx;
    wi++;
  }

  return QModelIndex();
}//searchZType

void
ZStdListDLg::generalActionEvent(QAction* pAction) {
  if (pAction==changeQAc) {
    ChangeCurrent(TableView->currentIndex().row());
    return;
  }
  if (pAction==insertQAc) {
    InsertCurrent(TableView->currentIndex().row());
    return;
  }
  if (pAction==appendQAc) {
    AppendCurrent();
    return;
  }
  if (pAction==deleteQAc) {
    DeleteCurrent(TableView->currentIndex().row());
    return;
  }
}
