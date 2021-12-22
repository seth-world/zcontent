#include "zcontentvisumain.h"
#include "ui_zcontentvisumain.h"
#include <QStandardItemModel>
#include <qaction.h>
#include <qactiongroup.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include <zio/zdir.h>
#include <stdio.h>
#include <fcntl.h>

#include <zcontent/zrandomfile/zrandomfile.h>
#include <zcontent/zindexedfile/zsmasterfile.h>

#include "displaymain.h"
#include "zscan.h"
#include "texteditmwn.h"

#include "zexceptiondlg.h"

#define __FIXED_FONT__ "courrier"

using namespace zbs;

ZContentVisuMain::ZContentVisuMain(QWidget *parent) :QMainWindow(parent),
                                                     ui(new Ui::ZContentVisuMain)
{
  ui->setupUi(this);

  setWindowTitle("File dump and explore");

  ui->tableTBw->setColumnCount(2);
  ui->tableTBw->setShowGrid(true);
  ui->tableTBw->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->tableTBw->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  ui->tableTBw->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Hexadecimal")));
  ui->tableTBw->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Ascii")));

  actionGroup = new QActionGroup(this);

  actionGroup->addAction(ui->setfileQAc); /* set the current file to be open */

  actionGroup->addAction(ui->openQAc);  /* open according file extension */

  actionGroup->addAction(ui->openRawQAc);/* force open as raw file */
  actionGroup->addAction(ui->openZRFQAc);/* force open as ZRF file */
  actionGroup->addAction(ui->openZMFQAc);/* force open as ZMF file */

  actionGroup->addAction(ui->surfaceScanZRFQAc); /* surface scan ZRF file */
   actionGroup->addAction(ui->surfaceScanRawQAc); /* surface scan raw file */

  actionGroup->addAction(ui->closeQAc);

  actionGroup->addAction(ui->QuitQAc);

  actionGroup->addAction(ui->displayHCBQAc);
  actionGroup->addAction(ui->displayFCBQAc);
  actionGroup->addAction(ui->displayMCBQAc);
  actionGroup->addAction(ui->displayPoolQAc);



  actionGroup->addAction(ui->HeaderRawUnlockQAc);

  actionGroup->addAction(ui->unlockZRFQAc);

  actionGroup->addAction(ui->clearQAc);
  actionGroup->addAction(ui->cloneQAc);
  actionGroup->addAction(ui->extendQAc);
  actionGroup->addAction(ui->truncateQAc);
  actionGroup->addAction(ui->reorganizeQAc);
  actionGroup->addAction(ui->upgradeZRFtoZMFQAc);
  actionGroup->addAction(ui->rebuildHeaderQAc);

/* ZMFMEn */
  actionGroup->addAction(ui->runRepairQAc); /* repair all indexes */
  actionGroup->addAction(ui->removeIndexQAc);
  actionGroup->addAction(ui->addIndexQAc);      /* adds an index from an xml definition */
  actionGroup->addAction(ui->downgradeZMFtoZRFQAc);
  actionGroup->addAction(ui->rebuildIndexQAc);      /* rebuild single index */
  actionGroup->addAction(ui->extractIndexQAc);      /* extract xml index definition */
  actionGroup->addAction(ui->extractAllIndexesQAc); /* extract all xml index definitions */
  actionGroup->addAction(ui->reorganizeZMFQAc);
  actionGroup->addAction(ui->MCBReportQAc);         /* list Master Control Block */


  ui->openQAc->setVisible(false);
  ui->openRawQAc->setVisible(false);
  ui->openZRFQAc->setVisible(false);
  ui->openZMFQAc->setVisible(false);
  ui->unlockZRFQAc->setVisible(false);
  ui->closeQAc->setVisible(false);

  ui->rawMEn->setEnabled(false);
  ui->ZRFMEn->setEnabled(false);
  ui->ZMFMEn->setEnabled(false);



  ui->RecordFRm->setVisible(false);
  ui->SequentialFRm->setVisible(false);

  ui->displayHCBQAc->setEnabled(false);
  ui->displayMCBQAc->setEnabled(false);
  ui->displayFCBQAc->setEnabled(false);
  ui->displayPoolQAc->setEnabled(false);

  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(false);
  ui->LoadAllBTn->setVisible(false);

  ui->ProgressPGb->setVisible(false);

  QObject::connect(ui->BackwardBTn, SIGNAL(pressed()), this, SLOT(backward()));
  QObject::connect(ui->ForwardBTn, SIGNAL(pressed()), this, SLOT(forward()));
  QObject::connect(ui->LoadAllBTn, SIGNAL(pressed()), this, SLOT(loadAll()));

  QObject::connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(actionEvent(QAction*)));

}

