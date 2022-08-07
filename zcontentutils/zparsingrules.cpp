#include "zparsingrules.h"
#include "ui_zparsingrulesdlg.h"
#include <zcppparser/zcppparsertype.h>
#include <zexceptiondlg.h>
#include <zcontent/zindexedfile/zdatatype.h>

#include <zcontent/zcontentutils/zparsingrulesdlg.h>

using namespace zparsernmsp;

//ZParsingRulesList::~ZParsingRulesList() {}

/*
        <typeconversion>
            <tokentype>ZTTP_UNKNOWN_TYPE</tokentype>
            <syntax>DocumentMetaType</syntax>
            <ztype>ZType_AtomicS64</ztype>
            <capacity>1</capacity>
            <universalsize>0</universalsize>
            <naturalsize>0</naturalsize>
        </typeconversion>
*/
ZParsingRulesList::ZParsingRulesList(ZCppParser* pParser,QWidget *parent ):ZStdListDLg(parent){

  Parser=pParser;

  setWindowTitle("Parsing rules list");
  ZStdListHeaders     wHeaders;
  ZStdListDLgContent  wContent;

//  wHeaders.add ("Token type");
  wHeaders.add ("Syntax");
  wHeaders.add ("Target ZType");
  wHeaders.add ("Target Capacity");
  wHeaders.add ("Universal Size");
  wHeaders.add ("Natural Size");

  for (long wi=0;wi < pParser->ConversionRules.count();wi++) {
    ZStdListDLgLine wContentLine;
    utf8VaryingString wStr;

//    wContentLine.add ( decode_TokenType(pParser->ConversionRules[wi].TokenType).toCChar()) ;
    wContentLine.add ( pParser->ConversionRules[wi].Syntax.toCChar()) ;
    wContentLine.add ( decode_ZType(pParser->ConversionRules[wi].ZType)) ;
    wStr.sprintf("%d",pParser->ConversionRules[wi].Capacity);
    wContentLine.add ( wStr.toCChar()) ;
    wStr.sprintf("%ld",pParser->ConversionRules[wi].UniversalSize);
    wContentLine.add ( wStr.toCChar()) ;
    wStr.sprintf("%ld",pParser->ConversionRules[wi].NaturalSize);
    wContentLine.add ( wStr.toCChar()) ;
    wContent.push(wContentLine);
  }

  dataSetup(wHeaders,wContent);
}//ZParsingRulesList CTOR
ZStdListDLgLine
ZParsingRulesList::formatLine(const ZConversionElement& pElt){
  utf8VaryingString wStr;
  ZStdListDLgLine wContentLine;
  wContentLine.add ( pElt.Syntax.toCChar()) ;
  wContentLine.add ( decode_ZType(pElt.ZType)) ;
  wStr.sprintf("%d",pElt.Capacity);
  wContentLine.add ( wStr.toCChar()) ;
  wStr.sprintf("%ld",pElt.UniversalSize);
  wContentLine.add ( wStr.toCChar()) ;
  wStr.sprintf("%ld",pElt.NaturalSize);
  wContentLine.add ( wStr.toCChar());
  return wContentLine;
}//formatLine


void ZParsingRulesList::AcceptCurrent(int pRow) {

  ChangeCurrent(pRow);
  return;
}
void ZParsingRulesList::ChangeCurrent(int pRow) {

  ZParsingRulesDLg* wRuleDlg=new ZParsingRulesDLg(Parser,this);
  wRuleDlg->setup(Parser->ConversionRules[pRow]);
  int wRet=wRuleDlg->exec();
  if (wRet==QDialog::Accepted) {
    Parser->ConversionRules[pRow] = wRuleDlg->getConversionElt();
    ZStdListDLg::ReplaceLine(pRow,formatLine(Parser->ConversionRules[pRow]));
  }
  delete wRuleDlg;
  return;
}
void ZParsingRulesList::AppendCurrent() {

  ZParsingRulesDLg* wRuleDlg=new ZParsingRulesDLg(Parser,this);
//  wRuleDlg->setup(Parser->ConversionRules[pRow]);
  int wRet=wRuleDlg->exec();
  if (wRet==QDialog::Accepted) {
    Parser->ConversionRules.push(wRuleDlg->getConversionElt());
    ZStdListDLg::AppendLine(formatLine(wRuleDlg->getConversionElt()));
  }
  delete wRuleDlg;
  return;
}
void ZParsingRulesList::DeleteCurrent(int pRow) {

  Parser->ConversionRules.erase(pRow);
  ZStdListDLg::DeleteLine(pRow);
  return;
}
void ZParsingRulesList::InsertCurrent(int pRow) {

  ZParsingRulesDLg* wRuleDlg=new ZParsingRulesDLg(Parser,this);
  //  wRuleDlg->setup(Parser->ConversionRules[pRow]);
  int wRet=wRuleDlg->exec();
  if (wRet==QDialog::Accepted) {
    Parser->ConversionRules.insert(wRuleDlg->getConversionElt(),pRow);
    ZStdListDLg::InsertLine(pRow,formatLine(wRuleDlg->getConversionElt()));
  }
  delete wRuleDlg;
  return;
}
