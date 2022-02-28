#ifndef ZKEYDLG_H
#define ZKEYDLG_H

#include <QDialog>

#include <zcontent/zindexedfile/zkeydictionary.h>

namespace Ui {
class ZKeyDLg;
}

class ZKeyDLg : public QDialog
{
  Q_OBJECT

public:
  explicit ZKeyDLg(QWidget *parent = nullptr);
  ~ZKeyDLg();

  void set(KeyDic_Pack* pKeyDic);

public slots:
  void OKBTnClicked();
  void CancelBTnClicked();
private:
  KeyDic_Pack* KeyDic=nullptr;
  Ui::ZKeyDLg *ui;
};

#endif // ZKEYDLG_H