ZContentVisuMain::~ZContentVisuMain()
{
  if (entityWnd)
    delete entityWnd;

  if (RandomFile)
    if (RandomFile->isOpen())
      {
        RandomFile->zclose();
        delete RandomFile;
      }
  if (MasterFile)
    {
      if (MasterFile->isOpen())
        MasterFile->zclose();
      delete MasterFile;
    }
  if (Fd>=0)
    ::close(Fd);

  delete ui;
}


void
ZContentVisuMain::actionEvent(QAction* pAction)
{
  ZStatus wSt=ZS_SUCCESS;


  if (pAction==ui->openQAc)
  {
    actionOpenFileByType();
    ui->OpenModeLbl->setText("ZRF");
    return;
  }
  if (pAction==ui->openRawQAc)
  {
    openRaw();
    ui->OpenModeLbl->setText("Raw file");
    return;
  }
  if (pAction==ui->openZRFQAc)
  {
    wSt=openZRF(URICurrent.toCChar());
    if ((wSt==ZS_SUCCESS)||(wSt==ZS_FILETYPEWARN))
    {

      removeAllRows();
//      displayListZRFFirstRecord(Width);
      return;
    }
    ui->OpenModeLbl->setText("Random file");
    return;
  }
  if (pAction==ui->openZMFQAc)
  {
    OpenMode = VMD_Master;
    removeAllRows();
    wSt=openZMF(URICurrent.toCChar());
    ui->OpenModeLbl->setText("Master file");
    return;
  }

  if (pAction==ui->unlockZRFQAc)
  {
    unlockZRFZMF(URICurrent.toCChar());
    return;
  }

  if (pAction==ui->setfileQAc)
  {
    chooseFile(true);
    return;
  }

  if (pAction==ui->ZRFRecordsQac)
  {
    ui->OpenModeLbl->setText("By record");
    ui->BackwardBTn->setVisible(true);
    ui->ForwardBTn->setVisible(true);
    return;
  }
  if (pAction==ui->unlockZRFQAc)
  {
    ZRFUnlock();
    return;
  }
  if (pAction==ui->HeaderRawUnlockQAc)
  {
    ZHeaderRawUnlock();
    return;
  }
  if (pAction==ui->closeQAc)
  {
    OpenMode = VMD_Nothing;
    ui->BackwardBTn->setVisible(false);
    ui->ForwardBTn->setVisible(false);
    actionClose();
    return;
  }
  if (pAction==ui->displayHCBQAc)
  {
    displayHeaderControlBlock();
    return;
  }
  if (pAction==ui->displayFCBQAc)
  {
    displayFCB();
    return;
  }
  if (pAction==ui->displayMCBQAc)
  {
    displayMCB();
    return;
  }
  if (pAction==ui->displayPoolQAc)
  {
    displayPool();
    return;
  }
  if (pAction==ui->surfaceScanZRFQAc)
  {
    surfaceScanZRF();
    return;
  }
  if (pAction==ui->surfaceScanRawQAc)
  {
    surfaceScanRaw();
    return;
  }

  if (pAction==ui->QuitQAc)
  {
    actionClose();
    //    this->deleteLater();
    QApplication::quit();
    return;
  }


  /* ZMFMEn */

  if (pAction==ui->runRepairQAc)
  {
    repairIndexes(ui->testRunQAc->isChecked(), ui->rebuilAllQAc->isChecked());
    return;
  }
  if (pAction==ui->MCBReportQAc)
  {
    reportMCB();
    return;
  }

  return;

}//actionEvent

void ZContentVisuMain::reportMCB()
{
  FILE* wReportLog = fopen("reportMCB.log","w");
  MasterFile->report(wReportLog);
  fflush(wReportLog);
  fclose(wReportLog);

  textEditMWn* wTextWin=new textEditMWn;

  wTextWin->setTextFromFile("reportMCB.log");
  wTextWin->show();
}


void ZContentVisuMain::repairIndexes(bool pTestRun,bool pRebuildAll)
{
  ZStatus wSt=ZS_SUCCESS;

  FILE* wRepairLog = fopen("repairindex.log","w");
  wSt=MasterFile->zrepairIndexes( URICurrent.toCChar(),
                                  pTestRun,
                                  pRebuildAll,
                                  wRepairLog);
  fflush(wRepairLog);
  fclose(wRepairLog);

  textEditMWn* wTextWin=new textEditMWn;

  wTextWin->setTextFromFile("repairindex.log");
  wTextWin->show();

  if (wSt==ZS_SUCCESS)
    return;

  ZExceptionDLg::displayLast();
  return;
}

