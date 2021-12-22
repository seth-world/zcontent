#ifndef ZFIELDDESCRIPTION_H
#define ZFIELDDESCRIPTION_H

#include <stdlib.h> //for atexit()
#include <sys/types.h>

#include <ztoolset/ztypetype.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/zaierrors.h>

namespace zbs {
#pragma pack(push)
#pragma pack(1)
struct FieldDesc_Export{
  ZTypeBase       ZType;
  uint32_t        ArrayCount; //< if data type is Array then count the rows 0 if not an array
  uint64_t        HeaderSize; //< Size of field header
  uint64_t        UniversalSize;  //< Only if ZType is fixed length. Otherwise set to 0
  uint64_t        NaturalSize;                    //< idem
  uint8_t         KeyEligible=false ;             //< could be set as Key field
  char            Name[cst_fieldnamelen+1+sizeof(uint16_t)];       //< Name of the field aligned to take care of utfdescString export

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


public:
  ZFieldDescription() = default;
  ZFieldDescription (ZFieldDescription& pIn) { _copyFrom(pIn); }
  ZFieldDescription (ZFieldDescription&& pIn) { _copyFrom(pIn); }

  ZFieldDescription& _copyFrom(ZFieldDescription& pIn);

  ZFieldDescription& operator = (ZFieldDescription& pIn) { return _copyFrom(pIn); }
  ZFieldDescription& operator = (ZFieldDescription&& pIn) { return _copyFrom(pIn); }

  ZTypeBase           ZType;
  URF_Capacity_type   Capacity;           //!< if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if an atomic
  uint64_t            HeaderSize;         //!< Size of field header
  uint64_t            UniversalSize;      //!< Only if ZType is fixed length. Otherwise set to 0
  uint64_t            NaturalSize;        //!< Only if ZType is fixed length. Otherwise set to 0
  ZBool               KeyEligible=false ; //!< May be used as Key field (true) or not (false)

private: utf8String          Name;        //!< Name of the field
public:  md5                 Hash;        //!< unique hashcode value for the field.

  void setFieldName(const utf8String& pName);
  utf8String& getName() {return Name;}
  bool hasName(const char* pName) const { return Name==pName; }
  bool hasName(const utf8String& pName) const { return Name==pName.toCChar(); }

  bool isAtomic(void) {return ZType & ZType_Atomic ;}
  bool isArray(void) {return ZType & ZType_Array ;}
  bool isByteSeq(void) {return ZType & ZType_ByteSeq ;}
  bool isBlob(void) {return ZType & ZType_Blob ;}
  bool isErrored(void){return (ZType==ZType_Unknown)||(ZType&ZType_Class);}  // ZType_Class is not allowed
  bool isVarying() {return (ZType&ZType_VaryingLength);}
  bool isSigned(void) {return ZType & ZType_Signed ;}
  bool isEndian(void) {return ZType & ZType_Endian ;}

  void clear() {ZType=0; Capacity=0;HeaderSize=0;UniversalSize=0;NaturalSize=0; KeyEligible=false;Name.clear(); Hash.clear();}

  utf8String toXml(int pLevel, bool pComment=true);
  ZStatus fromXml(zxmlNode* pRootNode, ZaiErrors* pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

  static FieldDesc_Export _exportConvert(ZFieldDescription&pIn,FieldDesc_Export* pOut);
  static ZFieldDescription _importConvert(ZFieldDescription& pOut,FieldDesc_Export* pIn);
};

} // namespace zbs

#endif // ZFIELDDESCRIPTION_H
