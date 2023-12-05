#ifndef ZGENERALPARAMSDLG_H
#define ZGENERALPARAMSDLG_H

#include <QDialog>
#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <zcontentcommon/zgeneralparameters.h>

class QLabel;
class QLineEdit;
class ZQTableView;
class textEditMWn;
class QStandardItem;
class QCheckBox;

class ZGeneralParamsDLg : public QDialog
{
public:
    explicit ZGeneralParamsDLg(QWidget* pParent);
    ~ZGeneralParamsDLg();

    void initLayout();

    ZStatus setup(ZGeneralParameters& pGeneralParameters);
    QStandardItem* fakeBTnsetup();
    QStandardItem* inactiveSetup();

    int getDir(uriString &pDir);
    int getFont(utf8VaryingString &pFont);

    ZVerbose_Base fromScreenVerbose();
    void displayVerbose(ZVerbose_Base pVerbose);

private slots:
    void searchXmlFileClicked ();
    void update();
    void ParamsTBvChanged (QStandardItem *pItem);
    void ParamsTBvClicked (const QModelIndex &pIndex);


private:
    ZBaseParameters     WorkBaseParams;
    ZGeneralParameters  WorkParams;

    uriString           XmlParamFile;

    QLabel*       XmlParamsLBl=nullptr;
    QLabel*       TargetRestoreLBl=nullptr;
    QLabel*       BcksetLBl=nullptr;
    QLabel*       BckDateLBl=nullptr;
    QLabel*       TargetZMFLBl=nullptr;
    QLabel*       CommentLBl=nullptr;
    ZQTableView*  ParamsTBv=nullptr;


    QCheckBox*    ZVB_BasicCBx=nullptr;
    QCheckBox*    ZVB_MutexCBx=nullptr;
    QCheckBox*    ZVB_ThreadCBx=nullptr;
    QCheckBox*    ZVB_StatsCBx=nullptr;
    QCheckBox*    ZVB_NetCBx=nullptr;
    QCheckBox*    ZVB_NetStatsCBx=nullptr;
    QCheckBox*    ZVB_XmlCBx=nullptr;
    QCheckBox*    ZVB_ZRFCBx=nullptr;
    QCheckBox*    ZVB_ZMFCBx=nullptr;
    QCheckBox*    ZVB_ZIFCBx=nullptr;
    QCheckBox*    ZVB_MemEngineCBx=nullptr;
    QCheckBox*    ZVB_FileEngineCBx=nullptr;
    QCheckBox*    ZVB_SearchEngineCBx=nullptr;

    QPushButton* SearchXmlFileBTn=nullptr;
    QPushButton* OkBTn=nullptr;
    QPushButton* CancelBTn=nullptr;

    textEditMWn* ComLog=nullptr;
};

#endif // ZGENERALPARAMSDLG_H
