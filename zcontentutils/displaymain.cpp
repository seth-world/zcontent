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

  QObject::connect(ui->OffsetSLd, SIGNAL(valueChanged(int)), this, SLOT(sliderChange(int)));
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
  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pData.Size);
  ui->OffsetSLd->setValue (0);
  ui->OffsetSLd->setSingleStep(1);

  displayHCBValues(pData.Data);

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

  Offset=wOffset;
  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pData.Size);
  ui->OffsetSLd->setValue (Offset);


  wStr.sprintf("%4ld - Ox%4lX",Offset,Offset);
  ui->OffsetLBl->setText(wStr.toCChar());

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
  if (pData.Size < (wOffset+sizeof(ZSMCBOwnData_Export)))
  {
    utf8String wStr;
    wStr.sprintf("Not enough loaded data. Requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",(wOffset+sizeof(ZSMCBOwnData_Export)),pData.Size);
    ZExceptionDLg::message("DisplayMain::getMCB",ZS_NEEDMORESPACE,Severity_Error,wStr.toCChar());
    return;
  }
  Offset=wOffset;
  wStr.sprintf("%4ld - Ox%4lX",Offset,Offset);
  ui->OffsetLBl->setText(wStr.toCChar());

  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pData.Size);
  ui->OffsetSLd->setValue (Offset);

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
  if (pData.Size < (wOffset+sizeof(ZSMCBOwnData_Export)))
    {
    utf8String wStr;
    wStr.sprintf("Not enough loaded data. Requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",(wOffset+sizeof(ZSMCBOwnData_Export)),pData.Size);
    ZExceptionDLg::message("DisplayMain::displayMCB",ZS_NEEDMORESPACE,Severity_Error,wStr.toCChar());
    return;
    }

  CurrentDisplay= ZDW_MCB;
  ContentToDump.setData(pData);

  Offset=wOffset;
  wStr.sprintf("%4ld - Ox%4lX",Offset,Offset);
  ui->OffsetLBl->setText(wStr.toCChar());

  ui->OffsetSLd->setMinimum(0);
  ui->OffsetSLd->setMaximum(pData.Size);
  ui->OffsetSLd->setValue (Offset);

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
  zaddress_type wOffsetReserved;
  int64_t       wInt64;
  long wOffset=Offset;

  int wRowOffsetFCB=0;
  ui->ZEntityLBl->setText("ZHeaderControlBlock");
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

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,StartSign);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->BlockID),"%ld");
  wDumpRow << createItem( "BlockID");
  wDumpRow.last()->setToolTip("Block identification");
  wDumpRow << createItem((uint8_t)wHCBExport->BlockID,"0x%02X");
  wDumpRow << createItem((uint8_t)wHCBExport->BlockID,"0x%02X");
  wDumpRow << createItem(decode_BlockId(wHCBExport->BlockID));
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,BlockID);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

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

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,EndianCheck);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->ZRFVersion),"%ld");
  wDumpRow << createItem( "ZRFVersion");
  wDumpRow.last()->setToolTip("Random file full software version expressed as an unsigned long");
  wDumpRow << createItem(wHCBExport->ZRFVersion,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<unsigned long>(wHCBExport->ZRFVersion),"%lu");
  unsigned long wVersion = reverseByteOrder_Conditional<unsigned long>(wHCBExport->ZRFVersion);
  wDumpRow << createItem(getVersionStr(wVersion).toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,ZRFVersion);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->FileType),"%ld");
  wDumpRow << createItem( "FileType");
  wDumpRow.last()->setToolTip("Type of file this header relates to : could be Random - Master - Raw Master - Index ");
  wDumpRow << createItem((uint8_t)wHCBExport->FileType,"0x%02X");
  wDumpRow << createItem((uint8_t)wHCBExport->FileType,"0x%02X");
  wDumpRow << createItem(decode_ZFile_type(wHCBExport->FileType));
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,FileType);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->Lock),"%ld");
  wDumpRow << createItem( "Lock");
  wDumpRow.last()->setToolTip("Lock mask file is currently locked with ");
  wDumpRow << createItem((uint8_t)wHCBExport->Lock,"0x%02X");
  wDumpRow << createItem((uint8_t)wHCBExport->Lock,"0x%02X");
  wDumpRow << createItem(decode_ZLockMask(wHCBExport->Lock).toChar());
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,Lock);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->LockOwner),"%ld");
  wDumpRow << createItem( "LockOwner");
  wDumpRow.last()->setToolTip("System id of user currently locking the file");
  wDumpRow << createItem(wHCBExport->LockOwner,"0x%X");
  wDumpRow << createItem(reverseByteOrder_Conditional<pid_t>(wHCBExport->LockOwner),"0x%X");
  wStr.sprintf("current: 0x%lX",getpid());
  wDumpRow << createItem(wStr.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,LockOwner);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->OffsetFCB),"%ld");
  wDumpRow << createItem( "OffsetFCB");
  wDumpRow.last()->setToolTip("Offset in bytes to File Control Block since begining of Header");
  wDumpRow << createItem(wHCBExport->OffsetFCB,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<zaddress_type>(wHCBExport->OffsetFCB),"%ld");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,OffsetFCB);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->OffsetReserved),"%ld");
  wDumpRow << createItem( "OffsetReserved");
  wDumpRow.last()->setToolTip("Offset in bytes to Reserved Section (may contain MCB for instance) since begining of Header");
  wDumpRow << createItem(wHCBExport->OffsetReserved,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<zaddress_type>(wHCBExport->OffsetReserved),"%ld");

  displayItemModel->appendRow(wDumpRow);

  wOffsetReserved=Offset+offsetof(ZHeaderControlBlock_Export,OffsetReserved);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->SizeReserved),"%ld");
  wDumpRow << createItem( "SizeReserved");
  wDumpRow.last()->setToolTip("Size in bytes of exported Reserved Section (may contain MCB for instance)");
  wDumpRow << createItem(wHCBExport->SizeReserved,"0x%lX");
  wInt64=reverseByteOrder_Conditional<zaddress_type>(wHCBExport->SizeReserved);
  wDumpRow << createItem(wInt64,"%lld");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,SizeReserved);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


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

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,EndSign);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

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

  ui->ZEntityLBl->setText("ZPool Values");
}


