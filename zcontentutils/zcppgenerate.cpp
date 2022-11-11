#include "zcppgenerate.h"

#include <zxml/zxmlprimitives.h>
#include <zexceptiondlg.h>

#include <zindexedfile/zdictionaryfile.h>
#include <zcppparser/zcppparser.h>


using namespace zbs;

const char* wCppGenComment =
"/**\n"
"  @defgroup DATA_FORMAT Natural vs Universal vs UVF vs URF\n"
"\n"
"  Natural format :\n"
"\n"
"    Data format internally stored on computer. Subject to Endianness (if system is little endian) an Sign (if data is signed)\n"
"\n"
"  Universal format :\n"
"    Data format internally stored on computer. Subject to Endianness (if system is little endian) an Sign (if data is signed)\n"
"\n"
"\n    Data format as it is stored as a key field : it may be sorted\n"
"\n"
"    Data is stored independantly of Endianness, with a preceeding byte sign if data is signed\n"
"    Universal format must be mandatorily coupled with\n"
"        - ZType_type  : enough for ZType_Atomic and ZType_Class (ZType_Date, ZType_Checksum,etc.) : size is deduced from type\n"
"        - if not atomic or class (ZType_Array and ZType_Strings) effective array count\n"
"\n"
"  Universal Varying Format (UVF) : concerns fixed strings (and only fixed strings)\n"
"\n"
"    Data format as it is stored within exported blocks in files headers (ZRF, ZMF, ZIF)\n"
"    string content is preceeded with an int16_t mentionning length of the string\n"
"\n"
"\n"
" Universal Record Format (URF)\n"
"\n"
"    Data format as it is stored within a file.\n"
"    Data is preceeded with a header describing the data.\n"
"    This header changes according ZType_type of data.\n"
"\n"
"\n"
"  ZType_Type vs URF Header sizes\n"
"\n"
"  ZType_Atomic          ZTypeBase\n"
"  ------------          data size is deduced from atomic type size\n"
"                        see : getAtomicZType_Sizes() function <zindexedfile/zdatatype.h>\n"
"\n"
"  ZType_Class\n"
"  -----------\n"
"  ZType_Date            ZTypeBase\n"
"                        data size is deduced from object class size.\n"
"                        size is sizeof (uint32_t)\n"
"\n"
"  ZType_DateFull        ZTypeBase\n"
"                        data size is deduced from object class size\n"
"                        size is sizeof (uint64_t)\n"
"\n"
"  ZType_CheckSum        ZTypeBase\n"
"                        data size is deduced from object class size\n"
"                        size is sizeof (cst_checksum)\n"
"  ZType_String\n"
"  ------------\n"
"  ZType_utfxxFixedString    ZTypeBase : gives the object type and the size of character unit i.e. ZType_Char, ZType_U8, ZType_U16, ZType_U32\n"
"                            uint16_t :  canonical size (capacity) : capacity of string class (maximum number of character units)\n"
"                            uint16_t : Effective Universal size in bytes : universal size in bytes of stored data (excluding URF header)\n"
"                                    NB: this size differs from canonical size representing the capacity of the string in character units\n"
"\n"
"                        NB: Maximum number of bytes is <65'534>. Maximum number of characters depends on atomic type.\n"
"\n"
"  ZType_FixedWString    ZTypeBase\n"
"                        uint16_t : canonical size : capacity of string class (number of characters)\n"
"                        uint16_t : Effective Universal size : universal size in bytes of stored data\n"
"                                    NB: this size differs from canonical size representing the capacity of the string in char\n"
"\n"
"                        NB: Maximum number of characters is <32'765>.\n"
"\n"
"  ZType_utfxxVaryingString  ZTypeBase  : gives the size of character unit i.e. ZType_U8, ZType_U16, ZType_U32\n"
"                            uint64_t : data byte size\n"
"                            warning :  data byte size WITHOUT Header Size\n"
"\n"
"  ZType_VaryingWString  ZTypeBase\n"
"                        uint64_t : data byte size\n"
"                        warning :   Number of characters is <data byte size / sizeof(wchar_t)>\n"
"                                    data byte size WITHOUT Header Size\n"
"\n"
"  ZType_Array           ZTypeBase  : gives the size of character unit i.e. ZType_U8, ZType_U16, ZType_U32\n"
"  -----------           uint32_t : Canonical (Array) count\n"
"                        warning : number of bytes is (Array Count * atomic data size) WITHOUT Header Size\n"
"\n"
"  ZType_Blob            ZTypeBase\n"
"  ----------            uint64_t : data byte size WITHOUT Header Size\n"
"\n"
"  */\n";


const char* GenerateBanner =
    "/** "
    " This file has been generated by zcontent utilities generator version %s\n"
    " using dictionary file %s\n"
    " Generation date %s\n"
    "\nAttention: This file must not be manually changed in order to preserve the ability to change it using dictionary generation facilities.\n"
    "-----------*/\n\n";
const char* GenerateHBannerErrored =
    "/** "
    " This file has been generated by zcontent utilities generator %s\n"
    " using dictionary file %s\n"
    " Generation date %s\n\n"
    " IMPORTANT : generation errors have been detected.\n"
    " %d field(s) are errored and are missing.\n"
    "\nAttention: This file must not be manually changed in order to preserve the ability to change it using dictionary generation facilities.\n"
    "-----------*/\n\n";
const char* GenerateCppBannerErrored =
    "/** "
    " This file has been generated by zcontent utilities generator %s\n"
    " using dictionary file %s\n"
    " Generation date %s\n\n"
    " IMPORTANT : generation errors have been detected.\n"
    " %d instructions have not been generated due to errored fields.\n"
    " */\n\n";

