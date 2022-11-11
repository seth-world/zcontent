#include "zrawmasterfilevisu.h"
#include "ui_zrawmasterfilevisu.h"

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

const long StringDiplayMax = 64;

ZRawMasterFileVisu::ZRawMasterFileVisu( QWidget *parent) :
                                                          QDialog(parent),
                                                          ui(new Ui::ZRawMasterFileVisu)
{
  ui->setupUi(this);

  ui->BottomLBl->setVisible(false);
  ui->TopLBl->setVisible(false);

  ui->ViewModeCBx->addItem(QObject::tr("Raw view","ZRawMasterFileVisu"));
  ui->ViewModeCBx->addItem(QObject::tr("URF Fields","ZRawMasterFileVisu"));

  ui->ViewModeCBx->setCurrentIndex(1);

  QFont wVisuFont ("Monospace");
  ui->BitsetHeaderLBl->setFont(wVisuFont);
  ui->BitsetContentLBl->setFont(wVisuFont);


  QObject::connect(ui->BackwardBTn, SIGNAL(clicked(bool)), this, SLOT(Backward()));
  QObject::connect(ui->ForwardBTn, SIGNAL(clicked(bool)), this, SLOT(Forward()));
  QObject::connect(ui->BeginBTn, SIGNAL(clicked(bool)), this, SLOT(ToBegin()));
  QObject::connect(ui->EndBTn, SIGNAL(clicked(bool)), this, SLOT(ToEnd()));
  QObject::connect(ui->ViewModeCBx, SIGNAL(currentIndexChanged(int)), this, SLOT(ViewModeChange(int)));

}
ZRawMasterFileVisu::~ZRawMasterFileVisu()
{
  delete ui;
}

ZStatus
ZRawMasterFileVisu::setup(const uriString& pURI , int pFd) {
  utf8VaryingString wStr;
  if (pFd < -1)
    return ZS_FILENOTOPEN;
  if (!pURI.exists())
    return ZS_FILENOTEXIST;

  Fd = pFd;

  FileSize=size_t(pURI.getFileSize());
  wStr.sprintf("%ld",FileSize);

  ui->FileSizeLBl->setText(wStr.toCChar());
  AllFileLoaded=false;

  wStr.sprintf("%d",BlockRank);
  ui->BlockNbLBl->setText(wStr.toCChar()) ;

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
  BlockList.clear();

  BlockTBv = new ZQTableView(this);
  BlockTBv->newModel(5);
  BlockTBv->setGeometry(10,190,660,320);

  //  BlockTBv->setContextMenuCallback(std::bind(&ZRawMasterFileVisu::VisuBvFlexMenuCallback, this,placeholders::_1));
  //  VisuTBv->setMouseClickCallback(std::bind(&ZContentVisuMain::VisuMouseCallback, this,placeholders::_1,placeholders::_2)  );

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

  Forward();

  return ZS_SUCCESS;
} // setup


