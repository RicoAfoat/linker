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

    // First phdr is for the program header itself
    vec.push_back(definePhdr(PT_PHDR,PF_R,8,&Ctx.OutPhdr));

    // Deal With Note Sections
    {
        auto chunks=Ctx.Chunks;
        auto end=chunks.size();
        for(decltype(end) i=0;i<end;){
            auto first=chunks[i];
            i++;
            if(!IsNote(first))continue;
            auto flags=toPhdrFlags(first);
            auto align=first->getShdr()->sh_addralign;
            vec.push_back(definePhdr(PT_NOTE,flags,align,first));
            auto& phdr=vec.back();
            for(;i<end&&IsNote(chunks[i])&&toPhdrFlags(chunks[i])==flags;i++){
                pushIntoPhdr(phdr,chunks[i]);
            }
        }
    };


    // Deal With Loadable Sections
    {
        auto chunks=std::vector<Chunk*>();
        for(auto chunk:Ctx.Chunks){
            if(!isTbss(chunk))
                chunks.push_back(chunk);
        }

        auto end=chunks.size();
        for(decltype(end) i=0;i<end;){
            auto first=chunks[i];
            i++;
            
            if((first->getShdr()->sh_flags&SHF_ALLOC)==0)
                break;

            auto flags=toPhdrFlags(first);
            auto align=4096;
            vec.push_back(definePhdr(PT_LOAD,flags,align,first));
            auto& phdr=vec.back();
            
            if(!IsBss(first)){
                for(;i<end&&!IsBss(chunks[i])&&toPhdrFlags(chunks[i])==flags;i++){
                    pushIntoPhdr(phdr,chunks[i]);
                }
            }
            for(;i<end&&IsBss(chunks[i])&&toPhdrFlags(chunks[i])==flags;i++){
                pushIntoPhdr(phdr,chunks[i]);
            }
        }
    };
    
    // Deal With TLS Sections
    {
        auto chunks=Ctx.Chunks;
        auto end=chunks.size();

        for(decltype(end) i=0;i<end;i++){
            if(!IsTls(chunks[i]))
                continue;
            
            auto first=chunks[i];
            i++;
            auto flags=toPhdrFlags(first);
            auto align=1;
            vec.push_back(definePhdr(PT_TLS,flags,align,first));
            auto& phdr=vec.back();
            for(;i<end&&IsTls(chunks[i]);i++){
                pushIntoPhdr(phdr,chunks[i]);
            }
        }
    }

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
