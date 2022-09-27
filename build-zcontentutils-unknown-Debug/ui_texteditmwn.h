/********************************************************************************
** Form generated from reading UI file 'texteditmwn.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTEDITMWN_H
#define UI_TEXTEDITMWN_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <zqplaintextedit.h>

QT_BEGIN_NAMESPACE

class Ui_textEditMWn
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *ClosedLBl;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *searchBTn;
    QLineEdit *searchLEd;
    QPushButton *searchMainBTn;
    QPushButton *filterBTn;
    QPushButton *lineNumbersBTn;
    QPushButton *wrapBTn;
    QPushButton *closeBTn;
    ZQPlainTextEdit *textPTe;
    QStatusBar *statusbar;
    QMenuBar *menubar;

    void setupUi(QMainWindow *textEditMWn)
    {
        if (textEditMWn->objectName().isEmpty())
            textEditMWn->setObjectName(QString::fromUtf8("textEditMWn"));
        textEditMWn->resize(813, 623);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textEditMWn->sizePolicy().hasHeightForWidth());
        textEditMWn->setSizePolicy(sizePolicy);
        textEditMWn->setSizeIncrement(QSize(3, 3));
        centralwidget = new QWidget(textEditMWn);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 10, 811, 571));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        ClosedLBl = new QLabel(verticalLayoutWidget);
        ClosedLBl->setObjectName(QString::fromUtf8("ClosedLBl"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        ClosedLBl->setFont(font);

        horizontalLayout->addWidget(ClosedLBl);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        searchBTn = new QPushButton(verticalLayoutWidget);
        searchBTn->setObjectName(QString::fromUtf8("searchBTn"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../zqt/icons/magnifyingglass.png"), QSize(), QIcon::Normal, QIcon::Off);
        searchBTn->setIcon(icon);

        horizontalLayout_2->addWidget(searchBTn);

        searchLEd = new QLineEdit(verticalLayoutWidget);
        searchLEd->setObjectName(QString::fromUtf8("searchLEd"));

        horizontalLayout_2->addWidget(searchLEd);


        horizontalLayout->addLayout(horizontalLayout_2);

        searchMainBTn = new QPushButton(verticalLayoutWidget);
        searchMainBTn->setObjectName(QString::fromUtf8("searchMainBTn"));
#if QT_CONFIG(tooltip)
        searchMainBTn->setToolTip(QString::fromUtf8("set search on/off"));
#endif // QT_CONFIG(tooltip)
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../zqt/icons/strigi.png"), QSize(), QIcon::Normal, QIcon::Off);
        searchMainBTn->setIcon(icon1);

        horizontalLayout->addWidget(searchMainBTn);

        filterBTn = new QPushButton(verticalLayoutWidget);
        filterBTn->setObjectName(QString::fromUtf8("filterBTn"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("../../zqt/icons/funel.png"), QSize(), QIcon::Normal, QIcon::Off);
        filterBTn->setIcon(icon2);

        horizontalLayout->addWidget(filterBTn);

        lineNumbersBTn = new QPushButton(verticalLayoutWidget);
        lineNumbersBTn->setObjectName(QString::fromUtf8("lineNumbersBTn"));

        horizontalLayout->addWidget(lineNumbersBTn);

        wrapBTn = new QPushButton(verticalLayoutWidget);
        wrapBTn->setObjectName(QString::fromUtf8("wrapBTn"));

        horizontalLayout->addWidget(wrapBTn);

        closeBTn = new QPushButton(verticalLayoutWidget);
        closeBTn->setObjectName(QString::fromUtf8("closeBTn"));

        horizontalLayout->addWidget(closeBTn);


        verticalLayout->addLayout(horizontalLayout);

        textPTe = new ZQPlainTextEdit(verticalLayoutWidget);
        textPTe->setObjectName(QString::fromUtf8("textPTe"));
        sizePolicy1.setHeightForWidth(textPTe->sizePolicy().hasHeightForWidth());
        textPTe->setSizePolicy(sizePolicy1);
        textPTe->setSizeIncrement(QSize(0, 0));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier"));
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        textPTe->setFont(font1);
        textPTe->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        textPTe->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        verticalLayout->addWidget(textPTe);

        textEditMWn->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(textEditMWn);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        textEditMWn->setStatusBar(statusbar);
        menubar = new QMenuBar(textEditMWn);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 813, 20));
        textEditMWn->setMenuBar(menubar);

        retranslateUi(textEditMWn);

        QMetaObject::connectSlotsByName(textEditMWn);
    } // setupUi

    void retranslateUi(QMainWindow *textEditMWn)
    {
        textEditMWn->setWindowTitle(QCoreApplication::translate("textEditMWn", "MainWindow", nullptr));
        ClosedLBl->setText(QCoreApplication::translate("textEditMWn", "No Open File", nullptr));
        searchBTn->setText(QString());
        searchMainBTn->setText(QString());
        filterBTn->setText(QString());
        lineNumbersBTn->setText(QCoreApplication::translate("textEditMWn", "Line numbers On", nullptr));
        wrapBTn->setText(QCoreApplication::translate("textEditMWn", "No wrap", nullptr));
        closeBTn->setText(QCoreApplication::translate("textEditMWn", "Close", nullptr));
#if QT_CONFIG(tooltip)
        textPTe->setToolTip(QCoreApplication::translate("textEditMWn", "Filter content", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class textEditMWn: public Ui_textEditMWn {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTEDITMWN_H
