#pragma once
#include "FileBuffer.h"
#include "ELF.h"
#include <vector>
class Symbol;
struct InputFile:public FileBuffer{
    std::vector<Sym*> ElfSyms;
    std::vector<Shdr*> ElfSections;
    std::vector<Symbol*> Symbols;
    std::vector<Symbol> LocalSymbols;

    std::pair<uint8_t*,size_t> Shstrtab;
    std::pair<uint8_t*,size_t> SymbolStrtab;
    
    int FirstGlobal;
    bool isAlive;

    void initFileStructure() override;
    std::pair<uint8_t*,size_t> getBytesFromShdr(Shdr* Shdr);
    std::pair<uint8_t*,size_t> getBytesFromIdx(int64_t Idx);

    void fillupElfSyms(Shdr* s);
    Shdr* findSection(uint32_t type);

    Ehdr* getEhdr();
};
