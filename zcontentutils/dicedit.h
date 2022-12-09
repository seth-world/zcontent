#ifndef DICEDIT_H
#define DICEDIT_H

#include <QMainWindow>
#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zarray.h>
#include <qtableview.h>
#include <QStandardItem>
#include <zcontent/zindexedfile/zfielddescription.h>

#include <zcu_common.h>
#include <zcontent/zindexedfile/zindexfield.h>

#include <zcontentutilsparams.h>

class textEditMWn;


class ZDataReference;
namespace  zbs {
class ZKeyDictionary;
class ZFieldDescription;
class KeyField_Pack;
class ZIndexField;
class ZDictionaryFile;

class ZCppGenerate;
}

namespace Ui {
class DicEdit;
}

class QStandardItemModel;
class QActionGroup;
class QAction;
class QFileDialog;

namespace zbs {
class ZRandomFile;
class ZMasterFile;
class ZMFDictionary;
}


class ZQTreeView;
class ZQTableView;
class QStandardItemModel;
class ZFieldDLg;

class RawFields;

namespace zbs {
class ZKeyHeaderRow;
class ZKeyFieldRow;
}


class DicEdit : public QMainWindow
{
  Q_OBJECT

  friend class RawFields;

public:
  explicit DicEdit(QWidget *parent = nullptr);
  explicit DicEdit(std::function<void()> pQuitCallback,QWidget *parent );
  ~DicEdit();

  void init();

  ZQTreeView*     keyTRv=nullptr;
  ZQTableView*    fieldTBv=nullptr;

  ZFieldDLg*      FieldDLg=nullptr;

  void Quit() ;

  void clear();
  void clearAllRows();

  void clearFieldRows();
  void clearKeyRows();


/**

fieldTBv

  fieldChange             changes the content of field pointed by pIdx using dialog ZFieldDLg. Changes dictionary accordingly.

  fieldInsertNewBefore    creates a new field using dialog ZFieldDLg and insert it before the field pointed by pIdx.

  fieldInsertNewAfter     creates a new field using dialog ZFieldDLg and insert it before the field pointed by pIdx.

  fieldAppend             creates a new field using dialog ZFieldDLg and appends it to table view and dictionary.

  _fieldDelete            deletes the field pointed by pIdx from table view and dictionary table.
                          A control is made wether field is part of keys. If yes a message is issued.
                          Deletion could be forced if flag pForceDelete is set to true

  fieldCopyToPinboard     copies the field data pointed by pIdx to pinboard. Field is not deleted from table view.

  fieldCutToPinboard      copies the field data pointed by pIdx to pinboard then delete it from table view and dictionary

  fieldAppendFromPinboard gets a field definition from last pinboard element and appends it to field table view and dictionary.

  _fieldInsertBefore      Low level routine : inserts a field before given index

  _fieldInsertAfter       Low level routine : inserts a field after given index

*/

  bool fieldCreateDLg(ZFieldDescription &pNewField);
  QStandardItem * fieldChangeDLg(QStandardItem *pFieldItem);

  bool keyCreateDLg(ZKeyHeaderRow &pNewKey);
  bool keyChangeDLg(ZKeyHeaderRow &pKey);

  /** @brief fieldInsert  creates a new field using dialog ZFieldDLg and insert it before the field pointed by pIdx .
   *  Infradata and dictionary data are changed accordingly. */
  bool fieldInsertNewBefore(QModelIndex pIdx);
//  bool fieldInsertNewAfter(QModelIndex pIdx);

  /** @brief _fieldInsert  insert field wFDesc at index pIdx to table view and to related dictionary array
   *                      if table view is empty or if pIdx is invalid, then appends. */

  /** @brief fieldAppend  creates a new field using dialog ZFieldDLg and appends it to table view and dictionary.
   *  Infradata and dictionary data are changed accordingly. */
  bool fieldAppend();

  /** @brief _fieldAppend append wFDesc to table view and to related dictionary array */
  bool _fieldAppend(ZFieldDescription &wFDesc);



  /** @brief fieldInsertAfterFromPinboard inserts the field description contained in Pinboard before field pointed by pIdx
   */
  bool fieldInsertAfterFromPinboard(QModelIndex pIdx);


  /** @brief fieldInsertAfterFromPinboard inserts the field description contained in Pinboard BEFORE field pointed by pIdx
   */
  bool _fieldInsertNewBefore(QModelIndex pIdx, ZFieldDescription &pFDesc);

