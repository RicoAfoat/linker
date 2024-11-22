#include "OutputPhdr.h"
#include "Context.h"
#include "Singleton.h"

OutputPhdr::OutputPhdr(){
    this->SectionHeader.sh_flags=SHF_ALLOC;
    this->SectionHeader.sh_addralign=8;
}

uint32_t toPhdrFlags(Chunk* chunk){
    uint32_t ret=PF_R;
    if(chunk->getShdr()->sh_flags&SHF_WRITE)
        ret|=PF_W;
    if(chunk->getShdr()->sh_flags&SHF_EXECINSTR)
        ret|=PF_X;
    return ret;
}

Phdr definePhdr(uint32_t type,uint32_t flags,uint64_t minAlign,Chunk* chunk){
    auto phdr=Phdr();ZeroInit(&phdr,sizeof(phdr));
    phdr.p_type=type;
    phdr.p_flags=flags;
    phdr.p_align=std::max(minAlign,chunk->getShdr()->sh_addralign);
    phdr.p_offset=chunk->getShdr()->sh_offset;
    if(chunk->getShdr()->sh_type==SHT_NOBITS)
        phdr.p_filesz=0;
    else
        phdr.p_filesz=chunk->getShdr()->sh_size;
    phdr.p_vaddr=chunk->getShdr()->sh_addr;
    phdr.p_paddr=chunk->getShdr()->sh_addr;
    phdr.p_memsz=chunk->getShdr()->sh_size;
    return phdr;
}

void pushIntoPhdr(Phdr& phdr,Chunk* chunk){
    phdr.p_align=std::max(phdr.p_align,chunk->getShdr()->sh_addralign);
    if(chunk->getShdr()->sh_type!=SHT_NOBITS)
        phdr.p_filesz=chunk->getShdr()->sh_addr+chunk->getShdr()->sh_size-phdr.p_vaddr;
    phdr.p_memsz=chunk->getShdr()->sh_addr+chunk->getShdr()->sh_size-phdr.p_vaddr;
}

std::vector<Chunk*> collect(std::vector<Chunk*>& chunks,std::function<bool(Chunk*)> condition){
    auto ret=std::vector<Chunk*>();
    for(auto chunk:chunks)
        if(condition(chunk))
            ret.push_back(chunk);
    return ret;
}

std::vector<Phdr> OutputPhdr::createPhdrs(){
    auto& Ctx=Singleton<Context>();
    auto vec=std::vector<Phdr>();
    
    auto IsTls=[](Chunk* chunk) -> bool {
        return (chunk->getShdr()->sh_flags&SHF_TLS)!=0;
    };

    auto IsBss=[&IsTls](Chunk* chunk) -> bool {
        return
        (chunk->getShdr()->sh_type==SHT_NOBITS)&&
        !IsTls(chunk);
    };

    auto IsNote=[](Chunk* chunk) -> bool {
        auto shdr=chunk->getShdr();
        return 
        (shdr->sh_type==SHT_NOTE)&&
        ((shdr->sh_flags&SHF_ALLOC)!=0)
        ;
    };

    auto GroupChunksIntoPhdrs=[&Ctx,&vec](std::vector<Chunk*> chunks,uint32_t PhdrType,uint64_t ConstAlignment) -> void {
        if(chunks.size()==0)return;
        std::unordered_map<uint32_t,Phdr> PhdrMap;
        for(auto chunk:chunks){
            auto Key=toPhdrFlags(chunk);
            uint64_t Alignment=(ConstAlignment!=0)?ConstAlignment:chunk->getShdr()->sh_addralign;
            if(PhdrMap.find(Key)==PhdrMap.end())
                PhdrMap[Key]=definePhdr(PhdrType,Key,Alignment,chunk);
            else
                pushIntoPhdr(PhdrMap[Key],chunk);
        }
        for(auto& [_,phdr]:PhdrMap)
            vec.push_back(phdr);
    };

    // First phdr is for the program header itself
    vec.push_back(definePhdr(PT_PHDR,PF_R,8,&Ctx.OutPhdr));

    // Deal With Note Sections
    GroupChunksIntoPhdrs(collect(Ctx.Chunks,IsNote),PT_NOTE,0);

    // Deal With Loadable Sections
    GroupChunksIntoPhdrs(collect(Ctx.Chunks,[&IsBss](Chunk* chunk) -> bool {
        if(isTbss(chunk))return false;
        if((chunk->getShdr()->sh_flags&SHF_ALLOC)==0)return false;
        return IsBss(chunk);
    }),PT_LOAD,4096);
    GroupChunksIntoPhdrs(collect(Ctx.Chunks,[&IsBss](Chunk* chunk) -> bool {
        if(isTbss(chunk))return false;
        if((chunk->getShdr()->sh_flags&SHF_ALLOC)==0)return false;
        return !IsBss(chunk);
    }),PT_LOAD,4096);
    
    // Deal With TLS Sections
    GroupChunksIntoPhdrs(collect(Ctx.Chunks,IsTls),PT_TLS,1);

    Ctx.TpAddr=vec.back().p_vaddr;
    return vec;
}

void OutputPhdr::UpdateShdr(){
    Phdrs=createPhdrs();
    this->SectionHeader.sh_size=this->Phdrs.size()*sizeof(Phdr);
}

void OutputPhdr::CopyBuf(){
    auto& Ctx=Singleton<Context>();
    CopyInto((uint8_t*)Phdrs.data(),Phdrs.size()*sizeof(Phdr));
}
