#include "zfielddlg.h"
#include "ui_zfielddlg.h"
#include "ztypedlg.h"

#include <zcontent/zcontentutils/ztypelistbutton.h>
#include <zexceptiondlg.h>

#include <qevent.h>
#include <zqt/zqtwidget/zqstandarditemmodel.h>
#include <zqt/zqtwidget/zqstandarditem.h>
#include <zqt/zqtwidget/zqtwidgettools.h>

#include <ztoolset/ztypetype.h>
#include <zcontent/zindexedfile/zdatatype.h>



#define __ICON_PATH__ "/home/gerard/Development/zbasetools/zqt/icons/"
#define __HASH_OK_ICON__      "HashcodeOk.png"
#define __HASH_WRONG_ICON__   "HashcodeWrong.png"

ZFieldDLg::ZFieldDLg(QWidget *parent) : QDialog(parent), ui(new Ui::ZFieldDLg)
{
  ui->setupUi(this);

  ui->CommentLBl->clear();

  uriString wImgFile = __ICON_PATH__;
  wImgFile += __HASH_OK_ICON__;
  HashOKPXm.load(wImgFile.toCChar());

  wImgFile = __ICON_PATH__;
  wImgFile += __HASH_WRONG_ICON__;
  HashWrongPXm.load(wImgFile.toCChar());

  ui->HashIconLBl->setScaledContents(true);
  ui->HashIconLBl->setPixmap(HashWrongPXm);

  setWindowTitle(QObject::tr("Dictionary field","DicEdit"));

  TypeListBtn= new ZTypeListButton(ui->ZTypeListWDg);
  TypeListBtn->setObjectName(QString::fromUtf8("wTypeListBtn"));
  TypeListBtn->setGeometry(QRect(10, 10, 51, 23));
  TypeListBtn->setText(QObject::tr("List","DicEdit"));

  QWidget::setTabOrder(ui->FieldNameLEd, TypeListBtn);
  QWidget::setTabOrder(TypeListBtn, ui->UniversalLEd);

//  QObject::connect(ui->ZTypeLEd,SIGNAL(sigFocusIn(QFocusEvent*)),this,SLOT(ZTypeFocusIn(QFocusEvent*)));
  QObject::connect(TypeListBtn,SIGNAL(clicked()),this,SLOT(ZTypeListClicked()));

  QObject::connect(ui->DiscardBTn,SIGNAL(clicked()),this,SLOT(DiscardClicked()));
  QObject::connect(ui->AcceptBTn,SIGNAL(clicked()),this,SLOT(AcceptClicked()));

  QObject::connect(ui->ComputeBTn,SIGNAL(clicked()),this,SLOT(ComputeClicked()));

  QObject::connect(ui->CreateArrayBTn,SIGNAL(clicked()),this,SLOT(ArrayClicked()));
  ui->CreateArrayBTn->setVisible(false);

  ui->AcceptBTn->setDefault(true);

}

ZFieldDLg::~ZFieldDLg()
{
  delete ui;
}

int ZFieldDLg::exec()
{
//  if (Field==nullptr)
//    reject();
  return QDialog::exec();
}


