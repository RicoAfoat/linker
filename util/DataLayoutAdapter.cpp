#include "DataLayoutAdapter.h"

bool SystemStructAdapter::check(){return true;}

SystemStructAdapter::SystemStructAdapter(void* _StartAddr,size_t _Size,std::initializer_list<std::tuple<std::string, size_t,uint8_t>> initList){
    StartAddr=_StartAddr;
    Size=_Size;
    for(auto& [Name,Offset,EleSize]:initList){
        ComponentMap[Name]=std::make_tuple((uint8_t*)StartAddr+Offset,EleSize);
    }
}

std::string SystemStructAdapter::loadRAW(std::string Name){
    auto& [Addr,EleSize]=ComponentMap[Name];
    std::string Ret((char*)Addr,EleSize);
    return Ret;
}