#ifndef ZPARSINGRULESDLG_H
#define ZPARSINGRULESDLG_H

#include <QDialog>
#include <zcppparser/zcppparser.h>

//extern utf8VaryingString SyntaxCopy;

using namespace zparsernmsp;

namespace Ui {
class ZParsingRulesDLg;
}

class ZTypeDLg;
class ZQLineEdit;

class ZParsingRulesDLg : public QDialog
{
  Q_OBJECT

public:
  explicit ZParsingRulesDLg(ZCppParser* pParser,QWidget *parent = nullptr);
  ~ZParsingRulesDLg();


  ZQLineEdit*             SyntaxLEd=nullptr;
  ZTypeDLg*              ZTypeTRv=nullptr;
  //ZTypeListButton*       TypeListBtn=nullptr;
  QPushButton*            TypeListBtn=nullptr;

  void setup(const ZConversionElement &pConvElt);
  ZConversionElement& getConversionElt();

  void setCreateFromSyntax(const utf8VaryingString& pSyntax);

  bool keyPress(QKeyEvent*pEvent);
  bool contextMenuCallback(QContextMenuEvent*pEvent);

  bool getSyntaxFromClipboard();

  ZConversionElement     ConversionElt;

  void refresh();

  void contextMenuEvent(QContextMenuEvent *pEvent) override;

public slots:
  //  void ZTypeFocusIn(QFocusEvent* pEvent);
  void ZTypeListClicked();
  void AcceptClicked();
  void DiscardClicked();
private:
  ZCppParser* Parser=nullptr;
  Ui::ZParsingRulesDLg *ui;
};


#endif // ZPARSINGRULESDLG_H
