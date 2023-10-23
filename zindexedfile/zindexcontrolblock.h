#ifndef ZINDEXCONTROLBLOCK_H
#define ZINDEXCONTROLBLOCK_H


//#include <zindexedfile/zkeydictionary.h>  // no dic in index control block
//#include <zindexedfile/zrecord.h>
#include <zindexedfile/zindextype.h>
#include <zindexedfile/zindexdata.h>

namespace zbs {

/**
 * @brief The ZIndexControlBlock class  Defines the operational data that describes one index.
 *
 * This infraData is stored in the ZIndexFile Header AND in the ZMasterFile index collection within its reserved Header block (ZMasterControlBlock)
 */
#pragma pack(push)
#pragma pack(1)

class ZICBOwnData;
class ZICB_Export{
public:
  ZICB_Export() {}
  ZICB_Export(const ZICB_Export& pIn) { _copyFrom(pIn);}

  ZICB_Export& _copyFrom(const ZICB_Export& pIn);
  ZICB_Export& set(const ZICBOwnData *pIn);

  ZICB_Export& operator = (const ZICB_Export& pIn) {return  _copyFrom(pIn);}

  bool isValid() const {
    return (StartSign==cst_ZBLOCKSTART) && (BlockId==ZBID_ICB) && (EndianCheck==cst_EndianCheck_Normal);
  }

  uint32_t      StartSign = cst_ZBLOCKSTART ;    // ZICB block start marker
  ZBlockId      BlockId   = ZBID_ICB;            // must be ZBID_ICB
  uint16_t      EndianCheck = cst_EndianCheck_Normal;
  unsigned long ZMFVersion= __ZMF_VERSION__ ;    // Self explainatory

  uint32_t      ICBTotalSize=0;    // ICB total size when written in file header (ZReserved header section size) this field must preceed ICB export content
  int32_t       ZKDicOffset;       // offset to ZDictionary (taking varying sized Index Name into account) or -1 if no key dictionary

  uint32_t      keyguessedsize=0;   // total key size according universal format (in line with ZKDic when exists)

//  ZIFKeyType_type         KeyType;            //!< RFFU
//  uint8_t                 AutoRebuild=false;  //!< RFFU
  ZSort_Type    Duplicates;         //!< Index key is allowing duplicates (ZST_Duplicates) or is not allowing duplicates (ZST_NODUPLICATES)

  ZICB_Export& setFromPtr(const unsigned char* &pPtrIn)
  {
    memmove (this,pPtrIn,sizeof(ZICB_Export));
    pPtrIn += sizeof(ZICB_Export);
    return *this;
  }

  void _convert();
  void serialize();
  void deserialize();

  bool isReversed() const  {if (EndianCheck==cst_EndianCheck_Reversed) return true; return false;}
  bool isNotReversed() const {if (EndianCheck==cst_EndianCheck_Normal) return true; return false;}

  void clear();
};
#pragma pack(pop)

class ZICBOwnData{
public:
  ZICBOwnData& _copyFrom(const ZICBOwnData& pIn) ;
  ZICBOwnData() {}
  ZICBOwnData(const ZICBOwnData& pIn) {_copyFrom(pIn);}
  ZICBOwnData(const ZICBOwnData&& pIn) {_copyFrom(pIn);}

  ZICBOwnData&  operator = (const ZICBOwnData& pIn) {return _copyFrom(pIn);}

// the values below are only exported and not stored
//  uint32_t            ICBTotalSize;      //!< ICB (ZSICBOwnData_Export+ Name varying string length + ZKDic export size) size when written in file header (ZReserved header field)
//  int32_t             ZKDicOffset;       //!< offset to ZDictionary (taking varying sized Index Name into account)

//  ::ZIFKeyType_type       KeyType;          //!< RFFU
//  uint8_t                 AutoRebuild=false;  //!< RFFU

  ZSort_Type              Duplicates=ZST_NoDuplicates;// Index key is allowing duplicates (ZST_Duplicates) or is not allowing duplicates (ZST_NODUPLICATES)
  uint32_t                KeyGuessedSize=0;         // total of key size with internal format (in line with ZKDic when exists)
  utf8VaryingString       IndexName;                  // Index user name
  uriString               URIIndex;                   // Index file name
  long                    IndexRank=0;                // self explainatory


