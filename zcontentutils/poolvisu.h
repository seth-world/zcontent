#ifndef POOLVISU_H
#define POOLVISU_H

#include <QMainWindow>

#include <ztoolset/zstatus.h>
#include <ztoolset/zdatabuffer.h>
#include <zcontent/zrandomfile/zfiledescriptor.h>

#include <QVariant>
#include <QBrush>
#include <QContextMenuEvent>

extern const int cst_MessageDuration ;

class uriString;
class ZQTableView;
class ZRawMasterFileVisu;
class QActionGroup;
class QAction;
class textEditMWn;

namespace Ui {
class poolVisu;
}

enum ZPoolRepair : uint8_t {
  ZPOR_TestRun    = 0, /* Nothing done - only diagnostics */
  ZPOR_FixState   = 1, /* ZBS_Used for ZBAT, ZBS_Free for ZFBT, ZBS_Deleted for ZDBT */
  ZPOR_FixSize    = 2, /* suppress zero sized blocks in pool - adjust pool size to content block size if different */
  ZPOR_FixOrphan  = 4, /* suppress pool block that does not point to a valid block start sign in content file */
  ZPOR_Unknown
};


enum ZPoolType : uint8_t {
  ZPTP_ZBAT     = 0 ,
  ZPTP_ZFBT     = 1 ,
  ZPTP_ZDBT     = 2 ,
  ZPTP_Unknown
};

enum ZBlockExistence : uint16_t {
  ZBEX_Correct          =      0,
  ZBEX_SysBadAddress    =      1,  /* cannot position or read at given address on file */
  ZBEX_Orphan           =      2,  /* cannot find <cst_ZFILEBLOCKSTART> at given address */
  ZBEX_WrngAddr         =      4,
  ZBEX_Start            =      8,
  ZBEX_Size             =   0x10,  /* block size in pool is different from block size on content file block header */
  ZBEX_PoolZeroSize     =   0x20,  /* block size in pool has zero value */
  ZBEX_ContentZeroSize  =   0x40,  /* block size on content file has zero value */
  ZBEX_MustBeUsed       =   0x80,  /* invalid block state : must be ZBS_Used */
  ZBEX_MustBeFree       = 0x0100,  /* invalid block state : must be ZBS_Free */
  ZBEX_MustBeDeleted    = 0x0200,  /* invalid block state : must be ZBS_Deleted */
};

class poolVisu : public QMainWindow
{
  Q_OBJECT

public:
  explicit poolVisu(QWidget *parent = nullptr);
  ~poolVisu();

  ZStatus set(const uriString &pContentUri, const uriString &pHeaderUri);

  void dataSetup(int pPoolid);

  void repairDisplay(const utf8VaryingString& pOut);

  /* updates the block header on content file with state pState */
  static ZStatus updateBlockHeaderState(const utf8VaryingString &pURIContent,
                                        int pFdContent, zaddress_type &pAddress, ZBlockState_type pState);

  static ZStatus updateHeaderFromPool(const uriString &pURIHeader, ZBlockPool* pZBAT, ZBlockPool* pZFBT, ZBlockPool* pZDBT);

  static uint16_t checkContentBlock(int pPoolId, int pFdContent, ZBlockDescriptor &pBlockDesc);

  static ZStatus repair(const uriString &pURIContent, const uriString &pURIHeader, uint8_t pFlag, std::function<void (utf8VaryingString&)> pDisplay=nullptr);

  void statusBarMessage(const char* pFormat,...);

  uriString URIContent;
  uriString URIHeader;

  size_t        ContentSize=0;
  zaddress_type StartOfData=0L;

  ZDataBuffer HeaderContent;

  int       FdHeader=-1;
  int       FdContent=-1;

  int       PoolId=0;

  bool      PoolNotLoaded=true;
  bool      PoolChanged=false;

  ZQTableView*      HeaderTBv=nullptr;
  ZQTableView*      ContentTBv=nullptr;

  textEditMWn* repairLog=nullptr;

  bool              BackGroundOnce=true;
  QVariant          DefaultBackGround;
  QVariant          WrongBackGround = QVariant(QBrush(Qt::yellow));

  void poolMouseCallback(int pZEF, QMouseEvent *pEvent);
  ZStatus updateHeader();

private slots:
  void refresh();
  void udpdateHeaderWnd();
  void PoolChange(int pIdx);
  void menuFlex(QContextMenuEvent *event) ;
  void flexMenActionEvent(QAction* pAction) ;

  void generalActionEvent(QAction* pAction);

private:
  void resizeEvent(QResizeEvent*) override;
  /* flex menu */
  QAction*  blockVisuQAc=nullptr;
  QAction*  moveFreeQAc =nullptr;
  QAction*  moveDeleteQAc =nullptr;
  QAction*  changeToUsedQAc =nullptr;
  QAction*  removeNullQAc =nullptr;
  QAction*  changeToFreeQAc =nullptr;
  QAction*  undeleteQAc =nullptr;

  QActionGroup* flexAGp=nullptr;


  /* general menu */
  QAction* quitQAc=nullptr;

  QAction* fixStateQAc = nullptr;
  QAction* fixSizeQAc = nullptr;
  QAction* fixOrphanQAc = nullptr;
  QAction* repairQAc = nullptr;
  QAction* unSelectAllQAc = nullptr;
  QAction* selectAllQAc = nullptr;
  /* header list menu */

  QAction* displayHCBQAc = nullptr;
  QAction* displayFCBQAc = nullptr;
  QAction* displayMCBQAc = nullptr;
  QAction* displayICBQAc = nullptr;

   QAction* unlockHeaderQAc = nullptr;


  const ZBlockDescriptor_Export* BDe=nullptr;

  ZRawMasterFileVisu* ContentVisu=nullptr;

  ZBlockPool  ZBAT;
  ZBlockPool  ZFBT;
  ZBlockPool  ZDBT;

  bool          FResizeInitial=true;
  Ui::poolVisu *ui;
};


ZStatus
grabFreeForward(const utf8VaryingString& pURIContent,
                int pFdContent,
                size_t pFileSize,
                size_t pStartData,
                ZBlockPool &pZFBT,
                long &pFreeRank,
                std::function<void (utf8VaryingString&)> pDisplay=nullptr );


ZStatus ZExceptionDisplayAll(std::function<void (utf8VaryingString&)> pDisplay);

utf8VaryingString decode_ZBEx(uint16_t pBEx);

#endif // POOLVISU_H
