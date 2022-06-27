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

#include <zqt/zqtwidget/zqstandarditem.h>

#include <zqt/zqtwidget/zdatareference.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <zentity.h>

#include <zfielddlg.h>
#include <zkeydlg.h>

#include <zcontent/zindexedfile/zfielddescription.h>

#include <zcontent/zindexedfile/zdictionaryfile.h>

#include <qclipboard.h>
#include <qmimedata.h>
#include <qfiledialog.h>

//#include <qtreeview.h>

#include <qaction.h>
#include <qactiongroup.h>
#include <zindexedfile/zfielddescription.h>
#include <zindexedfile/zkeydictionary.h>
#include <zindexedfile/zindexfield.h>


#include <texteditmwn.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include <rawfields.h>

#include <zindexedfile/zdictionaryfile.h>

#include <zstdlistdlg.h>

using namespace zbs;

const int cst_MessageDuration = 20000;  /* milliseconds */
int cst_KeyOffsetColumn = 2 ;
int cst_KeyUSizeColumn = 1 ;


extern ZPinboard   Pinboard;
extern bool        UseMimeData;
extern bool        DragEngaged;

DicEdit::~DicEdit()
{
  if (FieldDLg)
    delete FieldDLg;
  if (MasterDic)
    delete MasterDic;

  delete ui;
}
void
DicEdit::displayErrorCallBack(const utf8VaryingString& pMessage) {
    statusBar()->showMessage(QObject::tr(pMessage.toCChar()),cst_MessageDuration);
}

DicEdit::DicEdit(QWidget *parent) : QMainWindow(parent),ui(new Ui::DicEdit)
{
  ui->setupUi(this);

  setWindowTitle("Master Dictionary");

  Errorlog.setAutoPrintOn(ZAIES_Text);
  Errorlog.setDisplayCallback(std::bind(&DicEdit::displayErrorCallBack, this,_1));


  ui->DicNameLBl->setText ("No dictionary name");
  ui->VersionLBl->setText ("0.0-0");
  ui->KeysNbLBl ->setText ("0");
  ui->FieldsNbLBl->setText("0");


/*  ui->displayTBv->setStyleSheet(QString::fromUtf8("QTableView::item{border-left : 1px solid black;\n"
                                                "border-right  : 1px solid black;\n"
                                                "font: 75 12pt \"Courier\";\n"
                                                " }"));
*/
  fieldTBv = setupFieldTBv( ui->displayDicTBv,true,10);/* 10 columns */

  FieldSM=fieldTBv->selectionModel();

  int wCol=0;
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Name")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType code")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType symbols")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Capacity")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("HeaderSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("UniversalSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("NaturalSize")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("KeyEligible")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Hash")));
  fieldTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Tooltip")));

  fieldTBv->setShowGrid(true);

  fieldTBv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
  fieldTBv->setWordWrap(false);

  fieldTBv->setSupportedDropActions(Qt::CopyAction);

  fieldTBv->addFilterMask(ZEF_DoubleClick);

  fieldTBv->setAlternatingRowColors(true);

  fieldTBv->show();

  /* for key fields */



  keyTRv = setupKeyTRv(ui->displayKeyTRv,true,5); /* 5 columns */
  KeySM=keyTRv->selectionModel();

  wCol=0;
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Name")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("ZType code")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("KeyOffset")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("UniversalSize")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Hash")));
  keyTRv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Tooltip")));


//  ui->displayKeyTRv->setShowGrid(true);

  keyTRv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
  keyTRv->setWordWrap(false);

  keyTRv->setSupportedDropActions(Qt::CopyAction);

  keyTRv->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  keyTRv->setAlternatingRowColors(true);

  keyTRv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

  keyTRv->show();

  ui->ClosedLBl->setVisible(false);


  generalMEn=new QMenu(QObject::tr("general","DicEdit"),ui->menubar);
  ui->menubar->addMenu(generalMEn);

  generalGroup = new QActionGroup(generalMEn);

  parserQAc= new QAction(QObject::tr("c++ parser","DicEdit"),generalMEn);
  parserQAc->setObjectName("parserQAc");
  generalGroup->addAction(parserQAc);
  generalMEn->addAction(parserQAc);

  setupReadWriteMenu();

  generalGroup->addAction(FwritetoclipQAc);
  generalGroup->addAction(FwriteXmltofileQAc);
  generalGroup->addAction(FviewXmlQAc);
  generalGroup->addAction(FwritetoDicQAc);

  generalGroup->addAction(FloadfromclipQAc);
  generalGroup->addAction(FloadfromXmlFileQAc);
  generalGroup->addAction(FloadfromDicQAc);


  generalMEn->addMenu(FloadMEn);
  generalMEn->addMenu(FwritetoMEn);

  quitQAc= new QAction(QObject::tr("quit","DicEdit"),generalMEn);
  quitQAc->setObjectName("quitQAc");
  generalGroup->addAction(quitQAc);
  generalMEn->addAction(quitQAc);

  QObject::connect(generalGroup, SIGNAL(triggered(QAction*)), this, SLOT(generalActionEvent(QAction*)));

}
void
DicEdit::generalActionEvent(QAction* pAction) {

  if (pAction == parserQAc) {
    if (rawFields==nullptr) {
      rawFields = new RawFields( "/home/gerard/Development/zbasetools/zcontent/ztest_zindexedfile/testdata/zcppparserparameters.xml", this);
    }
  rawFields->showAll();
  rawFields->setFocus();
  } // parserQAc
  if (pAction == quitQAc) {
    Quit();
  } // parserQAc

  readWriteActionEvent(pAction);
}

void
DicEdit::Quit() {

  int wRet=ZExceptionDLg::message2B("DicEdit::Quit",ZS_EXCEPTION,Severity_Information,"Continue","Quit",
      "Are you sure you want to quit ?");
  if (wRet==QDialog::Rejected)
    return;
  QApplication::quit();
}

void
DicEdit::clear()
{
//  fieldTBv->ItemModel->clear();
//  keyTRv->I
  clearAllRows();

  ui->VersionLBl->clear();
  ui->FieldsNbLBl->clear();
  ui->KeysNbLBl->clear();
  ui->DicNameLBl->clear();
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
  FInsertBeforeQAc->setText(QCoreApplication::translate("DicEdit", "Insert new before", nullptr));
  FInsertBeforeQAc->setObjectName("FInsertBeforeQAc");

  FInsertAfterQAc= new QAction(fieldFlexMEn);
  FInsertAfterQAc->setText(QCoreApplication::translate("DicEdit", "Insert new after", nullptr));
  FInsertAfterQAc->setObjectName("FInsertAfterQAc");

  FAppendQAc= new QAction(fieldFlexMEn);
  FAppendQAc->setText(QCoreApplication::translate("DicEdit", "Append new", nullptr));
  FAppendQAc->setObjectName("FAppendQAc");

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

  FpasteInsertBeforeQAc= new QAction(fieldFlexMEn);
  FpasteInsertBeforeQAc->setText(QCoreApplication::translate("DicEdit", "Paste before", nullptr));
  FpasteInsertBeforeQAc->setObjectName("FpasteInsertBeforeQAc");

  FpasteInsertAfterQAc= new QAction(fieldFlexMEn);
  FpasteInsertAfterQAc->setText(QCoreApplication::translate("DicEdit", "Paste after", nullptr));
  FpasteInsertAfterQAc->setObjectName("FpasteInsertAfterQAc");


  FpasteAppendQAc= new QAction(fieldFlexMEn);
  FpasteAppendQAc->setText(QCoreApplication::translate("DicEdit", "Paste append", nullptr));
  FpasteAppendQAc->setObjectName("FpasteAppendQAc");

  fieldFlexMEn->addAction(FInsertBeforeQAc);
  fieldActionGroup->addAction(FInsertBeforeQAc);
  fieldFlexMEn->addAction(FInsertAfterQAc);
  fieldActionGroup->addAction(FInsertAfterQAc);

  fieldFlexMEn->addAction(FAppendQAc);
  fieldActionGroup->addAction(FAppendQAc);

  fieldFlexMEn->addAction(FDeleteQAc);
  fieldActionGroup->addAction(FDeleteQAc);


  fieldFlexMEn->addSeparator();
  fieldFlexMEn->addAction(FCutQAc);
  fieldActionGroup->addAction(FCutQAc);
  fieldFlexMEn->addAction(FcopyQAc);
  fieldActionGroup->addAction(FcopyQAc);

  fieldFlexMEn->addAction(FpasteInsertBeforeQAc);
  fieldActionGroup->addAction(FpasteInsertBeforeQAc);
  fieldFlexMEn->addAction(FpasteInsertAfterQAc);
  fieldActionGroup->addAction(FpasteInsertAfterQAc);

  fieldFlexMEn->addAction(FpasteAppendQAc);
  fieldActionGroup->addAction(FpasteAppendQAc);

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

void DicEdit::fieldActionEvent(QAction* pAction)
{
  utf8String wMsg;
  if (pAction->objectName()=="FInsertBeforeQAc")
  {
    QModelIndex wIdx=fieldTBv->currentIndex();
    fieldInsertNewBefore(wIdx);
    return;
  }
  if (pAction->objectName()=="FInsertAfterQAc")
  {
    QModelIndex wIdx=fieldTBv->currentIndex();
    fieldInsertNewAfter(wIdx);
    return;
  }

  if (pAction->objectName()=="FAppendQAc")
  {
    if (fieldAppend()) {
      ui->statusBar->showMessage(QObject::tr("1 field created and appended","DicEdit"),cst_MessageDuration);
    }
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
    if (_fieldDelete(fieldTBv->currentIndex())) {
      ui->statusBar->showMessage(QObject::tr("1 field deleted","DicEdit"),cst_MessageDuration);
    }
    return;
  }

  if (pAction->objectName()=="FcopyQAc") {

    if (fieldCopyToPinboard(fieldTBv->currentIndex()))
        ui->statusBar->showMessage(QObject::tr("1 field copied","DicEdit"),cst_MessageDuration);
    return;
  } // FcopyQAc

  if (pAction->objectName()=="FCutQAc") {
    if (!fieldCopyToPinboard(fieldTBv->currentIndex()))
      return;
    if (_fieldDelete(fieldTBv->currentIndex()))
      ui->statusBar->showMessage(QObject::tr("1 field cut","DicEdit"),cst_MessageDuration);
    return;
  } // FCutQAc

  if (pAction==FpasteInsertBeforeQAc)
  {
    if (fieldInsertBeforeFromPinboard(fieldTBv->currentIndex())) {
        ui->statusBar->showMessage(QObject::tr("1 field inserted","DicEdit"),cst_MessageDuration);
    }
    return;
  }
  if (pAction==FpasteInsertAfterQAc) {
    if (fieldInsertAfterFromPinboard(fieldTBv->currentIndex())) {
      ui->statusBar->showMessage(QObject::tr("1 field inserted","DicEdit"),cst_MessageDuration);
    }
    return;
  }
  if (pAction==FpasteAppendQAc) {
    if (fieldAppendFromPinboard()) {
      ui->statusBar->showMessage(QObject::tr("1 field pasted","DicEdit"),cst_MessageDuration);
    }
    return;
  }

  readWriteActionEvent(pAction);  /* call the common menu actions */

  return;
}//fieldActionEvent


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
    keyInsert(keyTRv->currentIndex());
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
    QVariant wV=wIdx.data(ZQtDataReference);
    ZDataReference wDRef=wV.value<ZDataReference>();

    if (wDRef.getZEntity()==ZEntity_KeyDic) {
      keyDelete(wIdx);
      return;
    }
    if (wDRef.getZEntity()!=ZEntity_KeyField) {
      Errorlog.errorLog("KDeleteQAc-E-INVTYP Invalid infradata type <%s> for row.",wDRef.getZEntity());
    }

    _keyfieldDelete(wIdx);
    return;
  }// KDeleteQAc

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
    QVariant wV=wIdx.data(ZQtDataReference);
    ZDataReference wDRef=wV.value<ZDataReference>();

    if (wDRef.getZEntity()==ZEntity_KeyDic)
      {
      keyCutToPinboard(wIdx);
      return;
      }

    if (wDRef.getZEntity()!=ZEntity_KeyField) {

    }
    if (!keyFieldCopyToPinboard(wIdx))
        return;
    _keyfieldDelete(wIdx);
    return;
    }// KcutQAc


  if (pAction->objectName()=="KpasteQAc") { /* paste from pinboard */
    keyTRvInsertFromPinboard(keyTRv->currentIndex());
    return;
    }//KpasteQAc

    if (pAction->objectName()=="KappendQAc") { /* append from pinboard */

      keyTRvAppendFromPinboard(keyTRv->currentIndex());

    }// KappendQAc

    readWriteActionEvent(pAction);  /* call the common menu actions */

  return;
}

