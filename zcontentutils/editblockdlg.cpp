#include "editblockdlg.h"



#include <zio/zioutils.h>
#include <zcontent/zrandomfile/zrfutilities.h>
#include <zexceptiondlg.h>

#include <QVBoxLayout>
#include <QGridLayout>

#include <QLabel>
#include <QPushButton>

editBlockDLg::editBlockDLg(QWidget *pParent) : QDialog(pParent)
{

}

void editBlockDLg::setup(__FILEHANDLE__ pContentFd,__FILEHANDLE__ pHeaderFd, int pPoolId,long pDataRank,ZBlockDescriptor* pBD)
{
  ContentFd=pContentFd;
  HeaderFd =  pHeaderFd;
  BD = pBD;


  utf8VaryingString wStr;
  off_t wFileSize=0;

  ZStatus wSt = rawSeekEnd(pContentFd,wFileSize);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::adhocMessage("Seeking content file",Severity_Error,
        "Cannot access content file <%s>.",getNameFromFd(pContentFd).toString());
    return;
  }

  FileSize = size_t(wFileSize);

  /* search for effective file's information */


  /* first get block descriptor from file
    if ok get
      block size
      state
      lock
      pid
  */

  FileBlockDescStatus=rawGetBlockDescriptor(ContentFd,FBD,pBD->Address);
  if (FileBlockDescStatus!=ZS_SUCCESS) {
    FileIsAccessible=false;
  }
  else
    FileIsAccessible=true;

  /* second get next block address and compute theorical block size :
    address found : give address - compute theorical block size
    EOF           : address is file size - compute theorical block size
    other errors  : no address - invalid theorical block size
  */


  ZBlockDescriptor  wZDB;
  zaddress_type     wNextAddress;
  //  const unsigned char* wSeq=(const unsigned char*)&cst_ZFILEBLOCKSTART;
  //  ZStatus wSt1 = rawSearchNextSequence(FdContent,ContentFileSize,(size_t)2000, wSeq,sizeof(uint32_t),wBD->Address+sizeof(cst_ZFILEBLOCKSTART),wNextAddress);
  NextStartSignStatus = rawSearchNextStartSign(ContentFd,FileSize,2000,pBD->Address+sizeof(cst_ZFILEBLOCKSTART),wNextAddress);
  /*
    Pool rank             <..>
    address               <..>
    next block address    <..>
    theorical size        <..>

    Pool     File
          block size    <..>    <..>
              state         <..>    <..>
                  lock          <..>    <..>
                      pid           <..>    <..>
                          */

  //    QDialog wBlockViewDLg (this);
  setWindowTitle(QObject::tr("file block","poolVisu"));
  resize(400,150);

  QVBoxLayout* QVL=new QVBoxLayout(this);
  setLayout(QVL);

  QHBoxLayout* QHL=new QHBoxLayout;
  QVL->insertLayout(0,QHL);

  QLabel* wPoolLBl ;
  switch (pPoolId) {
  case 0:
    wPoolLBl = new QLabel("Block access table",this);
    break;
  case 1:
    wPoolLBl = new QLabel("Free blocks table",this);
    break;
  case 2:
    wPoolLBl = new QLabel("Hole table",this);
    break;
  default:
    wPoolLBl = new QLabel("Unknown pool",this);
    break;
  }


  QHL->addWidget(wPoolLBl,0,Qt::AlignCenter);


  QGridLayout* QGLyt=new QGridLayout;
  QVL->insertLayout(-1,QGLyt);

  int wLine=0;

  QLabel* wLb0=new QLabel("Pool rank",this);

  ErrorPalette = NormalPalette = wLb0->palette();
  ErrorPalette.setColor(wLb0->foregroundRole(), Qt::red);

  QGLyt->addWidget(wLb0,wLine,0);

  wStr.sprintf("%ld",pDataRank);
  QLabel* wRank = new QLabel(wStr.toCChar());
  QGLyt->addWidget(wRank,wLine,1);

  wLine++;

  QLabel* wLb1=new QLabel("Address",this);
  QGLyt->addWidget(wLb1,wLine,0);

  wStr.sprintf("%ld",pBD->Address);
  QLabel* wAddressLBl = new QLabel(wStr.toCChar());
  QGLyt->addWidget(wAddressLBl,wLine,1);

  wLine++;

  QLabel* wLb11=new QLabel("Next Block Address",this);
  QGLyt->addWidget(wLb11,wLine,0);

  QLabel* wNextAddressLBl = nullptr;
  while (true) {
    if (NextStartSignStatus!=ZS_SUCCESS) {

      if ((NextStartSignStatus==ZS_EOF)||(NextStartSignStatus== ZS_OUTBOUNDHIGH)) {
        wStr.sprintf("%ld",FileSize);
        wNextAddressLBl=new QLabel(wStr.toCChar());
        QGLyt->addWidget(wNextAddressLBl,wLine,1);

        QGLyt->addWidget(new QLabel("End of file reached"),wLine,2);

        wLine++;

        QLabel* wLb12=new QLabel("Computed block size",this);
        QGLyt->addWidget(wLb12,wLine,0);

        wStr.sprintf("%ld",FileSize-size_t(pBD->Address));
        QLabel* wThSizeLBl=new QLabel(wStr.toCChar(),this);
        QGLyt->addWidget(wThSizeLBl,wLine,1);
        break;
      }
      wNextAddressLBl = new QLabel("invalid");
      QGLyt->addWidget(wNextAddressLBl,wLine,1);
      QGLyt->addWidget(new QLabel(decode_ZStatus(NextStartSignStatus)),wLine,2);

      wLine++;

      QLabel* wLb12=new QLabel("Computed block size",this);
      QGLyt->addWidget(wLb12,wLine,0);

      QLabel* wThSizeLBl=new QLabel("invalid size",this);
      QGLyt->addWidget(wThSizeLBl,wLine,1);
      break;
    }

    wStr.sprintf("%ld",wNextAddress);
    wNextAddressLBl=new QLabel(wStr.toCChar());
    QGLyt->addWidget(wNextAddressLBl,wLine,1);

    QGLyt->addWidget(new QLabel("from file surface scan"),wLine,2);

    wLine++;

    QLabel* wLb12=new QLabel("Computed block size",this);
    QGLyt->addWidget(wLb12,wLine,0);

    wStr.sprintf("%ld",wNextAddress-pBD->Address-1);
    QLabel* wThSizeLBl=new QLabel(wStr.toCChar(),this);
    QGLyt->addWidget(wThSizeLBl,wLine,1);

    break;
  }//while true

  QGridLayout* QGLyt1=new QGridLayout;
  QVL->insertLayout(-1,QGLyt1);

  wLine=0;

  QGLyt1->addWidget(new QLabel("Block Descriptor"),wLine,0);
  QGLyt1->addWidget(new QLabel("Pool value"),wLine,1);
  QGLyt1->addWidget(new QLabel("File value"),wLine,2);

  wLine ++;

  QGLyt1->addWidget(new QLabel("Block size"),wLine,0);

  wStr.sprintf("%ld",pBD->BlockSize);
  BlockSize = new QLabel(wStr.toCChar());

  if (FileIsAccessible)
    wStr.sprintf("%ld",FBD.BlockSize);
  else
    wStr="invalid";
  BlockSizeF = new QLabel(wStr.toCChar());

  if (pBD->BlockSize!=FBD.BlockSize){
    BlockSize->setPalette(ErrorPalette);
    BlockSizeF->setPalette(ErrorPalette);
    FileDiffersFromPool = true;
  }

  QGLyt1->addWidget(BlockSize,wLine,1);
  QGLyt1->addWidget(BlockSizeF,wLine,2);
  if (FileBlockDescStatus!=ZS_SUCCESS)     /* if errored : add status on first field column 4 */
    QGLyt1->addWidget(new QLabel(decode_ZStatus(FileBlockDescStatus)),wLine,3);

  wLine++;

  QGLyt1->addWidget(new QLabel("State"),wLine,0);

  State = new QLabel(decode_ZBS(pBD->State));
  if (FileIsAccessible)
    StateF = new QLabel(decode_ZBS(FBD.State));
  else
    StateF = new QLabel("invalid");


  if (pBD->State!=FBD.State){
    State->setPalette(ErrorPalette);
    StateF->setPalette(ErrorPalette);
    FileDiffersFromPool = true;
  }

  QGLyt1->addWidget(State,wLine,1);
  QGLyt1->addWidget(StateF,wLine,2);

  wLine++;

  QGLyt1->addWidget(new QLabel("Lock"),wLine,0);

  Lock = new QLabel(decode_ZLockMask(pBD->Lock).toCChar());

  if (FileIsAccessible)
    LockF = new QLabel(decode_ZLockMask(FBD.Lock).toCChar());
  else
    LockF = new QLabel("invalid");

  if (pBD->Lock!=FBD.Lock){
    Lock->setPalette(ErrorPalette);
    LockF->setPalette(ErrorPalette);
    FileDiffersFromPool=true;
  }

  QGLyt1->addWidget(Lock,wLine,1);
  QGLyt1->addWidget(LockF,wLine,2);

  wLine++;

  QGLyt1->addWidget(new QLabel("pid"),wLine,0);

  wStr.sprintf("%8X",pBD->Pid);
  Pid = new QLabel(wStr.toCChar());

  if (FileIsAccessible) {
    wStr.sprintf("%8X",FBD.Pid);
    PidF = new QLabel(wStr.toCChar());
  }
  else
    PidF = new QLabel("invalid");



  if (pBD->Pid!=FBD.Pid){
    Pid->setPalette(ErrorPalette);
    PidF->setPalette(ErrorPalette);
    FileDiffersFromPool=true;
  }

  QGLyt1->addWidget(Pid,wLine,1);
  QGLyt1->addWidget(PidF,wLine,2);


  QGridLayout* QGLyt2=new QGridLayout;
  QVL->insertLayout(-1,QGLyt2);

  if (FileBlockDescStatus!=ZS_SUCCESS){
    wStr.sprintf("Cannot access block descriptor on file status %s",decode_ZStatus(FileBlockDescStatus));
    QGLyt2->addWidget(new QLabel("Error",this),wLine,0);
    QGLyt2->addWidget(new QLabel(wStr.toCChar(),this),wLine,1);
    wLine++;
  }

  QGLyt2->addWidget(new QLabel("Errors",this),wLine,0);

  QLabel* wStatus= new QLabel(decode_ZBEx(rawCheckContentBlock(pPoolId,ContentFd,*pBD)).toCChar());
  QGLyt2->addWidget(wStatus,wLine,1);


  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QVL->insertLayout(-1,QHLBtn);

  QPushButton* wCloseBTn = new QPushButton("Quit",this);
  QPushButton* wViewBTn = new QPushButton("View content",this);

  if (FileIsAccessible && FileDiffersFromPool) {
    PoolToFileBTn = new QPushButton("Set file from Pool",this);
    FileToPoolBTn = new QPushButton("Set Pool from File",this);
    CommitBTn = new QPushButton("Commit changes",this);
    CommitBTn->setVisible(false);
  }
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHLBtn->addItem(wSpacer);
  if (FileIsAccessible) {
    QHLBtn->addWidget(CommitBTn);
    QHLBtn->addWidget(PoolToFileBTn);
    QHLBtn->addWidget(FileToPoolBTn);
  }
  QHLBtn->addWidget(wViewBTn);
  QHLBtn->addWidget(wCloseBTn);

  QObject::connect(wCloseBTn, &QPushButton::clicked, this, &editBlockDLg::Quit);
  QObject::connect(wViewBTn, &QPushButton::clicked, this, &editBlockDLg::ShowDetails);
  if (FileIsAccessible && FileDiffersFromPool) {
    QObject::connect(FileToPoolBTn, &QPushButton::clicked, this, &editBlockDLg::FileToPool);
    QObject::connect(PoolToFileBTn, &QPushButton::clicked, this, &editBlockDLg::PoolToFile);
    QObject::connect(CommitBTn, &QPushButton::clicked, this, &editBlockDLg::CommitChanges);
  }

}

