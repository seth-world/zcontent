#include "zrawmasterfilevisu.h"
#include "ui_zrawmasterfilevisu.h"

#include <zqt/zqtwidget/zqtutils.h> // for  setQLabelNum() template

#include <qnamespace.h>

#include <fcntl.h>

#include <QtWidgets/QApplication>
#include <QPlainTextEdit>
#include <QPushButton>

#include <QStandardItem>
#include <QStandardItemModel>

#include <ztoolset/zlimit.h>
#include <zcontent/zindexedfile/zdataconversion.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <zexceptiondlg.h>

#include <ztoolset/utfstringcommon.h>

#include <zentity.h>

#include <visulinecol.h>

#include <QVBoxLayout>
#include <QLineEdit>
#include <QMenu>

const long StringDiplayMax = 64;
extern const int cst_maxraisonablevalue;

extern const long cst_FileNudge ;

ZRawMasterFileVisu::ZRawMasterFileVisu( QWidget *parent) :
                                                          QDialog(parent),
                                                          ui(new Ui::ZRawMasterFileVisu)
{
  ui->setupUi(this);

  ui->BottomLBl->setVisible(false);
  ui->TopLBl->setVisible(false);


  QFont wVisuFont ("Monospace");
  ui->BitsetHeaderLBl->setFont(wVisuFont);
  ui->BitsetContentLBl->setFont(wVisuFont);


  QPalette palette = ui->WrongColorLBl->palette();

  palette.setColor(ui->WrongColorLBl->backgroundRole(), Qt::magenta);
  ui->WrongColorLBl->setAutoFillBackground(true);
  ui->WrongColorLBl->setPalette(palette);

  ui->WrongColorLBl->setToolTip(QObject::tr("Errored / unknown structure : wrong value","ZRawMasterFileVisu"));

  palette.setColor(ui->StartColorLBl->backgroundRole(), Qt::yellow);
  ui->StartColorLBl->setAutoFillBackground(true);
  ui->StartColorLBl->setPalette(palette);

  ui->StartColorLBl->setToolTip(QObject::tr("Field presence bit set (ZBitset)","ZRawMasterFileVisu"));

  palette.setColor(ui->HeaderColorLBl->backgroundRole(), Qt::green);
  ui->HeaderColorLBl->setAutoFillBackground(true);
  ui->HeaderColorLBl->setPalette(palette);

  ui->HeaderColorLBl->setToolTip(QObject::tr("Block header(class ZBlockHeader_Export)","ZRawMasterFileVisu"));

  palette.setColor(ui->PresenceColorLBl->backgroundRole(), Qt::cyan);
  ui->PresenceColorLBl->setAutoFillBackground(true);
  ui->PresenceColorLBl->setPalette(palette);

  ui->PresenceColorLBl->setToolTip(QObject::tr("Field URF header","ZRawMasterFileVisu"));

  palette.setColor(ui->URFSizeColorLBl->backgroundRole(), Qt::lightGray);
  ui->URFSizeColorLBl->setAutoFillBackground(true);
  ui->URFSizeColorLBl->setPalette(palette);

  ui->URFSizeColorLBl->setToolTip(QObject::tr("Leading URF data size value (uint64_t)","ZRawMasterFileVisu"));


  ui->ColorModeCBx->addItem("Unknown record");
  ui->ColorModeCBx->addItem("Master file record");
  ui->ColorModeCBx->addItem("Key record");

  ui->ColorModeCBx->setCurrentIndex(0);

  ui->ViewModeCBx->addItem("Raw view");
  ui->ViewModeCBx->addItem("URF Fields");

  ui->ViewModeCBx->setCurrentIndex(1);

  QObject::connect(ui->BackwardBTn, SIGNAL(clicked(bool)), this, SLOT(Backward()));
  QObject::connect(ui->ForwardBTn, SIGNAL(clicked(bool)), this, SLOT(Forward()));
  QObject::connect(ui->BeginBTn, SIGNAL(clicked(bool)), this, SLOT(ToBegin()));
  QObject::connect(ui->EndBTn, SIGNAL(clicked(bool)), this, SLOT(ToEnd()));
  QObject::connect(ui->ViewModeCBx, SIGNAL(currentIndexChanged(int)), this, SLOT(ViewModeChange(int)));
  QObject::connect(ui->ColorModeCBx, SIGNAL(currentIndexChanged(int)), this, SLOT(RecStructChange(int)));

}
ZRawMasterFileVisu::~ZRawMasterFileVisu()
{
  delete ui;
}

ZStatus
ZRawMasterFileVisu::setup(const uriString& pURI , int pFd) {

  ui->ForwardBTn->setEnabled(true);
  ui->BackwardBTn->setEnabled(true);
  ui->BeginBTn->setEnabled(true);
  ui->EndBTn->setEnabled(true);

  utf8VaryingString wStr;
  if (pFd < -1)
    return ZS_FILENOTOPEN;
  if (!pURI.exists())
    return ZS_FILENOTEXIST;

  BlockCur.BlockSize=0L;
  BlockCur.Address=0L;

  Fd = pFd;

  FileSize=size_t(pURI.getFileSize());

  setQLabelNum(ui->FileSizeLBl,FileSize);

  AllFileLoaded=false;

/*
  Fd = ::open(pURI.toCChar(),O_RDONLY);// open content file for read only
  if (Fd < 0)
  {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Severe,
        " Error opening file %s ",
        pURI.toCChar());
    return  ZS_ERROPEN;
  }
*/
  URICurrent = pURI;
//  BlockList.clear();

  BlockTBv = new ZQTableView(this);
  BlockTBv->newModel(5);
  BlockTBv->setGeometry(10,190,660,320);

  //  BlockTBv->setContextMenuCallback(std::bind(&ZRawMasterFileVisu::VisuBvFlexMenuCallback, this,placeholders::_1));

  QFont wVisuFont ("Monospace");
  BlockTBv->setFont(wVisuFont);

//  ui->horizontalLayoutTBv->addWidget(BlockTBv);
//  ui->verticalLayoutTBv->addWidget(BlockTBv);
  int wCol=0;
  BlockTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Offset"));
  BlockTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("ZType hexa"));
  BlockTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("ZType"));
  BlockTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Head.size"));
  BlockTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Data size"));
  BlockTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem("Field data"));


  BlockDumpTBv=new ZQTableView(this);
  BlockDumpTBv->setGeometry(10,190,660,320);
  BlockDumpTBv->newModel(21);

  BlockDumpTBv->setFont(wVisuFont);


//  BlockDumpTBv->setMouseClickCallback(std::bind(&ZRawMasterFileVisu::VisuMouseCallback, this,placeholders::_1,placeholders::_2)  );
  BlockDumpTBv->setContextMenuCallback(std::bind(&ZRawMasterFileVisu::VisuBvFlexMenuCallback, this,placeholders::_1));

  int wj=0;
  int wk=0;
  for (int wi=0;wi < 20;wi++) {
    if (wk==4) {
      wk=0;
      BlockDumpTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(" "));
      continue;
    }
    wStr.sprintf("%d",wj);
    BlockDumpTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(wStr.toCChar()));
    wj++;
    wk++;
  }
  BlockDumpTBv->ItemModel->setHorizontalHeaderItem(20,new QStandardItem(tr("Ascii")));


  BlockTBv->setVisible(false);
  BlockDumpTBv->setVisible(true);

  return ZS_SUCCESS;
} // setup


ZStatus
ZRawMasterFileVisu::displayRawBlock(ZDataBuffer& pData) {

  ui->ColorsGBx->setVisible(true);
  ui->StartColorLBl->setVisible(true);
  ui->WrongColorLBl->setVisible(true);

  BlockDumpTBv->setGeometry(BlockTBv->geometry());

  BlockDumpTBv->ItemModel->removeRows(0,BlockDumpTBv->ItemModel->rowCount());

  /* how many lines within pData block */

  int wLines=pData.Size / Width;

  int wRem=pData.Size - ( wLines * Width);

  if (wRem > 0)
    wLines++;

  int wBaseLine=BlockDumpTBv->ItemModel->rowCount();

  BlockDumpTBv->ItemModel->setRowCount(BlockDumpTBv->ItemModel->rowCount()+wLines);

  utf8VaryingString  wLineHexa,wLineAscii,wlineOffset;

  /* cut by chunk of Width bytes */
  long  wRemain=pData.Size;
  unsigned char* wPtr = pData.Data;
  unsigned char* wPtrEnd = pData.Data+pData.Size;

  int wTick = 0;
  long wProgress=0;
  int wCurLine=wBaseLine;
  while ((wPtr < wPtrEnd)&&(wRemain >= Width)) {

    displayOneLine(wCurLine,wPtr,wPtrEnd);

    FileOffset += Width;
    wRemain -= Width;
    wProgress += Width;
    wCurLine++;
    if (wTick++ > 10) {
      wTick=0;
//      ui->ProgressPGb->setValue(wProgress);
    }
  }//while ((wPtr < wPtrEnd)&&(wRemain >= Width))

  if (wRemain > 0) {
    displayOneLine(wCurLine,wPtr,wPtrEnd);
  }

  for (int wi=0;wi < BlockDumpTBv->ItemModel->columnCount();wi++)
    BlockDumpTBv->resizeColumnToContents(wi);
  for (int wi=0;wi < BlockDumpTBv->ItemModel->rowCount();wi++)
    BlockDumpTBv->resizeRowToContents(wi);

  BlockDumpTBv->setColumnWidth(4,3);
  BlockDumpTBv->setColumnWidth(9,3);
  BlockDumpTBv->setColumnWidth(16,3);

  if (ui->ColorModeCBx->currentIndex()==1)
    colorizeURFBlock(pData);
  else
    colorizeURFKeyBlock(pData);

//  ui->ProgressPGb->setValue(pData.Size);
  return ZS_SUCCESS;
}//displayBlock


