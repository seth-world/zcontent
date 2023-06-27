#ifndef ZSSEARCHARGUMENT_CPP
#define ZSSEARCHARGUMENT_CPP
#include <zrandomfile/zsearchargument.h>
#include <ztoolset/zexceptionmin.h>



#include <ztoolset/zmem.h>
#include <ztoolset/utfvaryingstring.h>

using namespace zbs;

//---------------ZSSearchArgument---------------------------------------------------
/**
 * @brief ZSSearchArgument::add Adds a new search rule
 *
 *  @note it is possible to have multiple rules.
 * Rules are stored in the definition order.
 * Rules are evaluated from the first to the last order of storage (ZArray rank order).
 *
 * @param[in] pFieldOffset  adhoc field offset to search on
 * @param[in] pFieldLength  adhoc field length to search on
 * @param[in] pSequence     Sequence of bytes to appy comparison rule on (a cstring, a binary sequence)
 * @param[in] pSearchType   type of match see @ref ZRFSearch_type
 * @param[in] pOR           a boolean mentionning whether a OR (true) or a AND (false) should be applied with the previous rules result
 * @return  a ZStatus with the following possible values
 * - ZS_SUCCESS : rule has been stored
 * - ZS_INVSIZE : rule is rejected because of the incompatibility of Sequence size.
 */
ZStatus
ZSearchArgument::add(const ssize_t pFieldOffset,
                             const ssize_t pFieldLength,
                             ZDataBuffer &pSequence,
                             const uint16_t pSearchType,
                             const bool pOR)
{
  FreeFieldSearch wNewFFS;
  wNewFFS.SearchSequence=pSequence;
  wNewFFS.FieldLength=pFieldLength;
  wNewFFS.FieldOffset=pFieldOffset;
  wNewFFS.SequenceSize = wNewFFS.SearchSequence.Size;
  wNewFFS.SequenceOffset = 0;
  wNewFFS.SearchType = pSearchType;


  wNewFFS.FFS |= FFS_OR;
  if (wNewFFS.SearchType & ZRFCaseRegardless) {
    wNewFFS.FFS |= FFS_CaseRegardless;
    wNewFFS.SearchType =(uint16_t) (wNewFFS.SearchType &  ~ZRFCaseRegardless) ;  // wipe out ZCaseRegardless
  }
  if (wNewFFS.SearchType & ZRFString) {
    wNewFFS.FFS |= FFS_String;
    wNewFFS.SearchType =(uint16_t)( wNewFFS.SearchType &  ~ZRFString) ;  // wipe out ZRFString

    while ((wNewFFS.SearchSequence.Data[wNewFFS.SequenceSize-1]=='\0')&&(wNewFFS.SequenceSize > 0))
      wNewFFS.SequenceSize--;
  }
  if (wNewFFS.SearchType & ZRFTrimspace) {
    wNewFFS.FFS |= FFS_TrimSpace;
    wNewFFS.SearchType =(uint16_t)( wNewFFS.SearchType &  ~ZRFTrimspace) ;  // wipe out ZRFTrimspace

    /* eliminate leading and trailing blank characters */

    utf8_t* wPtr =  wNewFFS.SearchSequence.Data ;

    utf8_t* wPtrEnd = (wPtr + wNewFFS.SequenceSize - 1);
    while ((*wPtr == ' ') && (wPtr < wPtrEnd))
      wPtr++;
    utf8_t* wPtrBegin = wPtr;
    wPtr=wPtrEnd;
    while ((*wPtr == '\0') && (*wPtr == ' ') && (wPtr > wPtrBegin))
      wPtr--;
    *wPtr = '\0';
    wPtrEnd = wPtr;

    size_t wSize = wPtrEnd - wPtrBegin + 1;

    utf8_t* wDup = zmalloc<utf8_t>(wSize);  /* duplicate string to copy */
    wPtr=wPtrBegin;

    while (wPtr < wPtrEnd) {
      *wDup++ = *wPtr++;
    }
    wNewFFS.SearchSequence.setData(wDup,wSize); /* replace sequence to search */
    zfree(wDup);
  } // if (wNewFFS.SearchType & ZRFTrimspace)

  if (wNewFFS.FieldLength < 1) { // if pLength is ommitted (either 0 or less)
    wNewFFS.FieldLength = wNewFFS.SequenceSize ;
  }

    // optimizing size and compare :
    // if sequence size equals field length, then use ZEqualsTo whatever pSearchType is
//     if (last().SequenceSize==last().FieldLength)
//                     last().SearchType = ZRFEqualsTo ;

  if ((wNewFFS.SearchType==ZRFEqualsTo)&&(last().FieldLength!=wNewFFS.SequenceSize)) {
 //        pop();
    ZException.setMessage(_GET_FUNCTION_NAME_,
                                 ZS_INVSIZE,
                                 Severity_Severe,
                                 "Invalid size of sequence to search <%ld> must be equal to field length <%ld>. Use ZRFStartsWith in place.",
                                 last().SequenceSize,
                                 last().FieldLength
                                 );
    return ZS_INVSIZE;
  }
  return ZS_SUCCESS;
}// AddArgument

