#include "zfielddlg.h"
#include "ui_zfielddlg.h"

#include <qevent.h>
#include <zqt/zqtwidget/zqstandarditemmodel.h>
#include <zqt/zqtwidget/zqstandarditem.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <ztoolset/ztypetype.h>
#include <zcontent/zindexedfile/zdatatype.h>

ZTypeClass::ZTypeClass(ZFieldDLg *pParent,QDialog* pDialog): ZQTreeView(pDialog)
{
//  pDialog->resize(371,361);
  ZQTreeView::setGeometry(QRect(0, 0, 900, 361));

  Parent=pParent;
  FatherDLg=pDialog;

  ZQStandardItem::setItemAlignment(Qt::AlignRight);


  setupTableView(true,2); /* 2 columns */

  int wCol=0;


  this->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Code","ZTypeClass").toUtf8().data()));
  this->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Symbol","ZTypeClass").toUtf8().data()));
  ZQStandardItem::setItemAlignment(Qt::AlignLeft);
  this->ItemModel->setHorizontalHeaderItem(wCol++,createItem(QObject::tr("Description","ZTypeClass").toUtf8().data()));
  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  dataSetup();

}

void
ZTypeClass::KeyFiltered(int pKey,int pMouseFlag)
{
  if(pKey == Qt::Key_Escape)
  {
    FatherDLg->reject();
    return;
  }
  if (pMouseFlag!=0)
    goto KKFMouseClickTrapped ;

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

KKFMouseClickTrapped:
  switch (pMouseFlag)
  {
  case ZEF_DoubleClickLeft:
  {
    QModelIndex wIdx= currentIndex();
    if(!wIdx.isValid())
      return;
    if (wIdx.column()!=0)
      wIdx=ItemModel->index(currentIndex().row(),0);
    if(!wIdx.isValid())
      return;
    if (getValueFromItem<ZTypeBase>(wIdx,ZType))
      FatherDLg->QDialog::accept();

    return;
  }
    //  case ZEF_SingleClickLeft:
  }

  return;
}


void ZTypeClass::setupTableView(bool pColumnAutoAdjust, int pColumns)
{
  //  FGeneric=true;
  int wFilter =  ZEF_Escape | ZEF_CommonControlKeys | ZEF_DragAndDrop | ZEF_DoubleClickLeft ;

  if (pColumnAutoAdjust)
    wFilter |= ZEF_AutoAdjustColumn;
  //  GenericTRe = new ZQTreeView((QWidget *)this,ZEF_CommonControlKeys | ZEF_AllowDataEnter );
  //  keyTRv = new ZQTreeView(this,ZLayout_Generic, wFilter ); // no data enter

  this->setZLayout(ZLayout_TypeDLg);
  this->setFilterMask(wFilter);

  //  ui->centralwidget->addWidget(GenericTRv);

  this->setSizePolicy(QSizePolicy ::Preferred , QSizePolicy ::Preferred );

  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  this->setDragEnabled(true);
  this->setDropIndicatorShown(true);
  this->setDataEnter(true);

  //  GenericTRv->ItemModel=new QStandardItemModel(0,GenericModelColumn,this) ;
  this->setModel(new QStandardItemModel(0,pColumns,this));
  this->ItemModel->setSortRole(ZSortRole);     // Key to have a model sorted on numeric data and not editable data

  // registering callback function : this notation allows to be readable
  //
  std::function<void(int,int)>                                wKeyFilterCBF=nullptr;
  std::function<void(QContextMenuEvent *)>                    wContextMenuCBF=nullptr;
  std::function<bool (int,ZDataReference *,ZDataReference *)> wDropEventCBF=nullptr;  // no Drag and Drop


  wKeyFilterCBF     = std::bind(&ZTypeClass::KeyFiltered, this,_1,_2);
/*
   wContextMenuCBF   = std::bind(&ZTypeClass::FlexMenu, this,_1);     // for context menu callback for ZQTreeView
  wDropEventCBF     = std::bind(&ZTypeClass::DropEvent, this,_1,_2);     // for drag and drop
*/
  this->_register(wKeyFilterCBF,wContextMenuCBF,wDropEventCBF);


  this->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  this->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  this->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  this->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  return ;

  //    PerDomainTRe->expandAll();
}//setupTableView

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
    {ZType_Unknown,"",""}
};
ZTypeEditStruct ZTypeVStringEdit[] = {
    {ZType_CharVaryingString,"ZType_CharVaryingString","a varying string of char- prefer using utf8 string."},
    {ZType_Utf8VaryingString,"ZType_Utf8VaryingString","a varying string of utf8 characters."},
    {ZType_Utf16VaryingString,"ZType_Utf16VaryingString","a varying string of utf16 characters. "},
    {ZType_Utf32VaryingString,"ZType_Utf32VaryingString","a varying string of utf32 characters. "},
    {ZType_URIString,"ZType_URIString","a base of utf8 varying string with URI capabilities"},
    {ZType_StdString,"ZType_StdString","a std::string class varying length : not used for storage, only for conversion"},
    {ZType_Unknown,"",""}
    };
