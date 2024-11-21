#include "chunk.h"

Chunk::Chunk() {
    ZeroInit(&SectionHeader,sizeof(Shdr));
    SectionHeader.sh_addralign=1;
}

Shdr* Chunk::getShdr() {
    return &SectionHeader;
}

void Chunk::CopyBuf() {}