void
ZRawMasterFileVisu::displayOneLine(int pRow,unsigned char* &wPtr,unsigned char* wPtrEnd) {

  utf8String  wLineHexa,wLineAscii,wlineOffset;

  wlineOffset.sprintf("%6d-%6X",FileOffset,FileOffset);

  wLineAscii.allocateUnitsBZero(Width+1);

  int wCol=0;
  int wD=0;
  for (int wi=0;(wi < Width) && (wPtr < wPtrEnd);wi++) {
    wLineHexa.sprintf("%02X",*wPtr);
    BlockDumpTBv->ItemModel->setItem(pRow,wCol,new QStandardItem(wLineHexa.toCChar()));
    if ((*wPtr>31)&&(*wPtr<127))
      wLineAscii.Data[wi]=*wPtr;
    else
      wLineAscii.Data[wi] = '.' ;

    wCol++;
    wD++;
    if (wD==4) {
      wD=0;
      wCol++;
    }
    wPtr++;
  }// for
  BlockDumpTBv->ItemModel->setVerticalHeaderItem (pRow,new QStandardItem(wlineOffset.toCChar()));

  //    VisuTBv->setItem(wCurLine,0,new QTableWidgetItem(wLineHexa.toCChar()));
  BlockDumpTBv->ItemModel->setItem(pRow,20,new QStandardItem(wLineAscii.toCChar()));
/*
  if (DefaultBackGround.isValid())
    return;
  DefaultBackGround = VisuTBv->ItemModel->item(0,0)->data(Qt::BackgroundRole);
*/
} // displayOneLine


ZStatus
ZRawMasterFileVisu::seekAndGet(ZDataBuffer& pOut, ssize_t &pSize, size_t pAddress) {
  FileOffset=lseek(Fd,off_t(pAddress),SEEK_SET);
  if (FileOffset < 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning at address <%ld> for file <%s> ",
        pAddress,
        URICurrent.toCChar());
    ZExceptionDLg::displayLast();
    ZException.pop();
    return  ZS_FILEPOSERR ;
  }

  if ((pSize + FileOffset) > FileSize) {
    pSize= FileSize -  FileOffset;
    if (pSize <= 0) {
      ui->BottomLBl->setVisible(true);
      ui->TopLBl->setVisible(false);
      return ZS_EOF;
    }
  }
  pOut.allocateBZero(pSize);
  pSize=::read(Fd,pOut.DataChar,pSize);
  if (pSize < 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_READERROR,
        Severity_Severe,
        "While reading size <%ld> from file <%s>.",pSize,URICurrent.toCChar());
    ZExceptionDLg::displayLast();
    ZException.pop();
    return ZS_READERROR;
  }

  return ZS_SUCCESS;
} //seekAndGet

/** pAddress is the file offset that corresponds to wRecord.Data */

ZStatus
ZRawMasterFileVisu::searchNextValidZType(const ZDataBuffer& pRecord,
                                          zaddress_type & pAddress,
                                          const unsigned char* &pPtr) {
  ZStatus wSt=ZS_SUCCESS;
  const unsigned char* wPtrEnd = pRecord.Data + pRecord.Size ;

  ZTypeBase wZType = reverseByteOrder_Ptr<ZTypeBase>(pPtr);

//  zaddress_type wAddress = pAddress;

    while (!ZTypeExists(wZType) && (pPtr < wPtrEnd)) {
      pAddress ++;
      pPtr++;
      wZType=reverseByteOrder_Ptr<ZTypeBase>(pPtr);
    }

    if (ZTypeExists(wZType)) {
      utf8VaryingString wStr;
      wStr.sprintf("searchNextValidZType Found type %X <%s> address %ld as next valid data type.",
          wZType,decode_ZType(wZType),pAddress);
      ui->MessagePTe->appendPlainText(wStr.toCChar());
/*
      _DBGPRINT ("ZRawMasterFileVisu::searchNextValidZType-I-FND Found type %X <%s> address %ld.\n",
          wZType,decode_ZType(wZType),pAddress)
*/
      return ZS_FOUND;
    }
    utf8VaryingString wStr;
    wStr.sprintf("searchNextValidZType No valid ZType can be found untill end of block.");
    ui->MessagePTe->appendPlainText(wStr.toCChar());
    return ZS_NOTFOUND;
}// searchNextValidZType

