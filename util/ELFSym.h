#pragma once
#include "DataLayoutAdapter.h"
class ELFSym:public SystemStructAdapter,
             public AttributeStringNameOffset<ELFSym,ConstTableEntry::st_name>{
public:
    ELFSym(void* _StartAddr):SystemStructAdapter(_StartAddr,ConstTableEntry::st_name,ConstTableEntry::EndOfSym){};
};

extern template ELFSym* getNew(void*,uint32_t);