#include "Context.h"
#include "Singleton.h"
#include <cstring>

Chunk::Chunk() {
    ZeroInit(&SectionHeader,sizeof(Shdr));
    SectionHeader.sh_addralign=1;
}

Shdr* Chunk::getShdr() {
    return &SectionHeader;
}

void Chunk::CopyBuf() {}

void Chunk::UpdateShdr(){}

std::string Chunk::getName(){
    return Name;
}

uint64_t Chunk::getShndx(){
    return Shndx;
}

void Chunk::CopyInto(uint8_t* buf, size_t size) {
    auto& Ctx=Singleton<Context>();
    auto copylocate=Ctx.OutputBuf.data()+SectionHeader.sh_offset;
    memcpy(copylocate,buf,size);
}

uint32_t Chunk::rank() {
    auto& Ctx=Singleton<Context>();
    auto tp=getShdr()->sh_type;
    auto flag=getShdr()->sh_flags;
    if((flag&SHF_ALLOC)==0){
        return UINT32_MAX-1;
    }
    if(this==&Ctx.OutShdr){
        return UINT32_MAX;
    }
    if(this==&Ctx.OutEhdr){
        return 0;
    }
    if(tp==SHT_NOTE){
        return 2;
    }
    uint32_t 
        writeable=(flag&SHF_WRITE)!=0,
        notExec=(flag&SHF_EXECINSTR)==0,
        notTls=(flag&SHF_TLS)==0,
        isBss=(tp==SHT_NOBITS);
    
    return uint32_t(
        (writeable<<7)|
        (notExec<<6)|
        (notTls<<5)|
        (isBss<<4)
    );
}
