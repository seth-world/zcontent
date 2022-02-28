/********************************************************************************
** Form generated from reading UI file 'zexceptiondlg_old.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZEXCEPTIONDLG_OLD_H
#define UI_ZEXCEPTIONDLG_OLD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZExceptionDLg
{
public:
    QLabel *LogoLBl;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTextEdit *MessageTEd;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QCheckBox *dontShowCKb;
    QSpacerItem *horizontalSpacer;
    QPushButton *MoreBTn;
    QPushButton *OtherBTn;
    QPushButton *OKBTn;
    QPushButton *CancelBTn;
    QWidget *layoutWidget1;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLabel *ModuleLBl;
    QLabel *label_4;
    QLabel *ZStatusLBl;
    QLabel *label_5;
    QLabel *SeverityLBl;

    void setupUi(QDialog *ZExceptionDLg)
    {
        if (ZExceptionDLg->objectName().isEmpty())
            ZExceptionDLg->setObjectName(QString::fromUtf8("ZExceptionDLg"));
        ZExceptionDLg->resize(560, 249);
        LogoLBl = new QLabel(ZExceptionDLg);
        LogoLBl->setObjectName(QString::fromUtf8("LogoLBl"));
        LogoLBl->setGeometry(QRect(460, 10, 51, 51));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LogoLBl->sizePolicy().hasHeightForWidth());
        LogoLBl->setSizePolicy(sizePolicy);
        LogoLBl->setAutoFillBackground(true);
        LogoLBl->setFrameShape(QFrame::Box);
        LogoLBl->setScaledContents(true);
        layoutWidget = new QWidget(ZExceptionDLg);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 80, 513, 151));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        MessageTEd = new QTextEdit(layoutWidget);
        MessageTEd->setObjectName(QString::fromUtf8("MessageTEd"));

        verticalLayout->addWidget(MessageTEd);


        verticalLayout_2->addLayout(verticalLayout);

        verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        dontShowCKb = new QCheckBox(layoutWidget);
        dontShowCKb->setObjectName(QString::fromUtf8("dontShowCKb"));

        horizontalLayout->addWidget(dontShowCKb);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        MoreBTn = new QPushButton(layoutWidget);
        MoreBTn->setObjectName(QString::fromUtf8("MoreBTn"));

        horizontalLayout->addWidget(MoreBTn);

        OtherBTn = new QPushButton(layoutWidget);
        OtherBTn->setObjectName(QString::fromUtf8("OtherBTn"));

        horizontalLayout->addWidget(OtherBTn);

        OKBTn = new QPushButton(layoutWidget);
        OKBTn->setObjectName(QString::fromUtf8("OKBTn"));

        horizontalLayout->addWidget(OKBTn);

        CancelBTn = new QPushButton(layoutWidget);
        CancelBTn->setObjectName(QString::fromUtf8("CancelBTn"));

        horizontalLayout->addWidget(CancelBTn);


        verticalLayout_2->addLayout(horizontalLayout);

        layoutWidget1 = new QWidget(ZExceptionDLg);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 10, 211, 59));
        formLayout = new QFormLayout(layoutWidget1);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        ModuleLBl = new QLabel(layoutWidget1);
        ModuleLBl->setObjectName(QString::fromUtf8("ModuleLBl"));

        formLayout->setWidget(0, QFormLayout::FieldRole, ModuleLBl);

        label_4 = new QLabel(layoutWidget1);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_4);

        ZStatusLBl = new QLabel(layoutWidget1);
        ZStatusLBl->setObjectName(QString::fromUtf8("ZStatusLBl"));

        formLayout->setWidget(1, QFormLayout::FieldRole, ZStatusLBl);

        label_5 = new QLabel(layoutWidget1);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        SeverityLBl = new QLabel(layoutWidget1);
        SeverityLBl->setObjectName(QString::fromUtf8("SeverityLBl"));

        formLayout->setWidget(2, QFormLayout::FieldRole, SeverityLBl);


        retranslateUi(ZExceptionDLg);

        QMetaObject::connectSlotsByName(ZExceptionDLg);
    } // setupUi

    void retranslateUi(QDialog *ZExceptionDLg)
    {
        ZExceptionDLg->setWindowTitle(QCoreApplication::translate("ZExceptionDLg", "Dialog", nullptr));
        LogoLBl->setText(QCoreApplication::translate("ZExceptionDLg", "icon", nullptr));
        label->setText(QCoreApplication::translate("ZExceptionDLg", "Main message", nullptr));
        dontShowCKb->setText(QCoreApplication::translate("ZExceptionDLg", "Don't show again", nullptr));
        MoreBTn->setText(QCoreApplication::translate("ZExceptionDLg", "More", nullptr));
        OtherBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Other", nullptr));
        OKBTn->setText(QCoreApplication::translate("ZExceptionDLg", "OK", nullptr));
        CancelBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Cancel", nullptr));
        label_3->setText(QCoreApplication::translate("ZExceptionDLg", "Module", nullptr));
        ModuleLBl->setText(QCoreApplication::translate("ZExceptionDLg", "module", nullptr));
        label_4->setText(QCoreApplication::translate("ZExceptionDLg", "Status", nullptr));
        ZStatusLBl->setText(QCoreApplication::translate("ZExceptionDLg", "ZStatus", nullptr));
        label_5->setText(QCoreApplication::translate("ZExceptionDLg", "Severity", nullptr));
        SeverityLBl->setText(QCoreApplication::translate("ZExceptionDLg", "Severity", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZExceptionDLg: public Ui_ZExceptionDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZEXCEPTIONDLG_OLD_H
