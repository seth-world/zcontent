#include "zhelp.h"
//#include "ui_ZHelp.h"

#include <ztoolset/uristring.h>
#include <ztoolset/utfvaryingstring.h>
#include <QtWidgets/QTextBrowser>
#include <QTextDocument>

#include <zcontent/zcontentutils/zexceptiondlg.h>

#include <zcppparser/zcppparsertype.h> // for getParserWorkDirectory()

#include <zcontentcommon/zgeneralparameters.h>

#include <QTextCursor>
#include <QTextBlock>
#include <QWidget>

#include <QAction>
#include <QMenu>
#include <QActionGroup>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QLineEdit>
#include <QCoreApplication>

#include <zcontentcommon/zgeneralparameters.h>
#include <ztoolset/zaierrors.h>

#include <zqt/zqtwidget/zqtextbrowser.h>
#include <zcontent/zcontentcommon/zgeneralparameters.h>

#include <zxml/zxml.h>
#include <zxml/zxmlprimitives.h>

#include "zexceptiondlg.h"
#include <ztoolset/utfmetafunctions.h>

#include <zqt/zqtwidget/zqtableview.h>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDialog>

#include "zhelpfreetextsearchdlg.h"


utf8VaryingString NoHelp = //"?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN\" \"http://www.w3.org/Math/DTD/mathml2/xhtml-math11-f.dtd\">"
    "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en-US\">"
    "<head profile=\"http://dublincore.org/documents/dcmi-terms/\">"
    "<meta name=\"xsl:vendor\" content=\"libxslt\"/>"
    "<style>"
    "table { border-collapse:collapse; border-spacing:0; empty-cells:show }"
"td, th { vertical-align:top; font-size:12pt;}"
"h1, h2, h3, h4, h5, h6 { clear:both;}"
"ol, ul { margin:0; padding:0;}"
"li { list-style: none; margin:0; padding:0;}"
"span.footnodeNumber { padding-right:1em; }"
"span.annotation_style_by_filter { font-size:95%; font-family:Arial; background-color:#fff000;  margin:0; border:0; padding:0;  }"
"span.heading_numbering { margin-right: 0.8rem; }* { margin:0;}"
".paragraph-P2{ font-size:12pt; font-family:'Liberation Serif'; writing-mode:horizontal-tb; direction:ltr;}"
".paragraph-P3_borderStart{ background-color:#aadcf7; border-left-style:none; border-right-style:none; border-top-width:0.0133cm; border-top-style:solid; border-top-color:#355269; font-size:18pt; font-weight:normal; margin-top:0.1665in; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;padding-bottom:0.0835in;  border-bottom-style:none; }"
".paragraph-P3_borderSides{ background-color:#aadcf7; border-left-style:none; border-right-style:none; font-size:18pt; font-weight:normal; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;padding-bottom:0.0835in; padding-top:0.1665in;  border-top-style:none; border-bottom-style:none; }"
".paragraph-P3_borderEnd{ background-color:#aadcf7; border-bottom-width:0.0133cm; border-bottom-style:solid; border-bottom-color:#355269; border-left-style:none; border-right-style:none; font-size:18pt; font-weight:normal; margin-bottom:0.0835in; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;padding-top:0.1665in;  border-top-style:none;}"
".paragraph-P3{ background-color:#aadcf7; border-bottom-width:0.0133cm; border-bottom-style:solid; border-bottom-color:#355269; border-left-style:none; border-right-style:none; border-top-width:0.0133cm; border-top-style:solid; border-top-color:#355269; font-size:18pt; font-weight:normal; margin-bottom:0.0835in; margin-top:0.1665in; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;}"
".paragraph-P5{ font-size:48pt; font-family:'Liberation Serif'; writing-mode:horizontal-tb; direction:ltr;text-align:center ! important; }"
/* ODF styles with no properties representable as CSS:
    .dp1  { } */
"</style>"
"    </head>"

"    <body dir=\"ltr\" style=\"max-width:8.2681in;margin-top:0.7874in; margin-bottom:0.7874in; margin-left:0.7874in; margin-right:0.7874in; \">"
    "<h1 class=\"paragraph-P3\"><a id=\"a__Help\"><span/></a>Help</h1>"
    "<p class=\"paragraph-P2\"> </p> <p class=\"paragraph-P2\"> </p>"
    "<p class=\"paragraph-P2\"> </p> <p class=\"paragraph-P2\"> </p>"
    "<p class=\"paragraph-P2\"> </p> <p class=\"paragraph-P2\"> </p> <p class=\"paragraph-P2\"> </p><p class=\"paragraph-P2\"> </p><p class=\"paragraph-P2\"> </p><p class=\"paragraph-P2\"> </p><p class=\"paragraph-P2\"> </p><p class=\"paragraph-P2\"> </p>"
    "<p class=\"paragraph-P5\">No help available</p>"
    "</body>"
    "</html>";




utf8VaryingString HelpListHeader =
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN\" \"http://www.w3.org/Math/DTD/mathml2/xhtml-math11-f.dtd\">\n"
    "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en-US\">\n"
    "<style>\n"
    "table { border-collapse:collapse; border-spacing:0; empty-cells:show ;  table-layout:auto ;  }\n"
    "th { font-size:18pt; font-weight:normal; margin-top:0.1665in; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;padding-bottom:0.0835in; "
            "border: 4px double #999; "
            "padding-top: 10px; padding-left: 10px; padding-bottom: 10px; padding-right: 10px; "
            "background-color:#aadcf7; "
//            "border: double; "
            "}\n"
    "td { text-align:left; "
                "border: 4px double #999; font-family:'Liberation Sans'; font-size:12pt; font-weight:normal;  margin: 10px; "
//    "border: double; "
    "padding-top: 10px; padding-left: 10px; padding-bottom: 10px; padding-right: 10px; "
    "}\n"


    "h1, h2, h3, h4, h5, h6 { clear:both;}\n"
    "ol, ul { margin:0; padding:0;}\n"
    "li { list-style: none; margin:0; padding:0;}\n"
    "span.footnodeNumber { padding-right:1em; }\n"
    "span.annotation_style_by_filter { font-size:95%; font-family:Arial; background-color:#fff000;  margin:0; border:0; padding:0;  }\n"
    "span.heading_numbering { margin-right: 0.8rem; }* { margin:0;}\n"

    ".paragraph-PHeader{ font-size:18pt; font-family:'Liberation Serif'; writing-mode:horizontal-tb; direction:ltr;"
