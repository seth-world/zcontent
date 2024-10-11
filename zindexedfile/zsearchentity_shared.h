#ifndef ZSEARCHDESCRIPTOR_H
#define ZSEARCHDESCRIPTOR_H
#include <stdint.h>

#include <memory>

#include "zsearchtoken.h"
#include "zmetadic.h"
#include "zmasterfile.h"

#include <zcontentcommon/zcontentconstants.h>

#include "zsearchdictionary.h"

#include <zcontentcommon/urfparser.h>

#include "zsearchfileentity.h"
//#include "zcollectionentity.h"
#include "zsearchdictionary.h"

//#include <zthread/zthread.h>
#include <zthread/zmutex.h>
#include <ztoolset/ztimer.h>

using namespace std;
using namespace zbs;

//#include "zsearchcontext.h"

namespace zbs {

typedef uint32_t  EFST_Type;

enum EntityFetchState : EFST_Type
{
    EFST_Nothing =      0,
    EFST_Partial =      1,
    EFST_Total   =      2,
    EFST_OutDated=      4

};



class ZSearchTokenizer;
//class ZSearchDictionary;
//class ZSearchContext;
//class ZSearchMasterFile;

#ifdef __COMMENT__

/* Cell format code : applies to selected table column */
enum ZCellFormat : int {
  ZCFMT_Nothing = 0,  /* default */

  ZCFMT_NumMask = 0x000F,
  ZCFMT_NumHexa = 0x0001, /* show numeric fields in hexa  (default is standard numeric representation )*/

  ZCFMT_DateMask= 0x0F00,

  ZCFMT_DMY     = 0x0100,   /* day/month/year only */
  ZCFMT_MDY     = 0x0200,   /* month/day/year only */
  ZCFMT_DMYHMS  = 0x0300,   /* day/month/year-hh:mm:ss */
  ZCFMT_MDYHMS  = 0x0400,   /* month/day/year-hh:mm:ss */

  ZCFMT_DLocale = 0x0500,   /* locale date format */
  ZCFMT_DUTC    = 0x0600,   /* UTC format */

  ZCFMT_ResMask = 0x0F0000,

  ZCFMT_ResSymb = 0x010000, /* Show symbol name in place of numeric value for ZEntities */
  ZCFMT_ResStd  = 0x020000, /* Resource numeric values are expressed in standard numeric representation (default is hexa)*/

  ZCFMT_ApplyAll= 0x100000  /* apply to all : only used by cell format dialog */

};
#endif // __CELL_FORMAT__

class ZSearchEntity;
//class ZSearchFormula;
//class ZSearchLogicalOperand;
class ZSearchLogicalTerm;
class ZSearchMasterFile;

class ZSearchJoinTuple {
public:
    ZSearchJoinTuple() = default;
    ZSearchJoinTuple(const ZSearchJoinTuple& pIn) { _copyFrom(pIn);}

    ZSearchJoinTuple(zaddress_type pMAddress,zaddress_type pSAddress)
    {
        MasterAddress = pMAddress;
        SlaveAddress = pSAddress;
    }
    ZSearchJoinTuple& _copyFrom(const ZSearchJoinTuple& pIn)
    {
        MasterAddress=pIn.MasterAddress;
        SlaveAddress=pIn.SlaveAddress;
        return *this;
    }

    ZSearchJoinTuple& operator = (const ZSearchJoinTuple& pIn) { return _copyFrom(pIn);}

    void setInvalid() { MasterAddress=-1 ; SlaveAddress=-1; }

    bool isValid() { return (MasterAddress > -1) && (SlaveAddress > -1) ; }

    zaddress_type MasterAddress=-1;
    zaddress_type SlaveAddress=-1;
};


class ZSearchEntityContext
{
public:
    ZSearchEntityContext() {}

