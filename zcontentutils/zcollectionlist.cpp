#include "zcollectionlist.h"
#include "ui_zcollectionlist.h"
#include <zcontent/zindexedfile/zsearchparser.h>
#include <zqt/zqtwidget/zqtableview.h>
#include <QStandardItem>

ZCollectionList::ZCollectionList(zbs::ZSearchParser* pParser, QWidget *parent) :
                                                    QDialog(parent),
                                                    ui(new Ui::ZCollectionList)
{
  ui->setupUi(this);
  Parser=pParser;
  setup();
}

ZCollectionList::~ZCollectionList()
{
  delete ui;
}

void
ZCollectionList::setup()
{
  CollectionListTBv=new ZQTableView(ui->MainWDg);
  QList<QStandardItem*> wRow;
  CollectionListTBv->setColumnCount(2);
  for (int wi=0;wi < Parser->EntityList.count();wi++) {
    wRow.clear();
    wRow << new QStandardItem(Parser->EntityList[wi]->getName().toCChar());
    if (Parser->EntityList[wi]->_CollectionEntity!=nullptr)
      wRow << new QStandardItem("Collection");
    else
        if (Parser->EntityList[wi]->_FileEntity!=nullptr)
          wRow << new QStandardItem("File");
    else
      wRow << new QStandardItem("Unknown");
    CollectionListTBv->ItemModel->appendRow(wRow);
  }
  for (int wi=0;wi < CollectionListTBv->ItemModel->columnCount();wi++)
    CollectionListTBv->resizeColumnToContents(wi);
  for (int wi=0;wi < CollectionListTBv->ItemModel->rowCount();wi++)
    CollectionListTBv->resizeRowToContents(wi);

  QObject::connect(ui->QuitBTn,&QPushButton::clicked,this,&QDialog::reject);
  QObject::connect(ui->IKBTn,&QPushButton::clicked,this,&QDialog::accept);
}
std::shared_ptr<ZSearchEntity>
ZCollectionList::get()
{
  QModelIndex wIdx= CollectionListTBv->currentIndex();
  if (!wIdx.isValid())
    return nullptr;
  return Parser->EntityList[wIdx.row()];
}

