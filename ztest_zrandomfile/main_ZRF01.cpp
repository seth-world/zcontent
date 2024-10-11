/*
 * Gérard POLO 09/11/2015
 *
*
 *
 *
 */


/** @defgroup testPlan    ZRF Test plan

    fix sized records

        create records
        random access to records

        delete 3 consecutive records : verify grabFreeBlocks works fine
        verify insert


    variable sized records
         create records                                                                                      done
         monitor time of load for huge files                                                                 done

         random access to records

         delete 3 adjacent records       : verify free pool status and verify state of physical file surface (VSPFS)  done
         in growing order                                                                                    done

         recover from Free pool                                                                                 done

         create 3 records after deletion   : verify space is correctly taken from free pool                  done


         delete 3 adjacent records :  verify grabFreeBlocks works fine

         delete record physical address 0
         delete record last physical address
         and create new records to fill in it
         verify grab free space behaviour





  Test history

  Test Journaling




  */


#define __MAIN__


#include <QCoreApplication>

#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stddef.h>

#include <zconfig.h>

#include <ztoolset/zarray.h>

#include <ztoolset/zsortedarray.h>

#include <ztoolset/zerror.h>

#include <zio/zdir.h>

#include <ztoolset/zsafiltered.h>

//#include <ztoolset/zkeymanager.h>


#include <ztoolset/ztoolset_common.h>
#include <ztoolset/zam.h>

#include <ztoolset/zjoinstatic.h>
#include <ztoolset/zjoindyn.h>


#include <zrandomfile/zrandomfiletypes.h>
#include <zrandomfile/zrandomfile.h>


#include <ctime>
#include <sys/time.h>

/** T1 : simple cool example  : T1 is the class definition for ZSA.  T1 is also used as 'bulk' structure because its key field is named _Key */

struct Keystruct {
    char KeyContent[50];
};

struct T1 {
           Keystruct String1  ;
           long AtomicField  ;
};


struct T2 {
    Keystruct String1  ;
    long AtomicField;

};

CUF_struct<T1> myCUF;   //! for CommitUserFunction

 ZCommitStatus CUFunction(const T1&ZAMRow,ZOp pOperation,void *pData)
 {
ZCommitStatus wZCS;
    char *wData =(char *)pData;

    if (pOperation==ZO_Insert)
                printf ("Insert found\n");
    wZCS=ZCS_Success;

     return(wZCS);
 }

