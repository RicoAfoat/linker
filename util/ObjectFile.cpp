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
    
    OF->Ehdr.reset(ELFHeader::getNewELFHeader(BIT64, buffer.data(), buffer.size()));
    
    // Get All Section Headers
    {
        auto Shoff=OF->Ehdr->getShoff();
        uint8_t* ptr=buffer.data()+Shoff;
        auto restSize=buffer.size()-Shoff;
        OF->Shdrs.emplace_back(SectionHeader::getNewSectionHeader(BIT64, ptr,restSize));
        
        auto ShdrSize=OF->Shdrs[0]->getSize();

        auto NumSections=OF->getNumSection();
        for(auto I=NumSections;I>1;I--){
            ptr+=ShdrSize;
            restSize-=ShdrSize;
            OF->Shdrs.emplace_back(SectionHeader::getNewSectionHeader(
                BIT64,
                ptr,
                restSize
            ));
        }

        // init all sections Name
        assert(OF->Shdrs[OF->getShstrndx()]->getShType()==0x3);
        auto NameSectionAddr=OF->getSectionAddr(OF->getShstrndx());
        for(auto I=0;I<NumSections;I++){
            OF->Shdrs[I]->initName(NameSectionAddr);
        }
    }
    return OF;
}