bool
DicEdit::getDicName(unsigned long &pVersion,utf8VaryingString& pDicName)
{
  utf8VaryingString wDicName;
  if (pDicName.isEmpty())
    wDicName="<no name>";
  else
    wDicName=pDicName;
  utf8VaryingString wVersion=getVersionStr(pVersion);

  QDialog wDicNameDLg;
  wDicNameDLg.setObjectName("wDicNameDLg");
  wDicNameDLg.setWindowTitle(QObject::tr("Dictionary name","DicEdit"));
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
  QLineEdit* wDicNameLEd=new QLineEdit(wDicName.toCChar());
  wDicNameLEd->setObjectName("wDicNameLEd");
  QHL->addWidget(wDicNameLEd);

  QHBoxLayout* QHL1=new QHBoxLayout;
  QHL1->setObjectName("QHL1");
  QVL->insertLayout(1,QHL1);
  QLabel* wLb1=new QLabel(QObject::tr("Version number","DicEdit"),&wDicNameDLg);
  wLb->setObjectName("wLb1");
  QHL1->addWidget(wLb1);
  QLineEdit* wVersionLEd=new QLineEdit(wVersion.toCChar());
  wVersionLEd->setObjectName("wVersionLEd");
  QHL1->addWidget(wVersionLEd);

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
  wDicNameLEd->setSelection(0,wDicName.UnitCount);

  wDicNameDLg.setWindowTitle(QObject::tr("Dictionary identification","DicEdit"));

  QObject::connect(wOk, &QPushButton::clicked, &wDicNameDLg, &QDialog::accept);
  QObject::connect(wCancel, &QPushButton::clicked, &wDicNameDLg, &QDialog::reject);

  int wRet=wDicNameDLg.exec();
  if (wRet==QDialog::Accepted)
    {
    pDicName=wDicNameLEd->text().toUtf8().data();
    pVersion = getVersionNum(wVersionLEd->text().toUtf8().data());
    return true;
    }
  return false;
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
    if (wMasterDic==nullptr)
      return;
    getDicName(wMasterDic->Version,wMasterDic->DicName);         /* name meta dictionary */

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
    if (Errorlog.hasMessages())
    {
      Severity_type wS = Errorlog.getSeverity();

      int wRet = ZExceptionDLg::message2BWAdd("FloadfromXmlFileQAc",
          wS > Severity_Warning?ZS_ERROR:ZS_WARNING,
          wS,
          Errorlog.allLoggedToString(),
          "Stop","Continue",
          "Some warning messages have been issued during load of xml definition from clipboard");
      if (wRet==QDialog::Rejected)
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
    getDicName(wMasterDic->Version,wMasterDic->DicName);

    utf8String wXmlDic=wMasterDic->XmlSaveToString(true);

    delete wMasterDic;

    /* display xml content */
    textEditMWn* wTEx=new textEditMWn((QWidget*)this,
                                        TEOP_Nothing, /*no line numbers and close button is really closing and destroying dialog */
                                        nullptr);   /* no close callback */
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
    getDicName(wMasterDic->Version,wMasterDic->DicName);

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
    if (Errorlog.hasMessages())
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
DicEdit::keyInsert(const QModelIndex &pIdx) {
  QVariant wV;
  ZDataReference wDRef;
  ZKeyHeaderRow wKHR(MasterDic);

  ZKeyDLg* wKeyDLg=new ZKeyDLg(MasterDic);
  wKeyDLg->setWindowTitle("Key");

  wKeyDLg->setCreate();

  int wRet=wKeyDLg->exec();
  if (wRet==QDialog::Rejected) {
    wKeyDLg->deleteLater();
    return false;
  }

  wKHR.set(wKeyDLg->get());
  wKeyDLg->deleteLater();
  ZKeyDictionary* wKDic=new ZKeyDictionary(wKeyDLg->get());
  return _keyInsert(pIdx,wKDic);
}

bool
DicEdit::_keyInsert (const QModelIndex& pIdx,ZKeyDictionary* pKey) {
  QVariant wV;
  ZDataReference  wDRef;
  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;
  /* define key index (row and array rank are same) to insert before */

  if ((keyTRv->ItemModel->rowCount()==0)||(!pIdx.isValid())) {
    return _keyAppend(pKey);
  }

  /* find key item to insert before */
  QStandardItem* wKeyItem=keyTRv->ItemModel->item(pIdx.row(),0);
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.getZEntity()==ZEntity_KeyField) {
    wKeyItem=wKeyItem->parent();  /* jump to key item */
  } else if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    Errorlog.errorLog("DicEdit::_keyInsert-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }
  /* create and insert key row */
  wKHR=new ZKeyHeaderRow(pKey);
  QList<QStandardItem*> wKeyRow = createKeyDicRow(wKHR);
  keyTRv->ItemModel->insertRow(wKeyItem->row(),wKeyRow);

  /* insert whole key data within dictionary */
  long wKeyRank=MasterDic->KeyDic.insert(pKey,wKeyItem->row());

  /* create and link infradata to key item */
  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyDic),wKeyRank);
  wV.setValue<ZDataReference>(wDRef);
  wKeyRow[0]->setData(wV,ZQtDataReference);

  /* create and insert key field rows individually */
  QList<QStandardItem*> wKeyFieldRow;
  for (long wi=0; wi < wKHR->count();wi++) {
    wKeyFieldRow.clear();
    /* create data to stick to field row */
    wKFR = new ZKeyFieldRow(pKey->Tab[wi]);
//    wKFR->set(pKey->Tab[wi]);
    /* create row from this data */
    wKeyFieldRow=createKeyFieldRow(*wKFR);
    /* stick this data as infradata to field row */
    wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyField),wi);
    wDRef.setPtr(wKFR );
    wV.setValue<ZDataReference>(wDRef);
    wKeyFieldRow[0]->setData(wV,ZQtDataReference);
    /* append row to mother key row */
    wKeyRow[0]->appendRow(wKeyFieldRow);
  }// for


  return true; /* done */

} // _keyInsert

bool
DicEdit::keyChange(QModelIndex pIdx) {

  if (!pIdx.isValid()) {
    return false;
  }
  QVariant wV;
  ZDataReference wDRef;
  ZKeyHeaderRow*  wKHR=nullptr;

  if (pIdx.row()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }

  QStandardItem* wKeyItem=keyTRv->ItemModel->itemFromIndex(pIdx);
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.getZEntity()==ZEntity_KeyField) {
    wKeyItem=wKeyItem->parent();  /* if field, jump to key item */
  } else if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    Errorlog.errorLog("DicEdit::_keyCopy-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }
  /* extract infradata again - now it has been validated keyItem points to the correct item */
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  wKHR=wDRef.getPtr<ZKeyHeaderRow*>();

  ZKeyDLg* wKeyDLg=new ZKeyDLg(MasterDic);
  wKeyDLg->setWindowTitle("Key");

  wKeyDLg->set(wKHR);

  int wRet=wKeyDLg->exec();
  if (wRet==QDialog::Rejected) {
    wKeyDLg->deleteLater();
    return false;
  }

  wKHR->set(wKeyDLg->get()); /* get back modified data and by the way update infradata */

  wKeyDLg->deleteLater();

  QModelIndex wIdx= wKeyItem->index(); /* get the correct index from key item */

  /* update dictionary data */

  MasterDic->KeyDic[wIdx.row()]->DicKeyName = wKHR->DicKeyName;
  MasterDic->KeyDic[wIdx.row()]->ToolTip = wKHR->ToolTip;
  MasterDic->KeyDic[wIdx.row()]->Duplicates = wKHR->Duplicates;
  /* infradata has been modified previously */

  /* change rows content */

  QList<QStandardItem*> wRow = itemRow(fieldTBv->ItemModel,wIdx, &Errorlog);
  utf8VaryingString wStr;
  int Col=0;
  wRow[Col]->setText(wKHR->DicKeyName.toCChar());
  Col++;
  Col++;
  wStr.sprintf("total size: %ld",wKHR->KeyUniversalSize);
  wRow[Col]->setText(wStr.toCChar());
  Col++;
//  wStr.sprintf("%s",wKHR->Duplicates?"Duplicates":"No duplicates");
  wRow[Col]->setText(wKHR->Duplicates?"Duplicates":"No duplicates");
  Col++;
  wRow[Col]->setText(wKHR->ToolTip.toCChar());
  Col++;

  return true;
}


bool
DicEdit::keyAppend () {

  ZKeyDLg* wKeyDLg=new ZKeyDLg(MasterDic);
  wKeyDLg->setWindowTitle("Key");

  wKeyDLg->setCreate();

  int wRet=wKeyDLg->exec();
  if (wRet==QDialog::Rejected) {
    wKeyDLg->deleteLater();
    return false;
  }

  return _keyAppend(wKeyDLg->get());
}

bool
DicEdit::keyAppendFromPinboard() {
  if (Pinboard.isEmpty())
    return false;

  ZKeyFieldRow* wKFR=nullptr;
  ZDataReference wDRef ;
  QVariant wV;
  QList<QStandardItem *>  wKeyFieldRow ;
  QStandardItem* wKeyItem=nullptr;
  wDRef=Pinboard.last().DRef;

  if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    return false;
  }
  ZKeyDictionary* wKeyDic= wDRef.getPtr<ZKeyDictionary*>();
  if (_keyAppend(wKeyDic)) {
    Pinboard.pop();
    return true;
  }
  return false;
} //keyAppendFromPinboard


bool
DicEdit::_keyAppend (ZKeyDictionary* pKey) {
  QVariant wV;
  ZDataReference wDRef;
  ZKeyHeaderRow* wKHR = new ZKeyHeaderRow(pKey);
  ZKeyFieldRow* wKFR=nullptr;

  ZKeyDictionary* wKey = pKey ;
  long wKeyRank=MasterDic->KeyDic.push(wKey);

  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyDic),wKeyRank);
  wDRef.setPtr(wKHR);
  wV.setValue<ZDataReference>(wDRef);

  QList<QStandardItem *> wKeyRow = createKeyDicRow(wKHR);
  keyTRv->ItemModel->appendRow(wKeyRow);
  wKeyRow[0]->setData(wV,ZQtDataReference);

  /* now create and append all children rows if any */

  QList<QStandardItem *> wKeyFieldRow;

  for (long wi=0;wi < wKey->count(); wi++) {
    wKFR=new ZKeyFieldRow(wKey->Tab[wi]);
    wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyField),wi);
    wDRef.setPtr(wKFR);
    wV.setValue<ZDataReference>(wDRef);

    wKeyFieldRow=createKeyFieldRow(*wKFR);
    wKeyFieldRow[0]->setData(wV,ZQtDataReference);
    wKeyRow.append(wKeyFieldRow);
  }

  return true;
}



bool
DicEdit::keyCutToPinboard(const QModelIndex &pIdx) {
  if (!keyCopyToPinboard(pIdx))
    return false;
  return _keyDelete(pIdx);
}

bool
DicEdit::keyCopyToPinboard (QModelIndex pIdx) {
  QVariant wV;
  ZDataReference  wDRef;
  ZKeyHeaderRow* pKHR=nullptr;

  /* define key index (row and array rank are same) to insert before */

  if (!pIdx.isValid()) {
    Errorlog.errorLog("DicEdit::_keyCopy-E-INVIDX Invalid row index.");
    return false;
  }
  if (pIdx.row()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }
  wV=pIdx.data(ZQtDataReference);
  wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    Errorlog.errorLog("DicEdit::keyfieldInsertFromTemporary-E-INVTYP Invalid infradata entity type <%s> while expected <ZEntity_KeyDic>.",
        decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }
  pKHR=wDRef.getPtr<ZKeyHeaderRow*>();

  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyDic),-1);
  wDRef.setPtr(new ZKeyHeaderRow(pKHR));

  ZPinboardElement wPElt;
  wPElt.setDataReference(wDRef);
  Pinboard.push(wPElt);
  utf8VaryingString wMsg;
  wMsg.sprintf("Key <%s> copied. %ld rows copied.",
      pKHR->DicKeyName.toCChar(),
      pKHR->count() );
  statusBar()->showMessage(QObject::tr(wMsg.toCChar()),cst_MessageDuration);
  return true;
} // _keyCopy


/**
 * @brief DicEdit::_keyDelete deletes key pointed by pIdx :
 *  - delete key row and deletes infradata associated to it
 *  - deletes all children field rows and infradata associated with QStandardItem at column 0 for children fields
 *  - deletes Key dictionary Key element and all dependent fields.
 */
