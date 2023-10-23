#ifndef ZKEYDLG_H
#define ZKEYDLG_H

#define __NEW_FIELDNAME__ "new field"
#define __NEW_KEYNAME__ "new key"

#include <QDialog>
#include <qnamespace.h>

class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;

//#include <zcontent/zindexedfile/zkeydictionary.h>
#include <zcontent/zcontentutils/zdisplayedfield.h>
/*
namespace Ui {
class ZKeyDLg;
}
*/
class QStatusBar;

class ZKeyDLg : public QDialog
{
  Q_OBJECT

public:
//  explicit ZKeyDLg(ZMFDictionary *pMasterDic,QWidget *parent = nullptr);
  explicit ZKeyDLg(QWidget *parent = nullptr);
  ~ZKeyDLg();

  void initLayout();

  void set(ZKeyHeaderRow* pKeyHeaderRow);
  void setCreate();
  ZKeyHeaderRow get()
  {
//      refresh();
      return KHR;
  }

  /* updates internal ZKeyHeaderRow object with dialog fields data  */
  void refresh();

public slots:
  void OKBTnClicked();
  void CancelBTnClicked();
  void ForcedCHkStateChanged(Qt::CheckState pState);
private:
  void resizeEvent(QResizeEvent* pEvent) override;
  bool _FResizeInitial=true;


  /* NB: usage of dictionary is deprecated */
//  KeyDic_Pack* KeyDic=nullptr;
//  ZMFDictionary*  MasterDic=nullptr;
//  ZKeyDictionary* KeyDic=nullptr;
    QWidget* MainWidget=nullptr;

    QLineEdit *KeyNameLEd=nullptr;
    QComboBox *DuplicatesCBx=nullptr;
    QCheckBox *ForcedCHk=nullptr;
    QPushButton *OKBTn=nullptr;
    QPushButton *DisguardBTn=nullptr;
    QLineEdit *TooltipLEd=nullptr;
    QLineEdit *KeySizeLEd=nullptr;

  ZKeyHeaderRow   KHR;
 // Ui::ZKeyDLg *ui;
};

#endif // ZKEYDLG_H
