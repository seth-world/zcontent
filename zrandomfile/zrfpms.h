#ifndef ZRFPMS_H
#define ZRFPMS_H

#include <string.h> /* for memset */
#include <stdio.h> /* for FILE*    */

#include <zrandomfile/zrandomfiletypes.h>


/**
 * @brief The ZRFPMS class Performance monitoring system : collects data about read / write operations on content and header during a session.

ZRandomFile collects information about its activity.
    Data is stored within ZRFPMS class associated to ZFileDescriptor.
    This data has a life duration limitated to a session.

    @Note a session starts from the ZRandomFile::_open method call until ZRandomFile::_close call

        It could be reported at any time using ZRandomFile performance reporting method.
    @see ZRandomFile::ZRFPMSReport

*/

class ZRFPMS
{
public:
  ZRFPMS();
  ZRFPMS(const ZRFPMS& pIn) {_copyFrom(pIn);}
  ZRFPMS& _copyFrom(const ZRFPMS& pIn) ;

  long        HFHReads;       /**< file header reads (pure header block) */
  long        HFHWrites;      /**< file header writes (pure header block) */

  long        LockReads;       /**< file header lock infos reads (pure header block) */
  long        LockWrites;      /**< file header lock infos writes (pure header block) */

  long        HReservedReads;     /**< reserved block reads */
  long        HReservedWrites;    /**< reverved block writes */

  long        HFCBReads;       /**< file control block reads */
  long        HFCBWrites;      /**< file control block writes */

  long        CBHReads;           /**< content block header reads */
  zsize_type  CBHReadBytesSize;   /**< content block header bytes read */

  long        CBHWrites;      /**< content block header writes */
  zsize_type  CBHWriteBytesSize;  /**< content block header bytes written */

  long        UserReads;      /**< user content reads */
  zsize_type  UserReadSize;   /**< total of user content reads in bytes */
  long        UserWrites;     /**< user content writes */
  zsize_type  UserWriteSize;  /**< total of user content writes in bytes */

  long        HighWaterWrites;    /**< total of highwater marking writes */
  zsize_type  HighWaterBytesSize; /**< total bytes written while highwatermarking writting */

  long        ExtentWrites;   /**< number of times file has been extended (faults on the free block pool) */
  zsize_type  ExtentSize;     /**< total size of file has been extended. */
  long        FreeMatches;    /**< number of times a block has been found and allocated from free pool. */

  long        FieldReads;     /**< single field content reads */
  zsize_type  FieldReadSize;  /**< total of single field content reads in bytes */
  long        FieldWrites;     /**< single field  content writes */
  zsize_type  FieldWriteSize;  /**< total of single field content writes in bytes */


  ZRFPMS & operator = (const ZRFPMS &pIn) {return _copyFrom(pIn);}
  ZRFPMS  operator - (const ZRFPMS &pPMSIn);
  ZRFPMS  operator + (const ZRFPMS &pPMSIn);

  void clear(void) {memset(this,0,sizeof(ZRFPMS)); return;}
  void reportDetails (FILE*pOutput=stdout);
  void reportDetails (ZaiErrors* pErrorLog) ;

  void PMSCounterRead(ZPMSCounter_type pC, const zsize_type pSize=0);
  void PMSCounterWrite(ZPMSCounter_type pC, const zsize_type pSize=0);

};

#endif // ZRFPMS_H
