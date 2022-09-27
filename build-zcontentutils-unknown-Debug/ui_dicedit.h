/********************************************************************************
** Form generated from reading UI file 'dicedit.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
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
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    ZQTableView *displayDicTBv;
    QLabel *label_5;
    ZQTreeView *displayKeyTRv;
    QMenuBar *menubar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *DicEdit)
    {
        if (DicEdit->objectName().isEmpty())
            DicEdit->setObjectName(QString::fromUtf8("DicEdit"));
        DicEdit->resize(1137, 685);
        centralwidget = new QWidget(DicEdit);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(0, 10, 1131, 41));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        KeysNbLBl = new QLabel(frame_2);
        KeysNbLBl->setObjectName(QString::fromUtf8("KeysNbLBl"));
        KeysNbLBl->setGeometry(QRect(1060, 10, 41, 20));
        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(900, 10, 71, 19));
        DicNameLBl = new ZQLabel(frame_2);
        DicNameLBl->setObjectName(QString::fromUtf8("DicNameLBl"));
        DicNameLBl->setGeometry(QRect(12, 10, 491, 19));
        FieldsNbLBl = new QLabel(frame_2);
        FieldsNbLBl->setObjectName(QString::fromUtf8("FieldsNbLBl"));
        FieldsNbLBl->setGeometry(QRect(950, 10, 51, 19));
        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(1010, 10, 51, 19));
        label_6 = new QLabel(frame_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(540, 10, 71, 19));
        VersionLBl = new ZQLabel(frame_2);
        VersionLBl->setObjectName(QString::fromUtf8("VersionLBl"));
        VersionLBl->setGeometry(QRect(610, 10, 111, 19));
        ActiveCBx = new QComboBox(frame_2);
        ActiveCBx->setObjectName(QString::fromUtf8("ActiveCBx"));
        ActiveCBx->setGeometry(QRect(740, 10, 121, 23));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 70, 1101, 561));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_2);

        displayDicTBv = new ZQTableView(layoutWidget);
        displayDicTBv->setObjectName(QString::fromUtf8("displayDicTBv"));
        displayDicTBv->setAcceptDrops(true);
        displayDicTBv->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout->addWidget(displayDicTBv);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_5);

        displayKeyTRv = new ZQTreeView(layoutWidget);
        displayKeyTRv->setObjectName(QString::fromUtf8("displayKeyTRv"));
        displayKeyTRv->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout->addWidget(displayKeyTRv);

        DicEdit->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DicEdit);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1137, 20));
        DicEdit->setMenuBar(menubar);
        statusBar = new QStatusBar(DicEdit);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
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
        label_2->setText(QCoreApplication::translate("DicEdit", "Meta dictionary", nullptr));
        label_5->setText(QCoreApplication::translate("DicEdit", "Keys", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DicEdit: public Ui_DicEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DICEDIT_H