void editBlockDLg::PoolToFile() {

  if (!FileIsAccessible) {
    ZExceptionDLg::adhocMessage("Copy to file",Severity_Error,"File descriptor cannot be accessed.");
    return;
  }

  utf8VaryingString wStr;

  FBD.BlockSize = BD->BlockSize;
  FBD.Pid = BD->Pid;
  FBD.State = BD->State;
  FBD.Lock = BD->Lock;

  BlockSize->setPalette(NormalPalette);
  BlockSizeF->setPalette(NormalPalette);
  wStr.sprintf("%ld",FBD.BlockSize);
  BlockSizeF->setText(wStr.toCChar());

  State->setPalette(NormalPalette);
  StateF->setPalette(NormalPalette);
  StateF->setText(decode_ZBS(FBD.State));

  Lock->setPalette(NormalPalette);
  LockF->setPalette(NormalPalette);
  LockF->setText(decode_ZLockMask(FBD.Lock).toCChar());

  Pid->setPalette(NormalPalette);
  PidF->setPalette(NormalPalette);
  wStr.sprintf("%8X",FBD.Pid);
  PidF->setText(wStr.toCChar());

  HasChanged|=EBK_FileChanged;

  CommitBTn->setVisible(true);
  PoolToFileBTn->setVisible(false);
  FileToPoolBTn->setVisible(false);

}