bool
DicEdit::_keyDelete (const QModelIndex& pIdx) {
  QVariant wV;
  ZDataReference  wDRef , wFDRef;
  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;

  /* define key index (row and array rank are same) to insert before */

  if (!pIdx.isValid()) {
    Errorlog.errorLog("DicEdit::_keyDelete-E-INVIDX Invalid row index.");
    return false;
  }

  QStandardItem* wKeyItem=keyTRv->ItemModel->item(pIdx.row(),0);
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_KeyDic)
    abort();
  wKHR=wDRef.getPtr<ZKeyHeaderRow*>();



  if (wDRef.getZEntity()==ZEntity_KeyField) {
    wKeyItem=wKeyItem->parent();  /* if field, jump to key item */
  } else if (wDRef.getZEntity()!=ZEntity_KeyDic) {
    Errorlog.errorLog("DicEdit::_keyDelete-E-INVTYP Invalid infradata entity type <%s> ",decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }

  delete wKHR; /* delete infradata for key row */

  /* delete infradata for each field */
  QStandardItem* wFieldItem=nullptr;
  for (int wi=0;wi < wKeyItem->rowCount();wi++)  {
    wFieldItem=wKeyItem->child(wi,0);
    wV=wFieldItem->data(ZQtDataReference);
    if (!wV.isValid())
      abort();
    wDRef = wV.value<ZDataReference>();
    if (wDRef.getZEntity()==ZEntity_KeyField)
      wKFR=wDRef.getPtr<ZKeyFieldRow*>();
    else
      abort();
    delete wKFR;
  }
  wKeyItem->removeRows(0,wKeyItem->rowCount());

  /* delete dictionary data */
  MasterDic->KeyDic.erase(wKeyItem->row());

  /* remove key row (key is the first child of root element) */

  keyTRv->ItemModel->removeRow(pIdx.row());
  return true;
} // _keyDelete

/** @brief DicEdit::_keyDelete deletes key field pointed by pIdx :
 *  - delete key field row and deletes infradata associated to it
 *  - recomputes and displays key universal size value and each key field offset within the mother key.
 */
bool
DicEdit::_keyfieldDelete (QModelIndex pIdx) {
  QVariant wV;
  ZDataReference  wDRef , wFDRef;
  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;

  /* define key index (row and array rank are same) to insert before */

  if (!pIdx.isValid()) {
    Errorlog.errorLog("DicEdit::_keyDelete-E-INVIDX Invalid row index.");
    return false;
  }
  if (pIdx.column()!=0) {
    pIdx = pIdx.siblingAtColumn(0);
  }

  wV=pIdx.data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  wKFR=wDRef.getPtr<ZKeyFieldRow*>();

  if (wDRef.getZEntity()!=ZEntity_KeyField) {
    Errorlog.errorLog("DicEdit::_keyDelete-E-INVTYPE Invalid row type. Expected <ZEntity_KeyField> while got <%s>",wDRef.getZEntity());
    return false;
  }
  /* get to item at column 0 (contains infradata) */
  QStandardItem* wKeyFieldItem=keyTRv->ItemModel->itemFromIndex(pIdx);

  /* delete infradata */
  delete wKFR;

  /* get mother key item infradata */
  QStandardItem* wKeyItem=wKeyFieldItem->parent();
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  wKHR=wDRef.getPtr<ZKeyHeaderRow*>();

  /* delete key dictionary key field element */
  MasterDic->KeyDic[wKeyItem->row()]->erase(wKeyFieldItem->row());

  /* delete key field row */
  wKeyItem->removeRow(wKeyItem->row());

  /* recompute values for key Universal Size and key field offsets */

  _recomputeKeyValues(wKeyItem);

  return true;  /* done */
} // _keyDelete


/** @brief DicEdit::_recomputeKeyValues  recomputes and displays for each field key offset and key Universal Size for the key  */
void DicEdit::_recomputeKeyValues(QStandardItem* wKeyItem) {

  QVariant wV;
  ZDataReference  wDRef , wFDRef;
  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;

  int wUniversalSize=0, wOffset=0 ;
  QStandardItem* wKField=nullptr;
  QStandardItem* wKFOffset=nullptr;
  QStandardItem* wKUSize=nullptr;
  utf8VaryingString wStr;

  /* get mother key item infradata */
  wV=wKeyItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
  wKHR=wDRef.getPtr<ZKeyHeaderRow*>();

  for (int wi = 0 ; wi < wKeyItem->rowCount() ; wi ++ ) {
    wKField=wKeyItem->child(wi,0);
    wV=wKField->data(ZQtDataReference);
    wDRef = wV.value<ZDataReference>();
    wKFR=wDRef.getPtr<ZKeyFieldRow*>();
    /* compute values and change infradata offset value */
    wKFR->KeyOffset = wOffset ;
    wOffset += wKFR->UniversalSize;
    wUniversalSize += wKFR->UniversalSize;
    /* display field offset new value */
    wKFOffset=wKeyItem->child(wi,cst_KeyOffsetColumn);  /* get offset item */
    wStr.sprintf("%ld", wKFR->KeyOffset);
    wKFOffset->setText(wStr.toCChar());   /* update displayed value */
  } // for

  /* update infradata */
  wKHR->KeyUniversalSize = wUniversalSize;

  /* update key displayed value */
  /* 'keyTRv->ItemModel->' because key is the uppermost element */
  int wRNb=wKeyItem->row();
  wKUSize = keyTRv->ItemModel->item(wKeyItem->row(),cst_KeyUSizeColumn);
  if (wKUSize==nullptr) {
    abort();
  }

  wStr.sprintf("total size: %d", wUniversalSize);
  wKUSize->setText(wStr.toCChar());   /* update displayed value */

  /* recompute key dictionary values */

  MasterDic->KeyDic[wKeyItem->row()]->computeKeyOffsets();
  MasterDic->KeyDic[wKeyItem->row()]->computeKeyUniversalSize();

  return ;  /* done */
}// _recomputeKeyValues

bool
DicEdit::keyFieldCopyToPinboard (QModelIndex pIdx) {
  if (!pIdx.isValid()) {
    Errorlog.errorLog("DicEdit::keyFieldCopyToPinboard-E-INVIDX Invalid row index.");
    return false;
  }
  if (pIdx.row()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }
  QVariant wV;
  ZDataReference  wDRef;
  ZKeyFieldRow*   wKFR=nullptr;
  wV=pIdx.data(ZQtDataReference);
  wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_KeyField) {
    Errorlog.errorLog("DicEdit::keyfieldInsertFromTemporary-E-INVTYP Invalid infradata entity type <%s> while expected <ZEntity_KeyField>.",
        decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }
  wKFR=wDRef.getPtr<ZKeyFieldRow*>();

  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyField),-1);
  wDRef.setPtr(new ZKeyFieldRow(*wKFR));

  ZPinboardElement wPElt;
  wPElt.setDataReference(wDRef);
  Pinboard.push(wPElt);
  utf8VaryingString wMsg;
  wMsg.sprintf("Key field <%ld> copied to pinboard.",wKFR->Name.toCChar());
  statusBar()->showMessage(QObject::tr(wMsg.toCChar()),cst_MessageDuration);
  return true;
} // _keyFieldCopy

bool
DicEdit::fieldAppendToKey(const QModelIndex &pIdx,ZFieldDescription& pField) {

  if (!pField.KeyEligible) {
    int wRet=ZExceptionDLg::message2B("DicEdit::fieldAppendToKey",ZS_INVTYPE,Severity_Error,
        "Discard","Force",
        "Field %s cannot be part of a key.\n"
        "Using such a field without conversion may induce problems."
        "Do you wish to continue anyway or discard ?",pField.getName().toCChar());
    if (wRet==QDialog::Rejected)
      return false;
  }


  ZKeyHeaderRow wKHR(MasterDic);
  ZKeyFieldRow wKFR;
  ZDataReference wDRefKey , wDRefKeyField;
  QVariant wV;
  QList<QStandardItem *> wKeyRow, wKeyFieldRow ;

  wKFR.Name = pField.getName();
  wKFR.ZType = pField.ZType;
  wKFR.MDicRank = MasterDic->searchFieldByName(wKFR.Name);

  wKFR.UniversalSize = pField.UniversalSize;
  wKFR.Hash = pField.Hash;
  wKFR.KeyOffset = 0; /* to be recomputed */


/* if there is nothing yet in tree view (no key defined) then
 *  - create a key with name __NEW_KEYNAME__
 *  - display this key
 *  - create a key field from given field description (done before)
 *  - append key field to created key
 */
  if (keyTRv->ItemModel->rowCount()==0) {

    /* create a key with name __NEW_KEYNAME__ */
    ZKeyDictionary* wKeyDic=new ZKeyDictionary(__NEW_KEYNAME__,MasterDic);
    long wKeyFieldRank=MasterDic->KeyDic.push(wKeyDic);
    wKHR.set(wKeyDic);

    wKeyRow=createKeyDicRow(wKHR);

    wDRefKey.setZLayout(ZLayout_KeyTRv);
    wDRefKey.setResource(getNewResource(ZEntity_KeyDic));
    wDRefKey.setDataRank(wKeyFieldRank);
    wDRefKey.setPtr(new ZKeyHeaderRow(wKHR)); /* store key dictionary Data (to be deleted) */
    wV.setValue<ZDataReference>(wDRefKey);

    wKeyRow[0]->setData(wV,ZQtDataReference);

    keyTRv->ItemModel->appendRow(wKeyRow); /* display this key */

    wKeyFieldRow=createKeyFieldRow(wKFR);
    /* if there is nothing yet in tree view (no key defined) then
 *  - create a key with name __NEW_KEYNAME__
 *  - display this key
 *  - create a key field from given field description (done before)
 *  - append key field to created key
 */
    ZIndexField wKField = wKFR.get();
    long wKFRank=MasterDic->KeyDic.last()->push( wKField);

    wDRefKeyField.setZLayout(ZLayout_KeyTRv);
    wDRefKeyField.setResource(getNewResource(ZEntity_KeyField));
    wDRefKeyField.setDataRank(wKFRank);
    wDRefKeyField.setPtr(new ZKeyFieldRow(wKFR)); /* store key dictionary Data (to be deleted) */
    wV.setValue<ZDataReference>(wDRefKey);
    wKeyFieldRow[0]->setData(wV,ZQtDataReference);

    wKeyRow[0]->appendRow(wKeyFieldRow); /* append key field to created key */

    _recomputeKeyValues(wKeyRow[0]);

    return true;
  } // if (keyTRv->ItemModel->rowCount()==0)

  /* append to current key :
   * from current model index : if type is ZEntity_KeyField get father
   * create row
   */

  QModelIndex wIdxFather , wIdx = pIdx;
  QStandardItem* wKeyItem=nullptr;

  if (wIdx.column()!=0)
    wIdx=wIdx.siblingAtColumn(0);

  /* get type */
  wV=wIdx.data(ZQtDataReference);
  wDRefKey=wV.value<ZDataReference>();

  if (wDRefKey.getZEntity()==ZEntity_KeyDic) {
    wIdxFather = wIdx;
  }
  else if (wDRefKey.getZEntity()==ZEntity_KeyField) {
    wIdxFather = wIdx.parent();
    wV=wIdxFather.data(ZQtDataReference);
    wDRefKey=wV.value<ZDataReference>();
  } else {
    Errorlog.errorLog("DicEdit::keyfieldAppend-E-INVTYP Invalid infradata entity type <%s> while expected <ZEntity_KeyField>.",decode_ZEntity(wDRefKey.getZEntity()).toChar());
    return false;
  }

  wKeyItem = keyTRv->ItemModel->itemFromIndex(wIdxFather);

  wKeyFieldRow=createKeyFieldRow(wKFR);

  wDRefKeyField.setZLayout(ZLayout_KeyTRv);
  wDRefKeyField.setResource(getNewResource(ZEntity_KeyField));
  wDRefKeyField.setDataRank(wKeyItem->rowCount());
  wDRefKeyField.setPtr(new ZKeyFieldRow(wKFR)); /* store key dictionary Data (to be deleted) */
  wV.setValue<ZDataReference>(wDRefKey);
  wKeyFieldRow[0]->setData(wV,ZQtDataReference);

  wKeyItem->appendRow(wKeyFieldRow);

  _recomputeKeyValues(wKeyItem);

  return true;
} //keyfieldAppend

bool
DicEdit::keyTRvInsertFromPinboard(QModelIndex pIdx) {
  if (Pinboard.isEmpty())
    return false;
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("nothing selected."),cst_MessageDuration);
    return false;
  }
  ZDataReference wDRef ;
  QVariant wV;

  wDRef=Pinboard.last().DRef;

  switch (Pinboard.last().getZEntity()) {
    /* key field move */
    case ZEntity_KeyField: {
      ZKeyFieldRow* wKFR=Pinboard.last().DRef.getPtr<ZKeyFieldRow*>();
      return _keyFieldInsertToKey(pIdx,wKFR);
    }
    /* a whole key must be inserted here */
    case ZEntity_KeyDic: {
      ZKeyHeaderRow* wKHR = Pinboard.last().DRef.getPtr<ZKeyHeaderRow*>();
      return _keyInsert(pIdx,wKHR);
    }
    /* a dictionary field must be converted to key field and inserted here */
    case ZEntity_DicField:{
      ZFieldDescription* wField = Pinboard.last().DRef.getPtr<ZFieldDescription*>();
      return _fieldInsertToKey(pIdx,wField);
    }
    default:
      Errorlog.errorLog("DicEdit::keyfieldInsertFromTemporary-E-INVTYP Invalid infradata entity type <%s> while expected <ZEntity_KeyField>.",decode_ZEntity(wDRef.getZEntity()).toChar());
      return false;
  }//switch

}// keyTRvInsertFromPinboard




