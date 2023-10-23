/********************************************************************************
** Form generated from reading UI file 'dicedit.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DICEDIT_H
#define UI_DICEDIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "zqlabel.h"
#include "zqtableview.h"
#include "zqtreeview.h"

QT_BEGIN_NAMESPACE

class Ui_DicEdit
{
public:
    QWidget *centralwidget;
    QFrame *frame_2;
    QLabel *KeysNbLBl;
    QLabel *label_4;
    ZQLabel *DicNameLBl;
    QLabel *FieldsNbLBl;
    QLabel *label_3;
    QLabel *label_6;
    ZQLabel *VersionLBl;
    QComboBox *ActiveCBx;
    QLabel *label;
    QLabel *SourceLBl;
    QLabel *SourceURILBl;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    ZQTableView *displayDicTBv;
    QLabel *label_5;
    ZQTreeView *displayKeyTRv;
    QLabel *label_2;
    QMenuBar *menubar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *DicEdit)
    {
        if (DicEdit->objectName().isEmpty())
            DicEdit->setObjectName("DicEdit");
        DicEdit->resize(1137, 685);
        centralwidget = new QWidget(DicEdit);
        centralwidget->setObjectName("centralwidget");
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName("frame_2");
        frame_2->setGeometry(QRect(0, 10, 1131, 51));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        KeysNbLBl = new QLabel(frame_2);
        KeysNbLBl->setObjectName("KeysNbLBl");
        KeysNbLBl->setGeometry(QRect(1060, 30, 41, 20));
        label_4 = new QLabel(frame_2);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(900, 30, 71, 19));
        DicNameLBl = new ZQLabel(frame_2);
        DicNameLBl->setObjectName("DicNameLBl");
        DicNameLBl->setGeometry(QRect(12, 30, 491, 19));
        FieldsNbLBl = new QLabel(frame_2);
        FieldsNbLBl->setObjectName("FieldsNbLBl");
        FieldsNbLBl->setGeometry(QRect(950, 30, 51, 19));
        label_3 = new QLabel(frame_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(1010, 30, 51, 19));
        label_6 = new QLabel(frame_2);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(540, 30, 71, 19));
        VersionLBl = new ZQLabel(frame_2);
        VersionLBl->setObjectName("VersionLBl");
        VersionLBl->setGeometry(QRect(610, 30, 111, 19));
        ActiveCBx = new QComboBox(frame_2);
        ActiveCBx->setObjectName("ActiveCBx");
        ActiveCBx->setGeometry(QRect(740, 30, 121, 23));
        label = new QLabel(frame_2);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 10, 81, 16));
        SourceLBl = new QLabel(frame_2);
        SourceLBl->setObjectName("SourceLBl");
        SourceLBl->setGeometry(QRect(130, 10, 261, 20));
        SourceURILBl = new QLabel(frame_2);
        SourceURILBl->setObjectName("SourceURILBl");
        SourceURILBl->setGeometry(QRect(430, 10, 651, 16));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 70, 1101, 561));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        displayDicTBv = new ZQTableView(layoutWidget);
        displayDicTBv->setObjectName("displayDicTBv");
        displayDicTBv->setAcceptDrops(true);
        displayDicTBv->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout->addWidget(displayDicTBv);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");
        label_5->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_5);

        displayKeyTRv = new ZQTreeView(layoutWidget);
        displayKeyTRv->setObjectName("displayKeyTRv");
        displayKeyTRv->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout->addWidget(displayKeyTRv);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(-10, 70, 1099, 14));
        label_2->setAlignment(Qt::AlignCenter);
        DicEdit->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DicEdit);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1137, 19));
        DicEdit->setMenuBar(menubar);
        statusBar = new QStatusBar(DicEdit);
        statusBar->setObjectName("statusBar");
        DicEdit->setStatusBar(statusBar);

        retranslateUi(DicEdit);

        QMetaObject::connectSlotsByName(DicEdit);
    } // setupUi

    void retranslateUi(QMainWindow *DicEdit)
    {
        DicEdit->setWindowTitle(QCoreApplication::translate("DicEdit", "MainWindow", nullptr));
        KeysNbLBl->setText(QCoreApplication::translate("DicEdit", "###", nullptr));
        label_4->setText(QCoreApplication::translate("DicEdit", "Fields", nullptr));
        DicNameLBl->setText(QCoreApplication::translate("DicEdit", "DicName", nullptr));
        FieldsNbLBl->setText(QCoreApplication::translate("DicEdit", "####", nullptr));
        label_3->setText(QCoreApplication::translate("DicEdit", "Keys", nullptr));
        label_6->setText(QCoreApplication::translate("DicEdit", "Version", nullptr));
        VersionLBl->setText(QCoreApplication::translate("DicEdit", "##.##-##", nullptr));
        label->setText(QCoreApplication::translate("DicEdit", "Source", nullptr));
        SourceLBl->setText(QCoreApplication::translate("DicEdit", "TextLabel", nullptr));
        SourceURILBl->setText(QCoreApplication::translate("DicEdit", "TextLabel", nullptr));
        label_5->setText(QCoreApplication::translate("DicEdit", "Keys", nullptr));
        label_2->setText(QCoreApplication::translate("DicEdit", "Meta dictionary", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DicEdit: public Ui_DicEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DICEDIT_H
