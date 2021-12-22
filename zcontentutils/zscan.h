#ifndef ZSCAN_H
#define ZSCAN_H

#include <QMainWindow>
#include <ztoolset/uristring.h>
#include <zcontent/zrandomfile/zrandomfiletypes.h>
#include <zcontent/zrandomfile/zblock.h>


namespace Ui {
class ZScan;
}

class ZScan : public QMainWindow
{
  Q_OBJECT

public:
  explicit ZScan(QWidget *parent = nullptr);
  ~ZScan();

  void setFile(const uriString& pFile);
  void close();

  ZStatus searchFirstStartMark(int pFd, ZBlockHeader &pBlockHeader, zaddress_type &pNextAddress);
  ZStatus searchNextStartMark(int pFd,
      const zaddress_type pAddress,
      ZBlockHeader &pBlockHeader,
      zaddress_type &pNextAddress);

  void scanMain();

  void displayOneBlock(zaddress_type pAddress,ZBlockHeader& pBlockHeader);

  void removeAllRows();
private:
  int FileDescriptor=-1;
  uriString Currentfile;

  size_t FileSize=0;

  int     UsedCount=0;
  size_t  UsedSize=0;
  int     FreeCount=0;
  size_t  FreeSize=0;
  int     HolesCount=0;
  size_t  HolesSize=0;
  int     DeletedCount=0;
  size_t  DeletedSize=0;

  Ui::ZScan *ui;
};

utf8String formatSize(long long wSize);

#endif // ZSCAN_H
