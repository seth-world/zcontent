#ifndef ZOPERANDDLG_H
#define ZOPERANDDLG_H

#include <QDialog>

class ZQueryWizardMWd;
class QLabel;
class QComboBox;
class QLineEdit;
class QPushButton;

class ZOperandDLg : public QDialog
{
public:
    ZOperandDLg( ZQueryWizardMWd* pParent);

    void initLayout();
private:
    ZQueryWizardMWd* QueryWizard=nullptr;

    QLabel*     EntityLBl=nullptr;

    QComboBox* FieldsCBx=nullptr;
    QComboBox* SymbolCBx=nullptr;

    QLineEdit* LiteralLEd=nullptr;

    QComboBox* ModifierCBx=nullptr;

    QPushButton* OKBTn=nullptr;
    QPushButton* CancelBTn=nullptr;
};

#endif // ZOPERANDDLG_H
