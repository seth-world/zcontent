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
    QLabel *LogoLBl;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *VertLayoutMsgVLy;
    QLabel *TitleLBl;
    QTextEdit *MessageTEd;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QCheckBox *dontShowCKb;
    QSpacerItem *horizontalSpacer;
    QPushButton *MoreBTn;
    QPushButton *OtherBTn;
    QPushButton *OKBTn;
    QPushButton *CancelBTn;
    QFrame *ExceptionFRm;
    QLabel *ZStatusLBl;
    QLabel *label_3;
    QLabel *ModuleLBl;
    QLabel *SeverityLBl;
    QLabel *label_5;
    QLabel *label_4;

    void setupUi(QDialog *ZExceptionDLg)
    {
        if (ZExceptionDLg->objectName().isEmpty())
            ZExceptionDLg->setObjectName(QString::fromUtf8("ZExceptionDLg"));
        ZExceptionDLg->resize(560, 270);
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
        verticalLayoutWidget = new QWidget(ZExceptionDLg);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 100, 541, 111));
        VertLayoutMsgVLy = new QVBoxLayout(verticalLayoutWidget);
        VertLayoutMsgVLy->setObjectName(QString::fromUtf8("VertLayoutMsgVLy"));
        VertLayoutMsgVLy->setContentsMargins(0, 0, 0, 0);
        TitleLBl = new QLabel(verticalLayoutWidget);
        TitleLBl->setObjectName(QString::fromUtf8("TitleLBl"));
        TitleLBl->setAlignment(Qt::AlignCenter);

        VertLayoutMsgVLy->addWidget(TitleLBl);

        MessageTEd = new QTextEdit(verticalLayoutWidget);
        MessageTEd->setObjectName(QString::fromUtf8("MessageTEd"));

        VertLayoutMsgVLy->addWidget(MessageTEd);

        layoutWidget = new QWidget(ZExceptionDLg);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 230, 531, 25));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
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

        ExceptionFRm = new QFrame(ZExceptionDLg);
        ExceptionFRm->setObjectName(QString::fromUtf8("ExceptionFRm"));
        ExceptionFRm->setGeometry(QRect(10, 10, 391, 81));
        ExceptionFRm->setFrameShape(QFrame::StyledPanel);
        ExceptionFRm->setFrameShadow(QFrame::Raised);
        ZStatusLBl = new QLabel(ExceptionFRm);
        ZStatusLBl->setObjectName(QString::fromUtf8("ZStatusLBl"));
        ZStatusLBl->setGeometry(QRect(80, 30, 151, 16));
        label_3 = new QLabel(ExceptionFRm);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 10, 54, 15));
        ModuleLBl = new QLabel(ExceptionFRm);
        ModuleLBl->setObjectName(QString::fromUtf8("ModuleLBl"));
        ModuleLBl->setGeometry(QRect(80, 10, 301, 16));
        SeverityLBl = new QLabel(ExceptionFRm);
        SeverityLBl->setObjectName(QString::fromUtf8("SeverityLBl"));
        SeverityLBl->setGeometry(QRect(80, 50, 141, 16));
        label_5 = new QLabel(ExceptionFRm);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 50, 54, 15));
        label_4 = new QLabel(ExceptionFRm);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 30, 54, 15));

        retranslateUi(ZExceptionDLg);

        QMetaObject::connectSlotsByName(ZExceptionDLg);
    } // setupUi

    void retranslateUi(QDialog *ZExceptionDLg)
    {
        ZExceptionDLg->setWindowTitle(QCoreApplication::translate("ZExceptionDLg", "Dialog", nullptr));
        LogoLBl->setText(QCoreApplication::translate("ZExceptionDLg", "logo", nullptr));
        TitleLBl->setText(QCoreApplication::translate("ZExceptionDLg", "Main message", nullptr));
        dontShowCKb->setText(QCoreApplication::translate("ZExceptionDLg", "Don't show again", nullptr));
        MoreBTn->setText(QCoreApplication::translate("ZExceptionDLg", "More", nullptr));
        OtherBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Other", nullptr));
        OKBTn->setText(QCoreApplication::translate("ZExceptionDLg", "OK", nullptr));
        CancelBTn->setText(QCoreApplication::translate("ZExceptionDLg", "Cancel", nullptr));
        ZStatusLBl->setText(QCoreApplication::translate("ZExceptionDLg", "ZStatus", nullptr));
        label_3->setText(QCoreApplication::translate("ZExceptionDLg", "Module", nullptr));
        ModuleLBl->setText(QCoreApplication::translate("ZExceptionDLg", "module", nullptr));
        SeverityLBl->setText(QCoreApplication::translate("ZExceptionDLg", "Severity", nullptr));
        label_5->setText(QCoreApplication::translate("ZExceptionDLg", "Severity", nullptr));
        label_4->setText(QCoreApplication::translate("ZExceptionDLg", "Status", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZExceptionDLg: public Ui_ZExceptionDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZEXCEPTIONDLG_H
