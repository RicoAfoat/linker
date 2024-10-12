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
    auto buffer = OF->getFileStorage();
    buffer=std::vector<uint8_t>(size);
    file.read((char*)buffer.data(), size);
    file.close();
    
    OF->Ehdr.reset(ELFHeader::getNewELFHeader(BIT64, buffer.data(), buffer.size()));
    
    size_t Shoff=OF->Ehdr->loadComponentAs<size_t>("e_shoff");
    uint8_t* ptr=buffer.data()+Shoff;
    auto restSize=buffer.size()-Shoff;
    OF->Shdrs.emplace_back(SectionHeader::getNewSectionHeader(BIT64, ptr,restSize));
    
    for(auto NumSections=OF->getNumSection();NumSections>1;NumSections--){
        ptr+=Shoff;
        restSize-=Shoff;
        OF->Shdrs.emplace_back(SectionHeader::getNewSectionHeader(
            BIT64,
            ptr,
            restSize
        ));
    }
    return OF;
}
