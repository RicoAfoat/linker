#include "chunk.h"
#include "Context.h"
#include "Singleton.h"
#include "Output.h"
#include "SectionFragment.h"
#include "Symbol.h"
#include "mergeablesection.h"
#include "InputSection.h"
#include <cstring>

MergedSection::MergedSection(std::string name,uint64_t flag,uint32_t type){
    this->Name=name;
    this->getShdr()->sh_flags=flag;
    this->getShdr()->sh_type=type;
}

MergedSection* MergedSection::getMergedSection(std::string_view name,uint64_t flag,uint32_t type){
    auto OName=getOutputName(std::string(name),flag);

    flag=flag&(~SHF_GROUP)&(~SHF_MERGE)&(~SHF_STRINGS)&(~SHF_COMPRESSED);

    auto& Ctx=Singleton<Context>();
    auto find=[&Ctx,&OName,&flag,&type]() ->MergedSection* {
        for(auto& ms:Ctx.mergedSections){
            if(ms->Name==OName&&flag==ms->getShdr()->sh_flags&&type==ms->getShdr()->sh_type)
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

void MergedSection::AssignOffsets(){
    std::vector<std::pair<std::string_view,SectionFragment*>> vec;
    for(auto& [name,ptr]:Map)
        vec.emplace_back(name,ptr.get());
    std::sort(vec.begin(),vec.end(),[](auto& a,auto& b){
        auto&& [namea,sa]=a;
        auto&& [nameb,sb]=b;
        if(sa->P2Align!=sb->P2Align)
            return sa->P2Align<sb->P2Align;
        auto&& sizea=namea.size();
        auto&& sizeb=nameb.size();
        if(sizea!=sizeb)
            return sizea<sizeb;
        return namea<nameb;
    });

    uint64_t offset=0;
    uint8_t align=0;
    for(auto& [_,sf]:vec){
        offset=AlignTo(offset,uint64_t(1)<<sf->P2Align);
        sf->Offset=offset;
        offset+=_.size();
        align=std::max(align,sf->P2Align);
    }
    getShdr()->sh_size=AlignTo(offset,uint64_t(1)<<align);
    getShdr()->sh_addralign=uint64_t(1)<<align;
}

void MergedSection::CopyBuf(){
    uint8_t* copylocate=Singleton<Context>().OutputBuf.data()+getShdr()->sh_offset;
    for(auto& [name,ptr]:Map){
        auto loc=copylocate+ptr->Offset;
        memcpy(loc,name.data(),name.size());
    }
}