void ZContentVisuMain::removeIndex()
{

}

void ZContentVisuMain::addIndex()
{

}

void ZContentVisuMain::rebuildIndex()
{

}

void
ZContentVisuMain::extractIndex()
{

}

void
ZContentVisuMain::dowdgrade()
{

}
void
ZContentVisuMain::reorganizeZMF()
{

}

void
ZContentVisuMain::ZRFUnlock()
{
  if (OpenMode != VMD_Random)
  {
    QMessageBox::critical(this,tr("Operation error"),"Cannot unlock : file must be open as random file.");
    return;
  }
  ZStatus wSt=ZRandomFile::zutilityUnlockZRF(URICurrent);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::display(*ZException.popR());
  }
}
void
ZContentVisuMain::ZHeaderRawUnlock()
{
  if (OpenMode != VMD_RawSequential)
  {
    QMessageBox::critical(this,tr("Operation error"),"Cannot unlock : header file must be open as raw file.");
    return;
  }
  ZStatus wSt=ZRandomFile::zutilityUnlockHeaderFile(URICurrent);
  if (wSt!=ZS_SUCCESS)
  {
  ZExceptionDLg::display(*ZException.popR());
  }
}

void
ZContentVisuMain::actionOpenFileByType(bool pChecked)
{
  ZStatus wSt;
  QList<QString> wNameFilters;
  wNameFilters << "*.zrf";
  wNameFilters << "*.zmf";


  QFileDialog *FileSelectionDLg = new QFileDialog(this);

  FileSelectionDLg->setNameFilters(wNameFilters);
  FileSelectionDLg->setFileMode(QFileDialog::ExistingFile);
  FileSelectionDLg->setAcceptMode(QFileDialog::AcceptOpen);

  FileSelectionDLg->setDirectory("/home/gerard/Development/zmftest");


  FileSelectionDLg->setLabelText(QFileDialog::Accept,  "Select");
  FileSelectionDLg->setLabelText(QFileDialog::Reject ,  "Cancel");



  while (true)
  {
    if (FileSelectionDLg->exec()==QDialog::Rejected)
    {
      FileSelectionDLg->deleteLater();
      return;
    }
    if (FileSelectionDLg->selectedFiles().isEmpty())
    {
      QMessageBox::critical(this,tr("No file selected"),"Please select a valid file");
    }
    else
    {

      URICurrent = FileSelectionDLg->selectedFiles()[0].toUtf8().data();
      if (URICurrent.getFileExtension()=="zrf")
      {
        if (ZS_SUCCESS==openZRF(URICurrent.toCChar()))
        {
          removeAllRows();
//          displayListZRFFirstRecord(Width);
          break;
        }

      }
      else
          if (URICurrent.getFileExtension()=="zmf")
      {
        ui->FileTypeLBl->setText("Master file");
        wSt=openZMF(URICurrent.toCChar());
        if ((wSt==ZS_SUCCESS)||(wSt==ZS_FILETYPEWARN))
        {
          removeAllRows();
//          displayListZRFFirstRecord(Width);
          break;
        }

      }
      else
      {
        if (ZS_SUCCESS==openOther(URICurrent.toCChar()))
        {
          removeAllRows();
          ui->LoadAllBTn->setVisible(true);
//          displayFdNextRawBlock(BlockSize,Width);
          break;
        }
//        QMessageBox::critical(this,tr("Random File open error"),ZException.formatFullUserMessage().toCChar());
        ZExceptionDLg::display(*ZException.popR());
      }
    }//else
    }//while (true)

  setFileType (FileSelectionDLg->selectedFiles()[0].toUtf8().data());
  ui->FullPathLbl->setText(FileSelectionDLg->selectedFiles()[0]);

  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(true);

  ui->displayHCBQAc->setEnabled(false);


  return;
}

