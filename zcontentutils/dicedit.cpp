#include "dicedit.h"
#include "ui_dicedit.h"
#include <QStandardItemModel>

#include <qmessagebox.h>

#include <zrandomfile/zheadercontrolblock.h>
#include <zrandomfile/zfilecontrolblock.h>
#include <zindexedfile/zmastercontrolblock.h>

#include <zindexedfile/zmfdictionary.h>

#include <zexceptiondlg.h>

#include <zqt/zqtwidget/zpinboard.h>

#include <zqt/zqtwidget/zqtreeview.h>
#include <zqt/zqtwidget/zqtableview.h>
//#include <zqt/zqtwidget/ZQStandardItemModel.h>
#include <zqt/zqtwidget/zqstandarditem.h>

#include <zqt/zqtwidget/zdatareference.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <zentity.h>

#include <zfielddlg.h>
#include <zkeydlg.h>

#include <zcontent/zindexedfile/zfielddescription.h>

#include <qclipboard.h>
#include <qmimedata.h>
#include <qfiledialog.h>

//#include <qtreeview.h>

#include <qaction.h>
#include <qactiongroup.h>
#include <zindexedfile/zfielddescription.h>
#include <zindexedfile/zindexfield.h>

#include <texteditmwn.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

ZPinboard   Pinboard;
bool        UseMimeData=false;
bool        DragEngaged=false;


class ZKeyFieldExtended: ZIndexField
{
  ZTypeBase     ZType;
  utf8_t        Name[cst_fieldnamelen];
  unsigned char HashCode[cst_md5];
};


/************************************************/
/* simulates resource allocation manager */
int ResourceCounter=0;

ZResource getNewResource(ZEntity_type pEntity)
{
  return ZResource (++ResourceCounter,pEntity)  ;
}
void releaseResource(ZResource pResource)
{
  return  ;
}
/***********************************************/


DicEdit::DicEdit(QWidget *parent) :
                                            QMainWindow(parent),
                                            ui(new Ui::DicEdit)
{
  ui->setupUi(this);

  setWindowTitle("Master Dictionary");

/*  ui->displayTBv->setStyleSheet(QString::fromUtf8("QTableView::item{border-left : 1px solid black;\n"
                                                "border-right  : 1px solid black;\n"
                                                "font: 75 12pt \"Courier\";\n"
                                                " }"));
*/

  fieldTBv = setupFieldTBv( ui->displayDicTBv,true,9);/* 9 columns */

  int wCol=0;
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Name")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("expanded")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Capacity")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("HeaderSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("UniversalSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("NaturalSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("KeyEligible")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Hash")));

  fieldTBv->setShowGrid(true);

  fieldTBv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
  fieldTBv->setWordWrap(false);

  fieldTBv->setSupportedDropActions(Qt::CopyAction);


  fieldTBv->addFilterMask(ZEF_DoubleClick);

  fieldTBv->setAlternatingRowColors(true);

  fieldTBv->show();

  /* for key fields */

  keyTRv = setupKeyTRv(ui->displayKeyTRv,true,6); /* 6 columns */


  wCol=0;
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Name")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType")));
//  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("MDicRank")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("KeyOffset")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("UniversalSize")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Hash")));


//  ui->displayKeyTRv->setShowGrid(true);

  keyTRv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
  keyTRv->setWordWrap(false);

  keyTRv->setSupportedDropActions(Qt::CopyAction);

  keyTRv->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  keyTRv->setAlternatingRowColors(true);

  keyTRv->show();

  ui->ClosedLBl->setVisible(false);
}

void DicEdit::setFileClosed(bool pYesNo)
{
  ui->ClosedLBl->setVisible(pYesNo);
}

void DicEdit::KeyTRvFlexMenu(QContextMenuEvent *event)
{
  if (keyFlexMEn==nullptr)
            setupKeyFlexMenu();
  keyFlexMEn->exec(event->globalPos());
//  menuFlex->deleteLater();
}

void DicEdit::FieldTBvFlexMenu(QContextMenuEvent *event)
{
  if (fieldFlexMEn==nullptr)
    setupFieldFlexMenu();
  fieldFlexMEn->exec(event->globalPos());
/*  QMenu* menuFlex=setupFieldFlexMenu();
  menuFlex->exec(event->globalPos());
  menuFlex->deleteLater();
*/
}


void
DicEdit::setupReadWriteMenu()
{
  FwritetoMEn=new QMenu(QObject::tr("write to","DicEdit"),this);

  FwritetoclipQAc = new QAction(QObject::tr("clipboard as xml","DicEdit"),this);
  FwritetoclipQAc->setObjectName("FwritetoclipQAc");
  FwritetoMEn->addAction(FwritetoclipQAc);

  FwriteXmltofileQAc = new QAction(QObject::tr("xml file","DicEdit"),this);
  FwriteXmltofileQAc->setObjectName("FwriteXmltofileQAc");
  FwritetoMEn->addAction(FwriteXmltofileQAc);

  FviewXmlQAc = new QAction(QObject::tr("view xml","DicEdit"),this);
  FviewXmlQAc->setObjectName("FviewXmlQAc");
  FwritetoMEn->addAction(FviewXmlQAc);

  FwritetoDicQAc = new QAction(QObject::tr("dictionary","DicEdit"),this);
  FwritetoDicQAc->setObjectName("FwritetoDicQAc");
  FwritetoMEn->addAction(FwritetoDicQAc);

  FloadMEn=new QMenu(QObject::tr("load from","DicEdit"),this);

  FloadfromclipQAc = new QAction(QObject::tr("clipboard","DicEdit"),this);
  FloadfromclipQAc->setObjectName("FloadfromclipQAc");
  FloadMEn->addAction(FloadfromclipQAc);

  FloadfromXmlFileQAc = new QAction(QObject::tr("xml file","DicEdit"),this);
  FloadfromXmlFileQAc->setObjectName("FloadfromXmlFileQAc");
  FloadMEn->addAction(FloadfromXmlFileQAc);

  FloadfromDicQAc = new QAction(QObject::tr("dictionary","DicEdit"),this);
  FloadfromDicQAc->setObjectName("FloadfromDicQAc");
  FloadMEn->addAction(FloadfromDicQAc);

}//setupReadWriteMenu


QMenu *
DicEdit::setupFieldFlexMenu()
{
  fieldFlexMEn=new QMenu(keyTRv);
  fieldFlexMEn->setTitle(QCoreApplication::translate("DicEdit", "Fields", nullptr));

  fieldActionGroup=new QActionGroup(fieldFlexMEn) ;
  QObject::connect(fieldActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(fieldActionEvent(QAction*)));

  FInsertBeforeQAc= new QAction(fieldFlexMEn);
  FInsertBeforeQAc->setText(QCoreApplication::translate("DicEdit", "New before", nullptr));
  FInsertBeforeQAc->setObjectName("FInsertBeforeQAc");

  FInsertAfterQAc= new QAction(fieldFlexMEn);
  FInsertAfterQAc->setText(QCoreApplication::translate("DicEdit", "New after", nullptr));
  FInsertAfterQAc->setObjectName("FInsertAfterQAc");

  FChangeQAc= new QAction(fieldFlexMEn);
  FChangeQAc->setText(QCoreApplication::translate("DicEdit", "Change", nullptr));
  FChangeQAc->setObjectName("FChangeQAc");

  FDeleteQAc= new QAction(fieldFlexMEn);
  FDeleteQAc->setText(QCoreApplication::translate("DicEdit", "Delete", nullptr));
  FDeleteQAc->setObjectName("FDeleteQAc");

  FCutQAc= new QAction(fieldFlexMEn);
  FCutQAc->setText(QCoreApplication::translate("DicEdit", "Cut", nullptr));
  FCutQAc->setObjectName("FCutQAc");

  FcopyQAc= new QAction(fieldFlexMEn);
  FcopyQAc->setText(QCoreApplication::translate("DicEdit", "Copy", nullptr));
  FcopyQAc->setObjectName("FcopyQAc");

  FpasteQAc= new QAction(fieldFlexMEn);
  FpasteQAc->setText(QCoreApplication::translate("DicEdit", "Paste", nullptr));
  FpasteQAc->setObjectName("FpasteQAc");

  fieldFlexMEn->addAction(FInsertBeforeQAc);
  fieldActionGroup->addAction(FInsertBeforeQAc);
  fieldFlexMEn->addAction(FInsertAfterQAc);
  fieldActionGroup->addAction(FInsertAfterQAc);
  fieldFlexMEn->addAction(FDeleteQAc);
  fieldActionGroup->addAction(FDeleteQAc);


  fieldFlexMEn->addSeparator();
  fieldFlexMEn->addAction(FCutQAc);
  fieldActionGroup->addAction(FCutQAc);
  fieldFlexMEn->addAction(FcopyQAc);
  fieldActionGroup->addAction(FcopyQAc);

  fieldFlexMEn->addAction(FpasteQAc);
  fieldActionGroup->addAction(FpasteQAc);

  if (FwritetoMEn==nullptr)
    setupReadWriteMenu();

  fieldFlexMEn->addSeparator();
  fieldFlexMEn->addMenu(FwritetoMEn);
  fieldFlexMEn->addMenu(FloadMEn);

  fieldActionGroup->addAction(FwritetoclipQAc);
  fieldActionGroup->addAction(FviewXmlQAc);
  fieldActionGroup->addAction(FwriteXmltofileQAc);
  fieldActionGroup->addAction(FwritetoDicQAc);

  fieldActionGroup->addAction(FloadfromclipQAc);
  fieldActionGroup->addAction(FloadfromXmlFileQAc);
  fieldActionGroup->addAction(FloadfromDicQAc);

  return fieldFlexMEn;
}//setupFieldFlexMenu


QMenu *
DicEdit::setupKeyFlexMenu()
{
  keyFlexMEn=new QMenu(keyTRv);
  keyFlexMEn->setTitle(QCoreApplication::translate("DicEdit", "Keys", nullptr));

  keyActionGroup=new QActionGroup(keyFlexMEn) ;
  QObject::connect(keyActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(keyActionEvent(QAction*)));

  KInsertKeyQAc= new QAction(keyFlexMEn);
  KInsertKeyQAc->setText(QCoreApplication::translate("DicEdit", "New key", nullptr));
  KInsertKeyQAc->setObjectName("KInsertKeyQAc");

  KDeleteKeyQAc= new QAction(keyFlexMEn);
  KDeleteKeyQAc->setText(QCoreApplication::translate("DicEdit", "Delete key", nullptr));
  KDeleteKeyQAc->setObjectName("KDeleteKeyQAc");

  KDeleteQAc= new QAction(keyFlexMEn);
  KDeleteQAc->setText(QCoreApplication::translate("DicEdit", "Delete", nullptr));
  KDeleteQAc->setObjectName("KDeleteQAc");

  KcutQAc= new QAction(keyFlexMEn);
  KcutQAc->setText(QCoreApplication::translate("DicEdit", "Cut", nullptr));
  KcutQAc->setObjectName("KcutQAc");

  KpasteQAc= new QAction(keyFlexMEn);
  KpasteQAc->setText(QCoreApplication::translate("DicEdit", "Paste", nullptr));
  KpasteQAc->setObjectName("KpasteQAc");

  KappendQAc= new QAction(keyFlexMEn);
  KappendQAc->setText(QCoreApplication::translate("DicEdit", "Append", nullptr));
  KappendQAc->setObjectName("KappendQAc");

  keyFlexMEn->addAction(KDeleteQAc);
  keyActionGroup->addAction(KDeleteQAc);

  keyFlexMEn->addSeparator();
  keyFlexMEn->addAction(KInsertKeyQAc);
  keyActionGroup->addAction(KInsertKeyQAc);
  keyFlexMEn->addAction(KDeleteKeyQAc);
  keyActionGroup->addAction(KDeleteKeyQAc);

  keyFlexMEn->addSeparator();
  keyFlexMEn->addAction(KcutQAc);
  keyActionGroup->addAction(KcutQAc);
  keyFlexMEn->addAction(KpasteQAc);
  keyActionGroup->addAction(KpasteQAc);
  keyFlexMEn->addAction(KappendQAc);
  keyActionGroup->addAction(KappendQAc);

  if (FwritetoMEn==nullptr)
    setupReadWriteMenu();

  keyFlexMEn->addSeparator();
  keyFlexMEn->addMenu(FwritetoMEn);
  keyFlexMEn->addMenu(FloadMEn);

  keyActionGroup->addAction(FwritetoclipQAc);
  keyActionGroup->addAction(FviewXmlQAc);
  keyActionGroup->addAction(FwriteXmltofileQAc);
  keyActionGroup->addAction(FwritetoDicQAc);

  keyActionGroup->addAction(FloadfromclipQAc);
  keyActionGroup->addAction(FloadfromXmlFileQAc);
  keyActionGroup->addAction(FloadfromDicQAc);

  return keyFlexMEn;
} // setupKeyFlexMenu

