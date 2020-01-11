#ifndef ZSSEARCHARGUMENT_H
#define ZSSEARCHARGUMENT_H
#include <ztoolset/zarray.h>
#include <ztoolset/zbasedatatypes.h>
#include <zrandomfile/zrandomfiletypes.h>
namespace zbs {

#ifndef ZRFSEARCH
#define ZRFSEARCH
/** @ingroup ZRFFreeSearch
 * @brief The ZRFSearch_type enum option to free search within a ZRandomFile
 */

enum ZRFSearch_type : uint16_t
{
    ZRFEqualsTo    = 0x01,    //!< exact search (the whole fields is the whole given sequence of bytes)
    ZRFStartsWith  = 0x02,    //!< Field must starts with given sequence of bytes
    ZRFEndsWith    = 0x04,    //!< Field must end with given sequence of bytes
    ZRFContains    = 0x08,    //!< The given bytes sequence must be found once inside the field
    ZRFGreaterThan = 0x10,    //!< Find anything greater than bytes sequence
    ZRFLessThan    = 0x20,    //!< Find anything lower than bytes sequence

    ZRFCaseRegardless = 0x0100, //!< Do not take care of case. Must be ORED with other Search instruction
    ZRFString         = 0x0200, //!< A cstring is processed (char + ends with '\0'char )
    ZRFTrimspace      = 0x0300  //!< For a cstring : request to trim leading and trailing spaces before test evaluation
};


/** @addtogroup ZRFFreeSearch */


struct FreeFieldSearch_struct
{
ZDataBuffer SearchSequence;     /**< byte sequence or string to search for */
ssize_t     SequenceOffset=0 ,
            SequenceSize=0 ;
uint16_t    SearchType = 0;
bool        FCaseRegardless, FString,FTrimSpace ;
ssize_t     FieldOffset , FieldLength , SizeToCollect;
bool        OR = false; //!< by default any search argument is ANDED. if OR is set, then it is ORED.
// OR apply to link current argument with previous, so that First OR is always set to false
} ;
#endif // ZRFSEARCH
/**
 * @brief The ZSSearchArgument class holds and manages the matching rules for a collection in order
 *
 */
class ZSSearchArgument : private ZArray<FreeFieldSearch_struct>
{
    friend class ZSIndexCollection;
    friend class ZRFCollection;
public:
    typedef ZArray<FreeFieldSearch_struct> _Base;

    friend class ZRFCollection;

    ZSSearchArgument(void) {}
    ~ZSSearchArgument(void) {clear();}

    using _Base::_cloneFrom ;
    using _Base::clone ;

    void clear(void)
    {
    while (size()>0)
                pop();
    }
    long pop(void)
    {
        last().SearchSequence.clear();
        return _Base::pop();
    }

    ZStatus add(const ssize_t pFieldOffset,
                const ssize_t pFieldLength,
                ZDataBuffer &pSequence,
                const uint16_t pSearchType,
                const bool pOR=false) ;

    void report (FILE* pOutput=stdout);

    bool evaluate (ZDataBuffer &pRecord);
};

} // namespace zbs

bool _testSequence(ZDataBuffer &wFieldContent,  // Field  (Record segment) to explore
                           ZDataBuffer &pSequence,       // Sequence to find
                           ssize_t wSequenceSize,          // real sequence size according ZRFString conditions or not
                           uint16_t wSearchType,          // Expurged ZSearch_type  (no ZRFString , ZRFCaseregardles
                           bool FString,                // Are we looking for a string (true) or binary sequence (false)
                           bool FTrimSpace,             // Request to trim leading and trailing spaces
                           bool FCaseregardless);       // should we disregard case (true) or not (false)

const char *decode_ZSearchType(const uint16_t pSearchType );



#endif // ZSSearchArgument_H
