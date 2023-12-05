#ifndef ZSEARCHDICTIONARY_H
#define ZSEARCHDICTIONARY_H

#include <ztoolset/zstatus.h>

#include "zsearchfield.h"

class ZSearchDictionary : public ZArray <ZSearchField>
{
public:
    ZSearchDictionary();

    void addMetaDic(const ZMetaDic* pMetaDic)
    {
        BaseMetaDic=pMetaDic;
    }

//    ZStatus addByRank(long pFieldRank);
//    void addAll(const ZMetaDic* pMetaDic);   // Meta dictionary must be defined when constructing ZSearchEntity


    long getFieldRankByName(const utf8VaryingString &pFieldName) const ;

 /* Deprecated
     void setEqualTo(std::shared_ptr<ZSearchEntity> pSourceEntity, const ZMetaDic *pMetaDic);
 */
    void setEqualTo(std::shared_ptr<ZSearchEntity> pSourceEntity);
/*
    long getDicRank(long pRank) {return Tab(pRank).dicRank();}
*/
    const ZMetaDic* getBaseMetaDic() {return BaseMetaDic;}

    ZCFMT_Type getGlobalCellFormat() {return CellFormat;}
    void setGlobalCellFormat(ZCFMT_Type pCellFormat) {CellFormat=pCellFormat;}

    ZCFMT_Type      CellFormat=ZCFMT_Nothing;   /* generic cell display format for all entity fields */
    const ZMetaDic* BaseMetaDic=nullptr;
    std::shared_ptr<ZSearchEntity>  Entity=nullptr;
};

#endif // ZSEARCHDICTIONARY_H
