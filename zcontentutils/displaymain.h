#ifndef DISPLAYMAIN_H
#define DISPLAYMAIN_H

#include <QMainWindow>
#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zarray.h>

namespace Ui {
class DisplayMain;
}

class QStandardItemModel;
class QActionGroup;
class QAction;
class QFileDialog;
namespace zbs {
class ZRandomFile;
class ZSMasterFile;
}
class DisplayMain : public QMainWindow
{
  Q_OBJECT

public:
  explicit DisplayMain(QWidget *parent = nullptr);
  ~DisplayMain();


  QStandardItemModel* displayItemModel =nullptr;

  void displayHCB(ZDataBuffer& pData);
  void displayFCB(ZDataBuffer &pData);
  void displayMCB(ZDataBuffer &pData);
  void displayPool(ZDataBuffer &pData);
  void displayHCBValues(unsigned char *pPtrIn);
  void displayFCBValues(unsigned char* pPtrIn);
  void displayMCBValues(unsigned char *pPtrIn);
  void displayPoolValues(unsigned char* pPtrIn);

  void displayAll(unsigned char *pPtrIn);
  void load(unsigned char* pPtrIn);


  ZDataBuffer           ContentToDump;
  long                  Offset=0;
  unsigned char*        PtrIn=nullptr;

  enum DisplayWhat {
    ZDW_Nothing = 0,
    ZDW_HCB = 1,
    ZDW_FCB = 2,
    ZDW_MCB = 3,
    ZDW_POOL = 4
  };

  DisplayWhat CurrentDisplay=ZDW_Nothing;

  void clear();

private slots:
  void sliderChange(int pValue);

private:
  Ui::DisplayMain *ui;
};

#endif // DISPLAYMAIN_H
