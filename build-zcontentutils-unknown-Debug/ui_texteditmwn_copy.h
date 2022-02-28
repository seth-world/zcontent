/********************************************************************************
** Form generated from reading UI file 'texteditmwn_copy.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTEDITMWN_COPY_H
#define UI_TEXTEDITMWN_COPY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_textEditMWn
{
public:
    QWidget *centralwidget;
    QPlainTextEdit *textPTe;
    QPushButton *closeBTn;
    QPushButton *moreBTn;
    QLabel *ClosedLBl;
    QStatusBar *statusbar;
    QMenuBar *menubar;

    void setupUi(QMainWindow *textEditMWn)
    {
        if (textEditMWn->objectName().isEmpty())
            textEditMWn->setObjectName(QString::fromUtf8("textEditMWn"));
        textEditMWn->resize(813, 615);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textEditMWn->sizePolicy().hasHeightForWidth());
        textEditMWn->setSizePolicy(sizePolicy);
        textEditMWn->setSizeIncrement(QSize(3, 3));
        centralwidget = new QWidget(textEditMWn);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        textPTe = new QPlainTextEdit(centralwidget);
        textPTe->setObjectName(QString::fromUtf8("textPTe"));
        textPTe->setGeometry(QRect(30, 10, 729, 529));
        sizePolicy.setHeightForWidth(textPTe->sizePolicy().hasHeightForWidth());
        textPTe->setSizePolicy(sizePolicy);
        textPTe->setSizeIncrement(QSize(0, 0));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        textPTe->setFont(font);
        textPTe->setLineWrapMode(QPlainTextEdit::NoWrap);
        closeBTn = new QPushButton(centralwidget);
        closeBTn->setObjectName(QString::fromUtf8("closeBTn"));
        closeBTn->setGeometry(QRect(680, 540, 80, 23));
        moreBTn = new QPushButton(centralwidget);
        moreBTn->setObjectName(QString::fromUtf8("moreBTn"));
        moreBTn->setGeometry(QRect(590, 540, 80, 23));
        ClosedLBl = new QLabel(centralwidget);
        ClosedLBl->setObjectName(QString::fromUtf8("ClosedLBl"));
        ClosedLBl->setGeometry(QRect(30, 540, 151, 31));
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        ClosedLBl->setFont(font1);
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
        closeBTn->setText(QCoreApplication::translate("textEditMWn", "Close", nullptr));
        moreBTn->setText(QCoreApplication::translate("textEditMWn", "More", nullptr));
        ClosedLBl->setText(QCoreApplication::translate("textEditMWn", "No Open File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class textEditMWn: public Ui_textEditMWn {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTEDITMWN_COPY_H
