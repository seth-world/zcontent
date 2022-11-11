#ifndef ZMASTERCONTROLBLOCK_H
#define ZMASTERCONTROLBLOCK_H

#include <stdint.h>
#include <zindexedfile/zmfconfig.h>
#include <zindexedfile/zmf_limits.h>
#include <zrandomfile/zrandomfiletypes.h>

//#include <zindexedfile/zmetadic.h>
//#include <zindexedfile/zmfdictionary.h>
#include <zindexedfile/zjournal.h>

#include <zindexedfile/zrawindexfile.h>

#include <zindexedfile/zindexdata.h>

#include <zindexedfile/zdictionaryfile.h>

#include <zindexedfile/zsjournalcontrolblock.h>

#include <zcontent/zindexedfile/zindextable.h>

namespace zbs {
/*

  Header Reserved components for Master file  (Master Control Block export)

  Master Control Block    ZBID_MCB
    MCB Own Data : offsets to other "control blocks" and dictionary are here
    IndexFilePath (UVF format)

  Index Control Blocks ZBID_ICB
      ICB 0
      ICB 1
      ...
      ICB n
  Journal Control Block ZBID_JCB

  Master Dictionary   ZBID_MDIC
      Meta Dictionary
      Key Dictionaries
*/
#ifdef __DEPRECATED__

class ZSMCBOwnData;

class ZSMCBOwnData{                         // will be the first block of data for ZSMCB
public:

  uint8_t           HistoryOn=false; /* RFFU: if set, journaling events are historized see if no duplicate role with journaling keep option */
  uriString         IndexFilePath;  // Directory path for index files. If empty, then directory path of main content file is taken
  utf8VaryingString DictionaryName; /* name of the dictionary to search for within dictionary file. If empty, then first active dictionary is taken */
  uriString         DictionaryPath; /* Directory path for dictionary- If empty, then directory path of main content file is taken */

  ZDictionaryFile*  Dictionary=nullptr;

  ZIndexTable           IndexTable;     // List of effective index objects that will manage physical ZIndexFile
  ZJournalControlBlock* ZJCB=nullptr;   // journal facilities


  bool hasDictionary() {return Dictionary != nullptr;}


  ZDataBuffer& _exportAppend(ZDataBuffer& pZDBExport);
  ZDataBuffer _export();
  /** _import imports from serialized data. Updates pZDBImport_Ptr to first byte after imported data */
  ZStatus _import(const unsigned char *&pPtrIn);

};
#endif // __DEPRECATED__

//-----------------Master control block-------------------------------------

/**
 * @brief The ZMasterFileControlBlock class Master File Control Block contains all operational infradata necessary for a master file to operate.
 *
 *  ZMCB is stored within reserved block in Master File header.
 *
 *  As it is a permanent infradata, it is stored BEFORE ZFCB structure.
 *
 */
class ZJournalControlBlock;
class ZMFDictionary;

class ZMasterControlBlock // : public ZSMCBOwnData
{
public:
  friend class ZSJournal;

  ZMasterControlBlock (ZRawMasterFile* pMasterFile) ;
  ~ZMasterControlBlock(void) ;

  long popIndex(void);

  void clear(void);

  utf8String toXml(int pLevel, bool pComment=true);
  /**
     * @brief fromXml loads master control block from its xml definition and return 0 when successfull.
     * When errors returns ZS_XMLERROR or appropriate status and pErrlog contains appropriate error messages.
     * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
     */
  ZStatus fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog);

  size_t _exportAppend(ZDataBuffer &pMCBContent);
  /**
   * @brief _import imports a full MCB to an existing Master control block. Eventually, adds indexes whenever required.
   *  When added, index file remains empty, only Index Control Block is feeded.
   *  IndexPresence returned array shows what indexes have been added.
   *  Pointer to input data pPtrIn is updated by this operation.
   */
//  ZStatus _import(ZRawMasterFile *pMaster, const unsigned char *&pPtrIn, ZArray<ZPRES> &pIndexPresence);
  ZStatus _import( const unsigned char *&pPtrIn);

  void report(FILE *pOutput=stdout);


  uint8_t     HistoryOn=false; /* RFFU: if set, journaling events are historized see if no duplicate role with journaling keep option */
  uriString   IndexFilePath;  // Directory path for index files. If empty, then directory path of main content file is taken
  //    uriString               JournalPath;        // see ZJCB - Directory path for journal file. If empty, then directory path of main content file is taken
  //    uint32_t                EndSign=cst_ZEND ;
  utf8VaryingString DictionaryName; /* name of the dictionary to search for within dictionary file. If empty, then first active dictionary is taken */
  uriString         DictionaryPath; /* Directory path for dictionary- If empty, then directory path of main content file is taken */

  ZDictionaryFile*        Dictionary=nullptr;

  ZIndexTable             IndexTable;     // List of effective index objects that will manage physical ZIndexFile
  ZJournalControlBlock*   ZJCB=nullptr;


  ZRawMasterFile* RawMasterFile=nullptr;

  bool hasDictionary() {return Dictionary != nullptr;}

} ;



#pragma pack(push)
#pragma pack(1)
/* if MetaDic does not exist then MDicOffset is -1
*/
class ZMCB_Export  {                         // will be the first block of data for ZSMCB
public:
  ZMCB_Export() {}
  ZMCB_Export(const ZMCB_Export& pIn) { _copyFrom(pIn);}

  ZMCB_Export&  _copyFrom(const ZMCB_Export& pIn);
  size_t        _exportAppend(ZDataBuffer &pReturn);

//  ZSMCBOwnData& _toMCB(ZSMCBOwnData &pOut);

  bool isValid() {
    return (StartSign==cst_ZBLOCKSTART)
           && (BlockId==ZBID_MCB)
           && (EndianCheck==cst_EndianCheck_Normal);
  }
  ZMCB_Export& operator = (const ZMCB_Export& pIn) {return  _copyFrom(pIn);}

  //  ZSMCBOwnData_Export& reverseConditional();

  ZMCB_Export& setFromPtr(const unsigned char*& pPtrIn);

  ZMCB_Export& _import(const unsigned char*& pPtrIn);

  void _convert();
  void serialize();
  void deserialize();

  bool isReversed() const {if (EndianCheck==cst_EndianCheck_Reversed) return true; return false;}
  bool isNotReversed() const {if (EndianCheck==cst_EndianCheck_Normal) return true; return false;}

  uint32_t        StartSign=  cst_ZBLOCKSTART ;
  ZBlockId        BlockId=    ZBID_MCB;
  uint16_t        EndianCheck=cst_EndianCheck_Normal;
  unsigned long   ZMFVersion= __ZMF_VERSION__;

  //  uint8_t     Components= ZCBC_Default;
  uint32_t    MCBSize;      // contains size of exported - imported  MCB
  //                                includes varying size of index path (varying string)
//  uint32_t    IndexCount=0;
  uint32_t    ICBOffset=0;
  uint32_t    ICBSize=0;
  uint32_t    JCBOffset=0;
  uint32_t    JCBSize=0;
  //  uint32_t    MDicOffset;
  //  uint32_t    MDicSize;

  uint8_t     HistoryOn=false;

  // ===========IndexFilePath is exported as Universal value using uriString::_exportUniversal()=======
  //                     leading uint16_t size then string content WITHOUT '\0' ending char
};
#pragma pack(pop)


} // namespace zbs

#endif // ZSMASTERCONTROLBLOCK_H
