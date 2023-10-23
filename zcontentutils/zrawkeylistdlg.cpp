#include "zrawkeylistdlg.h"

#include <zqt/zqtwidget/zqtableview.h>
#include <zexceptiondlg.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <QStandardItem>


using namespace zbs;

ZRawKeyListDLg::ZRawKeyListDLg(QWidget *parent) : QDialog(parent)
{
  setWindowTitle("Index key list");

  resize(400,250);

  QVBoxLayout* wMainVBLyt=new QVBoxLayout;
  setLayout(wMainVBLyt);

  KeyListTBv = new ZQTableView(this);
  wMainVBLyt->addWidget(KeyListTBv);

  KeyListTBv->newModel(3);
  KeyListTBv->setSelectionBehavior(QAbstractItemView::SelectRows);

  int wCol=0;
  KeyListTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Index key name"));
  KeyListTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Size"));
  KeyListTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Duplicates"));

  QHBoxLayout* wHBLYt1=new QHBoxLayout;
  wMainVBLyt->addLayout(wHBLYt1);


  QHBoxLayout* wButtonBox=new QHBoxLayout;
  wMainVBLyt->addLayout(wButtonBox);
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  wButtonBox->addSpacerItem(wSpacer);

  QPushButton* wQuitBTn=new QPushButton("Quit");
  wButtonBox->addWidget(wQuitBTn);

  QPushButton* wSelectBTn=new QPushButton("Select");
  wButtonBox->addWidget(wSelectBTn);

  QObject::connect(wQuitBTn,&QPushButton::clicked,this,&ZRawKeyListDLg::KeyListQuitBTnClicked);
  QObject::connect(wSelectBTn,&QPushButton::clicked,this,&ZRawKeyListDLg::KeyListwSelectBTnClicked);
}

ZStatus ZRawKeyListDLg::set(const uriString& pURIMasterFile) {
  MasterFile = new ZMasterFile;
  locallyCreated=true;
  ZStatus wSt=MasterFile->zopen(pURIMasterFile,ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS) {
    utf8VaryingString wExc=ZException.last().formatFullUserMessage().toString();
    ZExceptionDLg::adhocMessage("Open master file",Severity_Error,nullptr,&wExc,
        "Cannot open master file %s",pURIMasterFile.toString());
    return wSt;
  }

  return set(MasterFile);
}

ZStatus ZRawKeyListDLg::set(ZMasterFile* pMasterFile) {

  if (!pMasterFile->isOpen())
    return ZS_FILENOTOPEN;

  if (pMasterFile->IndexTable.count()==0)
    return ZS_EMPTYFILE;

  MasterFile=pMasterFile;
  for (long wi=0; wi < MasterFile->IndexTable.count() ; wi++){
    QList<QStandardItem*> wRow;
    utf8VaryingString wStr;
    wRow << new QStandardItem(MasterFile->IndexTable[wi]->IndexName.toCChar());
    wStr.sprintf("%ld",MasterFile->IndexTable[wi]->KeyGuessedSize);
    wRow << new QStandardItem(wStr.toCChar());
    wRow << new QStandardItem(decode_ZST( MasterFile->IndexTable[wi]->Duplicates));
    KeyListTBv->ItemModel->appendRow(wRow);
  }// for

  for (int wi=0;wi < KeyListTBv->ItemModel->columnCount();wi++)
    KeyListTBv->resizeColumnToContents(wi);
  for (int wi=0;wi < KeyListTBv->ItemModel->rowCount();wi++)
    KeyListTBv->resizeRowToContents(wi);
  return ZS_SUCCESS;
}

void ZRawKeyListDLg::KeyListQuitBTnClicked(bool pChecked){
  if (locallyCreated)
    MasterFile->zclose();
  reject();
}

void ZRawKeyListDLg::KeyListwSelectBTnClicked(bool pChecked){
  QModelIndex wIdx = KeyListTBv->currentIndex();
  if (!wIdx.isValid())
    return;
  CurrentIndexRank = wIdx.row();
  if (locallyCreated)
    MasterFile->zclose();
  accept();
}
