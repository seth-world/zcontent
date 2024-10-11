#ifndef ZDOMAINDLG_H
#define ZDOMAINDLG_H

#include <QObject>
#include <QDialog>

#include <ztoolset/uristring.h>
#include <ztoolset/zaierrors.h>



class ZQTableView;
class QLineEdit;
class QLabel;
class QHBoxLayout;
class QFrame;
class QCheckBox;
class QComboBox;
class QRadioButton;
class QStandardItem;

namespace zbs {

class ZDomainPath;

class ZDomainDLg : public QDialog
{
  Q_OBJECT
public:


  explicit ZDomainDLg(QWidget* pParent) ;
  ~ZDomainDLg() override ;

  void initLayout();
  void setupCreate(ZDomainPath* pFatherDomainPath,ZDomainPath* pDomainRoot) ;
  void setupChange(ZDomainPath* pDomainPath,ZDomainPath* pDomainRoot);

  ZQTableView* InferiorsTBv=nullptr;

  enum Mode_type {
      MT_Nothing = 0 ,
      MT_Create = 1,
      MT_Change = 2
  };

  ZDomainPath* getDomainPath() { return DomainPathRef; }

private slots:
  void AcceptClicked();
  void RejectClicked();
  void PathSearchClicked();

  void DomainNameChanged(const QString & pNewName);

  public: bool NameHasChanged=false;

private:
  void resizeEvent(QResizeEvent* pEvent) override;

  Mode_type Mode = MT_Nothing;
  ZDomainPath* DomainPathRef=nullptr;


  bool _FResizeInitial=true;

  QLabel*         FatherNameLBl=nullptr;
  QLabel*         FatherHierarchyLBl=nullptr;

  QLineEdit*      NameLEd=nullptr;
  QLineEdit*      ToolTipLEd=nullptr;
  QLineEdit*      ContentLEd=nullptr;

  QWidget*        TypeWDg=nullptr;

  QRadioButton*   RelativeRBn=nullptr;
  QRadioButton*   AbsoluteRBn=nullptr;

  QComboBox*      ObjectTypeCBx=nullptr;

  QRadioButton*   RemoteRBn=nullptr;
  QRadioButton*   LocalRBn=nullptr;

  QPushButton* PathSearchBTn=nullptr;

  QPushButton* AcceptBTn=nullptr;
  QPushButton* RejectBTn=nullptr;

  ZaiErrors     ErrorLog;


};

} // namespace zbs

QList<QStandardItem*> createZDomainSingleRow(ZDomainPath* pDomain, ZaiErrors *pErrorLog);

#endif // ZDOMAINDLG_H
