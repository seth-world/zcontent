#include "zoperanddlg.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

ZOperandDLg::ZOperandDLg(ZQueryWizardMWd *pParent) : QDialog((QWidget*)pParent)
{
    QueryWizard = pParent;
}

void
ZOperandDLg::initLayout()
{
    QVBoxLayout* MainVBox=new QVBoxLayout;
    setLayout(MainVBox);

    QHBoxLayout* HB0 = new QHBoxLayout;

    MainVBox->addLayout(HB0);

    HB0->addWidget(new QLabel("Entity"));
    EntityLBl = new QLabel("Entity name");
    HB0->addWidget(EntityLBl);
    MainVBox->addItem(HB0);



    QHBoxLayout* HB1 = new QHBoxLayout;
    MainVBox->addItem(HB1);

    QVBoxLayout* VB0 = new QVBoxLayout;
    HB1->addItem(VB0);
    FieldsCBx = new QComboBox(this);
    SymbolCBx = new QComboBox(this);

    LiteralLEd = new QLineEdit(this);

    VB0->addWidget(FieldsCBx);
    VB0->addWidget(SymbolCBx);
    VB0->addWidget(LiteralLEd);


    ModifierCBx = new QComboBox(this);
    HB1->addWidget(ModifierCBx);

    QHBoxLayout* HBButtons=new QHBoxLayout;

    HBButtons->setAlignment(Qt::AlignRight);

    OKBTn = new QPushButton("Ok",this);
    HBButtons->addWidget(OKBTn);
    CancelBTn = new QPushButton("Cancel",this);
    HBButtons->addWidget(CancelBTn);

    VB0->addItem(HBButtons);

    QObject::connect(OKBTn,SIGNAL(clicked(bool)),this,SLOT(accept));
    QObject::connect(CancelBTn,SIGNAL(clicked(bool)),this,SLOT(reject));
}
