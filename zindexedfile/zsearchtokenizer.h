#ifndef ZSEARCHTOKENIZER_H
#define ZSEARCHTOKENIZER_H

#include <ztoolset/zarray.h>

#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zaierrors.h>



#include "zsearchparsertype.h"

#include "zsearchtoken.h"

namespace zbs {


extern int Line,Column,Offset;


class ZDictionaryFile ;

class ZSearchTokenizer : public ZArray<ZSearchToken*> {
public:
  typedef ZArray<ZSearchToken*> _Base;
  ZSearchTokenizer(uint32_t pOptions=ZSRCHO_Default) : Options(pOptions) {}
  ~ZSearchTokenizer() {
    while (_Base::count())
      delete popR();
  }

  void clear()
  {
    while (count()) {
      delete last();
      pop();
    }

  }

  void setDictionary(ZDictionaryFile* pDictionary) {DictionaryFile = pDictionary;}

  bool hasDisplayCallback(){return _displayCallback!=nullptr;}
  void parse(const utf8VaryingString &pIn);

  void parseComment(const utf8VaryingString &pIn, ZSearchToken *pCurrentToken, long &pTokenIndex) ;

  //  void displayComment();

  void advance(long &pTokenIndex,int pInc=0);
  //  ZArray<ZSearchToken*> CommentTokenList;


  void report() {

    for(long wi=0; wi < count(); wi++) {
      Tab(wi)->displayNoindex();
    }
  }//report

  void setDisplayCallback(__DISPLAYCALLBACK__(pdisplayCallback) ) {
    _displayCallback=pdisplayCallback;
    ErrorLog.setDisplayCallback(pdisplayCallback);
  }

  __DISPLAYCALLBACK__(_displayCallback) = nullptr;

  void setProgressCallback(__PROGRESSCALLBACK__(pdisplayCallback) ) {
    _progressCallback=pdisplayCallback;
  }

  __PROGRESSCALLBACK__ (_progressCallback) = nullptr;

  void _print(const char* pFormat,...) const;
  void _print(const utf8VaryingString& pOut) const;

  void setOutput(FILE* pOutput=stdout) {Output=pOutput; ErrorLog.setOutput(pOutput);}

  bool skipNewLine() {return Options & ZSRCHO_SkipNL ;}

  void setSkipNewLine(bool pOnOff) {if (pOnOff) Options |= ZSRCHO_SkipNL; else Options &= ~ZSRCHO_SkipNL;}

  ZaiErrors ErrorLog;
  uint32_t Options=0;

private:
  FILE* Output=stdout;
  /**
   * @brief endToken ends given pToken and starts a new blank one
   * @param pToken
   */
  void endToken(ZSearchToken *&pToken);
  void endCommentToken(ZSearchToken* &pCommentToken);

  ZDictionaryFile* DictionaryFile=nullptr;


};



}//zbs
#endif // ZSEARCHTOKENIZER_H