ZStatus
ZRawMasterFileVisu::searchNextBlock(ZBlockDescriptor_Export& pBlock, zaddress_type pStartAddress) {
  ZBlockHeader_Export wBExp;
  ZDataBuffer wRecord;
  ssize_t wSize= ssize_t(sizeof(ZBlockHeader_Export));

  zaddress_type pOutAddress;

  ZStatus wSt=searchNextStartSign(pStartAddress,pOutAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wSt=seekAndGet(wRecord,wSize,pOutAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  memmove(&wBExp,wRecord.Data,sizeof(ZBlockHeader_Export));
  wBExp.deserialize();
  pBlock.State = wBExp.State;
  pBlock.Pid = wBExp.Pid;
  pBlock.Lock = wBExp.Lock;
  pBlock.BlockSize = wBExp.BlockSize;
  pBlock.Address = pOutAddress;

  return ZS_SUCCESS;
}
ZStatus
ZRawMasterFileVisu::searchPreviousBlock(ZBlockDescriptor_Export& pBlock, zaddress_type pStartAddress) {
  ZDataBuffer wRecord;
  zaddress_type pOutAddress;
  ZBlockHeader_Export wBExp;
  ssize_t wSize= ssize_t(sizeof(ZBlockHeader_Export));
  ZStatus wSt=searchPreviousStartSign(pStartAddress,pOutAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wSt=seekAndGet(wRecord,wSize,pOutAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  memmove(&wBExp,wRecord.Data,sizeof(ZBlockHeader_Export));
  wBExp.deserialize();
  pBlock.State = wBExp.State;
  pBlock.Pid = wBExp.Pid;
  pBlock.Lock = wBExp.Lock;
  pBlock.BlockSize = wBExp.BlockSize;
  pBlock.Address = pOutAddress;

  BlockCur = pBlock;

  return ZS_SUCCESS;
} //searchPreviousBlock


/** pAddress is the file offset that corresponds to wRecord.Data */
ZStatus
ZRawMasterFileVisu::searchNextStartSign(zaddress_type pStartAddress, zaddress_type & pOutAddress) {


  if ((pStartAddress < 0)||(pStartAddress>FileSize)) {
    utf8VaryingString wStr;
    wStr.sprintf("Invalid address to search <%lld>.",pStartAddress);
    ui->MessagePTe->appendPlainText(wStr.toCChar());
    return ZS_OUTBOUND;
  }

  ZStatus wSt=ZS_SUCCESS;
  long wNudge = FileNudge;

  ZDataBuffer wRecord;
  if ( long(pStartAddress) - long(FileSize) < wNudge)
    wNudge = long(FileSize) - long(pStartAddress) ;
  else
    pStartAddress += wNudge;

  wSt=seekAndGet(wRecord,wNudge,pStartAddress);

  unsigned char* wPtr = wRecord.Data ;
  unsigned char* wPtrEnd = wRecord.Data + wRecord.Size;

  uint32_t* wStartSign = (uint32_t*)wPtr;
  if (*wStartSign== cst_ZFILEBLOCKSTART) {
    pOutAddress = pStartAddress;
    FileOffset = off_t(pStartAddress);
    return ZS_SUCCESS;
  }


  while ((pStartAddress < zaddress_type(FileSize)) && (wSt==ZS_SUCCESS)) {

    while ((*wStartSign!=cst_ZFILEBLOCKSTART) && (wPtr < wPtrEnd)) {
      wStartSign = (uint32_t*)wPtr;
      pStartAddress ++;
      FileOffset++;
      wPtr++;
    }
    if (*wStartSign==cst_ZFILEBLOCKSTART)
      break;

    if (pStartAddress >= zaddress_type(FileSize)) {
      ui->MessagePTe->appendPlainText("No start block mark found. File surface exhausted.");
      return ZS_OUTBOUNDHIGH;
    }
    pStartAddress -= 3;
    FileOffset -= 3;
    wSt=seekAndGet(wRecord,wNudge,pStartAddress);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wPtr = wRecord.Data;
    wPtrEnd = wRecord.Data + wRecord.Size ;
  } // while wAddress


  pOutAddress = pStartAddress;
  FileOffset = off_t(pStartAddress);

  return ZS_SUCCESS;
}// searchNextStartSign


void ZRawMasterFileVisu::getPrevAddrVal(zaddress_type &pAddress, long &pNudge, long &pBucket){
  pNudge = FileNudge ;
  if (pAddress < pNudge) {
    pNudge = pBucket =  pAddress ;
    pAddress = 0L;
    return;
  }

  pAddress -= pNudge;

  if ((pAddress + FileNudge) > FileSize){
    pBucket = FileSize - pAddress ;
  }
  else
    pBucket = FileNudge + sizeof(cst_ZFILEBLOCKSTART) -1 ;
}

/* search previous start block starting at pAddress and decreasing to beginning of file */
ZStatus
ZRawMasterFileVisu::searchPreviousStartSign(zaddress_type pStartAddress, zaddress_type & pOutAddress ) {

  if ((pStartAddress < 0)||(pStartAddress>FileSize)) {
    utf8VaryingString wStr;
    wStr.sprintf("Invalid address to search <%lld>.",pStartAddress);
    ui->MessagePTe->appendPlainText(wStr.toCChar());
    return ZS_OUTBOUND;
  }

  ZStatus wSt=ZS_SUCCESS;

  long wNudge = FileNudge ;
  long wBucket = wNudge + sizeof(cst_ZFILEBLOCKSTART) -1; /* just to take a truncated start sign into account */

  FileOffset = off_t(pStartAddress);

  ZDataBuffer wRecord;
  zaddress_type wAddress=pStartAddress;

  unsigned char* wPtr ;
  unsigned char* wPtrEnd ;
  uint32_t* wStartSign ;

  getPrevAddrVal(wAddress,wNudge,wBucket);

  wSt=seekAndGet(wRecord,wBucket,wAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wPtrEnd = wRecord.Data ;
  wPtr =  wRecord.Data + wRecord.Size - sizeof(cst_ZFILEBLOCKSTART) ;
  pStartAddress = wAddress + wRecord.Size - sizeof(cst_ZFILEBLOCKSTART);
  wStartSign=(uint32_t*)wPtr;
  while ((pStartAddress >= zaddress_type(0L)) && (wSt==ZS_SUCCESS)) {
    while ((*wStartSign!=cst_ZFILEBLOCKSTART) && (wPtr >= wPtrEnd)) {
      wStartSign = (uint32_t*)wPtr;
//      pStartAddress --;
//      FileOffset --;
      wPtr--;
    }
    if (*wStartSign==cst_ZFILEBLOCKSTART)
      break;

    /* here wPtr == wRecord.Data */

    getPrevAddrVal(wAddress,wNudge,wBucket);

    wSt=seekAndGet(wRecord,wBucket,wAddress);
    if (wSt!=ZS_SUCCESS)
      return wSt;

    wPtrEnd = wRecord.Data  ;
    wPtr =  wRecord.Data + wRecord.Size - sizeof(cst_ZFILEBLOCKSTART);
    wStartSign=(uint32_t*)wPtr;
  } // while wAddress

  size_t wOffset = (wPtr-wRecord.Data) + 1;
  pOutAddress = wAddress + wOffset ;
  FileOffset = off_t(pOutAddress);
  return ZS_SUCCESS;
}// searchNextStartSign


void
ZRawMasterFileVisu::Backward(){

  ui->BottomLBl->setVisible(false);
  zaddress_type wAdr= BlockCur.Address - sizeof(cst_ZFILEBLOCKSTART);
  ZStatus wSt=searchPreviousBlock(BlockCur,wAdr);

  if ((wAdr == 0) || (wSt==ZS_OUTBOUNDLOW)) {
    ui->TopLBl->setVisible(true);
    return;
  }
  ui->BottomLBl->setVisible(false);
  displayBlock(BlockCur);
  return;
}


void
ZRawMasterFileVisu::Forward() {
  ZStatus wSt;
  ZDataBuffer wRecord;
  zaddress_type wStartAddress=BlockCur.Address + BlockCur.BlockSize;

  wSt=searchNextBlock(BlockCur,wStartAddress);
  if (wSt==ZS_OUTBOUNDHIGH) {
    ui->BottomLBl->setVisible(true);
    return;
  }
  if (FileOffset >= off_t(FileSize)) {
    ui->BottomLBl->setVisible(true);
    return;
  }

  ui->TopLBl->setVisible(false);
  displayBlock(BlockCur);
  return;
} // Forward

void
ZRawMasterFileVisu::setViewModeRaw() {
  ui->ViewModeCBx->setCurrentIndex(0);
}
void
ZRawMasterFileVisu::setViewModeURF() {
  ui->ViewModeCBx->setCurrentIndex(1);
}
void
ZRawMasterFileVisu::goToAddress(zaddress_type pAddress, zrank_type pRank) {
  ZStatus wSt;
  ssize_t wSize = sizeof(ZBlockHeader_Export);
  ZDataBuffer wRecord;

/*
  ui->ForwardBTn->setEnabled(false);
  ui->BackwardBTn->setEnabled(false);
  ui->BeginBTn->setEnabled(false);
  ui->EndBTn->setEnabled(false);
*/
//  BlockRank = pRank;



  wSt = seekAndGet(wRecord,wSize,pAddress);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("ZRawMasterFileVisu::goToAddress",Severity_Error,nullptr,nullptr,
        "Cannot set to address %lld for file <%s>",pAddress,URICurrent.toString());
    return;
  }
  memmove (&BlockCur,wRecord.Data,sizeof(ZBlockHeader_Export));

  if (BlockCur.StartSign!=cst_ZFILEBLOCKSTART){
    ZExceptionDLg::adhocMessage("ZRawMasterFileVisu::goToAddress",Severity_Error,nullptr,nullptr,
        "Invalid block found at address %lld for file <%s>",pAddress,URICurrent.toString());
    return;

  }

  BlockCur.deserialize();
  BlockCur.Address = pAddress;

//  BlockList.push(BlockCur);
//  BlockRank = BlockList.lastIdx();

  ui->TopLBl->setVisible(false);

  displayBlock(BlockCur);
  return;
} // goToAddress

void
ZRawMasterFileVisu::ToEnd(){

  ZStatus wSt=ZS_SUCCESS;

  ui->TopLBl->setVisible(false);

  FileOffset = FileSize ;

  wSt=searchPreviousBlock(BlockCur,zaddress_type(FileOffset));
  if (wSt!=ZS_SUCCESS){
      ZExceptionDLg::messageWAdd("ZRawMasterFileVisu::ToEnd",wSt,Severity_Severe,
          ZException.last().formatUtf8(),
          "Cannot get last block on file %s,",URICurrent.toCChar());
      return;
    }

  displayBlock(BlockCur);  /* BlockCur contains the last accessed Block */
}

void
ZRawMasterFileVisu::ToBegin(){
  FileOffset=0;
  ui->BottomLBl->setVisible(false);
  ZStatus wSt=searchNextBlock(BlockCur,zaddress_type(FileOffset));
  if (wSt!=ZS_SUCCESS){
    ZExceptionDLg::messageWAdd("ZRawMasterFileVisu::ToBegin",wSt,Severity_Severe,
        ZException.last().formatUtf8(),
        "Cannot get first block on file %s,",URICurrent.toCChar());
    return;
  }

  displayBlock(BlockCur);
  return;
}


template <class _Tp>
_Tp
convertAtomicBack(ZType_type pType,const unsigned char* &pPtrIn) {
  _Tp wValue;
  uint8_t wSign=1;
  if (pType & ZType_Signed) {
    wSign = *pPtrIn;
    pPtrIn += sizeof(uint8_t);
  }
  memmove(&wValue,pPtrIn,sizeof(_Tp));
  pPtrIn += sizeof(_Tp);
  wValue = reverseByteOrder_Conditional(wValue);
  if (!wSign) {
    wValue = _negate(wValue);
    wValue = -wValue;
  }
  return wValue;
} // convertAtomicBack

void
ZRawMasterFileVisu::ViewModeChange(int pIndex) {

  if (pIndex==0){
    BlockTBv->setVisible(false);
    BlockDumpTBv->setVisible(true);
    Raw=true;

    ui->URFSizeLabelLBl->setVisible(false);
    ui->URFSizeLBl->setVisible(false);
    displayBlock(BlockCur);
    return;
  }
  BlockTBv->setVisible(true);
  BlockDumpTBv->setVisible(false);
  Raw=false;

  ui->URFSizeLabelLBl->setVisible(true);
  ui->URFSizeLBl->setVisible(true);

  displayBlock(BlockCur);

  return;
}
void
ZRawMasterFileVisu::RecStructChange(int pIndex) {
  if ((pIndex==0)||(pIndex==2)) {
    ui->URFSizeLabelLBl->setVisible(false);
    ui->URFSizeLBl->setVisible(false);
    ui->FieldPresenceGBx->setVisible(false);
    displayBlock(BlockCur);

    return;
  }

  ui->URFSizeLabelLBl->setVisible(true);
  ui->URFSizeLBl->setVisible(true);
  ui->FieldPresenceGBx->setVisible(true);
  displayBlock(BlockCur);

  return;
}

ZStatus
ZRawMasterFileVisu::seekAndRead(ZDataBuffer& pRecord,ZBlockDescriptor_Export & pBlock) {
  if (pBlock.isReversed())
    pBlock.deserialize();

  FileOffset=lseek(Fd,off_t(pBlock.Address),SEEK_SET);
  if (FileOffset < 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEPOSERR,
        Severity_Severe,
        "Error positionning at address <%ld> for file <%s> ",
        pBlock.Address,
        URICurrent.toCChar());
    ZExceptionDLg::displayLast();
    ZException.pop();
    return  ZS_FILEPOSERR ;
  }
  pRecord.allocate(pBlock.BlockSize);
  ssize_t wSize=::read(Fd,pRecord.DataChar,pBlock.BlockSize);
  if (wSize < 0) {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_READERROR,
        Severity_Severe,
        "While reading size <%ld> from file <%s>.",pBlock.BlockSize,URICurrent.toCChar());
    ZExceptionDLg::displayLast();
    ZException.pop();
    return ZS_READERROR;
  }
  return ZS_SUCCESS;
}//seekAndRead


ZStatus
ZRawMasterFileVisu::displayBlock(ZBlockDescriptor_Export & pBlock)
{

  utf8VaryingString wStr;
  QList<QStandardItem*> wRow;
  RawRecord.allocate(pBlock.BlockSize);

  setQLabelNum<zsize_type>(ui->FileSizeLBl,pBlock.BlockSize);
//  setQLabel<long>(ui->BlockNbLBl,BlockRank);

  ZStatus wSt = seekAndRead(RawRecord,pBlock);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  setQLabelNum(ui->BeginLBl,pBlock.Address);
  setQLabelNum<zaddress_type>(ui->EndLBl,zaddress_type(pBlock.Address+pBlock.BlockSize));
  setQLabelNum<size_t>(ui->FileSizeLBl,pBlock.BlockSize);

  ui->StateLBl->setText(decode_ZBS(pBlock.State));

  if (Raw) {
    if (BlockTBv->isVisible()) {
      BlockTBv->setVisible(false);
      BlockDumpTBv->setVisible(true);
    }

    return displayRawBlock(RawRecord);
  }
  else {
    if (!BlockTBv->isVisible()) {
      BlockTBv->setVisible(true);
      BlockDumpTBv->setVisible(false);
    }
    if (ui->ColorModeCBx->currentIndex()==2)
        return displayURFKeyBlock(RawRecord);
    return displayURFBlock(RawRecord);
  }
}//displayBlock

int SpaceCount=5;
const int MaxCol=19;
void addCurCol(int &pCol, int &pLine) {
  pCol++;
  if (pCol >= MaxCol ) {
    pCol = 0;
    pLine ++;
  }
  if (((pCol +1) % SpaceCount)==0) {
    pCol++;
  }
}

void
ZRawMasterFileVisu::setSelectionBackGround( QVariant& pBackground,
                                            QVariant& pBackgroundFirst,
                                            ssize_t pOffset,
                                            size_t pSize,
                                            const utf8VaryingString& pToolTip,
                                            bool pScrollTo) {
  VisuLineCol wVLC;
  wVLC.compute(pOffset);
  int wStartLine = wVLC.line;
  int wStartColumn = wVLC.col;

  int wCurLine = wStartLine;
  int wCurCol = wStartColumn;
  int wCount=0;
  int wColLimit = Width + 3 ;

  /* highlight process */
  QStandardItem* wFirstItem=BlockDumpTBv->ItemModel->item(wCurLine,wCurCol);
  if (wFirstItem==nullptr) {
    wCurCol++;
    if (wCurCol >= wColLimit ) {
      wCurCol = 0;
      wCurLine ++;
    }
    wFirstItem = BlockDumpTBv->ItemModel->item(wCurLine,wCurCol);
  }
  if (BackGroundOnce && wFirstItem) {
    BackGroundOnce=false;
    wFirstItem->setData(pBackground, Qt::BackgroundRole);
  }

  while (wCount < pSize) {

    QStandardItem* wItem=BlockDumpTBv->ItemModel->item(wCurLine,wCurCol);
    if (wItem != nullptr) {
      if (wCount==0) {
        if (!pToolTip.isEmpty()) {
          wItem->setToolTip(pToolTip.toCChar());
        }
        wItem->setData(pBackgroundFirst, Qt::BackgroundRole);
      }
      else
        wItem->setData(pBackground, Qt::BackgroundRole);
    } // if (wItem != nullptr)
    wCount++; /* advance even if item is nullptr */

    addCurCol(wCurCol,wCurLine);
/*
    wCurCol++;
    if (wCurCol >= wColLimit ) {
      wCurCol = 0;
      wCurLine ++;
    }
*/
  }// while true

  if (pScrollTo) {
    QModelIndex wIdx=BlockDumpTBv->ItemModel->indexFromItem(wFirstItem);
    BlockTBv->scrollTo(wIdx);
  }

} // ZRawMasterFileVisu::setSelectionBackGround



ZStatus
ZRawMasterFileVisu::displayURFBlock(ZDataBuffer & pData)
{
  ui->ColorsGBx->setVisible(false);

  BlockTBv->ItemModel->removeRows(0,BlockTBv->ItemModel->rowCount());

  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wStr;
  QList<QStandardItem*> wRow;
  zaddress_type wOffset=0;
  const unsigned char* wPtr = pData.Data;

  setQLabelNum<size_t>(ui->FileSizeLBl,pData.Size);

  /* for debug purpose */
  ZBlockDescriptor_Export wBlke;
  memmove(&wBlke,wPtr,sizeof(ZBlockDescriptor_Export));
  wBlke.deserialize();
  wPtr += sizeof(ZBlockHeader_Export); /* NB: stored on file is ZBlockHeader and not ZBlockDescriptor (address is missing on file) */

  ZTypeBase wZType;
  size_t    wURFHeaderSize=0;
  uint64_t  wURFDataSize = 0;
  uint32_t  wKeyNb=0;
  zaddress_type wKeyAddress=0;

  /* leading user record size */
  _importAtomic<uint64_t>(wURFDataSize,wPtr);
  wStr.sprintf("%ld",wURFDataSize);
  ui->URFSizeLBl->setText(wStr.toCChar());

  const unsigned char* wPtrEnd = pData.Data + size_t(wURFDataSize);


  int wErrored=0;
  bool wZTypeErrored=false;
#ifdef __COMMENT__
  _importAtomic<ZTypeBase>(wZType,wPtr);
  while (true) {
    if ((wZType != ZType_bitset) && (wZType != ZType_bitsetFull)) {
      utf8VaryingString wStr;
      wStr.sprintf("Invalid format. While expecting <ZType_bitset>, found <%6X> <%s>.",wZType,decode_ZType(wZType));
      ui->MessagePTe->appendPlainText(wStr.toCChar());
      ui->MessagePTe->appendPlainText("Block is not a valid data record.");
      wErrored = 10;
      break;
    }
    if (wZType==ZType_bitset) {
      wPtr -= sizeof(ZTypeBase);
      ZBitset wBitset;
      ssize_t wSize=wBitset._importURF(wPtr);
      /* display bitset in header */
      utf8VaryingString wBSH;
      wBSH.allocateUnitsBZero(wBitset.EffectiveBitSize+1);
      utf8_t* wPtrH=wBSH.Data;
      int wC=0 , wCC=0;
      while (wC < int(wBitset.EffectiveBitSize+1)) {
        if (wCC == 10)
          wCC=0;
        *wPtrH = utf8_t(wCC + '0');
        wPtrH++;
        wC++;
        wCC++;
      } // while
      *wPtrH = 0;

      ui->BitsetHeaderLBl->setText(wBSH.toCChar());
      ui->BitsetContentLBl->setText(wBitset.toString().toCChar());
      wBSH.sprintf("%ld",wBitset.getEffectivebitsNumber());
      ui->FieldsNbLBl->setText(wBSH.toCChar());
      wOffset += wSize;
    } // if (wZType==ZType_bitset)

    else if (wZType==ZType_bitsetFull) {
      /* bitset full is ok : nothing to do */
      ui->BitsetHeaderLBl->setText(" ");
      ui->BitsetContentLBl->setText("all fields present");
      wOffset += sizeof(ZTypeBase);
    } // if (wZType==ZType_bitsetFull)

    /* second get user URF data size */

    _importAtomic<uint64_t>(wURFDataSize,wPtr);
    utf8VaryingString wStr;
    wStr.sprintf("%ld",wURFDataSize);
    ui->URFSizeLBl->setText(wStr.toCChar());
    wOffset += sizeof(uint64_t);
    break;
  }// while true
#endif // __COMMENT__

  while ((wPtr < wPtrEnd )&&(wErrored < 10)) {


    wZTypeErrored=displayOneURFField(wOffset,wPtr,wPtrEnd);

    if (wZTypeErrored && (wErrored < 10) ) {
      wErrored ++;
      wSt=searchNextValidZType(pData,wOffset,wPtr);
      if (wSt!=ZS_FOUND) {
        break;
      }
    }

  } // while (wPtr < wPtrEnd )

  if (wErrored >= 10) {
    wRow.clear();
    wRow  <<  new QStandardItem(" ");
    wRow  <<  new QStandardItem(" ");
    wRow  <<  new QStandardItem(" ");
    wRow  <<  new QStandardItem(" ");
    wRow  <<  new QStandardItem("URF parsing is interrupted.");

    ui->MessagePTe->appendPlainText("Too much errors detected. URF parsing is interrupted.");

    BlockTBv->ItemModel->appendRow(wRow);
    BlockTBv->ItemModel->setVerticalHeaderItem(BlockTBv->ItemModel->rowCount()-1,new QStandardItem("-error-"));
  }

  /* processing keys data */
  wPtr = pData.Data + size_t(wURFDataSize);
  wPtrEnd = pData.Data + pData.Size;

  wOffset = wURFDataSize + sizeof(uint64_t);

  if (wPtr < wPtrEnd) {

      /* get key data */

      wRow.clear();
      wStr.sprintf("%4ld %4lX",wOffset,wOffset);
      wRow << createItem(wStr.toCChar());

      _importAtomic<uint32_t>(wKeyNb,wPtr);

      wRow << createItem("uint32_t");
      wRow << createItem("Nb of keys");
      wRow << createItem("--");
      wRow << createItem(sizeof(uint32_t));
      wRow << createItem(wKeyNb);

      if (wKeyNb > 100) {
        wRow << createItem("Invalid keys number");
        wKeyNb = 100;
      }

      BlockTBv->ItemModel->appendRow(wRow);

      wOffset += sizeof(uint32_t);

      for (uint32_t wi=0; (wi < wKeyNb) && (wPtr < wPtrEnd) ; wi++) {

        _importAtomic<zaddress_type>(wKeyAddress,wPtr);

        wRow.clear();

        wStr.sprintf("%4ld %4lX",wOffset,wOffset);
        wRow << createItem(wStr.toCChar());

        wRow << createItem("zaddress_type");
        wRow << createItem("@Index record");
        wRow << createItem("--");
        wRow << createItem(sizeof(zaddress_type));
        wRow << createItem(wKeyAddress);

        BlockTBv->ItemModel->appendRow(wRow);

        wOffset += sizeof(zaddress_type);
        wPtr += sizeof(zaddress_type);
      } // for

  }//  if (wPtr < wPtrEnd)
  else {
    wRow.clear();
    wRow << createItem("---");
    wRow << new QStandardItem("--No key defined--");
    BlockTBv->ItemModel->appendRow(wRow);
  }

  for (int wi=0; wi < BlockTBv->ItemModel->columnCount(); wi ++ )
    BlockTBv->resizeColumnToContents(wi);
  for (int wi=0; wi < BlockTBv->ItemModel->rowCount(); wi ++ )
    BlockTBv->resizeRowToContents(wi);

  return ZS_SUCCESS;
}//displayURFBlock


ZStatus
ZRawMasterFileVisu::displayURFKeyBlock(ZDataBuffer & pData)
{
  ui->ColorsGBx->setVisible(false);

  BlockTBv->ItemModel->removeRows(0,BlockTBv->ItemModel->rowCount());

  ZStatus wSt=ZS_SUCCESS;
  utf8VaryingString wStr;
  QList<QStandardItem*> wRow;
  zaddress_type wOffset=0;
  const unsigned char* wPtr = pData.Data;
  const unsigned char* wPtrEnd = pData.Data + pData.Size;
  /* for debug purpose */
  ZBlockDescriptor_Export wBlke;
  memmove(&wBlke,wPtr,sizeof(ZBlockDescriptor_Export));
  wBlke.deserialize();
  wPtr += sizeof(ZBlockHeader_Export); /* NB: stored on file is ZBlockHeader and not ZBlockDescriptor (address is missing on file) */


  zaddress_type   wZMFAddress = 0;

  int wErrored=0;
  bool wZTypeErrored=false;

  /* get and display zmf address from key record */

  _importAtomic<zaddress_type>(wZMFAddress,wPtr);

    ui->URFSizeLBl->setText("not relevant");

    wStr.sprintf("%4ld %4lX",wOffset,wOffset);
    wRow << createItem(wStr.toCChar());

    wRow << new QStandardItem ("Master address");
    wRow << createItem("zaddress_type");

    wRow << createItem(0);                      /* no URF header */
    wRow << createItem(sizeof(zaddress_type));

    wStr.sprintf("%4ld - %4lX",wZMFAddress);
    wRow << createItem(wStr.toCChar());

    BlockTBv->ItemModel->appendRow(wRow);

    wOffset += sizeof(zaddress_type);

  while ((wPtr < wPtrEnd )&&(wErrored < 10)) {

    wZTypeErrored=displayOneURFField(wOffset,wPtr,wPtrEnd);

    if (wZTypeErrored && (wErrored < 10) ) {
      wErrored ++;
      wSt=searchNextValidZType(pData,wOffset,wPtr);
      if (wSt!=ZS_FOUND) {
        break;
      }
    }

  } // while (wPtr < wPtrEnd )

  if (wErrored >= 10) {
    wRow.clear();
    wRow  <<  new QStandardItem(" ");
    wRow  <<  new QStandardItem(" ");
    wRow  <<  new QStandardItem(" ");
    wRow  <<  new QStandardItem(" ");
    wRow  <<  new QStandardItem("URF parsing is interrupted.");

    ui->MessagePTe->appendPlainText("Too much errors detected. URF parsing is interrupted.");

    BlockTBv->ItemModel->appendRow(wRow);
    BlockTBv->ItemModel->setVerticalHeaderItem(BlockTBv->ItemModel->rowCount()-1,new QStandardItem("-error-"));
  }
  for (int wi=0; wi < BlockTBv->ItemModel->columnCount(); wi ++ )
    BlockTBv->resizeColumnToContents(wi);
  for (int wi=0; wi < BlockTBv->ItemModel->rowCount(); wi ++ )
    BlockTBv->resizeRowToContents(wi);

  return ZS_SUCCESS;
}//displayURFKeyBlock

bool
ZRawMasterFileVisu::displayOneURFField(zaddress_type &wOffset,const unsigned char* &wPtr,const unsigned char* wPtrEnd) {
  ZTypeBase wZType;
  size_t    wURFHeaderSize=0;
  uint64_t  wURFDataSize = 0;

  ZStatus wSt=ZS_SUCCESS;

  utf8VaryingString wStr;
  QList<QStandardItem*> wRow;
  _importAtomic<ZTypeBase>(wZType,wPtr);

  wRow.clear();
  wStr.sprintf("%4ld %4lX",wOffset,wOffset);
  wRow << createItem(wStr.toCChar());

  wRow << createItem(wZType,"0x%8X");
  wRow << createItem(decode_ZType(wZType));
  wURFHeaderSize = getURFHeaderSize(wZType);
  wRow << createItem(wURFHeaderSize);

  /* for atomic URF data, value is just following ZType. For other types, use _importURF function that implies ZType */
  if (!(wZType & ZType_Atomic))
    wPtr -= sizeof(ZTypeBase);
  else  {
    wZType &= ~ZType_Atomic;
    wOffset += sizeof(ZTypeBase);
  }
  switch (wZType) {
  case ZType_UChar:
  case ZType_U8: {
    uint8_t wValue;

    wValue=convertAtomicBack<uint8_t> (ZType_U8,wPtr);

    wRow << createItem(sizeof(uint8_t));

    wStr.sprintf("%d 0x%X",wValue,wValue);
    wRow << createItem(wStr);
    wOffset += sizeof(uint8_t);
    break;
  }
  case ZType_Char:
  case ZType_S8: {
    int8_t wValue;
    wValue=convertAtomicBack<int8_t> (ZType_S8,wPtr);

    wRow << createItem(sizeof(int8_t)+1);
    wStr.sprintf("%d 0x%X",wValue,wValue);
    wRow << createItem(wStr);
    wOffset += sizeof(int8_t)+1;
    break;
  }
  case ZType_U16:{
    uint16_t wValue;
    wValue=convertAtomicBack<uint16_t> (ZType_U16,wPtr);

    wRow << createItem(sizeof(uint16_t));
    wStr.sprintf("%d 0x%X",wValue,wValue);
    wRow << createItem(wStr);
    wOffset += sizeof(uint16_t);
    break;
  }
  case ZType_S16: {
    int16_t wValue;
    wValue=convertAtomicBack<int16_t> (ZType_S16,wPtr);

    wRow << createItem(sizeof(int16_t)+1);
    wStr.sprintf("%d 0x%X",wValue,wValue);
    wRow << createItem(wStr);
    wOffset += sizeof(int16_t)+1;
    break;
  }

  case ZType_U32:{
    uint32_t wValue;
    wValue=convertAtomicBack<uint32_t> (ZType_U32,wPtr);

    wRow << createItem(sizeof(uint32_t));

    wStr.sprintf("%d 0x%X",wValue,wValue);
    wRow << createItem(wStr);
    wOffset += sizeof(uint32_t);
    break;
  }
  case ZType_S32: {
    int32_t wValue;
    wValue=convertAtomicBack<int32_t> (ZType_S32,wPtr);

    wRow << createItem(sizeof(int32_t)+1);

    wStr.sprintf("%d 0x%X",wValue,wValue);
    wRow << createItem(wStr);
    wOffset += sizeof(int32_t)+1;
    break;
  }
  case ZType_U64: {
    uint64_t wValue;
    wValue=convertAtomicBack<uint64_t> (ZType_U64,wPtr);
    wRow << createItem(sizeof(uint64_t));

    wStr.sprintf("%llu 0x%X",wValue,wValue);
    wRow << createItem(wStr);
    wOffset += sizeof(uint64_t);
    break;
  }
  case ZType_S64: {
    int64_t wValue;
    wValue=convertAtomicBack<int64_t> (ZType_S64,wPtr);

    wRow << createItem(sizeof(int64_t)+1);

    wStr.sprintf("%lld 0x%X",wValue,wValue);
    wRow << createItem(wStr);
    wOffset += sizeof(int64_t)+1;
    break;
  }
  case ZType_Float: {
    float wValue;
    wValue=convertAtomicBack<float> (ZType_Float,wPtr);
    wRow << createItem(sizeof(float)+1);

    wRow << createItem(wValue,"%g");
    wOffset += sizeof(float)+1;
    break;
  }

  case ZType_Double: {
    double wValue;
    wValue=convertAtomicBack<double> (ZType_Double,wPtr);

    wRow << createItem(sizeof(double)+1);

    wRow << createItem(wValue,"%g");
    wOffset += sizeof(double)+1;
    break;
  }

  case ZType_LDouble: {
    long double wValue;
    wValue=convertAtomicBack<long double> (ZType_LDouble,wPtr);

    wRow << createItem(sizeof(long double)+1);

    wRow << createItem(wValue,"%g");
    wOffset += sizeof(long double)+1;
    break;
  }

    /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */

  case ZType_ZDate: {
    ssize_t wSize;
    ZDate wZDate;
    wRow << createItem(sizeof(uint32_t)); /* size of URF data without URF Header */
    if ((wSize = wZDate._importURF(wPtr)) < 0) {
      wRow << new QStandardItem("**Invalid value**");
      break;
    }

    wRow << new QStandardItem(wZDate.toLocale().toCChar());
    wOffset += wSize ;
    break;
  }
  case ZType_ZDateFull: {
    ssize_t wSize;
    ZDateFull wZDateFull;

    wRow << createItem(sizeof(uint64_t)); /* size of URF data without URF Header */

    if ((wSize = wZDateFull._importURF(wPtr)) < 0) {
      wRow << new QStandardItem("**Invalid value**");
      break;
    }
    wRow << new QStandardItem(wZDateFull.toLocale().toCChar());
    wOffset += wSize ;
    break;
  }

  case ZType_URIString:{
    uriString wString;
    ssize_t wSize = wString._importURF(wPtr);

    if (wString.strlen() > StringDiplayMax){
      utf8VaryingString w1 = cst_OverflowChar8 ;
      w1 += wString.Right(StringDiplayMax).toString();

      wString = w1;
    }

    if (wString.isEmpty()) {
      wRow << createItem(0); /* size of URF data without URF Header */
      wRow << new QStandardItem("<empty string>");
      break;
    }

    wRow << createItem(wSize - wString.getURFHeaderSize()); /* size of URF data without URF Header */
    wRow << new QStandardItem(wString.toCChar());

    wOffset += wSize ;
    break;
  }
  case ZType_Utf8VaryingString: {
    utf8VaryingString wString;
    ssize_t wSize = wString._importURF(wPtr);

    if (wString.strlen() > StringDiplayMax){
      utf8VaryingString w1 = cst_OverflowChar8 ;
      w1 += wString.Right(StringDiplayMax).toString();

      wString = w1;
    }

    if (wString.isEmpty()) {
      wRow << createItem(0); /* size of URF data without URF Header */
      wRow << new QStandardItem("<empty string>");
      break;
    }

    wRow << createItem(wSize - wString.getURFHeaderSize()); /* size of URF data without URF Header */
    wRow << new QStandardItem(wString.toCChar());

    wOffset += wSize ;
    break;
  }


  case ZType_Utf16VaryingString:{
    utf16VaryingString wString;

    ssize_t wSize = wString._importURF(wPtr);
    if (wString.strlen() > StringDiplayMax){
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar16);
    }
    if (wString.isEmpty()) {
      wRow << createItem(0); /* size of URF data without URF Header */
      wRow << new QStandardItem("<empty string>");
      break;
    }

    wRow << createItem(wSize - wString.getURFHeaderSize()); /* size of URF data without URF Header */
    wRow << new QStandardItem(QString((const QChar *)wString.toString()));

    wOffset += wSize ;
    break;
  }
  case ZType_Utf32VaryingString:{
    utf32VaryingString wString;
    utf16VaryingString wAdd;

    ssize_t wSize = wString._importURF(wPtr);
    if (wString.strlen() > StringDiplayMax){
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar32);
      wString += wAdd ;
    }
    if (wString.isEmpty()) {
      wRow << createItem(0); /* size of URF data without URF Header */
      wRow << new QStandardItem("<empty string>");
      break;
    }

    wRow << createItem(wSize - wString.getURFHeaderSize()); /* size of URF data without URF Header */
    wRow << new QStandardItem(QString((const QChar *)wString.toUtf16().toString()));
    break;
  }

  case ZType_Utf8FixedString:{
    utf8VaryingString wString;

    URF_Capacity_type   wCapacity;
    URF_UnitCount_type  wUnitsCount;
    size_t              wStringByteSize;

    wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

    wStringByteSize = size_t (wUnitsCount) * sizeof(utf8_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf8_t* wPtrOut = (utf8_t*)wString.Data;
    utf8_t* wPtrIn = (utf8_t*)wPtr;
    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString += cst_OverflowChar8 ;
    }
    wOffset += sizeof(ZTypeBase)+sizeof(URF_Capacity_type) +sizeof(URF_UnitCount_type)+ wStringByteSize;

    wRow << createItem(wStringByteSize ); /* size of URF data without URF Header */

    if (wUnitsCount == 0)
      wRow << new QStandardItem("<empty string>");
    else
      wRow << new QStandardItem(wString.toCChar());
    break;
  }

    /* for fixed string URF header is different */

  case ZType_Utf16FixedString:{
    utf16VaryingString wString;

    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;


    wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf16_t);

    /* the whole string must be imported, then possibly truncated afterwards to maximum displayable */

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf16_t* wPtrOut = (utf16_t*)wString.Data;
    utf16_t* wPtrIn = (utf16_t*)wPtr;
    while ( wI-- && *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit( cst_OverflowChar16);
    }
    wOffset += sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + wStringByteSize ;

    if (wUnitsCount==0) {
      wRow << createItem(0); /* size of URF data without URF Header */
      wRow << new QStandardItem("<empty string>");
      break;
    }

    wRow << createItem(wStringByteSize); /* size of URF data without URF Header */

    if (wUnitsCount == 0)
      wRow << new QStandardItem("<empty string>");
    else
      wRow << new QStandardItem(QString((const QChar *)wString.toString()));
    break;
  }

  case ZType_Utf32FixedString:{
    utf32VaryingString wString;
    URF_Capacity_type wCapacity;
    URF_UnitCount_type  wUnitsCount;

    wPtr += sizeof(ZTypeBase);

    _importAtomic<URF_Capacity_type>(wCapacity,wPtr);
    _importAtomic<URF_UnitCount_type>(wUnitsCount,wPtr);

    size_t wStringByteSize = size_t (wUnitsCount) * sizeof(utf32_t);

    URF_Capacity_type wI = wUnitsCount;

    wString.allocateUnitsBZero(size_t(wUnitsCount+1));

    utf32_t* wPtrOut = (utf32_t*)wString.Data;
    utf32_t* wPtrIn = (utf32_t*)wPtr;

    while (wI--&& *wPtrIn )
      *wPtrOut++ = *wPtrIn++;

    wPtr = (unsigned char*) wPtrIn;

    if (wUnitsCount > StringDiplayMax) {
      wString.truncate(StringDiplayMax);
      wString.addUtfUnit(cst_OverflowChar32);
    }
    wOffset += sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + wStringByteSize ;

    if (wUnitsCount==0) {
      wRow << createItem(0); /* size of URF data without URF Header */
      wRow << new QStandardItem("<empty string>");
      break;
    }
    wRow << createItem(wStringByteSize); /* size of URF data without URF Header */

    if (wUnitsCount == 0)
      wRow << new QStandardItem("<empty string>");
    else
      wRow << new QStandardItem(QString((const QChar *)wString.toUtf16().toString()));

    break;
  }

  case ZType_CheckSum: {
    checkSum wCheckSum;

    wCheckSum._importURF(wPtr);
    wOffset += wCheckSum.getURFSize();

    wRow << createItem(cst_checksum); /* size of URF data without URF Header */
    wRow << createItem(wCheckSum);
    break;
  }

  case ZType_MD5: {
    md5 wCheckSum;

    wCheckSum._importURF(wPtr);
    wOffset += wCheckSum.getURFSize();

    wRow << createItem(cst_md5); /* size of URF data without URF Header */
    wRow << createItem(wCheckSum);
    break;
  }

  case ZType_Blob: {
    uint64_t wDataSize;
    wPtr += sizeof(ZTypeBase);
    _importAtomic(wDataSize,wPtr);

    wRow << createItem(wDataSize); /* size of blob data */
    wRow << new QStandardItem("Blob content cannot be displayed");

    wPtr += size_t(wDataSize);
    wOffset += size_t(wDataSize) + sizeof(uint64_t);
    break;
  }

  case ZType_bitset: {
    ZBitset wBitset;

    ssize_t wSize=wBitset._importURF(wPtr);
    utf8VaryingString wBS = "<";
    wBS += wBitset.toString();
    wBS += ">";

    wOffset += wBitset.getURFSize();

    wRow << createItem(wSize-ssize_t(getURFHeaderSize(ZType_bitset))); /* size of URF data without URF Header */
    wRow << new QStandardItem(wBS.toCChar());

    break;
  }

  case ZType_bitsetFull: {
    wRow << createItem(0); /* size of URF data without URF Header */
    wRow << new QStandardItem("All bits are set");
    wPtr += sizeof(ZTypeBase);
    wOffset += sizeof(ZTypeBase);
    break;
  }

  case ZType_Resource: {
    ZResource wValue;
    ssize_t wSize=wValue._importURF(wPtr);

    wRow << createItem(wSize-getURFHeaderSize(ZType_Resource)); /* size of URF data without URF Header */
    if (!wValue.isValid()) {
      wRow << new QStandardItem("<Invalid resource>");
    }
    else {
      wStr.sprintf("entity<%s> id<%ld>",decode_ZEntity(wValue.Entity).toChar(),wValue.id);
      wRow << new QStandardItem(wStr.toCChar());
    }

    wOffset += wSize;
    break;
  }

  default: {
    wRow << createItem("---");
    wRow << new QStandardItem("--Unknown data type--");
    wPtr += sizeof(ZTypeBase);
    wOffset += sizeof(ZTypeBase);
    BlockTBv->ItemModel->appendRow(wRow);
    return true;
  }

  }// switch

  BlockTBv->ItemModel->appendRow(wRow);
  return false;
}