  /** @brief fieldInsertAfterFromPinboard inserts the field description contained in Pinboard AFTER field pointed by pIdx
   */
  bool _fieldInsertAfter(QModelIndex pIdx, ZFieldDescription &pFDesc);

  /** @brief fieldAppendFromPinboard gets a field definition from last pinboard element and appends it to field table view and dictionary.
   */
  bool fieldAppendFromPinboard();

//  bool fieldInsertBefore(QModelIndex &wIdx,ZFieldDescription& pField);
  /** @brief fieldChange  changes the content of field pointed by pIdx using dialog ZFieldDLg.
   *  Infradata and dictionary data are changed accordingly.
   *  returns changed field item if success and nullptr if change is not made.
   */
  QStandardItem *fieldChange(QModelIndex pIdx);

  /** @brief searchForFieldName search within all fieldTBv field rows infradata for pName to exist.
   * Returns row number if exists or -1 if not exists.*/
  int searchForFieldName (const utf8VaryingString& pName);

  /** @brief searchForFieldNameCase search within all fieldTBv field rows infradata for pName to exist Case regardless.
   * Returns row number if exists or -1 if not exists.*/
  int searchForFieldNameCase (const utf8VaryingString& pName);

  /** @brief searchForKeyName search within all keyTRv key rows infradata for pName to exist.
   * Returns row number if exists or -1 if not exists.*/
  int searchForKeyName (const utf8VaryingString& pName);

  /** @brief searchForKeyNameCase search within all keyTRv key rows infradata for pName to exist Case regardless.
   * Returns row number if exists or -1 if not exists.*/
  int searchForKeyNameCase (const utf8VaryingString& pName);
  /**
   * @brief _fieldDelete deletes the current field from table view.
        A control is made wether field is part of keys.
        If yes a dialog asks wether modify also concerned keys or quit field deletion.
   */
  bool fieldDelete();

  /** @brief appendNewKey creates by dialog (ZKeyDLg) a new empty key (no key fields) and appends it to key tree view.
   * returns a QStandardItem pointer onto the created key item.
   * returns nullptr if key has not been created for any reason.
   */
  QStandardItem *appendNewKey();
  /** @brief insertNewKey creates by dialog (ZKeyDLg) a new empty key (no key fields) and inserts it before current key.
   * NB: if tree view is empty, then appendNewKey is called.
   * returns a QStandardItem pointer onto the created key item.
   * returns nullptr if key has not been created for any reason.
   */
  QStandardItem* insertNewKey();

#ifdef __DEPRECATED__
  /**  @brief keyCopyToPinboard Duplicates and copies the whole key data (key and key fields) to a temporary data structure held by Pinboard
   *  If this routine is successful, Pinboard.last() contains the duplicated data under a ZDataReference with type <ZEntity_KeyField> */
  bool keyCopyToPinboard(QModelIndex pIdx);
  /** @brief keyDelete Deletes the whole key (key and key fields) from tree view as well as from key dictionary
   *          deletes also associated infradata */
//  bool keyDelete(const QModelIndex &pIdx) { return _keyDelete(pIdx);}
#endif // __DEPRECATED__

  /**  @brief _keyDelete Deletes the whole key (key and children key fields) from tree view
   *          deletes also associated infradata and release associated resource. */
  bool _keyDelete(const QModelIndex &pIdx);

  /** @brief keyChange modify by dialog (ZKeyDLg) key characteristics pointed by pIdx.
   * Only attributes local to key are changed.
   * Control is made on key name that must not be a duplicate name.
   */
  QStandardItem* keyChange();
//  inline bool _keyChange(const QModelIndex &pIdx, ZKeyHeaderRow & pKHR);

#ifdef __DEPRECATED__

  /** @brief KeyfieldInsert   creates a key field from a ZFieldDescription pField
   * within current key and inserts it before key field pointed by pIdx.
   * If key has no children fields, then it is appended.*/
  bool fieldInsertToKey(const QModelIndex &pIdx,ZFieldDescription& pField);


