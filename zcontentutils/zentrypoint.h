#ifndef ZEntryPoint_H
#define ZEntryPoint_H

#include <config/zconfig.h>

#include <QMainWindow>
#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zarray.h>


#include <zcontent/zrandomfile/zrandomfiletypes.h>
#include <zcontent/zrandomfile/zblock.h>

#include <zcontent/zrandomfile/zrandomfile.h>
#include <QModelIndex>

#include <ztoolset/zaierrors.h>

extern const int cst_maxraisonablevalue;

class ZQTableView;
class VisuRaw;

#define __DISPLAYCALLBACK__(__NAME__)  std::function<void (const utf8VaryingString&)> __NAME__
#define __PROGRESSCALLBACK__(__NAME__)  std::function<void (int)> __NAME__

#ifndef __VISUMODE__
#define __VISUMODE__
enum VisuMode_type : uint8_t
{
    VMD_Nothing = 0,
    VMD_RawSequential = 1,
    VMD_RawRandom     = 2,
    VMD_Random        = 3,
    VMD_Master        = 4,
    VMD_Header        = 5
};
#endif
/*
namespace Ui {
class ZEntryPoint;
}
*/




namespace zbs {
//class ZRandomFile;
class ZRawMasterFile;
}



class DisplayMain;
class textEditMWn;
class DicEditMWn;

class QLabel;
class QAction;
class QActionGroup;
class QPushButton;
class QMenu;
class QStatusBar;
class QMenuBar;

class QVBoxLayout;


extern class DicEditMWn* DicEdit;



class ZEntryPoint : public QMainWindow
{
  Q_OBJECT

public:
  explicit ZEntryPoint(QWidget *parent = nullptr);
  ~ZEntryPoint();


  ZStatus ZRFAccessByPool();
  ZStatus ZRFSurfaceAccess();

  ZStatus openZRH();

  bool displayWidgetBlockOnce=false;
  void displayOneLine(int pRow,unsigned char* &wPtr,unsigned char* wPtrEnd);

  void displayBlockData();

  void ZRFUnlock();  /* uses header unlock but access is made thru content file uri */


  static bool testRequestedSize(const uriString &pURI, ZDataBuffer &pRawData, size_t pRequestedSize);

  void displayHCB();
  void displayFCB();
  void displayMCB();
  void displayICBs();

  static ZStatus exportZMF(QWidget *pParent, zbs::ZRawMasterFile *pRawMasterFile);
  ZStatus exportZMFFromSurface();
  static ZStatus importZMF(QWidget* pParent,  ZaiErrors *pErrorLog);
  static ZStatus clearZMF(QWidget *pParent);

  ZStatus cloneZRF();
  ZStatus clearZRF();
  ZStatus ZRFrebuildHeader();
  ZStatus ZRFSurfaceScan();
  ZStatus ZRFupgradetoZMF();
  ZStatus ZMFdowngradetoZRF();
  ZStatus ZRFReorganize();
  ZStatus ZRFPutthemessinZBAT();
  ZStatus ZRFCopy();
  ZStatus ZRFRemove();

  void displayPool(const unsigned char *pPtr,
                   zaddress_type pOffset,
                   const utf8VaryingString &pTitle);

  void displayZBAT();
  void displayZFBT();
//  void displayZDBT();  // Deprecated

  void repairIndexes(bool pTestRun, bool pRebuildAll);
  void removeIndex();
  void addIndex();
  void rebuildIndex();
  void extractIndex();
  void dowdgrade();
  void reorganizeZMF();


  void initLayout();

  void textEditMorePressed();
  void closeMCBCB(const QEvent *pEvent);
//  void closeGenlogCB(const QEvent *pEvent);

//  textEditMWn* openGenLogWin();
/*
  void GenlogWindisplayErrorCallBack(const utf8VaryingString& pMessage) ;
  void GenlogWindisplayErrorColorCB(uint8_t pSeverity,const utf8VaryingString& pMessage);
*/
  void initLogWindow();

  void showStatusBar(const utf8VaryingString& pString);
  void showStatusBar(const char* pFormat,...);

  DisplayMain* entityWnd=nullptr;
  DicEditMWn* DicEdit=nullptr;
  textEditMWn* MCBWin=nullptr;

  ZaiErrors     ErrorLog;

//  zbs::ZRandomFile*     RandomFile=nullptr;
  zbs::ZRawMasterFile*  RawMasterFile=nullptr;

//  __FILEHANDLE__    Fd=-1;
  long              FileOffset=0;
  long              BlockSize=64;
  const int         Width=16;

  bool              AllFileLoaded=false;
  bool              HeaderFile = false;

