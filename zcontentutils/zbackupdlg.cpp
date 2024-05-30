#include "zbackupdlg.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>

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

ZBackupDLg::ZBackupDLg(QWidget* pParent) : QDialog(pParent)
{
  initLayout();
}

ZBackupDLg::~ZBackupDLg()
{
  if (ComLog != nullptr)
    ComLog->deleteLater();
  if (MasterFile.isOpen())
    MasterFile.zclose();
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
ZBackupDLg::initLayout() {

  /* backup dialog setup */

  setWindowTitle("Master file backup");
  resize(750,250);

  QVBoxLayout* QVL=new QVBoxLayout;
  setLayout(QVL);

  QHBoxLayout* QHL=new QHBoxLayout;
  QVL->insertLayout(0,QHL);

  URIMasterFileLBl=new QLabel();
  QHL->addWidget(URIMasterFileLBl);

  SearchZMFBTn = new QPushButton("Search file");
  SearchZMFBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );
  QHL->addWidget(SearchZMFBTn);
  QObject::connect(SearchZMFBTn, &QPushButton::clicked, this, &ZBackupDLg::searchFileClicked);

  QHBoxLayout* QHLDir=new QHBoxLayout;
  QVL->insertLayout(-1,QHLDir);

  QLabel* wLbDirLBl = new QLabel("Backup root directory");
  QHLDir->addWidget(wLbDirLBl);

  BckRootDirectoryLBl=new QLabel;
  QHLDir->addWidget(BckRootDirectoryLBl);

  BckDirBTn=new QPushButton("Search");
  BckDirBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );
  QHLDir->addWidget(BckDirBTn);
  QObject::connect(BckDirBTn, &QPushButton::clicked, this, &ZBackupDLg::searchDirClicked);

  QHBoxLayout* QHLBckExt=new QHBoxLayout;
  QVL->insertLayout(-1,QHLBckExt);

  QLabel* wLbBckExtLBl = new QLabel("Backup set name");
  QHLBckExt->addWidget(wLbBckExtLBl);
  BckNameLEd = new QLineEdit("bck");
  QHLBckExt->addWidget(BckNameLEd);

  QObject::connect(BckNameLEd,SIGNAL(textChanged),this,SLOT(bckextChanged));

  /* Index path */

  QHBoxLayout* QHLIndex=new QHBoxLayout;
  QVL->insertLayout(-1,QHLIndex);

  QLabel* wLbIndexPathLBl = new QLabel("Index files path");
  QHLIndex->addWidget(wLbIndexPathLBl);
  IndexPathLBl = new QLabel;
  QHLIndex->addWidget(IndexPathLBl);

  /* table view */

  QHBoxLayout* QHLTBv=new QHBoxLayout;
  QVL->insertLayout(-1,QHLTBv);

  BckTBv = new ZQTableView(this);
  QHLTBv->addWidget(BckTBv);


  QHBoxLayout* QHLPBr=new QHBoxLayout;
  QVL->insertLayout(-1,QHLPBr);

  ProgressPBr = new QProgressBar(this);
  QHLPBr->addWidget(ProgressPBr);
  ProgressPBr->setVisible(false);

  /* buttons */

  QHBoxLayout* wButtonBoxQHb=new QHBoxLayout;
  wButtonBoxQHb->setAlignment(Qt::AlignRight);
  QVL->insertLayout(-1,wButtonBoxQHb);

  CancelBTn = new QPushButton("Quit");
  wButtonBoxQHb->addWidget(CancelBTn);
  OkBTn = new QPushButton("Do backup");
  wButtonBoxQHb->addWidget(OkBTn);

  QObject::connect(OkBTn, &QPushButton::clicked, this, &ZBackupDLg::doBackup);
  QObject::connect(CancelBTn, &QPushButton::clicked, this, &QDialog::reject);

  /* table view setup */

  BckTBv->newModel(4);

  BckTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("File"));
  BckTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem("Created"));
  BckTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem("Last modified"));
  BckTBv->ItemModel->setHorizontalHeaderItem(3,new QStandardItem("Status"));
}

