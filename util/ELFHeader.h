#pragma once
#include "DataLayoutAdapter.h"

class ELFHeader:public SystemStructAdapter{
public:
    bool check() override;
    ELFHeader(void* StartAddr):SystemStructAdapter(StartAddr,ConstTableEntry::e_ident,ConstTableEntry::EndOfELFHeader){};

    inline uint64_t getShoff(){
        return loadComponentAs<uint64_t>(ConstTableEntry::e_shoff);
    }

    inline uint16_t getShnum(){
        return loadComponentAs<uint16_t>(ConstTableEntry::e_shnum);
    }

    inline uint16_t getShstrndx() {
        return loadComponentAs<uint16_t>(ConstTableEntry::e_shstrndx);
    }
};

extern template ELFHeader* getNew(void*,uint32_t);
