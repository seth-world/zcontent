/********************************************************************************
** Form generated from reading UI file 'poolvisu.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POOLVISU_H
#define UI_POOLVISU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_poolVisu
{
public:
    QWidget *centralwidget;
    QComboBox *PoolCBx;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *ContentVBl;
    QLabel *label_8;
    QLabel *PoolNameLBl;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *HeaderVBl;
    QLabel *label_7;
    QLabel *label_6;
    QLabel *OffsetRelLBl;
    QLabel *label_4;
    QLabel *OffsetAbsLBl;
    QPushButton *SaveBTn;
    QLabel *HeaderLBl;
    QLabel *ContentLBl;
    QPushButton *RefreshBTn;
    QGraphicsView *FileGraphicGVw;
    QPushButton *ZoomInBTn;
    QPushButton *ZoomOutBTn;
    QPushButton *FitBTn;
    QLabel *label_9;
    QLabel *ContentSizeLBl;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *poolVisu)
    {
        if (poolVisu->objectName().isEmpty())
            poolVisu->setObjectName("poolVisu");
        poolVisu->resize(820, 726);
        centralwidget = new QWidget(poolVisu);
        centralwidget->setObjectName("centralwidget");
        PoolCBx = new QComboBox(centralwidget);
        PoolCBx->setObjectName("PoolCBx");
        PoolCBx->setGeometry(QRect(630, 10, 171, 23));
        verticalLayoutWidget_2 = new QWidget(centralwidget);
        verticalLayoutWidget_2->setObjectName("verticalLayoutWidget_2");
        verticalLayoutWidget_2->setGeometry(QRect(10, 420, 791, 251));
        ContentVBl = new QVBoxLayout(verticalLayoutWidget_2);
        ContentVBl->setObjectName("ContentVBl");
        ContentVBl->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(260, 90, 161, 16));
        PoolNameLBl = new QLabel(centralwidget);
        PoolNameLBl->setObjectName("PoolNameLBl");
        PoolNameLBl->setGeometry(QRect(230, 10, 261, 20));
        QFont font;
        font.setBold(true);
        PoolNameLBl->setFont(font);
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(10, 210, 791, 211));
        HeaderVBl = new QVBoxLayout(verticalLayoutWidget);
        HeaderVBl->setObjectName("HeaderVBl");
        HeaderVBl->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 50, 71, 16));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 30, 71, 16));
        OffsetRelLBl = new QLabel(centralwidget);
        OffsetRelLBl->setObjectName("OffsetRelLBl");
        OffsetRelLBl->setGeometry(QRect(410, 90, 111, 16));
        OffsetRelLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 90, 121, 16));
        OffsetAbsLBl = new QLabel(centralwidget);
        OffsetAbsLBl->setObjectName("OffsetAbsLBl");
        OffsetAbsLBl->setGeometry(QRect(120, 90, 111, 16));
        OffsetAbsLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        SaveBTn = new QPushButton(centralwidget);
        SaveBTn->setObjectName("SaveBTn");
        SaveBTn->setGeometry(QRect(720, 40, 80, 23));
        HeaderLBl = new QLabel(centralwidget);
        HeaderLBl->setObjectName("HeaderLBl");
        HeaderLBl->setGeometry(QRect(100, 30, 541, 16));
        ContentLBl = new QLabel(centralwidget);
        ContentLBl->setObjectName("ContentLBl");
        ContentLBl->setGeometry(QRect(100, 50, 541, 16));
        RefreshBTn = new QPushButton(centralwidget);
        RefreshBTn->setObjectName("RefreshBTn");
        RefreshBTn->setGeometry(QRect(630, 40, 80, 23));
        FileGraphicGVw = new QGraphicsView(centralwidget);
        FileGraphicGVw->setObjectName("FileGraphicGVw");
        FileGraphicGVw->setGeometry(QRect(10, 110, 791, 101));
        ZoomInBTn = new QPushButton(centralwidget);
        ZoomInBTn->setObjectName("ZoomInBTn");
        ZoomInBTn->setGeometry(QRect(630, 70, 41, 23));
        ZoomOutBTn = new QPushButton(centralwidget);
        ZoomOutBTn->setObjectName("ZoomOutBTn");
        ZoomOutBTn->setGeometry(QRect(760, 70, 41, 23));
        FitBTn = new QPushButton(centralwidget);
        FitBTn->setObjectName("FitBTn");
        FitBTn->setGeometry(QRect(690, 70, 41, 23));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 70, 91, 16));
        ContentSizeLBl = new QLabel(centralwidget);
        ContentSizeLBl->setObjectName("ContentSizeLBl");
        ContentSizeLBl->setGeometry(QRect(100, 70, 101, 16));
        poolVisu->setCentralWidget(centralwidget);
        menubar = new QMenuBar(poolVisu);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 820, 20));
        poolVisu->setMenuBar(menubar);
        statusbar = new QStatusBar(poolVisu);
        statusbar->setObjectName("statusbar");
        poolVisu->setStatusBar(statusbar);

        retranslateUi(poolVisu);

        QMetaObject::connectSlotsByName(poolVisu);
    } // setupUi

    void retranslateUi(QMainWindow *poolVisu)
    {
        poolVisu->setWindowTitle(QCoreApplication::translate("poolVisu", "MainWindow", nullptr));
        label_8->setText(QCoreApplication::translate("poolVisu", "offset relative to FCB", nullptr));
        PoolNameLBl->setText(QCoreApplication::translate("poolVisu", "Block Access Table", nullptr));
        label_7->setText(QCoreApplication::translate("poolVisu", "Content", nullptr));
        label_6->setText(QCoreApplication::translate("poolVisu", "Header", nullptr));
        OffsetRelLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
        label_4->setText(QCoreApplication::translate("poolVisu", "Absolute Offset", nullptr));
        OffsetAbsLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
        SaveBTn->setText(QCoreApplication::translate("poolVisu", "Save", nullptr));
        HeaderLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
        ContentLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
        RefreshBTn->setText(QCoreApplication::translate("poolVisu", "Refresh", nullptr));
        ZoomInBTn->setText(QCoreApplication::translate("poolVisu", "+", nullptr));
        ZoomOutBTn->setText(QCoreApplication::translate("poolVisu", "-", nullptr));
        FitBTn->setText(QCoreApplication::translate("poolVisu", "Fit", nullptr));
        label_9->setText(QCoreApplication::translate("poolVisu", "Content size", nullptr));
        ContentSizeLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
    } // retranslateUi

};

namespace Ui {
    class poolVisu: public Ui_poolVisu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POOLVISU_H
