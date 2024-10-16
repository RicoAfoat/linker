#pragma once
#include "./ObjectFile.h"
#include "./SectionHeader.h"

struct SymbolEntry{
    ObjectFile* Obj;
    SectionHeader* Shdr;
    ELFSym* ESym;
};

using LinkerSymbolTable=std::unordered_map<std::string,SymbolEntry>;