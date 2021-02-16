#include "zblock.h"

ZBlockHeader& ZBlockHeader::_copyFrom(const ZBlockHeader& pIn)
{
  BlockSize=pIn.BlockSize;
  State=pIn.State;
  Lock=pIn.Lock;
  Pid=pIn.Pid;
  return *this;
}
