#ifndef ZRandomFile_QUICKSTART_DOC_H
#define ZRandomFile_QUICKSTART_DOC_H

namespace zbs {

/**
  @page ZRFQuickStart Starting up using ZRandomFile
@tableofcontents
@latexonly
\startcontents[mainsections]
\printcontents[mainsections]{l}{1}{\section*{Main Sections}\setcounter{tocdepth}{2}}
\clearpage
@endlatexonly

@section ZRFQSIntro Introduction - Example presentation

 This chapter is a user's guide concerning the use of ZRandomFile.

 We will follow an example all accross this chapter with a simple data definition.
 Indeed this example is silly, and not realistic. But this is for the purpose of showing usage and possibilities of ZRandomFile methods.

 In this example, we will
 - load in a ZRF a mix of structured data and informal, varying length data with various format.
 - feed ZRF with data
 - remove records
 - search for data using field definition capabilities and ZRFCollection object
 - use some ZRF utilities

@subsection ZRFQSRequired Required include files

@code
//------- for ZBaseSytem-------------

#include <config/zconfig.h>
#include <ztoolset/ZArray.h>
#include <ztoolset/zerror.h>
#include <zrandomfile/zrandomfile.h>

#include <zrandomfile/zrfcollection.h> // for using ZRFCollection objects

//------- for data types handling-----

@endcode

@subsection  ZRFQSDataDef Data definitions

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

// our structured 'meta data' for document records

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

  @section ZRFQSInstantiate Instantiating ZRandomFile

  Instantiating ZRandomFile object doen't create anything on storage.

  It simply creates an object with the memory structure within the process ready to be used either to create a ZRF or to open a ZRF.

  You may specify a file path at this stage with an uriString, but this is not mandatory.
  At anytime BEFORE having opened ZRF you may set or change this path using ZRandomFile::setPath().

 @subsection ZRFQSInsV1 version 1

@code
uriString wPath;
    wPath = "/home/gerard/testindexedpictures.zrf";
    ZRandomFile ZPicture(wPath);
@endcode

 The main file path should already have been tested and stored.

 This can be done using uriString facilities.

 But in this case, if the path is not a valid path, then ZRandomFile constructor aborts application.

 So, we do prefer the following.

 @subsection ZRFQSInsV2 version 2

@code
    ZRandomFile ZPicture ;
@endcode

  @section ZRFQSCreating Creating a ZRandomFile

  We will create an empty ZRandomFile structure with a free available space (contiguous best try) of 100000 bytes.

  This is the most simpler way to create a ZRandomFile structure.

 Excepted mandatory data, meaning file path and initial file space, all other parameters are defaulted.

@code
    // we do not want to leave the file open (pLeaveOpen is defaulted to false)

    wSt=ZPicture.zcreate("/home/gerard/testindexedpictures.zrf",100000 );
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();  // stop processing and dump exception stack
@endcode

  we chose an extension name of ".zrf" but this is NOT mandatory. Any extension is valid excepted the reserved ones.
  see:@ref FNPZRFName.


  @section ZRFQSSetting Setting ZRandomFile options

  At this stage, ZRandomFile structure is created including on storage, with all necessary infra-data to work with (mostly set to their default values).

  By default, grabFreeSpace is set to on (true), and highWaterMarking is set to off (false).

  AllocatedBlocks and BlockExtentQuota,the pool tables size parameters, are set to their defaulted parameters (_cst_default_allocation and _cst_realloc_quota)
  see: @ref ZArray

  We could change that (see further).


  But one rather important parameter is BlockTargetSize.
  We will set it to a value (this is not mandatory, but wishable).

 @note We remark that zcreate method has set up internal ZRandomFile data structure.
Even if the file has been closed, this data remains available and does not need to be mentionned again.
Using this facility, the first time we create or open a file, we have to mention required infradata for the given file.
Successive sessions (open/close) could be done without mentionning again this infradata.

@subsection ZRFQSSetParams Setting some options and openning the file

We will now set two optional parameters of the file : BlcokTargetSize and GrabFreeSpace.
When it has been done, we will open the file for writing.

 @note Setting an option after the file has been created may be done while the file is open or close, it has no importance.
 However, if the file is closed it may point to a valid file path : file path may have been previous set using setPath() method.
 During parameter(s) setting, file will be openned in ZRF_Exclusive mode.
 This means that if file is accessed by another user and/or process, this action will fail.
@code
    wSt=ZPicture.setBlockTargetSize( 100000 ); // it can be done while file is closed. File is opened then closed once finished
    if (wSt!=ZS_SUCCESS)                       // Warning for eventuality of file lock in between
                ZException.exit_abort();
    if (ZPicture.setGrabFreeSpace(true)!=ZS_SUCCESS)        // opens the file with exclusive access and closes it once finished
                                   ZException.exit_abort();
@endcode


@subsection ZRFQSLoad Loading documents and meta data into ZRandomFile

@code
    if ((wSt=ZPicture.zopen(ZRF_Write|ZRF_Exclusive))!=ZS_SUCCESS) // open to load documents : exclusive write. Not really necessary here. Only ZRF_Write is required.
                {
                    ZException.exit_abort();
                }

   for (int wi = 0;wi < 11;wi++)
        {
            memmove(&wT3 , &T3Cst[wi], sizeof(T3));     // get the occurrence of T3Cst

            wURIPicture = PictureFiles[wi];         // set document file path
            //----------Hereafter : test the extension file to define the content format -----------
            // NB: this is very raw and not corresponding to real world (see Mime types)

            if ((wURIPicture.getFileExtension()=="jpg")||(wURIPicture.getFileExtension()=="JPG"))
                    wT3.Type = DT_Jpeg ;
                else
                if ((wURIPicture.getFileExtension()=="mp4")||(wURIPicture.getFileExtension()=="MP4"))
                        wT3.Type = DT_Mp4 ;
                else
                    if ((wURIPicture.getFileExtension()=="pdf")||(wURIPicture.getFileExtension()=="PDF"))
                            wT3.Type = DT_Pdf ;
                else
                        if ((wURIPicture.getFileExtension()=="html")||(wURIPicture.getFileExtension()=="HTML"))
                                wT3.Type = DT_Html ;
                else
                    wT3.Type = DT_Other ;
            wRecord.setData( &wT3,sizeof(wT3));                       // move it to our user's record ZDataBuffer
            if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) // load document (must exist!)
                        {
                         ZPicture.zclose();
                         ZException.exit_abort();
                        }
            wRecord += wPicture;                    // append document content to record buffer
            if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) // load document
                        {
                        ZPicture.zclose();
                        ZException.exit_abort();
                        }
        }// for
@endcode

@section ZRFQSSearching Searching for content using adhoc defined fields

The masterpiece of free sequential selection and access on a ZRandomFile is ZRFCollection object.
We will so define a ZRFCollection that will hold and manage our request.

This collection will be recursively used and refined in changing the selection criterias.

This will be done for
- a string selection
- a selection on a float

@subsection ZRFQSSearchString Selection on adhoc fields considered as Cstring

We will take advantage of field String1 from T3 structure, and search as a string a certain substring
contained somewhere in the defined field.

Then we will refine this search using another substring.

@subsubsection  ZRFQSStr1 Establish the first collection of records ranks
The first search will be a global search, searching at once for all occurrence of record ranks satisfying the selection criteria.

@note In the code below, we may imagine that the opened file is a ZMasterFile.
A ZMasterFile could be opened as a ZRandomFile under the condition that the open mode is ZRF_ReadOnly.
The reason is :
- ZRandomFile preserves ZHeader::ZReservedBlock into which ZMasterFile infra data is written.
- But no change to data is allowed since change to data content will induce a discrepence into ZMasterFile defined indexes

see @ref ZRFZFile_type

@code
T3 * wT31;
long wRank;
ZDataBuffer wRecord;

ZDataBuffer wSearchSequence;    // will contain byte sequence to search for
ZRFCollection wCollection; // Collection object
wSearchSequence = "BeAt";       // set the search sequence with compount upper/lower letters
ssize_t wOffset = offsetof (T3,String1);    // define offset for adhoc field to search into
ssize_t wLength = sizeof(T3::String1);      // idem for length

ZRandomFile ZRFPicture;

   wSt=ZRFPicture.zopen("/home/gerard/testindexedpictures.zrf",ZRF_Read_Only);

// establish a first collection : records with String1 containing the string "BeAt" Case regardless.

   wSt= ZRFPicture.zsearchFieldAllCollection (wOffset, wLength,wSearchSequence,wCollection,ZRFContains|ZRFString|ZRFCaseRegardless);

   fprintf (stdout,
            " idx   Rank    String1                          Numeric      Type\n");
   for (long wi=0;wi<wCollection.size();wi++)
   {
       wSt=ZRFPicture.zget(wRecord,wCollection[wi],ZLock_Nolock);
       if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
       wT31 = (T3*)wRecord.Data ;
       fprintf (stdout,
                " %3ld > %3ld : <%50s> <%f> <%f> <%f> <%d>\n",
                wi,
                wCollection[wi],
                wT31->String1,
                wT31->NumericField[0],
                wT31->NumericField[1],
                wT31->NumericField[2],
                (int)wT31->Type);
   }//for

@endcode

The following comes out :

@code
idx   Rank    String1                          Numeric      Type
   0 >   0 : <                                    my heart beats> <110.000000> <-155.000000> <1.000000> <2>
   1 >   1 : <                            my heart beats for you> <110.000000> <-1024.000000> <1.000000> <2>
   2 >   2 : <                              and so beats my skin> <110.000000> <-152.000000> <1.000000> <2>
@endcode

@subsubsection  ZRFQSStr2 Refine the search
In this example, we will use the same adhoc field to refine the search using another substring to search for.
But it is possible to change even the adhoc field definition, pointing to another zone of the record.

This second search will be a dynamic search, searching one per one  record ranks satisfying the selection criteria,
and giving back control to calling procedure each time a new rank has been found.

@code


   wSearchSequence = "HeaRt"; // a new substring to find out

   fprintf (stdout,
            " idx   Rank    String1                          Numeric      Type\n");

// the two instructions below are equivalent to ZRandomFile::zsearchFieldFirstCollection(...)

   wCollection.reset();    // partial reset (Keeping ranks collection)
   wRank= ZRFPicture.zsearchFieldNextCollection  (wOffset,
                                               wLength,
                                               wSearchSequence,
                                               wCollection,
                                               ZRFContains|ZRFString|ZRFCaseRegardless,
                                               &wCollection);
    wi =0;
   while (wRank>-1)
       {
           wSt=ZRFPicture.zget(wRecord,wRank,ZLock_Nolock);  // get the corresponding record
           if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
           wT31 = (T3*)wRecord.Data ;

           fprintf (stdout,                                 // report content
                    " %3ld > %3ld : <%50s> <%f> <%f> <%f> <%d>\n",
                    wi,
                    wCollection.last(),
                    wT31->String1,
                    wT31->NumericField[0],
                    wT31->NumericField[1],
                    wT31->NumericField[2],
                    (int)wT31->Type);

           wRank= ZRFPicture.zsearchFieldNextCollection (wOffset,  // search for next record rank
                                                       wLength,
                                                       wSearchSequence,
                                                       wCollection,
                                                       ZRFContains|ZRFString|ZRFCaseRegardless,
// we may mention wCollection here above as there is a safe copy made during initialisation of request
                                                       &wCollection);
           wi ++;
       }//while

@endcode

The following comes out :
@code
 idx   Rank    String1                          Numeric      Type
   0 >   0 : <                                    my heart beats> <110.000000> <-155.000000> <1.000000> <2>
   1 >   1 : <                            my heart beats for you> <110.000000> <-1024.000000> <1.000000> <2>
@endcode

@subsection ZRFQSSearchFloat Selection on adhoc binary fields

By default, any adhoc field is considered as a binary set of bytes.
We will now search for a float value, then refine this search.

@subsubsection  ZRFQSFloat1 Establish the first collection of records ranks

@code
   wOffset = offsetof (T3,NumericField);
   wLength = sizeof(float)*3;
   float wValueToSearch = 110;
   wSearchSequence.setData(&wValueToSearch, sizeof (wValueToSearch));

// Note : searching all fields start with an initialisation of ZRFCollection
   wSt= ZRFPicture.zsearchFieldAllCollection  (wOffset,
                                               wLength,
                                               wSearchSequence,
                                               wCollection,
                                               ZRFContains,
                                               nullptr);
// hopefully all records will be selected

   fprintf (stdout,
            " idx   Rank    String1                          Numeric      Type\n");
   for (long wi=0;wi<wCollection.size();wi++)
       {
           wSt=ZRFPicture.zget(wRecord,wCollection[wi],ZLock_Nolock);
           if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
           wT31 = (T3*)wRecord.Data ;

           fprintf (stdout,
                    " %3ld > %3ld : <%50s> <%f> <%f> <%f> <%d>\n",
                    wi,
                    wCollection[wi],
                    wT31->String1,
                    wT31->NumericField[0],
                    wT31->NumericField[1],
                    wT31->NumericField[2],
                    (int)wT31->Type);
       }//for
@endcode
The following comes out :
@code
 idx   Rank    String1                          Numeric      Type
   0 >   0 : <                                    my heart beats> <110.000000> <-155.000000> <1.000000> <2>
   1 >   1 : <                            my heart beats for you> <110.000000> <-1024.000000> <1.000000> <2>
   2 >   2 : <                              and so beats my skin> <110.000000> <-152.000000> <1.000000> <2>
   3 >   3 : <                                              xxxx> <110.000000> <110.000000> <1.000000> <2>
   4 >   4 : <                                       this is one> <1.000000> <110.000000> <1.000000> <2>
   5 >   5 : <                                           aaaa   > <34.000000> <110.000000> <1.000000> <2>
   6 >   6 : <                                               eer> <5.000000> <110.000000> <1.000000> <4>
   7 >   7 : <                                              dddd> <250.000000> <110.000000> <1.000000> <1>
   8 >   8 : <                                        jkfdlsakfj> <2025.000000> <110.000000> <1.000000> <6>
   9 >   9 : <                                            egadfk> <25.000000> <110.000000> <1.000000> <2>
  10 >  10 : <                                           aaaa   > <12045.000000> <110.000000> <1.000000> <2>
@endcode

As all records have somewhere in their array of floats the requested value, we have all records rank selected in the collection.

@subsubsection  ZRFQSFloat2 Refine the search

@code
   wValueToSearch = -1024;
   wSearchSequence.setData(&wValueToSearch, sizeof (wValueToSearch));
   wSearchSequence.Dump();  // just Dump ZDataBuffer content to have an idea of internal representation
   wSt= ZRFPicture.zsearchFieldAllCollection  (wOffset,
                                               wLength,
                                               wSearchSequence,
                                               wCollection,
                                               ZRFContains,
                                               &wCollection); // again here we may use wCollection for a recursive search
   fprintf (stdout,
            " idx   Rank    String1                          Numeric      Type\n");
   for (long wi=0;wi<wCollection.size();wi++)
       {
           wSt=ZRFPicture.zget(wRecord,wCollection[wi],ZLock_Nolock);
           if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
           wT31 = (T3*)wRecord.Data ;

           fprintf (stdout,
                    " %3ld > %3ld : <%50s> <%f> <%f> <%f> <%d>\n",
                    wi,
                    wCollection[wi],
                    wT31->String1,
                    wT31->NumericField[0],
                    wT31->NumericField[1],
                    wT31->NumericField[2],
                    (int)wT31->Type);
       }//for
   ZRFPicture.zclose();
@endcode
The following comes out :
@code
Offset  __________________HexaDecimal___________________  _____Ascii______
     0 |00 00 80 C4                                      |....            |
ZArray::allocate-E-ERRALLOC requested new allocation <10> not greater than existing number of elements within ZArray <11> .
You should suppress elements from the ZArray and use trimming facility to reduce the memory allocated (ZArray::trim).
 idx   Rank    String1                          Numeric      Type
   0 >   1 : <                            my heart beats for you> <110.000000> <-1024.000000> <1.000000> <2>

@endcode

@note ZArray complains :
- because __ZRFVERBOSE__ have been set to another value than ZVB_NoVerbose
- because while copying the collection the defaulted allocation values are set with a value less than ZArray actual content.
ZArray refuses to adjust the space as it is requested to do.

@warning This example is obviously not usable in real world : searching for a float value in an array of float considered as a binary sequence of bytes
may be fooled and subject to hazardeous match.


@subsection ZRFQSGetXML Getting file's infra-data in XML format

This is a wise policy to generate and keep an xml definition of ZRandomFile infra-data, as soon as it has been created.
This definition may evolve, and then it is recommended to keep versions of these definitions.

 We will now get XML infra-data definition for the file.
@code
    ZPicture.zwriteXML_FileHeader(); // leave ZRandomFile generate its own file according xml file name generation rules
@endcode

This routine works either as a static routine, or could be invoked to generate the currently open ZRandomFile as a non static routine.

In the working directory, we can find a file called <testindexedpictures.xml>

 It contains our ZRandomFile infra-data definition in xml format.

@code
<?xml version='1.0' encoding='UTF-8'?>
<zicm version="2.00">
<File>
  <ZFileDescriptor>
      <URIContent>/home/gerard/testindexedpictures.ZRF</URIContent>
      <URIHeader>/home/gerard/testindexedpictures.zrh</URIHeader> <!-- not modifiable generated by ZRandomFile-->
      <URIDirectoryPath>/home/gerard/</URIDirectoryPath> <!-- not modifiable generated by ZRandomFile-->
      <ZHeaderControlBlock> <!-- not modifiable -->
          <FileType>ZFT_ZRandomFile</FileType>  <!-- do not modify : could cause data loss  see documentation-->
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
</File>
</zicm>
@endcode


  @section ZRFQSRemoving Removing content from ZRandomFile

  @section ZRFQSGetStat Getting statistical data
  @copy ZRFStats

  @section ZRFQSConverting Converting an existing ZRandomFile to a ZMasterFile.

  At any moment, it is possible to upgrade a ZRandomFile to a ZMasterFile.
  Why do such a conversion ?
  - because a ZRandomFile is not capable of maintaining Journaling facilities.
  - because you may create indexes.

  @note creating indexes for a ZMasterFile is not mandatory.
It is always possible to continue using it a ZRandomFile while taking benefit from Journaling.
  @section ZRFQSMaintenance Making some maintenance operations on file

@latexonly
\stopcontents[mainsections]
@endlatexonly
*/
// ZRFQuickStart



} // namespace zbs

#endif // ZRandomFile_QUICKSTART_DOC_H

