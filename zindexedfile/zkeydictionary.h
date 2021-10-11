#ifndef ZKEYDICTIONARY_H
#define ZKEYDICTIONARY_H

#include <ztoolset/zarray.h>

#include <zindexedfile/zsindextype.h>
#include <zindexedfile/zindexfield.h>


#ifndef __KEYDICTYPE__
#define __KEYDIdCTYPE__
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

namespace zbs {

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

class ZMetaDic;
class ZSKeyDictionary : public ZArray<ZSIndexField>
{
typedef ZArray<ZSIndexField> _Base;

public:
  ZSKeyDictionary(ZMetaDic*pMDic) ;
  ZSKeyDictionary(ZSKeyDictionary* pIn);
  ZSKeyDictionary(ZSKeyDictionary& pIn);
  ~ZSKeyDictionary() { } // just to call the base destructor

  ZSKeyDictionary& _copyFrom( ZSKeyDictionary& pIn);

  bool hasSameContentAs(ZSKeyDictionary*pKey);

  utf8String  IndexName;        //!< refers to ZICB::IndexName
  ZMetaDic*   MetaDic=nullptr;  //!< Record Dictionary to which Key Dictionary refers : WARNING : not store in xml <keydictionary>
                                //!  it is stored apart in dedicated <metadic> xml node
//    uint32_t KDicSize;          //!< Size of the dictionary when exported (other fields are not exported) this field comes first
        // nb: KDicSize is already given by ZAExport structure.
//    uint32_t KeyNaturalSize ;     //!< overall size of the key (sum of keyfields NATURAL (total) length). not necessary
//    uint32_t KeyUniversalSize ;   //!< total of key size with internal format MOVED TO ZSICBOwnData
  // WARNING: fields below are not stored (xml) nor exported (icb)
    bool    Recomputed=false;
    uint8_t Status=0;             // for Xml matching and actions. refers to ZPRES values set.
    /** @brief computeKeyOffsets (re)compute key fields offset, field by field, and returns the key universal size
     */
    uint32_t computeKeyOffsets();
    uint32_t computeKeyUniversalSize();

    ZTypeBase getType(const long pKFieldRank);
    uint64_t getUniversalSize(const long pKFieldRank);
    uint64_t getNaturalSize(const long pKFieldRank);


    ZStatus addFieldToZDicByName (const utf8_t *pFieldName);
    ZStatus addFieldToZDicByRank (const zrank_type pMDicRank);



    void print (FILE* pOutput=stdout);

    /* computes the total universal size of the key according its definition and returns this total size */
    uint32_t _reComputeKeySize(void) ;

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

    void clear (void)
    {
//      KeyNaturalSize=0;
//      KeyUniversalSize=0;
//      KDicSize=0;
      Recomputed=false;
      _Base::clear();
      return;
    }

    utf8String toXml(int pLevel, int pRank, bool pComment=false);
    ZStatus fromXml(zxmlNode* pKeyDicNode, ZaiErrors* pErrorlog);

    /** @brief _export exports current key dictionary in a ZAExport normalized format into a new ZDataBuffer as result
     */
    ZDataBuffer _export();
    /**
     * @brief _exportAppend same as previous but appends to pZDBExport ZDataBuffer
     */
    ZDataBuffer& _exportAppend(ZDataBuffer& pZDBExport);

    size_t _import(unsigned char *&pZDBImport_Ptr);



} ;

} //namespace zbs

#endif // ZKEYDICTIONARY_H
