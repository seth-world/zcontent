#ifndef ZSKEY_H
#define ZSKEY_H
#include <cstdarg>
#include <zrandomfile/zrandomfile.h>
#include <zindexedfile/zrawindexfile.h>

/**
 * @addtogroup ZSKeyGroup
 * @{ */
//---------------Adhoc key creation : Build Key from Value routines----------------------------------------------------
/**
@weakgroup ZSKey class  this object manages one key (given by KeyNumber) for a ZMasterFile using its ZSIndexControlBlock (ZICB)
 *
 *  ZKey class is used to form#include <zindexedfile/zsindexitem.h>at a key content for being used with ZMasterFile in queries.
 *  A key is dedicated to a ZMasterFile for a dedicated Index
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
namespace zbs {

class ZKey : public ZDataBuffer
{
    friend class ZMasterFile;
    friend class ZIndexFile;

    typedef ZDataBuffer _Base;
public:

    ZKey(ZRawMasterFile  *pZMF, const long pKeyRank) ;
    ZRawIndexFile*      ZIF=nullptr;     //!<  pointer to ZMasterFile ZICB element (also stored within ZIndexFile )
    ZRawMasterFile*     ZMF=nullptr;      //!< pointer to ZMasterFile
    long                IndexNumber;      //!<  ZMasterFile Index number (rank)


   ZArray<bool> FieldPresence;

   bool FPartialKey=false;  //!< Flag partial key : if partial key then
   ssize_t PartialLength=-1;

   template <class _Tp>
   friend
   ZStatus
   zsetKeyFieldValue (_Tp pValue, ZKey & pZKey,const long pFieldRank,ssize_t pArrayNumber);

   long getSize(void)  {return(long) Size;}

//   using ZDataBuffer::allocate;
   using ZDataBuffer::clearData;
   using ZDataBuffer::Dump;

//   ZStatus keyValueExtraction(ZRecord *pRecord) ;

//   ZStatus setFieldValue (const long pFieldRank,auto pValue, const long pArraySize=-1);
//   ZStatus setFieldValue (const utf8_t* pFieldName,auto pValue, const long pArraySize=-1);

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

   ZKey& setFieldRawValue (const void* pValue,const ssize_t pSize,const long pFieldRank);
   ZKey& setFieldRawValue (ZDataBuffer& pValue,const long pFieldRank);


   void clearData(void)
    {
       _Base::clearData();      // set ZDataBuffer to zero
       FieldPresence.bzero();
       FPartialKey = false;
       PartialLength = -1;
       return;
   }

};


template <class _Tp>
/**
 * @brief ZKey::setFieldValueT Template routine that loads the value (of type _Tp) pValue to the key field (defined in key dictionary)
 *  with a name that matches given string.
 * @note
 * Template is necessary in case of an array to keep the native data type.
 * @note
 * pArrayCount is not necessary in case of data type that is anything excepted a pointer.
 * In case of pointer of chars 'char*' (C string), pArrayCount is not necessary because deduced from size of string.
 * In case of pointer to std::string, pArrayCount is not necessary because deduced from size of string.
 *
 * This routine controls the coherence of given data.
 * - It makes the data conversion whenever required to have a dictionary defined data (natural data).
 * - It makes the conversion from natural field to internal key field value format.
 * - It addresses pointers vs arrays.
 * - It pads the key if the size of input data (specifically for arrays) is not large enough.
 * - It truncates the data if given value exceeds defined key field boundaries.
 *
 * @param[in] pFieldRank field order (rank) in key dictionary
 * @param[in] pValue  Value of undertermined type to set the field with
 * @param[in] pArrayCount Usefull value for pointers only, and for
 * It gives the corresponding number of atomic values the pointer points to.
 * If data type is other than pointer, array size value is deduced from data type.
 * A negative value means 'omitted'. Defaulted to -1.
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 * @return
 */
ZStatus
ZKey::setFieldValueT (const utf8_t* pFieldName,_Tp pValue, const long pArrayCount)
{
  long wFieldRank=ZIF->IdxKeyDic->zsearchFieldByName((const utf8_t*)pFieldName);
  if (wFieldRank<0)
    return ZException.getLastStatus();
  return zsetKeyFieldValue<_Tp>(pValue,*this,wFieldRank,pArrayCount);
}



} // namespace zbs
#endif // ZSKEY_H
