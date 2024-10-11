#include "ztokentypedlg.h"

#include <qevent.h>
#include <zqt/zqtwidget/zqstandarditemmodel.h>
#include <zqt/zqtwidget/zqstandarditem.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include "zfielddlg.h"

struct ZTypeEditStruct
{
  ZTypeBase Value;
  const char* Name;
  const char* Tooltip;
};
#ifdef __COMMENT__

    ZTTP_ARRAY                    =  0x40000000,

    ZTTP_CLASS                    = 0x00010110,

    ZTTP_STRUCT                   = 0x00010210,

    ZTTP_ENUM                     = 0x00010310,
    ZTTP_ENUM_WITHOUT_ID          = 0x00010330,    /* field declaration without enum id (cannot be registered) */
    ZTTP_ENUM_OPAQUE              = 0x00010314,   /* opaque declaration : enum must be registrated */
    ZTTP_ENUM_CLASS               = 0x00010400,



    /* atomic data types */
    ZTTP_UNKNOWN_TYPE     = 0xF000012000,

    ZTTP_NOTHING = 0

    ZTTP_LONG

    ZTTP_CHAR             = 0x00011003,
    ZTTP_INTEGER          = ZTTP_CHAR + 1,
    ZTTP_LONG_LONG        = ZTTP_INTEGER + 1,
    ZTTP_FLOAT            = ZTTP_LONG_LONG + 1,
    ZTTP_DOUBLE           = ZTTP_FLOAT + 1,
    ZTTP_LONG_DOUBLE      = ZTTP_DOUBLE | ZTTP_LONG,
    ZTTP_BOOL             = ZTTP_DOUBLE + 1,

    //      ZTTP_LONG_DOUBLE      = ZTTP_DOUBLE + 1,

    ZTTP_INT8             = 0x00011101,
    ZTTP_INT16            = ZTTP_INT8+1,    /* 0x00011102 */
    ZTTP_INT32            = ZTTP_INT16+1,   /* 0x00011103 */
    ZTTP_INT64            = ZTTP_INT32+1,   /* 0x00011104 */

    /* methods and functions */


    ZTTP_UTF8VARYINGSTRING = 0x10000011,
    ZTTP_UTF16VARYINGSTRING= 0x10000012,
    ZTTP_UTF32VARYINGSTRING= 0x10000013,

    ZTTP_URISTRING         = 0x10000031,

    ZTTP_UTF8FIXEDSTRING   = 0x10000001,
    ZTTP_UTF16FIXEDSTRING  = 0x10000002,
    ZTTP_UTF32FIXEDSTRING  = 0x10000003,

    ZTTP_ZDATEFULL         = 0x10000100,
    ZTTP_ZDATE             = 0x10000200,

    ZTTP_MD5                = 0x10001000,
    ZTTP_CHECKSUM           = 0x10002000,

    ZTTP_ZDATABUFFER        = 0x10004000,

    ZTTP_ZBITSET            = 0x10007000,

    ZTTP_ZRESOURCE          = 0x10005000,
    ZTTP_ZRESOURCECODE      = 0x10006000,
    ZTTP_STDSTRING          = 0x20000000
#endif // __COMMENT__

ZTypeEditStruct ZTokenTypeEdit[] = {
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





ZTokenTypeDLg::ZTokenTypeDLg(ZFieldDLg *pParent) : QDialog(pParent)//: ZQTreeView(pDialog)
{
  resize(900,361);

  ArrayCHk = new QCheckBox(this);
  ArrayCHk->setObjectName("ArrayCHk");
  ArrayCHk->setText(QObject::tr("Check to create an array of atomic data type","ZTokenTypeDLg"));
  ArrayCHk->setChecked(false);
  ArrayCHk->setGeometry(QRect(100,0,400,30));

  AcceptBTn = new QPushButton(this);
  AcceptBTn->setObjectName("AcceptBTn");
  AcceptBTn->setText(QObject::tr("Accept","ZTokenTypeDLg"));
  AcceptBTn->setGeometry(QRect(410,5,100,20));

  RejectBTn = new QPushButton(this);
  RejectBTn->setObjectName("RejectBTn");
  RejectBTn->setText(QObject::tr("Discard","ZTokenTypeDLg"));
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

  TreeView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Code","ZTokenTypeDLg").toUtf8().data()));
  TreeView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Symbol","ZTokenTypeDLg").toUtf8().data()));
  ZQStandardItem::setItemAlignment(Qt::AlignLeft);
  TreeView->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Description","ZTokenTypeDLg").toUtf8().data()));
  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  dataSetup();

  QObject::connect(AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));
  QObject::connect(RejectBTn,SIGNAL(clicked()),this,SLOT(RejectClicked()));
}


