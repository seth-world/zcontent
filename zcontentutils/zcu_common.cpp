#ifndef ZCU_COMMON_CPP
#define ZCU_COMMON_CPP

#include "zcu_common.h"

#include <zpinboard.h>


ZPinboard   Pinboard;
bool        UseMimeData=false;
bool        DragEngaged=false;



/************************************************/
/* simulates resource allocation manager */
static int ResourceCounter=0;

ZResource getNewResource(ZEntity_type pEntity)
{
  return ZResource (++ResourceCounter,pEntity)  ;
}
void releaseResource(ZResource pResource)
{
  return  ;
}
/***********************************************/


/* creates a list pointing to effective items */
QList<QStandardItem*> itemRow (QStandardItemModel*pModel,QModelIndex& pIdx, ZaiErrors* pErrorlog)
{
  QList<QStandardItem*> wRow;
  if (!pIdx.isValid())
  {
    if (pErrorlog)
      pErrorlog->errorLog("itemRow-E-INVIDX Index is not valid.\n");
    else
      fprintf(stderr,"itemRow-E-INVIDX Index is not valid.\n");
    return wRow;
  }

  /* get the number of columns for this kind of tree lines */
  int wColumns = pModel->columnCount(pIdx.parent());

  QStandardItem* wFatherItem= pModel->itemFromIndex(pIdx.parent()); /* general case get parent item */

  int wi=0;
  if (wFatherItem==nullptr) /* if top level */
    while (wi < wColumns)
    {
      QStandardItem* wItem=pModel->item(pIdx.row(),wi++);
      wRow << wItem;
    }
  else
    while (wi < wColumns)
    {
      QStandardItem* wItem=wFatherItem->child(pIdx.row(),wi++);
      wRow << wItem;
    }

  return wRow;
}//itemRow

/* creates a list pointing to cloned items */
QList<QStandardItem*> cloneRow (QStandardItemModel*pModel,QModelIndex& pIdx, ZaiErrors* pErrorlog)
{
  QList<QStandardItem*> wRow;
  if (!pIdx.isValid())
  {
    if (pErrorlog)
      pErrorlog->errorLog("cloneRow-E-INVIDX Index is not valid.\n");
    else
      fprintf(stderr,"cloneRow-E-INVIDX Index is not valid.\n");
    return wRow;
  }

  /* get the number of columns cloneRowfor this kind of tree lines */
  int wColumns = pModel->columnCount(pIdx.parent());

  QStandardItem* wFatherItem= pModel->itemFromIndex(pIdx.parent()); /* general case get parent item */

  int wi=0;
  if (wFatherItem==nullptr) /* if top level */
    while (wi < wColumns)
    {
      QStandardItem* wItem=pModel->item(pIdx.row(),wi++)->clone();
      wRow << wItem;
    }
  else
    while (wi < wColumns)
    {
      QStandardItem* wItem=wFatherItem->child(pIdx.row(),wi++)->clone();
      wRow << wItem;
    }

  return wRow;
}//cloneRow
QList<QStandardItem*> cloneRow (QStandardItemModel*pModel,QStandardItem* pFather,int pRow, ZaiErrors* pErrorlog)
{
  QList<QStandardItem*> wRow;

  /* get the number of columns cloneRowfor this kind of tree lines */

  int wColumns = pModel->columnCount();

  int wi=0;
  if (pFather==nullptr) /* if top level */
    while (wi < wColumns)
    {
      QStandardItem* wItem=pModel->item(pRow,wi++)->clone();
      wRow << wItem;
    }
  else
    while (wi < wColumns)
    {
      QStandardItem* wItem=pFather->child(pRow,wi++)->clone();
      wRow << wItem;
    }

  return wRow;
}//cloneRow
/* creates a list pointing to effective items after having cut them */
QList<QStandardItem*> cutRow (QStandardItemModel*pModel,QModelIndex& pIdx, ZaiErrors* pErrorlog)
{
  QList<QStandardItem*> wRow;
  if (!pIdx.isValid())
  {
    if (pErrorlog)
      pErrorlog->errorLog("cutRow-E-INVIDX Index is not valid.\n");
    else
      fprintf(stderr,"cutRow-E-INVIDX Index is not valid.\n");
    return wRow;
  }

  /* get the number of columns for this kind of tree lines */
  int wColumns = pModel->columnCount(pIdx.parent());

  QStandardItem* wFatherItem= pModel->itemFromIndex(pIdx.parent()); /* general case get parent item */

  int wi=0;
  if (wFatherItem==nullptr) /* if top level */
    return pModel->takeRow(pIdx.row());
  else
    return wFatherItem->takeRow(pIdx.row());
}//cutRow

QList<QStandardItem*> cutRow (QStandardItemModel*pModel,QStandardItem* wFatherItem, int pRow, ZaiErrors* pErrorlog)
{
  QList<QStandardItem*> wRow;

  int wi=0;
  if (wFatherItem==nullptr) /* if top level */
    return pModel->takeRow(pRow);
  else
    return wFatherItem->takeRow(pRow);
}//cutRow


/* hereafter is buggy */
#ifdef __COMMENT__
ZDataReference
getZDataReference(QStandardItemModel *pModel, int pRow)
{
  QStandardItem* wItem= pModel->item(pRow,0);
  QVariant wV=wItem->data(ZQtDataReference);
  if (!wV.isValid())
    return ZDataReference(); /* return invalid DRef */
  ZDataReference wDRef=wV.value<ZDataReference>();
  return wDRef;
}
#endif // __COMMENT__


#endif//ZCU_COMMON_CPP
