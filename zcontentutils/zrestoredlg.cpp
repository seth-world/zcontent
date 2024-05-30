#include "zrestoredlg.h"


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>

#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>

#include <QFileDialog>

#include <ztoolset/uristring.h>

#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory()

#include <zcontent/zcontentutils/zexceptiondlg.h>

#include <zqt/zqtwidget/zqtableview.h>

#include "texteditmwn.h"

/* for xml exchanges */
#include <zxml/zxmlprimitives.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zcontentcommon/zgeneralparameters.h>


ZRestoreDLg::ZRestoreDLg(QWidget* pParent) : QDialog(pParent)
{
  initLayout();
}

ZRestoreDLg::~ZRestoreDLg()
{
  if (ComLog != nullptr)
    ComLog->deleteLater();
  if (MasterFile.isOpen())
    MasterFile.zclose();
}

void
ZRestoreDLg::initLayout() {

  /* backup dialog setup */

  setWindowTitle("Master file restore");
  resize(1050,550);

  QVBoxLayout* QVL=new QVBoxLayout;
  setLayout(QVL);

  QHBoxLayout* QHL=new QHBoxLayout;
  QVL->insertLayout(0,QHL);

  QLabel* wLblBcksetLBl = new QLabel("Backup set file");
  QHL->addWidget(wLblBcksetLBl);

  BackupSetBl=new QLabel();
  QHL->addWidget(BackupSetBl);

  SearchBcksetBTn = new QPushButton("Search file");
  SearchBcksetBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );
  QHL->addWidget(SearchBcksetBTn);
  QObject::connect(SearchBcksetBTn, &QPushButton::clicked, this, &ZRestoreDLg::searchBcksetClicked);

  QHBoxLayout* QHLDir=new QHBoxLayout;
  QVL->insertLayout(-1,QHLDir);

  QLabel* wLbDirLBl = new QLabel("Backup root directory");
  QHLDir->addWidget(wLbDirLBl);

  TargetRestoreLBl=new QLabel;
  QHLDir->addWidget(TargetRestoreLBl);

  TargetRestoreDirBTn=new QPushButton("Search");
  QHLDir->addWidget(TargetRestoreDirBTn);
  TargetRestoreDirBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );
  QObject::connect(TargetRestoreDirBTn, &QPushButton::clicked, this, &ZRestoreDLg::searchDirClicked);


  QHBoxLayout* QHLBckDate=new QHBoxLayout;
  QVL->insertLayout(-1,QHLBckDate);

  QLabel* wLbBckDateLBl = new QLabel("Backup done on");
  QHLBckDate->addWidget(wLbBckDateLBl);
  BckDateLBl = new QLabel("<no date>");
  QHLBckDate->addWidget(BckDateLBl);


  QHBoxLayout* QHLTargetZMF=new QHBoxLayout;
  QVL->insertLayout(-1,QHLTargetZMF);

  QLabel* wLbTargetZMFLBl = new QLabel("Target master file");
  QHLTargetZMF->addWidget(wLbTargetZMFLBl);
  TargetZMFLBl = new QLabel("<no target>");
  QHLTargetZMF->addWidget(TargetZMFLBl);

  QHBoxLayout* QHLComment=new QHBoxLayout;
  QVL->insertLayout(-1,QHLComment);

  CommentLBl= new QLabel;
  QHLComment->addWidget(CommentLBl);

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
  OkBTn = new QPushButton("Do restore");
  wButtonBoxQHb->addWidget(OkBTn);

  QObject::connect(OkBTn, &QPushButton::clicked, this, &ZRestoreDLg::doRestore);
  QObject::connect(CancelBTn, &QPushButton::clicked, this, &QDialog::reject);

  /* table view setup */

  BckTBv->newModel(8);

  QList<QStandardItem*> wHeaderRow;

  wHeaderRow << new QStandardItem();
  wHeaderRow << new QStandardItem();
  wHeaderRow << new QStandardItem();
  wHeaderRow << new QStandardItem();
  wHeaderRow << new QStandardItem();
  wHeaderRow << new QStandardItem();
  wHeaderRow << new QStandardItem();
  wHeaderRow << new QStandardItem();

  BckTBv->ItemModel->appendRow(wHeaderRow);

  BckTBv->setSpan(0,0,1,4);  /* group columns [0 - 3]  */
  BckTBv->setSpan(0,4,1,3);  /* group columns [4 - 6]  */
  BckTBv->setSpan(0,7,1,2);  /* group columns [7 - 8]  */

  BckTBv->ItemModel->item(0,0)->setText("Backup set");
  BckTBv->ItemModel->item(0,0)->setTextAlignment(Qt::AlignCenter);

  BckTBv->ItemModel->item(0,4)->setText("Target");
  BckTBv->ItemModel->item(0,4)->setTextAlignment(Qt::AlignCenter);

  BckTBv->ItemModel->item(0,7)->setText("Result");
  BckTBv->ItemModel->item(0,7)->setTextAlignment(Qt::AlignCenter);

  wHeaderRow.clear();
  wHeaderRow << new QStandardItem("Base name");
  wHeaderRow << new QStandardItem("Size");
  wHeaderRow << new QStandardItem("Created");
  wHeaderRow << new QStandardItem("Modified");

  wHeaderRow << new QStandardItem("Size");
  wHeaderRow << new QStandardItem("Created");
  wHeaderRow << new QStandardItem("Modified");

  wHeaderRow << new QStandardItem("Action");
  wHeaderRow << new QStandardItem("Status");
  wHeaderRow << new QStandardItem("Comment");
  BckTBv->ItemModel->appendRow(wHeaderRow);