int main(int argc, char *argv[])
{
_MODULEINIT_ ;

/**
  *  ZAM test plan:
  *
  *         - 2 ZIndexes each time : list in both index order
  *
  * Multiple ZIndex updates (concurrent indexes )                           x
  * create an Index dynamically after having populated the ZAM  (rebuild)   x
  *
  *  question the ZIndex on _Key field      (getKeyInformation)
  *
  *  std::string as _Key type
  *         - _getDataType()                    x   for natural std::string type and pointers std::string*  (isStdString = true)
  *         - Compare routine                   x
  *
  *
  * update
  *         - ZAM::insert                       x
  *         - ZAM::push                         X
  *         - ZAM<<value                        x
  *         - ZAM::push_front                   x
  *         - ZAM::pop                          x
  *         - ZAM::popRP                        x
  *         - Result << ZAM  (popRP)            x
  *         - ZAM::pop_front                    x
  *         - ZAM::erase                        X
  *         - --ZAM                             x This is equivalent as pop_front with a returned ZStatus
  *
  *         - ZIx::add                          x
  *         - ZIx::remove                       x
  *         - ZIx::removeAll                    x   TO BE CAREFULLY WATCHED (because of ZIX update vs searchAll index collection
  *                                                 This is not usable with concurrent (THREADS) usage OR whole ZIX must be locked during removeALL operation
  * search
  *         - ZIx::seek                         x
  *         - ZIx::seekAll                      x    (see removeAll)
  *         - ZIX::seekAllPartial               x
  *
  * Journaling
  *
  *         - ZAM::commit                       x
  *         - ZAM::rollback -> given ZAMMark    x
  *         - ZAM::rollback a number of events  x
  *         - ZAM::rollback total
  *         - print  events journal             x
  *         - print journaling history          x
  *
  *  Error management :
  *         - dupkeyviolation on index rebuild   x      (abort on fatal)
  *         - dupkeyviolation on push            x      (rollback erase on indexes- journal & history update accordingly)
  *         - duplkeyviolation on push_front     x      (rollback erase on indexes- journal & history update accordingly)
  *         - dupkeyviolation on insert          x      (rollback erase on indexes- journal & history update accordingly)
  *         - error on ZAM::erase                x      (can only be out of ZAM boundaries)
  *         - error on ZIX::remove               x      (will also test removeAll) Added historization from ZIX::remove error
  *         - error on ZAM::replace              x      collision with ZIX existing key while ZST_NODUPLICATES (test historization from ZIX)
  *
  *         - CUF during commit                  x      User Data & Row content & ZOp accessible - Return ZCS_Error Return ZCS_Fatal
  *                                                     Row data non changeable
  *
  *   LATER
  *         - rollback with memory (rollback keeps its own journaling)
  *         - recommit rolled back journal (using rollback journaling)
  *   LATEREND
  *
  *
  */





    zbs::ZData_Type_struct ZDT ;
    zbs::_get_Data_Type<std::string>(&ZDT);

    zbs::_get_Data_Type<std::string*>(&ZDT);


    const size_t _Max_T1Cst = 11;
    const size_t _Max_T2Cst = 12;
    size_t wi;

    zbs::ZResult wRes;
    ZStatus wSt ;
    char wBuf[50];


    struct T1 T1Cst[] =
    {
    {"my heart beats"        ,  155 },       // row with multiple correspondance within Slave
    {"my heart beats for you",  1024},      // no correspondance within slave
    {"and so beats my skin",    152} ,
    {"xxxx",                    110},         // duplicate this row on ZIX1 key string
    {"this is one",             1 },
    {"aaaa   ",                 34 },
    {"eer",                     5},
    {"   dddd",                 250},
    {"jkfdlsakfj",              2025},      // row with no correspondance within Slave
    {"egadfk",                 25},
    {"aaaa   ",                12045 }
    };


    struct T2 T2Cst[] =
    {
    {"my heart beats"        ,  15500 },       // row with multiple correspondance within Slave
    {"my heart beats",         102400},
    {"my heart beats",          102401},
    {"and so beats my skin",    15200} ,
    {"xxxx",                    11000},
    {"this is one",             100 },
    {"aaaa   ",                 3400 },
    {"eer",                     500},
    {"   dddd",                 25000},
    {"jkfd",                     202500},       // row with no correspondance within Master
     {"egadfk",                 2500},
     {"aaaa   ",                1204500 }       // row with multiple correspondance within Master and slave (n , n) relationship
    };



    // +++++++++++++++Define the _Bulk structure for K1++++++++++++++++++++++
    #ifdef __GNUC__
            T1 T1_Obj ;
            constexpr const  size_t K1_Keyoffset = offsetof(T1,String1);
            constexpr const  size_t K1_remaining = sizeof(T1) - sizeof(T1_Obj.String1) - K1_Keyoffset ;

       #else
        #ifdef __clang__
            #pragma message("WARNING: clang is used and not GNU gcc. You have to define your own _Bulk structures - use gcc instead")
            #pragma message(__clang)
        #else
            #pragma message("WARNING: An unknown c++ compiler is used and not GNU gcc. You have to define your own _Bulk structures - use gcc instead")
        #endif
        constexpr size_t K1_Keyoffset=0;
        constexpr size_t K1_remaining=8;


    #endif

//!  create the main array

    zbs::ZAMain<T1> ZAM1 ;

//! First key will be C string String1
//!

    struct K1_Bulk
    {
        char K1_Filler1[K1_Keyoffset];
        Keystruct _Key;
        char K1_Filler2[K1_remaining];
    };



    // +++++++++++++++Define the _Bulk structure for T2++++++++++++++++++++++
    #ifdef __GNUC__
            T2 T2_Obj ;
            constexpr const  size_t K2_Keyoffset = offsetof(T2,String1);
            constexpr const  size_t K2_remaining = sizeof(T2) - sizeof(T2_Obj.String1) - K2_Keyoffset ;

       #else
        #ifdef __clang__
            #pragma message("WARNING: clang is used and not GNU gcc. You have to define your own _Bulk structures - use gcc instead")
        #else
            #pragma message("WARNING: An unknown c++ compiler is used and not GNU gcc. You have to define your own _Bulk structures - use gcc instead")
        #endif
        constexpr size_t K2_Keyoffset=0;
        constexpr size_t K2_remaining=8;

    #endif
    struct K2_Bulk
    {
        char K2_Filler1[K2_Keyoffset];
        Keystruct _Key;
        char K2_Filler2[K2_remaining];
    };

    //!  create the main array

        zbs::ZAMain<T2> ZAM2 ;


//!
//! ensure that journaling is turned on (for example purpose because this is the default option).
//!
        ZAM1.setJournaling(zbs::ZJManualCommit);  // lets do commit by ourselves
//!
//! populate main array with half of the constants
//!
    for (size_t wi=0;wi< _Max_T1Cst ;wi++)
                {
                ZAM1<<(T1)T1Cst[wi] ;
                }

    for (size_t wi=0;wi< _Max_T2Cst ;wi++)
                {
                ZAM2<<(T2)T2Cst[wi] ;
                }



//!
//! create indexes on field String1 on both ZAMs
//!
//!
    zbs::ZIndexFile<T1,K1_Bulk,Keystruct,zbs::compoundCMP<Keystruct> > ZI1(&ZAM1,ZST_DUPLICATES) ; //! ZIndex key allows duplicates

    zbs::ZIndexFile<T2,K2_Bulk,Keystruct,zbs::compoundCMP<Keystruct> > ZI2(&ZAM2,ZST_DUPLICATES) ; //! ZIndex key allows duplicates


//! print both ZAMs according Key index

//! list ZAM1 elements following ZI1 index order
/*
    printf ("\n\nZAM1 elements in ZI1 order \n");
    for (long wi=0;wi<ZI1.size();wi++)
                fprintf (stdout,"%4ld>> AtomicField <%6ld> String1 <%s>\n",
                            wi,
                            ZI1[wi].AtomicField,
                            ZI1[wi].String1.KeyContent);

    printf ("End ZAM1 elements in ZI1 order ------------\n");

    printf ("\n\nZAM2 elements in ZI2 order \n");
    for (long wi=0;wi<ZI2.size();wi++)
                fprintf (stdout,"%4ld>> AtomicField <%6ld> String1 <%s>\n",
                            wi,
                            ZI2[wi].AtomicField,
                            ZI2[wi].String1.KeyContent);

    printf ("End ZAM2 elements in ZI1 order ------------\n");


    printf ("\n\nZAM2 elements in Raw order \n");
    for (long wi=0;wi<ZAM1.size();wi++)
                fprintf (stdout,"%4ld>> AtomicField <%6ld> String1 <%s>\n",
                            wi,
                            ZAM1[wi].AtomicField,
                            ZAM1[wi].String1.KeyContent);

    printf ("End ZAM2 elements in Raw order ------------\n");
*/
    ZDataBuffer wRecord;

#ifdef __COMMENT__

//! create file
//!
    uriString wTestFile ("Testfile.zrf");
    ZRandomFile ZFile(wTestFile);
    ZStatus wZSt;


//! create file with :
//!             allocated   500 records
//!             recordEstimatedSize : sizeof(T1)  -fixed size-
//!             extent quota 10
//!             history off (false)
//!             AutoCommit off (false)
//!             Journaling off (false)
//!
    if (ZFile.zcreate(wTestFile,
                      500,          //! allocated blocks
                      10,           //! blocks extent quota
                      sizeof(T1),   //! record target size (fixed size for this file)
                      10000,        //! allocated size
                      100           //! extent size quota
                      )!=ZS_SUCCESS)
                    {
                    ZRFException.printUserMessage(stderr);
                    abort();
                    }


    if (ZFile.zopen(ZRF_Read|ZRF_Write|ZRF_Delete)!=ZS_SUCCESS)
                            {
                            ZRFException.printUserMessage(stderr);
                            abort();
                            }


    for (long wi =0;wi<ZAM1.size();wi++)
            {
            wRecord.setData(&ZAM1[wi],sizeof(ZAM1[wi]));

                if ((wZSt=ZFile.zadd(wRecord))!=ZS_SUCCESS)
                            break;
            }
    if (wZSt!=ZS_SUCCESS)
                {
                ZRFException.printUserMessage(stderr);
                exit (0);
                }
//!
//!  ReadNext :
//!  ReadIndex : reads ZRFile following mentionned index number order
//!
    T1 wBuffer;
    printf("Insertion order\n");

    size_t wSize;
    wSt= ZFile.zget(&wBuffer,wSize,0L);
    while (wSt==ZS_SUCCESS)  //! insertion order
    {
        printf (" %s , %d\n",
                wBuffer.String1.KeyContent,
                wBuffer.AtomicField);
        wSt=ZFile.zgetNext(&wBuffer,wSize);
    }

    ZFile.zclose();

    ZFile.zfullDump(wTestFile,16,stderr);   //! full logical dump


#endif //__COMMENT__


//!---------- Varying blocks : include documents ------------------
//!
//! will be a T1 Structure plus a picture as ZDataBuffer
//!


    ZRandomFile ZPicture ;

     ZPicture.zcreate("testpictures",100000,20000 );



    if ((wSt=ZPicture.zopen(ZRF_Write))!=ZS_SUCCESS) //! load document
                {
                    ZRFException.exit_abort();
                }
    if (ZPicture.setGrabFreeSpace(true)!=ZS_SUCCESS)
                                   ZRFException.exit_abort();

   const char * PictureFiles [] = {
        "/home/gerard/Pictures/DSC_01231.jpg",
        "/home/gerard/Pictures/DSC_01232.jpg",
        "/home/gerard/Pictures/DSC_0123.JPG",           // to be deleted idx 2
        "/home/gerard/Pictures/DSC_0146.JPG",
        "/home/gerard/Pictures/DSC_0394.JPG",           // to be deleted idx 4
        "/home/gerard/Pictures/DSC_0414.JPG",
        "/home/gerard/Pictures/Gelato.mp4",
        "/home/gerard/Pictures/mymatelas-facture2016-04-26.pdf",
        "/home/gerard/Pictures/QItemDelegate Class | Qt Widgets 5.7.html", // here idx 8
        "/home/gerard/Documents/DSC_0265.JPG",          //to be added idx 9
        "/home/gerard/Documents/DSC_0267.JPG",          // to be added idx 10
        "/home/gerard/Documents/DSCN0971.JPG"           // to be added idx 11
    };



    ZTimer wTimer;
    wTimer.init();

    uriString wURIPicture;
    ZDataBuffer wPicture;
   for (int wi = 0;wi < 9;wi++)
        {
            wRecord.setData( &ZAM1[wi],sizeof(T1));  //! get the occurrence of T1
            wURIPicture = PictureFiles[wi];         //! set document path
            if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) //! load document
                        {
                            ZException.exit_abort();
                        }

            wRecord += wPicture;                    //! append document content to record buffer
            if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) //! load document
                        {
                            ZRFException.exit_abort();
                        }
        }





   ZPicture.zclose();


   wTimer.end();
    fprintf (stdout,
             " loading time %s \n",
             wTimer.Result.toString());

   ZRandomFile::zheaderDump("testpictures");  //! using static features

   if (ZRandomFile::_ZRFstat("testpictures")!=ZS_SUCCESS)
                                                ZRFException.exit_abort();

   ZRandomFile::zsurfaceScan("testpictures");

