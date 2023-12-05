//#include <common/zentity.h>
#include <zcontentutils/zentity.h>
#include <ztoolset/charman.h>



utf8VaryingString
decode_ZEntity (ZEntity_type pEntity)
{
    utf8VaryingString wMimeContent;
    if (pEntity & ZEntity_MimeData)
    {
        pEntity &= ~ ZEntity_MimeData ; // clear this mask because all of these have it and will return true
        wMimeContent = "Mime <";
        if (pEntity&ZEntity_Mime_Image)
        {
            wMimeContent+="Mime Image ";
        }
        if (pEntity&ZEntity_Mime_Html)
        {
            wMimeContent+="Mime Html ";
        }
        if (pEntity&ZEntity_Mime_PlainText)
        {
            wMimeContent+="Mime PlainText ";
        }
        if (pEntity&ZEntity_Mime_OtherText)
        {
            wMimeContent+="Mime OtherText ";
        }
        if (pEntity&ZEntity_Mime_URI)
        {
            wMimeContent+="Mime URI ";
        }
        wMimeContent +=">";
        return wMimeContent;
    }

    switch (pEntity)
    {
    case ZEntity_Nothing :
        return "ZEntity_Nothing";
        //    case ZEntity_NoEntity :
        //                return "ZEntity_NoEntity";
    case ZEntity_KeyDic :
        return "ZEntity_KeyDic";
    case ZEntity_KeyField :
        return "ZEntity_KeyField";
    case ZEntity_DicField:
        return "ZEntity_DicField";

    case ZEntity_KeyDicRowsSet:     /* full set of items rows for key dictionary and fields component */
      return "ZEntity_KeyDicRowsSet";
    case ZEntity_KeyDicRow:         /* key dictionary items row */
      return "ZEntity_KeyDicRow";
    case ZEntity_KeyFieldRow:       /* key field items row  */
      return "ZEntity_KeyFieldRow";

/*    case ZEntity_Project :
        return "ZEntity_Project";
    case ZEntity_SubProject :
        return "ZEntity_SubProject";
    case ZEntity_Agent :
        return "ZEntity_Agent";
    case ZEntity_Category :
        return "ZEntity_Category";
    case ZEntity_Unit :
        return "ZEntity_Unit";
    case ZEntity_Currency :
        return "ZEntity_Currency";
*/
    case ZEntity_MasterDoc :
        return "ZEntity_MasterDesc";
    case ZEntity_Version :
        return "ZEntity_Version";
    case ZEntity_MasterLink :
        return "ZEntity_MasterLink";
    case ZEntity_ObjectLink:
        return "ZEntity_ObjectLink";
    case ZEntity_Tag :
        return "ZEntity_Tag";
    case ZEntity_Memo :
        return "ZEntity_Memo";


    case ZEntity_DocPhysical :
        return "ZEntity_DocPhysical";
    case ZEntity_DocPhyCreation :
        return "ZEntity_DocPhyCreation";

    case ZEntity_DocFilespec :  // ZEntity_URI
        return "ZEntity_DocFilespec";
        //   case ZEntity_DocContent :
        //               return "Document content";

    case ZEntity_MimeClass :
        return "ZEntity_MimeClass";

    case ZEntity_Image :
        return "ZEntity_Image";

    case ZEntity_Vault:
        return "ZEntity_Vault";
    case ZEntity_AuthMan :  /* Authorization manager */
        return "ZEntity_AuthMan";
    case ZEntity_Mime_Html :
        return "ZEntity_Mime_Html";

    case ZEntity_User:
      return "ZEntity_User";
    case ZEntity_Group:
      return "ZEntity_Group";
    case ZEntity_SystemUser:
      return "ZEntity_SystemUser";

    case ZEntity_CppField:
      return "ZEntity_CppField";
    case ZEntity_CppClass:
      return "ZEntity_CppClass";
    case ZEntity_CppParsingSelection:
      return "ZEntity_CppParsingSelection";

    default: {
      utf8VaryingString wStr;
      wStr.sprintf("Unknown Entity code %lX",pEntity);
        return wStr.toCChar();
    }
    }// switch
}//decode_ZEntity


