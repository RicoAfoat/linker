#pragma once
#include "ELF.h"
#include <utility>
#include <new>
#include <cstdint>
#include <string_view>

class ObjectFile;

// can be copied
struct InputSection{
    ObjectFile* File;
    std::pair<uint8_t*,size_t> Content;
    uint32_t Shndx;
    InputSection()=delete;
    InputSection(ObjectFile* f,uint32_t shndx);
    
    Shdr* getShdr();
    std::string_view getName();
};



