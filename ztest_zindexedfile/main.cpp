/*
 * Gérard POLO 09/11/2015
 *
*
 *
 *
 */


#define __MAIN__


#include <QCoreApplication>

#include <cstdio>
#include <iostream>
#include <stdio.h>
//#include <stddef.h>
#include <cstddef>
#include <zio/zdir.h>

#include <zconfig.h>
#include <ztoolset/zarray.h>
#include <ztoolset/zerror.h>
#include <zindexedfile/zmasterfile.h>
#include <zindexedfile/zdatatype.h>

#include <zindexedfile/zmfdictionary.h>
// /home/gerard/Development/zbasetools/zcontent/zindexedfile/zmfdictionary.h
//#include <zflowcommon.h>
#include <common/zapplicationtypes.h>
#include <zlayout.h>

#include <typeinfo>
//#include <type_traits>

#include <arpa/inet.h>

#include <zrandomfile/zrfcollection.h>

#include <znet/zbasenet/zsslclientsocket.h>

//#include <znet/zbasenet/zconsolecommand.h>

#include <zindexedfile/zrecord.h>
#include <zindexedfile/zmasterfile_utilities.h>


#include <stdio.h>
#include <fcntl.h>

//#include <locale>
#include <ztoolset/zerror.h>




#define __TEST_DIRECTORY__  "/home/gerard/Development/zbasetools/zcontent/ztest_zindexedfile/testdata/"
#define __TEST_FILE__ "/home/gerard/Development/zmftest/zdocphysical.zmf"

struct Keystruct {
    char KeyContent[50];
};

struct T1 {
           char String1 [50]  ;
           float AtomicField  ;
};

enum Doc_type  {
    DT_Nothing = 0,
    DT_Pdf = 1,
    DT_Jpeg = 2,
    DT_Html = 3,
    DT_Mp4 = 4,
    DT_Mail = 5,
    DT_Other = 6
};

struct T3 {
  char      String1 [50];
  float     NumericField [3]  ;
  Doc_type  Type;
};

struct T2 {
    char String1 [50];
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

#ifdef __COMMENT__
 void
 printIndexKey_0(ZMasterFile &ZPicture)

{
ZStatus wSt;

struct IndexKey_0_struct
{
    float AtomicField [3]  ;
    char String1 [50] ;
} IndexKey_0;


 fprintf (stdout,
          "\n ---------Index 0 Key dump----------\n"
//          "%5s <%15s> <%10s> <%50s>\n",
          "%5s <%10s> <%50s>\n",
          "Rank",
//          "Address",
          "AtomicField",
          "String1"
          );



 ZDataBuffer wIndexContent;
 long wi = 0;
 //wSt=ZPicture.getRawIndex(wRawIndexContent,wi,0L);


 wSt=ZPicture.getKeyIndexFields(wIndexContent,0L,0L);
 if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
 while (wSt==ZS_SUCCESS)
 {
     wIndexContent.moveOut<decltype(T3::NumericField)>(IndexKey_0.AtomicField,0L);
     wIndexContent.moveOut<decltype(T3::String1)>(IndexKey_0.String1,sizeof(T3::NumericField));
//     wIndexContent.moveOut<IndexKey_0_struct> (IndexKey_0); //! use ZDataBuffer template move to setup user defined struct ZIndexKey_0
//        wRawIndexContent.KeyContent.Dump(16,stdout);
     fprintf (stdout, "%5ld <%10f> <%10f> <%10f> <%-50s>\n",
//     fprintf (stdout, "%5ld <%10f> <%-50s>\n",
              wi,
//              wIndexContent.ZMFaddress,
              IndexKey_0.AtomicField[0],
             IndexKey_0.AtomicField[1],
             IndexKey_0.AtomicField[2],
              IndexKey_0.String1
              );
     wi++;
     wSt=ZPicture.getKeyIndexFields(wIndexContent,wi,0L);
 }
 fprintf (stdout,"\n ---------End Index 0 Key dump----------\n");
 if (wSt!=ZS_OUTBOUNDHIGH)
         {
             ZException.exit_abort();
         }
 return;
 }//printIndexKey_0