/**
 * @brief ZSSearchArgument::evaluate Evaluates all search condition matching a single give record pRecord
 *
 * Evaluation is made in the rule storage order (ZArray rank order) from the first stored to the last stored rule.
 *
 * For each condition argument :
 * - extracts field to evaluate from record
 * - evaluates the conditions STRICTLY from (left to right) the first one to the last one.
 * - AND / OR flag in the defined rule applies to the current member evaluation in regard to the actual result value at the moment of evaluation
 *
 * Examples:
 *
 * - true AND false OR true -> (true AND false)= false OR true -> TRUE
 * - true OR true AND false -> (true OR true)= true AND false -> FALSE
 *
 *@note if adhoc field definition points outside the varying record size, then condition is evaluated to false.
 *
 * @param[in] pRecord a ZDataBuffer containing a record to evaluate
 * @return a boolean : true if evaluation is successful - false if evaluation rejected the record.
 */
bool
ZSearchArgument::evaluate (ZDataBuffer &pRecord)
{
bool FGlobal = true, FCurrent = false;

long wRank = 0;
ZDataBuffer wFieldContent;
    while (wRank<size())
    {
        if ((Tab(wRank).FieldOffset+Tab(wRank).FieldLength)>pRecord.Size) // if outside the varying record size : return false
                return false;
    // field extraction
        wFieldContent.setData(&pRecord.Data[Tab(wRank).FieldOffset],Tab(wRank).FieldLength);
        FCurrent =_testSequence(wFieldContent,
                          Tab(wRank).SearchSequence,
                          Tab(wRank).SequenceSize,
                          Tab(wRank).SearchType,
                          Tab(wRank).FFS);

        if (Tab(wRank).FFS & FFS_OR)
                FGlobal = FGlobal | FCurrent ;
            else
                FGlobal = FGlobal & FCurrent ;
        wRank++;
    }// while

    return FGlobal;
}//evaluate

/**
 * @brief ZSSearchArgument::report Dumps the set of rules contained in ZSSearchArgument
 * @param[in] pOutput Output to which rules will be reported. Defaulted to stdout->
 */
void
ZSearchArgument::report(FILE*pOutput)
{
    fprintf(pOutput,
            "----------Search arguments-----------\n"
            "Field offset Length %16s %14s %11s   Search type\n    Sequence to search\n",
            "String",
            "Case impact",
            "Trim space");
    for (long wi=0;wi < size();wi++)
    {
    utf8VaryingString wFlag;
    wFlag.clear();
    wFlag += Tab(wi).FFS & FFS_String?"ZRFString":"Binary";
    wFlag += Tab(wi).FFS & FFS_CaseRegardless?"|ZRFCaseRegardless":"";
    wFlag += Tab(wi).FFS & FFS_TrimSpace?"|ZRFTrimSpaces":"";
    wFlag += "|";
    wFlag += decode_ZSearchType(Tab(wi).SearchType);
    fprintf(pOutput,
            "%2ld>> Field Offset <%6ld> Length <%6ld> Flags <%s> <%s>\n",
            wi,
            Tab(wi).FieldOffset,
            Tab(wi).FieldLength,
            wFlag.toString(),
            Tab(wi).FFS & FFS_OR?"OR":"AND");
            Tab(wi).SearchSequence.Dump(16,-1,pOutput);
    }// for

}// report



//---------------End ZSSearchArgument-------------------------------



/**
 * @brief ZRandomFile::_testSequence test wether a Field content is selected (ZS_FOUND) or not (ZS_NOTFOUND) according the given selection criterias.
 * @param[in] wFieldContent     Field  (Record segment) to explore
 * @param[in] pSequence         Sequence to find
 * @param[in] wSequenceSize     real sequence size according ZRFString conditions or not
 * @param[in] FString           Are we looking for a string (true) or binary sequence (false)
 * @param[in] FTrimSpace        Request to trim leading and trailing spaces (true)
 * @param[in] FCaseregardless   Should we disregard case (true) or not (false)
 * @param [in]wSearchType       Expurged ZSearch_type  (no ZRFString , no ZRFCaseregardless, no ZRFTrimspace)
 * @return true if the wFieldContent matches the rule condition - false if not
 */
bool
_testSequence(ZDataBuffer &wFieldContent,           // Field  (Record segment) to explore
    ZDataBuffer &pSequence,  // Sequence to find
    ssize_t wSequenceSize,   // real sequence size according ZRFString conditions or not
    uint16_t wSearchType,    // Expurged ZSearch_type  (no ZRFString , ZRFCaseregardles
    uint8_t pFFS)

