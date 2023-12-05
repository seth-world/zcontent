#include "zcopydlg.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>

#include <QComboBox>
#include <QCheckBox>

#include <QStandardItemModel>
#include <QStandardItem>



#include <QFileDialog>

#include <ztoolset/uristring.h>


#include <zcontent/zcontentutils/zexceptiondlg.h>

#include <zqt/zqtwidget/zqtableview.h>

#include "texteditmwn.h"

/* for xml exchanges */
#include <zxml/zxmlprimitives.h>
#include <zcontentcommon/zgeneralparameters.h>

#include <ztoolset/zdatabuffer.h>
#include <zio/zioutils.h>

#include <ztoolset/zaierrors.h>

ZCopyDLg::ZCopyDLg(ZaiErrors* pErrorLog,QWidget* pParent) : QDialog(pParent) , ZProgressBase(this,ZPTP_General|ZPTP_HasCloseBtn)
{
 /*   if (pComLog==nullptr)
        pComLog = new textEditMWn(pParent,TEOP_NoFileLab | TEOP_CloseBtnHide);
    ComLog = pComLog;
*/
    ErrorLog = pErrorLog;
  initLayout();
}

ZCopyDLg::~ZCopyDLg()
{
 /* if (ComLog != nullptr)
    ComLog->deleteLater();
*/
}
/*
      <master file full path>    <Search file btn>

  Backup root directory  <backup root directory> <Search btn>
  Backup set name <bck set name>

  Index files path  <Index path>

      ---table view---
      file                          status
  <master content file base name>   <Icon>
  <master header file base name>    <Icon>

  <dictionary full path name>       <Icon>

  <index 0 base name>               <Icon>
      ...
  <index n base name>               <Icon>

  [,,,,,,,progress bar.....]

                      <Give up btn> <Do backup btn>

 */
void
ZCopyDLg::initLayout() {

  /* backup dialog setup */

  setWindowTitle("Copy file(s)");
  resize(750,250);

  QVBoxLayout* QVL=new QVBoxLayout;
  setLayout(QVL);

  QHBoxLayout* QHL=new QHBoxLayout;
  QVL->insertLayout(0,QHL);

  QLabel* wSrcLBl = new QLabel("Source");
  QHL->addWidget(wSrcLBl);
  URISourceLBl=new QLabel("no source file");
  QHL->addWidget(URISourceLBl);

  SearchZMFBTn = new QPushButton("Search Source");
  SearchZMFBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );
  QHL->addWidget(SearchZMFBTn);
  QObject::connect(SearchZMFBTn, &QPushButton::clicked, this, &ZCopyDLg::searchFileClicked);

  QHBoxLayout* QHLDir=new QHBoxLayout;
  QVL->insertLayout(-1,QHLDir);

  QLabel* wLbDirLBl = new QLabel("Target directory");
  QHLDir->addWidget(wLbDirLBl);

  URITargetLBl=new QLabel("No target defined");
  QHLDir->addWidget(URITargetLBl);

  TargetBTn=new QPushButton("Search Target");
  TargetBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );
  QHLDir->addWidget(TargetBTn);
  QObject::connect(TargetBTn, &QPushButton::clicked, this, &ZCopyDLg::targetClicked);
/*
  QHBoxLayout* QHLBckExt=new QHBoxLayout;
  QVL->insertLayout(-1,QHLBckExt);

  QLabel* wLbBckExtLBl = new QLabel("Backup set name");
  QHLBckExt->addWidget(wLbBckExtLBl);
  BckNameLEd = new QLineEdit("bck");
  QHLBckExt->addWidget(BckNameLEd);
*/
  QHBoxLayout* QHLCBx=new QHBoxLayout;
  QVL->insertLayout(-1,QHLCBx);

  FlagCBx=new QComboBox;

  FlagCBx->addItem("Do not replace");
  FlagCBx->addItem("Replace target with no backup");
  FlagCBx->addItem("Backup target before");

  IncludeOptCBx = new QComboBox;
  IncludeOptCBx->addItem("Single file");
  IncludeOptCBx->addItem("Include all dependent files");

  QHLCBx->addWidget(FlagCBx);
  QHLCBx->addWidget(IncludeOptCBx);


  QHBoxLayout* QHLCHk=new QHBoxLayout;
  QVL->insertLayout(-1,QHLCHk);

  FailCHk = new QCheckBox("Stop on fail");
  PrivCHk = new QCheckBox("Keep owner and privileges");

  QHLCHk->addWidget(PrivCHk);
  QHLCHk->addWidget(FailCHk);


