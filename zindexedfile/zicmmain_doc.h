#ifndef ZINDEXEDMAIN_DOC_H
#define ZINDEXEDMAIN_DOC_H


namespace zbs {


/**
@mainpage
@author Gerard POLO <gerard.polo@gmail.com>
@copyright GNU Public License

@section MainIntroduction Introduction

The objective is to provide a set of tools, simple, light, fast, well architectured that allows to manage unstructured contents.
No sql access is required. Access is made thru relative positionning, or direct content access either through defined keys or even in the unstructured content itself.

Nowadays a maximum of content, documents, mails, images, are
 - stored remotely on servers and accessed through the net,
 - then loaded in local memory, and stay resident most of the time.

Even if memory tends to be huger and huger, we face strong limitations in terms of resources
either network resources and local resources.

The proposed tools are designed to manage contents with the following main advantages :
- they can be embedded directly in applications : these tools may perfectly work on a mobile phone, as well as a linux or windows server.
- so that they can run locally and store locally documents in a private
- usage of these documents does not consume network bandwidth and can even be done off-line
- the same application may run on various operating system without change
- they are "QT compatible" even if they do not require QT to work fine (no QT libraries are required)
- they are very few resources consumming
- they are quick because written in C++
- they are "manageable", with a set of tools to analyse, maintain, migrate and recover


@section MainWhat What content management are we talking about ?

The idea is to store locally the content we do need the most, in order to skip the gap of network, and this,
without degrading local performance for applications that finally may "starve" from lack of local resources.

Additionally, it is safer because we all know that transitting by the net is not very safe,
and the network encoding itself consumes as well pretty much resources.

Purpose of this tool set is to provide two main functionalities that will be exposed below.

@subsection MainContent Huge contents management with ZRandomFile

We intend to manage huge contents, with a varying size, with an optimized space management.

Access is done per relative position of each content block (record).

But it offers at this stage, full search capabilities using its "field" definition.
What is proposed here is not only "full text" search, but a search on a byte sequence in part of a file (collection of records), or in the total file content.
When can then, for example, search for set of pixel values in various image documents, or a even a machine code segment, if content is executable.

This layer is based on ZRandomFile concept.
@see @ref ZRandomFilePage

@subsection MainIndex Index key search with ZMasterFile

As an extension of ZRandomFile, ZMasterFile offers the possibility to define index keys using dictionaries that are handled and managed by ZMasterFile.
@see DictionaryGroup

Using specific algorithms, it allows fast search thru indexes.

@see @ref ZMasterFilePage

It takes into account Endian (big endian/little endian), for a transparent conversion.
@see EndianConversion

@subsection MainStat Statistical and dump tools
This set of tools offers its own statistical and dump tools to analyze and optimize files behavior, using the numerous options that are provided.
@see ZRFSTATS
@see ZMFSTATS

@subsection MainRepair Maintenance and repair tools
Tool set offers maintenance tools that allow to manage disk space, as well as to recover damaged file structure.
@see ZRFUtilities
@see ZMFUtilities

@section MainArchitecture Software architecture

It is always utmost complicated to make a tool simple and effective.
This was the challenge I faced.

I tried to make it in designing an architecture that was technically obvious, effective in terms of response times.

@subsection MainZBSToolset zbasesystem toolset

This toolset is based on zbasesystem toolset (OpenSource library.Author: Gerard POLO).
This toolset allows us, among other numerous usefull objects, to :
- ZArray :manipulate Arrays in a very usefull way, with embedded capabilities to export and import arrays to store them as flat content in a file's record.
- ZDataBuffer : manipulate any content as a piece of memory, with its own safer memory allocation/extension/deallocation, as well as encoding/decoding facilities.
@see @ref ZBaseSystemPage
@note
OpenSSL library software is required, specifically for encoding

@subsection MainZRFArch ZRandomFile Architecture
@copydoc ZRFPhysical

@subsection MainZMFArch ZMasterFile Architecture
@copydoc ZMFPhysical

@subsection MainLock ZICM Locks management and usage
@copydoc MainLockGroup


@section MainZRFZMFToDoList To Do list for ZRandomFile and ZMasterFile ZIndexFile

@copydoc ZICMToDoList

@subsection MainZRFToDoList ZRandomFile Todo list
@copydoc ZRFToDoList

@subsection MainZMFToDoList ZRandomFile Todo list
@copydoc ZMFToDoList

@section MainTest Tests plans
@subsection MainZRFTestPlan ZRandomFile Tests plan
@copydoc ZRFTestPlan

@subsection MainZMFTestPlan ZMasterFile Tests plan
@copydoc ZMFToDoList

*/

/**
 @defgroup MainLockGroup Record & file locking

 @subsection MLockFile File locking

    The whole file could be locked using its file header using one of the lock mask. This lock will apply to the entire file.

    Only one user-process can own a lock on a whole file at a time.



  @subsection MLockRecord ZMF record locking

 - Adding, suppressing, Modifying indexes
    File must be open in exclusive mode with read write capabilities : ZRF_Exclusive | ZRF_Write
    No lock is necessary.
    Must take care of processes opening the file in file header ZReserved

 - Creating a ZMF record
    no special lock

 - removing ZMF record
    - prepare : main file's record is locked at  ZLock_Write | ZLock_Delete

    - rollback : lock is released on the record in the end of rollback and lock is destroyed in the file header's lock table

    - commit : lock is destroyed in the file header's lock table. while marking free block lock is cleared on block.

 - modifying a record :

    - get : an exclusive lock is put on record : ZLock_All

    - rollback lock is released on the record in the end of rollback and lock is destroyed in the file header's lock table
    - commit lock is released on the record in the end of rollback and lock is destroyed in the file header's lock table


 - Additionally locks can be manually put on records using zlock , zunlock : users manages the lock / unlock

 - in case of failure : a full unlock frees all locks for a file

 - restart after error : locks may be destroyed.




*/

/**
  @defgroup MainFileAccess File access information table  (FAIT)

 This table registers the users / process id currently access the file.

 This table is stored in FileHeader's ZReserved zone  (for a ZMF only main file is concerned by this table)

 It mentions :

 - username
 - process id
 - open date-time
 - open mode



*/
} // zbs

#endif // ZINDEXEDMAIN_DOC_H