const char* wHHeader=
    "#ifndef %s_H\n"
    "#define %s_H\n\n"

    "#include <sys/types.h>\n"
    "#include <ztoolset/zlimit.h>\n"
    "#include <ztoolset/ztypetype.h>\n"
    "#include <ztoolset/zdatabuffer.h>\n"
    "#include <ztoolset/zarray.h>\n"

    ;

const char* wHClass=
    "\n/** @brief %s %s"
    " */\n"

//    "#pragma pack(push)\n"
//    "#pragma pack(1)         // memory alignment on byte\n"
    "\n"
    "class %s {\n"
    "private:\n"
    "   ZBitset     FieldPresence;\n"
//    "   uint32_t    StartSign=cst_ZMSTART;\n"
//    "   uint16_t    EndianCheck=cst_EndianCheck_Normal;\n"
    "public:\n"
    "\n";


const char* wHMethods =
    "/* Public methods */\n"
    "   %s()=default;\n"
    "   %s& _copyFrom(const %s& pIn);\n"
    "   %s& operator = (const %s& pIn) { return _copyFrom(pIn); }\n"
//    "   void setFromPtr(const unsigned char *&pPtrIn);\n"
//    "   void set(const %s& pIn);\n"
    "   ZDataBuffer toRecord();\n"
    "   ZStatus fromRecord(const ZDataBuffer& pRecord);\n"
    "   zbs::ZArray<ZDataBuffer> getAllKeys();\n"
    "   inline long getKeysNumber() { return %ld; }\n"
    ;
/*    "   bool isReversed();\n"
    "   bool isNotReversed(); \n"
*/


const char* wSCopyFromBegin =
    "   %s& %s::_copyFrom(const %s& pIn) {\n"
/*    "     StartSign = pIn.StartSign ;\n"
    "     EndianCheck = pIn.EndianCheck ;\n"
*/
    ;
const char* wSCopyFromLine =
    "     %s = pIn.%s ;\n";

const char* wSCopyFromLineErrored =
    "     /* Cannot copy errored field <%s >  */\n";

const char* wSMethodEnd =
    " }// %s\n\n " ;


const char* wCppDeserialize =
    "void %s::_convert() {\n"
    "  if (is_big_endian()) \n"
    "     return ;\n"
    " /* StartSign EndSign(palyndromas) do not need to be reversed */ "

    "   if (isNotReversed()) { \n"
    "       fprintf (stderr,\"%s::deserialize-W-ALRDY object <%s> already deserialized. \n\");\n"
    "       return;\n"
    "   }\n" ;
#ifdef __COMMENT__
const char* wCppSerialize =
    "void %s::_serialize() {\n"
    "  if (is_big_endian()) \n"
    "     return ;\n"
    " /* StartSign EndSign(palyndromas) do not need to be reversed */ "
    "   if (isReversed()) { \n"
    "       fprintf (stderr,\"%s::serialize-W-ALRDY object <%s> already serialized. \n\");\n"
    "       return;\n"
    "   }\n\n" ;
#endif //__COMMENT__

/* move to record */

const char* wCppToRecordBegin =
    "ZDataBuffer %s::toRecord() {\n"
    "   ZDataBuffer wReturn;\n\n"
    "   FieldPresence._allocate(%d);\n"
    "   FieldPresence.setAll();\n"
    "   FieldPresence._exportAppendURF(wReturn);\n\n"
    "\n"
    "/*         User content size computation  */\n"
    "   uint64_t wURFSize=0;\n";

const char* wCppToRSizeClassErrored =
    "     /* Cannot evaluate size of an errored field <%s>   */\n";

const char* wCppToRSizeClass =
    "   wURFSize += %s.getURFSize();\n";

const char* wCppToRSizeAtomic =
    "   wURFSize += getAtomicURFSize<%s>(%s);\n";

const char* wCppToRecordMiddle =
    "\n"
    "   unsigned char* wPtr = wReturn.extend(wURFSize+sizeof(uint64_t));\n"
    "   _exportAtomicPtr(wURFSize,wPtr); /* user record size is in universal format but not in URF (no preceeding URF header) */\n\n"
    "/*              move of individual fields to record space */\n";

const char* wCppToRecordMoveErrored =
    "     /* Cannot move to record an errored field <%s>   */\n";

const char* wCppToRecordMoveClass =
    "   %s._exportURF_Ptr(wPtr);\n";

const char* wCppToRecordMoveAtomic =
    "   exportAtomicURF_Ptr<%s>(%s,wPtr);\n";

const char* wCppToRecordEnd =
    "\n   return wReturn;\n"
    "}//toRecord\n\n";




ZCppGenerate::ZCppGenerate(ZDictionaryFile *pDictionaryFile)
{
  DictionaryFile = pDictionaryFile;
}


