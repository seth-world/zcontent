#include "zgeneralparamsdlg.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QGridLayout>
#include <QGroupBox>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QCheckBox>
#include <QFrame>

#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>

#include <QFileDialog>

#include <ztoolset/uristring.h>

#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory()

#include <zcontent/zcontentutils/zexceptiondlg.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <QTableView>

#include "texteditmwn.h"

/* for xml exchanges */
#include <zxml/zxmlprimitives.h>

#include <zqt/zqtwidget/zqtwidgettools.h>

#include <QIcon>
#include <QFileDialog>
#include <QFontDialog>

#define __SEARCH_ICON__ "question.png"

ZGeneralParamsDLg::ZGeneralParamsDLg(QWidget* pParent) : WorkParams(&WorkBaseParams) , QDialog(pParent)
{
    initLayout();
}

ZGeneralParamsDLg::~ZGeneralParamsDLg()
{
    if (ComLog != nullptr)
        ComLog->deleteLater();

}

void
ZGeneralParamsDLg::initLayout() {

    /* backup dialog setup */

    setWindowTitle("General parameters");
    resize(1050,550);

    QVBoxLayout* QVL=new QVBoxLayout;
    setLayout(QVL);

    QHBoxLayout* QHL=new QHBoxLayout;
    QVL->insertLayout(0,QHL);

    QLabel* wLblBcksetLBl = new QLabel("Parameter file");
    QHL->addWidget(wLblBcksetLBl);

    XmlParamsLBl=new QLabel();
    QHL->addWidget(XmlParamsLBl);

    SearchXmlFileBTn = new QPushButton("Search");
    SearchXmlFileBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );
    QHL->addWidget(SearchXmlFileBTn);
    QObject::connect(SearchXmlFileBTn, &QPushButton::clicked, this, &ZGeneralParamsDLg::searchXmlFileClicked);


    /* table view */

    QHBoxLayout* QHLTBv=new QHBoxLayout;
    QVL->insertLayout(-1,QHLTBv);

    ParamsTBv = new ZQTableView(this);
    QHLTBv->addWidget(ParamsTBv);

    QFrame* wFRm=new QFrame(this);
    wFRm->setFrameShape(QFrame::StyledPanel);
    wFRm->setFrameShadow(QFrame::Raised);
    QVBoxLayout* wVBL1 = new QVBoxLayout;
    wFRm->setLayout(wVBL1);


    QVL->addWidget(wFRm);

    ZVB_BasicCBx=new QCheckBox("Basic: Messages with no particular domain",this);
    wVBL1->addWidget(ZVB_BasicCBx);
    ZVB_MutexCBx=new QCheckBox("Mutex: Mutexes management",this);
    wVBL1->addWidget(ZVB_MutexCBx);
    ZVB_ThreadCBx=new QCheckBox("Thread: Threads management",this);
    wVBL1->addWidget(ZVB_ThreadCBx);
    ZVB_StatsCBx=new QCheckBox("Stats: Collects stats and performance data",this);
    wVBL1->addWidget(ZVB_StatsCBx);
    ZVB_NetCBx=new QCheckBox("Net: Sockets servers SSL & protocol",this);
    wVBL1->addWidget(ZVB_NetCBx);
    ZVB_NetStatsCBx=new QCheckBox("NetStats: Collects stats and performance for net operations",this);
    wVBL1->addWidget(ZVB_NetStatsCBx);
    ZVB_XmlCBx=new QCheckBox("Xml: Xml engine verbose",this);
    wVBL1->addWidget(ZVB_XmlCBx);

    ZVB_ZRFCBx=new QCheckBox("ZRF: ZRandomFile general messages",this);
    wVBL1->addWidget(ZVB_ZRFCBx);
    ZVB_MemEngineCBx=new QCheckBox("MemEngine: file memory space allocation and management operations",this);
    wVBL1->addWidget(ZVB_MemEngineCBx);
    ZVB_FileEngineCBx=new QCheckBox("FileEngine: file access operations from file engine",this);
    wVBL1->addWidget(ZVB_FileEngineCBx);
    ZVB_SearchEngineCBx=new QCheckBox("SearchEngine: all search operations from search enginest",this);
    wVBL1->addWidget(ZVB_SearchEngineCBx);

    ZVB_ZMFCBx=new QCheckBox("ZMF: ZMasterFile general messages",this);
    wVBL1->addWidget(ZVB_ZMFCBx);
    ZVB_ZIFCBx=new QCheckBox("ZIF: ZIndexFile general messages",this);
    wVBL1->addWidget(ZVB_ZIFCBx);


    /* buttons */

    QHBoxLayout* wButtonBoxQHb=new QHBoxLayout;
    wButtonBoxQHb->setAlignment(Qt::AlignRight);
    QVL->insertLayout(-1,wButtonBoxQHb);

    CancelBTn = new QPushButton("Quit");
    wButtonBoxQHb->addWidget(CancelBTn);
    OkBTn = new QPushButton("Update");
    wButtonBoxQHb->addWidget(OkBTn);

    QObject::connect(OkBTn, &QPushButton::clicked, this, &ZGeneralParamsDLg::update);
    QObject::connect(CancelBTn, &QPushButton::clicked, this, &QDialog::reject);

    /* table view setup */

    ParamsTBv->newModel(3);

    ParamsTBv->ItemModel->setHorizontalHeaderItem(0,new QStandardItem("Parameter name"));
    ParamsTBv->ItemModel->setHorizontalHeaderItem(1,new QStandardItem("Value"));
    ParamsTBv->ItemModel->setHorizontalHeaderItem(2,new QStandardItem(""));


    //  BckTBv->horizontalHeader()->hide();

    ParamsTBv->verticalHeader()->hide();

    QObject::connect(ParamsTBv->ItemModel, &QStandardItemModel::itemChanged, this, &ZGeneralParamsDLg::ParamsTBvChanged);
    QObject::connect(ParamsTBv, &QTableView::clicked, this, &ZGeneralParamsDLg::ParamsTBvClicked);
} //initLayout