/*                           bool FString,            // Are we looking for a string (true) or binary sequence (false)
                           bool FTrimSpace,         // Request to trim leading and trailing spaces
                           bool FCaseregardless)    // should we disregard case (true) or not (false)
*/
{
  ssize_t wLength=wFieldContent.Size;
  ssize_t wOffset = 0;

  if ((pFFS & FFS_String)&&(wSearchType==ZRFEndsWith))
  {
    wLength=wFieldContent.Size;
    while ((wFieldContent.Data[wLength-1]==0)&&(wLength>0))
      wLength--;
    if(wLength<wSequenceSize)
      return false;  // not a match next record
  }

  if (pFFS & FFS_TrimSpace)
  {
    wOffset=0;
    while (((wFieldContent.Data[wOffset])==cst_spaceChar)&&(wOffset < wLength))
      wOffset++;
    if (wOffset==wLength)
      return false;
  }

  switch (wSearchType)
  {
  case ZRFEqualsTo :
  {
    if (pFFS & FFS_CaseRegardless)
    {
      if (wFieldContent.bstartwithCaseRegardless(pSequence.Data,wSequenceSize,0)==0)
      {
        return true;
      }
    }// FCaseregardless
    else
        if (memcmp(pSequence.Data,wFieldContent.Data,wSequenceSize)==0) // here put analysis of field content
    {
      return true;
      //                            pMRes.ZTabAddress.push (getAddressFromRank(wi));
    }
    break;
  }// ZRFEqualsTo
  case ZRFStartsWith :
  {
    if (pFFS & FFS_CaseRegardless)
    {
      if (wFieldContent.bstartwithCaseRegardless(pSequence.Data,wSequenceSize,wOffset)==0)
      {
        return true;
      }
    }// FCaseregardless
    else
        if (memcmp(pSequence.Data,wFieldContent.Data,wSequenceSize)==0) //
    {
      return true;
    }
    break;
  }//ZRFStartsWith

  case ZRFEndsWith :
  {
    size_t wOffset = wLength + 1 - wSequenceSize;
    if (pFFS & FFS_CaseRegardless)
    {
      if (wFieldContent.bstartwithCaseRegardless(pSequence.Data,wSequenceSize,wOffset)==0)
      {
        return true;
      }
    }// FCaseregardless
    else
        if (memcmp(pSequence.Data,wFieldContent.Data+wOffset,wSequenceSize)==0) //
    {
      return true;
    }
    break;
  }//ZRFEndsWith

  case ZRFContains :
  {
    if (pFFS & FFS_CaseRegardless)
    {
      if (wFieldContent.bsearchCaseRegardless(pSequence.Data,wSequenceSize,0)>=0)
      {
        return true;
      }
    }
    else
        if (wFieldContent.bsearch(pSequence.Data, wSequenceSize,0)>=0)
    {
      return true;
    }
    break;
  }//ZRFContains


  case ZRFGreaterThan :
  {
    if (pFFS & FFS_CaseRegardless)
    {
      wFieldContent.toUpper();
      pSequence.toUpper();
    }
    return (memcmp(wFieldContent.Data,pSequence.Data,pSequence.Size)>0);
  }//ZRFContains
  case ZRFLessThan :
  {
    if (pFFS & FFS_CaseRegardless)
    {
      wFieldContent.toUpper();
      pSequence.toUpper();
    }
    return (memcmp(wFieldContent.Data,pSequence.Data,pSequence.Size)<0);
  }//ZRFContains
  }//switch

  return false;
} //_testSequence



/**
 * @brief decode_ZSearchType translates ZSearch type in clear text
 * @param pSearchType
 * @return
 */
utf8VaryingString
decode_ZSearchType(const uint16_t pSearchType )
{
  uint16_t wSearchType = pSearchType;
  utf8VaryingString wRet;
  wRet.clear();
  if (wSearchType & ZRFString)
  {
    wRet +=  "ZRFString | ";
    wSearchType = wSearchType  &  ~ZRFString;
  }
  if (wSearchType & ZRFCaseRegardless)
  {
    wRet +=  "ZRFCaseRegardless | ";
    wSearchType = wSearchType  &  ~ZRFCaseRegardless;
  }
  if (wSearchType & ZRFTrimspace)
  {
    wRet +=  "ZRFTrimspace | ";
    wSearchType = wSearchType  &  ~ZRFTrimspace;
  }

  switch (wSearchType)
  {
  case ZRFEqualsTo:
  {
    wRet +=  "ZRFEqualsTo";
    break;
  }
  case ZRFStartsWith:
  {
    wRet +=  "ZRFStartsWith";
    break;
  }
  case ZRFEndsWith:
  {
    wRet +=  "ZRFEndsWith";
    break;
  }
  case ZRFContains:
  {
    wRet +=  "ZRFContains";
    break;
  }
  case ZRFGreaterThan:
  {
    wRet +=  "ZRFGreaterThan";
    break;
  }
  case ZRFLessThan:
  {
    wRet +=  "ZRFLessThan";
    break;
  }
  }// switch
  return wRet;
}// decode_ZSearchType


#endif // ZSSearchArgument_H
