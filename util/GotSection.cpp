#include "GotSection.h"
#include "Context.h"
#include "Singleton.h"
#include <cstring>

GotSection::GotSection(){
    this->Name=".got";
    this->SectionHeader.sh_type=SHT_PROGBITS;
    this->SectionHeader.sh_flags=SHF_ALLOC|SHF_WRITE;
    this->SectionHeader.sh_addralign=0;
}

void GotSection::addGotTpSym(Symbol* sym){
    sym->GotTpIdx=int32_t(getShdr()->sh_addr/8);
    this->getShdr()->sh_size+=8;
    this->GotTpSyms.push_back(sym);
}

std::vector<GotEntry> GotSection::getEntries(){
    auto ret=std::vector<GotEntry>();
    for(auto sym:GotTpSyms){
        ret.push_back(GotEntry{
            sym->GotTpIdx,
            /* symbol's address - tpaddr */
            sym->getAddr()-Singleton<Context>().TpAddr
        });
    }
    return ret;
}

void GotSection::CopyBuf(){
    auto& Ctx=Singleton<Context>();
    auto copylocate=Ctx.OutputBuf.data()+getShdr()->sh_offset;
    for(auto ent:getEntries()){
        memcpy(copylocate,&ent.Val,8);
        copylocate+=8;
    }
}
