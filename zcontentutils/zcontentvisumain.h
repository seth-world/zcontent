#ifndef ZCONTENTVISUMAIN_H
#define ZCONTENTVISUMAIN_H

#include <QMainWindow>
#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zarray.h>


#include <zcontent/zrandomfile/zrandomfiletypes.h>
#include <zcontent/zrandomfile/zblock.h>

#include <zcontent/zrandomfile/zrandomfile.h>
#include <QModelIndex>

extern const int cst_maxraisonablevalue;

class ZQTableView;

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



namespace Ui {
class ZContentVisuMain;
}
class QStandardItemModel;
class QActionGroup;
class QAction;
class QFileDialog;
namespace zbs {
//class ZRandomFile;
class ZRawMasterFile;
}


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

class DisplayMain;
class DicEdit;
class textEditMWn;

class ZContentVisuMain : public QMainWindow
{
  Q_OBJECT

public:
  explicit ZContentVisuMain(QWidget *parent = nullptr);
  ~ZContentVisuMain();


  ZStatus openZRF();
  ZStatus openZMF(const char* pFileName);
  ZStatus openZRH(const char* pFileName);
  ZStatus openOther(const char *pFileName);

  void    openZRH();

  void    getRaw();

  ZStatus unlockZRFZMF(const char* pFilePath);

  ZStatus displayFdNextRawBlock(ssize_t pBlockSize, size_t pWidth=16);

  ZStatus displayListZRFNextRecord(size_t pWidth=16);
  ZStatus displayListZRFFirstRecord(size_t pWidth=16);
  ZStatus displayListZRFPreviousRecord(size_t pWidth=16);

  bool displayWidgetBlockOnce=false;
  ZStatus displayWidgetBlock(ZDataBuffer& pData);
  void displayOneLine(int pRow,unsigned char* &wPtr,unsigned char* wPtrEnd);

  void displayBlockData();

  void ZRFUnlock();
  void ZHeaderRawUnlock();

  static bool testRequestedSize(const uriString &pURI, ZDataBuffer &pRawData, size_t pRequestedSize);

  void displayHCB();
  void displayFCB();
  void displayMCB();
  void displayICBs();


  void displayPool(const unsigned char* pPtr,zaddress_type pOffset,const char* pTitle);

  void displayZBAT();
  void displayZFBT();
//  void displayZDBT();  // Deprecated

//  void Dictionary();

  void surfaceScanZRF(const uriString &pFileToScan);
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


  void getFileSize(const uriString &pFile);
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
  zbs::ZRawMasterFile* MasterFile=nullptr;

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

  QActionGroup* mainQAg=nullptr;
  QAction*      DictionaryQAc = nullptr;
  QAction*      GetRawQAc=nullptr;

  QAction*      openZRFQAc=nullptr;

  /* evaluate actions */

  QAction* uint16QAc = nullptr;
  QAction* int16QAc = nullptr;
  QAction* uint32QAc = nullptr;
  QAction* int32QAc = nullptr;
  QAction* uint64QAc = nullptr;
  QAction* int64QAc = nullptr;
  QAction* sizetQAc = nullptr;

  bool searchHexa(bool pReverse=false);
  bool searchAscii(bool pCaseRegardless=false, bool pReverse=false);

  void setSearchOffset(ssize_t pOffset);

  void setSelectionBackGround(QVariant &pBackground, ssize_t pOffset, size_t pSize, bool pScrollTo=false);

  void resizeEvent(QResizeEvent*) override;

private slots:
  void searchFwd();
  void searchBck();
  void searchCBxChanged(int pIndex);
  void actionMenuEvent(QAction* pAction);
  void actionOpenFileByType(bool pChecked=true);
  void openRaw();
  void actionClose(bool pChecked=true);

  bool chooseFile(bool pChecked);

  void VisuClicked(QModelIndex pIdx);


  void backward();
  void forward();
  void loadAll();

  void visuActionEvent(QAction* pAction);
  void VisuBvFlexMenuCallback(QContextMenuEvent *event);
  void VisuMouseCallback(int pZEF, QMouseEvent *pEvent);

private:

  QAction* displayICBQAc = nullptr;


  ZDataBuffer       SearchContent;
  ssize_t           SearchOffset=-1;
  ssize_t           FormerSearchOffset=-1;
  ssize_t           FormerSearchSize=-1;

  QVariant          DefaultBackGround;
  QVariant          SelectedBackGround;

  ZQTableView*      VisuTBv=nullptr;

  void              DicEditQuitCallback();
  bool              FResizeInitial=true;
  Ui::ZContentVisuMain *ui;
};

utf8String formatSize(long long wSize);

/**
 * @brief _searchBlockStart scans file pointed by pContentFd since pBeginAddress for a start mark (cst_ZBLOCKSTART 0xF5F5F5F5)
 * and returns
 *  . address of found mark in pNextAddress (pointing to first byte of start mark),
 *  . ZDataBuffer containing the file space between two start mark or until end of file.
 * Each read access loads pPayload bytes from file. pPayload is adjusted progressively to block sizes.
 * When found pNextAddress points to the first byte of start mark.
 * @param pContentFd    File descriptor to search. File must be opened with READ capabilities (O_RDONLY)
 * @param pBeginAddress address to begin the search.
 * WARNING: if pBeginAddress points to a cst_ZBLOCKSTART block, then this address will be returned as pNextAddress.
 * @param pNextAddress  address of first byte of found cst_START
 * @param pPayload      number of bytes to read at each read operation
 * @param pFileSize     total number of bytes for the file to be scanned
 * @return  a ZStatus
 * - ZS_FOUND     start mark has been found.
 *                pNextAddress points to first byte of start mark.
 *                pBlockContent contains block data since pBeginAddress until next start mark (excluded).
 * - ZS_EOF       no more to read and start mark has not been found since pBeginAddress.
 *                pNextAddress is set to the last address processed.
 *                pBlockContent contains data since pBeginAddress until EndofFile.
 * - ZS_READERROR a low level error has been encountered. ZException is set with appropriate message.
 *                pNextAddress is set to the last address processed.
 * - ZS_FILEERROR a seek operation failed with a low level error.
 *                ZException is set with appropriate message.
 *                pNextAddress is set to the last address processed.
 */
ZStatus
_searchBlockStart (int pContentFd,
                    zaddress_type pBeginAddress,      // Address to start searching for start mark
                    zaddress_type &pNextAddress,
                    ZDataBuffer &pBlockContent,
                    ssize_t &pPayload,
                    int     &pCount,
                    size_t  &pFileSize,
                    uint32_t *pBeginContent=nullptr);

void setLoadMax (ssize_t pLoadMax);

ssize_t computeOffsetFromCoord(int pRow, int pCol);



#endif // ZCONTENTVISUMAIN_H
