#ifndef ZFILECONTROLBLOCK_H
#define ZFILECONTROLBLOCK_H

#include <stdlib.h>
#include <stdint.h>
#include <zindexedfile/zmf_limits.h>

#include <ztoolset/zdatabuffer.h>
#include <zrandomfile/zrandomfiletypes.h>
class ZFileControlBlock;
/**
 * @brief The FCBParams class holds File Control Block parameters that may be modified for a ZRandom File.
 */
class FCBParams
{
public:
  FCBParams()=default;
  FCBParams(const FCBParams& pIn) {_copyFrom(pIn);}
  FCBParams(const FCBParams&& pIn) {_copyFrom(pIn);}

  FCBParams& _copyFrom(const FCBParams& pIn)
  {
    AllocatedBlocks=pIn.AllocatedBlocks;
    InitialSize=pIn.InitialSize;

    BlockExtentQuota=pIn.BlockExtentQuota;
    BlockTargetSize=pIn.BlockTargetSize;
    HighwaterMarking=pIn.HighwaterMarking;
    GrabFreeSpace=pIn.HighwaterMarking;
    return *this;
  }

  FCBParams& operator = (const FCBParams& pIn) { return _copyFrom(pIn);}
  FCBParams& operator = (const FCBParams&& pIn) { return _copyFrom(pIn);}

  unsigned long   AllocatedBlocks=cst_ZRF_default_allocation;  /**< for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT */
  unsigned long   BlockExtentQuota=cst_ZRF_default_extentquota;/**< for ZBAT & ZFBT : initial extension quota */
  zsize_type      InitialSize=0;              /**< Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk */
  size_t          BlockTargetSize=0;          /**< Block target size (user defined value) Foreseen medium size of blocks in a varying block context. */

  bool            HighwaterMarking=false;   /**< mark to zero the whole deleted block content when removed */
  bool            GrabFreeSpace=false;      /**< attempts to grab free space and holes at each block free operation */
};

class ZFileControlBlock;
#pragma pack(push)
#pragma pack(1)
class ZFCB_Export
{
public:
    uint32_t        StartSign=cst_ZBLOCKSTART; /**< StartSign word that mark start of data */
    ZBlockID        BlockID=ZBID_FCB;     /**< Block id is set to ZBID_FCB */
    uint16_t        EndianCheck=cst_EndianCheck_Normal;

    zaddress_type   StartOfData;          /**< offset where Data storage starts : 0L */
    unsigned long   AllocatedBlocks;      /**< for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT */
    unsigned long   BlockExtentQuota;     /**< for ZBAT & ZFBT : initial extension quota */

    size_t          ZBAT_DataOffset;      /**< Written on file header : Offset to Blocks Access Table array since begining of ZFCB */
    size_t          ZBAT_ExportSize;      /**<  Written on file header : size in bytes of ZBAT : to be written on file. This size is the global ZArray size in bytes */

    size_t          ZFBT_DataOffset;           /**< offset to Free Blocks Table array since begining of ZFCB */
    size_t          ZFBT_ExportSize;           /**< size in bytes of ZFBT : to be written on file */

    size_t          ZDBT_DataOffset;           /**< offset to Deleted Blocks Table array since begining of ZFCB */
    size_t          ZDBT_ExportSize;           /**< size in bytes of ZDBT : to be written on file */

//    size_t          ZReserved_DataOffset;      /**<  Written on file header : Reserved space address . Must be 0L */
//    size_t          ZReserved_ExportSize;      /**<  given by _getReservedSize */

//    void             (*_getReserved) (ZDataBuffer &) ;// routine to load zreserved from derived class

    zsize_type    InitialSize;                /* Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk */
    zsize_type    AllocatedSize;              /* Total current allocated size in bytes for file */
    zsize_type    UsedSize;                   /* Total of currently used size within file in bytes */
//    zsize_type    ExtentSizeQuota;            // extent quota size in bytes for file : no more used

