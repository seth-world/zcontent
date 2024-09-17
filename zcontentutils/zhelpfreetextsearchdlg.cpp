#include "zhelpfreetextsearchdlg.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QMenu>
#include <QFrame>
#include <QResizeEvent>

/* for text search within html document facilities */
#include <QTextDocument>
#include <QTextCursor>


#include <zexceptiondlg.h>

#include "zhelp.h"
#include <zqt/zqtwidget/zqtableview.h>

#include <zqt/zqtwidget/zqtwidgettools.h>
#include <zqt/zqtwidget/zqtwidgettypes.h>

#include <zcontentcommon/zgeneralparameters.h>

ZHelpFreetextSearchDLg::ZHelpFreetextSearchDLg(ZHelp *pZHelp) : QDialog((QWidget *)pZHelp)
{
    ZHelpPtr = pZHelp;
}

ZHelpFreetextSearchDLg::~ZHelpFreetextSearchDLg() {

}

void
ZHelpFreetextSearchDLg::initLayout()
{
  resize(900,361);

  AcceptBTn = new QPushButton(this);
  AcceptBTn->setText("Select");
  AcceptBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );

  RejectBTn = new QPushButton(this);
  RejectBTn->setText("Discard");
  RejectBTn->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Fixed );

  keywordTBv=new ZQTableView(this);
  keywordTBv->newModel(3); /* 3 columns */
  keywordTBv->setAlternatingRowColors(true);
  keywordTBv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  keywordTBv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected


  QVBoxLayout*  VBL=new QVBoxLayout(this);
  setLayout(VBL);

  QHBoxLayout* QHBL=new QHBoxLayout;
  VBL->addItem(QHBL);

  QHBL->addWidget(new QLabel("Domain"));

  DomainLBl = new QLabel("Domain name");
  QHBL->addWidget(DomainLBl);

  SearchFRm = new QFrame(this);

  SBL=new QHBoxLayout;
  SearchFRm->setLayout(SBL);

  ToSearch = new QLineEdit(this);
  SBL->addWidget(ToSearch);

  SBL->insertSpacing(-1,2);

  CaseSensitiveCHk = new QCheckBox("Case sensitive",this);
  SBL->addWidget(CaseSensitiveCHk);

  KeywordSearchBTn = new QPushButton("Search",this);
  SBL->addWidget(KeywordSearchBTn);



  VBL->addWidget(SearchFRm);
//  VBL->addItem(SBL);

  VBL->addWidget(keywordTBv);

  QHBoxLayout* HBL=new QHBoxLayout;
  HBL->setAlignment(Qt::AlignRight);
  HBL->addWidget(AcceptBTn);
  HBL->addWidget(RejectBTn);

  VBL->addLayout(HBL);


  int wCol=0;
  keywordTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("help keyword")));
  keywordTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("html file")));
  keywordTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("hits")));


  QObject::connect(KeywordSearchBTn,SIGNAL(clicked()),this,SLOT(KeywordSearchClicked()));
  QObject::connect(AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));
  QObject::connect(RejectBTn,SIGNAL(clicked()),this,SLOT(RejectClicked()));

  return ;
} // init()



void ZHelpFreetextSearchDLg::showKeywordsSetup(const utf8VaryingString& pDomain) {
    initLayout();

    KeywordSearchBTn->setVisible(false);
    ToSearch->setVisible(false);
    CaseSensitiveCHk->setVisible(false);

    ViewType = ZHFTSD_Keyword;

    DomainLBl->setText(pDomain.toCChar());

    perKeywordDisplay(pDomain);
}

void ZHelpFreetextSearchDLg::freeTextSetup(const utf8VaryingString& pDomain) {
    initLayout();

    KeywordSearchBTn->setVisible(true);
    ToSearch->setVisible(true);
    CaseSensitiveCHk->setVisible(true);

    ViewType = ZHFTSD_Freetext;

    DomainLBl->setText(pDomain.toCChar());
    ToSearch->setFocus();
}


int
ZHelpFreetextSearchDLg::_freeTextSearch(const utf8VaryingString& pToSearch,bool pCaseSentitve,ZArray<CFreeTextMatch> &pMatchList) {
    pMatchList.clear();
    utf8VaryingString wUtfContent;
    ZStatus wSt=ZS_SUCCESS;
    for (int wi=0; wi < ZHelpPtr->KeywordList.count();wi++) {
        CFreeTextMatch wMatch;
        wMatch.Count=0;
        wMatch.File=makeFullFilePath(ZHelpPtr->KeywordList[wi].getHtml());
        wSt = wMatch.File.loadUtf8(wUtfContent);
        if (wSt==ZS_SUCCESS) {
            utf8VaryingString::BSearchContext wSearchCtx;
            ssize_t wOffset=0;
            if (pCaseSentitve)
                wOffset=wUtfContent.bsearch(pToSearch.toUtf(),wSearchCtx);
            else
                wOffset=wUtfContent.bsearchCaseRegardless(pToSearch.toUtf(),wSearchCtx);

            while (wOffset > 0) {
                wMatch.Count++;
                if (pCaseSentitve)
                    wOffset=wUtfContent.bsearchNext(wSearchCtx);
                else
                    wOffset=wUtfContent.bsearchCaseRegardlessNext(wSearchCtx);

            }
            if (wMatch.Count > 0) {
                wMatch.Index = wi;
                pMatchList.push(wMatch);
            }
        }
    } // for
    return pMatchList.count();
} //freeTextSearch


