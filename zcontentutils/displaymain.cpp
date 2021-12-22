#include "displaymain.h"
#include "ui_displaymain.h"
#include <QStandardItemModel>

#include <qmessagebox.h>

#include <zrandomfile/zheadercontrolblock.h>
#include <zrandomfile/zfilecontrolblock.h>
#include <zindexedfile/zsmastercontrolblock.h>



DisplayMain::DisplayMain(QWidget *parent) :
                                            QMainWindow(parent),
                                            ui(new Ui::DisplayMain)
{
  ui->setupUi(this);

  setWindowTitle("Entity display");

/*  ui->displayTBv->setStyleSheet(QString::fromUtf8("QTableView::item{border-left : 1px solid black;\n"
                                                "border-right  : 1px solid black;\n"
                                                "font: 75 12pt \"Courier\";\n"
                                                " }"));
*/
  displayItemModel=new QStandardItemModel(0,5,this) ;/* 4 columns */

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

void
DisplayMain::displayHCB(ZDataBuffer &pData)
{
  utf8String wStr;
  PtrIn=pData.Data;
  if (pData.Size < sizeof(ZHeaderControlBlock_Export))
  {

    wStr.sprintf("Cannot view Header Control Block requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",sizeof(ZHeaderControlBlock_Export),pData.Size);
    QMessageBox::critical(this,tr("Data structure"),wStr.toCChar());
    return;
  }
  CurrentDisplay= ZDW_HCB;
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
    utf8String wStr;
    wStr.sprintf("Cannot view Header Control Block requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",sizeof(ZHeaderControlBlock_Export),pData.Size);
    QMessageBox::critical(this,tr("Data structure"),wStr.toCChar());
    return;
  }

  CurrentDisplay= ZDW_FCB;

  ZHeaderControlBlock_Export* wHCBExport=(ZHeaderControlBlock_Export*)pData.Data;
  zaddress_type wOffset=reverseByteOrder_Conditional(wHCBExport->OffsetFCB);
  if (pData.Size < (wOffset+sizeof(ZFCB_Export)))
  {
    utf8String wStr;
    wStr.sprintf("Cannot view File Control Block : requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",(wOffset+sizeof(ZFCB_Export)),pData.Size);
    QMessageBox::critical(this,tr("Data structure"),wStr.toCChar());
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
DisplayMain::displayMCB(ZDataBuffer& pData)
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
    QMessageBox::critical(this,tr("Data structure"),wStr.toCChar());
    return;
  }

  CurrentDisplay= ZDW_MCB;

  ZHeaderControlBlock_Export* wHCBExport=(ZHeaderControlBlock_Export*)pData.Data;
  zaddress_type wOffset=reverseByteOrder_Conditional(wHCBExport->OffsetReserved);
  if (pData.Size < (wOffset+sizeof(ZSMCBOwnData_Export)))
    {
    utf8String wStr;
    wStr.sprintf("Cannot view File Control Block : requested minimum size <%ld> Bytes \n"
                 "Only <%ld> Bytes have been loaded.",(wOffset+sizeof(ZSMCBOwnData_Export)),pData.Size);
    QMessageBox::critical(this,tr("Data structure"),wStr.toCChar());
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



const char*
decode_BlockId(ZBlockID pBID)
{
  switch (pBID)
  {
  case ZBID_Nothing:
    return "ZBID_Nothing";
  case ZBID_FileHeader:
    return "ZBID_FileHeader";
  case ZBID_FCB:
    return "ZBID_FCB";
  case ZBID_MCB:
    return "ZBID_MCB";
  case ZBID_ICB:
    return "ZBID_ICB";
  case ZBID_JCB:
    return "ZBID_JCB";
  case ZBID_Data:
    return "ZBID_Data";
  case ZBID_Index:
    return "ZBID_Index";
  case ZBID_Master:
    return "ZBID_Master";
  case ZBID_MDIC:
    return "ZBID_MDIC";
  default:
    return "Unknown Block id";
  }
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

Qt::AlignmentFlag QtAlignmentFlag=Qt::AlignRight;
void setItemAlignment(const Qt::AlignmentFlag pFlag)
{
  QtAlignmentFlag = pFlag;
}

QStandardItem* createItem(const utf8String& pValue,const char*pFormat=nullptr)
{
  QStandardItem* wCurItem=nullptr;
  if (pFormat==nullptr)
      wCurItem=new QStandardItem( pValue.toCChar());
  else
  {
    utf8String wStr;
    wStr.sprintf(pFormat,pValue.toCChar());
    wCurItem=new QStandardItem( wStr.toCChar());
  }
  wCurItem->setTextAlignment(QtAlignmentFlag);
  return wCurItem;
}

QStandardItem* createItem(uint8_t pValue,const char*pFormat=nullptr)
{
  if (pFormat==nullptr)
    pFormat="%c";
  utf8String wStr;
  wStr.sprintf(pFormat,pValue);
  QStandardItem* wCurItem=new QStandardItem( wStr.toCChar());
  wCurItem->setTextAlignment(QtAlignmentFlag);
  return wCurItem;
}
QStandardItem* createItem(uint32_t pValue,const char*pFormat=nullptr)
{
  if (pFormat==nullptr)
    pFormat="%u";
  utf8String wStr;
  wStr.sprintf(pFormat,pValue);
  QStandardItem* wCurItem=new QStandardItem( wStr.toCChar());
  wCurItem->setTextAlignment(QtAlignmentFlag);
  return wCurItem;
}
QStandardItem* createItem(int32_t pValue,const char*pFormat=nullptr)
{
  if (pFormat==nullptr)
    pFormat="%d";
  utf8String wStr;
  wStr.sprintf(pFormat,pValue);
  QStandardItem* wCurItem=new QStandardItem( wStr.toCChar());
  wCurItem->setTextAlignment(QtAlignmentFlag);
  return wCurItem;
}
QStandardItem* createItem(unsigned long pValue,const char*pFormat=nullptr)
{
  if (pFormat==nullptr)
    pFormat="%lu";
  utf8String wStr;
  wStr.sprintf(pFormat,pValue);
  QStandardItem* wCurItem=new QStandardItem( wStr.toCChar());
  wCurItem->setTextAlignment(QtAlignmentFlag);
  return wCurItem;
}
QStandardItem* createItem(long pValue,const char*pFormat=nullptr)
{
  if (pFormat==nullptr)
    pFormat="%ld";
  utf8String wStr;
  wStr.sprintf(pFormat,pValue);
  QStandardItem* wCurItem=new QStandardItem( wStr.toCChar());
  wCurItem->setTextAlignment(QtAlignmentFlag);
  return wCurItem;
}

void
DisplayMain::displayHCBValues(unsigned char *pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String wStr;
  zaddress_type wAddress;
  zaddress_type wOffsetReserved;
  zsize_type wSize;
  zsize_type wSizeReserved;
  int wOffset=Offset;

  int wRowOffsetFCB=0;
  ui->ZEntityLBl->setText("ZHeaderControlBlock");

  ContentToDump.setData(pPtrIn,sizeof(ZHeaderControlBlock_Export));
  utf8String wFieldContent;
  ZHeaderControlBlock_Export* wHCBExport=nullptr;



  wHCBExport=(ZHeaderControlBlock_Export*)ContentToDump.Data;

  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());


  setItemAlignment(Qt::AlignRight);

  int wRow=0;
  QList<QStandardItem *> wDumpRow ;
//  QStandardItem* wCurItem=nullptr;

  wDumpRow.clear();

  wDumpRow << createItem(sizeof(wHCBExport->StartSign),"%ld");
  wDumpRow << createItem( "StartSign");
  wDumpRow << createItem(wHCBExport->StartSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wHCBExport->StartSign),"0x%08X");
  wDumpRow <<  new QStandardItem("");
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,StartSign);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->BlockID),"%ld");
  wDumpRow << createItem( "BlockID");
  wDumpRow << createItem((uint8_t)wHCBExport->BlockID,"0x%02X");
  wDumpRow << createItem((uint8_t)wHCBExport->BlockID,"0x%02X");
  wDumpRow << createItem(decode_BlockId(wHCBExport->BlockID));
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,BlockID);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->ZRFVersion),"%ld");
  wDumpRow << createItem( "ZRFVersion");
  wDumpRow << createItem(wHCBExport->ZRFVersion,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<unsigned long>(wHCBExport->ZRFVersion),"%lu");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,ZRFVersion);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->FileType),"%ld");
  wDumpRow << createItem( "FileType");
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
  wDumpRow << createItem(wHCBExport->LockOwner,"0x%X");
  wDumpRow << createItem(reverseByteOrder_Conditional<pid_t>(wHCBExport->LockOwner),"0x%X");
  wFieldContent.sprintf("current: 0x%lX",getpid());
  wDumpRow << createItem(wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,LockOwner);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->OffsetFCB),"%ld");
  wDumpRow << createItem( "OffsetFCB");
  wDumpRow << createItem(wHCBExport->OffsetFCB,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<zaddress_type>(wHCBExport->OffsetFCB),"%ld");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,OffsetFCB);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->OffsetReserved),"%ld");
  wDumpRow << createItem( "OffsetReserved");
  wDumpRow << createItem(wHCBExport->OffsetReserved,"0x%lX");
  wDumpRow << createItem(reverseByteOrder_Conditional<zaddress_type>(wHCBExport->OffsetReserved),"%ld");

  displayItemModel->appendRow(wDumpRow);

  wOffsetReserved=Offset+offsetof(ZHeaderControlBlock_Export,OffsetReserved);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << createItem(sizeof(wHCBExport->SizeReserved),"%ld");
  wDumpRow << createItem( "SizeReserved");
  wDumpRow << createItem(wHCBExport->SizeReserved,"0x%lX");
  wSizeReserved=reverseByteOrder_Conditional<zaddress_type>(wHCBExport->SizeReserved);
  wDumpRow << createItem(wSizeReserved,"%ld");

  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,SizeReserved);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));


  /* update theorical offsetFCB value */
  wStr.sprintf("0x%X - %ld ",wSizeReserved+wOffsetReserved,wSizeReserved+wOffsetReserved);
  displayItemModel->setItem(wRowOffsetFCB,4,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();

  wDumpRow << createItem(sizeof(wHCBExport->EndSign),"%ld");
  wDumpRow << createItem( "EndSign");
  wDumpRow << createItem(wHCBExport->EndSign,"0x%08X");
  wDumpRow << createItem(reverseByteOrder_Conditional( wHCBExport->EndSign),"0x%08X");
  wDumpRow <<  new QStandardItem("");
  displayItemModel->appendRow(wDumpRow);

  wOffset=Offset+offsetof(ZHeaderControlBlock_Export,EndSign);
  wStr.sprintf("%4ld 0x%4lX",wOffset,wOffset);
  displayItemModel->setVerticalHeaderItem(wRow++,createItem(wStr.toCChar()));

  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();
}

