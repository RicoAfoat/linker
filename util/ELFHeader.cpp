#include "ELFHeader.h"
#include <cassert>

template ELFHeader* getNew(void*,uint32_t);

bool ELFHeader::check(){
    auto EhdrSize=getSize();
    if(EhdrSize>Size)return false;
    auto e_ident=getComponent<uint8_t>(ConstTableEntry::e_ident);
    auto hasMagic=(
        e_ident[0]==0x7F&&
        e_ident[1]==0x45&&
        e_ident[2]==0x4C&&
        e_ident[3]==0x46
    );
    return hasMagic;
}