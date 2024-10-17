#pragma once
#include "ELFHeader.h"
#include "SectionHeader.h"
#include "ELFSym.h"
#include "FileBuffer.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include <queue>

class Context;

class ArchiveFile;
class ObjectFile:public FileBuffer<ObjectFile> {
    ArchiveFile* Archive=nullptr;
    
    std::unique_ptr<ELFHeader> Ehdr;
    SectionHeader* SymtabShndx=nullptr;
    std::vector<std::unique_ptr<SectionHeader>> Shdrs;
    std::vector<std::unique_ptr<ELFSym>> SymbolTable;

    uint32_t fisrtGlobalIndex;

    bool isAlive=false;
public:
    inline bool& getLiveness(){return isAlive;}
    void initFileStructure() override;

    ObjectFile()=default;
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
        if(Shstrndx==SHN_XINDEX)
            Shstrndx=Shdrs[0]->getShLink();
        return Shstrndx;
    }

    inline void* getSectionAddr(uint32_t Idx){
        assert(Shdrs.size()>Idx);
        return FileRef.first+Shdrs[Idx]->getSectionOffset();
    }

    inline void* getSectionAddr(SectionHeader* Shdr){
        return FileRef.first+Shdr->getSectionOffset();
    }

    inline std::vector<std::unique_ptr<SectionHeader>>& getShdrs(){
        return Shdrs;
    }

    std::vector<SectionHeader*> getShdrs(std::function<bool(SectionHeader*)>);

    inline std::vector<std::unique_ptr<ELFSym>>& getSymbolTable(){return SymbolTable;}

    inline void setArchiveFile(ArchiveFile* _Ar){Archive=_Ar;}
    inline ArchiveFile* getArchiveFile(){return Archive;}

    void resolveSymbolsInExtractFiles(Context&);

    void registerDefinedSymbols(Context&);

    void resolveSymbols(Context&);
};