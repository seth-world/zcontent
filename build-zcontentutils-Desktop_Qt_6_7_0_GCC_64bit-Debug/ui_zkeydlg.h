/********************************************************************************
** Form generated from reading UI file 'zkeydlg.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
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
    QCheckBox *ForceCHk;

    void setupUi(QDialog *ZKeyDLg)
    {
        if (ZKeyDLg->objectName().isEmpty())
            ZKeyDLg->setObjectName("ZKeyDLg");
        ZKeyDLg->resize(510, 139);
        KeyNameLEd = new QLineEdit(ZKeyDLg);
        KeyNameLEd->setObjectName("KeyNameLEd");
        KeyNameLEd->setGeometry(QRect(120, 10, 211, 23));
        label = new QLabel(ZKeyDLg);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 20, 91, 16));
        DuplicatesCHk = new QCheckBox(ZKeyDLg);
        DuplicatesCHk->setObjectName("DuplicatesCHk");
        DuplicatesCHk->setGeometry(QRect(120, 50, 79, 21));
        label_2 = new QLabel(ZKeyDLg);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 50, 111, 16));
        OKBTn = new QPushButton(ZKeyDLg);
        OKBTn->setObjectName("OKBTn");
        OKBTn->setGeometry(QRect(420, 110, 80, 23));
        DisguardBTn = new QPushButton(ZKeyDLg);
        DisguardBTn->setObjectName("DisguardBTn");
        DisguardBTn->setGeometry(QRect(310, 110, 80, 23));
        TooltipLEd = new QLineEdit(ZKeyDLg);
        TooltipLEd->setObjectName("TooltipLEd");
        TooltipLEd->setGeometry(QRect(120, 80, 381, 23));
        label_3 = new QLabel(ZKeyDLg);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 80, 91, 16));
        label_4 = new QLabel(ZKeyDLg);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(150, 50, 141, 16));
        KeySizeLEd = new QLineEdit(ZKeyDLg);
        KeySizeLEd->setObjectName("KeySizeLEd");
        KeySizeLEd->setGeometry(QRect(250, 50, 111, 23));
        ForceCHk = new QCheckBox(ZKeyDLg);
        ForceCHk->setObjectName("ForceCHk");
        ForceCHk->setGeometry(QRect(380, 50, 111, 20));
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
        label_4->setText(QCoreApplication::translate("ZKeyDLg", "Guessed size", nullptr));
        ForceCHk->setText(QCoreApplication::translate("ZKeyDLg", "Force size", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZKeyDLg: public Ui_ZKeyDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZKEYDLG_H