void
ZFieldDLg::ZTypeListClicked()
{
  utf8String wStr;
  if (ZTypeTRv==nullptr)
    {
    ZTypeTRv=new ZTypeDLg(this);
    ZTypeTRv->setWindowTitle(QObject::tr("Data types","ZFieldDLg"));
    ZTypeTRv->setModal(true);
    }

  ZTypeTRv->set(Field.ZType);
  while (true) {
    int wRet=ZTypeTRv->exec();
    if (wRet==QDialog::Rejected) {
      ZTypeTRv->deleteLater();
      ZTypeTRv=nullptr;
      return;
    }

    ZTypeBase wType = ZTypeTRv->get();
    ZTypeBase wType2 = wType & ~ZType_Array ;
    if ((wType & ZType_Array) && !isAtomic(wType2)) {
      wRet=ZExceptionDLg::message2B("ZFieldDLg",ZS_INVTYPE,Severity_Error,
          "Quit","Retry",
          "An array must concern uniquely atomic data types.\n0X%X-<%s> is an invalid data type.",
          wType,decode_ZType(wType));
      if (wRet==QDialog::Rejected) {
        ZTypeTRv->deleteLater();
        ZTypeTRv=nullptr;
        return;
      }
      continue;
    }
    break;
  }//while true

  TypeListBtn->ZType = Field.ZType = ZTypeTRv->get();

  wStr.sprintf("0x%08X",Field.ZType);
  ui->ZTypeLEd->setText(wStr.toCChar());
  ui->ZTypeLEd->setReadOnly(true);

  ui->ZTypeStrLEd->setText(decode_ZType(Field.ZType));
  ui->ZTypeStrLEd->setReadOnly(true);

  Field.HeaderSize = _getURFHeaderSize(Field.ZType);
  wStr.sprintf("%ld",Field.HeaderSize);
  ui->HeaderSizeLEd->setText(wStr.toCChar());


  ui->KeyEligibleCHk->setChecked(!(Field.ZType & ZType_VaryingLength));

  if (Field.ZType & ZType_Atomic) {
    ui->CreateArrayBTn->setVisible(true);
    getAtomicZType_Sizes(Field.ZType,Field.NaturalSize,Field.UniversalSize);

    wStr.sprintf("%ld",Field.NaturalSize);
    ui->NaturalLEd->setText(wStr.toCChar());

    wStr.sprintf("%ld",Field.UniversalSize);
    ui->UniversalLEd->setText(wStr.toCChar());

    Field.Capacity=1;
    wStr.sprintf("%ld",Field.Capacity);
    ui->CapacityLEd->setText(wStr.toCChar());
  }
  else
    ui->CreateArrayBTn->setVisible(false);

}//ListClicked

void
ZFieldDLg::AcceptClicked()
{
  refresh();
  if (RawField) {
    accept();
    return;
  }
  if (controlField())
    accept();
}

void
ZFieldDLg::DiscardClicked()
{
  reject();
}


void
ZFieldDLg::ArrayClicked() {
/* This button is set visible if ZType is atomic
 *
 * 0- check ZType is atomic - check Capacity is non zero
 * 1- get universal and natural values from ZType
 * 2- compute natural and universal values
 * 3- put comment : array created with xxx occurence of natural xxxx and universal xxx each
 */

  refresh();

  if  (Field.Capacity==0)  {
    ZExceptionDLg::message("ZFieldDLg::ArrayClicked",ZS_INVOP,Severity_Error,"Cannot create an array.Capacity must reflect the number of array occurences.");
    return;
  }
  if (!isAtomic(Field.ZType ))  {
    ZExceptionDLg::message("ZFieldDLg::ArrayClicked",ZS_INVOP,Severity_Error,"Cannot create an array. Data type must be atomic.");
    return;
  }

  ZTypeBase wType=Field.ZType;

  /* already an Array ? */
  if (Field.ZType & ZType_Array )  {
    wType &= ~ ZType_Array;
  }
  /* get universal and natural values from ZType */

  size_t wUV=0, wNV=0;

  getAtomicZType_Sizes(wType,wNV,wUV);

  Field.NaturalSize = wNV * Field.Capacity;
  Field.UniversalSize = wNV * Field.Capacity;

  Field.ZType |= ZType_Array;

  setup(Field,RawField);

  /* put comment */
  utf8VaryingString wComment;
  wComment.sprintf("Created an array of %d.Sizes: natural %ld universal %ld.",Field.Capacity,wNV,wNV);
  setComment(wComment.toCChar());

  return;
}