void
ZContentVisuMain::chooseFile(bool pChecked)
{
  QList<QString> wNameFilters;
  wNameFilters << "*.*";

  QFileDialog *wFileSelectionDLg = new QFileDialog(this);

  wFileSelectionDLg->setNameFilters(wNameFilters);
  wFileSelectionDLg->setFileMode(QFileDialog::ExistingFile);
  wFileSelectionDLg->setAcceptMode(QFileDialog::AcceptOpen);

  wFileSelectionDLg->setDirectory("/home/gerard/Development/zmftest");

  wFileSelectionDLg->setLabelText(QFileDialog::Accept,  "Select");
  wFileSelectionDLg->setLabelText(QFileDialog::Reject ,  "Cancel");

  while (true)
  {
    if (wFileSelectionDLg->exec()==QDialog::Rejected)
    {
      wFileSelectionDLg->deleteLater();
      return;
    }
    if (wFileSelectionDLg->selectedFiles().isEmpty())
    {
      QMessageBox::critical(this,tr("No file selected"),"Please select a valid file");
    }
    else
      break;
    }//while (true)

  setFileType (wFileSelectionDLg->selectedFiles()[0].toUtf8().data());

  wFileSelectionDLg->deleteLater();

  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(true);

  ui->RecordFRm->setVisible(false);
  ui->SequentialFRm->setVisible(true);

  return;
}//chooseFile

void
ZContentVisuMain::openRaw()
{

  if (openOther(URICurrent.toCChar())!= ZS_SUCCESS)
    {
    QMessageBox::critical(this,tr("Random File open error"),ZException.formatFullUserMessage().toCChar());
    return;
    }

  removeAllRows();
//  displayFdNextRawBlock(BlockSize,16);

  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(true);

  ui->RecordFRm->setVisible(false);
  ui->SequentialFRm->setVisible(true);


  ui->rawMEn->setEnabled(true);

  ui->openQAc->setVisible(false);
  ui->openRawQAc->setVisible(false);
  ui->openZRFQAc->setVisible(false);
  ui->openZMFQAc->setVisible(false);
  ui->unlockZRFQAc->setVisible(false);

  return;
}//actionOpenFileAsRaw



ZStatus
ZContentVisuMain::openZRF(const char*pFilePath)
{
utf8String wStr;
ZStatus wSt=ZS_SUCCESS;

  if (RandomFile)
  {
    if (RandomFile->isOpen())
      RandomFile->zclose();
  }
  else
    RandomFile=new ZRandomFile;

  wSt = RandomFile->zopen(pFilePath,ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS)
    {
    if (wSt==ZS_FILETYPEWARN)
      ZExceptionDLg::displayLast(true);
    else
      {
      delete RandomFile;
      RandomFile=nullptr;
      ZExceptionDLg::displayLast(false);
      return wSt;
      }
    }

  RawData.clear();
  getFileSize(pFilePath);

  if (Fd>=0)
  {
    ::close(Fd);
    Fd=-1;
  }

  if (MasterFile)
  {
    if (MasterFile->isOpen())
      MasterFile->zclose();
    delete MasterFile;
    MasterFile=nullptr;
  }
  FileOffset=0;
  RecordNumber=0;

  ui->SequentialFRm->setVisible(false);
  ui->RecordFRm->setVisible(true);

  wStr.sprintf("%ld",RandomFile->getFileDescriptor().ZBAT.count());
  ui->RecordTotalLBl->setText(wStr.toCChar());

  ui->LoadAllBTn->setVisible(false);
  ui->BackwardBTn->setVisible(false);
  if (RandomFile->getFileDescriptor().ZBAT.count() > 1)
    ui->BackwardBTn->setVisible(true);
  else
    ui->BackwardBTn->setVisible(false);

  ui->OpenModeLbl->setText("Random file mode");
  OpenMode = VMD_Random;

  ui->openQAc->setVisible(false);
  ui->openRawQAc->setVisible(false);
  ui->openZRFQAc->setVisible(false);
  ui->openZMFQAc->setVisible(false);

   ui->FileTypeLBl->setText("Random file");

   ui->rawMEn->setEnabled(false);
   ui->ZRFMEn->setEnabled(true);
   ui->ZMFMEn->setEnabled(false);

  return ZS_SUCCESS;
//  return  displayListZRFFirstRecord(Width);
}//openZRF

