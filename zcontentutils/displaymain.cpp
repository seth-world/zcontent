#include "displaymain.h"
#include "ui_displaymain.h"


#include <qmessagebox.h>

#include <zqt/zqtwidget/zqstandarditem.h>

#include <QStandardItemModel>

#include <zrandomfile/zheadercontrolblock.h>
#include <zrandomfile/zfilecontrolblock.h>
#include <zindexedfile/zmastercontrolblock.h>

#include <zexceptiondlg.h>

#include <zcontentvisumain.h>

#include <zqt/zqtwidget/zqtwidgettools.h>

#include <ztoolset/zfunctions.h> /* for version numbers management */

#include <qevent.h>

extern Qt::AlignmentFlag QtAlignmentFlag;


DisplayMain::DisplayMain(ZContentVisuMain *parent) :QMainWindow((QWidget*)parent),ui(new Ui::DisplayMain)
{
  ui->setupUi(this);
  VisuMain=parent;
  setWindowTitle("Entity display");

/*  ui->displayTBv->setStyleSheet(QString::fromUtf8("QTableView::item{border-left : 1px solid black;\n"
                                                "border-right  : 1px solid black;\n"
                                                "font: 75 12pt \"Courier\";\n"
                                                " }"));
*/
  displayItemModel=new QStandardItemModel(0,5,this) ;/* 4 columns */

  ui->ClosedLBl->setVisible(false);

  displayItemModel->setHorizontalHeaderItem(0,new QStandardItem(tr("Size")));
  displayItemModel->setHorizontalHeaderItem(1,new QStandardItem(tr("Field")));
  displayItemModel->setHorizontalHeaderItem(2,new QStandardItem(tr("Raw value(hexa)")));
  displayItemModel->setHorizontalHeaderItem(3,new QStandardItem(tr("converted")));
  displayItemModel->setHorizontalHeaderItem(4,new QStandardItem(tr("additional info")));

  ui->displayTBv->setModel(displayItemModel );
  ui->displayTBv->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->displayTBv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


//  ui->displayTBv->verticalHeader()->hide();

  ui->displayTBv->setShowGrid(true);

  ui->displayTBv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
  ui->displayTBv->setWordWrap(false);

  ui->displayTBv->setSortingEnabled(false);  // will be set to true in the end of setup_effective

  ui->OffsetLBl->setText("0  - Ox0");

//  ui->OffsetSLd->setSingleStep(1);

  ui->offsetSBx->setSingleStep(1);

  QObject::connect(ui->offsetSBx, SIGNAL(valueChanged(int)), this, SLOT(sliderChange(int)));
}

void DisplayMain::setFileClosed(bool pYesNo)
{
  ui->ClosedLBl->setVisible(pYesNo);
}

void
DisplayMain::displayHCB(ZDataBuffer &pData)
{
  utf8String wStr;
  PtrIn=pData.Data;
  if (pData.Size < sizeof(ZHeaderControlBlock_Export))
  {
    wStr.sprintf("Cannot view Header Control Block requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",sizeof(ZHeaderControlBlock_Export),pData.Size);
    ZExceptionDLg::message("DisplayMain::displayHCB",ZS_NEEDMORESPACE,Severity_Error,wStr.toCChar());
    return;
  }
  CurrentDisplay= ZDW_HCB;

  ContentToDump.setData(pData);

  clear();
  show();

  Offset=0;

  ui->offsetSBx->setMaximum(int(pData.Size));

/*  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pData.Size);
  ui->OffsetSLd->setValue (0);
  ui->OffsetSLd->setSingleStep(1);
*/
  ui->offsetSBx->setValue(0);

  displayHCBValues(pData.Data);

}
void
DisplayMain::setOffset(size_t pOffset,size_t pMax) {
  Offset=pOffset;
  /*
  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pMax);
  ui->OffsetSLd->setValue (Offset);
*/
  ui->offsetSBx->setMaximum(int(pMax));
  ui->offsetSBx->setValue(int(Offset));


  utf8VaryingString wStr;
  wStr.sprintf("%4ld - Ox%4lX",Offset,Offset);
  ui->OffsetLBl->setText(wStr.toCChar());
}


void
DisplayMain::displayFCB(ZDataBuffer &pData)
{
  utf8String wStr;
  PtrIn=pData.Data;
  clear();
  show();
  if (pData.Size < sizeof(ZHeaderControlBlock_Export))
  {
    ZExceptionBase wE= ZExceptionBase::create("DisplayMain::displayFCB",ZS_INVSIZE,Severity_Error,
        "Cannot view File Control Block requested minimum size <%ld> Bytes \n"
        "Only <%ld> Bytes have been loaded.\n\n"
        "Load / reload file content <Reload>\n"
        "Quit <Quit>",sizeof(ZHeaderControlBlock_Export),pData.Size);

    int wRet=ZExceptionDLg::display2B("FCB Exception",wE,"Quit","Reload");
    if (wRet==ZEDLG_Rejected)
        return;
//      case ZEDLG_Accepted:
    if (VisuMain->URICurrent.getFileSize()>100000000)
        {
        ZExceptionDLg::message("DisplayMain::displayFCB",ZS_INVSIZE,Severity_Error,
                              "Cannot load content of file <%s> size <%lld> exceeds capacity",
                               VisuMain->URICurrent.toCChar(),
                               VisuMain->URICurrent.getFileSize());
        return;
        }
    if (VisuMain->URICurrent.getFileSize()<(sizeof(ZHeaderControlBlock_Export)+sizeof(ZFCB_Export)+1))
        {
        ZExceptionDLg::message("DisplayMain::displayFCB",ZS_INVSIZE,Severity_Error,
            "file <%s> has size <%lld> that does not allow to store a header + a file control block.",
            VisuMain->URICurrent.toCChar(),
            VisuMain->URICurrent.getFileSize());
        return;
        }
/*      ZDataBuffer wNewRaw;
      VisuMain->URICurrent.loadContent(wNewRaw);
      DisplayMain::displayFCB(wNewRaw);
*/
      VisuMain->URICurrent.loadContent(VisuMain->RawData);
      DisplayMain::displayFCB(VisuMain->RawData);
      return;
  }//if (pData.Size < sizeof(ZHeaderControlBlock_Export))

  CurrentDisplay= ZDW_FCB;

  ContentToDump.setData(pData);

  ZHeaderControlBlock_Export* wHCBExport=(ZHeaderControlBlock_Export*)pData.Data;
  zaddress_type wOffset=reverseByteOrder_Conditional(wHCBExport->OffsetFCB);
  if (pData.Size < (wOffset+sizeof(ZFCB_Export)))
  {
    utf8String wStr;
    wStr.sprintf("Cannot view File Control Block : requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",(wOffset+sizeof(ZFCB_Export)),pData.Size);
    ZExceptionDLg::message("DisplayMain::displayHCB",ZS_NEEDMORESPACE,Severity_Error,wStr.toCChar());
    return;
  }

  setOffset(wOffset,pData.Size);
/*
  Offset=wOffset;
  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pData.Size);
  ui->OffsetSLd->setValue (Offset);
  wStr.sprintf("%4ld - Ox%4lX",Offset,Offset);
  ui->OffsetLBl->setText(wStr.toCChar());
*/
  displayFCBValues(pData.Data+wOffset);
}

