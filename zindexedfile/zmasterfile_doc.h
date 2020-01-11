#ifndef ZMASTERFILE_DOC_H
#define ZMASTERFILE_DOC_H

// this file owns the doxygen general documentation for ZMasterFile

namespace zbs {


/**
@page ZMasterFilePage  ZMasterFile Reference Documentation

@section ZMFMainZMFGroup ZMasterFile general presentation

@copydoc ZMasterFileGroup

@section ZMFMainZMasterFileAccess Accessing ZMasterFile and its indexes

  @copydoc ZMasterFileAccess

@section ZMFMainZIndex     ZIndexFile : ZMasterFile Index management

@subsection ZMFMainZIndexFileGroup ZIndexFile - Key Type management (definition, extraction & packing, depacking)

  @copydoc ZIndexFileGroup

@subsection ZMFMainZDictionnary Key dictionary

  @copydoc DictionaryGroup

@subsection ZMFMainDataFieldGroup Key data fields management

  @copydoc DataFieldGroup

@section ZMFMainZIdxType Managing ZIndex Key data types


@subsection ZMFMainZKeyGroup Querying ZMasterFile using ZKey
  @copydoc ZKeyGroup

 @subsection


@section ZMFMainZMFSTATS  ZMasterFile Performance statistics
@copydoc ZMFSTATS

@section ZMFMainZMFPhysicalZMasterFile Physical layer

@copydoc ZMFPhysical

@section ZMFMainUtil    ZMasterFile Utilities

@copydoc ZMFUtilities

@section ZMFMainCHG Changing Files parameters and indexes
@copydoc CHGGroup

 */





/**
@defgroup ZMasterFileGroup  ZMasterFile : Indexed Random Access File for content storage

@section ZMFGWhat What is ZMasterFile

ZMasterFile (further abbreviated ZMF) uses ZRandomFile base class in order to provide an indexed acccess to content with variable size on disk.
@see @ref ZMasterFile

It is composed of three main components (and several others) :

- ZIndexFile class
   + Holds and manage one index key for ZMasterFile.
   + Manages all extractions and conversion for building a coherent index key according its key dictionary.

- ZMasterFile class itself
   + Holds the effective content
   + Manages ZIndexFile objects (one object per defined key)

- ZKey class
   + Builds key values up to be used by ZMF from litteral or any given data value, or extracts key value from ZMasterFile record
   + Key value is ready to be used in index queries

From a user point of view, ZIndexFile may stay ignored.


In addition to ZRandomFile facilities (random access per logical relative rank), it allows :

- creating and maintaining indexes on the content, so that access can be made by index key content rather than relative position within file
- managing Journaling on the operations on file
- managing History on the operations on file

Updates on ZMasterFile and ZIndexFile(s) are made using internal 2 phases commit process to increase performance in lowing storage access number with rollback/commit mechanism.

@subsection ZMFGMain Main methods

 - zcreate
    creates the file and its infra-data structures
    @see @ref ZMasterFile::zcreate
 - zcreateIndex
    creates an Index defined by its dictionary
 - zadd
    adds a record logically positionned at the end of the record list.
    Physically the block containing the user's record may be anywhere on the file's surface
    according the better placement capability.
 - zremove
    logically deletes a record. block containing record is then placed in free block pool.
    if grabFreeSpace option is chosen then adjacent free blocks are agglomerated to make one unique free block and benefit from the whole space set.
 - zget
    reads a record from file according to its logical relative position and returns records content.
 - zgetnext
    reads the next record
 - zgetField
    gets a field given by the relative position of its record, its offset and its length.


@subsection ZMFGSpecial Special methods


 - zrecover : recovers a free pool block. It creates back a block from first freed block contained in free block and ungrab all deleted block agglomerated to it.
                these undeleted blocks become free blocks in free pool and can be themselves recovered.


*/

/**
 * @addtogroup ZMasterFileGroup
 * @{   */

/**
  @defgroup ZMasterFileAccess Accessing ZMasterFile and its indexes


Due to the necessity of updating all dependant indexes, update for Master File uses 2 phases commit mechanism.

@subsection ZMFFAAdd Adding records

Only record content is provided by the user.
Indexes will be updated appropriately after key content extraction in prepare phase.

@code
ZMasterFile ZRandomFile                     ZIndexFile          ZRandomFile ZIndexFile Base
                                            method              method
- _add2PhasesCommit_Prepare                 _add                _insert
- _add2PhasesCommit_Rollback                                    _add
- _add2PhasesCommit_Commit

- _add_RollbackIndexes                      _addRollBackIndex
@endcode

@subsection ZMFFARemove Removing records

 There are many ways to remove records from a ZMasterFile.
 - remove by rank : argument is pRank mentionning the relative position of record to remove within ZMasterFile.
@see recordranks

- remove by index :
    Key is extracted from the record and searched within Index.
    ZMasterFile Record corresponding to first value found is then removed from ZMasterFile.
    Arguments :
        + ZDataBuffer containing ZMasterFile Record
        + const long with index key number to search for.


- remove by address : argument is the logical address for the block containing the record to be removed.
- Remove with return :   same routines as above but returning deleted record content.

@subsection ZMFFASearch search for records content

@subsubsection  search per relative position :
We can use the ZRandomFile search facilities per relative positionning (rank).
Free sequential search using adhoc fields may also be used through a ZCollection_Rank object.
In this case, ZMasterFile is considered as a ZRandomFile (this is possible as far as file is open with ZRF_Read_Only mode).


- search first index matching a key content : ZMasterFile::zsearch()
  Search have to be made using ZMasterFile object in specifying :
  + index number : the rank of the index for ZMasterFile (rank within ZMasterControlBlock )
  + index key content : a ZDataBuffer containing index data to search converted to ZIndex internal key format.
  @note it is up to the user to insure the data conversion to appropriate key fields data type.

- search using a ZKey object
    This is the recommendended (simplest - errorless) way of searching a ZMasterFile with indexes.

    ZKey object is obtained using ZMasterFile::createZKey() or ZMasterFile::createZKeyByName().

    @note ZKey can be instantiated independantly from ZMasterFile.

    Using ZKey object you may then :
    + load key fields with your desired values.
        ZKey insures the data conversion (type cast) whenever necessary to
        the appropriate key field data type before converting to key internal format.
    @see ZKey::setFieldValue()
    + verify key current content.
        Printing its fields values per field in "human readable" format.
    @see ZKey::zprintKeyFieldsValues()
    + search ZMasterFile content using ZKey
    @see ZKeyGroup ZKey




By default, the first occurrence of record with a corresponding key matching the mentionned key content will be returned.

When using searchAll set of methods, then a list of logical addresses corresponding to records matching the given key content is returned.
Records can then be accessed using their logical address.

A ZKey object may be defined and used to search for key values.

@see ZKeyGroup ZKey gathers

- free search :

    There is a possibility to search through any ZMasterFile record for a given value :
        + either structured free search : from a given Field (or segment of data) offset within ZMasterFile records, with the specified length
        + RFFU: or unstructured free search : wherever the given sequence could be found within all records.

@note Performance warning : these two methods have to be carefully used because of the performance issues it may induce.

  */


/**
  @defgroup ZJournaling    Journaling and History management

  Journaling and History are private to calling process and are NOT SHARED.

  file names are made according the following structure

  journaling file name :  zrf<pid>.jnl

  history file name : zrf<pid>.his



*/


/** @} */ // ZMasterFile group


/**
  @addtogroup ZMasterFileGroup
  @defgroup ZMFSearch Searching ZMasterFile for content

@section ZMFSearchSection Searching ZMasterFile for content

@subsection ZMFSearchTypes Index searches and free sequential searches

ZMasterFile allows the same free sequential search facilities than its base object ZRandomFile.
It is then possible to fully use these facilities.

Additionnally, ZMasterFile offers a search based upon index key values. Accessing key values using indexes saves drastically time and file accesses compared to free sequential search.
However, there is an priority order between access types to respect :
- first access indexes and establish a collection
- restrict collection using free sequential search (that will only scroll the existing collection content).

So that, there is a medium term to find between the capabilities of free sequential search and speed of access of indexed search.

@subsection ZMFIndexAccess Access ZMasterFile using its indexes.

It is possible to search for
- a single key value or for multiple key values i. e. a collection of values
- using an exact key value or a partial key value

@subsection ZMFPartialKey Exact key value or partial key value in a search operation

A search may be made with a partial key value, this means that requested key size will represent a partial segment of index key.
In this case, the search operation is made on the partial value of given key content and
- if a simple search operation is invoked, then the first found key value is returned
- if a multiple search operation giving a collection of records ( either zsearchAll or zsearchFirst, zsearchNext )

@subsection ZMFZKey Using ZKey object

ZKey is an object dedicated to index key search (see @ref ZKey). It allows to
- store index description (ZMasterFile index number, Key dictionary)
- format and load key value using field dictionary
Field(s) content are then formatted to appropriate key 'internal' format in order to be appropriatedly used.
- create a partial key value in order to search multiple results, or collection of results, from a single partial key value.

ZKey is designed to manage adHoc key accesses to ZIndexFile-ZMasterFile structure.

It gathers the necessary routines and methods to build up a key content, to control coherence of key field data types,
and to access and select ZMasterFile records.

  @see ZKey
  @see ZKeyCorrespondence




*/

/**
  @addtogroup ZMasterFileGroup

@defgroup ZMFRemoving Removing content from ZMasterFile

@section ZMFRemoveSingle Removing a single record

@section ZMFRemoveMass Removing a set (collection) of records
  */

/**  @addtogroup ZMasterFileGroup
 *
 *  @defgroup ZMFSTATS  ZMasterFile Performance statistics
 *
 *  ZMasterFile embeds a set of Performance statistics facilities that  gathers and extends the tools from ZRandomFile.
 *
 *  The main objective is to
 *  - follow activity on the different components of ZMasterFile
 *      + ZMasterFile behavior
 *      + The behavior of each individual ZIndexFile (one per defined index key)
 *  - monitor Performances on each of these above components with dynamic sessions
 *      You can then dynamically start a monitoring session and get the results when you wish so.
 *      These results will include a dynamic timer result with a delta time showing elapsed time since the session has been started.
 *
 *  @see ZRFSTATS
 *
 *
 */


/** @addtogroup ZMasterFileGroup
  @defgroup ZMFPhysical ZMasterFile Physical layer

ZMasterFile physical layer is based on ZRandomFile physical layer.
We may distinguish
- Main content file

This is based on a ZRandomFile base structure that embeds ZMasterFile infra-data extension.

- Indexes (ZIndexFile)

Each defined index is stored and managed through its own ZRandomFile structure that embeds ZIndexFile infra-data extension.

@note
ZMasterFile user does not have to care about this file structure, as he will address anything through ZMasterFile.
However for some operations
 - statistical data
 - split ZIndexes from ZMasterFile's location
it is good to known this architecture, as well as file's naming conventions.
@see

@image html zmfphysical.jpg "ZMasterFile physical layer" width=10cm
@image latex zmfphysical.jpg "ZMasterFile physical layer" width=10cm

@see ZMFFileNaming

 */

/** @addtogroup ZMasterFileGroup
 * @defgroup ZMFUtilities ZMasterFile utilities

 ZMasterFile offers a set of tools allowing to maintain and recover its structure and its indexes.

@subsection ZMFUIndexes Managing Indexes
    - zindexRebuild
        Rebuilds an index for the current ZMasterFile. see @ref ZMasterFile::zindexRebuild
    - zRepairIndexes
        A static method of ZMasterFile that allows to diagnose and repair damaged / missing indexes from the definition contained in
    ZMasterControlBlock of a given ZMasterfile. see @ref ZMasterFile::zRepairIndexes.
    - the whole set of XML routines that allows to store file parameters (Including Indexes definitions), and allows to rebuild / modify existing definition from the xml content.
    see @ref

@subsection ZMFUMaintenance ZMasterFile maintenance

 - zreorgFile

    Reorganize main content file structure and recreates all indexes.
    Even if the purpose of this routine is the same than ZRandomFile::zreorgFile(),
    the processing is much more important and takes care of all defined indexes that are rebuilded by this routine.
    + content data file is reorganized :
        . blocks are physically ordered in rank order
        . holes are eliminated
        . free blocks are gathered into one unique free block at the end of the file
        . deleted blocks are physically removed from file
    + each index file is reordered to correspond to new addresses of content file, one after the other.


  */

/**
  @addtogroup ZMasterFileGroup
@defgroup ZMFCollection Using collection of records with ZMasterFile

@section ZMFCollIntro Using collection of records with ZMasterFile

Index searches with potentially multiple results are using collections of results using the class @ref ZIndexCollection.
Collections may be refined later on by sequential free search as per ZRandomFile collections see @ref ZRFCollection

ZIndexCollection is always defined from a ZIndexFile of a ZMasterFile, and holds the references from index ranks
that have currently been found using a search operation.
It may hold the whole index file if no key selection have been mentionned. (IMPLEMENT)

It consists, for its base object, of a ZArray of ZIndexResult objects mentionning
- the Index rank (ZIndexFile relative position) matching the given key content (either exact or partial),
- the related ZMasterFile record address given by index that allows to get directly ZMF record using ZMasterFile::zgetByAddress() method.

@note ZIndexCollection is a snapshot of selected ZMasterfile records that correspond at one moment to selection criterias.
As it is not a dynamic object, collection is NOT modified accordingly when the ZMasterFile to which collection refers to is himself modified
- record content modification
- record suppression
- record add
So that, in order to preserve collection integrity while other processes are capable of accessing MasterFile origin of the collection,
it is utmostly important to lock selected records with the appropriate lock mask using ZIndexCollection::zlockAll() or, more simply,
in mentionning the appropriate lock mask while creating collection see @ref ZMFCollLock


@subsection ZMFCollCreate Creation and destruction of a collection

@par Creation of a collection

Collection object is generated during the first step of Index search operation

- search all operations : the whole collection object is created and populated at once.
- search first operation and search next operation : the collection object is created during search first operation,
therefore feeded on the flow by successive searches until key content value (exact or partial) is no more found in index.

@par Destruction of a collection

As a pointer to the generated collection object is returned (created by 'new'), it is up to the caller to delete this object after having used it.

@subsection ZMFCollRecursive Recursive use of collection of records

Collections of records may be used recursively, so that you may for instance :
- first generate a collection of records using an interval search on a key values range,
- refine this collection of records (and only this collection of record) using free search
In this case, collection is mentionned as input collection for the selection method.

@subsection ZMFCollLock Collection and record locks

If collection is used in a potential multi-access (ZMasterFile not opened with an Exclusive access mode) then it could be wise to lock selected records.

This could be done either

- in mentionning the appropriate lock mask while creating collection during an index search with multiple results
 + ZMasterFile::zsearchAll()
 + ZMasterFile::zsearchFirst()
 + ZMasterFile::zsearchInterval()

  for details see ZIndexCollection::getAllSelected() or ZIndexCollection::getFirstSelected() .

- in specifying the appropriate lock mask using ZIndexCollection::zlockAll()

@note As collection could be recursively refined with several rules, lock on ZMasterFile records are released as soon as they are rejected from current selection (IMPLEMENT).

@subsubsection ZMFCollAccess Accessing collection elements

@par During its creation

During its creation, using 'on the flow' creation methods, ZIndexCollection::getFirstSelected() and ZIndexCollection::getNextSelected()
a collection returns record content that has been selected
as well as its reference with a ZIndexResult object giving the index rank, and the ZMasterFile record address.

@par Using it after it had been fully created

Once having being fully created, collection may be accessed using operator [] and a collection rank.
It gives the reference of ZIndexFile rank and its corresponding ZMasterFile record address in the chosen index order.
ZMasterFile record content may then be accessed using ZMasterFile::zgetByAddress() method:
@verbatim
      <ZMasterFile object>.zgetByAddress(<record ZDataBuffer>,<collection>[rank].ZMFAddress,<lockmask>);
@endverbatim

@note a collection is a static snapshot of ZMasterFile content at a given moment.
ZMasterFile is susceptible to be modified by another process that may change existing selected records content,
 add new records that may correspond to selection criterias, or even suppress (remove) record selected within collection.
Locking selected records may be a temporary solution to prevent, at least, these selected records from being changed or deleted by another process.
However, it does not prevent another process from adding records that could be validly candidates to be selected within collection.

@subsubsection ZMFCollMass Mass operations using collections of record

Once a collection has been created and populated with a set of record, it is possible to use mass operations on established collections.

This means that collection offers methods that allow to process 'all records at once' for specific operations.
Basically, main operations are :
- get a whole collection of records ZIndexCollection::zgetAllSelected()
This creates the collection content all at once.
- mass deletion of selected records ZIndexCollection::zremoveAll()
Once collection has been totally created, it removes (delete) all records selected.
- mass lock selected records ZIndexCollection::zlockAll()
Once collection has been totally created, it applies a given lock mask to the whole set of selected records.
Apply a given lock mask to the whole set of selected records.
- mass unlock selected records ZIndexCollection::zunlockAll()
Unlocks to the whole set of selected records.
- mass change an adhoc field value on all records ZIndexCollection::zchangeField()
Modify a given adhoc field with the given value for the whole set of selected records.
Adhoc field must neither be part of a defined key nor even be a subfield or an overlapping field.

@subsubsection ZMFCollRetry Retry operations on collection after status failure ( lock retry management )

Collection embeds the capability to retry last attempted operation for continuing the engaged mass operation that fails with a locked status on the remaining set of unprocessed records.
When retrying, have to be mentionned
- a number of times to retry
- a number of millisecond to wait after each retry
If operation is still not successfull, returned status is
- ZS_LOCKED is sent back if record cannot be access because it is still locked
- appropriate low level file access status is sent back in case of other error and ZException is set with appropriate message


@subsection ZMFInterval Searching index with a range of key values - or Interval search

It is possible to search an index for a range of value.
We must then mention the lowest key value to select index values for and the highest value
  */


/** @defgroup ZMFFileNaming Physical files naming conventions

  Despite the user sees and uses one file pathname, there are many files that are created and maintained.

  It is important to know how names are deduced and generated, and what names are naming.

@note
 Some extensions cannot be used for naming a ZRandomFile or a ZMasterFile because reserved for internal naming purposes.
 This extension is by default <.zrh> and is defined by a preprocessor parameter named __HEADER_FILEEXTENSION__
 In the same way, it is forbidden to use in a ZRandomFile main file name the extension given by __ZINDEX_FILEEXTENSION__.
 This extension is by default <.zix> and is reserved to main ZIndexFile names.
 Both of these preprocessor parameters are defined in file zrandomfiletypes.h .



@subsection  FNPConventions Conventions

 In a full file name  /home/mydir/filename.ext

 - /home/mydir is the directory path

 - filename is the root name

 - ext is the extension


@subsection FNPZRFName Naming conventions for ZRandomFile

For each ZRF, user gives one file name and (its directory path).

This name will apply to ZRF main content file, i. e. the main file where effective data will be stored in.

This file is later on called "main file" or "main ZRF file" (or even later "main ZMF file" for ZMasterFile).

Additionnally to this file, ZRandomFile adds a file that will contain all infra-data describing the main file.

This file is called a "header file".

- Header file naming convention

@verbatim
    <ZRandomFile root name>.<__HEADER_FILEEXTENSION__>
@endverbatim

@subsection FNPZMFName Naming conventions for ZMasterFile

ZMasterFile main file itself is named with ZRandomFile naming conventions.


ZIndexFile Index files root names generation rule

@verbatim
          <ZMasterFile root name>[<index name>]-<index rank>
@endverbatim

As ZIndexFile file is itself a ZRandomFile, it then will be subject to ZRandomFile's naming conventions.

*/



/** @addtogroup ZMasterFileGroup
 * @defgroup ZMFRecovery Recovering a damaged ZMasterFile structure
 *
 * @section ZMFRecSec  Recovering a damaged ZMasterFile structure
 *
 * @subsection  ZMFRecCasesZMF ZMasterFile itself has been damaged
 *
 * @par Main Content Data file is damaged or missing
 *
 * 1. Restore a backup of the main content files
 *
 *  - ZMasterFile Main content file
 *  - ZMasterFile Main header file
 *
 * 2. And then restore each of its index(es)
 *
 *    + ZIndexFile content file
 *    + ZIndexFile data file
 *
 * OR
 *
 * 3. You may generate any defined index within ZMasterFile's ZMasterControlBlock using index repair utility (see further)
 *
 *
 * @par Main Header file is damaged or missing
 *
 * 1. Cautiously delete ZMasterFile Header file (important : AND NOT Content file)
 *
 * 2. Run zheaderRebuild() on this file, so that a header file will be created for this file
 *      @ref  ZRandomFile::zheaderRebuild()
 *      You then have a ZRandomFile with all your recovered content (ZMF content file plus ZMF header file)
 *
 * 3. Upgrade this ZRandomFile to a ZMasterFile structure (with no Index yet) see @ref ZMasterFile::zupgradeZRFtoZMF
 *
 * 4. Generate Index keys using either
 *
 *   + your initial program defining index content and dictionary
 *   + the XML definition file you already have, describing all your ZMasterFile structure, parameters and indexes definitions.
 *
 * @note This is the reason why it is utmost important to save your file / index definition to an xml file
 *       as soon as they have been set to ZMasterFile's header.
 *
 *    Your ZMasterFile is up and ready to be used.
 *
 *  @subsection ZMFRecCasesZIF Index(es) files are damaged or missing
 *
 *  As index(es) definition is stored in both ZMasterFile and ZIndexFiles, it is quite straight forward, using @ref ZMasterFile::zrepairIndexes static routine.
 *  This utility may run as diagnose tools without modifying the existing file (option pRepair = false ) or may diagnose then correct the dammaged indexes and rebuild them.
 *  An additional option (pRebuildAll=true) offers the possibility to rebuild all indexes from scratch, even those which are healthy indexes.
 *
 */

/** @addtogroup ZMFUtilities
  @defgroup XMLGroup Changing file characteristics using xml files definition

  @section XMLChanging Changing file characteristics using xml files definition

  Changing base file characteristics : @ref ZMasterFile::zapplyXMLFileDefinition

  Changing file indexes

  Changing or adding one single index @ref ZMasterFile::zapplyXMLIndexRankDefinition
  - add index
  - remove index using the tag <Remove>
  - add or remove fields into an index
  - change fields attributes

  Managing the whole set of indexes : @ref ZMasterFile::zapplyXMLFileDefinition
  - remove indexes
  - add indexes
  - modify indexes

@ref ZMasterFile::zapplyXMLFileDefinition

Using this static routine, you may perfectly change multiple parameters for a file at a time, including modifying or even adding indexes,
using xml file description, under certain conditions.

This modified description should point to an existing file that have to be closed prior invoking this above mentionned routine :
file will be opened with exclusive access.

@subsection XMLZRF ZRandomFile (and therefore ZMasterFile base) changes

Constraints :

- changing URIContent implies that the changed URIContent points to an existing ZRandomFile/ZMasterFile corresponding to description.
This means that if you change this value, a valid path file pointing to the file you want to change must be indicated.

Some fields are not modifiables :

- URIHeader : as it is deduced from content main file name URIContent at open time and create time, it CANNOT be modified: any modification will not been taken into account
- URIDirectoryPath : as it is deduced from content main file name URIContent at open time and create time, it CANNOT be modified: any modification will not been taken into account

Modification on base RandomFile parameter field will cause a change in the header file.

- BlockExtentQuota : will change the behavior of the pool tables : how much elements these tables will be increased at each time they need more space. see @ref zloadXMLFileDefinition
- InitialSize : this change the initial size of the file. This value will stay in header and is used when the file is cleared as the free block size the empty file will be loaded with.
- BlockTargetSize : you may adjust this value for performance purpose according the various stat you may report. see @ref ZRFSTATS

@subsection ZMFCHG ZMasterFile changes

- Duplicates : There is no specific problem to change a value from ZST_NODUPLICATES to ZST_DUPLICATES. However, changing ZST_DUPLICATES to ZST_NODUPLICATES may cause Index to be rejected if a duplicate key is encountered during index rebuild.
In this case, Index will be rejected and former Index will be lost.

@note If a typo is detected then it will be considered as ZST_DUPLICATES.

Any index without modification, or with modifications that only concern "non modifiable" fields, then index is not modified and not rebuilt.

Any modification(s) on an index will cause :
 - If index does not exist :
     + Its creation
     + Its rebuild
 - If index already exist :
     + Its rebuild : rebuild process clears the index file first, then re-create all indexes from master file.

Index modification(s) that are not involving such process is
- Key dictionnary field name change : field name is only changed within appropriate dictionary with no index rebuilt. see further Problems

@warning Changing fields description position in the xml file will change their rank in dictionary REGARDLESS the <Rank> tag that will not been taken into account.

These modifications could be applied either on Index major fields or one field of its key dictionary.

@par Potential Problems

- Two indexes have the same name : this is not allowed and creation will be rejected. So, if you duplicate an index definition, you must at least change its name.

- Changing the name of an index : as a new the file corresponding to former index name must be manually removed

- Inserting an index : all indexes after the insertion point will be considered as new indexes.


*/




} //namespace zbs


#endif // ZMASTERFILE_DOC_H

