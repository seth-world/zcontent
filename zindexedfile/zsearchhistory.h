#ifndef ZSEARCHHISTORY_H
#define ZSEARCHHISTORY_H

#include <ztoolset/zstatus.h>
#include <ztoolset/uristring.h>
#include <ztoolset/utfvaryingstring.h>

namespace zbs {

typedef int InstructionFlag_type;



class ZSearchHistory
{
public:
  ZSearchHistory()=default;
  ~ZSearchHistory() ;

  enum InstructionFlag : InstructionFlag_type {
      Forget        = 0 ,
      DataExecute   = 1 ,
      Other         = 2
  };


  ZStatus setup(const uriString& pHistoryPath);
  ZStatus loadHistory();

  ZStatus add(const utf8VaryingString& pInst, InstructionFlag_type pFlag=DataExecute);
  ZStatus getPrevious(utf8VaryingString &pPrevious);
  ZStatus getFirst(utf8VaryingString &pNext);
  ZStatus getNext(utf8VaryingString &pNext);

  ZStatus setMaximum(int pMax,ZaiErrors* pErrorLog);

  ZStatus removeFirst(int pLines,ZaiErrors* pErrorLog);
  ZStatus removeLast(int pLines,ZaiErrors* pErrorLog);

  ZStatus remove(int pRank,ZaiErrors* pErrorLog);

  void    show(ZaiErrors* pErrorLog);

  uriString      Path;

  ZStatus _openHistoryFile();
  ZStatus closeHistoryFile();
  ZStatus saveHistory();
  ZDataBuffer save();

  ZArray<utf8VaryingString> List;
  __FILEHANDLE__ Fd=-1;

  int Index=0;
  int Maximum=40;
};

}//namespace zbs

#endif // ZSEARCHHISTORY_H
