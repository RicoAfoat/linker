#include "ObjectFile.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <iomanip>

ELFHeader& ObjectFile::getEhdr(){return *Ehdr;}

void ObjectFile::initFileStructure(){
    const auto [BufferAddr,Limi]=getFileBuffer();
    Ehdr.reset(getNew<ELFHeader>(BufferAddr, Limi));
    
    // Get All Section Headers
    {
        auto Shoff=Ehdr->getShoff();
        uint8_t* ptr=BufferAddr+Shoff;
        auto restSize=Limi-Shoff;
        Shdrs.emplace_back(getNew<SectionHeader>(ptr,restSize));
        
        auto ShdrSize=Shdrs[0]->getSize();

        auto NumSections=getNumSection();
        for(auto I=NumSections;I>1;I--){
            ptr+=ShdrSize;
            restSize-=ShdrSize;
            auto Shdr=getNew<SectionHeader>(ptr,restSize);
            if(Shdr->getShType()==SHT_SYMTAB_SHNDX)
                SymtabShndx=Shdr;
            Shdrs.emplace_back();
        }

        // init all sections Name
        assert(Shdrs[getShstrndx()]->getShType()==0x3);
        auto NameSectionAddr=getSectionAddr(getShstrndx());
        for(auto I=0;I<NumSections;I++)
            Shdrs[I]->initAttributeStringNameOffset(NameSectionAddr);
    }

    {
        // Get Symbol Table initialized
        auto Syms=getShdrs([](SectionHeader* Shdr){
            return Shdr->getShType() == SHT_SYMTAB;
        });
        assert(Syms.size()==1&&"SYMTAB will be only one in an ELF");
        
        // find the first global
        fisrtGlobalIndex=Syms[0]->getShInfo();
        
        auto& SymbolTable=getSymbolTable();

        auto SymAddr=getSectionAddr(Syms[0]);
        auto SymSectionSize=Syms[0]->getShSize();
        
        // Init Every Symbol Table Entry
        // First One
        auto SymEntry=getNew<ELFSym>(SymAddr,SymSectionSize);
        SymbolTable.emplace_back(SymEntry);
        auto SymEntrySize=SymEntry->getSize();

        // Init rest of the Symbol Table
        auto SymEleSize=SymSectionSize/SymEntrySize;
        for(;SymEleSize>1;SymEleSize--){
            SymAddr=(uint8_t*)(SymAddr)+SymEntrySize;
            SymSectionSize-=SymEntrySize;
            SymbolTable.emplace_back(getNew<ELFSym>(SymAddr,SymSectionSize));
        }

        {
            // Get .symtab corresponding .strtab 
            // std::cerr<<Syms[0]->getShLink()<<std::endl;
            auto& StrTab=getShdrs()[Syms[0]->getShLink()];
            assert(StrTab->getShType()==SHT_STRTAB);
            
            // init all sections Name
            auto NameSectionAddr=getSectionAddr(StrTab.get());
            auto NumSyms=SymbolTable.size();
            for(auto I=0;I<NumSyms;I++)
                SymbolTable[I]->initAttributeStringNameOffset(NameSectionAddr);
        }
    }
}

std::vector<SectionHeader*> ObjectFile::getShdrs(std::function<bool(SectionHeader*)> Condition){
    std::vector<SectionHeader*> Result;
    auto numShdrs=getNumSection();
    for(auto I=0;I<numShdrs;I++){
        auto ptr=Shdrs[I].get();
        if(Condition(ptr))
            Result.push_back(ptr);
    }
    return Result;
}