ZStatus
ZCppGenerate::loadXmlParameters(const uriString& pXmlFile,
    ZArray<GenObj> &pGenObjList,
    ZArray<GenInclude> &pGenIncludeList,
    ZaiErrors* pErrorLog) {


  utf8VaryingString wXmlString;

  ZStatus wSt;
  pErrorLog->setAutoPrintOn(ZAIES_Text);

  if (!pXmlFile.exists())  {
    ZException.setMessage("ZCppParser::loadXmlParserRules",ZS_FILENOTEXIST,Severity_Error,"Parameter file <%s> has not been found.",pXmlFile.toCChar());
    pErrorLog->errorLog("ZCppParser::loadXmlParserRules-E-FILNFND Parameter file <%s> has not been found.",pXmlFile.toCChar());
    return ZS_FILENOTEXIST;
  }


  if ((wSt=pXmlFile.loadUtf8(wXmlString))!=ZS_SUCCESS) {
    pErrorLog->logZException();
    return wSt;
  }

  zxmlDoc     *wDoc = nullptr;
  zxmlElement *wRoot = nullptr;
  zxmlElement *wParamRootNode=nullptr;
  zxmlElement *wGenParamsNode=nullptr;
  zxmlElement *wObjectNode=nullptr;
  zxmlElement *wSwapNode=nullptr;

//  zxmlElement *wTypeNode=nullptr;
  zxmlElement *wIncludeFileNode=nullptr;

  utf8VaryingString wKeyword;

  ZTypeBase             wZType;

  utf8VaryingString     wIncludeFile;

  wDoc = new zxmlDoc;
  wSt = wDoc->ParseXMLDocFromMemory(wXmlString.toCChar(), wXmlString.getUnitCount(), nullptr, 0);
  if (wSt != ZS_SUCCESS) {
    pErrorLog->logZException();
    pErrorLog->errorLog(
        "DicEdit::loadGenerateParameters-E-PARSERR Xml parsing error for string <%s> ",
        wXmlString.subString(0, 25).toString());
    return wSt;
  }

  wSt = wDoc->getRootElement(wRoot);
  if (wSt != ZS_SUCCESS) {
    pErrorLog->logZException();
    return wSt;
  }
  if (!(wRoot->getName() == "zcppgenerate")) {
    ZException.setMessage("ZCppParser::loadXmlParserRules",
        ZS_XMLINVROOTNAME,
        Severity_Error,
        "Invalid root name <%s> - file <%s>.",wRoot->getName().toString(),pXmlFile.toCChar());
    pErrorLog->errorLog(
        "DicEdit::loadGenerateParameters-E-INVROOT Invalid root node name <%s> expected <zcppgenerate>",
        wRoot->getName().toString());
    return ZS_XMLINVROOTNAME;
  }
  /*------------------ various parameters -----------------------*/
  while (true) {
    wSt=wRoot->getChildByName((zxmlNode*&)wGenParamsNode,"generalparameters");
    if (wSt!=ZS_SUCCESS) {
      pErrorLog->logZStatus(
          ZAIES_Error,
          wSt,
          "DicEdit::loadGenerateParameters-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
          "generalparameters",
          decode_ZStatus(wSt));
      break;
    }
    wSt=XMLgetChildText( wGenParamsNode,"targetdirectorypath",wKeyword,pErrorLog);
    if (wSt==ZS_SUCCESS) {
      GenPath = wKeyword;
    }
    break;
  }// while true

  XMLderegister(wGenParamsNode);

  wSt=XMLgetChildText( wRoot,"commentbanner",CommentBanner,pErrorLog);


  /*------------------ include files per object types -----------------------*/

  while (true) {
    wSt=wRoot->getChildByName((zxmlNode*&)wIncludeFileNode,"includefiles");
    if (wSt!=ZS_SUCCESS) {
      pErrorLog->logZStatus(
          ZAIES_Error,
          wSt,
          "DicEdit::loadGenerateParameters-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
          "includefiles",
          decode_ZStatus(wSt));
      break;
    }

    wSt=wIncludeFileNode->getFirstChild((zxmlNode*&)wObjectNode);

    while (wSt==ZS_SUCCESS) {
      if (wObjectNode->getName()=="object") {

        wSt=XMLgetChildText( wObjectNode,"ztype",wKeyword,pErrorLog);
        wZType = encode_ZType(wKeyword);

        wSt=XMLgetChildText( wObjectNode,"file",wIncludeFile,pErrorLog);
        long wIncludeRank=0;
        for (;wIncludeRank < pGenIncludeList.count();wIncludeRank++) {
          if (pGenIncludeList[wIncludeRank].Include ==  wIncludeFile ) {
            break;
          }
        }
        if (wIncludeRank==pGenIncludeList.count()) {
          wIncludeRank = pGenIncludeList.push(GenInclude(wIncludeFile));
        }

        pGenObjList.push(GenObj(wZType,wIncludeRank));
      }//if (wObjectNode->getName()=="object")
      wSt=wObjectNode->getNextNode((zxmlNode*&)wSwapNode);
      XMLderegister(wObjectNode);
      wObjectNode=wSwapNode;
    }//while (wSt==ZS_SUCCESS)

    pErrorLog->textLog("_________________Generation parameters load report____________________\n"
                       " %ld object types %ld include files.", pGenObjList.count(),pGenIncludeList.count());
    pErrorLog->textLog("     Object types");

    for (long wi=0;wi<pGenObjList.count();wi++) {
      pErrorLog->textLog("%3ld- <%25s> <%s>",wi,
          decode_ZType( pGenObjList[wi].ZType),
          pGenObjList[wi].IncludeRank < 0?"***MISS INCLUDE***":pGenIncludeList[pGenObjList[wi].IncludeRank].Include.toCChar()
          );
    }
    pErrorLog->textLog("     include file definitions ");
    for (long wi=0;wi<pGenIncludeList.count();wi++) {
      pErrorLog->textLog("%3ld- <%25s>",wi,
          pGenIncludeList[wi].Include.toString()
          );
    }
    pErrorLog->textLog("________________________________________________________________");
    XMLderegister(wObjectNode);
    XMLderegister(wIncludeFileNode);
    XMLderegister(wRoot);
    break;
  } // while (true)


  XMLderegister((zxmlNode *&) wParamRootNode);
  if (wSt==ZS_EOF)
    return ZS_SUCCESS;
  return wSt;


}// loadGenerateParameters


