#include "zkeylistdlg.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>

#include <zdisplayedfield.h>
#include <dicedit.h>

#include <zentity.h>

#include <filegeneratedlg.h>

#include <zexceptiondlg.h>

ZKeyListDLg::ZKeyListDLg(QWidget *pParent) : QDialog(pParent)
{

}

ZKeyListDLg::~ZKeyListDLg() {
  clearKeyTRv();
/*  if (KeyDataRef!=nullptr)
    delete KeyDataRef;
*/
}

void
ZKeyListDLg::initLayout()
{
  resize(900,361);

  AcceptBTn = new QPushButton(this);
  AcceptBTn->setText("Select");
  AcceptBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );

  RejectBTn = new QPushButton(this);
  RejectBTn->setText("Discard");
  RejectBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );

  keyTRv=new ZQTreeView(this);
  keyTRv->newModel(6); /* 6 columns */
  keyTRv->setAlternatingRowColors(true);
  keyTRv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  keyTRv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected

  QVBoxLayout*  VBL=new QVBoxLayout(this);
  setLayout(VBL);

  VBL->addWidget(keyTRv);

  QHBoxLayout* HBL=new QHBoxLayout;
  HBL->setAlignment(Qt::AlignRight);
  HBL->addWidget(AcceptBTn);
  HBL->addWidget(RejectBTn);

  VBL->addLayout(HBL);


  int wCol=0;
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Name")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType code")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("KeyOffset")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("UniversalSize")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Hash code")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Tooltip")));



  QObject::connect(AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));
  QObject::connect(RejectBTn,SIGNAL(clicked()),this,SLOT(RejectClicked()));

  return ;
} // init()


ZStatus
ZKeyListDLg::displayKeyDictionaries(const ZMFDictionary* pDic)
{
  if (pDic==nullptr) {
    ZException.setMessage("ZKeyListDLg::displayKeyDictionaries",ZS_NULLPTR,Severity_Severe,"Input dictionary is null.");
    return ZS_NULLPTR;
  }
  ZMFDic = pDic;
  utf8VaryingString wStr;
  QList<QStandardItem *> wKeyRow, wKeyFieldRow ;

  keyTRv->ItemModel->removeRows(0,keyTRv->ItemModel->rowCount());

  for (long wi=0;wi < pDic->KeyDic.count();wi++) {
    KeyData wKHR ;
    wKHR.IndexName = pDic->KeyDic[wi]->DicKeyName;
    wKHR.Duplicates = pDic->KeyDic[wi]->Duplicates;

    if (wKHR.Forced)
        wKHR.KeySize = pDic->KeyDic[wi]->KeyGuessedSize;
    else {
        wKHR.KeySize = 0;
        for (long wj=0;wj < pDic->KeyDic[wi]->count(); wj++)
          wKHR.KeySize += pDic->TabConst(pDic->KeyDic[wi]->Tab(wj).MDicRank).UniversalSize;
    }

    wKeyRow=createKeyDicRow_KD(wKHR); /* create item row with appropriate infra data linked to it */

    for (long wj=0;wj < pDic->KeyDic[wi]->count(); wj++) {
      long wMDicRank=pDic->KeyDic[wi]->Tab(wj).MDicRank;
      wKeyFieldRow.clear();

      wKeyFieldRow << createItem(pDic->TabConst(wMDicRank).getName());

      wKeyFieldRow << createItem( decode_ZType (pDic->TabConst(wMDicRank).ZType));
      wKeyFieldRow << createItem( pDic->KeyDic[wi]->Tab(wj).KeyOffset,"%d");

      wKeyFieldRow << createItem( pDic->TabConst(wMDicRank).UniversalSize,"%ld");
      wKeyFieldRow << createItem( pDic->KeyDic[wi]->Tab(wj).Hash);

      wKeyRow[0]->appendRow(wKeyFieldRow);

      wKHR.KeySize += pDic->TabConst(wMDicRank).UniversalSize;

    }// for wj

    keyTRv->ItemModel->appendRow(wKeyRow);
  }// for wi

  keyTRv->expandAll();
  for (int wi=0;wi < keyTRv->ItemModel->columnCount();wi++ ){
      keyTRv->resizeColumnToContents(wi);
  }
  return ZS_SUCCESS;
}


int
ZKeyListDLg::searchForFieldName(const utf8VaryingString& pName) {
  for (long wi=0; wi < ZMFDic->ZMetaDic::count();wi++) {
    if (ZMFDic->TabConst(wi).getName()==pName)
      return int(wi);
  }
  return -1;
}






void
ZKeyListDLg::AcceptClicked() {
  ZDataReference          wDRef;
  QVariant                wV;
  if (!keyTRv->currentIndex().isValid()) {
    ZExceptionDLg::adhocMessage("Key selection",Severity_Error,nullptr,nullptr,"No key has been selected.");
    reject();
  }
  QModelIndex wIdx=keyTRv->currentIndex();
  QStandardItem* wItem = keyTRv->ItemModel->item(wIdx.row(),0);
  if (wItem->data(ZQtDataReference).isNull()) {
  /* if key field (no data) then get its parent */
  ZExceptionDLg::adhocMessage("Key selection",Severity_Error,nullptr,nullptr,"Please select a valid key (not a field).");
  reject();
  }
  wV = wItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
/*
  if (KeyDataRef!=nullptr)
    delete KeyDataRef;
*/
  KeyDataRef =  KeyData(*wDRef.getPtr<KeyData>());
  accept();
}

void
ZKeyListDLg::RejectClicked() {
  reject();
}


void
ZKeyListDLg::resizeEvent(QResizeEvent* pEvent)
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

  keyTRv->resize(wVW,wVH);  /* expands in width and height */

}//ZDicDLg::resizeEvent


QList<QStandardItem *>
createKeyDicRow_KD(const KeyData& pKHR) {
  QVariant wV;
  ZDataReference wDRef;
  QList<QStandardItem *>  wKeyRow;

  wKeyRow << createItem( pKHR.IndexName.toCChar());
  wKeyRow[0]->setEditable(false);

  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyDic),0);
  wDRef.setPtr(new KeyData(pKHR));
  wV.setValue(wDRef);
  wKeyRow[0]->setData(wV,ZQtDataReference);

  wKeyRow << createItem( "Dictionary key");

  wKeyRow << createItem( " ");

  /* other characteristics of the key */
  wKeyRow << createItem( pKHR.KeySize,"total size %ld");

  wKeyRow.last()->setEditable(false);

  //  wKeyRow << createItem( pKHR.Duplicates?"Duplicates":"No duplicate","%s");
  /*
  QStandardItem* wDup = createItemAligned( "Duplicates",Qt::AlignLeft);
  wDup->setEditable(false);
  wDup->setCheckable(true);
  wDup->setCheckState(pKHR.Duplicates?Qt::Checked:Qt::Unchecked);
  if (pKHR.Duplicates)
    wDup->setText("allow duplicates");
  else
    wDup->setText("No duplicates");
  wDup->setEditable(false);
  */
  wKeyRow << new QStandardItem(decode_ZST(pKHR.Duplicates));


  wKeyRow.last()->setEditable(false);

  return wKeyRow;
}

void
ZKeyListDLg::clearKeyTRv() {
  ZDataReference wDRef;
  QVariant wV;
  if (keyTRv==nullptr)
    return ;
  for (int wi=0; wi < keyTRv->ItemModel->rowCount();wi++) {
    releaseItemData<KeyData>(keyTRv->ItemModel->item(wi,0));
   }
  keyTRv->ItemModel->removeRows(0,keyTRv->ItemModel->rowCount());
}
