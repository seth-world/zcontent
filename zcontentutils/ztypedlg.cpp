#include "ztypedlg.h"
#include "zfielddlg.h"

#include <qevent.h>
#include <zqt/zqtwidget/zqstandarditemmodel.h>
#include <zqt/zqtwidget/zqstandarditem.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <zcontent/zcontentutils/ztypelistbutton.h>



struct ZTypeEditStruct
{
  ZTypeBase Value;
  const char* Name;
  const char* Tooltip;
};

ZTypeEditStruct ZTypeAtomicEdit[] = {
    {ZType_AtomicUChar,"ZType_AtomicUChar","unsigned char - mostly used for encoding/decoding"},
    {ZType_AtomicChar,"ZType_AtomicChar","char - a single ascii character - NOT SUBJECT to leading byte despite its signed status"},
    //    {ZType_AtomicWUChar,"ZType_AtomicWUChar","unsigned wchar_t (is a char and an uint16_t)"},
    //    {ZType_AtomicWChar,"ZType_AtomicWChar","wchar_t - a single double character "},
    {ZType_AtomicU8,"ZType_AtomicU8","arithmetic unsigned byte      not subject to Endian"},
    {ZType_AtomicS8,"ZType_AtomicS8","arithmetic signed byte      not subject to Endian"},
    {ZType_AtomicU16,"ZType_AtomicU16","arithmetic 2 bytes value unsigned (no leading sign byte)"},
    {ZType_AtomicS16,"ZType_AtomicS16","arithmetic 2 bytes value signed (with leading sign byte)"},
    {ZType_AtomicU32,"ZType_AtomicU32","arithmetic 4 bytes value unsigned (no leading sign byte)"},
    {ZType_AtomicS32,"ZType_AtomicS32","arithmetic 4 bytes value signed (no leading sign byte)"},
    {ZType_AtomicU64,"ZType_AtomicU64","arithmetic 8 bytes value unsigned (no leading sign byte)"},
    {ZType_AtomicS64,"ZType_AtomicS64","arithmetic 8 bytes value signed (with leading sign byte)"},
    {ZType_AtomicFloat,"ZType_AtomicFloat","floating point (therefore signed)"},
    {ZType_AtomicDouble,"ZType_AtomicDouble","floating point double (therefore signed)"},
    {ZType_AtomicLDouble,"ZType_AtomicLDouble","floating point long double (therefore signed)"},
    {ZType_Nothing,"",""}
};
ZTypeEditStruct ZTypeVStringEdit[] = {
    {ZType_CharVaryingString,"ZType_CharVaryingString","a varying string of char- prefer using utf8 string."},
    {ZType_Utf8VaryingString,"ZType_Utf8VaryingString","a varying string of utf8 characters."},
    {ZType_Utf16VaryingString,"ZType_Utf16VaryingString","a varying string of utf16 characters. "},
    {ZType_Utf32VaryingString,"ZType_Utf32VaryingString","a varying string of utf32 characters. "},
    {ZType_URIString,"ZType_URIString","a base of utf8 varying string with URI capabilities"},
    {ZType_StdString,"ZType_StdString","a std::string class varying length : not used for storage, only for conversion"},
    {ZType_Nothing,"",""}
};
ZTypeEditStruct ZTypeFStringEdit[] = {
    {ZType_CharFixedString,"ZType_CharFixedString","a fixed string of char characters. Prefer using utf8 fixed string. "},
    {ZType_Utf8FixedString,"ZType_Utf8FixedString","a fixed string of utf8 characters. "},
    {ZType_Utf16FixedString,"ZType_Utf16FixedString","a fixed string of utf16 characters. "},
    {ZType_Utf32FixedString,"ZType_Utf32FixedString","a fixed string of utf32 characters. "},
    {ZType_Nothing,"",""}
};
ZTypeEditStruct ZTypeOtherEdit[] = {
    {ZType_bitset,"ZType_bitset","bitset is a set of atomic data 'bit'"},
    {ZType_ZDate,"ZType_ZDate","a struct giving uint32_t  is ZDate (export and import)"},
    {ZType_ZDateFull,"ZType_ZDateFull","a struct giving uint64_t  is ZDateFull (export and import)"},
    {ZType_CheckSum,"ZType_CheckSum","a struct containing fixed array of unsigned char"},
    {ZType_Resource,"ZType_Resource","arithmetic 2 bytes value unsigned (no leading sign byte)"},
    {ZType_Unknown,"ZType_Unknown","this type is not registered and is unknown"},
    {ZType_Nothing,"",""}
};





