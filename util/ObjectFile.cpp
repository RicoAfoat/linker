#include "ObjectFile.h"
#include "Context.h"
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
            Shdrs.emplace_back(Shdr);
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

void ObjectFile::resolveSymbolsInExtractFiles(Context& Ctx){
    std::cerr<<"---Extract Symbols In "<<getFileName()<<"---"<<std::endl;
    auto& ESyms=getSymbolTable();
    for(uint32_t I=fisrtGlobalIndex,limi=ESyms.size();I<limi;I++){
        auto& ESym=ESyms[I];
        auto Name=ESym->getName();
        auto Shdr=Shdrs[ESym->getShndx(SymtabShndx,I)].get();
        if(ESym->isUndef()){
            std::cerr<<"find undef symbol:\t"<<Name<<std::endl;
            continue;
        }

        if(ESym->getSymbolBinding()==STB_WEAK){
            std::cerr<<"find weak symbol:\t"<<Name<<std::endl;
            continue;
        }

        std::cerr<<"Register Global Symbol:\t"<<Name<<std::endl;
        auto& SymTable=Ctx.ArchiveSymbolTable;
        if(SymTable.find(Name)!=SymTable.end())
            // The linker will use the first symbol defined in those extract from archive files
            // demonstrate by experiments 
            continue;
        auto& Entry=SymTable[Name];
        Entry.ESym=ESym.get();
        Entry.Obj=this;
        Entry.Shdr=Shdr;
    }
}

void ObjectFile::resolveSymbols(Context& Ctx){
    std::cerr<<"---Resolve Symbols In "<<getFileName()<<"---"<<std::endl;
    
    auto& ESyms=getSymbolTable();
    for(uint32_t I=fisrtGlobalIndex,limi=ESyms.size();I<limi;I++){
        auto& ESym=ESyms[I];
        auto Name=ESym->getName();
        auto Shdr=Shdrs[ESym->getShndx(SymtabShndx,I)].get();

        if(ESym->isUndef()&&ESym->getSymbolBinding()!=STB_WEAK){
            // Add to undef set
            std::cerr<<"Find undef symbol "<<ESym->getName()<<", add to undef set\n";
            Ctx.UndefSymbols.push_back(Name);
        }
        if(!ESym->isUndef()){
            // which has definition
            std::cerr<<"Find definition of symbol:"<<ESym->getName()<<", add to final set\n";
            auto& SymTable=Ctx.FinalSymbolTable;
            if(SymTable.find(Name)!=SymTable.end()){
                bool previousStrong=SymTable[Name].ESym->getSymbolBinding()==STB_GLOBAL;
                bool thisStrong=ESym->getSymbolBinding()==STB_GLOBAL;
                // strong and strong, fucked
                if(previousStrong&&thisStrong){
                    std::cerr<<"Redefiniton of symbol:"<<Name<<", previous defined in "<<SymTable[Name].Obj->getFileName();
                    exit(-1);
                }
                // weak and weak, continue
                continue;
                // weak and strong, legal
            }
            SymTable[Name]={this,Shdr,ESym.get()};
        }
        // undef and weak
    }
}
