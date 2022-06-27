#ifndef ZCU_COMMON_H
#define ZCU_COMMON_H

#include <stdint.h>
#include <QStandardItem>
#include <QList>
#include <ztoolset/utfvaryingstring.h>
#include <zdatareference.h>

class ZPinboard;

#ifndef ZCU_COMMON_CPP

extern ZPinboard   Pinboard;
extern bool        UseMimeData;
extern bool        DragEngaged;
#endif

class ZResource;
typedef uint64_t ZEntity_type;

ZResource getNewResource(ZEntity_type pEntity);
void releaseResource(ZResource pResource);




extern Qt::AlignmentFlag QtAlignmentFlag;     /* defined within ZQStandardItem.cpp */


/* here below : instantiated within displaymain.cpp */

void setItemAlignment(const Qt::AlignmentFlag pFlag);


//class QStandardItem;

QStandardItem* createItem(const utf8VaryingString& pValue,const char*pFormat);

QStandardItem* createItem(uint8_t pValue,const char*pFormat);
QStandardItem* createItem(uint32_t pValue,const char*pFormat);
QStandardItem* createItem(int32_t pValue,const char*pFormat);

QStandardItem* createItem(unsigned long pValue,const char*pFormat);

QStandardItem* createItem(long pValue,const char*pFormat);



//ZDataReference getZDataReference(QStandardItemModel* pModel, int pRow);
ZDataReference getParent( QStandardItemModel* pItemModel,QModelIndex &wIdx);
/* creates a list pointing to effective items */
QList<QStandardItem*> itemRow (QStandardItemModel*pModel,QModelIndex& pIdx, ZaiErrors* pErrorlog=nullptr);
/* creates a list pointing to duplicated items */
QList<QStandardItem*> itemRowDuplicate (QStandardItemModel*pModel,const QModelIndex&  pIdx);

/* creates a list pointing to effective items after having cut them */
QList<QStandardItem*> cutRow (QStandardItemModel*pModel,QModelIndex& pIdx, ZaiErrors* pErrorlog=nullptr);
/* creates a list pointing to cloned items : each item of the row is cloned to a copied row */
QList<QStandardItem*> cloneRow (QStandardItemModel*pModel,QModelIndex& pIdx, ZaiErrors* pErrorlog=nullptr);
/** @brief cloneRow creates a cloned list of items : Row is selected as child of row pRow for item pFather.
 *  if pFather is nullptr then top level of item model
*/
QList<QStandardItem*> cloneRow (QStandardItemModel*pModel,QStandardItem* pFather,int pRow, ZaiErrors* pErrorlog=nullptr);

#endif // ZCU_COMMON_H