void DicEdit::keyActionEvent(QAction* pAction)
{
  utf8String wMsg;
  if (pAction->objectName()=="KInsertKeyQAc")
  {
    QVariant wV;
    ZDataReference wDRef;
    KeyDic_Pack wKD_Pack;
    wKD_Pack.setName((const utf8_t*)"New key");
    wKD_Pack.KeyUniversalSize  = -1;
    wKD_Pack.Duplicates  = false;

    ZKeyDLg* wKeyDLg=new ZKeyDLg;
    wKeyDLg->setWindowTitle("Key");

    wKeyDLg->set(&wKD_Pack);

    int wRet=wKeyDLg->exec();

    wKeyDLg->deleteLater();

    if (wRet==QDialog::Rejected)
      return;

    wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyDic),-1);

    QList<QStandardItem *> wKeyRow;

    wKeyRow << createItem(wKD_Pack.getName().toCChar());
    wKeyRow << createItem (wKD_Pack.KeyUniversalSize ,"size: %ld");
    wKeyRow << createItem (wKD_Pack.Duplicates?"Yes":"No" ,"dup: %s");

    keyTRv->ItemModel->appendRow(wKeyRow);

    wDRef.setIndex(wKeyRow[0]->index());
    wV.setValue<ZDataReference>(wDRef);
    wKeyRow[0]->setData(wV,ZFlowRole);

    wV.setValue<KeyDic_Pack>(wKD_Pack);
    wKeyRow[0]->setData(wV,ZQtValueRole);

    return;
  }

  if (pAction->objectName()=="KDeleteQAc")
  {
    QModelIndex wIdx= keyTRv->currentIndex();
    if(!wIdx.isValid())
      return;
    if (wIdx.column()!=0)           /* because all data is stored at column 0 */
      wIdx=wIdx.siblingAtColumn(0);

    /* extract data from item */
    long wi=wIdx.row();
    QVariant wV=wIdx.data(ZFlowRole);
    ZDataReference wDRef=wV.value<ZDataReference>();

    if (wDRef.getZEntity()==ZEntity_KeyField) /* delete a key field */
      {
      /* search for treeview parent row which is key description for getting current index */
      QModelIndex wPIdx=wIdx.parent();
      if (!wPIdx.isValid())
        return;
      wV=wPIdx.data(ZFlowRole);
      if (!wV.isValid())
        return;
      ZDataReference wDRefKey= wV.value<ZDataReference>();
      if (wDRefKey.isInvalid())
        return;
      ZKeyDictionary* wKDic= (ZKeyDictionary*)wDRefKey.getPtr();
      long wDicRank=MasterDic->KeyDic[wDRef.getDataRank()]->Tab[wDRef.DataRank].MDicRank;

      wMsg.sprintf("Deleting key field <%s>",MasterDic->Tab[wDicRank].getName().toCChar());
      statusBar()->showMessage(QObject::tr(wMsg.toCChar()),cst_MessageDuration);
      /* suppress key field within key dictionary */
      wKDic->erase(wDRef.DataRank);
      /* then remove */
      keyTRv->ItemModel->removeRow(wIdx.row());
      /* recompute key size */
      int KeySize=0;
      if (wKDic->count())
          KeySize=wKDic->_reComputeKeySize();
      return;
      }

    if (wDRef.getZEntity()!=ZEntity_KeyDic)  /* if not key dictionary : wrong data */
      {
      fprintf(stderr,"KDeleteQAc- wrong ZDataReference entity <%s>\n",decode_ZEntity(wDRef.getZEntity()).toChar());
      return ;
      }
    /* Key dictionary : here we have a whole key to delete */

    ZKeyDictionary* wKDic= (ZKeyDictionary*)wDRef.getPtr();
    wMsg.sprintf("Deleting whole key number <%ld> name <%s>",wDRef.getDataRank(), wKDic->DicKeyName.toCChar());
    statusBar()->showMessage(QObject::tr(wMsg.toCChar()),cst_MessageDuration);
    /* suppress whole key within master dictionary */
    MasterDic->KeyDic.erase(wDRef.getDataRank());
    /* then remove */

    int wRowNb=keyTRv->ItemModel->rowCount(wIdx);  /* get number of children */

    keyTRv->ItemModel->removeRows(0,wRowNb,wIdx);  /* remove all child rows */
    keyTRv->ItemModel->removeRow(wIdx.row(),wIdx.parent()); /* remove key dictionary row */
    return;
  }//KDeleteQAc

  if (pAction->objectName()=="KcutQAc")
    {
    QModelIndex wIdx= keyTRv->currentIndex();
    if(!wIdx.isValid())
      {
        wMsg.sprintf("no row selected");
        ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
        return;
      }

    /* extract data from item */
    QVariant wV=wIdx.data(ZFlowRole);
    ZDataReference wDRef=wV.value<ZDataReference>();

    if (wDRef.getZEntity()==ZEntity_KeyDic)
      {
      wV=wIdx.data(ZQtValueRole);
      KeyDic_Pack wKDPack = wV.value<KeyDic_Pack>();
      QList<QList<QStandardItem*>>* wKeyRows = new QList<QList<QStandardItem*>>(cutAllKeyRows(wIdx));

      ZDataReference wKeyDRef;
      wKeyDRef.ZLayout=keyTRv->Layout;
//      wKeyDRef.setIndex(wIdx);
      wKeyDRef.setResource(getNewResource(ZEntity_KeyDicRowsSet));
      wKeyDRef.setPtr(wKeyRows);

      ZPinboardElement wPBElt;
      wPBElt.DRef = wKeyDRef;
      Pinboard.push(wPBElt);

      wMsg.sprintf("Key name <%s> full rows copied.",wKDPack.getName().toCChar());
      statusBar()->showMessage(QObject::tr(wMsg.toCChar()),cst_MessageDuration);
      return;
      }

    if (wDRef.getZEntity()!=ZEntity_KeyField)
      {
      wMsg.sprintf("Bad source row for cut operation.");
      statusBar()->showMessage(QObject::tr(wMsg.toCChar()),cst_MessageDuration);
      return;
      }

    wV=wIdx.data(ZQtValueRole);
    KeyField_Pack wKFPack = wV.value<KeyField_Pack>();

    ZDataReference wKeyFieldDRef;
    wKeyFieldDRef.ZLayout=keyTRv->Layout;
    wKeyFieldDRef.setResource(getNewResource(ZEntity_KeyFieldRow));
//    wKeyFieldDRef.setIndex(wIdx);

    QList<QStandardItem*>* wKeyFieldRow = new QList<QStandardItem*>(cutRow(keyTRv->ItemModel, wIdx));
    wKeyFieldDRef.setPtr(wKeyFieldRow);

    ZPinboardElement wPBElt;
    wPBElt.DRef = wKeyFieldDRef;
    Pinboard.push(wPBElt);

    wMsg.sprintf("Key field <%ld> cut.",wKFPack.getName().toCChar());
    statusBar()->showMessage(QObject::tr(wMsg.toCChar()),cst_MessageDuration);
    return;
    }// KcutQAc


  if (pAction->objectName()=="KpasteQAc") /* paste from pinboard */
    {
    QModelIndex wIdx= keyTRv->currentIndex(); /* destination index for paste */
    if(!wIdx.isValid())
    {
      wMsg.sprintf("no row selected");
      ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
      return;
    }
    if (Pinboard.isEmpty())
      {
      wMsg.sprintf("nothing selected.");
      ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
      return;
      }
    if (Pinboard.last().getZEntity()==ZEntity_KeyDicRowsSet)
      {
       pinboardPasteKeyRowSet(wIdx);
       return ;
      }
    if (Pinboard.last().getZEntity()==ZEntity_KeyFieldRow)
      {
        pinboardPasteKeyFieldRow(wIdx);
        return ;
      }
    /* extract data from item */

    if (Pinboard.last().getZEntity()==ZEntity_DicField)
    {
      pinboardPasteFieldToKey(wIdx);
      return;
    }
    return;
    }//if (pAction->objectName()=="KpasteQAc")

    if (pAction->objectName()=="KappendQAc") /* append from pinboard */
    {
      QModelIndex wIdx= keyTRv->currentIndex(); /* destination index for paste */
      if(!wIdx.isValid())
      {
        wMsg.sprintf("no row selected");
        ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
        return;
      }
      if (Pinboard.isEmpty())
      {
        wMsg.sprintf("nothing selected.");
        ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
        return;
      }
      if (Pinboard.last().getZEntity()==ZEntity_KeyDicRowsSet)
      {
        pinboardPasteKeyRowSet(wIdx,true);
        return ;
      }
      if (Pinboard.last().getZEntity()==ZEntity_KeyFieldRow)
      {
        pinboardPasteKeyFieldRow(wIdx,true);
        return ;
      }
      /* extract data from item */

      if (Pinboard.last().getZEntity()==ZEntity_DicField)
      {
        pinboardPasteFieldToKey(wIdx);
        return;
      }
      return;
    }// KpasteQAc

    readWriteActionEvent(pAction);  /* call the common menu actions */

  return;
}

utf8VaryingString
DicEdit::getDicName()
{
  utf8String wDicName=MasterDic->DicName;

  QDialog wDicNameDLg;
  wDicNameDLg.setObjectName("wDicNameDLg");
  wDicNameDLg.setWindowTitle(QObject::tr("Dictionary name","DicEdit"));
  wDicNameDLg.resize(400,150);
  QVBoxLayout* QVL=new QVBoxLayout(&wDicNameDLg);
  QVL->setObjectName("QVL");
  QHBoxLayout* QHL=new QHBoxLayout;
  QHL->setObjectName("QHL");
  wDicNameDLg.setLayout(QVL);
  QVL->insertLayout(0,QHL);
  QLabel* wLb=new QLabel(QObject::tr("Dictionary name","DicEdit"),&wDicNameDLg);
  wLb->setObjectName("wLb");
  QHL->addWidget(wLb);
  QLineEdit* wDicNameLEd=new QLineEdit(wDicName.toCChar());
  wDicNameLEd->setObjectName("wDicNameLEd");
  QHL->addWidget(wDicNameLEd);

  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QHLBtn->setObjectName("QHLBtn");
  QVL->insertLayout(1,QHLBtn);

  QPushButton* wOk=new QPushButton(QObject::tr("Ok","DicEdit"),&wDicNameDLg);
  wOk->setObjectName("wOk");
  QPushButton* wCancel=new QPushButton(QObject::tr("Cancel","DicEdit"),&wDicNameDLg);
  wCancel->setObjectName("wCancel");
  QHLBtn->addWidget(wCancel);
  QHLBtn->addWidget(wOk);

  wDicNameLEd->setText(wDicName.toCChar());

  QObject::connect(wOk, &QPushButton::clicked, &wDicNameDLg, &QDialog::accept);
  QObject::connect(wCancel, &QPushButton::clicked, &wDicNameDLg, &QDialog::reject);

  int wRet=wDicNameDLg.exec();
  if (wRet==QDialog::Accepted)
  {
    wDicName=wDicNameLEd->text().toUtf8().data();
  }
  return wDicName;
}//getDicName

