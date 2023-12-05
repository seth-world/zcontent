#ifndef ZRAWMASTERFILEVISU_H
#define ZRAWMASTERFILEVISU_H

#include <QDialog>

#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>

#include <zcontent/zrandomfile/zblock.h>


const long cst_FileNudge = 100;
const size_t cst_BlockMax = 20000;

class ZQTableView;
class QLabel;
class   VisuRaw;

namespace Ui {
class ZRawMasterFileVisu;
}

class ZRawMasterFileVisu : public QDialog
{
  Q_OBJECT

public:
  explicit ZRawMasterFileVisu(QWidget *parent = nullptr);
  ~ZRawMasterFileVisu();

  ZStatus setup(const uriString &pURI);
  ZStatus _setup();
  ZStatus displayBlock(ZBlockDescriptor_Export pBlock);

  ZStatus displayURFBlock(ZDataBuffer& pData, bool pTruncated);

  ZStatus displayRawBlock(ZDataBuffer& pData, bool pTruncated);
  void    colorizeURFBlock(ZDataBuffer & pData);
  void    colorizeURFKeyBlock(ZDataBuffer & pData);
  void    colorizeOneURFFields(const ZDataBuffer& pData,const unsigned char* &pPtr, const unsigned char* pPtrEnd, size_t &pColorOffset);

  void    displayOneLine(int pRow,unsigned char* &wPtr,unsigned char* wPtrEnd) ;

  bool    displayOneURFField(zaddress_type &wOffset, const unsigned char* &wPtr, const unsigned char* wPtrEnd);

  ZStatus displayURFKeyBlock(ZDataBuffer & pData);

  /** @brief getNextBlock search file surface for next block following pStartAddress (included)
   *   When found returns a ZBlockDescriptor with accurate data (including address) and a ZStatus of ZS_SUCCESS
   *    BlockCur and FileOffset are updated accordingly by this operation
   * @return
   *    ZS_SUCCESS  Block is found
   *    ZS_OUTBOUND if pStartAddress is invalid
   *    ZS_OUTBOUNDHIGH no next valid block is found on file surface since pStartAddress up to end of file
   * @param pBlock
   * @param pStartAddress
   */
  ZStatus searchNextBlock(ZBlockDescriptor_Export &pBlock, zaddress_type pStartAddress);
  ZStatus searchPreviousBlock(ZBlockDescriptor_Export &pBlock, zaddress_type pStartAddress);

  ZStatus seekAndGet(ZDataBuffer& pOut, ssize_t &pSize, size_t pAddress);

  /** @brief searchNextStartSign Searches file surface for start sign beginning at pStartAddress up to end of file
   *                                and return its file address in pOutAddress with a status set to ZS_SUCCESS.
   *  if pStartAddress points to a valid start sign then pStartAddress is returned
   *  if no start sign is found before end of file ZS_OUTBOUNDHIGH is returned
   *
   * @return
   * ZS_SUCCESS : start block has been found. pOutAddress contains start sign address on file
   * ZS_OUTBOUND : pStartAddress is out of file's boundaries
   * ZS_OUTBOUNDHIGH : no start sign has been found while end of file surface has been reached
   *  Any other status that may be emitted by low level routines.
   */
  ZStatus searchNextStartSign(zaddress_type pStartAddress, zaddress_type &pOutAddress);

  static ZStatus searchNextStartSign_S(__FILEHANDLE__ pFd,size_t pFileSize,long pNudge,zaddress_type pStartAddress, zaddress_type &pOutAddress);


  /** @brief searchPreviousStartSign Searches file surface for start sign beginning at pStartAddress down to beginning of file
   *                                and return its file address in pOutAddress with a status set to ZS_SUCCESS.
   *  if pStartAddress points to a valid start sign then pStartAddress is returned
   *  if no start sign is found before beginning of file ZS_OUTBOUNDLOW is returned
   *
   * @return
   * ZS_SUCCESS : start block has been found. pOutAddress contains start sign address on file
   * ZS_OUTBOUND : pStartAddress is out of file's boundaries
   * ZS_OUTBOUNDLOW : no start sign has been found while beginning of file surface has been reached
   *  Any other status that may be emitted by low level routines.
   */
  ZStatus searchPreviousStartSign(zaddress_type pStartAddress, zaddress_type &pOutAddress) ;