ZStatus
ZBackupDLg::dataSetup(uriString& pURIMaster, const uriString &pTargetDir) {
  ZStatus wSt = MasterFile.zopen(pURIMaster,ZRF_All);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  URIMasterFile = pURIMaster;
  URIMasterFileLBl->setText(URIMasterFile.toCChar());

  utf8VaryingString wMasterRootName = URIMasterFile.getRootname();

  if (MasterFile.IndexFilePath.isEmpty())
    IndexPathLBl->setText("<same as master file>");
  else
    IndexPathLBl->setText(MasterFile.IndexFilePath.toCChar());

  BckTBv->ItemModel->removeRows(0,BckTBv->ItemModel->rowCount());

  MasterBaseName = pURIMaster.getBasename();
  SourceDirPath = pURIMaster.getDirectoryPath();
  utf8VaryingString wStr;


  /* source :
   * for master file content and header file : only base name
   * for dictionary and index files : full path name is mentionned
   * destination :
   * only computed base names are mentionned
   */

  QList<QStandardItem*> wRow;

  BckElement wElt = getElt(pURIMaster);

  wRow << new QStandardItem(wElt.Source.getBasename().toCChar());
  wRow << new QStandardItem(wElt.Created.toLocale().toCChar());
  wRow << new QStandardItem(wElt.LastModified.toLocale().toCChar());
  wRow << new QStandardItem("");

  BckTBv->ItemModel->appendRow(wRow);


  wElt = getElt(MasterFile.getURIHeader());
  wRow.clear();
  wRow << new QStandardItem(wElt.Source.getBasename().toCChar());
  wRow << new QStandardItem(wElt.Created.toLocale().toCChar());
  wRow << new QStandardItem(wElt.LastModified.toLocale().toCChar());
  wRow << new QStandardItem("");
  BckTBv->ItemModel->appendRow(wRow);

  if (MasterFile.Dictionary!=nullptr) {
    wElt = getElt(MasterFile.Dictionary->URIDictionary);

    wRow.clear();
    wRow << new QStandardItem(wElt.Source.getBasename().toCChar());
    wRow << new QStandardItem(wElt.Created.toLocale().toCChar());
    wRow << new QStandardItem(wElt.LastModified.toLocale().toCChar());
    wRow << new QStandardItem("");
    BckTBv->ItemModel->appendRow(wRow);
  }


  for (long wi=0; wi < MasterFile.IndexTable.count();wi++) {
    wElt = getElt(MasterFile.IndexTable[wi]->getURIContent());

    wRow.clear();
    wRow << new QStandardItem(wElt.Source.getBasename().toCChar());
    wRow << new QStandardItem(wElt.Created.toLocale().toCChar());
    wRow << new QStandardItem(wElt.LastModified.toLocale().toCChar());
    wRow << new QStandardItem("");
    BckTBv->ItemModel->appendRow(wRow);

    wRow.clear();
    wElt = getElt(MasterFile.IndexTable[wi]->getURIHeader());
    wRow << new QStandardItem(wElt.Source.getBasename().toCChar());
    wRow << new QStandardItem(wElt.Created.toLocale().toCChar());
    wRow << new QStandardItem(wElt.LastModified.toLocale().toCChar());
    wRow << new QStandardItem("");
    BckTBv->ItemModel->appendRow(wRow);

  } //for (long wi=0; wi < MasterFile.IndexTable.count();wi++)


  BckTBv->resizeColumnsToContents();
  BckTBv->resizeRowsToContents();

  return ZS_SUCCESS;

}

BckElement ZBackupDLg::getElt(const uriString& pSource,const uriString& pTargetDir) {
  BckElement wElt;
  uriStat wStat;
  wElt.Source = pSource ;
  wElt.Size = wElt.Source.getFileSize();
  ZStatus wSt=wElt.Source.getStatR(wStat);
  if (wSt!=ZS_SUCCESS) {
    wElt.Status = wSt;
  }
  wElt.Created = wStat.Created;
  wElt.LastModified = wStat.LastModified;

  if (pTargetDir.isEmpty())
    return wElt;

  wElt.Target = pTargetDir;
  wElt.Target += wElt.Source.getBasename();
  return wElt;
}