utf8VaryingString decode_ZEntityL(ZEntity_type pEntity)
{
    utf8VaryingString wMimeContent;
    if (pEntity & ZEntity_MimeData) {
        pEntity
            &= ~ZEntity_MimeData; // clear this mask because all of these have it and will return true
        wMimeContent = "Mime <";
        if (pEntity & ZEntity_Mime_Image) {
            wMimeContent += "Mime Image ";
        }
        if (pEntity & ZEntity_Mime_Html) {
            wMimeContent += "Mime Html ";
        }
        if (pEntity & ZEntity_Mime_PlainText) {
            wMimeContent += "Mime PlainText ";
        }
        if (pEntity & ZEntity_Mime_OtherText) {
            wMimeContent += "Mime OtherText ";
        }
        if (pEntity & ZEntity_Mime_URI) {
            wMimeContent += "Mime URI ";
        }
        wMimeContent += ">";
        return wMimeContent;
    }

    switch (pEntity) {
    case ZEntity_Nothing:
        return "No Entity";
        //    case ZEntity_NoEntity :
        //                return "ZEntity_NoEntity";
    case ZEntity_KeyDic :
      return "ZEntity_KeyDic";
    case ZEntity_KeyField :
      return "ZEntity_KeyField";
    case ZEntity_DicField:
      return "ZEntity_DicField";
      /*
    case ZEntity_Activity:
        return "Activity";
    case ZEntity_Domain:
        return "Domain";
    case ZEntity_SubDomain:
        return "SubDomain";
    case ZEntity_Project:
        return "Project";
    case ZEntity_SubProject:
        return "SubProject";
    case ZEntity_Agent:
        return "Agent";
    case ZEntity_Category:
        return "Category";
    case ZEntity_Unit:
        return "Unit";
    case ZEntity_Currency:
        return "Currency";
*/
    case ZEntity_Tag:
        return "Tag keyword";

    case ZEntity_DocPhysical:
        return "Physical document";
    case ZEntity_DocPhyCreation:
        return "Phys. doc. in creation";
    case ZEntity_MasterDoc:
        return "Master document root";
    case ZEntity_Version:
        return "Master document version & release";
    case ZEntity_MasterLink:
        return "Master to physical Link";

    case ZEntity_DocFilespec: // ZEntity_URI
        return "URI/File specification";
        //   case ZEntity_DocContent :
        //               return "Document content";
    case ZEntity_Memo:
        return "Memo";
    case ZEntity_MimeClass:
        return "Mime definition";

    case ZEntity_ObjectLink:
        return "Link Document to Object";

    case ZEntity_Image:
        return "Image content";

    case ZEntity_User:
      return "ZEntity_User";
    case ZEntity_Group:
      return "ZEntity_Group";
    case ZEntity_SystemUser:
      return "ZEntity_SystemUser";
    default:
        return "Unknown Entity";

    } // switch
} //decode_ZEntity

ZEntity_type encode_ZEntity(const utf8VaryingString &pEntity)
{
  if (pEntity.hasToken((const utf8_t *) "ZEntity_KeyDic"))
    return ZEntity_KeyDic;
  if (pEntity.hasToken((const utf8_t *) "ZEntity_KeyField"))
    return ZEntity_KeyField;
  if (pEntity.hasToken((const utf8_t *) "ZEntity_DicField"))
    return ZEntity_DicField;
/*
    if (pEntity.hasToken((const utf8_t *) "ZEntity_Activity"))
        return ZEntity_Activity;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_Domain"))
        return ZEntity_Domain;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_SubDomain"))
        return ZEntity_SubDomain;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_Project"))
        return ZEntity_Project;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_SubProject"))
        return ZEntity_SubProject;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_Agent"))
        return ZEntity_Agent;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_Category"))
        return ZEntity_Category;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_Unit"))
        return ZEntity_Unit;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_SubProject"))
        return ZEntity_SubProject;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_SubProject"))
        return ZEntity_SubProject;
    if (pEntity.hasToken((const utf8_t *) "ZEntity_Currency"))
        return ZEntity_Currency;
*/

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_MasterDesc"))
        return ZEntity_MasterDoc;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Version"))
        return ZEntity_Version;

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_MasterLink"))
        return ZEntity_MasterLink;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_ObjectLink"))
        return ZEntity_ObjectLink;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Memo"))
        return ZEntity_Memo;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Tag"))
        return ZEntity_Tag;

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_MimeClass"))
        return ZEntity_MimeClass;

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_DocPhysical"))
        return ZEntity_DocPhysical;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_DocPhyCreation"))
        return ZEntity_DocPhyCreation;

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_DocFilespec"))
        return ZEntity_DocFilespec;

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_URI"))
        return ZEntity_URI;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Image"))
        return ZEntity_Image;

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_MimeData"))
        return ZEntity_MimeData;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Mime_Image"))
        return ZEntity_Mime_Image;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Mime_URI"))
        return ZEntity_Mime_URI;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Mime_Html"))
        return ZEntity_Mime_Html;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Mime_PlainText"))
        return ZEntity_Mime_PlainText;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Mime_OtherText"))
        return ZEntity_Mime_OtherText;

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_VaultDataBase"))
        return ZEntity_VaultDataBase;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Vault"))
        return ZEntity_Vault;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_User"))
        return ZEntity_User;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_AuthMan"))
        return ZEntity_AuthMan;

    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Pinboard"))
        return ZEntity_Pinboard;
    if (pEntity.strcasestr((const utf8_t *) "ZEntity_Trashbin"))
        return ZEntity_Trashbin;

    return ZEntity_Nothing;
}//encode_ZEntity