ZTypeDLg::ZTypeDLg(QWidget *pParent) : QDialog(pParent)//: ZQTreeView(pDialog)
{
  resize(900,361);

  ArrayCHk = new QCheckBox(this);
  ArrayCHk->setObjectName("ArrayCHk");
  ArrayCHk->setText(QObject::tr("Check to create an array of atomic data type","ZTypeDLg"));
  ArrayCHk->setChecked(false);
  ArrayCHk->setGeometry(QRect(100,0,400,30));

  AcceptBTn = new QPushButton(this);
  AcceptBTn->setObjectName("AcceptBTn");
  AcceptBTn->setText(QObject::tr("Accept","ZTypeDLg"));
  AcceptBTn->setGeometry(QRect(410,5,100,20));

  RejectBTn = new QPushButton(this);
  RejectBTn->setObjectName("RejectBTn");
  RejectBTn->setText(QObject::tr("Discard","ZTypeDLg"));
  RejectBTn->setGeometry(QRect(520,5,100,20));

  QRect wRDlg=geometry();
  QRect wR=ArrayCHk->geometry();

  TreeHeightResizeMargin = wR.height()+1;

  TreeView = new ZQTreeView(this);

  int wXpos=0;
  int wYpos= TreeHeightResizeMargin;
  int wWidth=wRDlg.width()-1;
  int wHeight=wRDlg.height()-TreeHeightResizeMargin;
  TreeView->setGeometry(QRect(wXpos, wYpos,
                        wWidth,
                        wHeight));

  TreeView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  setupTableView(true,2); /* 2 columns */

  int wCol=0;

  TreeView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Code","ZTypeDLg").toUtf8().data()));
  TreeView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Symbol","ZTypeDLg").toUtf8().data()));
  ZQStandardItem::setItemAlignment(Qt::AlignLeft);
  TreeView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Description","ZTypeDLg").toUtf8().data()));
  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  dataSetup();

  QObject::connect(AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));
  QObject::connect(RejectBTn,SIGNAL(clicked()),this,SLOT(RejectClicked()));
}


void
ZTypeDLg::resizeEvent(QResizeEvent* pEvent)
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

  TreeView->resize(wVW,wVH);  /* expands in width and height */

}//ZTypeDLg::resizeEvent

void
ZTypeDLg::AcceptClicked() {
  AcceptCurrent();
  accept();
}
void
ZTypeDLg::RejectClicked() {
  reject();
}

bool ZTypeDLg::AcceptCurrent() {
  QModelIndex wIdx= TreeView->currentIndex();
  if(!wIdx.isValid())
    return false ;
  if (wIdx.column()!=0)
    wIdx=wIdx.siblingAtColumn(0);
  //      wIdx=ItemModel->index(currentIndex().row(),0);
  if(!wIdx.isValid())
    return false ;
  if (getValueFromItem<ZTypeBase>(wIdx,ZType)) {
    accept();
  }
  return true;
}


bool
ZTypeDLg::KeyFiltered(int pKey, QKeyEvent *pEvent)
{
  if(pKey == Qt::Key_Escape)
  {
    reject();
    return true;
  }

  if(pKey == Qt::Key_Insert)
  {
    return true;;
  }
  if(pKey == Qt::Key_Delete)
  {
    //    deleteGeneric();
    return true;;
  }

  return true;;
}
bool
ZTypeDLg::MouseFiltered(int pKey,QMouseEvent* pEvent)
{
  switch (pKey)
  {
  case ZEF_DoubleClickLeft:
  {
    return AcceptCurrent();
  }
    //  case ZEF_SingleClickLeft:
  }

  return false;
}

void ZTypeDLg::setupTableView(bool pColumnAutoAdjust, int pColumns)
{
  //  FGeneric=true;
  int wFilter =  ZEF_Escape | ZEF_DoubleClickLeft ;

  if (pColumnAutoAdjust)
    wFilter |= ZEF_AutoAdjustColumn;
  //  GenericTRe = new ZQTreeView((QWidget *)this,ZEF_CommonControlKeys | ZEF_AllowDataEnter );
  //  keyTRv = new ZQTreeView(this,ZLayout_Generic, wFilter ); // no data enter

  TreeView->setZLayout(ZLayout_TypeDLg);
  TreeView->setFilterMask(wFilter);

  //  ui->centralwidget->addWidget(GenericTRv);

  TreeView->setSizePolicy(QSizePolicy ::Preferred , QSizePolicy ::Preferred );

  TreeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  TreeView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  TreeView->setDragEnabled(true);
  TreeView->setDropIndicatorShown(true);
  TreeView->setDataEnter(true);

  //  GenericTRv->ItemModel=new QStandardItemModel(0,GenericModelColumn,this) ;
  TreeView->setModel(new QStandardItemModel(0,pColumns,TreeView));
  TreeView->ItemModel->setSortRole(ZSortRole);     // Key to have a model sorted on numeric data and not editable data

  // registering callback function : this notation allows to be readable
  //
  std::function<void(int,int)>                                wKeyFilterCBF=nullptr;
  std::function<void(QContextMenuEvent *)>                    wContextMenuCBF=nullptr;
  std::function<bool (int,ZDataReference *,ZDataReference *)> wDropEventCBF=nullptr;  // no Drag and Drop

  /*
   wContextMenuCBF   = std::bind(&ZTypeClass::FlexMenu, this,_1);     // for context menu callback for ZQTreeView
  wDropEventCBF     = std::bind(&ZTypeClass::DropEvent, this,_1,_2);     // for drag and drop
*/
  TreeView->_register(std::bind(&ZTypeDLg::KeyFiltered, this,_1,_2),
                  std::bind(&ZTypeDLg::MouseFiltered, this,_1,_2),
                  nullptr,      /* no contextual menu */
                  nullptr);     /* no drop event callback */


  TreeView->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  TreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  TreeView->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  TreeView->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  return ;

  //    PerDomainTRe->expandAll();
}//setupTableView