 void
 printIndexKey_1(ZMasterFile &ZPicture,const zrank_type pIndexRank)

{
_MODULEINIT_
ZStatus wSt;

 struct IndexKey_1_struct
 {
     char String1[50]  ;
 } IndexKey_1;

 ZSIndexItem wRawIndexContent;
 ZPicture.zopen(ZRF_Read_Only);
 long wi = 0;

 ZPicture.zprintIndexFieldValues( zprintKeyFieldsValues (const zrank_type pRank,bool pHeader,bool pKeyDump,FILE*pOutput);

 wSt=ZPicture.getRawIndex(wRawIndexContent,wi,1L);
 ZSKeyDictionary* wZKDic = ZPicture.zgetKeyDictionary(pIndexRank);
 if (wZKDic==nullptr)
    {
    _ABORT_;
    }

 for (wi=0;wi<wZKDic->size();wi++)
 {


 }


 while (wSt==ZS_SUCCESS)
 {
     wRawIndexContent.KeyContent.moveOut<IndexKey_1_struct> (IndexKey_1); // use ZDataBuffer template move to setup user defined struct ZIndexKey_0
//        wRawIndexContent.KeyContent.Dump(16,stdout);
     fprintf (stdout, "\n%5ld <%15lld> <%-50s>\n",
              wi,
              wRawIndexContent.ZMFaddress,
              IndexKey_1.String1
              );
     wi++;
     wSt=ZPicture.getRawIndex(wRawIndexContent,wi,1L);
 }
 fprintf (stdout,"\n ---------End Index 1 Key dump----------\n");
 if (wSt!=ZS_OUTBOUNDHIGH)
         {
             ZException.exit_abort();
         }
    _RETURN_;
 }//printIndexKey_1

#endif // __COMMENT__

#include <ztoolset/zbitset.h>

#include <zxml/zxmldoc.h>
#include <zxml/zxmlnode.h>
#include <zxml/zxmlattribute.h>

const int Indent=4;

void
printElement(zxmlElement* pElement,int pLevel)
{
ZStatus wSt;
char wSpace[500];

zxmlNode*wNode,*wNode2;
zxmlAttribute* wAttribute;

utf8VaryingString wNodeContent;
utf8VaryingString wNodeText;

    memset (wSpace,' ',sizeof(wSpace));
    wSpace[pLevel*Indent]='\0';
    if (!pElement)
        {
        fprintf(stdout,"%s>******Nothing to print - null element*********\n",
                wSpace);
        return;
        }
    fprintf (stdout,"%s>Element name <%s> line <%d> child elements <%ld> ",
             wSpace,
             pElement->getName().toCChar(),
             pElement->getLine(),
             pElement->getChildElementCount()
             );
    fprintf (stdout," %s%s%s%s%s ",
//             wSpace,
             pElement->hasElement()?"Has Element(s) ":"",
             pElement->hasText()?"Has Text(s) ":"",
             pElement->hasComment()?"Has comment(s) ":"",
             pElement->hasAttribute()?"Has attribute(s) ":"",
             pElement->hasCData()?"Has CData section(s) ":"");

    fprintf (stdout," Path: <%s>\n",
             pElement->getNodePath().toCChar());

    if (pElement->hasText())
        {
        wSt=pElement->getFirstText(wNode,wNodeText);
        while ((wSt==ZS_SUCCESS)&&(wNode))
            {
            fprintf(stdout,
                    "%s Text <%s>\n",
                    wSpace,
                    (char*)wNodeText.Data);
            wSt=wNode->getNextText(wNode2,wNodeText);
            wNode=wNode2;
            }
        }
    if (pElement->hasComment())
        {
        wSt=pElement->getFirstChildComment(wNode);
        while ((wSt==ZS_SUCCESS)&&(wNode))
            {
            wSt=wNode->getNodeRaw(wNodeContent);
            if (wSt==ZS_SUCCESS)
                    fprintf(stdout,"%s Comment line <%d> <%s>\n",
                            wSpace,
                            wNode->getLine(),
                            wNodeContent.toCChar());
                else
                    fprintf(stdout,"%s cannot get comment content\n",
                            wSpace);
            wSt=wNode->getNextCommentSibling(wNode2);
            wNode=wNode2;
            }
        }
    if (pElement->hasAttribute())
        {
        wSt=pElement->getFirstAttribute(wAttribute);
        while ((wSt==ZS_SUCCESS)&&(wAttribute))
            {
                    fprintf(stdout,"%s Attribute <%s> : <%s>\n",
                            wSpace,
                            wAttribute->getName().toString(),
                            wAttribute->getValue().toString());
            wSt=wAttribute->getNextAttributeSibling(wAttribute);
            }
        }
    if (pElement->hasCData())
        {
        wSt=pElement->getFirstCData(wNode,wNodeContent);
        while ((wSt==ZS_SUCCESS)&&(wNode))
            {
            fprintf(stdout,
                    "%s CData content <%s>\n",
                    wSpace,
                    wNodeContent.toCChar());

            wSt=wNode->getNextCData(wNode2,wNodeContent);
            wNode=wNode2;
            }
        }

  return;
}


void
dumpElement(zxmlElement* pElement,int pLevel)
{
ZStatus wSt;

zxmlElement* wElement;

    wSt=pElement->getFirstChildElement(wElement);
    while ((wElement)&&(wSt==ZS_SUCCESS))
    {
    printElement(wElement,pLevel);

    dumpElement(wElement,pLevel+1);

    wSt=wElement->getNextElementSibling (wElement);
    }
  return;
}//levelNode
#include <ztoolset/charfixedstring.h>
#include <ztoolset/charvaryingstring.h>


#include <unicode/unistr.h>
#include <unicode/ucnv.h>
#include <unicode/sortkey.h>
#include <unicode/ucol.h>
#include <unicode/uloc.h>

#include <unicode/i18n/csdetect.h>
#include <unicode/i18n/csmatch.h>


#include <ucnv_bld.h>

#include <ztoolset/utfconverter.h>

#include <ztoolset/utfsprintf.h>

class ZDocPhysical
{
public:
  // Caller Mode
  ZResourceCode       Documentid;     //R    Documentid is the unique document identifier generated by DocumentServer :permanent resource code id
  DataRank_type       DataRank=cst_DataRankInvalid;
  utfcodeString       Short;
  utfdescString       Desc;          //RW

  DocumentMetaType    DocMetaType;    //RW
  Storage_t           Storage;        //RW   see Storage_type enum definition (ztypes.h) write once when physical document is created
  utfdescString       MimeName;       //R
  uriString           URI;            //R    Uri where the document is stored :
  //                                         At creation from Pinboard : first Cache URI
  //                                         then replaced with Document Server Vault URI or external location if document is externally stored
  //
  DocSize_t           DocSize;        //R

  ZDateFull           Registrated;    //R
  ZDateFull           Created;        //R
  ZDateFull           LastModified;   //R

  ZResourceCode       OwnerOrigin;    //R                 permanent resource code id
  //    ZResourceCode       OwnerCurrent;   //RW                permanent resource code id
  //                  ------------
  ZAccessRight_t      AccessRights;   //RW        : TODO Need to be addressed -> this overrides default Vault AccessRights
  //                  ------------
  checkSum            CheckSum;       //R     this is the SHA256 checksum for the UNCRYPTED document file content stored into the VAULT.
      //     to be compared to document content in cache
  uint8_t             Temporary;      //R   set to true if MetaData is for a temporary DocPhysical

  ZResourceCode       Ownerid;         //     User Identification number to whom belongs the Document ; permanent resource code id

  ZResourceCode       Vaultid;

  ZaiErrors ErrorLog;

};



class ZDocStored : public ZDocPhysical
{
public:
  utf8VaryingString* Content=nullptr;
  void clear()
  {
  zdelete (Content);
//  ZDocPhysical::clear();

  Documentid.clear();     //R    Documentid is the unique document identifier generated by DocumentServer :permanent resource code id
  DataRank=cst_DataRankInvalid;
  Short.clear();
  Desc.clear();

  DocMetaType=ZDMT_nothing;    //RW
  Storage=Storage_Nothing;        //RW   see Storage_type enum definition (ztypes.h) write once when physical document is created
  MimeName.clear();       //R
  URI.clear();            //R    Uri where the document is stored :
  //                                         At creation from Pinboard : first Cache URI
  //                                         then replaced with Document Server Vault URI or external location if document is externally stored
 DocSize=0;        //R

  Registrated.clear();    //R
  Created.clear();        //R
  LastModified.clear();   //R

  OwnerOrigin.clear();    //R                 permanent resource code id
  //    ZResourceCode       OwnerCurrent;   //RW                permanent resource code id
  //                  ------------
  AccessRights=0;   //RW        : TODO Need to be addressed -> this overrides default Vault AccessRights
  //                  ------------
  CheckSum.clear();       //R     this is the SHA256 checksum for the UNCRYPTED document file content stored into the VAULT.
      //     to be compared to document content in cache
  Temporary=0;      //R   set to true if MetaData is for a temporary DocPhysical

  Ownerid.clear();         //     User Identification number to whom belongs the Document ; permanent resource code id

  Vaultid.clear();

  }
};


void
docFeed(ZRecord* pRecord,ZDocStored& pDoc)
{
ZStatus wSt;
  pDoc.LastModified.getCurrentDateTime();
  wSt=pRecord->setFieldValuebyName_T(pDoc.Documentid.id,"Documentid");
  _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
  wSt=pRecord->setFieldValuebyName_T(pDoc.Desc,"Desc");
  _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
  wSt=pRecord->setFieldValuebyName_T(pDoc.LastModified,"LastModified");
  _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
  wSt=pRecord->setFieldValuebyName_T(pDoc.URI,"URI");
  _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
  wSt=pRecord->setFieldValuebyName_T(*pDoc.Content,"DocumentContent");
  _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
  wSt=pRecord->setFieldValuebyName_T(pDoc.DocSize,"DocSize");
  _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
  wSt=pRecord->setFieldValuebyName_T(pDoc.MimeName,"MimeName");
  _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
  wSt=pRecord->setFieldValuebyName_T(pDoc.CheckSum,"CheckSum");
  _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
  return;
}

ZStatus
getDoc(ZRecord* pRecord,ZDocStored& pDoc)
{
  ZStatus wSt;
  pDoc.clear();
  wSt=pRecord->getFieldValuebyRank_T(pDoc.Documentid.id,0L);
  wSt=pRecord->getFieldValuebyName_T(pDoc.Desc,"Desc");
  wSt=pRecord->getFieldValuebyName_T(pDoc.MimeName,"MimeName");
  wSt=pRecord->getFieldValuebyName_T(pDoc.CheckSum,"CheckSum");
  wSt=pRecord->getFieldValuebyName_T(pDoc.Created,"Created");
  if (pRecord->testFieldPresenceByName("DocumentContent"))
    {
    pDoc.Content=new utf8VaryingString;
    wSt=pRecord->getFieldValuebyName_T(*pDoc.Content,"DocumentContent");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
    }
  return wSt;
}


int main(int argc, char *argv[])
{

const char16_t* wTabKey[]={u"ceci est un texte français avéc des accents prononcés",
                   u"CECI est UN Texte FRANCAIS AVEC des accents PRONONCES",
                   u"cela est un text simple",
                   u"this is an english text without accented characters"};

const char* wTabKeyChar[]={"ceci est un texte français avéc des accents prononcés",
                   "CECI est UN Texte FRANCAIS AVEC des accents PRONONCES",
                   "cela est un text simple",
                   "this is an english text without accented characters"};

utf16_t wTabSortKey[4][100];

ZStatus wSt;



    const char* str_Utf8=u8"Всё смеша́лось в до́ме Облонских. Жена́ узна́ла, что";
    const utf16_t* str_utf16=(const utf16_t*)u"Всё смеша́лось в до́ме Облонских. Жена́ узна́ла, что";
    const utf32_t* str_utf32=(const utf32_t*)U"Всё смеша́лось в до́ме Облонских. Жена́ узна́ла, что";

    const char* str_utf8_Small=u8"Всё смеша́лось";   /* size is 13 */
    const utf16_t* str_utf16_Small=(const utf16_t*)u"Всё смеша́лось";
    const utf32_t* str_utf32_Small=(const utf32_t*)U"Всё смеша́лось";

    const utf32_t char_utf32=U'д';
    const utf16_t char_utf16=u'д';



    ZCharset_type wDefaultCharset = getDefaultCharset();

    const char* wDefaultEncodingName=getDefaultEncodingName();

    const char* wDefaultEncodingCanonical=validateCharsetName(wDefaultEncodingName);

 /*   icu::Locale wLocale;
    wLocale.getDefault();

    fprintf (stdout,"Locale \n"
             "  Name     <%s>\n"
             "  Script   <%s>\n"
             "  Language <%s>\n"
             "  Country  <%s>\n"
             "  Variant  <%s>\n",
             wLocale.getName(),
             wLocale.getScript(),
             wLocale.getLanguage(),
             wLocale.getCountry(),
             wLocale.getVariant()
             );

    wLocale.getRoot();
    fprintf (stdout,"Locale \n"
             "  Name     <%s>\n"
             "  Script   <%s>\n"
             "  Language <%s>\n"
             "  Country  <%s>\n"
             "  Variant  <%s>\n",
             wLocale.getName(),
             wLocale.getScript(),
             wLocale.getLanguage(),
             wLocale.getCountry(),
             wLocale.getVariant()
             );

*/
ZDataBuffer wUsername;
ZDataBuffer wPassword;
ZDataBuffer wDomain;

    utf8_t wByte = 0b11100000;

#ifdef __COMMENT__

    setVerbose(ZVB_Net);
    wUsername = "gerard";
    wPassword ="pissenlit";
#ifdef __USE_LINUX__
    signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE signal and manage error on read/write level
#endif
#define __BASE_SOCKET__

#ifdef __BASE_SOCKET__
ZClientSocket wSocket;
    wSt=wSocket.clientSetup(IPV4Host,
                            ZFileServerPort,
                            AF_UNSPEC,  // do not care about ip address format
                            false,      // non blocking
                            30000000,   // read timeout is 30 seconds
                            30000000);  // write timeout is 30 seconds
    if (wSt!=ZS_SUCCESS)
        {
        ZException.exit_abort();
        }


    wUsername.encodeB64();
    wPassword.encodeB64();

#ifdef __USE_LDAP__
    wDomain = "seth-world.com";
    wDomain.encodeB64();
    wSt=wSocket.clientConnectLDAP(wDomain,wUsername,wPassword);
#else
    wSt=wSocket.clientConnect(wUsername,wPassword);
#endif
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(stdout," shutting socket down\n");
        wSocket.zshutdown();
        ZException.exit_abort();
        }

#else







 ZS_SUCCESS)
        {
        ZException.exit_abort();
        }