  /**  @brief _keyFieldCopy Duplicates and copies the key field data to a temporary structure  ZKeyFieldRowTemporary.
   *  this routine delivers a ZKeyFieldRowTemporary content with effective key field data as ZKeyFieldRow and a QList<QList> with QStandardItem rows :
   *  Corresponding infradata is created and copied within duplicated rows (item column 0) */
  bool keyFieldCopyToPinboard (QModelIndex pIdx);
  /** @brief keyTRvInsertFromPinboard   inserts a key field from Pinboard last element
   * towards current key before key field pointed by pIdx.
   * If key has no children fields, then it is appended.
   * At the end of the process, temporary structure is deleted.
   * Accepts only a key field in input. Return false otherwise without any message.
   * @param pIdx: index to insert key before
  */
  bool keyTRvInsertFromPinboard(QModelIndex pIdx);


//  bool _keyInsertToKey(QModelIndex pIdx,ZKeyHeaderRow* pKHR);
//  bool _keyFieldInsertToKey(QModelIndex pIdx,ZKeyFieldRow* pKFR);

  bool _fieldInsertToKey(QModelIndex pIdx, QStandardItem *pFieldItem);


//  bool _keyAppendToKey(QModelIndex pIdx,ZKeyHeaderRow* pKHR);
  bool _keyFieldAppendToKey(QModelIndex pIdx,ZKeyFieldRow* pKFR);
  bool _fieldAppendToKey(QModelIndex pIdx,ZFieldDescription* pField);

  /** @brief keyfieldAppendFromPinboard pIdx must point to a key row
   * Accepts only a key field in input. Return false otherwise without any message. */
  bool keyTRvAppendFromPinboard(const QModelIndex &pIdx);


  /** @brief fieldAppendToKey  creates a key field from given field description ZFieldDescription
   *  and append this key field to current key (last position)
   *  if tree view is empty (no key defined yet) : create a key with name __NEW_KEY__ and append the key field to that key
   *  accepts a normal field and format a key field from it before appending it to the key pointed by pIdx */
  bool fieldAppendToKey(const QModelIndex &pIdx, ZFieldDescription& pField);
  bool fieldAppendToKeyFromPinboard(const QModelIndex &pIdx);
#endif // __DEPRECATED__

  /** @brief DicEdit::_keyDelete deletes key field pointed by pIdx :
   *  - delete key field row and deletes infradata associated to it
   *  - recomputes and displays key universal size value and each key field offset within the mother key. */
  bool _keyfieldDelete(QModelIndex pIdx);

  inline void _recomputeKeyValues(QStandardItem* wKF);

  bool keyItemRowCutToPinboard(int pRow);
  bool keyfieldItemRowCutToPinboard(QStandardItem* pKeyItem,int pRow);

  bool keyItemRowPasteFromPinboard() ;
  bool keyItemRowInsertFromPinboard(int pRow);
  bool keyfieldItemRowPasteFromPinboard(QStandardItem* pKeyItem) ;
  bool keyfieldItemRowInsertFromPinboard(QStandardItem* pKeyItem,int pRow);

  /** @brief keyInsert creates by dialog (ZKeyDLg) a new empty key (no key fields) and appends it to tree view as well as key dictionary.   */
  bool fieldMoveup();
  bool fieldMovedown();
  bool keyMoveup();
  bool keyMovedown();
  bool keyfieldMoveup();
  bool keyfieldMovedown();

/** @brief acceptFieldChange replaces the values (displayed value and dictionary value) of a field at model index wIdx */
  void acceptFieldChange(ZFieldDescription& pField, QModelIndex &wIdx);

  /** @brief displayFieldChange manage to change a field description : display the new field description and change dictionary accordingly
   */
//  bool displayFieldChange(ZFieldDescription& pField, QModelIndex pIdx);

#ifdef __DEPRECATED__
  void importDicFromFullHeader(ZDataBuffer &pHeaderContent);
  void importDicFromReserved(const unsigned char *pPtrReserved);
  void importDic(const unsigned char *pPtrIn);
#endif

  /* Set-up MetaDic and KeyDictionary display views */

  ZStatus loadXmlDictionary (const uriString& pXmlDic);
  void displayZMFDictionary(ZMFDictionary &pDic);
  void displayKeyDictionaries(ZMFDictionary &pDic);


  bool saveOrCreateDictionaryFile();
  bool loadDictionaryFile();
  void manageDictionaryFiles();

  ZStatus saveCurrentDictionary (unsigned long &pVersion, bool &pActive, utf8VaryingString &pDicName);

  ZStatus loadDictionaryFile(const uriString& pDicPath);

  /** @brief recomputeKeyValues recomputes and updates all key values for key whose model index is pKeyIdx.
   *  These values are Key universal size, and for each field, key offset, depending on field row position
   */
  bool updateKeyValues(const QModelIndex &pKeyIdx);

  ZMFDictionary* screenToDic();