ZStatus
ZContentVisuMain::openZMF(const char* pFilePath)
{
utf8String wStr;
ZStatus wSt=ZS_SUCCESS;

  if (MasterFile)
    {
    if (MasterFile->isOpen())
      MasterFile->zclose();
    }
  else
    MasterFile=new ZSMasterFile;

  wSt= MasterFile->zopen(pFilePath,ZRF_Read_Only);
  if (wSt!=ZS_SUCCESS)
  {
    if (wSt==ZS_FILETYPEWARN)
      ZExceptionDLg::displayLast(true);
    else
    {
      delete MasterFile;
      MasterFile=nullptr;
      ZExceptionDLg::displayLast(false);
      return wSt;
    }
  }

  RawData.clear();
  getFileSize(pFilePath);

  if (Fd>=0)
    {
    ::close(Fd);
    Fd=-1;
    }

  if (RandomFile)
  {
    if (RandomFile->isOpen())
      RandomFile->zclose();
    delete RandomFile;
    RandomFile=nullptr;
  }
  FileOffset=0;
  RecordNumber=0;

  wStr.sprintf("%ld",MasterFile->getFileDescriptor().ZBAT.count());
  ui->RecordTotalLBl->setText(wStr.toCChar());

  ui->LoadAllBTn->setVisible(false);
  ui->BackwardBTn->setVisible(false);
  if (MasterFile->getFileDescriptor().ZBAT.count() > 1)
    ui->BackwardBTn->setVisible(true);
  else
    ui->BackwardBTn->setVisible(false);

   ui->OpenModeLbl->setText("Master file mode");
   OpenMode = VMD_Master;

   ui->openQAc->setVisible(false);
   ui->openRawQAc->setVisible(false);
   ui->openZRFQAc->setVisible(false);
   ui->openZMFQAc->setVisible(false);

   ui->rawMEn->setEnabled(false);
   ui->ZRFMEn->setEnabled(false);
   ui->ZMFMEn->setEnabled(true);

  return ZS_SUCCESS;
} // openZMF


ZStatus
ZContentVisuMain::unlockZRFZMF(const char* pFilePath)
{

  ZStatus wSt=  ZRandomFile::zutilityUnlockZRF(pFilePath);

  if (wSt!=ZS_SUCCESS)
    ZExceptionDLg::display(*ZException.popR());
  else
    ZExceptionDLg::message("ZContentVisuMain::unlockZRFZMF",ZS_SUCCESS,Severity_Information,"File <%s> has been successfully unlocked.",pFilePath);
  return wSt;
}//unlockZRFZMF




utf8String
formatSize(long long wSize)
{
  utf8String wSizeStr;
  double wSizeD = (double)wSize;

  const char* wUnit= "B";
  if (wSizeD > 1024.0)
  {
    wSizeD= wSizeD / 1024.0;
    if (wSizeD > 1024.0)
    {
      wSizeD= wSizeD / 1024.0;
      if (wSizeD > 1024.0)
        {
        wSizeD= wSizeD / 1024.0;
        wUnit="gB";
        }
      else
        wUnit="mB";
    }
    else
      wUnit="kB";
  }
  wSizeStr.sprintf("%g %s",wSizeD,wUnit);
  return wSizeStr;
}

void
ZContentVisuMain::getFileSize(const char* pFilePath)
{
  uriString wFile = pFilePath;  
  ui->FileSizeLBl->setText(formatSize(wFile.getFileSize()).toCChar());
  ui->FullSizeLBl->setText(formatSize(wFile.getFileSize()).toCChar());
}


ZStatus
ZContentVisuMain::openOther(const char* pFileName)
{
  ZStatus wSt=ZS_SUCCESS;
  uriString wFilePath = pFileName;

  Fd = open(wFilePath.toCChar(),O_RDONLY);       // open content file for read only

  if (Fd < 0)
    {
    ZException.getErrno(errno,
              _GET_FUNCTION_NAME_,
              ZS_ERROPEN,
              Severity_Severe,
              " Error opening file %s ",
              wFilePath.toCChar());
    return  ZS_ERROPEN;
    }

  RawData.clear();
  ui->LoadAllBTn->setVisible(true);
  getFileSize(pFileName);

  if (MasterFile)
    {
    if (MasterFile->isOpen())
      MasterFile->zclose();
    delete MasterFile;
    MasterFile=nullptr;
    }

  if (RandomFile)
    {
      if (RandomFile->isOpen())
        RandomFile->zclose();
      delete RandomFile;
      RandomFile=nullptr;
    }

  AllFileLoaded=false;
  FileOffset=0;
  RecordNumber=0;

  ui->SequentialFRm->setVisible(true);
  ui->RecordFRm->setVisible(false);

  ui->LoadAllBTn->setVisible(true);

  ui->displayHCBQAc->setEnabled(true);
  ui->displayMCBQAc->setEnabled(true);
  ui->displayFCBQAc->setEnabled(true);
  ui->displayPoolQAc->setEnabled(true);

  ui->OpenModeLbl->setText("Raw mode");

  OpenMode = VMD_RawSequential;

  ui->rawMEn->setEnabled(true);
  ui->ZRFMEn->setEnabled(false);
  ui->ZMFMEn->setEnabled(false);


  return ZS_SUCCESS;
}//openOther

void
ZContentVisuMain::displayHeaderControlBlock()
{
  if (!entityWnd)
    entityWnd=new DisplayMain(this);
  entityWnd->displayHCB(RawData);
}

void
ZContentVisuMain::displayFCB()
{
  if (!entityWnd)
    entityWnd=new DisplayMain(this);
  entityWnd->displayFCB(RawData);
}
void
ZContentVisuMain::displayMCB()
{
  if (!entityWnd)
      entityWnd=new DisplayMain(this);
  entityWnd->displayMCB(RawData);
}
void
ZContentVisuMain::displayPool()
{
  if (!entityWnd)
    entityWnd=new DisplayMain(this);
  entityWnd->displayPool(RawData);
}


void
ZContentVisuMain::setFileType (const char* pFilePath)
{

  URICurrent = pFilePath;
  ui->FullPathLbl->setText(pFilePath);

  utf8String wExt=URICurrent.getFileExtension();

  while (true)
  {
  if (wExt=="zrf")
    {
    ui->FileTypeLBl->setText("Random file");
    break;
    }
    if (wExt=="zmf")
    {
      ui->FileTypeLBl->setText("Master file");
      break;
    }
    if (wExt=="zix")
    {
      ui->FileTypeLBl->setText("Index file");
      break;
    }
    if (wExt=="zrh")
    {
      ui->FileTypeLBl->setText("Header file");
      break;
    }
    ui->FileTypeLBl->setText("Unmanaged file type");
    break;
  }//while true


  ui->openQAc->setVisible(true);
  ui->openRawQAc->setVisible(true);
  ui->openZRFQAc->setVisible(true);
  ui->openZMFQAc->setVisible(true);
  ui->unlockZRFQAc->setVisible(true);
  ui->closeQAc->setVisible(true);

//  ui->actionMEn->setEnabled(true);
}

void
ZContentVisuMain::actionClose(bool pChecked)
{
  displayWidgetBlockOnce=false;
  if (RandomFile)
    {
    RandomFile->zclose();
    delete RandomFile;
    RandomFile=nullptr;
    }

  if (MasterFile)
    {
      MasterFile->zclose();
      delete MasterFile;
      MasterFile=nullptr;
    }
  if (Fd >= 0)
      {
      ::close(Fd);
      Fd=-1;
      }
    ui->BackwardBTn->setVisible(false);
    ui->ForwardBTn->setVisible(false);

    ui->rawMEn->setEnabled(false);
    ui->ZRFMEn->setEnabled(false);
    ui->ZMFMEn->setEnabled(false);

    ui->openQAc->setVisible(true);
    ui->openRawQAc->setVisible(true);
    ui->openZRFQAc->setVisible(true);
    ui->openZMFQAc->setVisible(true);

    ui->OpenModeLbl->setText("not open");
    OpenMode=VMD_Nothing;

  return;
}



ZStatus
ZContentVisuMain::displayFdNextRawBlock(ssize_t pBlockSize,size_t pWidth)
{
  ZDataBuffer wRecord;
  wRecord.allocate(pBlockSize);
  int wS=read(Fd,wRecord.Data,pBlockSize);
  if (wS < 0)
    {
    ZException.getErrno(errno,"displayFdOneRawBlock",ZS_FILEERROR,Severity_Error,
        "Error reading file <%s>",URICurrent.toCChar());
    return ZS_FILEERROR;
    }
  if (wS==0)
    {
      return ZS_EOF;
      AllFileLoaded=true;
      ui->ForwardBTn->setVisible(false);
    }
  if (wS < pBlockSize)
    wRecord.truncate(wS);

  RawData.appendData(wRecord);

  ui->ReadSizeLBl->setText(formatSize(RawData.Size).toCChar());

  return displayWidgetBlock(wRecord,pWidth);
}//displayFdNextRawBlock

