#pragma once
#include <string>
#include "ELF.h"
class ObjectFile;
class InputSection;
class SectionFragment;
struct Symbol{ 
    ObjectFile* File=nullptr;
    InputSection* InputSec=nullptr;
    SectionFragment* SecFrag=nullptr;
    std::string_view Name;
    uint64_t Value;
    int SymIdx=-1;
    Symbol()=delete;
    Symbol(std::string_view Name);
    static Symbol* getSymbolByName(std::string_view Name);
    void setInputSection(InputSection*);
    void setSectionFragment(SectionFragment*);
    Sym* getElfSym();
    void clear();
};
