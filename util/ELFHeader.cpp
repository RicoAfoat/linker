#include "ELFHeader.h"
#include <cassert>

size_t ELFHeader::getELFHeaderSize(){
    return size_t(static_cast<char*>(getComponent<void>("EndOfELFHeader")) - static_cast<char*>(StartAddr));
}

ELFHeader* ELFHeader::getNewELFHeader(DataLayOutEnum ChoosedLayout,void* StartAddr,size_t Size){
    ELFHeader* Ehdr=nullptr;
    if(ChoosedLayout==BIT64){
        Ehdr = new ELFHeader(
            StartAddr,
#define GET_ELFHeaderLayOut
#include "../conf/RISCV64.def"
        );
    }
    else{
        assert(0&&"unimpl");
    }
    assert(Size>=Ehdr->getELFHeaderSize()&&"file is smaller that ELFHeader");
    Ehdr->check();
    return Ehdr;
}

bool ELFHeader::check(){
    auto EhdrSize=getELFHeaderSize();
    if(EhdrSize>Size)return false;
    auto e_ident=getComponent<uint8_t>("e_ident");
    auto hasMagic=(
        e_ident[0]==0x7F&&
        e_ident[1]==0x45&&
        e_ident[2]==0x4C&&
        e_ident[3]==0x46
    );
    return hasMagic;
}