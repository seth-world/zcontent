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
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZFieldDLg
{
public:
    QLabel *fieldlabel;
    QLineEdit *ZTypeLEd;
    QLabel *label;
    QLineEdit *ZTypeStrLEd;
    QLineEdit *FieldNameLEd;
    QLabel *label_6;
    QLineEdit *HashcodeLEd;
    QPushButton *DisguardBTn;
    QPushButton *AcceptBTn;
    QPushButton *ComputeBTn;
    QPushButton *ValidateHashBTn;
    QWidget *ZTypeListWDg;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLineEdit *HeaderSizeLEd;
    QLabel *label_2;
    QLineEdit *NaturalLEd;
    QLabel *label_4;
    QLineEdit *UniversalLEd;
    QLabel *label_5;
    QLineEdit *CapacityLEd;
    QLabel *HashIconLBl;
    QCheckBox *KeyEligibleCHk;
    QLabel *label_7;

    void setupUi(QDialog *ZFieldDLg)
    {
        if (ZFieldDLg->objectName().isEmpty())
            ZFieldDLg->setObjectName(QString::fromUtf8("ZFieldDLg"));
        ZFieldDLg->resize(431, 373);
        fieldlabel = new QLabel(ZFieldDLg);
        fieldlabel->setObjectName(QString::fromUtf8("fieldlabel"));
        fieldlabel->setGeometry(QRect(10, 10, 81, 16));
        ZTypeLEd = new QLineEdit(ZFieldDLg);
        ZTypeLEd->setObjectName(QString::fromUtf8("ZTypeLEd"));
        ZTypeLEd->setGeometry(QRect(80, 70, 101, 23));
        ZTypeLEd->setReadOnly(true);
        label = new QLabel(ZFieldDLg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(140, 40, 101, 16));
        ZTypeStrLEd = new QLineEdit(ZFieldDLg);
        ZTypeStrLEd->setObjectName(QString::fromUtf8("ZTypeStrLEd"));
        ZTypeStrLEd->setGeometry(QRect(190, 70, 231, 23));
        ZTypeStrLEd->setReadOnly(true);
        FieldNameLEd = new QLineEdit(ZFieldDLg);
        FieldNameLEd->setObjectName(QString::fromUtf8("FieldNameLEd"));
        FieldNameLEd->setGeometry(QRect(110, 10, 171, 23));
        label_6 = new QLabel(ZFieldDLg);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 290, 91, 16));
        HashcodeLEd = new QLineEdit(ZFieldDLg);
        HashcodeLEd->setObjectName(QString::fromUtf8("HashcodeLEd"));
        HashcodeLEd->setGeometry(QRect(90, 290, 321, 23));
        HashcodeLEd->setReadOnly(true);
        DisguardBTn = new QPushButton(ZFieldDLg);
        DisguardBTn->setObjectName(QString::fromUtf8("DisguardBTn"));
        DisguardBTn->setGeometry(QRect(340, 340, 71, 23));
        AcceptBTn = new QPushButton(ZFieldDLg);
        AcceptBTn->setObjectName(QString::fromUtf8("AcceptBTn"));
        AcceptBTn->setGeometry(QRect(260, 340, 71, 23));
        ComputeBTn = new QPushButton(ZFieldDLg);
        ComputeBTn->setObjectName(QString::fromUtf8("ComputeBTn"));
        ComputeBTn->setGeometry(QRect(160, 260, 131, 23));
        ValidateHashBTn = new QPushButton(ZFieldDLg);
        ValidateHashBTn->setObjectName(QString::fromUtf8("ValidateHashBTn"));
        ValidateHashBTn->setGeometry(QRect(10, 260, 131, 23));
        ZTypeListWDg = new QWidget(ZFieldDLg);
        ZTypeListWDg->setObjectName(QString::fromUtf8("ZTypeListWDg"));
        ZTypeListWDg->setGeometry(QRect(10, 60, 61, 41));
        layoutWidget = new QWidget(ZFieldDLg);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 100, 201, 121));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        HeaderSizeLEd = new QLineEdit(layoutWidget);
        HeaderSizeLEd->setObjectName(QString::fromUtf8("HeaderSizeLEd"));
        HeaderSizeLEd->setReadOnly(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, HeaderSizeLEd);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        NaturalLEd = new QLineEdit(layoutWidget);
        NaturalLEd->setObjectName(QString::fromUtf8("NaturalLEd"));

        formLayout->setWidget(1, QFormLayout::FieldRole, NaturalLEd);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_4);

        UniversalLEd = new QLineEdit(layoutWidget);
        UniversalLEd->setObjectName(QString::fromUtf8("UniversalLEd"));

        formLayout->setWidget(2, QFormLayout::FieldRole, UniversalLEd);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_5);

        CapacityLEd = new QLineEdit(layoutWidget);
        CapacityLEd->setObjectName(QString::fromUtf8("CapacityLEd"));

        formLayout->setWidget(3, QFormLayout::FieldRole, CapacityLEd);

        HashIconLBl = new QLabel(ZFieldDLg);
        HashIconLBl->setObjectName(QString::fromUtf8("HashIconLBl"));
        HashIconLBl->setGeometry(QRect(370, 250, 32, 31));
        KeyEligibleCHk = new QCheckBox(ZFieldDLg);
        KeyEligibleCHk->setObjectName(QString::fromUtf8("KeyEligibleCHk"));
        KeyEligibleCHk->setEnabled(true);
        KeyEligibleCHk->setGeometry(QRect(90, 230, 16, 16));
        label_7 = new QLabel(ZFieldDLg);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 230, 91, 16));
        QWidget::setTabOrder(FieldNameLEd, UniversalLEd);
        QWidget::setTabOrder(UniversalLEd, NaturalLEd);
        QWidget::setTabOrder(NaturalLEd, CapacityLEd);
        QWidget::setTabOrder(CapacityLEd, KeyEligibleCHk);
        QWidget::setTabOrder(KeyEligibleCHk, ComputeBTn);
        QWidget::setTabOrder(ComputeBTn, AcceptBTn);
        QWidget::setTabOrder(AcceptBTn, DisguardBTn);
        QWidget::setTabOrder(DisguardBTn, ValidateHashBTn);
        QWidget::setTabOrder(ValidateHashBTn, ZTypeStrLEd);
        QWidget::setTabOrder(ZTypeStrLEd, HashcodeLEd);
        QWidget::setTabOrder(HashcodeLEd, HeaderSizeLEd);
        QWidget::setTabOrder(HeaderSizeLEd, ZTypeLEd);

        retranslateUi(ZFieldDLg);

        QMetaObject::connectSlotsByName(ZFieldDLg);
    } // setupUi

    void retranslateUi(QDialog *ZFieldDLg)
    {
        ZFieldDLg->setWindowTitle(QCoreApplication::translate("ZFieldDLg", "Dialog", nullptr));
        fieldlabel->setText(QCoreApplication::translate("ZFieldDLg", "Field name", nullptr));
        label->setText(QCoreApplication::translate("ZFieldDLg", "Data type", nullptr));
        label_6->setText(QCoreApplication::translate("ZFieldDLg", "Hashcode", nullptr));
        DisguardBTn->setText(QCoreApplication::translate("ZFieldDLg", "Disguard", nullptr));
        AcceptBTn->setText(QCoreApplication::translate("ZFieldDLg", "Accept", nullptr));
        ComputeBTn->setText(QCoreApplication::translate("ZFieldDLg", "Compute hashcode", nullptr));
        ValidateHashBTn->setText(QCoreApplication::translate("ZFieldDLg", "Validate hashcode", nullptr));
        label_3->setText(QCoreApplication::translate("ZFieldDLg", "Header size", nullptr));
        label_2->setText(QCoreApplication::translate("ZFieldDLg", "Natural size", nullptr));
        label_4->setText(QCoreApplication::translate("ZFieldDLg", "Universal size", nullptr));
        label_5->setText(QCoreApplication::translate("ZFieldDLg", "Capacity", nullptr));
        HashIconLBl->setText(QCoreApplication::translate("ZFieldDLg", "dteea", nullptr));
        KeyEligibleCHk->setText(QString());
        label_7->setText(QCoreApplication::translate("ZFieldDLg", "Key eligible", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZFieldDLg: public Ui_ZFieldDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZFIELDDLG_H
