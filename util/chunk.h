#pragma once
#include <cstddef>
#include <cstdint>
#include "ELF.h"

struct Chunk {
    // std::string Name;
    std::string Name;
    Shdr SectionHeader;
    Chunk();
};


