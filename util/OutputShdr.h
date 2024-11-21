#pragma once
#include "chunk.h"
struct OutputShdr:public Chunk{
    OutputShdr();
    void UpdateShdr();
    void CopyBuf()override;
};