    wUsername.encodeB64();
    wPassword.encodeB64();

#ifdef __USE_LDAP__
    wDomain = "seth-world.com";
    wDomain.encodeB64();
    wSt=wSocket.SSLclientConnectLDAP(wDomain,wUsername,wPassword);
#else
    wSt=wSocket.SSLclientConnect(wUsername,wPassword);

#endif
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(stdout," shutting socket down\n");
        wSocket.zshutdown();
        ZException.exit_abort();
        }

    wSt=wSocket.verifyPeerCertificate(wCertificateFilename);
    if (wSt!=ZS_SUCCESS)
        {
        fprintf(stdout," Peer certificate verification errored.Shutting socket down\n");
        wSocket.zshutdown();
        ZException.exit_abort();
        }

    fprintf(stdout," Peer certificate has been successfully verified.\n");


#endif // else #ifdef __BASE_SOCKET__

   ZDataBuffer wMes;

    wSt=wSocket.zread(wMes);

    if (wSt!=ZS_SUCCESS)
        {
        fprintf(stdout," closing socket \n");
        wSocket.zclose();
        ZException.exit_abort();
        }

    fprintf (stdout," Received from server <%s>\n",wMes.DataChar);

    wSocket.zclose();

    return 0;
#endif // __COMMENT__
    const size_t _Max_T1Cst = 11;
    const size_t _Max_T2Cst = 12;
    size_t wi;



    struct T1 T1Cst[] =
    {
    {"my heart beats"        ,  -155 },     // row with multiple correspondance within Slave
    {"my heart beats for you",  -1024},      // no correspondance within slave
    {"and so beats my skin",    -152} ,
    {"xxxx",                    110},         // duplicate this row on ZIX1 key string
    {"this is one",             1 },
    {"aaaa   ",                 34 },
    {"eer",                     5},
    {"   dddd",                 250},
    {"jkfdlsakfj",              2025},      // row with no correspondance within Slave
    {"egadfk",                 25},
    {"aaaa   b",               12045 }
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

    struct T3 T3Cst[] =
    {
    {"my heart beats"        ,      {110 ,-155 ,1} ,DT_Nothing  },          // 0
    {"my heart beats for you",      {120 ,-1024,10},DT_Nothing  },          // 1 *
    {"and so beats my skin",        {140 ,-152, 100},DT_Nothing  } ,        // 2
    {"but it beats slowly",         {150, 110 ,1000} ,DT_Nothing },         // 3
    {"this is one",                 {160 , -152 ,100000 },DT_Nothing },     // 4
    {"my heart is lonely",          {34 , 110 ,1},DT_Nothing  },            // 5
    {"this is not one but two",     {5, -1024 ,10 },DT_Nothing },           // 6 *
    {"   dddd",                     {250, 110 ,100 },DT_Nothing },          // 7
    {"my heart enjoys lonelyness",  {2025, -152 ,1000 },DT_Nothing },       // 8 *
    {"egadfk",                      {25, 110 ,10000 },DT_Nothing },         // 9
    {"aaaa",                        {12045, 110 ,1 } ,DT_Nothing }          // 10
    };


//---------- Varying blocks : include documents ------------------
//
// will be a T1 Structure plus a picture as ZDataBuffer
//

//---------------------------------------------------------------------------------
//  Creating an Indexed ZMasterFile :
//
//     First- instantiate a ZMasterFile with or whitout a file name
//   (file name and file path can be mentionned later on using setPath method)
//
//     Second- define key :
//         for each desired key define key fields :
//             for each field the ZIndexField_struct to specify each field composing the Key
//         then store it in an ZArray<ZIndexField_struct> in the order of the key (not necessarily growing offset order)
//
//     Third create the Index file from ZMasterFile using : zcreateIndexFile method
//
const char * PictureFiles [] = {
     "/home/gerard/Pictures/DSC_01231.jpg",
     "/home/gerard/Pictures/DSC_01232.jpg",
     "/home/gerard/Pictures/DSC_0123.JPG",           // to be deleted idx 2
     "/home/gerard/Pictures/DSC_0146.JPG",
     "/home/gerard/Pictures/DSC_0394.JPG",           // to be deleted idx 4
     "/home/gerard/Pictures/DSC_0414.JPG",
     "/home/gerard/Pictures/Gelato.mp4",
     "/home/gerard/Pictures/invoice2016-04-26.pdf",
     "/home/gerard/Pictures/QItemDelegate Class | Qt Widgets 5.7.html", // here idx 8
     "/home/gerard/Documents/DSC_0265.JPG",          //to be added idx 9
     "/home/gerard/Documents/DSC_0267.JPG",          // to be added idx 10
     "/home/gerard/Documents/DSCN0971.JPG"           // to be added idx 11
 };

/* 1- create ZMasterFile
 *
 *  create a MetaDictionary
 *  create all fields for the file
 *
 *  create the file using meta dictionary
 *
 * 2- populate the file
 *
 *
 *
 */




    setVerbose(ZVB_ZMF|ZVB_Basic);

    ZException.setAbortOnSeverity(Severity_Severe);

    ZMFDictionary wMasterDic("ZDocPhysical : Physical document entity");                      // empty meta dictionary
// populate dictionary here
    ZDocStored wDoc;
//fieldDesc_struct wField;
//    utf8VaryingString wUtf8Content;
    wSt=wMasterDic.addField_T<long>("Documentid");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
    wSt=wMasterDic.addField_T<utfdescString>("Desc");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
    wSt=wMasterDic.addField_T<utfcodeString>("DocMetaType");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<utf8String>("DocumentContent");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<uint32_t>("Storage");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<utfdescString>("MimeName");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<uriString>("URI");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<size_t>("DocSize");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<ZDateFull>("Registrated");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<ZDateFull>("Created");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
    wSt=wMasterDic.addField_T<ZDateFull>("LastModified");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<utfidentityString>("OwnerOrigin");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<utfidentityString>("OwnerCurrent");

    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)
    wSt=wMasterDic.addField_T<uint32_t>("AccessRights");

    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<checkSum>("CheckSum");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<uint8_t>("Temporary");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addField_T<UserId_type>("OwnerId");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)


    wMasterDic.print(stdout);


    ZKeyDictionary wK0Dic("First index",&wMasterDic);

    wSt=wK0Dic.addFieldToZKeyByName("Documentid");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    ZKeyDictionary wK1Dic("Second index",&wMasterDic);

    wSt=wK1Dic.addFieldToZKeyByName("Desc");
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)


    long wKeyRank=0;
    wSt=wMasterDic.addKey(&wK0Dic,wKeyRank);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=wMasterDic.addKey(&wK1Dic,wKeyRank);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    utf8String wXmlDicString = wMasterDic.toXml(1);

    uriString wXmlDic;

    wXmlDic = "/home/gerard/Development/zmftest/masterdictionary.xml";