void editBlockDLg::FileToPool() {
  HasChanged|=EBK_PoolChanged;
}

void editBlockDLg::Unlock() {
  if ((BD->Lock==ZLock_Nolock) && (FBD.Lock == ZLock_Nolock)) {
    ZExceptionDLg::adhocMessage("Unlock",Severity_Warning,"Block is not locked");
    return;
  }


  HasChanged|=EBK_PoolChanged|EBK_FileChanged;
}

void editBlockDLg::CommitChanges() {
  if (!HasChanged) {
    ZExceptionDLg::adhocMessage("Commit changes",Severity_Error,"There is nothing to change");
    return;
  }




  HasChanged=false;
  QDialog::done(3);
}

void editBlockDLg::Quit() {
  if (HasChanged) {

    int wRet= ZExceptionDLg::adhocMessage2B("Quitting",Severity_Warning,
        "Ooops","Confirm",
        "There are pending changes. Confirm that you do not want to commit");
    if (wRet==QDialog::Rejected)
      return;
  }
  QDialog::reject();
}

void editBlockDLg::ShowDetails() {
  if (HasChanged) {

    int wRet= ZExceptionDLg::adhocMessage2B("Exiting",Severity_Warning,
        "Ooops","Confirm",
        "There are pending changes. Confirm that you do not want to commit");
    if (wRet==QDialog::Rejected)
      return;
  }
  QDialog::accept();
}
