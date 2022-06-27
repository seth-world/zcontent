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
//#include <zcontent/zindexedfile/zkeydictionary.h>


extern const int cst_MessageDuration ;



class ZDataReference;
namespace  zbs {
class ZKeyDictionary;
class ZFieldDescription;
class KeyField_Pack;
class ZIndexField;
class ZDictionaryFile;
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

/*
class ZKeyRowTemporary {
public:
  ZKeyRowTemporary()=default;
  ZKeyRowTemporary(const ZKeyRowTemporary& pIn);

  ZKeyRowTemporary& _copyFrom(const ZKeyRowTemporary& pIn) {
    KeyData=pIn.KeyData;
    KeyRows=pIn.KeyRows;
    return *this;
  }
  ZKeyDictionary*               KeyData = nullptr;
  QList<QList<QStandardItem*>>  KeyRows ;
};

class ZKeyFieldRowTemporary {
public:
  ZKeyFieldRow          KFR;
  QList<QStandardItem*> Row ;
};
*/
class DicEdit : public QMainWindow
{
  Q_OBJECT

public:
  explicit DicEdit(QWidget *parent = nullptr);
  ~DicEdit();

  ZQTreeView*     keyTRv=nullptr;
  ZQTableView*    fieldTBv=nullptr;

  ZFieldDLg*      FieldDLg=nullptr;

  void Quit() ;

  void clear();
  void clearAllRows();


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


  /** @brief fieldInsert  creates a new field using dialog ZFieldDLg and insert it before the field pointed by pIdx .
   *  Infradata and dictionary data are changed accordingly. */
  bool fieldInsertNewBefore(QModelIndex pIdx);
  bool fieldInsertNewAfter(QModelIndex pIdx);

  /** @brief _fieldInsert  insert field wFDesc at index pIdx to table view and to related dictionary array
   *                      if table view is empty or if pIdx is invalid, then appends. */

  /** @brief fieldAppend  creates a new field using dialog ZFieldDLg and appends it to table view and dictionary.
   *  Infradata and dictionary data are changed accordingly. */
  bool fieldAppend();

  /** @brief _fieldAppend append wFDesc to table view and to related dictionary array */
  bool _fieldAppend(ZFieldDescription &wFDesc);

  /** @brief fieldCopyToPinboard copies the field data pointed by pIdx to pinboard */
  bool fieldCopyToPinboard(QModelIndex pIdx);

  /** @brief fieldCutToPinboard copies the field data pointed by pIdx to pinboard then delete it from table view and dictionary */
  bool fieldCutToPinboard(const QModelIndex &pIdx);

  /** @brief fieldInsertBeforeFromPinboard inserts the field description contained in Pinboard before field pointed by pIdx
   */
  bool fieldInsertBeforeFromPinboard(QModelIndex pIdx);

  /** @brief fieldInsertAfterFromPinboard inserts the field description contained in Pinboard before field pointed by pIdx
   */
  bool fieldInsertAfterFromPinboard(QModelIndex pIdx);


  /** @brief fieldInsertAfterFromPinboard inserts the field description contained in Pinboard BEFORE field pointed by pIdx
   */
  bool _fieldInsertBefore(QModelIndex pIdx, ZFieldDescription &pFDesc);

  /** @brief fieldInsertAfterFromPinboard inserts the field description contained in Pinboard AFTER field pointed by pIdx
   */
  bool _fieldInsertAfter(QModelIndex pIdx, ZFieldDescription &pFDesc);

  /** @brief fieldAppendFromPinboard gets a field definition from last pinboard element and appends it to field table view and dictionary.
   */
  bool fieldAppendFromPinboard();

//  bool fieldInsertBefore(QModelIndex &wIdx,ZFieldDescription& pField);
  /** @brief fieldChange  changes the content of field pointed by pIdx using dialog ZFieldDLg.
   *  Infradata and dictionary data are changed accordingly.
   */
  bool fieldChange(QModelIndex pIdx);

  /**
   * @brief _fieldDelete deletes the field pointed by pIdx from table view and dictionary table.
                              A control is made wether field is part of keys. If yes a message is issued.
                              Deletion could be forced if flag pForceDelete is set to true  */
  bool _fieldDelete(QModelIndex pIdx, bool pForceDelete=false);


