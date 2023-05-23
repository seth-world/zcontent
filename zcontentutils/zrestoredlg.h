#ifndef ZRESTOREDLG_H
#define ZRESTOREDLG_H

#include <QDialog>

#include <ztoolset/uristring.h>
#include <zcontent/zindexedfile/zmasterfile.h>

#include "bckelement.h"

class QLabel;
class QLineEdit;
class ZQTableView;
class QProgressBar;

class textEditMWn;


class ZRestoreDLg : public QDialog
{
Q_OBJECT

public:
  explicit ZRestoreDLg(QWidget* pParent);
  ~ZRestoreDLg();

  void initLayout();

  ZStatus dataSetup(uriString &pURIMaster, const uriString &pTargetDir="");

//  ZStatus XmlSaveBackupset(uriString& pXmlFile, bool pComment=true);
  ZStatus XmlLoadBackupset(const uriString &pXmlFile, ZDateFull &pBackupDate);

//  BckElement getElt(const uriString& pSource, const uriString& pTargetDir=uriString());

private slots:
  bool searchBcksetClicked() ;
  void searchDirClicked() ;

  void doRestore();


private:
  ZMasterFile       MasterFile;

  ZArray<BckElement> Files;

  uriString         URIBackupsetFile;
  utf8VaryingString TargetZMFBaseName;
  uriString         SourceDirPath;
  uriString         RestoreDirectory;

//  utf8VaryingString BckName="bck";
//  int               BckNumber=1;

  QLabel*       BackupSetBl=nullptr;
  QLabel*       TargetRestoreLBl=nullptr;
  QLabel*       BcksetLBl=nullptr;
  QLabel*       BckDateLBl=nullptr;
  QLabel*       TargetZMFLBl=nullptr;
  QLabel*       CommentLBl=nullptr;
  ZQTableView*  BckTBv=nullptr;
  QProgressBar* ProgressPBr=nullptr;


  QPushButton* SearchBcksetBTn=nullptr;
  QPushButton* TargetRestoreDirBTn=nullptr;
  QPushButton* OkBTn=nullptr;
  QPushButton* CancelBTn=nullptr;

  textEditMWn* ComLog=nullptr;

};

#endif // ZRESTOREDLG_H