  long RecordNumber=0;
  zaddress_type Address=0;

  ZBlock          Block;

  ZArray<long>  Addresses;

  uint8_t OpenMode = VMD_Nothing;

  uriString     URICurrent;

  uriString     URIHeader;
  ZDataBuffer   HeaderData;

  ZDataBuffer   RawData;

/*---------------------------------------------*/

  QMenuBar *menubar=nullptr;

/*----------------------------------*/
  QMenu *generalMEn=nullptr;

  QMenu*        ParametersQMe=nullptr;
  QAction*      ParamLoadQAc=nullptr;
  QAction*      ParamChangeQAc=nullptr;
  QAction *     QuitQAc=nullptr;

  QAction*      QuitIconQAc=nullptr;

/*----------------------------------*/
  QMenu *rawMEn=nullptr;
  QAction *rawDumpQAc=nullptr;
  QAction *surfaceScanRawQAc=nullptr;
/*----------------------------------*/

  QMenu *headerMEn=nullptr;
  QAction *openZRHQAc = nullptr;
  QAction *displayFCBQAc=nullptr;
  QAction *displayHCBQAc=nullptr;
  QAction *displayMCBQAc=nullptr;
  QAction* displayICBQAc = nullptr;

  QMenu *viewpoolMEn=nullptr;
  QAction *displayZBATQAc = nullptr;
  //  QAction *displayZDBTQAc = nullptr;
  QAction *displayZFBTQAc = nullptr;

  /*----------------------------------*/
  QMenu*        RandomFileMEn=nullptr;

  QAction*      ZRFPoolAccessQAc=nullptr;
  QAction*      ZRFSurfaceAccessQAc=nullptr;
  QAction*      ZRFCloneQAc=nullptr;
  QAction*      ZRFClearQAc=nullptr;
  QAction*      ZRFReorgQAc=nullptr;
  QAction*      ZRFRebuildHeaderQAc=nullptr;
  QAction*      ZRFtruncateQAc=nullptr;/* not implemented yet */
  QAction*      ZRFextendQAc=nullptr;  /* not implemented yet */
  QAction*      ZRFunlockQAc=nullptr;

  QAction*      ZRFSurfaceScanQAc=nullptr;

  QAction *ZRFupgradetoZMFQAc=nullptr;

  /*----------------------------------*/
  QMenu *       MasterFileMEn=nullptr;

  QAction *dictionaryQAc = nullptr;
  QAction *ZMFdowngradetoZRFQAc=nullptr;
  QAction *ZMFreorganizeQAc=nullptr;
  QAction*      ZmfDefQAc = nullptr;
  QAction*      IndexRebuildQAc = nullptr;
  QAction*      ZMFBackupQAc=nullptr;
  QAction*      ZMFRestoreQAc=nullptr;
  QAction*      ZMFExportQAc=nullptr;
  QAction*      ZMFExportFromSurfaceQAc=nullptr;
  QAction*      ZMFImportQAc=nullptr;
  QAction*      ZMFClearQAc=nullptr;
  QAction*      ZMFtruncateQAc=nullptr; /* not implemented yet */
  QAction*      ZMFextendQAc=nullptr;  /* not implemented yet */

   /*----------------------------------*/

  QAction*      ZMFQueryQAc=nullptr;
   /*----------------------------------*/
  QAction*      ZCopyQAc=nullptr;
   /*----------------------------------*/

  QAction *extractIndexQAc=nullptr;
  QAction *extractAllIndexesQAc;
  QAction *testRunQAc=nullptr;
  QAction *rebuilAllQAc=nullptr;
  QAction *runRepairQAc=nullptr;
  QAction *DictionaryQAc = nullptr;
  QAction *cppparserQAc = nullptr;

  QActionGroup* mainQAg=nullptr;

  QAction*      ZRFPutthemessQAc=nullptr;

  // QAction*      openZRFQAc=nullptr;
//  VisuRaw*      VizuRaw=nullptr;

  QWidget *centralwidget=nullptr;
/*  QLabel *OpenModeLbl;
  QLabel *FileTypeLBl;
  QLabel *FullPathLbl;
*/
  QStatusBar *statusbar;

/*---------------------------------------------*/



 // void resizeEvent(QResizeEvent*) override;

  textEditMWn* TEMWn=nullptr;

private slots:

  void actionMenuEvent(QAction* pAction);
  void actionClose(bool pChecked=true);

private:

  void DicEditQuitCallback();


  bool              FResizeInitial=true;
 // Ui::ZEntryPoint *ui;
};


#endif // ZEntryPoint_H