bool
ZBackupDLg::searchFileClicked() {
  while (true) {
    QString wFileName = QFileDialog::getOpenFileName(this, "Select content master file",
                                                     GeneralParameters.getWorkDirectory().toCChar(),
                                                     "ZContent files (*.zmf);;All (*.*)");
    if (wFileName.isEmpty()) {
      return false;
    }
    URIMasterFile = wFileName.toUtf8().data();
    URIMasterFileLBl->setText(wFileName);

    ZStatus wSt=dataSetup(URIMasterFile);
    if (wSt!=ZS_SUCCESS) {
      int wRet = ZExceptionDLg::adhocMessage2B("Search master file",Severity_Error,
          "Quit","Try another",
          "File <%s> appears to be an invalid master file.\n"
          "See exception for more information.\n"
          "Try another file or Quit the dialog.",
          URIMasterFile.toCChar());
      if (wRet==QDialog::Rejected)
        reject();
      continue;
    }
    break;
  }// while true;
  return true;
}

void
ZBackupDLg::searchDirClicked() {
  ZStatus wSt;
  QString wDirName = QFileDialog::getExistingDirectory(this, "Select target directory",
                                                       GeneralParameters.getWorkDirectory().toCChar());
  if (wDirName.isEmpty()) {
//    ZExceptionDLg::adhocMessage("Select directory",Severity_Error,"Please select a valid directory");
    return;
  }
  BckRootDirectory = wDirName.toUtf8().data();
  BckRootDirectoryLBl->setText(wDirName);
}

void
ZBackupDLg::bckextChanged() {
  BckName = BckNameLEd->text().toUtf8().data();
}

size_t
ZBackupDLg::computeAllSizes(){
  size_t wSize=0;
  for (long wi=0; wi < Files.count(); wi++) {
    wSize += Files[wi].Size;
  }
  return wSize;
}


void
ZBackupDLg::doBackup() {
  if (ComLog==nullptr) {
    ComLog = new textEditMWn(this,TEOP_NoFileLab | TEOP_CloseBtnHide,&ComLog);
    ComLog->setWindowTitle("Backup log");
  }
  ComLog->show();

  utf8VaryingString wErrMsg;
  uriStat wStat;

  if (BckName.isEmpty()){
    wErrMsg = "Backup extension must not be empty.";
  }
  if (BckRootDirectory.isEmpty()){
    if (!wErrMsg.isEmpty())
      wErrMsg+= "\n";
    wErrMsg = "Backup root directory is empty.";
  }
  if (!wErrMsg.isEmpty()) {
    ZExceptionDLg::adhocMessage("Do backup",Severity_Error,wErrMsg.toCChar());
    return;
  }
  utf8VaryingString wBackupSetName = BckName;
  wBackupSetName += ZDateFull::currentDateTime().toUTCGMT() ;

  uriString wTargetDir = BckRootDirectory;
  wTargetDir.addConditionalDirectoryDelimiter();
  wTargetDir += wBackupSetName ;


  ZStatus wSt = uriString::createDirectory(wTargetDir);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Create backup directory",Severity_Error,"Cannot create directory <%s>",wTargetDir.toString());
    ComLog->appendText("Cannot create directory %s\n"
                        "Exception follows:\n%s",
                        wTargetDir.toString(),
                        ZException.last().formatFullUserMessage().toString());
    return;
  }
  ComLog->appendText("Created directory %s",wTargetDir.toString());

  wTargetDir.addDirectoryDelimiter();

  ComLog->appendText("Preparing files to backup");

  BckElement wElt;
  /* prepare files to copy and compute all file sizes */
  size_t wSize=0;
  /* master content file */
  wElt.Source = MasterFile.getURIContent();

  wStat=wElt.Source.getStatR();
  wElt.Created = wStat.Created;
  wElt.LastModified = wStat.LastModified;

  wSize += wElt.Size = wStat.Size ;

  wElt.Target = MasterFile.getURIContent().getBasename();
  Files.push(wElt) ;

  /* master header file */
  wElt.Source = MasterFile.getURIHeader();

  wStat=wElt.Source.getStatR();
  wElt.Created = wStat.Created;
  wElt.LastModified = wStat.LastModified;

  wSize += wElt.Size = wStat.Size ;

  wElt.Target = MasterFile.getURIHeader().getBasename();