#define _DISPLAY_UINT32_(__NAME__)   wDumpRow.clear(); \
wDumpRow << new QStandardItem( ""); \
wDumpRow << new QStandardItem( "__NAME__"); \
wFieldContent.sprintf("%08X",wMCBExport->__NAME__); \
wDumpRow << new QStandardItem( wFieldContent.toCChar()); \
wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->__NAME__)); \
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

  utf8String wStr;

  ui->ZEntityLBl->setText("ZMasterControlBlock");

  ContentToDump.setData(pPtrIn,sizeof(ZSMCBOwnData_Export));
  utf8String wFieldContent;
  ZSMCBOwnData_Export* wMCBExport=nullptr;

  wMCBExport=(ZSMCBOwnData_Export*)ContentToDump.Data;

  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());


  int wRow=0;
  QList<QStandardItem *> wDumpRow ;

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "FCB");
  wDumpRow << new QStandardItem( "StartSign");
  wFieldContent.sprintf("0x%08X",wMCBExport->StartSign);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("0x%08X",reverseByteOrder_Conditional( wMCBExport->StartSign));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem("");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,StartSign));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "BlockID");
  wFieldContent.sprintf("0x%02X",wMCBExport->BlockId);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( decode_BlockId(wMCBExport->BlockId));
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,BlockId));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZMFVersion");
  wFieldContent.sprintf("%08X",wMCBExport->ZMFVersion);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->ZMFVersion));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,ZMFVersion));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "MCBSize");
  wFieldContent.sprintf("%08X",wMCBExport->MCBSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->MCBSize));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,MCBSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "IndexCount");
  wFieldContent.sprintf("%08X",wMCBExport->IndexCount);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->IndexCount));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,IndexCount));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ICBOffset");
  wFieldContent.sprintf("%08X",wMCBExport->ICBOffset);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->ICBOffset));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,ICBOffset));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ICBSize");
  wFieldContent.sprintf("%08X",wMCBExport->ICBSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->ICBSize));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,ICBSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "JCBOffset");
  wFieldContent.sprintf("%08X",wMCBExport->JCBOffset);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->JCBOffset));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,JCBOffset));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "JCBSize");
  wFieldContent.sprintf("%08X",wMCBExport->JCBSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->JCBSize));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,JCBSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "MDicOffset");
  wFieldContent.sprintf("%08X",wMCBExport->MDicOffset);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->MDicOffset));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,MDicOffset));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "MDicSize");
  wFieldContent.sprintf("%08X",wMCBExport->MDicSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("%ld",reverseByteOrder_Conditional(wMCBExport->MDicSize));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( "");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,MDicSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "HistoryOn");
  wFieldContent.sprintf("%s",wMCBExport->HistoryOn?"Yes":"No");
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZSMCBOwnData_Export,HistoryOn));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  utf8String wIndexFilePath;
  unsigned char* wPtrIn=pPtrIn+sizeof(ZSMCBOwnData_Export);
  wIndexFilePath._importUVF(wPtrIn);

  if (wIndexFilePath.UnitCount > 20)
    wIndexFilePath.Right(25);
  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "IndexFilePath");
  wDumpRow << new QStandardItem( wIndexFilePath.toCChar());
  wDumpRow <<  new QStandardItem( wIndexFilePath.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+sizeof(ZSMCBOwnData_Export));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));


  ui->displayTBv->resizeRowsToContents();
  ui->displayTBv->resizeColumnsToContents();
}

void
DisplayMain::displayFCBValues(unsigned char *pPtrIn)
{
  if (pPtrIn==nullptr)
    return;

  utf8String wStr;

  ui->ZEntityLBl->setText("ZFileControlBlock");

  ContentToDump.setData(pPtrIn,sizeof(ZFCB_Export));
  utf8String wFieldContent;
  ZFCB_Export* wFCBExport=nullptr;



  wFCBExport=(ZFCB_Export*)ContentToDump.Data;

  if (displayItemModel)
    if (displayItemModel->rowCount()>0)
      displayItemModel->removeRows(0,displayItemModel->rowCount());


  int wRow=0;
  QList<QStandardItem *> wDumpRow ;

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "FCB");
  wDumpRow << new QStandardItem( "StartSign");
  wFieldContent.sprintf("0x%08X",wFCBExport->StartSign);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("0x%08X",reverseByteOrder_Conditional( wFCBExport->StartSign));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem("");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,StartSign));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "BlockID");
  wFieldContent.sprintf("0x%02X",wFCBExport->BlockID);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( decode_BlockId(wFCBExport->BlockID));
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,BlockID));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "StartOfData");
  wFieldContent.sprintf("0x%08X",wFCBExport->StartOfData);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  zaddress_type wAddress=reverseByteOrder_Conditional<zaddress_type>(wFCBExport->StartOfData);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);


  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "AllocatedBlocks");
  wFieldContent.sprintf("0x%08X",wFCBExport->AllocatedBlocks);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<unsigned long>(wFCBExport->AllocatedBlocks);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,AllocatedBlocks));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "BlockExtentQuota");
  wFieldContent.sprintf("0x%08X",wFCBExport->BlockExtentQuota);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<unsigned long>(wFCBExport->BlockExtentQuota);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,BlockExtentQuota));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZBAT_DataOffset");
  wFieldContent.sprintf("0x%08X",wFCBExport->ZBAT_DataOffset);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->ZBAT_DataOffset);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,ZBAT_DataOffset));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZBAT_ExportSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->ZBAT_ExportSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->ZBAT_ExportSize);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,ZBAT_ExportSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZFBT_DataOffset");
  wFieldContent.sprintf("0x%08X",wFCBExport->ZFBT_DataOffset);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->ZFBT_DataOffset);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,ZFBT_DataOffset));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZFBT_ExportSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->ZFBT_ExportSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->ZFBT_ExportSize);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,ZFBT_ExportSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZDBT_DataOffset");
  wFieldContent.sprintf("0x%08X",wFCBExport->ZDBT_DataOffset);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->ZDBT_DataOffset);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,ZDBT_DataOffset));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZDBT_ExportSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->ZDBT_ExportSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->ZDBT_ExportSize);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,ZDBT_ExportSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZReserved_DataOffset");
  wFieldContent.sprintf("0x%08X",wFCBExport->ZReserved_DataOffset);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->ZReserved_DataOffset);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,ZReserved_DataOffset));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "ZReserved_ExportSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->ZReserved_ExportSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->ZReserved_ExportSize);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,ZReserved_ExportSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "InitialSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->InitialSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  zsize_type wSize=reverseByteOrder_Conditional<zsize_type>(wFCBExport->InitialSize);
  wFieldContent.sprintf("%ld",wSize);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,InitialSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "AllocatedSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->AllocatedSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wSize=reverseByteOrder_Conditional<zsize_type>(wFCBExport->AllocatedSize);
  wFieldContent.sprintf("%ld",wSize);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,AllocatedSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "UsedSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->UsedSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wSize=reverseByteOrder_Conditional<zsize_type>(wFCBExport->UsedSize);
  wFieldContent.sprintf("%ld",wSize);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,UsedSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));


  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "MinSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->MinSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->MinSize);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,MinSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "MaxSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->MaxSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->MaxSize);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,MaxSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "BlockTargetSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->BlockTargetSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->BlockTargetSize);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,BlockTargetSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "BlockTargetSize");
  wFieldContent.sprintf("0x%08X",wFCBExport->BlockTargetSize);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wAddress=reverseByteOrder_Conditional<size_t>(wFCBExport->BlockTargetSize);
  wFieldContent.sprintf("%ld",wAddress);
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,BlockTargetSize));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "HighwaterMarking");
   wFieldContent.sprintf("%s",wFCBExport->HighwaterMarking?"Yes":"No");
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,HighwaterMarking));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "GrabFreeSpace");
  wFieldContent.sprintf("%s",wFCBExport->GrabFreeSpace?"Yes":"No");
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,GrabFreeSpace));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

  wDumpRow.clear();
  wDumpRow << new QStandardItem( "");
  wDumpRow << new QStandardItem( "EndSign");
  wFieldContent.sprintf("0x%08X",wFCBExport->EndSign);
  wDumpRow << new QStandardItem( wFieldContent.toCChar());
  wFieldContent.sprintf("0x%08X",reverseByteOrder_Conditional( wFCBExport->EndSign));
  wDumpRow <<  new QStandardItem( wFieldContent.toCChar());
  wDumpRow <<  new QStandardItem("");
  displayItemModel->appendRow(wDumpRow);

  wStr.sprintf("0x%8X",Offset+offsetof(ZFCB_Export,EndSign));
  displayItemModel->setVerticalHeaderItem(wRow++,new QStandardItem(wStr.toCChar()));

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