//                        "background-color:#aadcf7; "
                        "border-left-style:solid; border-left-color:#fff000; "
                        "border-right-style:solid; border-right-color:#355269; "
                        "border-top-style:solid; border-top-color:#355269; border-top-width:0.02cm; "
                        "border-bottom-style:solid; border-bottom-color:#355269; "
                        "font-size:18pt; font-weight:normal; margin-top:0.1665in; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;padding-bottom:0.0835in;  "
                        "}\n"

    ".paragraph-P2{ font-size:24pt; font-family:'Liberation Serif'; writing-mode:horizontal-tb; direction:ltr;}\n"
    ".paragraph-P3_borderStart{ background-color:#aadcf7; border-left-style:none; border-right-style:none; border-top-width:0.0133cm; border-top-style:solid; border-top-color:#355269; font-size:18pt; font-weight:normal; margin-top:0.1665in; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;padding-bottom:0.0835in;  border-bottom-style:none; }\n"
    ".paragraph-P3_borderSides{ background-color:#aadcf7; border-left-style:none; border-right-style:none; font-size:18pt; font-weight:normal; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;padding-bottom:0.0835in; padding-top:0.1665in;  border-top-style:none; border-bottom-style:none; }\n"
    ".paragraph-P3_borderEnd{ background-color:#aadcf7; border-bottom-width:0.0133cm; border-bottom-style:solid; border-bottom-color:#355269; border-left-style:none; border-right-style:none; font-size:18pt; font-weight:normal; margin-bottom:0.0835in; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;padding-top:0.1665in;  border-top-style:none;}\n"
    ".paragraph-P3{ background-color:#aadcf7; border-bottom-width:0.0133cm; border-bottom-style:solid; border-bottom-color:#355269; border-left-style:none; border-right-style:none; border-top-width:0.0133cm; border-top-style:solid; border-top-color:#355269; font-size:18pt; font-weight:normal; margin-bottom:0.0835in; margin-top:0.1665in; padding:0.0201in; text-align:center ! important; font-family:'Liberation Sans'; writing-mode:horizontal-tb; direction:ltr;}\n"
    ".paragraph-P5{ font-size:48pt; font-family:'Liberation Serif'; writing-mode:horizontal-tb; direction:ltr;text-align:center ! important; }\n"
    ".Internet_20_link { font-family:'Liberation Serif'; color:#000080; text-decoration:underline; }\n"
    /* ODF styles with no properties representable as CSS:
    .dp1  { } */
    "</style>\n"
    "    </head>\n"
    "    <body dir=\"ltr\" style=\"max-width:8.2681in;margin-top:0.7874in; margin-bottom:0.7874in; margin-left:0.7874in; margin-right:0.7874in; \">\n"
    "<h1 class=\"paragraph-P3\"><a id=\"a__Help\"><span/></a>Help</h1>\n"

    "<p class=\"paragraph-P2\"> </p> <p class=\"paragraph-P2\"> </p>\n"
    "<table  cols=\"2\" align=\"center\">\n"
    "<tr>\n"
    "<th>keyword</th>\n"
    "<th>description</th>\n"
    "</tr>\n" ;

utf8VaryingString HelpListBody =
    "<tr>\n"
    "<td><a href=\"%s\" class=\"Internet_20_link\">%s</a></td>\n" // html file - keyword
    "<td>%s</td>\n" // brief
    "</tr>\n" ;

utf8VaryingString HelpListTrailer =
    "</table>"
    "</body>"
    "</html>";