//  BckTBv->horizontalHeader()->hide();

  for (int wi=0; wi < 9; wi++)
    BckTBv->ItemModel->setHorizontalHeaderItem(wi,new QStandardItem(""));

  BckTBv->verticalHeader()->hide();
}

ZStatus
ZRestoreDLg::dataSetup(uriString& pURIBckset, const uriString &pTargetDir) {
  ZDateFull wBackupDate;
  ZStatus wSt = XmlLoadBackupset(pURIBckset,wBackupDate);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  URIBackupsetFile = pURIBckset;
  BackupSetBl->setText(URIBackupsetFile.toCChar());

  BckDateLBl->setText(wBackupDate.toLocale().toCChar());

  TargetZMFBaseName = Files[0].Source.getBasename();
  TargetZMFLBl->setText(TargetZMFBaseName.toCChar());

  if ( (utf8VaryingString&)pTargetDir == Files[0].Source.getDirectoryPath() ){
    CommentLBl->setText("Target directory is the same as backup source directory");
  }
  else
    CommentLBl->setText("Target directory is different from backup source directory");

  utf8VaryingString wMasterRootName = Files[0].Source.getRootname();

  /* remove all rows excepted 2 header rows */
  BckTBv->ItemModel->removeRows(2,BckTBv->ItemModel->rowCount()-2);

  TargetZMFBaseName = pURIBckset.getBasename();
  SourceDirPath = pURIBckset.getDirectoryPath();
  utf8VaryingString wStr;


  /* source :
   * for master file content and header file : only base name
   * for dictionary and index files : full path name is mentionned
   * destination :
   * only computed base names are mentionned
   */

  QList<QStandardItem*> wRow;

  setQtItemAlignment( Qt::AlignRight);

  uriString wURIFile=pTargetDir;

  for (long wi=0; wi < Files.count(); wi++){
    wRow.clear();

    wRow << createItem(Files[wi].Source.getBasename());

    wRow << createItem(Files[wi].Size);
    wRow << createItem(Files[wi].Created.toLocale());
    wRow << createItem(Files[wi].LastModified.toLocale());

    uriString wTarget = RestoreDirectory;
    wTarget.addConditionalDirectoryDelimiter();
    wTarget += Files[wi].Source.getBasename();

    if (wTarget.exists()) {
      uriStat wStat = wTarget.getStatR();
      wRow << createItem((unsigned long)wStat.Size);
      wRow << createItem(wStat.Created.toLocale());
      wRow << createItem(wStat.LastModified.toLocale());

      wRow << createItem("To be replaced");
      wRow << new QStandardItem("");

      if ((wStat.Created == Files[wi].Created) &&
          (wStat.LastModified == Files[wi].LastModified) &&
          (size_t(wStat.Size) == Files[wi].Size) ){
        wRow << new QStandardItem("Same");
      }
      else
        wRow << new QStandardItem("Different");
    }
    else {
      wRow << new QStandardItem("--");
      wRow << new QStandardItem("--");
      wRow << new QStandardItem("--");

      wRow << createItem("To be created");
      wRow << new QStandardItem("");
      wRow << new QStandardItem("");
    }

    BckTBv->ItemModel->appendRow(wRow);
  } // for


  BckTBv->resizeColumnsToContents();
  BckTBv->resizeRowsToContents();

  return ZS_SUCCESS;
}