/*

utf8VaryingString
ZCppGenerate::genCopyFrom(utf8VaryingString& pClassName) {
  utf8VaryingString wReturn;
  wReturn.sprintf(wSCopyFromBegin,pClassName.toCChar(),pClassName.toCChar(),pClassName.toCChar());

  for (long wi=0 ; wi < DictionaryFile->count(); wi++) {
    wReturn.addsprintf(wSCopyFromLine,
        DictionaryFile->Tab[wi].getName().toString(),DictionaryFile->Tab[wi].getName().toString());
  }
  wReturn += "  return *this;\n";
  wReturn += wSMethodEnd ;
  return wReturn;
} // genCopyFrom
*/


utf8VaryingString
ZCppGenerate::genIncludes(ZTypeBase pType) {

  utf8VaryingString wFileInclude;
  /* search for include files when classes */
  if (pType & ZType_Atomic ) {
    for (long wR=0; wR < GenObjList.count();wR++) {
      if (GenObjList[wR].ZType == ZType_Atomic) {
        if (!GenIncludeList[GenObjList[wR].IncludeRank].Used) {
          wFileInclude.sprintf("#include <%s>\n",
              GenIncludeList[GenObjList[wR].IncludeRank].Include.toString());
          GenIncludeList[GenObjList[wR].IncludeRank].Used=true;
        }
        return wFileInclude;
      }
    }// for (long wR=0; wR < pGenObjList.count();wi++)
  }

  //  if ((pType & ZType_Class) ||(pType == ZType_bitset)){ /* classes have an include file associated to it */
  if (pType & (ZType_Class | ZType_ByteSeq) ) { /* classes have an include file associated to it */
    for (long wR=0; wR < GenObjList.count();wR++) {
      if (GenObjList[wR].ZType == (pType & ~(ZType_Pointer | ZType_Array)) ) {
        if (!GenIncludeList[GenObjList[wR].IncludeRank].Used) {
          wFileInclude.sprintf("#include <%s>\n",
              GenIncludeList[GenObjList[wR].IncludeRank].Include.toString());
          GenIncludeList[GenObjList[wR].IncludeRank].Used=true;
        }
        return wFileInclude;
      }
    }// for (long wR=0; wR < pGenObjList.count();wi++)
  } // if (DictionaryFile->Tab[wi].ZType & ZType_Class)

  return utf8VaryingString("");
} //genIncludes


utf8VaryingString
ZCppGenerate::genHeaderFields(utf8VaryingString& pFileIncludeList) {

  utf8VaryingString wReturn , wSingleVar;

  ZStatus wSt;

  for (long wi=0 ; wi < DictionaryFile->count(); wi++) {
     wSt= ZTypeToCTypeDefinition( DictionaryFile->Tab[wi].ZType,
                                  DictionaryFile->Tab[wi].Capacity,
                                  DictionaryFile->Tab[wi].getName(),
                                  wSingleVar,
                                  &ErrorLog);

      wReturn += "    ";
      wReturn += wSingleVar;
      if (wSt!=ZS_SUCCESS) {
        ErroredFields.push(wi);
        wReturn += "\n";
      }
      else {
        if (!DictionaryFile->Tab[wi].ToolTip.isEmpty()) {
          wReturn.addsprintf(" /*!< %s */" ,  DictionaryFile->Tab[wi].ToolTip.toString());
        }
      wReturn += ";\n";
      }
    /* search for include files when classes */

      pFileIncludeList += genIncludes(DictionaryFile->Tab[wi].ZType);
  } // for

  return wReturn;
}//genHeaderFields


/*
<?xml version='1.0' encoding='UTF-8'?>
 <zcppgenerate version = "'0.30-0'">
     <generalparameters>
        <targetdirectorypath>/home/gerard/Development/zmftest/</targetdirectorypath>
     </generalparameters>

     <includefiles>
        <object>
            <ztype>ZType_Utf8FixedString</type>
            <file>ztoolset/utffixedstring.h</file>
         </object>
    </includefiles>
  </zcppgenerate>
*/

ZStatus
ZCppGenerate::loadGenerateParameters(const uriString& pXmlFile,ZaiErrors *pErrorLog) {
  ZStatus wSt;
  const char* wWDParam=getenv(__PARSER_PARAM_DIRECTORY__);
  if (!wWDParam)
    wWDParam="";
  const char* wWDWork=getenv(__PARSER_WORK_DIRECTORY__);
  if (!wWDWork)
    wWDWork="";

  if (pXmlFile.isEmpty()) {
    XmlGenParamsFile = wWDParam;
    XmlGenParamsFile.addConditionalDirectoryDelimiter();
    XmlGenParamsFile += __CPPGENERATE_PARAMETER_FILE__;
  }
  wSt=loadXmlParameters(XmlGenParamsFile,GenObjList,GenIncludeList,pErrorLog);
  if (wSt!=ZS_SUCCESS) {
    std::cout.flush();
    ZExceptionDLg::displayLast("Load parameters file",false);
  }
  return wSt;
}//loadGenerateParameters


bool
ZCppGenerate::testErrored(long pRank) {
    for (long wj=0;wj < ErroredFields.count();wj++) {
      if (ErroredFields[wj]==pRank) {
        return true;
      }// if
    }// for
    return false;
}