namespace zbs {

void
ZHelp::_init(uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack)) {

 // setupUi(this);

    if (objectName().isEmpty())
        setObjectName("ZHelp");
    resize(813, 623);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setSizeIncrement(QSize(3, 3));
    centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralwidget");
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
    centralwidget->setSizePolicy(sizePolicy1);
    verticalLayoutWidget = new QWidget(centralwidget);
    verticalLayoutWidget->setObjectName("verticalLayoutWidget");
    verticalLayoutWidget->setGeometry(QRect(0, 10, 811, 571));
    verticalLayout = new QVBoxLayout(verticalLayoutWidget);
    verticalLayout->setObjectName("verticalLayout");
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName("horizontalLayout");


    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName("horizontalLayout_2");
    searchBTn = new QPushButton(verticalLayoutWidget);
    searchBTn->setObjectName("searchBTn");

    uriString wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "magnifyingglass.png";
/**/
    QIcon icon;
    icon.addFile(wIcon.toCChar(), QSize(), QIcon::Normal, QIcon::Off);
    searchBTn->setIcon(icon);

    horizontalLayout_2->addWidget(searchBTn);

    searchLEd = new QLineEdit(verticalLayoutWidget);
    searchLEd->setObjectName("searchLEd");

    horizontalLayout_2->addWidget(searchLEd);


    horizontalLayout->addLayout(horizontalLayout_2);

    searchMainBTn = new QPushButton(verticalLayoutWidget);
    searchMainBTn->setObjectName("searchMainBTn");
#if QT_CONFIG(tooltip)
    searchMainBTn->setToolTip("set search on/off");
#endif // QT_CONFIG(tooltip)

    wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "strigi.png";

    QIcon icon1;
    icon1.addFile(wIcon.toCChar(), QSize(), QIcon::Normal, QIcon::Off);
    searchMainBTn->setIcon(icon1);

    horizontalLayout->addWidget(searchMainBTn);

    filterBTn = new QPushButton(verticalLayoutWidget);
    filterBTn->setObjectName("filterBTn");

    wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "funel.png";

    QIcon icon2;
    icon2.addFile(wIcon.toCChar(), QSize(), QIcon::Normal, QIcon::Off);
    filterBTn->setIcon(icon2);

    horizontalLayout->addWidget(filterBTn);
/*
    lineNumbersBTn = new QPushButton(verticalLayoutWidget);
    lineNumbersBTn->setObjectName("lineNumbersBTn");

    horizontalLayout->addWidget(lineNumbersBTn);
*/
    wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "minihouse.png";
    QIcon wIcn1;
    wIcn1.addFile(wIcon.toCChar(), QSize(), QIcon::Normal, QIcon::Off);
    homeBTn = new QPushButton(this);
    homeBTn->setIcon(wIcn1);
    horizontalLayout->addWidget(homeBTn);

    wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "backward_black.png";
    QIcon wIcn3;
    wIcn3.addFile(wIcon.toCChar(), QSize(), QIcon::Normal, QIcon::Off);
    backwardBTn = new QPushButton(this);
    backwardBTn->setIcon(wIcn3);

    horizontalLayout->addWidget(backwardBTn);

    wIcon = GeneralParameters.getIconDirectory() ;
    wIcon.addConditionalDirectoryDelimiter();
    wIcon += "forward_black.png";
    QIcon wIcn4;
    wIcn4.addFile(wIcon.toCChar(), QSize(), QIcon::Normal, QIcon::Off);
    forwardBTn = new QPushButton(this);
    forwardBTn->setIcon(wIcn4);

    horizontalLayout->addWidget(forwardBTn);


    wrapBTn = new QPushButton(verticalLayoutWidget);
    wrapBTn->setObjectName("wrapBTn");

    horizontalLayout->addWidget(wrapBTn);

    closeBTn = new QPushButton(verticalLayoutWidget);
    closeBTn->setObjectName("closeBTn");

    horizontalLayout->addWidget(closeBTn);

    verticalLayout->addLayout(horizontalLayout);

    TextTBr = new ZQTextBrowser(verticalLayoutWidget);
    TextTBr->setObjectName("TextTBr");
    sizePolicy1.setHeightForWidth(TextTBr->sizePolicy().hasHeightForWidth());
    TextTBr->setSizePolicy(sizePolicy1);
    TextTBr->setSizeIncrement(QSize(0, 0));
    QFont font1;
    font1.setFamilies({QString::fromUtf8("Courier")});
    font1.setPointSize(10);
    font1.setBold(true);
    TextTBr->setFont(font1);
    TextTBr->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    TextTBr->setLineWrapMode(QTextBrowser::WidgetWidth);

    verticalLayout->addWidget(TextTBr);

    this->setCentralWidget(centralwidget);
    statusbar = new QStatusBar(this);
    statusbar->setObjectName("statusbar");
    this->setStatusBar(statusbar);
    menubar = new QMenuBar(this);
    menubar->setObjectName("menubar");
    menubar->setGeometry(QRect(0, 0, 813, 20));
    this->setMenuBar(menubar);

    this->setWindowTitle(QCoreApplication::translate("ZHelp", "MainWindow", nullptr));
    searchBTn->setText(QString());
    searchMainBTn->setText(QString());
    filterBTn->setText(QString());
//    lineNumbersBTn->setText(QCoreApplication::translate("ZHelp", "Line numbers On", nullptr));
    wrapBTn->setText(QCoreApplication::translate("ZHelp", "No wrap", nullptr));
    closeBTn->setText(QCoreApplication::translate("ZHelp", "Close", nullptr));
#if QT_CONFIG(tooltip)
    TextTBr->setToolTip(QCoreApplication::translate("ZHelp", "Filter content", nullptr));
#endif // QT_CONFIG(tooltip)


  Options = pOptions;

  genMEn = new QMenu("General",this);

  menubar->addMenu(genMEn);

  menuActionQAg = new QActionGroup(this);
  menuActionQAg->setExclusive(false);
/*
  lineNbQAc = new QAction("line numbers",genMEn);
  lineNbQAc->setCheckable(true);
  lineNbQAc->setChecked(false);
*/
  wrapQAc = new QAction("word wrap",genMEn);
  wrapQAc->setCheckable(true);
  wrapQAc->setChecked(false);

  writeQAc = new QAction("write to text file",genMEn);

  clearQAc = new QAction("clear",genMEn);

//  genMEn->addAction(lineNbQAc);
  genMEn->addAction(wrapQAc);

  genMEn->addSeparator();

  ShowKeywordsQAc = new QAction("show help keywords <ctrl-h>",genMEn);
  SearchKeywordsQAc = new QAction("freetext search for help <ctrl-f>",genMEn);
//  HideKeywordsQAc = new QAction("hide keyword panel",genMEn);
  genMEn->addAction(ShowKeywordsQAc);
  genMEn->addAction(SearchKeywordsQAc);
//  genMEn->addAction(HideKeywordsQAc);
  menuActionQAg->addAction(ShowKeywordsQAc);
  menuActionQAg->addAction(SearchKeywordsQAc);
//  menuActionQAg->addAction(HideKeywordsQAc);

  genMEn->addSeparator();

  genMEn->addAction(writeQAc);
  genMEn->addAction(clearQAc);

  genMEn->addSeparator();

//  menuActionQAg->addAction(lineNbQAc);
  menuActionQAg->addAction(wrapQAc);
  menuActionQAg->addAction(writeQAc);
  menuActionQAg->addAction(clearQAc);

  genMEn->addSeparator();

  zoomInQAc = new QAction("Zoom in",genMEn);
  zoomOutQAc = new QAction("Zoom out",genMEn);

  genMEn->addAction(zoomInQAc);
  genMEn->addAction(zoomOutQAc);

  menuActionQAg->addAction(zoomInQAc);
  menuActionQAg->addAction(zoomOutQAc);

  HistoryMEn = new QMenu("History");
  menubar->addMenu(HistoryMEn);

  HomeQAc = new QAction("Home",HistoryMEn);
  HistoryMEn->addAction(HomeQAc);
  menuActionQAg->addAction(HomeQAc);
  BackwardQAc = new QAction("Backward",HistoryMEn);
  HistoryMEn->addAction(BackwardQAc);
  menuActionQAg->addAction(BackwardQAc);
  ForwardQAc = new QAction("Forward",HistoryMEn);
  HistoryMEn->addAction(ForwardQAc);
  menuActionQAg->addAction(ForwardQAc);
/*
  TextTBr->setUseLineNumbers(Options & TEOP_ShowLineNumbers);
  if (TextTBr->getUseLineNumbers())
    lineNumbersBTn->setText(tr("Hide line numbers"));
  else
    lineNumbersBTn->setText(tr("Show line numbers"));
*/
  CloseCallBack = pCloseCallBack;

  setAttribute(Qt::WA_DeleteOnClose , true);

  TextTBr->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  TextTBr->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

//  TextTBr->setCenterOnScroll(true);

  TextTBr->setWordWrapMode(QTextOption::NoWrap);
/*
  Cursor = new QTextCursor(Text->textCursor());
  FmtDefault = new QTextCharFormat(Cursor->charFormat());
  delete Cursor;
  Cursor=nullptr;
*/
  filterBTn->setVisible(false);

  searchMainBTn->setVisible(true);
  searchBTn->setVisible(false);
  searchLEd->setVisible(false);

  wrapBTn->setText(QObject::tr("Wrap","ZHelp"));

  QMainWindow::setWindowTitle("Help");



//  QObject::connect(lineNumbersBTn, SIGNAL(pressed()), this, SLOT(lineNumbersBTnClicked()));
  QObject::connect(closeBTn, SIGNAL(pressed()), this, SLOT(closePressed()));
  QObject::connect(wrapBTn, SIGNAL(pressed()), this, SLOT(wrapPressed()));
  QObject::connect(filterBTn, SIGNAL(pressed()), this, SLOT(filterPressed()));

  QObject::connect(searchMainBTn, SIGNAL(pressed()), this, SLOT(searchMainPressed()));
  QObject::connect(searchBTn, SIGNAL(pressed()), this, SLOT(searchPressed()));

  QObject::connect(searchLEd, SIGNAL(returnPressed()), this, SLOT(searchReturnPressed()));

  QObject::connect(menuActionQAg, &QActionGroup::triggered, this, &ZHelp::MenuAction);


  QObject::connect(homeBTn, SIGNAL(pressed()), TextTBr, SLOT(home()));
  QObject::connect(backwardBTn, SIGNAL(pressed()), TextTBr, SLOT(backward()));
  QObject::connect(searchBTn, SIGNAL(pressed()), TextTBr, SLOT(forward()));

  if (pOptions & TEOP_NoCloseBtn) {
    setWindowFlag(Qt::WindowCloseButtonHint,false ); // no close button
    closeBTn->setVisible(false);
  }

  setCloseButtonRole();
}

