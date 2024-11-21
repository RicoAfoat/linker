#pragma once
#include <cstddef>
#include <cstdint>
#include "ELF.h"

struct Chunk {
private:
    Shdr SectionHeader;
public:
    std::string Name;
    Chunk();
    virtual Shdr* getShdr();
    virtual void CopyBuf();
    virtual ~Chunk()=default;
};