//    wXmlDic.writeContent(wXmlDicString);


    setZMFStatistics(false);        // default value

    ZException.setAbortOnSeverity(Severity_Severe);

    ZMasterFile ZPicture;

    ZMasterFile ZPicture1;

    addVerbose(ZVB_ZMF);

    printf ("**Main** zcreate \n");

    /* create file and do neither create backup nor leave it open */

    wSt=ZPicture.zcreateMasterFile(&wMasterDic,__TEST_FILE__,100000,false,false );
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wSt=ZPicture.zopen(ZRF_All | ZRF_Exclusive );
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    ZPicture._testZReserved();

    wXmlDicString =ZPicture.MasterDic->toXml(1);
    wXmlDic.writeContent(wXmlDicString);

    uriString  wIndexFileDirectoryPath, wIndexURI;

    wSt=ZPicture.zcreateIndexFromDictionary(
                              "First index",        // user defined name to search
                              ZST_NODUPLICATES,     // Allowing duplicates or not : not allowing duplicates
                              wKeyRank);               // don't replace file if ever exists

    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)


    wSt=ZPicture.zcreateIndexFromDictionary(
                              "Second index",        // user defined name
                              ZST_NODUPLICATES,     // Allowing duplicates or not : not allowing duplicates
                              wKeyRank);               // don't replace file if ever exists

    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    ZPicture._testZReserved();

    wSt=ZPicture.zclose();
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    printf ("**Main** zopen\n");
    _SET_ZMFVERBOSE_

    wSt=ZPicture.zopen(__TEST_FILE__,ZRF_All);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

