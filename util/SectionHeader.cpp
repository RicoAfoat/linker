#include "SectionHeader.h"
#include <cassert>

void SectionHeader::initName(void* NameSectionAddr){
    auto NameOffset=getNameOffset();
    NamePtr=(char*)NameSectionAddr+NameOffset;
    std::printf("%s\n",NamePtr);
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
    assert(Size>=Shdr->getSize()&&"file does not contain enough data for SectionHeader");
    return Shdr;
}
