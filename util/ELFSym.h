#pragma once
#include "DataLayoutAdapter.h"
#include <elf.h>
class ELFSym:public SystemStructAdapter,
             public AttributeStringNameOffset<ELFSym,ConstTableEntry::st_name>{
public:
    ELFSym(void* _StartAddr):SystemStructAdapter(_StartAddr,ConstTableEntry::st_name,ConstTableEntry::EndOfSym){};

    uint16_t getShndx(){
        return loadComponentAs<uint16_t>(ConstTableEntry::st_shndx);
    }

    inline bool isAbsulute(){
        if(getShndx()==SHN_ABS)return true;
        return false;
    };

    inline bool isUndef(){
        if(getShndx()==SHN_UNDEF)return true;
        return false;
    }
};

extern template ELFSym* getNew(void*,uint32_t);