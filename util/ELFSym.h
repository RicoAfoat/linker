#pragma once
#include "DataLayoutAdapter.h"

class ELFSym:public SystemStructAdapter{
public:
    ELFSym(void* _StartAddr, size_t _Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList):SystemStructAdapter(_StartAddr,_Size,initList){};
};
