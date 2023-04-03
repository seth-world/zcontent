/**  This file has been generated by zcontent utilities generator version 1.0-1
 using dictionary file /home/gerard/Development/zbasetools/zcontent/ztest_zindexedfile/testdata/zcppgenerateparameters.xml
 Generation date 2023-04-03 22:42:48

Attention: This file must not be manually changed in order to preserve the ability to change it using dictionary generation facilities.
-----------*/

#ifndef ZDOCPHYSICAL_H
#define ZDOCPHYSICAL_H

#include <sys/types.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zarray.h>
/*      private include files        */
#include <ztoolset/zbitset.h>
/*      local include files          */
#include <zcontentcommon/zresource.h>
#include <stdint.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zdate.h>
#include <zcrypt/checksum.h>

/** @brief ZDocPhysical Physical document one key */

class ZDocPhysical {
private:
   ZBitset     FieldPresence;
public:

    ZResource Documentid /*!< Unique document identifier - given by system resource allocator */;
    uint64_t DataRank;
    utf8FixedString<150> Short /*!< Short description of the document. Used for fast access- */;
    utf8VaryingString Desc;
    int64_t DocMetaType;
    int64_t Storage;
    utf8FixedString<150> MimeName;
    uriString URI;
    uint64_t DocSize;
    ZDateFull Created;
    ZDateFull Registrated;
    ZDateFull LastModified;
    ZResource OwnerOrigin;
    uint32_t AccessRights;
    checkSum CheckSum;
    uint8_t Temporary;
    ZResource Ownerid;
    ZResource Vaultid;
/* Public methods */
   ZDocPhysical()=default;
   ZDocPhysical& _copyFrom(const ZDocPhysical& pIn);
   ZDocPhysical& operator = (const ZDocPhysical& pIn) { return _copyFrom(pIn); }
   void clear();
   ZDataBuffer toRecord();
   ZStatus fromRecord(const ZDataBuffer& pRecord);
   zbs::ZArray<ZDataBuffer> getAllKeys();
   inline long getKeysNumber() { return 2; }
   /* Keys definition */
   ZDataBuffer getKey0(); /*  <Primary key> */
   utf8VaryingString getKey0_Name();
   ZDataBuffer getKey1(); /*  <Secondary key (string)> */
   utf8VaryingString getKey1_Name();
   ZDataBuffer       getKey(long pKeyNumber) ;
   utf8VaryingString getKeyName(long pKeyNumber) ;
}; // ZDocPhysical 
#endif //ZDOCPHYSICAL_H
