#ifndef ZLAYOUT_H
#define ZLAYOUT_H

#include <stdint.h>

/** @file zlayout.h this file is specific to application and must be defined per application */


/**
 * @brief The ZLayout_type enum application dependant enum describing transactions capable of being source or destination
 *          of a drag and drop operation or a cut and paste operation for example with ZDataReference being used
 *
 *        At the end of this file stays a set of value that is common to all applications and therefore should not be changed.
 */
enum ZLayout_type : uint32_t
{
  ZLayout_Nothing         =   0,
  ZLayout_NoLayout        =   0 ,
/*  ZLayout_TaskMain        =   0x01,
  ZLayout_PerDomain       =   0x02,
  ZLayout_Generic         =   0x03,
  ZLayout_PerProject      =   0x08 ,

  ZLayout_DomainMain      =   0x000010,
  ZLayout_SubDomainMain   =   0x000020,
  ZLayout_ProjectMain     =   0x000040,
  ZLayout_SubProjectMain  =   0x000080,

  ZLayout_ActivityMain    = 0x00000100,

  ZLayout_DocumentMain    = 0x00001000,
  ZLayout_DocumentList    = 0x00001000, // same as ZLayout_DocumentMain do not confuse with Master

  ZLayout_DocPhysical     = 0x00002000,
  ZLayout_MasterDocumentMain  = 0x00004000,
  ZLayout_Version         = 0x00008000,
  ZLayout_MasterList      = 0x00010000,  // MasterList dialog

  ZLayout_MemoMain        = 0x01000000,
  ZLayout_MemoList        = 0x02000000, // memo list dialog
*/
   ZLayout_FieldTBv      =   0x01,
   ZLayout_KeyTRv        =   0x02,
   ZLayout_TypeDLg       =   0x04,

   ZLayout_ParsingResultTRv   =   0x08,
   ZLayout_ParsingFieldTBv   =   0x10,

    ZLayout_FileBlock = 0x20,


  //--------Admin layouts---------------------------------------
  //
  ZLayout_VaultList       = 0x00010000, // Administrator Vault list
  ZLayout_UserList        = 0x00020000, // Administrator User list

  /*---------Following must stay and not be changed------*/

  ZLayout_TrashbinMain    = 0x00200000,
  ZLayout_PinboardMain    = 0x00220000,

  ZLayout_Mime            = 0x01000000,
  ZLayout_Clipboard       = 0x03000000,
  ZLayout_External        = 0x11000000  // when a drop occurs from outside : a file selection, an html content etc...

};

const char * decode_ZLayout (uint32_t pLayout);
const char * display_ZLayout (uint32_t pLayout);

#endif // ZLAYOUT_H