    int         CurrentRank=-1;
    URFParser   _URFParser;
};

/** a ZCollectionEntity is a single collection entit< with
 *  - an array of URFParser objects, one per entity in the entities set
 *  - an associated dictionary
 *      pointing to dictionary of selected entities :
 *          if selected entity is a file entity, dictionary is file dictionary
 *  - a join clause
 *      to link entities between each others
 *      this clause is omitted if only one entity is in the selected entities set
 *      this clause must exist if more than one entity is in the selected entities set
 *  - a selection clause to select records from each entity of the set
 *  - a number of records to display
 */

/**
 * @brief The ZSearchEntity class generic entity class that may hold both file entity OR collection entity.
 * Both may transparently be used and accessed as if it refers to a file.
 *
 * A ZSearchEntity has no name, its name is stored within either _BaseFileEntity or ZCollectionEntity.
 *
 * A ZSearchEntity is a set of one or more base entities with
 *  - an array of URFParser objects, one per base entity in the entities set
 *  - an associated dictionary
 *      pointing to dictionary of selected entities :
 *          if selected entity is a file entity, dictionary is file dictionary
 *  - a join clause
 *      to link any of base entities for entity set between each others
 *      this clause is omitted if only one entity is in the selected base entities set
 *      this clause must exist if more than one entity is in the selected base entities set
 *  - a selection clause to select records from each of base entity within the set
 *  - a number of resulting records to display as first found records
 */

class ZSearchEntity // : public std::enable_shared_from_this <ZSearchEntity>
{
//    friend class _BaseFileEntity;
public:
ZSearchEntity() : _URFParser(&LocalMetaDic) {}

  ZSearchEntity( ZSearchMasterFile* pZSMaster,const utf8VaryingString& pName)
  {
      _FileEntity = pZSMaster ;
      EntityName = EntityFullName = pName;
      /* file entity has no build dic : direct field extraction from ZMasterFile without any change */
      /*             has no local meta dic -> meta dic is the one from master file (via _URFParser) */
      BuildDic.clear();

      for (int wi=0; wi < pZSMaster->getDictionary()->count(); wi++) {
          LocalMetaDic.push(pZSMaster->getDictionary()->TabConst(wi));
      }
      _URFParser.setDictionary(&LocalMetaDic);
      ErrorLog=pZSMaster->ErrorLog;
  }
  /* create a search entity from another entity (file entity or collection) */
  ZSearchEntity( std::shared_ptr<ZSearchEntity> pBaseEntity,ZSearchToken* pToken)
  {
      _BaseEntity = pBaseEntity;
      setNameByToken(pToken);
      ErrorLog=pBaseEntity->_getErrorLog();
      _URFParser.setDictionary(&LocalMetaDic);
  }

public:

  ZSearchEntity(const ZSearchEntity& pIn) {_copyFrom(pIn);}

  ~ZSearchEntity()
  {
      if (_FileEntity!=nullptr) {
        delete _FileEntity;
      }
      if (LogicalTerm!=nullptr) {
          delete LogicalTerm;
      }
  }


  ZSearchEntity& _copyFrom (const ZSearchEntity& pIn) ;

  ZSearchEntity& operator = (const ZSearchEntity& pIn) {return _copyFrom(pIn);}

//  std::shared_ptr<ZSearchEntity> getSharedPtr() {return shared_from_this();}

  ZSearchMasterFile*  getRootFileEntity() {
      if (isFile())
        return  _FileEntity;
    return _BaseEntity->getRootFileEntity();
//      return _CollectionEntityList[0]->_BaseEntity->getBaseFileEntity();
  }


//  static std::shared_ptr<ZSearchEntity> constructWithZSearchEntity(std::shared_ptr<ZSearchEntity> pSearchEntity, ZSearchToken* pToken);


  /* returns a shared point to created entity (file entity) or nullptr in case of error. In this later case, ZException is set with appropriate explainations */

