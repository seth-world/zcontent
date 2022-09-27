#ifndef ZKEYDLG_H
#define ZKEYDLG_H

#define __NEW_FIELDNAME__ "new field"
#define __NEW_KEYNAME__ "new key"

#include <QDialog>

//#include <zcontent/zindexedfile/zkeydictionary.h>
#include <zcontent/zcontentutils/zdisplayedfield.h>

namespace Ui {
class ZKeyDLg;
}
class QStatusBar;

class ZKeyDLg : public QDialog
{
  Q_OBJECT

public:
//  explicit ZKeyDLg(ZMFDictionary *pMasterDic,QWidget *parent = nullptr);
  explicit ZKeyDLg(QWidget *parent = nullptr);
  ~ZKeyDLg();

//  void set(KeyDic_Pack* pKeyDic);
  void set(ZKeyHeaderRow* pKeyHeaderRow);
  void setCreate();
  ZKeyHeaderRow get() {return KHR;}

  /* updates internal ZKeyHeaderRow object with dialog fields data  */
  void refresh();

public slots:
  void OKBTnClicked();
  void CancelBTnClicked();
private:
  /* NB: usage of dictionary is deprecated */
//  KeyDic_Pack* KeyDic=nullptr;
//  ZMFDictionary*  MasterDic=nullptr;
//  ZKeyDictionary* KeyDic=nullptr;
  ZKeyHeaderRow   KHR;
  Ui::ZKeyDLg *ui;
};

#endif // ZKEYDLG_H