void
DisplayMain::displayPool(ZDataBuffer& pData)
{
  PtrIn=pData.Data;
  CurrentDisplay= ZDW_POOL;
  clear();
  show();
  displayPoolValues(PtrIn);

}

void
DisplayMain::getMCB(ZDataBuffer& pData)
{
  utf8String wStr;
  PtrIn=pData.Data;
  clear();
  show();
  if (pData.Size < sizeof(ZHeaderControlBlock_Export))
  {
    utf8String wStr;
    wStr.sprintf("Cannot view Header Control Block : requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",sizeof(ZHeaderControlBlock_Export),pData.Size);
    ZExceptionDLg::message("DisplayMain::getMCB",ZS_NEEDMORESPACE,Severity_Error,wStr.toCChar());
    return;
  }

  CurrentDisplay= ZDW_MCB;

  ZHeaderControlBlock_Export* wHCBExport=(ZHeaderControlBlock_Export*)pData.Data;
  zaddress_type wOffset=reverseByteOrder_Conditional(wHCBExport->OffsetReserved);
  if (pData.Size < (wOffset+sizeof(ZMCB_Export)))
  {
    utf8String wStr;
    wStr.sprintf("Not enough loaded data. Requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",(wOffset+sizeof(ZMCB_Export)),pData.Size);
    ZExceptionDLg::message("DisplayMain::getMCB",ZS_NEEDMORESPACE,Severity_Error,wStr.toCChar());
    return;
  }
  setOffset(wOffset,pData.Size);
  /*
  Offset=wOffset;
  wStr.sprintf("%4ld - Ox%4lX",Offset,Offset);
  ui->OffsetLBl->setText(wStr.toCChar());

  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pData.Size);
  ui->OffsetSLd->setValue (Offset);
*/
  displayMCBValues(pData.Data+wOffset);
}


void
DisplayMain::displayMCB(ZDataBuffer& pData)
{
  utf8String wStr;
  PtrIn=pData.Data;
  clear();
  show();
  if (pData.Size < sizeof(ZHeaderControlBlock_Export))
  {
    utf8String wStr;
    wStr.sprintf("Not enough loaded data. Requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",sizeof(ZHeaderControlBlock_Export),pData.Size);
    ZExceptionDLg::message("DisplayMain::displayMCB",ZS_NEEDMORESPACE,Severity_Error,wStr.toCChar());
    return;
  }

  ZHeaderControlBlock_Export* wHCBExport=(ZHeaderControlBlock_Export*)pData.Data;
  zaddress_type wOffset=reverseByteOrder_Conditional(wHCBExport->OffsetReserved);
  if (pData.Size < (wOffset+sizeof(ZMCB_Export)))
    {
    utf8String wStr;
    wStr.sprintf("Not enough loaded data. Requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",(wOffset+sizeof(ZMCB_Export)),pData.Size);
    ZExceptionDLg::message("DisplayMain::displayMCB",ZS_NEEDMORESPACE,Severity_Error,wStr.toCChar());
    return;
    }

  CurrentDisplay= ZDW_MCB;
  ContentToDump.setData(pData);

  setOffset(wOffset,pData.Size);
  /*
  Offset=wOffset;
  wStr.sprintf("%4ld - Ox%4lX",Offset,Offset);
  ui->OffsetLBl->setText(wStr.toCChar());

  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pData.Size);
  ui->OffsetSLd->setValue (Offset);
*/
  displayMCBValues(pData.Data+wOffset);
}

void
DisplayMain::displayAll(unsigned char* pData)
{
  PtrIn=pData;
  clear();
  show();
  displayHCBValues(PtrIn);
  ZHeaderControlBlock_Export* wZHCB=(ZHeaderControlBlock_Export*)PtrIn;
  displayFCBValues(PtrIn+wZHCB->OffsetFCB);
  displayMCBValues(PtrIn+wZHCB->OffsetReserved);
}




void
DisplayMain::sliderChange(int pValue)
{
  utf8String wStr;
  Offset = pValue ;
  wStr.sprintf("%4ld - Ox%4lX",Offset,Offset);
  ui->OffsetLBl->setText(wStr.toCChar());



  switch (CurrentDisplay)
  {
  case ZDW_HCB:
    displayHCBValues(ContentToDump.Data+pValue);
    return;
  case ZDW_FCB:
    displayFCBValues(ContentToDump.Data+pValue);
    return;
  case ZDW_MCB:
    displayMCBValues(ContentToDump.Data+pValue);
    return;
  case ZDW_ICB:
    displayICBValues(ContentToDump.Data+pValue);
    return;
  case ZDW_POOL:
    displayPoolValues(ContentToDump.Data+pValue);
    return;
  default:
    return;
  }

}//sliderChange


