#include "zscan.h"
#include "ui_zscan.h"
#include <stdio.h>
#include <fcntl.h>

#include <ztoolset/zexceptionmin.h>
#include <zio/zioutils.h>

#include "zexceptiondlg.h"

ZScan::ZScan(QWidget *parent) :
                                QMainWindow(parent),
                                ui(new Ui::ZScan)
{
  ui->setupUi(this);


  setWindowTitle("surface scan");

  ui->tableTBw->setColumnCount(5);
  ui->tableTBw->setShowGrid(true);
  ui->tableTBw->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->tableTBw->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  ui->tableTBw->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Physical address")));
  ui->tableTBw->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("State")));
  ui->tableTBw->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Block size")));
  ui->tableTBw->setHorizontalHeaderItem(3,new QTableWidgetItem(tr("User content size")));
  ui->tableTBw->setHorizontalHeaderItem(4,new QTableWidgetItem(tr("Theorical next")));
}

ZScan::~ZScan()
{
  close();
  delete ui;
}

void
ZScan::setFile(const uriString& pFile)
{
  if (FileDescriptor>=0)
    rawClose(FileDescriptor);
  Currentfile=pFile;
  FileSize=Currentfile.getFileSize();

  ZStatus wSt=rawOpen(FileDescriptor,Currentfile.toCChar(),O_RDONLY);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Open file error");
    return;
  }

  removeAllRows();
}

void
ZScan::close()
{
  if (FileDescriptor>-1)
  {
    rawClose(FileDescriptor);
    FileDescriptor=-1;
  }
}



void
ZScan::removeAllRows()
{
  if (ui->tableTBw->rowCount() > 0)
    {
      ui->tableTBw->clearContents();
      ui->tableTBw->setRowCount(0);
    }
}

int cst_MaxRows=50;


