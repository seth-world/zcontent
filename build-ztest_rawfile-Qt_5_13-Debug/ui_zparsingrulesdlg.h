/********************************************************************************
** Form generated from reading UI file 'zparsingrulesdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZPARSINGRULESDLG_H
#define UI_ZPARSINGRULESDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZParsingRulesDLg
{
public:
    QLabel *label;
    QLineEdit *ZTypeLEd;
    QFrame *SizeFRm;
    QLabel *label_3;
    QLineEdit *HeaderSizeLEd;
    QLabel *label_2;
    QLineEdit *NaturalLEd;
    QLabel *label_4;
    QLineEdit *UniversalLEd;
    QLabel *label_5;
    QLineEdit *CapacityLEd;
    QCheckBox *KeyEligibleCHk;
    QLabel *label_7;
    QPushButton *CreateArrayBTn;
    QPushButton *AcceptBTn;
    QLineEdit *ZTypeStrLEd;
    QPushButton *DiscardBTn;
    QWidget *ZTypeListWDg;
    QWidget *SyntaxWDg;

    void setupUi(QDialog *ZParsingRulesDLg)
    {
        if (ZParsingRulesDLg->objectName().isEmpty())
            ZParsingRulesDLg->setObjectName(QString::fromUtf8("ZParsingRulesDLg"));
        ZParsingRulesDLg->resize(489, 273);
        label = new QLabel(ZParsingRulesDLg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 161, 16));
        ZTypeLEd = new QLineEdit(ZParsingRulesDLg);
        ZTypeLEd->setObjectName(QString::fromUtf8("ZTypeLEd"));
        ZTypeLEd->setGeometry(QRect(80, 60, 101, 23));
        ZTypeLEd->setReadOnly(true);
        SizeFRm = new QFrame(ZParsingRulesDLg);
        SizeFRm->setObjectName(QString::fromUtf8("SizeFRm"));
        SizeFRm->setGeometry(QRect(10, 110, 461, 121));
        SizeFRm->setFrameShape(QFrame::StyledPanel);
        SizeFRm->setFrameShadow(QFrame::Raised);
        label_3 = new QLabel(SizeFRm);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 20, 141, 23));
        HeaderSizeLEd = new QLineEdit(SizeFRm);
        HeaderSizeLEd->setObjectName(QString::fromUtf8("HeaderSizeLEd"));
        HeaderSizeLEd->setGeometry(QRect(118, 20, 91, 23));
        HeaderSizeLEd->setReadOnly(false);
        label_2 = new QLabel(SizeFRm);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(230, 20, 81, 23));
        NaturalLEd = new QLineEdit(SizeFRm);
        NaturalLEd->setObjectName(QString::fromUtf8("NaturalLEd"));
        NaturalLEd->setGeometry(QRect(348, 20, 101, 23));
        label_4 = new QLabel(SizeFRm);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(230, 50, 121, 23));
        UniversalLEd = new QLineEdit(SizeFRm);
        UniversalLEd->setObjectName(QString::fromUtf8("UniversalLEd"));
        UniversalLEd->setGeometry(QRect(348, 50, 101, 23));
        label_5 = new QLabel(SizeFRm);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 50, 121, 23));
        CapacityLEd = new QLineEdit(SizeFRm);
        CapacityLEd->setObjectName(QString::fromUtf8("CapacityLEd"));
        CapacityLEd->setGeometry(QRect(120, 50, 91, 23));
        KeyEligibleCHk = new QCheckBox(SizeFRm);
        KeyEligibleCHk->setObjectName(QString::fromUtf8("KeyEligibleCHk"));
        KeyEligibleCHk->setEnabled(true);
        KeyEligibleCHk->setGeometry(QRect(90, 90, 16, 16));
        label_7 = new QLabel(SizeFRm);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 90, 91, 16));
        CreateArrayBTn = new QPushButton(SizeFRm);
        CreateArrayBTn->setObjectName(QString::fromUtf8("CreateArrayBTn"));
        CreateArrayBTn->setGeometry(QRect(190, 90, 181, 23));
        AcceptBTn = new QPushButton(ZParsingRulesDLg);
        AcceptBTn->setObjectName(QString::fromUtf8("AcceptBTn"));
        AcceptBTn->setGeometry(QRect(390, 240, 80, 23));
        ZTypeStrLEd = new QLineEdit(ZParsingRulesDLg);
        ZTypeStrLEd->setObjectName(QString::fromUtf8("ZTypeStrLEd"));
        ZTypeStrLEd->setGeometry(QRect(190, 60, 271, 23));
        ZTypeStrLEd->setReadOnly(true);
        DiscardBTn = new QPushButton(ZParsingRulesDLg);
        DiscardBTn->setObjectName(QString::fromUtf8("DiscardBTn"));
        DiscardBTn->setGeometry(QRect(290, 240, 80, 23));
        ZTypeListWDg = new QWidget(ZParsingRulesDLg);
        ZTypeListWDg->setObjectName(QString::fromUtf8("ZTypeListWDg"));
        ZTypeListWDg->setGeometry(QRect(10, 50, 61, 41));
        SyntaxWDg = new QWidget(ZParsingRulesDLg);
        SyntaxWDg->setObjectName(QString::fromUtf8("SyntaxWDg"));
        SyntaxWDg->setGeometry(QRect(180, 10, 291, 31));

        retranslateUi(ZParsingRulesDLg);

        QMetaObject::connectSlotsByName(ZParsingRulesDLg);
    } // setupUi

    void retranslateUi(QDialog *ZParsingRulesDLg)
    {
        ZParsingRulesDLg->setWindowTitle(QCoreApplication::translate("ZParsingRulesDLg", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("ZParsingRulesDLg", "Source syntax ", nullptr));
        label_3->setText(QCoreApplication::translate("ZParsingRulesDLg", "Header size", nullptr));
        label_2->setText(QCoreApplication::translate("ZParsingRulesDLg", "Natural size", nullptr));
        label_4->setText(QCoreApplication::translate("ZParsingRulesDLg", "Universal size", nullptr));
        label_5->setText(QCoreApplication::translate("ZParsingRulesDLg", "Capacity", nullptr));
        KeyEligibleCHk->setText(QString());
        label_7->setText(QCoreApplication::translate("ZParsingRulesDLg", "Key eligible", nullptr));
        CreateArrayBTn->setText(QCoreApplication::translate("ZParsingRulesDLg", "Create array from capacity", nullptr));
        AcceptBTn->setText(QCoreApplication::translate("ZParsingRulesDLg", "Validate", nullptr));
        DiscardBTn->setText(QCoreApplication::translate("ZParsingRulesDLg", "Give up", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZParsingRulesDLg: public Ui_ZParsingRulesDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZPARSINGRULESDLG_H