bool
DicEdit::_keyFieldInsertToKey(QModelIndex pIdx, ZKeyFieldRow* pKFR) {
  QVariant        wV;
  ZDataReference  wDRef ;
  QStandardItem*  wKeyItem=nullptr;

  /* append to current key :
   * from current model index : if type is ZEntity_KeyField get father
   * create row
   */
  QModelIndex wIdx = pIdx;
  if (wIdx.column()!=0) {
    wIdx=pIdx.siblingAtColumn(0);
  }

  wV=wIdx.data(ZQtDataReference);
  if (wV.isNull()) {
    Errorlog.errorLog("DicEdit::_keyFieldInsertToKey-E-VARNULL Cannot get infradata.");
    return false;
  }
  wDRef=wV.value<ZDataReference>();

  /* if target row is a Key dictionary, simply append inside this key */
  if (wDRef.getZEntity()==ZEntity_KeyDic) {
    wKeyItem=keyTRv->ItemModel->itemFromIndex(wIdx);
    return keyTRvAppendFromPinboard(wIdx);
  }
  else if (wDRef.getZEntity()!=ZEntity_KeyField) {
    Errorlog.errorLog("DicEdit::keyfieldInsertFromTemporary-E-INVTYP Invalid infradata entity type <%s> while expected <ZEntity_KeyField>.",decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }
  /* here this is a key field row */

  pKFR = wDRef.getPtr<ZKeyFieldRow*>();

  QModelIndex wKeyIdx=wIdx.parent();
  wKeyItem=keyTRv->ItemModel->item(wKeyIdx.row(),0);

  /* insert row in tree view */
  QList<QStandardItem*> wRow= createKeyFieldRow(*pKFR);

  /* insert key field in key dictionary */
  ZIndexField wIFld(MasterDic->KeyDic[wKeyItem->row()]);

  wIFld.KeyOffset=0; /* to be recomputed */
  wIFld.Hash = pKFR->Hash;
  wIFld.MDicRank = pKFR->MDicRank;
  //  wIFld.MDicRank=MasterDic->searchFieldByName(pTemp->KFR.Name);
  //  wIFld.KeyDic = MasterDic->KeyDic[wKeyItem->row()]; // already done with ZIndexField CTOR

  long wFldDicRank=MasterDic->KeyDic[wKeyItem->row()]->insert(wIFld,wIdx.row());

  /* update row infradata */

  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyField),wFldDicRank);
  pKFR->MDicRank=wFldDicRank;
  wDRef.setPtr(pKFR);

  wV.setValue<ZDataReference>(wDRef);
  wRow[0]->setData(wV,ZQtDataReference);

  wKeyItem->insertRow(pIdx.row(),wRow);

  /* update and display key values */

  _recomputeKeyValues(wKeyItem);

  /* delete Temporary */

  //  delete wKFR;

  return true;  /* done */
} // DicEdit::_keyFieldInsertToKey

bool
DicEdit::_keyFieldAppendToKey(QModelIndex pIdx, ZKeyFieldRow* pKFR) {
  QVariant        wV;
  ZDataReference  wDRef ;
  QStandardItem*  wKeyItem=nullptr;

  /* append to current key :
   * from current model index : if type is ZEntity_KeyField get father
   * create row
   */
  QModelIndex wIdx = pIdx;
  if (wIdx.column()!=0) {
    wIdx=pIdx.siblingAtColumn(0);
  }

  wV=wIdx.data(ZQtDataReference);
  if (wV.isNull()) {
    Errorlog.errorLog("DicEdit::_keyFieldInsertToKey-E-VARNULL Cannot get infradata.");
    return false;
  }
  wDRef=wV.value<ZDataReference>();

  /* if target row is a Key dictionary, simply append inside this key */
  if (wDRef.getZEntity()==ZEntity_KeyDic) {
    wKeyItem=keyTRv->ItemModel->itemFromIndex(wIdx);
    return keyTRvAppendFromPinboard(wIdx);
  }
  else if (wDRef.getZEntity()!=ZEntity_KeyField) {
    Errorlog.errorLog("DicEdit::keyfieldInsertFromTemporary-E-INVTYP Invalid infradata entity type <%s> while expected <ZEntity_KeyField>.",decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }
  /* here this is a key field row */

  pKFR = wDRef.getPtr<ZKeyFieldRow*>();

  QModelIndex wKeyIdx=wIdx.parent();
  wKeyItem=keyTRv->ItemModel->item(wKeyIdx.row(),0);


  QList<QStandardItem*> wRow= createKeyFieldRow(*pKFR);

  /* append key field to key in key dictionary */

  ZIndexField wIFld(MasterDic->KeyDic[wKeyItem->row()]); /* initialize a new key field with key dictionary */

  wIFld.KeyOffset=0; /* to be recomputed */
  wIFld.Hash = pKFR->Hash;
  wIFld.MDicRank = pKFR->MDicRank;
  //  wIFld.MDicRank=MasterDic->searchFieldByName(pTemp->KFR.Name);
  //  wIFld.KeyDic = MasterDic->KeyDic[wKeyItem->row()]; // already done with ZIndexField CTOR
  /* append key field */
  long wFldDicRank=MasterDic->KeyDic[wKeyItem->row()]->push(wIFld);

  /* set row infradata */

  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyField),wFldDicRank);
  pKFR->MDicRank=wFldDicRank;
  wDRef.setPtr(pKFR);

  wV.setValue<ZDataReference>(wDRef);
  wRow[0]->setData(wV,ZQtDataReference);

  /* append row in tree view */
  wKeyItem->appendRow(wRow);
  keyTRv->resizeColumns();

  /* update and display key values */

  _recomputeKeyValues(wKeyItem);

  /* delete Temporary */

  //  delete wKFR;

  return true;  /* done */
} // DicEdit::_keyFieldAppendToKey


bool
DicEdit::_fieldInsertToKey(QModelIndex pIdx,ZFieldDescription* pField) {
  if (!pField->KeyEligible) {
    int wRet=ZExceptionDLg::message2B("DicEdit::fieldAppendToKey",ZS_INVTYPE,Severity_Error,
        "Discard","Force",
        "Field %s cannot be part of a key.\n"
        "Using such a field without conversion may induce problems."
        "Do you wish to continue anyway or discard ?",pField->getName().toCChar());
    if (wRet==QDialog::Rejected)
      return false;
  }
  ZKeyHeaderRow   wKHR(MasterDic);
  ZKeyFieldRow    wKFR;
  ZDataReference  wDRefKey;
  ZDataReference  wDRefKeyField;
  QVariant        wV;
  QList<QStandardItem *> wKeyRow, wKeyFieldRow ;

  wKFR.Name = pField->getName();
  wKFR.ZType = pField->ZType;
  wKFR.MDicRank = MasterDic->searchFieldByName(wKFR.Name);

  wKFR.UniversalSize = pField->UniversalSize;
  wKFR.Hash = pField->Hash;
  wKFR.KeyOffset = 0; /* to be recomputed */


  /* if there is nothing yet in tree view (no key defined) then
 *  - create a key with name __NEW_KEYNAME__
 *  - display this key
 *  - create a key field from given field description (done before)
 *  - append key field to created key
 */
  if (keyTRv->ItemModel->rowCount()==0) {
    return _fieldAppendToKey(pIdx,pField);
  } // if (keyTRv->ItemModel->rowCount()==0)

  /* append to current key :
   * from current model index : if type is ZEntity_KeyField get father
   * create row
   */

  QModelIndex wIdxFather , wIdx = pIdx;
  QStandardItem* wKeyItem=nullptr;

  if (wIdx.column()!=0)
    wIdx=wIdx.siblingAtColumn(0);

  /* get type */
  wV=wIdx.data(ZQtDataReference);
  wDRefKey=wV.value<ZDataReference>();

  if (wDRefKey.getZEntity()==ZEntity_KeyDic) {
    wIdxFather = wIdx;
  }
  else if (wDRefKey.getZEntity()==ZEntity_KeyField) {
    wIdxFather = wIdx.parent();
    wV=wIdxFather.data(ZQtDataReference);
    wDRefKey=wV.value<ZDataReference>();
  } else {
    Errorlog.errorLog("DicEdit::keyfieldAppend-E-INVTYP Invalid infradata entity type <%s> while expected <ZEntity_KeyField>.",decode_ZEntity(wDRefKey.getZEntity()).toChar());
    return false;
  }

  wKeyItem = keyTRv->ItemModel->itemFromIndex(wIdxFather);

  wKeyFieldRow=createKeyFieldRow(wKFR);

  wDRefKeyField.setZLayout(ZLayout_KeyTRv);
  wDRefKeyField.setResource(getNewResource(ZEntity_KeyField));
  wDRefKeyField.setDataRank(wKeyItem->rowCount());
  wDRefKeyField.setPtr(new ZKeyFieldRow(wKFR)); /* store key dictionary Data (to be deleted) */
  wV.setValue<ZDataReference>(wDRefKeyField);
  wKeyFieldRow[0]->setData(wV,ZQtDataReference);
  if (wKeyItem->rowCount()==0) {  /* key has no associated fields yet -> append */
    wKeyItem->appendRow(wKeyFieldRow);
  }
  else
    wKeyItem->insertRow(pIdx.row(),wKeyFieldRow);

  keyTRv->expandItem(wIdxFather);
//  keyTRv->resizeColumns();

  _recomputeKeyValues(wKeyItem);

  return true;
} //_fieldInsertToKey
bool
DicEdit::_fieldAppendToKey(QModelIndex pIdx,ZFieldDescription* pField) {
  if (!pField->KeyEligible) {
    int wRet=ZExceptionDLg::message2B("DicEdit::fieldAppendToKey",ZS_INVTYPE,Severity_Error,
        "Discard","Force",
        "Field %s cannot be part of a key.\n"
        "Using such a field without conversion may induce problems."
        "Do you wish to continue anyway or discard ?",pField->getName().toCChar());
    if (wRet==QDialog::Rejected)
      return false;
  }
  ZKeyHeaderRow wKHR(MasterDic);
  ZKeyFieldRow wKFR;
  ZDataReference wDRefKey , wDRefKeyField;
  QVariant wV;
  QList<QStandardItem *> wKeyRow, wKeyFieldRow ;

  wKFR.Name = pField->getName();
  wKFR.ZType = pField->ZType;
  wKFR.MDicRank = MasterDic->searchFieldByName(wKFR.Name);

  wKFR.UniversalSize = pField->UniversalSize;
  wKFR.Hash = pField->Hash;
  wKFR.KeyOffset = 0; /* to be recomputed */


  /* if there is nothing yet in tree view (no key defined) then
 *  - create a key with name __NEW_KEYNAME__
 *  - display this key
 *  - create a key field from given field description (done before)
 *  - append key field to created key
 */
  if (keyTRv->ItemModel->rowCount()==0) {

    /* create a key with name __NEW_KEYNAME__ */
    ZKeyDictionary* wKeyDic=new ZKeyDictionary(__NEW_KEYNAME__,MasterDic);
    long wKeyFieldRank=MasterDic->KeyDic.push(wKeyDic);
    wKHR.set(wKeyDic);

    wKeyRow=createKeyDicRow(wKHR);

    wDRefKey.setZLayout(ZLayout_KeyTRv);
    wDRefKey.setResource(getNewResource(ZEntity_KeyDic));
    wDRefKey.setDataRank(wKeyFieldRank);
    wDRefKey.setPtr(new ZKeyHeaderRow(wKHR)); /* store key dictionary Data (to be deleted) */
    wV.setValue<ZDataReference>(wDRefKey);

    wKeyRow[0]->setData(wV,ZQtDataReference);

    keyTRv->ItemModel->appendRow(wKeyRow); /* append this key to treeview - level 0 */


    wKeyFieldRow=createKeyFieldRow(wKFR);
  } // if (keyTRv->ItemModel->rowCount()==0)

    /* if there is nothing yet in tree view (no key defined) then
 *  - create a key with name __NEW_KEYNAME__
 *  - display this key
 *  - create a key field from given field description (done before)
 *  - append key field to created key
 */
    ZIndexField wKField = wKFR.get();
    long wKFRank=MasterDic->KeyDic.last()->push( wKField);


    wDRefKeyField.setZLayout(ZLayout_KeyTRv);
    wDRefKeyField.setResource(getNewResource(ZEntity_KeyField));
    wDRefKeyField.setDataRank(wKFRank);
    wDRefKeyField.setPtr(new ZKeyFieldRow(wKFR)); /* store key dictionary Data (to be deleted) */
    wV.setValue<ZDataReference>(wDRefKey);
    wKeyFieldRow[0]->setData(wV,ZQtDataReference);

    wKeyRow[0]->appendRow(wKeyFieldRow); /* append key field to created key */

    keyTRv->resizeColumns();
    _recomputeKeyValues(wKeyRow[0]);

    return true;
} //_fieldAppendToKey

