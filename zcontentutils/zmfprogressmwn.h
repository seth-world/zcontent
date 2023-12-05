#ifndef ZMFPROGRESSMWN_H
#define ZMFPROGRESSMWN_H

#include <QMainWindow>
#include <ztoolset/ztimer.h>
#include <chrono>

namespace zbs {
    class ZRandomFile;
}

typedef uint8_t ZProgressEnum_type;

enum ZProgress_enum : ZProgressEnum_type
{
    ZPTP_Nothing        =   0,
    ZPTP_General        =   1,
    ZPTP_HasCloseBtn    =   2,
    ZPTP_TimedClose     =   4
};

class QLabel;
class QProgressBar;
class QPushButton;
class QFrame;

class QVBoxLayout;

class ZProgressBase
{
public:
    ZProgressBase(QWidget *pParent,ZProgressEnum_type pZPTP) {ZPTP=pZPTP; Parent=pParent;}
    ~ZProgressBase() {}
    void initProgressLayout(QVBoxLayout *pVB);

    void initGeneralCount(int pMaximum);
    void initCount(int pMaximum);

    void GeneralAdvanceDisplay(int pCount);
    void AdvanceDisplay(int pCount);

    void setGeneralDone(bool pErrored=false);
    void setDone(bool pErrored=false);

    void setStep(const utf8VaryingString& pStep) ;

    void labelsSetup(const utf8VaryingString& pDescBudy,
                     const utf8VaryingString& pDescText,
                     const utf8VaryingString& pDescription);

    void setDescBudy (const utf8VaryingString& pDescBudy) ;
    void setDescText (const utf8VaryingString& pDescText) ;
    void setGeneralAdvanceDescription (const utf8VaryingString& pDescription);
    void setAdvanceDescription (const utf8VaryingString& pDescription) ;

    void advanceSetupCallBack(int pValue,const utf8VaryingString& pTitle);

    void advanceCallBack(int pValue,const utf8VaryingString& pStep=utf8VaryingString());

    void registerZRFCallBacks(zbs::ZRandomFile* pZRF);

    void setUpdateRate(int pRate) {updateRate = pRate;}

    bool hasGeneral() { return bool(ZPTP & ZPTP_General) ; }
    bool hasCloseBtn() { return bool(ZPTP & ZPTP_HasCloseBtn) ; }
    bool hasTimedClose() { return bool(ZPTP & ZPTP_TimedClose); }

    void setTimeToClose(int pTimeToClose)  { TimeToClose = pTimeToClose; }
private:

    //    zbs::ZRandomFile* ZRF=nullptr;

    QLabel* MainDescBudyLBl=nullptr;
    QLabel* MainDescriptionLBl=nullptr;

    /*----------------general-------------------*/

    QFrame *GeneralFRm=nullptr;

    QLabel* GeneralProgressDescLBl=nullptr;
    QLabel* GeneralProgressLBl=nullptr;
    QProgressBar* GeneralAdvancePGb=nullptr;

    QLabel* GeneralTargetTimeLBl=nullptr;

    ZTime GeneralStartTime;
    int   GeneralMaximum = 0;
    int   GeneralValue = 0;

    /*------------------detail-----------------*/

    QLabel* ProgressDescLBl=nullptr;
    QLabel* ProgressLBl=nullptr;
    QLabel* TargetTimeLBl=nullptr;

    QLabel* StepLBl=nullptr;


    QProgressBar* AdvancePGb=nullptr;
    QPushButton* CloseBTn=nullptr;

    QTimer*       TimerForClose=nullptr;
    int           TimeToClose = 5000 ;  /* five seconds to close */

    ZTime StartTime;
    int   Maximum=0;


    ZProgressEnum_type ZPTP=ZPTP_Nothing;
    QWidget *Parent=nullptr;

    int updateRate = 1 ,currentUpdateRate=0;
};

class ZMFProgressMWn : public QMainWindow , public ZProgressBase
{
    Q_OBJECT
public:
    explicit ZMFProgressMWn(const QString &pTitle, QWidget *parent, ZProgressEnum_type pZPTP);
    ~ZMFProgressMWn();
    void initLayout();

    void setDone (bool pErrored);
    void setDone (ZStatus wSt);
private:

    QLabel* MainDescBudyLBl=nullptr;
    QLabel* MainDescriptionLBl=nullptr;
    QLabel* StepLBl=nullptr;
    QLabel* GeneralProgressDescLBl=nullptr;
    QLabel* GeneralProgressLBl=nullptr;
    QLabel* GeneralTargetTimeLBl=nullptr;
    QProgressBar* GeneralAdvancePGb=nullptr;

    QLabel* ProgressDescLBl=nullptr;
    QLabel* ProgressLBl=nullptr;
    QLabel* TargetTimeLBl=nullptr;
    QProgressBar* AdvancePGb=nullptr;


private slots:

signals:

private:
//    int updateRate = 1 ,currentUpdateRate=0;

};

#endif // ZMFPROGRESSMWN_H
