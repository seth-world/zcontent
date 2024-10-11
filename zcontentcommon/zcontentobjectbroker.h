#ifndef ZCONTENTOBJECTBROKER_H
#define ZCONTENTOBJECTBROKER_H

#include <zio/zdir.h>
#include <zcontent/zrandomfile/zrandomfiletypes.h>

#include <ztoolset/uristring.h>
#include <QIcon>

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

enum ZDomain_type : uint32_t
{
    ZDOM_Nothing = 0,
    ZDOM_Remote   = 1 ,
    ZDOM_Absolute = 2 ,
    ZDOM_Relative = 4 ,
    ZDOM_ObjectMask =   0xFF00,
    ZDOM_Path       =   0x0100,
    ZDOM_Icon       =   0x0200,
    ZDOM_Image      =   0x0400,
    ZDOM_File       = 0x010000,
    ZDOM_ImageFile  = 0x010400,

    ZDOM_Viewable   = ZDOM_Icon | ZDOM_Image ,


    ZDOM_Root       = 0x10000000
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

/*

    Syntax :

    <domain logical path>/<file name including extension>

    <domain logical path>.<object name>

    Examples :

    gabu.dgabu.detail/image.png

    <gabu.dgabu.detail> are all paths objects

    translated value

    /content gabu/content dgabu/content detail/image.png

    using following rules
        - gabu is absolute or null
        - dgabu , detail are relative
            if one of the later above is absolute, then absolute content replaces already computed path


    gabu.dgabu.detail.icon
    <gabu.dgabu.detail> are all paths objects

    <icon> is an object (possibly of type icon and icon'content will contain an image file name)

    /content gabu/content dgabu/content detail/content icon


    Parsing process :

    break domain into parts



*/

class ZDomainPath
{
public:
    ZDomainPath()=default;
    ZDomainPath(uint32_t pType) { Type=pType; }
    ZDomainPath(ZDomainPath& pIn) { _copyFrom(pIn); }

    ~ZDomainPath()
    {
        while (Inferiors.count()>0)
            delete (Inferiors.popR());
    }

    ZDomainPath& _copyFrom(const ZDomainPath& pIn) ;

    utf8VaryingString   getHierarchy() ;

    void clear();

    utf8VaryingString getName()
    {
        if ((Father==nullptr)&&(Name.isEmpty()))
            return "root";
        return Name;
    }

    void getPathPart(uriString& pOutPath,ZaiErrors* pErrorLog);


    bool isRoot() { return ( Father==nullptr ) && ( Name.isEmpty() ) ; }
    bool isNull(){ return isRoot() && (Inferiors.count()==0) && (Type == ZDOM_Nothing) ; }
    bool isValid() { return !isNull();}

    bool removeInferior(ZDomainPath*pToRemove);

    utf8VaryingString   Name;
    utf8VaryingString   Content;
    utf8VaryingString   ToolTip;
    uint32_t            Type = ZDOM_Nothing; // see ZDomain_type
    ZDomainPath*        Father=nullptr;

    ZArray<ZDomainPath*> Inferiors;
};


class ZDomainObject : public ZDomainPath
{
public:
    ZDomainObject() = default;
    ZDomainObject(const ZDomainObject& pIn) { _copyFrom(pIn); }

    ZDomainObject& _copyFrom(const ZDomainObject& pIn)
    {
        Path.clear();
        for (long wi = 0 ; wi < pIn.Path.count() ; wi ++ ) {
            Path.push(pIn.Path[wi]);
        }
        ZDomainPath::_copyFrom(pIn);
        return *this;
    }

    ZDomainObject& operator = (const ZDomainObject& pIn) { return _copyFrom(pIn); }

    ZArray<ZDomainPath*> Path;

    void clear()
    {
        while (Path.count())
            Path.pop();
        ZDomainPath::clear();
    }

    bool isValid() {return ZDomainPath::isValid();}

