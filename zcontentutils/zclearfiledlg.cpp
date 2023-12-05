#include "zclearfiledlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSpacerItem>

#include <QGridLayout>

#include <zcontent/zrandomfile/zrandomfile.h>
#include <zcontent/zrandomfile/zfilecontrolblock.h>
#include <zcontent/zrandomfile/zblock.h>
#include <zcontent/zrandomfile/zblockpool.h>
#include <zcontent/zindexedfile/zrawmasterfile.h>


using namespace zbs;

ZClearFileDLg::ZClearFileDLg(QWidget* pParent) : QDialog(pParent)
{
}

void
ZClearFileDLg::initLayout()
{
    setWindowTitle("Clear file");

    QVBoxLayout* QVBL= new QVBoxLayout(this);
    setLayout(QVBL);

    QHBoxLayout* Qhb0 = new QHBoxLayout;
    QVBL->addLayout(Qhb0);
    FileTypeLBl = new QLabel("File",this);
    Qhb0->addWidget(FileTypeLBl);
    FileDescriptionLBl=new QLabel("",this);
    Qhb0->addWidget(FileDescriptionLBl);


    QGridLayout* QGdLy = new QGridLayout(this);
    QVBL->addLayout(QGdLy);
    QLabel* _lbl01 = new QLabel("Current file size",this);
    CurrentFileSizeLBl=new QLabel("0",this);
    QGdLy->addWidget(_lbl01,0,0);
    QGdLy->addWidget(CurrentFileSizeLBl,0,1);

    QLabel* _lbl02 = new QLabel("Target block size",this);
    BlockTargetSizeLBl=new QLabel("0",this);
    QGdLy->addWidget(_lbl02,1,0);
    QGdLy->addWidget(BlockTargetSizeLBl,1,1);

    QLabel* _lbl03 = new QLabel("Number of used blocks",this);
    UsedBlocksLBl=new QLabel("0",this);
    QGdLy->addWidget(_lbl03,2,0);
    QGdLy->addWidget(UsedBlocksLBl,2,1);

    QLabel* _lbl04 = new QLabel("Number of used blocks",this);
    SizeToKeepLEd = new QLineEdit("-1",this);
    QGdLy->addWidget(_lbl04,3,0);
    QGdLy->addWidget(SizeToKeepLEd,3,1);

    QHBoxLayout* Qhb2 = new QHBoxLayout;
    QVBL->addLayout(Qhb2);
    AlignSizeCHk=new QCheckBox("Align to existing size",this);
    Qhb2->addWidget(AlignSizeCHk);

/*
    QHBoxLayout* Qhb1 = new QHBoxLayout;
    QVBL->addLayout(Qhb1);
    QLabel* _lbl01 = new QLabel("Current file size",this);
    Qhb1->addWidget(_lbl01);
    CurrentFileSizeLBl=new QLabel("0",this);
    Qhb1->addWidget(CurrentFileSizeLBl);

    QHBoxLayout* Qhb2 = new QHBoxLayout;
    QVBL->addLayout(Qhb2);
    QLabel* _lbl01 = new QLabel("Total size to keep",this);
    Qhb2->addWidget(_lbl01);
    SizeToKeepLEd=new QLineEdit("0",this);
    Qhb2->addWidget(SizeToKeepLEd);


    QHBoxLayout* Qhb3 = new QHBoxLayout;
    QVBL->addLayout(Qhb3);
    QLabel* _lbl02 = new QLabel("Total size to keep",this);
    Qhb3->addWidget(_lbl02);
    SizeToKeepLEd = new QLineEdit("0",this);
    Qhb3->addWidget(SizeToKeepLEd);
*/
    QHBoxLayout* Qhb3 = new QHBoxLayout;
    QVBL->addLayout(Qhb3);
    HighwaterCHk = new QCheckBox("Highwater marking",this);
    Qhb3->addWidget(HighwaterCHk);


    QHBoxLayout* QhbBtn = new QHBoxLayout;
    QVBL->addLayout(QhbBtn);
    OkBTn = new QPushButton("Go",this);
    CancelBTn = new QPushButton("Cancel",this);
    QhbBtn->addWidget(CancelBTn);
    QhbBtn->addWidget(OkBTn);
    QSpacerItem* spc01 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QhbBtn->addItem(spc01);


    QObject::connect(OkBTn,&QPushButton::clicked,this,&ZClearFileDLg::OkBTnClicked);
    QObject::connect(CancelBTn,&QPushButton::clicked,this,&QDialog::reject);
    QObject::connect(AlignSizeCHk,&QCheckBox::clicked,this,&ZClearFileDLg::AlignSizeCHkChanged);
}

void ZClearFileDLg::OkBTnClicked()
{

}

void ZClearFileDLg::AlignSizeCHkChanged(bool pChecked)
{
    if (pChecked) {
        SizeToKeepLEd->setText("-1");
        SizeToKeepLEd->setEnabled(false);
        return;
    }

    SizeToKeepLEd->setText(CurrentFileSizeLBl->text());
    SizeToKeepLEd->setEnabled(true);
    return;
}

void ZClearFileDLg::setup(ZRawMasterFile* pURIFile)
{
    utf8VaryingString wStr;
    RawMasterFile = pURIFile;
    FileTypeLBl->setText("Master file");
    FileDescriptionLBl->setText(RawMasterFile->getURIContent().toCChar());

    HighwaterCHk->setChecked (RawMasterFile->getFCB()->HighwaterMarking) ;

    size_t wSize=RawMasterFile->getURIContent().getFileSize();
    size_t wNbBlocks = RawMasterFile->getUsedBlocksCount();
    size_t wBlockTargetSize = RawMasterFile->getFCB()->BlockTargetSize;

    wStr.sprintf("%ld",wSize);
    CurrentFileSizeLBl->setText(wStr.toCChar());

    wStr.sprintf("%ld",wBlockTargetSize);
    BlockTargetSizeLBl->setText(wStr.toCChar());

    wStr.sprintf("%ld",wNbBlocks);
    UsedBlocksLBl->setText(wStr.toCChar());
}

void ZClearFileDLg::setup(ZRandomFile* pURIFile)
{
    utf8VaryingString wStr;
    RandomFile = pURIFile;
    FileTypeLBl->setText("Random file");
    FileDescriptionLBl->setText(RandomFile->getURIContent().toCChar());
    HighwaterCHk->setChecked (RandomFile->getFCB()->HighwaterMarking) ;

    size_t wSize=RandomFile->getURIContent().getFileSize();
    size_t wNbBlocks = RandomFile->getUsedBlocksCount();
    size_t wBlockTargetSize = RandomFile->getFCB()->BlockTargetSize;

    wStr.sprintf("%ld",wSize);
    CurrentFileSizeLBl->setText(wStr.toCChar());

    wStr.sprintf("%ld",wBlockTargetSize);
    BlockTargetSizeLBl->setText(wStr.toCChar());

    wStr.sprintf("%ld",wNbBlocks);
    UsedBlocksLBl->setText(wStr.toCChar());

}

bool ZClearFileDLg::getHighwater()
{
    return HighwaterCHk->isChecked();
}
ssize_t ZClearFileDLg::getSizeToKeep()
{
    if (AlignSizeCHk->isChecked())
        return -1;
    utf8VaryingString wStr = SizeToKeepLEd->text().toUtf8().data();
    return wStr.toLong();
}
