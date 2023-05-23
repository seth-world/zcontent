#include "visuraw.h"

#include <zqt/zqtwidget/zqtableview.h>
#include <zexceptiondlg.h>

#include <ztoolset/zdatabuffer.h>

#include <zcontent/zcontentcommon/urfparser.h>
#include <zcontent/zcontentcommon/urffield.h>

#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include <QContextMenuEvent>
#include <QAction>
#include <QActionGroup>
#include <QMenu>

#include <visulinecol.h>

#include <zcontent/zrandomfile/zblock.h> // for ZBlockHeader_Export

VisuRaw::VisuRaw(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord)
{
  VisuTBv=pBlockDumpTBv;
  RawData=pRawRecord;
}


void VisuRaw::visuBlockHeader(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord){
  utf8VaryingString wStr;

  size_t wValueSize=0;
  QModelIndex wIdx=pBlockDumpTBv->currentIndex();
  if (!wIdx.isValid())
    return;
  ssize_t wOffset=computeOffsetFromCoord(wIdx.row(),wIdx.column());
  if (wOffset+sizeof(ZBlockHeader_Export) > pRawRecord->Size ) {
    ZExceptionDLg::adhocMessage("Block header",Severity_Error,"Out of file's boundaries.\nThere is no enough room for a block header data structure");
    return;
  }

  ZBlockHeader_Export wBHExp ;
  memmove(&wBHExp,pRawRecord->Data + wOffset,sizeof(ZBlockHeader_Export)) ;

  if (wBHExp.StartSign != cst_ZFILEBLOCKSTART) {
    ZExceptionDLg::adhocMessage("Block header",Severity_Error,"This is not a valid block header (wrong start sign)");
    return;
  }

  /* Visu dialog common setup */

  QDialog wVisuDLg (nullptr);
  wVisuDLg.setObjectName("visuBlockHeader");
  wVisuDLg.setWindowTitle(QObject::tr("Evaluate block header","ZContentVisuMain"));

  wVisuDLg.resize(450,150);

  QVBoxLayout* QVL=new QVBoxLayout(&wVisuDLg);
  QVL->setObjectName("QVL");
  wVisuDLg.setLayout(QVL);

  QGridLayout* QGLyt=new QGridLayout;
  QGLyt->setObjectName("QGLyt");
  QVL->insertLayout(0,QGLyt);

  /* dialog setup */

  QLabel* LBlStart=new QLabel("Start sign",&wVisuDLg);
  QGLyt->addWidget(LBlStart,0,0);
  QLabel* wStartLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wStartLBl,0,1);

  wStr.sprintf("%08X",wBHExp.StartSign);
  wStartLBl->setText(wStr.toCChar());

  QLabel* wLbEndianLBl=new QLabel("Endian check",&wVisuDLg);
  QGLyt->addWidget(wLbEndianLBl,1,0);

  QLabel* wEndianRawLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wEndianRawLBl,1,1);
  wStr.sprintf("0x%08X",wBHExp.EndianCheck);
  wEndianRawLBl->setText(wStr.toCChar());

  QLabel* wEndianRevLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wEndianRevLBl,1,2);
  wEndianRevLBl->setText(wBHExp.isReversed()?"Reversed":"Not reversed");


  QLabel* wLbBlockSizeLBl=new QLabel("Block size",&wVisuDLg);
  QGLyt->addWidget(wLbBlockSizeLBl,2,0);

  QLabel* wBlockSizeRawLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wBlockSizeRawLBl,2,1);
  wStr.sprintf("0x%16lX",wBHExp.BlockSize);
  wBlockSizeRawLBl->setText(wStr.toCChar());

  QLabel* wBlockSizeLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wBlockSizeLBl,2,2);
  uint64_t  wValue = reverseByteOrder_Conditional<uint64_t>(wBHExp.BlockSize);
  wStr.sprintf("%lld",wValue);
  wBlockSizeLBl->setText(wStr.toCChar());


  QLabel* wLbStateLBl=new QLabel("State",&wVisuDLg);
  QGLyt->addWidget(wLbStateLBl,3,0);

  QLabel* wStateRawLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wStateRawLBl,3,1);
  wStr.sprintf("0x%02X",wBHExp.State);
  wStateRawLBl->setText(wStr.toCChar());

  QLabel* wStateLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wStateLBl,3,2);
  wStateLBl->setText(decode_ZBS(wBHExp.State));


  QLabel* wLbLockLBl=new QLabel("Lock mask",&wVisuDLg);
  QGLyt->addWidget(wLbLockLBl,4,0);

  QLabel* wLbLockRawLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wLbLockRawLBl,4,1);
  wStr.sprintf("0x%02X",wBHExp.Lock);
  wLbLockRawLBl->setText(wStr.toCChar());

  QLabel* wLockLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wLockLBl,4,2);
  wLockLBl->setText(decode_ZLockMask(wBHExp.Lock).toChar());


  QLabel* wLbPidLBl=new QLabel("Pid",&wVisuDLg);
  QGLyt->addWidget(wLbPidLBl,5,0);

  QLabel* wPidRawLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wPidRawLBl,5,1);
  wStr.sprintf("%d",wBHExp.Pid);
  wPidRawLBl->setText(wStr.toCChar());

  QLabel* wPidLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wPidLBl,5,2);
  pid_t wPid = reverseByteOrder_Conditional<pid_t>(wBHExp.Pid);
  wStr.sprintf("%d",wPid);
  wPidLBl->setText(wStr.toCChar());

  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QHLBtn->setObjectName("QHLBtn");
  QVL->insertLayout(1,QHLBtn);


  QPushButton* wNext=new QPushButton(QObject::tr("Next","ZContentVisuMain"),&wVisuDLg);
  QPushButton* wClose=new QPushButton(QObject::tr("Close","ZContentVisuMain"),&wVisuDLg);
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHLBtn->addItem(wSpacer);

  QHLBtn->addWidget(wNext);
  QHLBtn->addWidget(wClose);


  QObject::connect(wNext, &QPushButton::clicked, &wVisuDLg, &QDialog::accept);
  QObject::connect(wClose, &QPushButton::clicked, &wVisuDLg, &QDialog::reject);

  int wRet=wVisuDLg.exec();

  if (wRet==QDialog::Rejected) {
    return;
  }

  /* skip value in offset */
  if ( (wOffset + wValueSize) > pRawRecord->Size )
    return;
  //  setSearchOffset(wOffset+wValueSize);
  VisuLineCol wNewPosition;
  wOffset+=sizeof(ZBlockHeader_Export);
  wNewPosition.compute (wOffset);
  QModelIndex wNewIdx = wIdx.sibling(wNewPosition.line,wNewPosition.col);
  if (!wNewIdx.isValid())
    return ;

  pBlockDumpTBv->setFocus();
  pBlockDumpTBv->setCurrentIndex(wNewIdx);

  return;
} // visuBlockHeader

