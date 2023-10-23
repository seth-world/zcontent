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

private slots:
    void searchXmlFileClicked ();
    void update();
    void ParamsTBvChanged (QStandardItem *pItem);
    void ParamsTBvClicked (const QModelIndex &pIndex);


private:
    ZGeneralParameters  WorkParams;
    uriString           XmlParamFile;


    QLabel*       XmlParamsLBl=nullptr;
    QLabel*       TargetRestoreLBl=nullptr;
    QLabel*       BcksetLBl=nullptr;
    QLabel*       BckDateLBl=nullptr;
    QLabel*       TargetZMFLBl=nullptr;
    QLabel*       CommentLBl=nullptr;
    ZQTableView*  ParamsTBv=nullptr;


    QPushButton* SearchXmlFileBTn=nullptr;
    QPushButton* OkBTn=nullptr;
    QPushButton* CancelBTn=nullptr;

    textEditMWn* ComLog=nullptr;
};

#endif // ZGENERALPARAMSDLG_H
