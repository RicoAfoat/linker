#pragma once
#include "DataLayoutAdapter.h"

class SectionHeader:public SystemStructAdapter{
public:
    SectionHeader(void* StartAddr,size_t Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList):SystemStructAdapter(StartAddr,Size,initList){};
    
    size_t getSectionHeaderSize();
    static SectionHeader* getNewSectionHeader(DataLayOutEnum ChoosedLayout,void* StartAddr,size_t Size);
};