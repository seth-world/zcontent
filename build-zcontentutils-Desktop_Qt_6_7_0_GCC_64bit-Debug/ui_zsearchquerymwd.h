/********************************************************************************
** Form generated from reading UI file 'zsearchquerymwd.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZSEARCHQUERYMWD_H
#define UI_ZSEARCHQUERYMWD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZSearchQueryMWd
{
public:
    QWidget *centralwidget;
    QTextEdit *textEdit;
    QTableView *tableView;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ZSearchQueryMWd)
    {
        if (ZSearchQueryMWd->objectName().isEmpty())
            ZSearchQueryMWd->setObjectName("ZSearchQueryMWd");
        ZSearchQueryMWd->resize(814, 641);
        centralwidget = new QWidget(ZSearchQueryMWd);
        centralwidget->setObjectName("centralwidget");
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(0, 10, 801, 91));
        tableView = new QTableView(centralwidget);
        tableView->setObjectName("tableView");
        tableView->setGeometry(QRect(0, 130, 801, 411));
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(0, 550, 801, 41));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_2 = new QPushButton(horizontalLayoutWidget);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout->addWidget(pushButton_2);

        pushButton = new QPushButton(horizontalLayoutWidget);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        ZSearchQueryMWd->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ZSearchQueryMWd);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 814, 20));
        ZSearchQueryMWd->setMenuBar(menubar);
        statusbar = new QStatusBar(ZSearchQueryMWd);
        statusbar->setObjectName("statusbar");
        ZSearchQueryMWd->setStatusBar(statusbar);

        retranslateUi(ZSearchQueryMWd);

        QMetaObject::connectSlotsByName(ZSearchQueryMWd);
    } // setupUi

    void retranslateUi(QMainWindow *ZSearchQueryMWd)
    {
        ZSearchQueryMWd->setWindowTitle(QCoreApplication::translate("ZSearchQueryMWd", "MainWindow", nullptr));
        pushButton_2->setText(QCoreApplication::translate("ZSearchQueryMWd", "PushButton", nullptr));
        pushButton->setText(QCoreApplication::translate("ZSearchQueryMWd", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZSearchQueryMWd: public Ui_ZSearchQueryMWd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZSEARCHQUERYMWD_H
