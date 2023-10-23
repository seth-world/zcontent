/********************************************************************************
** Form generated from reading UI file 'poolvisuDLg.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POOLVISUDLG_H
#define UI_POOLVISUDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_poolVisu
{
public:
    QLabel *label_7;
    QComboBox *PoolCBx;
    QLabel *label_6;
    QLabel *HeaderLBl;
    QLabel *label_4;
    QLabel *label_8;
    QLabel *ContentLBl;
    QLabel *OffsetAbsLBl;
    QLabel *OffsetRelLBl;
    QLabel *PoolNameLBl;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *HeaderVBl;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *ContentVBl;
    QPushButton *SaveBTn;

    void setupUi(QDialog *poolVisu)
    {
        if (poolVisu->objectName().isEmpty())
            poolVisu->setObjectName("poolVisu");
        poolVisu->resize(813, 546);
        label_7 = new QLabel(poolVisu);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 40, 71, 16));
        PoolCBx = new QComboBox(poolVisu);
        PoolCBx->setObjectName("PoolCBx");
        PoolCBx->setGeometry(QRect(650, 0, 151, 23));
        label_6 = new QLabel(poolVisu);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 20, 71, 16));
        HeaderLBl = new QLabel(poolVisu);
        HeaderLBl->setObjectName("HeaderLBl");
        HeaderLBl->setGeometry(QRect(100, 20, 541, 16));
        label_4 = new QLabel(poolVisu);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 60, 121, 16));
        label_8 = new QLabel(poolVisu);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(260, 60, 161, 16));
        ContentLBl = new QLabel(poolVisu);
        ContentLBl->setObjectName("ContentLBl");
        ContentLBl->setGeometry(QRect(100, 40, 541, 16));
        OffsetAbsLBl = new QLabel(poolVisu);
        OffsetAbsLBl->setObjectName("OffsetAbsLBl");
        OffsetAbsLBl->setGeometry(QRect(120, 60, 111, 16));
        OffsetAbsLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        OffsetRelLBl = new QLabel(poolVisu);
        OffsetRelLBl->setObjectName("OffsetRelLBl");
        OffsetRelLBl->setGeometry(QRect(410, 60, 111, 16));
        OffsetRelLBl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        PoolNameLBl = new QLabel(poolVisu);
        PoolNameLBl->setObjectName("PoolNameLBl");
        PoolNameLBl->setGeometry(QRect(230, 0, 261, 20));
        QFont font;
        font.setBold(true);
        PoolNameLBl->setFont(font);
        verticalLayoutWidget = new QWidget(poolVisu);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(10, 90, 791, 191));
        HeaderVBl = new QVBoxLayout(verticalLayoutWidget);
        HeaderVBl->setObjectName("HeaderVBl");
        HeaderVBl->setContentsMargins(0, 0, 0, 0);
        verticalLayoutWidget_2 = new QWidget(poolVisu);
        verticalLayoutWidget_2->setObjectName("verticalLayoutWidget_2");
        verticalLayoutWidget_2->setGeometry(QRect(10, 290, 791, 251));
        ContentVBl = new QVBoxLayout(verticalLayoutWidget_2);
        ContentVBl->setObjectName("ContentVBl");
        ContentVBl->setContentsMargins(0, 0, 0, 0);
        SaveBTn = new QPushButton(poolVisu);
        SaveBTn->setObjectName("SaveBTn");
        SaveBTn->setGeometry(QRect(720, 30, 80, 23));

        retranslateUi(poolVisu);

        QMetaObject::connectSlotsByName(poolVisu);
    } // setupUi

    void retranslateUi(QDialog *poolVisu)
    {
        poolVisu->setWindowTitle(QCoreApplication::translate("poolVisu", "Dialog", nullptr));
        label_7->setText(QCoreApplication::translate("poolVisu", "Content", nullptr));
        label_6->setText(QCoreApplication::translate("poolVisu", "Header", nullptr));
        HeaderLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
        label_4->setText(QCoreApplication::translate("poolVisu", "Absolute Offset", nullptr));
        label_8->setText(QCoreApplication::translate("poolVisu", "offset relative to FCB", nullptr));
        ContentLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
        OffsetAbsLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
        OffsetRelLBl->setText(QCoreApplication::translate("poolVisu", "123", nullptr));
        PoolNameLBl->setText(QCoreApplication::translate("poolVisu", "Block Access Table", nullptr));
        SaveBTn->setText(QCoreApplication::translate("poolVisu", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class poolVisu: public Ui_poolVisu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POOLVISUDLG_H
