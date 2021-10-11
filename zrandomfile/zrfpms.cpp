#include "zrfpms.h"

ZRFPMS::ZRFPMS()
{
  clear();
}

//----------ZRFPMS---------------------------------


ZRFPMS& ZRFPMS::_copyFrom(const ZRFPMS& pIn)
{
  HFHReads=pIn.HFHReads;
  HFHWrites=pIn.HFHWrites;
  LockReads=pIn.LockReads;
  LockWrites=pIn.LockWrites;
  HReservedReads=pIn.HReservedReads;
  HReservedWrites=pIn.HReservedWrites;
  HFDReads=pIn.HFDReads;
  HFDWrites=pIn.HFDWrites;
  CBHReads=pIn.CBHReads;
  CBHReadBytesSize=pIn.CBHReadBytesSize;
  CBHWrites=pIn.CBHWrites;
  CBHWriteBytesSize=pIn.CBHWriteBytesSize;
  UserReads=pIn.UserReads;
  UserReadSize=pIn.UserReadSize;
  UserWrites=pIn.UserWrites;
  UserWriteSize=pIn.UserWriteSize;
  HighWaterWrites=pIn.HighWaterWrites;
  HighWaterBytesSize=pIn.HighWaterBytesSize;
  ExtentWrites=pIn.ExtentWrites;
  ExtentSize=pIn.ExtentSize;
  FreeMatches=pIn.FreeMatches;
  FieldReads=pIn.FieldReads;
  FieldReadSize=pIn.FieldReadSize;
  FieldWrites=pIn.FieldWrites;
  FieldWriteSize=pIn.FieldWriteSize;
  return *this;
}

/**
 * @brief ZRFPMS::operator - Operator overload : subtracts two ZRFPMS
 * @param pPMSIn
 * @return
 */
ZRFPMS
ZRFPMS::operator - (const ZRFPMS &pPMSIn)
{
  ZRFPMS wZRFPMS1;
  wZRFPMS1 = *this;
  wZRFPMS1.HFHReads -= pPMSIn.HFHReads;
  wZRFPMS1.HFHWrites -= pPMSIn.HFHWrites;
  wZRFPMS1.HReservedReads -= pPMSIn.HReservedReads;
  wZRFPMS1.HReservedWrites -= pPMSIn.HReservedWrites;
  wZRFPMS1.HFDReads -= pPMSIn.HFDReads;
  wZRFPMS1.HFDWrites -= pPMSIn.HFDWrites;
  wZRFPMS1.CBHReads -= pPMSIn.CBHReads;
  wZRFPMS1.CBHReadBytesSize -= pPMSIn.CBHReadBytesSize;
  wZRFPMS1.CBHWrites -= pPMSIn.CBHWrites;
  wZRFPMS1.CBHWriteBytesSize -= pPMSIn.CBHWriteBytesSize;
  wZRFPMS1.UserReads -= pPMSIn.UserReads;
  wZRFPMS1.UserWrites -= pPMSIn.UserWrites;
  wZRFPMS1.UserWriteSize -= pPMSIn.UserWriteSize;
  wZRFPMS1.UserReadSize -= pPMSIn.UserReadSize;
  wZRFPMS1.FieldReads -= pPMSIn.FieldReads;
  wZRFPMS1.FieldWrites -= pPMSIn.FieldWrites;
  wZRFPMS1.FieldWriteSize -= pPMSIn.FieldWriteSize;
  wZRFPMS1.FieldReadSize -= pPMSIn.FieldReadSize;
  wZRFPMS1.HighWaterWrites -= pPMSIn.HighWaterWrites;
  wZRFPMS1.HighWaterBytesSize -= pPMSIn.HighWaterBytesSize;
  wZRFPMS1.ExtentWrites -= pPMSIn.ExtentWrites;
  wZRFPMS1.ExtentSize -= pPMSIn.ExtentSize;
  wZRFPMS1.FreeMatches -= pPMSIn.FreeMatches;
  return wZRFPMS1 ;
}
/**
 * @brief ZRFPMS::operator + operator overload : adds two ZRFPMS
 * @param pPMSIn
 * @return
 */
ZRFPMS
ZRFPMS::operator + (const ZRFPMS &pPMSIn)
{
  ZRFPMS wZRFPMS1;
  wZRFPMS1 = *this;
  wZRFPMS1.HFHReads += pPMSIn.HFHReads;
  wZRFPMS1.HFHWrites += pPMSIn.HFHWrites;
  wZRFPMS1.HReservedReads += pPMSIn.HReservedReads;
  wZRFPMS1.HReservedWrites += pPMSIn.HReservedWrites;
  wZRFPMS1.HFDReads += pPMSIn.HFDReads;
  wZRFPMS1.HFDWrites += pPMSIn.HFDWrites;
  wZRFPMS1.CBHReads += pPMSIn.CBHReads;
  wZRFPMS1.CBHReadBytesSize += pPMSIn.CBHReadBytesSize;
  wZRFPMS1.CBHWrites += pPMSIn.CBHWrites;
  wZRFPMS1.CBHWriteBytesSize += pPMSIn.CBHWriteBytesSize;
  wZRFPMS1.UserReads += pPMSIn.UserReads;
  wZRFPMS1.UserWrites += pPMSIn.UserWrites;
  wZRFPMS1.UserWriteSize += pPMSIn.UserWriteSize;
  wZRFPMS1.UserReadSize += pPMSIn.UserReadSize;
  wZRFPMS1.FieldReads += pPMSIn.FieldReads;
  wZRFPMS1.FieldWrites += pPMSIn.FieldWrites;
  wZRFPMS1.FieldWriteSize += pPMSIn.FieldWriteSize;
  wZRFPMS1.FieldReadSize += pPMSIn.FieldReadSize;
  wZRFPMS1.HighWaterWrites += pPMSIn.HighWaterWrites;
  wZRFPMS1.HighWaterBytesSize += pPMSIn.HighWaterBytesSize;
  wZRFPMS1.ExtentWrites += pPMSIn.ExtentWrites;
  wZRFPMS1.ExtentSize += pPMSIn.ExtentSize;
  wZRFPMS1.FreeMatches += pPMSIn.FreeMatches;
  return wZRFPMS1 ;
}