// populate the file

    ZPicture._testZReserved();

 //   wDoc.URI = "/home/gerard/refman.rtf";

    wDoc.URI = __TEST_DIRECTORY__ ;
    wDoc.URI +=  "testdocument.pdf";
    wDoc.Content=new utf8String;
    wDoc.URI.loadUtf8(*wDoc.Content);


    wDoc.DocSize= wDoc.Content->getByteSize();
    wDoc.CheckSum=wDoc.Content->getcheckSum();


    wDoc.Documentid = ZResourceCode(ZEntity_DocPhysical,1);  /* fake document id */
    wDoc.Short = "Short document description";
    wDoc.Desc = "Document text with full description";
    wDoc.LastModified=ZDateFull::currentDateTime();
    wDoc.Created = ZDateFull::currentDateTime();
    wDoc.Registrated = ZDateFull::currentDateTime();

    wDoc.MimeName =(const utf8_t*)"application/rtf";

    ZRecord* wRecord=ZPicture.generateRecord();

    wRecord->prepareForFeed();

    wDoc.Documentid=123456789;

    docFeed(wRecord,wDoc);

    printf ("**Main** before adding record 0\n");
    wRecord->checkFields();
    wRecord->FieldPresence.print();

    printf ("**Main** Add record 0 \n");

    _SET_ZMFVERBOSE_
    _SET_ZRFVERBOSE_

    wSt=ZPicture.zadd(wRecord);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    ZPicture._testZReserved();

    printf ("\n\n**Main** Adding record 1\n");
    wRecord->prepareForFeed();

    wDoc.URI = __TEST_DIRECTORY__ ;
    wDoc.URI +=  "compta-remarques";
    wDoc.URI.loadUtf8(*wDoc.Content);
//    wDoc.URI.loadUtf8(wUtf8Content);

    wDoc.DocSize= wDoc.Content->getByteSize();
    wDoc.CheckSum=wDoc.Content->getcheckSum();

    wDoc.Documentid = ZResourceCode(ZEntity_DocPhysical,1);  /* fake document id */
    wDoc.Short = "Compta remarques";
    wDoc.Desc = "Remarques sur la comptabilité";
    wDoc.LastModified=ZDateFull::currentDateTime();
    wDoc.Created = ZDateFull::currentDateTime();
    wDoc.Registrated = ZDateFull::currentDateTime();

    wDoc.MimeName =(const utf8_t*)"application/rtf";

    wDoc.Documentid=5;

    docFeed(wRecord,wDoc);


    wRecord->checkFields();
    wRecord->FieldPresence.print();

    wSt=ZPicture.zadd(wRecord);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    ZPicture._testZReserved();

    /* record # 2 */
  printf ("\n\n**Main** Adding record 2\n");
    wRecord->prepareForFeed();

    wDoc.URI = __TEST_DIRECTORY__ ;
    wDoc.URI +=  "testodtdocument.odt";
    wDoc.URI.loadUtf8(*wDoc.Content);
 //   wDoc.URI.loadUtf8(wUtf8Content);

    wDoc.DocSize= wDoc.Content->getByteSize();
    wDoc.CheckSum=wDoc.Content->getcheckSum();

    wDoc.Documentid = ZResourceCode(ZEntity_DocPhysical,1);  /* fake document id */
    wDoc.Short = "Meeting minutes";
    wDoc.Desc = "Meeting minutes sample of odt document";
    wDoc.LastModified=ZDateFull::currentDateTime();
    wDoc.Created = ZDateFull::currentDateTime();
    wDoc.Registrated = ZDateFull::currentDateTime();

    wDoc.MimeName =(const utf8_t*)"application/rtf";

    wDoc.Documentid=5;

    docFeed(wRecord,wDoc);


    wRecord->checkFields();
    wRecord->FieldPresence.print();
    wRecord->RecordCheckAndMap(wRecord->RawContent);

    wSt=ZPicture.zadd(wRecord);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    ZPicture._testZReserved();

    /* record # 3 */
  printf ("\n\n**Main** Adding record 2\n");
    wRecord->prepareForFeed();

    wDoc.URI = __TEST_DIRECTORY__ ;
    wDoc.URI +=  "FIVB Meeting minutes 2017-06-06.doc";
    wDoc.URI.loadUtf8(*wDoc.Content);
    //   wDoc.URI.loadUtf8(wUtf8Content);

    wDoc.DocSize= wDoc.Content->getByteSize();
    wDoc.CheckSum=wDoc.Content->getcheckSum();

    wDoc.Documentid = ZResourceCode(ZEntity_DocPhysical,1);  /* fake document id */
    wDoc.Short = "FIVB Meeting minutes";
    wDoc.Desc = "FIVB Meeting minutes sample of windows doc format";
    wDoc.LastModified=ZDateFull::currentDateTime();
    wDoc.Created = ZDateFull::currentDateTime();
    wDoc.Registrated = ZDateFull::currentDateTime();

    wDoc.MimeName =(const utf8_t*)"application/rtf";

    wDoc.Documentid=6;

    docFeed(wRecord,wDoc);

    wSt=ZPicture.zadd(wRecord);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    ZPicture.zclose();

//    ZRandomFile::zfullDump(__TEST_FILE__);

    delete wRecord;

    ZException.setAbortDefault();
    ZException.setThrowDefault();
#ifdef __COMMENT__
zxmlDoc wXMLDoc;

    ZException.setAbortOnSeverity(Severity_Error);
    wSt=wXMLDoc.createDomDocument("1.0");

    zxmlElementPtr wRoot=nullptr,wxmlRec=nullptr;
    zxmlAttributePtr wAttribute=nullptr;
    wSt=wXMLDoc.createBlankRootElement(wRoot,"ZMasterFile",nullptr);
   _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS);

    utfdescString wVersion ;
    wVersion.sprintf("%ld");
    wRoot->newAttribute(wAttribute,"ZRFVersion",(char*)utfdescString().sprintf("%d",__ZRF_VERSION__).toString());
    wRoot->newAttribute(wAttribute,"ZMFVersion",(char*)utfdescString().sprintf("%d",__ZMF_VERSION__).toString());

    FILE* wXMLFile=fopen("ZRecord.xml","w");
    wRecord->writeXML(wXMLFile);
    fclose(wXMLFile);


    wSt=wRecordIn->createXMLRecord(wxmlRec);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
    wRoot->addChildList((zxmlNodePtr)wxmlRec);
    wXMLDoc.logicalDump(stdout,4);

    wSt=wXMLDoc.writeDOMDocumentEncoded("ZXMLRecord.xml","UTF-8",true);