//  wElt.Target = wTargetDir;
//  wElt.Target += MasterFile.getURIHeader().getBasename();
  Files.push(wElt) ;

  /* dictionary file if any */
  if (MasterFile.Dictionary!=nullptr) {
    wElt.Source = MasterFile.Dictionary->URIDictionary ;

    wStat=wElt.Source.getStatR();
    wElt.Created = wStat.Created;
    wElt.LastModified = wStat.LastModified;

    wSize += wElt.Size = wStat.Size ;

    wElt.Target = MasterFile.Dictionary->URIDictionary.getBasename();
    Files.push(wElt);
  }

  for (long wi=0; wi < MasterFile.IndexTable.count(); wi++) {

    wElt.Source = MasterFile.IndexTable[wi]->getURIContent() ;
    wElt.Size = wElt.Source.getFileSize();

    wStat=wElt.Source.getStatR();
    wElt.Created = wStat.Created;
    wElt.LastModified = wStat.LastModified;

    wSize += wElt.Size = wStat.Size ;

    wElt.Target = MasterFile.IndexTable[wi]->getURIContent().getBasename();
    Files.push(wElt);

    wElt.Source = MasterFile.IndexTable[wi]->getURIHeader() ;
    wElt.Size = wElt.Source.getFileSize();

    wStat=wElt.Source.getStatR();

    wElt.Created = wStat.Created;
    wElt.LastModified = wStat.LastModified;
    wSize += wElt.Size = wStat.Size ;

    wElt.Target = MasterFile.IndexTable[wi]->getURIHeader().getBasename();
    Files.push(wElt);
  } // for

  ProgressPBr->setMinimum(0);
  ProgressPBr->setMaximum(int(wSize));
  ProgressPBr->setValue(0);
  ProgressPBr->setVisible(true);

  ComLog->appendText("Starting backup of %ld files",Files.count());

  uriString wTargetFile ;

  int wSizeCur=0;
  for (long wi=0; wi < Files.count(); wi++) {

    wTargetFile = wTargetDir;
    wTargetFile += Files[wi].Target;

    Files[wi].Status=uriString::copyFile(wTargetFile,Files[wi].Source);

    BckTBv->ItemModel->item(wi,3)->setText(decode_ZStatus(Files[wi].Status));

    if (Files[wi].Status==ZS_SUCCESS)
      ComLog->appendText("Backing up file <%s> to <%s>",Files[wi].Source.toString(),wTargetFile.toString());
    else {
      ZException.setMessage("ZMFBckDLg::doBackup",Files[wi].Status,Severity_Error,
          "Error while copying file <%s> to backup file <%s>",
          Files[wi].Source.toString(),Files[wi].Target.toString());
      ComLog->appendText("Problem while backing up file <%s> to <%s>\n"
                          "Exception follows\n%s",
                          Files[wi].Source.toString(),Files[wi].Target.toString(),
                          ZException.last().formatFullUserMessage().toString());
      QPalette p = ProgressPBr->palette();
      p.setColor(QPalette::Highlight, Qt::red);
      ProgressPBr->setPalette(p);
    }
    wSizeCur += int(Files[wi].Size);
    ProgressPBr->setValue(wSizeCur);
  }// for

  ComLog->appendText("Writing backup set xml description file");

  uriString wURIBckset = utf8VaryingString(wTargetDir) + wBackupSetName + ".bckset";

  wSt = XmlSaveBackupset(wURIBckset,true);
  if (wSt!=ZS_SUCCESS) {

    ComLog->appendText("Problem while writing backup set xml description file <%s> \n"
                       "Exception follows\n%s",
                        wURIBckset.toString(),
                        ZException.last().formatFullUserMessage().toString());
  }
  else
    ComLog->appendText("Backup set xml description file successfully written");
  return ;
}// doBackup




ZStatus
ZBackupDLg::XmlSaveBackupset(uriString& pXmlFile, bool pComment) {
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
ZBackupDLg::XmlLoadBackupset(const uriString& pXmlFile,ZDateFull& pBackupDate)
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

  Files.clear();  /* clear files definitions */
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