bool
DicEdit::_fieldInsertToKey1(QModelIndex pIdx, ZFieldDescription* pField) {
  QVariant        wV;
  ZDataReference  wDRef ;
  QStandardItem*  wKeyItem=nullptr;
  ZKeyFieldRow*    wKFR=nullptr;
  /* append to current key :
   * from current model index : if type is ZEntity_KeyField get father
   * create row
   */
  QModelIndex wIdx = pIdx;
  if (wIdx.column()!=0) {
    wIdx=pIdx.siblingAtColumn(0);
  }

  wV=wIdx.data(ZQtDataReference);
  if (wV.isNull()) {
    Errorlog.errorLog("DicEdit::_fieldInsertToKey-E-VARNULL Cannot get infradata.");
    return false;
  }
  wDRef=wV.value<ZDataReference>();



  /* if target row is a Key dictionary, simply append inside this key */
  if (wDRef.getZEntity()==ZEntity_KeyDic) {
    wKeyItem=keyTRv->ItemModel->itemFromIndex(wIdx);
    return keyTRvAppendFromPinboard(wIdx);
  }
  else if (wDRef.getZEntity()!=ZEntity_KeyField) {
    Errorlog.errorLog("DicEdit::keyfieldInsertFromTemporary-E-INVTYP Invalid infradata entity type <%s> while expected <ZEntity_KeyField>.",decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }
  /* here this is a key field row */

  wKFR = wDRef.getPtr<ZKeyFieldRow*>();

  QModelIndex wKeyIdx=wIdx.parent();
  wKeyItem=keyTRv->ItemModel->item(wKeyIdx.row(),0);

  /* insert row in tree view */
  QList<QStandardItem*> wRow= createKeyFieldRow(*wKFR);

  /* insert key field in key dictionary */
  ZIndexField wIFld(MasterDic->KeyDic[wKeyItem->row()]);

  wIFld.KeyOffset=0; /* to be recomputed */
  wIFld.Hash = wKFR->Hash;
  wIFld.MDicRank = wKFR->MDicRank;
  //  wIFld.MDicRank=MasterDic->searchFieldByName(pTemp->KFR.Name);
  //  wIFld.KeyDic = MasterDic->KeyDic[wKeyItem->row()]; // already done with ZIndexField CTOR

  long wFldDicRank=MasterDic->KeyDic[wKeyItem->row()]->insert(wIFld,wIdx.row());

  /* update row infradata */

  wDRef.set(ZLayout_KeyTRv,getNewResource(ZEntity_KeyField),wFldDicRank);
  wKFR->MDicRank=wFldDicRank;
  wDRef.setPtr(wKFR);

  wV.setValue<ZDataReference>(wDRef);
  wRow[0]->setData(wV,ZQtDataReference);

  wKeyItem->insertRow(pIdx.row(),wRow);

  /* update and display key values */

  _recomputeKeyValues(wKeyItem);

  /* delete Temporary */

  //  delete wKFR;

  return true;  /* done */
} // DicEdit::_fieldInsertToKey

bool
DicEdit::keyTRvAppendFromPinboard(const QModelIndex &pIdx) {
  if (Pinboard.isEmpty())
    return false;
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("nothing selected."),cst_MessageDuration);
    return false;
  }
  ZKeyFieldRow* wKFR=nullptr;
  ZDataReference wDRef ;
  QVariant wV;
  QStandardItem* wKeyItem=nullptr;

  wDRef=Pinboard.last().DRef;

  switch (Pinboard.last().getZEntity()) {
  case ZEntity_KeyField: {
    ZKeyFieldRow* wKFR=Pinboard.last().DRef.getPtr<ZKeyFieldRow*>();
    return _keyFieldAppendToKey(pIdx,wKFR);
  }
  case ZEntity_KeyDic: { /* a whole key must be appended */
    ZKeyHeaderRow* wKHR = Pinboard.last().DRef.getPtr<ZKeyHeaderRow*>();
    return _keyAppend(wKHR);
  }
  case ZEntity_DicField:{ /* a dictionary field must be converted to key field and inserted here */
    ZFieldDescription* wField = Pinboard.last().DRef.getPtr<ZFieldDescription*>();
    return _fieldAppendToKey(pIdx,wField);
  }
  default:
    Errorlog.errorLog("DicEdit::keyTRvAppendFromPinboard-E-INVTYP Invalid infradata entity type <%s>.",decode_ZEntity(wDRef.getZEntity()).toChar());
    return false;
  }//switch


}//keyTRvAppendFromPinboard

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

  QVariant wV = wKeyIdx.data(ZQtDataReference);
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
  wStr.sprintf("total size: %ld",wKDPack.KeyUniversalSize);
  wItem->setText(wStr.toCChar());

  return true;
}//updateKeyValues


void
DicEdit::KeyTRvKeyFiltered(int pKey,QKeyEvent* pEvent)
{

  if(pKey == Qt::Key_Escape) {
    if( QMessageBox::question (this,tr("You pressed <Esc> key"), tr("Do you really want to quit without saving anything ?"),QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
    {
      QApplication::quit();
      return;
    }
  }

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
}//KeyTReKeyFiltered

void
DicEdit::KeyTRvMouseFiltered(int pKey,QMouseEvent* pEvent)
{

}

bool DicEdit::_fieldDelete(QModelIndex pIdx, bool pForceDelete)
{
  utf8VaryingString wStr;
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }
  if (pIdx.column()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }
//  QStandardItem* wItem = fieldTBv->ItemModel->itemFromIndex(pIdx);
//  QStandardItem* wFather = wItem->parent();  /* for a table view this is not really necessary */

  QVariant wV;
  wV = pIdx.data(ZQtDataReference);
  if (!wV.isValid()){
    Errorlog.errorLog("DicEdit::_fieldDelete-E-INVENT Cannot get row infra data <ZQtDataReference>.\n");
    return false;
  }

  ZDataReference wDRef=wV.value<ZDataReference>();
  ZFieldDescription* wF=wDRef.getPtr<ZFieldDescription*>();

  long wR=-1;
  int wKRank=0;
  int wKNb=0;
  /* look for all keys that involve this field */
  for (long wi=0;wi < MasterDic->KeyDic.count();wi++) {
    if ((wR=MasterDic->KeyDic.Tab[wi]->hasFieldNameCase(wF->getName()))>=0) {
      wKRank=wi;
      wKNb++;
    }
  }
  if (wKNb>0) {
  if (pForceDelete) {
      Errorlog.errorLog("field named <%s> is used by %ld key(s). Last key name is <%s>. Cannot be deleted."
          ,wKNb,MasterDic->KeyDic[wKRank]->DicKeyName.toCChar());
      return false;
    }
    else
      Errorlog.warningLog("field named <%s> is used by %ld key(s). Last key name is <%s>."
          ,wKNb,MasterDic->KeyDic[wKRank]->DicKeyName.toCChar());
  }//if (wKNb>0)


//  wFather->removeRow(pIdx.row());
  fieldTBv->ItemModel->removeRow(pIdx.row());

  long wRank=MasterDic->searchFieldByName(wF->getName());

  MasterDic->erase(wRank);

  wStr.sprintf("Field <%s> has been deleted.",wF->getName().toCChar());
  delete wF;

  statusBar()->showMessage(QObject::tr(wStr.toCChar()),cst_MessageDuration);
  return true;
}

bool DicEdit::fieldChange(QModelIndex pIdx)
{
  utf8String wStr;
  if (!pIdx.isValid())
    return false;
  if (pIdx.column()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }
  int wItemRow=pIdx.row();
  QVariant wV=fieldTBv->ItemModel->item(wItemRow,0)->data(ZQtDataReference); /* we can do that because simple table view */
  if (!wV.isValid())
    return false;
  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_DicField)
  {
    fprintf(stderr,"DicEdit::fieldChange-E-INVENT Invalid entity : expected ZEntity_DicField.");
    return false;
  }
  if (FieldDLg==nullptr) {
    FieldDLg=new ZFieldDLg(this);
  }
  ZFieldDescription* wFDescPtr= wDRef.getPtr<ZFieldDescription*>();
  ZFieldDescription wFDesc(*wFDescPtr);
  bool wB=false;
  while (!wB) {
    FieldDLg->setup(wFDesc);
    int wRet=FieldDLg->exec();
    if (wRet==QDialog::Rejected)
      return false;
    wB=controlField(wFDesc);
  }

/* displayFieldChange manages the changes in table view and in dictionary */

  if (!displayFieldChange(wFDesc,pIdx)) {
    Errorlog.errorLog("unable to change field <%s>. Something went wrong.",
        MasterDic->Tab[wDRef.DataRank].getName().toCChar());
    return false;
  }
  wStr.sprintf("field <%s> changed",MasterDic->Tab[wDRef.DataRank].getName().toCChar());
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

  wV=pItem->data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();

  if (wDRef.DataRank >= pDataRank)
  {
    wDRef.DataRank++;
    wV.setValue<ZDataReference>(wDRef);
    pItem->setData(wV,ZQtDataReference);
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


bool DicEdit::fieldAppend() {
  QVariant wV;
  ZDataReference wDRef;

  if (FieldDLg==nullptr)
    FieldDLg=new ZFieldDLg(this);

  FieldDLg->setCreate();
  int wRet=FieldDLg->exec();
  if (wRet==QDialog::Rejected)
    return false;
  ZFieldDescription wFDesc=FieldDLg->getFieldDescription();
  return _fieldAppend(wFDesc);

}//fieldAppend

bool DicEdit::fieldInsertNewBefore(QModelIndex pIdx) {
  QVariant wV;
  ZDataReference wDRef;

  if (FieldDLg==nullptr)
    FieldDLg=new ZFieldDLg(this);

  FieldDLg->setCreate();
  int wRet=FieldDLg->exec();
  if (wRet==QDialog::Rejected)
      return false;
  ZFieldDescription wFDesc=FieldDLg->getFieldDescription();
  return _fieldInsertBefore(pIdx,wFDesc);

}//fieldInsertNewBefore


bool DicEdit::fieldInsertNewAfter(QModelIndex pIdx) {
  QVariant wV;
  ZDataReference wDRef;

  if (FieldDLg==nullptr)
    FieldDLg=new ZFieldDLg(this);

  FieldDLg->setCreate();
  int wRet=FieldDLg->exec();
  if (wRet==QDialog::Rejected)
    return false;
  ZFieldDescription wFDesc=FieldDLg->getFieldDescription();
  return _fieldInsertAfter(pIdx,wFDesc);

}//fieldInsertNewAfter

bool DicEdit::_fieldAppend(ZFieldDescription &wFDesc) {
  QVariant wV;
  ZDataReference wDRef;
  QList<QStandardItem*> wRow = createFieldRowFromField(&wFDesc);

  long wRIdx=0;
  wRIdx=MasterDic->push(wFDesc);

  wDRef.set(ZLayout_FieldTBv,getNewResource(ZEntity_DicField),wRIdx);
  wDRef.setPtr(new ZFieldDescription(MasterDic->Tab[wRIdx]));
  wDRef.setDataRank(wRIdx);
  wV.setValue<ZDataReference>(wDRef);

  wRow[0]->setData(wV,ZQtDataReference);

  fieldTBv->ItemModel->appendRow(wRow);

  fieldTBv->resizeRowsToContents();
  fieldTBv->resizeColumnsToContents();
  return true;
}//_fieldAppend


bool DicEdit::fieldCopyToPinboard(QModelIndex pIdx)
{
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }
  if (pIdx.column()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }
  ZDataReference wDRef ;

  /* get data to access dictionary */
  QVariant wV=pIdx.data(ZQtDataReference);
  wDRef = wV.value<ZDataReference>();
//  wDRef.setDataRank(pIdx.row());        /* store row as DataRank */
//  wDRef.setDataRankInvalid();

  wDRef.setIndex ( pIdx); /* store row  */

  ZPinboardElement wPBElt;
  wPBElt.DRef = wDRef;

  ZFieldDescription* wFD=wDRef.getPtr<ZFieldDescription*>();
  wPBElt.DRef.setPtr(new ZFieldDescription(*wFD));

  Pinboard.push(wPBElt);
  return true;
}//_fieldCopyToPinboard

bool DicEdit::fieldInsertBeforeFromPinboard(QModelIndex pIdx)
{
  if (Pinboard.isEmpty()) {
    ui->statusBar->showMessage(QObject::tr("Nothing in pinboard.","DicEdit"),cst_MessageDuration);
    return false;
  }
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }

  if (Pinboard.getLast()->DRef.getZEntity()!=ZEntity_DicField ) {
    Errorlog.errorLog("DicEdit::fieldInsertFromPinboard-E-INVENT Invalid entity from pinboard <%s> while expecting ZEntity_DicField.",
                      decode_ZEntity (Pinboard.getLast()->DRef.getZEntity()).toChar());
    return false;
  }
  ZFieldDescription* wFD= Pinboard.getLast()->DRef.getPtr<ZFieldDescription*>(); /* will be linked to created field row*/

  bool wRet=_fieldInsertBefore(pIdx, *wFD);

  delete wFD;
  Pinboard.pop();

  fieldTBv->resizeColumnsToContents();
  fieldTBv->resizeRowsToContents();

  return wRet;
}//fieldInsertBeforeFromPinboard