#endif // __COMMENT__

    wSt=ZPicture.zopen(__TEST_FILE__,ZRF_All);

    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)


    ZRecord* wRecordIn=ZPicture.generateRecord();
    wSt=ZPicture.zget(wRecordIn,0L);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

    wRecordIn->RecordCheckAndMap(wRecordIn->RawContent);

    wSt=ZPicture.zprintIndexFieldValues(0L,0L,true,true); // printf index fields for index OL rank 0L (first raw of first index

    wSt=getDoc(wRecordIn,wDoc);
    _ASSERT_ZEXCEPTION_(wSt!=ZS_SUCCESS)

//#define _TestRepairTools_

#ifdef _TestRepairTools_
    wSt= ZMasterFile::zrepairIndexes("/home/gerard/testindexedpictures.zmf",true,true,stdout);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();

    ZMasterFile::zdowngradeZMFtoZRF("/home/gerard/testindexedpictures.zmf",stdout);


    ZMasterFile::zupgradeZRFtoZMF("/home/gerard/testindexedpictures.zmf",stdout);

    wSt=ZMasterFile::zapplyXMLIndexRankDefinition( "/home/gerard/testIndex.xml",
                                              "/home/gerard/testindexedpictures.zmf",
                                              true,  // apply for real or not
                                              stdout);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();

    wSt=ZMasterFile::zapplyXMLFileDefinition( "/home/gerard/testindexedpictures_new.xml",
                                              nullptr,  // nullptr means : seek for uri into xml definition
                                              true,  // apply for real or not
                                              stdout);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
#endif




//#define _Creation_
#ifdef _Creation_

    ZMasterFile ZPicture ;
    // we do not want to leave the file open (pLeaveOpen is defaulted to false)

    wSt=ZPicture.zcreate("/home/gerard/testindexedpictures.zmf",100000,false,false );
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
0
    if (ZPicture.setParameters(true,
                               false,
                               10000)!=ZS_SUCCESS)
                                   ZException.exit_abort();
/*
// setting jounaling parameters
    uriString wU("gabu.jnl");
    if (ZPicture.setJournalLocalDirectoryPath(wU)!=ZS_SUCCESS)
                                   ZException.exit_abort();
    if (ZPicture.initJournaling(wU)!=ZS_SUCCESS)
                                   ZException.exit_abort();

    wSt=ZPicture.setJournalingOn();  // start journaling on for the file
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
*/
// define index keys
//
//            one index key on String1 then NumericField
//
  typedef decltype (T3::String1) T3String1;
//  wSt=wDic.zaddField<T3String1> ("String1",offsetof (T3,String1));
  if (wSt!=ZS_SUCCESS)
              ZException.exit_abort();
    // dumping field list content
    //

// Fields list is created then create the Index key itself (create the physical files)
//     At this occasion, you should mention the key specifications
//

    wSt=ZPicture.zcreateIndex(wDic,                 // the key Dictionary that just has been set
                              "First Index",        // user defined name
                              ZST_NODUPLICATES,    // Allowing duplicates or not : not allowing duplicates
                              false);                // don't replace file if ever exists

    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }
    wDic.clear();
    typedef decltype (T3::NumericField) T3Numeric;
//    wSt=wDic.zaddField<T3Numeric>("NumericField",offsetof (T3,NumericField));
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();

    wSt=ZPicture.zcreateIndex(wDic,                 // the key Dictionary that just has been set
                              "Second Index",        // user defined name
                              ZST_NODUPLICATES,    // Allowing duplicates or not : not allowing duplicates
                              true);                // replace index file if ever exists

    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }

    wDic.clear();
//    typedef decltype (T3::Type) T3Type;
// enum is converted to its integral type
//    wSt=wDic.zaddField<int>("Type",offsetof (T3,Type));
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();

    wSt=ZPicture.zcreateIndex(wDic,                 // the key Dictionary that just has been set
                              "Third Index",        // user defined name
                              ZST_DUPLICATES,    // Allowing duplicates or not :  allowing duplicates
                              false);              // replace index file if ever exists

    if (wSt!=ZS_SUCCESS)
            {
            ZException.exit_abort();
            }

// At this stage
//         Master file has been created with its header
//         Index file has been created with its own header
//
//     Nota Bene : if you wish to store Index files (All Index files) you need to specify a location
//                 using the method setIndexFilesDirectoryPath giving a valid, existing directory specification
//                 This action should be taken BEFORE calling zcreateIndexFile method.

    if ((wSt=ZPicture.zopen(ZRF_Write))!=ZS_SUCCESS) // load documents
                {
              wRecord      ZException.exit_abort();
                }

    ZPicture.ZMCBreport();

//   if (ZPicture.zstartIndexPMSMonitoring(0)!=ZS_SUCCESS) // let's start monitoring index 0 : a timer is set here
//            wRecordZException.printUserMessage();

   for (int wi = 0;wi < 11;wi++)
        {
 //           wRecord.setData( &T1Cst[wi],sizeof(T1));  // get the occurrence of T1
 //           memmove(&wT3 , &T3Cst[wi], sizeof(T3));     // get the occurrence of T3Cst
//              // get the occurrence of T1

            wURIPicture = PictureFiles[wi];         // set document path
            //----------Hereafter : test the extension file to define the content format -----------
            // NB: this is very raw and not corresponding to real world (see Mime types)

            if ((wURIPicture.getFileExtension()=="jpg")||(wURIPicture.getFileExtension()=="JPG"))
                    wT3.Type = DT_Jpeg ;
                else
              wRecord  if ((wURIPicture.getFileExtension()=="mp4")||(wURIPicture.getFileExtension()=="MP4"))
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
                            ZException.exit_abort();
                        }
            wRecord += wPicture;                    // append document content to record buffer
            if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) // load document
                        {
                        ZPicture.zclose();
                        ZException.exit_abort();
                        }
        }// for

