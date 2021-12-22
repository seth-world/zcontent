#ifndef ZINDEXCONTROLBLOCK_H
#define ZINDEXCONTROLBLOCK_H


//#include <zindexedfile/zkeydictionary.h>  // no dic in index control block
//#include <zindexedfile/zrecord.h>
#include <zindexedfile/zsindextype.h>
#include <zindexedfile/zindexdata.h>

namespace zbs {

/**
 * @brief The ZIndexControlBlock class  Defines the operational data that describes one index.
 *
 * This infraData is stored in the ZSIndexFile Header AND in the ZSMasterFile index collection within its reserved Header block (ZMasterControlBlock)
 */
#pragma pack(push)
#pragma pack(1)

class ZSICBOwnData;
struct ZSICB_Export{
  ZSICB_Export() {}
  ZSICB_Export(const ZSICB_Export& pIn) { _copyFrom(pIn);}



  ZSICB_Export& _copyFrom(const ZSICB_Export& pIn);
  ZSICB_Export& set(const ZSICBOwnData *pIn);

  ZSICB_Export& operator = (const ZSICB_Export& pIn) {return  _copyFrom(pIn);}

  ZSICB_Export& _reverseConditional();

  uint32_t      StartSign = cst_ZBLOCKSTART ;    // ZICB block start marker
  ZBlockID      BlockId   = ZBID_ICB;            // must be ZBID_ICB
  uint32_t      ZMFVersion= __ZMF_VERSION__ ;    // Self explainatory

  uint32_t      ICBTotalSize=0;    // ICB total size when written in file header (ZReserved header section size) this field must preceed ICB export content
  int32_t       ZKDicOffset;       // offset to ZDictionary (taking varying sized Index Name into account) or -1 if no key dictionary

  uint32_t      KeyUniversalSize=0;   // total key size according universal format (in line with ZKDic when exists)

//  ZIFKeyType_type         KeyType;            //!< RFFU
//  uint8_t                 AutoRebuild=false;  //!< RFFU
  ZSort_Type    Duplicates;         //!< Index key is allowing duplicates (ZST_DUPLICATES) or is not allowing duplicates (ZST_NODUPLICATES)



  void clear();
};
#pragma pack(pop)

class ZSICBOwnData{
public:
  ZSICBOwnData& _copyFrom(const ZSICBOwnData& pIn) ;
  ZSICBOwnData() {}
  ZSICBOwnData(const ZSICBOwnData& pIn) {_copyFrom(pIn);}
  ZSICBOwnData(const ZSICBOwnData&& pIn) {_copyFrom(pIn);}

  ZSICBOwnData&  operator = (const ZSICBOwnData& pIn) {return _copyFrom(pIn);}

// the values below are only exported and not stored
//  uint32_t            ICBTotalSize;      //!< ICB (ZSICBOwnData_Export+ Name varying string length + ZKDic export size) size when written in file header (ZReserved header field)
//  int32_t             ZKDicOffset;       //!< offset to ZDictionary (taking varying sized Index Name into account)

//  ::ZIFKeyType_type       KeyType;          //!< RFFU
//  uint8_t                 AutoRebuild=false;  //!< RFFU

  ZSort_Type              Duplicates=ZST_NODUPLICATES;//!< Index key is allowing duplicates (ZST_DUPLICATES) or is not allowing duplicates (ZST_NODUPLICATES)
  uint32_t                KeyUniversalSize=0;         //!< total of key size with internal format (in line with ZKDic when exists)
  utf8String              IndexName;                  //!< Index user name
  long                    IndexRank=0;                //!< self explainatory
//  bool                    HasKeyDictionary=false;



  utf8String toXml(int pLevel);
  ZStatus fromXml(zxmlNode* pIndexRankNode,ZaiErrors* pErrorlog);
//  ZDataBuffer& _exportAppend(ZDataBuffer& pICBE);

  ZStatus _import(unsigned char *&pPtrIn);
  void clear()
  {
//    ICBTotalSize=0;
//    ZKDicOffset=0;
//    HasKeyDictionary=false;
    KeyUniversalSize=0;
//    AutoRebuild=false;
    Duplicates= ZST_NODUPLICATES;
    IndexName.clear();}
};
class ZMFDictionary;
class ZMetaDic;
class ZSIndexControlBlock : public ZSICBOwnData
{

public:
  ZSIndexControlBlock& _copyFrom(const ZSIndexControlBlock& pIn) ;
//  ZSIndexControlBlock(ZMetaDic *pMetaDic) ;
  ZSIndexControlBlock() {}

  ~ZSIndexControlBlock(void);
  ZSIndexControlBlock(const ZSIndexControlBlock& pIn):ZSICBOwnData(pIn) {_copyFrom(pIn);}
  ZSIndexControlBlock(const ZSIndexControlBlock&& pIn):ZSICBOwnData(pIn) {_copyFrom(pIn);}
  ZSIndexControlBlock&  operator = (const ZSIndexControlBlock& pIn) {ZSICBOwnData::_copyFrom(pIn); return _copyFrom(pIn);}

//  void setMasterDic (ZMFDictionary* pDic) {MasterDic=pDic;}
  ZSKeyDictionary         *KeyDic = nullptr ; //< Fields dictionary for the key :                                          // a pointer that is instantiated by new and destroyed when ZICB rank is removed
//  ZMetaDic*               MetaDic=nullptr;  // reference to meta dictionary for the indexed file
  /* no dictionary in ICB, in MCB */
//  ZMFDictionary*          MasterDic=nullptr;  // main dictionary if exists : managed at ZSMasterFile level
  checkSum*   CheckSum=nullptr;
  md5         MD5;
//  void clear(ZMetaDic *pMetaDic);
//  bool hasDictionary() {return ZKDic!=nullptr;}
//  bool hasDictionary() {return MasterDic!=nullptr;}

  void newKeyDic(ZSKeyDictionary *pZKDic, ZMetaDic *pMetaDic);


  ZDataBuffer&_exportAppend(ZDataBuffer &pICBContent) ;
  ZDataBuffer _export() ;
/*  no dictionary in icb
 *
  ZStatus     _importICB (ZMFDictionary* pMasterDic,ZDataBuffer &pRawICB, size_t &pImportedSize, size_t pOffset=0) ;
  ZStatus     _import (unsigned char *& pPtrIn, ZMFDictionary *pMetaDic=nullptr);
*/
  ZStatus     _import (unsigned char *& pPtrIn);

 // ZStatus     zkeyValueExtraction (ZRecord *pRecord, ZDataBuffer& pKey);

  // generates checksum for Index Control block on ZSMasterFile side

  checkSum* generateCheckSum(void) { CheckSum = _export().newcheckSum(); return CheckSum;}

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
