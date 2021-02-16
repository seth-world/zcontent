#ifndef ZFIELDDESCRIPTION_H
#define ZFIELDDESCRIPTION_H

#include <stdlib.h> //for atexit()

#include <zxml/zxml.h>
#include <zxml/zxmlprimitives.h>

namespace zbs {
#pragma pack(push)
#pragma pack(0)
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
  ZFieldDescription& _copyFrom(ZFieldDescription& pIn)
  {
    ZType=pIn.ZType;
    Capacity=pIn.Capacity;
    HeaderSize=pIn.HeaderSize;
    UniversalSize=pIn.UniversalSize;
    NaturalSize=pIn.NaturalSize;
    KeyEligible=pIn.KeyEligible;
    Name=pIn.Name;
  }
public:
  ZFieldDescription() = default;
  ZFieldDescription (ZFieldDescription& pIn) { _copyFrom(pIn); }
  ZFieldDescription (ZFieldDescription&& pIn) { _copyFrom(pIn); }

  ZFieldDescription& operator = (ZFieldDescription& pIn) { _copyFrom(pIn); }
  ZFieldDescription& operator = (ZFieldDescription&& pIn) { _copyFrom(pIn); }

  ZTypeBase           ZType;
  URF_Capacity_type   Capacity;           //!< if Array : number of rows, if Fixed string: capacity expressed in character units, 1 if an atomic
  uint64_t            HeaderSize;         //!< Size of field header
  uint64_t            UniversalSize;      //!< Only if ZType is fixed length. Otherwise set to 0
  uint64_t            NaturalSize;        //!< Only if ZType is fixed length. Otherwise set to 0
  ZBool               KeyEligible=false ; //!< May be used as Key field (true) or not (false)
  utf8String          Name;               //!< Name of the field


  bool isAtomic(void) {return ZType & ZType_Atomic ;}
  bool isArray(void) {return ZType & ZType_Array ;}
  bool isByteSeq(void) {return ZType & ZType_ByteSeq ;}
  bool isBlob(void) {return ZType & ZType_Blob ;}
  bool isErrored(void){return (ZType==ZType_Unknown)||(ZType&ZType_Class);}  // ZType_Class is not allowed
  bool isVarying() {return (ZType&ZType_VaryingLength);}
  bool isSigned(void) {return ZType & ZType_Signed ;}
  bool isEndian(void) {return ZType & ZType_Endian ;}

  void clear() {ZType=0; Capacity=0;HeaderSize=0;UniversalSize=0;NaturalSize=0; KeyEligible=false;Name.clear();}

  utf8String toXml(int pLevel);
  int fromXml(zxmlNode* pRootNode,ZaiErrors* pErrorlog);

  static FieldDesc_Export _exportConvert(ZFieldDescription&pIn,FieldDesc_Export* pOut);
  static ZFieldDescription _importConvert(ZFieldDescription& pOut,FieldDesc_Export* pIn);
};

} // namespace zbs

#endif // ZFIELDDESCRIPTION_H