void
ZRawMasterFileVisu::colorizeURFBlock(ZDataBuffer & pData)
{
  if (BlockDumpTBv->ItemModel->rowCount()==0) {
    return;
  } // if rows

  DefaultBackGround = BlockDumpTBv->ItemModel->item(0,0)->data(Qt::BackgroundRole);

  ui->ColorsGBx->setVisible(true);

  size_t wColorOffset=0 ;
  uint32_t* wStart= (uint32_t* )pData.Data;
  if (*wStart!=cst_ZFILEBLOCKSTART){
    setSelectionBackGround(WrongBackGround,WrongBackGround,0,sizeof(uint32_t),"Unknown - wrong data block",false);
    return ;
  }

  setSelectionBackGround(BlockHeaderBackGround,BlockHeaderBackGround_first,wColorOffset,sizeof(ZBlockHeader_Export),"ZBlockHeader_Export",false);

  wColorOffset += sizeof(ZBlockHeader_Export) ; // start sign is part of block header export structure

  const unsigned char* wPtr = pData.Data + sizeof(ZBlockHeader_Export) ;
  const unsigned char* wPtrEnd = pData.Data + pData.Size;

  /* colorize presence bitset */

  ZBitset wPresence;


  ssize_t wSize = wPresence._importURF(wPtr);
  if (wSize < 0) {
    return;
  }
  setSelectionBackGround(PresenceBackGround,PresenceBackGround_first,wColorOffset,wSize,"Presence bit field (ZBitset)",false);

  /* colorize urf data size */

  wColorOffset += size_t(wSize);

  setSelectionBackGround(URFSizeBackGround,URFSizeBackGround_first,wColorOffset,sizeof(uint64_t),"leading URF data size",false);
  wColorOffset += sizeof(uint64_t);

  wPtr += sizeof(uint64_t);

  while (wPtr < wPtrEnd ) {
    colorizeOneURFFields(pData,wPtr,wPtrEnd,wColorOffset);
  }

}//colorizeURFBlock