void
DicEdit::readWriteActionEvent(QAction*pAction)
{
  utf8String wMsg;

  /* common menu actions key and field menu */

  /* https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types */

  if  (pAction->objectName()=="FwritetoclipQAc") /* generate xml from current content and write xml to clipboard */
  {
    /* generate xml from current views content */

    ZMFDictionary* wMasterDic = screenToDic();  /* get Master dictionary content from views */

    wMasterDic->DicName = getDicName();         /* name meta dictionary */

    utf8String wXmlDic=wMasterDic->XmlSaveToString(true); /* generate full xml definition */

    /* effective write to clipboard */
    QByteArray wBA (wXmlDic.toCChar());
    QClipboard *wClipboard = QGuiApplication::clipboard();
    QMimeData* wMime=new QMimeData;
    /* mime type could be application/xml or (deprecated) text/xml */
    /* but clipboard is always text/plain */
    wMime->setData("text/plain",wBA);
    wClipboard->setMimeData(wMime,QClipboard::Clipboard);

    statusBar()->showMessage(QObject::tr("Full dictionary content has been copied to clipboard.","DicEdit"),cst_MessageDuration);
    delete wMasterDic;

    return;
  }// FwritetoclipQAc

  if  (pAction->objectName()=="FloadfromclipQAc")
  {
    ZStatus wSt;
    ZMFDictionary wMasterDic;
    utf8String wXmlContent;
    ZaiErrors Errorlog;
    QByteArray wBA ;
    QClipboard *wClipboard = QGuiApplication::clipboard();
    const QMimeData * wMData=wClipboard->mimeData(QClipboard::Clipboard);
    if (wMData==nullptr)
    {
      statusBar()->showMessage(QObject::tr("Clipboard is empty","DicEdit"),cst_MessageDuration);
    }
    if (!wMData->hasFormat("text/plain"))
    {
      statusBar()->showMessage(QObject::tr("Clipboard has no text data.","DicEdit"),cst_MessageDuration);
    }
    wXmlContent = wMData->data("text/plain").data();  /* load mime data as utf8 string */

    Errorlog.setErrorLogContext("FloadfromclipQAc");
    /* here load master dictionary */
    wSt =wMasterDic.XmlLoadFromString(wXmlContent,true,&Errorlog);
    if (wSt!=ZS_SUCCESS)   /* XmlLoadFromString uses returns a ZStatus but uses errorlog to log messages */
    {
      ZExceptionDLg::messageWAdd("FloadfromXmlFileQAc",
          wSt,
          Errorlog.getSeverity(),
          Errorlog.allLoggedToString(),
          "Status is %s : Cannot load xml string definition from clipboard.",decode_ZStatus(wSt));
      return;
    }
    /* here we may only have warnings, infos or text messages */
    if (Errorlog.count()>0)
    {
      Severity_type wS = Errorlog.getSeverity();

      int wRet = ZExceptionDLg::message2BWAdd("FloadfromXmlFileQAc",
          wS > Severity_Warning?ZS_ERROR:ZS_NOTHING,
          wS,
          Errorlog.allLoggedToString(),
          "Stop","Continue",
          "Some warning messages have been issued during load of xml definition from clipboard");
      if (wRet==ZEDLG_Rejected)
        return;
    }

    delete MasterDic;
    MasterDic = new ZMFDictionary(wMasterDic) ;

    wMsg.sprintf("Xml dictionary definition has been successfully loaded from clipboard.");
    statusBar()->showMessage(QObject::tr(wMsg.toCChar(),"DicEdit"),cst_MessageDuration);
  } //  FloadfromclipQAc


  if  (pAction->objectName()=="FviewXmlQAc") /* generate xml from current content and view it */
  {
    /* generate xml from current views content */
    ZMFDictionary* wMasterDic = screenToDic();
    wMasterDic->DicName = getDicName();

    utf8String wXmlDic=wMasterDic->XmlSaveToString(true);

    delete wMasterDic;

    /* display xml content */
    textEditMWn* wTEx=new textEditMWn((QWidget*)this);
    wTEx->setText(wXmlDic,wMasterDic->DicName);
    wTEx->show();
    return;
  }//  FviewXmlQAc

  if  (pAction->objectName()=="FwriteXmltofileQAc")
  {
    QFileDialog wFd;

    wFd.setWindowTitle(QObject::tr("Xml file","DicEdit"));
    wFd.setLabelText(QFileDialog::Accept,  "Select");
    wFd.setLabelText(QFileDialog::Reject ,  "Cancel");
    while (true)
    {
      int wRet=wFd.exec();
      if (wRet==QDialog::Rejected)
        return;

      if (wFd.selectedFiles().isEmpty())
      {
        ZExceptionDLg::message("FviewXmlQAc",ZS_EMPTY,Severity_Warning,"No file has been selected.\nPlease select a valid file.");
//        QMessageBox::critical(this,tr("No file selected"),"Please select a valid file");
      }
      else
        break;
    }//while (true)

    uriString wOutFile= wFd.selectedFiles()[0].toUtf8().data();

    bool Fexists=wOutFile.exists();

    /* generate xml from current views content */
    ZMFDictionary* wMasterDic = screenToDic();
    wMasterDic->DicName = getDicName();

    utf8String wXmlDic=wMasterDic->XmlSaveToString(true);

    wOutFile.writeContent(wXmlDic);

    delete wMasterDic;

    wMsg.sprintf("file %s has been %s",wOutFile.toCChar(),Fexists?"replaced":"created");
    statusBar()->showMessage(QObject::tr(wMsg.toCChar(),"DicEdit"),cst_MessageDuration);
    return;
  } // FwriteXmltofileQAc


  if  (pAction->objectName()=="FloadfromXmlFileQAc")
  {
    QFileDialog wFd;
    wFd.setWindowTitle(QObject::tr("Xml file","DicEdit"));
    wFd.setLabelText(QFileDialog::Accept,  "Select");
    wFd.setLabelText(QFileDialog::Reject ,  "Cancel");
    while (true)
      {
      int wRet=wFd.exec();
      if (wRet==QDialog::Rejected)
        return;

      if (wFd.selectedFiles().isEmpty())
        {
        QMessageBox::critical(this,tr("No file selected"),"Please select a valid file");
        }
      else
        break;
      }//while (true)

    uriString wXmlFile= wFd.selectedFiles()[0].toUtf8().data();

    ZMFDictionary wMasterDic;
    utf8VaryingString wXmlContent;
    ZaiErrors Errorlog;

    ZStatus wSt = wXmlFile.loadUtf8(wXmlContent);
    if (wSt!=ZS_SUCCESS)
      {
      ZExceptionDLg::displayLast();  /* loadUtf8 uses ZException */
      return;
      }
    /* here load master dictionary */
    wSt =wMasterDic.XmlLoadFromString(wXmlContent,true,&Errorlog);
    if (wSt!=ZS_SUCCESS)   /* XmlLoadFromString uses returns a ZStatus but uses errorlog to log messages */
      {
      ZExceptionDLg::messageWAdd("FloadfromXmlFileQAc",
          wSt,
          Errorlog.getSeverity(),
          Errorlog.allLoggedToString(),
          "Status is %s : Cannot load xml string definition from file %s",decode_ZStatus(wSt), wXmlFile.toCChar());
      return;
      }
  /* here we may only have warnings, infos or text messages */
    if (Errorlog.count()>0)
      {
      Severity_type wS = Errorlog.getSeverity();

      int wRet = ZExceptionDLg::message2BWAdd("FloadfromXmlFileQAc",
              wS > Severity_Warning?ZS_ERROR:ZS_WARNING,
              wS,
              Errorlog.allLoggedToString(),
              "Stop","Continue",
              "Some warning messages have been issued during load of xml definition from file %s",wXmlFile.toCChar());
      if (wRet==QDialog::Rejected)
              return;
      }

  delete MasterDic;
  MasterDic = new ZMFDictionary(wMasterDic) ;

  wMsg.sprintf("Xml file %s has been successfully loaded.",wXmlFile.toCChar());
  statusBar()->showMessage(QObject::tr(wMsg.toCChar(),"DicEdit"),cst_MessageDuration);
  return;
  } // FloadfromXmlFileQAc


  if  (pAction->objectName()=="FwritetoDicQAc")
    {
      statusBar()->showMessage(QObject::tr("Sorry not yet implemented.","DicEdit"),cst_MessageDuration);
      return;
    } // FwritetoDicQAc


  if  (pAction->objectName()=="FloadfromDicQAc")
    {
      statusBar()->showMessage(QObject::tr("Sorry not yet implemented.","DicEdit"),cst_MessageDuration);
      return;
    } // FloadfromDicQAc

  return;

}//readWriteActionEvent


bool
DicEdit::pinboardPasteFieldToKey(const QModelIndex& pKeyIdx,bool pAppend)
{
  if (Pinboard.isEmpty())
    {
    statusBar()->showMessage(QObject::tr("Pinboard is empty-","DicEdit"),cst_MessageDuration);
    return false;
    }

  utf8String wStr;
  if (!pKeyIdx.isValid())
    {
    statusBar()->showMessage(QObject::tr("No target element selected","DicEdit"),cst_MessageDuration);
    return false;
    }

  FieldDesc_Pack* wFDPack= Pinboard.last().getDRefPtr<FieldDesc_Pack*>();


  QVariant wV = pKeyIdx.data(ZFlowRole);
  if (!wV.isValid())
    {
    ui->statusBar->showMessage("Invalid data from insert row.",cst_MessageDuration);
    return false;
    }
  ZDataReference wDRef =wV.value<ZDataReference>();

  QList<QStandardItem *> wKeyFieldRow;
  if (!pinboardGetKeyField(pKeyIdx.row(),wKeyFieldRow))  /* wIdx.row() gives the key number-could be given by wDRef.DataRank */
    return false;

  if (wDRef.getZEntity()==ZEntity_KeyDic) /* target is key dictionary : append field to key */
    {
    /* append to key Item */
    QStandardItem* wKeyItem=keyTRv->ItemModel->itemFromIndex( pKeyIdx );
    wKeyItem->appendRow(wKeyFieldRow);

    keyTRv->resizeColumns();

    /* update model index in Row[0] */
    ZDataReference wDRef1;
    QModelIndex wNewIdx = wKeyFieldRow[0]->index();
    wV=wKeyFieldRow[0]->data(ZFlowRole);
    wDRef1=wV.value<ZDataReference>();
    wDRef1.setIndex(wNewIdx);
    wV.setValue<ZDataReference>(wDRef1);
    wKeyFieldRow[0]->setData(wV,ZFlowRole);



    updateKeyValues(pKeyIdx);/* update values for the key */
    wStr.sprintf("field <%s> appended to key <%s>.",wFDPack->getName().toCChar(),wKeyItem->text().toUtf8().data());
    ui->statusBar->showMessage(wStr.toCChar(),cst_MessageDuration);
    return true;
    }

  if (wDRef.getZEntity()==ZEntity_KeyField) /* target is key field : insert key field before this key field */
    {
    /* search key item father and append before current idx row */
    QModelIndex wKeyParentIdx = pKeyIdx.parent();
    QStandardItem* wItem=keyTRv->ItemModel->itemFromIndex(wKeyParentIdx);
    const char* wAppCh;
    if (pAppend)
      {
      wItem->appendRow(wKeyFieldRow);
      wAppCh="appended";
      }
    else
      {
      wItem->insertRow(pKeyIdx.row(),wKeyFieldRow);
      wAppCh="inserted";
      }
    /* update model index in Row[0] */
    ZDataReference wDRef1;
    QModelIndex wNewIdx = wKeyFieldRow[0]->index();
    wV=wKeyFieldRow[0]->data(ZFlowRole);
    wDRef1=wV.value<ZDataReference>();
    wDRef1.setIndex(wNewIdx);
    wV.setValue<ZDataReference>(wDRef1);
    wKeyFieldRow[0]->setData(wV,ZFlowRole);

    updateKeyValues(pKeyIdx.parent()); /* update values for the whole key */
    wStr.sprintf("field <%s> %s to key <%s>.",wFDPack->getName().toCChar(),wAppCh,wItem->text().toUtf8().data());
    ui->statusBar->showMessage(wStr.toCChar(),cst_MessageDuration);
    return true;
    }

  ui->statusBar->showMessage("Invalid type of row.",cst_MessageDuration);
  return false;
}

