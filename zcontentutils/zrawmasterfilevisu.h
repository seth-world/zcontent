#ifndef ZRAWMASTERFILEVISU_H
#define ZRAWMASTERFILEVISU_H

#include <QDialog>

#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>

#include <zcontent/zrandomfile/zblock.h>

class ZQTableView;

namespace Ui {
class ZRawMasterFileVisu;
}

class ZRawMasterFileVisu : public QDialog
{
  Q_OBJECT

public:
  explicit ZRawMasterFileVisu(QWidget *parent = nullptr);
  ~ZRawMasterFileVisu();

  ZStatus setup(const uriString& pURI, int pFd);
  ZStatus displayBlock(ZBlockDescriptor_Export &pBlock);

  ZStatus displayURFBlock(ZDataBuffer& pData);


  ZStatus displayRawBlock(ZDataBuffer& pData);
  void    colorizeURFBlock(ZDataBuffer & pData);
  void    colorizeURFKeyBlock(ZDataBuffer & pData);
  void    colorizeOneURFFields(const ZDataBuffer& pData,const unsigned char* &pPtr, const unsigned char* pPtrEnd, size_t &pColorOffset);

  void    displayOneLine(int pRow,unsigned char* &wPtr,unsigned char* wPtrEnd) ;

  bool    displayOneURFField(zaddress_type &wOffset, const unsigned char* &wPtr, const unsigned char* wPtrEnd);

  ZStatus displayURFKeyBlock(ZDataBuffer & pData);

  ZStatus seekAndGet(ZDataBuffer& pOut, ssize_t &pSize, size_t pAddress);

  ZStatus searchStartSign(ZDataBuffer& wRecord, zaddress_type &pAddress);

  ZStatus searchNextValidZType(const ZDataBuffer &wRecord,
                                zaddress_type & pAddress,
                                const unsigned char *&pPtr);

  ZStatus seekAndRead(ZDataBuffer& pRecord,ZBlockDescriptor_Export & pBlock);

  ZStatus getNextBlock(ZDataBuffer & wRecord,ssize_t wSize,zaddress_type & wAddress);

  void resizeEvent(QResizeEvent*) override;

  void setSelectionBackGround(QVariant& pBackground,
                              QVariant &pBackgroundFirst,
                              ssize_t pOffset,
                              size_t pSize,
                              const utf8VaryingString &pToolTip="",
                              bool pScrollTo=false);

  /* evaluate actions */

  QAction* uint16QAc = nullptr;
  QAction* int16QAc = nullptr;
  QAction* uint32QAc = nullptr;
  QAction* int32QAc = nullptr;
  QAction* uint64QAc = nullptr;
  QAction* int64QAc = nullptr;
  QAction* sizetQAc = nullptr;

  void goToAddress(zaddress_type pAddress);

  void firstIterate() { Forward();}

private slots:
  void Forward();
  void Backward();
  void ToEnd();
  void ToBegin();
  void ViewModeChange(int pIndex);


  void visuActionEvent(QAction* pAction);
  void VisuBvFlexMenuCallback(QContextMenuEvent *event);
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

  ZArray<ZBlockDescriptor_Export> BlockList;

  size_t        CurrentAddress=0;
  size_t        FileSize=0;
  off_t         FileOffset=0;
  ZQTableView*  BlockTBv=nullptr;
  ZQTableView*  BlockDumpTBv=nullptr;
  int           Fd=-1;
  long          BlockRank=-1;
  uriString     URICurrent;
  bool          AllFileLoaded=false;
  bool          FResizeInitial=true;
  Ui::ZRawMasterFileVisu *ui;
};


bool ZTypeExists(ZTypeBase pType);

ssize_t computeOffsetFromCoord(int pRow, int pCol);

#endif // ZRAWMASTERFILEVISU_H
