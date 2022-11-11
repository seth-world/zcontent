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
  void    displayOneLine(int pRow,unsigned char* &wPtr,unsigned char* wPtrEnd) ;

  ZStatus seekAndGet(ZDataBuffer& pOut, ssize_t &pSize, size_t pAddress);

  ZStatus searchStartSign(ZDataBuffer& wRecord, zaddress_type &pAddress);

  ZStatus searchNextValidZType(ZDataBuffer& wRecord,
                                zaddress_type & pAddress,
                                const unsigned char *&pPtr);

  ZStatus seekAndRead(ZDataBuffer& pRecord,ZBlockDescriptor_Export & pBlock);

  ZStatus getNextBlock(ZDataBuffer & wRecord,ssize_t wSize,zaddress_type & wAddress);

  void resizeEvent(QResizeEvent*) override;

private slots:
  void Forward();
  void Backward();
  void ToEnd();
  void ToBegin();
  void ViewModeChange(int pIndex);

private:
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

#endif // ZRAWMASTERFILEVISU_H
