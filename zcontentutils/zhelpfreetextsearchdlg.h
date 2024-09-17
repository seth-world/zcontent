#ifndef ZHELPFREETEXTSEARCHDLG_H
#define ZHELPFREETEXTSEARCHDLG_H

#include <QObject>
#include <QDialog>
#include <qcheckbox.h>


#include <ztoolset/uristring.h>


class ZQTableView;
class QLineEdit;
class QLabel;
class QHBoxLayout;
class QFrame;


namespace zbs {

class ZHelp;
#ifndef __CFreeTextMatch__
#define __CFreeTextMatch__
class CFreeTextMatch {
public:
    CFreeTextMatch()=default;
    CFreeTextMatch(const CFreeTextMatch& pIn) { _copyFrom(pIn);}
    CFreeTextMatch& _copyFrom(const CFreeTextMatch& pIn) {
        File=pIn.File;
        Count=pIn.Count;
        Index=pIn.Index;
        return *this;
    }
    uriString File;
    int       Index=0;
    int       Count=0;
};
#endif

class ZHelpFreetextSearchDLg : public QDialog
{
  Q_OBJECT
public:


  explicit ZHelpFreetextSearchDLg(ZHelp *pZHelp) ;
  ~ZHelpFreetextSearchDLg() override ;


  enum ZHFTSD
  {
      ZHFTSD_Freetext = 0,
      ZHFTSD_Keyword = 1
  };

  void initLayout();

  void showKeywordsSetup(const utf8VaryingString& pDomain);

  void freeTextSetup(const utf8VaryingString& pDomain) ;


  ZStatus displayFoundText();
  int _freeTextSearch(const utf8VaryingString& pToSearch,bool pCaseSentitve, ZArray<CFreeTextMatch> &pMatchList);
  int _HtmlFreeTextSearch(const utf8VaryingString& pToSearch, bool pCaseSentitve,ZArray<CFreeTextMatch> &pMatchList);
  void freeTextDisplay();

  void perKeywordDisplay(const utf8VaryingString& pDomain);

  uriString makeFullFilePath(const uriString& pHelpFile);

  ZQTableView* keywordTBv=nullptr;



private slots:
  void AcceptClicked();
  void RejectClicked();
  void KeywordSearchClicked();

private:
  void resizeEvent(QResizeEvent* pEvent) override;


  bool _FResizeInitial=true;

  ZHFTSD ViewType = ZHFTSD_Keyword;

  QHBoxLayout* SBL=nullptr;

  QLabel*      DomainLBl=nullptr;

  QFrame*      SearchFRm=nullptr;
  QLineEdit*   ToSearch=nullptr;
  QCheckBox*    CaseSensitiveCHk=nullptr;

  QPushButton* KeywordSearchBTn=nullptr;

  QPushButton* AcceptBTn=nullptr;
  QPushButton* RejectBTn=nullptr;



  ZHelp*    ZHelpPtr=nullptr;

};

} // namespace zbs

#endif // ZHELPFREETEXTSEARCHDLG_H
