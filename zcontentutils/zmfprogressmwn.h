#ifndef ZMFPROGRESSMWN_H
#define ZMFPROGRESSMWN_H

#include <QMainWindow>
#include <ztoolset/ztimer.h>

class QLabel;
class QProgressBar;
class QPushButton;

class ZMFProgressMWn : public QMainWindow
{
    Q_OBJECT
public:
    explicit ZMFProgressMWn(QWidget *parent = nullptr);
    explicit ZMFProgressMWn(const QString& pTitle,QWidget *parent = nullptr);
    void initLayout();

    void initCount(int pMaximum);
    void ProgressDisplay(int pCount);

    void setDone(bool pErrored=false);


    QLabel* MainDescBudyLBl=nullptr;
    QLabel* MainDescriptionLBl=nullptr;
    QLabel* RecordsProcessedLBl=nullptr;
    QLabel* TargetTimeLBl=nullptr;
    QProgressBar* AdvancePGb=nullptr;
    QPushButton* CloseBTn=nullptr;

    ZTime StartTime;
    int   Maximum=0;
//    bool event(QEvent *event) override;

private slots:

signals:

private:
    int updateRate = 1 ,currentUpdateRate=0;

};

#endif // ZMFPROGRESSMWN_H