ZStatus
ZCppGenerate::genHeader(const utf8VaryingString& pClassName,
                        const uriString& pHeaderFile,
                        const utf8VaryingString &pBrief) {


  const char* wHClassEnd =
      "}; // %s \n";

  const char* wHEndClass =
      //    "#pragma pack(pop)\n"
      "#endif //%s_H\n";


  const char* cst_getKeyMain =
      "   ZDataBuffer       getKey(long pKeyNumber) ;\n"
      "   utf8VaryingString getKeyName(long pKeyNumber) ;\n";
  ZStatus wSt;
  utf8VaryingString wHContent,wIncludeContent,wFieldsContent,wHBanner;
  utf8VaryingString wClassUpper = pClassName.toUpper().toString();

  wHContent.addsprintf(wHHeader,
      wClassUpper.toString(),
      wClassUpper.toString());

  /* add bitset as default include file */

  wIncludeContent += "/*      private include files        */\n";
  wIncludeContent += genIncludes(ZType_bitset);
  wIncludeContent += "/*      local include files          */\n";

  wFieldsContent += genHeaderFields(wIncludeContent);

  wHContent += wIncludeContent;

  wHContent.addsprintf(wHClass,
      pClassName.toCChar(),
      pBrief.toString(),
      pClassName.toCChar());


  wHContent += wFieldsContent;

  wHContent.addsprintf(wHMethods,
      pClassName.toCChar(),  /* CTor */
      pClassName.toCChar(), /* _copyFrom */
      pClassName.toCChar(),

      pClassName.toCChar(), /* operator = */
      pClassName.toCChar(),

      DictionaryFile->KeyDic.count() /* getKeysNumber */
      );

  wHContent += genHeaderKeys();

  wHContent += cst_getKeyMain;

  wHContent.addsprintf(wHClassEnd,
      pClassName.toCChar());

  wHContent.addsprintf(wHEndClass,wClassUpper.toString());


  /* ---------- effective write to header file  ------------------*/

  utf8VaryingString wDic ;
  if (DictionaryFile->getURIContent().isEmpty())
    wDic = XmlGenParamsFile;
  else
    wDic = DictionaryFile->getURIContent() ;

  if (ErroredFields.count()==0) {
      wHBanner.sprintf(GenerateBanner,
                      __CPPGENERATE_VERSION__,
                      wDic.toString(),
                      ZDateFull::currentDateTime().toFormatted().toString());
  }
  else
  {
    wHBanner.sprintf(GenerateBanner,
        __CPPGENERATE_VERSION__,
        wDic.toString(),
        ZDateFull::currentDateTime().toFormatted().toString());
  }
  wSt=pHeaderFile.writeContent(wHBanner);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Write header file",false);
    return wSt;
  }
  wSt=pHeaderFile.appendContent(wHContent);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Append header file",false);
    return wSt;
  }
  return ZS_SUCCESS;
} // genHeader

utf8VaryingString
ZCppGenerate::genHeaderKeys() {
  utf8VaryingString wReturn;
  wReturn = "   /* Keys definition */\n";

  const char* cst_HKey=
      "   ZDataBuffer getKey%ld(); /*  <%s> */\n"
      "   utf8VaryingString getKey%ld_Name();\n";

  for (long wi=0 ; wi < DictionaryFile->KeyDic.count() ; wi ++ ) {

    ZKeyDictionary* wKeydic = DictionaryFile->KeyDic[wi];
    if (!wKeydic->ToolTip.isEmpty()) {
      utf8VaryingString wKeyName = wKeydic->DicKeyName;
      wKeyName.replace(" ","_");
      wReturn.addsprintf("    /** @brief %s %s */\n",
          wKeyName.toString(),
          wKeydic->ToolTip.toString()
          );
    }
    wReturn.addsprintf(cst_HKey,wi,wKeydic->DicKeyName.toString(),wi);
  }

  return wReturn;
}// genHeaderKeys


