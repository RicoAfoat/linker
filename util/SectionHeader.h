#pragma once
#define GET_SectionHeaderLayOut
#include "DataLayoutAdapter.h"

class SectionHeader:public SystemStructAdapter{
    char* NamePtr;
public:
    void initName(void* NameSectionAddr);

    SectionHeader(void* StartAddr,size_t Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList):SystemStructAdapter(StartAddr,Size,initList){};
    
    inline uint32_t getNameOffset(){
        return loadComponentAs<uint32_t>("sh_name");
    }

    inline uint32_t getShLink(){
        return loadComponentAs<uint32_t>("sh_link");
    }
    
    inline uint64_t getShSize(){
        return loadComponentAs<uint64_t>("sh_size");
    }

    inline uint32_t getShType(){
        return loadComponentAs<uint32_t>("sh_type");
    }

    inline uint64_t getSectionOffset(){
        return loadComponentAs<uint64_t>("sh_offset");
    }
};