void
ZRawMasterFileVisu::colorizeURFKeyBlock(ZDataBuffer & pData)
{
  if (BlockDumpTBv->ItemModel->rowCount()==0) {
    return;
  } // if rows

  DefaultBackGround = BlockDumpTBv->ItemModel->item(0,0)->data(Qt::BackgroundRole);

  ui->ColorsGBx->setVisible(true);

  size_t wColorOffset=0 , wColorBegin = 0;
  uint32_t* wStart= (uint32_t* )pData.Data;
  if (*wStart!=cst_ZFILEBLOCKSTART){
    setSelectionBackGround(WrongBackGround,WrongBackGround,0,sizeof(uint32_t),"Unknown - wrong data block",false);
    return ;
  }

  setSelectionBackGround(BlockHeaderBackGround,BlockHeaderBackGround_first,wColorOffset,sizeof(ZBlockHeader_Export),"ZBlockHeader_Export",false);

  wColorOffset += sizeof(ZBlockHeader_Export) ; // start sign is part of block header export structure

  const unsigned char* wPtr = pData.Data + sizeof(ZBlockHeader_Export) ;
  const unsigned char* wPtrEnd = pData.Data + pData.Size;

  /* colorize ZMF address */
  while (wPtr < wPtrEnd ) {
    setSelectionBackGround(URFSizeBackGround,URFSizeBackGround_first,wColorOffset,sizeof(zaddress_type),"zmasterfile zmf address",false);
    wColorOffset += sizeof(zaddress_type);

    wPtr += sizeof(zaddress_type);

    colorizeOneURFFields(pData,wPtr,wPtrEnd,wColorOffset);
  }
}//colorizeURFKeyBlock