//  QObject::connect(BckNameLEd,SIGNAL(textChanged),this,SLOT(bckextChanged));

  /* Index path */

  QHBoxLayout* QHLIndex=new QHBoxLayout;
  QVL->insertLayout(-1,QHLIndex);

  QLabel* wLbIndexPathLBl = new QLabel("Index files path");
  QHLIndex->addWidget(wLbIndexPathLBl);
  IndexPathLBl = new QLabel;
  QHLIndex->addWidget(IndexPathLBl);

  /* table view */
/*
  QHBoxLayout* QHLTBv=new QHBoxLayout;
  QVL->insertLayout(-1,QHLTBv);

  BckTBv = new ZQTableView(this);
  QHLTBv->addWidget(BckTBv);
*/

  QHBoxLayout* QHLPBr=new QHBoxLayout;
  QVL->insertLayout(-1,QHLPBr);

  initProgressLayout(QVL);

  /* buttons */

  QHBoxLayout* wButtonBoxQHb=new QHBoxLayout;
  wButtonBoxQHb->setAlignment(Qt::AlignRight);
  QVL->insertLayout(-1,wButtonBoxQHb);

  CancelBTn = new QPushButton("Quit");
  wButtonBoxQHb->addWidget(CancelBTn);
  OkBTn = new QPushButton("Do copy");
  wButtonBoxQHb->addWidget(OkBTn);

  QObject::connect(OkBTn, &QPushButton::clicked, this, &ZCopyDLg::doCopy);
  QObject::connect(CancelBTn, &QPushButton::clicked, this, &QDialog::reject);

  /* table view setup */
/*
  BckTBv->newModel(4);

  BckTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("File"));
  BckTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem("Created"));
  BckTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem("Last modified"));
  BckTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem("Status"));
*/
}

bool
ZCopyDLg::searchFileClicked() {

        QList<QString> wFileList = QFileDialog::getOpenFileNames(this, "Select file(s)",
                                                     GeneralParameters.getWorkDirectory().toCChar(),
                                                     "ZContent files (*.zmf *.zrf *.zix );;All (*.*)");
    if (wFileList.isEmpty()) {
      return false;
    }
    for (int wi=0;wi < wFileList.count();wi++)
        SourceList.push(uriString(wFileList[wi].toUtf8().data()));
 //   URIMasterFile = wFileName.toUtf8().data();
    if (SourceList.count() > 1)
        URISourceLBl->setText("<list of files>");
    else
        URISourceLBl->setText(wFileList[0]);

  return true;
}

void
ZCopyDLg::targetClicked() {
  ZStatus wSt;
  QString wDirName = QFileDialog::getExistingDirectory(this, "Select target directory",
                                                       GeneralParameters.getWorkDirectory().toCChar());
  if (wDirName.isEmpty()) {
//    ZExceptionDLg::adhocMessage("Select directory",Severity_Error,"Please select a valid directory");
    return;
  }
  TargetPath = wDirName.toUtf8().data();
  URITargetLBl->setText(wDirName);
}

/*
void
ZCopyDLg::bckextChanged() {
  BckName = BckNameLEd->text().toUtf8().data();
}
*/


/* source :
     *  - unique file (count == 1) :
     *    Then target has to be either
     *    . a directory only - it keeps base file name(s) to copy into directory. NB: Directory must not be the source directory
     *    . a full file spec change base file name with new file name whatever target directory is. NB: Cannot be the same directory and same file name.
     *  - multiple files (count > 1):
     *    . target MUST BE a directory only
     */