bool DicEdit::fieldInsertAfterFromPinboard(QModelIndex pIdx)
{
  if (Pinboard.isEmpty()) {
    ui->statusBar->showMessage(QObject::tr("Nothing in pinboard.","DicEdit"),cst_MessageDuration);
    return false;
  }
  if (!pIdx.isValid())
  {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }

  if (Pinboard.getLast()->DRef.getZEntity()!=ZEntity_DicField ) {
    Errorlog.errorLog("DicEdit::fieldInsertFromPinboard-E-INVENT Invalid entity from pinboard <%s> while expecting ZEntity_DicField.",
        decode_ZEntity (Pinboard.getLast()->DRef.getZEntity()).toChar());
    return false;
  }
  ZFieldDescription* wFD= Pinboard.getLast()->DRef.getPtr<ZFieldDescription*>(); /* will be linked to created field row*/

  bool wRet=_fieldInsertAfter(pIdx, *wFD);

  delete wFD;
  Pinboard.pop();

  fieldTBv->resizeColumnsToContents();
  fieldTBv->resizeRowsToContents();

  return wRet;
}//fieldInsertAfterFromPinboard

bool DicEdit::_fieldInsertAfter(QModelIndex pIdx, ZFieldDescription &pFDesc)
{
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }

  QVariant wV;
  ZDataReference wDRef (ZLayout_FieldTBv,getNewResource(ZEntity_DicField));
  QList<QStandardItem*> wRow ;

  if (pIdx.column()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }
//  QStandardItem* wItem = fieldTBv->ItemModel->itemFromIndex(pIdx);
//  QStandardItem* wFather = wItem->parent();  /* for a table view this is not really necessary */

  int wRank= pIdx.row() + 1;

  long wFieldRank;

  wRow = createFieldRowFromField(&pFDesc);

  if (fieldTBv->ItemModel->rowCount() < wRank) {   /* if last row : append */
    fieldTBv->ItemModel->appendRow(wRow);
    wFieldRank=MasterDic->push(pFDesc);
  }
  else {

    fieldTBv->ItemModel->insertRow(wRank,wRow);
    wFieldRank=MasterDic->insert(pFDesc,wRank);
  }
  wDRef.setDataRank(wFieldRank);
  wDRef.setPtr(new ZFieldDescription(pFDesc));
  wV.setValue<ZDataReference>(wDRef);
  wRow[0]->setData(wV,ZQtDataReference);

  return true;
}//_fieldInsertAfter

bool DicEdit::_fieldInsertBefore(QModelIndex pIdx, ZFieldDescription &pFDesc)
{
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage(QObject::tr("No valid field row selected.","DicEdit"),cst_MessageDuration);
    return false;
  }

  QVariant wV;
  ZDataReference wDRef (ZLayout_FieldTBv,getNewResource(ZEntity_DicField));
  QList<QStandardItem*> wRow ;

  if (pIdx.column()!=0) {
    pIdx=pIdx.siblingAtColumn(0);
  }

  int wRank= pIdx.row();

  long wFieldRank = MasterDic->insert(pFDesc,wRank);

  wRow = createFieldRowFromField(&pFDesc);
  wDRef.setDataRank(wFieldRank);
  wDRef.setPtr(new ZFieldDescription(pFDesc));
  wV.setValue<ZDataReference>(wDRef);
  wRow[0]->setData(wV,ZQtDataReference);

  fieldTBv->ItemModel->insertRow(wRank,wRow);

  return true;
}//_fieldInsertAfter

bool DicEdit::fieldAppendFromPinboard()
{
  if (Pinboard.isEmpty())
    return false;

  if (Pinboard.getLast()->DRef.getZEntity()!=ZEntity_DicField ) {
    Errorlog.errorLog("DicEdit::fieldInsertFromPinboard-E-INVENT Invalid entity from pinboard <%s> while expecting ZEntity_DicField.",
        decode_ZEntity (Pinboard.getLast()->DRef.getZEntity()).toChar());
    return false;
  }
  QVariant wV;
  ZDataReference wDRef= Pinboard.getLast()->DRef;
  QList<QStandardItem*> wRow ;

  ZFieldDescription* wFD= Pinboard.getLast()->DRef.getPtr<ZFieldDescription*>();

  bool wRet = _fieldAppend(*wFD);

  delete wFD;
  Pinboard.pop() ;

  fieldTBv->resizeColumnsToContents();
  fieldTBv->resizeRowsToContents();

  return wRet;
}//_fieldAppendFromPinboard

bool DicEdit::fieldCutToPinboard(const QModelIndex &pIdx)
{
  if (!fieldCopyToPinboard(pIdx))
      return false;

  return _fieldDelete(pIdx);
}


bool DicEdit::displayFieldChange(ZFieldDescription& pField,QModelIndex pIdx)
{
  if (!pIdx.isValid()) {
    ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
    return false;
  }
  if (pIdx.column()!=0) {
    pIdx.siblingAtColumn(0);
  }

  utf8VaryingString wStr;
  QList<QStandardItem*> wRow = itemRow(fieldTBv->ItemModel,pIdx);

  QVariant wV=wRow[0]->data(ZQtDataReference);
  if (!wV.isValid()) {
    fprintf(stderr,"DicEdit::displayFieldChange-E-INVDAT Invalid data .\n");
    return false;
  }
  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_DicField) {
    fprintf(stderr,"DicEdit::displayFieldChange-E-INVENT Invalid entity : expected ZEntity_DicField.\n");
    return false;
  }
  ZFieldDescription* wField= wDRef.getPtr<ZFieldDescription*>();

  /*search dictionary for former field name (could be changed) */

  long wRank= MasterDic->searchFieldByName(wField->getName());
  /* update data (possibly field name) */
  wField->_copyFrom(pField);
  /* update dictionary */
  MasterDic->Tab[wRank]._copyFrom(pField);
  /* display change */
  return setFieldRowFromField(wRow,pField);
}//displayFieldChange

/* replaces the values (displayed value and dictionary value) of a field at model index wIdx */
void DicEdit::acceptFieldChange(ZFieldDescription& pField,QModelIndex &wIdx)
{
  utf8String wStr;
//  QModelIndex wIdx=fieldTBv->currentIndex();
  if (!wIdx.isValid()) {
    ui->statusBar->showMessage("No current row selected.",cst_MessageDuration);
    return;
  }

  QStandardItem* wItem=fieldTBv->ItemModel->item(wIdx.row(),0);
  QVariant wV=wItem->data(ZQtDataReference);
  if (!wV.isValid())
    return;
  ZDataReference wDRef=wV.value<ZDataReference>();
  if (wDRef.getZEntity()!=ZEntity_DicField)
  {
    fprintf(stderr,"DicEdit::acceptFieldChange-E-INVENT target item has invalid entity : expected ZEntity_DicField.");
    return;
  }

  ZFieldDescription* wField= wDRef.getPtr<ZFieldDescription*>();
  wField->_copyFrom(pField);

/* displayFieldChange manages the changes in table view and in dictionary */
  if (!displayFieldChange(pField,wIdx))
    return;

  wDRef.setPtr(&MasterDic->Tab[wDRef.DataRank]);  /* store pointer to dictionary field */
  wV.setValue<ZDataReference>(wDRef);
  wItem->setData(wV,ZQtDataReference);

} //acceptFieldChange


void
DicEdit::FieldTBvKeyFiltered(int pKey, QKeyEvent *pEvent)
{

  if(pKey == Qt::Key_Escape)
  {
    this->hide();
    return;
  }

  if(pKey == Qt::Key_Insert)
  {
    ZDataReference WR;
    WR.ZLayout = ZLayout_FieldTBv;
    WR.ResourceReference = getNewResource(ZEntity_DicField);
    WR.DataRank=-1;
    QModelIndex wIdx = fieldTBv->currentIndex();
    if (wIdx.isValid())
      fieldInsertNewBefore(wIdx);
//    createEvent(WR);
    return;
  }
  if(pKey == Qt::Key_Delete)
  {
//    deleteGeneric();
    return;
  }

  return;

}//KeyTReKeyFiltered

void
DicEdit::FieldTBvMouseFiltered(int pKey, QMouseEvent *pEvent)
{
  switch (pKey)
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
}



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

  pFieldTBv->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );

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
  std::function<void(int,QKeyEvent *)>                        wKeyFilterCBF;
  std::function<void(QContextMenuEvent *)>                    wContextMenuCBF;
  std::function<bool (int,ZDataReference *,ZDataReference *)> wDropEventCBF=nullptr;
  std::function<void ()>                                      wRawStartDrag;
  std::function<bool (QDropEvent *)>                          wRawDropEventCBF;
/*
  wKeyFilterCBF     = std::bind(&DicEdit::FieldTBvKeyFiltered, this,_1,_2);
  wContextMenuCBF   = std::bind(&DicEdit::FieldTBvFlexMenu, this,_1);     // for context menu callback for ZQTreeView
//  wDropEventCBF     = std::bind(&DicEdit::FieldTBvDropEvent, this,_1,_2,_3);     // for drag and drop
  wRawStartDrag     = std::bind(&DicEdit::FieldTBvRawStartDrag, this);
  wRawDropEventCBF  = std::bind(&DicEdit::FieldTBvRawDropEvent, this,_1);
*/
  pFieldTBv->_registerCallBacks(std::bind(&DicEdit::FieldTBvKeyFiltered, this,_1,_2),
                      std::bind(&DicEdit::FieldTBvMouseFiltered, this,_1,_2),
                      std::bind(&DicEdit::FieldTBvFlexMenu, this,_1),
                      nullptr);

  pFieldTBv->setDestructorCallback(std::bind(&DicEdit::DestructorCallback, this));

  pFieldTBv->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  pFieldTBv->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  pFieldTBv->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  pFieldTBv->setSupportedDropActions(Qt::CopyAction|Qt::MoveAction);

  pFieldTBv->setRawStartDragCallback(std::bind(&DicEdit::FieldTBvRawStartDrag, this));
  pFieldTBv->setRawDropEventCallback(std::bind(&DicEdit::FieldTBvRawDropEvent, this,_1));

  return pFieldTBv;
}//setupFieldTBv

void DicEdit::DestructorCallback() {
  int wFieldRows=fieldTBv->ItemModel->rowCount();
  QStandardItem* wItem;
  QVariant wV;
  for (int wi=0;wi< wFieldRows;wi++) {
    wItem=fieldTBv->ItemModel->item(wi,0);
    if (wItem==nullptr)
      continue;
    wV = wItem->data(ZQtDataReference);
    if (!wV.isValid())
      continue;
    ZFieldDescription* wFD =(ZFieldDescription*) wV.value<void*>();
    delete wFD;
  }
} // DestructorCallback

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

  pKeyTRv->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );

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
  std::function<void(int,QKeyEvent*)>                         wKeyFilterCBF;
  std::function<void(QContextMenuEvent *)>                    wContextMenuCBF;
  std::function<bool (int,ZDataReference *,ZDataReference *)> wDropEventCBF=nullptr;
  std::function<void ()>                                      wRawStartDragCBF;
  std::function<bool (QDropEvent *)>                          wRawDropEventCBF;

  wKeyFilterCBF     = std::bind(&DicEdit::KeyTRvKeyFiltered, this,_1,_2);
  wContextMenuCBF   = std::bind(&DicEdit::KeyTRvFlexMenu, this,_1);         // for context menu callback for ZQTreeView

  wRawStartDragCBF  = std::bind(&DicEdit::KeyTRvRawStartDrag, this);     // for raw drag and drop
  wRawDropEventCBF  = std::bind(&DicEdit::KeyTRvRawDropEvent, this,_1);  // for raw drag and drop

  pKeyTRv->_register( std::bind(&DicEdit::KeyTRvKeyFiltered, this,_1,_2),
                      nullptr,
                      std::bind(&DicEdit::KeyTRvFlexMenu, this,_1),
                      nullptr);

  pKeyTRv->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  pKeyTRv->setEditTriggers(QAbstractItemView::NoEditTriggers);  /* it is not editable */
  pKeyTRv->setSelectionBehavior(QAbstractItemView::SelectRows); /* select line per line */
  pKeyTRv->setSelectionMode(QAbstractItemView::SingleSelection); /* only one line */

  pKeyTRv->setRawStartDragCallback(std::bind(&DicEdit::KeyTRvRawStartDrag, this));
  pKeyTRv->setRawDropEventCallback(std::bind(&DicEdit::KeyTRvRawDropEvent,this,_1));

  pKeyTRv->setSupportedDropActions(Qt::CopyAction|Qt::MoveAction);

  return pKeyTRv;
  //    PerDomainTRe->expandAll();
}//setupKeyTRv



