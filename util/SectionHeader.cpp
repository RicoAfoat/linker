#include "SectionHeader.h"
#include <cassert>

template SectionHeader* getNew<SectionHeader>(DataLayOutEnum, void*, size_t);

void SectionHeader::initName(void* NameSectionAddr){
    auto NameOffset=getNameOffset();
    NamePtr=(char*)NameSectionAddr+NameOffset;
}