utf8VaryingString
ZCppGenerate::genCppKeys(const utf8VaryingString& pClassName) {
  utf8VaryingString wReturn;
  wReturn = "   /* Keys */\n";

  const char* cst_CppKeyName =
      " utf8VaryingString %s::getKey%ld_Name() {\n"
      "   return \"%s\";\n"
      " }\n";

  const char* cst_CppKeyHeader=
      " ZDataBuffer %s::getKey%ld() {\n"
      "   ZDataBuffer wReturn;\n"
//      "   ZTypeBase wType;\n\n"
//      "   size_t wUniversalSize=0;\n"
      ;
/*
  const char* cst_CppKeyAtomicSize=
      "   wUniversalSize += getAtomicUniversalSize_T<%s>(%s);\n";
  const char* cst_CppKeyClassSize=
      "   wUniversalSize += %s.getUniversalSize();\n";

  const char* cst_CppKeyMiddle=
      "\n"
      "   unsigned char* wPtr=wReturn.allocate(wUniversalSize+sizeof(uint32_t));\n"
      "   _exportAtomicPtr<uint32_t>(uint32_t(wUniversalSize,wPtr));\n\n"
      ;
*/
//  const char* cst_CppKeyAtomicMove=
//      "   _getAtomicUfN_Ptr<%s>(%s,wPtr);\n";

//  const char* cst_CppKeyClassMove=
//      "   %s.getUniversal_Ptr(wPtr);\n";

  const char* cst_CppKeyTrailer=
      "   return wReturn;\n"
      " } // getKey%ld \n";

  const char* cst_CppKeyAtomicURFMove=
      "   /* export atomic data following URF rules */\n"
      "   _exportAtomic<ZTypeBase>(%s,wReturn);\n"
      "   _exportAtomic<%s>(%s,wReturn);\n"
      ;
  const char* cst_CppKeyClassURFMove=
      "   /* export class using embedded URF export routine */\n"
      "   %s._exportURF(wReturn);\n";


  const char* cst_getKeyMainHeader =
      "   ZDataBuffer %s::getKey(long pKeyNumber) {\n"
      "     switch (pKeyNumber) {\n"
      ;
  const char* cst_getKeyMainSwitch =
      "       case %ld :\n"
      "         return getKey%ld();\n"
      ;
  const char* cst_getKeyMainTrailer =
      "       default :\n"
      "         fprintf(stderr,\"%s::getKey-E-INVVALUE Invalid key number <%s> while expected [0,%ld].\\n\",pKeyNumber);\n"
      "         return ZDataBuffer();\n"
      "     } // switch\n"
      "   } // getKey\n"
      ;

  const char* cst_getKeyNameHeader =
      "   utf8VaryingString %s::getKeyName(long pKeyNumber) {\n"
      "     switch (pKeyNumber) {\n"
      ;
  const char* cst_getKeyNameSwitch =
      "       case %ld :\n"
      "         return getKey%ld_Name();\n"
      ;
  const char* cst_getKeyNameTrailer =
      "       default :\n"
      "         fprintf(stderr,\"%s::getKey-E-INVVALUE Invalid key number <%s> while expected [0,%ld].\\n\",pKeyNumber);\n"
      "         return utf8VaryingString();"
      "     } // switch\n"
      "   } // getKeyName\n"
      ;

  const char* cst_getAllKeys=
      "ZArray<ZDataBuffer> %s::getAllKeys() {\n"
      "   ZArray<ZDataBuffer> wReturn;\n"
      "   for (long wi=0; wi < %ld ; wi++) {\n"
      "     wReturn.push(getKey(wi));\n"
      "   }\n"
      "   return wReturn;\n"
      " } // getAllKeys\n"
      ;



  for (long wi=0 ; wi < DictionaryFile->KeyDic.count() ; wi ++ ) {

    ZKeyDictionary* wKeydic = DictionaryFile->KeyDic[wi];
    /* generate get key name facility */
    wReturn.addsprintf(cst_CppKeyName,pClassName.toString(), wi,wKeydic->DicKeyName.toString());

    /* generate universal move method */
    /* method header */
    wReturn.addsprintf(cst_CppKeyHeader, pClassName.toString(),wi);
#ifdef __DEPRECATED__
    /* generate universal size computation */
    for (long wj=0;wj < wKeydic->count(); wj++) {

      long wDicRank = wKeydic->Tab[wj].MDicRank;

      if (DictionaryFile->Tab[wDicRank].ZType & ZType_Atomic) {
        wReturn.addsprintf(cst_CppKeyAtomicSize,
                           ZTypeToCType(DictionaryFile->Tab[wDicRank].ZType,1),
                           DictionaryFile->Tab[wDicRank].getName().toString());
      }
      else {
        wReturn.addsprintf(cst_CppKeyClassSize,
            DictionaryFile->Tab[wDicRank].getName().toString());
      }

    } // for each field to size

    wReturn += cst_CppKeyMiddle;
#endif // __DEPRECATED__

    /* generate URF data move */
    for (long wj=0;wj < wKeydic->count(); wj++) {
      long wDicRank = wKeydic->Tab[wj].MDicRank;

      if (DictionaryFile->Tab[wDicRank].ZType & ZType_Atomic) {
        wReturn.addsprintf(cst_CppKeyAtomicURFMove,
            decode_ZType(DictionaryFile->Tab[wDicRank].ZType),
            ZTypeToCType(DictionaryFile->Tab[wDicRank].ZType,1),
            DictionaryFile->Tab[wDicRank].getName().toString());
      }
      else {
        wReturn.addsprintf(cst_CppKeyClassURFMove,
            DictionaryFile->Tab[wDicRank].getName().toString());
      }

    } // for each field to move

    wReturn.addsprintf( cst_CppKeyTrailer , wi) ;

  }// for each key dictionary

  wReturn.addsprintf(cst_getKeyMainHeader,pClassName.toString());

  for (long wi=0;wi < DictionaryFile->KeyDic.count(); wi++) {
    wReturn.addsprintf(cst_getKeyMainSwitch,wi,wi);
  }

  wReturn.addsprintf( cst_getKeyMainTrailer,
                      pClassName.toString(),
                      "%ld",
                      DictionaryFile->KeyDic.count()-1);


  wReturn.addsprintf(cst_getKeyNameHeader,pClassName.toString());

  for (long wi=0;wi < DictionaryFile->KeyDic.count(); wi++) {
    wReturn.addsprintf(cst_getKeyNameSwitch,wi,wi);
  }

  wReturn.addsprintf( cst_getKeyNameTrailer,
                      pClassName.toString(),
                      "%ld",
                      DictionaryFile->KeyDic.count()-1);

  wReturn.addsprintf(cst_getAllKeys,pClassName.toString(), DictionaryFile->KeyDic.count());

  return wReturn;
} // genCppKeys

