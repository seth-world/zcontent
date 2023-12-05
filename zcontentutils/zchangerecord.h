#ifndef ZCHANGERECORD_H
#define ZCHANGERECORD_H
#include <stddef.h>
#include <ztoolset/utfvaryingstring.h>
#include "keydata.h"


enum ZFGenChange : uint8_t {
  ZFGC_Nothing      = 0,
  ZFGC_KeyRawAppend = 1,      /* New raw key created */
  ZFGC_KeyDicAppend = 3,      /* Created key from dictionary */
  ZFGC_KeyDelete    = 4,      /* current key has been deleted */
  ZFGC_INameChange  = 0x10,   /* key name is changed */

  ZFGC_ChgAlloc     = 0x21,
  ZFGC_ChgExtent    = 0x22,
  ZFGC_ChgDuplicate = 0x23,
  ZFGC_ChgGrab      = 0x24,
  ZFGC_ChgHigh      = 0x25,
  ZFGC_ChgKeySize   = 0x26


  //  ZFGC_HasMaster    = 0x80    /* There is a loaded master file */
};


const char* decode_ZFGC(ZFGenChange pCode);


class ZChangeRecord {
public:
  ZChangeRecord() ;
  ZChangeRecord(ZFGenChange pChgCode) ;
  ZChangeRecord(const ZChangeRecord&pIn) {
    Pointers.VoidPtr.Ante=nullptr;
    Pointers.VoidPtr.Post=nullptr;
    _copyFrom(pIn);
  }
  ~ZChangeRecord() {
    clearPointers();
  } // ~ZChangeRecord

  void clearPointers() ;

  void clearAnte();
  void clearPost();

  void setChangeCode(ZFGenChange pChgCode) ;
  void setIndexRank(long pIdx){
    IndexRank = pIdx;
  }

  long getIndexRank() {
    return IndexRank;
  }

  ZChangeRecord& _copyFrom(const ZChangeRecord& pIn) ;

  ZChangeRecord& operator = (const ZChangeRecord& pIn) {return _copyFrom(pIn);}

  void setChangeKey(const utf8VaryingString& pChangeKey) {
    ChangeKey = pChangeKey;
  }

  void setAnteU64(size_t pAnte) ;
  void setPostU64(size_t pPost) ;

  void setAnteBool(bool pAnte) ;
  void setPostBool(bool pPost) ;

  void setAnteZSortType(ZSort_Type pAnte) ;
  void setPostZSortType(ZSort_Type pPost) ;

  void setAnteString(const utf8VaryingString& pAnte) ;
  void setPostString(const utf8VaryingString&  pPost) ;

  void setAnteKeyData(const KeyData& pAnte) ;
  void setPostKeyData(const KeyData&  pPost) ;

  size_t getAnteU64() const{
    return *Pointers.SizePtr.Ante;
  }
  bool getAnteBool() const{
    return *Pointers.BoolPtr.Ante;
  }
  ZSort_Type getAnteZSortType() const{
    return *Pointers.SortTypePtr.Ante;
  }
  utf8VaryingString getAnteString(){
    return *Pointers.StringPtr.Ante;
  }

  KeyData getAnteKeyData() const{
    return *Pointers.KeyDataPtr.Ante;
  }

  size_t getPostU64() const{
    return *Pointers.SizePtr.Post;
  }
  bool getPostBool() const {
    return *Pointers.BoolPtr.Post;
  }
  ZSort_Type getPostZSortType() const{
    return *Pointers.SortTypePtr.Ante;
  }
  utf8VaryingString getPostString() const{
    return *Pointers.StringPtr.Post;
  }

  KeyData getPostKeyData() const {
    return *Pointers.KeyDataPtr.Post;
  }

  ZFGenChange getChangeCode () const {return ChangeCode;}
  long  getIndexRank() const {return IndexRank;}

  utf8VaryingString getChangeKey () const {return ChangeKey;}

  utf8VaryingString getAnte_Str () const ;
  utf8VaryingString getPost_Str () const ;

  utf8VaryingString getChangeCode_Str() const;
  utf8VaryingString getZType_Str() const;


private:
  long        IndexRank =0;
  ZFGenChange ChangeCode=ZFGC_Nothing;
  ZType_type  ZType     =ZType_Nothing;
  utf8VaryingString ChangeKey;

  union {
    struct {
      size_t* Ante;
      size_t* Post;
    } SizePtr;
    struct {
      ZSort_Type* Ante;
      ZSort_Type* Post;
    } SortTypePtr;
    struct {
      bool* Ante;
      bool* Post;
    } BoolPtr;
    struct {
      utf8VaryingString* Ante;
      utf8VaryingString* Post;
    } StringPtr;
    struct {
      KeyData* Ante;
      KeyData* Post;
    } KeyDataPtr;
    struct {
      void* Ante;
      void* Post;
    } VoidPtr;
  } Pointers ;

};

#endif // ZCHANGERECORD_H
