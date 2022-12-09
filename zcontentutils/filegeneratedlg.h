#ifndef FILEGENERATEDLG_H
#define FILEGENERATEDLG_H

//#include <QDialog>
#include <QMainWindow>
#include <QAction>
#include <QActionGroup>

#include <ztoolset/uristring.h>


class QPlainTextEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;
class QFormLayout;
class QLabel;
class QLineEdit;
class QCheckBox;
class ZQTableView;
class QStandardItem;

class KeyData {
public:
  KeyData()=default;
  KeyData(const KeyData& pIn) {_copyFrom(pIn);}
  KeyData& _copyFrom(const KeyData& pIn) {
    KeySize=pIn.KeySize;
    Allocated=pIn.Allocated;
    AllocatedSize=pIn.AllocatedSize;
    ExtentQuota=pIn.ExtentQuota;
    ExtentSize=pIn.ExtentSize;
    IndexRootName=pIn.IndexRootName;
    return *this;
  }
  KeyData& operator = (const KeyData& pIn) { return _copyFrom(pIn);}
  size_t  KeySize=0;
  size_t  Allocated=0;
  size_t  AllocatedSize=0;
  size_t  ExtentQuota=0;
  size_t  ExtentSize=0;
  bool    Duplicates=false;
  utf8VaryingString IndexRootName;
};
namespace zbs {
class ZDictionaryFile;
template <class _Tp>
class ZArray;
}


namespace Ui {
class FileGenerateDLg;
}

class FileGenerateDLg : public QMainWindow
{
  Q_OBJECT

public:
  explicit FileGenerateDLg(QWidget *parent = nullptr);
  explicit FileGenerateDLg(const zbs::ZDictionaryFile* pDictionary, QWidget *parent = nullptr);
  ~FileGenerateDLg();

  void initLayout() ;
  void dataSetupFromDictionary() ;
  void dataSetupFromMasterFile(const uriString& pURIMaster) ;

  void LoadFromFile();

  void resizeEvent(QResizeEvent*) override;

Q_SLOT
  void Quit();
  void Compute();
  void Guess();
  void GuessItemChanged(QStandardItem* pItem);
  void KeyItemChanged(QStandardItem* pItem);
  void SearchDir();
  void BaseNameEdit();
  void AllocatedEdit();
  void ExtentQuotaEdit();
  void MeanSizeEdit();

  void GenXml();
  void GenFile();

  void MenuAction(QAction* pAction);

private:
  bool DoNotChangeKeyValues=false;
  void IndexSearchDir();

  uriString TargetDirectory;
  uriString IndexDirectory;
  utf8VaryingString RootName;
  size_t MeanRecordSize=0;
  size_t AllocatedBlocks = 0, AllocatedSize = 0;
  size_t ExtentQuota = 0, ExtentQuotaSize = 0;
  size_t InitialBlocks = 0, InitialSize = 0;

  zbs::ZArray<KeyData>* KeyValues=nullptr;

  QPlainTextEdit *plainTextEdit=nullptr;

  QPushButton *GenerateBTn=nullptr;
  QPushButton *DiscardBTn=nullptr;
  QPushButton *DirectoryBTn=nullptr;
  QWidget *verticalLayoutWidget=nullptr;

  QLineEdit * RootNameLEd=nullptr;
  QLabel*     DirectoryLBl=nullptr;
  QLabel*     IndexDirectoryLBl=nullptr;
  QPushButton* SearchDirBTn=nullptr;

  QLineEdit *MeanSizeLEd=nullptr;

  QPushButton *ComputeBTn=nullptr;

  QLineEdit *AllocatedLEd=nullptr;
  QLineEdit *AllocatedSizeLEd=nullptr;

  QLineEdit *ExtentQuotaLEd=nullptr;
  QLineEdit *ExtentQuotaSizeLEd=nullptr;

  QLineEdit *InitialSizeLEd=nullptr;

  QCheckBox *HighWaterMarkingCHk=nullptr;
  QCheckBox *GrabFreeSpaceCHk=nullptr;
  QCheckBox *JournalingCHk=nullptr;


  ZQTableView *KeyTBv=nullptr;
  ZQTableView *GuessTBv=nullptr;

  QAction* SearchDirQAc=nullptr;
  QAction* IndexSearchDirQAc=nullptr;
  QAction* GenXmlQAc=nullptr;
  QAction* LoadFromFileQAc=nullptr;
  QAction* GenFileQAc=nullptr;
  QAction* QuitQAc=nullptr;

  QActionGroup* GenActionGroup=nullptr;

  const zbs::ZDictionaryFile* DictionaryFile=nullptr;

  bool FResizeInitial=true;
//  Ui::FileGenerateDLg *ui;
};

template <class _Tp>
_Tp getValueFromString(const utf8VaryingString& pStr) {
  _Tp wValue=0;
  utf8_t* wPtr=pStr.Data;
  utf8_t* wPtrEnd=pStr.Data + pStr.strlen();

  while (!std::isdigit(*wPtr) && (wPtr < wPtrEnd)) {
    //    if (*wPtr=='-')
    //      wSign = -1;
    wPtr++;
  }
  while (std::isdigit(*wPtr) && (wPtr < wPtrEnd)) {
    wValue = wValue * 10;
    wValue += long(*wPtr - '0');
    wPtr++;
  }
  return wValue;
}


#endif // FILEGENERATEDLG_H