void
ZFieldDLg::ComputeClicked()
{
  refresh();
  Field.computeMd5();
  ui->HashcodeLEd->setText(Field.Hash.toHexa().toChar());
  ui->HashIconLBl->setScaledContents(true);
  ui->HashIconLBl->setPixmap(HashOKPXm);
}

bool
ZFieldDLg::HashCodeCompare()
{
  refresh();
  if (Field.checkHashcode())
    {
      ui->HashIconLBl->setPixmap(HashOKPXm);
      return true;
    }

  ui->HashIconLBl->setPixmap(HashWrongPXm);
  return false;
}//HashCodeCompare

void ZFieldDLg::setup(ZFieldDescription &pField, bool pRawFields)
{
  Field=pField;
  utf8String wStr;

  RawField=pRawFields;

  ui->FieldNameLEd->setText(Field.getName().toCChar());

  wStr.sprintf("0x%08X",Field.ZType);
  ui->ZTypeLEd->setText(wStr.toCChar());

  TypeListBtn->ZType = Field.ZType;

  ui->ZTypeStrLEd->setText(decode_ZType(Field.ZType));
  ui->ZTypeStrLEd->setReadOnly(true);

  ui->KeyEligibleCHk->setChecked(decode_ZType(Field.KeyEligible));

  wStr.sprintf("%ld",Field.NaturalSize);
  ui->NaturalLEd->setText(wStr.toCChar());
//  ui->NaturalLEd->setEnabled(false);
  ui->NaturalLEd->setEnabled(true);

  wStr.sprintf("%ld",Field.UniversalSize);
  ui->UniversalLEd->setText(wStr.toCChar());
//  ui->UniversalLEd->setEnabled(false);
  ui->UniversalLEd->setEnabled(true);

  wStr.sprintf("%ld",Field.HeaderSize);
  ui->HeaderSizeLEd->setText(wStr.toCChar());
  ui->HeaderSizeLEd->setReadOnly(true);

  wStr.sprintf("%ld",Field.Capacity);
  ui->CapacityLEd->setText(wStr.toCChar());

  bool wEnableSizeFRm=!RawField;
#ifdef __COMMENT__
  /* for all fixed string */

  switch (Field.ZType) {
    case ZType_CharFixedString  :
    case ZType_Utf8FixedString  :
    case ZType_Utf16FixedString  :
    case ZType_Utf32FixedString  :
      wEnableSizeFRm=true;
      break;

    default: /* for arrays */
      if ((Field.ZType & ZType_Array)==ZType_Array) {
          wEnableSizeFRm=true;
          break;
      }

  }

  ui->SizeFRm->setEnabled(wEnableSizeFRm);

//  ui->SizeFRm->setEnabled(!RawField);
#endif // __COMMENT__
  ui->SizeFRm->setEnabled(true);

  ui->HashcodeFRm->setVisible(!RawField);

  ui->HashcodeLEd->setText(pField.Hash.toHexa().toChar());
  ui->HashcodeLEd->setReadOnly(true);

  ui->FieldNameLEd->setText(pField.getName().toCChar());
  ui->FieldNameLEd->setSelection(0,Field.getName().UnitCount);

  if (pField.ToolTip.isEmpty())
    ui->TooltipLEd->setText("");
  else
    ui->TooltipLEd->setText(pField.ToolTip.toCChar());

}//ZFieldDLg::setup

