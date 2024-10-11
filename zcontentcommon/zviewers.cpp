#include "zviewers.h"

#include <QObject>
#include <QVBoxLayout>
#include <QIcon>
#include <QIcon>
#include <QPoint>
#include <QPushButton>
#include <QLabel>
#include <QMainWindow>
#include <QTextEdit>
#include <QProcess>

#include <zcontent/zcontentcommon/zdomainpath.h>
#include <zcontent/zcontentcommon/zdomainobject.h>
#include <zcontent/zcontentcommon/zcontentobjectbroker.h>

ZViewers::ZViewers() {}


namespace zbs {

bool imageViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    QWidget* ViewerWNd = new QWidget(pFather,Qt::ToolTip);

    QVBoxLayout* QVB=new QVBoxLayout;
    ViewerWNd->setLayout(QVB);
    ZDomainObject wObject ;
    wObject.constructFromDP(wDP,pErrorLog);

    QLabel* wPixImageLBl = new QLabel(ViewerWNd);
    QVB->addWidget(wPixImageLBl);

    //            ZQLabel* wZQL=ContentObjectBroker.labelFactory("general.iconfactory/check.gif",ErrorLog);

    QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory.iconcheck",pErrorLog);
    QPushButton* PBClose=new QPushButton(wCheck,"",ViewerWNd);
    PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QVB->addWidget(PBClose,Qt::AlignRight);
    QObject::connect(PBClose,&QPushButton::clicked,ViewerWNd,&QWidget::close);
    //            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);
    ViewerWNd->move(QCursor::pos());
    QImage wImg (wObject.getFullPath(pErrorLog).toCChar());
    wPixImageLBl->setPixmap(QPixmap::fromImage(wImg));
    ViewerWNd->show();
    return true;
}
void textViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    QMainWindow* ViewerMWn = new QMainWindow(pFather);
    QWidget* ViewerWDg = new QWidget(pFather,Qt::ToolTip);
    ViewerMWn->setCentralWidget(ViewerWDg);


    QVBoxLayout* QVB=new QVBoxLayout;
    ViewerWDg->setLayout(QVB);
    ZDomainObject wObject ;
    wObject.constructFromDP(wDP,pErrorLog);

    QTextEdit* wTextTEd = new QTextEdit(ViewerWDg);
    QVB->addWidget(wTextTEd);


    QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory.iconcheck",pErrorLog);
    QPushButton* PBClose=new QPushButton(wCheck,"",ViewerWDg);
    PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QVB->addWidget(PBClose,Qt::AlignRight);
    QObject::connect(PBClose,&QPushButton::clicked,ViewerMWn,&QWidget::close);
    //            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);

    utf8VaryingString wTextContent;
    wObject.loadUtf8(wTextContent,pErrorLog);

    if (wTextContent.isEmpty()) {
        QTextCursor wCursor = wTextTEd->textCursor();
        QTextCharFormat wFmt = wCursor.charFormat();
        wFmt.setFontPointSize(24.0);
        wCursor.setCharFormat(wFmt);
        wTextTEd->setTextCursor(wCursor);
        wTextTEd->setText("\n\nNothing to display");
    }
    else
        wTextTEd->setText(wTextContent.toCChar());
    ViewerMWn->setWindowTitle(wObject.getHierarchy().toCChar());
    ViewerMWn->move(QCursor::pos());
    ViewerMWn->show();
    return ;
}

void erroredViewer( QWidget* pFather)
{
    ZaiErrors ErrorLog;
    //    QMainWindow* ViewerMWn = new QMainWindow(pFather);
    QWidget* ViewerWDg = new QWidget(pFather,Qt::ToolTip);
    //    ViewerMWn->setCentralWidget(ViewerWDg);


    QVBoxLayout* QVB=new QVBoxLayout;
    ViewerWDg->setLayout(QVB);
    /*
    ZDomainObject wObject ;
    wObject.constructFromDP(wDP,pErrorLog);
    */

    QTextEdit* wTextTEd = new QTextEdit(ViewerWDg);
    QVB->addWidget(wTextTEd);


    QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory.iconcheck",&ErrorLog);
    QPushButton* PBClose=new QPushButton(wCheck,"",ViewerWDg);
    PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QVB->addWidget(PBClose,Qt::AlignRight);
    QObject::connect(PBClose,&QPushButton::clicked,ViewerWDg,&QWidget::close);
    //            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);

    QTextCursor wCursor = wTextTEd->textCursor();
    QTextCharFormat wFmt = wCursor.charFormat();
    wFmt.setFontPointSize(24.0);
    wCursor.setCharFormat(wFmt);
    wTextTEd->setTextCursor(wCursor);
    wTextTEd->setText("\n\nNo viewer available");

    ViewerWDg->move(QCursor::pos());
    ViewerWDg->show();
    return ;
}

void htmlViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    QMainWindow* ViewerMWn = new QMainWindow(pFather);
    QWidget* ViewerWDg = new QWidget(pFather,Qt::ToolTip);
    ViewerMWn->setCentralWidget(ViewerWDg);


    QVBoxLayout* QVB=new QVBoxLayout;
    ViewerWDg->setLayout(QVB);
    ZDomainObject wObject ;
    wObject.constructFromDP(wDP,pErrorLog);

    QTextEdit* wTextTEd = new QTextEdit(ViewerWDg);
    QVB->addWidget(wTextTEd);


    QIcon wCheck = ContentObjectBroker.iconFactory("general.iconfactory.iconcheck",pErrorLog);
    QPushButton* PBClose=new QPushButton(wCheck,"",ViewerWDg);
    PBClose->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QVB->addWidget(PBClose,Qt::AlignRight);
    QObject::connect(PBClose,&QPushButton::clicked,ViewerMWn,&QWidget::close);
    //            QObject::connect(wZQL,&ZQLabel::clicked,ToolTipWDg,&QWidget::hide);

    utf8VaryingString wTextContent;
    wObject.loadUtf8(wTextContent,pErrorLog);

    if (wTextContent.isEmpty()) {
        QTextCursor wCursor = wTextTEd->textCursor();
        QTextCharFormat wFmt = wCursor.charFormat();
        wFmt.setFontPointSize(24.0);
        wCursor.setCharFormat(wFmt);
        wTextTEd->setTextCursor(wCursor);
        wTextTEd->setText("\n\nNothing to display");
    }
    else
        wTextTEd->setHtml(wTextContent.toCChar());
    ViewerMWn->setWindowTitle(wObject.getHierarchy().toCChar());
    ViewerMWn->move(QCursor::pos());
    ViewerMWn->show();
    return;
}

bool genericViewer (ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    ZDomain_type wType = wDP->Type & ZDOM_ObjectMask;
    switch (wType)
    {
    case ZDOM_Icon:
    case ZDOM_Image:
        imageViewer(wDP,pFather,pErrorLog);
        return true;
    case ZDOM_HtmlFile:
        htmlViewer(wDP,pFather,pErrorLog);
        return true;
    case ZDOM_TextFile:
        textViewer(wDP,pFather,pErrorLog);
        return true;
    case ZDOM_XmlFile:
        textViewer(wDP,pFather,pErrorLog);
        return true;
        /*    case ZDOM_PdfFile:
        textViewer(wDP,pFather,pErrorLog);
        return true;
*/
    default:
    {
        ZStatus wSt = externalViewer(wDP,pFather,pErrorLog);
        return wSt==ZS_SUCCESS;
    }
    }// switch
    return false;
}

bool genericEditor (ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    ZStatus wSt = externalEditor(wDP,pFather,pErrorLog);
    return wSt==ZS_SUCCESS;

}
/* launch appropriate viewer for domain object pointed by path wDP
 * viewer is selected according wDP Type
 */
ZStatus
externalViewer(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    ZDomain_type wObjTyp=wDP->Type & ZDOM_ObjectMask;

    switch (wObjTyp)
    {
    case ZDOM_Icon:
    case ZDOM_Image:
    case ZDOM_TextFile:
    case ZDOM_HtmlFile:
    case ZDOM_XmlFile:
        pErrorLog->errorLog("externalViewer-E-INVTYP Invalid object type <%s>",decode_ZDomain_type(wObjTyp).toCChar());
        return ZS_INVTYPE;
    default:
        break;
    }// switch


    ZDomainObject wViewer=ContentObjectBroker.getViewerFromType(wDP->Type,pErrorLog);
    if (wViewer.isNull()) {
        pErrorLog->errorLog("externalViewer-E-NOVIEWER Object type <%s> has no defined viewer.",
                            decode_ZDomain_type(wObjTyp).toCChar() );

        return ZS_NOTFOUND;
    }

    ZDomainObject wToDisplay;
    wToDisplay.constructFromDP(wDP,pErrorLog);

    uriString wUToDisplay = wToDisplay.getFullPath(pErrorLog);

    QProcess *wProcess = new QProcess(pFather);
    //    QString program = "dolphin";
    uriString wURIViewer = wViewer.getFullPath(pErrorLog);
    //    wProcess->start(wViewer.getFullPath(pErrorLog).toCChar(),
    wProcess->start(wURIViewer.toCChar(),
                    QStringList() << wUToDisplay.toCChar());
    return ZS_SUCCESS;
}

/* launch appropriate viewer for domain object pointed by path wDP
 * viewer is selected according wDP Type
 */
ZStatus
externalEditor(ZDomainPath* wDP, QWidget* pFather,ZaiErrors* pErrorLog)
{
    ZDomain_type wObjTyp=wDP->Type & ZDOM_ObjectMask;

    switch (wObjTyp)
    {
    case ZDOM_Icon:
    case ZDOM_Image:
    case ZDOM_Executable:
        pErrorLog->errorLog("externalEditor-E-INVTYP Invalid object type <%s>",decode_ZDomain_type(wObjTyp).toCChar());
        return ZS_INVTYPE;
    default:
        break;
    }// switch


    ZDomainObject wEditor=ContentObjectBroker.getEditorFromType(wDP->Type,pErrorLog);
    if (wEditor.isNull())
        return ZS_NOTFOUND;

    ZDomainObject wToDisplay;
    wToDisplay.constructFromDP(wDP,pErrorLog);

    uriString wUToDisplay = wToDisplay.getFullPath(pErrorLog);

    QProcess *wProcess = new QProcess(pFather);
    //    QString program = "dolphin";
    uriString wURIEditor = wEditor.getFullPath(pErrorLog);
    //    wProcess->start(wViewer.getFullPath(pErrorLog).toCChar(),
    wProcess->start(wURIEditor.toCChar(),
                    QStringList() << wUToDisplay.toCChar());
    return ZS_SUCCESS;
}


}// namespace zbs