void VisuRaw::visuURFField(ZQTableView* pBlockDumpTBv,const ZDataBuffer* pRawRecord){
  utf8VaryingString wStr;
  URFParser wURFParser;
  QModelIndex wIdx=pBlockDumpTBv->currentIndex();
  if (!wIdx.isValid())
    return;
  ssize_t wOffset=computeOffsetFromCoord(wIdx.row(),wIdx.column());

  const unsigned char* wPtr = pRawRecord->Data + wOffset;

  ssize_t wFieldSize = wURFParser.getURFFieldSize(wPtr);
  if (wFieldSize < 0) {
    ZExceptionDLg::adhocMessage("URF field",Severity_Error,"Invalid URF field ZType.");
    return;
  }


  /* Visu dialog common setup */

  QDialog wVisuDLg (nullptr);
  wVisuDLg.setObjectName("visuURFField");
  wVisuDLg.setWindowTitle(QObject::tr("Evaluate URF field","ZContentVisuMain"));

  wVisuDLg.resize(450,150);

  QVBoxLayout* QVL=new QVBoxLayout(&wVisuDLg);
  wVisuDLg.setLayout(QVL);

  QGridLayout* QGLyt=new QGridLayout;
  QVL->insertLayout(-1,QGLyt);


  /* dialog setup */

  QLabel* LBlZType=new QLabel("ZType",&wVisuDLg);
  QGLyt->addWidget(LBlZType,0,0);

  QLabel* wZTypeNumLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wZTypeNumLBl,0,1);

  ZTypeBase wZType= reverseByteOrder_Ptr<ZTypeBase>(wPtr);
  wStr.sprintf("%08X",wZType);
  wZTypeNumLBl->setText(wStr.toCChar());

  QLabel* wZTypeLBl=new QLabel(&wVisuDLg);
  QGLyt->addWidget(wZTypeLBl,0,2);
  wZTypeLBl->setText(decode_ZType(wZType));

  QVBoxLayout* QVLVal=new QVBoxLayout;
  QVL->insertLayout(-1,QVLVal); /* negative index  means insert at the end */
