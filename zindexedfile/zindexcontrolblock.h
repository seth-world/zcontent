#ifndef ZINDEXCONTROLBLOCK_H
#define ZINDEXCONTROLBLOCK_H

#include <zindexedfile/zsindextype.h>

#include <zindexedfile/zkeydictionary.h>

namespace zbs {

/**
 * @brief The ZIndexControlBlock class  Defines the operational data that describes one index.
 *
 * This infraData is stored in the ZSIndexFile Header AND in the ZSMasterFile index collection within its reserved Header block (ZMasterControlBlock)
 */
#pragma pack(push)
#pragma pack(0)
struct ZSICBOwnData_Export{
  uint32_t                StartSign=cst_ZSTART ;         //!< ZICB block start marker
  ZBlockID                BlockID;            //!< must be ZBID_ICB
  unsigned long           ZMFVersion;         //!< Self explainatory
  size_t                  ICBTotalSize;       //!< ICB total size when written in file header (ZReserved header field size)
  size_t                  ZKDicOffset;        //!< Offset to ZKey dictionary
  ZIFKeyType_type         KeyType;            //!< RFFU
  uint8_t                 AutoRebuild=false;  //!< RFFU
  ZSort_Type              Duplicates;         //!< Index key is allowing duplicates (ZST_DUPLICATES) or is not allowing duplicates (ZST_NODUPLICATES)
  //    char*                    Name=nullptr;      //!< Name for the index
  //    uint32_t                EndSign ;        //!< ZICB block end marker --NO--
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

  //    uint32_t                StartSign=cst_ZSTART;//!< ZICB block start marker
  //    ZBlockID                BlockID;            //!< must be ZBID_ICB
  //    unsigned long           ZMFVersion;         //!< Self explainatory
  size_t                  ICBTotalSize;      //!< ICB (ZSICBOwnData_Export+ Name varying string length + ZKDic export size) size when written in file header (ZReserved header field)
  size_t                  ZKDicOffset;       //!< offset to ZDictionary (taking varying sized Index Name into account)

  ZIFKeyType_type         KeyType;            //!< RFFU
  uint8_t                 AutoRebuild=false;  //!< RFFU
  ZSort_Type              Duplicates;         //!< Index key is allowing duplicates (ZST_DUPLICATES) or is not allowing duplicates (ZST_NODUPLICATES)
  utf8String              Name;               //!< Name for the index
  //    uint32_t                EndSign=cst_ZEND ;  //!< ZICB block end marker

  utf8String toXml(int pLevel);
  int fromXml(zxmlNode* pIndexRankNode,ZaiErrors* pErrorlog);

  ZDataBuffer& _exportICBOwn(ZDataBuffer& pZDBExport);
  ZStatus _importICBOwn(unsigned char* pZDBImport_Ptr);
  void clear() {ICBTotalSize=0; ZKDicOffset=0;AutoRebuild=false; Duplicates= ZST_NODUPLICATES; Name.clear();}
};

class ZSIndexControlBlock : public ZSICBOwnData
{

public:
  ZSIndexControlBlock& _copyFrom(const ZSIndexControlBlock& pIn) ;
  ZSIndexControlBlock(ZMetaDic *pMetaDic) ;
  ~ZSIndexControlBlock(void);
  ZSIndexControlBlock(const ZSIndexControlBlock& pIn):ZSICBOwnData(pIn) {_copyFrom(pIn);}
  ZSIndexControlBlock(const ZSIndexControlBlock&& pIn):ZSICBOwnData(pIn) {_copyFrom(pIn);}
  ZSIndexControlBlock&  operator = (const ZSIndexControlBlock& pIn) {ZSICBOwnData::_copyFrom(pIn); return _copyFrom(pIn);}

  ZSKeyDictionary         *ZKDic = nullptr ;  //< Fields dictionary for the key :
      //for memory management reasons it is a pointer that is instantiated by new and destroyed when ZICB rank is removed
  checkSum*               CheckSum=nullptr;
  ZMetaDic*               MetaDic=nullptr;
  void clear(ZMetaDic *pMetaDic);

  ZDataBuffer&_exportICB (ZDataBuffer &pICBContent) ;
  ZStatus     _importICB (ZMetaDic* pMetaDic,ZDataBuffer &pRawICB, size_t &pImportedSize, size_t pOffset=0) ;
  ZStatus     _importICB (ZMetaDic* pMetaDic,unsigned char* pBuffer, size_t &pImportedSize);

  ZStatus     zkeyValueExtraction (ZRecord *pRecord, ZDataBuffer& pKey);

  // generates checksum for Index Control block on ZSMasterFile side

  void generateCheckSum(void) {ZDataBuffer ICBExport; CheckSum=_exportICB(ICBExport).newcheckSum(); return;}

  /**
    * @brief IndexRecordSize() computes and return the effective size of a ZIndex key record.
    * Variable fields length impose to compute record size at record level
    */
  ssize_t IndexRecordSize (void);


  utf8String toXml(int pLevel);
  /**
     * @brief fromXml loads index control block from its xml definition and return 0 when successfull.
     * When errors returns <>0 and pErrlog contains appropriate error messages.
     * pIndexRankNode xml node root for the index, typically <indexrank> tag. No validation is made on root node for its name value.
     */
  int fromXml(zxmlNode* pIndexRankNode,ZaiErrors* pErrorlog);
};

class ZSIndexControlTable : private ZArray<ZSIndexControlBlock*>

{
  typedef ZArray<ZSIndexControlBlock*> _Base ;
public:
  ZSIndexControlTable() {}
  ~ZSIndexControlTable() {}
  //    using _Base::push;  // push is overloaded
  using _Base::newBlankElement;
  using _Base::size;
  using _Base::last;
  using _Base::lastIdx;
  using _Base::operator [];
  //using ZIndexControlBlock::clear;

  long erase(const long pIdx) ;
  long push (ZSIndexControlBlock *pICB);
  long pop (void);
  void clear (void) ;

  long zsearchIndexByName (const char* pName);

  utf8String toXml(int pLevel);
  /**
     * @brief fromXml loads header control block from its xml definition and return 0 when successfull.
     * When errors returns <>0 and pErrlog contains appropriate error messages.
     * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
     */
  int fromXml(zxmlNode* pFDBRootNode, ZaiErrors* pErrorlog);

}; // ZIndexControlTable

}//namespace zbs

#endif // ZINDEXCONTROLBLOCK_H