void
ZGeneralParamsDLg::ParamsTBvChanged (QStandardItem *pItem)
{
    int wRet=0;
    if (pItem->column() != 1)
        return;
    if (pItem->row() < 3) {
        uriString wDir=ParamsTBv->ItemModel->item(pItem->row(),1)->text().toUtf8().data();
        if (!wDir.exists()) {
            wRet=ZExceptionDLg::adhocMessage3B("Directory",Severity_Warning,"Find another","Give up","Force",
                                                     "Directory does not exist.\nFind another one, force anyway give up ?");
            if (wRet==ZEDLG_Third) { /* if third : search for a new directory value */
                wRet =getDir(wDir);
            }
            if (wRet==QDialog::Rejected) {
                switch (pItem->row())
                {
                case 0:
                    pItem->setText(WorkParams.WorkDirectory.toCChar());
                    return;
                case 1:
                    pItem->setText(WorkParams.ParamDirectory.toCChar());
                    return;
                case 2:
                    pItem->setText(WorkParams.IconDirectory.toCChar());
                    return;
                }// switch
                return;
            }
            if (wRet==QDialog::Accepted) { /* if ok : force the directory value to a non existing one */
                switch (pItem->row()) {
                case 0:
                    WorkParams.WorkDirectory = wDir;
                    pItem->setText(WorkParams.WorkDirectory.toCChar());
                    return;
                case 1:
                    WorkParams.ParamDirectory = wDir;
                    pItem->setText(WorkParams.ParamDirectory.toCChar());
                    return;
                case 2:
                    WorkParams.IconDirectory = wDir;
                    pItem->setText(WorkParams.IconDirectory.toCChar());
                    return;
                } // switch
            } // Accepted
        }
        switch (pItem->row())
        {
        case 0:
            WorkParams.WorkDirectory = wDir;
            return;
        case 1:
            WorkParams.ParamDirectory = wDir;
            return;
        case 2:
            WorkParams.IconDirectory = wDir;
            return;
        }// switch
    } // if (pIndex.row()<3)

    if (pItem->row() == 3) {
        if (QFont(pItem->text()).exactMatch())
            WorkParams.FixedFont = pItem->text().toUtf8().data();
        else {
            utf8VaryingString wFont = pItem->text().toUtf8().data();
            int wRet=getFont(wFont);
            if (wRet==QDialog::Rejected) {
                pItem->setText("");
                return;
            }
            WorkParams.FixedFont = wFont;
            pItem->setText(WorkParams.FixedFont.toCChar());
        }
        return;
    } // if (pItem->row() == 3)

}
int
ZGeneralParamsDLg::getFont(utf8VaryingString& pFont)
{
    QFont wFont;
    QFontDialog* wFontDLg=nullptr;
    if (!pFont.isEmpty()) {
        wFont.setFamily(pFont.toCChar());
        wFontDLg=new QFontDialog(wFont,this);
    }
    else
        wFontDLg=new QFontDialog(this);

    int wRet=wFontDLg->exec();
    if (wRet==QDialog::Rejected)
        return wRet;
    wFont = wFontDLg->selectedFont();
    pFont = wFont.family().toUtf8().data();
    return QDialog::Accepted;
}
int
ZGeneralParamsDLg::getDir(uriString &pDir)
{
    uriString wDir=pDir;
    QString wFileName = QFileDialog::getExistingDirectory(this, tr("Directory"),
                                                     wDir.toCChar());
    if (wFileName.isEmpty())
        return QDialog::Rejected;
    pDir=wFileName.toUtf8().data();
    return QDialog::Accepted;
}
void
ZGeneralParamsDLg::ParamsTBvClicked (const QModelIndex &pIndex)
{
    if (pIndex.column() != 2)
        return;

    if (pIndex.row()<3) {
        uriString wDir=ParamsTBv->ItemModel->item(pIndex.row(),1)->text().toUtf8().data();
        int wRet=getDir(wDir);
        if (wRet==QDialog::Rejected)
            return;
        /* a directory has been selected */

        switch (pIndex.row())
        {
        case 0:
            WorkParams.WorkDirectory = wDir;
            ParamsTBv->ItemModel->item(pIndex.row(),1)->setText(WorkParams.WorkDirectory.toCChar());
            return;
        case 1:
            WorkParams.ParamDirectory = wDir;
            ParamsTBv->ItemModel->item(pIndex.row(),1)->setText(WorkParams.ParamDirectory.toCChar());
            return;
        case 2:
            WorkParams.IconDirectory = wDir;
            ParamsTBv->ItemModel->item(pIndex.row(),1)->setText(WorkParams.IconDirectory.toCChar());
            return;
        }// switch
    } // if (pIndex.row()<3)

    if (pIndex.row()==3) {
        int wRet=getFont(WorkParams.FixedFont);
        if (wRet==QDialog::Rejected)
            return;
        ParamsTBv->ItemModel->item(pIndex.row(),1)->setText(WorkParams.FixedFont.toCChar());
        return;
    } //if (pIndex.row()==3)

}

