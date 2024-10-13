#include "ObjectFile.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <iomanip>

std::vector<uint8_t>&ObjectFile::getFileStorage(){return FileStorage;}

ELFHeader& ObjectFile::getEhdr(){return *Ehdr;}

ObjectFile* ObjectFile::OpenWith(char* FileName){
    std::ifstream file(FileName, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto OF =new ObjectFile();
    auto& buffer = OF->getFileStorage();
    buffer=std::vector<uint8_t>(size);
    file.read((char*)buffer.data(), size);
    file.close();
    
    OF->Ehdr.reset(getNew<ELFHeader>(BIT64, buffer.data(), buffer.size()));
    
    // Get All Section Headers
    {
        auto Shoff=OF->Ehdr->getShoff();
        uint8_t* ptr=buffer.data()+Shoff;
        auto restSize=buffer.size()-Shoff;
        OF->Shdrs.emplace_back(getNew<SectionHeader>(BIT64, ptr,restSize));
        
        auto ShdrSize=OF->Shdrs[0]->getSize();

        auto NumSections=OF->getNumSection();
        for(auto I=NumSections;I>1;I--){
            ptr+=ShdrSize;
            restSize-=ShdrSize;
            OF->Shdrs.emplace_back(getNew<SectionHeader>(
                BIT64,
                ptr,
                restSize
            ));
        }

        // init all sections Name
        assert(OF->Shdrs[OF->getShstrndx()]->getShType()==0x3);
        auto NameSectionAddr=OF->getSectionAddr(OF->getShstrndx());
        for(auto I=0;I<NumSections;I++)
            OF->Shdrs[I]->initAttributeStringNameOffset(NameSectionAddr);
    }

    {
        // Get Symbol Table initialized
        auto Syms=OF->getShdrs([](SectionHeader* Shdr){
            return Shdr->getShType() == static_cast<uint32_t>(SH_Type_Enum::SHT_SYMTAB);
        });
        assert(Syms.size()==1&&"Currently Lets Assume it to be only one");
        
        auto& SymbolTable=OF->getSymbolTable();

        auto SymAddr=OF->getSectionAddr(Syms[0]);
        auto SymSectionSize=Syms[0]->getShSize();
        
        // Init Every Symbol Table Entry
        // First One
        auto SymEntry=getNew<ELFSym>(BIT64,SymAddr,SymSectionSize);
        SymbolTable.emplace_back(SymEntry);
        auto SymEntrySize=SymEntry->getSize();

        // Init rest of the Symbol Table
        auto SymEleSize=SymSectionSize/SymEntrySize;
        for(;SymEleSize>1;SymEleSize--){
            SymAddr=(uint8_t*)(SymAddr)+SymEntrySize;
            SymSectionSize-=SymEntrySize;
            SymbolTable.emplace_back(getNew<ELFSym>(BIT64,SymAddr,SymSectionSize));
        }

        {
            // Get .symtab corresponding .strtab 
            // std::cerr<<Syms[0]->getShLink()<<std::endl;
            auto& StrTab=OF->getShdrs()[Syms[0]->getShLink()];
            assert(StrTab->getShType()==static_cast<uint32_t>(SH_Type_Enum::SHT_STRTAB));
            
            // init all sections Name
            auto NameSectionAddr=OF->getSectionAddr(StrTab.get());
            auto NumSyms=SymbolTable.size();
            for(auto I=0;I<NumSyms;I++)
                SymbolTable[I]->initAttributeStringNameOffset(NameSectionAddr);
        }
    }
    return OF;
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