int
ZHelpFreetextSearchDLg::_HtmlFreeTextSearch(const utf8VaryingString& pToSearch, bool pCaseSentitve, ZArray<CFreeTextMatch> &pMatchList) {
    QTextDocument wDoc(this);
    pMatchList.clear();
    utf8VaryingString wUtfContent;
    ZStatus wSt=ZS_SUCCESS;
    int wKeySize = pToSearch.strlen();

    for (int wi=0; wi < ZHelpPtr->KeywordList.count();wi++) {
        CFreeTextMatch wMatch;
        wMatch.Count=0;
        wMatch.File=makeFullFilePath(ZHelpPtr->KeywordList[wi].getHtml());
        wSt = wMatch.File.loadUtf8(wUtfContent);
        if (wSt==ZS_SUCCESS) {
            wDoc.setHtml(wUtfContent.toCChar());

            utf8VaryingString::BSearchContext wSearchCtx;
            int wOffset=0;
            QTextCursor wTCursor;
            if (pCaseSentitve)
                wTCursor =  wDoc.find(pToSearch.toCChar(),wOffset,QTextDocument::FindCaseSensitively);
            else
                wTCursor =  wDoc.find(pToSearch.toCChar(),wOffset);
            while (!wTCursor.isNull()) {
                wMatch.Count++;
                wOffset = wTCursor.position() + wKeySize;
                wTCursor.setPosition(wOffset);
                wDoc.cursorPositionChanged(wTCursor);
                if (pCaseSentitve)
                    wTCursor =  wDoc.find(pToSearch.toCChar(),wOffset,QTextDocument::FindCaseSensitively);
                else
                    wTCursor =  wDoc.find(pToSearch.toCChar(),wOffset);
            }
            if (wMatch.Count > 0) {
                wMatch.Index = wi;
                pMatchList.push(wMatch);
            }
        }
    } // for
    return pMatchList.count();
} //freeTextSearch

uriString
ZHelpFreetextSearchDLg::makeFullFilePath(const uriString& pHelpFile)
{
    uriString wMainHtml =  GeneralParameters.HelpDirectory ;
    wMainHtml.addConditionalDirectoryDelimiter();
    wMainHtml += pHelpFile;
    return wMainHtml;
}


void
ZHelpFreetextSearchDLg::freeTextDisplay()
{

    if (ToSearch->text().isEmpty()) {
        ZExceptionDLg::adhocMessage("Free text search",Severity_Error,nullptr,nullptr,"Nothing to search for.");
        return;
    }
    ZArray<CFreeTextMatch> wMatchList;
    utf8VaryingString wToSearch = ToSearch->text().toUtf8().data();

    _HtmlFreeTextSearch(wToSearch,CaseSensitiveCHk->isChecked(), wMatchList);

    if (keywordTBv->ItemModel==nullptr) {
        keywordTBv->newModel(3); /* 3 columns */
        keywordTBv->setAlternatingRowColors(true);
        keywordTBv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        keywordTBv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
    }
    else {
        keywordTBv->ItemModel->clear(); /* removes all columns lines and headers- sets line and column number to 0 */
        keywordTBv->setColumnCount(3);
    }

    int wCol=0;
    keywordTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Keyword")));
    keywordTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("help file")));
    keywordTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("match count")));


    if (wMatchList.count()==0) {
        QList<QStandardItem*> wRow;
        wRow << new QStandardItem("*** No match ***");
        keywordTBv->ItemModel->appendRow(wRow);
        keywordTBv->show() ;
        return ;
    }


    for (int wi=0; wi < wMatchList.count();wi++) {
        QVariant wV;
        wV.setValue(wMatchList[wi].Index);

        QList<QStandardItem*> wRow;

        wRow << new QStandardItem(ZHelpPtr->KeywordList[wMatchList[wi].Index].getKeyword().toCChar());


        wRow << new QStandardItem(wMatchList[wi].File.toCChar());
        wRow << createItem(wMatchList[wi].Count);

        wRow[0]->setData(QVariant(wMatchList[wi].Index),ZQtValueRole);

        keywordTBv->ItemModel->appendRow(wRow);
    }

    for (int wi=0;wi < keywordTBv->ItemModel->columnCount();wi++)
        keywordTBv->resizeColumnToContents(wi);
    for (int wi=0;wi < keywordTBv->ItemModel->rowCount();wi++)
        keywordTBv->resizeRowToContents(wi);

    keywordTBv->show();
    return;
} //freeTextDisplay