bool
ZRestoreDLg::searchBcksetClicked() {
  while (true) {
    QString wFileName = QFileDialog::getOpenFileName(this, "Select backup set file",
                                                     GeneralParameters.getWorkDirectory().toCChar(),
                                                     "Backup set files (*.bckset);;All (*.*)");
    if (wFileName.isEmpty()) {
      return false;
    }
    URIBackupsetFile = wFileName.toUtf8().data();
    BackupSetBl->setText(wFileName);

    ZStatus wSt=dataSetup(URIBackupsetFile);
    if (wSt!=ZS_SUCCESS) {
      int wRet = ZExceptionDLg::adhocMessage2B("Search master file",Severity_Error,
          "Quit","Try another",
          "File <%s> appears to be an invalid backup set file.\n"
          "See exception for more information.\n"
          "Try another file or Quit the dialog.",
          URIBackupsetFile.toCChar());
      if (wRet==QDialog::Rejected)
        reject();
      continue;
    }
    break;
  }// while true
  return true;
}// searchBcksetClicked

void
ZRestoreDLg::searchDirClicked() {
  QString wDirName = QFileDialog::getExistingDirectory(this, "Select restore target directory",
                                                       GeneralParameters.getWorkDirectory().toCChar());
  if (wDirName.isEmpty()) {
    return;
  }
  RestoreDirectory = wDirName.toUtf8().data();
  TargetRestoreLBl->setText(wDirName);


//  ZStatus wSt = dataSetup(URIBackupsetFile,RestoreDirectory);


  QList <QStandardItem*> wRow;

  for (long wi=0; wi < Files.count(); wi++) {

    uriString wTarget = RestoreDirectory;
    wTarget.addConditionalDirectoryDelimiter();
    wTarget += Files[wi].Source.getBasename();

    if (wTarget.exists()) {
      uriStat wTargetStat = wTarget.getStatR();

      BckTBv->ItemModel->setItem(int(wi+2),4,createItem((unsigned long)wTargetStat.Size));
      BckTBv->ItemModel->setItem(int(wi+2),5,createItem(wTargetStat.Created.toLocale()));
      BckTBv->ItemModel->setItem(int(wi+2),6,createItem(wTargetStat.LastModified.toLocale()));

      BckTBv->ItemModel->setItem(int(wi+2),7,createItem("To be replaced"));
      BckTBv->ItemModel->setItem(int(wi+2),8,new QStandardItem(""));
      if ((wTargetStat.Created == Files[wi].Created) &&
          (wTargetStat.LastModified == Files[wi].LastModified) &&
          (size_t(wTargetStat.Size) == Files[wi].Size) ){
        BckTBv->ItemModel->setItem(int(wi+2),9,new QStandardItem("Same"));
      }
      else
        BckTBv->ItemModel->setItem(int(wi+2),9,new QStandardItem("Different"));
      continue;
    }

    BckTBv->ItemModel->setItem(int(wi+2),4,new QStandardItem("--"));
    BckTBv->ItemModel->setItem(int(wi+2),5,new QStandardItem("--"));
    BckTBv->ItemModel->setItem(int(wi+2),6,new QStandardItem("--"));

    BckTBv->ItemModel->setItem(int(wi+2),7,createItem("To be created"));
    BckTBv->ItemModel->setItem(int(wi+2),8,new QStandardItem(""));
    BckTBv->ItemModel->setItem(int(wi+2),9,new QStandardItem(""));

  }// for

  BckTBv->resizeColumnsToContents();
  BckTBv->resizeRowsToContents();
}


