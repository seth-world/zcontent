#include "zmfprogressmwn.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

#include <QFrame>

#include <QString>
#include <QTimer>

#include <QApplication>

#include <zcontent/zrandomfile/zrandomfile.h>

#include <chrono>


ZMFProgressMWn::ZMFProgressMWn(const QString &pTitle, QWidget *parent, ZProgressEnum_type pZPTP)
    : QMainWindow{parent} , ZProgressBase(this,pZPTP)
{

    this->setAttribute( Qt::WA_DeleteOnClose, true );
    setWindowTitle(pTitle);
    initLayout();
}
ZMFProgressMWn::~ZMFProgressMWn()
{

}
void
ZMFProgressMWn::initLayout()
{
    QWidget* wCentralWDg = new QWidget(this);
    setCentralWidget(wCentralWDg);
    resize(300,200);
    QVBoxLayout* wVB1 = new QVBoxLayout (this->centralWidget());
    wCentralWDg->setLayout(wVB1);

    initProgressLayout(wVB1);

    QHBoxLayout* wHB5=new QHBoxLayout;
    wVB1->addLayout(wHB5);




} // initLayout

void ZProgressBase::initProgressLayout(QVBoxLayout *pVB)
{

    QHBoxLayout* wHB1=new QHBoxLayout;
    pVB->addLayout(wHB1);

    MainDescBudyLBl=new QLabel("Name",Parent);
    wHB1->addWidget(MainDescBudyLBl);

    MainDescriptionLBl=new QLabel(" ",Parent);
    wHB1->addWidget(MainDescriptionLBl);

    StepLBl = new QLabel("Nothing yet",Parent);
    pVB->addWidget(StepLBl);

    /* -----------------------general----------------- */

    if (hasGeneral()) {
        GeneralFRm = new QFrame(Parent);
        GeneralFRm->setFrameShape(QFrame::StyledPanel);
        GeneralFRm->setFrameShadow(QFrame::Raised);

        pVB->addWidget(GeneralFRm);
        QVBoxLayout* wFrmVBx=new QVBoxLayout;
        GeneralFRm->setLayout(wFrmVBx);

        QHBoxLayout* wHB20=new QHBoxLayout;
        wFrmVBx->addLayout(wHB20);

        GeneralProgressDescLBl=new QLabel("Total size",Parent);
        wHB20->addWidget(GeneralProgressDescLBl);

        GeneralProgressLBl=new QLabel("0",Parent);
        wHB20->addWidget(GeneralProgressLBl);

        QHBoxLayout* wHB21=new QHBoxLayout;
        wFrmVBx->addLayout(wHB21);

        QLabel* wlb21LBl=new QLabel("Estimated finish time",Parent);
        wHB21->addWidget(wlb21LBl);

        GeneralTargetTimeLBl=new QLabel("unknown",Parent);
        wHB21->addWidget(GeneralTargetTimeLBl);


        GeneralAdvancePGb = new QProgressBar(Parent);
        wFrmVBx->addWidget(GeneralAdvancePGb);



        GeneralFRm->setVisible(true);
    }

    /* -------------detail-------------------------- */

    QHBoxLayout* wHB30=new QHBoxLayout;
    pVB->addLayout(wHB30);

    ProgressDescLBl=new QLabel("Size",Parent);
    wHB30->addWidget(ProgressDescLBl);

    ProgressLBl=new QLabel("0",Parent);
    wHB30->addWidget(ProgressLBl);

    QHBoxLayout* wHB4=new QHBoxLayout;
    pVB->addLayout(wHB4);

    QLabel* wlb3LBl=new QLabel("Estimated finish time",Parent);
    wHB4->addWidget(wlb3LBl);
    TargetTimeLBl=new QLabel("unknown",Parent);
    wHB4->addWidget(TargetTimeLBl);

    AdvancePGb=new QProgressBar(Parent);
    AdvancePGb->setMinimum(0);
    AdvancePGb->setValue(0);

    pVB->addWidget(AdvancePGb);

    if (hasCloseBtn()) {

        QHBoxLayout* wHB5=new QHBoxLayout;
        pVB->addLayout(wHB5);

        CloseBTn=new QPushButton("Close",Parent);
        QSpacerItem* wSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        CloseBTn->setVisible(false);

        QObject::connect(CloseBTn,&QPushButton::clicked,Parent,&QWidget::close);

        wHB5->addItem(wSpacer);
        wHB5->addWidget(CloseBTn);
    }





} // initLayout