void DicEdit::KeyTRvRawStartDrag()
{
  QModelIndex wIdx=fieldTBv->currentIndex();
  if (!wIdx.isValid())
    return;
  keyCopyToPinboard(wIdx);
  return;

  if(wIdx.column()!=0)
    wIdx=wIdx.siblingAtColumn(0);

  QVariant wV=wIdx.data(ZQtDataReference);
  if (!wV.isValid()) {
    ui->statusBar->showMessage(QObject::tr("Invalid data for current row","DicEdit"),cst_MessageDuration);
    return;
  }
  ZDataReference wDRef=wV.value<ZDataReference>();
  switch (wDRef.getZEntity()) {
  case ZEntity_KeyDic:
    keyCopyToPinboard(wIdx);
    return;
  case ZEntity_KeyField:
    keyFieldCopyToPinboard(wIdx);
    return;

  }

  if (wDRef.getZEntity()!=ZEntity_KeyField) { /* only key field (not key dic) is been allowed to be dragged */
    ui->statusBar->showMessage(QObject::tr("Only key field (not dictionary key) is allowed to be dragged","DicEdit"),cst_MessageDuration);
    return;
  }

  ZPinboardElement wPBE;
  wPBE.setDataReference(wDRef);

//  wPBE.setData(&wFDPack,sizeof(KeyField_Pack));
  Pinboard.push(wPBE,true);
  return;
}

bool DicEdit::KeyTRvRawDropEvent(QDropEvent *pEvent)
{
  ZPinboard* wPinboard=&Pinboard; /* debug */

  if (Pinboard.isEmpty()) {
      statusBar()->showMessage(QObject::tr("No drag source element selected","DicEdit"),cst_MessageDuration);
      return false;
  }

  QModelIndex wIdx;
  if (keyTRv->ItemModel->rowCount()==0) {
    /* no key yet defined : define one */

    if (!keyAppend())
      return false; /* if not defined or error : return */

    wIdx = keyTRv->ItemModel->index(0,0); /* set index to newly created key */
  }
  else {
    wIdx = keyTRv->indexAt (pEvent->pos());
  }
  if (!wIdx.isValid()) {
    statusBar()->showMessage(QObject::tr("No target element selected","DicEdit"),cst_MessageDuration);
    return false;
  }
  return keyTRvInsertFromPinboard(wIdx);

  /* get the origin of the dragndrop operation */
  switch (Pinboard.getLast()->DRef.getZEntity()) {
  case ZEntity_DicField: {
//    ZFieldDescription * wFD= Pinboard.getLast()->DRef.getPtr<ZFieldDescription*>();
    return keyTRvInsertFromPinboard(wIdx);
  }
  case ZEntity_KeyDic: {
    ZKeyDictionary *wKey= Pinboard.getLast()->DRef.getPtr<ZKeyDictionary*>();
    _keyInsert(wIdx,wKey);
    Pinboard.pop();
    return true;
  }
  case ZEntity_KeyField:
    keyTRvInsertFromPinboard(wIdx);
    return true;
  default:
    Errorlog.errorLog("DicEdit::_keyCopy-E-INVTYP Invalid infradata entity type <%s> ",
        decode_ZEntity(Pinboard.getLast()->DRef.getZEntity()).toChar());
    return false;
  }


  ui->statusBar->showMessage("Move for key field is not yet implemented.",cst_MessageDuration);
  return false;
}// KeyTRvRawDropEvent

void DicEdit::FieldTBvRawStartDrag()
{
  if (fieldCopyToPinboard(fieldTBv->currentIndex()))
    OriginIndex=fieldTBv->currentIndex();

  return;
}

bool DicEdit::FieldTBvRawDropEvent(QDropEvent *pEvent)
{
  const QMimeData *wMimeData = pEvent->mimeData();

  if (wMimeData->hasUrls()) {
    if (wMimeData->urls().isEmpty()){
      pEvent->ignore(); // error no url while one was expected : meaning external to Qt application
      return false;
    }
    uriString wURI ;
    wURI = wMimeData->urls()[0].toString();
    wURI = wURI.getLocal();
    /* eliminate <file://>   */

    if (rawFields==nullptr) {
      rawFields = new RawFields( wURI,
          "/home/gerard/Development/zbasetools/zcontent/ztest_zindexedfile/testdata/zcppparserparameters.xml", this);
    }

    rawFields->showAll();
    rawFields->parse();
    return true;
  }//if (wMimeData->hasUrls())

  QList<QString>FormatList = wMimeData->formats();

  QVariant wV;
  ZDataReference wDRef;

  QModelIndex wIdx = fieldTBv->indexAt (pEvent->pos());

  if (Pinboard.isEmpty()) {
      ui->statusBar->showMessage(QObject::tr("Nothing in pinboard.","DicEdit"),cst_MessageDuration);
      return false;
  }

  wDRef=Pinboard.last().DRef;

//  QList<QStandardItem*> wRowToDelete= fieldTBv->ItemModel->takeRow( wDRef.getIndex().row());

  bool wRet= fieldInsertBeforeFromPinboard(wIdx);
  if (wRet) {
    /* if successful : delete associated data */
    if (OriginIndex.isValid()) {
      wV=OriginIndex.data(ZQtDataReference);
      if (wV.isValid()) {
        wDRef=wV.value<ZDataReference>();
        ZFieldDescription* wFD=wDRef.getPtr<ZFieldDescription*>();
        delete wFD;
      }
      /* then delete displayed row */
      fieldTBv->ItemModel->removeRow( OriginIndex.row()); /* origin row is set to ZDataReference::Index in startdrag routine */
    }
  }

  return wRet;
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

  displayZMFDictionary(MasterDic);
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

  ZKeyHeaderRow*  wKHR=nullptr;
  ZKeyFieldRow*   wKFR=nullptr;

  ZDataReference wDRef, wDRefKh, wDRefKf;

  int wFieldRows=fieldTBv->ItemModel->rowCount();
  QStandardItem* wItem;
  QVariant wV;
  for (int wi=0;wi< wFieldRows;wi++) {
    wItem=fieldTBv->ItemModel->item(wi,0);
    wV = wItem->data(ZQtDataReference);
    wDRef=wV.value<ZDataReference>();

    ZFieldDescription* wFD =wDRef.getPtr<ZFieldDescription*>();
    wMetaDic->push(*wFD);
    } // for

  int wKeyRows=keyTRv->ItemModel->rowCount();
  QStandardItem* wKeyItem;
  for (int wi=0;wi < wKeyRows;wi++) {
    wKeyItem=keyTRv->ItemModel->item(wi,0);
    wV = wKeyItem->data(ZQtDataReference);
    wDRefKh=wV.value<ZDataReference>();
    wKHR = wDRefKh.getPtr<ZKeyHeaderRow*>();

    wKeyDic = new ZKeyDictionary(wKHR->get());
/*
    wKeyDic->DicKeyName = wKHR->DicKeyName;
    wKeyDic->Duplicates = wKHR->Duplicates;
    wKeyDic->ToolTip = wKHR->ToolTip;
*/
    int wKeyFieldCount = wKeyItem->rowCount() ;
    /* key fields */
    for (int wj=0;wj < wKeyItem->rowCount();wj++)
      {
      wItem=wKeyItem->child(wj,0);
      wV = wItem->data(ZQtDataReference);
      wDRefKf=wV.value<ZDataReference>();
      wKFR = wDRefKf.getPtr<ZKeyFieldRow*>();

      ZIndexField wIxFld = wKFR->get();
      wIxFld.MDicRank=-1;
      long wF=0;
      for (;wF< wMetaDic->count();wF++)
        if (wKFR->Name == wMetaDic->Tab[wF].getName()){
            wIxFld.MDicRank=wF;
            break;
        }
      if (wIxFld.MDicRank<0)
        {
        fprintf(stderr,"DicEdit::screenToDic-E-KFNTFND Key field <%s> not found in master dictionary (field name not found). Key field discarded.\n",
            wKFR->Name.toCChar());
        continue;
        }

        wIxFld.KeyOffset = wKFR->KeyOffset;
      for (int wh=0;wh<cst_md5;wh++)
        wIxFld.Hash.content[wh] = wKFR->Hash.content[wh];

      wKeyDic->push(wIxFld);
      }//for (int wj=0;wj < wKeyItem->rowCount();wj++)

    wMetaDic->KeyDic.push(wKeyDic);  /* then push the whole Key dictionary */
    }//for (int wi=0;wi< wKeyRows;wi++)


  if (MasterDic==nullptr) {
    ZExceptionDLg::message("DicEdit::screenToDic",ZS_NULLPTR,Severity_Error,
        "No dictionary defined.");
    return nullptr;
  }

  wMetaDic->DicName = MasterDic->DicName;
  return wMetaDic;
}//screenToDic

ZStatus
DicEdit::loadDictionaryFile(const uriString& pDicPath,long pRank){

  ZDataBuffer wRecord;
  if (DictionaryFile==nullptr) {
    DictionaryFile=new ZDictionaryFile;
  }

  if (DictionaryFile->isOpen())
    DictionaryFile->zclose();

  ZStatus wSt=DictionaryFile->zopen(pDicPath,ZMode_Edit);

  ZDicHeaderList wDicHeaderList;
  wDicHeaderList=DictionaryFile->getAllDicHeaders();

  ZStdListHeaders wHeaders;
  ZStdListDLgContent wContent;

  wHeaders.add ("Dictionary name");
  wHeaders.add ("Active");
  wHeaders.add ("Version");
  wHeaders.add ("Keys");
  wHeaders.add ("Created");
  wHeaders.add ("Modified");

  for (long wi=0;wi < wDicHeaderList.count();wi++) {
    ZStdListDLgLine wContentLine;
    utf8VaryingString wStr;

    wContentLine.add (wDicHeaderList[wi]->DicName) ;
    wContentLine.add (wDicHeaderList[wi]->Active?"Active":"") ;
    wContentLine.add (getVersionStr( wDicHeaderList[wi]->Version)) ;
    wStr.sprintf("%ld",wDicHeaderList[wi]->DicKeyCount) ;
    wContentLine.add (wStr) ;
    wContentLine.add (wDicHeaderList[wi]->CreationDate.toDateTimeFormatted().toCChar()) ;
    wContentLine.add (wDicHeaderList[wi]->ModificationDate.toDateTimeFormatted().toCChar()) ;
    wContent.push(wContentLine);
  }

  ZStdListDLg wListDlg(this);

  wListDlg.setWindowTitle("Available dictionaries");

  wListDlg.dataSetup(wHeaders,wContent);

  int wRet=wListDlg.exec();

  if (wRet==QDialog::Rejected)
    return ZS_REJECTED;

  long wRank = wListDlg.get().row();

  return DictionaryFile->loadDictionaryByRank(wRank);
}

bool
DicEdit::saveOrCreateDictionaryFile(){

  QFileDialog wFD(this,"Dictionary file ","",__DICTIONARY_EXTENSION__);

  int wRet=wFD.exec();

  if (wRet==QDialog::Rejected)
    return false;
  uriString wSelected = wFD.selectedFiles()[0].toUtf8().data();

  if (wSelected.exists()) {
    DictionaryFile->saveDictionary();
    if (saveDictionaryFile(wSelected)==ZS_SUCCESS)
      return true;
    return false;
  }

  return true;
}

bool
DicEdit::loadDictionaryFile(){
  QFileDialog wFD(this,"Dictionary file","",__DICTIONARY_EXTENSION__);

  while (true) {
    int wRet=wFD.exec();

    if (wRet==QDialog::Rejected)
      return false;
    uriString wSelected = wFD.selectedFiles()[0].toUtf8().data();

    if (wSelected.exists()){
      break;
    }
    if (ZExceptionDLg::adhocMessage2B("Dictionary file",Severity_Error,"Abort","Retry",nullptr,"File %s does not exist ",wSelected.toCChar())==QDialog::Rejected)
      return false;

  } // while true




  return true;
}

ZStatus
DicEdit::saveDictionaryFile(const uriString& pDicPath, unsigned long pVersion){


  if (DictionaryFile->CreationDate.isInvalid())
    DictionaryFile->CreationDate = ZDateFull::currentDateTime();
  DictionaryFile->ModificationDate = ZDateFull::currentDateTime();

  if (!pDicPath.exists()) {
    return createDictionaryFile(pDicPath,pVersion);
  }

  return DictionaryFile->saveDictionary(pVersion);
}

ZStatus
DicEdit::createDictionaryFile(const uriString& pDicPath, unsigned long pVersion){
  DictionaryFile->CreationDate = ZDateFull::currentDateTime();
  DictionaryFile->ModificationDate = ZDateFull::currentDateTime();
}