void
ZRestoreDLg::doRestore() {
  if (ComLog==nullptr) {
    ComLog = new textEditMWn(this,TEOP_NoFileLab | TEOP_NoCloseBtn,&ComLog);
    ComLog->setWindowTitle("Restore log");
  }
  ComLog->show();
  utf8VaryingString wErrMsg;

  if (URIBackupsetFile.isEmpty()) {
    wErrMsg = "No backup set selected.";
  }
  if (RestoreDirectory.isEmpty()){
    if (!wErrMsg.isEmpty())
      wErrMsg+= "\n";
    wErrMsg = "Restore target directory has not been defined.";
  }
  if (!wErrMsg.isEmpty()) {
    ZExceptionDLg::adhocMessage("Do restore",Severity_Error,wErrMsg.toCChar());
    return;
  }

  size_t wSize=0;
  for (long wi=0; wi < Files.count();wi++)
    wSize += Files[wi].Size;

  ProgressPBr->setMinimum(0);
  ProgressPBr->setMaximum(int(wSize));
  ProgressPBr->setValue(0);
  ProgressPBr->setVisible(true);

  uriString wSourceDir = URIBackupsetFile.getDirectoryPath();
  wSourceDir.addConditionalDirectoryDelimiter();
  uriString wSourceFile;
  uriString wTargetDir = RestoreDirectory;
  uriString wTargetFile;

  utf8VaryingString wAction;
  wTargetDir.addConditionalDirectoryDelimiter();

  ComLog->appendText("Restoring files");

  int wSizeCur=0;

  long wi=0;
  for (; wi < Files.count(); wi++) {
    wSourceFile = wSourceDir;
    wSourceFile += Files[wi].Target;


    wTargetFile = wTargetDir;
    wTargetFile += Files[wi].Target;
    if (wTargetFile.exists()) {
      wAction = "Replace";
    }
    else
      wAction = "Create";
    ZStatus wSt = uriString::copyFile(wTargetFile,wSourceFile);

    while (true) {
      if (wSt==ZS_SUCCESS) {
        ComLog->appendText("Created file from <%s> to <%s>",wSourceFile.toString(),wTargetFile.toString());
        break;
      }
      if (wSt==ZS_FILEREPLACED) {
        ComLog->appendText("Replaced file from <%s> to existing file <%s>",wSourceFile.toString(),wTargetFile.toString());
        break;
      }
        ZException.setMessage("ZRestoreDLg::doRestore",wSt,Severity_Error,
            "Error while copying file <%s> to backup file <%s>",
            wSourceFile.toString(),wTargetFile.toString());
        ComLog->appendText("Problem while backing up file <%s> to <%s>\n"
                           "Exception follows\n%s",
            wSourceFile.toString(),wTargetFile.toString(),
            ZException.last().formatFullUserMessage().toString());
        QPalette p = ProgressPBr->palette();
        p.setColor(QPalette::Highlight, Qt::red);
        ProgressPBr->setPalette(p);
        break;
    }// while true

    BckTBv->ItemModel->item(int(wi+2),7)->setText(wAction.toCChar());
    BckTBv->ItemModel->item(int(wi+2),8)->setText(decode_ZStatus(wSt));

    wSizeCur += int(Files[wi].Size) ;
    ProgressPBr->setValue(wSizeCur);

    BckTBv->resizeColumnsToContents();
    BckTBv->resizeRowsToContents();

  }// for

  ComLog->appendText("Restored %ld files",wi);

  return ;
}// doBackup



ZStatus
ZRestoreDLg::XmlLoadBackupset(const uriString& pXmlFile,ZDateFull& pBackupDate)
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

  wSt=XMLgetChildZDateFull(wRoot,"date",pBackupDate);

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

    Files.push(wElt);

    wSt=wSingleFileNode->getNextNode((zxmlNode*&)wSwapNode);
    XMLderegister(wSingleFileNode);
    wSingleFileNode=wSwapNode;
  } // while

  XMLderegister(wSingleFileNode);
  XMLderegister(wFileRootNode);
  XMLderegister(wRoot);

  return ZS_SUCCESS;
} // ZRestoreDLg::XmlLoadBackupset
/*
BckElement
ZRestoreDLg::getElt(const uriString& pSource,const uriString& pTargetDir) {
  BckElement wElt;
  uriStat wStat;
  wElt.Source = pSource ;
  wElt.Size = wElt.Source.getFileSize();
  ZStatus wSt=wElt.Source.getStatR(wStat);
  if (wSt!=ZS_SUCCESS) {
    wElt.Status = wSt;
  }
  wElt.SourceCreation = wStat.Created;
  wElt.SourceLastModified = wStat.LastModified;

  if (pTargetDir.isEmpty())
    return wElt;

  wElt.Target = pTargetDir;
  wElt.Target += wElt.Source.getBasename();
  return wElt;
}
*/
