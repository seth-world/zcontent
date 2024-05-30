#include "zsearchentity.h"
#include "zsearchentitycontext.h"
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

 // _URFParser.setDictionary(&LocalMetaDic);

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
ZSearchEntity::_getRawRecord(ZSearchEntityContext& pSEC,const long pRank,zaddress_type& pAddress)
{
    pSEC._URFParser.clear();
    if (isFile()) {
        return _FileEntity->getByRank(pSEC,pRank,pAddress);
    }
    //    return _CollectionEntityList[0]->get(pRecord,pRank,pAddress);
    return _BaseEntity->_getRawRecord(pSEC,pRank,pAddress);
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



ZSearchEntityContext ZSearchEntity::newEntityContext(std::shared_ptr<ZSearchEntity> pEntity)
{
    ZSearchEntityContext wCtx ;
    wCtx.Entity = pEntity;
    wCtx._URFParser.setDictionary(&pEntity->LocalMetaDic);
    wCtx.ErrorLog = pEntity->ErrorLog;
    if (pEntity->isFile()) {
        wCtx.BaseContext = new ZSearchEntityContext(pEntity->_FileEntity->newEntityContext());
    }
    if (pEntity->isCollection()) {
        wCtx.BaseContext = new ZSearchEntityContext(ZSearchEntity::newEntityContext(pEntity->_BaseEntity));
    }
    if (pEntity->isJoin()) {
        wCtx.BaseContext = new ZSearchEntityContext(ZSearchEntity::newEntityContext(pEntity->_JoinList[0]));
        wCtx.SlaveContext = new ZSearchEntityContext(ZSearchEntity::newEntityContext(pEntity->_JoinList[1]));
    }
    return wCtx;
}

URFParser* ZSearchEntity::getParserFromField(ZSearchEntityContext& pSEC,ZSearchField& pField )
{
    if (pSEC.BaseContext==nullptr) {
        _DBGPRINT("ZSearchEntity::getParserFromField Entity context has no base context defined or is corrupted.");
        pSEC.Status = ZS_NULLPTR;
        abort();
    }
    if (isJoin()) {
        if (pSEC.SlaveContext==nullptr) {
            _DBGPRINT("ZSearchEntity::getParserFromField Entity context (join entity) has no slave context defined or is corrupted.");
            pSEC.Status = ZS_NULLPTR;
            abort();
        }
        if (pField.getEntity()==_JoinList[0])
            return &pSEC.BaseContext->_URFParser;
        if (pField.getEntity()==_JoinList[1])
            return &pSEC.SlaveContext->_URFParser;
    }
    return &pSEC.BaseContext->_URFParser;

}
URFParser* ZSearchEntity::getParserFromEntity(ZSearchEntityContext& pSEC,std::shared_ptr<ZSearchEntity> pEntity )
{
    if (pSEC.BaseContext==nullptr) {
        _DBGPRINT("ZSearchEntity::getParserFromField Entity context has no base context defined or is corrupted.");
        pSEC.Status = ZS_NULLPTR;
        abort();
    }
    if (isJoin()) {
        if (pSEC.SlaveContext==nullptr) {
            _DBGPRINT("ZSearchEntity::getParserFromField Entity context (join entity) has no slave context defined or is corrupted.");
            pSEC.Status = ZS_NULLPTR;
            abort();
        }
        if (pEntity==_JoinList[0])
            return &pSEC.BaseContext->_URFParser;
        if (pEntity==_JoinList[1])
            return &pSEC.SlaveContext->_URFParser;
    }
    return &pSEC.BaseContext->_URFParser;

}
#endif // __COMMENT__
//ZStatus ZSearchEntity::constructURF(URFParser &pInParser)
/**
 * @brief ZSearchEntity::constructURF constructs URFParser for current entity using either _BaseEntity or _FileEntity URFParser.
 *                                    at the end, URFParser has an appropriate record and an updated URF field list.
 * @return
 */
ZStatus ZSearchEntity::constructURF(ZSearchEntityContext& pSEC)
{
    if (pSEC.BaseContext == nullptr) {
        ErrorLog->errorLog("ZSearchEntity::constructURF Entity <%s> Base master context is not initialized or corrupted.",
                           getEntityName().toString());
        _DBGPRINT("ZSearchEntity::constructURF Entity <%s> Base master context is not initialized or corrupted.\n",
                  getEntityName().toString());
        abort();
    }
    if (pSEC.BaseContext->_URFParser.URFPS != URFPS_TotallyParsed) {
        ErrorLog->errorLog("ZSearchEntity::constructURF Entity <%s> Base master URF parser state is invalid <%s> (Not totally parsed).",
                           getEntityName().toString(),decode_URFPS(pSEC.BaseContext->_URFParser.URFPS));
        _DBGPRINT("ZSearchEntity::constructURF Entity <%s>  Base master URF parser state is invalid <%s> (Not totally parsed).\n",
                  getEntityName().toString(),decode_URFPS(pSEC.BaseContext->_URFParser.URFPS));
        exit(EXIT_FAILURE);
    }
    if (pSEC.SlaveContext != nullptr) {
        if (pSEC.SlaveContext->_URFParser.URFPS != URFPS_TotallyParsed) {
            ErrorLog->errorLog("ZSearchEntity::constructURF Entity <%s> Base slave URF parser state is invalid <%s> (Not totally parsed).",
                               getEntityName().toString(),decode_URFPS(pSEC.SlaveContext->_URFParser.URFPS));
            _DBGPRINT("ZSearchEntity::constructURF Entity <%s>  Base slave URF parser state is invalid <%s> (Not totally parsed).",
                      getEntityName().toString(),decode_URFPS(pSEC.SlaveContext->_URFParser.URFPS));
            exit(EXIT_FAILURE);
        }
    }

    ZBitset         wPresence;
    ZDataBuffer     wURFContent;
    ZOperandContent wContent;

    pSEC.Status=ZS_SUCCESS;

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
#ifdef __DEPRECATED__
            /* URF parser is located at field level (see ZSearchFiedlOperand::Entity). It must be fully updated. */
            wSt = BuildDic[wi].getURFParser()->_getURFFieldByRank(BuildDic[wi].getMetaDicRank(),wURFField) ;
#endif
            URFParser* wParser = pSEC.getBaseParserFromField(BuildDic[wi]);
            if (wParser==nullptr) {
                _DBGPRINT("ZSearchEntity::constructURF Entity <%s> cannot get URF parser from base entity.\n",
                          getEntityName().toString())
                abort();
            }
            pSEC.Status = wParser->_getURFFieldByRank(BuildDic[wi].getMetaDicRank(),wURFField) ;
            if (pSEC.Status!=ZS_SUCCESS)
                return pSEC.Status;

            if (wURFField.Present) {
                wPresence.set(wi);

                if (wSFO->ModifierType==ZSRCH_NOTHING) { /* field has no modifier  */
                    /* move it as is to out record */
                    wURFContent.appendData(wURFField.FieldPtr,
                                           wURFField.Size);
                }
                else
                {
                    pSEC.Status = getURFOperandContentWModifier (pSEC,wURFField,wSFO->ModifierType,wSFO->ModParams,&wContent);
                    if (pSEC.Status!=ZS_SUCCESS)
                        return pSEC.Status ;
                    /* convert resulting content to URF format */
                    pSEC.Status = wContent.appendURF(wURFContent);
                    if (pSEC.Status!=ZS_SUCCESS)
                        return pSEC.Status;
                }
            } // present

            break;
        } // ZSTO_Field
        case ZSTO_Literal:
        {
            ZSearchLiteral* wSOB=static_cast<ZSearchLiteral*>(BuildDic[wi].Operand);
            pSEC.Status = wSOB->appendURF(wURFContent);
            if (pSEC.Status!=ZS_SUCCESS)
                return pSEC.Status;
            wPresence.set(wi);
            break;
        } // ZSTO_Literal

        case ZSTO_Logical:
        {
            ZSearchLogicalOperand* wSLOP=static_cast<ZSearchLogicalOperand*>(BuildDic[wi].Operand);
            URFParser* wParser = pSEC.getBaseParserFromField(BuildDic[wi]);
            if (wParser==nullptr) {
                abort();
            }
            bool wBool=wSLOP->evaluate(pSEC);

            size_t wL=getAtomicURFSize<bool>(wBool);
            unsigned char* wPtr=wURFContent.extend(wL);
            exportAtomicURF_Ptr<bool>(wBool,wPtr);

            wPresence.set(wi);
            break;
        }// ZSTO_Logical

        case ZSTO_Arithmetic:
        {
            URFParser* wParser = pSEC.getBaseParserFromField(BuildDic[wi]);

            if (wParser==nullptr) {
                abort();
            }
            ZSearchArithmeticTerm* wSLOP=static_cast<ZSearchArithmeticTerm*>(BuildDic[wi].Operand);
            pSEC.Status=computeArithmetic(pSEC,wSLOP,wZSTOMain,wContent);
            if (pSEC.Status!=ZS_SUCCESS)
                return pSEC.Status;
            pSEC.Status = wContent.appendURF(wURFContent);
            if (pSEC.Status!=ZS_SUCCESS)
                return pSEC.Status;
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

    pSEC._URFParser.Record.clear();

    wPresence._exportURF (pSEC._URFParser.Record);
    pSEC._URFParser.Record.appendData(wURFContent);
    pSEC._URFParser.URFPS = URFPS_RecordSet ;

    pSEC._URFParser._setupRecord();
    return pSEC.Status=pSEC._URFParser.parse(ErrorLog);
} // ZSearchEntity::constructURF


/* has to be modifier to use indexes */
ZStatus
ZSearchEntity::findFirstFieldValueSequential(ZSearchEntityContext& pSEC,long pFieldRank,ZOperandContent* pSearchValue,ZDataBuffer& pOutRecord)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    pSEC.Status=ZS_SUCCESS;
    zaddress_type wAddress=0;
    ZOperandContent wOpContent;
    ZSearchTokentype_type wZSTO = pSearchValue->OperandZSTO;
    pSEC.Status=getFirst(pSEC,wAddress);
    while (pSEC.Status==ZS_SUCCESS)
    {
        if ((pFieldRank < pSEC._URFParser.URFFieldList.count()) && (pSEC._URFParser.URFFieldList[pFieldRank].Present)) {
            pSEC.Status=getURFOperandContentWModifier(pSEC,pSEC._URFParser.URFFieldList[pFieldRank],ZSRCH_NOTHING,nullptr,&wOpContent);
            if (pSEC.Status==ZS_SUCCESS) {
                if (pSearchValue->isSame( &wOpContent ) ) {
                    pSEC.Status = ZS_FOUND;
                    goto findFirstFieldValueSequentialEnd;
                }
            }
        }
        pSEC.Status=getNext(pSEC,wAddress);
    }
    pSEC.Status = ZS_NOTFOUND;
findFirstFieldValueSequentialEnd:
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status;
}

ZStatus ZSearchEntity::getFirst(ZSearchEntityContext &pSEC, zaddress_type &pAddress)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock : no status returned */
    pSEC.Status=_getFirst(pSEC,pAddress);
    _Mutex.unlock();
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status;
} //ZSearchEntity::getFirst

ZStatus ZSearchEntity::_getFirst(ZSearchEntityContext &pSEC, zaddress_type &pAddress)
{
    if (AddressList.count()==0) {
        pSEC.Status=_populateFirst(pSEC,1);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
    }
    if (_FileEntity!=nullptr) {
        pSEC.Status=_FileEntity->getByAddress(*pSEC.BaseContext,AddressList[0]);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
        pAddress = AddressList[0];
        pSEC.Status=constructURF(pSEC);
        if (pSEC.Status==ZS_SUCCESS) {
            pSEC.CurrentRank = CurrentRank = 0 ;
            pSEC.LastAddress = LastAddress = AddressList[0];
        }
        return pSEC.Status;
    } // file

    pSEC.Status=_BaseEntity->_getByAddressRaw(*pSEC.BaseContext,AddressList[0]); /* use URFParser feeding while accessing base entity */
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;

    pSEC.CurrentRank = CurrentRank = 0 ;
    pSEC.LastAddress = LastAddress = AddressList[0];

    pAddress = AddressList[0];

    pSEC.Status=constructURF(pSEC);
 /*   if (pSEC.Status==ZS_SUCCESS) {
        pSEC.CurrentRank = CurrentRank = 0 ;
        pSEC.LastAddress = LastAddress = AddressList[0];
    }
*/
    return pSEC.Status;
} //ZSearchEntity::_getFirst


ZStatus ZSearchEntity::_getFirstHard(ZSearchEntityContext &pSEC, zaddress_type &pAddress)
{
    bool wResult=false;
    long wRank=0;

    if (_FileEntity!=nullptr) {
        pSEC.Status=_FileEntity->getFirst(*pSEC.BaseContext,pAddress);
        if (pSEC.Status!=ZS_SUCCESS) {
            pSEC.LastAddress = LastAddress = 0;
            pSEC.CurrentRank = CurrentRank = 0;
            return pSEC.Status;
        }

        pSEC.Status = evaluateRecord(pSEC,wResult);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;

        while ((!wResult)&&(pSEC.Status==ZS_SUCCESS)) {
            pSEC.Status=_FileEntity->getNext(*pSEC.BaseContext,pAddress);
            if (pSEC.Status!=ZS_SUCCESS)
                return pSEC.Status;
            pSEC.Status = evaluateRecord(pSEC,wResult);
        }
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
        pSEC.Status=constructURF(pSEC);
        if (pSEC.Status==ZS_SUCCESS) {
            pSEC.CurrentRank = CurrentRank = 0 ;
            pSEC.LastAddress = LastAddress = pAddress;
        }
        return pSEC.Status;
    } // file

    pSEC.Status=_BaseEntity->getFirst(*pSEC.BaseContext,pAddress); /* use URFParser feeding while accessing base entity */
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;

    pSEC.Status = evaluateRecord(pSEC,wResult);
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;

    while ((!wResult)&&(pSEC.Status==ZS_SUCCESS)) {
        pSEC.Status=_BaseEntity->getNext(*pSEC.BaseContext,pAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;

        pSEC.Status = evaluateRecord(pSEC,wResult);
    }
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;
    /* construct entity record from base raw record using BuildDic */

    /* ZSearchEntityContext is given at entity level (not base) */
    pSEC.Status=constructURF(pSEC);
    if (pSEC.Status==ZS_SUCCESS) {
        pSEC.CurrentRank = CurrentRank = 0;
        pSEC.LastAddress = LastAddress = pAddress;
    }
    return pSEC.Status;
} //ZSearchEntity::_getFirstHard


ZStatus ZSearchEntity::getNext(ZSearchEntityContext &pSEC,zaddress_type &pAddress)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock  : no status returned */

//    pSEC.Status=_getByRank(pSEC,pSEC.CurrentRank+1,pAddress);
    pSEC.Status=_getNext(pSEC,pAddress);
    _Mutex.unlock();
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status;
} //ZSearchEntity::getNext

