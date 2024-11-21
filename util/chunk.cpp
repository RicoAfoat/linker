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