void ZFieldDLg::setCreate( )
{
  Field.clear();
  Field.setFieldName(__NEW_FIELDNAME__);

  utf8VaryingString wStr;

  ui->FieldNameLEd->setText(Field.getName().toCChar());

  TypeListBtn->ZType = Field.ZType;

  wStr.sprintf("0x%08X",Field.ZType);
  ui->ZTypeLEd->setText(wStr.toCChar());

  ui->ZTypeStrLEd->setText(decode_ZType(Field.ZType));
  ui->ZTypeStrLEd->setReadOnly(true);

  ui->KeyEligibleCHk->setChecked(decode_ZType(Field.KeyEligible));

  wStr.sprintf("%ld",Field.NaturalSize);
  ui->NaturalLEd->setText(wStr.toCChar());

  wStr.sprintf("%ld",Field.UniversalSize);
  ui->UniversalLEd->setText(wStr.toCChar());

  wStr.sprintf("%ld",Field.HeaderSize);
  ui->HeaderSizeLEd->setText(wStr.toCChar());
  ui->HeaderSizeLEd->setReadOnly(true);

  wStr.sprintf("%ld",Field.Capacity);
  ui->CapacityLEd->setText(wStr.toCChar());

  ui->HashcodeLEd->setText(Field.Hash.toHexa().toChar());
  ui->HashcodeLEd->setReadOnly(true);

  ui->FieldNameLEd->setText(Field.getName().toCChar());
  ui->FieldNameLEd->setSelection(0,Field.getName().UnitCount);

  if (Field.ToolTip.isEmpty())
    ui->TooltipLEd->setText("");
  else
    ui->TooltipLEd->setText(Field.ToolTip.toCChar());
}//ZFieldDLg::create

ZFieldDescription ZFieldDLg::getFieldDescription()
{
  Field.computeMd5();
  return Field;
}

void ZFieldDLg::refresh()
{
  utf8String wStr;

  Field.setFieldName(ui->FieldNameLEd->text().toUtf8().data());

  Field.ZType = TypeListBtn->ZType;

  Field.KeyEligible = ui->KeyEligibleCHk->isChecked();

  wStr=ui->CapacityLEd->text().toUtf8().data();
  Field.Capacity = (URF_Capacity_type)wStr.toInt();

  wStr=ui->NaturalLEd->text().toUtf8().data();
  Field.NaturalSize = (uint64_t)wStr.toULong();

  wStr=ui->UniversalLEd->text().toUtf8().data();
  Field.UniversalSize = (uint64_t)wStr.toULong();

  wStr=ui->UniversalLEd->text().toUtf8().data();
  Field.UniversalSize = (uint64_t)wStr.toULong();

  Field.ToolTip = ui->TooltipLEd->text().toUtf8().data();

 // Field.Hash.fromHexa(ui->HashcodeLEd->text().toUtf8().data());
}

void
ZFieldDLg::setComment(const utf8VaryingString& pComment) {
  ui->CommentLBl->setText(pComment.toCChar());
}


bool ZFieldDLg::controlField()
{
  if (Field.getName()==__NEW_FIELDNAME__)
  {
    ZExceptionDLg::message("ZFieldDLg::controlField",ZS_INVNAME,Severity_Error,
        "Field name <%s> is invalid. Please enter a valid field name.",Field.getName().toCChar());
    return false;
  }
  if (Field.ZType==ZType_Unknown)
  {
    ZExceptionDLg::message("DicEdit::controlField",ZS_INVTYPE,Severity_Error,
        "Field type 0x%08X <%s> is invalid. Please enter a valid field type.",Field.ZType,decode_ZType(Field.ZType));
    return false;
  }
  if (Field.HeaderSize!=_getURFHeaderSize(Field.ZType))
    {
    int wRet=ZExceptionDLg::message2B("ZFieldDLg::controlField",ZS_INVVALUE,Severity_Warning, "Force","Modify",
        "Field type 0x%08X <%s> has a specified header size of <%ld> while you entered <%ld>. Please confirm.",
        Field.ZType,decode_ZType(Field.ZType),
        _getURFHeaderSize(Field.ZType),Field.HeaderSize);
    if (wRet==QDialog::Rejected)
      return false;
    return true;
    }
  if (HashCodeCompare())
    return true;
  ZExceptionDLg::message("ZFieldDLg::controlField",ZS_INVVALUE,Severity_Error,
      "Hashcode control does not match between computed hashcode and current hashcode.\n"
      "May be you changed some data into this field (even slight thing like a space) that makes it different.");
  return false;
} //controlField
