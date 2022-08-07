#ifndef ZRECORDACCESSTABLE_H
#define ZRECORDACCESSTABLE_H
#include <ztoolset/zarray.h>
#include <zrandomfile/zrandomfiletypes.h>
#include <ztoolset/zbasedatatypes.h>
#include <zrandomfile/zblock.h>

namespace zbs {

class ZBlock : public ZBlockHeader
{
public:
    ZDataBuffer           Content;    //! user data content
    ZBlock (void) {clear();}
    void clear(void) {memset(this,0,sizeof(ZBlock));}

};

} // namespace zbs
#endif // ZRECORDACCESSTABLE_H
