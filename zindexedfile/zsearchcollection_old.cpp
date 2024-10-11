#include "zsearchcollection.h"

#include "zsearchformula.h"

using namespace  zbs;

ZSearchCollection::~ZSearchCollection()
{
/*  if (Entity!=nullptr)
    delete Entity;
*/
  while (AddressList.count())
    AddressList.pop();
  if (Formula!=nullptr)
    delete Formula;
}



ZSearchCollection&
ZSearchCollection::_copyFrom (const ZSearchCollection& pIn)
{
  Entity = pIn.Entity;
  AddressList.clear();
  for (int wi=0;wi < pIn.AddressList.count();wi++)
    AddressList.push (pIn.AddressList[wi]);

  if (Formula!=nullptr)
    delete Formula;

  if (pIn.Formula!=nullptr)
    Formula = new ZSearchFormula(*pIn.Formula);
    else
      Formula = nullptr;

  return *this;
}


size_t
ZSearchCollection::getRecordsToProcess()
{
  Entity->getMaxRecords();
}
