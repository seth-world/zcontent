/**  This file has been generated by zcontent utilities generator version 1.0-1
 using dictionary file /home/gerard/Development/zbasetools/zcontent/ztest_zindexedfile/testdata/zcppgenerateparameters.xml
 Generation date 2023-05-08 21:42:13

Attention: This file must not be manually changed in order to preserve the ability to change it using dictionary generation facilities.
-----------*/

#include "/home/gerard/Development/zbasetools/zcontent/test_zrawfile/zdocphysical.h"
#include <ztoolset/zatomicconvert.h>
#include <zindexedfile/zdataconversion.h>


 using namespace zbs;
   ZDocPhysical& ZDocPhysical::_copyFrom(const ZDocPhysical& pIn) {
     Documentid = pIn.Documentid ;
     DataRank = pIn.DataRank ;
     Short = pIn.Short ;
     Desc = pIn.Desc ;
     DocMetaType = pIn.DocMetaType ;
     Storage = pIn.Storage ;
     MimeName = pIn.MimeName ;
     URI = pIn.URI ;
     DocSize = pIn.DocSize ;
     Created = pIn.Created ;
     Registrated = pIn.Registrated ;
     LastModified = pIn.LastModified ;
     OwnerOrigin = pIn.OwnerOrigin ;
     AccessRights = pIn.AccessRights ;
     CheckSum = pIn.CheckSum ;
     Temporary = pIn.Temporary ;
     Ownerid = pIn.Ownerid ;
     Vaultid = pIn.Vaultid ;
    return *this;
 }// _copyFrom

    void ZDocPhysical::clear() {
      Documentid.clear() ;
      DataRank=0L ;
      Short.clear() ;
      Desc.clear() ;
      DocMetaType=0L ;
      Storage=0L ;
      MimeName.clear() ;
      URI.clear() ;
      DocSize=0L ;
      Created.clear() ;
      Registrated.clear() ;
      LastModified.clear() ;
      OwnerOrigin.clear() ;
      AccessRights=0 ;
      CheckSum.clear() ;
      Temporary=0 ;
      Ownerid.clear() ;
      Vaultid.clear() ;
      return; 
   }