void ZProgressBase::initGeneralCount(int pMaximum)
{
    if (!hasGeneral())
        return;
    GeneralMaximum = pMaximum;
    GeneralAdvancePGb->setValue(0);
    GeneralAdvancePGb->setMaximum(pMaximum);
    GeneralStartTime=ZTime::getCurrentTime();
    GeneralFRm->setVisible(true);
}

void ZProgressBase::initCount(int pMaximum)
{
    Maximum = pMaximum;
    AdvancePGb->setValue(0);
    AdvancePGb->setMaximum(pMaximum);
    StartTime=ZTime::getCurrentTime();
}



void ZProgressBase::AdvanceDisplay(int pCount)
{
    if (++currentUpdateRate < updateRate)
        return ;

utf8VaryingString wStr;
    currentUpdateRate = 0;

    AdvancePGb->setValue(pCount);

    wStr.sprintf("%ld / %ld",pCount,Maximum);
    ProgressLBl->setText(wStr.toCChar());
    /* compute predicted target time */
    ZTime wElapsed = ZTime::getCurrentTime() - StartTime ;
    /* how many times elapsed time should be multiplied to get achievement time */
    double wRemainTimes = double(Maximum) / double(pCount);
    wElapsed = wElapsed * wRemainTimes ;
    ZTime wTargetTime = StartTime + wElapsed;
    TargetTimeLBl->setText(wTargetTime.toString("%d-%m-%y %T").toCChar());

    if (hasGeneral())
        GeneralAdvanceDisplay(pCount);

    QApplication::processEvents();
}
void ZProgressBase::setStep(const utf8VaryingString& pStep) {
    StepLBl->setText(pStep.toCChar());
}

void ZProgressBase::setGeneralDone(bool pErrored)
{
    if (!hasGeneral())
        return;
    if (pErrored) {
        QPalette wP0=GeneralTargetTimeLBl->palette();
        wP0.setColor(QPalette::WindowText,Qt::red);
        GeneralTargetTimeLBl->setPalette(wP0);
        GeneralTargetTimeLBl->setText("Errored");
        QPalette wP=GeneralAdvancePGb->palette();
        wP.setColor(QPalette::Highlight,Qt::red);
        GeneralAdvancePGb->setPalette(wP);
    }
    else {
        GeneralAdvancePGb->setValue(Maximum);
        GeneralTargetTimeLBl->setText("Done");
        StepLBl->setText("Processing is finished");
    }
}
void ZProgressBase::setDone(bool pErrored)
{
    if (pErrored) {
        QPalette wP0=TargetTimeLBl->palette();
        wP0.setColor(QPalette::WindowText,Qt::red);
        TargetTimeLBl->setPalette(wP0);
        TargetTimeLBl->setText("Errored");
        QPalette wP=AdvancePGb->palette();
        wP.setColor(QPalette::Highlight,Qt::red);
        AdvancePGb->setPalette(wP);

    }
    else {
        AdvancePGb->setValue(Maximum);
        TargetTimeLBl->setText("Done");
    }
    utf8VaryingString wStr;
    if (StartTime.isNull()) {
        wStr = "Processing start time not set.";
    }
    else {
        ZTime wTi=ZTime::currentTime() - StartTime;
        ZTimer wTr ;
        wTr.set(StartTime);
        wTr.end();
        wStr="Processing elapsed time ";
        wStr += wTr.reportElapsed();
    }
    StepLBl->setText(wStr.toCChar());

    if (hasGeneral()) {
        GeneralValue += Maximum;
        GeneralAdvanceDisplay(GeneralValue);
    }
    if (hasCloseBtn())
        CloseBTn->setVisible(true);

    if (hasTimedClose()) {
        TimerForClose = new QTimer(Parent);
        TimerForClose->setSingleShot(true);

        QObject::connect(TimerForClose,&QTimer::timeout,Parent,&QWidget::close);
        TimerForClose->start(TimeToClose) ;
    }
}
void
ZProgressBase::advanceCallBack(int pValue,const utf8VaryingString& pStep)
{
    if (!pStep.isEmpty()) {
        setStep(pStep.toCChar());
    }
    AdvanceDisplay(pValue);
}

