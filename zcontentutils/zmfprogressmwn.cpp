#include "zmfprogressmwn.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QString>

#include <QApplication>



ZMFProgressMWn::ZMFProgressMWn(QWidget *parent)
    : QMainWindow{parent}
{
    this->setAttribute( Qt::WA_DeleteOnClose, true );
}
ZMFProgressMWn::ZMFProgressMWn(const QString& pTitle,QWidget *parent)
    : QMainWindow{parent}
{
    this->setAttribute( Qt::WA_DeleteOnClose, true );
    setWindowTitle(pTitle);
    initLayout();
}
void
ZMFProgressMWn::initLayout()
{
    QWidget* wCentralWDg = new QWidget(this);
    setCentralWidget(wCentralWDg);
    resize(300,200);
    QVBoxLayout* wVB1 = new QVBoxLayout (this->centralWidget());
    wCentralWDg->setLayout(wVB1);

    QHBoxLayout* wHB1=new QHBoxLayout;
    wVB1->addLayout(wHB1);

    MainDescBudyLBl=new QLabel("Index key name",this);
    wHB1->addWidget(MainDescBudyLBl);

    MainDescriptionLBl=new QLabel(" ");
    wHB1->addWidget(MainDescriptionLBl);

    QHBoxLayout* wHB2=new QHBoxLayout;
    wVB1->addLayout(wHB2);

    QLabel* wlb2LBl=new QLabel("Records processed",this);
    wHB2->addWidget(wlb2LBl);

    RecordsProcessedLBl=new QLabel("0",this);
    wHB2->addWidget(RecordsProcessedLBl);

    QHBoxLayout* wHB3=new QHBoxLayout;
    wVB1->addLayout(wHB3);

    QLabel* wlb3LBl=new QLabel("Estimated finish time",this);
    wHB3->addWidget(wlb3LBl);
    TargetTimeLBl=new QLabel("unknown",this);
    wHB3->addWidget(TargetTimeLBl);

    AdvancePGb=new QProgressBar(this);
    AdvancePGb->setMinimum(0);
 //   AdvancePGb->setMaximum(int(MasterFile->getRecordCount()));
    AdvancePGb->setValue(0);

    wVB1->addWidget(AdvancePGb);

    QHBoxLayout* wHB4=new QHBoxLayout;
    wVB1->addLayout(wHB4);

    CloseBTn=new QPushButton("Close",this);
    QSpacerItem* wSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    CloseBTn->setVisible(false);

    QObject::connect(CloseBTn,&QPushButton::clicked,this,&QMainWindow::close);

    wHB4->addWidget(CloseBTn);
    wHB4->addItem(wSpacer);

} // initLayout

void ZMFProgressMWn::initCount(int pMaximum)
{
    Maximum = pMaximum;
    AdvancePGb->setValue(0);
    AdvancePGb->setMaximum(pMaximum);
    StartTime=ZTime::getCurrentTime();
}



void ZMFProgressMWn::ProgressDisplay(int pCount)
{
    if (++currentUpdateRate < updateRate)
        return ;

utf8VaryingString wStr;
    currentUpdateRate = 0;

    AdvancePGb->setValue(pCount);

    wStr.sprintf("%ld / %ld",pCount,AdvancePGb->maximum());
    RecordsProcessedLBl->setText(wStr.toCChar());
    /* compute predicted target time */
    ZTime wElapsed = ZTime::getCurrentTime() - StartTime ;
    /* how many times elapsed time should be multiplied to get achievement time */
    double wRemainTimes = double(Maximum) / double(pCount);
    wElapsed = wElapsed * wRemainTimes ;
    ZTime wTargetTime = StartTime + wElapsed;
    TargetTimeLBl->setText(wTargetTime.toString("%d-%m-%y %T").toCChar());

    QApplication::processEvents();
}


void ZMFProgressMWn::setDone(bool pErrored)
{
    if (pErrored)
        TargetTimeLBl->setText("Errored");
    else {
        AdvancePGb->setValue(AdvancePGb->maximum());
        TargetTimeLBl->setText("Done");
    }
    CloseBTn->setVisible(true);
}
