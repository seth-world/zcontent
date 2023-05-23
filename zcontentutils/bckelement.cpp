#include "bckelement.h"

BckElement::BckElement()
{

}
BckElement&
BckElement::_copyFrom(const BckElement& pIn) {
  Source = pIn.Source;
  Size = pIn.Size;
  Target = pIn.Target;
  Status = pIn.Status;
  Created = pIn.Created;
  LastModified = pIn.LastModified;
  return *this;
}