ZTypeEditStruct ZTypeFStringEdit[] = {
    {ZType_CharFixedString,"ZType_CharFixedString","a fixed string of char characters. Prefer using utf8 fixed string. "},
    {ZType_Utf8FixedString,"ZType_Utf8FixedString","a fixed string of utf8 characters. "},
    {ZType_Utf16FixedString,"ZType_Utf16FixedString","a fixed string of utf16 characters. "},
    {ZType_Utf32FixedString,"ZType_Utf32FixedString","a fixed string of utf32 characters. "},
    {ZType_Unknown,"",""}
};
ZTypeEditStruct ZTypeOtherEdit[] = {
    {ZType_bitset,"ZType_bitset","bitset is a set of atomic data 'bit'"},
    {ZType_ZDate,"ZType_ZDate","a struct giving uint32_t  is ZDate (export and import)"},
    {ZType_ZDateFull,"ZType_ZDateFull","a struct giving uint64_t  is ZDateFull (export and import)"},
    {ZType_CheckSum,"ZType_CheckSum","a struct containing fixed array of unsigned char"},
    {ZType_Resource,"ZType_Resource","arithmetic 2 bytes value unsigned (no leading sign byte)"},
     {ZType_Unknown,"",""}
    };


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
/* test current item and if not OK get children recursively */
QModelIndex _searchZType(QStandardItemModel* pItemModel,QStandardItem* pItem,ZTypeBase pZType)
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
    wIdx = _searchZType(pItemModel,wItem1,pZType);
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


QModelIndex searchZType(QStandardItemModel* pItemModel,ZTypeBase pZType)
{
  QModelIndex wIdx ;

  QStandardItem* wItem= pItemModel->item(0,0) ;

  int wi = 0;
  while (wItem && (wi < wItem->rowCount()))
    {
    wItem= pItemModel->item(wi,0) ;
    wIdx=_searchZType(pItemModel,wItem,pZType);
    if (wIdx.isValid())
      return wIdx;
    wi++;
    }

    return QModelIndex();
}//searchZType


void ZTypeClass::set(ZTypeBase pZType)
{
  ZType=pZType;

  QModelIndex wIdx=searchZType(ItemModel,pZType);
  if (!wIdx.isValid())
    return;

  this->setCurrentIndex(wIdx);
}

void ZTypeClass::dataSetup()
{
  QList<QStandardItem*> wTypeRow;
  QVariant wV;
  QStandardItem* wItem=nullptr;

  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  wItem=createItem("Atomic data types");
  ItemModel->appendRow(wItem);

  for(int wi=0;ZTypeAtomicEdit[wi].Value!=ZType_Unknown;wi++ )
    wItem->appendRow(createItemRow(ZTypeAtomicEdit[wi]));

  wItem=createItem("Varying string types");
  ItemModel->appendRow(wItem);

  for(int wi=0;ZTypeVStringEdit[wi].Value!=ZType_Unknown;wi++ )
    wItem->appendRow(createItemRow(ZTypeVStringEdit[wi]));

  wItem=createItem("Fixed string types");
  ItemModel->appendRow(wItem);

  for(int wi=0;ZTypeFStringEdit[wi].Value!=ZType_Unknown;wi++ )
    wItem->appendRow(createItemRow(ZTypeFStringEdit[wi]));


  wItem=createItem("Class object types");
  ItemModel->appendRow(wItem);

  for(int wi=0;ZTypeOtherEdit[wi].Value!=ZType_Unknown;wi++ )
    wItem->appendRow(createItemRow(ZTypeOtherEdit[wi]));

  for (int wi=0;wi < 3;wi++)
    this->resizeColumnToContents(wi);

  this->expandAll();

}


void ZTypeClass::closeEvent(QCloseEvent *event)
{
  Parent->ZTypeTRv=nullptr;
  this->deleteLater();
}


ZFieldDLg::ZFieldDLg(QWidget *parent) :
                                        QDialog(parent),
                                        ui(new Ui::ZFieldDLg)
{
  ui->setupUi(this);

//  QObject::connect(ui->ZTypeLEd,SIGNAL(sigFocusIn(QFocusEvent*)),this,SLOT(ZTypeFocusIn(QFocusEvent*)));
  QObject::connect(ui->ListBTn,SIGNAL(clicked()),this,SLOT(ListClicked()));

  QObject::connect(ui->DisguardBTn,SIGNAL(clicked()),this,SLOT(DisguardClicked()));
  QObject::connect(ui->AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));

  ui->AcceptBTn->setDefault(true);

}

