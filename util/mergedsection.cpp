#include "chunk.h"
#include "Context.h"
#include "Singleton.h"
#include "Output.h"
#include "SectionFragment.h"
#include "Symbol.h"
#include "mergeablesection.h"
#include "InputSection.h"

MergedSection::MergedSection(std::string name,uint64_t flag,uint32_t type){
    this->Name=name;
    this->SectionHeader.sh_flags=flag;
    this->SectionHeader.sh_type=type;
}

MergedSection* MergedSection::getMergedSection(std::string_view name,uint64_t flag,uint32_t type){
    auto OName=getOutputName(std::string(name),flag);

    flag=flag&(~SHF_GROUP)&(~SHF_MERGE)&(~SHF_STRINGS)&(~SHF_COMPRESSED);

    auto& Ctx=Singleton<Context>();
    auto find=[&Ctx,&OName,&flag,&type]() ->MergedSection* {
        for(auto& ms:Ctx.mergedSections){
            if(ms->Name==OName&&flag==ms->SectionHeader.sh_flags&&type==ms->SectionHeader.sh_type)
                return ms.get();
        }
        return nullptr;
    };

    if(auto ms=find())
        return ms;
    
    auto ms=new MergedSection(OName,flag,type);
    Ctx.mergedSections.emplace_back(ms);
    return ms;
}

SectionFragment* MergedSection::Insert(std::string_view Content,uint8_t P2Align){
    auto& Storage=Map[Content];
    
    if(Storage==nullptr){
        auto sf=new SectionFragment(this);
        Storage.reset(sf);
    }

    if(P2Align>Storage->P2Align)
        Storage->P2Align=P2Align;

    return Storage.get();
}