ZStatus
ZCppGenerate::genCpp( const utf8VaryingString& pClassName,
                      const uriString& pHeaderFile,
                      const uriString& pCppFile) {
  ZStatus wSt;
  int wErroredInstructions=0;
  utf8VaryingString wCppContent,wCppBanner;

  const char* wCppHeader =
      "#include \"%s\"\n"
      "#include <ztoolset/zatomicconvert.h>\n"
      "#include <zindexedfile/zdataconversion.h>\n\n"
      "\n using namespace zbs;\n"
      ;

  const char* wCppFromRecordBegin =
      "ZStatus %s::fromRecord(const ZDataBuffer& pRecord) { \n"
      "   const unsigned char* wPtr = pRecord.Data;\n"
      "   const unsigned char* wPtrEnd = pRecord.Data + pRecord.Size;\n"
//      "   ZBitset FieldPresence;\n"
      "\n"
//      "   if ((wPtr + FieldPresence.getURFSize()) >= wPtrEnd) {\n"
//      "     fprintf(stderr, \"%s::fromRecord-F-OUTBOUND Out of input boundaries while getting field <FieldPresence>.\\n\");\n"
//      "     return ZS_OUTBOUND;\n"
//      "   }\n"
      "   FieldPresence.clear();\n"
      "   FieldPresence._importURF(wPtr);\n"
      "\n"
      "   uint64_t wURFSize=0;\n"
      "\n"
      "   if ((wPtr + sizeof(uint64_t)) >= wPtrEnd) {\n"
      "     fprintf(stderr, \"%s::fromRecord-F-OUTBOUND Out of input boundaries while getting field <wURFSize>.\\n\");\n"
      "     return ZS_OUTBOUND;\n"
      "   }\n"
      "   _importAtomic<uint64_t>(wURFSize,wPtr);  /* size is in universal format but not in URF (no preceeding URF header) */\n"
      "   int wRank=0;\n";


  const char* wCppFromMoveErrored =
      "     /* Cannot import from record an errored field <%s>   */\n"
      "     wRank++; /* skipping field rank  */\n";
  const char* wCppFromMoveClass =
      "     if (FieldPresence.test(wRank++)) {\n"
      "       if ((wPtr + %s.getURFSize()) > wPtrEnd) {\n"
      "         fprintf(stderr, \"%s::fromRecord-F-OUTBOUND Out of input boundaries while getting field <%s>.\\n\");\n"
      "         return ZS_OUTBOUND;\n"
      "       }\n"
      "       %s._importURF(wPtr);\n"
      "     }\n"
      ;

  const char* wCppFromMoveAtomic =
      "   if (FieldPresence.test(wRank++)) {\n"
      "     if (wPtr + getAtomicURFSize<%s>(%s) > wPtrEnd) {\n"
      "       fprintf(stderr, \"%s::fromRecord-F-OUTBOUND Out of input boundaries while getting field <%s>.\\n\");\n"
      "       return ZS_OUTBOUND;\n"
      "     }\n"
      "     importAtomicURF<%s>(%s,wPtr);\n"
      "   }\n"
      ;



  wCppContent.addsprintf(wCppHeader,pHeaderFile.toString(), pClassName.toCChar(),pClassName.toCChar());

  /* _copyFrom expanded method */
  wCppContent.addsprintf(wSCopyFromBegin,pClassName.toCChar(),pClassName.toCChar(),pClassName.toCChar());
  for (long wi=0 ; wi < DictionaryFile->count(); wi++) {
    if (testErrored(wi)) {
      wCppContent.addsprintf(wSCopyFromLineErrored, DictionaryFile->Tab[wi].getName().toString());
      wErroredInstructions ++;
    }
    else
      wCppContent.addsprintf(wSCopyFromLine, DictionaryFile->Tab[wi].getName().toString(),DictionaryFile->Tab[wi].getName().toString());
  }// for
  wCppContent += "    return *this;\n";
  wCppContent.addsprintf(wSMethodEnd,"_copyFrom");

  /* move to record */
  wCppContent.addsprintf(wCppToRecordBegin,pClassName.toCChar(),DictionaryFile->count());

  /* size computation */

  for (long wi=0; wi < DictionaryFile->count() ; wi ++) {

    if (testErrored(wi)) {
      wCppContent.addsprintf(wCppToRSizeClassErrored, DictionaryFile->Tab[wi].getName().toString());
      wErroredInstructions ++;
    }
    else {
      if (DictionaryFile->Tab[wi].ZType & ZType_Atomic) {
        wCppContent.addsprintf(wCppToRSizeAtomic,
            ZTypeToCType(DictionaryFile->Tab[wi].ZType,DictionaryFile->Tab[wi].Capacity),
            DictionaryFile->Tab[wi].getName().toCChar());
      } else {
        wCppContent.addsprintf(wCppToRSizeClass,DictionaryFile->Tab[wi].getName().toCChar());
      }
    }

  }// for

  wCppContent += wCppToRecordMiddle ;

  /* effective move */
  for (long wi=0; wi < DictionaryFile->count() ; wi ++) {

    if (testErrored(wi)) {
      wCppContent.addsprintf(wCppToRecordMoveErrored, DictionaryFile->Tab[wi].getName().toString());
      wErroredInstructions ++;
    }
    else {
      if (DictionaryFile->Tab[wi].ZType & ZType_Atomic) {
        wCppContent.addsprintf(wCppToRecordMoveAtomic,
            ZTypeToCType(DictionaryFile->Tab[wi].ZType,DictionaryFile->Tab[wi].Capacity),
            DictionaryFile->Tab[wi].getName().toCChar());
      } else {
        wCppContent.addsprintf(wCppToRecordMoveClass,DictionaryFile->Tab[wi].getName().toCChar());
      }
    }

  }// for
  wCppContent += "    return wReturn;\n";
  wCppContent.addsprintf(wSMethodEnd,"toRecord");

  /* from record */
  wCppContent.addsprintf(wCppFromRecordBegin,pClassName.toCChar(),pClassName.toCChar(),pClassName.toCChar());

  /* effective import from record */
  for (long wi=0; wi < DictionaryFile->count() ; wi ++) {

    if (testErrored(wi)) {
      wCppContent.addsprintf(wCppFromMoveErrored, DictionaryFile->Tab[wi].getName().toString());
      wErroredInstructions ++;
    }
    else {
      if (DictionaryFile->Tab[wi].ZType & ZType_Atomic) {
        wCppContent.addsprintf(wCppFromMoveAtomic,
            ZTypeToCType(DictionaryFile->Tab[wi].ZType,DictionaryFile->Tab[wi].Capacity),
            DictionaryFile->Tab[wi].getName().toString(),
            pClassName.toString(),
            DictionaryFile->Tab[wi].getName().toString(),

            ZTypeToCType(DictionaryFile->Tab[wi].ZType,DictionaryFile->Tab[wi].Capacity),
            DictionaryFile->Tab[wi].getName().toCChar());
      } else {
        wCppContent.addsprintf(wCppFromMoveClass,
            DictionaryFile->Tab[wi].getName().toString(),
            pClassName.toString(),
            DictionaryFile->Tab[wi].getName().toString(),

            DictionaryFile->Tab[wi].getName().toString());
      }
    }

  }// for

  wCppContent += "    return ZS_SUCCESS ;\n";
  wCppContent.addsprintf(wSMethodEnd,"fromRecord");

  wCppContent += genCppKeys(pClassName);

  /* ---------- effective write to cpp file  ------------------*/
  utf8VaryingString wDic ;
  if (DictionaryFile->getURIContent().isEmpty())
    wDic = XmlGenParamsFile;
  else
    wDic = DictionaryFile->getURIContent() ;

  if (wErroredInstructions==0) {
    wCppBanner.sprintf(GenerateBanner,
                      __CPPGENERATE_VERSION__,
                      wDic.toString(),
                      ZDateFull::currentDateTime().toFormatted().toString());
  }
  else
  {
    wCppBanner.sprintf(GenerateCppBannerErrored,
                      __CPPGENERATE_VERSION__,
                      wDic.toString(),
                      ZDateFull::currentDateTime().toFormatted().toString(),
                      wErroredInstructions);
  }
  wSt=pCppFile.writeContent(wCppBanner);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Write header file",false);
    return wSt;
  }
  wSt=pCppFile.appendContent(wCppContent);
  if (wSt!=ZS_SUCCESS) {
    ZExceptionDLg::displayLast("Append header file",false);
    return wSt;
  }

  return ZS_SUCCESS;
}// genCpp


