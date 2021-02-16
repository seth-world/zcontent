#ifndef ZFILECONTROLBLOCK_H
#define ZFILECONTROLBLOCK_H

#include <stdlib.h>
#include <stdint.h>
#include <ztoolset/zdatabuffer.h>
#include <zrandomfile/zrandomfiletypes.h>

#pragma pack(push)
#pragma pack(0)
class ZFileControlBlock_Export
{
public:
    uint32_t        StartSign=cst_ZSTART ;         /**< StartSign word that mark start of data */
    ZBlockID        BlockID;            /**< Block id is set to ZBID_FCB */
    zaddress_type   StartOfData;        /**< offset where Data storage starts : 0L */
    unsigned long   AllocatedBlocks;            /**< for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT */
    unsigned long   BlockExtentQuota;            /**< for ZBAT & ZFBT : initial extension quota */

    size_t          ZBAT_DataOffset;            /**< Written on file header : Offset to Blocks Access Table array since begining of ZFCB */
    size_t          ZBAT_ExportSize;            /**<  Written on file header : size in bytes of ZBAT : to be written on file. This size is the global ZArray size in bytes */

    size_t          ZFBT_DataOffset;           /**< offset to Free Blocks Table array since begining of ZFCB */
    size_t          ZFBT_ExportSize;           /**< size in bytes of ZFBT : to be written on file */

    size_t          ZDBT_DataOffset;           /**< offset to Deleted Blocks Table array since begining of ZFCB */
    size_t          ZDBT_ExportSize;           /**< size in bytes of ZDBT : to be written on file */

    size_t          ZReserved_DataOffset;      /**<  Written on file header : Reserved space address . Must be 0L */
    size_t          ZReserved_ExportSize;      /**<  given by _getReservedSize */

//    void             (*_getReserved) (ZDataBuffer &) ;// routine to load zreserved from derived class

    zsize_type    InitialSize;                  /**< Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk */
    zsize_type    AllocatedSize;              /**< Total current allocated size in bytes for file */
    zsize_type    UsedSize;                   /**< Total of currently used size within file in bytes */
//    zsize_type    ExtentSizeQuota;            // extent quota size in bytes for file : no more used

    size_t   MinSize;                    /**< statistical value : minimum length of block record in file  (existing statistic) */
    size_t   MaxSize;                    /**< statistical value : maximum length of block record in file (existing statistic ) */
    size_t   BlockTargetSize;           /**< Block target size (user defined value) Foreseen medium size of blocks in a varying block context. */

/*    bool            History;
    bool            Autocommit;
    bool            Journaling;*/  // ZRandomFile Does NOT have journaling, history, autocommit : see ZMasterFile instead

    uint8_t         HighwaterMarking;           /**< mark to zero the whole deleted block content when removed */
    uint8_t         GrabFreeSpace;              /**< attempts to grab free space and holes at each block free operation */
    uint32_t        EndSign=cst_ZEND;           /**< EndSign word that marks end of data */

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

/* not copied neither exported to xml */
    uint32_t       StartSign=cst_ZSTART ; /**< StartSign word that mark start of data */
    ZBlockID       BlockID;               /**< Block id is set to ZBID_FCB */
//private:
/* copied and exported to xml */
    zaddress_type  StartOfData;           /**< offset where Data storage starts : 0L */
public:
    unsigned long   AllocatedBlocks;            /**< for ZBAT & ZFBT : initial number of available allocated slots in ZBAT and ZFBT */
    unsigned long   BlockExtentQuota;           /**< for ZBAT & ZFBT : initial extension quota */

    size_t          ZBAT_DataOffset;            /**< Written on file header : Offset to Blocks Access Table array since begining of ZFCB */
    size_t          ZBAT_ExportSize;            /**<  Written on file header : size in bytes of ZBAT : to be written on file. This size is the global ZArray size in bytes */

    size_t          ZFBT_DataOffset;           /**< offset to Free Blocks Table array since begining of ZFCB */
    size_t          ZFBT_ExportSize;           /**< size in bytes of ZFBT : to be written on file */

    size_t          ZDBT_DataOffset;           /**< offset to Deleted Blocks Table array since begining of ZFCB */
    size_t          ZDBT_ExportSize;           /**< size in bytes of ZDBT : to be written on file */

    size_t          ZReserved_DataOffset;      /**<  Written on file header : Reserved space address . Must be 0L */
    size_t          ZReserved_ExportSize;      /**<  given by _getReservedSize */

//    void             (*_getReserved) (ZDataBuffer &) ;// routine to load zreserved from derived class

    zsize_type    InitialSize;                  /**< Initial Size allocated to file during creation : file is created to this size then truncated to size 0 to reserve allocation on disk */
    zsize_type    AllocatedSize;              /**< Total current allocated size in bytes for file */
    zsize_type    UsedSize;                   /**< Total of currently used size within file in bytes */
//    zsize_type    ExtentSizeQuota;            // extent quota size in bytes for file : no more used

    size_t   MinSize;                    /**< statistical value : minimum length of block record in file  (existing statistic) */
    size_t   MaxSize;                    /**< statistical value : maximum length of block record in file (existing statistic ) */
    size_t   BlockTargetSize;           /**< Block target size (user defined value) Foreseen medium size of blocks in a varying block context. */

/*    bool            History;
    bool            Autocommit;
    bool            Journaling;*/  // ZRandomFile Does NOT have journaling, history, autocommit : see ZMasterFile instead

    uint8_t         HighwaterMarking;           /**< mark to zero the whole deleted block content when removed */
    uint8_t         GrabFreeSpace;              /**< attempts to grab free space and holes at each block free operation */
/* not copied neither exported to xml */
    uint32_t        EndSign=cst_ZEND;           /**< EndSign word that marks end of data */

  ZFileControlBlock (void) ;
  ZFileControlBlock (const ZFileControlBlock& pIn) {_copyFrom(pIn);}
  ZFileControlBlock (const ZFileControlBlock&& pIn) {_copyFrom(pIn);}

  ZFileControlBlock& _copyFrom(const ZFileControlBlock& pIn);
  ZFileControlBlock& operator = (const ZFileControlBlock& pIn){_copyFrom(pIn);}

  void clear(void) ;

  utf8String toXml(int pLevel);
    /**
     * @brief fromXml loads header control block from its xml definition and return 0 when successfull.
     * When errors returns <>0 and pErrlog contains appropriate error messages.
     * pHeaderRootNode xml node root for the hcb, typically <headercontrolblock> tag. No validation is made on root node for its name value.
     */
  int fromXml(zxmlNode* pFCBRootNode, ZaiErrors* pErrorlog);

  ZDataBuffer& _export(ZDataBuffer& pZDBExport);
  ZFileControlBlock& _import(unsigned char* pZDBImport_Ptr);
};


#endif // ZFILECONTROLBLOCK_H