//!
//! delete 3 adjacent records
//!
   fprintf (stdout,"\n\n---------------------------------\n"
            "                 remove test\n");

   ZPicture.zopen("testpictures",ZRF_Write);

   ZBlockDescriptor wBD;

   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZRFException.exit_abort();


   fprintf (stdout,"=============================removing record #4 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);

   wTimer.init();

   if (ZPicture.zremove(4)!=ZS_SUCCESS)
                ZRFException.exit_abort();

   wTimer.end();
    fprintf (stdout,
             " removing time %s \n",
             wTimer.Result.toString());


        ZRandomFile::zsurfaceScan("testpictures");



   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZRFException.exit_abort();


   fprintf (stdout,"\n===============================removing record #3 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);
   wTimer.init();
   if (ZPicture.zremove(3)!=ZS_SUCCESS)
                ZRFException.exit_abort();
   wTimer.end();
    fprintf (stdout,
             " removing time %s \n",
             wTimer.Result.toString());

         ZRandomFile::zsurfaceScan("testpictures");

   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZRFException.exit_abort();


   fprintf (stdout,"\n===============================removing record #2 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);

   if (ZPicture.zremove(2)!=ZS_SUCCESS)
                ZRFException.exit_abort();


//   if (ZRandomFile::_ZRFstat("testpictures")!=ZS_SUCCESS)
//                                                ZRFException.exit_abort();
   ZRandomFile::zsurfaceScan("testpictures");

#ifdef __COMMENT__

   fprintf (stdout,"\n===============================recovering free pool block  #0\n");

   long wIndex;
   if ((wSt=ZPicture.zrecover(0L,wIndex))!=ZS_SUCCESS) //! load document
               {
                   ZRFException.exit_abort();
               }

   ZRandomFile::zsurfaceScan("testpictures");
#endif // __COMMENT__
//!
//! Add 3 more records  from Table #6  (huge one)  #10 #11
//!
   fprintf (stdout,"\n\n---------------------------------\n"
            "                 add test\n");

   wTimer.init();


   wRecord.setData( &ZAM1[6],sizeof(T1));  //! get the occurrence of T1
   wURIPicture = PictureFiles[6];         //! set document path
   if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) //! load document
                                                ZException.exit_abort();

   wRecord += wPicture;                    //! append document content to record buffer
   if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) //! load document

    wTimer.end();
   fprintf (stdout,
            " adding time %s \n",
                wTimer.Result.toString());


   ZRandomFile::zsurfaceScan("testpictures");
   for (int wi = 10;wi < 12;wi++)
        {
            wRecord.setData( &ZAM1[wi],sizeof(T1));  //! get the occurrence of T1
            wURIPicture = PictureFiles[wi];         //! set document path
            if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) //! load document
                        {
                            ZException.exit_abort();
                        }

            wRecord += wPicture;                    //! append document content to record buffer
            if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) //! load document
                        {
                            ZRFException.exit_abort();
                        }
        }

   ZPicture.zclose();

      ZRandomFile::zsurfaceScan("testpictures");