ZHelp::ZHelp(QWidget *parent) :QMainWindow(parent)//,ui(new Ui::ZHelp)
{
  _init(TEOP_Nothing,nullptr);
}


ZHelp::ZHelp(QWidget *parent,uint32_t pOptions, __CLOSE_CALLBACK__(pCloseCallBack)) :QMainWindow(parent)//,ui(new Ui::ZHelp)
{
  _init(pOptions,pCloseCallBack);
}
ZHelp::ZHelp( const utf8VaryingString& pTitle,QWidget *parent) :QMainWindow(parent)//,ui(new Ui::ZHelp)
{
    _init(TEOP_Nothing,nullptr);
    setWindowTitle(pTitle.toCChar());

}

ZHelp::~ZHelp()
{
    if (hasErrorLog())
        ErrorLog->clearDisplayCallBacks();
 // delete ui;
}

void
ZHelp::MenuAction(QAction* pAction) {
  if (pAction==quitQAc) {
    closePressed();
    return;
  }

  if (pAction==HomeQAc) {
      TextTBr->home();
      return;
  }

  if (pAction==BackwardQAc) {
      TextTBr->backward();
      return;
  }
  if (pAction==ForwardQAc) {
      TextTBr->forward();
      return;
  }


/*  if (pAction==lineNbQAc) {
    if (lineNbQAc->isChecked()){
      if (hasLineNumbers())
        return;
      lineNumbersOnOff();
      return;
    }
    if (!hasLineNumbers())
      return;
    lineNumbersOnOff();
    return;
  }// lineNbQAc
*/
  if (pAction==wrapQAc) {
    if (wrapQAc->isChecked()){
      if (TextTBr->wordWrapMode()==QTextOption::WordWrap)
        return;
      wordWrap();
      return;
    }
    if (TextTBr->wordWrapMode()==QTextOption::NoWrap)
      return;
    wordWrap();
    return;
  }// wrapQAc

  if (pAction==ShowKeywordsQAc) {
      ZHelpFreetextSearchDLg* wFTS=new ZHelpFreetextSearchDLg(this);
      wFTS->setWindowTitle("Show keywords");
      wFTS->showKeywordsSetup(HelpDomain);
      wFTS->show();
      return;
  } //ShowKeywordsQAc

  if (pAction==SearchKeywordsQAc) {
      ZHelpFreetextSearchDLg* wFTS=new ZHelpFreetextSearchDLg(this);
      wFTS->setWindowTitle("Help free text search");
      wFTS->freeTextSetup(HelpDomain);
      wFTS->show();
      return;

  } //ShowKeywordsQAc
  /*
  if (pAction==HideKeywordsQAc) {
      KeywordTRv->setVisible(false);
  }
*/
  if (pAction==writeQAc) {
    uriString wDir = GeneralParameters.getWorkDirectory();
    QString wFileName = QFileDialog::getSaveFileName(this, "text log file",
        wDir.toCChar(),
        "Text files (*.txt *.log);;All (*.*)");
    if (wFileName.isEmpty())
      return;
    uriString wFN=wFileName.toUtf8().data();
    QString wC=TextTBr->toPlainText();
    utf8VaryingString wTS=wC.toUtf8().data();
    ZStatus wSt=wFN.writeContent(wTS);
    if (wSt!=ZS_SUCCESS) {
      ZExceptionDLg::displayLast("Saving text file");
      return;
    }

  }//writeQAc

  if (pAction==clearQAc) {
    TextTBr->clear();
  } //clearQAc


  if (pAction==zoomInQAc) {
    QFont wF=TextTBr->font();
    qreal wFwght = wF.pointSizeF();
    wF.setPointSizeF(++wFwght);
    TextTBr->setFont(wF);
    return;
  }//zoomInQAc

  if (pAction==zoomOutQAc) {
    QFont wF=TextTBr->font();
    qreal wFwght = wF.pointSizeF();
    wF.setPointSizeF(--wFwght);
    TextTBr->setFont(wF);
    return;
  }//zoomInQAc

} //MenuAction
void ZHelp::displayNoHelp()
{
    uriString wURINoHelp="nohelp.html";
    wURINoHelp.writeContent(NoHelp);
    TextTBr->setSource(QString(wURINoHelp.toCChar()),QTextDocument::HtmlResource);
    TextTBr->show();
    return;
}

void ZHelp::setHtmlSource(const uriString& pSource) {
    if (!pSource.exists()) {
        displayNoHelp();
        return;
    }
    TextTBr->setSource(QString(pSource.toCChar()),QTextDocument::HtmlResource);
    TextTBr->show();
}
void ZHelp::setSource(const uriString& pSource) {
    TextTBr->setSource(QString(pSource.toCChar()));
    TextTBr->show();
}

