#pragma once
#include "DataLayoutAdapter.h"

static constexpr const char ST_NAME[] = "st_name";
class ELFSym:public SystemStructAdapter,
             public AttributeGetNameOffset<ELFSym,ST_NAME>,
             public AttributeStringNameOffset<ELFSym>{
public:
    ELFSym(void* _StartAddr, size_t _Size,std::initializer_list<std::tuple<std::string, size_t, uint8_t>> initList):SystemStructAdapter(_StartAddr,_Size,initList){};
};