void
ZProgressBase::GeneralAdvanceDisplay(int pValue)
{
    if (!hasGeneral())
        return;

    if (pValue < 0)
        return ;
    utf8VaryingString wStr;
    if (GeneralValue+pValue > GeneralMaximum) {
        pValue = GeneralMaximum - GeneralValue;
    }
    GeneralAdvancePGb->setValue(GeneralValue+pValue);
    wStr.sprintf("%ld / %ld",GeneralValue+pValue,GeneralMaximum);
    GeneralProgressLBl->setText(wStr.toCChar());
    /* compute predicted target time */
    ZTime wElapsed = ZTime::getCurrentTime() - GeneralStartTime ;
    /* how many times elapsed time should be multiplied to get achievement time */
    double wRemainTimes = double(GeneralMaximum) / double(GeneralValue+pValue);
    wElapsed = wElapsed * wRemainTimes ;
    ZTime wTargetTime = StartTime + wElapsed;
    GeneralTargetTimeLBl->setText(wTargetTime.toString("%d-%m-%y %T").toCChar());

    QApplication::processEvents();
}


void
ZProgressBase::advanceSetupCallBack(int pValue,const utf8VaryingString& pTitle)
{
    initCount(pValue);

    QApplication::processEvents();
}

void ZProgressBase::labelsSetup(const utf8VaryingString& pDescBudy,
                                 const utf8VaryingString& pDescText,
                                 const utf8VaryingString& pDescription)
{
    setDescBudy(pDescBudy);
    setDescText(pDescText);
    setAdvanceDescription(pDescription);
}


void ZProgressBase::setDescBudy (const utf8VaryingString& pDescBudy)
{
    MainDescBudyLBl->setText(pDescBudy.toCChar());
}

void ZProgressBase::setDescText (const utf8VaryingString& pDescText)
{
    MainDescriptionLBl->setText(pDescText.toCChar());
}

void ZProgressBase::setGeneralAdvanceDescription (const utf8VaryingString& pDescription)
{
    GeneralProgressDescLBl->setText(pDescription.toCChar());
}
void ZProgressBase::setAdvanceDescription (const utf8VaryingString& pDescription)
{
    ProgressDescLBl->setText(pDescription.toCChar());
}

void ZProgressBase::registerZRFCallBacks(zbs::ZRandomFile* pZRF)
{
//    ZRF=pZRF;
    pZRF->registerProgressCallBack(std::bind(&ZProgressBase::advanceCallBack,this,std::placeholders::_1,std::placeholders::_2));
    pZRF->registerProgressSetupCallBack(std::bind(&ZProgressBase::advanceSetupCallBack,this,std::placeholders::_1,std::placeholders::_2));

}

void ZMFProgressMWn::setDone(bool pErrored)
{
    ZProgressBase::setDone(pErrored);

}

void ZMFProgressMWn::setDone(ZStatus wSt)
{
    ZProgressBase::setDone(wSt!=ZS_SUCCESS);
}
