#ifndef ZSEARCHDESCRIPTOR_H
#define ZSEARCHDESCRIPTOR_H

#include <memory>

#include "zsearchtoken.h"
#include "zmetadic.h"
#include "zmasterfile.h"

#include <zcontentcommon/urfparser.h>

namespace zbs {

using namespace std;
using namespace zbs;


class ZSearchTokenizer;

//class ZSearchMasterFile;


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




class ZSearchMasterFile : public std::enable_shared_from_this <ZSearchMasterFile>
{
public:
  ZSearchMasterFile()=default;
  ZSearchMasterFile(ZSearchMasterFile&)=delete;

  ~ZSearchMasterFile()
  {
    if (_MasterFile.isOpen())
      _MasterFile.zclose();
  }

  ZStatus set(const uriString& pPath);

  std::shared_ptr<ZSearchMasterFile> getSharedPtr() {return shared_from_this();}     /* increase count */
  ZStatus openReadOnly(const uriString& pURI)
  {
    return _MasterFile.zopen(pURI,ZRF_Read_Only);
  }
  ZStatus openModify(const uriString& pURI)
  {
    return _MasterFile.zopen(pURI,ZRF_Modify);
  }

  const ZDictionaryFile& getFieldDictionary() const
  {
    return *_MasterFile.Dictionary;
  }
  const ZDictionaryFile* getFieldDictionaryPtr() const
  {
    return _MasterFile.Dictionary;
  }
  const ZArray<ZKeyDictionary*>& getKeyDictionary() const
  {
    return _MasterFile.Dictionary->KeyDic;
  }
  ZStatus close()
  {
    return _MasterFile.zclose();
  }

  size_t getRecordCount()
  {
    return _MasterFile.getRecordCount();
  }

  ZStatus  openReadOnly()
  {
    return _MasterFile.zopen(ZRF_Read_Only);
  }
  ZStatus openModify()
  {
    return _MasterFile.zopen(ZRF_Modify);
  }

  bool    isOpen()
  {
    return _MasterFile.isOpen();
  }
  bool    isOpenModify()
  {
    return (_MasterFile.getOpenMode() & ZRF_Modify)== ZRF_Modify;
  }
  bool    isOpenAtLeastReadOnly()
  {
    return (_MasterFile.getOpenMode() & ZRF_Read_Only)== ZRF_Read_Only;
  }
  zmode_type getOpenMode(){
    return _MasterFile.getOpenMode() ;
  }

  ZStatus get(ZDataBuffer& pRecord,const long pRank,zaddress_type &pAddress) {
    return _MasterFile.zgetWAddress(pRecord,pRank,pAddress);
  }

  ZStatus getByAddress(ZDataBuffer& pRecord,const zaddress_type pAddress) {
    return _MasterFile.zgetByAddress(pRecord,pAddress);
  }

  uriString getPath() {return _MasterFile.getURIContent();}

  ZMasterFile _MasterFile;
};


class  ZSearchFileZEntity
{
public:
  ZSearchFileZEntity()=default;

  ZSearchFileZEntity(const ZSearchFileZEntity& pIn) {_copyFrom(pIn);}

  ZSearchFileZEntity& _copyFrom(const ZSearchFileZEntity& pIn) {
    Symbol=pIn.Symbol;
    Value=pIn.Value;
    return *this;
  }

  ZSearchFileZEntity& operator=(const ZSearchFileZEntity& pIn) {return _copyFrom(pIn);}

  utf8VaryingString Symbol;
  long              Value;
};



/* a file entity must have an associated master file */
class _BaseFileEntity : public std::enable_shared_from_this <_BaseFileEntity>
{
public:
//  ZSearchEntity() {}
  /* std::shared_ptr<ZSearchMasterFil is passed by value to increase usage count */
  _BaseFileEntity( std::shared_ptr<ZSearchMasterFile> pMasterPtr,ZSearchToken* pToken)
  {
    _MasterFile=pMasterPtr->getSharedPtr();
    setToken(pToken);
  }
  _BaseFileEntity(const _BaseFileEntity& pIn) {_copyFrom(pIn);}

  ~_BaseFileEntity() { }

  _BaseFileEntity& _copyFrom (const _BaseFileEntity& pIn) ;

  _BaseFileEntity& operator = (const _BaseFileEntity& pIn) {return _copyFrom(pIn);}

  std::shared_ptr<_BaseFileEntity> getSharedPtr() {return shared_from_this();}

  static std::shared_ptr<_BaseFileEntity> construct( std::shared_ptr<ZSearchMasterFile> pMasterPtr,ZSearchToken* pToken)
  {
    return std::make_shared<_BaseFileEntity>(pMasterPtr,pToken);
  }

  bool isValid() {return (_MasterFile!=nullptr )&& (!Name.isEmpty()); }