void
ZScan::scanMain()
{
  ZStatus wSt;
  ZBlockHeader wBlockHeader;
  zaddress_type wAddress=0;
  zaddress_type wTheoricalAddress=0;

  zaddress_type wOldAddress=0;
  long          wOldSize=0;

  int wTick=0;
  utf8String wStr;
  int wRow=0;
  int wCol=0;

  ui->FilePathLBl->setText(Currentfile.toCChar());

  ui->FileSizeLBl->setText(formatSize(FileSize).toCChar());
  ui->SizeReadLBl->setText("0");

  ui->ProgressPGb->setRange(0,FileSize);

  ui->ProgressPGb->setVisible(true);
  ui->ProgressPGb->setTextVisible(true);
  ui->ProgressPGb->setValue(0);

  ui->tableTBw->setRowCount(cst_MaxRows);

//"Physical address    |   State       | Block size         | User content size  | Theorical next Add\n"

  wSt=searchNextStartMark(FileDescriptor,0L,wBlockHeader,wAddress);

  while((wSt==ZS_FOUND)&&(wRow < cst_MaxRows))
    {
      wTheoricalAddress = wOldAddress+wOldSize;
      if (wAddress!=wTheoricalAddress)          /* hole encountered */
        {
        wCol=0;
        wStr.sprintf("%8X",wTheoricalAddress);
        ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));

        ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem("---<hole>----"));
        wStr.sprintf("%6ld",wAddress-wTheoricalAddress);
        ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));

        ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(""));
        ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(""));
        wRow++;
        HolesCount++;
        HolesSize += wAddress-wTheoricalAddress;
        }


    if(wBlockHeader.State==ZBS_Used)
        {
        UsedCount++;
        UsedSize += wBlockHeader.BlockSize;
        }
        else
        if (wBlockHeader.State==ZBS_Free)
        {
          FreeCount++;
          FreeSize += wBlockHeader.BlockSize;
        }
        else
        if (wBlockHeader.State==ZBS_Deleted)
          {
            DeletedCount++;
            DeletedSize += wBlockHeader.BlockSize;
          }

      wOldSize=wBlockHeader.BlockSize;
      wCol=0;
      wStr.sprintf("%8X",wAddress);
      ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));

      ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(decode_ZBS(wBlockHeader.State)));
      wStr.sprintf("%6ld (0x%8X)",wBlockHeader.BlockSize,wBlockHeader.BlockSize);
      ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));
      size_t wSize= wBlockHeader.BlockSize-(long)sizeof(ZBlockHeader_Export);
      wStr.sprintf("%6ld (0x%8X)",wSize,wSize);
      ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));

      wStr.sprintf("%8X",wAddress+wBlockHeader.BlockSize);
      ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));
      wAddress += sizeof(wBlockHeader);
      wRow++;

      if (++wTick > 10)
        {
        wTick=0;
        ui->ProgressPGb->setValue(wAddress);
        ui->SizeReadLBl->setText(formatSize( wAddress+sizeof(ZBlockHeader)).toCChar());
        wStr.sprintf("%ld",UsedCount);
        ui->UsedCountLBl->setText(wStr.toCChar());
        ui->UsedSizeLBl->setText(formatSize( UsedSize).toCChar());
        wStr.sprintf("%ld",DeletedCount);
        ui->DeletedCountLBl->setText(wStr.toCChar());
        ui->DeletedSizeLBl->setText(formatSize( DeletedSize).toCChar());
        wStr.sprintf("%ld",HolesCount);
        ui->HolesCountLBl->setText(wStr.toCChar());
        ui->HolesSizeLBl->setText(formatSize( HolesSize).toCChar());
        wStr.sprintf("%ld",FreeCount);
        ui->FreeCountLBl->setText(wStr.toCChar());
        ui->FreeSizeLBl->setText(formatSize( FreeSize).toCChar());
        }//if (++wTick > 10)


      wSt=searchNextStartMark(FileDescriptor,wAddress,wBlockHeader,wAddress);
    }

  ui->ProgressPGb->setValue(wAddress);
  ui->SizeReadLBl->setText(formatSize( wAddress+sizeof(ZBlockHeader)).toCChar());
  wStr.sprintf("%ld",UsedCount);
  ui->UsedCountLBl->setText(wStr.toCChar());
  ui->UsedSizeLBl->setText(formatSize( UsedSize).toCChar());
  wStr.sprintf("%ld",DeletedCount);
  ui->DeletedCountLBl->setText(wStr.toCChar());
  ui->DeletedSizeLBl->setText(formatSize( DeletedSize).toCChar());
  wStr.sprintf("%ld",HolesCount);
  ui->HolesCountLBl->setText(wStr.toCChar());
  ui->HolesSizeLBl->setText(formatSize( HolesSize).toCChar());
  wStr.sprintf("%ld",FreeCount);
  ui->FreeCountLBl->setText(wStr.toCChar());
  ui->FreeSizeLBl->setText(formatSize( FreeSize).toCChar());


  if (wRow < cst_MaxRows)
    ui->tableTBw->setRowCount(wRow);

  for (long wi=0;wi < ui->tableTBw->model()->columnCount();wi++)
    ui->tableTBw->resizeColumnToContents(wi);
  for (long wi=0;wi < ui->tableTBw->model()->rowCount();wi++)
    ui->tableTBw->resizeRowsToContents();

}//ZScan::scanMain


void
ZScan::displayOneBlock(zaddress_type pAddress,ZBlockHeader& pBlockHeader)
{
  int wRow=0;
  int wCol=0;
  utf8String wStr;
  //            "Physical address    |   State       | Block size         | User content size  | Theorical next Add\n"

  wStr.sprintf("%8X",pAddress);
  ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));

  ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(decode_ZBS(pBlockHeader.State)));
  wStr.sprintf("%6ld",pBlockHeader.BlockSize);
  ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));

  wStr.sprintf("%6ld",pBlockHeader.BlockSize-(long)sizeof(ZBlockHeader_Export));
  ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));

  wStr.sprintf("%8X",pAddress+pBlockHeader.BlockSize);
  ui->tableTBw->setItem(wRow,wCol++,new QTableWidgetItem(wStr.toCChar()));

  return;
}


ZStatus
ZScan::searchFirstStartMark(__FILEHANDLE__ pFd, ZBlockHeader& pBlockHeader, zaddress_type &pNextAddress)
{
  return searchNextStartMark(pFd,0,pBlockHeader,pNextAddress);
}

