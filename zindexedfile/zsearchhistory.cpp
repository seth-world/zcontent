#include "zsearchhistory.h"

#include <ztoolset/zexceptionmin.h>
#include <zio/zioutils.h>
#include <ztoolset/zaierrors.h>

namespace zbs {


ZSearchHistory::~ZSearchHistory()
{
  closeHistoryFile();
}


ZStatus
ZSearchHistory::setup(const uriString& pHistoryPath)
{
  ZStatus wSt=ZS_SUCCESS;
  Path=pHistoryPath;
  if (pHistoryPath.exists() ) {
    wSt = loadHistory();
    if (wSt!=ZS_SUCCESS) {
      ZException.setMessage("ZSearchHistory::setup",wSt,Severity_Error,"Invalid history file status <%s>\n"
                                                                          " File <%s> will be replaced,",decode_ZStatus(wSt),Path.toString());
      return wSt;
    }
  }

  return ZS_SUCCESS;
}

ZStatus
ZSearchHistory::loadHistory()
{
  ZDataBuffer wContent;
  utf8String wInst;
  ZStatus wSt=Path.loadContent(wContent);
  if (wSt!=ZS_SUCCESS)
    return wSt;

  const unsigned char* wPtr = wContent.Data;
  const unsigned char* wPtrEnd = wContent.Data + wContent.Size;
  uint32_t wMax;
  _importAtomic<uint32_t>(wMax,wPtr);
  if ((wMax > 500) || (wMax < 4)) {
    return ZS_CORRUPTED;
  }
  Maximum = int(wMax);
  wPtr+= sizeof(cst_ZBLOCKEND);

  while (wPtr < wPtrEnd){
    wInst._importUVF(wPtr);
    wPtr+= sizeof(cst_ZBLOCKEND);
    List.push(wInst);
    wInst.clear();
  }
  Index=List.count();
  return wSt;
}


ZStatus
ZSearchHistory::add(const utf8VaryingString& pInst)
{
  /* remove trailing spaces \t \n characters */

  utf8_t* wInst = pInst.duplicate();
  const utf8_t wSet[] = {'\t','\n','\r','\0'};
  unsigned char* wPtr = wInst + pInst.UnitCount -1;
  bool wFound=false;
  while (true) {
    while (wPtr > wInst) {
      for (int wi=0; wi < int(sizeof(wSet));wi++) {
        if (wSet[wi]==*wPtr) {
          wFound=true;
          break;
        }
      }// for
      if (!wFound)
        break;
      wFound=false;
      wPtr--;
    } // while


    wPtr++;
    *wPtr = '\0';
    break;
  }// while true


  List.push(wInst);
  zfree(wInst);

  while (List.count()>Maximum) {
    List.pop_front();
  }

  ZStatus wSt=_openHistoryFile();
  if (wSt!=ZS_SUCCESS)
    return wSt;

  ZDataBuffer wZDB;
  size_t      wSize=0;
  pInst._exportAppendUVF(wZDB);
  wZDB.appendData(&cst_ZBLOCKEND,sizeof(cst_ZBLOCKEND));

  wSt=rawSeekEnd(Fd);
  if (wSt!=ZS_SUCCESS) {
    goto add_Errored;
  }
  wSt=rawWrite(Fd,wZDB,wSize);
  Index=List.count();
add_Errored:
  rawClose(Fd);
  Fd=-1;
  return wSt;
}


ZStatus
ZSearchHistory::getPrevious(utf8VaryingString& pPrevious)
{
  if (Index==0)
    return ZS_OUTBOUNDLOW;
  if (List.count()==0)
    return ZS_EMPTY;

  if (Index >= List.count())
    Index=List.count();

  Index--;

  pPrevious = List[Index];
  return ZS_SUCCESS;
}


ZStatus
ZSearchHistory::getNext(utf8VaryingString& pNext)
{
  if (Index > List.count()-2)
    return ZS_OUTBOUNDHIGH;
  if (List.count()==0)
    return ZS_EMPTY;

  Index++;

  pNext = List[Index];

  return ZS_SUCCESS;
}

ZStatus
ZSearchHistory::saveHistory()
{
  ZDataBuffer wZDB = save();

  return Path.writeContent(wZDB);
}


ZDataBuffer
ZSearchHistory::save()
{
  ZDataBuffer wZDB;
  _exportAtomic<uint32_t>(uint32_t(Maximum),wZDB);
  wZDB.appendData(&cst_ZBLOCKEND,sizeof(cst_ZBLOCKEND));


  for (int wi=0; wi < List.count();wi++) {
    List[wi]._exportAppendUVF(wZDB);
    wZDB.appendData(&cst_ZBLOCKEND,sizeof(cst_ZBLOCKEND));
  }
  return wZDB;
}

ZStatus
ZSearchHistory::setMaximum(int pMax,ZaiErrors* pErrorLog)
{
  if (pMax < 0) {
    return ZS_INVVALUE;
  }
  if (List.count()>pMax) {
    while (List.count()>pMax) {
      List.pop_front();
    }
  }
  ZStatus wSt=saveHistory();
  Maximum = pMax;
  if (wSt==ZS_SUCCESS)
    pErrorLog->textLog(" History maximum is set to %d ",Maximum);
  else
    pErrorLog->textLog(" Error: Cannot set History maximum to value %d. Status is <%s>",pMax, decode_ZStatus(wSt));
  return wSt;
}


ZStatus
ZSearchHistory::_openHistoryFile()
{
  ZStatus wSt=rawOpen(Fd,Path, O_RDWR  );
  if (wSt==ZS_SUCCESS){
    return rawSeekEnd(Fd);
  }

  return rawOpen(Fd,Path, O_RDWR | O_CREAT , S_IRWXU | S_IRWXG );
}


ZStatus
ZSearchHistory::closeHistoryFile()
{
  ZDataBuffer wZDB = save();
  return Path.writeContent(wZDB);
}


ZStatus
ZSearchHistory::removeFirst(int pLines,ZaiErrors* pErrorLog)
{
  if (List.count()<=pLines){
    List.clear();
    pErrorLog->infoLog("            ---instruction log is cleared---");
    return saveHistory();
  }
  for (int wi=0;wi < pLines;wi++)
    List.erase(0);

  pErrorLog->infoLog("            --- removed %d lines [0,%d] from the beginning of instruction log ---",pLines,pLines);
  return saveHistory();
}
ZStatus
ZSearchHistory::removeLast(int pLines,ZaiErrors* pErrorLog)
{
  if (List.count()<=pLines){
    List.clear();
    pErrorLog->infoLog("            ---instruction log is cleared---");
    return saveHistory();
  }
  for (int wi=0;wi < pLines;wi++)
    List.pop();

  pErrorLog->infoLog("            --- removed %d lines from then end of instruction log ---",pLines,pLines);
  return saveHistory();
}
ZStatus
ZSearchHistory::remove(int pRank,ZaiErrors* pErrorLog)
{
  int wRank=pRank-1;
  if (wRank < 0) {
    pErrorLog->errorLog("            ---cannot remove instruction at rank %d from instruction log. Count is %d---",pRank,List.count());
    return saveHistory();
  }
  if (List.count()<=wRank){
    pErrorLog->errorLog("            ---cannot remove instruction at rank %d from instruction log. Count is %d---",pRank,List.count());
    return saveHistory();
  }
  List.erase(wRank);

  pErrorLog->infoLog("            --- removed instruction at rank %d from instruction log ---",pRank);

  return saveHistory();
}

void
ZSearchHistory::show (ZaiErrors* pErrorLog)
{
  pErrorLog->textLog("______________________________________Instructions history______________________________________");

  if (List.count()==0) {
    pErrorLog->textLog("            ---no instruction to show--.");
  }
  else {
    for (int wi=0;wi < List.count(); wi++) {
      pErrorLog->textLog("%3d- %s",wi+1,List[wi].toString());
    }
  }

  pErrorLog->textLog("_____________________________________________________________________________________________");
}

}//namespace zbs
