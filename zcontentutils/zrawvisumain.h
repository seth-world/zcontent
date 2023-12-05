#ifndef ZRAWVISU_H
#define ZRAWVISU_H

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

enum ScanBlock_type : uint8_t {
  SBT_Nothing     = 0,
  SBT_Correct     = 1,
  SBT_InvHeader   = 2,
  SBT_MissEB      = 4,  /* end block is missing */
  SBT_InvBlkSize  = 8   /* end block -> computed size is not equal to header block size */
};


class ScanBlock {
public:
  ScanBlock() = default;
  ScanBlock(const ScanBlock& pIn) {_copyFrom(pIn);}

  ScanBlock& _copyFrom(const ScanBlock& pIn) {
    Begin=pIn.Begin;
    End=pIn.End;
//    EndBlock=EndBlock;
    BlockHeader = pIn.BlockHeader;
    return *this;
  }

  void clear(){
    Begin=0;
    End=0;
//    EndBlock=0;
    BlockHeader.clear();
  }

  ScanBlock& operator =(const ScanBlock& pIn) {return _copyFrom(pIn);}

  void setInvalid (){
    BlockHeader.clear();
    BlockHeader.State = ZBS_Invalid;
  }

  zaddress_type Begin=0;
  zaddress_type End=0;
//  ssize_t       EndBlock=-1;
  ZBlockHeader  BlockHeader;
};


/*
namespace Ui {
class ZContentVisuMain;
}
*/




namespace zbs {
//class ZRandomFile;
class ZRawMasterFile;
}

#ifndef __VISUMODE__
#define __VISUMODE__
//class textEditMWn;
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

class DisplayMain;
class textEditMWn;
class DicEditMWn;

class QLabel;
class QLineEdit;
class QComboBox;
class QAction;
class QStandardItemModel;
class QActionGroup;
class QPushButton;
class QMenu;
class QStatusBar;
class QMenuBar;
class QFrame;
class QProgressBar;
class QVBoxLayout;
class QCheckBox;


extern class DicEditMWn* DicEdit;

enum ZCVM_VisuType  {
    ZCVM_Nothing        = 0,
    ZCVM_Raw            = 1,
    ZCVM_RandomFile     = 2,
    ZCVM_MasterFile     = 3
};

class ZRawVisuMain : public QMainWindow
{
public:
  explicit ZRawVisuMain(QWidget *parent = nullptr);
  ~ZRawVisuMain();

  ZStatus setUpRawFile(const uriString& pFile);
  ZStatus openZRF();
  ZStatus openZMF(const uriString &pFileName);
  ZStatus openZRH(const uriString &pFileName);
  ZStatus openOther(const uriString &pFile);

  ZStatus openZRH();

  void    getRaw();

  ZStatus unlockZRFZMF(const char* pFilePath);

  ZStatus displayFdNextRawBlock(ssize_t pBlockSize, size_t pWidth=16);

  ZStatus displayListZRFNextRecord(size_t pWidth=16);
  ZStatus displayListZRFFirstRecord(size_t pWidth=16);
  ZStatus displayListZRFPreviousRecord(size_t pWidth=16);

  bool displayWidgetBlockOnce=false;
  ZStatus displayWidgetBlock(ZDataBuffer &pData, size_t &pFileOffset);
  void displayOneLine( int pRow, unsigned char *&pPtr, unsigned char *pPtrEnd, size_t pFileOffset, bool pUseHexa);

  void displayBlockData();

  void ZRFUnlock();
  void ZHeaderRawUnlock();

  static bool testRequestedSize(const uriString &pURI, ZDataBuffer &pRawData, size_t pRequestedSize);

  void displayHCB();
  void displayFCB();
  void displayMCB();
  void displayICBs();

  static ZStatus exportZMF(QWidget *pParent, zbs::ZRawMasterFile* pRawMasterFile, ZaiErrors *pErrorLog);
  static ZStatus importZMF(QWidget* pParent,  ZaiErrors *pErrorLog);
  static ZStatus clearZMF(QWidget* pParent,  ZaiErrors *pErrorLog);

  ZStatus cloneZRF();
  ZStatus clearZRF();
//  ZStatus rebuildHeaderZRF();
 // ZStatus reorganizeZRF();

  void displayPool(const unsigned char* pPtr,zaddress_type pOffset,const char* pTitle);

  void displayZBAT();
  void displayZFBT();
//  void displayZDBT();  // Deprecated

  void surfaceScanZRF();
  void displayRawSurfaceScan(const uriString& pFile);

  ZStatus surfaceScanRaw(const uriString & pURIContent, FILE *pOutput=stdout);

  void repairIndexes(bool pTestRun, bool pRebuildAll);
  void removeIndex();
  void addIndex();
  void rebuildIndex();
  void extractIndex();
  void dowdgrade();
  void reorganizeZMF();
  void reportMCB();

