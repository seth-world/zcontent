#ifndef ZMASTERFILE_QUICKSTART_DOC_H
#define ZMASTERFILE_QUICKSTART_DOC_H

namespace zbs {

/**
  @page ZMFQuickStart Starting up using ZMasterFile
@tableofcontents
@latexonly
\startcontents[mainsections]
\printcontents[mainsections]{l}{1}{\section*{Main Sections}\setcounter{tocdepth}{2}}
\clearpage
@endlatexonly

@section ZMFQSIntro Introduction - Example presentation

 This chapter is a user's guide concerning the use of ZMasterFile.

 We will follow an example all accross this chapter with a simple data definition.
 Indeed this example is silly, and not realistic. But this is for the purpose of showing usage and possibilities of ZMasterFile methods.

 In this example, we will
 - load in a ZMF a mix of structured data and informal, varying length data with various format.
 - define two indexes
 - feed ZMF with data
 - remove records
 - search for data in multiple ways
 - use some ZMF utilities

@subsection ZMFQSRequired Required include files

@code
//------- for ZBaseSytem-------------

#include <config/zconfig.h>
#include <ztoolset/zarray.h>
#include <ztoolset/zerror.h>
#include <zindexedfile/zmasterfile.h>

//------- for data types handling-----

#include <typeinfo>
@endcode

@subsection  ZMFQSDataDef Data definitions

@code

ZStatus wSt;

// an enum for qualifying content : indeed we should better use Mime type.

    enum Doc_type : uint8_t {
        DT_Nothing = 0,
        DT_Pdf = 1,
        DT_Jpeg = 2,
        DT_Html = 3,
        DT_Mp4 = 4,
        DT_Mail = 5,
        DT_Other = 6
    };

// our structured data for records

    struct T3 {
      char      String1 [50];
      float     NumericField [3]  ;
      Doc_type  Type;
    };

// our constants to feed the file with
// Note that DT_Nothing will be updated later

    struct T3 T3Cst[] =
    {
    {"my heart beats"        ,  {110 ,-155 ,1} ,DT_Nothing  },
    {"my heart beats for you",  {110 ,-1024,1},DT_Nothing  },
    {"and so beats my skin",    {110 ,-152, 1},DT_Nothing  } ,
    {"xxxx",                     {110, 110 ,1} ,DT_Nothing },
    {"this is one",             {1 , 110 ,1 },DT_Nothing },
    {"aaaa   ",                 {34 , 110 ,1},DT_Nothing  },
    {"eer",                     {5, 110 ,1 },DT_Nothing },
    {"   dddd",                 {250, 110 ,1 },DT_Nothing },
    {"jkfdlsakfj",              {2025, 110 ,1 },DT_Nothing },
    {"egadfk",                 {25, 110 ,1 },DT_Nothing },
    {"aaaa   ",                {12045, 110 ,1 } ,DT_Nothing }
    };

// some files with various format : in fact the main file's content

   const char * PictureFiles [] = {
        "/home/gerard/Pictures/DSC_01231.jpg",
        "/home/gerard/Pictures/DSC_01232.jpg",
        "/home/gerard/Pictures/DSC_0123.JPG",
        "/home/gerard/Pictures/DSC_0146.JPG",
        "/home/gerard/Pictures/DSC_0394.JPG",
        "/home/gerard/Pictures/DSC_0414.JPG",
        "/home/gerard/Pictures/Gelato.mp4",
        "/home/gerard/Pictures/invoice2016-04-26.pdf",
        "/home/gerard/Pictures/QItemDelegate Class | Qt Widgets 5.7.html",
        "/home/gerard/Documents/DSC_0265.JPG",
        "/home/gerard/Documents/DSC_0267.JPG",
        "/home/gerard/Documents/DSCN0971.JPG"
    };

@endcode



  @section ZMFQSInstantiate Instantiating ZMasterFile

  Instantiating ZMasterFile object doen't create anything on storage.

  It simply creates an object with the memory structure within the process ready to be used either to create a ZMF or to open a ZMF.

  You may specify a file path at this stage with an uriString, but this is not mandatory.
  At anytime BEFORE having opened ZMF you may set or change this path using ZMasterFile::setPath().

 @subsection ZMFQSInsV1 version 1

@code
uriString wPath;
    wPath = "/home/gerard/testindexedpictures.zmf";
    ZMasterFile ZPicture(wPath);
@endcode

 The main file path should already have been tested and stored.

 This can be done using uriString facilities.

 But in this case, if the path is not a valid path, then ZMasterFile constructor aborts application.

 So, we do prefer the following.

 @subsection ZMFQSInsV2 version 2

@code
    ZMasterFile ZPicture ;
@endcode


  @section ZMFQSCreating Creating a ZMasterFile

  We will create an empty ZMasterFile structure with a free available space (contiguous best try) of 100000 bytes.

  This is the most simpler way to create a ZMasterFile structure.

 Excepted mandatory data, meaning file path and initial file space, all other parameters are defaulted.

@code
    // we do not want to leave the file open (pLeaveOpen is defaulted to false)

    wSt=ZPicture.zcreate("/home/gerard/testindexedpictures.zmf",100000 );
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
@endcode

  we chose an extension name of ".zmf" but this is NOT mandatory. Any extension is valid excepted the reserved ones.
  see:@ref FNPZMFName.


  @section ZMFQSSetting Setting ZMasterFile options

  By default, grabFreeSpace is set to on (true), and highWaterMarking is set to off (false).

  AllocatedBlocks and BlockExtentQuota,the pool tables size parameters, are set to their defaulted parameters (_cst_default_allocation and _cst_realloc_quota)
  see: @ref ZArray

  We could change that at this stage.

  But one rather important parameter is BlockTargetSize.
  We will set it to a value (this is not mandatory, but wishable).
@code
    wSt=ZPicture.setBlockTargetSize( 100000 ); // it can be done while file is closed. File is opened then closed
    if (wSt!=ZS_SUCCESS)                       // Warning for eventuality of file lock in between
                ZException.exit_abort();
@endcode

  @section ZMFQSCreatIdx Creating Indexes

  At this stage, ZMasterFile structure is created including on storage, with all necessary infra-data to work with.

  It's time to create indexes.

  For this example, we will create two indexes, among which one is "silly", but necessary to show capabilities.

  For creating an index the underneeth process is

  - create a key dictionary CZDictionary : upload fields in their key order (key rank)
  - create the Index structure using this dictionary

  This dictionary will store any field in the key order we define to interpret, extract, convert whenever necessary data.

  @subsection ZMFQSCreatIdxFirst First index

   Its name will be "First Index", with a touch of originality.

   - String1 as first rank (an array of char)
   - NumericField as second rank (this is an array of float data type)

@note
    Index names must be at maximum 15 characters.
@note
    I am conscious that the first field may be strictly enough. But, I want to show how we can manage multiple fields indexes.

  The process is :
  - define fields in a key dictionary
  - control by printing fields definitions if you whish
  - create index within ZMasterFile

@code
CZKeyDictionary wDic;       // the dictionary for the created index

// define index keys
//            one index key on String1 then NumericField
//
  typedef decltype (T3::String1) T3String1;
  wSt=wDic.zaddField<T3String1> ("String1",offsetof (T3,String1));
  if (wSt!=ZS_SUCCESS)
              ZException.exit_abort();

  typedef decltype (T3::NumericField) T3Numeric;
  wSt=wDic.zaddField<T3Numeric>("NumericField",offsetof (T3,NumericField));
  if (wSt!=ZS_SUCCESS)
              ZException.exit_abort();

  wDic.print(stdout); // dumping field list content

  wSt=ZPicture.zcreateIndex(wDic,                 // the key Dictionary that just has been set
                            "First Index",        // user defined name (not used internally)
                            ZST_NODUPLICATES);    // Allowing duplicates or not : not allowing duplicates

  if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();;

@endcode

 As a result, we have, listed to stdout
@code
-----------------------ZKeyFieldList content-------------------
 rank   Offset  Natural Internal Array nb              Name ZType
 < 0>        0       50       50       50 <        String1> <ZType_Array | ZType_Char>
 < 1>       52       12       15        3 <   NumericField> <ZType_Array | ZType_Signed | ZType_Endian | ZType_Float>
---------------------------------------------------------------
@endcode

 We can then verify that data type analysis has been correctly done.

 The column entitled 'Natural' gives the natural total size of the field,
 while the one entitle 'Internal' gives the size ZIndexFile will store after its conversion.
 ArrayNb gives the number of occurrences for an array (for an atomic data type this value is 1).

 And to stderr, it appears

@code
 ---------Rebuilding Index <First Index>------------
 File is /home/gerard/testindexedpictures-FirstIndex-00.zix
 Index file size is computed to be 0
Clearing index file
 ---------Successfull end rebuilding process for Index <First Index>------------

@endcode

 With that message, we know that ZMasterFile tried to rebuild the index file from its father's content.
 We have the mention of the index main content file that have been generated from the name we gave initially to ZMasterFile,
 here /home/gerard/testindexedpictures-FirstIndex-00.zix.

 This will be systematically the case : when a new index is created, MasterFile could be populated,
 then it tries to rebuild the index from its content in order to keep indexes aligned.

 So that, creating new indexes on an existing file could take some time and consume some resources.

  @subsection ZMFQSCreatIdxSecond Second index

   Its name will be "Second Index".

   - Type will be the one and unique field (an enum type, yes it works also)

@code
    wDic.clear();                                   // reset work dictionary
    typedef decltype (T3::Type) T3Type;

    wSt=wDic.zaddField<T3Type> ("Type", offsetof(T3,Type));

// Then create index within ZMasterFile

    wSt=ZPicture.zcreateIndex(wDic,                 // the key Dictionary that just has been set
                              "Second Index",        // user defined name (not used internally)
                              ZST_DUPLICATES);     // Allowing duplicates

    wDic.print(stdout);                             // control for fun : see what comes out of an enum

@endcode

    @subsection ZMFQSSetParams Setting some options and openning the file
 We will now set an optional parameter of the file : setGrabFreeSpace, and when it has been done, open the file for writing.

 @note Setting an option after the file has been created may be done while the file is open or close, it has no importance.
 However, if the file is closed it may point to a valid file path : file path may have been previous set using setPath() method.
 During parameter(s) setting, file will be openned in ZRF_Exclusive mode.
 This means that if file is accessed by another user and/or process, this action will fail.


@code
    if (ZPicture.setGrabFreeSpace(true)!=ZS_SUCCESS)
                                   ZException.exit_abort();

    if ((wSt=ZPicture.zopen(ZRF_Write|ZRF_Exclusive))!=ZS_SUCCESS) // load document
                {
                    ZException.exit_abort();
                }
@endcode

@subsection ZMFQSGetXML Getting file's infra-data in XML format

 We will now get XML infra-data definition for the file.
@code
    ZPicture.zwriteXML_FileDescriptor();
@endcode

 In the working directory, we can find a file called <testindexedpictures.xml>

 It contains our ZMasterFile infra-data definition in xml format.

@code
<?xml version='1.0' encoding='UTF-8'?>
<zicm version="2.00">
<File>
  <ZFileDescriptor>
      <URIContent>/home/gerard/testindexedpictures.zmf</URIContent>
      <URIHeader>/home/gerard/testindexedpictures.zrh</URIHeader> <!-- not modifiable generated by ZRandomFile-->
      <URIDirectoryPath>/home/gerard/</URIDirectoryPath> <!-- not modifiable generated by ZRandomFile-->
      <ZHeaderControlBlock> <!-- not modifiable -->
          <FileType>ZFT_ZMasterFile</FileType>  <!-- do not modify : could cause data loss  see documentation-->
          <ZRFVersion>2000</ZRFVersion> <!-- not modifiable -->
          <OffsetFCB>4698</OffsetFCB> <!-- not modifiable -->
          <OffsetReserved>64</OffsetReserved> <!-- not modifiable -->
          <SizeReserved>4634</SizeReserved> <!-- not modifiable -->
      </ZHeaderControlBlock>
   <ZFileControlBlock>
      <AllocatedBlocks>15</AllocatedBlocks> <!-- not modifiable -->
      <BlockExtentQuota>5</BlockExtentQuota>
      <InitialSize>100000</InitialSize>
      <BlockTargetSize>1</BlockTargetSize>
      <HighwaterMarking>false</HighwaterMarking>
      <GrabFreeSpace>true</GrabFreeSpace>
   </ZFileControlBlock>
  </ZFileDescriptor>
  <ZMasterControlBlock>
         <HistoryOn>false</HistoryOn>
         <JournalingOn>true</JournalingOn>
         <IndexFileDirectoryPath></IndexFileDirectoryPath>
         <!-- Changing IndexRank position in a file description will imply huge modifications -->
         <Index>
         <IndexRank>
           <Rank>0</Rank> <!-- not significant -->
           <Name>First Index</Name> <!-- warning modification of this field is dangerous see documentation -->
           <Duplicates>ZST_NODUPLICATES</Duplicates> <!-- warning modification of this field is dangerous see documentation -->
           <AutoRebuild>false</AutoRebuild>
           <ZKDic>
               <NaturalSize>62</NaturalSize>   <!-- not modifiable -->
               <InternalSize>65</InternalSize> <!-- not modifiable -->
              <KeyField>
                <Rank>0</Rank>  <!-- not significant -->
                <Name>String1</Name>
                <NaturalSize>50</NaturalSize>
                <InternalSize>50</InternalSize>
                <ArraySize>50</ArraySize>
                <ZType>ZType_Array | ZType_Char</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>0</RecordOffset>
              </KeyField>
              <KeyField>
                <Rank>1</Rank>  <!-- not significant -->
                <Name>NumericField</Name>
                <NaturalSize>12</NaturalSize>
                <InternalSize>15</InternalSize>
                <ArraySize>3</ArraySize>
                <ZType>ZType_Array | ZType_Signed | ZType_Endian | ZType_Float</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>52</RecordOffset>
              </KeyField>
          </ZKDic>
         </IndexRank>
         <IndexRank>
           <Rank>1</Rank> <!-- not significant -->
           <Name>Second Index</Name> <!-- warning modification of this field is dangerous see documentation -->
           <Duplicates>ZST_DUPLICATES</Duplicates> <!-- warning modification of this field is dangerous see documentation -->
           <AutoRebuild>false</AutoRebuild>
           <ZKDic>
               <NaturalSize>4</NaturalSize>   <!-- not modifiable -->
               <InternalSize>5</InternalSize> <!-- not modifiable -->
              <KeyField>
                <Rank>0</Rank>  <!-- not significant -->
                <Name>Type</Name>
                <NaturalSize>4</NaturalSize>
                <InternalSize>5</InternalSize>
                <ArraySize>1</ArraySize>
                <ZType>ZType_Enum| ZType_Signed | ZType_Endian | ZType_S32</ZType>   <!-- see ZType_type definition : beware the typos -->
                <RecordOffset>64</RecordOffset>
              </KeyField>
          </ZKDic>
         </IndexRank>
         </Index>
  </ZMasterControlBlock>
</File>
</zicm>
@endcode




@section ZMFQSSetIdx Setting indexes option
    - separating physical index files from Main ZMasterFile location.
    - changing indexes parameters value


@section ZMFQSOpening Opening and closing ZMasterFile
  - session
  - open modes

 @section ZMFQSFeeding Feeding ZMasterFile

  You do not have to take care of indexes while feeding ZMasterFile.
  You can use a ZMasterFile as a ZRandomFile.
  Once having being defined, indexes will transparently be updated at any single update operation (check return status however).

  - testing the return status :

Apart from system errors, that are severe errors, you may encounter the status ZS_DUPLICATE.
This error status indicates that one index has a duplicate value on its key and the index has been defined as ZST_NoDuplicates.

  @section ZMFQSAddIdx Adding indexes once file has been populated

  @section ZMFQSModIdx Modifying Indexes

@section ZMFQSSearching Searching for content with a ZMasterFile

ZMasterFile offers the possibility to define and use indexes

@subsection ZMFQSSearchTypes Index searches and free sequential searches

ZMasterFile allows the same free sequential search facilities than its base object ZRandomFile.
It is then possible to fully use these facilities.

Additionnally, ZMasterFile offers a search based upon index key values. Accessing key values using indexes saves drastically time and file accesses compared to free sequential search.
However, there is an priority order between access types to respect :
- first access indexes and establish a collection
- restrict collection using free sequential search (that will only scroll the existing collection content).

So that, there is a medium term to find between the capabilities of free sequential search and speed of access of indexed search.

@subsection ZMFQSIndexAccess Access ZMasterFile using its indexes.

It is possible to search for
- a single key value or for multiple key values i. e. a collection of values
- using an exact key value or a partial key value

@subsection ZMFQSPartialKey Exact key value or partial key value in a search operation

A search may be made with a partial key value, this means that requested key size will represent a partial segment of index key.
In this case, the search operation is made on the partial value of given key content and
- if a simple search operation is invoked, then the first found key value is returned
- if a multiple search operation giving a collection of records ( either zsearchAll or zsearchFirst, zsearchNext )

@subsection ZMFQSZKey Using ZKey object

ZKey is an object dedicated to index key search (see @ref ZKey). It allows to
- store index description (ZMasterFile index number, Key dictionary)
- format and load key value using field dictionary
Field(s) content are then formatted to appropriate key 'internal' format in order to be appropriatedly used.
- create a partial key value in order to search multiple results, or collection of results, from a single partial key value.


@subsection ZMFQSCollection Using collection of records

Searches with potentially multiple results are using collection of results see @ref ZIndexCollection.

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


@subsubsection ZMFCollCreate Creation - destruction of a collection

A collection object is generated during the first time of search operation :
- search all operations : the whole collection object is created and populated
- search first operation and search next operation : the collection object is created during search first operation,
therefore feeded on the flow by successive searches until key content value (exact or partial) is no more found in index.

@note A pointer to the generated collection object is returned (created by 'new').
It is up to the caller to delete this object after having used it.

@subsubsection ZMFCollRecursive Recursive use of collection of records

Collections of records may be used recursively, so that you may for instance :
- first generate a collection of records using an interval search on a key values range,
- refine this collection of records (and only this collection of record) using free search
In this case, collection is mentionned as input collection for the selection method.

@subsubsection ZMFCollLock Collection and record locks

If collection is used in a potential multi-access (ZMasterFile not opened in an Exclusive access mode) then it could be wise to lock selected records.
This could be done either
- in specifying the appropriate lock mask using ZIndexCollection::zlockAll()
- in mentionning the appropriate lock mask while creating collection (see ZIndexCollection::getAllSelected() or ZIndexCollection::getFirstSelected() ).

@note As collection could be recursively refined with several rules, lock on ZMasterFile records are released as soon as they are rejected from current selection (IMPLEMENT).

@subsubsection ZMFCollMass Mass operations using collections of record

Once a collection has been created and populated with a set of record, it is possible to use mass operations on established collections.

This means that collection offers methods that allow to process 'all records at once' for specific operations.
Basically, main operations are :
- get a whole collection of records ZIndexCollection::zgetAllSelected()
- mass deletion of selected records ZIndexCollection::zremoveAll()
- mass lock selected records ZIndexCollection::zlockAll()
Apply a given lock mask to the whole set of selected records
- mass unlock selected records ZIndexCollection::zunlockAll()
- mass change an adhoc field value on all records ZIndexCollection::zchangeField()
Ad hoc field must neither be part of a defined key nor even be a subfield or an overlapping field.

@subsubsection ZMFCollRetry Retry operations on collection after lock status failure

Collection embeds the capability to retry last attempted operation for continuing the engaged mass operation that fails with a locked status on the remaining set of unprocessed records.
When retrying, has to be mentionned
- a number of times to retry
- a number of millisecond to wait after each retry
If operation is still not successfull, returned status is
- ZS_LOCKED is sent back if record cannot be access because it is still locked
- appropriate low level file access status is sent back in case of other error and ZException is set with appropriate message


@subsection ZMFQSInterval Searching index with a range of key values - or Interval search

It is possible to search an index for a range of value.
We must then mention the lowest key value to select index values for and the highest value


@section ZMFQSRemoving Removing content from ZMasterFile


@subsection ZMFQSRemoveSingle Removing a single record

@subsection ZMFQSRemoveMass Removing a set (collection) of records




  @section ZMFQSGetStat Getting statistical data

  @section ZMFQSConverting Converting an existing ZRandomFile to a ZMasterFile.

  @section ZMFQSMaintenance Making some maintenance operations on file

@latexonly
\stopcontents[mainsections]
@endlatexonly
*/




} // namespace zbs

#endif // ZMASTERFILE_QUICKSTART_DOC_H

