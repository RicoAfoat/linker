#pragma once
#include "chunk.h"
struct OutputEhdr:public Chunk{
    OutputEhdr();
    void CopyBuf() override;
};