//!
//! Now join both ZAMs using ZIX
//!
    return(0);
    zbs::ZJoinStatic<T1,K1_Bulk, T2,K2_Bulk,Keystruct,zbs::compoundCMP<Keystruct> > ZJ1 (&ZI1,&ZI2,ZST_DUPLICATES,true);


//!  print Join result
//!
//    zbs::_TypeMerge wTM;

    printf ("--------------------Join result-----------------------\n");
    for (long wi = 0; wi< ZJ1.size();wi++)
                    {
                    printf ("%3ld >> %7ld |%30s |%7ld | %s \n",
                            wi,
                            ZJ1[wi].ZMaster.AtomicField,
                            ZJ1[wi].ZMaster.String1.KeyContent,
                            ZJ1[wi].ZSlave.AtomicField,
                            ZJ1[wi].ZSlave.String1.KeyContent

                            );
                    }

    printf ("--------------------End Join result--------------------\n");

    printf (" rejected ranks of ZAM1-------------------------------\n");
    for (long wi = 0; wi< ZJ1.ZARejected1->size();wi++)
                    {
                    printf ("%3ld >> %7ld |%30s  \n",
                            wi,
                            ZJ1.ZARejected1->Tab[wi].AtomicField,
                            ZJ1.ZARejected1->Tab[wi].String1.KeyContent
                            );
                    }
    printf ("--------------------End Join result--------------------\n");

    printf (" rejected ranks of ZAM2 from the join-------------------------------\n");
    for (long wi = 0; wi< ZJ1.ZARejected2->size();wi++)
                    {
                    printf ("%3ld >> %7ld |%30s  \n",
                            wi,
                            ZJ1.ZARejected2->Tab[wi].AtomicField,
                            ZJ1.ZARejected2->Tab[wi].String1.KeyContent
                            );
                    }

