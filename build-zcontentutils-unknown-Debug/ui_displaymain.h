/********************************************************************************
** Form generated from reading UI file 'displaymain.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAYMAIN_H
#define UI_DISPLAYMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DisplayMain
{
public:
    QWidget *centralwidget;
    QFrame *frame;
    QLabel *label;
    QLabel *OffsetLBl;
    QLabel *label_2;
    QLabel *ZEntityLBl;
    QSlider *OffsetSLd;
    QLabel *ClosedLBl;
    QTableView *displayTBv;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *DisplayMain)
    {
        if (DisplayMain->objectName().isEmpty())
            DisplayMain->setObjectName(QString::fromUtf8("DisplayMain"));
        DisplayMain->resize(632, 573);
        centralwidget = new QWidget(DisplayMain);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 621, 101));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 20, 61, 16));
        OffsetLBl = new QLabel(frame);
        OffsetLBl->setObjectName(QString::fromUtf8("OffsetLBl"));
        OffsetLBl->setGeometry(QRect(120, 20, 161, 16));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(390, 20, 61, 16));
        ZEntityLBl = new QLabel(frame);
        ZEntityLBl->setObjectName(QString::fromUtf8("ZEntityLBl"));
        ZEntityLBl->setGeometry(QRect(490, 20, 131, 16));
        OffsetSLd = new QSlider(frame);
        OffsetSLd->setObjectName(QString::fromUtf8("OffsetSLd"));
        OffsetSLd->setGeometry(QRect(110, 50, 261, 16));
        OffsetSLd->setMaximum(100);
        OffsetSLd->setOrientation(Qt::Horizontal);
        ClosedLBl = new QLabel(frame);
        ClosedLBl->setObjectName(QString::fromUtf8("ClosedLBl"));
        ClosedLBl->setGeometry(QRect(490, 40, 151, 31));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        ClosedLBl->setFont(font);
        displayTBv = new QTableView(centralwidget);
        displayTBv->setObjectName(QString::fromUtf8("displayTBv"));
        displayTBv->setGeometry(QRect(0, 110, 621, 431));
        DisplayMain->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(DisplayMain);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        DisplayMain->setStatusBar(statusbar);

        retranslateUi(DisplayMain);

        QMetaObject::connectSlotsByName(DisplayMain);
    } // setupUi

    void retranslateUi(QMainWindow *DisplayMain)
    {
        DisplayMain->setWindowTitle(QCoreApplication::translate("DisplayMain", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("DisplayMain", "Offset", nullptr));
        OffsetLBl->setText(QCoreApplication::translate("DisplayMain", "Offset value", nullptr));
        label_2->setText(QString());
        ZEntityLBl->setText(QString());
        ClosedLBl->setText(QCoreApplication::translate("DisplayMain", "No Open File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DisplayMain: public Ui_DisplayMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAYMAIN_H
