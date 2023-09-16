#include "zsearchentity.h"


#include "zsearchtokenizer.h"
#include "zsearchparser.h"

#include "zsearchoperand.h"
#include "zsearchlogicalterm.h"

#include <zcontent/zindexedfile/zmasterfile.h>
//#include "zsearchcollection.h"
//#include "zsearchformula.h"

#include <zcontent/zcontentcommon/urfparser.h>

namespace zbs {

using namespace std;


ZStatus
ZSearchMasterFile::set(const uriString& pPath)
{
  return _MasterFile.setPath(pPath);
}



_BaseFileEntity&
_BaseFileEntity::_copyFrom (const _BaseFileEntity& pIn) {
  Name = pIn.Name;
  Path = pIn.Path;
  _MasterFile = pIn._MasterFile->getSharedPtr();
  return *this;
}

void _BaseFileEntity::setToken(ZSearchToken* pToken)
{
  Name = pToken->Text;
}

void _BaseFileEntity::set(std::shared_ptr<ZSearchMasterFile> pMasterFile, ZSearchToken* pToken)
{
  Name = pToken->Text;
  _MasterFile=pMasterFile;
  Path = pMasterFile->getPath();
}

ZStatus _BaseFileEntity::setFile(std::shared_ptr<ZSearchMasterFile> pMasterFile)
{
  _MasterFile=pMasterFile;
  Path = pMasterFile->getPath();
}

utf8VaryingString
_BaseFileEntity::getName( ) const
{
  return Name;
}

long
_BaseFileEntity::getFieldRankbyName(const utf8VaryingString& pFieldName) const {
  return _MasterFile->getFieldDictionary().searchFieldByName(pFieldName);
}

ZFieldDescription
_BaseFileEntity::getFieldByRank(long pRank) const {
  return _MasterFile->getFieldDictionary().TabConst(pRank);
}

size_t
_BaseFileEntity::getRecordCount()
{
  return _MasterFile->getRecordCount();
}

ZStatus
_BaseFileEntity::get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress)
{
  return _MasterFile->get(pRecord,pRank,pAddress);
}

const ZMetaDic&
_BaseFileEntity::getFieldDictionary() const
{
  return _MasterFile->getFieldDictionary();
}
const ZMetaDic*
_BaseFileEntity::getFieldDictionaryPtr() const
{
  return _MasterFile->getFieldDictionaryPtr();
}
const ZArray<ZKeyDictionary*>&
_BaseFileEntity::getKeyDictionary() const
{
  return _MasterFile->getKeyDictionary();
}

_BaseCollectionEntity& _BaseCollectionEntity::_copyFrom(const _BaseCollectionEntity& pIn)
{
  Name = pIn.Name;
  _BaseEntity = pIn._BaseEntity->getSharedPtr();
  AddressList.clear();
  for (int wi=0;wi < pIn.AddressList.count();wi++)
    AddressList.push (pIn.AddressList[wi]);

  if (LogicalTerm)
    delete LogicalTerm;
  LogicalTerm = nullptr;
  if (pIn.LogicalTerm!=nullptr)
    LogicalTerm = new ZSearchLogicalTerm(*pIn.LogicalTerm);
/*
  if (Formula!=nullptr)
    delete Formula;

  if (pIn.Formula!=nullptr)
    Formula = new ZSearchFormula(*pIn.Formula);
  else
    Formula = nullptr;
*/
  return *this;
}

utf8VaryingString
_BaseCollectionEntity::getName( ) const
{
  return Name;
}

void
_BaseCollectionEntity::setToken(ZSearchToken *pToken )
{
  Name = pToken->Text ;
}


void
_BaseCollectionEntity::setLogicalTerm(ZSearchLogicalTerm* pTerm)
{
  if (LogicalTerm!=nullptr)
    delete LogicalTerm;
  if (pTerm==nullptr)
    LogicalTerm = nullptr;
  else
    LogicalTerm = new ZSearchLogicalTerm(pTerm);
}

const ZMetaDic&
_BaseCollectionEntity::getFieldDictionary() const
{
  return _BaseEntity->getFieldDictionary();
}

