#ifndef ZKEYDICTIONARY_H
#define ZKEYDICTIONARY_H

#include <ztoolset/zarray.h>

#include <zindexedfile/zsindextype.h>

#include <zindexedfile/zindexfield.h>


#ifndef __KEYDICTYPE__
#define __KEYDICTYPE__
enum ZKeyDic_type {
    ZKD_Nothing         = 0,
    ZKD_NaturalSize     = 1,
    ZKD_UniversalSize   = 2,
    ZKD_KeyOffset       = 4,
    ZKD_RecordOffset    = 8,
    ZKD_ArraySize       = 0x10,
    ZKD_ZType           = 0x20,
    ZKD_Name            = 0x40
};
#endif //__KEYDICTYPE__
/**
 * @brief The ZSKeyDictionary class store ZIndex index key fields definition for one key (and therefore one ZIndexFile)
 *
 * ZSKeyDictionary is only a set of pointers to meta dictionary rank.
 * However, as Fields may have varying length it is necessary to use not the meta dictionary itself but the record dictionary
 *
 *
 * Nota Bene: when exporting, only ZArray of fields definition are exported in universal format
 *            when importing, total Natural size and Universal size are computed.
 *
 */

//class ZSKeyDictionary : public zbs::ZArray<ZSIndexField_struct>
class ZSKeyDictionary : public ZArray<ZSIndexField>
{
typedef zbs::ZArray<ZSIndexField> _Base;

public:
  ZSKeyDictionary(ZMetaDic*pMDic) ;
  ~ZSKeyDictionary() { } // just to call the base destructor
    ZMetaDic* MetaDic=nullptr;  //!< Record Dictionary to which Key Dictionary refers : WARNING : not store in xml <keydictionary>
                                //!  it is stored apart in dedicated <metadic> xml node
    size_t KDicSize;            //!< Size of the dictionary when exported (other fields are not exported) this field comes first
    size_t KeyNaturalSize ;     //!< overall size of the key (sum of keyfields NATURAL (total) length).
    size_t KeyUniversalSize ;   //!< total of key size with internal format
    bool   Recomputed=false;    // WARNING: this field is not stored (xml) or exported (icb)
//    using _Base::newBlankElement;

    ssize_t fieldKeyOffset (const long pRank);      //< compute offset from beginning of key record for a given field rank with KDic
    ssize_t fieldRecordOffset (const long pRank);

    ZTypeBase getType(const long pKFieldRank)
        {
        long wRank =Tab[pKFieldRank].MDicRank;
        return (MetaDic->Tab[wRank].ZType);
        }
    uint64_t getUniversalSize(const long pKFieldRank)
        {
        long wRank =Tab[pKFieldRank].MDicRank;
        return (MetaDic->Tab[wRank].UniversalSize);
        }
    uint64_t getNaturalSize(const long pKFieldRank)
        {
        long wRank =Tab[pKFieldRank].MDicRank;
        return (MetaDic->Tab[wRank].NaturalSize);
        }


    ZStatus addFieldToZDicByName (const utf8_t *pFieldName);
    ZStatus addFieldToZDicByRank (const zrank_type pMDicRank);



    void print (FILE* pOutput=stdout);

    void _reComputeSize (void) ;

    ZStatus zremoveField (const long pKDicRank);


/*    template <class _Tp>
    ZStatus zaddField (const char *pFieldName,const size_t pZMFOffset);
    template <class _Tp>
    ZStatus zaddField (descString &pFieldName,const size_t pZMFOffset);
    ZStatus zremoveField (const char *pFieldName);
    ZStatus zremoveField (const long pFieldRank);
    ZStatus zsetField (const long pFieldRank,ZKeyDic_type pZKD,auto pValue);
*/
    long zsearchFieldByName(const utf8_t* pFieldName) const ;
    long zsearchFieldByName(const utf8String &pFieldName) const ;

    void clear (void) {KeyNaturalSize=0; KeyUniversalSize=0; _Base::clear(); return;}

    utf8String toXml(int pLevel);
    int fromXml(zxmlNode* pIndexRankNode,ZaiErrors* pErrorlog);

    ZDataBuffer& _export(ZDataBuffer& pZDBExport);
    size_t _import(unsigned char* pZDBImport_Ptr);

} ;


#endif // ZKEYDICTIONARY_H
