#include "OutputSection.h"
#include "InputSection.h"
#include "Context.h"
#include "Singleton.h"
#include "Output.h"
#include <cstring>

OutputSection::OutputSection(std::string Name,uint32_t Type,uint64_t Flags){
    this->Name=Name;
    SectionHeader.sh_type=Type;
    SectionHeader.sh_flags=Flags;
}

void OutputSection::CopyBuf(){
    auto& Ctx=Singleton<Context>();
    if(SectionHeader.sh_type==SHT_NOBITS)
        return;
    
    auto base=Ctx.OutputBuf.data()+SectionHeader.sh_offset;
    for(auto& i:Member)
        i->WriteTo(base);
}

OutputSection* OutputSection::getOutputSection(std::string Name,uint32_t Type,uint64_t Flags){
    auto& Ctx=Singleton<Context>();
    Name=getOutputName(Name,Flags);
    Flags=Flags&(~SHF_GROUP)&(~SHF_COMPRESSED)&(~SHF_LINK_ORDER);

    for(auto& i:Ctx.OutSections){
        if(i->Name==Name&&i->SectionHeader.sh_type==Type&&i->SectionHeader.sh_flags==Flags)
            return i.get();
    }

    auto osec=new OutputSection(Name,Type,Flags);
    Ctx.OutSections.emplace_back(osec);
    return osec;
}
