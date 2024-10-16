#include "ELFSym.h"
#include "SectionHeader.h"

template ELFSym* getNew(void*,uint32_t);

uint32_t ELFSym::getShndx(SectionHeader* SymtabShndx,uint32_t Index){
    auto Shndx=getShndx();
    if(getShndx()==SHN_XINDEX){
        auto SymtabShndxAddr=(uint32_t*)SymtabShndx->getStartAddr();
        return SymtabShndxAddr[Index];
    }
    return Shndx;
}