//!
//!  -----------Dynamic joins ZJoinDyn--------------------------
//! requires <zjoin.h> <zjoindyn.h>

//!
//! Same Join as previous but with ZJoinDyn
//!

zbs::ZJoinDyn<T1,K1_Bulk, T2,K2_Bulk,Keystruct,zbs::compoundCMP<Keystruct> > ZJ2 (&ZI1,&ZI2,ZST_DUPLICATES,true);

        printf ("--------------------Dynamic Join results-----------------------\n");
        for (long wi = 0; wi< ZJ2.size();wi++)
                        {
                        printf ("%3ld >> %7ld |%30s |%7ld | %s \n",
                                wi,
                                ZJ2[wi].ZMaster.AtomicField,
                                ZJ2[wi].ZMaster.String1.KeyContent,
                                ZJ2[wi].ZSlave.AtomicField,
                                ZJ2[wi].ZSlave.String1.KeyContent

                                );
                        }

        printf ("--------------------End Dynamic Join results--------------------\n");

        printf (" rejected ranks of ZAM1-------------------------------\n");
        for (long wi = 0; wi< ZJ2.Rejected1.size();wi++)
                        {
                        printf ("%3ld >> %7ld |%30s  \n",
                                wi,
                                ZJ2.Rejected1[wi].AtomicField,
                                ZJ2.Rejected1[wi].String1.KeyContent
                                );
                        }
        printf ("--------------------End Join result--------------------\n");

        printf (" rejected ranks of ZAM2 from the dynamic join-------------------------------\n");
        for (long wi = 0; wi< ZJ2.Rejected2.size();wi++)
                        {
                        printf ("%3ld >> %7ld |%30s  \n",
                                wi,
                                ZJ2.Rejected2[wi].AtomicField,
                                ZJ2.Rejected2[wi].String1.KeyContent
                                );
                        }