bool
DicEdit::pinboardPasteKeyRowSet(const QModelIndex& pKeyIdx,bool pAppend) /* if pKeyIdx is invalid, then append key row set */
{
  utf8String wStr;
  if (Pinboard.isEmpty())
    {
    wStr = "No previous element in Pinboard.";
    statusBar()->showMessage(QObject::tr("No previous element in Pinboard.","DicEdit"),cst_MessageDuration);
    return false;
    }
  if (Pinboard.last().getZEntity()!=ZEntity_KeyDicRowsSet)
    {
    wStr.sprintf("Expecting ZEntity_KeyDicRowsSet in Pinboard while got <%s>\n",decode_ZEntity( Pinboard.last().getZEntity()).toChar());
    statusBar()->showMessage(QObject::tr(wStr.toCChar(),"DicEdit"),cst_MessageDuration);
    fprintf(stderr,"DicEdit::pinboardPasteKeyRowSet-E-INVENT expecting ZEntity_KeyDicRowsSet in Pinboard.\n"
                    "while got <%s>\n",decode_ZEntity( Pinboard.last().getZEntity()).toChar());

    return false;
    }
  QModelIndex wKeyIdx=pKeyIdx;
  ZDataReference wDRef;
  if (!pKeyIdx.isValid())/* if invalid then append to end - if not insert before index */
  {
    statusBar()->showMessage(QObject::tr("No target element selected- Appending.","DicEdit"),cst_MessageDuration);
    wDRef.setInvalid();
  }
  else
    {
    QVariant wV = pKeyIdx.data(ZFlowRole);
    if (!wV.isValid())
      { /* if data is not valid - append */
        statusBar()->showMessage(QObject::tr("Current index data is invalid - Appending.","DicEdit"),cst_MessageDuration);
        wDRef.setInvalid();     /* to append */
        wKeyIdx=QModelIndex();  /* set target model index to invalid */
      }
    else
      {
      wDRef = wV.value<ZDataReference>();
      if (wDRef.getZEntity()==ZEntity_KeyField)
        {/* if key field get parent key to insert before */

          wKeyIdx = pKeyIdx.parent();
        }
        else if (wDRef.getZEntity()!=ZEntity_KeyDic)
          {
          statusBar()->showMessage(QObject::tr("Trying to insert key rows in non appropriate row.","DicEdit"),cst_MessageDuration);
          return false;
          }
      }// else
    }//else

/* insert or append key rows set */

  QList<QList<QStandardItem *>>* wKeyRows = Pinboard.getLast()->DRef.getPtr<QList<QList<QStandardItem *>>*>();

  QList<QStandardItem *> wKeyDicRow = (*wKeyRows)[0]; /* first is key dictionary row */

  if (pAppend ||(!wKeyIdx.isValid()))
  {
    /* append key dictionary row */
    keyTRv->ItemModel->appendRow((*wKeyRows)[0]);

    /* append each key field rows to qstandarditem */

    for (int wi=1;wi < wKeyRows->count();wi++)
      (*wKeyRows)[0][0]->appendRow((*wKeyRows)[wi]);

    wStr.sprintf("1 key dictionary and %d key fields added.",wKeyRows->count()-1);
    ui->statusBar->showMessage(QObject::tr(wStr.toCChar()),cst_MessageDuration);

    Pinboard.pop();

    return true;
  }


// wKeyIdx.isValid()

    int wKeyRow=wKeyIdx.row();
    /* insert key dictionary row (rank 0) to root index */
    keyTRv->ItemModel->insertRows(wKeyRow,1,wKeyIdx.parent());
    /* for this key dictionary row, insert each item in its appropriate column */
    for (int wCol=0;wCol < wKeyDicRow.count();wCol++)
      keyTRv->ItemModel->setItem(wKeyRow,wCol,wKeyDicRow[wCol]);

    /* then append each child row to parent item */
    for (int wi=1;wi<wKeyRows->count();wi++)
      wKeyDicRow[0]->appendRow((*wKeyRows)[wi]);

    /* delete Pinboard element, and rows */

    wStr.sprintf("1 key dictionary and %d key fields inserted.",wKeyRows->count()-1);
    ui->statusBar->showMessage(QObject::tr(wStr.toCChar()),cst_MessageDuration);

    delete wKeyRows;

    Pinboard.pop();

    wStr.sprintf("1 key dictionary and %d key fields added.",wKeyRows->count()-1);
    ui->statusBar->showMessage(QObject::tr(wStr.toCChar()),cst_MessageDuration);
    return true;
  /* wKeyIdx is not valid : append */


} //pinboardInsertKeyRowSet

/* pinboardPasteKeyFieldRow paste a QList<QStandardItem*> row representing a key field row.
 * This comes from keyTRv to be pasted to keyTRv (move).
 *
 * if pIdx points to
 * - a Key dictionary row, then field row is append at the end of existing key field rows
 * - a key field row, then field row is inserted before pIdx or appended if pAppend is true
 *
 *  key values are recomputed
 */
bool
DicEdit::pinboardPasteKeyFieldRow(const QModelIndex& pIdx,bool pAppend) /* if pKeyIdx is invalid, then append key row set */
{
  utf8String wStr;
  if (Pinboard.isEmpty())
  {
    wStr = "No previous element in Pinboard.";
    statusBar()->showMessage(QObject::tr("No previous element in Pinboard.","DicEdit"),cst_MessageDuration);
    return false;
  }
  if (Pinboard.last().getZEntity()!=ZEntity_KeyFieldRow)
    {
    wStr.sprintf("Expecting ZEntity_KeyFieldRow in Pinboard while got <%s>\n",decode_ZEntity( Pinboard.last().getZEntity()).toChar());
    statusBar()->showMessage(QObject::tr(wStr.toCChar(),"DicEdit"),cst_MessageDuration);
    fprintf(stderr,"DicEdit::pinboardPasteKeyFieldRow-E-INVENT expecting ZEntity_KeyFieldRow in Pinboard.\n"
                    "while got <%s>\n",decode_ZEntity( Pinboard.last().getZEntity()).toChar());

    return false;
    }

  if (!pIdx.isValid())
    {
    statusBar()->showMessage(QObject::tr("No target element selected","DicEdit"),cst_MessageDuration);
    return false;
    }

  QVariant wV = pIdx.data(ZFlowRole);
  if (!wV.isValid())
    {
    ui->statusBar->showMessage("Invalid data for target row.",cst_MessageDuration);
    fprintf(stderr,"DicEdit::pinboardPasteKeyFieldRow-E-INVDATA invalid data for target row.\n");
    return false;
    }
  ZDataReference wDRef =wV.value<ZDataReference>();

  QList<QStandardItem *>* wKeyFieldRow = Pinboard.last().DRef.getPtr<QList<QStandardItem *>*>();

  if (wDRef.getZEntity()==ZEntity_KeyDic) /* target is key dictionary : append field to key */
    {
    /* append to key Item */
    QStandardItem* wKeyItem=keyTRv->ItemModel->itemFromIndex( pIdx );
    wKeyItem->appendRow(*wKeyFieldRow);

    keyTRv->resizeColumns();

    /* update model index in Row[0] */
    updateKeyValues(pIdx);/* update values for the key */

    wStr.sprintf("1 field added to key.");
    ui->statusBar->showMessage(QObject::tr(wStr.toCChar()),cst_MessageDuration);

    Pinboard.pop();

    return true;
    }

  if (wDRef.getZEntity()==ZEntity_KeyField) /* target is key field : insert key field before this key field  or append it */
    {
    /* search key item father and append before current idx row */
    QModelIndex wKeyParentIdx = pIdx.parent();
    QStandardItem* wItem=keyTRv->ItemModel->itemFromIndex(wKeyParentIdx);
    if (pAppend)
      wItem->appendRow(*wKeyFieldRow);
    else
      wItem->insertRow(pIdx.row(),*wKeyFieldRow);

    /* update model index in Row[0] */
    updateKeyValues(pIdx.parent()); /* update values for the whole key */
    return true;
    }

  ui->statusBar->showMessage("Invalid type of row.",cst_MessageDuration);
  return false;
}


bool
DicEdit::updateKeyValues(const QModelIndex& pKeyIdx)
{
  utf8String wStr;
  if (!pKeyIdx.isValid())
    {
    fprintf(stderr,"DicEdit::updateKeyValues-E-INVIDX Index is not valid.\n");
    return false;
    }

  QModelIndex wKeyIdx=pKeyIdx;
  if (pKeyIdx.column()!=0)
    wKeyIdx=pKeyIdx.siblingAtColumn(0);

  QVariant wV = wKeyIdx.data(ZFlowRole);
  if (!wV.isValid())
    {
    fprintf(stderr,"DicEdit::updateKeyValues-E-INVDATA Index data is not valid.\n");
    return false;
    }

  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.isInvalid())
    {
    fprintf(stderr,"DicEdit::updateKeyValues-E-INVDREF Invalid Data reference\n");
    return false;
    }

  if (wDRef.getZEntity()!=ZEntity_KeyDic)
    {
      fprintf(stderr,"DicEdit::updateKeyValues-E-INVROW Index is not a Key Dictionary.\n");
      return false;
    }

  int wRNb=keyTRv->ItemModel->rowCount(wKeyIdx);

  KeyDic_Pack wKDPack;
  KeyField_Pack wKFPack;

  wKDPack.KeyUniversalSize=0;
  long wKeyOffset=0;
  QStandardItem* wItem=nullptr;
  QModelIndex wItemIdx;

  int wRow=0;
  QModelIndex wIdx=keyTRv->ItemModel->index(wRow,0,wKeyIdx);
  while (wRow < wRNb)
  {
    wIdx=keyTRv->ItemModel->index(wRow,0,wKeyIdx);
    wV=wIdx.data(ZQtValueRole);
    wKFPack = wV.value<KeyField_Pack>();
    wKFPack.KeyOffset = wKeyOffset;

    wKDPack.KeyUniversalSize += wKFPack.UniversalSize;
    wKeyOffset+=wKFPack.UniversalSize;

    wV.setValue<KeyField_Pack>(wKFPack); /* update back item data */
    wItem= keyTRv->ItemModel->itemFromIndex(wIdx);
    wItem->setData(wV,ZQtValueRole);

    /* update displayed key offset value */

    wItemIdx = keyTRv->ItemModel->index(wRow,2,wKeyIdx);
    wItem= keyTRv->ItemModel->itemFromIndex(wItemIdx);
    wStr.sprintf("%ld",wKFPack.KeyOffset);
    wItem->setText(wStr.toCChar());
    /* done */

    wRow++;
  }//while (wRow < wRNb)

  /* update back key dic header data */

  wItem= keyTRv->ItemModel->itemFromIndex(wKeyIdx);
  wV.setValue<KeyDic_Pack>(wKDPack);
  wItem->setData(wV,ZQtValueRole);


  /* update key size */

  wItemIdx = wKeyIdx.siblingAtColumn(1);
  wItem= keyTRv->ItemModel->itemFromIndex(wItemIdx);
  wStr.sprintf("size : %ld",wKDPack.KeyUniversalSize);
  wItem->setText(wStr.toCChar());

  return true;
}//recomputeKeyValues


bool DicEdit::getIndexFieldData (QModelIndex& wIdx,ZDataReference &wDRef,ZFieldDescription &wFDesc)
{
  if (!wIdx.isValid())
    {
      ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
      return false;
    }
  QVariant wV=wIdx.data(ZFlowRole);
  if (!wV.isValid())
    return false;
  wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_DicField)
    return false;
  wV=wIdx.data(ZQtValueRole);
  if (!wV.isValid())
    return false;
  FieldDesc_Pack wFDescPack = wV.value<FieldDesc_Pack>();
  wFDesc = wFDescPack.toFieldDescription();
  return true;
}
bool DicEdit::pinboardPushField (QModelIndex& wIdx)
{
  if (!wIdx.isValid())
  {
    ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
    return false;
  }
  QVariant wV=wIdx.data(ZFlowRole);
  if (!wV.isValid())
  {
    ui->statusBar->showMessage("Cannot find data in row element.",cst_MessageDuration);
    return false;
  }
  ZDataReference wDRef=wV.value<ZDataReference>();
  wV=wIdx.data(ZQtValueRole);
  if (!wV.isValid())
  {
    ui->statusBar->showMessage("Cannot find data in row element.",cst_MessageDuration);
    return false;
  }
  FieldDesc_Pack wFDPack = wV.value<FieldDesc_Pack>();

  ZPinboardElement wPElt;
  wPElt.setDataReference(wDRef);

  wPElt.setData(&wFDPack,sizeof(FieldDesc_Pack));
  Pinboard.push(wPElt);
  return true;
}
bool DicEdit::pinboardGetField (ZFieldDescription& pField)
{
  if (Pinboard.isEmpty())
  {
    ui->statusBar->showMessage("No previous data in clipboard.",cst_MessageDuration);
    return false;
  }

  if (Pinboard.last().DRef.getZEntity()!=ZEntity_DicField)
  {
    ui->statusBar->showMessage("Data in clipboard is not appropriate to paste there.",cst_MessageDuration);
    return false;
  }

  FieldDesc_Pack* wFDPackPtr = Pinboard.last().getDataPtr<FieldDesc_Pack>();

  pField=wFDPackPtr->toFieldDescription();

  return true;
}

