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
namespace zbs {
class ZRandomFile;
class ZMasterFile;
}

//class ZContentVisuMain;
class uriString;
class DisplayMain : public QMainWindow
{
  Q_OBJECT

public:
 // explicit DisplayMain(ZContentVisuMain *parent = nullptr);
  explicit DisplayMain(const uriString* pURIContent,QWidget *parent = nullptr);
  ~DisplayMain();

  void initLayout();

  QStandardItemModel* displayItemModel =nullptr;


  void setFileClosed(bool pYesNo);

  void setOffset(size_t pOffset,size_t pMax);

  void displayHCB(ZDataBuffer& pData);
  void displayFCB(ZDataBuffer &pData);
  void displayMCB(ZDataBuffer &pData);
  void displayICBs(ZDataBuffer &pData);
  void displayPool(ZDataBuffer &pData);
  void displayHCBValues(unsigned char *pPtrIn);
  void displayFCBValues(unsigned char* pPtrIn);
  void displayMCBValues(const unsigned char *pPtrIn);
  void displayICBValues(const unsigned char *pPtrIn);
  bool displaySingleICBValues(const unsigned char *&pPtrIn, size_t &pOffset, int &wRow);
  void displayPoolValues(unsigned char* pPtrIn);

  void displayAll(unsigned char *pPtrIn);
  void load(unsigned char* pPtrIn);

  void getMCB(ZDataBuffer &pData);

  void createMarginItem(size_t pValue, int pRow);

  void resizeEvent(QResizeEvent*) override;
  bool FResizeInitial=true;

  ZDataBuffer           ContentToDump;
  size_t                Offset=0;
  unsigned char*        PtrIn=nullptr;
//  ZContentVisuMain*     VisuMain=nullptr;
  const uriString* URIContent=nullptr;
  ZDataBuffer RawData;

  enum DisplayWhat {
    ZDW_Nothing = 0,
    ZDW_HCB = 1,
    ZDW_FCB = 2,
    ZDW_MCB = 3,
    ZDW_ICB = 4,
    ZDW_POOL = 5
  };

  DisplayWhat CurrentDisplay=ZDW_Nothing;

  void clear();




private slots:
  void sliderChange(int pValue);



private:
  Ui::DisplayMain *ui;
};

#endif // DISPLAYMAIN_H
