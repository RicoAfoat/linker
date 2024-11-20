#include "chunk.h"

Chunk::Chunk() {
    ZeroInit(&SectionHeader,sizeof(Shdr));
    SectionHeader.sh_addralign=1;
}