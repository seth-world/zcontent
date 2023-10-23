#include "zchangerecord.h"

ZChangeRecord::ZChangeRecord()
{
  Pointers.VoidPtr.Ante=nullptr;
  Pointers.VoidPtr.Post=nullptr;
}

ZChangeRecord::ZChangeRecord(ZFGenChange pChgCode) {
  Pointers.VoidPtr.Ante=nullptr;
  Pointers.VoidPtr.Post=nullptr;
  setChangeCode(pChgCode);
}

void
ZChangeRecord::setChangeCode(ZFGenChange pChgCode) {
  ChangeCode = pChgCode;
  switch (ChangeCode) {
  case ZFGC_KeyRawAppend:
  case ZFGC_KeyDicAppend:
    ZType = ZType_Class;
    return;
  case ZFGC_KeyDelete:
    ZType = ZType_Class;
    return;
  case ZFGC_INameChange:
    ZType = ZType_Utf8VaryingString;
    return;
  case ZFGC_ChgAlloc:
  case ZFGC_ChgExtent:
  case ZFGC_ChgKeySize:
    ZType = ZType_U64;
    return;
  case ZFGC_ChgDuplicate:
  case ZFGC_ChgGrab:
  case ZFGC_ChgHigh:
    ZType = ZType_Bool;
    return;
  default:
    fprintf(stderr,"ZChangeRecord-F-INVCODE Invalid Change code\n");
    abort();
  }
}

ZChangeRecord& ZChangeRecord::_copyFrom(const ZChangeRecord& pIn){
  clearPointers();
  ChangeKey=pIn.ChangeKey;
  ChangeCode = pIn.ChangeCode;
  ZType = pIn.ZType;
  switch (ZType){
  case ZType_Utf8VaryingString:
    if (pIn.Pointers.StringPtr.Ante!=nullptr) {
      Pointers.StringPtr.Ante = new utf8String(*pIn.Pointers.StringPtr.Ante);
    }

    if (pIn.Pointers.StringPtr.Post!=nullptr) {
      Pointers.StringPtr.Post = new utf8String(*pIn.Pointers.StringPtr.Post);
    }

    return *this;
  case ZType_Class:
    if (pIn.Pointers.KeyDataPtr.Ante!=nullptr) {
      Pointers.KeyDataPtr.Ante = new KeyData(*pIn.Pointers.KeyDataPtr.Ante);
    }
    if (pIn.Pointers.KeyDataPtr.Post!=nullptr) {
      Pointers.KeyDataPtr.Post = new KeyData(*pIn.Pointers.KeyDataPtr.Post);
    }
    return *this;
  case ZType_U64:
    if (pIn.Pointers.SizePtr.Ante!=nullptr) {
      Pointers.SizePtr.Ante = new size_t(*pIn.Pointers.SizePtr.Ante);
    }
    if (pIn.Pointers.SizePtr.Post!=nullptr) {
      Pointers.SizePtr.Post = new size_t(*pIn.Pointers.SizePtr.Post);
    }
    return *this;
  case ZType_Bool:
    if (pIn.Pointers.SizePtr.Ante!=nullptr) {
      Pointers.BoolPtr.Ante = new bool(*pIn.Pointers.BoolPtr.Ante);
    }
    if (pIn.Pointers.SizePtr.Post!=nullptr) {
      Pointers.BoolPtr.Post = new bool(*pIn.Pointers.BoolPtr.Post);
    }
    return *this;
  default:
    break;
  }
  fprintf (stderr,"ZChangeRecord::_copyFrom-F-INVTYP ChangeCode has an invalid ZType value.\n");
  abort();
} // _copyFrom


void ZChangeRecord::clearPointers() {
  switch (ZType){
  case ZType_Utf8VaryingString:
    if (Pointers.StringPtr.Ante!=nullptr) {
      delete Pointers.StringPtr.Ante;
      Pointers.StringPtr.Ante = nullptr;
    }
    if (Pointers.StringPtr.Post!=nullptr) {
      delete Pointers.StringPtr.Post;
      Pointers.StringPtr.Post = nullptr;
    }
    break;
  case ZType_Class:
    if (Pointers.KeyDataPtr.Ante!=nullptr) {
      delete Pointers.KeyDataPtr.Ante;
      Pointers.KeyDataPtr.Ante = nullptr;
    }
    if (Pointers.KeyDataPtr.Post!=nullptr) {
      delete Pointers.KeyDataPtr.Post;
      Pointers.KeyDataPtr.Post = nullptr;
    }
    break;
  case ZType_U64:
  case ZType_Bool:
    zfree(Pointers.SizePtr.Ante);
    zfree(Pointers.SizePtr.Post);
    break;
  }
}

