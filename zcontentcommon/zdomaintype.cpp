#include "zdomaintype.h"

namespace zbs {

utf8VaryingString
decode_ZDomain_type(ZDomain_type pType)
{
    utf8VaryingString wReturn;

    if (pType & ZDOM_Root)
        wReturn.addConditionalOR("ZDOM_Root");

    if (pType & ZDOM_Remote)
        wReturn.addConditionalOR("ZDOM_Remote");
    if (pType & ZDOM_Absolute)
        wReturn.addConditionalOR("ZDOM_Absolute");
    /*
    if (pType & ZDOM_Relative)
        wReturn.addConditionalOR("ZDOM_Relative");
*/
    pType &= ZDOM_ObjectMask;

    for (int wi = 0; TypeDecodeTable[wi].Type != ZDOM_End ; wi++)
        if (pType ==TypeDecodeTable[wi].Type)
            wReturn.addConditionalOR(TypeDecodeTable[wi].TypeString);
    /*
    if ((pType & ZDOM_Path)==ZDOM_Path)
        wReturn.addConditionalOR("ZDOM_Path");
    if ((pType & ZDOM_Icon)==ZDOM_Icon)
        wReturn.addConditionalOR("ZDOM_Icon");

    if ((pType & ZDOM_File)==ZDOM_File)
        wReturn.addConditionalOR("ZDOM_File");

    if ((pType & ZDOM_Executable)==ZDOM_Executable)
        wReturn.addConditionalOR("ZDOM_Executable");

    if ((pType & ZDOM_TextFile)==ZDOM_TextFile)
        wReturn.addConditionalOR("ZDOM_TextFile");

    if ((pType & ZDOM_PdfFile)==ZDOM_PdfFile)
        wReturn.addConditionalOR("ZDOM_PdfFile");

    if ((pType & ZDOM_OdfFile)==ZDOM_OdfFile)
        wReturn.addConditionalOR("ZDOM_OdfFile");


    if ((pType & ZDOM_MasterFile)==ZDOM_MasterFile)
        wReturn.addConditionalOR("ZDOM_MasterFile");
    if ((pType & ZDOM_HeaderFile)==ZDOM_HeaderFile)
        wReturn.addConditionalOR("ZDOM_HeaderFile");
*/
    if (wReturn.isEmpty())
        return "ZDOM_Nothing";
    return wReturn;
}
utf8VaryingString
decode_ZDomainTypeForDisplay(ZDomain_type pType)
{
    utf8VaryingString wReturn;

    if (pType & ZDOM_Root)
        wReturn.addConditionalOR("Root");

    if (pType & ZDOM_Remote)
        wReturn.addConditionalOR("Remote");
    else
        wReturn.addConditionalOR("Local");


    if (pType & ZDOM_Absolute)
        wReturn.addConditionalOR("Absolute");
    else
        wReturn.addConditionalOR("Relative");

    pType &= ZDOM_ObjectMask;

    for (int wi = 0; TypeDecodeTable[wi].Type != ZDOM_End ; wi++)
        if (pType == TypeDecodeTable[wi].Type)
            wReturn.addConditionalOR(TypeDecodeTable[wi].Display);

    /*

    if (pType & ZDOM_Path)
        wReturn.addConditionalOR("Path");
    if (pType & ZDOM_Icon)
        wReturn.addConditionalOR("Icon");

    if (pType & ZDOM_Executable)
        wReturn.addConditionalOR("ZDOM_Executable");
    if (pType & ZDOM_Icon)
        wReturn.addConditionalOR("ZDOM_MasterFile");
    if (pType & ZDOM_Icon)
        wReturn.addConditionalOR("ZDOM_HeaderFile");
    if (pType & ZDOM_File)
        wReturn.addConditionalOR("ZDOM_File");


*/
    if (wReturn.isEmpty()){
        wReturn.addConditionalOR("Unknown object");
    }

    return wReturn;
}


/*  Not usefull
uint32_t
encode_ZDomain_type(utf8VaryingString& pType)
{
    uint32_t wType=ZDOM_Nothing;
    if (pType.containsCase("ZDOM_Root"))
        wType |= ZDOM_Root;

    if (pType.containsCase("ZDOM_Remote"))
        wType |= ZDOM_Remote;
    if (pType.containsCase("ZDOM_Absolute"))
        wType |= ZDOM_Absolute;
//    if (pType.containsCase("ZDOM_Relative"))
//        wType |= ZDOM_Relative;
    if (pType.containsCase("ZDOM_Path"))
        wType |= ZDOM_Path;
    if (pType.containsCase("ZDOM_Image"))
        wType |= ZDOM_Icon;
    if (pType.containsCase("ZDOM_Path"))
        wType |= ZDOM_Path;
    if (pType.containsCase("ZDOM_Path"))
        wType |= ZDOM_Path;
    if (pType.containsCase("ZDOM_Path"))
        wType |= ZDOM_Path;
    if (pType.containsCase("ZDOM_Path"))
        wType |= ZDOM_Path;
    return wType;
}
*/
utf8VaryingString getObjectTypeString(uint32_t pType)
{
    pType &= ZDOM_ObjectMask ;
    for (long wi=0 ; TypeDecodeTable[wi].Type != ZDOM_End ; wi ++ ) {
        if (TypeDecodeTable[wi].Type == pType)
            return TypeDecodeTable[wi].Display;
    }
    return "Unknown";
}
} // namespace zbs
