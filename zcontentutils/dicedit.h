#ifndef DICEDIT_H
#define DICEDIT_H

#include <QMainWindow>
#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zarray.h>
#include <qtableview.h>
#include <QStandardItem>
#include <zcontent/zindexedfile/zfielddescription.h>

const int cst_MessageDuration = 20000;  /* milliseconds */


#define __NEW_FIELDNAME__ "new field"

class ZDataReference;
namespace  zbs {
class ZKeyDictionary;
class ZFieldDescription;
class KeyField_Pack;
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




class DicEdit : public QMainWindow
{
  Q_OBJECT

public:
  explicit DicEdit(QWidget *parent = nullptr);
  ~DicEdit();


  ZQTreeView*     keyTRv=nullptr;
  ZQTableView*    fieldTBv=nullptr;

  ZFieldDLg*      FieldDLg=nullptr;


  bool fieldCreateBefore(QModelIndex &wIdx);
  bool fieldMoveBefore(QModelIndex &wIdxSource,QModelIndex &wIdxTarget);

  bool fieldInsertBefore(QModelIndex &wIdx,ZFieldDescription& pField);
  bool fieldChange(QModelIndex &pIdx);
  bool fieldDelete(QModelIndex &pIdx);

  bool getIndexFieldData (QModelIndex& wIdx,ZDataReference &wDRef,ZFieldDescription &wFDesc);

  void acceptFieldChange(ZFieldDescription& pField, QModelIndex &wIdx);
  bool displayFieldChange(ZFieldDescription& pField,QModelIndex& pIdx);

  void importDicFromFullHeader(ZDataBuffer &pHeaderContent);
  void importDicFromReserved(const unsigned char *pPtrReserved);
  void importDic(const unsigned char *pPtrIn);

  /* Set-up MetaDic and KeyDictionary display views */

  void displayZMFDictionary(ZMFDictionary &pDic);
  void displayKeyDictionaries(ZMFDictionary &pDic);

  ZMFDictionary* screenToDic();

  void setFileClosed(bool pYesNo);

  ZDataBuffer           ContentToDump;
  long                  Offset=0;
  const unsigned char*  PtrIn=nullptr;

  void clear();

  ZQTableView* setupFieldTBv(ZQTableView *KeyTRv,bool pColumnAutoAdjust, int pColumns);
  ZQTreeView*  setupKeyTRv(ZQTreeView *KeyTRv, bool pColumnAutoAdjust, int pColumns);

  void KeyTRvFlexMenu(QContextMenuEvent *event);
  void FieldTBvFlexMenu(QContextMenuEvent *event);

  void GenericKeyFiltered(int pKey,int pMouse);
  void KeyTRvKeyFiltered(int pKey,int pMouse);
  void FieldTBvKeyFiltered(int pKey, int pMouseFlag);

//  bool KeyTRvDropEvent(int pDropType,ZDataReference * pSource, ZDataReference *pTarget);
  void KeyTRvRawStartDrag();
  bool KeyTRvRawDropEvent(QDropEvent *pTarget); /* only used raw drop event: low level managed drop */


//  bool FieldTBvDropEvent(int pDropType,ZDataReference * pSource,ZDataReference *pTarget);
  void FieldTBvRawStartDrag();
  bool FieldTBvRawDropEvent(QDropEvent *pTarget); /* only used raw drop event: low level managed drop */


  /** @brief recomputeKeyValues recomputes and updates all key values for key whose model index is pKeyIdx.
   *  These values are Key universal size, and for each field, key offset, depending on field row position
   */
  bool updateKeyValues(const QModelIndex &pKeyIdx);

  /** @brief cutAllKeyRows creates a list pointing to effective rows composing a key after having cut these rows */
  QList<QList<QStandardItem*>> cutAllKeyRows(const QModelIndex& pKeyIdx);
  /** @brief pasteAllKeyRows creates a list pointing to effective rows composing a key after having cut these rows */
  bool pasteAllKeyRows(const QModelIndex& pKeyIdx, QList<QList<QStandardItem*>>& pKeyRows);

  bool pinboardGetField (ZFieldDescription& pField);
  bool pinboardPushField (QModelIndex& wIdx);

  bool pinboardGetKeyField(int pKeyRank, QList<QStandardItem *> &pKeyFieldRow);

  /** @brief pinboardInsertField creates a dictionary field from an other dictionary field stored in pinboard
   */
  bool pinboardInsertField(QModelIndex wIdx);
  /** @brief pinboardInsertKeyField creates a key field from a dictionary field stored in pinboard
   */
  bool pinboardPasteFieldToKey(const QModelIndex& pKeyIdx, bool pAppend=false);