/* get rank next to pSEC.CurrentRank */
ZStatus ZSearchEntity::_getNext(ZSearchEntityContext &pSEC,zaddress_type &pAddress)
{
bool wResult=false;
long wRank=0;
    /* test if base entity is positionned to pSEC.CurrentRank */
    if (pSEC.CurrentRank < 0) {
        return _getFirst(pSEC,pAddress);
    }

    wRank=pSEC.CurrentRank+1;

    if (CurrentRank != pSEC.CurrentRank) {  
        return _getByRank(pSEC,wRank,pAddress); /* currentrank and lastaddress are updated within entity and Context */
    }

    pSEC.Status=ZS_SUCCESS;
    if (_FileEntity!=nullptr) {
        while ((!wResult)&&(pSEC.Status==ZS_SUCCESS)) {
            pSEC.Status=_FileEntity->getNext(*pSEC.BaseContext,pAddress);
            if (pSEC.Status!=ZS_SUCCESS)
                return pSEC.Status;
            pSEC.Status = evaluateRecord(pSEC,wResult);
        }
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
        pSEC.CurrentRank = ++CurrentRank  ;
        pSEC.LastAddress = LastAddress=pAddress;
        return pSEC.Status = constructURF(pSEC);
    } // file


    while ((!wResult)&&(pSEC.Status==ZS_SUCCESS)) {
        pSEC.Status =_BaseEntity->getNext(*pSEC.BaseContext,pAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
        pSEC.Status = evaluateRecord(pSEC,wResult);
    }
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;
    pSEC.CurrentRank=++CurrentRank;
    pSEC.LastAddress = LastAddress=pAddress;
    return constructURF(pSEC);

} //ZSearchEntity::_getNext

ZStatus ZSearchEntity::_getNextHard(ZSearchEntityContext &pSEC,zaddress_type &pAddress)
{
    bool wResult=false;

    pSEC.Status=ZS_SUCCESS;
    if (_FileEntity!=nullptr) {
        while ((!wResult)&&(pSEC.Status==ZS_SUCCESS)) {
            pSEC.Status=_FileEntity->getNext(*pSEC.BaseContext,pAddress);
            if (pSEC.Status!=ZS_SUCCESS)
                return pSEC.Status;
            pSEC.Status = evaluateRecord(pSEC,wResult);
        }
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;

        pSEC.CurrentRank = ++CurrentRank ;
        pSEC.LastAddress = LastAddress=pAddress;
        pSEC.Status = constructURF(pSEC);
//        if (pSEC.Status!=ZS_SUCCESS)
//            return pSEC.Status ;
        return pSEC.Status ;
    } // file


    while ((!wResult)&&(pSEC.Status==ZS_SUCCESS)) {
        int wRet=_BaseEntity->_Mutex.lock();
        if (wRet) {
            if (wRet==EPERM)
                return pSEC.Status=ZS_LOCKED;
            utf8VaryingString wEr=decode_MutexError("ZSearchEntity::populateFirst",wRet);
            ErrorLog->errorLog("Error on entity <%s> mutex lock %X %s",
                               _BaseEntity->getEntityName(),
                               wRet,wEr.toString());
            return pSEC.Status=ZS_LOCKERROR;
        }
        pSEC.Status =_BaseEntity->_getNextHard(*pSEC.BaseContext,pAddress);
        _BaseEntity->_Mutex.unlock();
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
        pSEC.Status = evaluateRecord(pSEC,wResult);
    }
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;

    pSEC.CurrentRank = ++CurrentRank ;
    pSEC.LastAddress = LastAddress=pAddress;

    pSEC.Status = constructURF(pSEC);
    return pSEC.Status ;

} //ZSearchEntity::_getNextHard

ZStatus ZSearchEntity::populateFirst(ZSearchEntityContext &pSEC, long pCount)
{
    if (pCount <= 0)
        return populateAll(pSEC);

    int wRet=_Mutex.lock();
    if (wRet) {
        if (wRet==EPERM)
            return pSEC.Status=ZS_LOCKED;
        utf8VaryingString wEr=decode_MutexError("ZSearchEntity::populateFirst",wRet);
        ErrorLog->errorLog("Error on entity <%s> mutex lock %X %s",
                           getEntityName(),
                           wRet,wEr.toString());
        return pSEC.Status=ZS_LOCKERROR;
    }

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();

    pSEC.Status = _populateFirst (pSEC,pCount);

    _Mutex.unlock();

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status;
} // ZSearchEntity::populateFirst

ZStatus ZSearchEntity::_populateFirst(ZSearchEntityContext &pSEC, long pCount)
{
    if (pCount <= 0)
        return populateAll(pSEC);

    pSEC.Status=ZS_SUCCESS;
    zaddress_type wAddress;
    AddressList.clear();

    pSEC.Status=_getFirstHard(pSEC,wAddress);
    if (pSEC.Status!=ZS_SUCCESS)
        goto _populateFirstEnd;
    if (AddressList.count()) {
        _DBGPRINT("ZSearchEntity::_populateFirst Warning AddressList is already populated while populating first record.\n"
                  "                              Replacing first address in AddressList.\n")
        AddressList[0] = wAddress;
    }
    else
        AddressList.push(wAddress);

    pSEC.CurrentRank = CurrentRank = 0;
    pSEC.LastAddress = LastAddress = wAddress;

    pCount--;
    while ((pSEC.Status==ZS_SUCCESS)&&(pCount>0)) {

        pSEC.Status=_getNextHard(pSEC,wAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            goto _populateFirstEnd;
        AddressList.push(wAddress);
        pSEC.CurrentRank = ++CurrentRank ;
        pSEC.LastAddress = LastAddress = wAddress;
        pCount--;
    }
_populateFirstEnd:
    if (pSEC.Status==ZS_EOF)
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    return pSEC.Status;
} // ZSearchEntity::_populateFirst

ZStatus ZSearchEntity::populateNext(ZSearchEntityContext &pSEC, long pCount)
{
    int wRet = _Mutex.lock();
    if (wRet) {
        if (wRet==EPERM)
            return ZS_LOCKED;
        utf8VaryingString wEr=decode_MutexError("ZSearchEntity::populateFirst",wRet);
        ErrorLog->errorLog("Error on entity mutex lock %X %s",wRet,wEr.toString());
        return ZS_LOCKERROR;
    }
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();

    pSEC.Status=_populateNext(pSEC , pCount);

    if (pSEC.Status==ZS_EOF)
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status;
} // ZSearchEntity::populateNext

ZStatus ZSearchEntity::_populateNext(ZSearchEntityContext &pSEC, long pCount)
{
    pSEC.Status=ZS_SUCCESS;
    zaddress_type wAddress;

    /* position to last selected address for entity */
    if (AddressList.count()==0) {
        pSEC.Status = _getFirstHard(pSEC,wAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            goto _populateNextEnd;
        AddressList.push(wAddress);
        pSEC.CurrentRank = CurrentRank = 0;
        pSEC.LastAddress = LastAddress = wAddress;
        pCount--;
    }
    else {
        pSEC.Status = _getByAddressRaw(pSEC,AddressList.last());
        pSEC.CurrentRank = CurrentRank = AddressList.count()-1;
        pSEC.LastAddress = LastAddress = AddressList.last();
    }

    if (pCount > 0) {
        while ((pSEC.Status==ZS_SUCCESS)&&(pCount > 0)) {
            pSEC.Status=_getNextHard(pSEC,wAddress);
            if (pSEC.Status!=ZS_SUCCESS)
                goto _populateNextEnd;
            AddressList.push(wAddress);
            pSEC.CurrentRank = ++CurrentRank ;
            pSEC.LastAddress = LastAddress = wAddress;
            pCount--;
        }
        goto _populateNextEnd ;
    } // if (pCount > 0

    /* if pCount is equal or less than 0, then AddressList is populated til end of available records */
    while (pSEC.Status==ZS_SUCCESS) {
        pSEC.Status=_getNextHard(pSEC,wAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            goto _populateNextEnd ;
        pSEC.CurrentRank = ++CurrentRank ;
        pSEC.LastAddress = LastAddress = wAddress;
        AddressList.push(wAddress);
    }
_populateNextEnd:
    if (pSEC.isEOF())
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    return pSEC.Status;
} // ZSearchEntity::_populateNext

ZStatus ZSearchEntity::populateAll(ZSearchEntityContext &pSEC)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    if (FetchState == EFST_Total) {
        if (pSEC.CaptureTime)
            pSEC.ProcessTi.end();
        return pSEC.Status=ZS_SUCCESS;
    }
    _Mutex.lock();
    pSEC.Status=ZS_SUCCESS;
    zaddress_type wAddress;
    AddressList.clear();

    /* check if base entity has been populated : if not Fetch it all
     * Nb a _FileEntity is always 100% populated
     */

    if ((isCollection()) && (_BaseEntity->FetchState!=EFST_Total)) {
        pSEC.Status=_BaseEntity->populateAll(*pSEC.BaseContext);
        if (pSEC.Status!=ZS_SUCCESS)
            goto populateAllEnd;
    }


    pSEC.Status=_getFirstHard(pSEC,wAddress);
    if (pSEC.Status!=ZS_SUCCESS)
        goto populateAllEnd;
    AddressList.push(wAddress);
    while (pSEC.Status==ZS_SUCCESS) {
        pSEC.Status=_getNextHard(pSEC,wAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            goto populateAllEnd;
        AddressList.push(wAddress);
    }
populateAllEnd:
    if ((pSEC.Status==ZS_EOF)||(pSEC.Status==ZS_OUTBOUNDHIGH)||(pSEC.Status==ZS_SUCCESS))
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
     _Mutex.unlock();

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    if ((pSEC.Status==ZS_EOF)||(pSEC.Status==ZS_OUTBOUNDHIGH))
         pSEC.Status=ZS_EOF;
    return pSEC.Status;
} // ZSearchEntity::populateAll


ZStatus ZSearchEntity::getByRank(ZSearchEntityContext &pSEC,long pRank,zaddress_type &pAddress)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock  : no status returned */
    pSEC.Status=_getByRank(pSEC,pRank,pAddress);
    _Mutex.unlock();
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status;
} //ZSearchEntity::getByRank

ZStatus
ZSearchEntity::_getByRank(ZSearchEntityContext &pSEC,long pRank,zaddress_type &pAddress)
{
    pSEC.Status=ZS_SUCCESS;
    if (pRank >= AddressList.count()) {
        pSEC.Status=_populateNext(pSEC,pRank+1-AddressList.count());
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
    }

    pAddress = AddressList[pRank];
    pSEC.Status = _getByAddressRaw(pSEC,AddressList[pRank]);
    if (pSEC.Status==ZS_SUCCESS) {
        pSEC.CurrentRank = CurrentRank = pRank;
        pSEC.LastAddress = LastAddress = AddressList[pRank];
    }
    return pSEC.Status;
} // ZSearchEntity::getByRank

/* record address is reputated to be valided by selection clause.
 * Neither local Entity Counters CurrentRank, LastAddress nor Entity Context counters are updated */
ZStatus
ZSearchEntity::_getByAddressRaw(ZSearchEntityContext &pSEC,zaddress_type pAddress)
{
    pSEC.Status=ZS_SUCCESS;

    if (_FileEntity!=nullptr) {
        pSEC.Status = _FileEntity->getByAddress(pSEC,pAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
        return constructURF(pSEC);
    }

    pSEC.Status =_BaseEntity->_getByAddressRaw(pSEC,pAddress);
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;

    return constructURF(pSEC);

} // ZSearchEntity::_getByAddress

/* record is evaluated using base dictionary and not entity dictionary :
 * selection may occur on fields wich are not selected for entity
 */

/* pSEC is the top entity instance of ZSearchEntityContext : URFParser will be chosen in base contextes according field */
ZStatus
ZSearchEntity::evaluateRecord(ZSearchEntityContext& pSEC, bool &pOutResult)
{
    if (LogicalTerm==nullptr) {
        pOutResult=true;
        return ZS_SUCCESS ;
    }

    return LogicalTerm->evaluate(pSEC, pOutResult);
}

ZStatus ZSearchEntity::populateAllJoin(ZSearchEntityContext &pSEC)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    if (FetchState == EFST_Total) {
        if (pSEC.CaptureTime)
            pSEC.ProcessTi.end();
        return pSEC.Status=ZS_SUCCESS;
    }
    _Mutex.lock();
    pSEC.Status=ZS_SUCCESS;
    bool wResult=false;

    zaddress_type wMAddress,wSAddress;
    JoinAddressList.clear();
    AddressList.clear();

    ZSearchJoinAddress wAddress;

 //   _JoinList[0]->CaptureTime=_JoinList[1]->CaptureTime=false;

    pSEC.Status=_getFirstJoin(pSEC,wAddress);

    while (pSEC.Status==ZS_SUCCESS) {
        JoinAddressList.push(wAddress);
        pSEC.Status=_getNextJoin(pSEC,wAddress);
    }// while


populateAllJoinEnd:
    if ((pSEC.Status==ZS_OUTBOUNDHIGH)||(pSEC.Status==ZS_SUCCESS))
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    if ((pSEC.Status==ZS_EOF)||(pSEC.Status==ZS_OUTBOUNDHIGH))
        pSEC.Status=ZS_EOF;
    return pSEC.Status;
} // ZSearchEntity::populateAllJoin

ZStatus
ZSearchEntity::populateFirstJoin(ZSearchEntityContext &pSEC,int pCount)
{

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    if (FetchState == EFST_Total) {
        if (pSEC.CaptureTime)
            pSEC.ProcessTi.end();
        return ZS_SUCCESS;
    }
    pSEC.BaseContext->CaptureTime = pSEC.SlaveContext->CaptureTime = false ;
    _Mutex.lock();

    pSEC.Status=_populateFirstJoin(pSEC,pCount);

    _Mutex.unlock();

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    if ((pSEC.Status==ZS_EOF)||(pSEC.Status==ZS_OUTBOUNDHIGH))
        pSEC.Status=ZS_EOF;
    return pSEC.Status;
} // ZSearchEntity::populateFirstJoin

ZStatus
ZSearchEntity::_populateFirstJoin(ZSearchEntityContext &pSEC,int pCount)
{
    pSEC.Status=ZS_SUCCESS;
    bool wResult=false;

    JoinAddressList.clear();
    AddressList.clear();

    ZSearchJoinAddress wAddress;

    pSEC.Status=_getFirstJoin(pSEC,wAddress);

    while ((pSEC.Status==ZS_SUCCESS) && --pCount) {
        JoinAddressList.push(wAddress);
        pSEC.Status=_getNextJoin(pSEC,wAddress);
    }// while

_populateFirstJoinEnd:
    if ((pSEC.Status==ZS_OUTBOUNDHIGH)||(pSEC.Status==ZS_SUCCESS))
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    if ((pSEC.Status==ZS_EOF)||(pSEC.Status==ZS_OUTBOUNDHIGH))
        pSEC.Status=ZS_EOF;
    return pSEC.Status;
} // ZSearchEntity::populateFirstJoin




ZStatus ZSearchEntity::populateNextJoin(ZSearchEntityContext &pSEC, int pCount)
{
    pSEC.Status=ZS_SUCCESS;
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    if (FetchState == EFST_Total) {
        if (pSEC.CaptureTime)
            pSEC.ProcessTi.end();
        return ZS_SUCCESS;
    }
    _Mutex.lock();

    bool wResult=false;

    ZSearchJoinAddress wAddress;

//    _JoinList[0]->CaptureTime=_JoinList[1]->CaptureTime=false;
    pSEC.BaseContext->CaptureTime = pSEC.SlaveContext->CaptureTime = false ;

    pSEC.Status=_getNextJoin(pSEC,wAddress);

    while ((pSEC.Status==ZS_SUCCESS) && --pCount) {
        JoinAddressList.push(wAddress);
        pSEC.Status=_getNextJoin(pSEC,wAddress);
    }// while

populateFirstJoinEnd:
    if ((pSEC.Status==ZS_OUTBOUNDHIGH)||(pSEC.Status==ZS_SUCCESS))
        FetchState = EFST_Total;
    else
        FetchState = EFST_Partial;
    _Mutex.unlock();

    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    if ((pSEC.Status==ZS_EOF)||(pSEC.Status==ZS_OUTBOUNDHIGH))
        pSEC.Status=ZS_EOF;
    return pSEC.Status;
} // ZSearchEntity::populateNextJoin

ZStatus ZSearchEntity::getFirstJoin(ZSearchEntityContext &pSEC, ZSearchJoinAddress &pAddress)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock : no status returned */
    pSEC.Status=_getFirstJoin(pSEC,pAddress);
    _Mutex.unlock();
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status;
} //ZSearchEntity::getFirstJoin


ZStatus ZSearchEntity::_getFirstJoin(ZSearchEntityContext &pSEC, ZSearchJoinAddress &pAddress)
{
    bool wResult=false;

    ZStatus wSt=ZS_SUCCESS;

    pAddress.setInvalid();
    bool wHasAtLeastOne=false;
//    _JoinList[0]->CaptureTime=_JoinList[1]->CaptureTime=false;
    pSEC.BaseContext->CaptureTime =pSEC.SlaveContext->CaptureTime = false;

    /* NB: selection clause (WITH) for each entity member of join are evaluated within base / slave entity */
    pSEC.Status=_JoinList[0]->_getFirst(*pSEC.BaseContext,pAddress.MasterAddress);
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;

    while ((pSEC.Status==ZS_SUCCESS) && !wResult) {
        pSEC.Status=_JoinList[1]->_getFirst(*pSEC.SlaveContext,pAddress.SlaveAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
        while ((pSEC.Status==ZS_SUCCESS) && !wResult) {
            pSEC.Status = _Using->evaluate(pSEC,wResult);
            if (wResult) {
                wHasAtLeastOne=true;
                pSEC.Status = constructURF(pSEC);
                pSEC.CurrentRank = CurrentRank = 0;
                pSEC.LastAddressJoin = LastAddressJoin = pAddress ;
                return pSEC.Status;
            }
            /* NB: selection clause (WITH) for each entity member of join are evaluated within base / slave entity */
            pSEC.Status=_JoinList[1]->_getNext(*pSEC.SlaveContext,pAddress.SlaveAddress);
            if (pSEC.Status!=ZS_SUCCESS) {
                if (pSEC.isEOF())
                    break;
                return pSEC.Status;
            }
        } // while slave
        pSEC.Status=_JoinList[0]->_getNext(*pSEC.BaseContext,pAddress.MasterAddress);
        if (pSEC.Status!=ZS_SUCCESS) {
            if (pSEC.isEOF())
                break;
            return pSEC.Status;
        }
    }//while  master

    return pSEC.Status;

} //ZSearchEntity::_getFirstJoin

ZStatus ZSearchEntity::getNextJoin(ZSearchEntityContext &pSEC, ZSearchJoinAddress &pAddress)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock : no status returned */
    pSEC.Status=_getNextJoin(pSEC,pAddress);
    _Mutex.unlock();
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status;
} //ZSearchEntity::getFirstJoin

ZStatus ZSearchEntity::_getNextJoin(ZSearchEntityContext &pSEC, ZSearchJoinAddress &pAddress)
{
    bool wResult=false;

    pSEC.Status=ZS_SUCCESS;
    pAddress.setInvalid();

    /*
    get a next rank from slave entity that matches USING clause against current master entity record

    if no match on USING clause,
        - get next rank from master entity
        - rewind to first record of slave entity and try to get a match for USING clause
        - while not found slave entity
                get next until match USING clause or eof for slave entity
                if match
                    construct record for join entity with fields from both master and slave entities
        if eof for slave entity,
            get next rank from master entity and repeat

    */

    /* NB: selection clause (WITH) for each entity member of join are evaluated within base / slave entity */
    pSEC.Status =_JoinList[1]->_getNext(*pSEC.SlaveContext,pAddress.SlaveAddress);
    if (pSEC.Status!=ZS_SUCCESS) {
        if (!pSEC.isEOF())
            return pSEC.Status;
        /* slave ranks are exhausted, then jump to next master rank and set slave to first rank */
        pSEC.Status =_JoinList[0]->_getNext(*pSEC.BaseContext,pAddress.MasterAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;

        pSEC.Status =_JoinList[1]->_getFirst(*pSEC.SlaveContext,pAddress.SlaveAddress);
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
    } // not success

    if (_Using == nullptr) {  /* if no <USING> clause then a cartesian product is made */
        pSEC.CurrentRank = ++CurrentRank;
        pSEC.LastAddressJoin = LastAddressJoin = pAddress ;
        return  constructURF(pSEC);
    }

    while (!wResult && (pSEC.Status==ZS_SUCCESS)) {
        while ((!wResult)&&(pSEC.Status==ZS_SUCCESS)) {

            pSEC.Status=_Using->evaluate(pSEC,wResult) ;
            if (pSEC.Status!=ZS_SUCCESS)
                return pSEC.Status;
            if (wResult) {
                pSEC.CurrentRank = ++CurrentRank;
                pSEC.LastAddressJoin = LastAddressJoin = pAddress ;
                return  constructURF(pSEC);
            }
            pSEC.Status=_JoinList[1]->_getNext(pSEC,pAddress.SlaveAddress);
        }// while

        if (!pSEC.isEOF())
            return pSEC.Status;

        pSEC.Status=_JoinList[0]->_getNext(pSEC,pAddress.MasterAddress);
        if (pSEC.Status==ZS_SUCCESS) {
            pSEC.Status=_JoinList[1]->_getFirst(pSEC,pAddress.SlaveAddress);
        }
    }// while
    return pSEC.Status;

} //ZSearchEntity::_getNextJoin

ZStatus ZSearchEntity::_getByAddressJoin(ZSearchEntityContext &pSEC, ZSearchJoinAddress &pAddress)
{
    pSEC.Status=ZS_SUCCESS;
    pAddress.setInvalid();

    pSEC.Status=_JoinList[0]->_getByAddressRaw(pSEC,pAddress.SlaveAddress);
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;
    pSEC.Status=_JoinList[1]->_getByAddressRaw(pSEC,pAddress.SlaveAddress);
    if (pSEC.Status!=ZS_SUCCESS)
        return pSEC.Status;
    return constructURF(pSEC);

} //ZSearchEntity::_getByAddressJoin


ZStatus ZSearchEntity::getByRankJoin(ZSearchEntityContext &pSEC,
                                     long pRank,
                                     ZSearchJoinAddress &pAddress)
{
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.start();
    _Mutex.lockBlock(); /* blocking lock : no status returned */
    pSEC.Status=_getByRankJoin(pSEC,pRank,pAddress);
    _Mutex.unlock();
    if (pSEC.CaptureTime)
        pSEC.ProcessTi.end();
    return pSEC.Status ;
} //ZSearchEntity::getByRankJoin

ZStatus ZSearchEntity::_getByRankJoin(ZSearchEntityContext &pSEC,
                                      long pRank,
                                      ZSearchJoinAddress &pAddress)
{
    pSEC.Status=ZS_SUCCESS;
    if (pRank > JoinAddressList.count()) {
        pSEC.Status=populateNextJoin(pSEC,pRank+1-JoinAddressList.count());
        if (pSEC.Status!=ZS_SUCCESS)
            return pSEC.Status;
    }

    pSEC.Status= _getByAddressJoin(pSEC,JoinAddressList[pRank]);
    if (pSEC.Status==ZS_SUCCESS) {
        pSEC.CurrentRank = CurrentRank = pRank ;
        pSEC.LastAddressJoin = LastAddressJoin =  JoinAddressList[pRank];
    }
    return pSEC.Status;
} //ZSearchEntity::_getByRankJoin

}//namespace zbs