ZDataBuffer ZDocPhysical::toRecord() {
   ZDataBuffer wReturn;

   FieldPresence._allocate(18);
   FieldPresence.setAll();
   FieldPresence._exportURF(wReturn);


/*         User content size computation  */
   uint64_t wURFSize=0;
   wURFSize += Documentid.getURFSize();
   wURFSize += getAtomicURFSize<uint64_t>(DataRank);
   wURFSize += Short.getURFSize();
   wURFSize += Desc.getURFSize();
   wURFSize += getAtomicURFSize<int64_t>(DocMetaType);
   wURFSize += getAtomicURFSize<int64_t>(Storage);
   wURFSize += MimeName.getURFSize();
   wURFSize += URI.getURFSize();
   wURFSize += getAtomicURFSize<uint64_t>(DocSize);
   wURFSize += Created.getURFSize();
   wURFSize += Registrated.getURFSize();
   wURFSize += LastModified.getURFSize();
   wURFSize += OwnerOrigin.getURFSize();
   wURFSize += getAtomicURFSize<uint32_t>(AccessRights);
   wURFSize += CheckSum.getURFSize();
   wURFSize += getAtomicURFSize<uint8_t>(Temporary);
   wURFSize += Ownerid.getURFSize();
   wURFSize += Vaultid.getURFSize();

   unsigned char* wPtr = wReturn.extend(wURFSize);
/*              move of individual fields to record space */
   Documentid._exportURF_Ptr(wPtr);
   exportAtomicURF_Ptr<uint64_t>(DataRank,wPtr);
   Short._exportURF_Ptr(wPtr);
   Desc._exportURF_Ptr(wPtr);
   exportAtomicURF_Ptr<int64_t>(DocMetaType,wPtr);
   exportAtomicURF_Ptr<int64_t>(Storage,wPtr);
   MimeName._exportURF_Ptr(wPtr);
   URI._exportURF_Ptr(wPtr);
   exportAtomicURF_Ptr<uint64_t>(DocSize,wPtr);
   Created._exportURF_Ptr(wPtr);
   Registrated._exportURF_Ptr(wPtr);
   LastModified._exportURF_Ptr(wPtr);
   OwnerOrigin._exportURF_Ptr(wPtr);
   exportAtomicURF_Ptr<uint32_t>(AccessRights,wPtr);
   CheckSum._exportURF_Ptr(wPtr);
   exportAtomicURF_Ptr<uint8_t>(Temporary,wPtr);
   Ownerid._exportURF_Ptr(wPtr);
   Vaultid._exportURF_Ptr(wPtr);
    return wReturn;
 }// toRecord

 ZStatus ZDocPhysical::fromRecord(const ZDataBuffer& pRecord) { 
   const unsigned char* wPtr = pRecord.Data;
   const unsigned char* wPtrEnd = pRecord.Data + pRecord.Size;

   FieldPresence.clear();
   FieldPresence._importURF(wPtr);

   int wRank=0;
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + Documentid.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Documentid>.\n");
         return ZS_OUTBOUND;
       }
       Documentid._importURF(wPtr);
     }
   if (FieldPresence.test(wRank++)) {
     if (wPtr + getAtomicURFSize<uint64_t>(DataRank) > wPtrEnd) {
       fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <DataRank>.\n");
       return ZS_OUTBOUND;
     }
     importAtomicURF<uint64_t>(DataRank,wPtr);
   }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + Short.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Short>.\n");
         return ZS_OUTBOUND;
       }
       Short._importURF(wPtr);
     }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + Desc.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Desc>.\n");
         return ZS_OUTBOUND;
       }
       Desc._importURF(wPtr);
     }
   if (FieldPresence.test(wRank++)) {
     if (wPtr + getAtomicURFSize<int64_t>(DocMetaType) > wPtrEnd) {
       fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <DocMetaType>.\n");
       return ZS_OUTBOUND;
     }
     importAtomicURF<int64_t>(DocMetaType,wPtr);
   }
   if (FieldPresence.test(wRank++)) {
     if (wPtr + getAtomicURFSize<int64_t>(Storage) > wPtrEnd) {
       fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Storage>.\n");
       return ZS_OUTBOUND;
     }
     importAtomicURF<int64_t>(Storage,wPtr);
   }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + MimeName.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <MimeName>.\n");
         return ZS_OUTBOUND;
       }
       MimeName._importURF(wPtr);
     }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + URI.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <URI>.\n");
         return ZS_OUTBOUND;
       }
       URI._importURF(wPtr);
     }
   if (FieldPresence.test(wRank++)) {
     if (wPtr + getAtomicURFSize<uint64_t>(DocSize) > wPtrEnd) {
       fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <DocSize>.\n");
       return ZS_OUTBOUND;
     }
     importAtomicURF<uint64_t>(DocSize,wPtr);
   }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + Created.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Created>.\n");
         return ZS_OUTBOUND;
       }
       Created._importURF(wPtr);
     }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + Registrated.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Registrated>.\n");
         return ZS_OUTBOUND;
       }
       Registrated._importURF(wPtr);
     }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + LastModified.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <LastModified>.\n");
         return ZS_OUTBOUND;
       }
       LastModified._importURF(wPtr);
     }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + OwnerOrigin.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <OwnerOrigin>.\n");
         return ZS_OUTBOUND;
       }
       OwnerOrigin._importURF(wPtr);
     }
   if (FieldPresence.test(wRank++)) {
     if (wPtr + getAtomicURFSize<uint32_t>(AccessRights) > wPtrEnd) {
       fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <AccessRights>.\n");
       return ZS_OUTBOUND;
     }
     importAtomicURF<uint32_t>(AccessRights,wPtr);
   }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + CheckSum.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <CheckSum>.\n");
         return ZS_OUTBOUND;
       }
       CheckSum._importURF(wPtr);
     }
   if (FieldPresence.test(wRank++)) {
     if (wPtr + getAtomicURFSize<uint8_t>(Temporary) > wPtrEnd) {
       fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Temporary>.\n");
       return ZS_OUTBOUND;
     }
     importAtomicURF<uint8_t>(Temporary,wPtr);
   }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + Ownerid.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Ownerid>.\n");
         return ZS_OUTBOUND;
       }
       Ownerid._importURF(wPtr);
     }
     if (FieldPresence.test(wRank++)) {
       if ((wPtr + Vaultid.getURFSize()) > wPtrEnd) {
         fprintf(stderr, "ZDocPhysical::fromRecord-F-OUTBOUND Out of input boundaries while getting field <Vaultid>.\n");
         return ZS_OUTBOUND;
       }
       Vaultid._importURF(wPtr);
     }
    return ZS_SUCCESS ;
 }// fromRecord

    /* Keys */
 utf8VaryingString ZDocPhysical::getKey0_Name() {
   return "Primary key";
 }
 ZDataBuffer ZDocPhysical::getKey0() {
   ZDataBuffer wReturn;
   /* export class using embedded URF export routine */
   Documentid._exportURF(wReturn);
   return wReturn;
 } // getKey0 
   ZDataBuffer ZDocPhysical::getKey(long pKeyNumber) {
     switch (pKeyNumber) {
       case 0 :
         return getKey0();
       default :
         fprintf(stderr,"ZDocPhysical::getKey-E-INVVALUE Invalid key number <%ld> while expected [0,0].\n",pKeyNumber);
         return ZDataBuffer();
     } // switch
   } // getKey
   utf8VaryingString ZDocPhysical::getKeyName(long pKeyNumber) {
     switch (pKeyNumber) {
       case 0 :
         return getKey0_Name();
       default :
         fprintf(stderr,"ZDocPhysical::getKey-E-INVVALUE Invalid key number <%ld> while expected [0,0].\n",pKeyNumber);
         return utf8VaryingString();     } // switch
   } // getKeyName
ZArray<ZDataBuffer> ZDocPhysical::getAllKeys() {
   ZArray<ZDataBuffer> wReturn;
   for (long wi=0; wi < 1 ; wi++) {
     wReturn.push(getKey(wi));
   }
   return wReturn;
 } // getAllKeys