    ZStatus constructFromString(const utf8VaryingString& pDomainPath, ZaiErrors* pErrorLog);
    void constructFromDP(ZDomainPath* pDomainPath, ZaiErrors* pErrorLog);
    uriString getPathOnly(ZaiErrors* pErrorLog);
    uriString getPath(ZaiErrors* pErrorLog);

};


/*
 Domain and file name

 <domain hierarchy>/<file name>

<domain hierachy>
    <highest level domain name>.<dependent domain>.< ...>...

<file name>
    file base name (including file name extension)

Example of valid domain file specification

    generalparameters.iconfactory.icon.QuitIcon  with type ZDOM_Icon
        is an icon object path and points to the file /home/gerard/Development/zbasetools/zqt/icons/system-shutdown.png
    generalparameters.iconfactory/system-shutown.png with type ZDOM_Path
        is a file path and points to the same file /home/gerard/Development/zbasetools/zqt/icons/system-shutdown.png
*/

class ZDomainBroker
{
public:
    ZDomainBroker()
    {
        DomainRoot = new ZDomainPath(ZDOM_Root) ;
    }
    ZDomainBroker(const ZDomainBroker&pIn) { _copyFrom(pIn); }
    ZDomainBroker& _copyFrom(const ZDomainBroker&pIn) ;
    ~ZDomainBroker() {
        if (DomainRoot!=nullptr)
            delete DomainRoot;
    }

    ZDomainBroker& operator = (const ZDomainBroker&pIn) { return _copyFrom(pIn);}

    void clear() ;

    bool isNull()  { return DomainRoot->Inferiors.count() == 0 ; }

    ZStatus XmlLoad(uriString& pXmlFile,ZaiErrors* pErrorLog);
    ZStatus XmlLoadString(utf8VaryingString& pXmlString, ZaiErrors *pErrorLog);

    utf8VaryingString toXmlDomainItem(ZDomainPath *pDomain, int pLevel, ZaiErrors *pErrorLog);
    utf8VaryingString toXml(int pLevel, ZaiErrors *pErrorLog);

//    ZStatus constructPath(const utf8VaryingString &pDomainPath, uriString &pOutPath, ZDomainPath *pObject, ZaiErrors *pErrorLog);

    ZDomainObject constructFromPath(const utf8VaryingString& pDomainPath,ZaiErrors* pErrorLog);

    ZStatus constructPath(const utf8VaryingString& pDomainPath,ZDomainObject& pObject, ZaiErrors* pErrorLog);

    /* returns a pointer to father domain of pPtrToSearch (pointing to a valid domain)
     * returns nullptr if pPtrToSearch points to a first rank domain */
//    ZDomainPath* findFatherDomain(ZDomainPath* pPtrToSearch);

    ZDomainPath* whoHasInferior(ZDomainPath* pPtrToSearch);
    ZDomainPath* _whoHasInferior(ZDomainPath* pDomain,ZDomainPath* pPtrToSearch);

    ZDomainPath*        DomainRoot=nullptr;


//    ZArray<ZDomainPath*> DomainList;

    ZStatus _breakDomain(const utf8VaryingString& pInPath,
                         ZArray<ZDomainPath*>& pDPList,
                         utf8VaryingString& pFileName,
                         ZaiErrors* pErrorLog);

private:
    ZDomainPath* XmlGetDomain(zxmlElement* pDomainNode, ZDomainPath *pFather, int pLevel, ZaiErrors *pErrorLog);
    void breakDomainPath(const utf8VaryingString &pDomainPath, ZArray<utf8VaryingString>& pDomainList);
    void breakDomainPath(const utf8VaryingString &pDomainPath, ZArray<utf8VaryingString>& pDomainList,utf8VaryingString& pFileName);



}  ;




class ZContentObjectBroker
{
public:
    ZContentObjectBroker();
    ZContentObjectBroker(const ZContentObjectBroker&pIn) { _copyFrom(pIn); }
    ZContentObjectBroker& _copyFrom(const ZContentObjectBroker&pIn) ;

    ZStatus loadContent(const utf8VaryingString& pDomainFileSpec, ZDataBuffer& pContent,ZaiErrors* pErrorLog);
    ZStatus loadUtf8(const utf8VaryingString& pDomainFileSpec,utf8VaryingString& pUtfContent,ZaiErrors* pErrorLog);

    ZDomainObject getObjetFromDomain(const utf8VaryingString& pDomainFileSpec,ZaiErrors* pErrorLog);

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


extern class ZDomainBroker DomainBroker ;
extern class ZContentObjectBroker ContentObjectBroker;


utf8VaryingString decode_ZDomain_type(uint32_t pType);
uint32_t encode_ZDomain_type(utf8VaryingString& pType);
utf8VaryingString decode_ZDomainTypeForDisplay(uint32_t pType);

}//namespace zbs
#endif // ZCONTENTOBJECTBROKER_H
