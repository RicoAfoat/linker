#pragma once
#include "DataLayoutAdapter.h"

class ELFHeader:public SystemStructAdapter{
public:
    bool check() override;
    ELFHeader(void* StartAddr,size_t Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList):SystemStructAdapter(StartAddr,Size,initList){};

    inline uint64_t getShoff(){
        return loadComponentAs<uint64_t>("e_shoff");
    }

    inline uint16_t getShnum(){
        return loadComponentAs<uint16_t>("e_shnum");
    }

    inline uint16_t getShstrndx() {
        return loadComponentAs<uint16_t>("e_shstrndx");
    }
};

extern template ELFHeader* getNewImpl(DataLayOutEnum ChoosedLayout,void* StartAddr,size_t Size);