/*
  QLabel* wLbValueLBl=new QLabel("Field content",&wVisuDLg);
  QVL->addWidget(wLbValueLBl);
*/
  QLineEdit* wTL = new QLineEdit(&wVisuDLg);
  wTL->setText(URFParser::displayOneURFField(wPtr,false).toCChar());
  QVL->addWidget(wTL);

  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QVL->insertLayout(-1,QHLBtn);

  QPushButton* wNext=new QPushButton(QObject::tr("Next","ZContentVisuMain"),&wVisuDLg);
  QPushButton* wClose=new QPushButton(QObject::tr("Close","ZContentVisuMain"),&wVisuDLg);
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHLBtn->addItem(wSpacer);

  QHLBtn->addWidget(wNext);
  QHLBtn->addWidget(wClose);

  QObject::connect(wNext, &QPushButton::clicked, &wVisuDLg, &QDialog::accept);
  QObject::connect(wClose, &QPushButton::clicked, &wVisuDLg, &QDialog::reject);

  int wRet=wVisuDLg.exec();

  if (wRet==QDialog::Rejected) {
    return;
  }

  /* skip value in offset */
  if ( (wOffset + wFieldSize) > pRawRecord->Size )
    return;
  //  setSearchOffset(wOffset+wValueSize);
  VisuLineCol wNewPosition;
  wOffset += wFieldSize;
  wNewPosition.compute (wOffset);
  QModelIndex wNewIdx = wIdx.sibling(wNewPosition.line,wNewPosition.col);
  if (!wNewIdx.isValid())
    return ;

  pBlockDumpTBv->setFocus();
  pBlockDumpTBv->setCurrentIndex(wNewIdx);

  return;
} // visuURFField