//    ZPicture.zstopIndexPMSMonitoring(0); // stop monitoring
//    ZPicture.zstopIndexPMSMonitoring(1); // stop  monitoring
//    ZPicture.zreportIndexPMSMonitoring(0,stdout); // get results for index 0
//    ZPicture.zreportIndexPMSMonitoring(1,stdout); // get results for index 1
/*
   fprintf (stdout,"=============================removing record #4\n");


   if (ZPicture.zremoveByRank(4)!=ZS_SUCCESS)
                ZException.exit_abort();

// printing all indexes value

   bool wHeader = true;
    wi = 0;
    wSt=ZS_SUCCESS;
    while (wSt==ZS_SUCCESS)
            {
          wSt=ZPicture.zprintIndexFieldValues(0,wi,wHeader,false,stdout);
          wHeader=false;
          wi ++;
            }


*/

    ZPicture.zclose();

    addVerbose(ZVB_ZMF);
#endif // _Creation_


//#define _TestXML_
#ifdef _TestXML_
   wSt=ZPicture.zopen(ZRF_Read_Only);
   if (wSt!=ZS_SUCCESS)
               ZException.exit_abort();

    ZPicture.zwriteXML_FileHeader();

    wSt=ZPicture.zwriteXML_IndexDefinition(0L);
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
    wSt=ZPicture.zwriteXML_IndexDefinition(1L);
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
    wSt=ZPicture.zwriteXML_IndexDefinition(2L);
    if (wSt!=ZS_SUCCESS)
                ZException.exit_abort();
/*   ZIndexControlBlock wZICB;
   wSt=ZPicture.zloadXML_Index( "/home/gerard/testindexedpictures-First Index00.xml",wZICB);
   if (wSt!=ZS_SUCCESS)
           ZException.exit_abort();


   CZKeyDictionary wDicPtr;
   wSt=ZPicture.zloadXML_Dictionary("/home/gerard/testindexedpictures-First Index00-kdic.xml",wDicPtr);
   if (wSt!=ZS_SUCCESS)
           ZException.exit_abort();
   wDicPtr.print();
   */
#endif // _TestXML_

//#define _TestSequence_
#ifdef _TestSequence_
// search for multiple sequences/fields

// selection argument :  string1 contains "heart" AND numeric[0] greater 110.0 OR numeric contains -1024 : Expected result ranks 1 6 8

    ZRandomFile ZRFPicture;

    wSt=ZRFPicture.zopen("/home/gerard/testindexedpictures.zmf",ZRF_Read_Only);

   ssize_t wOffset ;
   ssize_t wLength ;
   float wValueToSearch;

   ZDataBuffer wSearchSequence;
   ZRFCollection wCollection(&ZRFPicture);

   wSearchSequence = "HeArT";
   wOffset = offsetof (T3,String1);
   wLength = sizeof(T3::String1);

   wCollection.addArgument(wOffset,wLength,wSearchSequence,ZRFContains|ZRFString|ZRFCaseRegardless);  // first argument is always ANDed (default)

   wOffset = offsetof (T3,NumericField);
   wLength = sizeof(float);
   wValueToSearch = 110;
   wSearchSequence.setData(&wValueToSearch, sizeof (wValueToSearch));

    wCollection.addArgument(wOffset,wLength,wSearchSequence,ZRFGreaterThan,false);  // ANDed

    wOffset = offsetof (T3,NumericField);
    wLength = sizeof(float)*3;
    wValueToSearch = -1024;
    wSearchSequence.setData(&wValueToSearch, sizeof (wValueToSearch));
wRecord
    wCollection.addArgument(wOffset,wLength,wSearchSequence,ZRFContains,true); // ORed

    wCollection.reportArgument();


   wSt= ZRFPicture.zsearchFieldAllCollection  (wCollection,
                                               ZLock_Nolock,
                                               nullptr);


T3* wT31;



