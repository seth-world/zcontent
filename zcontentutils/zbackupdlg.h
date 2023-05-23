#ifndef ZMFBCKDLG_H
#define ZMFBCKDLG_H

#include <QDialog>

#include <ztoolset/uristring.h>
#include <zcontent/zindexedfile/zmasterfile.h>


#include "bckelement.h"

class QLabel;
class QLineEdit;
class ZQTableView;
class QProgressBar;

class textEditMWn;




class ZBackupDLg : public QDialog
{
Q_OBJECT

public:
  explicit ZBackupDLg(QWidget* pParent);
  ~ZBackupDLg();

  void initLayout();

  ZStatus dataSetup(uriString &pURIMaster, const uriString &pTargetDir="");

  ZStatus XmlSaveBackupset(uriString& pXmlFile, bool pComment=true);
  ZStatus XmlLoadBackupset(const uriString &pXmlFile, ZDateFull &pBackupDate);

  BckElement getElt(const uriString& pSource, const uriString& pTargetDir=uriString());

private slots:
  bool searchFileClicked() ;
  void searchDirClicked() ;
  void bckextChanged();

  void doBackup();

  size_t computeAllSizes();

private:
  ZMasterFile       MasterFile;

  ZArray<BckElement> Files;

  uriString         URIMasterFile;
  utf8VaryingString MasterBaseName;
  uriString         SourceDirPath;
  uriString         BckRootDirectory;

  utf8VaryingString BckName="bck";
  int               BckNumber=1;

  QLabel*       URIMasterFileLBl=nullptr;
  QLabel*       IndexPathLBl=nullptr;
  QLabel*       BckRootDirectoryLBl=nullptr;
  QLineEdit*    BckNameLEd=nullptr;
  ZQTableView*  BckTBv=nullptr;
  QProgressBar* ProgressPBr=nullptr;


  QPushButton* SearchZMFBTn=nullptr;
  QPushButton* BckDirBTn=nullptr;
  QPushButton* OkBTn=nullptr;
  QPushButton* CancelBTn=nullptr;

  textEditMWn* ComLog=nullptr;
};

#endif // ZMFBCKDLG_H
