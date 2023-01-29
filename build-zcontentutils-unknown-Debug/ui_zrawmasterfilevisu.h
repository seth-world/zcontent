/********************************************************************************
** Form generated from reading UI file 'zrawmasterfilevisu.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZRAWMASTERFILEVISU_H
#define UI_ZRAWMASTERFILEVISU_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ZRawMasterFileVisu
{
public:
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *BeginLBl;
    QLabel *label_2;
    QLabel *EndLBl;
    QLabel *label_3;
    QLabel *StateLBl;
    QPushButton *BackwardBTn;
    QPushButton *ForwardBTn;
    QPushButton *BeginBTn;
    QPushButton *EndBTn;
    QLabel *label_4;
    QLabel *FileSizeLBl;
    QLabel *TopLBl;
    QLabel *BottomLBl;
    QLabel *URFSizeLabelLBl;
    QLabel *URFSizeLBl;
    QComboBox *ViewModeCBx;
    QLabel *FieldsNbLBl;
    QGroupBox *ColorsGBx;
    QLabel *HeaderColorLBl;
    QLabel *PresenceColorLBl;
    QLabel *URFSizeColorLBl;
    QLabel *WrongColorLBl;
    QLabel *StartColorLBl;
    QComboBox *ColorModeCBx;
    QGroupBox *FieldPresenceGBx;
    QLabel *BitsetHeaderLBl;
    QLabel *BitsetContentLBl;
    QPlainTextEdit *MessagePTe;

    void setupUi(QDialog *ZRawMasterFileVisu)
    {
        if (ZRawMasterFileVisu->objectName().isEmpty())
            ZRawMasterFileVisu->setObjectName(QString::fromUtf8("ZRawMasterFileVisu"));
        ZRawMasterFileVisu->resize(684, 546);
        groupBox = new QGroupBox(ZRawMasterFileVisu);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 671, 151));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(150, 30, 54, 15));
        BeginLBl = new QLabel(groupBox);
        BeginLBl->setObjectName(QString::fromUtf8("BeginLBl"));
        BeginLBl->setGeometry(QRect(210, 30, 101, 16));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(150, 50, 54, 15));
        EndLBl = new QLabel(groupBox);
        EndLBl->setObjectName(QString::fromUtf8("EndLBl"));
        EndLBl->setGeometry(QRect(210, 50, 111, 16));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(320, 30, 54, 15));
        StateLBl = new QLabel(groupBox);
        StateLBl->setObjectName(QString::fromUtf8("StateLBl"));
        StateLBl->setGeometry(QRect(380, 30, 81, 16));
        BackwardBTn = new QPushButton(groupBox);
        BackwardBTn->setObjectName(QString::fromUtf8("BackwardBTn"));
        BackwardBTn->setGeometry(QRect(530, 40, 31, 23));
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../zqt/icons/backward.gif"), QSize(), QIcon::Normal, QIcon::Off);
        BackwardBTn->setIcon(icon);
        ForwardBTn = new QPushButton(groupBox);
        ForwardBTn->setObjectName(QString::fromUtf8("ForwardBTn"));
        ForwardBTn->setGeometry(QRect(570, 40, 31, 23));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../zqt/icons/forward.gif"), QSize(), QIcon::Normal, QIcon::Off);
        ForwardBTn->setIcon(icon1);
        BeginBTn = new QPushButton(groupBox);
        BeginBTn->setObjectName(QString::fromUtf8("BeginBTn"));
        BeginBTn->setGeometry(QRect(490, 40, 31, 23));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("../../zqt/icons/tobegin.gif"), QSize(), QIcon::Normal, QIcon::Off);
        BeginBTn->setIcon(icon2);
        EndBTn = new QPushButton(groupBox);
        EndBTn->setObjectName(QString::fromUtf8("EndBTn"));
        EndBTn->setGeometry(QRect(610, 40, 31, 23));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("../../zqt/icons/toend.gif"), QSize(), QIcon::Normal, QIcon::Off);
        EndBTn->setIcon(icon3);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 30, 54, 15));
        FileSizeLBl = new QLabel(groupBox);
        FileSizeLBl->setObjectName(QString::fromUtf8("FileSizeLBl"));
        FileSizeLBl->setGeometry(QRect(50, 30, 101, 16));
        TopLBl = new QLabel(groupBox);
        TopLBl->setObjectName(QString::fromUtf8("TopLBl"));
        TopLBl->setGeometry(QRect(490, 20, 141, 16));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        TopLBl->setFont(font);
        TopLBl->setAlignment(Qt::AlignCenter);
        BottomLBl = new QLabel(groupBox);
        BottomLBl->setObjectName(QString::fromUtf8("BottomLBl"));
        BottomLBl->setGeometry(QRect(490, 20, 141, 16));
        BottomLBl->setFont(font);
        BottomLBl->setAlignment(Qt::AlignCenter);
        URFSizeLabelLBl = new QLabel(groupBox);
        URFSizeLabelLBl->setObjectName(QString::fromUtf8("URFSizeLabelLBl"));
        URFSizeLabelLBl->setGeometry(QRect(320, 50, 54, 15));
        URFSizeLBl = new QLabel(groupBox);
        URFSizeLBl->setObjectName(QString::fromUtf8("URFSizeLBl"));
        URFSizeLBl->setGeometry(QRect(380, 50, 81, 16));
        ViewModeCBx = new QComboBox(groupBox);
        ViewModeCBx->setObjectName(QString::fromUtf8("ViewModeCBx"));
        ViewModeCBx->setGeometry(QRect(490, 70, 151, 23));
        FieldsNbLBl = new QLabel(groupBox);
        FieldsNbLBl->setObjectName(QString::fromUtf8("FieldsNbLBl"));
        FieldsNbLBl->setGeometry(QRect(80, 50, 31, 16));
        ColorsGBx = new QGroupBox(groupBox);
        ColorsGBx->setObjectName(QString::fromUtf8("ColorsGBx"));
        ColorsGBx->setGeometry(QRect(320, 50, 161, 51));
        HeaderColorLBl = new QLabel(ColorsGBx);
        HeaderColorLBl->setObjectName(QString::fromUtf8("HeaderColorLBl"));
        HeaderColorLBl->setGeometry(QRect(70, 30, 21, 21));
        PresenceColorLBl = new QLabel(ColorsGBx);
        PresenceColorLBl->setObjectName(QString::fromUtf8("PresenceColorLBl"));
        PresenceColorLBl->setGeometry(QRect(100, 30, 21, 21));
        URFSizeColorLBl = new QLabel(ColorsGBx);
        URFSizeColorLBl->setObjectName(QString::fromUtf8("URFSizeColorLBl"));
        URFSizeColorLBl->setGeometry(QRect(130, 30, 21, 21));
        WrongColorLBl = new QLabel(ColorsGBx);
        WrongColorLBl->setObjectName(QString::fromUtf8("WrongColorLBl"));
        WrongColorLBl->setGeometry(QRect(10, 30, 21, 21));
        StartColorLBl = new QLabel(ColorsGBx);
        StartColorLBl->setObjectName(QString::fromUtf8("StartColorLBl"));
        StartColorLBl->setGeometry(QRect(40, 30, 21, 21));
        ColorModeCBx = new QComboBox(groupBox);
        ColorModeCBx->setObjectName(QString::fromUtf8("ColorModeCBx"));
        ColorModeCBx->setGeometry(QRect(490, 100, 151, 23));
        FieldPresenceGBx = new QGroupBox(groupBox);
        FieldPresenceGBx->setObjectName(QString::fromUtf8("FieldPresenceGBx"));
        FieldPresenceGBx->setGeometry(QRect(10, 90, 401, 61));
        BitsetHeaderLBl = new QLabel(FieldPresenceGBx);
        BitsetHeaderLBl->setObjectName(QString::fromUtf8("BitsetHeaderLBl"));
        BitsetHeaderLBl->setGeometry(QRect(0, 20, 401, 16));
        BitsetContentLBl = new QLabel(FieldPresenceGBx);
        BitsetContentLBl->setObjectName(QString::fromUtf8("BitsetContentLBl"));
        BitsetContentLBl->setGeometry(QRect(0, 40, 391, 16));
        MessagePTe = new QPlainTextEdit(ZRawMasterFileVisu);
        MessagePTe->setObjectName(QString::fromUtf8("MessagePTe"));
        MessagePTe->setGeometry(QRect(0, 150, 661, 41));

        retranslateUi(ZRawMasterFileVisu);

        QMetaObject::connectSlotsByName(ZRawMasterFileVisu);
    } // setupUi

    void retranslateUi(QDialog *ZRawMasterFileVisu)
    {
        ZRawMasterFileVisu->setWindowTitle(QCoreApplication::translate("ZRawMasterFileVisu", "Dialog", nullptr));
        groupBox->setTitle(QString());
        label->setText(QCoreApplication::translate("ZRawMasterFileVisu", "Address", nullptr));
        BeginLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "TextLabel", nullptr));
        label_2->setText(QCoreApplication::translate("ZRawMasterFileVisu", "End", nullptr));
        EndLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "TextLabel", nullptr));
        label_3->setText(QCoreApplication::translate("ZRawMasterFileVisu", "State", nullptr));
        StateLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "TextLabel", nullptr));
        BackwardBTn->setText(QString());
        ForwardBTn->setText(QString());
        BeginBTn->setText(QString());
        EndBTn->setText(QString());
        label_4->setText(QCoreApplication::translate("ZRawMasterFileVisu", "Size", nullptr));
        FileSizeLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "TextLabel", nullptr));
        TopLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "Top", nullptr));
        BottomLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "Bottom", nullptr));
        URFSizeLabelLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "URF size", nullptr));
        URFSizeLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "TextLabel", nullptr));
        FieldsNbLBl->setText(QString());
        ColorsGBx->setTitle(QString());
        HeaderColorLBl->setText(QString());
        PresenceColorLBl->setText(QString());
        URFSizeColorLBl->setText(QString());
        WrongColorLBl->setText(QString());
        StartColorLBl->setText(QString());
        FieldPresenceGBx->setTitle(QCoreApplication::translate("ZRawMasterFileVisu", "Field presence", nullptr));
        BitsetHeaderLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "01234567890123456789", nullptr));
        BitsetContentLBl->setText(QCoreApplication::translate("ZRawMasterFileVisu", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZRawMasterFileVisu: public Ui_ZRawMasterFileVisu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZRAWMASTERFILEVISU_H
