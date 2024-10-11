#ifndef ZSEARCHARGUMENT_CPP
#define ZSEARCHARGUMENT_CPP
#include <zrandomfile/zsearchargument.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/zutfstrings.h> // for utfmessageString

using namespace zbs;

//---------------ZSearchArgument---------------------------------------------------
/**
 * @brief ZSearchArgument::add Adds a new search rule
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
     newBlankElement();
     last().OR = pOR;

     last().FieldLength=pFieldLength;
     last().FieldOffset=pFieldOffset;
     last().SequenceSize = pSequence.Size;
     last().SequenceOffset = 0;

     last().SearchType = pSearchType;

     if (last().SearchType & ZRFCaseRegardless)
              {
              last().FCaseRegardless = true;
              last().SearchType =(ZRFSearch_type) (last().SearchType &  ~ZRFCaseRegardless) ;  // wipe out ZCaseRegardless
              }
      if (last().SearchType & ZRFString)
          {
          last().FString = true;
          last().SearchType =(ZRFSearch_type)( last().SearchType &  ~ZRFString) ;  // wipe out ZRFString

          while (pSequence.DataChar[last().SequenceSize-1]=='\0')
                                                 last().SequenceSize--;
          }
    if (last().SearchType & ZRFTrimspace)
      {
      last().FTrimSpace = true;
      last().SearchType =(ZRFSearch_type)( last().SearchType &  ~ZRFTrimspace) ;  // wipe out ZRFTrimspace

      while (pSequence.DataChar[last().SequenceSize-1]==' ')
                                                 last().SequenceSize--;
      last().SequenceOffset = 0;
      while (pSequence.DataChar[last().SequenceOffset]==' ')
                                                 last().SequenceOffset++;
      }
    // setting up Size to scan
    //
     last().SearchSequence.setData(pSequence.Data+last().SequenceOffset,(last().SequenceSize-last().SequenceOffset));

     if (last().FieldLength<1)  // if pLength is ommitted (either 0 or less)
         {
         last().FieldLength = last().SequenceSize ;
         }

    // optimizing size and compare :
    // if sequence size equals field length, then use ZEqualsTo whatever pSearchType is
//     if (last().SequenceSize==last().FieldLength)
//                     last().SearchType = ZRFEqualsTo ;

     if ((last().SearchType==ZRFEqualsTo)&&(last().FieldLength!=last().SequenceSize))
         {
         pop();
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
 * @brief ZSearchArgument::evaluate Evaluates all search condition matching a single give record pRecord
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
        if ((Tab[wRank].FieldOffset+Tab[wRank].FieldLength)>pRecord.Size) // if outside the varying record size : return false
                return false;
    // field extraction
        wFieldContent.setData(&pRecord.Data[Tab[wRank].FieldOffset],Tab[wRank].FieldLength);
        FCurrent =_testSequence(wFieldContent,
                          Tab[wRank].SearchSequence,
                          Tab[wRank].SequenceSize,
                          Tab[wRank].SearchType,
                          Tab[wRank].FString,
                          Tab[wRank].FTrimSpace,
                          Tab[wRank].FCaseRegardless);

        if (Tab[wRank].OR)
                FGlobal = FGlobal | FCurrent ;
            else
                FGlobal = FGlobal & FCurrent ;
        wRank++;
    }// while

    return FGlobal;
}//evaluate

/**
 * @brief ZSearchArgument::report Dumps the set of rules contained in ZSearchArgument
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
    utfmessageString wFlag;
    wFlag.clear();
    wFlag += Tab[wi].FString?"ZRFString":"Binary";
    wFlag += Tab[wi].FCaseRegardless?"|ZRFCaseRegardless":"";
    wFlag += Tab[wi].FTrimSpace?"|ZRFTrimSpaces":"";
    wFlag += "|";
    wFlag += decode_ZSearchType(Tab[wi].SearchType);
    fprintf(pOutput,
            "%2ld>> Field Offset <%6ld> Length <%6ld> Flags <%s> <%s>\n",
            wi,
            Tab[wi].FieldOffset,
            Tab[wi].FieldLength,
            wFlag.toString(),
            Tab[wi].OR?"OR":"AND");
            Tab[wi].SearchSequence.Dump(16,-1,pOutput);
    }// for

}// report



//---------------End ZSearchArgument-------------------------------



#endif // ZSEARCHARGUMENT_H