void DicEdit::fieldActionEvent(QAction* pAction)
{
  utf8String wMsg;
  if (pAction->objectName()=="FInsertBeforeQAc")
  {
    QModelIndex wIdx=fieldTBv->currentIndex();
    if (!wIdx.isValid())
    {
      statusBar()->showMessage(QObject::tr("No field row selected","DicEdit"),cst_MessageDuration);
      return ;
    }
    fieldCreateBefore(wIdx);
    return;
  }
  if (pAction->objectName()=="FInsertAfterQAc")
  {
    statusBar()->showMessage(QObject::tr("Not implemented","DicEdit"),cst_MessageDuration);
    return;
  }
  if (pAction->objectName()=="FChangeQAc")
  {
    QModelIndex wIdx=fieldTBv->currentIndex();
    if (!wIdx.isValid())
    {
      statusBar()->showMessage(QObject::tr("No field row selected","DicEdit"),cst_MessageDuration);
      return ;
    }
    fieldChange(wIdx);
    return;
  }

  if (pAction->objectName()=="FDeleteQAc")
  {
    utf8String wMsg;
    QModelIndex wIdx= fieldTBv->currentIndex();
    if(!wIdx.isValid())
      {
      ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
      return;
      }

    ZDataReference    wDRef;
    ZFieldDescription wFDesc;

    if (!getIndexFieldData(wIdx,wDRef,wFDesc))
      return;

    /* search for keys with that field involved */
    int wKField=0;
    for (long wi=0;wi < MasterDic->KeyDic.count();wi++)
      {
      for (long wj=0;wi < MasterDic->KeyDic[wi]->count();wi++)
            if (wDRef.getDataRank()==MasterDic->KeyDic[wi]->Tab[wj].MDicRank)
              {
              wKField++;
              MasterDic->KeyDic[wi]->erase(wj); /* remove key with this field as key field */
              wj--;
              }
      }
      if (wKField==0)
          wMsg.sprintf("Deleting dictionary field <%s>",wFDesc.getName().toCChar());
      else
      {
        displayKeyDictionaries(*MasterDic);  /* if some key(s) have been changed, display all key again */
        wMsg.sprintf("Deleting dictionary field <%s>. Changing <%d> key(s)",
            wFDesc.getName().toCChar(),wKField);
      }
    /* then remove */
    MasterDic->erase(wDRef.getDataRank());
    fieldTBv->ItemModel->QStandardItemModel::removeRow(wIdx.row(),wIdx.parent());
    ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
    return;
  }

  if (pAction->objectName()=="FcopyQAc")
    {
    utf8String wMsg;
    QModelIndex wIdx= fieldTBv->currentIndex();
    if(!wIdx.isValid())
    {
      ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
      return;
    }
    ZDataReference    wDRef;
    ZFieldDescription wFDesc;

/*    if (!getIndexFieldData(wIdx,wDRef,wFDesc))
      return;
*/

    pinboardPushField(wIdx);

    wMsg.sprintf("1 field copied");
    ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
    return;
    }


  if (pAction->objectName()=="FCutQAc")
    {
      QModelIndex wIdx= fieldTBv->currentIndex();
      if(!wIdx.isValid())
        {
        wMsg.sprintf("no row selected");
        ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
        return;
        }

      QVariant wV=wIdx.data(ZFlowRole);
      ZDataReference wDRef=wV.value<ZDataReference>();
      wV=wIdx.data(ZQtValueRole);
      FieldDesc_Pack wFDPack = wV.value<FieldDesc_Pack>();

      ZPinboardElement wPElt;
      wPElt.setDataReference(wDRef);

      wPElt.setData(&wFDPack,sizeof(FieldDesc_Pack));
      Pinboard.push(wPElt);

      fieldTBv->ItemModel->removeRow(wIdx.row(),wIdx.parent());
      wMsg.sprintf("1 field cut");
      ui->statusBar->showMessage(wMsg.toCChar(),cst_MessageDuration);
      return;
    }

  if (pAction->objectName()=="FpasteQAc")
  {
    utf8String wMsg;
    QModelIndex wIdx= fieldTBv->currentIndex();
    if(!wIdx.isValid())
      {
      ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
      return;
      }
    pinboardInsertField(wIdx);
    return;
  }

  readWriteActionEvent(pAction);  /* call the common menu actions */

  return;
}//fieldActionEvent

bool
DicEdit::pinboardInsertField(QModelIndex wIdx)
{
  if (Pinboard.isEmpty())
    return false;
  if (Pinboard.getLast()->DRef.getZEntity()!=ZEntity_DicField) /* only dictionary field is allowed */
    return false;
  ZPinboard* wPB=&Pinboard;
  utf8String wMsg;
  if(!wIdx.isValid())
  {
    ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
    return false;
  }

  ZFieldDescription wFDesc;
  FieldDesc_Pack* wFDPackPtr = Pinboard.last().getDataPtr<FieldDesc_Pack>();
  wFDesc=wFDPackPtr->toFieldDescription();

  QList<QStandardItem*> wRow=createRowFromField(wFDesc);

  QVariant wV;
  wV.setValue<ZDataReference>(Pinboard.last().DRef);
  wRow[0]->setData(wV,ZFlowRole);

  FieldDesc_Pack* wFDPack=Pinboard.last().getDataPtr<FieldDesc_Pack>();
  if (wFDPack==nullptr)
  {
    fprintf(stderr,"Pinboard-E-INVFDP: invalid FieldDesc_Pack - nullptr.\n");
    return false;
  }
  wV.setValue<FieldDesc_Pack>(*wFDPack);
  wRow[0]->setData(wV,ZQtValueRole);

  fieldTBv->ItemModel->insertRow(wIdx.row(), wRow);

  fieldTBv->resizeRowsToContents();

  return true;
}

void
DicEdit::KeyTRvKeyFiltered(int pKey,int pMouse)
{

  if(pKey == Qt::Key_Escape)
  {
    if( QMessageBox::question (this,tr("You pressed <Esc> key"), tr("Do you really want to quit without saving anything ?"),QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
    {
      QApplication::quit();
      return;
    }
  }
  if (pMouse!=0)
    goto KKFMouseClickTrapped ;

  if(pKey == Qt::Key_Insert)
  {
//    ZDataReference WR;
//    WR.Layout = CurrentTree->getLayout();
 //   WR.ResourceReference = getNewResource(ZEntity_Activity);
 //   WR.DataRank=-1;
//    createEvent(WR);
    return;
  }
  if(pKey == Qt::Key_Delete)
  {
//    deleteGeneric();
    return;
  }


  return;

KKFMouseClickTrapped:

  return;

}//KeyTReKeyFiltered

bool DicEdit::fieldDelete(QModelIndex &pIdx)
{
  if (!pIdx.isValid())
    return false;

  ZFieldDescription wF;
  QVariant wV;
  wV = pIdx.data(ZQtValueRole);
  if (!wV.isValid())
    return false;

  fieldTBv->ItemModel->removeRow(pIdx.row());

  return true;
}

bool DicEdit::fieldChange(QModelIndex& pIdx)
{
  utf8String wStr;
  if (!pIdx.isValid())
    return false;
  long wi=pIdx.row(); 
  QVariant wV=fieldTBv->ItemModel->item(wi,0)->data(ZFlowRole);
  if (!wV.isValid())
    return false;
  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_DicField)
  {
    fprintf(stderr,"DicEdit::fieldChange-E-INVENT Invalid entity : expected ZEntity_DicField.");
    return false;
  }
  if (FieldDLg==nullptr)
    FieldDLg=new ZFieldDLg(this);

  ZFieldDescription wFDesc = MasterDic->Tab[wDRef.DataRank];  /* temporary structure for storing changes */
  bool wB=false;
  while (!wB)
  {
    FieldDLg->setup(&wFDesc);
    int wRet=FieldDLg->exec();
    if (wRet==QDialog::Rejected)
      return false;
    wB=controlField(wFDesc);
  }

  /* accept changes */
  MasterDic->Tab[wDRef.DataRank]=wFDesc;
  wV.setValue<ZFieldDescription>(wFDesc);
  fieldTBv->ItemModel->item(wi,0)->setData(wV,ZQtValueRole);

  if (!displayFieldChange(wFDesc,pIdx))
    {
    wStr.sprintf("unable to change field <%s>. Something went wrong.",
        MasterDic->Tab[wDRef.DataRank].getName().toCChar());
    statusBar()->showMessage(QObject::tr(wStr.toCChar()),cst_MessageDuration);
    return false;
    }
  wStr.sprintf("Changed field <%s>",MasterDic->Tab[wDRef.DataRank].getName().toCChar());
  statusBar()->showMessage(QObject::tr(wStr.toCChar()),cst_MessageDuration);
  return true;
}//DicEdit::fieldChange

bool controlField(ZFieldDescription& pField)
{
  if (pField.getName()==__NEW_FIELDNAME__)
    {
    ZExceptionDLg::message("DicEdit::controlField",ZS_INVNAME,Severity_Error,
        "Field name <%s> is invalid. Please enter a valid field name.",pField.getName().toCChar());
    return false;
    }
  if (pField.ZType==ZType_Unknown)
    {
    ZExceptionDLg::message("DicEdit::controlField",ZS_INVTYPE,Severity_Error,
        "Field type 0x%08X <%s> is invalid. Please enter a valid field type.",pField.ZType,decode_ZType(pField.ZType));
    return false;
    }
  if (pField.HeaderSize!=_getURFHeaderSize(pField.ZType))
    {
      int wRet=ZExceptionDLg::message2B("DicEdit::controlField",ZS_INVVALUE,Severity_Warning, "Force","Modify",
          "Field type 0x%08X <%s> has a specified header size of <%ld> while you entered <%ld>. Please confirm.",
        pField.ZType,decode_ZType(pField.ZType),
        _getURFHeaderSize(pField.ZType),pField.HeaderSize);
      if (wRet==QDialog::Rejected)
        return false;

      return true;
    }
  return true;
}

void processItem(QStandardItem* pItem, long pDataRank)
{
  ZDataReference wDRef;
  QVariant wV;
  if (pItem==nullptr)
    return ;

  wV=pItem->data(ZFlowRole);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.DataRank >= pDataRank)
  {
    wDRef.DataRank++;
    wV.setValue<ZDataReference>(wDRef);
    pItem->setData(wV,ZFlowRole);
  }
}

/* test current item and if not OK get children recursively */
void _updateDataRank(QStandardItemModel* pItemModel,QStandardItem* pItem,long pDataRank)
{
  QStandardItem* wItem1=nullptr;

  if (pItem==nullptr)
    return ;

  processItem(pItem,pDataRank);

  int wi = 0;
  wItem1 = pItem->child(wi);
  if (wItem1 ==nullptr)
    return ;

  while (wItem1 && (wi < pItem->rowCount()))
  {
    _updateDataRank(pItemModel,wItem1,pDataRank);
    wi++;
    wItem1 = pItem->child(wi);
  }
  return ;
}// _updateDataRank
void updateDataRank(QStandardItemModel* pItemModel,long pDataRank)
{
  QModelIndex wIdx ;
  ZDataReference wDRef;

  QStandardItem* wItem= pItemModel->item(0,0) ;

  int wi = 0;
  while (wItem && (wi < wItem->rowCount()))
  {
    wItem= pItemModel->item(wi,0) ;
    _updateDataRank(pItemModel,wItem,pDataRank);
    wi++;
  }

  return ;
}//updateDataRank


bool DicEdit::fieldCreateBefore(QModelIndex &wIdx)
{
  if (!wIdx.isValid())
  {
    ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
    return false;
  }

  /* get data to access dictionary */
  QVariant wV = wIdx.data(ZFlowRole);
  ZDataReference wDRef = wV.value<ZDataReference>();

  if (FieldDLg==nullptr)
    FieldDLg=new ZFieldDLg(this);

  ZFieldDescription wFDesc;
  wFDesc.setFieldName(__NEW_FIELDNAME__);
  wFDesc.ZType=ZType_Unknown;
  wFDesc.HeaderSize=0;
  wFDesc.Capacity=0;
  wFDesc.UniversalSize=0;
  wFDesc.NaturalSize=0;
  wFDesc.KeyEligible=false;
  wFDesc.Hash.clear();

  bool wB=false;
  while (!wB)
    {
    FieldDLg->setup(&wFDesc);
    int wRet=FieldDLg->exec();
    if (wRet==QDialog::Rejected)
      return false;
    wB=controlField(wFDesc);
    }

  MasterDic->insertField(wFDesc,wDRef.DataRank);
  wDRef.setPtr(&MasterDic->Tab[wDRef.DataRank]);

  /* update (shift) data ranks */
  updateDataRank(fieldTBv->ItemModel,wDRef.DataRank);

  QList<QStandardItem*> wRow = createRowFromField(wFDesc);

  wV.setValue<ZDataReference>(wDRef);
  wRow[0]->setData(wV,ZFlowRole);

  fieldTBv->ItemModel->insertRow(wIdx.row(),wRow);



  return true;
}
bool DicEdit::fieldMoveBefore(QModelIndex &wIdxSource,QModelIndex &wIdxTarget)
{
  if (!wIdxSource.isValid())
  {
    ui->statusBar->showMessage("No valid source row selected.",cst_MessageDuration);
    return false;
  }
  if (!wIdxTarget.isValid())
  {
    ui->statusBar->showMessage("No target source row selected.",cst_MessageDuration);
    return false;
  }

  /* get data to access dictionary */
  QVariant wV = wIdxSource.data(ZFlowRole);
  ZDataReference wDRef = wV.value<ZDataReference>();


  ZFieldDescription wFDesc;
  wFDesc._copyFrom(*(ZFieldDescription*)wDRef.getPtr());

  fieldDelete(wIdxSource);

  return true;
}