void ZRawMasterFileVisu::colorizeOneURFFields(const ZDataBuffer& pData,const unsigned char* &pPtr,const unsigned char* pPtrEnd,size_t &pColorOffset) {
  size_t  wColorBegin = 0;
  ZType_type wTrueType;

  ZTypeBase wZType;
  //  size_t    wURFHeaderSize=0;

//  while (pPtr < pPtrEnd ) {

    wColorBegin = pColorOffset;

    _importAtomic<ZTypeBase>(wZType,pPtr);

    wTrueType = (ZType_type) wZType;


    /* for atomic URF data, value is just following ZType. For other types, use _importURF function that implies ZType */

    if (wZType & ZType_Atomic)  {
      wZType &= ~ZType_Atomic;
      pColorOffset += sizeof(ZTypeBase);
    }
    else  {
      pPtr -= sizeof(ZTypeBase);
    }
    switch (wZType) {

    case ZType_UChar:
    case ZType_U8: {
      pColorOffset += sizeof(uint8_t);
      pPtr += sizeof(uint8_t);
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }
    case ZType_Char:
    case ZType_S8: {
      pColorOffset += sizeof(int8_t)+1;
      pPtr += sizeof(int8_t)+1;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }
    case ZType_U16:{
      pColorOffset += sizeof(uint16_t);
      pPtr += sizeof(uint16_t);
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }
    case ZType_S16: {
      pColorOffset += sizeof(int16_t)+1;
      pPtr += sizeof(int16_t)+1;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }

    case ZType_U32:{
      pColorOffset += sizeof(uint32_t);
      pPtr += sizeof(uint32_t);
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }
    case ZType_S32: {
      pColorOffset += sizeof(int32_t)+1;
      pPtr += sizeof(int32_t)+1;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }
    case ZType_U64: {
      pColorOffset += sizeof(uint64_t);
      pPtr += sizeof(uint64_t);
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }
    case ZType_S64: {
      pColorOffset += sizeof(int64_t)+1;
      pPtr += sizeof(int64_t)+1;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }
    case ZType_Float: {
      pColorOffset += sizeof(float)+1;
      pPtr += sizeof(float)+1;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }

    case ZType_Double: {
      pColorOffset += sizeof(double)+1;
      pPtr += sizeof(double)+1;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }

    case ZType_LDouble: {
      pColorOffset += sizeof(long double)+1;
      pPtr += sizeof(long double)+1;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }

      /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */

    case ZType_ZDate: {
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      pColorOffset += sizeof(ZTypeBase) + sizeof(uint32_t);
      pPtr += sizeof(ZTypeBase) + sizeof(uint32_t);
      break;
    }
    case ZType_ZDateFull: {
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      pColorOffset += sizeof(ZTypeBase) + sizeof(uint64_t);
      pPtr += sizeof(ZTypeBase) + sizeof(uint64_t);
      break;
    }

    case ZType_URIString: {
      uriString wStr;
      ssize_t wS=wStr._importURF(pPtr);
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,wStr.getURFHeaderSize() , decode_ZType(wZType) ,false);
      pColorOffset += wS;
      break;
    }
    case ZType_Utf8VaryingString:{
      utf8VaryingString wStr;
      ssize_t wS=wStr._importURF(pPtr);
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,wStr.getURFHeaderSize() , decode_ZType(wZType) ,false);
      pColorOffset += wS;
      break;
    }

    case ZType_Utf16VaryingString:{
      utf16VaryingString wStr;
      ssize_t wS=wStr._importURF(pPtr);
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,wStr.getURFHeaderSize() , decode_ZType(wZType) ,false);
      pColorOffset += wS;

      break;
    }
    case ZType_Utf32VaryingString:{
      utf32VaryingString wStr;
      ssize_t wS=wStr._importURF(pPtr);
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,wStr.getURFHeaderSize() , decode_ZType(wZType) ,false);
      pColorOffset += wS;
      break;
    }

    case ZType_Utf8FixedString:{
      URF_Capacity_type   wCapacity;
      URF_UnitCount_type  wUnitsCount;
      size_t              wStringByteSize;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase)+sizeof(URF_Capacity_type)+sizeof(URF_Capacity_type) , decode_ZType(wZType) ,false);
      pPtr += sizeof(ZTypeBase);
      _importAtomic<URF_Capacity_type>(wCapacity,pPtr);
      _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);
      wStringByteSize = size_t(wUnitsCount) * sizeof(utf8_t);

      pPtr += wStringByteSize ;
      pColorOffset +=  sizeof(ZTypeBase) + size_t(wStringByteSize) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
      break;
    }

      /* for fixed string URF header is different */

    case ZType_Utf16FixedString:{
      URF_Capacity_type wUnitCount;
      URF_UnitCount_type  wUnitsCount;
      size_t              wStringByteSize;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase)+sizeof(URF_Capacity_type)+sizeof(URF_Capacity_type) , decode_ZType(wZType) ,false);
      pPtr += sizeof(ZTypeBase);
      _importAtomic<URF_Capacity_type>(wUnitCount,pPtr);
      _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);
      wStringByteSize = size_t(wUnitsCount) * sizeof(utf16_t);

      pPtr += wStringByteSize ;
      pColorOffset +=  sizeof(ZTypeBase) + size_t(wStringByteSize) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
      break;
    }

    case ZType_Utf32FixedString:{
      URF_Capacity_type wUnitCount;
      URF_UnitCount_type  wUnitsCount;
      size_t              wStringByteSize;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,pColorOffset,sizeof(URF_Capacity_type)+sizeof(URF_Capacity_type) , decode_ZType(wZType) ,false);
      pPtr += sizeof(ZTypeBase);
      _importAtomic<URF_Capacity_type>(wUnitCount,pPtr);
      _importAtomic<URF_UnitCount_type>(wUnitsCount,pPtr);
      wStringByteSize = size_t(wUnitsCount) * sizeof(utf32_t);

      pPtr += wStringByteSize ;
      pColorOffset +=  sizeof(ZTypeBase) + size_t(wStringByteSize) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type);
      break;
    }

    case ZType_CheckSum: {
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      checkSum wCheckSum;
      pPtr+=wCheckSum.getURFSize() ;
      pColorOffset += sizeof(ZTypeBase) + cst_checksum ;

      break;
    }

    case ZType_MD5: {
      md5 wCheckSum;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      pPtr+=wCheckSum.getURFSize() ;
      pColorOffset += sizeof(ZTypeBase) + cst_md5 ;
      break;
    }

    case ZType_Blob: {
      uint64_t wDataSize;
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) + sizeof(uint64_t) , decode_ZType(wZType) ,false);
      pPtr += sizeof(ZTypeBase);
      _importAtomic(wDataSize,pPtr);

      pPtr+=size_t(wDataSize) ;
      pColorOffset += sizeof(ZTypeBase) + size_t(wDataSize)  + sizeof(uint64_t) ;
      break;
    }

    case ZType_bitset: {
      ZBitset wBitset;
      ssize_t wBSSize = wBitset._importURF(pPtr);
      if (wBSSize < 0) {
        ZStatus wSt=searchNextValidZType(pData,(zaddress_type&)pColorOffset,pPtr);
        setSelectionBackGround(WrongBackGround,URFHeaderBackGround_first,wColorBegin,pColorOffset-wColorBegin , decode_ZType(wZType) ,false);
        break;
      }
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,wBitset.getURFHeaderSize() , decode_ZType(wZType) ,false);
      pColorOffset += size_t(wBSSize);
      break;
    }

    case ZType_bitsetFull: {
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,sizeof(ZTypeBase) , decode_ZType(wZType) ,false);
      break;
    }

    case ZType_Resource: {
      ZResource wR;
      ssize_t wS = wR._importURF(pPtr);
      if (wS < 0) {
        ZStatus wSt=searchNextValidZType(pData,(zaddress_type&)pColorOffset,pPtr);
        setSelectionBackGround(WrongBackGround,URFHeaderBackGround_first,wColorBegin,pColorOffset-wColorBegin , decode_ZType(wZType) ,false);
        break;
      }
      setSelectionBackGround(URFHeaderBackGround,URFHeaderBackGround_first,wColorBegin,wR.getURFHeaderSize() , decode_ZType(wZType) ,false);
      pColorOffset += size_t(wS);
      break;
    }

    default: {
      ZStatus wSt=searchNextValidZType(pData,(zaddress_type&)pColorOffset,pPtr);
      setSelectionBackGround(WrongBackGround,WrongBackGround,wColorBegin,pColorOffset-wColorBegin , "Unknown type - wrong data" ,false);
      break;
    }

    }// switch

