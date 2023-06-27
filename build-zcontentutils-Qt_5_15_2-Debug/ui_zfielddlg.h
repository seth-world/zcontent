/********************************************************************************
** Form generated from reading UI file 'zfielddlg.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZFIELDDLG_H
#define UI_ZFIELDDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZFieldDLg
{
public:
    QLabel *fieldlabel;
    QLineEdit *ZTypeLEd;
    QLabel *label;
    QLineEdit *ZTypeStrLEd;
    QLineEdit *FieldNameLEd;
    QPushButton *DiscardBTn;
    QPushButton *AcceptBTn;
    QWidget *ZTypeListWDg;
    QLabel *label_8;
    QLineEdit *TooltipLEd;
    QFrame *SizeFRm;
    QLabel *label_3;
    QLineEdit *HeaderSizeLEd;
    QLabel *label_2;
    QLineEdit *NaturalLEd;
    QLabel *label_4;
    QLineEdit *UniversalLEd;
    QLabel *label_5;
    QLineEdit *CapacityLEd;
    QCheckBox *KeyEligibleCHk;
    QLabel *label_7;
    QPushButton *CreateArrayBTn;
    QLabel *KeyEligibleLBl;
    QLabel *CommentLBl;
    QFrame *HashcodeFRm;
    QPushButton *ValidateHashBTn;
    QLineEdit *HashcodeLEd;
    QPushButton *ComputeBTn;
    QLabel *label_6;

    void setupUi(QDialog *ZFieldDLg)
    {
        if (ZFieldDLg->objectName().isEmpty())
            ZFieldDLg->setObjectName(QString::fromUtf8("ZFieldDLg"));
        ZFieldDLg->resize(491, 440);
        fieldlabel = new QLabel(ZFieldDLg);
        fieldlabel->setObjectName(QString::fromUtf8("fieldlabel"));
        fieldlabel->setGeometry(QRect(10, 10, 81, 16));
        ZTypeLEd = new QLineEdit(ZFieldDLg);
        ZTypeLEd->setObjectName(QString::fromUtf8("ZTypeLEd"));
        ZTypeLEd->setGeometry(QRect(80, 50, 101, 23));
        ZTypeLEd->setReadOnly(true);
        label = new QLabel(ZFieldDLg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(310, 30, 101, 16));
        ZTypeStrLEd = new QLineEdit(ZFieldDLg);
        ZTypeStrLEd->setObjectName(QString::fromUtf8("ZTypeStrLEd"));
        ZTypeStrLEd->setGeometry(QRect(190, 50, 271, 23));
        ZTypeStrLEd->setReadOnly(true);
        FieldNameLEd = new QLineEdit(ZFieldDLg);
        FieldNameLEd->setObjectName(QString::fromUtf8("FieldNameLEd"));
        FieldNameLEd->setGeometry(QRect(110, 10, 171, 23));
        DiscardBTn = new QPushButton(ZFieldDLg);
        DiscardBTn->setObjectName(QString::fromUtf8("DiscardBTn"));
        DiscardBTn->setGeometry(QRect(320, 400, 71, 23));
        AcceptBTn = new QPushButton(ZFieldDLg);
        AcceptBTn->setObjectName(QString::fromUtf8("AcceptBTn"));
        AcceptBTn->setGeometry(QRect(400, 400, 71, 23));
        ZTypeListWDg = new QWidget(ZFieldDLg);
        ZTypeListWDg->setObjectName(QString::fromUtf8("ZTypeListWDg"));
        ZTypeListWDg->setGeometry(QRect(10, 40, 61, 41));
        label_8 = new QLabel(ZFieldDLg);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 120, 61, 16));
        TooltipLEd = new QLineEdit(ZFieldDLg);
        TooltipLEd->setObjectName(QString::fromUtf8("TooltipLEd"));
        TooltipLEd->setGeometry(QRect(80, 120, 391, 23));
        SizeFRm = new QFrame(ZFieldDLg);
        SizeFRm->setObjectName(QString::fromUtf8("SizeFRm"));
        SizeFRm->setGeometry(QRect(10, 150, 461, 141));
        SizeFRm->setFrameShape(QFrame::StyledPanel);
        SizeFRm->setFrameShadow(QFrame::Raised);
        label_3 = new QLabel(SizeFRm);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 20, 141, 23));
        HeaderSizeLEd = new QLineEdit(SizeFRm);
        HeaderSizeLEd->setObjectName(QString::fromUtf8("HeaderSizeLEd"));
        HeaderSizeLEd->setGeometry(QRect(118, 20, 91, 23));
        HeaderSizeLEd->setReadOnly(true);
        label_2 = new QLabel(SizeFRm);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(230, 20, 81, 23));
        NaturalLEd = new QLineEdit(SizeFRm);
        NaturalLEd->setObjectName(QString::fromUtf8("NaturalLEd"));
        NaturalLEd->setGeometry(QRect(348, 20, 101, 23));
        label_4 = new QLabel(SizeFRm);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(230, 50, 121, 23));
        UniversalLEd = new QLineEdit(SizeFRm);
        UniversalLEd->setObjectName(QString::fromUtf8("UniversalLEd"));
        UniversalLEd->setGeometry(QRect(348, 50, 101, 23));
        label_5 = new QLabel(SizeFRm);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 50, 121, 23));
        CapacityLEd = new QLineEdit(SizeFRm);
        CapacityLEd->setObjectName(QString::fromUtf8("CapacityLEd"));
        CapacityLEd->setGeometry(QRect(120, 50, 91, 23));
        KeyEligibleCHk = new QCheckBox(SizeFRm);
        KeyEligibleCHk->setObjectName(QString::fromUtf8("KeyEligibleCHk"));
        KeyEligibleCHk->setEnabled(true);
        KeyEligibleCHk->setGeometry(QRect(130, 90, 16, 16));
        label_7 = new QLabel(SizeFRm);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 90, 91, 16));
        CreateArrayBTn = new QPushButton(SizeFRm);
        CreateArrayBTn->setObjectName(QString::fromUtf8("CreateArrayBTn"));
        CreateArrayBTn->setGeometry(QRect(10, 110, 181, 23));
        KeyEligibleLBl = new QLabel(SizeFRm);
        KeyEligibleLBl->setObjectName(QString::fromUtf8("KeyEligibleLBl"));
        KeyEligibleLBl->setGeometry(QRect(170, 90, 281, 16));
        CommentLBl = new QLabel(ZFieldDLg);
        CommentLBl->setObjectName(QString::fromUtf8("CommentLBl"));
        CommentLBl->setGeometry(QRect(10, 90, 451, 16));
        HashcodeFRm = new QFrame(ZFieldDLg);
        HashcodeFRm->setObjectName(QString::fromUtf8("HashcodeFRm"));
        HashcodeFRm->setGeometry(QRect(10, 310, 461, 81));
        HashcodeFRm->setFrameShape(QFrame::StyledPanel);
        HashcodeFRm->setFrameShadow(QFrame::Raised);
        ValidateHashBTn = new QPushButton(HashcodeFRm);
        ValidateHashBTn->setObjectName(QString::fromUtf8("ValidateHashBTn"));
        ValidateHashBTn->setGeometry(QRect(20, 20, 131, 23));
        HashcodeLEd = new QLineEdit(HashcodeFRm);
        HashcodeLEd->setObjectName(QString::fromUtf8("HashcodeLEd"));
        HashcodeLEd->setGeometry(QRect(100, 50, 321, 23));
        HashcodeLEd->setReadOnly(true);
        ComputeBTn = new QPushButton(HashcodeFRm);
        ComputeBTn->setObjectName(QString::fromUtf8("ComputeBTn"));
        ComputeBTn->setGeometry(QRect(170, 20, 131, 23));
        label_6 = new QLabel(HashcodeFRm);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 50, 91, 16));
        QWidget::setTabOrder(FieldNameLEd, AcceptBTn);
        QWidget::setTabOrder(AcceptBTn, DiscardBTn);
        QWidget::setTabOrder(DiscardBTn, ZTypeStrLEd);
        QWidget::setTabOrder(ZTypeStrLEd, ZTypeLEd);

        retranslateUi(ZFieldDLg);

        QMetaObject::connectSlotsByName(ZFieldDLg);
    } // setupUi

    void retranslateUi(QDialog *ZFieldDLg)
    {
        ZFieldDLg->setWindowTitle(QCoreApplication::translate("ZFieldDLg", "Dialog", nullptr));
        fieldlabel->setText(QCoreApplication::translate("ZFieldDLg", "Field name", nullptr));
        label->setText(QCoreApplication::translate("ZFieldDLg", "Data type", nullptr));
        DiscardBTn->setText(QCoreApplication::translate("ZFieldDLg", "Discard", nullptr));
        AcceptBTn->setText(QCoreApplication::translate("ZFieldDLg", "Accept", nullptr));
        label_8->setText(QCoreApplication::translate("ZFieldDLg", "Tooltip", nullptr));
        label_3->setText(QCoreApplication::translate("ZFieldDLg", "Header size", nullptr));
        label_2->setText(QCoreApplication::translate("ZFieldDLg", "Natural size", nullptr));
        label_4->setText(QCoreApplication::translate("ZFieldDLg", "Universal size", nullptr));
        label_5->setText(QCoreApplication::translate("ZFieldDLg", "Capacity", nullptr));
#if QT_CONFIG(tooltip)
        KeyEligibleCHk->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        KeyEligibleCHk->setText(QString());
#if QT_CONFIG(tooltip)
        label_7->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        label_7->setText(QCoreApplication::translate("ZFieldDLg", "Key eligible", nullptr));
        CreateArrayBTn->setText(QCoreApplication::translate("ZFieldDLg", "Create array from capacity", nullptr));
#if QT_CONFIG(tooltip)
        KeyEligibleLBl->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        KeyEligibleLBl->setText(QString());
        CommentLBl->setText(QCoreApplication::translate("ZFieldDLg", "Comment", nullptr));
        ValidateHashBTn->setText(QCoreApplication::translate("ZFieldDLg", "Validate hashcode", nullptr));
        ComputeBTn->setText(QCoreApplication::translate("ZFieldDLg", "Compute hashcode", nullptr));
        label_6->setText(QCoreApplication::translate("ZFieldDLg", "Hashcode", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZFieldDLg: public Ui_ZFieldDLg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZFIELDDLG_H