    size_t   MinSize;                    /* statistical value : minimum length of block record in file  (existing statistic) */
    size_t   MaxSize;                    /* statistical value : maximum length of block record in file (existing statistic ) */
    size_t   BlockTargetSize;           /* Block target size (user defined value) Foreseen medium size of blocks in a varying block context. */

    uint8_t         HighwaterMarking;           /* mark to zero the whole deleted block content when removed */
    uint8_t         GrabFreeSpace;              /* attempts to grab free space and holes at each block free operation */
    uint32_t        EndSign=cst_ZBLOCKEND;           /* EndSign word that marks end of data */

    ZFCB_Export()=default;
    ZFCB_Export(ZFCB_Export&pIn) {_copyFrom(pIn);}
    ZFCB_Export(ZFCB_Export&&pIn) {_copyFrom(pIn);}

    ZFCB_Export& _copyFrom(const ZFCB_Export& pIn);
    ZFCB_Export& operator = (const ZFCB_Export& pIn){return _copyFrom(pIn);}
    ZFCB_Export& operator = (const ZFileControlBlock& pIn);


    ZFCB_Export&        set(const ZFileControlBlock& pIn);
    void                setFromPtr(const unsigned char *&pPtrIn);
    ZFileControlBlock&  toFCB(ZFileControlBlock& pOut);

    void _convert();
    void serialize();
    void deserialize();

    bool isReversed() {if (EndianCheck==cst_EndianCheck_Reversed) return true; return false;}
    bool isNotReversed() {if (EndianCheck==cst_EndianCheck_Normal) return true; return false;}

};
#pragma pack(pop)

/**
 * @brief The ZFileControlBlock class contains all operational information of a ZRandomFile.

This block is written in header file.

It contains in particular the 3 block pools :
   - ZBlockAccessTable (ZBAT): block pool that gives for a used block its address within the file using its rank number
   - ZFreeBlockPool (ZFBT): block pool of free/deleted blocks available for usage in record creation process
   - ZDeletedBlockPool (ZDBT): pool of deleted block before they could have been grabbed by grab mechanism

@par ZFileDescriptor simplified map
@verbatim


        each pool size is calculated using ZArray<>::getExportSize() method

0L                                                                                              Expansion
+-------Variables---------------+-----------------------+--------------------------+-------------------------->

xxxx @ZBAT  @ZFBT xxxxxxxxxxxxxxx......ZBAT content......,,,,,,,,,,ZFBT content.....,,,,,,,,,,ZDBT content.....

@endverbatim

@par BlockTargetSize
    Foreseen medium size of blocks in a varying block context.
    This value is a important value used for allocation of free blocks as well as searches for physical block headers
    @see ZRandomFile::_getFreeBlock() ZRandomFile::_searchNextPhysicalBlock() ZRandomFile::_searchPreviousPhysicalBlock() ZRandomFile::_surfaceScan()

@par StartOfData
    Offset from the beginning of the content file where data blocks start to be stored.
    This value cannot be changed by user

@par AllocatedBlocks
    for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT

@par BlockExtentQuota
    for ZBAT & ZFBT : initial extension quota

@par InitialSize
    This is the initial space the file has been created with.
    This value is used when ZRandomFile::zclear() : file size is kept at this minimum.

@par AllocatedSize
    Total current allocated size in bytes for file.

@par UsedSize
    Total of currently used size within file in bytes (Meaning : sum of user record sizes for ZBlockAccessTable pool )

@par Statistical values

   - MinSize minimum length of block record in file  (existing statistic)
   - MaxSize maximum length of block record in file  (existing statistic)

    These values are not dynamic values. They are updated via different tools and are not systematically accurate.
    @see ZRandomFile::ZRFstat ZRandomFile::zreorgFile

@par HighwaterMarking (Option)
    When this option is set, freed blocks are marked to binary zero.

@par GrabFreeSpace (Option)
    When this option is set, ZRandomFile will try to gather adjacent free blocks and holes together when a _freeBlock is invoked.



*/
class ZFileControlBlock
{
friend class ZFileDescriptor;
friend class ZRandomFile;
friend class ZMasterFile;
friend class ZIndexFile;
public:


    zaddress_type  StartOfData=0L;               /**< offset where Data storage starts : 0L */
public:

    unsigned long   AllocatedBlocks=0;          /**< for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT */
    unsigned long   BlockExtentQuota=0;         /**< for ZBAT & ZFBT : initial extension quota */
    zsize_type      InitialSize=0;              /**< Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk */
    size_t          BlockTargetSize=0;          /**< Block target size (user defined value) Foreseen medium size of blocks in a varying block context. */

    uint8_t         HighwaterMarking=ZBool(false);         /**< mark to zero the whole deleted block content when removed */
    uint8_t         GrabFreeSpace=ZBool(false);            /**< attempts to grab free space and holes at each block free operation */


/* Following data is not exported to xml */
    size_t          ZBAT_DataOffset=0;            /**< Written on file header : Offset to Blocks Access Table array since begining of ZFCB */
    size_t          ZBAT_ExportSize=0;            /**<  Written on file header : size in bytes of ZBAT : to be written on file. This size is the global ZArray size in bytes */

    size_t          ZFBT_DataOffset;           /**< offset to Free Blocks Table array since begining of ZFCB */
    size_t          ZFBT_ExportSize;           /**< size in bytes of ZFBT : to be written on file */

    size_t          ZDBT_DataOffset;           /**< offset to Deleted Blocks Table array since begining of ZFCB */
    size_t          ZDBT_ExportSize;           /**< size in bytes of ZDBT : to be written on file */

    size_t          ZReserved_DataOffset;      /**<  Written on file header : Reserved space address . Must be 0L */
    size_t          ZReserved_ExportSize;      /**<  given by _getReservedSize */

/* End following data is not exported to xml */

//    void             (*_getReserved) (ZDataBuffer &) ;// routine to load zreserved from derived class

    zsize_type    AllocatedSize;          /**< Total current allocated size in bytes for file */
    zsize_type    UsedSize;               /**< Total of currently used size within file in bytes */
//    zsize_type    ExtentSizeQuota;            // extent quota size in bytes for file : no more used

    size_t   MinSize;                     /**< statistical value : minimum length of block record in file  (existing statistic) */
    size_t   MaxSize;                     /**< statistical value : maximum length of block record in file (existing statistic ) */

/*    bool            History;
    bool            Autocommit;
    bool            Journaling;*/  // ZRandomFile Does NOT have journaling, history, autocommit : see ZMasterFile instead

/* not copied neither exported to xml */
//   uint32_t        EndSign=cst_ZEND;           /**< EndSign word that marks end of data */

  ZFileControlBlock (void) ;
  ZFileControlBlock (const ZFileControlBlock& pIn) {_copyFrom(pIn);}
  ZFileControlBlock (const ZFileControlBlock&& pIn) {_copyFrom(pIn);}

  ZFileControlBlock& _copyFrom(const ZFileControlBlock& pIn);
  ZFileControlBlock& operator = (const ZFileControlBlock& pIn){_copyFrom(pIn);}

  void clear(void) ;

  FCBParams getUseableParams();
  void setUseableParams(const FCBParams& pIn);


  /* NB: if pComment is set to true, then xml code is commented, if not, no comment is generated */
  utf8String toXml(int pLevel, bool pComment=false);
    /**
     * @brief fromXml loads header control block from its xml definition and return 0 when successfull.
     * When errors returns <>0 and pErrlog contains appropriate error messages.
     * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
     */
  int fromXml(zxmlNode* pFCBRootNode, ZaiErrors* pErrorlog);

  ZDataBuffer& _exportAppend(ZDataBuffer& pZDBExport);
  ZDataBuffer _export();

  ZStatus _import(const unsigned char *&pZDBImport_Ptr);
}; // ZFileControlBlock


#endif // ZFILECONTROLBLOCK_H