void
DisplayMain::displayHCBValues(unsigned char *pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String wStr;
  zaddress_type wOffsetReserved=0;
  int64_t       wInt64;
  long wOffset=Offset;

  int wRowOffsetFCB=0;
//  ui->ZEntityLBl->setText("ZHeaderControlBlock");
  setWindowTitle(QObject::tr("Header control block","DisplayMain"));
/*
  ContentToDump.setData(pPtrIn,sizeof(ZHeaderControlBlock_Export));
  ZHeaderControlBlock_Export* wHCBExport=(ZHeaderControlBlock_Export*)ContentToDump.Data;
*/
  ZHeaderControlBlock_Export* wHCBExport=(ZHeaderControlBlock_Export*)pPtrIn;


  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());


  ZQStandardItem::setItemAlignment(Qt::AlignRight);

  int wRow=0;
  QList<QStandardItem *> wDumpRow ;
//  QStandardItem* wCurItem=nullptr;

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->StartSign),"%ld");
  wDumpRow << createItem( "StartSign");
  wDumpRow.last()->setToolTip("Special value that marks the beginning of the block");
  wDumpRow << createItem(wHCBExport->StartSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wHCBExport->StartSign),"0x%08X");
  wStr.sprintf("%s",wHCBExport->StartSign==cst_ZBLOCKSTART?"correct":"incorrect");
  wDumpRow << createItem(wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,StartSign),wRow++);

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->BlockId),"%ld");
  wDumpRow << createItem( "BlockId");
  wDumpRow.last()->setToolTip("Block identification");
  wDumpRow << createItem((uint8_t)wHCBExport->BlockId,"0x%02X");
  wDumpRow << createItem((uint8_t)wHCBExport->BlockId,"0x%02X");
  wDumpRow << createItem(decode_BlockId(wHCBExport->BlockId));
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,BlockId),wRow++);

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->EndianCheck),"%ld");
  wDumpRow << createItem( "EndianCheck");
  wDumpRow.last()->setToolTip("Controls if values have been serialized or not");
  wStr.sprintf("0x%04X",wHCBExport->EndianCheck);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%d",reverseByteOrder_Conditional(wHCBExport->EndianCheck));
  wDumpRow <<  createItem( wStr.toCChar());

  wStr.sprintf("%s",wHCBExport->isReversed()?"serialized":"NOT serialized");
  wDumpRow <<  createItem(wStr.toCChar());
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,EndianCheck),wRow++);

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->ZRFVersion),"%ld");
  wDumpRow << createItem( "ZRFVersion");
  wDumpRow.last()->setToolTip("Random file full software version expressed as an unsigned long");
  wDumpRow << createItem(wHCBExport->ZRFVersion,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<unsigned long>(wHCBExport->ZRFVersion),"%lu");
  unsigned long wVersion = reverseByteOrder_Conditional<unsigned long>(wHCBExport->ZRFVersion);
  wDumpRow << createItem(getVersionStr(wVersion).toCChar());

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,ZRFVersion),wRow++);


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->FileType),"%ld");
  wDumpRow << createItem( "FileType");
  wDumpRow.last()->setToolTip("Type of file this header relates to : could be Random - Master - Raw Master - Index ");
  wDumpRow << createItem((uint8_t)wHCBExport->FileType,"0x%02X");
  wDumpRow << createItem((uint8_t)wHCBExport->FileType,"0x%02X");
  wDumpRow << createItem(decode_ZFile_type(wHCBExport->FileType));
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,FileType),wRow++);

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->Lock),"%ld");
  wDumpRow << createItem( "Lock");
  wDumpRow.last()->setToolTip("Lock mask file is currently locked with ");
  wDumpRow << createItem((uint8_t)wHCBExport->Lock,"0x%02X");
  wDumpRow << createItem((uint8_t)wHCBExport->Lock,"0x%02X");
  wDumpRow << createItem(decode_ZLockMask(wHCBExport->Lock).toCChar());
  displayItemModel->appendRow(wDumpRow);


  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,Lock),wRow++);

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->LockOwner),"%ld");
  wDumpRow << createItem( "LockOwner");
  wDumpRow.last()->setToolTip("System id of user currently locking the file");
  wDumpRow << createItem(wHCBExport->LockOwner,"0x%X");
  wDumpRow << createItem(reverseByteOrder_Conditional<pid_t>(wHCBExport->LockOwner),"0x%X");
  wStr.sprintf("current: 0x%lX",getpid());
  wDumpRow << createItem(wStr.toCChar());
  displayItemModel->appendRow(wDumpRow);


  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,LockOwner),wRow++);


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->OffsetFCB),"%ld");
  wDumpRow << createItem( "OffsetFCB");
  wDumpRow.last()->setToolTip("Offset in bytes to File Control Block since begining of Header");
  wDumpRow << createItem(wHCBExport->OffsetFCB,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<zaddress_type>(wHCBExport->OffsetFCB),"%ld");

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,OffsetFCB),wRow++);

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->OffsetReserved),"%ld");
  wDumpRow << createItem( "OffsetReserved");
  wDumpRow.last()->setToolTip("Offset in bytes to Reserved Section (may contain MCB for instance) since begining of Header");
  wDumpRow << createItem(wHCBExport->OffsetReserved,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<zaddress_type>(wHCBExport->OffsetReserved),"%ld");

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,OffsetReserved),wRow++);

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->SizeReserved),"%ld");
  wDumpRow << createItem( "SizeReserved");
  wDumpRow.last()->setToolTip("Size in bytes of exported Reserved Section (may contain MCB for instance)");
  wDumpRow << createItem(wHCBExport->SizeReserved,"0x%lX");
  wInt64=reverseByteOrder_Conditional<zaddress_type>(wHCBExport->SizeReserved);
  wDumpRow << createItem(wInt64,"%lld");

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,SizeReserved),wRow++);

  /* update theorical offsetFCB value */
  wStr.sprintf("0x%X - %ld ",wInt64+wOffsetReserved,wInt64+wOffsetReserved);
  displayItemModel->setItem(wRowOffsetFCB,4,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();

  wDumpRow << createItem(sizeof(wHCBExport->EndSign),"%ld");
  wDumpRow << createItem( "EndSign");
  wDumpRow.last()->setToolTip("Special value that mark the end of the block");
  wDumpRow << createItem(wHCBExport->EndSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wHCBExport->EndSign),"0x%08X");
  wStr.sprintf("%s",wHCBExport->EndSign==cst_ZBLOCKEND?"correct":"incorrect");
  wDumpRow << createItem(wStr.toCChar());
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(Offset+offsetof(ZHeaderControlBlock_Export,EndSign),wRow++);

  size_t wCurrentOffset = sizeof(ZHeaderControlBlock_Export);

  wDumpRow.clear();
  wDumpRow << createItem(" ");
  wDumpRow << createItem(" ");
  wDumpRow << createItem( "---end---");
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
  wRow++;

  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();
}// displayHCBValues