/**
 * @brief createRowFromFieldDescription creates an item list describing field desciption.
 *                                      data is not set and must be set by callee.
 * @param pField
 * @return
 */

QList<QStandardItem*> createRowFromField(ZFieldDescription& pField)
{
  utf8String wStr;
  QList<QStandardItem*> wFieldRow;

  wFieldRow << new QStandardItem(pField.getName().toCChar());
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("0x%08X",pField.ZType);
  wFieldRow << new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wFieldRow << new QStandardItem( decode_ZType(pField.ZType));
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("%d",pField.Capacity);
  wFieldRow << new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("%ld",pField.HeaderSize);
    wFieldRow <<  new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("%ld",pField.UniversalSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("%ld",pField.NaturalSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wFieldRow <<  new QStandardItem( pField.KeyEligible?"Yes":"No");
  wFieldRow.last()->setEditable(false);

  wFieldRow <<  new QStandardItem( pField.Hash.toHexa().toChar());
  wFieldRow.last()->setEditable(false);

  return wFieldRow;
}

bool setRowFromField(QList<QStandardItem*> &wRow,ZFieldDescription& pField)
{
  utf8String wStr;
  if (wRow.count()<10)
  {
    fprintf(stderr,"setRowFromField-E-IVCNT  Invalid number of QStandardItem within list to update.\n");
    return false;
  }

  wRow[0]->setText(pField.getName().toCChar());
  wRow[0]->setEditable(false);

  wStr.sprintf("0x%08X",pField.ZType);
  wRow[1]->setText(wStr.toCChar());
  wRow[0]->setEditable(false);

  wRow[2]->setText(decode_ZType(pField.ZType));
  wRow[0]->setEditable(false);

  wStr.sprintf("%d",pField.Capacity);
  wRow[3]->setText(wStr.toCChar());
  wRow[0]->setEditable(false);

  wStr.sprintf("%d",pField.Capacity);
  wRow[4]->setText(wStr.toCChar());
  wRow[0]->setEditable(false);

  wStr.sprintf("%ld",pField.HeaderSize);
  wRow[5]->setText(wStr.toCChar());
  wRow[0]->setEditable(false);

  wStr.sprintf("%ld",pField.UniversalSize);
  wRow[6]->setText(wStr.toCChar());
  wRow[0]->setEditable(false);

  wStr.sprintf("%ld",pField.NaturalSize);
  wRow[7]->setText(wStr.toCChar());
  wRow[0]->setEditable(false);

  wRow[8]->setText(pField.KeyEligible?"Yes":"No");
  wRow[0]->setEditable(false);

  wRow[9]->setText(pField.Hash.toHexa().toChar());
  wRow[0]->setEditable(false);

  return true;
}



bool DicEdit::displayFieldChange(ZFieldDescription& pField,QModelIndex& pIdx)
{
  if (!pIdx.isValid())
  {
    ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
    return false;
  }
  utf8String wStr;
  QList<QStandardItem*> wRow = itemRow(fieldTBv->ItemModel,pIdx);

  QVariant wV=wRow[0]->data(ZFlowRole);
  if (!wV.isValid())
    return false;
  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_DicField)
  {
    fprintf(stderr,"DicEdit::displayFieldChange-E-INVENT Invalid entity : expected ZEntity_DicField.");
    return false;
  }

  return setRowFromField(wRow,pField);
}//displayFieldChange

/* replaces the values (displayed value and dictionary value) of a field at model index wIdx */
void DicEdit::acceptFieldChange(ZFieldDescription& pField,QModelIndex &wIdx)
{
  utf8String wStr;
//  QModelIndex wIdx=fieldTBv->currentIndex();
  if (!wIdx.isValid())
  {
    ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
    return;
  }

  QStandardItem* wItem=fieldTBv->ItemModel->item(wIdx.row(),0);
  QVariant wV=wItem->data(ZFlowRole);
  if (!wV.isValid())
    return;
  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_DicField)
  {
    fprintf(stderr,"DicEdit::acceptFieldChange-E-INVENT target item has invalid entity : expected ZEntity_DicField.");
    return;
  }

  if (!displayFieldChange(pField,wIdx))
    return;

  MasterDic->Tab[wDRef.DataRank] = pField;

  wDRef.setPtr(&MasterDic->Tab[wDRef.DataRank]);  /* store pointer to dictionary field */
  wV.setValue<ZDataReference>(wDRef);
  wItem->setData(wV,ZFlowRole);

} //acceptFieldChange


void
DicEdit::FieldTBvKeyFiltered(int pKey,int pMouseFlag)
{

  if(pKey == Qt::Key_Escape)
  {
    this->hide();
    return;
  }
  if (pMouseFlag!=0)
    goto KKFMouseClickTrapped ;

  if(pKey == Qt::Key_Insert)
  {
    ZDataReference WR;
    WR.ZLayout = ZLayout_FieldTBv;
    WR.ResourceReference = getNewResource(ZEntity_DicField);
    WR.DataRank=-1;
    QModelIndex wIdx = fieldTBv->currentIndex();
    if (wIdx.isValid())
      fieldCreateBefore(wIdx);
//    createEvent(WR);
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
    QModelIndex wIdx= fieldTBv->currentIndex();
    if(!wIdx.isValid())
    {
      statusBar()->showMessage(QObject::tr("No row element selected","DicEdit"),cst_MessageDuration);
      return;
    }
    fieldChange(wIdx);
    break;
  }
//  case ZEF_SingleClickLeft:
  }

  return;

}//KeyTReKeyFiltered

ZQTableView* DicEdit::setupFieldTBv(ZQTableView* pFieldTBv, bool pColumnAutoAdjust, int pColumns)
{
  int wFilter =  ZEF_Escape | ZEF_CommonControlKeys | ZEF_DragAndDrop ;

  if (pColumnAutoAdjust)
    wFilter |= ZEF_AutoAdjustColumn;
//  GenericTRe = new ZQTreeView((QWidget *)this,ZEF_CommonControlKeys | ZEF_AllowDataEnter );
//  keyTRv = new ZQTreeView(this,ZLayout_Generic, wFilter ); // no data enter

  pFieldTBv->setZLayout(ZLayout_FieldTBv);
  pFieldTBv->setFilterMask(wFilter);

  //  ui->centralwidget->addWidget(GenericTRv);

  pFieldTBv->setSizePolicy(QSizePolicy ::Preferred , QSizePolicy ::Preferred );

  pFieldTBv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  pFieldTBv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  pFieldTBv->setDragEnabled(true);
  pFieldTBv->setDropIndicatorShown(true);
  pFieldTBv->setDataEnter(false);
  pFieldTBv->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  //  GenericTRv->ItemModel=new QStandardItemModel(0,GenericModelColumn,this) ;
  pFieldTBv->setModel(new QStandardItemModel(0,pColumns,pFieldTBv));
  pFieldTBv->ItemModel->setSortRole(ZSortRole);     // Key to have a model sorted on numeric data and not editable data


  // registering callback function : this notation allows to be readable
  //
  std::function<void(int,int)>                                wKeyFilterCBF;
  std::function<void(QContextMenuEvent *)>                    wContextMenuCBF;
  std::function<bool (int,ZDataReference *,ZDataReference *)> wDropEventCBF=nullptr;
  std::function<void ()>                                      wRawStartDrag;
  std::function<bool (QDropEvent *)>                          wRawDropEventCBF;

  wKeyFilterCBF     = std::bind(&DicEdit::FieldTBvKeyFiltered, this,_1,_2);
  wContextMenuCBF   = std::bind(&DicEdit::FieldTBvFlexMenu, this,_1);     // for context menu callback for ZQTreeView
//  wDropEventCBF     = std::bind(&DicEdit::FieldTBvDropEvent, this,_1,_2,_3);     // for drag and drop
  wRawStartDrag     = std::bind(&DicEdit::FieldTBvRawStartDrag, this);
  wRawDropEventCBF  = std::bind(&DicEdit::FieldTBvRawDropEvent, this,_1);

  pFieldTBv->_register(wKeyFilterCBF,wContextMenuCBF,wDropEventCBF);

  pFieldTBv->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  pFieldTBv->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  pFieldTBv->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */


  pFieldTBv->setSupportedDropActions(Qt::CopyAction|Qt::MoveAction);

  pFieldTBv->setRawStartDragCallback(wRawStartDrag);
  pFieldTBv->setRawDropEventCallback(wRawDropEventCBF);

  return pFieldTBv;

  //    PerDomainTRe->expandAll();
}//setupFieldTBv



ZQTreeView *DicEdit::setupKeyTRv(ZQTreeView* pKeyTRv, bool pColumnAutoAdjust, int pColumns)
{
//  FGeneric=true;
  int wFilter =  ZEF_Escape | ZEF_CommonControlKeys | ZEF_DragAndDrop ;

  if (pColumnAutoAdjust)
    wFilter |= ZEF_AutoAdjustColumn;
//        GenericTRe = new ZQTreeView((QWidget *)this,ZEF_CommonControlKeys | ZEF_AllowDataEnter );
//  pKeyTRv = new ZQTreeView(this,ZLayout_Generic, wFilter ); // no data enter

  pKeyTRv->setZLayout(ZLayout_KeyTRv);
  pKeyTRv->setFilterMask(wFilter);

//  ui->centralwidget->addWidget(GenericTRv);

  pKeyTRv->setSizePolicy(QSizePolicy ::Preferred , QSizePolicy ::Preferred );

  pKeyTRv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  pKeyTRv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  pKeyTRv->setDragEnabled(true);
  pKeyTRv->setDropIndicatorShown(true);
  pKeyTRv->setDataEnter(true);
  pKeyTRv->setAcceptDrops(true);


//  GenericTRv->ItemModel=new QStandardItemModel(0,GenericModelColumn,this) ;
  pKeyTRv->setModel(new QStandardItemModel(0,pColumns,pKeyTRv));
  pKeyTRv->ItemModel->setSortRole(ZSortRole);     // Key to have a model sorted on numeric data and not editable data

  // registering callback function : this notation allows to be readable
  //
  std::function<void(int,int)>                                wKeyFilterCBF;
  std::function<void(QContextMenuEvent *)>                    wContextMenuCBF;
  std::function<bool (int,ZDataReference *,ZDataReference *)> wDropEventCBF=nullptr;
  std::function<void ()>                                      wRawStartDragCBF;
  std::function<bool (QDropEvent *)>                          wRawDropEventCBF;

  wKeyFilterCBF     = std::bind(&DicEdit::KeyTRvKeyFiltered, this,_1,_2);
  wContextMenuCBF   = std::bind(&DicEdit::KeyTRvFlexMenu, this,_1);         // for context menu callback for ZQTreeView
 // wDropEventCBF     = std::bind(&DicEdit::KeyTRvDropEvent, this,_1,_2,_3);  // for drag and drop

  wRawStartDragCBF  = std::bind(&DicEdit::KeyTRvRawStartDrag, this);     // for raw drag and drop
  wRawDropEventCBF  = std::bind(&DicEdit::KeyTRvRawDropEvent, this,_1);     // for raw drag and drop

  pKeyTRv->_register(wKeyFilterCBF,wContextMenuCBF,wDropEventCBF);

  pKeyTRv->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  pKeyTRv->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  pKeyTRv->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  pKeyTRv->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  pKeyTRv->setRawStartDragCallback(wRawStartDragCBF);
  pKeyTRv->setRawDropEventCallback(wRawDropEventCBF);

  pKeyTRv->setSupportedDropActions(Qt::CopyAction|Qt::MoveAction);

  return pKeyTRv;
  //    PerDomainTRe->expandAll();
}//setupKeyTRv

/*
bool DicEdit::KeyTRvDropEvent(int pDropType, ZDataReference *pSource, ZDataReference * pTarget)
{
  ZPinboardElement* wDropElement= Pinboard.getLast();
  return true;
}
*/


void DicEdit::KeyTRvRawStartDrag()
{
  QModelIndex wIdx=fieldTBv->currentIndex();
  if (!wIdx.isValid())
    return;
  QVariant wV=wIdx.data(ZFlowRole);
  if (!wV.isValid())
    return;
  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_KeyField)  /* only key field (not key dic) is been allowed to be dragged */
    return;
  wV=wIdx.data(ZQtValueRole);
  if (!wV.isValid())
    return;
  KeyField_Pack wFDPack = wV.value<KeyField_Pack>();
  ZPinboardElement wPBE;
  wPBE.setDataReference(wDRef);
  wPBE.setData(&wFDPack,sizeof(KeyField_Pack));
  Pinboard.push(wPBE,true);
  return;
}

