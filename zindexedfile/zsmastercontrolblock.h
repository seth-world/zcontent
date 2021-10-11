#ifndef ZSMASTERCONTROLBLOCK_H
#define ZSMASTERCONTROLBLOCK_H

#include <stdint.h>
#include <zindexedfile/zmfconfig.h>
#include <zindexedfile/zmf_limits.h>
#include <zrandomfile/zrandomfiletypes.h>

//#include <zindexedfile/zmetadic.h>
//#include <zindexedfile/zmfdictionary.h>
#include <zindexedfile/zsjournal.h>

#include <zindexedfile/zrawindexfile.h>
#include <zindexedfile/zindexdata.h>


namespace zbs {

/*
enum ZControlBlockComponents : uint8_t
{
  ZCBC_Nothing = 0,
  ZCBC_HasDictionary = 1,
  ZCBC_HasJournal = 2,

  ZCBC_Default = ZCBC_HasDictionary | ZCBC_HasJournal
};
*/
#pragma pack(push)
#pragma pack(0)

/* if MetaDic does not exist then MDicOffset is -1
*/
class ZSMCBOwnData;
class ZSMCBOwnData_Export{                         // will be the first block of data for ZSMCB
public:
  ZSMCBOwnData_Export() {}
  ZSMCBOwnData_Export(const ZSMCBOwnData_Export& pIn) { _copyFrom(pIn);}

  ZSMCBOwnData_Export& _copyFrom(const ZSMCBOwnData_Export& pIn);
  ZSMCBOwnData_Export& _copyFrom(const ZSMCBOwnData& pIn);

  ZSMCBOwnData& _toMCBOwnData(ZSMCBOwnData &pOut);


  ZSMCBOwnData_Export& operator = (const ZSMCBOwnData_Export& pIn) {return  _copyFrom(pIn);}
  ZSMCBOwnData_Export& operator = (const ZSMCBOwnData& pIn) {return  _copyFrom(pIn);}

  ZSMCBOwnData_Export& reverseConditional();

  uint32_t    StartSign=  cst_ZSTART ;
  ZBlockID    BlockID=    ZBID_MCB;
  uint32_t    ZMFVersion= __ZMF_VERSION__;

//  uint8_t     Components= ZCBC_Default;
  uint32_t    MCBSize;      // contains size of exported - imported  MCB
  //                                includes varying size of index path (varying string)
  uint32_t    MDicOffset;
  uint32_t    MDicSize;
  uint32_t    IndexCount;
  uint32_t    ICBOffset;
  uint32_t    ICBSize;
  uint32_t    JCBOffset;
  uint32_t    JCBSize;

  uint8_t     HistoryOn=false;

  // ===========IndexFilePath is exported as Universal value using uriString::_exportUniversal()=======
  //                     leading uint16_t size then string content WITHOUT '\0' ending char
};
#pragma pack(pop)


class ZSMCBOwnData{                         // will be the first block of data for ZSMCB
public:
  //    uint32_t                StartSign=cst_ZSTART ;
  //    ZBlockID                BlockID;
  //    unsigned long           ZMFVersion;
  uint32_t    MCBSize;      // contains size of exported - imported  MCB
  //                                includes varying size of index path (varying string)
  uint32_t    MDicOffset;     // MDic offset = 0 -> no dictionary
  int32_t     MDicSize;       // MDic size = 0 -> no dictionary
  uint32_t    IndexCount;
  uint32_t    ICBOffset;
  uint32_t    ICBSize;
  uint32_t    JCBOffset;      // JCBOffset = 0 -> no journalling
  int32_t     JCBSize;        // JCBSize = 0 -> no journalling
  uint8_t     HistoryOn=false; /* RFFU: if set, journaling events are historized see if no duplicate role with journaling keep option */
  //    uint8_t                    JournalingOn=false; //  will define wether update or load ZSJournalControlBlock from header while updating/reading ZMasterControlBlock
  uriString   IndexFilePath;  // Directory path for index files. If empty, then directory path of main content file is taken
  //    uriString               JournalPath;        // see ZJCB - Directory path for journal file. If empty, then directory path of main content file is taken
  //    uint32_t                EndSign=cst_ZEND ;
  bool hasDictionary() {return MDicOffset > 0;}
  bool hasJournal() {return JCBOffset > 0;}

  ZDataBuffer& _exportAppend(ZDataBuffer& pZDBExport);
  ZDataBuffer _export();
  /** _import imports from serialized data. Updates pZDBImport_Ptr to first byte after imported data */
  ZStatus _import(unsigned char *&pZDBImport_Ptr);

};

//-----------------Master control block-------------------------------------

/**
 * @brief The ZSMasterFileControlBlock class Master File Control Block contains all operational infradata necessary for a master file to operate.
 *
 *  ZMCB is stored within reserved block in Master File header.
 *
 *  As it is a permanent infradata, it is stored BEFORE ZFCB structure.
 *
 */
class ZSJournalControlBlock;
class ZSMasterControlBlock : public ZSMCBOwnData
{
public:
  friend class ZSJournal;

//  ZSIndexControlTable             Index;          // List of Indexes definition content
  ZIndexTable                     IndexTable;     // List of effective index objects that will manage physical ZSIndexFile
  ZSJournalControlBlock*          ZJCB=nullptr;   // journaling is defined here. If no journaling, stays to nullptr
  ZMFDictionary*                  MasterDic=nullptr;  // optional master dictionary (raw master file does not have one)

  ZSMasterControlBlock (void) ;
  ~ZSMasterControlBlock(void) ;

//  void pushICBtoIndexTable(ZSIndexControlBlock *pICB);
//  void removeICBfromIndexTable(const long pRank);

  long popIndex(void);

  void clear(void);

  utf8String toXml(int pLevel, bool pComment=true);
  /**
     * @brief fromXml loads master control block from its xml definition and return 0 when successfull.
     * When errors returns ZS_XMLERROR or appropriate status and pErrlog contains appropriate error messages.
     * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
     */
  ZStatus fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog);

  ZDataBuffer& _exportMCBAppend (ZDataBuffer &pMCBContent);
  ZDataBuffer _exportMCB ();

//  static ZStatus     _importMCB  (ZDataBuffer& pBuffer, ZSMCBOwnData& pMCBOwn, ZArray<ZSIndexControlBlock *> &pICBTable);

//  ZStatus     _importMCB  (ZDataBuffer& pBuffer);
//  ZStatus     _importMCB  (unsigned char *&wPtrIn);


  /**
   * @brief _import imports a full MCB to an existing Master control block. Eventually, adds indexes whenever required.
   *  When added, index file remains empty, only Index Control Block is feeded.
   *  IndexPresence returned array shows what indexes have been added.
   *  Pointer to input data pPtrIn is updated by this operation.
   */
  ZStatus _import(unsigned char *&pPtrIn, ZArray<ZPRES> &pIndexPresence);

  ZStatus _import(ZDataBuffer &pDataIn);

  void report(FILE *pOutput=stdout);
} ;


} // namespace zbs

#endif // ZSMASTERCONTROLBLOCK_H
