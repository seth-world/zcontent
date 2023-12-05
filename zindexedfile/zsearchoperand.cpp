#include "zsearchoperand.h"
#include "zsearchtoken.h"

#include "zmetadic.h"

#include "zsearchparser.h"
#include "zsearchlogicalterm.h"
#include "zsearcharithmeticterm.h"

#include "zdataconversion.h"


ZSearchOperandBase::~ZSearchOperandBase()
{
    if (ModParams!=nullptr) {
        switch (ModifierType)
        {
        case ZSRCH_TRANSLATE:
        {
            struct Mod_Translate* wMod_Decode=static_cast<Mod_Translate*>(ModParams);
            delete wMod_Decode;
            ModParams=nullptr;
            break;
        }
        case ZSRCH_SUBSTRING:
        {
            struct Mod_SubString* wMod=static_cast<Mod_SubString*>(ModParams);
            delete wMod;
            ModParams=nullptr;
            break;
        }
        case ZSRCH_SUBSTRINGRIGHT:
        case ZSRCH_SUBSTRINGLEFT:
        {
            struct Mod_LeftRight* wMod=static_cast<Mod_LeftRight*>(ModParams);
            delete wMod;
            ModParams=nullptr;
            break;
        }//ZSRCH_SUBSTRINGLEFT
        }// switch
    } // ModParams not null
    return;
} //ZSearchOperandBase::~ZSearchOperandBase

void
ZSearchOperandBase::clear()
{
    ZSTO=ZSTO_Nothing;
    Comment.clear();
    FullFieldName.clear();

    ZOperandContent::clear();

    if (ModParams!=nullptr) {
        switch (ModifierType)
        {
        case ZSRCH_TRANSLATE:
        {
            struct Mod_Translate* wMod_Decode=static_cast<Mod_Translate*>(ModParams);
            delete wMod_Decode;
            ModParams=nullptr;
            break;
        }
        case ZSRCH_SUBSTRING:
        {
            struct Mod_SubString* wMod=static_cast<Mod_SubString*>(ModParams);
            delete wMod;
            ModParams=nullptr;
            break;
        }
        case ZSRCH_SUBSTRINGRIGHT:
        case ZSRCH_SUBSTRINGLEFT:
        {
            struct Mod_LeftRight* wMod=static_cast<Mod_LeftRight*>(ModParams);
            delete wMod;
            ModParams=nullptr;
            break;
        }//ZSRCH_SUBSTRINGLEFT
        }// switch
    } // ModParams not null
    ModifierType=ZSRCH_NOTHING;
    return;
} //ZSearchOperandBase::~ZSearchOperandBase



ZSearchOperandBase&
ZSearchOperandBase::_copyFrom(const ZSearchOperandBase& pIn)
{
    clear();
    ZSTO = pIn.ZSTO;
    ZOperandContent::_copyFrom(pIn);
    ModifierType = pIn.ModifierType;

    Comment = pIn.Comment;
    FullFieldName = pIn.FullFieldName;
    _URFParser = pIn._URFParser;

    if (pIn.ModParams!=nullptr) {
        switch (ModifierType)
        {
        case ZSRCH_TRANSLATE:
        {
            Mod_Translate* wMod_In=static_cast<Mod_Translate*>(pIn.ModParams);
            ModParams = new Mod_Translate(*wMod_In);
            break;
        }
        case ZSRCH_SUBSTRING:
        {
            Mod_SubString* wMod_In=static_cast<Mod_SubString*>(pIn.ModParams);
            ModParams = new Mod_SubString(*wMod_In);
            break;
        }
        case ZSRCH_SUBSTRINGRIGHT:
        case ZSRCH_SUBSTRINGLEFT:
        {
            Mod_LeftRight* wMod=static_cast<Mod_LeftRight*>(pIn.ModParams);
            ModParams = new Mod_LeftRight(*wMod);
            break;
            break;
        }//ZSRCH_SUBSTRINGLEFT
        }// switch
    } // ModParams not null

    return *this;
}


ZOperandContent::ZOperandContent()
{ }

ZOperandContent&
ZOperandContent::_copyFrom(const ZOperandContent& pIn) {
    clear();

  OperandZSTO = pIn.OperandZSTO;
  /*
  String = pIn.String;
  URI = pIn.URI;
  Integer = pIn.Integer;
  Float = pIn.Float;
  CheckSum = pIn.CheckSum;
  Bool = pIn.Bool;
  Date = pIn.Date;
  Resource = pIn.Resource;
*/

  if (pIn.isNull())
      return *this;

  ZSearchOperandType_type wZSTO = OperandZSTO&ZSTO_BaseMask;

  switch (wZSTO)
  {
  case ZSTO_String:
  {
      utf8VaryingString* wContent=static_cast<utf8VaryingString*>(pIn.Value);
      Value=new utf8VaryingString(*wContent);
  }
  case ZSTO_Integer:
  {
      long* wContent=static_cast<long*>(pIn.Value);
      Value=new long(*wContent);
      break;
  }
  case ZSTO_Float:
  {
      double* wContent=static_cast<double*>(pIn.Value);
      Value=new double(*wContent);
      break;
  }
  case ZSTO_Bool:
  {
      bool* wContent=static_cast<bool*>(pIn.Value);
      Value=new bool(*wContent);
      break;
  }
  case ZSTO_Checksum:
  {
      checkSum* wContent=static_cast<checkSum*>(pIn.Value);
      Value=new checkSum(*wContent);
      break;
  }
  case ZSTO_Resource:
  {
      ZResource* wContent=static_cast<ZResource*>(pIn.Value);
      Value=new ZResource(*wContent);
      break;
  }
  case ZSTO_Date:
  {
      ZDateFull* wContent=static_cast<ZDateFull*>(pIn.Value);
      Value=new ZDateFull(*wContent);
      break;
  }
  case ZSTO_UriString:
  {
      uriString* wContent=static_cast<uriString*>(pIn.Value);
      Value=new uriString(*wContent);
      break;
  }
  default:
      _DBGPRINT("ZOperandContent::_copyFrom  Invalid operand type 0x%X.",wZSTO)
      return *this;
  } // switch

  return *this;
}


void ZOperandContent::clear() {

    ZSearchOperandType_type wZSTO = OperandZSTO&ZSTO_BaseMask;

    OperandZSTO=ZSTO_Nothing;

    if (Value==nullptr) {
        return;
    }

    switch (wZSTO)
    {
    case ZSTO_String:
    {
        utf8VaryingString* wContent=static_cast<utf8VaryingString*>(Value);
        delete wContent;
        Value=nullptr;
        return;
    }
    case ZSTO_Integer:
    {
        long* wContent=static_cast<long*>(Value);
        delete wContent;
        Value=nullptr;
        return;
    }
    case ZSTO_Float:
    {
        double* wContent=static_cast<double*>(Value);
        delete wContent;
        Value=nullptr;
        return;
    }
    case ZSTO_Bool:
    {
        bool* wContent=static_cast<bool*>(Value);
        delete wContent;
        Value=nullptr;
        return;
    }
    case ZSTO_Checksum:
    {
        checkSum* wContent=static_cast<checkSum*>(Value);
        delete wContent;
        Value=nullptr;
        return;
    }
    case ZSTO_Resource:
    {
        ZResource* wContent=static_cast<ZResource*>(Value);
        delete wContent;
        Value=nullptr;
        return;
    }
    case ZSTO_Date:
    {
        ZDateFull* wContent=static_cast<ZDateFull*>(Value);
        delete wContent;
        Value=nullptr;
        return;
    }
    case ZSTO_UriString:
    {
        uriString* wContent=static_cast<uriString*>(Value);
        delete wContent;
        Value=nullptr;
        return;
    }
    default:
        _DBGPRINT("ZOperandContent::clear  Invalid operand type 0x%X.",wZSTO)
        return ;
    } // switch



    /*
    Integer=0;
    String.clear();
    URI.clear();
    Float=0.0;
    Bool=false;
    Date.clear();
    Resource.clear();
    CheckSum.clear();
*/
} // ZOperandContent::clear