//!
//! let's add some rows into ZAM1 and let's see what happens to the dynamic join
//!

    struct T1 T3Cst[] =
    {
    {"my heart beats",                 1505 },           // multiple correspondances in both ZIX
    {"my heart beats no more",          104},           // no correspondance in any
    {"xxxx",                            11011},          // with one correspondance
    {"xxxx",                            11012},          // many
    {"xxxx",                            11013},          // many times
    {"my heart beats",                 1506 },           // many to many
    {"jkfd",                            2025},                  // row that have been rejected
    {"jkfdlsakfj",                       2025}                  // row that have been rejected In ZAM1 to be inserted in ZAM2
};

    struct T2 T4Cst[] =
    {
    {"my heart beats",                 1505 },           // multiple correspondances in both ZIX
    {"my heart beats no more",          104},           // no correspondance in any
    {"xxxx",                            11011},          // with one correspondance
    {"xxxx",                            11012},          // many
    {"xxxx",                            11013},          // many times
    {"my heart beats",                 1506 },           // many to many
    {"jkfd",                            2025},                  // row that have been rejected
    {"jkfdlsakfj",                       2025}                  // row that have been rejected In ZAM1 to be inserted in ZAM2
};


    ZAM1<<(T1)T3Cst[6];
    ZAM2<<(T2)T4Cst[7];

    printf ("--------------------Dynamic Join results-----------------------\n");
    for (long wi = 0; wi< ZJ2.size();wi++)
                    {
                    printf ("%3ld >> %7ld |%30s |%7ld | %s \n",
                            wi,
                            ZJ2[wi].ZMaster.AtomicField,
                            ZJ2[wi].ZMaster.String1.KeyContent,
                            ZJ2[wi].ZSlave.AtomicField,
                            ZJ2[wi].ZSlave.String1.KeyContent

                            );
                    }

    printf ("--------------------End Dynamic Join results--------------------\n");

    printf (" rejected ranks of ZAM1-------------------------------\n");
    for (long wi = 0; wi< ZJ2.Rejected1.size();wi++)
                    {
                    printf ("%3ld >> %7ld |%30s  \n",
                            wi,
                            ZJ2.Rejected1[wi].AtomicField,
                            ZJ2.Rejected1[wi].String1.KeyContent
                            );
                    }
    printf ("--------------------End Join result--------------------\n");

    printf (" rejected ranks of ZAM2 from the dynamic join-------------------------------\n");
    for (long wi = 0; wi< ZJ2.Rejected2.size();wi++)
                    {
                    printf ("%3ld >> %7ld |%30s  \n",
                            wi,
                            ZJ2.Rejected2[wi].AtomicField,
                            ZJ2.Rejected2[wi].String1.KeyContent
                            );
                    }



    ZAM1.reset();

#ifdef __COMMENT__

//! Declare the CUF (Commit User Function)
//!
    char wCUFBuf [50] = "This is commit Data";
     wSt=ZAM1.declareCommitUserFunction( &CUFunction,(void *)&wCUFBuf);

    wSt=ZAM1.setJournaling(zbs::ZJAutoCommit);  //! for getting response within CUF quickly