ZStatus
ZRawMasterFileVisu::displayRawBlock(ZDataBuffer& pData) {

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
ZRawMasterFileVisu::searchNextValidZType( ZDataBuffer& pRecord,
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
}// searchStartSign

/** pAddress is the file offset that corresponds to wRecord.Data */
ZStatus
ZRawMasterFileVisu::searchStartSign(ZDataBuffer& wRecord,zaddress_type & pAddress) {
  ZStatus wSt=ZS_SUCCESS;
  unsigned char* wPtr = wRecord.Data;
  unsigned char* wPtrEnd = wRecord.Data + wRecord.Size ;

  uint32_t* wStartSign = (uint32_t*)wPtr;
  zaddress_type wAddress = pAddress;
  while ((wAddress < zaddress_type(FileSize)) && (wPtr < wPtrEnd) && (wSt==ZS_SUCCESS)) {

    while ((*wStartSign!=cst_ZFILEBLOCKSTART) && (wPtr < wPtrEnd)) {
      wStartSign = (uint32_t*)wPtr;
      wAddress ++;
      wPtr++;
    }
    if (*wStartSign==cst_ZFILEBLOCKSTART)
      break;

    /* here wPtr == wPtrEnd */
    ssize_t wSize;
    if (BlockList.count()>0)
      wSize = BlockList.last().BlockSize;
    else {
      wSize = 10000;
    }
    if (wAddress == zaddress_type(FileSize)) {
//      utf8VaryingString wStr;
//      wStr.sprintf("No start block mark found. File surface exhausted.");
      ui->MessagePTe->appendPlainText("No start block mark found. File surface exhausted.");
      return ZS_NOTFOUND;
    }
    wAddress -= 3;
    wSt=seekAndGet(wRecord,wSize,wAddress);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    wPtr = wRecord.Data;
    wPtrEnd = wRecord.Data + wRecord.Size ;
  } // while wAddress

  if (wAddress != pAddress){
    ZDataBuffer wNewBuffer;
    size_t wNewSize = wPtrEnd-wPtr;

    wNewBuffer.allocate(wNewSize);
    wNewBuffer.setData(wPtr,wNewSize);

    wRecord.setData(wNewBuffer);
  }
  pAddress = wAddress;

  return ZS_SUCCESS;
}// searchStartSign


ZStatus
ZRawMasterFileVisu::getNextBlock(ZDataBuffer & wRecord,ssize_t wSize,zaddress_type & wAddress) {

  ZStatus wSt = seekAndGet(wRecord,wSize,wAddress);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  /* search next startblock */

  return  searchStartSign(wRecord,wAddress) ;
}// getNextBlock


void
ZRawMasterFileVisu::Forward() {
  ZStatus wSt;
  ssize_t wSize;
  ZDataBuffer wRecord;
  zaddress_type wAddress=0;

  ui->TopLBl->setVisible(false);

  if (FileOffset >= off_t(FileSize)) {
    ui->BottomLBl->setVisible(true);
    return;
  }

  if (BlockList.count() > 0) {
    FileOffset = BlockList[BlockRank].Address + BlockList[BlockRank].BlockSize ;
    if (FileOffset == off_t(FileSize)) {
      ui->BottomLBl->setVisible(true);
      ui->TopLBl->setVisible(false);
      return;
    }
    if ((BlockRank >= 0) && (BlockRank < (BlockList.count() - 2) )) {
      ++BlockRank;
      displayBlock(BlockList[BlockRank]);
      return;
    }
    wAddress = BlockList.last().Address + BlockList.last().BlockSize;
    if (wAddress >= zaddress_type(FileSize)) {
      ui->BottomLBl->setVisible(true);
      ui->TopLBl->setVisible(false);
    }

    wSize = ssize_t(BlockList.last().BlockSize);
  }
  else
    wSize = 1000; /* if no block registered yet, then used 1000 as hopefully value */

  /* need to access a block that is not already stored */

  wSt=getNextBlock(wRecord,wSize,wAddress);
  if (wSt!=ZS_SUCCESS){
    if (wSt==ZS_EOF) {
      ui->BottomLBl->setVisible(true);
      ui->TopLBl->setVisible(false);
      return;
    }
    ZExceptionDLg::messageWAdd("ZRawMasterFileVisu::Forward",wSt,Severity_Severe,
        ZException.last().formatUtf8(),
        "Cannot get next block on file %s,",URICurrent.toCChar());
    return;
  }

  if (wRecord.Size < sizeof(ZBlockHeader_Export)) {
    wSize = sizeof(ZBlockHeader_Export);
    wSt = seekAndGet(wRecord,wSize,wAddress);
    if (wSt!=ZS_SUCCESS) {
      if (wSt==ZS_EOF) {
        ui->BottomLBl->setVisible(true);
        ui->TopLBl->setVisible(false);
        return;
      }
      return;
    } //if (wSt!=ZS_SUCCESS)
  }

  memmove (&BlockCur,wRecord.Data,sizeof(ZBlockHeader_Export));

  BlockCur.deserialize();
  BlockCur.Address = wAddress;

  BlockList.push(BlockCur);
  BlockRank = BlockList.lastIdx();

  ui->TopLBl->setVisible(false);

  displayBlock(BlockCur);
  return;
} // Forward




void
ZRawMasterFileVisu::Backward(){
  if (BlockRank == 0) {
    ui->BottomLBl->setVisible(false);
    ui->TopLBl->setVisible(true);
    return;
  }
  --BlockRank;
  ui->BottomLBl->setVisible(false);
  displayBlock(BlockList[BlockRank]);
  return;
}

void
ZRawMasterFileVisu::ToEnd(){

  ZDataBuffer wRecord;
  ssize_t wSize=0;
  ZStatus wSt=ZS_SUCCESS;

  ui->TopLBl->setVisible(false);

  FileOffset = 0; /* reset file offset to beginning */

  if (BlockList.count()>0) {
    FileOffset = off_t(BlockList.last().Address);
    if ((FileOffset+BlockList.last().BlockSize) >= FileSize) {  /* all blocks have been already stored */
      BlockCur = BlockList.last();
      displayBlock(BlockCur);
      return;
    }
    /* here blocks have been partially stored. FileOffset points to beginning of last read block */
    FileOffset +=  off_t(BlockList.last().BlockSize); /* let's point to theorical beginning of next block */
  }

  wSize = sizeof(ZBlockHeader_Export);
  wSt=getNextBlock(wRecord,wSize,FileOffset);
  if (wSt!=ZS_SUCCESS){
    if (wSt==ZS_EOF) {
      ui->BottomLBl->setVisible(true);
      ui->TopLBl->setVisible(false);
      return;
    }
    ZExceptionDLg::messageWAdd("ZRawMasterFileVisu::Forward",wSt,Severity_Severe,
        ZException.last().formatUtf8(),
        "Cannot get next block on file %s,",URICurrent.toCChar());
    return;
  } //if (wSt!=ZS_SUCCESS)

  while (wSt==ZS_SUCCESS) {
    if (wRecord.Size < sizeof(ZBlockHeader_Export)) {
      wSize = sizeof(ZBlockHeader_Export);
      wSt = seekAndGet(wRecord,wSize,FileOffset);
      if (wSt!=ZS_SUCCESS) {
        if (wSt==ZS_EOF) {
          ui->BottomLBl->setVisible(true);
          ui->TopLBl->setVisible(false);
          return;
        }
        return;
      } //if (wSt!=ZS_SUCCESS)
    }
    memmove (&BlockCur,wRecord.Data,sizeof(ZBlockHeader_Export));
    BlockCur.deserialize();
    BlockCur.Address = zaddress_type(FileOffset);

    BlockList.push(BlockCur);
    BlockRank = BlockList.lastIdx();

    if ((BlockCur.BlockSize+BlockCur.Address) >= FileSize)
      break;

    FileOffset += off_t(BlockCur.BlockSize);

    wSt=getNextBlock(wRecord,wSize,FileOffset);
    if (wSt!=ZS_SUCCESS){
      if (wSt==ZS_EOF) {
        ui->BottomLBl->setVisible(true);
        ui->TopLBl->setVisible(false);
        return;
      }
      ZExceptionDLg::messageWAdd("ZRawMasterFileVisu::Forward",wSt,Severity_Severe,
          ZException.last().formatUtf8(),
          "Cannot get next block on file %s,",URICurrent.toCChar());
      return;
    }
  }//while (wSt==ZS_SUCCESS)
  if (BlockList.count() > 0) {
    displayBlock(BlockCur);  /* BlockCur contains the last accessed Block */
  }

}

void
ZRawMasterFileVisu::ToBegin(){
  BlockRank = 0;
  ui->BottomLBl->setVisible(false);
  displayBlock(BlockList[BlockRank]);
  return;
}
template <class _Tp>
void setQLabel(QLabel* pLabel,_Tp pValue) {
  utf8VaryingString wStr;
  if (sizeof(_Tp)>4)
    wStr.sprintf("%ld",pValue);
  else
    wStr.sprintf("%d",pValue);
  pLabel->setText(wStr.toCChar());
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
  if (pIndex==0) {
    BlockTBv->setVisible(false);
    BlockDumpTBv->setVisible(true);
    Raw=true;

    ui->URFSizeLabelLBl->setVisible(false);
    ui->URFSizeLBl->setVisible(false);
    ui->FieldPresenceLabelLBl->setVisible(false);
    ui->BitsetHeaderLBl->setVisible(false);
    ui->BitsetContentLBl->setVisible(false);

    if (BlockRank > -1)
      displayBlock(BlockList[BlockRank]);
    return;
  }
  BlockTBv->setVisible(true);
  BlockDumpTBv->setVisible(false);
  Raw=false;

  ui->URFSizeLabelLBl->setVisible(true);
  ui->URFSizeLBl->setVisible(true);
  ui->FieldPresenceLabelLBl->setVisible(true);
  ui->BitsetHeaderLBl->setVisible(true);
  ui->BitsetContentLBl->setVisible(true);

  if (BlockRank > -1)
    displayBlock(BlockList[BlockRank]);
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
  ZDataBuffer wRecord;
  utf8VaryingString wStr;
  QList<QStandardItem*> wRow;
  wRecord.allocate(pBlock.BlockSize);

  ZStatus wSt = seekAndRead(wRecord,pBlock);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  setQLabel(ui->BeginLBl,pBlock.Address);
  setQLabel<zaddress_type>(ui->EndLBl,zaddress_type(pBlock.Address+pBlock.BlockSize));
  setQLabel<size_t>(ui->FileSizeLBl,FileSize);

  ui->StateLBl->setText(decode_ZBS(pBlock.State));
  setQLabel<long>(ui->BlockNbLBl,BlockRank);

  if (Raw) {
    if (BlockTBv->isVisible()) {
      BlockTBv->setVisible(false);
      BlockDumpTBv->setVisible(true);
    }

    return displayRawBlock(wRecord);
  }
  else {
    if (!BlockTBv->isVisible()) {
      BlockTBv->setVisible(true);
      BlockDumpTBv->setVisible(false);
    }
  return displayURFBlock(wRecord);
  }
}//displayBlock


ZStatus
ZRawMasterFileVisu::displayURFBlock(ZDataBuffer & pData)
{
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

  ZTypeBase wZType;
  size_t    wURFHeaderSize=0;
  uint64_t  wURFDataSize = 0;

  int wErrored=0;
  bool wZTypeErrored=false;

  /* first get bit set */

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



  while ((wPtr < wPtrEnd )&&(wErrored < 10)) {

    wZTypeErrored=false;

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

      wRow << createItem(sizeof(int8_t));
      wStr.sprintf("%d 0x%X",wValue,wValue);
      wRow << createItem(wStr);
      wOffset += sizeof(int8_t);
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

      wRow << createItem(sizeof(int16_t));
      wStr.sprintf("%d 0x%X",wValue,wValue);
      wRow << createItem(wStr);
      wOffset += sizeof(int16_t);
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

      wRow << createItem(sizeof(int32_t));

      wStr.sprintf("%d 0x%X",wValue,wValue);
      wRow << createItem(wStr);
      wOffset += sizeof(int32_t);
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

      wRow << createItem(sizeof(int64_t));

      wStr.sprintf("%lld 0x%X",wValue,wValue);
      wRow << createItem(wStr);
      wOffset += sizeof(int64_t);
      break;
    }
    case ZType_Float: {
      float wValue;
      wValue=convertAtomicBack<float> (ZType_S64,wPtr);
      wRow << createItem(sizeof(float));

      wRow << createItem(wValue,"%g");
      wOffset += sizeof(float);
      break;
    }

    case ZType_Double: {
      double wValue;
      wValue=convertAtomicBack<double> (ZType_Double,wPtr);

      wRow << createItem(sizeof(double));

      wRow << createItem(wValue,"%g");
      wOffset += sizeof(double);
      break;
    }

    case ZType_LDouble: {
      long double wValue;
      wValue=convertAtomicBack<long double> (ZType_S64,wPtr);

      wRow << createItem(sizeof(long double));

      wRow << createItem(wValue,"%g");
      wOffset += sizeof(long double);
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
      wRow << createItem(wSize - getURFHeaderSize(ZType_URIString)); /* size of URF data without URF Header */
      wRow << new QStandardItem(wString.toCChar());

      wOffset += wSize ;
      break;
    }

    case ZType_Utf8VaryingString:{
      utf8VaryingString wString;
      ssize_t wSize = wString._importURF(wPtr);

      if (wString.strlen() > StringDiplayMax){
        wString.truncate(StringDiplayMax);
        wString += cst_OverflowChar8 ;
      }
      wRow << createItem(wSize - getURFHeaderSize(ZType_Utf8VaryingString)); /* size of URF data without URF Header */
      wRow << new QStandardItem(wString.toCChar());

      wOffset += wSize ;
      break;
    }

    case ZType_Utf16VaryingString:{
      utf16VaryingString wString;
      utf16VaryingString wAdd;

      ssize_t wSize = wString._importURF(wPtr);
      if (wString.strlen() > StringDiplayMax){
        wString.truncate(StringDiplayMax);
        wString.addUtfUnit(cst_OverflowChar16);
        wString += wAdd ;
      }
      wRow << createItem(wSize - ssize_t(getURFHeaderSize(ZType_Utf8VaryingString))); /* size of URF data without URF Header */

      wRow << new QStandardItem(QString((const QChar *)wString.toString()));

      wOffset += wSize ;
      break;
    }
    case ZType_Utf32VaryingString:{
      utf32VaryingString wString;
      utf16VaryingString wAdd;

      ssize_t wSize=wString._importURF(wPtr);
      if (wString.strlen() > StringDiplayMax){
        wString.truncate(StringDiplayMax);
        wString.addUtfUnit(cst_OverflowChar16);
        wString += wAdd ;
      }
      wRow << createItem(wSize - ssize_t(getURFHeaderSize(ZType_Utf8VaryingString))); /* size of URF data without URF Header */

      wRow << new QStandardItem(QString((const QChar *)wString.toUtf16().toString()));
      break;
    }

    case ZType_Utf8FixedString:{
      utf8VaryingString wString;

      URF_Capacity_type wUnitCount;
      URF_Fixed_Size_type wStringByteSize;

      _importAtomic<URF_Capacity_type>(wUnitCount,wPtr);
      _importAtomic<URF_Fixed_Size_type>(wStringByteSize,wPtr);

      URF_Capacity_type wI = wUnitCount;

      wString.allocateUnitsBZero(size_t(wUnitCount+1));

      utf8_t* wPtrOut = (utf8_t*)wString.Data;
      utf8_t* wPtrIn = (utf8_t*)wPtr;
      while (wI--&& *wPtrIn )
        *wPtrOut++ = *wPtrIn++;

      wPtr = (unsigned char*) wPtrIn;

      if (wUnitCount > StringDiplayMax) {
        wString.truncate(StringDiplayMax);
        wString += cst_OverflowChar8 ;
      }
      wOffset += getURFHeaderSize(ZType_Utf8FixedString) + (wUnitCount * sizeof(utf8_t));


      wRow << createItem(wStringByteSize ); /* size of URF data without URF Header */
      wRow << new QStandardItem(wString.toCChar());
      break;
    }

      /* for fixed string URF header is different */

    case ZType_Utf16FixedString:{
      utf16VaryingString wString;

      URF_Capacity_type wUnitCount;
      URF_Fixed_Size_type wStringByteSize;
      _importAtomic<URF_Capacity_type>(wUnitCount,wPtr);
      _importAtomic<URF_Fixed_Size_type>(wStringByteSize,wPtr);

      /* the whole string must be imported, then possibly truncated afterwards to maximum displayable */

      URF_Capacity_type wI = wUnitCount;

      wString.allocateUnitsBZero(size_t(wUnitCount+1));

      utf16_t* wPtrOut = (utf16_t*)wString.Data;
      utf16_t* wPtrIn = (utf16_t*)wPtr;
      while (wI--&& *wPtrIn )
        *wPtrOut++ = *wPtrIn++;

      wPtr = (unsigned char*) wPtrIn;

      if (wUnitCount > StringDiplayMax) {
        wString.truncate(StringDiplayMax);
        wString.addUtfUnit( cst_OverflowChar16);
      }
      wOffset += getURFHeaderSize(ZType_Utf16FixedString) + (wUnitCount * sizeof(utf16_t));

      wRow << createItem(wStringByteSize); /* size of URF data without URF Header */
      wRow << new QStandardItem(QString((const QChar *)wString.toString()));
      break;
    }

    case ZType_Utf32FixedString:{
      utf32VaryingString wString;
      URF_Capacity_type wUnitCount;
      URF_Fixed_Size_type wStringByteSize;
      _importAtomic<URF_Capacity_type>(wUnitCount,wPtr);
      _importAtomic<URF_Fixed_Size_type>(wStringByteSize,wPtr);

      URF_Capacity_type wI = wUnitCount;

      wString.allocateUnitsBZero(size_t(wUnitCount+1));

      utf32_t* wPtrOut = (utf32_t*)wString.Data;
      utf32_t* wPtrIn = (utf32_t*)wPtr;

      while (wI--&& *wPtrIn )
        *wPtrOut++ = *wPtrIn++;

      wPtr = (unsigned char*) wPtrIn;

      if (wUnitCount > StringDiplayMax) {
        wString.truncate(StringDiplayMax);
        wString.addUtfUnit(cst_OverflowChar32);
      }
      wOffset += getURFHeaderSize(ZType_Utf32FixedString) + size_t(wStringByteSize);

      wRow << createItem(wStringByteSize); /* size of URF data without URF Header */
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
        wStr.sprintf("%s-%ld",decode_ZEntity(wValue.Entity).toChar(),wValue.id);
        wRow << new QStandardItem(wStr.toCChar());
      }

      wOffset += wSize;
      break;
    }

    default: {
      wErrored ++;
      wZTypeErrored=true;
      wRow << createItem("---");
      wRow << new QStandardItem("--Unknown data type--");
      wPtr += sizeof(ZTypeBase);
      wOffset += sizeof(ZTypeBase);
      break;
    }

    }// switch

    BlockTBv->ItemModel->appendRow(wRow);

    if (wZTypeErrored && (wErrored < 10) ) {
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
}//displayURFBlock

