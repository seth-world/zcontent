#include "zblockpool.h"

ZBlockPool::ZBlockPool():ZArray<ZBlockDescriptor>()
{

}

//================================= ZBlockPool =====================

/**
 * @brief ZBlockPool::_exportPool DO NOT USE ZArray Standard import/export facilities for Block pools
 * @param pZDBExport
 * @return
 */
size_t
ZBlockPool::_exportAppendPool(ZDataBuffer&pZDBExport)
{

#ifdef __ZTHREAD_AUTOMATIC__
  _Base::lock();
#endif
  ZAExport ZAE;

  ZAE.AllocatedSize = _Base::getAllocatedSize();   // get the total allocated size
  ZAE.CurrentSize = _Base::getCurrentSize();     // get the current size corresponding to currently stored elements

  ZAE.AllocatedElements = _Base::getAllocation();
  ZAE.InitialAllocation = _Base::getInitalAllocation();
  ZAE.ExtentQuota = _Base::getQuota();
  ZAE.NbElements = count();

  ZAE.DataSize = ZAE.NbElements * sizeof(ZBlockDescriptor_Export);

  ZAE.FullSize = getPoolExportSize();


#ifdef __REPORT_POOLS__
  fprintf (stdout,
      " -----Pool export report :\n"
      "    Allocated size      %ld\n"
      "    Current size        %ld\n"
      "    Data export size    %ld\n"
      "    Full export size    %ld\n"
      "    Allocated elemts    %ld\n"
      "    Extent quota        %ld\n"
      "    Initial allocation  %ld\n"
      "    Nb elements         %ld\n"
      "    Data offset         %ld\n",
      ZAE.AllocatedSize,
      ZAE.CurrentSize,
      ZAE.DataSize,
      ZAE.FullSize,
      ZAE.AllocatedElements,
      ZAE.ExtentQuota,
      ZAE.InitialAllocation,
      ZAE.NbElements,
      ZAE.DataOffset);
#endif // __REPORT_POOLS__

  //    _Base::convertZAExport_I(ZAE);


  unsigned char* wPtr=pZDBExport.extendBZero(ZAE.FullSize);
  ZAE.serialize();
  memmove(wPtr,&ZAE,sizeof(ZAExport));

  wPtr += sizeof(ZAExport);

  ZBlockDescriptor_Export* wBlkExp =(ZBlockDescriptor_Export*) wPtr;
  for (long wi=0;wi<_Base::size();wi++)
  {
    _Base::Tab(wi)._exportConvert(wBlkExp,_Base::Tab(wi));
    wBlkExp++;
  }

#ifdef __ZTHREAD_AUTOMATIC__
  _Base::unlock();
#endif
  return ZAE.FullSize ;
}// _exportPool

size_t ZBlockPool::getPoolExportSize()
{
  return sizeof(ZAExport) + (sizeof(ZBlockDescriptor_Export) * count());
}


/**
 * @brief ZBlockPool::_importPool  DO NOT USE ZArray Standard import/export facilities for Block pools
 * @param pBuffer
 * @return
 */
size_t
ZBlockPool::_importPool(const unsigned char *& pPtrIn)
{

  ZAExport wZAE;
  wZAE.setFromPtr(pPtrIn); /* pPtrIn is updated to point after ZAExport */
  wZAE.deserialize();

//    return _ZAimport((ZArray<ZBlockDescriptor>*)this,pBuffer,&ZBlockDescriptor::_importConvert);

#ifdef __ZTHREAD_AUTOMATIC__
  _Base::lock();
#endif

//ZAExport ZAE = _Base::ZAEimport(pBuffer);

#ifdef __REPORT_POOLS__
  fprintf (stdout,
      " +++++Pool import report :\n"
      "    Allocated size      %ld\n"
      "    Current size        %ld\n"
      "    Data export size    %ld\n"
      "    Full export size    %ld\n"
      "    Allocated elemts    %ld\n"
      "    Extent quota        %ld\n"
      "    Initial allocation  %ld\n"
      "    Nb elements         %ld\n"
      "    Data offset         %ld\n",
      wZAE.AllocatedSize,
      wZAE.CurrentSize,
      wZAE.DataSize,
      wZAE.FullSize,
      wZAE.AllocatedElements,
      wZAE.ExtentQuota,
      wZAE.InitialAllocation,
      wZAE.NbElements,
      wZAE.DataOffset);
#endif// __REPORT_POOLS__

  _Base::setInitialAllocation(wZAE.InitialAllocation,false);  // no lock
  // set reallocation quota
  if (wZAE.ExtentQuota > 0)
    _Base::setQuota(wZAE.ExtentQuota);
  else
    _Base::setQuota(cst_ZRF_default_extentquota);

  if (wZAE.NbElements > (ssize_t)wZAE.AllocatedElements) {
    size_t wAlloc = ( (wZAE.NbElements/ZReallocQuota)+1 ) * ZReallocQuota ;
    _Base::setAllocation(wAlloc,false);           // no lock : allocate space as number of elements
  }
  else
    _Base::setAllocation(wZAE.AllocatedElements,false);     // no lock : allocate space as allocated elements parameter
//  _Base::bzero(0,-1,false);                                   // no lock : set to zero
  long wZCurrentNb=0;
  ZBlockDescriptor wBDout;
  if (wZAE.NbElements>0)
  {
//    _Base::newBlankElement(wZAE.NbElements,false); // no use of pClear : can memset ZBlockDescriptor
    ZBlockDescriptor_Export* wEltPtr_In=(ZBlockDescriptor_Export*)(pPtrIn);
    for (wZCurrentNb=0 ; wZCurrentNb < wZAE.NbElements ; wZCurrentNb++) {
      ZBlockDescriptor::_importConvert(wBDout,&wEltPtr_In[wZCurrentNb]);
      push(wBDout);
    }// for
  } //(ZAE.NbElements>0)
#ifdef __ZTHREAD_AUTOMATIC__
  _Base::unlock();
#endif
  pPtrIn += wZAE.DataSize;
  return wZAE.FullSize;

}// _importPool