ZStatus
ZCppGenerate::generateInterface(const utf8VaryingString& pOutBase,
                                const utf8VaryingString& pClassName,
                                const utf8VaryingString& pBrief,
                                const uriString& pGenPath) {
  ZStatus wSt=ZS_SUCCESS;

//  uriString GenHeaderFile, GenCppFile;

  utf8VaryingString wHBanner,wCppBanner,wHContent,wFieldsContent, wIncludeContent ;
//  utf8VaryingString wClassUpper = pClassName.toUpper().toString();
  utf8VaryingString wClassLower = pClassName.toLower().toString();

  utf8VaryingString wBase= wClassLower;

  const char* wWDWork=getenv(__PARSER_WORK_DIRECTORY__);
  if (!wWDWork)
    wWDWork="";

  /* directory path precedence :
   *    1- pGenPath
   *    2- GenPath
   *    3- computed from environment variable __PARSER_WORK_DIRECTORY__
   *    4- current directory
   */
  if (pOutBase.isEmpty()){
    wBase  = wClassLower;
  }
  else {
    wBase  = pOutBase;
  }

  while (true) {
      if (pGenPath.isEmpty()) {
        if (GenPath.isEmpty()) {
          if (GenHeaderFile.isEmpty()) {
            GenHeaderFile = wWDWork;
             GenCppFile = wWDWork;
            break;
          }
        }
        /* GenPath is not empty */
          GenHeaderFile = GenPath;
           GenCppFile = GenPath;
          break;
      }//if (pGenPath.isEmpty())

      /* pGenPath is not empty */
      GenHeaderFile = pGenPath;
       GenCppFile = pGenPath;
      break;
  } // while true

  GenHeaderFile.addConditionalDirectoryDelimiter();
  GenHeaderFile += wBase ;
  GenHeaderFile += ".h";

   GenCppFile.addConditionalDirectoryDelimiter();
   GenCppFile += wBase ;
   GenCppFile += ".cpp";

  /* Code generation */

  wSt = genHeader(pClassName,GenHeaderFile,pBrief);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }
  /* cpp file generation */

  wSt = genCpp(pClassName,GenHeaderFile, GenCppFile);

  return wSt;
}



ZStatus ZTypeToCTypeDefinition (ZTypeBase pType,
                                long pCapacity,
                                const utf8VaryingString& pName,
                                utf8VaryingString& pDeclaration,
                                ZaiErrors* pErrorLog)
{

  ZTypeBase wType = pType;


  pDeclaration.clear();

  if (pType==ZType_Nothing) {
    pDeclaration.sprintf("/* Null type for field %s */",pName.toCChar());
    ZException.setMessage("ZTypeToCTypeDefinition",ZS_INVTYPE,Severity_Error,
        "Field <%s> has a null type ZType_Nothing.",pName.toString());
    pErrorLog->logZException();
    return  ZS_INVTYPE;
  }
  if (pType==ZType_Unknown) {
    pDeclaration.sprintf("/* Unknown type for field <%s> */",pName.toCChar());
    ZException.setMessage("ZTypeToCTypeDefinition",ZS_INVTYPE,Severity_Error,
        "Field <%s> has an unknown type ZType_Unknown",pName.toString());
    pErrorLog->logZException();
    return  ZS_INVTYPE;
  }
  ZTypeBase wSType = pType & ZType_StructureMask ;
  ZTypeBase wAType= pType & ZType_AtomicMask ;

  pDeclaration.clear();

  pDeclaration = ZTypeToCType(pType,pCapacity);

  pDeclaration += " ";

  pDeclaration += pName;

  if (pType & ZType_Array) {
    pDeclaration.addsprintf(" [%ld] ",pCapacity);
  }

  return ZS_SUCCESS ;

} // ZTypeToCType

