#pragma once
#include "DataLayoutAdapter.h"

class ArHdr:public SystemStructAdapter{
public:
    ArHdr(void *StartAddr, size_t Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList) : SystemStructAdapter(StartAddr, Size, initList){};
};