void
ZTokenTypeDLg::resizeEvent(QResizeEvent* pEvent)
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

}//ZTokenTypeDLg::resizeEvent

void
ZTokenTypeDLg::AcceptClicked() {
  AcceptCurrent();
  accept();
}
void
ZTokenTypeDLg::RejectClicked() {
  reject();
}

void ZTokenTypeDLg::AcceptCurrent() {
  QModelIndex wIdx= TreeView->currentIndex();
  if(!wIdx.isValid())
    return;
  if (wIdx.column()!=0)
    wIdx=wIdx.siblingAtColumn(0);
  //      wIdx=ItemModel->index(currentIndex().row(),0);
  if(!wIdx.isValid())
    return;
  if (getValueFromItem<ZTypeBase>(wIdx,ZType)) {
    accept();
  }
  return;
}


void
ZTokenTypeDLg::KeyFiltered(int pKey, QKeyEvent *pEvent)
{
  if(pKey == Qt::Key_Escape)
  {
    reject();
    return;
  }

  if(pKey == Qt::Key_Insert)
  {
    return;
  }
  if(pKey == Qt::Key_Delete)
  {
    //    deleteGeneric();
    return;
  }

  return;
}
void
ZTokenTypeDLg::MouseFiltered(int pKey,QMouseEvent* pEvent)
{
  switch (pKey)
  {
  case ZEF_DoubleClickLeft:
  {
    AcceptCurrent();
    return;
  }
    //  case ZEF_SingleClickLeft:
  }

  return;
}

void ZTokenTypeDLg::setupTableView(bool pColumnAutoAdjust, int pColumns)
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
  TreeView->_register(std::bind(&ZTokenTypeDLg::KeyFiltered, this,_1,_2),
                  std::bind(&ZTokenTypeDLg::MouseFiltered, this,_1,_2),
                  nullptr,      /* no contextual menu */
                  nullptr);     /* no drop event callback */


  TreeView->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  TreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  TreeView->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  TreeView->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  return ;

  //    PerDomainTRe->expandAll();
}//setupTableView


void ZTokenTypeDLg::set(ZTypeBase pZType)
{
  ZType=pZType;

  ZTypeBase wZType = pZType &= ~ZType_Array;

  QModelIndex wIdx=searchZType(wZType);
  if (!wIdx.isValid())
    return;

  ArrayCHk->setChecked( pZType & ZType_Array);

  TreeView->setCurrentIndex(wIdx);
}


ZTypeBase ZTokenTypeDLg::get() {
  if (ArrayCHk->isChecked())
    return ZType | ZType_Array;
  return ZType;
}


void ZTokenTypeDLg::dataSetup()
{
  QList<QStandardItem*> wTypeRow;
  QVariant wV;
  QStandardItem* wItem=nullptr;

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  wItem=createItem("Atomic data types");
  TreeView->ItemModel->appendRow(wItem);



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
  for (int wi=0;wi < 3;wi++)
    TreeView->resizeColumnToContents(wi);

  TreeView->expandAll();

}

void ZTokenTypeDLg::closeEvent(QCloseEvent *event)
{
  TreeView->deleteLater();
}



/* test current item and if not OK get children recursively */
QModelIndex
ZTokenTypeDLg::_searchZType(QStandardItem* pItem,ZTypeBase pZType)
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
ZTokenTypeDLg::searchZType(ZTypeBase pZType)
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
