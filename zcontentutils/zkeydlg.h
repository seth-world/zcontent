#ifndef ZKEYDLG_H
#define ZKEYDLG_H

#define __NEW_FIELDNAME__ "new field"
#define __NEW_KEYNAME__ "new key"

#include <QDialog>

#include <zcontent/zindexedfile/zkeydictionary.h>

namespace Ui {
class ZKeyDLg;
}

class ZKeyDLg : public QDialog
{
  Q_OBJECT

public:
  explicit ZKeyDLg(ZMFDictionary *pMasterDic,QWidget *parent = nullptr);
  ~ZKeyDLg();

//  void set(KeyDic_Pack* pKeyDic);
  void set(ZKeyHeaderRow* pKeyDic);
  void setCreate();
  ZKeyDictionary* get() {return KeyDic;}


public slots:
  void OKBTnClicked();
  void CancelBTnClicked();
private:
//  KeyDic_Pack* KeyDic=nullptr;
  ZMFDictionary*  MasterDic=nullptr;
  ZKeyDictionary* KeyDic=nullptr;
  Ui::ZKeyDLg *ui;
};

#endif // ZKEYDLG_H
