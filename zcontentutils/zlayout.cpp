/** @file zlayout.cpp this file is specific to application and must be defined per application

*/

#include "zlayout.h"

const char * decode_ZLayout (uint32_t pLayout)
{
  switch (pLayout)
  {
  case ZLayout_Nothing :
    return "ZLayout_Nothing";
/*  case ZLayout_PerDomain :
    return "ZLayout_PerDomain";
  case ZLayout_Generic :
    return "ZLayout_Generic";
  case ZLayout_PerProject :
    return "ZLayout_PerProject";
  case ZLayout_DomainMain :
    return "ZLayout_DomainMain";
  case ZLayout_SubDomainMain :
    return "ZLayout_SubDomainMain";
  case ZLayout_ProjectMain :
    return "ZLayout_ProjectMain";
  case ZLayout_SubProjectMain :
    return "ZLayout_SubProjectMain";
  case ZLayout_ActivityMain :
    return "ZLayout_ActivityMain";
  case ZLayout_DocPhysical :
    return "ZLayout_DocPhysical";
  case ZLayout_DocumentMain :
    return "ZLayout_DocumentMain";
  case ZLayout_MasterDocumentMain :
    return "ZLayout_MasterDocumentMain";
*/
  case ZLayout_FieldTBv :
    return "ZLayout_FieldTBv";
  case ZLayout_KeyTRv :
    return "ZLayout_KeyTRv";
  case ZLayout_TypeDLg :
    return "ZLayout_TypeDLg";

  case ZLayout_TrashbinMain :
    return "ZLayout_TrashbinMain";
  case ZLayout_PinboardMain :
    return "ZLayout_PinboardMain";
  case ZLayout_Mime :
    return "ZLayout_Mime";
  case ZLayout_Clipboard :
    return "ZLayout_Clipboard";
  case ZLayout_External :
    return "ZLayout_External";
  default:
    return "Unknown Layout";
  }// switch
}//decodeZLayout



