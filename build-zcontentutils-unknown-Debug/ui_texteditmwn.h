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
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

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
    QPushButton *moreBTn;
    QPushButton *wrapBTn;
    QPushButton *closeBTn;
    QPlainTextEdit *textPTe;
    QStatusBar *statusbar;
    QMenuBar *menubar;

    void setupUi(QMainWindow *textEditMWn)
    {
        if (textEditMWn->objectName().isEmpty())
            textEditMWn->setObjectName(QString::fromUtf8("textEditMWn"));
        textEditMWn->resize(813, 628);
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

        moreBTn = new QPushButton(verticalLayoutWidget);
        moreBTn->setObjectName(QString::fromUtf8("moreBTn"));

        horizontalLayout->addWidget(moreBTn);

        wrapBTn = new QPushButton(verticalLayoutWidget);
        wrapBTn->setObjectName(QString::fromUtf8("wrapBTn"));

        horizontalLayout->addWidget(wrapBTn);

        closeBTn = new QPushButton(verticalLayoutWidget);
        closeBTn->setObjectName(QString::fromUtf8("closeBTn"));

        horizontalLayout->addWidget(closeBTn);


        verticalLayout->addLayout(horizontalLayout);

        textPTe = new QPlainTextEdit(verticalLayoutWidget);
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
        moreBTn->setText(QCoreApplication::translate("textEditMWn", "More", nullptr));
        wrapBTn->setText(QCoreApplication::translate("textEditMWn", "No wrap", nullptr));
        closeBTn->setText(QCoreApplication::translate("textEditMWn", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class textEditMWn: public Ui_textEditMWn {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTEDITMWN_H