ZStatus
ZContentVisuMain::displayWidgetBlock(ZDataBuffer& pData,int pWidth)
{
  ui->ProgressPGb->setRange(0,pData.Size);
  ui->ProgressPGb->setValue(0);
  ui->ProgressPGb->setVisible(true);

  /* how many lines within pData block */

  int wLines=pData.Size / pWidth;

  int wRem=pData.Size - ( wLines * pWidth);

  if (wRem > 0)
    wLines++;

  int wBaseLine=ui->tableTBw->rowCount();

  ui->tableTBw->setRowCount(ui->tableTBw->rowCount()+wLines);

  utf8String  wLineHexa,wLineAscii,wlineOffset;

  /* cut by chunk of pWidth bytes */
  long  wRemain=pData.Size;
  unsigned char* wPtr = pData.Data;
  unsigned char* wPtrEnd = pData.Data+pData.Size;

  int wTick = 0;
  long wProgress=0;
  int wCurLine=wBaseLine;
  while ((wPtr < wPtrEnd)&&(wRemain >= pWidth))
  {
//    wlineOffset.sprintf("%6X - %6ld",FileOffset,FileOffset);
    wlineOffset.sprintf("%6X",FileOffset);
    zmemDumpHexa(wPtr,pWidth,wLineHexa,wLineAscii);
    ui->tableTBw->setVerticalHeaderItem (wCurLine,new QTableWidgetItem(wlineOffset.toCChar()));
    ui->tableTBw->setItem(wCurLine,0,new QTableWidgetItem(wLineHexa.toCChar()));
    ui->tableTBw->setItem(wCurLine,1,new QTableWidgetItem(wLineAscii.toCChar()));



    wPtr+= pWidth;
    FileOffset += pWidth;
    wRemain -= pWidth;
    wProgress += pWidth;
    wCurLine++;
    if (wTick++ > 10)
      {
      wTick=0;
      ui->ProgressPGb->setValue(wProgress);
      }
  }
  if (wRemain > 0)
    {
    zmemDumpHexa(wPtr,wRemain,wLineHexa,wLineAscii);
//    wlineOffset.sprintf("%6X - %6ld",FileOffset,FileOffset);
    wlineOffset.sprintf("%6X",FileOffset);
    ui->tableTBw->setVerticalHeaderItem(wCurLine,new QTableWidgetItem(wlineOffset.toCChar()));
    ui->tableTBw->setItem(wCurLine,0,new QTableWidgetItem(wLineHexa.toCChar()));
    ui->tableTBw->setItem(wCurLine,1,new QTableWidgetItem(wLineAscii.toCChar()));
    }

  ui->tableTBw->resizeRowsToContents();
  ui->tableTBw->resizeColumnsToContents();
  if (!displayWidgetBlockOnce)
    {
    ui->tableTBw->resizeRowsToContents();
    ui->tableTBw->resizeColumnsToContents();
    displayWidgetBlockOnce=true;
    }
  ui->ProgressPGb->setValue(pData.Size);
return ZS_SUCCESS;
}//displayBlock

void
ZContentVisuMain::displayBlockData()
{
  utf8String wStr;
  wStr.sprintf("%6ld",RecordNumber+1L) ;
  ui->RecordNumberLBl->setText(wStr.toCChar());
  wStr.sprintf("0x%08X",Address) ;
  ui->AddressLBl->setText(wStr.toCChar());

  wStr.sprintf("%8ld",Block.BlockSize) ;
  ui->BlockSizeLBl->setText(wStr.toCChar());

  wStr.sprintf("%8ld",Block.DataSize()) ;
  ui->UserSizeLBl->setText(wStr.toCChar());

  ui->StateLBl->setText(decode_ZBS(Block.State));

  wStr.sprintf("%02X %s",Block.Lock,decode_ZLockMask(Block.Lock).toChar());
  ui->LockMaskLBl->setText(wStr.toCChar());

}

