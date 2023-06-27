#ifndef ZFIELDDESCRIPTION_H
#define ZFIELDDESCRIPTION_H

#include <stdlib.h> //for atexit()
#include <sys/types.h>

#include <ztoolset/zlimit.h>

#include <ztoolset/ztypetype.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/zaierrors.h>
#include <QDataStream> /* for Q_DECLARE_METATYPE */

namespace zbs {

const size_t cst_FieldNameCapacity=cst_fieldnamelen+sizeof(uint8_t)+sizeof(uint16_t);
class ZFieldDescription;
#pragma pack(push)
#pragma pack(1)
/**
 * @brief The FieldDesc_Export class used to export ZFieldDescription structure.
 * This structure does not include Name field, which is processed separately.
 */
class FieldDesc_Export
{
public:
  uint32_t            StartSign=cst_ZFIELDSTART;
  uint16_t            EndianCheck=cst_EndianCheck_Normal;
//  unsigned char   Name[cst_fieldnamelen+sizeof(uint8_t)+sizeof(uint16_t)];       //!< Name of the field with UVF format contained within a fixed Structure
  ZTypeBase           ZType;
  URF_Capacity_type   Capacity; //!< if data type is Array then count the rows 0 if not an array
  uint16_t            HeaderSize; //!< Size of field header
  uint32_t            UniversalSize;  //!< Only if ZType is fixed length. Otherwise set to 0
  uint32_t            NaturalSize;                    //!< idem
  uint8_t             KeyEligible=false ;             //!< could be set as Key field
  unsigned char       Hash[cst_md5];

    FieldDesc_Export()=default;

    FieldDesc_Export& _copyFrom(FieldDesc_Export& pIn);

    FieldDesc_Export(FieldDesc_Export& pIn) {_copyFrom(pIn);}
    FieldDesc_Export(FieldDesc_Export&& pIn) {_copyFrom(pIn);}

    FieldDesc_Export& operator = (FieldDesc_Export& pIn) {return _copyFrom(pIn);}
    FieldDesc_Export& operator = (FieldDesc_Export&& pIn) {return _copyFrom(pIn);}

    FieldDesc_Export& set(ZFieldDescription& pIn);
    void    setFromPtr(const unsigned char *&pPtrIn);
    ZFieldDescription toFieldDescription();

    void _convert();
    void serialize();
    void deserialize();

    bool isReversed() const
    {
      if (EndianCheck==cst_EndianCheck_Reversed) return true;
      return false;
    }
    bool isNotReversed() const
    {
      if (EndianCheck==cst_EndianCheck_Normal) return true;
      return false;
    }

};
/**
 * @brief The FieldDesc_Check class used to compute md5 hashcode
 */
class FieldDesc_Check
{
public:

  //       //!< Name of the field with UVF format contained within a fixed Structure
  ZTypeBase           ZType;
  URF_Capacity_type   Capacity; //!< if data type is Array then count the rows 0 if not an array
  uint16_t            HeaderSize; //!< Size of field header
  uint32_t            UniversalSize;  //!< Only if ZType is fixed length. Otherwise set to 0
  uint32_t            NaturalSize;                    //!< idem
  uint8_t             KeyEligible=false ;             //!< could be set as Key field
  utf8_t              Name[cst_FieldNameCapacity];

  FieldDesc_Check()=default;

  FieldDesc_Check& _copyFrom(FieldDesc_Check& pIn);

  FieldDesc_Check(FieldDesc_Check& pIn) {_copyFrom(pIn);}

  FieldDesc_Check& operator = (FieldDesc_Check& pIn) {return _copyFrom(pIn);}
  FieldDesc_Check& operator = (FieldDesc_Check&& pIn) {return _copyFrom(pIn);}

  FieldDesc_Check& set(ZFieldDescription& pIn);
  ZFieldDescription toFieldDescription();

};

/* used to store field description in ZPinboard */
/**
 * @brief The FieldDesc_Pack class used to pack ZFieldDescription structure, including Name and Hashcode,
 * in order to store it in ZPinboard.
 */
class KeyField_Pack;
class FieldDesc_Pack
{
public:

  //       //!< Name of the field with UVF format contained within a fixed Structure
  ZTypeBase           ZType;
  URF_Capacity_type   Capacity; //!< if data type is Array then count the rows 0 if not an array
  uint16_t            HeaderSize; //!< Size of field header
  uint32_t            UniversalSize;  //!< Only if ZType is fixed length. Otherwise set to 0
  uint32_t            NaturalSize;                    //!< idem
  uint8_t             KeyEligible=false ;             //!< could be set as Key field
  utf8_t              Name[cst_FieldNameCapacity];
  unsigned char       Hash[cst_md5];

  FieldDesc_Pack()=default;

  FieldDesc_Pack& _copyFrom(const FieldDesc_Pack& pIn);

  FieldDesc_Pack(const FieldDesc_Pack& pIn) {_copyFrom(pIn);}

  FieldDesc_Pack& operator = (const FieldDesc_Pack& pIn) {return _copyFrom(pIn);}
  FieldDesc_Pack& operator = (const FieldDesc_Pack&& pIn) {return _copyFrom(pIn);}

  FieldDesc_Pack& set(ZFieldDescription& pIn);
  ZFieldDescription toFieldDescription();

