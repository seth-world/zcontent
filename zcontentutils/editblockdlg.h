#ifndef EDITBLOCKDLG_H
#define EDITBLOCKDLG_H

#include <config/zconfig.h>

#include <QDialog>

enum EdBlk : uint8_t {
  EBK_Nothing = 0,
  EBK_FileChanged = 1 ,
  EBK_PoolChanged = 2
};

#include <zcontent/zrandomfile/zblock.h>

class QLabel;

class editBlockDLg : public QDialog
{
 Q_OBJECT
public:
  editBlockDLg(QWidget* pParent);
  void setup(__FILEHANDLE__ pContentFd,__FILEHANDLE__ pHeaderFd,int pPoolId,long pDataRank,ZBlockDescriptor* pBD);
private slots:
  void FileToPool();
  void PoolToFile();
  void Unlock();
  void ShowDetails();
  void CommitChanges();
  void Quit();
private:
  QLabel* BlockSize=nullptr;
  QLabel* BlockSizeF=nullptr;

  QLabel* State=nullptr;
  QLabel* StateF=nullptr;

  QLabel* Lock=nullptr;
  QLabel* LockF=nullptr;

  QLabel* Pid=nullptr;
  QLabel* PidF=nullptr;

  QPushButton* CommitBTn=nullptr;
  QPushButton* PoolToFileBTn=nullptr;
  QPushButton* FileToPoolBTn=nullptr;

  QPalette ErrorPalette;
  QPalette NormalPalette;

  ZBlockDescriptor* BD=nullptr;
  ZBlockDescriptor FBD; /* block descriptor on file */

  bool FileIsAccessible=false;
  bool FileDiffersFromPool=false;
  ZStatus FileBlockDescStatus=ZS_SUCCESS;
  ZStatus NextStartSignStatus=ZS_SUCCESS;

  int8_t          HasChanged=0;
  size_t          FileSize=0;
  __FILEHANDLE__  ContentFd=-1;
  __FILEHANDLE__  HeaderFd=-1;
};

#endif // EDITBLOCKDLG_H
