/********************************************************************************
** Form generated from reading UI file 'zcollectionlist.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZCOLLECTIONLIST_H
#define UI_ZCOLLECTIONLIST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZCollectionList
{
public:
    QWidget *MainWDg;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *QuitBTn;
    QPushButton *IKBTn;

    void setupUi(QDialog *ZCollectionList)
    {
        if (ZCollectionList->objectName().isEmpty())
            ZCollectionList->setObjectName("ZCollectionList");
        ZCollectionList->resize(386, 273);
        MainWDg = new QWidget(ZCollectionList);
        MainWDg->setObjectName("MainWDg");
        MainWDg->setGeometry(QRect(10, 10, 371, 201));
        horizontalLayoutWidget = new QWidget(ZCollectionList);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(180, 220, 201, 51));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        QuitBTn = new QPushButton(horizontalLayoutWidget);
        QuitBTn->setObjectName("QuitBTn");

        horizontalLayout->addWidget(QuitBTn);

        IKBTn = new QPushButton(horizontalLayoutWidget);
        IKBTn->setObjectName("IKBTn");

        horizontalLayout->addWidget(IKBTn);


        retranslateUi(ZCollectionList);

        QMetaObject::connectSlotsByName(ZCollectionList);
    } // setupUi

    void retranslateUi(QDialog *ZCollectionList)
    {
        ZCollectionList->setWindowTitle(QCoreApplication::translate("ZCollectionList", "Dialog", nullptr));
        QuitBTn->setText(QCoreApplication::translate("ZCollectionList", "Quit", nullptr));
        IKBTn->setText(QCoreApplication::translate("ZCollectionList", "Select", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZCollectionList: public Ui_ZCollectionList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZCOLLECTIONLIST_H
