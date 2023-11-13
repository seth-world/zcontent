#ifndef FILEGENERATEDLG_H
#define FILEGENERATEDLG_H

#include <QMainWindow>
#include <QAction>
#include <QActionGroup>



#include "masterfilevalues.h"
#include <ztoolset/zaierrors.h>

extern const QColor ErroredQCl;

#define __SAME_AS_MASTER__ "<same as master file>"


namespace zbs {
class ZDictionaryFile;
class ZMasterFile;
class ZMFDictionary;
template <class _Tp>
class ZArray;
}



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

class ZComboDelegate;
class ZMFProgressMWn;

enum FGState : uint8_t {
  FGST_Nothing  = 0,
  FGST_FDic     = 1,
  FGST_FRawMF   = 2,
  FGST_FZMF     = 3
};
/*
namespace Ui {
class FileGenerateDLg;
}
*/


class FileGenerateMWn : public QMainWindow , MasterFileValues
{
  Q_OBJECT

public:
  explicit FileGenerateMWn(QWidget *parent = nullptr);
  explicit FileGenerateMWn(ZDictionaryFile *pDictionary, QWidget *parent = nullptr);
  ~FileGenerateMWn()override ;

  void initLayout() ;
  void dataSetupFromDictionary() ;
  bool dataSetupFromMasterFile(const uriString& pURIMaster) ;
  bool dataSetupFromXmlDefinition(const uriString& pXmlFile) ;

  void _dataSetup();

  QList<QStandardItem*> formatKeyRow(KeyData& pKey);

  void _refresh();

  bool setupFromZmfFile(); /* set up definition from an existing master file  */
  bool setupFromXmlDic();  /* generate a definition and set up from an xml dictionary file */
  bool setupFromDicFile();  /* generate a definition and set up from a dictionary file */
  bool setupFromXmlDef();  /* load definition set up from a definition contained in an xml file */


  /* loads an external dictionary from an xml file without setting up parameters */
  ZStatus loadExternalXmlDic();
  /* loads an external dictionary from a dic file without setting up parameters */
  ZStatus loadExternalDicFile();

  ZStatus XmlDefinitionSave(uriString& pXmlFile,bool pComment=true); /* save definition to an xml formatted text file */
  ZStatus XmlDefinitionLoad(const utf8VaryingString&pXmlContent, ZaiErrors* pErrorlog);

//  bool changeChosenZmf(); /* apply to a zmf file current definition  deprecated */
  bool applyToCurrentZmf(); /* apply to currently loaded ZMF */

  /* no test run possible */
//  ZStatus applyChangesZmf(const uriString &pURIMaster, bool pBackup=true);

  ZStatus applyChangesZmf(bool pBackup=true);

  ZStatus createDic(ZMFDictionary& pDic, const uriString &pURIMaster);

  ZStatus rebuildIndex(long pIndexRankToRebuild);

  void exportCallBack(int pValue);

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

  void changeKeySize(KeyData& pKeyData,size_t pNewKeySize);

  /* controls master file and its index keys values before creation or applying changes
   * returns true if error(s) has(have) been detected
   * returns false if no error has been detected
   */
  bool ValuesControl();

  ZStatus indexRebuildFromMenu();

  void sleepWithLoop(int pTimes);

  bool testGuessed();

  void DataExport();
  void DataImport();

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
  QPixmap CrossBluePXm,CrossRedPXm;

  uint8_t       State=FGST_Nothing;

  textEditMWn* ComLog=nullptr;
  textEditMWn* ChangeLogMWn=nullptr;

  bool DoNotChangeKeyValues=false;

  ZMFProgressMWn* ProgressMWn=nullptr;

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

  QAction* DataExportQAc=nullptr;
  QAction* DataImportQAc=nullptr;


  QAction* GenXmlQAc=nullptr;

  QAction* SetupFromZmfQAc=nullptr;
  QAction* SetupFromXmlDicQAc=nullptr;
  QAction* SetupFromDicFileQAc=nullptr;
  QAction* SetupFromXmlDefQAc=nullptr;

  QAction* LoadXmlDicQAc=nullptr;   /* load external dictionary from xml dictionary definition */
  QAction* LoadDicFileQAc=nullptr;  /* load external dictionary from dictionary file */

//  QAction* ApplyToZmfQAc=nullptr;
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
//  QAction* ApplyToLoadedQAc=nullptr;
  QAction* indexRebuildQAc=nullptr;
  QAction* TestRunQAc=nullptr;
  QAction* QuitQAc=nullptr;

  QAction* EditLoadedDicQAc=nullptr;
  QAction* EditEmbeddedDicQAc=nullptr;
  QAction* EmbedDicQAc=nullptr;


  QActionGroup* GenActionGroup=nullptr;

  zbs::ZDictionaryFile* DictionaryFile=nullptr;
  zbs::ZMasterFile* MasterFile=nullptr;

  bool FResizeInitial=true;

  ZComboDelegate* ComboDelegate=nullptr;
};



extern class DicEditMWn* DicEdit;
extern class FileGenerateMWn* FileGenerate;


#endif // FILEGENERATEDLG_H