void ZTypeDLg::set(ZTypeBase pZType)
{
  ZType=pZType;

  ZTypeBase wZType = pZType &= ~ZType_Array;

  QModelIndex wIdx=searchZType(wZType);
  if (!wIdx.isValid())
    return;

  ArrayCHk->setChecked( pZType & ZType_Array);

  TreeView->setCurrentIndex(wIdx);
}


ZTypeBase ZTypeDLg::get() {
  if (ArrayCHk->isChecked())
    return ZType | ZType_Array;
  return ZType;
}


void ZTypeDLg::dataSetup()
{
  QList<QStandardItem*> wTypeRow;
  QVariant wV;
  QStandardItem* wItem=nullptr;

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  wItem=createItem("Atomic data types");
  TreeView->ItemModel->appendRow(wItem);

  for(int wi=0;ZTypeAtomicEdit[wi].Value!=ZType_Nothing;wi++ )
    wItem->appendRow(createItemRow(ZTypeAtomicEdit[wi]));

  wItem=createItem("Varying string types");
  TreeView->ItemModel->appendRow(wItem);

  for(int wi=0;ZTypeVStringEdit[wi].Value!=ZType_Nothing;wi++ )
    wItem->appendRow(createItemRow(ZTypeVStringEdit[wi]));

  wItem=createItem("Fixed string types");
  TreeView->ItemModel->appendRow(wItem);

  for(int wi=0;ZTypeFStringEdit[wi].Value!=ZType_Nothing;wi++ )
    wItem->appendRow(createItemRow(ZTypeFStringEdit[wi]));


  wItem=createItem("Class object types");
  TreeView->ItemModel->appendRow(wItem);

  for(int wi=0;ZTypeOtherEdit[wi].Value!=ZType_Nothing;wi++ ) {
    wItem->appendRow(createItemRow(ZTypeOtherEdit[wi]));
  }

  TreeView->expandAll();
  for (int wi=0;wi < 3;wi++)
    TreeView->resizeColumnToContents(wi);
}

void ZTypeDLg::closeEvent(QCloseEvent *event)
{
  TreeView->deleteLater();
}



/* test current item and if not OK get children recursively */
QModelIndex
ZTypeDLg::_searchZType(QStandardItem* pItem,ZTypeBase pZType)
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
ZTypeDLg::searchZType(ZTypeBase pZType)
{
  QModelIndex wIdx ;

  QStandardItem* wItem= TreeView->ItemModel->item(0,0) ;

  int wi = 0;
  while (wItem && (wi < wItem->rowCount()))
  {
    wItem= TreeView->ItemModel->item(wi,0) ;
    wIdx=_searchZType(wItem,pZType);
    if (wIdx.isValid())
      return wIdx;
    wi++;
  }
  /* not found : search ZType_Unknown */

  while (wItem && (wi < wItem->rowCount()))
  {
    wItem= TreeView->ItemModel->item(wi,0) ;
    wIdx=_searchZType(wItem,ZType_Unknown);
    if (wIdx.isValid())
      return wIdx;
    wi++;
  }

  return QModelIndex();
}//searchZType

QList<QStandardItem*> createItemRow (ZTypeEditStruct &pTypeStr)
{
  QList<QStandardItem*> wTypeRow;
  QVariant wV;
  wTypeRow.clear();

  wTypeRow << createItem(pTypeStr.Value,"0x%08X");
  wV.setValue<ZTypeBase>((const int)pTypeStr.Value);
  wTypeRow[0]->setData(wV,ZQtValueRole);
  wTypeRow[0]->setToolTip(pTypeStr.Tooltip);

  wTypeRow << createItem(pTypeStr.Name);
  //  ZQStandardItem::setItemAlignment(Qt::AlignLeft);
  wTypeRow << createItem(pTypeStr.Tooltip);
  wTypeRow.last()->setData((Qt::TextWordWrap|Qt::AlignLeft),Qt::TextAlignmentRole);
  //  ZQStandardItem::setItemAlignment(Qt::AlignRight);
  return wTypeRow;
}
