#ifndef ZICM_TODO_TEST_H
#define ZICM_TODO_TEST_H
namespace zbs {

/**
@defgroup ZICMToDoList ZICM Todo list

- xml file definition
    + export from existing file
    + create empty files structure from xml definition (parse and control, then create)

- create a command line utility project <zicmcmd>
    + export xml definition
    + zreorg
    + zheader rebuild
    + zcreate
    + ztrunctate
    + zextend
    + Dump and reports

- migrate a ZRandomFile to a ZMasterFile



@defgroup ZRFToDoList ZRandomFile Todo list

- Implement zreorg (done) &radic;
- zreorg static (done) &radic;
- test zreorg

- Implement option zmode exclusive : impacts on rewrite of file header : (done)&radic;
No access to file header when it is open in exclusive mode.
Access is only done in 2 times :
    at open time    (read file header)
    at close time   (write file header)
This should be transparent : in _getFileHeader and in _writeFileHeader.
(add a boolean for effective read and effective write)

- Implement ztruncate (done) &radic;

- Implement posix_fallocate at creation time (done) &radic;

- implement zcloneFile  (done) &radic;
duplicates the file structure and the data
- same structure, same parameters
- however copy is done in reorganizing blocks in the order of relative ranks
- size is set using : used size + pFreeBlock (fallocate)
Only one free block of size pFreeBlock is added at the end of physical file
if pFreeBlocks = -1 (default) then BlockTargetSize is taken.

- implement zreogExternal : of no use, zcloneFile is better

- implement zheaderRebuild   (done) &radic;
    + scan surface for content file and report
    + create header file
    + search physical block after physical block :
        - block is ZBS_Used create an entry in ZBAT
        - block is ZBS_Free create an entry in ZFBT
        - block is ZBS_Deleted create an entry in ZDBT

    + test it (done) &radic;
*/

/**
@defgroup ZMFToDoList ZMasterFile Todo list

- implement ZIndex zrebuildIndex (done) &radic;

- implement zreorgFile for ZMasterFile (done) &radic;
- test it (done) &radic;

- implement ZPMS as a pointer to zstatistics

- implement ZKey facitilies
    + ZKey field extraction for query
     - ZType analyzis for fields (done) &radic;
     - fields value extraction and key setup with content (done) &radic;
     - define partial key capabilities
    + ZKey integration into ZMasterFile ZIndexFile usage

- implement in ZType_type "Byte sequence" : ByteSeq

- implement zsearch
        Search the first occurrence of a key value and return the corresponding ZMF address

- zgetByKey (index rank, key value)
        returns the first occurrence of ZMF record that matches the given key value for fiven index rank

- implement zsearchAll (done) &radic;
        searches all occurrences of key value and return a ZArray with the matching ZMF addresses

- implement zgetByAddress(done) &radic;
        returns a ZMF record using its address


- implement zsearchAllPartial  & zsearchNextPartial
    + raw with ZDataBuffer
    + using ZKey : implement operators

- implement zsearchNextPar


- implement ZRandomFile to ZMasterFile conversion
    immediate conversion : no need of special conversion.

- implement zremoveByKey
Remove a record or a set of records using a key value
- implement zremoveByAddress

-

- journaling for ZMF
    + define journaling class as ZRandomFile
    + define journaling life cycle : creation, storage, purge

- remote mirroring for ZMF (from journaling)
    + store mirroring info in ZReserved.
    + define parameters : port, host, username, password

    + define ZMF mirroring service on target host.



 */ // todo

/**
@defgroup ZRFZMFTestPlan Tests plans for ZRandomFile and ZMasterFile ZIndexFile
@{
@defgroup ZRFTestPlan ZRandomFile Tests Plan

@par fixed size records

   - create records (done) &radic;
   - random access to records by relative position (done) &radic;

   - delete 3 consecutive records  (done) &radic;
      verify grabFreeBlocks works fine (done) &radic;
   - verify insert (done) &radic;


@par variable sized records

   - create records    (done) &radic;
   - monitor time of load for huge files (done) &radic;
   - test file structure dump (done) &radic;

   - random access to records (done) &radic;

   - delete 3 adjacent records
      verify free pool status and verify state of physical file surface (VSPFS) (done) &radic;
         - in growing order (done) &radic;
         - in random order (done) &radic;

   - recover from Free pool (done) &radic;

   - create 3 records after deletion
       verify space is correctly taken from free pool  (done) &radic;


   - delete 3 adjacent records :  (verify grabFreeBlocks works fine)
         - delete record physical address 0 (done) &radic;
         - delete record last physical address (done) &radic;
         - and create new records to fill in it (done) &radic;
         - verify grab free space behaviour (done) &radic;

@par Dump and Statistical tools

   - Test all content dump tools (done) &radic;
   - Test surface scan  (done) &radic;
   - Test ZRFstat  (done) &radic;

@par Utilities and HighwaterMarking

   - zclearFile without highwater (done) &radic;
   - zclearfile with highwater (multiple modulos)  (done) &radic;
   - zreorg
    + create a test data set with unorganized blocks, and with holes (done) &radic;
    + test zreorgFile
        - without grabFreeSpace  (done) &radic;
        - with grabFreeSpace (done) &radic;
        - with highwaterMarking (done) &radic;
        - performance tests (done) &radic;
    - zheaderRebuild (done) &radic;

    - export xml definition
        + for ZMasterFile (done) &radic;
        + for ZRandomFile (done) &radic;

    - export xml content

    - create from xml (done) &radic;


*/

/**
@defgroup ZMFTestPlan   ZMasterFile Tests plan

@subsection ZMFTPZIK ZMasterFile Index key

@par Index key data structure

   - Create ZMasterFile structure (done) &radic;
   - Create associated ZIndexKey (done) &radic;
   - Populate it with appropriate variable sized records (done) &radic;
   - Verify key content in dumping ZIndexFile key (done) &radic;

@par Endianness management

   - Atomic data
    - long (done) &radic;
    - float (done) &radic;
    - double (done) &radic;
   - Array of
        - long (done) &radic;
        - float (done) &radic;
        - double (done) &radic;

   - Key negative atomic values (done) &radic;
   - Key arrays of char (done) &radic;
   - Key arrays of other atomic data values (done) &radic;
   - Key class

@subsection ZMFTPSearch Searches

@par Key content:
    - printout Keys content whatever data type is (done) &radic;
    - rebuild zindex key(done) &radic;

    Need to test zindexRebuild (done) &radic;

@par ZKey facilities test plan
    + ZKey field extraction for query
     - ZType analyzis for fields
     - fields value extraction and key setup with content
     - define partial key capabilities
    + ZKey integration into ZMasterFile ZIndexFile usage

@par Using key pointers in seek and search
    - pointer of char
    - pointer of unsigned char
    - pointer to other atomic data value


@par Key content:
    - printout Keys content whatever data type is (done) &radic;
    - rebuild zindex key(done) &radic;

    Need to test zindexRebuild (done) &radic;

@par Statistics
    - test ZMasterFile / ZIndexFile performance statistics (done) &radic;

@par utilities and xml structure


    - zrepairIndexes (done) &radic;
    - zclearMCB (done) &radic;

    - export various xml definitions (done) &radic;

    - zapplyXMLFileDefinition (done) &radic;
    - zapplyXMLIndexDefintion (done) &radic;


@par Searches

    - search of field :
        + zsearchAll
        + zsearchNext
        + zsearchAllCollection
        + zsearchNextCollection

    - zsearch on no duplicate key on duplicate key
    - zsearchAll on duplicate key, on no duplicate key
    - zsearchAllPartial
    - zsearchNextPartial
    - zsearchAllPartialField
    - zsearchNextPartialField

- Test Journaling

 */

/** @} */ // global test plan

}// zbs
#endif // ZICM_TODO_TEST_H

