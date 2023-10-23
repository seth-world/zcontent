/********************************************************************************
** Form generated from reading UI file 'rawfields.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAWFIELDS_H
#define UI_RAWFIELDS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "zqtableview.h"
#include "zqtreeview.h"

QT_BEGIN_NAMESPACE

class Ui_RawFields
{
public:
    QWidget *centralwidget;
    QFrame *frame_2;
    QLabel *ClassNameLBl;
    QLabel *label_4;
    QLabel *CppFileNameLBl;
    QLabel *FieldsNbLBl;
    QLabel *label_3;
    QLabel *label_6;
    QLabel *DateLBl;
    QLabel *label_5;
    QProgressBar *parsingPBa;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    ZQTreeView *parsingResultTRv;
    QLabel *label_2;
    ZQTableView *fieldTBv;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *ParseBTn;
    QPushButton *ToDicBTn;
    QPushButton *QuitBTn;
    QMenuBar *menubar;
    QMenu *GeneralMEn;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RawFields)
    {
        if (RawFields->objectName().isEmpty())
            RawFields->setObjectName("RawFields");
        RawFields->resize(1153, 600);
        centralwidget = new QWidget(RawFields);
        centralwidget->setObjectName("centralwidget");
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName("frame_2");
        frame_2->setGeometry(QRect(0, 10, 1131, 41));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        ClassNameLBl = new QLabel(frame_2);
        ClassNameLBl->setObjectName("ClassNameLBl");
        ClassNameLBl->setGeometry(QRect(780, 10, 161, 20));
        label_4 = new QLabel(frame_2);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(950, 10, 71, 19));
        CppFileNameLBl = new QLabel(frame_2);
        CppFileNameLBl->setObjectName("CppFileNameLBl");
        CppFileNameLBl->setGeometry(QRect(12, 10, 491, 19));
        FieldsNbLBl = new QLabel(frame_2);
        FieldsNbLBl->setObjectName("FieldsNbLBl");
        FieldsNbLBl->setGeometry(QRect(1000, 10, 71, 19));
        label_3 = new QLabel(frame_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(730, 10, 51, 19));
        label_6 = new QLabel(frame_2);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(490, 10, 101, 20));
        DateLBl = new QLabel(frame_2);
        DateLBl->setObjectName("DateLBl");
        DateLBl->setGeometry(QRect(560, 10, 161, 20));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(500, 60, 91, 16));
        parsingPBa = new QProgressBar(centralwidget);
        parsingPBa->setObjectName("parsingPBa");
        parsingPBa->setGeometry(QRect(10, 60, 81, 16));
        parsingPBa->setValue(24);
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(0, 80, 1131, 481));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        parsingResultTRv = new ZQTreeView(verticalLayoutWidget);
        parsingResultTRv->setObjectName("parsingResultTRv");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(parsingResultTRv->sizePolicy().hasHeightForWidth());
        parsingResultTRv->setSizePolicy(sizePolicy);
        parsingResultTRv->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout->addWidget(parsingResultTRv);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_2);

        fieldTBv = new ZQTableView(verticalLayoutWidget);
        fieldTBv->setObjectName("fieldTBv");
        fieldTBv->setAcceptDrops(true);
        fieldTBv->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout->addWidget(fieldTBv);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        ParseBTn = new QPushButton(verticalLayoutWidget);
        ParseBTn->setObjectName("ParseBTn");
        QFont font;
        font.setPointSize(9);
        font.setItalic(false);
        font.setKerning(true);
        ParseBTn->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../zqt/icons/gearing.png"), QSize(), QIcon::Normal, QIcon::Off);
        ParseBTn->setIcon(icon);
        ParseBTn->setIconSize(QSize(26, 26));

        horizontalLayout_2->addWidget(ParseBTn);

        ToDicBTn = new QPushButton(verticalLayoutWidget);
        ToDicBTn->setObjectName("ToDicBTn");
        ToDicBTn->setFont(font);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../zqt/icons/leftarrowbluelight.png"), QSize(), QIcon::Normal, QIcon::Off);
        ToDicBTn->setIcon(icon1);
        ToDicBTn->setIconSize(QSize(26, 26));

        horizontalLayout_2->addWidget(ToDicBTn);

        QuitBTn = new QPushButton(verticalLayoutWidget);
        QuitBTn->setObjectName("QuitBTn");

        horizontalLayout_2->addWidget(QuitBTn);


        verticalLayout->addLayout(horizontalLayout_2);

        RawFields->setCentralWidget(centralwidget);
        menubar = new QMenuBar(RawFields);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1153, 20));
        GeneralMEn = new QMenu(menubar);
        GeneralMEn->setObjectName("GeneralMEn");
        RawFields->setMenuBar(menubar);
        statusBar = new QStatusBar(RawFields);
        statusBar->setObjectName("statusBar");
        RawFields->setStatusBar(statusBar);

        menubar->addAction(GeneralMEn->menuAction());

        retranslateUi(RawFields);

        QMetaObject::connectSlotsByName(RawFields);
    } // setupUi

    void retranslateUi(QMainWindow *RawFields)
    {
        RawFields->setWindowTitle(QCoreApplication::translate("RawFields", "MainWindow", nullptr));
        ClassNameLBl->setText(QCoreApplication::translate("RawFields", "<no class>", nullptr));
        label_4->setText(QCoreApplication::translate("RawFields", "Fields", nullptr));
        CppFileNameLBl->setText(QCoreApplication::translate("RawFields", "CppFileNameLBl", nullptr));
        FieldsNbLBl->setText(QCoreApplication::translate("RawFields", "<no fields>", nullptr));
        label_3->setText(QCoreApplication::translate("RawFields", "Class", nullptr));
        label_6->setText(QCoreApplication::translate("RawFields", "Modified", nullptr));
        DateLBl->setText(QCoreApplication::translate("RawFields", "../../.. .. .. ..", nullptr));
        label_5->setText(QCoreApplication::translate("RawFields", "Parsing result", nullptr));
        label_2->setText(QCoreApplication::translate("RawFields", "Selected fields", nullptr));
#if QT_CONFIG(tooltip)
        ParseBTn->setToolTip(QCoreApplication::translate("RawFields", "Parse loaded code file and generate fields", nullptr));
#endif // QT_CONFIG(tooltip)
        ParseBTn->setText(QString());
#if QT_CONFIG(tooltip)
        ToDicBTn->setToolTip(QCoreApplication::translate("RawFields", "Transfer all selected fields to dictionary", nullptr));
#endif // QT_CONFIG(tooltip)
        ToDicBTn->setText(QString());
        QuitBTn->setText(QCoreApplication::translate("RawFields", "Hide", nullptr));
        GeneralMEn->setTitle(QCoreApplication::translate("RawFields", "General", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RawFields: public Ui_RawFields {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAWFIELDS_H