  void closeEvent(QCloseEvent *event) override {
    Quit();
  }

  ZDataBuffer           ContentToDump;
  long                  Offset=0;
  const unsigned char*  PtrIn=nullptr;

  ZQTableView* setupFieldTBv(ZQTableView *KeyTRv,bool pColumnAutoAdjust, int pColumns);
  ZQTreeView*  setupKeyTRv(ZQTreeView *KeyTRv, bool pColumnAutoAdjust, int pColumns);

  void _registerQuitCallback (std::function<void()> pQuitCallback) { QuitDicEditCallback = pQuitCallback; }


  void KeyTRvFlexMenu(QContextMenuEvent *event);
  void FieldTBvFlexMenu(QContextMenuEvent *event);

  void KeyTRvKeyFiltered(int pKey, QKeyEvent *pEvent);
  void KeyTRvMouseFiltered(int pKey, QMouseEvent *pEvent);
  void FieldTBvKeyFiltered(int pKey, QKeyEvent *pEvent);
  void FieldTBvMouseFiltered(int pKey, QMouseEvent *pEvent);

  void KeyTRvRawStartDrag();
  bool KeyTRvRawDropEvent(QDropEvent *pTarget); /* only used raw drop event: low level managed drop */


  bool keyTRvInsertFromPinboard (QModelIndex pIdx);

  void DestructorCallback();

//  bool FieldTBvDropEvent(int pDropType,ZDataReference * pSource,ZDataReference *pTarget);
  void FieldTBvRawStartDrag();
  bool FieldTBvRawDropEvent(QDropEvent *pTarget); /* only used raw drop event: low level managed drop */

  /** @brief changeFieldRowFromField replace the content of each item of the given row with pField content.
   *  infra data is replaced ZDescriptionField instance is deleted and a new instance is created,
   *  resource is released and a new resource is allocated.
   */
  QStandardItem *changeFieldRowFromField(int pRow, ZFieldDescription &pField);

  QMenu *setupKeyFlexMenu();
  QMenu *setupFieldFlexMenu();
  void setupReadWriteMenu();

  bool getDicName(unsigned long &pVersion, bool &pActive, utf8VaryingString &pDicName);

  /** @brief getGenNames  gets two names while generating code : the class name to be generated and the output file base name.
   * if output file base name is empty, class name is chosen.
   * Output file base name will be used to create two files a header file and a cpp source file.
   * @return a boolean : true -> OK, false -> quit or error
   */
  bool getGenerationNames(utf8VaryingString &pOutFileBaseName, utf8VaryingString &pClass, utf8VaryingString &pBrief, utf8VaryingString &pGenPath);

  void searchDirectory();

  void readWriteActionEvent(QAction*pAction);

  void displayErrorCallBack(const utf8VaryingString &pMessage) ;

  void resizeEvent(QResizeEvent* pEvent) override;
  bool _FResizeInitial=true;

  void renewDicFile(ZMFDictionary& pDic);

  void statusBarMessage(const char* pFormat,...);

  void closeGenShowCppCB(QEvent *pEvent);
  void closeGenShowHeaderCB(QEvent *pEvent);

public slots:
  void keyActionEvent(QAction*pAction);
  void fieldActionEvent(QAction*pAction);
  void generalActionEvent(QAction* pAction);
  void dicDescriptionClicked();

private:

  RawFields* rawFields=nullptr;

  zbs::ZCppGenerate* GenerateEngine=nullptr;

  textEditMWn*  GenCppFileWin=nullptr;
  textEditMWn*  GenHeaderFileWin=nullptr;

  QMenu *generalMEn=nullptr;
  QActionGroup *generalGroup=nullptr;
  QAction* parserQAc=nullptr;
  QAction* quitQAc=nullptr;

  QMenu*    generateMainMEn=nullptr;
  QAction*  generateQAc=nullptr;

  QMenu*    genShowHideMEn=nullptr;
  QAction*  genShowLogQAc=nullptr;
  QAction*  genShowHeaderQAc=nullptr;
  QAction*  genShowCppQAc=nullptr;

  QMenu*    genFileMEn=nullptr;
  QAction*  fileXmlGenQAc=nullptr;
  QAction*  fileCreateQAc=nullptr;


  QMenu *keyFlexMEn=nullptr;
  QActionGroup* keyActionGroup=nullptr;
  QAction* KInsertKeyQAc=nullptr;
  QAction* KAppendKeyQAc=nullptr;
  QAction* KChangeQAc=nullptr;
//  QAction* KDeleteKeyQAc=nullptr;

