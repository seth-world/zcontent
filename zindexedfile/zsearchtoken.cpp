#include "zsearchtoken.h"

#include "zsearchtokenizer.h"

namespace zbs {

using namespace std;
using namespace zbs;

ZSearchToken& ZSearchToken::_copyFrom (const ZSearchToken& pIn) {
  Type=pIn.Type;
  Text=pIn.Text;
  TokenColumn=pIn.TokenColumn;
  TokenLine=pIn.TokenLine;
  TokenOffset=pIn.TokenOffset;
  Tokenizer=pIn.Tokenizer;
  return *this;
}
ZSearchToken& ZSearchToken::_copyFrom (const ZSearchToken* pIn) {
  Type=pIn->Type;
  Text=pIn->Text;
  TokenColumn=pIn->TokenColumn;
  TokenLine=pIn->TokenLine;
  TokenOffset=pIn->TokenOffset;
  Tokenizer=pIn->Tokenizer;
  return *this;
}

void ZSearchToken::newLine() {

  Column=0;
  Line++;
  /* Offset counts already newline */
}


char wTp[200];
const char* ZSearchToken::TypetoCChar() const {
  if (isInvalid())
    return "<invalid type>";
  strncpy(wTp,decode_SearchTokenType(Type).toCChar(),200);
  return wTp;
}
const char* ZSearchToken::toCChar() const {
  if (isInvalid())
    return "<null>";
  return Text.toCChar();
}
void ZSearchToken::display(int pIdx) const {
  Tokenizer->_print("%3d>> line %4d col.%3d %30s <%s>",pIdx,TokenLine,TokenColumn,decode_SearchTokenType(Type).toCChar(),Text.toCChar());
}
void ZSearchToken::displayNoindex() const {
  Tokenizer->_print("line %4d col.%3d %30s <%s>",TokenLine,TokenColumn,decode_SearchTokenType(Type).toCChar(),Text.toCChar());
}

} // namespace zbs
