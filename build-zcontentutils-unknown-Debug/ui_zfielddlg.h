/********************************************************************************
** Form generated from reading UI file 'zfielddlg.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZFIELDDLG_H
#define UI_ZFIELDDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ZFieldDLg
{
public:
    QLabel *fieldlabel;
    QLineEdit *ZTypeLEd;
    QLabel *label_4;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QLineEdit *HeaderSizeLEd;
    QLineEdit *UniversalLEd;
    QLineEdit *NaturalLEd;
    QLineEdit *ZTypeStrLEd;
    QLineEdit *FieldNameLEd;
    QLineEdit *CapacityLEd;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLineEdit *HashcodeLEd;
    QCheckBox *KeyEligibleCHk;
    QPushButton *ListBTn;
    QPushButton *DisguardBTn;
    QPushButton *AcceptBTn;

    void setupUi(QDialog *ZFieldDLg)
    {
        if (ZFieldDLg->objectName().isEmpty())
            ZFieldDLg->setObjectName(QString::fromUtf8("ZFieldDLg"));
        ZFieldDLg->resize(431, 373);
        fieldlabel = new QLabel(ZFieldDLg);
        fieldlabel->setObjectName(QString::fromUtf8("fieldlabel"));
        fieldlabel->setGeometry(QRect(10, 30, 81, 16));
        ZTypeLEd = new QLineEdit(ZFieldDLg);
        ZTypeLEd->setObjectName(QString::fromUtf8("ZTypeLEd"));
        ZTypeLEd->setGeometry(QRect(80, 90, 101, 23));
        ZTypeLEd->setReadOnly(true);
        label_4 = new QLabel(ZFieldDLg);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 210, 121, 16));
        label_3 = new QLabel(ZFieldDLg);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 140, 91, 16));
        label_2 = new QLabel(ZFieldDLg);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 170, 111, 16));
        label = new QLabel(ZFieldDLg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(140, 60, 101, 16));
        HeaderSizeLEd = new QLineEdit(ZFieldDLg);
        HeaderSizeLEd->setObjectName(QString::fromUtf8("HeaderSizeLEd"));
        HeaderSizeLEd->setGeometry(QRect(140, 140, 67, 23));
        HeaderSizeLEd->setReadOnly(true);
        UniversalLEd = new QLineEdit(ZFieldDLg);
        UniversalLEd->setObjectName(QString::fromUtf8("UniversalLEd"));
        UniversalLEd->setGeometry(QRect(140, 200, 67, 23));
        NaturalLEd = new QLineEdit(ZFieldDLg);
        NaturalLEd->setObjectName(QString::fromUtf8("NaturalLEd"));
        NaturalLEd->setGeometry(QRect(140, 170, 66, 23));
        ZTypeStrLEd = new QLineEdit(ZFieldDLg);
        ZTypeStrLEd->setObjectName(QString::fromUtf8("ZTypeStrLEd"));
        ZTypeStrLEd->setGeometry(QRect(190, 90, 231, 23));
        ZTypeStrLEd->setReadOnly(true);
        FieldNameLEd = new QLineEdit(ZFieldDLg);
        FieldNameLEd->setObjectName(QString::fromUtf8("FieldNameLEd"));
        FieldNameLEd->setGeometry(QRect(110, 30, 171, 23));
        CapacityLEd = new QLineEdit(ZFieldDLg);
        CapacityLEd->setObjectName(QString::fromUtf8("CapacityLEd"));
        CapacityLEd->setGeometry(QRect(140, 240, 67, 23));
        label_5 = new QLabel(ZFieldDLg);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 240, 101, 16));
        label_6 = new QLabel(ZFieldDLg);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 280, 91, 16));
        label_7 = new QLabel(ZFieldDLg);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 310, 111, 16));
        HashcodeLEd = new QLineEdit(ZFieldDLg);
        HashcodeLEd->setObjectName(QString::fromUtf8("HashcodeLEd"));
        HashcodeLEd->setGeometry(QRect(140, 280, 271, 23));
        HashcodeLEd->setReadOnly(true);
        KeyEligibleCHk = new QCheckBox(ZFieldDLg);
        KeyEligibleCHk->setObjectName(QString::fromUtf8("KeyEligibleCHk"));
        KeyEligibleCHk->setEnabled(true);
        KeyEligibleCHk->setGeometry(QRect(140, 310, 79, 21));
        ListBTn = new QPushButton(ZFieldDLg);
        ListBTn->setObjectName(QString::fromUtf8("ListBTn"));
        ListBTn->setGeometry(QRect(20, 90, 51, 23));
        DisguardBTn = new QPushButton(ZFieldDLg);
        DisguardBTn->setObjectName(QString::fromUtf8("DisguardBTn"));
        DisguardBTn->setGeometry(QRect(340, 340, 71, 23));
        AcceptBTn = new QPushButton(ZFieldDLg);
        AcceptBTn->setObjectName(QString::fromUtf8("AcceptBTn"));
        AcceptBTn->setGeometry(QRect(260, 340, 71, 23));
        QWidget::setTabOrder(FieldNameLEd, ZTypeLEd);
        QWidget::setTabOrder(ZTypeLEd, ZTypeStrLEd);
        QWidget::setTabOrder(ZTypeStrLEd, HeaderSizeLEd);
        QWidget::setTabOrder(HeaderSizeLEd, NaturalLEd);
        QWidget::setTabOrder(NaturalLEd, UniversalLEd);
        QWidget::setTabOrder(UniversalLEd, CapacityLEd);
        QWidget::setTabOrder(CapacityLEd, HashcodeLEd);
        QWidget::setTabOrder(HashcodeLEd, KeyEligibleCHk);

        retranslateUi(ZFieldDLg);

        QMetaObject::connectSlotsByName(ZFieldDLg);
    } // setupUi

    void retranslateUi(QDialog *ZFieldDLg)
    {
        ZFieldDLg->setWindowTitle(QCoreApplication::translate("ZFieldDLg", "Dialog", nullptr));
        fieldlabel->setText(QCoreApplication::translate("ZFieldDLg", "Field name", nullptr));
        label_4->setText(QCoreApplication::translate("ZFieldDLg", "Universal size", nullptr));
        label_3->setText(QCoreApplication::translate("ZFieldDLg", "Header size", nullptr));
        label_2->setText(QCoreApplication::translate("ZFieldDLg", "Natural size", nullptr));
        label->setText(QCoreApplication::translate("ZFieldDLg", "Data type", nullptr));
        label_5->setText(QCoreApplication::translate("ZFieldDLg", "Capacity", nullptr));
        label_6->setText(QCoreApplication::translate("ZFieldDLg", "Hashcode", nullptr));
        label_7->setText(QCoreApplication::translate("ZFieldDLg", "Key eligible", nullptr));
        KeyEligibleCHk->setText(QString());
        ListBTn->setText(QCoreApplication::translate("ZFieldDLg", "List", nullptr));
        DisguardBTn->setText(QCoreApplication::translate("ZFieldDLg", "Disguard", nullptr));
        AcceptBTn->setText(QCoreApplication::translate("ZFieldDLg", "Accept", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZFieldDLg: public Ui_ZFieldDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZFIELDDLG_H
