#ifndef ZCONTENTOBJECTBROKER_H
#define ZCONTENTOBJECTBROKER_H

#include <zio/zdir.h>
#include <zcontent/zrandomfile/zrandomfiletypes.h>

#include <ztoolset/uristring.h>
#include <QIcon>
#include <QImage>

#include "zdomaintype.h"
#include "zdomainpath.h"
#include "zdomainobject.h"


class ZQLabel;

namespace zbs {



/* type structure uint32_t
 *         ---> first byte : ZDFT_type
 *     XX XX
 *      ------> MSB : from enum ZFile_type
 */
enum ZObjectType : uint32_t {
    ZOBT_Nothing = 0 ,
    ZOBT_RegularFile  = ZDFT_RegularFile    ,
    ZOBT_Directory  = ZDFT_Directory      ,
    ZDFT_SymbolicLink =     ZDFT_SymbolicLink ,
    ZDFT_Other = ZDFT_Other  ,
    ZDFT_Hidden = ZDFT_Hidden ,
    ZDFT_All = ZDFT_All  ,
    // enum ZFile_type  from ZRandomFile_type
    ZOBT_ZRandomFile = ZFT_ZRandomFile << 16 | ZDFT_RegularFile ,    //!< file is ZRandomFile
    ZOBT_ZIndexFile = ZFT_ZIndexFile << 16 | ZDFT_RegularFile,    //!< file is ZIndexFile
    ZOBT_ZRawMasterFile = ZFT_ZRawMasterFile << 16 | ZDFT_RegularFile,   //!< file is raw master file (keys are only defined by their universal size)
    ZOBT_ZDicMasterFile = ZFT_ZDicMasterFile << 16 | ZDFT_RegularFile,   //!< this file uses a dictionary
    ZOBT_ZMasterFile = (ZFT_ZDicMasterFile|ZFT_ZRawMasterFile) << 16 | ZDFT_RegularFile ,   //!< file is Structured Master File using a dictionary

    ZOBTDictionaryFile = ZFT_DictionaryFile << 16 | ZDFT_RegularFile ,
    ZOBT_Any         = ZFT_Any   << 16 | ZDFT_RegularFile  //!< all file types allowed
};

/*
 *

 generalparameters
    icons
    help
    fixedfont
 local
    defaultworkdirectory
    ...

 content
    data
    images

    testdata
    testimages

codeparser
    parameters

query
    parameters
    parser
    script




*/




class ZContentObjectBroker
{
public:
    ZContentObjectBroker();
    ZContentObjectBroker(const ZContentObjectBroker&pIn) { _copyFrom(pIn); }
    ZContentObjectBroker& _copyFrom(const ZContentObjectBroker&pIn) ;

    ZStatus loadContent(const utf8VaryingString& pDomainFileSpec, ZDataBuffer& pContent,ZaiErrors* pErrorLog);
    ZStatus loadUtf8(const utf8VaryingString& pDomainFileSpec,utf8VaryingString& pUtfContent,ZaiErrors* pErrorLog);

    ZDomainObject getObjetFromDomain(const utf8VaryingString& pDomainFileSpec,ZaiErrors* pErrorLog);

    ZDomainObject  getViewerFromType(uint32_t pType, ZaiErrors *pErrorLog);

    /* searches pDomain for icon whose image file name is pImageFileName and generates locally an icon within pIcon using given image file */
    QIcon iconFactory(const utf8VaryingString& pDomain, ZaiErrors *pErrorLog);
/*    ZStatus iconFactory(const utf8VaryingString& pImageDomainSpec,  QIcon& pIcon, ZaiErrors *pErrorLog);
    QIcon iconFactory(const utf8VaryingString& pImageDomainSpec, ZaiErrors* pErrorLog);

    ZStatus imageFactory(const utf8VaryingString& pDomain, const utf8VaryingString& pImageFileName, QImage& pImage, ZaiErrors* pErrorLog);
 */
    QImage imageFactory(const utf8VaryingString& pDomain, ZaiErrors* pErrorLog);

    ZQLabel* labelFactory(const utf8VaryingString& pImageDomainSpec, ZaiErrors* pErrorLog);



    ZStatus open(const utf8VaryingString& pDomain,ZArray<utf8VaryingString>& pDirList);
    ZStatus _open(ZArray<utf8VaryingString> pDomainList,ZArray<utf8VaryingString>& pDirList);

    ZStatus dirFull(const utf8VaryingString& pDomain,ZArray<DirMap>& pDirList);
    ZStatus _dirFull(ZArray<utf8VaryingString> pDomainList,ZArray<DirMap>& pDirList);


}  ;

class ZContentInfos
{
    ZContentInfos()=default;
    ZContentInfos(ZContentInfos&)=delete;

    static ZStatus getInfos(const utf8VaryingString& pDomainPath , const utf8VaryingString& pObjectName , ZContentInfos& pInfos);

    ZStatus dir(const utf8VaryingString& pDomainPath, ZArray<utf8VaryingString>& pDirList);
    ZStatus _dir(ZArray<utf8VaryingString> pDomainList,ZArray<utf8VaryingString>& pDirList);

    ZStatus dirFull(const utf8VaryingString& pDomain,ZArray<DirMap>& pDirList);
    ZStatus _dirFull(ZArray<utf8VaryingString> pDomainList,ZArray<DirMap>& pDirList);
};



extern class ZContentObjectBroker ContentObjectBroker;



}//namespace zbs
#endif // ZCONTENTOBJECTBROKER_H