bool ZOperandContent::operator==(const ZOperandContent &pIn) const
{
    if ((OperandZSTO&ZSTO_BaseMask) != (pIn.OperandZSTO&ZSTO_BaseMask) )
        return false;
    ZSearchOperandType_type wZSTO = OperandZSTO&ZSTO_BaseMask;

    switch (wZSTO)
    {
    case ZSTO_String:
    {
        utf8VaryingString* wContent=static_cast<utf8VaryingString*>(Value);
        utf8VaryingString* wContent1=static_cast<utf8VaryingString*>(pIn.Value);
        return *wContent == *wContent1;
    }

    case ZSTO_Integer:
    {
        long* wContent=static_cast<long*>(Value);
        long* wContent1=static_cast<long*>(pIn.Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Float:
    {
        double* wContent=static_cast<double*>(Value);
        double* wContent1=static_cast<double*>(pIn.Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Bool:
    {
        bool* wContent=static_cast<bool*>(Value);
        bool* wContent1=static_cast<bool*>(pIn.Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Checksum:
    {
        checkSum* wContent=static_cast<checkSum*>(Value);
        checkSum* wContent1=static_cast<checkSum*>(pIn.Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Resource:
    {
        ZResource* wContent=static_cast<ZResource*>(Value);
        ZResource* wContent1=static_cast<ZResource*>(pIn.Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Date:
    {
        ZDateFull* wContent=static_cast<ZDateFull*>(Value);
        ZDateFull* wContent1=static_cast<ZDateFull*>(pIn.Value);
        return *wContent == *wContent1;
    }
    case ZSTO_UriString:
    {
        uriString* wContent=static_cast<uriString*>(Value);
        uriString* wContent1=static_cast<uriString*>(pIn.Value);
        return *wContent == *wContent1;
    }
    default:
        _DBGPRINT("ZOperandContent::operator==  Invalid operand type 0x%X.",wZSTO)
        return false;
    } // switch
} // ZOperandContent::operator ==

bool ZOperandContent::isSame (const ZOperandContent* pIn)
{
    if ((OperandZSTO&ZSTO_BaseMask) != (pIn->OperandZSTO&ZSTO_BaseMask) )
        return false;
    ZSearchOperandType_type wZSTO = OperandZSTO & ZSTO_BaseMask;

    switch (wZSTO)
    {
    case ZSTO_String:
    {
        utf8VaryingString* wContent=static_cast<utf8VaryingString*>(Value);
        utf8VaryingString* wContent1=static_cast<utf8VaryingString*>(pIn->Value);
        return *wContent == *wContent1;
    }

    case ZSTO_Integer:
    {
        long* wContent=static_cast<long*>(Value);
        long* wContent1=static_cast<long*>(pIn->Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Float:
    {
        double* wContent=static_cast<double*>(Value);
        double* wContent1=static_cast<double*>(pIn->Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Bool:
    {
        bool* wContent=static_cast<bool*>(Value);
        bool* wContent1=static_cast<bool*>(pIn->Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Checksum:
    {
        checkSum* wContent=static_cast<checkSum*>(Value);
        checkSum* wContent1=static_cast<checkSum*>(pIn->Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Resource:
    {
        ZResource* wContent=static_cast<ZResource*>(Value);
        ZResource* wContent1=static_cast<ZResource*>(pIn->Value);
        return *wContent == *wContent1;
    }
    case ZSTO_Date:
    {
        ZDateFull* wContent=static_cast<ZDateFull*>(Value);
        ZDateFull* wContent1=static_cast<ZDateFull*>(pIn->Value);
        return *wContent == *wContent1;
    }
    case ZSTO_UriString:
    {
        uriString* wContent=static_cast<uriString*>(Value);
        uriString* wContent1=static_cast<uriString*>(pIn->Value);
        return *wContent == *wContent1;
    }
    default:
        _DBGPRINT("ZOperandContent::operator==  Invalid operand type 0x%X.",wZSTO)
        return false;
    } // switch
} // ZOperandContent::isSame

long ZOperandContent::getInteger()
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Integer) {
        _DBGPRINT("ZOperandContent::getInteger-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Integer,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return 0;
    }
    return *static_cast<long*>(Value);
}

void ZOperandContent::replaceWithInteger(long pValue)
{
    ZSearchOperandType_type wBase = OperandZSTO & ~ ZSTO_BaseMask; /* keep operand class type */
    clear();
    Value = new long(pValue);
    OperandZSTO= wBase | ZSTO_Integer;  /* change ZSTO to appropriate */
    return;
}

ZStatus ZOperandContent::setInteger(long pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Integer) {
        _DBGPRINT("ZOperandContent::setInteger-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Integer,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new long(pValue);
    else
        *static_cast<long*>(Value)=pValue;
    return ZS_SUCCESS ;
}

double ZOperandContent::getFloat()
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Float) {
        _DBGPRINT("ZOperandContent::getFloat-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Float,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return 0;
    }
    return *static_cast<double*>(Value);
}

ZStatus ZOperandContent::setFloat(double pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Float) {
        _DBGPRINT("ZOperandContent::setFloat-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Float,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new double(pValue);
    else
        *static_cast<double*>(Value)=pValue;
    return ZS_SUCCESS ;
}

void ZOperandContent::replaceWithFloat(double pValue)
{
    ZSearchOperandType_type wBase = OperandZSTO & ~ ZSTO_BaseMask; /* keep operand class type */
    clear();
    Value = new double(pValue);
    OperandZSTO= wBase | ZSTO_Float;  /* change ZSTO to appropriate */
    return;
}


bool ZOperandContent::getBool()
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Bool) {
        _DBGPRINT("ZOperandContent::getBool-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Bool,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return 0;
    }
    return *static_cast<bool*>(Value);
}
ZStatus ZOperandContent::setBool(bool pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Bool) {
        _DBGPRINT("ZOperandContent::setBool-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Bool,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new bool(pValue);
    else
        *static_cast<bool*>(Value)=pValue;
    return ZS_SUCCESS ;
}

void ZOperandContent::replaceWithBool(bool pValue)
{
    ZSearchOperandType_type wBase = OperandZSTO & ~ ZSTO_BaseMask; /* keep operand class type */
    clear();
    Value = new bool(pValue);
    OperandZSTO= wBase | ZSTO_Bool;  /* change ZSTO to appropriate */
    return;
}


utf8VaryingString ZOperandContent::getString()
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_String) {
        _DBGPRINT("ZOperandContent::getString-E Invalid operand content type 0x%X %s while expecting one of ZSTO_String,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return utf8VaryingString();
    }
    if (Value==nullptr)
        return utf8VaryingString();
    return *static_cast<utf8VaryingString*>(Value);
}


ZStatus ZOperandContent::setString(utfVaryingString<utf8_t> pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_String) {
        _DBGPRINT("ZOperandContent::setString-E Invalid operand content type 0x%X %s while expecting one of ZSTO_String,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new utf8VaryingString(pValue.Data);
    else
        *static_cast<utf8VaryingString*>(Value)=pValue;
    return ZS_SUCCESS ;
}

ZStatus ZOperandContent::setString(utf8VaryingString pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_String) {
        _DBGPRINT("ZOperandContent::setString-E Invalid operand content type 0x%X %s while expecting one of ZSTO_String,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new utf8VaryingString(pValue);
    else
        *static_cast<utf8VaryingString*>(Value)=pValue;
    return ZS_SUCCESS ;
}

ZStatus ZOperandContent::setString(utf8_t* pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_String) {
        _DBGPRINT("ZOperandContent::setString-E Invalid operand content type 0x%X %s while expecting one of ZSTO_String,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new utf8VaryingString(pValue);
    else
        *static_cast<utf8VaryingString*>(Value)=pValue;
    return ZS_SUCCESS ;
}

void ZOperandContent::replaceWithString(utf8VaryingString pValue)
{
    ZSearchOperandType_type wBase = OperandZSTO & ~ ZSTO_BaseMask; /* keep operand class type */

    clear();
    Value = new utf8VaryingString(pValue);

    OperandZSTO= wBase | ZSTO_String;  /* change ZSTO to appropriate */
    return;
}


uriString ZOperandContent::getURI()
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_UriString) {
        _DBGPRINT("ZOperandContent::getURI-E Invalid operand content type 0x%X %s while expecting one of ZSTO_UriString,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return uriString();
    }
    if (Value==nullptr)
        return uriString();
    return *static_cast<uriString*>(Value);
}

ZStatus ZOperandContent::setURI(uriString pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_UriString) {
        _DBGPRINT("ZOperandContent::setURI-E Invalid operand content type 0x%X %s while expecting one of ZSTO_UriString,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new uriString(pValue);
    else
        *static_cast<uriString*>(Value)=pValue;
    return ZS_SUCCESS ;
}

void ZOperandContent::replaceWithURI(uriString pValue)
{
    ZSearchOperandType_type wBase = OperandZSTO & ~ ZSTO_BaseMask; /* keep operand class type */
    clear();
    Value = new uriString(pValue);

    OperandZSTO = wBase | ZSTO_UriString;  /* change ZSTO to appropriate */

    return;

}

ZDateFull ZOperandContent::getDate()
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Date) {
        _DBGPRINT("ZOperandContent::getDate-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Date,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZDateFull();
    }
    return *static_cast<ZDateFull*>(Value);
}

ZStatus ZOperandContent::setDate(ZDateFull pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Date) {
        _DBGPRINT("ZOperandContent::setDate-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Date,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new ZDateFull(pValue);
    else
        *static_cast<ZDateFull*>(Value)=pValue;
    return ZS_SUCCESS ;
}

void ZOperandContent::replaceWithDate(ZDateFull pValue)
{
    ZSearchOperandType_type wBase = OperandZSTO & ~ ZSTO_BaseMask; /* keep operand class type */
    clear();
    Value = new ZDateFull(pValue);
    OperandZSTO= wBase | ZSTO_Date;  /* change ZSTO to appropriate */
    return;
}

ZResource ZOperandContent::getResource()
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Resource) {
        _DBGPRINT("ZOperandContent::getDate-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Resource,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZResource();
    }
    return *static_cast<ZResource*>(Value);
}

ZStatus ZOperandContent::setResource(ZResource pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Resource) {
        _DBGPRINT("ZOperandContent::setDate-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Resource,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new ZResource(pValue);
    else
        *static_cast<ZResource*>(Value)=pValue;
    return ZS_SUCCESS ;
}

void ZOperandContent::replaceWithResource(ZResource pValue)
{
    ZSearchOperandType_type wBase = OperandZSTO & ~ ZSTO_BaseMask; /* keep operand class type */
    clear();
    Value = new ZResource(pValue);
    OperandZSTO= wBase | ZSTO_Resource;  /* change ZSTO to appropriate */
    return;
}


checkSum ZOperandContent::getChecksum()
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Checksum) {
        _DBGPRINT("ZOperandContent::getDate-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Checksum,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return checkSum();
    }
    return *static_cast<checkSum*>(Value);
}

ZStatus ZOperandContent::setChecksum(checkSum pValue)
{
    if ((OperandZSTO&ZSTO_BaseMask)!=ZSTO_Checksum) {
        _DBGPRINT("ZOperandContent::setDate-E Invalid operand content type 0x%X %s while expecting one of ZSTO_Checksum,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE ;
    }
    if (Value == nullptr)
        Value = new checkSum(pValue);
    else
        *static_cast<checkSum*>(Value)=pValue;
    return ZS_SUCCESS ;
}

void ZOperandContent::replaceWithChecksum(checkSum pValue)
{
    ZSearchOperandType_type wBase = OperandZSTO & ~ ZSTO_BaseMask; /* keep operand class type */
    clear();
    Value = new checkSum(pValue);
    OperandZSTO= wBase | ZSTO_Checksum;  /* change ZSTO to appropriate */
    return;
}

ZStatus
ZOperandContent::appendURF(ZDataBuffer& pURFBuffer)
{
    URFField wURFField;
    ZSearchOperandType_type wZSTO = OperandZSTO & ZSTO_BaseMask ;
    switch (wZSTO)
    {
    case ZSTO_Integer:
        {
        long wValue=*static_cast<long*>(Value);
        size_t wL=getAtomicURFSize<long>(wValue);
        unsigned char* wPtr=pURFBuffer.extend(wL);
        exportAtomicURF_Ptr<long>(wValue,wPtr);
        return ZS_SUCCESS;
        }
    case ZSTO_Bool:
        {
        bool wValue=*static_cast<bool*>(Value);
        size_t wL=getAtomicURFSize<bool>(wValue);
        unsigned char* wPtr=pURFBuffer.extend(wL);
        exportAtomicURF_Ptr<bool>(wValue,wPtr);
        return ZS_SUCCESS;
        }
    case ZSTO_Float:
        {
        bool wValue=*static_cast<double*>(Value);
        size_t wL=getAtomicURFSize<double>(wValue);
        unsigned char* wPtr=pURFBuffer.extend(wL);
        exportAtomicURF_Ptr<double>(wValue,wPtr);
        return ZS_SUCCESS;
        }

    case ZSTO_String:
    {
        utf8VaryingString* wValue=static_cast<utf8VaryingString*>(Value);
        wValue->_exportURF(pURFBuffer);
        return ZS_SUCCESS;
    }
    case ZSTO_UriString:
    {
        uriString* wValue=static_cast<uriString*>(Value);
        wValue->_exportURF(pURFBuffer);
        return ZS_SUCCESS;
    }
    case ZSTO_Date:
    {
        ZDateFull* wValue=static_cast<ZDateFull*>(Value);
        wValue->_exportURF(pURFBuffer);
        return ZS_SUCCESS;
    }
    case ZSTO_Resource:
    {
        ZResource* wValue=static_cast<ZResource*>(Value);
        wValue->_exportURF(pURFBuffer);
        return ZS_SUCCESS;
    }
    case ZSTO_Checksum:
    {
        checkSum* wValue=static_cast<checkSum*>(Value);
        wValue->_exportURF(pURFBuffer);
        return ZS_SUCCESS;
    }
    default:
    {
        _DBGPRINT("ZOperandContent::appendURF-E Invalid operand content type 0x%X %s while expecting of of ZSTO_Bool,...\n",
                  OperandZSTO , decode_OperandType(OperandZSTO))
        return ZS_INVTYPE;
    }
    }// switch
    return ZS_INVTYPE;
} //ZOperandContent::appendURF

 utf8VaryingString ZOperandContent::displayContent()
{
     utf8VaryingString wReturn;

    ZSearchOperandType_type wBaseZSTO = OperandZSTO & ZSTO_BaseMask;
     switch (wBaseZSTO)
     {
     case ZSTO_Integer:
         wReturn.sprintf("%ld",getInteger());

     case ZSTO_Float:
         wReturn.sprintf("%g",getFloat());

     case ZSTO_String:
         return getString();
     case ZSTO_UriString:
         return getURI();

     case ZSTO_FieldBool:
         return getBool()?"TRUE":"FALSE";

     case ZSTO_Date:
         return getDate().toFormatted();

     case ZSTO_Resource:
     {
         long wi=0;
         for (;wi < ZEntitySymbolList.count();wi++) {
             if (ZEntitySymbolList[wi].Value==getResource().Entity) {
                 break;
             }
         }// for
         if (wi==ZEntitySymbolList.count())
             wReturn.sprintf("ZResource(%6X,%6X)",getResource().Entity,getResource().id);
         else
             wReturn.sprintf("ZResource(%s,%6X)",ZEntitySymbolList[wi].Symbol.toString(),getResource().id);
         return wReturn;
     } //ZSTO_Resource
     case ZSTO_Checksum:
         return getChecksum().toHexa();
     }
     return wReturn;

}//ZOperandContent::displayContent


using namespace  zbs;
ZSearchOperator::~ZSearchOperator()
{
/*  while (TokenList.count())
    TokenList.pop();
*/
}

ZSearchOperator&
ZSearchOperator::_copyFrom (const ZSearchOperator& pIn) {
  ZSOPV=pIn.ZSOPV;
  return *this;
}


void
ZSearchOperator::add(ZSearchToken* pTokenOperator) {
//  TokenList.push(pTokenOperator);
  ZSOPV |= convert_ZSRCH_ZSOPV( pTokenOperator->Type );
}
void
ZSearchOperator::set(ZSearchToken* pTokenOperator) {
//  TokenList.push(pTokenOperator);
  ZSOPV = convert_ZSRCH_ZSOPV( pTokenOperator->Type );
}
bool ZSearchOperator::isValid(ZSearchToken* pTokenOperator) {
  return (convert_ZSRCH_ZSOPV( pTokenOperator->Type) != ZSOPV_INVALID);
}
bool ZSearchOperator::isLogical(ZSearchToken *pTokenOperator) {
  return ((pTokenOperator->Type & ZSRCH_OPERATOR_LOGICAL)== ZSRCH_OPERATOR_LOGICAL );
}
bool ZSearchOperator::isStringComparator(ZSearchToken* pTokenOperator) {
  return ((pTokenOperator->Type & ZSRCH_OPERATOR_COMPARESTRING)== ZSRCH_OPERATOR_COMPARESTRING );
}

bool ZSearchOperator::isComparator(ZSearchToken* pTokenOperator) {
  return isLogical(pTokenOperator) || isStringComparator(pTokenOperator);
}
bool ZSearchOperator::isAndOr(ZSearchToken* pTokenOperator) {
  return (pTokenOperator->Type == ZSRCH_OPERATOR_AND ) || (pTokenOperator->Type == ZSRCH_OPERATOR_OR );
}
bool ZSearchOperator::isArithmeric(ZSearchToken* pTokenOperator) {
  return (pTokenOperator->Type & ZSRCH_OPERATOR_ARITHMETIC)== ZSRCH_OPERATOR_ARITHMETIC ;
}
bool ZSearchOperator::_isValid() {
  return ZSOPV != ZSOPV_INVALID;
}
bool ZSearchOperator::_isLogical() {
  return (ZSOPV & ZSOPV_LOGICAL_MASK)||(ZSOPV==ZSOPV_GREATEREQUAL);
}
bool ZSearchOperator::_isStringComparator() {
  return ZSOPV & ZSOPV_STRING_MASK;
}

bool ZSearchOperator::_isComparator() {
  switch(ZSOPV)
  {
  case ZSOPV_EQUAL:
  case ZSOPV_NOTEQUAL:
  case ZSOPV_LESS:
  case ZSOPV_GREATER:
  case ZSOPV_LESSEQUAL:
  case ZSOPV_GREATEREQUAL:
    return true;
  default:
    return false;
  }
}

bool ZSearchOperator::_isArithmeric() {
  //  return (Type & ~ZSOPV_GREATEREQUAL) & ZSOPV_ARITHMETIC_MASK;

  switch(ZSOPV)
  {
  case ZSOPV_MINUS:
  case ZSOPV_DIVIDE:
  case ZSOPV_MULTIPLY:
  case ZSOPV_POWER:
  case ZSOPV_MODULO:
  case ZSOPV_PLUS:
    return true;
  default:
    return false;
  }
}
bool ZSearchOperator::_isAndOr() {

  if ((ZSOPV == ZSOPV_AND) || (ZSOPV == ZSOPV_OR))
    return true;
  return false;
}
bool ZSearchOperator::_isNothing() {

  return  (ZSOPV == ZSOPV_Nothing) ;
}

utf8VaryingString
ZSearchOperator::_reportFormula(bool pDetailed)
{
  utf8VaryingString wReturn;

  switch (ZSOPV)
  {
  case ZSOPV_AND:
    wReturn="AND";
    break;
  case ZSOPV_OR:
    wReturn="OR";
    break;
  case ZSOPV_NOT:
    wReturn="NOT";
    break;

  case ZSOPV_EQUAL:
    wReturn="==";
    break;
  case ZSOPV_NOTEQUAL:
    wReturn="!=";
    break;

  case ZSOPV_LESS:
    wReturn="<";
    break;
  case ZSOPV_GREATER:
    wReturn=">";
    break;
  case ZSOPV_LESSEQUAL:
    wReturn="<=";
    break;
  case ZSOPV_GREATEREQUAL:
    wReturn=">=";
    break;
  case ZSOPV_CONTAINS:
    wReturn="CONTAINS";
    break;
  case ZSOPV_STARTS_WITH:
    wReturn="STARTS_WITH";
    break;
  case ZSOPV_ENDS_WITH:
    wReturn="ENDS_WITH";
    break;

  case ZSOPV_PLUS:
    wReturn="+";
    break;
  case ZSOPV_MINUS:
    wReturn="-";
    break;
  case ZSOPV_DIVIDE:
    wReturn="/";
    break;
  case ZSOPV_MULTIPLY:
    wReturn="*";
    break;
  case ZSOPV_POWER:
    wReturn="^";
    break;
  case ZSOPV_MODULO:
    wReturn="%";
    break;

  case ZSOPV_Nothing:
    if (pDetailed)
      wReturn = "";
    break;

  case ZSOPV_INVALID:
    wReturn = "ZSOPV_INVALID";
    break;

  default:
    wReturn.sprintf( "unknown operator <0x%X>",ZSOPV);
    break;
  }// switch

  return wReturn;
} //ZSearchOperator::_reportOp


utf8VaryingString
ZSearchOperator::_report(int pLevel)
{
  utf8VaryingString wReturn, wRet;
  if (ZSOPV & ZSOPV_AND) {
    wReturn="ZSOPV_AND";
  }
  if (ZSOPV & ZSOPV_OR) {
    wReturn.addConditionalOR("ZSOPV_OR");
  }
  if (ZSOPV & ZSOPV_NOT) {
    wReturn.addConditionalOR("ZSOPV_NOT");
  }


  if (ZSOPV & ZSOPV_EQUAL) {
    wReturn.addConditionalOR("ZSOPV_EQUAL");
  }
  if (ZSOPV & ZSOPV_NOTEQUAL) {
    wReturn.addConditionalOR("ZSOPV_NOTEQUAL");
  }
  if (ZSOPV & ZSOPV_LESS) {
    wReturn.addConditionalOR("ZSOPV_LESS");
  }
  if (ZSOPV & ZSOPV_GREATER) {
    wReturn.addConditionalOR("ZSOPV_GREATER");
  }
  if (ZSOPV & ZSOPV_LESSEQUAL) {
    wReturn.addConditionalOR("ZSOPV_LESSEQUAL");
  }
  if (ZSOPV & ZSOPV_GREATEREQUAL) {
    wReturn.addConditionalOR("ZSOPV_GREATEREQUAL");
  }

  if (ZSOPV & ZSOPV_CONTAINS) {
    wReturn.addConditionalOR("ZSOPV_CONTAINS");
  }
  if (ZSOPV & ZSOPV_STARTS_WITH) {
    wReturn.addConditionalOR("ZSOPV_STARTS_WITH");
  }
  if (ZSOPV & ZSOPV_ENDS_WITH) {
    wReturn.addConditionalOR("ZSOPV_ENDS_WITH");
  }



  if (ZSOPV & ZSOPV_PLUS) {
    wReturn.addConditionalOR("ZSOPV_PLUS");
  }
  if (ZSOPV & ZSOPV_MINUS) {
    wReturn.addConditionalOR("ZSOPV_MINUS");
  }
  if (ZSOPV & ZSOPV_DIVIDE) {
    wReturn.addConditionalOR("ZSOPV_DIVIDE");
  }
  if (ZSOPV & ZSOPV_MULTIPLY) {
    wReturn.addConditionalOR("ZSOPV_MULTIPLY");
  }
  if (ZSOPV & ZSOPV_POWER) {
    wReturn.addConditionalOR("ZSOPV_POWER");
  }
  if (ZSOPV & ZSOPV_MODULO) {
    wReturn.addConditionalOR("ZSOPV_MODULO");
  }

  if (wReturn.isEmpty()) {
    wReturn = "End-no operator";
  }

  wRet.sprintf ("%*c%s\n",pLevel,' ', wReturn.toString());
  return wRet;
} //ZSearchOperator::_report




ZSearchLogicalOperand::~ZSearchLogicalOperand()
{
  clearOperand(Operand);
}

void ZSearchLogicalOperand::clear()
{
  clearOperand(Operand);
}

void
ZSearchLogicalOperand::copyOperand(void*& pOperand,const void* pOpIn)
{
  _DBGPRINT("ZSearchLogicalOperand::copyOperand\n")
  _copyOperand(pOperand,pOpIn);
}

ZSearchLogicalOperand&
ZSearchLogicalOperand::_copyFrom(const ZSearchLogicalOperand& pIn)
{
  ZSearchOperandBase::_copyFrom(pIn);
 _copyOperand(Operand,pIn.Operand);
  return *this;
} // _copyFrom

utf8VaryingString
ZSearchLogicalOperand::_evaluateOpLiteral(void *pOp)
{
  utf8VaryingString wReturn;

  if (!(static_cast<ZSearchOperandBase*>(pOp)->ZSTO & ZSTO_Literal)) {
    wReturn.sprintf("_evaluateOpLiteral-E-INVTYP Invalid type to evaluate <%s>",
        decode_OperandType(static_cast<ZSearchOperandBase*>(pOp)->ZSTO));
    return wReturn;
  }

  ZSearchLiteral* wOp = static_cast<ZSearchLiteral*>(pOp);

  switch (static_cast<const ZSearchOperandBase*>(pOp)->ZSTO )
  {
  case ZSTO_LiteralString:
  {
 //   ZSearchLiteral<utf8VaryingString>* wOp=static_cast<ZSearchLiteral<utf8VaryingString>*>(pOp);
    wReturn.sprintf("\"%s\"",wOp->getString().toString());
//    return wOp->Content;
    return wReturn;
  }
  case ZSTO_LiteralUriString:
  {
//    ZSearchLiteral<uriString>* wOp=static_cast<ZSearchLiteral<uriString>*>(pOp);
    wReturn.sprintf("\"%s\"",wOp->getURI().toString());
    return wReturn;
  }
  case ZSTO_LiteralInteger:
  {
//    ZSearchLiteral<long>* wOp=static_cast<ZSearchLiteral<long>*>(pOp);
    wReturn.sprintf("%ld",wOp->getInteger());
    return wReturn;
  }
  case ZSTO_LiteralFloat:
  {
//    ZSearchLiteral<double>* wOp=static_cast<ZSearchLiteral<double>*>(pOp);
    wReturn.sprintf("%g",wOp->getFloat());
    return wReturn;
  }
  case ZSTO_LiteralDate:
  {
//    ZSearchLiteral<ZDateFull>* wOp=static_cast<ZSearchLiteral<ZDateFull>*>(pOp);
      return wOp->getDate().toLocale();
  }
  case ZSTO_LiteralResource:
  {
//    ZSearchLiteral<ZResource>* wOp=static_cast<ZSearchLiteral<ZResource>*>(pOp);
//    return wOp->Content.toHexa();
    long wi=0;
    for (;wi < ZEntitySymbolList.count();wi++) {
        if (ZEntitySymbolList[wi].Value==wOp->getResource().Entity) {
        break;
      }
    }// for
    if (wi==ZEntitySymbolList.count())
      wReturn.sprintf("ZResource(%6X,%6X)",wOp->getResource().Entity,wOp->getResource().id);
    else
      wReturn.sprintf("ZResource(%s,%6X)",ZEntitySymbolList[wi].Symbol.toString(),wOp->getResource().id);
    return wReturn;
  }
  case ZSTO_LiteralBool:
  {
//    ZSearchLiteral<bool>* wOp=static_cast<ZSearchLiteral<bool>*>(pOp);
    return wOp->getBool()?"TRUE":"FALSE";
  }
  case ZSTO_LiteralChecksum:
  {
//    ZSearchLiteral<checkSum>* wOp=static_cast<ZSearchLiteral<checkSum>*>(pOp);
//    return wOp->Content.toHexa();
    wReturn.sprintf("checkSum(%s)",wOp->getChecksum().toHexa().toString());
    return wReturn;
  }
  default:
  {
    wReturn.sprintf("_evaluateOpLiteral-E-INVTYP Invalid type to evaluate <%s>",
        decode_OperandType(static_cast<ZSearchOperandBase*>(pOp)->ZSTO));
    return wReturn;
  }

  }// switch
}

utf8VaryingString
ZOperandContent::display()
{
  utf8VaryingString wReturn;
  ZSearchOperandType wT = ZSearchOperandType(OperandZSTO & ZSTO_BaseMask);
  switch (wT) {
  case ZSTO_String:
      if (Value==nullptr)
        wReturn="String <null>";
      else
        wReturn.sprintf("String <%s>",getString().toString());
    break;
  case ZSTO_UriString:
        if (Value==nullptr)
            wReturn="uriString <null>";
        else
            wReturn.sprintf("uriString <%s>",getURI().toString());
    break;
  case ZSTO_Integer:
      if (Value==nullptr)
        wReturn="Integer <null>";
      else
        wReturn.sprintf("Integer <%ld>",getInteger());
    break;

  case ZSTO_Float:
      if (Value==nullptr)
          wReturn="Float <null>";
      else
          wReturn.sprintf("Float <%g>",getFloat());
    break;

  case ZSTO_Date:
      if (Value==nullptr)
          wReturn="Date <null>";
      else
          wReturn.sprintf("Date <%s>",getDate().toLocale().toString());
    break;

  case ZSTO_Resource:
      if (Value==nullptr)
          wReturn="Resource <null>";
      else
          wReturn.sprintf("Resource <%s>",getResource().toHexa().toString());
    break;
  case ZSTO_Bool:
      if (Value==nullptr)
          wReturn="Bool <null>";
      else
          wReturn.sprintf("Bool <%s>",getBool()?"true":"false");
    break;
  case ZSTO_Checksum:
      if (Value==nullptr)
          wReturn="Checksum <null>";
      else
          wReturn.sprintf("Checksum <%s>",getChecksum().toHexa().toString());
    break;
  default:
      wReturn.sprintf( "unknown operand type %X-%s",wT,decode_ZSTO(wT));
  }
  return wReturn;
}

ZStatus buildOperandContent(ZSearchEntityContext &pSEC, void *pOp, ZOperandContent *pOperandContent)
{
  pOperandContent->clear();
  ZSearchOperandType_type wZSTO =  static_cast<ZSearchOperandBase*> (pOp)->ZSTO;
  ZStatus wSt=ZS_SUCCESS;
  if (wZSTO==ZSTO_Logical) {
    /* Operand is a logical expression.
       * It returns a boolean
       * Operator is a logical link {AND,OR} to OperandNext */
    pOperandContent->OperandZSTO = ZSTO_Logical; /* This is a logical expression - type is bool */
    _DBGPRINT("getOperandContent Operand is a logical expression : evaluation ")
    pOperandContent->setBool( static_cast<ZSearchLogicalOperand*> (pOp)->evaluate(pSEC));
    _DBGPRINT(pOperandContent->display().toCChar())
    return ZS_SUCCESS;
  }
  /* Operand is an arithmetic expression.
   * result of an arithmetic expression is a qualified literal and must be processed as a literal constant
   */
  if (wZSTO & ZSTO_Arithmetic) {
    _DBGPRINT("getOperandContent Operand is an arithmetic expression ")
    ZSearchOperandType_type wArithmeticType=ZSTO_Arithmetic;
    ZSearchArithmeticTerm* wSLOP=static_cast<ZSearchArithmeticTerm*>(pOp);
    wSt=computeArithmetic(pSEC,wSLOP,wArithmeticType,*pOperandContent);
    _DBGPRINT(pOperandContent->display().toCChar())
    return wSt;
  }
  /* operand is not an expression
   * It is either a field operand or a literal operand
   *  get Next operand and evaluate expression
  */
  _DBGPRINT("getOperandContent Operand is a valid operand either field or arithmetic\n")
//  wSt = extractOperandContent(pOp,pURFParser,pOperandContent);
  wSt = extractOperandContent(pSEC,pOp,pOperandContent);

  _DBGPRINT("getOperandContent resulting value %s\n", pOperandContent->display().toCChar())
  return wSt;
}
/*
ZOperandContent
ZSearchLogicalOperand::getContent(URFParser& pURFParser) {
  return getOperandContent(Operand,pURFParser);
}
*/

bool
ZSearchLogicalOperand::evaluate(ZSearchEntityContext &pSEC)
{
    ZOperandContent wOperandContent;
    ZStatus wSt  = buildOperandContent(pSEC,Operand,&wOperandContent);
    if (wSt!=ZS_SUCCESS)
        return false;
    return wOperandContent.getBool();
} //ZSearchLogicalOperand::evaluate


utf8VaryingString
ZSearchLogicalOperand::_reportDetailed(int pLevel)
{
  utf8VaryingString wReturn;
    ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(Operand);
  wReturn.addsprintf("%*cOperand\n",pLevel,' ');
  while (true) {
    if (wOpBase->ZSTO == ZSTO_Arithmetic) {
      wReturn += static_cast<ZSearchArithmeticTerm*>(Operand)->_report(pLevel+1);
      break;
    }
    if (wOpBase->ZSTO == ZSTO_Logical) {
      wReturn += static_cast<ZSearchLogicalTerm*>(Operand)->_report(pLevel+1);
      break;
    }

    if (wOpBase->ZSTO & ZSTO_Field) {
      ZSearchFieldOperand* wFldData = static_cast<ZSearchFieldOperand*>( Operand);
      wReturn.addsprintf("%*ctype %s full field name <%s> type <%s>",
          pLevel,' ',
          decode_OperandType(wOpBase->ZSTO),
                         wFldData->FullFieldName.toString(),
                         decode_ZSearchType(wFldData->ZSTO)
          );
      break;
    }
    if (wOpBase->ZSTO & ZSTO_Literal) {
      wReturn.addsprintf("%*c type %s value <%s>",
          pLevel,' ',
          decode_OperandType(wOpBase->ZSTO),
          _evaluateOpLiteral(Operand).toString()
          );
      break;
    }
    wReturn.addsprintf("%*c type %s <Invalid type>\n",
        pLevel,' ',
        decode_OperandType(wOpBase->ZSTO)
        );
    break;
  }// while true

  wReturn += _reportModifier(pLevel);
  wReturn += "\n";

  return wReturn;
} // ZSearchLogicalOperand::_reportDetailed

utf8VaryingString
ZSearchLogicalOperand::_report(int pLevel)
{
    utf8VaryingString wReturn;
    ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(Operand);
    wReturn.addsprintf("%*cOperand\n",pLevel,' ');
    while (true) {
        if (wOpBase->ZSTO == ZSTO_Arithmetic) {
            wReturn += static_cast<ZSearchArithmeticTerm*>(Operand)->_report(pLevel+1);
            break;
        }
        if (wOpBase->ZSTO == ZSTO_Logical) {
            wReturn += static_cast<ZSearchLogicalTerm*>(Operand)->_report(pLevel+1);
            break;
        }

        if (wOpBase->ZSTO & ZSTO_Field) {
            ZSearchFieldOperand* wFldData = static_cast<ZSearchFieldOperand*>( Operand);
            wReturn.addsprintf("%*c %s ",
                               pLevel,' ',
                               wFldData->FullFieldName.toString() );
            break;
        }
        if (wOpBase->ZSTO & ZSTO_Literal) {
            wReturn.addsprintf("%*c%s",
                               pLevel,' ',
                               _evaluateOpLiteral(Operand).toString()
                               );
            break;
        }
        wReturn.addsprintf("%*c type %s <Invalid type>",
                           pLevel,' ',
                           decode_OperandType(wOpBase->ZSTO)
                           );
        break;
    }// while true
    /*
  if (static_cast<ZSearchOperandBase*>(Operand)->TokenList.count()>0) {
    wReturn.addsprintf("%*c Tokens <",pLevel,' ');
    for (int wj=0; wj < static_cast<ZSearchOperandBase*>(Operand)->TokenList.count();wj++) {
      wReturn.addsprintf("%s",static_cast<ZSearchOperandBase*>(Operand)->TokenList[wj]->Text.toString());
    }
    wReturn += ">\n";

  } else {
    wReturn.addsprintf("%*c No Token for this operand\n",pLevel,' ');
  }
*/
    //    setMaxSprintfBufferCount(4096);

    wReturn += _reportModifier(pLevel);
//    wReturn += "\n";

    return wReturn;
} // ZSearchLogicalOperand::_report

utf8VaryingString
ZSearchLogicalOperand::_reportBrief(int pLevel)
{
    utf8VaryingString wReturn;
    ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(Operand);
    while (true) {
        if (wOpBase->ZSTO == ZSTO_Arithmetic) {
            wReturn += static_cast<ZSearchArithmeticTerm*>(Operand)->_reportFormula(pLevel+1);
            break;
        }
        if (wOpBase->ZSTO == ZSTO_Logical) {
            wReturn += static_cast<ZSearchLogicalTerm*>(Operand)->_reportFormula(pLevel+1);
            break;
        }

        if (wOpBase->ZSTO & ZSTO_Field) {
            ZSearchFieldOperand* wFldData = static_cast<ZSearchFieldOperand*>( Operand);
            wReturn.addsprintf("%s", wFldData->FullFieldName.toString() );
            break;
        }
        if (wOpBase->ZSTO & ZSTO_Literal) {
            wReturn.addsprintf("%s",_evaluateOpLiteral(Operand).toString()
                               );
            break;
        }
        wReturn.addsprintf("type %X <Invalid operand type>", wOpBase->ZSTO);
        break;
    }// while true

    wReturn += _reportModifierBrief();
    //    wReturn += "\n";

    return wReturn;
} // ZSearchLogicalOperand::_report
utf8VaryingString
ZSearchOperandBase::_reportModifier(int pLevel)
{
    if (ModifierType==ZSRCH_NOTHING)
        return utf8VaryingString();
    utf8VaryingString wReturn;

    switch (ModifierType)
    {
    case ZSRCH_YEAR:
    {
        wReturn.sprintf("%*c%sYEAR resulting type is <ZSTO_Integer>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_MONTH:
    {
        wReturn.sprintf("%*c%sMONTH resulting type is <ZSTO_Integer>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_DAY:
    {
        wReturn.sprintf("%*c%sDAY resulting type is <ZSTO_Integer>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_HOUR:
    {
        wReturn.sprintf("%*c%sHOUR resulting type is <ZSTO_Integer>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_MIN:
    {
        wReturn.sprintf("%*c%sMIN resulting type is <ZSTO_Integer>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_SEC:
    {
        wReturn.sprintf("%*c%sSEC resulting type is <ZSTO_Integer>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }

    case ZSRCH_ZENTITY:
    {
        wReturn.sprintf("%*c%sENTITY resulting type is <ZSTO_Integer>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_ID:
    {
        wReturn.sprintf("%*c%sID resulting type is <ZSTO_Integer>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }


    case ZSRCH_PATH:
    {
        wReturn.sprintf("%*c%sPATH resulting type is <ZSTO_String>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_EXTENSION:
    {
        wReturn.sprintf("%*c%sEXTENSION resulting type is <ZSTO_String>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_BASENAME:
    {
        wReturn.sprintf("%*c%sBASENAME resulting type is <ZSTO_String>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }
    case ZSRCH_ROOTNAME:
    {
        wReturn.sprintf("%*c%sROOTNAME resulting type is <ZSTO_String>",
                        pLevel,' ',__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    }


    case ZSRCH_SUBSTRING:
    {
        Mod_SubString* wMod=static_cast<Mod_SubString*>(ModParams);
        wReturn.sprintf("%*c%sSUBSTRING(%d,%d) resulting type is <ZSTO_String>\n",
                           pLevel,' ',__MODIFIER_SEPARATOR_ASC__,
                           wMod->Offset,wMod->Length);
        return wReturn;
    }
    case ZSRCH_SUBSTRINGLEFT:
    {
        Mod_LeftRight* wMod=static_cast<Mod_LeftRight*>(ModParams);
        wReturn.sprintf("%*c%sLEFT(%d) resulting type is <ZSTO_String>\n",
                           pLevel,' ',__MODIFIER_SEPARATOR_ASC__,
                           wMod->Length);
        return wReturn;
    }
    case ZSRCH_SUBSTRINGRIGHT:
    {
        Mod_LeftRight* wMod=static_cast<Mod_LeftRight*>(ModParams);
        wReturn.addsprintf("%*c%sRIGHT(%d) resulting type is <ZSTO_String>\n",
                           pLevel,' ',__MODIFIER_SEPARATOR_ASC__,
                           wMod->Length);
        return wReturn;
    }

    case ZSRCH_TRANSLATE:
    {
        Mod_Translate* wMod=static_cast<Mod_Translate*>(ModParams);
        wReturn.addsprintf("%*c%sTRANSLATE  <%s> \n"
                           "%*c   key field rank %d name <%s> type <%s>\n"
                           "%*c   resulting field rank %d name <%s> "
                           "%*c   resulting ZType <%s> ZSTO <%s>\n",
                           pLevel,' ',__MODIFIER_SEPARATOR_ASC__,
                           wMod->Entity->EntityName.toString(),
                           pLevel,' ',
                           wMod->KeyDicRank,
                           wMod->Entity->LocalMetaDic[wMod->KeyDicRank].getName().toString(),
                           decode_ZType(wMod->Entity->LocalMetaDic[wMod->KeyDicRank].ZType),
                           pLevel,' ',
                           wMod->ResultDicRank,
                           wMod->Entity->LocalMetaDic[wMod->ResultDicRank].getName().toString(),
                           pLevel,' ',
                           decode_ZType(wMod->Entity->LocalMetaDic[wMod->ResultDicRank].ZType),
                           decode_ZSTO(getZSTOFromZType(wMod->Entity->LocalMetaDic[wMod->ResultDicRank].ZType)));
        return wReturn;
    }
    default:
        wReturn.sprintf("%*cModifier <%s>",
                           pLevel,' ',
                           decode_SearchTokenType(ModifierType).toString());
        return wReturn;
    }
    return wReturn;
} // ZSearchOperandBase::_reportModifier

utf8VaryingString
ZSearchOperandBase::_reportModifierBrief()
{
    if (ModifierType==ZSRCH_NOTHING)
        return utf8VaryingString();
    utf8VaryingString wReturn ;
    switch (ModifierType)
    {
    case ZSRCH_SUBSTRING:
    {
        Mod_SubString* wMod=static_cast<Mod_SubString*>(ModParams);
        wReturn.sprintf ("%sSUBSTRING ( %d ,%d )",__MODIFIER_SEPARATOR_ASC__, wMod->Offset,wMod->Length);
        return wReturn;
    }
    case ZSRCH_SUBSTRINGLEFT:
    {
        Mod_LeftRight* wMod=static_cast<Mod_LeftRight*>(ModParams);
        wReturn.sprintf ("%sLEFT ( %d )",__MODIFIER_SEPARATOR_ASC__, wMod->Length);
        return wReturn;
    }
    case ZSRCH_SUBSTRINGRIGHT:
    {
        Mod_LeftRight* wMod=static_cast<Mod_LeftRight*>(ModParams);
        wReturn.sprintf ("%sRIGHT ( %d )",__MODIFIER_SEPARATOR_ASC__, wMod->Length);
        return wReturn;
    }

    case ZSRCH_TRANSLATE:
    {
        Mod_Translate* wMod=static_cast<Mod_Translate*>(ModParams);
        wReturn.sprintf("%sTRANSLATE ( %s , %s , %s )",
                           __MODIFIER_SEPARATOR_ASC__,
                           wMod->Entity->EntityName.toString(),
                           wMod->Entity->LocalMetaDic[wMod->KeyDicRank].getName().toString(),
                           wMod->Entity->LocalMetaDic[wMod->ResultDicRank].getName().toString());
        return wReturn;
    }

    case ZSRCH_YEAR:
        wReturn.sprintf( "%sYEAR",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    case ZSRCH_MONTH:
        wReturn.sprintf( "%sMONTH",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    case ZSRCH_DAY:
        wReturn.sprintf( "%sDAY",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    case ZSRCH_HOUR:
        wReturn.sprintf( "%sHOUR",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;;
    case ZSRCH_MIN:
        wReturn.sprintf( "%sMIN",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    case ZSRCH_SEC:
        wReturn.sprintf( "%sSEC",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;

    case ZSRCH_ENTITY:
        wReturn.sprintf( "%sENTITY",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    case ZSRCH_ID:
        wReturn.sprintf( "%sID",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;

    case ZSRCH_PATH:
        wReturn.sprintf( "%sPATH",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    case ZSRCH_ROOTNAME:
        wReturn.sprintf( "%sROOTNAME",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    case ZSRCH_BASENAME:
        wReturn.sprintf( "%sBASENAME",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;
    case ZSRCH_EXTENSION:
        wReturn.sprintf( "%sEXTENSION",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;

    default:
        wReturn.sprintf( "%sUNKNOWN",__MODIFIER_SEPARATOR_ASC__);
        return wReturn;

    }
    return wReturn;
} // ZSearchOperandBase::_reportModifierBrief

utf8VaryingString
ZSearchLogicalOperand::_reportFormula()
{
  utf8VaryingString wReturn;

  while (true) {
    if (static_cast<ZSearchOperandBase*>(Operand)->ZSTO == ZSTO_Arithmetic) {
      wReturn += static_cast<ZSearchArithmeticTerm*>(Operand)->_reportFormula();
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->ZSTO == ZSTO_Logical) {
      wReturn += static_cast<ZSearchLogicalTerm*>(Operand)->_reportFormula(true);
      break;
    }

    if (static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Field) {
      ZSearchFieldOperand* wFldData = static_cast<ZSearchFieldOperand*>( Operand);

      wReturn.addsprintf(" %s ", wFldData->FullFieldName.toString() );
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->ZSTO & ZSTO_Literal) {
      wReturn.addsprintf(" %s",_evaluateOpLiteral(Operand).toString() );
      break;
    }
    wReturn.addsprintf("type %s <Invalid type>\n",
        decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->ZSTO) );
    break;
  }// while true

  if (static_cast<ZSearchOperandBase*>(Operand)->ModifierType!=ZSRCH_NOTHING){
      wReturn+=static_cast<ZSearchOperandBase*>(Operand)->_reportModifierBrief();
  }

  if (!static_cast<ZSearchOperandBase*>(Operand)->FullFieldName.isEmpty()) {
      wReturn.addsprintf("(%s)",static_cast<ZSearchOperandBase*>(Operand)->FullFieldName.toString());
  }
  wReturn += " ";

  return wReturn;
} // ZSearchLogicalOperand::_report


utf8VaryingString
ZSearchArithmeticTerm::_report(int pLevel)
{
  utf8VaryingString wReturn;
  ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(Operand);

  wReturn.sprintf("%*cArithmetic operand Parenthesis level %d collateral %d\n",pLevel,' ',ParenthesisLevel);

  wReturn.addsprintf("%*cOperand\n",pLevel,' ');
  while (true) {
    if (wOpBase->ZSTO == ZSTO_Arithmetic) {
      wReturn += static_cast<ZSearchArithmeticTerm*>(Operand)->_report(pLevel+1);
      break;
    }

    if (wOpBase->ZSTO & ZSTO_Field) {
      ZSearchFieldOperand* wFldData = static_cast<ZSearchFieldOperand*>( Operand);
      wReturn.addsprintf("%*c type %s full field name <%s> type <%s>\n",
          pLevel,' ',
          decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->ZSTO),
          wFldData->FullFieldName.toString(),
                         decode_ZSearchType(wFldData->ZSTO)
          );
      break;
    }
    if (wOpBase->ZSTO & ZSTO_Literal) {
      wReturn.addsprintf("%*c type %s value <%s>\n",
          pLevel,' ',
          decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->ZSTO),
          ZSearchLogicalOperand::_evaluateOpLiteral(Operand).toString()
          );
      break;
    }
    wReturn.addsprintf("%*c type %s <Invalid type>\n",
        pLevel,' ',
        decode_OperandType(wOpBase->ZSTO)
        );
    break;
/*
    if (static_cast<ZSearchOperandBase*>(Operand)->TokenList.count()>0) {
      wReturn.addsprintf("%*c Tokens <",pLevel,' ');
      for (int wj=0; wj < static_cast<ZSearchOperandBase*>(Operand)->TokenList.count();wj++) {
        wReturn.addsprintf("%s",static_cast<ZSearchOperandBase*>(Operand)->TokenList[wj]->Text.toString());
      }
      wReturn += ">\n";
    } else {
      wReturn.addsprintf("%*c No Token for this operand\n",pLevel,' ');
    }
*/
    //    setMaxSprintfBufferCount(4096);

    if (wOpBase->ModifierType!=ZSRCH_NOTHING){
      wReturn += wOpBase->_reportModifier(pLevel);
      wReturn += "\n";
    }
  }// while true

  wReturn.addsprintf("%*cOperator\n",pLevel,' ');
  wReturn += Operator._report(pLevel+1);

  if (OperandNext==nullptr) {
    wReturn.addsprintf("%*cNext arithmetic operand is <nullptr>\n",pLevel,' ');
    return wReturn;
  }

  wReturn += static_cast<ZSearchArithmeticTerm*>(OperandNext)->_report(pLevel);
  return wReturn;
} // ZSearchArithmeticTerm::_report

utf8VaryingString
ZSearchArithmeticTerm::_reportFormula(bool pDetailed)
{
  utf8VaryingString wReturn;

  while (true) {
    if (static_cast<ZSearchOperandBase*>(Operand)->ZSTO == ZSTO_Arithmetic) {
      wReturn += " ( ";
      wReturn = static_cast<ZSearchArithmeticTerm*>(Operand)->_reportFormula(pDetailed);
      wReturn += " ) ";
      break;
    }

    wReturn += OperandReportFormula(Operand);
    break;
/*
    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Field) {
      ZSearchFieldOperandOwnData* wFldOwnData = static_cast<ZSearchFieldOperandOwnData*>( Operand);
      wReturn.addsprintf(" %s", wFldOwnData->FullFieldName.toString() );
      break;
    }
    if (static_cast<ZSearchOperandBase*>(Operand)->Type & ZSTO_Literal) {
      wReturn.addsprintf(" %s", ZSearchLogicalOperand::_evaluateOpLiteral(Operand).toString() );
      break;
    }

    wReturn.addsprintf("%s <Invalid type> ", decode_OperandType(static_cast<ZSearchOperandBase*>(Operand)->Type) );
    break;
*/

  }// while true

  if (static_cast<ZSearchOperandBase*>(Operand)->ModifierType!=ZSRCH_NOTHING){
      wReturn += static_cast<ZSearchOperandBase*>(Operand)->_reportModifierBrief();
  }

  wReturn += " ";

  wReturn += Operator._reportFormula();


  if (OperandNext==nullptr) {
    if (pDetailed)
      wReturn += "<nullptr>";
    return wReturn;
  }

  wReturn += static_cast<ZSearchArithmeticTerm*>(OperandNext)->_reportFormula(pDetailed);
  return wReturn;
} // ZSearchArithmeticTerm::_report



ZSearchArithmeticTerm::~ZSearchArithmeticTerm()
{

  clearOperand(OperandNext);
  clearOperand(Operand);
}


/*----------------------Evaluate operands' content routines -------------------*/

/*
utf8VaryingString
displayOperandContent(ZOperandContent* pContent) {
  utf8VaryingString wReturn;
  switch (pContent->OperandZSTO)
  {
  case ZSTO_Integer:
      wReturn.sprintf("%ld",pContent->getInteger());

  case ZSTO_Float:
      wReturn.sprintf("%g",pContent->getFloat());

  case ZSTO_String:
      return pContent->getString();
  case ZSTO_UriString:
      return pContent->getURI();

  case ZSTO_FieldBool:
      return pContent->getBool()?"TRUE":"FALSE";

  case ZSTO_Date:
      return pContent->getDate().toFormatted();

  case ZSTO_Resource:
      return pContent->getResource().toHexa();

  case ZSTO_Checksum:
      return pContent->getChecksum().toHexa();
  }
  return wReturn;
}
*/

ZStatus setStringOperandModifierValue(utf8VaryingString &pRawValue,
                              ZSearchTokentype_type pModifier,
                              void*                 pModParams,
                              ZOperandContent* pOperandContent)
{
    ZStatus wSt=ZS_SUCCESS;
    pOperandContent->OperandZSTO = ZSTO_String;
    switch (pModifier) /* NB: Content ZType remains the same */
    {
    case ZSRCH_NOTHING:  /* No modifier */
    case ZSRCH_TRANSLATE: /* if translate, then source value must be set to current operand */
    {
        wSt=pOperandContent->setString ( pRawValue);
        break;
    }

    case ZSRCH_SUBSTRING:
    {
        Mod_SubString* wMP=static_cast<Mod_SubString*>(pModParams);
        wSt=pOperandContent->setString ( pRawValue.subString(wMP->Offset,wMP->Length) );
        break;
    }
    case ZSRCH_SUBSTRINGRIGHT:
    {
        Mod_LeftRight* wMP=static_cast<Mod_LeftRight*>(pModParams);
        wSt=pOperandContent->setString ( pRawValue.Right( wMP->Length) );
        break;
    }
    case ZSRCH_SUBSTRINGLEFT:
    {
        Mod_LeftRight* wMP=static_cast<Mod_LeftRight*>(pModParams);
        wSt=pOperandContent->setString ( pRawValue.Left( wMP->Length) );
        break;
    }

    default:
        return ZS_INVTYPE;
    }// switch pModifier

    return wSt ;
}// setStringOperandModifierValue


ZStatus getURFOperandContentWModifier(ZSearchEntityContext &pSEC,
                                      URFField &wField,
                                        ZSearchTokentype_type pModifier,
                                        void*                 pModParams,
                                        ZOperandContent* pOperandContent)
{
  ZStatus wSt=ZS_SUCCESS;
  pOperandContent->clear();

  switch (wField.ZType) {
  case ZType_AtomicChar:
  case ZType_Char:
  {
    char wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<char>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));
    break;
  }
  case ZType_UChar:
  case ZType_AtomicUChar:
  {
    unsigned char wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<unsigned char>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));
    break;
  }
  case ZType_S8:
  case ZType_AtomicS8:
  {
    int8_t wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<int8_t>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));
    break;
  }
  case ZType_U8:
  case ZType_AtomicU8:
  {
    uint8_t wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<uint8_t>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));
    break;
  }
  case ZType_S16:
  case ZType_AtomicS16:
  {
    int16_t wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<int16_t>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));

    break;
  }
  case ZType_U16:
  case ZType_AtomicU16:
  {
    uint16_t wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<uint16_t>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));

    break;
  }
  case ZType_S32:
  case ZType_AtomicS32:
  {
    int32_t wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<int32_t>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));

    break;
  }
  case ZType_U32:
  case ZType_AtomicU32:
  {
    uint32_t wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<uint32_t>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));

    break;
  }
  case ZType_S64:
  case ZType_AtomicS64:
  {
    int64_t wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<int64_t>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));

    break;
  }
  case ZType_U64:
  case ZType_AtomicU64:
  {
    uint64_t wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<uint64_t>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Integer;
    wSt=pOperandContent->setInteger (long(wVal));

    break;
  }
  case ZType_Float:
  case ZType_AtomicFloat:
  {
    float wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<float>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Float;
    wSt=pOperandContent->setFloat (double(wVal));

    break;
  }
  case ZType_Double:
  case ZType_AtomicDouble:
  {
    double wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<double>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Float;
    wSt=pOperandContent->setFloat (double(wVal));

    break;
  }
  case ZType_LDouble:
  case ZType_AtomicLDouble:
  {
    long double wVal;
    wField.FieldPtr += sizeof(ZTypeBase);
    _importAtomic<long double>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Float;
    wSt=pOperandContent->setFloat (double(wVal));
    break;
  }
  case ZType_Bool:
  {
    bool wVal;
    _importAtomic<bool>(wVal,wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Bool;
    wSt=pOperandContent->setBool ( bool(wVal));

    break;
  }

  case ZType_Utf8VaryingString:
  {
    utf8VaryingString wVal;
    wVal._importURF(wField.FieldPtr);

    wSt= setStringOperandModifierValue(wVal,pModifier,pModParams,pOperandContent);
    if (wSt==ZS_INVTYPE) {
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      pModifier,
                      decode_SearchTokenType(pModifier),
                      wField.ZType,
                      decode_ZType(wField.ZType))
            return ZS_INVTYPE;
    }

    break;
  } // ZType_Utf8VaryingString

  case ZType_Utf16VaryingString:
  {
    utf16VaryingString wVal1;
    utf8VaryingString wVal;
    wVal1._importURF(wField.FieldPtr);
    wVal.fromUtf16( wVal1);
    wSt= setStringOperandModifierValue(wVal,pModifier,pModParams,pOperandContent);
    if (wSt==ZS_INVTYPE) {
        _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                  _GET_FUNCTION_NAME_,
                  pModifier,
                  decode_SearchTokenType(pModifier),
                  wField.ZType,
                  decode_ZType(wField.ZType))
        return ZS_INVTYPE;
    }
    break;
  }
  case ZType_Utf32VaryingString: {
    utf32VaryingString wVal1;
    utf8VaryingString wVal;
    wVal1._importURF(wField.FieldPtr);
    wVal.fromUtf32( wVal1);
    wSt= setStringOperandModifierValue(wVal,pModifier,pModParams,pOperandContent);
    if (wSt==ZS_INVTYPE) {
        _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                  _GET_FUNCTION_NAME_,
                  pModifier,
                  decode_SearchTokenType(pModifier),
                  wField.ZType,
                  decode_ZType(wField.ZType))
        return ZS_INVTYPE;
    }
    break;
  }

  case ZType_ZDateFull: {
      ZDateFull wDate;
    wDate._importURF(wField.FieldPtr);

    switch (pModifier) /* NB: Content Type is changed to integer */
    {
        case ZSRCH_NOTHING:
        case ZSRCH_TRANSLATE: /* if translate, then source value must be set to current operand */
        {
            pOperandContent->OperandZSTO = ZSTO_Date;
            wSt=pOperandContent->setDate ( wDate);

            break;
        }
        case ZSRCH_YEAR:
        {
            pOperandContent->OperandZSTO = ZSTO_Integer;
            wSt=pOperandContent->setInteger ( wDate.year());

            break;
        }
        case ZSRCH_MONTH:
        {
            pOperandContent->OperandZSTO = ZSTO_Integer;
            wSt=pOperandContent->setInteger ( wDate.month());

            break;
        }
        case ZSRCH_DAY:
        {
            pOperandContent->OperandZSTO = ZSTO_Integer;
            wSt=pOperandContent->setInteger ( wDate.day());

            break;
        }
        case ZSRCH_HOUR:
        {
            pOperandContent->OperandZSTO = ZSTO_Integer;
            wSt=pOperandContent->setInteger ( wDate.hour());

            break;
        }
        case ZSRCH_MIN:
        {
            pOperandContent->OperandZSTO = ZSTO_Integer;
            wSt=pOperandContent->setInteger ( wDate.min());

            break;
        }
        case ZSRCH_SEC:
        {
            pOperandContent->OperandZSTO= ZSTO_Integer;
            wSt=pOperandContent->setInteger ( wDate.sec());

            break;
        }
        default:
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      pModifier,
                      decode_SearchTokenType(pModifier),
                      wField.ZType,
                      decode_ZType(wField.ZType))
            return ZS_INVTYPE;
    }// switch pModifier
    break;
  } // ZType_ZDateFull

  case ZType_Resource: {
      ZResource wResource;
      wResource._importURF(wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Resource;
    switch (pModifier) /* NB: Content ZType is changed to integer */
    {
        case ZSRCH_NOTHING:
        case ZSRCH_TRANSLATE: /* if translate, then source value must be set to current operand */
        {
            pOperandContent->OperandZSTO = ZSTO_Resource;
            wSt=pOperandContent->setResource ( wResource );

            break;
        }
        case ZSRCH_ZENTITY:
        {
            pOperandContent->OperandZSTO = ZSTO_Integer;
            wSt=pOperandContent->setInteger ( wResource.Entity );

            break;
        }
        case ZSRCH_ID:
        {
            pOperandContent->OperandZSTO = ZSTO_Integer;
            wSt=pOperandContent->setInteger ( wResource.id );

            break;
        }
        default:
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      pModifier,
                      decode_SearchTokenType(pModifier),
                      wField.ZType,
                      decode_ZType(wField.ZType))
            return ZS_INVTYPE;
    }// switch pModifier
    break;
  } // ZType_Resource

  case ZType_CheckSum: {
    checkSum wCheckSum ;
    wCheckSum._importURF(wField.FieldPtr);
    pOperandContent->OperandZSTO = ZSTO_Checksum;
    wSt=pOperandContent->setChecksum( wCheckSum );

    break;
  }
  case ZType_URIString: {
    uriString wURI;
    wURI._importURF(wField.FieldPtr);

    switch (pModifier) /* NB: Content ZType remains the same */
    {
        case ZSRCH_NOTHING:  /* No modifier */
        case ZSRCH_TRANSLATE: /* if translate, then source value must be set to current operand */
        {
            pOperandContent->OperandZSTO = ZSTO_UriString;
            wSt=pOperandContent->setURI ( wURI);

            break;
        }
        case ZSRCH_PATH:
        {
            pOperandContent->OperandZSTO = ZSTO_UriString;
            wSt=pOperandContent->setURI ( wURI.getDirectoryPath());

            break;
        }
        case ZSRCH_EXTENSION:
        {
            pOperandContent->OperandZSTO = ZSTO_String;
            wSt=pOperandContent->setString ( wURI.getFileExtension());

            break;
        }
        case ZSRCH_BASENAME:
        {
            pOperandContent->OperandZSTO = ZSTO_String;
            wSt=pOperandContent->setString ( wURI.getBasename());

            break;
        }
        case ZSRCH_ROOTNAME:
        {
            pOperandContent->OperandZSTO = ZSTO_String;
            wSt=pOperandContent->setString ( wURI.getRootname());

            break;
        }
        default:
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      pModifier,
                      decode_SearchTokenType(pModifier),
                      wField.ZType,
                      decode_ZType(wField.ZType))
            return ZS_INVTYPE;
    }// switch pModifier
    break;
  } // ZType_URIString

  default:
    _DBGPRINT("Cannot convert data type %s - data type not supported.",decode_ZType(wField.ZType))
    return ZS_INVTYPE;
  }//switch

  if (pModifier==ZSRCH_TRANSLATE) {
      ZArray<URFField> wFieldList;
      ZDataBuffer wRecord;
      Mod_Translate* wMD=static_cast<Mod_Translate*>(pModParams);
      ZStatus wSt=ZS_SUCCESS;
      wSt=wMD->Entity->findFirstFieldValueSequential(pSEC,wMD->KeyDicRank,pOperandContent,wRecord);
      if (wSt==ZS_FOUND) {

        return getURFOperandContentWModifier(pSEC,pSEC._URFParser.URFFieldList[wMD->ResultDicRank],ZSRCH_NOTHING,nullptr, pOperandContent);
      }
      return wSt;
  } // ZSRCH_DECODE



  _DBGPRINT("getFieldOperandContent field type <%s> operand type<%s> value <%s>\n",
            decode_ZType(wField.ZType),decode_OperandType(pOperandContent->OperandZSTO),pOperandContent->display().toString())
//      decode_ZType(wField.ZType),decode_OperandType(pOperandContent->OperandZSTO),displayOperandContent(pOperandContent).toString())

  return wSt;
} // getFieldOperandContentWModifier

ZStatus getURFOperandContent(URFField &wField, ZOperandContent* pOperandContent)
{
    ZStatus wSt=ZS_SUCCESS;
    pOperandContent->clear();

    switch (wField.ZType) {
    case ZType_AtomicChar:
    case ZType_Char:
    {
        char wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<char>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_UChar:
    case ZType_AtomicUChar:
    {
        unsigned char wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<unsigned char>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_S8:
    case ZType_AtomicS8:
    {
        int8_t wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<int8_t>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_U8:
    case ZType_AtomicU8:
    {
        uint8_t wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<uint8_t>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_S16:
    case ZType_AtomicS16:
    {
        int16_t wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<int16_t>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_U16:
    case ZType_AtomicU16:
    {
        uint16_t wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<uint16_t>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_S32:
    case ZType_AtomicS32:
    {
        int32_t wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<int32_t>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_U32:
    case ZType_AtomicU32:
    {
        uint32_t wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<uint32_t>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_S64:
    case ZType_AtomicS64:
    {
        int64_t wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<int64_t>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_U64:
    case ZType_AtomicU64:
    {
        uint64_t wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<uint64_t>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Integer;
        wSt=pOperandContent->setInteger ( long(wVal));

        break;
    }
    case ZType_Float:
    case ZType_AtomicFloat:
    {
        float wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<float>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Float;
        wSt=pOperandContent->setFloat ( double(wVal));

        break;
    }
    case ZType_Double:
    case ZType_AtomicDouble:
    {
        double wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<double>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Float;
        wSt=pOperandContent->setFloat ( double(wVal));

        break;
    }
    case ZType_LDouble:
    case ZType_AtomicLDouble:
    {
        long double wVal;
        wField.FieldPtr += sizeof(ZTypeBase);
        _importAtomic<long double>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Float;
        wSt=pOperandContent->setFloat ( double(wVal));

        break;
    }
    case ZType_Bool:
    {
        bool wVal;
        _importAtomic<bool>(wVal,wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Bool;
        wSt=pOperandContent->setBool ( bool(wVal));

        break;
    }


    case ZType_Utf8VaryingString: {
        utf8VaryingString wVal;
        wVal._importURF(wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_String;
        wSt=pOperandContent->setString (wVal);

        break;
    }
    case ZType_Utf16VaryingString: {
        utf16VaryingString wVal1;
        utf8VaryingString wVal;
        wVal1._importURF(wField.FieldPtr);
        wVal.fromUtf16( wVal1);
        pOperandContent->OperandZSTO = ZSTO_String;
        wSt=pOperandContent->setString (wVal);

        break;
    }
    case ZType_Utf32VaryingString: {
        utf32VaryingString wVal1;
        utf8VaryingString wVal;
        wVal1._importURF(wField.FieldPtr);
        wVal.fromUtf32( wVal1);
        pOperandContent->OperandZSTO = ZSTO_String;
        wSt=pOperandContent->setString (wVal);

        break;
    }

    case ZType_ZDateFull: {
        ZDateFull wDate;
        wDate._importURF(wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Date;
        wSt=pOperandContent->setDate (wDate);

        break;
    }

    case ZType_Resource: {
        ZResource wResource;
        wResource._importURF(wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Resource;
        wSt=pOperandContent->setResource (wResource);

        break;
    }
    case ZType_CheckSum: {
        checkSum wCheckSum;
        wCheckSum._importURF(wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_Checksum;
        wSt=pOperandContent->setChecksum (wCheckSum);

        break;
    }
    case ZType_URIString: {
        uriString wVal;
        wVal._importURF(wField.FieldPtr);
        pOperandContent->OperandZSTO = ZSTO_UriString;
        wSt=pOperandContent->setURI (wVal);

        break;
    }
    default:
        _DBGPRINT("Cannot convert data type %s - data type not supported.",decode_ZType(wField.ZType))
        return ZS_INVTYPE;
    }//switch


    _DBGPRINT("getFieldOperandContent field type <%s> operand type<%s> value <%s>\n",
              decode_ZType(wField.ZType),decode_OperandType(pOperandContent->OperandZSTO),pOperandContent->display().toString())

    return wSt;
} // getFieldOperandContent

ZStatus getLiteralOperandContent(void *pLiteral, ZOperandContent* pOperandContent)
{
//  ZOperandContent pOpContent;
    pOperandContent->clear();
  ZSearchOperandBase* wOpBase  = static_cast<ZSearchOperandBase*>(pLiteral);

  if (!(wOpBase->ZSTO & ZSTO_Literal) ) {
    _DBGPRINT("getLiteralOperandContent-E-INVTYP Operand type is not literal.\n")
    return ZS_INVTYPE;
  }

  int wOpType = wOpBase->ZSTO & ZSTO_BaseMask;
  pOperandContent->OperandZSTO = ZSearchOperandType(wOpType);
  switch (pOperandContent->OperandZSTO) {
  case ZSTO_String: {
      pOperandContent->setString( (static_cast<ZSearchLiteral*>(pLiteral)->getString()));
    pOperandContent->OperandZSTO = ZSTO_String;
    break;
  }
  case ZSTO_UriString: {
      pOperandContent->setURI (static_cast<ZSearchLiteral*>(pLiteral)->getURI());
    pOperandContent->OperandZSTO = ZSTO_UriString;
    break;
  }
  case ZSTO_Integer: {
      pOperandContent->setInteger ( static_cast<ZSearchLiteral*>(pLiteral)->getInteger());
    pOperandContent->OperandZSTO = ZSTO_Integer;
    break;
  }
  case ZSTO_Float: {
      pOperandContent->setFloat ( static_cast<ZSearchLiteral*>(pLiteral)->getFloat());
    pOperandContent->OperandZSTO = ZSTO_Float;
    break;
  }
  case ZSTO_Date: {
      pOperandContent->setDate ( static_cast<ZSearchLiteral*>(pLiteral)->getDate());
    pOperandContent->OperandZSTO = ZSTO_Date;
    break;
  }
  case ZSTO_Resource: {
      pOperandContent->setResource ( static_cast<ZSearchLiteral*>(pLiteral)->getResource());
    pOperandContent->OperandZSTO = ZSTO_Resource;
    break;
  }
  case ZSTO_Checksum: {
      pOperandContent->setChecksum ( static_cast<ZSearchLiteral*>(pLiteral)->getChecksum());
    pOperandContent->OperandZSTO = ZSTO_Checksum;
    break;
  }
  case ZSTO_Bool: {
      pOperandContent->setBool ( static_cast<ZSearchLiteral*>(pLiteral)->getBool());
    pOperandContent->OperandZSTO = ZSTO_Bool;
    break;
  }
  default:
    _DBGPRINT("getLiteralOperandContent-E-INVTYP Invalid literal operand type<%s> <0x%X>\n",
        decode_OperandType(wOpType),wOpType)

    return ZS_INVTYPE;
  }//switch

  _DBGPRINT("getLiteralOperandContent literal operand type<%s> value <%s>\n",
      decode_OperandType(pOperandContent->OperandZSTO),pOperandContent->display().toString())

  return ZS_SUCCESS;
} // getLiteralOperandContent

//ZStatus extractOperandContent (void* pOperand,URFParser& pURFParser,ZOperandContent* pOperandContent)
ZStatus extractOperandContent(ZSearchEntityContext &pSEC,
                              void *pOperand,
                              ZOperandContent *pOperandContent)

{
//  URFParser wURFParser(pRecord);

  ZStatus wSt=ZS_SUCCESS;
  URFField wField;
  pOperandContent->clear();

  URFParser* wParser=nullptr;
/*
  if (pURFParser.URFPS < URFPS_TotallyParsed) {
      _DBGPRINT("extractOperandContent-E-NOTPARSED Given parser is not up to date while parsing field rank <%d> name <%s>.\n",
                wOpFieldData->MetaDicRank,wOpFieldData->FullFieldName.toCChar(),decode_ZStatus(wSt))
      abort();
  }
*/
//  ZOperandContent wContent ;
  ZSearchOperandBase* wOpBase=static_cast<ZSearchOperandBase*>(pOperand);
  if (wOpBase->ZSTO & ZSTO_Field) {
    ZSearchFieldOperand* wOpFieldData=static_cast<ZSearchFieldOperand*>(pOperand);
    wParser=pSEC.getParserFromEntity(wOpFieldData->Entity);

    wSt = wParser->_getURFFieldByRank(wOpFieldData->MetaDicRank,wField );
    if (wSt!=ZS_SUCCESS) {
        _DBGPRINT("gettermOperandContent-E-NOTFND Field rank <%d> name <%s> has not been found by URFParser with status <%s>\n",
                  wOpFieldData->MetaDicRank,wOpFieldData->FullFieldName.toCChar(),decode_ZStatus(wSt))
        return ZS_MISS_FIELD;
    }



    if (!wField.Present) {
        _DBGPRINT("gettermOperandContent-E-MISSFLD Field rank <%d> name <%s> is declared missing by URFParser.\n",
                  wOpFieldData->MetaDicRank,wOpFieldData->FullFieldName.toCChar())
        return ZS_MISS_FIELD;
    }
    /* here field has been extracted from record according its position
     *  VERIFY it is the correct field (ZType).
     *  If not, then dictionary is not adapted to record
     */
    if (wField.ZType != wOpFieldData->getZType()) {
        _DBGPRINT("gettermOperandContent Field extracted from record at rank %ld has type <%s> while dictionary mentions <%s>.\n"
                 "                      Either dictionary is invalid or Record is corrupted.",
                  wOpFieldData->MetaDicRank,decode_ZType(wField.ZType),decode_ZType(wOpFieldData->getZType()));
        return ZS_CORRUPTED;
    }

    wSt = getURFOperandContent (wField,pOperandContent);
  } // ZSTO_Field
  else {
     wSt = getLiteralOperandContent( pOperand,pOperandContent);
  } // ZSTO_Literal

  if (wOpBase->ModifierType==ZSRCH_NOTHING)
    return ZS_SUCCESS;


  switch(wOpBase->ModifierType) {
    /* uristring modifiers */
  case ZSRCH_PATH:
      pOperandContent->replaceWithURI ( uriString(pOperandContent->getURI().getDirectoryPath()));

    break;
  case ZSRCH_EXTENSION:
      pOperandContent->replaceWithString ( pOperandContent->getURI().getFileExtension());

    break;
  case ZSRCH_BASENAME:
      pOperandContent->replaceWithString ( pOperandContent->getURI().getBasename());
    break;
  case ZSRCH_ROOTNAME:
      pOperandContent->replaceWithString ( pOperandContent->getURI().getRootname());
    break;

    /* Date modifier */
  case ZSRCH_YEAR:
      pOperandContent->replaceWithInteger ( pOperandContent->getDate().year());
    break;
  case ZSRCH_MONTH:
      pOperandContent->replaceWithInteger ( pOperandContent->getDate().month());
    pOperandContent->OperandZSTO = ZSTO_Integer;
    break;
  case ZSRCH_DAY:
    pOperandContent->replaceWithInteger ( pOperandContent->getDate().day());
    break;
  case ZSRCH_HOUR:
      pOperandContent->replaceWithInteger (pOperandContent->getDate().hour());
    break;
  case ZSRCH_MIN:
      pOperandContent->replaceWithInteger ( pOperandContent->getDate().min());
    break;
  case ZSRCH_SEC:
      pOperandContent->replaceWithInteger (pOperandContent->getDate().sec());
    break;

    /* resource modifier */
  case ZSRCH_ZENTITY:
      pOperandContent->replaceWithInteger (pOperandContent->getResource().Entity);
    break;
  case ZSRCH_ID:
      pOperandContent->replaceWithInteger ( pOperandContent->getResource().id);
    break;
  }

  return ZS_SUCCESS;
} // gettermOperandContent

ZStatus
evaluateTermString(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZSearchOperandType_type wType = pOp2.OperandZSTO & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
    break;
  case ZSTO_UriString:
  {
      utf8VaryingString wD = pOp2.getURI();
      pOp2.setString (wD );
    break;
  }
  case ZSTO_Bool:
  {
      utf8VaryingString wD = pOp2.getBool()?"TRUE":"FALSE";
      pOp2.setString(wD);
    break;
  }
  case ZSTO_Integer:
  {
      utf8VaryingString wD;
      wD.sprintf("%ld",pOp2.getInteger());
      pOp2.setString(wD);
    break;
  }
  case ZSTO_Float:
  {
      utf8VaryingString wD;
      wD.sprintf("%g",pOp2.getFloat());
      pOp2.setString(wD);
      break;
  }
  case ZSTO_Date:
  {
      utf8VaryingString wD;
      wD=pOp2.getDate().toUTCGMT();
      pOp2.setString(wD);
      break;
  }
  case ZSTO_Checksum:
  {
      utf8VaryingString wD;
      wD=pOp2.getChecksum().toHexa();
      pOp2.setString(wD);
      break;
  }
  case ZSTO_Resource:
  {
      utf8VaryingString wD;
      wD=pOp2.getResource().toHexa();
      pOp2.setString(wD);
      break;
  }
  } // switch

  switch (pOperator.ZSOPV)
  {
  case ZSOPV_EQUAL:
      pOutResult = pOp1.getString().compare(pOp2.getString())==0;
    break;
  case ZSOPV_NOTEQUAL:
    pOutResult = pOp1.getString().compare(pOp2.getString())!=0;
    break;
  case ZSOPV_LESS:
      pOutResult = pOp1.getString() .compare(pOp2.getString())<0;
    break;
  case ZSOPV_GREATER:
    pOutResult = pOp1.getString().compare(pOp2.getString())>0;
    break;
  case ZSOPV_LESSEQUAL:
    pOutResult = pOp1.getString().compare(pOp2.getString())<=0;
    break;
  case ZSOPV_GREATEREQUAL:
    pOutResult = pOp1.getString().compare(pOp2.getString())>=0;
    break;

  case ZSOPV_CONTAINS:
    pOutResult = pOp1.getString().contains(pOp2.getString().toString());
    break;
  case ZSOPV_STARTS_WITH:
    pOutResult = pOp1.getString().startsWith(pOp2.getString().toString());
    break;
  case ZSOPV_ENDS_WITH:
    pOutResult = pOp1.getString().endsWith(pOp2.getString().toString());
    break;
  default:
    pOutResult = false;
    _DBGPRINT("evaluateTermString Invalid operator for type string logical compare operator %s\n",decode_ZSOPV(pOperator.ZSOPV))
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

ZStatus
evaluateLogicalTermInteger(bool & pOutReturn,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZSearchOperandType_type wType = pOp2.OperandZSTO & ZSTO_BaseMask;
    long wIntOp2=0;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
      wIntOp2= pOp2.getString().toLong();
    break;
  case ZSTO_Integer:
      wIntOp2=pOp2.getInteger();
    break;
  case ZSTO_Float:
      wIntOp2= long(pOp2.getFloat());
    break;

  case ZSTO_Bool:
  case ZSTO_UriString:
  case ZSTO_Date:
  case ZSTO_Checksum:
    _DBGPRINT("evaluateTermInteger Cannot compare integer (operand 1) with %s (operand 2) invalid result",
        decode_OperandType(wType))
    return ZS_INVOP;
  } // switch

  switch (pOperator.ZSOPV)
  {
  case ZSOPV_EQUAL:
      pOutReturn = (pOp1.getInteger() == wIntOp2 ) ;
    break;
  case ZSOPV_NOTEQUAL:
    pOutReturn = ( pOp1.getInteger() != wIntOp2) ;
    break;
  case ZSOPV_LESS:
    pOutReturn = ( pOp1.getInteger() < wIntOp2);
    break;
  case ZSOPV_GREATER:
    pOutReturn = ( pOp1.getInteger() > wIntOp2);
    break;
  case ZSOPV_LESSEQUAL:
    pOutReturn = ( pOp1.getInteger() <= wIntOp2);
    break;
  case ZSOPV_GREATEREQUAL:
    pOutReturn = ( pOp1.getInteger() >= wIntOp2);
    break;
  default:
    _DBGPRINT("evaluateTermInteger Invalid operator for integer logical compare operation %s\n",decode_ZSOPV(pOperator.ZSOPV))
    pOutReturn = false;
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
} //evaluateLogicalTermInteger


#ifdef __COMMENT__ see ZSearchArithmeticTerm::computeArithmeticOperation()
ZStatus
evaluateArithmeticTermInteger(ZOperandContent& pOutReturn,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
    ZSearchOperandType_type wType = pOp2.ZSTO & ZSTO_BaseMask;
    long wIntOp2=0;
    /* manage possible conversions */
    switch (wType)
    {
    case ZSTO_String:
        wIntOp2= pOp2.getString().toLong();
        break;
    case ZSTO_Integer:
        wIntOp2=pOp2.getInteger();
        break;
    case ZSTO_Float:
        wIntOp2= long(pOp2.getFloat());
        break;

    case ZSTO_Bool:
    case ZSTO_UriString:
    case ZSTO_Date:
    case ZSTO_Checksum:
        _DBGPRINT("evaluateTermInteger Cannot compare integer (operand 1) with %s (operand 2) invalid result",
                  decode_OperandType(wType))
        return ZS_INVOP;
    } // switch

    switch (pOperator.ZSOPV)
    {
    case ZSOPV_PLUS:
        pOutReturn.replaceWithInteger( pOp1.getInteger() + wIntOp2 ) ;
        break;
    case ZSOPV_MINUS:
        pOutReturn.replaceWithInteger( pOp1.getInteger() - wIntOp2 ) ;
        break;
    case ZSOPV_MULTIPLY:
         pOutReturn.replaceWithInteger( pOp1.getInteger() * wIntOp2 ) ;
        break;
    case ZSOPV_DIVIDE:
        pOutReturn = ( pOp1.getInteger() / wIntOp2);
        break;
    case ZSOPV_POWER:
        pOutReturn = ( pOp1.getInteger() ^ wIntOp2);
        break;
    case ZSOPV_MODULO:
        pOutReturn = ( pOp1.getInteger() % wIntOp2);
        break;
    default:
        _DBGPRINT("evaluateTermInteger Invalid operator for integer logical compare operation %s\n",decode_ZSOPV(pOperator.ZSOPV))
        pOutReturn = false;
        return ZS_INV_OPERATOR;
    }
    return ZS_SUCCESS;
} //evaluateLogicalTermInteger
#endif // __COMMENT__

bool approximatelyEqual(double a, double b)
{
  return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * std::numeric_limits<double>::epsilon());
}

ZStatus
evaluateTermFloat(bool & pOutReturn,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZSearchOperandType_type wType = pOp2.OperandZSTO & ZSTO_BaseMask;
    double wFloatOp2=0.0;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
      wFloatOp2 = pOp2.getString().toDouble();
    break;
  case ZSTO_Float:
      wFloatOp2 = pOp2.getFloat();
    break;
  case ZSTO_Integer:
      wFloatOp2 = double(pOp2.getInteger());
    break;

  case ZSTO_Bool:
  case ZSTO_UriString:
  case ZSTO_Date:
  case ZSTO_Checksum:
    _DBGPRINT("evaluateTermFloat Cannot compare double (operand 1) with %s (operand 2) invalid result",
        decode_OperandType(wType))
    return ZS_INVOP;
  } // switch

  switch (pOperator.ZSOPV)
  {
  case ZSOPV_EQUAL:
      pOutReturn = approximatelyEqual(pOp1.getFloat(), wFloatOp2) ;
    break;
  case ZSOPV_NOTEQUAL:
    pOutReturn = ! approximatelyEqual(pOp1.getFloat(), wFloatOp2);
    break;
  case ZSOPV_LESS:
    pOutReturn = ( pOp1.getFloat() < wFloatOp2);
    break;
  case ZSOPV_GREATER:
    pOutReturn = ( pOp1.getFloat() > wFloatOp2);
    break;
  case ZSOPV_LESSEQUAL:
    pOutReturn = ( pOp1.getFloat() < wFloatOp2) || approximatelyEqual(pOp1.getFloat(), wFloatOp2);
    break;
  case ZSOPV_GREATEREQUAL:
    pOutReturn = ( pOp1.getFloat() > wFloatOp2) || approximatelyEqual(pOp1.getFloat(), wFloatOp2);
    break;
  default:
    _DBGPRINT("evaluateTermFloat Invalid operator for type double logical compare %s\n",decode_ZSOPV(pOperator.ZSOPV))
    pOutReturn = false;
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

ZStatus
evaluateTermDate(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZDatePrecision wDP=ZDTPR_nano;

  ZDateFull wDateOp2;

  ZSearchOperandType_type wType = pOp2.OperandZSTO & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
      wDateOp2 = ZDateFull::fromString(pOp2.getString());
    break;
  case ZSTO_UriString:
  case ZSTO_Bool:
  case ZSTO_Integer:
  case ZSTO_Float:
  case ZSTO_Checksum:
    _DBGPRINT("evaluateTermDate Cannot compare tyoe ZDate (operand 1) with %s (operand 2) result set to invalid\n",
        decode_OperandType(pOp2.OperandZSTO))
    return ZS_INVOP;

  case ZSTO_Date:
      wDateOp2 = pOp2.getDate();
    break;
  } // switch

  switch (pOperator.ZSOPV)
  {
  case ZSOPV_EQUAL:

//    wDP = std::min(pOp1.getDate().Precision,pOp2.Date.Precision);
    wDP = pOp1.getDate().Precision;

    pOutResult = pOp1.getDate()._Compare(wDateOp2,wDP)==0;
    break;
  case ZSOPV_NOTEQUAL:
    pOutResult = pOp1.getDate()._Compare(wDateOp2,wDP)!=0;
    break;
  case ZSOPV_LESS:
    pOutResult = pOp1.getDate()._Compare(wDateOp2,wDP) < 0;
    break;
  case ZSOPV_GREATER:
    pOutResult = pOp1.getDate()._Compare(wDateOp2,wDP) > 0;
    break;
  case ZSOPV_LESSEQUAL:
    pOutResult = pOp1.getDate()._Compare(wDateOp2,wDP)<=0;
    break;
  case ZSOPV_GREATEREQUAL:
    pOutResult = pOp1.getDate()._Compare(wDateOp2,wDP)>=0;
    break;

  default:
    _DBGPRINT("Invalid operator for type ZDateFull logical compare operator %s\n",decode_ZSOPV(pOperator.ZSOPV))
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

ZStatus
evaluateLogicalTermResource(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{
  ZSearchOperandType_type wType = pOp2.OperandZSTO & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_Resource:
    break;
  default:
    _DBGPRINT("evaluateTermResource-W-SETINV Cannot compare type ZResource (operand 1) with %s (operand 2) result set to invalid\n",
        decode_OperandType(pOp2.OperandZSTO))
    return ZS_INVOP;
  } // switch

  switch (pOperator.ZSOPV)
  {
  case ZSOPV_EQUAL:
      pOutResult = pOp1.getResource() == pOp2.getResource();
    fprintf(stdout,"evaluateTerm- Op1 <%s> == Op2 <%s> Result %s\n",
        pOp1.getResource().toHexa().toString(),
        pOp1.getResource().toHexa().toString(), pOutResult?"true":"false");
    break;
  case ZSOPV_NOTEQUAL:
    pOutResult = !(pOp1.getResource() == pOp2.getResource());
    fprintf(stdout,"evaluateTerm- Op1 <%s> != Op2 <%s> Result %s\n",
        pOp1.getResource().toHexa().toString(),
        pOp1.getResource().toHexa().toString(), pOutResult?"true":"false");
    break;

  default:
    _DBGPRINT("evaluateTermResource-E-INVOP Invalid operator for ZResource logical compare operation %s\n",decode_ZSOPV(pOperator.ZSOPV))
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

ZStatus
evaluateTermChecksum(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{

    checkSum wCSOp2;

  ZSearchOperandType_type wType = pOp2.OperandZSTO & ZSTO_BaseMask;
  /* manage possible conversions */
  switch (wType)
  {
  case ZSTO_String:
      wCSOp2.fromHexa(pOp2.getString());
    break;
  case ZSTO_Checksum:
      wCSOp2=pOp2.getChecksum();
    break;
  default:
    _DBGPRINT("evaluateTermChecksum-W-SETINV Cannot compare tyoe Checksum (operand 1) with %s (operand 2) result set to invalid\n",
        decode_OperandType(pOp2.OperandZSTO))
    return ZS_INVOP;
  } // switch

  switch (pOperator.ZSOPV)
  {
  case ZSOPV_EQUAL:
    pOutResult = pOp1.getChecksum() == wCSOp2 ;
    break;
  case ZSOPV_NOTEQUAL:
    pOutResult = pOp1.getChecksum() != wCSOp2 ;
    break;


  default:
    _DBGPRINT("evaluateTermChecksum-E-INVOP Invalid operator for type Checksum logical compare operator %s\n",decode_ZSOPV(pOperator.ZSOPV))
    return ZS_INV_OPERATOR;
  }
  return ZS_SUCCESS;
}

/* Manage possible conversion then logical operation between two operands.
 * the first operand defines the conversion if necessary
 */

ZStatus
evaluateTerm(bool &pOutResult,ZOperandContent& pOp1,ZOperandContent& pOp2,ZSearchOperator& pOperator)
{

  ZSearchOperandType_type wType = pOp1.OperandZSTO & ZSTO_BaseMask;
  switch (wType)
  {
  case ZSTO_String:
    return evaluateTermString(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Integer:
    return evaluateLogicalTermInteger(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Float:
    return evaluateTermFloat(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Date:
    return evaluateTermDate(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Resource:
    return evaluateLogicalTermResource(pOutResult,pOp1,pOp2,pOperator);
  case ZSTO_Checksum:
    return evaluateTermChecksum(pOutResult,pOp1,pOp2,pOperator);
  default:
      _DBGPRINT("evaluateTerm-F-INVOP Invalid operand type %X %s\n",wType,decode_OperandType(wType))
  }

  return ZS_INVOP;
}

utf8VaryingString
OperandReportFormula(void * pOperand, bool pDetailed)
{
  utf8VaryingString wReturn;
  if (pOperand==nullptr) {
    wReturn="<NULL>";
    return wReturn;
  }
  ZSearchOperandBase* wOpBase=static_cast<ZSearchOperandBase*>(pOperand);
  while (true) {
    if (wOpBase->ZSTO & ZSTO_Arithmetic) {
      wReturn.addsprintf("[%s]", static_cast<ZSearchArithmeticTerm*>(pOperand)->_reportFormula().toString());
      break;
    }
    if (wOpBase->ZSTO == ZSTO_Logical) {
      wReturn += static_cast<ZSearchLogicalTerm*>(pOperand)->_reportFormula(true);
      break;
    }

    if (wOpBase->ZSTO & ZSTO_Field) {
      ZSearchFieldOperand* wFldData = static_cast<ZSearchFieldOperand*>( pOperand);

      wReturn.addsprintf(" %s", wFldData->FullFieldName.toString() );
      break;
    }
    if (wOpBase->ZSTO & ZSTO_Literal) {
      wReturn.addsprintf(" %s",ZSearchLogicalOperand::_evaluateOpLiteral(pOperand).toString() );
      break;
    }
    wReturn.addsprintf("<Invalid type %s 0x%X >\n",
        decode_OperandType(static_cast<ZSearchOperandBase*>(pOperand)->ZSTO),
        static_cast<ZSearchOperandBase*>(pOperand)->ZSTO
        );
    break;
  }// while true

  wReturn += wOpBase->_reportModifierBrief();

  if (pDetailed) {
/*
    if (!static_cast<ZSearchOperandBase*>(pOperand)->Comment.isEmpty()) {
      wReturn.addsprintf("(%s)",static_cast<ZSearchOperandBase*>(pOperand)->Comment.toString());
    }
*/
      if (!wOpBase->FullFieldName.isEmpty() && !(wOpBase->ZSTO & ZSTO_Field)) {
        wReturn.addsprintf("[%s]",wOpBase->FullFieldName.toString());
    }
  }

  wReturn += " ";

  return wReturn;
} // OperandReportFormula

utf8VaryingString
OperandReportDetailed(void* pOperand,int pLevel)
{
  utf8VaryingString wReturn;

    ZSearchOperandBase* wOpBase = static_cast<ZSearchOperandBase*>(pOperand);

  wReturn.addsprintf("%*cOperand\n",pLevel,' ');
  while (true) {
    if (wOpBase->ZSTO == ZSTO_Arithmetic) {
      wReturn += static_cast<ZSearchArithmeticTerm*>(pOperand)->_report(pLevel+1);
      break;
    }
    if (wOpBase->ZSTO == ZSTO_Logical) {
      wReturn += static_cast<ZSearchLogicalTerm*>(pOperand)->_report(pLevel+1);
      break;
    }

    if (wOpBase->ZSTO & ZSTO_Field) {
      ZSearchFieldOperand* wFldData = static_cast<ZSearchFieldOperand*>( pOperand);
      wReturn.addsprintf("%*c type %s full field name <%s> dictionary type <%s>\n",
          pLevel,' ',
          decode_OperandType(static_cast<ZSearchOperandBase*>(pOperand)->ZSTO),
          wFldData->FullFieldName.toString(),
                         decode_ZType(wFldData->getZType())
          );
      break;
    }
    if (wOpBase->ZSTO & ZSTO_Literal) {
      wReturn.addsprintf("%*c type %s value <%s>\n",
          pLevel,' ',
          decode_OperandType(wOpBase->ZSTO),
          OperandReportLiteral(pOperand).toString()
          );
      break;
    }
    wReturn.addsprintf("%*c type %s <Invalid type>\n",
        pLevel,' ',
        decode_OperandType(wOpBase->ZSTO)
        );
    break;
  }// while true

  wReturn += wOpBase->_reportModifier(pLevel);

  return wReturn;
} // OperandReportDetailed

utf8VaryingString
OperandReportLiteral(void *pOp)
{
  utf8VaryingString wReturn;

  if (!(static_cast<ZSearchOperandBase*>(pOp)->ZSTO & ZSTO_Literal)) {
    wReturn.sprintf("_evaluateOpLiteral-E-INVTYP Invalid type to evaluate <%s>",
        decode_OperandType(static_cast<ZSearchOperandBase*>(pOp)->ZSTO));
    return wReturn;
  }
  switch (static_cast<const ZSearchOperandBase*>(pOp)->ZSTO )
  {
  case ZSTO_LiteralString:
  {
    ZSearchLiteral* wOp=static_cast<ZSearchLiteral*>(pOp);
    wReturn.sprintf("\"%s\"",wOp->getString());
    //    return wOp->Content;
    return wReturn;
  }
  case ZSTO_LiteralUriString:
  {
    ZSearchLiteral* wOp=static_cast<ZSearchLiteral*>(pOp);
    return wOp->getURI();
  }
  case ZSTO_LiteralInteger:
  {
    ZSearchLiteral* wOp=static_cast<ZSearchLiteral*>(pOp);
    wReturn.sprintf("%ld",wOp->getInteger());
    return wReturn;
  }
  case ZSTO_LiteralFloat:
  {
    ZSearchLiteral* wOp=static_cast<ZSearchLiteral*>(pOp);
    wReturn.sprintf("%g",wOp->getFloat());
    return wReturn;
  }
  case ZSTO_LiteralDate:
  {
    ZSearchLiteral* wOp=static_cast<ZSearchLiteral*>(pOp);
    return wOp->getDate().toLocale();
  }
  case ZSTO_LiteralResource:
  {
    ZSearchLiteral* wOp=static_cast<ZSearchLiteral*>(pOp);
    //    return wOp->Content.toHexa();
    long wi=0;
    for (;wi < ZEntitySymbolList.count();wi++) {
        if (ZEntitySymbolList[wi].Value==wOp->getResource().Entity) {
        break;
      }
    }// for
    if (wi==ZEntitySymbolList.count())
      wReturn.sprintf("ZResource(%6X,%6X)",wOp->getResource().Entity,wOp->getResource().id);
    else
      wReturn.sprintf("ZResource(%s,%6X)",ZEntitySymbolList[wi].Symbol.toString(),wOp->getResource().id);
    return wReturn;
  }
  case ZSTO_LiteralBool:
  {
    ZSearchLiteral* wOp=static_cast<ZSearchLiteral*>(pOp);
    return wOp->getBool()?"TRUE":"FALSE";
  }
  case ZSTO_LiteralChecksum:
  {
    ZSearchLiteral* wOp=static_cast<ZSearchLiteral*>(pOp);
    //    return wOp->Content.toHexa();
    wReturn.sprintf("checkSum(%s)",wOp->getChecksum().toHexa().toString());
    return wReturn;
  }
  default:
  {
    wReturn.sprintf("_evaluateOpLiteral-E-INVTYP Invalid type to evaluate <%s>",
        decode_OperandType(static_cast<ZSearchOperandBase*>(pOp)->ZSTO));
    return wReturn;
  }

  }// switch
  return wReturn;
} // OperandReportLiteral

void clearOperand(void *&pOp)
{
  if (pOp==nullptr)
    return;
  if (static_cast<ZSearchOperandBase*>(pOp)->ZSTO & ZSTO_Arithmetic)
  {
    delete static_cast<ZSearchArithmeticTerm*>(pOp);
    pOp=nullptr;
    return;
  }
  if (static_cast<ZSearchOperandBase*>(pOp)->ZSTO & ZSTO_Logical)
  {
      delete static_cast<ZSearchLogicalTerm*>(pOp);
      pOp=nullptr;
      return;
  }

  if (static_cast<ZSearchOperandBase*>(pOp)->ZSTO & ZSTO_Field)
  {
      delete static_cast<ZSearchFieldOperand*>(pOp);
      pOp=nullptr;
      return;
  }

  if (static_cast<ZSearchOperandBase*>(pOp)->ZSTO & ZSTO_Literal) {
      delete static_cast<ZSearchLiteral*>(pOp);
      pOp=nullptr;
      return;
  }
  pOp=nullptr;
  return;
}

void _copyOperand(void*& pOperand,const void* pOpIn)
{
  clearOperand(pOperand);
  if (pOpIn==nullptr) {
    _DBGPRINT("_copyOperand-E-NULL Input operand is NULL\n")
    return;
  }
  if (static_cast<const ZSearchOperandBase*>(pOpIn)->ZSTO & ZSTO_Arithmetic)
  {
    _DBGPRINT("_copyOperand-I Arithmetic term\n")
    pOperand = new ZSearchArithmeticTerm(static_cast<const ZSearchArithmeticTerm*>(pOpIn));
    return;
  }
  if (static_cast<const ZSearchOperandBase*>(pOpIn)->ZSTO & ZSTO_Field)
  {
      _DBGPRINT("_copyOperand-I Field operand\n")
      pOperand = new ZSearchFieldOperand(*static_cast<const ZSearchFieldOperand*>(pOpIn));
      return;
  }
  if (static_cast<const ZSearchOperandBase*>(pOpIn)->ZSTO & ZSTO_Literal) {
      _DBGPRINT("_copyOperand-I Literal operand\n")
      pOperand = new ZSearchLiteral(*static_cast<const ZSearchLiteral*>(pOpIn));
  }
  _DBGPRINT("_copyOperand-I Single operand of type <%s>\n",decode_OperandType(static_cast<const ZSearchOperandBase*>(pOpIn)->ZSTO))
  return;
}// copyOperand

ZSearchOperandType getZSTOFromZType(ZTypeBase pZType)
{

    switch (pZType) {
    case ZType_AtomicChar:
    case ZType_Char:
    {
        return ZSTO_Integer;
    }
    case ZType_UChar:
    case ZType_AtomicUChar:
    {
        return  ZSTO_Integer;
    }
    case ZType_S8:
    case ZType_AtomicS8:
    {
        return ZSTO_Integer;
    }
    case ZType_U8:
    case ZType_AtomicU8:
    {
        return ZSTO_Integer;
    }
    case ZType_S16:
    case ZType_AtomicS16:
    {
        return ZSTO_Integer;;
    }
    case ZType_U16:
    case ZType_AtomicU16:
    {
        return ZSTO_Integer;
    }
    case ZType_S32:
    case ZType_AtomicS32:
    {
        return ZSTO_Integer;
    }
    case ZType_U32:
    case ZType_AtomicU32:
    {
        return ZSTO_Integer;
    }
    case ZType_S64:
    case ZType_AtomicS64:
    {
        return ZSTO_Integer;
    }
    case ZType_U64:
    case ZType_AtomicU64:
    {
        return ZSTO_Integer;
    }
    case ZType_Float:
    case ZType_AtomicFloat:
    {
        return ZSTO_Float;
    }
    case ZType_Double:
    case ZType_AtomicDouble:
    {
        return ZSTO_Float;
    }
    case ZType_LDouble:
    case ZType_AtomicLDouble:
    {
        return ZSTO_Float;
    }
    case ZType_Bool:
    {
        return ZSTO_Bool;
    }


    case ZType_Utf8VaryingString: {
        return ZSTO_String;
    }
    case ZType_Utf16VaryingString: {
        return ZSTO_String;
    }
    case ZType_Utf32VaryingString: {
        return ZSTO_String;;
    }

    case ZType_ZDateFull: {
        return ZSTO_Date;
    }

    case ZType_Resource: {
        return ZSTO_Resource;
    }
    case ZType_CheckSum: {
        return ZSTO_Checksum;
    }
    case ZType_URIString: {
        return ZSTO_UriString;
    }
    default:
        _DBGPRINT("Cannot convert data type <0x%X> %s - data type not supported.",pZType,decode_ZType(pZType))
        return ZSTO_Nothing;
    }//switch

} // getFieldOperandContent



ZStatus ZSearchFieldOperand::getURFContent(ZSearchEntityContext &pSEC,URFField & pURFField)
{
    ZStatus wSt=ZS_SUCCESS;
    ZOperandContent::clear();

    const unsigned char* wPtr=pURFField.FieldPtr;

    switch (pURFField.ZType) {
    case ZType_AtomicChar:
    case ZType_Char:
    {
        char wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<char>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_UChar:
    case ZType_AtomicUChar:
    {
        unsigned char wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<unsigned char>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_S8:
    case ZType_AtomicS8:
    {
        int8_t wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<int8_t>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_U8:
    case ZType_AtomicU8:
    {
        uint8_t wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<uint8_t>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_S16:
    case ZType_AtomicS16:
    {
        int16_t wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<int16_t>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_U16:
    case ZType_AtomicU16:
    {
        uint16_t wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<uint16_t>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_S32:
    case ZType_AtomicS32:
    {
        int32_t wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<int32_t>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_U32:
    case ZType_AtomicU32:
    {
        uint32_t wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<uint32_t>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_S64:
    case ZType_AtomicS64:
    {
        int64_t wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<int64_t>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_U64:
    case ZType_AtomicU64:
    {
        uint64_t wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<uint64_t>(wVal,wPtr);
        setInteger (long(wVal));
        OperandZSTO = ZSTO_Integer;
        break;
    }
    case ZType_Float:
    case ZType_AtomicFloat:
    {
        float wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<float>(wVal,wPtr);
        setFloat (double(wVal));
        OperandZSTO = ZSTO_Float;
        break;
    }
    case ZType_Double:
    case ZType_AtomicDouble:
    {
        double wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<double>(wVal,wPtr);
        setFloat (double(wVal));
        OperandZSTO = ZSTO_Float;
        break;
    }
    case ZType_LDouble:
    case ZType_AtomicLDouble:
    {
        long double wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<long double>(wVal,wPtr);
        setFloat (double(wVal));
        OperandZSTO = ZSTO_Float;
        break;
    }
    case ZType_Bool:
    {
        bool wVal;
        wPtr += sizeof(ZTypeBase);
        _importAtomic<bool>(wVal,wPtr);
        setBool ( bool(wVal));
        OperandZSTO = ZSTO_Bool;
        break;
    }

    case ZType_Utf8VaryingString:
    {
        utf8VaryingString wVal;
        wVal._importURF(wPtr);

        wSt= setStringOperandModifierValue(wVal,ModifierType,ModParams,this);
        if (wSt==ZS_INVTYPE) {
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      ModifierType,
                      decode_SearchTokenType(ModifierType),
                      pURFField.ZType,
                      decode_ZType(pURFField.ZType))
            return ZS_INVTYPE;
        }

        break;
    } // ZType_Utf8VaryingString

    case ZType_Utf16VaryingString:
    {
        utf16VaryingString wVal1;
        utf8VaryingString wVal;
        wVal1._importURF(wPtr);
        wVal.fromUtf16( wVal1);
        wSt= setStringOperandModifierValue(wVal,ModifierType,ModParams,this);
        if (wSt==ZS_INVTYPE) {
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      ModifierType,
                      decode_SearchTokenType(ModifierType),
                      pURFField.ZType,
                      decode_ZType(pURFField.ZType))
            return ZS_INVTYPE;
        }
        break;
    }
    case ZType_Utf32VaryingString: {
        utf32VaryingString wVal1;
        utf8VaryingString wVal;
        wVal1._importURF(wPtr);
        wVal.fromUtf32( wVal1);
        wSt= setStringOperandModifierValue(wVal,ModifierType,ModParams,this);
        if (wSt==ZS_INVTYPE) {
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      ModifierType,
                      decode_SearchTokenType(ModifierType),
                      pURFField.ZType,
                      decode_ZType(pURFField.ZType))
            return ZS_INVTYPE;
        }
        break;
    }

    case ZType_ZDateFull: {
        ZDateFull wDate;
        wDate._importURF(wPtr);

        switch (ModifierType) /* NB: Content Type is changed to integer */
        {
        case ZSRCH_NOTHING:
        case ZSRCH_TRANSLATE: /* if translate, then source value must be set to current operand */
        {
            setDate ( wDate);
            OperandZSTO = ZSTO_Date;
            break;
        }
        case ZSRCH_YEAR:
        {
            setInteger ( wDate.year());
            OperandZSTO = ZSTO_Integer;
            break;
        }
        case ZSRCH_MONTH:
        {
            setInteger ( wDate.month());
            OperandZSTO = ZSTO_Integer;
            break;
        }
        case ZSRCH_DAY:
        {
            setInteger ( wDate.day());
            OperandZSTO = ZSTO_Integer;
            break;
        }
        case ZSRCH_HOUR:
        {
            setInteger ( wDate.hour());
            OperandZSTO = ZSTO_Integer;
            break;
        }
        case ZSRCH_MIN:
        {
            setInteger ( wDate.min());
            OperandZSTO = ZSTO_Integer;
            break;
        }
        case ZSRCH_SEC:
        {
            setInteger ( wDate.sec());
            OperandZSTO= ZSTO_Integer;
            break;
        }
        default:
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      ModifierType,
                      decode_SearchTokenType(ModifierType),
                      pURFField.ZType,
                      decode_ZType(pURFField.ZType))
            return ZS_INVTYPE;
        }// switch pModifier
        break;
    } // ZType_ZDateFull

    case ZType_Resource: {
        ZResource wResource;
        wResource._importURF(wPtr);
        OperandZSTO = ZSTO_Resource;
        switch (ModifierType) /* NB: Content ZType is changed to integer */
        {
        case ZSRCH_NOTHING:
        case ZSRCH_TRANSLATE: /* if translate, then source value must be set to current operand */
        {
            setResource ( wResource );
            OperandZSTO = ZSTO_Resource;
            break;
        }
        case ZSRCH_ZENTITY:
        {
            setInteger ( wResource.Entity );
            OperandZSTO = ZSTO_Integer;
            break;
        }
        case ZSRCH_ID:
        {
            setInteger ( wResource.id );
            OperandZSTO = ZSTO_Integer;
            break;
        }
        default:
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      ModifierType,
                      decode_SearchTokenType(ModifierType),
                      pURFField.ZType,
                      decode_ZType(pURFField.ZType))
            return ZS_INVTYPE;
        }// switch pModifier
        break;
    } // ZType_Resource

    case ZType_CheckSum: {
        checkSum wCheckSum ;
        wCheckSum._importURF(wPtr);
        setChecksum( wCheckSum );
        OperandZSTO = ZSTO_Checksum;
        break;
    }
    case ZType_URIString: {
        uriString wURI;
        wURI._importURF(wPtr);

        switch (ModifierType) /* NB: Content ZType remains the same */
        {
        case ZSRCH_NOTHING:  /* No modifier */
        case ZSRCH_TRANSLATE: /* if translate, then source value must be set to current operand */
        {
            setURI ( wURI);
            OperandZSTO = ZSTO_UriString;
            break;
        }
        case ZSRCH_PATH:
        {
            setURI ( wURI.getDirectoryPath());
            OperandZSTO = ZSTO_UriString;
            break;
        }
        case ZSRCH_EXTENSION:
        {
            setString ( wURI.getFileExtension());
            OperandZSTO = ZSTO_String;
            break;
        }
        case ZSRCH_BASENAME:
        {
            setString ( wURI.getBasename());
            OperandZSTO = ZSTO_String;
            break;
        }
        case ZSRCH_ROOTNAME:
        {
            setString ( wURI.getRootname());
            OperandZSTO = ZSTO_String;
            break;
        }
        default:
            _DBGPRINT("%s-E-INVTYP Invalid modifier 0x%X <%s> for field type 0x%X %s.",
                      _GET_FUNCTION_NAME_,
                      ModifierType,
                      decode_SearchTokenType(ModifierType),
                      pURFField.ZType,
                      decode_ZType(pURFField.ZType))
            return ZS_INVTYPE;
        }// switch pModifier
        break;
    } // ZType_URIString

    default:
        _DBGPRINT("Cannot convert data type %s - data type not supported.",decode_ZType(pURFField.ZType))
        return ZS_INVTYPE;
    }//switch

    if (ModifierType==ZSRCH_TRANSLATE) {
        ZArray<URFField> wFieldList;
        ZDataBuffer wRecord;
        Mod_Translate* wMD=static_cast<Mod_Translate*>(ModParams);
        ZStatus wSt=ZS_SUCCESS;
        wSt=wMD->Entity->findFirstFieldValueSequential(pSEC,wMD->KeyDicRank,this,wRecord);
        if (wSt==ZS_FOUND) {
            return getURFOperandContentWModifier(pSEC,pSEC._URFParser.URFFieldList[wMD->ResultDicRank],ZSRCH_NOTHING,nullptr, this);
        }
        return wSt;
    } // ZSRCH_DECODE



    _DBGPRINT("ZSearchFieldOperand::getURFContent field type <%s> operand type<%s> value <%s>\n",
              decode_ZType(pURFField.ZType),decode_OperandType(OperandZSTO),this->display().toString())

    return ZS_SUCCESS;
}// ZSearchFieldOperand::getURFContent



namespace zbs {
const char* decode_TSST(int pSt)
{
    switch (pSt)
    {
    case TSST_Success:
        return "TSST_Success";
    case TSST_MissOperand:
        return "TSST_MissOperand";
    case TSST_MissOperator:
        return "TSST_MissOperator";
    case TSST_MissLogTerm:
        return "TSST_MissLogTerm";
    case TSST_MissArithTerm:
        return "TSST_MissArithTerm";
    case TSST_Other:
        return "TSST_Other";
    default:
        return "Unknown Term Scan Status";
    }
}

} // namespace zbs