#define _DISPLAY_UINT32_(__NAME__)   wDumpRow.clear(); \
wDumpRow << new QStandardItem( ""); \
wDumpRow << new QStandardItem( "__NAME__"); \
wStr.sprintf("%08X",wMCBExport->__NAME__); \
wDumpRow << new QStandardItem( wFieldContent.toCChar()); \
wStr.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->__NAME__)); \
wDumpRow <<  new QStandardItem( wFieldContent.toCChar()); \
wDumpRow <<  new QStandardItem( ""); \
displayItemModel->appendRow(wDumpRow); \
\
wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,__NAME__)); \
displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));





void
DisplayMain::displayPoolValues(unsigned char* pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String wStr;

  setWindowTitle(QObject::tr("Pool","DisplayMain"));
}


void
DisplayMain::displayICBs(ZDataBuffer &pData) {
  CurrentDisplay= ZDW_ICB;
  ContentToDump.setData(pData);

  const unsigned char* wPtr = pData.Data;
  ZHeaderControlBlock_Export wZHCBe;
  memmove(&wZHCBe,ContentToDump.Data,sizeof(ZHeaderControlBlock_Export));
  if (wZHCBe.StartSign!=cst_ZBLOCKSTART) {
    ZExceptionDLg::adhocMessage("Invalid Header",Severity_Error,nullptr,nullptr,"Header Control Block appears to be corrupted.");
    return;
  }
  wZHCBe.deserialize();
  ZMCB_Export wZMCBe;
  memmove(&wZMCBe,wPtr+wZHCBe.OffsetReserved,sizeof(ZMCB_Export));
  wZMCBe.deserialize();
  if (!wZMCBe.isValid()) {
    ZExceptionDLg::adhocMessage("Invalid MCB",Severity_Error,nullptr,nullptr,"Master Control Block (Reserved space) appears to be corrupted.");
    return;
  }

  size_t wOffset = wZHCBe.OffsetReserved + wZMCBe.ICBOffset;

  setOffset(wOffset,pData.Size);

  wPtr = wPtr + Offset;

  displayICBValues(wPtr);

}
void
//DisplayMain::displayICBValues(const unsigned char* pPtrIn,size_t &pOffsetFromMCB, int &wRow) {
DisplayMain::displayICBValues(const unsigned char *pPtrIn) {

  setWindowTitle(QObject::tr("Master File ICBs list","DisplayMain"));
  const unsigned char* wPtr = pPtrIn ;
  bool wRet = true;
  int wCount=0;
  int wRow=0;

  displayItemModel->removeRows(0,displayItemModel->rowCount());

  size_t wOffset=Offset;
  while (wRet && (wCount++ < 20)) { /* no more than 20 indexes to display */
    wRet=displaySingleICBValues(wPtr,wOffset,wRow);
  }

  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();

  show();
}


void DisplayMain::createMarginItem(size_t pValue, int pRow) {
  utf8VaryingString wStr;
  wStr.sprintf("%4lu 0x%4lX",pValue,pValue);
  displayItemModel->setVerticalHeaderItem(pRow,createItem(wStr.toCChar()));
}


/** pPtrIn is supposed to point to first ICB.
 *  pOffset is offset to be displayed (since beginning of file)
 */
