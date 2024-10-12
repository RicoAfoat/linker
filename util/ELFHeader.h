#pragma once
#include "DataLayoutAdapter.h"

class ELFHeader:public SystemStructAdapter{
    bool check();
public:
    static ELFHeader* getNewELFHeader(DataLayOutEnum ChoosedLayout,void* StartAddr,size_t Size);

    size_t getELFHeaderSize();

    ELFHeader(void* StartAddr,size_t Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList):SystemStructAdapter(StartAddr,Size,initList){};

    // inline uint16_t getShnum(bool GetRaw=true){
    //     auto Shnum = loadComponentAs<uint16_t>("e_shnum");
    //     if(GetRaw||Shnum!=0)return Shnum;
    //     assert(0&&"unsupported yet");
    // };
};