void ZChangeRecord::clearAnte() {
  switch (ZType){
  case ZType_Utf8VaryingString:
    if (Pointers.StringPtr.Ante!=nullptr) {
      delete Pointers.StringPtr.Ante;
      Pointers.StringPtr.Ante = nullptr;
    }
    break;
  case ZType_Class:
    if (Pointers.KeyDataPtr.Ante!=nullptr) {
      delete Pointers.KeyDataPtr.Ante;
      Pointers.KeyDataPtr.Ante = nullptr;
    }
    break;
  case ZType_U64:
  case ZType_Bool:
    zfree(Pointers.SizePtr.Ante);
    break;
  }
}

void ZChangeRecord::clearPost() {
  switch (ZType){
  case ZType_Utf8VaryingString:
    if (Pointers.StringPtr.Post!=nullptr) {
      delete Pointers.StringPtr.Post;
      Pointers.StringPtr.Post = nullptr;
    }
    break;
  case ZType_Class:
    if (Pointers.KeyDataPtr.Post!=nullptr) {
      delete Pointers.KeyDataPtr.Post;
      Pointers.KeyDataPtr.Post = nullptr;
    }
    break;
  case ZType_U64:
  case ZType_Bool:
    zfree(Pointers.SizePtr.Post);
    break;
  }
}

void ZChangeRecord::setAnteU64(size_t pAnte) {
  if (ZType != ZType_U64)
    abort();
  clearAnte();
  Pointers.SizePtr.Ante = new size_t(pAnte);
}
void ZChangeRecord::setPostU64(size_t pPost) {
  if (ZType != ZType_U64)
    abort();
  clearPost();
  Pointers.SizePtr.Post = new size_t(pPost);
}

void ZChangeRecord::setAnteBool(bool pAnte) {
  if (ZType != ZType_Bool)
    abort();
  clearAnte();
  Pointers.BoolPtr.Ante = new bool(pAnte);
}

void ZChangeRecord::setPostBool(bool pPost) {
  if (ZType != ZType_Bool)
    abort();
  clearPost();
  Pointers.BoolPtr.Post = new bool(pPost);
}

void ZChangeRecord::setAnteZSortType(ZSort_Type pAnte) {
  if (ZType != ZType_U8)
    abort();
  clearAnte();
  Pointers.SortTypePtr.Ante = new ZSort_Type(pAnte);
}

void ZChangeRecord::setPostZSortType(ZSort_Type pPost) {
  if (ZType != ZType_U8)
    abort();
  clearPost();
  Pointers.SortTypePtr.Post = new ZSort_Type(pPost);
}

void ZChangeRecord::setAnteString(const utf8VaryingString& pAnte) {
  if (ZType != ZType_Utf8VaryingString)
    abort();
  clearAnte();
  Pointers.StringPtr.Ante = new utf8VaryingString(pAnte);
}
void ZChangeRecord::setPostString(const utf8VaryingString& pPost) {
  if (ZType != ZType_Utf8VaryingString)
    abort();
  clearPost();
  Pointers.StringPtr.Post = new utf8VaryingString(pPost);
}

void ZChangeRecord::setAnteKeyData(const KeyData& pAnte) {
  if (ZType != ZType_Class)
    abort();
  clearAnte();
  Pointers.KeyDataPtr.Ante=new KeyData(pAnte);
}
void ZChangeRecord::setPostKeyData(const KeyData& pPost) {
  if (ZType != ZType_Class)
    abort();
  clearPost();
  Pointers.KeyDataPtr.Post=new KeyData(pPost);
}

