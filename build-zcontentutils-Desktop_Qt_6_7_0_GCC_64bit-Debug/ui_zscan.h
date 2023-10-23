/********************************************************************************
** Form generated from reading UI file 'zscan.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZSCAN_H
#define UI_ZSCAN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZScan
{
public:
    QWidget *centralwidget;
    QTableWidget *tableTBw;
    QFrame *frame;
    QLabel *FilePathLBl;
    QProgressBar *ProgressPGb;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *UsedCountLBl;
    QLabel *DeletedCountLBl;
    QLabel *HolesCountLBl;
    QLabel *UsedSizeLBl;
    QLabel *DeletedSizeLBl;
    QLabel *HolesSizeLBl;
    QLabel *SizeReadLBl;
    QLabel *label_3;
    QLabel *FileSizeLBl;
    QLabel *label_7;
    QLabel *FreeCountLBl;
    QLabel *FreeSizeLBl;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ZScan)
    {
        if (ZScan->objectName().isEmpty())
            ZScan->setObjectName("ZScan");
        ZScan->resize(800, 600);
        centralwidget = new QWidget(ZScan);
        centralwidget->setObjectName("centralwidget");
        tableTBw = new QTableWidget(centralwidget);
        tableTBw->setObjectName("tableTBw");
        tableTBw->setGeometry(QRect(10, 160, 771, 381));
        frame = new QFrame(centralwidget);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(10, 10, 771, 131));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        FilePathLBl = new QLabel(frame);
        FilePathLBl->setObjectName("FilePathLBl");
        FilePathLBl->setGeometry(QRect(10, 10, 711, 16));
        ProgressPGb = new QProgressBar(frame);
        ProgressPGb->setObjectName("ProgressPGb");
        ProgressPGb->setGeometry(QRect(10, 80, 261, 23));
        ProgressPGb->setValue(24);
        label = new QLabel(frame);
        label->setObjectName("label");
        label->setGeometry(QRect(310, 70, 101, 16));
        label_2 = new QLabel(frame);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(310, 90, 101, 16));
        label_4 = new QLabel(frame);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(310, 50, 101, 16));
        label_5 = new QLabel(frame);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(410, 30, 101, 16));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_6 = new QLabel(frame);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(580, 30, 131, 16));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        UsedCountLBl = new QLabel(frame);
        UsedCountLBl->setObjectName("UsedCountLBl");
        UsedCountLBl->setGeometry(QRect(410, 50, 101, 16));
        UsedCountLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        DeletedCountLBl = new QLabel(frame);
        DeletedCountLBl->setObjectName("DeletedCountLBl");
        DeletedCountLBl->setGeometry(QRect(410, 70, 101, 16));
        DeletedCountLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        HolesCountLBl = new QLabel(frame);
        HolesCountLBl->setObjectName("HolesCountLBl");
        HolesCountLBl->setGeometry(QRect(410, 90, 101, 16));
        HolesCountLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        UsedSizeLBl = new QLabel(frame);
        UsedSizeLBl->setObjectName("UsedSizeLBl");
        UsedSizeLBl->setGeometry(QRect(610, 50, 101, 16));
        UsedSizeLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        DeletedSizeLBl = new QLabel(frame);
        DeletedSizeLBl->setObjectName("DeletedSizeLBl");
        DeletedSizeLBl->setGeometry(QRect(610, 70, 101, 16));
        DeletedSizeLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        HolesSizeLBl = new QLabel(frame);
        HolesSizeLBl->setObjectName("HolesSizeLBl");
        HolesSizeLBl->setGeometry(QRect(610, 90, 101, 16));
        HolesSizeLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        SizeReadLBl = new QLabel(frame);
        SizeReadLBl->setObjectName("SizeReadLBl");
        SizeReadLBl->setGeometry(QRect(10, 50, 71, 16));
        label_3 = new QLabel(frame);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(110, 50, 31, 16));
        FileSizeLBl = new QLabel(frame);
        FileSizeLBl->setObjectName("FileSizeLBl");
        FileSizeLBl->setGeometry(QRect(150, 50, 101, 16));
        label_7 = new QLabel(frame);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(310, 110, 101, 16));
        FreeCountLBl = new QLabel(frame);
        FreeCountLBl->setObjectName("FreeCountLBl");
        FreeCountLBl->setGeometry(QRect(410, 110, 101, 16));
        FreeCountLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        FreeSizeLBl = new QLabel(frame);
        FreeSizeLBl->setObjectName("FreeSizeLBl");
        FreeSizeLBl->setGeometry(QRect(610, 110, 101, 16));
        FreeSizeLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        ZScan->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ZScan);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 20));
        ZScan->setMenuBar(menubar);
        statusbar = new QStatusBar(ZScan);
        statusbar->setObjectName("statusbar");
        ZScan->setStatusBar(statusbar);

        retranslateUi(ZScan);

        QMetaObject::connectSlotsByName(ZScan);
    } // setupUi

    void retranslateUi(QMainWindow *ZScan)
    {
        ZScan->setWindowTitle(QCoreApplication::translate("ZScan", "MainWindow", nullptr));
        FilePathLBl->setText(QCoreApplication::translate("ZScan", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("ZScan", "Deleted blocks", nullptr));
        label_2->setText(QCoreApplication::translate("ZScan", "Holes", nullptr));
        label_4->setText(QCoreApplication::translate("ZScan", "Used blocks", nullptr));
        label_5->setText(QCoreApplication::translate("ZScan", "count", nullptr));
        label_6->setText(QCoreApplication::translate("ZScan", "cumulated size", nullptr));
        UsedCountLBl->setText(QCoreApplication::translate("ZScan", "count", nullptr));
        DeletedCountLBl->setText(QCoreApplication::translate("ZScan", "count", nullptr));
        HolesCountLBl->setText(QCoreApplication::translate("ZScan", "count", nullptr));
        UsedSizeLBl->setText(QCoreApplication::translate("ZScan", "count", nullptr));
        DeletedSizeLBl->setText(QCoreApplication::translate("ZScan", "count", nullptr));
        HolesSizeLBl->setText(QCoreApplication::translate("ZScan", "count", nullptr));
        SizeReadLBl->setText(QCoreApplication::translate("ZScan", "TextLabel", nullptr));
        label_3->setText(QCoreApplication::translate("ZScan", "of", nullptr));
        FileSizeLBl->setText(QCoreApplication::translate("ZScan", "TextLabel", nullptr));
        label_7->setText(QCoreApplication::translate("ZScan", "Free blocks", nullptr));
        FreeCountLBl->setText(QCoreApplication::translate("ZScan", "count", nullptr));
        FreeSizeLBl->setText(QCoreApplication::translate("ZScan", "count", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZScan: public Ui_ZScan {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZSCAN_H
