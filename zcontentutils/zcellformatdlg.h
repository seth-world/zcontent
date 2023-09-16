#ifndef ZCELLFOMATDLG_H
#define ZCELLFOMATDLG_H

#include <QDialog>

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

  int get();
  void setAppliance(const char* pAppliance);

private:
  Ui::ZCellFormatDLg *ui;
};

#endif // ZCELLFOMATDLG_H
