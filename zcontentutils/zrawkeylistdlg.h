#ifndef ZRAWKEYLISTDLG_H
#define ZRAWKEYLISTDLG_H

#include <QDialog>
#include <ztoolset/uristring.h>
#include <zcontent/zindexedfile/zmasterfile.h>

class   ZQTableView;


class ZRawKeyListDLg : public QDialog
{
public:
  explicit ZRawKeyListDLg( QWidget *parent = nullptr )  ;
  ~ZRawKeyListDLg() {
    if (locallyCreated && (MasterFile!=nullptr)){
      MasterFile->zclose();
      delete MasterFile;
    }
  }

private slots:
  void KeyListQuitBTnClicked(bool pChecked);
  void KeyListwSelectBTnClicked(bool pChecked);
public:
  long getCurrentIndexRank() {return CurrentIndexRank;}
  /* opens ZMasterFile pointed by pURIMasterFile. */
  ZStatus set(const uriString& pURIMasterFile);
  /* uses pMasterFile. it must be opend prior calling */
  ZStatus set(ZMasterFile* pMasterFile);

private:
  bool locallyCreated=false;
  long CurrentIndexRank=-1;
  zbs::ZMasterFile* MasterFile=nullptr;
  ZQTableView* KeyListTBv=nullptr;
};

#endif // ZRAWKEYLISTDLG_H
