#ifndef ZSKEY_H
#define ZSKEY_H
#include <cstdarg>
#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zsindexfile.h>

/**
 * @addtogroup ZSKeyGroup
 * @{ */
//---------------Adhoc key creation : Build Key from Value routines----------------------------------------------------
/**
@weakgroup ZSKey class  this object manages one key (given by KeyNumber) for a ZSMasterFile using its ZSIndexControlBlock (ZICB)
 *
 *  ZKey class is used to format a key content for being used with ZMasterFile in queries.
 *  A key is dedicated to a ZSMasterFile for a dedicated Index
 *
 *
 *  The objective is to offer the whole set of functionalities to create a key,
 *  either a search key or even a key content from a ZMF record.
 *
 *  ZKey imbeds all appropriate data type analyse and conversion routines.
 *
 *  A ZKey object is obtained "ready to use" from a ZMasterFile using
 * - ZMasterFile::createZKey() : create a ZKey corresponding to given ZMF index's rank
 * - ZMasterFile::createZKeyByName() : create a ZKey corresponding to given ZMF index's name
 *
 * @note As these routines deliver instantiated object with 'new' clause,
 *  this is of the user's responsibility to delete the object once it is no more in use.
 *
 *  Both routines give back a pointer to a ZKey object loaded with the appropriate key dictionary.
 *  Once it has been created, this object may be loaded with value using :
 * - setFieldValue() when loading key with litterals or data for search
 * - keyValueExtraction() for extracting a key content from a ZMasterFile user record.
 *
 *
 *  Key value may be searched with
 * - EXACT value correspondance : this means that both content and size must strictly correspond between search key content and index kex content
 * - PARTIAL value correspondance : Key content represents partial content of index key content to search for (starting from beginning of field).
 *
 *  Partial key option is set with ZKey::setKeyPartial() routine.
 *
 */

class ZSKey : public ZDataBuffer
{
    friend class ZSMasterFile;
    friend class ZSIndexFile;

    typedef ZDataBuffer _Base;
public:

    ZSKey(ZSMasterFile *pZMF, const long pKeyRank) ;
    ZSIndexControlBlock *ZICB;          //!<  pointer to ZSMasterFile ZICB element (also stored within ZIndexFile )
    ZSMasterFile*        ZMF;           //!< pointer to ZSMasterFile
    long                IndexNumber;    //!<  ZMasterFile Index number (rank)


   ZArray<bool> FieldPresence;

   bool FPartialKey=false;  //!< Flag partial key : if partial key then
   ssize_t PartialLength=-1;

   template <class _Tp>
   friend
   ZStatus
   zsetKeyFieldValue (_Tp pValue, ZSKey & pZKey,const long pFieldRank,ssize_t pArrayNumber);

   long getSize(void)  {return(long) Size;}

//   using ZDataBuffer::allocate;
   using ZDataBuffer::clearData;
   using ZDataBuffer::Dump;

   ZStatus keyValueExtraction(ZRecord *pRecord) ;

   ZStatus setFieldValue (const long pFieldRank,auto pValue, const long pArraySize=-1);
   ZStatus setFieldValue (const utf8_t* pFieldName,auto pValue, const long pArraySize=-1);

   ZStatus setKeyPartial (const ssize_t pFieldRank ,const ssize_t pLength);
   ZStatus setKeyPartial (const utf8_t* pFieldName,ssize_t pLength);

   template <class _Tp>
   ZStatus setFieldValueT (const long pFieldRank,_Tp pValue, const long pArraySize=-1);

   template <class _Tp>
   ZStatus setFieldValueT (const utf8_t* pFieldName,_Tp pValue, const long pArraySize=-1);

    ZStatus setString (const long pFieldRank,std::string &pString );
    ZStatus setString (const long pFieldRank, const char* pString);

    void zprintKeyFieldsValues (bool pHeader=true,bool pKeyDump=false,FILE*pOutput=stdout);

   void clearFieldSpace (const long pFieldRank);

   ZSKey& setFieldRawValue (const void* pValue,const ssize_t pSize,const long pFieldRank);
   ZSKey& setFieldRawValue (ZDataBuffer& pValue,const long pFieldRank);


   void clearData(void)
    {
       _Base::clearData();      // set ZDataBuffer to zero
       FieldPresence.bzero();
       FPartialKey = false;
       PartialLength = -1;
       return;
   }

};


#endif // ZSKEY_H