  /**
   * @brief set sets up Descriptor from current tokenizer array position and updates token text and type, row column and offset as well as token number
   */
  void setToken(ZSearchToken *pToken);
  void set(std::shared_ptr<ZSearchMasterFile> pMasterFile, ZSearchToken* pToken) ;
  utf8VaryingString getName( ) const ;

  bool hasFile() {return _MasterFile!=nullptr;}
  long getFieldRankbyName(const utf8VaryingString &pFieldName) const ;

  ZStatus setFile(std::shared_ptr<ZSearchMasterFile> pMasterFile);

  const ZDictionaryFile& getFieldDictionary() const;
  const ZDictionaryFile *getFieldDictionaryPtr() const;
  const ZArray<ZKeyDictionary*>& getKeyDictionary() const;

  ZFieldDescription         getFieldByRank(long pRank) const;

  size_t          getRecordCount();
  ZStatus get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress);

  utf8VaryingString         Name;
  uriString                 Path;
  std::shared_ptr <ZSearchMasterFile> _MasterFile = nullptr;
};



class ZSearchEntity;
//class ZSearchFormula;
//class ZSearchLogicalOperand;
class ZSearchLogicalTerm;

class _BaseCollectionEntity : public std::enable_shared_from_this <_BaseCollectionEntity>
{
public:
  //  ZSearchEntity() {}
  /* std::shared_ptr<ZSearchMasterFil is passed by value to increase usage count */
  _BaseCollectionEntity( std::shared_ptr<ZSearchEntity> pBaseEntity,ZSearchToken* pToken)
  {
    _BaseEntity=pBaseEntity;
    setToken(pToken);
  }
  _BaseCollectionEntity(const _BaseCollectionEntity& pIn) {_copyFrom(pIn);}

  ~_BaseCollectionEntity()
  {
    if (LogicalTerm != nullptr)
      delete LogicalTerm;
  }

  _BaseCollectionEntity& _copyFrom (const _BaseCollectionEntity& pIn) ;

  _BaseCollectionEntity& operator = (const _BaseCollectionEntity& pIn) {return _copyFrom(pIn);}

  bool isValid() {return (_BaseEntity!=nullptr )&& (!Name.isEmpty()); }

  std::shared_ptr<_BaseCollectionEntity> getSharedPtr() {return shared_from_this();}

  static std::shared_ptr<_BaseCollectionEntity> construct( std::shared_ptr<ZSearchEntity> pBaseEntity,ZSearchToken* pToken)
  {
    std::shared_ptr<_BaseCollectionEntity> wOut = std::make_shared<_BaseCollectionEntity>(pBaseEntity,pToken);
    return wOut;
  }


  /**
   * @brief set sets up Descriptor from current tokenizer array position and updates token text and type, row column and offset as well as token number
   */
  void setToken(ZSearchToken *pToken);

  utf8VaryingString getName( ) const ;

//  void setFormula(ZSearchFormula * pFormula);
//  void setLogicalOperand(ZSearchLogicalOperand * pFormula);

  void setLogicalTerm(ZSearchLogicalTerm * pTerm);

  long getFieldRankbyName(const utf8VaryingString &pFieldName) const
  {
    return getFieldDictionary().searchFieldByName(pFieldName);
  }

  const ZDictionaryFile&     getFieldDictionary() const;
  const ZDictionaryFile*     getFieldDictionaryPtr() const;
  const ZArray<ZKeyDictionary*>& getKeyDictionary() const;

  ZFieldDescription   getFieldByRank(long pRank) const
  {
    return getFieldDictionary().TabConst(pRank);
  }

  size_t getMaxRecords();
  size_t getCount() { return AddressList.count(); }

//  ZStatus evaluateFormula(bool &pOutResult, ZSearchFormula *wFormula, const ZDataBuffer &pRecord, int pDepth);

//  ZStatus evaluateFormulaNew(bool &pOutResult, ZSearchFormula* wFormula, const ZDataBuffer& pRecord,int pDepth);

  ZStatus evaluate(bool &pOutResult, const ZDataBuffer &pRecord);
  ZStatus get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress);

  URFParser                 _URFParser;

  utf8VaryingString                   Name;
  std::shared_ptr <ZSearchEntity>     _BaseEntity = nullptr;
  ZArray<zaddress_type>               AddressList;
  ZSearchLogicalTerm*                 LogicalTerm=nullptr;
};

/**
 * @brief The ZSearchEntity class generic entity class that may hold both file entity OR collection entity.
 * Both may transparently be used and accessed as if it refers to a file.
 *
 * A ZSearchEntity has no name, its name is stored within either _BaseFileEntity or _BaseCollectionEntity.
 */
class ZSearchEntity : public std::enable_shared_from_this <ZSearchEntity>
{
public:
  ZSearchEntity() {}