ZFieldDLg::~ZFieldDLg()
{
  if (ZTypeDLg)
    ZTypeDLg->deleteLater();

  delete ui;
}

int ZFieldDLg::exec()
{
  if (Field==nullptr)
    reject();
  return QDialog::exec();
}
/*
void
ZFieldDLg::ZTypeFocusIn(QFocusEvent* pEvent)
{
  if (ZTypeDLg==nullptr)
  {
    ZTypeDLg=new QDialog(this);
    ZTypeTRv=new ZTypeClass(this,ZTypeDLg);
    ZTypeTRv->ZQTreeView::setObjectName(QString::fromUtf8("ZTypeTRv"));
  }
  ZTypeDLg->show();
}
*/


void
ZFieldDLg::ListClicked()
{
  utf8String wStr;
  if (ZTypeDLg==nullptr)
    {
    ZTypeDLg=new QDialog(this);
    ZTypeTRv=new ZTypeClass(this,ZTypeDLg);
    ZTypeTRv->ZQTreeView::setObjectName(QString::fromUtf8("ZTypeTRv"));
    ZTypeDLg->setWindowTitle(QObject::tr("Data type","ZFieldDLg"));
    ZTypeDLg->setModal(true);
    }

  ZTypeTRv->set(Field->ZType);
  int wRet=ZTypeDLg->exec();
  if (wRet==QDialog::Rejected)
    return;

  Field->ZType = ZTypeTRv->get();

  ui->ZTypeStrLEd->setText(decode_ZType(Field->ZType));
  ui->ZTypeStrLEd->setReadOnly(true);

  Field->HeaderSize = _getURFHeaderSize(Field->ZType);
  wStr.sprintf("%ld",Field->HeaderSize);
  ui->HeaderSizeLEd->setText(wStr.toCChar());


  ui->KeyEligibleCHk->setChecked(!(Field->ZType & ZType_VaryingLength));

  if (Field->ZType & ZType_Atomic)
  {
    getAtomicZType_Sizes(Field->ZType,Field->NaturalSize,Field->UniversalSize);

    wStr.sprintf("%ld",Field->NaturalSize);
    ui->NaturalLEd->setText(wStr.toCChar());

    wStr.sprintf("%ld",Field->UniversalSize);
    ui->UniversalLEd->setText(wStr.toCChar());

    Field->Capacity=1;
    wStr.sprintf("%ld",Field->Capacity);
    ui->CapacityLEd->setText(wStr.toCChar());
  }



}//ListClicked

void
ZFieldDLg::AcceptClicked()
{
  accept();
//  ZTypeDLg->hide();
}

void
ZFieldDLg::DisguardClicked()
{
  reject();
//  ZTypeDLg->hide();
}




void ZFieldDLg::setup(ZFieldDescription *pField)
{
  Field=pField;
  utf8String wStr;

  ui->FieldNameLEd->setText(Field->getName().toCChar());

  wStr.sprintf("0x%08X",Field->ZType);
  ui->ZTypeLEd->setText(wStr.toCChar());

  ui->ZTypeStrLEd->setText(decode_ZType(Field->ZType));
  ui->ZTypeStrLEd->setReadOnly(true);

  ui->KeyEligibleCHk->setChecked(decode_ZType(Field->KeyEligible));

  wStr.sprintf("%ld",Field->NaturalSize);
  ui->NaturalLEd->setText(wStr.toCChar());

  wStr.sprintf("%ld",Field->UniversalSize);
  ui->UniversalLEd->setText(wStr.toCChar());

  wStr.sprintf("%ld",Field->HeaderSize);
  ui->HeaderSizeLEd->setText(wStr.toCChar());
  ui->HeaderSizeLEd->setReadOnly(true);

  wStr.sprintf("%ld",Field->Capacity);
  ui->CapacityLEd->setText(wStr.toCChar());

  ui->HashcodeLEd->setText(pField->Hash.toHexa().toChar());
  ui->HashcodeLEd->setReadOnly(true);
}

void ZFieldDLg::refresh()
{
  utf8String wStr;

  Field->setFieldName( ui->FieldNameLEd->text().toUtf8().data());

  Field->KeyEligible = ui->KeyEligibleCHk->isChecked();

  if (Field->ZType & ZType_Atomic )
    return;

  wStr=ui->CapacityLEd->text().toUtf8().data();
  Field->Capacity = (URF_Capacity_type)wStr.toInt();

  wStr=ui->NaturalLEd->text().toUtf8().data();
  Field->NaturalSize = (uint64_t)wStr.toULong();

  wStr=ui->UniversalLEd->text().toUtf8().data();
  Field->UniversalSize = (uint64_t)wStr.toULong();

  wStr=ui->UniversalLEd->text().toUtf8().data();
  Field->UniversalSize = (uint64_t)wStr.toULong();

}

