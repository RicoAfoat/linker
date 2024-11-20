#include "Symbol.h"
#include "ObjectFile.h"
#include "InputSection.h"
#include "Context.h"
#include "Singleton.h"
#include <cassert>

Symbol::Symbol(std::string_view Name) : Name(Name) {}

Symbol* Symbol::getSymbolByName(std::string_view Name) {
    auto& ST=Singleton<Context>().SymbolMap;
    if(ST.find(Name)==ST.end())
        ST[Name]=std::make_unique<Symbol>(Name);
    return ST[Name].get();
}

Sym* Symbol::getElfSym() {
    assert(SymIdx<File->ElfSyms.size());
    return File->ElfSyms[SymIdx];
}

void Symbol::clear() {
    File=nullptr;
    InputSec=nullptr;
    SymIdx=-1;
}