  bool hasHash(const unsigned char* pHash) {return memcmp(Hash,pHash,cst_md5)==0;}
  bool hasSameHash(const FieldDesc_Pack& pIn) {return memcmp(Hash,pIn.Hash,cst_md5)==0;}
  bool hasSameHash(const KeyField_Pack& pIn) ;

  QDataStream& write(QDataStream &dataStream);
  QDataStream& read(QDataStream &dataStream);

  friend   QDataStream &operator << (QDataStream &out, const FieldDesc_Pack &myObj)
  {
    out.writeBytes((const char *)&myObj,sizeof(FieldDesc_Pack)) ;
    return(out);
  }
  utf8VaryingString getName();
  void setName(const utf8VaryingString& pName);

};

#pragma pack(pop)

/**
 * @brief The fieldDesc_struct struct Field definition for ZMetaDic
 *
 *  see ZIndexField_struct in <zindextype.h>
 *  see ZType_type
 *
 *  Nota Bene :
 *  As fields may have varying length, offet and lengths for each field are volatile data.
 *  It is available at ZRecord level within ZRecordDictionary.
 *
 *
 */
class ZFieldDescription
{
  friend class FieldDesc_Export;
  friend class FieldDesc_Pack;
  friend class FieldDesc_Check;
public:
  ZFieldDescription() = default;
  ZFieldDescription (const ZFieldDescription& pIn) { _copyFrom(pIn); }
  ZFieldDescription (const ZFieldDescription&& pIn) { _copyFrom(pIn); }

 ~ZFieldDescription () {  }

  ZFieldDescription& _copyFrom(const ZFieldDescription &pIn);

  ZFieldDescription& operator = (const ZFieldDescription& pIn) { return _copyFrom(pIn); }
  ZFieldDescription& operator = (const ZFieldDescription&& pIn) { return _copyFrom(pIn); }

  ZTypeBase           ZType;
  URF_Capacity_type   Capacity;           //!< if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if an atomic
  uint64_t            HeaderSize;         //!< Size of field header
  uint64_t            UniversalSize;      //!< Only if ZType is fixed length. Otherwise set to 0
  uint64_t            NaturalSize;        //!< Only if ZType is fixed length. Otherwise set to 0
  ZBool               KeyEligible=false ; //!< May be used as Key field (true) or not (false)

  utf8VaryingString   ToolTip;           //!< Help for the field

private: utf8VaryingString  Name;        //!< Name of the field
public:  md5                Hash;        //!< unique hashcode value for the field.

  void clear() ;

  void setFieldName(const utf8VaryingString& pName);

  utf8VaryingString& getName()  {return Name;}
  const utf8VaryingString& getName() const  {return Name;}

  bool hasName(const utf8VaryingString& pName) const;
  bool hasHashCode (const md5& pHashcode) const ;

  /** @brief checkHashcode check if existing hashcode value is still in line with field description content */
  bool checkHashcode() ;

  /** @brief computeMd5() compute md5 hash key with all current data from the field and store it within Hash field. returns computed value */
  md5& computeMd5();
  /** @brief _computeMd5() compute md5 hash key with all current data from the field, WITHOUT storeing it within Hash field and returns computed value */
  md5 _computeMd5();

  bool isAtomic(void) {return ZType & ZType_Atomic ;}
  bool isArray(void) {return ZType & ZType_Array ;}
  bool isByteSeq(void) {return ZType & ZType_ByteSeq ;}
  bool isBlob(void) {return ZType & ZType_Blob ;}
  bool isErrored(void){return (ZType==ZType_Unknown)||(ZType&ZType_Class);}  // ZType_Class is not allowed
  bool isVarying() {return (ZType&ZType_VaryingLength);}
  bool isSigned(void) {return ZType & ZType_Signed ;}
  bool isEndian(void) {return ZType & ZType_Endian ;}

  utf8VaryingString toXml(int pLevel, bool pComment=true);
  /**
   * @brief fromXml  populate Field description's attributes with xml content whose root is given by pFieldRootNode.
   * @param pFieldRootNode root node for field to load : must point to <field>
   * @param pCheckHash    if set, this option will induce a computation of hashcode and a comparizon with the existing one.
   *                      if comparizon do not match, a warning message is logged
   * @param pErrorlog     Pointer to message logging object
   * @param pSeverity     Severity level to
   * @return ZS_SUCCESS if Ok,
   * ZS_XMLWARNING if a field had some warning(s)  but may be taken as valid
   * ZS_XMLMISREQ some required node(s) is/are missing
   * ZS_XMLINVROOTNAME <field> root node is missing in pFieldRootNode
   */
  ZStatus fromXml(zxmlNode* pFieldRootNode, bool pCheckHash, ZaiErrors* pErrorlog);

  FieldDesc_Export getFDExp();

  /**
   * @brief _exportAppendFlat This routine does not use ZAExport because of varying elements due to field name in UVF format
   * @return
   */
  ZDataBuffer& _exportAppendFlat(ZDataBuffer& pZDBExport);

  static FieldDesc_Export _exportConvert(ZFieldDescription&pIn,FieldDesc_Export* pOut);
  static ZFieldDescription _importConvert(ZFieldDescription& pOut,FieldDesc_Export* pIn);
};

} // namespace zbs

using namespace  zbs;

Q_DECLARE_METATYPE(zbs::ZFieldDescription );   // required for using such structure as variant
Q_DECLARE_METATYPE(zbs::FieldDesc_Pack);   // required for using such structure as variant
#endif // ZFIELDDESCRIPTION_H