  /* constructs : the shared ZSearchMasterFile object using given path , then the ZSearchEntity
   *  Resulting ZSearchEntity is then a file entity.
   */
/* Deprecated
  static std::shared_ptr<ZSearchEntity> constructWithFilePath(const utf8VaryingString& pZMFPath, zmode_type pAccessMode, ZSearchToken* pToken);

  static std::shared_ptr<ZSearchEntity> constructWithFileEntity(std::shared_ptr<ZMasterFile> pMF,
                                                                ZSearchToken *pToken);
*/
#ifdef __DEPRECATED__
  /* uses an already constructed  base file entity to construct the ZSearchEntity. Entity name is given by pToken.
   * Dictionary is not fed and remains empty. It has to be fed with selected fields from base file dictionary
   */
  static std::shared_ptr<ZSearchEntity> constructWithFileEntity(_BaseFileEntity* pBaseFileEntity,
                                                                ZSearchToken *pToken);

  static std::shared_ptr<ZSearchEntity> constructByEntityName(const utf8VaryingString& pSourceEntityName, ZSearchToken* pToken);


  static std::shared_ptr<ZSearchEntity> constructWithMaster(std::shared_ptr<ZSearchMasterFile> pMasterFile,  ZSearchToken* pToken);
  static std::shared_ptr<ZSearchEntity> constructWithCollection(std::shared_ptr<ZSearchEntity> pZSearchEntity, ZSearchToken* pToken);
#endif // __DEPRECATED__

  static void _checkCircularReference(std::shared_ptr<ZSearchEntity> pTop) ;

  bool isValid()
  {
    if (isFile())
        return _FileEntity!=nullptr;
    return _BaseEntity!=nullptr;

  }

//  void setFormula(ZSearchFormula* pFormula);
//  void setLogicalOperand(ZSearchLogicalOperand *pOperand);
  void setLogicalTerm(ZSearchLogicalTerm * pTerm);
  void set(std::shared_ptr<ZSearchMasterFile> pMasterFile, ZSearchToken* pToken) ;

  long getFieldRankbyName(const utf8VaryingString &pFieldName) const ;
/*
  const ZDictionaryFile& getFieldDictionary() const;
  const ZDictionaryFile* getFieldDictionaryPtr() const;
  const ZArray<ZKeyDictionary*>& getKeyDictionary() const;
*/
  bool isCollection() const { return _BaseEntity != nullptr; }
  bool isFile() const       { return _FileEntity!=nullptr; }
  bool isJoin() const       { return _JoinList.count()>0 ; }

 // bool isUnique() const { return _CollectionEntityList.count() == 1;}

  /**
   * @brief populateAll generates AddressList from base
   */
  ZStatus populateAll();

  /* To be used in a thread */
  static ZStatus populateAll(std::shared_ptr<ZSearchEntity> pEntity);
  /**
   * @brief populateFirst Initialize AddressList from base with at least pCount valid addresses
   */
  ZStatus populateFirst(long pCount);
  /**
   * @brief populateNext Feeds AddressList from base with at least pCount valid additional addresses
   *                   if pCount is equal or less than 0, then AddressList will be complemented until end of available records.
   */
  ZStatus populateNext(long pCount);

  ZStatus populateAllJoin();

  ZStatus populateFirstJoin(int pCount=1);
  ZStatus populateNextJoin(int pCount=1);

  ZStatus getFirstJoin(ZSearchJoinTuple& pAddress);
  ZStatus getNextJoin(ZSearchJoinTuple& pAddress);

  ZStatus _getFirstJoin(ZSearchJoinTuple& pAddress);
  ZStatus _getNextJoin(ZSearchJoinTuple& pAddress);

  ZStatus _getByAddressJoin(ZSearchJoinTuple &pAddress);

  ZStatus _getByRankJoin(long pRank,ZSearchJoinTuple& pAddress);

  /**
   * @brief getFirst Seeks first record from base (either file or entity) that correspond to selection formula
   * When successfull, a valid record formatted according LocalMetaDic is available in _URFParser, with its full list of URFFields.
   */
  ZStatus getFirst(zaddress_type &pAddress);