  /** @brief keyInsert creates by dialog (ZKeyDLg) a new empty key (no key fields) and inserts it before key pointed by pIdx.   */
  bool keyInsert (const QModelIndex &pIdx);
  /** @brief _keyInsert inserts a new key as pkey - and all its key fields - before key pointed by pIdx  within tree view and within key dictionary :
   * if tree view is empty or if pIdx is invalid then key is appended */
  inline bool _keyInsert (const QModelIndex& pIdx, ZKeyDictionary *pKey);
  /** @brief keyInsert creates by dialog (ZKeyDLg) a new empty key (no key fields) and appends it to tree view as well as key dictionary.   */
  bool keyAppend();
  /** @brief _keyAppend appends a key as pkey - and all its key fields -  within tree view and within key dictionary  */
  inline bool _keyAppend (ZKeyDictionary* pKey);
  bool keyAppendFromPinboard();
  /**  @brief keyCopyToPinboard Duplicates and copies the whole key data (key and key fields) to a temporary data structure held by Pinboard
   *  If this routine is successful, Pinboard.last() contains the duplicated data under a ZDataReference with type <ZEntity_KeyField> */
  bool keyCopyToPinboard(QModelIndex pIdx);
  /** @brief keyDelete Deletes the whole key (key and key fields) from tree view as well as from key dictionary
   *          deletes also associated infradata */
  bool keyDelete(const QModelIndex &pIdx) { return _keyDelete(pIdx);}
  /**  @brief _keyDelete Deletes the whole key (key and key fields) from tree view as well as from key dictionary
   *          deletes also associated infradata */
  bool _keyDelete(const QModelIndex &pIdx);

  bool _keyFieldDelete(const QModelIndex &pIdx);

  /** @brief keyChange modify by dialog (ZKeyDLg) key characteristics pointed by pIdx. Only attributes local to key are changed.   */
  bool keyChange(QModelIndex pIdx);
//  inline bool _keyChange(const QModelIndex &pIdx, ZKeyHeaderRow & pKHR);

  /**  @brief _keyFieldCopy Duplicates and copies the key field data to a temporary structure  ZKeyFieldRowTemporary.
   *  this routine delivers a ZKeyFieldRowTemporary content with effective key field data as ZKeyFieldRow and a QList<QList> with QStandardItem rows :
   *  Corresponding infradata is created and copied within duplicated rows (item column 0) */
  bool keyFieldCopyToPinboard (QModelIndex pIdx);

  /** @brief KeyfieldInsert   creates a key field from a ZFieldDescription pField
   * within current key and inserts it before key field pointed by pIdx.
   * If key has no children fields, then it is appended.*/
  bool fieldInsertToKey(const QModelIndex &pIdx,ZFieldDescription& pField);

  /** @brief keyTRvInsertFromPinboard   inserts a key field from Pinboard last element
   * towards current key before key field pointed by pIdx.
   * If key has no children fields, then it is appended.
   * At the end of the process, temporary structure is deleted.
   * Accepts only a key field in input. Return false otherwise without any message.
   * @param pIdx: index to insert key before
  */
  bool keyTRvInsertFromPinboard(QModelIndex pIdx);

//  bool _keyInsertToKey(QModelIndex pIdx,ZKeyHeaderRow* pKHR);
  bool _keyFieldInsertToKey(QModelIndex pIdx,ZKeyFieldRow* pKFR);
  bool _fieldInsertToKey(QModelIndex pIdx,ZFieldDescription* pField);
  bool _fieldInsertToKey1(QModelIndex pIdx,ZFieldDescription* pField);

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

  /** @brief DicEdit::_keyDelete deletes key field pointed by pIdx :
   *  - delete key field row and deletes infradata associated to it
   *  - recomputes and displays key universal size value and each key field offset within the mother key. */
  bool _keyfieldDelete(QModelIndex pIdx);

  /**  @brief keyCutToPinboard Copies the whole key data to pinboard (key and key fields), and deletes it from tree view as well as from key dictionary */
  bool keyCutToPinboard(const QModelIndex &pIdx);

  inline void _recomputeKeyValues(QStandardItem* wKF);

/** @brief acceptFieldChange replaces the values (displayed value and dictionary value) of a field at model index wIdx */
  void acceptFieldChange(ZFieldDescription& pField, QModelIndex &wIdx);

  /** @brief displayFieldChange manage to change a field description : display the new field description and change dictionary accordingly
   */
  bool displayFieldChange(ZFieldDescription& pField, QModelIndex pIdx);

  void importDicFromFullHeader(ZDataBuffer &pHeaderContent);
  void importDicFromReserved(const unsigned char *pPtrReserved);
  void importDic(const unsigned char *pPtrIn);

  /* Set-up MetaDic and KeyDictionary display views */

  void setDictionary (const ZMFDictionary& pZMFDic);
  void setNewDictionary();
  ZStatus loadXmlDictionary (const uriString& pXmlDic);
  void displayZMFDictionary(ZMFDictionary *pDic);
  void displayKeyDictionaries(ZMFDictionary *pDic);
  QList<QStandardItem *> createKeyDicRow(const ZKeyHeaderRow& pKHR);
  QList<QStandardItem *> createKeyFieldRow(const ZKeyFieldRow& wKFR);


