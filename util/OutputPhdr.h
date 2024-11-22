#pragma once
#include "chunk.h"
#include <vector>

struct OutputPhdr:public Chunk{
    std::vector<Phdr> Phdrs;
    OutputPhdr();
    void UpdateShdr()override;
    std::vector<Phdr> createPhdrs();
    void CopyBuf();
};
 