  uriString getURIIndex() {return URIIndex;}

  utf8String toXml(int pLevel);
  ZStatus fromXml(zxmlNode* pIndexRankNode,ZaiErrors* pErrorlog);
//  ZDataBuffer& _exportAppend(ZDataBuffer& pICBE);

  ZStatus _import(const unsigned char *&pPtrIn);
  void clear()
  {
//    ICBTotalSize=0;
//    ZKDicOffset=0;
//    HasKeyDictionary=false;
    KeyGuessedSize=0;
//    AutoRebuild=false;
    Duplicates= ZST_NoDuplicates;
    IndexName.clear();}
};
//class ZMFDictionary;
//class ZMetaDic;

 /* ZIndexControlBlock is used to managed raw index.
  * It has no key dictionary definition : key dictionary is stored with ZMFDictionary when it exists. */

class ZIndexControlBlock : public ZICBOwnData
{

public:
  ZIndexControlBlock& _copyFrom(const ZIndexControlBlock& pIn) ;
//  ZSIndexControlBlock(ZMetaDic *pMetaDic) ;
  ZIndexControlBlock() {}

  ~ZIndexControlBlock(void);
  ZIndexControlBlock(const ZIndexControlBlock& pIn):ZICBOwnData(pIn) {_copyFrom(pIn);}
  ZIndexControlBlock(const ZIndexControlBlock&& pIn):ZICBOwnData(pIn) {_copyFrom(pIn);}
  ZIndexControlBlock&  operator = (const ZIndexControlBlock& pIn) {ZICBOwnData::_copyFrom(pIn); return _copyFrom(pIn);}

//  void setDictionary (ZMFDictionary* pDic) {Dictionary=pDic;}
  ZKeyDictionary         *IdxKeyDic = nullptr ; // Fields dictionary for the key :                                          // a pointer that is instantiated by new and destroyed when ZICB rank is removed
//  ZMetaDic*               MetaDic=nullptr;  // reference to meta dictionary for the indexed file
  /* no dictionary in ICB, in MCB */
//  ZMFDictionary*          Dictionary=nullptr;  // main dictionary if exists : managed at ZMasterFile level
//  checkSum*   CheckSum=nullptr;
//  md5         MD5;
//  void clear(ZMetaDic *pMetaDic);
//  bool hasDictionary() {return ZKDic!=nullptr;}
//  bool hasDictionary() {return Dictionary!=nullptr;}

//  void newKeyDic(ZSKeyDictionary *pZKDic, ZMetaDic *pMetaDic);

  ssize_t _exportAppend(ZDataBuffer &pICBContent) ;
  ZDataBuffer _export() ;
/*  no dictionary in icb
 *
  ZStatus     _importICB (ZMFDictionary* pDictionary,ZDataBuffer &pRawICB, size_t &pImportedSize, size_t pOffset=0) ;
  ZStatus     _import (unsigned char *& pPtrIn, ZMFDictionary *pMetaDic=nullptr);
*/
  ZStatus     _import (const unsigned char *&pPtrIn);

 // ZStatus     zkeyValueExtraction (ZRecord *pRecord, ZDataBuffer& pKey);

  // generates checksum for Index Control block on ZMasterFile side

//  checkSum* generateCheckSum(void) { CheckSum = _export().newcheckSum(); return CheckSum;}

  /**
    * @brief IndexRecordSize() computes and return the effective size of a ZIndex key record.
    * Variable fields length impose to compute record size at record level
    */
  ssize_t IndexRecordSize (void);


  utf8String toXml(int pLevel, bool pComment=true);
  /**
     * @brief fromXml loads index control block from its xml definition and return ZS_SUCCESS when successfull.
     * When errors returns <>0 and pErrlog contains appropriate error messages.
     * pIndexRankNode xml node root for the index, typically <indexrank> tag. No validation is made on root node for its name value.
     */
  ZStatus fromXml(zxmlNode* pIndexNode, ZaiErrors* pErrorlog);
};

}//namespace zbs

#endif // ZINDEXCONTROLBLOCK_H