const ZMetaDic*
_BaseCollectionEntity::getFieldDictionaryPtr() const
{
  return _BaseEntity->getFieldDictionaryPtr();
}

 const ZArray<ZKeyDictionary*>&
_BaseCollectionEntity::getKeyDictionary() const
{
  return _BaseEntity->getKeyDictionary();
}
size_t
_BaseCollectionEntity::getMaxRecords()
{
  return _BaseEntity->getMaxRecords();
}


ZStatus
_BaseCollectionEntity::get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress)
{
    return _BaseEntity->get(pRecord,pRank,pAddress);
}




ZStatus
_BaseCollectionEntity::evaluate(bool &pOutResult,const ZDataBuffer& pRecord)
{
  _URFParser.set(&pRecord);
  /* if there is no selection clause then all records are selected */
  if (LogicalTerm==nullptr) {
    pOutResult = true;
    return ZS_EMPTY;
  }
  pOutResult = LogicalTerm->evaluate(_URFParser);
  return ZS_SUCCESS;
 // return evaluateFormula(pOutResult,Formula,pRecord,0);
}




ZSearchEntity&
ZSearchEntity::_copyFrom (const ZSearchEntity& pIn) {
  if (CellFormat!=nullptr)
    zfree(CellFormat);
  int wFieldNumber=0;
  if (isFile()) {
    _FileEntity = pIn._FileEntity->getSharedPtr();
    wFieldNumber = pIn._FileEntity->getFieldDictionary().count();
  }
  if (isCollection()) {
    _CollectionEntity = pIn._CollectionEntity->getSharedPtr();
    wFieldNumber = pIn._CollectionEntity->getFieldDictionary().count();
  }
  if (CellFormat!=nullptr)
    zfree(CellFormat);

  CellFormat = (int*)malloc((wFieldNumber+1)*sizeof(int));
  for (int wi=0;wi < wFieldNumber+1;wi++) {
    CellFormat[wi]=pIn.CellFormat[wi];
  }

  return *this;
}

utf8VaryingString
ZSearchEntity::getName () const {
  if (isFile())
    return _FileEntity->getName();
  if (isCollection())
    return _CollectionEntity->getName();
  return utf8VaryingString();
}

void
ZSearchEntity::setToken(ZSearchToken *pToken)
{
  if (isCollection())
  {
    _CollectionEntity->setToken(pToken);
    return;
  }

  _FileEntity->setToken(pToken);
}

void
ZSearchEntity::setLogicalTerm(ZSearchLogicalTerm*pTerm)
{
  if (!isCollection()) {
    _DBGPRINT("ZSearchEntity::setTerm Entity <%s> is not a collection while trying to set a logical term.",getName().toString())
    abort();
  }

  _CollectionEntity->setLogicalTerm(pTerm);
  return;
}
const char*
decode_ZSearchEntityType(std::shared_ptr <ZSearchEntity> pZSE) {
  if (pZSE->isFile())
    return "File entity";
  else if (pZSE->isCollection())
    return "Collection";
  return "Unknown entity type (neither collection nor file entity)";
}

utf8VaryingString
ZSearchEntity::_report() {
  utf8VaryingString wReturn;
  const char* wType = "Unknown entity type (neither collection nor file entity)";
  if (isFile())
    wType = "File entity";
  else if (isCollection())
    wType = "Collection";
  wReturn.sprintf(" Name <%s> %s\n",
      getName().toString(), wType);
  if (isFile()) {
    wReturn.addsprintf("%s\n",_FileEntity->Path.toString());
  }
  if (isCollection()){
/*    if (_CollectionEntity->Formula!=nullptr)
      wReturn +=_CollectionEntity->Formula->_report();

    if (_CollectionEntity->LogicalOperand!=nullptr)
      wReturn +=_CollectionEntity->LogicalOperand->_report(0);
    else
      wReturn += "***No logical operation***\n";
*/

    if (_CollectionEntity->LogicalTerm!=nullptr) {
      wReturn += "______________________selection clause______________________\n";
      wReturn +=_CollectionEntity->LogicalTerm->_reportFormula();
      wReturn += "\n";
    }
    else
      wReturn += "***No selection clause***\n";
    wReturn.addsprintf("__________________Base entity___________________\n"
                       "Name <%s> Type <%s>\n",
        _CollectionEntity->_BaseEntity->getName().toString(),
        decode_ZSearchEntityType(_CollectionEntity->_BaseEntity));
    wReturn.addsprintf("Addresses count <%ld>\n",_CollectionEntity->AddressList.count());
    wReturn += "________________________________________________";
  }

  return wReturn;
}