  /* std::shared_ptr<ZSearchMasterFil is passed by value to increase usage count */
  ZSearchEntity( std::shared_ptr<_BaseFileEntity> pFileEntity)
  {
    _FileEntity=pFileEntity;
  }

  ZSearchEntity( std::shared_ptr<ZSearchMasterFile> pMasterPtr,ZSearchToken* pToken)
  {
    _FileEntity = make_shared<_BaseFileEntity>(pMasterPtr,pToken);
    _FileEntity->setToken(pToken);
    allocateCellFormat();
  }


  ZSearchEntity( std::shared_ptr<_BaseCollectionEntity> pCollectionEntity)
  {
    _CollectionEntity=pCollectionEntity->getSharedPtr();
    allocateCellFormat();
  }


  ZSearchEntity(const ZSearchEntity& pIn) {_copyFrom(pIn);}

  ~ZSearchEntity()
  {
    if (CellFormat!=nullptr)
      zfree(CellFormat);
  }

  ZSearchEntity& _copyFrom (const ZSearchEntity& pIn) ;

  ZSearchEntity& operator = (const ZSearchEntity& pIn) {return _copyFrom(pIn);}

  std::shared_ptr<ZSearchEntity> getSharedPtr() {return shared_from_this();}

  std::shared_ptr<_BaseFileEntity> getBaseFileEntity() {
    if (_FileEntity==nullptr)
      return nullptr;
    return make_shared<_BaseFileEntity> (*_FileEntity);
  }


  /* returns a shared point to created entity (file entity) or nullptr in case of error. In this later case, ZException is set with appropriate explainations */

  /* constructs : the master file object using path, the base file entity, then the ZSearchEntity */
  static std::shared_ptr<ZSearchEntity> constructWithFileEntity(const utf8VaryingString& pZMFPath, zmode_type pAccessMode, ZSearchToken* pToken);
  /* uses an already constructed  base file entity to construct the ZSearchEntity */
  static std::shared_ptr<ZSearchEntity> constructWithFileEntity(std::shared_ptr<_BaseFileEntity> pBaseFileEntity,  ZSearchToken* pToken);

  static std::shared_ptr<ZSearchEntity> constructWithMaster(std::shared_ptr<ZSearchMasterFile> pMasterFile,  ZSearchToken* pToken);

  static std::shared_ptr<ZSearchEntity> constructWithCollectionEntity(const utf8VaryingString& pSourceEntityName, ZSearchToken* pToken);
  static std::shared_ptr<ZSearchEntity> constructWithCollectionEntity(std::shared_ptr<ZSearchEntity> pZSearchEntity, ZSearchToken* pToken);


  static void _checkCircularReference(std::shared_ptr<ZSearchEntity> pTop) ;

  bool isValid()
  {
    if (isCollection())
      return _CollectionEntity->isValid();
    if (isFile())
      return _FileEntity->isValid();
    return false;
  }

//  void setFormula(ZSearchFormula* pFormula);
//  void setLogicalOperand(ZSearchLogicalOperand *pOperand);
  void setLogicalTerm(ZSearchLogicalTerm * pTerm);
  /**
   * @brief set sets up Descriptor from current tokenizer array position and updates token text and type, row column and offset as well as token number
   */
  void setToken(ZSearchToken *pToken);
  void set(std::shared_ptr<ZSearchMasterFile> pMasterFile, ZSearchToken* pToken) ;
  utf8VaryingString getName( ) const ;

  long getFieldRankbyName(const utf8VaryingString &pFieldName) const ;

  const ZDictionaryFile& getFieldDictionary() const;
  const ZDictionaryFile* getFieldDictionaryPtr() const;
  const ZArray<ZKeyDictionary*>& getKeyDictionary() const;

  bool isCollection() const {return _CollectionEntity!=nullptr; }
  bool isFile() const       {return _FileEntity!=nullptr; }

  utf8VaryingString _report() ;

  size_t getMaxRecords() {
    if (isFile()) {
      return _FileEntity->getRecordCount();
    }
    if (isCollection())
      return _CollectionEntity->getMaxRecords();
    _DBGPRINT("ZSearchEntity::getMaxRecords Search entity is neither a file entity nor a collection entity.")
    abort;
  }

  ZStatus get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress)
  {
    if (isFile()) {
      return _FileEntity->get(pRecord,pRank,pAddress);
    }
    return _CollectionEntity->get(pRecord,pRank,pAddress);
  }

  ZFieldDescription         getFieldByRank(long pRank) const;

  void allocateCellFormat();
  void reallocateCellFormat();

  int* CellFormat=nullptr;

  std::shared_ptr<_BaseFileEntity>          _FileEntity = nullptr;
  std::shared_ptr<_BaseCollectionEntity>    _CollectionEntity = nullptr;
};

} // namespace zbs

#endif // ZSEARCHDESCRIPTOR_H
