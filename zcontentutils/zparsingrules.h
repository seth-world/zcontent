#ifndef ZPARSINGRULES_H
#define ZPARSINGRULES_H

#include <zstdlistdlg.h>
#include <zcppparser/zcppparser.h>
#include <zcontentutilsparams.h>
#include <ztypedlg.h>

namespace Ui {
class ZParsingRulesDLg;
}
using namespace zbs;

//class ZTypeDLg;
//class ZTypeListButton;

class ZParsingRulesList : public ZStdListDLg {
public:
  explicit ZParsingRulesList(ZCppParser* pParser,QWidget *parent = nullptr);
//  ~ZParsingRulesList() ;

  ZStdListDLgLine formatLine(const ZConversionElement& pElt);

  bool setCurrent(const utf8VaryingString& pSyntax ) {
    long wRank=0;
    if ((wRank=Parser->ConversionRules.searchForSyntax(pSyntax))<0)
      return false;
    set(int(wRank));
    return true;
  }

  virtual void AcceptCurrent(int pRow) override ;
  virtual void ChangeCurrent(int pRow) override ;
  virtual void DeleteCurrent(int pRow) override ;
  virtual void InsertCurrent(int pRow) override ;
  virtual void AppendCurrent() override ;
private:
  ZCppParser* Parser=nullptr;
};


#endif // ZPARSINGRULES_H