std::shared_ptr<ZSearchEntity>
ZSearchEntity::constructWithFileEntity(const utf8VaryingString& pZMFPath,zmode_type pAccessMode,ZSearchToken* pToken)
{
  ZStatus wSt=ZS_SUCCESS;
  std::shared_ptr<ZSearchEntity> wEntityPtr=nullptr;

  if (GParser->EntityList.getEntityByName(pToken->Text)!=nullptr) {
    GParser->textLog("An entity with name <%s> has already been registrated previously.",
        pToken->Text.toString());

    return nullptr;
  }

  std::shared_ptr <ZSearchMasterFile> wMF=nullptr;
  int wi=0;
  for (;wi < GParser->MasterFileList.count();wi++){
    if (pZMFPath==GParser->MasterFileList[wi]->_MasterFile.getURIContent()) {
      wMF=GParser->MasterFileList[wi]->getSharedPtr();
      if (pAccessMode == ZRF_Modify) {
        if (wMF->getOpenMode()!=ZRF_Modify) {
          GParser->warningLog("File <%s> is already is use with access mode <%s> cannot use mode <ZRF_Modify>.",
              pZMFPath.toString(), decode_ZRFMode(wMF->getOpenMode()));
          return nullptr;
        }
        break;
      }// ZRF_Modify
      /* if not open for modification then it is open for read only */
      if ((wMF->getOpenMode()&ZRF_Read_Only)!=ZRF_Read_Only) { /* check file is open at least for read only */
        GParser->warningLog("File <%s> is already is use with access mode <%s> cannot use mode <ZRF_Modify>.",
            pZMFPath.toString(), decode_ZRFMode(wMF->getOpenMode()));
        return nullptr;
      }
      break;
    } // if (pZMFPath==GParser->MasterFileList[wi]->MasterFile.getURIContent())
    } // for

  if (wi == GParser->MasterFileList.count())  {  /* not found in master file list : create one in list */
    wMF=std::make_shared <ZSearchMasterFile>();
    if (pAccessMode == ZRF_Modify) {
      wSt=wMF->openModify(pZMFPath);
    }
    else {
      wSt=wMF->openReadOnly(pZMFPath);
    }
    if (wSt!=ZS_SUCCESS) {
      GParser->logZStatus(ZAIES_Error,wSt,"ZSearchParser::_parse-E-ERROPEN Cannot access file <%s> mode <%s>.",
          pZMFPath.toString(),decode_ZRFMode( pAccessMode));
      return nullptr;
    }
    GParser->MasterFileList.push(wMF);
  }// not found

  /* up to there we've got the file, let's construct _BaseFileEntity */

  wEntityPtr = std::make_shared<ZSearchEntity>(wMF,pToken);

  return wEntityPtr;
}
std::shared_ptr<ZSearchEntity>
ZSearchEntity::constructWithFileEntity(std::shared_ptr<_BaseFileEntity> pBaseFileEntity,  ZSearchToken* pToken)
{
  return std::make_shared<ZSearchEntity> (pBaseFileEntity);
}

std::shared_ptr<ZSearchEntity>
ZSearchEntity::constructWithMaster(std::shared_ptr<ZSearchMasterFile> pMasterFile,  ZSearchToken* pToken)
{
  std::shared_ptr<_BaseFileEntity> wBaseFileEntity=_BaseFileEntity::construct(pMasterFile,pToken);

  return std::make_shared<ZSearchEntity> (wBaseFileEntity);
}

std::shared_ptr<ZSearchEntity>
ZSearchEntity::constructWithCollectionEntity(std::shared_ptr<ZSearchEntity> pZSearchEntity,ZSearchToken* pToken)
{
  return std::make_shared<ZSearchEntity> (_BaseCollectionEntity::construct(pZSearchEntity,pToken));
}