ZVerbose_Base
ZGeneralParamsDLg::fromScreenVerbose()
{
    ZVerbose_Base wVerbose=ZVB_NoVerbose;

    if (ZVB_BasicCBx->isChecked())
        wVerbose |= ZVB_Basic ;
    if (ZVB_MutexCBx->isChecked())
        wVerbose |= ZVB_Mutex ;
    if (ZVB_ThreadCBx->isChecked())
        wVerbose |= ZVB_Thread ;
    if (ZVB_StatsCBx->isChecked())
        wVerbose |= ZVB_Stats ;
    if (ZVB_NetCBx->isChecked())
        wVerbose |= ZVB_Net ;
    if (ZVB_NetStatsCBx->isChecked())
        wVerbose |= ZVB_NetStats ;
    if (ZVB_XmlCBx->isChecked())
        wVerbose |= ZVB_Xml ;
    if (ZVB_ZRFCBx->isChecked())
        wVerbose |= ZVB_ZRF ;
    if (ZVB_ZMFCBx->isChecked())
        wVerbose |= ZVB_ZMF ;
    if (ZVB_ZIFCBx->isChecked())
        wVerbose |= ZVB_ZIF ;
    if (ZVB_MemEngineCBx->isChecked())
        wVerbose |= ZVB_MemEngine ;
    if (ZVB_FileEngineCBx->isChecked())
        wVerbose |= ZVB_FileEngine ;
    if (ZVB_SearchEngineCBx->isChecked())
        wVerbose |= ZVB_SearchEngine ;

    return wVerbose;
}
void
ZGeneralParamsDLg::displayVerbose(ZVerbose_Base pVerbose)
{
    ZVB_BasicCBx->setChecked(pVerbose&ZVB_Basic);
    ZVB_MutexCBx->setChecked(pVerbose&ZVB_Mutex);
    ZVB_ThreadCBx->setChecked(pVerbose&ZVB_Thread);
    ZVB_StatsCBx->setChecked(pVerbose&ZVB_Stats);
    ZVB_NetCBx->setChecked(pVerbose&ZVB_Net);
    ZVB_NetStatsCBx->setChecked(pVerbose&ZVB_NetStats);
    ZVB_XmlCBx->setChecked(pVerbose&ZVB_Xml);
    ZVB_ZRFCBx->setChecked(pVerbose&ZVB_ZRF);
    ZVB_ZMFCBx->setChecked(pVerbose&ZVB_ZMF);
    ZVB_ZIFCBx->setChecked(pVerbose&ZVB_ZIF);
    ZVB_MemEngineCBx->setChecked(pVerbose&ZVB_MemEngine);
    ZVB_FileEngineCBx->setChecked(pVerbose&ZVB_FileEngine);
    ZVB_SearchEngineCBx->setChecked(pVerbose&ZVB_SearchEngine);

    return ;
}

