#ifndef ZFIELDDLG_H
#define ZFIELDDLG_H

#include <QDialog>
#include <QPushButton>

#include <qwindow.h>
#include <zqt/zqtwidget/zqtreeview.h>

#include <zindexedfile/zfielddescription.h>


#define __NEW_FIELDNAME__ "new field"
#define __NEW_KEYNAME__ "new key"

namespace Ui {
class ZFieldDLg;
}


class ZTypeDLg;
class ZTypeListButton;

class ZFieldDLg : public QDialog
{
  Q_OBJECT

public:
  explicit ZFieldDLg(QWidget *parent = nullptr);
  ~ZFieldDLg() override;

  /* updates dialog content with pField */
  void setup(ZFieldDescription &pField, bool pRawFields=false);
  void setCreate( );

  ZFieldDescription getFieldDescription();

  /* updates Field from dialog content */
  void refresh();

  bool HashCodeCompare();

  bool controlField();

  void setComment(const utf8VaryingString& pComment);

  ZFieldDescription   Field;
//  QDialog*            ZTypeDLg=nullptr;
  ZTypeDLg*           ZTypeTRv=nullptr;

  ZTypeListButton*    TypeListBtn=nullptr;

  QPixmap HashOKPXm;
  QPixmap HashWrongPXm;

public slots:
//  void ZTypeFocusIn(QFocusEvent* pEvent);
  void ZTypeListClicked();
  void AcceptClicked();
  void DisguardClicked();
  void ComputeClicked();
  void ArrayClicked();

  int exec() override;

private:
  bool RawField=false;
  Ui::ZFieldDLg *ui;
};

#endif // ZFIELDDLG_H
