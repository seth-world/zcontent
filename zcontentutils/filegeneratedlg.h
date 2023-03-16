#ifndef FILEGENERATEDLG_H
#define FILEGENERATEDLG_H

//#include <QDialog>
#include <QMainWindow>
#include <QAction>
#include <QActionGroup>

#include <ztoolset/uristring.h>
#include <ztoolset/zaierrors.h>

#include <zcontent/zindexedfile/zmasterfile.h>

#include "keydata.h"
#include "zchangerecord.h"

#define __SAME_AS_MASTER__ "<same as master file>"

class DicEditMWn;
class FileGenerateMWn;
extern class DicEditMWn* DicEdit;
extern class FileGenerateMWn* FileGenerate;

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
class textEditMWn;


namespace zbs {
class ZDictionaryFile;
template <class _Tp>
class ZArray;
}



class MasterFileValues {
public:
  MasterFileValues()=default;
  ~MasterFileValues() {
    if (KeyValues!=nullptr)
      delete KeyValues;
    if (DeletedKeyValues!=nullptr)
      delete DeletedKeyValues;
  }
  MasterFileValues(MasterFileValues& pIn) {_copyFrom(pIn);}
  MasterFileValues& _copyFrom(MasterFileValues& pIn) {
    TargetDirectory=pIn.TargetDirectory;
    IndexDirectory=pIn.IndexDirectory;
    RootName=pIn.RootName;
    MeanRecordSize=pIn.MeanRecordSize;
    AllocatedBlocks=pIn.AllocatedBlocks;
    AllocatedSize=pIn.AllocatedSize;
    ExtentQuota=pIn.ExtentQuota;
    ExtentQuotaSize=pIn.ExtentQuotaSize;
    InitialBlocks=pIn.InitialBlocks;
    InitialSize=pIn.InitialSize;

    GrabFreeSpace=pIn.GrabFreeSpace;
    HighWaterMarking=pIn.HighWaterMarking;
    Journaling=pIn.Journaling;

    if (pIn.KeyValues==nullptr) {
      if (KeyValues!=nullptr) {
        delete KeyValues;
        KeyValues=nullptr;
      }
      return *this;
    }// if (pIn.KeyValues==nullptr)
    if (KeyValues==nullptr)
      KeyValues = new zbs::ZArray<KeyData>;
    else
      KeyValues->clear();
    for (long wi=0;wi < pIn.KeyValues->count();wi++) {
      KeyValues->push(pIn.KeyValues->Tab[wi]);
    } // for
    return *this;
  } // _copyFrom

  MasterFileValues& operator=(MasterFileValues& pIn) {return _copyFrom(pIn);}

  uriString TargetDirectory;
  uriString IndexDirectory;
  utf8VaryingString RootName;
  size_t MeanRecordSize=0;
  size_t AllocatedBlocks = 0, AllocatedSize = 0;
  size_t ExtentQuota = 0, ExtentQuotaSize = 0;
  size_t InitialBlocks = 0, InitialSize = 0;
  bool HighWaterMarking = false ;
  bool GrabFreeSpace = true;
  bool Journaling = false;

  zbs::ZArray<KeyData>* KeyValues=nullptr;
  zbs::ZArray<KeyData>* DeletedKeyValues=nullptr;

//  zbs::ZArray<KeyChange> KeyLog;
  zbs::ZArray<ZChangeRecord> ChangeLog;
};

enum FGState : uint8_t {
  FGST_Nothing  = 0,
  FGST_FDic     = 1,
  FGST_FRawMF   = 2,
  FGST_FZMF     = 3
};

namespace Ui {
class FileGenerateDLg;
}

class FileGenerateMWn : public QMainWindow , MasterFileValues
{
  Q_OBJECT

public:
  explicit FileGenerateMWn(QWidget *parent = nullptr);
  explicit FileGenerateMWn(ZDictionaryFile *pDictionary, QWidget *parent = nullptr);
  ~FileGenerateMWn();

  void initLayout() ;
  void dataSetupFromDictionary() ;
  bool dataSetupFromMasterFile(const uriString& pURIMaster) ;
  bool dataSetupFromXmlDefinition(const uriString& pXmlFile) ;

  void _dataSetup();

  QList<QStandardItem*> formatKeyRow(KeyData& pKey);

  void _refresh();

  bool LoadFromZmfFile(); /* load definition from an existing master file  */
  bool LoadFromXmlDic();  /* generate a definition from an xml dictionary file */
  bool LoadFromXmlDef();  /* load definition from an xml file */