  /**
   * @brief getNext Seeks from base (either file or entity) for next record that correspond to selection formula
   * When successfull, a valid record formatted according LocalMetaDic is available in _URFParser, with its full list of URFFields.
   */
  ZStatus getNext(zaddress_type &pAddress);

  ZStatus _getFirst(zaddress_type &pAddress);
  ZStatus _getNext(zaddress_type &pAddress);

  ZStatus getByRank(long pRank);

  ZStatus _getByAddress(zaddress_type pAddress);

  /* Gets the whole raw record from the very base file entity (file effective record)
   * and returns it in raw format as pRecord ZDataBuffer as well as its file's address pAddress */
  ZStatus _getRawRecord(const long pRank, zaddress_type &pAddress);

  /**
   * @brief constructURFRecord Builds a record corresponding to a Search Dictionary from an input record
   */
#ifdef __COMMENT__
  static ZStatus constructURFRecord(URFParser &pOutParser,
                                    ZDataBuffer &pOutRecord,
                                    ZSearchDictionary &pBuildDic,
                                    URFParser &pBaseParser,
                                    ZDataBuffer &pBaseRecord,
                                    ZaiErrors *pErrorLog);

  static ZStatus constructURFRecord(ZDataBuffer &pOutRecord,
                                    ZSearchDictionary &pBuildDic,
                                    ZDataBuffer &pBaseRecord,
                                    ZaiErrors *pErrorLog);

  ZStatus constructURFRecord(ZDataBuffer &pOutRecord, ZDataBuffer &pBaseRecord);
#endif // __COMMENT__
//  ZStatus constructURF(URFParser &pInParser);
  /**
 * @brief ZSearchEntity::constructURF constructs URFParser for current entity using either _BaseEntity or _FileEntity URFParser.
 *                                    at the end, URFParser has an appropriate record and an updated URF field list.
 * @return a ZStatus
 *  errored statuses may come from
 *      URFParser::_getURFFieldByRank()
 *          ZS_MISS_FIELD : field presence is set to false
 *          ZS_OUTBOUNDHIGH : requested field rank exceeds fields number
 *      URFParser::parse()
 *          ZS_CORRUPTED URFParser record is malformed : presence bitset is not found or corrupted
 *
 *      ZS_INVTYPE : operand content type is not what is expected in an expression
 *
 */
  ZStatus constructURF();

  ZStatus evaluateRecord(bool &pOutResult);

#ifdef __DEPRECATED_
  ZStatus evaluateBaseRecord(bool &pOutResult);

_

  /* returns the fields content within pFieldList from record at rank pRank (collection rank, not metadic rank)
   *        and its accurate record file address in pAddress after having extracted them from corresponding record
   */
  ZStatus getRecordFieldsValue(const long pRank, zaddress_type &pAddress);

  ZStatus getRecordFields(ZArray<URFField> &pFieldList, const long pRank, zaddress_type &pAddress);
  ZStatus getRecordFields_old(ZArray<URFField>& pFieldList,const long pRank,zaddress_type& pAddress);
#endif // __DEPRECATED__
  ZStatus findFirstFieldValueSequential(long pFieldRank,
                              ZOperandContent *pSearchValue,
                              ZDataBuffer &pOutRecord);

  utf8VaryingString _report() ;
  utf8VaryingString _reportDetail();
  utf8VaryingString _reportJoin() ;

  size_t getCount() {
      if (isFile()) {
          return _FileEntity->getRecordCount();
      }
      if (isCollection())
          return _BaseEntity->getCount();
      _DBGPRINT("ZSearchEntity::getMaxRecords Search entity is neither a file entity nor a collection entity.")
      abort();
  }

  void _enableErrorLogFromBase()
  {
      ErrorLog=_getErrorLog();
  }
  void setErrorLog(ZaiErrors* pErrorLog) {ErrorLog=pErrorLog; }

