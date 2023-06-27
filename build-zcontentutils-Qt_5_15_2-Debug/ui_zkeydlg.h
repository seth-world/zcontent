/********************************************************************************
** Form generated from reading UI file 'zkeydlg.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZKEYDLG_H
#define UI_ZKEYDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ZKeyDLg
{
public:
    QLineEdit *KeyNameLEd;
    QLabel *label;
    QCheckBox *DuplicatesCHk;
    QLabel *label_2;
    QPushButton *OKBTn;
    QPushButton *DisguardBTn;
    QLineEdit *TooltipLEd;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *KeySizeLEd;

    void setupUi(QDialog *ZKeyDLg)
    {
        if (ZKeyDLg->objectName().isEmpty())
            ZKeyDLg->setObjectName(QString::fromUtf8("ZKeyDLg"));
        ZKeyDLg->resize(510, 139);
        KeyNameLEd = new QLineEdit(ZKeyDLg);
        KeyNameLEd->setObjectName(QString::fromUtf8("KeyNameLEd"));
        KeyNameLEd->setGeometry(QRect(120, 10, 211, 23));
        label = new QLabel(ZKeyDLg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 91, 16));
        DuplicatesCHk = new QCheckBox(ZKeyDLg);
        DuplicatesCHk->setObjectName(QString::fromUtf8("DuplicatesCHk"));
        DuplicatesCHk->setGeometry(QRect(180, 50, 79, 21));
        label_2 = new QLabel(ZKeyDLg);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 50, 141, 16));
        OKBTn = new QPushButton(ZKeyDLg);
        OKBTn->setObjectName(QString::fromUtf8("OKBTn"));
        OKBTn->setGeometry(QRect(420, 110, 80, 23));
        DisguardBTn = new QPushButton(ZKeyDLg);
        DisguardBTn->setObjectName(QString::fromUtf8("DisguardBTn"));
        DisguardBTn->setGeometry(QRect(310, 110, 80, 23));
        TooltipLEd = new QLineEdit(ZKeyDLg);
        TooltipLEd->setObjectName(QString::fromUtf8("TooltipLEd"));
        TooltipLEd->setGeometry(QRect(120, 80, 381, 23));
        label_3 = new QLabel(ZKeyDLg);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 80, 91, 16));
        label_4 = new QLabel(ZKeyDLg);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(220, 50, 141, 16));
        KeySizeLEd = new QLineEdit(ZKeyDLg);
        KeySizeLEd->setObjectName(QString::fromUtf8("KeySizeLEd"));
        KeySizeLEd->setGeometry(QRect(350, 40, 151, 23));
        QWidget::setTabOrder(KeyNameLEd, DuplicatesCHk);
        QWidget::setTabOrder(DuplicatesCHk, TooltipLEd);
        QWidget::setTabOrder(TooltipLEd, OKBTn);
        QWidget::setTabOrder(OKBTn, DisguardBTn);

        retranslateUi(ZKeyDLg);

        QMetaObject::connectSlotsByName(ZKeyDLg);
    } // setupUi

    void retranslateUi(QDialog *ZKeyDLg)
    {
        ZKeyDLg->setWindowTitle(QCoreApplication::translate("ZKeyDLg", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("ZKeyDLg", "Key Name", nullptr));
        DuplicatesCHk->setText(QString());
        label_2->setText(QCoreApplication::translate("ZKeyDLg", "Allow Duplicates", nullptr));
        OKBTn->setText(QCoreApplication::translate("ZKeyDLg", "OK", nullptr));
        DisguardBTn->setText(QCoreApplication::translate("ZKeyDLg", "Discard", nullptr));
        label_3->setText(QCoreApplication::translate("ZKeyDLg", "Tooltip", nullptr));
        label_4->setText(QCoreApplication::translate("ZKeyDLg", "Universal size", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZKeyDLg: public Ui_ZKeyDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZKEYDLG_H
