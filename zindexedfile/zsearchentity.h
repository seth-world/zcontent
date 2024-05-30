#ifndef ZSEARCHENTITY_H
#define ZSEARCHENTITY_H
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
#include "zsearchentitycontext.h"

#include "zsearchjoinaddress.h"

//#include <zthread/zthread.h>
#include <zthread/zmutex.h>
#include <ztoolset/ztimer.h>



using namespace std;
using namespace zbs;

//#include "zsearchcontext.h"

/*

 Once accessing entity, the objective is NOT to get a record, but to get an updated, accurate URFParser corresponding to entity and record.

Join entity

    do loop until <selection clause is true> or <EOF for master entity>

        get <master base entity> ( <Base context URF parser> owns record and fields )
        What is done in get operation :
            get raw record from <master base entity>
            parse raw record using <Base context URF parser>

        do loop until EOF for slave entity or <USING clause is true>

            get <slave base entity>  ( <slave context URF parser> owns record and fields )
            What is done in get operation :
                get raw record from <slave base entity>
                parse raw record using <Slave context URF parser>

            evaluate USING clause using fields from <base URF parser> corresponding to <base entity> (either master or slave)

        loop until evaluate <USING is true> or EOF for slave entity

        evaluate selection clause using fields from <base URF parser> corresponding to <base entity> (either master or slave)

    loop until <selection clause is true> or <EOF for master entity>

    compose upper record in <upper entity context URF Parser> using <base URF Parser> corresponding to <base entity> (either master or slave)

    parse record using <upper entity context URF Parser>

    done one get for join entity



Single collection

    do loop until <selection clause is true> or <EOF for base entity>

        get base entity ( <Base context URF parser> owns record and fields )
        What is done in get operation :
            get raw record from <base entity>
            parse raw record using <Base context URF parser>


        evaluate selection clause using fields from <base URF parser>

    loop until <selection clause is true> or <EOF for master entity>

    compose upper record in <upper entity context URF Parser> using <base URF Parser>

    parse record using <upper entity context URF Parser>

    done one get for join entity


*/



namespace zbs {

class ZSearchTokenizer;

class ZSearchEntity;
//class ZSearchFormula;
//class ZSearchLogicalOperand;
class ZSearchLogicalTerm;
class ZSearchMasterFile;
/*
class ZSearchJoinAddress {
public:
    ZSearchJoinAddress() = default;
    ZSearchJoinAddress(const ZSearchJoinAddress& pIn) { _copyFrom(pIn);}

    ZSearchJoinAddress(zaddress_type pMAddress,zaddress_type pSAddress)
    {
        MasterAddress = pMAddress;
        SlaveAddress = pSAddress;
    }
    ZSearchJoinAddress& _copyFrom(const ZSearchJoinAddress& pIn)
    {
        MasterAddress=pIn.MasterAddress;
        SlaveAddress=pIn.SlaveAddress;
        return *this;
    }

    ZSearchJoinAddress& operator = (const ZSearchJoinAddress& pIn) { return _copyFrom(pIn);}

    void setInvalid() { MasterAddress=-1 ; SlaveAddress=-1; }

    bool isValid() { return (MasterAddress > -1) && (SlaveAddress > -1) ; }

    zaddress_type MasterAddress=-1;
    zaddress_type SlaveAddress=-1;
};
*/

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
ZSearchEntity()  {}


