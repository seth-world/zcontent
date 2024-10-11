#include "zsearchentity.h"
//#include "zsearchdictionary.h"

#include "zsearchtokenizer.h"
#include "zsearchparser.h"

#include "zsearchoperand.h"
#include "zsearchlogicalterm.h"

#include "zdataconversion.h"

//#include "zmasterfile.h"

//#include "zsearchcollection.h"
//#include "zsearchformula.h"

#include <zcontent/zcontentcommon/urfparser.h>

#include "zsearchfileentity.h"
//#include "zcollectionentity.h"

namespace zbs {


ZSearchEntity&
ZSearchEntity::_copyFrom (const ZSearchEntity& pIn) {
  int wFieldNumber = 0 ;

  EntityName = pIn.EntityName;

  if (_FileEntity!=nullptr) {
//      _FileEntity->reset();   /* deletes local copy of shared pointer and decrement count */
    delete _FileEntity;
    _FileEntity=nullptr;
  }
  if (_BaseEntity!=nullptr)
      _BaseEntity.reset(); /* deletes local copy of shared pointer and decrement count */
//      _CollectionEntity.reset();   /* No base collection is not shared Only entities and master files are shared */

  if (pIn.isFile()) {
    _FileEntity = pIn._FileEntity ;
  }
    if (isCollection()) {
      _BaseEntity = pIn._BaseEntity;
  }
  for (int wi=0; wi < pIn.BuildDic.count();wi++)
      BuildDic.push(pIn.BuildDic[wi]);

  for (int wi=0; wi < pIn.LocalMetaDic.count();wi++)
      LocalMetaDic.push(pIn.LocalMetaDic[wi]) ;

  _URFParser.setDictionary(&LocalMetaDic);

  return *this;
}


void
ZSearchEntity::setNameByToken(ZSearchToken *pToken)
{
  EntityName = pToken->Text ;
  if (isFile()) {
      EntityFullName = EntityName + "::File";
      return;
  }
  if (isCollection()) {
      EntityFullName.sprintf("%s::%s",
                             EntityName.toString(),
                             _BaseEntity->EntityFullName.toString());
      return;
  }
  /* this is a join */
  EntityFullName = EntityName + "::Join";
  return;
}
void
ZSearchEntity::setName(const utf8VaryingString& pName)
{
    EntityName = pName;
    if (isFile()) {
        EntityFullName = EntityName + "::File";
        return;
    }
    if (isCollection()) {
        EntityFullName.sprintf("%s::%s",
                               EntityName.toString(),
                               _BaseEntity->EntityFullName.toString());
        return;
    }
    /* this is a join */
    EntityFullName = EntityName + "::Join";
    return;
}

utf8VaryingString
ZSearchEntity::getEntityName( ) const
{
    return EntityName;
}

utf8VaryingString
ZSearchEntity::getEntityFullName( ) const
{
    return EntityFullName;
}

void
ZSearchEntity::setLogicalTerm(ZSearchLogicalTerm*pTerm)
{
  if (LogicalTerm!=nullptr)
      delete LogicalTerm;
  if (pTerm==nullptr)
      LogicalTerm = nullptr;
  else
      LogicalTerm = new ZSearchLogicalTerm(pTerm);
  return;
}

const char*
decode_ZSearchEntityType(std::shared_ptr <ZSearchEntity> pZSE) {
  if (pZSE->isFile())
    return "File entity";
  else if (pZSE->isCollection())
    return "Collection";
  else if (pZSE->isJoin())
      return "Join";
  return "Unknown entity type (neither collection nor file entity)";
}



utf8VaryingString
ZSearchEntity::_report() {

  if (isJoin()) {
    return  _reportJoin() ;
  }
  utf8VaryingString wReturn;
  const char* wType = "Unknown entity type (neither collection nor file entity)";
  if (isFile())
    wType = "File entity";
  else if (isCollection())
    wType = "Collection";
  else if (isJoin())
      wType = "Join";
  wReturn.sprintf(" Name <%s> Full name <%s> Type %s\n",
                  getEntityName().toString(), getEntityFullName().toString(),wType);
  if (isFile()) {
      wReturn.addsprintf("Path <%s>\n",_FileEntity->getPath().toString());
  }

  wReturn += _reportDetail();

  return wReturn;
}

utf8VaryingString
ZSearchEntity::_reportDetail() {
    utf8VaryingString wReturn;
    if (LogicalTerm!=nullptr) {
        wReturn += "______________________selection clause______________________\n";
        wReturn +=LogicalTerm->_report(0);
        wReturn += "\n";
    }
    else
        wReturn += "***No selection clause***\n";
    if (_BaseEntity!=nullptr){
        wReturn.addsprintf("__________________Base entity___________________\n"
                           "Name <%s> Type <%s>\n",
                           _BaseEntity->getEntityName().toString(),
                           decode_ZSearchEntityType(_BaseEntity));
        wReturn.addsprintf("Addresses count <%ld>\n",_BaseEntity->AddressList.count());
        wReturn += "________________________________________________";
    }

    return wReturn;
}

utf8VaryingString
ZSearchEntity::_reportJoin() {
    if (!isJoin())
        return "invalid call to _reportJoin()";

    utf8VaryingString wReturn;

    wReturn.sprintf(" Name <%s> Full name <%s> Type Join\n",
                    getEntityName().toString(), getEntityFullName().toString());


    wReturn.addsprintf("______________________Main entity <%s> full name <%s> Type <%s>______________________\n",
                       _JoinList[0]->getEntityName(),_JoinList[0]->getEntityFullName(),_JoinList[0]->getEntityName(),decode_ZSearchEntityType(_JoinList[0]));


    wReturn += _JoinList[0]->_reportDetail();


    wReturn.addsprintf("______________________Slave entity <%s> full name <%s> Type <%s>______________________\n",
                       _JoinList[1]->getEntityName(),_JoinList[1]->getEntityFullName(),_JoinList[1]->getEntityName(),decode_ZSearchEntityType(_JoinList[1]));

    wReturn += _JoinList[1]->_reportDetail();


    if (_Using!=nullptr) {
        wReturn += "______________________Using clause______________________\n";
        wReturn +=_Using->_reportFormula();
        wReturn += "\n";
    }
    else
        wReturn += " WARNING : No <USING> clause.\n";

    wReturn += "________________________________________________\n";
    return wReturn;
}
#ifdef __DEPRECATED__
std::shared_ptr<ZSearchEntity>
ZSearchEntity::constructWithFilePath(const utf8VaryingString& pZMFPath,zmode_type pAccessMode,ZSearchToken* pToken)
{
  ZStatus wSt=ZS_SUCCESS;
//  std::shared_ptr<ZSearchEntity> wEntityPtr=nullptr;

  if (GParser->EntityList.getEntityByName(pToken->Text)!=nullptr) {
      GParser->ErrorLog.textLog("An entity with name <%s> has already been registrated previously.",
        pToken->Text.toString());

    return nullptr;
  }

//  std::shared_ptr <ZSearchMasterFile> wMF=nullptr;
  std::shared_ptr<ZMasterFile> wZMF=nullptr;
  int wi=0;
  for (;wi < GParser->MasterFileList.count();wi++){
    if (pZMFPath==GParser->MasterFileList[wi]->getURIContent()) {
          /* construct ZSearchMasterFile with shared pointer to master file */
//        wMF = new ZSearchMasterFile(GParser->MasterFileList[wi],GParser->MasterFileList[wi].Name);
        wZMF = GParser->MasterFileList[wi];
//      wMF=GParser->MasterFileList[wi]->getSharedPtr();
        if (pAccessMode == ZRF_Modify) {
            if (wZMF->getOpenMode()!=ZRF_Modify) {
                GParser->ErrorLog.warningLog("File <%s> is already is use with access mode <%s> cannot use mode <ZRF_Modify>.",
                pZMFPath.toString(), decode_ZRFMode(wZMF->getOpenMode()));
                return nullptr;
            }
            break;
        }// ZRF_Modify
      /* if not open for modification then it is open for read only */
      if ((wZMF->getOpenMode()&ZRF_Read_Only)!=ZRF_Read_Only) { /* check file is open at least for read only */
        GParser->ErrorLog.warningLog("File <%s> is already is use with access mode <%s> cannot use mode <ZRF_Modify>.",
            pZMFPath.toString(), decode_ZRFMode(wZMF->getOpenMode()));
        return nullptr;
      }
      break;
    } // if (pZMFPath==GParser->MasterFileList[wi]->MasterFile.getURIContent())
    } // for

  if (wi == GParser->MasterFileList.count())  {  /* not found in master file list : create one in list */
    wZMF= std::make_shared<ZMasterFile>();
    if (pAccessMode == ZRF_Modify) {
      wSt=wZMF->zopen(pZMFPath,ZRF_Modify);
    }
    else {
      wSt=wZMF->zopen(pZMFPath,ZRF_Read_Only);
    }
    if (wSt!=ZS_SUCCESS) {
        GParser->ErrorLog.logZExceptionLast("ZSearchEntity::constructWithFileEntity");
        GParser->ErrorLog.logZStatus(ZAIES_Error,wSt,"ZSearchEntity::_parse-E-ERROPEN Cannot access file <%s> mode <%s>.",
                            pZMFPath.toString(),
                            decode_ZRFMode( pAccessMode));
      return nullptr;
    }
    ZMasterFileItem wMFItem(pToken->Text,wZMF);
    GParser->MasterFileList.push(wMFItem);

//    wMF = new ZSearchMasterFile(wZMF,pToken->Text);
  }// not found

  /* up to there we've got the file, let's construct _BaseFileEntity */


  return constructWithFileEntity(wZMF,pToken);
  /*
  std::shared_ptr<ZSearchEntity> wEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(wMF,pToken));
  const ZMetaDic* wMDic = wMF->getMetaDicPtr();
  for (int wi=0 ; wi < wMDic->count() ; wi++) {
      wEntity->LocalMetaDic.push(wMDic->TabConst(wi));
  }
  wEntity->BuildDic.setEqualToMetaDic(wMDic);

  return wEntity;
  */
}


std::shared_ptr<ZSearchEntity>
ZSearchEntity::constructWithFileEntity(std::shared_ptr <ZMasterFile> wMF,ZSearchToken* pToken)
{
    std::shared_ptr<ZSearchEntity> wEntity = std::shared_ptr<ZSearchEntity>(new ZSearchEntity(wMF,pToken->Text));
/*    const ZMetaDic* wMDic = wMF->Dictionary;
    for (int wi=0 ; wi < wMDic->count() ; wi++) {
        wEntity->LocalMetaDic.push(wMDic->TabConst(wi));
    }
    wEntity->BuildDic.setEqualTo(wMDic);
*/
    return wEntity;
}

#endif // __DEPRECATED__


void ZSearchEntity::_checkCircularReference(std::shared_ptr<ZSearchEntity> pTop) {
  std::shared_ptr<ZSearchEntity> wCurEntity=pTop;
  ZArray <utf8VaryingString> wNames;
  while (!wCurEntity->isFile()) {
    if (!wCurEntity->isCollection()) {
      _DBGPRINT("ZSearchEntity::_checkCircularReference Empty Search entity is neither file nor collection.")
      abort();
    }
    for (int wi=0; wi < wNames.count();wi++)
      if (wNames[wi]==wCurEntity->getEntityName()) {
        _DBGPRINT("ZSearchEntity::_checkCircularReference Circular reference detected. Search entity <%s> is declared as circular at level <%d>.",
            wCurEntity->getEntityName().toString(),
            wi)
        abort();
      }
    wNames.push(wCurEntity->getEntityName());
    wCurEntity = wCurEntity->_BaseEntity;
  } // while true
}


long
ZSearchEntity::getFieldRankbyName(const utf8VaryingString &pFieldName) const
{
    return BuildDic.getFieldRankByName(pFieldName);
/*  if (isFile())
    return _FileEntity->getFieldRankbyName(pFieldName);
  if (isCollection())
    return _CollectionEntity->getFieldRankbyName(pFieldName);
*/
}




ZStatus
ZSearchEntity::_getRawRecord(const long pRank,zaddress_type& pAddress)
{
    _URFParser.clear();
    if (isFile()) {
        return _FileEntity->get(pRank,pAddress);
    }
    //    return _CollectionEntityList[0]->get(pRecord,pRank,pAddress);
    return _BaseEntity->_getRawRecord(pRank,pAddress);
}





/*

    Entity dictionaries :

    - ZSearchDictionary : used to extract and create target URF record structure from existing URF record accessed with its Local dictionary (ZMetaDic)
    - ZMetaDic : local dictionary used to access extracted URF record structure


        Base entity                                         Entity
        Source                                              Target
        Record                                              Record
    +---------+                                             +---------+
    | bitset  |                                             | bitset  |<---------Presence bitset is adjusted
    |URF field|                                             |URF field|                     |
    |   ...   |   Record is parsed using                    |         |                     |
    |         |   Entity ZSearchDictionary----------------> |URF field|                     |
    |         |   Arithmetic expression result generates    |         |                     |
    |         |   new URF Field---------------------------> |URF field|                     |
    |         |   idem for literals                         |         |                     |
    |         |                                             +---------+                     |
    |         |             In the end                                                      |
    |         |                 |                                                           |
    |         |                 |                                                           |
    |         |                 +-----------------------------------------------------------+
    |URF field|
    +---------+


        Newly created Entity record content may be parsed with URFParser using created record and ZEntity::LocalDic

*/


/*  construct a new record from a input record coming from base entity inside an URFParser
 * NB: URFParser has Record content and is already parsed
 */
#ifdef __COMMENT__
ZStatus ZSearchEntity::constructURFRecord(URFParser &pOutParser,
                                          ZDataBuffer &pOutRecord,
                                          ZSearchDictionary &pBuildDic,
                                          URFParser &pBaseParser,
                                          ZDataBuffer &pBaseRecord,
                                          ZaiErrors *pErrorLog)
{
    ZBitset         wPresence;
    ZDataBuffer     wURFContent;
    ZOperandContent wContent;
    ZStatus         wSt=ZS_SUCCESS;

    wSt=pBaseParser.rawParse(pBaseRecord,pErrorLog);
    if (wSt!=ZS_SUCCESS) {
        return wSt;
    }
    /* creates new entity record from base record, adding URFField from source to target if present */

    wPresence._allocate(pBuildDic.count());

    /* Format out URF record data :
     *
     * Pure fields :
     *  extract URFFields and move it to out record
     *
     * Fields with modifiers :
     *      extract fields
     *      compute modifier
     *      move modifier content to out record with modifier type translated to ZType (URF type)
     *
     * Literal :
     *      new field is created with literal with its literal type translated to ZType (URF type)
     *
     * Arithmetic expression :
     *      extract fields required in expression
     *      compute expression with all fields and/or literals
     *      move result to out record with expression type translated to ZType (URF type)
     *
     */
    for (int wi=0; wi < pBuildDic.count(); wi++) {
        wContent.clear();

        ZSearchOperandBase* wSOB=static_cast<ZSearchOperandBase*>(pBuildDic[wi].Operand);
        ZSearchOperandType_type wZSTOMain = wSOB->ZSTO & (~ZSTO_BaseMask);
        switch (wZSTOMain)
        {
        case ZSTO_Field:
        {
            ZSearchFieldOperand* wSFO=static_cast<ZSearchFieldOperand*>(pBuildDic[wi].Operand);
            /* pick field content in input record according BuildDic definition */

//            URFField wURFField = pInFieldList [pSearchDic[wi].MetaDicRank] ;
            URFField wURFField ;
            if ((wSt=pBaseParser.getURFFieldByRankIncremental( pBuildDic[wi].getMetaDicRank(),wURFField))=ZS_SUCCESS)
                return wSt;

            if (wSFO->ModifierType==ZSRCH_NOTHING) { /* field has no modifier  */
                /* move it as is to out record */
                wURFContent.appendData(wURFField.FieldPtr,
                                       wURFField.Size);
            }
            else
            {
                wSt = getURFOperandContentWModifier (wURFField,wSFO->ModifierType,wSFO->ModParams,&wContent);
                if (wSt!=ZS_SUCCESS)
                    return wSt;
                /* convert resulting content to URF format */
                wSt = wContent.appendURF(wURFContent);
                if (wSt!=ZS_SUCCESS)
                    return wSt;
                if (wURFField.Present)
                    wPresence.set(wi);
            }
            break;
        } // ZSTO_Field
        case ZSTO_Literal:
        {
            ZSearchLiteral* wSOB=static_cast<ZSearchLiteral*>(pBuildDic[wi].Operand);
            wSt = wSOB->appendURF(wURFContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wPresence.set(wi);
            break;
        } // ZSTO_Literal

        case ZSTO_Logical:
        {
            ZSearchLogicalOperand* wSLOP=static_cast<ZSearchLogicalOperand*>(pBuildDic[wi].Operand);

            bool wBool=wSLOP->evaluate(pBaseParser);

            size_t wL=getAtomicURFSize<bool>(wBool);
            unsigned char* wPtr=wURFContent.extend(wL);
            exportAtomicURF_Ptr<bool>(wBool,wPtr);

            wPresence.set(wi);
            break;
        }// ZSTO_Logical

        case ZSTO_Arithmetic:
        {
            ZSearchArithmeticTerm* wSLOP=static_cast<ZSearchArithmeticTerm*>(pBuildDic[wi].Operand);
            wSt=computeArithmetic(wSLOP,pBaseParser,wZSTOMain,wContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wSt = wContent.appendURF(wURFContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wPresence.set(wi);
            break;
        } // ZSTO_Arithmetic

        default:
        {
            _DBGPRINT("ZSearchEntity::constructURFRecord-E Rank %d invalid field operand content MAIN type 0x%X %s while expecting of of ZSTO_Bool,...\n",
                      wi, wZSTOMain , decode_OperandType(wZSTOMain))
            return ZS_INVTYPE;
        }

        }//switch

    } // for

    pOutRecord.clear();
    wPresence._exportURF (pOutRecord);

    pOutRecord.appendData(wURFContent);

    return pOutParser.rawParse(pOutRecord,pErrorLog);

 //   return ZS_SUCCESS;
} // ZSearchEntity::constructURFRecord

#endif
#ifdef __COMMENT__
ZStatus ZSearchEntity::constructURFRecord(ZDataBuffer &pOutRecord,
                                          ZSearchDictionary &pBuildDic,
                                          ZDataBuffer &pBaseRecord,
                                          ZaiErrors *pErrorLog)
{
    ZBitset         wPresence;
    ZDataBuffer     wURFContent;
    ZOperandContent wContent;
    ZStatus         wSt=ZS_SUCCESS;

    URFParser pOutParser , pBaseParser ;

    wSt=pBaseParser.rawParse(pBaseRecord,pErrorLog);
    if (wSt!=ZS_SUCCESS) {
        return wSt;
    }
    /* creates new entity record from base record, adding URFField from source to target if present */

    wPresence._allocate(pBuildDic.count());

    /* Format out URF record data :
     *
     * Pure fields :
     *  extract URFFields and move it to out record
     *
     * Fields with modifiers :
     *      extract fields
     *      compute modifier
     *      move modifier content to out record with modifier type translated to ZType (URF type)
     *
     * Literal :
     *      new field is created with literal with its literal type translated to ZType (URF type)
     *
     * Arithmetic expression :
     *      extract fields required in expression
     *      compute expression with all fields and/or literals
     *      move result to out record with expression type translated to ZType (URF type)
     *
     */
    for (int wi=0; wi < pBuildDic.count(); wi++) {
        wContent.clear();

        ZSearchOperandBase* wSOB=static_cast<ZSearchOperandBase*>(pBuildDic[wi].Operand);
        ZSearchOperandType_type wZSTOMain = wSOB->ZSTO & (~ZSTO_BaseMask);
        switch (wZSTOMain)
        {
        case ZSTO_Field:
        {
            ZSearchFieldOperand* wSFO=static_cast<ZSearchFieldOperand*>(pBuildDic[wi].Operand);
            /* pick field content in input record according BuildDic definition */

            //            URFField wURFField = pInFieldList [pSearchDic[wi].MetaDicRank] ;
            URFField wURFField ;
            if ((wSt=pBaseParser.getURFFieldByRankIncremental( pBuildDic[wi].getMetaDicRank(),wURFField))=ZS_SUCCESS)
                return wSt;

            if (wSFO->ModifierType==ZSRCH_NOTHING) { /* field has no modifier  */
                /* move it as is to out record */
                wURFContent.appendData(wURFField.FieldPtr,
                                       wURFField.Size);
            }
            else
            {
                wSt = getURFOperandContentWModifier (wURFField,wSFO->ModifierType,wSFO->ModParams,&wContent);
                if (wSt!=ZS_SUCCESS)
                    return wSt;
                /* convert resulting content to URF format */
                wSt = wContent.appendURF(wURFContent);
                if (wSt!=ZS_SUCCESS)
                    return wSt;
                if (wURFField.Present)
                    wPresence.set(wi);
            }
            break;
        } // ZSTO_Field
        case ZSTO_Literal:
        {
            ZSearchLiteral* wSOB=static_cast<ZSearchLiteral*>(pBuildDic[wi].Operand);
            wSt = wSOB->appendURF(wURFContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wPresence.set(wi);
            break;
        } // ZSTO_Literal

        case ZSTO_Logical:
        {
            ZSearchLogicalOperand* wSLOP=static_cast<ZSearchLogicalOperand*>(pBuildDic[wi].Operand);

            bool wBool=wSLOP->evaluate(pBaseParser);

            size_t wL=getAtomicURFSize<bool>(wBool);
            unsigned char* wPtr=wURFContent.extend(wL);
            exportAtomicURF_Ptr<bool>(wBool,wPtr);

            wPresence.set(wi);
            break;
        }// ZSTO_Logical

        case ZSTO_Arithmetic:
        {
            ZSearchArithmeticTerm* wSLOP=static_cast<ZSearchArithmeticTerm*>(pBuildDic[wi].Operand);
            wSt=computeArithmetic(wSLOP,pBaseParser,wZSTOMain,wContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wSt = wContent.appendURF(wURFContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wPresence.set(wi);
            break;
        } // ZSTO_Arithmetic

        default:
        {
            _DBGPRINT("ZSearchEntity::constructURFRecord-E Rank %d invalid field operand content MAIN type 0x%X %s while expecting of of ZSTO_Bool,...\n",
                      wi, wZSTOMain , decode_OperandType(wZSTOMain))
            return ZS_INVTYPE;
        }

        }//switch

    } // for

    pOutRecord.clear();
    wPresence._exportURF (pOutRecord);

    pOutRecord.appendData(wURFContent);

    return ZS_SUCCESS;

//    return pOutParser.rawParse(pOutRecord,pErrorLog);

    //   return ZS_SUCCESS;
} // ZSearchEntity::constructURFRecord

ZStatus ZSearchEntity::constructURFRecord(ZDataBuffer &pOutRecord,
                                          ZDataBuffer &pBaseRecord)
{
    if (isFile()) {
        pOutRecord.setData(pBaseRecord);
        return ZS_SUCCESS;
    }
    ZBitset         wPresence;
    ZDataBuffer     wURFContent;
    ZOperandContent wContent;
    ZStatus         wSt=ZS_SUCCESS;

    URFParser wBaseParser ;

    if (BuildDic.count()==0) {
        ErrorLog->errorLog("Entity <%s> build dictionary is empty. Cannot build any record.",getEntityName().toString());
        return ZS_EMPTY;
    }

    wSt=wBaseParser.rawParse(pBaseRecord,ErrorLog);
    if (wSt!=ZS_SUCCESS) {
        return wSt;
    }
    /* creates new entity record from base record, adding URFField from source to target if present */

    wPresence._allocate(BuildDic.count());

    /* Format out URF record data :
     *
     * Pure fields :
     *  extract URFFields and move it to out record
     *
     * Fields with modifiers :
     *      extract fields
     *      compute modifier
     *      move modifier content to out record with modifier type translated to ZType (URF type)
     *
     * Literal :
     *      new field is created with literal with its literal type translated to ZType (URF type)
     *
     * Arithmetic expression :
     *      extract fields required in expression
     *      compute expression with all fields and/or literals
     *      move result to out record with expression type translated to ZType (URF type)
     *
     */

    for (int wi=0; wi < BuildDic.count(); wi++) {
        wContent.clear();

        ZSearchOperandBase* wSOB=static_cast<ZSearchOperandBase*>(BuildDic[wi].Operand);
        ZSearchOperandType_type wZSTOMain = wSOB->ZSTO & (~ZSTO_BaseMask);
        switch (wZSTOMain)
        {
        case ZSTO_Field:
        {
            ZSearchFieldOperand* wSFO=static_cast<ZSearchFieldOperand*>(BuildDic[wi].Operand);
            /* pick field content in input record according BuildDic definition */

            //            URFField wURFField = pInFieldList [pSearchDic[wi].MetaDicRank] ;
            URFField wURFField ;

            _DBGPRINT("ZSearchEntity::constructURFRecord getting field target rank %d base entity rank %ld\n", wi,BuildDic[wi].getMetaDicRank())
            wSt = wBaseParser._getURFFieldByRank(BuildDic[wi].getMetaDicRank(),wURFField) ;
            if (wURFField.Present) {
                wPresence.set(wi);
/*
            if ((wSt=wBaseParser.getURFFieldByRankIncremental( BuildDic[wi].getMetaDicRank(),wURFField))=ZS_SUCCESS)
                return wSt;
*/
            if (wSFO->ModifierType==ZSRCH_NOTHING) { /* field has no modifier  */
                /* move it as is to out record */
                wURFContent.appendData(wURFField.FieldPtr,
                                       wURFField.Size);
            }
            else
            {
                wSt = getURFOperandContentWModifier (wURFField,wSFO->ModifierType,wSFO->ModParams,&wContent);
                if (wSt!=ZS_SUCCESS)
                    return wSt;
                /* convert resulting content to URF format */
                wSt = wContent.appendURF(wURFContent);
                if (wSt!=ZS_SUCCESS)
                    return wSt;
            }
            } // present

            break;
        } // ZSTO_Field
        case ZSTO_Literal:
        {
            ZSearchLiteral* wSOB=static_cast<ZSearchLiteral*>(BuildDic[wi].Operand);
            wSt = wSOB->appendURF(wURFContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wPresence.set(wi);
            break;
        } // ZSTO_Literal

        case ZSTO_Logical:
        {
            ZSearchLogicalOperand* wSLOP=static_cast<ZSearchLogicalOperand*>(BuildDic[wi].Operand);

            bool wBool=wSLOP->evaluate(wBaseParser);

            size_t wL=getAtomicURFSize<bool>(wBool);
            unsigned char* wPtr=wURFContent.extend(wL);
            exportAtomicURF_Ptr<bool>(wBool,wPtr);

            wPresence.set(wi);
            break;
        }// ZSTO_Logical

        case ZSTO_Arithmetic:
        {
            ZSearchArithmeticTerm* wSLOP=static_cast<ZSearchArithmeticTerm*>(BuildDic[wi].Operand);
            wSt=computeArithmetic(wSLOP,wBaseParser,wZSTOMain,wContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wSt = wContent.appendURF(wURFContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wPresence.set(wi);
            break;
        } // ZSTO_Arithmetic

        default:
        {
            _DBGPRINT("ZSearchEntity::constructURFRecord-E Rank %d invalid field operand content MAIN type 0x%X %s while expecting of of ZSTO_Bool,...\n",
                      wi, wZSTOMain , decode_OperandType(wZSTOMain))
            return ZS_INVTYPE;
        }

        }//switch

    } // for

    pOutRecord.clear();
    wPresence._exportURF (pOutRecord);

    pOutRecord.appendData(wURFContent);

    return ZS_SUCCESS;
} // ZSearchEntity::constructURFRecord

#endif // __COMMENT__

//ZStatus ZSearchEntity::constructURF(URFParser &pInParser)
/**
 * @brief ZSearchEntity::constructURF constructs URFParser for current entity using either _BaseEntity or _FileEntity URFParser.
 *                                    at the end, URFParser has an appropriate record and an updated URF field list.
 * @return
 */
ZStatus ZSearchEntity::constructURF()
{
    if (isFile()) {
        if (_FileEntity->_URFParser.URFPS != URFPS_TotallyParsed) {
            ErrorLog->errorLog("ZSearchEntity::constructURF File entity <%s> URF parser state is invalid <%s> (Not totally parsed).",
                               getEntityName().toString(),decode_URFPS(_FileEntity->_URFParser.URFPS));
            _DBGPRINT("ZSearchEntity::constructURF File entity <%s> URF parser state is invalid <%s> (Not totally parsed).",
                      getEntityName().toString(),decode_URFPS(_FileEntity->_URFParser.URFPS));
            exit(EXIT_FAILURE);
        }
        _URFParser.setRecord(_FileEntity->_URFParser.getRecord());
        ZStatus wSt=_URFParser.parse(ErrorLog);
        if (wSt!=ZS_SUCCESS)
            return wSt;

        _URFParser.URFPS = URFPS_TotallyParsed ;
        return ZS_SUCCESS;
    }
    ZBitset         wPresence;
    ZDataBuffer     wURFContent;
    ZOperandContent wContent;
    ZStatus         wSt=ZS_SUCCESS;

/*  may be it is a join entity : parser is located at field level (see ZSearchFiedlOperand::Entity)
 *
    if (_BaseEntity->_URFParser.URFPS < URFPS_TotallyParsed) {
        if ((_BaseEntity->_URFParser.URFPS == URFPS_Nothing)||(_BaseEntity->_URFParser.Record.isEmpty())) {
            _DBGPRINT(" ZSearchEntity::constructURF-F-RECNULL Base URF record is empty or base URF parser has not been set.")
            exit(EXIT_FAILURE);
        }
        _DBGPRINT(" ZSearchEntity::constructURF-F-PARTIAL Base URF record is partially parsed (must be totally parsed).")
        exit(EXIT_FAILURE);
    }
*/
    wPresence._allocate(BuildDic.count());

    /* Format out URF record data :
     *
     * Pure fields :
     *  extract URFFields and move it to out record
     *
     * Fields with modifiers :
     *      extract fields from record using ZSearchEntity::_URFParser
     *      compute modifier if any
     *      move modifier content to out record with modifier type translated to ZType (URF type)
     *
     * Literal :
     *      new field is created with literal with its literal type translated to ZType (URF type)
     *
     * Arithmetic expression :
     *      extract fields required in expression
     *      compute expression with all fields and/or literals
     *      move result to out record with expression type translated to ZType (URF type)
     *
     */

    for (int wi=0; wi < BuildDic.count(); wi++) {
        wContent.clear();

        ZSearchOperandBase* wSOB=static_cast<ZSearchOperandBase*>(BuildDic[wi].Operand);
        ZSearchOperandType_type wZSTOMain = wSOB->ZSTO & (~ZSTO_BaseMask);
        switch (wZSTOMain)
        {
        case ZSTO_Field:
        {
            ZSearchFieldOperand* wSFO=static_cast<ZSearchFieldOperand*>(BuildDic[wi].Operand);
            /* pick field content in input record according BuildDic definition */
            URFField wURFField ;
            _DBGPRINT("ZSearchEntity::constructURF getting field from entity <%s> local metadic rank %ld\n",
                      BuildDic[wi].getEntity()->getEntityName().toCChar(),
                      BuildDic[wi].getMetaDicRank())

            /* URF parser is located at field level (see ZSearchFiedlOperand::Entity). It must be fully updated. */
            wSt = BuildDic[wi].getURFParser()->_getURFFieldByRank(BuildDic[wi].getMetaDicRank(),wURFField) ;
            if (wSt!=ZS_SUCCESS)
                return wSt;

            if (wURFField.Present) {
                wPresence.set(wi);

                if (wSFO->ModifierType==ZSRCH_NOTHING) { /* field has no modifier  */
                    /* move it as is to out record */
                    wURFContent.appendData(wURFField.FieldPtr,
                                           wURFField.Size);
                }
                else
                {
                    wSt = getURFOperandContentWModifier (wURFField,wSFO->ModifierType,wSFO->ModParams,&wContent);
                    if (wSt!=ZS_SUCCESS)
                        return wSt;
                    /* convert resulting content to URF format */
                    wSt = wContent.appendURF(wURFContent);
                    if (wSt!=ZS_SUCCESS)
                        return wSt;
                }
            } // present

            break;
        } // ZSTO_Field
        case ZSTO_Literal:
        {
            ZSearchLiteral* wSOB=static_cast<ZSearchLiteral*>(BuildDic[wi].Operand);
            wSt = wSOB->appendURF(wURFContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wPresence.set(wi);
            break;
        } // ZSTO_Literal

        case ZSTO_Logical:
        {
            ZSearchLogicalOperand* wSLOP=static_cast<ZSearchLogicalOperand*>(BuildDic[wi].Operand);

            bool wBool=wSLOP->evaluate(_BaseEntity->_URFParser);

            size_t wL=getAtomicURFSize<bool>(wBool);
            unsigned char* wPtr=wURFContent.extend(wL);
            exportAtomicURF_Ptr<bool>(wBool,wPtr);

            wPresence.set(wi);
            break;
        }// ZSTO_Logical

        case ZSTO_Arithmetic:
        {
            ZSearchArithmeticTerm* wSLOP=static_cast<ZSearchArithmeticTerm*>(BuildDic[wi].Operand);
            wSt=computeArithmetic(wSLOP,_BaseEntity->_URFParser,wZSTOMain,wContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wSt = wContent.appendURF(wURFContent);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wPresence.set(wi);
            break;
        } // ZSTO_Arithmetic

        default:
        {
            _DBGPRINT("ZSearchEntity::constructURFRecord-E Rank %d invalid field operand content MAIN type 0x%X %s while expecting of of ZSTO_Bool,...\n",
                      wi, wZSTOMain , decode_OperandType(wZSTOMain))
            return ZS_INVTYPE;
        }

        }//switch

    } // for

    _URFParser.Record.clear();

    wPresence._exportURF (_URFParser.Record);
    _URFParser.Record.appendData(wURFContent);
    _URFParser.URFPS = URFPS_RecordSet ;

    return _URFParser.parse(ErrorLog);
} // ZSearchEntity::constructURF


/* has to be modifier to use indexes */
ZStatus
ZSearchEntity::findFirstFieldValueSequential(long pFieldRank,ZOperandContent* pSearchValue,ZDataBuffer& pOutRecord)
{
    if (CaptureTime)
        ProcessTi.start();
    ZStatus wSt=ZS_SUCCESS;
    long wRank=0;
    zaddress_type wAddress=0;
    ZOperandContent wOpContent;
    ZSearchTokentype_type wZSTO = pSearchValue->OperandZSTO;
    wSt=getFirst(wAddress);
    while (wSt==ZS_SUCCESS)
    {
        if ((pFieldRank < _URFParser.URFFieldList.count()) && (_URFParser.URFFieldList[pFieldRank].Present)) {
            wSt=getURFOperandContentWModifier(_URFParser.URFFieldList[pFieldRank],ZSRCH_NOTHING,nullptr,&wOpContent);
            if (wSt==ZS_SUCCESS) {
                if (pSearchValue->isSame( &wOpContent ) ) {
                    wSt = ZS_FOUND;
                    goto findFirstFieldValueSequentialEnd;
                }
            }
        }
        wRank++;
        wSt=getNext(wAddress);
    }
    wSt = ZS_NOTFOUND;
findFirstFieldValueSequentialEnd:
    if (CaptureTime)
        ProcessTi.end();
    return wSt;
}


ZStatus ZSearchEntity::getFirst(zaddress_type &pAddress)
{
    if (CaptureTime)
        ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock : no status returned */
    ZStatus wSt=_getFirst(pAddress);
    _Mutex.unlock();
    if (CaptureTime)
        ProcessTi.end();
    return wSt;
} //ZSearchEntity::getFirst

ZStatus ZSearchEntity::_getFirst(zaddress_type &pAddress)
{
    ZDataBuffer wBaseRecord;
    bool wResult=true;
    long wRank=0;
    ZStatus wSt=ZS_SUCCESS;
    if (_FileEntity!=nullptr) {
        wSt=_FileEntity->getFirst(pAddress);
        if (wSt!=ZS_SUCCESS)
            return wSt;

//        wSt = evaluateRecord(wResult,wBaseRecord);
        wSt = evaluateRecord(wResult);

        if (wSt!=ZS_SUCCESS)
            return wSt;
        while ((!wResult)&&(wSt==ZS_SUCCESS)) {
            wSt=_FileEntity->getNext(wRank,pAddress);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wSt = evaluateRecord(wResult);
        }
        if (wSt!=ZS_SUCCESS)
            return wSt;
        return constructURF();
    } // file

    wSt=_BaseEntity->getFirst(pAddress); /* use URFParser feeding while accessing base entity */
    if (wSt!=ZS_SUCCESS)
        return wSt;
//    wSt = evaluateRecord(wResult,wBaseRecord);
    wSt = evaluateRecord(wResult);
    if (wSt!=ZS_SUCCESS)
        return wSt;
    while ((!wResult)&&(wSt==ZS_SUCCESS)) {
        wSt=_BaseEntity->getNext(pAddress);
        if (wSt!=ZS_SUCCESS)
            return wSt;
//        wSt = evaluateRecord(wResult,wBaseRecord);
        wSt = evaluateRecord(wResult);
    }
    if (wSt!=ZS_SUCCESS)
        return wSt;
    /* construct entity record from base raw record using BuildDic */
//    return constructURFRecord(_URFParser,pEntityRecord,BuildDic,_BaseEntity->_URFParser,wBaseRecord,ErrorLog);
//    return constructURFRecord(pEntityRecord,wBaseRecord);
    return constructURF();
} //ZSearchEntity::_getFirst

ZStatus ZSearchEntity::getNext(zaddress_type &pAddress)
{
    if (CaptureTime)
        ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock  : no status returned */
    ZStatus wSt=_getNext(pAddress);
    _Mutex.unlock();
    if (CaptureTime)
        ProcessTi.end();
    return wSt;
} //ZSearchEntity::getNext

ZStatus ZSearchEntity::_getNext(zaddress_type &pAddress)
{
//    ZDataBuffer wBaseRecord;
    bool wResult=false;
    long wRank=0;
    ZStatus wSt=ZS_SUCCESS;
    if (_FileEntity!=nullptr) {
        while ((!wResult)&&(wSt==ZS_SUCCESS)) {
            wSt=_FileEntity->getNext(wRank,pAddress);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wSt = evaluateRecord(wResult);
        }
        if (wSt!=ZS_SUCCESS)
            return wSt;
        return constructURF();
    } // file

    while ((!wResult)&&(wSt==ZS_SUCCESS)) {
        wSt=_BaseEntity->getNext(pAddress);
        if (wSt!=ZS_SUCCESS)
            return wSt;
//            wSt = evaluateRecord(wResult,wBaseRecord);
        wSt = evaluateRecord(wResult);
    }
    if (wSt!=ZS_SUCCESS)
        return wSt;
    return constructURF();
} //ZSearchEntity::_getNext

ZStatus
ZSearchEntity::populateFirst(long pCount)
{
    if (pCount <= 0)
        return populateAll();

    int wRet=_Mutex.lock();
    if (wRet) {
        if (wRet==EPERM)
            return ZS_LOCKED;
        utf8VaryingString wEr=decode_MutexError("ZSearchEntity::populateFirst",wRet);
        ErrorLog->errorLog("Error on entity mutex lock %X %s",wRet,wEr.toString());
        return ZS_LOCKERROR;
    }

    if (CaptureTime)
        ProcessTi.start();

    ZStatus wSt=ZS_SUCCESS;
//    ZDataBuffer wRecord;
    zaddress_type wAddress;
    AddressList.clear();

    wSt=getFirst(wAddress);
    if (wSt!=ZS_SUCCESS)
        goto populateFirstEnd;
    AddressList.push(wAddress);
    pCount--;
    while ((wSt==ZS_SUCCESS)&&(pCount>0)) {

        wSt=getNext(wAddress);
        if (wSt!=ZS_SUCCESS)
            goto populateFirstEnd;
        AddressList.push(wAddress);
        pCount--;
    }
populateFirstEnd:
    if (wSt==ZS_EOF)
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (CaptureTime)
        ProcessTi.end();
    return wSt;
} // ZSearchEntity::populateFirst

ZStatus
ZSearchEntity::populateNext(long pCount)
{
    int wRet = _Mutex.lock();
    if (wRet) {
        if (wRet==EPERM)
            return ZS_LOCKED;
        utf8VaryingString wEr=decode_MutexError("ZSearchEntity::populateFirst",wRet);
        ErrorLog->errorLog("Error on entity mutex lock %X %s",wRet,wEr.toString());
        return ZS_LOCKERROR;
    }
    if (CaptureTime)
        ProcessTi.start();

    ZStatus wSt=ZS_SUCCESS;
//    ZDataBuffer wRecord;
    zaddress_type wAddress;
    if (pCount > 0) {
        while ((wSt==ZS_SUCCESS)&&(pCount>0)) {
            wSt=getNext(wAddress);
            if (wSt!=ZS_SUCCESS)
                goto populateNextEnd;
            AddressList.push(wAddress);
            pCount--;
        }
        goto populateNextEnd;
    }

    /* if pCount is equal or less than 0, then AddressList is populated til end of available records */
    while (wSt==ZS_SUCCESS) {
        wSt=getNext(wAddress);
        if (wSt!=ZS_SUCCESS)
            goto populateNextEnd;;
        AddressList.push(wAddress);
    }
populateNextEnd:
    if (wSt==ZS_EOF)
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (CaptureTime)
        ProcessTi.end();
    return wSt;
} // ZSearchEntity::populateNext

ZStatus
ZSearchEntity::populateAll(std::shared_ptr<ZSearchEntity> pEntity)
{
    return pEntity->populateAll();
}


ZStatus
ZSearchEntity::populateAll()
{
    if (CaptureTime)
        ProcessTi.start();
    if (FetchState == EFST_Total) {
        if (CaptureTime)
            ProcessTi.end();
        return ZS_SUCCESS;
    }
    _Mutex.lock();
    ZStatus wSt=ZS_SUCCESS;
//    ZDataBuffer wRecord;
    zaddress_type wAddress;
    AddressList.clear();

    /* check if base entity has been populated : if not Fetch it all
     * Nb a _FileEntity is always 100% populated
     */

    if ((isCollection()) && (_BaseEntity->FetchState!=EFST_Total)) {
        wSt=_BaseEntity->populateAll();
        if (wSt!=ZS_SUCCESS)
            goto populateAllEnd;
    }


    wSt=_getFirst(wAddress);
    if (wSt!=ZS_SUCCESS)
        goto populateAllEnd;
    AddressList.push(wAddress);
    while (wSt==ZS_SUCCESS) {
        wSt=_getNext(wAddress);
        if (wSt!=ZS_SUCCESS)
            goto populateAllEnd;
        AddressList.push(wAddress);
    }
populateAllEnd:
    if ((wSt==ZS_EOF)||(wSt==ZS_OUTBOUNDHIGH)||(wSt==ZS_SUCCESS))
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
     _Mutex.unlock();

    if (CaptureTime)
        ProcessTi.end();
    if ((wSt==ZS_EOF)||(wSt==ZS_OUTBOUNDHIGH))
         wSt=ZS_SUCCESS;
    return wSt;
} // ZSearchEntity::populateAll

ZStatus ZSearchEntity::getByRank(long pRank)
{
    if (CaptureTime)
        ProcessTi.start();
    ZStatus wSt=ZS_SUCCESS;
    if (pRank > AddressList.count()) {
        wSt=populateNext(pRank+1-AddressList.count());
        if (wSt!=ZS_SUCCESS)
            goto getByRankEnd;
    }

    wSt= _getByAddress(AddressList[pRank]);

getByRankEnd:
    if (CaptureTime)
        ProcessTi.end();
    return wSt;

} // ZSearchEntity::getByRank

/* record address is reputated to be valided by selection clause */
ZStatus
ZSearchEntity::_getByAddress(zaddress_type pAddress)
{

    ZStatus wSt=ZS_SUCCESS;
//    ZDataBuffer wBaseRecord;

    if (_FileEntity!=nullptr) {
        wSt = _FileEntity->getByAddress(pAddress);
        if (wSt!=ZS_SUCCESS)
            return wSt;
        return constructURF();
    }

    wSt =_BaseEntity->_getByAddress(pAddress);
    if (wSt!=ZS_SUCCESS)
        return wSt;
    return constructURF();

//    return constructURFRecord(_URFParser,pRecord,BuildDic,_FileEntity->_URFParser,wBaseRecord,ErrorLog);
//    return constructURFRecord(pRecord,wBaseRecord);
 //   return constructURF(pRecord);
} // ZSearchEntity::_getByAddress

/* record is evaluated using base dictionary and not entity dictionary :
 * selection may occur on fields wich are not selected for entity
 */
#ifdef __DEPRECATED__
ZStatus
ZSearchEntity::evaluateRecord(bool &pOutResult,const ZDataBuffer& pBaseRecord)
{
    if (LogicalTerm==nullptr) { /* No selection rule : all records are selected */
        pOutResult = true;
        return ZS_SUCCESS;
    }
    if (_FileEntity!=nullptr) {
 //       _FileEntity->_URFParser.set(pBaseRecord);
        /* parsing is done during record get */
        return LogicalTerm->evaluate(_FileEntity->_URFParser , pOutResult);
    }

//    _BaseEntity->_URFParser.set(pBaseRecord);
    /* parsing is done during record get */
    return LogicalTerm->evaluate(_BaseEntity->_URFParser , pOutResult);
}

ZStatus
ZSearchEntity::evaluateBaseRecord(bool &pOutResult)
{
    if (LogicalTerm==nullptr) { /* No selection rule : all records are selected */
        pOutResult = true;
        return ZS_SUCCESS;
    }

    if (_FileEntity!=nullptr) {
        //       _FileEntity->_URFParser.set(pBaseRecord);
        if (_FileEntity->_URFParser.URFPS < URFPS_TotallyParsed) {
            if ((_FileEntity->_URFParser.URFPS == URFPS_Nothing)||(_FileEntity->_URFParser.Record.isEmpty())) {
                _DBGPRINT(" ZSearchEntity::evaluateRecord-F-RECNULL Base URF record is empty or base URF parser has not been set.")
                exit(EXIT_FAILURE);
            }
            _DBGPRINT(" ZSearchEntity::evaluateRecord-F-PARTIAL Base URF record is partially parsed (must be totally parsed).")
            exit(EXIT_FAILURE);
        }
        /* parsing is done during record get */
//        return LogicalTerm->evaluate(_FileEntity->_URFParser , pOutResult);
        return LogicalTerm->evaluate( pOutResult);
    }

    if (_BaseEntity->_URFParser.URFPS < URFPS_TotallyParsed) {
            if ((_BaseEntity->_URFParser.URFPS == URFPS_Nothing)||(_FileEntity->_URFParser.Record.isEmpty())) {
                _DBGPRINT(" ZSearchEntity::evaluateRecord-F-RECNULL Base URF record is empty or base URF parser has not been set.")
                exit(EXIT_FAILURE);
            }
            _DBGPRINT(" ZSearchEntity::evaluateRecord-F-PARTIAL Base URF record is partially parsed (must be totally parsed).")
            exit(EXIT_FAILURE);
    }
    //    _BaseEntity->_URFParser.set(pBaseRecord);
    /* parsing is done during record get */
//    return LogicalTerm->evaluate(_BaseEntity->_URFParser , pOutResult);
    return LogicalTerm->evaluate(pOutResult);
}

#endif // __DEPRECATED__
ZStatus
ZSearchEntity::evaluateRecord(bool &pOutResult)
{
    if (LogicalTerm==nullptr) {
        pOutResult=true;
        return ZS_SUCCESS ;
    }
    return LogicalTerm->evaluate(pOutResult);
}

ZStatus
ZSearchEntity::populateAllJoin()
{
    if (CaptureTime)
        ProcessTi.start();
    if (FetchState == EFST_Total) {
        if (CaptureTime)
            ProcessTi.end();
        return ZS_SUCCESS;
    }
    _Mutex.lock();
    ZStatus wSt=ZS_SUCCESS;
    bool wResult=false;

    zaddress_type wMAddress,wSAddress;
    JoinAddressList.clear();
    AddressList.clear();

    ZSearchJoinTuple wAddress;

    _JoinList[0]->CaptureTime=_JoinList[1]->CaptureTime=false;

    wSt=_getFirstJoin(wAddress);

    while (wSt==ZS_SUCCESS) {
        JoinAddressList.push(wAddress);
        wSt=_getNextJoin(wAddress);
    }// while


populateAllJoinEnd:
    if ((wSt==ZS_OUTBOUNDHIGH)||(wSt==ZS_SUCCESS))
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (CaptureTime)
        ProcessTi.end();
    if ((wSt==ZS_EOF)||(wSt==ZS_OUTBOUNDHIGH))
        wSt=ZS_SUCCESS;
    return wSt;
} // ZSearchEntity::populateAllJoin

ZStatus
ZSearchEntity::populateFirstJoin(int pCount)
{
    if (CaptureTime)
        ProcessTi.start();
    if (FetchState == EFST_Total) {
        if (CaptureTime)
            ProcessTi.end();
        return ZS_SUCCESS;
    }
    _Mutex.lock();
    ZStatus wSt=ZS_SUCCESS;
    bool wResult=false;

    JoinAddressList.clear();
    AddressList.clear();

    ZSearchJoinTuple wAddress;

    _JoinList[0]->CaptureTime=_JoinList[1]->CaptureTime=false;

    wSt=_getFirstJoin(wAddress);

    while ((wSt==ZS_SUCCESS) && --pCount) {
        JoinAddressList.push(wAddress);
        wSt=_getNextJoin(wAddress);
    }// while

populateFirstJoinEnd:
    if ((wSt==ZS_OUTBOUNDHIGH)||(wSt==ZS_SUCCESS))
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (CaptureTime)
        ProcessTi.end();
    if ((wSt==ZS_EOF)||(wSt==ZS_OUTBOUNDHIGH))
        wSt=ZS_SUCCESS;
    return wSt;
} // ZSearchEntity::populateFirstJoin

ZStatus
ZSearchEntity::populateNextJoin(int pCount)
{
    if (CaptureTime)
        ProcessTi.start();
    if (FetchState == EFST_Total) {
        if (CaptureTime)
            ProcessTi.end();
        return ZS_SUCCESS;
    }
    _Mutex.lock();
    ZStatus wSt=ZS_SUCCESS;
    bool wResult=false;


    ZSearchJoinTuple wAddress;

    _JoinList[0]->CaptureTime=_JoinList[1]->CaptureTime=false;

    wSt=_getNextJoin(wAddress);

    while ((wSt==ZS_SUCCESS) && --pCount) {
        JoinAddressList.push(wAddress);
        wSt=_getNextJoin(wAddress);
    }// while

populateFirstJoinEnd:
    if ((wSt==ZS_OUTBOUNDHIGH)||(wSt==ZS_SUCCESS))
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (CaptureTime)
        ProcessTi.end();
    if ((wSt==ZS_EOF)||(wSt==ZS_OUTBOUNDHIGH))
        wSt=ZS_SUCCESS;
    return wSt;
} // ZSearchEntity::populateNextJoin


ZStatus ZSearchEntity::getFirstJoin(ZSearchJoinTuple &pAddress)
{
    if (CaptureTime)
        ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock : no status returned */
    ZStatus wSt=_getFirstJoin(pAddress);
    _Mutex.unlock();
    if (CaptureTime)
        ProcessTi.end();
    return wSt;
} //ZSearchEntity::getFirstJoin

ZStatus ZSearchEntity::_getFirstJoin(ZSearchJoinTuple &pAddress)
{
    bool wResult=false;

    ZStatus wSt=ZS_SUCCESS;

    pAddress.setInvalid();

    _JoinList[0]->CaptureTime=_JoinList[1]->CaptureTime=false;

    wSt=_JoinList[0]->_getFirst(pAddress.MasterAddress);
    if (wSt!=ZS_SUCCESS)
        return wSt;
    wSt=_JoinList[1]->_getFirst(pAddress.SlaveAddress);
    if (wSt!=ZS_SUCCESS)
        return wSt;
    wSt=constructURF();
    if (wSt!=ZS_SUCCESS)
        return wSt;
    wSt = evaluateRecord(wResult);
    if (wSt!=ZS_SUCCESS)
        return wSt;

    while (!wResult && (wSt==ZS_SUCCESS)) {
        while ((!wResult)&&(wSt==ZS_SUCCESS)) {
            wSt=constructURF();
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wSt = evaluateRecord(wResult);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            if (wResult) {
                if (_Using != nullptr) {
                    wSt=_Using->evaluate(wResult) ;
                    if (wSt!=ZS_SUCCESS)
                        return wSt;
                    if (wResult)
                        return wSt;
                }
            }
            wSt=_JoinList[1]->_getNext(pAddress.SlaveAddress);
        }// while
        if ((wSt!=ZS_EOF)&&(wSt!=ZS_OUTBOUNDHIGH))
            break;
        wSt=_JoinList[0]->_getNext(pAddress.MasterAddress);
        if (wSt==ZS_SUCCESS) {
            wSt=_JoinList[1]->_getFirst(pAddress.SlaveAddress);
        }
    }// while
    return wSt;
} //ZSearchEntity::_getFirstJoin


ZStatus ZSearchEntity::getNextJoin(ZSearchJoinTuple &pAddress)
{
    if (CaptureTime)
        ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock : no status returned */
    ZStatus wSt=_getNextJoin(pAddress);
    _Mutex.unlock();
    if (CaptureTime)
        ProcessTi.end();
    return wSt;
} //ZSearchEntity::getFirstJoin

ZStatus ZSearchEntity::_getNextJoin(ZSearchJoinTuple &pAddress)
{
    bool wResult=false;

    ZStatus wSt=ZS_SUCCESS;
    pAddress.setInvalid();

    wSt=_JoinList[1]->_getNext(pAddress.SlaveAddress);
    if ((wSt!=ZS_EOF)&&(wSt!=ZS_OUTBOUNDHIGH))
        return wSt;
    while (!wResult && (wSt==ZS_SUCCESS)) {
        while ((!wResult)&&(wSt==ZS_SUCCESS)) {
            wSt=constructURF();
            if (wSt!=ZS_SUCCESS)
                return wSt;
            wSt = evaluateRecord(wResult);
            if (wSt!=ZS_SUCCESS)
                return wSt;
            if (wResult) {
                if (_Using != nullptr) {  /* if no <USING> clause then a cartesian product is made */
                    wSt=_Using->evaluate(wResult) ;
                    if (wSt!=ZS_SUCCESS)
                        return wSt;
                    if (wResult)
                        return wSt;
                }
            }
            wSt=_JoinList[1]->_getNext(pAddress.SlaveAddress);
        }// while
        if ((wSt!=ZS_EOF)&&(wSt!=ZS_OUTBOUNDHIGH))
            break;
        wSt=_JoinList[0]->_getNext(pAddress.MasterAddress);
        if (wSt==ZS_SUCCESS) {
            wSt=_JoinList[1]->_getFirst(pAddress.SlaveAddress);
        }
    }// while
    return wSt;

} //ZSearchEntity::_getNextJoin


ZStatus ZSearchEntity::_getByAddressJoin(ZSearchJoinTuple& pAddress)
{
    ZStatus wSt=ZS_SUCCESS;
    pAddress.setInvalid();

    wSt=_JoinList[0]->_getByAddress(pAddress.SlaveAddress);
    if (wSt!=ZS_SUCCESS)
        return wSt;
    wSt=_JoinList[1]->_getByAddress(pAddress.SlaveAddress);
    if (wSt!=ZS_SUCCESS)
        return wSt;
    return constructURF();

} //ZSearchEntity::_getByAddressJoin

ZStatus ZSearchEntity::_getByRankJoin(long pRank,ZSearchJoinTuple &pAddress)
{
    if (CaptureTime)
        ProcessTi.start();
    ZStatus wSt=ZS_SUCCESS;
    if (pRank > JoinAddressList.count()) {
        wSt=populateNextJoin(pRank+1-JoinAddressList.count());
        if (wSt!=ZS_SUCCESS)
            goto _getByRankJoinEnd;
    }

    wSt= _getByAddressJoin(JoinAddressList[pRank]);

_getByRankJoinEnd:
    if (CaptureTime)
        ProcessTi.end();
    return wSt;
} //ZSearchEntity::_getByRankJoin

}//namespace zbs
