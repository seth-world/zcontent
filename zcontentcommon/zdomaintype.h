#ifndef ZDOMAINTYPE_H
#define ZDOMAINTYPE_H

#include <stdint.h>
#include <ztoolset/utfvaryingstring.h>

namespace zbs {


typedef uint32_t ZDomain_type ;

enum ZDomain_enum : ZDomain_type
{
    ZDOM_Nothing    =       0,

    ZDOM_Remote     =       1 ,
    ZDOM_Absolute   =       2 ,
    //    ZDOM_Relative   =       4 ,



    ZDOM_Root       =         0x10,

    ZDOM_ObjectMask =   0xFFFFFF00,
    ZDOM_Path       =       0x0100,
    ZDOM_Icon       =       0x0200,
    ZDOM_Image      =       0x0400,

    ZDOM_Executable =       0x0800,

    ZDOM_File       =     0x010000,
    ZDOM_ImageFile  =     0x010400,

    ZDOM_TextFile   =     0x011000,
    ZDOM_HtmlFile   =     0x012000,
    ZDOM_PdfFile    =     0x014000,

    ZDOM_OdfFile    =     0x018000,

    ZDOM_MasterFile =     0x01000000,
    ZDOM_HeaderFile =     0x03000000,

    //    ZDOM_Executable =     0x01000000,


    ZDOM_HasViewer  = ZDOM_ObjectMask & ~ ZDOM_File,

    ZDOM_HasEditor  = ZDOM_ObjectMask & ~ ZDOM_File,

    ZDOM_Viewable   = ZDOM_Icon | ZDOM_Image | ZDOM_HasViewer,  /* NB: Icons and Images have a built in viewer */

    ZDOM_End        =   0xFFFFFFF
};

struct TypeDecodeStruct
{
    ZDomain_enum                Type = ZDOM_Nothing;
    const utf8VaryingString     TypeString;
    const utf8VaryingString     Display ;
};

const TypeDecodeStruct TypeDecodeTable [] =
    {
        ZDOM_Path,      "ZDOM_Path" ,       "Path",
        ZDOM_Icon,      "ZDOM_Icon" ,       "Icon",
        ZDOM_Image,     "ZDOM_Image" ,      "Image",
        ZDOM_Executable,"ZDOM_Executable" , "Executable",

        ZDOM_TextFile,  "ZDOM_TextFile" ,   "Text file",
        ZDOM_PdfFile,   "ZDOM_PdfFile" ,    "Pdf file",
        ZDOM_OdfFile,   "ZDOM_OdfFile" ,    "Odf file",
        ZDOM_MasterFile,"ZDOM_MasterFile" , "Content master file",
        ZDOM_HeaderFile,"ZDOM_HeaderFile" , "Content header file",

        ZDOM_File,      "ZDOM_File" ,       "Other file",
        ZDOM_End,       "ZDOM_End" ,        "End"
} ;

utf8VaryingString getObjectTypeString(uint32_t pType);

utf8VaryingString decode_ZDomain_type(ZDomain_type pType);

utf8VaryingString decode_ZDomainTypeForDisplay(ZDomain_type pType);

//uint32_t encode_ZDomain_type(utf8VaryingString& pType);  // not usefull



}// namespace zbs



#endif // ZDOMAINTYPE_H