  ZaiErrors* _getErrorLog() {return ErrorLog; }

  size_t _rootGetCount() {
      if (_FileEntity!=nullptr)
          return getCount();
      return _BaseEntity->_rootGetCount();
  }



  URFParser& getURFParser()
  {
      return _URFParser;
  /*      if (isFile()) {
          fprintf(stderr," Cannot get URFParser from file entity.\\n");
          abort();
      }
      return _CollectionEntity->getURFParser();
  */
  }


  ZStatus _baseGet(const long pRank,zaddress_type& pAddress)
  {
      if (isFile()) {
          return _FileEntity->get(pRank,pAddress);
      }
      return _BaseEntity->_baseGet(pRank,pAddress);
  }

  /* adds all fields from source entity to ZSearchDictionary and ZMetadic (BuildDic and LocalMetaDic) */
  void addAllFields(std::shared_ptr<ZSearchEntity> &pSourceEntity);

  /* clears dictionaries and all fields from source entity to ZSearchDictionary and ZMetadic (BuildDic and LocalMetaDic) */
  void setEqual(std::shared_ptr<ZSearchEntity> &pSourceEntity);


  ZSearchDictionary* getDic() { return &BuildDic; }
  const ZMetaDic* getMetaDic() {
/*      if (isFile()) {
          return _FileEntity->getMetaDic();
      }
      return _CollectionEntity->_BaseEntity->getMetaDic();
*/
      return (const ZMetaDic*)&LocalMetaDic;
  }
  /**
   * @brief set sets up Descriptor from current tokenizer array position and updates token text and type, row column and offset as well as token number
   */
  void setNameByToken(ZSearchToken *pToken);
  void setName(const utf8VaryingString& pName);

  utf8VaryingString getEntityName( ) const;
  utf8VaryingString getEntityFullName( ) const;
  bool hasFormula()
  {
      return (LogicalTerm!=nullptr);
  }
/*  Deprecated
  ZStatus evaluateRecord(bool &pOutResult, const ZDataBuffer &pBaseRecord);
*/


  ZArray<URFField>& getFieldList() {return _URFParser.URFFieldList; }

  void setCaptureTime(bool pOnOff) { CaptureTime=pOnOff; }

//  ZStatus (*DisplayCB) (ZDataBuffer&) = nullptr;

  long                                          CurrentRank=-1;

  utf8VaryingString                             EntityName;
  utf8VaryingString                             EntityFullName;
  /** @brief BuildDic : result of parsing.
   *  This dictionary will be used to extract/compute fields/expressions/literals from base entity records
   *  in order to create  URF formatted records with its own LocalMetaDic meta dictionary.
   *  It has the link to base entity BuildDic.
  */
  ZSearchDictionary                             BuildDic;

  ZMetaDic                                      LocalMetaDic;
  ZSearchMasterFile*                            _FileEntity=nullptr; /* ZSearchMasterFile is unique per File entity and owns all context */

  /* base collection is not shared : only master file access and ZSearchEntity global entities are potentially shared */
  std::shared_ptr<ZSearchEntity>                _BaseEntity=nullptr;
  ZArray<std::shared_ptr<ZSearchEntity>>        _JoinList ;
  ZArray<ZSearchJoinTuple>                      JoinAddressList;
  ZSearchLogicalTerm*                           _Using=nullptr;
  ZArray<zaddress_type>                         AddressList ;
  ZSearchLogicalTerm*                           LogicalTerm=nullptr;  /* entity selection phrase */
  ZaiErrors*                                    ErrorLog=nullptr;  /* entity does not own an errorlog but uses the one from file entity */
  URFParser                                     _URFParser;

  EFST_Type                                     FetchState=EFST_Nothing;

  bool                                          CaptureTime=true;
  ZTimer                                        ProcessTi;
  ZMutex                                        _Mutex;
};



} // namespace zbs

#endif // ZSEARCHDESCRIPTOR_H
