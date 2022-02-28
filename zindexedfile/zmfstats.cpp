#include "zmfstats.h"

//----------zstatistic---------------------------------
/**
@addtogroup ZMFSTATS

@{
    */

    /**
 * @brief zstatistics::operator - Operator overload : subtracts two zstatistics
 *  @note ZTimer itself remains unchanged : only DeltaTime is changed
 *
 * @param pPMSIn        second term of operator
 * @return a zstatistics structure
 */
ZMFStats
ZMFStats::operator - (const ZMFStats &pIn)
{
  ZMFStats wzstatistics1;
wzstatistics1 = *this;
wzstatistics1.Iterations -= pIn.Iterations;
wzstatistics1.Reads -= pIn.Reads;
wzstatistics1.Delta -= pIn.Delta;

wzstatistics1.ZRFPMS::operator -(pIn);

// ZRFPMS base
/*wzstatistics1.HFHReads -= pIn.HFHReads;
wzstatistics1.HFHWrites -= pIn.HFHWrites;
wzstatistics1.HReservedReads -= pIn.HReservedReads;
wzstatistics1.HReservedWrites -= pIn.HReservedWrites;
wzstatistics1.HFDReads -= pIn.HFDReads;
wzstatistics1.HFDWrites -= pIn.HFDWrites;
wzstatistics1.CBHReads -= pIn.CBHReads;
wzstatistics1.CBHReadBytesSize -= pIn.CBHReadBytesSize;
wzstatistics1.CBHWrites -= pIn.CBHWrites;
wzstatistics1.CBHWriteBytesSize -= pIn.CBHWriteBytesSize;
wzstatistics1.UserReads -= pIn.UserReads;
wzstatistics1.UserWrites -= pIn.UserWrites;
wzstatistics1.UserWriteSize -= pIn.UserWriteSize;
wzstatistics1.UserReadSize -= pIn.UserReadSize;
wzstatistics1.HighWaterWrites -= pIn.HighWaterWrites;
wzstatistics1.HighWaterBytesSize -= pIn.HighWaterBytesSize;
wzstatistics1.ExtentWrites -= pIn.ExtentWrites;
wzstatistics1.ExtentSize -= pIn.ExtentSize;
wzstatistics1.FreeMatches -= pIn.FreeMatches;*/
return  wzstatistics1 ;
}


ZMFStats& ZMFStats::_copyFrom (ZMFStats& pIn)
{
  Iterations=pIn.Iterations;
  Reads=pIn.Reads;
  Writes=pIn.Writes;
  Removes=pIn.Removes;
  Timer=pIn.Timer;
  Delta=pIn.Delta;
  PMSStart=pIn.PMSStart;

  ZRFPMS::_copyFrom(pIn);

  return *this;
}

/**
 * @brief zstatistics::init Initializes statistical monitoring : starts a timer
 */
void
ZMFStats::init (void)
{
  Timer.start();
  ZRFPMS::clear();
  return;
}// init
/**
 * @brief zstatistics::end End monitoring session. Stops the timer and compute delta time.
 * @return a reference to current zstatistics object
 */
ZMFStats &
ZMFStats::end (void)
{
  ZRFPMS wzstatistics1;
  Timer.end();
  *this = wzstatistics1 - PMSStart ;
  return *this ;
}
/**
 * @brief zstatistics::operator -=  Operator overload for zstatistics
 *  @note ZTimer itself remains unchanged : only DeltaTime is changed
 *
 * @param pPMSIn
 * @return
 */
ZMFStats&
ZMFStats::operator -= (ZMFStats &pPMSIn)
{
  Iterations -= pPMSIn.Iterations;
  Reads -= pPMSIn.Reads;
  Delta -= pPMSIn.Delta;


  // ZRFPMS base
  HFHReads -= pPMSIn.HFHReads;
  HFHWrites -= pPMSIn.HFHWrites;
  HReservedReads -= pPMSIn.HReservedReads;
  HReservedWrites -= pPMSIn.HReservedWrites;
  HFCBReads -= pPMSIn.HFCBReads;
  HFCBWrites -= pPMSIn.HFCBWrites;
  CBHReads -= pPMSIn.CBHReads;
  CBHReadBytesSize -= pPMSIn.CBHReadBytesSize;
  CBHWrites -= pPMSIn.CBHWrites;
  CBHWriteBytesSize -= pPMSIn.CBHWriteBytesSize;
  UserReads -= pPMSIn.UserReads;
  UserWrites -= pPMSIn.UserWrites;
  UserWriteSize -= pPMSIn.UserWriteSize;
  UserReadSize -= pPMSIn.UserReadSize;
  HighWaterWrites -= pPMSIn.HighWaterWrites;
  HighWaterBytesSize -= pPMSIn.HighWaterBytesSize;
  ExtentWrites -= pPMSIn.ExtentWrites;
  ExtentSize -= pPMSIn.ExtentSize;
  FreeMatches -= pPMSIn.FreeMatches;
  return *this ;
}