void
ZHelp::setHtmlSourceWithHighlight(const uriString& pSource,const utf8VaryingString& pSearch)
{
    setSource(pSource);
    highlightAll(pSearch,Qt::yellow,Qt::black);
}

/*
void ZHelp::useLineNumbers(bool pOnOff) {
  TextTBr->setUseLineNumbers(pOnOff);
}
*/

ZStatus ZHelp::setup(const utf8VaryingString& pDomain,ZaiErrors *pErrorLog)
{
    ErrorLog = pErrorLog;
    HelpDomain = pDomain;
    uriString wXmlFile = RootDirectory = GeneralParameters.HelpDirectory ;
    wXmlFile.addConditionalDirectoryDelimiter();
    wXmlFile += "zhelp.xml" ;
    ZStatus wSt = XmlLoad(wXmlFile,HelpDomain,ErrorLog);
 //   HelpMainHtml = makeFullFilePath(HelpMainHtml);
    uriString wMainHtml = RootDirectory = GeneralParameters.HelpDirectory ;
    wMainHtml.addConditionalDirectoryDelimiter();
    wMainHtml += HelpMainHtml;
    HelpMainHtml = wMainHtml;

    uriString wMainHeader = GeneralParameters.HelpDirectory;
    wMainHeader.addConditionalDirectoryDelimiter();
    wMainHeader += "_main.html";

    utf8VaryingString wMainContent = HelpListHeader;
    for (int wi=0; wi < KeywordList.count();wi++) {
        uriString wHtml = GeneralParameters.HelpDirectory;
        wHtml.addConditionalDirectoryDelimiter();
        wHtml += KeywordList[wi].Html;
        wMainContent.addsprintf(HelpListBody.toCChar(),
                                wHtml.toCChar(),                    // first full html file
                                KeywordList[wi].Keyword.toCChar(),  // second keyword (will be the link)
                                KeywordList[wi].Brief.toCChar()     // third brief description
                                );
    }

    wMainContent += HelpListTrailer ;

    wMainHeader.writeContent(wMainContent);




    setHtmlSource(wMainHeader);
//    displayNoHelp();
    return wSt;
} // ZHelp::setup

/*
?xml version='1.0' encoding='UTF-8'?>
 <zhelp version = "'0.0-1'">
   <query>    <!-- Domain There could be multiple domains in the same xml help file -->
      <helpmain>querygeneralindex.html</helpmain>
      <helplist>
        <helpitem>
          <keyword>declare</keyword>
          <file>declare.html</file>
        </helpitem>
        <helpitem>
          <keyword>join</keyword>
          <file>join.html</file>
        </helpitem>
        <helpitem>
          <keyword>set</keyword>
          <file>set.html</file>
        </helpitem>
        <helpitem>
          <keyword>display</keyword>
          <file>display.html</file>
        </helpitem>
        <helpitem>
          <keyword>field_specifiers</keyword>
          <file>field_specifiers.html</file>
        </helpitem>
        <helpitem>
           <keyword>literal_specifiers</keyword>
          <file>literal_specifiers.html</file>
        </helpitem>
        <helpitem>
           <keyword>clear</keyword>
          <file>clear.html</file>
        </helpitem>
      </helplist>
   </query>
 </zhelp>


 */