  void setPayLoad(size_t pPayLoad) {PayLoad=pPayLoad;}
  ZStatus displayRawFromOffset(size_t pPayLoad);

  void setFileDescription (const uriString &pFilePath);
  void setFileType(const uriString &pFilePath);

  void removeAllRows();
  void setUseHexa(bool pOnOff) {UseHexa = pOnOff;}

  void textEditMorePressed();
  void closeMCBCB(const QEvent *pEvent);
  void closeGenlogCB(const QEvent *pEvent);

  textEditMWn* openGenLogWin();
  void GenlogWindisplayErrorCallBack(const utf8VaryingString& pMessage) ;
  void GenlogWindisplayErrorColorCB(uint8_t pSeverity,const utf8VaryingString& pMessage);

  void initLogWindow();

  DisplayMain* entityWnd=nullptr;

  textEditMWn* MCBWin=nullptr;
  textEditMWn* GenlogWin=nullptr;

  ZaiErrors     ErrorLog;

  zbs::ZRandomFile*     RandomFile=nullptr;
  zbs::ZRawMasterFile*  RawMasterFile=nullptr;

  __FILEHANDLE__    Fd=-1;
  long              FileOffset=0;
  size_t            FileSize=0;
  long              BlockSize=64;
  const int         Width=16;

  bool              AllFileLoaded=false;
  bool              HeaderFile = false;

  size_t            PayLoad=2500;

  long RecordNumber=0;
  zaddress_type Address=0;

  ZBlock          Block;

  ZArray<long>  Addresses;

  uint8_t OpenMode  = VMD_Nothing;
  bool    UseHexa   = false;
  class DicEditMWn* DicEdit=nullptr;
  uriString     URICurrent;
  ZDataBuffer   RawData;

/*---------------------------------------------*/

    void initLayout();

  QAction *openByTypeQAc;
  QAction *rawQAc;
  QAction *ZRFRecordsQac;
  QAction *actionSuface;
  QAction *surfaceScanRawQAc;
  QAction *RecoveryQAc;
  QAction *closeQAc;
  QAction *rawDumpQAc;
  QAction *surfaceScanZRFQAc;
  QAction *displayHCBQAc;
  QAction *QuitQAc;
  QAction *displayFCBQAc;
  QAction *displayMCBQAc;
  QAction *setfileQAc;
  QAction *openRawQAc;
  QAction *openZRFQAc;
  QAction *openZMFQAc;
  QAction *actionHeader_file;
  QAction *unlockZRFQAc;
  QAction *headerFileUnlockQAc;
 // QAction *HeaderRawUnlockQAc;
  QAction *listIndexesQAc;
  QAction *clearQAc;
  QAction *cloneQAc;
  QAction *truncateQAc;
  QAction *extendQAc;
  QAction *rebuildHeaderQAc;
  QAction *reorganizeQAc;
  QAction *downgradeZMFtoZRFQAc;
  QAction *upgradeZRFtoZMFQAc;
  QAction *reorganizeZMFQAc;
  QAction *removeIndexQAc;
  QAction *addIndexQAc;
  QAction *rebuildIndexQAc;
  QAction *extractIndexQAc;
  QAction *extractAllIndexesQAc;
  QAction *testRunQAc;
  QAction *rebuilAllQAc;
  QAction *runRepairQAc;
  QAction *MCBReportQAc;
  QAction *DictionaryQAc;
  QAction *displayZBATQAc;
  QAction *displayZDBTQAc;
  QAction *displayZFBTQAc;
  QAction *dictionaryQAc;
  QAction *dicLoadXmlQAc;
  QAction *cppparserQAc;
  QAction *openZRHQAc;
  QWidget *centralwidget=nullptr;
  QFrame *MainFRm;
  QLabel *OpenModeLbl;
  QLabel *FileTypeLBl;
  QLabel *FullPathLbl;
  QPushButton *BackwardBTn;
  QPushButton *ForwardBTn;
  QFrame *RecordFRm;
  QLabel *RecordNumberLBl;
  QLabel *AddressLBl;
  QLabel *BlockSizeLBl;
  QLabel *UserSizeLBl;
  QLabel *StateLBl;
  QLabel *LockMaskLBl;
  QLabel *RecordTotalLBl;
  QLabel *FileSizeLBl;
  QPushButton *LoadAllBTn;
  QProgressBar *ProgressPGb=nullptr;
  QFrame *SequentialFRm;
  QLineEdit *ReadSizeLBl;

  QCheckBox* ViewHexaCHk=nullptr;