  /* no copy choice in keyTRv menu */
  QAction* KDeleteQAc=nullptr;
  QAction* KcutQAc=nullptr;
  QAction* KpasteQAc=nullptr;
  QAction* KappendQAc=nullptr;
  QAction* KMoveupQAc=nullptr;
  QAction* KMovedownQAc=nullptr;
//  QAction* KKeyfieldMoveupQAc=nullptr;
//  QAction* KKeyfieldMovedownQAc=nullptr;

  QMenu *fieldFlexMEn=nullptr;
  QActionGroup* fieldActionGroup=nullptr;
  QAction* FInsertQAc=nullptr;
  QAction* FAppendQAc=nullptr;
  QAction* FChangeQAc=nullptr;
  QAction* FDeleteQAc=nullptr;
  QAction* FCutQAc=nullptr;
  QAction* FcopyQAc=nullptr;
  QAction* FpasteQAc=nullptr;
  QAction* FmoveupQAc=nullptr;
  QAction* FmovedownQAc=nullptr;

  QAction* SaveQAc=nullptr;
  QMenu *  FwritetoMEn=nullptr;
  QAction* FwritetoclipQAc=nullptr;
  QAction* FviewXmlQAc=nullptr;
  QAction* FwriteXmltofileQAc=nullptr;
  QAction* FwritetoDicQAc=nullptr;

  QMenu *  FloadMEn=nullptr;
  QAction* FloadfromclipQAc=nullptr;
  QAction* FloadfromXmlFileQAc=nullptr;
  QAction* FloadfromDicQAc=nullptr;

  QItemSelectionModel* FieldSM=nullptr;
  QItemSelectionModel* KeySM=nullptr;


  QLineEdit* GenPathLEd=nullptr;

//  ZMFDictionary* MasterDic=nullptr;

  ZaiErrors Errorlog;

  uriString SelectedDirectory;

  QPersistentModelIndex OriginIndex;

  ZDictionaryFile* DictionaryFile=nullptr;
  uriString XmlDictionaryFile;
  bool DictionaryChanged=false;

  QWidget* Parent=nullptr;
  std::function<void()> QuitDicEditCallback=nullptr;

  Ui::DicEdit *ui;
};

/* @brief  createFieldRowFromField creates QStandardItem row for a field and link appropriate infra data to item[0] */
QList<QStandardItem*> createFieldRowFromField(ZFieldDescription* pField);

/* @brief  createKeyDicRow creates QStandardItem row for key dictionary and link appropriate infra data to item[0] */
QList<QStandardItem *> createKeyDicRow(const ZKeyHeaderRow& pKHR);

/* @brief  createKeyFieldRow creates QStandardItem row for key field and link appropriate infra data to item[0] */
QList<QStandardItem *> createKeyFieldRow(const ZKeyFieldRow& wKFR);

/* @brief  createKeyFieldRowFromField creates QStandardItem row for a field from a ZFieldDescription and link appropriate infra data to item[0] */
QList<QStandardItem *> createKeyFieldRowFromField(ZFieldDescription* pField);

bool controlField(ZFieldDescription &pField);

/* @brief  setFieldRowFromField changes QStandardItem row for field with a ZFieldDescription
 *  and change accordingly infra data linked to item[0] */
QStandardItem *setFieldRowFromField(QStandardItemModel* pModel, int pRow, ZFieldDescription& pField);


/* search all keys for key fields that have wItem linked */
class KeyFieldRes {
public:
  KeyFieldRes()=default;
  KeyFieldRes(const KeyFieldRes& pIn) {_copyFrom(pIn);}

  KeyFieldRes& _copyFrom(const KeyFieldRes& pIn) {
    KeyName=pIn.KeyName;
    KeyFieldRow = pIn.KeyFieldRow;
    return *this;
  }

  utf8VaryingString KeyName;
  QStandardItem*    KeyFieldRow;
};


template <class _Tp>
_Tp* getItemData(QStandardItem* pItem) {
  if (pItem==nullptr)
    return nullptr;
  QVariant wV;
  ZDataReference wDRef;
  wV=pItem->data(ZQtDataReference);
  if (wV.isNull())
    return nullptr;
  wDRef=wV.value<ZDataReference>();
  if (wDRef.isInvalid())
    return nullptr;
  return wDRef.getPtr<_Tp*>();
}
#endif // DICEDIT_H