bool
//DisplayMain::displayICBValues(const unsigned char* pPtrIn,size_t &pOffsetFromMCB, int &wRow) {
DisplayMain::displaySingleICBValues(const unsigned char* &pPtrIn,size_t &pDisplayOffset,int &wRow) {
  if (pPtrIn==nullptr)
    return false;

//  ui->ZEntityLBl->setText("ZIndexControlBlock");

  utf8VaryingString wStr;
//  utf8VaryingString wIndexFilePath,wIndexName,wKeyName;


  QList<QStandardItem*> wDumpRow;
  uint32_t wUInt32;

  const ZICB_Export* wICBe=(ZICB_Export*)pPtrIn;

//  size_t wDisplayOffsetBase = pDisplayOffset ;  /* general offset shift */
  size_t wCurrentOffset=pDisplayOffset;

  if (wICBe->StartSign == cst_ZBLOCKEND) {
    wDumpRow.clear();
    wDumpRow << createItem(sizeof(uint32_t),"%u");
    wDumpRow << createItem( "EndBlockMark");
    wDumpRow << createItem(wICBe->StartSign,"0x%08X");
    wDumpRow << createItem(reverseByteOrder_Conditional( wICBe->StartSign),"0x%08X");
    wDumpRow << createItem("No more index control blocks");

    displayItemModel->appendRow(wDumpRow);

    createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
    wRow++;

    wCurrentOffset += sizeof(uint32_t) ;
    wDumpRow.clear();
    wDumpRow << createItem(" ");
    wDumpRow << createItem(" ");
    wDumpRow << createItem("--end of ZICB--");
    displayItemModel->appendRow(wDumpRow);

    createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
    wRow++;

    pDisplayOffset += sizeof(cst_ZBLOCKEND);
    return false;
  }

  wDumpRow.clear();
  wDumpRow << createItem(" ");
  wDumpRow << createItem( "---");
  wDumpRow << createItem(wICBe->StartSign,"Index Control Block");
  wDumpRow << createItem("---");
  wDumpRow << createItem(" ");
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
  wRow++;

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wICBe->StartSign),"%u");
  wDumpRow << createItem( "StartSign");
  wDumpRow.last()->setToolTip("Special value that marks the beginning of the block");
  wDumpRow << createItem(wICBe->StartSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wICBe->StartSign),"0x%08X");
  wDumpRow << createItem(wICBe->StartSign==cst_ZBLOCKSTART?"correct":"incorrect");

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
  wRow++;

  wCurrentOffset += offsetof(ZHeaderControlBlock_Export,StartSign) ;


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wICBe->BlockId),"%lu");
  wDumpRow << createItem( "BlockId");
  wDumpRow.last()->setToolTip("Block identification");
  wDumpRow << createItem((uint8_t)wICBe->BlockId,"0x%02X");
  wDumpRow << createItem((uint8_t)wICBe->BlockId,"0x%02X");
  wDumpRow << createItem(decode_BlockId(wICBe->BlockId));
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(pDisplayOffset + offsetof(ZICB_Export,BlockId),displayItemModel->rowCount()-1);
  wRow++;


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wICBe->EndianCheck),"%lu");
  wDumpRow << createItem( "EndianCheck");
  wDumpRow.last()->setToolTip("Controls if values have been serialized or not");
  wStr.sprintf("0x%04X",wICBe->EndianCheck);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%u",reverseByteOrder_Conditional(wICBe->EndianCheck));
  wDumpRow <<  createItem( wStr.toCChar());
  wStr.sprintf("%s",wICBe->isReversed()?"serialized":"NOT serialized");
  wDumpRow <<  createItem(wStr.toCChar());
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(pDisplayOffset + offsetof(ZICB_Export,EndianCheck),displayItemModel->rowCount()-1);
  wRow++;


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wICBe->ZMFVersion),"%lu");
  wDumpRow << createItem( "ZMFVersion");
  wDumpRow.last()->setToolTip("Master file full software version expressed as an unsigned long");
  wDumpRow << createItem(wICBe->ZMFVersion,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<unsigned long>(wICBe->ZMFVersion),"%lu");
  unsigned long wVersion = reverseByteOrder_Conditional<unsigned long>(wICBe->ZMFVersion);
  wDumpRow << createItem(getVersionStr(wVersion).toCChar());

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(pDisplayOffset + offsetof(ZICB_Export,ZMFVersion),displayItemModel->rowCount()-1);
  wRow++;

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wICBe->ICBTotalSize),"%lu");
  wDumpRow << createItem( "ICBSize");
  wDumpRow.last()->setToolTip("Total size in bytes of exported index control block (including key dictionary)");
  wDumpRow << createItem(wICBe->ICBTotalSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional<uint32_t>(wICBe->ICBTotalSize);
  wDumpRow << createItem(wUInt32,"%u");

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(pDisplayOffset + offsetof(ZICB_Export,ICBTotalSize),displayItemModel->rowCount()-1);
  wRow++;


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wICBe->ZKDicOffset),"%lu");
  wDumpRow << createItem( "ZKDicOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to just following key dictionary.");
  wDumpRow << createItem(wICBe->ZKDicOffset,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wICBe->ZKDicOffset);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(pDisplayOffset + offsetof(ZICB_Export,ZKDicOffset),displayItemModel->rowCount()-1);
  wRow++;

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wICBe->KeyUniversalSize),"%lu");
  wDumpRow << createItem( "KeyUniversalSize");
  wDumpRow.last()->setToolTip("Offset in bytes to first Index Control Block since beginning of Master Control Block");
  wDumpRow << createItem(wICBe->KeyUniversalSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wICBe->KeyUniversalSize);
  wDumpRow << createItem(wUInt32,"%u");

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(pDisplayOffset + offsetof(ZICB_Export,KeyUniversalSize),displayItemModel->rowCount()-1);
  wRow++;

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wICBe->Duplicates),"%lu");
  wDumpRow << createItem( "Duplicates");
  wDumpRow.last()->setToolTip("Defines if index accepts duplicates (set) or not (unset)");
  wDumpRow << createItem((uint8_t)wICBe->Duplicates,"0x%X");
  wDumpRow << createItem(wICBe->Duplicates?"Yes":"No");

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(pDisplayOffset + offsetof(ZICB_Export,Duplicates),displayItemModel->rowCount()-1);
  wRow++;


  wCurrentOffset = pDisplayOffset + sizeof(ZICB_Export); /* update offset */

  ZICB_Export wCheck ;
  memmove(&wCheck,wICBe,sizeof(ZICB_Export));
  wCheck.deserialize();
  if (!wCheck.isValid()) { /* invalid block : no more to dump */

    wDumpRow.clear();
    wDumpRow << createItem(" ");
    wDumpRow << createItem( " ");

    wDumpRow << createItem("Invalid ICB. No more to display.");

    displayItemModel->appendRow(wDumpRow);

    createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
    wRow++;

    ui->displayTBv->resizeRowsToContents();
    ui->displayTBv->resizeColumnsToContents();
    return false;
  }
  /* dump 2 strings (index name and index file name */
  utf8VaryingString wIndexName ;
  uriString    wURIIndex;

  pPtrIn += sizeof(ZICB_Export) ;


  size_t wSP=wIndexName._importUVF(pPtrIn); /* pPtrIn is updated */

  wDumpRow.clear();
  wDumpRow << createItem(wSP,"%lu");
  wDumpRow << createItem( "IndexName");
  wDumpRow.last()->setToolTip("Unique name for the index");
  wDumpRow << createItem( wIndexName.toCChar());
  wDumpRow <<  createItem( wIndexName.toCChar());

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
  wRow++;

  wCurrentOffset += wSP;

  wSP=wURIIndex._importUVF(pPtrIn); /* pPtrIn is updated */

  utf8VaryingString wDir=wURIIndex.getDirectoryPath() ;

  if (wDir.strlen() > 20) {

    wDir = wDir.Right(20).insert(utf8VaryingString((const utf8_t*)"◊"),0);
  }
  wDumpRow.clear();
  wDumpRow << createItem(wSP,"%lu");
  wDumpRow << createItem( "IndexFile");
  wDumpRow.last()->setToolTip("Index file location and name");
  wDumpRow << createItem( wDir.toCChar());
  wDumpRow << createItem( wURIIndex.getBasename().toCChar());

  displayItemModel->appendRow(wDumpRow);

  createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
  wRow++;

  wCurrentOffset += wSP;

  wDumpRow.clear();
  wDumpRow << createItem(" ");
  wDumpRow << createItem(" ");
  wDumpRow << createItem( "---end---");
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(wCurrentOffset,displayItemModel->rowCount()-1);
  wRow++;

  pDisplayOffset = wCurrentOffset;

  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();

  return true;

}// displayICB
void
DisplayMain::displayMCBValues(const unsigned char* pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String    wStr;
  size_t        wOffset=Offset;

  uint32_t      wUInt32;

  setWindowTitle(QObject::tr("Master control block","DisplayMain"));
//  ui->ZEntityLBl->setText("ZMasterControlBlock");
/*
  ContentToDump.setData(pPtrIn,sizeof(ZSMCBOwnData_Export));
  ZSMCBOwnData_Export* wMCBExport=(ZSMCBOwnData_Export*)ContentToDump.Data;
*/
  const ZMCB_Export* wMCBExport= (const ZMCB_Export*)(pPtrIn);

  ZMCB_Export wMCBExportDeserialized;
  wMCBExportDeserialized._import(pPtrIn);

  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());

  int wRow=0;
  QList<QStandardItem *> wDumpRow ;

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->StartSign),"%lu");
  wDumpRow << createItem( "StartSign");
  wDumpRow.last()->setToolTip("Special value that marks the beginning of the block");
  wDumpRow << createItem(wMCBExport->StartSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wMCBExport->StartSign),"0x%08X");
  wStr.sprintf("%s",wMCBExport->StartSign==cst_ZBLOCKSTART?"correct":"incorrect");
  wDumpRow << createItem(wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=wOffset+offsetof(ZHeaderControlBlock_Export,StartSign);
  wStr.sprintf("%4lu 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->BlockId),"%lu");
  wDumpRow << createItem( "BlockId");
  wDumpRow.last()->setToolTip("Block identification");
  wDumpRow << createItem((uint8_t)wMCBExport->BlockId,"0x%02X");
  wDumpRow << createItem((uint8_t)wMCBExport->BlockId,"0x%02X");
  wDumpRow << createItem(decode_BlockId(wMCBExport->BlockId));
  displayItemModel->appendRow(wDumpRow);

  wOffset=wOffset+offsetof(ZHeaderControlBlock_Export,BlockId);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->EndianCheck),"%lu");
  wDumpRow << createItem( "EndianCheck");
  wDumpRow.last()->setToolTip("Controls if values have been serialized or not");
  wStr.sprintf("0x%04X",wMCBExport->EndianCheck);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%d",reverseByteOrder_Conditional(wMCBExport->EndianCheck));
  wDumpRow <<  createItem( wStr.toCChar());

  wStr.sprintf("%s",wMCBExport->isReversed()?"serialized":"NOT serialized");
  wDumpRow <<  createItem(wStr.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,EndianCheck);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->ZMFVersion),"%lu");
  wDumpRow << createItem( "ZMFVersion");
  wDumpRow.last()->setToolTip("Master file full software version expressed as an unsigned long");
  wDumpRow << createItem(wMCBExport->ZMFVersion,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<unsigned long>(wMCBExport->ZMFVersion),"%lu");
  unsigned long wVersion = reverseByteOrder_Conditional<unsigned long>(wMCBExport->ZMFVersion);
  wDumpRow << createItem(getVersionStr(wVersion).toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZMCB_Export,ZMFVersion);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->MCBSize),"%lu");
  wDumpRow << createItem( "MCBSize");
  wDumpRow.last()->setToolTip("Size in bytes of exported Master Control Block");
  wDumpRow << createItem(wMCBExport->MCBSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->MCBSize);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZMCB_Export,MCBSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->ICBOffset),"%lu");
  wDumpRow << createItem( "ICBOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to first Index Control Block since beginning of Master Control Block");
  wDumpRow << createItem(wMCBExport->ICBOffset,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->ICBOffset);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZMCB_Export,ICBOffset);
  wStr.sprintf("%4ld 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->ICBSize),"%lu");
  wDumpRow << createItem( "ICBSize");
  wDumpRow.last()->setToolTip("Size in bytes of Index Control Blocks table");
  wDumpRow << createItem(wMCBExport->ICBSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional<uint32_t>(wMCBExport->ICBSize);
  wDumpRow << createItem(wUInt32,"%u");

  displayItemModel->appendRow(wDumpRow);

  wOffset=size_t(Offset)+offsetof(ZMCB_Export,ICBSize);
  wStr.sprintf("%4lu 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->JCBOffset),"%ld");
  wDumpRow << createItem( "JCBOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to Journal Control Block since beginning of Master Control Block");
  wDumpRow << createItem(wMCBExport->JCBOffset,"0x%X");
  wUInt32=reverseByteOrder_Conditional<uint32_t>(wMCBExport->JCBOffset);
  wDumpRow << createItem(wUInt32,"%u");

  displayItemModel->appendRow(wDumpRow);

  wOffset=size_t(Offset)+offsetof(ZMCB_Export,JCBOffset);
  wStr.sprintf("%4ld 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->JCBSize),"%lu");
  wDumpRow << createItem( "JCBSize");
  wDumpRow.last()->setToolTip("Size in bytes of Journal Control Block");
  wDumpRow << createItem(wMCBExport->JCBSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional<uint32_t>(wMCBExport->JCBSize);
  wDumpRow << createItem(wUInt32,"%u");

  displayItemModel->appendRow(wDumpRow);

  wOffset=size_t(Offset)+offsetof(ZMCB_Export,JCBSize);
  wStr.sprintf("%4lu 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->HistoryOn),"%lu");
  wDumpRow << createItem( "HistoryOn");
  wDumpRow.last()->setToolTip("(Option) If set, historize process is on (Reserved for future use).");
  wStr.sprintf("0x%X",wMCBExport->HistoryOn);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%s",wMCBExport->HistoryOn?"Option set":"Option not set");
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+ offsetof(ZMCB_Export,HistoryOn);
  wStr.sprintf("%4lu 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  if (wMCBExport->StartSign != cst_ZBLOCKSTART) {
    ui->displayTBv->resizeRowsToContents();
    ui->displayTBv->resizeColumnsToContents();
    return;
  }

  /* display 3 strings */
/*
  wReturn.appendData(DictionaryName._exportUVF());
  wReturn.appendData(DictionaryPath._exportUVF());
  wReturn.appendData(IndexFilePath._exportUVF());
*/

  utf8VaryingString wDictionaryName,wDictionaryPath,wIndexFilePath;
  wOffset = Offset + sizeof(ZMCB_Export);
  const unsigned char* wPtr = pPtrIn + wOffset;

  size_t wSP=wDictionaryName._importUVF(wPtr); /* wPtr is updated */

  wDumpRow.clear();
  wDumpRow << createItem(wSP,"%lu");
  wDumpRow << createItem( "DictionaryName");
  wDumpRow.last()->setToolTip("Dictionary name used");
  wDumpRow << createItem( wDictionaryName.toCChar());
  wDumpRow <<  createItem( wDictionaryName.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("%4lu 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wOffset += wSP;

  wSP=wDictionaryPath._importUVF(wPtr); /* wPtr is updated */

  wDumpRow.clear();
  wDumpRow << createItem(wSP,"%lu");
  wDumpRow << createItem( "DictionaryPath");
  wDumpRow.last()->setToolTip("Optional dictionary path");
  wDumpRow << createItem( wDictionaryName.toCChar());
  wDumpRow <<  createItem( wDictionaryName.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("%4lu 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wOffset += wSP;

  wSP=wIndexFilePath._importUVF(wPtr); /* wPtr is updated */

  wDumpRow.clear();
  wDumpRow << createItem(wSP,"%lu");
  wDumpRow << createItem( "IndexFilePath");
  wDumpRow.last()->setToolTip("Optional index file path");
  wDumpRow << createItem( wIndexFilePath.toCChar());
  wDumpRow <<  createItem( wIndexFilePath.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("%4lu 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wOffset += wSP;

  wDumpRow.clear();
  wDumpRow << createItem(" ");
  wDumpRow << createItem(" ");
  wDumpRow << createItem( "---end---");
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(wOffset,displayItemModel->rowCount()-1);
  wRow++;

  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();

  return;
}

void
DisplayMain::displayFCBValues(unsigned char *pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String wStr;
  int wOffset=0;

  setWindowTitle(QObject::tr("File control block","DisplayMain"));
//  ui->ZEntityLBl->setText("ZFileControlBlock");
/*
  ContentToDump.setData(pPtrIn,sizeof(ZFCB_Export));
  ZFCB_Export* wFCBExport=(ZFCB_Export*)ContentToDump.Data;
*/
  ZFCB_Export* wFCBExport=(ZFCB_Export*)pPtrIn;


  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());


  int wRow=0;
  QList<QStandardItem *> wDumpRow ;

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->StartSign),"%ld");
  wDumpRow << createItem( "StartSign");
  wDumpRow.last()->setToolTip("Special value that marks the beginning of the block");
  wDumpRow << createItem(wFCBExport->StartSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wFCBExport->StartSign),"0x%08X");
  wStr.sprintf("%s",wFCBExport->StartSign==cst_ZBLOCKSTART?"correct":"incorrect");
  wDumpRow << createItem(wStr.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,StartSign);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->BlockId),"%ld");
  wDumpRow << createItem( "BlockId");
  wDumpRow.last()->setToolTip("Block identification");
  wDumpRow << createItem((uint8_t)wFCBExport->BlockId,"0x%02X");
  wDumpRow << createItem((uint8_t)wFCBExport->BlockId,"0x%02X");
  wDumpRow << createItem(decode_BlockId(wFCBExport->BlockId));
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,BlockId);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->EndianCheck),"%ld");
  wDumpRow << createItem( "EndianCheck");
  wDumpRow.last()->setToolTip("Controls if values have been serialized or not");
  wStr.sprintf("0x%04X",wFCBExport->EndianCheck);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%d",reverseByteOrder_Conditional(wFCBExport->EndianCheck));
  wDumpRow <<  createItem( wStr.toCChar());

  wStr.sprintf("%s",wFCBExport->isReversed()?"serialized":"NOT serialized");
  wDumpRow <<  createItem(wStr.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,EndianCheck);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->StartOfData),"%ld");
  wDumpRow << createItem( "StartOfData");
  wDumpRow.last()->setToolTip("Offset to first data block within content file-");
  wStr.sprintf("0x%lX",wFCBExport->StartOfData);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%d",reverseByteOrder_Conditional<zaddress_type>(wFCBExport->StartOfData));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,StartOfData);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->AllocatedBlocks),"%ld");
  wDumpRow << createItem( "AllocatedBlocks");
  wDumpRow.last()->setToolTip("Number of currently allocated blocks");
  wStr.sprintf("0x%lX",wFCBExport->AllocatedBlocks);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<unsigned long>(wFCBExport->AllocatedBlocks));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,AllocatedBlocks);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->BlockExtentQuota),"%ld");
  wDumpRow << createItem( "BlockExtentQuota");
  wDumpRow.last()->setToolTip("Quota of byte with which block will be extended during extend operation");
  wStr.sprintf("0x%lX",wFCBExport->BlockExtentQuota);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<unsigned long>(wFCBExport->BlockExtentQuota));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,BlockExtentQuota);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->ZBAT_DataOffset),"%ld");
  wDumpRow << createItem( "ZBAT_DataOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to Block Access Table since begining of File control block");
  wStr.sprintf("0x%lX",wFCBExport->ZBAT_DataOffset);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->ZBAT_DataOffset));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,ZBAT_DataOffset);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->ZBAT_ExportSize),"%ld");
  wDumpRow << createItem( "ZBAT_ExportSize");
  wDumpRow.last()->setToolTip("Size in bytes of exported Block Access Table");
  wStr.sprintf("0x%lX",wFCBExport->ZBAT_ExportSize);

  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->ZBAT_ExportSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,ZBAT_ExportSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->ZFBT_DataOffset),"%ld");
  wDumpRow << createItem( "ZFBT_DataOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to Free Blocks Table since beginning of File Control Block");
  wStr.sprintf("0x%lX",wFCBExport->ZFBT_DataOffset);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->ZFBT_DataOffset));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,ZFBT_DataOffset);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->ZFBT_ExportSize),"%ld");
  wDumpRow << createItem( "ZFBT_ExportSize");
  wDumpRow.last()->setToolTip("Size in bytes of exported Free Blocks Table");
  wStr.sprintf("0x%lX",wFCBExport->ZFBT_ExportSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->ZFBT_ExportSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,ZFBT_ExportSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

/* Deprecated  - ZDBT removed
  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->ZDBT_DataOffset),"%ld");
  wDumpRow << createItem( "ZDBT_DataOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to Deleted Blocks Table since beginning of File Control Block");
  wStr.sprintf("0x%lX",wFCBExport->ZDBT_DataOffset);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->ZDBT_DataOffset));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,ZDBT_DataOffset);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->ZDBT_ExportSize),"%ld");
  wDumpRow << createItem( "ZDBT_ExportSize");
  wDumpRow.last()->setToolTip("Size in bytes of Deleted Blocks Table");
  wStr.sprintf("0x%lX",wFCBExport->ZDBT_ExportSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->ZDBT_ExportSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,ZDBT_ExportSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));
*/

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->InitialSize),"%ld");
  wDumpRow << createItem( "InitialSize");
  wDumpRow.last()->setToolTip("Initial size in bytes allocated to file during creation");
  wStr.sprintf("0x%lX",wFCBExport->InitialSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->InitialSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,InitialSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->AllocatedSize),"%ld");
  wDumpRow << createItem( "AllocatedSize");
  wDumpRow.last()->setToolTip("Total current allocated size in bytes for file");
  wStr.sprintf("0x%lX",wFCBExport->AllocatedSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->AllocatedSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,AllocatedSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->UsedSize),"%ld");
  wDumpRow << createItem( "UsedSize");
  wDumpRow.last()->setToolTip(" Total of currently used size within file in bytes ");
  wStr.sprintf("0x%lX",wFCBExport->UsedSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->UsedSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,UsedSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->MinSize),"%ld");
  wDumpRow << createItem( "MinSize");
  wDumpRow.last()->setToolTip("Statistical value : minimum length of block record in file  (existing statistic) ");
  wStr.sprintf("0x%lX",wFCBExport->MinSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->MinSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,MinSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->MaxSize),"%ld");
  wDumpRow << createItem( "MaxSize");
  wDumpRow.last()->setToolTip("Statistical value : maximum length of block record in file (existing statistic) ");
  wStr.sprintf("0x%lX",wFCBExport->MaxSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->MaxSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,MaxSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->BlockTargetSize),"%ld");
  wDumpRow << createItem( "BlockTargetSize");
  wDumpRow.last()->setToolTip("(user defined value) Foreseen average size of blocks (records) in a varying context.");
  wStr.sprintf("0x%lX",wFCBExport->BlockTargetSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->BlockTargetSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,BlockTargetSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->HighwaterMarking),"%ld");
  wDumpRow << createItem( "HighwaterMarking");
  wDumpRow.last()->setToolTip("(Option)If set, this option marks to zero the whole deleted block content when removed.");
  wStr.sprintf("0x%X",wFCBExport->HighwaterMarking);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%s",wFCBExport->HighwaterMarking?"Option set":"Option not set");
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,HighwaterMarking);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->GrabFreeSpace),"%ld");
  wDumpRow << createItem( "GrabFreeSpace");
  wDumpRow.last()->setToolTip("(Option) If set, attempts to grab free space and holes at each block free operation.");
  wStr.sprintf("0x%X",wFCBExport->GrabFreeSpace);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%s",wFCBExport->GrabFreeSpace?"Option set":"Option not set");
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,GrabFreeSpace);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->EndSign),"%ld");
  wDumpRow << createItem( "EndSign");
  wDumpRow.last()->setToolTip("Special value that mark the end of the block");
  wDumpRow << createItem(wFCBExport->EndSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wFCBExport->EndSign),"0x%08X");
  wStr.sprintf("%s",wFCBExport->EndSign==cst_ZBLOCKEND?"correct":"incorrect");
  wDumpRow << createItem(wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,EndSign);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wOffset += sizeof(wFCBExport->EndSign);

  wDumpRow.clear();
  wDumpRow << createItem(" ");
  wDumpRow << createItem(" ");
  wDumpRow << createItem( "---end---");
  displayItemModel->appendRow(wDumpRow);

  createMarginItem(wOffset,displayItemModel->rowCount()-1);
  wRow++;



  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();
}



void
DisplayMain::clear()
{
  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());

//  ui->OffsetSLd->setValue(0);
  ui->offsetSBx->setValue(0);
  ui->OffsetLBl->setText("0");
}



DisplayMain::~DisplayMain()
{
  delete ui;
}



void DisplayMain::resizeEvent(QResizeEvent* pEvent)
{
  QSize wGOldSize = pEvent->oldSize();

  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (FResizeInitial) {
    FResizeInitial=false;
    return;
  }
  QRect wCWg = ui->centralwidget->geometry();

  int wWMargin = wGOldSize.width()- wCWg.width();
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wGOldSize.height() - wCWg.height();
  int wVH=pEvent->size().height() - wHMargin ;

  ui->centralwidget->resize(wVW,wVH);  /* expands in width and height */

  QRect wOldTBv = ui->displayTBv->geometry();

  int wTBwMargin = wCWg.width()- wOldTBv.width();
  int wTBvWidth= wVW - wTBwMargin;
  int wTBvHMargin = wCWg.height() - wOldTBv.height();
  int wTBvHeight= wVH - wTBvHMargin ;

  ui->displayTBv->resize(wTBvWidth,wTBvHeight);  /* expands in width and height */

}//ZContentVisuMain::resizeEvent