  ZStatus searchNextValidZType(const ZDataBuffer &wRecord,
                                zaddress_type & pAddress,
                                const unsigned char *&pPtr);

  ZStatus seekAndRead(ZDataBuffer& pRecord,ZBlockDescriptor_Export & pBlock);

  ZStatus searchNextBlock(ZDataBuffer & wRecord,ssize_t wSize,zaddress_type & wAddress);

  void resizeEvent(QResizeEvent*) override;

  void setSelectionBackGround(QVariant& pBackground,
                              QVariant &pBackgroundFirst,
                              ssize_t pOffset,
                              size_t pSize,
                              const utf8VaryingString &pToolTip="",
                              bool pScrollTo=false);

  void setVisuIndexFile() ;

  void setNudge(long pNudge) { if (pNudge > 0) FileNudge=pNudge; }


  QPushButton* ExitBTn=nullptr;
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


  long FileNudge=cst_FileNudge;

  void goToAddress(zaddress_type pAddress, zrank_type pRank=0);
  void _goToAddress(zaddress_type pAddress, zrank_type pRank=0);

  void setViewModeRaw();
  void setViewModeURF();

  /* search next block from pAddress (included)  - fills up BlockCur with accurate date*/
  ZStatus searchNextBlock(zaddress_type pAddress, ZBlockDescriptor_Export& pBlock);
  /* search previous block from pAddress  - fills up BlockCur with accurate date*/
  ZStatus searchPreviousBlock(zaddress_type pAddress,ZBlockDescriptor_Export& pBlock);
  /* search the block whose address equals pAddress, fills BlockList array whenever required and returns it */
  ZBlockDescriptor_Export searchBlockByAddress(zaddress_type pAddress);

  void firstIterate() { Forward();}

  void getPrevAddrVal(zaddress_type &pAddress, long &pNudge, long &pBucket);
/*
  void visuBlockHeader();
  void visuURFField();
*/
private slots:
  void Forward();
  void Backward();
  void ToEnd();
  void ToBegin();
  void ViewModeChange(int pIndex);

  void RecStructChange(int pIndex);
// Deprecated  replaced with class VisuRaw
//  void visuActionEvent(QAction* pAction);

//  void visuActionEventOld(QAction* pAction);   // Deprecated
// Deprecated  replaced with class VisuRaw
//  void VisuBvFlexMenuCallback(QContextMenuEvent *event);

//  void VisuMouseCallback(int pZEF, QMouseEvent *pEvent);

private:
  ZDataBuffer RawRecord;

  bool              BackGroundOnce=true;
  QVariant          DefaultBackGround;
  QVariant          WrongBackGround = QVariant(QBrush(Qt::magenta));
  QVariant          BlockHeaderBackGround =  QVariant(QBrush(Qt::green));
  QVariant          BlockHeaderBackGround_first =  QVariant(QBrush(Qt::darkGreen));
  QVariant          PresenceBackGround =  QVariant(QBrush(Qt::yellow));
  QVariant          PresenceBackGround_first =  QVariant(QBrush(Qt::darkYellow));
  QVariant          URFSizeBackGround =  QVariant(QBrush(Qt::lightGray));
  QVariant          URFSizeBackGround_first =  QVariant(QBrush(Qt::darkGray));
  QVariant          URFHeaderBackGround =  QVariant(QBrush(Qt::cyan));
  QVariant          URFHeaderBackGround_first =  QVariant(QBrush(Qt::green));
  int Width = 16;
  bool  Raw=false;
  ZBlockDescriptor_Export BlockCur;


//  ZArray<ZBlockDescriptor_Export> BlockList;

  size_t        CurrentAddress=0;
  size_t        FileSize=0;
  off_t         FileOffset=0;
  ZQTableView*  BlockTBv=nullptr;
  ZQTableView*  BlockDumpTBv=nullptr;

  VisuRaw*      VizuRaw=nullptr;

  __FILEHANDLE__  Fd=-1;
//  long          BlockRank=-1;
  uriString     URICurrent;
  bool          AllFileLoaded=false;
  bool          FResizeInitial=true;
  Ui::ZRawMasterFileVisu *ui;
};


bool ZTypeExists(ZTypeBase pType);

ssize_t computeOffsetFromCoord(int pRow, int pCol);




#endif // ZRAWMASTERFILEVISU_H