void
DicEdit::setDictionary (const ZMFDictionary& pZMFDic)
{
  if (MasterDic!=nullptr)
    delete MasterDic;
  MasterDic = new ZMFDictionary(pZMFDic);
}
void
DicEdit::setNewDictionary ()
{
  if (MasterDic!=nullptr)
    delete MasterDic;
  MasterDic = new ZMFDictionary;
}

ZStatus
DicEdit::loadXmlDictionary (const uriString& pXmlDic)
{
  ZMFDictionary wMasterDic;
  utf8VaryingString wXmlContent,wStr;

  ZaiErrors Errorlog;

  clear();

  ZStatus wSt = pXmlDic.loadUtf8(wXmlContent);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast();  /* loadUtf8 uses ZException */
    return wSt;
  }
  /* here load master dictionary */
  wSt =wMasterDic.XmlLoadFromString(wXmlContent,true,&Errorlog);
  if (wSt!=ZS_SUCCESS)   /* XmlLoadFromString uses returns a ZStatus but uses errorlog to log messages */
  {
    ZExceptionDLg::messageWAdd("FloadfromXmlFileQAc",
        wSt,
        Errorlog.getSeverity(),
        Errorlog.allLoggedToString(),
        "Status is %s : Cannot load xml string definition from file %s",decode_ZStatus(wSt), pXmlDic.toCChar());
    return wSt;
  }
  /* here we may only have warnings, infos or text messages */
  if (Errorlog.hasMessages()) {
    Severity_type wS = Errorlog.getSeverity();

    int wRet = ZExceptionDLg::message2BWAdd("FloadfromXmlFileQAc",
        wS > Severity_Warning?ZS_ERROR:ZS_WARNING,
        wS,
        Errorlog.allLoggedToString(),
        "Stop","Continue",
        "Some warning messages have been issued during load of xml definition from file %s",pXmlDic.toCChar());
    if (wRet==QDialog::Rejected)
      return ZS_CANCEL;
  }

  wStr.sprintf("Xml file %s has been successfully loaded.",pXmlDic.toCChar());
  statusBar()->showMessage(QObject::tr(wStr.toCChar(),"DicEdit"),cst_MessageDuration);
  if (MasterDic!=nullptr)
    delete MasterDic;
  MasterDic = new ZMFDictionary(wMasterDic);
  displayZMFDictionary(MasterDic);
  return ZS_SUCCESS;
}// loadXmlDictionary

void
DicEdit::displayZMFDictionary(ZMFDictionary *pDic)
{
  utf8String wStr;
  QVariant   wV;
  ZDataReference wDRef;

  if (pDic->DicName.isEmpty())
    ui->DicNameLBl->setText("<no name>");
  else
    ui->DicNameLBl->setText(pDic->DicName.toCChar());

  ui->VersionLBl->setText(getVersionStr(pDic->Version).toCChar());

  wStr.sprintf("%ld",pDic->count());
  ui->FieldsNbLBl->setText(wStr.toCChar());

  wStr.sprintf("%ld",pDic->KeyDic.count());
  ui->KeysNbLBl->setText(wStr.toCChar());

  if (fieldTBv->ItemModel)
    if (fieldTBv->ItemModel->rowCount()>0)
      fieldTBv->ItemModel->removeRows(0,fieldTBv->ItemModel->rowCount());


  int wRow=0;
  QList<QStandardItem *> wFieldRow ;

  ZFieldDescription* wFD=nullptr;

  for (long wi=0;wi < pDic->count();wi++)
    {
    wFieldRow = createFieldRowFromField(&pDic->Tab[wi]);  /* create item list from field description */
    /* set up data to item 0 */
    wFD=new ZFieldDescription(pDic->Tab[wi]);
    wDRef.setZLayout(ZLayout_FieldTBv);
    wDRef.setResource(getNewResource(ZEntity_DicField));
    wDRef.setDataRank((long)wi);    /* store rank within meta dictionary (for information) */
    wDRef.setPtr(wFD);              /* store pointer to effective field data */
    wV.setValue<ZDataReference>(wDRef);
    wFieldRow[0]->setData(wV,ZQtDataReference);

    fieldTBv->ItemModel->appendRow(wFieldRow);

    }// for

  fieldTBv->resizeRowsToContents();
  fieldTBv->resizeColumnsToContents();

  displayKeyDictionaries(pDic);

}//DicEdit::displayZMFDictionary

void
DicEdit::displayKeyDictionaries(ZMFDictionary *pDic)
{
  ZKeyHeaderRow* wKHR;
  ZKeyFieldRow wKFR;

  utf8String wStr;
  QVariant   wV;
  ZDataReference wDRefKey , wDRefKeyField;
  QList<QStandardItem *> wKeyRow, wKeyFieldRow ;

  if (keyTRv->ItemModel)
    if (keyTRv->ItemModel->rowCount()>0)
      keyTRv->ItemModel->removeRows(0,keyTRv->ItemModel->rowCount());

  for (long wi=0;wi < pDic->KeyDic.count();wi++) {
    wKHR->set(pDic->KeyDic[wi]);

    wKeyRow=createKeyDicRow(wKHR);

    wDRefKey.setZLayout(ZLayout_KeyTRv);
    wDRefKey.setResource(getNewResource(ZEntity_KeyDic));
    wDRefKey.setDataRank((long)wi);
    wDRefKey.setPtr(new ZKeyHeaderRow(wKHR)); /* store key dictionary Data (to be deleted) */
    wV.setValue<ZDataReference>(wDRefKey);

    wKeyRow[0]->setData(wV,ZQtDataReference);

    for (long wj=0;wj < pDic->KeyDic[wi]->count(); wj++) {
      wKFR.set(pDic->KeyDic[wi]->Tab[wj]);

        wKeyFieldRow = createKeyFieldRow(wKFR);

        wDRefKeyField.clear();

        wDRefKeyField.setResource(getNewResource(ZEntity_KeyField));
        wDRefKeyField.setDataRank((long)wj);
        wDRefKeyField.setPtr(new ZKeyFieldRow(wKFR)); /* store key field Data (to be deleted) */
        wV.setValue<ZDataReference>(wDRefKeyField);
        wKeyFieldRow[0]->setData(wV,ZQtDataReference);

        wKeyRow[0]->appendRow(wKeyFieldRow);
      }// for wj

    keyTRv->ItemModel->appendRow(wKeyRow);

    /* update index after child row has been created */
    wDRefKey.setIndex(wKeyRow[0]->index());
    wV.setValue<ZDataReference>(wDRefKey);
    wKeyRow[0]->setData(wV,ZQtDataReference);

  }// for wi

//  ui->displayKeyTRv->resizeRowsToContents();
  keyTRv->expandAll();
  keyTRv->resizeColumns();
}

QList<QStandardItem *>
DicEdit::createKeyDicRow(const ZKeyHeaderRow& pKHR) {
  QList<QStandardItem *>  wKeyRow;

  wKeyRow << createItem( pKHR.DicKeyName.toCChar());
  wKeyRow[0]->setEditable(false);

  wKeyRow << createItem( "Dictionary key");

  wKeyRow << createItem( " ");

  /* other characteristics of the key */
  wKeyRow << createItem( pKHR.KeyUniversalSize,"size %ld");
  cst_KeyUSizeColumn = 3 ;
  wKeyRow.last()->setEditable(false);
  wKeyRow << createItem( pKHR.Duplicates?"Duplicates":"No duplicate","%s");
  wKeyRow.last()->setEditable(false);

  if (pKHR.ToolTip.isEmpty())
    wKeyRow << createItem( " ");
  else
    wKeyRow << createItemAligned( pKHR.ToolTip.toCChar(),"%s",Qt::AlignLeft);
  wKeyRow.last()->setEditable(false);
  return wKeyRow;
}



void
DicEdit::clearAllRows()
{
  if (fieldTBv->ItemModel)
    if (fieldTBv->ItemModel->rowCount()>0)
      fieldTBv->ItemModel->removeRows(0,fieldTBv->ItemModel->rowCount());

  if (keyTRv->ItemModel)
    if (keyTRv->ItemModel->rowCount()>0)
      keyTRv->ItemModel->removeRows(0,keyTRv->ItemModel->rowCount());
}

void
DicEdit::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (_FResizeInitial) {
    _FResizeInitial=false;
    return;
  }
  QRect wR1 = ui->layoutWidget->geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  ui->layoutWidget->resize(wVW,wVH);  /* expands in width and height */

}//DicEdit::resizeEvent

QList<QStandardItem*> createKeyRowFromKey(ZKeyHeaderRow &wKHR)
{
  QList<QStandardItem *> wKeyRow;

  wKeyRow << createItem(wKHR.DicKeyName.toCChar());
  wKeyRow << createItem (" ");
  wKeyRow << createItem (wKHR.KeyUniversalSize ,"total size: %ld");
  cst_KeyUSizeColumn = 2;
  wKeyRow << createItem (wKHR.Duplicates?"Duplicates":"No duplicates");
  wKeyRow << createItem (wKHR.ToolTip.toCChar());
  return wKeyRow;
}
QList<QStandardItem *>
DicEdit::createKeyFieldRow(const ZKeyFieldRow& wKFR) {
  QList<QStandardItem *>  wKeyFieldRow;
  ZDataReference          wDRef;
  QVariant                wV;

  wKeyFieldRow << createItem(wKFR.Name.toCChar());
  wKeyFieldRow << createItem( wKFR.ZType,"0x%08X");
  wKeyFieldRow << createItem( wKFR.KeyOffset,"%d");
  cst_KeyOffsetColumn = 2;
  wKeyFieldRow << createItem( wKFR.UniversalSize,"%ld");
  wKeyFieldRow << createItem( wKFR.Hash);
  return wKeyFieldRow;
}


/**
 * @brief createRowFromFieldDescription creates an item list describing field desciption.
 *                                      data is not set and must be set by callee.
 * @param pField
 * @return
 */

QList<QStandardItem*> createFieldRowFromField(ZFieldDescription* pField)
{
  utf8String wStr;
  QList<QStandardItem*> wFieldRow;

  wFieldRow << new QStandardItem(pField->getName().toCChar());
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("0x%08X",pField->ZType);
  wFieldRow << new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wFieldRow << new QStandardItem( decode_ZType(pField->ZType));
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("%d",pField->Capacity);
  wFieldRow << new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("%ld",pField->HeaderSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("%ld",pField->UniversalSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wStr.sprintf("%ld",pField->NaturalSize);
  wFieldRow <<  new QStandardItem( wStr.toCChar());
  wFieldRow.last()->setEditable(false);

  wFieldRow <<  new QStandardItem( pField->KeyEligible?"Yes":"No");
  wFieldRow.last()->setEditable(false);

  wFieldRow <<  new QStandardItem( pField->Hash.toHexa().toChar());
  wFieldRow.last()->setEditable(false);

  if (pField->ToolTip.isEmpty())
    wFieldRow <<  new QStandardItem( "" );
  else
    wFieldRow <<  new QStandardItem( pField->ToolTip.toCChar());
  wFieldRow.last()->setEditable(false);


  return wFieldRow;
}

bool setFieldRowFromField(QList<QStandardItem*> &wRow,ZFieldDescription& pField)
{
  utf8String wStr;
  if (wRow.count() < 10) {
    fprintf(stderr,"setRowFromField-E-IVCNT  Invalid number of QStandardItem within list to update.\n");
    return false;
  }
  int Col=0;
  wRow[Col]->setText(pField.getName().toCChar());
  wRow[Col++]->setEditable(false);

  wStr.sprintf("0x%08X",pField.ZType);
  wRow[Col]->setText(wStr.toCChar());
  wRow[Col++]->setEditable(false);

  wRow[Col]->setText(decode_ZType(pField.ZType));
  wRow[Col++]->setEditable(false);

  wStr.sprintf("%d",pField.Capacity);
  wRow[Col]->setText(wStr.toCChar());
  wRow[Col++]->setEditable(false);

  wStr.sprintf("%ld",pField.HeaderSize);
  wRow[Col]->setText(wStr.toCChar());
  wRow[Col++]->setEditable(false);

  wStr.sprintf("%ld",pField.UniversalSize);
  wRow[Col]->setText(wStr.toCChar());
  wRow[Col++]->setEditable(false);

  wStr.sprintf("%ld",pField.NaturalSize);
  wRow[Col]->setText(wStr.toCChar());
  wRow[Col++]->setEditable(false);

  wRow[Col]->setText(pField.KeyEligible?"Yes":"No");
  wRow[Col++]->setEditable(false);

  wRow[Col]->setText(pField.Hash.toHexa().toChar());
  wRow[Col++]->setEditable(false);

  if (pField.ToolTip.isEmpty())
    wRow[Col]->setText("");
  else
    wRow[Col]->setText(pField.ToolTip.toCChar());
  wRow[Col++]->setEditable(false);

  return true;
}