//  } // while (wPtr < wPtrEnd )


  return ;
}

void ZRawMasterFileVisu::resizeEvent(QResizeEvent* pEvent)
{
  QSize wDlgOld = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (FResizeInitial)
  {
    FResizeInitial=false;
    return;
  }
  /* message zone is only resized in width */
  QRect wMsgPTe = ui->MessagePTe->geometry();
  int wWMargPTe = (wDlgOld.width()-wMsgPTe.width());
  int wMsgPTeWidth=pEvent->size().width() - wWMargPTe;
  ui->MessagePTe->resize(wMsgPTeWidth,wMsgPTe.height());  /* resize only in width */

   /* Table view zone is resized both in width and height */
  QRect wBTBv = BlockTBv->geometry();
//  QRect wBTBv = ui->centerWidgetWDg->geometry();
  int wWMargin = (wDlgOld.width()-wBTBv.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wDlgOld.height() - wBTBv.height();
  int wVH=pEvent->size().height() - wHMargin ;

  BlockTBv->resize(wVW,wVH);
  BlockDumpTBv->resize(wVW,wVH);
//  BlockDumpTBv->setGeometry(BlockTBv->geometry());
/*  wBTBv.setWidth(wVW);
  wBTBv.setHeight(wVH);
  ui->centerWidgetWDg->setGeometry(wBTBv);
*/
}//ZRawMasterFileVisu::resizeEvent



void ZRawMasterFileVisu::visuActionEvent(QAction* pAction) {
  QDialog wVisuDLg (nullptr);
  wVisuDLg.setWindowTitle(QObject::tr("Evaluate values","ZContentVisuMain"));

  wVisuDLg.resize(450,150);

  QVBoxLayout* QVL=new QVBoxLayout(&wVisuDLg);
  wVisuDLg.setLayout(QVL);

  QGridLayout* QGLyt=new QGridLayout(&wVisuDLg);
  QVL->insertLayout(0,QGLyt);

  QHBoxLayout* QHL=new QHBoxLayout(&wVisuDLg);
  QGLyt->addLayout(QHL,0,4);
  QHL->setAlignment(Qt::AlignCenter);

  QLabel* LBlType=new QLabel("Type",&wVisuDLg);
  QHL->addWidget(LBlType);
  QLabel* wTypeLBl=new QLabel(&wVisuDLg);
  QHL->addWidget(wTypeLBl);


  QLabel* wLbDec=new QLabel(QObject::tr("Decimal","ZContentVisuMain"),&wVisuDLg);
  QGLyt->addWidget(wLbDec,1,1);
  QLabel* wLbHexa=new QLabel(QObject::tr("Hexa","ZContentVisuMain"),&wVisuDLg);
  QGLyt->addWidget(wLbHexa,1,4);

  QLabel* wLb=new QLabel(QObject::tr("Raw","ZContentVisuMain"),&wVisuDLg);
  wLb->setAlignment(Qt::AlignCenter);
  QGLyt->addWidget(wLb,2,0);

  QLineEdit* wRawValueLEd=new QLineEdit(&wVisuDLg);
  wRawValueLEd->setAlignment(Qt::AlignRight);
  QGLyt->addWidget(wRawValueLEd,2,1);

  QLineEdit* wRawHexaLEd=new QLineEdit(&wVisuDLg);
  wRawHexaLEd->setAlignment(Qt::AlignRight);
  QGLyt->addWidget(wRawHexaLEd,2,4);

  QLabel* wLb1=new QLabel(QObject::tr("Deserialized","ZContentVisuMain"),&wVisuDLg);
  QGLyt->addWidget(wLb1,3,0);
  wLb1->setAlignment(Qt::AlignCenter);
  QLineEdit* wDeserializedLEd=new QLineEdit(&wVisuDLg);
  wDeserializedLEd->setAlignment(Qt::AlignRight);
  QGLyt->addWidget(wDeserializedLEd,3,1);

  QLineEdit* wDeserializedHexaLEd=new QLineEdit(&wVisuDLg);
  wDeserializedHexaLEd->setAlignment(Qt::AlignRight);
  QGLyt->addWidget(wDeserializedHexaLEd,3,4);

  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QHLBtn->setObjectName("QHLBtn");
  QVL->insertLayout(1,QHLBtn);

  QLineEdit* wAlphaLEd=new QLineEdit(&wVisuDLg);
  QVL->insertWidget(2,wAlphaLEd);



  QPushButton* wNext=new QPushButton(QObject::tr("Next","ZContentVisuMain"),&wVisuDLg);
  QPushButton* wClose=new QPushButton(QObject::tr("Close","ZContentVisuMain"),&wVisuDLg);
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHLBtn->addItem(wSpacer);

  QHLBtn->addWidget(wNext);
  QHLBtn->addWidget(wClose);




  QObject::connect(wNext, &QPushButton::clicked, &wVisuDLg, &QDialog::accept);
  QObject::connect(wClose, &QPushButton::clicked, &wVisuDLg, &QDialog::reject);

  size_t wValueSize=0;
  QModelIndex wIdx=BlockDumpTBv->currentIndex();
  if (!wIdx.isValid())
    return;

  ssize_t wOffset=computeOffsetFromCoord(wIdx.row(),wIdx.column());


  utf8VaryingString wStr;

  if (pAction==ZTypeQAc) {
    wValueSize=sizeof(ZTypeBase);
    if (wOffset+sizeof(ZTypeBase) > RawRecord.Size )
      return;
    wTypeLBl->setText("ZType");
    ZTypeBase * wValuePtr = (ZTypeBase *)(RawRecord.Data + wOffset);
    ZTypeBase wValue = *wValuePtr;
    ZTypeBase wDeSerialized = reverseByteOrder_Conditional<ZTypeBase>(wValue);
    wStr.sprintf("%u",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%d",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
    wAlphaLEd->setText(decode_ZType(wDeSerialized));

  }

  if (pAction==uint16QAc) {
    wValueSize=sizeof(uint16_t);
    if (wOffset+sizeof(uint16_t) > RawRecord.Size )
      return;
    wTypeLBl->setText("uint16");
    uint16_t * wValuePtr = (uint16_t *)(RawRecord.Data + wOffset);
    uint16_t wValue = *wValuePtr;
    uint16_t wDeSerialized = reverseByteOrder_Conditional<uint16_t>(wValue);
    wStr.sprintf("%u",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%d",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  }
  if (pAction==int16QAc) {
    wValueSize=sizeof(int16_t);
    if (wOffset+sizeof(int16_t) > RawRecord.Size )
      return;
    wTypeLBl->setText("int16");
    int16_t * wValuePtr = (int16_t *)(RawRecord.Data + wOffset);
    int16_t wValue = *wValuePtr;
    int16_t wDeSerialized = reverseByteOrder_Conditional<int16_t>(wValue);
    wStr.sprintf("%|d",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%|d",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  }
  if (pAction==uint32QAc) {
    wValueSize=sizeof(uint32_t);
    if (wOffset+sizeof(uint32_t) > RawRecord.Size )
      return;
    wTypeLBl->setText("uint32");
    uint32_t * wValuePtr = (uint32_t *)(RawRecord.Data + wOffset);
    uint32_t wValue = *wValuePtr;
    uint32_t wDeSerialized = reverseByteOrder_Conditional<uint32_t>(wValue);
    wStr.sprintf("%|u",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%|u",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  }
  if (pAction==int32QAc) {
    wValueSize=sizeof(int32_t);
    if (wOffset+sizeof(int32_t) > RawRecord.Size )
      return;
    wTypeLBl->setText("int32");
    int32_t * wValuePtr = (int32_t *)(RawRecord.Data + wOffset);
    int32_t wValue = *wValuePtr;
    int32_t wDeSerialized = reverseByteOrder_Conditional<int32_t>(wValue);
    wStr.sprintf("%|d",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%|d",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  }
  if (pAction==uint64QAc) {
    wValueSize=sizeof(uint64_t);
    if (wOffset+sizeof(uint64_t) > RawRecord.Size )
      return;
    wTypeLBl->setText("uint64");
    uint64_t * wValuePtr = (uint64_t *)(RawRecord.Data + wOffset);
    uint64_t wValue = *wValuePtr;
    uint64_t wDeSerialized = reverseByteOrder_Conditional<uint64_t>(wValue);
    wStr.sprintf("%|lu",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%|lu",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  }
  if (pAction==int64QAc) {
    wValueSize=sizeof(int64_t);
    if (wOffset+sizeof(int64_t) > RawRecord.Size )
      return;
    wTypeLBl->setText("int64");
    int64_t * wValuePtr = (int64_t *)(RawRecord.Data + wOffset);
    int64_t wValue = *wValuePtr;
    int64_t wDeSerialized = reverseByteOrder_Conditional<int64_t>(wValue);
    wStr.sprintf("%|ld",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%|ld",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  }
  if (pAction==sizetQAc) {
    wValueSize=sizeof(size_t);
    if (wOffset+sizeof(size_t) > RawRecord.Size )
      return;
    wTypeLBl->setText("size_t");
    size_t * wValuePtr = (size_t *)(RawRecord.Data + wOffset);
    size_t wValue = *wValuePtr;
    size_t wDeSerialized = reverseByteOrder_Conditional<size_t>(wValue);
    wStr.sprintf("%|lu",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    if (sizeof(size_t) > 4) {
      wStr.sprintf("%|lu",wValue);
      wRawValueLEd->setText(wStr.toCChar());
      wStr.sprintf("%016lX",wValue);
      wRawHexaLEd->setText(wStr.toCChar());
      wStr.sprintf("%|lu",wDeSerialized);
      wDeserializedLEd->setText(wStr.toCChar());
      wStr.sprintf("%016lX",wDeSerialized);
      wDeserializedHexaLEd->setText(wStr.toCChar());
    }
    else {
      wStr.sprintf("%u",wValue);
      wRawValueLEd->setText(wStr.toCChar());
      wStr.sprintf("%08X",wValue);
      wRawHexaLEd->setText(wStr.toCChar());
      wStr.sprintf("%u",wDeSerialized);
      wDeserializedLEd->setText(wStr.toCChar());
      wStr.sprintf("%08X",wDeSerialized);
      wDeserializedHexaLEd->setText(wStr.toCChar());
    }
  }
  int wRet=wVisuDLg.exec();

  if (wRet==QDialog::Rejected) {
    return;
  }
  /* skip value in offset */

  if ( (wOffset + wValueSize) > RawRecord.Size )
    return;
  //  setSearchOffset(wOffset+wValueSize);
  VisuLineCol wNewPosition;
  wOffset+=wValueSize;
  wNewPosition.compute (wOffset);
  QModelIndex wNewIdx = wIdx.sibling(wNewPosition.line,wNewPosition.col);
  if (!wNewIdx.isValid())
    abort();
/*
  wStr.sprintf("%ld",wOffset);
  ui->CurAddressLBl->setText(wStr.toCChar());
*/
  BlockDumpTBv->setFocus();
  BlockDumpTBv->setCurrentIndex(wNewIdx);
//  BlockDumpTBv->scrollTo(wNewIdx);

  return;
}


void ZRawMasterFileVisu::VisuBvFlexMenuCallback(QContextMenuEvent *event)
{
  QMenu* visuFlexMEn=new QMenu;
  visuFlexMEn->setTitle(QObject::tr("Evaluate","ZRawMasterFileVisu"));

  QActionGroup* visuActionGroup=new QActionGroup(visuFlexMEn) ;
  //  QObject::connect(visuActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(visuActionEvent(QAction*)));
  QObject::connect(visuActionGroup, &QActionGroup::triggered,  this, &ZRawMasterFileVisu::visuActionEvent);

  ZTypeQAc = new QAction("ZType",visuFlexMEn);
  visuFlexMEn->addAction(ZTypeQAc);
  visuActionGroup->addAction(ZTypeQAc);

  uint16QAc= new QAction("uint16",visuFlexMEn);
  visuFlexMEn->addAction(uint16QAc);
  visuActionGroup->addAction(uint16QAc);

  int16QAc= new QAction("int16",visuFlexMEn);
  visuFlexMEn->addAction(int16QAc);
  visuActionGroup->addAction(int16QAc);

  uint32QAc= new QAction("uint32",visuFlexMEn);
  visuFlexMEn->addAction(uint32QAc);
  visuActionGroup->addAction(uint32QAc);

  int32QAc= new QAction("int32",visuFlexMEn);
  visuFlexMEn->addAction(int32QAc);
  visuActionGroup->addAction(int32QAc);

  uint64QAc= new QAction("uint64",visuFlexMEn);
  visuFlexMEn->addAction(uint64QAc);
  visuActionGroup->addAction(uint64QAc);

  int64QAc= new QAction("int64",visuFlexMEn);
  visuFlexMEn->addAction(int64QAc);
  visuActionGroup->addAction(int64QAc);

  sizetQAc= new QAction("size_t",visuFlexMEn);
  visuFlexMEn->addAction(sizetQAc);
  visuActionGroup->addAction(sizetQAc);

  visuFlexMEn->exec(event->globalPos());
  visuFlexMEn->deleteLater();
}//VisuBvFlexMenu

void ZRawMasterFileVisu::setVisuIndexFile() {
  ui->ColorModeCBx->setCurrentIndex(2);
}