  ZStatus XmlSave(uriString& pXmlFile,bool pComment=true); /* save definition to an xml formatted text file */
  ZStatus XmlLoad(const utf8VaryingString&pXmlContent, ZaiErrors* pErrorlog);

  bool changeChosenZmf(); /* apply to a zmf file current definition */
  bool applyToCurrentZmf(); /* apply to currently loaded ZMF */

  /* no test run possible */
  ZStatus processZmf(const uriString &pURIMaster, bool pBackup=true);

  ZStatus createDic(ZMFDictionary& pDic, const uriString &pURIMaster);

  void resizeEvent(QResizeEvent*) override;

  void KeyDelete();
  void KeyAppendRaw();
  ZStatus KeyAppendFromLoadedDic();
  ZStatus KeyAppendFromEmbeddedDic();

  ZStatus KeyAppendFromZMFDic(const ZMFDictionary *pDic);

  void displayErrorCallBack(const utf8VaryingString& pMessage) ;
  void DicEditQuitCallback();
  void closeComlogCB(const QEvent* pEvent);

  void displayChangeLog();
  void displayChangeLine(const ZChangeRecord& pChgRec);

Q_SLOT
  void Quit();
  void Compute();
  void GuessItemChanged(QStandardItem* pItem);
  void KeyItemChanged(QStandardItem* pItem);
  void SearchDir();
  void SearchIndexDir();
  void SameAsMaster();
  void HideGuess();
  void BaseNameEdit();
  void AllocatedEdit();
  void ExtentQuotaEdit();
  void MeanSizeEdit();

  void GenFile(); /* generates a new master file using current definition */

  void MenuAction(QAction* pAction);

private:
  uint8_t       State=FGST_Nothing;

  textEditMWn* ComLog=nullptr;
  textEditMWn* ChangeLogMWn=nullptr;

  bool DoNotChangeKeyValues=false;


  ZaiErrors  ErrorLog;

  QPushButton *DirectoryBTn=nullptr;
  QWidget *verticalLayoutWidget=nullptr;

  QLabel*     SourceLBl = nullptr;
  QLabel*     DicEmbedLBl = nullptr;
  QLabel*     DicLoadLBl = nullptr;
  QLabel*     SourceContentLBl = nullptr ;

  QGroupBox*  GuessGBx=nullptr;

  QLineEdit * RootNameLEd=nullptr;
  QLabel*     DirectoryLBl=nullptr;
  QLabel*     IndexDirectoryLBl=nullptr;
  QPushButton* SearchDirBTn=nullptr;
  QPushButton* SearchIndexDirBTn=nullptr;
  QPushButton* SameAsMasterBTn=nullptr;

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

  QPushButton*   HideGuessBTn=nullptr;

  QAction* SearchDirQAc=nullptr;
  QAction* IndexSearchDirQAc=nullptr;
  QAction* SameAsMasterQAc=nullptr;
  QAction* GenXmlQAc=nullptr;

  QAction* LoadFromZmfQAc=nullptr;
  QAction* LoadFromXmlDicQAc=nullptr;
  QAction* LoadFromXmlDefQAc=nullptr;

  QAction* ApplyToZmfQAc=nullptr;
  QAction* SaveToXmlQAc=nullptr;

  QAction* KeyAppendRawQAc=nullptr;
  QAction* KeyAppendFromEmbeddedDicQAc=nullptr;
  QAction* KeyAppendFromLoadedDicQAc=nullptr;

  QAction* KeyDeleteQAc=nullptr;

  QAction* ShowGuessValQAc=nullptr;
  QAction* HideGuessValQAc=nullptr;

  QAction* ShowLogQAc=nullptr;
  QAction* HideLogQAc=nullptr;

  QAction* ShowGenLogQAc=nullptr;
  QAction* HideGenLogQAc=nullptr;


  QAction* GenFileQAc=nullptr;
  QAction* ApplyToCurrentQAc=nullptr;
  QAction* ApplyToLoadedQAc=nullptr;
  QAction* TestRunQAc=nullptr;
  QAction* QuitQAc=nullptr;

  QAction* EditLoadedDicQAc=nullptr;
  QAction* EditEmbeddedDicQAc=nullptr;

  QActionGroup* GenActionGroup=nullptr;

  zbs::ZDictionaryFile* DictionaryFile=nullptr;
  zbs::ZMasterFile* MasterFile=nullptr;

  bool FResizeInitial=true;
};


#endif // FILEGENERATEDLG_H
