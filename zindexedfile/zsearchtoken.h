#ifndef ZSEARCHTOKEN_H
#define ZSEARCHTOKEN_H


#include <ztoolset/zarray.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zaierrors.h>

#include "zsearchparsertype.h"

#define __DISPLAYCALLBACK__(__NAME__)  std::function<void (const utf8VaryingString&)> __NAME__
#define __PROGRESSCALLBACK__(__NAME__)  std::function<void (int)> __NAME__


namespace zbs {


extern int Line,Column,Offset;

class ZSearchTokenizer;


class ZSearchToken
{
public:
public:
  ZSearchToken() = default;
  ~ZSearchToken() {}
  ZSearchToken(ZSearchTokenizer*pTokenizer ) {Tokenizer=pTokenizer;}
  ZSearchToken(const ZSearchToken& pIn) {_copyFrom(pIn);}
  ZSearchToken(const ZSearchToken* pIn) {_copyFrom(pIn);}

  ZSearchToken& operator = (const ZSearchToken& pIn) {return _copyFrom(pIn);}

  ZSearchToken& _copyFrom (const ZSearchToken& pIn) ;
  ZSearchToken& _copyFrom (const ZSearchToken* pIn) ;

  bool isValid() const  {return Type!=0xFFFFFFFF;}
  bool isInvalid()const  {return Type==0xFFFFFFFF;}
  void setInvalid () {Type = 0xFFFFFFFF;}

  void clear() {reset();}

  ZSearchTokentype_type   Type{ZSRCH_SPACE};
  utf8VaryingString Text;
  int TokenColumn=0;
  int TokenLine=0;
  int TokenOffset=0;

  const char* TypetoCChar() const;
  const char* toCChar() const;

  void reset() {
    Type=ZSRCH_SPACE;
    Text.clear();
    TokenLine=Line;     /* NB: Keep line number and columns number */
    TokenColumn=Column;
  }

  void newLine() ;

  void setCoords(int pLine,int pColumn,int pOffset) {TokenColumn=pColumn; TokenLine=pLine; TokenOffset=pOffset;}

  void setCoords(const ZSearchToken & pIn) {TokenColumn=pIn.TokenColumn; TokenLine=pIn.TokenLine; TokenOffset=pIn.TokenOffset;}

  void display(int pIdx) const;
  void displayNoindex() const;

  ZSearchTokenizer* Tokenizer =nullptr;

};
} // namespace zbs



#endif // ZSEARCHTOKEN_H
