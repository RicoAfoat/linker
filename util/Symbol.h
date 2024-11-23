#pragma once
#include <string>
#include "ELF.h"
class ObjectFile;
class InputSection;
class SectionFragment;

enum CustomSymbolFlags : uint32_t {
    NeedsGotTp=1<<0,
};

struct Symbol{ 
    ObjectFile* File=nullptr;
    InputSection* InputSec=nullptr;
    SectionFragment* SecFrag=nullptr;
    std::string_view Name;
    uint64_t Value;
    int SymIdx=-1;
    int GotTpIdx=0;
    uint32_t Flags=0;
    Symbol()=delete;
    Symbol(std::string_view Name);
    static Symbol* getSymbolByName(std::string_view Name);
    void setInputSection(InputSection*);
    void setSectionFragment(SectionFragment*);
    Sym* getElfSym();
    void clear();
    uint64_t getAddr();
    uint64_t getGotTpAddr();
};