void
ZCopyDLg::doCopy() {

  utf8VaryingString wErrMsg;
  uriStat wStat;

  if (SourceList.isEmpty()){
      if (!wErrMsg.isEmpty())
          wErrMsg+= "\n";
      wErrMsg = "Source file list is empty.";
  }
  if (TargetPath.isEmpty()){
    if (!wErrMsg.isEmpty())
      wErrMsg+= "\n";
    wErrMsg = "Target is empty.";
  }
  if (!wErrMsg.isEmpty()) {
    ZExceptionDLg::adhocMessage("Do copy",Severity_Error,wErrMsg.toCChar());
    return;
  }
  if ((SourceList.count()>1)&& (!TargetPath.isDirectory())) {
      ZException.setMessage("ZCopyDLg::doCopy",ZS_INVPARAMS,Severity_Error,
                            "Copying multiples files (%d files) implies that target is a (valid) directory.\n"
                            "invalid target is <%s>",
                            SourceList.count(),TargetPath.toString());
      ErrorLog->logZExceptionLast("ZCopyDLg::doCopy");
      return;
  }

  const char* wFileSel = "Single file copy";
  if (IncludeOptCBx->currentIndex()==1)
      wFileSel="Include all dependent files (header,indexes,dictionary)";

  ErrorLog->textLog("Starting copy request for %ld main source file(s)",SourceList.count());
  ErrorLog->textLog("Options are <%s>\n"
                    "            <%s>\n"
                    "            <%s>",
                    wFileSel,
                    PrivCHk->isChecked()?"Keep owner and privileges":"Set current owner and default privileges",
                    FailCHk->isChecked()?"Stop on fail":"Continue despite fail");


  ZMNP_type wFlags = getFlags();

  ZArray<ZCopyElement> wEffectiveSourceList;
//  ZRandomFile wZRF;
  __FILEHANDLE__ wFd = __FILEHANDLEINVALID__;
  __FILEHANDLE__ wFdHead = __FILEHANDLEINVALID__;
  ZStatus wSt;
  ZDataBuffer wReadRecord;
  ZDataBuffer wHeaderContent;
  ZHeaderControlBlock_Export  wHeader_Exp ;

  const unsigned char* wPtr=nullptr;
  uint32_t wBlockMarker=0;
  size_t wCopySize=0;

  for (int wi=0; wi < SourceList.count(); wi++) {

      wCopySize += SourceList[wi].getFileSize();  // add file size in any cases
      wEffectiveSourceList.push(ZCopyElement(SourceList[wi]));
      ErrorLog->textLog("Processing file %s size %ld",SourceList[wi].toString(),SourceList[wi].getFileSize());

      if ( wFlags & ZMNP_IncludeAll) {
          if (wEffectiveSourceList.last().Size==0) {
              ErrorLog->warningLog("     File is empty : cannot detect file type from content.\n"
                                   "     file is to be considered as common sequential file : option ZMNP_IncludeAll ignored.");
              continue;
          }
          wSt=rawOpenRead(wFd,SourceList[wi]);
          wSt=rawRead(wFd,wReadRecord,sizeof(uint32_t));
          wBlockMarker = wReadRecord.moveTo<uint32_t>();
          rawClose(wFd);
          if ((wBlockMarker!=cst_ZFILEBLOCKSTART)&&(wBlockMarker!=cst_ZBLOCKSTART)) {
              /* this is neither a header file nor a content file : process as common file */
              continue;
          }

          if (wBlockMarker==cst_ZFILEBLOCKSTART) { /* is a content file then get its header file */

              ErrorLog->textLog("   File is detected to belong to ZRandomFile family.");

              uriString wHeaderPath ;
              uriString wDicPath;

              wSt=generateURIHeader(SourceList[wi],wHeaderPath);

              wCopySize += wHeaderPath.getFileSize();
              ErrorLog->textLog("   Processing header %s size %ld",wHeaderPath.toString(),wHeaderPath.getFileSize());

              wEffectiveSourceList.push(wHeaderPath);

              wSt=wHeaderPath.loadContent(wHeaderContent);
              if (wSt!=ZS_SUCCESS) {
                  ZException.setMessage("ZCopyDLg::doCopy",ZS_CORRUPTED,Severity_Error,
                                        "Cannot load header file content file <%s>. Aborting copy.",
                                        wHeaderPath.toString());
                  ErrorLog->logZExceptionLast("ZCopyDLg::doCopy");
                  return ; /* then stop */
              }

              if (wHeaderContent.Size < sizeof(ZHeaderControlBlock_Export)) {
                  ZException.setMessage("ZCopyDLg::doCopy",ZS_CORRUPTED,Severity_Error,
                                        "Header file <%s> appears to be corrupted. Cannot load ZHeaderControlBlock.\n"
                                        "Continuing copy."
                                        "But all linked files (indexes and dictionary) will not be copied.",
                                        wHeaderPath.toString());
                  ErrorLog->logZExceptionLast("ZCopyDLg::doCopy");
                  continue ; /* continue anyway : may be want to copy a corrupted file among the list */
              }

              wHeader_Exp._copyFrom( wHeaderContent.moveTo<ZHeaderControlBlock_Export>());
              wHeader_Exp.deserialize();  // this is not necessary for uint8_t
              switch (wHeader_Exp.FileType)
              {
              case ZFT_ZRawMasterFile:
              case ZFT_ZMasterFile: {
                  ErrorLog->textLog("   File is detected to be a ZRawMasterFile or ZMasterFile.");
                  ZRawMasterFile wZMF;
                  wSt=wZMF.zopen(SourceList[wi]);
                  /* if master file : capture all files : zindex contents and headers, dictionary file if any */
/*                  wPtr = wHeaderContent.Data + wHeader_Exp.OffsetReserved;
                  wSt=wZMF.ZMasterControlBlock::_import(wPtr);
*/
                  if (wZMF.hasDictionary()) {
                      wCopySize += wZMF.Dictionary->URIDictionary.getFileSize();
                      wEffectiveSourceList.push(wZMF.Dictionary->URIDictionary);
                      ErrorLog->textLog("Found dictionary %s size %ld",wZMF.Dictionary->URIDictionary.toString(),wZMF.Dictionary->URIDictionary.getFileSize());
                  }
                  else
                      ErrorLog->textLog("   File has no embedded dictionary.");
                  for (int wj=0; wj < wZMF.IndexTable.count();wj++) {

                      wCopySize += wZMF.IndexTable[wj]->getURIContent().getFileSize();
                      wCopySize += wZMF.IndexTable[wj]->getURIHeader().getFileSize();
                      wEffectiveSourceList.push(wZMF.IndexTable[wj]->getURIContent());
                      wEffectiveSourceList.push(wZMF.IndexTable[wj]->getURIHeader());
                      ErrorLog->textLog("   Processing index content %s size %ld",wZMF.IndexTable[wj]->getURIContent().toString(),wZMF.IndexTable[wj]->getURIContent().getFileSize());
                      ErrorLog->textLog("   Processing index header %s size %ld",wZMF.IndexTable[wj]->getURIHeader().toString(),wZMF.IndexTable[wj]->getURIHeader().getFileSize());

                  }// for
                 wZMF.zclose();
              } //ZFT_ZMasterFile

              case ZFT_ZRandomFile:
              case ZFT_ZIndexFile:
                {
                  continue; /* copy request has been already stored */
                }

                }// switch
          } // cst_ZFILEBLOCKSTART
      }// include all

  }// for

  ErrorLog->textLog("Effective files list count %ld",wEffectiveSourceList.count());

  for (int wi=0; wi < wEffectiveSourceList.count();wi++) {
      uriString wTarget=TargetPath;
      wTarget.addConditionalDirectoryDelimiter();
      wTarget += wEffectiveSourceList[wi].getBasename();
      ErrorLog->textLog("size %ld file %s to %s",wEffectiveSourceList[wi].Size,wEffectiveSourceList[wi].toString(),wTarget.toString());
  }
  ErrorLog->textLog("____________________________________________________________");

  ErrorLog->textLog("Total size to copy %ld",wCopySize);

  setGeneralAdvanceDescription("Global volume copied");
  setAdvanceDescription("Bytes copied");

  initGeneralCount(wCopySize);
  size_t wCumulSize=0;
   for (long wi=0;wi < wEffectiveSourceList.count();wi++) {
 //     setStep(wEffectiveSourceList[wi]);
 //     initCount(wEffectiveSourceList[wi].Size);
 //     advanceSetupCallBack(0,wEffectiveSourceList[wi].getBasename());

      uriString wTargetURI = TargetPath;
      wTargetURI.addConditionalDirectoryDelimiter();
      wTargetURI += wEffectiveSourceList[wi].getBasename();

      wSt=rawCopy(wEffectiveSourceList[wi],wTargetURI,wFlags, -1,
                    std::bind(&ZProgressBase::advanceCallBack,this,std::placeholders::_1,std::placeholders::_2),
                    std::bind(&ZProgressBase::advanceSetupCallBack,this,std::placeholders::_1,std::placeholders::_2),
                    ErrorLog);
      setDone((wSt!=ZS_EOF)&&(wSt!=ZS_SUCCESS)&&(wSt!=ZS_EMPTY));

      if (FailCHk->isChecked() && (wSt!=ZS_EOF)&&(wSt!=ZS_SUCCESS)&&(wSt!=ZS_EMPTY))
          break;

      wCumulSize += wEffectiveSourceList[wi].Size;
      ErrorLog->textLog("%ld bytes copied - total %ld of %ld", wEffectiveSourceList[wi].Size,wCumulSize,wCopySize);


  }//for

   setGeneralDone((wSt!=ZS_EOF)&&(wSt!=ZS_SUCCESS)&&(wSt!=ZS_EMPTY));

   OkBTn->setEnabled(false);

   SourceList.clear();
   URISourceLBl->setText("<no source>");

  return ;
}// doCopy