  bool saveOrCreateDictionaryFile();
  bool loadDictionaryFile();

  ZStatus loadDictionaryFile(const uriString& pDicPath, long pRank=0);
  ZStatus saveDictionaryFile(const uriString& pDicPath, unsigned long pVersion);
  ZStatus createDictionaryFile(const uriString& pDicPath, unsigned long pVersion);


  /** @brief recomputeKeyValues recomputes and updates all key values for key whose model index is pKeyIdx.
   *  These values are Key universal size, and for each field, key offset, depending on field row position
   */
  bool updateKeyValues(const QModelIndex &pKeyIdx);

  ZMFDictionary* screenToDic();

  void setFileClosed(bool pYesNo);

  void closeEvent(QCloseEvent *event) override {
    Quit();
  }


  ZDataBuffer           ContentToDump;
  long                  Offset=0;
  const unsigned char*  PtrIn=nullptr;

  ZQTableView* setupFieldTBv(ZQTableView *KeyTRv,bool pColumnAutoAdjust, int pColumns);
  ZQTreeView*  setupKeyTRv(ZQTreeView *KeyTRv, bool pColumnAutoAdjust, int pColumns);

  void KeyTRvFlexMenu(QContextMenuEvent *event);
  void FieldTBvFlexMenu(QContextMenuEvent *event);

  void KeyTRvKeyFiltered(int pKey, QKeyEvent *pEvent);
  void KeyTRvMouseFiltered(int pKey, QMouseEvent *pEvent);
  void FieldTBvKeyFiltered(int pKey, QKeyEvent *pEvent);
  void FieldTBvMouseFiltered(int pKey, QMouseEvent *pEvent);

  void KeyTRvRawStartDrag();
  bool KeyTRvRawDropEvent(QDropEvent *pTarget); /* only used raw drop event: low level managed drop */

  void DestructorCallback();

//  bool FieldTBvDropEvent(int pDropType,ZDataReference * pSource,ZDataReference *pTarget);
  void FieldTBvRawStartDrag();
  bool FieldTBvRawDropEvent(QDropEvent *pTarget); /* only used raw drop event: low level managed drop */


  QMenu *setupKeyFlexMenu();
  QMenu *setupFieldFlexMenu();
  void setupReadWriteMenu();

  bool getDicName(unsigned long &pVersion, utf8VaryingString &pDicName);

  void readWriteActionEvent(QAction*pAction);

  void displayErrorCallBack(const utf8VaryingString &pMessage) ;

  void resizeEvent(QResizeEvent* pEvent) override;
  bool _FResizeInitial=true;

public slots:
  void keyActionEvent(QAction*pAction);
  void fieldActionEvent(QAction*pAction);
  void generalActionEvent(QAction* pAction);

private:

  RawFields* rawFields=nullptr;

  QMenu *generalMEn=nullptr;
  QActionGroup *generalGroup=nullptr;
  QAction* parserQAc=nullptr;
  QAction* quitQAc=nullptr;


  QMenu *keyFlexMEn=nullptr;
  QActionGroup* keyActionGroup=nullptr;
  QAction* KInsertKeyQAc=nullptr;
  QAction* KDeleteKeyQAc=nullptr;
  QAction* KDeleteQAc=nullptr;
  QAction* KcutQAc=nullptr;
  QAction* KpasteQAc=nullptr;
  QAction* KappendQAc=nullptr;

  QMenu *fieldFlexMEn=nullptr;
  QActionGroup* fieldActionGroup=nullptr;
  QAction* FInsertBeforeQAc=nullptr;
  QAction* FInsertAfterQAc=nullptr;
  QAction* FAppendQAc=nullptr;
  QAction* FChangeQAc=nullptr;
  QAction* FDeleteQAc=nullptr;
  QAction* FCutQAc=nullptr;
  QAction* FcopyQAc=nullptr;
  QAction* FpasteInsertBeforeQAc=nullptr;
  QAction* FpasteInsertAfterQAc=nullptr;
  QAction* FpasteAppendQAc=nullptr;

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

  ZMFDictionary* MasterDic=nullptr;

  ZaiErrors Errorlog;

  QPersistentModelIndex OriginIndex;

  ZDictionaryFile* DictionaryFile=nullptr;

  Ui::DicEdit *ui;
};

bool getIndexFieldData (QModelIndex& wIdx,ZDataReference &wDRef,ZFieldDescription &wFDesc);
QList<QStandardItem*> createFieldRowFromField(ZFieldDescription* pField);
bool controlField(ZFieldDescription &pField);
bool setFieldRowFromField(QList<QStandardItem*> &wRow,ZFieldDescription& pField);

#endif // DICEDIT_H
