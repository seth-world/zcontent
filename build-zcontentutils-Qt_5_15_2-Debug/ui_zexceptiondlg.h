/********************************************************************************
** Form generated from reading UI file 'zexceptiondlg.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZEXCEPTIONDLG_H
#define UI_ZEXCEPTIONDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
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
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *dontShowCKb;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *ErrlogBTn;
    QPushButton *MoreBTn;
    QPushButton *OtherBTn;
    QPushButton *OKBTn;
    QPushButton *CancelBTn;
    QLabel *LogoLBl;
    QFrame *ExceptionFRm;
    QLabel *ZStatusLBl;
    QLabel *label_6;
    QLabel *ModuleLBl;
    QLabel *SeverityLBl;
    QLabel *label_7;
    QLabel *label_8;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *VertLayoutMsgVLy_2;
    QLabel *TitleLBl;
    QTextEdit *MessageTEd;

    void setupUi(QDialog *ZExceptionDLg)
    {
        if (ZExceptionDLg->objectName().isEmpty())
            ZExceptionDLg->setObjectName(QString::fromUtf8("ZExceptionDLg"));
        ZExceptionDLg->resize(587, 349);
        layoutWidget = new QWidget(ZExceptionDLg);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 300, 557, 25));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        dontShowCKb = new QCheckBox(layoutWidget);
        dontShowCKb->setObjectName(QString::fromUtf8("dontShowCKb"));

        horizontalLayout_2->addWidget(dontShowCKb);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        ErrlogBTn = new QPushButton(layoutWidget);
        ErrlogBTn->setObjectName(QString::fromUtf8("ErrlogBTn"));

        horizontalLayout_2->addWidget(ErrlogBTn);

        MoreBTn = new QPushButton(layoutWidget);
        MoreBTn->setObjectName(QString::fromUtf8("MoreBTn"));

        horizontalLayout_2->addWidget(MoreBTn);

        OtherBTn = new QPushButton(layoutWidget);
        OtherBTn->setObjectName(QString::fromUtf8("OtherBTn"));

        horizontalLayout_2->addWidget(OtherBTn);

        OKBTn = new QPushButton(layoutWidget);
        OKBTn->setObjectName(QString::fromUtf8("OKBTn"));

        horizontalLayout_2->addWidget(OKBTn);

        CancelBTn = new QPushButton(layoutWidget);
        CancelBTn->setObjectName(QString::fromUtf8("CancelBTn"));

        horizontalLayout_2->addWidget(CancelBTn);

        LogoLBl = new QLabel(ZExceptionDLg);
        LogoLBl->setObjectName(QString::fromUtf8("LogoLBl"));
        LogoLBl->setGeometry(QRect(410, 10, 61, 61));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LogoLBl->sizePolicy().hasHeightForWidth());
        LogoLBl->setSizePolicy(sizePolicy);
        LogoLBl->setAutoFillBackground(false);
        LogoLBl->setStyleSheet(QString::fromUtf8("background:transparent"));
        LogoLBl->setFrameShape(QFrame::StyledPanel);
        LogoLBl->setScaledContents(true);
        ExceptionFRm = new QFrame(ZExceptionDLg);
        ExceptionFRm->setObjectName(QString::fromUtf8("ExceptionFRm"));
        ExceptionFRm->setGeometry(QRect(10, 10, 391, 81));
        ExceptionFRm->setFrameShape(QFrame::StyledPanel);
        ExceptionFRm->setFrameShadow(QFrame::Raised);
        ZStatusLBl = new QLabel(ExceptionFRm);
        ZStatusLBl->setObjectName(QString::fromUtf8("ZStatusLBl"));
        ZStatusLBl->setGeometry(QRect(80, 30, 151, 16));
        label_6 = new QLabel(ExceptionFRm);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 10, 54, 15));
        ModuleLBl = new QLabel(ExceptionFRm);
        ModuleLBl->setObjectName(QString::fromUtf8("ModuleLBl"));
        ModuleLBl->setGeometry(QRect(80, 10, 301, 16));
        SeverityLBl = new QLabel(ExceptionFRm);
        SeverityLBl->setObjectName(QString::fromUtf8("SeverityLBl"));
        SeverityLBl->setGeometry(QRect(80, 50, 141, 16));
        label_7 = new QLabel(ExceptionFRm);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 50, 54, 15));
        label_8 = new QLabel(ExceptionFRm);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 30, 54, 15));
        verticalLayoutWidget = new QWidget(ZExceptionDLg);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 100, 551, 191));
        VertLayoutMsgVLy_2 = new QVBoxLayout(verticalLayoutWidget);
        VertLayoutMsgVLy_2->setObjectName(QString::fromUtf8("VertLayoutMsgVLy_2"));
        VertLayoutMsgVLy_2->setContentsMargins(0, 0, 0, 0);
        TitleLBl = new QLabel(verticalLayoutWidget);
        TitleLBl->setObjectName(QString::fromUtf8("TitleLBl"));
        TitleLBl->setAlignment(Qt::AlignCenter);

        VertLayoutMsgVLy_2->addWidget(TitleLBl);

        MessageTEd = new QTextEdit(verticalLayoutWidget);
        MessageTEd->setObjectName(QString::fromUtf8("MessageTEd"));

        VertLayoutMsgVLy_2->addWidget(MessageTEd);


        retranslateUi(ZExceptionDLg);

        QMetaObject::connectSlotsByName(ZExceptionDLg);
    } // setupUi

    void retranslateUi(QDialog *ZExceptionDLg)
    {
        ZExceptionDLg->setWindowTitle(QCoreApplication::translate("ZExceptionDLg", "Dialog", nullptr));
        dontShowCKb->setText(QCoreApplication::translate("ZExceptionDLg", "Don't show again", nullptr));
        ErrlogBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Error log", nullptr));
        MoreBTn->setText(QCoreApplication::translate("ZExceptionDLg", "More", nullptr));
        OtherBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Other", nullptr));
        OKBTn->setText(QCoreApplication::translate("ZExceptionDLg", "OK", nullptr));
        CancelBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Cancel", nullptr));
        LogoLBl->setText(QCoreApplication::translate("ZExceptionDLg", "logo", nullptr));
        ZStatusLBl->setText(QCoreApplication::translate("ZExceptionDLg", "ZStatus", nullptr));
        label_6->setText(QCoreApplication::translate("ZExceptionDLg", "Module", nullptr));
        ModuleLBl->setText(QCoreApplication::translate("ZExceptionDLg", "module", nullptr));
        SeverityLBl->setText(QCoreApplication::translate("ZExceptionDLg", "Severity", nullptr));
        label_7->setText(QCoreApplication::translate("ZExceptionDLg", "Severity", nullptr));
        label_8->setText(QCoreApplication::translate("ZExceptionDLg", "Status", nullptr));
        TitleLBl->setText(QCoreApplication::translate("ZExceptionDLg", "Main message", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZExceptionDLg: public Ui_ZExceptionDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZEXCEPTIONDLG_H