utf8VaryingString
ZChangeRecord::getAnte_Str() const{
  utf8VaryingString wStr;

  if (Pointers.VoidPtr.Ante==nullptr)
    return "<nullptr>";

  switch (ZType){
  case ZType_Utf8VaryingString:
      return *Pointers.StringPtr.Ante;

  case ZType_Class:
    wStr.addsprintf("<%s> | keysize %ld | Dup %s| Alloc %ld | Extent %ld | Grab %s | High %s | Root ",
                      Pointers.KeyDataPtr.Ante->IndexName.toString(),
                      Pointers.KeyDataPtr.Ante->KeySize,
                      decode_ZST(Pointers.KeyDataPtr.Ante->Duplicates),
                      Pointers.KeyDataPtr.Ante->Allocated,
                      Pointers.KeyDataPtr.Ante->ExtentQuota,
                      Pointers.KeyDataPtr.Ante->GrabFreeSpace?"<true>":"<false>",
                      Pointers.KeyDataPtr.Ante->HighwaterMarking?"<true>":"<false>"
                      );
    wStr += Pointers.KeyDataPtr.Ante->IndexRootName.toString();
    return wStr;

  case ZType_U64:
    wStr.sprintf("%ld",size_t(*Pointers.SizePtr.Ante));
    return wStr;

  case ZType_Bool:
    wStr.sprintf("%s",bool(*Pointers.BoolPtr.Ante)?"true":"false");
    return wStr;

  default:
    return "Unknown ZType";
  }
} // getAnte_Str

utf8VaryingString
ZChangeRecord::getPost_Str() const{
  utf8VaryingString wStr;

  if (Pointers.VoidPtr.Post==nullptr)
    return "<nullptr>";

  switch (ZType){
  case ZType_Utf8VaryingString:
    return *Pointers.StringPtr.Post;

  case ZType_Class:
    wStr.addsprintf("<%s> | keysize %ld | Dup %s| Alloc %ld | Extent %ld | Grab %s | High %s | Root ",
                    Pointers.KeyDataPtr.Post->IndexName.toString(),
                    Pointers.KeyDataPtr.Post->KeySize,
                    decode_ZST(Pointers.KeyDataPtr.Post->Duplicates),
                    Pointers.KeyDataPtr.Post->Allocated,
                    Pointers.KeyDataPtr.Post->ExtentQuota,
                    Pointers.KeyDataPtr.Post->GrabFreeSpace?"<true>":"<false>",
                    Pointers.KeyDataPtr.Post->HighwaterMarking?"<true>":"<false>"
                    );
    wStr += Pointers.KeyDataPtr.Post->IndexRootName.toString();
    return wStr;

  case ZType_U64:
    wStr.sprintf("%ld",size_t(*Pointers.SizePtr.Post));
    return wStr;

  case ZType_Bool:
    wStr.sprintf("%s",bool(*Pointers.BoolPtr.Post)?"true":"false");
    return wStr;

  default:
    return "Unknown ZType";
  }
} // getPost_Str

utf8VaryingString
ZChangeRecord::getChangeCode_Str() const {
  return decode_ZFGC(ChangeCode);
}

utf8VaryingString
ZChangeRecord::getZType_Str() const {
  switch (ZType){
  case ZType_Utf8VaryingString:
    return "ZType_Utf8VaryingString";

  case ZType_Class:
    return "KeyData";


  case ZType_U64:
    return "ZType_U64";

  case ZType_Bool:
    return "ZType_Bool";

  default:
    return "Unmanaged ZType";
  }
}

const char* decode_ZFGC(ZFGenChange pCode) {

  switch (pCode) {
  case ZFGC_Nothing:
    return "ZFGC_Nothing";
  case ZFGC_KeyRawAppend:
    return "ZFGC_KeyRawAppend";
  case ZFGC_KeyDicAppend:
    return "ZFGC_KeyDicAppend";
  case ZFGC_KeyDelete:
    return "ZFGC_KeyDelete";
  case ZFGC_INameChange:
    return "ZFGC_INameChange";
  case ZFGC_ChgAlloc:
    return "ZFGC_ChgAlloc";
  case ZFGC_ChgExtent:
    return "ZFGC_ChgExtent";
  case ZFGC_ChgDuplicate:
    return "ZFGC_ChgDuplicate";
  case ZFGC_ChgGrab:
    return "ZFGC_ChgGrab";
  case ZFGC_ChgHigh:
    return "ZFGC_ChgHigh";
  case ZFGC_ChgKeySize:
    return "ZFGC_ChgKeySize";
  default:
    return "Unknown change code";

  }

}
