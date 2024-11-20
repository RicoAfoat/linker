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
    uint32_t ShSize=0;
    bool isAlive;
    uint8_t P2Align=0;

    InputSection()=delete;
    InputSection(ObjectFile* f,uint32_t shndx);
    
    Shdr* getShdr();
    std::string_view getName();
};



