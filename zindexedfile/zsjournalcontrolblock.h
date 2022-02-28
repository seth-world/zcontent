#ifndef ZSJOURNALCONTROLBLOCK_H
#define ZSJOURNALCONTROLBLOCK_H


#include <stdint.h>
#include <zindexedfile/zmfconfig.h>
#include <zindexedfile/zmf_limits.h>
//#include <zrandomfile/zrandomfiletypes.h>

#include <zindexedfile/zmasterfiletype.h>

namespace zbs {
#ifndef ZREMOTEMIRRORING
#define ZREMOTEMIRRORING
class ZRemoteMirroring
{
public:
  int8_t              Protocol;
  ZHostAddressType    AddressType=ZHAT_IPV4;
  utf8String          Host;
  int                 Port;
  utf8String          Authenticate;
  /* key and vector are for application data encoding using OpenSSL over a standard non encoded connection
   * and not using a SSL connection */
  ZCryptKeyAES256*    SSLKey=nullptr ;
  ZCryptVectorAES256* SSLVector=nullptr;

  utf8String toXml(int pLevel);


  /* SSL Encryption rules :
     * ----------------------
     * if there is an SSL key, there must be an SSL vector  -> xml encryption defined
     *    (if one of these exists while the other miss : xml corrupted)
     * if there is neither SSL key nor SSL vector -> xml encryption not defined
     * if xml has encryption defined :
     *    -> SSL key and vector do not exist : create them and load with xml key and vector -> encryption is enabled
     *    -> SSL key and vector exist : replace them with xml key and vector -> encryption stays enabled
     *
     * if xml encryption not defined :
     *    -> SSL key and vector do not exist : leave as it is -> encryption stays disabled
     *    -> SSL key and vector exist : DESTROY THEM -> encryption is disabled
     */
  ZStatus fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog, ZaiE_Severity pSeverity);
};

#endif //ZREMOTEMIRRORING


#pragma pack(push)
#pragma pack(1)
class ZSJCBOwnData;
class ZSJCBOwnData_Export
{
public:
  ZSJCBOwnData_Export() {}
  ZSJCBOwnData_Export(const ZSJCBOwnData_Export& pIn) { _copyFrom(pIn);}

  ZSJCBOwnData_Export& _copyFrom(const ZSJCBOwnData_Export& pIn);
  ZSJCBOwnData_Export& set(const ZSJCBOwnData& pIn);

  ZSJCBOwnData& _toJCBOwnData(ZSJCBOwnData &pOut);

  void _convert();

  void serialize();
  void deserialize();

  bool isReversed() {if (EndianCheck==cst_EndianCheck_Reversed) return true; return false;}
  bool isNotReversed() {if (EndianCheck==cst_EndianCheck_Normal) return true; return false;}


  uint32_t                StartSign   = cst_ZBLOCKSTART ;
  ZBlockID                BlockId     = ZBID_JCB;
  uint16_t                EndianCheck=cst_EndianCheck_Normal;
  uint32_t                ZMFVersion  = __ZMF_VERSION__;
  uint32_t                JCBSize;                    //! size of JCBOwndata including exported JournalLocalDirectoryPath
  uint8_t                 JournalingOn=false;         //!< Journaling is started (true) or not (false)
  uint8_t                 Keep=false;                 // uint8_t used as bool
  uint32_t                Depth=-1;
//  uriString               JournalLocalDirectoryPath;
};

#pragma pack(pop)


/**
 * @brief The ZSJournalControlBlock class this object is part of file header and is present (not nullptr) when ZMasterFile has journaling option on.
 *
 */


class ZSJCBOwnData
{
public:
//  uint32_t                JCBSize;
  uint8_t                 JournalingOn=false;         //!< Journaling is started (true) or not (false)
  uint8_t                 Keep=false;// uint8_t used as bool
  int                     Depth=-1;
  uriString               JournalLocalDirectoryPath;  //!< Directory path for journal file. If empty, then directory path of main content file is taken



  ZDataBuffer& _exportAppend(ZDataBuffer& pZDBExport);
  ZDataBuffer _export();

  ZSJCBOwnData& _import(const unsigned char *&pPtrIn);

};
class ZSJournal;
class ZRemoteMirroring;
class ZSJournalControlBlock : public ZSJCBOwnData
{
public:
  friend class ZSJournal;
  ZSJournalControlBlock ();
  ~ZSJournalControlBlock(void) ;

  ZSJournalControlBlock (const ZSJournalControlBlock& pIn) {_copyFrom(pIn);}

  ZSJournalControlBlock& _copyFrom(const ZSJournalControlBlock& pIn);

  void clear(void);


  ZDataBuffer& _exportAppend(ZDataBuffer& pZDB);

  ZDataBuffer& _exportJCB(ZDataBuffer &pJCBContent);
  size_t _getExportSize();

  ZStatus _import (const unsigned char *&pPtrIn);

  bool _isSameAs(const ZSJournalControlBlock* pJCB);

  void report(FILE *pOutput=stdout);

  void setParameters (uriString &pJournalPath);
  void setRemoteMirroring (int8_t pProtocol,
                          char* pHost,
                          int pPort,
                          char* pAuthenticate,
                          ZCryptKeyAES256 *pSSLKey,
                          ZCryptVectorAES256 *pSSLVector);


  utf8String toXml(int pLevel, bool pComment=true);
  ZStatus fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

  ZStatus purge(const zrank_type pKeepRanks=-1);
  ZSJournal             *Journal=nullptr;
  ZRemoteMirroring      *Remote=nullptr;
} ;

} // namespace zbs

#endif // ZSJOURNALCONTROLBLOCK_H