/*

ZStatus
ZCopyDLg::XmlSaveBackupset(uriString& pXmlFile, bool pComment) {
  utf8VaryingString wReturn = fmtXMLdeclaration();
  int wLevel=0;
  wReturn += fmtXMLnodeWithAttributes("zbackupset","version",__ZRF_XMLVERSION_CONTROL__,0);

  wReturn+=fmtXMLdatefull("date",ZDateFull::currentDateTime(),wLevel);

  wReturn += fmtXMLnode("filelist",wLevel);
  wLevel++;

  for (long wi=0; wi < Files.count();wi++) {
    wReturn += fmtXMLnode("file",wLevel);
    wLevel++;
    wReturn+=fmtXMLchar("source",Files[wi].Source.toString(),wLevel);
    wReturn+=fmtXMLchar("target",Files[wi].Target.toString(),wLevel);
    wReturn+=fmtXMLulong("size",Files[wi].Size,wLevel);
    wReturn+=fmtXMLZStatus("status",Files[wi].Status,wLevel);
    wReturn+=fmtXMLdatefull("creationdate",Files[wi].Created,wLevel);
    wReturn+=fmtXMLdatefull("modificationdate",Files[wi].LastModified,wLevel);
    wLevel--;
    wReturn += fmtXMLendnode("file",wLevel);
  }

  wLevel--;
  wReturn += fmtXMLendnode("filelist",wLevel);
  wLevel--;

  wReturn += fmtXMLendnode("zbackupset",0);

  return pXmlFile.writeContent(wReturn);
}

ZStatus
ZCopyDLg::XmlLoadBackupset(const uriString& pXmlFile,ZDateFull& pBackupDate)
{
  ZStatus wSt;

  utf8VaryingString wXmlContent;

  zxmlDoc *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;
  zxmlElement *wFileRootNode=nullptr;
  zxmlElement *wSingleFileNode=nullptr;
  zxmlElement *wSwapNode=nullptr;

  wSt = pXmlFile.loadUtf8(wXmlContent);
  if (wSt!=ZS_SUCCESS) {
    ComLog->appendText("Problem while loading backup set xml description file <%s> \n"
                       "Exception follows\n%s",
        pXmlFile.toString(),
        ZException.last().formatFullUserMessage().toString());
    return wSt;
  }

  wDoc = new zxmlDoc;
  wSt = wDoc->ParseXMLDocFromMemory(wXmlContent.toCChar(), wXmlContent.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    ZException.setMessage("ZMFBckDLg::XmlLoadBackupset",wSt,Severity_Error,"Xml parsing error for xml document <%s>",
        wXmlContent.subString(0, 25).toUtf());
    return wSt;
  }
  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    return wSt;
  }
  if (!(wRoot->getName() == "zbackupset")) {
    ZException.setMessage("ZMFBckDLg::XmlLoadBackupset",ZS_XMLINVROOTNAME,Severity_Error,
        "Invalid root node name <%s> expected <zbackupset>",
        wRoot->getName().toCChar());
    return ZS_XMLINVROOTNAME;
  }

  wSt=XMLgetChildZDateFull(wSingleFileNode,"date",pBackupDate);

  wSt=wRoot->getChildByName((zxmlNode*&)wFileRootNode,"filelist");
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("ZMFBckDLg::XmlLoadBackupset",ZS_XMLMISSREQ,Severity_Error,
        "Cannot find node name <filelist>");
    return ZS_XMLMISSREQ;
  }

  wSt=wFileRootNode->getFirstChild((zxmlNode*&)wSingleFileNode);
  if (wSt!=ZS_SUCCESS) {
    ZException.setMessage("ZMFBckDLg::XmlLoadBackupset",ZS_XMLCORRUPTED,Severity_Error,
        "Cannot find child node name under node <filelist>. Xml document is incomplete or corrupted.");
    return ZS_XMLCORRUPTED;
  }
  if (wSingleFileNode->getName()!="file") {
    ZException.setMessage("ZMFBckDLg::XmlLoadBackupset",ZS_XMLINVNODENAME,Severity_Error,
        "Got node name <%s> while expecting node <file>",wSingleFileNode->getName().toString());
    return ZS_XMLINVNODENAME;
  }

  Files.clear();
  while (wSt==ZS_SUCCESS)
  {
    BckElement wElt;
    wSt=XMLgetChildText(wSingleFileNode,"source",wElt.Source);
    wSt=XMLgetChildText(wSingleFileNode,"target",wElt.Target);
    wSt=XMLgetChildULong(wSingleFileNode,"size",wElt.Size);
    wSt=XMLgetChildZStatus(wSingleFileNode,"status",(int64_t&)wElt.Status);
    wSt=XMLgetChildZDateFull(wSingleFileNode,"creationdate",wElt.Created);
    wSt=XMLgetChildZDateFull(wSingleFileNode,"modificationdate",wElt.LastModified);

    wSt=wSingleFileNode->getNextNode((zxmlNode*&)wSwapNode);
    XMLderegister(wSingleFileNode);
    wSingleFileNode=wSwapNode;
  } // while

  XMLderegister(wSingleFileNode);
  XMLderegister(wFileRootNode);
  XMLderegister(wRoot);

  return ZS_SUCCESS;
} // ZMFBckDLg::XmlLoadBackupset

*/

ZMNP_type
ZCopyDLg::getFlags()
{
    ZMNP_type wFlags= ZMNP_type (FlagCBx->currentIndex());
    wFlags |= ZMNP_type(PrivCHk->isChecked() ? 1 : 0 );
    wFlags |= ZMNP_type(IncludeOptCBx->currentIndex() * int(ZMNP_IncludeAll) ) ; /* 0 or ZMNP_IncludeAll */
    return wFlags;
}


