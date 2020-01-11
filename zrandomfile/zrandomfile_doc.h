#ifndef ZRANDOMFILE_DOC_H
#define ZRANDOMFILE_DOC_H

// this file owns the doxygen general documentation for ZRandomFile
namespace zbs {

/**
@page ZRandomFilePage   ZRandomFile Reference Documentation

@section ZRFMainZRandomFileGroup ZRandomFile : Random Access File for content storage
  @copydoc ZRandomFileGroup


@section ZRFMainArchitecture  ZRandomFile architecture

@subsection ZRFMainPhysical ZRandomFile Physical layer
    @copydoc ZRFPhysical

@subsection ZRFMainPools  ZRandomFile Blocks Pools
   @copydoc ZRFPools

@subsection ZRFMainHeader  ZRandomFile file header structure
    @copydoc ZRFHeader

@section ZRFMainFree Using adhoc fields with ZRandomFile
@copydoc ZRFFieldAccess

@section ZRFMainFreeSearch Searching in free sequential mode
@copydoc ZRFFreeSearch

*/

/**
@defgroup ZRandomFileGroup  ZRandomFile Random Access File for content storage

ZRandomFile (also further abbreviated ZRF) is the base tool for managing content with variable size on disk.

It allows a random access per logical relative rank to a user record content.
Logical relative rank is the user logical rank view and does NOT correspond to physical storage order :
    @see recordranks

ZRandomFile places the contents according its size to optimize its behavior, AND NOT necessarily according the record creation order.

It manages free space using specific algorithms that reallocate space whenever required and extend the file size only when required.

It shall reserve (allocate) a given disk space at creation time to avoid physical holes on disk.

ZRandomFile offers many optional features, left to the choice, in order to better manage ZRF behavior,
knowing there is a choice to make between instant write/delete access time and global performance.

@subsection ZRFGGfs grabFreeSpace

@copydoc ZRFGrabFreeSpace

@subsection ZRFGHwm highWaterMarking
Using highWaterMarking option parameter, it will zero any removed record content.
This option could be of interest to preserve some confidentiality for data.

@note When using this option, surface scan will no more show up deleted block headers.

@subsection ZRFGPerf Performance monitoring
ZRandomFile has multiple statistic, monitoring and dump tools that allows to have a vision on how content is stored in order to manage performance.
@see ZRFSTATS

@note Endianness : It is of user's responsibility to manage the content format, and especially the numeric (arithmetic) data format in case of Endianness.
@see EndianConversion .

@subsection ZRFGSurf File surface management

There is a set of tools for
 - analyzing the file surface :
    + how block records are stored
    + where are the holes and how many they are
 - reorganize the ZRandomFile in record ranks order
 - clone the file
 - extend the file
 - clear the file content
 - truncate

@section ZRFGUsing Using ZRandomFile

Despite the fact that ZRandomFile is a true relative file management system with variable content size, it allows to be used as an ZArray, but with variable content.
Data is handled using ZDataBuffer data structure.

ZRandomFile offers the possibility to use standard ZArray overloaded operators and methods emulated by ZRandomFile:

  - standard ZArray emulation
   + push
   + push_front
   + popRP

  - operators
   + zrf[id]  : returns the record of rank id
   + zrf << ZDataBuffer : equivalent to push


   @note
    move        : Method not allowed - only low level move may be made between two blocks (protected method)
    swap        : Method not allowed

The difference with ZArray is that all these operators return a reference read-only data.
This means that data cannot be modified in the file using these methods.

@section ZRFGMainMeth Main methods

   - zcreate : creates the file and its infra-data structures.
        @see @ref ZRandomFile::zcreate
   - zopen : opens the file.
   - zadd : adds a record logically positionned at the end of the record list.
          Physically the block containing the user's record may be anywhere on the file's surface according the better placement capability.

   - zinsert : insert a record at a given rank

   - zremove : logically deletes a record. block containing record is then placed in free block pool.
            if grabFreeSpace option is chosen then adjacent free blocks are agglomerated to make one unique free block and benefit from the whole space set.
   - zget     : reads a record from file according to its logical relative position and returns records content.
   - zgetnext : reads the next record

@section ZRFGSpecial Special methods

There is a set of other methods for :
   - Content dump and statistics
        @see ZRFSTATS
   - File surface utilities management
        @see  ZRFUtilities
   - Fields access
        + Free search using fieldsrecord
            @see ZRFFreeSearch
        + content change using field definition
            @see ZRFFieldAccess

   - zrecover : recovers a free pool block. It creates back a block from first freed block contained in free block and ungrab all deleted block agglomerated to it.
   these undeleted blocks become free blocks in free pool and can be themselves recovered.

  */  // ZRandomFileGroup


/**
 @addtogroup ZRandomFileGroup
 @{
  @defgroup ZRFPools ZRandomFile Blocks Pools

  ZRandomFile is organized by blocks.
  ZRandomFile manages 3 blocks pools, among which 2 are fundamental.
  These pools are maintained and located within ZFileControlBlock in the header file.

  @par ZBlockAccessTable (abbreviated ZBAT)
  This primary pool contains all references to active  blocks(State = ZBS_Used).
  The order of blocks within this pool defines the relative position of the access to blocks within ZRandomFile.
  @see recordranks ZBlockState_type

  @par ZFreeBlockPool (abbreviated ZFBT)
  This primary pool contains all references to freed (logically deleted) blocks within the file (State=ZBS_Free).
  So that, use of ZRandomFile::zremove will not physically suppress the block.
  The removed block is simply removed from ZBAT and added as a new entry within ZFBT.
  It is then available for being allocated to a newly created record thru ZRandomFile::_getFreeBlock routine.

  @par ZDeletedBlockTable (abbreviated ZDBT)
  This secondary pool is maintained by grabbing process and free block allocation process.
  when a free block is grabbed to be agglomerated to another free block in the grabbing algorithm, then its track is not lost (excepted when using option HighwaterMarking).
  This block is a set as a new entry to ZDBT.
  when the container block will be allocated, then all ZDBT reference to blocks included within the allocated block will be suppressed.

  @defgroup ZRFHighWaterMarking  HighWaterMarking for ZRandomFile surface

  HighWaterMarking is an option that consists in marking to binary zero any freed block (deleted record) in the file.
  This insure that deleted records are effectively and physically erased within the file.
  @warning Chosing this option implies additional accesses to file surface while deleting a record.
  @warning This option prohibit the possible recovery of deleted records @see @ref ZRandomFile::zrecover()


  @defgroup ZRFBlockTargetSize  BlockTargetSize : an average size of record length

  In a varying record length context, Record target siuze is helpfull for many file space allocation usage.
  In addition, for scanning the file surface, searching for block headers either previously the current block or after the current block,
  it will be an allocation unit for search operation.
  It is arbitrarily fixed at creation time, and it evolves during file life time.
  @warning It is important that this value is set at file creation time.

  @defgroup ZRFGrabFreeSpace  Grabbing free space blocks and holes to compact the file structure

  GrabFreeSpace is an on-flow process that consist in agglomerating free blocks before and free blocks after the current block being free.
  So that the deleted block grabs the available space around him.
  It reduces the eventuality :
  - to have holes between blocks
  - to have a huge number of atomized free blocks

  As grabFreeSpace will weight a little bit on resource consumption, it should be carefully used.

  grabFreeSpace will offer its best when :
  - using varying size record content
  - standard deviation of record size is important compared to block target size (average size of blocks existing in file- or forced value):
        ZRandomFile offers statistical tools to study the records/blocks population and to take appropriate decisions.
@note   block target size is a statistical value that is computed to be the average size of existing blocks withing the file.

This means that : in case of fixed length record, gradFreeSpace option is not appropriate (even if it may be used without any problem),
because it may induce a (small) waste of computing time.

  @} */ // ZRandomFileGroup










/**
@addtogroup ZRandomFileGroup
@{
  @weakgroup ZRFSTATS Dump utilities and Statistical tools for ZRandomFile

ZRandomFile embeds a set of statistical classes and methods for  ZRandomFile .

   - File content data dump :
        - File header
        - Content dump
        - Full dump (Header and Content)
        - On demand block dump
   - File surface analysis and dump
   - File content statistics

   - ZRFPMS : File activity report and follow-up
@}
*/

/**
@addtogroup ZRandomFileGroup
@{
  @weakgroup ZRFPhysical ZRandomFile Physical layer

For performance purposes, ZRandomFile is composed of 2 physical files that may strictly stay aligned :
 - Content file
This is the main file that will contain user data.
This is the file that is named by used to handle a ZRandomFile.
The name of this file is left to user's choice, despite some restrictions(see below).
This name will actually be the name of the ZRandomFile itself.
 - Header file
 This file contains infradata that is mandatory to the ZRandomFile behavior.
 The name of this file is deduced from the content file and is stored in the same path that content file.

 @note ZRandomFile users do not have to care about this duality, excepted when ZRandomFiles are backuped or moved.
 Header file name is transparently deduced from the content file name (uriString).
 In a normal usage case, user does not see header file. He only addresses the main ZRandomFile content file, the one he named.

@warning This implies that header file is absolutely necessary for a ZRandomFile to be recognized and to be read.
Users should never ever modify directly, or even delete, such files, nor move them without moving the main ZRF content file.
In such cases, damages could stay non recoverable.
However, there is an utility routine to rebuild a header file in case of damaged header file @ref ZRFUtilHRebuild

@see ZMFFileNaming


@see generateURIHeader for functions list for details

@image html zrfphysical.jpg "ZRandomFile physical layer" width=10cm
@image latex zrfphysical.jpg "ZRandomFile physical layer" width=10cm

@see ZMFSTATS
for an example of ZRandomFile physical layer applied to ZMasterFile ZIndexFiles

*/

/** @} */

/**
@addtogroup ZRandomFileGroup
@weakgroup ZRFUtilities ZRandomFile Surface management utilities

ZRandomFile embeds a set of utilities for managing disk space.

   @subsection ZRFUtilClearing Clearing ZRF file content

   If ZRandomFile contains records these records will be lost.
   If highWaterMarking option has been selected and is on, the existing used file space will be marked to binary zero.
   File space remains allocated and is fully available as one free block with all available space to be used.
    see: @ref  ZRandomFile::zclearFile()

   @subsection ZRFUtilTrunc ZRF File truncation

   File content could be truncated to a physical size whenever possible.
   The remaining space at the end of the file (belonging to Free blocks pool) is truncated.
   If no free block are available and the physical end of the content file, nothing happens.
   Existing content remains unchanged.
    see: @ref ZRandomFile::ztruncateFile()

   @subsection ZRFUtilExtend ZRF File extend

   This utility artificially extend ZRF file space in adding a free block space at the physical end of the content file.
    see: @ref  ZRandomFile::zextendFile()


   @subsection ZRFUtilReorg ZRF File re-organization

   This utility has two goals
     + reorder physically the file
     Records will be physically reordered in their respective logical rank.
     There will be no appearing change for users.
     + eliminate the possible holes between records
     At the end of the processing, the file's content remains strictly unchanged, and records rank order is preserved.
      see: @ref  ZRandomFile::zreorgFile

   @subsection ZRFUtilClone ZRF File cloning

   This utility will duplicate the ZRF file. For doing this it will
     + Create a new ZRF file with the same characteristics that the file to clone.
     + Populate this new file with origin content
     + Reorder physically the file in blocks' rank order
     + Add a free space at the end of the cloned file.
     see: @ref  ZRandomFile::zcloneFile()

   @subsection ZRFUtilHRebuild ZRF header rebuild

   This utility generates - or re-generates - a header context from a ZRandomFile well formed content file.
   This is usefull if a header file has been damaged or lost.
    see: @ref  ZRandomFile::zheaderRebuild()

    @note this utility explores the file surface (surfacescan) of the file to detect start/end of blocks with a specific bytes marker.
    If ever there will be such a sequence in data, it will be detected as a start or end of block record and will induce a loss of data.


*/


/**
@addtogroup ZRandomFileGroup
 * @defgroup ZRFLockGroup ZRandomFile File and Block (record) locking mechanism
 *
 * @par Locks at open mode
 *  - Automatic lock mechanism :
 *     + read locks block
 *     + write unlocks blocks
 *     + readnext unlocks current and locks next record
 *     + close release all locks (unlocks all)
 *
 *  - Exclusive lock :  locks all file. This option will improve global access performance
 *  in the meaning that it will save access to read/write file header during file session.
 *
 *  @par Lock masks at open
 *
 *                  no lock   0x0000
 *                  exclusive 0x0010
 *                  automatic 0x0020
 *
 *
 *@section ZRFLockInternals record lock internals - ZRandomFileLock
 *
 * ZRandomFile doesn't allow to manage locks at record level.
 * ZRandomFile has 3 structures for record locking.
 * These structures are managed by ZRandomLockFile, a class extension of ZRandomFile.
 *
 * - Record block header :
 *  + field Lock
 * Contains the lock mask that currently locks the record
 *  + field Pid
 * Process id of lock owner
 *
 * - File header : Used blocks pool (ZBAT) at record rank level
 *  + field Lock
 *  + field Pid
 *
 * - ZLockPool : located within ZFileDescriptor  and is particular to process.
 *  ZFileDescriptor holds in its private data a reference to any lock set to the file in a ZLockPool object that keeps memory of currently locks hold on records by current process.
 * It contains :
 *  + record address (only stable information for addressing records)
 *  + lock mask
 *
  */







/**
@addtogroup ZRandomFileGroup
@weakgroup recordranks Relative rank of a record for a ZRandomFile or ZMasterFile.

With ZRandomFile, the normal record access is made thru record relative position, or record rank.

This rank is given by the record position pointer in the used block records pool ZBlockAccessTable (ZBAT).

The logical relative rank is the user logical rank view and does NOT correspond to physical storage order :
ZRandomFile places the contents according to its size to optimize its behavior,
AND NOT necessarily according the record creation order.

ZRandomFile keeps for the current file session a set of indicators that depends on what action has been done on the file.
For instance last PhysicalPosition and Logical Position accessed as well as the record rank is kept for the process,
and allows to navigate through the file.
In addition, statistical data is cumulated during a file session.
@see ZRFSession

*/



/**
@addtogroup ZRandomFileGroup
@weakgroup ZRFSession what is a file session

A file session for ZRandomFile and ZMasterFile starts when a file is opened and ends when a file is closed.
Internal pointers and statistical counters and reset at the beginning of a session.

*/




/** @ingroup ZMasterFileGroup ZRandomFileGroup
 * @defgroup ZRFZFile_type  Opening a file as an other file type than the native one
 *
 *@section ZRFZFT Opening a file as an other file type than the native one
 *
 *  There may be multiple reasons why we may want to open an existing file as another type than the native one it has been originally designed for :
 *  - upgrade a ZRandomFile to a ZMasterFile
 *  - quick sequential search in an ZIndexFile without opening its father ZMasterFile
 *  - ...
 *
 *  However, doing such must comply to strict rules in order to preserve file's integrity.
 *
 *  Two elements while define these possibility, and the returning status while openning files in such a way.
 *
 * @subsection ZRFZFTCorr Correspondance between requested ZFile_type and what we found in Header
 *
 *@verbatim
 *  File header         Requested       Actions
 *  FileType            FileType
 *
 *  Any ZFile_type      ZFT_Nothing     no access   : rejected with   ZS_BADFILETYPE
 *
 *  ZFT_ZRandomFile     ZFT_ZRandomFile in any open mode.
 *                      ZMasterFile     in this case, ZRandomFile will be upgraded to a ZMasterFile, and a ZS_FILETYPEWARN warning is issued.
 *                                      The file must be open in exclusive access (ZRF_Exclusive) otherwise a ZS_BADFILETYPE will be emitted.
 *
 *
 *  ZFT_ZMasterFile     ZFT_ZMasterFile in any open mode.
 *                      ZFT_ZRandomFile only in ZRF_Read_Only open mode : in this case a ZS_FILETYPEWARN warning is issued
 *                                      For other open modes, trying to open a ZMasterFile as a ZRandomFile will be rejected with a ZS_MODEINVALID
 *
 *  ZFT_ZIndexFile      ZFT_ZIndexFile  in any open mode.
 *                      ZFT_ZRandomFile only in ZRF_Read_Only open mode : in this case a ZS_FILETYPEWARN warning is issued
 *                                      For other open modes, trying to open a IndexFile as a ZRandomFile will be rejected with a ZS_MODEINVALI
 * @endverbatim
 *
 *  In case of error a status ZS_BADFILETYPE is emitted and base _open routine returns to caller without opening the file.
 *
 * @par What is the logic underneeth :
 * - Opening any file types as a ZRandomFile
 * Because ZRandomFile manage and preserve ZReserved space without using infra-data, any other file type may be used as a read-only ZRandomFile,
 * even if it does not use ZReserved space. It only preserve it.
 * To avoid changes in the file content it must be opened as read only.
 * - Converting a ZMasterFile to ZRandomFile
 * However, there is a utility (static ZMasterFile method) that allows to downgrade a ZMasterFile to a ZRandomFile : @ref ZMasterFile::zdowngradeZMFtoZRF().
 *  - destroys the Reserved block into file's header
 *  - leave existing indexes to themselves
 *
 * - Opening a ZRandomFile as other type
 *    + ZRandomFile cannot be openned as a ZMasterFile : it must be first converted to a ZMasterFile
 * see @ref ZMasterFile::zupgradeZRFtoZMF() static method.
 * Using this method ZMasterFile will automatically create and write its ZMasterControlBlock into Reserved space.
 * File must be opened as exclusive access (ZRF_Exclusive|ZRF_All).
 * ZMasterFile is created without any index.
 *
 *    + this is not the case for ZIndexedFile
 * ZRandomFile is supposed to be a data file (and therefore not an Index file) and cannot not be upgraded to a ZIndexFile.
 *
 */

/**
@addtogroup ZRandomFileGroup
@defgroup ZRFFieldAccess Accessing a ZRandomFile by Field (Segment of record)

ZRandomFile manage a compound content and does not offers to access content through a dictionary.
However, it is possible to access its user content (records) by adhoc fields definition.

A ZRandomFile field is a segment of record defined by
   - its offset from the beginning of the user record,
   - a length.

As it is possible to search for values for a field, it is possible to access directly the field content:
   - to read its content
   - to write its content

In both cases, only the file segment corresponding to the given field will be accessed,
so that you may, for example, change only the segment of record corresponding to the defined field.

This is possible using the below two ZRandomFile methods

@ref ZRandomFile::zgetSingleField()
@ref ZRandomFile::zwriteSingleField()

@note Record data integrity AND data format is left to user's responsibility.

*/ // ZRFFieldAccess


/**
@addtogroup ZRandomFileGroup
@defgroup ZRFFreeSearch Searching in free sequential mode


ZRandomFile and ZMasterFile offer the possibility of a free sequential search for all or part of the file's records content.
The resulting structure is a collection of ZRandomFile records ranks (see @ref ZRFRanks) that will match given sequence(s) of bytes (text or binary data) according given rules.
Free search is done using user defined fields per rule.
A field is defined by
- a field offset from the beginning of user's record (starting from zero)
- a field length
@note length could be ommitted in giving a negative or null value (reference to a zero long).
In this case, the field length will be adjusted to given sequence ZDataBuffer::Size attribute.

The masterpiece of this free search facility is ZCollection_Rank that will store matching rules see @ref Z::add()

Resulting to this search, a collection of records' ranks is given, and may be refined using successive searches
that will apply to a previous collection optionally given in input.

As the optional collection to base the search on is safely copied during search initialisation,
it is then possible to recursively search on the same collection of records and refine search criterias.

This could be used by example for :
- searching a first selection giving a first collection of row within ZRFResult::ZTabIdx array
- then restrict the collection with a new search in passing reference of this array as ZArray<long> as pCollection.

Search will return a ZRFCollection object with a collection of record ranks. see @ref ZRFCollection.


@subsection ZRFFreeModes Searching modes

@par Searching all at once in the whole file or a whole record collection

@ref ZRandomFile::zsearchFieldAllCollection()

This routine searches all record set for matching fields values and returns the whole collection of records.

@par Searching dynamically by successive calls

@ref ZRandomFile::zsearchFieldFirstCollection()
@ref ZRandomFile::zsearchFieldNextCollection()

These two routines gets dynamically the corresponding records rank, enriching the records' collection ranks with matching records during successive calls.

The first one initialize the search and gets the first matching record.

The second one successively gets the next record matching the given sequence.

@subsection ZRFFreeSearchType Search rules processing options

By default, the search is made assuming binary search.
However it is possible to specify using optional mask that search should consider field and search sequence as CString
(meaning ended with '\0').
In this case, this allows to 'trim' leading and trailing spaces.

You may use the following options, depending on ZRFSearch_type enum, when searching in free sequential mode in a ZRandomFile :

Enum code is given by @ref ZRFSearch_type

@par Main mask values

+ ZRFEqualsTo   equal : exact search (the whole fields is the whole sequence)
+ ZRFStartsWith starts with  : Field starts with Sequence
+ ZRFEndsWith   ends    with : Field ends with Sequence
+ ZRFContains   contains     : Sequence is contained somewhere within Field

@par optional ORed mask

The following masks have to be ORED with one of the previous value to refine the search comparizon behavior.
+ ZRFString
the search sequence AND the read fields are to be processed as CString (string size is given by '\0' character)
+ ZRFTrimSpace
indicates that leading and trailing space characters have to be suppressed before comparing values
+ ZRFCaseRegardles
indicates that comparizon must not take care of upper or lower case


@subsection ZRFFreeError   Size error cases

When searching with ZEqualsTo option set (default option), the size of Sequence ZDataBuffer must be equal to the field Length,
otherwise an error status will be returned before any search operation and a ZException will be set with appropriate message.

Sequence size must be in any case less than or equal to field length.

Hereafter are the cases of incoherence between both ZDataBuffer::Size Sequence data and field length :

- ZEqualsTo       size of Sequence ZDatabuffer does not equal search field length.
- ZStartsWith     Field length is smaller that ZDataBuffer::Size.
- ZEndsWith       Field length is smaller that ZDataBuffer::Size.
- ZContains       Field length is smaller that ZDataBuffer::Size.

@note  if Field Length equals ZDataBuffer::Size this is equivalent to a ZEqualsTo option.


*/




}// namespace zbs


#endif // ZRANDOMFILE_DOC_H

