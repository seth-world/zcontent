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
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "zqtableview.h"
#include "zqtreeview.h"

QT_BEGIN_NAMESPACE

class Ui_DicEdit
{
public:
    QWidget *centralwidget;
    ZQTableView *displayDicTBv;
    QLabel *label_2;
    QLabel *label_5;
    ZQTreeView *displayKeyTRv;
    QFrame *frame_2;
    QLabel *ClosedLBl;
    QLabel *KeysNbLBl;
    QLabel *label_4;
    QLabel *DicNameLBl;
    QLabel *FieldsNbLBl;
    QLabel *label_3;
    QLabel *label_6;
    QLabel *VersionLBl;
    QMenuBar *menubar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *DicEdit)
    {
        if (DicEdit->objectName().isEmpty())
            DicEdit->setObjectName(QString::fromUtf8("DicEdit"));
        DicEdit->resize(1153, 685);
        centralwidget = new QWidget(DicEdit);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        displayDicTBv = new ZQTableView(centralwidget);
        displayDicTBv->setObjectName(QString::fromUtf8("displayDicTBv"));
        displayDicTBv->setGeometry(QRect(10, 80, 1130, 331));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(displayDicTBv->sizePolicy().hasHeightForWidth());
        displayDicTBv->setSizePolicy(sizePolicy);
        displayDicTBv->setAcceptDrops(true);
        displayDicTBv->setEditTriggers(QAbstractItemView::NoEditTriggers);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(470, 60, 211, 16));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(500, 420, 71, 16));
        displayKeyTRv = new ZQTreeView(centralwidget);
        displayKeyTRv->setObjectName(QString::fromUtf8("displayKeyTRv"));
        displayKeyTRv->setGeometry(QRect(10, 440, 1130, 199));
        sizePolicy.setHeightForWidth(displayKeyTRv->sizePolicy().hasHeightForWidth());
        displayKeyTRv->setSizePolicy(sizePolicy);
        displayKeyTRv->setEditTriggers(QAbstractItemView::NoEditTriggers);
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(0, 10, 1131, 41));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        ClosedLBl = new QLabel(frame_2);
        ClosedLBl->setObjectName(QString::fromUtf8("ClosedLBl"));
        ClosedLBl->setGeometry(QRect(942, 10, 180, 19));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        ClosedLBl->setFont(font);
        KeysNbLBl = new QLabel(frame_2);
        KeysNbLBl->setObjectName(QString::fromUtf8("KeysNbLBl"));
        KeysNbLBl->setGeometry(QRect(880, 10, 41, 20));
        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(720, 10, 71, 19));
        DicNameLBl = new QLabel(frame_2);
        DicNameLBl->setObjectName(QString::fromUtf8("DicNameLBl"));
        DicNameLBl->setGeometry(QRect(12, 10, 491, 19));
        FieldsNbLBl = new QLabel(frame_2);
        FieldsNbLBl->setObjectName(QString::fromUtf8("FieldsNbLBl"));
        FieldsNbLBl->setGeometry(QRect(770, 10, 51, 19));
        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(830, 10, 51, 19));
        label_6 = new QLabel(frame_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(520, 10, 71, 19));
        VersionLBl = new QLabel(frame_2);
        VersionLBl->setObjectName(QString::fromUtf8("VersionLBl"));
        VersionLBl->setGeometry(QRect(580, 10, 111, 19));
        DicEdit->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DicEdit);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1153, 20));
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
        label_2->setText(QCoreApplication::translate("DicEdit", "Meta dictionary", nullptr));
        label_5->setText(QCoreApplication::translate("DicEdit", "Keys", nullptr));
        ClosedLBl->setText(QCoreApplication::translate("DicEdit", "No Open File", nullptr));
        KeysNbLBl->setText(QCoreApplication::translate("DicEdit", "###", nullptr));
        label_4->setText(QCoreApplication::translate("DicEdit", "Fields", nullptr));
        DicNameLBl->setText(QCoreApplication::translate("DicEdit", "DicName", nullptr));
        FieldsNbLBl->setText(QCoreApplication::translate("DicEdit", "####", nullptr));
        label_3->setText(QCoreApplication::translate("DicEdit", "Keys", nullptr));
        label_6->setText(QCoreApplication::translate("DicEdit", "Version", nullptr));
        VersionLBl->setText(QCoreApplication::translate("DicEdit", "##.##-##", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DicEdit: public Ui_DicEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DICEDIT_H
