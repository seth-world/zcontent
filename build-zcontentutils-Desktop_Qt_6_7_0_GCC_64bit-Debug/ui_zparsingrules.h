/********************************************************************************
** Form generated from reading UI file 'zparsingrules.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZPARSINGRULES_H
#define UI_ZPARSINGRULES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZParsingRules
{
public:
    QWidget *centralwidget;
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
    QWidget *ZTypeListWDg;
    QLineEdit *ZTypeStrLEd;
    QLineEdit *ZTypeLEd;
    QLineEdit *ZTypeLEd_2;
    QLabel *label;
    QPushButton *OKBTn;
    QPushButton *QuitBTn;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ZParsingRules)
    {
        if (ZParsingRules->objectName().isEmpty())
            ZParsingRules->setObjectName("ZParsingRules");
        ZParsingRules->resize(511, 340);
        centralwidget = new QWidget(ZParsingRules);
        centralwidget->setObjectName("centralwidget");
        SizeFRm = new QFrame(centralwidget);
        SizeFRm->setObjectName("SizeFRm");
        SizeFRm->setGeometry(QRect(30, 130, 461, 121));
        SizeFRm->setFrameShape(QFrame::StyledPanel);
        SizeFRm->setFrameShadow(QFrame::Raised);
        label_3 = new QLabel(SizeFRm);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 20, 141, 23));
        HeaderSizeLEd = new QLineEdit(SizeFRm);
        HeaderSizeLEd->setObjectName("HeaderSizeLEd");
        HeaderSizeLEd->setGeometry(QRect(118, 20, 91, 23));
        HeaderSizeLEd->setReadOnly(true);
        label_2 = new QLabel(SizeFRm);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(230, 20, 81, 23));
        NaturalLEd = new QLineEdit(SizeFRm);
        NaturalLEd->setObjectName("NaturalLEd");
        NaturalLEd->setGeometry(QRect(348, 20, 101, 23));
        label_4 = new QLabel(SizeFRm);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(230, 50, 121, 23));
        UniversalLEd = new QLineEdit(SizeFRm);
        UniversalLEd->setObjectName("UniversalLEd");
        UniversalLEd->setGeometry(QRect(348, 50, 101, 23));
        label_5 = new QLabel(SizeFRm);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 50, 121, 23));
        CapacityLEd = new QLineEdit(SizeFRm);
        CapacityLEd->setObjectName("CapacityLEd");
        CapacityLEd->setGeometry(QRect(120, 50, 91, 23));
        KeyEligibleCHk = new QCheckBox(SizeFRm);
        KeyEligibleCHk->setObjectName("KeyEligibleCHk");
        KeyEligibleCHk->setEnabled(true);
        KeyEligibleCHk->setGeometry(QRect(90, 90, 16, 16));
        label_7 = new QLabel(SizeFRm);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 90, 91, 16));
        CreateArrayBTn = new QPushButton(SizeFRm);
        CreateArrayBTn->setObjectName("CreateArrayBTn");
        CreateArrayBTn->setGeometry(QRect(190, 90, 181, 23));
        ZTypeListWDg = new QWidget(centralwidget);
        ZTypeListWDg->setObjectName("ZTypeListWDg");
        ZTypeListWDg->setGeometry(QRect(20, 70, 61, 41));
        ZTypeStrLEd = new QLineEdit(centralwidget);
        ZTypeStrLEd->setObjectName("ZTypeStrLEd");
        ZTypeStrLEd->setGeometry(QRect(200, 80, 271, 23));
        ZTypeStrLEd->setReadOnly(true);
        ZTypeLEd = new QLineEdit(centralwidget);
        ZTypeLEd->setObjectName("ZTypeLEd");
        ZTypeLEd->setGeometry(QRect(90, 80, 101, 23));
        ZTypeLEd->setReadOnly(true);
        ZTypeLEd_2 = new QLineEdit(centralwidget);
        ZTypeLEd_2->setObjectName("ZTypeLEd_2");
        ZTypeLEd_2->setGeometry(QRect(150, 30, 181, 23));
        ZTypeLEd_2->setReadOnly(false);
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 30, 131, 16));
        OKBTn = new QPushButton(centralwidget);
        OKBTn->setObjectName("OKBTn");
        OKBTn->setGeometry(QRect(410, 260, 80, 23));
        QuitBTn = new QPushButton(centralwidget);
        QuitBTn->setObjectName("QuitBTn");
        QuitBTn->setGeometry(QRect(310, 260, 80, 23));
        ZParsingRules->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ZParsingRules);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 511, 20));
        ZParsingRules->setMenuBar(menubar);
        statusbar = new QStatusBar(ZParsingRules);
        statusbar->setObjectName("statusbar");
        ZParsingRules->setStatusBar(statusbar);

        retranslateUi(ZParsingRules);

        QMetaObject::connectSlotsByName(ZParsingRules);
    } // setupUi

    void retranslateUi(QMainWindow *ZParsingRules)
    {
        ZParsingRules->setWindowTitle(QCoreApplication::translate("ZParsingRules", "MainWindow", nullptr));
        label_3->setText(QCoreApplication::translate("ZParsingRules", "Header size", nullptr));
        label_2->setText(QCoreApplication::translate("ZParsingRules", "Natural size", nullptr));
        label_4->setText(QCoreApplication::translate("ZParsingRules", "Universal size", nullptr));
        label_5->setText(QCoreApplication::translate("ZParsingRules", "Capacity", nullptr));
        KeyEligibleCHk->setText(QString());
        label_7->setText(QCoreApplication::translate("ZParsingRules", "Key eligible", nullptr));
        CreateArrayBTn->setText(QCoreApplication::translate("ZParsingRules", "Create array from capacity", nullptr));
        label->setText(QCoreApplication::translate("ZParsingRules", "Syntax to parse", nullptr));
        OKBTn->setText(QCoreApplication::translate("ZParsingRules", "Validate", nullptr));
        QuitBTn->setText(QCoreApplication::translate("ZParsingRules", "Give up", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZParsingRules: public Ui_ZParsingRules {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZPARSINGRULES_H
