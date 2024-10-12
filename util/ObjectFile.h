#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "ELFHeader.h"
#include "SectionHeader.h"
class ObjectFile{
    std::vector<uint8_t> FileStorage;
    std::unique_ptr<ELFHeader> Ehdr;

    std::vector<std::unique_ptr<SectionHeader>> Shdrs;
public:
    static ObjectFile* OpenWith(char*);
    ObjectFile()=default;
    std::vector<uint8_t>& getFileStorage();
    
    ELFHeader& getEhdr();

    inline uint64_t getNumSection(){
        uint64_t Shnum = Ehdr->loadComponentAs<uint16_t>("e_shnum");
        if(Shnum!=0)return Shnum;
        else{
            return Shdrs[0]->loadComponentAs<uint64_t>("sh_size");
        }
    }   
};