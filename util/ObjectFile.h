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
        uint64_t Shnum = Ehdr->getShnum();
        if(Shnum!=0)return Shnum;
        else{
            return Shdrs[0]->getShSize();
        }
    }

    inline uint32_t getShstrndx(){
        uint32_t Shstrndx=Ehdr->getShstrndx();
        if(Shstrndx==UINT16_MAX){
            Shstrndx=Shdrs[0]->getShLink();
        }
        return Shstrndx;
    }

    inline void* getSectionAddr(uint32_t Idx){
        assert(Shdrs.size()>Idx);
        return FileStorage.data()+Shdrs[Idx]->getSectionOffset();
    }
};