std::shared_ptr<ZSearchEntity>
ZSearchEntity::constructWithCollectionEntity(const utf8VaryingString& pSourceEntityName,ZSearchToken* pToken)
{
  ZStatus wSt=ZS_SUCCESS;
  std::shared_ptr<ZSearchEntity> wOutEntityPtr=nullptr;

  if (GParser->EntityList.getEntityByName(pToken->Text)!=nullptr) {
    GParser->textLog("An entity with name <%s> has already been registrated previously.",
        pToken->Text.toString());

    return nullptr;
  }


  std::shared_ptr <ZSearchEntity> wSE=nullptr;
  int wi=0;
  for (;wi < GParser->EntityList.count();wi++){
    if (pSourceEntityName==GParser->EntityList[wi]->getName()) {
      wSE=GParser->EntityList[wi]->getSharedPtr();
      break;
    } // if (pZMFPath==GParser->MasterFileList[wi]->MasterFile.getURIContent())
    } // for

  if (wi == GParser->EntityList.count())  {  /* not found in master file list : create one in list */
    GParser->logZStatus(ZAIES_Error,wSt,"ZSearchParser::constructCollectionEntity Cannot find entity named <%s>.",
        pSourceEntityName.toString());
    return nullptr;
  }

  return std::make_shared<ZSearchEntity> (_BaseCollectionEntity::construct(wSE,pToken));
}

void ZSearchEntity::_checkCircularReference(std::shared_ptr<ZSearchEntity> pTop) {
  std::shared_ptr<ZSearchEntity> wCurEntity=pTop;
  ZArray <utf8VaryingString> wNames;
  while (!wCurEntity->isFile()) {
    if (!wCurEntity->isCollection()) {
      _DBGPRINT("ZSearchEntity::_checkCircularReference Empty Search entity is neither file nor collection.")
      abort();
    }
    for (int wi=0; wi < wNames.count();wi++)
      if (wNames[wi]==wCurEntity->getName()) {
        _DBGPRINT("ZSearchEntity::_checkCircularReference Circular reference detected. Search entity <%s> is declared as circular at level <%d>.",
            wCurEntity->getName().toString(),
            wi)
        abort();
      }
    wNames.push(wCurEntity->getName());
    wCurEntity = wCurEntity->_CollectionEntity->_BaseEntity;
  } // while true
}

void
ZSearchEntity::allocateCellFormat()
{
  if (CellFormat!=nullptr) {
    zfree(CellFormat);
  }
  int wNb=getFieldDictionary().count();
  CellFormat = (int*)calloc(wNb,sizeof(int));
  memset (CellFormat,0,wNb*sizeof(int));
}
void
ZSearchEntity::reallocateCellFormat()
{
  CellFormat = (int*)realloc(CellFormat,getFieldDictionary().count()*sizeof(int));
}

const ZMetaDic&
ZSearchEntity::getFieldDictionary() const
{
    if (isFile())
      return _FileEntity->getFieldDictionary();
    if (isCollection())
      return _CollectionEntity->getFieldDictionary();

}


const ZMetaDic*
ZSearchEntity::getFieldDictionaryPtr() const
{
  if (isFile())
    return _FileEntity->getFieldDictionaryPtr();
  if (isCollection())
    return _CollectionEntity->getFieldDictionaryPtr();

}


long
ZSearchEntity::getFieldRankbyName(const utf8VaryingString &pFieldName) const
{
  if (isFile())
    return _FileEntity->getFieldRankbyName(pFieldName);
  if (isCollection())
    return _CollectionEntity->getFieldRankbyName(pFieldName);

}


ZFieldDescription
ZSearchEntity::getFieldByRank(long pRank) const
{
  if (isFile())
    return _FileEntity->getFieldByRank(pRank);
  if (isCollection())
    return _CollectionEntity->getFieldByRank(pRank);

}




const ZArray<ZKeyDictionary*>&
ZSearchEntity::getKeyDictionary() const
{
  if (isFile())
    return _FileEntity->getKeyDictionary();
  if (isCollection())
    return _CollectionEntity->getKeyDictionary();
}


}//namespace zbs