void
DisplayMain::displayMCBValues(unsigned char* pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String    wStr;
  int           wOffset=Offset;
  int64_t       wInt64;
  uint32_t      wUInt32;

  ui->ZEntityLBl->setText("ZMasterControlBlock");
/*
  ContentToDump.setData(pPtrIn,sizeof(ZSMCBOwnData_Export));
  ZSMCBOwnData_Export* wMCBExport=(ZSMCBOwnData_Export*)ContentToDump.Data;
*/
  ZSMCBOwnData_Export* wMCBExport=(ZSMCBOwnData_Export*)pPtrIn;


  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());


  int wRow=0;
  QList<QStandardItem *> wDumpRow ;


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->StartSign),"%ld");
  wDumpRow << createItem( "StartSign");
  wDumpRow.last()->setToolTip("Special value that marks the beginning of the block");
  wDumpRow << createItem(wMCBExport->StartSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wMCBExport->StartSign),"0x%08X");
  wStr.sprintf("%s",wMCBExport->StartSign==cst_ZBLOCKSTART?"correct":"incorrect");
  wDumpRow << createItem(wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,StartSign);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->BlockId),"%ld");
  wDumpRow << createItem( "BlockID");
  wDumpRow.last()->setToolTip("Block identification");
  wDumpRow << createItem((uint8_t)wMCBExport->BlockId,"0x%02X");
  wDumpRow << createItem((uint8_t)wMCBExport->BlockId,"0x%02X");
  wDumpRow << createItem(decode_BlockId(wMCBExport->BlockId));
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,BlockID);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->EndianCheck),"%ld");
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
  wDumpRow << createItem(sizeof(wMCBExport->ZMFVersion),"%ld");
  wDumpRow << createItem( "ZMFVersion");
  wDumpRow.last()->setToolTip("Master file full software version expressed as an unsigned long");
  wDumpRow << createItem(wMCBExport->ZMFVersion,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<unsigned long>(wMCBExport->ZMFVersion),"%lu");
  unsigned long wVersion = reverseByteOrder_Conditional<unsigned long>(wMCBExport->ZMFVersion);
  wDumpRow << createItem(getVersionStr(wVersion).toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZSMCBOwnData_Export,ZMFVersion);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->MCBSize),"%ld");
  wDumpRow << createItem( "MCBSize");
  wDumpRow.last()->setToolTip("Size in bytes of exported Master Control Block");
  wDumpRow << createItem(wMCBExport->MCBSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->MCBSize);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZSMCBOwnData_Export,MCBSize);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->IndexCount),"%ld");
  wDumpRow << createItem( "IndexCount");
  wDumpRow.last()->setToolTip("Number of indexes (keys) contained in Master Control Block");
  wDumpRow << createItem(wMCBExport->IndexCount,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->IndexCount);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZSMCBOwnData_Export,IndexCount);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->ICBOffset),"%ld");
  wDumpRow << createItem( "ICBOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to first Index Control Block since beginning of Master Control Block");
  wDumpRow << createItem(wMCBExport->ICBOffset,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->ICBOffset);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=int(Offset+offsetof(ZSMCBOwnData_Export,ICBOffset));
  wStr.sprintf("%4ld 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->ICBSize),"%ld");
  wDumpRow << createItem( "ICBOffset");
  wDumpRow.last()->setToolTip("Size in bytes of Index Control Blocks table");
  wDumpRow << createItem(wMCBExport->ICBSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->ICBSize);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=int(Offset+offsetof(ZSMCBOwnData_Export,ICBSize));
  wStr.sprintf("%4ld 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->JCBOffset),"%ld");
  wDumpRow << createItem( "JCBOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to Journal Control Block since beginning of Master Control Block");
  wDumpRow << createItem(wMCBExport->JCBOffset,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->JCBOffset);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=int(Offset+offsetof(ZSMCBOwnData_Export,JCBOffset));
  wStr.sprintf("%4ld 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->JCBSize),"%ld");
  wDumpRow << createItem( "JCBSize");
  wDumpRow.last()->setToolTip("Size in bytes of Journal Control Block");
  wDumpRow << createItem(wMCBExport->JCBSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->JCBSize);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=int(Offset+offsetof(ZSMCBOwnData_Export,JCBSize));
  wStr.sprintf("%4ld 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->MDicOffset),"%ld");
  wDumpRow << createItem( "MDicOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to serialized Meta Dictionary since beginning of Master Control Block");
  wDumpRow << createItem(wMCBExport->MDicOffset,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->MDicOffset);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=int(Offset+offsetof(ZSMCBOwnData_Export,MDicOffset));
  wStr.sprintf("%4ld 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->MDicSize),"%ld");
  wDumpRow << createItem( "MDicSize");
  wDumpRow.last()->setToolTip("Full size in bytes of serialized Meta Dictionary (including keys)");
  wDumpRow << createItem(wMCBExport->MDicSize,"0x%X");
  wUInt32=reverseByteOrder_Conditional(wMCBExport->MDicSize);
  wDumpRow << createItem(wUInt32,"%d");

  displayItemModel->appendRow(wDumpRow);

  wOffset=int(Offset+offsetof(ZSMCBOwnData_Export,MDicSize));
  wStr.sprintf("%4ld 0x%4X",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wMCBExport->HistoryOn),"%ld");
  wDumpRow << createItem( "HistoryOn");
  wDumpRow.last()->setToolTip("(Option) If set, historize process is on (Reserved for future use).");
  wStr.sprintf("0x%X",wMCBExport->HistoryOn);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%s",wMCBExport->HistoryOn?"Option set":"Option not set");
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZSMCBOwnData_Export,HistoryOn);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  utf8String wIndexFilePath;
  const unsigned char* wPtrIn=pPtrIn+sizeof(ZSMCBOwnData_Export);
  size_t wSP=wIndexFilePath._importUVF(wPtrIn);
  if (wIndexFilePath.UnitCount > 20)
    wIndexFilePath.Right(25);

  wDumpRow.clear();
  wDumpRow << createItem(wSP,"%ld");
  wDumpRow << createItem( "IndexFilePath");
  wDumpRow.last()->setToolTip("Optional path to index files directory");
  wDumpRow << createItem( wIndexFilePath.toCChar());
  wDumpRow <<  createItem( wIndexFilePath.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=int(Offset+sizeof(ZSMCBOwnData_Export));
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();
}

void
DisplayMain::displayFCBValues(unsigned char *pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String wStr;
  int wOffset=0;

  ui->ZEntityLBl->setText("ZFileControlBlock");
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
  wDumpRow << createItem(sizeof(wFCBExport->BlockID),"%ld");
  wDumpRow << createItem( "BlockID");
  wDumpRow.last()->setToolTip("Block identification");
  wDumpRow << createItem((uint8_t)wFCBExport->BlockID,"0x%02X");
  wDumpRow << createItem((uint8_t)wFCBExport->BlockID,"0x%02X");
  wDumpRow << createItem(decode_BlockId(wFCBExport->BlockID));
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,BlockID);
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

/*  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->ZReserved_DataOffset),"%ld");
  wDumpRow << createItem( "ZReserved_DataOffset");
  wDumpRow.last()->setToolTip("Offset in bytes to Reserved Section (may contain MCB for instance) since beginning of File Control Block");
  wStr.sprintf("0x%lX",wFCBExport->ZReserved_DataOffset);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->ZReserved_DataOffset));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,ZReserved_DataOffset);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wFCBExport->ZReserved_ExportSize),"%ld");
  wDumpRow << createItem( "ZReserved_ExportSize");
  wDumpRow.last()->setToolTip("Size in bytes of Reserved Section (may contain MCB for instance)");
  wStr.sprintf("0x%lX",wFCBExport->ZReserved_ExportSize);
  wDumpRow << createItem( wStr.toCChar());
  wStr.sprintf("%ld",reverseByteOrder_Conditional<size_t>(wFCBExport->ZReserved_ExportSize));
  wDumpRow <<  createItem( wStr.toCChar());

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZFCB_Export,ZReserved_ExportSize);
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


  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();
}



void
DisplayMain::clear()
{
  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());

  ui->OffsetSLd->setValue(0);
  ui->OffsetLBl->setText("0");
}



DisplayMain::~DisplayMain()
{
  delete ui;
}


