#ifndef ZINDEXTABLE_H
#define ZINDEXTABLE_H

#include <zindexedfile/zmfconfig.h>

#include <ztoolset/zarray.h>
#include <ztoolset/zstatus.h>
#include <ztoolset/zutfstrings.h>

namespace zbs {


class ZRawIndexFile;

class ZIndexTable :  private ZArray<ZRawIndexFile*>
{
  typedef ZArray<ZRawIndexFile*> _Base ;
public:
  ZIndexTable() ;
  ~ZIndexTable() {}// just to call the base destructor
//  using _Base::push;
  using _Base::size;
  using _Base::count;
  using _Base::last;
  using _Base::lastIdx;
//  using _Base::newBlankElement;
// using _Base::operator [];

  ZRawIndexFile* operator [] (int wI) ;

  long pop (void);
  long erase (long pRank);
  long insert(ZRawIndexFile* pIn, long pRank);
  long push(ZRawIndexFile*pIn) ;
  void clear(void);


  long searchIndexByName (const char* pName);
  long searchCaseIndexByName (const char* pName);
  long searchIndexByName (const utf8String& pName);
  long searchCaseIndexByName (const utf8String& pName);



  utf8String toXml(int pLevel,bool pComment=true);
  ZStatus fromXml(zxmlNode* pRoot,ZaiErrors*pErrorlog);
} ;

}// namespace zbs
#endif // ZINDEXTABLE_H