//! push 3 new constants and insert one

    struct T1 T3Cst[] =
    {
    {"my heart does not beat",          1505 },         // to be pushed
    {"my heart beats no more",          104},           // to be pushed
    {"and so does not beat my skin",    1512} ,         // to be pushed
    {"oops",                            11011} };       // to be inserted at position 3

    wSt= ZAM1<<(T1&)T2Cst[0];
    printf ("push %ld  %s\n",T2Cst[0].AtomicField, T2Cst[0].String1);

    ZAM1<<(T1&)T2Cst[1];
    printf ("push %ld  %s\n",T2Cst[1].AtomicField, T2Cst[1].String1);
    ZAM1<<(T1&)T2Cst[2];
    printf ("push %ld  %s\n",T2Cst[2].AtomicField, T2Cst[2].String1);

    const T1 Key1_coll = {"xxxx",                    110} ;
/*    long st = ZAM1.insert((T1&)Key1_coll,1);
*/

     void * wZIXptr= ZAM1.askZIndex(0,sizeof(T1::AtomicField));

     zbs::ZIndex<T1,K1_Bulk,char*,zbs::stringCMP<char*> > *ZIXptr=static_cast<zbs::ZIndex<T1,K1_Bulk,char*,zbs::stringCMP<char*> >*>(wZIXptr);





