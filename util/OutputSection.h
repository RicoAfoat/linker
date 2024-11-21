#pragma once
#include "chunk.h"
#include <vector>
class InputSection;
struct OutputSection : public Chunk {
    std::vector<InputSection*> Member;
    OutputSection()=delete;
    OutputSection(std::string Name,uint32_t Type,uint64_t Flags);
    void CopyBuf() override;
    static OutputSection* getOutputSection(std::string Name,uint32_t Type,uint64_t Flags);
};