void VisuRaw::visuAtomic(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord,VisuRaw_type pVRTP) {

  /* Visu dialog common setup */

  utf8VaryingString wStr;

  size_t wValueSize=0;
  QModelIndex wIdx=pBlockDumpTBv->currentIndex();
  if (!wIdx.isValid())
    return;

  ssize_t wOffset=computeOffsetFromCoord(wIdx.row(),wIdx.column());

  QDialog wVisuDLg (nullptr);
  wVisuDLg.setObjectName("visuAtomic");
  wVisuDLg.setWindowTitle(QObject::tr("Evaluate values","ZContentVisuMain"));

  wVisuDLg.resize(450,150);

  QVBoxLayout* QVL=new QVBoxLayout(&wVisuDLg);
  wVisuDLg.setLayout(QVL);

  QGridLayout* QGLyt=new QGridLayout;
  QVL->insertLayout(0,QGLyt);

  QHBoxLayout* QHL=new QHBoxLayout;
  QGLyt->addLayout(QHL,0,4);
  QHL->setAlignment(Qt::AlignCenter);

  /* for any atomic data view */

  QLabel* LBlType=new QLabel("data format",&wVisuDLg);
  QHL->addWidget(LBlType);
  QLabel* wTypeLBl=new QLabel(&wVisuDLg);
  QHL->addWidget(wTypeLBl);

  QLabel* wLbDec=new QLabel(QObject::tr("Decimal","ZContentVisuMain"),&wVisuDLg);
  QGLyt->addWidget(wLbDec,1,1);
  QLabel* wLbHexa=new QLabel(QObject::tr("Hexa","ZContentVisuMain"),&wVisuDLg);
  QGLyt->addWidget(wLbHexa,1,4);

  QLabel* wLb=new QLabel(QObject::tr("Raw","ZContentVisuMain"),&wVisuDLg);
  wLb->setAlignment(Qt::AlignCenter);
  QGLyt->addWidget(wLb,2,0);

  QLineEdit* wRawValueLEd=new QLineEdit(&wVisuDLg);
  wRawValueLEd->setAlignment(Qt::AlignRight);
  QGLyt->addWidget(wRawValueLEd,2,1);

  QLineEdit* wRawHexaLEd=new QLineEdit(&wVisuDLg);
  wRawHexaLEd->setAlignment(Qt::AlignRight);
  QGLyt->addWidget(wRawHexaLEd,2,4);

  QLabel* wLb1=new QLabel(QObject::tr("Deserialized","ZContentVisuMain"),&wVisuDLg);
  QGLyt->addWidget(wLb1,3,0);
  wLb1->setAlignment(Qt::AlignCenter);
  QLineEdit* wDeserializedLEd=new QLineEdit(&wVisuDLg);
  wDeserializedLEd->setAlignment(Qt::AlignRight);
  QGLyt->addWidget(wDeserializedLEd,3,1);

  QLineEdit* wDeserializedHexaLEd=new QLineEdit(&wVisuDLg);
  wDeserializedHexaLEd->setAlignment(Qt::AlignRight);
  QGLyt->addWidget(wDeserializedHexaLEd,3,4);

  QHBoxLayout* QHLBtn=new QHBoxLayout;
  QHLBtn->setObjectName("QHLBtn");
  QVL->insertLayout(1,QHLBtn);

  QLineEdit* wAlphaLEd=new QLineEdit(&wVisuDLg);
  QVL->insertWidget(2,wAlphaLEd);



  QPushButton* wNext=new QPushButton(QObject::tr("Next","ZContentVisuMain"),&wVisuDLg);
  QPushButton* wClose=new QPushButton(QObject::tr("Close","ZContentVisuMain"),&wVisuDLg);
  QSpacerItem* wSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QHLBtn->addItem(wSpacer);

  QHLBtn->addWidget(wNext);
  QHLBtn->addWidget(wClose);

  QObject::connect(wNext, &QPushButton::clicked, &wVisuDLg, &QDialog::accept);
  QObject::connect(wClose, &QPushButton::clicked, &wVisuDLg, &QDialog::reject);

  if (pVRTP ==VRTP_ZType){
    wValueSize=sizeof(ZTypeBase);
    if (wOffset+sizeof(ZTypeBase) > pRawRecord->Size )
      return;
    wTypeLBl->setText("ZType");
    ZTypeBase * wValuePtr = (ZTypeBase *)(pRawRecord->Data + wOffset);
    ZTypeBase wValue = *wValuePtr;
    ZTypeBase wDeSerialized = reverseByteOrder_Conditional<ZTypeBase>(wValue);
    wStr.sprintf("%u",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%u",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
    wAlphaLEd->setText(decode_ZType(wDeSerialized));
  } //VRTP_ZType

  if (pVRTP ==VRTP_uint16) {
    wValueSize=sizeof(uint16_t);
    if (wOffset+sizeof(uint16_t) > pRawRecord->Size )
      return;
    wTypeLBl->setText("uint16");
    uint16_t * wValuePtr = (uint16_t *)(pRawRecord->Data + wOffset);
    uint16_t wValue = *wValuePtr;
    uint16_t wDeSerialized = reverseByteOrder_Conditional<uint16_t>(wValue);
    wStr.sprintf("%u",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%u",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } // VRTP_uint16

  if (pVRTP ==VRTP_int16) {
    wValueSize=sizeof(int16_t);
    if (wOffset+sizeof(int16_t) > pRawRecord->Size )
      return;
    wTypeLBl->setText("int16");
    int16_t * wValuePtr = (int16_t *)(pRawRecord->Data + wOffset);
    int16_t wValue = *wValuePtr;
    int16_t wDeSerialized = reverseByteOrder_Conditional<int16_t>(wValue);
    wStr.sprintf("%d",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%d",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%04X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } // VRTP_int16

  if (pVRTP ==VRTP_uint32) {
    wValueSize=sizeof(uint32_t);
    if (wOffset+sizeof(uint32_t) > pRawRecord->Size )
      return;
    wTypeLBl->setText("uint32");
    uint32_t * wValuePtr = (uint32_t *)(pRawRecord->Data + wOffset);
    uint32_t wValue = *wValuePtr;
    uint32_t wDeSerialized = reverseByteOrder_Conditional<uint32_t>(wValue);
    wStr.sprintf("%u",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%u",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } //VRTP_uint32

  if (pVRTP ==VRTP_int32) {
    wValueSize=sizeof(int32_t);
    if (wOffset+sizeof(int32_t) > pRawRecord->Size )
      return;
    wTypeLBl->setText("int32");
    int32_t * wValuePtr = (int32_t *)(pRawRecord->Data + wOffset);
    int32_t wValue = *wValuePtr;
    int32_t wDeSerialized = reverseByteOrder_Conditional<int32_t>(wValue);
    wStr.sprintf("%d",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%d",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%08X",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } // VRTP_int32

  if (pVRTP ==VRTP_uint64) {
    wValueSize=sizeof(uint64_t);
    if (wOffset+sizeof(uint64_t) > pRawRecord->Size )
      return;
    wTypeLBl->setText("uint64");
    uint64_t * wValuePtr = (uint64_t *)(pRawRecord->Data + wOffset);
    uint64_t wValue = *wValuePtr;
    uint64_t wDeSerialized = reverseByteOrder_Conditional<uint64_t>(wValue);
    wStr.sprintf("%lu",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%lu",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } // VRTP_uint64

  if (pVRTP ==VRTP_int64) {
    wValueSize=sizeof(int64_t);
    if (wOffset+sizeof(int64_t) > pRawRecord->Size )
      return;
    wTypeLBl->setText("int64");
    int64_t * wValuePtr = (int64_t *)(pRawRecord->Data + wOffset);
    int64_t wValue = *wValuePtr;
    int64_t wDeSerialized = reverseByteOrder_Conditional<int64_t>(wValue);
    wStr.sprintf("%ld",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%ld",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } // VRTP_int64

  if (pVRTP ==VRTP_sizet) {
    wValueSize=sizeof(size_t);
    if (wOffset+sizeof(size_t) > pRawRecord->Size )
      return;
    wTypeLBl->setText("size_t");
    size_t * wValuePtr = (size_t *)(pRawRecord->Data + wOffset);
    size_t wValue = *wValuePtr;
    size_t wDeSerialized = reverseByteOrder_Conditional<size_t>(wValue);
    wStr.sprintf("%lu",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    if (sizeof(size_t) > 4) {
      wStr.sprintf("%lu",wValue);
      wRawValueLEd->setText(wStr.toCChar());
      wStr.sprintf("%016lX",wValue);
      wRawHexaLEd->setText(wStr.toCChar());
      wStr.sprintf("%lu",wDeSerialized);
      wDeserializedLEd->setText(wStr.toCChar());
      wStr.sprintf("%016lX",wDeSerialized);
      wDeserializedHexaLEd->setText(wStr.toCChar());
    }
    else {
      wStr.sprintf("%u",wValue);
      wRawValueLEd->setText(wStr.toCChar());
      wStr.sprintf("%08X",wValue);
      wRawHexaLEd->setText(wStr.toCChar());
      wStr.sprintf("%u",wDeSerialized);
      wDeserializedLEd->setText(wStr.toCChar());
      wStr.sprintf("%08X",wDeSerialized);
      wDeserializedHexaLEd->setText(wStr.toCChar());
    }
  }//VRTP_sizet
  if (pVRTP ==VRTP_float) {
    wValueSize=sizeof(float);
    if (wOffset+sizeof(float) > pRawRecord->Size )
      return;
    wTypeLBl->setText("float");
    float * wValuePtr = (float *)(pRawRecord->Data + wOffset);
    float wValue = *wValuePtr;
    float wDeSerialized = reverseByteOrder_Conditional<float>(wValue);
    wStr.sprintf("%g",double(wValue));
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",double(wValue));
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%g",double(wDeSerialized));
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",double(wDeSerialized));
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } // VRTP_float
  if (pVRTP ==VRTP_double) {
    wValueSize=sizeof(double);
    if (wOffset+sizeof(double) > pRawRecord->Size )
      return;
    wTypeLBl->setText("double");
    double * wValuePtr = (double *)(pRawRecord->Data + wOffset);
    double wValue = *wValuePtr;
    double wDeSerialized = reverseByteOrder_Conditional<double>(wValue);
    wStr.sprintf("%g",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%g",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } // VRTP_double

  if (pVRTP ==VRTP_longdouble) {
    wValueSize=sizeof(long double);
    if (wOffset+sizeof(long double) > pRawRecord->Size )
      return;
    wTypeLBl->setText("double");
    long double * wValuePtr = (long double *)(pRawRecord->Data + wOffset);
    long double wValue = *wValuePtr;
    long double wDeSerialized = reverseByteOrder_Conditional<long double>(wValue);
    wStr.sprintf("%lg",wValue);
    wRawValueLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wValue);
    wRawHexaLEd->setText(wStr.toCChar());
    wStr.sprintf("%lg",wDeSerialized);
    wDeserializedLEd->setText(wStr.toCChar());
    wStr.sprintf("%016lX",wDeSerialized);
    wDeserializedHexaLEd->setText(wStr.toCChar());
  } // VRTP_longdouble

  int wRet=wVisuDLg.exec();

  if (wRet==QDialog::Rejected) {
    return;
  }
  /* skip value in offset */

  if ( (wOffset + wValueSize) > pRawRecord->Size )
    return;

  VisuLineCol wNewPosition;
  wOffset+=wValueSize;
  wNewPosition.compute (wOffset);
  QModelIndex wNewIdx = wIdx.sibling(wNewPosition.line,wNewPosition.col);
  if (!wNewIdx.isValid())
    abort();

  pBlockDumpTBv->setFocus();
  pBlockDumpTBv->setCurrentIndex(wNewIdx);

  return;
} // visuAtomic

void VisuRaw::visuActionEvent(ZQTableView* pBlockDumpTBv,const ZDataBuffer *pRawRecord,VisuRaw_type pVRTP) {

  switch (pVRTP){
  case VRTP_ZBlockHeader :
    visuBlockHeader(pBlockDumpTBv,pRawRecord);
    return;
  case VRTP_URFField :
    visuURFField(pBlockDumpTBv,pRawRecord);
    return;
  case VRTP_ZType:
  case VRTP_uint16:
  case VRTP_int16:
  case VRTP_uint64:
  case VRTP_int64:
  case VRTP_sizet:

  case VRTP_float:
  case VRTP_double:
  case VRTP_longdouble:
    visuAtomic(pBlockDumpTBv,pRawRecord,pVRTP);
    return;
  default:
    return;
  }

  return;
}


void VisuRaw::VisuBvFlexMenuCallback(QContextMenuEvent *event)
{
  QMenu* visuFlexMEn=new QMenu;
  visuFlexMEn->setTitle(QObject::tr("Evaluate","ZContentVisuMain"));

  QActionGroup* visuActionGroup=new QActionGroup(visuFlexMEn) ;
  QObject::connect(visuActionGroup, SIGNAL(triggered(QAction*)), this,  SLOT(visuActionEvent(QAction*)));

  ZBlockHeaderQAc= new QAction("ZBLockHeader",visuFlexMEn);
  visuFlexMEn->addAction(ZBlockHeaderQAc);
  visuActionGroup->addAction(ZBlockHeaderQAc);

  URFFieldQAc= new QAction("URF field",visuFlexMEn);
  visuFlexMEn->addAction(URFFieldQAc);
  visuActionGroup->addAction(URFFieldQAc);

  visuFlexMEn->addSeparator();

  ZTypeQAc= new QAction("ZType",visuFlexMEn);
  visuFlexMEn->addAction(ZTypeQAc);
  visuActionGroup->addAction(ZTypeQAc);

  uint16QAc= new QAction("uint16",visuFlexMEn);
  visuFlexMEn->addAction(uint16QAc);
  visuActionGroup->addAction(uint16QAc);

  int16QAc= new QAction("int16",visuFlexMEn);
  visuFlexMEn->addAction(int16QAc);
  visuActionGroup->addAction(int16QAc);

  uint32QAc= new QAction("uint32",visuFlexMEn);
  visuFlexMEn->addAction(uint32QAc);
  visuActionGroup->addAction(uint32QAc);

  int32QAc= new QAction("int32",visuFlexMEn);
  visuFlexMEn->addAction(int32QAc);
  visuActionGroup->addAction(int32QAc);

  uint64QAc= new QAction("uint64",visuFlexMEn);
  visuFlexMEn->addAction(uint64QAc);
  visuActionGroup->addAction(uint64QAc);

  int64QAc= new QAction("int64",visuFlexMEn);
  visuFlexMEn->addAction(int64QAc);
  visuActionGroup->addAction(int64QAc);

  sizetQAc= new QAction("size_t",visuFlexMEn);
  visuFlexMEn->addAction(sizetQAc);
  visuActionGroup->addAction(sizetQAc);

  floatQAc= new QAction("float",visuFlexMEn);
  visuFlexMEn->addAction(floatQAc);
  visuActionGroup->addAction(floatQAc);

  doubleQAc= new QAction("double",visuFlexMEn);
  visuFlexMEn->addAction(doubleQAc);
  visuActionGroup->addAction(doubleQAc);

  longdoubleQAc= new QAction("long double",visuFlexMEn);
  visuFlexMEn->addAction(longdoubleQAc);
  visuActionGroup->addAction(longdoubleQAc);

  visuFlexMEn->exec(event->globalPos());
  visuFlexMEn->deleteLater();
}//VisuBvFlexMenu

void VisuRaw::visuActionEvent(QAction* pAction) {

  if (pAction==ZBlockHeaderQAc) {
    VisuRaw::visuBlockHeader(VisuTBv,RawData);
    return;
  } // if (pAction==ZBlockHeaderQAc)

  if (pAction==URFFieldQAc) {
    VisuRaw::visuURFField(VisuTBv,RawData);
    return;
  } // if (pAction==ZBlockHeaderQAc)

  if (pAction==ZTypeQAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_ZType);
    return;
  } // ZTypeQAc
  if (pAction==uint16QAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_uint16);
    return;
  } // uint16QAc
  if (pAction==int16QAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_int16);
    return;
  } // int16QAc
  if (pAction==uint32QAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_uint32);
    return;
  }
  if (pAction==int32QAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_int32);
    return;
  }
  if (pAction==uint64QAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_uint64);
    return;
  }
  if (pAction==int64QAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_int64);
    return;
  }
  if (pAction==sizetQAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_sizet);
    return;
  }

  if (pAction==floatQAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_float);
    return;
  }
  if (pAction==doubleQAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_double);
    return;
  }
  if (pAction==longdoubleQAc) {
    VisuRaw::visuAtomic(VisuTBv,RawData,VRTP_longdouble);
    return;
  }

  return;
}//visuActionEvent