bool DicEdit::KeyTRvRawDropEvent(QDropEvent *pEvent)
{
  ZPinboard* wPinboard=&Pinboard;

  if (Pinboard.isEmpty())
    {
      statusBar()->showMessage(QObject::tr("No drag source element selected","DicEdit"),cst_MessageDuration);
      return false;
    }
  //
  // determines the destination for the drop operation
  //
  QModelIndex wIdx = keyTRv->indexAt (pEvent->pos());
  if (!wIdx.isValid())
    {
      statusBar()->showMessage(QObject::tr("No target element selected","DicEdit"),cst_MessageDuration);
      return false;
    }

  QVariant wV = wIdx.data(ZFlowRole);
  if (!wV.isValid())
    return false;


  /* get the origin of the dragndrop operation */

  if (Pinboard.getLast()->DRef.ZLayout==fieldTBv->Layout ) /* coming from field list */
    {
    return pinboardPasteFieldToKey(wIdx);
    }//if (Pinboard.getLast()->DRef.Layout==ZLayout_KeyTRv )

    if (Pinboard.getLast()->DRef.ZLayout!=keyTRv->Layout ) /* not coming from same widget -> not admitted */
      return false;

    /* same widget : this is a move */

  ui->statusBar->showMessage("Move for key field is not yet implemented.",cst_MessageDuration);
  return false;
}// KeyTRvRawDropEvent
/*
bool DicEdit::FieldTBvDropEvent(int pDropType,ZDataReference *pSource,ZDataReference *pTarget)
{
  return true;
}
*/
void DicEdit::FieldTBvRawStartDrag()
{
  QModelIndex wIdx=fieldTBv->currentIndex();
  if (!wIdx.isValid())
    return;
  if (wIdx.column()!=0)
    wIdx=wIdx.siblingAtColumn(0);

  QVariant wV=wIdx.data(ZFlowRole);
  if (!wV.isValid())
    return;
  ZDataReference wDRef=wV.value<ZDataReference>();
  wDRef.setIndex(wIdx);

  wV=wIdx.data(ZQtValueRole);
  if (!wV.isValid())
    return;
  FieldDesc_Pack wFDPack = wV.value<FieldDesc_Pack>();
  ZPinboardElement wPBE;
  wPBE.setDataReference(wDRef);
  wPBE.setData<FieldDesc_Pack>(wFDPack);
  ZPinboard* wPB=&Pinboard;
  Pinboard.push(wPBE,true);
  return;
}

bool DicEdit::FieldTBvRawDropEvent(QDropEvent *pEvent)
{
  //
  // determines the destination for the drop operation
  //
  QModelIndex wIdx = fieldTBv->indexAt (pEvent->pos());
  if (!wIdx.isValid())
    return false;

  QModelIndex wSIdx = Pinboard.getLast()->DRef.getIndex();
  if (!wSIdx.isValid())
    return false;

  if (!pinboardInsertField(wIdx))
    return false;
  fieldTBv->ItemModel->removeRow(wSIdx.row(),wSIdx.parent());

  Pinboard.pop();
  return true;
}

void
DicEdit::importDic(const unsigned char* pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String wStr;

//  ZMFDictionary wDic;

  if (MasterDic!=nullptr)
    delete MasterDic;

  MasterDic = new ZMFDictionary;

//  ssize_t wSize;
  ZStatus wSt= MasterDic->_import(pPtrIn);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::message("DicEdit::displayZMFDictionaryValues",wSt,Severity_Error,
        "Cannot load successfully dictionary.");
    return ;
  }

  displayZMFDictionary(*MasterDic);
}

void
DicEdit::importDicFromReserved(const unsigned char* pPtrReserved)
{
  if (pPtrReserved==nullptr)
    return;

  ZSMCBOwnData_Export* wMCBExport=(ZSMCBOwnData_Export*)pPtrReserved;

  if (wMCBExport->StartSign!=cst_ZBLOCKSTART)
    {
    ZExceptionDLg::message("DicEdit::importDicFromReserved",ZS_BADMCB,Severity_Error,
        "While loading MCB, found start sign <%X> while expecting <%X>.",wMCBExport->StartSign,cst_ZBLOCKSTART);
    return;
    }
  if (wMCBExport->BlockId!=ZBID_MCB)
    {
    ZExceptionDLg::message("DicEdit::importDicFromReserved",ZS_BADMCB,Severity_Error,
        "While loading MCB, found block id  <%X> while expecting <%X>.",wMCBExport->BlockId,ZBID_MCB);
    return;
    }


    uint32_t wMDicOffset = reverseByteOrder_Conditional<uint32_t>(wMCBExport->MDicOffset);
    uint32_t wMDicSize = reverseByteOrder_Conditional<uint32_t>(wMCBExport->MDicSize);

    importDic(pPtrReserved+wMDicOffset);

}//importDicFromReserved

void
DicEdit::importDicFromFullHeader(ZDataBuffer& pHeaderContent)
{
  utf8String wStr;
  PtrIn=pHeaderContent.Data;
  clear();
  show();
  if (pHeaderContent.Size < sizeof(ZHeaderControlBlock_Export))
  {
/*    utf8String wStr;
    wStr.sprintf("Cannot view Header Control Block : requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",sizeof(ZHeaderControlBlock_Export),pHeaderContent.Size);
    QMessageBox::critical(this,tr("Data structure"),wStr.toCChar());
 */
    ZExceptionDLg::message("DicEdit::importDicFromFullHeader",ZS_MEMOVFLW,Severity_Severe,
        "Cannot view Header Control Block : requested minimum size <%ld> Bytes \n"
        "Only <%ld> Bytes have been loaded.",sizeof(ZHeaderControlBlock_Export),pHeaderContent.Size);

    return;
  }


  ZHeaderControlBlock_Export* wHCBExport=(ZHeaderControlBlock_Export*)pHeaderContent.Data;
  zaddress_type wOffset=reverseByteOrder_Conditional(wHCBExport->OffsetReserved);
  if (pHeaderContent.Size < (wOffset+sizeof(ZSMCBOwnData_Export)))
    {
    ZExceptionDLg::message("DicEdit::importDicFromFullHeader",ZS_MEMOVFLW,Severity_Severe,
        "Cannot view File Control Block : requested minimum size <%ld> Bytes \n"
        "Only <%ld> Bytes have been loaded.",(wOffset+sizeof(ZSMCBOwnData_Export)),pHeaderContent.Size);

    return;
    }
  Offset=wOffset;

  importDicFromReserved(pHeaderContent.Data+wOffset);
}

ZMFDictionary*
DicEdit::screenToDic()
{
  ZMFDictionary* wMetaDic=new ZMFDictionary;
  ZKeyDictionary* wKeyDic=nullptr;

  int wFieldRows=fieldTBv->ItemModel->rowCount();
  QStandardItem* wItem;
  QVariant wV;
  for (int wi=0;wi< wFieldRows;wi++)
    {
    wItem=fieldTBv->ItemModel->item(wi,0);
    wV = wItem->data(ZQtValueRole);
    FieldDesc_Pack wFDPack = wV.value<FieldDesc_Pack>();
    ZFieldDescription wFD = wFDPack.toFieldDescription();
    wMetaDic->push(wFD);
    }
  int wKeyRows=keyTRv->ItemModel->rowCount();
  QStandardItem* wKeyItem;
  for (int wi=0;wi< wKeyRows;wi++)
    {
    wKeyItem=keyTRv->ItemModel->item(wi,0);
    wV = wKeyItem->data(ZQtValueRole);
    KeyDic_Pack wKDPack = wV.value<KeyDic_Pack>();

    wKeyDic = new ZKeyDictionary(wKDPack.getName(),wMetaDic);
    wKeyDic->Duplicates = wKDPack.Duplicates;

    /* key fields */
    for (int wj=0;wj < wKeyItem->rowCount();wj++)
      {
      wItem=wKeyItem->child(wj,0);
      wV = wItem->data(ZQtValueRole);
      KeyField_Pack wKFPack = wV.value<KeyField_Pack>();

      ZIndexField wIxFld;
      wIxFld.MDicRank=-1;
      for (long wF=0;wF< wMetaDic->count();wF++)
        if (wKFPack.hasHash(wMetaDic->Tab[wF].Hash.content))
            {
            wIxFld.MDicRank=wF;
            break;
            }
      if (wIxFld.MDicRank<0)
        {
        fprintf(stderr,"DicEdit::screenToDic-E-KFNTFND Key field <%s> not found in master dictionary (hascode not found). Key field disguarded.\n",wKFPack.getName().toCChar());
        continue;
        }
      wIxFld.KeyOffset = wKFPack.KeyOffset;
      for (int wh=0;wh<cst_md5;wh++)
        wIxFld.Hash.content[wh] = wKFPack.Hash[wh];
      wKeyDic->push(wIxFld);

      }//for (int wj=0;wj < wKeyItem->rowCount();wj++)

    wMetaDic->KeyDic.push(wKeyDic);  /* then push the whole Key dictionary */
    }//for (int wi=0;wi< wKeyRows;wi++)


  wMetaDic->DicName = MasterDic->DicName;
  return wMetaDic;
}//screenToDic


void
DicEdit::displayZMFDictionary(ZMFDictionary &pDic)
{

  utf8String wStr;
  QVariant   wV;
  ZDataReference wDRef;

  if (pDic.DicName.isEmpty())
    ui->DicNameLBl->setText("<no name>");
  else
    ui->DicNameLBl->setText(pDic.DicName.toCChar());

  ui->VersionLBl->setText(getVersionStr(pDic.Version).toCChar());

  wStr.sprintf("%ld",pDic.count());
  ui->FieldsNbLBl->setText(wStr.toCChar());

  wStr.sprintf("%ld",pDic.KeyDic.count());
  ui->KeysNbLBl->setText(wStr.toCChar());

  if (fieldTBv->ItemModel)
    if (fieldTBv->ItemModel->rowCount()>0)
      fieldTBv->ItemModel->removeRows(0,fieldTBv->ItemModel->rowCount());


  int wRow=0;
  QList<QStandardItem *> wFieldRow ;


  for (long wi=0;wi < pDic.count();wi++)
    {
    wFieldRow = createRowFromField(pDic[wi]);  /* create item list from field description */
    /* set up data to item 0 */
    wDRef.setZLayout(ZLayout_FieldTBv);
    wDRef.setResource(getNewResource(ZEntity_DicField));
    wDRef.setRank((long)wi);          /* store rank within meta dictionary */
    wDRef.setPtr(&pDic[wi]);          /* store pointer to dictionary field */
    wV.setValue<ZDataReference>(wDRef);
    wFieldRow[0]->setData(wV,ZFlowRole);

    FieldDesc_Pack wFDPack;
    wFDPack.set(pDic[wi]);
    wV.setValue<FieldDesc_Pack>(wFDPack);
    wFieldRow[0]->setData(wV,ZQtValueRole);

    fieldTBv->ItemModel->appendRow(wFieldRow);

    }// for

  fieldTBv->resizeRowsToContents();
  fieldTBv->resizeColumnsToContents();

  displayKeyDictionaries(pDic);

}
void
DicEdit::displayKeyDictionaries(ZMFDictionary &pDic)
{
  utf8String wStr;
  QVariant   wV;
  ZDataReference wDRefKey , wDRefKeyField;
  QList<QStandardItem *> wKeyRow, wKeyFieldRow ;

  if (keyTRv->ItemModel)
    if (keyTRv->ItemModel->rowCount()>0)
      keyTRv->ItemModel->removeRows(0,keyTRv->ItemModel->rowCount());

  long wMDicRank=0;

  for (long wi=0;wi < pDic.KeyDic.count();wi++)
  {
  wKeyRow.clear();
  wKeyRow << createItem( pDic.KeyDic[wi]->DicKeyName.toCChar());

  wDRefKey.setZLayout(ZLayout_KeyTRv);
  wDRefKey.setResource(getNewResource(ZEntity_KeyDic));
  wDRefKey.setRank((long)wi);
  wDRefKey.setPtr(&pDic.KeyDic[wi]); /* store pointer to key dictionary  */

  wV.setValue<ZDataReference>(wDRefKey);
  wKeyRow[0]->setData(wV,ZFlowRole);

  KeyDic_Pack wKDPack(pDic.KeyDic[wi]);
  wV.setValue<KeyDic_Pack>(wKDPack);
  wKeyRow[0]->setData(wV,ZQtValueRole);

  wKeyRow[0]->setEditable(false);

  /* other characteristics of the key */
  wKeyRow << createItem( wKDPack.KeyUniversalSize,"size: %ld");
  wKeyRow.last()->setEditable(false);
  wKeyRow << createItem( wKDPack.Duplicates?"Yes":"No","dup: %s");
  wKeyRow.last()->setEditable(false);

  for (long wj=0;wj < pDic.KeyDic[wi]->count(); wj++)
    {
    wKeyFieldRow.clear();
    wMDicRank = pDic.KeyDic[wi]->Tab[wj].MDicRank;

    wKeyFieldRow << createItem(pDic[wMDicRank].getName().toCChar());

    wDRefKeyField.clear();

    wDRefKeyField.setResource(getNewResource(ZEntity_KeyField));
    wDRefKeyField.setRank((long)wj);
    wDRefKeyField.setPtr(&pDic.KeyDic[wi]->Tab[wj]); /* store pointer to key field  */

    wV.setValue<ZDataReference>(wDRefKeyField);
    wKeyFieldRow[0]->setData(wV,ZFlowRole);

    KeyField_Pack wKFPack;
    wKFPack.set(*MasterDic,wi,wj);
    wV.setValue<KeyField_Pack>(wKFPack);
    wKeyFieldRow[0]->setData(wV,ZQtValueRole);

    wKeyFieldRow << createItem( wKFPack.ZType,"0x%08X");
//    wKeyFieldRow << createItem( wMDicRank,"%ld");
    wKeyFieldRow << createItem( wKFPack.KeyOffset,"%d");
    wKeyFieldRow << createItem( wKFPack.UniversalSize,"%ld");
    wKeyFieldRow << createItemMd5( wKFPack.Hash);

    wKeyRow[0]->appendRow(wKeyFieldRow);
  /* update index after child row has been appended */
    wDRefKeyField.setIndex( wKeyFieldRow[0]->index());
    wV.setValue<ZDataReference>(wDRefKeyField);
    if (!wV.isValid())
    {
      fprintf(stderr,"DicEdit::displayKeyDictionaries-E-INVVALUE cannot set value to QVariant with ZDataReference.\n");
      abort();
    }
    wKeyFieldRow[0]->setData(wV,ZFlowRole);
    }// for wj

    keyTRv->ItemModel->appendRow(wKeyRow);

    /* update index after child row has been created */
    wDRefKey.setIndex(wKeyRow[0]->index());
    wV.setValue<ZDataReference>(wDRefKey);
    wKeyRow[0]->setData(wV,ZFlowRole);

  }// for wi

//  ui->displayKeyTRv->resizeRowsToContents();
  keyTRv->expandAll();
  keyTRv->resizeColumns();
}