bool
ZTypeExists(ZTypeBase pType) {
  if (pType & ZType_Atomic)
    pType &= ~ZType_Atomic;

  switch (pType) {
  case ZType_Char:
  case ZType_UChar:
  case ZType_U8:
  case ZType_S8:
  case ZType_U16:
  case ZType_S16:

  case ZType_U32:
  case ZType_S32:
  case ZType_U64:
  case ZType_S64:
  case ZType_Float:
  case ZType_Double:
  case ZType_LDouble:
    return true;
    /* from here <wPtr -= sizeof(ZTypeBase);>  has been made and wPtr points on ZType */

  case ZType_ZDate:
  case ZType_ZDateFull:

  case ZType_Utf8VaryingString:

  case ZType_Utf16VaryingString:
  case ZType_Utf32VaryingString:

  case ZType_Utf8FixedString:

  case ZType_Utf16FixedString:

  case ZType_Utf32FixedString:

  case ZType_CheckSum:
  case ZType_MD5:

  case ZType_Resource:

  case ZType_bitset:
  case ZType_bitsetFull:
    return true;

  default:
    return false;

  }// switch

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
/*  wBTBv.setWidth(wVW);
  wBTBv.setHeight(wVH);
  ui->centerWidgetWDg->setGeometry(wBTBv);
*/
}//ZContentVisuMain::resizeEvent
