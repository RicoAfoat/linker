#pragma once
#include "DataLayoutAdapter.h"
#include <elf.h>

class SectionHeader;
class ELFSym:public SystemStructAdapter,
             public AttributeStringNameOffset<ELFSym,ConstTableEntry::st_name>{
public:
    ELFSym(void* _StartAddr):SystemStructAdapter(_StartAddr,ConstTableEntry::st_name,ConstTableEntry::EndOfSym){};

    inline uint16_t getShndx(){
        return loadComponentAs<uint16_t>(ConstTableEntry::st_shndx);
    }

    inline auto getSTInfo(){
        return loadComponentAs<uint8_t>(ConstTableEntry::st_info);
    }

    uint32_t getShndx(SectionHeader*,uint32_t);

    inline bool isAbsulute(){
        if(getShndx()==SHN_ABS)return true;
        return false;
    };

    inline bool isUndef(){
        if(getShndx()==SHN_UNDEF)return true;
        return false;
    }

    inline bool isCommon(){
        if(getShndx()==SHN_COMMON)return true;
        return false;
    }

    inline uint8_t getSymbolBinding(){
        return ELF64_ST_BIND(getSTInfo());
    }

    inline uint64_t getValue(){
        return loadComponentAs<uint64_t>(ConstTableEntry::st_value);
    }
};

extern template ELFSym* getNew(void*,uint32_t);