void
DicEdit::clear()
{
  if (fieldTBv->ItemModel)
    if (fieldTBv->ItemModel->rowCount()>0)
      fieldTBv->ItemModel->removeRows(0,fieldTBv->ItemModel->rowCount());

  if (keyTRv->ItemModel)
    if (keyTRv->ItemModel->rowCount()>0)
      keyTRv->ItemModel->removeRows(0,keyTRv->ItemModel->rowCount());
}

DicEdit::~DicEdit()
{
  if (FieldDLg)
    delete FieldDLg;
  if (MasterDic)
    delete MasterDic;

  delete ui;
}

bool
DicEdit::pinboardGetKeyField(int pKeyRank, QList<QStandardItem *> &pKeyFieldRow)
{
  if (Pinboard.isEmpty())
    {
    ui->statusBar->showMessage("No previous data in clipboard.",cst_MessageDuration);
    return false;
    }
  if (Pinboard.last().DRef.getZEntity()!=ZEntity_DicField)
    {
      ui->statusBar->showMessage("Data in clipboard is not appropriate to paste there.",cst_MessageDuration);
      return false;
    }

    ZFieldDescription wFDesc;
    pinboardGetField(wFDesc);
    /* create key field data */
    /* search field within fieldTBv rows */

    QModelIndex wFieldRootIndex = Pinboard.last().DRef.getIndex().parent();
    QModelIndex wFieldIdx ;

    int wRowCount= fieldTBv->ItemModel->rowCount(wFieldRootIndex);

    QVariant wV;
    FieldDesc_Pack wFD_Pack;
    FieldDesc_Pack* wFDPackPtr = Pinboard.last().getDataPtr<FieldDesc_Pack>();
    long wDicFieldRank=-1;
    int wRow=0;
    for (;wRow < wRowCount;wRow++)
      {
        wFieldIdx = fieldTBv->ItemModel->index(wRow,0,wFieldRootIndex);
        if (!wFieldIdx.isValid())
          continue;

        wV=wFieldIdx.data(ZQtValueRole);
        wFD_Pack=wV.value<FieldDesc_Pack>();

        if (wFDPackPtr->hasSameHash(wFD_Pack))
          {
          wDicFieldRank = wRow;
          break;
          }
      }//for (;wRow < wRowCount;wRow++)
#ifdef __COMMENT__
    long wf=0,wMDicFieldRank=-1;
    for (;wf<MasterDic->count();wf++) /* search field within MasterDic : must search within rows */
    {
      if (MasterDic->Tab[wf].Hash==wFDesc.Hash)
      {
        wMDicFieldRank=wf;
        break;
      }
    }//for
#endif//__COMMENT__
    if (wDicFieldRank<0)
      {
      ui->statusBar->showMessage("Cannot find field.",cst_MessageDuration);
      return false;
      }
    KeyField_Pack wKFPack;

    wKFPack.set(wFD_Pack,pKeyRank);
    ZDataReference wDRef;

    /* append row */
    pKeyFieldRow << createItem(wFDesc.getName().toCChar());
    wV.setValue<KeyField_Pack>(wKFPack);
    pKeyFieldRow.last()->setData(wV,ZQtValueRole);

    wDRef.setRank(pKeyRank);
    wDRef.setZLayout(ZLayout_KeyTRv);
    wDRef.setResource(getNewResource(ZEntity_KeyField));
/* item index must be set */
    wV.setValue<ZDataReference>(wDRef);
    pKeyFieldRow.last()->setData(wV,ZFlowRole);

    pKeyFieldRow << createItem(wFDesc.ZType,"0x%08X");
//    pKeyFieldRow << createItem(wFieldRank,"%ld");
    pKeyFieldRow << createItem(wKFPack.KeyOffset,"%d");
    pKeyFieldRow << createItem(wFDesc.UniversalSize,"%ld");
    pKeyFieldRow << createItem(wFDesc.Hash.toHexa().toChar());

    Pinboard.pop();

    return true;
}//pinboardGetKeyField



ZDataReference
getZDataReference(QStandardItemModel *pModel, int pRow)
{
  QStandardItem* wItem= pModel->item(pRow,0);
  QVariant wV=wItem->data(ZFlowRole);
  if (!wV.isValid())
    return ZDataReference(); /* return invalid DRef */
  ZDataReference wDRef=wV.value<ZDataReference>();
  return wDRef;
}

/* creates a list pointing to effective items */
QList<QStandardItem*> itemRow (QStandardItemModel*pModel,QModelIndex& pIdx)
{
  QList<QStandardItem*> wRow;
  if (!pIdx.isValid())
    {
    fprintf(stderr,"getAllKeyRows-E-INVIDX Index is not valid.\n");
    return wRow;
    }

  /* get the number of columns for this kind of tree lines */
  int wColumns = pModel->columnCount(pIdx.parent());

  QStandardItem* wFatherItem= pModel->itemFromIndex(pIdx.parent()); /* general case get parent item */

  int wi=0;
  if (wFatherItem==nullptr) /* if top level */
    while (wi < wColumns)
      {
      QStandardItem* wItem=pModel->item(pIdx.row(),wi++);
      wRow << wItem;
      }
  else
    while (wi < wColumns)
      {
      QStandardItem* wItem=wFatherItem->child(pIdx.row(),wi++);
      wRow << wItem;
      }

  return wRow;
}//itemRow

/* creates a list pointing to cloned items */
QList<QStandardItem*> cloneRow (QStandardItemModel*pModel,QModelIndex& pIdx)
{
  QList<QStandardItem*> wRow;
  if (!pIdx.isValid())
  {
    fprintf(stderr,"getAllKeyRows-E-INVIDX Index is not valid.\n");
    return wRow;
  }

  /* get the number of columns for this kind of tree lines */
  int wColumns = pModel->columnCount(pIdx.parent());

  QStandardItem* wFatherItem= pModel->itemFromIndex(pIdx.parent()); /* general case get parent item */

  int wi=0;
  if (wFatherItem==nullptr) /* if top level */
    while (wi < wColumns)
      {
      QStandardItem* wItem=pModel->item(pIdx.row(),wi++)->clone();
      wRow << wItem;
      }
  else
    while (wi < wColumns)
      {
      QStandardItem* wItem=wFatherItem->child(pIdx.row(),wi++)->clone();
      wRow << wItem;
      }

  return wRow;
}//cloneRow
#ifdef __COMMENT__
/* creates a list pointing to effective items after having cut them */
QList<QStandardItem*> cutRow(QStandardItemModel*pModel,QModelIndex& pIdx)
{
  QList<QStandardItem*> wRow;
  if (!pIdx.isValid())
    return wRow;

  /* get the number of columns for this kind of tree lines */
  int wColumns = pModel->columnCount(pIdx.parent());

  int wi=0;
  while (wi < wColumns)
  {
    QStandardItem* wItem=pModel->takeItem(pIdx.row(),wi++);
    wRow << wItem;
  }
  return wRow;
}//cutRow
#endif // __COMMENT__

/* creates a list pointing to effective items after having cut them */
QList<QStandardItem*> cutRow (QStandardItemModel*pModel,QModelIndex& pIdx)
{
  QList<QStandardItem*> wRow;
  if (!pIdx.isValid())
  {
    fprintf(stderr,"getAllKeyRows-E-INVIDX Index is not valid.\n");
    return wRow;
  }

  /* get the number of columns for this kind of tree lines */
  int wColumns = pModel->columnCount(pIdx.parent());

  QStandardItem* wFatherItem= pModel->itemFromIndex(pIdx.parent()); /* general case get parent item */

  int wi=0;
  if (wFatherItem==nullptr) /* if top level */
    return pModel->takeRow(pIdx.row());
  else
    return wFatherItem->takeRow(pIdx.row());
}//cutRow



QList<QList<QStandardItem*>> DicEdit::cutAllKeyRows(const QModelIndex& pKeyIdx)
{
  QList<QList<QStandardItem*>> wKeyRows;

  if (!pKeyIdx.isValid())
    {
    fprintf(stderr,"getAllKeyRows-E-INVIDX Index is not valid.\n");
    return wKeyRows;
    }
  QModelIndex wKeyIdx=pKeyIdx;
  if (pKeyIdx.column()!=0)
    wKeyIdx=pKeyIdx.siblingAtColumn(0);

  QVariant wV=wKeyIdx.data(ZFlowRole);
  if (!wV.isValid())
    {
    fprintf(stderr,"getAllKeyRows-E-INVDATA Index data is not valid.\n");
    return wKeyRows;
    }

  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_KeyDic)
    {
    fprintf(stderr,"getAllKeyRows-E-INVROW Index is not a Key Dictionary.\n");
    return wKeyRows;
    }

  QStandardItem* wItem = keyTRv->ItemModel->itemFromIndex(wKeyIdx);

  int wRNb=wItem->rowCount();
  int wRow=0;
  while (wRow < wRNb)
    {
    wKeyRows << wItem->takeRow(wRow);
    wRow++;
    }//while (wRow < wRNb)

//  wItem->removeRows(0,wRNb);

  int wCRow=wKeyIdx.row();
  /* then insert key dictionary row at rank 0 */
  wKeyRows.insert(0,keyTRv->ItemModel->takeRow(wCRow));
  /* remove key dictionary empty row */
//  keyTRv->ItemModel->removeRows(wCRow,1);
  return wKeyRows;
}


bool DicEdit::pasteAllKeyRows(const QModelIndex& pKeyIdx,QList<QList<QStandardItem*>>& pKeyRows)
{
  if (!pKeyIdx.isValid())
    {
    fprintf(stderr,"pasteAllKeyRows-E-INVIDX Index is not valid.\n");
    return false;
    }
  QModelIndex wKeyIdx=pKeyIdx;
  if (pKeyIdx.column()!=0)
    wKeyIdx=pKeyIdx.siblingAtColumn(0);

  QVariant wV=wKeyIdx.data(ZFlowRole);
  if (!wV.isValid())
    {
    fprintf(stderr,"pasteAllKeyRows-E-INVDATA Index data is not valid.\n");
    return false;
    }

  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_KeyDic)
    {
    fprintf(stderr,"pasteAllKeyRows-E-INVROW Index is not a Key Dictionary.\n");
    return false;
    }




  return true;
}