  ZSearchEntity(  ZSearchMasterFile* pZSMaster,const utf8VaryingString& pName)
  {
      _FileEntity = pZSMaster ;
      EntityName = EntityFullName = pName;
      /* file entity has no build dic : direct field extraction from ZMasterFile without any change */
      /*             has no local meta dic -> meta dic is the one from master file (via _URFParser) */
      BuildDic.clear();

      for (int wi=0; wi < pZSMaster->getDictionary()->count(); wi++) {
          LocalMetaDic.push(pZSMaster->getDictionary()->TabConst(wi));
      }
      ErrorLog=pZSMaster->_getErrorLog();
  }
  /* create a search entity from another entity (file entity or collection) */
  ZSearchEntity( std::shared_ptr<ZSearchEntity> pBaseEntity,ZSearchToken* pToken)
  {
      _BaseEntity = pBaseEntity;
      setNameByToken(pToken);
      ErrorLog=pBaseEntity->_getErrorLog();
//      _URFParser.setDictionary(&LocalMetaDic);
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

// moved to ZSearchEntityContext
//  static ZSearchEntityContext newEntityContext(std::shared_ptr<ZSearchEntity> pEntity) ;


//  std::shared_ptr<ZSearchEntity> getSharedPtr() {return shared_from_this();}

  ZSearchMasterFile*  getRootFileEntity() {
      if (isFile())
        return  _FileEntity;
    return _BaseEntity->getRootFileEntity();
//      return _CollectionEntityList[0]->_BaseEntity->getBaseFileEntity();
  }


  static void _checkCircularReference(std::shared_ptr<ZSearchEntity> pTop) ;

  bool isValid()
  {
    if (isFile())
        return _FileEntity!=nullptr;
    return _BaseEntity!=nullptr;

  }


  void setLogicalTerm(ZSearchLogicalTerm * pTerm);
  void set(std::shared_ptr<ZSearchMasterFile> pMasterFile, ZSearchToken* pToken) ;

  long getFieldRankbyName(const utf8VaryingString &pFieldName) const ;

  bool isCollection() const { return _BaseEntity != nullptr; }
  bool isFile() const       { return _FileEntity!=nullptr; }
  bool isJoin() const       { return _JoinList.count()>0 ; }

 // bool isUnique() const { return _CollectionEntityList.count() == 1;}



  ZStatus populateAllJoin(ZSearchEntityContext &pSEC);

  ZStatus populateFirstJoin(ZSearchEntityContext &pSEC,int pCount=1);
  ZStatus _populateFirstJoin(ZSearchEntityContext &pSEC,int pCount=1);
  ZStatus populateNextJoin(ZSearchEntityContext &pSEC,int pCount=1);

  ZStatus getFirstJoin(ZSearchEntityContext &pSEC,ZSearchJoinAddress& pAddress);
  ZStatus getNextJoin(ZSearchEntityContext &pSEC,ZSearchJoinAddress& pAddress);

  ZStatus _getFirstJoin(ZSearchEntityContext &pSEC,ZSearchJoinAddress& pAddress);
  ZStatus _getNextJoin(ZSearchEntityContext &pSEC,ZSearchJoinAddress& pAddress);

  ZStatus _getByAddressJoin(ZSearchEntityContext &pSEC,ZSearchJoinAddress &pAddress);

  ZStatus getByRankJoin(ZSearchEntityContext &pSEC,long pRank,ZSearchJoinAddress& pAddress);
  ZStatus _getByRankJoin(ZSearchEntityContext &pSEC,long pRank,ZSearchJoinAddress& pAddress);


  /* Populate operations : refers to FETCH */

  /**
   * @brief populateAll generates AddressList from base
   */
  ZStatus populateAll(ZSearchEntityContext& pSEC);

  /**
   * @brief populateFirst Initialize AddressList from base with at least pCount valid addresses
   */
  ZStatus populateFirst(ZSearchEntityContext &pSEC,long pCount=1);
  ZStatus _populateFirst(ZSearchEntityContext &pSEC,long pCount=1);
  /**
   * @brief populateNext Feeds AddressList from base with at least pCount valid additional addresses
   *                   if pCount is equal or less than 0, then AddressList will be complemented until end of available records.
   */
  ZStatus populateNext(ZSearchEntityContext &pSEC,long pCount=1);

  ZStatus _populateNext(ZSearchEntityContext &pSEC,long pCount);

  /** get operations
   *
   * NB: Current rank within entity and within context are updated ONLY if get operation is TOTALLY successfull */

  /**
   * @brief getFirst Seeks first record from base (either file or entity) that correspond to selection formula
   * When successfull, a valid record formatted according LocalMetaDic is available in _URFParser, with its full list of URFFields.
   *
   */
  ZStatus getFirst(ZSearchEntityContext &pSEC,zaddress_type &pAddress);

  /**
   * @brief getNext Seeks from base (either file or entity) for next record that correspond to selection formula
   * Next record has entity rank = ZSearchEntityContext::CurrentRank + 1
   * When successfull,
   * ZSearchEntityContext::_URFParser contains a valid record formatted according LocalMetaDic is available in
   *                                            with its associated full list of URFFields.
   * ZSearchEntityContext::CurrentRank is updated with current position within entity
   */
  ZStatus getNext(ZSearchEntityContext &pSEC, zaddress_type &pAddress);

  ZStatus _getFirst(ZSearchEntityContext &pSEC,zaddress_type &pAddress);
  /** @brief _getNext
   * find next URF content from entity according current entity rank :
   *                            Entity AddressList is first searched for appropriate (Gives the first next selected rank of base entity)
   */
  ZStatus _getNext(ZSearchEntityContext &pSEC, zaddress_type &pAddress);
  /* effective access to entity first record (selected record) : ZSearchEntity::AddressList is not used even if available */
  ZStatus _getFirstHard(ZSearchEntityContext &pSEC,zaddress_type &pAddress);

  /* effective access to next record : ZSearchEntity::AddressList is not used even if available
   *                                    next rank is obtained from current entity and sub-entities state */
  ZStatus _getNextHard(ZSearchEntityContext &pSEC,zaddress_type &pAddress);

  ZStatus getByRank(ZSearchEntityContext &pSEC,long pRank,zaddress_type &pAddress);
  ZStatus _getByRank(ZSearchEntityContext &pSEC,long pRank,zaddress_type &pAddress);

  ZStatus _getByAddressRaw(ZSearchEntityContext &pSEC, zaddress_type pAddress);

  /* Gets the whole raw record from the very base file entity (file effective record)
   * and returns it in raw format as pRecord ZDataBuffer as well as its file's address pAddress */
  ZStatus _getRawRecord(ZSearchEntityContext &pSEC, const long pRank, zaddress_type &pAddress);

  /**
   * @brief constructURFRecord Builds a record corresponding to a Search Dictionary from an input record
   */

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
  ZStatus constructURF(ZSearchEntityContext &pSEC);
#ifdef __COMMENT__
  URFParser* getParserFromField(ZSearchEntityContext& pSEC,ZSearchField& pField );
  URFParser* getParserFromEntity(ZSearchEntityContext& pSEC,std::shared_ptr<ZSearchEntity> pEntity );
#endif // __COMMENT__

  ZStatus evaluateRecord(ZSearchEntityContext &pSEC, bool &pOutResult);

  ZStatus findFirstFieldValueSequential(ZSearchEntityContext &pSEC,
                                        long pFieldRank,
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
      if (isJoin())
          return _JoinList[0]->getCount();
      _DBGPRINT("ZSearchEntity::getMaxRecords Search entity is neither a file entity nor a collection entity nor a join entity.\n")
      abort();
  }

  void _enableErrorLogFromBase()
  {
      ErrorLog=_getErrorLog();
  }
  void setErrorLog(ZaiErrors* pErrorLog) {ErrorLog=pErrorLog; }

  ZaiErrors* _getErrorLog()
  {
      if (ErrorLog==nullptr) {
          if (_FileEntity!=nullptr) {
              return ErrorLog = _FileEntity->_getErrorLog();
          }
          return ErrorLog = _BaseEntity->_getErrorLog();
      }
      return ErrorLog;
  }

  size_t _rootGetCount() {
      if (_FileEntity!=nullptr)
          return getCount();
      return _BaseEntity->_rootGetCount();
  }


/*
  URFParser& getURFParser()
  {
      return _URFParser;
  }
*/

  ZStatus _baseGet(ZSearchEntityContext& pSEC, const long pRank,zaddress_type& pAddress)
  {
      if (isFile()) {
          return _FileEntity->getByRank(pSEC,pRank,pAddress);
      }
      return _BaseEntity->_baseGet(pSEC,pRank,pAddress);
  }

  /* adds all fields from source entity to ZSearchDictionary and ZMetadic (BuildDic and LocalMetaDic) */
  void addAllFields(std::shared_ptr<ZSearchEntity> &pSourceEntity);

  /* clears dictionaries and all fields from source entity to ZSearchDictionary and ZMetadic (BuildDic and LocalMetaDic) */
  void setEqual(std::shared_ptr<ZSearchEntity> &pSourceEntity);

  ZSearchDictionary* getDic() { return &BuildDic; }
  const ZMetaDic* getMetaDic() {
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

//  void setCaptureTime(bool pOnOff) { CaptureTime=pOnOff; }
  long                                          CurrentRank=-1;
  zaddress_type                                 LastAddress=-1;
  ZSearchJoinAddress                            LastAddressJoin= ZSearchJoinAddress(-1,-1);

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
  ZArray<ZSearchJoinAddress>                      JoinAddressList;
  ZSearchLogicalTerm*                           _Using=nullptr;
  ZArray<zaddress_type>                         AddressList ;
  ZSearchLogicalTerm*                           LogicalTerm=nullptr;  /* entity selection phrase */
  ZaiErrors*                                    ErrorLog=nullptr;  /* entity does not own an errorlog but uses the one from file entity */
//  URFParser                                     _URFParser;

  EFST_Type                                     FetchState=EFST_Nothing;

//  bool                                          CaptureTime=true;
//  ZTimer                                        ProcessTi;
  ZMutex                                        _Mutex;
};

} // namespace zbs

#endif // ZSEARCHENTITY_H
