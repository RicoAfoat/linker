#pragma once
#include <string>
#include "ELF.h"
class ObjectFile;
class InputSection;
struct Symbol{ 
    ObjectFile* File=nullptr;
    InputSection* InputSec=nullptr;
    std::string_view Name;
    uint64_t Value;
    int SymIdx;
    Symbol()=delete;
    Symbol(std::string_view Name);
    static Symbol* getSymbolByName(std::string_view Name);
    Sym* getElfSym();
    void clear();
};