//    wSt=ZAM1.erase (1025); // nope
//    wSt=ZI2.remove(999);
    wi=3;
    printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
            wi,
            ZI1[wi].AtomicField,        //! direct read of the ZAM fields from ZIndex
            ZI1[wi].String1);

    wSt=ZAM1.replace((T1&)Key1_coll,2);

    ZAM1.printJournal();
    ZAM1.printHistory();

    printf ("ZAM1 content ---------\n");
    for (long wi=0;wi< ZAM1.size();wi++)
                {
                printf ("%3ld>> AtomicField <%ld> String1 <%s>\n",
                        wi,
                        ZAM1[wi].AtomicField ,
                        ZAM1[wi].String1
                        );
                }
    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    printf ("----------ZI2 list before erase(2,1)-----------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }
    printf ("------------->erase(2,1)   AtomicField <%ld>  String1 <%s>\n",
            ZAM1[2].AtomicField,
            ZAM1[2].String1);

    ZAM1.erase(2,1);   // the third is bad : let's suppress it  we can use also ZAM1.erase(2); it is quicker
    printf ("Dump  ZI2.ZReverse content ---------\n");

    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%s>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.printKeyValue(wi,wBuf)
                        );
                }
    printf ("----------ZI2 list after erase(2,1)-----------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }
    printf ("------------->insert 3rd constant à rank 3  AtomicField <%ld> String1 <%s> \n",
            T2Cst[3].AtomicField,T2Cst[3].String1);

    ZAM1.insert((T1&)T2Cst[3],3); // insert the 3rd constant at rank 3  (within ZAM1)
    printf ("\n\nZAM1 elements in raw ZAM order \n");
    for (long wi=0;wi<ZAM1.size();wi++)
                fprintf (stdout,"%4ld>> AtomicField <%6ld> String1 <%s>\n",
                            wi,
                            ZAM1[wi].AtomicField,
                            ZAM1[wi].String1);




//!--------ZAM Journaling facilities--------------
//!
//! lets see what contains ZJournaling log
//!

    ZAM1.printJournal(-1,       //! (default:-1) -1 for all journal indexes (or specify a positive number for index)
                       true,    //! (default:false) true means 'include dump' of journalized row content in print out
                       stdout);  //! (default:stdout) what is the output chanel (could be something else)

//! OOps we made a mistake the last 3 operations
//! lets rollback
//!
//!  if the ZAMmark is positive : then search the effective operation number within the journal
//!  it ZAMmark is negative : this comes to roll back the last mentionned
//!
//!
//!
//!


    printf ("Dump Index ZI1 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%s>\n",
                        wi,
                        ZI1.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI1.Tab[wi].State),
                         ZI1.Tab[wi]._Key
                        );
                }

    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    //!             Rollback syntax
    //!
    //! rollback ();        means : rollback all available journal events
    //! rollback (-n) ;     means : rollback the last n elements starting at the last element until last -n
    //! rollback (m) ;      means : rollback all events until journal rank m

    ZAM1.rollback(-3); //! lets take for roll back the last 3 operations


    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    printf ("----------ZI2 list -----------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }

    printf (" pop ZAM1 for rank <%ld> <%ld> <%s>\n",
            ZAM1.lastIdx(),
            ZAM1[ZAM1.lastIdx()].AtomicField,
            ZAM1[ZAM1.lastIdx()].String1);


    ZAM1.printJournal();
    ZAM1.printHistory();
    ZAM1.pop();             //suppress the last element

    printf ("Dump  ZI2.ZReverse content ---------\n");

    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    printf ("----------ZI2 list -----------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }
    printf (" ------------------pop_front ZAM1  <%ld> <%s>\n",
            ZAM1[0].AtomicField,
            ZAM1[0].String1);

    ZAM1.pop_front();       // suppress the first ZAM event

    printf ("----------ZI2 list after pop_front()-----------\n");

    printf ("Dump Index ZI2 content ---------\n");
    for (long wi=0;wi< ZI1.size();wi++)
                {
                printf ("%3ld>> ZAMIndex <%4ld>  ZAMState <%s> Key <%ld>\n",
                        wi,
                        ZI2.Tab[wi].ZAMIdx ,
                        decode_ZAMState(ZI2.Tab[wi].State),
                        ZI2.Tab[wi]._Key
                        );
                }
    printf("ZAM1 ordered ZI2\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%4ld>> AtomicField <%6ld> String1 <%s> \n",
                        wi,
                        ZI2[wi].AtomicField,
                        ZI2[wi].String1);
                }


    //! search for ZI2 key value 104
    //!

    zbs::ZResult wZRes= ZI2.seek(12045);

    if (wZRes.ZSt!=ZS_FOUND)
                {
            printf ("Key 12045 not found on ZI2 status %s\n",decode_ZStatus(wZRes.ZSt));
                }

//    ZAM1[wZRes.ZIdx].AtomicField = 20786; // false cannot modify ZAM like that (read only values)

    T1 Rep ;
    Rep.AtomicField= ZAM1[wZRes.ZIdx].AtomicField;
    Rep.String1 = ZAM1[wZRes.ZIdx].String1;

    Rep.AtomicField = 20786 ;
    ZAM1.replace(Rep,wZRes.ZIdx);   // then replace.

    printf ("----------ZI2 list after replace with <20786>-----------\n");


    printf ("Dump Index ZAM1 content ---------\n");
    for (long wi=0;wi< ZI2.size();wi++)
                {
                printf ("%3ld>> AtomicField <%ld> String1 <%s>\n",
                        wi,
                        ZAM1[wi].AtomicField ,
                        ZAM1[wi].String1
                        );
                }


    //! lets print again what contains ZJournaling log
    //!

        ZAM1.printJournal(-1,       //! (default:-1) -1 for all journal indexes (or specify a positive number for index)
                           false,    //! (default:false) true means 'include dump' of journalized row content in print out
                           stdout);  //! (default:stdout) what is the output chanel (could be something else)


//!        That's correct, let' commit all
//!
//!
        ZAM1.commit(nullptr,-1);  //! nullptr : no user routine - -1 : commit all events

        ZAM1.printJournal();  //! lets print again the journal log

        //! see what's in history
        ZAM1.printHistory(-1,       //! same as printJournal for Historized events
                          false,
                          stdout);




    /**     --Dynamic Join  Test Plan--
     *  Overload of [] operator for accessing dynamic join content
     *
     * 1. Master side
     *  adding row within Master with multiple correspondance within Slave ZKA
     *  add row within Master with one correspondance within Slave
     *  add row within Master with NO correspondance within Slave
     *
     * 2. Slave side
     *  add row within Slave
     *  add row within Slave with multiple correspondance within Master
     *  add row within Slave with no correspondance within Master
     *
     * 3. Erase
     *  erase row within Master with only one correspondance within Slave
     *  erase row within Master with multiple correspondances within Slave
     *  erase row within Slave with multiple correspondance within Master
     *  erase multiple rows within Master
     *  erase multiple rows within Slave
     *
     * 4.change
     *  change row within Master
     *  change row witnin Slave
     *  modify row Master using = operator on _Key value (error case)
     *  modify row Master using = operator on other field than _Key value
     *  modify row Slave using = operator on _Key value (error case)
     *  modify row Slave using = operator on other field than _Key value
     *
     *
     */

#endif

_RETURN_ (0);


} // main