/**
 * @brief ZRFPMS::reportDetails detailed report of PMS data
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. Defaulted to stdout.
 */
void
ZRFPMS::reportDetails (FILE*pOutput)
{
  long long wAverageUserWrite;
  if ((UserWrites)==0)
    wAverageUserWrite = 0;
  else
    wAverageUserWrite=(long long)((float)UserWriteSize / (float)UserWrites);
  long long wAverageUserRead;
  if ((UserReads)==0)
    wAverageUserRead = 0;
  else
    wAverageUserRead=(long long)((float)UserReadSize / (float)UserReads);
  long long wAverageExtent;
  if ((ExtentWrites)==0)
    wAverageExtent = 0;
  else
    wAverageExtent=(long long)((float)ExtentSize / (float)ExtentWrites);

  long long wAverageFieldRead;
  if (FieldReads  ==0)
    wAverageFieldRead = 0;
  else
    wAverageFieldRead=(long long)((float)FieldReadSize / (float)FieldReads);

  long long wAverageFieldWrite;
  if (FieldWrites  ==0)
    wAverageFieldWrite = 0;
  else
    wAverageFieldWrite=(long long)((float)FieldWriteSize / (float)FieldWrites);

  fprintf (pOutput,
      "________________________________________________\n"
      " Header\n"
      "   File header reads          %10ld\n"
      "               writes         %10ld\n"
      "   File lock info read        %10ld\n"
      "                  write       %10ld\n"
      "   File descriptor reads      %10ld\n"
      "                   writes     %10ld\n"
      "   Reserved block reads       %10ld\n"
      "                  writes      %10ld\n"
      " Content\n"
      "   Block header reads         %10ld\n"
      "                size          %10lld\n"
      "                writes        %10ld\n"
      "                size          %10lld\n"
      "   User content reads         %10ld\n"
      "                size          %10lld\n"
      "             average/read     %10lld\n"

      "   User content writes        %10ld\n"
      "                size          %10lld\n"
      "             average/write    %10lld\n"

      "   Field content reads        %10ld\n"
      "                size          %10lld\n"
      "             average/read     %10lld\n"

      "   Field content writes       %10ld\n"
      "                size          %10lld\n"
      "             average/write    %10lld\n"

      "   Highwater marking writes   %10ld\n"

      "                     size     %10lld\n"

      "   Free pool matches          %10ld\n"
      "   Free pool mismatches :\n"
      "   Extent writes              %10ld\n"
      "          size                %10lld\n"
      "      extent average/write    %10lld\n"

      "________________________________________________\n",

      HFHReads,
      HFHWrites,
      LockReads,
      LockWrites,
      HFDReads,
      HFDWrites,
      HReservedReads,
      HReservedWrites,

      CBHReads,
      CBHReadBytesSize,
      CBHWrites,
      CBHWriteBytesSize,

      UserReads,
      UserReadSize,
      wAverageUserRead,

      UserWrites,
      UserWriteSize,
      wAverageUserWrite,

      FieldReads,
      FieldReadSize,
      wAverageFieldRead,

      FieldWrites,
      FieldWriteSize,
      wAverageFieldWrite,

      HighWaterWrites,
      //             pDescriptor.ZFCB->HighwaterMarking?"<On>":"<Off>",
      HighWaterBytesSize,

      FreeMatches,
      ExtentWrites,
      ExtentSize,
      wAverageExtent
      );

  return ;
}

//! @cond Development

void
ZRFPMS::PMSCounterRead(ZPMSCounter_type pC, const zsize_type pSize)
{
  switch (pC)
  {
  case ZPMS_BlockHeader :
  {
    CBHReads += 1;
    CBHReadBytesSize += pSize;
    return;
  }
  case ZPMS_User :
  {
    UserReads += 1;
    UserReadSize += pSize;
    return;
  }
  case ZPMS_Field :
  {
    FieldReads += 1;
    FieldReadSize += pSize;
    return;
  }
  default:
  {
    return;
  }

  }
} //PMSCounterRead
void
ZRFPMS::PMSCounterWrite(ZPMSCounter_type pC, const zsize_type pSize)
{
  switch (pC)
  {
  case ZPMS_BlockHeader :
  {
    CBHWrites += 1;
    CBHWriteBytesSize += pSize;
    return;
  }
  case ZPMS_User :
  {
    UserWrites += 1;
    UserWriteSize += pSize;
    return;
  }
  case ZPMS_Field :
  {
    FieldWrites += 1;
    FieldWriteSize += pSize;
    return;
  }
  default:
  {
    return;
  }
  }
} //PMSCounterWrite

//! @endcond

//----------End ZRFPMS---------------------------------
