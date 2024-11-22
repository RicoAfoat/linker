#pragma once
#include "chunk.h"
struct OutputEhdr:public Chunk{
    OutputEhdr();
    void CopyBuf() override;
    static uint64_t getEntryAddr();
    static uint32_t getFlags();
};
