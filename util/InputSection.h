#pragma once
#include "ELF.h"
#include <utility>
#include <new>
#include <cstdint>
#include <string_view>

class ObjectFile;
class OutputSection;

// can be copied
struct InputSection{
    ObjectFile* File;
    std::pair<uint8_t*,size_t> Content;
    uint32_t Shndx;
    bool isAlive;
    uint8_t P2Align=0;

    uint32_t offset=UINT32_MAX;
    uint32_t RelsecIdx=UINT32_MAX;

    OutputSection* OutSec=nullptr;

    InputSection()=delete;
    InputSection(ObjectFile* f,uint32_t shndx);
    
    Shdr* getShdr();
    std::string_view getName();

    uint64_t getAddr();

    std::pair<Rela*,size_t> getRels();
    void ScanRelocations();

    void WriteTo(uint8_t* dst/**/);

    void ApplyRelocations(std::pair<uint8_t*,size_t>);
};



