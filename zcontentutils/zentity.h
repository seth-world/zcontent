#ifndef ZENTITY_H
#define ZENTITY_H


/** @file zentity.h this file describes the type of objects in the application that are resources with a ZResourceCode
 *  this file is particular to the application but contains a subset of commonly defined entity code values
 *
 * This file is dedicated to application zcontentutils
 *
 * @see ZLayout within zlayout.h
 */

#include <stdint.h>

typedef uint64_t ZEntity_type;
const ZEntity_type cst_InvalidEntity = 0;


/* ZEntity qualifies a ZResource: it is requested by Lockmanager when it generates a new ZResourceCode object */

enum ZEntity : ZEntity_type  /* application defined ZEntity */
{
    ZEntity_Nothing     =                   0,
    ZEntity_NoEntity    =                   0,

    // ---------Data structures----------------------

    ZEntity_Class       =                   1,  // mask to say 'this is an application data structure'

    ZEntity_KeyDic      =          0x0000011,
    ZEntity_KeyField    =          0x0000021,
    ZEntity_DicField    =          0x0000031,
    ZEntity_KeyDicRow   =          0x0000041, /* QList with key field items row  */
    ZEntity_KeyDicRowsSet =        0x0000051, /* QList of Qlist with key field rows - with row[0] as key dictionary row */
    ZEntity_KeyFieldRow =          0x0000061, /* QList with key field items row  */
    // -----------Document entities  ---------------------
    //
    ZEntity_MasterDoc  =          0x0000091,
    ZEntity_Version     =          0x00000A1,
    //    ZEntity_Master      =   ZEntity_MasterDesc | ZEntity_Version ,     // generic should be suppressed

    ZEntity_MasterLink  =          0x00000B1,
    ZEntity_ObjectLink  =          0x00000C1,
    ZEntity_Memo        =          0x00100D1,
    ZEntity_Tag         =         0x000000E1,

    ZEntity_MimeClass   =         0x000000F1,  // Entity class that contains Mime definition (viewer, editor etc.)

    //                                  +-----> reserved for future use
    //                                  !
    ZEntity_DocPhysical =         0x00001001,
    ZEntity_DocPhyCreation=       0x00003001,     // DocPhysical is not yet created but in process of being created (typically in Cache and validated before being confirmed)



    //-------------_End Data structure----------------------------
    //

    ZEntity_DocFilespec =         0x00100000,

  /*=============== This part herebelow is common to all applications ================*/

    ZEntity_URI         =         0x00100000,  // same as ZEntity_DocFilespec : Pinboard will point to an uriString content
    ZEntity_Image       =         0x00200000,  // Pinboard will point to a QVariant containing a QImage

    //-------------- Mime Data--------------------------------------

    ZEntity_MimeData    =         0x01000000,     // generic : e. g. pinboard will contain MIME data and not a data structure nor a URI or QImage
    ZEntity_Mime_Image =          0x03000000,     //  Stores a mimedata with a QImage  - size is 0
    ZEntity_Mime_URI    =         0x05000000,     //  Stores a pointer to a QList<QUrl> - size is O
    ZEntity_Mime_Html   =         0x09000000,     //  Stores a pointer to a QString containing html text  : has later to be set to /setTextFormat(Qt::RichText)/
    ZEntity_Mime_PlainText =      0x11000000,     // Stores a pointer to a QString containing Plain text : has later to be set to /setTextFormat(Qt::PlainText)/
    ZEntity_Mime_OtherText  =     0x21000000,     // Stores a pointer to a QString containing Plain text : has later to be set to /setTextFormat(Qt::PlainText)/
    ZEntity_AppData         = 0x100000000000,  // Stores a pointer to an application data structure /


    ZEntity_Pinboard        = 0x101000000010,
    ZEntity_Trashbin        = 0x101000000020,



  //----------Admin entities----------------------------
  //


  ZEntity_VaultDataBase   =   0x100100000000,       /* Vault database (managed by  ) */
  ZEntity_Vault           =   0x100100000001,       /* Vault object */
  ZEntity_User            =   0x300000001002,       /* Single user */
  ZEntity_Group           =   0x300000001004,       /* group of user, may be any organization */
  ZEntity_SystemUser      =   0x000000000FF2,       /* System user (all privileges)- volatile id */
  ZEntity_AuthMan         =   0x100400000001,       /* Authorization manager (users base) see <authorizations.h> */
  ZEntity_LockManager     =   0x100400000000,       /* LockManager id */



  ZEntity_ZFlowSystem     =   0x100000000000,
  ZEntity_UserOrGroup     =   0x300000000000,
  ZEntity_ZFlowDocument   =   0x200000000000

};



class CharMan;
CharMan decode_ZEntity(ZEntity_type pEntity);
//#include <ztoolset/utfvtemplatestring.h>
//#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zutfstrings.h>
ZEntity_type encode_ZEntity(const utf8String &pEntity);
#endif // ZENTITY_H