ZStatus ZHelp::XmlLoad(uriString& pXmlFile,const utf8VaryingString& pDomain,ZaiErrors* pErrorLog)
{
    utf8VaryingString wXmlString;

    ZStatus wSt;
    if (pErrorLog!=nullptr)
        pErrorLog->setAutoPrintOn(ZAIES_Text);

    if (!pXmlFile.exists())  {
//        ZException.setMessage("ZGeneralParameters::XMLLoad",ZS_FILENOTEXIST,Severity_Error,"Parameter file <%s> has not been found.",pXmlFile.toCChar());
        if (pErrorLog!=nullptr) {
            pErrorLog->errorLog("ZHelp::XMLLoad-E-FILNFND Help parameter file <%s> has not been found. No help available.",pXmlFile.toCChar());
        }
        return ZS_FILENOTEXIST;
    }


    if ((wSt=pXmlFile.loadUtf8(wXmlString))!=ZS_SUCCESS) {
        if (pErrorLog!=nullptr) {
            pErrorLog->logZExceptionLast();
        }
        return wSt;
    }

    utf8VaryingString wVerbose;
    zxmlDoc     *wDoc = nullptr;
    zxmlElement *wRoot = nullptr;
    zxmlElement *wDomainNode=nullptr;
    zxmlElement *wDomainRootNode=nullptr;
    zxmlElement *wItemNode=nullptr;
    zxmlElement *wKeywordListNode=nullptr;
    zxmlElement *wKeyWordNode=nullptr;
    zxmlElement *wSwapNode=nullptr;

    //  zxmlElement *wTypeNode=nullptr;
    zxmlElement *wIncludeFileNode=nullptr;

    utf8VaryingString wKeyword;

    ZTypeBase             wZType;

    utf8VaryingString     wIncludeFile;

    wDoc = new zxmlDoc;
    wSt = wDoc->ParseXMLDocFromMemory(wXmlString.toCChar(), wXmlString.getUnitCount(), nullptr, 0);
    if (wSt != ZS_SUCCESS) {
        if (pErrorLog!=nullptr) {
            pErrorLog->logZExceptionLast();
            pErrorLog->errorLog(
                "ZGeneralParameters::XMLLoad-E-PARSERR Xml parsing error for string <%s> ",
                wXmlString.subString(0, 25).toString());
        }
        return wSt;
    }

    wSt = wDoc->getRootElement(wRoot);
    if (wSt != ZS_SUCCESS) {
        if (pErrorLog!=nullptr)
            pErrorLog->logZExceptionLast();
        return wSt;
    }
    if (!(wRoot->getName() == "zhelp")) {
/*        ZException.setMessage("ZHelp::XMLLoad",
                              ZS_XMLINVROOTNAME,
                              Severity_Error,
                              "Invalid root name <%s> expected <zhelp> - file <%s>.",
                              wRoot->getName().toString(),
                              pXmlFile.toCChar());
*/
 //       if (pErrorLog!=nullptr)
            pErrorLog->errorLog(
                "ZHelp::XMLLoad-E-INVROOT Invalid root node name <%s> expected <zhelp> - file <%s>",
                wRoot->getName().toString(),pXmlFile.toCChar());
        return ZS_XMLINVROOTNAME;
    }

    /* get Help Domain list */

    wSt=wRoot->getFirstChild((zxmlNode*&)wDomainNode);

    DomainList.clear();
    wDomainRootNode = nullptr;
    while (wSt==ZS_SUCCESS) {
        DomainList.push(wDomainNode->getName());
        if (wDomainNode->getName()==pDomain) {
            wDomainRootNode = wDomainNode;
        }
        wSwapNode = wDomainNode;
        wSt=wDomainNode->getNextNode((zxmlNode*&)wSwapNode);
        if(wDomainNode!=wDomainRootNode)
            XMLderegister(wDomainNode);
    }

    if (wDomainRootNode==nullptr) {
        pErrorLog->logZStatus(
            ZAIES_Error,
            wSt,
            "ZHelp::loadGenerateParameters-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pDomain.toCChar(),
            decode_ZStatus(wSt));
        return wSt;
    }

    /*------------------ Get domain node  -----------------------*/
/*    wSt=wRoot->getChildByName((zxmlNode*&)wDomainRootNode,pDomain);
    if (wSt!=ZS_SUCCESS) {
        pErrorLog->logZStatus(
            ZAIES_Error,
            wSt,
            "ZHelp::loadGenerateParameters-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
            pDomain.toCChar(),
            decode_ZStatus(wSt));
        return wSt;
    }
*/
    wSt=XMLgetChildText( wDomainRootNode,"helpmain",HelpMainHtml,pErrorLog,ZAIES_Error);

    KeywordList.clear();

    wSt=wDomainRootNode->getChildByName((zxmlNode*&)wKeywordListNode,"helplist");

    ZHelpKeyword wKeywordItem;

    wSt=wKeywordListNode->getFirstChild((zxmlNode*&)wKeyWordNode);
    int wKeyItems=0,wKeyErrored=0;

    while (wSt==ZS_SUCCESS) {
        if (wKeyWordNode->getName() == "helpitem")  {
            utf8VaryingString wKeyword;
            uriString wHtml;
            if (XMLgetChildText(wKeyWordNode,"keyword",wKeyword,pErrorLog,ZAIES_Error) == ZS_SUCCESS) {
                if (XMLgetChildText(wKeyWordNode,"file",wHtml,pErrorLog,ZAIES_Error) == ZS_SUCCESS) {
                    wKeywordItem.Keyword = wKeyword;
                    wKeywordItem.Html = wHtml;
                    if (XMLgetChildText(wKeyWordNode,"brief",wKeywordItem.Brief,pErrorLog,ZAIES_Error) != ZS_SUCCESS) {
                        wKeywordItem.Brief="No brief description";
                    }
                    KeywordList.push(wKeywordItem);
                    wKeywordItem.resetToNull();
                    wKeyItems++;
                }
                else
                    wKeyErrored++;
            }
            else
                wKeyErrored++;

        }//helpitem

        wSt=wKeyWordNode->getNextNode((zxmlNode*&)wSwapNode);
        XMLderegister(wKeyWordNode);
        wKeyWordNode=wSwapNode;
    } // while (wSt==ZS_SUCCESS)

    pErrorLog->infoLog(
        "ZHelp::XMLLoad-I-Report file <%s>\nHelp domain <%s>\n<%d> keywords and associated files\n<%d> errored keywords",
        pXmlFile.toCChar(),
        pDomain.toCChar(),
        wKeyItems,
        wKeyErrored);


    XMLderegister(wRoot);
    XMLderegister((zxmlNode *&) wDomainRootNode);
    XMLderegister((zxmlNode *&) wKeywordListNode);
    if (wSt==ZS_EOF) {
        return ZS_SUCCESS;
    }
    return wSt;
}//  XmlLoad



void ZHelp::setCloseButtonRole () {

  if (Options & TEOP_CloseBtnHide) {
    closeBTn->setText(QObject::tr("Hide","ZHelp"));
    }
    else {
      closeBTn->setText(QObject::tr("Close","ZHelp"));
    }
}

void ZHelp::closeEvent(QCloseEvent *event)
{
  if (CloseCallBack!=nullptr) {
    CloseCallBack(event);
  }
  QMainWindow::closeEvent(event);
  return;
}


void ZHelp::keyPressEvent(QKeyEvent *pEvent) {

  if (pEvent->key()==Qt::Key_F3) {
    searchPressed();
  }

  if (pEvent->modifiers() & Qt::ControlModifier) {
    if (pEvent->key()==Qt::Key_F) {
    searchMainPressed();
    }
  }

  QWidget::keyPressEvent(pEvent);
}


void ZHelp::resizeEvent(QResizeEvent* pEvent)
{
  QSize wRDlg = pEvent->oldSize();
  QWidget::resize(pEvent->size().width(),pEvent->size().height());

  if (FResizeInitial)
  {
    FResizeInitial=false;
    return;
  }
  QRect wR1 = verticalLayoutWidget->geometry();

  int wWMargin = (wRDlg.width()-wR1.width());
  int wVW=pEvent->size().width() - wWMargin;
  int wHMargin = wRDlg.height() - wR1.height();
  int wVH=pEvent->size().height() - wHMargin ;

  verticalLayoutWidget->resize(wVW,wVH);  /* expands in width and height */

}//ZHelp::resizeEvent

void ZHelp::morePressed()
{
  if (MoreCallBack!=nullptr)
    MoreCallBack();
}
/*
bool ZHelp::hasLineNumbers() {
  return TextTBr->getUseLineNumbers();
}

void ZHelp::lineNumbersBTnClicked()
{
  lineNumbersOnOff();
  if (hasLineNumbers()){
    if (lineNbQAc->isChecked())
        return;
    lineNbQAc->setChecked(true);
    return;
  }
  if (!lineNbQAc->isChecked())
    return;
  lineNbQAc->setChecked(false);
}

void ZHelp::lineNumbersOnOff()
{
  TextTBr->setUseLineNumbers(!TextTBr->getUseLineNumbers());
  if (TextTBr->getUseLineNumbers())  {
    lineNumbersBTn->setText(tr("Hide line numbers"));
    Options &= ~ TEOP_ShowLineNumbers;
    return;
  }

  lineNumbersBTn->setText(tr("Show line numbers"));
  Options |=  TEOP_ShowLineNumbers;

}
*/
void ZHelp::wrapPressed()
{
  wordWrap();
  if (TextTBr->wordWrapMode()==QTextOption::WordWrap){
    if (wrapQAc->isChecked())
      return;
    wrapQAc->setChecked(true);
    return;
  }
  if (!wrapQAc->isChecked())
    return;
  wrapQAc->setChecked(false);
}

