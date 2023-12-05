#ifndef ZCELLFORMATDLG_H
#define ZCELLFORMATDLG_H

#include <QDialog>
#include <zcontentcommon/zcontentconstants.h>

using namespace zbs;

namespace Ui {
class ZCellFormatDLg;
}

class ZCellFormatDLg : public QDialog
{
  Q_OBJECT

public:
  explicit ZCellFormatDLg(QWidget *parent = nullptr);
  ~ZCellFormatDLg();
  void setup(int pCellFormat);

  ZCFMT_Type get();
  void setAppliance(const char* pAppliance);

private:
  Ui::ZCellFormatDLg *ui;
};

#endif // ZCELLFORMATDLG_H
