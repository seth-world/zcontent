#ifndef ZMFOBJECT_H
#define ZMFOBJECT_H

#include "zdomainobject.h"
#include <zcontent/zindexedfile/zsearchfileentity.h>  /* for ZSearchMasterFile */

namespace zbs {

enum DomainObjectInstruction : uint32_t
{
    DOINS_Nothing           =   0 ,
    DOINS_OpenReadonly      =   0x11,   /* ZMF only : Entities are open within Declare clause */
    DOINS_OpenModifiy       =   0x12,   /* ZMF only : Entities are open within Declare clause */

    DOINS_Close             =   0xF0,   /* ZMF only : Entities are closed */

    /*--------------------------------------------content requests-----------------------------*/

    /* following may be used both for ZMF and Entity */

    DOINS_Get               =   0x0200020,  /*  arg : record logical rank (uint64_t) return ZStatus(uint32_t)+ ZDataBuffer : exported record content */
    DOINS_GetFirst          =   0x0200021,  /*  arg : none  return ZStatus(uint32_t)+ ZDataBuffer : exported record content if ZS_SUCCESS */
    DOINS_GetNext           =   0x0200022,  /*  arg : none  return ZStatus(uint32_t)+ ZDataBuffer : exported record content if ZS_SUCCESS */

    DOINS_GetPrevious       =   0x0200023,  /*  arg : none  return ZStatus(uint32_t)+ ZDataBuffer : exported record content if ZS_SUCCESS */

    DOINS_Add               =   0x0200100,  /*  arg : ZDataBuffer : exported record content  return ZStatus(uint32_t) */
    DOINS_Change            =   0x0200100,  /*  arg : record logical rank (uint64_t) + ZDataBuffer : exported record content  return : ZStatus(uint32_t) */

    DOINS_Remove            =   0x0200200,  /*  arg : record logical rank (uint64_t)  return ZStatus(uint32_t) */

    DOINS_Info              =   0x0100000,      /*-------------Information requests-------------*/

    DOINS_Exists            =   0x0100001,      /* returns a bool   - ZMF only*/
    DOINS_IsOpen            =   0x0100100,      /* return a bool    - ZMF only */

    DOINS_RecordCount       =   0x0100100,      /* return an int64_t (-1 file error ; 0-... nb of records ) - All (ZMF and Entity) */

    DOINS_GetMetaDic        =   0x0110000,      /* returns metadic as export data           - All (ZMF and Entity) */
    DOINS_GetKeyDic         =   0x0120000,      /* returns keydic as export data            - All (ZMF and Entity) */
    DOINS_GetDictionary     =   0x0130000,      /* returns both as export data              - All (ZMF and Entity) */

/* Entity declaration */

    DOINS_Declare           =   0x0300000       /* arg string declare phrase returns ZStatus ErrorLog messages array */

    /* to retreive entity ranks use Getxxxx instructions code */


};


class ZMFObject : public ZDomainObject, public ZSearchMasterFile
{
public:
    ZMFObject();
    ZMFObject(const ZMFObject& pIn) { ZDomainObject::_copyFrom(pIn); }
    ZMFObject(ZDomainPath* pDP,ZaiErrors* pErrorLog) : ZDomainObject(pDP,pErrorLog){}

    ZMFObject& _copyFrom(const ZMFObject& pIn);

    ZMFObject& operator = (const ZMFObject& pIn) { return _copyFrom(pIn); }

    void clear()
    {
        ZDomainObject::clear();
    }

    bool isOpen() ;

    ZStatus openReading();

    ZStatus openModify();

    ZStatus getFirst();

    ZStatus getNext();

    ZStatus close();

}; // ZMFObject

}// namespace zbs

#endif // ZMFOBJECT_H