void ZHelp::wordWrap()
{
  if (TextTBr->wordWrapMode()==QTextOption::NoWrap) {
    TextTBr->setWordWrapMode(QTextOption::WordWrap);
    wrapBTn->setText(tr("No wrap"));
  }
  else {
    TextTBr->setWordWrapMode(QTextOption::NoWrap);
    wrapBTn->setText(tr("Word wrap"));
  }
}

void ZHelp::searchMainPressed()
{
  if (FSearch) {
    FSearch=false;
    searchBTn->setVisible(false);
    searchLEd->setVisible(false);
    searchLEd->clear();
    TextTBr->resetSelection();
    return;
  }
  FSearch=true;
  searchBTn->setVisible(true);
  searchLEd->setVisible(true);
  return;
}
void ZHelp::searchPressed()
{
  search(searchLEd->text().toUtf8().data());
}

void ZHelp::searchReturnPressed()
{
  searchFirst(searchLEd->text().toUtf8().data());
}

void ZHelp::closePressed()
{
  if (Options & TEOP_CloseBtnHide) {
    this->hide();
    if (CloseCallBack) {
      QEvent wEv(QEvent::Hide);

      CloseCallBack(&wEv);
    }
    return;
  }
  this->close();
  this->deleteLater();
  return;
}


ZStatus
ZHelp::setTextFromFile(const uriString& pTextFile)
{
  utf8VaryingString wText;
  ZStatus wSt=pTextFile.loadUtf8(wText);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  TextTBr->setPlainText(wText.toQString());


  setWindowTitle(pTextFile.getBasename().toCChar());

  searchMainBTn->setVisible(true);

  return ZS_SUCCESS;
}

void
ZHelp::setText(const utf8VaryingString& pText,const utf8VaryingString& pTitle)
{
  TextTBr->setPlainText(pText.toCChar());
  QMainWindow::setWindowTitle(pTitle.toCChar());
  searchMainBTn->setVisible(true);
}

void
ZHelp::appendText(const utf8VaryingString& pText) {
  TextTBr->append(pText.toCChar());
}
void
ZHelp::appendTextColor(QColor pBkgndColor,QColor pTextColor,const utf8VaryingString& pText) {
  TextTBr->appendTextColor(pBkgndColor,pTextColor,pText.toCChar());
}
void
ZHelp::appendText(const char* pText,...) {
  utf8VaryingString wT;
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

  TextTBr->append(wT.toCChar());
}

void
ZHelp::appendHtml(const char* pText,...) {
  utf8VaryingString wT = utf8VaryingString::escapeHtmlSeq(pText);
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

  TextTBr->appendHtml(wT);
}

void
ZHelp::appendTextColor(QColor pBkgndColor,QColor pTextColor,const char* pText,...) {
  utf8VaryingString wT;
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

  appendTextColor(pBkgndColor,pTextColor,wT);
}

void
ZHelp::appendTextColor(QColor pTextColor,const char* pText,...) {
  utf8VaryingString wT;
  va_list args;
  va_start (args, pText);
  wT.vsnprintf(cst_messagelen,pText, args);
  va_end(args);

   TextTBr->appendTextColor(QColor(),pTextColor,wT.toCChar());
}
void
ZHelp::clear() {
  TextTBr->clear();
}

void
ZHelp::highlightWordAtOffset(int pOffset)
{
  TextTBr->highlightWordAtOffset(pOffset);
}
void
ZHelp::highlightCharactersAtOffset(int pOffset,int pLength)
{
    TextTBr->highlightLengthAtOffset(pOffset,pLength);
}
void
ZHelp::highlightLine(int pLine)
{
  TextTBr->highlightLine(pLine);
}
void
ZHelp::resetLine(int pLine) {
  TextTBr->resetLine(pLine);
}
void
ZHelp::resetWordAtOffset(int pOffset) {
  TextTBr->resetWordAtOffset(pOffset);
}

void
ZHelp::setPositionOrigin (){
  TextTBr->setPositionOrigin ();
}

void ZHelp::filterPressed() {
  filtrate();
  TextTBr->refresh();
}

void ZHelp::setFiltrateActive(bool pOnOff) {
  if (pOnOff && (FiltrateCallBack==nullptr)) {
    fprintf(stderr,"ZHelp::setFiltrateActive-F-NOCALLBCK FiltrateCallBack has not been registrated while trying to set filtrate on.\n");
    std::cout.flush();
    abort();
  }
  FiltrateActive=pOnOff;
  if (FiltrateActive) {
    filterBTn->setEnabled(true);
    filterBTn->setVisible(true);
  }
  else {
    filterBTn->setEnabled(false);
    filterBTn->setVisible(false);
  }
}

void
ZHelp::filtrate() {
  bool wEndText=false;
  if (FiltrateCallBack==nullptr)
    return;
  QTextDocument* wTDoc=TextTBr->document();
  QTextBlock wTBloc= wTDoc->firstBlock();
  while (true) {
    utf8VaryingString wBC=wTBloc.text().toUtf8().data();
    if (!wBC.isEmpty()) {
      if ( FiltrateCallBack(wBC))
        wTBloc.setVisible(true);
      else
        wTBloc.setVisible(false);
    }
    if (wEndText)
      break;
    wTBloc=wTBloc.next();
    wEndText=(wTBloc == wTDoc->lastBlock());
  }// while true

  TextTBr->update();
} // ZHelp::filtrate

void
ZHelp::searchFirst(const utf8VaryingString& pSearchString) {

  if (pSearchString.isEmpty()) {
    statusbar->showMessage(QObject::tr("search string is empty."),cst_MessageDuration);
    return;
  }
  if (searchDoc==nullptr) {
    searchDoc = TextTBr->document();
  }

  searchCursor= searchDoc->find (pSearchString.toCChar(),searchOffset);
  if (searchCursor.isNull()) {
      statusbar->showMessage(QObject::tr("search string not found."),cst_MessageDuration);
      return;
  }

  /* here string has been found and is pointed by wSearchCursor */
  TextTBr->highlightSelection(searchCursor);

} // ZHelp::searchFirst


