#include "SectionHeader.h"
#include <cassert>

size_t SectionHeader::getSectionHeaderSize(){
    return size_t(static_cast<char*>(getComponent<void>("EndOfSectionHeader")) - static_cast<char*>(StartAddr));
}

SectionHeader* SectionHeader::getNewSectionHeader(DataLayOutEnum ChoosedLayout,void* StartAddr,size_t Size){
    SectionHeader* Shdr=nullptr;
    if(ChoosedLayout==BIT64){
        Shdr = new SectionHeader(
            StartAddr,
#define GET_SectionHeaderLayOut
#include "../conf/RISCV64.def"
        );
    }
    else
        assert(0&&"unimp");
    assert(Size>=Shdr->getSectionHeaderSize()&&"file does not contain enough data for SectionHeader");
    return Shdr;
}
