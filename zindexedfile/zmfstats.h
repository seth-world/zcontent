#ifndef ZMFSTATS_H
#define ZMFSTATS_H

#include <zrandomfile/zrfpms.h>

/**
* @addtogroup ZMFSTATS
* @{
*
*
 * @brief The zstatistics struct This object is used to store statistic on performance during searches on Indexes for a ZMasterFile
 *

 *
 *  Each high level operation (zsearch, zadd, etc...) gives its own zstatistics values, specific to the operation,
 * whatever the result is, even in case of failure.
 *
 *
 * As it accesses ZRandomFile, it collects also the whole ZRFPMS data set for the operation.
 *
 *
 */
class ZMFStats : ZRFPMS
{
public:
    long    Iterations;     //!< Number of iterations a seek operation needed
    //
    long    Reads;          //!< Number of ZRandomFile read accesses : zget
    long    Writes;         //!< Number of ZRandomFile write accesses : zwrite
    long    Removes;        //!< Number of ZRandomFile remove accesses : zremove
    ZTimer  Timer;          //!< Timer set for the operation
    ZTime   Delta;          //!< Elapsed
    ZRFPMS  PMSStart;

    explicit ZMFStats() {clear(); }
    ~ZMFStats() {}

    ZMFStats(ZMFStats& pIn) {_copyFrom(pIn);}

    ZMFStats& _copyFrom (ZMFStats& pIn);
    void clear (void) {memset(this,0,sizeof(ZMFStats)); return;}

    void setPMSBase (ZRFPMS  *pZPMS) {ZRFPMS::_copyFrom(*pZPMS); }


    void init (void) ;
    ZMFStats & end(void);

    void reportShort(FILE* pOutput=stdout);
    void reportFull(FILE* pOutput=stdout);

    ZMFStats & operator = (const ZRFPMS& pPMSIn) { memmove (this,&pPMSIn,sizeof(ZRFPMS)); return *this;}
    ZMFStats & operator = (ZMFStats pStatIn) { memmove (this,&pStatIn,sizeof(ZMFStats)); return *this;}

    ZMFStats operator - (const ZMFStats &pIn);
    ZMFStats  operator + (const ZMFStats &pStatIn);

    ZMFStats& operator -= (ZMFStats &pPMSIn);
    ZMFStats& operator += (ZMFStats &pPMSIn);

};

/** @} */ //  ZMFSTATS
#endif // ZMFSTATS_H