void
ZHelp::search(const utf8VaryingString& pSearchString) {

  if (pSearchString.isEmpty()) {
    statusbar->showMessage(QObject::tr("search string is empty."),cst_MessageDuration);
    return;
  }
  if (searchDoc==nullptr) {
    searchDoc = TextTBr->document();
  }
  if (searchCursor.isNull()) {
    searchCursor= searchDoc->find (pSearchString.toCChar(),searchOffset);
    if (searchCursor.isNull()) {
      statusbar->showMessage(QObject::tr("search string not found."),cst_MessageDuration);
      return;
    }
  }
  else {
    searchCursor= searchDoc->find (pSearchString.toCChar(),searchCursor);
    if (searchCursor.isNull()) {
      statusbar->showMessage(QObject::tr("search string no more found."),cst_MessageDuration);
      return;
    }
  }

  /* here string has been found and is pointed by wSearchCursor */
  TextTBr->highlightSelection(searchCursor);

} // ZHelp::search

void
ZHelp::highlightAll(const utf8VaryingString &pSearchString,QColor pBackgnd,QColor pForegnd)
{
    if (pSearchString.isEmpty()) {
        statusbar->showMessage(QObject::tr("search string is empty."),cst_MessageDuration);
        return;
    }

    ZTextFormatTuple wHLT;
    int wSearchOffset=0;
    int wSearchLength = pSearchString.strlen();
    QTextCursor wCursor= TextTBr->document()->find (pSearchString.toCChar(),wSearchOffset);
    while (!wCursor.isNull()) {
        wHLT.Offset = wCursor.position();
        wHLT.Length = wSearchLength;
        wHLT.CharFmt = wCursor.charFormat();
        TextTBr->highlightSelection(wCursor,pBackgnd,pForegnd);
        TextFormatList.push(wHLT);
        wSearchOffset = wCursor.position() + wSearchLength ;
        wCursor = TextTBr->document()->find (pSearchString.toCChar(),wSearchOffset);
    }
}



void
ZHelp::resetHighLightAll()
{
    if (TextFormatList.count()==0)
        return;

    for (int wi=0; wi < TextFormatList.count() ; wi++) {
        TextTBr->setTextFormat(TextFormatList[wi]);
    }

}


void ZHelp::displayColorCallBack (uint8_t pSeverity,const utf8VaryingString& pMessage)
{
    switch (pSeverity) {
    case ZAIES_Text:
    {
        QPalette wP = palette();
        QColor wC=wP.color(QPalette::Text);
        appendTextColor(wC, pMessage);
 //       appendTextColor(Qt::black, pMessage);
//        appendText(pMessage);
        return;
    }
    case ZAIES_Info:
        appendTextColor(Qt::blue, pMessage);
        return;
    case ZAIES_Warning:
        appendTextColor(Qt::darkGreen, pMessage);
        return;
    case ZAIES_Error:
        appendTextColor(Qt::red, pMessage);
        return;
    case ZAIES_Fatal:
        appendTextColor(Qt::magenta, pMessage);
        return;
    default:
        appendTextColor(Qt::yellow, pMessage);
        return;
    }
}//ZHelp::displayColorCallBack
void ZHelp::displayCallBack (const utf8VaryingString& pMessage)
{
        appendText(pMessage);
}//ZHelp::displayCallBack

/*
  displays keyword pToSearch
  searches for KeywordList to find keyword.
  if found, display corresponding html file
  if search approximation and suggest it
 */
void ZHelp::displayKeyword(const utf8VaryingString& pToSearch)
{
    if (KeywordList.count()==0) {
        ZExceptionDLg::adhocMessage("Help keyword",Severity_Error,nullptr,nullptr,"No help available");
        return;
    }

    int wS=-1;
    for (int wi=0; wi < KeywordList.count();wi++) {
        if (pToSearch.compareCase(KeywordList[wi].getKeyword())==0) {
            wS=wi;
            break;
        }
    } // for

    if (wS<0) {
        /* create keyword list to search */
        /* suggest symbols */
        ZArray<utf8VaryingString> wSugArray ;
        /* add entities names to suggest */
        for (int wi=0;wi < KeywordList.count();wi++)
            wSugArray.push(KeywordList[wi].getKeyword());
        /* try to suggest */
        utf8VaryingString wSuggest=searchAdHocWeighted (pToSearch,wSugArray);
        int wRet=0;
        if (wSuggest.isEmpty()) {
            wRet=ZExceptionDLg::adhocMessage2B("Help keyword",Severity_Error,
                                                     "Quit","FreeText",
                                                     "Help keyword not found\n"
                                                     "You may search freetext.");
        }
        else {
            wRet=ZExceptionDLg::adhocMessage2B("Help keyword",Severity_Error,
                                                      "Quit","FreeText",
                                                      "Help keyword not found\n"
                                                      "Do you mean <%s> ?"
                                                      "You may search freetext.",wSuggest.toString());
        }
        if (wRet==QDialog::Rejected)
            return;
        return;
    } // if (wS<0)


    setHtmlSource(KeywordList[wS].getHtml()) ;

} // displayKeyword
/*
#ifndef __CFreeTextMatch__
#define __CFreeTextMatch__
class CFreeTextMatch {
public:
    CFreeTextMatch()=default;
    CFreeTextMatch(const CFreeTextMatch& pIn) { _copyFrom(pIn);}
    CFreeTextMatch& _copyFrom(const CFreeTextMatch& pIn) {
        File=pIn.File;
        Count=pIn.Count;
        return *this;
    }
    uriString File;
    int       Count=0;
};
#endif

void ZHelp::freeTextSearch(const utf8VaryingString& pToSearch)
{
    ZArray<CFreeTextMatch> wMatchList;
    utf8VaryingString wUtfContent;
    uriString gabu;
    ZStatus wSt=ZS_SUCCESS;
    for (int wi=0; wi < KeywordList.count();wi++) {
        CFreeTextMatch wMatch;
        wMatch.Count=0;
        wMatch.File=KeywordList[wi].Html;
        wSt = wMatch.File.loadUtf8(wUtfContent);
        if (wSt==ZS_SUCCESS) {
            ssize_t wOffset=0;
            wOffset=wUtfContent.bsearch(pToSearch.toCChar(),(size_t)wOffset);
            while (wOffset>0) {
                wMatch.Count++;
                wOffset=wUtfContent.bsearch(pToSearch.toCChar(),(size_t)wOffset);
            }
            if (wMatch.Count)
                wMatchList.push(wMatch);
        }
    } // for


} //freeTextSearch
*/
} // namespace zbs
