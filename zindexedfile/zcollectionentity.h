#ifndef ZCOLLECTIONENTITY_H
#define ZCOLLECTIONENTITY_H

#include <memory> // for shared pointers
#include <ztoolset/utfvaryingstring.h>

#include <zcontentcommon/urfparser.h>

#include "zsearchdictionary.h"

#include "zsearchcontext.h"

namespace zbs {

class ZSearchLogicalTerm;
class ZSearchEntity;
class ZSearchToken;

class ZCollectionEntity : public std::enable_shared_from_this <ZCollectionEntity>
{
public:
    //  ZSearchEntity() {}
    ZCollectionEntity( std::shared_ptr<ZSearchEntity>& pBaseEntity)
    {
        _BaseEntity=pBaseEntity;
        /* Do not set URFParser from base entity : it might be shared by many others
         * Set URFParser from father ZSearchEntity that instantiate collection  (see ZSearchEntity).
        */
    }

    ZCollectionEntity(const ZCollectionEntity& pIn) {_copyFrom(pIn);}

    ~ZCollectionEntity()
    {
        if (LogicalTerm != nullptr)
            delete LogicalTerm;
    }

    ZCollectionEntity& _copyFrom (const ZCollectionEntity& pIn) ;

    ZCollectionEntity& operator = (const ZCollectionEntity& pIn) {return _copyFrom(pIn);}

    bool isValid() {return (_BaseEntity != nullptr );}

    std::shared_ptr<ZCollectionEntity> getSharedPtr() {return shared_from_this();}
/*
    static std::shared_ptr<ZCollectionEntity> construct( std::shared_ptr<ZSearchEntity> pBaseEntity)
    {
        std::shared_ptr<ZCollectionEntity> wOut = std::make_shared<ZCollectionEntity>(pBaseEntity);
        return wOut;
    }
*/


    void setLogicalTerm(ZSearchLogicalTerm* pTerm);

    utf8VaryingString getEntityName( ) const ;


    /* returns AddressList count */
    size_t getCount() ;
    /* returns getCount from underneath entity (_BaseEntity) */
    size_t _baseGetCount();

    ZaiErrors* _getErrorLog() ;

    ZStatus executeFind ();

    ZStatus evaluateRecord(bool &pOutResult, const ZDataBuffer &pRecord);


   /* gets from immediate entity :
   *    if Entity is file entity, returns values from file at pRank
   *    if Entity is collection entity, returns values from AddressList at pRank
   */
    ZStatus get(ZDataBuffer& pRecord,const long pRank,zaddress_type& pAddress);

    void setURFParser(URFParser* pParser)       {_URFParser = pParser; }

    URFParser*                                 _URFParser=nullptr;  /* set by father ZSearchEntity who owns the parser object */


    std::shared_ptr <ZSearchEntity>           _BaseEntity = nullptr;
    ZArray<zaddress_type>                     AddressList;
    ZSearchLogicalTerm*                       LogicalTerm=nullptr;  /* entity selection phrase */
};

} //namespace zbs
#endif // ZCOLLECTIONENTITY_H