/**
 * @brief zstatistics::operator + operator overload : adds two zstatistics
 *  @note ZTimer itself remains unchanged : only DeltaTime is changed
 * @param pPMSIn
 * @return
 */
ZMFStats
ZMFStats::operator + (const ZMFStats &pPMSIn)
{
  ZMFStats wzstatistics1;
  wzstatistics1 = *this;
  wzstatistics1.Iterations += pPMSIn.Iterations;
  wzstatistics1.Reads += pPMSIn.Reads;
  wzstatistics1.Delta += pPMSIn.Delta;

  // ZRFPMS base
  wzstatistics1.HFHReads += pPMSIn.HFHReads;
  wzstatistics1.HFHWrites += pPMSIn.HFHWrites;
  wzstatistics1.HReservedReads += pPMSIn.HReservedReads;
  wzstatistics1.HReservedWrites += pPMSIn.HReservedWrites;
  wzstatistics1.HFCBReads += pPMSIn.HFCBReads;
  wzstatistics1.HFCBWrites += pPMSIn.HFCBWrites;
  wzstatistics1.CBHReads += pPMSIn.CBHReads;
  wzstatistics1.CBHReadBytesSize += pPMSIn.CBHReadBytesSize;
  wzstatistics1.CBHWrites += pPMSIn.CBHWrites;
  wzstatistics1.CBHWriteBytesSize += pPMSIn.CBHWriteBytesSize;
  wzstatistics1.UserReads += pPMSIn.UserReads;
  wzstatistics1.UserWrites += pPMSIn.UserWrites;
  wzstatistics1.UserWriteSize += pPMSIn.UserWriteSize;
  wzstatistics1.UserReadSize += pPMSIn.UserReadSize;
  wzstatistics1.HighWaterWrites += pPMSIn.HighWaterWrites;
  wzstatistics1.HighWaterBytesSize += pPMSIn.HighWaterBytesSize;
  wzstatistics1.ExtentWrites += pPMSIn.ExtentWrites;
  wzstatistics1.ExtentSize += pPMSIn.ExtentSize;
  wzstatistics1.FreeMatches += pPMSIn.FreeMatches;
  return wzstatistics1 ;
}

/**
 * @brief zstatistics::operator +=  Operator overload for zstatistics
 *  @note ZTimer itself remains unchanged : only DeltaTime is changed
 *
 * @param pPMSIn
 * @return
 */
ZMFStats&
ZMFStats::operator += (ZMFStats &pPMSIn)
{
  Iterations += pPMSIn.Iterations;
  Reads += pPMSIn.Reads;
  Delta += pPMSIn.Delta;


  // ZRFPMS base
  HFHReads += pPMSIn.HFHReads;
  HFHWrites += pPMSIn.HFHWrites;
  HReservedReads += pPMSIn.HReservedReads;
  HReservedWrites += pPMSIn.HReservedWrites;
  HFCBReads += pPMSIn.HFCBReads;
  HFCBWrites += pPMSIn.HFCBWrites;
  CBHReads += pPMSIn.CBHReads;
  CBHReadBytesSize += pPMSIn.CBHReadBytesSize;
  CBHWrites += pPMSIn.CBHWrites;
  CBHWriteBytesSize += pPMSIn.CBHWriteBytesSize;
  UserReads += pPMSIn.UserReads;
  UserWrites += pPMSIn.UserWrites;
  UserWriteSize += pPMSIn.UserWriteSize;
  UserReadSize += pPMSIn.UserReadSize;
  HighWaterWrites += pPMSIn.HighWaterWrites;
  HighWaterBytesSize += pPMSIn.HighWaterBytesSize;
  ExtentWrites += pPMSIn.ExtentWrites;
  ExtentSize += pPMSIn.ExtentSize;
  FreeMatches += pPMSIn.FreeMatches;
  return *this ;
}

/**
 * @brief zstatistics::reportShort displays to pOuput the content of statistical counters
 *
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZMFStats::reportShort(FILE* pOutput)
{
  fprintf(pOutput,
      "________________________________________________\n"
      "               Statistics\n"
      " Iterations %ld\n"
      " Reads      %ld\n"
      " Time        %s\n",
      Iterations,
      Reads,
      Timer.reportDeltaTime().toString());

  return ;
}
/**
 * @brief zstatistics::reportFull displays to pOuput the content of statistical counters plus base ZRFPMS data
 *
 * @param pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZMFStats::reportFull(FILE* pOutput)
{
  reportShort(pOutput);
  reportDetails(pOutput);
  return ;
}

/** @} */ //  ZMFSTATS