void
ZHelpFreetextSearchDLg::perKeywordDisplay(const utf8VaryingString& pDomain) {

    utf8VaryingString wToSearch = ToSearch->text().toUtf8().data();

    ToSearch->setText(pDomain.toCChar());
    KeywordSearchBTn->setVisible(false);

    if (keywordTBv->ItemModel==nullptr) {
        keywordTBv->newModel(2); /* 3 columns */
        keywordTBv->setAlternatingRowColors(true);
        keywordTBv->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        keywordTBv->setSelectionMode(QAbstractItemView::SingleSelection); // only one row can be selected
    }
    else {
        keywordTBv->ItemModel->clear(); /* removes all columns lines and headers- sets line and column number to 0 */
        keywordTBv->setColumnCount(2);
    }

    int wCol=0;
    keywordTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("Keyword")));
    keywordTBv->ItemModel->setHorizontalHeaderItem(wCol++,new QStandardItem(tr("help file")));


    if (ZHelpPtr->KeywordList.count()==0) {
        QList<QStandardItem*> wRow;
        wRow << new QStandardItem("*** No help available ***");
        keywordTBv->ItemModel->appendRow(wRow);
        keywordTBv->show() ;
        return ;
    }

    //    keywordTBv->ItemModel->removeRows(0,keywordTBv->ItemModel->rowCount());


    for (int wi=0; wi < ZHelpPtr->KeywordList.count();wi++) {
        QVariant wV(wi);
//        wV.setValue<int>(wi);

        QList<QStandardItem*> wRow;

        wRow << new QStandardItem(ZHelpPtr->KeywordList[wi].getKeyword().toCChar());

        wRow << new QStandardItem(ZHelpPtr->KeywordList[wi].getHtml().toCChar());
        wRow[0]->setData(wV,ZQtValueRole);

        keywordTBv->ItemModel->appendRow(wRow);
    }

    keywordTBv->show();
    return;

} //freeTextDisplay


void
ZHelpFreetextSearchDLg::KeywordSearchClicked(){

    freeTextDisplay();
}


void
ZHelpFreetextSearchDLg::AcceptClicked() {
  QVariant                wV;
  if (!keywordTBv->currentIndex().isValid()) {
    ZExceptionDLg::adhocMessage("Keyword selection",Severity_Error,nullptr,nullptr,"No keyword has been selected.");
    reject();
  }
  QModelIndex wIdx=keywordTBv->currentIndex();
  QStandardItem* wItem = keywordTBv->ItemModel->item(wIdx.row(),0);
  if (wItem==nullptr) {
      /* if key field (no data) then get its parent */
      ZExceptionDLg::adhocMessage("Keyword selection",Severity_Error,nullptr,nullptr,"Please select a valid keyword.");
      return;
  }
  if (wItem->data(ZQtValueRole).isNull()) {
    /* if key field (no data) then get its parent */
    ZExceptionDLg::adhocMessage("Keyword selection",Severity_Error,nullptr,nullptr,"Please select a valid keyword.");
    return;
  }
  int wIndex=wItem->data(ZQtValueRole).toInt();

  uriString wHelpFile = GeneralParameters.getHelpDirectoryChar();
  wHelpFile.addConditionalDirectoryDelimiter();
  wHelpFile += ZHelpPtr->KeywordList[wIndex].getHtml();

  if (!wHelpFile.exists()) {
      ZExceptionDLg::adhocMessage("Help file missing",Severity_Error,nullptr,nullptr,"Unfortunately help file <%s> does not exist." , wHelpFile.toCChar());
      return;
  }

  if (ToSearch->text().isEmpty())
    ZHelpPtr->setHtmlSource(wHelpFile);
  else {
      utf8VaryingString wToSearch = ToSearch->text().toUtf8().data();
      ZHelpPtr->setHtmlSourceWithHighlight(wHelpFile,wToSearch);
  }
  accept();
}

void
ZHelpFreetextSearchDLg::RejectClicked() {
  reject();
}


void
ZHelpFreetextSearchDLg::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QDialog::resize(pEvent->size().width(),pEvent->size().height());

  if (_FResizeInitial) {
    _FResizeInitial=false;
    return;
  }
  QRect wR1 = geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  keywordTBv->resize(wVW,wVH);  /* expands in width and height */

}//ZDicDLg::resizeEvent

