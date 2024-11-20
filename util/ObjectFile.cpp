#include <fstream>
#include <iostream>
#include <cassert>
#include <iomanip>

#include "ObjectFile.h"
#include "InputSection.h"
#include "Symbol.h"

void ObjectFile::initFileStructure(){
    InputFile::initFileStructure();
    this->SymtabSec=this->findSection(SHT_SYMTAB);
    if(this->SymtabSec!=nullptr){
        this->FirstGlobal=this->SymtabSec->sh_info;
        this->fillupElfSyms(this->SymtabSec);
        this->SymbolStrtab=this->getBytesFromIdx(this->SymtabSec->sh_link);
    }
    initSections();
    initSymbols();
}

void ObjectFile::initSections(){
    Sections=std::vector<std::unique_ptr<InputSection>>(ElfSections.size());
    for(int i=0,limi=ElfSections.size();i<ElfSections.size();i++){
        auto elfsec=ElfSections[i];
        switch (elfsec->sh_type)
        {
        case SHT_GROUP:
        case SHT_SYMTAB:
        case SHT_STRTAB:
        case SHT_REL:
        case SHT_RELA:
        case SHT_NULL:
            break;
        case SHT_SYMTAB_SHNDX:
            fillupSymtabShndxSec(elfsec);
            break;
        default:
            Sections[i]=std::make_unique<InputSection>(this,i);
            break;
        }
    }
}

void ObjectFile::fillupSymtabShndxSec(Shdr* s){
    auto [Content,Size]=this->getBytesFromIdx(s->sh_link);
    auto nums=Size/4;
    this->SymtabShndxSec=std::pair<uint32_t*,size_t>((uint32_t*)Content,nums);
}

void ObjectFile::initSymbols(){
    if(this->SymtabSec==nullptr)
        return;
    
    for(int i=0;i<FirstGlobal;i++)
        LocalSymbols.push_back(Symbol(""));
    LocalSymbols[0].File=this;

    for(int i=0;i<FirstGlobal;i++){
        auto esym=ElfSyms[i];
        auto& sym=LocalSymbols[i];
        sym.Name=ElfGetName(SymbolStrtab,esym->st_name);
        sym.File=this;
        sym.Value=esym->st_value;
        sym.SymIdx=i;

        if(!IsAbs(esym))
            sym.InputSec=Sections[getShndx(esym,i)].get();
    }

    for(int i=0;i<FirstGlobal;i++)
        Symbols.push_back(&LocalSymbols[i]);

    for(int i=FirstGlobal;i<ElfSyms.size();i++){
        auto esym=ElfSyms[i];
        auto name=ElfGetName(SymbolStrtab,esym->st_name);
        Symbols.push_back(Symbol::getSymbolByName(name));
    }
}

uint64_t ObjectFile::getShndx(Sym* esym,int idx){
    assert(idx>=0&&idx<ElfSyms.size());
    if(esym->st_shndx==SHN_XINDEX){
        assert(idx<SymtabShndxSec.second);
        return SymtabShndxSec.first[idx];
    }
    return esym->st_shndx;
}

void ObjectFile::resolveSymbols(){
    for(auto i=FirstGlobal;i<ElfSyms.size();i++){
        auto esym=ElfSyms[i];
        auto sym=Symbols[i];
        if(IsUndef(esym))
            continue;
        
        InputSection* isec=nullptr;
        if(!IsAbs(esym)){
            isec=getSection(esym,i);
            if(isec==nullptr)
                continue;
        }

        auto shndx=getShndx(esym,i);
        sym->InputSec=Sections[shndx].get();
    }
}

InputSection* ObjectFile::getSection(Sym* esym,int idx){
    return Sections[getShndx(esym,idx)].get();
}

void ObjectFile::markLiveObjects(std::function<void(ObjectFile*)> f){
    assert(isAlive);
    for(int i=FirstGlobal;i<ElfSyms.size();i++){
        auto esym=ElfSyms[i];
        auto sym=Symbols[i];
        
        if(sym->File==nullptr)
            continue;

        if(IsUndef(esym)&&!sym->File->isAlive){
            sym->File->isAlive=true;
            f(sym->File);
        }
    }
}

void ObjectFile::clearSymbols(){
    for(int i=FirstGlobal;i<ElfSyms.size();i++){
        auto sym=Symbols[i];
        if(sym->File==this)
            sym->clear();
    }
}