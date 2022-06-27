#ifndef ZCONTENTVISUMAIN_H
#define ZCONTENTVISUMAIN_H

#include <QMainWindow>
#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zarray.h>


#include <zcontent/zrandomfile/zrandomfiletypes.h>
#include <zcontent/zrandomfile/zblock.h>

#include <zcontent/zrandomfile/zrandomfile.h>

#define __DISPLAYCALLBACK__(__NAME__)  std::function<void (const utf8VaryingString&)> __NAME__
#define __PROGRESSCALLBACK__(__NAME__)  std::function<void (int)> __NAME__


namespace Ui {
class ZContentVisuMain;
}
class QStandardItemModel;
class QActionGroup;
class QAction;
class QFileDialog;
namespace zbs {
//class ZRandomFile;
class ZMasterFile;
}


//class textEditMWn;
enum VisuMode_type : uint8_t
{
  VMD_Nothing = 0,
  VMD_RawSequential = 1,
  VMD_RawRandom     = 2,
  VMD_Random        = 3,
  VMD_Master        = 4
};

class DisplayMain;
class DicEdit;
class textEditMWn;

class ZContentVisuMain : public QMainWindow
{
  Q_OBJECT

public:
  explicit ZContentVisuMain(QWidget *parent = nullptr);
  ~ZContentVisuMain();


  ZStatus openZRF(const char *pFilePath);
  ZStatus openZMF(const char* pFileName);
  ZStatus openOther(const char *pFileName);

  ZStatus unlockZRFZMF(const char* pFilePath);

  ZStatus displayFdNextRawBlock(ssize_t pBlockSize, size_t pWidth=16);



  ZStatus displayListZRFNextRecord(size_t pWidth=16);
  ZStatus displayListZRFFirstRecord(size_t pWidth=16);
  ZStatus displayListZRFPreviousRecord(size_t pWidth=16);

  bool displayWidgetBlockOnce=false;
  ZStatus displayWidgetBlock(ZDataBuffer& pData,int pWidth);

  void displayBlockData();

  void ZRFUnlock();
  void ZHeaderRawUnlock();

  void displayHCB();
  void displayFCB();
  void displayMCB();


  void displayPool(unsigned char* pPtr,zaddress_type pOffset,const char* pTitle);

  void displayZBAT();
  void displayZFBT();
  void displayZDBT();

  void Dictionary();

  void surfaceScanZRF();
  void surfaceScanRaw();


  void repairIndexes(bool pTestRun, bool pRebuildAll);
  void removeIndex();
  void addIndex();
  void rebuildIndex();
  void extractIndex();
  void dowdgrade();
  void reorganizeZMF();
  void reportMCB();


  void getFileSize(const char *pFilePath);
  void setFileType (const char* pFilePath);

  void removeAllRows();


  void textEditMorePressed();
  void closeMCBCB(const QEvent *pEvent);
  void closeGenlogCB(const QEvent *pEvent);

  textEditMWn* openGenLogWin();
/*
  void _print(const utf8VaryingString& pOut);
  void _print(const char* pFormat,...);

  __DISPLAYCALLBACK__(_displayCallBack)=nullptr;
*/
  DisplayMain* entityWnd=nullptr;
  DicEdit* dictionaryWnd=nullptr;

  textEditMWn* MCBWin=nullptr;
  textEditMWn* GenlogWin=nullptr;


  zbs::ZRandomFile* RandomFile=nullptr;
  zbs::ZMasterFile* MasterFile=nullptr;

  int   Fd=-1;
  long  FileOffset=0;
  long  BlockSize=64;
  const int Width=16;

  bool AllFileLoaded=false;
  bool HeaderFile = false;

  long RecordNumber=0;
  zaddress_type Address=0;

  ZBlock          Block;

  ZArray<long>  Addresses;

  uint8_t OpenMode = VMD_Nothing;

  uriString     URICurrent;

  ZDataBuffer   RawData;

//  QStandardItemModel* TBlItemModel =nullptr;

  QActionGroup* actionGroup=nullptr;

private slots:

  void actionMenuEvent(QAction* pAction);
  void actionOpenFileByType(bool pChecked=true);
  void openRaw();
  void actionClose(bool pChecked=true);

  void chooseFile(bool pChecked);


  void backward();
  void forward();
  void loadAll();

private:
  Ui::ZContentVisuMain *ui;
};

utf8String formatSize(long long wSize);

#endif // ZCONTENTVISUMAIN_H