void
ZGeneralParamsDLg::update()
{
    WorkBaseParams.setVerbose(fromScreenVerbose());

    ZStatus wSt;
    QString wParamsBase = __GENERAL_PARAMETERS_FILE__;
    uriString wDir = WorkParams.getParamDirectory();
    if (!WorkParams.currentXml.isEmpty()) {
        wDir = WorkParams.currentXml;
        wParamsBase = WorkParams.currentXml.getBasename().toCChar();
    }
    QString wFileName = QFileDialog::getSaveFileName(this, tr("Xml parameter file"),
                                                     wDir.toCChar(),
                                                     "xml files (*.xml);;All (*.*)",
                                                     &wParamsBase);
    if (wFileName.isEmpty())
        return;
    uriString wXmlParams= wFileName.toUtf8().data();
    uriString wBck;
    if (wXmlParams.exists()) {
        wBck=wXmlParams.getBckName(wXmlParams,"bck");
        wSt=wXmlParams.rename(wBck);
    }
    wSt=WorkParams.XmlSave(wXmlParams,nullptr);
    if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp = ZException.last().formatFullUserMessage().toString();
        ZExceptionDLg::adhocMessage("Save parameters",Severity_Error,
                                    nullptr,&wExcp,"Error saving xml parameter file %s",wXmlParams.toString());
        return;
    }
    utf8VaryingString wSaveMsg ;
    wSaveMsg.sprintf("Saved parameters as %s\n",wXmlParams.toString());
    if (!wBck.isEmpty()) {
        wSaveMsg.addsprintf("\nPrevious parameters version saved as %s",wBck.toString());
    }
    wSaveMsg.addsprintf("\n\nMake these parameters current <Make current>",wBck.toString());
    int wRet=ZExceptionDLg::adhocMessage2B("Save parameters",Severity_Error, "Give up","Make current",
                                nullptr,nullptr,wSaveMsg.toCChar());

    if (wRet==QDialog::Rejected) {
        return;
    }
    GeneralParameters._copyFrom(WorkParams);
    GeneralParameters.XmlSave(XmlParamFile);
    return;
} // update