//   ZDataBuffer wRecord;
   fprintf (stdout,
            " idx   Rank    String1                          Numeric      Type\n");
   for (long wi=0;wi<wCollection.size();wi++)
   {
       wSt=ZRFPicture.zget(wRecord,wCollection[wi]);
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

//   ZRFPicture.zclose();

//   T3 * wT31;
   long wRank;
   #define _TestCollection1_
   #ifdef _TestCollection1_

// restrain this collection to records of this collection containing "HeaRt"

   wCollection.reset();

   wCollection.reset();
   wOffset = offsetof(T3,Type);
   wLength = sizeof(Doc_type);
   Doc_type wType = DT_Mp4 ;
   wSearchSequence.setData(&wType,sizeof(Doc_type));
   wCollection.addArgument(wOffset,wLength,wSearchSequence,ZRFEqualsTo);

   fprintf (stdout,
            " idx   Rank    String1                                     Numeric                            Type\n");
   wSt= ZRFPicture.zsearchFieldFirstCollection (wRecord,
                                                wRank,
                                                wCollection,
                                                ZLock_Nolock,
                                                nullptr);
   wi =0;
  while (wRank>-1)
      {
          wSt=ZRFPicture.zget(wRecord,wRank);
          if (wSt!=ZS_SUCCESS)
               ZException.exit_abort();
          wT31 = (T3*)wRecord.Data ;

          fprintf (stdout,
                   " %3ld > %3ld : <%50s> <%f> <%f> <%f> <%d>\n",
                   wi,
                   wCollection.last(),
                   wT31->String1,
                   wT31->NumericField[0],
                   wT31->NumericField[1],
                   wT31->NumericField[2],
                   (int)wT31->Type);

          wRank= ZRFPicture.zsearchFieldNextCollection (wRecord,
                                                        wRank,
                                                        wCollection);
          wi ++;
      }//while

   ZRFPicture.zclose();
#endif //_TestCollection1_


#endif//_TestSequence_

// Searching ZMasterFile for keys and adHoc fields values
//#define _TestSearch_
#ifdef _TestSearch_
ZMasterFile ZPicture ;
   ZPicture.zopen("/home/gerard/testindexedpictures.zmf",ZRF_Read_Only); // open as read only

   ZPicture.zheaderDump();


   ZKey *wKey = ZPicture.createZKey(0L); // create a key object for index number
   if (wKey==nullptr)
        {
        ZPicture.zclose();
        ZException.exit_abort();
        }

ZIndexCollection wIndexCollection;

const char* wS="my heart";


    ZPicture.zprintIndexFieldValues(0L,0L,true,false,stdout);
    for (long wi=1;wi<ZPicture.zgetIndexSize(0L);wi++)
    {
      ZPicture.zprintIndexFieldValues(0L,wi,false,false,stdout);
    }


    wKey->setFieldValue(0L,wS,-1);

    wKey->setKeyPartial("String1",strlen(wS));

    ZPicture.zsearchAll(*wKey,0L,wIndexCollection);

    fprintf (stdout,
             " ---------Index collection ------\n");
    for (long wi=0;wi<wIndexCollection.size();wi++)
        fprintf(stdout,
                " %ld >> index rank <%ld> zmfaddress <%lld>\n",
                wi,
                wIndexCollection[wi].IndexRank,
                wIndexCollection[wi].ZMFAddress);


    fprintf (stdout,
             " idx   Rank  Address %50s %54s Type\n",
             "String1",
             "Numeric array");
    for (long wi=0;wi<wIndexCollection.size();wi++)
        {
            wSt=ZPicture.zgetByAddress(wRecord,wIndexCollection[wi].ZMFAddress);
            if (wSt!=ZS_SUCCESS)
                 {
                 ZPicture.zclose();
                 ZException.exit_abort();
                 }
            wT31 = (T3*)wRecord.Data ;

            fprintf (stdout,
                     " %3ld > %3ld %10lld : <%50s> <%10f> <%10f> <%10f> <%d>\n",
                     wi,
                     wIndexCollection[wi].IndexRank,
                     wIndexCollection[wi].ZMFAddress,
                     wT31->String1,
                     wT31->NumericField[0],
                     wT31->NumericField[1],
                     wT31->NumericField[2],
                     (int)wT31->Type);
        }//for
    ZPicture.zclose();


   float  pV[] = {110 ,-1024,1};


//   std::string wS="my heart beats for you";

//   wKey->setFieldValue(0L,pV,3);
   wKey->setFieldValue("NumericField",pV,3);

//   wKey->setFieldValueT<decltype(wS)>(1L,wS);

   wKey->setFieldValue(0L,wS,-1);

   // let's see what we have got

   wKey->zprintKeyFieldsValues(true, true,stdout);

   wSt=ZPicture.zsearch(wRecord,wKey);

   delete wKey;

   ZKey *wKey1 = ZPicture.createZKey(0L);
   wKey1->keyValueExtraction(wRecord);

   wKey1->zprintKeyFieldsValues(true, true,stdout);


   wKey=ZPicture.createZKey(1L);



   return(0);

//   ZRandomFile::zsurfaceScan("/home/gerard/testindexedpictures.zrf");

   fprintf(stdout,"--------Putting the mess------------\n");
   ZPicture.zopen(ZRF_Exclusive);
    ZPicture.putTheMess();
    ZPicture.zclose();

    ZRandomFile::zsurfaceScan("/home/gerard/testindexedpictures.zmf");



    wSt=ZPicture.zreorgFile(true);
    if (wSt!=ZS_SUCCESS)
            ZException.exit_abort();
//    ZPicture.zcloneFile();
//   ZPicture.zclose();

#endif // _TestSearch_


#ifdef __COMMENT__
    ZPicture.zopen(ZRF_Read);
   printIndexKey_0(ZPicture);
    ZPicture.zclose();


   ZRandomFile::zheaderDump("/home/gerard/testindexedpictures.zmf");  //! using static features

   if (ZRandomFile::ZRFstat("/home/gerard/testindexedpictures.zmf")!=ZS_SUCCESS)
                                                ZException.exit_abort();

   ZRandomFile::zsurfaceScan("/home/gerard/testindexedpictures.zmf");

//
// search test
//

   ZKey *wKey = ZPicture.createKeyValue(0);  //! Create a key structure for index number 0

   wKey->set<int>(155,0);
   wKey->setString("my heart beats",1);


   wKey->Dump(32,stdout);

 //  if (ZPicture.zgenerateKeyValue(wKey,0,2,155,wField1) !=ZS_SUCCESS)
 //                                               ZException.exit_abort();
   if (ZPicture.zsearch (wRecord,wKey)!=ZS_FOUND)
            ZException.exit_abort();
    return (0);
//!
//! delete 3 adjacent records
//!
   fprintf (stdout,"\n\n---------------------------------\n"
            "                 remove test\n");

   ZPicture.zopen("testpictures",ZRF_Write);

   ZBlockDescriptor wBD;

   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZException.exit_abort();


   fprintf (stdout,"=============================removing record #4 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);

   wTimer.start();

   if (ZPicture.zremove(4)!=ZS_SUCCESS)
                ZException.exit_abort();

   wTimer.end();
    fprintf (stdout,
             " removing time %s \n",
             wTimer.reportDeltaTime().toString());


        ZRandomFile::zsurfaceScan("testpictures");



   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZException.exit_abort();


   fprintf (stdout,"\n===============================removing record #3 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);
   wTimer.start();
   if (ZPicture.zremove(3)!=ZS_SUCCESS)
                ZException.exit_abort();
   wTimer.end();
    fprintf (stdout,
             " removing time %s \n",
             wTimer.reportDeltaTime().toString());

         ZRandomFile::zsurfaceScan("testpictures");

   if (ZPicture.zgetBlockDescriptor(2L,wBD)!=ZS_SUCCESS)
                                ZException.exit_abort();


   fprintf (stdout,"\n===============================removing record #2 address %lld block size %lld\n", wBD.Address, wBD.BlockSize);

   if (ZPicture.zremove(2)!=ZS_SUCCESS)
                ZException.exit_abort();


//   if (ZRandomFile::_ZRFstat("testpictures")!=ZS_SUCCESS)
//                                                ZException.exit_abort();
   ZRandomFile::zsurfaceScan("testpictures");
#endif // _COMMENT__
#ifdef __COMMENT__

   fprintf (stdout,"\n===============================recovering free pool block  #0\n");

   long wIndex;ment
               {
                   ZException.exit_abort();
               }

   if ((wSt=ZPicture.zrecover(0L,wIndex))!=ZS_SUCCESS) //! load docu
   ZRandomFile::zsurfaceScan("testpictures");

//
// Add 3 more records  from Table #6  (huge one)  #10 #11
//

   wRecord.setData( &T1Cst[6],sizeof(T1));  // get the occurrence of T1
   wURIPicture = PictureFiles[6];         // set document path
   if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) // load document
                                                ZException.exit_abort();

   wRecord += wPicture;                    // append document content to record buffer
   if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) // load document
                   ZPicture.zsurfaceScan("testpictures");
   for (int wi = 10;wi < 12;wi++)
        {
            wRecord.setData( &T1Cst[wi],sizeof(T1));  //! get the occurrence of T1
            wURIPicture = PictureFiles[wi];         //! set document path
            if ((wSt=wURIPicture.loadContent(wPicture))!=ZS_SUCCESS) //! load document
                        {
                            ZException.exit_abort();
                        }

            wRecord += wPicture;                    //! append document content to record buffer
            if ((wSt=ZPicture.zadd(wRecord))!=ZS_SUCCESS) //! load document
                        {
                            ZException.exit_abort();
                        }
        }

   ZPicture.zclose();

      ZPicture.zsurfaceScan("testpictures");

#endif // __COMMENT__
//
// let's add some rows into ZAM1 and let's see what happens to the dynamic join
//
/*
    struct T1 T1Cst[] =
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
*/
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




return (0);


} // main