ZStatus
ZContentVisuMain::displayListZRFNextRecord(size_t pWidth)
{
  ZStatus wSt;
  if (RecordNumber==0)
    ui->BackwardBTn->setVisible(false);
  else
    ui->BackwardBTn->setVisible(true);

  // get first block of the file
  wSt=RandomFile->_getNext(Block,RecordNumber,Address);     // next logical block
  if (wSt==ZS_EOF)
  {
    ui->ForwardBTn->setVisible(false);
    return ZS_EOF;
  }
  ui->ForwardBTn->setVisible(true);
  if (wSt!=ZS_SUCCESS)
  {
    ZExceptionDLg::displayLast(false);
//    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }


  displayBlockData();

  RawData=Block.Content;

  removeAllRows();

  displayWidgetBlock(RawData,Width);

  ui->ForwardBTn->setVisible(true);
  return ZS_SUCCESS;
}//displayListZRFNextRecord

ZStatus
ZContentVisuMain::displayListZRFPreviousRecord(size_t pWidth)
{
  ZStatus wSt;

  RecordNumber--;
  if (RecordNumber==0)
    ui->BackwardBTn->setVisible(false);

  // get first block of the file
  wSt=RandomFile->_getByRank(Block,RecordNumber,Address);     // get first block of the file
  if (wSt==ZS_EOF)
  {
    ui->ForwardBTn->setVisible(false);
    return ZS_EOF;
  }

  if (wSt!=ZS_SUCCESS)
  {
    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }

  displayBlockData();

  if (RandomFile->getFileDescriptor().ZBAT.count() > 1)
    ui->ForwardBTn->setVisible(true);
  RawData=Block.Content;

  displayWidgetBlock(RawData,Width);


  return ZS_SUCCESS;
}//displayListZRFPreviousRecord

ZStatus
ZContentVisuMain::displayListZRFFirstRecord(size_t pWidth)
{
  ZStatus wSt;

  ui->BackwardBTn->setVisible(false);

  FileOffset=0;
  RecordNumber = 0L;
  wSt=RandomFile->_getByRank(Block,RecordNumber,Address);     // get first block of the file
  if (wSt==ZS_EOF)
  {
    ui->ForwardBTn->setVisible(false);
    return ZS_EOF;
  }

  if (wSt!=ZS_SUCCESS)
  {
    QMessageBox::critical(this,tr("Random File error"),ZException.formatFullUserMessage().toCChar());
    return wSt;
  }

  displayBlockData();

  RawData=Block.Content;

  displayWidgetBlock(RawData,Width);

  ui->ForwardBTn->setVisible(true);
  return ZS_SUCCESS;
}//displayListZRFFirstRecord

void
ZContentVisuMain::removeAllRows()
{
/*  if (TBlItemModel)
    if (TBlItemModel->rowCount()>0)
      TBlItemModel->removeRows(0,TBlItemModel->rowCount());

  if (ui->ListOffsetWDg->count() > 0)
    ui->ListOffsetWDg->clear();
  if (ui->ListHexaWDg->count() > 0)
    ui->ListHexaWDg->clear();
  if (ui->ListAsciiWDg->count() > 0)
    ui->ListAsciiWDg->clear();
*/
  if (ui->tableTBw->rowCount() > 0)
  {
    ui->tableTBw->clearContents();
    ui->tableTBw->setRowCount(0);
  }
}

void
ZContentVisuMain::backward()
{
  ui->ForwardBTn->setEnabled(true);
  ui->ForwardBTn->setVisible(true);

  if (RandomFile)
  {
    displayListZRFPreviousRecord(Width);
    return;
  }

  FileOffset=lseek(Fd,(__off_t)-BlockSize,SEEK_CUR);
  if (FileOffset <= 0)
    {
    ui->BackwardBTn->setVisible(false);
    }
  displayFdNextRawBlock(BlockSize,Width);
}
void
ZContentVisuMain::forward()
{
  if (RandomFile)
  {
    displayListZRFNextRecord(Width);
    return;
  }

  ui->AddressLBl->setText("0");
  ui->RecordNumberLBl->setText("0");

//  ui->BackwardBTn->setVisible(true);
  ZStatus wSt=displayFdNextRawBlock(BlockSize,Width);
  if (wSt==ZS_EOF)
    ui->ForwardBTn->setVisible(false);
}

void
ZContentVisuMain::loadAll()
{
  ui->BackwardBTn->setVisible(false);
  ui->ForwardBTn->setVisible(false);
  ui->LoadAllBTn->setVisible(false);

  ui->SequentialFRm->setVisible(true);

  FileOffset=0;
  URICurrent.loadContent(RawData);
  removeAllRows();
  ZStatus wSt=displayWidgetBlock(RawData,Width);

  ui->ReadSizeLBl->setText((formatSize(RawData.Size)).toCChar());

}

void
ZContentVisuMain::surfaceScanZRF()
{
  FILE* wScan=fopen ("surfacescanzrf.txt","w");
  ZStatus wSt=RandomFile->zsurfaceScan(URICurrent,wScan);
  fflush(wScan);
  fclose(wScan);
  if (wSt!=ZS_SUCCESS)
  {
    QMessageBox::critical(this,tr("Random file error"),ZException.formatFullUserMessage().toCChar());
    return;
  }


  textEditMWn* wTextWin=new textEditMWn;

  wTextWin->setTextFromFile("surfacescanzrf.txt");
  wTextWin->show();

  return;
}

void
ZContentVisuMain::surfaceScanRaw()
{

  ZScan* wScan= new ZScan(this);
  wScan->setFile(URICurrent);
  wScan->show();
  wScan->scanMain();
  return;
}
