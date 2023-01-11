/********************************************************************************
** Form generated from reading UI file 'poolvisu.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POOLVISU_H
#define UI_POOLVISU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
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
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *poolVisu)
    {
        if (poolVisu->objectName().isEmpty())
            poolVisu->setObjectName(QString::fromUtf8("poolVisu"));
        poolVisu->resize(820, 600);
        centralwidget = new QWidget(poolVisu);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        PoolCBx = new QComboBox(centralwidget);
        PoolCBx->setObjectName(QString::fromUtf8("PoolCBx"));
        PoolCBx->setGeometry(QRect(630, 10, 171, 23));
        verticalLayoutWidget_2 = new QWidget(centralwidget);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(10, 300, 791, 251));
        ContentVBl = new QVBoxLayout(verticalLayoutWidget_2);
        ContentVBl->setObjectName(QString::fromUtf8("ContentVBl"));
        ContentVBl->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(260, 70, 161, 16));
        PoolNameLBl = new QLabel(centralwidget);
        PoolNameLBl->setObjectName(QString::fromUtf8("PoolNameLBl"));
        PoolNameLBl->setGeometry(QRect(230, 10, 261, 20));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        PoolNameLBl->setFont(font);
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 100, 791, 191));
        HeaderVBl = new QVBoxLayout(verticalLayoutWidget);
        HeaderVBl->setObjectName(QString::fromUtf8("HeaderVBl"));
        HeaderVBl->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 50, 71, 16));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 30, 71, 16));
        OffsetRelLBl = new QLabel(centralwidget);
        OffsetRelLBl->setObjectName(QString::fromUtf8("OffsetRelLBl"));
        OffsetRelLBl->setGeometry(QRect(410, 70, 111, 16));
        OffsetRelLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 70, 121, 16));
        OffsetAbsLBl = new QLabel(centralwidget);
        OffsetAbsLBl->setObjectName(QString::fromUtf8("OffsetAbsLBl"));
        OffsetAbsLBl->setGeometry(QRect(120, 70, 111, 16));
        OffsetAbsLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        SaveBTn = new QPushButton(centralwidget);
        SaveBTn->setObjectName(QString::fromUtf8("SaveBTn"));
        SaveBTn->setGeometry(QRect(720, 40, 80, 23));
        HeaderLBl = new QLabel(centralwidget);
        HeaderLBl->setObjectName(QString::fromUtf8("HeaderLBl"));
        HeaderLBl->setGeometry(QRect(100, 30, 541, 16));
        ContentLBl = new QLabel(centralwidget);
        ContentLBl->setObjectName(QString::fromUtf8("ContentLBl"));
        ContentLBl->setGeometry(QRect(100, 50, 541, 16));
        RefreshBTn = new QPushButton(centralwidget);
        RefreshBTn->setObjectName(QString::fromUtf8("RefreshBTn"));
        RefreshBTn->setGeometry(QRect(630, 40, 80, 23));
        poolVisu->setCentralWidget(centralwidget);
        menubar = new QMenuBar(poolVisu);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 820, 20));
        poolVisu->setMenuBar(menubar);
        statusbar = new QStatusBar(poolVisu);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
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
    } // retranslateUi

};

namespace Ui {
    class poolVisu: public Ui_poolVisu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POOLVISU_H