ZStatus
ZScan::searchNextStartMark(__FILEHANDLE__ pFd,
                          const zaddress_type pAddress,
                          ZBlockHeader& pBlockHeader,
                          zaddress_type &pNextAddress)
{
  ZStatus         wSt;
  ssize_t         wSizeToRead=0;
  size_t          wAdvanceQuota;
  zaddress_type   wAddress;
  ZDataBuffer     wBuffer;
  off_t           wSeekRet;
  long            wSizeRead=0;

  wAdvanceQuota = sizeof(ZBlockHeader_Export) + 512 ; // user content target size * 2 must be OK
  wSizeToRead = sizeof(ZBlockHeader_Export) + 512 ; // user content target size * 2 must be OK

  unsigned char wStartSign [5]={cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE,cst_ZSTART_BYTE , 0 };


  wAddress = pAddress;

  wBuffer.allocateBZero(wSizeToRead);
  zaddress_type wOffset= -1;

  if (wAddress > 3)/* assume that wStartSign is just truncated at the end of wBuffer (max will be 3 bytes) */
  {
    wSt=rawSeekToRelative(pFd,-3,(size_t&)wAddress);
  }
  else {
    wSt=rawSeekBegin(pFd);
    wAddress = 0;
  }
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wSt=rawRead(pFd,wBuffer,wSizeToRead);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wOffset=wBuffer.bsearch(wStartSign,4L,0L);
  while ((wSt==ZS_SUCCESS)&&(wBuffer.Size>0)&&(wOffset==-1)) {
      wAddress += wBuffer.Size;
      wSt=rawRead(pFd,wBuffer,wSizeToRead);
      if ((wSt!=ZS_SUCCESS)||(wBuffer.Size < size_t(wSizeToRead)))
        break;
      wOffset=wBuffer.bsearch(wStartSign,4L,0L);
    }// while

  if (wSt==ZS_EOF) {
      pNextAddress = -1;
      return  (ZS_EOF);
    }
  if (wSt != ZS_SUCCESS)
    {
    return wSt;
    }
  if (wSizeRead < wSizeToRead)  /* read is partial till end of file */
  {
    if (wOffset<0)
    {
      pNextAddress =-1;   /* Not found until EOF */
      return  (ZS_EOF);
    }
    pNextAddress = wAddress + wOffset;

    /* loaded enough for getting block ? */
    if((wBuffer.Size-wOffset)< sizeof(ZBlockHeader_Export))
      {
      ZDataBuffer wAdd (sizeof(ZBlockHeader_Export)+wBuffer.Size+1-wOffset);
      }

    wSt=pBlockHeader._importConvert(pBlockHeader,(ZBlockHeader_Export*)&wBuffer.DataChar[wOffset]);
    if (wSt!=ZS_SUCCESS)
          { return (wSt); }
    return  (ZS_FOUND) ;
  }//if (wSizeRead<wSizeToRead)


  /* loaded enough for getting block ? */
  if((wBuffer.Size-wOffset)< sizeof(ZBlockHeader_Export)) {
    /* no : get complementary data */
    size_t wAddSize = sizeof(ZBlockHeader_Export)+wBuffer.Size+1-wOffset;
    ZDataBuffer wAdd ;
    wAdd.allocateBZero(wAddSize);
    wSt=rawRead(pFd,wAdd,wAddSize);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    if (wAdd.Size < wAdd.Size) {
      /* in this case block header is truncated by EOF mark : this is an error */
      ZException.getErrno(errno,"ZContentVisuMain::searchNextStartMark",
          ZS_CORRUPTED,Severity_Severe,
          "Block is truncated by EOF for file <%s>",Currentfile.toCChar());
      return ZS_CORRUPTED;
    }
    wBuffer.appendData(wAdd);
  }

  pNextAddress = wAddress + wOffset;
  wSt=pBlockHeader._importConvert(pBlockHeader,(ZBlockHeader_Export*)&wBuffer.DataChar[wOffset]);
  if (wSt!=ZS_SUCCESS)
        { return (wSt); }
  return  (ZS_FOUND) ;

}//searchNextStartMark