void
ZGeneralParamsDLg::searchXmlFileClicked ()
{
    uriString wDir = GeneralParameters.getParamDirectory();
    QString wFileName = QFileDialog::getOpenFileName(this, tr("Xml parameters file"),
                                                     wDir.toCChar(),
                                                     "xml files (*.xml);;All (*.*)");
    if (wFileName.isEmpty())
        return;
    uriString wXmlParams= wFileName.toUtf8().data();
    if (!wXmlParams.exists())
        return;
    ZStatus wSt=WorkParams.XmlLoad(wXmlParams,nullptr);
    if (wSt!=ZS_SUCCESS) {
        utf8VaryingString wExcp = ZException.last().formatFullUserMessage().toString();
        ZExceptionDLg::adhocMessage("Load parameters",Severity_Error,
                                    nullptr,&wExcp,"Error while loading xml parameter file %s",wXmlParams.toString());
        return;
    }
    setup(WorkParams);
    return;
} // searchXmlFileClicked

bool InitFBS=true;
QStandardItem*
ZGeneralParamsDLg::fakeBTnsetup()
{
    QStandardItem* wButton=nullptr;
    static QPixmap wSearchPXm;
    static QIcon wSearchICn;
 //   QBrush wBrush0(Qt::blue);
//    QColor wLightBlue(30,136,229);

    QPalette wPalette=OkBTn->palette();
    QBrush wBrush0(wPalette.window());
    QBrush wBrush1(wPalette.windowText());
    /* /home/gerard/Development/zbasetools/zqt/icons/question.png */
    if (InitFBS) {
        InitFBS=false;
        if (!WorkParams.IconDirectory.isEmpty()) {
            uriString wPixURI="/home/gerard/Development/zbasetools/zqt/icons/";
 //           uriString wPixURI=WorkParams.IconDirectory;
            wPixURI.addConditionalDirectoryDelimiter() ;
            wPixURI += __SEARCH_ICON__;
            wSearchPXm.load(wPixURI.toCChar());
            wSearchICn.addPixmap(wSearchPXm);
            wButton = new QStandardItem(wSearchICn,"Search");
 //           wButton = new QStandardItem();
 //           QBrush wBrushIcn(wSearchPXm);
 //           wButton->setForeground(wBrushIcn);
        }
        else {
            wButton = new QStandardItem("Search");
        }
        wButton->setBackground(wBrush0);
        wButton->setForeground(wBrush1);
        wButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        return wButton;
    }

    if (!WorkParams.IconDirectory.isEmpty()) {
        wButton = new QStandardItem(wSearchICn,"Search");
    }
    else {
        wButton = new QStandardItem("Search");
    }
    wButton->setBackground(wBrush0);
    wButton->setForeground(wBrush1);
    wButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    return wButton;
} //fakeBTnsetup


