#ifndef ZKEYLISTDLG_H
#define ZKEYLISTDLG_H

#include <QObject>
#include <QDialog>
#include <qcheckbox.h>
#include <zqt/zqtwidget/zqtreeview.h>
#include <zqt/zqtwidget/zqtwidgettools.h>
#include <ztoolset/ztypetype.h>
#include <zindexedfile/zdictionaryfile.h>

#include <filegeneratedlg.h>

class ZKeyListDLg : public QDialog
{
  Q_OBJECT
public:
  explicit ZKeyListDLg(QWidget *pParent=nullptr) ;
  ~ZKeyListDLg() override ;

  void initLayout();

  void setDic(const ZMFDictionary* pDic) {
    ZMFDic = pDic;
  }
  ZStatus displayKeyDictionaries(const ZMFDictionary *pDic);
  int searchForFieldName(const utf8VaryingString& pName);

  void clearKeyTRv();

  ZQTreeView* keyTRv=nullptr;

  KeyData getKeyData() { return KeyDataRef; }

private slots:
  void AcceptClicked();
  void RejectClicked();

private:
  void resizeEvent(QResizeEvent* pEvent) override;

  const ZMFDictionary* ZMFDic=nullptr;

  bool _FResizeInitial=true;

  KeyData KeyDataRef;

  QPushButton* AcceptBTn=nullptr;
  QPushButton* RejectBTn=nullptr;

};


QList<QStandardItem *>
createKeyDicRow_KD(const KeyData& pKHR);

#endif // ZKEYLISTDLG_H