  /** @brief pinboardInsertKeyRowSet creates a key field from a list of rows (items) representing a key dictionary
   * and its key fields child rows : This is a move within keyTRv   */
  bool pinboardPasteKeyRowSet(const QModelIndex& pKeyIdx, bool pAppend=false);

  /** @brief pinboardInsertKeyFieldRow creates a key field from a key field row stored in pinboard
   *  This is a move within keyTRv */
  bool pinboardPasteKeyFieldRow(const QModelIndex& pKeyIdx, bool pAppend=false);



  QMenu *setupKeyFlexMenu();
  QMenu *setupFieldFlexMenu();
  void setupReadWriteMenu();

  utf8VaryingString getDicName();

  void readWriteActionEvent(QAction*pAction);

public slots:
  void keyActionEvent(QAction*pAction);
  void fieldActionEvent(QAction*pAction);

private:
  QMenu *keyFlexMEn=nullptr;
  QActionGroup *keyActionGroup=nullptr;
  QAction* KInsertKeyQAc=nullptr;
  QAction* KDeleteKeyQAc=nullptr;
  QAction* KDeleteQAc=nullptr;
  QAction* KcutQAc=nullptr;
  QAction* KpasteQAc=nullptr;
  QAction* KappendQAc=nullptr;
/*
  QMenu *  KwritetoMEn=nullptr;
  QAction* KwritetoclipQAc=nullptr;
  QAction* KwritetofileQAc=nullptr;

 QMenu *  KloadMEn=nullptr;
  QAction* KloadfromclipQAc=nullptr;
  QAction* KloadfromfileQAc=nullptr;
*/

  QMenu *fieldFlexMEn=nullptr;
  QActionGroup *fieldActionGroup=nullptr;
  QAction* FInsertBeforeQAc=nullptr;
  QAction* FInsertAfterQAc=nullptr;
  QAction* FChangeQAc=nullptr;
  QAction* FDeleteQAc=nullptr;
  QAction* FCutQAc=nullptr;
  QAction* FcopyQAc=nullptr;
  QAction* FpasteQAc=nullptr;

  QMenu *  FwritetoMEn=nullptr;
  QAction* FwritetoclipQAc=nullptr;
  QAction* FviewXmlQAc=nullptr;
  QAction* FwriteXmltofileQAc=nullptr;
  QAction* FwritetoDicQAc=nullptr;

  QMenu *  FloadMEn=nullptr;
  QAction* FloadfromclipQAc=nullptr;
  QAction* FloadfromXmlFileQAc=nullptr;
  QAction* FloadfromDicQAc=nullptr;


  ZMFDictionary* MasterDic=nullptr;


  Ui::DicEdit *ui;
};

extern Qt::AlignmentFlag QtAlignmentFlag;     /* defined within ZQStandardItem.cpp */


/* here below : instantiated within displaymain.cpp */

void setItemAlignment(const Qt::AlignmentFlag pFlag);


//class QStandardItem;

QStandardItem* createItem(const utf8String& pValue,const char*pFormat);

QStandardItem* createItem(uint8_t pValue,const char*pFormat);
QStandardItem* createItem(uint32_t pValue,const char*pFormat);
QStandardItem* createItem(int32_t pValue,const char*pFormat);

QStandardItem* createItem(unsigned long pValue,const char*pFormat);

QStandardItem* createItem(long pValue,const char*pFormat);

bool controlField(ZFieldDescription &pField);
bool getIndexFieldData (QModelIndex& wIdx,ZDataReference &wDRef,ZFieldDescription &wFDesc);

QList<QStandardItem*> createRowFromField(ZFieldDescription& pField);
ZDataReference getZDataReference(QStandardItemModel* pModel, int pRow);
ZDataReference getParent( QStandardItemModel* pItemModel,QModelIndex &wIdx);
/* creates a list pointing to effective items */
QList<QStandardItem*> itemRow (QStandardItemModel*pModel,QModelIndex& pIdx);
/* creates a list pointing to duplicated items */
QList<QStandardItem*> itemRowDuplicate (QStandardItemModel*pModel,const QModelIndex&  pIdx);
bool setRowFromField(QList<QStandardItem*> &wRow,ZFieldDescription& pField);
/* creates a list pointing to effective items after having cut them */
QList<QStandardItem*> cutRow (QStandardItemModel*pModel,QModelIndex& pIdx);



#endif // DICEDIT_H