  QLabel *InterPunctLBl=nullptr;
  QLabel *FullSizeLBl=nullptr;
  QLabel *PayLoadLBl=nullptr;
  QLabel *ZRFVersionLBl;
  QLabel *ZMFVersionLBl;
//  QLabel *ClosedLBl;
  QLineEdit *searchLEd;
  QComboBox *searchTypeCBx;
  QPushButton *searchFwdBTn;
  QLabel *CurAddressLBl;
  QPushButton *searchBckBTn;
  QWidget *verticalLayoutWidget;
  QVBoxLayout *verticalLayout;
  QMenuBar *menubar;
  QMenu *fileMEn=nullptr;
  QMenu *ZRFMEn;
  QMenu *ZMFMEn=nullptr;
  QMenu *menurepair_all_indexes;
  QMenu *rawMEn;
  QMenu *headerMEn;
  QMenu *menuView_pool;
  QStatusBar *statusbar;
  QToolBar *toolBar;

/*---------------------------------------------*/

  QActionGroup* mainQAg=nullptr;
//  QAction*      DictionaryQAc = nullptr;
  QAction*      GetRawQAc=nullptr;
  QMenu *       MasterFileMEn=nullptr;
  QAction*      ZmfDefQAc = nullptr;
  QAction*      IndexRebuildQAc = nullptr;
  QAction*      ZMFBackupQAc=nullptr;
  QAction*      ZMFRestoreQAc=nullptr;
  QAction*      ZMFQueryQAc=nullptr;
  QAction*      ZMFExportQAc=nullptr;
  QAction*      ZMFImportQAc=nullptr;
  QAction*      ZMFClearQAc=nullptr;

  QMenu*        RandomFileMEn=nullptr;

  QAction*      ZRFCloneQAc=nullptr;
  QAction*      ZRFClearQAc=nullptr;
  QAction*      ZRFReorgQAc=nullptr;
  QAction*      ZRFRebuildHeaderQAc=nullptr;
  QAction*      ZRFSurfaceScanQAc=nullptr;

  QMenu*        ParametersQMe=nullptr;
  QAction*      ParamLoadQAc=nullptr;
  QAction*      ParamChangeQAc=nullptr;
 // QAction*      openZRFQAc=nullptr;
  VisuRaw*      VizuRaw=nullptr;

  /* evaluate actions */
  QAction* ZBlockHeaderQAc = nullptr;
  QAction* URFFieldQAc = nullptr;

  QAction* ZTypeQAc = nullptr;
  QAction* uint16QAc = nullptr;
  QAction* int16QAc = nullptr;
  QAction* uint32QAc = nullptr;
  QAction* int32QAc = nullptr;
  QAction* uint64QAc = nullptr;
  QAction* int64QAc = nullptr;
  QAction* sizetQAc = nullptr;
  QAction* floatQAc = nullptr;
  QAction* doubleQAc = nullptr;
  QAction* longdoubleQAc = nullptr;


  bool searchHexa(bool pReverse=false);
  bool searchAscii(bool pCaseRegardless=false, bool pReverse=false);

  void setSearchOffset(ssize_t pOffset);

  void setSelectionBackGround(QVariant &pBackground, ssize_t pOffset, size_t pSize, bool pScrollTo=false);

  void resizeEvent(QResizeEvent*) override;

private slots:
  void searchFwd();
  void searchBck();
  void UserOffsetChanged(const QString & pText);
  void ViewHexaChanged(bool pToggled);
  void searchCBxChanged(int pIndex);
  void actionMenuEvent(QAction* pAction);
  void actionOpenFileByType(bool pChecked=true);
  void openRaw();
  void actionClose(bool pChecked=true);

//  bool chooseFile(bool pChecked);

  void VisuClicked(QModelIndex pIdx);


  void backward();
  void forward();
  ZStatus loadAll();
  ZStatus loadRawPayLoad();
  // Deprecated
  //  void visuActionEvent(QAction* pAction);
  //  void visuActionEventOld(QAction* pAction);  // Deprecated
  // Deprecated
  //  void VisuBvFlexMenuCallback(QContextMenuEvent *event);
  void VisuMouseCallback(int pZEF, QMouseEvent *pEvent);

private:

  QAction* displayICBQAc = nullptr;

  bool UserOffsetDoNotChange = false;

  ZDataBuffer       SearchContent;
  ssize_t           SearchOffset=-1;
  ssize_t           FormerSearchOffset=-1;
  ssize_t           FormerSearchSize=-1;

  ZCVM_VisuType     VisuType=ZCVM_Nothing;
 // bool              FromOffset=false;

  QVariant          DefaultBackGround;
  QVariant          SelectedBackGround;

  ZQTableView*      VisuTBv=nullptr;

  void              DicEditQuitCallback();
  bool              FResizeInitial=true;

}; // ZRawVisuMain



//utf8VaryingString formatSize(long long wSize);


void setLoadMax (ssize_t pLoadMax);

//ssize_t computeOffsetFromCoord(int pRow, int pCol);

void sleepTimes (int pTimes);

#endif // ZRAWVISU_H