QStandardItem*
ZGeneralParamsDLg::inactiveSetup()
{
    QStandardItem* wButton=nullptr;
    static QPixmap wSearchPXm;
    static QIcon wSearchICn;
    QBrush wBrush0(Qt::blue);
    QBrush wBrush1(Qt::white);

    /* /home/gerard/Development/zbasetools/zqt/icons/question.png */
    if (InitFBS) {
        InitFBS=false;
 /*       if (!WorkParams.IconDirectory.isEmpty()) {
            uriString wPixURI=WorkParams.IconDirectory;
            wPixURI.addConditionalDirectoryDelimiter() ;
            wPixURI += __SEARCH_ICON__;
            wSearchPXm.load(wPixURI.toCChar());
            wSearchICn.addPixmap(wSearchPXm);
//            wButton = new QStandardItem(wSearchICn,"");
            wButton = new QStandardItem();
            wButton->setData(QVariant(wSearchPXm), Qt::DecorationRole);
        }
        else { */
            wButton = new QStandardItem("Search");
            wButton->setBackground(wBrush0);
            wButton->setForeground(wBrush1);
 //       }
        wButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        return wButton;
    }

    if (!WorkParams.IconDirectory.isEmpty()) {
//        wButton = new QStandardItem(wSearchICn,"");
        wButton = new QStandardItem();
        wButton->setData(QVariant(wSearchPXm), Qt::DecorationRole);
    }
    else {
        wButton = new QStandardItem("Search");
        wButton->setBackground(wBrush0);
        wButton->setForeground(wBrush1);
    }
    wButton->setFlags(Qt::NoItemFlags);
    return wButton;
} //fakeBTnsetup
ZStatus
ZGeneralParamsDLg::setup(ZGeneralParameters &pGeneralParameters) {

    WorkParams = pGeneralParameters;

    XmlParamsLBl->setText(WorkParams.currentXml.toCChar());

    QList<QStandardItem*> wRow;

    setQtItemAlignment( Qt::AlignRight);

    wRow.clear();
    wRow << createItem("Working directory");
    wRow.last()->setFlags(Qt::ItemIsEnabled );
    wRow << createItem(WorkParams.getWorkDirectory());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    wRow << fakeBTnsetup() ;
    ParamsTBv->ItemModel->appendRow(wRow);

    wRow.clear();
    wRow << createItem("Parameters directory");
    wRow.last()->setFlags(Qt::ItemIsEnabled );
    wRow << createItem(WorkParams.getParamDirectory());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    wRow << fakeBTnsetup() ;
    ParamsTBv->ItemModel->appendRow(wRow);

    wRow.clear();
    wRow << createItem("Icon directory");
    wRow.last()->setFlags(Qt::ItemIsEnabled );
    wRow << createItem(WorkParams.getIconDirectory());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    wRow << fakeBTnsetup() ;
    ParamsTBv->ItemModel->appendRow(wRow);

    wRow.clear();
    wRow << createItem("Fixed size font");
    wRow.last()->setFlags(Qt::ItemIsEnabled );
    wRow << createItem(WorkParams.getFixedFont());
    wRow.last()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    wRow << fakeBTnsetup() ;
    ParamsTBv->ItemModel->appendRow(wRow);

    ParamsTBv->resizeColumnsToContents();
    ParamsTBv->resizeRowsToContents();

    displayVerbose(WorkParams.getVerbose());
  /*
    ZVB_BasicCBx->setChecked(BaseParameters->VerboseBasic());
    ZVB_MutexCBx->setChecked(BaseParameters->VerboseMutex());
    ZVB_ThreadCBx->setChecked(BaseParameters->VerboseThread());
    ZVB_StatsCBx->setChecked(BaseParameters->VerboseStats());
    ZVB_NetCBx->setChecked(BaseParameters->VerboseNet());
    ZVB_NetStatsCBx->setChecked(BaseParameters->VerboseNetStats());
    ZVB_XmlCBx->setChecked(BaseParameters->VerboseXml());
    ZVB_ZRFCBx->setChecked(BaseParameters->VerboseZRF());
    ZVB_ZMFCBx->setChecked(BaseParameters->VerboseZMF());
    ZVB_ZIFCBx->setChecked(BaseParameters->VerboseZIF());
    ZVB_MemEngineCBx->setChecked(BaseParameters->VerboseMemEngine());
    ZVB_FileEngineCBx->setChecked(BaseParameters->VerboseFileEngine());
    ZVB_SearchEngineCBx->setChecked(BaseParameters->VerboseSearchEngine());
    ZVB_BasicCBx->setChecked(BaseParameters->VerboseBasic());
*/
    return ZS_SUCCESS;
} // dataSetup
