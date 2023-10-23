/********************************************************************************
** Form generated from reading UI file 'zcellformatdlg.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZCELLFORMATDLG_H
#define UI_ZCELLFORMATDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_ZCellFormatDLg
{
public:
    QGroupBox *NumericGBx;
    QRadioButton *NumStdRBn;
    QRadioButton *NumHexaRBn;
    QGroupBox *ApplyGBx;
    QLabel *ApplianceLBl;
    QGroupBox *ResourceGBx;
    QCheckBox *ResSymbolCKb;
    QCheckBox *ResStdNumCKb;
    QPushButton *OKBTn;
    QGroupBox *DateGBx;
    QRadioButton *DMYRBn;
    QRadioButton *MDYRBn;
    QRadioButton *DMYHMSRBn;
    QRadioButton *MDYHMSRBn;
    QRadioButton *UTCRBn;
    QRadioButton *LocaleRBn;

    void setupUi(QDialog *ZCellFormatDLg)
    {
        if (ZCellFormatDLg->objectName().isEmpty())
            ZCellFormatDLg->setObjectName("ZCellFormatDLg");
        ZCellFormatDLg->resize(374, 468);
        NumericGBx = new QGroupBox(ZCellFormatDLg);
        NumericGBx->setObjectName("NumericGBx");
        NumericGBx->setGeometry(QRect(10, 90, 351, 81));
        NumStdRBn = new QRadioButton(NumericGBx);
        NumStdRBn->setObjectName("NumStdRBn");
        NumStdRBn->setGeometry(QRect(10, 30, 311, 21));
        NumStdRBn->setChecked(true);
        NumHexaRBn = new QRadioButton(NumericGBx);
        NumHexaRBn->setObjectName("NumHexaRBn");
        NumHexaRBn->setGeometry(QRect(10, 50, 251, 21));
        ApplyGBx = new QGroupBox(ZCellFormatDLg);
        ApplyGBx->setObjectName("ApplyGBx");
        ApplyGBx->setGeometry(QRect(10, 0, 361, 61));
        ApplianceLBl = new QLabel(ApplyGBx);
        ApplianceLBl->setObjectName("ApplianceLBl");
        ApplianceLBl->setGeometry(QRect(0, 30, 351, 20));
        ResourceGBx = new QGroupBox(ZCellFormatDLg);
        ResourceGBx->setObjectName("ResourceGBx");
        ResourceGBx->setGeometry(QRect(10, 340, 361, 81));
        ResSymbolCKb = new QCheckBox(ResourceGBx);
        ResSymbolCKb->setObjectName("ResSymbolCKb");
        ResSymbolCKb->setGeometry(QRect(10, 30, 341, 21));
        ResStdNumCKb = new QCheckBox(ResourceGBx);
        ResStdNumCKb->setObjectName("ResStdNumCKb");
        ResStdNumCKb->setGeometry(QRect(10, 50, 341, 21));
        OKBTn = new QPushButton(ZCellFormatDLg);
        OKBTn->setObjectName("OKBTn");
        OKBTn->setGeometry(QRect(260, 430, 80, 23));
        DateGBx = new QGroupBox(ZCellFormatDLg);
        DateGBx->setObjectName("DateGBx");
        DateGBx->setGeometry(QRect(0, 170, 361, 161));
        DMYRBn = new QRadioButton(DateGBx);
        DMYRBn->setObjectName("DMYRBn");
        DMYRBn->setGeometry(QRect(10, 30, 261, 21));
        DMYRBn->setChecked(true);
        MDYRBn = new QRadioButton(DateGBx);
        MDYRBn->setObjectName("MDYRBn");
        MDYRBn->setGeometry(QRect(10, 50, 221, 21));
        DMYHMSRBn = new QRadioButton(DateGBx);
        DMYHMSRBn->setObjectName("DMYHMSRBn");
        DMYHMSRBn->setGeometry(QRect(10, 70, 221, 21));
        MDYHMSRBn = new QRadioButton(DateGBx);
        MDYHMSRBn->setObjectName("MDYHMSRBn");
        MDYHMSRBn->setGeometry(QRect(10, 90, 221, 21));
        UTCRBn = new QRadioButton(DateGBx);
        UTCRBn->setObjectName("UTCRBn");
        UTCRBn->setGeometry(QRect(10, 130, 221, 21));
        LocaleRBn = new QRadioButton(DateGBx);
        LocaleRBn->setObjectName("LocaleRBn");
        LocaleRBn->setGeometry(QRect(10, 110, 221, 21));

        retranslateUi(ZCellFormatDLg);

        QMetaObject::connectSlotsByName(ZCellFormatDLg);
    } // setupUi

    void retranslateUi(QDialog *ZCellFormatDLg)
    {
        ZCellFormatDLg->setWindowTitle(QCoreApplication::translate("ZCellFormatDLg", "Dialog", nullptr));
        NumericGBx->setTitle(QCoreApplication::translate("ZCellFormatDLg", "Numeric", nullptr));
        NumStdRBn->setText(QCoreApplication::translate("ZCellFormatDLg", "standard numeric representation (default)", nullptr));
        NumHexaRBn->setText(QCoreApplication::translate("ZCellFormatDLg", "Hexadecimal", nullptr));
        ApplyGBx->setTitle(QString());
        ApplianceLBl->setText(QCoreApplication::translate("ZCellFormatDLg", "TextLabel", nullptr));
        ResourceGBx->setTitle(QCoreApplication::translate("ZCellFormatDLg", "Resource", nullptr));
        ResSymbolCKb->setText(QCoreApplication::translate("ZCellFormatDLg", "Show ZEntity symbol (default is numeric value)", nullptr));
        ResStdNumCKb->setText(QCoreApplication::translate("ZCellFormatDLg", "Standard numeric representation (default hexa)", nullptr));
        OKBTn->setText(QCoreApplication::translate("ZCellFormatDLg", "Done", nullptr));
        DateGBx->setTitle(QCoreApplication::translate("ZCellFormatDLg", "Date", nullptr));
        DMYRBn->setText(QCoreApplication::translate("ZCellFormatDLg", "day/month/year (default)", nullptr));
        MDYRBn->setText(QCoreApplication::translate("ZCellFormatDLg", "month/day/year ", nullptr));
        DMYHMSRBn->setText(QCoreApplication::translate("ZCellFormatDLg", "day/month/year-hh:mm:ss", nullptr));
        MDYHMSRBn->setText(QCoreApplication::translate("ZCellFormatDLg", "month/day/year -hh:mm:ss", nullptr));
        UTCRBn->setText(QCoreApplication::translate("ZCellFormatDLg", "UTC", nullptr));
        LocaleRBn->setText(QCoreApplication::translate("ZCellFormatDLg", "locale date format", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZCellFormatDLg: public Ui_ZCellFormatDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZCELLFORMATDLG_H
